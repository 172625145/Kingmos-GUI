/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：MD 加密与解密的封装
版本号：1.0.0
定版日期：2004-10-09
作者：肖远钢
修改记录：
******************************************************/
#include "encode.h"

void	CreateMD5_Ex( FRAGSETA* pFragSetA, OUT LPBYTE pBufDigest )
{
	MD5_CTX		ctx;
	FRAGMENT*	pFrag;
	FRAGMENT*	pFragEnd;

	//初始化
	MD5Init( &ctx );
	//计算所有数据
	pFrag = pFragSetA->pFrag;
	pFragEnd = pFrag + pFragSetA->nCnt;
	for( ; (pFrag<pFragEnd) && (pFrag->pData) && pFrag->nLen; pFrag++ )
	{
		MD5Update( &ctx, pFrag->pData, pFrag->nLen );
	}
	//得出结果
	MD5Final( pBufDigest, &ctx );
}


