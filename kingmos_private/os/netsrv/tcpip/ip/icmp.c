/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：ICMP
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
//#include "netif_ip_dl.h"
#include "netif_tcp_ip.h"
#include "iphlpapi_call.h"
//"local"
#include "icmp_def.h"


/***************  全局区 定义， 声明 *****************/


extern	void	TcpIn_IcmpNotify( BYTE bType, BYTE bCode, IP_HDR* pHdrIP_icmp, WORD wLenData );
extern	void	UdpIn_IcmpNotify( BYTE bType, BYTE bCode, IP_HDR* pHdrIP_icmp, WORD wLenData );


	ICMP_GLOBAL*	g_lpGlobalIcmp = NULL;

extern	DWORD	Icmp_HandleEchoReply( IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData );
static	DWORD	Icmp_HandleEcho( IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData );

/******************************************************/


BOOL	Icmp_Init( )
{
	ICMP_GLOBAL*		lpGlobalIcmp;

	//分配1个TCP的功能 和基本初始化
	lpGlobalIcmp = (ICMP_GLOBAL*)HANDLE_ALLOC( sizeof(ICMP_GLOBAL) );
	if( !lpGlobalIcmp )
	{
		return FALSE;
	}
	HANDLE_INIT( lpGlobalIcmp, sizeof(ICMP_GLOBAL) );
	List_InitHead( &lpGlobalIcmp->hListEcho );
	InitializeCriticalSection( &lpGlobalIcmp->csListEcho );
	//

	g_lpGlobalIcmp = lpGlobalIcmp;
	return TRUE;
}
void	Icmp_Deinit( )
{
}

void	Icmp_CheckProc( HANDLE hProc )
{
}


//DWORD	Icmp_TrIn( HANDLE hNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
DWORD	Icmp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pData, WORD wLenData )
{
	IP_HDR*		pHdrIP_icmp;
	IP_HDR* pHdrIP = &pDataIn->HdrIP;
	DWORD dwRetv = 0;

	//校验数据
	if( IP_CRC(pData, wLenData) )
	{
		RETAILMSG(1,(TEXT("  Icmp_TrIn: Wrong IP_CRC Len=[%d]---\r\n"), wLenData));
		goto _return;
	}

	//
	switch( *pData )
	{
	case ICMP_TYPE_DES_UNREACH:
	case ICMP_TYPE_SRC_QUENCH:
		//长度检查
		if( wLenData<(sizeof(ICMP_HDR)+sizeof(DWORD)+sizeof(IP_HDR)) )
		{
			break;
		}
		pHdrIP_icmp = (IP_HDR*)(pData+sizeof(ICMP_HDR)+sizeof(DWORD));
		wLenData -= (sizeof(ICMP_HDR)+sizeof(DWORD));
		switch( pHdrIP_icmp->bProType )
		{
		case IP_PROTYPE_TCP:
			TcpIn_IcmpNotify( *pData, pData[1], pHdrIP_icmp, wLenData );
			break;
		case IP_PROTYPE_UDP:
			UdpIn_IcmpNotify( *pData, pData[1], pHdrIP_icmp, wLenData );
			break;
		}
		break;

	//case ICMP_TYPE_ADDR_MASK_REP:
	//	break;

	case ICMP_TYPE_TIME_EXCEED:
		break;

	case ICMP_TYPE_ECHO_REP:
		//长度检查
		if( wLenData<(sizeof(ICMP_ECHO_HDR)) )
		{
			break;
		}
		Icmp_HandleEchoReply( pHdrIP, pData, wLenData );
		break;
	case ICMP_TYPE_ECHO:
		//长度检查
		if( wLenData<(sizeof(ICMP_ECHO_HDR)) || !pHdrIP->bTTL )
		{
			break;
		}
		Icmp_HandleEcho( pHdrIP, pData, wLenData );
		break;

	default :
		break;
	}

_return:
	IPDataIn_Free(pDataIn);
	return dwRetv;
}

DWORD	Icmp_HandleEcho( IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
{
	ICMP_ECHO_HDR*	pHdrIcmpEcho = (ICMP_ECHO_HDR*)pData;
	BYTE			pBufFragSet[sizeof(FRAGSETA)];
	FRAGSETA*		pFragSetA = (FRAGSETA*)pBufFragSet;
	DWORD			dwRet;
	DWORD			dwIP;
	WORD			wLenHdr;
	
	//
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = pData;
	pFragSetA->pFrag[0].nLen = wLenData;
	//
	pHdrIcmpEcho->bType = ICMP_TYPE_ECHO_REP;
	pHdrIcmpEcho->wTotalCRC = 0;
	pHdrIcmpEcho->wTotalCRC = IP_CRC(pData, wLenData);
	//
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	dwIP = pHdrIP->dwSrcIP;

	pHdrIP->dwSrcIP = pHdrIP->dwDesIP;
	pHdrIP->dwDesIP = dwIP;
	pHdrIP->wFragment = 0;
	pHdrIP->wLenTotal = wLenHdr + wLenData;
	//
	dwRet = IP_NetOut( NULL, pHdrIP, pFragSetA );
	//分析IP_NetOut的发送结果
	if( dwRet )
	{
		switch( dwRet )
		{
		case IPERR_NO_INTF:
			return E_NOTCONN;
			break;
		case IPERR_SND_NOBUF:
			return E_NOBUFS;
			break;
		default :
			break;
		}
	}

	return 0;
}






