/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：gwme 服务器
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eapisrv.h>

#include <gwmesrv.h>
  
//////////////////////////////////////////////////////////////
//定义系统API（窗口对象处理）函数界面
static const PFNVOID lpWndAPI[] = {
	NULL,
//#define WND_CREATEEX        1         
    (PFNVOID)Win_CreateEx,
//#define WND_ISWINDOW       2
    (PFNVOID)Win_IsWindow,
//#define WND_SHOWSCROLLBAR         3
    (PFNVOID)NULL,//Win_ShowScrollBar,
//#define WND_DEFPROC        4
    (PFNVOID)NULL,//Win_DefProc,
//#define WND_DESTROY        5
    (PFNVOID)Win_Destroy,
//#define WND_ISCHILD        6
    (PFNVOID)Win_IsChild,
//#define WND_GETPARENT      7
    (PFNVOID)Win_GetParent,
//#define WND_SETPARENT      8
    (PFNVOID)Win_SetParent,
//#define WND_ISVISIBLE      9
    (PFNVOID)Win_IsVisible,
//#define WND_SHOW            10
    (PFNVOID)Win_Show,
//#define WND_ENABLE          11
    (PFNVOID)Win_Enable,
//#define WND_ISENABLED       12
    (PFNVOID)Win_IsEnabled,
//#define WND_SETTEXT         13
    (PFNVOID)Win_SetText,
//#define WND_GETTEXT         14
    (PFNVOID)Win_GetText,
//#define WND_GETTEXtLENGTH   15
    (PFNVOID)Win_GetTextLength,
//#define WND_GETCLIENTRECT   16
    (PFNVOID)Win_GetClientRect,
//#define WND_GETWINDOWRECT   17
    (PFNVOID)Win_GetWindowRect,
//#define WND_SETPOS          18
    (PFNVOID)Win_SetPos,
//#define WND_MOVE            19 
    (PFNVOID)Win_Move,
//#define WND_BRINGTOTOP      20
    (PFNVOID)Win_BringToTop,
//#define WND_CALLPROC        21
    (PFNVOID)Win_CallProc,
//#define WND_ADJUSTRECTEX    22
    (PFNVOID)Win_AdjustRectEx,
//#define WND_FLASH           23
    (PFNVOID)Win_Flash,
//#define WND_CLOSE           24
    (PFNVOID)Win_Close,
//#define WND_ISICONIC        25
    (PFNVOID)Win_IsIconic,
//#define WND_ISZOOMED        26
    (PFNVOID)Win_IsZoomed,
//#define WND_GETLONG         27
    (PFNVOID)Win_GetLong,
//#define WND_SETLONG        28
    (PFNVOID)Win_SetLong,
//#define WND_GETDESKTOP     29
    (PFNVOID)Win_GetDesktop,
//#define WND_FIND           30
    (PFNVOID)Win_Find,
//#define WND_UPDATE         31
    (PFNVOID)Win_Update,
//#define WND_GETUPDATERECT  32
    (PFNVOID)Win_GetUpdateRect,
//#define WND_GETUPDATERGN   33
    (PFNVOID)Win_GetUpdateRgn,
//#define WND_ENUM           34
    (PFNVOID)Win_Enum,
//#define WND_ENUMCHILD      35
    (PFNVOID)Win_EnumChild,
//#define WND_GETTOP         36
    (PFNVOID)Win_GetTop,
//#define WND_GET            37
    (PFNVOID)Win_Get,
//#define WND_GETNEXT        38
    (PFNVOID)Win_GetNext,
//#define WND_CLIENTTOSCREEN     39
    (PFNVOID)Win_ClientToScreen,
//#define WND_SCREENTOCLIENT     40
    (PFNVOID)Win_ScreenToClient,
//#define WND_MAPPOINTS          41
    (PFNVOID)Win_MapPoints,
//#define WND_FROMPOINT          42
    (PFNVOID)Win_FromPoint,
//#define WND_CHILDFROMPOINT      43
    (PFNVOID)Win_ChildFromPoint,
//#define WND_GETFOREGROUND       44
    (PFNVOID)Win_GetForeground,
//#define WND_SETFOREGROUND       45
    (PFNVOID)Win_SetForeground,
//#define WND_SETACTIVE           46
    (PFNVOID)Win_SetActive,
//#define WND_SETTIMER            47
    (PFNVOID)Win_SetTimer,
//#define WND_KILLTIMER           48
    (PFNVOID)Win_KillTimer,
//#define WND_SETSYSTIMER         49
    (PFNVOID)Win_SetSysTimer,
//#define WND_KILLSYSTIMER        50
    (PFNVOID)Win_KillSysTimer,
//#define WND_GETACTIVE           51
    (PFNVOID)Win_GetActive,
//#define WND_GETTOPLEVELPARENT   52
    (PFNVOID)Win_GetTopLevelParent,
//#define WND_SETFOCUS            53
    (PFNVOID)Win_SetFocus,
//#define WND_GETFOCUS            54
    (PFNVOID)Win_GetFocus,
//#define WND_SETCAPTURE          55
    (PFNVOID)Win_SetCapture,
//#define WND_RELEASECAPTURE      56
    (PFNVOID)Win_ReleaseCapture,
//#define WND_GETCAPTURE          57
    (PFNVOID)Win_GetCapture,
//#define WND_BEGINPAINT          58
    (PFNVOID)WinGdi_BeginPaint,
//#define WND_ENDPAINT            59
    (PFNVOID)WinGdi_EndPaint,
//#define WND_GETCLIENTDC         60
    (PFNVOID)WinGdi_GetClientDC,
//#define WND_RELEASEDC           61
    (PFNVOID)WinGdi_ReleaseDC,
//#define WND_GETWINDOWDC         62
    (PFNVOID)WinGdi_GetWindowDC,
//#define WND_DRAWCAPTION         63
    (PFNVOID)NULL,//Win_DrawCaption,
//#define WND_SCROLL              64
    (PFNVOID)Win_Scroll,
//#define WND_GETDCEX             65
    (PFNVOID)WinGdi_GetDCEx,
//#define WND_INVALIDATERECT      66
    (PFNVOID)Win_InvalidateRect,
//#define WND_INVALIDATERGN       67
    (PFNVOID)Win_InvalidateRgn,
//#define WND_VALIDATERECT        68
    (PFNVOID)Win_ValidateRect,
//#define WND_VALIDATERGN         69
    (PFNVOID)Win_ValidateRgn,
//#define WND_GETTHREADPROCESSID  70
    (PFNVOID)Win_GetThreadProcessId,
//#define WND_ENABLESCROLLBAR       71    
    (PFNVOID)NULL,//Win_EnableScrollBar,
//#define WND_GETSCROLLINFO         72
    (PFNVOID)NULL,//Win_GetScrollInfo,
//#define WND_SETSCROLLINFO         73
    (PFNVOID)NULL,//Win_SetScrollInfo,
//#define WND_GETCLASSNAME
	(PFNVOID)Win_GetClassName,
//#define WND_CLEAR                 74
    (PFNVOID)Win_Clear,
//#define CLASS_REGISTER            75
    (PFNVOID)WinClass_Register,
//#define CLASS_GETINFO             76
    (PFNVOID)WinClass_GetInfo,
//#define CLASS_UNREGISTER          77
    (PFNVOID)WinClass_Unregister,
//#define CLASS_GETLONG             78
    (PFNVOID)WinClass_GetLong,
//#define CLASS_GETLONG             79
    (PFNVOID)WinClass_SetLong,
//#define CLASS_CLEAR
	(PFNVOID)WinClass_Clear,

	(PFNVOID)NULL,//WinDlg_CreateIndirect,
	(PFNVOID)NULL,//WinDlg_CreateIndirectParam,
	(PFNVOID)NULL,//WinDlg_BoxIndirect,
	(PFNVOID)NULL,//WinDlg_BoxIndirectParam,
	(PFNVOID)NULL,//WinDlg_End,
	(PFNVOID)NULL,//WinDlg_GetCtrlID,
	(PFNVOID)NULL,//WinDlg_GetItem,
	(PFNVOID)NULL,//WinDlg_GetItemText,
	(PFNVOID)NULL,//WinDlg_SetItemText,
	(PFNVOID)NULL,//WinDlg_GetItemInt,
	(PFNVOID)NULL,//WinDlg_SetItemInt,
	(PFNVOID)NULL,//WinDlg_SendItemMessage,
	(PFNVOID)NULL,//WinDlg_MessageBox,
//ex
	(PFNVOID)NULL,//WinDlg_CreateIndirectEx,
	(PFNVOID)NULL,//WinDlg_CreateIndirectParamEx,
	(PFNVOID)NULL,//WinDlg_BoxIndirectEx,
	(PFNVOID)NULL,//WinDlg_BoxIndirectParamEx,
//ex-end
    (PFNVOID)Win_ShowDesktop,
	(PFNVOID)Win_SetRgn,
	(PFNVOID)Win_GetRgn
};

//定义系统API（窗口对象处理）参数界面
static const DWORD dwWndArgs[] = {
	NULL,
    ARG12_MAKE( DWORD, PTR, PTR, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//Win_CreateEx,
    ARG1_MAKE( DWORD ),//Win_IsWindow,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//ShowScrollBar,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//DefProc,
    ARG1_MAKE( DWORD ),//Destroy,
    ARG2_MAKE( DWORD, DWORD ),//IsChild,
    ARG1_MAKE( DWORD ),//GetParent,
    ARG2_MAKE( DWORD, DWORD ),//SetParent,
    ARG1_MAKE( DWORD ),//IsVisible,
    ARG2_MAKE( DWORD, DWORD ),//Show,
    ARG2_MAKE( DWORD, DWORD ),//Enable,
    ARG1_MAKE( DWORD ),//IsEnabled,
    ARG2_MAKE( DWORD, PTR ),//SetText,
    ARG3_MAKE( DWORD, PTR, DWORD ),//GetText,
    ARG1_MAKE( DWORD ),//GetTextLength,
    ARG2_MAKE( DWORD, PTR ),//GetClientRect,
    ARG2_MAKE( DWORD, PTR ),//GetWindowRect,
    ARG7_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//SetPos,
    ARG6_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//Move,
    ARG1_MAKE( DWORD ),//BringToTop,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//CallProc,
    ARG4_MAKE( PTR, DWORD, DWORD, DWORD ),//AdjustRectEx,
    ARG2_MAKE( DWORD, DWORD ),//Flash,
    ARG1_MAKE( DWORD ),//Close,
    ARG1_MAKE( DWORD ),//IsIconic,
    ARG1_MAKE( DWORD ),//IsZoomed,
    ARG2_MAKE( DWORD, DWORD ),//GetLong,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//SetLong,
    ARG0_MAKE(),//GetDesktop,
    ARG2_MAKE( PTR, PTR ),//Find,
    ARG1_MAKE( DWORD ),//Update,
    ARG3_MAKE( DWORD, PTR, DWORD ),//GetUpdateRect,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//GetUpdateRgn,
    ARG2_MAKE( DWORD, DWORD ),//Enum,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//EnumChild,
    ARG1_MAKE( DWORD ),//GetTop,
    ARG2_MAKE( DWORD, DWORD ),//Get,
    ARG2_MAKE( DWORD, DWORD ),//GetNext,
    ARG2_MAKE( DWORD, PTR ),//ClientToScreen,
    ARG2_MAKE( DWORD, PTR ),//ScreenToClient,
    ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//MapPoints,
    ARG2_MAKE( DWORD, DWORD ),//FromPoint,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//ChildFromPoint,
    ARG0_MAKE(),//GetForeground,
    ARG1_MAKE( DWORD ),//SetForeground,
    ARG1_MAKE( DWORD ),//SetActive,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//SetTimer,
    ARG2_MAKE( DWORD, DWORD ),//KillTimer,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//SetSysTimer,
    ARG2_MAKE( DWORD, DWORD ),//KillSysTimer,
    ARG0_MAKE(),//GetActive,
    ARG1_MAKE( DWORD ),//GetTopLevelParent,
    ARG1_MAKE( DWORD ),//SetFocus,
    ARG0_MAKE(),//GetFocus,
    ARG1_MAKE( DWORD ),//SetCapture,
    ARG0_MAKE(),//ReleaseCapture,
    ARG0_MAKE(),//GetCapture,
    ARG2_MAKE( DWORD, PTR ),//BeginPaint,
    ARG2_MAKE( DWORD, PTR ),//EndPaint,
    ARG1_MAKE( DWORD ),//GetClientDC,
    ARG2_MAKE( DWORD, DWORD ),//ReleaseDC,
    ARG1_MAKE( DWORD ),//GetWindowDC,
    ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//DrawCaption,
    ARG8_MAKE( DWORD, DWORD, DWORD, PTR, PTR, DWORD, PTR, DWORD ),//Scroll,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//GetDCEx,
    ARG3_MAKE( DWORD, PTR, DWORD ),//InvalidateRect,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//InvalidateRgn,
    ARG2_MAKE( DWORD, PTR ),//ValidateRect,
    ARG2_MAKE( DWORD, DWORD ),//ValidateRgn,
    ARG2_MAKE( DWORD, PTR ),//GetThreadProcessId,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//EnableScrollBar,
    ARG3_MAKE( DWORD, DWORD, PTR ),//GetScrollInfo,
    ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//SetScrollInfo,
	ARG3_MAKE( DWORD, PTR, DWORD ),//GetClassName,
    ARG1_MAKE( DWORD ),//Clear,
	// class
    ARG1_MAKE( PTR ),//Register,
    ARG3_MAKE( DWORD, PTR, PTR ),//GetInfo,
    ARG2_MAKE( PTR, DWORD ),//Unregister,
    ARG2_MAKE( DWORD, DWORD ),//GetLong,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//GetLong,
	ARG1_MAKE( DWORD ),//Clear,
	// dialog
	ARG4_MAKE( DWORD, PTR, DWORD, DWORD ),//CreateIndirect,
	ARG5_MAKE( DWORD, PTR, DWORD, DWORD, DWORD ),//CreateIndirectParam,
	ARG4_MAKE( DWORD, PTR, DWORD, DWORD ),//BoxIndirect,
	ARG5_MAKE( DWORD, PTR, DWORD, DWORD, DWORD ),//BoxIndirectParam,
	ARG2_MAKE( DWORD, DWORD ),//End,
	ARG1_MAKE( DWORD ),//GetCtrlID,
	ARG2_MAKE( DWORD, DWORD ),//GetItem,
	ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//GetDlgItemText,
	ARG3_MAKE( DWORD, DWORD, PTR ),//SetDlgItemText,

	ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),//GetDlgItemInt,
	ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//SetDlgItemInt,

	ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//SendItemMessage,
	ARG4_MAKE( DWORD, PTR, PTR, DWORD ),//MessageBox,
//ex
	ARG4_MAKE( DWORD, PTR, DWORD, DWORD ),//CreateIndirectEx,
	ARG5_MAKE( DWORD, PTR, DWORD, DWORD, DWORD ),//CreateIndirectParamEx,
	ARG4_MAKE( DWORD, PTR, DWORD, DWORD ),//BoxIndirectEx,
	ARG5_MAKE( DWORD, PTR, DWORD, DWORD, DWORD ),//BoxIndirectParamEx
//ex-end
    ARG1_MAKE( DWORD ),           // ShowDesktop
	ARG3_MAKE( DWORD, DWORD, DWORD ),           // SetWindowRgn
	ARG2_MAKE( DWORD, DWORD )
};

//定义系统API（窗口对象消息处理）函数界面
static const PFNVOID lpMsgAPI[] = {
	NULL,
//#define MSG_POST                  78
    (PFNVOID)WinMsg_Post,
//#define MSG_POSTTHREAD            79  
    (PFNVOID)WinMsg_PostThread,
//#define MSG_GET                   80
    (PFNVOID)WinMsg_Get,
//#define MSG_PEEK                  81
    (PFNVOID)WinMsg_Peek,
//#define MSG_POSTQUIT              82
    (PFNVOID)WinMsg_PostQuit,
//#define MSG_DISPATCH              83
    (PFNVOID)WinMsg_Dispatch,
//#define MSG_TRANSLATE             84
    (PFNVOID)NULL,//WinMsg_Translate,
//#define MSG_SEND                  85
    (PFNVOID)WinMsg_Send,
//#define MSG_WAIT                  86
    (PFNVOID)WinMsg_Wait,
//#define MQ_FREETHREADQUEUE
//	(PFNVOID)MQ_FreeThreadQueue
    (PFNVOID)WinEvent_MouseEvent,
	(PFNVOID)WinEvent_KeybdEvent,
	(PFNVOID)WinMsg_WaitForMultipleObjects,
	(PFNVOID)WinMsg_Beep,
	(PFNVOID)WinMsg_GetKeyState,
	(PFNVOID)WinMsg_SendTimeout
};

//定义系统API（窗口对象消息处理）参数界面
static const DWORD dwMsgArgs[] = {
	NULL,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//WinMsg_Post,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//PostThread,
    ARG4_MAKE( PTR, DWORD, DWORD, DWORD ),//Get,
    ARG5_MAKE( PTR, DWORD, DWORD, DWORD, DWORD ),//Peek,
    ARG1_MAKE( DWORD ),//PostQuit,
    ARG1_MAKE( PTR ),//Dispatch,
    ARG1_MAKE( PTR ),//Translate,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//Send,
    ARG0_MAKE(),//Wait,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//MouseEvent,
	ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//KeybdEvent
	ARG5_MAKE( DWORD, PTR, DWORD, DWORD, DWORD ), //WinMsg_WaitForMultipleObjects
	ARG1_MAKE( DWORD ), //WinMsg_Beep
	ARG1_MAKE( DWORD ), // WinMsg_GetKeyState
	ARG7_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, PTR )
};

static const SERVER_CTRL_INFO sci = { sizeof( SERVER_CTRL_INFO ),
                                      0,
									  GwmeServerHandler };

//定义系统API（GWME 系统管理/对象）函数界面
static const PFNVOID lpGwmeAPI[] = {
	(PFNVOID)&sci,//GwmeServerHandler,//NULL,
    (PFNVOID)WinSys_CloseObject,
//#define SYS_GETMETRICS            87
    (PFNVOID)WinSys_GetMetrics,
//#define SYS_SETMETRICS            
    (PFNVOID)WinSys_SetMetrics,

//#define SYS_GETCOLOR              88
    (PFNVOID)WinSys_GetColor,
//#define SYS_SETCOLORS             89
    (PFNVOID)WinSys_SetColors,
//#define SYS_GETCOLORBRUSH         90
    (PFNVOID)WinSys_GetColorBrush,
	(PFNVOID)NULL,//WinSys_TouchCalibrate,
//#define CARET_CREATE              91
    (PFNVOID)WinCaret_Create,          
//#define CARET_DESTROY             92
    (PFNVOID)WinCaret_Destroy,
//#define CARET_GETBLINKTIME        93
    (PFNVOID)WinCaret_GetBlinkTime,
//#define CARET_GETPOS              94
    (PFNVOID)WinCaret_GetPos,
//#define CARET_HIDE                95
    (PFNVOID)WinCaret_Hide,
//#define CARET_SETBLINKTIME        96
    (PFNVOID)WinCaret_SetBlinkTime,
//#define CARET_SETPOS              97
    (PFNVOID)WinCaret_SetPos,
//#define CARET_SHOW                98
    (PFNVOID)WinCaret_Show,
//////////menu
	(PFNVOID)NULL,//WinMenu_Create,
	(PFNVOID)NULL,//WinMenu_CreatePopup,
	(PFNVOID)NULL,//WinMenu_Destroy,
	(PFNVOID)NULL,//WinMenu_Remove,
	(PFNVOID)NULL,//WinMenu_Delete,
	(PFNVOID)NULL,//WinMenu_Insert,
	(PFNVOID)NULL,//WinMenu_InsertItem,
	(PFNVOID)NULL,//WinMenu_EnableItem,
	(PFNVOID)NULL,//WinMenu_GetDefaultItem,
	(PFNVOID)NULL,//WinMenu_SetDefaultItem,
	(PFNVOID)NULL,//WinMenu_SetItemInfo,
	(PFNVOID)NULL,//WinMenu_GetItemCount,
	(PFNVOID)NULL,//WinMenu_GetItemID,
	(PFNVOID)NULL,//WinMenu_GetItemInfo,
	(PFNVOID)NULL,//WinMenu_GetSub,
	(PFNVOID)NULL,//WinMenu_TrackPopup,
// code page
//  这两个函数以放到 apisrv里，因为是device等进程也会用道
    NULL,//(PFNVOID)WinCP_MultiByteToWideChar,
	NULL,//(PFNVOID)WinCP_WideCharToMultiByte
	(PFNVOID)WinSys_SetCalibrateWindow,
	(PFNVOID)WinSys_SetCalibratePoints
};

//定义系统API（GWME 系统管理/对象）参数界面
static const DWORD dwGwmeArgs[] = {
	NULL,
    ARG1_MAKE( DWORD ),//CloseObject,
    ARG1_MAKE( DWORD ),//GetMetrics,
	ARG2_MAKE( DWORD, DWORD ), //SetMetrics
    ARG1_MAKE( DWORD ),//GetColor,
    ARG3_MAKE( DWORD, PTR, PTR ),//SetColors,
    ARG1_MAKE( DWORD ),//GetColorBrush,
	ARG0_MAKE(),  //TouchCalibrate
	// caret
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//WinCaret_Create,          
    ARG0_MAKE(),//WinCaret_Destroy,
    ARG0_MAKE(),//WinCaret_GetBlinkTime,
    ARG1_MAKE( PTR ),//WinCaret_GetPos,
    ARG1_MAKE( DWORD ),//WinCaret_Hide,
    ARG1_MAKE( DWORD ),//WinCaret_SetBlinkTime,
    ARG2_MAKE( DWORD, DWORD ),//WinCaret_SetPos,
    ARG1_MAKE( DWORD ),//WinCaret_Show,
//////////menu
	ARG0_MAKE(),//WinMenu_Create,
	ARG0_MAKE(),//WinMenu_CreatePopup,
	ARG1_MAKE( DWORD ),//WinMenu_Destroy,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//WinMenu_Remove,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//WinMenu_Delete,
	ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, PTR ),//WinMenu_Insert,
	ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinMenu_InsertItem,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//WinMenu_EnableItem,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//WinMenu_GetDefaultItem,
	ARG3_MAKE( DWORD, DWORD, DWORD ),//WinMenu_SetDefaultItem,
	ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinMenu_SetItemInfo,
	ARG1_MAKE( DWORD ),//WinMenu_GetItemCount,
	ARG2_MAKE( DWORD, DWORD ),//WinMenu_GetItemID,
	ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinMenu_GetItemInfo,
	ARG2_MAKE( DWORD, DWORD ),//WinMenu_GetSub,
	ARG7_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, PTR ),//WinMenu_TrackPopup
	// code page
    0, //ARG6_MAKE( DWORD, DWORD, PTR, DWORD, PTR, DWORD ),//MultiByteToWideChar,
	0, //ARG8_MAKE( DWORD, DWORD, PTR, DWORD, PTR, DWORD, PTR, PTR ),//WideCharToMultiByte
	ARG1_MAKE(DWORD),
	ARG5_MAKE( DWORD, PTR, PTR, PTR, PTR )
};

//定义系统API（图形设备）函数界面
static const PFNVOID lpGdiAPI[] = {
    NULL,
///////////////////////////////////////
//#define GDI_BITBLT                
    (PFNVOID)WinGdi_BitBlt,           
//#define GDI_PATBLT                
    (PFNVOID)WinGdi_PatBlt,
//#define GDI_MASKBLT        
    (PFNVOID)WinGdi_MaskBlt,
//#define GDI_DRAWEDGE         
    (PFNVOID)WinGdi_DrawEdge,
//#define GDI_DRAWICON         
    (PFNVOID)WinGdi_DrawIcon,
//#define GDI_DRAWICONEX       
    (PFNVOID)WinGdi_DrawIconEx,
//#define GDI_DRAWTEXT             
    (PFNVOID)WinGdi_DrawText,
//#define GDI_DRAWFOCUSRECT     
    (PFNVOID)WinGdi_DrawFocusRect,
//#define GDI_DPTOLP             
    (PFNVOID)WinGdi_DPtoLP,
//#define GDI_ELLIPSE
	(PFNVOID)WinGdi_Ellipse,
//#define GDI_EXTTEXTOUT          
    (PFNVOID)WinGdi_ExtTextOut,
//#define GDI_EXCLUDECLIPRECT     
    (PFNVOID)WinGdi_ExcludeClipRect,
//#define GDI_FILLRECT            110
    (PFNVOID)WinGdi_FillRect,
//#define GDI_FILLRGN            
    (PFNVOID)WinGdi_FillRgn,
//#define GDI_INTERSECTCLIPRECT        
    (PFNVOID)WinGdi_IntersectClipRect,
//#define GDI_GETBKCOLOR               
    (PFNVOID)WinGdi_GetBkColor,
//#define GDI_GETBKMODE                
    (PFNVOID)WinGdi_GetBkMode,
//#define GDI_GETCURRENTPOSITION       
    (PFNVOID)WinGdi_GetCurrentPosition,
//#define GDI_GETCLIPBOX               
    (PFNVOID)WinGdi_GetClipBox,
//#define GDI_GETCLIPRGN               
    (PFNVOID)WinGdi_GetClipRgn,
//#define GDI_GETCURRENTOBJECT         
    (PFNVOID)WinGdi_GetCurrentObject,
//#define GDI_GETDIBITS               
    (PFNVOID)WinGdi_GetDIBits,
//#define GDI_CREATEICONINDIRECT      120  
    (PFNVOID)WinGdi_CreateIconIndirect,
//#define GDI_DESTROYICON                  
    (PFNVOID)WinGdi_DestroyIcon,
//#define GDI_DESTROYCURSOR              
    (PFNVOID)WinGdi_DestroyCursor,
//#define GDI_GETICONINFO                
    (PFNVOID)WinGdi_GetIconInfo,
//#define GDI_GETOBJECTTYPE              
    (PFNVOID)WinGdi_GetObjectType,
//#define GDI_GETOBJECTINFO              
    (PFNVOID)WinGdi_GetObjectInfo,
//#define GDI_GETPIXEL                   
    (PFNVOID)WinGdi_GetPixel,
//#define GDI_GETROP2                    
    (PFNVOID)WinGdi_GetROP2,
//#define GDI_GETTEXTCOLOR               
    (PFNVOID)WinGdi_GetTextColor,
//#define GDI_GETTEXTALIGN               
    (PFNVOID)WinGdi_GetTextAlign,
//#define GDI_GETTEXTEXTENTPOINT32         
    (PFNVOID)WinGdi_GetTextExtentPoint32,
//#define GDI_GETTEXTEXTENTEXPOINT      130
    (PFNVOID)WinGdi_GetTextExtentExPoint,
//#define GDI_GETVIEWPORTORG             
    (PFNVOID)WinGdi_GetViewportOrg,
//#define GDI_GETWINDOWORG               
    (PFNVOID)WinGdi_GetWindowOrg,
//#define GDI_INVERTRECT                 
    (PFNVOID)WinGdi_InvertRect,
//#define GDI_INVERTRGN                  
    (PFNVOID)WinGdi_InvertRgn,
//#define GDI_LINETO                     
    (PFNVOID)WinGdi_LineTo,
//#define GDI_LINE                       
    (PFNVOID)WinGdi_Line,
//#define GDI_LPTODP                     
    (PFNVOID)WinGdi_LPtoDP,
//#define GDI_MOVETO                     
    (PFNVOID)WinGdi_MoveTo,
//#define GDI_OFFSETCLIPRGN              
    (PFNVOID)WinGdi_OffsetClipRgn,
//#define GDI_OFFSETVIEWPORTORG          140
    (PFNVOID)WinGdi_OffsetViewportOrg,
//#define GDI_OFFSETWINDOWORG            
    (PFNVOID)WinGdi_OffsetWindowOrg,
//#define GDI_POLYLINE                   
    (PFNVOID)WinGdi_Polyline,
//#define GDI_PTVISIBLE                  
    (PFNVOID)WinGdi_PtVisible,
//#define GDI_RECTANGLE                  
    (PFNVOID)WinGdi_Rectangle,
//#define GDI_ROUNDRECT
	(PFNVOID)WinGdi_RoundRect,
//#define GDI_RECTVISIBLE                
    (PFNVOID)WinGdi_RectVisible,
//#define GDI_SELECTOBJECT               
    (PFNVOID)WinGdi_SelectObject,
//#define GDI_DELETEOBJECT               
    (PFNVOID)WinGdi_DeleteObject,
//#define GDI_DELETEDC                   
    (PFNVOID)WinGdi_DeleteDC,
//#define GDI_SELECTCLIPRGN              
    (PFNVOID)WinGdi_SelectClipRgn,
//#define GDI_EXTSELECTCLIPRGN           150
    (PFNVOID)WinGdi_ExtSelectClipRgn,
//#define GDI_SETBKCOLOR                 
    (PFNVOID)WinGdi_SetBkColor,
//#define GDI_SETBKMODE                  
    (PFNVOID)WinGdi_SetBkMode,
//#define GDI_SETBRUSHORG                
    (PFNVOID)WinGdi_SetBrushOrg,
//#define GDI_SETMAPMODE                 
    (PFNVOID)WinGdi_SetMapMode,
//#define GDI_SETPIXEL                   
    (PFNVOID)WinGdi_SetPixel,
//#define GDI_SETROP2                    
    (PFNVOID)WinGdi_SetROP2,
//#define GDI_SETTEXTCOLOR               
    (PFNVOID)WinGdi_SetTextColor,
//#define GDI_SETTEXTALIGN
    (PFNVOID)WinGdi_SetTextAlign,
//#define GDI_SETVIEWPORTORG             
    (PFNVOID)WinGdi_SetViewportOrg,
//#define GDI_SETWINDOWORG               
    (PFNVOID)WinGdi_SetWindowOrg,
//#define GDI_TEXTOUT                    160
    (PFNVOID)WinGdi_TextOut,
//#define GDI_CREATEPENINDIRECT          
    (PFNVOID)WinGdi_CreatePenIndirect,
//#define GDI_CREATEPEN                  
    (PFNVOID)WinGdi_CreatePen,
//#define GDI_CREATEBRUSHINDIRECT       
    (PFNVOID)WinGdi_CreateBrushIndirect,
//#define GDI_CREATEHATCHBRUSH          
    (PFNVOID)WinGdi_CreateHatchBrush,
//#define GDI_CREATESOLIDBRUSH          
    (PFNVOID)WinGdi_CreateSolidBrush,
//#define GDI_CREATEBITMAP              
    (PFNVOID)WinGdi_CreateBitmap,
//#define GDI_CREATECOMPATIBLEBITMAP    
    (PFNVOID)WinGdi_CreateCompatibleBitmap,
//#define GDI_CREATEBITMAPINDIRECT      
    NULL,//(PFNVOID)WinGdi_CreateBitmapIndirect,
//#define GDI_CREATECOMPATIBLEDC        
    (PFNVOID)WinGdi_CreateCompatibleDC,
//#define GDI_CREATEDIBSECTION          170
    (PFNVOID)WinGdi_CreateDIBSection,
//#define GDI_SETDIBITS                 
    (PFNVOID)WinGdi_SetDIBits,
//#define GDI_CREATEDIBITMAP           
    (PFNVOID)WinGdi_CreateDIBitmap,
//#define GDI_GETSTOCKOBJECT            
    (PFNVOID)WinGdi_GetStockObject,
//#define GDI_GETTEXTMETRICS            
    (PFNVOID)WinGdi_GetTextMetrics,
//#define GDI_GETCHARWIDTH              
    (PFNVOID)WinGdi_GetCharWidth,
//#define GDI_LOADIMAGE                 
    (PFNVOID)WinGdi_LoadImage,
//#define GDI_LOADICON                  
    (PFNVOID)WinGdi_LoadIcon,
//#define GDI_LOADCURSOR              168     
    (PFNVOID)WinGdi_LoadCursor,      

//#define RGN_CREATERECT              169
    (PFNVOID)WinRgn_CreateRect,
//#define RGN_CREATERECTINDIRECT      170
    (PFNVOID)WinRgn_CreateRectIndirect,
//#define RGN_COMBINE                 171
    (PFNVOID)WinRgn_Combine,
//#define RGN_EQUAL                   172
    (PFNVOID)WinRgn_Equal,
//#define RGN_OFFSET                  173
    (PFNVOID)WinRgn_Offset,
//#define RGN_GETBOX                  174
    (PFNVOID)WinRgn_GetBox,
//#define RGN_RECTINREGION            175
    (PFNVOID)WinRgn_RectInRegion,
//#define RGN_PTINREGION              176
    (PFNVOID)WinRgn_PtInRegion,
//#define RGN_SETRECT                 177
    (PFNVOID)WinRgn_SetRect,
//
   	(PFNVOID)WinGdi_Arc,
	(PFNVOID)WinGdi_SetDIBitsToDevice,
	(PFNVOID)WinGdi_GetMapMode,
	(PFNVOID)WinGdi_TextOutW,
    (PFNVOID)WinGdi_GetTextExtentPoint32W,
	(PFNVOID)WinGdi_CreateFontIndirect,
	(PFNVOID)WinGdi_TransparentBlt,
	(PFNVOID)WinGdi_StretchBlt,
	(PFNVOID)WinGdi_WindowFromDC,
	(PFNVOID)WinGdi_SetDIBColorTable,
	(PFNVOID)WinGdi_GetDeviceCaps,
	(PFNVOID)WinGdi_AlphaBlendEx,
	(PFNVOID)WinGdi_LoadString
};

//定义系统API（图形设备）参数界面
static const DWORD dwGdiArgs[] = {
    NULL,
///////////////////////////////////////
    ARG9_MAKE( DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD ),//WinGdi_BitBlt,           
    ARG6_MAKE( DWORD,DWORD,DWORD,DWORD,DWORD,DWORD ),//WinGdi_PatBlt,
    ARG12_MAKE( DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD ),//WinGdi_MaskBlt,
    ARG4_MAKE( DWORD, PTR, DWORD, DWORD ),//WinGdi_DrawEdge,
    ARG4_MAKE( DWORD,DWORD,DWORD,DWORD ),//WinGdi_DrawIcon,
    ARG9_MAKE( DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD,DWORD ),//WinGdi_DrawIconEx,
    ARG5_MAKE( DWORD, PTR, DWORD, PTR, DWORD ),//WinGdi_DrawText,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_DrawFocusRect,
    ARG3_MAKE( DWORD, PTR, DWORD ),//WinGdi_DPtoLP,
	ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),
    ARG8_MAKE( DWORD, DWORD, DWORD, DWORD, PTR, PTR, DWORD, PTR ),//WinGdi_ExtTextOut,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_ExcludeClipRect,
    ARG3_MAKE( DWORD, PTR, DWORD ),//WinGdi_FillRect,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_FillRgn,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_IntersectClipRect,
    ARG1_MAKE( DWORD ),//WinGdi_GetBkColor,
    ARG1_MAKE( DWORD ),//WinGdi_GetBkMode,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_GetCurrentPosition,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_GetClipBox,
    ARG2_MAKE( DWORD, DWORD  ),//WinGdi_GetClipRgn,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_GetCurrentObject,
    ARG7_MAKE( DWORD, DWORD, DWORD, DWORD, PTR, PTR, DWORD ),//(PFNVOID)WinGdi_GetDIBits,
    ARG1_MAKE( PTR ),//WinGdi_CreateIconIndirect,
    ARG1_MAKE( DWORD ),//WinGdi_DestroyIcon,
    ARG1_MAKE( DWORD ),//WinGdi_DestroyCursor,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_GetIconInfo,
    ARG1_MAKE( DWORD ),//WinGdi_GetObjectType,
    ARG3_MAKE( DWORD, DWORD, PTR ),//WinGdi_GetObjectInfo,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_GetPixel,
    ARG1_MAKE( DWORD ),//WinGdi_GetROP2,
    ARG1_MAKE( DWORD ),//WinGdi_GetTextColor,
	ARG1_MAKE( DWORD ),//WinGdi_GetTextAlign,
    ARG4_MAKE( DWORD, PTR, DWORD, PTR ),//WinGdi_GetTextExtentPoint32,
    ARG7_MAKE( DWORD, PTR, DWORD, DWORD, PTR, PTR, PTR ),//WinGdi_GetTextExtentExPoint,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_GetViewportOrg,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_GetWindowOrg,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_InvertRect,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_InvertRgn,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_LineTo,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_Line,
    ARG3_MAKE( DWORD, PTR, DWORD ),//WinGdi_LPtoDP,
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_MoveTo,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_OffsetClipRgn,
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_OffsetViewportOrg,
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_OffsetWindowOrg,
    ARG3_MAKE( DWORD, PTR, DWORD ),//WinGdi_Polyline,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_PtVisible,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_Rectangle,
	ARG7_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_RoundRect,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_RectVisible,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SelectObject,
    ARG1_MAKE( DWORD ),//WinGdi_DeleteObject,
    ARG1_MAKE( DWORD ),//WinGdi_DeleteDC,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SelectClipRgn,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_ExtSelectClipRgn,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SetBkColor,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SetBkMode,
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_SetBrushOrg,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SetMapMode,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//WinGdi_SetPixel,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SetROP2,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_SetTextColor,
	ARG2_MAKE( DWORD, DWORD ),//WinGdi_SetTextAlign
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_SetViewportOrg,
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_SetWindowOrg,
    ARG5_MAKE( DWORD, DWORD, DWORD, PTR, DWORD ),//WinGdi_TextOut,
    ARG1_MAKE( PTR ),//WinGdi_CreatePenIndirect,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_CreatePen,
    ARG1_MAKE( PTR ),//WinGdi_CreateBrushIndirect,
    ARG2_MAKE( DWORD, DWORD ),//WinGdi_CreateHatchBrush,
    ARG1_MAKE( DWORD ),//WinGdi_CreateSolidBrush,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, PTR ),//WinGdi_CreateBitmap,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinGdi_CreateCompatibleBitmap,
    0,//(PFNVOID)WinGdi_CreateBitmapIndirect,
    ARG1_MAKE( DWORD ),//WinGdi_CreateCompatibleDC,
    ARG6_MAKE( DWORD, PTR, DWORD, PTR, DWORD, DWORD ),//WinGdi_CreateDIBSection,
    ARG7_MAKE( DWORD, DWORD, DWORD, DWORD, PTR, PTR, DWORD ),//WinGdi_SetDIBits,
    ARG6_MAKE( DWORD, PTR, DWORD, PTR, PTR, DWORD ),//WinGdi_CreateDIBitmap,
    ARG1_MAKE( DWORD ),//WinGdi_GetStockObject,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_GetTextMetrics,
    ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),//WinGdi_GetCharWidth,
    ARG6_MAKE( DWORD, PTR, DWORD, DWORD, DWORD, DWORD ),//WinGdi_LoadImage,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_LoadIcon,
    ARG2_MAKE( DWORD, PTR ),//WinGdi_LoadCursor,   
	// region
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//WinRgn_CreateRect,
    ARG1_MAKE( PTR ),//WinRgn_CreateRectIndirect,
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//WinRgn_Combine,
    ARG2_MAKE( DWORD, DWORD ),//WinRgn_Equal,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinRgn_Offset,
    ARG2_MAKE( DWORD, PTR ),//WinRgn_GetBox,
    ARG2_MAKE( DWORD, PTR ),//WinRgn_RectInRegion,
    ARG3_MAKE( DWORD, DWORD, DWORD ),//WinRgn_PtInRegion,
    ARG5_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD ),//WinRgn_SetRect,
	//
	ARG9_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_Arc
	ARG12_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, PTR, PTR, DWORD ),//WinGdi_SetDIBitsToDevice
	ARG1_MAKE( DWORD ),//WinGdi_GetMapMode
	ARG5_MAKE( DWORD, DWORD, DWORD, PTR, DWORD ),//WinGdi_TextOutW
	ARG4_MAKE( DWORD, PTR, DWORD, PTR ),//WinGdi_GetTextExtentPoint32W,
	ARG1_MAKE( PTR ),    //WinFont_CreateIndirect
	ARG11_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_TransparentBlt
	ARG11_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD ),//WinGdi_StretchBlt
	ARG1_MAKE( DWORD ), //WinGdi_WindowFromDC
	ARG4_MAKE( DWORD, DWORD, DWORD, PTR ),  //WinGdi_SetDIBColorTable
	ARG2_MAKE( DWORD, DWORD ),    //WinGdi_GetDeviceCaps
	ARG11_MAKE( DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, PTR ),//WinGdi_AlphaBlend
	ARG4_MAKE( DWORD, DWORD, PTR, DWORD ),  //WinGdi_LoadString
};

// **************************************************
// 声明：void InstallGwmeServer( void )
// 参数：
// 	无
// 返回值：
//	无
// 功能描述：
//	初始化 GWME 系统
// 引用: 
//	
// ************************************************

void InstallGwmeServer( void )
{	//注册 GWME 系统 API
	API_RegisterEx( API_GDI,  lpGdiAPI, dwGdiArgs, sizeof( lpGdiAPI ) / sizeof( PFNVOID ) );
    API_RegisterEx( API_WND,  lpWndAPI, dwWndArgs, sizeof( lpWndAPI ) / sizeof( PFNVOID ) );
    API_RegisterEx( API_MSG,  lpMsgAPI, dwMsgArgs, sizeof( lpMsgAPI ) / sizeof( PFNVOID ) );
    API_RegisterEx( API_GWE,  lpGwmeAPI, dwGwmeArgs, sizeof( lpGwmeAPI ) / sizeof( PFNVOID ) );
}


