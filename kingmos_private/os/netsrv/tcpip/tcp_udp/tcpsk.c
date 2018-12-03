/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：TCP Socket函数表 处理
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
#include "tcp_def.h"


/***************  全局区 定义， 声明 *****************/

//TCP提供的函数表
static	SKERR	TcpSk_Create( SOCK_INST* pSockInst );
static	SKERR	TcpSk_Bind( SOCK_INST* pSockInst, const SOCKADDR* myaddr, int addr_len );
static	SKERR	TcpSk_Connect( SOCK_INST* pSockInst, const SOCKADDR* addr, int addr_len, int flags );
static	SKERR	TcpSk_Listen( SOCK_INST* pSockInst, int queue_len );
static	SKERR	TcpSk_Accept( SOCK_INST* pSockInst, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags );
static	SKERR	TcpSk_IOCtl( SOCK_INST* pSockInst, long nCmd, IN OUT DWORD* pdwArg );
static	SKERR	TcpSk_SendMsg( SOCK_INST* pSockInst, RXTXMSG* pMsgRxTx );
static	SKERR	TcpSk_RecvMsg( SOCK_INST* pSockInst, IN OUT RXTXMSG* pMsgRxTx );
static	SKERR	TcpSk_Poll( POLLTBL* pTblPoll, IN OUT int* pnCntPoll, DWORD dwTmout );

static	SKERR	TcpSk_GetSockOpt( SOCK_INST* pSockInst, int level, int optname, OUT char* optval, int optlen );
static	SKERR	TcpSk_SetSockOpt( SOCK_INST* pSockInst, int level, int optname, const char* optval, int optlen );
static	SKERR	TcpSk_GetName( SOCK_INST* pSockInst, OUT SOCKADDR* addr, OUT int* paddr_len, int peer );

static	SKERR	TcpSk_ShutDown( SOCK_INST* pSockInst, int how );
static	SKERR	TcpSk_Close( SOCK_INST* pSockInst );

//内部辅助函数
static	BOOL	Tcp_FindPort( DWORD dwSrcIP, WORD wSrcPort, TCP_GLOBAL* lpGlobalTcp, TCP_TCB** ppTcbTcp );
static	BOOL	Tcp_NewPort( TCP_TCB* pTcbTcp, TCP_GLOBAL* lpGlobalTcp );
static	BOOL	Tcp_UsePort( TCP_TCB* pTcbTcp, WORD wSrcPort, TCP_GLOBAL* lpGlobalTcp );
static	SKERR	Tcp_AutoBind( TCP_TCB* pTcbTcp, DWORD dwDesIP, TCP_GLOBAL* lpGlobalTcp );

static	BOOL	Tcp_LookSynRcvd( TCP_TCB* pTcbTcp, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR_IN* pAddrIn, OUT int* paddr_len );

static	int	Tcp_PollInfo( POLLTBL* pTblPoll, int nCntPoll );

static	BOOL	Tcp_StartKeepAlive( TCP_TCB* pTcbTcp );
static	void	Tcp_StopKeepAlive( TCP_TCB* pTcbTcp );



/******************************************************/


BOOL	TcpSk_Init( TCP_GLOBAL* lpGlobalTcp )
{
	TL_LOCAL*			pTlLocal;

	//初始化 传输层信息
	pTlLocal = &lpGlobalTcp->LocalTl;
	HANDLE_SET( pTlLocal );
	List_InitHead( &pTlLocal->hListTl );
	pTlLocal->hTlGlobal = (HANDLE)lpGlobalTcp;

	pTlLocal->nFamily = AF_INET;
	pTlLocal->nType = SOCK_STREAM;
	pTlLocal->nProtocol = IPPROTO_TCP;

	//填充 传输层函数表
	pTlLocal->pFnTlOpen      = TcpSk_Create    ;
	pTlLocal->pFnTlBind      = TcpSk_Bind      ;
	pTlLocal->pFnTlConnect   = TcpSk_Connect   ;
	pTlLocal->pFnTlListen    = TcpSk_Listen    ;
	pTlLocal->pFnTlAccept    = TcpSk_Accept    ;
	pTlLocal->pFnTlIoctl     = TcpSk_IOCtl     ;
	pTlLocal->pFnTlSendmsg   = TcpSk_SendMsg   ;
	pTlLocal->pFnTlRecvmsg   = TcpSk_RecvMsg   ;
	pTlLocal->pFnTlPoll      = TcpSk_Poll      ;

	pTlLocal->pFnTlGetSockOpt= TcpSk_GetSockOpt;
	pTlLocal->pFnTlSetSockOpt= TcpSk_SetSockOpt;
	pTlLocal->pFnTlGetName   = TcpSk_GetName   ;

	pTlLocal->pFnTlShutDown  = TcpSk_ShutDown  ;
	pTlLocal->pFnTlClose     = TcpSk_Close     ;

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
SKERR	TcpSk_Create( SOCK_INST* pSockInst )
{
	TCP_GLOBAL*	lpGlobalTcp;
	TCP_TCB*	pTcbTcp;

	//句柄安全检查
	lpGlobalTcp = (TCP_GLOBAL*)pSockInst->pTlLocal->hTlGlobal;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
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
	if( !Tcp_CreateTcb( &pTcbTcp, lpGlobalTcp ) )
	{
		//空间不够而 失败
		return E_NOBUFS;
	}
	//初始化
	pTcbTcp->dwSrcIP = INADDR_ANY;
	pTcbTcp->dwState = TCPS_CLOSED;
	//关联到
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	List_InsertTail( &lpGlobalTcp->hListTcb, &pTcbTcp->hListTcb );
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	//保存结果
	pSockInst->hIfTl = (HANDLE)pTcbTcp;

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
SKERR	TcpSk_Bind( SOCK_INST* pSockInst, const SOCKADDR* myaddr, int addr_len )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)myaddr;
	DWORD			dwSrcIP;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pAddrIn || addr_len<sizeof(SOCKADDR) )
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
	if( pTcbTcp->wSrcPort )
	{
		//该socket已经被绑定
		return E_INVAL;
	}

	//绑定本地IP地址
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
	pTcbTcp->dwSrcIP = dwSrcIP;
	//绑定本地端口
	if( pAddrIn->sin_port==0 )
	{
		//分配新端口，并且绑定端口
		if( !Tcp_NewPort( pTcbTcp, lpGlobalTcp ) )
		{
			pTcbTcp->dwSrcIP = INADDR_ANY;
			//分配新端口失败
			return E_NOMOREPORT;
		}
	}
	else
	{
		//检查该端口是否被占用，并且绑定端口
		if( !Tcp_UsePort(pTcbTcp, pAddrIn->sin_port, lpGlobalTcp) )
		{
			pTcbTcp->dwSrcIP = INADDR_ANY;
			//表明该端口被占用
			return E_ADDRINUSE;
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
SKERR	TcpSk_Connect( SOCK_INST* pSockInst, const SOCKADDR* addr, int addr_len, int flags )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;
	SKERR			nErr;
	DWORD			dwDesIP;
	DWORD			dwWait;
	WORD			wMSS_me;

	//RETAILMSG(1,(TEXT("  TcpSk_Connect: Enter\r\n")));
	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--1\r\n")));
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--2\r\n")));
		return E_NETDOWN;
	}
	//检查参数：
	if( !pAddrIn || (addr_len<sizeof(SOCKADDR_IN)) )
	{
		//参数出错
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--3\r\n")));
		return E_FAULT;
	}
	if( pAddrIn->sin_family!=AF_INET )
	{
		//不支持的协议族
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--4\r\n")));
		return E_AFNOSUPPORT;
	}
	dwDesIP = pAddrIn->sin_addr.S_un.S_addr;
	if( dwDesIP==INADDR_ANY || INADDR_TEST_BC(dwDesIP) || pAddrIn->sin_port==0 )
	{
		//远程地址无效---客户不能连接到 “任何地址” 和 “广播地址”
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--5\r\n")));
		return E_ADDRNOTAVAIL;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--6\r\n")));
		return E_NETDOWN;
	}
	//检查状态：
	if( pTcbTcp->dwState==TCPS_SYN_SENT )
	{
		//连接的操作正在进行
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--7\r\n")));
		return E_ALREADY;
	}
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		//连接已经建立
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--8\r\n")));
		return E_ISCONN;
	}
	if( (pTcbTcp->dwState!=TCPS_CLOSED) || (pTcbTcp->wAttr & TCPA_LISTEN) )
	{
		//该socket可能正处在监听(listening)等
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--9\r\n")));
		return E_INVAL;
	}

	//确保 dwSrcIP and wSrcPort---“必需要绑定或自动绑定固定IP”和PORT
	if( (pTcbTcp->dwSrcIP==INADDR_ANY) || (pTcbTcp->wSrcPort==0) )
	{
		//自动绑定
		//HeapValidate( GetProcessHeap(), 0, 0 );
		if( nErr=Tcp_AutoBind( pTcbTcp, dwDesIP, lpGlobalTcp ) )
		{
			RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--10\r\n")));
			return nErr;
		}
	}
	//避免数据发给自己（这个IP，这个PORT）
	if( (pTcbTcp->wSrcPort==pAddrIn->sin_port) && (pTcbTcp->dwSrcIP==dwDesIP) )
	{
		//远程地址无效
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--11\r\n")));
		return E_ADDRNOTAVAIL;
	}

	//保存远程地址
	pTcbTcp->dwDesIP = dwDesIP;
	pTcbTcp->wDesPort = pAddrIn->sin_port;
	//查询 IP层网络信息 和 本地MSS
	if( !Tcp_QueryMSS(pTcbTcp->dwSrcIP, pTcbTcp->dwDesIP, &wMSS_me) )
	{
		//网络层失败
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--12\r\n")));
		return E_NETDOWN;
	}
	pTcbTcp->wSnd_MSS = wMSS_me;
	pTcbTcp->wRcv_MSS = wMSS_me;

	//进入工作状态
	pTcbTcp->dwState = TCPS_SYN_SENT;
	pTcbTcp->wAttr |= TCPA_CONNECT;

	//准备 发送操作
	if( !TcpOut_StartSend(pTcbTcp) )
	{
		//一些系统操作失败
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--13\r\n")));
		pTcbTcp->dwState = TCPS_CLOSED;
		pTcbTcp->wAttr &= ~TCPA_CONNECT;
		return E_SYSFUN;
	}
	//启动 发送操作
	SetEvent( pTcbTcp->hEvtSend );

	//判断 非阻塞操作
	if( pTcbTcp->dwOption & TCPIPO_NONBLOCK )
	{
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave ok--Nonblocking\r\n")));
		return E_WOULDBLOCK;
	}

	//操作结果
	//RETAILMSG(1,(TEXT("  TcpSk_Connect: then Wait...\r\n")));
	dwWait = WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_CONNECT );
	//判断结果---连接成功
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		//RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave OK\r\n")));
		return E_SUCCESS;
	}
	//判断结果---设置退出
	pTcbTcp->dwState = TCPS_CLOSED;
	pTcbTcp->wFlag = 0;
	if( dwWait==WAIT_TIMEOUT )
	{
		//超时
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--15\r\n")));
		return E_TIMEDOUT;
	}
	else
	{
		//指定错误
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--16\r\n")));
		if( pTcbTcp->nErr )
		{
			return pTcbTcp->nErr;
		}
		else
		{
			return E_TIMEDOUT;
		}
	}
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
SKERR	TcpSk_Listen( SOCK_INST* pSockInst, int queue_len )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//检查状态：
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		//连接已经建立
		return E_ISCONN;
	}
	if( (pTcbTcp->dwState!=TCPS_CLOSED) || (pTcbTcp->wAttr & TCPA_CONNECT) )
	{
		//该socket可能正处在监听(listening)等
		return E_INVAL;
	}
	if( pTcbTcp->wSrcPort==0 )
	{
		//还没有绑定bind
		return E_INVAL;
	}

	//保存参数
	if( (DWORD)queue_len>(DWORD)MAXWORD )
	{
		pTcbTcp->wMaxConn = MAXWORD;
	}
	else if( pTcbTcp->wMaxConn==0 )
	{
		pTcbTcp->wMaxConn = SOMAXCONN;
	}
	else
	{
		pTcbTcp->wMaxConn = (WORD)queue_len;
	}

	//进入工作状态
	pTcbTcp->dwState = TCPS_LISTEN;
	pTcbTcp->wAttr |= TCPA_LISTEN;

	return 0;
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
SKERR	TcpSk_Accept( SOCK_INST* pSockInst, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pAddrIn || (*paddr_len<sizeof(SOCKADDR_IN)) )
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
	//检查状态：
	if( (pTcbTcp->dwState!=TCPS_LISTEN) || !(pTcbTcp->wAttr & TCPA_LISTEN) )
	{
		//请先调用listen
		return E_INVAL;
	}
	//检查是否队列已满
	if( pTcbTcp->wCntConn > pTcbTcp->wMaxConn )
	{
		//接收连接的个数已满
		return E_MFILE;
	}

	//
	*paddr_len = 0;
	memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
	while( (pTcbTcp->dwState==TCPS_LISTEN) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//判断本地IP地址是否有效---防止拔卡操作
		if( (pTcbTcp->dwSrcIP!=INADDR_ANY) && !IP_IsValidAddr( pTcbTcp->dwSrcIP ) )
		{
			//该IP地址无效
			return E_ADDRNOTAVAIL;
		}

		//检查是否有连接
		if( Tcp_LookSynRcvd( pTcbTcp, pSockInst_New, pAddrIn, paddr_len ) )
		{
			//已经获取到了
			return E_SUCCESS;
		}
		//检查是否队列已满
		if( pTcbTcp->wCntConn > pTcbTcp->wMaxConn )
		{
			//接收连接的个数已满
			return E_MFILE;
		}

		//Nonblocking操作
		if( pTcbTcp->dwOption & TCPIPO_NONBLOCK )
		{
			//启动了 异步操作
			return E_WOULDBLOCK;
		}

		//退出
		if( (pTcbTcp->dwState!=TCPS_LISTEN) || (pTcbTcp->wFlag & TCPF_EXIT) )
		{
			break;
		}
		//等待连接的进入
		WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_WAITACCEPT );//INFINITE
	}

	//被中断退出
	return E_INTR;
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
SKERR	TcpSk_IOCtl( SOCK_INST* pSockInst, long nCmd, IN OUT DWORD* pdwArg )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SKERR			nErr;
	//int				nOptVal;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
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
			pTcbTcp->dwOption |= TCPIPO_NONBLOCK;
		}
		else
		{
			pTcbTcp->dwOption &= ~TCPIPO_NONBLOCK;
		}
		break;

	//获取接收缓冲里数据的长度
	case FIONREAD :
		*pdwArg = pTcbTcp->dwUsedRxBufLen;//pTcbTcp->stRxBufRW.dwCntRW;
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
SKERR	TcpSk_SendMsg( SOCK_INST* pSockInst, RXTXMSG* pMsgRxTx )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	DWORD			dwLenAll;
	DWORD			dwLenData;
	DWORD			dwLenTmp;
	//BOOL			fStartSend;
	//SKERR			nErr;
	//DWORD			dwTickStart;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pMsgRxTx || !pMsgRxTx->pData )
	{
		//参数出错
		return E_FAULT;
	}
	if( !(dwLenData=pMsgRxTx->dwLenData) )
	{
		return E_SUCCESS;
	}
	dwLenAll = dwLenData;
	pMsgRxTx->dwLenData = 0;
	//if( dwLenData > pTcbTcp->stTxBufRW.dwLenRW )
	//{
	//	//数据包太大 失败
	//	return E_MSGSIZE;
	//}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//检查状态：
	if( !(pTcbTcp->wAttr & TCPA_ESTABLISH) )
	{
		//请先建立连接
		return E_NOTCONN;
	}
	if( pTcbTcp->dwLenLeftTx )
	{
		return E_INPROGRESS;
	}

	//
	//pTcbTcp->dwLenLeftTx += dwLenData;
	pTcbTcp->dwLenLeftTx = dwLenData;
	//dwTickStart = GetTickCount();
	while( !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//判断 网络连接
		if( pTcbTcp->wAttr & TCPA_ERR_ALL )
		{
			if( pTcbTcp->wAttr & TCPA_CONNRESET )
			{
				//网络连接被对方断开
				return E_CONNRESET;
			}
			if( !(pTcbTcp->wAttr & TCPA_SEND) || (pTcbTcp->wFlag & TCPF_LFIN) )
			{
				//网络连接被SHUTDOWN
				return E_SHUTDOWN;
			}
			if( pTcbTcp->wAttr & TCPA_NETRESET )
			{
				//keep-alive检测到网络连接失败
				return E_NETRESET;
			}
			if( pTcbTcp->wAttr & TCPA_HOST_UNREACH )
			{
				//主机不可到达
				return E_HOSTUNREACH;
			}
			if( pTcbTcp->wAttr & TCPA_CONN_ABORTED )
			{
				//连接终止
				return E_CONNABORTED;
			}
		}
		if( !(pTcbTcp->wAttr & TCPA_SEND) || (pTcbTcp->wFlag & TCPF_LFIN) )
		{
			//网络连接被SHUTDOWN
			return E_SHUTDOWN;
		}

		//查看是否还有数据发送
		//RETAILMSG(1,(TEXT("  TcpSk_SendMsg: dwLenData=[%d]\r\n"), dwLenData));
		if( dwLenData )
		{
			//是否启动 发送操作
			//if( pTcbTcp->stTxBufRW.dwCntRW==0 )
			//{
			//	fStartSend = TRUE;
			//}
			//else
			//{
			//	fStartSend = FALSE;
			//}
			//拷贝数据到“发送缓冲里的剩余空间”
			dwLenTmp = dwLenData;
			if( RWBuf_WriteData( &pTcbTcp->stTxBufRW, pMsgRxTx->pData, &dwLenTmp, 0 ) )
			{
				pTcbTcp->dwSnd_max += dwLenTmp;//添加发送的数据

				pTcbTcp->dwLenLeftTx -= dwLenTmp;
				dwLenData -= dwLenTmp;
			}
			//else
			//{
			//	fStartSend = FALSE;
			//}
			//是否启动 发送操作
			//if( fStartSend )
			{
				//启动 发送操作
				PulseEvent( pTcbTcp->hEvtSend );
				//SetEvent( pTcbTcp->hEvtSend );
			}
		}

		//查看是否数据发送完成
		if( (pTcbTcp->stTxBufRW.dwCntRW==0) && !dwLenData )
		{
			pMsgRxTx->dwLenData = dwLenAll;
			//已经发送完了
			RETAILMSG(1,(TEXT("  TcpSk_SendMsg: ok dwLenAll=[%d]\r\n"), dwLenAll));
			return E_SUCCESS;
		}
		//if( pTcbTcp->nErr )
		//{
		//	nErr = pTcbTcp->nErr;
		//	pTcbTcp->nErr = 0;
		//	return nErr;
		//}
		if( pTcbTcp->dwOption & TCPIPO_NONBLOCK )
		{
			//启动了 异步操作
			return E_WOULDBLOCK;
		}

		//等待发送的完成
		WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_WAITSEND );
		//
		//if( (GetTickCount()-dwTickStart) > TCPTM_SEND )
		//{
		//	break;
		//}
	}

	//被中断退出
	return E_TIMEDOUT;
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
BOOL TCP_ReadData( TCP_TCB* pTcbTcp, LPVOID pData, LPDWORD pdwLenData );
SKERR	TcpSk_RecvMsg( SOCK_INST* pSockInst, IN OUT RXTXMSG* pMsgRxTx )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	LPBYTE			pData;
	DWORD			dwLenData;
	SOCKADDR_IN*	pAddrIn;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
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
	//检查状态：
	if( !(pTcbTcp->wAttr & TCPA_ESTABLISH) )
	{
		//请先建立连接
		return E_NOTCONN;
	}
	//RETAILMSG( 1, ( "TcpSk_RecvMsg while.\r\n" ) );
	//循环接收数据
	while( !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		DWORD dwReadLen = dwLenData;
		//判断 接收数据
		if( TCP_ReadData( pTcbTcp, pData, &dwReadLen ) )
		{
			//RETAILMSG(1,(TEXT("  TcpSk_RecvMsg: read[%d]\r\n"), dwReadLen));
			pMsgRxTx->dwLenData = dwReadLen;
			if( pMsgRxTx->lpPeer )
			{
				pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
				memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
				pAddrIn->sin_family = AF_INET;
				pAddrIn->sin_port = pTcbTcp->wDesPort;
				pAddrIn->sin_addr.S_un.S_addr = pTcbTcp->dwDesIP;
				pMsgRxTx->dwLenPeer = sizeof(SOCKADDR_IN);
			}
			return E_SUCCESS;
		}
		//判断 网络连接
		if( pTcbTcp->wAttr & TCPA_ERR_ALL )
		{
			if( pTcbTcp->wAttr & TCPA_CONNRESET )
			{
				//网络连接被对方断开
				return E_CONNRESET;
			}
			if( !(pTcbTcp->wAttr & TCPA_RECV) || (pTcbTcp->wFlag & TCPF_LFIN) )
			{
				//网络连接被SHUTDOWN
				return E_SHUTDOWN;
			}
			if( pTcbTcp->wAttr & TCPA_NETRESET )
			{
				//keep-alive检测到网络连接失败
				return E_NETRESET;
			}
			if( pTcbTcp->wAttr & TCPA_HOST_UNREACH )
			{
				//主机不可到达
				return E_HOSTUNREACH;
			}
			if( pTcbTcp->wAttr & TCPA_CONN_ABORTED )
			{
				//连接终止
				return E_CONNABORTED;
			}
		}
		if( !(pTcbTcp->wAttr & TCPA_RECV) || (pTcbTcp->wFlag & TCPF_LFIN) )
		{
			//网络连接被SHUTDOWN
			return E_SHUTDOWN;
		}

		//判断 接收数据
		//if( RWBuf_ReadData( &pTcbTcp->stRxBufRW, pData, &dwReadLen ) )
/* //lilin remove to up
		if( TCP_ReadData( pTcbTcp, pData, &dwReadLen ) )
		{
			//RETAILMSG(1,(TEXT("  TcpSk_RecvMsg: read[%d]\r\n"), dwReadLen));
			pMsgRxTx->dwLenData = dwReadLen;
			if( pMsgRxTx->lpPeer )
			{
				pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
				memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
				pAddrIn->sin_family = AF_INET;
				pAddrIn->sin_port = pTcbTcp->wDesPort;
				pAddrIn->sin_addr.S_un.S_addr = pTcbTcp->dwDesIP;
				pMsgRxTx->dwLenPeer = sizeof(SOCKADDR_IN);
			}
			return E_SUCCESS;
		}
*/
		//判断 非阻塞操作
		if( pTcbTcp->dwOption & TCPIPO_NONBLOCK )
		{
			if( pMsgRxTx->lpPeer )
			{
				pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
				memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
				pAddrIn->sin_family = AF_INET;
				pAddrIn->sin_port = pTcbTcp->wDesPort;
				pAddrIn->sin_addr.S_un.S_addr = pTcbTcp->dwDesIP;
				pMsgRxTx->dwLenPeer = sizeof(SOCKADDR_IN);
			}
			return E_WOULDBLOCK;
		}

		//等待发送的完成
		WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_WAITRECV );
	}

	//被中断退出
	return E_TIMEDOUT;
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
SKERR	TcpSk_Poll( POLLTBL* pTblPoll, IN OUT int* pnCntPoll, DWORD dwTmout )
{
	TCP_TCB*		pTcbTcp;
	HANDLE*			pArrEvt;
	int				nCntWait;
	int				i;
	int				nPos;
	DWORD			dwTickStart;
	DWORD			dwTickWait;
	DWORD			dwWait;
	BOOL			fWaittingForYou;

	//
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
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
	*pnCntPoll = Tcp_PollInfo( pTblPoll, nCntWait );
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
		pTcbTcp = (TCP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//句柄安全检查
		if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
		{
			free( pArrEvt );
			return E_NOTSOCK;
		}
		pArrEvt[nPos++] = pTcbTcp->hEvtPoll;
	}
	if( !nPos )
	{
		return E_FAULT;
	}
	//开始等待
	if( dwTmout<3000 )
	{
		dwTickWait = dwTmout;
		fWaittingForYou = FALSE;
	}
	else
	{
		dwTickWait = 3000;
		if( dwTmout==INFINITE )
		{
			fWaittingForYou = TRUE;
		}
		else
		{
			fWaittingForYou = FALSE;
		}
	}
	dwTickStart = GetTickCount();
	while( 1 )
	{
		dwWait = WaitForMultipleObjects( nPos, pArrEvt, FALSE, dwTickWait );
		*pnCntPoll = Tcp_PollInfo( pTblPoll, nCntWait );
		if( *pnCntPoll )
		{
			//已经获取信息
			return E_SUCCESS;
		}
		else if( !fWaittingForYou )
		{
			dwWait = GetTickCount()-dwTickStart;
			if( dwWait>=dwTmout )
			{
				//超时退出
				return E_INVAL;
			}
			else
			{
				//如果剩余时间 比较少
				if( (dwWait+3000)>dwTmout )
				{
					dwTickWait = dwTmout-dwWait;
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
SKERR	TcpSk_GetSockOpt( SOCK_INST* pSockInst, int level, int optname, OUT char* optval, int optlen )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SKERR			nErr;
	//int				nOptVal;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
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
			if( pTcbTcp->dwOption & TCPIPO_DEBUG )
			{
				*(BOOL*)optval = TRUE;
			}
			else
			{
				*(BOOL*)optval = FALSE;
			}
		}
		break;
	//关闭保活机制
	case SO_KEEPALIVE:
		if( optlen!=sizeof(BOOL) )
		{
			nErr = E_FAULT;	//选项的内容或者长度 出错
		}
		else
		{
			if( pTcbTcp->dwOption & TCPIPO_NONKEEPALIVE )
			{
				*(BOOL*)optval = FALSE;
			}
			else
			{
				*(BOOL*)optval = TRUE;
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
			*(int*)optval = pTcbTcp->dwUsedRxBufLen;//(int)pTcbTcp->stRxBufRW.dwLenRW;
		}
		break;
	//设置发送缓冲的长度
	case SO_SNDBUF:
		if( optlen!=sizeof(int) )
		{
			nErr = E_FAULT;	//选项的内容或者长度 出错
		}
		else
		{
			*(int*)optval = (int)pTcbTcp->stTxBufRW.dwLenRW;
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
SKERR	TcpSk_SetSockOpt( SOCK_INST* pSockInst, int level, int optname, const char* optval, int optlen )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SKERR			nErr;
	//int				nOptVal;
	//DWORD			dwOptVal;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( level!=SOL_SOCKET )
	{
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
		if( *(BOOL*)optval )
		{
			pTcbTcp->dwOption |= TCPIPO_DEBUG;
		}
		else
		{
			pTcbTcp->dwOption &= ~TCPIPO_DEBUG;
		}
		break;
	//关闭保活机制
	case SO_KEEPALIVE:
		if( *(BOOL*)optval )
		{
			if( !Tcp_StartKeepAlive(pTcbTcp) )
			{
				nErr = E_FAULT;	//选项的内容或者长度 出错
			}
		}
		else
		{
			Tcp_StopKeepAlive( pTcbTcp );
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
		//重新 设置接收缓冲的长度
//		if( !RWBuf_Alloc( &pTcbTcp->stRxBufRW, *(DWORD*)optval, MIN_RXBUF ) )
//		{
//			nErr = E_NOBUFS;
//		}
		pTcbTcp->dwMaxRxBufSize = MAX( *(DWORD*)optval, MIN_RXBUF );
		break;
	//设置发送缓冲的长度
	case SO_SNDBUF:
		//重新 设置接收缓冲的长度
		if( !RWBuf_Alloc( &pTcbTcp->stTxBufRW, *(DWORD*)optval, MIN_TXBUF ) )
		{
			nErr = E_NOBUFS;
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
//	OUT addr-保存地址信息
//	OUT paddr_len-保存地址的长度
//	IN peer-指定对方或者本地
// 返回值：
//	
// 功能描述：获取本地或者对方的地址信息
// 引用: 
// ********************************************************************
SKERR	TcpSk_GetName( SOCK_INST* pSockInst, OUT SOCKADDR* addr, OUT int* paddr_len, int peer )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查参数：
	if( !pAddrIn || (*paddr_len<sizeof(SOCKADDR_IN)) )
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
	if( pTcbTcp->dwState!=TCPS_ESTABLISHED )
	{
		//连接已经断开
		return E_NOTCONN;
	}

	//获取地址信息
	memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
	pAddrIn->sin_family = AF_INET;
	if( peer )
	{
		pAddrIn->sin_port = pTcbTcp->wDesPort;
		pAddrIn->sin_addr.S_un.S_addr = pTcbTcp->dwDesIP;
	}
	else
	{
		pAddrIn->sin_port = pTcbTcp->wSrcPort;
		pAddrIn->sin_addr.S_un.S_addr = pTcbTcp->dwSrcIP;
	}

	return E_SUCCESS;
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
SKERR	TcpSk_ShutDown( SOCK_INST* pSockInst, int how )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	//DWORD			dwTickStart;

	//句柄安全检查
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//检查网络
	if( !IP_IsUp() )
	{
		//网络层失败
		return E_NETDOWN;
	}
	//检查状态：
	if( !(pTcbTcp->wAttr & TCPA_ESTABLISH) )
	{
		//请先建立连接
		return E_NOTCONN;
	}
	if( pTcbTcp->dwState==TCPS_CLOSED )
	{
		return E_SUCCESS;
	}

	//设置是否要收发数据
	switch( how )
	{
	case SD_RECEIVE:
		pTcbTcp->wAttr &= ~TCPA_RECV;
		break;
	case SD_SEND:
		pTcbTcp->wAttr &= ~TCPA_SEND;
		break;
	case SD_BOTH:
		pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
		break;
	default:
		break;
	}
	
	//进入FIN状态
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		pTcbTcp->dwState = TCPS_FIN_WAIT1;
		
		SetEvent( pTcbTcp->hEvtSend );
	}

	//需要等待到TCPS_CLOSED吗？
	//dwTickStart = GetTickCount();
	//while( (pTcbTcp->dwState!=TCPS_CLOSED) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	//{
	//	//等待连接的进入
	//	WaitForSingleObject( pTcbTcp->hEvtPoll, 2000 );
	//	if( (GetTickCount()-dwTickStart) > TCPTM_WAITSHUT )
	//	{
	//		break;
	//	}
	//}

	return E_SUCCESS;
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
SKERR	TcpSk_Close( SOCK_INST* pSockInst )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;

	//句柄安全检查
	if( !HANDLE_CHECK(pTcbTcp) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( !HANDLE_CHECK(lpGlobalTcp) )
	{
		return E_NETDOWN;
	}
	//设置退出标志
	pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	pTcbTcp->wFlag |= TCPF_EXIT;
	pTcbTcp->nErr = E_INTR;
	Sleep( 1 );
	//检查状态：
	if( (pTcbTcp->dwState!=TCPS_CLOSED) &&
		(pTcbTcp->dwState!=TCPS_LISTEN) &&
		(pTcbTcp->dwSrcIP) && (pTcbTcp->dwDesIP) && 
		(pTcbTcp->wSrcPort) && (pTcbTcp->wDesPort)
	  )
	{
		//发送RST，通知对方退出
		if( (pTcbTcp->dwSrcIP) && (pTcbTcp->dwDesIP) && (pTcbTcp->wSrcPort) && (pTcbTcp->wDesPort) )
		{
			TcpOut_SendRst( pTcbTcp, pTcbTcp->dwSrcIP, pTcbTcp->wSrcPort, pTcbTcp->dwDesIP, pTcbTcp->wDesPort, 0 );
		}
	}
	//通知自己退出
	if( pTcbTcp->hEvtPoll )
	{
		PulseEvent( pTcbTcp->hEvtPoll );
	}
	if( pTcbTcp->hEvtSend )
	{
		SetEvent( pTcbTcp->hEvtSend );
	}
	Sleep( 1 );
	Sleep( 1 );

	//如果有的话，释放所有 hListSynRcvd 列表
	if( pTcbTcp->wAttr & TCPA_LISTEN )
	{
		TCP_TCB*		pTcbSynRcvd;
		PLIST_UNIT		pUnit;
		PLIST_UNIT		pUnitHeader;

		pUnitHeader = &pTcbTcp->hListSynRcvd;
		EnterCriticalSection( &pTcbTcp->csTcb );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			//根据结构成员地址，得到结构地址，并继续
			pTcbSynRcvd = LIST_CONTAINER( pUnit, TCP_TCB, hListSynRcvd );
			pUnit = pUnit->pNext;
			
			//开始释放
			Sleep( 1 );
			List_RemoveUnit( &pTcbSynRcvd->hListSynRcvd );
			List_InitHead( &pTcbSynRcvd->hListSynRcvd );
			Sleep( 1 );
			Tcp_DeleteTCB( pTcbSynRcvd );
		}
		LeaveCriticalSection( &pTcbTcp->csTcb );
	}

	//释放TCB
	Sleep( 1 );
	Tcp_DeleteTCB( pTcbTcp );

	//保存信息
	pSockInst->hIfTl = NULL;

	return E_SUCCESS;
}




/////////////////////////////////////////////////////////////////////
//
//内部辅助函数
//
/////////////////////////////////////////////////////////////////////


// ********************************************************************
// 声明：
// 参数：
//	IN dwSrcIP-指定的IP
//	IN wSrcPort-指定的端口
//	IN lpGlobalTcp-TCPIP的全局信息
//	IN/OUT ppTcbTcp-要查找的TCB
// 返回值：
//	找到后，返回TRUE，失败返回FALSE
// 功能描述：查找指定的IP和端口 的TCB
// 引用: 
//注释说明：
//关于本地地址对: 在“本地”的“某个IP地址”上的“Port号”，是唯一使用的
// ********************************************************************
BOOL	Tcp_FindPort( DWORD dwSrcIP, WORD wSrcPort, TCP_GLOBAL* lpGlobalTcp, TCP_TCB** ppTcbTcp )
{
	TCP_TCB*	pTcbTcp_Find;
	BOOL		fFind = FALSE;
	PLIST_UNIT	pUnitHeader;
	PLIST_UNIT	pUnit;

	pUnitHeader = &lpGlobalTcp->hListTcb;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//根据结构成员地址，得到结构地址，并继续
		pTcbTcp_Find = LIST_CONTAINER( pUnit, TCP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 
		
		//端口比较
		if( pTcbTcp_Find->wSrcPort==wSrcPort )
		{
			//地址比较
			if( (pTcbTcp_Find->dwSrcIP==dwSrcIP) || 
				(pTcbTcp_Find->dwSrcIP==INADDR_ANY) || 
				(dwSrcIP==INADDR_ANY)
			  )
			{
				fFind = TRUE;
				if( ppTcbTcp )
				{
					*ppTcbTcp = pTcbTcp_Find;
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
//	IN lpGlobalTcp-TCPIP的全局信息
//	IN/OUT ppTcbTcp-要分配新端口的TCB
// 返回值：
//	
// 功能描述：给TCB分配新端口
// 引用: 
// ********************************************************************
BOOL	Tcp_NewPort( TCP_TCB* pTcbTcp, TCP_GLOBAL* lpGlobalTcp )
{
	BOOL	fSuccess = FALSE;
	WORD	wPort_Auto;
	WORD	wPort_Back;

	//HeapValidate( GetProcessHeap(), 0, 0 );
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	//HeapValidate( GetProcessHeap(), 0, 0 );
	wPort_Back = lpGlobalTcp->wPort_Auto;
	wPort_Auto = wPort_Back;
	do
	{
		if( !Tcp_FindPort( pTcbTcp->dwSrcIP, ++wPort_Auto, lpGlobalTcp, NULL ) )
		{
			//分配新端口，并且绑定端口
			pTcbTcp->wSrcPort = htons_m(wPort_Auto);
			fSuccess = TRUE;
			break;
		}
		if( wPort_Auto>PORTAUTO_MAX )
		{
			wPort_Auto = PORTAUTO_MIN;
		}
	}while( wPort_Auto!=wPort_Back );
	//
	lpGlobalTcp->wPort_Auto = wPort_Auto;
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	return fSuccess;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpGlobalTcp-TCPIP的全局信息
//	IN/OUT ppTcbTcp-要使用新端口的TCB
// 返回值：
//	
// 功能描述：给TCB使用新端口
// 引用: 
// ********************************************************************
BOOL	Tcp_UsePort( TCP_TCB* pTcbTcp, WORD wSrcPort, TCP_GLOBAL* lpGlobalTcp )
{
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	if( !Tcp_FindPort( pTcbTcp->dwSrcIP, wSrcPort, lpGlobalTcp, NULL ) )
	{
		pTcbTcp->wSrcPort = wSrcPort;
		LeaveCriticalSection( &lpGlobalTcp->csListTcb );
		return TRUE;
	}
	else
	{
		LeaveCriticalSection( &lpGlobalTcp->csListTcb );
		return FALSE;
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT ppTcbTcp-要自动绑定的TCB
//	IN lpGlobalTcp-TCPIP的全局信息
// 返回值：
//	
// 功能描述：对TCB自动绑定源地址对
// 引用: 
// ********************************************************************
SKERR	Tcp_AutoBind( TCP_TCB* pTcbTcp, DWORD dwDesIP, TCP_GLOBAL* lpGlobalTcp )
{
	//获取缺省的IP地址
	if( pTcbTcp->dwSrcIP==INADDR_ANY )
	{
		if( !IP_GetBestAddr( &pTcbTcp->dwSrcIP, dwDesIP, TRUE ) )
		{
			//该IP地址无效
			return E_ADDRNOTAVAIL;
		}
	}

	//分配1个新端口
	if( pTcbTcp->wSrcPort!=0 )
	{
		return E_SUCCESS;
	}
	//HeapValidate( GetProcessHeap(), 0, 0 );
	if( Tcp_NewPort( pTcbTcp, lpGlobalTcp ) )
	{
		return E_SUCCESS;
	}
	else
	{
		return E_NOMOREPORT;
	}
}

// ********************************************************************
// 声明：
// 参数：
//	IN pTcbTcp-TCB实例信息
//	IN pSockInst_New-接收连接的socket实例信息
//	IN pAddrIn-指定目的地址信息
//	IN paddr_len-保存指定addr的长度
// 返回值：
//	
// 功能描述：检索接收目的的连接的信息
// 引用: 
// ********************************************************************
BOOL	Tcp_LookSynRcvd( TCP_TCB* pTcbTcp, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR_IN* pAddrIn, OUT int* paddr_len )
{
	TCP_TCB*		pTcbTcp_New;
	TCP_TCB*		pTcbSynRcvd;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	//
	EnterCriticalSection( &pTcbTcp->csTcb );
	//
	if( List_IsEmpty(&pTcbTcp->hListSynRcvd) )
	{
		LeaveCriticalSection( &pTcbTcp->csTcb );
		//如果没有连接，直接返回
		//RETAILMSG(1,(TEXT("  Tcp_LookSynRcvd: No hListSynRcvd\r\n")));
		return FALSE;
	}
	//
	pTcbTcp_New = NULL;
	pUnitHeader = &pTcbTcp->hListSynRcvd;
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//根据结构成员地址，得到结构地址，并继续
		pTcbSynRcvd = LIST_CONTAINER( pUnit, TCP_TCB, hListSynRcvd );
		pUnit = pUnit->pNext;
		
		//注意： tcp_in.c 可以通知退出
		if( (pTcbSynRcvd->wFlag & TCPF_EXIT) || ((GetTickCount()-pTcbSynRcvd->dwTickLife)>=TCPTM_SYNRCVD) )
		{
			//需要被释放
			pTcbTcp->wCntConn --;
			List_RemoveUnit( &pTcbSynRcvd->hListSynRcvd );
			List_InitHead( &pTcbSynRcvd->hListSynRcvd );
			Sleep( 1 );
			Tcp_DeleteTCB( pTcbSynRcvd );
			RETAILMSG(1,(TEXT("  Tcp_LookSynRcvd: Yes---but delete\r\n")));
			continue ;
		}
		if( !pTcbTcp_New && (pTcbSynRcvd->dwState==TCPS_ESTABLISHED) )
		{
			//从accept上 hListSynRcvd队列 中取出来
			pTcbTcp->wCntConn --;
			pTcbTcp_New = pTcbSynRcvd;
			pTcbTcp_New->pTcbParent = NULL;
			List_RemoveUnit( &pTcbTcp_New->hListSynRcvd );
			List_InitHead( &pTcbTcp_New->hListSynRcvd );
			RETAILMSG(1,(TEXT("  Tcp_LookSynRcvd: Yes---get\r\n")));
		}
	}
	//
	LeaveCriticalSection( &pTcbTcp->csTcb );

	//取、存
	if( pTcbTcp_New && !(pTcbTcp_New->wFlag & TCPF_EXIT) )
	{
		EnterCriticalSection( &pTcbTcp_New->csTcb );
		//添加到ppSockInst_New
		pSockInst_New->hIfTl = (HANDLE)pTcbTcp_New;
		//获取地址信息
		pAddrIn->sin_family = AF_INET;
		pAddrIn->sin_port = pTcbTcp_New->wDesPort;
		pAddrIn->sin_addr.S_un.S_addr = pTcbTcp_New->dwDesIP;
		*paddr_len = sizeof(SOCKADDR_IN);
		LeaveCriticalSection( &pTcbTcp_New->csTcb );
		RETAILMSG(1,(TEXT("  Tcp_LookSynRcvd: Yes---get xxxx\r\n")));
		return TRUE;
	}

	return FALSE;
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
int	Tcp_PollInfo( POLLTBL* pTblPoll, int nCntPoll )
{
	TCP_TCB*		pTcbTcp;
	DWORD			dwFlag;
	int				nReady;
	int				i;

	//查询每个
	nReady = 0;
	for( i=0; i<nCntPoll; i++ )
	{
		pTcbTcp = (TCP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//句柄安全检查
		if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
		{
			pTblPoll[i].dwFlag |= SOCK_POLL_READ;
			nReady ++;
			continue;
		}
		dwFlag = pTblPoll[i].dwFlag;
		//查可读性
		if( dwFlag & SOCK_POLL_QREAD )
		{
			if( pTcbTcp->dwState==TCPS_ESTABLISHED )
			{
				//查看是否有数据，可以Read
				//if( pTcbTcp->stRxBufRW.dwCntRW )
				if( pTcbTcp->dwUsedRxBufLen )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_READ;
					nReady ++;
				}
			}
			else if( pTcbTcp->dwState==TCPS_LISTEN )
			{
				if( pTcbTcp->wAttr & TCPA_LISTEN )
				{
					//accept是否得到1个新连接---accept支持Nonblocking操作
					if( pTcbTcp->wFlag & TCPQ_LISTEN_YES )
					{
						pTblPoll[i].dwFlag |= SOCK_POLL_READ;
						nReady ++;
						
						pTcbTcp->wFlag &= ~TCPQ_LISTEN_YES;
					}
				}
			}
			else if( (pTcbTcp->dwState!=TCPS_SYN_SENT) || (pTcbTcp->dwState!=TCPS_SYN_RECVD) )
			{
				//连接是否进入closed/reset/terminated
				pTblPoll[i].dwFlag |= SOCK_POLL_READ;
				nReady ++;
			}
		}
		//查可写性
		if( dwFlag & SOCK_POLL_QWRITE )
		{
			if( pTcbTcp->dwState==TCPS_ESTABLISHED )
			{
				//查看数据是否发送完成，可以继续Write
				if( (pTcbTcp->stTxBufRW.dwCntRW==0) && !( pTcbTcp->dwLenLeftTx) )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_WRITE;
					nReady ++;
				}
				//查看connect是否发送完成，可以继续Write
				else if( pTcbTcp->wAttr & TCPA_CONNECT )
				{
					if( pTcbTcp->wFlag & TCPQ_CNN_YES )
					{
						pTblPoll[i].dwFlag |= SOCK_POLL_WRITE;
						nReady ++;
						
						pTcbTcp->wFlag &= ~TCPQ_CNN_YES;
					}
				}
			}
		}
		//查意外性
		if( dwFlag & SOCK_POLL_QEXCEPT )
		{
			//查看connect是否失败
			if( pTcbTcp->wAttr & TCPA_CONNECT )
			{
				if( pTcbTcp->wFlag & TCPQ_CNN_NO )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_EXCEPT;
					nReady ++;
					
					pTcbTcp->wFlag &= ~TCPQ_CNN_NO;
				}
			}
			//查看accept是否失败
			if( pTcbTcp->wAttr & TCPA_LISTEN )
			{
				//accept是否得到1个新连接---accept支持Nonblocking操作
				if( pTcbTcp->wFlag & TCPQ_LISTEN_NO )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_EXCEPT;
					nReady ++;
					
					pTcbTcp->wFlag &= ~TCPQ_LISTEN_NO;
				}
			}
			//查看是否有OOB数据可读
			//
		}
	}

	return nReady;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pTcbTcp-TCB实例信息
// 返回值：
//	
// 功能描述：启动保活机制
// 引用: 
// ********************************************************************
BOOL	Tcp_StartKeepAlive( TCP_TCB* pTcbTcp )
{
	pTcbTcp->dwOption &= ~TCPIPO_NONKEEPALIVE;

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pTcbTcp-TCB实例信息
// 返回值：
//	
// 功能描述：关闭保活机制
// 引用: 
// ********************************************************************
void	Tcp_StopKeepAlive( TCP_TCB* pTcbTcp )
{
	pTcbTcp->dwOption |= TCPIPO_NONKEEPALIVE;
}

