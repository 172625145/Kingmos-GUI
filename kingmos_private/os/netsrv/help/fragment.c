/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：各个内存BUFFER分片操作
版本号：  1.0.0
开发时期：2004-08-23
作者：    肖远钢
修改记录：
******************************************************/
#include <ewindows.h>
//"\inc_local"
#include "fragment.h"

BOOL	FgA_CopyToBuf( FRAGSETA* pFragSetA, OUT LPBYTE pData )
{
	FRAGMENT*	pFrag;
	FRAGMENT*	pFragEnd;
	LPBYTE		pCopy;

	//所有数据字节求和
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

