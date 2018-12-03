/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵���������ڴ�BUFFER��Ƭ����
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-08-23
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
//"\inc_local"
#include "fragment.h"

BOOL	FgA_CopyToBuf( FRAGSETA* pFragSetA, OUT LPBYTE pData )
{
	FRAGMENT*	pFrag;
	FRAGMENT*	pFragEnd;
	LPBYTE		pCopy;

	//���������ֽ����
	pFrag = pFragSetA->pFrag;
	pFragEnd = pFrag + pFragSetA->nCnt;
	pCopy = pData;
	for( ; (pFrag<pFragEnd) && pFrag->pData; pFrag++ )
	{
		memcpy( pCopy, pFrag->pData, pFrag->nLen );
		pCopy += pFrag->nLen;
	}

	return TRUE;
}

BOOL	FgD_CopyToBuf( FRAGSETD* pFragSetD, OUT LPBYTE pData )
{
	FRAGMENT*	pFrag;
	LPBYTE		pCopy = pData;
	LONG		k;

	k = pFragSetD->nCntUsed;
	pFrag = &(pFragSetD->pFrag[k-1]);
	for( ; k && pFrag->pData; k--, pFrag-- )
	{
		memcpy( pCopy, pFrag->pData, pFrag->nLen );
		pCopy += pFrag->nLen;
	}
	return TRUE;
}

