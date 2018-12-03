/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����UDP Socket������ ����
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
#include "udp_def.h"


/***************  ȫ���� ���壬 ���� *****************/


//

//TCP�ṩ�ĺ�����
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

//�ڲ���������
static	BOOL	Udp_FindPort( DWORD dwSrcIP, WORD wSrcPort, UDP_GLOBAL* lpGlobalUdp, UDP_TCB** ppTcbUdp );
static	BOOL	Udp_NewPort( UDP_TCB* pTcbUdp, UDP_GLOBAL* lpGlobalUdp );
static	BOOL	Udp_UsePort( UDP_TCB* pTcbUdp, WORD wSrcPort, UDP_GLOBAL* lpGlobalUdp );

static	int	Udp_PollInfo( POLLTBL* pTblPoll, int nCntPoll );




/******************************************************/


// ********************************************************************
// ������
// ������
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	
// ������������ʼ�� �������Ϣ
// ����: 
// ********************************************************************
BOOL	UdpSk_Init( UDP_GLOBAL* lpGlobalUdp )
{
	TL_LOCAL*			pTlLocal;

	//��ʼ�� �������Ϣ
	pTlLocal = &lpGlobalUdp->LocalTl;
	HANDLE_SET( pTlLocal );
	List_InitHead( &pTlLocal->hListTl );
	pTlLocal->hTlGlobal = (HANDLE)lpGlobalUdp;

	pTlLocal->nFamily = AF_INET;
	pTlLocal->nType = SOCK_DGRAM;
	pTlLocal->nProtocol = IPPROTO_UDP;

	//��� ����㺯����
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
// ������
// ������
//	IN pSockInst-socketʵ����Ϣ
// ����ֵ��
//	
// ��������������TCB
// ����: 
// ********************************************************************
SKERR	UdpSk_Create( SOCK_INST* pSockInst )
{
	UDP_GLOBAL*	lpGlobalUdp;
	UDP_TCB*	pTcbUdp;

	//�����ȫ���
	lpGlobalUdp = (UDP_GLOBAL*)pSockInst->pTlLocal->hTlGlobal;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
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
	if( !Udp_CreateTcb( &pTcbUdp, lpGlobalUdp ) )
	{
		//�ռ䲻���� ʧ��
		return E_NOBUFS;
	}

	//��ʼ��
	//pTcbUdp->dwSrcIP = INADDR_ANY;

	//������Ϣ
	pSockInst->hIfTl = (HANDLE)pTcbUdp;

	//������
	EnterCriticalSection( &lpGlobalUdp->csListTcb );
	List_InsertTail( &lpGlobalUdp->hListTcb, &pTcbUdp->hListTcb );
	LeaveCriticalSection( &lpGlobalUdp->csListTcb );

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
SKERR	UdpSk_Bind( SOCK_INST* pSockInst, const SOCKADDR* myaddr, int addr_len )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)myaddr;
	DWORD			dwSrcIP;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pAddrIn || (addr_len<sizeof(SOCKADDR_IN)) )
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
	if( pTcbUdp->wSrcPort )
	{
		//��socket�Ѿ�����
		return E_INVAL;
	}

	//�󶨱���IP��ַ
	if( !(pTcbUdp->dwOption & TCPIPO_IPINTF) )
	{
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
		pTcbUdp->dwSrcIP = dwSrcIP;
	}
	//�󶨱��ض˿�
	if( pAddrIn->sin_port==0 )
	{
		//�����¶˿ڣ����Ұ󶨶˿�
		if( !Udp_NewPort( pTcbUdp, lpGlobalUdp ) )
		{
			pTcbUdp->dwSrcIP = INADDR_ANY;
			//�����¶˿�ʧ��
			return E_NOMOREPORT;
		}
	}
	else
	{
		if( !(pTcbUdp->dwOption & TCPIPO_IPINTF) )
		{
			//���ö˿��Ƿ�ռ�ã����Ұ󶨶˿�
			if( !Udp_UsePort(pTcbUdp, pAddrIn->sin_port, lpGlobalUdp) )
			{
				pTcbUdp->dwSrcIP = INADDR_ANY;
				//�����ö˿ڱ�ռ��
				return E_ADDRINUSE;
			}
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
SKERR	UdpSk_Connect( SOCK_INST* pSockInst, const SOCKADDR* addr, int addr_len, int flags )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SOCKADDR_IN*	pAddrIn = (SOCKADDR_IN*)addr;
	DWORD			dwDesIP;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pAddrIn || (addr_len<sizeof(SOCKADDR_IN)) )
	{
		//��������
		return E_FAULT;
	}
	if( pAddrIn->sin_family!=AF_INET )
	{
		//��֧�ֵ�Э����
		return E_AFNOSUPPORT;
	}
	dwDesIP = pAddrIn->sin_addr.S_un.S_addr;
	if( dwDesIP==INADDR_ANY || INADDR_TEST_BC(dwDesIP) || pAddrIn->sin_port==0 )
	{
		//Զ�̵�ַ��Ч---�ͻ��������ӵ� ���κε�ַ�� �� ���㲥��ַ��
		return E_ADDRNOTAVAIL;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//ȷ�� dwSrcIP and wSrcPort---������ʹ��ANY IP������ҪPORT
	if( pTcbUdp->wSrcPort==0 )
	{
		//�Զ���
		if( !Udp_NewPort( pTcbUdp, lpGlobalUdp ) )
		{
			return E_NOMOREPORT;
		}
	}
	//�������ݷ����Լ������IP�����PORT��
	if( pTcbUdp->wSrcPort==pAddrIn->sin_port )
	{
		if( pTcbUdp->dwSrcIP==dwDesIP )
		{
			//Զ�̵�ַ��Ч
			return E_ADDRNOTAVAIL;
		}
		if( pTcbUdp->dwSrcIP==INADDR_ANY )
		{
			if( IP_IsValidAddr(dwDesIP) )
			{
				//Զ�̵�ַ��Ч
				return E_ADDRNOTAVAIL;
			}
		}
	}

	//����Զ�̵�ַ
	pTcbUdp->dwDesIP = dwDesIP;
	pTcbUdp->wDesPort = pAddrIn->sin_port;

	//ע�⣺PC-Windows���������£���ϵͳ�����ɣ�
	//�ж��� dwDesIP��pTcbUdp->dwSrcIP�Ƿ���ͬ1�������ڣ�������ڣ��򷵻�10065

	return E_SUCCESS;
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
SKERR	UdpSk_Listen( SOCK_INST* pSockInst, int queue_len )
{
	return E_OPNOTSUPP;
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
SKERR	UdpSk_Accept( SOCK_INST* pSockInst, OUT SOCK_INST* pSockInst_New, OUT SOCKADDR* addr, OUT int* paddr_len, int flags )
{
	return E_OPNOTSUPP;
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
SKERR	UdpSk_IOCtl( SOCK_INST* pSockInst, long nCmd, IN OUT DWORD* pdwArg )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SKERR			nErr;
	//int				nOptVal;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
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
			pTcbUdp->dwOption |= TCPIPO_NONBLOCK;
		}
		else
		{
			pTcbUdp->dwOption &= ~TCPIPO_NONBLOCK;
		}
		break;

	//��ȡ���ջ��������ݵĳ���
	case FIONREAD :
		//*pdwArg = pTcbUdp->stRxBufRW.dwCntRW;
		*pdwArg = Udp_GetRxFirstData( pTcbUdp, NULL, NULL, NULL );
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
SKERR	UdpSk_SendMsg( SOCK_INST* pSockInst, RXTXMSG* pMsgRxTx )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//��������
	if( !pMsgRxTx || !pMsgRxTx->pData )
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
	//�ɹ����أ����û�еķ�������
	if( !pMsgRxTx->dwLenData )
	{
		return E_SUCCESS;
	}
	if( pTcbUdp->wSrcPort==0 )
	{
		//�Զ���
		if( !Udp_NewPort( pTcbUdp, pTcbUdp->lpGlobalUdp ) )
		{
			return E_NOMOREPORT;
		}
	}

	return Udp_Send( pTcbUdp, pMsgRxTx );
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
SKERR	UdpSk_RecvMsg( SOCK_INST* pSockInst, IN OUT RXTXMSG* pMsgRxTx )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	LPBYTE			pData;
	DWORD			dwLenData;
	//DWORD			dwLenTmp;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
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
	//
	while( !(pTcbUdp->wFlag & UDPF_EXIT) )
	{
		//�鿴�Ƿ������ݣ����ӽ��ջ����� ��������
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
			//������ �첽����
			return E_WOULDBLOCK;
		}

		//�ȴ����͵����
		WaitForSingleObject( pTcbUdp->hEvtPoll, UDPTM_WAITRECV );//INFINITE
	}

	return E_INTR;
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

	//�����ȫ���
	if( HANDLE_F_FAIL(g_lpGlobalUdp, TCPIPF_EXIT) )
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
	*pnCntPoll = Udp_PollInfo( pTblPoll, nCntWait );
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
		pTcbUdp = (UDP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//�����ȫ���
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
	//��ʼ�ȴ�
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
			//�Ѿ���ȡ��Ϣ
			return E_SUCCESS;
		}
		else if( !fWaittingForYou )
		{
			if( (GetTickCount()-dwTickStart)>=dwTmout )
			{
				//��ʱ�˳�
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
SKERR	UdpSk_GetSockOpt( SOCK_INST* pSockInst, int level, int optname, OUT char* optval, int optlen )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SKERR			nErr;
	//int				nOptVal;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
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
			//*(int*)optval = (int)pTcbUdp->stRxBufRW.dwLenRW;
			*(int*)optval = (int)pTcbUdp->dwLenRcvBuf;
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
SKERR	UdpSk_SetSockOpt( SOCK_INST* pSockInst, int level, int optname, const char* optval, int optlen )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;
	SKERR			nErr;
	//int				nOptVal;
	DWORD			dwOptVal;

	//�����ȫ���
	if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
	{
		return E_NETDOWN;
	}
	//�������
	if( !IP_IsUp() )
	{
		//�����ʧ��
		return E_NETDOWN;
	}
	//��������
	if( level!=SOL_SOCKET )
	{
		//if( level!=IPPROTO_IP )
		//{
		//}
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
	//���ദ��
	nErr = E_SUCCESS;
	switch( optname )
	{
	//���������Ϣ
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

	//����OOB����
	//case SO_OOBINLINE:
	//	break;

	//���ý��ջ���ĳ���
	case SO_RCVBUF:
		//dwOptVal = *(DWORD*)optval;
		//���� ���ý��ջ���ĳ���
		//if( !RWBuf_Alloc( &pTcbUdp->stRxBufRW, *(DWORD*)optval, MIN_RXBUF ) )
		if( !Udp_SetRxNodeLen( pTcbUdp, *(DWORD*)optval ) )
		{
			nErr = E_NOBUFS;
		}
		break;

	case SO_SNDBUF:
		break;

	//���ñ��ص�ַ
	//case SO_REUSEADDR:
	//	break;

	//���������շ��㲥
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

	//����ֱ��ʹ��ָ��IP�ӿ��շ����ݵ� IP�ӿ�����
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
				nErr = E_FAULT;	//ѡ������ݻ��߳��� ����
			}
		}
		break;

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
SKERR	UdpSk_GetName( SOCK_INST* pSockInst, OUT SOCKADDR* addr, OUT int* paddr_len, int peer )
{
	return E_OPNOTSUPP;
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
SKERR	UdpSk_ShutDown( SOCK_INST* pSockInst, int how )
{
	return E_OPNOTSUPP;
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
SKERR	UdpSk_Close( SOCK_INST* pSockInst )
{
	UDP_TCB*		pTcbUdp = (UDP_TCB*)pSockInst->hIfTl;
	UDP_GLOBAL*		lpGlobalUdp;

	//�����ȫ���
	if( !HANDLE_CHECK(pTcbUdp) )
	{
		return E_NOTSOCK;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( !HANDLE_CHECK(lpGlobalUdp) )
	{
		return E_NETDOWN;
	}
	//�ͷ�TCB
	Udp_DeleteTCB( (HANDLE)pTcbUdp );

	//������Ϣ
	pSockInst->hIfTl = NULL;

	return E_SUCCESS;
}




/////////////////////////////////////////////////////////////////////
//
//�ڲ�����
//
/////////////////////////////////////////////////////////////////////

// ********************************************************************
// ������
// ������
//	IN dwSrcIP-ָ����IP
//	IN wSrcPort-ָ���Ķ˿�
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbUdp-Ҫ���ҵ�TCB
// ����ֵ��
//	�ҵ��󣬷���TRUE��ʧ�ܷ���FALSE
// ��������������ָ����IP�Ͷ˿� ��TCB
// ����: 
//ע��˵����
//���ڱ��ص�ַ��: �ڡ����ء��ġ�ĳ��IP��ַ���ϵġ�Port�š�����Ψһʹ�õ�
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
		
		//�˿ڱȽ�
		if( pTcbUdp_Find->wSrcPort==wSrcPort )
		{
			//��ַ�Ƚ�
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
// ������
// ������
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbUdp-Ҫ�����¶˿ڵ�TCB
// ����ֵ��
//	
// ������������TCB�����¶˿�
// ����: 
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
			//�����¶˿ڣ����Ұ󶨶˿�
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
// ������
// ������
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbUdp-Ҫʹ���¶˿ڵ�TCB
// ����ֵ��
//	
// ������������TCBʹ���¶˿�
// ����: 
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
// ������
// ������
//	IN pTblPoll-Ҫ��ѯ��ʵ����Ϣ
//	IN nCntPoll-ָ��pTblPoll��Ԫ�صĸ���
// ����ֵ��
//	
// ������������ѯ��Ӧ����Ϣ
// ����: 
// ********************************************************************
int	Udp_PollInfo( POLLTBL* pTblPoll, int nCntPoll )
{
	UDP_TCB*		pTcbUdp;
	DWORD			dwFlag;
	int				nReady;
	int				i;

	//��ѯÿ��
	nReady = 0;
	for( i=0; i<nCntPoll; i++ )
	{
		pTcbUdp = (UDP_TCB*)pTblPoll[i].pSockInst->hIfTl;
		//�����ȫ���
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			pTblPoll[i].dwFlag |= SOCK_POLL_READ;
			nReady ++;
			continue;
		}
		dwFlag = pTblPoll[i].dwFlag;
		//��ɶ���
		if( dwFlag & SOCK_POLL_QREAD )
		{
			//�鿴�Ƿ������ݣ�����Read
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
		//���д��
		//if( dwFlag & SOCK_POLL_QWRITE )
		//{
		//}
		//��������
		//if( dwFlag & SOCK_POLL_QEXCEPT )
		//{
		//}
	}

	return nReady;
}


