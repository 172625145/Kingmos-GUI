/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ڴ浥Ԫ�� ����---"MUP": memory unit pool 
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-02-17
���ߣ�    ФԶ��
�޸ļ�¼��
˵����
    MUP����Ҫ���ú�ʹ��Ŀ�꣺
	  �൱��ʵ����1��1ά���飬��������Ԥ����ܶ������������Ƶ���ڴ�ȡ����������Խ��2�����⡣
	  1����ֹ�Ժ��ڴ���䲻��
	  2����ֹ��ɴ������ڴ���Ƭ��
******************************************************/
#include <ewindows.h>
#include <listunit.h>
//"inc_app"
#include <mup.h>
//"inc_drv"
#include "handle_check.h"


/***************  ȫ���� ���壬 ���� *****************/

typedef	struct	_MEMUNITPOOL
{
	LPVOID				this;

	CRITICAL_SECTION	csUnit;
	WORD				wFlag;
	WORD				wLenUnit;	//ÿ����Ԫ�ĳ���
	WORD				wCntMax;	//��Ԫ���ܸ���
	WORD				wCntUsed;	//��ʹ�õĸ���
	WORD				wCntCur;	//��ǰ��λ��
	WORD				wTryEvery;	//

	LONG				nCntRef;

	//��Buffer�ĵ�ַ���ܳ���
	DWORD				dwLenBuf;	//BUFFER�ܳ���
	BYTE				pPoolBuf[1];//BUFFER��ַ������2����---1�����е�ַ��ֵ�б�2�����е�ַ�������ռ�

} MEMUNITPOOL;

#define	MU_HDR_SIZE		sizeof(LPVOID)
#define	MU_HDR_OFFSET	(MU_HDR_SIZE/sizeof(DWORD))

#define	MUPF_EXIT		0x8000
#define	MUPF_CLRUNIT	0x0001

/******************************************************/



// ********************************************************************
// ������
// ������
//	IN wCntMax-�ڴ浥Ԫ�ĸ���
//	IN wLenUnit-�ڴ浥Ԫ�ĳ���
// ����ֵ��
//	�ɹ������ط�0ֵ;ʧ�ܣ�����0 
// ��������������1�� �ڴ浥Ԫ�ؾ����������
// ����: 
// ********************************************************************
HPOOL	MUP_AllocPool( WORD wCntMax, WORD wLenUnit, BOOL fIsClearUnit, WORD wTryEvery, WORD wTryFirst )
{
	MEMUNITPOOL*	pMUP;
	DWORD			dwLenBuf;
	DWORD			dwLenAlloc;
	WORD			i;

	//�������
	if( wCntMax<2 )
	{
		wCntMax = 2;
	}
	if( wTryFirst > MAX_TRYFIRST )
	{
		wTryFirst = MAX_TRYFIRST;
	}
	if( wTryEvery > MAX_TRYEVERY )
	{
		wTryEvery = MAX_TRYEVERY;
	}
	//����: dwLenBuf---���е�Ԫ�Ŀռ��С�������룩�� 
	//      dwLenAlloc---�������Ҫ����Ŀռ��С�������룩
	wLenUnit = (WORD)(( (MU_HDR_SIZE + wLenUnit + 7)/4 ) * 4);
	dwLenBuf = wLenUnit * wCntMax;
	dwLenAlloc = dwLenBuf + sizeof(MEMUNITPOOL);
	dwLenAlloc = (( dwLenAlloc + 7)/4) * 4;
	//����
	for( i=0; i<wTryFirst; i++ )
	{
		pMUP = (MEMUNITPOOL*)malloc( dwLenAlloc );
		if( pMUP )
		{
			break;
		}
		Sleep( WAIT_TRYFIRST );		
	}
	if( !pMUP )
	{
		return NULL;
	}
	HANDLE_INIT( pMUP, dwLenAlloc );
	InitializeCriticalSection( &pMUP->csUnit );

	//������Ϣ
	pMUP->wLenUnit = wLenUnit;
	pMUP->wCntMax  = wCntMax;
	pMUP->dwLenBuf = dwLenBuf;
	pMUP->wTryEvery = wTryEvery;
	if( fIsClearUnit )
	{
		pMUP->wFlag |= MUPF_CLRUNIT;
	}

	return (HPOOL)pMUP;
}

// ********************************************************************
// ������
// ������
//	IN hMUP-�ڴ浥Ԫ�ؾ��
// ����ֵ��
//	��
// �����������ͷ�1�� �ڴ浥Ԫ�ؾ��
// ����: 
// ********************************************************************
void	MUP_FreePool( HPOOL hMUP )
{
	MEMUNITPOOL*		pMUP = (MEMUNITPOOL*)hMUP;
	WORD				i;
	DWORD*				pBufUnit_hdr;
	WORD				wLenUnit;
	WORD				wCntMax;

	//�����ȫ���
	if( !HANDLE_CHECK(pMUP) )
	{
		return ;
	}
	//
	pMUP->wFlag |= MUPF_EXIT;

	//�ȴ�
	for( i=0; i<100; i++ )
	{
		if( pMUP->nCntRef<=0 )
		{
			break;
		}
		Sleep( 2 );
	}

	//��ȫ�˳�
	if( pMUP->wFlag & MUPF_CLRUNIT )
	{
		memset( pMUP->pPoolBuf, 0, pMUP->dwLenBuf );
	}
	else
	{
		pBufUnit_hdr = (DWORD*)(pMUP->pPoolBuf);
		wLenUnit = pMUP->wLenUnit;
		wCntMax = pMUP->wCntMax;
		for( i=0; i<wCntMax; i++ )
		{
			*pBufUnit_hdr = 0;
			pBufUnit_hdr += wLenUnit;
		}
	}
	DeleteCriticalSection( &pMUP->csUnit );
	HANDLE_FREE( pMUP );
}

// ********************************************************************
// ������
// ������
//	IN hMUP-�ڴ浥Ԫ�ؾ��
// ����ֵ��
//	�ɹ������ط�0ֵ;ʧ�ܣ�����0 
// ��������������1�� �ڴ浥Ԫ���е� �ڴ浥Ԫ��������
// ����: 
// ********************************************************************
LPVOID	MUP_AllocUnit( HPOOL hMUP )
{
	MEMUNITPOOL*		pMUP = (MEMUNITPOOL*)hMUP;
	DWORD*				pBufUnit_hdr;
	LPBYTE				pPoolBuf;
	WORD				wCntMax;
	WORD				wCntCur;
	WORD				wLenUnit;
	WORD				i;

	//�����ȫ���
	if( HANDLE_F_FAIL(pMUP, MUPF_EXIT) )
	{
		return NULL;
	}
	pMUP->nCntRef ++;
	// �ȴ��пռ�
	wCntMax  = pMUP->wCntMax;
	for( i=0; (i<pMUP->wTryEvery) && !(pMUP->wFlag & MUPF_EXIT); i++ )
	{
		if( pMUP->wCntUsed<wCntMax )
		{
			break;
		}
		Sleep( WAIT_TRYEVERY );
	}
	if( (pMUP->wFlag & MUPF_EXIT) || (pMUP->wCntUsed>=wCntMax) )
	{
		pMUP->nCntRef --;
		return NULL;
	}

	//
	// Ѱ��û�б�ʹ�õ�Ԫ
	//
	pPoolBuf = pMUP->pPoolBuf;
	wLenUnit = pMUP->wLenUnit;
	EnterCriticalSection( &pMUP->csUnit );
	if( (pMUP->wFlag & MUPF_EXIT) || (pMUP->wCntUsed>=wCntMax) )
	{
		LeaveCriticalSection( &pMUP->csUnit );
		pMUP->nCntRef --;
		return 0;
	}
	wCntCur  = pMUP->wCntCur;
	//��λ����ǰλ��
	pBufUnit_hdr = (DWORD*)(pPoolBuf + wCntCur * wLenUnit);
	//�����Ҫ���ҿ���λ��
	if( *pBufUnit_hdr )
	{
		//��������
		for( i=wCntCur; !(pMUP->wFlag & MUPF_EXIT) && (i<wCntMax); i++ )
		{
			pBufUnit_hdr += wLenUnit;
			if( *pBufUnit_hdr==0 )
			{
				break;
			}
		}
		//�����еĻ����ͻ�ͷ��
		if( *pBufUnit_hdr )
		{
			for( i=0; !(pMUP->wFlag & MUPF_EXIT) && (i<wCntCur); i++ )
			{
				pBufUnit_hdr += wLenUnit;
				if( *pBufUnit_hdr==0 )
				{
					break;
				}
			}
		}

		//
		wCntCur = i+1;
	}
	else
	{
		wCntCur ++;
	}
	// ���
	if( *pBufUnit_hdr==0 )
	{
		//�����ڴ浥Ԫ��Ϣ
		if( pMUP->wFlag & MUPF_CLRUNIT )
		{
			memset( pBufUnit_hdr, 0, wLenUnit );
		}
		*pBufUnit_hdr = (DWORD)pMUP;

		//����POOL��Ϣ
		if( wCntCur>=wCntMax )
		{
			wCntCur = 0;
		}
		pMUP->wCntCur  = wCntCur;
		pMUP->wCntUsed ++;
	}
	LeaveCriticalSection( &pMUP->csUnit );

	pMUP->nCntRef --;
	return (LPVOID)(pBufUnit_hdr+MU_HDR_OFFSET);
}

// ********************************************************************
// ������
// ������
//	IN pBufUnit-�ڴ浥Ԫ���е� �ڴ浥Ԫ
// ����ֵ��
//	�ɹ������ط�0ֵ;ʧ�ܣ�����0 
// �����������ͷ�1�� �ڴ浥Ԫ���е� �ڴ浥Ԫ
// ����: 
// ********************************************************************
void	MUP_FreeUnit( LPVOID pBufUnit )
{
	MEMUNITPOOL*		pMUP;
	DWORD*				pBufUnit_hdr;

	//�������
	if( !pBufUnit )
	{
		return ;
	}
	pBufUnit_hdr = (DWORD*)( (DWORD)pBufUnit - MU_HDR_SIZE );
	pMUP = (MEMUNITPOOL*)(*pBufUnit_hdr);
	//�����ȫ���
	if( HANDLE_F_FAIL(pMUP, MUPF_EXIT) )
	{
		return ;
	}
	//�ͷ�
	pMUP->nCntRef ++;
	EnterCriticalSection( &pMUP->csUnit );

	//��ʼ�ͷ�
	if( pMUP->wFlag & MUPF_CLRUNIT )
	{
		memset( pBufUnit_hdr, 0, pMUP->wLenUnit );
	}
	else
	{
		*pBufUnit_hdr = 0;
	}
	//�����ǰ�ĵ�Ԫ�Ǳ�ʹ�õģ���������ͷŵĵ�Ԫ��Ϊ��ǰ��Ԫ
	if( *( (DWORD*)(pMUP->pPoolBuf + pMUP->wCntCur*pMUP->wLenUnit) ) )
	{
		pMUP->wCntCur  = (WORD)( ((DWORD)((LPBYTE)pBufUnit_hdr - pMUP->pPoolBuf))  /  pMUP->wLenUnit );
		if( pMUP->wCntCur>=pMUP->wCntMax )
		{
			pMUP->wCntCur = 0;
		}
	}
	pMUP->wCntUsed --;
	
	LeaveCriticalSection( &pMUP->csUnit );
	pMUP->nCntRef --;
}


