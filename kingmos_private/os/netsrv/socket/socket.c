/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：Socket
版本号：  2.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
//"\inc_app"
#include <eobjlist.h>
#include <socket.h>
#include <iphlpapi.h>
//"\inc_local"
#include "netif_skinst.h"
#include "skt_call.h"
#include "iphlpapi_call.h"
#include "dns_call.h"
//"local"
#include "socket_def.h"

#define SKT_ID_USE_PTR

/***************  全局区 定义， 声明 *****************/
//
extern	void	Icmp_CheckProc( HANDLE hProc );

//-----------------------------------------------------
		SOCK_GLOBAL*		g_lpGlobalSock = NULL;


static	BOOL	Sock_IsLocalHost( const char* pName );


static	BOOL	Sock_FindTl( OUT TL_LOCAL** ppTlLocal, int af, int type, int protocol );

#define	PROCF_USECS			0x01
#define	PROCF_REMOVE		0x02
static	BOOL	Sock_FindProc( OUT SOCK_PROC** ppSockProc, HANDLE hProc, DWORD dwOption );

static	BOOL	Sock_CreateProc( OUT SOCK_PROC** ppSockProc );
static	void	Sock_DeleteProc( SOCK_PROC* pSockProc );
static	void	Sock_DestroyProc( SOCK_PROC* pSockProc );

static	SOCKET	Sock_NewID( OUT SOCK_INST* pSockInst, SOCK_PROC* pSockProc );
static	SOCKET	Sock_CreateInst( OUT SOCK_INST** ppSockInst, SOCK_PROC* pSockProc, TL_LOCAL* pTlLocal );
static	void	Sock_DestroyInst( SOCK_INST* pSockInst, SOCK_PROC* pSockProc );
//#define	INSTF_REMOVE		0x02
static	BOOL	Sock_FindInstAndProc( OUT SOCK_INST** ppSockInst, IN OUT SOCK_PROC** ppSockProc, SOCKET s, DWORD dwOption );


static	int		Sock_NewPollTbl( OUT POLLTBL** ppTblPoll, OUT SOCK_INST** ppSockInst, SOCK_PROC** ppSockProc, fd_set* readfds, fd_set* writefds, fd_set* exceptfds );
static	void	Sock_LookPollTbl( POLLTBL* pTblPoll, int nCntAll, OUT fd_set* readfds, OUT fd_set* writefds, OUT fd_set* exceptfds );


//-----------------------------------------------------



/******************************************************/


// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：初始化
// 引用: 
// ********************************************************************
BOOL	Sock_Init( )
{
	SOCK_GLOBAL*		lpGlobalSock;

	lpGlobalSock = (SOCK_GLOBAL*)HANDLE_ALLOC( sizeof(SOCK_GLOBAL) );
	if( !lpGlobalSock )
	{
		return FALSE;
	}
	HANDLE_INIT( lpGlobalSock, sizeof(SOCK_GLOBAL) );
	List_InitHead( &lpGlobalSock->hListTl );
	List_InitHead( &lpGlobalSock->hListSkProc );
	InitializeCriticalSection( &lpGlobalSock->csListTl );
	InitializeCriticalSection( &lpGlobalSock->csListSkProc );

	g_lpGlobalSock = lpGlobalSock;
	return TRUE;
}

void	Sock_Deinit( )
{
}

// ********************************************************************
// 声明：
// 参数：
//	IN pTlLocal-传输层信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：注册1个传输层
// 引用: 
// ********************************************************************
BOOL	Sock_Register( TL_LOCAL* pTlLocal )
{
	TL_LOCAL*		pTlLocal_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	EnterCriticalSection( &g_lpGlobalSock->csListTl );
	//首先判断是否已经存在了
	pUnitHeader = &g_lpGlobalSock->hListTl;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pTlLocal_Find = LIST_CONTAINER( pUnit, TL_LOCAL, hListTl );
		pUnit = pUnit->pNext;    // advance to next 
		
		//比较
		if( (pTlLocal_Find->nFamily==pTlLocal->nFamily) && (pTlLocal_Find->nType==pTlLocal->nType) )
		{
			LeaveCriticalSection( &g_lpGlobalSock->csListTl );
			//重复注册失败
			return FALSE;
		}
	}
	//注册1个传输层
	List_InsertTail( &g_lpGlobalSock->hListTl, &pTlLocal->hListTl );
	LeaveCriticalSection( &g_lpGlobalSock->csListTl );

	return TRUE;
}

DWORD	WINAPI	SKAPI_CallBack( HANDLE hServer, DWORD dwServerCode, DWORD dwParam, LPVOID lpParam )
{
	if( (dwServerCode==SCC_BROADCAST_PROCESS_EXIT) && dwParam )
	{
		HANDLE			hProc = (HANDLE)dwParam;
		SOCK_PROC*		pSockProc;

		//
		if( Sock_FindProc( &pSockProc, hProc, PROCF_USECS |PROCF_REMOVE ) )
		{
			//需要释放
			Sock_DestroyProc( pSockProc );
		}
		//
		Icmp_CheckProc( hProc );
		//
	}
	else
	{
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}

	return 0;
}


// ********************************************************************
// 声明：
// 参数：
//	IN wVersionRequired-指定socket版本
//	OUT lpWSAData-保存获取的socket版本和描述信息
// 返回值：
//	成功，返回0;失败，返回错误数值
// 功能描述：启动网络功能，获取socket版本和描述信息
// 引用: 
// ********************************************************************
SKERR	WINAPI	SKAPI_WSAStartup( WORD wVersionRequired, OUT LPWSADATA lpWSAData )
{
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalSock) )
	{
		return E_VERNOTSUPPORTED;
	}
	if( wVersionRequired==0x0101 )
	{
		SOCK_PROC*	pSockProc;

		//有可能启动多次
		if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
		{
			//分配该进程的所有SOCKET信息
			if( !Sock_CreateProc( &pSockProc ) )
			{
				return E_NOBUFS;
			}
		}
		//
		lpWSAData->wVersion = 0x0101;
		lpWSAData->wHighVersion = 0x0101;
		lpWSAData->iMaxSockets = SOCKET_MAX;
		lpWSAData->iMaxUdpDg = 1000;

		strcpy( lpWSAData->szDescription,"Kingmos socket 1.0" );
		lpWSAData->lpVendorInfo = 0;
		lpWSAData->szSystemStatus[0] = 0;

		return E_SUCCESS;
	}
	else
	{
		return E_VERNOTSUPPORTED;
	}
}

// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：释放网络功能
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_WSACleanup(void)
{
	SOCK_PROC*		pSockProc;

	//
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS |PROCF_REMOVE ) )
	{
		return 0;
	}
	//需要释放
	Sock_DestroyProc( pSockProc );
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN wVersionRequired-指定socket版本
// 返回值：
//	无
// 功能描述：设置socket的最近错误
// 引用: 
// ********************************************************************
void	WINAPI	SKAPI_WSASetLastError( SKERR nErrSock )
{
	SOCK_PROC*		pSockProc;

	//
	if( Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		pSockProc->nErrSock = nErrSock;
	}
}

// ********************************************************************
// 声明：
// 参数：
//	IN wVersionRequired-指定socket版本
// 返回值：
//	返回socket的最近错误
// 功能描述：获取socket的最近错误
// 引用: 
// ********************************************************************
SKERR	WINAPI	SKAPI_WSAGetLastError(void)
{
	SOCK_PROC*		pSockProc;

	//
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return E_NOTINITIALISED;
	}
	return pSockProc->nErrSock;
}


// ********************************************************************
// 声明：
// 参数：
//	IN af-指定的地址协议族
//	IN type-指定的Socket类型
//	IN protocol-指定的协议类型
// 返回值：
//	成功，返回SOCKET;失败，返回INVALID_SOCKET
// 功能描述：创建SOCKET
// 引用: 
// ********************************************************************
SOCKET	WINAPI	SKAPI_Socket( int af, int type, int protocol )
{
	SOCK_PROC*		pSockProc;
	TL_LOCAL*		pTlLocal;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return INVALID_SOCKET;
	}
	//寻找 对应的传输层TL
	if( !Sock_FindTl( &pTlLocal, af, type, protocol ) )
	{
		pSockProc->nErrSock = E_AFNOSUPPORT;
		return INVALID_SOCKET;
	}
	//新建1个SOCK_INST，并初始化
	if( Sock_CreateInst(&pSockInst, pSockProc, pTlLocal)==INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}
	//调用 传输层执行
	if( nErr=pTlLocal->pFnTlOpen(pSockInst) )
	{
		//请删除
		Sock_DestroyInst( pSockInst, pSockProc );
		pSockProc->nErrSock = nErr;
		return INVALID_SOCKET;
	}
	
	return pSockInst->sock_id;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN myaddr-指定本地地址信息
//	IN addr_len-指定myaddr的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：绑定源地址对
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Bind( SOCKET s, const SOCKADDR* myaddr, int addr_len )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr = pSockInst->pTlLocal->pFnTlBind(pSockInst, myaddr, addr_len) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN addr-指定目的地址信息
//	IN addr_len-指定addr的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：连接到目的地址对
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Connect( SOCKET s, const SOCKADDR* addr, int addr_len )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		RETAILMSG(1,(TEXT("\r\nSKAPI_Connect: connect s=[%x], Failed!--1\r\n"), s));
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr = pSockInst->pTlLocal->pFnTlConnect(pSockInst, addr, addr_len, 0) )
	{
		RETAILMSG(1,(TEXT("\r\nSKAPI_Connect: connect s=[%x], Failed!--2, nErr=%d\r\n"), s, nErr));
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN queue_len-指定接收连接个数的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：将socket进入监听状态，并设置接收连接个数
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Listen( SOCKET s, int queue_len )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr = pSockInst->pTlLocal->pFnTlListen(pSockInst, queue_len) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN addr-指定目的地址信息
//	IN paddr_len-保存指定addr的长度
// 返回值：
//	成功，返回SOCKET;失败，返回INVALID_SOCKET
// 功能描述：接收目的的连接，并返回它的地址对
// 引用: 
// ********************************************************************
SOCKET	WINAPI	SKAPI_Accept( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len )
{
	SOCK_PROC*		pSockProc;
	int				flags=0;
	SOCK_INST*		pSockInst_New;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return INVALID_SOCKET;
	}
	//新建1个SOCK_INST，并初始化
	if( Sock_CreateInst(&pSockInst_New, pSockProc, pSockInst->pTlLocal)==INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlAccept(pSockInst, pSockInst_New, (SOCKADDR*)addr, paddr_len, flags) )
	{
		//请删除
		Sock_DestroyInst( pSockInst_New, pSockProc );
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return pSockInst_New->sock_id;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN cmd-指定命令
//	IN argp-指定参数信息
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：IO设置
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_IoctlSocket( SOCKET s, long cmd, IN OUT DWORD* argp )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlIoctl(pSockInst, cmd, argp) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN buf-指定发送数据的Buffer
//	IN len-指定发送数据的长度
//	IN flags-指定发送数据的标志
// 返回值：
//	成功，返回发送数据的长度;失败，返回SOCKET_ERROR
// 功能描述：发送数据
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Send( SOCKET s, const char* buf, int len, int flags )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	memset( &MsgRxTx, 0, sizeof(RXTXMSG) );
	MsgRxTx.pData = (LPBYTE)buf;
	MsgRxTx.dwLenData = (DWORD)len;
	MsgRxTx.dwFlag = (DWORD)flags;
	if( nErr=pSockInst->pTlLocal->pFnTlSendmsg(pSockInst, &MsgRxTx) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return MsgRxTx.dwLenData;
}


// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN buf-指定发送数据的Buffer
//	IN len-指定发送数据的长度
//	IN flags-指定发送数据的标志
//	IN to-指定目的地址信息
//	IN tolen-指定to的长度
// 返回值：
//	成功，返回发送数据的长度;失败，返回SOCKET_ERROR
// 功能描述：发送数据
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_SendTo( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	memset( &MsgRxTx, 0, sizeof(RXTXMSG) );
	MsgRxTx.pData = (LPBYTE)buf;
	MsgRxTx.dwLenData = (DWORD)len;
	MsgRxTx.dwFlag = (DWORD)flags;
	MsgRxTx.lpPeer = (char*)to;
	MsgRxTx.dwLenPeer = (DWORD)tolen;
	if( nErr=pSockInst->pTlLocal->pFnTlSendmsg(pSockInst, &MsgRxTx) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return MsgRxTx.dwLenData;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	OUT buf-指定接收数据的Buffer
//	IN len-指定接收数据的长度
//	IN flags-指接收送数据的标志
// 返回值：
//	成功，返回接收数据的长度;失败，返回SOCKET_ERROR
// 功能描述：接收数据
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Recv( SOCKET s, OUT char* buf, int len, int flags )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	memset( &MsgRxTx, 0, sizeof(RXTXMSG) );
	MsgRxTx.pData = (LPBYTE)buf;
	MsgRxTx.dwLenData = (DWORD)len;
	MsgRxTx.dwFlag = (DWORD)flags;
	if( nErr=pSockInst->pTlLocal->pFnTlRecvmsg(pSockInst, &MsgRxTx) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return MsgRxTx.dwLenData;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	OUT buf-指定接收数据的Buffer
//	IN len-指定接收数据的长度
//	IN flags-指接收送数据的标志
//	OUT from-指定目的地址信息
//	IN fromlen-指定from的长度
// 返回值：
//	成功，返回接收数据的长度;失败，返回SOCKET_ERROR
// 功能描述：接收数据
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_RecvFrom( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, OUT int* fromlen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	memset( &MsgRxTx, 0, sizeof(RXTXMSG) );
	MsgRxTx.pData = (LPBYTE)buf;
	MsgRxTx.dwLenData = (DWORD)len;
	MsgRxTx.dwFlag = (DWORD)flags;
	MsgRxTx.lpPeer = (char*)from;
	MsgRxTx.dwLenPeer = (DWORD)fromlen;
	if( nErr=pSockInst->pTlLocal->pFnTlRecvmsg(pSockInst, &MsgRxTx) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return MsgRxTx.dwLenData;
}


// ********************************************************************
// 声明：
// 参数：
//	IN nfds-保留
//	IN readfds-可读性的SOCKET集
//	IN writefds-可写性的SOCKET集
//	IN exceptfds-意外性的SOCKET集
//	IN timeout-等待超时的时间
// 返回值：
//	成功，返回等到有效事件的个数;失败，返回SOCKET_ERROR
// 功能描述：Select操作
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	POLLTBL*		pTblPoll;
	int				nCntAll;
	int				nPos;
	DWORD			dwTmout;
	SKERR			nErr;

	//
#ifdef SKT_ID_USE_PTR
	pSockProc = NULL;
#else
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return INVALID_SOCKET;
	}
#endif
	//
	if( !(nPos=Sock_NewPollTbl(&pTblPoll, &pSockInst, &pSockProc, readfds, writefds, exceptfds)) )
	{
		return SOCKET_ERROR;
	}
	nCntAll = nPos;
	//
	if( timeout )
	{
		if( (timeout->tv_sec==INFINITE) || (timeout->tv_usec==INFINITE) )
		{
			//dwTmout = 0;
			dwTmout = INFINITE;
		}
		else
		{
			dwTmout = timeout->tv_sec * 1000 + timeout->tv_usec;
		}
	}
	else
	{
		dwTmout = INFINITE;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlPoll(pTblPoll, &nPos, dwTmout) )
	{
		pSockProc->nErrSock = nErr;
		free( pTblPoll );
		return SOCKET_ERROR;
	}

	//根据pFnTlPoll获得的pTblPoll，重新设置 readfds, writefds, exceptfds内容
	Sock_LookPollTbl( pTblPoll, nCntAll, readfds, writefds, exceptfds );

	free( pTblPoll );
	return nPos;
}


// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN level-选项的级别
//	IN optname-选项的名称
//	OUT optval-选项的内容
//	IN optlen-选项的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：获取选项
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_GetSockOpt( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlGetSockOpt(pSockInst, level, optname, optval, *optlen) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN level-选项的级别
//	IN optname-选项的名称
//	IN optval-选项的内容
//	IN optlen-选项的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：设置选项
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_SetSockOpt( SOCKET s, int level, int optname, const char* optval, int optlen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlSetSockOpt(pSockInst, level, optname, optval, optlen) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	OUT addr-保存地址信息
//	OUT paddr_len-保存地址的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：获取对方的地址信息
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_GetPeerName( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlGetName(pSockInst, name, namelen, 1) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	OUT addr-保存地址信息
//	OUT paddr_len-保存地址的长度
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：获取本地的地址信息
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_GetSockName( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlGetName(pSockInst, name, namelen, 0) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
//	IN how-关闭的方式
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：主动关闭本地端
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_ShutDown( SOCKET s, int how )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//获取 socket实例
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//调用 传输层执行
	if( nErr=pSockInst->pTlLocal->pFnTlShutDown(pSockInst, how) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// 声明：
// 参数：
//	IN s-指定的SOCKET
// 返回值：
//	成功，返回0;失败，返回SOCKET_ERROR
// 功能描述：关闭本地端
// 引用: 
// ********************************************************************
SKRET	WINAPI	SKAPI_CloseSocket( SOCKET s )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	
	//获取 socket实例
#ifdef SKT_ID_USE_PTR
	pSockProc = NULL;
#else
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return FALSE;
	}
#endif
	// lilin 2004-12-03
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )//INSTF_REMOVE ) )
	{
		return SOCKET_ERROR;
	}
	Sock_DestroyInst( pSockInst, pSockProc );
	return 0;
}


///////////       -----------------       ///////////



SKRET	WINAPI	SKAPI_GetHostName( OUT char* name, int namelen )
{
	//DWORD	dwSize = namelen;

	//检查
	if( !HANDLE_CHECK(g_lpGlobalSock) )
	{
		return INVALID_SOCKET;
	}
	//GetComputerNameEx( ComputerNameNetBIOS, name, &dwSize );

	return INVALID_SOCKET;
	return 0;
}

HOSTENT*	WINAPI	SKAPI_GetHostByName( const char* name )
{
	SOCK_PROC*		pSockProc;
	
	//获取 socket实例
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return NULL;
	}
	pSockProc->nErrSock = E_SOCKTNOSUPPORT;
	return NULL;
}
HOSTENT*	WINAPI	SKAPI_GetHostByAddr( const char* addr, int len, int type )
{
	SOCK_PROC*		pSockProc;
	
	//获取 socket实例
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return NULL;
	}
	pSockProc->nErrSock = E_SOCKTNOSUPPORT;
	return NULL;
}

//成功，返回获取地址信息的个数；失败，返回SOCKET_ERROR，请检查GetLastError()
HOSTENT*	WINAPI	SKAPI_GetHostByNameEx( const char* name, OUT LPBYTE pBufInfo, int len )
{
	HOSTENT*		pHostInfo = (HOSTENT*)pBufInfo;
	SOCK_PROC*		pSockProc;
	SKERR			nErr;
	DWORD			dwIP;
	WORD			wCntAddr;

	//获取 socket实例
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return NULL;
	}
	//检查这个最起码的长度---可以保存1个地址
	if( len < (sizeof(HOSTENT) +(sizeof(LPVOID)+ADDRLEN_IPV4)*1 +sizeof(LPVOID)) )
	{
		pSockProc->nErrSock = E_FAULT;//E_NOBUFS
		return NULL;
	}
	pHostInfo->h_name      = (char*)name;
	pHostInfo->h_addrtype  = AF_UNKNOWN;//htons_m(DNS_RRTYPE_A);

	//首先检查字符是否合格

	//再检查是否就是IP地址---直接填充地址信息返回
	dwIP = inet_addr( name );
	if( dwIP!=INADDR_NONE )
	{
		wCntAddr = 1;
		pHostInfo->h_aliases   = NULL;
		if( Skt_SavHostentAddr( pHostInfo, (DWORD)len, (LPBYTE)&dwIP, ADDRLEN_IPV4, &wCntAddr ) )
		{
			return pHostInfo;
		}
		else
		{
			pSockProc->nErrSock = E_NOBUFS;
			return NULL;
		}
	}

	//比较是否为本地主机名称---直接枚举
	if( Sock_IsLocalHost(name) )
	{
		DWORD				dwCntAddr;
		BYTE				pBufAddrs[10*ADDRLEN_IPV4];

		//请枚举本地的 所有IP接口的信息
		if( IPHAPI_GetIpAddrList( pBufAddrs, sizeof(pBufAddrs), &dwCntAddr ) && dwCntAddr )
		{
			wCntAddr = (WORD)dwCntAddr;
			if( Skt_SavHostentAddr( pHostInfo, (DWORD)len, pBufAddrs, ADDRLEN_IPV4, &wCntAddr ) )
			{
				return pHostInfo;
			}
			else
			{
				pSockProc->nErrSock = E_NOBUFS;
				return NULL;
			}
		}
		else
		{
			//pSockProc->nErrSock = ;
			return NULL;
		}
	}
	
	//比较是否为主机名称---采用NBT协议查询---目前不支持

	//如果是域名---采用DNS协议查询
	wCntAddr = 0;
	nErr = DnsCall_Query( name, pHostInfo, len, &wCntAddr, 0 );
	if( wCntAddr )
	{
		//g_lpGlobalSock->dwOptDns = !g_lpGlobalSock->dwOptDns;
		return pHostInfo;
	}
	else
	{
		//g_lpGlobalSock->dwOptDns = 0;
		pSockProc->nErrSock = nErr;
		return NULL;
	}
}

HOSTENT*	WINAPI	SKAPI_GetHostByAddrEx( const char* addr, int len, int type, OUT LPBYTE pBufInfo, int lenInfo )
{
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
//	OUT ppTlLocal-保存寻找的传输层TL
//	IN af-指定的地址协议族
//	IN type-指定的Socket类型
//	IN protocol-指定的协议类型
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：寻找 对应的传输层TL
// 引用: 
// ********************************************************************
BOOL	Sock_FindTl( OUT TL_LOCAL** ppTlLocal, int af, int type, int protocol )
{
	TL_LOCAL*	pTlLocal_Find;
	PLIST_UNIT	pUnitHeader;
	PLIST_UNIT	pUnit;

	*ppTlLocal = NULL;
	pUnitHeader = &g_lpGlobalSock->hListTl;
	EnterCriticalSection( &g_lpGlobalSock->csListTl );
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pTlLocal_Find = LIST_CONTAINER( pUnit, TL_LOCAL, hListTl );
		pUnit = pUnit->pNext;    // advance to next 
		
		//比较
		if( (pTlLocal_Find->nFamily==af) && (pTlLocal_Find->nType==type) )
		{
			*ppTlLocal = pTlLocal_Find;
			break;
		}
	}
	LeaveCriticalSection( &g_lpGlobalSock->csListTl );

	return (*ppTlLocal)?TRUE:FALSE;
}




// ********************************************************************
// 声明：
// 参数：
//	OUT ppSockProc-保存寻找的SOCK_PROC
//	IN hProc-指定的进程
//	IN fUse-指定是否互斥
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：寻找 指定的进程的SOCK_PROC
// 引用: 
// ********************************************************************
BOOL	Sock_FindProc( OUT SOCK_PROC** ppSockProc, HANDLE hProc, DWORD dwOption )
{
	SOCK_PROC*	pSockProc_Find;
	PLIST_UNIT	pUnitHeader;
	PLIST_UNIT	pUnit;

	//
	*ppSockProc = NULL;
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalSock) )
	{
		return FALSE;
	}
	//
	//pUnitHeader = &g_lpGlobalSock->hListSkProc; //lilin remove
	if( dwOption & PROCF_USECS )
	{
		EnterCriticalSection( &g_lpGlobalSock->csListSkProc );
	}
	pUnitHeader = &g_lpGlobalSock->hListSkProc; //lilin add
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pSockProc_Find = LIST_CONTAINER( pUnit, SOCK_PROC, hListSkProc );
		pUnit = pUnit->pNext;    // advance to next 
		
		if( !HANDLE_CHECK(pSockProc_Find) )
		{
			continue;
		}
		//比较
		if( pSockProc_Find->hProc==hProc )
		{
			if( dwOption & PROCF_REMOVE )
			{
				List_RemoveUnit( &pSockProc_Find->hListSkProc );
			}
			*ppSockProc = pSockProc_Find;
			break;
		}
	}
	if( dwOption & PROCF_USECS )
	{
		LeaveCriticalSection( &g_lpGlobalSock->csListSkProc );
	}

	return (*ppSockProc)?TRUE:FALSE;
}

// ********************************************************************
// 声明：
// 参数：
//	OUT ppSockProc-保存创建的SOCK_PROC实例信息
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：创建SOCK_PROC
// 引用: 
// ********************************************************************
BOOL	Sock_CreateProc( OUT SOCK_PROC** ppSockProc )
{
	SOCK_PROC*		pSockProc;
	SOCK_PROC*		pSockProc_Find;

	//分配
	pSockProc = (SOCK_PROC*)HANDLE_ALLOC( sizeof(SOCK_PROC) );
	if( !pSockProc )
	{
		return FALSE;
	}
	HANDLE_INIT( pSockProc, sizeof(SOCK_PROC) );
	List_InitHead( &pSockProc->hListSkProc );
	List_InitHead( &pSockProc->hListSkInst );
	InitializeCriticalSection( &pSockProc->csListSkInst );
	//初始化
	pSockProc->hProc = GetCallerProcess( );

	//添加
	EnterCriticalSection( &g_lpGlobalSock->csListSkProc );
	if( Sock_FindProc( &pSockProc_Find, pSockProc->hProc, 0 ) )
	{
		//释放他
		Sock_DeleteProc( pSockProc );
		//获取找到的
		pSockProc = pSockProc_Find;
	}
	else
	{
		List_InsertTail( &g_lpGlobalSock->hListSkProc, &pSockProc->hListSkProc );
	}
	LeaveCriticalSection( &g_lpGlobalSock->csListSkProc);

	*ppSockProc = pSockProc;
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN ppSockProc-SOCK_PROC实例信息
// 返回值：
//	无
// 功能描述：删除SOCK_PROC
// 引用: 
// ********************************************************************
void	Sock_DeleteProc( SOCK_PROC* pSockProc )
{
	//句柄安全检查
	//if( !HANDLE_CHECK(g_lpGlobalSock) || !HANDLE_CHECK(pSockProc) )
	if( !HANDLE_CHECK(pSockProc) )
	{
		return ;
	}
	//
	DeleteCriticalSection( &pSockProc->csListSkInst );
	//
	HANDLE_FREE( pSockProc );
}

// ********************************************************************
// 声明：
// 参数：
//	IN ppSockProc-SOCK_PROC实例信息
// 返回值：
//	无
// 功能描述：销毁SOCK_PROC
// 引用: 
// ********************************************************************
void	Sock_DestroyProc( SOCK_PROC* pSockProc )
{
	SOCK_INST*		pSockInst_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//断开
	EnterCriticalSection( &g_lpGlobalSock->csListSkProc);
	List_RemoveUnit( &pSockProc->hListSkProc );
	LeaveCriticalSection( &g_lpGlobalSock->csListSkProc);

	//寻找 SockInst，并销毁
	EnterCriticalSection( &pSockProc->csListSkInst );
	pUnitHeader = &pSockProc->hListSkInst;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pSockInst_Find = LIST_CONTAINER( pUnit, SOCK_INST, hListSkInst );
		//pUnit = pUnit->pNext;    // advance to next  //lilin remove

		//句柄安全检查
		if( !HANDLE_CHECK(pSockInst_Find) )
		{
			continue;
		}
		//销毁
		Sock_DestroyInst( pSockInst_Find, pSockProc );
		pUnit = pUnitHeader->pNext;    // advance to next  //lilin add
	}
	LeaveCriticalSection( &pSockProc->csListSkInst );

	//
	Sock_DeleteProc( pSockProc );
}



// ********************************************************************
// 声明：
// 参数：
//	OUT pSockInst-指定的SockInst信息
//	IN pSockProc-指定的SockProc信息
// 返回值：
//	成功，返回SOCKET;失败，返回INVALID_SOCKET
// 功能描述：分配新SOCKET
// 引用: 
// ********************************************************************
SOCKET	Sock_NewID( OUT SOCK_INST* pSockInst, SOCK_PROC* pSockProc )
{
	SOCKET			sock_id;

	EnterCriticalSection( &pSockProc->csListSkInst );
#ifdef SKT_ID_USE_PTR
	sock_id = (SOCKET)PTR_TO_HANDLE( pSockInst );
	pSockProc->nNewID ++;
#else
	sock_id = ++pSockProc->nNewID;
	if( pSockProc->fToEnd )
	{
		//需要判断---暂时保留
	}
	else if( sock_id>=SKTID_MAX )
	{
		pSockProc->fToEnd = TRUE;
		
		pSockProc->nNewID = SKTID_MIN;	//attention here
	}
#endif
	//保存
	if( sock_id!=INVALID_SOCKET )
	{
		pSockInst->sock_id = sock_id;
	}
	LeaveCriticalSection( &pSockProc->csListSkInst );

	return sock_id;
}

// ********************************************************************
// 声明：
// 参数：
//	OUT ppSockInst-保存创建的SOCKET实例信息
//	IN pSockProc-指定的SockProc信息
//	IN pTlLocal-指定的传输层TL信息
// 返回值：
//	成功，返回SOCKET;失败，返回INVALID_SOCKET
// 功能描述：创建SOCK_INST
// 引用: 
// ********************************************************************
SOCKET	Sock_CreateInst( OUT SOCK_INST** ppSockInst, SOCK_PROC* pSockProc, TL_LOCAL* pTlLocal )
{
	SOCK_INST*		pSockInst;

	//分配
	pSockInst = (SOCK_INST*)HANDLE_ALLOC( sizeof(SOCK_INST) );
	if( !pSockInst )
	{
		pSockProc->nErrSock = E_NOBUFS;
		return INVALID_SOCKET;
	}
	HANDLE_INIT( pSockInst, sizeof(SOCK_INST) );
	List_InitHead( &pSockInst->hListSkInst );
	//初始化
	pSockInst->pTlLocal = pTlLocal;
	pSockInst->lpSkProc = (LPVOID)pSockProc;
	//添加
	EnterCriticalSection( &pSockProc->csListSkInst );
	List_InsertTail( &pSockProc->hListSkInst, &pSockInst->hListSkInst );
	LeaveCriticalSection( &pSockProc->csListSkInst);

	//分配 socket ID
	if( Sock_NewID( pSockInst, pSockProc )==INVALID_SOCKET )
	{
		//请删除
		Sock_DestroyInst( pSockInst, pSockProc );
		//
		pSockProc->nErrSock = E_MFILE;
		return INVALID_SOCKET;
	}

	*ppSockInst = pSockInst;
	return pSockInst->sock_id;
}


// ********************************************************************
// 声明：
// 参数：
//	IN pSockInst-SOCK_INST实例信息
//	IN pSockProc-指定的SockProc信息
// 返回值：
//	无
// 功能描述：销毁SOCK_INST
// 引用: 
// ********************************************************************
void	Sock_DestroyInst( SOCK_INST* pSockInst, SOCK_PROC* pSockProc )
{
	BOOL bFree = FALSE;
	//断开
	ASSERT( pSockProc && pSockInst );
	if( pSockProc && pSockInst )
	{
		EnterCriticalSection( &pSockProc->csListSkInst );

		if( HANDLE_CHECK( pSockInst ) )
		{
			pSockInst->wFlag |= SKINSTF_EXIT;
#ifdef SKT_ID_USE_PTR
			pSockProc->nNewID --;
#endif
			List_RemoveUnit( &pSockInst->hListSkInst );
			bFree = TRUE;
		}

		LeaveCriticalSection( &pSockProc->csListSkInst);
	}

	if( bFree )
	{
		//调用 传输层执行
		if( pSockInst->pTlLocal && pSockInst->hIfTl )
		{
			//Sleep( 3 );
			pSockInst->pTlLocal->pFnTlClose( pSockInst );
			//Sleep( 3 );
		}
		//
		HANDLE_FREE( pSockInst );
	}
}

// ********************************************************************
// 声明：
// 参数：
//	OUT ppSockInst-保存寻找的SOCK_INST
//	IN/OUT ppSockProc-保存寻找的SOCK_PROC
//	IN s-指定的SOCKET
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：寻找 指定的进程的SOCK_PROC和SOCK_INST
// 引用: 
// ********************************************************************

#ifdef SKT_ID_USE_PTR
BOOL	Sock_FindInstAndProc( OUT SOCK_INST** ppSockInst, IN OUT SOCK_PROC** ppSockProc, SOCKET s, DWORD dwOption )
{
	SOCK_INST*		pSockInst_Find;

	//
	*ppSockInst = NULL;
	//句柄安全检查
	if( !HANDLE_CHECK(g_lpGlobalSock) )
	{
		return FALSE;
	}
	//寻找 SockInst
	pSockInst_Find = (SOCK_INST*)HANDLE_TO_PTR( s );
	if( SKINST_CHECK_FAIL(pSockInst_Find) )
	{
		return FALSE;
	}
	*ppSockInst = pSockInst_Find;
	//寻找 SockProc
	if( !(*ppSockProc) )
	{
		*ppSockProc = (SOCK_PROC*)pSockInst_Find->lpSkProc;
		if( !HANDLE_CHECK((*ppSockProc)) )
		{
			return FALSE;
		}
	}
	/*  //lilin remove , Sock_DestroyInst whill remove it 
	if( dwOption & INSTF_REMOVE )
	{
		EnterCriticalSection( &(*ppSockProc)->csListSkInst );
		List_RemoveUnit( &pSockInst_Find->hListSkInst );
		LeaveCriticalSection( &(*ppSockProc)->csListSkInst );
	}
	*/
	return (*ppSockInst)?TRUE:FALSE;
}

#else

/*
BOOL	Sock_FindInstAndProc( OUT SOCK_INST** ppSockInst, IN OUT SOCK_PROC** ppSockProc, SOCKET s, DWORD dwOption )
{
	SOCK_INST*		pSockInst_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppSockInst = NULL;
	//寻找 SockProc
	if( !(*ppSockProc) )
	{
		if( !Sock_FindProc( ppSockProc, GetCallerProcess(), PROCF_USECS ) )
		{
			return FALSE;
		}
	}
	else
	{
		//句柄安全检查
		if( !HANDLE_CHECK(g_lpGlobalSock) )
		{
			return FALSE;
		}
	}
	//寻找 SockInst
	//pUnitHeader = &(*ppSockProc)->hListSkInst; //lilin remove
	EnterCriticalSection( &(*ppSockProc)->csListSkInst );
	pUnitHeader = &(*ppSockProc)->hListSkInst;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pSockInst_Find = LIST_CONTAINER( pUnit, SOCK_INST, hListSkInst );
		pUnit = pUnit->pNext;    // advance to next 
		
		if( !HANDLE_CHECK(pSockInst_Find) || (pSockInst_Find->wFlag & SKINSTF_EXIT) )
		{
			continue;
		}
		//比较
		if( pSockInst_Find->sock_id==s )
		{
			if( dwOption & INSTF_REMOVE )
			{
				List_RemoveUnit( &pSockInst_Find->hListSkInst );
			}
			*ppSockInst = pSockInst_Find;
			break;
		}
	}
	LeaveCriticalSection( &(*ppSockProc)->csListSkInst );
	//
	if( !HANDLE_CHECK((*ppSockInst)) || ((*ppSockInst)->wFlag & SKINSTF_EXIT) )
	{
		(*ppSockProc)->nErrSock = E_NOTSOCK;
		return FALSE;
	}
	return (*ppSockInst)?TRUE:FALSE;
}
*/

#endif


int		Sock_NewPollTbl( OUT POLLTBL** ppTblPoll, OUT SOCK_INST** ppSockInst, SOCK_PROC** ppSockProc, fd_set* readfds, fd_set* writefds, fd_set* exceptfds )
{
	POLLTBL*		pTblPoll;
	int				nCntRead;
	int				nCntWrite;
	int				nCntExcept;
	int				nCntAll;
	int				i;
	int				nPos;

	//统计个数
	if( readfds )
	{
		nCntRead = readfds->fd_count;
	}
	else
	{
		nCntRead = 0;
	}
	if( writefds )
	{
		nCntWrite = writefds->fd_count;
	}
	else
	{
		nCntWrite = 0;
	}
	if( exceptfds )
	{
		nCntExcept = exceptfds->fd_count;
	}
	else
	{
		nCntExcept = 0;
	}
	nCntAll = nCntRead + nCntWrite + nCntExcept;
	if( nCntAll==0 )
	{
		return 0;
	}
	//生成 POLLTBL
	pTblPoll = (POLLTBL*)malloc( nCntAll*sizeof(POLLTBL) );
	if( !pTblPoll )
	{
		(*ppSockProc)->nErrSock = E_NOBUFS;
		return 0;
	}
	memset( pTblPoll, 0, nCntAll*sizeof(POLLTBL) );
	nPos = 0;
	for( i=0; i<nCntRead; i++ )
	{
		//获取 socket实例
		if( Sock_FindInstAndProc( ppSockInst, ppSockProc, readfds->fd_array[i], 0 ) )
		{
			pTblPoll[nPos].sID = readfds->fd_array[i];
			pTblPoll[nPos].pSockInst = *ppSockInst;
			pTblPoll[nPos].dwFlag |= SOCK_POLL_QREAD;
			nPos ++;
		}
	}
	for( i=0; i<nCntWrite; ++ i )
	{
		//获取 socket实例
		if( Sock_FindInstAndProc( ppSockInst, ppSockProc, writefds->fd_array[i], 0 ) )
		{
			pTblPoll[nPos].sID = readfds->fd_array[i];
			pTblPoll[nPos].pSockInst = *ppSockInst;
			pTblPoll[nPos].dwFlag |= SOCK_POLL_QWRITE;
			nPos ++;
		}
	}
	for( i=0; i<nCntExcept; ++ i )
	{
		//获取 socket实例
		if( Sock_FindInstAndProc( ppSockInst, ppSockProc, exceptfds->fd_array[i], 0 ) )
		{
			pTblPoll[nPos].sID = readfds->fd_array[i];
			pTblPoll[nPos].pSockInst = *ppSockInst;
			pTblPoll[nPos].dwFlag |= SOCK_POLL_QEXCEPT;
			nPos ++;
		}
	}
	//
	if( nPos )
	{
		*ppTblPoll = pTblPoll;
	}
	else
	{
		free( pTblPoll );
	}
	return nPos;
}


void	Sock_LookPollTbl( POLLTBL* pTblPoll, int nCntAll, OUT fd_set* readfds, OUT fd_set* writefds, OUT fd_set* exceptfds )
{
	int			i;

	if( readfds && readfds->fd_count )
	{
		FD_ZERO( readfds );
		for( i=0; i<nCntAll; i++ )
		{
			if( pTblPoll[i].dwFlag & SOCK_POLL_READ )
			{
				FD_SET( pTblPoll[i].sID, readfds );
			}
		}
	}
	if( writefds && writefds->fd_count )
	{
		FD_ZERO( writefds );
		for( i=0; i<nCntAll; i++ )
		{
			if( pTblPoll[i].dwFlag & SOCK_POLL_WRITE )
			{
				FD_SET( pTblPoll[i].sID, writefds );
			}
		}
	}
	if( exceptfds && exceptfds->fd_count )
	{
		FD_ZERO( exceptfds );
		for( i=0; i<nCntAll; i++ )
		{
			if( pTblPoll[i].dwFlag & SOCK_POLL_WRITE )
			{
				FD_SET( pTblPoll[i].sID, exceptfds );
			}
		}
	}
}



BOOL	Sock_IsLocalHost( const char* pName )
{
	DWORD	dwSize = MAX_COMPUTERNAME_LENGTH ;
	char	pLocal[MAX_COMPUTERNAME_LENGTH+4];

	if( GetComputerNameEx( ComputerNameNetBIOS, pLocal, &dwSize ) )
	{
		if( stricmp( pLocal, pName )==0 )
		{
			return TRUE;
		}
	}
	return FALSE;
}


// ********************************************************************
// 声明：
// 参数：
//	OUT pHostInfo-保存获取的主机信息
//	IN dwLenBuf-指定pHostInfo长度
//	IN pAddrList-指定地址列表的BUFFER
//	IN wAddrLen-指定每个地址的长度
//	IN/OUT pwCntAddr-指定地址的个数，和保存获取到的个数
// 返回值：
//	成功，返回0;失败，返回ERROR
// 功能描述：保存地址列表到主机信息BUFFER
// 引用: 
// 注释：
//  |      HOSTENT                 |         pBufAddrs            |      pAddrValue       |
//  |------------------------------|------------------------------|-----------------------|
//  |                | h_addr_list |  A1 |  A2 | ... |  An | NULL |  V1 |  V2 | ... |  Vn |
//  |-----------------------|------|------------------------------|-----------------------|
//                          |         ^
//                          |         |
//                          -----------
// ********************************************************************
BOOL	Skt_SavHostentAddr( OUT HOSTENT* pHostInfo, DWORD dwLenBuf, LPBYTE pAddrList, WORD wAddrLen, IN OUT WORD* pwCntAddr )
{
	LPBYTE		pBufAddrs;
	LPBYTE		pAddrValue;
	WORD		wCntAddr;
	WORD		i;

	//
	ASSERT( wAddrLen%4==0 );
	//检查这个最起码的长度---可以保存1个地址
	if( dwLenBuf < (sizeof(HOSTENT) +sizeof(LPVOID)+wAddrLen +sizeof(LPVOID)) )
	{
		//说明长度不够
		return FALSE;//E_NOBUFS
	}
	//计算能够实际保存的地址的个数
	wCntAddr = (WORD)((dwLenBuf -sizeof(HOSTENT) -sizeof(LPVOID)) / (sizeof(LPVOID)+wAddrLen));
	//首先计算长度
	if( wCntAddr > *pwCntAddr )
	{
		wCntAddr = *pwCntAddr;
	}
	//
	pBufAddrs = (LPBYTE)pHostInfo + sizeof(HOSTENT);
	pHostInfo->h_length    = wAddrLen;
	pHostInfo->h_addr_list = (char**)(pBufAddrs);
	//填充 h_addr_list信息
	pAddrValue = pBufAddrs + sizeof(LPVOID)*wCntAddr +sizeof(LPVOID);
	for( i=0; i<wCntAddr; i++ )
	{
		//填充 pAddrValue地址--->*pBufAddrs
		pHostInfo->h_addr_list[i] = pAddrValue;
		//填充 *pAddrList--->*pAddrValue
		memcpy( pAddrValue, pAddrList, wAddrLen );

		//
		pAddrValue += wAddrLen;
		pAddrList += wAddrLen;
	}
	pHostInfo->h_addr_list[i] = NULL;

	//保存填充地址的个数
	*pwCntAddr = wCntAddr;

	return TRUE;//E_SUCCESS
}


