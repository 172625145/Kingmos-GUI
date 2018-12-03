/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __WINSRV_H
#define __WINSRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#undef CreateWindowEx
#define CreateWindowEx Win_CreateEx
API_TYPE HWND WINAPI Win_CreateEx(                 
                    DWORD dwExStyle,
                    LPCBYTE lpClassName,
		            LPCBYTE lpText,
                    DWORD dwMainStyle,
		            int x, int y, int dx, int dy,
		            HWND hParent,
		            HMENU hMenu,
		            HINSTANCE hInstance,
		            LPVOID lpCreateParams );

#undef CreateWindow
#define CreateWindow( lpClassName, lpText, dwMainStyle, x, y, dx, dy,\
hParent, hMenu, hInstance, lpCreateParam ) \
Win_CreateEx( 0, lpClassName, lpText, dwMainStyle, x, y, dx, dy,\
hParent, hMenu, (HINSTANCE)(hInstance), lpCreateParam )

#undef IsWindow
#define IsWindow Win_IsWindow
API_TYPE BOOL WINAPI Win_IsWindow( HWND hWnd );

#undef ShowScrollBar
#define ShowScrollBar Win_ShowScrollBar
API_TYPE BOOL WINAPI Win_ShowScrollBar( HWND hWnd, int fnBar, BOOL bShow );

#undef DestroyWindow
#define DestroyWindow Win_Destroy 
API_TYPE BOOL WINAPI Win_Destroy( HWND hWnd );

#undef IsChild
#define IsChild Win_IsChild
API_TYPE BOOL WINAPI Win_IsChild( HWND hParent, HWND hWnd );

#undef GetParent
#define GetParent Win_GetParent
API_TYPE HWND WINAPI Win_GetParent( HWND hWnd );

#undef SetParent
#define SetParent Win_SetParent
API_TYPE HWND WINAPI Win_SetParent( HWND hChild, HWND hNewParent );

#undef IsWindowVisible
#define IsWindowVisible Win_IsVisible
API_TYPE BOOL WINAPI Win_IsVisible( HWND hWnd );

#undef ShowWindow
#define ShowWindow Win_Show
API_TYPE BOOL WINAPI Win_Show( HWND hWnd, int cmd );

#undef EnableWindow
#define EnableWindow Win_Enable
API_TYPE BOOL WINAPI Win_Enable( HWND hWnd, BOOL bEnable );

#undef IsWindowEnabled
#define IsWindowEnabled Win_IsEnabled
API_TYPE BOOL WINAPI Win_IsEnabled( HWND hWnd );

#undef SetWindowText
#define SetWindowText Win_SetText
API_TYPE BOOL WINAPI Win_SetText( HWND hWnd, LPCSTR lpcstr );

#undef GetWindowText
#define GetWindowText Win_GetText
API_TYPE int WINAPI Win_GetText( HWND hWnd, LPSTR lpstr, int nMaxCount );

#undef GetWindowTextLength
#define GetWindowTextLength Win_GetTextLength
API_TYPE int WINAPI Win_GetTextLength( HWND hWnd );

#undef GetClientRect
#define GetClientRect Win_GetClientRect
API_TYPE BOOL WINAPI Win_GetClientRect( HWND hWnd, LPRECT lpRect );

#undef GetWindowRect
#define GetWindowRect Win_GetWindowRect
API_TYPE BOOL WINAPI Win_GetWindowRect( HWND hWnd, LPRECT lpRect );

#undef SetWindowPos
#define SetWindowPos Win_SetPos
API_TYPE BOOL WINAPI Win_SetPos( HWND hWnd, HWND hWndInsertAfter,
                   int x, int y, int cx, int cy,
                   UINT uFlags );

#undef MoveWindow
#define MoveWindow Win_Move
API_TYPE BOOL WINAPI Win_Move( HWND hWnd, int x, int y, int cx, int cy, BOOL bRepaint );

#undef BringWindowToTop
#define BringWindowToTop Win_BringToTop
API_TYPE BOOL WINAPI Win_BringToTop( HWND hWnd );

#undef CallWindowProc
#define CallWindowProc Win_CallProc
API_TYPE LRESULT WINAPI Win_CallProc( WNDPROC lpPrevWndFunc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#undef AdjustWindowRectEx
#define AdjustWindowRectEx Win_AdjustRectEx
API_TYPE BOOL WINAPI Win_AdjustRectEx( LPRECT lpRect, DWORD dwMainStyle, BOOL bMenu, DWORD dwSubStyle );

#undef FlashWindow
#define FlashWindow Win_Flash
API_TYPE BOOL WINAPI Win_Flash( HWND hWnd, BOOL bInvert );

#undef CloseWindow
#define CloseWindow Win_Close
API_TYPE BOOL WINAPI Win_Close( HWND hWnd );

#undef IsWindowIconic
#define IsWindowIconic Win_IsIconic
API_TYPE BOOL WINAPI Win_IsIconic( HWND hWnd );

#undef IsWindowZoomed
#define IsWindowZoomed Win_IsZoomed
API_TYPE BOOL WINAPI Win_IsZoomed( HWND hWnd );

#undef GetWindowLong
#define GetWindowLong Win_GetLong
API_TYPE LONG WINAPI Win_GetLong( HWND hWnd, int nIndex );

#undef SetWindowLong
#define SetWindowLong Win_SetLongInside
API_TYPE LONG WINAPI Win_SetLongInside( HWND hWnd, int nIndex, LONG dwNewLong );

API_TYPE LONG WINAPI Win_SetLong( HWND hWnd, int nIndex, LONG dwNewLong );

#undef GetDesktopWindow
#define GetDesktopWindow Win_GetDesktop
API_TYPE HWND WINAPI Win_GetDesktop( void );

#undef FindWindow
#define FindWindow Win_Find
API_TYPE HWND WINAPI Win_Find( LPCTSTR lpcClassName, LPCTSTR lpcWindowName );

#undef UpdateWindow
#define UpdateWindow Win_Update
API_TYPE BOOL WINAPI Win_Update( HWND hWnd );

#undef GetUpdateRect
#define GetUpdateRect Win_GetUpdateRect
API_TYPE BOOL WINAPI Win_GetUpdateRect( HWND hWnd, LPRECT lpRect, BOOL bErase );

#undef GetUpdateRgn
#define GetUpdateRgn Win_GetUpdateRgn
API_TYPE int WINAPI Win_GetUpdateRgn( HWND hWnd, HRGN hrgn, BOOL bErase );

#undef EnumWindows
#define EnumWindows Win_EnumInside
//内部调用
API_TYPE BOOL WINAPI _Win_EnumInside( WNDENUMPROC lpEnumFunc, LPARAM lParam ); 
//系统调用
API_TYPE BOOL WINAPI Win_Enum( WNDENUMPROC lpEnumFunc, LPARAM lParam );

#undef EnumChildWindows
#define EnumChildWindows _Win_EnumChildInside
//内部调用
API_TYPE BOOL WINAPI _Win_EnumChildInside( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam );
//系统调用
API_TYPE BOOL WINAPI Win_EnumChild( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam );

#undef GetTopWindow
#define GetTopWindow Win_GetTop
API_TYPE HWND WINAPI Win_GetTop( HWND hWnd );

#undef GetWindow
#define GetWindow Win_Get
API_TYPE HWND WINAPI Win_Get( HWND hWnd, UINT uCmd );

#undef GetNextWindow
#define GetNextWindow Win_GetNext
API_TYPE HWND WINAPI Win_GetNext( HWND hWnd, UINT wCmd );

#undef ClientToScreen
#define ClientToScreen Win_ClientToScreen
API_TYPE BOOL WINAPI Win_ClientToScreen( HWND hWnd, LPPOINT lpPoint );

#undef ScreenToClient
#define ScreenToClient Win_ScreenToClient
API_TYPE BOOL WINAPI Win_ScreenToClient( HWND hWnd, LPPOINT lpPoint );

#undef MapWindowPoints
#define MapWindowPoints Win_MapPoints
API_TYPE DWORD WINAPI Win_MapPoints( HWND hwndFrom, HWND hwndTo, LPPOINT lppt, UINT cpt );

#undef WindowFromPoint
#define WindowFromPoint Win_FromPoint

// HWND WINAPI Win_FromPoint( POINT ) 改变为 
// HWND WINAPI Win_FromPoint( int x, int y ) 
// beause the CALL API 参数传递是依赖与平台的

API_TYPE HWND WINAPI Win_FromPoint( int x, int y );//POINT point );

#undef ChildWindowFromPointEx
#define ChildWindowFromPointEx Win_ChildFromPoint
// HWND WINAPI Win_ChildFromPoint( ,POINT, ) 改变为 
// HWND WINAPI Win_FromPoint( , int x, int y,  ) 
// beause the CALL API 参数传递是依赖与平台的

//API_TYPE HWND WINAPI Win_ChildFromPoint( HWND hParent, POINT point, UINT uFlags );
API_TYPE HWND WINAPI Win_ChildFromPoint( HWND hParent, int x, int y, UINT uFlags );

#undef GetForegroundWindow
#define GetForegroundWindow Win_GetForeground
API_TYPE HWND WINAPI Win_GetForeground( void );

#undef SetForegroundWindow
#define SetForegroundWindow Win_SetForeground
API_TYPE BOOL WINAPI Win_SetForeground( HWND hWnd );

#undef SetActiveWindow
#define SetActiveWindow Win_SetActive
API_TYPE HWND WINAPI Win_SetActive( HWND hWnd );

#undef SetTimer
#define SetTimer Win_SetTimer
API_TYPE UINT WINAPI Win_SetTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc );

#undef KillTimer
#define KillTimer Win_KillTimer
API_TYPE BOOL WINAPI Win_KillTimer( HWND hWnd, UINT id );

#undef SetSysTime
#define SetSysTime Win_SetSysTimer
API_TYPE UINT WINAPI Win_SetSysTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc );

#undef KillSysTimer
#define KillSysTimer Win_KillSysTimer
API_TYPE BOOL WINAPI Win_KillSysTimer( HWND hWnd, UINT id );

#undef GetActiveWindow
#define GetActiveWindow Win_GetActive
API_TYPE HWND WINAPI Win_GetActive(void);

#undef GetTopLevelParent
#define GetTopLevelParent Win_GetTopLevelParent
API_TYPE HWND WINAPI Win_GetTopLevelParent( HWND hWnd );

#undef SetFocus
#define SetFocus Win_SetFocus
API_TYPE HWND WINAPI Win_SetFocus( HWND hWnd );

#undef GetFocus
#define GetFocus Win_GetFocus
API_TYPE HWND WINAPI Win_GetFocus( void );

#undef SetCapture
#define SetCapture Win_SetCapture
API_TYPE HWND WINAPI Win_SetCapture( HWND hWnd );

#undef ReleaseCapture
#define ReleaseCapture Win_ReleaseCapture
API_TYPE BOOL WINAPI Win_ReleaseCapture(void);

#undef GetCapture
#define GetCapture Win_GetCapture
API_TYPE HWND WINAPI Win_GetCapture( void );

#undef BeginPaint
#define BeginPaint WinGdi_BeginPaint
API_TYPE HDC WINAPI WinGdi_BeginPaint( HWND hWnd, LPPAINTSTRUCT lpPaint );

#undef EndPaint
#define EndPaint WinGdi_EndPaint
API_TYPE BOOL WINAPI WinGdi_EndPaint( HWND hWnd, const PAINTSTRUCT *lpPaint );

#undef GetDC
#define GetDC WinGdi_GetClientDC
API_TYPE HDC WINAPI WinGdi_GetClientDC( HWND hWnd );

#undef ReleaseDC
#define ReleaseDC WinGdi_ReleaseDC
API_TYPE int WINAPI WinGdi_ReleaseDC( HWND hWnd, HDC hdc );

#undef GetWindowDC
#define GetWindowDC WinGdi_GetWindowDC
API_TYPE HDC WINAPI WinGdi_GetWindowDC( HWND hWnd );

#undef DrawCaption
#define DrawCaption Win_DrawCaption
API_TYPE BOOL WINAPI Win_DrawCaption( HWND hWnd, HDC hdc, LPCRECT lprc, UINT uFlags );

#undef ScrollWindowEx
#define ScrollWindowEx Win_Scroll
API_TYPE int WINAPI Win_Scroll( HWND hWnd,
                    int dx,
                    int dy,
                    LPCRECT lpScrollRect,
                    LPCRECT lpClipRect,
                    HRGN hrgnUpdate,
                    LPRECT lpUpdateRect,
                    UINT flags );

#undef GetDCEx
#define GetDCEx WinGdi_GetDCEx
API_TYPE HDC WINAPI WinGdi_GetDCEx(HWND hWnd, HRGN hrgn, DWORD flags);

#undef InvalidateRect
#define InvalidateRect Win_InvalidateRect
API_TYPE BOOL WINAPI Win_InvalidateRect(HWND hWnd, LPCRECT lprc, BOOL bErase);

#undef InvalidateRgn
#define InvalidateRgn Win_InvalidateRgn
API_TYPE BOOL WINAPI Win_InvalidateRgn(HWND hWnd, HRGN hrgn, BOOL bErase);

#undef ValidateRect
#define ValidateRect Win_ValidateRect
API_TYPE BOOL WINAPI Win_ValidateRect( HWND hWnd, const RECT * lpRect );

#undef ValidateRgn
#define ValidateRgn Win_ValidateRgn
API_TYPE BOOL WINAPI Win_ValidateRgn( HWND hWnd, HRGN hrgn );

#undef GetWindowThreadProcessId
#define GetWindowThreadProcessId Win_GetThreadProcessId
API_TYPE DWORD WINAPI Win_GetThreadProcessId( HWND hWnd, DWORD * lpProcessID );

#undef EnableScrollBar
#define EnableScrollBar Win_EnableScrollBar
API_TYPE BOOL WINAPI Win_EnableScrollBar(HWND hWnd, UINT fnBar, UINT wArrows );

#undef GetScrollInfo
#define GetScrollInfo Win_GetScrollInfo
API_TYPE int WINAPI Win_GetScrollInfo(HWND hWnd, int fnBar, LPSCROLLINFO lpsi );

#undef SetScrollInfo
#define SetScrollInfo Win_SetScrollInfo
API_TYPE int WINAPI Win_SetScrollInfo(HWND hWnd, int fnBar, LPCSCROLLINFO lpcsi, BOOL fRedraw);

#undef DefWindowProc
#define DefWindowProc Win_DefProc
API_TYPE LRESULT WINAPI Win_DefProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

#undef GetClassName
#define GetClassName Win_GetClassName
API_TYPE int WINAPI Win_GetClassName( HWND hWnd, LPTSTR lpszClassName, int nMaxCount );

#undef ClearWindow
#define ClearWindow Win_Clear
API_TYPE BOOL WINAPI Win_Clear( DWORD dwThreadID );

////////////////////////////////////////////////////////////

#undef RegisterClass
#define RegisterClass WinClass_Register
API_TYPE ATOM WINAPI WinClass_Register( LPCWNDCLASS lpWndClass );

#undef GetClassInfo
#define GetClassInfo WinClass_GetInfo
API_TYPE BOOL WINAPI WinClass_GetInfo( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpWndClass);

#undef UnregisterClass
#define UnregisterClass WinClass_Unregister
API_TYPE int WINAPI WinClass_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance );

#undef GetClassLong
#define GetClassLong WinClass_GetLong
API_TYPE DWORD WINAPI WinClass_GetLong( HWND hWnd, int nIndex );

#undef SetClassLong
#define SetClassLong WinClass_SetLong
DWORD WINAPI WinClass_SetLong( HWND hWnd, int nIndex, LONG lNewValue );

#undef ClearClass
#define ClearClass WinClass_Clear
API_TYPE BOOL CALLBACK WinClass_Clear( HANDLE hInst );

////////////////////////////////////////////////////////////

#undef PostMessage
#define PostMessage WinMsg_Post
API_TYPE BOOL WINAPI WinMsg_Post(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

#undef PostThreadMessage
#define PostThreadMessage WinMsg_PostThread
API_TYPE BOOL WINAPI WinMsg_PostThread( DWORD dwThreadID, UINT uMsg, WPARAM wParam, LPARAM lParam );

#undef GetMessage
#define GetMessage WinMsg_Get
API_TYPE BOOL WINAPI WinMsg_Get( MSG FAR* lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax );

#undef PeekMessage
#define PeekMessage WinMsg_Peek
API_TYPE BOOL WINAPI WinMsg_Peek( MSG FAR* lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax, UINT wRemoveMsg );

#undef PostQuitMessage
#define PostQuitMessage WinMsg_PostQuit
API_TYPE void WINAPI WinMsg_PostQuit( int nExitCode );

#undef DispatchMessage
#define DispatchMessage WinMsg_Dispatch
API_TYPE LRESULT WINAPI WinMsg_Dispatch( const MSG * lpMsg );

//#undef TranslateMessage
//#define TranslateMessage WinMsg_Translate
//API_TYPE BOOL WINAPI WinMsg_Translate( const MSG FAR * lpMsg );

#undef SendMessage
#define SendMessage WinMsg_Send
API_TYPE LRESULT WINAPI WinMsg_Send( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
//内部使用，屏蔽
API_TYPE LRESULT WINAPI _WinMsg_SendSync( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );


#undef SendMessageTimeout
#define SendMessageTimeout WinMsg_SendTimeout
API_TYPE LRESULT WINAPI WinMsg_SendTimeout( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, PDWORD lpdwResult );

#undef WaitMessage
#define WaitMessage WinMsg_Wait
API_TYPE BOOL WINAPI WinMsg_Wait( void );

#undef mouse_event 
#define mouse_event WinEvent_MouseEvent
VOID WINAPI WinEvent_MouseEvent( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );

#undef keybd_event
#define keybd_event WinEvent_KeybdEvent
VOID WINAPI WinEvent_KeybdEvent( BYTE bVk, BYTE bScan, DWORD dwFlags, DWORD dwExtraInfo );

#undef MsgWaitForMultipleObjects
#define MsgWaitForMultipleObjects WinMsg_WaitForMultipleObjects
DWORD WINAPI WinMsg_WaitForMultipleObjects(
									   DWORD nCount,
									   LPHANDLE pHandles,
									   BOOL fWaitAll,
									   DWORD dwMilliseconds,
									   DWORD dwWakeMask
									   );


//API_TYPE BOOL CALLBACK MQ_FreeThreadQueue( _MSGQUEUE * lpQueue );

////////////////////////////////////////////////////

API_TYPE void WINAPI WinSys_CloseObject( HANDLE hProcess );

#undef GetSystemMetrics
#define GetSystemMetrics WinSys_GetMetrics
API_TYPE int WINAPI WinSys_GetMetrics( int nIndex );

#undef SetSystemMetrics
#define SetSystemMetrics WinSys_SetMetrics
API_TYPE int WINAPI WinSys_SetMetrics( int nIndex, int iNewValue );

#undef GetSysColor
#define GetSysColor WinSys_GetColor
API_TYPE DWORD WINAPI WinSys_GetColor(int nIndex);

#undef SetSysColors
#define SetSysColors WinSys_SetColors
API_TYPE BOOL WINAPI WinSys_SetColors(int cElements, 
						  const int *lpaElements, 
						  const COLORREF *lpaRgbValues );

#undef GetSysColorBrush
#define GetSysColorBrush WinSys_GetColorBrush
API_TYPE HBRUSH WINAPI WinSys_GetColorBrush(int nIndex);

API_TYPE BOOL WINAPI WinSys_TouchCalibrate( void );
API_TYPE BOOL WINAPI WinSys_SetCalibrateWindow( HWND hWnd );
API_TYPE BOOL WINAPI WinSys_SetCalibratePoints(	
	int   cCalibrationPoints,
    int   *pScreenXBuffer,
    int   *pScreenYBuffer,
    int   *pUncalXBuffer,
    int   *pUncalYBuffer );

///////////////////////////////////////////////////

#undef CreateCaret
#define CreateCaret WinCaret_Create
API_TYPE BOOL WINAPI WinCaret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight );

#undef DestroyCaret
#define DestroyCaret WinCaret_Destroy
API_TYPE BOOL WINAPI WinCaret_Destroy( void );

#undef GetCaretBlinkTime
#define GetCaretBlinkTime WinCaret_GetBlinkTime
API_TYPE UINT WINAPI WinCaret_GetBlinkTime( void );

#undef GetCaretPos
#define GetCaretPos WinCaret_GetPos
API_TYPE BOOL WINAPI WinCaret_GetPos( LPPOINT lpPoint );

#undef HideCaret
#define HideCaret WinCaret_Hide
API_TYPE BOOL WINAPI WinCaret_Hide( HWND hWnd );

#undef SetCaretBlinkTime
#define SetCaretBlinkTime WinCaret_SetBlinkTime
API_TYPE BOOL WINAPI WinCaret_SetBlinkTime( UINT uMSeconds );

#undef SetCaretPos
#define SetCaretPos WinCaret_SetPos
API_TYPE BOOL WINAPI WinCaret_SetPos( int x, int y );

#undef ShowCaret
#define ShowCaret WinCaret_Show
API_TYPE BOOL WINAPI WinCaret_Show( HWND hWnd );

///////////////////////////////////////////////////

#undef CreateDialogIndirect
#define CreateDialogIndirect WinDlg_CreateIndirect
API_TYPE HWND WINAPI WinDlg_CreateIndirect( HINSTANCE hInst, LPCDLG_TEMPLATE lpTemplate, HWND hParent, DLGPROC lpDialogFunc );

#undef CreateDialogIndirectParam
#define CreateDialogIndirectParam WinDlg_CreateIndirectParam
API_TYPE HWND WINAPI WinDlg_CreateIndirectParam( HINSTANCE hInst, LPCDLG_TEMPLATE lpName, HWND hParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

#undef DialogBoxIndirect
#define DialogBoxIndirect WinDlg_BoxIndirect
API_TYPE int WINAPI WinDlg_BoxIndirect( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc );

#undef DialogBoxIndirectParam
#define DialogBoxIndirectParam WinDlg_BoxIndirectParam
API_TYPE int WINAPI WinDlg_BoxIndirectParam( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

//ex
#undef CreateDialogIndirectEx
#define CreateDialogIndirectEx WinDlg_CreateIndirectEx
API_TYPE HWND WINAPI WinDlg_CreateIndirectEx( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpTemplate, HWND hParent, DLGPROC lpDialogFunc );

#undef CreateDialogIndirectParamEx
#define CreateDialogIndirectParamEx WinDlg_CreateIndirectParamEx
API_TYPE HWND WINAPI WinDlg_CreateIndirectParamEx( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpName, HWND hParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

#undef DialogBoxIndirectEx
#define DialogBoxIndirectEx WinDlg_BoxIndirectEx
API_TYPE int WINAPI WinDlg_BoxIndirectEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc );

#undef DialogBoxIndirectParamEx
#define DialogBoxIndirectParamEx WinDlg_BoxIndirectParamEx
API_TYPE int WINAPI WinDlg_BoxIndirectParamEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit );

//ex-end

#undef EndDialog
#define EndDialog WinDlg_End
API_TYPE BOOL WINAPI WinDlg_End( HWND hDlg, int nResult );

#undef GetDlgCtrlID
#define GetDlgCtrlID WinDlg_GetCtrlID
API_TYPE int WINAPI WinDlg_GetCtrlID( HWND hwndCtl );

#undef GetDlgItem
#define GetDlgItem WinDlg_GetItem
API_TYPE HWND WINAPI WinDlg_GetItem( HWND hDlg, int nID );

#undef GetDlgItemText
#define GetDlgItemText WinDlg_GetItemText
API_TYPE UINT WINAPI WinDlg_GetItemText( HWND hDlg, int nID, LPTSTR lpstr, int nMaxCount );

#undef SetDlgItemText
#define SetDlgItemText WinDlg_SetItemText
API_TYPE BOOL WINAPI WinDlg_SetItemText( HWND hDlg, int nID, LPCTSTR lpstr );

#undef GetDlgItemInt
#define GetDlgItemInt WinDlg_GetItemInt
UINT WINAPI WinDlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned );

#undef SetDlgItemInt
#define SetDlgItemInt WinDlg_SetItemInt
BOOL WINAPI WinDlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned );

#undef SendDlgItemMessage
#define SendDlgItemMessage WinDlg_SendItemMessage
API_TYPE LONG WINAPI WinDlg_SendItemMessage( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam );

#undef MessageBox
#define MessageBox WinDlg_MessageBox
API_TYPE int WINAPI WinDlg_MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType );

// code page
//#undef MultiByteToWideChar
//#define MultiByteToWideChar WinCP_MultiByteToWideChar

//API_TYPE int WINAPI WinCP_MultiByteToWideChar(
//						UINT uiCodePage, 
//						DWORD dwFlags, 
//						LPCSTR lpMultiByteStr, 
//						int cbMultiByte, 
//						LPWSTR lpWideCharStr, 
//						int cchWideChar 
//						);

//#undef WideCharToMultiByte
//#define WideCharToMultiByte WinCP_WideCharToMultiByte
//API_TYPE int WINAPI WinCP_WideCharToMultiByte(
//						UINT uiCodePage, 
//						DWORD dwFlags, 
//						LPCWSTR lpWideCharStr, 
//						int cchWideChar, 
//						LPSTR lpMultiByteStr, 
//						int cbMultiByte, 
//						LPCSTR lpDefaultChar, 
//						BOOL * lpUsedDefaultChar 
//						);

#undef MessageBeep
#define MessageBeep WinMsg_Beep
BOOL WINAPI WinMsg_Beep( UINT uType );

#undef GetKeyState
#define GetKeyState WinMsg_GetKeyState
SHORT WINAPI WinMsg_GetKeyState( int nVirtKey );

#undef ShowDesktop
#define ShowDesktop Win_ShowDesktop
BOOL WINAPI Win_ShowDesktop( UINT uiFlag );

#undef SetWindowRgn
#define SetWindowRgn Win_SetRgn
int WINAPI Win_SetRgn( HWND hWnd, 
					   HRGN hrgn,   //需要设置的域
					   BOOL bRedraw //是否立即重绘
						);
int WINAPI Win_GetRgn(
					  HWND hWnd,  //窗口句柄
					  HRGN hRgn	  //需要被设置的域
					);

 
#ifdef __cplusplus
}
#endif  /* __cplusplus */

///////////////////////////////////////////////////

#endif

