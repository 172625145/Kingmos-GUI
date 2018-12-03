/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TCP 
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��

 ���汾TCPIP ˵����ФԶ�� 2004-04-12 д����
1�� ���״������ɣ������������ϵļ���ϣ�����д�ģ������������е���ػ��ơ�
    �ο����ϣ�RFC/IANA��WHL��TCPIP�汾��TCPIP��⡢���ʻ���
2�� ֧��Block��Nonblock����
3�� ����û�connect����ʧ�ܺ󣬻����Լ������Ե���connect
4�� ����û�connect���ӳɹ����ٶϿ��������Լ������Ե���connect
5�� �������connect�󣬾Ͳ����ٵ���listen/accept��
    ͬ������listen/accept�󣬾Ͳ����ٵ���connect
6�� ��������£�ɾ��TCB�����ϲ������ĵ���ִ�еġ���������TCPS_SYN_RECVD��TCB��
    ������TcpIn_SynRcvdɾ�� ���� ��������ʱ��û�б����ӳɹ�ʱ�������ɾ��
7�� ���accept����ʧ�ܣ����Բ鿴slect������socket----MSDN û������ܡ�
8�� 1��socketֻ�ܳɹ�����1��listen
9�� ֧�ַ��ͻ��ƵĲ����У�
    �״η��͡���ʱ�ش��������ش����Ӵ�ʱ�ӵ�ACK��֪ͨRST��֪ͨACK��
	����RTT��RTO����ֶ�ʱ��
10����TCPS_SYN_RECVDʱ��TCB��û�й����ϲ�ṹ�ģ����Դ���ɾ������Է��Ͽ�FIN�����⡣
    ɾ���������������Ͽ�ʱ��ֱ�ӽ���TCPS_CLOSED�����÷���FIN��
11���μ�������Tcp_ChkSegment�����յ�RST�������Է��Ѿ��Ͽ�����ʱ
    �������TCPF_EXIT�����û��˳�TCB���ŵ�---���Ը���ĳ���ռ�ö˿ڵĻ��᣻
    ���������TCPF_EXIT���ŵ�---���û�������������
12��TCP��accept����ʹ��ANY IP��connect����Ҫ�󶨻��Զ��󶨹̶�IP��
13��UDP������ANY IPʱ�ģ����ܷ�����
    1> ���Խ��գ����������ĸ�PORT������
    2> ����ѡ��1���ʵ�������IP�����з��ͣ���ν�ʵ����ǣ�����ѡ����Ŀ��IP��ͬ1�������ڵ�����IP
    3> ���������Ƚ��ʺ�ͬ1̨�����в�ͬ�����Ķ���������������ԣ�
       ����������������ѡ��Ŀ�������ڵ��������͡�

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

TCP_GLOBAL*		g_lpGlobalTcp = NULL;


static	void	Tcp_Free( TCP_GLOBAL* lpGlobalTcp );


/******************************************************/


// ********************************************************************
// ������
// ������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ������������ʼ�� TCP
// ����: 
// ********************************************************************
BOOL	Tcp_Init( )
{
	TCP_GLOBAL*		lpGlobalTcp;

	//����1��TCP�Ĺ��� �ͻ�����ʼ��
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
// ������
// ������
// ����ֵ��
//	
// �����������ͷ� TCP
// ����: 
// ********************************************************************
void	Tcp_Free( TCP_GLOBAL* lpGlobalTcp )
{
	//�����ȫ���
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

	//�����ȫ���
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
	{
		return FALSE;
	}
	//���� �� ������ʼ��
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

	//�շ�---����BUFFER
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

	//�����ȫ���
	if( !HANDLE_CHECK(pTcbTcp) )
	{
		return ;
	}
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( !HANDLE_CHECK(lpGlobalTcp) )
	{
		return ;
	}
	//�����˳���־��״̬
	pTcbTcp->dwState = TCPS_CLOSED;
	pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	pTcbTcp->wFlag |= TCPF_EXIT;
	pTcbTcp->nErr = E_INTR;
	Sleep( 1 );
	//֪ͨ�Լ��˳�
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

	//�� lpGlobalTcp������ �� accept�� hListSynRcvd���� �Ͽ�
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	List_RemoveUnit( &pTcbTcp->hListTcb );
	List_RemoveUnit( &pTcbTcp->hListSynRcvd );
	List_InitHead( &pTcbTcp->hListTcb );
	List_InitHead( &pTcbTcp->hListSynRcvd );
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	//�ͷ�
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
	//�ͷž��
	HANDLE_FREE( pTcbTcp );
}


BOOL	Tcp_QueryMSS( DWORD dwSrcIP, DWORD dwDesIP, OUT WORD* pwMSS_me )
{
	NETINFO_IPV4		InfoNet;

	//��ѯ IP��������Ϣ
	if( !IP_Query( dwSrcIP, &InfoNet ) )
	{
		//�����ʧ��
		return FALSE;
	}
	//��ȡ����MSS
	if( ((dwSrcIP & InfoNet.dwIP_Mask)==(dwDesIP & InfoNet.dwIP_Mask)) || (InfoNet.wMTU<=512) )
	{
		//ͬ1������
		*pwMSS_me = InfoNet.wMTU - sizeof(IP_HDR) - sizeof(TCP_HDR);	//attention_xyg_2004-04-19
	}
	else
	{
		//����ͬ1������
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

	//��ȡʣ��ռ�
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

	//����ISN������������ʼ��
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













