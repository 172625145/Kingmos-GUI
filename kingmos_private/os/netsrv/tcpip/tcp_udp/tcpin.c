/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TCP接收数据状态机 处理
版本号：  1.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：

注释：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
//"\inc_local"
#include "proto_ip_tcp.h"
#include "netif_tcp_ip.h"
#include "netif_skinst.h"
//"local"
#include "tcp_udp.h"
#include "tcp_def.h"

typedef	SKERR	(*FNTCPIN)(TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree);

/***************  全局区 定义， 声明 *****************/

//TCP 分发数据 状态机
static	SKERR	TcpIn_Listen( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_SynSent( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_SynRcvd( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_Established( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_Closed( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_CloseWait( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_LastACK( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_FinWait1( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_FinWait2( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_Closing( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );
static	SKERR	TcpIn_TimeWait( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );

//内部辅助函数
static	BOOL	Tcp_MatchTcb( OUT TCP_TCB** ppTcbTcp, TCP_GLOBAL* lpGlobalTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp );
static	void	Tcp_ParseOpt( TCP_TCB* pTcbTcp, TCP_HDR* pHdrTcp );

static	BOOL	Tcp_IsWithinSeq( DWORD dwNum, DWORD dwValBegin, DWORD dwValEnd );
static	BOOL	Tcp_ChkAck( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp );
static	BOOL	Tcp_ChkNum( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp );
static	BOOL	Tcp_ChkSegment( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree );

static	void	Tcp_SavRxData( TCP_TCB* pTcbTcp, BOOL * lpbAutoFree );
static VOID	AddCurrentPacketToUncommitList( TCP_TCB* pTcbTcp, BOOL * lpbAutoFree );
/******************************************************/
//extern VOID IPDataIn_Free( VOID* pDataIn );
static VOID CheckUncommitPacket(TCP_TCB* pTcbTcp);
BOOL Tcp_ClearRxData( TCP_TCB* pTcbTcp );


static const FNTCPIN				pFnTcpIn[ TCPS_MAX ] = {
	TcpIn_Closed,//#define	TCPS_CLOSED				0	//关闭结束
	TcpIn_Listen,//#define	TCPS_LISTEN				1	//监听
	TcpIn_SynSent,//#define	TCPS_SYN_SENT			2	//主动SYN
	TcpIn_SynRcvd,//#define	TCPS_SYN_RECVD			3	//被动SYN
	TcpIn_Established,//#define	TCPS_ESTABLISHED		4	//连接
	TcpIn_CloseWait,//#define	TCPS_CLOSE_WAIT			5	//被动关闭
	TcpIn_LastACK,//#define	TCPS_LAST_ACK			6	//应答关闭
	TcpIn_FinWait1,//#define	TCPS_FIN_WAIT1			7	//主动关闭
	TcpIn_FinWait2,//#define	TCPS_FIN_WAIT2			8	//正常关闭
	TcpIn_Closing,//#define	TCPS_CLOSING			9	//同时关闭
	TcpIn_TimeWait,//#define	TCPS_TIME_WAIT			10	//等待关闭
//#define	TCPS_MAX				11
};
// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalTcp-TCPIP的全局信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：初始化TCP接收
// 引用: 
// ********************************************************************
BOOL	TcpIn_Init( TCP_GLOBAL* lpGlobalTcp )
{
	//填充 TCP接收状态机的函数表
	/*
	lpGlobalTcp->pFnTcpIn[TCPS_CLOSED		] = TcpIn_Closed;
	lpGlobalTcp->pFnTcpIn[TCPS_LISTEN		] = TcpIn_Listen;
	lpGlobalTcp->pFnTcpIn[TCPS_SYN_SENT	    ] = TcpIn_SynSent;
	lpGlobalTcp->pFnTcpIn[TCPS_SYN_RECVD	] = TcpIn_SynRcvd;
	lpGlobalTcp->pFnTcpIn[TCPS_ESTABLISHED  ] = TcpIn_Established;
	lpGlobalTcp->pFnTcpIn[TCPS_CLOSE_WAIT	] = TcpIn_CloseWait;
	lpGlobalTcp->pFnTcpIn[TCPS_LAST_ACK		] = TcpIn_LastACK;
	lpGlobalTcp->pFnTcpIn[TCPS_FIN_WAIT1	] = TcpIn_FinWait1;
	lpGlobalTcp->pFnTcpIn[TCPS_FIN_WAIT2	] = TcpIn_FinWait2;
	lpGlobalTcp->pFnTcpIn[TCPS_CLOSING		] = TcpIn_Closing;
	lpGlobalTcp->pFnTcpIn[TCPS_TIME_WAIT	] = TcpIn_TimeWait;
*/
	return TRUE;
}


DWORD	Tcp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pBufData, WORD wLenData )
{
	TCP_TCB*		pTcbTcp;
	TCP_HDR*		pHdrTcp;
	WORD			wHLen;
	IP_HDR* pHdrIP = &pDataIn->HdrIP;
	DWORD dwDesIP, dwSrcIP;
	WORD wDesPort, wSrcPort;
	DWORD dwRetv = 0;
	BOOL bAutoFree = TRUE;
	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
	{
		goto _return;
	}
	//检查校验码
	if( Tcp_CRC(pHdrIP->dwSrcIP, pHdrIP->dwDesIP, pHdrIP->bProType, pBufData, wLenData) )
	{
		goto _return;
	}
	pHdrTcp = (TCP_HDR*)pBufData;
	wHLen = ( (pHdrTcp->wHLenFlag & TCP_HLEN_MASK) )>>2;
	if( (wLenData<sizeof(TCP_HDR)) || (wHLen<sizeof(TCP_HDR)) || (wHLen>wLenData) )
	{
		goto _return;
	}

	//寻找 目标TCB
	if( !Tcp_MatchTcb(&pTcbTcp, g_lpGlobalTcp, pHdrIP, pHdrTcp) )
	{
		//发送RST通知对方，并忽略
		TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, ntohl(pHdrTcp->dwSeqNum) );
		goto _return;
	}

	//
	//提交给状态机处理
	//
	EnterCriticalSection( &pTcbTcp->csTcb );

	//保存 和 转换相关信息
#ifndef BIG_END
	pHdrTcp->dwSeqNum = ntohl( pHdrTcp->dwSeqNum );
	pHdrTcp->dwAckNum = ntohl( pHdrTcp->dwAckNum );
	pHdrTcp->wWinSize = ntohs( pHdrTcp->wWinSize );
#endif
	pTcbTcp->wLenHdr = wHLen;
	pTcbTcp->wLenData = wLenData - wHLen;
	pTcbTcp->pTcpData = pBufData + wHLen;
	//将 IPDATA_IN 包转化为TCPDATA_IN包
	pTcbTcp->pCurDataIn = pDataIn;
	

	//处理数据
	
	dwDesIP = pHdrIP->dwDesIP;
	dwSrcIP = pHdrIP->dwSrcIP;
	
	wDesPort = pHdrTcp->wDesPort;
	wSrcPort = pHdrTcp->wSrcPort;
	//以下功能可能改变 报数据

	//HeapValidate( GetProcessHeap(), 0, NULL );
	//(*pTcbTcp->lpGlobalTcp->pFnTcpIn[pTcbTcp->dwState])( pTcbTcp, pHdrIP, pHdrTcp );
	
	pFnTcpIn[pTcbTcp->dwState]( pTcbTcp, pHdrIP, pHdrTcp, &bAutoFree );
	//HeapValidate( GetProcessHeap(), 0, NULL );
//hggg  ghgh
	//
	if( pTcbTcp->wFlag & TCPF_NEEDACK )
	{
		//发送 “经过时延的ACK”
		pTcbTcp->wFlag &= ~TCPF_NEEDACK;
		if( !(pTcbTcp->wFlag & TCPF_SENDING) )
		{	//lilin 
			//TcpOut_SendAck( pTcbTcp, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, FALSE );
			TcpOut_SendAck( pTcbTcp, dwDesIP, wDesPort, dwSrcIP, wSrcPort, FALSE );
		}
	}
	

	//
	LeaveCriticalSection( &pTcbTcp->csTcb );

_return:

	if( bAutoFree )
	{	//自动释放
		IPDataIn_Free( pDataIn );
	}

	return dwRetv;
}


void	TcpIn_IcmpNotify( BYTE bType, BYTE bCode, IP_HDR* pHdrIP_icmp, WORD wLenData )
{
	TCP_HDR*		pHdrTcp;
	TCP_TCB*		pTcbTcp;
	WORD			wLenHdr;

	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
	{
		return ;
	}
	//保证 wLenData>=(sizeof(IP_HDR)+sizeof(TCP_HDR))
	wLenHdr = ((pHdrIP_icmp->bVerHLen & IPL_MASK)<<2);
	if( wLenData<(wLenHdr + sizeof(TCP_HDR)) )
	{
		return ;
	}
	//寻找该端口
	pHdrTcp = (TCP_HDR*)((LPBYTE)pHdrIP_icmp + wLenHdr);
	if( !Tcp_MatchTcb( &pTcbTcp, g_lpGlobalTcp, pHdrIP_icmp, pHdrTcp ) )
	{
		return ;
	}

	//分析ICMP的信息
	switch( bType )
	{
	case ICMP_TYPE_DES_UNREACH:
		switch( bCode )
		{
		case ICMP_CODE_DESUR_NET	:
			pTcbTcp->nErr = E_NETUNREACH;
			break;
		case ICMP_CODE_DESUR_HOST	:
		case ICMP_CODE_DESUR_PROTOCOL:
			pTcbTcp->nErr = E_HOSTUNREACH;
			break;
		case ICMP_CODE_DESUR_PORT	:
			pTcbTcp->nErr = E_CONNRESET;
			break;
		case ICMP_CODE_DESUR_FRAGMENT:
		case ICMP_CODE_DESUR_ROUTE	:
		case ICMP_CODE_DESUR_UNNET	:
		case ICMP_CODE_DESUR_UNHOST	:
		default :
			break;
		}
		PulseEvent( pTcbTcp->hEvtPoll );

	case ICMP_TYPE_SRC_QUENCH:
		break;

	default :
		break;
	}

}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：监听时，处理接收的信息
//1。收RST|FIN时，忽略
//2。收“有ACK 或者 没有SYN”时，发送RST通知对方，并忽略
//3。检查最大的连接数
//4。查询 IP层网络信息 和 本地MSS
//5。分配新TCB结构，用于表示1个新连接
//6。初始化TCB：状态、本地MSS、Socket对
//7。保存对方传过来的信息：SEQ、选项、
//8。启动新TCB的发送线程，通知发送 (SYN | ACK)
// 引用: 
// ********************************************************************
SKERR	TcpIn_Listen( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	TCP_GLOBAL*		lpGlobalTcp;
	TCP_TCB*		pTcbTcp_New;
	WORD			wSnd_MSS;

	//收RST|FIN时，忽略
	if( pHdrTcp->wHLenFlag & (TCP_FLAG_RST | TCP_FLAG_FIN) )
	{
		//忽略
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Wrong wHLenFlag1=[RST FIN]\r\n")));
		return 0;
	}
	//收“有ACK 或者 没有SYN”时，发送RST通知对方，并忽略
	if( (pHdrTcp->wHLenFlag & TCP_FLAG_ACK) || !(pHdrTcp->wHLenFlag & TCP_FLAG_SYN) )
	{
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Wrong wHLenFlag2=[ACK]\r\n")));
		//发送RST通知对方，并忽略
		TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );
		return 0;
	}

	//检查最大的连接数
	if( pTcbTcp->wCntConn>=pTcbTcp->wMaxConn )
	{
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: wCntConn=[%d], wMaxConn=[%d]\r\n"), pTcbTcp->wCntConn, pTcbTcp->wMaxConn));
		//发送RST通知对方，并忽略---因为这是1个软错误，所以不要发RST，请见 V1-Page197
		//TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort );
		return 0;
	}
	//查询 IP层网络信息 和 本地MSS
	if( !Tcp_QueryMSS(pHdrIP->dwDesIP, pHdrIP->dwSrcIP, &wSnd_MSS) )
	{
		BYTE*		pIP = (BYTE*)&pHdrIP->dwDesIP;
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Wrong Des IP=[%d.%d.%d.%d]\r\n"), *pIP, pIP[1], pIP[2], pIP[3]));
		//该IP地址无效
		//return E_ADDRNOTAVAIL;
		return 0;
	}

	//分配新TCB结构，用于表示1个新连接
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( !Tcp_CreateTcb( &pTcbTcp_New, lpGlobalTcp ) )
	{
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Failed to Tcp_CreateTcb\r\n")));
		return 0;
	}

	//初始化TCB：状态、本地MSS、Socket对
	pTcbTcp_New->dwState = TCPS_SYN_RECVD;
	//pTcbTcp_New->wAttr |= TCPA_SYNRCVD; //for debug

	pTcbTcp_New->wSnd_MSS = wSnd_MSS;
	pTcbTcp_New->wRcv_MSS = wSnd_MSS;

	pTcbTcp_New->dwSrcIP = pHdrIP->dwDesIP;
	pTcbTcp_New->dwDesIP = pHdrIP->dwSrcIP;
	pTcbTcp_New->wSrcPort = pHdrTcp->wDesPort;
	pTcbTcp_New->wDesPort = pHdrTcp->wSrcPort;

	pTcbTcp_New->wRcv_wnd = pHdrTcp->wWinSize;

	pTcbTcp_New->wLenHdr = pTcbTcp->wLenHdr;
	pTcbTcp_New->wLenData = pTcbTcp->wLenData;

	//保存对方传过来的信息：SEQ、选项、
	pTcbTcp_New->wFlag |= TCPF_RSYN;	//收到对方的SYN
	pTcbTcp_New->dwRcv_nxt = pHdrTcp->dwSeqNum + 1;
	Tcp_ParseOpt( pTcbTcp_New, pHdrTcp );

	//关联到 accept的 hListSynRcvd队列 上，
	pTcbTcp_New->pTcbParent = pTcbTcp;
	pTcbTcp_New->dwTickLife = GetTickCount();
	List_InsertTail( &pTcbTcp->hListSynRcvd, &pTcbTcp_New->hListSynRcvd );
	pTcbTcp->wCntConn ++;

	//关联到
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	List_InsertTail( &lpGlobalTcp->hListTcb, &pTcbTcp_New->hListTcb );
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	//启动新TCB的发送线程
	if( !TcpOut_StartSend( pTcbTcp_New ) )
	{
		//一般情况都会成功的！
		Tcp_DeleteTCB( pTcbTcp_New );
		return 0;
	}

	RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: yes pTcbTcp_New=[0x%x]\r\n"), pTcbTcp_New));
	//通知发送 (SYN | ACK)
	SetEvent( pTcbTcp_New->hEvtSend );

	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：主动SYN时，处理接收的信息
//1。收RST|FIN时，直接退出，并通知connect/select
//2。收ACK时，判断ACK是否合理---支持同时打开(please see Figure 8)
//3。更新wRcv_wnd
//4。收SYN时，保存对方信息---支持同时打开(please see Figure 8)
//5。判断 是否完成连接，进入连接状态、通知connect、接收数据
// 引用: 
// ********************************************************************
SKERR	TcpIn_SynSent( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//收RST|FIN时，直接退出---理由：在没有建立连接前收到RST|FIN，可以重试connect
	if( pHdrTcp->wHLenFlag & (TCP_FLAG_RST | TCP_FLAG_FIN) )
	{
		//通知connect/select，退出当前操作，并且可以让用户继续操作
		pTcbTcp->dwState = TCPS_CLOSED;
		pTcbTcp->wFlag = TCPQ_CNN_NO;
		pTcbTcp->nErr = E_CONNREFUSED;
		PulseEvent( pTcbTcp->hEvtPoll );
		return 0;
	}

	//收ACK时，判断ACK是否合理---支持同时打开(please see Figure 8)
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		//保证对方传过来的ACK是正确的
		if( Tcp_ChkAck(pTcbTcp, pHdrIP, pHdrTcp) )
		{
			pTcbTcp->wFlag |= TCPF_LSYN;	//收到对方的SYN's ACK
		}
		else
		{
			//发送RST通知对方，并忽略---理由：参考“网际互连”V2-P193
			TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );
			return 0;
		}
	}

	//更新wRcv_wnd
	pTcbTcp->wRcv_wnd = pHdrTcp->wWinSize;

	//收SYN时，保存对方信息---支持同时打开(please see Figure 8)
	if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
	{
		//保存对方传过来的信息：SEQ、选项、
		pTcbTcp->wFlag |= TCPF_RSYN;	//收到对方的SYN
		pTcbTcp->dwRcv_nxt = pHdrTcp->dwSeqNum + 1;
		Tcp_ParseOpt( pTcbTcp, pHdrTcp );

		//通知发送 “经过时延的ACK”和 SYN
		pTcbTcp->wFlag |= TCPF_NEEDACK;
	}

	//判断 是否完成连接，进入连接状态、通知connect、接收数据
	if( (pTcbTcp->wFlag & TCPF_ALLSYN)==TCPF_ALLSYN )
	{
		//进入连接，并通知connect/select
		pTcbTcp->dwState = TCPS_ESTABLISHED;

		pTcbTcp->lpGlobalTcp->dwISN += 64000;	//参考 “详解”V1-P
		pTcbTcp->wFlag |= TCPQ_CNN_YES;
		pTcbTcp->wAttr |= (TCPA_ESTABLISH | TCPA_RECV | TCPA_SEND);
		PulseEvent( pTcbTcp->hEvtPoll );		//到连接后通知

		//如果有数据
		if( pTcbTcp->wLenData )
		{
			Tcp_SavRxData( pTcbTcp, lpbAutoFree );

			//通知发送 “经过时延的ACK”
			pTcbTcp->wFlag |= TCPF_NEEDACK;
		}
	}

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：被动SYN时，处理接收的信息
//1。收RST|FIN时，直接退出
//2。收SYN时，先快速发送ACK(please see Figure 9)
//3。判断 SEQ是否合理
//3。更新wRcv_wnd
//4。要求收ACK，而且保证ACK_NUM是正确的
//5。更新发送SEQ信息、进入连接状态、通知accept
//6。收FIN时，进入被动关闭状态
// 引用: 
// ********************************************************************
SKERR	TcpIn_SynRcvd( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	TCP_TCB*		pTcbParent;

	//句柄安全检查
	pTcbParent = pTcbTcp->pTcbParent;
	if( !HANDLE_CHECK(pTcbParent) )
	{
		pTcbTcp->wFlag = TCPF_EXIT;
		PulseEvent( pTcbParent->hEvtPoll );
		return 0;
	}
	//收RST|FIN时，直接退出---在没有建立连接前收到RST|FIN，是要直接退出的
	if( pHdrTcp->wHLenFlag & (TCP_FLAG_RST|TCP_FLAG_FIN) )
	{
		//直接退出该Socket---不需要发送 “经过时延的ACK”
		pTcbTcp->wFlag = TCPF_EXIT;
		PulseEvent( pTcbParent->hEvtPoll );
		return 0;
	}
	//收SYN时，先快速发送ACK(please see Figure 9)
	if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
	{
		//通知发送 SYN|ACK
		//SetEvent( pTcbTcp->hEvtSend );
		pTcbTcp->wFlag |= TCPF_NEEDACK;
		return 0;
	}
	//判断 SEQ是否合理---就是这么肯定原来的SEQ！
	if( pHdrTcp->dwSeqNum!=pTcbTcp->dwRcv_nxt )
	{
		//忽略
		//发送RST通知对方，并忽略
		//TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort );
		return 0;
	}
	//更新wRcv_wnd
	pTcbTcp->wRcv_wnd = pHdrTcp->wWinSize;

	//要求收ACK，而且保证ACK_NUM是正确的
	if( !(pHdrTcp->wHLenFlag & TCP_FLAG_ACK) )
	{
		//忽略
		return 0;
	}
	if( !Tcp_ChkAck(pTcbTcp, pHdrIP, pHdrTcp) )
	{
		//发送RST通知对方，并忽略
		TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );
		return 0;
	}
	
	//更新发送SEQ信息、进入连接状态、通知accept
	pTcbTcp->wFlag |= TCPF_LSYN;

	//进入连接，并通知accept/select
	pTcbTcp->dwState = TCPS_ESTABLISHED;

	pTcbParent->wFlag |= TCPQ_LISTEN_YES;
	pTcbTcp->wAttr |= (TCPA_ESTABLISH | TCPA_RECV | TCPA_SEND);
	PulseEvent( pTcbParent->hEvtPoll );		//到连接后通知

	RETAILMSG(1,(TEXT(" \r\n TcpIn_SynRcvd: yes pTcbTcp_New=[0x%x]\r\n"), pTcbTcp));
	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：连接时，处理接收的信息
//1。判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
//2。收FIN时，进入被动关闭状态
//3。收SYN时，半打开连接(please see Figure 10)，不能保存SEQ
// 引用: 
// ********************************************************************
SKERR	TcpIn_Established( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp, lpbAutoFree) )
	{
		//退出 或者 忽略
		return 0;
	}
	//收FIN时，进入被动关闭状态
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//进入TCPS_CLOSE_WAIT
		pTcbTcp->dwState = TCPS_CLOSE_WAIT;
		pTcbTcp->wFlag |= TCPF_RFIN;		//收到对方的FIN
		pTcbTcp->wAttr |= TCPA_CONNRESET;
		pTcbTcp->dwRcv_nxt ++;
		//通知发送 “经过时延的ACK”
		pTcbTcp->wFlag |= TCPF_NEEDACK;
	}
	//收SYN时，半打开连接(please see Figure 10)，不能保存SEQ
	if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
	{
		pTcbTcp->dwRcv_nxt ++;
		//通知发送 “经过时延的ACK”
		pTcbTcp->wFlag |= TCPF_NEEDACK;

		//注意！！！
		//pTcbTcp->dwState = TCPS_CLOSED;
		//发送RST通知对方，并忽略
		//TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort );
	}

	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：关闭时，处理接收的信息
//1。收RST时：直接退出
//2。发送RST通知对方，并忽略
// 引用: 
// ********************************************************************
SKERR	TcpIn_Closed( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	//收RST时：直接退出
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//直接退出该Socket---不需要发送 “经过时延的ACK”
		return 0;
	}
	//发送RST通知对方，并忽略
	TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：被动关闭时，处理接收的信息
//1。收RST时：直接退出
//2。此时不再接收FIN，因为pTcbTcp->dwRcv_nxt已经变化，会出现SEQ不合理。
// 引用: 
// ********************************************************************
SKERR	TcpIn_CloseWait( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp, lpbAutoFree) )
	{
		//忽略
		return 0;
	}

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：被动关闭时，处理接收的信息
//1。收RST时：直接退出
//2。希望收正确的ACK
// 引用: 
// ********************************************************************
SKERR	TcpIn_LastACK( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp, lpbAutoFree) )
	{
		//忽略
		return 0;
	}
	//分析 TCP HEADER---同时关闭：希望收ACK
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		pTcbTcp->dwState = TCPS_CLOSED;
		pTcbTcp->wFlag |= TCPF_LFIN;	//收到对方的FIN's ACK
	}

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：主动关闭时，处理接收的信息
//1。收RST时：直接退出
//2。收FIN时：进入同时关闭
//3。当只收ACK时：进入正常关闭
//4。通知发送 ACK
// 引用: 
// ********************************************************************
SKERR	TcpIn_FinWait1( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//收RST时：直接退出
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//直接退出该Socket---不需要发送 “经过时延的ACK”
		return 0;
	}
	//判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp,lpbAutoFree) )
	{
		//忽略
		return 0;
	}
	//分析 TCP HEADER---同时关闭
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//进入TCPS_CLOSING---同时关闭状态
		pTcbTcp->dwState = TCPS_CLOSING;
		//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
		pTcbTcp->wFlag |= TCPF_RFIN;		//收到对方的FIN
		pTcbTcp->dwRcv_nxt ++;
		//通知发送 “经过时延的ACK”
		pTcbTcp->wFlag |= TCPF_NEEDACK;

		return 0;
	}
	//分析 TCP HEADER---正常关闭
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		pTcbTcp->dwState = TCPS_FIN_WAIT2;
		pTcbTcp->wFlag |= TCPF_LFIN;	//收到对方的FIN's ACK
	}

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：正常关闭时，处理接收的信息
//1。收RST时：直接退出
//2。收FIN时：进入等待关闭状态
//3。通知发送 ACK
// 引用: 
// ********************************************************************
SKERR	TcpIn_FinWait2( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp,lpbAutoFree) )
	{
		//忽略
		return 0;
	}
	//分析 TCP HEADER---正常关闭
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//进入TCPS_TIME_WAIT---等待关闭状态
		pTcbTcp->dwState = TCPS_TIME_WAIT;
		//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
		pTcbTcp->wFlag |= TCPF_RFIN;//收到对方的FIN
		pTcbTcp->dwRcv_nxt ++;
		//通知发送 “经过时延的ACK”
		pTcbTcp->wFlag |= TCPF_NEEDACK;

		return 0;
	}

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：同时关闭时，处理接收的信息
//1。收RST时：直接退出
//2。希望收正确的ACK，进入等待关闭状态
// 引用: 
// ********************************************************************
SKERR	TcpIn_Closing( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	//判断 收RST时直接退出、SEQ和ACK是否合理，并保存WinSize信息、更新发送信息、接收数据
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp,lpbAutoFree) )
	{
		//忽略
		return 0;
	}
	//分析 TCP HEADER---同时关闭：希望收ACK
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		pTcbTcp->dwState = TCPS_TIME_WAIT;
		pTcbTcp->wAttr |= TCPA_CLOSING;	//表示是从同时关闭的状态切换过来
		pTcbTcp->wFlag |= TCPF_LFIN;	//收到对方的FIN's ACK
	}

	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：等待关闭时，处理接收的信息
//1。收RST时：直接退出
//2。不接收其它任何数据，在2MSL等待时间内，如果是正常关闭，则发送ACK
// 引用: 
// ********************************************************************
SKERR	TcpIn_TimeWait( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	//收RST时：直接退出
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//直接退出该Socket---不需要发送 “经过时延的ACK”
		return 0;
	}
	//判断对方的 SEQ和ACK 是否合理
	if( !Tcp_ChkNum(pTcbTcp, pHdrIP, pHdrTcp) )
	{
		//忽略
		return FALSE;
	}
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//重发 Pure ACK，通知对方
		TcpOut_SendAck( pTcbTcp, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, TRUE );
	}

	return 0;
}







/////////////////////////////////////////////////////////////////////
//
//内部辅助函数
//
/////////////////////////////////////////////////////////////////////


// ********************************************************************
// 声明：
// 参数：
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
//	IN lpGlobalTcp-TCPIP的全局信息
//	IN/OUT ppTcbTcp-要匹配的TCB
// 返回值：
//	
// 功能描述：匹配pHdrIP和pHdrTcp指定的TCB
// 引用: 
// ********************************************************************
BOOL	Tcp_MatchTcb( OUT TCP_TCB** ppTcbTcp, TCP_GLOBAL* lpGlobalTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp )
{
	static TCP_TCB* pTcbTcp_Cache = NULL;

	TCP_TCB*		pTcbTcp_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppTcbTcp = NULL;
	//lilin modify
	//pUnitHeader = &lpGlobalTcp->hListTcb;
	//EnterCriticalSection( &lpGlobalTcp->csListTcb );
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	//lilin add code
	//句柄安全寻找
	/*
	if( pTcbTcp_Cache &&
		HANDLE_CHECK(pTcbTcp_Cache) && 
		(pTcbTcp_Cache->wFlag & TCPF_EXIT) == 0 &&
		pTcbTcp_Cache->wSrcPort==pHdrTcp->wDesPort )
	{
		//地址比较---
		if( (pTcbTcp_Cache->dwSrcIP==pHdrIP->dwDesIP) && 
			(pTcbTcp_Cache->dwDesIP==pHdrIP->dwSrcIP) && 
			(pTcbTcp_Cache->wDesPort==pHdrTcp->wSrcPort)
			)
		{
			*ppTcbTcp = pTcbTcp_Cache;
			LeaveCriticalSection( &lpGlobalTcp->csListTcb );
			return TRUE;
		}
	}
	*/
	//lilin-end

	pUnitHeader = &lpGlobalTcp->hListTcb;
	//
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(lpGlobalTcp->wFlag & TCPIPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pTcbTcp_Find = LIST_CONTAINER( pUnit, TCP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 

		//句柄安全寻找
		if( !HANDLE_CHECK(pTcbTcp_Find) )
		{
			ASSERT( 0 );
			break;
		}
		if( pTcbTcp_Find->wFlag & TCPF_EXIT )
		{
			continue;
		}
		//本地端口比较
		if( pTcbTcp_Find->wSrcPort==pHdrTcp->wDesPort )
		{
			//地址比较---
			if( (pTcbTcp_Find->dwSrcIP==pHdrIP->dwDesIP) && 
				(pTcbTcp_Find->dwDesIP==pHdrIP->dwSrcIP) && 
				(pTcbTcp_Find->wDesPort==pHdrTcp->wSrcPort)
				)
			{
				*ppTcbTcp = pTcbTcp_Find;
				pTcbTcp_Cache = pTcbTcp_Find; //lilin add code
				break;
			}
			//监听socket的特征--- [ 源端 IP(固定或ANY)，PORT(固定) ] --- [ 目的端 (ANY) ]
			else if( pTcbTcp_Find->dwState==TCPS_LISTEN  )
			{
				if( (pTcbTcp_Find->dwSrcIP==pHdrIP->dwDesIP) || (pTcbTcp_Find->dwSrcIP==INADDR_ANY) )
				{
					*ppTcbTcp = pTcbTcp_Find;
				}
			}
		}
	}
	//
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	return (*ppTcbTcp)?TRUE:FALSE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：分析pHdrTcp数据中的选项，并存入pTcbTcp
// 引用: 
//注释：
//option format:
//octet---0----------1--------2~n
//       type   All_length   data
// ********************************************************************
void	Tcp_ParseOpt( TCP_TCB* pTcbTcp, TCP_HDR* pHdrTcp )
{
	DWORD	dwLen;
	LPBYTE	pData;
	LPBYTE	pDataEnd;
	WORD	wMSS;
	BYTE	bLen;

	dwLen = pTcbTcp->wLenHdr;
	if( !dwLen || dwLen>(sizeof(TCP_HDR)+40) )
	{
		return;
	}
	pData = (LPBYTE)pHdrTcp+sizeof(TCP_HDR);
	pDataEnd = pData + dwLen;
	while( pData < pDataEnd )
	{
		switch( *pData )
		{
		case TCP_OPT_END:
			return;
		case TCP_OPT_SKIP:
			++ pData;
			break;
			
		case TCP_OPT_MSS:
			if( pData[1]!=TCP_OPT_MSS_LEN )
				return;
			//reference RFC1122 4.2.2.6
			wMSS = (WORD)pntohs( pData+2 );
			if( 0 == wMSS )
			{
				pTcbTcp->wRcv_MSS = MSS_DEFAULT;
			}
			else if( wMSS < pTcbTcp->wRcv_MSS )
			{
				pTcbTcp->wRcv_MSS = wMSS;
			}
			pData += TCP_OPT_MSS_LEN;
			break;

		default:
			bLen = pData[1];
			if( bLen < 2 )
			{
				return;
			}
			pData += bLen;
			break;
		}
	}
	//return ;
}

// ********************************************************************
// 声明：
// 参数：
//	IN dwNum-待检查的数值
//	IN dwValBegin-待检查值范围的起始位置
//	IN dwValEnd-待检查值范围的结束位置
// 返回值：
//	
// 功能描述：判断 dwNum是不是在 “[dwValBegin, dwValEnd]之内” 或者 “(dwValEnd, dwValBegin)之外”
// 引用: 
//注释：
//如果[90, 100]---[90, 100]，则90, 91, 92, ... 100都成立，     89、101都失败
//如果[900, 10]---(10, 900)，则900, 901, ..., 9, 10都成立，    899、11都失败
// ********************************************************************
BOOL	Tcp_IsWithinSeq( DWORD dwNum, DWORD dwValBegin, DWORD dwValEnd )
{
	//保证在
	//
	if( dwValEnd >= dwValBegin )
	{
		if( (dwNum < dwValBegin) || (dwNum > dwValEnd) )
		{
			return FALSE;
		}
	}
	else
	{
		if( (dwNum < dwValBegin) && (dwNum > dwValEnd) )
		{
			return FALSE;
		}
	}

	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：判断接收的ACK 是否合理，并做相应的设置
// 引用: 
// ********************************************************************
BOOL	Tcp_ChkAck( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp )
{
	//if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		if( (pTcbTcp->dwState==TCPS_ESTABLISHED) || 
			(pTcbTcp->dwState==TCPS_CLOSE_WAIT) ||
			(pTcbTcp->dwState==TCPS_FIN_WAIT2) || 
			(pTcbTcp->dwState==TCPS_TIME_WAIT)// || 
			//((pTcbTcp->dwState==TCPS_FIN_WAIT1) && (pHdrTcp->wHLenFlag & TCP_FLAG_FIN))
		  )
		{
			//保证dwAckNum应该在“只能接收的ACK的范围”内，
			//---即在 “[dwSnd_una, dwSnd_real]之内” 或者 “(dwSnd_real, dwSnd_una)之外”
			//if( !Tcp_IsWithinSeq( pHdrTcp->dwAckNum, pTcbTcp->dwSnd_una, pTcbTcp->dwSnd_max ) )
			if( !Tcp_IsWithinSeq( pHdrTcp->dwAckNum, pTcbTcp->dwSnd_una, pTcbTcp->dwSnd_real ) )
			{
				//忽略
				return FALSE;
			}
		}
		else
		{
			//保证dwAckNum应该在“只能接收的ACK的范围”内，
			//---即在 “[dwSnd_una+1, dwSnd_real+1]之内” 或者 “(dwSnd_real+1, dwSnd_una+1)之外”
			//象 SYN-RECVED  SYN-SENT  FIN-WAIT1(正常关闭)  CLOSING  LAST-ACK都是
			//如果它代表1个数据段的接收，则有可能是在 “[dwSnd_una+1, dwSnd_real+1]之内” 或者 “(dwSnd_real+1, dwSnd_una+1)之外”
			if( !Tcp_IsWithinSeq( pHdrTcp->dwAckNum, pTcbTcp->dwSnd_una+1, pTcbTcp->dwSnd_real+1 ) )
			//if( pHdrTcp->dwAckNum!=(pTcbTcp->dwSnd_nxt+1) )
			{
				//忽略
				return FALSE;
			}

			//更新发送信息
			pTcbTcp->dwSnd_nxt ++;
			pTcbTcp->dwSnd_real ++;
			pTcbTcp->dwSnd_una ++;
			pTcbTcp->dwSnd_max ++;
		}
	}

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：判断对方的SEQ是否合理，判断接收的ACK 是否合理，并做相应的设置
// 引用: 
// ********************************************************************
BOOL	Tcp_ChkNum( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp )
{
	DWORD	dwRcv_max;

	//判断对方的SEQ是否合理---它应该在“只能接收的SEQ序列号的范围”内，
	//---保证dwSeqNum在 “[dwRcv_nxt, dwRcv_max]之内” 或者 “(dwRcv_max, dwRcv_nxt)之外”
	dwRcv_max = pTcbTcp->dwRcv_nxt + pTcbTcp->wSnd_wnd_Last;
	if( !Tcp_IsWithinSeq( pHdrTcp->dwSeqNum, pTcbTcp->dwRcv_nxt, dwRcv_max ) )
	{
		//忽略
		return FALSE;
	}

	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		if( !Tcp_ChkAck(pTcbTcp, pHdrIP, pHdrTcp) )
		{
			//重发 Pure ACK，通知对方
			TcpOut_SendAck( pTcbTcp, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, FALSE );
			//pTcbTcp->wFlag |= TCPF_NEEDACK;
			//忽略
			return FALSE;
		}
	}

	//更新wRcv_wnd
	pTcbTcp->wRcv_wnd = pHdrTcp->wWinSize;
	if( (pTcbTcp->wFlag & TCPF_NEEDPERSIST) && pTcbTcp->wRcv_wnd )
	{
		pTcbTcp->wFlag &= ~TCPF_NEEDPERSIST;
		SetEvent( pTcbTcp->hEvtSend );
	}

	return TRUE;
}
// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrIP-接收的数据包IP Header
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：判断RST、SEQ、ACK，并更新发送信息、保存接收的数据，同时计算快速重传和RTO时间
// 引用: 
// ********************************************************************
BOOL	Tcp_ChkSegment( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp,BOOL * lpbAutoFree )
{
	DWORD	dwTmp;

	//收RST时：直接退出
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//直接退出该Socket---不需要发送 “经过时延的ACK”
		pTcbTcp->dwState = TCPS_CLOSED;
		//pTcbTcp->wFlag |= TCPF_EXIT;
		pTcbTcp->wFlag = 0;		//---让用户在该socket上，可以继续重试连接
		pTcbTcp->wAttr |= TCPA_CONNRESET;

		//通知
		pTcbTcp->nErr = E_CONNRESET;//E_CONNREFUSED
		Tcp_ClearRxData( pTcbTcp );
		PulseEvent( pTcbTcp->hEvtPoll );
		return FALSE;
	}
	//判断对方的 SEQ和ACK 是否合理
	if( !Tcp_ChkNum(pTcbTcp, pHdrIP, pHdrTcp) )
	{
		//忽略
		return FALSE;
	}
	//根据ACK，更新发送信息
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		//检查ACK
		if( pHdrTcp->dwAckNum==pTcbTcp->dwSnd_una )
		{
			//---计算快速重传的计数
			//if( (pTcbTcp->dwSnd_una!=pTcbTcp->dwSnd_nxt) && !pTcbTcp->wLenData )
			if( (pTcbTcp->dwSnd_una!=pTcbTcp->dwSnd_nxt) )
			{
				pTcbTcp->wSnd_quick ++;
				if( pTcbTcp->wSnd_quick>=TCPQK_MAX )
				{
					//请进行快速重传
					SetEvent( pTcbTcp->hEvtSend );
					pTcbTcp->wSnd_quick = 0;
				}
			}
		}
		else
		{
			pTcbTcp->wSnd_quick = 0;
			//计算 RTT和RTO
			dwTmp = GetTickCount() - pTcbTcp->dwTickRTT;
			dwTmp *= 2;
			if( dwTmp<TCPTM_SEND_MIN )
			{
				dwTmp = TCPTM_SEND_MIN;
			}
			else if( dwTmp>TCPTM_SEND_MAX )
			{
				dwTmp = TCPTM_SEND_MAX;
			}
			pTcbTcp->dwRTO = dwTmp;

			EnterCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );
			//计算 对方接收的字节数
			dwTmp = pHdrTcp->dwAckNum - pTcbTcp->dwSnd_una;//这个是正确的如： (1 - MAXDWORD)==2
			//---更新发送缓冲的信息
			pTcbTcp->dwSnd_una = pHdrTcp->dwAckNum;
			//
			RWBuf_SetReadOffset( &pTcbTcp->stTxBufRW, dwTmp, FALSE );
			//
			LeaveCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );
		}
	}
	//判断序列号，保存接收的数据
	if( pHdrTcp->dwSeqNum==pTcbTcp->dwRcv_nxt )
	{
		//if( pTcbTcp->wLenData && !(pTcbTcp->wAttr & TCPA_NORECV) )
//		RETAILMSG( 1, ( "pHdrTcp->dwSeqNum=0x%x,%u,pTcbTcp->wLenData=%d.\r\n", pHdrTcp->dwSeqNum, pHdrTcp->dwSeqNum, pTcbTcp->wLenData ) );
		if( pTcbTcp->wLenData && (pTcbTcp->wAttr & TCPA_RECV) )
		{
			Tcp_SavRxData( pTcbTcp, lpbAutoFree );

		}
		//启动“经过时延的ACK”
		pTcbTcp->wFlag |= TCPF_NEEDACK;
	}
	else if( pTcbTcp->wLenData )
	{
		//请立即发送1个 Pure ACK 通知对方，收到1个乱序的数据包！
		//启动“经过时延的ACK”
		pTcbTcp->wFlag |= TCPF_NEEDACK;
		//lilin
		if( pTcbTcp->wAttr & TCPA_RECV )
		{
			AddCurrentPacketToUncommitList(pTcbTcp, lpbAutoFree);//lilin add code
		}
	}

	return TRUE;
}

static int GetPackets( PLIST_UNIT		pUnit )
{
	PLIST_UNIT		pUnitHeade = pUnit;
	int c = 0;

	while( pUnit->pNext != pUnitHeade )
	{
		c++;
		pUnit = pUnit->pNext;
	}
	return c;
}

static void CheckPacket( TCP_TCB* pTcbTcp )
{
	int iCommit = GetPackets( &pTcbTcp->hCommitPacketDataIn );
	int iunCommit = GetPackets( &pTcbTcp->hUnCommitPacketDataIn );

	if( iCommit + iunCommit != pTcbTcp->lPackCount )
	{
		iCommit = iCommit;
		RETAILMSG( 1, ( "lPackCount error:lPackCount=%d,iCommit=%d,iunCommit=%d.\r\n", pTcbTcp->lPackCount,iCommit,iunCommit ) );
	}

}

static void AddPacketToList( TCP_TCB* pTcbTcp, LIST_UNIT * pHead, TCPDATA_IN*	pTcpDataIn, BOOL bTail )
{
	//CheckOutPtr( __FILE__, __LINE__ );
//	CheckPacket( pTcbTcp );
	pTcpDataIn->pThis = pTcpDataIn;
	InterlockedIncrement( &pTcbTcp->lPackCount );
	//RETAILMSG( 1, ( "lPackCount add=%d.\r\n", pTcbTcp->lPackCount ) );
	
	if( bTail )
	{
	    List_InsertTail( pHead, &pTcpDataIn->hListDataIn );
	}
	else
	{
		List_InsertHead( pHead, &pTcpDataIn->hListDataIn );
	}
//	CheckPacket( pTcbTcp );
	//CheckOutPtr( __FILE__, __LINE__ );
}

static FreePacketFromList( TCP_TCB* pTcbTcp, TCPDATA_IN*	pTcpDataIn )
{
	//CheckOutPtr( __FILE__, __LINE__ );
//	CheckPacket( pTcbTcp );

	List_RemoveUnit( &pTcpDataIn->hListDataIn );
	ASSERT( pTcpDataIn->pThis == pTcpDataIn );
	pTcpDataIn->pThis = NULL;
	IPDataIn_Free( (IPDATA_IN*)pTcpDataIn );
	InterlockedDecrement( &pTcbTcp->lPackCount );
	ASSERT( pTcbTcp->lPackCount >= 0 );

	//CheckOutPtr( __FILE__, __LINE__ );
//	CheckPacket( pTcbTcp );
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：保存接收的数据，并更新接收信息
// 引用: 
// ********************************************************************
#define SAVRX_NOSPACE 1
#define SAVRX_OK      0
static VOID	_Tcp_SavRxPacket( TCP_TCB* pTcbTcp,  TCPDATA_IN * pTCPDataIn )
{
	//CheckOutPtr( __FILE__, __LINE__ );
	EnterCriticalSection( &pTcbTcp->csRx );

	
	//List_InsertTail( &pTcbTcp->hCommitPacketDataIn, &pTCPDataIn->hListDataIn );
	AddPacketToList( pTcbTcp, &pTcbTcp->hCommitPacketDataIn, pTCPDataIn, TRUE );
	//请在此处计算判断 SWS情况

	//存储数据
	//dwLenData = pTcbTcp->wLenData;
	//
	//设置回答的ACK序列号
	//
	pTcbTcp->dwRcv_nxt += pTCPDataIn->wLenData;
//	pTCPDataIn->bAutoFree = 0;
	pTcbTcp->dwUsedRxBufLen += pTCPDataIn->wLenData;
	LeaveCriticalSection( &pTcbTcp->csRx );
	//CheckOutPtr( __FILE__, __LINE__ );
	//return SAVRX_OK;
}

BOOL TCP_ReadData( TCP_TCB* pTcbTcp, LPVOID pData, LPDWORD pdwLenData )
{
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	TCPDATA_IN*		pDataIn;
	DWORD dwLenAll, dwCopyLen;

	//参数检查
//	RETAILMSG( 1, ( "TCP_ReadData entry.\r\n" ) );

	dwLenAll = *pdwLenData;
	if( !pData || !dwLenAll )
	{
		//不需要 读取数据
	//	RETAILMSG( 1, ( "TCP_ReadData Leave01.\r\n" ) );
		return FALSE;
	}
	//CheckOutPtr( __FILE__, __LINE__ );
	EnterCriticalSection( &pTcbTcp->csRx );
	pUnit = pUnitHeader = &pTcbTcp->hCommitPacketDataIn;

	while(dwLenAll)
	{
		pUnit = pUnitHeader->pNext;
		if( pUnit!=pUnitHeader )
		{
			//TCP_HDR*		pHdrTcp;
//			WORD wDataLen;

			pDataIn = LIST_CONTAINER( pUnit, TCPDATA_IN, hListDataIn );
			//pHdrTcp = pDataIn->pData;

			//wHLen = pDataIn->wLenData - ( (pHdrTcp->wHLenFlag & TCP_HLEN_MASK) )>>2;
			//pTcbTcp->wLenHdr = wHLen;
			//pTcbTcp->wLenData = pDataIn->wLenData - wHLen;
			
			dwCopyLen = MIN( dwLenAll, pDataIn->wLenData );
			if( dwCopyLen )
			{
				memcpy( pData, pDataIn->pData, dwCopyLen ); 
				pDataIn->wLenData -= (WORD)dwCopyLen;
				pDataIn->pData = (LPBYTE)( (DWORD)pDataIn->pData + dwCopyLen );
				pData = (LPVOID)( (DWORD)pData + dwCopyLen );
				dwLenAll -= dwCopyLen;
			}
			if( pDataIn->wLenData == 0 )
			{
				//List_RemoveUnit( pUnit );
				//IPDataIn_Free( pUnit );
//				pDataIn->pThis = NULL;// safe work
				FreePacketFromList( pTcbTcp, pDataIn );
				
			}
		}
		else
			break; //over
	}

	dwCopyLen = *pdwLenData - dwLenAll;
	*pdwLenData = dwCopyLen;
	pTcbTcp->dwUsedRxBufLen -= dwCopyLen;
	ASSERT( (int)pTcbTcp->dwUsedRxBufLen >= 0 );
	LeaveCriticalSection( &pTcbTcp->csRx );
	//CheckOutPtr( __FILE__, __LINE__ );
//	RETAILMSG( 1, ( "TCP_ReadData Leave02.\r\n" ) );
	return dwCopyLen;// - dwLenAll;
}
static VOID ClearRxData( TCP_TCB* pTcbTcp, PLIST_UNIT pUnitHeader )
{
//	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	while( 1 )
	{
		pUnit = pUnitHeader->pNext;
		if( pUnit!=pUnitHeader )
		{
			//List_RemoveUnit( pUnit );
			//IPDataIn_Free( pUnit );
			FreePacketFromList( pTcbTcp, (TCPDATA_IN*)pUnit );
		}
		else
			break;
	}
}

BOOL Tcp_ClearRxData( TCP_TCB* pTcbTcp )
{
	//CheckOutPtr( __FILE__, __LINE__ );
	EnterCriticalSection( &pTcbTcp->csRx );
	ClearRxData( pTcbTcp, &pTcbTcp->hCommitPacketDataIn );
	pTcbTcp->dwUsedRxBufLen = 0;
	LeaveCriticalSection( &pTcbTcp->csRx );

	EnterCriticalSection( &pTcbTcp->csRxUnCommit );

	ClearRxData( pTcbTcp, &pTcbTcp->hUnCommitPacketDataIn );
	

	LeaveCriticalSection( &pTcbTcp->csRxUnCommit );

	ASSERT( pTcbTcp->lPackCount == 0 );
	//CheckOutPtr( __FILE__, __LINE__ );

	return TRUE;
}

/*
static UINT	_Tcp_SavRxPacket( TCP_TCB* pTcbTcp )
{
	DWORD		dwLenData;

	//获取剩余空间
	if( !RWBuf_GetCntLeft( &pTcbTcp->stRxBufRW, TRUE ) )
	{
		RETAILMSG(1,(TEXT("  Tcp_SavRxData: No space 111, come[%d], cnt[%d]\r\n"), pTcbTcp->wLenData, pTcbTcp->stRxBufRW.dwCntRW));
		return SAVRX_NOSPACE;
	}

	//请在此处计算判断 SWS情况

	//存储数据
	dwLenData = pTcbTcp->wLenData;
	if( !RWBuf_WriteData( &pTcbTcp->stRxBufRW, pTcbTcp->pTcpData, &dwLenData, RWF_W_ENOUGH ) )
	{
		RETAILMSG(1,(TEXT("  Tcp_SavRxData: No space 222, come[%d], cnt[%d]\r\n"), pTcbTcp->wLenData, pTcbTcp->stRxBufRW.dwCntRW));
		return SAVRX_NOSPACE;
	}
	//RETAILMSG(1,(TEXT("  Tcp_SavRxData: come[%d], save[%d], cnt[%d]\r\n"), pTcbTcp->wLenData, dwLenData, pTcbTcp->stRxBufRW.dwCntRW));

	//
	//设置回答的ACK序列号
	//
	pTcbTcp->dwRcv_nxt += dwLenData;
//	PulseEvent( pTcbTcp->hEvtPoll );

	return SAVRX_OK;
}
*/
// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
//	IN pHdrTcp-接收的数据包TCP Header
// 返回值：
//	
// 功能描述：保存接收的数据，并更新接收信息
// 引用: 
// ********************************************************************
static void	Tcp_SavRxData( TCP_TCB* pTcbTcp, BOOL * lpbAutoFree )
{
//	BOOL bOk;
	IPDATA_IN * pIPDataIn = (IPDATA_IN *)pTcbTcp->pCurDataIn;
	TCPDATA_IN * pTCPDataIn = (TCPDATA_IN *)pTcbTcp->pCurDataIn;

	//CheckOutPtr( __FILE__, __LINE__ );

	pTcbTcp->pCurDataIn = NULL;
	ASSERT( pTCPDataIn->pThis != pTCPDataIn );
	pTCPDataIn->pHdrTcp = pIPDataIn->pData;
	pTCPDataIn->pData = pTcbTcp->pTcpData;
	pTCPDataIn->wLenData = pTcbTcp->wLenData;
	
//	pTcbTcp->wLenData = wLenData - wHLen;
//	pTcbTcp->pTcpData = pBufData + wHLen;
	_Tcp_SavRxPacket( pTcbTcp, pTCPDataIn );

	
	//if( bOk == SAVRX_OK )
	{
		CheckUncommitPacket(pTcbTcp);
		PulseEvent( pTcbTcp->hEvtPoll );
	}
	*lpbAutoFree = FALSE;// 由队列处理函数释放
	
	//CheckOutPtr( __FILE__, __LINE__ );
}

//lilin add fun
static VOID	AddCurrentPacketToUncommitList( TCP_TCB* pTcbTcp, BOOL * lpbAutoFree )
{
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	TCPDATA_IN*		pDataIn;
	DWORD dwSeqNum;
	IPDATA_IN * pIPDataIn = (IPDATA_IN *)pTcbTcp->pCurDataIn;
	TCPDATA_IN * pTCPDataIn = (TCPDATA_IN *)pTcbTcp->pCurDataIn;
	//CheckOutPtr( __FILE__, __LINE__ );

	ASSERT( pTCPDataIn->pThis != pTCPDataIn );
	//pTcbTcp->pCurDataIn->wFlag |= IPINF_NOAUTOFREE;
//	pTCPDataIn->bAutoFree = 0;
	pTcbTcp->pCurDataIn = NULL;
	pTCPDataIn->pHdrTcp = pIPDataIn->pData;
	pTCPDataIn->pData = pTcbTcp->pTcpData;
	pTCPDataIn->wLenData = pTcbTcp->wLenData;

	dwSeqNum=pTCPDataIn->pHdrTcp->dwSeqNum;

	EnterCriticalSection( &pTcbTcp->csRxUnCommit );

	pUnit = pUnitHeader = &pTcbTcp->hUnCommitPacketDataIn;
//	if( List_IsEmpty( pUnitHeader ) )
	//{
		//List_InsertHead( pUnitHeader, &pTCPDataIn->hListDataIn );
	//	AddPacketToList( pTcbTcp, pUnitHeader, &pTCPDataIn->hListDataIn, FALSE );
	//}
	//else
	{
		//pDataIn = NULL;
		while(1)
		{
			pUnit = pUnit->pNext;
			pDataIn = LIST_CONTAINER( pUnit, TCPDATA_IN, hListDataIn );
			if( pUnit!=pUnitHeader )
			{
				//pDataIn = LIST_CONTAINER( pUnit, TCPDATA_IN, hListDataIn );
				if( dwSeqNum < pDataIn->pHdrTcp->dwSeqNum )
				{	//insert it
					break;
				}
			}
			else
			{  // end list
				break;
			}
		}
		//insert it
		//List_InsertTail( pUnit, &pDataIn->hListDataIn );
		AddPacketToList( pTcbTcp, pUnit, pTCPDataIn, TRUE );
	}
	LeaveCriticalSection( &pTcbTcp->csRxUnCommit );

	//success
	*lpbAutoFree = FALSE;	//// 由队列处理函数释放
	
	//CheckOutPtr( __FILE__, __LINE__ );
	//pTcbTcp->pCurDataIn = NULL;	//clear
}

static VOID CheckUncommitPacket(TCP_TCB* pTcbTcp)
{
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	TCPDATA_IN*		pDataIn;

	//DWORD dwSeqNum=pTcbTcp->pCurDataIn->HdrIP.dwSeqNum;

	//CheckOutPtr( __FILE__, __LINE__ );
	EnterCriticalSection( &pTcbTcp->csRxUnCommit );

	pUnitHeader = &pTcbTcp->hUnCommitPacketDataIn;
	if( !List_IsEmpty( pUnitHeader ) )
	{	// has data
		while( (pTcbTcp->wFlag & TCPF_EXIT) == 0 )
		{
			pUnit = pUnitHeader->pNext;
			if( pUnit!=pUnitHeader )
			{
				TCP_HDR*		pHdrTcp;

				pDataIn = LIST_CONTAINER( pUnit, TCPDATA_IN, hListDataIn );
				pHdrTcp = pDataIn->pHdrTcp;

				//判断序列号，保存接收的数据
				if( pHdrTcp->dwSeqNum==pTcbTcp->dwRcv_nxt )
				{
					//if( pTcbTcp->wLenData && !(pTcbTcp->wAttr & TCPA_NORECV) )
//					UINT uRetv;
					//WORD wHLen = ( (pHdrTcp->wHLenFlag & TCP_HLEN_MASK) )>>2;
					//pTcbTcp->wLenHdr = wHLen;
					//pTcbTcp->wLenData = pDataIn->wLenData - wHLen;
					//pTcbTcp->pTcpData = pDataIn->pData + wHLen;	
					//pTcbTcp->pCurDataIn = pDataIn;
					List_RemoveUnit( pUnit );
					InterlockedDecrement( &pTcbTcp->lPackCount );
					RETAILMSG( 1, ( "add uncommit packet(dwSeqNum=0x%x,%d).\r\n", pHdrTcp->dwSeqNum ) );

					_Tcp_SavRxPacket( pTcbTcp, pDataIn );
					continue;
					//if( (uRetv = ) == SAVRX_OK )
					//{						
					//	continue;
					//}
					//ASSERT( 0 );
					//else if( uRetv == SAVRX_NOSPACE )
					//{
					//	;//Sleep(500); //get ap a chance to get data
//
//					}
					//启动“经过时延的ACK”
//					pTcbTcp->wFlag |= TCPF_NEEDACK;
				}
				else if(pHdrTcp->dwSeqNum<pTcbTcp->dwRcv_nxt)
				{	//该包应该丢弃
					//List_RemoveUnit( pUnit );
					//IPDataIn_Free( pUnit );
					RETAILMSG( 1, ( "free a unused packet(SegNum=0x%x,%d).\r\n", pHdrTcp->dwSeqNum ) );
					FreePacketFromList( pTcbTcp, pDataIn );
					continue;
				}
				else
					break;
			}
			else
			{	//搜索完
				break;
			}
		}
		//insert it
		//List_InsertTail( pUnit, &pTcbTcp->pCurDataIn->hListDataIn );
	}
	LeaveCriticalSection( &pTcbTcp->csRxUnCommit );

	//CheckOutPtr( __FILE__, __LINE__ );

}
