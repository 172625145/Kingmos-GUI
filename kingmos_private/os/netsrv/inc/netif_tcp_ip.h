/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_TCP_IP_H_
#define	_NETIF_TCP_IP_H_

#ifndef	_NETIF_COMM_H_
#include "netif_comm.h"
#endif

#ifndef	_NETIF_IP_H_
#include "netif_ip.h"
#endif

#ifndef	_NETIF_TCP_H_
#include "netif_tcp.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
typedef DWORD	IPRET;

#define	IPERR_SUCCESS			0
#define	IPERR_NO_INTF			1	//没有找到IP源接口
#define	IPERR_FRAG_DF			2	//分片过大，又不让分片
#define	IPERR_SND_LARGE			3	//发送的数据过大，超过分片能力
#define	IPERR_SND_NOBUF			4	//内存不够
#define	IPERR_FRAGA_NUM			5	//Frag的个数太多
#define	IPERR_SND_OPTION		6	//Frag的个数太多


//
extern	IPRET	IP_NetOut( HANDLE hNetIntf, IP_HDR* pHdrIP, FRAGSETA* pFragSet );


//dwOption of IP_NetOut2 function
#define	IPTXOPT_INTF_BEST		0x0001
#define	IPTXOPT_INTF_MULT		0x0002
#define	IPTXOPT_INTF_SPEC		0x0004
extern	DWORD	IP_NetOut2( DWORD dwOption, IP_HDR* pHdrIP, FRAGSETA* pFragSetA );




#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_TCP_IP_H_
