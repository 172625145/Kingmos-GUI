/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：unicode-1252 转换
版本号：1.0.0
开发时期：2005-10-05
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <unicode.h>

static DWORD Create( DWORD dwReserver );
static BOOL Delete( DWORD dwHandle );
//utf8->unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar );
// unicode->utf8
static int Get1252( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar );
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar );


const CODE_PAGE_DRV CP_1252Drv = 
{
	Create,
	Delete,
	GetUnicode,
	Get1252,
	CountBufferSize
};

static DWORD Create( DWORD dwReserver )
{
	return 1;
}

static BOOL Delete( DWORD dwHandle )
{
	return TRUE;
}

//1252 -> unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar )
{
	int cchSaveWideChar = cchWideChar;

	while( cbMultiByte && cchWideChar )
	{
		*lpWideCharStr++ = *lpMultiByteStr++;
		cchWideChar--;
		cbMultiByte--;
	}

	return cchSaveWideChar - cchWideChar;
}

// unicode -> 1252
static int Get1252( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar )
{
	int cbSaveMultiByte = cbMultiByte;


	while( cbMultiByte && cchWideChar )
	{		
		WCHAR wc = *lpWideCharStr++;
		cchWideChar--;

		if( wc <= 0xff )
		{	////单一字符
			*lpMultiByteStr++ = (BYTE)wc;
			cbMultiByte--;
		}
		else
		{  // 无法识别
			*lpMultiByteStr++ = (BYTE)'?';
			cbMultiByte--;
		}
	}
	return cbSaveMultiByte - cbMultiByte;
}

//统计需要的字符数
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar )
{
	return cbCountLen;
}

