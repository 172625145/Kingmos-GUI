#ifndef __EPWIN_H
#define __EPWIN_H

// scrollbar data
/*
typedef struct __SCROLLBAR{
    short nMin; // log pos
    short nMax; // log pos
    short nPage; // log pos
    short nPos;
    short nTrackPos;
    WORD state;
}_SCROLLDATA, FAR * _LPSCROLLDATA;


typedef struct __SCROLL{
    _SCROLLDATA sd;
    RECT rect;
    short fnBar;
}_SCROLL, FAR * _LPSCROLL;

#define OFF_MIN        0
#define OFF_MAX        2
#define OFF_PAGE       4
#define OFF_POS        6
#define OFF_TRACKPOS   8
#define OFF_STATE     10
#define MIN_THUMBSIZE 14
#define SBS_TRACKTHUMB 0x1000

static BOOL Scrl_GetScrollData( HWND hWnd, int fnBar, _LPSCROLL );
static BOOL Scrl_SendScrollMsg( HWND, int fnBar, UINT dwMsg, WPARAM, LPARAM );
static void Scrl_SetScrollValue( HWND, int fnBar, int offset, int v );
static int Scrl_GetScrollValue( HWND, int fnBar, int offset );
static int Scrl_GetDrawPos( _LPSCROLL, int pos[6] );
static int Scrl_DrawScrollBar( _LPSCROLL, HDC );
static HDC Scrl_GetScrollDC( HWND, int fnBar );
BOOL Scrl_Inital( _LPSCROLLDATA lpsd );
int Scrl_ShowThumbBox( _LPSCROLL, HDC, BOOL );
void Scrl_GetBarRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle, int which );
static BOOL Scrl_GetThumbRect( _LPSCROLL lpsd, LPRECT lprect );
int Scrl_RedrawScrollBar( HWND hWnd, HDC hdc, int fnBar );
static BOOL Scrl_GetRect( _LPSCROLL, int fBar, LPRECT );
// this is all message handle proc
LRESULT Scrl_DoLBUTTONDOWN( HWND hWnd, int fnBar, int x, int y );

//LRESULT Scrl_DoSETSCROLLINFO( HWND hWnd, int fnBar, const void * lpsi, BOOL fRedraw );
LRESULT Scrl_DoSETSCROLLINFO( HWND hWnd, int fnBar, LPCSCROLLINFO lpsi,  BOOL fRedraw );
LRESULT Scrl_DoGETSCROLLINFO( HWND hWnd, int fnBar, void * lpsi );
//LRESULT Scrl_DoSETPOS( HWND hWnd, int fnBar, int nPos, BOOL fRedraw );
//LRESULT Scrl_DoGETPOS( HWND hWnd, int fnBar );
//LRESULT Scrl_DoGETRANGE( HWND hWnd, int fnBar, LPINT lpnMinPos, LPINT lpnMaxPos );
//LRESULT Scrl_DoSETRANGE( HWND hWnd, int fnBar, int nMinPos, int nMaxPos, BOOL fRedraw );
*/

// window struct
//typedef struct __INSTANCE
//{
//    HANDLE hThread;
    //HWND hwndActive;
//}_INSTANCE;

//#define _GET_HINST_PTR( hinst ) ((_INSTANCE*)(hinst))
//#define _GET_HTHREAD_PTR( hinst ) ((_LPINSTANCE)(hinst))

typedef struct __REGCLASS
{
	DWORD objType;   // OBJ_CLASS
	int nRefCount;
	struct __REGCLASS FAR * lpNext;
	HANDLE hOwnerProcess;
	//OBJLIST obj;

    WNDCLASS wc;  //下面不能加任何变量，因为附加的类成员在下面！！！
}_REGCLASS, FAR * _LPREGCLASS;

typedef struct __PROCESS_CLASS
{
	struct __PROCESS_CLASS FAR * lpNext;
	HANDLE hProcess;
	_LPREGCLASS lpRegClass;
}_PROCESS_CLASS, FAR * _LPPROCESS_CLASS;


BOOL KL_AddClassRef( _LPREGCLASS lpClass );
BOOL KL_DecClassRef( _LPREGCLASS lpClass );

_LPREGCLASS KL_GetClassPtr( LPCSTR lpcClassName, HINSTANCE hInstance );
BOOL KL_UnregisterClass( HINSTANCE hInst );  // free all class oweed by instance when progrm terminal

// window state
//#define WINS_UPDATEDC   0x00000001
//#define WINS_INSEND       0x00000001
#define WINS_INUSERPROC     0x0002
//#define WINS_SHOWDESKTOP  0x00000004
#define WINS_ZORDER_CHANGING 0x0008
//typedef struct _WIN_CALL_INFO 
//{
  //  WNDPROC lpfnCurWndProc;
//	HANDLE  hCurProcess;
//}WIN_CALL_INFO, FAR * LPWIN_CALL_INFO;
typedef struct __LAYERED_DATA
{
	//HBITMAP hbmpLayered;			// 用于 Layered Window , WS_EX_LAYERED
	COLORREF clrTransparentKey;     // specifies the transparent color key
	UINT bAlpha;					// value for the blend function	
	DWORD dwFlags;
	HBITMAP hBitmap;
	//HDC hdcMem;
}_LAYERED_DATA, FAR * _LPLAYERED_DATA;
typedef struct __WINDATA
{
    DWORD   objType;    // must OBJ_WINDOW
	//LPCSTR lpcClassName;
	_LPREGCLASS lpClass;
	HWND hThis;

//    DWORD idClass;  // class id
	WORD   uState;
	WORD   nSendCount;
    LPSTR  lpWinText;
    DWORD dwMainStyle;
    DWORD dwExStyle;
    HMENU hMenu;
    HINSTANCE hinst;
	HANDLE hOwnerProcess;
	DWORD dwThreadID;

    RECT rectClient;  // coordinate relative to top-left of window
    RECT rectWindow;  // coordinate relative to screen window
    //RECT rectRestore;
	//DWORD dwRestoreMainStyle;
	//DWORD dwRestoreExStyle;

	//WIN_CALL_INFO wci;
    WNDPROC lpfnWndProc;
	//HANDLE  hCurProcess;
//    DWORD id;

    //_SCROLLDATA vScrollBar;
    //_SCROLLDATA hScrollBar;
	//_SCROLLDATA * lpvScrollBar;
	//_SCROLLDATA * lphScrollBar;
	VOID * lpvScrollBar;
	VOID * lphScrollBar;

    DWORD dwUserData;
    int   uOwnCount;
	HICON hSmallIcon;
	HICON hBigIcon;
	//HRGN  hrgnInvalidate;
//#ifdef __DEBUG
//    struct __WINDATA FAR* hParent;
//    struct __WINDATA FAR* hOwner;
//    struct __WINDATA FAR* hNext;
//    struct __WINDATA FAR* hPrev;
//    struct __WINDATA FAR* hChild;
//#else

	struct __WINDATA FAR * lpwsParent;
	struct __WINDATA FAR * lpwsOwner;
	struct __WINDATA FAR * lpwsNext;
	struct __WINDATA FAR * lpwsPrev;
	struct __WINDATA FAR * lpwsChild;
 
	//HWND hParent;
    //HWND hOwner;
    //HWND hNext;
    //HWND hPrev;
    //HWND hChild;
//#endif
    HRGN hrgnWindow;        // coordinate relative to top-left of window

	_LPLAYERED_DATA lpLayer;   //  假如 WS_EX_LAYERED 有效，指向非 NULL

}_WINDATA, FAR * _LPWINDATA;

//_LPWINDATA _GetSafeOwner( _LPWINDATA lpwsParent );//, HWND * lphwndTop );
//_SCROLLDATA * GetWindowScrollBar( _LPWINDATA lpws, UINT type );
WORD _GetWindowWord( HWND hWnd, int pos );
WORD _SetWindowWord( HWND hWnd, int pos, WORD v );

#ifdef __DEBUG
_LPWINDATA __GetHWNDPtr( HWND hWnd, char * file, int line );
#define _GetHWNDPtr( hWnd ) __GetHWNDPtr( (hWnd), __FILE__, __LINE__ )
#endif


//#include <equeue.h>
#define ES_KEY          QS_KEY    //0x00000001
#define ES_MOUSE        QS_MOUSEBUTTON   //0x00000002
#define ES_MOUSEMOVE    QS_MOUSEMOVE     //
#define ES_TIMER        QS_TIMER     //0x00000004

typedef struct __MSGDATA
{
	MSG msg;
    WORD eventType;  //谁产生的消息， Post ? keyboard ? mouse ..
	WORD dump;
}_MSGDATA, FAR * _LPMSGDATA;

typedef struct __MSGQUEUE
{
   	CRITICAL_SECTION csThreadQueue;
    HANDLE hWaitEvent;
	DWORD dwEventMask;
	int size;
    int count;
    //UINT uSignal;
    WORD nQueueTail;
    WORD nQueueHead;
    _LPMSGDATA lpMsgData;
}_MSGQUEUE;


typedef struct _GWEDATA
{
    //SEMAPHORE FAR * lpsemWaitMsg;  // used by message queue
	//DWORD  dwSemaphore;
//	struct _GWEDATA lpNext;
//	LONG nRefCount;
//    DWORD dwThreadId;

    struct __MSGQUEUE msgQueue;
	//UINT   uiPaintMsgCount;
	HWND   hwndCapture;
	HWND   hwndFocus;
	HWND   hwndActive;
	HWND   hwndKeyboard;  // key window
}GWEDATA, FAR * LPGWEDATA;

//#define INIT_GWEDATA  { 0, 0, 0, 0, 0, 0 }
//extern LPGWEDATA WINAPI KL_GetThreadGWEData( DWORD dwThreadID );

//extern _WINDATA hwndScreen;
extern _LPWINDATA lphwndRoot;
#define hwndScreen (*lphwndRoot)

_LPREGCLASS GetClassPtr( LPCSTR lpcClassName, HINSTANCE hInstance );
BOOL AddClassRef( _LPREGCLASS lpClass );
BOOL DecClassRef( _LPREGCLASS lpClass );

//DWORD SetForegroundThreadID( DWORD dwNewID );
//DWORD GetForegroundThreadID( void );
DWORD LockForegroundThreadId( void );
void UnlockForegroundThreadId( void );
void SetForegroundThreadId( DWORD dwNewId );
void ExchangeForegroundThreadId( DWORD dwNewId, DWORD dwCompareId );
BOOL IsForegroundThread( void );

extern BOOL CALLBACK __LockRepaintRgn( HRGN * lphrgn );
extern BOOL CALLBACK __UnlockRepaintRgn( void );


//HRGN GetExposeRgn( HWND hWnd, DWORD flags );
int GetExposeRgn( HWND hWnd, DWORD flags, HRGN * lphrgn );
BOOL IsForegroundThread( void );
BOOL _HideCaret( HWND hWnd, HDC hdc );
BOOL _ShowCaret( HWND hWnd, HDC hdc );
BOOL CheckWinTimerEvent( void );

#define ATQ_ONLYONE  0x80000000l
BOOL AddMsgToThreadQueue( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT uFlag );
// clear from start <= msg && msg <= msgEnd
BOOL ClearThreadQueue( DWORD dwThreadId, HWND hWnd, UINT msgStart, UINT msgEnd );

DWORD GetSYSQueueState( void );
void HandleSystemQueue( void );



int CheckThreadQueue( _LPMSGDATA lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, BOOL fRemove, BOOL fCheckChild );
//BOOL WINAPI KL_RemoveThreadQueue( void );
//BOOL WINAPI KL_ClearThreadQueue( HWND hWnd, UINT msg );
//DWORD WINAPI KL_GetSYSQueueState( void );
DWORD SetSYSQueueState( DWORD dwStateBit, BOOL bSetBit );
//void WINAPI KL_HandleSystemQueue( void );
//BOOL WINAPI KL_FreeThreadQueue( _MSGQUEUE * lpQueue );

//BOOL WINAPI KL_AddToThreadQueue( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT uFlag );



BOOL FASTCALL MQ_FreeThreadQueue( void );
#define TLS_MSGQUEUE           31
//extern LPVOID WINAPI KL_GetThreadTlsValue( DWORD dwThreadId, DWORD dwTlsIndex );

//#define GetThreadGWEPtr( dwThreadId )  ( (LPGWEDATA)TlsGetThreadValue( (dwThreadId), TLS_MSGQUEUE ) )
//#define SetThreadGWEPtr( dwThreadId, lpgwe )  ( TlsSetThreadValue( (dwThreadId), TLS_MSGQUEUE, (lpgwe) ) )
VOID SetThreadGWEPtr( DWORD dwThreadId, LPGWEDATA lpgweData );
LPGWEDATA GetThreadGWEPtr( DWORD dwThreadId );

LPGWEDATA GetGWEPtr( DWORD dwThreadId );

int UpdateDCState( UINT uiState );
//LRESULT _CallWindowProc( _LPWINDATA lpws, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ); //LN, 2003-05-30
LRESULT _CallWindowProc( _LPWINDATA lpws, WNDPROC lpCallProc, UINT message, WPARAM wParam, LPARAM lParam );

BOOL DoEnumTopLevelWindow( WNDENUMPROC lpProc, LPARAM lParam, HANDLE hOwnerProcess );
BOOL WINAPI _WinSys_PostThreadMessage( DWORD dwThreadID, UINT uMsg, WPARAM wParam, LPARAM lParam );
DWORD WINAPI _WinSys_WaitMessage( DWORD dwTimeOut );

extern HINSTANCE hgwmeInstance;
extern HANDLE hgwmeProcess;
extern HANDLE hbmpClose; 
extern HANDLE hbmpHelp;
extern HANDLE hbmpOk;
extern HANDLE hbmpGrayClose; 
extern HANDLE hbmpGrayHelp;
extern HANDLE hbmpGrayOk;
extern HICON hicoSysMenu;
extern HICON hicoGraySysMenu;

#endif  // __EPWIN_H



