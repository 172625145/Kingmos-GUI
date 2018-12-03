/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：socket 辅助函数 
版本号：  2.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：

******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <endalign.h>


/***************  全局区 定义， 声明 *****************/

/******************************************************/


// ********************************************************************
// 声明：
// 参数：
//	IN wWord-
// 返回值：
//	返回
// 功能描述：主机字-->网络字顺序的主机存储 操作
// 引用: 
// ********************************************************************
WORD	htons( WORD wWord )
{
	return REORDER_S(wWord);
}
// ********************************************************************
// 声明：
// 参数：
//	IN wWord-
// 返回值：
//	返回
// 功能描述：网络字顺序的主机存储-->主机字 操作
// 引用: 
// ********************************************************************
WORD	ntohs( WORD wWord )
{
	return REORDER_S(wWord);
}
// ********************************************************************
// 声明：
// 参数：
//	IN dwWord-
// 返回值：
//	返回
// 功能描述：主机字-->网络字顺序的主机存储 操作
// 引用: 
// ********************************************************************
DWORD	htonl( DWORD dwWord )
{
	return REORDER_L(dwWord);
}
// ********************************************************************
// 声明：
// 参数：
//	IN dwWord-
// 返回值：
//	返回
// 功能描述：网络字顺序的主机存储-->主机字 操作
// 引用: 
// ********************************************************************
DWORD	ntohl( DWORD dwWord )
{
	return REORDER_L(dwWord);
}
// ********************************************************************
// 声明：
// 参数：
//	IN pBuf-
// 返回值：
//	返回
// 功能描述：主机字-->网络字顺序的主机存储 操作
// 引用: 
// ********************************************************************
WORD	phtons( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	WORD	wWord;
	
	pTmp = (LPBYTE)&wWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
#else
	pTmp[1] = pBuf[0];
	pTmp[0] = pBuf[1];
#endif
	return wWord;
}
// ********************************************************************
// 声明：
// 参数：
//	IN pBuf-
// 返回值：
//	返回
// 功能描述：网络字顺序的主机存储-->主机字 操作
// 引用: 
// ********************************************************************
WORD	pntohs( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	WORD	wWord;
	
	pTmp = (LPBYTE)&wWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
#else
	pTmp[1] = pBuf[0];
	pTmp[0] = pBuf[1];
#endif
	return wWord;
}
// ********************************************************************
// 声明：
// 参数：
//	IN pBuf-
// 返回值：
//	返回
// 功能描述：主机字-->网络字顺序的主机存储 操作
// 引用: 
// ********************************************************************
DWORD	phtonl( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	DWORD	dwWord;
	
	pTmp = (LPBYTE)&dwWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
	pTmp[2] = pBuf[2];
	pTmp[3] = pBuf[3];
#else
	pTmp[3] = pBuf[0];
	pTmp[2] = pBuf[1];
	pTmp[1] = pBuf[2];
	pTmp[0] = pBuf[3];
#endif
	return dwWord;
}
// ********************************************************************
// 声明：
// 参数：
//	IN pBuf-
// 返回值：
//	返回
// 功能描述：网络字顺序的主机存储-->主机字 操作
// 引用: 
// ********************************************************************
DWORD	pntohl( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	DWORD	dwWord;
	
	pTmp = (LPBYTE)&dwWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
	pTmp[2] = pBuf[2];
	pTmp[3] = pBuf[3];
#else
	pTmp[3] = pBuf[0];
	pTmp[2] = pBuf[1];
	pTmp[1] = pBuf[2];
	pTmp[0] = pBuf[3];
#endif
	return dwWord;
}

void	phtonsp( OUT LPBYTE pBufN, LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufN[0] = pBufH[0];
	pBufN[1] = pBufH[1];
#else
	pBufN[1] = pBufH[0];
	pBufN[0] = pBufH[1];
#endif
}
void	pntohsp( LPBYTE pBufN, OUT LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufH[0] = pBufN[0];
	pBufH[1] = pBufN[1];
#else
	pBufH[1] = pBufN[0];
	pBufH[0] = pBufN[1];
#endif
}
void	phtonlp( OUT LPBYTE pBufN, LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufN[0] = pBufH[0];
	pBufN[1] = pBufH[1];
	pBufN[2] = pBufH[2];
	pBufN[3] = pBufH[3];
#else
	pBufN[3] = pBufH[0];
	pBufN[2] = pBufH[1];
	pBufN[1] = pBufH[2];
	pBufN[0] = pBufH[3];
#endif
}
void	pntohlp( LPBYTE pBufN, OUT LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufH[0] = pBufN[0];
	pBufH[1] = pBufN[1];
	pBufH[2] = pBufN[2];
	pBufH[3] = pBufN[3];
#else
	pBufH[3] = pBufN[0];
	pBufH[2] = pBufN[1];
	pBufH[1] = pBufN[2];
	pBufH[0] = pBufN[3];
#endif
}
