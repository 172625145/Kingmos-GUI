/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TCP发送数据状态机 处理
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


/***************  全局区 定义， 声明 *****************/


static	DWORD	WINAPI	TcpOut_ThrdSend( LPVOID lpParam );
static	DWORD	WINAPI	TcpOut_ThrdPersist( LPVOID lpParam );

//内部辅助函数
static	SKERR	TcpOut_SendPk( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp );
static	WORD	TcpOut_GetSndData( TCP_TCB* pTcbTcp, FRAGSETA* pFragSetA );
static	void	TcpOut_FillOpt( TCP_TCB* pTcbTcp, OUT LPBYTE pData );


/******************************************************/


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT pTcbTcp-要操作的TCB
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：启动发送线程
// 引用: 
// ********************************************************************
BOOL	TcpOut_StartSend( TCP_TCB* pTcbTcp )
{
	DWORD	dwThrdID;

	//设置第一次超时，启动发送线程
	pTcbTcp->dwRTO = TCPTM_SEND_FIRST;
	if( !pTcbTcp->hThrdSend )
	{
		pTcbTcp->hThrdSend = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)TcpOut_ThrdSend, (LPVOID)pTcbTcp, 0, &dwThrdID );
		if( !pTcbTcp->hThrdSend )
		{
			return FALSE;
		}
	}
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalTcp-TCPIP的全局信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：启动线程
// 引用: 
// ********************************************************************
BOOL	TcpOut_StartPersist( TCP_GLOBAL* lpGlobalTcp )
{
	DWORD	dwThrdID;

	if( !lpGlobalTcp->hThrdPersist )
	{
		lpGlobalTcp->hThrdPersist = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)TcpOut_ThrdPersist, (LPVOID)lpGlobalTcp, 0, &dwThrdID );
		if( !lpGlobalTcp->hThrdPersist )
		{
			return FALSE;
		}
	}
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpParam-要操作的TCB
// 返回值：
//	
// 功能描述：发送线程
//   调度---第1次发送、超时重发、快速重发、捎带ACK
//   内容---发送各阶段的协议标志 和 发送数据包
// 引用: 
// ********************************************************************
DWORD	WINAPI	TcpOut_ThrdSend( LPVOID lpParam )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)lpParam;
	TCP_GLOBAL*		lpGlobalTcp;
	BYTE			bBuffer[sizeof(IP_HDR) + sizeof(TCP_HDR) + 40];	//40 for tcp option
	IP_HDR*			pHdrIP = (IP_HDR*)bBuffer;
	TCP_HDR*		pHdrTcp = (TCP_HDR*)(bBuffer + sizeof(IP_HDR));
	BOOL			fNeedInit = TRUE;
	DWORD			dwTmout;
	SKERR			nErr;

	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	dwTmout = pTcbTcp->dwRTO;
	//RETAILMSG(1,(TEXT("  TcpOut_ThrdSend: pTcbTcp->dwState=[%d]...\r\n"), pTcbTcp->dwState));
	while( !( (pTcbTcp->dwState==TCPS_CLOSED) || (pTcbTcp->wFlag & TCPF_EXIT) || (lpGlobalTcp->wFlag & TCPIPF_EXIT) ) )
	{
		//等待发送事件
		WaitForSingleObject( pTcbTcp->hEvtSend, dwTmout );
		//RETAILMSG(1,(TEXT("  TcpOut_ThrdSend: then Get hEvtSend...\r\n")));
		if( (pTcbTcp->dwState==TCPS_CLOSED) || (pTcbTcp->wFlag & TCPF_EXIT) || (lpGlobalTcp->wFlag & TCPIPF_EXIT) )
		{
			break;
		}

		//
		//if( !(pTcbTcp->wAttr & TCPA_SEND) )
		//{
		//	continue;
		//}

		//初始化---仅1次
		if( fNeedInit )
		{
			memset( bBuffer, 0, sizeof(bBuffer) );
			//填充 IP地址对
			pHdrIP->bVerHLen = IPV4_VHDEF;
			pHdrIP->bProType = IP_PROTYPE_TCP;
			pHdrIP->dwSrcIP = pTcbTcp->dwSrcIP;
			pHdrIP->dwDesIP = pTcbTcp->dwDesIP;
			pHdrTcp->wSrcPort = pTcbTcp->wSrcPort;
			pHdrTcp->wDesPort = pTcbTcp->wDesPort;

			fNeedInit = FALSE;
		}

		//发送 协议标志和数据
		nErr = TcpOut_SendPk( pTcbTcp, pHdrIP, pHdrTcp );
		if( nErr==E_NOTCONN )
		{
			pTcbTcp->nErr = nErr;
			PulseEvent( pTcbTcp->hEvtPoll );
			SetEvent( pTcbTcp->hEvtPoll );
			break;
		}
		//
		if( pTcbTcp->wFlag & TCPF_NEEDPERSIST )
		{
			SetEvent( lpGlobalTcp->hEvtPersist );
			//break;
		}
		//注意： xyg_2004-5-14
		if( (pTcbTcp->dwState==TCPS_ESTABLISHED) && (pTcbTcp->dwSnd_una==pTcbTcp->dwSnd_max) )
		{
			dwTmout = INFINITE;
		}
		else
		{
			dwTmout = pTcbTcp->dwRTO;
		}
	}

	//退出该线程
	if( pTcbTcp->hThrdSend )
	{
		CloseHandle( pTcbTcp->hThrdSend );
		pTcbTcp->hThrdSend = NULL;
	}
	//RETAILMSG(1,(TEXT("\r\n +++++%%%%%%%%%%\r\n")));
	//RETAILMSG(1,(TEXT("  TcpOut_ThrdSend: exit!\r\n")));
	//RETAILMSG(1,(TEXT("\r\n +++++%%%%%%%%%%\r\n")));

	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpParam-TCPIP的全局信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：线程---检测“长时间没有被连接成功的被动连接TCB”，并删除它
// 引用: 
// ********************************************************************
//坚持定时器，通告对方，获取对方的WndSize
DWORD	WINAPI	TcpOut_ThrdPersist( LPVOID lpParam )
{
	TCP_GLOBAL*		lpGlobalTcp = (TCP_GLOBAL*)lpParam;
	TCP_TCB*		pTcbTcp;
	DWORD			dwTmout;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	dwTmout = INFINITE;
	while( HANDLE_F_OK(lpGlobalTcp, TCPIPF_EXIT) )
	{
		//等待
		WaitForSingleObject( lpGlobalTcp->hEvtPersist, dwTmout );
		if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
		{
			break;
		}
		dwTmout = INFINITE;
		//查询所有的 TCB，并删除指定的
		pUnitHeader = &lpGlobalTcp->hListTcb;
		EnterCriticalSection( &lpGlobalTcp->csListTcb );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			//根据结构成员地址，得到结构地址，并继续
			pTcbTcp = LIST_CONTAINER( pUnit, TCP_TCB, hListTcb );
			pUnit = pUnit->pNext;
			
			//句柄安全寻找
			if( !HANDLE_CHECK(pTcbTcp) )
			{
				dwTmout = 2000;	//??????????
				break;
			}
			if( pTcbTcp->wFlag & TCPF_EXIT )
			{
				continue;
			}
			//
			if( (pTcbTcp->wFlag & TCPF_NEEDPERSIST) && (pTcbTcp->dwState!=TCPS_CLOSED) )
			{
				if( pTcbTcp->wRcv_wnd )
				{
					pTcbTcp->wFlag &= ~TCPF_NEEDPERSIST;
					SetEvent( pTcbTcp->hEvtSend );
				}
				else
				{
					TcpOut_SendAck( pTcbTcp, pTcbTcp->dwSrcIP, pTcbTcp->wSrcPort, pTcbTcp->dwDesIP, pTcbTcp->wDesPort, FALSE );
					dwTmout = TCPTM_WAITPERSIST;
				}
			}
		}
		LeaveCriticalSection( &lpGlobalTcp->csListTcb );
	}

	return 0;
}





// ********************************************************************
// 声明：
// 参数：
//	IN pTcbTcp-提供：wWinSize、dwRcv_nxt、dwSnd_nxt、要发送的数据
//	IN pHdrIP-提供：IP信息
//	IN pHdrTcp-提供：TCP信息
// 返回值：
//	
// 功能描述：发送线程
//   调度---第1次发送、超时重发、快速重发、捎带ACK
//   内容---发送各阶段的协议标志 和 发送数据包
// 引用: 
// ********************************************************************
SKERR	TcpOut_SendPk( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp )
{
	IPRET		dwRet;
	BOOL		fSendSyn = FALSE;
	BOOL		fSendData = TRUE;

	BYTE		pBufFragSet[sizeof(FRAGSETA) + 2*sizeof(FRAGMENT)];
	FRAGSETA*	pFragSetA = (FRAGSETA*)pBufFragSet;

	WORD		wHLen;
	WORD		wBytesTx;
	WORD		wTmp;

	//RETAILMSG(1,(TEXT("  TcpOut_SendPk: called \r\n")));
	//填充 wHLenFlag信息
	if( pTcbTcp->dwState!=TCPS_ESTABLISHED )
	{
		//
		//分析 非连接状态时的 协议包特征
		//

		//判断停止发送的状态---FIN_WAIT2 和 由同时关闭而进入的TIME_WAIT
		if( pTcbTcp->dwState==TCPS_FIN_WAIT2 )
		{
			return E_SUCCESS;
		}
		if( (pTcbTcp->dwState==TCPS_TIME_WAIT) && (pTcbTcp->wAttr & TCPA_CLOSING) )
		{
			return E_SUCCESS;
		}

		//判断是否发送SYN
		if( !(pTcbTcp->wFlag & TCPF_LSYN) )//表明用户没有收到正确的ACK
		{
			if( (pTcbTcp->dwState==TCPS_SYN_RECVD) || (pTcbTcp->dwState==TCPS_SYN_SENT) )
			{
				fSendSyn = TRUE;
			}
		}
		//计算、填充---wHLenFlag信息
		if( fSendSyn )
		{
			//RETAILMSG(1,(TEXT("  TcpOut_SendPk: Set SYN\r\n")));
			Tcp_CalISN( pTcbTcp );

			//填充---wHLenFlag信息
			wHLen = (sizeof(TCP_HDR)+40);
			pHdrTcp->wHLenFlag = ( TCP_HLEN_MASK | TCP_FLAG_SYN );
		}
		else
		{
			//填充---wHLenFlag信息
			wHLen = sizeof(TCP_HDR);
			pHdrTcp->wHLenFlag = TCP_HLEN_FIX;
		}

		//判断是否发送FIN
		if( pTcbTcp->dwState==TCPS_FIN_WAIT1 || pTcbTcp->dwState==TCPS_LAST_ACK )
		{
			pHdrTcp->wHLenFlag |= TCP_FLAG_FIN;
			
		}

		//判断是否发送“经过时延的ACK”
		if( pTcbTcp->dwState!=TCPS_SYN_SENT )
		{
			pHdrTcp->wHLenFlag |= TCP_FLAG_ACK;
			pTcbTcp->wFlag &= ~TCPF_NEEDACK;
		}

		//判断是否发送数据
		if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
		{
			fSendData = FALSE;

			//添加发送选项
			TcpOut_FillOpt( pTcbTcp, (LPBYTE)pHdrTcp+sizeof(TCP_HDR) );
		}
		else if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
		{
			if( !(pTcbTcp->wAttr & TCPA_SEND) )
			{
				fSendData = FALSE;
			}
		}
	}
	else
	{
		//查看是否有数据，如果没有数据，就退出
		//判断数据是否发送完毕
		//if( pTcbTcp->dwSnd_nxt==pTcbTcp->dwSnd_max )
		if( pTcbTcp->dwSnd_una==pTcbTcp->dwSnd_max )
		{
			return 0;
		}
		//
		//填充 连接状态时的 协议包特征---发送“经过时延的ACK”
		//
		wHLen = sizeof(TCP_HDR);
		pHdrTcp->wHLenFlag = ( TCP_HLEN_FIX | TCP_FLAG_ACK );
		pTcbTcp->wFlag &= ~TCPF_NEEDACK;
	}

	//
	//循环发送数据
	//
	//RETAILMSG(1,(TEXT("  TcpOut_SendPk: ok cnt=[%d]\r\n"), pTcbTcp->stTxBufRW.dwCntRW));

	//初始化
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrTcp;
	pFragSetA->pFrag[0].nLen = wHLen;

	wBytesTx = 0;
	pTcbTcp->wFlag |= TCPF_SENDING;
	pTcbTcp->dwTickRTT = GetTickCount();
	//恢复到发送的起点---有可能是因为超时重发引起的
	pTcbTcp->dwSnd_nxt = pTcbTcp->dwSnd_una;

	//
	while( (pTcbTcp->dwState!=TCPS_CLOSED) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//捎带ACK
		if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
		{
			pTcbTcp->wFlag &= ~TCPF_NEEDACK;
		}
		//计算信息
		wTmp = Tcp_CalWndSize( pTcbTcp );

		//发送缓冲---
		//1. 与Tcp_TrIn互斥-------修改 发送缓冲的 序列号信息，和缓冲大小，Read位置
		//2. 与TcpSk_send---修改 发送缓冲的 缓冲大小，Write位置，可能会修改数据，如果剩余空间不足够的话。
		EnterCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );

		//填充 wWinSize信息、ACK信息、SEQ信息、UP信息
#ifndef BIG_END
		pHdrTcp->dwSeqNum = htonl(pTcbTcp->dwSnd_nxt);
		pHdrTcp->dwAckNum = htonl(pTcbTcp->dwRcv_nxt);
		pHdrTcp->wUrgPtr = 0;
#else
		pHdrTcp->dwSeqNum = pTcbTcp->dwSnd_nxt;
		pHdrTcp->dwAckNum = pTcbTcp->dwRcv_nxt;
		pHdrTcp->wUrgPtr = 0;
#endif
		pHdrTcp->wWinSize = htons_m(wTmp);
		//保存部分信息 
		pTcbTcp->dwAck_Last = pTcbTcp->dwRcv_nxt;
		pTcbTcp->wSnd_wnd_Last = wTmp;

		//获取 “发送缓冲里的数据”，并更新发送信息
		if( fSendData && pTcbTcp->stTxBufRW.dwCntRW )
		{
			wBytesTx = TcpOut_GetSndData( pTcbTcp, pFragSetA );
		}
		//
		if( wBytesTx )
		{
			pHdrTcp->wHLenFlag |= TCP_FLAG_PSH;
		}
		else
		{
			pFragSetA->nCnt = 1;
		}
		//计算校验码
		pHdrTcp->wTotalCRC = 0;
		pHdrTcp->wTotalCRC = Tcp_CRC2( pHdrIP->dwSrcIP, pHdrIP->dwDesIP, IP_PROTYPE_TCP, pFragSetA );
		//发送数据包
		pHdrIP->wLenTotal = sizeof(IP_HDR)+wHLen+wBytesTx;

		//Tcp_DbgOut( pTcbTcp->dwState, pHdrTcp, (WORD)(wHLen+wBytesTx), FALSE );
		dwRet = IP_NetOut( NULL, pHdrIP, pFragSetA );
		//判断数据是否发送完毕
		if( pTcbTcp->dwSnd_nxt==pTcbTcp->dwSnd_max )
		{
			pTcbTcp->wFlag &= ~TCPF_SENDING;
		}
		LeaveCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );
		//分析IP_NetOut的发送结果
		if( dwRet )
		{
			switch( dwRet )
			{
			case IPERR_NO_INTF:
				return E_NOTCONN;
				break;
			case IPERR_SND_NOBUF:
				return E_NOBUFS;
				break;
			default :
				return E_SND_UNKNOWN;
				break;
			}
		}

		//判断数据是否发送完毕
		if( pTcbTcp->dwSnd_nxt==pTcbTcp->dwSnd_max )
		{
			break;
		}
	}
	pTcbTcp->wFlag &= ~TCPF_SENDING;

	return E_SUCCESS;
}

//添加发送选项
void	TcpOut_FillOpt( TCP_TCB* pTcbTcp, OUT LPBYTE pData )
{
	*pData++ = TCP_OPT_MSS;
	*pData++ = TCP_OPT_MSS_LEN;
	phtonsp( pData, (LPBYTE)&pTcbTcp->wSnd_MSS );
}


WORD	TcpOut_GetSndData( TCP_TCB* pTcbTcp, FRAGSETA* pFragSetA )
{
	WORD		wBytesSent;
	WORD		wBytesTx;
	WORD		wBytesTxOnly;
	DWORD		nLen1;

	//获取“已经发送但对方没有ACK”的数据的长度
	if( pTcbTcp->dwSnd_nxt >= pTcbTcp->dwSnd_una )
	{
		wBytesSent = (WORD)(pTcbTcp->dwSnd_nxt - pTcbTcp->dwSnd_una);
	}
	else
	{
		wBytesSent = (WORD)(pTcbTcp->dwSnd_nxt + (MAXDWORD - pTcbTcp->dwSnd_una) );
	}
	//确定要发送的长度---根据“发送缓冲的剩余数据”和“对方MSS长度”，
	if( pTcbTcp->stTxBufRW.dwCntRW<MAXWORD )	//发送缓冲的剩余长度
	{
		wBytesTx = (WORD)(pTcbTcp->stTxBufRW.dwCntRW - wBytesSent);
	}
	else
	{
		wBytesTx = MAXWORD - wBytesSent;
	}
	if( wBytesTx > pTcbTcp->wRcv_MSS )	//不能超过对方的MSS
	{
		wBytesTx = pTcbTcp->wRcv_MSS;
	}
	if( wBytesTx > pTcbTcp->wSnd_MSS )	//不能超过自己的MSS
	{
		wBytesTx = pTcbTcp->wSnd_MSS;
	}
	//检查发送长度---在 对方Winsize内
	if( wBytesTx )
	{
		//检查对方Winsize
		if( pTcbTcp->wRcv_wnd==0 )
		{
			//请用坚持定时器，通告对方
			pTcbTcp->wFlag |= TCPF_NEEDPERSIST;
			//SetEvent( pTcbTcp->lpGlobalTcp->hEvtPersist );
			pFragSetA->nCnt = 1;
			return 0;
		}
		else
		{
			//判断对方WinSize内剩余的空间内，只能发送的长度
			if( pTcbTcp->wRcv_wnd > wBytesSent )
			{
				wBytesTxOnly = pTcbTcp->wRcv_wnd - wBytesSent;
				
				//先检查 SWS情况
				
				//
				if( wBytesTx > wBytesTxOnly )
				{
					wBytesTx = wBytesTxOnly;
				}
			}
			else
			{
				//请用坚持定时器，通告对方
				pTcbTcp->wFlag |= TCPF_NEEDPERSIST;
				//SetEvent( pTcbTcp->lpGlobalTcp->hEvtPersist );
				pFragSetA->nCnt = 1;
				return 0;
			}
		}
	}
	//准备数据发送
	if( wBytesTx )
	{
		//获取偏移后 发送的位置 和 发送行的长度
		RWBuf_GetReadBuf( &pTcbTcp->stTxBufRW, wBytesSent, &pFragSetA->pFrag[1].pData, &nLen1 );
		//判断剩余数据
		if( nLen1 >= (DWORD)wBytesTx )
		{
			pFragSetA->nCnt = 2;
			//---如果第1段有足够长的数据发送
			pFragSetA->pFrag[1].nLen = wBytesTx;

			//pFragSetA->pFrag[2].pData = NULL;
		}
		else
		{
			pFragSetA->nCnt = 3;
			//---还需要发送第2段的数据
			pFragSetA->pFrag[1].nLen = nLen1;

			pFragSetA->pFrag[2].pData = pTcbTcp->stTxBufRW.pBufRW;
			pFragSetA->pFrag[2].nLen = wBytesTx - (WORD)nLen1;
		}
	}
	//else
	//{
	//	//pFragSetA->nCnt = 1;
	//	//pFragSetA->pFrag[1].pData = NULL;
	//	//pFragSetA->pFrag[2].pData = NULL;
	//}

	//更新发送信息
	if( wBytesTx )
	{
		//更新当前发送位置
		pTcbTcp->dwSnd_nxt += wBytesTx;	

		//更新实际最长发送位置
		if( pTcbTcp->dwSnd_real > pTcbTcp->dwSnd_nxt )
		{
			if( pTcbTcp->dwSnd_nxt <= pTcbTcp->dwSnd_max )
			{
				pTcbTcp->dwSnd_real = pTcbTcp->dwSnd_nxt;
			}
		}
		else if( pTcbTcp->dwSnd_real < pTcbTcp->dwSnd_nxt )
		{
			if( (pTcbTcp->dwSnd_nxt<=pTcbTcp->dwSnd_max) || (pTcbTcp->dwSnd_max<=pTcbTcp->dwSnd_real) )
			{
				pTcbTcp->dwSnd_real = pTcbTcp->dwSnd_nxt;
			}
		}
	}

	return wBytesTx;
}



// ********************************************************************
// 声明：
// 参数：
//	IN pHdrIP-提供：IP地址
//	IN pHdrTcp-提供：PORT
// 返回值：
//	
// 功能描述：
// 引用: 
// ********************************************************************
IPRET	TcpOut_SendRst( TCP_TCB* pTcbTcp, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDesIP, WORD wDesPort, DWORD dwAckNum )
{
	BYTE		bBuffer[sizeof(IP_HDR) + sizeof(TCP_HDR)];
	IP_HDR*		pHdrIP = (IP_HDR*)bBuffer;
	TCP_HDR*	pHdrTcp = (TCP_HDR*)(bBuffer + sizeof(IP_HDR));

	BYTE		pBufFragSet[sizeof(FRAGSETA)];
	FRAGSETA*	pFragSetA = (FRAGSETA*)pBufFragSet;

	//
	memset( bBuffer, 0, sizeof(bBuffer) );
	//填充 IP地址对 
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_TCP;
	pHdrIP->dwSrcIP = dwSrcIP;
	pHdrIP->dwDesIP = dwDesIP;
	pHdrTcp->wSrcPort = wSrcPort;
	pHdrTcp->wDesPort = wDesPort;
	//填充 wHLenFlag信息
	pHdrTcp->wHLenFlag = ( TCP_HLEN_FIX | TCP_FLAG_RST );
	//填充 wWinSize信息、ACK信息、SEQ信息、UP信息
	if( pTcbTcp )
	{
#ifndef BIG_END
		pHdrTcp->dwSeqNum = htonl(pTcbTcp->dwSnd_nxt);
		pHdrTcp->dwAckNum = htonl(pTcbTcp->dwRcv_nxt);
#else
		pHdrTcp->dwSeqNum = pTcbTcp->dwSnd_nxt;
		pHdrTcp->dwAckNum = pTcbTcp->dwRcv_nxt;
#endif
	}
	else
	{
#ifndef BIG_END
		pHdrTcp->dwAckNum = htonl(dwAckNum);
#else
		pHdrTcp->dwAckNum = dwAckNum;
#endif
	}
	//保存部分信息 

	//准备数据包
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrTcp;
	pFragSetA->pFrag[0].nLen = sizeof(TCP_HDR);
	//计算校验码
	pHdrTcp->wTotalCRC = Tcp_CRC( dwSrcIP, dwDesIP, IP_PROTYPE_TCP, (LPBYTE)pHdrTcp, sizeof(TCP_HDR) );
	//发送数据包
	pHdrIP->wLenTotal = sizeof(IP_HDR)+sizeof(TCP_HDR);

	//if( pTcbTcp )
	//{
	//	Tcp_DbgOut( pTcbTcp->dwState, pHdrTcp, sizeof(TCP_HDR), FALSE );
	//}
	//else
	//{
	//	Tcp_DbgOut( TCPS_MAX, pHdrTcp, sizeof(TCP_HDR), FALSE );
	//}
	return IP_NetOut( NULL, pHdrIP, pFragSetA );
}
// ********************************************************************
// 声明：
// 参数：
//	IN pTcbTcp-提供：wWinSize、dwRcv_nxt、dwSnd_nxt
//	IN pHdrIP-提供：IP地址
//	IN pHdrTcp-提供：PORT
//	IN wHLenFlag-提供：Flag
//	IN fWin_Zero-提供：是否将wWinSize设置成0
// 返回值：
//	
// 功能描述：
//----直接快速发送 Pure RST，在下列条件下：
//1. Tcp_TrIn 中，没有找到相应的TCB
//2. TcpIn_Listen 中，收“有ACK 或者 没有SYN”时
//3. TcpIn_Listen 中，超出最大的连接数时
//4. TcpIn_SynSent 中，收“传过来的ACK不是正确的”时
//5. TcpIn_SynRcvd 中，收“不合理的SEQ”时
//6. TcpIn_SynRcvd 中，收“不合理的ACK”时
//7. TcpIn_Closed中，不接收任何信息，除RST外

//----直接快速发送 Pure ACK，在下列条件下：
//1. 
//2. 
//3. 
//4. 
//5. 
//6. 

// 引用: 
// ********************************************************************
IPRET	TcpOut_SendAck( TCP_TCB* pTcbTcp, DWORD dwSrcIP, WORD wSrcPort, DWORD dwDesIP, WORD wDesPort, BOOL fSetZeroWnd )
{
	BYTE		bBuffer[sizeof(IP_HDR) + sizeof(TCP_HDR)];
	IP_HDR*		pHdrIP = (IP_HDR*)bBuffer;
	TCP_HDR*	pHdrTcp = (TCP_HDR*)(bBuffer + sizeof(IP_HDR));

	BYTE		pBufFragSet[sizeof(FRAGSETA)];
	FRAGSETA*	pFragSetA = (FRAGSETA*)pBufFragSet;

	if( (dwSrcIP==0) || (dwDesIP==0) || (wSrcPort==0) || (wDesPort==0) )
	{
		return 0;
	}
	//
	memset( bBuffer, 0, sizeof(bBuffer) );
	//填充 IP地址对
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_TCP;
	pHdrIP->dwSrcIP = dwSrcIP;
	pHdrIP->dwDesIP = dwDesIP;
	pHdrTcp->wSrcPort = wSrcPort;
	pHdrTcp->wDesPort = wDesPort;
	//填充 wHLenFlag信息
	pHdrTcp->wHLenFlag = ( TCP_HLEN_FIX | TCP_FLAG_ACK );
	//填充 wWinSize信息、ACK信息、SEQ信息、UP信息
#ifndef BIG_END
	pHdrTcp->dwSeqNum = htonl(pTcbTcp->dwSnd_nxt);
	pHdrTcp->dwAckNum = htonl(pTcbTcp->dwRcv_nxt);
#else
	pHdrTcp->dwSeqNum = pTcbTcp->dwSnd_nxt;
	pHdrTcp->dwAckNum = pTcbTcp->dwRcv_nxt;
#endif
	if( !fSetZeroWnd )
	{
		pHdrTcp->wWinSize = htons( Tcp_CalWndSize( pTcbTcp ) );
	}
	//保存部分信息 
	pTcbTcp->dwAck_Last = pTcbTcp->dwRcv_nxt;
	pTcbTcp->wSnd_wnd_Last = ntohs( pHdrTcp->wWinSize );

	//准备数据包
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrTcp;
	pFragSetA->pFrag[0].nLen = sizeof(TCP_HDR);
	//计算校验码
	pHdrTcp->wTotalCRC = Tcp_CRC( dwSrcIP, dwDesIP, IP_PROTYPE_TCP, (LPBYTE)pHdrTcp, sizeof(TCP_HDR) );
	//发送数据包
	pHdrIP->wLenTotal = sizeof(IP_HDR)+sizeof(TCP_HDR);

	//Tcp_DbgOut( pTcbTcp->dwState, pHdrTcp, sizeof(TCP_HDR), FALSE );
	return IP_NetOut( NULL, pHdrIP, pFragSetA );
}
