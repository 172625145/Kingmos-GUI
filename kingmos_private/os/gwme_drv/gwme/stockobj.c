/***************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：
版本号：2.0.0
开发时期：1999
作者：李林
修改记录：
**************************************************/

#include <eframe.h>
#include <eassert.h>
#include <gwmeobj.h>
#include <gdc.h>
#include <epwin.h>
#include <gwmesrv.h>

//保存所有的stock对象  
static HGDIOBJ hStockObjects[SYS_STOCK_GDIOBJ_NUM];
//
extern HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle );
extern HPEN _CreateSysPen( COLORREF color, UINT uiStyle );
extern HFONT _CreateSysFont( UINT id );
extern HBITMAP _CreateSysBitmap( int nWidth, int nHeight, UINT cBitsPerPel );
extern HRGN _CreateSysRgn( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect );

// **************************************************
// 声明：void LoadGwmeResource( HINSTANCE hInstance )
// 参数：
// 	hInstance - 实例句柄
// 返回值：
//	无
// 功能描述：
//	初始化图形资源
// 引用: 
//	
// ************************************************
#define DEBUG_LoadGwmeResource 0
static void LoadGwmeResource( HINSTANCE hInstance )
{
	int yIconHeight = WinSys_GetMetrics(SM_CYSMICON);
    int xIconWidth = WinSys_GetMetrics(SM_CXSMICON);

	DEBUGMSG( DEBUG_LoadGwmeResource, ( "LoadGwmeResource entry.\r\n" ) );
	/*
	hicoSysMenu = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OIC_SYSLOGO), IMAGE_ICON, xIconWidth, yIconHeight, LR_SHARED );
	hicoGraySysMenu = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OIC_SYSLOGO_GRAY), IMAGE_ICON, xIconWidth, yIconHeight, LR_SHARED );
	hbmpClose = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_CLOSE), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hbmpGrayClose = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_CLOSE_GRAY ), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hbmpOk = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_OK), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hbmpGrayOk = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_OK_GRAY), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hbmpHelp = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_HELP), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hbmpGrayHelp = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_HELP_GRAY), IMAGE_BITMAP, 0, 0, LR_SHARED );
	*/
	hStockObjects[SYS_STOCK_LOGO] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OIC_SYSLOGO), IMAGE_ICON, xIconWidth, yIconHeight, LR_SHARED );
	hStockObjects[SYS_STOCK_LOGO_GRAY] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OIC_SYSLOGO_GRAY), IMAGE_ICON, xIconWidth, yIconHeight, LR_SHARED );
	hStockObjects[SYS_STOCK_CAPTION_CLOSE] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_CLOSE), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hStockObjects[SYS_STOCK_CAPTION_CLOSE_GRAY] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_CLOSE_GRAY ), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hStockObjects[SYS_STOCK_CAPTION_OK] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_OK), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hStockObjects[SYS_STOCK_CAPTION_OK_GRAY] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_OK_GRAY), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hStockObjects[SYS_STOCK_CAPTION_HELP] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_HELP), IMAGE_BITMAP, 0, 0, LR_SHARED );
	hStockObjects[SYS_STOCK_CAPTION_HELP_GRAY] = WinGdi_LoadImage( NULL, MAKEINTRESOURCE(OBM_CAPTION_HELP_GRAY), IMAGE_BITMAP, 0, 0, LR_SHARED );

	DEBUGMSG( DEBUG_LoadGwmeResource, ( "LoadGwmeResource leave.\r\n" ) );
}

/**************************************************
声明：BOOL _InitStockObject( void )
参数：
	无
返回值：
	假如成功，返回TRUE; 否则，返回FALSE
功能描述：
	初始化stock对象
引用: 
	当gwme系统加载/初始化时，会调用该函数
************************************************/

BOOL _InitStockObject( void )
{
	memset( hStockObjects, 0, sizeof( hStockObjects ) );

	hStockObjects[WHITE_BRUSH] = _CreateSysBrush( CL_WHITE, BS_SOLID );
	hStockObjects[BLACK_BRUSH] = _CreateSysBrush( CL_BLACK, BS_SOLID );
	hStockObjects[LTGRAY_BRUSH] = _CreateSysBrush( CL_LIGHTGRAY, BS_SOLID );
	hStockObjects[GRAY_BRUSH] = _CreateSysBrush( CL_GRAY, BS_SOLID );//hStockObjects[DKGRAY_BRUSH];
	hStockObjects[DKGRAY_BRUSH] = _CreateSysBrush( CL_DARKGRAY, BS_SOLID );
	

	hStockObjects[NULL_BRUSH] = _CreateSysBrush( 0, BS_NULL );
	
	hStockObjects[WHITE_PEN] = _CreateSysPen( CL_WHITE, PS_SOLID );
	hStockObjects[BLACK_PEN] = _CreateSysPen( CL_BLACK, PS_SOLID );
    hStockObjects[NULL_PEN] = _CreateSysPen( CL_BLACK, PS_NULL );

#ifdef HAVE_16X16_SYMBOL_FIXED
	hStockObjects[SYSTEM_FONT_SYMBOL16X16] = _CreateSysFont( SYSTEM_FONT_SYMBOL16X16 );
#endif
#ifdef HAVE_24X24_SYMBOL_FIXED
	hStockObjects[SYSTEM_FONT_SYMBOL24X24] = _CreateSysFont( SYSTEM_FONT_SYMBOL24X24 );
#endif
#ifdef HAVE_8X16_PHONETIC_FIXED
	hStockObjects[SYSTEM_FONT_PHONETIC] = _CreateSysFont( SYSTEM_FONT_PHONETIC );
#endif
#ifdef HAVE_8X8_ENG_FIXED
	hStockObjects[SYSTEM_FONT_ENGLISH8X8] = _CreateSysFont( SYSTEM_FONT_ENGLISH8X8 );
#endif
#ifdef HAVE_8X6_ENG_FIXED
	hStockObjects[SYSTEM_FONT_ENGLISH8X6] = _CreateSysFont( SYSTEM_FONT_ENGLISH8X6 );
#endif
#ifdef HAVE_16X16_CHS_FIXED
	hStockObjects[SYSTEM_FONT_CHINESE16X16] = _CreateSysFont( SYSTEM_FONT_CHINESE16X16 );
#endif
#ifdef HAVE_24X24_CHS_FIXED
	hStockObjects[SYSTEM_FONT_CHINESE24X24] = _CreateSysFont( SYSTEM_FONT_CHINESE24X24 );
#endif

#ifdef HAVE_TRUETYPE
	//hStockObjects[SYSTEM_FONT_TRUETYPE_BIG] = hStockObjects[SYSTEM_FONT_TRUETYPE] = _CreateSysFont( SYSTEM_FONT_TRUETYPE_BIG );
	//hStockObjects[SYSTEM_FONT_TRUETYPE_SMALL] = _CreateSysFont( SYSTEM_FONT_TRUETYPE_SMALL );
	hStockObjects[SYSTEM_FONT_TRUETYPE_BIG] = _CreateSysFont( SYSTEM_FONT_TRUETYPE_BIG );
	hStockObjects[SYSTEM_FONT_TRUETYPE_SMALL] = hStockObjects[SYSTEM_FONT_TRUETYPE] = _CreateSysFont( SYSTEM_FONT_TRUETYPE_SMALL );

#endif


	hStockObjects[SYSTEM_FONT] = hStockObjects[_uSysFontID];
	hStockObjects[SYSTEM_FONT_SYMBOL] = hStockObjects[_uSysSymbolFontID];//_CreateSysFont(  );


	hStockObjects[SYS_STOCK_BITMAP1x1] = _CreateSysBitmap( 1, 1, 1 );

	hStockObjects[SYS_STOCK_RGN] = _CreateSysRgn( 0, 0, 0, 0 );
	hStockObjects[SYS_STOCK_WHITE] = (HGDIOBJ)lpDrvDisplayDefault->lpRealizeColor( CL_WHITE, NULL, 0, 0 );
	hStockObjects[SYS_STOCK_BLACK] = (HGDIOBJ)lpDrvDisplayDefault->lpRealizeColor( CL_BLACK, NULL, 0, 0 );

	LoadGwmeResource( hgwmeInstance );
	
	return TRUE;
}

BOOL _GetSysFontSize( SIZE * lpSize )
{
	if( hStockObjects[SYSTEM_FONT] )
	{
		_LPFONT lpFont = _GetHFONTPtr( hStockObjects[SYSTEM_FONT] );
		lpSize->cx = lpFont->lpDriver->lpMaxWidth( lpFont->handle );
		lpSize->cy = lpFont->lpDriver->lpMaxHeight( lpFont->handle );
		return TRUE;
	}
	return FALSE;
}

/**************************************************
声明：HGDIOBJ WINAPI OEM_GetStockObject( int fObject )
参数：
	fObject - stock对象索引
返回值：
	假如成功，返回有效的句柄；否则，返回NULL
功能描述：
	从系统得到一个stock对象
引用: 
	内部使用
************************************************/
HGDIOBJ WINAPI OEM_GetStockObject( int fObject )
{
	if( fObject >= 0 && fObject < SYS_STOCK_GDIOBJ_NUM )
	{
		return hStockObjects[fObject];
	}
	else
	{
		ASSERT( 0 );
	}
	SetLastError( ERROR_INVALID_PARAMETER ); 
	return NULL;
}
