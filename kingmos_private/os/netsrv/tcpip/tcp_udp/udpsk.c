/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：UDP Socket函数表 处理
版本号：  1.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：
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
#include "udp_def.h"


/***************  全局区 定义， 声明 *****************/


//

//TCP提供的函数表
static	SKERR	UdpSk_Create( SOCK_INST* pSockInst );
static	SKERR	UdpSk_Bind( SOCK_INST* pSockInst, const SOCKADDR* myaddr, int addr_len );
static	SKERR	UdpSk_Connect( SOCK_INST* pSockInst, const SOCKADDR* addr, int addr_len, int flags );
static	SKERR	UdpSk_Listen( SOCK_INST* pSockInst, int queue_len );
static	SKERR	UdpSk_Accept( SOCK_INST* pSockInst, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags );
static	SKERR	UdpSk_IOCtl( SOCK_INST* pSockInst, long nCmd, IN OUT DWORD* pdwArg );
static	SKERR	UdpSk_SendMsg( SOCK_INST* pSockInst, RXTXMSG* pMsgRxTx );
static	SKERR	UdpSk_RecvMsg( SOCK_INST* pSockInst, IN OUT RXTXMSG* pMsgRxTx );
static	SKERR	UdpSk_Poll( POLLTBL* pTblPoll, IN OUT int* pnCntPoll, DWORD dwTmout );

static	SKERR	UdpSk_GetSockOpt( SOCK_INST* pSockInst, int level, int optname, OUT char* optval, int optlen );
static	SKERR	UdpSk_SetSockOpt( SOCK_INST* pSockInst, int level, int optname, const char* optval, int optlen );
static	SKERR	UdpSk_GetName( SOCK_INST* pSockInst, OUT SOCKADDR* addr, OUT int* paddr_len, int peer );

static	SKERR	UdpSk_ShutDown( SOCK_INST* pSockInst, int how );
static	SKERR	UdpSk_Close( SOCK_INST* pSockInst );

//内部辅助函数
static	BOOL	Udp_FindPort( DWORD dwSrcIP, WORD wSrcPort, UDP_GLOBAL* lpGlobalUdp, UDP_TCB** ppTcbUdp );
static	BOOL	Udp_NewPort( UDP_TCB* pTcbUdp, UDP_GLOBAL* lpGlobalUdp );
static	BOOL	Udp_UsePort( UDP_TCB* pTcbUdp, WORD wSrcPort, UDP_GLOBAL* lpGlobalUdp );

static	int	Udp_PollInfo( POLLTBL* pTblPoll, int nCntPoll );




/******************************************************/


// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalUdp-TCPIP的全局信息
// 返回值：
//	
// 功能描述：初始化 传输层信息
// 引用: 
// ********************************************************************
BOOL	UdpSk_Init( UDP_GLOBAL* lpGlobalUdp )
{
	TL_LOCAL*			pTlLocal;

	//初始化 传输层信息
	pTlLocal = &lpGlobalUdp->LocalTl;
	HANDLE_SET( pTlLocal );
	List_InitHead( &pTlLocal->hListTl );
	pTlLocal->hTlGlobal = (HANDLE)lpGlobalUdp;

	pTlLocal->nFamily = AF_INET;
	pTlLocal->nType = SOCK_DGRAM;
	pTlLocal->nProtocol = IPPROTO_UDP;

	//填充 传输层函数表
	pTlLocal->pFnTlOpen      = UdpSk_Create    ;
	pTlLocal->pFnTlBind      = UdpSk_Bind      ;
	pTlLocal->pFnTlConnect   = UdpSk_Connect   ;
	pTlLocal->pFnTlListen    = UdpSk_Listen    ;
	pTlLocal->pFnTlAccept    = UdpSk_Accept    ;
	pTlLocal->pFnTlIoctl     = UdpSk_IOCtl     ;
	pTlLocal->pFnTlSendmsg   = UdpSk_SendMsg   ;
	pTlLocal->pFnTlRecvmsg   = UdpSk_RecvMsg   ;
	pTlLocal->pFnTlPoll      = UdpSk_Poll      ;

	pTlLocal->pFnTlGetSockOpt= UdpSk_GetSockOpt;
	pTlLocal->pFnTlSetSockOpt= UdpSk_SetSockOpt;
	pTlLocal->pFnTlGetName   = UdpSk_GetName   ;

	pTlLocal->pFnTlShutDown  = UdpSk_ShutDown  ;
	pTlLocal->pFnTlClose     = UdpSk_Close     ;

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
// 返回值：
//	
// 功能描述：创建TCB
// 引用: 
// ********************************************************************
SKERR	UdpSk_Create( SOCK_INST* pSockInst )
{
	UDP_GLOBAL*	lpGlobalUdp;
	UDP_TCB*	pTcbUdp;

	//句柄安全检查
	lpGlobalUdp = (UDP_GLOBAL*)pSockInst->pTlLocal->hTlGlobal;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//分配TCB
	if( !Udp_CreateTcb( &pTcbUdp, lpGlobalUdp ) )
	{
		//空间不够而 失败
		return E_NOBUFS;
	}

	//初始化
	//pTcbUdp->dwSrcIP = INADDR_ANY;

	//保存信息
	pSockInst->hIfTl = (HANDLE)pTcbUdp;

	//关联到
	EnterCriticalSection( &lpGlobalUdp->csListTcb );
	List_InsertTail( &lpGlobalUdp->hListTcb, &pTcbUdp->hListTcb );
	LeaveCriticalSection( &lpGlobalUdp->csListTcb );

	return E_SUCCESS;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN myaddr-指定本地地址信息
//	IN addr_len-指定myaddr的长度
// 返回值：
//	
// 功能描述：对TCB绑定源地址对
// 引用: 
// ********************************************************************
SKERR	UdpSk_Bind( SOCK_INST* pSockInst, const SOCKADDR* myaddr, int addr_len )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)myaddr;
	DWORD			dwSrcIP;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pAddrIn || (addr_len<sizeof(SOCKADDR_IN)) )
	{
		//参数出错
		return E_FAULT;
	}
	if( pAddrIn->sin_family!=AF_INET )
	{
		//不支持的协议族
		return E_AFNOSUPPORT;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//检查状态：
	if( pTcbUdp->wSrcPort )
	{
		//该socket已经被绑定
		return E_INVAL;
	}

	//绑定本地IP地址
	if( !(pTcbUdp->dwOption & TCPIPO_IPINTF) )
	{
		dwSrcIP = pAddrIn->sin_addr.S_un.S_addr;
		if( dwSrcIP!=INADDR_ANY )
		{
			if( INADDR_TEST_BC(dwSrcIP) )
			{
				//该IP地址无效
				return E_ADDRNOTAVAIL;
			}
			if( !IP_IsValidAddr( dwSrcIP ) )
			{
				//该IP地址无效
				return E_ADDRNOTAVAIL;
			}
		}
		pTcbUdp->dwSrcIP = dwSrcIP;
	}
	//绑定本地端口
	if( pAddrIn->sin_port==0 )
	{
		//分配新端口，并且绑定端口
		if( !Udp_NewPort( pTcbUdp, lpGlobalUdp ) )
		{
			pTcbUdp->dwSrcIP = INADDR_ANY;
			//分配新端口失败
			return E_NOMOREPORT;
		}
	}
	else
	{
		if( !(pTcbUdp->dwOption & TCPIPO_IPINTF) )
		{
			//检查该端口是否被占用，并且绑定端口
			if( !Udp_UsePort(pTcbUdp, pAddrIn->sin_port, lpGlobalUdp) )
			{
				pTcbUdp->dwSrcIP = INADDR_ANY;
				//表明该端口被占用
				return E_ADDRINUSE;
			}
		}
	}

	return E_SUCCESS;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN addr-指定目的地址信息
//	IN addr_len-指定addr的长度
//	IN flags-保留
// 返回值：
//	
// 功能描述：连接到目的地址对
// 引用: 
// ********************************************************************
SKERR	UdpSk_Connect( SOCK_INST* pSockInst, const SOCKADDR* addr, int addr_len, int flags )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;
	DWORD			dwDesIP;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pAddrIn || (addr_len<sizeof(SOCKADDR_IN)) )
	{
		//参数出错
		return E_FAULT;
	}
	if( pAddrIn->sin_family!=AF_INET )
	{
		//不支持的协议族
		return E_AFNOSUPPORT;
	}
	dwDesIP = pAddrIn->sin_addr.S_un.S_addr;
	if( dwDesIP==INADDR_ANY || INADDR_TEST_BC(dwDesIP) || pAddrIn->sin_port==0 )
	{
		//远程地址无效---客户不能连接到 “任何地址” 和 “广播地址”
		return E_ADDRNOTAVAIL;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//确保 dwSrcIP and wSrcPort---“可以使用ANY IP”，但要PORT
	if( pTcbUdp->wSrcPort==0 )
	{
		//自动绑定
		if( !Udp_NewPort( pTcbUdp, lpGlobalUdp ) )
		{
			return E_NOMOREPORT;
		}
	}
	//避免数据发给自己（这个IP，这个PORT）
	if( pTcbUdp->wSrcPort==pAddrIn->sin_port )
	{
		if( pTcbUdp->dwSrcIP==dwDesIP )
		{
			//远程地址无效
			return E_ADDRNOTAVAIL;
		}
		if( pTcbUdp->dwSrcIP==INADDR_ANY )
		{
			if( IP_IsValidAddr(dwDesIP) )
			{
				//远程地址无效
				return E_ADDRNOTAVAIL;
			}
		}
	}

	//保存远程地址
	pTcbUdp->dwDesIP = dwDesIP;
	pTcbUdp->wDesPort = pAddrIn->sin_port;

	//注意：PC-Windows的做法如下（本系统不采纳）
	//判断了 dwDesIP与pTcbUdp->dwSrcIP是否在同1个子网内，如果不在，则返回10065

	return E_SUCCESS;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN queue_len-指定接收连接个数的长度
// 返回值：
//	
// 功能描述：将socket进入监听状态，并设置接收连接个数
// 引用: 
// ********************************************************************
SKERR	UdpSk_Listen( SOCK_INST* pSockInst, int queue_len )
{
	return E_OPNOTSUPP;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN pSockInst_New-接收连接的socket实例信息
//	IN addr-指定目的地址信息
//	IN paddr_len-保存指定addr的长度
//	IN flags-保留
// 返回值：
//	
// 功能描述：接收目的的连接，并返回它的地址对
// 引用: 
// ********************************************************************
SKERR	UdpSk_Accept( SOCK_INST* pSockInst, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags )
{
	return E_OPNOTSUPP;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN nCmd-指定命令
//	IN pdwArg-指定参数信息
// 返回值：
//	
// 功能描述：IO设置
// 引用: 
// ********************************************************************
SKERR	UdpSk_IOCtl( SOCK_INST* pSockInst, long nCmd, IN OUT DWORD* pdwArg )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SKERR			nErr;
	//int				nOptVal;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pdwArg )
	{
		//参数出错
		return E_FAULT;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//分类处理：
	nErr = E_SUCCESS;
	switch( nCmd )
	{
	//设置 Nonblock模式
	case FIONBIO:
		if( *pdwArg )
		{
			pTcbUdp->dwOption |= TCPIPO_NONBLOCK;
		}
		else
		{
			pTcbUdp->dwOption &= ~TCPIPO_NONBLOCK;
		}
		break;

	//获取接收缓冲里数据的长度
	case FIONREAD :
		//*pdwArg = pTcbUdp->stRxBufRW.dwCntRW;
		*pdwArg = Udp_GetRxFirstData( pTcbUdp, NULL, NULL, NULL );
		break;

	//检测是否有OOB数据
	//case SIOCATMARK:
	//	*pdwArg = 0;
	//	break;
	
	//
	default :
		//参数出错
		return E_FAULT;
		break;
	}

	return E_SUCCESS;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN pMsgRxTx-指定发送数据的信息
// 返回值：
//	
// 功能描述：发送数据
// 引用: 
// ********************************************************************
SKERR	UdpSk_SendMsg( SOCK_INST* pSockInst, RXTXMSG* pMsgRxTx )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pMsgRxTx || !pMsgRxTx->pData )
	{
		//参数出错
		return E_FAULT;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//成功返回，如果没有的发送数据
	if( !pMsgRxTx->dwLenData )
	{
		return E_SUCCESS;
	}
	if( pTcbUdp->wSrcPort==0 )
	{
		//自动绑定
		if( !Udp_NewPort( pTcbUdp, pTcbUdp->lpGlobalUdp ) )
		{
			return E_NOMOREPORT;
		}
	}

	return Udp_Send( pTcbUdp, pMsgRxTx );
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN/OUT pMsgRxTx-指定接收数据的信息
// 返回值：
//	
// 功能描述：接收数据
// 引用: 
// ********************************************************************
SKERR	UdpSk_RecvMsg( SOCK_INST* pSockInst, IN OUT RXTXMSG* pMsgRxTx )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	LPBYTE			pData;
	DWORD			dwLenData;
	//DWORD			dwLenTmp;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pMsgRxTx || !(pData=pMsgRxTx->pData) )
	{
		//参数出错
		return E_FAULT;
	}
	if( !(dwLenData=pMsgRxTx->dwLenData) )
	{
		//成功返回，如果没有的接收数据
		return E_SUCCESS;
	}
	if( pMsgRxTx->lpPeer && (pMsgRxTx->dwLenPeer<sizeof(SOCKADDR_IN)) )
	{
		//参数出错
		return E_FAULT;
	}
	pMsgRxTx->dwLenData = 0;
	pMsgRxTx->dwLenPeer = 0;
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//
	while( !(pTcbUdp->wFlag & UDPF_EXIT) )
	{
		//查看是否有数据，并从接收缓冲里 拷贝数据
		//if( pTcbUdp->stRxBufRW.dwCntRW )
		//{
		//	//dwLenTmp = dwLenData;
		//	//if( RWBuf_ReadData( &pTcbUdp->stRxBufRW, pData, &dwLenTmp ) )
		//	//{
		//	//	pMsgRxTx->dwLenData = dwLenTmp;
		//	//	return E_SUCCESS;
		//	//}
		//	if( RWBuf_ReadData( &pTcbUdp->stRxBufRW, pData, &dwLenData ) )
		//	{
		//		pMsgRxTx->dwLenData = dwLenData;
		//		return E_SUCCESS;
		//	}
		//}
		if( Udp_GetRxFirstData( pTcbUdp, pData, &dwLenData, pMsgRxTx ) )
		{
			pMsgRxTx->dwLenData = dwLenData;
			return E_SUCCESS;
		}
		//
		if( pTcbUdp->dwOption & TCPIPO_NONBLOCK )
		{
			//启动了 异步操作
			return E_WOULDBLOCK;
		}

		//等待发送的完成
		WaitForSingleObject( pTcbUdp->hEvtPoll, UDPTM_WAITRECV );//INFINITE
	}

	return E_INTR;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pTblPoll-要查询的实例信息
//	IN/OUT pnCntPoll-指定pTblPoll中元素的个数，保存获取到的个数
//	IN dwTmout-指定超时时间
// 返回值：
//	
// 功能描述：在指定时间内查询相应的信息
// 引用: 
// ********************************************************************
SKERR	UdpSk_Poll( POLLTBL* pTblPoll, IN OUT int* pnCntPoll, DWORD dwTmout )
{
	UDP_TCB*		pTcbUdp;
	HANDLE*			pArrEvt;
	int				nCntWait;
	int				i;
	int				nPos;
	DWORD			dwTickStart;
	DWORD			dwTickWait;
	DWORD			dwWait;
	BOOL			fWaittingForYou;

	//句柄安全检查
	if( HANDLE_F_FAIL(g_lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//查询信息
	nCntWait = *pnCntPoll;
	*pnCntPoll = Udp_PollInfo( pTblPoll, nCntWait );
	if( *pnCntPoll )
	{
		//已经获取信息
		return E_SUCCESS;
	}
	//统计需要等待的个数
	pArrEvt = (HANDLE*)malloc( nCntWait*sizeof(HANDLE) );
	if( !pArrEvt )
	{
		return E_NOBUFS;
	}
	nPos = 0;
	for( i=0; i<nCntWait; i++ )
	{
		pTcbUdp = (UDP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//句柄安全检查
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			//continue;
			free( pArrEvt );
			return E_NOTSOCK;
		}
		pArrEvt[nPos++] = pTcbUdp->hEvtPoll;
	}
	if( !nPos )
	{
		return E_FAULT;
	}
	//开始等待
	if( dwTmout<1000 )
	{
		dwTickWait = dwTmout;
		fWaittingForYou = FALSE;
		dwTickStart = GetTickCount();
	}
	else
	{
		dwTickWait = 1000;
		if( dwTmout==INFINITE )
		{
			fWaittingForYou = TRUE;
		}
		else
		{
			fWaittingForYou = FALSE;
			dwTickStart = GetTickCount();
		}
	}
	while( 1 )
	{
		dwWait = WaitForMultipleObjects( nPos, pArrEvt, FALSE, dwTickWait );
		*pnCntPoll = Udp_PollInfo( pTblPoll, nCntWait );
		if( *pnCntPoll )
		{
			//已经获取信息
			return E_SUCCESS;
		}
		else if( !fWaittingForYou )
		{
			if( (GetTickCount()-dwTickStart)>=dwTmout )
			{
				//超时退出
				return E_INVAL;
			}
			else
			{
				dwWait = GetTickCount()-dwTickStart;
				if( (dwTmout-1000)<dwWait )
				{
					dwTickWait = dwWait;
				}
			}
		}
	}

	return E_INTR;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN level-选项的级别
//	IN optname-选项的名称
//	OUT optval-选项的内容
//	IN optlen-选项的长度
// 返回值：
//	
// 功能描述：获取选项
// 引用: 
// ********************************************************************
SKERR	UdpSk_GetSockOpt( SOCK_INST* pSockInst, int level, int optname, OUT char* optval, int optlen )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SKERR			nErr;
	//int				nOptVal;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( level!=SOL_SOCKET )
	{
		//选项level无效
		return E_INVAL;
	}
	if( !SOC_ISIN(optname) )
	{
		return E_FAULT;	//选项的内容或者长度 出错
	}
	if( optname & SOC_FIXLEN )
	{
		if( SOC_GETFIXLEN(optname)!=optlen )
		{
			return E_FAULT;	//选项的内容或者长度 出错
		}
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//分类处理：
	nErr = E_SUCCESS;
	switch( optname )
	{
	//输出调试信息
	case SO_DEBUG:
		if( optlen!=sizeof(BOOL) )
		{
			nErr = E_FAULT;	//选项的内容或者长度 出错
		}
		else
		{
			if( pTcbUdp->dwOption & TCPIPO_DEBUG )
			{
				*(BOOL*)optval = TRUE;
			}
			else
			{
				*(BOOL*)optval = FALSE;
			}
		}
		break;
	//接收OOB数据
	//case SO_OOBINLINE:
	//	if( optlen!=sizeof(BOOL) )
	//	{
	//		nErr = E_FAULT;	//选项的内容或者长度 出错
	//	}
	//	else
	//	{
	//	}
	//	break;
	//设置接收缓冲的长度
	case SO_RCVBUF:
		if( optlen!=sizeof(int) )
		{
			nErr = E_FAULT;	//选项的内容或者长度 出错
		}
		else
		{
			//*(int*)optval = (int)pTcbUdp->stRxBufRW.dwLenRW;
			*(int*)optval = (int)pTcbUdp->dwLenRcvBuf;
		}
		break;

	//重用本地地址
	//case SO_REUSEADDR:
	//	if( optlen!=sizeof(BOOL) )
	//	{
	//		nErr = E_FAULT;	//选项的内容或者长度 出错
	//	}
	//	else
	//	{
	//	}
	//	break;

	//Unknown，或者不支持
	default :
		nErr = E_NOPROTOOPT;	//选项的名称 Unknown，或者不支持
		break;
	}

	return nErr;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN level-选项的级别
//	IN optname-选项的名称
//	IN optval-选项的内容
//	IN optlen-选项的长度
// 返回值：
//	
// 功能描述：设置选项
// 引用: 
// ********************************************************************
SKERR	UdpSk_SetSockOpt( SOCK_INST* pSockInst, int level, int optname, const char* optval, int optlen )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SKERR			nErr;
	//int				nOptVal;
	DWORD			dwOptVal;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//检查参数：
	if( level!=SOL_SOCKET )
	{
		//if( level!=IPPROTO_IP )
		//{
		//}
		//选项level无效
		return E_INVAL;
	}
	if( !SOC_ISOUT(optname) )
	{
		return E_FAULT;	//选项的内容或者长度 出错
	}
	if( optname & SOC_FIXLEN )
	{
		if( SOC_GETFIXLEN(optname)!=optlen )
		{
			return E_FAULT;	//选项的内容或者长度 出错
		}
	}
	//分类处理：
	nErr = E_SUCCESS;
	switch( optname )
	{
	//输出调试信息
	case SO_DEBUG:
		if( *(BOOL*)optval )
		{
			pTcbUdp->dwOption |= TCPIPO_DEBUG;
		}
		else
		{
			pTcbUdp->dwOption &= ~TCPIPO_DEBUG;
		}
		break;

	//接收OOB数据
	//case SO_OOBINLINE:
	//	break;

	//设置接收缓冲的长度
	case SO_RCVBUF:
		//dwOptVal = *(DWORD*)optval;
		//重新 设置接收缓冲的长度
		//if( !RWBuf_Alloc( &pTcbUdp->stRxBufRW, *(DWORD*)optval, MIN_RXBUF ) )
		if( !Udp_SetRxNodeLen( pTcbUdp, *(DWORD*)optval ) )
		{
			nErr = E_NOBUFS;
		}
		break;

	case SO_SNDBUF:
		break;

	//重用本地地址
	//case SO_REUSEADDR:
	//	break;

	//设置允许收发广播
	case SO_BROADCAST :
		if( *(BOOL*)optval )
		{
			pTcbUdp->dwOption |= TCPIPO_BROADCAST;
		}
		else
		{
			pTcbUdp->dwOption &= ~TCPIPO_BROADCAST;
		}
		break;

	//设置直接使用指定IP接口收发数据的 IP接口索引
	case SO_IPINTF :
		dwOptVal = *(DWORD*)optval;
		if( dwOptVal==0xFFFFFFFF )
		{
			pTcbUdp->dwOption &= ~TCPIPO_IPINTF;
		}
		else
		{
			pTcbUdp->hNetIntf = IP_GetIntfByIndex( dwOptVal );
			if( pTcbUdp->hNetIntf )
			{
				pTcbUdp->dwOption |= TCPIPO_IPINTF;
			}
			else
			{
				pTcbUdp->dwOption &= ~TCPIPO_IPINTF;
				nErr = E_FAULT;	//选项的内容或者长度 出错
			}
		}
		break;

	//Unknown，或者不支持
	default :
		nErr = E_NOPROTOOPT;	//选项的名称 Unknown，或者不支持
		break;
	}

	return nErr;
}



// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	OUT addr-保存地址信息
//	OUT paddr_len-保存地址的长度
//	IN peer-指定对方或者本地
// 返回值：
//	
// 功能描述：获取本地或者对方的地址信息
// 引用: 
// ********************************************************************
SKERR	UdpSk_GetName( SOCK_INST* pSockInst, OUT SOCKADDR* addr, OUT int* paddr_len, int peer )
{
	return E_OPNOTSUPP;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
//	IN how-关闭的方式
// 返回值：
//	
// 功能描述：主动关闭本地端
// 引用: 
// ********************************************************************
SKERR	UdpSk_ShutDown( SOCK_INST* pSockInst, int how )
{
	return E_OPNOTSUPP;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-socket实例信息
// 返回值：
//	
// 功能描述：关闭连接，释放TCB
// 引用: 
// ********************************************************************
SKERR	UdpSk_Close( SOCK_INST* pSockInst )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;

	//句柄安全检查
	if( !HANDLE_CHECK(pTcbUdp) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( !HANDLE_CHECK(lpGlobalUdp) )
	{
		return E_NETDOWN;
	}
	//释放TCB
	Udp_DeleteTCB( (HANDLE)pTcbUdp );

	//保存信息
	pSockInst->hIfTl = NULL;

	return E_SUCCESS;
}




/////////////////////////////////////////////////////////////////////
//
//内部函数
//
/////////////////////////////////////////////////////////////////////

// ********************************************************************
// 声明：
// 参数：
//	IN dwSrcIP-指定的IP
//	IN wSrcPort-指定的端口
//	IN lpGlobalUdp-TCPIP的全局信息
//	IN/OUT ppTcbUdp-要查找的TCB
// 返回值：
//	找到后，返回TRUE，失败返回FALSE
// 功能描述：查找指定的IP和端口 的TCB
// 引用: 
//注释说明：
//关于本地地址对: 在“本地”的“某个IP地址”上的“Port号”，是唯一使用的
// ********************************************************************
BOOL	Udp_FindPort( DWORD dwSrcIP, WORD wSrcPort, UDP_GLOBAL* lpGlobalUdp, UDP_TCB** ppTcbUdp )
{
	UDP_TCB*	pTcbUdp_Find;
	BOOL		fFind = FALSE;
	PLIST_UNIT	pUnitHeader;
	PLIST_UNIT	pUnit;

	pUnitHeader = &lpGlobalUdp->hListTcb;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pTcbUdp_Find = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 
		
		//端口比较
		if( pTcbUdp_Find->wSrcPort==wSrcPort )
		{
			//地址比较
			if( (pTcbUdp_Find->dwSrcIP==dwSrcIP) || 
				(pTcbUdp_Find->dwSrcIP==INADDR_ANY) || 
				(dwSrcIP==INADDR_ANY)
			  )
			{
				fFind = TRUE;
				if( ppTcbUdp )
				{
					*ppTcbUdp = pTcbUdp_Find;
				}
				break;
			}
		}
	}

	return fFind;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalUdp-TCPIP的全局信息
//	IN/OUT ppTcbUdp-要分配新端口的TCB
// 返回值：
//	
// 功能描述：给TCB分配新端口
// 引用: 
// ********************************************************************
BOOL	Udp_NewPort( UDP_TCB* pTcbUdp, UDP_GLOBAL* lpGlobalUdp )
{
	BOOL	fSuccess = FALSE;
	WORD	wPort_Auto;
	WORD	wPort_Back;

	EnterCriticalSection( &lpGlobalUdp->csListTcb );
	wPort_Back = lpGlobalUdp->wPort_Auto;
	wPort_Auto = wPort_Back;
	do
	{
		if( !Udp_FindPort( pTcbUdp->dwSrcIP, ++wPort_Auto, lpGlobalUdp, NULL ) )
		{
			//分配新端口，并且绑定端口
			pTcbUdp->wSrcPort = htons_m(wPort_Auto);
			fSuccess = TRUE;
			break;
		}
		if( wPort_Auto>PORTAUTO_MAX )
		{
			wPort_Auto = PORTAUTO_MIN;
		}
	}while( wPort_Auto!=wPort_Back );
	//
	lpGlobalUdp->wPort_Auto = wPort_Auto;
	LeaveCriticalSection( &lpGlobalUdp->csListTcb );

	return fSuccess;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalUdp-TCPIP的全局信息
//	IN/OUT ppTcbUdp-要使用新端口的TCB
// 返回值：
//	
// 功能描述：给TCB使用新端口
// 引用: 
// ********************************************************************
BOOL	Udp_UsePort( UDP_TCB* pTcbUdp, WORD wSrcPort, UDP_GLOBAL* lpGlobalUdp )
{
	EnterCriticalSection( &lpGlobalUdp->csListTcb );
	if( !Udp_FindPort( pTcbUdp->dwSrcIP, wSrcPort, lpGlobalUdp, NULL ) )
	{
		pTcbUdp->wSrcPort = wSrcPort;
		LeaveCriticalSection( &lpGlobalUdp->csListTcb );
		return TRUE;
	}
	else
	{
		LeaveCriticalSection( &lpGlobalUdp->csListTcb );
		return FALSE;
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN pTblPoll-要查询的实例信息
//	IN nCntPoll-指定pTblPoll中元素的个数
// 返回值：
//	
// 功能描述：查询相应的信息
// 引用: 
// ********************************************************************
int	Udp_PollInfo( POLLTBL* pTblPoll, int nCntPoll )
{
	UDP_TCB*		pTcbUdp;
	DWORD			dwFlag;
	int				nReady;
	int				i;

	//查询每个
	nReady = 0;
	for( i=0; i<nCntPoll; i++ )
	{
		pTcbUdp = (UDP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//句柄安全检查
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			pTblPoll[i].dwFlag |= SOCK_POLL_READ;
			nReady ++;
			continue;
		}
		dwFlag = pTblPoll[i].dwFlag;
		//查可读性
		if( dwFlag & SOCK_POLL_QREAD )
		{
			//查看是否有数据，可以Read
			//if( pTcbUdp->stRxBufRW.dwCntRW )
			if( Udp_GetRxFirstData( pTcbUdp, NULL, NULL, NULL ) )
			{
				pTblPoll[i].dwFlag |= SOCK_POLL_READ;
				nReady ++;
			}
			else if( pTcbUdp->nErr )
			{
				pTblPoll[i].dwFlag |= SOCK_POLL_READ;
				nReady ++;
				//pTcbUdp->nErr = 0;
			}
		}
		//查可写性
		//if( dwFlag & SOCK_POLL_QWRITE )
		//{
		//}
		//查意外性
		//if( dwFlag & SOCK_POLL_QEXCEPT )
		//{
		//}
	}

	return nReady;
}


