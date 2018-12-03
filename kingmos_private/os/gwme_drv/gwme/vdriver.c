#include <eframe.h>

#include <gwmeobj.h>
//#include <winsrv.h>
//#include <gdisrv.h>
#include <gdc.h>

//extern _BITMAPDATA oemDisplayBitmap;


static COLORREF _VirtualPutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _VirtualGetPixel( _LPPIXELDATA lpPixelData );
static BOOL _VirtualLine( _LPLINEDATA lpLineData );
static BOOL _VirtualBlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _VirtualBlkBitMaskBlt( _LPBLKBITBLT lpData );
static BOOL _VirtualBlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _VirtualRealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static COLORREF _VirtualUnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );

const _DISPLAYDRV _vDisplayDriver = {
    _VirtualPutPixel,
    _VirtualGetPixel,
    _VirtualLine,
    _VirtualBlkBitTransparentBlt,
    _VirtualBlkBitMaskBlt,
    _VirtualBlkBitBlt,
    _VirtualRealizeColor,
    _VirtualUnrealizeColor
};

//extern _BITMAPDATA oemDisplayBitmap;
extern _LPBITMAPDATA lpMainLayerImage;

extern void FlushDirtyRect( LPCRECT lp )
{
	extern BOOL _FlushGdi( LPCRECT lprcDirty );
	_FlushGdi( lp );

}

static COLORREF _VirtualPutPixel( _LPPIXELDATA lpPixelData )
{
	COLORREF col;

	col = lpDrvDisplayDevice->lpPutPixel( lpPixelData );
    if( lpPixelData->lpDestImage == lpMainLayerImage )
	{
		RECT rect;
		rect.left = lpPixelData->x;
		rect.right = lpPixelData->x + 1;
		rect.top = lpPixelData->y;
		rect.bottom = lpPixelData->y + 1;
		FlushDirtyRect( &rect );
	}
	return col;
}

static COLORREF _VirtualGetPixel( _LPPIXELDATA lpPixelData )
{
    return lpDrvDisplayDevice->lpGetPixel( lpPixelData );
}

static VOID CALLBACK DiaLine( _LPLINEDATA lpLineData, LPCRECT lprc )
{
	FlushDirtyRect( lprc );
}

static BOOL _VirtualLine( _LPLINEDATA lpLineData )
{
	int ret;
    _LINEDATA ld;
	RECT rc;

	if( lpLineData->lpDestImage == lpMainLayerImage )
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
	    ret = lpDrvDisplayDevice->lpLine( lpLineData );  
		if( ret && lpLineData->lpDestImage == lpMainLayerImage )
		{
			RECT bounds;			
			int N_plus_1 = 1;			// Minor length of bounding rect + 1			
	
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

			FlushDirtyRect( &bounds );
		}
	}
	else
	{	//斜线
		if( lpLineData->lpDestImage == lpMainLayerImage )
		{
			lpLineData->lpfnCallback = DiaLine;
			ret = lpDrvDisplayDevice->lpLine( lpLineData );  
			lpLineData->lpfnCallback = NULL;
		}
		else
		{
			ret = lpDrvDisplayDevice->lpLine( lpLineData );  
		}
	}

	return ret;
}

static BOOL _VirtualBlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
//	_BLKBITBLT blk;
	
	int ret = lpDrvDisplayDevice->lpBlkBitTransparentBlt( lpData );
	if( ret )
	{
		if( lpData->lpDestImage == lpMainLayerImage )
		{
			FlushDirtyRect( lpData->lprcDest );                
		}
	}
	return ret;
}

static BOOL _VirtualBlkBitMaskBlt( _LPBLKBITBLT lpData )
{
//	_BLKBITBLT blk;

	int ret = lpDrvDisplayDevice->lpBlkBitMaskBlt( lpData );
	if( ret )
	{
		if( lpData->lpDestImage == lpMainLayerImage )
			FlushDirtyRect( lpData->lprcDest );
	}
	
	return ret;
}


static BOOL _VirtualBlkBitBlt( _LPBLKBITBLT lpData )
{
//	_BLKBITBLT blk;

	int ret = lpDrvDisplayDevice->lpBlkBitBlt( lpData );
	
	if( ret )
	{
		if( lpData->lpDestImage == lpMainLayerImage )
			FlushDirtyRect( lpData->lprcDest );
	}
	
	return ret;
}

static COLORREF _VirtualRealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
	return lpDrvDisplayDevice->lpRealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
}

static COLORREF _VirtualUnrealizeColor( DWORD color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
	return lpDrvDisplayDevice->lpUnrealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
}



