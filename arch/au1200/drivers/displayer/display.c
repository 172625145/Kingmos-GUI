#include <eframe.h>
#include <gwmeobj.h>
#include <edevice.h>
#include <eobjtype.h>

extern const _DISPLAYDRV _drvDisplay16BPP;
//extern const _DISPLAYDRV _drvDisplay8BPP;
static const _DISPLAYDRV *gpXBppDrive = &_drvDisplay16BPP;
static COLORREF _PutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _GetPixel( _LPPIXELDATA lpPixelData );
static BOOL _Line( _LPLINEDATA lpData );
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpdwPalEntry, UINT uPalNumber, UINT uPalFormat );
static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpdwPalEntry, UINT uPalNumber, UINT uPalFormat );

static const _DISPLAYDRV oemDisplay16BPP = {
    _PutPixel,
    _GetPixel,
    _Line,
    _BlkBitTransparentBlt,
    _BlkBitMaskBlt,
    _BlkBitBlt,
    _RealizeColor,
    _UnrealizeColor,
};

typedef struct _VIEDO_INFO{
	unsigned long ulBitsPixel;
	unsigned long ulResx;
	unsigned long ulResy;
	unsigned long ulWidthBytes;
	unsigned char *pFrameBuf;	
}VIEDO_INFO, *PVIEDO_INFO;


extern BOOL InitFb(PVIEDO_INFO pViedoInfo);
extern BOOL DeInitFb();

static void Draw3OriginalColor(unsigned C1,unsigned C2,unsigned C3);
static void Draw3OriginalColorAlternate(unsigned win,unsigned C1,unsigned C2,unsigned C3);
static void DisplayerOff(void);

extern void msWait(unsigned);       //Added by Bruce
extern const int nSkipHeight;
extern const int nSkipWidth;
extern const unsigned short wSkipImage[];

static WORD cursorSaveBackground[32][32];   // 32 x 32 16bits background
static WORD cursorImage[16][16] = { // 光标 图象
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0x0000, 0x0000, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0xffff, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xffff, 0xffff, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },
}; 
static BYTE cursorImageMask[16][16] = {  // 光标 屏蔽位 0， 无效， 1， 有效
	{ 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0xff, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

};

static int xCursor; // 光标当前屏幕位置X
static int yCursor; // 光标当前屏幕位置Y
static int nCursorCount = 0;
static BOOL bBackgroundValid = FALSE;
static void Cursor_Off( void );
static void Cursor_On( void );
static BOOL Cursor_IsIntersect( LPCRECT lprc );
static BOOL Cursor_PtInRect( int x, int y );
static void Cursor_SetPos( int x, int y );
//#define  ROTATE


static  _BITMAPDATA oemDisplayBitmap = 
{ 
	{ OBJ_BITMAP, NULL, NULL, NULL, NULL }, 
		0,
		1,
		16,
		240,
		320,
		240 * 2,
		NULL
};

static void InitDisplay( void )
{
	VIEDO_INFO stVideoInfo;
	if(!InitFb(&stVideoInfo))
	{
		ASSERT(0);
		return ;
	}
	oemDisplayBitmap.bmWidth = (int)stVideoInfo.ulResx;
	oemDisplayBitmap.bmHeight = (int)stVideoInfo.ulResy;
	oemDisplayBitmap.bmWidthBytes = (int)stVideoInfo.ulWidthBytes;
	oemDisplayBitmap.bmBits = stVideoInfo.pFrameBuf;
	oemDisplayBitmap.bmBitsPixel = (BYTE)stVideoInfo.ulBitsPixel;

	DEBUGMSG( 1, ( "framebuf:0x%x,gpXBppDrive=0x%x,_drvDisplay16BPP=0x%x\r\n",stVideoInfo.pFrameBuf, gpXBppDrive, &_drvDisplay16BPP ) );
	
	//if(oemDisplayBitmap.bmBits == 16)
	//{
	//	gpXBppDrive = &_drvDisplay16BPP;
	//}
	//else
	//{
	//	ASSERT(0);
	//}
}
static void DisplayerOff(void)
{
	ASSERT(DeInitFb());
}
static int DoDestroy( void )
{
	return 1;
}
static int DoCreate( void )
{
	InitDisplay();
//	Cursor_On();
//	Cursor_SetPos(  400, 400 );
    return 1;
}
static int DoOpen( void )
{
	
	return TRUE;
}
#define DEBUG_GWDI_DisplayEnter 1
DWORD CALLBACK GWDI_DisplayEnter( UINT msg, DWORD dwParam, LPVOID lParam )
{
    switch( msg )
    {
    case GWDI_CREATE:   //初始化硬件
		DEBUGMSG( DEBUG_GWDI_DisplayEnter, ( "GWDI_CREATE\r\n" ) );
        return DoCreate();
    case GWDI_OPEN:
		DEBUGMSG( DEBUG_GWDI_DisplayEnter, ( "GWDI_OPEN\r\n" ) );
    	return DoOpen();
    case GWDI_DESTROY:   //关掉显示设备
		DEBUGMSG( DEBUG_GWDI_DisplayEnter, ( "GWDI_DESTROY\r\n" ) );
        return DoDestroy();
    case GWDI_GET_MODE_NUM:
        return 1;  //返回有多少种显示模式
    //case GWDI_POWEROFF:
        // 开关机处理
      //  return DoPowerOff();
    //case GWDI_POWERON:
    // 开关机处理
      //  return DoPowerOn();
    //case GWDI_GETBITMAP:
      //  *((_LPBITMAPDATA)lParam) = *((_LPBITMAPDATA)&oemDisplayBitmap);
        //return 1;
	case GWDI_GET_DISPLAY_PTR:
		DEBUGMSG( DEBUG_GWDI_DisplayEnter, ( "GWDI_GET_DISPLAY_PTR\r\n" ) );
		*((_LPDISPLAYDRV*)lParam) = (_LPDISPLAYDRV)&oemDisplay16BPP;
		return 1;
    case GWDI_GET_BITMAP_PTR:
		DEBUGMSG( DEBUG_GWDI_DisplayEnter, ( "GWDI_GET_BITMAP_PTR\r\n" ) );
        *((_LPBITMAPDATA*)lParam) = (_LPBITMAPDATA)&oemDisplayBitmap;
        return 1;
    }
    return 0;
}

static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
{
//	DEBUGMSG( 1, ( "_PutPixel\r\n" ) );
	BOOL bSaveCursor = FALSE;
	COLORREF clrRetv;

	if( lpPixelData->lpDestImage ==  &oemDisplayBitmap )
	{
		if( Cursor_PtInRect( lpPixelData->x, lpPixelData->y ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}

    clrRetv = gpXBppDrive->lpPutPixel( lpPixelData );

	if( bSaveCursor )
		Cursor_On();

	return clrRetv;
}

static COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
{
	BOOL bSaveCursor = FALSE;
	COLORREF clrRetv;

	if( lpPixelData->lpDestImage ==  &oemDisplayBitmap )
	{
		if( Cursor_PtInRect( lpPixelData->x, lpPixelData->y ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}

//EBUGMSG( 1, ( "_GetPixel\r\n" ) );

	clrRetv = gpXBppDrive->lpGetPixel( lpPixelData );

	if( bSaveCursor )
		Cursor_On();

	return clrRetv;
}

static BOOL _Line( _LPLINEDATA lpData )
{
	BOOL bSaveCursor = FALSE;
	BOOL bRetv;
//	DEBUGMSG( 1, ( "_Line\r\n" ) );
	if( lpData->lpDestImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcClip ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}

	bRetv = gpXBppDrive->lpLine( lpData );

	if( bSaveCursor )
		Cursor_On();

	return bRetv;
}

static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
	BOOL bSaveCursor = FALSE;
	BOOL bRetv;

//	DEBUGMSG( 1, ( "_BlkBitTransparentBlt\r\n" ) );
	if( lpData->lpDestImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcDest ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}
	else if( lpData->lpSrcImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcSrc ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}
	
	bRetv = gpXBppDrive->lpBlkBitTransparentBlt( lpData );		

	if( bSaveCursor )
		Cursor_On();

	return bRetv;

}

static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData )
{
	BOOL bSaveCursor = FALSE;
	BOOL bRetv;

//	DEBUGMSG( 1, ( "_BlkBitMaskBlt\r\n" ) );

	if( lpData->lpDestImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcDest ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}
	else if( lpData->lpSrcImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcSrc ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}

	bRetv = gpXBppDrive->lpBlkBitMaskBlt( lpData );

	if( bSaveCursor )
		Cursor_On();

	return bRetv;

}

static BOOL _BlkBitBlt( _LPBLKBITBLT lpData )
{
	BOOL bSaveCursor = FALSE;
	BOOL bRetv;

//	DEBUGMSG( 1, ( "_BlkBitMaskBlt\r\n" ) );

	if( lpData->lpDestImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcDest ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}
	else if( lpData->lpSrcImage ==  &oemDisplayBitmap )
	{
		if( Cursor_IsIntersect( lpData->lprcSrc ) )
		{
			bSaveCursor = TRUE;
			Cursor_Off();
		}
	}

//	DEBUGMSG( 1, ( "_BlkBitBlt\r\n" ) );
	bRetv = gpXBppDrive->lpBlkBitBlt( lpData );

	if( bSaveCursor )
		Cursor_On();

	return bRetv;

}

static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpdwPalEntry, UINT uPalNumber, UINT uPalFormat )
{
//EBUGMSG( 1, ( "_RealizeColor\r\n" ) );
    return gpXBppDrive->lpRealizeColor( color, lpdwPalEntry, uPalNumber, uPalFormat);
}

static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpdwPalEntry, UINT uPalNumber, UINT uPalFormat )
{
//	DEBUGMSG( 1, ( "_UnrealizeColor\r\n" ) );
    return gpXBppDrive->lpUnrealizeColor( color, lpdwPalEntry, uPalNumber, uPalFormat );   
}

static BOOL Cursor_IsIntersect( LPCRECT lprc )
{
	RECT rcCursor;
	rcCursor.left = xCursor;
	rcCursor.right = rcCursor.left + 32;

	rcCursor.top = yCursor;
	rcCursor.bottom = rcCursor.top + 32;
	return IntersectRect( &rcCursor, &rcCursor, lprc );
}

static BOOL Cursor_PtInRect( int x, int y )
{
	RECT rcCursor;
	POINT pt = { x, y };
	rcCursor.left = xCursor;
	rcCursor.right = rcCursor.left + 32;

	rcCursor.top = yCursor;
	rcCursor.bottom = rcCursor.top + 32;
	return PtInRect( &rcCursor, pt );
}


static void Cursor_RestoreBackground( void )
{
	if( bBackgroundValid )
	{
		int top, bottom;
		int left, right, size;
		LPWORD lpwFbStart = (LPWORD)( oemDisplayBitmap.bmBits + oemDisplayBitmap.bmWidthBytes * yCursor );
		LPWORD lpwCursorSave = (LPWORD)&cursorSaveBackground;
		
		lpwFbStart += xCursor;
		top = yCursor;
		bottom = top + 32;
		bottom = MIN( bottom, oemDisplayBitmap.bmHeight );
		left = xCursor;
		right = left + 32;
		right = MIN( right, oemDisplayBitmap.bmWidth );
		size = (right - left) * sizeof(WORD);
		
		for( ; top < bottom; top++ )
		{
			memcpy( lpwFbStart, lpwCursorSave, size );
			lpwFbStart = (LPWORD)( (LPBYTE)lpwFbStart + oemDisplayBitmap.bmWidthBytes );
			lpwCursorSave += 32;
		}
		bBackgroundValid = FALSE;
	}
}

static void Cursor_SaveBackground( void )
{
	int top, bottom;
	int left, right, size;
//	int row, col;
	LPWORD lpwFbStart = (LPWORD)( oemDisplayBitmap.bmBits + oemDisplayBitmap.bmWidthBytes * yCursor );
	LPWORD lpwCursorSave = (LPWORD)&cursorSaveBackground;
	
	lpwFbStart += xCursor;
	top = yCursor;
	bottom = top + 32;
	bottom = MIN( bottom, oemDisplayBitmap.bmHeight );
	left = xCursor;
	right = left + 32;
	right = MIN( right, oemDisplayBitmap.bmWidth );
	size = (right - left) * sizeof(WORD);
	// 保存背景
	for( ; top < bottom; top++ )
	{
		memcpy( lpwCursorSave, lpwFbStart, size );
		lpwFbStart = (LPWORD)( (LPBYTE)lpwFbStart + oemDisplayBitmap.bmWidthBytes );
		lpwCursorSave += 32;
	}
	bBackgroundValid = TRUE;
}

static void Cursor_ShowCursor( void )
{
	int top, bottom;
	int left, right;//, size;
	int row, col;
	LPWORD lpwFbStart = (LPWORD)( oemDisplayBitmap.bmBits + oemDisplayBitmap.bmWidthBytes * yCursor );
	LPWORD lpwCursorSave = (LPWORD)&cursorSaveBackground;
	
	lpwFbStart += xCursor;
	top = yCursor;
	bottom = top + 16;
	bottom = MIN( bottom, oemDisplayBitmap.bmHeight );
	left = xCursor;
	right = left + 16;
	right = MIN( right, oemDisplayBitmap.bmWidth );

	// 显示光标
	for( top = yCursor, row = 0; top < bottom; top++, row++ )
	{
		LPWORD lpwLine = lpwFbStart;
		for( left = xCursor, col = 0; left < right; left++, col++ )
		{
			if( cursorImageMask[row][col] )
				*lpwLine = cursorImage[row][col];
			lpwLine++;
		}
		lpwFbStart = (LPWORD)( (LPBYTE)lpwFbStart + oemDisplayBitmap.bmWidthBytes );
	}
}

//static int nCursorCount = 0;
static void Cursor_Off( void )
{
	--nCursorCount;
	if( nCursorCount == 0 )
	{  // 之前是打开的， 现在关闭，恢复背景
		Cursor_RestoreBackground();		
	}
}

static void Cursor_SetPos( int x, int y )
{
//	int top, bottom;
//	int left, right;//, size;

	if( x >= oemDisplayBitmap.bmWidth )
		x = oemDisplayBitmap.bmWidth - 1;
	if( y >= oemDisplayBitmap.bmHeight )
		y = oemDisplayBitmap.bmHeight - 1;
	if( x < 0 )
		x = 0;
	if( y < 0 )
		y = 0;
	if( x == xCursor && y == yCursor )
		return;  //same
	
	Cursor_Off();
	xCursor = x;  //新位置
	yCursor = y;
	Cursor_On();
}

static void Cursor_On( void )
{
	if( nCursorCount <= 0 )
	{
		++nCursorCount;
		if( nCursorCount == 1 )
		{	// 之前是关闭的， 现在打开
			Cursor_SaveBackground();
			Cursor_ShowCursor();
		}
	}	
}
