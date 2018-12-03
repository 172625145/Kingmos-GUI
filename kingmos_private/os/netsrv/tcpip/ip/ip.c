/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：IP
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
#include <iphlpapi.h>
//"\inc_local"
#include "proto_ip_tcp.h"
#include "netif_ip_dl.h"
#include "netif_tcp_ip.h"
#include "iphlpapi_call.h"
//"local"
#include "ip_def.h"


/***************  全局区 定义， 声明 *****************/

//
extern	BOOL	IPFrag_Init( );
extern	void	IPFrag_Rx( IP_INTF* pNetIntf, IP_HDR* pHdrIP );
extern	DWORD	IPFrag_Tx( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData );

//
extern	BOOL	IPData_Start( IP_INTF* pNetIntf );
extern	BOOL	IPData_Out( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData );
extern	BOOL	IPData_In( IP_INTF* pNetIntf, IP_HDR* pHdrIP, WORD wLenData );


//
IP_GLOBAL*		g_lpGlobalIP = NULL;

//
		void	IP_Deinit( );

static	void	IP_DeleteIf( IP_INTF* pNetIntf );
static	BOOL	IP_FindIntf( IP_INTF** ppNetIntf, DWORD dwSrcIP );
static	BOOL	IP_IsSend( IP_INTF* pNetIntf, IP_HDR* pHdrIP );

//
static	DWORD	IP_Notify( HANDLE hNetIntf, DWORD dwEvent );
static	DWORD	IP_NetIn( HANDLE hNetIntf, LPBYTE pBufData, WORD wLenData );


/******************************************************/



// ---------------------------------------------------
// 
// IP层提供的接口函数信息
// 
// ---------------------------------------------------

BOOL	IP_Init( )
{
	g_lpGlobalIP = (IP_GLOBAL*)HANDLE_ALLOC( sizeof(IP_GLOBAL) );
	if( !g_lpGlobalIP )
	{
		return FALSE;
	}
	HANDLE_INIT( g_lpGlobalIP, sizeof(IP_GLOBAL) );
	List_InitHead( &g_lpGlobalIP->hListIntf );
	List_InitHead( &g_lpGlobalIP->hListFgNode );
	InitializeCriticalSection( &g_lpGlobalIP->csListIntf );
	InitializeCriticalSection( &g_lpGlobalIP->csListFgNode );
	//
	if( !IPFrag_Init() )
	{
		IP_Deinit( );
		return FALSE;
	}
	return TRUE;
}

void	IP_Deinit( )
{
}

// ********************************************************************
// 声明：
// 参数：
//	IN OUT pInfoLink---指定的IP层 和 Datalink层 的关联信息
//	IN fByRas---指定是否由RAS直接调用的
// 返回值：
//	返回TRUE
// 功能描述：添加 IP设备，和获取相关IP的信息
// 引用: 
// ********************************************************************
BOOL	IP_CreateIf( NET_LINK_DL* pInfoLink )
{
	IP_INTF*			pNetIntf;
	NETINFO_IPV4		InfoNet;

	//参数检查
	if( !pInfoLink ||
		!pInfoLink->LocalDl.hIfDev || !pInfoLink->LocalDl.pFnDlNotify || 
		!pInfoLink->LocalDl.pFnDlSet || !pInfoLink->LocalDl.pFnDlOut ||
		!pInfoLink->LocalDl.pFnDlQuery || !pInfoLink->LocalDl.pFnDlGetMIB
	  )
	{
		return FALSE;
	}
	//分配下端设备接口
	pNetIntf = (IP_INTF*)HANDLE_ALLOC( sizeof(IP_INTF) );
	if( !pNetIntf )
	{
		return FALSE;
	}
	HANDLE_INIT( pNetIntf, sizeof(IP_INTF) );
	List_InitHead( &pNetIntf->hListIntf );
	List_InitHead( &pNetIntf->hListDataIn );
	List_InitHead( &pNetIntf->hListDataOut );
	InitializeCriticalSection( &pNetIntf->csListDataIn );
	InitializeCriticalSection( &pNetIntf->csListDataOut );
	pNetIntf->hEvtDataIn = CreateEvent( NULL, FALSE, FALSE, NULL );
	pNetIntf->hEvtDataOut = CreateEvent( NULL, FALSE, FALSE, NULL );

	//保存 DataLink层信息：
	pNetIntf->hIfDl = pInfoLink->LocalDl.hIfDev;
	pNetIntf->pFnDlNotify = pInfoLink->LocalDl.pFnDlNotify;
	pNetIntf->pFnDlQuery = pInfoLink->LocalDl.pFnDlQuery;
	pNetIntf->pFnDlGetMIB = pInfoLink->LocalDl.pFnDlGetMIB;
	pNetIntf->pFnDlSet = pInfoLink->LocalDl.pFnDlSet;
	pNetIntf->pFnDlOut = pInfoLink->LocalDl.pFnDlOut;

	//填充 IP层的相关信息
	pInfoLink->LocalNet.hIfDev = (HANDLE)pNetIntf;
	pInfoLink->LocalNet.pFnNetNotify = IP_Notify;
	pInfoLink->LocalNet.pFnNetIn = IP_NetIn;

	//查询 DataLink层网络信息
	if( !(*pNetIntf->pFnDlQuery)( pNetIntf->hIfDl, &InfoNet ) )
	{
		IP_DeleteIf( pNetIntf );
		return FALSE;
	}
	pNetIntf->dwIP       = InfoNet.dwIP     ;
	pNetIntf->dwIP_Mask  = InfoNet.dwIP_Mask;
	pNetIntf->dwIP_Gateway  = InfoNet.dwIP_Gateway;
	pNetIntf->dwIP_DNS   = InfoNet.dwIP_DNS ;
	pNetIntf->dwIP_DNS2  = InfoNet.dwIP_DNS2;
	pNetIntf->wMTU       = InfoNet.wMTU     ;
	pNetIntf->bTTL       = TTL_DEFAULT ;
	if( InfoNet.dwEnableDhcp )
	{
		pNetIntf->wFlag |= IPIFF_DHCP;
	}

	//启动 数据收发功能
	if( !IPData_Start( pNetIntf ) )
	{
		IP_DeleteIf( pNetIntf );
		return FALSE;
	}

//	//是否启动 DHCP工作
//	if( pNetIntf->wFlag & IPIFF_DHCP )
//	{
//		if( !Dhcp_Start(pNetIntf->dwIndex) )
//		{
//			IP_DeleteIf( pNetIntf );
//			return FALSE;
//		}
//	}

	//添加该接口
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pNetIntf->dwIndex = ++g_lpGlobalIP->dwAuto_Index;
	List_InsertTail( &g_lpGlobalIP->hListIntf, &pNetIntf->hListIntf );
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	return TRUE;
}


DWORD	IP_Notify( HANDLE hNetIntf, DWORD dwEvent )
{
	IP_INTF*	pNetIntf = (IP_INTF*)hNetIntf;

	//句柄安全检查
	if( !HANDLE_CHECK(pNetIntf) )
	{
		return 0;
	}
	//
	switch( dwEvent )
	{
	case PROEVT_DL_STOP:
		pNetIntf->wFlag |= IPIFF_EXIT;
		break;

	case PROEVT_DL_DN:
		IP_DeleteIf( pNetIntf );
		break;
	}

	return 0;
}

BOOL	IP_Query( DWORD dwSrcIP, NETINFO_IPV4* pInfoNet )
{
	IP_INTF*		pNetIntf;

	if( IP_FindIntf( &pNetIntf, dwSrcIP ) )
	{
		//pInfoNet->dwSize = sizeof(NETINFO_IPV4);

		pInfoNet->dwIP = dwSrcIP;
		pInfoNet->dwIP_Mask = pNetIntf->dwIP_Mask;
		pInfoNet->dwIP_Gateway = pNetIntf->dwIP_Gateway;
		pInfoNet->dwIP_DNS  = pNetIntf->dwIP_DNS;
		pInfoNet->wMTU      = pNetIntf->wMTU;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


DWORD	IP_NetIn( HANDLE hNetIntf, LPBYTE pBufData, WORD wLenData )
{
	IP_INTF*	pNetIntf = (IP_INTF*)hNetIntf;
	IP_HDR*		pHdrIP = (IP_HDR*)pBufData;
	WORD		wLenHdr;
	WORD		wTmp;

	//检查句柄
	if( HANDLE_F_FAIL(pNetIntf, IPIFF_EXIT) )
	{
		return 0;
	}
	if( wLenData<sizeof(IP_HDR) )
	{
		return 0;
	}
	//IPv4 版本
	if( (pHdrIP->bVerHLen & IPV_MASK)!=IPV4 )
	{
		return 0;
	}
	//检查 IP数据的Checksum
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	if( wLenHdr>wLenData )
	{
		return 0;
	}
	if( IP_CRC(pBufData, wLenHdr) )
	{
		return 0;
	}
	//
#ifndef BIG_END
	pHdrIP->wLenTotal = ntohs( pHdrIP->wLenTotal );
	pHdrIP->wXID = ntohs( pHdrIP->wXID );
	pHdrIP->wFragment = ntohs( pHdrIP->wFragment );
#endif
	//检查 IP数据的长度信息
	if( pHdrIP->wLenTotal > wLenData )
	{
		return 0;
	}
	//wLenData = pHdrIP->wLenTotal;//xyg: attention here

	//
	//检查 目的IP地址
	//
	if( pHdrIP->dwDesIP!=pNetIntf->dwIP )
	{
		//是不是网络广播
		if( !INADDR_TEST_BC(pHdrIP->dwDesIP) )
		{
			//是不是支持转发功能
			if( !(pNetIntf->wFlag & IPIFF_ROUTE) || (pHdrIP->bTTL==0) )
			{
				return 0;
			}
			else
			{
				//转发执行---提交到发送队列中
				pHdrIP->bTTL -= 1;
				wTmp = pHdrIP->wLenTotal-wLenHdr;
				IPData_Out( pNetIntf, pHdrIP, (LPBYTE)pHdrIP+wLenHdr, wTmp );
				return 0;
			}
		}
	}

	//检查 是否分片
	if( ((pHdrIP->wFragment & IPF_MF)==0) && ((pHdrIP->wFragment & IPF_OFFSET)==0) )
	{
		//最后1个分片，并编号为0---单片数据包，则 上传处理
		//提交到接收队列中
		wTmp = pHdrIP->wLenTotal-wLenHdr;
		IPData_In( pNetIntf, pHdrIP, wTmp );
	}
	else
	{
		//分片组装
		IPFrag_Rx( pNetIntf, pHdrIP );
	}

	return 0;
}

#define DEBUG_IP_NetOut 1
IPRET	IP_NetOut( HANDLE hNetIntf, IP_HDR* pHdrIP, FRAGSETA* pFragSetA )
{
	DWORD			dwRet;
	IP_INTF*		pNetIntf;
	LPBYTE			pAlloc;
	LPBYTE			pData;
	WORD			wLenHdr;
	WORD			wLenData;

	//参数检查
	if( (pFragSetA->nCnt==0) || (pFragSetA->nCnt>10) )
	{
		DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_FRAGA_NUM.\r\n" ) );
		return IPERR_FRAGA_NUM;		//Frag的个数太多
	}
	//选接口
	if( hNetIntf )
	{
		pNetIntf = (IP_INTF*)hNetIntf;
		//检查句柄
		if( HANDLE_F_FAIL(pNetIntf, IPIFF_EXIT) )
		{
			DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_NO_INTF0.\r\n" ) );
			return IPERR_NO_INTF;		//没有找到IP源接口
		}
	}
	else
	{
		if( !IP_FindIntf( &pNetIntf, pHdrIP->dwSrcIP ) )
		{
			DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_NO_INTF1.\r\n" ) );
			return IPERR_NO_INTF;		//没有找到IP源接口
		}
	}
	//判断是不是发送
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wLenData = pHdrIP->wLenTotal - wLenHdr;
	if( pHdrIP->wLenTotal > pNetIntf->wMTU )
	{
		if( pHdrIP->wFragment & IPF_DF )
		{
			DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_FRAG_DF.\r\n" ) );
			return IPERR_FRAG_DF;//分片过大，又不让分片
		}
		if( wLenData>IPF_MAXFRAG )
		{
			DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_SND_LARGE.\r\n" ) );
			return IPERR_SND_LARGE;	//要发送的数据超过分片能力
		}
	}
	//
	//准备数据--- pData(copy) / wLenHdr / wLenData 
	//
	if( pFragSetA->nCnt==1 )
	{
		pAlloc = NULL;
		pData = pFragSetA->pFrag[0].pData;
	}
	else
	{
		pAlloc = (LPBYTE)malloc( wLenData );
		if( !pAlloc )
		{
			DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_SND_NOBUF0.\r\n" ) );
			return IPERR_SND_NOBUF;
		}
		if( !FgA_CopyToBuf(pFragSetA, pAlloc) )
		{
			free( pAlloc );
			DEBUGMSG( DEBUG_IP_NetOut, ( "IP_NetOut: IPERR_SND_NOBUF1.\r\n" ) );
			return IPERR_SND_NOBUF;
		}
		pData = pAlloc;
	}

	//
	//填充 IP信息
	//
	pHdrIP->wXID         = htons(pNetIntf->wXID++);
	//下面的IP信息，由上层指定
	//pHdrIP->bVerHLen   = ;
	//pHdrIP->bTOS       = ;	//由上层指定
	//pHdrIP->wFragment  = 0;
	pHdrIP->bTTL       = pNetIntf->bTTL;
	//pHdrIP->bProType   = ;
	//pHdrIP->dwSrcIP    = ;
	//pHdrIP->dwDesIP    = ;

	//判断是否要分片
	if( pHdrIP->wLenTotal <= pNetIntf->wMTU )
	{
		dwRet = IPERR_SUCCESS;	//返回成功值
		//是否发送出去
		if( IP_IsSend( pNetIntf, pHdrIP ) )
		{
			//提交到发送队列中
			if( !IPData_Out( pNetIntf, pHdrIP, pData, wLenData ) )
			{
				dwRet = IPERR_SND_NOBUF;
			}
		}
	}
	else
	{
		//发送分片
		dwRet = IPFrag_Tx( pNetIntf, pHdrIP, pData, wLenData );
	}
	
	//释放临时
	if( pAlloc )
	{
		free( pAlloc );	
	}
	return dwRet;
}

#define	SIZE_LOCALIPCNT		50
DWORD	IP_NetOut2( DWORD dwOption, IP_HDR* pHdrIP, FRAGSETA* pFragSetA )
{
	DWORD		dwRet;

	//
	if( dwOption & IPTXOPT_INTF_BEST )
	{
		//获取适当的本机IP地址
		if( !IP_GetBestAddr( &pHdrIP->dwSrcIP, pHdrIP->dwDesIP, TRUE ) )
		{
			return IPERR_NO_INTF;
		}
		dwOption |= IPTXOPT_INTF_SPEC;
	}
	if( dwOption & IPTXOPT_INTF_SPEC )
	{
		return IP_NetOut( NULL, pHdrIP, pFragSetA );
	}

	//
	if( dwOption & IPTXOPT_INTF_MULT )
	{
		BYTE				pBufIPList[(SIZE_LOCALIPCNT+1)*sizeof(DWORD)];
		DWORD*				pdwIP = (DWORD*)pBufIPList;
		DWORD				dwSize = SIZE_LOCALIPCNT;
		DWORD				i;

		//请枚举本地的 所有IP接口的信息
		if( !(dwSize=IPHAPI_GetLocalIPList( pdwIP, SIZE_LOCALIPCNT )) )
		{
			return IPERR_NO_INTF;
		}
		dwRet = IPERR_NO_INTF;
		for( i=0; i<dwSize; i++ )
		{
			pHdrIP->dwSrcIP = pdwIP[i];
			if( IP_NetOut( NULL, pHdrIP, pFragSetA )==IPERR_SUCCESS )
			{
				dwRet = IPERR_SUCCESS;
			}
		}
		return dwRet;
	}

	return IPERR_SND_OPTION;
}



// ---------------------------------------------------
// 
// IP层提供的 IP_Help函数
// 
// ---------------------------------------------------

BOOL	IP_IsUp( )
{
	//检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		return FALSE;
	}
	return TRUE;
}


//检查所有的 IP_INTF，只要这个IP是有效的
BOOL	IP_IsValidAddr( DWORD dwSrcIP )
{
	IP_INTF*		pNetIntf;

	if( IP_FindIntf( &pNetIntf, dwSrcIP ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


BOOL	IP_GetBestAddr( DWORD* pdwSrcIP, DWORD dwDesIP, BOOL fGetAny )
{
	IP_INTF*		pNetIntf;

	if( IP_FindSubnet( &pNetIntf, dwDesIP, fGetAny ) )
	{
		*pdwSrcIP = pNetIntf->dwIP;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

HANDLE	IP_GetIntfByIndex( DWORD dwIndex )
{
	HANDLE			hRetIntf;
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//网络检查
	if( HANDLE_F_FAIL(g_lpGlobalIP, GIPF_EXIT) )
	{
		return 0;
	}
	//准备
	hRetIntf = NULL;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	//
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//
		if( HANDLE_F_FAIL(pNetIntf_Find, IPIFF_EXIT) )
		{
			continue;
		}
		if( pNetIntf_Find->dwIndex==dwIndex )
		{
			hRetIntf = (HANDLE)pNetIntf_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	return hRetIntf;
}





/////////////////////////////////////////////////////////////////////
// ---------------------------------------------------
// 
// IP层提供的内部辅助函数
// 
// ---------------------------------------------------
/////////////////////////////////////////////////////////////////////


void	IP_DeleteIf( IP_INTF* pNetIntf )
{
	pNetIntf->wFlag |= IPIFF_EXIT;
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	List_RemoveUnit( &pNetIntf->hListIntf );
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );
	Sleep( 1 );
	//
	HANDLE_FREE( pNetIntf );
}

BOOL	IP_FindIntf( IP_INTF** ppNetIntf, DWORD dwSrcIP )
{
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppNetIntf = NULL;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}
		//匹配参数
		if( pNetIntf_Find->dwIP==dwSrcIP )
		{
			//如果是
			*ppNetIntf = pNetIntf_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	return (*ppNetIntf)?TRUE:FALSE;
}

BOOL	IP_FindSubnet( IP_INTF** ppNetIntf, DWORD dwDesIP, BOOL fGetAny )
{
	IP_INTF*		pNetIntf_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppNetIntf = NULL;
	pUnitHeader = &g_lpGlobalIP->hListIntf;
	EnterCriticalSection( &g_lpGlobalIP->csListIntf );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pNetIntf_Find = LIST_CONTAINER( pUnit, IP_INTF, hListIntf );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pNetIntf_Find) )
		{
			break;
		}
		if( pNetIntf_Find->wFlag & IPIFF_EXIT )
		{
			continue;
		}
		//
		if( fGetAny )
		{
			*ppNetIntf = pNetIntf_Find;//Default
		}
		//匹配参数
		if( (pNetIntf_Find->dwIP & pNetIntf_Find->dwIP_Mask)==(dwDesIP & pNetIntf_Find->dwIP_Mask) )
		{
			//如果是
			*ppNetIntf = pNetIntf_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListIntf );

	return (*ppNetIntf)?TRUE:FALSE;
}


BOOL	IP_IsSend( IP_INTF* pNetIntf, IP_HDR* pHdrIP )
{
	IP_INTF*	pNetIntf_Find;
	WORD		wTmp;

	//看看是否发送给本机上的不同 IP SOCKET对
	if( IP_FindIntf( &pNetIntf_Find, pHdrIP->dwDesIP ) )
	{
		wTmp = pHdrIP->wLenTotal-((pHdrIP->bVerHLen & IPL_MASK)<<2);
		//只 发送给本机
		IPData_In( pNetIntf_Find, pHdrIP, wTmp );
		return FALSE;
	}
	//是不是广播
	else if( INADDR_TEST_BC(pHdrIP->dwDesIP) )
	{
		wTmp = pHdrIP->wLenTotal-((pHdrIP->bVerHLen & IPL_MASK)<<2);
		//同时也 发送给本机
		IPData_In( pNetIntf, pHdrIP, wTmp );
	}

	return TRUE;
}

