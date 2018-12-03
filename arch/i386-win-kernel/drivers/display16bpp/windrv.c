#include <eframe.h>
#include <gwmeobj.h>
#include <edevice.h>

//WORD __wDisplayFrameBuf[160/8*160/2];
//_BITMAPDATA oemDisplayBitmap = { OBJ_BITMAP, 0, 160, 160, 20, 1, 1, (BYTE*)__wDisplayFrameBuf };
extern _BITMAPDATA oemDisplayBitmap;

//_BITMAPDATA oemDisplayBitmap;

extern void DrawDirtyRect( _LPBITMAPDATA lpDestImage, void * lp );

static COLORREF _WinPutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _WinGetPixel( _LPPIXELDATA lpPixelData );
static BOOL _WinLine( _LPLINEDATA lpLineData );
//static BOOL _WinVertialLine( _LPLINEDATA lpLineData );
//static BOOL _WinScanLine( _LPLINEDATA lpData );
//static BOOL _WinTextBitBlt( _LPBLKBITBLT lpData );
static BOOL _WinBlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _WinBlkBitMaskBlt( _LPBLKBITBLT lpData );
static BOOL _WinBlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _WinRealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static COLORREF _WinUnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );

#ifdef COLOR_1BPP
extern const _DISPLAYDRV _drvDisplay1BPP;
#define DEFAULT_DRV _drvDisplay1BPP
#endif

#ifdef COLOR_8BPP
extern const _DISPLAYDRV _drvDisplay8BPP;
#define DEFAULT_DRV _drvDisplay8BPP
#endif

#ifdef COLOR_16BPP
extern const _DISPLAYDRV _drvDisplay16BPP;
#define DEFAULT_DRV _drvDisplay16BPP
#endif

const _DISPLAYDRV _oemDisplayDefault = {
    _WinPutPixel,
    _WinGetPixel,
  //  _WinVertialLine,
//    _WinScanLine,
    _WinLine,
    //_WinTextBitBlt,
    _WinBlkBitTransparentBlt,
    _WinBlkBitMaskBlt,
    _WinBlkBitBlt,
    _WinRealizeColor,
    _WinUnrealizeColor
};

extern _BITMAPDATA oemDisplayBitmap;
//const _DISPLAYDRV * _lpDrvDisplayDefault = &_drvDisplayDefault;

COLORREF _WinPutPixel( _LPPIXELDATA lpPixelData )
{
	_PIXELDATA pix;
	COLORREF col;

	if( lpPixelData->lpDestImage == &oemDisplayBitmap )
	{
		pix = *lpPixelData;
	    lpPixelData = &pix;
		lpPixelData->x += GetSystemMetrics( SM_XVIEW );
		lpPixelData->y += GetSystemMetrics( SM_YVIEW );
	}
	col = DEFAULT_DRV.lpPutPixel( lpPixelData );
    if( lpPixelData->lpDestImage == &oemDisplayBitmap )
	{
		    RECT rect;
			rect.left = lpPixelData->x;
			rect.right = lpPixelData->x + 1;
			rect.top = lpPixelData->y;
			rect.bottom = lpPixelData->y + 1;
			DrawDirtyRect( lpPixelData->lpDestImage, (void*)&rect );
	}
	return col;
}

COLORREF _WinGetPixel( _LPPIXELDATA lpPixelData )
{
	_PIXELDATA pix;
	
	if( lpPixelData->lpDestImage == &oemDisplayBitmap )
	{
 	    pix = *lpPixelData;
		lpPixelData = &pix;
		lpPixelData->x += GetSystemMetrics( SM_XVIEW );
		lpPixelData->y += GetSystemMetrics( SM_YVIEW );
	}

    return DEFAULT_DRV.lpGetPixel( lpPixelData );
}
/*
BOOL _WinLine( _LPLINEDATA lpLineData )
{
	int ret;
	//if( lpLineData->dy == 0 && lpLineData->dx < 0 )
	//	ret = 0;

	ret = DEFAULT_DRV.lpLine( lpLineData );
	if( ret )
	{
		if( lpLineData->lpDestImage == &oemDisplayBitmap )
		{
		    RECT rect;

			if( lpLineData->dx == 0 )
			{
				rect.left = lpLineData->x0;
				rect.right = rect.left + 1;
			}
			else if( lpLineData->dx > 0 )
			{
				rect.left = lpLineData->x0;
				rect.right = rect.left + lpLineData->dx;
			}
			else if( lpLineData->dx < 0 )
			{
				rect.left = lpLineData->x0 + lpLineData->dx;
				rect.right = lpLineData->x0 + 1;
			}

			if( lpLineData->dy == 0 )
			{
				rect.top = lpLineData->y0;
				rect.bottom = rect.top + 1;
			}
			else if( lpLineData->dy > 0 )
			{
				rect.top = lpLineData->y0;
				rect.bottom = rect.top + lpLineData->dy;
			}
			else
			{ //< 0 
				rect.top = lpLineData->y0 + lpLineData->dy;
				rect.bottom = lpLineData->y0 + 1;
			}
			InflateRect(&rect, 1 + lpLineData->width, 1 + lpLineData->width ); 
			
			IntersectRect(&rect, &rect, lpLineData->lprcClip );
			if( rect.left < rect.right &&
				rect.top < rect.bottom )
			    DrawDirtyRect( lpLineData->lpDestImage, &rect );
		}
	}
	return ret;
}
*/

VOID CALLBACK DiaLine( _LPLINEDATA lpLineData, LPCRECT lprc )
{
	//FlushLCD( lprc->left, lprc->top, lprc->right, lprc->bottom );
	DrawDirtyRect( lpLineData->lpDestImage, (void *)lprc );
}

static BOOL _WinLine( _LPLINEDATA lpLineData )
{
	int ret;
    _LINEDATA ld;
	RECT rc;

	if( lpLineData->lpDestImage == &oemDisplayBitmap )
	{
 	    ld = *lpLineData;
		lpLineData = &ld;
		rc = *lpLineData->lprcClip;
		lpLineData->lprcClip = &rc;
		OffsetRect( &rc, GetSystemMetrics( SM_XVIEW ), GetSystemMetrics( SM_YVIEW ) );		
		lpLineData->xStart += GetSystemMetrics( SM_XVIEW );
		lpLineData->yStart += GetSystemMetrics( SM_YVIEW );
	}

	if( lpLineData->dn == 0 )
	{	//水平或垂直线
	    ret = DEFAULT_DRV.lpLine( lpLineData );  
		if( ret && lpLineData->lpDestImage == &oemDisplayBitmap )
		{
			RECT bounds;			
			int N_plus_1 = 1;			// Minor length of bounding rect + 1
			
			//if( lpLineData->dn)	// The line has a diagonal component (we'll refresh the bounding rect)
			//	N_plus_1 = 2 + ((lpLineData->cPels * lpLineData->dm) / lpLineData->dm);
			//else
			//	N_plus_1 = 1;
			
			switch(lpLineData->iDir) 
			{
			case 0:  // ->
				bounds.left = lpLineData->xStart;
				bounds.top = lpLineData->yStart;
				bounds.right = lpLineData->xStart + lpLineData->cPels;// + 1;
				bounds.bottom = bounds.top + N_plus_1;
				break;
			case 1: // |
				bounds.left = lpLineData->xStart;
				bounds.top = lpLineData->yStart;
				bounds.bottom = lpLineData->yStart + lpLineData->cPels;// + 1;
				bounds.right = bounds.left + N_plus_1;
				break;
			case 2:
				bounds.right = lpLineData->xStart + 1;
				bounds.top = lpLineData->yStart;
				bounds.bottom = lpLineData->yStart + lpLineData->cPels;// + 1;
				bounds.left = bounds.right - N_plus_1;
				break;
			case 3:
				bounds.right = lpLineData->xStart + 1;
				bounds.top = lpLineData->yStart;
				bounds.left = lpLineData->xStart - lpLineData->cPels + 1;
				bounds.bottom = bounds.top + N_plus_1;
				break;
			case 4:
				bounds.right = lpLineData->xStart + 1;
				bounds.bottom = lpLineData->yStart + 1;
				bounds.left = lpLineData->xStart - lpLineData->cPels + 1;
				bounds.top = bounds.bottom - N_plus_1;
				break;
			case 5:
				bounds.right = lpLineData->xStart + 1;
				bounds.bottom = lpLineData->yStart + 1;
				bounds.top = lpLineData->yStart - lpLineData->cPels + 1;
				bounds.left = bounds.right - N_plus_1;
				break;
			case 6:
				bounds.left = lpLineData->xStart;
				bounds.bottom = lpLineData->yStart + 1;
				bounds.top = lpLineData->yStart - lpLineData->cPels + 1;
				bounds.right = bounds.left + N_plus_1;
				break;
			case 7:
				bounds.left = lpLineData->xStart;
				bounds.bottom = lpLineData->yStart + 1;
				bounds.right = lpLineData->xStart + lpLineData->cPels;// + 1;
				bounds.top = bounds.bottom - N_plus_1;
				break;
			}
			if( lpLineData->width > 1 )
			{
				int h = (lpLineData->width + 1) / 2;
				InflateRect( &bounds, h, h );
				IntersectRect(&bounds, &bounds, lpLineData->lprcClip );
			}
			if( !(bounds.left >=0 && bounds.left <= 240 &&
				bounds.right >= 0 && bounds.right <= 240 &&
				bounds.left < bounds.right &&
		        bounds.top  >=0 && bounds.top <= 320 &&
				bounds.bottom >=0 && bounds.bottom <= 320 &&
				bounds.top < bounds.bottom)
			   )
			{
				bounds.top = bounds.top;
			}
			//FlushLCD( bounds.left, bounds.top, bounds.right, bounds.bottom );
			DrawDirtyRect( lpLineData->lpDestImage, &bounds );
/*			
			InflateRect(&bounds, 1 + lpLineData->width, 1 + lpLineData->width ); 
			if( lpLineData->width > 1 )
				IntersectRect(&bounds, &bounds, lpLineData->lprcClip );
			
			{
				const RECT rect = { 0, 0, 240, 320 };
				IntersectRect( &bounds, &bounds, &rect );
			}
			if( !IsRectEmpty( &bounds ) )
			{
				FlushLCD( bounds.left, bounds.top, bounds.right, bounds.bottom );
			}
*/
		}
	}
	else
	{	//斜线
		if( lpLineData->lpDestImage == &oemDisplayBitmap )
		{
			lpLineData->lpfnCallback = DiaLine;
			ret = DEFAULT_DRV.lpLine( lpLineData );  
			lpLineData->lpfnCallback = NULL;
		}
		else
		{
			ret = DEFAULT_DRV.lpLine( lpLineData );  
		}
	}

	return ret;
}

/*
BOOL _WinLine( _LPLINEDATA lpLineData )
{
	int ret;
    _LINEDATA ld;
	RECT rc;
	if( lpLineData->lpDestImage == &oemDisplayBitmap )
	{
 	    ld = *lpLineData;
		lpLineData = &ld;
		rc = *lpLineData->lprcClip;
		lpLineData->lprcClip = &rc;
		OffsetRect( &rc, GetSystemMetrics( SM_XVIEW ), GetSystemMetrics( SM_YVIEW ) );		
		lpLineData->xStart += GetSystemMetrics( SM_XVIEW );
		lpLineData->yStart += GetSystemMetrics( SM_YVIEW );
	}


	ret = DEFAULT_DRV.lpLine( lpLineData );
	if( ret )
	{
		if( lpLineData->lpDestImage == &oemDisplayBitmap )
		{
		    RECT bounds;

		int N_plus_1;			// Minor length of bounding rect + 1
		
		if( lpLineData->dn)	// The line has a diagonal component (we'll refresh the bounding rect)
			N_plus_1 = 2 + ((lpLineData->cPels * lpLineData->dm) / lpLineData->dm);
		else
			N_plus_1 = 1;
		
		switch(lpLineData->iDir) 
		{
		case 0:
			bounds.left = lpLineData->xStart;
			bounds.top = lpLineData->yStart;
			bounds.right = lpLineData->xStart + lpLineData->cPels + 1;
			bounds.bottom = bounds.top + N_plus_1;
			break;
		case 1:
			bounds.left = lpLineData->xStart;
			bounds.top = lpLineData->yStart;
			bounds.bottom = lpLineData->yStart + lpLineData->cPels + 1;
			bounds.right = bounds.left + N_plus_1;
			break;
		case 2:
			bounds.right = lpLineData->xStart + 1;
			bounds.top = lpLineData->yStart;
			bounds.bottom = lpLineData->yStart + lpLineData->cPels + 1;
			bounds.left = bounds.right - N_plus_1;
			break;
		case 3:
			bounds.right = lpLineData->xStart + 1;
			bounds.top = lpLineData->yStart;
			bounds.left = lpLineData->xStart - lpLineData->cPels;
			bounds.bottom = bounds.top + N_plus_1;
			break;
		case 4:
			bounds.right = lpLineData->xStart + 1;
			bounds.bottom = lpLineData->yStart + 1;
			bounds.left = lpLineData->xStart - lpLineData->cPels;
			bounds.top = bounds.bottom - N_plus_1;
			break;
		case 5:
			bounds.right = lpLineData->xStart + 1;
			bounds.bottom = lpLineData->yStart + 1;
			bounds.top = lpLineData->yStart - lpLineData->cPels;
			bounds.left = bounds.right - N_plus_1;
			break;
		case 6:
			bounds.left = lpLineData->xStart;
			bounds.bottom = lpLineData->yStart + 1;
			bounds.top = lpLineData->yStart - lpLineData->cPels;
			bounds.right = bounds.left + N_plus_1;
			break;
		case 7:
			bounds.left = lpLineData->xStart;
			bounds.bottom = lpLineData->yStart + 1;
			bounds.right = lpLineData->xStart + lpLineData->cPels + 1;
			bounds.top = bounds.bottom - N_plus_1;
			break;
		}

			InflateRect(&bounds, 1 + lpLineData->width, 1 + lpLineData->width ); 
			if( lpLineData->width > 1 )			
			    IntersectRect(&bounds, &bounds, lpLineData->lprcClip );
			if( bounds.left < bounds.right &&
				bounds.top < bounds.bottom )
			    DrawDirtyRect( lpLineData->lpDestImage, &bounds );
		}
	}
	return ret;
}
*/


/*
BOOL _WinVertialLine( _LPLINEDATA lpLineData )
{
	int ret = DEFAULT_DRV.lpVertialLine( lpLineData );
	if( ret )
	{
		if( lpLineData->lpDestImage == &oemDisplayBitmap )
		{
		    RECT rect;
			rect.left = lpLineData->x0;
			rect.right = lpLineData->x0 + 1;
			rect.top = lpLineData->y0;
			rect.bottom = lpLineData->y1;
			DrawDirtyRect( lpLineData->lpDestImage, &rect );
		}
	}
	return ret;
}
*/
/*
BOOL _WinScanLine( _LPLINEDATA lpData )
{
	int ret = DEFAULT_DRV.lpScanLine( lpData );
	if( ret )
	{
		if( lpData->lpDestImage == &oemDisplayBitmap )
		{
		    RECT rect;
			rect.left = lpData->x0;
			rect.right = lpData->x1;
			rect.top = lpData->y0;
			rect.bottom = lpData->y0 + 1;
			DrawDirtyRect( lpData->lpDestImage, (void*)&rect );
		}
	}
	return ret;
}
*/
/*
BOOL _WinTextBitBlt( _LPBLKBITBLT lpData )
{
    int ret = _TextBitBlt( lpData );
	if( ret )
	{
   		if( lpData->lpDestImage == &oemDisplayBitmap )
    	    DrawDirtyRect( lpData->lpDestImage, (void*)lpData->lprcDest );

	}
	return ret;
}
*/
BOOL _WinBlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
	_BLKBITBLT blk;

	if( lpData->lpDestImage == &oemDisplayBitmap )
	{
		blk = *lpData;
		lpData = &blk;		
		OffsetRect( (LPRECT)lpData->lprcDest, GetSystemMetrics( SM_XVIEW ), GetSystemMetrics( SM_YVIEW ) );
	}

    {
		int ret = DEFAULT_DRV.lpBlkBitTransparentBlt( lpData );
		if( ret )
		{
			if( lpData->lpDestImage == &oemDisplayBitmap )
			{
				DrawDirtyRect( lpData->lpDestImage, (void*)lpData->lprcDest );                
			}
		}
		if( lpData->lpDestImage == &oemDisplayBitmap )
		    OffsetRect( (LPRECT)lpData->lprcDest, -GetSystemMetrics( SM_XVIEW ), -GetSystemMetrics( SM_YVIEW ) );
		return ret;
	}
}

BOOL _WinBlkBitMaskBlt( _LPBLKBITBLT lpData )
{
	_BLKBITBLT blk;

	if( lpData->lpDestImage == &oemDisplayBitmap )
	{
		blk = *lpData;
		lpData = &blk;		
		OffsetRect( (LPRECT)lpData->lprcDest, GetSystemMetrics( SM_XVIEW ), GetSystemMetrics( SM_YVIEW ) );
	}

	{
		
		int ret = DEFAULT_DRV.lpBlkBitMaskBlt( lpData );
		if( ret )
		{
			if( lpData->lpDestImage == &oemDisplayBitmap )
				DrawDirtyRect( lpData->lpDestImage, (void*)lpData->lprcDest );
		}
		if( lpData->lpDestImage == &oemDisplayBitmap )
			OffsetRect( (LPRECT)lpData->lprcDest, -GetSystemMetrics( SM_XVIEW ), -GetSystemMetrics( SM_YVIEW ) );
		
		return ret;
		
	}
}


BOOL _WinBlkBitBlt( _LPBLKBITBLT lpData )
{
	_BLKBITBLT blk;

	if( lpData->lpDestImage == &oemDisplayBitmap )
	{
		blk = *lpData;
		lpData = &blk;		
		OffsetRect( (LPRECT)lpData->lprcDest, GetSystemMetrics( SM_XVIEW ), GetSystemMetrics( SM_YVIEW ) );
	}

	{
		int ret = DEFAULT_DRV.lpBlkBitBlt( lpData );
		
		if( ret )
		{
			if( lpData->lpDestImage == &oemDisplayBitmap )
				DrawDirtyRect( lpData->lpDestImage, (void*)lpData->lprcDest );
		}
		if( lpData->lpDestImage == &oemDisplayBitmap )
			OffsetRect( (LPRECT)lpData->lprcDest, -GetSystemMetrics( SM_XVIEW ), -GetSystemMetrics( SM_YVIEW ) );
		
		return ret;
	}
}

static COLORREF _WinRealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
#ifdef COLOR_16BPP
/*
    DWORD r, g, b;

    // format to 5b 6g 5r
    r = ((color << 24) >> 27) & 0x001f;
    g = ((color << 16) >> 21) & 0x07e0;
    b = ((color << 8) >> 16) & 0xf800;
    return (b | g | r);
*/
	return DEFAULT_DRV.lpRealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );

/*  这代码将会导致 灰色失真（偏绿）
	DWORD r, g, b;

	r = ( (color & 0xff) * 0x1f / 0xff ) << 11;	
	g = ( ( (color >> 8 ) & 0xff) * 0x3f / 0xff ) << 5;
	b = ( ( (color >> 16 ) & 0xff) * 0x1f / 0xff );
	
	return r | g | b;
*/
#endif

#ifdef COLOR_8BPP
     return DEFAULT_DRV.lpRealizeColor( color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
#endif

#ifdef COLOR_1BPP
     return DEFAULT_DRV.lpRealizeColor( color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
#endif

}

static COLORREF _WinUnrealizeColor( DWORD color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
#ifdef COLOR_16BPP
/*
    DWORD r = color & 0x001f;
    DWORD g = color & 0x07e0;
    DWORD b = color & 0xf800;

    r = ((r << 27) >> 24);
    g = ((g << 21) >> 16);
    b = ((b << 16) >> 8);
    return (b | g | r);
*/
	return DEFAULT_DRV.lpUnrealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );

/*  这代码将会导致 灰色失真（偏绿）
	DWORD r, g, b;

    r = ( (color & 0xf800) >> 11) * 0xff / 0x1f;
	g = ( ( (color & 0x07e0) >> 5) * 0xff / 0x3f ) << 8;
	b = ( ( (color & 0x001f)) * 0xff / 0x1f ) << 16;

	return r | g | b;
*/

#endif
#ifdef COLOR_8BPP
    return DEFAULT_DRV.lpUnrealizeColor( color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
#endif
#ifdef COLOR_1BPP
    return DEFAULT_DRV.lpUnrealizeColor( color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
#endif

}

VOID DoPowerOff( VOID );
VOID DoPowerOn( VOID );
BOOL DoDisplayerCreate( void );
BOOL DoDisplayerDestroy( void );

DWORD CALLBACK GWDI_DisplayEnter( UINT msg, DWORD dwParam, LPVOID lParam )
{
    switch( msg )
    {
	//case GWDI_OPEN:
	//	return 1;
	//case GWDI_CLOSE
    //	return 1;
	case GWDI_FLUSH:
		{
			RECT rc = { 0, 0, oemDisplayBitmap.bmWidth, oemDisplayBitmap.bmHeight };
			DrawDirtyRect( &oemDisplayBitmap, (void*)&rc );
		}
		return 0;

    case GWDI_POWEROFF:
        // 开关机处理
        DoPowerOff();
		break;
    case GWDI_POWERON:
		// 开关机处理
        DoPowerOn();
		break;
    //case GWDI_GETBITMAP:
      //  *((_LPBITMAPDATA)lParam) = *((_LPBITMAPDATA)&oemDisplayBitmap);
        //return 1;
	case GWDI_GET_DISPLAY_PTR:
		*((_LPDISPLAYDRV*)lParam) = (_LPDISPLAYDRV)&_oemDisplayDefault;
		return 1;
    case GWDI_GET_BITMAP_PTR:
        *((_LPBITMAPDATA*)lParam) = (_LPBITMAPDATA)&oemDisplayBitmap;
        return 1;
    case GWDI_CREATE:   //初始化硬件
        return DoDisplayerCreate();
    case GWDI_DESTROY:   //关掉显示设备
        return DoDisplayerDestroy();
    case GWDI_GET_MODE_NUM:
        return 1;  //返回有多少种显示模式
    }
    return 0;
}


