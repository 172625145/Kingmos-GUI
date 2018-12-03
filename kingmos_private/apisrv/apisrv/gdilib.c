/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>


typedef BOOL ( WINAPI * PGDI_BITBLT )(HDC hdcDest, int nDestX, int nDestY, int nWidth, int nHeight,HDC hdcSrc, int nSrcX, int nSrcY, DWORD dwRop );

BOOL WINAPI Gdi_BitBlt(HDC hdcDest, 
					   int nDestX, 
					   int nDestY, 
					   int nWidth, 
					   int nHeight,
				       HDC hdcSrc,
					   int nSrcX,
					   int nSrcY,
					   DWORD dwRop )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_BITBLT, 9 );
    cs.arg0 = (DWORD)hdcDest;
    return (DWORD)CALL_SERVER( &cs, nDestX, nDestY, nWidth, nHeight, hdcSrc, nSrcX, nSrcY, dwRop );
#else

	PGDI_BITBLT pBitBlt;// = (PGDI_BITBLT)lpGDIAPI[GDI_BITBLT];
	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_BITBLT, &pBitBlt, &cs ) )
	{
		bRetv = pBitBlt( hdcDest, nDestX, nDestY, nWidth, nHeight, hdcSrc, nSrcX, nSrcY, dwRop );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_PATBLT )(HDC hdc, int nX, int nY, int nWidth, int nHeight, DWORD dwRop );
BOOL WINAPI Gdi_PatBlt(HDC hdc, int nX, int nY, int nWidth, int nHeight, DWORD dwRop )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_PATBLT, 6 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, nX, nY, nWidth, nHeight, dwRop );
#else

	PGDI_PATBLT pPatBlt;// = (PGDI_PATBLT)lpGDIAPI[GDI_PATBLT];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_PATBLT, &pPatBlt, &cs ) )
	{
		bRetv = pPatBlt( hdc, nX, nY, nWidth, nHeight, dwRop );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_MASKBLT )(HDC hdcdest,int xDest,int yDest,int width,int height,HDC hdcSrc,int xSrc,int ySrc,HBITMAP hbmMask,int xMask,int yMask,DWORD dwRop );
BOOL WINAPI Gdi_MaskBlt(
				 HDC hdcdest,
                 int xDest,
                 int yDest,
                 int width,
                 int height,
                 HDC hdcSrc,
                 int xSrc,
                 int ySrc,
                 HBITMAP hbmMask,
                 int xMask,
                 int yMask,
                 DWORD dwRop )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_MASKBLT, 12 );
    cs.arg0 = (DWORD)hdcdest;
    return (DWORD)CALL_SERVER( &cs, xDest, yDest, width, height, hdcSrc, xSrc, ySrc, hbmMask, xMask, yMask, dwRop );
#else

	PGDI_MASKBLT pMaskBlt;// = (PGDI_MASKBLT)lpGDIAPI[GDI_MASKBLT];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_MASKBLT, &pMaskBlt, &cs ) )
	{
		bRetv = pMaskBlt( hdcdest, xDest, yDest, width, height, hdcSrc, xSrc, ySrc, hbmMask, xMask, yMask, dwRop );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PGDI_DRAWEDGE )( HDC hdc, LPCRECT lpcrc, UINT uType, UINT uFlags );
BOOL WINAPI Gdi_DrawEdge( HDC hdc, LPCRECT lpcrc, UINT uType, UINT uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DRAWEDGE, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcrc, uType, uFlags );
#else

	PGDI_DRAWEDGE pDrawEdge;// = (PGDI_DRAWEDGE)lpGDIAPI[GDI_DRAWEDGE];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_DRAWEDGE, &pDrawEdge, &cs ) )
	{
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		bRetv = pDrawEdge( hdc, lpcrc, uType, uFlags );
		API_Leave(  );
	}
	return bRetv;
#endif
}
///////////////////////////////////////////////////////////////
typedef BOOL ( WINAPI * PGDI_DRAWICON )( HDC hdc, int nX, int nY, HICON );
BOOL WINAPI Gdi_DrawIcon( HDC hdc, int nX, int nY, HICON hIcon )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DRAWICON, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, nX, nY, hIcon );
#else

	PGDI_DRAWICON pDrawIcon;// = (PGDI_DRAWICON)lpGDIAPI[GDI_DRAWICON];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_DRAWICON, &pDrawIcon, &cs ) )
	{
		bRetv = pDrawIcon( hdc, nX, nY, hIcon );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DRAWICONEX )( HDC hdc, int x, int y, HICON hIcon, int cxWidth, int cyWidth, UINT uiIndexAnimal,HBRUSH hBrush, UINT uiFlags ); 
BOOL WINAPI Gdi_DrawIconEx( HDC hdc, 
						    int x, 
							int y, 
							HICON hIcon, 
							int cxWidth, 
							int cyWidth, 
							UINT uiIndexAnimal,
							HBRUSH hBrush,
							UINT uiFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DRAWICONEX, 9 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y, hIcon, cxWidth, cyWidth, uiIndexAnimal, hBrush, uiFlags );
#else

	PGDI_DRAWICONEX pDrawIconEx;// = (PGDI_DRAWICONEX)lpGDIAPI[GDI_DRAWICONEX];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_DRAWICONEX, &pDrawIconEx, &cs ) )
	{
		bRetv = pDrawIconEx( hdc, x, y, hIcon, cxWidth, cyWidth, uiIndexAnimal, hBrush, uiFlags );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_DRAWTEXT )( HDC, LPCTSTR lpcstr, int nCount, LPRECT lpcrc, UINT uFormat );
int WINAPI Gdi_DrawText( HDC hdc, 
						 LPCTSTR lpcstr, 
						 int nCount, 
						 LPRECT lpcrc, 
						 UINT uFormat )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DRAWTEXT, 5 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcstr, nCount, lpcrc, uFormat );
#else

	PGDI_DRAWTEXT pDrawText;// = (PGDI_DRAWTEXT)lpGDIAPI[GDI_DRAWTEXT];

	CALLSTACK cs;
	int  iRetv = FALSE;

	if( API_Enter( API_GDI, GDI_DRAWTEXT, &pDrawText, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		iRetv = pDrawText( hdc, lpcstr, nCount, lpcrc, uFormat );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DRAWFOCUSRECT )( HDC hdc, LPCRECT lprc );

BOOL WINAPI Gdi_DrawFocusRect( HDC hdc, LPCRECT lprc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DRAWFOCUSRECT, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lprc );
#else

	PGDI_DRAWFOCUSRECT pDrawFocusRect;// = (PGDI_DRAWFOCUSRECT)lpGDIAPI[GDI_DRAWFOCUSRECT];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_DRAWFOCUSRECT, &pDrawFocusRect, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pDrawFocusRect( hdc, lprc );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DPTOLP )( HDC hdc, LPPOINT lpPoints, int nCount );
BOOL WINAPI Gdi_DPtoLP( HDC hdc, LPPOINT lpPoints, int nCount )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DPTOLP, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpPoints, nCount );
#else

	PGDI_DPTOLP pDPtoLP;// = (PGDI_DPTOLP)lpGDIAPI[GDI_DPTOLP];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_DPTOLP, &pDPtoLP, &cs ) )
	{
		lpPoints = MapProcessPtr( lpPoints, (LPPROCESS)cs.lpvData );

		bRetv = pDPtoLP( hdc, lpPoints, nCount );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_ELLIPSE ) ( HDC hdc, int left, int top, int right, int bottom );
BOOL WINAPI Gdi_Ellipse( HDC hdc, int left, int top, int right, int bottom )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_ELLIPSE, 5 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, left, top, right, bottom );
#else

	PGDI_ELLIPSE pEllipse;// = (PGDI_EXCLUDECLIPRECT)lpGDIAPI[GDI_EXCLUDECLIPRECT];

	CALLSTACK cs;
	BOOL  iRetv = FALSE;

	if( API_Enter( API_GDI, GDI_ELLIPSE, &pEllipse, &cs ) )
	{

		iRetv = pEllipse( hdc, left, top, right, bottom );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_ARC ) ( HDC hdc, int left, int top, int right, int bottom, int xStartArc, int yStartArc, int xEndArc, int yEndArc );
BOOL WINAPI Gdi_Arc( HDC hdc, int left, int top, int right, int bottom, int xStartArc, int yStartArc, int xEndArc, int yEndArc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_ARC, 9 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, left, top, right, bottom, xStartArc, yStartArc, xEndArc, yEndArc );
#else

	PGDI_ARC pfn;// = (PGDI_EXCLUDECLIPRECT)lpGDIAPI[GDI_EXCLUDECLIPRECT];

	CALLSTACK cs;
	BOOL  iRetv = FALSE;

	if( API_Enter( API_GDI, GDI_ARC, &pfn, &cs ) )
	{
		iRetv = pfn( hdc, left, top, right, bottom, xStartArc, yStartArc, xEndArc, yEndArc );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_EXTTEXTOUT )( HDC hdc, int x, int y, UINT option, LPCRECT lpRect, LPCSTR, UINT count, int * lpNextPos );
BOOL WINAPI Gdi_ExtTextOut( HDC hdc, 
						    int x, 
							int y, 
							UINT option, 
							LPCRECT lpRect, 
							LPCSTR lpcstr, 
							UINT count, 
							int * lpNextPos )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_EXTTEXTOUT, 8 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y, option, lpRect, lpcstr, count, lpNextPos );
#else

	PGDI_EXTTEXTOUT pExtTextOut;// = (PGDI_EXTTEXTOUT)lpGDIAPI[GDI_EXTTEXTOUT];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_EXTTEXTOUT, &pExtTextOut, &cs ) )
	{
		lpRect = MapProcessPtr( lpRect, (LPPROCESS)cs.lpvData );
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );
		lpNextPos = MapProcessPtr( lpNextPos, (LPPROCESS)cs.lpvData );
		

		bRetv = pExtTextOut( hdc, x, y, option, lpRect, lpcstr, count, lpNextPos );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_EXCLUDECLIPRECT ) ( HDC hdc, int left, int top, int right, int bottom );
int WINAPI Gdi_ExcludeClipRect( HDC hdc, int left, int top, int right, int bottom )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_EXCLUDECLIPRECT, 5 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, left, top, right, bottom );
#else

	PGDI_EXCLUDECLIPRECT pExcludeClipRect;// = (PGDI_EXCLUDECLIPRECT)lpGDIAPI[GDI_EXCLUDECLIPRECT];

	CALLSTACK cs;
	int  iRetv = ERROR;

	if( API_Enter( API_GDI, GDI_EXCLUDECLIPRECT, &pExcludeClipRect, &cs ) )
	{

		iRetv = pExcludeClipRect( hdc, left, top, right, bottom );
		API_Leave(  );
	}
	return iRetv;
#endif
}


typedef BOOL ( WINAPI * PGDI_FILLRECT )( HDC, LPCRECT, HBRUSH hBrush );
BOOL WINAPI Gdi_FillRect( HDC hdc, LPCRECT lpcrc, HBRUSH hBrush )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_FILLRECT, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcrc, hBrush );
#else

	PGDI_FILLRECT pFillRect;// = (PGDI_FILLRECT)lpGDIAPI[GDI_FILLRECT];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_FILLRECT, &pFillRect, &cs ) )
	{
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		bRetv = pFillRect( hdc, lpcrc, hBrush );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_FILLRGN )( HDC hdc, HRGN hrgn, HBRUSH hBrush );
BOOL WINAPI Gdi_FillRgn( HDC hdc, HRGN hrgn, HBRUSH hBrush )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_FILLRGN, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hrgn, hBrush );
#else

	PGDI_FILLRGN pFillRgn;// = (PGDI_FILLRGN)lpGDIAPI[GDI_FILLRGN];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_FILLRGN, &pFillRgn, &cs ) )
	{
		bRetv = pFillRgn( hdc, hrgn, hBrush );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_INTERSECTCLIPRECT )( HDC hdc, int left, int top, int right, int bottom );
int WINAPI Gdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_INTERSECTCLIPRECT, 5 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, left, top, right, bottom );
#else

	PGDI_INTERSECTCLIPRECT pIntersectClipRect;// = (PGDI_INTERSECTCLIPRECT)lpGDIAPI[GDI_INTERSECTCLIPRECT];

	CALLSTACK cs;
	int   iRetv = ERROR;

	if( API_Enter( API_GDI, GDI_INTERSECTCLIPRECT, &pIntersectClipRect, &cs ) )
	{
		iRetv = pIntersectClipRect( hdc, left, top, right, bottom );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef COLORREF ( WINAPI * PGDI_GETBKCOLOR )( HDC hdc );
COLORREF WINAPI Gdi_GetBkColor( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETBKCOLOR, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETBKCOLOR pGetBkColor;// = (PGDI_GETBKCOLOR)lpGDIAPI[GDI_GETBKCOLOR];
	CALLSTACK cs;
	COLORREF  clRetv = CLR_INVALID;

	if( API_Enter( API_GDI, GDI_GETBKCOLOR, &pGetBkColor, &cs ) )
	{
		clRetv = pGetBkColor( hdc );
		API_Leave(  );
	}
	return clRetv;
#endif
}

typedef int ( WINAPI * PGDI_GETBKMODE )( HDC hdc );
int WINAPI Gdi_GetBkMode( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETBKMODE, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETBKMODE pGetBkMode;// = (PGDI_GETBKMODE)lpGDIAPI[GDI_GETBKMODE];

	CALLSTACK cs;
	int   iRetv = 0;

	if( API_Enter( API_GDI, GDI_GETBKMODE, &pGetBkMode, &cs ) )
	{
		iRetv = pGetBkMode( hdc );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_GETCURRENTPOSITION )( HDC hdc, LPPOINT lpPoint );
BOOL WINAPI Gdi_GetCurrentPosition( HDC hdc, LPPOINT lpPoint )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETCURRENTPOSITION, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpPoint );
#else

	PGDI_GETCURRENTPOSITION pGetCurrentPosition;// = (PGDI_GETCURRENTPOSITION)lpGDIAPI[GDI_GETCURRENTPOSITION];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_GDI, GDI_GETCURRENTPOSITION, &pGetCurrentPosition, &cs ) )
	{
		lpPoint = MapProcessPtr( lpPoint, (LPPROCESS)cs.lpvData );

		bRetv = pGetCurrentPosition( hdc, lpPoint );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_GETCLIPBOX )( HDC hdc, LPRECT lprc );
int WINAPI Gdi_GetClipBox( HDC hdc, LPRECT lprc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETCLIPBOX, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lprc );
#else

	PGDI_GETCLIPBOX pGetClipBox;// = (PGDI_GETCLIPBOX)lpGDIAPI[GDI_GETCLIPBOX];

	CALLSTACK cs;
	int   iRetv = ERROR;

	if( API_Enter( API_GDI, GDI_GETCLIPBOX, &pGetClipBox, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		iRetv = pGetClipBox( hdc, lprc );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef int ( WINAPI * PGDI_GETCLIPRGN )( HDC hdc, HRGN hrgn );
int WINAPI Gdi_GetClipRgn( HDC hdc, HRGN hrgn )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETCLIPRGN, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hrgn );
#else

	PGDI_GETCLIPRGN pGetClipRgn;// = (PGDI_GETCLIPRGN)lpGDIAPI[GDI_GETCLIPRGN];

	CALLSTACK cs;
	int   iRetv = -1;

	if( API_Enter( API_GDI, GDI_GETCLIPRGN, &pGetClipRgn, &cs ) )
	{
		iRetv = pGetClipRgn( hdc, hrgn );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef HGDIOBJ ( WINAPI * PGDI_GETCURRENTOBJECT )( HDC hdc, UINT uObjectType );
HGDIOBJ WINAPI Gdi_GetCurrentObject( HDC hdc, UINT uObjectType )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETCURRENTOBJECT, 2 );
    cs.arg0 = (DWORD)hdc;
    return (HGDIOBJ)CALL_SERVER( &cs, uObjectType );
#else

	PGDI_GETCURRENTOBJECT pGetCurrentObject;// = (PGDI_GETCURRENTOBJECT)lpGDIAPI[GDI_GETCURRENTOBJECT];

	CALLSTACK cs;
	HGDIOBJ  hRetv = NULL;

	if( API_Enter( API_GDI, GDI_GETCURRENTOBJECT, &pGetCurrentObject, &cs ) )
	{
		hRetv = pGetCurrentObject( hdc, uObjectType );
		API_Leave(  );
	}
	return hRetv;
#endif
}

typedef int ( WINAPI * PGDI_GETDIBITS )(
						 HDC hdc,           
						 HBITMAP hbmp,      
						 UINT uStartScan,   
						 UINT cScanLines,   
						 LPVOID lpvBits,    
						 LPBITMAPINFO lpbi, 
						 UINT uiUsage );
int WINAPI Gdi_GetDIBits(
						 HDC hdc,           
						 HBITMAP hbmp,      
						 UINT uStartScan,   
						 UINT cScanLines,   
						 LPVOID lpvBits,    
						 LPBITMAPINFO lpbi, 
						 UINT uiUsage )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETDIBITS, 7 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hbmp, uStartScan, cScanLines, lpvBits, lpbi, uiUsage );
#else

	PGDI_GETDIBITS pGetDIBits;// = (PGDI_GETDIBITS)lpGDIAPI[GDI_GETDIBITS];

	int   iRetv = 0;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETDIBITS, &pGetDIBits, &cs ) )
	{
		lpvBits = MapProcessPtr( lpvBits, (LPPROCESS)cs.lpvData );
		lpbi = MapProcessPtr( lpbi, (LPPROCESS)cs.lpvData );

		iRetv = pGetDIBits( hdc, hbmp, uStartScan, cScanLines, lpvBits, lpbi, uiUsage );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef HICON ( WINAPI * PGDI_CREATEICONINDIRECT )( LPICONINFO piconinfo );
HICON WINAPI Gdi_CreateIconIndirect( LPICONINFO piconinfo )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEICONINDIRECT, 1 );
    cs.arg0 = (DWORD)piconinfo ;
    return (HICON)CALL_SERVER( &cs );
#else

	PGDI_CREATEICONINDIRECT pCreateIconIndirect;// = (PGDI_CREATEICONINDIRECT)lpGDIAPI[GDI_CREATEICONINDIRECT];

	HICON   hRetv = NULL;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_CREATEICONINDIRECT, &pCreateIconIndirect, &cs ) )
	{
		piconinfo = MapProcessPtr( piconinfo, (LPPROCESS)cs.lpvData );

		hRetv = pCreateIconIndirect( piconinfo );
		API_Leave(  );
	}
	return hRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DESTROYICON )( HICON hIcon );
BOOL WINAPI Gdi_DestroyIcon( HICON hIcon )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DESTROYICON, 1 );
    cs.arg0 = (DWORD)hIcon ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_DESTROYICON pDestroyIcon;// = (PGDI_DESTROYICON)lpGDIAPI[GDI_DESTROYICON];

	BOOL  bRetv = FALSE;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_DESTROYICON, &pDestroyIcon, &cs ) )
	{
		bRetv = pDestroyIcon( hIcon );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DESTROYCURSOR )( HCURSOR hCursor );
BOOL WINAPI Gdi_DestroyCursor( HCURSOR hCursor )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DESTROYCURSOR, 1 );
    cs.arg0 = (DWORD)hCursor ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_DESTROYCURSOR pDestroyCursor;// = (PGDI_DESTROYCURSOR)lpGDIAPI[GDI_DESTROYCURSOR];

	BOOL  bRetv = FALSE;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_DESTROYCURSOR, &pDestroyCursor, &cs ) )
	{
		bRetv = pDestroyCursor( hCursor );
		API_Leave(  );
	}
	return bRetv;
#endif
}
 
typedef BOOL ( WINAPI * PGDI_GETICONINFO )( HICON hIcon, LPICONINFO lpiconInfo);
BOOL WINAPI Gdi_GetIconInfo( HICON hIcon, LPICONINFO lpiconInfo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETICONINFO, 2 );
    cs.arg0 = (DWORD)hIcon;
    return (DWORD)CALL_SERVER( &cs, lpiconInfo );
#else

	PGDI_GETICONINFO pGetIconInfo;// = (PGDI_GETICONINFO)lpGDIAPI[GDI_GETICONINFO];

	BOOL  bRetv = FALSE;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETICONINFO, &pGetIconInfo, &cs ) )
	{
		lpiconInfo = MapProcessPtr( lpiconInfo, (LPPROCESS)cs.lpvData );

		bRetv = pGetIconInfo( hIcon, lpiconInfo );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef  ( WINAPI * PGDI_GETOBJECTTYPE )( HGDIOBJ handle );
DWORD WINAPI Gdi_GetObjectType( HGDIOBJ handle )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETOBJECTTYPE, 1 );
    cs.arg0 = (DWORD)handle ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETOBJECTTYPE pGetObjectType;// = (PGDI_GETOBJECTTYPE)lpGDIAPI[GDI_GETOBJECTTYPE];

	DWORD  dwRetv = 0;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETOBJECTTYPE, &pGetObjectType, &cs ) )
	{
		dwRetv = pGetObjectType( handle );
		API_Leave(  );
	}
	return dwRetv;
#endif
}

typedef int ( WINAPI * PGDI_GETOBJECTINFO )( HGDIOBJ handle, int nSize, LPVOID lpvBuf );
int WINAPI Gdi_GetObjectInfo( HGDIOBJ handle, int nSize, LPVOID lpvBuf )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETOBJECTINFO, 3 );
    cs.arg0 = (DWORD)handle;
    return (DWORD)CALL_SERVER( &cs, nSize, lpvBuf );
#else

	PGDI_GETOBJECTINFO pGetObjectInfo;// = (PGDI_GETOBJECTINFO)lpGDIAPI[GDI_GETOBJECTINFO];

	int  iRetv = 0;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETOBJECTINFO, &pGetObjectInfo, &cs ) )
	{
		lpvBuf = MapProcessPtr( lpvBuf, (LPPROCESS)cs.lpvData );

		iRetv = pGetObjectInfo( handle, nSize, lpvBuf );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef COLORREF ( WINAPI * PGDI_GETPIXEL )( HDC, int x, int y );
COLORREF WINAPI Gdi_GetPixel( HDC hdc, int x, int y )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETPIXEL, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y );
#else

	PGDI_GETPIXEL pGetPixel;// = (PGDI_GETPIXEL)lpGDIAPI[GDI_GETPIXEL];

	COLORREF clRetv = CLR_INVALID;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETPIXEL, &pGetPixel, &cs ) )
	{
		clRetv = pGetPixel( hdc, x, y );
		API_Leave(  );
	}
	return clRetv;
#endif
}

typedef int ( WINAPI * PGDI_GETROP2 )( HDC );
int WINAPI Gdi_GetROP2( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETROP2, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETROP2 pGetROP2;// = (PGDI_GETROP2)lpGDIAPI[GDI_GETROP2];

	int  iRetv = 0;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETROP2, &pGetROP2, &cs ) )
	{
		iRetv = pGetROP2( hdc );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef COLORREF ( WINAPI * PGDI_GETTEXTCOLOR )( HDC );
COLORREF WINAPI Gdi_GetTextColor( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETTEXTCOLOR, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETTEXTCOLOR pGetTextColor;// = (PGDI_GETTEXTCOLOR)lpGDIAPI[GDI_GETTEXTCOLOR];

	COLORREF clRetv = CLR_INVALID;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETTEXTCOLOR, &pGetTextColor, &cs ) )
	{
		clRetv = pGetTextColor( hdc );
		API_Leave(  );
	}
	return clRetv;
#endif
}

typedef UINT ( WINAPI * PGDI_GETTEXTALIGN )( HDC );
UINT WINAPI Gdi_GetTextAlign( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETTEXTALIGN, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETTEXTALIGN pfn;// = (PGDI_GETTEXTCOLOR)lpGDIAPI[GDI_GETTEXTCOLOR];

	UINT retv = GDI_ERROR;
    CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETTEXTALIGN, &pfn, &cs ) )
	{
		retv = pfn( hdc );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PGDI_GETTEXTEXTENTPOINT32 )( HDC hdc, LPCTSTR lpcstr, int nLength, LPSIZE lpSize );
BOOL WINAPI Gdi_GetTextExtentPoint32( HDC hdc, 
								    LPCTSTR lpcstr, 
									int nLength, 
									LPSIZE lpSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETTEXTEXTENTPOINT32, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcstr, nLength, lpSize );
#else

	PGDI_GETTEXTEXTENTPOINT32 pGetTextExtentPoint32;

	BOOL  bRetv = FALSE;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETTEXTEXTENTPOINT32, &pGetTextExtentPoint32, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );
		lpSize = MapProcessPtr( lpSize, (LPPROCESS)cs.lpvData );

		bRetv = pGetTextExtentPoint32( hdc, lpcstr, nLength, lpSize );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_GETTEXTEXTENTEXPOINT )( HDC hdc, LPCTSTR lpcstr, int nLength, int nMaxWidth, LPINT lpFitCharNum, LPINT lpGroupWidth, LPSIZE lpSize );
BOOL WINAPI Gdi_GetTextExtentExPoint( HDC hdc, 
									  LPCTSTR lpcstr, 
									  int nLength, 
									  int nMaxWidth, 
									  LPINT lpFitCharNum, 
									  LPINT lpGroupWidth, 
									  LPSIZE lpSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETTEXTEXTENTEXPOINT, 7 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcstr, nLength, nMaxWidth, lpFitCharNum, lpGroupWidth, lpSize );
#else

	PGDI_GETTEXTEXTENTEXPOINT pGetTextExtentExPoint;// = (PGDI_GETTEXTEXTENTEXPOINT)lpGDIAPI[GDI_GETTEXTEXTENTEXPOINT];

	BOOL  bRetv = FALSE;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETTEXTEXTENTEXPOINT, &pGetTextExtentExPoint, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );
		lpFitCharNum = MapProcessPtr( lpFitCharNum, (LPPROCESS)cs.lpvData );
		lpGroupWidth = MapProcessPtr( lpGroupWidth, (LPPROCESS)cs.lpvData );
		lpSize = MapProcessPtr( lpSize, (LPPROCESS)cs.lpvData );

		bRetv = pGetTextExtentExPoint( hdc, lpcstr, nLength, nMaxWidth, lpFitCharNum, lpGroupWidth, lpSize );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_GETVIEWPORTORG )( HDC hdc, LPPOINT lpPoint );
BOOL WINAPI Gdi_GetViewportOrg( HDC hdc, LPPOINT lpPoint )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETVIEWPORTORG, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpPoint );
#else

	PGDI_GETVIEWPORTORG pGetViewportOrg ;//= (PGDI_GETVIEWPORTORG)lpGDIAPI[GDI_GETVIEWPORTORG];

	BOOL  bRetv = FALSE;
    CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETVIEWPORTORG, &pGetViewportOrg, &cs ) )
	{
		lpPoint = MapProcessPtr( lpPoint, (LPPROCESS)cs.lpvData );

		bRetv = pGetViewportOrg( hdc, lpPoint );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PGDI_GETWINDOWORG )( HDC hdc, LPPOINT lpPoint );
BOOL WINAPI Gdi_GetWindowOrg( HDC hdc, LPPOINT lpPoint )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETWINDOWORG, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpPoint );
#else

	PGDI_GETWINDOWORG pGetWindowOrg;//= (PGDI_GETWINDOWORG)lpGDIAPI[GDI_GETWINDOWORG];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETWINDOWORG, &pGetWindowOrg, &cs ) )
	{
		lpPoint = MapProcessPtr( lpPoint, (LPPROCESS)cs.lpvData );

		bRetv = pGetWindowOrg( hdc, lpPoint );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PGDI_INVERTRECT )( HDC, LPCRECT );
BOOL WINAPI Gdi_InvertRect( HDC hdc, LPCRECT lpcrect )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_INVERTRECT, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcrect );
#else

	PGDI_INVERTRECT pInvertRect;// = (PGDI_INVERTRECT)lpGDIAPI[GDI_INVERTRECT];

    
	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_INVERTRECT, &pInvertRect, &cs ) )
	{
		lpcrect = MapProcessPtr( lpcrect, (LPPROCESS)cs.lpvData );

		bRetv = pInvertRect( hdc, lpcrect );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_INVERTRGN )( HDC, HRGN );
BOOL WINAPI Gdi_InvertRgn( HDC hdc, HRGN hrgn )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_INVERTRGN, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hrgn );
#else

	PGDI_INVERTRGN pInvertRgn;// = (PGDI_INVERTRGN)lpGDIAPI[GDI_INVERTRGN];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_INVERTRGN, &pInvertRgn, &cs ) )
	{
		bRetv = pInvertRgn( hdc, hrgn );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PGDI_LINETO )( HDC hdc, int nXEnd, int nYEnd );
BOOL WINAPI Gdi_LineTo( HDC hdc, int nXEnd, int nYEnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LINETO, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, nXEnd, nYEnd );
#else

	PGDI_LINETO pLineTo;// = (PGDI_LINETO)lpGDIAPI[GDI_LINETO];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LINETO, &pLineTo, &cs ) )
	{
		bRetv = pLineTo( hdc, nXEnd, nYEnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_LINE )( HDC hdc, int x0, int y0, int x1, int y1 );
BOOL WINAPI Gdi_Line( HDC hdc, int x0, int y0, int x1, int y1 )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LINE, 5 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x0, y0, x1, y1 );
#else

	PGDI_LINE pLine;// = (PGDI_LINE)lpGDIAPI[GDI_LINE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LINE, &pLine, &cs ) )
	{
		bRetv = pLine( hdc, x0, y0, x1, y1 );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PGDI_LPTODP )( HDC hdc, LPPOINT lpPoints, int nCount );
BOOL WINAPI Gdi_LPtoDP( HDC hdc, LPPOINT lpPoints, int nCount )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LPTODP, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpPoints, nCount );
#else

	PGDI_LPTODP pLPtoDP;// = (PGDI_LPTODP)lpGDIAPI[GDI_LPTODP];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LPTODP, &pLPtoDP, &cs ) )
	{
		lpPoints = MapProcessPtr( lpPoints, (LPPROCESS)cs.lpvData );

		bRetv = pLPtoDP( hdc, lpPoints, nCount );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_MOVETO )( HDC hdc, int x, int y, LPPOINT lppt );
BOOL WINAPI Gdi_MoveTo( HDC hdc, int x, int y, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_MOVETO, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y, lppt );
#else

	PGDI_MOVETO pMoveTo;// = (PGDI_MOVETO)lpGDIAPI[GDI_MOVETO];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_MOVETO, &pMoveTo, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		bRetv = pMoveTo( hdc, x, y, lppt );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_OFFSETCLIPRGN )( HDC hdc, int xOff, int yOff );
int WINAPI Gdi_OffsetClipRgn( HDC hdc, int xOff, int yOff )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_OFFSETCLIPRGN, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, xOff, yOff );
#else

	PGDI_OFFSETCLIPRGN pOffsetClipRgn;// = (PGDI_OFFSETCLIPRGN)lpGDIAPI[GDI_OFFSETCLIPRGN];

	int  iRetv = ERROR;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_OFFSETCLIPRGN, &pOffsetClipRgn, &cs ) )
	{
		iRetv = pOffsetClipRgn( hdc, xOff, yOff );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_OFFSETVIEWPORTORG )( HDC hdc, int xOff, int yOff, LPPOINT lppt );
BOOL WINAPI Gdi_OffsetViewportOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_OFFSETVIEWPORTORG, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, xOff, yOff, lppt );
#else

	PGDI_OFFSETVIEWPORTORG pOffsetViewportOrg;// = (PGDI_OFFSETVIEWPORTORG)lpGDIAPI[GDI_OFFSETVIEWPORTORG];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_OFFSETVIEWPORTORG, &pOffsetViewportOrg, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		bRetv = pOffsetViewportOrg( hdc, xOff, yOff, lppt );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_OFFSETWINDOWORG )( HDC hdc, int xOff, int yOff, LPPOINT lppt );
BOOL WINAPI Gdi_OffsetWindowOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_OFFSETWINDOWORG, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, xOff, yOff, lppt );
#else

	PGDI_OFFSETWINDOWORG pOffsetWindowOrg;// = (PGDI_OFFSETWINDOWORG)lpGDIAPI[GDI_OFFSETWINDOWORG];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_OFFSETWINDOWORG, &pOffsetWindowOrg, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		bRetv = pOffsetWindowOrg( hdc, xOff, yOff, lppt );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_POLYLINE )( HDC hdc, const POINT * lpcPoints, int nCount );
BOOL WINAPI Gdi_Polyline( HDC hdc, const POINT * lpcPoints, int nCount )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_POLYLINE, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcPoints, nCount );
#else

	PGDI_POLYLINE pPolyline;// = (PGDI_POLYLINE)lpGDIAPI[GDI_POLYLINE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_POLYLINE, &pPolyline, &cs ) )
	{
		lpcPoints = MapProcessPtr( lpcPoints, (LPPROCESS)cs.lpvData );

		bRetv =  pPolyline( hdc, lpcPoints, nCount );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_PTVISIBLE )( HDC, int x, int y );
BOOL WINAPI Gdi_PtVisible( HDC hdc, int x, int y )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_PTVISIBLE, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y );
#else

	PGDI_PTVISIBLE pPtVisible;// = (PGDI_PTVISIBLE)lpGDIAPI[GDI_PTVISIBLE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_PTVISIBLE, &pPtVisible, &cs ) )
	{
		bRetv =  pPtVisible( hdc, x, y );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_RECTANGLE )( HDC hdc, int left, int top, int right, int bottom );
BOOL WINAPI Gdi_Rectangle( HDC hdc, int left, int top, int right, int bottom )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_RECTANGLE, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, left, top, right, bottom );
#else

	PGDI_RECTANGLE pRectangle;// = (PGDI_RECTANGLE)lpGDIAPI[GDI_RECTANGLE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_RECTANGLE, &pRectangle, &cs ) )
	{
		bRetv = pRectangle( hdc, left, top, right, bottom );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_ROUNDRECT ) ( HDC hdc, int left, int top, int right, int bottom, int nWidth, int nHeight );
BOOL WINAPI Gdi_RoundRect( HDC hdc, int left, int top, int right, int bottom, int nWidth, int nHeight )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_ROUNDRECT, 7 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, left, top, right, bottom, nWidth, nHeight );
#else

	PGDI_ROUNDRECT pfn;// = (PGDI_EXCLUDECLIPRECT)lpGDIAPI[GDI_EXCLUDECLIPRECT];

	CALLSTACK cs;
	BOOL  iRetv = FALSE;

	if( API_Enter( API_GDI, GDI_ROUNDRECT, &pfn, &cs ) )
	{
		iRetv = pfn( hdc, left, top, right, bottom, nWidth, nHeight );
		API_Leave(  );
	}
	return iRetv;
#endif
}


typedef BOOL  ( WINAPI * PGDI_RECTVISIBLE )( HDC hdc, LPCRECT lpcrc );
BOOL WINAPI Gdi_RectVisible( HDC hdc, LPCRECT lpcrc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_RECTVISIBLE, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcrc );
#else

	PGDI_RECTVISIBLE pRectVisible;// = (PGDI_RECTVISIBLE)lpGDIAPI[GDI_RECTVISIBLE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_RECTVISIBLE, &pRectVisible, &cs ) )
	{
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		bRetv =  pRectVisible( hdc, lpcrc );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HGDIOBJ ( WINAPI * PGDI_SELECTOBJECT )(HDC hdc, HGDIOBJ handle);
HGDIOBJ WINAPI Gdi_SelectObject(HDC hdc, HGDIOBJ handle)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SELECTOBJECT, 2 );
    cs.arg0 = (DWORD)hdc;
    return (HGDIOBJ)CALL_SERVER( &cs, handle );
#else

	PGDI_SELECTOBJECT pSelectObject;// = (PGDI_SELECTOBJECT)lpGDIAPI[GDI_SELECTOBJECT];

	HGDIOBJ hRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SELECTOBJECT, &pSelectObject, &cs ) )
	{
		hRetv =  pSelectObject( hdc, handle );
		API_Leave(  );
	}
	return hRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DELETEOBJECT )(HGDIOBJ handle);
BOOL WINAPI Gdi_DeleteObject(HGDIOBJ handle)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DELETEOBJECT, 1 );
    cs.arg0 = (DWORD)handle ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_DELETEOBJECT pDeleteObject;/// = (PGDI_DELETEOBJECT)lpGDIAPI[GDI_DELETEOBJECT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_DELETEOBJECT, &pDeleteObject, &cs ) )
	{
		bRetv = pDeleteObject( handle );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_DELETEDC )(HDC hdc);
BOOL WINAPI Gdi_DeleteDC(HDC hdc)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_DELETEDC, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_DELETEDC pDeleteDC;// = (PGDI_DELETEDC)lpGDIAPI[GDI_DELETEDC];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_DELETEDC, &pDeleteDC, &cs ) )
	{
		bRetv = pDeleteDC( hdc );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_SELECTCLIPRGN )( HDC hdc, HRGN hrgn );
int WINAPI Gdi_SelectClipRgn( HDC hdc, HRGN hrgn )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SELECTCLIPRGN, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hrgn );
#else

	PGDI_SELECTCLIPRGN pSelectClipRgn;// = (PGDI_SELECTCLIPRGN)lpGDIAPI[GDI_SELECTCLIPRGN];

	int  iRetv = ERROR;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SELECTCLIPRGN, &pSelectClipRgn, &cs ) )
	{
		iRetv =  pSelectClipRgn( hdc, hrgn );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef int ( WINAPI * PGDI_EXTSELECTCLIPRGN )( HDC hdc, HRGN hrgn, int iMode );
int WINAPI Gdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int iMode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_EXTSELECTCLIPRGN, 3 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hrgn, iMode );
#else

	PGDI_EXTSELECTCLIPRGN pExtSelectClipRgn;// = (PGDI_EXTSELECTCLIPRGN)lpGDIAPI[GDI_EXTSELECTCLIPRGN];

	int  iRetv = ERROR;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_EXTSELECTCLIPRGN, &pExtSelectClipRgn, &cs ) )
	{
		iRetv = pExtSelectClipRgn( hdc, hrgn, iMode );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef int ( WINAPI * PGDI_SETBKCOLOR )( HDC hdc, COLORREF color );
COLORREF WINAPI Gdi_SetBkColor( HDC hdc, COLORREF color )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETBKCOLOR, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, color );
#else

	PGDI_SETBKCOLOR pSetBkColor;// = (PGDI_SETBKCOLOR)lpGDIAPI[GDI_SETBKCOLOR];

	COLORREF clRetv = CLR_INVALID;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETBKCOLOR, &pSetBkColor, &cs ) )
	{
		clRetv = pSetBkColor( hdc, color );
		API_Leave(  );
	}
	return clRetv;
#endif
}

typedef int ( WINAPI * PGDI_SETBKMODE )( HDC hdc, int mode );
int WINAPI Gdi_SetBkMode( HDC hdc, int mode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETBKMODE, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, mode );
#else

	PGDI_SETBKMODE pSetBkMode;// = (PGDI_SETBKMODE)lpGDIAPI[GDI_SETBKMODE];

	int  iRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETBKMODE, &pSetBkMode, &cs ) )
	{
		iRetv = pSetBkMode( hdc, mode );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PGDI_SETBRUSHORG )( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );
BOOL WINAPI Gdi_SetBrushOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETBRUSHORG, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, xOrg, yOrg, lppt );
#else

	PGDI_SETBRUSHORG pSetBrushOrg;// = (PGDI_SETBRUSHORG)lpGDIAPI[GDI_SETBRUSHORG];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETBRUSHORG, &pSetBrushOrg, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		bRetv = pSetBrushOrg( hdc, xOrg, yOrg, lppt );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PGDI_SETMAPMODE )( HDC hdc, int mode );
int WINAPI Gdi_SetMapMode( HDC hdc, int mode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETMAPMODE, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, mode );
#else

	PGDI_SETMAPMODE pSetMapMode;// = (PGDI_SETMAPMODE)lpGDIAPI[GDI_SETMAPMODE];

	int   retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETMAPMODE, &pSetMapMode, &cs ) )
	{
		retv = pSetMapMode( hdc, mode );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef COLORREF ( WINAPI * PGDI_SETPIXEL )( HDC hdc, int x, int y, COLORREF color );
COLORREF WINAPI Gdi_SetPixel( HDC hdc, int x, int y, COLORREF color )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETPIXEL, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y, color );
#else

	PGDI_SETPIXEL pSetPixel;// = (PGDI_SETPIXEL)lpGDIAPI[GDI_SETPIXEL];

	COLORREF   retv = CLR_INVALID;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETPIXEL, &pSetPixel, &cs ) )
	{
		retv = pSetPixel( hdc, x, y, color );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PGDI_SETROP2 )( HDC hdc , int fMode );
int WINAPI Gdi_SetROP2( HDC hdc , int fMode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETROP2, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, fMode );
#else

	PGDI_SETROP2 pSetROP2;// = (PGDI_SETROP2)lpGDIAPI[GDI_SETROP2];

	int   retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETROP2, &pSetROP2, &cs ) )
	{
		retv = pSetROP2( hdc, fMode );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef COLORREF ( WINAPI * PGDI_SETTEXTCOLOR )( HDC hdc, COLORREF color );
COLORREF WINAPI Gdi_SetTextColor( HDC hdc, COLORREF color )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETTEXTCOLOR, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, color );
#else

	PGDI_SETTEXTCOLOR pSetTextColor;// = (PGDI_SETTEXTCOLOR)lpGDIAPI[GDI_SETTEXTCOLOR];

	COLORREF   retv = CLR_INVALID;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETTEXTCOLOR, &pSetTextColor, &cs ) )
	{
		retv = pSetTextColor( hdc, color );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PGDI_SETTEXTALIGN )( HDC hdc, UINT uiAlignMode);
UINT WINAPI Gdi_SetTextAlign( HDC hdc, UINT uiAlignMode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETTEXTALIGN, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, uiAlignMode );
#else

	PGDI_SETTEXTALIGN pfn;// = (PGDI_SETTEXTCOLOR)lpGDIAPI[GDI_SETTEXTCOLOR];

	UINT   retv = GDI_ERROR;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETTEXTALIGN, &pfn, &cs ) )
	{
		retv = pfn( hdc, uiAlignMode );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PGDI_SETVIEWPORTORG )( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );
BOOL WINAPI Gdi_SetViewportOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETVIEWPORTORG, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, xOrg, yOrg, lppt );
#else

	PGDI_SETVIEWPORTORG pSetViewportOrg;// = (PGDI_SETVIEWPORTORG)lpGDIAPI[GDI_SETVIEWPORTORG];

	BOOL   retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETVIEWPORTORG, &pSetViewportOrg, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		retv = pSetViewportOrg( hdc, xOrg, yOrg, lppt );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PGDI_SETWINDOWORG )( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );
BOOL WINAPI Gdi_SetWindowOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETWINDOWORG, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, xOrg, yOrg, lppt );
#else

	PGDI_SETWINDOWORG pSetWindowOrg;// = (PGDI_SETWINDOWORG)lpGDIAPI[GDI_SETWINDOWORG];

	BOOL   retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETWINDOWORG, &pSetWindowOrg, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		retv = pSetWindowOrg( hdc, xOrg, yOrg, lppt );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PGDI_TEXTOUT )( HDC hdc, int x, int y, LPCSTR lpcstr, int count );
BOOL WINAPI Gdi_TextOut( HDC hdc, int x, int y, LPCSTR lpcstr, int count )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_TEXTOUT, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y, lpcstr, count );
#else

	PGDI_TEXTOUT pTextOut;// = (PGDI_TEXTOUT)lpGDIAPI[GDI_TEXTOUT];

	BOOL   retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_TEXTOUT, &pTextOut, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );

		retv = pTextOut( hdc, x, y, lpcstr, count );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HPEN ( WINAPI * PGDI_CREATEPENINDIRECT )( const LOGPEN * pLogPen );
HPEN WINAPI Gdi_CreatePenIndirect( const LOGPEN * pLogPen )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEPENINDIRECT, 1 );
    cs.arg0 = (DWORD)pLogPen ;
    return (HPEN)CALL_SERVER( &cs );
#else

	PGDI_CREATEPENINDIRECT pCreatePenIndirect;// = (PGDI_CREATEPENINDIRECT)lpGDIAPI[GDI_CREATEPENINDIRECT];

	HPEN   retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEPENINDIRECT, &pCreatePenIndirect, &cs ) )
	{
		pLogPen = MapProcessPtr( pLogPen, (LPPROCESS)cs.lpvData );

		retv = pCreatePenIndirect( pLogPen );
		API_Leave(  );
	}
	return retv;
#endif	
}

typedef HPEN ( WINAPI * PGDI_CREATEPEN )( int fnPenStyle, int nWidth, COLORREF crColor );
HPEN WINAPI Gdi_CreatePen( int fnPenStyle, int nWidth, COLORREF crColor )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEPEN, 3 );
    cs.arg0 = (DWORD)fnPenStyle;
    return (HPEN)CALL_SERVER( &cs, nWidth, crColor );
#else

	PGDI_CREATEPEN pCreatePen;// = (PGDI_CREATEPEN)lpGDIAPI[GDI_CREATEPEN];

	HPEN   retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEPEN, &pCreatePen, &cs ) )
	{
		retv = pCreatePen( fnPenStyle, nWidth, crColor );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBRUSH ( WINAPI * PGDI_CREATEBRUSHINDIRECT )(const LOGBRUSH*);
HBRUSH WINAPI Gdi_CreateBrushIndirect( const LOGBRUSH* pBrush )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEBRUSHINDIRECT, 1 );
    cs.arg0 = (DWORD)pBrush ;
    return (HBRUSH)CALL_SERVER( &cs );
#else

	PGDI_CREATEBRUSHINDIRECT pCreateBrushIndirect;// = (PGDI_CREATEBRUSHINDIRECT)lpGDIAPI[GDI_CREATEBRUSHINDIRECT];

	HBRUSH retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEBRUSHINDIRECT, &pCreateBrushIndirect, &cs ) )
	{
		pBrush = MapProcessPtr( pBrush, (LPPROCESS)cs.lpvData );

		retv = pCreateBrushIndirect( pBrush );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBRUSH ( WINAPI * PGDI_CREATEHATCHBRUSH )( int style, COLORREF color );
HBRUSH WINAPI Gdi_CreateHatchBrush( int style, COLORREF color )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEHATCHBRUSH, 2 );
    cs.arg0 = (DWORD)style;
    return (HBRUSH)CALL_SERVER( &cs, color );
#else

	PGDI_CREATEHATCHBRUSH pCreateHatchBrush;// = (PGDI_CREATEHATCHBRUSH)lpGDIAPI[GDI_CREATEHATCHBRUSH];

	HBRUSH retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEHATCHBRUSH, &pCreateHatchBrush, &cs ) )
	{
		retv = pCreateHatchBrush( style, color );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBRUSH ( WINAPI * PGDI_CREATESOLIDBRUSH )( COLORREF );
HBRUSH WINAPI Gdi_CreateSolidBrush( COLORREF color)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATESOLIDBRUSH, 1 );
    cs.arg0 = (DWORD)color ;
    return (HBRUSH)CALL_SERVER( &cs );
#else

	PGDI_CREATESOLIDBRUSH pCreateSolidBrush;// = (PGDI_CREATESOLIDBRUSH)lpGDIAPI[GDI_CREATESOLIDBRUSH];

	HBRUSH retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATESOLIDBRUSH, &pCreateSolidBrush, &cs ) )
	{
		retv = pCreateSolidBrush( color );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBITMAP ( WINAPI * PGDI_CREATEBITMAP )(int nWidth, int nHeight,  UINT nPlanes, UINT nBitsPerPel, const VOID * lpcBits);
HBITMAP WINAPI Gdi_CreateBitmap(int nWidth, 
								int nHeight,
								UINT nPlanes,
								UINT nBitsPerPel,
								const VOID * lpcBits)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEBITMAP, 5 );
    cs.arg0 = (DWORD)nWidth;
    return (HBITMAP)CALL_SERVER( &cs, nHeight, nPlanes, nBitsPerPel, lpcBits );
#else

	PGDI_CREATEBITMAP pCreateBitmap;// = (PGDI_CREATEBITMAP)lpGDIAPI[GDI_CREATEBITMAP];

	HBITMAP retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEBITMAP, &pCreateBitmap, &cs ) )
	{
		lpcBits = MapProcessPtr( lpcBits, (LPPROCESS)cs.lpvData );

		retv = pCreateBitmap( nWidth, nHeight, nPlanes, nBitsPerPel, lpcBits );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBITMAP ( WINAPI * PGDI_CREATECOMPATIBLEBITMAP )( HDC hdc, int nWidth, int nHeight );
HBITMAP WINAPI Gdi_CreateCompatibleBitmap( HDC hdc, int nWidth, int nHeight )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATECOMPATIBLEBITMAP, 3 );
    cs.arg0 = (DWORD)hdc;
    return (HBITMAP)CALL_SERVER( &cs, nWidth, nHeight );
#else

	PGDI_CREATECOMPATIBLEBITMAP pCreateCompatibleBitmap;// = (PGDI_CREATECOMPATIBLEBITMAP)lpGDIAPI[GDI_CREATECOMPATIBLEBITMAP];

	HBITMAP retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATECOMPATIBLEBITMAP, &pCreateCompatibleBitmap, &cs ) )
	{
		retv = pCreateCompatibleBitmap( hdc, nWidth, nHeight );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBITMAP ( WINAPI * PGDI_CREATEBITMAPINDIRECT )( const BITMAP * pBitmap );
HBITMAP WINAPI Gdi_CreateBitmapIndirect( const BITMAP * pBitmap )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEBITMAPINDIRECT, 1 );
    cs.arg0 = (DWORD)pBitmap ;
    return (HBITMAP)CALL_SERVER( &cs );
#else

	PGDI_CREATEBITMAPINDIRECT pCreateBitmapIndirect;// = (PGDI_CREATEBITMAPINDIRECT)lpGDIAPI[GDI_CREATEBITMAPINDIRECT];

	HBITMAP retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEBITMAPINDIRECT, &pCreateBitmapIndirect, &cs ) )
	{
		pBitmap = MapProcessPtr( pBitmap, (LPPROCESS)cs.lpvData );

		retv = pCreateBitmapIndirect( pBitmap );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HDC ( WINAPI * PGDI_CREATECOMPATIBLEDC )( HDC hdc );
HDC WINAPI Gdi_CreateCompatibleDC( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATECOMPATIBLEDC, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (HDC)CALL_SERVER( &cs );
#else

	PGDI_CREATECOMPATIBLEDC pCreateCompatibleDC;// = (PGDI_CREATECOMPATIBLEDC)lpGDIAPI[GDI_CREATECOMPATIBLEDC];

	HDC retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATECOMPATIBLEDC, &pCreateCompatibleDC, &cs ) )
	{
		retv = pCreateCompatibleDC( hdc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBITMAP ( WINAPI * PGDI_CREATEDIBSECTION )( 
						 HDC hdc, 
						 CONST BITMAPINFO *lpbmi,
						 UINT iUsage,         //must = DIB_PAL_COLORS
						 VOID ** lppvBits,
						 HANDLE hSection,  // must = NULL
						 DWORD dwOffset   // = 0
						 );
HBITMAP WINAPI Gdi_CreateDIBSection ( 
						 HDC hdc, 
						 CONST BITMAPINFO *lpbmi,
						 UINT iUsage,         //must = DIB_PAL_COLORS
						 VOID ** lppvBits,
						 HANDLE hSection,  // must = NULL
						 DWORD dwOffset   // = 0
						 )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEDIBSECTION, 6 );
    cs.arg0 = (DWORD)hdc;
    return (HBITMAP)CALL_SERVER( &cs, lpbmi, iUsage, lppvBits, hSection, dwOffset );
#else

	PGDI_CREATEDIBSECTION pCreateDIBSection;// = (PGDI_CREATEDIBSECTION)lpGDIAPI[GDI_CREATEDIBSECTION];

	HBITMAP retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEDIBSECTION, &pCreateDIBSection, &cs ) )
	{
		lpbmi = MapProcessPtr( lpbmi, (LPPROCESS)cs.lpvData );
		lppvBits = MapProcessPtr( lppvBits, (LPPROCESS)cs.lpvData );

		retv = pCreateDIBSection( hdc, lpbmi, iUsage, lppvBits, hSection, dwOffset );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PGDI_SETDIBITS )(
						 HDC hdc,                  
						 HBITMAP hbmp,             
						 UINT uStartScan,          
						 UINT cScanLines,          
						 CONST VOID *lpvBits,      
						 CONST BITMAPINFO *lpbmi,  
						 UINT fuColorUse           
						 );
int WINAPI Gdi_SetDIBits(
						 HDC hdc,                  
						 HBITMAP hbmp,             
						 UINT uStartScan,          
						 UINT cScanLines,          
						 CONST VOID *lpvBits,      
						 CONST BITMAPINFO *lpbmi,  
						 UINT fuColorUse           
						 )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETDIBITS, 7 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse );
#else

	PGDI_SETDIBITS pSetDIBits;// = (PGDI_SETDIBITS)lpGDIAPI[GDI_SETDIBITS];

	int retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETDIBITS, &pSetDIBits, &cs ) )
	{
		lpvBits = MapProcessPtr( lpvBits, (LPPROCESS)cs.lpvData );
		lpbmi = MapProcessPtr( lpbmi, (LPPROCESS)cs.lpvData );

		retv = pSetDIBits( hdc, hbmp, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBITMAP ( WINAPI * PGDI_CREATEDIBITMAP )(
								  HDC hdc,                        
								  CONST BITMAPINFOHEADER *lpbmih, 
								  DWORD fdwInit,                  
								  CONST VOID *lpbInit,            
								  CONST BITMAPINFO *lpbmi,        
								  UINT fuUsage                    
								  );
HBITMAP WINAPI Gdi_CreateDIBitmap(
								  HDC hdc,                        
								  CONST BITMAPINFOHEADER *lpbmih, 
								  DWORD fdwInit,                  
								  CONST VOID *lpbInit,            
								  CONST BITMAPINFO *lpbmi,        
								  UINT fuUsage                    
								  )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEDIBITMAP, 6 );
    cs.arg0 = (DWORD)hdc;
    return (HBITMAP)CALL_SERVER( &cs, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage );
#else

	PGDI_CREATEDIBITMAP pCreateDIBitmap;// = (PGDI_CREATEDIBITMAP)lpGDIAPI[GDI_CREATEDIBITMAP];

	HBITMAP retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEDIBITMAP, &pCreateDIBitmap, &cs ) )
	{
		lpbmih = MapProcessPtr( lpbmih, (LPPROCESS)cs.lpvData );
		lpbInit = MapProcessPtr( lpbInit, (LPPROCESS)cs.lpvData );
		lpbmi = MapProcessPtr( lpbmi, (LPPROCESS)cs.lpvData );

		retv = pCreateDIBitmap( hdc, lpbmih, fdwInit, lpbInit, lpbmi, fuUsage );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HGDIOBJ ( WINAPI * PGDI_GETSTOCKOBJECT )( int fObject );
HGDIOBJ WINAPI Gdi_GetStockObject( int fObject )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETSTOCKOBJECT, 1 );
    cs.arg0 = (DWORD)fObject ;
    return (HGDIOBJ)CALL_SERVER( &cs );
#else

	PGDI_GETSTOCKOBJECT pGetStockObject;// = (PGDI_GETSTOCKOBJECT)lpGDIAPI[GDI_GETSTOCKOBJECT];

	HGDIOBJ retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETSTOCKOBJECT, &pGetStockObject, &cs ) )
	{
		retv = pGetStockObject( fObject );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PGDI_GETTEXTMETRICS )( HDC hdc, LPTEXTMETRIC lptm );
BOOL WINAPI Gdi_GetTextMetrics( HDC hdc, LPTEXTMETRIC lptm )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETTEXTMETRICS, 2 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lptm );
#else

	PGDI_GETTEXTMETRICS pGetTextMetrics;// = (PGDI_GETTEXTMETRICS)lpGDIAPI[GDI_GETTEXTMETRICS];

	BOOL retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETTEXTMETRICS, &pGetTextMetrics, &cs ) )
	{
		lptm = MapProcessPtr( lptm, (LPPROCESS)cs.lpvData );

		retv = pGetTextMetrics( hdc, lptm );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PGDI_GETCHARWIDTH )( HDC hdc, WORD iFirst, WORD iLast, LPINT lpiBuf );
BOOL WINAPI Gdi_GetCharWidth( HDC hdc, WORD iFirst, WORD iLast, LPINT lpiBuf )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETCHARWIDTH, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, iFirst, iLast, lpiBuf );
#else

	PGDI_GETCHARWIDTH pGetCharWidth;// = (PGDI_GETCHARWIDTH)lpGDIAPI[GDI_GETCHARWIDTH];

	BOOL retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETCHARWIDTH, &pGetCharWidth, &cs ) )
	{
		lpiBuf = MapProcessPtr( lpiBuf, (LPPROCESS)cs.lpvData );

		retv = pGetCharWidth( hdc, iFirst, iLast, lpiBuf );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HANDLE ( WINAPI * PGDI_LOADIMAGE )( 
                     HINSTANCE hinst,
                     LPCTSTR lpszName, 
                     UINT uType, 
                     int cxDesired, 
                     int cyDesired, 
                     UINT fuLoad 
                     );
HANDLE WINAPI Gdi_LoadImage( 
                     HINSTANCE hinst,
                     LPCTSTR lpszName, 
                     UINT uType, 
                     int cxDesired, 
                     int cyDesired, 
                     UINT fuLoad 
                     )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LOADIMAGE, 6 );
    cs.arg0 = (DWORD)hinst;
    return (HANDLE)CALL_SERVER( &cs, lpszName, uType, cxDesired, cyDesired, fuLoad );
#else

	PGDI_LOADIMAGE pLoadImage;// = (PGDI_LOADIMAGE)lpGDIAPI[GDI_LOADIMAGE];

	HANDLE retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LOADIMAGE, &pLoadImage, &cs ) )
	{
		lpszName = MapProcessPtr( lpszName, (LPPROCESS)cs.lpvData );

		retv = pLoadImage( hinst, lpszName, uType, cxDesired, cyDesired, fuLoad );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HICON ( WINAPI * PGDI_LOADICON )(  HINSTANCE hInstance, LPCTSTR lpIconName );
HICON WINAPI Gdi_LoadIcon(  HINSTANCE hInstance, LPCTSTR lpIconName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LOADICON, 2 );
    cs.arg0 = (DWORD)hInstance;
    return (HICON)CALL_SERVER( &cs, lpIconName );
#else

	PGDI_LOADICON pLoadIcon;// = (PGDI_LOADICON)lpGDIAPI[GDI_LOADICON];

	HICON retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LOADICON, &pLoadIcon, &cs ) )
	{
		lpIconName = MapProcessPtr( lpIconName, (LPPROCESS)cs.lpvData );

		retv = pLoadIcon( hInstance, lpIconName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HCURSOR ( WINAPI * PGDI_LOADCURSOR )( HINSTANCE hInstance, LPCTSTR lpCursorName );
HCURSOR WINAPI Gdi_LoadCursor( HINSTANCE hInstance, LPCTSTR lpCursorName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LOADCURSOR, 2 );
    cs.arg0 = (DWORD)hInstance;
    return (HCURSOR)CALL_SERVER( &cs, lpCursorName );
#else

	PGDI_LOADCURSOR pLoadCursor;// = (PGDI_LOADCURSOR)lpGDIAPI[GDI_LOADCURSOR];

	HCURSOR retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LOADCURSOR, &pLoadCursor, &cs ) )
	{
		lpCursorName = MapProcessPtr( lpCursorName, (LPPROCESS)cs.lpvData );

		retv = pLoadCursor( hInstance, lpCursorName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PGDI_SETDIBITSTODEVICE )(
						 HDC hdc,
						 int xDest,
						 int yDest,
						 DWORD dwWidth,
						 DWORD dwHeight,
						 int xSrc,
						 int ySrc,
						 UINT uStartScan,
						 UINT cScanLines,
						 CONST VOID *lpvBits,
						 CONST BITMAPINFO *lpbmi,
						 UINT fuColorUse );
													
int WINAPI Gdi_SetDIBitsToDevice(
						 HDC hdc,
						 int xDest,
						 int yDest,
						 DWORD dwWidth,

						 DWORD dwHeight,
						 int xSrc,
						 int ySrc,
						 UINT uStartScan,

						 UINT cScanLines,
						 CONST VOID *lpvBits,
						 CONST BITMAPINFO *lpbmi,
						 UINT fuColorUse
						 )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETDIBITSTODEVICE, 12 );
    cs.arg0 = (DWORD)
						  hdc;
    return (DWORD)CALL_SERVER( &cs,
						  xDest,
						  yDest,
						  dwWidth,
						  dwHeight,
						  xSrc,
						  ySrc,
						  uStartScan,
						  cScanLines,
						  lpvBits,
						  lpbmi,
						  fuColorUse );
#else

	PGDI_SETDIBITSTODEVICE pfn;// = (PGDI_LOADCURSOR)lpGDIAPI[GDI_LOADCURSOR];

	int retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETDIBITSTODEVICE, &pfn, &cs ) )
	{
		lpvBits = MapProcessPtr( lpvBits, (LPPROCESS)cs.lpvData );
		lpbmi = MapProcessPtr( lpbmi, (LPPROCESS)cs.lpvData );

		retv = pfn(	hdc,
					xDest,
					yDest,
					dwWidth,
					dwHeight,
					xSrc,
					ySrc,
					uStartScan,
					cScanLines,
					lpvBits,
					lpbmi,
					fuColorUse );
		API_Leave();
	}
	return retv;
#endif
}


typedef int ( WINAPI * PGDI_GETMAPMODE )( HDC hdc );
int WINAPI Gdi_GetMapMode( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETMAPMODE, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_GETMAPMODE pfn;// = (PGDI_GETTEXTMETRICS)lpGDIAPI[GDI_GETTEXTMETRICS];

	int retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETMAPMODE, &pfn, &cs ) )
	{
		retv = pfn( hdc );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PGDI_TEXTOUTW )( HDC hdc, int x, int y, LPCTSTR lpcstr, int count );
BOOL WINAPI Gdi_TextOutW( HDC hdc, int x, int y, LPCTSTR lpcstr, int count )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_TEXTOUTW, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, x, y, lpcstr, count );
#else

	PGDI_TEXTOUTW pTextOutW;// = (PGDI_TEXTOUT)lpGDIAPI[GDI_TEXTOUT];

	BOOL   retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_TEXTOUTW, &pTextOutW, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );

		retv = pTextOutW( hdc, x, y, lpcstr, count );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PGDI_GETTEXTEXTENTPOINT32W )( HDC hdc, LPCTSTR lpcstr, int nLength, LPSIZE lpSize );
BOOL WINAPI Gdi_GetTextExtentPoint32W( HDC hdc, 
								    LPCTSTR lpcstr, 
									int nLength, 
									LPSIZE lpSize )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETTEXTEXTENTPOINT32W, 4 );
    cs.arg0 = (DWORD)hdc;
    return (DWORD)CALL_SERVER( &cs, lpcstr, nLength, lpSize );
#else

	PGDI_GETTEXTEXTENTPOINT32W pfn;

	BOOL  bRetv = FALSE;
    CALLSTACK cs;
	if( API_Enter( API_GDI, GDI_GETTEXTEXTENTPOINT32W, &pfn, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );
		lpSize = MapProcessPtr( lpSize, (LPPROCESS)cs.lpvData );

		bRetv = pfn( hdc, lpcstr, nLength, lpSize );
		API_Leave();
	}
	return bRetv;
#endif
}

typedef HFONT ( WINAPI * PGDI_CREATEFONTINDIRECT )( CONST LOGFONT *lplf );
HFONT WINAPI Gdi_CreateFontIndirect( CONST LOGFONT *lplf )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_CREATEFONTINDIRECT, 1 );
    cs.arg0 = (DWORD)lplf ;
    return (HFONT)CALL_SERVER( &cs );
#else

	PGDI_CREATEFONTINDIRECT pfn;// = (PGDI_CREATEBRUSHINDIRECT)lpGDIAPI[GDI_CREATEBRUSHINDIRECT];

	HFONT retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_CREATEFONTINDIRECT, &pfn, &cs ) )
	{
		lplf = MapProcessPtr( lplf, (LPPROCESS)cs.lpvData );

		retv = pfn( lplf );
		API_Leave(  );
	}
	return retv;
#endif
}

BOOL WINAPI Gdi_Pie(
  HDC hdc,         // handle to DC
  int nLeftRect,   // x-coord of upper-left corner of rectangle
  int nTopRect,    // y-coord of upper-left corner of rectangle
  int nRightRect,  // x-coord of lower-right corner of rectangle
  int nBottomRect, // y-coord of lower-right corner of rectangle
  int nXRadial1,   // x-coord of first radial's endpoint
  int nYRadial1,   // y-coord of first radial's endpoint
  int nXRadial2,   // x-coord of second radial's endpoint
  int nYRadial2    // y-coord of second radial's endpoint
)
{
	MessageBox(NULL, "There no Pie!", NULL, MB_OK);
	return TRUE;
}

BOOL WINAPI Gdi_Polygon(
  HDC hdc,                // handle to DC
  CONST POINT *lpPoints,  // polygon vertices
  int nCount              // count of polygon vertices
)
{
	MessageBox(NULL, "There no Polygon!", NULL, MB_OK);
	return TRUE;
}

BOOL WINAPI Gdi_PlgBlt(
  HDC hdcDest,          // handle to destination DC
  CONST POINT *lpPoint, // destination vertices
  HDC hdcSrc,           // handle to source DC
  int nXSrc,            // x-coord of source upper-left corner
  int nYSrc,            // y-coord of source upper-left corner
  int nWidth,           // width of source rectangle
  int nHeight,          // height of source rectangle
  HBITMAP hbmMask,      // handle to bitmask
  int xMask,            // x-coord of bitmask upper-left corner
  int yMask             // y-coord of bitmask upper-left corner
)
{
	MessageBox(NULL, "There no PlgBlt!", NULL, MB_OK);
	return TRUE;
}

typedef BOOL ( WINAPI * PGDI_TRANSPARENTBLT )( HDC hdcDest, 
								  int xDest, 
								  int yDest, 
								  int width, 
								  int height,
								  HDC hdcSrc,
								  int xSrc, 
								  int ySrc, 
								  int widthSrc,
								  int heightSrc,
								  UINT clTransparent );
BOOL WINAPI Gdi_TransparentBlt(HDC hdcDest, 
								  int xDest, 
								  int yDest, 
								  int width, 
								  int height,
								  HDC hdcSrc,
								  int xSrc, 
								  int ySrc, 
								  int widthSrc,
								  int heightSrc,
								  DWORD clTransparent )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_TRANSPARENTBLT, 11 );
    cs.arg0 = (DWORD)hdcDest ;
    return (BOOL)CALL_SERVER( &cs, xDest,yDest,width,height,hdcSrc,xSrc,ySrc,widthSrc,heightSrc,clTransparent );
#else

	PGDI_TRANSPARENTBLT pfn;// = (PGDI_CREATEBRUSHINDIRECT)lpGDIAPI[GDI_CREATEBRUSHINDIRECT];

	BOOL retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_TRANSPARENTBLT, &pfn, &cs ) )
	{
		retv = pfn( hdcDest, xDest,yDest,width,height,hdcSrc,xSrc,ySrc,widthSrc,heightSrc,clTransparent );
		API_Leave();
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PGDI_StretchBlt )
				(
				HDC hdcDest, 
				int xDest, 
				int yDest, 
				int nWidthDest, 
				int nHeightDest, 
				HDC hdcSrc, 
				int xSrc, 
				int ySrc, 
				int nWidthSrc, 
				int nHeightSrc, 
				DWORD dwRop );
BOOL WINAPI Gdi_StretchBlt(
				HDC hdcDest, 
				int xDest, 
				int yDest, 
				int nWidthDest, 
				int nHeightDest, 
				HDC hdcSrc, 
				int xSrc, 
				int ySrc, 
				int nWidthSrc, 
				int nHeightSrc, 
				DWORD dwRop )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_STRETCHBLT, 11 );
    cs.arg0 = (DWORD)hdcDest ;
    return (BOOL)CALL_SERVER( &cs, xDest,yDest,nWidthDest,nHeightDest,hdcSrc,xSrc,ySrc,nWidthSrc,nHeightSrc,dwRop );
#else

	PGDI_StretchBlt pfn;

	BOOL retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_STRETCHBLT, &pfn, &cs ) )
	{
		retv = pfn( hdcDest, xDest,yDest,nWidthDest,nHeightDest,hdcSrc,xSrc,ySrc,nWidthSrc,nHeightSrc,dwRop );
		API_Leave();
	}
	return retv;
#endif
}


typedef HWND ( WINAPI * PGDI_WINDOWFROMDC )( HDC hdc );
HWND WINAPI Gdi_WindowFromDC( HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_WINDOWFROMDC, 1 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PGDI_WINDOWFROMDC pfn;

	HWND retv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_WINDOWFROMDC, &pfn, &cs ) )
	{
		retv = pfn( hdc );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef UINT ( WINAPI * PGDI_SETDIBCOLORTABLE )( HDC hdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors );
UINT WINAPI Gdi_SetDIBColorTable( HDC hdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETDIBCOLORTABLE, 4 );
    cs.arg0 = (DWORD)hdc ;
    return (UINT)CALL_SERVER( &cs, uStartIndex, cEntries, pColors );
#else

	PGDI_SETDIBCOLORTABLE pfn;

	UINT retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETDIBCOLORTABLE, &pfn, &cs ) )
	{
		retv = pfn( hdc, uStartIndex, cEntries, pColors );
		API_Leave();
	}
	return retv;
#endif
}

typedef int ( WINAPI * PGDI_GETDEVICECAPS )( HDC hdc, int nIndex );
int WINAPI Gdi_GetDeviceCaps( HDC hdc, int nIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_GETDEVICECAPS, 2 );
    cs.arg0 = (DWORD)hdc ;
    return (DWORD)CALL_SERVER( &cs, nIndex );
#else

	PGDI_GETDEVICECAPS pfn;

	int retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_GETDEVICECAPS, &pfn, &cs ) )
	{
		retv = pfn( hdc, nIndex );
		API_Leave();
	}
	return retv;
#endif
}


typedef BOOL ( WINAPI * PGDI_SetPixelV )( HDC hdc, int x, int y, COLORREF color );
BOOL WINAPI Gdi_SetPixelV( HDC hdc, int x, int y, COLORREF color )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_SETPIXEL, 4 );
    cs.arg0 = (DWORD)hdc;
    return ( (DWORD)CALL_SERVER( &cs, x, y, color ) != (DWORD)-1 );
#else

	PGDI_SETPIXEL pSetPixel;// = (PGDI_SETPIXEL)lpGDIAPI[GDI_SETPIXEL];

	COLORREF   retv = CLR_INVALID;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_SETPIXEL, &pSetPixel, &cs ) )
	{
		retv = pSetPixel( hdc, x, y, color );
		API_Leave(  );
	}
	return retv != (DWORD)-1;
#endif
}


typedef BOOL ( WINAPI * PGdi_AlphaBlendEx )(
					HDC hdcDest,
					int nXOriginDest,
					int nYOriginDest,
					int nWidthDest,
					int nHeightDest,
					HDC hdcSrc,
					int nXOriginSrc,
					int nYOriginSrc,
					int nWidthSrc,
					int nHeightSrc,
					LPBLENDFUNCTION_EX lpBlendFunction  // alpha-blending function
					);
BOOL WINAPI Gdi_AlphaBlendEx(
					HDC hdcDest,
					int nXOriginDest,
					int nYOriginDest,
					int nWidthDest,
					int nHeightDest,
					HDC hdcSrc,
					int nXOriginSrc,
					int nYOriginSrc,
					int nWidthSrc,
					int nHeightSrc,
					LPBLENDFUNCTION_EX lpBlendFunction  // alpha-blending function
					)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_ALPHABLEND_EX, 11 );
    cs.arg0 = (DWORD)hdcDest;
    return (BOOL)CALL_SERVER( &cs, nXOriginDest, nYOriginDest, nWidthDest,nHeightDest,hdcSrc,nXOriginSrc,nYOriginSrc,nWidthSrc,nHeightSrc,lpBlendFunction );
#else

	PGdi_AlphaBlendEx pfn;// = (PGDI_SETPIXEL)lpGDIAPI[GDI_SETPIXEL];

	BOOL   retv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_ALPHABLEND_EX, &pfn, &cs ) )
	{
		retv = pfn( hdcDest, nXOriginDest, nYOriginDest, nWidthDest,nHeightDest,hdcSrc,nXOriginSrc,nYOriginSrc,nWidthSrc,nHeightSrc,lpBlendFunction );
		API_Leave();
	}
	return retv;
#endif
}


BOOL WINAPI Gdi_AlphaBlend(
					HDC hdcDest,
					int nXOriginDest,
					int nYOriginDest,
					int nWidthDest,
					int nHeightDest,
					HDC hdcSrc,
					int nXOriginSrc,
					int nYOriginSrc,
					int nWidthSrc,
					int nHeightSrc,
					BLENDFUNCTION blendFunction  // alpha-blending function
					)
{
	BLENDFUNCTION_EX bgex;

	bgex.bf = blendFunction;
	bgex.rgbTransparent = -1;  //透明RGB, -1 无效
	return Gdi_AlphaBlendEx( 
					hdcDest,
					nXOriginDest,
					nYOriginDest,
					nWidthDest,
					nHeightDest,
					hdcSrc,
					nXOriginSrc,
					nYOriginSrc,
					nWidthSrc,
					nHeightSrc,
					&bgex );
}

typedef int ( WINAPI * PGdi_LoadString )(
  HINSTANCE hInstance,  // handle to resource module
  UINT uID,             // resource identifier
  LPTSTR lpBuffer,      // resource buffer
  int nBufferMax        // size of buffer
);


//从实例装入字符串资源
int WINAPI Gdi_LoadString(
  HINSTANCE hInstance,  // handle to resource module
  UINT uID,             // resource identifier
  LPTSTR lpBuffer,      // resource buffer
  int nBufferMax        // size of buffer
)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, GDI_LOADSTRING, 4 );
    cs.arg0 = (DWORD)hInstance;
    return (BOOL)CALL_SERVER( &cs, uID, lpBuffer, nBufferMax );
#else

	PGdi_LoadString pfn;

	int   retv = 0;
	CALLSTACK cs;

	if( API_Enter( API_GDI, GDI_LOADSTRING, &pfn, &cs ) )
	{
		retv = pfn( hInstance, uID, lpBuffer, nBufferMax );
		API_Leave();
	}
	return retv;
#endif

}
