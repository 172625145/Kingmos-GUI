/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TCP Socket������ ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��
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


/***************  ȫ���� ���壬 ���� *****************/

//TCP�ṩ�ĺ�����
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

//�ڲ���������
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

	//��ʼ�� �������Ϣ
	pTlLocal = &lpGlobalTcp->LocalTl;
	HANDLE_SET( pTlLocal );
	List_InitHead( &pTlLocal->hListTl );
	pTlLocal->hTlGlobal = (HANDLE)lpGlobalTcp;

	pTlLocal->nFamily = AF_INET;
	pTlLocal->nType = SOCK_STREAM;
	pTlLocal->nProtocol = IPPROTO_TCP;

	//��� ����㺯����
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
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
// ����ֵ��
//	
// ��������������TCB
// ����: 
// ********************************************************************
SKERR	TcpSk_Create( SOCK_INST* pSockInst )
{
	TCP_GLOBAL*	lpGlobalTcp;
	TCP_TCB*	pTcbTcp;

	//�����ȫ���
	lpGlobalTcp = (TCP_GLOBAL*)pSockInst->pTlLocal->hTlGlobal;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//����TCB
	if( !Tcp_CreateTcb( &pTcbTcp, lpGlobalTcp ) )
	{
		//�ռ䲻���� ʧ��
		return E_NOBUFS;
	}
	//��ʼ��
	pTcbTcp->dwSrcIP = INADDR_ANY;
	pTcbTcp->dwState = TCPS_CLOSED;
	//������
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	List_InsertTail( &lpGlobalTcp->hListTcb, &pTcbTcp->hListTcb );
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	//������
	pSockInst->hIfTl = (HANDLE)pTcbTcp;

	return E_SUCCESS;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN myaddr-ָ�����ص�ַ��Ϣ
//	IN addr_len-ָ��myaddr�ĳ���
// ����ֵ��
//	
// ������������TCB��Դ��ַ��
// ����: 
// ********************************************************************
SKERR	TcpSk_Bind( SOCK_INST* pSockInst, const SOCKADDR* myaddr, int addr_len )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)myaddr;
	DWORD			dwSrcIP;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pAddrIn || addr_len<sizeof(SOCKADDR) )
	{
		//��������
		return E_FAULT;
	}
	if( pAddrIn->sin_family!=AF_INET )
	{
		//��֧�ֵ�Э����
		return E_AFNOSUPPORT;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���״̬��
	if( pTcbTcp->wSrcPort )
	{
		//��socket�Ѿ�����
		return E_INVAL;
	}

	//�󶨱���IP��ַ
	dwSrcIP = pAddrIn->sin_addr.S_un.S_addr;
	if( dwSrcIP!=INADDR_ANY )
	{
		if( INADDR_TEST_BC(dwSrcIP) )
		{
			//��IP��ַ��Ч
			return E_ADDRNOTAVAIL;
		}
		if( !IP_IsValidAddr( dwSrcIP ) )
		{
			//��IP��ַ��Ч
			return E_ADDRNOTAVAIL;
		}
	}
	pTcbTcp->dwSrcIP = dwSrcIP;
	//�󶨱��ض˿�
	if( pAddrIn->sin_port==0 )
	{
		//�����¶˿ڣ����Ұ󶨶˿�
		if( !Tcp_NewPort( pTcbTcp, lpGlobalTcp ) )
		{
			pTcbTcp->dwSrcIP = INADDR_ANY;
			//�����¶˿�ʧ��
			return E_NOMOREPORT;
		}
	}
	else
	{
		//���ö˿��Ƿ�ռ�ã����Ұ󶨶˿�
		if( !Tcp_UsePort(pTcbTcp, pAddrIn->sin_port, lpGlobalTcp) )
		{
			pTcbTcp->dwSrcIP = INADDR_ANY;
			//�����ö˿ڱ�ռ��
			return E_ADDRINUSE;
		}
	}

	return E_SUCCESS;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN addr-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN addr_len-ָ��addr�ĳ���
//	IN flags-����
// ����ֵ��
//	
// �������������ӵ�Ŀ�ĵ�ַ��
// ����: 
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
	//�����ȫ���
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
	//��������
	if( !pAddrIn || (addr_len<sizeof(SOCKADDR_IN)) )
	{
		//��������
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--3\r\n")));
		return E_FAULT;
	}
	if( pAddrIn->sin_family!=AF_INET )
	{
		//��֧�ֵ�Э����
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--4\r\n")));
		return E_AFNOSUPPORT;
	}
	dwDesIP = pAddrIn->sin_addr.S_un.S_addr;
	if( dwDesIP==INADDR_ANY || INADDR_TEST_BC(dwDesIP) || pAddrIn->sin_port==0 )
	{
		//Զ�̵�ַ��Ч---�ͻ��������ӵ� ���κε�ַ�� �� ���㲥��ַ��
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--5\r\n")));
		return E_ADDRNOTAVAIL;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--6\r\n")));
		return E_NETDOWN;
	}
	//���״̬��
	if( pTcbTcp->dwState==TCPS_SYN_SENT )
	{
		//���ӵĲ������ڽ���
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--7\r\n")));
		return E_ALREADY;
	}
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		//�����Ѿ�����
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--8\r\n")));
		return E_ISCONN;
	}
	if( (pTcbTcp->dwState!=TCPS_CLOSED) || (pTcbTcp->wAttr & TCPA_LISTEN) )
	{
		//��socket���������ڼ���(listening)��
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--9\r\n")));
		return E_INVAL;
	}

	//ȷ�� dwSrcIP and wSrcPort---������Ҫ�󶨻��Զ��󶨹̶�IP����PORT
	if( (pTcbTcp->dwSrcIP==INADDR_ANY) || (pTcbTcp->wSrcPort==0) )
	{
		//�Զ���
		//HeapValidate( GetProcessHeap(), 0, 0 );
		if( nErr=Tcp_AutoBind( pTcbTcp, dwDesIP, lpGlobalTcp ) )
		{
			RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--10\r\n")));
			return nErr;
		}
	}
	//�������ݷ����Լ������IP�����PORT��
	if( (pTcbTcp->wSrcPort==pAddrIn->sin_port) && (pTcbTcp->dwSrcIP==dwDesIP) )
	{
		//Զ�̵�ַ��Ч
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--11\r\n")));
		return E_ADDRNOTAVAIL;
	}

	//����Զ�̵�ַ
	pTcbTcp->dwDesIP = dwDesIP;
	pTcbTcp->wDesPort = pAddrIn->sin_port;
	//��ѯ IP��������Ϣ �� ����MSS
	if( !Tcp_QueryMSS(pTcbTcp->dwSrcIP, pTcbTcp->dwDesIP, &wMSS_me) )
	{
		//�����ʧ��
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--12\r\n")));
		return E_NETDOWN;
	}
	pTcbTcp->wSnd_MSS = wMSS_me;
	pTcbTcp->wRcv_MSS = wMSS_me;

	//���빤��״̬
	pTcbTcp->dwState = TCPS_SYN_SENT;
	pTcbTcp->wAttr |= TCPA_CONNECT;

	//׼�� ���Ͳ���
	if( !TcpOut_StartSend(pTcbTcp) )
	{
		//һЩϵͳ����ʧ��
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--13\r\n")));
		pTcbTcp->dwState = TCPS_CLOSED;
		pTcbTcp->wAttr &= ~TCPA_CONNECT;
		return E_SYSFUN;
	}
	//���� ���Ͳ���
	SetEvent( pTcbTcp->hEvtSend );

	//�ж� ����������
	if( pTcbTcp->dwOption & TCPIPO_NONBLOCK )
	{
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave ok--Nonblocking\r\n")));
		return E_WOULDBLOCK;
	}

	//�������
	//RETAILMSG(1,(TEXT("  TcpSk_Connect: then Wait...\r\n")));
	dwWait = WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_CONNECT );
	//�жϽ��---���ӳɹ�
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		//RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave OK\r\n")));
		return E_SUCCESS;
	}
	//�жϽ��---�����˳�
	pTcbTcp->dwState = TCPS_CLOSED;
	pTcbTcp->wFlag = 0;
	if( dwWait==WAIT_TIMEOUT )
	{
		//��ʱ
		RETAILMSG(1,(TEXT("  TcpSk_Connect: Leave Failed--15\r\n")));
		return E_TIMEDOUT;
	}
	else
	{
		//ָ������
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
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN queue_len-ָ���������Ӹ����ĳ���
// ����ֵ��
//	
// ������������socket�������״̬�������ý������Ӹ���
// ����: 
// ********************************************************************
SKERR	TcpSk_Listen( SOCK_INST* pSockInst, int queue_len )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���״̬��
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		//�����Ѿ�����
		return E_ISCONN;
	}
	if( (pTcbTcp->dwState!=TCPS_CLOSED) || (pTcbTcp->wAttr & TCPA_CONNECT) )
	{
		//��socket���������ڼ���(listening)��
		return E_INVAL;
	}
	if( pTcbTcp->wSrcPort==0 )
	{
		//��û�а�bind
		return E_INVAL;
	}

	//�������
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

	//���빤��״̬
	pTcbTcp->dwState = TCPS_LISTEN;
	pTcbTcp->wAttr |= TCPA_LISTEN;

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN pSockInst_New-�������ӵ�socketʵ����Ϣ
//	IN addr-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN paddr_len-����ָ��addr�ĳ���
//	IN flags-����
// ����ֵ��
//	
// ��������������Ŀ�ĵ����ӣ����������ĵ�ַ��
// ����: 
// ********************************************************************
SKERR	TcpSk_Accept( SOCK_INST* pSockInst, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pAddrIn || (*paddr_len<sizeof(SOCKADDR_IN)) )
	{
		//��������
		return E_FAULT;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���״̬��
	if( (pTcbTcp->dwState!=TCPS_LISTEN) || !(pTcbTcp->wAttr & TCPA_LISTEN) )
	{
		//���ȵ���listen
		return E_INVAL;
	}
	//����Ƿ��������
	if( pTcbTcp->wCntConn > pTcbTcp->wMaxConn )
	{
		//�������ӵĸ�������
		return E_MFILE;
	}

	//
	*paddr_len = 0;
	memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
	while( (pTcbTcp->dwState==TCPS_LISTEN) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//�жϱ���IP��ַ�Ƿ���Ч---��ֹ�ο�����
		if( (pTcbTcp->dwSrcIP!=INADDR_ANY) && !IP_IsValidAddr( pTcbTcp->dwSrcIP ) )
		{
			//��IP��ַ��Ч
			return E_ADDRNOTAVAIL;
		}

		//����Ƿ�������
		if( Tcp_LookSynRcvd( pTcbTcp, pSockInst_New, pAddrIn, paddr_len ) )
		{
			//�Ѿ���ȡ����
			return E_SUCCESS;
		}
		//����Ƿ��������
		if( pTcbTcp->wCntConn > pTcbTcp->wMaxConn )
		{
			//�������ӵĸ�������
			return E_MFILE;
		}

		//Nonblocking����
		if( pTcbTcp->dwOption & TCPIPO_NONBLOCK )
		{
			//������ �첽����
			return E_WOULDBLOCK;
		}

		//�˳�
		if( (pTcbTcp->dwState!=TCPS_LISTEN) || (pTcbTcp->wFlag & TCPF_EXIT) )
		{
			break;
		}
		//�ȴ����ӵĽ���
		WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_WAITACCEPT );//INFINITE
	}

	//���ж��˳�
	return E_INTR;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN nCmd-ָ������
//	IN pdwArg-ָ��������Ϣ
// ����ֵ��
//	
// ����������IO����
// ����: 
// ********************************************************************
SKERR	TcpSk_IOCtl( SOCK_INST* pSockInst, long nCmd, IN OUT DWORD* pdwArg )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SKERR			nErr;
	//int				nOptVal;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pdwArg )
	{
		//��������
		return E_FAULT;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���ദ��
	nErr = E_SUCCESS;
	switch( nCmd )
	{
	//���� Nonblockģʽ
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

	//��ȡ���ջ��������ݵĳ���
	case FIONREAD :
		*pdwArg = pTcbTcp->dwUsedRxBufLen;//pTcbTcp->stRxBufRW.dwCntRW;
		break;

	//����Ƿ���OOB����
	//case SIOCATMARK:
	//	*pdwArg = 0;
	//	break;
	
	//
	default :
		//��������
		return E_FAULT;
		break;
	}

	return E_SUCCESS;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN pMsgRxTx-ָ���������ݵ���Ϣ
// ����ֵ��
//	
// ������������������
// ����: 
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

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pMsgRxTx || !pMsgRxTx->pData )
	{
		//��������
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
	//	//���ݰ�̫�� ʧ��
	//	return E_MSGSIZE;
	//}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���״̬��
	if( !(pTcbTcp->wAttr & TCPA_ESTABLISH) )
	{
		//���Ƚ�������
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
		//�ж� ��������
		if( pTcbTcp->wAttr & TCPA_ERR_ALL )
		{
			if( pTcbTcp->wAttr & TCPA_CONNRESET )
			{
				//�������ӱ��Է��Ͽ�
				return E_CONNRESET;
			}
			if( !(pTcbTcp->wAttr & TCPA_SEND) || (pTcbTcp->wFlag & TCPF_LFIN) )
			{
				//�������ӱ�SHUTDOWN
				return E_SHUTDOWN;
			}
			if( pTcbTcp->wAttr & TCPA_NETRESET )
			{
				//keep-alive��⵽��������ʧ��
				return E_NETRESET;
			}
			if( pTcbTcp->wAttr & TCPA_HOST_UNREACH )
			{
				//�������ɵ���
				return E_HOSTUNREACH;
			}
			if( pTcbTcp->wAttr & TCPA_CONN_ABORTED )
			{
				//������ֹ
				return E_CONNABORTED;
			}
		}
		if( !(pTcbTcp->wAttr & TCPA_SEND) || (pTcbTcp->wFlag & TCPF_LFIN) )
		{
			//�������ӱ�SHUTDOWN
			return E_SHUTDOWN;
		}

		//�鿴�Ƿ������ݷ���
		//RETAILMSG(1,(TEXT("  TcpSk_SendMsg: dwLenData=[%d]\r\n"), dwLenData));
		if( dwLenData )
		{
			//�Ƿ����� ���Ͳ���
			//if( pTcbTcp->stTxBufRW.dwCntRW==0 )
			//{
			//	fStartSend = TRUE;
			//}
			//else
			//{
			//	fStartSend = FALSE;
			//}
			//�������ݵ������ͻ������ʣ��ռ䡱
			dwLenTmp = dwLenData;
			if( RWBuf_WriteData( &pTcbTcp->stTxBufRW, pMsgRxTx->pData, &dwLenTmp, 0 ) )
			{
				pTcbTcp->dwSnd_max += dwLenTmp;//��ӷ��͵�����

				pTcbTcp->dwLenLeftTx -= dwLenTmp;
				dwLenData -= dwLenTmp;
			}
			//else
			//{
			//	fStartSend = FALSE;
			//}
			//�Ƿ����� ���Ͳ���
			//if( fStartSend )
			{
				//���� ���Ͳ���
				PulseEvent( pTcbTcp->hEvtSend );
				//SetEvent( pTcbTcp->hEvtSend );
			}
		}

		//�鿴�Ƿ����ݷ������
		if( (pTcbTcp->stTxBufRW.dwCntRW==0) && !dwLenData )
		{
			pMsgRxTx->dwLenData = dwLenAll;
			//�Ѿ���������
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
			//������ �첽����
			return E_WOULDBLOCK;
		}

		//�ȴ����͵����
		WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_WAITSEND );
		//
		//if( (GetTickCount()-dwTickStart) > TCPTM_SEND )
		//{
		//	break;
		//}
	}

	//���ж��˳�
	return E_TIMEDOUT;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN/OUT pMsgRxTx-ָ���������ݵ���Ϣ
// ����ֵ��
//	
// ������������������
// ����: 
// ********************************************************************
BOOL TCP_ReadData( TCP_TCB* pTcbTcp, LPVOID pData, LPDWORD pdwLenData );
SKERR	TcpSk_RecvMsg( SOCK_INST* pSockInst, IN OUT RXTXMSG* pMsgRxTx )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	LPBYTE			pData;
	DWORD			dwLenData;
	SOCKADDR_IN*	pAddrIn;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pMsgRxTx || !(pData=pMsgRxTx->pData) )
	{
		//��������
		return E_FAULT;
	}
	if( !(dwLenData=pMsgRxTx->dwLenData) )
	{
		//�ɹ����أ����û�еĽ�������
		return E_SUCCESS;
	}
	if( pMsgRxTx->lpPeer && (pMsgRxTx->dwLenPeer<sizeof(SOCKADDR_IN)) )
	{
		//��������
		return E_FAULT;
	}
	pMsgRxTx->dwLenData = 0;
	pMsgRxTx->dwLenPeer = 0;
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���״̬��
	if( !(pTcbTcp->wAttr & TCPA_ESTABLISH) )
	{
		//���Ƚ�������
		return E_NOTCONN;
	}
	//RETAILMSG( 1, ( "TcpSk_RecvMsg while.\r\n" ) );
	//ѭ����������
	while( !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		DWORD dwReadLen = dwLenData;
		//�ж� ��������
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
		//�ж� ��������
		if( pTcbTcp->wAttr & TCPA_ERR_ALL )
		{
			if( pTcbTcp->wAttr & TCPA_CONNRESET )
			{
				//�������ӱ��Է��Ͽ�
				return E_CONNRESET;
			}
			if( !(pTcbTcp->wAttr & TCPA_RECV) || (pTcbTcp->wFlag & TCPF_LFIN) )
			{
				//�������ӱ�SHUTDOWN
				return E_SHUTDOWN;
			}
			if( pTcbTcp->wAttr & TCPA_NETRESET )
			{
				//keep-alive��⵽��������ʧ��
				return E_NETRESET;
			}
			if( pTcbTcp->wAttr & TCPA_HOST_UNREACH )
			{
				//�������ɵ���
				return E_HOSTUNREACH;
			}
			if( pTcbTcp->wAttr & TCPA_CONN_ABORTED )
			{
				//������ֹ
				return E_CONNABORTED;
			}
		}
		if( !(pTcbTcp->wAttr & TCPA_RECV) || (pTcbTcp->wFlag & TCPF_LFIN) )
		{
			//�������ӱ�SHUTDOWN
			return E_SHUTDOWN;
		}

		//�ж� ��������
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
		//�ж� ����������
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

		//�ȴ����͵����
		WaitForSingleObject( pTcbTcp->hEvtPoll, TCPTM_WAITRECV );
	}

	//���ж��˳�
	return E_TIMEDOUT;
}

// ********************************************************************
// ������
// ������
//	IN pTblPoll-Ҫ��ѯ��ʵ����Ϣ
//	IN/OUT pnCntPoll-ָ��pTblPoll��Ԫ�صĸ����������ȡ���ĸ���
//	IN dwTmout-ָ����ʱʱ��
// ����ֵ��
//	
// ������������ָ��ʱ���ڲ�ѯ��Ӧ����Ϣ
// ����: 
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
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//��ѯ��Ϣ
	nCntWait = *pnCntPoll;
	*pnCntPoll = Tcp_PollInfo( pTblPoll, nCntWait );
	if( *pnCntPoll )
	{
		//�Ѿ���ȡ��Ϣ
		return E_SUCCESS;
	}
	//ͳ����Ҫ�ȴ��ĸ���
	pArrEvt = (HANDLE*)malloc( nCntWait*sizeof(HANDLE) );
	if( !pArrEvt )
	{
		return E_NOBUFS;
	}
	nPos = 0;
	for( i=0; i<nCntWait; i++ )
	{
		pTcbTcp = (TCP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//�����ȫ���
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
	//��ʼ�ȴ�
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
			//�Ѿ���ȡ��Ϣ
			return E_SUCCESS;
		}
		else if( !fWaittingForYou )
		{
			dwWait = GetTickCount()-dwTickStart;
			if( dwWait>=dwTmout )
			{
				//��ʱ�˳�
				return E_INVAL;
			}
			else
			{
				//���ʣ��ʱ�� �Ƚ���
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
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN level-ѡ��ļ���
//	IN optname-ѡ�������
//	OUT optval-ѡ�������
//	IN optlen-ѡ��ĳ���
// ����ֵ��
//	
// ������������ȡѡ��
// ����: 
// ********************************************************************
SKERR	TcpSk_GetSockOpt( SOCK_INST* pSockInst, int level, int optname, OUT char* optval, int optlen )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SKERR			nErr;
	//int				nOptVal;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( level!=SOL_SOCKET )
	{
		//ѡ��level��Ч
		return E_INVAL;
	}
	if( !SOC_ISIN(optname) )
	{
		return E_FAULT;	//ѡ������ݻ��߳��� ����
	}
	if( optname & SOC_FIXLEN )
	{
		if( SOC_GETFIXLEN(optname)!=optlen )
		{
			return E_FAULT;	//ѡ������ݻ��߳��� ����
		}
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���ദ��
	nErr = E_SUCCESS;
	switch( optname )
	{
	//���������Ϣ
	case SO_DEBUG:
		if( optlen!=sizeof(BOOL) )
		{
			nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
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
	//�رձ������
	case SO_KEEPALIVE:
		if( optlen!=sizeof(BOOL) )
		{
			nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
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
	//����OOB����
	//case SO_OOBINLINE:
	//	if( optlen!=sizeof(BOOL) )
	//	{
	//		nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
	//	}
	//	else
	//	{
	//	}
	//	break;
	//���ý��ջ���ĳ���
	case SO_RCVBUF:
		if( optlen!=sizeof(int) )
		{
			nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
		}
		else
		{
			*(int*)optval = pTcbTcp->dwUsedRxBufLen;//(int)pTcbTcp->stRxBufRW.dwLenRW;
		}
		break;
	//���÷��ͻ���ĳ���
	case SO_SNDBUF:
		if( optlen!=sizeof(int) )
		{
			nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
		}
		else
		{
			*(int*)optval = (int)pTcbTcp->stTxBufRW.dwLenRW;
		}
		break;

	//���ñ��ص�ַ
	//case SO_REUSEADDR:
	//	if( optlen!=sizeof(BOOL) )
	//	{
	//		nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
	//	}
	//	else
	//	{
	//	}
	//	break;

	//Unknown�����߲�֧��
	default :
		nErr = E_NOPROTOOPT;	//ѡ������� Unknown�����߲�֧��
		break;
	}

	return nErr;
}


// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN level-ѡ��ļ���
//	IN optname-ѡ�������
//	IN optval-ѡ�������
//	IN optlen-ѡ��ĳ���
// ����ֵ��
//	
// ��������������ѡ��
// ����: 
// ********************************************************************
SKERR	TcpSk_SetSockOpt( SOCK_INST* pSockInst, int level, int optname, const char* optval, int optlen )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SKERR			nErr;
	//int				nOptVal;
	//DWORD			dwOptVal;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( level!=SOL_SOCKET )
	{
		//ѡ��level��Ч
		return E_INVAL;
	}
	if( !SOC_ISOUT(optname) )
	{
		return E_FAULT;	//ѡ������ݻ��߳��� ����
	}
	if( optname & SOC_FIXLEN )
	{
		if( SOC_GETFIXLEN(optname)!=optlen )
		{
			return E_FAULT;	//ѡ������ݻ��߳��� ����
		}
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���ദ��
	nErr = E_SUCCESS;
	switch( optname )
	{
	//���������Ϣ
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
	//�رձ������
	case SO_KEEPALIVE:
		if( *(BOOL*)optval )
		{
			if( !Tcp_StartKeepAlive(pTcbTcp) )
			{
				nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
			}
		}
		else
		{
			Tcp_StopKeepAlive( pTcbTcp );
		}
		break;
	//����OOB����
	//case SO_OOBINLINE:
	//	if( optlen!=sizeof(BOOL) )
	//	{
	//		nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
	//	}
	//	else
	//	{
	//	}
	//	break;
	//���ý��ջ���ĳ���
	case SO_RCVBUF:
		//���� ���ý��ջ���ĳ���
//		if( !RWBuf_Alloc( &pTcbTcp->stRxBufRW, *(DWORD*)optval, MIN_RXBUF ) )
//		{
//			nErr = E_NOBUFS;
//		}
		pTcbTcp->dwMaxRxBufSize = MAX( *(DWORD*)optval, MIN_RXBUF );
		break;
	//���÷��ͻ���ĳ���
	case SO_SNDBUF:
		//���� ���ý��ջ���ĳ���
		if( !RWBuf_Alloc( &pTcbTcp->stTxBufRW, *(DWORD*)optval, MIN_TXBUF ) )
		{
			nErr = E_NOBUFS;
		}
		break;

	//���ñ��ص�ַ
	//case SO_REUSEADDR:
	//	if( optlen!=sizeof(BOOL) )
	//	{
	//		nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
	//	}
	//	else
	//	{
	//	}
	//	break;

	//Unknown�����߲�֧��
	default :
		nErr = E_NOPROTOOPT;	//ѡ������� Unknown�����߲�֧��
		break;
	}

	return nErr;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	OUT addr-�����ַ��Ϣ
//	OUT paddr_len-�����ַ�ĳ���
//	IN peer-ָ���Է����߱���
// ����ֵ��
//	
// ������������ȡ���ػ��߶Է��ĵ�ַ��Ϣ
// ����: 
// ********************************************************************
SKERR	TcpSk_GetName( SOCK_INST* pSockInst, OUT SOCKADDR* addr, OUT int* paddr_len, int peer )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pAddrIn || (*paddr_len<sizeof(SOCKADDR_IN)) )
	{
		//��������
		return E_FAULT;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	if( pTcbTcp->dwState!=TCPS_ESTABLISHED )
	{
		//�����Ѿ��Ͽ�
		return E_NOTCONN;
	}

	//��ȡ��ַ��Ϣ
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
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
//	IN how-�رյķ�ʽ
// ����ֵ��
//	
// ���������������رձ��ض�
// ����: 
// ********************************************************************
SKERR	TcpSk_ShutDown( SOCK_INST* pSockInst, int how )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;
	//DWORD			dwTickStart;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//���״̬��
	if( !(pTcbTcp->wAttr & TCPA_ESTABLISH) )
	{
		//���Ƚ�������
		return E_NOTCONN;
	}
	if( pTcbTcp->dwState==TCPS_CLOSED )
	{
		return E_SUCCESS;
	}

	//�����Ƿ�Ҫ�շ�����
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
	
	//����FIN״̬
	if( pTcbTcp->dwState==TCPS_ESTABLISHED )
	{
		pTcbTcp->dwState = TCPS_FIN_WAIT1;
		
		SetEvent( pTcbTcp->hEvtSend );
	}

	//��Ҫ�ȴ���TCPS_CLOSED��
	//dwTickStart = GetTickCount();
	//while( (pTcbTcp->dwState!=TCPS_CLOSED) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	//{
	//	//�ȴ����ӵĽ���
	//	WaitForSingleObject( pTcbTcp->hEvtPoll, 2000 );
	//	if( (GetTickCount()-dwTickStart) > TCPTM_WAITSHUT )
	//	{
	//		break;
	//	}
	//}

	return E_SUCCESS;
}

// ********************************************************************
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
// ����ֵ��
//	
// �����������ر����ӣ��ͷ�TCB
// ����: 
// ********************************************************************
SKERR	TcpSk_Close( SOCK_INST* pSockInst )
{
	TCP_TCB*		pTcbTcp = (TCP_TCB*)pSockInst->hIfTl;
	TCP_GLOBAL*		lpGlobalTcp;

	//�����ȫ���
	if( !HANDLE_CHECK(pTcbTcp) )
	{
		return E_NOTSOCK;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( !HANDLE_CHECK(lpGlobalTcp) )
	{
		return E_NETDOWN;
	}
	//�����˳���־
	pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	pTcbTcp->wFlag |= TCPF_EXIT;
	pTcbTcp->nErr = E_INTR;
	Sleep( 1 );
	//���״̬��
	if( (pTcbTcp->dwState!=TCPS_CLOSED) &&
		(pTcbTcp->dwState!=TCPS_LISTEN) &&
		(pTcbTcp->dwSrcIP) && (pTcbTcp->dwDesIP) && 
		(pTcbTcp->wSrcPort) && (pTcbTcp->wDesPort)
	  )
	{
		//����RST��֪ͨ�Է��˳�
		if( (pTcbTcp->dwSrcIP) && (pTcbTcp->dwDesIP) && (pTcbTcp->wSrcPort) && (pTcbTcp->wDesPort) )
		{
			TcpOut_SendRst( pTcbTcp, pTcbTcp->dwSrcIP, pTcbTcp->wSrcPort, pTcbTcp->dwDesIP, pTcbTcp->wDesPort, 0 );
		}
	}
	//֪ͨ�Լ��˳�
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

	//����еĻ����ͷ����� hListSynRcvd �б�
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
			//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
			pTcbSynRcvd = LIST_CONTAINER( pUnit, TCP_TCB, hListSynRcvd );
			pUnit = pUnit->pNext;
			
			//��ʼ�ͷ�
			Sleep( 1 );
			List_RemoveUnit( &pTcbSynRcvd->hListSynRcvd );
			List_InitHead( &pTcbSynRcvd->hListSynRcvd );
			Sleep( 1 );
			Tcp_DeleteTCB( pTcbSynRcvd );
		}
		LeaveCriticalSection( &pTcbTcp->csTcb );
	}

	//�ͷ�TCB
	Sleep( 1 );
	Tcp_DeleteTCB( pTcbTcp );

	//������Ϣ
	pSockInst->hIfTl = NULL;

	return E_SUCCESS;
}




/////////////////////////////////////////////////////////////////////
//
//�ڲ���������
//
/////////////////////////////////////////////////////////////////////


// ********************************************************************
// ������
// ������
//	IN dwSrcIP-ָ����IP
//	IN wSrcPort-ָ���Ķ˿�
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbTcp-Ҫ���ҵ�TCB
// ����ֵ��
//	�ҵ��󣬷���TRUE��ʧ�ܷ���FALSE
// ��������������ָ����IP�Ͷ˿� ��TCB
// ����: 
//ע��˵����
//���ڱ��ص�ַ��: �ڡ����ء��ġ�ĳ��IP��ַ���ϵġ�Port�š�����Ψһʹ�õ�
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
		//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
		pTcbTcp_Find = LIST_CONTAINER( pUnit, TCP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 
		
		//�˿ڱȽ�
		if( pTcbTcp_Find->wSrcPort==wSrcPort )
		{
			//��ַ�Ƚ�
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
// ������
// ������
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbTcp-Ҫ�����¶˿ڵ�TCB
// ����ֵ��
//	
// ������������TCB�����¶˿�
// ����: 
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
			//�����¶˿ڣ����Ұ󶨶˿�
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
// ������
// ������
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbTcp-Ҫʹ���¶˿ڵ�TCB
// ����ֵ��
//	
// ������������TCBʹ���¶˿�
// ����: 
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
// ������
// ������
//	IN/OUT ppTcbTcp-Ҫ�Զ��󶨵�TCB
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	
// ������������TCB�Զ���Դ��ַ��
// ����: 
// ********************************************************************
SKERR	Tcp_AutoBind( TCP_TCB* pTcbTcp, DWORD dwDesIP, TCP_GLOBAL* lpGlobalTcp )
{
	//��ȡȱʡ��IP��ַ
	if( pTcbTcp->dwSrcIP==INADDR_ANY )
	{
		if( !IP_GetBestAddr( &pTcbTcp->dwSrcIP, dwDesIP, TRUE ) )
		{
			//��IP��ַ��Ч
			return E_ADDRNOTAVAIL;
		}
	}

	//����1���¶˿�
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
// ������
// ������
//	IN pTcbTcp-TCBʵ����Ϣ
//	IN pSockInst_New-�������ӵ�socketʵ����Ϣ
//	IN pAddrIn-ָ��Ŀ�ĵ�ַ��Ϣ
//	IN paddr_len-����ָ��addr�ĳ���
// ����ֵ��
//	
// ������������������Ŀ�ĵ����ӵ���Ϣ
// ����: 
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
		//���û�����ӣ�ֱ�ӷ���
		//RETAILMSG(1,(TEXT("  Tcp_LookSynRcvd: No hListSynRcvd\r\n")));
		return FALSE;
	}
	//
	pTcbTcp_New = NULL;
	pUnitHeader = &pTcbTcp->hListSynRcvd;
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
		pTcbSynRcvd = LIST_CONTAINER( pUnit, TCP_TCB, hListSynRcvd );
		pUnit = pUnit->pNext;
		
		//ע�⣺ tcp_in.c ����֪ͨ�˳�
		if( (pTcbSynRcvd->wFlag & TCPF_EXIT) || ((GetTickCount()-pTcbSynRcvd->dwTickLife)>=TCPTM_SYNRCVD) )
		{
			//��Ҫ���ͷ�
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
			//��accept�� hListSynRcvd���� ��ȡ����
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

	//ȡ����
	if( pTcbTcp_New && !(pTcbTcp_New->wFlag & TCPF_EXIT) )
	{
		EnterCriticalSection( &pTcbTcp_New->csTcb );
		//��ӵ�ppSockInst_New
		pSockInst_New->hIfTl = (HANDLE)pTcbTcp_New;
		//��ȡ��ַ��Ϣ
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
// ������
// ������
//	IN pTblPoll-Ҫ��ѯ��ʵ����Ϣ
//	IN nCntPoll-ָ��pTblPoll��Ԫ�صĸ���
// ����ֵ��
//	
// ������������ѯ��Ӧ����Ϣ
// ����: 
// ********************************************************************
int	Tcp_PollInfo( POLLTBL* pTblPoll, int nCntPoll )
{
	TCP_TCB*		pTcbTcp;
	DWORD			dwFlag;
	int				nReady;
	int				i;

	//��ѯÿ��
	nReady = 0;
	for( i=0; i<nCntPoll; i++ )
	{
		pTcbTcp = (TCP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//�����ȫ���
		if( HANDLE_F_FAIL(pTcbTcp, TCPF_EXIT) )
		{
			pTblPoll[i].dwFlag |= SOCK_POLL_READ;
			nReady ++;
			continue;
		}
		dwFlag = pTblPoll[i].dwFlag;
		//��ɶ���
		if( dwFlag & SOCK_POLL_QREAD )
		{
			if( pTcbTcp->dwState==TCPS_ESTABLISHED )
			{
				//�鿴�Ƿ������ݣ�����Read
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
					//accept�Ƿ�õ�1��������---accept֧��Nonblocking����
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
				//�����Ƿ����closed/reset/terminated
				pTblPoll[i].dwFlag |= SOCK_POLL_READ;
				nReady ++;
			}
		}
		//���д��
		if( dwFlag & SOCK_POLL_QWRITE )
		{
			if( pTcbTcp->dwState==TCPS_ESTABLISHED )
			{
				//�鿴�����Ƿ�����ɣ����Լ���Write
				if( (pTcbTcp->stTxBufRW.dwCntRW==0) && !( pTcbTcp->dwLenLeftTx) )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_WRITE;
					nReady ++;
				}
				//�鿴connect�Ƿ�����ɣ����Լ���Write
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
		//��������
		if( dwFlag & SOCK_POLL_QEXCEPT )
		{
			//�鿴connect�Ƿ�ʧ��
			if( pTcbTcp->wAttr & TCPA_CONNECT )
			{
				if( pTcbTcp->wFlag & TCPQ_CNN_NO )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_EXCEPT;
					nReady ++;
					
					pTcbTcp->wFlag &= ~TCPQ_CNN_NO;
				}
			}
			//�鿴accept�Ƿ�ʧ��
			if( pTcbTcp->wAttr & TCPA_LISTEN )
			{
				//accept�Ƿ�õ�1��������---accept֧��Nonblocking����
				if( pTcbTcp->wFlag & TCPQ_LISTEN_NO )
				{
					pTblPoll[i].dwFlag |= SOCK_POLL_EXCEPT;
					nReady ++;
					
					pTcbTcp->wFlag &= ~TCPQ_LISTEN_NO;
				}
			}
			//�鿴�Ƿ���OOB���ݿɶ�
			//
		}
	}

	return nReady;
}


// ********************************************************************
// ������
// ������
//	IN pTcbTcp-TCBʵ����Ϣ
// ����ֵ��
//	
// ���������������������
// ����: 
// ********************************************************************
BOOL	Tcp_StartKeepAlive( TCP_TCB* pTcbTcp )
{
	pTcbTcp->dwOption &= ~TCPIPO_NONKEEPALIVE;

	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN pTcbTcp-TCBʵ����Ϣ
// ����ֵ��
//	
// �����������رձ������
// ����: 
// ********************************************************************
void	Tcp_StopKeepAlive( TCP_TCB* pTcbTcp )
{
	pTcbTcp->dwOption |= TCPIPO_NONKEEPALIVE;
}

