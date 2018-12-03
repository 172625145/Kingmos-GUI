/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __GDISRV_H
#define __GDISRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#undef BitBlt
#define BitBlt WinGdi_BitBlt
API_TYPE BOOL WINAPI WinGdi_BitBlt(HDC hdcDest, int xDest, int yDest, int width, int height,HDC hdcSrc, int xSrc, int ySrc, DWORD dwRop );

#undef PatBlt
#define PatBlt WinGdi_PatBlt
API_TYPE BOOL WINAPI WinGdi_PatBlt(HDC hdc, int nX, int nY,  int nWidth, int nHeight, DWORD dwRop);

#undef MaskBlt
#define MaskBlt WinGdi_MaskBlt
API_TYPE BOOL WINAPI WinGdi_MaskBlt( HDC hdc,int xDest,int yDest,int width,int height,HDC hdcSrc,int xSrc,int ySrc,HBITMAP hbmMask,int xMask,int yMask,DWORD dwRop );

#undef DrawEdge
#define DrawEdge WinGdi_DrawEdge
API_TYPE BOOL WINAPI WinGdi_DrawEdge( HDC hdc, LPCRECT lpRect, UINT edgeType, UINT flags );

#undef DrawIcon
#define DrawIcon WinGdi_DrawIcon
API_TYPE BOOL WINAPI WinGdi_DrawIcon( HDC hdc, int x, int y, HICON hIcon );

#undef DrawIconEx
#define DrawIconEx WinGdi_DrawIconEx
API_TYPE BOOL WINAPI WinGdi_DrawIconEx( HDC hdc, int x, int y, HICON hIcon, int cxWidth, int cyWidth, UINT indexAnimal, HBRUSH hBrush, UINT uiFlags );

#undef DrawText
#define DrawText WinGdi_DrawText
API_TYPE int WINAPI WinGdi_DrawText( HDC hdc, LPCTSTR lpstr, int nCount, LPRECT lpRect, UINT uFormat );

#undef DrawFocusRect
#define DrawFocusRect WinGdi_DrawFocusRect
API_TYPE BOOL WINAPI WinGdi_DrawFocusRect( HDC hdc, LPCRECT lprc );

#undef DPtoLP
#define DPtoLP WinGdi_DPtoLP
API_TYPE BOOL WINAPI WinGdi_DPtoLP( HDC hdc, LPPOINT lpPoints, int nCount );

#undef Ellipse
#define Ellipse WinGdi_Ellipse
API_TYPE BOOL WINAPI WinGdi_Ellipse( HDC hdc, int left, int top, int right, int bottom );

#undef ExtTextOut
#define ExtTextOut WinGdi_ExtTextOut
API_TYPE BOOL WINAPI WinGdi_ExtTextOut( HDC hdc, int x, int y,UINT option, LPCRECT lpRect,LPCSTR lpstr,UINT count,int * lpNextPos );

#undef ExcludeClipRect
#define ExcludeClipRect WinGdi_ExcludeClipRect
API_TYPE int WINAPI WinGdi_ExcludeClipRect( HDC hdc, int left, int top, int right, int bottom );

#undef FillRect
#define FillRect WinGdi_FillRect
API_TYPE BOOL WINAPI WinGdi_FillRect( HDC hdc, LPCRECT lpRect, HBRUSH hBrush );

#undef FillRgn
#define FillRgn WinGdi_FillRgn
API_TYPE BOOL WINAPI WinGdi_FillRgn( HDC hdc, HRGN hrgn, HBRUSH hBrush );

#undef IntersectClipRect
#define IntersectClipRect WinGdi_IntersectClipRect
API_TYPE int WINAPI WinGdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom );

#undef GetBkColor
#define GetBkColor WinGdi_GetBkColor
API_TYPE COLORREF WINAPI WinGdi_GetBkColor( HDC hdc );

#undef GetBkMode
#define GetBkMode WinGdi_GetBkMode
API_TYPE int WINAPI WinGdi_GetBkMode( HDC hdc );

#undef GetCurrentPosition
#define GetCurrentPosition WinGdi_GetCurrentPosition
API_TYPE BOOL WINAPI WinGdi_GetCurrentPosition( HDC hdc, LPPOINT lpPoint );

#undef GetClipBox
#define GetClipBox WinGdi_GetClipBox
API_TYPE int WINAPI WinGdi_GetClipBox( HDC hdc, LPRECT lpRect );

#undef GetClipRgn
#define GetClipRgn WinGdi_GetClipRgn
API_TYPE int WINAPI WinGdi_GetClipRgn( HDC hdc, HRGN hrgn );

#undef GetCurrentObject
#define GetCurrentObject WinGdi_GetCurrentObject
API_TYPE HGDIOBJ WINAPI WinGdi_GetCurrentObject( HDC hdc, UINT uObjectType );

#undef GetDIBits
#define GetDIBits WinGdi_GetDIBits

API_TYPE int WINAPI WinGdi_GetDIBits(
						 HDC hdc,
						 HBITMAP hbmp,
						 UINT uStartScan,
						 UINT cScanLines,
						 LPVOID lpvBits,   
						 LPBITMAPINFO lpbi,
						 UINT uiUsage );

#undef CreateIconIndirect
#define CreateIconIndirect WinGdi_CreateIconIndirect
API_TYPE HICON WINAPI WinGdi_CreateIconIndirect( LPICONINFO lpIconInfo );

#undef DestroyIcon
#define DestroyIcon WinGdi_DestroyIcon
API_TYPE BOOL WINAPI WinGdi_DestroyIcon( HICON hIcon );

#undef DestroyCursor
#define DestroyCursor WinGdi_DestroyCursor
API_TYPE BOOL WINAPI WinGdi_DestroyCursor( HCURSOR hCursor );

#undef GetIconInfo
#define GetIconInfo WinGdi_GetIconInfo
API_TYPE BOOL WINAPI WinGdi_GetIconInfo( HICON hIcon, LPICONINFO lpInfo );

#undef GetObjectType
#define GetObjectType WinGdi_GetObjectType
API_TYPE DWORD WINAPI WinGdi_GetObjectType( HGDIOBJ h );

#undef GetObjectInfo
#define GetObjectInfo WinGdi_GetObjectInfo
API_TYPE int WINAPI WinGdi_GetObjectInfo( HGDIOBJ h, int bufferSize, void * lpBuf );

#undef GetPixel
#define GetPixel WinGdi_GetPixel
API_TYPE COLORREF WINAPI WinGdi_GetPixel( HDC hdc, int x, int y );

#undef GetROP2
#define GetROP2 WinGdi_GetROP2
API_TYPE int WINAPI WinGdi_GetROP2( HDC hdc );

#undef GetTextColor
#define GetTextColor WinGdi_GetTextColor
API_TYPE COLORREF WINAPI WinGdi_GetTextColor( HDC hdc );

#undef GetTextAlign
#define GetTextAlign WinGdi_GetTextAlign
API_TYPE UINT WINAPI WinGdi_GetTextAlign( HDC hdc );

#undef GetTextExtentPoint32
#define GetTextExtentPoint WinGdi_GetTextExtentPoint32
API_TYPE BOOL WINAPI WinGdi_GetTextExtentPoint32( HDC hdc, LPCSTR lpcstr, int nCount, LPSIZE lpSize );

#undef GetTextExtentExPoint
#define GetTextExtentExPoint WinGdi_GetTextExtentExPoint
API_TYPE BOOL WINAPI WinGdi_GetTextExtentExPoint( HDC hdc,LPCTSTR lpszStr,int nString,int nMaxExtent,LPINT lpFit,LPINT lpDx,LPSIZE lpSize );

#undef GetViewportOrg
#define GetViewportOrg WinGdi_GetViewportOrg
API_TYPE BOOL WINAPI WinGdi_GetViewportOrg( HDC hdc, LPPOINT lpPoint );

#undef GetWindowOrg
#define GetWindowOrg WinGdi_GetWindowOrg
API_TYPE BOOL WINAPI WinGdi_GetWindowOrg( HDC hdc, LPPOINT lpPoint );

#undef InvertRect
#define InvertRect WinGdi_InvertRect
API_TYPE BOOL WINAPI WinGdi_InvertRect( HDC hdc, LPCRECT lpcRect );

#undef InvertRgn
#define InvertRgn WinGdi_InvertRgn
API_TYPE BOOL WINAPI WinGdi_InvertRgn( HDC hdc, HRGN hrgn );

#undef LineTo
#define LineTo WinGdi_LineTo
API_TYPE BOOL WINAPI WinGdi_LineTo( HDC hdc, int x1, int y1 );

#undef Line
#define Line WinGdi_Line
API_TYPE BOOL WINAPI WinGdi_Line( HDC hdc, int x0, int y0, int x1, int y1 );

#undef LPtoDP
#define LPtoDP WinGdi_LPtoDP
API_TYPE BOOL WINAPI WinGdi_LPtoDP( HDC hdc, LPPOINT lpPoints, int nCount );

#undef MoveTo
#define MoveTo WinGdi_MoveTo
API_TYPE BOOL WINAPI WinGdi_MoveTo( HDC hdc, int x, int y, LPPOINT lppt );

#undef OffsetClipRgn
#define OffsetClipRgn WinGdi_OffsetClipRgn
API_TYPE int WINAPI WinGdi_OffsetClipRgn( HDC hdc, int xOff, int yOff );

#undef OffsetViewportOrg
#define OffsetViewportOrg WinGdi_OffsetViewportOrg
API_TYPE BOOL WINAPI WinGdi_OffsetViewportOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt );

#undef OffsetWindowOrg
#define OffsetWindowOrg WinGdi_OffsetWindowOrg
API_TYPE BOOL WINAPI WinGdi_OffsetWindowOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt );

#undef Polyline
#define Polyline WinGdi_Polyline
API_TYPE BOOL WINAPI WinGdi_Polyline( HDC hdc, const POINT* lppts, int nCount );

#undef PtVisible
#define PtVisible WinGdi_PtVisible
API_TYPE BOOL WINAPI WinGdi_PtVisible( HDC hdc, int x, int y );

#undef Rectangle
#define Rectangle WinGdi_Rectangle
API_TYPE BOOL WINAPI WinGdi_Rectangle( HDC hdc, int left, int top, int right, int bottom );

#undef RoundRect
#define RoundRect WinGdi_RoundRect
API_TYPE BOOL WINAPI WinGdi_RoundRect( HDC hdc, int left, int top, int right, int bottom, int nWidth, int nHeight );

#undef RectVisible
#define RectVisible WinGdi_RectVisible
API_TYPE BOOL WINAPI WinGdi_RectVisible( HDC hdc, LPCRECT lpcRect );

#undef SelectObject
#define SelectObject WinGdi_SelectObject
API_TYPE HGDIOBJ WINAPI WinGdi_SelectObject(HDC hdc, HGDIOBJ hgdi);

#undef DeleteObject
#define DeleteObject WinGdi_DeleteObject
API_TYPE BOOL WINAPI WinGdi_DeleteObject( HGDIOBJ hgdi );

#undef DeleteDC
#define DeleteDC WinGdi_DeleteDC
API_TYPE BOOL WINAPI WinGdi_DeleteDC( HDC hdc );

#undef SelectClipRgn
#define SelectClipRgn WinGdi_SelectClipRgn
API_TYPE int WINAPI WinGdi_SelectClipRgn( HDC hdc, HRGN hrgn );

#undef ExtSelectClipRgn
#define ExtSelectClipRgn WinGdi_ExtSelectClipRgn
API_TYPE int WINAPI WinGdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode );

#undef SetBkColor
#define SetBkColor WinGdi_SetBkColor
API_TYPE COLORREF WINAPI WinGdi_SetBkColor( HDC hdc, COLORREF color );

#undef SetBkMode
#define SetBkMode WinGdi_SetBkMode
API_TYPE int WINAPI WinGdi_SetBkMode( HDC hdc, int iMode );

#undef SetBrushOrg
#define SetBrushOrg WinGdi_SetBrushOrg
API_TYPE BOOL WINAPI WinGdi_SetBrushOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );

#undef SetPixel
#define SetPixel WinGdi_SetPixel
API_TYPE COLORREF WINAPI WinGdi_SetPixel( HDC hdc, int x, int y, COLORREF color );

#undef SetROP2
#define SetROP2 WinGdi_SetROP2
API_TYPE int WINAPI WinGdi_SetROP2( HDC hdc, int rop );

#undef SetTextColor
#define SetTextColor WinGdi_SetTextColor
API_TYPE COLORREF WINAPI WinGdi_SetTextColor( HDC hdc,  COLORREF color );

#undef SetTextAlign
#define SetTextAlign WinGdi_SetTextAlign
API_TYPE UINT WINAPI WinGdi_SetTextAlign( HDC hdc, UINT uiAlignMode );

#undef SetViewportOrg
#define SetViewportOrg WinGdi_SetViewportOrg
API_TYPE BOOL WINAPI WinGdi_SetViewportOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );

#undef SetWindowOrg
#define SetWindowOrg WinGdi_SetWindowOrg
API_TYPE BOOL WINAPI WinGdi_SetWindowOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );

#undef TextOut
#define TextOut WinGdi_TextOut
API_TYPE BOOL WINAPI WinGdi_TextOut( HDC hdc, int x, int y, LPCTSTR lpString, int count );

#undef TextOutW
#define TextOutW WinGdi_TextOutW
API_TYPE BOOL WINAPI WinGdi_TextOutW( HDC hdc, int x, int y, LPCTSTR lpString, int count );

#undef CreatePenIndirect
#define CreatePenIndirect WinGdi_CreatePenIndirect
API_TYPE HPEN WINAPI WinGdi_CreatePenIndirect(const LOGPEN * lpPenData);

#undef CreatePen
#define CreatePen WinGdi_CreatePen
API_TYPE HPEN WINAPI WinGdi_CreatePen( int style, int width, COLORREF color );

#undef CreateBrushIndirect
#define CreateBrushIndirect WinGdi_CreateBrushIndirect
API_TYPE HBRUSH WINAPI WinGdi_CreateBrushIndirect(const LOGBRUSH *lpBrushData);

#undef CreateHatchBrush
#define CreateHatchBrush WinGdi_CreateHatchBrush
API_TYPE HBRUSH WINAPI WinGdi_CreateHatchBrush(int style, COLORREF color);

#undef CreateSolidBrush
#define CreateSolidBrush WinGdi_CreateSolidBrush
API_TYPE HBRUSH WINAPI WinGdi_CreateSolidBrush( COLORREF color );

#undef CreateBitmap
#define CreateBitmap WinGdi_CreateBitmap
API_TYPE HBITMAP WINAPI WinGdi_CreateBitmap( int nWidth, int nHeight, UINT cPlanes, UINT cBitsPerPel, const VOID *lpvBits);

#undef CreateCompatibleBitmap
#define CreateCompatibleBitmap WinGdi_CreateCompatibleBitmap
API_TYPE HBITMAP WINAPI WinGdi_CreateCompatibleBitmap(HDC hdc, int iWidth, int iHeight);

#undef CreateCompatibleDC
#define CreateCompatibleDC WinGdi_CreateCompatibleDC
API_TYPE HDC WINAPI WinGdi_CreateCompatibleDC(HDC hdc);

#undef CreateDIBSection
#define CreateDIBSection WinGdi_CreateDIBSection
API_TYPE HBITMAP WINAPI WinGdi_CreateDIBSection( HDC hdc, CONST BITMAPINFO *lpbmi, UINT iUsage, VOID ** lppvBits, HANDLE hSection,DWORD dwOffset);

#undef SetDIBits
#define SetDIBits WinGdi_SetDIBits
API_TYPE int WINAPI WinGdi_SetDIBits( HDC hdc,HBITMAP hbmp,UINT uStartScan,UINT cScanLines,CONST VOID *lpvBits,CONST BITMAPINFO *lpbmi,UINT fuColorUse);

#undef CreateDIBitmap
#define CreateDIBitmap WinGdi_CreateDIBitmap
API_TYPE HBITMAP WINAPI WinGdi_CreateDIBitmap(HDC hdc,CONST BITMAPINFOHEADER *lpbmih,DWORD fdwInit,CONST VOID *lpbInit,CONST BITMAPINFO *lpbmi,UINT fuUsage);

#undef GetStockObject
#define GetStockObject WinGdi_GetStockObject
API_TYPE HGDIOBJ WINAPI WinGdi_GetStockObject( int fObject );

#undef GetTextMetrics
#define GetTextMetrics WinGdi_GetTextMetrics
API_TYPE BOOL WINAPI WinGdi_GetTextMetrics( HDC hdc, LPTEXTMETRIC lptm );

#undef GetCharWidth
#define GetCharWidth WinGdi_GetCharWidth
API_TYPE BOOL WINAPI WinGdi_GetCharWidth( HDC hdc, WORD iFirst, WORD iLast, LPINT lpiBuf );

#undef LoadImage
#define LoadImage WinGdi_LoadImage
API_TYPE HANDLE WINAPI WinGdi_LoadImage( HINSTANCE hInst,LPCTSTR lpszName,UINT uType,int cxDesired,int cyDesired, UINT fuLoad );

#undef LoadIcon
#define LoadIcon WinGdi_LoadIcon
API_TYPE HICON WINAPI WinGdi_LoadIcon( HINSTANCE hInst, LPCTSTR lpIconName );

#undef LoadCursor
#define LoadCursor WinGdi_LoadCursor
API_TYPE HCURSOR WINAPI WinGdi_LoadCursor( HINSTANCE hInst, LPCTSTR lpIconName );

///////////////////////////////////////////////////////////

#undef CreateRectRgn
#define CreateRectRgn WinRgn_CreateRect
API_TYPE HRGN WINAPI WinRgn_CreateRect( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect );

#undef CreateRectRgnIndirect
#define CreateRectRgnIndirect WinRgn_CreateRectIndirect
API_TYPE HRGN WINAPI WinRgn_CreateRectIndirect(LPCRECT lpRect);

#undef CombineRgn
#define CombineRgn WinRgn_Combine
API_TYPE int WINAPI WinRgn_Combine( HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, int fnCombineMode );

#undef EqualRgn
#define EqualRgn WinRgn_Equal
API_TYPE BOOL WINAPI WinRgn_Equal(HRGN hSrcRgn1, HRGN hSrcRgn2);

#undef OffsetRgn
#define OffsetRgn WinRgn_Offset
API_TYPE int  WINAPI WinRgn_Offset(HRGN hrgn, int xOffset, int yOffset);

#undef GetRgnBox
#define GetRgnBox WinRgn_GetBox
API_TYPE int  WINAPI WinRgn_GetBox(HRGN hrgn, LPRECT lprc);

#undef RectInRegion
#define RectInRegion WinRgn_RectInRegion
API_TYPE BOOL WINAPI WinRgn_RectInRegion(HRGN hrgn, const LPRECT lprc);

#undef PtInRegion
#define PtInRegion WinRgn_PtInRegion
API_TYPE BOOL WINAPI WinRgn_PtInRegion(HRGN hrgn, int x, int y);

#undef SetRectRgn
#define SetRectRgn WinRgn_SetRect
API_TYPE BOOL WINAPI WinRgn_SetRect(HRGN hrgn, int left, int top, int right, int bottom);

#undef Arc
#define Arc WinRgn_Arc
API_TYPE BOOL WINAPI WinGdi_Arc(
				HDC hdc,
				int left,
				int top,
				int right,
				int bottom,
				int nXStartArc,
				int nYStartArc,
				int nXEndArc,
				int nYEndArc
				);

#undef SetDIBitsToDevice
#define SetDIBitsToDevice WinGdi_SetDIBitsToDevice
API_TYPE int WINAPI WinGdi_SetDIBitsToDevice(
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
						 );
#undef GetMapMode
#define GetMapMode WinGdi_GetMapMode
API_TYPE int WINAPI WinGdi_GetMapMode( HDC hdc );

#undef SetMapMode
#define SetMapMode WinGdi_SetMapMode
API_TYPE int WINAPI WinGdi_SetMapMode( HDC hdc, int iNewMode );

#undef GetTextExtentPoint32W
#define GetTextExtentPoint32W WinGdi_GetTextExtentPoint32W
API_TYPE BOOL WINAPI WinGdi_GetTextExtentPoint32W( HDC hdc, LPCSTR lpcstr, int nCount, LPSIZE lpSize );

#undef CreateFontIndirect
#define CreateFontIndirect WinFont_CreateIndirect
API_TYPE HFONT WINAPI WinFont_CreateIndirect( CONST LOGFONT *lplf );


API_TYPE BOOL WINAPI WinGdi_TransparentBlt(HDC hdcDest, 
								  int xDest, 
								  int yDest, 
								  int width, 
								  int height,
								  HDC hdcSrc,
								  int xSrc, 
								  int ySrc, 
								  int widthSrc,
								  int heightSrc,
								  DWORD clTransparent );

API_TYPE BOOL WINAPI WinGdi_StretchBlt(
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

API_TYPE HWND WINAPI WinGdi_WindowFromDC( HDC hdc );

API_TYPE UINT WINAPI WinGdi_SetDIBColorTable( HDC hdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors );
API_TYPE int WINAPI WinGdi_GetDeviceCaps( HDC hdc, int nIndex );

API_TYPE BOOL WINAPI WinGdi_AlphaBlendEx(
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
									   LPBLENDFUNCTION_EX lpBlendFunction
									   );
API_TYPE HFONT WINAPI WinGdi_CreateFontIndirect(  const LOGFONT *lplf );

API_TYPE int WINAPI WinGdi_LoadString(
					 HINSTANCE hInstance, 
					 UINT uIDName, 
					 LPTSTR lpBuffer, 
					 int nBufferMax 
					 );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

///////////////////////////////////////////////////

#endif   //__GDISRV_H

