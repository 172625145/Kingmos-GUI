/***************************************************
Copyright (R) ��Ȩ����(C) 1998-2003΢�߼�����������Ȩ����
***************************************************/

// ʵ�� OPENGL ����Ҫ����ϵͳû��ʵ�ֵĺ���
/*
���	ȱ�ٵ�ϵͳ����				״̬
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
11      fprintf
*/

#include <ewindows.h>
#include <emmsys.h>
#include <egdi.h>
#include "gl/mlg_glstruct.h"
#include "gl/mlg_glthird.h"



/*	��������
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
	RETAILMSG(1, TEXT("gl_third.c--GetGlyphOutline()--ERROR!!!"));
	return GDI_ERROR;
}

/*	��������
The GetDeviceCaps function retrieves device-specific information for the specified device.
*/
/*
int GetDeviceCaps(
  HDC hdc,     // handle to DC
  int nIndex   // index of capability
)
{
	return 0;
}*/

/*	��������
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
	RETAILMSG(1, TEXT("gl_third.c--GetPaletteEntries()--ERROR!!!"));
	return 0;
}

/*	��������
The SetDIBColorTable function sets RGB (red, green, blue) 
color values in a range of entries in the color table of 
the DIB that is currently selected into a specified device context.
*//*
UINT SetDIBColorTable(
  HDC hdc,               // handle to DC
  UINT uStartIndex,      // color table index of first entry
  UINT cEntries,         // number of color table entries
  CONST RGBQUAD *pColors // array of color table entries
)
{
	return 0;
}*/

/*	��������
The GetNearestPaletteIndex function retrieves the index for the 
entry in the specified logical palette most closely matching a 
specified color value. 
*/
UINT GetNearestPaletteIndex(
	HPALETTE hpal,     // handle to logical palette
	COLORREF crColor   // color to be matched
)
{
	RETAILMSG(1, TEXT("gl_third.c--GetNearestPaletteIndex()--ERROR!!!"));
	return 0;
}

/*	��������
The CreatePalette function creates a logical palette.
*/
HPALETTE CreatePalette(
	CONST	LOGPALETTE *lplgpl   // logical palette
)
{
	RETAILMSG(1, TEXT("gl_third.c--CreatePalette()--ERROR!!!"));
	return NULL;
}

/*	��������
The SetPixelV function sets the pixel at the specified 
coordinates to the closest approximation of the specified 
color. The point must be in the clipping region and the
 visible part of the device surface. 
*//*
BOOL SetPixelV(
	HDC hdc,           // handle to device context
	int X,             // x-coordinate of pixel
	int Y,             // y-coordinate of pixel
	COLORREF crColor   // new pixel color
)
{
	return FALSE;
}*/

/*	��������
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
	else
	{
		RETAILMSG(1, TEXT("gl_third.c--CreateIC()--ERROR!!!"));
	}
	return NULL;

}

/*	��������
Find the next token in a string.
*/
/******************************************************************
������char *strtok( char *strToken, const char *strDelimit  )
������
	IN char *strToken	- String containing token or tokens.
	IN const char *strDelimit - Set of delimiter characters. 
����ֵ��
	��strToken�ַ�����Ѱ��strDelimit��
	Returns a pointer to the next token found in strToken;
	They return NULL when no more tokens are found.
������������
*******************************************************************/
static	int strSearchToken( char *lpSource, const char *lpToken )	//	strtok��������
{
	int iPointer = 0;
	char *lpTempToken = (char *)lpToken;

	while(1)
	{
		if ( *lpSource != *lpTempToken )
		{	// �ַ������
			lpTempToken++;
			if (*lpTempToken==0)
			{	// lpTempToken������
				lpSource++;
				iPointer++;
				if (*lpSource)
				{	// �ַ���Ч
					lpTempToken = (char *)lpToken;
				}
				else
				{	// �ַ�������
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
//	strtok��������
static	char *my_strtok( char *strToken, const char *strDelimit )
{
	static	char *lpstrStatic = NULL;
	static	char *lpOristr = NULL;
	int	iPointer = 0;

	if (strToken)
	{	// ָ��ǿգ�˵������һ��strtok�Ĳ�������ʼ�����еı���
		if (!strDelimit)
		{	// ���Ҫ���ҵ��ִ�Ϊ�գ�����NULL
			return NULL;
		}

		lpstrStatic = strToken;
	}
	else
	{	// ָ��Ϊ�գ�˵���ڽ���ͬһ�����Ҳ���
		lpstrStatic ++;

		if (!strDelimit||!lpstrStatic)
		{	// ���Ҫ���ҵ��ִ���������Ϊ�գ�����NULL
			return NULL;
		}
		if (*lpstrStatic==0)
		{	// �ַ�������
			return NULL;
		}
	}

	lpOristr = lpstrStatic;
	iPointer = strSearchToken( lpstrStatic, strDelimit );
	lpstrStatic += iPointer;

	return lpOristr;
}

char *___strtok( char *strToken, const char *strDelimit )
{
	char *lpstrRet = NULL;

	while( 1 )
	{
		lpstrRet = my_strtok( strToken, strDelimit );
		if ( !lpstrRet )
		{	// ����Ϊ��
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

//	The SetCursor function sets the cursor shape.
HCURSOR SetCursor(
  HCURSOR hCursor   // handle to cursor
)
{
	RETAILMSG(1, TEXT("gl_third.c--SetCursor()--ERROR!!!"));
	return NULL;
}

//	The SetCursorPos function moves the cursor to the specified screen coordinates.
//If the new coordinates are not within the screen rectangle set by the most recent
//ClipCursor function call, the system automatically adjusts the coordinates so that
//the cursor stays within the rectangle. 
BOOL SetCursorPos(
  int X,  // horizontal position
  int Y   // vertical position
)
{
	RETAILMSG(1, TEXT("gl_third.c--SetCursorPos()--ERROR!!!"));
	return 1;
}


//	The GetCursorPos function retrieves the cursor's position, in screen coordinates.
BOOL GetCursorPos(
  LPPOINT lpPoint   // cursor position
)
{
	RETAILMSG(1, TEXT("gl_third.c--GetCursorPos()--ERROR!!!"));
	return 0;
}

//	The ChangeDisplaySettings function changes the settings of the default display 
//device to the specified graphics mode.
LONG ChangeDisplaySettings(
  LPDEVMODEA lpDevMode,  // graphics mode
  DWORD dwflags         // graphics mode options
)
{
	RETAILMSG(1, TEXT("gl_third.c--ChangeDisplaySettings()--ERROR!!!"));
	return 0;
}

//	The EnumDisplaySettings function retrieves information about one of the graphics
//modes for a display device. To retrieve information for all the graphics modes of 
//a display device, make a series of calls to this function.
BOOL EnumDisplaySettings(
  LPCTSTR lpszDeviceName,  // display device
  DWORD iModeNum,          // graphics mode
  LPDEVMODEA lpDevMode      // graphics mode settings
)
{
	RETAILMSG(1, TEXT("gl_third.c--EnumDisplaySettings()--ERROR!!!"));
	return 0;
}




#define JOYERR_NOERROR        (0)                  /* no error */
#define JOYERR_PARMS          (JOYERR_BASE+5)      /* bad parameters */
#define JOYERR_NOCANDO        (JOYERR_BASE+6)      /* request not completed */
#define JOYERR_UNPLUGGED      (JOYERR_BASE+7)      /* joystick is unplugged */

#define UINT_PTR  UINT


//	The joyGetDevCaps function queries a joystick to determine its capabilities.
MMRESULT joyGetDevCaps(
  UINT  uJoyID,
  //LPJOYCAPS pjc,
  LPVOID pjc,
  UINT      cbjc
)
{
	RETAILMSG(1, TEXT("gl_third.c--joyGetDevCaps()--ERROR!!!"));
	return JOYERR_NOERROR;
}

//	The joyGetPosEx function queries a joystick for its position and button status.
MMRESULT joyGetPosEx(
  UINT uJoyID,
  //LPJOYINFOEX pji
  LPVOID pji
)
{
	RETAILMSG(1, TEXT("gl_third.c--joyGetPosEx()--ERROR!!!"));
	return JOYERR_NOERROR;
}

//	The joyReleaseCapture function releases the specified captured joystick.
MMRESULT joyReleaseCapture(
  UINT uJoyID  
)
{
	RETAILMSG(1, TEXT("gl_third.c--joyReleaseCapture()--ERROR!!!"));
	return JOYERR_NOERROR;
}

//	The joySetThreshold function sets the movement threshold of a joystick.
MMRESULT joySetThreshold(
  UINT uJoyID,     
  UINT uThreshold  
)
{
	RETAILMSG(1, TEXT("gl_third.c--joySetThreshold()--ERROR!!!"));
	return JOYERR_NOERROR;
}

//	The joySetCature function captures a joystick by causing its messages to
//be sent to the specified window.
MMRESULT joySetCapture(
  HWND hwnd,    
  UINT uJoyID,  
  UINT uPeriod, 
  BOOL fChanged 
)
{
	RETAILMSG(1, TEXT("gl_third.c--joySetThreshold()--ERROR!!!"));
	return JOYERR_NOERROR;
}

//	The GdiFlush function flushes the calling thread's current batch.
BOOL GdiFlush( VOID )
{
	RETAILMSG(1, TEXT("gl_third.c--GdiFlush()--ERROR!!!"));
	return 1;
}

//	The AppendMenu function appends a new item to the end of the specified 
//menu bar, drop-down menu, submenu, or shortcut menu. You can use this 
//function to specify the content, appearance, and behavior of the menu item. 
BOOL AppendMenu(
  HMENU hMenu,         // handle to menu
  UINT uFlags,         // menu-item options
  UINT_PTR uIDNewItem, // identifier, menu, or submenu
  LPCTSTR lpNewItem    // menu-item content
)
{
	RETAILMSG(1, TEXT("gl_third.c--AppendMenu()--ERROR!!!"));
	return 1;
}

//	The ModifyMenu function changes an existing menu item. This function is 
//used to specify the content, appearance, and behavior of the menu item. 
BOOL ModifyMenu(
  HMENU hMnu,           // handle to menu
  UINT uPosition,       // menu item to modify
  UINT uFlags,          // options
  UINT_PTR uIDNewItem,  // identifier, menu, or submenu
  LPCTSTR lpNewItem     // menu item content
)
{
	RETAILMSG(1, TEXT("gl_third.c--ModifyMenu()--ERROR!!!"));
	return 1;
}

//	The AdjustWindowRect function calculates the required size of the window
//rectangle, based on the desired client-rectangle size. The window rectangle
//can then be passed to the CreateWindow function to create a window whose 
//client area is the desired size. 
BOOL AdjustWindowRect(
  LPRECT lpRect,  // client-rectangle structure
  DWORD dwStyle,  // window styles
  BOOL bMenu      // menu-present option
)
{
	RETAILMSG(1, TEXT("gl_third.c--AdjustWindowRect()--ERROR!!!"));
	return 1;
}

//	The RealizePalette function maps palette entries from the current logical
//palette to the system palette.
UINT RealizePalette(
  HDC hdc   // handle to DC
)
{
	RETAILMSG(1, TEXT("gl_third.c--RealizePalette()--ERROR!!!"));
	return GDI_ERROR;
}

//	The SelectPalette function selects the specified logical palette into a 
//device context.
HPALETTE SelectPalette(
  HDC hdc,                // handle to DC
  HPALETTE hpal,          // handle to logical palette
  BOOL bForceBackground   // foreground or background mode
)
{
	RETAILMSG(1, TEXT("gl_third.c--SelectPalette()--ERROR!!!"));
	return NULL;
}

BOOL UnrealizeObject(
  HGDIOBJ hgdiobj   // handle to logical palette
)
{
	RETAILMSG(1, TEXT("gl_third.c--UnrealizeObject()--ERROR!!!"));
	return 1;
}

//	The GetSystemPaletteEntries function retrieves a range of palette entries from
//the system palette that is associated with the specified device context (DC).
UINT GetSystemPaletteEntries(
  HDC hdc,              // handle to DC
  UINT iStartIndex,     // first entry to be retrieved
  UINT nEntries,        // number of entries to be retrieved
  PALETTEENTRY *lppe   // array that receives entries
)
{
	RETAILMSG(1, TEXT("gl_third.c--GetSystemPaletteEntries()--ERROR!!!"));
	return 0;
}

//	The SetPaletteEntries function sets RGB (red, green, blue) color values and 
//flags in a range of entries in a logical palette. 
UINT SetPaletteEntries(
  HPALETTE hpal,             // handle to logical palette
  UINT iStart,               // index of first entry to set
  UINT cEntries,             // number of entries to set
  CONST PALETTEENTRY *lppe   // array of palette entries
)
{
	RETAILMSG(1, TEXT("gl_third.c--SetPaletteEntries()--ERROR!!!"));
	return 0;
}

int std_fprintf( VOID *stream, const char *format, ...)
{
	RETAILMSG(1, TEXT("gl_third.c--fprintf()--ERROR!!!"));
	return 0;
}
