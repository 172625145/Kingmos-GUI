/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����IP ��Ƭ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-12-01
���ߣ�    ФԶ��
�޸ļ�¼��
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
#include "iphlpapi_call.h"
//"local"
#include "ip_def.h"
#include "ipfrag.h"


/***************  ȫ���� ���壬 ���� *****************/

extern	BOOL	IPData_Out( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData );
extern	BOOL	IPData_In( IP_INTF* pNetIntf, IP_HDR* pHdrIP, WORD wLenData );

static	BOOL	IP_FindFgNode( IPFRAG_NODE** ppFgNode, IP_GLOBAL* lpGlobalIP, IP_HDR* pHdrIP );
static	void	IP_NewFgNode( IPFRAG_NODE** ppFgNode, IP_INTF* pNetIntf, IP_HDR* pHdrIP );
static	BOOL	IP_SavFgNode( IPFRAG_NODE* pFgNode, IP_HDR* pHdrIP, LPBYTE* ppBuf, WORD* pwLenData );
static	void	IP_DelFgNode( IPFRAG_NODE* pFgNode );
static	DWORD	WINAPI	IP_ThrdFgNode( LPVOID lpParam );

/******************************************************/

BOOL	IPFrag_Init( )
{
	DWORD			dwThrdID;

	g_lpGlobalIP->hEvtFgNode = CreateEvent( NULL, FALSE, FALSE, NULL );
	//����
	g_lpGlobalIP->hThrdFgNode = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)IP_ThrdFgNode, (LPVOID)g_lpGlobalIP, 0, &dwThrdID );
	if( !g_lpGlobalIP->hThrdFgNode )
	{
		return FALSE;
	}
	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN lpParam-IP��ȫ����Ϣ
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE 
// �����������߳�---��⡰������ɾ����
// ����: 
// ********************************************************************
DWORD	WINAPI	IP_ThrdFgNode( LPVOID lpParam )
{
	IP_GLOBAL*		lpGlobalIP = (IP_GLOBAL*)lpParam;
	IPFRAG_NODE*	pFgNode_Find;
	PLIST_UNIT		pUnit;
	PLIST_UNIT		pUnitHeader;

	while( HANDLE_F_OK(lpGlobalIP, GIPF_EXIT) )
	{
		//�ȴ�
		WaitForSingleObject( lpGlobalIP->hEvtFgNode, TIMER_FRAGNODE );
		if( HANDLE_F_FAIL(lpGlobalIP, GIPF_EXIT) )
		{
			break;
		}
		//��ѯ���е� FRAGNODE����ɾ��ָ����
		pUnitHeader = &lpGlobalIP->hListFgNode;
		EnterCriticalSection( &lpGlobalIP->csListFgNode );
		pUnit = pUnitHeader->pNext;
		while( pUnit!=pUnitHeader )
		{
			pFgNode_Find = LIST_CONTAINER( pUnit, IPFRAG_NODE, hListFgNode );
			pUnit = pUnit->pNext;
			
			if( !HANDLE_CHECK(pFgNode_Find) )
			{
				break;
				//continue;
			}
			if( (GetTickCount()-pFgNode_Find->dwTickLife)>=TIMER_FRAGNODE )
			{
				//���ͷŸ�FRAGNODE
				IP_DelFgNode( pFgNode_Find );
			}
		}
		LeaveCriticalSection( &lpGlobalIP->csListFgNode );
	}

	return 0;
}

#define DEBUG_IPFrag_Rx 0
void	IPFrag_Rx( IP_INTF* pNetIntf, IP_HDR* pHdrIP )
{
	IPFRAG_NODE*	pFgNode;
	LPBYTE			pBuf;
	WORD			wLenData;

	pBuf = NULL;
	EnterCriticalSection( &g_lpGlobalIP->csListFgNode );
	//Ѱ�Ҹý��
	if( IP_FindFgNode( &pFgNode, g_lpGlobalIP, pHdrIP ) )
	{
		//ֱ�ӱ������ݣ����ж��Ƿ�����ύ
		DEBUGMSG( DEBUG_IPFrag_Rx, ( "call  IP_SavFgNode.\r\n" ) );
		IP_SavFgNode( pFgNode, pHdrIP, &pBuf, &wLenData );
	}
	else
	{
		//��Ӹý�㣬����������
		DEBUGMSG( DEBUG_IPFrag_Rx, ( "call   IP_NewFgNode.\r\n" ) );
		IP_NewFgNode( &pFgNode, pNetIntf, pHdrIP );
	}
	LeaveCriticalSection( &g_lpGlobalIP->csListFgNode );

	//
	if( pBuf )
	{
		//�ύ�����ն�����
		IPData_In( pNetIntf, (IP_HDR*)pBuf, wLenData );
		free( pBuf );
	}
}


BOOL	IP_FindFgNode( IPFRAG_NODE** ppFgNode, IP_GLOBAL* lpGlobalIP, IP_HDR* pHdrIP )
{
	IPFRAG_NODE*	pFgNode_Find;
	IP_HDR*			pHdrIP_Find;
	PLIST_UNIT		pUnitHeader;
	PLIST_UNIT		pUnit;

	*ppFgNode = NULL;
	pUnitHeader = &lpGlobalIP->hListFgNode;
	pUnit = pUnitHeader->pNext;
	while( (pUnit!=pUnitHeader) && !(lpGlobalIP->wFlag & GIPF_EXIT) )
	{
		pFgNode_Find = LIST_CONTAINER( pUnit, IPFRAG_NODE, hListFgNode );
		pUnit = pUnit->pNext;    // advance to next 

		//
		if( !(HANDLE_CHECK(pFgNode_Find)) )
		{
			break;
			//continue;
		}
		//ƥ�����
		pHdrIP_Find = &pFgNode_Find->HdrIP;
		if( (pHdrIP_Find->bProType==pHdrIP->bProType) &&
			(pHdrIP_Find->dwSrcIP==pHdrIP->dwSrcIP) &&
			(pHdrIP_Find->dwDesIP==pHdrIP->dwDesIP) &&
			(pHdrIP_Find->wXID==pHdrIP->wXID) 
		  )
		{
			//�����
			*ppFgNode = pFgNode_Find;
			break;
		}
	}

	return (*ppFgNode)?TRUE:FALSE;
}


void	IP_NewFgNode( IPFRAG_NODE** ppFgNode, IP_INTF* pNetIntf, IP_HDR* pHdrIP )
{
	IPFRAG_NODE*	pFgNode;
	WORD			wLen;
	WORD			wTmp;

	//��֤���� �׸���Ƭ���ݰ�---IPF_MF, Offset==0
	if( ((pHdrIP->wFragment & IPF_MF)==0) || (pHdrIP->wFragment & IPF_OFFSET) )
	{
		return ;
	}
	//
	wLen = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wTmp = wLen + sizeof(IPFRAG_NODE) - sizeof(IP_HDR);
	//
	pFgNode = (IPFRAG_NODE*)HANDLE_ALLOC( wTmp );
	if( !pFgNode )
	{
		return ;
	}
	HANDLE_INIT( pFgNode, wTmp );
	List_InitHead( &pFgNode->hListFgNode );
	//
	pFgNode->pNetIntf = pNetIntf;
	pFgNode->dwTickLife = GetTickCount();
	//����ͷ��Ϣ
	memcpy( &pFgNode->HdrIP, pHdrIP, wLen );

	//��������
	if( !IP_SavFgNode( pFgNode, pHdrIP, NULL, NULL ) )
	{
		//���ͷŸ�FRAGNODE
		IP_DelFgNode( pFgNode );
		return ;
	}
	else
	{
		//
		List_InsertTail( &g_lpGlobalIP->hListFgNode, &pFgNode->hListFgNode );
		return ;
	}
}

static DWORD IP_IsFgNodeOk( IPFRAG_NODE* pFgNode )
{
	IPFRAG_BUF*		pFgBufCur;
	WORD wOffset = 0;
	pFgBufCur = pFgNode->pHeadBuf;
	while( pFgBufCur )
	{
		//������
		if( pFgBufCur->wOffset != wOffset )
		{
			return 0;
		}
		wOffset += pFgBufCur->wLenData;
		pFgBufCur = pFgBufCur->pNext;
	}
	return wOffset;
}

static VOID IP_InsertFg( IPFRAG_NODE* pFgNode, IPFRAG_BUF*		pNewFragBuf )
{
	IPFRAG_BUF*		pFgBufCur, *pFgBufPrev;
	
	
	WORD wOffset = pNewFragBuf->wOffset;

	if( !pFgNode->pHeadBuf )
	{
		pFgNode->pHeadBuf = pNewFragBuf;
	}
	//if( !pFgNode->pTailBuf )
	//{
	//	pFgNode->pTailBuf = pFragBuf;
	//}
	//else if( wOffset == pFgNode->wOffset )
	//{
	//	pFgNode->pTailBuf->pNext = pFragBuf;
	//	pFgNode->pTailBuf = pFragBuf;
	//}
	else
	{
		pFgBufPrev = pFgBufCur = pFgNode->pHeadBuf;
		
		while( pFgBufCur )
		{
			if( wOffset < pFgBufCur->wOffset )
			{				
				if( pFgBufPrev == pFgBufCur )
				{	//��ͷ
					pNewFragBuf->pNext = pFgBufCur;
					pFgNode->pHeadBuf = pNewFragBuf;
				}
				else
				{
					pNewFragBuf->pNext = pFgBufCur;
					pFgBufPrev->pNext = pNewFragBuf;
				}
				break;
			}			
			//������
			pFgBufPrev = pFgBufCur;
			pFgBufCur = pFgBufCur->pNext;
		}
		if( pFgBufCur == NULL && pFgBufPrev )
		{  //�ŵ�ĩβ
			pFgBufPrev->pNext = pNewFragBuf;
		}
	}
}

BOOL	IP_SavFgNode( IPFRAG_NODE* pFgNode, IP_HDR* pHdrIP, LPBYTE* ppBuf, WORD* pwLenData )
{
	IPFRAG_BUF*		pFragBuf;
	IP_HDR*			pHdrIP_Run;
	LPBYTE			pTmp;
	IPFRAG_BUF*		pFgBufCur;
	WORD			wOffset;
	WORD			wLenHdr;
	WORD			wLenData;

	//�Ƚϵ�ǰ�ȴ��ķ�Ƭ��ţ��ǲ���Ҫ�ȴ��ı��
	wOffset = (pHdrIP->wFragment & IPF_OFFSET) <<3;
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wLenData = pHdrIP->wLenTotal - wLenHdr;
/* lilin remove
	if( (wOffset!=pFgNode->wOffset) || !wLenData )
	{
		return FALSE;
	}
	*/
	if( !wLenData )
		return FALSE;

	//�ж�ÿ����Ƭ�ļ��ʱ�䣬�Ƿ���
	if( pHdrIP->wFragment & IPF_MF )
	{
		if( (GetTickCount()-pFgNode->dwTickLife)>=TIMER_FRAGNODE )
		{
			//���ͷŸ�FRAGNODE
			IP_DelFgNode( pFgNode );
			return FALSE;
		}
	}

	
	pFgNode->dwTickLife = GetTickCount(); 

	//����
	pFragBuf = (IPFRAG_BUF*)malloc( sizeof(IPFRAG_BUF) + wLenData );
	if( !pFragBuf )
	{
		//�Ƿ��Ƿ����� FRAGNODE
		return FALSE;
	}
	memset( pFragBuf, 0, sizeof(IPFRAG_BUF) );

	//��������
	memcpy( pFragBuf->pData, (LPBYTE)pHdrIP + wLenHdr, wLenData );
	pFragBuf->wLenData = wLenData;
	pFragBuf->wOffset = wOffset; //lilin add -2004-12-14

	IP_InsertFg( pFgNode, pFragBuf ); //lilin add-insert & sort-2004-12-14

	//��ӵ�FRAGNODE
/*
	//pFgNode->wOffset += wLenData;	//lilin remove

	if( !pFgNode->pHeadBuf )
	{
		pFgNode->pHeadBuf = pFragBuf;
	}
	if( !pFgNode->pTailBuf )
	{
		pFgNode->pTailBuf = pFragBuf;
	}
	else
	{
		pFgNode->pTailBuf->pNext = pFragBuf;
		pFgNode->pTailBuf = pFragBuf;
	}
*/
	//�Ƿ����һ��Ƭ
	if( pFgNode->wEndFlag == FALSE )
		pFgNode->wEndFlag = !(pHdrIP->wFragment & IPF_MF);
	//�ж��ǲ������1Ƭ
	
	//if( !(pHdrIP->wFragment & IPF_MF) )
	if( pFgNode->wEndFlag && 
		(wOffset = (WORD)IP_IsFgNodeOk(pFgNode)) != 0 )  //lilin modify
	{			
		//���ύ�����ݰ�
		wLenHdr = (pFgNode->HdrIP.bVerHLen & IPL_MASK)<<2;
		//pTmp = (LPBYTE)malloc( pFgNode->wOffset+wLenHdr );
		pTmp = (LPBYTE)malloc( wOffset+wLenHdr );
		if( pTmp )
		{
			*ppBuf = pTmp;
			//��ȡͷ��Ϣ
			memcpy( pTmp, &pFgNode->HdrIP, wLenHdr );
			pTmp += wLenHdr;

			//��ȡ����
			wLenData = 0;
			pFgBufCur = pFgNode->pHeadBuf;
			while( pFgBufCur )
			{
				//��������
				memcpy( pTmp, pFgBufCur->pData, pFgBufCur->wLenData );
				pTmp += pFgBufCur->wLenData;
				wLenData += pFgBufCur->wLenData;

				//������
				pFgBufCur = pFgBufCur->pNext;
			}
			*pwLenData = wLenData;
			pHdrIP_Run = (IP_HDR*)*ppBuf;
			pHdrIP_Run->wLenTotal = wLenHdr + wLenData;
		}

		//���ͷŸ�FRAGNODE
		IP_DelFgNode( pFgNode );
	}
	
	return TRUE;
}

void	IP_DelFgNode( IPFRAG_NODE* pFgNode )
{
	IPFRAG_BUF*		pFgBufCur;
	IPFRAG_BUF*		pFgBufNxt;

	//
	if( !HANDLE_CHECK(pFgNode) )
	{
		return ;
	}

	//�Ͽ�
	List_RemoveUnit( &pFgNode->hListFgNode );

	//�ͷ���������
	pFgBufCur = pFgNode->pHeadBuf;
	while( pFgBufCur )
	{
		//������
		pFgBufNxt = pFgBufCur->pNext;

		//�ͷ�����
		free( pFgBufCur );
		
		//������
		pFgBufCur = pFgBufNxt;
	}

	//�ͷŸ�FRAGNODE
	free( pFgNode );
}

DWORD	IPFrag_Tx( IP_INTF* pNetIntf, IP_HDR* pHdrIP, LPBYTE pData, WORD wLenData )
{
	LPBYTE		pOffset;
	WORD		wLenHdr;
	WORD		wLenAll;
	WORD		wMaxData;
	WORD		wTmp;
	
	//׼��һЩ��ͬ����Ϣ
	wLenHdr = (pHdrIP->bVerHLen & IPL_MASK)<<2;
	wMaxData = pNetIntf->wMTU - wLenHdr;
	wMaxData >>= 3;
	wMaxData <<= 3;
	pOffset = pData;
	wLenAll = wLenData;

	pHdrIP->bVerHLen = IPV4_VHDEF;

	//
	//���ͷ�����Ƭ: ------------------
	//
	while( (wLenAll > wMaxData) && !(pNetIntf->wFlag & IPIFF_EXIT) )
	{
		//�ύ�����Ͷ�����
		wTmp = ((WORD)(pOffset - pData))>>3;
		pHdrIP->wFragment = (IPF_MF | wTmp);	//��ʾ���и����Ƭ������wTmpƫ��
		//�ύ�����Ͷ�����
		if( !IPData_Out( pNetIntf, pHdrIP, pOffset, wMaxData ) )
		{
			return IPERR_SND_NOBUF;
		}
		wLenAll -= wMaxData;
		pOffset  += wMaxData;
	}

	//
	//��������Ƭ: ------------------
	//
	if( wLenAll )
	{
		//�ύ�����Ͷ�����
		wTmp = ((WORD)(pOffset - pData))>>3;
		pHdrIP->wFragment = wTmp;	//��ʾ������Ƭ������wTmpƫ��
		//�ύ�����Ͷ�����
		if( !IPData_Out( pNetIntf, pHdrIP, pOffset, wLenAll ) )
		{
			return IPERR_SND_NOBUF;
		}
	}

	return IPERR_SUCCESS;
}

