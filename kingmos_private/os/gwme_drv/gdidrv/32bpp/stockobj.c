/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：图形设备驱动程序(32bit)-stock object define
版本号：1.0.0
开发时期：2003-7-17
作者：李林
修改记录：

******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <gwmeobj.h>
 
static HGDIOBJ hStockObjects[STOCK_GDIOBJ_NUM];

extern const _DISPLAYDRV * _lpDrvDisplayDefault;
extern HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle );
extern HPEN _CreateSysPen( COLORREF color, UINT uiStyle );
extern HFONT _CreateSysFont( UINT id );

BOOL _InitStockObject( void )
{
	memset( hStockObjects, 0, sizeof( hStockObjects ) );

	hStockObjects[WHITE_BRUSH] = _CreateSysBrush( CL_WHITE, BS_SOLID );
	hStockObjects[BLACK_BRUSH] = _CreateSysBrush( CL_BLACK, BS_SOLID );
	hStockObjects[LTGRAY_BRUSH] = _CreateSysBrush( CL_LIGHTGRAY, BS_SOLID );
	hStockObjects[DKGRAY_BRUSH] = _CreateSysBrush( CL_DARKGRAY, BS_SOLID );
	hStockObjects[NULL_BRUSH] = _CreateSysBrush( 0, BS_NULL );
	
	hStockObjects[WHITE_PEN] = _CreateSysPen( CL_WHITE, PS_SOLID );
	hStockObjects[BLACK_PEN] = _CreateSysPen( CL_BLACK, PS_SOLID );
    hStockObjects[NULL_PEN] = _CreateSysPen( CL_BLACK, PS_NULL );

	hStockObjects[SYSTEM_FONT] = _CreateSysFont( SYSTEM_FONT );
	hStockObjects[SYSTEM_FONT_SYMBOL16X16] = _CreateSysFont( SYSTEM_FONT_SYMBOL16X16 );
	hStockObjects[SYSTEM_FONT_PHONETIC] = _CreateSysFont( SYSTEM_FONT_PHONETIC );
	hStockObjects[SYSTEM_FONT_ENGLISH8X8] = _CreateSysFont( SYSTEM_FONT_ENGLISH8X8 );
	hStockObjects[SYSTEM_FONT_ENGLISH8X6] = _CreateSysFont( SYSTEM_FONT_ENGLISH8X6 );
	
	return TRUE;
}

HGDIOBJ WINAPI OEM_GetStockObject( int fObject )
{
	if( fObject >= 0 && fObject < STOCK_GDIOBJ_NUM )
	{
		return hStockObjects[fObject];
	}
	SetLastError( ERROR_INVALID_PARAMETER ); 
	return NULL;
}
