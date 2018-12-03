#include <eframe.h>

//#include <egdi.h>
#include <eassert.h>
#include <gwmedrv.h>


extern const _FONTDRV FAR _SYSFont;
static _BRUSHDATA _whiteBrush={ OBJ_BRUSH, 1, { 0, 0 }, BS_SOLID, };
static _BRUSHDATA _blackBrush={ OBJ_BRUSH, 0, { 0, 0 }, BS_SOLID, };
static _BRUSHDATA _nullBrush={ OBJ_BRUSH, 0, { 0, 0 }, BS_NULL, };
static _BRUSHDATA _ltgrayBrush={ OBJ_BRUSH, 1, { 0, 0 }, BS_HATCHED,0,
                                {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55} };
static _BRUSHDATA _dkgrayBrush={ OBJ_BRUSH, 0, { 0, 0 }, BS_HATCHED,0,
                                {0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55} };

static _PENDATA   _whitePen={ OBJ_PEN, 1, PS_SOLID, 1 };
static _PENDATA   _blackPen={ OBJ_PEN, 0, PS_SOLID, 1 };
static _PENDATA   _nullPen={ OBJ_PEN, 0, PS_NULL, 1 };
const HFONT _hSYSFont = (HFONT)&_SYSFont;

BOOL _IsStockObject( HGDIOBJ hObject )
{
    if( (hObject >= (HGDIOBJ)&_whiteBrush &&
        hObject <= (HGDIOBJ)&_nullPen) ||
        hObject == _hSYSFont )
        return TRUE;
    return FALSE;
}

HGDIOBJ GetStockObject( int fObject )
{
    switch( fObject )
    {
    case WHITE_BRUSH:
        return (HGDIOBJ)&_whiteBrush;
    case LTGRAY_BRUSH:
        return (HGDIOBJ)&_ltgrayBrush;
    case GRAY_BRUSH:
        return (HGDIOBJ)&_ltgrayBrush;
    case DKGRAY_BRUSH:
        return (HGDIOBJ)&_dkgrayBrush;
    case BLACK_BRUSH:
        return (HGDIOBJ)&_blackBrush;
    case NULL_BRUSH:
        return (HGDIOBJ)&_nullBrush;
    case WHITE_PEN:
        return (HGDIOBJ)&_whitePen;
    case BLACK_PEN:
        return (HGDIOBJ)&_blackPen;
    case NULL_PEN:
        return (HGDIOBJ)&_nullPen;
    //case OEM_FIXED_FONT:
        //return (HGDIOBJ)NULL;
    case ANSI_FIXED_FONT:
        return (HGDIOBJ)NULL;
    case ANSI_VAR_FONT:
        return (HGDIOBJ)NULL;
    case SYSTEM_FONT:
        return (HGDIOBJ)_hSYSFont;
    //case DEVICE_DEFAULT_FONT:
        //return (HGDIOBJ)NULL;
    //case DEFAULT_PALETTE:
        //return (HGDIOBJ)NULL;
    //case SYSTEM_FIXED_FONT:
        //return (HGDIOBJ)_hSYSFont;
    }
    ASSERT( 0 );
    return 0;
}
