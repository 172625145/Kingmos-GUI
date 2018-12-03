/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：unicode-utf8 转换
版本号：1.0.0
开发时期：2005-02-24
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <estring.h>
#include "unicode.h"
/*
Char. number range  |        UTF-8 octet sequence
      (hexadecimal)    |              (binary)
   --------------------+---------------------------------------------
   0000 0000-0000 007F | 0xxxxxxx
   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

*/

static DWORD Create( DWORD dwReserver );
static BOOL Delete( DWORD dwHandle );
//utf8->unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar );
// unicode->utf8
static int GetUTF8( DWORD dwHandle,
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


const CODE_PAGE_DRV utf8Drv = 
{
	Create,
	Delete,
	GetUnicode,
	GetUTF8,
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

//utf8 -> unicode
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar )
{
	int cchSaveWideChar = cchWideChar;

	while( cbMultiByte && cchWideChar )
	{
		
		BYTE c = *lpMultiByteStr;

		if( (c & 0x80) == 0 )
		{  //单一字符（0000 0000-0000 007F | 0xxxxxxx）
			*lpWideCharStr++ = c;
			cchWideChar--;
			lpMultiByteStr++;
			cbMultiByte--;
		}
		else if( (c & 0xe0 ) == 0xe0 )
		{	//三字节编码 (0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx)
			if( cbMultiByte >= 3 )
			{
				*lpWideCharStr++ = ( (WORD)( *lpMultiByteStr & 0x0f ) << 12 ) | 
								   ( (WORD)( *(lpMultiByteStr+1) & 0x3f ) << 6 ) |
								   ( (WORD)( *(lpMultiByteStr+2) & 0x3f ) );
				cchWideChar--;
				lpMultiByteStr += 3;
				cbMultiByte -= 3;
			}
			else
				cbMultiByte = 0;
		}
		else if( (c & 0xc0 ) == 0xc0 )
		{	//双字节编码（0000 0080-0000 07FF | 110xxxxx 10xxxxxx）
			if( cbMultiByte >= 2 )
			{
				*lpWideCharStr++ = ( (WORD)( *lpMultiByteStr & 0x1f ) << 6 ) | 
								   ( (WORD)( *(lpMultiByteStr+1) & 0x3f ) );
				cchWideChar--;
				lpMultiByteStr += 2;
				cbMultiByte -= 2;
			}
			else
				cbMultiByte = 0;
		}
		else if( (c & 0xf0 ) == 0xf0 )
		{	//四字节编码0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			ASSERT( 0 );
			lpMultiByteStr++;
			cbMultiByte--;
		}
	}
	return cchSaveWideChar - cchWideChar;
}

// unicode -> utf8
static int GetUTF8( DWORD dwHandle,
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

		if( wc < 80 )
		{	////单一字符（0000 0000-0000 007F | 0xxxxxxx）
			*lpMultiByteStr++ = (BYTE)wc;
			cbMultiByte--;
		}
		else if( cbMultiByte >= 3 )
		{
			//三字节编码 (0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx)
			*lpMultiByteStr++ = ( (wc & 0xf000) >> 12 ) | 0xe0;
			*lpMultiByteStr++ = ( (wc & 0x0fc0) >> 6 ) | 0x80;
			*lpMultiByteStr++ = ( (wc & 0x003f) ) | 0x80;
			cbMultiByte -= 3;
		}
		else
			break;
	}
	return cbSaveMultiByte - cbMultiByte;
}
//统计需要的字符数
static int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar )
{
	int iCount = 0;
	if( IsWideChar )
	{   // wide-char
		LPCWSTR lpWideCharStr = (LPCWSTR)lpStr;
		int cchWideChar = cbCountLen;

		while( cchWideChar )
		{
			WCHAR wc = *lpWideCharStr++;
			cchWideChar--;

			if( wc < 80 )
			{	//单一字符（0000 0000-0000 007F | 0xxxxxxx）
				iCount++;
			}
			else
			{
				iCount += 3;
			}
		}
	}
	else
	{   // multibytes
		LPBYTE lpMultiByteStr = (LPBYTE)lpStr;
		int cbMultiByte = cbCountLen;
		while( cbMultiByte > 0 )
		{
			BYTE c = *lpMultiByteStr;
			
			if( (c & 0x80) == 0 )
			{   //单一字符（0000 0000-0000 007F | 0xxxxxxx）
				iCount++;
				lpMultiByteStr++;
				cbMultiByte--;
			}
			else if( (c & 0xe0 ) == 0xe0 )
			{	//三字节编码 (0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx)
				if( cbMultiByte >= 3 )
				{
					iCount++;
					lpMultiByteStr += 3;
					cbMultiByte -= 3;
				}
				else
					cbMultiByte = 0;
			}
			else if( (c & 0xc0 ) == 0xc0 )
			{	//双字节编码（0000 0080-0000 07FF | 110xxxxx 10xxxxxx）
				if( cbMultiByte >= 2 )
				{
					iCount++;
					lpMultiByteStr += 2;
					cbMultiByte -= 2;
				}
				else
					cbMultiByte = 0;
			}
			else if( (c & 0xf0 ) == 0xf0 )
			{	//四字节编码0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
				ASSERT( 0 ); // not support
				iCount+=2;
				lpMultiByteStr++;
				cbMultiByte--;
			}
		}
	}
	return iCount;
}

