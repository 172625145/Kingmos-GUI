/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_TCP_H_
#define	_NETIF_TCP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ipdata.h>
//
extern	DWORD	Tcp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pBufData, WORD wLenData );
extern	DWORD	Udp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pBufData, WORD wLenData );
extern	DWORD	Icmp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pBufData, WORD wLenData );

//
extern	DWORD	Icmp_TrOut_Unreach( BYTE bCode, DWORD dwDesIP, DWORD dwSrcIP, LPBYTE pData, WORD wLenData );


#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_TCP_H_
