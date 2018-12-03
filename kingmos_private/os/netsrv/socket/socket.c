/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����Socket
�汾�ţ�  2.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��
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

/***************  ȫ���� ���壬 ���� *****************/
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
// ������
// ������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ������������ʼ��
// ����: 
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
// ������
// ������
//	IN pTlLocal-�������Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ����������ע��1�������
// ����: 
// ********************************************************************
BOOL	Sock_Register( TL_LOCAL* pTlLocal )
{
	TL_LOCAL*		pTlLocal_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	EnterCriticalSection( &g_lpGlobalSock->csListTl );
	//�����ж��Ƿ��Ѿ�������
	pUnitHeader = &g_lpGlobalSock->hListTl;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pTlLocal_Find = LIST_CONTAINER( pUnit, TL_LOCAL, hListTl );
		pUnit = pUnit->pNext;    // advance to next 
		
		//�Ƚ�
		if( (pTlLocal_Find->nFamily==pTlLocal->nFamily) && (pTlLocal_Find->nType==pTlLocal->nType) )
		{
			LeaveCriticalSection( &g_lpGlobalSock->csListTl );
			//�ظ�ע��ʧ��
			return FALSE;
		}
	}
	//ע��1�������
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
			//��Ҫ�ͷ�
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
// ������
// ������
//	IN wVersionRequired-ָ��socket�汾
//	OUT lpWSAData-�����ȡ��socket�汾��������Ϣ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ش�����ֵ
// �����������������繦�ܣ���ȡsocket�汾��������Ϣ
// ����: 
// ********************************************************************
SKERR	WINAPI	SKAPI_WSAStartup( WORD wVersionRequired, OUT LPWSADATA lpWSAData )
{
	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalSock) )
	{
		return E_VERNOTSUPPORTED;
	}
	if( wVersionRequired==0x0101 )
	{
		SOCK_PROC*	pSockProc;

		//�п����������
		if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
		{
			//����ý��̵�����SOCKET��Ϣ
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
// ������
// ������
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// �����������ͷ����繦��
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_WSACleanup(void)
{
	SOCK_PROC*		pSockProc;

	//
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS |PROCF_REMOVE ) )
	{
		return 0;
	}
	//��Ҫ�ͷ�
	Sock_DestroyProc( pSockProc );
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN wVersionRequired-ָ��socket�汾
// ����ֵ��
//	��
// ��������������socket���������
// ����: 
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
// ������
// ������
//	IN wVersionRequired-ָ��socket�汾
// ����ֵ��
//	����socket���������
// ������������ȡsocket���������
// ����: 
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
// ������
// ������
//	IN af-ָ���ĵ�ַЭ����
//	IN type-ָ����Socket����
//	IN protocol-ָ����Э������
// ����ֵ��
//	�ɹ�������SOCKET;ʧ�ܣ�����INVALID_SOCKET
// ��������������SOCKET
// ����: 
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
	//Ѱ�� ��Ӧ�Ĵ����TL
	if( !Sock_FindTl( &pTlLocal, af, type, protocol ) )
	{
		pSockProc->nErrSock = E_AFNOSUPPORT;
		return INVALID_SOCKET;
	}
	//�½�1��SOCK_INST������ʼ��
	if( Sock_CreateInst(&pSockInst, pSockProc, pTlLocal)==INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}
	//���� �����ִ��
	if( nErr=pTlLocal->pFnTlOpen(pSockInst) )
	{
		//��ɾ��
		Sock_DestroyInst( pSockInst, pSockProc );
		pSockProc->nErrSock = nErr;
		return INVALID_SOCKET;
	}
	
	return pSockInst->sock_id;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN myaddr-ָ�����ص�ַ��Ϣ
//	IN addr_len-ָ��myaddr�ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ������������Դ��ַ��
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Bind( SOCKET s, const SOCKADDR* myaddr, int addr_len )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr = pSockInst->pTlLocal->pFnTlBind(pSockInst, myaddr, addr_len) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN addr-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN addr_len-ָ��addr�ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// �������������ӵ�Ŀ�ĵ�ַ��
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Connect( SOCKET s, const SOCKADDR* addr, int addr_len )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		RETAILMSG(1,(TEXT("\r\nSKAPI_Connect: connect s=[%x], Failed!--1\r\n"), s));
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr = pSockInst->pTlLocal->pFnTlConnect(pSockInst, addr, addr_len, 0) )
	{
		RETAILMSG(1,(TEXT("\r\nSKAPI_Connect: connect s=[%x], Failed!--2, nErr=%d\r\n"), s, nErr));
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN queue_len-ָ���������Ӹ����ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ������������socket�������״̬�������ý������Ӹ���
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Listen( SOCKET s, int queue_len )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr = pSockInst->pTlLocal->pFnTlListen(pSockInst, queue_len) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN addr-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN paddr_len-����ָ��addr�ĳ���
// ����ֵ��
//	�ɹ�������SOCKET;ʧ�ܣ�����INVALID_SOCKET
// ��������������Ŀ�ĵ����ӣ����������ĵ�ַ��
// ����: 
// ********************************************************************
SOCKET	WINAPI	SKAPI_Accept( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len )
{
	SOCK_PROC*		pSockProc;
	int				flags=0;
	SOCK_INST*		pSockInst_New;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return INVALID_SOCKET;
	}
	//�½�1��SOCK_INST������ʼ��
	if( Sock_CreateInst(&pSockInst_New, pSockProc, pSockInst->pTlLocal)==INVALID_SOCKET )
	{
		return INVALID_SOCKET;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlAccept(pSockInst, pSockInst_New, (SOCKADDR*)addr, paddr_len, flags) )
	{
		//��ɾ��
		Sock_DestroyInst( pSockInst_New, pSockProc );
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return pSockInst_New->sock_id;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN cmd-ָ������
//	IN argp-ָ��������Ϣ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ����������IO����
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_IoctlSocket( SOCKET s, long cmd, IN OUT DWORD* argp )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlIoctl(pSockInst, cmd, argp) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN buf-ָ���������ݵ�Buffer
//	IN len-ָ���������ݵĳ���
//	IN flags-ָ���������ݵı�־
// ����ֵ��
//	�ɹ������ط������ݵĳ���;ʧ�ܣ�����SOCKET_ERROR
// ������������������
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Send( SOCKET s, const char* buf, int len, int flags )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
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
// ������
// ������
//	IN s-ָ����SOCKET
//	IN buf-ָ���������ݵ�Buffer
//	IN len-ָ���������ݵĳ���
//	IN flags-ָ���������ݵı�־
//	IN to-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN tolen-ָ��to�ĳ���
// ����ֵ��
//	�ɹ������ط������ݵĳ���;ʧ�ܣ�����SOCKET_ERROR
// ������������������
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_SendTo( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
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
// ������
// ������
//	IN s-ָ����SOCKET
//	OUT buf-ָ���������ݵ�Buffer
//	IN len-ָ���������ݵĳ���
//	IN flags-ָ���������ݵı�־
// ����ֵ��
//	�ɹ������ؽ������ݵĳ���;ʧ�ܣ�����SOCKET_ERROR
// ������������������
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_Recv( SOCKET s, OUT char* buf, int len, int flags )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
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
// ������
// ������
//	IN s-ָ����SOCKET
//	OUT buf-ָ���������ݵ�Buffer
//	IN len-ָ���������ݵĳ���
//	IN flags-ָ���������ݵı�־
//	OUT from-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN fromlen-ָ��from�ĳ���
// ����ֵ��
//	�ɹ������ؽ������ݵĳ���;ʧ�ܣ�����SOCKET_ERROR
// ������������������
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_RecvFrom( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, OUT int* fromlen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	RXTXMSG			MsgRxTx;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
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
// ������
// ������
//	IN nfds-����
//	IN readfds-�ɶ��Ե�SOCKET��
//	IN writefds-��д�Ե�SOCKET��
//	IN exceptfds-�����Ե�SOCKET��
//	IN timeout-�ȴ���ʱ��ʱ��
// ����ֵ��
//	�ɹ������صȵ���Ч�¼��ĸ���;ʧ�ܣ�����SOCKET_ERROR
// ����������Select����
// ����: 
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
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlPoll(pTblPoll, &nPos, dwTmout) )
	{
		pSockProc->nErrSock = nErr;
		free( pTblPoll );
		return SOCKET_ERROR;
	}

	//����pFnTlPoll��õ�pTblPoll���������� readfds, writefds, exceptfds����
	Sock_LookPollTbl( pTblPoll, nCntAll, readfds, writefds, exceptfds );

	free( pTblPoll );
	return nPos;
}


// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN level-ѡ��ļ���
//	IN optname-ѡ�������
//	OUT optval-ѡ�������
//	IN optlen-ѡ��ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ������������ȡѡ��
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_GetSockOpt( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;

	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlGetSockOpt(pSockInst, level, optname, optval, *optlen) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN level-ѡ��ļ���
//	IN optname-ѡ�������
//	IN optval-ѡ�������
//	IN optlen-ѡ��ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ��������������ѡ��
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_SetSockOpt( SOCKET s, int level, int optname, const char* optval, int optlen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlSetSockOpt(pSockInst, level, optname, optval, optlen) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	OUT addr-�����ַ��Ϣ
//	OUT paddr_len-�����ַ�ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ������������ȡ�Է��ĵ�ַ��Ϣ
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_GetPeerName( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlGetName(pSockInst, name, namelen, 1) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	OUT addr-�����ַ��Ϣ
//	OUT paddr_len-�����ַ�ĳ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ������������ȡ���صĵ�ַ��Ϣ
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_GetSockName( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlGetName(pSockInst, name, namelen, 0) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
//	IN how-�رյķ�ʽ
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// ���������������رձ��ض�
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_ShutDown( SOCKET s, int how )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	SKERR			nErr;
	
	//��ȡ socketʵ��
	pSockProc = NULL;
	if( !Sock_FindInstAndProc( &pSockInst, &pSockProc, s, 0 ) )
	{
		return SOCKET_ERROR;
	}
	//���� �����ִ��
	if( nErr=pSockInst->pTlLocal->pFnTlShutDown(pSockInst, how) )
	{
		pSockProc->nErrSock = nErr;
		return SOCKET_ERROR;
	}
	return 0;
}

// ********************************************************************
// ������
// ������
//	IN s-ָ����SOCKET
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����SOCKET_ERROR
// �����������رձ��ض�
// ����: 
// ********************************************************************
SKRET	WINAPI	SKAPI_CloseSocket( SOCKET s )
{
	SOCK_PROC*		pSockProc;
	SOCK_INST*		pSockInst;
	
	//��ȡ socketʵ��
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

	//���
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
	
	//��ȡ socketʵ��
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
	
	//��ȡ socketʵ��
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return NULL;
	}
	pSockProc->nErrSock = E_SOCKTNOSUPPORT;
	return NULL;
}

//�ɹ������ػ�ȡ��ַ��Ϣ�ĸ�����ʧ�ܣ�����SOCKET_ERROR������GetLastError()
HOSTENT*	WINAPI	SKAPI_GetHostByNameEx( const char* name, OUT LPBYTE pBufInfo, int len )
{
	HOSTENT*		pHostInfo = (HOSTENT*)pBufInfo;
	SOCK_PROC*		pSockProc;
	SKERR			nErr;
	DWORD			dwIP;
	WORD			wCntAddr;

	//��ȡ socketʵ��
	if( !Sock_FindProc( &pSockProc, GetCallerProcess(), PROCF_USECS ) )
	{
		return NULL;
	}
	//������������ĳ���---���Ա���1����ַ
	if( len < (sizeof(HOSTENT) +(sizeof(LPVOID)+ADDRLEN_IPV4)*1 +sizeof(LPVOID)) )
	{
		pSockProc->nErrSock = E_FAULT;//E_NOBUFS
		return NULL;
	}
	pHostInfo->h_name      = (char*)name;
	pHostInfo->h_addrtype  = AF_UNKNOWN;//htons_m(DNS_RRTYPE_A);

	//���ȼ���ַ��Ƿ�ϸ�

	//�ټ���Ƿ����IP��ַ---ֱ������ַ��Ϣ����
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

	//�Ƚ��Ƿ�Ϊ������������---ֱ��ö��
	if( Sock_IsLocalHost(name) )
	{
		DWORD				dwCntAddr;
		BYTE				pBufAddrs[10*ADDRLEN_IPV4];

		//��ö�ٱ��ص� ����IP�ӿڵ���Ϣ
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
	
	//�Ƚ��Ƿ�Ϊ��������---����NBTЭ���ѯ---Ŀǰ��֧��

	//���������---����DNSЭ���ѯ
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
//�ڲ���������
//
/////////////////////////////////////////////////////////////////////


// ********************************************************************
// ������
// ������
//	OUT ppTlLocal-����Ѱ�ҵĴ����TL
//	IN af-ָ���ĵ�ַЭ����
//	IN type-ָ����Socket����
//	IN protocol-ָ����Э������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������Ѱ�� ��Ӧ�Ĵ����TL
// ����: 
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
		
		//�Ƚ�
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
// ������
// ������
//	OUT ppSockProc-����Ѱ�ҵ�SOCK_PROC
//	IN hProc-ָ���Ľ���
//	IN fUse-ָ���Ƿ񻥳�
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������Ѱ�� ָ���Ľ��̵�SOCK_PROC
// ����: 
// ********************************************************************
BOOL	Sock_FindProc( OUT SOCK_PROC** ppSockProc, HANDLE hProc, DWORD dwOption )
{
	SOCK_PROC*	pSockProc_Find;
	PLIST_UNIT	pUnitHeader;
	PLIST_UNIT	pUnit;

	//
	*ppSockProc = NULL;
	//�����ȫ���
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
		//�Ƚ�
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
// ������
// ������
//	OUT ppSockProc-���洴����SOCK_PROCʵ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ��������������SOCK_PROC
// ����: 
// ********************************************************************
BOOL	Sock_CreateProc( OUT SOCK_PROC** ppSockProc )
{
	SOCK_PROC*		pSockProc;
	SOCK_PROC*		pSockProc_Find;

	//����
	pSockProc = (SOCK_PROC*)HANDLE_ALLOC( sizeof(SOCK_PROC) );
	if( !pSockProc )
	{
		return FALSE;
	}
	HANDLE_INIT( pSockProc, sizeof(SOCK_PROC) );
	List_InitHead( &pSockProc->hListSkProc );
	List_InitHead( &pSockProc->hListSkInst );
	InitializeCriticalSection( &pSockProc->csListSkInst );
	//��ʼ��
	pSockProc->hProc = GetCallerProcess( );

	//���
	EnterCriticalSection( &g_lpGlobalSock->csListSkProc );
	if( Sock_FindProc( &pSockProc_Find, pSockProc->hProc, 0 ) )
	{
		//�ͷ���
		Sock_DeleteProc( pSockProc );
		//��ȡ�ҵ���
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
// ������
// ������
//	IN ppSockProc-SOCK_PROCʵ����Ϣ
// ����ֵ��
//	��
// ����������ɾ��SOCK_PROC
// ����: 
// ********************************************************************
void	Sock_DeleteProc( SOCK_PROC* pSockProc )
{
	//�����ȫ���
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
// ������
// ������
//	IN ppSockProc-SOCK_PROCʵ����Ϣ
// ����ֵ��
//	��
// ��������������SOCK_PROC
// ����: 
// ********************************************************************
void	Sock_DestroyProc( SOCK_PROC* pSockProc )
{
	SOCK_INST*		pSockInst_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	//�Ͽ�
	EnterCriticalSection( &g_lpGlobalSock->csListSkProc);
	List_RemoveUnit( &pSockProc->hListSkProc );
	LeaveCriticalSection( &g_lpGlobalSock->csListSkProc);

	//Ѱ�� SockInst��������
	EnterCriticalSection( &pSockProc->csListSkInst );
	pUnitHeader = &pSockProc->hListSkInst;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pSockInst_Find = LIST_CONTAINER( pUnit, SOCK_INST, hListSkInst );
		//pUnit = pUnit->pNext;    // advance to next  //lilin remove

		//�����ȫ���
		if( !HANDLE_CHECK(pSockInst_Find) )
		{
			continue;
		}
		//����
		Sock_DestroyInst( pSockInst_Find, pSockProc );
		pUnit = pUnitHeader->pNext;    // advance to next  //lilin add
	}
	LeaveCriticalSection( &pSockProc->csListSkInst );

	//
	Sock_DeleteProc( pSockProc );
}



// ********************************************************************
// ������
// ������
//	OUT pSockInst-ָ����SockInst��Ϣ
//	IN pSockProc-ָ����SockProc��Ϣ
// ����ֵ��
//	�ɹ�������SOCKET;ʧ�ܣ�����INVALID_SOCKET
// ����������������SOCKET
// ����: 
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
		//��Ҫ�ж�---��ʱ����
	}
	else if( sock_id>=SKTID_MAX )
	{
		pSockProc->fToEnd = TRUE;
		
		pSockProc->nNewID = SKTID_MIN;	//attention here
	}
#endif
	//����
	if( sock_id!=INVALID_SOCKET )
	{
		pSockInst->sock_id = sock_id;
	}
	LeaveCriticalSection( &pSockProc->csListSkInst );

	return sock_id;
}

// ********************************************************************
// ������
// ������
//	OUT ppSockInst-���洴����SOCKETʵ����Ϣ
//	IN pSockProc-ָ����SockProc��Ϣ
//	IN pTlLocal-ָ���Ĵ����TL��Ϣ
// ����ֵ��
//	�ɹ�������SOCKET;ʧ�ܣ�����INVALID_SOCKET
// ��������������SOCK_INST
// ����: 
// ********************************************************************
SOCKET	Sock_CreateInst( OUT SOCK_INST** ppSockInst, SOCK_PROC* pSockProc, TL_LOCAL* pTlLocal )
{
	SOCK_INST*		pSockInst;

	//����
	pSockInst = (SOCK_INST*)HANDLE_ALLOC( sizeof(SOCK_INST) );
	if( !pSockInst )
	{
		pSockProc->nErrSock = E_NOBUFS;
		return INVALID_SOCKET;
	}
	HANDLE_INIT( pSockInst, sizeof(SOCK_INST) );
	List_InitHead( &pSockInst->hListSkInst );
	//��ʼ��
	pSockInst->pTlLocal = pTlLocal;
	pSockInst->lpSkProc = (LPVOID)pSockProc;
	//���
	EnterCriticalSection( &pSockProc->csListSkInst );
	List_InsertTail( &pSockProc->hListSkInst, &pSockInst->hListSkInst );
	LeaveCriticalSection( &pSockProc->csListSkInst);

	//���� socket ID
	if( Sock_NewID( pSockInst, pSockProc )==INVALID_SOCKET )
	{
		//��ɾ��
		Sock_DestroyInst( pSockInst, pSockProc );
		//
		pSockProc->nErrSock = E_MFILE;
		return INVALID_SOCKET;
	}

	*ppSockInst = pSockInst;
	return pSockInst->sock_id;
}


// ********************************************************************
// ������
// ������
//	IN pSockInst-SOCK_INSTʵ����Ϣ
//	IN pSockProc-ָ����SockProc��Ϣ
// ����ֵ��
//	��
// ��������������SOCK_INST
// ����: 
// ********************************************************************
void	Sock_DestroyInst( SOCK_INST* pSockInst, SOCK_PROC* pSockProc )
{
	BOOL bFree = FALSE;
	//�Ͽ�
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
		//���� �����ִ��
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
// ������
// ������
//	OUT ppSockInst-����Ѱ�ҵ�SOCK_INST
//	IN/OUT ppSockProc-����Ѱ�ҵ�SOCK_PROC
//	IN s-ָ����SOCKET
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������Ѱ�� ָ���Ľ��̵�SOCK_PROC��SOCK_INST
// ����: 
// ********************************************************************

#ifdef SKT_ID_USE_PTR
BOOL	Sock_FindInstAndProc( OUT SOCK_INST** ppSockInst, IN OUT SOCK_PROC** ppSockProc, SOCKET s, DWORD dwOption )
{
	SOCK_INST*		pSockInst_Find;

	//
	*ppSockInst = NULL;
	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalSock) )
	{
		return FALSE;
	}
	//Ѱ�� SockInst
	pSockInst_Find = (SOCK_INST*)HANDLE_TO_PTR( s );
	if( SKINST_CHECK_FAIL(pSockInst_Find) )
	{
		return FALSE;
	}
	*ppSockInst = pSockInst_Find;
	//Ѱ�� SockProc
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
	//Ѱ�� SockProc
	if( !(*ppSockProc) )
	{
		if( !Sock_FindProc( ppSockProc, GetCallerProcess(), PROCF_USECS ) )
		{
			return FALSE;
		}
	}
	else
	{
		//�����ȫ���
		if( !HANDLE_CHECK(g_lpGlobalSock) )
		{
			return FALSE;
		}
	}
	//Ѱ�� SockInst
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
		//�Ƚ�
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

	//ͳ�Ƹ���
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
	//���� POLLTBL
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
		//��ȡ socketʵ��
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
		//��ȡ socketʵ��
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
		//��ȡ socketʵ��
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
// ������
// ������
//	OUT pHostInfo-�����ȡ��������Ϣ
//	IN dwLenBuf-ָ��pHostInfo����
//	IN pAddrList-ָ����ַ�б��BUFFER
//	IN wAddrLen-ָ��ÿ����ַ�ĳ���
//	IN/OUT pwCntAddr-ָ����ַ�ĸ������ͱ����ȡ���ĸ���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ�����ERROR
// ���������������ַ�б�������ϢBUFFER
// ����: 
// ע�ͣ�
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
	//������������ĳ���---���Ա���1����ַ
	if( dwLenBuf < (sizeof(HOSTENT) +sizeof(LPVOID)+wAddrLen +sizeof(LPVOID)) )
	{
		//˵�����Ȳ���
		return FALSE;//E_NOBUFS
	}
	//�����ܹ�ʵ�ʱ���ĵ�ַ�ĸ���
	wCntAddr = (WORD)((dwLenBuf -sizeof(HOSTENT) -sizeof(LPVOID)) / (sizeof(LPVOID)+wAddrLen));
	//���ȼ��㳤��
	if( wCntAddr > *pwCntAddr )
	{
		wCntAddr = *pwCntAddr;
	}
	//
	pBufAddrs = (LPBYTE)pHostInfo + sizeof(HOSTENT);
	pHostInfo->h_length    = wAddrLen;
	pHostInfo->h_addr_list = (char**)(pBufAddrs);
	//��� h_addr_list��Ϣ
	pAddrValue = pBufAddrs + sizeof(LPVOID)*wCntAddr +sizeof(LPVOID);
	for( i=0; i<wCntAddr; i++ )
	{
		//��� pAddrValue��ַ--->*pBufAddrs
		pHostInfo->h_addr_list[i] = pAddrValue;
		//��� *pAddrList--->*pAddrValue
		memcpy( pAddrValue, pAddrList, wAddrLen );

		//
		pAddrValue += wAddrLen;
		pAddrList += wAddrLen;
	}
	pHostInfo->h_addr_list[i] = NULL;

	//��������ַ�ĸ���
	*pwCntAddr = wCntAddr;

	return TRUE;//E_SUCCESS
}


