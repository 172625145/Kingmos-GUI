/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：PPP
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
#include <gprscfg.h>
//"\inc_local"
#include "netif_dl_ras.h"
#include "netif_ip_dl.h"
//#include "iphlpapi_call.h"
//"local"
#include "ppp_def.h"
#include "lcp.h"


/***************  全局区 定义， 声明 *****************/

//for debug
#if DEBUG_PPP_LOG
extern	void	PppDbg_Open();
extern	void	PppDbg_Write( PPPDBG_ITEM* pItemDbg, LPBYTE pBuf );
extern	void	PppDbg_Close();

static	long	g_nCntRx_IP = 0;
static	long	g_nCntRx_ppp = 0;
#endif

// ppp hdlc
extern	DWORD	PPP_InByHdlc( HANDLE hIfDev, LPBYTE pBufData, WORD wLen );
extern	DWORD	PPP_OutByHdlc( PPP_DEV* pIfDev, WORD wProtocol, FRAGSETD* pFragSetD );

// lcp / pap / chap / ipcp ---up and handle function
extern	void	LCP_SendCfgReq( PPP_DEV* pIfDev );
extern	void	LCP_Up( PPP_DEV* pIfDev );
extern	void	LCP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern	void	PAP_SendCfgReq( PPP_DEV* pIfDev );
extern	void	PAP_Up( PPP_DEV* pIfDev );
extern	void	PAP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern	void	CHAP_Up( PPP_DEV* pIfDev );
extern	void	CHAP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern	void	IPCP_SendCfgReq( PPP_DEV* pIfDev );
extern	void	IPCP_Up( PPP_DEV* pIfDev );
extern	void	IPCP_Handle( PPP_DEV* pIfDev, LPBYTE pBuf, WORD wLen );

extern DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );

//---------------------------------------------------
// PPP 函数
//---------------------------------------------------
// 
#define	MAX_LCPCFG_TIMEOUT		680

PPP_GLOBAL*		g_lpGlobalPPP = NULL;

static	void	PPP_Free( );
static	void	PPP_FreeDev( PPP_DEV* pIfDev );
static	DWORD	PPP_GetMask( BYTE bIP );

static	void	PPP_SendCfg( PPP_DEV* pIfDev, BOOL fTerminate );
static	DWORD	WINAPI	PPP_ThrdCfg( LPVOID lpParameter );

//
static	void	PPP_DoUp( PPP_DEV* pIfDev );
static	void	PPP_DoStop( PPP_DEV* pIfDev );
static	void	PPP_DoClose( PPP_DEV* pIfDev );
static	void	PPP_StartIP( PPP_DEV* pIfDev );

//
static	DWORD	PPP_DlOut( HANDLE hIfDl, PARAM_DLOUT* pParamOut );
static	BOOL	PPP_Query( HANDLE hIfDev, OUT NETINFO_IPV4* pInfoNet );
static	BOOL	PPP_Set( HANDLE hIfDev, NETINFO_IPV4* pInfoNet );

/******************************************************/


//---------------------------------------------------
//PPP 
//---------------------------------------------------

// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：PPP初始化
// 引用: 
// ********************************************************************
BOOL	PPP_Init( )
{
	//
	g_lpGlobalPPP = (PPP_GLOBAL*)HANDLE_ALLOC( sizeof(PPP_GLOBAL) );
	if( !g_lpGlobalPPP )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalPPP, sizeof(PPP_GLOBAL) );
	List_InitHead( &g_lpGlobalPPP->hListIfDev );
	InitializeCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//

	return TRUE;
}

void	PPP_Deinit( )
{
}


// ********************************************************************
// 声明：
// 参数：
//	IN OUT pInfoLink---指定的Datalink层 和 RAS/TSP层 的关联信息
//	IN fOverHdlc---指定是否由RAS直接调用的
// 返回值：
//	返回TRUE
// 功能描述：添加 PPP设备，和获取相关PPP的信息
// 引用: 
// ********************************************************************
BOOL	PPP_CreateDev( PPP_LINK_RAS* pInfoLink, BOOL fOverHdlc )
{
	PPP_DEV*	pIfDev;
	HANDLE		hThrd;
	DWORD		dwThrdID;

	//
	RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: begin\r\n")));
	//分配
	pIfDev = (PPP_DEV*)HANDLE_ALLOC( sizeof(PPP_DEV) );
	if( !pIfDev )
	{
		RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: fail000 to end\r\n")));
		return FALSE;
	}
	HANDLE_INIT( pIfDev, sizeof(PPP_DEV) );
	List_InitHead( &pIfDev->hListIfDev );

	//保存 RAS信息
	pIfDev->hRasDev = pInfoLink->hRasDev;
	memcpy( &pIfDev->DialParam, pInfoLink->pDialParam, sizeof(DIALPARAM) );
	//保存 TSP信息
	//pIfDev->hTspDev = pInfoLink->hTspDev;
	//pIfDev->pFnTspOut = pInfoLink->pFnTspOut;
	if( !pInfoLink->pInfoFromTsp->dwMRU )
	{
		pIfDev->wMRU = MRU_PPPRECV;
	}
	else if( pInfoLink->pInfoFromTsp->dwMRU<MRU_PPP_MIN )
	{
		pIfDev->wMRU = MRU_PPP_MIN;
	}
	else
	{
		pIfDev->wMRU = (WORD)pInfoLink->pInfoFromTsp->dwMRU;
	}
	if( pIfDev->wMRU < MRU_PPPRECV )
	{
		pIfDev->wMaxRecv = MRU_PPPRECV;
	}
	else
	{
		pIfDev->wMaxRecv = pIfDev->wMRU;
	}
#ifdef VC386
	//pIfDev->wMRU = 512; // for debug
#endif
	pIfDev->wMaxRecv += 20;
	//保存 Over信息 和 填充信息
	if( fOverHdlc )
	{
		pInfoLink->LocalDl.pFnDlIn = PPP_InByHdlc;
	}
	else
	{
		pIfDev->hIfOver = pInfoLink->LocalDl.hIfDev;
		pIfDev->pFnOverNotify = pInfoLink->LocalDl.pFnDlNotify;		//用于 PPP操作DL时，通知PPPOES
		pIfDev->pFnOverOut = pInfoLink->LocalDl.pFnDlOut;	 //用于 PPP选择发送路径：UNIMODEM 或者 PPPOES

		pInfoLink->LocalDl.pFnDlIn = PPP_DlIn;
	}
	pInfoLink->LocalDl.hIfDev = (HANDLE)pIfDev;
	pInfoLink->LocalDl.pFnDlNotify = PPP_Notify;
	pInfoLink->LocalDl.pFnDlQuery = PPP_Query;
	pInfoLink->LocalDl.pFnDlSet = 0;

	//初始化
	if( fOverHdlc )
	{
		DWORD	dwTmp;

		dwTmp = sizeof(PPP_HDLC)+pIfDev->wMaxRecv+10;
		pIfDev->pInfoHdlc = (PPP_HDLC*)malloc( dwTmp );
		if( !pIfDev->pInfoHdlc )
		{
			PPP_FreeDev( pIfDev );
			RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: fail111 to end\r\n")));
			return FALSE;
		}
		memset( pIfDev->pInfoHdlc, 0, sizeof(PPP_HDLC) );
	}
	//pIfDev->pInfoHdlc->wFCS = 0;
	pIfDev->wStateCnn = PPP_CNN_DIALED;	//PPP状态 
	pIfDev->bRunCfg = 1;

#if DEBUG_PPP_LOG
	PppDbg_Open();
	g_nCntRx_IP = 0;
	g_nCntRx_ppp = 0;
#endif

	//
	//启动工作线程
	hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)PPP_ThrdCfg, (LPVOID)pIfDev, 0, &dwThrdID );
	if( !hThrd )
	{
#if DEBUG_PPP_LOG
		PppDbg_Close();
#endif
		PPP_FreeDev( pIfDev );
		RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: fail222 to end\r\n")));
		return FALSE;
	}
	pIfDev->hThrdCfg = hThrd;

	//添加该接口
	EnterCriticalSection( &g_lpGlobalPPP->csListIfDev );
	List_InsertTail( &g_lpGlobalPPP->hListIfDev, &pIfDev->hListIfDev );
	LeaveCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//
	RETAILMSG(DEBUG_PPP,(TEXT("  netsrv_PPP_CreateDev: succeed to end\r\n")));
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN dwEvent---指定PPP设备的事件
// 返回值：
//	返回0
// 功能描述：处理所有 PPP的事件
// 引用: 
// ********************************************************************
DWORD	PPP_Notify( HANDLE hIfDev, DWORD dwEvent )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		return 0;
	}
	//PPP层的配置会话计数 复位
	pIfDev->bCntRx_Nak_Rej = 0;
	pIfDev->bCntTx_ReCfg = 0;
	pIfDev->bCntRx_ReCfg = 0;
	//
	switch( dwEvent )
	{
	//来自   RAS
	case PROEVT_DEV_LINKED:
		//PPP_DoUp( pIfDev );
		LCP_Up( pIfDev );
		break;
	//来自   RAS 或者 PPPOES
	case PROEVT_DEV_STOP:
		PPP_DoStop( pIfDev );
		break;
	//来自   RAS 或者 PPPOES
	case PROEVT_DEV_CLOSE:
		PPP_DoClose( pIfDev );
		break;
	
	//来自   PPP内部---LCP
	case PPP_EVT_LCP_OK:
		RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_LCP_OK, RASERR_SUCCESS );
		if( pIfDev->wFlagLCP & PF_LCP_PAP )
		{
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_ap: PAP!\r\n")));
			PAP_Up( pIfDev );
		}
		else if( pIfDev->wFlagLCP & PF_LCP_CHAP )
		{
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n")));
			RETAILMSG(DEBUG_PPP,(TEXT(" ++++ netsrv_ap: CHAP!")));
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n")));
			CHAP_Up( pIfDev );
		}
		else
		{
			IPCP_Up( pIfDev );
		}
		break;
	//来自   PPP内部---PAP/CHAP
	case PPP_EVT_AP_OK:
		RasIf_NotifyEvt( pIfDev->hIfNet, RASCS_IPCP, RASERR_SUCCESS );
		IPCP_Up( pIfDev );
		break;
	//来自   PPP内部---IPCP
	case PPP_EVT_IPCP_OK:
		//事件通知
		RasIf_NotifyEvt( pIfDev->hIfNet, RASCS_IPCP_OK, RASERR_SUCCESS );
		//判断分析 IP地址和MASK
		pIfDev->dwIP_Mask = PPP_GetMask( GET_HBYTE3_L(pIfDev->dwIP_Clt) );

		if( !pIfDev->dwIP_Srv )
		{
			GprsCfg_GetPPG( GPRS_NETTYPE_CURRENT, &pIfDev->dwIP_Srv, NULL );
		}

		if( IN_LOOPBACK(pIfDev->dwIP_DNS) )
		{
			pIfDev->dwIP_DNS = pIfDev->dwIP_Srv;
		}
		if( IN_LOOPBACK(pIfDev->dwIP_DNS2) )
		{
			pIfDev->dwIP_DNS2 = pIfDev->dwIP_Srv;
		}
		if( DEBUG_PPP )
		{
			char	pAddr[32];

			RETAILMSG(DEBUG_PPP,(TEXT("\r\n"), pAddr));
			RETAILMSG(DEBUG_PPP,(TEXT(" ++++ netsrv_ap: IPCP_ok\r\n")));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Srv, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: SrvIP[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Clt, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: CltIP[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_Mask, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: CltIP_Mask[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_DNS, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: DnsIP[ %s ]\r\n"), pAddr));

			inet_ntoa_ip( (LPBYTE)&pIfDev->dwIP_DNS2, pAddr );
			RETAILMSG(DEBUG_PPP,(TEXT("\r\n  ++++ netsrv_PPP_OK: Dns2IP[ %s ]\r\n"), pAddr));

			RETAILMSG(DEBUG_PPP,(TEXT("\r\n"), pAddr));
		}

		PPP_StartIP( pIfDev );
		break;

	//来自   网络(IP)层
	case PROEVT_DL_UP:
		break;
	//来自   网络(IP)层
	case PROEVT_DL_DN:
		break;
	}
	
	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN OUT pInfoQuery---保存查询的网络信息
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：查询网络信息
// 引用: 
// ********************************************************************
BOOL	PPP_Query( HANDLE hIfDev, OUT NETINFO_IPV4* pInfoNet )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		return FALSE;
	}
	//状态检查
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return FALSE;
	}

	//查询网络信息: IP
	pInfoNet->dwIP         = pIfDev->dwIP_Clt;
	pInfoNet->dwIP_Mask    = pIfDev->dwIP_Mask;
	pInfoNet->dwIP_Gateway = pIfDev->dwIP_Srv;
	pInfoNet->dwIP_DNS     = pIfDev->dwIP_DNS;
	pInfoNet->dwIP_DNS2    = pIfDev->dwIP_DNS2;

	pInfoNet->dwEnableDhcp = FALSE;
	pInfoNet->wMTU = pIfDev->wMRU;

	return TRUE;
}

BOOL	PPP_GetMIB( HANDLE hIfDev, OUT PMIB_IFROW pIfRow )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		return FALSE;
	}
	//状态检查
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return FALSE;
	}

	return FALSE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN pInfoQuery---指定设置的网络信息
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：设置网络信息
// 引用: 
// ********************************************************************
BOOL	PPP_Set( HANDLE hIfDev, NETINFO_IPV4* pInfoNet )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;

	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		return FALSE;
	}
	//状态检查
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return FALSE;
	}

	//设置网络信息: IP
	if( pInfoNet->dwMask & NET_MASK_IP_Gateway )
	{
		pIfDev->dwIP_Srv = pInfoNet->dwIP_Gateway;
	}

	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN pParamOut---指定发送的信息
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：发送数据
// 引用: 
// ********************************************************************
DWORD	PPP_DlOut( HANDLE hIfDl, PARAM_DLOUT* pParamOut )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDl;
	FRAGSETD*	pFragSetD = pParamOut->pFragSetD;

	WORD		wProtocol;

	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		return 1;
	}
	//检查参数
	if( !pFragSetD || !pFragSetD->nCntAll || !pFragSetD->nCntUsed || !pFragSetD->dwBytes )
	{
		return ERROR_SUCCESS;
	}
	//状态检查
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return 1;
	}
	//
	pIfDev->dwFramesXmited ++;
	//得到 协议字
	if( pParamOut->dwProtoID==PROID_IP )
	{
		wProtocol = PPP_PRO_IP;
	}
	else
	{
		switch( pParamOut->dwProtoID )
		{
		case PROID_LCP:
			wProtocol = PPP_PRO_LCP;
			break;
		case PROID_PAP:
			wProtocol = PPP_PRO_PAP;
			break;
		case PROID_CHAP:
			wProtocol = PPP_PRO_CHAP;
			break;
		case PROID_IPCP:
			wProtocol = PPP_PRO_IPCP;
			break;
		default:
			return 2;
			break;
		}
		//attention here
	}
	//CheckOutPtr( __FILE__, __LINE__ );
	//分析发送的路
	if( pIfDev->pFnOverOut )
	{
		//添 协议头
		pFragSetD->pFrag[pFragSetD->nCntUsed].pData = (LPBYTE)&wProtocol;
		pFragSetD->pFrag[pFragSetD->nCntUsed].nLen  = sizeof(WORD);
		pFragSetD->nCntUsed += 1;
		pFragSetD->dwBytes += sizeof(WORD);

		//发送给 pFnOverOut
		return (*pIfDev->pFnOverOut)( pIfDev->hIfOver, pParamOut );
	}
	else
	{
		//填 协议字
		//发送给 pFnTspOut
		return PPP_OutByHdlc( pIfDev, wProtocol, pFragSetD );
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN wProtocol---指定PPP的协议字
//	IN pBuf---指定发送的PPP数据
//	IN wLen---指定发送的PPP数据长度
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：PPP内部 发送函数
// 引用: 
// ********************************************************************
#define	MAX_FRAGCNT			20
DWORD	PPP_OutBuf( PPP_DEV* pIfDev, WORD wProtocol, LPBYTE pBuf, WORD wLen )
{
	//
	pIfDev->dwFramesXmited ++;
	//
	if( pIfDev->pFnOverOut )
	{
		PARAM_DLOUT	ParamOut;
		BYTE		pBufFragSet[sizeof(FRAGSETD) + MAX_FRAGCNT*sizeof(FRAGMENT)];
		FRAGSETD*	pFragSetD = (FRAGSETD*)pBufFragSet;
		FRAGMENT*	pFrag = pFragSetD->pFrag;
		
		//添 协议分片
		pFragSetD->nCntAll = MAX_FRAGCNT;
		pFragSetD->nCntUsed = 2;
		pFragSetD->dwBytes = wLen + sizeof(WORD);
		pFrag[0].pData = pBuf;
		pFrag[0].nLen  = (LONG)wLen;
		pFrag[1].pData = (LPBYTE)&wProtocol;
		pFrag[1].nLen  = sizeof(WORD);

		memset( &ParamOut, 0, sizeof(PARAM_DLOUT) );
		ParamOut.dwProtoID = PROID_PPP;
		ParamOut.pFragSetD = pFragSetD;
		//发送给 pFnOverOut
		return (*pIfDev->pFnOverOut)( pIfDev->hIfOver, &ParamOut );
	}
	else
	{
		FRAGSETD		FragSetD;

		//填 协议字
		FragSetD.nCntAll = 1;
		FragSetD.nCntUsed = 1;
		FragSetD.dwBytes = wLen;

		FragSetD.pFrag[0].pData = pBuf;
		FragSetD.pFrag[0].nLen  = (LONG)wLen;
		//发送给 pFnTspOut
		return PPP_OutByHdlc( pIfDev, wProtocol, &FragSetD );
	}
}

//------------------------------------------------------
//功能：发送 LCP、PAP、CHAP、IPCP等 数据包，因为他们都是相同的 Header---CTL_HDR
//参数：
//pIfDev---指定 PPP的下端接口
//wProtocol---指定协议
//bCode/bAskerID/wLenTotal---指定他们的CTL_HDR的数值
//pBuf---指定要发送的BUFFER，他包括CTL_HDR和其数据
//------------------------------------------------------
void	PPP_SendPro( PPP_DEV* pIfDev, WORD wProtocol, BYTE bCode, BYTE bAskerID, WORD wLenTotal, IN OUT LPBYTE pBuf )
{
	// 初始化 Header
	pBuf[0] = bCode;
	pBuf[1] = bAskerID;
	phtonsp( pBuf+2, (LPBYTE)&wLenTotal );
	// 发送
	PPP_OutBuf( pIfDev, wProtocol, pBuf, wLenTotal );
}


// ********************************************************************
// 声明：
// 参数：
//	IN hIfDev---指定PPP设备
//	IN pFrame---指定接收的PPP的PROTOCOL数据
//	IN dwLen---指定接收的PPP的PROTOCOL数据的长度
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：接收PPP的PROTOCOL数据
// 引用: 
// ********************************************************************
DWORD	PPP_DlIn( HANDLE hIfDev, LPBYTE pFrmBuf, WORD wLen )
{
#if DEBUG_PPP_LOG
	PPPDBG_ITEM	ItemDbg;
#endif
	PPP_DEV*	pIfDev = (PPP_DEV*)hIfDev;
	LPBYTE		pFrame = pFrmBuf;
	WORD		wProtocol;

	//句柄安全检查
	if( !HANDLE_CHECK(pIfDev) )
	{
		return 1;
	}
	//状态检查
	if( pIfDev->wStateCnn==PPP_CNN_DOWN )
	{
		return 1;
	}
	//
	pIfDev->dwFramesRcved ++;

	// ProtoID
	if( *pFrame & 1 )	//说明 这已经是压缩过的ProtoID
	{
		wProtocol = TO_NBYTE1_S( *pFrame );
		pFrame += 1;
		wLen   -= 1;
	}
	else
	{
		memcpy( &wProtocol, pFrame, sizeof(WORD) );
		pFrame += 2;
		wLen   -= 2;
	}

	//for debug
#if DEBUG_PPP_LOG
	ItemDbg.wFlag = PDBGF_DIRIN;
	ItemDbg.wXXX = 0;
	ItemDbg.wProtocol = wProtocol;
	ItemDbg.wLen = wLen;
	ItemDbg.dwTick = GetTickCount();
	PppDbg_Write( &ItemDbg, pFrame );
#endif

//#if DEBUG_PPP
//	if( wProtocol==PPP_PRO_IP )
//	{
//		RETAILMSG(DEBUG_PPP,(TEXT(" \r\nPPP_DlIn_IP: g_nCntRx_IP=[%d], g_nCntRx_ppp=[%d], wLen=[%d]\r\n"), ++g_nCntRx_IP, g_nCntRx_ppp, wLen));
//	}
//	else
//	{
//		RETAILMSG(DEBUG_PPP,(TEXT(" \r\nPPP_DlIn_ppp: g_nCntRx_IP=[%d], g_nCntRx_ppp=[%d], wLen=[%d]\r\n"), g_nCntRx_IP, ++g_nCntRx_ppp, wLen));
//	}
//#endif

	// 各个协议处理
	switch( wProtocol )
	{
	case PPP_PRO_IP	:
		if( pIfDev->pFnNetIn )
		{
			(*pIfDev->pFnNetIn)( pIfDev->hIfNet, pFrame, wLen );
		}
		break;

	case PPP_PRO_LCP:
		LCP_Handle( pIfDev, pFrame, wLen );
		break;
	case PPP_PRO_PAP:
		PAP_Handle( pIfDev, pFrame, wLen );
		break;
	case PPP_PRO_CHAP:
		CHAP_Handle( pIfDev, pFrame, wLen );
		break;
	case PPP_PRO_IPCP:
		IPCP_Handle( pIfDev, pFrame, wLen );
		break;

	default:
		//XYG注释：如果该数据包不支持，直接丢掉！
		//LCP_SendPro( pIfDev, LCP_CODE_REJ_PRO, 33, wLen, pFrame );
		RETAILMSG(DEBUG_PPP,(TEXT(" \r\n PPP_DlIn: unknown=[0x%x], wLen=[%d]\r\n"), REORDER_S(wProtocol), wLen));
		break;
	}

	return 0;
}






//------------------------------------------------------
// PPP 内部工作函数..........................
//------------------------------------------------------


// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：PPP初始化
// 引用: 
// ********************************************************************
void	PPP_Free( )
{
	//通知所有的设备
	//????????????????????????????????

	//
	//
	DeleteCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//
	free( g_lpGlobalPPP );
	g_lpGlobalPPP = NULL;
}


void	PPP_FreeDev( PPP_DEV* pIfDev )
{
	//断开
	//EnterCriticalSection( &g_lpGlobalPPP->csListIfDev );
	//List_RemoveUnit( &pIfDev->hListIfDev );
	//LeaveCriticalSection( &g_lpGlobalPPP->csListIfDev );

	pIfDev->bRunCfg = 0;
	Sleep( 1 );

	//
	if( pIfDev->pInfoHdlc )
	{
		free( pIfDev->pInfoHdlc );
		pIfDev->pInfoHdlc = NULL;
	}
	if( pIfDev->hThrdCfg )
	{
		Sleep( MAX_LCPCFG_TIMEOUT );
		CloseHandle( pIfDev->hThrdCfg );
	}

	//
	free( pIfDev );
}

DWORD	PPP_GetMask( BYTE bIP )
{
	if( bIP<128 )
	{
		return ntohl_m(0xFF000000);
	}
	else if( bIP<192 )
	{
		return ntohl_m(0xFFFF0000);
	}
	else if( bIP<224 )
	{
		return ntohl_m(0xFFFFFF00);
	}
	else
	{
		return ntohl_m(0xFFFFFFFF);
	}
}

void	PPP_DoUp( PPP_DEV* pIfDev )
{
	LCP_Up( pIfDev );
}

void	PPP_DoStop( PPP_DEV* pIfDev )
{
	//通知对方
	if( pIfDev->bRunCfg )
	{
		int		i;
		BYTE	bBuf[sizeof(CTL_HDR)];//是否添加 MN，可以分析一下
	
		for( i=0; i<3; i++ )
		{
			LCP_SendPro( pIfDev, CTL_CODE_TER_REQ, 0, sizeof(CTL_HDR), bBuf );
		}
		pIfDev->wStateCnn = PPP_CNN_TERMINATE;
		
		pIfDev->bRunCfg = 0;
	}

	//PPP停用该设备接口
	pIfDev->wStateCnn = PPP_CNN_DOWN;
	if( pIfDev->hThrdCfg )
	{
		WaitForSingleObject( pIfDev->hThrdCfg, 4000 );
		CloseHandle( pIfDev->hThrdCfg );
		pIfDev->hThrdCfg = NULL;
	}

	//判断是否在IP层注册接口
	if( pIfDev->hIfNet )
	{
		//去掉 在IP层注册的接口
		(*pIfDev->pFnNetNotify)( pIfDev->hIfNet, PROEVT_DL_STOP );
	}
}

void	PPP_DoClose( PPP_DEV* pIfDev )
{
	//判断是否在IP层注册接口
	if( pIfDev->hIfNet )
	{
		//去掉 在IP层注册的接口
		(*pIfDev->pFnNetNotify)( pIfDev->hIfNet, PROEVT_DL_DN );
		pIfDev->hIfNet = NULL;
	}

#if DEBUG_PPP_LOG
	PppDbg_Close();
#endif

	//从PPP中删除当前这个接口
	EnterCriticalSection( &g_lpGlobalPPP->csListIfDev );
	List_RemoveUnit( &pIfDev->hListIfDev );
	LeaveCriticalSection( &g_lpGlobalPPP->csListIfDev );

	//
	HANDLE_FREE( pIfDev );
}

void	PPP_StartIP( PPP_DEV* pIfDev )
{
	NET_LINK_DL		InfoLink;

	//添加 IP层网络接口
	memset( &InfoLink, 0, sizeof(NET_LINK_DL) );
	InfoLink.LocalDl.hIfDev = (HANDLE)pIfDev;
	InfoLink.LocalDl.pFnDlNotify = PPP_Notify;
	InfoLink.LocalDl.pFnDlQuery = PPP_Query;
	InfoLink.LocalDl.pFnDlGetMIB = PPP_GetMIB;
	InfoLink.LocalDl.pFnDlSet = PPP_Set;
	InfoLink.LocalDl.pFnDlOut = PPP_DlOut;
	InfoLink.LocalDl.pFnDlIn = 0;
	if( !IP_CreateIf( &InfoLink ) )
	{
		//通知 RAS
		RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_Disconnected, 0 );
		return ;
	}
	//检查
	//保存
	pIfDev->hIfNet = InfoLink.LocalNet.hIfDev;
	pIfDev->pFnNetNotify = InfoLink.LocalNet.pFnNetNotify;
	pIfDev->pFnNetIn = InfoLink.LocalNet.pFnNetIn;

	//设置状态
	pIfDev->wStateCnn = PPP_CNN_IP;

	//通知
	(*pIfDev->pFnNetNotify)( pIfDev->hIfNet, PROEVT_DL_UP );

	//通知 RAS
	RasIf_NotifyEvt( pIfDev->hRasDev, RASCS_Connected, RASERR_SUCCESS );
}




//配置重发机制
DWORD	WINAPI	PPP_ThrdCfg( LPVOID lpParameter )
{
	PPP_DEV*	pIfDev = (PPP_DEV*)lpParameter;
	BOOL		fTerminate;

	//没有到IP层，或者没有DOWN
	while( 1 )
	{
		if( (!pIfDev->bRunCfg) || (pIfDev->wStateCnn==PPP_CNN_IP) || (pIfDev->wStateCnn==PPP_CNN_DOWN) )
		{
			break;
		}
		Sleep( MAX_LCPCFG_TIMEOUT );

		//
		// 要求退出：我方发送重新配置的次数过多、我方接收重新配置的次数过多、被NAK和REJ的次数过多
		if( (pIfDev->bCntTx_ReCfg>MAX_TX_RE_CFG) || (pIfDev->bCntRx_ReCfg>MAX_TX_RE_CFG) || (pIfDev->bCntRx_Nak_Rej>MAX_RX_NAK_REJ) )
		{
			RasIf_HangUp( pIfDev->hRasDev, 0, RASCS_PPP, RASERR_PPP_CFG );
			return 0;
		}
		//可以发送重新配置
		if( (GetTickCount()-pIfDev->dwTickCfg) > WAII_TX_CFG )
		{
			//可以发送Terminate, 要求重新来过
			if( pIfDev->bCntTx_Cfg>MAX_RE_TX_CFG )
			{
				pIfDev->bCntTx_Cfg = 0;
				fTerminate = TRUE;
			}
			else
			{
				fTerminate = FALSE;
			}
			PPP_SendCfg( pIfDev, fTerminate );
		}
	}

	return 0;
}

void	PPP_SendCfg( PPP_DEV* pIfDev, BOOL fTerminate )
{
	switch( pIfDev->wStateCnn )
	{
	case PPP_CNN_LCP:
		if( fTerminate )
		{
			BYTE	bBuf[sizeof(CTL_HDR)];//是否添加 MN，可以分析一下
			pIfDev->bCntTx_ReCfg ++;
			LCP_SendPro( pIfDev, CTL_CODE_TER_REQ, (++pIfDev->bAckerID), sizeof(CTL_HDR), bBuf );
			RETAILMSG(DEBUG_PPP,(TEXT("  LCP_SendPro: send [CTL_CODE_TER_REQ]\r\n")));
		}
		else if( !(pIfDev->wFlagCnn & LCP_CNN_RXACK) )
		{
			LCP_SendCfgReq( pIfDev );
		}
		break;

	case PPP_CNN_PAP:
		if( fTerminate )
		{
			BYTE	bBuf[sizeof(CTL_HDR)];//是否添加 MN，可以分析一下
			pIfDev->bCntTx_ReCfg ++;
			LCP_SendPro( pIfDev, CTL_CODE_TER_REQ, (++pIfDev->bAckerID), sizeof(CTL_HDR), bBuf );
			RETAILMSG(DEBUG_PPP,(TEXT("  LCP_SendPro: send [CTL_CODE_TER_REQ]\r\n")));
		}
		else if( !(pIfDev->wFlagCnn & AP_CNN_RXACK) )
		{
			PAP_SendCfgReq( pIfDev );
		}
		break;

	case PPP_CNN_CHAP:
		//attention
		//if( !(pIfDev->wFlagCnn & AP_CNN_RXACK) )
		//{
		//	CHAP_SendCfgReq( pIfDev );
		//}
		break;
	case PPP_CNN_IPCP:
		if( fTerminate )
		{
			BYTE	bBuf[sizeof(CTL_HDR)];//是否添加 MN，可以分析一下
			pIfDev->bCntTx_ReCfg ++;
			IPCP_SendPro( pIfDev, CTL_CODE_TER_REQ, (++pIfDev->bAckerID), sizeof(CTL_HDR), bBuf );
			RETAILMSG(DEBUG_PPP,(TEXT("  IPCP_SendPro: send [CTL_CODE_TER_REQ]\r\n")));
		}
		else if( !(pIfDev->wFlagCnn & IPCP_CNN_RXACK) )
		{
			IPCP_SendCfgReq( pIfDev );
		}
		break;
	}
}


//---------------------------------------------------
//PPP 网络断开 处理
//---------------------------------------------------
//

//网络断开的类别和处理
//1。被对方通知（收到Terminate，且不是IP层建立时）---被动断开
     //1> PPP通知 RAS APP，并请求断开连接信息
     //2> 在PPP，进行 完全断开的操作

//2。配置的次数过多，要求主动退出---配置被拒绝，PPP主动断开
     //1> PPP通知 RAS APP，并请求断开连接信息
     //2> 设置状态，对于该接口不用再接收任何数据(接口呈现：只能发送，不再接收数据)
     //3> 发送Terminate 3次通知对方
     //4> 在PPP，进行 完全断开的操作

//3。设备已经被断开（拨走、没有应答、设备关闭），通知PPP---设备断开
     //1> TSP通知 RAS APP，并请求断开连接信息
     //2> 在RAS APP，进行 断开操作---attention
     //2> 进行 完全断开的操作

//4。已经连接成功，用户要求断开---用户断开
     //0> 在RAS APP，进行 断开操作---attention
     //1> 设置状态，对于该接口不用再接收任何数据(接口呈现：只能发送，不再接收数据)
     //2> 发送Terminate 3次通知对方
     //3> 进行 完全断开的操作

//完全断开的操作
     //1> 设置状态，停止关于 该接口pIfDev的所有动作
     //2> 通知PPP，取消和释放关于 该接口pIfDev的所有动作，包括是否存在IP层的接口
     //3> 通知PPP接口层，关闭拨号设备
     //4> 关闭PPP接口层

//注意：断开的操作，只能在 RAS 进行操作

