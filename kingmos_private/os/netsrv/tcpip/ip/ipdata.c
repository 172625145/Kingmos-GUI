/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����IP���ݰ����ϴ����ʹ���
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-04-21
���ߣ�    ФԶ��
�޸ļ�¼��

ע�ͣ�ФԶ�� 2004-04-21 д����
1���ö��к��̵߳ķ���������ҪΪ���湦���ṩ֧�֣�
   1> �ڽ��մ����У�����ת�����ݰ�
   2> �ڷ��ʹ����У����Ի����ύ
2��Ŀǰ����IP���ݰ��Ļ��ƣ�
   1> �û����ύ�������߳�ǰ���ð�����
      У�顢��������ת����顢ת��IPͷ��HOST˳����װ���γ�1��IP���ݰ�BUFFER
   2> 
3��Ŀǰ����IP���ݰ��Ļ��ƣ�
   1> �û����ύ�������߳�ǰ����������ع�����
      ѡ�ӿڡ�ѡ·�����ѡ·��ַ�����IPͷ��Ϣ����Ƭ���γ�1��IP���ݰ�BUFFER
   2> �����̵߳����񣬾��ǻ�ȡ�ð���ֱ���ύ��������·�㷢��
4����������ȱ�㣺
   1> �����������·�㷢��ʧ�ܣ������ò����ô�����Ϣ
   2> ��ֱ���ύ����㴦��Ļ��ƣ����˷�1Щʱ�䣻
      һ������������ٶ���Ҫ������Ӱ�죬�����Ǵ����ٶ�
5�����������ŵ㣺
   1> ���յ����ݣ���IP_HDR��˵�Ƕ����
   2> ����������Ϊ�Ƕ�����ʽ�����ԣ������ٶȻ�ȽϿ�

******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
#include <iphlpapi.h>
//"\inc_local"
#include "proto_ip_tcp.h"
#include "netif_ip_dl.h"
#include "netif_tcp_ip.h"
//"local"
#include "ip_def.h"


/***************  ȫ���� ���壬 ���� *****************/

void	IP_Deliver( IP_INTF* pNetIntf, IPDATA_IN* pIPDataIn, WORD wLenData );

DWORD	WINAPI	IP_ThrdDataIn( LPVOID lpParam );
DWORD	WINAPI	IP_ThrdDataOut( LPVOID lpParam );

static	long	g_nCntRx_TCP = 0;
/******************************************************/
IPDATA_IN* IPDataIn_Alloc( UINT uDataSize );



BOOL	IPData_Start( IP_INTF* pNetIntf )
{
	DWORD	dwThrdID;

	//�������յ��߳�
	pNetIntf->hThrdDataIn = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)IP_ThrdDataIn, (LPVOID)pNetIntf, 0, &dwThrdID );
	if( !pNetIntf->hThrdDataIn )
	{
		return FALSE;
	}
	//�������͵��߳�
	pNetIntf->hThrdDataOut = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)IP_ThrdDataOut, (LPVOID)pNetIntf, 0, &dwThrdID );
	if( !pNetIntf->hThrdDataOut )
	{
		return FALSE;
	}
	return TRUE;
}


DWORD	WINAPI	IP_ThrdDataIn( LPVOID lpParam )
{
	IP_INTF*		pNetIntf = (IP_INTF*)lpParam;
	IPDATA_IN*		pDataIn;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	pUnitHeader = &pNetIntf->hListDataIn;
	while( HANDLE_F_OK(pNetIntf, IPIFF_EXIT) )
	{
		//�ȴ����ݰ�
		WaitForSingleObject( pNetIntf->hEvtDataIn, INFINITE );
		if( HANDLE_F_FAIL(pNetIntf, IPIFF_EXIT) )
		{
			break;
		}
		//��ȡ���ݰ�
		while( !(pNetIntf->wFlag & IPIFF_EXIT) )
		{
			//��ȡ���ݰ�
			EnterCriticalSection( &pNetIntf->csListDataIn );
			pUnit = pUnitHeader->pNext;
			if( pUnit!=pUnitHeader )
			{
				//��ȡ���ݰ�
				pDataIn = LIST_CONTAINER( pUnit, IPDATA_IN, hListDataIn );
				List_RemoveUnit( &pDataIn->hListDataIn );
			}
			else
			{
				LeaveCriticalSection( &pNetIntf->csListDataIn );
				break;
			}
			LeaveCriticalSection( &pNetIntf->csListDataIn );
			//�������ݰ�
			if( pUnit!=pUnitHeader )
			{
				//�ϴ����ݰ�
				//pDataIn->bAutoFree = 1;
				IP_Deliver( pNetIntf, pDataIn, pDataIn->wLenData );
				//ASSERT( pDataIn->bAutoFree == 1 || pDataIn->bAutoFree == 0 );
				//�ͷ����ݰ�
				//free( pDataIn );
				//if(  pDataIn->bAutoFree == 1 )
				//	IPDataIn_Free( pDataIn );	//auto free
				//else
				//{
				//	pDataIn = pDataIn;  // only for test
				//}
			}
		}
	}

	return 0;
}

#define DEBUG_IP_Deliver 0
void	IP_Deliver( IP_INTF* pNetIntf, IPDATA_IN* pDataIn, WORD wLenData )
{
	LPBYTE		pData;
	IP_HDR * pHdrIP ;

	//�ַ�Э���
	pHdrIP = &pDataIn->HdrIP;
	pDataIn->pData = pData = (LPBYTE)pHdrIP + ((pHdrIP->bVerHLen & IPL_MASK)<<2);
	
	switch( pHdrIP->bProType )
	{
	case IP_PROTYPE_TCP:
		//RETAILMSG(1,(TEXT(" \r\nIP_Deliver_TCP: g_nCntRx_TCP=[%d], wLenData=[%d]\r\n"), ++g_nCntRx_TCP, wLenData));
		Tcp_TrIn( (HANDLE)pNetIntf, pDataIn, pData, wLenData );
		break;
	case IP_PROTYPE_UDP:
		Udp_TrIn( (HANDLE)pNetIntf, pDataIn, pData, wLenData );
		break;
	case IP_PROTYPE_ICMP:
		Icmp_TrIn( (HANDLE)pNetIntf, pDataIn, pData, wLenData );
		break;
	default :
		WARNMSG( DEBUG_IP_Deliver,(TEXT("IP_Deliver: get a unused packet.\r\n") ));
		break;
	}

}
/*
LPVOID lpCheckPtr = NULL;

static void CheckPtr( LPVOID p, char * file, int line )
{
	if( p )
	{
		LPDWORD lp = (LPDWORD)( (DWORD)p - 4 );
		
		if( *lp != 0x1234abcd )
		{
			RETAILMSG( 1, ( "error ptr chesum(0x%x),file=%s,line=%d.\r\n", *lp, file, line ) );
		}			
		else if( HeapValidate( GetProcessHeap(), 0, lp ) == FALSE )
		{
			RETAILMSG( 1, ( "error ptr:file=%s,line=%d.\r\n", file, line ) );
		}
	}
}
*/
//void CheckOutPtr( char * file, int line )
//{
	//CheckPtr( lpCheckPtr, file, line );
//}

#define	MAX_FRAGCNT			20
DWORD	WINAPI	IP_ThrdDataOut( LPVOID lpParam )
{
	IP_INTF*		pNetIntf = (IP_INTF*)lpParam;
	IPDATA_OUT*		pDataOut;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;
	PARAM_DLOUT		ParamOut;
	BYTE			pBufFragSet[sizeof(FRAGSETD) + MAX_FRAGCNT*sizeof(FRAGMENT)];
	FRAGSETD*		pFragSetD = (FRAGSETD*)pBufFragSet;
	DWORD			dwRet;

	//
	pUnitHeader = &pNetIntf->hListDataOut;
	pFragSetD->nCntAll = MAX_FRAGCNT;
	//
	while( HANDLE_F_OK(pNetIntf, IPIFF_EXIT) )
	{
		//�ȴ����ݰ�
		WaitForSingleObject( pNetIntf->hEvtDataOut, INFINITE );
		if( HANDLE_F_FAIL(pNetIntf, IPIFF_EXIT) )
		{
			break;
		}
		//��ȡ���ݰ�
		while( !(pNetIntf->wFlag & IPIFF_EXIT) )
		{
			//��ȡ���ݰ�
			EnterCriticalSection( &pNetIntf->csListDataOut );
			pUnit = pUnitHeader->pNext;
/*
			if( pUnit!=pUnitHeader )
			{
				pDataOut = LIST_CONTAINER( pUnit, IPDATA_OUT, hListDataOut );
				List_RemoveUnit( &pDataOut->hListDataOut );
			}
			else
			{
				LeaveCriticalSection( &pNetIntf->csListDataOut );
				break;
			}
*/
			if( pUnit==pUnitHeader )
			{ 
				LeaveCriticalSection( &pNetIntf->csListDataOut );
				break;
			}			

			pDataOut = LIST_CONTAINER( pUnit, IPDATA_OUT, hListDataOut );

			//RETAILMSG(1,(TEXT("out001,pDataOut=%x,next=%x,prev=%x.\r\n"), pDataOut, pDataOut->hListDataOut.pNext, pDataOut->hListDataOut.pPrev ));
			//RETAILMSG(1,(TEXT("out001-1,pUnitHeader=%x,next=%x,prev=%x.\r\n"), pUnitHeader, pUnitHeader->pNext, pUnitHeader->pPrev ));

			//CheckPtr( pDataOut, __FILE__, __LINE__ );
			
			List_RemoveUnit( &pDataOut->hListDataOut );



			//RETAILMSG(1,(TEXT("out0022.\r\n") ));

			LeaveCriticalSection( &pNetIntf->csListDataOut );
			//�������ݰ�
			//RETAILMSG(1,(TEXT("out003.\r\n") ));
//			if( pUnit!=pUnitHeader )
			{
				//�������ݰ�
				ParamOut.dwProtoID = PROID_IP;
				ParamOut.dwOption = DLOUT_OPT_ADDR_IP;
				ParamOut.pDesAddr = (LPBYTE)&pDataOut->dwDesIP;
				ParamOut.dwLenAddr = sizeof(DWORD);
				ParamOut.pFragSetD = pFragSetD;



				pFragSetD->nCntUsed = 1;
				pFragSetD->dwBytes = pDataOut->wLenTotal;
				pFragSetD->pFrag[0].pData = pDataOut->pBufIP;
				pFragSetD->pFrag[0].nLen = pDataOut->wLenTotal;

				//CheckPtr( pDataOut, __FILE__, __LINE__ );
												
				//RETAILMSG(1,(TEXT("out004.\r\n") ));
				
				//lpCheckPtr = pDataOut;
				dwRet = (*pNetIntf->pFnDlOut)( pNetIntf->hIfDl, &ParamOut );
				//lpCheckPtr = NULL;

				//RETAILMSG(1,(TEXT(" \r\n IP_ThrdDataOut: wLenTotal=[%d], dwRet=[%d]\r\n"), pFragSetD->dwBytes, dwRet));
				//HeapValidate( GetProcessHeap(), 0, pDataOut );
				//CheckOutPtr( __FILE__, __LINE__ );
				//CheckPtr( pDataOut, __FILE__, __LINE__ );

				//RETAILMSG(1,(TEXT("out005.\r\n") ));
				//�ͷ����ݰ�
				free( (LPBYTE)pDataOut );
				//RETAILMSG(1,(TEXT("out006.\r\n") ));				
			}
			//RETAILMSG(1,(TEXT("out006.\r\n") ));
		}
	}

	return 0;
}

IPDATA_IN* IPDataIn_Alloc( UINT uDataSize )
{
	return (IPDATA_IN*)malloc( sizeof(IPDATA_IN)+uDataSize );
}

VOID IPDataIn_Free( IPDATA_IN* pDataIn )
{
	free( pDataIn );
}

BOOL	IPData_In( IP_INTF* pNetIntf, IP_HDR* pHdrIP, WORD wLenData )
{
	IPDATA_IN*		pDataIn;

	//���������
	pDataIn = IPDataIn_Alloc(pHdrIP->wLenTotal);//(IPDATA_IN*)malloc( sizeof(IPDATA_IN)+pHdrIP->wLenTotal );
	if( !pDataIn )
	{
		return FALSE;
	}
	memset( pDataIn, 0, sizeof(IPDATA_IN) );
	List_InitHead( &pDataIn->hListDataIn );
	pDataIn->wLenData = wLenData;
	memcpy( pDataIn->pBufIP, pHdrIP, pHdrIP->wLenTotal );
	//���
	EnterCriticalSection( &pNetIntf->csListDataIn );
	List_InsertTail( &pNetIntf->hListDataIn, &pDataIn->hListDataIn );
	LeaveCriticalSection( &pNetIntf->csListDataIn );
	SetEvent( pNetIntf->hEvtDataIn );
	//PulseEvent( pNetIntf->hEvtDataIn );

	return TRUE;
}

#define DEBUG_IPData_Out 0
BOOL	IPData_Out( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
{
	IPDATA_OUT*		pDataOut;
	WORD			wLenHdr;
	WORD			wLenTotal;

	DEBUGMSG( DEBUG_IPData_Out, ( "IPData_Out: wLenData=%d.\r\n", wLenData ) );
	//
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wLenTotal = wLenHdr+wLenData;
	//���䷢����
	pDataOut = (IPDATA_OUT*)malloc( sizeof(IPDATA_OUT)+wLenTotal);
	if( !pDataOut )
	{
		return FALSE;
	}
	//*( (LPDWORD)pDataOut ) = 0x1234abcd;
	//pDataOut = (IPDATA_OUT*)((LPBYTE)pDataOut + 4);

	//RETAILMSG( 1, ( "pDataOut:0x%x.\r\n", pDataOut ) );
	memset( pDataOut, 0, sizeof(IPDATA_OUT) );
	List_InitHead( &pDataOut->hListDataOut );
	pDataOut->dwDesIP = pHdrIP->dwDesIP;
	pDataOut->wLenTotal = wLenTotal;

	//ת�� IP��Ϣ--- wLenTotal / wFragment / wHdrCRC
	pHdrIP->wHdrCRC = 0;
#ifndef BIG_ENDIAN
	pHdrIP->wLenTotal  = htons_m(wLenTotal);
	pHdrIP->wFragment  = htons(pHdrIP->wFragment);
#else
	pHdrIP->wLenTotal  = wLenTotal;
	//pHdrIP->wFragment  = pHdrIP->wFragment;
#endif
	pHdrIP->wHdrCRC = IP_CRC( (LPBYTE)pHdrIP, wLenHdr );

	//��������
	memcpy( pDataOut->pBufIP, pHdrIP, wLenHdr );
	memcpy( pDataOut->pBufIP+wLenHdr, pData, wLenData );

	//CheckPtr( pDataOut, __FILE__, __LINE__ );

	//���
	EnterCriticalSection( &pNetIntf->csListDataOut );
	List_InsertTail( &pNetIntf->hListDataOut, &pDataOut->hListDataOut );
	LeaveCriticalSection( &pNetIntf->csListDataOut );

	//CheckPtr( pDataOut, __FILE__, __LINE__ );
	//����
	SetEvent( pNetIntf->hEvtDataOut );
	//PulseEvent( pNetIntf->hEvtDataOut );

	return TRUE;
}







