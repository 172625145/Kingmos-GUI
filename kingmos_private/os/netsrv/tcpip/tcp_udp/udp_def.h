/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _UDP_DEF_H_
#define _UDP_DEF_H_

#ifndef _RW_BUF_H_
#include "rwbuf.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------
//  将所有 dwIP & wPort 相同的ip数据放入该队列
//---------------------------------------------------
typedef	struct	_UDP_RXNODE
{
	HANDLE_THIS( _UDP_RXNODE );

	//RxNode链表
	LIST_UNIT			hListRxNode;  //加入UDP_TCB list

	//
	DWORD				dwTickRcv;

	//接收信息
	DWORD				dwIP;
	WORD				wPort;
	WORD				wXXX;
	//RWBUF2				stRx_RWBuf;//接收缓冲 //lilin 

	LIST_UNIT			hIPList;  //受到的IP FIFO 队列（IPDATA_IN* 结构）
	DWORD               dwAvailDataLen;// 该队列可用数据长度
	DWORD               dwDataLenLimit;// 该队列数据长度限制


	//IPDATA_IN* pDataIn;  // lilin add;

} UDP_RXNODE;

//---------------------------------------------------
// TCB
//---------------------------------------------------
struct	_UDP_GLOBAL;
typedef	struct	_UDP_GLOBAL		UDP_GLOBAL;

typedef	struct	_UDP_TCB
{
	HANDLE_THIS( _UDP_TCB );

	UDP_GLOBAL*			lpGlobalUdp;

	//TCB链表
	LIST_UNIT			hListTcb;

	//Socket对
	DWORD				dwSrcIP;
	DWORD				dwDesIP;
	WORD				wSrcPort;
	WORD				wDesPort;
	DWORD				dwSrcIP_Snd;

	//操作信息
	WORD				wAttr;		//属性标志
	WORD				wFlag;		//操作标志
	DWORD				dwOption;	//操作选项：_SOL_SOCKET级别_BOOL类型的设置
	HANDLE				hEvtPoll;	//通知 Block/Nonblock的事件
										//Block时：connect / accept / send / recv
										//Nonblock时：select
	SKERR				nErr;		//用于
	//LPBYTE				pUdpData;	//保存 每次接收计算的信息
	//DWORD				dwLenData;

	//接收缓冲
	//RWBUFFER			stRxBufRW;
	//RxNode链表
	LIST_UNIT			hListRxNode;
	CRITICAL_SECTION	csListRxNode;
	DWORD				dwLenRcvBuf;


	//直接关联到 IP层接口
	HANDLE				hNetIntf;

} UDP_TCB;

//wFlag
#define	UDPF_EXIT				0x1000	//要求退出

//dwLenRx
#define	LEN_RXBUF				4096
#define	MIN_RXBUF				2048
#define	MAX_RXBUF				0x10000

//
#define	UDPTM_WAITRECV			(5*1000)
#define	UDPTM_WAITRXNODE		(60*1000)


// ------------------------------------------------------
// UDP 全局信息
// ------------------------------------------------------
//typedef	struct	_UDP_GLOBAL
struct	_UDP_GLOBAL
{
	HANDLE_THIS( _UDP_GLOBAL );

	//所有 TCP SOCKET实例
	LIST_UNIT			hListTcb;
	CRITICAL_SECTION	csListTcb;

	//操作信息
	WORD				wFlag;		//标志
	WORD				wPort_Auto;	//自动绑定的端口

	//传输层 函数表和信息
	TL_LOCAL			LocalTl;

	//
	HANDLE				hEvtRxNode;
	HANDLE				hThrdRxNode;

} ;//UDP_GLOBAL;


extern	UDP_GLOBAL*		g_lpGlobalUdp;

extern	BOOL	Udp_CreateTcb( UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp );
extern	void	Udp_DeleteTCB( UDP_TCB* pTcbUdp );

extern	SKERR	Udp_Send( UDP_TCB* pTcbUdp, RXTXMSG* pMsgRxTx );

extern	BOOL	Udp_CreateRxNode( OUT UDP_RXNODE** ppUdpRxNode, UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort );
extern	void	Udp_DelRxNode( UDP_RXNODE* pUdpRxNode );
extern	void	Udp_CheckRxNode( UDP_TCB* pTcbUdp, BOOL fDelete );
extern	DWORD	Udp_GetRxFirstData( UDP_TCB* pTcbUdp, OUT LPBYTE pData, IN OUT DWORD* pdwLenData, OUT RXTXMSG* pMsgRxTx );
extern	BOOL	Udp_SetRxNodeLen( UDP_TCB* pTcbUdp, DWORD dwLenNew );

#ifdef __cplusplus
}	
#endif

#endif	//_UDP_DEF_H_
