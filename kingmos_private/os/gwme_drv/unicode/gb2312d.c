/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：unicode-gb2312 转换
版本号：1.0.0
开发时期：2003-05-16
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <estring.h>
#include "unicode.h"
#include "gb2312c.c"


static DWORD Create( DWORD dwReserver );
static BOOL Delete( DWORD dwHandle );
static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar );
static int GetChar( DWORD dwHandle,
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


const CODE_PAGE_DRV gb2312Drv = 
{
	Create,
	Delete,
	GetUnicode,
	GetChar,
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

static DWORD GetUnicode( DWORD dwHandle,
				         LPCSTR lpMultiByteStr,
				         int cbMultiByte,  
				         LPWSTR lpWideCharStr,
				         int cchWideChar )
{
	int cchSaveWideChar = cchWideChar;
	while( cbMultiByte && cchWideChar )
	{
		BYTE c = *lpMultiByteStr++;
		cbMultiByte--;

		if( c < 0x80 )
		{
			*lpWideCharStr++ = c;
			cchWideChar--;
			continue;
		}
	    else if( cbMultiByte >= 1 )
		{
			 BYTE c0 = c & 0x7F;

			 if( ( c0 >= 0x21 && c0 <= 0x29 ) || 
				 ( c0 >= 0x30 && c0 <= 0x77 ) ) 
			 {
	 			 BYTE c1 = ( *lpMultiByteStr & 0x7F );

				 if( c1 >= 0x21 && c1 < 0x7f )
				 {
					 UINT idx = 94 * (c0 - 0x21) + (c1 - 0x21);
					 WCHAR wc = 0xffff;
					 if( idx < 1410 )
					 {
						 if( idx < 831 )
						     wc = gb2312_2uni_page21[idx];
					 } 
					 else 
					 {
						 if( idx < 8178 )
							 wc = gb2312_2uni_page30[idx-1410];
					 }
					 if( wc != 0xffff )
					 {
						 *lpWideCharStr++ = wc;
						 cchWideChar--;
						 cbMultiByte--;
						 lpMultiByteStr++;
						 continue;
					 }					 
				 }
			 }
		}
		*lpWideCharStr = c;
		cchWideChar--;
	}
	return cchSaveWideChar - cchWideChar;
}

/*
static int
gb2312_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (n >= 2) {
    const Summary16 *summary = NULL;
    if (wc >= 0x0000 && wc < 0x0460)
      summary = &gb2312_uni2indx_page00[(wc>>4)];
    else if (wc >= 0x2000 && wc < 0x2650)
      summary = &gb2312_uni2indx_page20[(wc>>4)-0x200];
    else if (wc >= 0x3000 && wc < 0x3230)
      summary = &gb2312_uni2indx_page30[(wc>>4)-0x300];
    else if (wc >= 0x4e00 && wc < 0x9cf0)
      summary = &gb2312_uni2indx_page4e[(wc>>4)-0x4e0];
    else if (wc >= 0x9e00 && wc < 0x9fb0)
      summary = &gb2312_uni2indx_page9e[(wc>>4)-0x9e0];
    else if (wc >= 0xff00 && wc < 0xfff0)
      summary = &gb2312_uni2indx_pageff[(wc>>4)-0xff0];
    if (summary) {
      unsigned short used = summary->used;
      unsigned int i = wc & 0x0f;
      if (used & ((unsigned short) 1 << i)) {
        unsigned short c;
        /// Keep in `used' only the bits 0..i-1. 
        used &= ((unsigned short) 1 << i) - 1;
        // Add `summary->indx' and the number of bits set in `used'.
        used = (used & 0x5555) + ((used & 0xaaaa) >> 1);
        used = (used & 0x3333) + ((used & 0xcccc) >> 2);
        used = (used & 0x0f0f) + ((used & 0xf0f0) >> 4);
        used = (used & 0x00ff) + (used >> 8);
        c = gb2312_2charset[summary->indx + used];
        r[0] = (c >> 8); r[1] = (c & 0xff);
        return 2;
      }
    }
    return RET_ILSEQ;
  }
  return RET_TOOSMALL;
}
*/

int GetChar( DWORD dwHandle,
			   LPCWSTR lpWideCharStr, 
			   int cchWideChar, 
			   LPSTR lpMultiByteStr, 
			   int cbMultiByte, 
			   LPCSTR lpDefaultChar, 
			   BOOL * lpUsedDefaultChar )
{
	const SUMMARY16 *lpSummary;
	int cbSaveMultiByte = cbMultiByte;


	while( cbMultiByte && cchWideChar )
	{		
		WCHAR wc = *lpWideCharStr++;
		cchWideChar--;

		if( wc >= 0x0000 && wc < 0x0460 )
			lpSummary = &gb2312_uni2indx_page00[(wc>>4)];
		else if( wc >= 0x2000 && wc < 0x2650 )
			lpSummary = &gb2312_uni2indx_page20[(wc>>4)-0x200];
		else if( wc >= 0x3000 && wc < 0x3230 )
			lpSummary = &gb2312_uni2indx_page30[(wc>>4)-0x300];
		else if( wc >= 0x4e00 && wc < 0x9cf0 )
			lpSummary = &gb2312_uni2indx_page4e[(wc>>4)-0x4e0];
		else if( wc >= 0x9e00 && wc < 0x9fb0 )
			lpSummary = &gb2312_uni2indx_page9e[(wc>>4)-0x9e0];
		else if( wc >= 0xff00 && wc < 0xfff0 )
			lpSummary = &gb2312_uni2indx_pageff[(wc>>4)-0xff0];
		else
		{  // not find
			lpSummary = NULL;
		}

		if( lpSummary && cbMultiByte >= 2)
		{
			WORD used = lpSummary->used;
			UINT i = wc & 0x0f;
			if( used & ((unsigned short) 1 << i) )
			{
				WORD c;
				// Keep in `used' only the bits 0..i-1.
				used &= ((unsigned short) 1 << i) - 1;
				// Add `lpSummary->indx' and the number of bits set in `used'.
				used = (used & 0x5555) + ((used & 0xaaaa) >> 1);
				used = (used & 0x3333) + ((used & 0xcccc) >> 2);
				used = (used & 0x0f0f) + ((used & 0xf0f0) >> 4);
				used = (used & 0x00ff) + (used >> 8);
				c = gb2312_2charset[lpSummary->indx + used];
				*lpMultiByteStr++ = (c >> 8) | 0x80;
				*lpMultiByteStr++ = (c & 0xff) | 0x80;
				cbMultiByte -= 2;
				//return 2;
				continue;
			}
		}
		//
		if( wc > 0xff )
		{
		    *lpMultiByteStr++ = (wc >> 8);
			cbMultiByte--;
		}
		if( cbMultiByte )
		{
		    *lpMultiByteStr++ = (wc & 0xff);
		    cbMultiByte--;
		}
		//
		//return 0;
	}
	return cbSaveMultiByte - cbMultiByte;
}

int CountBufferSize( DWORD dwHandle,
				     LPCVOID lpStr,
				     int cbCountLen,
			         BOOL IsWideChar )
{
	int iCount = 0;
	if( IsWideChar )
	{   // wide-char
		const SUMMARY16 *lpSummary;
		LPCWSTR lpWideCharStr = (LPCWSTR)lpStr;
		int cchWideChar = cbCountLen;

		while( cchWideChar )
		{
			WCHAR wc = *lpWideCharStr++;
			cchWideChar--;
			
			if( wc >= 0x0000 && wc < 0x0460 )
				lpSummary = &gb2312_uni2indx_page00[(wc>>4)];
			else if( wc >= 0x2000 && wc < 0x2650 )
				lpSummary = &gb2312_uni2indx_page20[(wc>>4)-0x200];
			else if( wc >= 0x3000 && wc < 0x3230 )
				lpSummary = &gb2312_uni2indx_page30[(wc>>4)-0x300];
			else if( wc >= 0x4e00 && wc < 0x9cf0 )
				lpSummary = &gb2312_uni2indx_page4e[(wc>>4)-0x4e0];
			else if( wc >= 0x9e00 && wc < 0x9fb0 )
				lpSummary = &gb2312_uni2indx_page9e[(wc>>4)-0x9e0];
			else if( wc >= 0xff00 && wc < 0xfff0 )
				lpSummary = &gb2312_uni2indx_pageff[(wc>>4)-0xff0];
			else
			{  // not find
				lpSummary = NULL;
			}
			
			if( lpSummary )
			{
				WORD used = lpSummary->used;
				UINT i = wc & 0x0f;
				if( used & ((unsigned short) 1 << i) )
				{
					WORD c;
					// Keep in `used' only the bits 0..i-1.
					used &= ((unsigned short) 1 << i) - 1;
					// Add `lpSummary->indx' and the number of bits set in `used'.
					used = (used & 0x5555) + ((used & 0xaaaa) >> 1);
					used = (used & 0x3333) + ((used & 0xcccc) >> 2);
					used = (used & 0x0f0f) + ((used & 0xf0f0) >> 4);
					used = (used & 0x00ff) + (used >> 8);
					c = gb2312_2charset[lpSummary->indx + used];

//					*lpMultiByteStr++ = (c >> 8);
//					*lpMultiByteStr++ = (c & 0xff);
//					cbMultiByte -= 2;
					//return 2;
					iCount += 2;
					continue;
				}
			}
			//
			//*lpMultiByteStr++ = (wc >> 8);
			//*lpMultiByteStr++ = (wc & 0xff);
			//cbMultiByte -= 2;
			iCount += 2;
			//
			//return 0;
		}
	}
	else
	{   // multibytes
		LPBYTE lpMultiByteStr = (LPBYTE)lpStr;
		int cbMultiByte = cbCountLen;
		while( cbCountLen > 0 )
		{
			BYTE c = *lpMultiByteStr++;
			cbMultiByte--;
			
			if( c < 0x80 )
			{
				iCount++;
//				*lpWideCharStr++ = c;
//				cchWideChar--;
				continue;
			}
			else if( cbMultiByte >= 1 )
			{
				BYTE c0 = c & 0x7F;
				
				if( ( c0 >= 0x21 && c0 <= 0x29 ) || 
					( c0 >= 0x30 && c0 <= 0x77 ) ) 
				{
					BYTE c1 = ( *lpMultiByteStr & 0x7F );
					
					if( c1 >= 0x21 && c1 < 0x7f )
					{
						UINT idx = 94 * (c0 - 0x21) + (c1 - 0x21);
						WCHAR wc = 0xffff;
						if( idx < 1410 )
						{
							if( idx < 831 )
								wc = gb2312_2uni_page21[idx];
						} 
						else 
						{
							if( idx < 8178 )
								wc = gb2312_2uni_page30[idx-1410];
						}
						if( wc != 0xffff )
						{
							iCount++;
//							*lpWideCharStr++ = wc;
//							cchWideChar--;
							cbMultiByte--;
							lpMultiByteStr++;
							continue;
						}					 
					}
				}
			}
			iCount++;
//			*lpWideCharStr = c;
//			cchWideChar--;
		}
		//return cchSaveWideChar - cchWideChar;
	}
	return iCount;
}
