/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EGDI_H
#define __EGDI_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define MAX_COLORS 16

// define Stock Brush Style
#define BS_SOLID            0
#define BS_NULL             1
#define BS_HOLLOW           BS_NULL
#define BS_HATCHED          2
#define BS_PATTERN          3

// Hatch Styles
#define STOCK_BRUSH_NUM 6
#define HS_HORIZONTAL       0       // -----
#define HS_VERTICAL         1       // ||||| 
#define HS_FDIAGONAL        2       // \\\\\ 
#define HS_BDIAGONAL        3       // /////
#define HS_CROSS            4       // +++++
#define HS_DIAGCROSS        5       // xxxxx

// define Stock Icon Style
#define STOCK_ICON_NUM 1
enum STOCK_ICN
{
    ICN_SYS
};

enum STOCK_CUR
{
    CURSOR_NORMAL = 1,
    CURSOR_SIZENS = 2,
    CURSOR_SIZEWE = 3,
    CURSOR_SIZENWSE = 4,
    CURSOR_SIZENESW = 5,
    CURSOR_ICON = 6,
    CURSOR_WAITING = 7,
    CURSOR_EDIT = 8,
    CURSOR_USER
};

// define Stock Icon Style
enum STOCK_BMP
{
    BMP_ZOOM,
    BMP_MAXBOX,
    BMP_MINBOX,
    BMP_LEFTARROW,
    BMP_RIGHTARROW,
    BMP_UPARROW,
    BMP_DOWNARROW,
    BMP_CHKNOR,
    BMP_CHKSEL,
    BMP_RADIONOR,
    BMP_RADIOSEL,
    STOCK_BMP_NUM
};

// defien Stock Pen Style
#define STOCK_PEN_NUM 6
enum STOCK_PS
{
    PS_SOLID        = 0xFF,
    PS_DOT          = 0xAA,
    PS_DASH         = 0xFE,
    PS_DASHDOT      = 0xFA,
    PS_DASHDOTDOT   = 0xEA,
    PS_NULL         = 0
};

//define Stock Color
#define CLR_INVALID     (-1)
#define GDI_ERROR       (-1)
#define STOCK_COLOR_NUM 16

#define RGB(r,g,b)      ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))

#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))

#define CL_BLACK        RGB( 0x00, 0x00, 0x00 )  //0
#define CL_DARKRED      RGB( 0x80, 0x00, 0x00 )  //1
#define CL_DARKGREEN    RGB( 0x00, 0x80, 0x00 )  //2
#define CL_DARKYELLOW   RGB( 0x80, 0x80, 0x00 )  //3
#define CL_DARKBLUE     RGB( 0x00, 0x00, 0x80 )  //4
#define CL_DARKVIOLET   RGB( 0x80, 0x00, 0x80 )  //5
#define CL_DARKCYAN     RGB( 0x00, 0x80, 0x80 )  //6
#define CL_DARKGRAY     RGB( 0x80, 0x80, 0x80 )  //7
#define CL_LIGHTGRAY    RGB( 0xf0, 0xf0, 0xf0 )  //8
#define CL_RED          RGB( 0xff, 0x00, 0x00 )  //9
#define CL_GREEN        RGB( 0x00, 0xff, 0x00 )  //10
#define CL_YELLOW       RGB( 0xff, 0xff, 0x00 )  //11
#define CL_BLUE         RGB( 0x00, 0x00, 0xff )  //12
#define CL_VIOLET       RGB( 0xff, 0x00, 0xff )  //13
#define CL_CYAN         RGB( 0x00, 0xff, 0xff )  //14
#define CL_WHITE        RGB( 0xff, 0xff, 0xff )  //15

#define CL_GRAY         RGB( 0xe0, 0xe0, 0xe0 )  //

#define R2_BLACK            1   
#define R2_NOTMERGEPEN      2   
#define R2_MASKNOTPEN       3   
#define R2_NOTCOPYPEN       4   
#define R2_MASKPENNOT       5   
#define R2_NOT              6   
#define R2_XORPEN           7   
#define R2_NOTMASKPEN       8   
#define R2_MASKPEN          9   
#define R2_NOTXORPEN        10  
#define R2_NOP              11  
#define R2_MERGENOTPEN      12  
#define R2_COPYPEN          13  
#define R2_MERGEPENNOT      14  
#define R2_MERGEPEN         15  
#define R2_WHITE            16  
#define R2_LAST             16



//define EXTTEXTOUT OPTION
enum
{
    ETO_CLIPPED   = 0x0001,
    ETO_OPAQUE = 0x0002
};

// define Text Align
enum
{
    TA_TOP        = 0x0000,
    TA_LEFT       = 0x0000,
    TA_BOTTOM     = 0x0001,
    TA_RIGHT      = 0x0002,
    TA_CENTER     = 0x0004
};

#define TA_NOUPDATECP                0
#define TA_UPDATECP                  0x0008

#define TA_BASELINE                  0x0010

#define DFLT_TAB 8

enum BACKMODE
{
    OPAQUE,
    TRANSPARENT
};

#define AD_COUNTERCLOCKWISE 1
#define AD_CLOCKWISE        2

// define BITMAP
/*
typedef struct _BITMAP
{
    WORD        bmType;
    WORD        bmWidth;
    WORD        bmHeight;
    WORD        bmWidthBytes;
    WORD        bmPlanes;
    WORD        bmBitsPixel;
    LPBYTE      bmBits;
}BITMAP, *PBITMAP, FAR *LPBITMAP;
*/
typedef struct _BITMAP 
{
  LONG   bmType; 
  LONG   bmWidth; 
  LONG   bmHeight; 
  LONG   bmWidthBytes; 
  WORD   bmPlanes; 
  WORD   bmBitsPixel; 
  LPVOID bmBits; 
} BITMAP, *PBITMAP, FAR *LPBITMAP; 


typedef struct _RGBQUAD 
{
	BYTE    rgbBlue;
	BYTE    rgbGreen;
	BYTE    rgbRed;
	BYTE    rgbReserved;
} RGBQUAD, FAR* LPRGBQUAD;

typedef struct _PALETTEENTRY 
{ 
    BYTE peRed; 
    BYTE peGreen; 
    BYTE peBlue; 
    BYTE peFlags; 
} PALETTEENTRY, FAR * LPPALETTEENTRY;

#define DIB_RGB_COLORS      0
#define DIB_PAL_COLORS      1

#define BI_RGB        0L
#define BI_BITFIELDS  3L

#ifdef __MSWC__
#pragma pack(2)
#endif

typedef _ARMCC_PACKED_ struct _BITMAPFILEHEADER 
{ 
  WORD    bfType; 
  DWORD   bfSize; 
  WORD    bfReserved1; 
  WORD    bfReserved2; 
  DWORD   bfOffBits; 
} _PACKED_ BITMAPFILEHEADER, * PBITMAPFILEHEADER, FAR * LPBITMAPFILEHEADER;

#ifdef __MSWC__
#pragma pack()
#endif

typedef struct _BITMAPINFOHEADER
{
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, FAR *LPBITMAPINFOHEADER;

typedef struct _BITMAPINFO 
{
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO,  *PBITMAPINFO, FAR *LPBITMAPINFO;

typedef struct _DIBSECTION
{
    BITMAP              dsBm;
    BITMAPINFOHEADER    dsBmih;
    DWORD               dsBitfields[3];
    HANDLE              dshSection;
    DWORD               dsOffset;
} DIBSECTION, FAR *LPDIBSECTION, *PDIBSECTION;


// ternary raster operations
#define SRCCOPY             (DWORD)0x00CC0020l
#define SRCPAINT            (DWORD)0x00EE0086l
#define SRCAND              (DWORD)0x008800C6l
#define SRCINVERT           (DWORD)0x00660046l
#define NOTSRCCOPY          (DWORD)0x00330008l
#define PATCOPY             (DWORD)0x00F00021l
#define PATINVERT           (DWORD)0x005A0049l
#define DSTINVERT           (DWORD)0x00550009l
#define BLACKNESS           (DWORD)0x00000042l
#define WHITENESS           (DWORD)0x00FF0062l

#define MERGECOPY           (DWORD)0x00C000CAl
#define MERGEPAINT          (DWORD)0x00BB0226l

#define MAKEROP4( fore, back ) (DWORD)((((back) << 8) & 0xFF000000) | (fore))

#define BitBlt Gdi_BitBlt
BOOL WINAPI Gdi_BitBlt(HDC hdcDest, int nDestX, int nDestY, int nWidth, int nHeight,
				HDC hdcSrc, int nSrcX, int nSrcY, DWORD dwRop );

#define PatBlt Gdi_PatBlt
BOOL WINAPI Gdi_PatBlt(HDC hdc, int nX, int nY, int nWidth, int nHeight, DWORD dwRop );

#define MaskBlt Gdi_MaskBlt
BOOL WINAPI Gdi_MaskBlt(HDC hdcdest,
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
                 DWORD dwRop );

// 3D border styles
#define BDR_RAISEDOUTER 0x0001
#define BDR_SUNKENOUTER 0x0002
#define BDR_RAISEDINNER 0x0004
#define BDR_SUNKENINNER 0x0008

#define BDR_OUTER       0x0003
#define BDR_INNER       0x000c
#define BDR_RAISED      0x0005
#define BDR_SUNKEN      0x000a

#define EDGE_RAISED     (BDR_RAISEDOUTER | BDR_RAISEDINNER)
#define EDGE_SUNKEN     (BDR_SUNKENOUTER | BDR_SUNKENINNER)
#define EDGE_ETCHED     (BDR_SUNKENOUTER | BDR_RAISEDINNER)
#define EDGE_BUMP       (BDR_RAISEDOUTER | BDR_SUNKENINNER)

// Border flags
#define BF_LEFT         0x0001
#define BF_TOP          0x0002
#define BF_RIGHT        0x0004
#define BF_BOTTOM       0x0008

#define BF_TOPLEFT      (BF_TOP | BF_LEFT)
#define BF_TOPRIGHT     (BF_TOP | BF_RIGHT)
#define BF_BOTTOMLEFT   (BF_BOTTOM | BF_LEFT)
#define BF_BOTTOMRIGHT  (BF_BOTTOM | BF_RIGHT)
#define BF_RECT         (BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM)

#define BF_DIAGONAL     0x0010

#define BF_DIAGONAL_ENDTOPRIGHT     (BF_DIAGONAL | BF_TOP | BF_RIGHT)
#define BF_DIAGONAL_ENDTOPLEFT      (BF_DIAGONAL | BF_TOP | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMLEFT   (BF_DIAGONAL | BF_BOTTOM | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMRIGHT  (BF_DIAGONAL | BF_BOTTOM | BF_RIGHT)

#define BF_MIDDLE       0x0800
#define BF_FLAT         0x1000

#define DrawEdge Gdi_DrawEdge
BOOL WINAPI Gdi_DrawEdge( HDC hdc, LPCRECT lpcrc, UINT uType, UINT uFlags );

#define DrawIcon Gdi_DrawIcon
BOOL WINAPI Gdi_DrawIcon( HDC hdc, int nX, int nY, HICON );

#define DI_NORMAL       0x0003
#define DI_DEFAULTSIZE  0x0008
#define DrawIconEx Gdi_DrawIconEx
BOOL WINAPI Gdi_DrawIconEx( HDC hdc, 
						    int x, 
							int y, 
							HICON hIcon, 
							int cxWidth, 
							int cyWidth, 
							UINT uiIndexAnimal,
							HBRUSH hBrush,
							UINT uiFlags );

// DrawText Format Flags
#define DT_TOP              0x00000000
#define DT_LEFT             0x00000000
#define DT_CENTER           0x00000001
#define DT_RIGHT            0x00000002
#define DT_VCENTER          0x00000004
#define DT_BOTTOM           0x00000008
#define DT_WORDBREAK        0x00000010
#define DT_SINGLELINE       0x00000020
#define DT_TABSTOP          0x00000040
#define DT_NOCLIP           0x00000080
#define DT_NOPREFIX         0x00000100
#define DT_CALCRECT         0x00000200
#define DT_END_ELLIPSIS     0x00008000

#define DrawText Gdi_DrawText
int WINAPI Gdi_DrawText( HDC, LPCTSTR lpcstr, int nCount, LPRECT lpcrc, UINT uFormat );

#define DrawFocusRect Gdi_DrawFocusRect
BOOL WINAPI Gdi_DrawFocusRect( HDC hdc, LPCRECT lprc );

#define DPtoLP Gdi_DPtoLP
BOOL WINAPI Gdi_DPtoLP( HDC hdc, LPPOINT lpPoints, int nCount );

#define Ellipse Gdi_Ellipse
BOOL WINAPI Gdi_Ellipse( HDC hdc, int left, int top, int right, int bottom );

#define ExtTextOut Gdi_ExtTextOut
BOOL WINAPI Gdi_ExtTextOut( HDC hdc, int x, int y, UINT option, LPCRECT lpRect, LPCTSTR, UINT count, int * lpNextPos );

#define ExcludeClipRect Gdi_ExcludeClipRect
int WINAPI Gdi_ExcludeClipRect( HDC, int left, int top, int right, int bottom );

#define FillRect Gdi_FillRect
BOOL WINAPI Gdi_FillRect( HDC, LPCRECT, HBRUSH hBrush );

#define FillRgn Gdi_FillRgn
BOOL WINAPI Gdi_FillRgn( HDC, HRGN, HBRUSH hBrush );

#define IntersectClipRect Gdi_IntersectClipRect
int WINAPI Gdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom );

#define GetBkColor Gdi_GetBkColor
COLORREF WINAPI Gdi_GetBkColor( HDC hdc );

#define GetBkMode Gdi_GetBkMode
int WINAPI Gdi_GetBkMode( HDC hdc );

#define GetCurrentPositionEx Gdi_GetCurrentPosition
BOOL WINAPI Gdi_GetCurrentPosition( HDC hdc, LPPOINT lpPoint );

#define GetClipBox Gdi_GetClipBox
int WINAPI Gdi_GetClipBox( HDC hdc, LPRECT lprc );

#define GetClipRgn Gdi_GetClipRgn
int WINAPI Gdi_GetClipRgn( HDC hdc, HRGN hrgn );

#define GetCurrentObject Gdi_GetCurrentObject
HGDIOBJ WINAPI Gdi_GetCurrentObject( HDC hdc, UINT uObjectType );

typedef struct _ICONINFO 
{
    BOOL    fIcon;
    WORD    xHotspot;
    WORD    yHotspot;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
} ICONINFO, FAR * LPICONINFO;

#define GetDIBits Gdi_GetDIBits
int WINAPI Gdi_GetDIBits(
						 HDC hdc,           
						 HBITMAP hbmp,      
						 UINT uStartScan,   
						 UINT cScanLines,   
						 LPVOID lpvBits,    
						 LPBITMAPINFO lpbi, 
						 UINT uiUsage );


#define MM_TEXT             1

#define GetMapMode Gdi_GetMapMode
int WINAPI Gdi_GetMapMode( HDC hdc );

#define CreateIconIndirect Gdi_CreateIconIndirect
HICON WINAPI Gdi_CreateIconIndirect( LPICONINFO piconinfo );

#define DestroyIcon Gdi_DestroyIcon
BOOL WINAPI Gdi_DestroyIcon( HICON hIcon );

#define DestroyCursor Gdi_DestroyCursor
BOOL WINAPI Gdi_DestroyCursor( HCURSOR hCursor );
 
#define GetIconInfo Gdi_GetIconInfo
BOOL WINAPI Gdi_GetIconInfo( HICON hIcon, LPICONINFO lpiconInfo);

#define GetObjectType Gdi_GetObjectType
DWORD WINAPI Gdi_GetObjectType( HGDIOBJ handle );

#define GetObject Gdi_GetObjectInfo
int WINAPI Gdi_GetObjectInfo( HGDIOBJ handle, int nSize, LPVOID lpvBuf );

#define GetPixel Gdi_GetPixel
COLORREF WINAPI Gdi_GetPixel( HDC, int x, int y );

#define GetROP2 Gdi_GetROP2
int WINAPI Gdi_GetROP2( HDC );

#define GetTextColor Gdi_GetTextColor
COLORREF WINAPI Gdi_GetTextColor( HDC );

#define GetTextAlign Gdi_GetTextAlign
UINT WINAPI Gdi_GetTextAlign( HDC );

#define GetTextExtentPoint32 Gdi_GetTextExtentPoint32
BOOL WINAPI Gdi_GetTextExtentPoint32( HDC hdc, LPCTSTR lpcstr, int nLength, LPSIZE lpSize );

#define GetTextExtentPoint32W Gdi_GetTextExtentPoint32W
BOOL WINAPI Gdi_GetTextExtentPoint32W( HDC hdc, LPCTSTR lpcstr, int nLength, LPSIZE lpSize );

#define GetTextExtentExPoint Gdi_GetTextExtentExPoint
BOOL WINAPI Gdi_GetTextExtentExPoint( HDC hdc, LPCTSTR lpcstr, int nLength, int nMaxWidth, LPINT lpFitCharNum, LPINT lpGroupWidth, LPSIZE lpSize );

#define GetViewportOrgEx Gdi_GetViewportOrg
BOOL WINAPI Gdi_GetViewportOrg( HDC, LPPOINT lpPoint );

#define GetWindowOrgEx Gdi_GetWindowOrg
BOOL WINAPI Gdi_GetWindowOrg( HDC hdc, LPPOINT lpPoint );

#define InvertRect Gdi_InvertRect
BOOL WINAPI Gdi_InvertRect( HDC, LPCRECT );

#define InvertRgn Gdi_InvertRgn
BOOL WINAPI Gdi_InvertRgn( HDC, HRGN );

#define LineTo Gdi_LineTo
BOOL WINAPI Gdi_LineTo( HDC, int nXEnd, int nYEnd );

#define Line Gdi_Line
BOOL WINAPI Gdi_Line( HDC hdc, int x0, int y0, int x1, int y1 );

#define LPtoDP Gdi_LPtoDP
BOOL WINAPI Gdi_LPtoDP( HDC hdc, LPPOINT lpPoints, int nCount );

#define MoveTo( hdc, x, y ) Gdi_MoveTo( (hdc), (x), (y), 0 )
#define MoveToEx Gdi_MoveTo
BOOL WINAPI Gdi_MoveTo( HDC, int x, int y, LPPOINT lppt );

#define OffsetClipRgn Gdi_OffsetClipRgn
int WINAPI Gdi_OffsetClipRgn( HDC hdc, int xOff, int yOff );

#define OffsetViewportOrgEx Gdi_OffsetViewportOrg
BOOL WINAPI Gdi_OffsetViewportOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt );

#define OffsetWindowOrgEx Gdi_OffsetWindowOrg
BOOL WINAPI Gdi_OffsetWindowOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt );

#define Polyline Gdi_Polyline
BOOL WINAPI Gdi_Polyline( HDC hdc, const POINT * lpcPoints, int nCount );

#define PtVisible Gdi_PtVisible
BOOL WINAPI Gdi_PtVisible( HDC, int x, int y );

#define Rectangle Gdi_Rectangle
BOOL WINAPI Gdi_Rectangle( HDC, int left, int top, int right, int bottom );

#define RoundRect Gdi_RoundRect
BOOL WINAPI Gdi_RoundRect( HDC hdc, int left, int top, int right, int bottom, int nWidth, int nHeight );

#define RectVisible Gdi_RectVisible
BOOL WINAPI Gdi_RectVisible( HDC hdc, LPCRECT lpcrc );

#define SelectObject Gdi_SelectObject
HGDIOBJ WINAPI Gdi_SelectObject(HDC hdc, HGDIOBJ handle);

#define DeleteObject Gdi_DeleteObject
BOOL WINAPI Gdi_DeleteObject(HGDIOBJ handle);

#define DeleteDC Gdi_DeleteDC
BOOL WINAPI Gdi_DeleteDC(HDC hdc);

#define SelectClipRgn Gdi_SelectClipRgn
int WINAPI Gdi_SelectClipRgn( HDC hdc, HRGN hrgn );

#define ExtSelectClipRgn Gdi_ExtSelectClipRgn
int WINAPI Gdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int iMode );

#define SetBkColor Gdi_SetBkColor
COLORREF WINAPI Gdi_SetBkColor( HDC hdc, COLORREF color );

#define SetBkMode Gdi_SetBkMode
int WINAPI Gdi_SetBkMode( HDC hdc, int mode );

#define SetBrushOrgEx Gdi_SetBrushOrg
BOOL WINAPI Gdi_SetBrushOrg( HDC, int xOrg, int yOrg, LPPOINT lppt );

#define SetMapMode Gdi_SetMapMode
int WINAPI Gdi_SetMapMode( HDC, int mode );

#define SetPixel Gdi_SetPixel
COLORREF WINAPI Gdi_SetPixel( HDC hdc, int x, int y, COLORREF color );

#define SetROP2 Gdi_SetROP2
int WINAPI Gdi_SetROP2( HDC hdc , int fMode );

#define SetTextColor Gdi_SetTextColor
COLORREF WINAPI Gdi_SetTextColor( HDC hdc, COLORREF color );

#define SetTextAlign Gdi_SetTextAlign
UINT WINAPI Gdi_SetTextAlign( HDC hdc, UINT uiAlignMode );

#define SetViewportOrgEx Gdi_SetViewportOrg
BOOL WINAPI Gdi_SetViewportOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt );

#define SetWindowOrgEx Gdi_SetWindowOrg
BOOL WINAPI Gdi_SetWindowOrg( HDC, int xOrg, int yOrg, LPPOINT lppt );

#define TextOut Gdi_TextOut
BOOL WINAPI Gdi_TextOut( HDC, int x, int y, LPCTSTR, int count );

typedef struct _LOGPEN
{
    WORD     lognStyle;
    POINT    lognWidth;
    COLORREF lognColor;
}LOGPEN, FAR * LPLOGPEN;

#define CreatePenIndirect Gdi_CreatePenIndirect
HPEN WINAPI Gdi_CreatePenIndirect(const LOGPEN *);

#define CreatePen Gdi_CreatePen
HPEN WINAPI Gdi_CreatePen(int, int, COLORREF);

typedef struct _LOGBRUSH
{
    WORD     lbStyle;
    COLORREF lbColor;
    LONG     lbHatch;
}LOGBRUSH, FAR * LPLOGBRUSH;

#define CreateBrushIndirect Gdi_CreateBrushIndirect
HBRUSH WINAPI Gdi_CreateBrushIndirect(const LOGBRUSH*);

#define CreateHatchBrush Gdi_CreateHatchBrush
HBRUSH WINAPI Gdi_CreateHatchBrush(int style, COLORREF color);

#define CreateSolidBrush Gdi_CreateSolidBrush
HBRUSH WINAPI Gdi_CreateSolidBrush( COLORREF color);

#define CreateBitmap Gdi_CreateBitmap
HBITMAP WINAPI Gdi_CreateBitmap(int nWidth, int nHeight,  UINT nPlanes, UINT nBitsPerPel, const VOID * lpcBits);

#define CreateCompatibleBitmap Gdi_CreateCompatibleBitmap
HBITMAP WINAPI Gdi_CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight);

#define CreateBitmapIndirect Gdi_CreateBitmapIndirect
HBITMAP WINAPI Gdi_CreateBitmapIndirect(const BITMAP *);

#define CreateCompatibleDC Gdi_CreateCompatibleDC
HDC WINAPI Gdi_CreateCompatibleDC(HDC);

#define CreateDIBSection  Gdi_CreateDIBSection
HBITMAP WINAPI Gdi_CreateDIBSection ( 
						 HDC hdc, 
						 CONST BITMAPINFO *lpbmi,
						 UINT iUsage,         //must = DIB_PAL_COLORS
						 VOID ** lppvBits,
						 HANDLE hSection,  // must = NULL
						 DWORD dwOffset   // = 0
						 );

#define SetDIBits Gdi_SetDIBits
int WINAPI Gdi_SetDIBits(
						 HDC hdc,                  
						 HBITMAP hbmp,             
						 UINT uStartScan,          
						 UINT cScanLines,          
						 CONST VOID *lpvBits,      
						 CONST BITMAPINFO *lpbmi,  
						 UINT fuColorUse           
						 );


#define CBM_INIT        0x04L   
#define CreateDIBitmap  Gdi_CreateDIBitmap
HBITMAP WINAPI Gdi_CreateDIBitmap(
								  HDC hdc,                        
								  CONST BITMAPINFOHEADER *lpbmih, 
								  DWORD fdwInit,                  
								  CONST VOID *lpbInit,            
								  CONST BITMAPINFO *lpbmi,        
								  UINT fuUsage                    
								  );

#define ANSI_CHARSET            0
#define DEFAULT_CHARSET         1
#define SYMBOL_CHARSET          2
#define SHIFTJIS_CHARSET        128
#define HANGEUL_CHARSET         129
#define HANGUL_CHARSET          129
#define GB2312_CHARSET          134
#define CHINESEBIG5_CHARSET     136
#define OEM_CHARSET             255

#define LF_FACESIZE         32
typedef struct _LOGFONT 
{ // lf
	LONG lfHeight; 
	LONG lfWidth; 
	LONG lfEscapement; 
	LONG lfOrientation; 
	LONG lfWeight; 
	BYTE lfItalic; 
	BYTE lfUnderline; 
	BYTE lfStrikeOut; 
	BYTE lfCharSet; 
	BYTE lfOutPrecision; 
	BYTE lfClipPrecision; 
	BYTE lfQuality; 
	BYTE lfPitchAndFamily; 
	TCHAR lfFaceName[LF_FACESIZE]; 
} LOGFONT, *PLOGFONT, FAR* LPLOGFONT; 

// Stock Logical Objects
#define WHITE_BRUSH         0
#define LTGRAY_BRUSH        1
#define GRAY_BRUSH          2
#define DKGRAY_BRUSH        3
#define BLACK_BRUSH         4
#define NULL_BRUSH          5
#define HOLLOW_BRUSH        NULL_BRUSH

#define WHITE_PEN           6
#define BLACK_PEN           7
#define NULL_PEN            8

#define ANSI_FIXED_FONT     9
#define ANSI_VAR_FONT       10


#define SYSTEM_FONT              11		//系统默认字体
#define SYSTEM_FIXED_FONT        11

#define SYSTEM_FONT_SYMBOL16X16  12
#define SYSTEM_FONT_PHONETIC     13
#define SYSTEM_FONT_ENGLISH8X8   14
#define SYSTEM_FONT_ENGLISH8X6   15
#define SYSTEM_FONT_CHINESE16X16  16
#define SYSTEM_FONT_CHINESE24X24  17
#define SYSTEM_FONT_SYMBOL24X24   18
#define SYSTEM_FONT_SYMBOL        19		//系统默认符号字体
#define SYSTEM_FONT_TRUETYPE      20
        
#define SYSTEM_FONT_TRUETYPE_BIG           21         //系统大字体
#define SYSTEM_FONT_TRUETYPE_SMALL         22         //系统小字体

#define STOCK_GDIOBJ_NUM          23


#define GetStockObject Gdi_GetStockObject
HGDIOBJ WINAPI Gdi_GetStockObject( int fObject );

typedef struct _TEXTMETRIC
{ // tm
  LONG tmHeight; 
  LONG tmAscent; 
  LONG tmDescent; 
  LONG tmInternalLeading; 
  LONG tmExternalLeading; 
  LONG tmAveCharWidth; 
  LONG tmMaxCharWidth; 
  LONG tmWeight; 
  LONG tmOverhang; 
  LONG tmDigitizedAspectX; 
  LONG tmDigitizedAspectY; 
  TCHAR tmFirstChar; 
  TCHAR tmLastChar; 
  TCHAR tmDefaultChar; 
  TCHAR tmBreakChar; 
  BYTE tmItalic; 
  BYTE tmUnderlined; 
  BYTE tmStruckOut; 
  BYTE tmPitchAndFamily; 
  BYTE tmCharSet; 
}TEXTMETRIC, * PTEXTMETRIC, FAR * LPTEXTMETRIC; 

#define GetTextMetrics Gdi_GetTextMetrics
BOOL WINAPI Gdi_GetTextMetrics( HDC, LPTEXTMETRIC lptm );

#define GetCharWidth Gdi_GetCharWidth
BOOL WINAPI Gdi_GetCharWidth( HDC, WORD iFirst, WORD iLast, LPINT lpiBuf );

#define MAKEINTRESOURCE( v ) (LPTSTR)((DWORD)((WORD)(v)))

// predefined resource types

#define RT_CURSOR           MAKEINTRESOURCE(1)
#define RT_BITMAP           MAKEINTRESOURCE(2)
#define RT_ICON             MAKEINTRESOURCE(3)
#define RT_MENU             MAKEINTRESOURCE(4)
#define RT_DIALOG           MAKEINTRESOURCE(5)
#define RT_STRING           MAKEINTRESOURCE(6)
#define RT_FONTDIR          MAKEINTRESOURCE(7)
#define RT_FONT             MAKEINTRESOURCE(8)
#define RT_ACCELERATOR      MAKEINTRESOURCE(9)
#define RT_RCDATA           MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE     MAKEINTRESOURCE(11)

#define DIFFERENCE          11
#define RT_GROUP_CURSOR MAKEINTRESOURCE((DWORD)RT_CURSOR + DIFFERENCE)
#define RT_GROUP_ICON   MAKEINTRESOURCE((DWORD)RT_ICON + DIFFERENCE)
#define RT_VERSION      MAKEINTRESOURCE(16)
#define RT_DLGINCLUDE   MAKEINTRESOURCE(17)

#define IMAGE_BITMAP        0
#define IMAGE_ICON          1
#define IMAGE_CURSOR        2

#define LR_LOADFROMFILE     0x0001
#define LR_DEFAULTSIZE      0x0040
#define LR_SHARED           0x8000

#define LoadImage Gdi_LoadImage
HANDLE WINAPI Gdi_LoadImage( 
                     HINSTANCE hinst,
                     LPCTSTR lpszName, 
                     UINT uType, 
                     int cxDesired, 
                     int cyDesired, 
                     UINT fuLoad 
                     );

#define LoadIcon Gdi_LoadIcon
HICON WINAPI Gdi_LoadIcon(  HINSTANCE hInstance, LPCTSTR lpIconName );

#define IDI_SYSID          0x7E00    //32256
#define IDI_APPLICATION     MAKEINTRESOURCE(IDI_SYSID) 

#define LoadCursor Gdi_LoadCursor
HCURSOR WINAPI Gdi_LoadCursor( HINSTANCE hInstance, LPCTSTR lpCursorName );

#define IDC_SYSID          0x7E00    //32256
#define IDC_ARROW          MAKEINTRESOURCE(32256)
#define IDC_IBEAM          MAKEINTRESOURCE(32257)

#define Arc Gdi_Arc
BOOL WINAPI Gdi_Arc(
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

#define SetDIBitsToDevice Gdi_SetDIBitsToDevice
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
						 );

#define TextOutW Gdi_TextOutW
BOOL WINAPI Gdi_TextOutW( HDC, int x, int y, LPCTSTR, int count );

#define GetTextExtentPoint32W Gdi_GetTextExtentPoint32W
BOOL WINAPI Gdi_GetTextExtentPoint32W( HDC hdc, LPCSTR lpcstr, int nCount, LPSIZE lpSize );

#define CreateFontIndirect Gdi_CreateFontIndirect
HFONT WINAPI Gdi_CreateFontIndirect( CONST LOGFONT *lplf );

#define CreateFont Gdi_CreateFont
HFONT WINAPI Gdi_CreateFont(
							  int nHeight,
							  int nWidth,
							  int nEscapement,
							  int nOrientation,
							  int fnWeight,
							  DWORD fdwItalic,
							  DWORD fdwUnderline,
							  DWORD fdwStrikeOut,
							  DWORD fdwCharSet,
							  DWORD fdwOutputPrecision,
							  DWORD fdwClipPrecision,
							  DWORD fdwQuality,
							  DWORD fdwPitchAndFamily,
							  LPCTSTR lpszFace
						     );
#define Pie Gdi_Pie
BOOL WINAPI Gdi_Pie(
					HDC hdc,
					int nLeftRect,
					int nTopRect,
					int nRightRect,
					int nBottomRect,
					int nXRadial1,
					int nYRadial1,
					int nXRadial2,
					int nYRadial2
					);

#define Polygon Gdi_Polygon
BOOL WINAPI Gdi_Polygon( HDC hdc, CONST POINT *lpPoints, int nCount );

#define PlgBlt Gdi_PlgBlt
BOOL WINAPI Gdi_PlgBlt(
					   HDC hdcDest,
					   CONST POINT *lpPoint,
					   HDC hdcSrc,
					   int nXSrc,
					   int nYSrc,
					   int nWidth,
					   int nHeight,
					   HBITMAP hbmMask,
					   int xMask,
					   int yMask
					   );

//假如 (clTransparent & 0x80000000) != 0, 
//则说明 clTransparent 是表示一个在源位图的设备相关的颜色值，否则，是一个RGB值
#define TB_DEV_COLOR        0x80000000
#define TransparentBlt Gdi_TransparentBlt
BOOL WINAPI Gdi_TransparentBlt(   HDC hdcDest, 
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

#define StretchBlt Gdi_StretchBlt
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
				DWORD dwRop );

#define WindowFromDC Gdi_WindowFromDC
HWND WINAPI Gdi_WindowFromDC( HDC hdc );

#define SetDIBColorTable Gdi_SetDIBColorTable
UINT WINAPI Gdi_SetDIBColorTable( HDC hdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors );

// 设备
#define DT_PLOTTER          0
#define DT_RASDISPLAY       1
#define DT_RASPRINTER       2
#define DT_RASCAMERA        3
#define DT_CHARSTREAM       4
#define DT_METAFILE         5
#define DT_DISPFILE         6

// 设备能力
#define DRIVERVERSION 0
#define TECHNOLOGY    2
#define HORZSIZE      4
#define VERTSIZE      6
#define HORZRES       8
#define VERTRES       10
#define BITSPIXEL     12
#define PLANES        14
#define NUMBRUSHES    16
#define NUMPENS       18
#define NUMMARKERS    20
#define NUMFONTS      22
#define NUMCOLORS     24
#define PDEVICESIZE   26
#define CURVECAPS     28
#define LINECAPS      30
#define POLYGONALCAPS 32
#define TEXTCAPS      34
#define CLIPCAPS      36
#define RASTERCAPS    38
#define ASPECTX       40
#define ASPECTY       42
#define ASPECTXY      44

#define LOGPIXELSX    88 
#define LOGPIXELSY    90

#define SIZEPALETTE  104
#define NUMRESERVED  106
#define COLORRES     108

//打印机相关
#define PHYSICALWIDTH   110 
#define PHYSICALHEIGHT  111 
#define PHYSICALOFFSETX 112 
#define PHYSICALOFFSETY 113 
#define SCALINGFACTORX  114 
#define SCALINGFACTORY  115 


// 光栅能力
#define RC_NONE             0
#define RC_BITBLT           1
#define RC_BANDING          2
#define RC_SCALING          4
#define RC_BITMAP64         8
#define RC_GDI20_OUTPUT     0x0010
#define RC_GDI20_STATE      0x0020
#define RC_SAVEBITMAP       0x0040
#define RC_DI_BITMAP        0x0080
#define RC_PALETTE          0x0100
#define RC_DIBTODEV         0x0200
#define RC_BIGFONT          0x0400
#define RC_STRETCHBLT       0x0800
#define RC_FLOODFILL        0x1000
#define RC_STRETCHDIB       0x2000
#define RC_OP_DX_OUTPUT     0x4000
#define RC_DEVBITS          0x8000


// 曲线能力
#define CC_NONE             0
#define CC_CIRCLES          1
#define CC_PIE              2
#define CC_CHORD            4
#define CC_ELLIPSES         8
#define CC_WIDE             16
#define CC_STYLED           32
#define CC_WIDESTYLED       64
#define CC_INTERIORS        128
#define CC_ROUNDRECT        256

// 线能力
#define LC_NONE             0
#define LC_POLYLINE         2
#define LC_MARKER           4
#define LC_POLYMARKER       8
#define LC_WIDE             16
#define LC_STYLED           32
#define LC_WIDESTYLED       64
#define LC_INTERIORS        128

//多边形能力
#define PC_NONE             0
#define PC_POLYGON          1
#define PC_RECTANGLE        2
#define PC_WINDPOLYGON      4
#define PC_TRAPEZOID        4
#define PC_SCANLINE         8
#define PC_WIDE             16
#define PC_STYLED           32
#define PC_WIDESTYLED       64
#define PC_INTERIORS        128
#define PC_POLYPOLYGON      256
#define PC_PATHS            512


//文本能力
#define TC_OP_CHARACTER     0x00000001
#define TC_OP_STROKE        0x00000002
#define TC_CP_STROKE        0x00000004
#define TC_CR_90            0x00000008
#define TC_CR_ANY           0x00000010
#define TC_SF_X_YINDEP      0x00000020
#define TC_SA_DOUBLE        0x00000040
#define TC_SA_INTEGER       0x00000080
#define TC_SA_CONTIN        0x00000100
#define TC_EA_DOUBLE        0x00000200
#define TC_IA_ABLE          0x00000400
#define TC_UA_ABLE          0x00000800
#define TC_SO_ABLE          0x00001000
#define TC_RA_ABLE          0x00002000
#define TC_VA_ABLE          0x00004000
#define TC_RESERVED         0x00008000
#define TC_SCROLLBLT        0x00010000

#define GetDeviceCaps Gdi_GetDeviceCaps
int WINAPI Gdi_GetDeviceCaps( HDC hdc, int nIndex );

#define SetPixelV Gdi_SetPixelV
BOOL WINAPI Gdi_SetPixelV( HDC hdc, int x, int y, COLORREF color );

//逻辑调色板
typedef struct _LOGPALETTE {
    WORD        palVersion;
    WORD        palNumEntries;
    PALETTEENTRY        palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, FAR *LPLOGPALETTE;


#define AC_SRC_OVER			1
#define AC_SRC_ALPHA		1

typedef struct _BLENDFUNCTION {
	BYTE     BlendOp;
	BYTE     BlendFlags;
	BYTE     SourceConstantAlpha;
	BYTE     AlphaFormat;
	//DWORD    rgbTransparent;  //透明RGB, -1 无效
}BLENDFUNCTION, *PBLENDFUNCTION, *LPBLENDFUNCTION;

typedef struct _BLENDFUNCTION_EX {
	BLENDFUNCTION bf;

	DWORD    rgbTransparent;  //透明RGB, -1 无效
}BLENDFUNCTION_EX, *PBLENDFUNCTION_EX, *LPBLENDFUNCTION_EX;

#define AlphaBlend Gdi_AlphaBlend
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
					);


#define AlphaBlendEx Gdi_AlphaBlendEx
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
					);



#ifdef __cplusplus
}
#endif  // __cplusplus


#endif // __EGDI_H
