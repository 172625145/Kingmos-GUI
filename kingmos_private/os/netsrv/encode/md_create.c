/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����MD ��������ܵķ�װ
�汾�ţ�1.0.0
�������ڣ�2004-10-09
���ߣ�ФԶ��
�޸ļ�¼��
******************************************************/
#include "encode.h"

void	CreateMD5_Ex( FRAGSETA* pFragSetA, OUT LPBYTE pBufDigest )
{
	MD5_CTX		ctx;
	FRAGMENT*	pFrag;
	FRAGMENT*	pFragEnd;

	//��ʼ��
	MD5Init( &ctx );
	//������������
	pFrag = pFragSetA->pFrag;
	pFragEnd = pFrag + pFragSetA->nCnt;
	for( ; (pFrag<pFragEnd) && (pFrag->pData) && pFrag->nLen; pFrag++ )
	{
		MD5Update( &ctx, pFrag->pData, pFrag->nLen );
	}
	//�ó����
	MD5Final( pBufDigest, &ctx );
}


