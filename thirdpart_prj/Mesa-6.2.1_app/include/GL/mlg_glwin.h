// 定义GLU库中没有定义的结构或者其他的定义

#ifndef __MLG_GLWIN
#define __MLG_GLWIN

#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */

#include "erect.h"

typedef struct tagMINMAXINFO {
    POINT ptReserved;
    POINT ptMaxSize;
    POINT ptMaxPosition;
    POINT ptMinTrackSize;
    POINT ptMaxTrackSize;
} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;

// 补充定义
#define VK_MENU				0x12
#define WM_ENTERMENULOOP	0x0211
#define WM_EXITMENULOOP		0x0212
#define WM_MENUSELECT		0x011F
#define WM_GETMINMAXINFO	0x0024
#define JOYSTICKID1			0
#define JOYERR_NOERROR		0
#define WM_PALETTECHANGED	0x0311
#define WM_QUERYNEWPALETTE	0x030F

#define MM_JOY1MOVE 0x3A0
#define MM_JOY1ZMOVE 0x3A2
#define MM_JOY1BUTTONDOWN 0x3B5
#define MM_JOY1BUTTONUP 0x3B7

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif