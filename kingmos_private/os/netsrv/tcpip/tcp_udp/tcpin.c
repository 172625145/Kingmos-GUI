/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����TCP��������״̬�� ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��

ע�ͣ�
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

/***************  ȫ���� ���壬 ���� *****************/

//TCP �ַ����� ״̬��
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

//�ڲ���������
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
	TcpIn_Closed,//#define	TCPS_CLOSED				0	//�رս���
	TcpIn_Listen,//#define	TCPS_LISTEN				1	//����
	TcpIn_SynSent,//#define	TCPS_SYN_SENT			2	//����SYN
	TcpIn_SynRcvd,//#define	TCPS_SYN_RECVD			3	//����SYN
	TcpIn_Established,//#define	TCPS_ESTABLISHED		4	//����
	TcpIn_CloseWait,//#define	TCPS_CLOSE_WAIT			5	//�����ر�
	TcpIn_LastACK,//#define	TCPS_LAST_ACK			6	//Ӧ��ر�
	TcpIn_FinWait1,//#define	TCPS_FIN_WAIT1			7	//�����ر�
	TcpIn_FinWait2,//#define	TCPS_FIN_WAIT2			8	//�����ر�
	TcpIn_Closing,//#define	TCPS_CLOSING			9	//ͬʱ�ر�
	TcpIn_TimeWait,//#define	TCPS_TIME_WAIT			10	//�ȴ��ر�
//#define	TCPS_MAX				11
};
// ********************************************************************
// ������
// ������
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ������������ʼ��TCP����
// ����: 
// ********************************************************************
BOOL	TcpIn_Init( TCP_GLOBAL* lpGlobalTcp )
{
	//��� TCP����״̬���ĺ�����
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
	//�����ȫ���
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
	{
		goto _return;
	}
	//���У����
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

	//Ѱ�� Ŀ��TCB
	if( !Tcp_MatchTcb(&pTcbTcp, g_lpGlobalTcp, pHdrIP, pHdrTcp) )
	{
		//����RST֪ͨ�Է���������
		TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, ntohl(pHdrTcp->dwSeqNum) );
		goto _return;
	}

	//
	//�ύ��״̬������
	//
	EnterCriticalSection( &pTcbTcp->csTcb );

	//���� �� ת�������Ϣ
#ifndef BIG_END
	pHdrTcp->dwSeqNum = ntohl( pHdrTcp->dwSeqNum );
	pHdrTcp->dwAckNum = ntohl( pHdrTcp->dwAckNum );
	pHdrTcp->wWinSize = ntohs( pHdrTcp->wWinSize );
#endif
	pTcbTcp->wLenHdr = wHLen;
	pTcbTcp->wLenData = wLenData - wHLen;
	pTcbTcp->pTcpData = pBufData + wHLen;
	//�� IPDATA_IN ��ת��ΪTCPDATA_IN��
	pTcbTcp->pCurDataIn = pDataIn;
	

	//��������
	
	dwDesIP = pHdrIP->dwDesIP;
	dwSrcIP = pHdrIP->dwSrcIP;
	
	wDesPort = pHdrTcp->wDesPort;
	wSrcPort = pHdrTcp->wSrcPort;
	//���¹��ܿ��ܸı� ������

	//HeapValidate( GetProcessHeap(), 0, NULL );
	//(*pTcbTcp->lpGlobalTcp->pFnTcpIn[pTcbTcp->dwState])( pTcbTcp, pHdrIP, pHdrTcp );
	
	pFnTcpIn[pTcbTcp->dwState]( pTcbTcp, pHdrIP, pHdrTcp, &bAutoFree );
	//HeapValidate( GetProcessHeap(), 0, NULL );
//hggg  ghgh
	//
	if( pTcbTcp->wFlag & TCPF_NEEDACK )
	{
		//���� ������ʱ�ӵ�ACK��
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
	{	//�Զ��ͷ�
		IPDataIn_Free( pDataIn );
	}

	return dwRetv;
}


void	TcpIn_IcmpNotify( BYTE bType, BYTE bCode, IP_HDR* pHdrIP_icmp, WORD wLenData )
{
	TCP_HDR*		pHdrTcp;
	TCP_TCB*		pTcbTcp;
	WORD			wLenHdr;

	//�����ȫ���
	if( HANDLE_F_FAIL(g_lpGlobalTcp, TCPIPF_EXIT) )
	{
		return ;
	}
	//��֤ wLenData>=(sizeof(IP_HDR)+sizeof(TCP_HDR))
	wLenHdr = ((pHdrIP_icmp->bVerHLen & IPL_MASK)<<2);
	if( wLenData<(wLenHdr + sizeof(TCP_HDR)) )
	{
		return ;
	}
	//Ѱ�Ҹö˿�
	pHdrTcp = (TCP_HDR*)((LPBYTE)pHdrIP_icmp + wLenHdr);
	if( !Tcp_MatchTcb( &pTcbTcp, g_lpGlobalTcp, pHdrIP_icmp, pHdrTcp ) )
	{
		return ;
	}

	//����ICMP����Ϣ
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
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ��������������ʱ��������յ���Ϣ
//1����RST|FINʱ������
//2���ա���ACK ���� û��SYN��ʱ������RST֪ͨ�Է���������
//3���������������
//4����ѯ IP��������Ϣ �� ����MSS
//5��������TCB�ṹ�����ڱ�ʾ1��������
//6����ʼ��TCB��״̬������MSS��Socket��
//7������Է�����������Ϣ��SEQ��ѡ�
//8��������TCB�ķ����̣߳�֪ͨ���� (SYN | ACK)
// ����: 
// ********************************************************************
SKERR	TcpIn_Listen( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	TCP_GLOBAL*		lpGlobalTcp;
	TCP_TCB*		pTcbTcp_New;
	WORD			wSnd_MSS;

	//��RST|FINʱ������
	if( pHdrTcp->wHLenFlag & (TCP_FLAG_RST | TCP_FLAG_FIN) )
	{
		//����
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Wrong wHLenFlag1=[RST FIN]\r\n")));
		return 0;
	}
	//�ա���ACK ���� û��SYN��ʱ������RST֪ͨ�Է���������
	if( (pHdrTcp->wHLenFlag & TCP_FLAG_ACK) || !(pHdrTcp->wHLenFlag & TCP_FLAG_SYN) )
	{
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Wrong wHLenFlag2=[ACK]\r\n")));
		//����RST֪ͨ�Է���������
		TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );
		return 0;
	}

	//�������������
	if( pTcbTcp->wCntConn>=pTcbTcp->wMaxConn )
	{
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: wCntConn=[%d], wMaxConn=[%d]\r\n"), pTcbTcp->wCntConn, pTcbTcp->wMaxConn));
		//����RST֪ͨ�Է���������---��Ϊ����1����������Բ�Ҫ��RST����� V1-Page197
		//TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort );
		return 0;
	}
	//��ѯ IP��������Ϣ �� ����MSS
	if( !Tcp_QueryMSS(pHdrIP->dwDesIP, pHdrIP->dwSrcIP, &wSnd_MSS) )
	{
		BYTE*		pIP = (BYTE*)&pHdrIP->dwDesIP;
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Wrong Des IP=[%d.%d.%d.%d]\r\n"), *pIP, pIP[1], pIP[2], pIP[3]));
		//��IP��ַ��Ч
		//return E_ADDRNOTAVAIL;
		return 0;
	}

	//������TCB�ṹ�����ڱ�ʾ1��������
	lpGlobalTcp = pTcbTcp->lpGlobalTcp;
	if( !Tcp_CreateTcb( &pTcbTcp_New, lpGlobalTcp ) )
	{
		RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: Failed to Tcp_CreateTcb\r\n")));
		return 0;
	}

	//��ʼ��TCB��״̬������MSS��Socket��
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

	//����Է�����������Ϣ��SEQ��ѡ�
	pTcbTcp_New->wFlag |= TCPF_RSYN;	//�յ��Է���SYN
	pTcbTcp_New->dwRcv_nxt = pHdrTcp->dwSeqNum + 1;
	Tcp_ParseOpt( pTcbTcp_New, pHdrTcp );

	//������ accept�� hListSynRcvd���� �ϣ�
	pTcbTcp_New->pTcbParent = pTcbTcp;
	pTcbTcp_New->dwTickLife = GetTickCount();
	List_InsertTail( &pTcbTcp->hListSynRcvd, &pTcbTcp_New->hListSynRcvd );
	pTcbTcp->wCntConn ++;

	//������
	EnterCriticalSection( &lpGlobalTcp->csListTcb );
	List_InsertTail( &lpGlobalTcp->hListTcb, &pTcbTcp_New->hListTcb );
	LeaveCriticalSection( &lpGlobalTcp->csListTcb );

	//������TCB�ķ����߳�
	if( !TcpOut_StartSend( pTcbTcp_New ) )
	{
		//һ���������ɹ��ģ�
		Tcp_DeleteTCB( pTcbTcp_New );
		return 0;
	}

	RETAILMSG(1,(TEXT(" \r\n TcpIn_Listen: yes pTcbTcp_New=[0x%x]\r\n"), pTcbTcp_New));
	//֪ͨ���� (SYN | ACK)
	SetEvent( pTcbTcp_New->hEvtSend );

	return 0;
}


// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ��������������SYNʱ��������յ���Ϣ
//1����RST|FINʱ��ֱ���˳�����֪ͨconnect/select
//2����ACKʱ���ж�ACK�Ƿ����---֧��ͬʱ��(please see Figure 8)
//3������wRcv_wnd
//4����SYNʱ������Է���Ϣ---֧��ͬʱ��(please see Figure 8)
//5���ж� �Ƿ�������ӣ���������״̬��֪ͨconnect����������
// ����: 
// ********************************************************************
SKERR	TcpIn_SynSent( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//��RST|FINʱ��ֱ���˳�---���ɣ���û�н�������ǰ�յ�RST|FIN����������connect
	if( pHdrTcp->wHLenFlag & (TCP_FLAG_RST | TCP_FLAG_FIN) )
	{
		//֪ͨconnect/select���˳���ǰ���������ҿ������û���������
		pTcbTcp->dwState = TCPS_CLOSED;
		pTcbTcp->wFlag = TCPQ_CNN_NO;
		pTcbTcp->nErr = E_CONNREFUSED;
		PulseEvent( pTcbTcp->hEvtPoll );
		return 0;
	}

	//��ACKʱ���ж�ACK�Ƿ����---֧��ͬʱ��(please see Figure 8)
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		//��֤�Է���������ACK����ȷ��
		if( Tcp_ChkAck(pTcbTcp, pHdrIP, pHdrTcp) )
		{
			pTcbTcp->wFlag |= TCPF_LSYN;	//�յ��Է���SYN's ACK
		}
		else
		{
			//����RST֪ͨ�Է���������---���ɣ��ο������ʻ�����V2-P193
			TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );
			return 0;
		}
	}

	//����wRcv_wnd
	pTcbTcp->wRcv_wnd = pHdrTcp->wWinSize;

	//��SYNʱ������Է���Ϣ---֧��ͬʱ��(please see Figure 8)
	if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
	{
		//����Է�����������Ϣ��SEQ��ѡ�
		pTcbTcp->wFlag |= TCPF_RSYN;	//�յ��Է���SYN
		pTcbTcp->dwRcv_nxt = pHdrTcp->dwSeqNum + 1;
		Tcp_ParseOpt( pTcbTcp, pHdrTcp );

		//֪ͨ���� ������ʱ�ӵ�ACK���� SYN
		pTcbTcp->wFlag |= TCPF_NEEDACK;
	}

	//�ж� �Ƿ�������ӣ���������״̬��֪ͨconnect����������
	if( (pTcbTcp->wFlag & TCPF_ALLSYN)==TCPF_ALLSYN )
	{
		//�������ӣ���֪ͨconnect/select
		pTcbTcp->dwState = TCPS_ESTABLISHED;

		pTcbTcp->lpGlobalTcp->dwISN += 64000;	//�ο� ����⡱V1-P
		pTcbTcp->wFlag |= TCPQ_CNN_YES;
		pTcbTcp->wAttr |= (TCPA_ESTABLISH | TCPA_RECV | TCPA_SEND);
		PulseEvent( pTcbTcp->hEvtPoll );		//�����Ӻ�֪ͨ

		//���������
		if( pTcbTcp->wLenData )
		{
			Tcp_SavRxData( pTcbTcp, lpbAutoFree );

			//֪ͨ���� ������ʱ�ӵ�ACK��
			pTcbTcp->wFlag |= TCPF_NEEDACK;
		}
	}

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ��������������SYNʱ��������յ���Ϣ
//1����RST|FINʱ��ֱ���˳�
//2����SYNʱ���ȿ��ٷ���ACK(please see Figure 9)
//3���ж� SEQ�Ƿ����
//3������wRcv_wnd
//4��Ҫ����ACK�����ұ�֤ACK_NUM����ȷ��
//5�����·���SEQ��Ϣ����������״̬��֪ͨaccept
//6����FINʱ�����뱻���ر�״̬
// ����: 
// ********************************************************************
SKERR	TcpIn_SynRcvd( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	TCP_TCB*		pTcbParent;

	//�����ȫ���
	pTcbParent = pTcbTcp->pTcbParent;
	if( !HANDLE_CHECK(pTcbParent) )
	{
		pTcbTcp->wFlag = TCPF_EXIT;
		PulseEvent( pTcbParent->hEvtPoll );
		return 0;
	}
	//��RST|FINʱ��ֱ���˳�---��û�н�������ǰ�յ�RST|FIN����Ҫֱ���˳���
	if( pHdrTcp->wHLenFlag & (TCP_FLAG_RST|TCP_FLAG_FIN) )
	{
		//ֱ���˳���Socket---����Ҫ���� ������ʱ�ӵ�ACK��
		pTcbTcp->wFlag = TCPF_EXIT;
		PulseEvent( pTcbParent->hEvtPoll );
		return 0;
	}
	//��SYNʱ���ȿ��ٷ���ACK(please see Figure 9)
	if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
	{
		//֪ͨ���� SYN|ACK
		//SetEvent( pTcbTcp->hEvtSend );
		pTcbTcp->wFlag |= TCPF_NEEDACK;
		return 0;
	}
	//�ж� SEQ�Ƿ����---������ô�϶�ԭ����SEQ��
	if( pHdrTcp->dwSeqNum!=pTcbTcp->dwRcv_nxt )
	{
		//����
		//����RST֪ͨ�Է���������
		//TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort );
		return 0;
	}
	//����wRcv_wnd
	pTcbTcp->wRcv_wnd = pHdrTcp->wWinSize;

	//Ҫ����ACK�����ұ�֤ACK_NUM����ȷ��
	if( !(pHdrTcp->wHLenFlag & TCP_FLAG_ACK) )
	{
		//����
		return 0;
	}
	if( !Tcp_ChkAck(pTcbTcp, pHdrIP, pHdrTcp) )
	{
		//����RST֪ͨ�Է���������
		TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );
		return 0;
	}
	
	//���·���SEQ��Ϣ����������״̬��֪ͨaccept
	pTcbTcp->wFlag |= TCPF_LSYN;

	//�������ӣ���֪ͨaccept/select
	pTcbTcp->dwState = TCPS_ESTABLISHED;

	pTcbParent->wFlag |= TCPQ_LISTEN_YES;
	pTcbTcp->wAttr |= (TCPA_ESTABLISH | TCPA_RECV | TCPA_SEND);
	PulseEvent( pTcbParent->hEvtPoll );		//�����Ӻ�֪ͨ

	RETAILMSG(1,(TEXT(" \r\n TcpIn_SynRcvd: yes pTcbTcp_New=[0x%x]\r\n"), pTcbTcp));
	return 0;
}


// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ��������������ʱ��������յ���Ϣ
//1���ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
//2����FINʱ�����뱻���ر�״̬
//3����SYNʱ���������(please see Figure 10)�����ܱ���SEQ
// ����: 
// ********************************************************************
SKERR	TcpIn_Established( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//�ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp, lpbAutoFree) )
	{
		//�˳� ���� ����
		return 0;
	}
	//��FINʱ�����뱻���ر�״̬
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//����TCPS_CLOSE_WAIT
		pTcbTcp->dwState = TCPS_CLOSE_WAIT;
		pTcbTcp->wFlag |= TCPF_RFIN;		//�յ��Է���FIN
		pTcbTcp->wAttr |= TCPA_CONNRESET;
		pTcbTcp->dwRcv_nxt ++;
		//֪ͨ���� ������ʱ�ӵ�ACK��
		pTcbTcp->wFlag |= TCPF_NEEDACK;
	}
	//��SYNʱ���������(please see Figure 10)�����ܱ���SEQ
	if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
	{
		pTcbTcp->dwRcv_nxt ++;
		//֪ͨ���� ������ʱ�ӵ�ACK��
		pTcbTcp->wFlag |= TCPF_NEEDACK;

		//ע�⣡����
		//pTcbTcp->dwState = TCPS_CLOSED;
		//����RST֪ͨ�Է���������
		//TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort );
	}

	return 0;
}


// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// �����������ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2������RST֪ͨ�Է���������
// ����: 
// ********************************************************************
SKERR	TcpIn_Closed( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	//��RSTʱ��ֱ���˳�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//ֱ���˳���Socket---����Ҫ���� ������ʱ�ӵ�ACK��
		return 0;
	}
	//����RST֪ͨ�Է���������
	TcpOut_SendRst( NULL, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, pHdrTcp->dwSeqNum );

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ���������������ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2����ʱ���ٽ���FIN����ΪpTcbTcp->dwRcv_nxt�Ѿ��仯�������SEQ������
// ����: 
// ********************************************************************
SKERR	TcpIn_CloseWait( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//�ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp, lpbAutoFree) )
	{
		//����
		return 0;
	}

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ���������������ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2��ϣ������ȷ��ACK
// ����: 
// ********************************************************************
SKERR	TcpIn_LastACK( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//�ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp, lpbAutoFree) )
	{
		//����
		return 0;
	}
	//���� TCP HEADER---ͬʱ�رգ�ϣ����ACK
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		pTcbTcp->dwState = TCPS_CLOSED;
		pTcbTcp->wFlag |= TCPF_LFIN;	//�յ��Է���FIN's ACK
	}

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ���������������ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2����FINʱ������ͬʱ�ر�
//3����ֻ��ACKʱ�����������ر�
//4��֪ͨ���� ACK
// ����: 
// ********************************************************************
SKERR	TcpIn_FinWait1( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//��RSTʱ��ֱ���˳�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//ֱ���˳���Socket---����Ҫ���� ������ʱ�ӵ�ACK��
		return 0;
	}
	//�ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp,lpbAutoFree) )
	{
		//����
		return 0;
	}
	//���� TCP HEADER---ͬʱ�ر�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//����TCPS_CLOSING---ͬʱ�ر�״̬
		pTcbTcp->dwState = TCPS_CLOSING;
		//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
		pTcbTcp->wFlag |= TCPF_RFIN;		//�յ��Է���FIN
		pTcbTcp->dwRcv_nxt ++;
		//֪ͨ���� ������ʱ�ӵ�ACK��
		pTcbTcp->wFlag |= TCPF_NEEDACK;

		return 0;
	}
	//���� TCP HEADER---�����ر�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		pTcbTcp->dwState = TCPS_FIN_WAIT2;
		pTcbTcp->wFlag |= TCPF_LFIN;	//�յ��Է���FIN's ACK
	}

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ���������������ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2����FINʱ������ȴ��ر�״̬
//3��֪ͨ���� ACK
// ����: 
// ********************************************************************
SKERR	TcpIn_FinWait2( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//�ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp,lpbAutoFree) )
	{
		//����
		return 0;
	}
	//���� TCP HEADER---�����ر�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//����TCPS_TIME_WAIT---�ȴ��ر�״̬
		pTcbTcp->dwState = TCPS_TIME_WAIT;
		//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
		pTcbTcp->wFlag |= TCPF_RFIN;//�յ��Է���FIN
		pTcbTcp->dwRcv_nxt ++;
		//֪ͨ���� ������ʱ�ӵ�ACK��
		pTcbTcp->wFlag |= TCPF_NEEDACK;

		return 0;
	}

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ����������ͬʱ�ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2��ϣ������ȷ��ACK������ȴ��ر�״̬
// ����: 
// ********************************************************************
SKERR	TcpIn_Closing( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	//�ж� ��RSTʱֱ���˳���SEQ��ACK�Ƿ����������WinSize��Ϣ�����·�����Ϣ����������
	if( !Tcp_ChkSegment(pTcbTcp, pHdrIP, pHdrTcp,lpbAutoFree) )
	{
		//����
		return 0;
	}
	//���� TCP HEADER---ͬʱ�رգ�ϣ����ACK
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		pTcbTcp->dwState = TCPS_TIME_WAIT;
		pTcbTcp->wAttr |= TCPA_CLOSING;	//��ʾ�Ǵ�ͬʱ�رյ�״̬�л�����
		pTcbTcp->wFlag |= TCPF_LFIN;	//�յ��Է���FIN's ACK
	}

	return 0;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// �����������ȴ��ر�ʱ��������յ���Ϣ
//1����RSTʱ��ֱ���˳�
//2�������������κ����ݣ���2MSL�ȴ�ʱ���ڣ�����������رգ�����ACK
// ����: 
// ********************************************************************
SKERR	TcpIn_TimeWait( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp, BOOL * lpbAutoFree )
{
	//pTcbTcp->wAttr &= ~(TCPA_RECV | TCPA_SEND);
	//��RSTʱ��ֱ���˳�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//ֱ���˳���Socket---����Ҫ���� ������ʱ�ӵ�ACK��
		return 0;
	}
	//�ж϶Է��� SEQ��ACK �Ƿ����
	if( !Tcp_ChkNum(pTcbTcp, pHdrIP, pHdrTcp) )
	{
		//����
		return FALSE;
	}
	if( pHdrTcp->wHLenFlag & TCP_FLAG_FIN )
	{
		//�ط� Pure ACK��֪ͨ�Է�
		TcpOut_SendAck( pTcbTcp, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, TRUE );
	}

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
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbTcp-Ҫƥ���TCB
// ����ֵ��
//	
// ����������ƥ��pHdrIP��pHdrTcpָ����TCB
// ����: 
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
	//�����ȫѰ��
	/*
	if( pTcbTcp_Cache &&
		HANDLE_CHECK(pTcbTcp_Cache) && 
		(pTcbTcp_Cache->wFlag & TCPF_EXIT) == 0 &&
		pTcbTcp_Cache->wSrcPort==pHdrTcp->wDesPort )
	{
		//��ַ�Ƚ�---
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
		//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
		pTcbTcp_Find = LIST_CONTAINER( pUnit, TCP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 

		//�����ȫѰ��
		if( !HANDLE_CHECK(pTcbTcp_Find) )
		{
			ASSERT( 0 );
			break;
		}
		if( pTcbTcp_Find->wFlag & TCPF_EXIT )
		{
			continue;
		}
		//���ض˿ڱȽ�
		if( pTcbTcp_Find->wSrcPort==pHdrTcp->wDesPort )
		{
			//��ַ�Ƚ�---
			if( (pTcbTcp_Find->dwSrcIP==pHdrIP->dwDesIP) && 
				(pTcbTcp_Find->dwDesIP==pHdrIP->dwSrcIP) && 
				(pTcbTcp_Find->wDesPort==pHdrTcp->wSrcPort)
				)
			{
				*ppTcbTcp = pTcbTcp_Find;
				pTcbTcp_Cache = pTcbTcp_Find; //lilin add code
				break;
			}
			//����socket������--- [ Դ�� IP(�̶���ANY)��PORT(�̶�) ] --- [ Ŀ�Ķ� (ANY) ]
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
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ��������������pHdrTcp�����е�ѡ�������pTcbTcp
// ����: 
//ע�ͣ�
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
// ������
// ������
//	IN dwNum-��������ֵ
//	IN dwValBegin-�����ֵ��Χ����ʼλ��
//	IN dwValEnd-�����ֵ��Χ�Ľ���λ��
// ����ֵ��
//	
// �����������ж� dwNum�ǲ����� ��[dwValBegin, dwValEnd]֮�ڡ� ���� ��(dwValEnd, dwValBegin)֮�⡱
// ����: 
//ע�ͣ�
//���[90, 100]---[90, 100]����90, 91, 92, ... 100��������     89��101��ʧ��
//���[900, 10]---(10, 900)����900, 901, ..., 9, 10��������    899��11��ʧ��
// ********************************************************************
BOOL	Tcp_IsWithinSeq( DWORD dwNum, DWORD dwValBegin, DWORD dwValEnd )
{
	//��֤��
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
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// �����������жϽ��յ�ACK �Ƿ����������Ӧ������
// ����: 
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
			//��֤dwAckNumӦ���ڡ�ֻ�ܽ��յ�ACK�ķ�Χ���ڣ�
			//---���� ��[dwSnd_una, dwSnd_real]֮�ڡ� ���� ��(dwSnd_real, dwSnd_una)֮�⡱
			//if( !Tcp_IsWithinSeq( pHdrTcp->dwAckNum, pTcbTcp->dwSnd_una, pTcbTcp->dwSnd_max ) )
			if( !Tcp_IsWithinSeq( pHdrTcp->dwAckNum, pTcbTcp->dwSnd_una, pTcbTcp->dwSnd_real ) )
			{
				//����
				return FALSE;
			}
		}
		else
		{
			//��֤dwAckNumӦ���ڡ�ֻ�ܽ��յ�ACK�ķ�Χ���ڣ�
			//---���� ��[dwSnd_una+1, dwSnd_real+1]֮�ڡ� ���� ��(dwSnd_real+1, dwSnd_una+1)֮�⡱
			//�� SYN-RECVED  SYN-SENT  FIN-WAIT1(�����ر�)  CLOSING  LAST-ACK����
			//���������1�����ݶεĽ��գ����п������� ��[dwSnd_una+1, dwSnd_real+1]֮�ڡ� ���� ��(dwSnd_real+1, dwSnd_una+1)֮�⡱
			if( !Tcp_IsWithinSeq( pHdrTcp->dwAckNum, pTcbTcp->dwSnd_una+1, pTcbTcp->dwSnd_real+1 ) )
			//if( pHdrTcp->dwAckNum!=(pTcbTcp->dwSnd_nxt+1) )
			{
				//����
				return FALSE;
			}

			//���·�����Ϣ
			pTcbTcp->dwSnd_nxt ++;
			pTcbTcp->dwSnd_real ++;
			pTcbTcp->dwSnd_una ++;
			pTcbTcp->dwSnd_max ++;
		}
	}

	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// �����������ж϶Է���SEQ�Ƿ�����жϽ��յ�ACK �Ƿ����������Ӧ������
// ����: 
// ********************************************************************
BOOL	Tcp_ChkNum( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp )
{
	DWORD	dwRcv_max;

	//�ж϶Է���SEQ�Ƿ����---��Ӧ���ڡ�ֻ�ܽ��յ�SEQ���кŵķ�Χ���ڣ�
	//---��֤dwSeqNum�� ��[dwRcv_nxt, dwRcv_max]֮�ڡ� ���� ��(dwRcv_max, dwRcv_nxt)֮�⡱
	dwRcv_max = pTcbTcp->dwRcv_nxt + pTcbTcp->wSnd_wnd_Last;
	if( !Tcp_IsWithinSeq( pHdrTcp->dwSeqNum, pTcbTcp->dwRcv_nxt, dwRcv_max ) )
	{
		//����
		return FALSE;
	}

	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		if( !Tcp_ChkAck(pTcbTcp, pHdrIP, pHdrTcp) )
		{
			//�ط� Pure ACK��֪ͨ�Է�
			TcpOut_SendAck( pTcbTcp, pHdrIP->dwDesIP, pHdrTcp->wDesPort, pHdrIP->dwSrcIP, pHdrTcp->wSrcPort, FALSE );
			//pTcbTcp->wFlag |= TCPF_NEEDACK;
			//����
			return FALSE;
		}
	}

	//����wRcv_wnd
	pTcbTcp->wRcv_wnd = pHdrTcp->wWinSize;
	if( (pTcbTcp->wFlag & TCPF_NEEDPERSIST) && pTcbTcp->wRcv_wnd )
	{
		pTcbTcp->wFlag &= ~TCPF_NEEDPERSIST;
		SetEvent( pTcbTcp->hEvtSend );
	}

	return TRUE;
}
// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// �����������ж�RST��SEQ��ACK�������·�����Ϣ��������յ����ݣ�ͬʱ��������ش���RTOʱ��
// ����: 
// ********************************************************************
BOOL	Tcp_ChkSegment( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp,BOOL * lpbAutoFree )
{
	DWORD	dwTmp;

	//��RSTʱ��ֱ���˳�
	if( pHdrTcp->wHLenFlag & TCP_FLAG_RST )
	{
		//ֱ���˳���Socket---����Ҫ���� ������ʱ�ӵ�ACK��
		pTcbTcp->dwState = TCPS_CLOSED;
		//pTcbTcp->wFlag |= TCPF_EXIT;
		pTcbTcp->wFlag = 0;		//---���û��ڸ�socket�ϣ����Լ�����������
		pTcbTcp->wAttr |= TCPA_CONNRESET;

		//֪ͨ
		pTcbTcp->nErr = E_CONNRESET;//E_CONNREFUSED
		Tcp_ClearRxData( pTcbTcp );
		PulseEvent( pTcbTcp->hEvtPoll );
		return FALSE;
	}
	//�ж϶Է��� SEQ��ACK �Ƿ����
	if( !Tcp_ChkNum(pTcbTcp, pHdrIP, pHdrTcp) )
	{
		//����
		return FALSE;
	}
	//����ACK�����·�����Ϣ
	if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
	{
		//���ACK
		if( pHdrTcp->dwAckNum==pTcbTcp->dwSnd_una )
		{
			//---��������ش��ļ���
			//if( (pTcbTcp->dwSnd_una!=pTcbTcp->dwSnd_nxt) && !pTcbTcp->wLenData )
			if( (pTcbTcp->dwSnd_una!=pTcbTcp->dwSnd_nxt) )
			{
				pTcbTcp->wSnd_quick ++;
				if( pTcbTcp->wSnd_quick>=TCPQK_MAX )
				{
					//����п����ش�
					SetEvent( pTcbTcp->hEvtSend );
					pTcbTcp->wSnd_quick = 0;
				}
			}
		}
		else
		{
			pTcbTcp->wSnd_quick = 0;
			//���� RTT��RTO
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
			//���� �Է����յ��ֽ���
			dwTmp = pHdrTcp->dwAckNum - pTcbTcp->dwSnd_una;//�������ȷ���磺 (1 - MAXDWORD)==2
			//---���·��ͻ������Ϣ
			pTcbTcp->dwSnd_una = pHdrTcp->dwAckNum;
			//
			RWBuf_SetReadOffset( &pTcbTcp->stTxBufRW, dwTmp, FALSE );
			//
			LeaveCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );
		}
	}
	//�ж����кţ�������յ�����
	if( pHdrTcp->dwSeqNum==pTcbTcp->dwRcv_nxt )
	{
		//if( pTcbTcp->wLenData && !(pTcbTcp->wAttr & TCPA_NORECV) )
//		RETAILMSG( 1, ( "pHdrTcp->dwSeqNum=0x%x,%u,pTcbTcp->wLenData=%d.\r\n", pHdrTcp->dwSeqNum, pHdrTcp->dwSeqNum, pTcbTcp->wLenData ) );
		if( pTcbTcp->wLenData && (pTcbTcp->wAttr & TCPA_RECV) )
		{
			Tcp_SavRxData( pTcbTcp, lpbAutoFree );

		}
		//����������ʱ�ӵ�ACK��
		pTcbTcp->wFlag |= TCPF_NEEDACK;
	}
	else if( pTcbTcp->wLenData )
	{
		//����������1�� Pure ACK ֪ͨ�Է����յ�1����������ݰ���
		//����������ʱ�ӵ�ACK��
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
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ����������������յ����ݣ������½�����Ϣ
// ����: 
// ********************************************************************
#define SAVRX_NOSPACE 1
#define SAVRX_OK      0
static VOID	_Tcp_SavRxPacket( TCP_TCB* pTcbTcp,  TCPDATA_IN * pTCPDataIn )
{
	//CheckOutPtr( __FILE__, __LINE__ );
	EnterCriticalSection( &pTcbTcp->csRx );

	
	//List_InsertTail( &pTcbTcp->hCommitPacketDataIn, &pTCPDataIn->hListDataIn );
	AddPacketToList( pTcbTcp, &pTcbTcp->hCommitPacketDataIn, pTCPDataIn, TRUE );
	//���ڴ˴������ж� SWS���

	//�洢����
	//dwLenData = pTcbTcp->wLenData;
	//
	//���ûش��ACK���к�
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

	//�������
//	RETAILMSG( 1, ( "TCP_ReadData entry.\r\n" ) );

	dwLenAll = *pdwLenData;
	if( !pData || !dwLenAll )
	{
		//����Ҫ ��ȡ����
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

	//��ȡʣ��ռ�
	if( !RWBuf_GetCntLeft( &pTcbTcp->stRxBufRW, TRUE ) )
	{
		RETAILMSG(1,(TEXT("  Tcp_SavRxData: No space 111, come[%d], cnt[%d]\r\n"), pTcbTcp->wLenData, pTcbTcp->stRxBufRW.dwCntRW));
		return SAVRX_NOSPACE;
	}

	//���ڴ˴������ж� SWS���

	//�洢����
	dwLenData = pTcbTcp->wLenData;
	if( !RWBuf_WriteData( &pTcbTcp->stRxBufRW, pTcbTcp->pTcpData, &dwLenData, RWF_W_ENOUGH ) )
	{
		RETAILMSG(1,(TEXT("  Tcp_SavRxData: No space 222, come[%d], cnt[%d]\r\n"), pTcbTcp->wLenData, pTcbTcp->stRxBufRW.dwCntRW));
		return SAVRX_NOSPACE;
	}
	//RETAILMSG(1,(TEXT("  Tcp_SavRxData: come[%d], save[%d], cnt[%d]\r\n"), pTcbTcp->wLenData, dwLenData, pTcbTcp->stRxBufRW.dwCntRW));

	//
	//���ûش��ACK���к�
	//
	pTcbTcp->dwRcv_nxt += dwLenData;
//	PulseEvent( pTcbTcp->hEvtPoll );

	return SAVRX_OK;
}
*/
// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
//	IN pHdrTcp-���յ����ݰ�TCP Header
// ����ֵ��
//	
// ����������������յ����ݣ������½�����Ϣ
// ����: 
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
	*lpbAutoFree = FALSE;// �ɶ��д������ͷ�
	
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
	*lpbAutoFree = FALSE;	//// �ɶ��д������ͷ�
	
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

				//�ж����кţ�������յ�����
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
					//����������ʱ�ӵ�ACK��
//					pTcbTcp->wFlag |= TCPF_NEEDACK;
				}
				else if(pHdrTcp->dwSeqNum<pTcbTcp->dwRcv_nxt)
				{	//�ð�Ӧ�ö���
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
			{	//������
				break;
			}
		}
		//insert it
		//List_InsertTail( pUnit, &pTcbTcp->pCurDataIn->hListDataIn );
	}
	LeaveCriticalSection( &pTcbTcp->csRxUnCommit );

	//CheckOutPtr( __FILE__, __LINE__ );

}
