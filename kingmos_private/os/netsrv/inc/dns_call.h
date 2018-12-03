/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _DNS_CALL_H_
#define _DNS_CALL_H_

#ifdef __cplusplus
extern "C" {
#endif


//dwOptDns
#define	DNSOPT_NOT_FROM_TBL		0x01
#define	DNSOPT_NOT_FROM_NET		0x02

//return value of DnsCall_Query
#define	DNSERR_SUCCESS			0
#define	DNSERR_PARAM			1
#define	DNSERR_NO_BUFS			2
#define	DNSERR_NO_DNSSRV		3
#define	DNSERR_NOT_FOUND		4
#define	DNSERR_WRONG_RESP		5

extern	DWORD	DnsCall_Query( LPCSTR pszNameDns, OUT HOSTENT* pHostInfo, int len, OUT WORD* pwCntAddr, DWORD dwOptDns );


#ifdef __cplusplus
}	
#endif

#endif	//_DNS_CALL_H_
