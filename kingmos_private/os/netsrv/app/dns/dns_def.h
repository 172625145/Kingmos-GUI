/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_DNS_DEF_H_
#define	_DNS_DEF_H_

#ifndef _HANDLE_CHK_2004_H_
#include "handle_check.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define IPPORT_DNS_SRV			53

//---------------------------------------------------
// DHCP----RFC 1035
//---------------------------------------------------
//
//----DNS protocol format : (p: means that optional)
//
//    +---------------------+
//    |        Header       |
//    +---------------------+
//    |       Question      | the question for the name server
//    +---------------------+
//    |        Answer(p)    | RRs answering the question
//    +---------------------+
//    |      Authority(p)   | RRs pointing toward an authority
//    +---------------------+
//    |      Additional(p)  | RRs holding additional information
//    +---------------------+
//
//
//----Header section format :
//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      ID                       |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    QDCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    ANCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    NSCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    ARCOUNT                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    QR: query (0), or a response (1).
//    OPCODE: QUERY / IQUERY / STATUS 
//    AA: Authoritative Answer - this bit is valid in responses
//    TC: TrunCation for transmission channel
//    RD: Recursion Desired - may be set in query and copied into the response
//    RA: Recursion Available - be set or cleared in a response, whether recursive query support is available in the name server.
//    Z : Reserved for future use.  Must be zero
//    RCODE: Response code 
//
//
//----Question section format:
//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                                               |
//    /                     QNAME                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     QTYPE                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     QCLASS                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//
//----All RRs have the same top level format shown below:
//                                    1  1  1  1  1  1
//      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                                               |
//    /                                               /
//    /                      NAME                     /
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      TYPE                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     CLASS                     |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                      TTL                      |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                   RDLENGTH                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
//    /                     RDATA                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//
//
//----RDATA format:
//CNAME RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                     CNAME                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//HINFO RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                      CPU                      /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                       OS                      /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//MX RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                  PREFERENCE                   |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                   EXCHANGE                    /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//NS RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                   NSDNAME                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//PTR RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                   PTRDNAME                    /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//SOA RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                     MNAME                     /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                     RNAME                     /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    SERIAL                     |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    REFRESH                    |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                     RETRY                     |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    EXPIRE                     |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    MINIMUM                    |
//    |                                               |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//TXT RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    /                   TXT-DATA                    /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//
//A RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    ADDRESS                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//WKS RDATA format
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |                    ADDRESS                    |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    |       PROTOCOL        |                       |
//    +--+--+--+--+--+--+--+--+                       |
//    |                                               |
//    /                   <BIT MAP>                   /
//    /                                               /
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//
//
//关于 Domain name compression:
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//    | 1  1|                OFFSET                   |
//    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
//The OFFSET field specifies an offset from
//the start of the message (i.e., the first octet of the ID field in the
//domain header).  A zero offset specifies the first byte of the ID field,
//etc.


//
//定义 DNS头
//
typedef	struct	_DNS_HDR
{
	WORD	wXID;		//Requester's ID
	WORD	wFlag;

	WORD	wCntQD;		//Question section
	WORD	wCntAn;		//Answer RR
	WORD	wCntNS;		//Name Server RR
	WORD	wCntAR;		//Additional Records section

} DNS_HDR;

#define	DNS_OFFSET_QD			4
#define	DNS_OFFSET_AN			(4 +2)
#define	DNS_OFFSET_NS			(4 +2 +2)
#define	DNS_OFFSET_AR			(4 +2 +2 +2)

#define	DNS_RR_ATTR_LEN			(2 +2 +4)
#define	DNS_RR_RLEN_LEN			(2)

#define	DNS_QUESTION_LEN		(sizeof(WORD)+sizeof(WORD))
//wFlag
#define	DNSF_QR_MASK			REORDER_S(0x8000)
#define	DNSF_QR_Q				REORDER_S(0x0000)
#define	DNSF_QR_R				REORDER_S(0x8000)
#define	DNSF_OP_MASK			REORDER_S(0x7800)
#define	DNSF_OP_STD				REORDER_S(0x0000)	//0
#define	DNSF_OP_INV				REORDER_S(0x0800)	//1
#define	DNSF_OP_STAU			REORDER_S(0x1000)	//2
													//请求时					回答时
#define	DNSF_AA					REORDER_S(0x0400)	//
#define	DNSF_TC					REORDER_S(0x0200)	//是否可以截断				是否已经截断
#define	DNSF_RD					REORDER_S(0x0100)	//期望递归或者迭代查询		使用递归或者迭代查询
#define	DNSF_RA					REORDER_S(0x0080)	//是否支持递归查询			是否支持递归查询

#define	DNSF_Z_MASK				REORDER_S(0x0070)
#define	DNSF_RCODE_MASK			REORDER_S(0x000F)
#define	DNSF_RCODE_OK			REORDER_S(0x0000)
#define	DNSF_RCODE_FORMAT		REORDER_S(0x0001)
#define	DNSF_RCODE_SERVER		REORDER_S(0x0002)
#define	DNSF_RCODE_NAME			REORDER_S(0x0003)
#define	DNSF_RCODE_QTYPE		REORDER_S(0x0004)
#define	DNSF_RCODE_REFUSED		REORDER_S(0x0005)


//
//定义 Type
//
#define	DNS_RRTYPE_A			REORDER_S(1  )	// a host address
#define	DNS_RRTYPE_NS			REORDER_S(2  )	// an authoritative name server
#define	DNS_RRTYPE_CNAME		REORDER_S(5  )	// the canonical name for an alias
#define	DNS_RRTYPE_SOA			REORDER_S(6  )	// marks the start of a zone of authority
#define	DNS_RRTYPE_WKS			REORDER_S(11 )	// a well known service description
#define	DNS_RRTYPE_PTR			REORDER_S(12 )	// a domain name pointer
#define	DNS_RRTYPE_HINFO		REORDER_S(13 )	// host information
#define	DNS_RRTYPE_MINFO		REORDER_S(14 )	// mailbox or mail list information
#define	DNS_RRTYPE_MX			REORDER_S(15 )	// mail exchange
#define	DNS_RRTYPE_TXT			REORDER_S(16 )	// text strings

#define	DNS_QTYPE_AXFR			REORDER_S(252)	// A request for a transfer of an entire zone
#define	DNS_QTYPE_ANY			REORDER_S(255)	// A request for all records

//
//定义 Class
//
#define	DNS_RRCLASS_IN			REORDER_S(1  )	// the Internet
#define	DNS_RRCLASS_CH			REORDER_S(3  )	// the CHAOS class
#define	DNS_RRCLASS_HS			REORDER_S(4  )	// Hesiod [Dyer 87]


//---------------------------------------------------
// DNS 查询项
//---------------------------------------------------

#define	LEN_DNSRES		1024
typedef	struct	_DNS_ITEM
{
	SOCKET				skDns;

	//源信息
	//LPSTR				pszNameDns;

	//DWORD				dwWaitS;
	DWORD				dwDnsSrvIP;
	WORD				wDnsSrvPort;
	WORD				wXID;

	LPBYTE				pBufDNS;				//发送的 查询数据包
	BYTE				pDataRes[LEN_DNSRES];	//接收的 回答数据包
	WORD				wLenDns;				//pBufDNS's Len
	WORD				wLenResRx;				//pDataRes's real len

	//
	LPBYTE				pAddrList;		//发送的 查询数据包
	WORD				wXXX;
	WORD				wAddrLen;
	WORD				wCntAddrBuf;
	WORD				wCntAddrReal;

} DNS_ITEM;

//---------------------------------------------------
// DNS 全局信息
//---------------------------------------------------

typedef	struct	_DNS_HOSTADDR
{
	HANDLE_THIS( _DNS_HOSTADDR );

	LIST_UNIT			hListHAddr;

	//
	DWORD				dwTickLife;
	LPSTR				pszNameDns;

	LPBYTE				pAddrList;		//发送的 查询数据包
	WORD				wXXX;
	WORD				wAddrLen;
	WORD				wCntAddrBuf;
	WORD				wCntAddrReal;

} DNS_HOSTADDR;

//dwTickLife
#define	TIMER_HOSTIPLIFE	(5*60*1000)

//wCntAddrBuf wCntAddrReal
#define	CNTADDR_GROW		4


//---------------------------------------------------
// DNS 全局信息
//---------------------------------------------------

typedef	struct	_DNS_GLOBAL
{
	HANDLE_THIS( _DNS_GLOBAL );

	//所有 DHCP item
	CRITICAL_SECTION	csListHAddr;
	LIST_UNIT			hListHAddr;

	WORD				wXID;

} DNS_GLOBAL;


extern	DNS_GLOBAL*		g_lpGlobalDNS;


//函数

extern	BOOL	DnsTbl_SaveHostAddr( LPCSTR pszNameDns, LPBYTE pAddrList, WORD wAddrLen, WORD wCntAddr );
extern	BOOL	DnsTbl_FindHostByName( OUT DNS_HOSTADDR** ppHAddrDns, LPCSTR pszNameHost );



#ifdef __cplusplus
}	
#endif

#endif	//_DNS_DEF_H_
