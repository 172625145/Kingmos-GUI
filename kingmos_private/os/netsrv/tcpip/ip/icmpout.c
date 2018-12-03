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

/******************************************************/

#if 0
DWORD	Icmp_TrOut_Echo( DWORD dwDesIP, LPVOID lpReqData, WORD wReqSize )
{
	LPBYTE				pBufEcho;
	ICMP_ECHO_HDR*		pHdrEcho;
	IP_HDR*				pHdrIP;

	BYTE				pBufFragSet[sizeof(FRAGSETA)];
	FRAGSETA*			pFragSetA = (FRAGSETA*)pBufFragSet;
	DWORD				dwRet;

	DWORD				dwSize;
	WORD				wLenTotal;
	DWORD				wLenEcho;

	//
	if( wReqSize>8192 )
	{
		return 1;
	}
	if( !lpReqData )
	{
		wReqSize = 0;
	}
	dwSize = wReqSize;
	//统计要发送的长度
	wLenTotal = sizeof(IP_HDR) + sizeof(ICMP_ECHO_HDR) + wReqSize;
	wLenEcho = wLenTotal;
	//
	pBufEcho = (LPBYTE)malloc( wLenEcho );
	if( !pBufEcho )
	{
		return 2;
	}
	memset( pBufEcho, 0, wLenEcho );
	pItemEcho->pBufEcho = pBufEcho;

	pItemEcho->dwDesIP = dwDesIP;
	pHdrIP = (IP_HDR*)pBufEcho;
	pHdrEcho = (ICMP_ECHO_HDR*)(pBufEcho+sizeof(IP_HDR));
	//
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrEcho;
	pFragSetA->pFrag[0].nLen = sizeof(ICMP_ECHO_HDR) + wReqSize;
	//
	if( wReqSize )
	{
		memcpy( (LPBYTE)pHdrEcho+sizeof(ICMP_ECHO_HDR), lpReqData, wReqSize );
	}
	//
	pHdrEcho->bType = ICMP_TYPE_ECHO;
	pHdrEcho->bCode = 0;
	pHdrEcho->wXID = pItemEcho->wXID;
	pHdrEcho->wSeqNum = ++pItemEcho->wSeqNum;
	pHdrEcho->wTotalCRC = 0;
	pHdrEcho->wTotalCRC = IP_CRC((LPBYTE)pHdrEcho, (WORD)(sizeof(ICMP_ECHO_HDR) + wReqSize) );
	//
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_ICMP;
	pHdrIP->dwDesIP = dwDesIP;
	//pHdrIP->wFragment = 0;
	pHdrIP->wLenTotal = wLenTotal;
	dwRet = IP_NetOut2( IPTXOPT_INTF_MULT, pHdrIP, pFragSetA );
	if( !dwRet )
	{
		return ERROR_FUNCTION_FAILED;
	}
	//等待结果
}
#endif

DWORD	Icmp_TrOut_Unreach( BYTE bCode, DWORD dwDesIP, DWORD dwSrcIP, LPBYTE pData, WORD wLenData )
{
	BYTE				pBufIcmpTmp[sizeof(IP_HDR)+sizeof(ICMP_UNREACH_HDR)+sizeof(IP_HDR)+sizeof(UDP_HDR)+80];
	ICMP_UNREACH_HDR*	pHdrUnreach = (ICMP_UNREACH_HDR*)pBufIcmpTmp;
	IP_HDR*				pHdrIP;

	BYTE				pBufFragSet[sizeof(FRAGSETA)+sizeof(FRAGMENT)];
	FRAGSETA*			pFragSetA = (FRAGSETA*)pBufFragSet;
	DWORD				dwRet;

	WORD				wLenTotal;

	//
	if( wLenData>(sizeof(IP_HDR)+sizeof(UDP_HDR)+80) )
	{
		return 1;
	}
	pHdrIP = (IP_HDR*)pBufIcmpTmp;
	pHdrUnreach = (ICMP_UNREACH_HDR*)(pBufIcmpTmp+sizeof(IP_HDR));
	//
	wLenTotal = sizeof(ICMP_UNREACH_HDR)+wLenData;
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrUnreach;
	pFragSetA->pFrag[0].nLen = wLenTotal;
	memset( pBufIcmpTmp, 0, sizeof(pBufIcmpTmp) );
	//
	pHdrUnreach->bType = ICMP_TYPE_DES_UNREACH;
	pHdrUnreach->bCode = bCode;
	pHdrUnreach->dwUnused = 0;
	pHdrUnreach->wTotalCRC = 0;
	memcpy( (LPBYTE)pHdrUnreach+sizeof(ICMP_UNREACH_HDR), pData, wLenData );
	pHdrUnreach->wTotalCRC = IP_CRC(pBufIcmpTmp, wLenTotal );
	//
	wLenTotal += sizeof(IP_HDR);
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_ICMP;
	pHdrIP->dwDesIP = dwDesIP;
	pHdrIP->dwSrcIP = dwSrcIP;
	//pHdrIP->wFragment = 0;
	pHdrIP->wLenTotal = wLenTotal;
	//
	dwRet = IP_NetOut2( IPTXOPT_INTF_SPEC, pHdrIP, pFragSetA );
	if( !dwRet )
	{
		return 2;
	}
	return 0;
}



