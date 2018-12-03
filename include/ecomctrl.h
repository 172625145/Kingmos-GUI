/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ECOMCTRL_H
#define __ECOMCTRL_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#include "imgbrowser.h"

extern const char classLISTCTRL[];
extern const char classSETDATE[];
extern const char classUPDOWN32[];
extern const char classCANVAS[];
extern const char classKEYBOARD[];
extern const char classTabCtrl[] ;
extern const char classSliderCtrl[];
extern const char classHtmlView[];
extern const char classSMIE_BROWERS[];
extern const char classFileBrowser[];
extern const char classUPDOWN32[];
extern const char classDATESEL[];
extern const char classDATESHOW[];
extern const char classEDITTIME[];
extern const char classComboToolBar[];
extern const TCHAR	classTOOLBAR[];
extern const TCHAR classMMSBrowser[];
extern const TCHAR classIMGBrowser[];
extern const TCHAR classIPADDRESS[];
const char classPROGRESS[];


BOOL WINAPI InitCommonControls( void );

typedef struct _INITCOMMONCONTROLSEX 
{
    DWORD dwSize;
    DWORD dwICC;
} INITCOMMONCONTROLSEX, *LPINITCOMMONCONTROLSEX;

#define ICC_LISTVIEW_CLASSES 0x00000001 
#define ICC_TREEVIEW_CLASSES 0x00000002 
#define ICC_BAR_CLASSES      0x00000004 
#define ICC_TAB_CLASSES      0x00000008 
#define ICC_UPDOWN_CLASS     0x00000010 
#define ICC_PROGRESS_CLASS   0x00000020 
#define ICC_DATE_CLASSES     0x00000100 
#define ICC_COOL_CLASSES     0x00000400 
#define ICC_INTERNET_CLASSES 0x00000800 
#define ICC_TOOLTIP_CLASSES  0x00001000 
#define ICC_CAPEDIT_CLASS    0x00002000 
#define ICC_CANVAS_CLASS	 0x00010000 
#define ICC_SHEET_CLASS		 0x00010000 
BOOL WINAPI InitCommonControlsEx(LPINITCOMMONCONTROLSEX);

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // __ECOMCTRL_H
