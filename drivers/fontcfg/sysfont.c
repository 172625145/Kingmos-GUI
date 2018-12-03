/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：系统字体字模驱动程序
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>
#include <eobjtype.h>

extern const _FONTDRV FAR _SYSFont16Drv;
extern const _FONTDRV FAR _SYSFont24Drv;
extern const _FONTDRV FAR _TrueTypeDrv;

//#define USE_24X24_CHINESE_FONT
/*
#ifdef USE_24X24_CHINESE_FONT
	const _FONTDRV FAR * _lpSYSFontDrv = &_SYSFont24Drv;
	const UINT _uSysFontID = SYSTEM_FONT_CHINESE24X24;
	const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL24X24;//SYSTEM_FONT_CHINESE24X24;
#else
	const _FONTDRV FAR * _lpSYSFontDrv = &_SYSFont16Drv;
	const UINT _uSysFontID = SYSTEM_FONT_CHINESE16X16;
	const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL16X16;//
#endif
*/

const _FONTDRV FAR * _lpSYSFontDrv = &_TrueTypeDrv;//_SYSFont24Drv;
const UINT _uSysFontID = SYSTEM_FONT_TRUETYPE;
const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL16X16;//

//const _FONTDRV FAR * _lpSYSFontDrv = &_SYSFont16Drv;//_SYSFont24Drv;

//const UINT _uSysFontID = SYSTEM_FONT_TRUETYPE;
//const UINT _uSysFontID = SYSTEM_FONT_TRUETYPE;
//const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL24X24;//SYSTEM_FONT_CHINESE24X24;
//const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL24X24;//SYSTEM_FONT_CHINESE24X24;
//	const _FONTDRV FAR * _lpSYSFontDrv = &_SYSFont24Drv;
//	const UINT _uSysFontID = SYSTEM_FONT_CHINESE24X24;
//	const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL24X24;//SYSTEM_FONT_CHINESE24X24;

//	const _FONTDRV FAR * _lpSYSFontDrv = &_SYSFont16Drv;
//	const UINT _uSysFontID = SYSTEM_FONT_CHINESE16X16;
//	const UINT _uSysSymbolFontID = SYSTEM_FONT_SYMBOL16X16;//

// *****************************************************************
// 声明：_LPFONT _GetHFONTPtr( HFONT hFont )
// 参数：
//	IN hFont - 字体句柄对象
// 返回值：
//	假如成功，返回对象指针；否则，返回NULL
// 功能描述：
//	得到字体对象指针
// 引用: 
//	
// *****************************************************************
_LPFONT _GetHFONTPtr( HFONT hFont )
{
	_LPFONT lpf = (_LPFONT)HANDLE_TO_PTR( hFont );

	if( lpf && 
		( (WORD)lpf->obj.objType ) == OBJ_FONT )
		return lpf;
    return NULL;
}

