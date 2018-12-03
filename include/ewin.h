/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EWIN_H
#define __EWIN_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

//    define class
typedef struct _WNDCLASS 
{
    DWORD    style; 
    WNDPROC lpfnWndProc; 
    int     cbClsExtra; 
    int     cbWndExtra; 
    HANDLE  hInstance; 
    HICON   hIcon; 
    HCURSOR hCursor; 
    HBRUSH  hbrBackground; 
    LPCTSTR lpszMenuName; 
    LPCTSTR lpszClassName; 
} WNDCLASS, * PWNDCLASS, FAR *LPWNDCLASS;
typedef const WNDCLASS FAR* LPCWNDCLASS;

typedef struct _CREATESTRUCT
{
    LPVOID    lpCreateParams;
    HINSTANCE hInstance;
    HMENU     hMenu;
    HWND      hParent;
    int       cy;
    int       cx;
    int       y;
    int       x;
    DWORD     style;
    LPCSTR    lpszClass;
    LPCSTR    lpszName;
    DWORD     dwExStyle;
} CREATESTRUCT, * PCREATESTRUCT, FAR * LPCREATESTRUCT;

typedef struct _WINDOWPOS 
{
    HWND    hwnd;
    HWND    hwndInsertAfter;
    int     x;
    int     y;
    int     cx;
    int     cy;
    ULONG   flags;
} WINDOWPOS, * PWINDOWPOS, FAR * LPWINDOWPOS;

// WM_NCCALCSIZE parameter structure

typedef struct _NCCALCSIZE_PARAMS 
{
    RECT       rgrc[3];
    PWINDOWPOS lppos;
} NCCALCSIZE_PARAMS, * PNCCALCSIZE_PARAMS, FAR * LPNCCALCSIZE_PARAMS;

typedef struct _PAINTSTRUCT 
{
    HDC         hdc;
    BOOL        fErase;
    RECT        rcPaint;
    BOOL        fRestore;
    BOOL        fIncUpdate;
} PAINTSTRUCT, * PPAINTSTRUCT, FAR *LPPAINTSTRUCT;

typedef struct _NMHDR
{
    HWND  hwndFrom;
    UINT  idFrom;
    UINT  code;         
}NMHDR, * PNMHDR, FAR * LPNMHDR;

// define class style
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_DBLCLKS          0x0008
#define CS_CLASSDC          0x0040
#define CS_PARENTDC         0x0080

// system define class name
extern const char classBUTTON[];
extern const char classSTATIC[];
extern const char classDIALOG[];
extern const char classLISTBOX[];
extern const char classCOMBOLISTBOX[];
extern const char classSCROLLBAR[];
extern const char classSTARTBAR[];
extern const char classCOMBOBOX[];
extern const char classMULTIEDIT[];
extern const char classEDIT[];

#define RegisterClass Class_Register
ATOM WINAPI Class_Register( LPCWNDCLASS );

#define GetClassInfo  Class_GetInfo
BOOL WINAPI Class_GetInfo( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpwc );


#define UnregisterClass Class_Unregister
int WINAPI Class_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance );

#define GCL_MENUNAME        (-8)
#define GCL_HBRBACKGROUND   (-10)
#define GCL_HCURSOR         (-12)
#define GCL_HICON           (-14)
#define GCL_HMODULE         (-16)
#define GCL_CBWNDEXTRA      (-18)
#define GCL_CBCLSEXTRA      (-20)
#define GCL_WNDPROC         (-24)
#define GCL_STYLE           (-26)
#define GCW_ATOM            (-32)

#define GetClassLong  Class_GetLong
DWORD WINAPI Class_GetLong( HWND hwnd, int nIndex );

#define SetClassLong  Class_SetLong
DWORD WINAPI Class_SetLong( HWND hWnd, int nIndex, LONG lNewValue );

// basic window types
#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L

// clipping styles
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L

// main window styles
#define WS_CAPTION          0x00C00000L     // WS_BORDER | WS_DLGFRAME
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L

// control window styles
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L

// common window styles
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define WS_POPUPWINDOW      (WS_POPUP | WS_BORDER | WS_SYSMENU)
#define WS_CHILDWINDOW      (WS_CHILD)

// generic window states
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L

// main window states
#define WS_MINIMIZE         0x20000000L
#define WS_MAXIMIZE         0x01000000L

// define extend style
#define WS_EX_TOPMOST       0x00000001L
#define WS_EX_CLIENTEDGE    0x00000200L


#define WS_EX_CLOSEBOX      0x80000000L
#define WS_EX_OKBOX         0x40000000L
#define WS_EX_HELPBOX       0x20000000L
#define WS_EX_NOMOVE        0x10000000L
#define WS_EX_INPUTWINDOW   0x08000000L
#define WS_EX_TITLE         0x04000000L

#define WS_EX_NOFOCUS       0x02000000L

#define WS_EX_LAYERED       0x00008000L

// special value for createWindow
#define HWND_DESKTOP        ((HWND)0)

#define CreateWindowEx Wnd_CreateEx
HWND WINAPI Wnd_CreateEx(   
					 DWORD dwExStyle,
                     LPCTSTR lpcClassName,
		             LPCTSTR lpcText,
		             DWORD dwMainStyle,
		             int x, int y, int dx, int dy,
		             HWND hParent,
		             HMENU hMenu,
		             HINSTANCE hInstance,
		             LPVOID lpCreateParam );
#define CreateWindow( lpClassName, lpText, dwMainStyle, x, y, dx, dy,\
hParent, hMenu, hInstance, lpCreateParam )\
Wnd_CreateEx( 0, lpClassName, lpText, dwMainStyle, x, y, dx, dy,\
hParent, hMenu, (HINSTANCE)(hInstance), lpCreateParam )

#define IsWindow Wnd_IsWindow
BOOL WINAPI Wnd_IsWindow( HWND );

#define ShowScrollBar Wnd_ShowScrollBar
BOOL WINAPI Wnd_ShowScrollBar( HWND hWnd, int fnBar, BOOL bShow );

#define DefWindowProc Wnd_DefProc
LRESULT WINAPI Wnd_DefProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define DestroyWindow Wnd_Destroy
BOOL WINAPI Wnd_Destroy( HWND );

#define IsChild  Wnd_IsChild
BOOL WINAPI Wnd_IsChild( HWND hwndParent, HWND hwndChild );

#define GetParent Wnd_GetParent
HWND WINAPI Wnd_GetParent( HWND hWnd );

#define SetParent Wnd_SetParent
HWND WINAPI Wnd_SetParent( HWND hwndChild, HWND hwndParent );

#define IsWindowVisible Wnd_IsVisible
BOOL WINAPI Wnd_IsVisible( HWND hWnd );

#define ShowDesktop Wnd_ShowDesktop
// uiFlags 是保留为将来， 设置为0
BOOL WINAPI Wnd_ShowDesktop( UINT uiFlags );

// Wnd_Show nCmdShow argument
#define SW_HIDE             0
#define SW_SHOW             1
#define SW_SHOWNORMAL       2
#define SW_RESTORE          9

#define ShowWindow Wnd_Show
BOOL WINAPI Wnd_Show(HWND hWnd, int nCmdShow);

#define EnableWindow Wnd_Enable
BOOL WINAPI Wnd_Enable( HWND hWnd, BOOL bEnable );

#define IsWindowEnabled Wnd_IsEnabled
BOOL WINAPI Wnd_IsEnabled( HWND hWnd );

#define SetWindowText Wnd_SetText
BOOL WINAPI Wnd_SetText(HWND hWnd, LPCSTR lpcstr);

#define GetWindowText Wnd_GetText
int WINAPI Wnd_GetText(HWND hWnd, LPSTR lpstr, int nMax);

#define GetWindowTextLength Wnd_GetTextLength
int WINAPI Wnd_GetTextLength(HWND hWnd);

#define GetClientRect Wnd_GetClientRect
BOOL WINAPI Wnd_GetClientRect( HWND hWnd, LPRECT lprc );

#define GetWindowRect Wnd_GetWindowRect
BOOL WINAPI Wnd_GetWindowRect( HWND hWnd, LPRECT lprc );

//Win_SetPos Flags
#define SWP_NOSIZE          0x0001
#define SWP_NOMOVE          0x0002
#define SWP_NOZORDER        0x0004
#define SWP_NOREDRAW        0x0008
#define SWP_NOACTIVATE      0x0010
#define SWP_FRAMECHANGED    0x0020
#define SWP_SHOWWINDOW      0x0040
#define SWP_HIDEWINDOW      0x0080
#define SWP_NOSENDCHANGING  0x0400

#define HWND_TOP        ((HWND)0)
#define HWND_BOTTOM     ((HWND)1)
#define HWND_TOPMOST    ((HWND)-1)

#define SetWindowPos Wnd_SetPos
BOOL WINAPI Wnd_SetPos( HWND hWnd, HWND hwndAfter, int x, int y, int width, int height, UINT uFlags );

#define MoveWindow Wnd_Move
BOOL WINAPI Wnd_Move( HWND hWnd, int x, int y, int width, int height, BOOL bRepaint );

#define BringWindowToTop Wnd_BringToTop
BOOL WINAPI Wnd_BringToTop( HWND hWnd);

#define CallWindowProc Wnd_CallProc
LRESULT WINAPI Wnd_CallProc( WNDPROC lpwProc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define AdjustWindowRectEx Wnd_AdjustRectEx
BOOL WINAPI Wnd_AdjustRectEx( LPRECT lprc, DWORD dwMainStyle, BOOL fMenu, DWORD dwExStyle );

#define FlashWindow Wnd_Flash
BOOL WINAPI Wnd_Flash(HWND hWnd, BOOL );

#define CloseWindow Wnd_Close
BOOL WINAPI Wnd_Close(HWND hWnd);

#define IsIconic Wnd_IsIconic
BOOL WINAPI Wnd_IsIconic(HWND hWnd);

#define IsZoomed Wnd_IsZoomed
BOOL WINAPI Wnd_IsZoomed(HWND hWnd);

// Wnd_GetLong index
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)
#define GWL_MAINSTYLE       GWL_STYLE
#define GWL_SUBSTYLE        GWL_EXSTYLE
#define GWL_VSCROLL         (-22)
#define GWL_HSCROLL         (-24)
#define GWL_HICONSM         (-26)
#define GWL_HICON           (-28)
#define GWL_TEXT_PTR        (-30)

#define GetWindowLong Wnd_GetLong
LONG WINAPI Wnd_GetLong( HWND, int nIndex );

#define GetClassName Wnd_GetClassName
int WINAPI Wnd_GetClassName( HWND hWnd, LPTSTR lpszClassName, int nMaxCount );

typedef struct _STYLESTRUCT
{
    DWORD styleOld; 
    DWORD styleNew; 
}STYLESTRUCT, * LPSTYLESTRUCT;
 
#define SetWindowLong Wnd_SetLong
LONG WINAPI Wnd_SetLong( HWND hWnd, int nIndex, LONG dwNewLong );

#define GetDesktopWindow Wnd_GetDesktop
HWND WINAPI Wnd_GetDesktop(void);

#define FindWindow Wnd_Find
HWND WINAPI Wnd_Find( LPCSTR lpcClassName, LPCSTR lpcWindowName );

#define UpdateWindow Wnd_Update
BOOL WINAPI Wnd_Update( HWND hWnd );

#define GetUpdateRect Wnd_GetUpdateRect
BOOL WINAPI Wnd_GetUpdateRect( HWND hWnd, LPRECT lprc, BOOL bErase );

#define GetUpdateRgn Wnd_GetUpdateRgn
int WINAPI Wnd_GetUpdateRgn( HWND hWnd, HRGN hrgn, BOOL bErase );

typedef BOOL (CALLBACK* WNDENUMPROC)(HWND hWnd, LPARAM lParam);
#define EnumWindows Wnd_Enum
BOOL WINAPI Wnd_Enum( WNDENUMPROC lpProc, LPARAM lParam );

#define EnumChildWindows Wnd_EnumChild
BOOL WINAPI Wnd_EnumChild( HWND hwndParent, WNDENUMPROC lpProc, LPARAM lParam );

#define GetTopWindow Wnd_GetTop
HWND WINAPI Wnd_GetTop(HWND);

#define GW_HWNDFIRST    0
#define GW_HWNDLAST     1
#define GW_HWNDNEXT     2
#define GW_HWNDPREV     3
#define GW_OWNER        4
#define GW_CHILD        5

#define GetWindow Wnd_Get
HWND WINAPI Wnd_Get(HWND hWnd, UINT uCmd);

#define GetNextWindow Wnd_GetNext
HWND WINAPI Wnd_GetNext(HWND hWnd, UINT uCmd );

#define ClientToScreen Wnd_ClientToScreen
BOOL WINAPI Wnd_ClientToScreen( HWND hWnd, LPPOINT lppt );

#define ScreenToClient Wnd_ScreenToClient
BOOL WINAPI Wnd_ScreenToClient( HWND hWnd, LPPOINT lppt );

#define MapWindowPoints Wnd_MapPoints
DWORD WINAPI Wnd_MapPoints(HWND hwndFrom, HWND hwndTo, LPPOINT lppt, UINT cpt);

#define WindowFromPoint Wnd_FromPoint
HWND WINAPI Wnd_FromPoint(POINT pt);

#define CWP_ALL             0x0000
#define CWP_SKIPINVISIBLE   0x0001
#define CWP_SKIPDISABLED    0x0002
#define CWP_SKIPTRANSPARENT 0x0004

#define ChildWindowFromPoint( hWnd, pt ) Wnd_ChildFromPoint( (hWnd), (pt), CWP_ALL )
#define ChildWindowFromPointEx Wnd_ChildFromPoint
HWND WINAPI Wnd_ChildFromPoint(HWND hWnd, POINT pt, UINT uFlags);

#define GetForegroundWindow Wnd_GetForeground
HWND WINAPI Wnd_GetForeground(void);

#define SetForegroundWindow Wnd_SetForeground
BOOL WINAPI Wnd_SetForeground(HWND hWnd );

#define SetActiveWindow Wnd_SetActive
HWND WINAPI Wnd_SetActive(HWND hWnd);

#define SetTimer Wnd_SetTimer
UINT WINAPI Wnd_SetTimer(HWND hWnd, UINT uID, UINT uElapse, TIMERPROC lpTimerProc);

#define KillTimer Wnd_KillTimer
BOOL WINAPI Wnd_KillTimer(HWND hWnd, UINT uID);

#define SetSysTimer Wnd_SetSysTimer
UINT WINAPI Wnd_SetSysTimer(HWND hWnd, UINT uID, UINT uElapse, TIMERPROC lpTimerProc);

#define KillSysTimer Wnd_KillSysTimer
BOOL WINAPI Wnd_KillSysTimer(HWND hWnd, UINT uID);

#define GetActiveWindow Wnd_GetActive
HWND WINAPI Wnd_GetActive(void);

#define GetTopLevelParent Wnd_GetTopLevelParent
HWND WINAPI Wnd_GetTopLevelParent( HWND hWnd );

#define SetFocus Wnd_SetFocus
HWND WINAPI Wnd_SetFocus(HWND hWnd);

#define GetFocus Wnd_GetFocus
HWND WINAPI Wnd_GetFocus(void);

#define SetCapture Wnd_SetCapture
HWND WINAPI Wnd_SetCapture(HWND hWnd);

#define ReleaseCapture Wnd_ReleaseCapture
BOOL WINAPI Wnd_ReleaseCapture(void);

#define GetCapture Wnd_GetCapture
HWND WINAPI Wnd_GetCapture(void);

#define BeginPaint Wnd_BeginPaint
HDC WINAPI Wnd_BeginPaint( HWND hWnd, LPPAINTSTRUCT lpps );

#define EndPaint Wnd_EndPaint
BOOL WINAPI Wnd_EndPaint( HWND hWnd, const PAINTSTRUCT * lpps );

#define GetDC Wnd_GetClientDC
HDC WINAPI Wnd_GetClientDC( HWND hWnd );

#define ReleaseDC Wnd_ReleaseDC
int WINAPI Wnd_ReleaseDC( HWND hWnd, HDC hdc );

#define GetWindowDC Wnd_GetWindowDC
HDC WINAPI Wnd_GetWindowDC( HWND );

#define DC_ACTIVE   0x0001
#define DC_ICON   0x0004
#define DC_TEXT   0x0008

#define DrawCaption Wnd_DrawCaption
BOOL WINAPI Wnd_DrawCaption( HWND hwnd, HDC hdc, LPCRECT lprc, UINT uFlags );

#define SW_INVALIDATE       0x0001  // Invalidate after scrolling
#define SW_ERASE            0x0002
#define SW_SCROLLCHILDREN   0x0004

#define ScrollWindowEx Wnd_Scroll
int WINAPI Wnd_Scroll(   
					HWND hWnd,
                    int dx, int dy,
                    LPCRECT lpcScrollRect, LPCRECT lpcClipRect,
                    HRGN hrgnUpdate,
                    LPRECT lpUpdateRect,
                    UINT uFlags );

// Wnd_GetDCEx flags
#define DCX_WINDOW           0x00000001L
#define DCX_CLIPCHILDREN     0x00000008L
#define DCX_CLIPSIBLINGS     0x00000010L
#define DCX_PARENTCLIP       0x00000020L
#define DCX_EXCLUDERGN       0x00000040L
#define DCX_INTERSECTRGN     0x00000080L
#define DCX_EXCLUDEUPDATE    0x00000100L
#define DCX_INTERSECTUPDATE  0x00000200L
#define DCX_VALIDATE         0x00200000L

#define DCX_NOLAYERED        0x80000000L
//#define DCX_LAYERED          0x40000000L

#define GetDCEx Wnd_GetDCEx
HDC WINAPI Wnd_GetDCEx( HWND hwnd, HRGN hrgnClip, DWORD dwFlags );

#define InvalidateRect Wnd_InvalidateRect
BOOL WINAPI Wnd_InvalidateRect(HWND hWnd, LPCRECT lprc, BOOL bErase);

#define InvalidateRgn Wnd_InvalidateRgn
BOOL WINAPI Wnd_InvalidateRgn(HWND hWnd, HRGN hrgn, BOOL bErase);

#define ValidateRect Wnd_ValidateRect
BOOL WINAPI Wnd_ValidateRect( HWND hWnd, LPCRECT lpcrc );

#define ValidateRgn Wnd_ValidateRgn
BOOL WINAPI Wnd_ValidateRgn( HWND hWnd, HRGN hrgn );

#define GetWindowThreadProcessId Wnd_GetThreadProcessId
DWORD WINAPI Wnd_GetThreadProcessId( HWND hWnd, DWORD * lpProcessID );

#define SB_HORZ             0
#define SB_VERT             1
#define SB_CTL              2
#define SB_BOTH             3
#define SB_CORNER           4

// arrow value
#define ESB_ENABLE_BOTH     0x0000
#define ESB_DISABLE_BOTH    0x0003

#define ESB_DISABLE_LEFT    0x0001
#define ESB_DISABLE_RIGHT   0x0002

#define ESB_DISABLE_UP      0x0001
#define ESB_DISABLE_DOWN    0x0002

#define ESB_DISABLE_LTUP    ESB_DISABLE_LEFT
#define ESB_DISABLE_RTDN    ESB_DISABLE_RIGHT

#define EnableScrollBar Wnd_EnableScrollBar
BOOL WINAPI Wnd_EnableScrollBar(HWND hWnd, UINT uFlags,UINT uArrows);

// define scroll bar function
#define SIF_RANGE           0x0001
#define SIF_PAGE            0x0002
#define SIF_POS             0x0004
#define SIF_DISABLENOSCROLL 0x0008
#define SIF_TRACKPOS        0x0010
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)

typedef struct _SCROLLINFO
{
    UINT cbSize; 
    UINT fMask; 
    int  nMin; 
    int  nMax; 
    UINT nPage; 
    int  nPos; 
    int  nTrackPos; 
}SCROLLINFO, FAR *LPSCROLLINFO;
typedef const SCROLLINFO FAR * LPCSCROLLINFO;

#define GetScrollInfo Wnd_GetScrollInfo
int WINAPI Wnd_GetScrollInfo(HWND, int fnBar, LPSCROLLINFO lpsi );

#define SetScrollInfo Wnd_SetScrollInfo
int WINAPI Wnd_SetScrollInfo(HWND, int fnBar, LPCSCROLLINFO lpcsi, BOOL fRedraw);

// DRAWITEMSTRUCT for ownerdraw
typedef struct _DRAWITEMSTRUCT 
{
    UINT  CtlType; 
    UINT  CtlID; 
    UINT  itemID; 
    UINT  itemAction; 
    UINT  itemState; 
    HWND  hwndItem; 
    HDC   hDC; 
    RECT  rcItem; 
    DWORD itemData; 
} DRAWITEMSTRUCT, FAR *LPDRAWITEMSTRUCT;

//DELETEITEMSTRUCT for ownerdraw

typedef struct _DELETEITEMSTRUCT 
{
    WORD       CtlType;
    WORD       CtlID;
    WORD       itemID;
    HWND       hwndItem;
    DWORD      itemData;
}DELETEITEMSTRUCT,FAR *LPDELETEITEMSTRUCT;

// MEASUREITEMSTRUCT for ownerdraw
 
typedef struct _MEASUREITEMSTRUCT 
{
    UINT       CtlType;
    UINT       CtlID;
    UINT       itemID;
    UINT       itemWidth;
    UINT       itemHeight;
    DWORD      itemData;
}MEASUREITEMSTRUCT, FAR *LPMEASUREITEMSTRUCT;

// COMPAREITEMSTUCT for ownerdraw sorting 
typedef struct _COMPAREITEMSTRUCT 
{
    UINT        CtlType;
    UINT        CtlID;
    HWND        hwndItem;
    UINT        itemID1;
    DWORD       itemData1;
    UINT        itemID2;
    DWORD       itemData2;
    DWORD       dwLocaleId;
}COMPAREITEMSTRUCT, FAR *LPCOMPAREITEMSTRUCT;

#define ODT_MENU        1
#define ODT_LISTBOX     2
#define ODT_COMBOBOX    3
#define ODT_BUTTON      4
#define ODT_STATIC      5

//owner draw actions
#define ODA_DRAWENTIRE  0x0001
#define ODA_SELECT      0x0002
#define ODA_FOCUS       0x0004

//owner draw state

#define ODS_DEFAULT     0x0000
#define ODS_SELECTED    0x0001
#define ODS_GRAYED      0x0002
#define ODS_DISABLED    0x0004
#define ODS_CHECKED     0x0008
#define ODS_FOCUS       0x0010

// button control styles
#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX         0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE           0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX         0x00000007L
#define BS_USERBUTTON       0x00000008L
#define BS_AUTORADIOBUTTON  0x00000009L
#define BS_OWNERDRAW        0x0000000BL
#define BS_LEFTTEXT         0x00000020L

#define BS_TEXT             0x00000000L
#define BS_ICON             0x00000040L
#define BS_BITMAP           0x00000080L
#define BS_LEFT             0x00000100L
#define BS_RIGHT            0x00000200L
#define BS_CENTER           0x00000300L
#define BS_TOP              0x00000400L
#define BS_BOTTOM           0x00000800L
#define BS_VCENTER          0x00000C00L
#define BS_PUSHLIKE         0x00001000L
#define BS_MULTILINE        0x00002000L
#define BS_NOTIFY           0x00004000L


// User Button Notification Codes
#define BN_CLICKED          0
#define BN_PAINT            1
#define BN_HILITE           2
#define BN_UNHILITE         3
#define BN_DISABLE          4
#define BN_DOUBLECLICKED    5

#define BN_PUSHED           BN_HILITE
#define BN_UNPUSHED         BN_UNHILITE

#define BN_SETFOCUS         6
#define BN_KILLFOCUS        7

// Button Control Messages
#define BM_GETCHECK        0x00F0
#define BM_SETCHECK        0x00F1
#define BM_GETSTATE        0x00F2
#define BM_SETSTATE        0x00F3
#define BM_SETSTYLE        0x00F4

#define BM_SETIMAGE        0x00F5
#define BM_GETIMAGE        0x00F6
//这两个消息是Kingmos扩展的消息
//关闭或打开按钮的焦点功能
// wParam = 0 , 关闭
// wParam = 1, 打开
#define BM_ENABLEFOCUS       0x00F7
//

#define BST_UNCHECKED      0x0000
#define BST_CHECKED        0x0001
#define BST_INDETERMINATE  0x0002
#define BST_PUSHED         0x0004
#define BST_FOCUS          0x0008


// List Error code
#define LB_ERR              (-1)
#define LB_ERRSPACE         (-2)
// List Notify code
#define LBN_ERRSPACE        (-2)
#define LBN_SELCHANGE       1
#define LBN_DBLCLK          2
#define LBN_SELCANCEL       3
#define LBN_SETFOCUS        4
#define LBN_KILLFOCUS       5

// 自定义消息
// lParam = LPLBNOTIFY struct
#define LBN_CLICK           100 
typedef struct _LBNOTIFY
{
    HWND hwndFrom;
	int  iItem;
    UINT uiState;
	DWORD dwItemData;
}LBNOTIFY, * PLBNOTIFY, FAR * LPLBNOTIFY;

//Listbox Styles

#define LBS_NOTIFY            0x0001L
#define LBS_SORT              0x0002L
#define LBS_NOREDRAW          0x0004L
#define LBS_MULTIPLESEL       0x0008L
#define LBS_OWNERDRAWFIXED    0x0010L
#define LBS_OWNERDRAWVARIABLE 0x0020L
#define LBS_HASSTRINGS        0x0040L
//#define LBS_USETABSTOPS       0x0080L
//#define LBS_NOINTEGRALHEIGHT  0x0100L
#define LBS_MULTICOLUMN       0x0200L
//#define LBS_WANTKEYBOARDINPUT 0x0400L
//#define LBS_EXTENDEDSEL       0x0800L
//#define LBS_DISABLENOSCROLL   0x1000L
#define LBS_NODATA            0x2000L
#define LBS_NOSEL             0x4000L
#define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

// List message
#define LB_ADDSTRING            0x0180
#define LB_INSERTSTRING         0x0181
#define LB_DELETESTRING         0x0182
#define LB_SELITEMRANGEEX       0x0183
#define LB_RESETCONTENT         0x0184
#define LB_SETSEL               0x0185
#define LB_SETCURSEL            0x0186
#define LB_GETSEL               0x0187
#define LB_GETCURSEL            0x0188
#define LB_GETTEXT              0x0189
#define LB_GETTEXTLEN           0x018A
#define LB_GETCOUNT             0x018B
#define LB_SELECTSTRING         0x018C
#define LB_DIR                  0x018D
#define LB_GETTOPINDEX          0x018E
#define LB_FINDSTRING           0x018F
#define LB_GETSELCOUNT          0x0190
#define LB_GETSELITEMS          0x0191
#define LB_SETTABSTOPS          0x0192
#define LB_GETHORIZONTALEXTENT  0x0193
#define LB_SETHORIZONTALEXTENT  0x0194
#define LB_SETCOLUMNWIDTH       0x0195
#define LB_ADDFILE              0x0196
#define LB_SETTOPINDEX          0x0197
#define LB_GETITEMRECT          0x0198
#define LB_GETITEMDATA          0x0199
#define LB_SETITEMDATA          0x019A
#define LB_SELITEMRANGE         0x019B
#define LB_SETANCHORINDEX       0x019C
#define LB_GETANCHORINDEX       0x019D
#define LB_SETCARETINDEX        0x019E
#define LB_GETCARETINDEX        0x019F
#define LB_SETITEMHEIGHT        0x01A0
#define LB_GETITEMHEIGHT        0x01A1
#define LB_FINDSTRINGEXACT      0x01A2
#define LB_SETLOCALE            0x01A5
#define LB_GETLOCALE            0x01A6
#define LB_SETCOUNT             0x01A7

// Scroll Bar Styles
#define SBS_HORZ                    0x0000L
#define SBS_VERT                    0x0001L


typedef VOID ( CALLBACK * PERROR_WINDOW_HOOK )( HWND hWnd );

#define SetWindowRgn Wnd_SetRgn
int WINAPI Wnd_SetRgn( HWND hWnd, HRGN hrgn, BOOL bRedraw );

#define GetWindowRgn Wnd_GetRgn
int WINAPI Wnd_GetRgn( HWND hWnd, HRGN hrgn );


#define LWA_COLORKEY 1 
#define LWA_ALPHA 2 

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // __EWIN_H
