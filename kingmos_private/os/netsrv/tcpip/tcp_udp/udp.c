/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����UDP 
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��

 ���汾TCPIP ˵����ФԶ�� 2004-04-12 д����
1�� ���״������ɣ������������ϵļ���ϣ�����д�ģ������������е���ػ��ơ�
    �ο����ϣ�RFC/IANA��WHL��TCPIP�汾��TCPIP��⡢���ʻ���
2�� ֧��Block��Nonblock����
3�� 
4�� 

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
UDP_GLOBAL*		g_lpGlobalUdp = NULL;


//
extern	BOOL	UdpSk_Init( UDP_GLOBAL* lpGlobalUdp );


//
void	Udp_DeleteTCB( UDP_TCB* pTcbUdp );
void	Udp_Free( UDP_GLOBAL* lpGlobalUdp );



/******************************************************/



// ********************************************************************
// ������
// ������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// ������������ʼ��UDP
// ����: 
// ********************************************************************
BOOL	Udp_Init( )
{
	UDP_GLOBAL*		lpGlobalUdp;

	//����1��UDP�Ĺ��� �ͻ�����ʼ��
	lpGlobalUdp = (UDP_GLOBAL*)HANDLE_ALLOC( sizeof(UDP_GLOBAL) );
	if( !lpGlobalUdp )
	{
		return FALSE;
	}
	HANDLE_INIT( lpGlobalUdp, sizeof(UDP_GLOBAL) );
	List_InitHead( &lpGlobalUdp->hListTcb );
	InitializeCriticalSection( &lpGlobalUdp->csListTcb );
	lpGlobalUdp->hEvtRxNode = CreateEvent( NULL, FALSE, FALSE, NULL );

	//
	lpGlobalUdp->wPort_Auto = PORTAUTO_MIN;
	//
	if( !UdpSk_Init( lpGlobalUdp ) )
	{
		goto EXIT_UDPINIT;
	}

	//
	if( !Sock_Register( &lpGlobalUdp->LocalTl ) )
	{
		goto EXIT_UDPINIT;
	}

	g_lpGlobalUdp = lpGlobalUdp;
	return TRUE;
EXIT_UDPINIT:
	if( lpGlobalUdp )
	{
		Udp_Free( lpGlobalUdp );
	}
	return FALSE;
}

void	Udp_Deinit( )
{
}

// ********************************************************************
// ������
// ������
//	IN hGlobalTcp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	
// �����������ͷ�TCP
// ����: 
// ********************************************************************
void	Udp_Free( UDP_GLOBAL* lpGlobalUdp )
{
	//�����ȫ���
	if( !HANDLE_CHECK(lpGlobalUdp) )
	{
		return ;
	}

	//
	DeleteCriticalSection( &lpGlobalUdp->csListTcb );

	//
	HANDLE_FREE( lpGlobalUdp );
}


// ********************************************************************
// ������
// ������
//  OUT ppTcbUdp-���洴����TCB
//	IN lpGlobalUdp-TCPIP��ȫ����Ϣ
// ����ֵ��
//	
// ��������������TCB
// ����: 
// ********************************************************************
BOOL	Udp_CreateTcb( UDP_TCB** ppTcbUdp, UDP_GLOBAL* lpGlobalUdp )
{
	UDP_TCB*	pTcbUdp;

	//���� �� ������ʼ��
	*ppTcbUdp = NULL;
	pTcbUdp = (UDP_TCB*)HANDLE_ALLOC( sizeof(UDP_TCB) );
	if( !pTcbUdp )
	{
		return FALSE;
	}
	HANDLE_INIT( pTcbUdp, sizeof(UDP_TCB) );
	List_InitHead( &pTcbUdp->hListTcb );
	//InitializeCriticalSection( &pTcbUdp->stRxBufRW.csBufRW );
	List_InitHead( &pTcbUdp->hListRxNode );
	InitializeCriticalSection( &pTcbUdp->csListRxNode );
	pTcbUdp->dwLenRcvBuf = LEN_RXBUF;

	pTcbUdp->lpGlobalUdp = lpGlobalUdp;
	pTcbUdp->hEvtPoll = CreateEvent( NULL, FALSE, FALSE, NULL );

	//��---����BUFFER
	//if( !RWBuf_Alloc( &pTcbUdp->stRxBufRW, LEN_RXBUF, MIN_RXBUF ) )
	//{
	//	goto EXIT_CREATETCB;
	//}

	*ppTcbUdp = pTcbUdp;
//EXIT_CREATETCB:
	if( !(*ppTcbUdp) && pTcbUdp )
	{
		Udp_DeleteTCB( pTcbUdp );
	}

	return (*ppTcbUdp)?TRUE:FALSE;
}


// ********************************************************************
// ������
// ������
//  IN pTcbUdp-ָ����TCB
// ����ֵ��
//	
// ����������ɾ��TCB
// ����: 
// ********************************************************************
void	Udp_DeleteTCB( UDP_TCB* pTcbUdp )
{
	UDP_GLOBAL*		lpGlobalUdp;

	//�����ȫ���
	if( !HANDLE_CHECK(pTcbUdp) )
	{
		return ;
	}
	lpGlobalUdp = pTcbUdp->lpGlobalUdp;
	if( !HANDLE_CHECK(lpGlobalUdp) )
	{
		lpGlobalUdp = NULL;
	}
	//
	pTcbUdp->wFlag |= UDPF_EXIT;
	//�� lpGlobalUdp������ �Ͽ�
	if( lpGlobalUdp )
	{
		EnterCriticalSection( &lpGlobalUdp->csListTcb );
		List_RemoveUnit( &pTcbUdp->hListTcb );
		List_InitHead( &pTcbUdp->hListTcb );
		LeaveCriticalSection( &lpGlobalUdp->csListTcb );
	}
	Sleep( 1 );
	//ֹͣ����TCB�Ĳ���
	if( pTcbUdp->hEvtPoll )
	{
		PulseEvent( pTcbUdp->hEvtPoll );
		SetEvent( pTcbUdp->hEvtPoll );
		Sleep( 1 );
	}
	Sleep( 1 );
	//
	Udp_CheckRxNode( pTcbUdp, TRUE );

	//
	if( pTcbUdp->hEvtPoll )
	{
		CloseHandle( pTcbUdp->hEvtPoll );
	}
	//
	//RWBuf_Free( &pTcbUdp->stRxBufRW );

	//�ͷž��
	HANDLE_FREE( pTcbUdp );
}


BOOL	Udp_CreateRxNode( OUT UDP_RXNODE** ppUdpRxNode, UDP_TCB* pTcbUdp, DWORD dwSrcIP, WORD wSrcPort )
{
	UDP_RXNODE*		pUdpRxNode;

	*ppUdpRxNode = NULL;
	//
	pUdpRxNode = (UDP_RXNODE*)HANDLE_ALLOC( sizeof(UDP_RXNODE) );
	if( !pUdpRxNode )
	{
		return FALSE;
	}
	HANDLE_INIT( pUdpRxNode, sizeof(UDP_RXNODE) );
	List_InitHead( &pUdpRxNode->hListRxNode );
	List_InitHead( &pUdpRxNode->hIPList );
	pUdpRxNode->dwDataLenLimit = pTcbUdp->dwLenRcvBuf;
	/* lilin remove
	//
	if( !RWBuf2_Alloc( &pUdpRxNode->stRx_RWBuf, pTcbUdp->dwLenRcvBuf, MIN_RXBUF ) )
	{
		Udp_DelRxNode( pUdpRxNode );
		return FALSE;
	}
	*/
	//
	//caller has EnterCriticalSection
	//EnterCriticalSection( &pTcbUdp->csListRxNode );
	List_InsertTail( &pTcbUdp->hListRxNode, &pUdpRxNode->hListRxNode );

	pUdpRxNode->dwIP = dwSrcIP;
	pUdpRxNode->wPort = wSrcPort;

	//LeaveCriticalSection( &pTcbUdp->csListRxNode );

	*ppUdpRxNode = pUdpRxNode;
	return TRUE;
}

static UINT Udp_GetRxDataLen( UDP_RXNODE*		pUdpRxNode )
{
	
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	IPDATA_IN* pDataIn;
	
	pUnitHeader = &pUdpRxNode->hIPList;
	
	//pUnit = pUnitHeader->pNext;
	if( (pUnit= pUnitHeader->pNext) !=pUnitHeader  )
	{
		pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
		return pDataIn->wLenData;
	}
	return 0;

	//return pUdpRxNode->dwAvailDataLen;
}

static UINT Udp_GetRxData( UDP_RXNODE*		pUdpRxNode, VOID * pData, DWORD * pdwDataLen )
{
	if( pdwDataLen )
	{
		PLIST_UNIT		pUnitHeader;
		PLIST_UNIT		pUnit;
		IPDATA_IN* pDataIn;

		UINT size = *pdwDataLen;//MIN( *pdwDataLen, pUdpRxNode->dwAvailDataLen );

		pUnitHeader = &pUdpRxNode->hIPList;
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader && size )
		{
			UINT minSize;
			pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
			pUnit = pUnit->pNext;    // advance to next 
			minSize = MIN( pDataIn->wLenData, size );
			memcpy( pData, pDataIn->pData, minSize ); 

			pDataIn->wLenData -= minSize;
			
			if( pDataIn->wLenData == 0 )
			{	//û�и�������ݣ��Ƴ�& ɾ��
				List_RemoveUnit( &pDataIn->hListDataIn );
				IPDataIn_Free(pDataIn);
			}

			pDataIn->pData = (LPBYTE)pDataIn->pData + minSize;
			size -= minSize;
			break;
		}
		//ʵ�ʿ���������
		*pdwDataLen -= size;

		pUdpRxNode->dwAvailDataLen -= *pdwDataLen;
		ASSERT( ( (LONG)pUdpRxNode->dwAvailDataLen ) >= 0 );


		return TRUE;
	}
	return FALSE;
}

static VOID Udp_FreeRxData( UDP_RXNODE*		pUdpRxNode )
{
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	IPDATA_IN* pDataIn;
	
	pUnitHeader = &pUdpRxNode->hIPList;
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
		pUnit = pUnit->pNext;    // advance to next 			
		
		//�Ƴ�& ɾ��
		List_RemoveUnit( &pDataIn->hListDataIn );
		IPDataIn_Free(pDataIn);
	}
}

DWORD	Udp_GetRxFirstData( UDP_TCB* pTcbUdp, OUT LPBYTE pData, IN OUT DWORD* pdwLenData, OUT RXTXMSG* pMsgRxTx )
{
	DWORD			dwFirstLen = 0;
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	EnterCriticalSection( &pTcbUdp->csListRxNode );
	//Ѱ�� Ŀ��RxNode����Ͷ������
	pUnitHeader = &pTcbUdp->hListRxNode;

	//
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			break;
		}

		//
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 

		//���������
		//if( (dwFirstLen= pUdpRxNode_Find->stRx_RWBuf.dwCntRW) )
		if( ( dwFirstLen= Udp_GetRxDataLen(pUdpRxNode_Find) ) )
		{
			if( pData )
			{
				//��ȡ����
				//if( RWBuf2_ReadData( &pUdpRxNode_Find->stRx_RWBuf, pData, pdwLenData ) )
				if( Udp_GetRxData( pUdpRxNode_Find, pData, pdwLenData ) )
				{
					dwFirstLen = *pdwLenData;
				}
				else
				{
					dwFirstLen = 0;
				}

				//���ȡ���������Ƶ����
				//if( !pUdpRxNode_Find->stRx_RWBuf.dwCntRW )
				if( !Udp_GetRxDataLen(pUdpRxNode_Find) )
				{
					List_RemoveUnit( &pUdpRxNode_Find->hListRxNode );
					List_InsertTail( &pTcbUdp->hListRxNode, &pUdpRxNode_Find->hListRxNode );
				}

				//
				if( pMsgRxTx->lpPeer )
				{
					SOCKADDR_IN*	pAddrIn;

					pAddrIn = (SOCKADDR_IN*)pMsgRxTx->lpPeer;
					memset( pAddrIn, 0, sizeof(SOCKADDR_IN) );
					pAddrIn->sin_family = AF_INET;
					pAddrIn->sin_port = pUdpRxNode_Find->wPort;
					pAddrIn->sin_addr.S_un.S_addr = pUdpRxNode_Find->dwIP;
					pMsgRxTx->dwLenPeer = sizeof(SOCKADDR_IN);
				}
			}
			else
			{
				//��������ݣ����Ƶ���ǰ��
				List_RemoveUnit( &pUdpRxNode_Find->hListRxNode );
				List_InsertHead( &pTcbUdp->hListRxNode, &pUdpRxNode_Find->hListRxNode );
			}

			break;
		}
	}
	//
	LeaveCriticalSection( &pTcbUdp->csListRxNode );
	return dwFirstLen;
}

void	Udp_DelRxNode( UDP_RXNODE* pUdpRxNode )
{
	List_RemoveUnit( &pUdpRxNode->hListRxNode );
	//RWBuf2_Free( &pUdpRxNode->stRx_RWBuf ); //lilin 
	Udp_FreeRxData( pUdpRxNode );

	HANDLE_FREE( pUdpRxNode );
}

void	Udp_CheckRxNode( UDP_TCB* pTcbUdp, BOOL fDelete )
{
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	EnterCriticalSection( &pTcbUdp->csListRxNode );
	//Ѱ�� Ŀ��RxNode����Ͷ������
	pUnitHeader = &pTcbUdp->hListRxNode;
	//
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			break;
		}

		//
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 

		//���û������
		if( fDelete || 
			//( !pUdpRxNode_Find->stRx_RWBuf.dwCntRW && 
			( !Udp_GetRxDataLen(pUdpRxNode_Find) &&
			  ((GetTickCount()-pUdpRxNode_Find->dwTickRcv)>=UDPTM_WAITRXNODE)) )
		{
			//ɾ����
			Udp_DelRxNode( pUdpRxNode_Find );
			break;
		}
	}
	//
	LeaveCriticalSection( &pTcbUdp->csListRxNode );
}


BOOL	Udp_SetRxNodeLen( UDP_TCB* pTcbUdp, DWORD dwLenNew )
{
	UDP_RXNODE*		pUdpRxNode_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;


	//
	if( !dwLenNew || dwLenNew==pTcbUdp->dwLenRcvBuf || dwLenNew<MIN_RXBUF )
	{
		return TRUE;
	}
	EnterCriticalSection( &pTcbUdp->csListRxNode );

	pTcbUdp->dwLenRcvBuf = dwLenNew;
	//Ѱ�� Ŀ��RxNode����Ͷ������
	pUnitHeader = &pTcbUdp->hListRxNode;

	//
	pUnit = pUnitHeader->pNext;
	while( pUnit!=pUnitHeader )
	{
		//
		if( HANDLE_F_FAIL(pTcbUdp, UDPF_EXIT) )
		{
			break;
		}

		//
		pUdpRxNode_Find = LIST_CONTAINER( pUnit, UDP_RXNODE, hListRxNode );
		pUnit = pUnit->pNext;    // advance to next 
		pUdpRxNode_Find->dwDataLenLimit = dwLenNew;



		//���
//		if( pUdpRxNode_Find->stRx_RWBuf.dwLenRW!=dwLenNew )
//		{
//			RWBuf2_Alloc( &pUdpRxNode_Find->stRx_RWBuf, dwLenNew, MIN_RXBUF );
//		}
	}
	//
	LeaveCriticalSection( &pTcbUdp->csListRxNode );

	return TRUE;

}





