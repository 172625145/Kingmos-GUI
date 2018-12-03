/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ESYSSET_H
#define __ESYSSET_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#define SM_CXSCREEN             0
#define SM_CYSCREEN             1
#define SM_CXVSCROLL            2
#define SM_CYHSCROLL            3
#define SM_CYCAPTION            4
#define SM_CXBORDER             5
#define SM_CYBORDER             6
#define SM_CXDLGFRAME           7
#define SM_CYDLGFRAME           8
#define SM_CXFRAME              9
#define SM_CYFRAME              10
#define SM_CXICON               11
#define SM_CYICON               12
#define SM_CXCURSOR             13
#define SM_CYCURSOR             14
#define SM_CYMENU               15
#define SM_CXFULLSCREEN         16
#define SM_CYFULLSCREEN         17
#define SM_CYVSCROLL            18
#define SM_CXHSCROLL            19
#define SM_DEBUG                20
#define SM_CXMIN                21
#define SM_CYMIN                22
#define SM_CXSIZE               23
#define SM_CYSIZE               24
#define SM_CXMINTRACK           25
#define SM_CYMINTRACK           26
#define SM_CXICONSPACING        27
#define SM_CYICONSPACING        28
#define SM_PENWINDOWS           29
#define SM_CXMAXTRACK           30
#define SM_CYMAXTRACK           31
#define SM_CXMAXIMIZED          32
#define SM_CYMAXIMIZED          33
#define SM_NETWORK              34
#define SM_CXSMICON             35
#define SM_CYSMICON             36
#define SM_CYVTHUMB             37
#define SM_CXHTHUMB             38
#define SM_CXTOUCH              39
#define SM_CYTOUCH              40
#define SM_XTOUCHOFF            41
#define SM_YTOUCHOFF            42
#define SM_XVIEW                43
#define SM_YVIEW                44
// 当用固定大小的光栅字体，以下两个有效
#define SM_CXSYSFONT            45
#define SM_CYSYSFONT            46
//

#define SM_XCENTER              47		//对话框居中 X
#define SM_YCENTER              48		//对话框居中 Y
#define SM_CXCENTER             49		//对话框居中 CX
#define SM_CYCENTER             50		//对话框居中 CY
// 定义List item默认高度
#define SM_CYLISTITEM           51

// Color Types
enum {
    COLOR_3DDKSHADOW,//            0
    COLOR_BTNFACE,//               1
	COLOR_BTNHILIGHT,//            2  
	COLOR_3DLIGHT,//               3
	COLOR_BTNSHADOW,//             4
	COLOR_ACTIVEBORDER,//          5
	COLOR_ACTIVECAPTION,//         6
	COLOR_APPWORKSPACE,//          7
	COLOR_BACKGROUND,//            8
	COLOR_BTNTEXT,//               9
	COLOR_CAPTIONTEXT,//           10
	COLOR_GRAYTEXT,//              11
	COLOR_HIGHLIGHT,//             12 
	COLOR_HIGHLIGHTTEXT,//         13
	COLOR_INACTIVEBORDER,//        14
    COLOR_INACTIVECAPTION,//       15
    COLOR_INACTIVECAPTIONTEXT,//   16
    COLOR_INFOBK,//               17
    COLOR_INFOTEXT,//              18
    COLOR_MENU,//                  19 
    COLOR_MENUTEXT,//              20
    COLOR_SCROLLBAR,//             21
    COLOR_STATIC,//                22
    COLOR_STATICTEXT,//            23
    COLOR_WINDOW,//                24
    COLOR_WINDOWFRAME,//           25
    COLOR_WINDOWTEXT,//            26
	COLOR_SCROLLBARTHUMB,			// 滚动条背景颜色

	SYS_COLOR_NUM
};

#define COLOR_3DFACE  COLOR_BTNFACE
    
#define COLOR_3DHILIGHT             COLOR_BTNHILIGHT
#define COLOR_3DHIGHLIGHT           COLOR_BTNHILIGHT
#define COLOR_BTNHIGHLIGHT          COLOR_BTNHILIGHT

#define COLOR_3DSHADOW              COLOR_BTNSHADOW

#define COLOR_DESKTOP               COLOR_BACKGROUND

#define GetSystemMetrics Sys_GetMetrics
int WINAPI Sys_GetMetrics( int nIndex );

#define SetSystemMetrics Sys_SetMetrics
int WINAPI Sys_SetMetrics( int nIndex, int iNewValue );

#define GetSysColor Sys_GetColor
DWORD WINAPI Sys_GetColor(int nIndex);

#define SetSysColors Sys_SetColors
BOOL WINAPI Sys_SetColors(int cElements, 
						  const int *lpaElements, 
						  const COLORREF *lpaRgbValues );

#define GetSysColorBrush Sys_GetColorBrush
HBRUSH WINAPI Sys_GetColorBrush(int nIndex);

#define TouchCalibrate Sys_TouchCalibrate
BOOL WINAPI Sys_TouchCalibrate( void );
BOOL WINAPI Sys_SetCalibrateWindow( HWND hWnd );
BOOL WINAPI Sys_SetCalibratePoints(   
	int   cCalibrationPoints,
    int   *pScreenXBuffer,
    int   *pScreenYBuffer,
    int   *pUncalXBuffer,
    int   *pUncalYBuffer
    );

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif   // __ESYSSET_H
