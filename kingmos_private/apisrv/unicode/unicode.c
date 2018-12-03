/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：unicode 转换
版本号：1.0.0
开发时期：2003-05-16
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <enls.h>
#include "unicode.h"

typedef struct _CODE_PAGE_INFO
{
	UINT uiCodePageID;
	DWORD dwcpHandle;
	const CODE_PAGE_DRV * pCodePageDrv;
}CODE_PAGE_INFO, * PCODE_PAGE_INFO;

extern const CODE_PAGE_DRV gb2312Drv;
extern const CODE_PAGE_DRV utf8Drv;
extern const CODE_PAGE_DRV CP_936Drv;
extern const CODE_PAGE_DRV CP_1252Drv;
static CODE_PAGE_INFO cpTable[] = 
{
	{ CP_GB2312, 1, &gb2312Drv },
	{ 936, 1, &CP_936Drv },
	{ 1252, 1, &CP_1252Drv },
	{ CP_UTF8, 1, &utf8Drv },
	{ 0 , 0 }
};

// ********************************************************************
//声明：PCODE_PAGE GetCodePage( UINT uiCodePage )
//参数：
//	IN uiCodePage - 代码页
//返回值：
//	代码表
//功能描述：根据代码页得到对应的代码表
//引用: 
// ********************************************************************

static const CODE_PAGE_INFO * GetCodePage( UINT uiCodePage )
{
	int i;
	PCODE_PAGE_INFO pcp = cpTable;

	for( i = 0; i < sizeof( cpTable ) / sizeof( CODE_PAGE_INFO ); i++, pcp++ )
	{
		if( pcp->uiCodePageID == uiCodePage )
			return pcp;
	}
	return NULL;
}


// ********************************************************************
//声明：int WinCP_MultiByteToWideChar( UINT uiCodePage,
//                               DWORD dwFlags,
//                               LPCSTR lpMultiByteStr,
//                               int cbMultiByte,
//                               LPWSTR lpWideCharStr, 
//						         int cchWideChar
//						       );
//参数：
//	IN uiCodePage - 代码页
//	IN dwFlags - 无用，必须为0
//	IN lpMultiByteStr - 需要转化的字符串指针
//	IN cbMultiByte - lpMultiByteStr 指向的需要转化的字符个数，如果为-1，表示 lpMultiByteStr 以0结束
//	IN lpWideCharStr - 接受缓存
//	IN cchWideChar - lpWideCharStr 指向缓存的大小，如果为0，表示该函数仅仅返回需要的接受缓存的大小（没有实际的转换过程）
//返回值：
//	假如成功并且cchWideChar非0，返回实际写到 lpWideCharStr 的宽字符数（包含0结束）
//	假如成功并且cchWideChar为0，返回需要的接受宽字符数大小
//功能描述：
//	将一种字符格式转化为 unicode 格式
//引用: 
// ********************************************************************

int WINAPI WinCP_MultiByteToWideChar(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPWSTR lpWideCharStr, 
						int cchWideChar 
						)
{
	const CODE_PAGE_INFO * pcp = GetCodePage( uiCodePage );

	if( pcp )
	{
		if( cchWideChar )
		{
			if( cbMultiByte == -1 )
				cbMultiByte = strlen( lpMultiByteStr ) + 1;
			if( cbMultiByte )
			    return pcp->pCodePageDrv->lpGetUnicode( pcp->dwcpHandle, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
		}
		else
		{	//得到需要的缓存宽字符数
			return pcp->pCodePageDrv->lpCountBufferSize( pcp->dwcpHandle, 
				                                         (LPCVOID)lpMultiByteStr, 
														 cbMultiByte, 
														 FALSE );
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}

// ********************************************************************
//声明：int WINAPI WinCP_WideCharToMultiByte(
//						UINT uiCodePage, 
//						DWORD dwFlags, 
//						LPCWSTR lpWideCharStr, 
//						int cchWideChar, 
//						LPSTR lpMultiByteStr, 
//						int cbMultiByte, 
//						LPCSTR lpDefaultChar, 
//						BOOL * lpUsedDefaultChar 
//						)
//参数：
//	IN uiCodePage - 代码页
//	IN dwFlags - 无用，必须为0
//	IN lpWideCharStr - 需要转化的 unicode 字符指针
//	IN cchWideChar - lpWideCharStr 指向的需要转化的字符个数，如果为-1，表示 lpWideCharStr 以0结束
//	IN lpMultiByteStr - 接受缓存
//	IN cbMultiByte - lpMultiByteStr 指向缓存的大小，如果为0，表示该函数仅仅返回需要的接受缓存的大小（没有实际的转换过程）
//	IN lpDefaultChar - 如果一个字符无法转换，用该指针指定的字符代替
//	IN lpUsedDefaultChar - 用于指示 lpDefaultChar 标志是否在转化过程中用过，值为 TRUE/FALSE
//返回值：
//	假如成功并且cbMultiByte非0，返回实际写到 lpMultiByteStr的字节数（包含0结束）
//	假如成功并且cbMultiByte为0，返回需要的接受缓存大小
//功能描述：
//	将 unicode 字符转化为指定的一种字符格式
//引用: 
// ********************************************************************

int WINAPI WinCP_WideCharToMultiByte(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						int cchWideChar, 
						LPSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL * lpUsedDefaultChar 
						)
{
	const CODE_PAGE_INFO * pcp = GetCodePage( uiCodePage );

	if( pcp )
	{
		if( cbMultiByte )
		{
			if( cchWideChar == -1 )
				cchWideChar = wcslen( lpWideCharStr ) + 1;
			if( cchWideChar )
			    return pcp->pCodePageDrv->lpGetChar( pcp->dwcpHandle, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, NULL, NULL );
		}
		else
		{	//得到需要的缓存字符数
			return pcp->pCodePageDrv->lpCountBufferSize( pcp->dwcpHandle, lpWideCharStr, cchWideChar, TRUE );
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
	return 0;
}
