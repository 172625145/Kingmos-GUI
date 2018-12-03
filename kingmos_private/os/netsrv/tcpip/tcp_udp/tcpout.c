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


/***************  ȫ���� ���壬 ���� *****************/


static	DWORD	WINAPI	TcpOut_ThrdSend( LPVOID lpParam );
static	DWORD	WINAPI	TcpOut_ThrdPersist( LPVOID lpParam );

//�ڲ���������
static	SKERR	TcpOut_SendPk( TCP_TCB* pTcbTcp, IP_HDR* pHdrIP, TCP_HDR* pHdrTcp );
static	WORD	TcpOut_GetSndData( TCP_TCB* pTcbTcp, FRAGSETA* pFragSetA );
static	void	TcpOut_FillOpt( TCP_TCB* pTcbTcp, OUT LPBYTE pData );


/******************************************************/


// ********************************************************************
// ������
// ������
//	IN/OUT pTcbTcp-Ҫ������TCB
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �������������������߳�
// ����: 
// ********************************************************************
BOOL	TcpOut_StartSend( TCP_TCB* pTcbTcp )
{
	DWORD	dwThrdID;

	//���õ�һ�γ�ʱ�����������߳�
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
// ������
// ������
//	IN lpGlobalTcp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ���������������߳�
// ����: 
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
// ������
// ������
//	IN lpParam-Ҫ������TCB
// ����ֵ��
//	
// ���������������߳�
//   ����---��1�η��͡���ʱ�ط��������ط����Ӵ�ACK
//   ����---���͸��׶ε�Э���־ �� �������ݰ�
// ����: 
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
		//�ȴ������¼�
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

		//��ʼ��---��1��
		if( fNeedInit )
		{
			memset( bBuffer, 0, sizeof(bBuffer) );
			//��� IP��ַ��
			pHdrIP->bVerHLen = IPV4_VHDEF;
			pHdrIP->bProType = IP_PROTYPE_TCP;
			pHdrIP->dwSrcIP = pTcbTcp->dwSrcIP;
			pHdrIP->dwDesIP = pTcbTcp->dwDesIP;
			pHdrTcp->wSrcPort = pTcbTcp->wSrcPort;
			pHdrTcp->wDesPort = pTcbTcp->wDesPort;

			fNeedInit = FALSE;
		}

		//���� Э���־������
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
		//ע�⣺ xyg_2004-5-14
		if( (pTcbTcp->dwState==TCPS_ESTABLISHED) && (pTcbTcp->dwSnd_una==pTcbTcp->dwSnd_max) )
		{
			dwTmout = INFINITE;
		}
		else
		{
			dwTmout = pTcbTcp->dwRTO;
		}
	}

	//�˳����߳�
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
// ������
// ������
//	IN lpParam-TCPIP��ȫ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �����������߳�---��⡰��ʱ��û�б����ӳɹ��ı�������TCB������ɾ����
// ����: 
// ********************************************************************
//��ֶ�ʱ����ͨ��Է�����ȡ�Է���WndSize
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
		//�ȴ�
		WaitForSingleObject( lpGlobalTcp->hEvtPersist, dwTmout );
		if( HANDLE_F_FAIL(lpGlobalTcp, TCPIPF_EXIT) )
		{
			break;
		}
		dwTmout = INFINITE;
		//��ѯ���е� TCB����ɾ��ָ����
		pUnitHeader = &lpGlobalTcp->hListTcb;
		EnterCriticalSection( &lpGlobalTcp->csListTcb );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
			pTcbTcp = LIST_CONTAINER( pUnit, TCP_TCB, hListTcb );
			pUnit = pUnit->pNext;
			
			//�����ȫѰ��
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
// ������
// ������
//	IN pTcbTcp-�ṩ��wWinSize��dwRcv_nxt��dwSnd_nxt��Ҫ���͵�����
//	IN pHdrIP-�ṩ��IP��Ϣ
//	IN pHdrTcp-�ṩ��TCP��Ϣ
// ����ֵ��
//	
// ���������������߳�
//   ����---��1�η��͡���ʱ�ط��������ط����Ӵ�ACK
//   ����---���͸��׶ε�Э���־ �� �������ݰ�
// ����: 
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
	//��� wHLenFlag��Ϣ
	if( pTcbTcp->dwState!=TCPS_ESTABLISHED )
	{
		//
		//���� ������״̬ʱ�� Э�������
		//

		//�ж�ֹͣ���͵�״̬---FIN_WAIT2 �� ��ͬʱ�رն������TIME_WAIT
		if( pTcbTcp->dwState==TCPS_FIN_WAIT2 )
		{
			return E_SUCCESS;
		}
		if( (pTcbTcp->dwState==TCPS_TIME_WAIT) && (pTcbTcp->wAttr & TCPA_CLOSING) )
		{
			return E_SUCCESS;
		}

		//�ж��Ƿ���SYN
		if( !(pTcbTcp->wFlag & TCPF_LSYN) )//�����û�û���յ���ȷ��ACK
		{
			if( (pTcbTcp->dwState==TCPS_SYN_RECVD) || (pTcbTcp->dwState==TCPS_SYN_SENT) )
			{
				fSendSyn = TRUE;
			}
		}
		//���㡢���---wHLenFlag��Ϣ
		if( fSendSyn )
		{
			//RETAILMSG(1,(TEXT("  TcpOut_SendPk: Set SYN\r\n")));
			Tcp_CalISN( pTcbTcp );

			//���---wHLenFlag��Ϣ
			wHLen = (sizeof(TCP_HDR)+40);
			pHdrTcp->wHLenFlag = ( TCP_HLEN_MASK | TCP_FLAG_SYN );
		}
		else
		{
			//���---wHLenFlag��Ϣ
			wHLen = sizeof(TCP_HDR);
			pHdrTcp->wHLenFlag = TCP_HLEN_FIX;
		}

		//�ж��Ƿ���FIN
		if( pTcbTcp->dwState==TCPS_FIN_WAIT1 || pTcbTcp->dwState==TCPS_LAST_ACK )
		{
			pHdrTcp->wHLenFlag |= TCP_FLAG_FIN;
			
		}

		//�ж��Ƿ��͡�����ʱ�ӵ�ACK��
		if( pTcbTcp->dwState!=TCPS_SYN_SENT )
		{
			pHdrTcp->wHLenFlag |= TCP_FLAG_ACK;
			pTcbTcp->wFlag &= ~TCPF_NEEDACK;
		}

		//�ж��Ƿ�������
		if( pHdrTcp->wHLenFlag & TCP_FLAG_SYN )
		{
			fSendData = FALSE;

			//��ӷ���ѡ��
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
		//�鿴�Ƿ������ݣ����û�����ݣ����˳�
		//�ж������Ƿ������
		//if( pTcbTcp->dwSnd_nxt==pTcbTcp->dwSnd_max )
		if( pTcbTcp->dwSnd_una==pTcbTcp->dwSnd_max )
		{
			return 0;
		}
		//
		//��� ����״̬ʱ�� Э�������---���͡�����ʱ�ӵ�ACK��
		//
		wHLen = sizeof(TCP_HDR);
		pHdrTcp->wHLenFlag = ( TCP_HLEN_FIX | TCP_FLAG_ACK );
		pTcbTcp->wFlag &= ~TCPF_NEEDACK;
	}

	//
	//ѭ����������
	//
	//RETAILMSG(1,(TEXT("  TcpOut_SendPk: ok cnt=[%d]\r\n"), pTcbTcp->stTxBufRW.dwCntRW));

	//��ʼ��
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrTcp;
	pFragSetA->pFrag[0].nLen = wHLen;

	wBytesTx = 0;
	pTcbTcp->wFlag |= TCPF_SENDING;
	pTcbTcp->dwTickRTT = GetTickCount();
	//�ָ������͵����---�п�������Ϊ��ʱ�ط������
	pTcbTcp->dwSnd_nxt = pTcbTcp->dwSnd_una;

	//
	while( (pTcbTcp->dwState!=TCPS_CLOSED) && !(pTcbTcp->wFlag & TCPF_EXIT) )
	{
		//�Ӵ�ACK
		if( pHdrTcp->wHLenFlag & TCP_FLAG_ACK )
		{
			pTcbTcp->wFlag &= ~TCPF_NEEDACK;
		}
		//������Ϣ
		wTmp = Tcp_CalWndSize( pTcbTcp );

		//���ͻ���---
		//1. ��Tcp_TrIn����-------�޸� ���ͻ���� ���к���Ϣ���ͻ����С��Readλ��
		//2. ��TcpSk_send---�޸� ���ͻ���� �����С��Writeλ�ã����ܻ��޸����ݣ����ʣ��ռ䲻�㹻�Ļ���
		EnterCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );

		//��� wWinSize��Ϣ��ACK��Ϣ��SEQ��Ϣ��UP��Ϣ
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
		//���沿����Ϣ 
		pTcbTcp->dwAck_Last = pTcbTcp->dwRcv_nxt;
		pTcbTcp->wSnd_wnd_Last = wTmp;

		//��ȡ �����ͻ���������ݡ��������·�����Ϣ
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
		//����У����
		pHdrTcp->wTotalCRC = 0;
		pHdrTcp->wTotalCRC = Tcp_CRC2( pHdrIP->dwSrcIP, pHdrIP->dwDesIP, IP_PROTYPE_TCP, pFragSetA );
		//�������ݰ�
		pHdrIP->wLenTotal = sizeof(IP_HDR)+wHLen+wBytesTx;

		//Tcp_DbgOut( pTcbTcp->dwState, pHdrTcp, (WORD)(wHLen+wBytesTx), FALSE );
		dwRet = IP_NetOut( NULL, pHdrIP, pFragSetA );
		//�ж������Ƿ������
		if( pTcbTcp->dwSnd_nxt==pTcbTcp->dwSnd_max )
		{
			pTcbTcp->wFlag &= ~TCPF_SENDING;
		}
		LeaveCriticalSection( &pTcbTcp->stTxBufRW.csBufRW );
		//����IP_NetOut�ķ��ͽ��
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

		//�ж������Ƿ������
		if( pTcbTcp->dwSnd_nxt==pTcbTcp->dwSnd_max )
		{
			break;
		}
	}
	pTcbTcp->wFlag &= ~TCPF_SENDING;

	return E_SUCCESS;
}

//��ӷ���ѡ��
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

	//��ȡ���Ѿ����͵��Է�û��ACK�������ݵĳ���
	if( pTcbTcp->dwSnd_nxt >= pTcbTcp->dwSnd_una )
	{
		wBytesSent = (WORD)(pTcbTcp->dwSnd_nxt - pTcbTcp->dwSnd_una);
	}
	else
	{
		wBytesSent = (WORD)(pTcbTcp->dwSnd_nxt + (MAXDWORD - pTcbTcp->dwSnd_una) );
	}
	//ȷ��Ҫ���͵ĳ���---���ݡ����ͻ����ʣ�����ݡ��͡��Է�MSS���ȡ���
	if( pTcbTcp->stTxBufRW.dwCntRW<MAXWORD )	//���ͻ����ʣ�೤��
	{
		wBytesTx = (WORD)(pTcbTcp->stTxBufRW.dwCntRW - wBytesSent);
	}
	else
	{
		wBytesTx = MAXWORD - wBytesSent;
	}
	if( wBytesTx > pTcbTcp->wRcv_MSS )	//���ܳ����Է���MSS
	{
		wBytesTx = pTcbTcp->wRcv_MSS;
	}
	if( wBytesTx > pTcbTcp->wSnd_MSS )	//���ܳ����Լ���MSS
	{
		wBytesTx = pTcbTcp->wSnd_MSS;
	}
	//��鷢�ͳ���---�� �Է�Winsize��
	if( wBytesTx )
	{
		//���Է�Winsize
		if( pTcbTcp->wRcv_wnd==0 )
		{
			//���ü�ֶ�ʱ����ͨ��Է�
			pTcbTcp->wFlag |= TCPF_NEEDPERSIST;
			//SetEvent( pTcbTcp->lpGlobalTcp->hEvtPersist );
			pFragSetA->nCnt = 1;
			return 0;
		}
		else
		{
			//�ж϶Է�WinSize��ʣ��Ŀռ��ڣ�ֻ�ܷ��͵ĳ���
			if( pTcbTcp->wRcv_wnd > wBytesSent )
			{
				wBytesTxOnly = pTcbTcp->wRcv_wnd - wBytesSent;
				
				//�ȼ�� SWS���
				
				//
				if( wBytesTx > wBytesTxOnly )
				{
					wBytesTx = wBytesTxOnly;
				}
			}
			else
			{
				//���ü�ֶ�ʱ����ͨ��Է�
				pTcbTcp->wFlag |= TCPF_NEEDPERSIST;
				//SetEvent( pTcbTcp->lpGlobalTcp->hEvtPersist );
				pFragSetA->nCnt = 1;
				return 0;
			}
		}
	}
	//׼�����ݷ���
	if( wBytesTx )
	{
		//��ȡƫ�ƺ� ���͵�λ�� �� �����еĳ���
		RWBuf_GetReadBuf( &pTcbTcp->stTxBufRW, wBytesSent, &pFragSetA->pFrag[1].pData, &nLen1 );
		//�ж�ʣ������
		if( nLen1 >= (DWORD)wBytesTx )
		{
			pFragSetA->nCnt = 2;
			//---�����1�����㹻�������ݷ���
			pFragSetA->pFrag[1].nLen = wBytesTx;

			//pFragSetA->pFrag[2].pData = NULL;
		}
		else
		{
			pFragSetA->nCnt = 3;
			//---����Ҫ���͵�2�ε�����
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

	//���·�����Ϣ
	if( wBytesTx )
	{
		//���µ�ǰ����λ��
		pTcbTcp->dwSnd_nxt += wBytesTx;	

		//����ʵ�������λ��
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
// ������
// ������
//	IN pHdrIP-�ṩ��IP��ַ
//	IN pHdrTcp-�ṩ��PORT
// ����ֵ��
//	
// ����������
// ����: 
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
	//��� IP��ַ�� 
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_TCP;
	pHdrIP->dwSrcIP = dwSrcIP;
	pHdrIP->dwDesIP = dwDesIP;
	pHdrTcp->wSrcPort = wSrcPort;
	pHdrTcp->wDesPort = wDesPort;
	//��� wHLenFlag��Ϣ
	pHdrTcp->wHLenFlag = ( TCP_HLEN_FIX | TCP_FLAG_RST );
	//��� wWinSize��Ϣ��ACK��Ϣ��SEQ��Ϣ��UP��Ϣ
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
	//���沿����Ϣ 

	//׼�����ݰ�
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrTcp;
	pFragSetA->pFrag[0].nLen = sizeof(TCP_HDR);
	//����У����
	pHdrTcp->wTotalCRC = Tcp_CRC( dwSrcIP, dwDesIP, IP_PROTYPE_TCP, (LPBYTE)pHdrTcp, sizeof(TCP_HDR) );
	//�������ݰ�
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
// ������
// ������
//	IN pTcbTcp-�ṩ��wWinSize��dwRcv_nxt��dwSnd_nxt
//	IN pHdrIP-�ṩ��IP��ַ
//	IN pHdrTcp-�ṩ��PORT
//	IN wHLenFlag-�ṩ��Flag
//	IN fWin_Zero-�ṩ���Ƿ�wWinSize���ó�0
// ����ֵ��
//	
// ����������
//----ֱ�ӿ��ٷ��� Pure RST�������������£�
//1. Tcp_TrIn �У�û���ҵ���Ӧ��TCB
//2. TcpIn_Listen �У��ա���ACK ���� û��SYN��ʱ
//3. TcpIn_Listen �У���������������ʱ
//4. TcpIn_SynSent �У��ա���������ACK������ȷ�ġ�ʱ
//5. TcpIn_SynRcvd �У��ա��������SEQ��ʱ
//6. TcpIn_SynRcvd �У��ա��������ACK��ʱ
//7. TcpIn_Closed�У��������κ���Ϣ����RST��

//----ֱ�ӿ��ٷ��� Pure ACK�������������£�
//1. 
//2. 
//3. 
//4. 
//5. 
//6. 

// ����: 
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
	//��� IP��ַ��
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_TCP;
	pHdrIP->dwSrcIP = dwSrcIP;
	pHdrIP->dwDesIP = dwDesIP;
	pHdrTcp->wSrcPort = wSrcPort;
	pHdrTcp->wDesPort = wDesPort;
	//��� wHLenFlag��Ϣ
	pHdrTcp->wHLenFlag = ( TCP_HLEN_FIX | TCP_FLAG_ACK );
	//��� wWinSize��Ϣ��ACK��Ϣ��SEQ��Ϣ��UP��Ϣ
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
	//���沿����Ϣ 
	pTcbTcp->dwAck_Last = pTcbTcp->dwRcv_nxt;
	pTcbTcp->wSnd_wnd_Last = ntohs( pHdrTcp->wWinSize );

	//׼�����ݰ�
	pFragSetA->nCnt = 1;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrTcp;
	pFragSetA->pFrag[0].nLen = sizeof(TCP_HDR);
	//����У����
	pHdrTcp->wTotalCRC = Tcp_CRC( dwSrcIP, dwDesIP, IP_PROTYPE_TCP, (LPBYTE)pHdrTcp, sizeof(TCP_HDR) );
	//�������ݰ�
	pHdrIP->wLenTotal = sizeof(IP_HDR)+sizeof(TCP_HDR);

	//Tcp_DbgOut( pTcbTcp->dwState, pHdrTcp, sizeof(TCP_HDR), FALSE );
	return IP_NetOut( NULL, pHdrIP, pFragSetA );
}
