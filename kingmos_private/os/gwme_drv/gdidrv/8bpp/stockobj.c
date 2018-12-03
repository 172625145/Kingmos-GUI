#include <eframe.h>
#include <eassert.h>
#include <gwmedrv.h>
 
extern const _FONTDRV FAR _SYSFont;
_BRUSHDATA _whiteBrush={ {OBJ_BRUSH|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 255, { 0, 0 }, BS_SOLID, };
_BRUSHDATA _blackBrush={ {OBJ_BRUSH|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 0, { 0, 0 }, BS_SOLID, };
_BRUSHDATA _nullBrush={ {OBJ_BRUSH|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 0, { 0, 0 }, BS_NULL, };
_BRUSHDATA _ltgrayBrush={ {OBJ_BRUSH|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 7, { 0, 0 }, BS_SOLID, };
_BRUSHDATA _dkgrayBrush={ {OBJ_BRUSH|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 248, { 0, 0 }, BS_SOLID, };
_PENDATA   _whitePen={ { OBJ_PEN|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 255, PS_SOLID, 1 };
_PENDATA   _blackPen={ { OBJ_PEN|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 0, PS_SOLID, 1 };
_PENDATA   _nullPen={ { OBJ_PEN|OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 }, 0, PS_NULL, 1 };
/*
static const BYTE bMask[] = {
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
	0xff, 0xff,
};

static const _BITMAPDATA bmpMask = {
	{ OBJ_BITMAP|OBJ_FREE_DISABLE|OBJ_WRITE_DISABLE, NULL, NULL, NULL },
	0,
	16,
	16,
	2,
	1,
	1,
	bMask
};

const BYTE bColor[] = {
    0X00,0X00,
    0X7F,0XFE,
    0X40,0X02,
    0X40,0XAA,
    0X40,0X02,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X7F,0XFE,
    0X00,0X00,
   };

const static _BITMAPDATA bmpColor = {
	{ OBJ_BITMAP|OBJ_FREE_DISABLE|OBJ_WRITE_DISABLE, NULL, NULL, NULL },
	0,
	16,
	16,
	2,
	1,
	1,
	bColor
};

const static _ICONDATA icnDefaultApIcon = 
{
    OBJ_ICON|OBJ_FREE_DISABLE|OBJ_WRITE_DISABLE, NULL, NULL,
    1,    // 1, ICON ; 0 CURSOR
    0,
    0,

    (HBITMAP)&bmpMask,
    (HBITMAP)&bmpColor
};
*/


const HFONT _hSYSFont = (HFONT)&_SYSFont;

BOOL _IsStockObject( HGDIOBJ hObject )
{
    if( (hObject >= (HGDIOBJ)&_whiteBrush &&
        hObject <= (HGDIOBJ)&_nullPen) ||
        hObject == _hSYSFont )
        return TRUE;
    return FALSE;
}

BOOL _InitStockObject( void )
{
	
    /*_whiteBrush.color = _lpDrvDisplayDefault->lpRealizeColor( _whiteBrush.color );
	_blackBrush.color = _lpDrvDisplayDefault->lpRealizeColor( _blackBrush.color );
	_ltgrayBrush.color = _lpDrvDisplayDefault->lpRealizeColor( _ltgrayBrush.color );
	_dkgrayBrush.color = _lpDrvDisplayDefault->lpRealizeColor( _dkgrayBrush.color );

	_whitePen.color = _lpDrvDisplayDefault->lpRealizeColor( _whitePen.color );
	_blackPen.color = _lpDrvDisplayDefault->lpRealizeColor( _blackPen.color );
	//_blackBrush.color = _lpDrvDisplayDefault->lpRealizeColor( _blackBrush.color );
    */
	return TRUE;
}


HGDIOBJ WINAPI OEM_GetStockObject( int fObject )
{
    switch( fObject )
    {
    case WHITE_BRUSH:
        return (HGDIOBJ)&_whiteBrush;
    case BLACK_BRUSH:
        return (HGDIOBJ)&_blackBrush;
    case WHITE_PEN:
        return (HGDIOBJ)&_whitePen;
    case BLACK_PEN:
        return (HGDIOBJ)&_blackPen;
    case SYSTEM_FONT:
        return (HGDIOBJ)_hSYSFont;
    case LTGRAY_BRUSH:
        return (HGDIOBJ)&_ltgrayBrush;
    case DKGRAY_BRUSH:
        return (HGDIOBJ)&_dkgrayBrush;
    case GRAY_BRUSH:
        return (HGDIOBJ)&_ltgrayBrush;
    case NULL_BRUSH:
        return (HGDIOBJ)&_nullBrush;
    case NULL_PEN:
        return (HGDIOBJ)&_nullPen;
    //case OEM_FIXED_FONT:
        //return (HGDIOBJ)0;
    case ANSI_FIXED_FONT:
        return (HGDIOBJ)0;
    case ANSI_VAR_FONT:
        return (HGDIOBJ)0;
	//case 10000:   //APPLICATION_ICON:
		//return &icnDefaultApIcon;
    //case DEVICE_DEFAULT_FONT:
        //return (HGDIOBJ)0;
    //case DEFAULT_PALETTE:
        //return (HGDIOBJ)0;
    //case SYSTEM_FIXED_FONT:
        //return (HGDIOBJ)_hSYSFont;
    }
    ASSERT_NOTIFY( 0, "Call GetStockObject: not exist the StockObject\r\n"  );
    return 0;
}
