/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TSP 标准 操作
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：

  肖远钢 2004-05-11注释：
    1。如果要定义成dll形式，请 define _USRDLL
    2。如果要定义成modem形式，请 define RDT_MDM
    3。如果要定义成pppoed形式，请 define RDT_PPPOE
    4。如果在PC机上，请 define VC386

******************************************************/
#ifdef	_USRDLL
#include <windows.h>
#include <stdio.h>
#else	//_USRDLL
#include <ewindows.h>
#endif	//_USRDLL
#include <listunit.h>
//"\inc_app"
#include <ras.h>
//"\inc_drv"
#include "tapi.h"
//"local"
#define RDT_MDM
#include "tsp_fun.h"
#include "uni_dev.h"
#include "uni_mdm.h"


/***************  全局区 定义， 声明 *****************/

//全局变量
static	TSP_GLOBAL*	g_lpGlobalUni = NULL;
static	TSPITBL		g_hFunTblTsp;


//内部函数
static	BOOL	TSPI_Init( HINSTANCE hInst );
static	void	TSPI_Deinit( );

static	void	lineDev_Find( DWORD dwEnumID, OUT LINE_DEV** ppLineDev );


/******************************************************/


#ifdef	_USRDLL

// ********************************************************************
// 声明：
// 参数：
//	IN hInstDLL-实例
//	IN dwReason-理由
//	IN lpvReserved-保留
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：DLL入口函数
// 引用: 
// ********************************************************************
BOOL	APIENTRY	DllMain( HANDLE hInstDLL, DWORD dwReason, LPVOID lpvReserved )
{
    switch (dwReason) 
    {
    case DLL_PROCESS_ATTACH:
		return TSPI_Init( hInstDLL );
		break;
	case DLL_PROCESS_DETACH:
		TSPI_Deinit( );
		break;
    default:
        break;
    }
    return TRUE;
}

#endif	//_USRDLL

// ********************************************************************
// 声明：
// 参数：
//	OUT ppTspiTbl-保存TSP的函数表
//	IN hInst-实例
//	IN pNotifyRas-SP到RAS的回调通知函数
// 返回值：
//	无
// 功能描述：获取 TSP的函数表，把回调函数 告诉TSP
// 引用: 
// ********************************************************************
UNIMODEM_API	void	TSPI_GetTable( OUT TSPITBL** ppTspiTbl, HINSTANCE hInst, FNRASNOTIFY pNotifyRas )
{
	TSPI_Init( hInst );

	//g_lpGlobalUni->pFnNotifyRas = pNotifyRas;
	*ppTspiTbl = &g_hFunTblTsp;
}

// ********************************************************************
// 声明：
// 参数：
//	IN dwEnumID---指定的设备ID
//	IN pszDrvReg-设备驱动的注册表位置
//	IN lpRasOpenDev-设备的打开方式
// 返回值：
//	成功，返回句柄
// 功能描述：获取 TSP的函数表，把回调函数 告诉TSP
// 引用: 
// ********************************************************************
UNIMODEM_API	HANDLE	TSPI_CreateDev( DWORD dwEnumID, LPTSTR pszDrvReg, RASDEVOPEN* lpRasOpenDev )
{
	LINE_DEV*	pLineDev;
	DWORD		dwDevType;

	//寻找 TSP设备
	lineDev_Find( dwEnumID, &pLineDev );
	if( pLineDev )
	{
		return (HANDLE)pLineDev;
	}
	//确定 TSP设备的类型 跟 TSP DLL的类型 是相符合的。
	if( !lineDev_CheckType( pszDrvReg, &dwDevType ) )
	{
		return NULL;
	}
	//分配 TSP设备
	pLineDev = (LINE_DEV*)HANDLE_ALLOC( sizeof(LINE_DEV) );
	if( !pLineDev )
	{
		return NULL;
	}
	HANDLE_INIT( pLineDev, sizeof(LINE_DEV) );
	InitializeCriticalSection( &pLineDev->csDev );
	List_InitHead( &pLineDev->hListDev );

	//初始化
	pLineDev->dwEnumID = dwEnumID;
	memcpy( &pLineDev->rasOpen, lpRasOpenDev, sizeof(RASDEVOPEN) );
	strcpy( pLineDev->szDrvReg, pszDrvReg );

	pLineDev->dwDevType = dwDevType;
	pLineDev->dwDevState = MDM_IDLE;

	if( !lineDev_Create(pLineDev) )
	{
		lineDev_Free( pLineDev );
		return NULL;
	}

	//添加 RAS设备
	EnterCriticalSection( &g_lpGlobalUni->csListDev );
	List_InsertTail( &g_lpGlobalUni->hListDev, &pLineDev->hListDev );
	LeaveCriticalSection( &g_lpGlobalUni->csListDev );
	
	return (HANDLE)pLineDev;
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN pParamOpen-设备的打开方式
// 返回值：
//	成功，返回句柄
// 功能描述：绑定、打开 和 配置设备
// 引用: 
// ********************************************************************
UNIMODEM_API	BOOL	TSPI_OpenDev( HANDLE hLineDev, PARAM_OPENDEV* pParamOpen )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pLineDev) )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_NOTIDLE );
		return FALSE;
	}
	//判断设备状态、绑定、保存拨号条目（用于配置设备等）、提示打开设备、判断打开方式
	EnterCriticalSection( &pLineDev->csDev );
	//判断设备状态
	if( pLineDev->dwDevState!=MDM_IDLE )
	{
		RASEVT_SEND( RASCS_OpenPort, RASERR_NOTIDLE );
		LeaveCriticalSection( &pLineDev->csDev );
		return FALSE;
	}
	//绑定、保存拨号条目（用于配置设备等）
	pLineDev->hRasDev = pParamOpen->hRasDev;
	strcpy( pLineDev->szEntryName, pParamOpen->pszEntry );

	//提示打开设备
	RASEVT_SEND( RASCS_OpenPort, RASERR_SUCCESS );

	//判断打开方式和准备
	if( pLineDev->rasOpen.fOpenName )
	{
		if( pLineDev->hPort && pLineDev->hPort!=INVALID_HANDLE_VALUE )
		{
			CloseHandle( pLineDev->hPort );
			pLineDev->hPort = NULL;
		}
	}
	else
	{
		pLineDev->hPort = pLineDev->rasOpen.hDev;
	}

	LeaveCriticalSection( &pLineDev->csDev );

	//打开 和 配置设备
	return lineDev_Open( pLineDev );
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	OUT pInfoDl-保存TSP设备提供的链路层信息
// 返回值：
//	成功，返回TRUE
// 功能描述：获取TSP设备提供的链路层信息
// 引用: 
// ********************************************************************
UNIMODEM_API	BOOL	TSPI_lineGetInfo( HANDLE hLineDev, OUT TSP_DLINFO* pInfoDl )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pLineDev) )
	{
		return FALSE;
	}
	//
	if( !lineDev_GetInfo( pLineDev, pInfoDl ) )
	{
		RASEVT_SEND( RASCS_DeviceConnected, RASERR_TSP_NOINFO );
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN pDialParam-设备的拨号参数
// 返回值：
//	成功，返回TRUE
// 功能描述：设备拨号
// 引用: 
// ********************************************************************
UNIMODEM_API	BOOL	TSPI_lineDial( HANDLE hLineDev, LPCSTR pszLocalNumber )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pLineDev) )
	{
		return FALSE;
	}

	return lineDev_Dial( pLineDev, pszLocalNumber );
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN hIfDl---TSP设备的数据的 处理者句柄
//	IN pFnIn---TSP设备的数据的 处理者函数
// 返回值：
//	成功，返回TRUE
// 功能描述：设置 TSP设备的数据的 处理者，并切换到MDM_DATA
// 引用: 
// ********************************************************************
UNIMODEM_API	void	TSPI_lineSetRecv( HANDLE hLineDev, HANDLE hIfDl, FNPROIN pFnIn )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pLineDev) )
	{
		return ;
	}
	//lineDev_SetRecv( pLineDev, hIfDl, pFnIn );
	//将modem切换到  数据状态
	lineDev_NewState( pLineDev, MDM_DATA );
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN pBuf---要发送的数据
//	IN dwLen---要发送的数据的长度
// 返回值：
//	成功，返回TRUE
// 功能描述：TSP设备的数据 发送函数
// 引用: 
// ********************************************************************
UNIMODEM_API	DWORD	TSPI_lineSend( HANDLE hLineDev, LPBYTE pBuf, DWORD dwLen )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pLineDev) )
	{
		return MR_FAILURE;
	}

	return lineDev_Send( pLineDev, pBuf, dwLen );
}


// ********************************************************************
// 声明：
// 参数：
//	IN hLineDev---指定的TSP设备
//	IN dwEvent---TSP设备的事件
// 返回值：
//	返回0
// 功能描述：RAS 通过该函数 通知TSP设备的操作
// 引用: 
// ********************************************************************
UNIMODEM_API	DWORD	TSPI_lineNotify( HANDLE hLineDev, DWORD dwEvent )
{
	LINE_DEV*	pLineDev = (LINE_DEV*)hLineDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pLineDev) )
	{
		return 0;
	}
	if( dwEvent==PROEVT_DEV_STOP )
	{
		// 0 停用该设备,  
		lineDev_Stop0( pLineDev );

		// 1 停用该设备,  
		lineDev_NewState( pLineDev, MDM_DOWN );
		pLineDev->dwDevState = MDM_DOWN;
		Sleep( 1 );
		pLineDev->dwDevState = MDM_DOWN;
		//2 LINE_DEV去掉所有与PPP相关的信息
		//pLineDev->pFnDlIn = NULL;
		
		//2 stop 设备 
		lineDev_Stop( pLineDev );
	}
	else if( dwEvent==PROEVT_DEV_CLOSE )
	{
		//释放当前设备
		EnterCriticalSection( &g_lpGlobalUni->csListDev );
		List_RemoveUnit( &pLineDev->hListDev );
		LeaveCriticalSection( &g_lpGlobalUni->csListDev );

		lineDev_Free( pLineDev );
	}

	return 0;
}





//--------------------------------------------------------------------------------------------
// TSP 内部函数..........................
//--------------------------------------------------------------------------------------------




// ********************************************************************
// 声明：
// 参数：
//	IN dwEnumID---指定的设备ID
//	OUT ppLineDev-保存查找到的设备
// 返回值：
//	无
// 功能描述：查找指定设备ID的设备
// 引用: 
// ********************************************************************
void	lineDev_Find( DWORD dwEnumID, OUT LINE_DEV** ppLineDev )
{
	LINE_DEV*	pLineDev = NULL;
	BOOL		fFind = FALSE;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	pHeader = &g_lpGlobalUni->hListDev;
	EnterCriticalSection( &g_lpGlobalUni->csListDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pLineDev = LIST_CONTAINER( pUnit, LINE_DEV, hListDev );
		pUnit = pUnit->pNext;    // advance to next 

		if( pLineDev->dwEnumID==dwEnumID )
		{
			fFind = TRUE;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalUni->csListDev );

	if( fFind )
	{
		*ppLineDev = pLineDev;
	}
	else
	{
		*ppLineDev = 0;
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
//	IN dwDevState---指定的TSP设备的新状态
// 返回值：
//	成功TRUE
// 功能描述：设置 TSP设备的新状态，只要原状态不是MDM_DOWN，否则返回TRUE
// 引用: 
// ********************************************************************
BOOL	lineDev_NewState( LINE_DEV* pLineDev, DWORD dwDevState )
{
	BOOL	fSuccess;

	EnterCriticalSection( &pLineDev->csDev );
	if( pLineDev->dwDevState!=MDM_DOWN )
	{
		fSuccess = TRUE;
		pLineDev->dwDevState = dwDevState;
	}
	else
	{
		fSuccess = FALSE;
		if( pLineDev->hEvtCmd )
		{
			SetEvent( pLineDev->hEvtCmd );
			Sleep( 2 );
		}
	}
	LeaveCriticalSection( &pLineDev->csDev );
	RETAILMSG(1,(TEXT("\r\n\r\n lineDev_NewState: dwDevState=[0x%x]\r\n\r\n"), pLineDev->dwDevState));

	return fSuccess;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pLineDev---指定的TSP设备
// 返回值：
//	成功TRUE
// 功能描述：判断 TSP设备的状态，是不是MDM_DOWN
// 引用: 
// ********************************************************************
BOOL	lineDev_IsDown( LINE_DEV* pLineDev )
{
	BOOL	fDown;
	
	if( !HANDLE_CHECK(pLineDev) )
	{
		return TRUE;
	}

	EnterCriticalSection( &pLineDev->csDev );
	if( pLineDev->dwDevState!=MDM_DOWN )
	{
		fDown = FALSE;
	}
	else
	{
		fDown = TRUE;
	}
	LeaveCriticalSection( &pLineDev->csDev );

	return fDown;
}




// ********************************************************************
// 声明：
// 参数：
//	IN hInstDLL-实例
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：TSP DLL的初始化
// 引用: 
// ********************************************************************
BOOL	TSPI_Init( HINSTANCE hInst )
{
	//测试 g_hFunTblTsp
	if( g_lpGlobalUni && g_hFunTblTsp.TSPI_CreateDev==TSPI_CreateDev && g_hFunTblTsp.TSPI_lineDial==TSPI_lineDial )
	{
		return TRUE;
	}
	//
	g_lpGlobalUni = (TSP_GLOBAL*)HANDLE_ALLOC( sizeof(TSP_GLOBAL) );
	if( !g_lpGlobalUni )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalUni, sizeof(TSP_GLOBAL) );
	List_InitHead( &g_lpGlobalUni->hListDev );
	InitializeCriticalSection( &g_lpGlobalUni->csListDev );
	//
	g_lpGlobalUni->hInst = (HINSTANCE)hInst;
	g_hFunTblTsp.TSPI_CreateDev = TSPI_CreateDev;
	g_hFunTblTsp.TSPI_OpenDev = TSPI_OpenDev;
	//g_hFunTblTsp.TSPI_CloseDev = TSPI_CloseDev;
	g_hFunTblTsp.TSPI_lineNotify = TSPI_lineNotify;

	g_hFunTblTsp.TSPI_lineDial = TSPI_lineDial;
	g_hFunTblTsp.TSPI_lineGetInfo = TSPI_lineGetInfo;
	g_hFunTblTsp.TSPI_lineSetRecv = TSPI_lineSetRecv;
	g_hFunTblTsp.TSPI_lineSend = TSPI_lineSend;

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN hInstDLL-实例
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：TSP DLL的释放
// 引用: 
// ********************************************************************
void	TSPI_Deinit( )
{
	LINE_DEV*	pLineDev = NULL;
	BOOL		fFind = FALSE;
	PLIST_UNIT	pHeader;
	PLIST_UNIT	pUnit;

	//释放所有设备
	pHeader = &g_lpGlobalUni->hListDev;
	EnterCriticalSection( &g_lpGlobalUni->csListDev );
	pUnit = pHeader->pNext;
	while( pUnit!=pHeader )
	{
		pLineDev = LIST_CONTAINER( pUnit, LINE_DEV, hListDev );
		pUnit = pUnit->pNext;    // advance to next 

		if( HANDLE_CHECK(pLineDev) )
		{
			List_RemoveUnit( &pLineDev->hListDev );
			lineDev_Free( pLineDev );
		}
	}
	LeaveCriticalSection( &g_lpGlobalUni->csListDev );

	//释放TSPI的所有信息
	DeleteCriticalSection( &g_lpGlobalUni->csListDev );
	free( g_lpGlobalUni );
	g_lpGlobalUni = NULL;
}

