/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：可变BUFFER操作
版本号：  1.0.0
开发时期：2004-06-10
作者：    肖远钢
修改记录：
******************************************************/
#ifdef XYG_PC_PRJ
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#else
#include <ewindows.h>

#endif
#include "varbuf.h"
#include "pcfile.h"

/***************  全局区 定义， 声明 *****************/
/******************************************************/


// ********************************************************************
// 声明：
// 参数：
//	IN lpDealData-待处理的数据缓冲处理结构
//	IN dwFlag-分配标志
//	IN dwLenGrow-每次增长的长度
//	IN dwLenMaxAlloc-最大分配的数据的长度
//	IN dwLenTotal-当前分配的数据的长度
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：分配 数据缓冲处理结构的BUFFER
// 引用: 
// ********************************************************************
BOOL	VarBuf_Alloc( DATA_DEAL* lpDealData, DWORD dwFlag, DWORD dwLenGrow, DWORD dwLenMaxAlloc, DWORD dwLenTotal )
{
	//分配
	lpDealData->lpData = (LPBYTE)malloc( dwLenTotal );
	if( !lpDealData->lpData )
	{
		return FALSE;
	}
	//初始化
	if( dwFlag & DDF_ZEROALLOC )
	{
		memset( lpDealData->lpData, 0, dwLenTotal );
	}
	lpDealData->dwFlag = dwFlag;
	if( !dwLenGrow || dwLenGrow<DDGROW_DEFAULT )
	{
		lpDealData->dwLenGrow = DDGROW_DEFAULT;
	}
	else
	{
		lpDealData->dwLenGrow = dwLenGrow;
	}
	lpDealData->dwLenMaxAlloc = dwLenMaxAlloc;
	lpDealData->dwLenTotal = dwLenTotal;

	lpDealData->dwLenDealed = 0;
	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpDealData-待处理的数据缓冲处理结构
//	IN dwLenGrow-每次增长的长度
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：自动重分配 数据缓冲处理结构的BUFFER
// 引用: 
// ********************************************************************
BOOL	VarBuf_Grow( DATA_DEAL* lpDealData, DWORD dwLenGrow )
{
	LPBYTE			lpData;
	DWORD			dwLenTotal;

	//检查属性
	if( !(lpDealData->dwFlag & DDF_GROW) )
	{
		return FALSE;
	}

	//
	//1. 计算空间
	//
	//首先进行最大的分配
	if( dwLenGrow < lpDealData->dwLenGrow )
	{
		dwLenTotal = lpDealData->dwLenGrow + lpDealData->dwLenTotal;
	}
	else
	{
		dwLenTotal = dwLenGrow + lpDealData->dwLenTotal;
	}
	if( lpDealData->dwFlag & DDF_MAXALLOC  )
	{
		//如果过大的话，看是不是可以 选择适当的分配
		if( dwLenTotal > lpDealData->dwLenMaxAlloc )
		{
			if( dwLenGrow < lpDealData->dwLenGrow )
			{
				return FALSE;
			}
			//选择进行适当的分配
			dwLenTotal = dwLenGrow + lpDealData->dwLenTotal;
			if( dwLenTotal > lpDealData->dwLenMaxAlloc )
			{
				return FALSE;
			}
		}
	}
	//
	//2. 分配
	//
	lpData = (LPBYTE)malloc( dwLenTotal );
	if( !lpData )
	{
		return FALSE;
	}
	if( lpDealData->dwFlag & DDF_ZEROALLOC )
	{
		memset( lpData, 0, dwLenTotal );
	}


	//
	//3. 保存
	//
	if( lpDealData->lpData )
	{
		memcpy( lpData, lpDealData->lpData, lpDealData->dwLenTotal );
		free( lpDealData->lpData );
	}
	lpDealData->lpData = lpData;
	lpDealData->dwLenTotal = dwLenTotal;

	return TRUE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN lpDealData-待处理的数据缓冲处理结构
// 返回值：
//	
// 功能描述：释放 数据缓冲处理结构的BUFFER
// 引用: 
// ********************************************************************
void	VarBuf_Free( DATA_DEAL* lpDealData )
{
	if( lpDealData->lpData )
	{
		free( lpDealData->lpData );
	}
	lpDealData->lpData = NULL;
	lpDealData->dwLenDealed = 0;
	lpDealData->dwLenTotal = 0;
}

BOOL	VarBuf_ReadFile( LPCTSTR pszFileName, OUT DATA_DEAL* lpDealDataRecv )
{
	HANDLE			hFile;
	DWORD			dwSize;
	DWORD			dwTmp;

	//打开文件
	hFile = ver_CreateFile( pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile==INVALID_HANDLE_VALUE )
	{
		return FALSE;
	}
	//获取文件大小
	dwSize = ver_GetFileSize( hFile, NULL );
	if( !dwSize )
	{
		ver_CloseHandle( hFile );
		return FALSE;
	}
	//alloc buffer
	if( !VarBuf_Alloc( lpDealDataRecv, DDF_GROW, 0, 0, dwSize ) )
	{
		ver_CloseHandle( hFile );
		//表示失败
		return FALSE;
	}
	//读取文件
	if( !ver_ReadFile( hFile, lpDealDataRecv->lpData, lpDealDataRecv->dwLenTotal, &dwTmp, NULL ) )
	{
		ver_CloseHandle( hFile );
		VarBuf_Free( lpDealDataRecv );
		//表示失败
		return FALSE;
	}
	ver_CloseHandle( hFile );
	lpDealDataRecv->dwLenTotal = dwTmp;
	return TRUE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN lpDealData-待处理的数据缓冲处理结构
//	OUT pbRead-保存解码后的数据内容
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：从数据缓冲处理结构的BUFFER中，解码 BYTE 类型的数据
// 引用: 
// ********************************************************************
BOOL	VarDec_Byte( DATA_DEAL* lpDealData, OUT BYTE* pbRead )
{
	//参数检查
	if( lpDealData->dwLenDealed >= lpDealData->dwLenTotal )
	{
		return FALSE;
	}
	//读取数据，并移动指针到下一个位置
	*pbRead = *(lpDealData->lpData+lpDealData->dwLenDealed);
	lpDealData->dwLenDealed++;
	return TRUE;
}
// ********************************************************************
// 声明：
// 参数：
//	IN lpDealData-待处理的数据缓冲处理结构
//	OUT pbRead-保存解码后的数据内容
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：从数据缓冲处理结构的BUFFER中，解码 BYTE 类型的数据，但不自动移动
// 引用: 
// ********************************************************************
BOOL	VarPeek_Byte( DATA_DEAL* lpDealData, OUT BYTE* pbRead )
{
	//参数检查
	if( lpDealData->dwLenDealed >= lpDealData->dwLenTotal )
	{
		return FALSE;
	}
	*pbRead = *(lpDealData->lpData+lpDealData->dwLenDealed);
	return TRUE;
}
// ********************************************************************
// 声明：
// 参数：
//	IN OUT lpDealData-待处理的数据缓冲处理结构
//	IN bWrite-指定编码的数据内容
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：向数据缓冲处理结构的BUFFER中，编码 BYTE 类型的数据
// 引用: 
// ********************************************************************
BOOL	VarEnc_Byte( DATA_DEAL* lpDealData, BYTE bWrite )
{
	//参数检查
	if( (lpDealData->dwLenDealed+1) > lpDealData->dwLenTotal )
	{
		if( !VarBuf_Grow(lpDealData, 1) )
		{
			//表示失败
			return FALSE;
		}
	}
	//读取数据，并移动指针到下一个位置
	*(lpDealData->lpData+lpDealData->dwLenDealed) = bWrite;
	lpDealData->dwLenDealed++;
	return TRUE;
}
// ********************************************************************
// 声明：
// 参数：
//	IN OUT lpDealData-待处理的数据缓冲处理结构
//	IN pBufWrite-指定编码的数据内容
//	IN dwLenWrite-指定编码的数据长度
// 返回值：
//	成功，返回TRUE;失败，返回FALSE 
// 功能描述：向数据缓冲处理结构的BUFFER中，编码 BUFFER 类型的数据
// 引用: 
// ********************************************************************
BOOL	VarEnc_Buffer( DATA_DEAL* lpDealData, LPBYTE pBufWrite, DWORD dwLenWrite )
{
	//参数检查
	if( dwLenWrite==0 )
	{
		return TRUE;
	}
	if( (lpDealData->dwLenDealed+dwLenWrite) > lpDealData->dwLenTotal )
	{
		if( !VarBuf_Grow(lpDealData, dwLenWrite) )
		{
			//表示失败
			return FALSE;
		}
	}
	//
	memcpy( lpDealData->lpData+lpDealData->dwLenDealed, pBufWrite, dwLenWrite );
	lpDealData->dwLenDealed += dwLenWrite;
	return TRUE;
}
