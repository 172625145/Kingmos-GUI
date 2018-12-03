/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TCP_DEF_H_
#define _TCP_DEF_H_

#ifndef _RW_BUF_H_
#include "rwbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



//---------------------------------------------------
// TCB
//---------------------------------------------------

struct	_TCP_GLOBAL;
typedef	struct	_TCP_GLOBAL		TCP_GLOBAL;

//保存每个 “TCP SOCKET对” 的所有操作信息
typedef	struct	_TCP_TCB
{
	HANDLE_THIS( _TCP_TCB );

	TCP_GLOBAL*			lpGlobalTcp;

	//TCB链表
	LIST_UNIT			hListTcb;

	//Socket对
	DWORD				dwSrcIP;
	DWORD				dwDesIP;
	WORD				wSrcPort;
	WORD				wDesPort;

	//操作信息
	//
	CRITICAL_SECTION	csTcb;

	//TCP 和 TCP Socket 操作信息
	WORD				wAttr;		//属性标志
	WORD				wFlag;		//操作标志
	DWORD				dwState;	//操作阶段
	DWORD				dwOption;	//操作选项：_SOL_SOCKET级别_BOOL类型的设置
	HANDLE				hEvtPoll;	//通知 Block/Nonblock的事件
										//Block时：connect / accept / send / recv
										//Nonblock时：select
	SKERR				nErr;		//用于
	DWORD				dwTickLife;	//生命周期的起点，用于SYN_RECVD的TCB

	//accept操作
	LIST_UNIT			hListSynRcvd;//该accept上所有的接收TCB，但都没有挂到pInfoSocket上
	struct	_TCP_TCB*	pTcbParent;

	//如果是 监听socket
	WORD				wMaxConn;
	WORD				wCntConn;

	//发送线程操作
	DWORD				dwTickRTT;
	DWORD				dwRTO;
	HANDLE				hEvtSend;
	HANDLE				hThrdSend;

	//发送缓冲
//
//情况1-----
//
//         dwSnd_una                 dwSnd_nxt     dwSnd_max
//  |---------|-------------------------|--------------|------------|
//  | empty   | has sent but no ack     | wait to send |   empty    |
//  |---------|-------------------------|--------------|------------|
//            |----  wBytesSent  -------|-- wBytesTx --|
//            |----------------  dwCntTx  -------------|
//  0      dwReadTx                 dwStartTx      dwWriteTx
//
//
//情况2-----
//
//dwSnd_max               dwSnd_una            dwSnd_nxt     
//  |--------------|---------|---------------------|----------------|
//  | wait to send | empty   | has sent but no ack | wait to send   |
//  |--------------|---------|---------------------|----------------|
//  |-- wBytesTx2--|         |----  wBytesSent  ---|-- wBytesTx1 ---|
//  |-- dwCntTx2 --|         |----------------  dwCntTx1  ----------|
//  0          dwWriteTx  dwReadTx              dwStartTx 
//
	RWBUFFER			stTxBufRW;
	DWORD				dwLenLeftTx;

	DWORD				dwSnd_una;	//“已经发送但对方没有ACK”的数据的 SEQ
	DWORD				dwSnd_nxt;	//下次发送的数据的位置 SEQ
	DWORD				dwSnd_max;	//最后将要发送的最大位置
	DWORD				dwSnd_real;	//目前实际发送的最长位置

	//WORD				wSnd_wnd;	//自己当前的Window size
	WORD				wXXX;
	WORD				wSnd_wnd_Last;	//自己当前的Window size---最后1次发送包含的信息
	WORD				wSnd_MSS;	//自己当前的MSS
	WORD				wSnd_quick;	//快速重传的计数

	//接收缓冲
//	RWBUFFER			stRxBufRW;

	DWORD				dwRcv_nxt;	//下次接收的数据的SEQ  和  每次要发送的Ack
	WORD				wRcv_wnd;	//对方通告的Window size
	WORD				wRcv_MSS;	//对方通告的MSS

	//
	//保存 每次接收计算的信息
	//
	LPBYTE				pTcpData;
	WORD				wLenHdr;
	WORD				wLenData;


	//保存 每次发送信息
	//
	DWORD				dwAck_Last;

	IPDATA_IN		    * pCurDataIn;//当前的输入包
	LIST_UNIT			hCommitPacketDataIn;	//顺序并且未读的数据包
	LIST_UNIT			hUnCommitPacketDataIn;	//未决的乱序数据包
	DWORD               dwMaxRxBufSize;
	DWORD               dwUsedRxBufLen;
	CRITICAL_SECTION     csRx;
	CRITICAL_SECTION     csRxUnCommit;
	LONG                 lPackCount;
} TCP_TCB;

//dwLenRx
#define	LEN_RXBUF				4096
#define	MIN_RXBUF				2048
#define	MAX_RXBUF				0x10000
//wBytesTx
#define	LEN_TXBUF				4096
#define	MIN_TXBUF				2048
#define	MAX_TXBUF				0x10000

//wSnd_quick
#define	TCPQK_MAX				3

//dwRTO
#define	TCPTM_SEND_FIRST		(6*1000)
#define	TCPTM_SEND_MIN			(2*1000)
#define	TCPTM_SEND_MAX			(2*60*1000)  //(5*60*1000)

#define	TCPTM_CONNECT			(1+2*60*1000)	//connect操作等待的时间
#define	TCPTM_WAITACCEPT		(60*1000)
#define	TCPTM_WAITBUSY			(3*1000)
#define	TCPTM_WAITSEND			(6*1000)
#define	TCPTM_SEND				(15*60*1000)
#define	TCPTM_WAITRECV			(5*1000)
#define	TCPTM_WAITSHUT			(60*1000)
#define	TCPTM_WAITPERSIST		(30*1000)


#define	TCPTM_SYNRCVD			(3*60*1000)

//dwState
#define	TCPS_CLOSED				0	//关闭结束
#define	TCPS_LISTEN				1	//监听
#define	TCPS_SYN_SENT			2	//主动SYN
#define	TCPS_SYN_RECVD			3	//被动SYN
#define	TCPS_ESTABLISHED		4	//连接
#define	TCPS_CLOSE_WAIT			5	//被动关闭
#define	TCPS_LAST_ACK			6	//应答关闭
#define	TCPS_FIN_WAIT1			7	//主动关闭
#define	TCPS_FIN_WAIT2			8	//正常关闭
#define	TCPS_CLOSING			9	//同时关闭
#define	TCPS_TIME_WAIT			10	//等待关闭
#define	TCPS_MAX				11

//wAttr---TCB属性，一般如果存在的话，将会是它的生命周期之内都存在；所以只有重新创建TCB
#define	TCPA_CONNECT			0x0001	//
#define	TCPA_LISTEN				0x0002	//
//#define	TCPA_SYNRCVD			0x0004	//for debug
#define	TCPA_CLOSING			0x0008	//同时关闭
#define	TCPA_ESTABLISH			0x0010	//

#define	TCPA_RECV				0x0040	// for shutdown
#define	TCPA_SEND				0x0080	// for shutdown
#define	TCPA_CONNRESET			0x0100	// for recv FIN/SYN
#define	TCPA_NETRESET			0x0200	// for keep-alive failure
#define	TCPA_HOST_UNREACH		0x0400	// for recv icmp... when sendmsg
#define	TCPA_CONN_ABORTED		0x0800	//

#define	TCPA_ERR_ALL			(TCPA_CONNRESET |TCPA_NETRESET |TCPA_HOST_UNREACH |TCPA_CONN_ABORTED)


//wFlag---数据包
#define	TCPF_LSYN				0x0001	//收到对方的SYN's ACK--表示自己发送的SYN已经被成功接收
#define	TCPF_RSYN				0x0002	//收到对方的SYN
#define	TCPF_ALLSYN				(TCPF_LSYN | TCPF_RSYN)	//本地收发SYN都成功

#define	TCPF_LFIN				0x0004	//收到对方的FIN's ACK--表示自己发送的FIN已经被成功接收
#define	TCPF_RFIN				0x0008	//收到对方的FIN
#define	TCPF_ALLFIN				( TCPF_LFIN | TCPF_RFIN )	//本地收发FIN都成功

#define	TCPF_ALLFRM				(TCPF_ALLSYN | TCPF_ALLFIN)

//wFlag---查询标志
#define	TCPQ_CNN_YES			0x0100
#define	TCPQ_CNN_NO				0x0200
#define	TCPQ_LISTEN_YES			0x0400
#define	TCPQ_LISTEN_NO			0x0800

//wFlag---发送标志
#define	TCPF_EXIT				0x1000	//要求退出
#define	TCPF_NEEDACK			0x2000	//要求 发送“经过时延的ACK”
//#define	TCPF_2MSL_ACK			0x4000	//在2MSL等待时间内，发送ACK
#define	TCPF_SENDING			0x4000	//在2MSL等待时间内，发送ACK
#define	TCPF_NEEDPERSIST		0x8000	//需要使用坚持定时器



//---------------------------------------------------
// Global
//---------------------------------------------------

//typedef	SKERR	(*FNTCPIN)(TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp);

//typedef	struct	_TCP_GLOBAL
struct	_TCP_GLOBAL
{
	HANDLE_THIS( _TCP_GLOBAL );

	//所有 TCP SOCKET实例
	LIST_UNIT			hListTcb;
	CRITICAL_SECTION	csListTcb;

	//操作信息
	WORD				wFlag;		//标志
	WORD				wPort_Auto;	//自动绑定的端口
	DWORD				dwTickISN;	//ISN
	DWORD				dwISN;
	HANDLE				hEvtPersist;//Persist
	HANDLE				hThrdPersist;

	//传输层 函数表和信息
	TL_LOCAL			LocalTl;
	//TCP 输入状态处理 函数表
//	FNTCPIN				pFnTcpIn[ TCPS_MAX ];

} ;//TCP_GLOBAL;

extern	TCP_GLOBAL*		g_lpGlobalTcp;

//---------------------------------------------------
// 函数
//---------------------------------------------------
extern	BOOL	Tcp_CreateTcb( TCP_TCB** ppTcbTcp, TCP_GLOBAL* lpGlobalTcp );
extern	void	Tcp_DeleteTCB( TCP_TCB* pTcbTcp );
extern	BOOL	Tcp_QueryMSS( DWORD dwSrcIP, DWORD dwDesIP, OUT WORD* pwMSS_me );
extern	WORD	Tcp_CalWndSize( TCP_TCB* pTcbTcp );
extern	DWORD	Tcp_CalISN( TCP_TCB* pTcbTcp );

extern	BOOL	TcpSk_Init( TCP_GLOBAL* lpGlobalTcp );
extern	BOOL	TcpIn_Init( TCP_GLOBAL* lpGlobalTcp );
extern	BOOL	TcpOut_StartSend( TCP_TCB* pTcbTcp );
extern	BOOL	TcpOut_StartPersist( TCP_GLOBAL* lpGlobalTcp );

extern	IPRET	TcpOut_SendRst( TCP_TCB* pTcbTcp, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDesIP, WORD wDesPort, DWORD dwAckNum );
extern	IPRET	TcpOut_SendAck( TCP_TCB* pTcbTcp, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDesIP, WORD wDesPort, BOOL fSetZeroWnd );

extern	void	Tcp_DbgIn( DWORD dwState, TCP_HDR* pHdrTcp, WORD wLenTotal, BOOL fIsHost );
extern	void	Tcp_DbgOut( DWORD dwState, TCP_HDR* pHdrTcp, WORD wLenTotal, BOOL fIsHost );
//dwOption
//#define	TCPDBG_HOSTNUM			0x0001
//#define	TCPDBG_HOSTPORT			0x0001

#ifdef __cplusplus
}	
#endif

#endif	//_TCP_DEF_H_

