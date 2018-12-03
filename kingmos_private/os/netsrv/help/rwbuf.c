/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：RW BUF 操作
版本号：  1.0.0
开发时期：2004-05-17
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
//"\inc_local"
#include "rwbuf.h"


/***************  全局区 定义， 声明 *****************/

/******************************************************/


// ********************************************************************
// 声明：
// 参数：
//	IN lpRWBuf-要操作的RWBUFFER
// 返回值：
//	
// 功能描述：设置接收缓冲
// 引用: 
// ********************************************************************
BOOL	RWBuf_Alloc( RWBUFFER* lpRWBuf, DWORD nLenNew, DWORD nLenMin )
{
	LPBYTE		pAlloc;

	//开始分配
	EnterCriticalSection( &lpRWBuf->csBufRW );
	if( lpRWBuf->pBufRW )
	{
		if( (nLenNew<nLenMin) || (nLenNew==lpRWBuf->dwLenRW) )
		{
			//如果已经分配，但分配超过最低底线，或者分配与当前大小相同，则直接返回
			LeaveCriticalSection( &lpRWBuf->csBufRW );
			return TRUE;
		}
		pAlloc = (LPBYTE)realloc( lpRWBuf->pBufRW, nLenNew );
	}
	else
	{
		pAlloc = (LPBYTE)malloc( nLenNew );
	}
	//保存分配信息
	if( pAlloc )
	{
		lpRWBuf->pBufRW = pAlloc;
		lpRWBuf->dwLenRW = nLenNew;

		lpRWBuf->dwCntRW = 0;
		lpRWBuf->dwReadRW = 0;
		lpRWBuf->dwWriteRW = 0;

		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return TRUE;
	}
	else
	{
		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return FALSE;
	}
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpRWBuf-要操作的RWBUFFER
// 返回值：
//	
// 功能描述：释放RWBUFFER
// 引用: 
// ********************************************************************
void	RWBuf_Free( RWBUFFER* lpRWBuf )
{
	//释放
	if( lpRWBuf->pBufRW )
	{
		free( lpRWBuf->pBufRW );
		lpRWBuf->pBufRW = NULL;
	}
	//
	DeleteCriticalSection( &lpRWBuf->csBufRW );
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT lpRWBuf-要操作的RWBUFFER
//	IN pData-要保存的数据
//	IN/OUT pdwLenData-指定pData的长度，并获取保存数据的长度
//	IN dwFlagWrite-指定保存的标志
// 返回值：
//	
// 功能描述：保存数据
// 引用: 
// ********************************************************************
BOOL	RWBuf_WriteData( RWBUFFER* lpRWBuf, LPBYTE pData, IN OUT DWORD* pdwLenData, DWORD dwFlagWrite )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_W;

	//参数检查
	dwLenAll = *pdwLenData;
	if( !pData )
	{
		return FALSE;
	}
	if( !dwLenAll )
	{
		//不需要 保存数据
		return TRUE;
	}
	//
	EnterCriticalSection( &lpRWBuf->csBufRW );
	//获取剩余空间
	nLen_W = lpRWBuf->dwLenRW - lpRWBuf->dwCntRW;
	if( !nLen_W )
	{
		//没有剩余空间 保存数据
		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return FALSE;
	}
	//计算可保存的空间 是否足够
	pOffset = pData;
	if( dwLenAll > nLen_W )
	{
		if( dwFlagWrite & RWF_W_ENOUGH )
		{
			//没有足够剩余空间 保存数据
			LeaveCriticalSection( &lpRWBuf->csBufRW );
			return FALSE;
		}
		dwLenAll = nLen_W;
	}
	*pdwLenData = dwLenAll;

	//存储到第1段
	if( lpRWBuf->dwWriteRW >= lpRWBuf->dwReadRW )
	{
		//判断第1段的长度
		nLen_W = (lpRWBuf->dwLenRW - lpRWBuf->dwWriteRW);
		if( nLen_W > dwLenAll )
		{
			nLen_W = dwLenAll;
		}
		//拷贝数据
		memcpy( lpRWBuf->pBufRW+lpRWBuf->dwWriteRW, pOffset, nLen_W );
		pOffset += nLen_W;
		dwLenAll -= nLen_W;
		//更新接收信息
		lpRWBuf->dwCntRW += nLen_W;
		lpRWBuf->dwWriteRW += nLen_W;
		if( lpRWBuf->dwWriteRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwWriteRW -= lpRWBuf->dwLenRW;
		}
	}
	//存储到第2段
	if( dwLenAll )
	{
		//拷贝数据
		memcpy( lpRWBuf->pBufRW+lpRWBuf->dwWriteRW, pOffset, dwLenAll );
		//更新接收信息
		lpRWBuf->dwCntRW += dwLenAll;
		lpRWBuf->dwWriteRW += dwLenAll;
		if( lpRWBuf->dwWriteRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwWriteRW -= lpRWBuf->dwLenRW;
		}
	}
	//
	LeaveCriticalSection( &lpRWBuf->csBufRW );

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT lpRWBuf-要操作的RWBUFFER
//	OUT pData-要保存的提取数据
//	IN/OUT pdwLenData-指定pData的长度，并保存 获取数据的长度
// 返回值：
//	
// 功能描述：提取数据
// 引用: 
// ********************************************************************
BOOL	RWBuf_ReadData( RWBUFFER* lpRWBuf, LPBYTE pData, IN OUT DWORD* pdwLenData )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_R;

	//参数检查
	dwLenAll = *pdwLenData;
	if( !pData || !dwLenAll )
	{
		//不需要 读取数据
		return FALSE;
	}
	//
	EnterCriticalSection( &lpRWBuf->csBufRW );
	//确定要读取的长度
	if( dwLenAll > lpRWBuf->dwCntRW )
	{
		dwLenAll = lpRWBuf->dwCntRW;
	}
	if( dwLenAll==0 )
	{
		//没有数据可读
		LeaveCriticalSection( &lpRWBuf->csBufRW );
		return FALSE;
	}
	pOffset = pData;
	*pdwLenData = dwLenAll;

	//读取数据从第1段
	if( lpRWBuf->dwWriteRW <= lpRWBuf->dwReadRW )
	{
		//判断第1段的长度
		nLen_R = (lpRWBuf->dwLenRW - lpRWBuf->dwReadRW);
		if( nLen_R > dwLenAll )
		{
			nLen_R = dwLenAll;
		}
		//拷贝数据
		memcpy( pOffset, lpRWBuf->pBufRW+lpRWBuf->dwReadRW, nLen_R );
		pOffset += nLen_R;
		dwLenAll -= nLen_R;
		//更新接收信息
		lpRWBuf->dwCntRW -= nLen_R;
		lpRWBuf->dwReadRW += nLen_R;
		if( lpRWBuf->dwReadRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwReadRW -= lpRWBuf->dwLenRW;
		}
	}
	//读取数据从第2段
	if( dwLenAll )
	{
		//拷贝数据
		memcpy( pOffset, lpRWBuf->pBufRW+lpRWBuf->dwReadRW, dwLenAll );
		//更新接收信息
		lpRWBuf->dwCntRW -= dwLenAll;
		lpRWBuf->dwReadRW += dwLenAll;
		if( lpRWBuf->dwReadRW >= lpRWBuf->dwLenRW )
		{
			lpRWBuf->dwReadRW -= lpRWBuf->dwLenRW;
		}
	}
	//
	LeaveCriticalSection( &lpRWBuf->csBufRW );

	return TRUE;
}


DWORD	RWBuf_GetCntLeft( RWBUFFER* lpRWBuf, BOOL fUseCri )
{
	DWORD			dwLenLeft;

	//
	if( fUseCri )
	{
		EnterCriticalSection( &lpRWBuf->csBufRW );
	}
	//
	dwLenLeft = lpRWBuf->dwLenRW - lpRWBuf->dwCntRW;
	//
	if( fUseCri )
	{
		LeaveCriticalSection( &lpRWBuf->csBufRW );
	}
	return dwLenLeft;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpRWBuf-要操作的RWBUFFER
// 返回值：
//	
// 功能描述：设置接收缓冲
// 引用: 
// ********************************************************************
BOOL	RWBuf_SetReadOffset( RWBUFFER* lpRWBuf, DWORD dwReadOffset, BOOL fUseCri )
{
	//
	if( fUseCri )
	{
		EnterCriticalSection( &lpRWBuf->csBufRW );
	}
	//
	if( dwReadOffset>lpRWBuf->dwCntRW )
	{
		dwReadOffset = lpRWBuf->dwCntRW;
	}
	//
	lpRWBuf->dwCntRW -= dwReadOffset;
	lpRWBuf->dwReadRW += dwReadOffset;
	if( lpRWBuf->dwReadRW >= lpRWBuf->dwLenRW )
	{
		lpRWBuf->dwReadRW -= lpRWBuf->dwLenRW;
	}
	//
	if( fUseCri )
	{
		LeaveCriticalSection( &lpRWBuf->csBufRW );
	}
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpRWBuf-要操作的RWBUFFER
// 返回值：
//	
// 功能描述：获取偏移后 读的位置 和 读行的长度
// 引用: 
// ********************************************************************
BOOL	RWBuf_GetReadBuf( RWBUFFER* lpRWBuf, DWORD dwReadOffset, LPBYTE* ppBufRead, DWORD* pdwLenRead )
{
	if( dwReadOffset>lpRWBuf->dwCntRW )
	{
		*ppBufRead = NULL;
		*pdwLenRead = 0;
		return FALSE;
	}
	//获取位置
	dwReadOffset += lpRWBuf->dwReadRW;
	if( dwReadOffset >= lpRWBuf->dwLenRW )
	{
		dwReadOffset -= lpRWBuf->dwLenRW;
	}
	*ppBufRead = lpRWBuf->pBufRW + dwReadOffset;
	if( dwReadOffset>lpRWBuf->dwWriteRW )
	{
		*pdwLenRead = lpRWBuf->dwLenRW - dwReadOffset;
	}
	else
	{
		*pdwLenRead = lpRWBuf->dwWriteRW - dwReadOffset;
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////

// ********************************************************************
// 声明：
// 参数：
//	IN lpRWBuf2-要操作的RWBUF2
// 返回值：
//	
// 功能描述：设置接收缓冲
// 引用: 
// ********************************************************************
BOOL	RWBuf2_Alloc( RWBUF2* lpRWBuf2, DWORD nLenNew, DWORD nLenMin )
{
	LPBYTE		pAlloc;

	//开始分配
	if( lpRWBuf2->pBufRW )
	{
		if( (nLenNew<nLenMin) || (nLenNew==lpRWBuf2->dwLenRW) )
		{
			//如果已经分配，但分配超过最低底线，或者分配与当前大小相同，则直接返回
			return TRUE;
		}
		pAlloc = (LPBYTE)realloc( lpRWBuf2->pBufRW, nLenNew );
	}
	else
	{
		pAlloc = (LPBYTE)malloc( nLenNew );
	}
	//保存分配信息
	if( pAlloc )
	{
		lpRWBuf2->pBufRW = pAlloc;
		lpRWBuf2->dwLenRW = nLenNew;

		lpRWBuf2->dwCntRW = 0;
		lpRWBuf2->dwReadRW = 0;
		lpRWBuf2->dwWriteRW = 0;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpRWBuf2-要操作的RWBUF2
// 返回值：
//	
// 功能描述：释放RWBUF2
// 引用: 
// ********************************************************************
void	RWBuf2_Free( RWBUF2* lpRWBuf2 )
{
	//释放
	if( lpRWBuf2->pBufRW )
	{
		free( lpRWBuf2->pBufRW );
		lpRWBuf2->pBufRW = NULL;
	}
}


// ********************************************************************
// 声明：
// 参数：
//	IN/OUT lpRWBuf2-要操作的RWBUF2
//	IN pData-要保存的数据
//	IN/OUT pdwLenData-指定pData的长度，并获取保存数据的长度
//	IN dwFlagWrite-指定保存的标志
// 返回值：
//	
// 功能描述：保存数据
// 引用: 
// ********************************************************************
BOOL	RWBuf2_WriteData( RWBUF2* lpRWBuf2, LPBYTE pData, IN OUT DWORD* pdwLenData, DWORD dwFlagWrite )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_W;

	//参数检查
	dwLenAll = *pdwLenData;
	if( !pData )
	{
		return FALSE;
	}
	if( !dwLenAll )
	{
		//不需要 保存数据
		return TRUE;
	}
	//获取剩余空间
	nLen_W = lpRWBuf2->dwLenRW - lpRWBuf2->dwCntRW;
	if( !nLen_W )
	{
		//没有剩余空间 保存数据
		return FALSE;
	}
	//计算可保存的空间 是否足够
	pOffset = pData;
	if( dwLenAll > nLen_W )
	{
		if( dwFlagWrite & RWF_W_ENOUGH )
		{
			//没有足够剩余空间 保存数据
			return FALSE;
		}
		dwLenAll = nLen_W;
	}
	*pdwLenData = dwLenAll;

	//存储到第1段---
	if( lpRWBuf2->dwWriteRW >= lpRWBuf2->dwReadRW )
	{
		//判断第1段的长度
		nLen_W = (lpRWBuf2->dwLenRW - lpRWBuf2->dwWriteRW);
		if( nLen_W > dwLenAll )
		{
			nLen_W = dwLenAll;
		}
		//拷贝数据
		memcpy( lpRWBuf2->pBufRW+lpRWBuf2->dwWriteRW, pOffset, nLen_W );
		pOffset += nLen_W;
		dwLenAll -= nLen_W;
		//更新接收信息
		lpRWBuf2->dwCntRW += nLen_W;
		lpRWBuf2->dwWriteRW += nLen_W;
		if( lpRWBuf2->dwWriteRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwWriteRW -= lpRWBuf2->dwLenRW;
		}
	}
	//存储到第2段---
	if( dwLenAll )
	{
		//拷贝数据
		memcpy( lpRWBuf2->pBufRW+lpRWBuf2->dwWriteRW, pOffset, dwLenAll );
		//更新接收信息
		lpRWBuf2->dwCntRW += dwLenAll;
		lpRWBuf2->dwWriteRW += dwLenAll;
		if( lpRWBuf2->dwWriteRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwWriteRW -= lpRWBuf2->dwLenRW;
		}
	}

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN/OUT lpRWBuf2-要操作的RWBUF2
//	OUT pData-要保存的提取数据
//	IN/OUT pdwLenData-指定pData的长度，并保存 获取数据的长度
// 返回值：
//	
// 功能描述：提取数据
// 引用: 
// ********************************************************************
BOOL	RWBuf2_ReadData( RWBUF2* lpRWBuf2, LPBYTE pData, IN OUT DWORD* pdwLenData )
{
	LPBYTE		pOffset;
	DWORD		dwLenAll;
	DWORD		nLen_R;

	//参数检查
	dwLenAll = *pdwLenData;
	if( !pData || !dwLenAll )
	{
		//不需要 读取数据
		return FALSE;
	}
	//确定要读取的长度
	if( dwLenAll > lpRWBuf2->dwCntRW )
	{
		dwLenAll = lpRWBuf2->dwCntRW;
	}
	if( dwLenAll==0 )
	{
		//没有数据可读
		return FALSE;
	}
	pOffset = pData;
	*pdwLenData = dwLenAll;

	//读取数据从第1段
	if( lpRWBuf2->dwWriteRW <= lpRWBuf2->dwReadRW )
	{
		//判断第1段的长度
		nLen_R = (lpRWBuf2->dwLenRW - lpRWBuf2->dwReadRW);
		if( nLen_R > dwLenAll )
		{
			nLen_R = dwLenAll;
		}
		//拷贝数据
		memcpy( pOffset, lpRWBuf2->pBufRW+lpRWBuf2->dwReadRW, nLen_R );
		pOffset += nLen_R;
		dwLenAll -= nLen_R;
		//更新接收信息
		lpRWBuf2->dwCntRW -= nLen_R;
		lpRWBuf2->dwReadRW += nLen_R;
		if( lpRWBuf2->dwReadRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwReadRW -= lpRWBuf2->dwLenRW;
		}
	}
	//读取数据从第2段
	if( dwLenAll )
	{
		//拷贝数据
		memcpy( pOffset, lpRWBuf2->pBufRW+lpRWBuf2->dwReadRW, dwLenAll );
		//更新接收信息
		lpRWBuf2->dwCntRW -= dwLenAll;
		lpRWBuf2->dwReadRW += dwLenAll;
		if( lpRWBuf2->dwReadRW >= lpRWBuf2->dwLenRW )
		{
			lpRWBuf2->dwReadRW -= lpRWBuf2->dwLenRW;
		}
	}

	return TRUE;
}



