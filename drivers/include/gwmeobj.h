/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __GWMEOBJ_H
#define __GWMEOBJ_H

#ifndef __EOBJLIST_H
#include <eobjlist.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

// rgn obj struct
typedef struct __RECTNODE {
    RECT rect;
    struct __RECTNODE FAR * lpNext;
}_RECTNODE;
typedef _RECTNODE FAR* _LPRECTNODE;

typedef struct __RGNDATA {
	OBJLIST   obj;   // OBJ_REGION

	RECT rect;
    short int count;
    _LPRECTNODE lpNodeFirst;
}_RGNDATA;
typedef _RGNDATA FAR* _LPRGNDATA;
_LPRGNDATA _GetHRGNPtr( HRGN hrgn );
/*
typedef struct __BITMAP
{
	OBJLIST   obj; // OBJ_BITMAP

    WORD        bmType;
    WORD        bmWidth;
    WORD        bmHeight;
    WORD        bmWidthBytes;
    WORD        bmPlanes;
    WORD        bmBitsPixel;
    LPBYTE      bmBits;
}_BITMAPDATA, FAR *_LPBITMAPDATA;
*/

#define BF_VIRTUAL_ALLOC      0x0001
#define BF_DIB                0x0002
#define BF_SHARE              0x0004
typedef struct __BITMAP
{
	OBJLIST   obj; // OBJ_BITMAP

    //BYTE      bmType;
	//BYTE      bmDir;   // 0, top -> bottom; 1, bottom->top
	WORD      bmFlags;//  0x0001, ->bmBits = VirtualAlloc else = malloc
	                  //  0x0002, the struct is _LPBITMAPDIB  
						// 0x0004, the struct is _LPBITMAP_SHARE  
	BYTE      bmPlanes;
	BYTE      bmBitsPixel;

    int       bmWidth;
    int       bmHeight;
    int       bmWidthBytes;
    LPBYTE    bmBits;
}_BITMAPDATA, FAR *_LPBITMAPDATA;

typedef struct __BITMAP_DIB
{
    _BITMAPDATA   bitmap;
	int    biDir;//
    DWORD  biCompression; 
    DWORD  biSizeImage; 
    LONG   biXPelsPerMeter; 
    LONG   biYPelsPerMeter; 
    DWORD  biClrUsed; 
    DWORD  biClrImportant;
	PALETTEENTRY  palEntry[1];
}_BITMAP_DIB, FAR *_LPBITMAP_DIB;

//需要共享的位图资源
typedef struct __BITMAP_SHARE
{
    _BITMAPDATA   bitmap;
	HINSTANCE hInst;	//
	DWORD szName;	//
}_BITMAP_SHARE, FAR *_LPBITMAP_SHARE;

_LPBITMAPDATA _GetHBITMAPPtr( HBITMAP );

struct __FONTDRV;

typedef struct __FONT
{
	OBJLIST   obj;// OBJ_FONT

	DWORD dwData;
	HANDLE handle;
	struct __FONTDRV FAR * lpDriver;
}_FONT, FAR * _LPFONT;
_LPFONT _GetHFONTPtr( HFONT );

typedef struct __ICONDATA
{	
	OBJLIST obj;

    WORD    fIcon;    // 1, ICON ; 0 CURSOR
    WORD    xHotspot;
    WORD    yHotspot;
	WORD    wIconName;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
	HANDLE  hInst;
}_ICONDATA, FAR *_LPICONDATA;
_LPICONDATA _GetHICONPtr( HICON );

typedef struct __BRUSHATTRIB
{
    COLORREF clrRef;// device indepent color
	DWORD color;// device depent color
    //POINT origin;
	_LPBITMAPDATA lpbmpBrush;
    BYTE style;
    BYTE hatch;
    BYTE pattern[8];
}_BRUSHATTRIB, * _PBRUSHATTRIB, FAR* _LPBRUSHATTRIB;
_LPBRUSHATTRIB _GetHBRUSHATTRIBPtr( HBRUSH );

typedef struct __BRUSHDATA
{
	OBJLIST   obj;  // OBJ_BRUSH
	_BRUSHATTRIB brushAttrib;
}_BRUSHDATA, * _PBRUSHDATA, FAR* _LPBRUSHDATA;
_LPBRUSHDATA _GetHBRUSHPtr( HBRUSH );


//#define PEN_OBJECT 2

typedef struct __PENATTRIB{
    COLORREF clrRef;// device indepent color
    DWORD color;// device depent color
    BYTE pattern;
    BYTE width;
}_PENATTRIB, * _PPENATTRIB, FAR* _LPPENATTRIB;
_LPPENATTRIB _GetHPENATTRIBPtr( HPEN );

typedef struct __PEN{
	OBJLIST   obj;   //OBJ_PEN
	_PENATTRIB penAttrib;
}_PENDATA, * _PPENDATA, FAR* _LPPENDATA;
_LPPENDATA _GetHPENPtr( HPEN );


// driver and data struct
struct __LINEDATA;
typedef VOID ( CALLBACK * LPLINECALLBACK )( struct __LINEDATA FAR * lpLineData, LPCRECT lprc );
typedef struct __LINEDATA
{
    _LPBITMAPDATA lpDestImage;
    int xStart;
    int yStart;

    BYTE pattern;
    BYTE rop;
    BYTE backMode;
    char iDir; //0-7

	 // 4\5|6/7
    //-----+------ 
	//	3/2|1\0
	//short xDir;  // 1 , -1
	//short yDir;  // 1, -1

	DWORD dm;
	DWORD dn;
	int   cPels;

	LONG  iErrorCount;

	//short dx;
	//short dy;
	int   width;

    LPRECT lprcClip; 

    COLORREF color;
    COLORREF clrBack;
	LPLINECALLBACK lpfnCallback; // 当调用斜线功能时，该值有效

}_LINEDATA, FAR * _LPLINEDATA;


typedef struct __PIXELDATA
{
    _LPBITMAPDATA lpDestImage;
    int x;
    int y;
    COLORREF color;
    BYTE pattern;
    BYTE rop;
}_PIXELDATA, FAR * _LPPIXELDATA;

typedef struct __BLKBITBLT
{
    _LPBITMAPDATA lpDestImage;
    LPCRECT lprcDest;

    _LPBITMAPDATA lpSrcImage;
    LPCRECT lprcSrc;

    _LPBITMAPDATA lpMaskImage;
    LPCRECT lprcMask;

	LPCRECT lprcClip;   // when strerch mode

    _LPBRUSHATTRIB lpBrush;
	LPPOINT lpptBrushOrg;

    COLORREF solidColor;
    COLORREF solidBkColor;
    int yPositive;   // use for copy
    int xPositive;  //  use for copy
    WORD backMode;
    DWORD dwRop;
}_BLKBITBLT, FAR * _LPBLKBITBLT;

// define device driver
#define PAL_INDEX    1   //调色板类型
#define PAL_BITFIELD 2	 //调色板类型
DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiSrcColorType );
DWORD _Gdi_RealizeColor( COLORREF clRgbColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiDestColorType );
typedef struct __DISPLAYDRV
{
    COLORREF (*lpPutPixel)( _LPPIXELDATA );
    COLORREF (*lpGetPixel)( _LPPIXELDATA );
    BOOL     (*lpLine)( _LPLINEDATA );
    BOOL     (*lpBlkBitTransparentBlt)( _LPBLKBITBLT );
    BOOL     (*lpBlkBitMaskBlt)( _LPBLKBITBLT );
    BOOL     (*lpBlkBitBlt)( _LPBLKBITBLT );
    COLORREF (*lpRealizeColor)( COLORREF, LPCDWORD lpdwPalEntry, UINT uPalNumber, UINT uPalFormat );
    COLORREF (*lpUnrealizeColor)( COLORREF, LPCDWORD lpdwPalEntry, UINT uPalNumber, UINT uPalFormat );
}_DISPLAYDRV, *_LPDISPLAYDRV;
typedef const _DISPLAYDRV * _LPCDISPLAYDRV;

typedef struct __CHAR_METRICS
{	//位图信息
	_BITMAPDATA bitmap;
	int       xAdvance;
	int       yAdvance;
	int       left;
	int       top;

}_CHAR_METRICS, FAR * _LPCHAR_METRICS;

typedef struct __FONTDRV
{
    DWORD (*lpInit)(void);
    BOOL  (*lpDeinit)( DWORD dwData );
	BOOL (*lpInstallFont)( DWORD dwData, LPCTSTR lpszPathName );
	HANDLE (*lpCreateFont)( DWORD dwData, const LOGFONT *lplf );
	BOOL (*lpDeleteFont)( HANDLE );
    int (*lpMaxHeight)( HANDLE );
    int (*lpMaxWidth)( HANDLE );
    int (*lpWordLength)( HANDLE, const BYTE FAR* lpText );
    int (*lpWordHeight)( HANDLE, WORD wWord );
    int (*lpWordWidth)( HANDLE, WORD wWord );
    int (*lpWordBitmap)( HANDLE, const BYTE FAR* lpText, UINT textLen, _LPCHAR_METRICS );
    int (*lpTextWidth)( HANDLE, const BYTE FAR* lpText, int iLimiteWidth );
    int (*lpTextHeight)( HANDLE, const BYTE FAR* lpText, int iLineWidth );
    const BYTE FAR* (*lpNextWord)( HANDLE, const BYTE FAR* );
}_FONTDRV, FAR *_LPFONTDRV;

// oem support driver
// graphic window driver interface
//typedef BOOL HandlePosEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo )
typedef BOOL ( CALLBACK * LPPOS_CALLBACK )( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );
typedef BOOL ( CALLBACK * LPPOS_CALIBRATION_CALLBACK )( DWORD x, DWORD y );

typedef DWORD ( CALLBACK * _LPGWDI_ENTER )( UINT msg, DWORD dwParam, LPVOID lParam );

// common message     1 ~ 255

#define GWDI_CREATE       1
#define GWDI_DESTROY      2
#define GWDI_OPEN         3
#define GWDI_CLOSE        4
#define GWDI_RESUME       5
#define GWDI_POWEROFF     6
#define GWDI_POWERON      7
#define GWDI_FLUSH        8

// 
// display dev message
#define GWDI_SET_PLANE            200
#define GWDI_GET_MODE_NUM         201
#define GWDI_GET_BITMAP           202
#define GWDI_GET_BITMAP_PTR       203
#define GWDI_GET_DISPLAY_PTR      204

// touch panel message
#define GWDI_CALIBRATION          200
// 

extern _BITMAPDATA * lpDisplayBitmap;
extern HBITMAP       hbmpDisplayBitmap;
extern HBITMAP		 hbmpCurrentFrameBuffer;
extern ULONG   ufNeedFlushGDICount;

extern _DISPLAYDRV * lpDrvDisplayDefault;
extern _DISPLAYDRV * lpDrvDisplayDevice;
// display driver 
extern _LPGWDI_ENTER lpGwdiDisplayEnter;
// pos driver , like mouse, touch...
extern _LPGWDI_ENTER  lpGwdiPosEnter;
// key driver
extern _LPGWDI_ENTER  lpGwdiKeyEnter;
//
// 初始化图形设备平台 oem init function
BOOL _InitDefaultGwdi( void );
// 设置定位设备回调函数，成功返回之前的回调函数，失败，NULL
LPPOS_CALLBACK _SetPosEventCallBack( LPPOS_CALLBACK lpNewCallBackFun );

extern const _DISPLAYDRV _drvDisplay1BPP;
extern const _DISPLAYDRV _drvDisplay4BPP;
extern const _DISPLAYDRV _drvDisplay8BPP;
extern const _DISPLAYDRV _drvDisplay16BPP;
extern const _DISPLAYDRV _drvDisplay32BPP;
extern const _DISPLAYDRV _drvDisplay24BPP;
extern HANDLE hgwmeBlockHeap;

//#define SYS_STOCK_GDIOBJ_NUM (STOCK_GDIOBJ_NUM+4)
enum {
    SYS_STOCK_BITMAP1x1 = STOCK_GDIOBJ_NUM,
    SYS_STOCK_RGN,
    SYS_STOCK_WHITE,
    SYS_STOCK_BLACK,
	SYS_STOCK_LOGO,
	SYS_STOCK_LOGO_GRAY,
	SYS_STOCK_CAPTION_CLOSE,
	SYS_STOCK_CAPTION_CLOSE_GRAY,
	SYS_STOCK_CAPTION_OK,
	SYS_STOCK_CAPTION_OK_GRAY,
	SYS_STOCK_CAPTION_HELP,
	SYS_STOCK_CAPTION_HELP_GRAY,


	SYS_STOCK_GDIOBJ_NUM
};

typedef struct __PALETTE_DATA{
	OBJLIST   obj;   //OBJ_PAL
	LOGPALETTE  logPal;
}_PALETTE_DATA, FAR * _LPPALETTE_DATA;


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // __GWMEOBJ_H




