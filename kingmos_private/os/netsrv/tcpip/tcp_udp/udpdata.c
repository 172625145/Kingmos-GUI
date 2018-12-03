/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����UDP ���ݴ���
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
//#include "netif_tcp.h"
#include "netif_skinst.h"
//"local"
#include "tcp_udp.h"
#include "udp_def.h"


/***************  ȫ���� ���壬 ���� *****************/

//
static	BOOL	Udp_MatchTcb( OUT UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp, IP_HDR* pHdrIP, UDP_HDR* pHdrUdp );
//static	void	UdpIn_SaveData( UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort, LPBYTE pUdpData, WORD wLenUdpData );
static BOOL UdpIn_SaveData(
								UDP_TCB* pTcbUdp,
								 DWORD dwSrcIP,
								 WORD wSrcPort,
								 IPDATA_IN * pDataIn, 
								 LPBYTE pUdpData, 
								 WORD wLenUdpData);

static	DWORD	WINAPI	UdpIn_ThrdRxNode( LPVOID lpParam );

/******************************************************/



// ********************************************************************
// ������
// ������
//	IN pInfoSkt-socketʵ����Ϣ
//	IN pMsgRxTx-ָ���������ݵ���Ϣ
// ����ֵ��
//	
// ������������������
// ����: 
// ********************************************************************
#define DEBUG_Udp_Send 0
SKERR	Udp_Send( UDP_TCB* pTcbUdp, RXTXMSG* pMsgRxTx )
{
	BYTE			bBuffer[sizeof(IP_HDR) + sizeof(UDP_HDR)];
	IP_HDR*			pHdrIP = (IP_HDR*)bBuffer;
	UDP_HDR*		pHdrUdp = (UDP_HDR*)(bBuffer + sizeof(IP_HDR));
	SOCKADDR_IN*	pAddrIn;
	LPBYTE			pOffset;
	DWORD			dwLenAll;

	BYTE			pBufFragSet[sizeof(FRAGSETA)+sizeof(FRAGMENT)];
	FRAGSETA*		pFragSetA = (FRAGSETA*)pBufFragSet;
	DWORD			dwRet;

	WORD			wLenData;
	WORD			wLenTotal;

	//
	RETAILMSG( 1, ( "Call Udp_Send entry,0x%x\r\n", pMsgRxTx ) );

	memset( bBuffer, 0, sizeof(bBuffer) );
	//��� 
	pHdrIP->bVerHLen = IPV4_VHDEF;
	pHdrIP->bProType = IP_PROTYPE_UDP;
	//��� Ŀ����Ϣ
	if( pMsgRxTx->lpPeer )
	{
		if( pMsgRxTx->dwLenPeer<sizeof(SOCKADDR_IN) )
		{
			//��������
			WARNMSG( DEBUG_Udp_Send, ( "Udp_Send error: pMsgRxTx->dwLenPeer<sizeof(SOCKADDR_IN).\r\n" ) );
			return E_FAULT;
		}
		pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
		if( (pAddrIn->sin_family!=AF_INET) || 
			(pAddrIn->sin_addr.S_un.S_addr==INADDR_ANY) ||
			(pAddrIn->sin_port==0) )
		{
			//Զ�̵�ַ ��Ч
			WARNMSG( DEBUG_Udp_Send, ( "Udp_Send warn: E_ADDRNOTAVAIL.\r\n" ) );
			return E_ADDRNOTAVAIL;
		}
		pHdrIP->dwDesIP = pAddrIn->sin_addr.S_un.S_addr;
		pHdrUdp->wDesPort = pAddrIn->sin_port;
	}
	else
	{
		if( (pTcbUdp->dwDesIP==INADDR_ANY) || (pTcbUdp->wDesPort==0) )
		{
			//��Ҫ Զ�̵�ַ
			WARNMSG( DEBUG_Udp_Send, ( "Udp_Send warn: E_DESTADDRREQ.\r\n" ) );
			return E_DESTADDRREQ;
		}
		pHdrIP->dwDesIP = pTcbUdp->dwDesIP;
		pHdrUdp->wDesPort = pTcbUdp->wDesPort;
	}
	//��� Դ��Ϣ
	if( !(pTcbUdp->dwOption & TCPIPO_IPINTF) )
	{
		if( pTcbUdp->dwSrcIP==INADDR_ANY )
		{
			if( pTcbUdp->dwSrcIP_Snd==INADDR_ANY )
			{
				//��ȡ�ʵ��ı���IP��ַ
				if( !IP_GetBestAddr( &pTcbUdp->dwSrcIP_Snd, pHdrIP->dwDesIP, TRUE ) )
				{
					WARNMSG( DEBUG_Udp_Send, ( "Udp_Send warn: IP_GetBestAddr return false.\r\n" ) );
					return E_NOTCONN;
				}
			}
			pHdrIP->dwSrcIP = pTcbUdp->dwSrcIP_Snd;
		}
		else
		{
			pHdrIP->dwSrcIP = pTcbUdp->dwSrcIP;
		}
	}
	pHdrUdp->wSrcPort = pTcbUdp->wSrcPort;

	//
	//׼�����ݰ�
	pOffset = pMsgRxTx->pData;
	dwLenAll = pMsgRxTx->dwLenData;
	pFragSetA->nCnt = 2;
	pFragSetA->pFrag[0].pData = (LPBYTE)pHdrUdp;
	pFragSetA->pFrag[0].nLen = sizeof(UDP_HDR);
	do
	{
		//���� ���ͳ���
		if( dwLenAll>IPF_MAXFRAG )
		{
			wLenData = IPF_MAXFRAG;
		}
		else
		{
			wLenData = (WORD)dwLenAll;
		}
		pFragSetA->pFrag[1].pData = pOffset;
		pFragSetA->pFrag[1].nLen = wLenData;

		//���� UDP HEADER��Ϣ
		wLenTotal = wLenData+sizeof(UDP_HDR);
		pHdrUdp->wLenTotal = htons_m(wLenTotal);
		pHdrUdp->wTotalCRC = Tcp_CRC2( pHdrIP->dwSrcIP, pHdrIP->dwDesIP, pHdrIP->bProType, pFragSetA );

		//Udp_DbgOut( 0, pHdrUdp, wLenData, FALSE );
		//�������ݰ�
		pHdrIP->wLenTotal = sizeof(IP_HDR)+wLenTotal;
		if( pTcbUdp->dwOption & TCPIPO_IPINTF )
		{
			DEBUGMSG( DEBUG_Udp_Send, ( "Udp_Send : use TCPIPO_IPINTF.\r\n" ) );
			dwRet = IP_NetOut( pTcbUdp->hNetIntf, pHdrIP, pFragSetA );
		}
		else
		{
			DEBUGMSG( DEBUG_Udp_Send, ( "Udp_Send : send data.\r\n" ) );
			dwRet = IP_NetOut( NULL, pHdrIP, pFragSetA );
		}
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
				break;
			}

			return E_SND_UNKNOWN;
		}
		//���·�����Ϣ
		dwLenAll -= wLenData;
		pOffset += wLenData;

	}while( dwLenAll );

	return E_SUCCESS;
}


// ********************************************************************
// ������
// ������
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ���������������̼߳�⡰��ʱ��û�б����ӳɹ��ı�������TCB��
// ����: 
// ********************************************************************
BOOL	UdpIn_StartRxNode( UDP_GLOBAL* lpGlobalUdp )
{
	DWORD	dwThrdID;

	if( !lpGlobalUdp->hThrdRxNode )
	{
		lpGlobalUdp->hThrdRxNode = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)UdpIn_ThrdRxNode, (LPVOID)lpGlobalUdp, 0, &dwThrdID );
		if( !lpGlobalUdp->hThrdRxNode )
		{
			return FALSE;
		}
	}
	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN lpParam-UDPIP��ȫ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �����������߳�---��⡰��ʱ��û�н������ݵ�RxNode�������û�����ݵĻ�������ɾ����
// ����: 
// ********************************************************************
DWORD	WINAPI	UdpIn_ThrdRxNode( LPVOID lpParam )
{
	UDP_GLOBAL*		lpGlobalUdp = (UDP_GLOBAL*)lpParam;
	UDP_TCB*		pTcbUdp;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	while( HANDLE_F_OK(lpGlobalUdp, TCPIPF_EXIT) )
	{
		//�ȴ�
		WaitForSingleObject( lpGlobalUdp->hEvtRxNode, UDPTM_WAITRXNODE );
		if( HANDLE_F_FAIL(lpGlobalUdp, TCPIPF_EXIT) )
		{
			break;
		}
		//��ѯ���е� RxNode����ɾ��ָ����
		pUnitHeader = &lpGlobalUdp->hListTcb;
		EnterCriticalSection( &lpGlobalUdp->csListTcb );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
			pTcbUdp = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
			pUnit = pUnit->pNext;    // advance to next 

			//�����ȫѰ��
			if( !HANDLE_CHECK(pTcbUdp) )
			{
				break;
			}
			if( pTcbUdp->wFlag & UDPF_EXIT )
			{
				continue;
			}
			//
			Udp_CheckRxNode( pTcbUdp, FALSE );
		}
		LeaveCriticalSection( &lpGlobalUdp->csListTcb );
	}

	return 0;
}


//DWORD	Udp_TrIn( HANDLE hNetIntf, IP_HDR* pHdrIP, LPBYTE pBufData, WORD wLenData )
#define DEBUG_Udp_TrIn 0
DWORD	Udp_TrIn( HANDLE hNetIntf, IPDATA_IN* pDataIn, LPBYTE pBufData, WORD wLenData )
{
	UDP_TCB*		pTcbUdp;
	UDP_HDR*		pHdrUdp;
	BOOL			fSavData;

	LPBYTE			pUdpData;
	WORD			wLenUdpData;
	WORD			wLenTotal;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	IP_HDR* pHdrIP = &pDataIn->HdrIP;
	DWORD dwRetv = 0;

	DEBUGMSG( DEBUG_Udp_TrIn, ( "Udp_TrIn entry.\r\n", wLenData ) );
	//�����ȫ���
	if( HANDLE_F_FAIL(g_lpGlobalUdp, TCPIPF_EXIT) )
	{
		goto _return;
	}
	//���---BUFFER����
	if( wLenData<sizeof(UDP_HDR) )
	{
		goto _return;
	}
	//���---У����
	if( Tcp_CRC(pHdrIP->dwSrcIP, pHdrIP->dwDesIP, pHdrIP->bProType, pBufData, wLenData) )
	{
		goto _return;
	}
	//���---UDP_HDR���ȳ�Ա
	pHdrUdp = (UDP_HDR*)(pBufData);
#ifndef	BIG_END
	wLenTotal = ntohs(pHdrUdp->wLenTotal);
#else
	wLenTotal = pHdrUdp->wLenTotal;
#endif
	if( wLenTotal>wLenData )
	{
		goto _return;
	}
	//���---�ɹ�
	pUdpData = pBufData + sizeof(UDP_HDR);
	wLenUdpData = wLenTotal-sizeof(UDP_HDR);

	//Ѱ�� Ŀ��TCB����Ͷ������
	pUnitHeader = &g_lpGlobalUdp->hListTcb;
	EnterCriticalSection( &g_lpGlobalUdp->csListTcb );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(g_lpGlobalUdp->wFlag & TCPIPF_EXIT) )
	{
		//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
		pTcbUdp = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 

		//�����ȫѰ��
		if( !HANDLE_CHECK(pTcbUdp) )
		{
			break;
		}
		if( pTcbUdp->wFlag & UDPF_EXIT )
		{
			continue;
		}
		//���ض˿ڱȽ�
		if( pTcbUdp->wSrcPort==pHdrUdp->wDesPort )
		{
			if( pTcbUdp->dwOption & TCPIPO_IPINTF )
			{
				//�Ƿ�ֱ�Ӱ󶨵���IP�ӿ�
				if( pTcbUdp->hNetIntf==hNetIntf )
				{
					//Ͷ�ݣ���������
					fSavData = TRUE;
				}
				else
				{
					fSavData = FALSE;
				}
			}
			else
			{
				//��ַƥ��
				if( (pTcbUdp->dwSrcIP==pHdrIP->dwDesIP) ||			//Ψһ������
					(pTcbUdp->dwSrcIP==INADDR_ANY) ||				//Ψһ������
					( INADDR_TEST_BC(pHdrIP->dwDesIP) &&			//����ǹ㲥����
					  ( (pTcbUdp->dwOption & TCPIPO_BROADCAST) ||		//TCB�����շ��㲥
					    (pTcbUdp->dwDesIP==pHdrIP->dwDesIP)				//TCB�Ѿ�connect��1���㲥��ַ
					  )
					)
				  )
				{
					//Ͷ�ݣ���������
					fSavData = TRUE;
				}
				else
				{
					fSavData = FALSE;
				}
			}
			//Ͷ�ݣ���������
			if( fSavData )
			{
				RETAILMSG( 1, ( "Call UdpIn_SaveData=%d.\r\n", wLenUdpData ) );
				//if( UdpIn_SaveData( pTcbUdp, pHdrIP->dwSrcIP, pHdrUdp->wSrcPort, pUdpData, wLenUdpData ) )
				if( UdpIn_SaveData( pTcbUdp, pHdrIP->dwSrcIP, pHdrUdp->wSrcPort, pDataIn, pUdpData, wLenUdpData ) )
				{
					LeaveCriticalSection( &g_lpGlobalUdp->csListTcb );
					return dwRetv;
				}
			}
		}
	}
	//
	LeaveCriticalSection( &g_lpGlobalUdp->csListTcb );

	//
	if( !fSavData )
	{
		//����ICMP������---Ŀ�Ķ˿ڲ��ɵ���
		Icmp_TrOut_Unreach( ICMP_CODE_DESUR_PORT, pHdrIP->dwSrcIP, pHdrIP->dwDesIP, (LPBYTE)pHdrIP, sizeof(IP_HDR)+sizeof(UDP_HDR) );
	}

	//Udp_DbgIn( 0, pHdrUdp, wLenUdpData, FALSE );
_return:
	IPDataIn_Free(pDataIn);

	return dwRetv;
}


void	UdpIn_IcmpNotify( BYTE bType, BYTE bCode, IP_HDR* pHdrIP_icmp, WORD wLenData )
{
	UDP_HDR*		pHdrUdp;
	UDP_TCB*		pTcbUdp;
	WORD			wLenHdr;

	//�����ȫ���
	if( HANDLE_F_FAIL(g_lpGlobalUdp, TCPIPF_EXIT) )
	{
		return ;
	}
	//��֤ wLenData>=(sizeof(IP_HDR)+sizeof(UDP_HDR))
	wLenHdr = ((pHdrIP_icmp->bVerHLen & IPL_MASK)<<2);
	if( wLenData<(wLenHdr + sizeof(UDP_HDR)) )
	{
		return ;
	}
	//Ѱ�Ҹö˿�
	pHdrUdp = (UDP_HDR*)((LPBYTE)pHdrIP_icmp + wLenHdr);
	if( !Udp_MatchTcb( &pTcbUdp, g_lpGlobalUdp, pHdrIP_icmp, pHdrUdp ) )
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
			pTcbUdp->nErr = E_NETUNREACH;
			break;
		case ICMP_CODE_DESUR_HOST	:
		case ICMP_CODE_DESUR_PROTOCOL:
			pTcbUdp->nErr = E_HOSTUNREACH;
			break;
		case ICMP_CODE_DESUR_PORT	:
			pTcbUdp->nErr = E_CONNRESET;
			break;
		case ICMP_CODE_DESUR_FRAGMENT:
		case ICMP_CODE_DESUR_ROUTE	:
		case ICMP_CODE_DESUR_UNNET	:
		case ICMP_CODE_DESUR_UNHOST	:
		default :
			break;
		}
		PulseEvent( pTcbUdp->hEvtPoll );

	case ICMP_TYPE_SRC_QUENCH:
		break;

	default :
		break;
	}

}




/////////////////////////////////////////////////////////////////////
//
//�ڲ���������
//
/////////////////////////////////////////////////////////////////////

#define DEBUG_UdpIn_SaveData 0
//void BOOL UdpIn_SaveData( UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort, LPBYTE pUdpData, WORD wLenUdpData )
static BOOL UdpIn_SaveData( UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort, IPDATA_IN* pDataIn, LPBYTE pUdpData, WORD wLenUdpData )

{
//	DWORD			dwLenData;
	UDP_RXNODE*		pUdpRxNode = NULL;
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	BOOL bRetv = FALSE;

	EnterCriticalSection( &pTcbUdp->csListRxNode );


	//Ѱ�� Ŀ��RxNode����Ͷ������
	pUnitHeader = &pTcbUdp->hListRxNode;

	//
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && HANDLE_F_OK(pTcbUdp, UDPF_EXIT) )
	{
		//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 

		//
		if( (pUdpRxNode_Find->dwIP==dwSrcIP) && (pUdpRxNode_Find->wPort==wSrcPort) )
		{
			pUdpRxNode = pUdpRxNode_Find;
			break;
		}
	}
	//
	if( !pUdpRxNode )
	{
		if( !Udp_CreateRxNode(&pUdpRxNode, pTcbUdp, dwSrcIP, wSrcPort) )
		{
			//LeaveCriticalSection( &pTcbUdp->csListRxNode );
			//return FALSE;
			goto _return;
		}
	}
	//dwLenData = wLenUdpData;
	//RWBuf2_WriteData( &pUdpRxNode->stRx_RWBuf, pUdpData, &dwLenData, 0 );
	pUdpRxNode->dwTickRcv = GetTickCount();

	//if( pUdpRxNode->pDataIn == NULL )
	if( pUdpRxNode->dwAvailDataLen + wLenUdpData <= pUdpRxNode->dwDataLenLimit )
	{
		//pUdpRxNode->pDataIn = pDataIn;
		//��������
		pDataIn->pData = pUdpData;
		pDataIn->wLenData = wLenUdpData;
		
		// FIFO
		List_InsertTail( &pUdpRxNode->hIPList, &pDataIn->hListDataIn );
		pUdpRxNode->dwAvailDataLen += wLenUdpData;		

		bRetv = TRUE;
	}
	else
	{
		WARNMSG( DEBUG_UdpIn_SaveData, ( "pUdpRxNode->pDataIn has data!.\r\n" ) );
	}
	
	//
_return:

	LeaveCriticalSection( &pTcbUdp->csListRxNode );
	return bRetv;

}


// ********************************************************************
// ������
// ������
//	IN pHdrIP-���յ����ݰ�IP Header
//	IN pHdrUdp-���յ����ݰ�TCP Header
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
//	IN/OUT ppTcbUdp-Ҫƥ���TCB
// ����ֵ��
//	
// ����������ƥ��pHdrIP��pHdrUdpָ����TCB
// ����: 
// ********************************************************************
BOOL	Udp_MatchTcb( OUT UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp, IP_HDR* pHdrIP, UDP_HDR* pHdrUdp )
{
	UDP_TCB*		pTcbUdp_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppTcbUdp = NULL;
	pUnitHeader = &lpGlobalUdp->hListTcb;
	EnterCriticalSection( &lpGlobalUdp->csListTcb );
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(lpGlobalUdp->wFlag & TCPIPF_EXIT) )
	{
		//���ݽṹ��Ա��ַ���õ��ṹ��ַ��������
		pTcbUdp_Find = LIST_CONTAINER( pUnit, UDP_TCB, hListTcb );
		pUnit = pUnit->pNext;    // advance to next 

		//�����ȫѰ��
		if( !HANDLE_CHECK(pTcbUdp_Find) )
		{
			break;
		}
		if( pTcbUdp_Find->wFlag & UDPF_EXIT )
		{
			continue;
		}
		//���ض˿ڱȽ�
		if( pTcbUdp_Find->wSrcPort==pHdrUdp->wDesPort )
		{
			//��ַ�Ƚ�---
			if( (pTcbUdp_Find->dwSrcIP==pHdrIP->dwDesIP) && 
				(pTcbUdp_Find->dwDesIP==pHdrIP->dwSrcIP) && 
				(pTcbUdp_Find->wDesPort==pHdrUdp->wSrcPort)
				)
			{
				*ppTcbUdp = pTcbUdp_Find;
				break;
			}
		}
	}
	//
	LeaveCriticalSection( &lpGlobalUdp->csListTcb );

	return (*ppTcbUdp)?TRUE:FALSE;
}




