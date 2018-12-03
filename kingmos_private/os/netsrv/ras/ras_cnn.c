/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：网络设备管理的实现---RAS
版本号：  2.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <eapisrv.h>
#include <listunit.h>
//"\inc_app"
#include <ras.h>
//"\inc_drv"
#include "tapi.h"
//"\inc_local"
#include "netif_dl_ras.h"
#include "ras_call.h"
//"local"
#include "ras_def.h"
#include "ras_dev.h"


/***************  全局区 定义， 声明 *****************/
		RAS_GLOBAL*		g_lpGlobalRas = NULL;

//
static	void	RasT_SendEvt( DWORD dwNotifierType, LPVOID notifier, DWORD dwEvt, DWORD dwErr );

static	DWORD	RasT_GetTsp( OUT TSPINFO** ppTspInfo, OUT TSPITBL** ppTspTbl, RAS_DEV* pRasDev, DWORD dwNotifierType, LPVOID notifier );

static	BOOL	RasT_CheckOp( RAS_DEV* pRasDev, DWORD dwOp );
static	DWORD	WINAPI	RasT_ThrdDiscnn( LPVOID lpParam );

static	BOOL	RasT_BeginStat( RAS_DEV* pRasDev );
static	BOOL	RasT_EnableBps( RAS_DEV* pRasDev, BOOL fEnableBps );
static	DWORD	WINAPI	RasT_ThrdStatBps( LPVOID lpParam );

/******************************************************/


//------------------------------------------------------
//功能： 初始化
//------------------------------------------------------
BOOL	Ras_Init( HWND hWndNetApp )
{
	g_lpGlobalRas = (RAS_GLOBAL*)HANDLE_ALLOC( sizeof(RAS_GLOBAL) );
	if( !g_lpGlobalRas )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalRas, sizeof(RAS_GLOBAL) );
	List_InitHead( &g_lpGlobalRas->hListRasDev );
	List_InitHead( &g_lpGlobalRas->hListTsp );
	InitializeCriticalSection( &g_lpGlobalRas->csListRasDev );
	InitializeCriticalSection( &g_lpGlobalRas->csListTsp );

	g_lpGlobalRas->hWndNetMgr = hWndNetApp;

	return TRUE;
}

void	Ras_Deinit( )
{
	//挂断所有设备

	//释放全局结构
	DeleteCriticalSection( &g_lpGlobalRas->csListRasDev );
	DeleteCriticalSection( &g_lpGlobalRas->csListTsp );
	HANDLE_FREE( g_lpGlobalRas );
}

DWORD	WINAPI	RAPI_CallBack( HANDLE hServer, DWORD dwServerCode, DWORD dwParam, LPVOID lpParam )
{
	switch( dwServerCode )
	{
	case SCC_BROADCAST_SHUTDOWN:
		//断开所有连接....
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}


// ------------------------------------------------------
// 拨号用的API函数----拨号、挂断、获取信息、枚举连接
// ------------------------------------------------------

// ********************************************************************
// 声明：
// 参数：
//	IN pDialParam-指定拨号参数
//	IN dwNotifierType-指定拨号事件的通知类型
//	IN notifier-指定拨号事件的接收者
//	OUT phRasConn-保存拨号连接的句柄
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：根据拨号条目和拨号参数，进行拨号
// 引用: 
// ********************************************************************
DWORD	WINAPI	RAPI_Dial( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn )
{
	DWORD			dwErr;
	
	DIALPARAM		stDialParam_Def;
	RAS_DEV*		pRasDev;
	TSPINFO*		pTspInfo;
	TSPITBL*		pTspTbl;
	HANDLE			hLineDev;

	RASENTRY		stRasEntry;
	PARAM_OPENDEV	ParamOpenDev;
	PPP_LINK_RAS	InfoLink;
	TSP_DLINFO		InfoDl;

	DWORD			dwTmp;
	BOOL			fSuccess;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDialParam, RASERR_NOPARAM );
		return RASERR_NOPARAM;
	}
	//参数检查
	if( pDialParam==NULL )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDialParam, RASERR_SUCCESS );
		fSuccess = TRUE;
		stDialParam_Def.dwSize = sizeof(stDialParam_Def);
		stDialParam_Def.szEntryName[0] = 0;
		stDialParam_Def.szPhoneNumber[0] = 0;
		stDialParam_Def.szUserName[0] = 0;
		stDialParam_Def.szPassword[0] = 0;
		stDialParam_Def.szDomain[0] = 0;
		if( !RAPI_GetEntryDialParams( lpszPhonebook, &stDialParam_Def, &fSuccess ) )
		{
			RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDialParam, RASERR_NOPARAM );
			return RASERR_NOPARAM;
		}
		pDialParam = &stDialParam_Def;
	}
	//{
	//	//RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOPARAM );
	//	//return RASERR_NOPARAM;
	//}
	dwErr = RASERR_SUCCESS;

//RasDial 拨号工作流程：
	
	//=< 1 >====<< 寻找RAS_DEV，并进入拨号使用状态 >>============================================
	RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_SUCCESS );
	//1。获取 RASENTRY->szDrvReg: 根据 DIALPARAMSW->szEntryName，获取 RASENTRY
	dwTmp = sizeof(RASENTRY);
	stRasEntry.szDrvReg[0] = 0;
	if( !RAPI_GetEntryProperties( lpszPhonebook, pDialParam->szEntryName, &stRasEntry, &dwTmp ) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOENTRY );
		return FALSE;
	}
	if( stRasEntry.szDrvReg[0]==0 )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOENTRY );
		return RASERR_NOENTRY;
	}
	//2。寻找 RAS_DEV: 根据 RASENTRY->szDrvReg 来搜索所有注册的拨号设备
	if( !RasD_FindDev( &pRasDev, stRasEntry.szDrvReg ) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NODEVICE );
		return RASERR_NODEVICE;
	}
	//3。判断是否可以拨号：根据 RAS_DEV 的状态判断，并设成 使用状态
	if( pRasDev->dwDevStatus!=RASDEV_IDLE )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTIDLE );
		return RASERR_NOTIDLE;
	}
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_IDLE )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTIDLE );
		return RASERR_NOTIDLE;
	}
	pRasDev->dwDevStatus = RASDEV_LOAD;
	pRasDev->dwNotifierType = dwNotifierType;
	pRasDev->notifier = notifier;
	if( pDialParam->szEntryName[0] )
	{
		strncpy( pRasDev->szEntryName, pDialParam->szEntryName, RAS_MaxEntryName );
		pRasDev->szEntryName[RAS_MaxEntryName]=0;
	}
	else
	{
		RAPI_GetEntryDef( lpszPhonebook, pRasDev->szEntryName, RAS_MaxEntryName );
	}
	ResetEvent( pRasDev->hEvtWaitCnn );
	LeaveCriticalSection( &pRasDev->csRasDev );

	*phRasConn = (HANDLE)pRasDev;
	//=< 2 >====<< 寻找或加载 TSPINFO >>========================================================
	dwErr = RasT_GetTsp( &pTspInfo, &pTspTbl, pRasDev, dwNotifierType, notifier );
	if( dwErr!=RASERR_SUCCESS )
	{
		goto EXIT_RASDIAL;
	}

	//=< 3 >====<< 调用TSP函数拨号 >>===========================================================
	//1。TSPI_CreateDev---寻找或创建 TSP设备：
		// 根据 RAS_DEV->dwEnumID（RAS_DEV被插入加载后的唯一ID号）寻找或创建，
		// 同时保存 szDrvRegPath 和 打开方式
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LOAD )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	if( pRasDev->hLineDev==NULL )
	{
		hLineDev = (*pTspTbl->TSPI_CreateDev)( pRasDev->dwEnumID, pRasDev->szDrvReg, &pRasDev->rasOpen );
		if( hLineDev==NULL )
		{
			LeaveCriticalSection( &pRasDev->csRasDev );
			RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTSPDEV );
			dwErr = RASERR_NOTSPDEV;
			goto EXIT_RASDIAL;
		}
		pRasDev->hLineDev = hLineDev;
	}
	else
	{
		hLineDev = pRasDev->hLineDev;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	//2。TSPI_OpenDev---TSP设备的 绑定、打开、配置 ：
		// hRasDev---表明该 TSP设备 已经绑定到 RAS上，可以相互通知了。
		// pszEntry 和 lpEntry---用于该设备 配置参数
	ParamOpenDev.hRasDev = (HANDLE)pRasDev;
	ParamOpenDev.pszEntry = pRasDev->szEntryName;
	ParamOpenDev.lpEntry = &stRasEntry;
	if( !(*pTspTbl->TSPI_OpenDev)(hLineDev, &ParamOpenDev) )
	{
		dwErr = RASERR_NOTSPOPEN;
		goto EXIT_RASDIAL;
	}
	//3。TSPI_lineDial---TSP设备的Init、拨号启动、进行拨号： 根据 pDialParam
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LOAD )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	pRasDev->dwDevStatus = RASDEV_DIALING;
	LeaveCriticalSection( &pRasDev->csRasDev );
	if( !(*pTspTbl->TSPI_lineDial)( hLineDev, pDialParam->szPhoneNumber ) )
	{
		dwErr = RASERR_TSP_DIAL;
		goto EXIT_RASDIAL;
	}
	RasT_SendEvt( dwNotifierType, notifier, RASCS_DeviceConnected, RASERR_SUCCESS );
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_DIALING )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_DeviceConnected, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	pRasDev->dwDevStatus = RASDEV_DIALED;
	LeaveCriticalSection( &pRasDev->csRasDev );

	//初始化统计信息
	//if( !RasT_BeginStat( pRasDev ) )
	//{
	//}

	//=< 4 >====<< 关联 拨号链路层 与 RAS层、TSP层 >>===========================================
	//1。TSPI_lineGetInfo---获取 TSP层的相关信息，以便告诉 拨号链路层：如 AC的MAC地址
	if( !(*pTspTbl->TSPI_lineGetInfo)( hLineDev, &InfoDl ) )
	{
		dwErr = RASERR_TSP_NOINFO;
		goto EXIT_RASDIAL;
	}
	//2。xxx_CreateDev---在拨号链路层 添加设备接口，并关联： PPP_CreateDev 或者 PPPoEs_CreateDev
		//RAS层：提供 拨号参数 和 RAS设备句柄
		//TSP层：提供 TSP设备句柄 和 发送函数
		//拨号链路层：创建接口，提供接收函数、
		//保存 拨号链路层的句柄 到RAS层
	memset( &InfoLink, 0, sizeof(PPP_LINK_RAS) );
	InfoLink.hRasDev = (HANDLE)pRasDev;
	InfoLink.pDialParam = pDialParam;
	//InfoLink.hTspDev = hLineDev;
	//InfoLink.pFnTspOut = pTspTbl->TSPI_lineSend;
	InfoLink.pInfoFromTsp = &InfoDl;
	//if( RASDT_GETMAIN(pRasDev->dwDevType)==RASDT_PPPoE )
	//{
	//	fSuccess = PPPoEs_CreateDev( &InfoLink, TRUE );
	//}
	//else
	{
		fSuccess = PPP_CreateDev( &InfoLink, TRUE );
	}
	//3。TSPI_lineSetRecv---将拨号链路层的相关信息 告诉TSP层
	if( fSuccess )
	{
		//告诉TSP层
		(*pTspTbl->TSPI_lineSetRecv)( hLineDev, 0, 0 );
		//将信息保存到RAS层
		pRasDev->hIfDl = InfoLink.LocalDl.hIfDev;
		pRasDev->pFnDlNotify = InfoLink.LocalDl.pFnDlNotify;
		pRasDev->pFnDlIn = InfoLink.LocalDl.pFnDlIn;
	}
	else
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_PPP, RASERR_PPP_DEV );
		dwErr = RASERR_PPP_DEV;
		goto EXIT_RASDIAL;
	}
	//4。xxx_Notify---通知 拨号链路层 (PROEVT_DEV_LINKED)：PPP_Notify 或者 PPPoEs_Notify
	RasT_SendEvt( dwNotifierType, notifier, RASCS_PPP, RASERR_SUCCESS );
	(*pRasDev->pFnDlNotify)( pRasDev->hIfDl, PROEVT_DEV_LINKED );

	//=< 5 >====<< 等待拨号链路层的完成通知 >>==================================================
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_DIALED )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_DeviceConnected, RASERR_BREAK );
		dwErr = RASERR_BREAK;
		goto EXIT_RASDIAL;
	}
	pRasDev->dwDevStatus = RASDEV_WAITCNN;
	LeaveCriticalSection( &pRasDev->csRasDev );
	//WaitForSingleObject( pRasDev->hEvtWaitCnn, 20*60*1000 );
	WaitForSingleObject( pRasDev->hEvtWaitCnn, INFINITE );

	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LINK )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		dwErr = RASERR_TIMEOUT;
		goto EXIT_RASDIAL;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	//初始化统计信息
	RETAILMSG(1,(TEXT("\r\n RAPI_Dial: dwDevStatus[0x%x]\r\n"), pRasDev->dwDevStatus));
	RasT_BeginStat( pRasDev );

EXIT_RASDIAL:
	pRasDev->wDevOp &= ~RASOP_WAITMS;	//注意!!!
	if( dwErr!=RASERR_SUCCESS )
	{
		RasIf_HangUp( (HANDLE)pRasDev, RASH_OP_DIRECT, 0, 0 );
	}

	return dwErr;
}

// ********************************************************************
// 声明：
// 参数：
//	IN hRasConn-拨号连接的句柄
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：挂断 1个拨号连接
// 引用: 
// ********************************************************************
DWORD	WINAPI	RAPI_HangUp( HANDLE hRasConn )
{
#if 0
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	return RasIf_HangUp( (HANDLE)pRasDev, RASH_OP_WAITOVER, 0, 0 );
#else
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	return RasIf_HangUp( hRasConn, RASH_OP_WAITOVER, 0, 0 );
#endif
}

// ********************************************************************
// 声明：
// 参数：
//	IN hRasConn-拨号连接的句柄
//	OUT lpRasConnInfo-拨号连接的信息
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：挂断 1个拨号连接
// 引用: 
// ********************************************************************
DWORD	WINAPI	RAPI_GetConnectInfo( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	//分析状态
	if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
	{
		return 1;
	}

	//获取信息
	//lpRasConnInfo->rasDevInfo.dwSize       = sizeof(RASDEVINFO);
	lpRasConnInfo->rasDevInfo.dwDevType    = pRasDev->dwDevType;
	lpRasConnInfo->rasDevInfo.dwDevStatus  = pRasDev->dwDevStatus;
	strcpy( lpRasConnInfo->rasDevInfo.szDrvReg, pRasDev->szDrvReg );
	//获取 RAS设备NAME
	lpRasConnInfo->rasDevInfo.szDeviceName[0] = 0;
	RAPI_LookDriver( pRasDev->szDrvReg, 0, 0, 0, lpRasConnInfo->rasDevInfo.szDeviceName );

	//恢复状态
	pRasDev->wDevOp &= ~RASOP_WAITMS;

	return 0;
}

DWORD	WINAPI	RAPI_GetConnectStatus( HANDLE hRasConn, OUT DWORD* pdwDevStatus )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}

	return 0;
}


DWORD	WINAPI	RAPI_EnableStat( HANDLE hRasConn, BOOL fEnbaleBps )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	//分析状态
	if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
	{
		return 1;
	}
	//
	RasT_EnableBps( pRasDev, fEnbaleBps );
	return 0;
}

DWORD	WINAPI	RAPI_GetLinkStatistics( HANDLE hRasConn, OUT RAS_STATS *lpStatistics )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasConn;
#ifdef RAS_BPS_1
#else
	DWORD		dwTickCur;
	DWORD		dwTickS;
#endif

	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	if( !HANDLE_CHECK(pRasDev) )
	{
		return 1;
	}
	//分析状态
	if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
	{
		return 1;
	}
	if( pRasDev->wDevOp & RASOP_NO_BPS )
	{
		return 2;
	}
	//
	lpStatistics->dwBytesXmited = pRasDev->dwBytesXmited;
	lpStatistics->dwBytesRcved = pRasDev->dwBytesRcved;
	lpStatistics->dwConnectDuration = GetTickCount()-pRasDev->dwTickCnn;
#ifdef RAS_BPS_1
	lpStatistics->dwBps = pRasDev->dwRxBps;
	lpStatistics->dwBps_Tx = pRasDev->dwTxBps;
	//lpStatistics->dwFramesXmited = ;
	//lpStatistics->dwFramesRcved = ;
	//lpStatistics->dwCrcErr = ;
#else
	//
	dwTickCur = GetTickCount();
	dwTickS = dwTickCur - pRasDev->dwTickRx_Bps;
	if( dwTickS )
	{
		lpStatistics->dwBps = pRasDev->dwBytesRx_Bps / dwTickS;
		if( dwTickS>TIMER_STATBPS )
		{
			pRasDev->dwTickRx_Bps = dwTickCur;
			pRasDev->dwBytesRx_Bps = 0;
		}
	}
	else
	{
		lpStatistics->dwBps = pRasDev->dwBytesRx_Bps;
	}
	//
	dwTickS = dwTickCur - pRasDev->dwTickTx_Bps;
	if( dwTickS )
	{
		lpStatistics->dwBps_Tx = pRasDev->dwBytesTx_Bps / dwTickS;
		if( dwTickS>TIMER_STATBPS )
		{
			pRasDev->dwTickTx_Bps = dwTickCur;
			pRasDev->dwBytesTx_Bps = 0;
		}
	}
	else
	{
		lpStatistics->dwBps_Tx = pRasDev->dwBytesTx_Bps;
	}
#endif

	return 0;
}


DWORD	WINAPI	RAPI_EnumConnections( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn )
{
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	return 0;
}

// RAS拨号辅助操作
DWORD	WINAPI	RAPI_GetStateStr( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize )
{
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//参数检查
	if( !pszStateStr || dwBufSize<2 )
	{
		return 0; //ERROR_INSUFFICIENT_BUFFER
	}
	dwBufSize -= 1;
	pszStateStr[dwBufSize] = 0;
	//
	switch( dwRasState )
	{
	case RASCS_LoadDialParam:
		strncpy( pszStateStr, TEXT("正在加载默认拨号参数..."), dwBufSize );
		break;

	case RASCS_LoadDev:
		strncpy( pszStateStr, TEXT("正在加载设备..."), dwBufSize );
		break;
	case RASCS_OpenPort:
		strncpy( pszStateStr, TEXT("正在打开设备..."), dwBufSize );
		break;
	case RASCS_PortOpened:
		strncpy( pszStateStr, TEXT("设备打开成功!"), dwBufSize );
		break;
	case RASCS_ModemReady:
		strncpy( pszStateStr, TEXT("modem正在准备..."), dwBufSize );
		break;
	case RASCS_ConnectDevice:
		strncpy( pszStateStr, TEXT("设备正在拨号..."), dwBufSize );
		break;
	case RASCS_DeviceConnected:
		strncpy( pszStateStr, TEXT("设备拨号成功!"), dwBufSize );
		break;

	case RASCS_PPP:
		strncpy( pszStateStr, TEXT("正在进行网络配置..."), dwBufSize );
		break;
	case RASCS_LCP_OK:
		strncpy( pszStateStr, TEXT("网络配置成功！"), dwBufSize );
		break;
	case RASCS_Authenticate:
		strncpy( pszStateStr, TEXT("正在验证用户和密码..."), dwBufSize );
		break;
	case RASCS_AuthAck:
		strncpy( pszStateStr, TEXT("验证用户和密码成功!"), dwBufSize );
		break;
	case RASCS_IPCP:
		strncpy( pszStateStr, TEXT("正在向网络注册计算机..."), dwBufSize );
		break;
	case RASCS_IPCP_OK:
		strncpy( pszStateStr, TEXT("向网络注册计算机成功！"), dwBufSize );
		break;
	case RASCS_Connected:
		strncpy( pszStateStr, TEXT("连接成功!"), dwBufSize );
		break;
	case RASCS_Disconnected:
		strncpy( pszStateStr, TEXT("连接断开!"), dwBufSize );
		break;
	default:
		{
			TCHAR	pszTemp[50];
			sprintf( pszTemp, "Unknown state[%d]", dwRasState );
			strncpy( pszStateStr, pszTemp, dwBufSize );
		}
		break;
	}
	return 0;
}

DWORD	WINAPI	RAPI_GetErrStr( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize )
{
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalRas) )
	{
		return FALSE;
	}
	//参数检查
	if( !pszErrStr || dwBufSize<2 )
	{
		return 0; //ERROR_INSUFFICIENT_BUFFER
	}
	dwBufSize -= 1;
	pszErrStr[dwBufSize] = 0;
	//
	switch( dwRasErr )
	{
	case RASERR_NOPARAM:
		strncpy( pszErrStr, TEXT("没有拨号参数，或者拨号参数有错误!"), dwBufSize );
		break;
	case RASERR_BREAK:
		strncpy( pszErrStr, TEXT("设备拨号操作被打断!"), dwBufSize );
		break;
	case RASERR_NOSIGNAL:
		strncpy( pszErrStr, TEXT("设备没有信号!"), dwBufSize );
		break;
	case RASERR_TIMEOUT:
		strncpy( pszErrStr, TEXT("设备拨号超时!"), dwBufSize );
		break;
	case RASERR_NOTIDLE:
		strncpy( pszErrStr, TEXT("设备不是处在空闲状态!"), dwBufSize );
		break;

	case RASERR_NOENTRY:
		strncpy( pszErrStr, TEXT("在检查拨号连接的属性时出错!"), dwBufSize );
		break;
	case RASERR_NODEVICE:
		strncpy( pszErrStr, TEXT("没有找到modem设备!"), dwBufSize );
		break;
	case RASERR_NOTSP:
		strncpy( pszErrStr, TEXT("加载TSP时失败!"), dwBufSize );
		break;
	case RASERR_NOTSPDEV:
		strncpy( pszErrStr, TEXT("加载TSP设备时失败!"), dwBufSize );
		break;
	case RASERR_NOTSPOPEN:
		strncpy( pszErrStr, TEXT("绑定TSP设备时失败!"), dwBufSize );
		break;
		
	case RASERR_TSP_OPEN:
		strncpy( pszErrStr, TEXT("打开设备时失败!"), dwBufSize );
		break;
	case RASERR_TSP_CFG:
		strncpy( pszErrStr, TEXT("配置设备时失败!"), dwBufSize );
		break;
	case RASERR_TSP_ENTRY:
		strncpy( pszErrStr, TEXT("获取拨号条目时失败!"), dwBufSize );
		break;
	case RASERR_TSP_START:
		strncpy( pszErrStr, TEXT("启动MODEM时失败!"), dwBufSize );
		break;
	case RASERR_TSP_TESTMDM:
		strncpy( pszErrStr, TEXT("测试MODEM时失败!"), dwBufSize );
		break;
	case RASERR_TSP_SETMDM:
		strncpy( pszErrStr, TEXT("设置MODEM时失败!"), dwBufSize );
		break;
		
	case RASERR_TSP_INIT:
		strncpy( pszErrStr, TEXT("启动设备拨号时失败!"), dwBufSize );
		break;
	case RASERR_TSP_DIAL:
		strncpy( pszErrStr, TEXT("设备拨号失败!"), dwBufSize );
		break;
	case RASERR_TSP_DIAL_BUSY:
		strncpy( pszErrStr, TEXT("设备拨号失败，因为占线!"), dwBufSize );
		break;
	case RASERR_TSP_DIAL_NOCAR:
		strncpy( pszErrStr, TEXT("设备拨号失败，因为没有载波信号!"), dwBufSize );
		break;
		
	case RASERR_TSP_NOINFO:
		strncpy( pszErrStr, TEXT("在TSP DLL中获取相关信息时失败!"), dwBufSize );
		break;
		
	case RASERR_PPP_DEV:
		strncpy( pszErrStr, TEXT("在拨号链路层添加设备时失败!"), dwBufSize );
		break;
	case RASERR_PPP_CFG:
		strncpy( pszErrStr, TEXT("多次配置请求都失败!"), dwBufSize );
		break;
		
	case RASERR_PPP_PASSWORD:
		strncpy( pszErrStr, TEXT("密码或用户密码错误!"), dwBufSize );
		break;
	case RASERR_PPP_TICKOFF:
		strncpy( pszErrStr, TEXT("被对方终止而断开!"), dwBufSize );
		break;
	default:
		{
			TCHAR	pszTemp[50];
			sprintf( pszTemp, "Unknown error[%d]", dwRasErr );
			strncpy( pszErrStr, pszTemp, dwBufSize );
		}
		break;
	}
	return 0;
}


// ------------------------------------------------------
// RAS提供给TSP/PPP等内部使用的接口函数
// ------------------------------------------------------

// ********************************************************************
// 声明：
// 参数：
//	IN hRasDev-拨号RAS设备的句柄
//	IN dwRasCnnStatus-通知状态
//	IN dwErr-通知错误
// 返回值：
//	无
// 功能描述：向RAS通知事件，由RAS向RASAPP通知事件
// 引用: 
// ********************************************************************
void	RasIf_NotifyEvt( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;
	BOOL		fResend = FALSE;

	//向RAS通知事件的规则：
	//TSP设备：在打开绑定后，负责通知所有过程的事件
	//PPP设备：在CreateDev后，负责通知所有过程的事件

	//请 检查句柄
	if( !HANDLE_CHECK(pRasDev) )
	{
		return ;
	}
	//
	if( pRasDev->dwNotifierType==DIALNOTIFY_WND )
	{
		if( pRasDev->notifier )
		{
			//SendMessage( (HWND)pRasDev->notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
			PostMessage( (HWND)pRasDev->notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
		}
		else
		{
			fResend = TRUE;
		}
	}
	//else
	//{
	//}

	//
	if( fResend )
	{
		//PostMessage( g_lpGlobalRas->hWndNetMgr, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
	}
	//
	if( dwRasCnnStatus==RASCS_Connected && dwErr==RASERR_SUCCESS )
	{
		//进入新状态
		EnterCriticalSection( &pRasDev->csRasDev );
		if( pRasDev->dwDevStatus==RASDEV_WAITCNN )
		{
			pRasDev->dwDevStatus = RASDEV_LINK;
			//通知连接成功
			if( pRasDev->hEvtWaitCnn )
			{
				SetEvent( pRasDev->hEvtWaitCnn );
			}
		}
		LeaveCriticalSection( &pRasDev->csRasDev );
	}
	//
	else if( dwErr==RASERR_NOSIGNAL )
	{
		RasIf_HangUp( hRasDev, 0, 0, 0 );
	}
	else
	{
		//
		//if( dwRasCnnStatus==RASCS_Disconnected )
		//{
		//	int kk=0;
		//}
	}
}

#define DEBUG_RasIf_DownData 0
DWORD	RasIf_DownData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;

	DEBUGMSG( DEBUG_RasIf_DownData, ( "RasIf_DownData entry.\r\n" ) );
	//CheckOutPtr( __FILE__, __LINE__ );
	//请 检查句柄
	if( !HANDLE_CHECK(pRasDev) || ((pRasDev->dwDevStatus!=RASDEV_LINK) && (pRasDev->dwDevStatus!=RASDEV_WAITCNN)) )
	{
		DEBUGMSG( DEBUG_RasIf_DownData, ( "RasIf_DownData leave01.\r\n" ) );
		return 1;
	}
	//统计
	pRasDev->dwBytesXmited += dwLen;
	pRasDev->dwBytesTx_Bps += dwLen;
	//
	//CheckOutPtr( __FILE__, __LINE__ );
	if( pRasDev->lpTspInfo->pTspTbl->TSPI_lineSend )
	{
		(*pRasDev->lpTspInfo->pTspTbl->TSPI_lineSend)( pRasDev->hLineDev, pBuf, dwLen );
	}
	//CheckOutPtr( __FILE__, __LINE__ );
	DEBUGMSG( DEBUG_RasIf_DownData, ( "RasIf_DownData leave02.\r\n" ) );
	return 0;
}

DWORD	RasIf_UpData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;

	//请 检查句柄
	if( !HANDLE_CHECK(pRasDev) || ((pRasDev->dwDevStatus!=RASDEV_LINK) && (pRasDev->dwDevStatus!=RASDEV_WAITCNN)) )
	{
		return 1;
	}
	//统计
	pRasDev->dwBytesRcved += dwLen;
	pRasDev->dwBytesRx_Bps += dwLen;
	//InterlockedExchangeAdd( &pRasDev->dwBytesRx_Bps, dwLen );
	//InterlockedExchangeAdd( &pRasDev->dwBytesRcved, dwLen );
	//
	if( pRasDev->pFnDlIn )
	{
		(*pRasDev->pFnDlIn)( pRasDev->hIfDl, pBuf, (WORD)dwLen );
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN hRasDev-拨号RAS设备的句柄
//	IN dwOperation-挂断 的操作(同步、异步、拔走设备)
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：挂断 1个拨号连接
// 引用: 
// ********************************************************************
DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)hRasDev;

	//保证设备存在
	RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Enter h=[0x%x], opt=[0x%x]\r\n"), pRasDev, dwOperation));
	if( !HANDLE_CHECK(pRasDev) )
	{
		RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Leave Failed--1\r\n")));
		return 0;
	}
	//操作检查
	if( !RasT_CheckOp(pRasDev, dwOperation) )
	{
		RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Leave Failed--2\r\n")));
		return 0;
	}
	//
	if( dwRasCnnStatus && (pRasDev->dwNotifierType==DIALNOTIFY_WND) && pRasDev->notifier )
	{
		PostMessage( (HWND)pRasDev->notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
	}
	//启动工作
	if( dwOperation & RASH_OP_DIRECT )
	{
		//直接 工作
		RasT_ThrdDiscnn( (LPVOID)pRasDev );
	}
	else
	{
		HANDLE	hThrd;
		DWORD	dwThrdID;

		//线程 启动
		hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RasT_ThrdDiscnn, (LPVOID)pRasDev, 0, &dwThrdID );
		if( !hThrd )
		{
			RETAILMSG(1,(TEXT("\r\n RasIf_HangUp: Leave Failed--3\r\n")));
			return 1;
		}
		//线程 等待
		if( dwOperation & RASH_OP_WAITOVER )
		{
			WaitForSingleObject( hThrd, INFINITE );
		}
		CloseHandle( hThrd );
	}

	RETAILMSG(1,(TEXT("  RasIf_HangUp: Leave ok\r\n")));
	return 0;
}


// ------------------------------------------------------
// 拨号用的内部函数
// ------------------------------------------------------


DWORD	RasT_GetTsp( OUT TSPINFO** ppTspInfo, OUT TSPITBL** ppTspTbl, RAS_DEV* pRasDev, DWORD dwNotifierType, LPVOID notifier )
{
	TCHAR			pszTspDll[MAX_PATH];
	DWORD			dwTmp;

	//1。获取 pszTspDll：根据该RAS_DEV->szDrvRegPath
	dwTmp = MAX_PATH;
	if( !RAPI_LookDriver( pRasDev->szDrvReg, 0, 0, pszTspDll, 0 ) )
	{
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTSP );
		return RASERR_NOTSP;
	}
	//2。寻找 TSPINFO：根据该 pszTspDll，并设成 引用状态
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->dwDevStatus!=RASDEV_LOAD )
	{
		LeaveCriticalSection( &pRasDev->csRasDev );
		RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_BREAK );
		return RASERR_BREAK;
	}
	//ASSERT( pRasDev->lpTspInfo==NULL && pRasDev->hLineDev==NULL );//Attention
	if( pRasDev->lpTspInfo==NULL )
	{
		if( !RasT_FindTsp( ppTspInfo, pszTspDll, TRUE ) )
		{
			//3。加载 TSPINFO：根据该 pszTspDll，并设成 引用状态
			if( !RasT_LoadTsp( ppTspInfo, pszTspDll, TRUE ) )
			{
				pRasDev->dwDevStatus = RASDEV_IDLE;
				LeaveCriticalSection( &pRasDev->csRasDev );
				RasT_SendEvt( dwNotifierType, notifier, RASCS_LoadDev, RASERR_NOTSP );
				return RASERR_NOTSP;
			}
			pRasDev->lpTspInfo = *ppTspInfo;
		}
	}
	else
	{
		*ppTspInfo = pRasDev->lpTspInfo;
		EnterCriticalSection( &g_lpGlobalRas->csListTsp );
		(*ppTspInfo)->nRefCnt ++;
		LeaveCriticalSection( &g_lpGlobalRas->csListTsp );
	}
	*ppTspTbl = (*ppTspInfo)->pTspTbl;
	LeaveCriticalSection( &pRasDev->csRasDev );

	return RASERR_SUCCESS;
}



// ********************************************************************
// 声明：
// 参数：
//	IN dwNotifierType-指定拨号事件的通知类型
//	IN notifier-指定拨号事件的接收者
//	IN dwRasCnnStatus-通知状态
//	IN dwErr-通知错误
// 返回值：
//	无
// 功能描述：RAS向RASAPP通知事件
// 引用: 
// ********************************************************************
void	RasT_SendEvt( DWORD dwNotifierType, LPVOID notifier, DWORD dwRasCnnStatus, DWORD dwErr )
{
	if( dwNotifierType==DIALNOTIFY_WND )
	{
		//SendMessage( (HWND)notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
		PostMessage( (HWND)notifier, WM_RASDIALEVENT, (WPARAM)dwRasCnnStatus, (LPARAM)dwErr );
	}
}

// ********************************************************************
// 声明：
// 参数：
//	IN pRasDev-拨号RAS设备的句柄
//	IN dwOp-挂断 的操作(同步、异步、拔走设备)
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：挂断 1个拨号连接前的检查，以便确定是否继续操作
// 引用: 
// ********************************************************************
BOOL	RasT_CheckOp( RAS_DEV* pRasDev, DWORD dwOp )
{
	RAS_DEV*	pRasDev_Find;
	BOOL		fContinue = FALSE;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	pHeader = &g_lpGlobalRas->hListRasDev;
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pRasDev_Find = LIST_CONTAINER( pUnit, RAS_DEV, hListRasDev);
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pRasDev_Find) )
		{
			break;
		}
		if( (pRasDev->wFlag & RASDF_EXIT) || (pRasDev->wDevOp & RASOP_FREE) )
		{
			continue;
		}

		//如果找到，说明还没有被删除掉；如果找不到，说明已经被删除了
		if( pRasDev==pRasDev_Find )
		{
			if( pRasDev->wDevOp & RASOP_HANGUP )
			{
				//如果已经进行 断开或者删除的操作 的话，请检查操作状态，
				//如果需要的话，设置成删除；并决定是否继续操作
				if( dwOp & RASH_OP_FREEDEV )
				{
					pRasDev->wDevOp |= RASOP_FREE;
				}
			}
			else
			{
				//如果还没有进行 断开或者删除的操作 的话，请设置
				//EnterCriticalSection( &pRasDev->csRasDev );
				pRasDev->dwDevStatus = RASDEV_DISCNN;
				//LeaveCriticalSection( &pRasDev->csRasDev );
				pRasDev->wDevOp = RASOP_HANGUP;
				if( dwOp & RASH_OP_FREEDEV )
				{
					pRasDev->wDevOp |= RASOP_FREE;
				}
				fContinue = TRUE;
			}
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );

	return fContinue;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpParam-拨号RAS设备的句柄
// 返回值：
//	0
// 功能描述：挂断 1个拨号连接 的 设备操作
// 引用: 
// ********************************************************************
DWORD	WINAPI	RasT_ThrdDiscnn( LPVOID lpParam )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)lpParam;
	TSPITBL*	pTspTbl = NULL;

	//
	RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: Enter h=[0x%x]\r\n"), pRasDev));
	if( !HANDLE_CHECK(pRasDev) )
	{
		RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: leave Failed--1\r\n")));
		return 0;
	}
	//
	//1. 状态的设置
	Sleep(1);
	EnterCriticalSection( &pRasDev->csRasDev );
	pRasDev->dwDevStatus = RASDEV_DISCNN;
	LeaveCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->hEvtWaitCnn )
	{
		SetEvent( pRasDev->hEvtWaitCnn );
		SetEvent( pRasDev->hEvtWaitCnn );
	}
	Sleep(1);

	//2. 通知 PPP层停用该设备接口
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->hIfDl && pRasDev->pFnDlNotify )
	{
		(*pRasDev->pFnDlNotify)( pRasDev->hIfDl, PROEVT_DEV_STOP );
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//3.调用 TSP层停用该设备
	EnterCriticalSection( &pRasDev->csRasDev );
	if( (pRasDev->lpTspInfo) && (pTspTbl=pRasDev->lpTspInfo->pTspTbl) && (pRasDev->hLineDev) )
	{
		(*pTspTbl->TSPI_lineNotify)( pRasDev->hLineDev, PROEVT_DEV_STOP );
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//4. 通知 PPP层关闭该设备接口
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pRasDev->hIfDl && pRasDev->pFnDlNotify )
	{
		(*pRasDev->pFnDlNotify)( pRasDev->hIfDl, PROEVT_DEV_CLOSE );
		pRasDev->hIfDl = NULL;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//5. TSP关闭该设备接口
	EnterCriticalSection( &pRasDev->csRasDev );
	if( pTspTbl && (pRasDev->hLineDev) )
	{
		// 1 TSP关闭该设备接口
		(*pTspTbl->TSPI_lineNotify)( pRasDev->hLineDev, PROEVT_DEV_CLOSE );
		pRasDev->hLineDev = NULL;
		// 2 Check TSP Dll wether free
		RasT_FreeTsp( pRasDev->lpTspInfo );	//
		pRasDev->lpTspInfo = NULL;
	}
	LeaveCriticalSection( &pRasDev->csRasDev );
	Sleep(1);

	//6. 设置该设备的操作状态
	EnterCriticalSection( &g_lpGlobalRas->csListRasDev );
	pRasDev->wDevOp &= ~RASOP_HANGUP;
	if( pRasDev->wDevOp & RASOP_FREE )
	{
		List_RemoveUnit( &pRasDev->hListRasDev );
	}
	else
	{
		pRasDev->dwDevStatus = RASDEV_IDLE;
	}
	LeaveCriticalSection( &g_lpGlobalRas->csListRasDev );
	Sleep(1);

	//7. 删除该设备
	if( pRasDev->wDevOp & RASOP_FREE )
	{
		RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: Free device!\r\n")));
		RasT_FreeDev( pRasDev );
	}

	RETAILMSG(1,(TEXT("\r\n RasT_ThrdDiscnn: leave ok\r\n")));
	return 0;
}


BOOL	RasT_BeginStat( RAS_DEV* pRasDev )
{
	//
	pRasDev->dwTickCnn = GetTickCount();
	pRasDev->dwBytesXmited = 0;
	pRasDev->dwBytesRcved = 0;
	//
#ifdef RAS_BPS_1
	pRasDev->dwRxBps = 0;
	pRasDev->dwBytesRx_Bps = 0;
	pRasDev->dwTxBps = 0;
	pRasDev->dwBytesTx_Bps = 0;
	return RasT_EnableBps( pRasDev, TRUE );
#else
	pRasDev->dwBytesRx_Bps = 0;
	pRasDev->dwTickRx_Bps = GetTickCount();

	pRasDev->dwBytesTx_Bps = 0;
	pRasDev->dwTickTx_Bps = GetTickCount();
	return TRUE;
#endif
}

BOOL	RasT_EnableBps( RAS_DEV* pRasDev, BOOL fEnableBps )
{
#ifdef RAS_BPS_1
	//初始化
	pRasDev->dwRxBps = 0;
	pRasDev->dwTxBps = 0;
	//
	if( fEnableBps )
	{
		DWORD	dwThrdID;

		pRasDev->dwBytesRx_Bps = 0;
		pRasDev->dwBytesTx_Bps = 0;
		pRasDev->dwTickRx_Bps = GetTickCount();
		pRasDev->dwTickTx_Bps = GetTickCount();
		pRasDev->wDevOp &= ~RASOP_NO_BPS;
		Sleep( 1 );
		//线程 启动
		if( pRasDev->hThrdBps )
		{
			return TRUE;
		}
		//线程 启动
		pRasDev->hThrdBps = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)RasT_ThrdStatBps, (LPVOID)pRasDev, 0, &dwThrdID );
		if( !pRasDev->hThrdBps )
		{
			return FALSE;
		}
	}
	else
	{
		if( pRasDev->hThrdBps )
		{
			pRasDev->wDevOp |= RASOP_NO_BPS;
			SetEvent( pRasDev->hEvtBps );
		}
	}
#endif
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpParam-拨号RAS设备的句柄
// 返回值：
//	0
// 功能描述：挂断 1个拨号连接 的 设备操作
// 引用: 
// ********************************************************************
#ifdef RAS_BPS_1
DWORD	WINAPI	RasT_ThrdStatBps( LPVOID lpParam )
{
	RAS_DEV*	pRasDev = (RAS_DEV*)lpParam;
	DWORD		dwTickCur;

	while( 1 )
	{
		//等待
		WaitForSingleObject( pRasDev->hEvtBps, TIMER_STATBPS );
		if( !HANDLE_CHECK(pRasDev) )
		{
			break;
		}
		if( (pRasDev->wDevOp & (RASOP_HANGUP|RASOP_FREE|RASOP_NO_BPS)) || (pRasDev->dwDevStatus!=RASDEV_LINK) )
		{
			break;
		}
		dwTickCur = GetTickCount();
		//统计---速度---接收
		//pRasDev->dwRxBps = InterlockedExchange( &pRasDev->dwBytesRx_Bps, 0 )/(dwTickCur-InterlockedExchange( &pRasDev->dwTickRx_Bps, dwTickCur ));
		pRasDev->dwRxBps = (pRasDev->dwBytesRx_Bps*1000)/(dwTickCur-pRasDev->dwTickRx_Bps);
		pRasDev->dwBytesRx_Bps = 0;
		pRasDev->dwTickRx_Bps = dwTickCur;
		//统计---速度---发送
		//pRasDev->dwRxBps = InterlockedExchange( &pRasDev->dwBytesRx_Bps, 0 )/(dwTickCur-InterlockedExchange( &pRasDev->dwTickRx_Bps, dwTickCur ));
		pRasDev->dwTxBps = (pRasDev->dwBytesTx_Bps*1000)/(dwTickCur-pRasDev->dwTickTx_Bps);
		pRasDev->dwBytesTx_Bps = 0;
		pRasDev->dwTickTx_Bps = dwTickCur;
	}
	//
	if( pRasDev->hThrdBps )
	{
		CloseHandle( pRasDev->hThrdBps );
		pRasDev->hThrdBps = NULL;
	}

	return 0;
}
#endif

