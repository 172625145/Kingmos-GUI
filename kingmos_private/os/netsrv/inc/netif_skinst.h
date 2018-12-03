/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _NETIF_SKINST_H_
#define _NETIF_SKINST_H_

#ifndef	_NETIF_COMM_H_
#include "netif_comm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


// ------------------------------------------------------
// 预声明
// ------------------------------------------------------
struct	_SOCK_INST;
typedef	struct	_SOCK_INST		SOCK_INST;

struct	_RXTXMSG;
typedef	struct	_RXTXMSG		RXTXMSG;

struct	_POLLTBL;
typedef	struct	_POLLTBL		POLLTBL;

//typedef	struct	_SOCK_INST		SOCK_INST;
//typedef	struct	_RXTXMSG		RXTXMSG;
//typedef	struct	_POLLTBL		POLLTBL;

// ------------------------------------------------------
// Transfer(TCP/UDP)层提供 函数表和信息
// ------------------------------------------------------
typedef	struct	_TL_LOCAL
{
	HANDLE_THIS( _TL_LOCAL );

	LIST_UNIT	hListTl;
	HANDLE		hTlGlobal;

	int		nFamily;
	int		nType;
	int		nProtocol;
	
	SKERR	(*pFnTlOpen)( SOCK_INST* pInfoSkt );
	SKERR	(*pFnTlBind)( SOCK_INST* pInfoSkt, const SOCKADDR* myaddr, int addr_len );
	SKERR	(*pFnTlConnect)( SOCK_INST* pInfoSkt, const SOCKADDR* addr, int addr_len, int flags );
	SKERR	(*pFnTlListen)( SOCK_INST* pInfoSkt, int queue_len );
	SKERR	(*pFnTlAccept)( SOCK_INST* pInfoSkt, OUT SOCK_INST* pInfoSkt_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags );
	SKERR	(*pFnTlIoctl)( SOCK_INST* pInfoSkt, long nCmd, IN OUT DWORD* pdwArg );
	SKERR	(*pFnTlSendmsg)( SOCK_INST* pInfoSkt, RXTXMSG* pMsgRxTx );
	SKERR	(*pFnTlRecvmsg)( SOCK_INST* pInfoSkt, IN OUT RXTXMSG* pMsgRxTx );
	SKERR	(*pFnTlPoll)( POLLTBL* pTblPoll, IN OUT int* pnCntPoll, DWORD dwTmout );

	SKERR	(*pFnTlGetSockOpt)( SOCK_INST* pInfoSkt, int level, int optname, OUT char* optval, int optlen );
	SKERR	(*pFnTlSetSockOpt)( SOCK_INST* pInfoSkt, int level, int optname, const char* optval, int optlen );
	SKERR	(*pFnTlGetName)( SOCK_INST* pInfoSkt, OUT SOCKADDR* addr, OUT int* paddr_len, int peer );

	SKERR	(*pFnTlShutDown)( SOCK_INST* pInfoSkt, int how );
	SKERR	(*pFnTlClose)( SOCK_INST* pInfoSkt );

} TL_LOCAL;


// ------------------------------------------------------
// Transfer(TCP/UDP)层 收发信息
// ------------------------------------------------------
//typedef	struct	_RXTXMSG
struct	_RXTXMSG
{
	char*		lpPeer;		//对方的信息 Socket name
	DWORD		dwLenPeer;

	LPBYTE		pData;
	DWORD		dwLenData;
	DWORD		dwFlag;

} ;//RXTXMSG;

// ------------------------------------------------------
// Transfer(TCP/UDP)层 查询信息
// ------------------------------------------------------
//typedef	struct	_POLLTBL
struct	_POLLTBL
{
	SOCKET			sID;
	SOCK_INST*		pSockInst;
	DWORD			dwFlag;

} ;//POLLTBL;

//dwFlag of POLLTBL
#define SOCK_POLL_READ			0x0001
#define SOCK_POLL_WRITE			0x0002
#define SOCK_POLL_EXCEPT		0x0004
#define SOCK_POLL_RESULT		(SOCK_POLL_READ |SOCK_POLL_WRITE |SOCK_POLL_EXCEPT)

#define SOCK_POLL_QREAD			0x0010	//通知accept_succeed, data_read, cnn_close/reset/terminated
#define SOCK_POLL_QWRITE		0x0020
#define SOCK_POLL_QEXCEPT		0x0040

// ------------------------------------------------------
// Transfer(TCP/UDP)层 实例
// ------------------------------------------------------
//typedef	struct	_SOCK_INST
struct	_SOCK_INST
{
	HANDLE_THIS( _SOCK_INST );

	LIST_UNIT			hListSkInst;

	//属性内容
	SOCKET				sock_id;
	//int					af;
	//int					type;
	//int					protocol;
	WORD				wFlag;
	WORD				wXXX;

	//传输层信息
	TL_LOCAL*			pTlLocal;
	HANDLE				hIfTl;

	//
	LPVOID				lpSkProc;

} ;//SOCK_INST;

//wFlag
#define	SKINSTF_EXIT		0x1000

#define	SKINST_CHECK_OK(lp)			( HANDLE_CHECK(lp) && !(lp->wFlag & SKINSTF_EXIT) )
#define	SKINST_CHECK_FAIL(lp)		( !HANDLE_CHECK(lp) || (lp->wFlag & SKINSTF_EXIT) )

//
extern	BOOL	Sock_Register( TL_LOCAL* pTlLocal );


#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_SKINST_H_
