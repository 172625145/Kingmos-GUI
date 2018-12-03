/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_PROTO_IP_TCP_H_
#define	_PROTO_IP_TCP_H_

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------
// IP---RFC 791
//---------------------------------------------------
//
// Internet Header Format: 
//    0                   1                   2                   3   
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |Version|  IHL  |Type of Service|          Total Length         |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |         Identification        |Flags|      Fragment Offset    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Time to Live |    Protocol   |         Header Checksum       |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                       Source Address                          |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Destination Address                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Options                    |    Padding    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

#pragma  pack(push,1)
typedef	struct	_IP_HDR
{
	BYTE	bVerHLen;	
	BYTE	bTOS;		
	WORD	wLenTotal;	//#	//按Host顺序处理
	WORD	wXID;		//#	//按Host顺序处理
	WORD	wFragment;	//#	//按Host顺序处理
	BYTE	bTTL;		
	BYTE	bProType;	
	WORD	wHdrCRC;	//#
	DWORD	dwSrcIP;	
	DWORD	dwDesIP;	

} _PACKED_ IP_HDR;
#pragma	pack(pop)


//注意：打#表示由IP层转化和指定

//bVerHLen
#define	IPV4					0x40
#define	IPV_MASK				0xF0
#define	IPL_MASK				0x0F
#define	IPV4_VHDEF				0x45

//wFragment---按Host顺序处理
#define	IPF_DF					0x4000
#define	IPF_MF					0x2000
#define	IPF_FLAG				0x6000
#define	IPF_OFFSET				0x1FFF
#define	IPF_MAXFRAG				65500

//for IP_HDR's bProType
#define	IP_PROTYPE_ICMP			1
#define	IP_PROTYPE_IGMP			2
#define	IP_PROTYPE_TCP			6
#define	IP_PROTYPE_UDP			17

//
#define	TTL_DEFAULT				128


//---------------------------------------------------
// ICMP---RFC 792
//---------------------------------------------------
//
// Internet Control Message Protocol Header Format: 
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |     Code      |          Checksum             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//
//Destination Unreachable Message
//Source Quench Message
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |     Code      |          Checksum             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                             unused                            |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |      Internet Header + 64 bits of Original Data Datagram      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
//
//Echo or Echo Reply Message
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |     Code      |          Checksum             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |           Identifier          |        Sequence Number        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Data ...
//   +-+-+-+-+-

#pragma  pack(push,1)

typedef	struct	_ICMP_HDR
{
	BYTE	bType;
	BYTE	bCode;
	WORD	wTotalCRC;	//Both Header and data check

} _PACKED_ ICMP_HDR;

typedef	struct	_ICMP_ECHO_HDR
{
	BYTE	bType;
	BYTE	bCode;
	WORD	wTotalCRC;	//Both Header and data check

	WORD	wXID;
	WORD	wSeqNum;

} _PACKED_ ICMP_ECHO_HDR;

typedef	struct	_ICMP_UNREACH_HDR
{
	BYTE	bType;
	BYTE	bCode;
	WORD	wTotalCRC;	//Both Header and data check

	DWORD	dwUnused;

} _PACKED_ ICMP_UNREACH_HDR;

#pragma	pack(pop)

//bType:  (*表示当前要支持的，Ignore---6)
#define	ICMP_TYPE_ECHO_REP					0	//*	
#define	ICMP_TYPE_DES_UNREACH				3	//*	
#define	ICMP_TYPE_SRC_QUENCH				4	//*	
#define	ICMP_TYPE_REDIRECT					5	//*
#define	ICMP_TYPE_ECHO						8
#define	ICMP_TYPE_R_AD						9		//[RFC1256]
#define	ICMP_TYPE_R_SOLICIT					10		//[RFC1256]
#define	ICMP_TYPE_TIME_EXCEED				11		//for Traceroute
#define	ICMP_TYPE_PARAMETER					12	//*
#define	ICMP_TYPE_TIMESTAMP					13	//*
#define	ICMP_TYPE_TIMESTAMP_REP				14	//*
#define	ICMP_TYPE_ADDR_MASK					17	//*	//[RFC950]
#define	ICMP_TYPE_ADDR_MASK_REP				18	//*	//[RFC950]
#define	ICMP_TYPE_TRACEROUTE				30		//[RFC1393]
#define	ICMP_TYPE_DNS						37		//[RFC1788]
#define	ICMP_TYPE_DNS_REP					38		//[RFC1788]

//bCode
//-------ICMP_TYPE_DES_UNREACH
#define ICMP_CODE_DESUR_NET					0
#define ICMP_CODE_DESUR_HOST				1
#define ICMP_CODE_DESUR_PROTOCOL			2
#define ICMP_CODE_DESUR_PORT				3
#define ICMP_CODE_DESUR_FRAGMENT			4
#define ICMP_CODE_DESUR_ROUTE				5
#define ICMP_CODE_DESUR_UNNET				6
#define ICMP_CODE_DESUR_UNHOST				7


//---------------------------------------------------
// TCP---RFC 826
//---------------------------------------------------
//
// TCP Header Format:
//    0                   1                   2                   3   
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |          Source Port          |       Destination Port        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                        Sequence Number                        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Acknowledgment Number                      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |  Data |           |U|A|P|R|S|F|                               |
//   | Offset| Reserved  |R|C|S|S|Y|I|            Window             |
//   |       |           |G|K|H|T|N|N|                               |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |           Checksum            |         Urgent Pointer        |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                    Options                    |    Padding    |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                             data                              |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

typedef	struct	_TCP_HDR
{
	WORD		wSrcPort;
	WORD		wDesPort;
	DWORD		dwSeqNum;
	DWORD		dwAckNum;

	WORD		wHLenFlag;
	WORD		wWinSize;

	WORD		wTotalCRC;//Include TCP header and data
	WORD		wUrgPtr;

} TCP_HDR;

#pragma  pack(push,1)
typedef	struct	_TCP_HDR_DBG
{
	WORD		wSrcPort;
	WORD		wDesPort;
	DWORD		dwSeqNum;
	DWORD		dwAckNum;

	WORD		wHLenFlag;
	WORD		wWinSize;

	WORD		wTotalCRC;//Include TCP header and data
	WORD		wUrgPtr;

} _PACKED_ TCP_HDR_DBG;
#pragma	pack(pop)

//wHLenFlag
#define	TCP_HLEN_MASK	REORDER_S(0xF000)
#define	TCP_HLEN_FIX	REORDER_S(0x5000)
//#define	TCP_HLEN_SYN	REORDER_S(0xF000)

#define	TCP_RESD_MASK	REORDER_S(0x0FC0)

#define	TCP_FLAG_MASK	REORDER_S(0x003F)
#define	TCP_FLAG_URG	REORDER_S(0x0020)
#define	TCP_FLAG_ACK	REORDER_S(0x0010)
#define	TCP_FLAG_PSH	REORDER_S(0x0008)
#define	TCP_FLAG_RST	REORDER_S(0x0004)
#define	TCP_FLAG_SYN	REORDER_S(0x0002)
#define	TCP_FLAG_FIN	REORDER_S(0x0001)

//TCP_HDR 后面的 选项
#define TCP_OPT_END				0
#define TCP_OPT_SKIP			1

#define TCP_OPT_MSS				2
#define TCP_OPT_MSS_LEN			4

#define TCP_OPT_WSOPT			3
#define TCP_OPT_WSOPT_LEN		3

#define TCP_OPT_SACK_PERM		4
#define TCP_OPT_SACK_PERM_LEN	2

#define TCP_OPT_SACK			5

#define TCP_OPT_TSOPT			8
#define TCP_OPT_TSOPT_LEN		10

//
#define	MSS_DEFAULT				512


//---------------------------------------------------
// UDP---RFC 768
//---------------------------------------------------
//
// UDP Header Format:
//     0      7 8     15 16    23 24    31  
//    +--------+--------+--------+--------+ 
//    |     Source      |   Destination   | 
//    |      Port       |      Port       | 
//    +--------+--------+--------+--------+ 
//    |                 |                 | 
//    |     Length      |    Checksum     | 
//    +--------+--------+--------+--------+ 

typedef	struct	_UDP_HDR
{
	WORD		wSrcPort;
	WORD		wDesPort;

	WORD		wLenTotal;
	WORD		wTotalCRC;

} UDP_HDR;

typedef	struct	_PSEUDO_HDR
{
	DWORD	dwSrcIP;
	DWORD	dwDesIP;
	BYTE	bZero;
	BYTE	bProType;
	WORD	wTotalUDPLen;

} PSEUDO_HDR;

#pragma  pack(push,1)
typedef	struct	_UDP_HDR_DBG
{
	WORD		wSrcPort;
	WORD		wDesPort;

	WORD		wLenTotal;
	WORD		wTotalCRC;

} _PACKED_ UDP_HDR_DBG;
#pragma	pack(pop)


//---------------------------------------------------
// CRC
//---------------------------------------------------

#ifndef _FRAGMENT_H_
#include "fragment.h"
#endif

extern	WORD	IP_CRC( LPBYTE pBuf, DWORD dwLenBuf );
extern	WORD	Tcp_CRC( DWORD dwSrcIP, DWORD dwDesIP, BYTE bProType, LPBYTE pBufData, WORD wLenData );
extern	WORD	Tcp_CRC2( DWORD dwSrcIP, DWORD dwDesIP, BYTE bProType, FRAGSETA* pFragSet );


#ifdef __cplusplus
}	
#endif


#endif	//_PROTO_IP_TCP_H_

