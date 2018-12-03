/***************************************************
Copyright (R) 版权所有(C) 1998-2003微逻辑。保留所有权利。
***************************************************/

// 实现 OPENGL 中需要但是系统没有实现的函数
/*
序号	缺少的系统函数				状态
1		GetWindowDC					OK
2		GetGlyphOutline				NO
3		GetDeviceCaps				NO	--->OK
4		GetPaletteEntries			NO
5		SetDIBColorTable			NO
6		GetNearestPaletteIndex		NO
7		CreatePalette				NO
8		SetPixelV					NO
9		CreateIC					NO
10		strtok						OK
*/

#include "ewindows.h"
#include "egdi.h"
#include "gl/mlg_glstruct.h"
#include "gl/mlg_glthird.h"

/*	【描述】
	If GGO_BITMAP, GGO_GRAY2_BITMAP, GGO_GRAY4_BITMAP, GGO_GRAY8_BITMAP, or GGO_NATIVE 
is specified and the function succeeds, the return value is greater than zero; otherwise,
 the return value is GDI_ERROR. If one of these flags is specified and the buffer size 
 or address is zero, the return value specifies the required buffer size, in bytes. 

If GGO_METRICS is specified and the function fails, the return value is GDI_ERROR. 
*/
DWORD GetGlyphOutline(
  HDC hdc,             // handle to DC
  UINT uChar,          // character to query
  UINT uFormat,        // data format
  LPGLYPHMETRICS lpgm, // glyph metrics
  DWORD cbBuffer,      // size of data buffer
  LPVOID lpvBuffer,    // data buffer
  CONST MAT2 *lpmat2   // transformation matrix
)
{
	return GDI_ERROR;
}

/*	【描述】
The GetDeviceCaps function retrieves device-specific information for the specified device.
*/
/*  remove by lilin
int GetDeviceCaps(
  HDC hdc,     // handle to DC
  int nIndex   // index of capability
)
{
	return 0;
}
*/

/*	【描述】
The GetPaletteEntries function retrieves a specified range of palette entries from the 
given logical palette.
*/
UINT GetPaletteEntries(
  HPALETTE hpal,        // handle to logical palette
  UINT iStartIndex,     // first entry to retrieve
  UINT nEntries,        // number of entries to retrieve
  PALETTEENTRY *lppe   // array that receives entries
)
{
	return 0;
}

/*	【描述】
The SetDIBColorTable function sets RGB (red, green, blue) 
color values in a range of entries in the color table of 
the DIB that is currently selected into a specified device context.
*/
/*	remove by lilin
UINT SetDIBColorTable(
  HDC hdc,               // handle to DC
  UINT uStartIndex,      // color table index of first entry
  UINT cEntries,         // number of color table entries
  CONST RGBQUAD *pColors // array of color table entries
)
{
	return 0;
}
*/

/*	【描述】
The GetNearestPaletteIndex function retrieves the index for the 
entry in the specified logical palette most closely matching a 
specified color value. 
*/
UINT GetNearestPaletteIndex(
	HPALETTE hpal,     // handle to logical palette
	COLORREF crColor   // color to be matched
)
{
	return 0;
}

/*	【描述】
The CreatePalette function creates a logical palette.
*/
HPALETTE CreatePalette(
	CONST	LOGPALETTE *lplgpl   // logical palette
)
{
	return NULL;
}

/*	【描述】
The SetPixelV function sets the pixel at the specified 
coordinates to the closest approximation of the specified 
color. The point must be in the clipping region and the
 visible part of the device surface. 
*/
/*  remove by lilin
BOOL SetPixelV(
	HDC hdc,           // handle to device context
	int X,             // x-coordinate of pixel
	int Y,             // y-coordinate of pixel
	COLORREF crColor   // new pixel color
)
{
	return FALSE;
}
*/

/*	【描述】
The CreateIC function creates an information context for 
the specified device. The information context provides a
 fast way to get information about the device without 
 creating a device context (DC). However, GDI drawing 
 functions cannot accept a handle to an information context. 
*/
HDC CreateIC(
  LPCTSTR lpszDriver,	// driver name
  LPCTSTR lpszDevice,	// device name
  LPCTSTR lpszOutput,	// port or file name
  int	  iVoid			// optional initialization data
)
{
	if ( 0==strcmp(lpszDriver,"display") )	// display
	{
		HDC hdc = GetDC( NULL );

		return hdc;
	}
	return NULL;

}

/*	【描述】
Find the next token in a string.
*/
/******************************************************************
声明：char *strtok( char *strToken, const char *strDelimit  )
参数：
	IN char *strToken	- String containing token or tokens.
	IN const char *strDelimit - Set of delimiter characters. 
返回值：
	在strToken字符串中寻找strDelimit：
	Returns a pointer to the next token found in strToken;
	They return NULL when no more tokens are found.
功能描述：无
*******************************************************************/
int strSearchToken( char *lpSource, const char *lpToken )	//	strtok辅助函数
{
	int iPointer = 0;
	char *lpTempToken = (char *)lpToken;

	while(1)
	{
		if ( *lpSource != *lpTempToken )
		{	// 字符不相等
			lpTempToken++;
			if (*lpTempToken==0)
			{	// lpTempToken结束了
				lpSource++;
				iPointer++;
				if (*lpSource)
				{	// 字符有效
					lpTempToken = (char *)lpToken;
				}
				else
				{	// 字符串结束
					break ;
				}
			}
		}
		else
		{
			*lpSource = 0;
			break ;
		}
	}

	return iPointer;
}
//	strtok辅助函数
char *my_strtok( char *strToken, const char *strDelimit )
{
	static	char *lpstrStatic = NULL;
	static	char *lpOristr = NULL;
	int	iPointer = 0;

	if (strToken)
	{	// 指针非空，说明重新一个strtok的操作，初始化所有的变量
		if (!strDelimit)
		{	// 如果要查找的字串为空，返回NULL
			return NULL;
		}

		lpstrStatic = strToken;
	}
	else
	{	// 指针为空，说明在进行同一个查找操作
		lpstrStatic ++;

		if (!strDelimit||!lpstrStatic)
		{	// 如果要查找的字串或者主串为空，返回NULL
			return NULL;
		}
		if (*lpstrStatic==0)
		{	// 字符串结束
			return NULL;
		}
	}

	lpOristr = lpstrStatic;
	iPointer = strSearchToken( lpstrStatic, strDelimit );
	lpstrStatic += iPointer;

	return lpOristr;
}

char *strtok( char *strToken, const char *strDelimit )
{
	char *lpstrRet = NULL;

	while( 1 )
	{
		lpstrRet = my_strtok( strToken, strDelimit );
		if ( !lpstrRet )
		{	// 返回为空
			return NULL;
		}
		else
		{
			if (*lpstrRet==0)
			{
				continue;
			}
			else
			{
				return lpstrRet;
			}
		}
	}
	return NULL;
}










