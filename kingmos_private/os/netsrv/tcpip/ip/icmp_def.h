/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_ICMP_DEF_H_
#define	_ICMP_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------------------------
// ICMP ECHO ITEM
//-------------------------------------------------------------------------
typedef	struct	_ECHO_ITEM
{
	HANDLE_THIS( _ECHO_ITEM );

	//所有 Echo的链表
	LIST_UNIT			hListEcho;

	//
	HANDLE				hProc;
	HANDLE				hEvtReply;
	WORD				wFlag;
	WORD				wCntUsed;

	DWORD				dwDesIP;
	WORD				wXID;
	WORD				wSeqNum;
	DWORD				dwTickTx;

	//
	LPBYTE				pBufEcho;
	LPBYTE				pBufReply;
	WORD				wLenEcho;
	WORD				wLenReply;
	WORD				wLenReply_Buf;
	WORD				wXXX;
	IP_OPTION_INFORMATION	Options;

} ECHO_ITEM;

//wFlag
#define	ECHOITEMF_EXIT		0x0001
#define	ECHOITEMF_BUSY		0x0002
#define	ECHOITEMF_REPLY		0x0004


//-------------------------------------------------------------------------
// ICMP 全局信息
//-------------------------------------------------------------------------
typedef	struct	_ICMP_GLOBAL
{
	HANDLE_THIS( _ICMP_GLOBAL );

	//所有 Echo的链表
	CRITICAL_SECTION	csListEcho;
	LIST_UNIT			hListEcho;

	//所有统计信息
	MIBICMPSTATS		icmpInStats; 
	MIBICMPSTATS		icmpOutStats; 


	//操作信息
	WORD				wFlag;
	WORD				wXID;

} ICMP_GLOBAL;


//
#define	GICMPF_EXIT				0x1000

extern	ICMP_GLOBAL*	g_lpGlobalIcmp;


extern	WORD	IP_CRC( LPBYTE pBuf, DWORD dwLenBuf );


#ifdef __cplusplus
}	
#endif


#endif	//_ICMP_DEF_H_

