/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TCP 
版本号：  1.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：

 本版本TCPIP 说明（肖远钢 2004-04-12 写）：
1。 这套代码的完成，是在下述资料的肩膀上，重新写的，并集成了其中的相关机制。
    参考资料：RFC/IANA、WHL的TCPIP版本、TCPIP详解、网际互连
2。 支持Block和Nonblock操作
3。 如果用户connect连接失败后，还可以继续尝试调用connect
4。 如果用户connect连接成功后，再断开，还可以继续尝试调用connect
5。 如果调用connect后，就不能再调用listen/accept；
    同样调用listen/accept后，就不能再调用connect
6。 正常情况下，删除TCB是由上层对下面的调用执行的。但，对于TCPS_SYN_RECVD的TCB，
    可以由TcpIn_SynRcvd删除 或者 它本身因长时间没有被连接成功时而被检测删除
7。 如果accept操作失败，可以查看slect的意外socket----MSDN 没有这项功能。
8。 1个socket只能成功调用1次listen
9。 支持发送机制的操作有：
    首次发送、超时重传、快速重传、捎带时延的ACK、通知RST、通知ACK、
	计算RTT和RTO、坚持定时器
10。在TCPS_SYN_RECVD时的TCB是没有关联上层结构的，所以存在删除和与对方断开FIN的问题。
    删除见上面所述。断开时，直接进入TCPS_CLOSED，不用发送FIN。
11。参见函数：Tcp_ChkSegment，当收到RST，表明对方已经断开；此时
    如果设置TCPF_EXIT，让用户退出TCB，优点---可以给别的程序占用端口的机会；
    如果不设置TCPF_EXIT，优点---让用户可以重试连接
12。TCP的accept可以使用ANY IP，connect必需要绑定或自动绑定固定IP；
13。UDP本地是ANY IP时的，功能分析：
    1> 可以接收，所有网卡的该PORT的数据
    2> 可以选择1个适当的网卡IP，进行发送；所谓适当就是，优先选择与目的IP在同1个子网内的网卡IP
    3> 该做法，比较适合同1台机，有不同子网的多个网卡。这样可以，
       监听所有网卡，并选择目的子网内的网卡发送。

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

TCP_GLOBAL*		g_lpGlobalTcp = NULL;


static	void	Tcp_Free( TCP_GLOBAL* lpGlobalTcp );


/******************************************************/


// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：初始化 TCP
// 引用: 
// ********************************************************************
BOOL	Tcp_Init( )
{
	TCP_GLOBAL*		lpGlobalTcp;

	//分配1个TCP的功能 和基本初始化
	lpGlobalTcp = (TCP_GLOBAL*)HANDLE_ALLOC( sizeof(TCP_GLOBAL) );
	if( !lpGlobalTcp )
	{
		return FALSE;
	}
	HANDLE_INIT( lpGlobalTcp, sizeof(TCP_GLOBAL) );
	List_InitHead( &lpGlobalTcp->hListTcb );
	InitializeCriticalSection( &lpGlobalTcp->csListTcb );
	//
	lpGlobalTcp->wPort_Auto = PORTAUTO_MIN;
	lpGlobalTcp->dwTickISN = GetTickCount();
	lpGlobalTcp->hEvtPersist = CreateEvent( NULL, FALSE, FALSE, NULL );

	//
	//
	if( !TcpIn_Init( lpGlobalTcp ) )
	{
		goto EXIT_TCPINIT;
	}
	//
	if( !TcpSk_Init( lpGlobalTcp ) )
	{
		goto EXIT_TCPINIT;
	}

	if( !TcpOut_StartPersist( lpGlobalTcp ) )
	{
		goto EXIT_TCPINIT;
	}

	//
	if( !Sock_Register( &lpGlobalTcp->LocalTl ) )
	{
		goto EXIT_TCPINIT;
	}

	g_lpGlobalTcp = lpGlobalTcp;
	return TRUE;

EXIT_TCPINIT:
	if( lpGlobalTcp )
	{
		Tcp_Free( lpGlobalTcp );
	}
	return FALSE;
}

void	Tcp_Deinit( )
{
}

// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	
// 功能描述：释放 TCP
// 引用: 
// ********************************************************************
void	Tcp_Free( TCP_GLOBAL* lpGlobalTcp )
{
	//句柄安全检查
	if( !HANDLE_CHECK(lpGlobalTcp) )
	{
		return ;
	}

	//
	HANDLE_FREE( lpGlobalTcp );
}


BOOL	Tcp_CreateTcb( TCP_TCB** ppTcbTcp, TCP_GLOBAL* lpGlobalTcp )
{
	TCP_TCB*	pTcbTcp;

	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
	{
		return FALSE;
	}
	//分配 和 基本初始化
	*ppTcbTcp = NULL;
	pTcbTcp = (TCP_TCB*)HANDLE_ALLOC( sizeof(TCP_TCB) );
	if( !pTcbTcp )
	{
		return FALSE;
	}
	HANDLE_INIT( pTcbTcp, sizeof(TCP_TCB) );
	List_InitHead( &pTcbTcp->hListTcb );
	List_InitHead( &pTcbTcp->hListSynRcvd );
	List_InitHead( &pTcbTcp->hUnCommitPacketDataIn ); //lilin add code
	List_InitHead( &pTcbTcp->hCommitPacketDataIn ); //lilin add code
	pTcbTcp->dwMaxRxBufSize = 10*1024;//10k//lilin add code

	InitializeCriticalSection( &pTcbTcp->csTcb );
//	InitializeCriticalSection( &pTcbTcp->stRxBufRW.csBufRW );
	InitializeCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );
	InitializeCriticalSection( &pTcbTcp->csRx );
	InitializeCriticalSection( &pTcbTcp->csRxUnCommit );

	pTcbTcp->lpGlobalTcp = lpGlobalTcp;
	pTcbTcp->hEvtPoll = CreateEvent( NULL, TRUE, FALSE, NULL );
	pTcbTcp->hEvtSend = CreateEvent( NULL, FALSE, FALSE, NULL );

	//收发---分配BUFFER
//	if( !RWBuf_Alloc( &pTcbTcp->stRxBufRW, LEN_RXBUF, MIN_RXBUF ) )
//	{
//		goto EXIT_CREATETCB;
//	}
	if( !RWBuf_Alloc( &pTcbTcp->stTxBufRW, LEN_TXBUF, MIN_TXBUF ) )
	{
		goto EXIT_CREATETCB;
	}

	*ppTcbTcp = pTcbTcp;
EXIT_CREATETCB:
	if( !(*ppTcbTcp) && pTcbTcp )
	{
		Tcp_DeleteTCB( pTcbTcp );
	}

	return (*ppTcbTcp)?TRUE:FALSE;
}

void	Tcp_DeleteTCB( TCP_TCB* pTcbTcp )
{
	TCP_GLOBAL*		lpGlobalTcp;

	//句柄安全检查
	if( !HANDLE_CHECK(pTcbTcp) )
	{
		return ;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( !HANDLE_CHECK(lpGlobalTcp) )
	{
		return ;
	}
	//设置退出标志、状态
	pTcbTcp->dwState = TCPS_CLOSED;
	pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	pTcbTcp->wFlag |= TCPF_EXIT;
	pTcbTcp->nErr = E_INTR;
	Sleep( 1 );
	//通知自己退出
	if( pTcbTcp->hEvtSend )
	{
		SetEvent( pTcbTcp->hEvtSend );
		Sleep( 1 );
	}
	if( pTcbTcp->hEvtPoll )
	{
		PulseEvent( pTcbTcp->hEvtPoll );
		SetEvent( pTcbTcp->hEvtPoll );
		Sleep( 1 );
	}
	if( pTcbTcp->hThrdSend )
	{
		if( WaitForSingleObject( pTcbTcp->hThrdSend, 3000 )!=WAIT_OBJECT_0 )
		{
			TerminateThread( pTcbTcp->hThrdSend, 0 );
		}
	}

	//从 lpGlobalTcp链表上 和 accept的 hListSynRcvd队列 断开
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	List_RemoveUnit( &pTcbTcp->hListTcb );
	List_RemoveUnit( &pTcbTcp->hListSynRcvd );
	List_InitHead( &pTcbTcp->hListTcb );
	List_InitHead( &pTcbTcp->hListSynRcvd );
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	//释放
	if( pTcbTcp->hEvtPoll )
	{
		CloseHandle( pTcbTcp->hEvtPoll );
	}
	if( pTcbTcp->hThrdSend )
	{
		CloseHandle( pTcbTcp->hThrdSend );
	}
	if( pTcbTcp->hEvtSend )
	{
		CloseHandle( pTcbTcp->hEvtSend );
	}

	{
		extern BOOL Tcp_ClearRxData( TCP_TCB* pTcbTcp );
		Tcp_ClearRxData( pTcbTcp );
	}
	EnterCriticalSection( &pTcbTcp->csRx );
	LeaveCriticalSection( &pTcbTcp->csRx );
	DeleteCriticalSection( &pTcbTcp->csRx );

	EnterCriticalSection( &pTcbTcp->csRxUnCommit );
	LeaveCriticalSection( &pTcbTcp->csRxUnCommit );
	DeleteCriticalSection( &pTcbTcp->csRxUnCommit );


	//
//	RWBuf_Free( &pTcbTcp->stRxBufRW );

	RWBuf_Free( &pTcbTcp->stTxBufRW );
	DeleteCriticalSection( &pTcbTcp->csTcb );
	//释放句柄
	HANDLE_FREE( pTcbTcp );
}


BOOL	Tcp_QueryMSS( DWORD dwSrcIP, DWORD dwDesIP, OUT WORD* pwMSS_me )
{
	NETINFO_IPV4		InfoNet;

	//查询 IP层网络信息
	if( !IP_Query( dwSrcIP, &InfoNet ) )
	{
		//网络层失败
		return FALSE;
	}
	//获取本地MSS
	if( ((dwSrcIP & InfoNet.dwIP_Mask)==(dwDesIP & InfoNet.dwIP_Mask)) || (InfoNet.wMTU<=512) )
	{
		//同1子网内
		*pwMSS_me = InfoNet.wMTU - sizeof(IP_HDR) - sizeof(TCP_HDR);	//attention_xyg_2004-04-19
	}
	else
	{
		//不是同1子网内
		*pwMSS_me = 1024;// 512;
	}

	return TRUE;
}

WORD	Tcp_CalWndSize( TCP_TCB* pTcbTcp )
{
	DWORD		dwLeft;
	DWORD		dwRcv_MSS;
	DWORD		dwCal;
	LONG        lPacket;

	//获取剩余空间
//	EnterCriticalSection( &pTcbTcp->stRxBufRW.csBufRW );
	EnterCriticalSection( &pTcbTcp->csRx );
	dwLeft = pTcbTcp->dwMaxRxBufSize - pTcbTcp->dwUsedRxBufLen;//RWBuf_GetCntLeft( &pTcbTcp->stRxBufRW, FALSE ); //lilin
	dwRcv_MSS = pTcbTcp->wRcv_MSS;
	lPacket = pTcbTcp->lPackCount;
	LeaveCriticalSection( &pTcbTcp->csRx );
//	LeaveCriticalSection( &pTcbTcp->stRxBufRW.csBufRW );

	if( lPacket > 20 )
	{
		RETAILMSG( 1,  ("Packet > 20,dwLeft=%d.\r\n", dwLeft ) );
		return  0;
	}
	//
	if( !dwRcv_MSS )
	{
		return (dwLeft>MAXWORD)?MAXWORD:(WORD)dwLeft;
	}
	if( !dwLeft )
	{
		return 0;
	}
	if( dwLeft<dwRcv_MSS )
	{
		if( dwLeft>=512 )
		{
			return 512;
		}
		else
		{
			return 0;
		}
	}
	if( dwRcv_MSS>512 )
	{
		dwCal = dwLeft;
		dwCal /= 512;
		dwCal *= 512;
	}
	else
	{
		dwCal = 0;
	}
	//
	dwLeft /= dwRcv_MSS;
	dwLeft *= dwRcv_MSS;
	//
	if( dwCal>dwLeft )
	{
		dwLeft = dwCal;
	}

	return (dwLeft>MAXWORD)?MAXWORD:(WORD)dwLeft;
}

DWORD	Tcp_CalISN( TCP_TCB* pTcbTcp )
{
	DWORD		dwTmp;

	//计算ISN，并且用它初始化
	dwTmp = (GetTickCount() - pTcbTcp->lpGlobalTcp->dwTickISN)/4;
	pTcbTcp->lpGlobalTcp->dwTickISN = GetTickCount();
	pTcbTcp->lpGlobalTcp->dwISN += dwTmp;
	dwTmp = pTcbTcp->lpGlobalTcp->dwISN;

	pTcbTcp->dwSnd_una = dwTmp;
	pTcbTcp->dwSnd_nxt = dwTmp;
	pTcbTcp->dwSnd_real = dwTmp;
	pTcbTcp->dwSnd_max = dwTmp;
	return dwTmp;
}













