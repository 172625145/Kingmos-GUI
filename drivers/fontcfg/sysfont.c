/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����ϵͳ������ģ��������
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

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
// ������_LPFONT _GetHFONTPtr( HFONT hFont )
// ������
//	IN hFont - ����������
// ����ֵ��
//	����ɹ������ض���ָ�룻���򣬷���NULL
// ����������
//	�õ��������ָ��
// ����: 
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

