/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵�������ڹ���ģ��
�汾�ţ�3.0.0
����ʱ�ڣ�1998
���ߣ�����
�޸ļ�¼��
2005-09-26, �� ���� hrgnWindow �������򣩵Ĺ���
2005-08-30, �� GWME �����о��ͳһΪ index
2005-03-25, �޸� RemovePopupWindow, �� �µķ������ü���֦��
2005-03-12, �� SendMessage ��Ϊ WinMsg_Send
2005-01-08, ������ SetWindowLong �� GetWindowLong�� case ˳��
2004-12-28��BeginPaint �Ƶ�������һ���Դ��� 2004-12-20
2004-10-13, �޸� SetWindowLong �� GWL_WNDPROC ���йؽ��̵ļ��
2004-09-09������,������������Ϣ
2004-08-30: ���� WS_EX_NOFOCUS ���
2004-08-04: �� Win_SetPos ��Ϊ _Win_SetPos
2003-02-24: �� __GET_OBJECT_HANDLE ���� PTR_TO_HANDLE 
2003-09-15 -��16��: SelectClipRgn, GetDCEx, WM_NCPAINT, ExtSelectClipRgn�� SelectObject��
                    hrgnӦ��Client���꣬ ��ǰ��Screen����,�ָ�֮
2003-09-03  ���Ӷ�lpFind->lpcClassName == NULL�Ĵ���
2003-08-04: ���� WS_MINIMIZE ����
2003-07-30: �������������
2003-07-01: Win_GetText,  ��Thread��ΪProcess
2003-06-19: Win_SetLong ���µ�style �� old style ��ͬʱ������
2003-06-03: ���Ӷ� dwForegroundThreadID �Ļ���

2003-06-02: SetWindowLong ɾ���� GWL_HWNDPARENT�Ĵ���
2003-05-26: ���Ӷ� _IsMinimized �Ĵ��ڵļ��
2003-05-14: Win_Desktory ���ӵ�ǰ̨���ڱ��ƻ�ʱ��ϵͳ��ǰ̨����
2003-05-13��GetClassName �д���
2003-04-23: error:���ƻ��κ�windowʱ��ϵͳ��dwForegroundThreadId������ΪNULL
              
2003-04-10������ap֮���л�û�з�WM_KILLFOUCS��Ϣ
            1.����DoSetActive��DoSetFocus��������
			2.��Win_SetActive��Win_SetFocus�Ĳ��ֹ���
			  �ŵ�DoSetActive��DoSetFocus
			3.�޸�Win_SetForeground
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <eassert.h>
#include <ekeybrd.h>
#include <eapisrv.h>

#include <bheap.h>
#include <epwin.h>
#include <gwmeobj.h>
#include <winsrv.h>
#include <gdisrv.h>
#include <eobjcall.h>
#ifdef EML_DOS  
    #include <emouse.h>
#endif
#include <gdc.h>

extern BOOL ReleasePaintDC( HWND hWnd, HDC hdc );
extern HDC GetPaintDC( HWND hwnd, LPCRECT lprect, HRGN hrgnClip, UINT uiFlags );
extern int _GetRgnInfo(HRGN hrgn);

_LPWINDATA lphwndRoot = NULL;

static _REGCLASS classScreen;

//extern proc define
extern BOOL CALLBACK __LockRepaintRgn( HRGN * lphrgn );
extern BOOL CALLBACK __UnlockRepaintRgn( void );

extern void FreeStr( LPSTR lpstr );

// private proc define start
static HRGN _GetNClientRegion( _LPWINDATA lpws );
static BOOL CALLBACK HitTest( HWND hwnd, LPARAM lParam );
static BOOL CALLBACK EnumTest( HWND hwnd, LPARAM lParam );
static _LPWINDATA _BottomChildWindow( _LPWINDATA );
static BOOL _ChangeBound( HWND hWnd, LPRECT lpWindowRect, LPRECT lpClientRect );
static BOOL _DeleteWinObject( _LPWINDATA );
#define GET_Z_ORDER_UP( lpws ) ( ( (lpws)->dwMainStyle & WS_CHILD ) ? (lpws)->lpwsParent : (&hwndScreen) )

static HWND _GetHitedWindow();
static int _GetTopHeight( DWORD dwMainStyle, DWORD dwSubStyle );
static int _GetLeftBorderWidth( DWORD dwMainStyle, DWORD dwSubStyle );
static int _GetBottomHight( DWORD dwMainStyle, DWORD dwSubStyle );
static int _GetRightWidth( DWORD dwMainStyle, DWORD dwSubStyle );
static int _GetMenuHeight( void );
static int _SetState( _LPWINDATA lpws, DWORD style, BOOL bEnable );

static LRESULT _CalcClientBound( HWND hWnd, LPRECT lprect );
static BOOL Link_InsertWindowToScreen( _LPWINDATA lpws );
static BOOL Link_InsertWindowAfter( _LPWINDATA lpwsInsertAfter, _LPWINDATA lpws, _LPWINDATA lpwsTop );
static BOOL Link_InsertToParent( _LPWINDATA lpwsInsertAfter, _LPWINDATA lpws, _LPWINDATA lpwsParent );
static BOOL _IsWindowStyle( HWND hWnd, DWORD dwStyle );
static BOOL _IsMinimized( HWND );
static void _MapAbsCoordinate( HWND, LPPOINT );
static HWND _PrevTopmostWindow( HWND );
void CALLBACK __PutPaintEvent( LPMSG );
static BOOL Link_RemoveWindow( _LPWINDATA );
static BOOL _SetWinParam( _LPWINDATA pws, LPCREATESTRUCT pcs );
static HANDLE _AllocWinObjectHandle( int s );
static HWND DoSetActive( LPGWEDATA lpgwe, _LPWINDATA lpws, BOOL bSync );
static HWND DoSetFocus( LPGWEDATA lpgwe, _LPWINDATA lpws, BOOL bSync );
static BOOL DoEnumWindow( _LPWINDATA lpwsChild, BOOL bEnumChild, CALLBACKDATA * lpcd, LPARAM lParam );


_LPWINDATA NeedLayered( _LPWINDATA lpws );
HDC GetLayerWindowDC( _LPWINDATA lpwsThis, _LPWINDATA lpwsLayered, DWORD dwFlag );

#define WDF_HIDE           0x1
#define WDF_CLEAR          0x2

//#define IS_SHOWENABLE( dwStyle ) ( ( (dwStyle) & WS_VISIBLE ) && ( (dwStyle) & WS_MINIMIZE ) == 0 )

//#define IS_SHOWENABLE( lpws ) ( ( (lpws)->dwMainStyle & WS_VISIBLE ) &&	\
///								( (lpws)->dwMainStyle & WS_MINIMIZE ) == 0 && \
//								( (lpws)->dwExStyle & WS_EX_LAYERED ) == 0 )

#define IS_SHOWENABLE( lpws ) ( ( (lpws)->dwMainStyle & WS_VISIBLE ) &&	\
								( (lpws)->dwMainStyle & WS_MINIMIZE ) == 0 )


// 
//#define IS_SHOWENABLE( lpws ) ( ( (lpws)->dwMainStyle & WS_VISIBLE ) &&	\
//								( (lpws)->dwMainStyle & WS_MINIMIZE ) == 0 \
//								 )

static BOOL _Win_Destroy( _LPWINDATA lpws, DWORD dwCurThreadID, UINT uiFlag );
static _LPWINDATA _GetSafeOwner( _LPWINDATA lpwsParent );
static HWND GetDesktopExplore( void );
static BOOL WINAPI _Win_SetPos( _LPWINDATA lpws, HWND hWndInsertAfter,
                   int x, int y, int cx, int cy,
                   UINT uFlags );

static CRITICAL_SECTION csWindow;
static DWORD dwForegroundThreadId = 0;
static CRITICAL_SECTION csForegroundThreadId;

#define MAGIC_NUM 0xCAFEEFAC
#define __GET_OBJECT_ADDR( h ) ( (_LPWINDATA)HANDLE_TO_PTR( h ) )
#define __GET_OBJECT_HANDLE( ptr ) PTR_TO_HANDLE( ptr )

// **************************************************
// ������static LONG _GetLong( const LPBYTE lpBuffer )
// ������
// 	IN lpBuffer - �ڴ��ַ
// ����ֵ��
//	long ��ֵ
// ����������
//	��һ���ڴ���õ�һ������ֵ
// ����: 
//	
// ************************************************

static LONG _GetLong( const LPBYTE lpBuffer )
{
    LONG d;
    memcpy( &d, lpBuffer, sizeof( LONG ) );
    return d;
}

// **************************************************
// ������static void _SetLong( LPBYTE lpBuffer, LONG d )
// ������
// 	IN lpBuffer - �ڴ��ַ
//	IN d - ����ֵ
// ����ֵ��
//	��
// ����������
//	����һ������ֵ���ڴ�
// ����: 
//	
// ************************************************

static void _SetLong( LPBYTE lpBuffer, LONG d )
{
    memcpy( lpBuffer, &d, sizeof( LONG ) );
}

// **************************************************
// ������static LRESULT WINAPI _ScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
// ������
// 	
// ����ֵ��
//	
// ����������
//	�����ڳ��˺�������ʵ����;
// ����: 
//	
// ************************************************

static LRESULT WINAPI _ScreenWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return 0;
}

// **************************************************
// ������void LockCSWindow( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	��ס����
// ����: 
//	����
// ************************************************

//       TEST-code
void LockCSWindow( void )
{
	EnterCriticalSection( &csWindow );
}

// **************************************************
// ������void UnlockCSWindow( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	����
// ����: 
//	
// ************************************************

void UnlockCSWindow( void )
{
	LeaveCriticalSection( &csWindow );
}

// **************************************************
// ������BOOL _InitWindowManager( HINSTANCE hInst )
// ������
// 	IN hInst - ����ʵ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ�����ڹ�����
// ����: 
//	�� gwme.c ����
// ************************************************

BOOL _InitWindowManager( HINSTANCE hInst )
{	
	//��ʼ�����ڳ�ͻ��
    InitializeCriticalSection( &csWindow );
	csWindow.lpcsName = "CS-WIN";
	//��ʼ��ǰ���������ó�ͻ��
	InitializeCriticalSection( &csForegroundThreadId );
	//��ʼ�����ڿ��
	//��ʼ����Ļ��
	memset( &classScreen , 0, sizeof( classScreen ) );    
	classScreen.objType = OBJ_CLASS;
	classScreen.hOwnerProcess = hgwmeProcess;
	classScreen.nRefCount = 1;
	classScreen.wc.hInstance = hInst;
    classScreen.wc.lpszClassName= "___winsys___";
    classScreen.wc.lpfnWndProc = (WNDPROC)_ScreenWndProc;
	//�����������
	lphwndRoot = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(hwndScreen) );
	
	//��ʼ����������
	memset( &hwndScreen, 0, sizeof( hwndScreen ) );
    hwndScreen.objType = OBJ_WINDOW;
    hwndScreen.rectWindow.right = WinSys_GetMetrics( SM_CXSCREEN );
    hwndScreen.rectWindow.bottom = WinSys_GetMetrics( SM_CYSCREEN );
    hwndScreen.rectClient.right = WinSys_GetMetrics( SM_CXSCREEN );
    hwndScreen.rectClient.bottom = WinSys_GetMetrics( SM_CYSCREEN );
    hwndScreen.dwMainStyle = WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	hwndScreen.lpClass = &classScreen;
    hwndScreen.lpfnWndProc = _ScreenWndProc;
	hwndScreen.hThis = (HANDLE)__GET_OBJECT_HANDLE(&hwndScreen);

    return TRUE;
}

// **************************************************
// ������DWORD LockForegroundThreadId( void )
// ������
// 	��
// ����ֵ��
//	���ص�ǰ��ǰ������ID
// ����������
//	��ס���õ���ǰ��ǰ������ID
// ����: 
//	
// ************************************************

DWORD LockForegroundThreadId( void )
{
	EnterCriticalSection( &csForegroundThreadId );
	return dwForegroundThreadId;
}

// **************************************************
// ������void UnlockForegroundThreadId( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	����
// ����: 
//	
// ************************************************

void UnlockForegroundThreadId( void )
{
	LeaveCriticalSection( &csForegroundThreadId );
}

// **************************************************
// ������void SetForegroundThreadId( DWORD dwNewId )
// ������
// 	IN dwNewId - �߳�ID
// ����ֵ��
//	��
// ����������
//	���õ�ǰ��ǰ���߳�
// ����: 
//	
// ************************************************

void SetForegroundThreadId( DWORD dwNewId )
{
	EnterCriticalSection( &csForegroundThreadId );
	if( dwForegroundThreadId )
	{
		Thread_ChangeRotate( dwForegroundThreadId, -2 ); 
	}
	if( dwNewId )
	{
		Thread_ChangeRotate( dwNewId, 2 ); 
	}
	dwForegroundThreadId = dwNewId;
	
	LeaveCriticalSection( &csForegroundThreadId );
}

// **************************************************
// ������void ExchangeForegroundThreadId( DWORD dwNewId, DWORD dwCompareId )
// ������
// 	IN dwNewId - �µ�ǰ���߳�ID
//	IN dwCompareId - ��Ҫ�Ƚϵ�ǰ���߳�ID
// ����ֵ��
//	��
// ����������
//	���統ǰ��ǰ���̵߳��� dwCompareId��������dwNewIdΪ��ǰ��ǰ���߳�
// ����: 
//	
// ************************************************

void ExchangeForegroundThreadId( DWORD dwNewId, DWORD dwCompareId )
{
	EnterCriticalSection( &csForegroundThreadId );
	if( dwCompareId == dwForegroundThreadId )
	{
		if( dwForegroundThreadId )
		{
			Thread_ChangeRotate( dwForegroundThreadId, -2 ); 
		}
		if( dwNewId )
		{
			Thread_ChangeRotate( dwNewId, 2 ); 
		}

	    dwForegroundThreadId = dwNewId;
	}
	LeaveCriticalSection( &csForegroundThreadId );
}

// **************************************************
// ������BOOL IsForegroundThread( void )
// ������
// 	��
// ����ֵ��
//	�����ǣ�����TRUE�����򣬷���FALSE
// ����������
//	�жϵ�ǰ���߳��Ƿ�ǰ���߳�
// ����: 
//	
// ************************************************

BOOL IsForegroundThread( void )
{
	return dwForegroundThreadId == GetCurrentThreadId() ? TRUE : FALSE;
}

// **************************************************
// ������BOOL _DeInitWindowManager( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷŴ��ڹ���������ϵͳ�˳�ʱ����øú�����
// ����: 
//	
// ************************************************

BOOL _DeInitWindowManager( void )
{
    BlockHeap_Free( hgwmeBlockHeap, 0, lphwndRoot, sizeof(_WINDATA) );
	DeleteCriticalSection( &csWindow );
	return TRUE;
}

// **************************************************
// ������static HWND GetDesktopExplore( void )
// ������
// 	��
// ����ֵ��
//	����ɹ������ص�ǰ�����洰�ھ�������򣬷���NULL
// ����������
//	�õ���ǰ�����洰�ھ��
// ����: 
//	
// ************************************************

static HWND GetDesktopExplore( void )
{
#ifdef INLINE_PROGRAM
	extern const char classDESKTOP[];	
#else
	const char classDESKTOP[] = "_DESKTOP_CLASS_";
#endif
	static HWND hDesktop = NULL;
	if( hDesktop == NULL )
	{
	    hDesktop = Win_Find( classDESKTOP, NULL );
	}
	return hDesktop;
}

// **************************************************
// ������BOOL WINAPI Win_AdjustRectEx( LPRECT lpRect, DWORD dwMainStyle, BOOL bMenu, DWORD dwSubStyle )
// ������
// 	IN/OUT lpRect - ���ھ���
//	IN dwMainStyle - ���ڷ��
//	IN bMenu - �����Ƿ�����˵�
//	IN dwSubStyle - ���ڵ���չ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���㣬�õ���ʵ�Ĵ��ھ���
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Win_AdjustRectEx( LPRECT lpRect, DWORD dwMainStyle, BOOL bMenu, DWORD dwSubStyle )
{
    lpRect->top -= _GetTopHeight( dwMainStyle, dwSubStyle );
    lpRect->left -= _GetLeftBorderWidth( dwMainStyle, dwSubStyle );
    lpRect->bottom += _GetBottomHight( dwMainStyle, dwSubStyle );
    lpRect->right += _GetRightWidth( dwMainStyle, dwSubStyle );
    if( bMenu )
        lpRect->top -= _GetMenuHeight();
    return TRUE;
}

// **************************************************
// ������HDC WINAPI WinGdi_GetDCEx(HWND hWnd, HRGN hrgn, DWORD flags)
// ������
//	IN hWnd-���ھ��
//	IN hrgn-��������
//	IN flags-��־��������
//��������DCX_WINDOW-���ش��������DC�����ǿͻ������DC
//��������DCX_CLIPCHILDREN-���õ��ô��ڵ��Ӵ���
//��������DCX_CLIPSIBLINGS-���õ��ô��ڵ��ֵܴ���
//��������DCX_EXCLUDERGN-���õ�hrgnClip����
//��������DCX_INTERSECTRGN-�ڽ�hrgnClip����
//��������DCX_VALIDATE-����DCX_INTERSECTUPDATE���ʱ����BeginPaint����
//��������DCX_PARENTCLIP-�ø����ڵĿ�������
// ����ֵ��
//	�ɹ�������DC���
//	���򣺷���NULL
// ����������
//	��ϵͳ����һ����ͼDC
// ����: 
//	ϵͳAPI
// ************************************************
#define DCX_LAYERED_UPDATE 0x80000000
HDC WINAPI WinGdi_GetDCEx(HWND hWnd, HRGN hrgn, DWORD flags)
{
    _LPWINDATA lpws;
    HRGN hrgnExpose, hrgnDraw;
    RECT rect;
    HDC hdc;	

    if( hWnd == NULL || hWnd == hwndScreen.hThis )
    {	//��Ļ���ڣ��õ�һ��ȫ��Ļ��DC
        rect = hwndScreen.rectWindow;
        hrgnDraw = WinRgn_CreateRect( rect.left, rect.top, rect.right, rect.bottom ); 
        hdc = GetPaintDC( hWnd, &rect, hrgnDraw, flags | DCF_GETDC_ALLOC );
#ifdef EML_DOS
        _HideMouse();
#endif
        return hdc;
    }
    lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {	//��Ч
		_LPWINDATA lpwsLayer;
		//
		if( (flags & DCX_NOLAYERED) == 0 )
		{
			if( ( lpwsLayer = NeedLayered( lpws ) ) )
			{
				return GetLayerWindowDC( lpws, lpwsLayer, flags );			
			}
		}		

        if( flags & DCX_WINDOW )
            rect = lpws->rectWindow;	//�ô��ھ���
        else
        {	//�ÿͻ�������
            rect = lpws->rectClient;
            OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
        }
		//��ʼ���ü���
        hrgnDraw = WinRgn_CreateRect( 0, 0, 0, 0 );

        ASSERT( hrgnDraw );
		if( IS_SHOWENABLE( lpws ) )  //Ϊ ShowDesktop�޸�
        {	//�õ����ڵı�¶����

			GetExposeRgn( hWnd, flags, &hrgnExpose );

            ASSERT( hrgnExpose );
			// hrgn��client ����, ƫ�Ƶ�screen
			if( hrgn )
			{
				WinRgn_Offset( hrgn, rect.left, rect.top );
			}
			
            if( flags & DCX_EXCLUDERGN )  //��Ҫ�ų� hrgn�� ��
                WinRgn_Combine( hrgnDraw, hrgnExpose, hrgn, RGN_DIFF );
            else if( flags & DCX_INTERSECTRGN )	//��Ҫ�ڽ� hrgn�� ��
                WinRgn_Combine( hrgnDraw, hrgnExpose, hrgn, RGN_AND );
            else	//��Ҫ���� hrgn�� ��
                WinRgn_Combine( hrgnDraw, hrgnExpose, 0, RGN_COPY );
            if( flags & DCX_VALIDATE )	//�Ƿ���Ҫ����ϵͳ�ü���
            {   // exlude hrgnExpose from update region
                if( flags & DCX_INTERSECTUPDATE )
                    WinRgn_Combine( hrgn, hrgn, hrgnExpose, RGN_DIFF );
            }
			// �ָ�hrgn��ԭʼ״̬
			if( hrgn )
			{
				WinRgn_Offset( hrgn, -rect.left, -rect.top );
			}
            WinGdi_DeleteObject( hrgnExpose );
        }
		//��ϵͳ����/����ʼ��һ��DC���� 
        hdc = GetPaintDC( hWnd, &rect, hrgnDraw, flags | DCF_GETDC_ALLOC );
    #ifdef EML_DOS
        _HideMouse();
    #endif
        return hdc;
    }
    return 0;
}

// **************************************************
// ������HDC WINAPI WinGdi_GetClientDC( HWND hWnd )
// ������
// 	IN hWnd-���ھ��
// ����ֵ��
//	�ɹ������ظô��ڿͻ���DC���
//	���򣺷���NULL
// ����������
//	�õ��ͻ���DC
// ����: 
//	ϵͳ API
// ************************************************

HDC WINAPI WinGdi_GetClientDC( HWND hWnd )
{
	DWORD dwStyle;
	DWORD f = 0;

	if( hWnd )
	{
		dwStyle = Win_GetLong( hWnd, GWL_STYLE );
		if( !(dwStyle & WS_CHILD) )
			f |= DCX_CLIPSIBLINGS;
		if( dwStyle & WS_CLIPCHILDREN )
			f |= DCX_CLIPCHILDREN;
		if( dwStyle & WS_CLIPSIBLINGS ) 
			f |= DCX_CLIPSIBLINGS;
	}
	else
	{
		dwStyle = DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN;
	}


    //return WinGdi_GetDCEx( hWnd, 0, DCX_CLIPSIBLINGS | DCX_CLIPCHILDREN );
    return WinGdi_GetDCEx( hWnd, 0, f );
}

// **************************************************
// ������HDC WINAPI WinGdi_GetWindowDC( HWND hWnd )
// ������
// 	IN hWnd-���ھ��
// ����ֵ��
//	�ɹ������ظô���DC���
//	���򣺷���NULL
// ����������
//	�õ�����DC
// ����: 
//	ϵͳAPI
// ************************************************

HDC WINAPI WinGdi_GetWindowDC( HWND hWnd )
{
	DWORD dwStyle;
	DWORD f = DCX_WINDOW;

	if( hWnd )
	{
		dwStyle = Win_GetLong( hWnd, GWL_STYLE );
		if( !(dwStyle & WS_CHILD) )
			f |= DCX_CLIPSIBLINGS;
		if( dwStyle & WS_CLIPCHILDREN )
			f |= DCX_CLIPCHILDREN;
		if( dwStyle & WS_CLIPSIBLINGS ) 
			f |= DCX_CLIPSIBLINGS;
	}
	else
	{
		dwStyle = DCX_CLIPSIBLINGS | DCX_WINDOW;
	}


    return WinGdi_GetDCEx( hWnd, 0, f );//DCX_CLIPSIBLINGS | DCX_WINDOW );
}

// **************************************************
// ������HDC WINAPI WinGdi_BeginPaint( HWND hWnd, LPPAINTSTRUCT lpPaint )
// ������
//	IN hWnd-���ھ��
//	IN lpps-PAINTSTRUCT �ṹָ��
// ����ֵ��
//	�ɹ������ظô���DC���
//	���򣺷���NULL
// ����������
//	׼�����ڵĻ�ͼ����,������DC���
// ����: 
//	ϵͳAPI
// ************************************************

HDC WINAPI WinGdi_BeginPaint( HWND hWnd, LPPAINTSTRUCT lpPaint )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    DWORD flags = 0;
    HRGN hrgn, hrgnUpdate, hrgnRemove;
    RECT rc;
	
    if( lpws )
    {	
		_LPWINDATA lpwsLayer;
		//		
		if( ( lpwsLayer = NeedLayered( lpws ) ) )
		{
			lpPaint->hdc = GetLayerWindowDC( lpws, lpwsLayer, 0 );
			ClearThreadQueue( 0, hWnd, WM_PAINT, WM_PAINT );			
			lpPaint->rcPaint = lpws->rectClient;
			OffsetRect( &lpPaint->rcPaint, -lpPaint->rcPaint.left, -lpPaint->rcPaint.top );
			lpPaint->fErase = (BOOL)WinMsg_Send( hWnd, WM_ERASEBKGND, (WPARAM)lpPaint->hdc, 0 );
			return lpPaint->hdc;
		}

		//���ݷ��õ���ص� DC
        if( lpws->dwMainStyle & WS_CLIPSIBLINGS )
            flags |= DCX_CLIPSIBLINGS;
        if( lpws->dwMainStyle & WS_CLIPCHILDREN )
            flags |= DCX_CLIPCHILDREN;
        if( !(lpws->dwMainStyle & WS_CHILD) )
            flags |= DCX_CLIPSIBLINGS;
        if( lpws->dwMainStyle & WS_CHILD )
            flags |= DCX_WINDOW;
		flags |= DCX_VALIDATE | DCX_INTERSECTUPDATE;
		//����ô��ڵ� WM_PAINT��Ϣ
		ClearThreadQueue( 0, hWnd, WM_PAINT, WM_PAINT );



		//EnterCriticalSection( &csWindow );
		//�����ͻ��
		LockCSWindow(); //2004-12-28
		//�õ��ô��ڵı�¶����
		GetExposeRgn( hWnd, flags, &hrgnUpdate );
		//�õ���������
		GetExposeRgn( hWnd, DCX_CLIPCHILDREN | DCX_WINDOW | flags, &hrgnRemove );

		UnlockCSWindow(); //2004-12-28
		
		//��סϵͳ��Ч����
		__LockRepaintRgn( &hrgn );  // 2003.1.25        
		//���ڱ�¶������ϵͳ��Ч�������ڽ�
		WinRgn_Combine( hrgnUpdate, hrgnUpdate, hrgn, RGN_AND );
		//��ϵͳ��Ч�������������
		WinRgn_Combine( hrgn, hrgn, hrgnRemove, RGN_DIFF );
		//����ϵͳ��Ч����
		__UnlockRepaintRgn();   //2003.1.25

		WinGdi_DeleteObject(hrgnRemove); 
		
		
        if( lpws->dwMainStyle & WS_CHILD )
        {	//�Ӵ���
			//�õ��ü�������
            WinRgn_GetBox(hrgnUpdate, &rc);
			//ת�����ͻ�����
            OffsetRect( &rc, -lpws->rectWindow.left, -lpws->rectWindow.top );
            if( !(rc.left >= lpws->rectClient.left &&
				rc.top >= lpws->rectClient.top &&
                rc.right <=  lpws->rectClient.right &&
                rc.bottom <= lpws->rectClient.bottom) )
            {
                HRGN hrgnNC;
				int xOffset = lpws->rectWindow.left;
				int yOffset = lpws->rectWindow.top;
				// hrgn��screen ����, ƫ�Ƶ�client
				WinRgn_Offset( hrgnUpdate, -xOffset, -yOffset );
				// ���ͷǿͻ���������Ϣ
                WinMsg_Send( hWnd, WM_NCPAINT, (WPARAM)hrgnUpdate, 0 );
				//�ָ�
				WinRgn_Offset( hrgnUpdate, xOffset, yOffset );
				//�õ��ǿͻ�������
                hrgnNC = _GetNClientRegion( lpws );
				//ȥ���ǿͻ�������
                WinRgn_Combine( hrgnUpdate, hrgnUpdate, hrgnNC, RGN_DIFF );
                WinGdi_DeleteObject( hrgnNC );
            }
        }
        else
        {
        }
		//�õ��ͻ������Σ��������꣩
        rc = lpws->rectClient;
		//ת������Ļ����
        OffsetRect( &rc, lpws->rectWindow.left, lpws->rectWindow.top );
        lpPaint->hdc = GetPaintDC( hWnd, &rc, hrgnUpdate, flags | DCF_BEGIN_PAINT_ALLOC );
		WinRgn_GetBox( hrgnUpdate, &lpPaint->rcPaint );
		//ת�����ͻ�������
		OffsetRect( &lpPaint->rcPaint, -(lpws->rectWindow.left+lpws->rectClient.left), -(lpws->rectWindow.top+lpws->rectClient.top) ); 
		
#ifdef EML_DOS
        _HideMouse();
#endif
		//������ܣ�����caret
		if( lpPaint->hdc )
			_HideCaret( hWnd, lpPaint->hdc );

		//��������		
        lpPaint->fErase = (BOOL)WinMsg_Send( hWnd, WM_ERASEBKGND, (WPARAM)lpPaint->hdc, 0 );
		
		
		
		return lpPaint->hdc;
    }
    return NULL;
}

// **************************************************
// ������int WINAPI WinGdi_ReleaseDC( HWND hWnd, HDC hdc )
// ������
// 	IN hWnd-���ھ��
//	IN hdc-DC���
// ����ֵ��
//	�ɹ�������1
//	���򣺷���0
// ����������
//	�ͷ�DC
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinGdi_ReleaseDC( HWND hWnd, HDC hdc )
{
	int retv; 
#ifdef EML_DOS
    _ShowMouse();
#endif
    retv = ReleasePaintDC( hWnd, hdc );
	
	return retv;
}

// **************************************************
// ������BOOL WINAPI WinGdi_EndPaint( HWND hWnd, const PAINTSTRUCT *lpPaint )
// ������
// 	IN hWnd-���ھ��
//	IN lpPaint- PAINTSTRUCT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�������ͷ�DC
// ����: 
//	ϵͳAPI
// ************************************************
#define DEBUG_WinGdi_EndPaint 0
BOOL WINAPI WinGdi_EndPaint( HWND hWnd, const PAINTSTRUCT *lpPaint )
{   
    if( lpPaint->hdc )
	{
        _ShowCaret( hWnd, lpPaint->hdc );
	    WinGdi_ReleaseDC( hWnd, lpPaint->hdc );
	}
	else
	{
		ASSERT( 0 );
		ERRORMSG( DEBUG_WinGdi_EndPaint, ( "error in WinGdi_EndPaint: invalid hdc.\r\n" ) );
	}
    
    return TRUE;
}

// **************************************************
// ������BOOL WINAPI Win_BringToTop( HWND hWnd )
// ������
// 	IN hWnd �C ���ھ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����ڷŵ�Z��Ķ���
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Win_BringToTop( HWND hWnd )
{
    return Win_SetPos( hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
}

// **************************************************
// ������LRESULT WINAPI Win_CallProc( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
// ������
// 	IN lpPrevWndFunc �C ǰһ��������Ϣ������
// 	IN hWnd �C ���ھ��
// 	IN msg �C ������Ϣ
// 	IN wParam �C ��һ������
// 	IN lParam �C �ڶ�������
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	�ص�������Ϣ������
// ����: 
//	ϵͳAPI
// ************************************************

LRESULT WINAPI Win_CallProc( WNDPROC lpPrevWndFunc, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    _LPWINDATA lpws;

    lpws = _GetHWNDPtr( hWnd );
    if( lpws )
	{
		if( (DWORD)lpPrevWndFunc == -1 )
			lpPrevWndFunc = lpws->lpClass->wc.lpfnWndProc; // ����Ĭ�ϵ���Ϣ������
	    return _CallWindowProc( lpws, lpPrevWndFunc, Msg, wParam, lParam );
	}
	return -1;
}

// **************************************************
// ������HWND WINAPI Win_ChildFromPoint( HWND hParent, int x, int y, UINT uFlags )
// ������
// 	IN hParent - ������
//	IN x - x ���꣨�ͻ����꣩
//	IN y - y ���꣨�ͻ����꣩
//	IN uFlags - ���ӱ�־
// ����ֵ��
//	����ɹ�������ӵ�иõ���Ӵ��ڣ����򣬷���NULL
// ����������
//	�õ�ӵ�иõ���Ӵ���
// ����: 
//	ϵͳAPI
// ************************************************

HWND WINAPI Win_ChildFromPoint( HWND hParent, int x, int y, UINT uFlags )
{
    _LPWINDATA lpwsParent = _GetHWNDPtr( hParent );
    HWND hwndHas = 0;
	POINT point;

	point.x = x;
	point.y = y;
    
	ASSERT( lpwsParent );

    if( lpwsParent )
    {	//��������Ч
		_LPWINDATA lpwsChild = lpwsParent->lpwsChild;
		//����ת������Ļ����
        Win_ClientToScreen( hParent, &point );
		//
        if( PtInRect( &lpwsParent->rectWindow, point ) )
        {   // ���ڸ������� search all child windows
            hwndHas = hParent;
			//�������е��Ӵ��ڣ�����Ƿ�ӵ�иõ�
            while( lpwsChild )
            {
                if( uFlags == CWP_ALL )
                {	//�������е��Ӵ���
                    if( PtInRect( &lpwsChild->rectWindow, point ) )
                    {	//�ڸ��Ӵ���
						if( lpwsChild->hrgnWindow )
						{
							if( PtInRegion( lpwsChild->hrgnWindow, point.x, point.y ) )
							{
								hwndHas = lpwsChild->hThis;
								break;
							}
						}
						else
						{
							hwndHas = lpwsChild->hThis;
							break;
						}
                    }
                }
                else
                {	//������������Ч���Ӵ���
                    if( (uFlags & CWP_SKIPINVISIBLE) &&
						IS_SHOWENABLE( lpwsChild ) == 0  //Ϊ ShowDesktop�޸�						
					  )
                        goto CFP_NEXTWIN;
                    if( (uFlags & CWP_SKIPDISABLED) &&
                        (lpwsChild->dwMainStyle & WS_DISABLED ) )
                        goto CFP_NEXTWIN;
                    if( PtInRect( &lpwsChild->rectWindow, point ) )
                    {	//����						
						if( lpwsChild->hrgnWindow )
						{
							if( PtInRegion( lpwsChild->hrgnWindow, point.x, point.y ) )
							{
								hwndHas = lpwsChild->hThis;
								break;							
							}
						}
						else
						{
							hwndHas = lpwsChild->hThis;
							break;
						}
                    }
                }
    CFP_NEXTWIN:
                //��һ��
				lpwsChild = lpwsChild->lpwsNext;
            }
        }
    }
    return hwndHas;
} 

// **************************************************
// ������BOOL WINAPI Win_ClientToScreen( HWND hWnd, LPPOINT lpPoint )
// ������
//	IN hWnd--���ھ��
//	IN lpPoint--�ͻ�����,���ɹ�����ʱ,������������Ļ����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͻ����굽��Ļ����
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Win_ClientToScreen( HWND hWnd, LPPOINT lpPoint )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );

    if( lpws )
    {
        lpPoint->x += lpws->rectClient.left + lpws->rectWindow.left;
        lpPoint->y += lpws->rectClient.top + lpws->rectWindow.top;
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Win_Close( HWND hWnd )
// ������
//	IN hWnd--���ھ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�رմ���
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Win_Close( HWND hWnd )
{   // minimize the window
    //Win_Show( hWnd, SW_MINIMIZE );
    return TRUE;
}

// **************************************************
// ������HWND WINAPI Win_CreateEx(                 
//						DWORD dwExStyle,
//						LPCBYTE lpClassName,
//						LPCBYTE lpText,
//						DWORD dwMainStyle,
//						int x, int y, int dx, int dy,
//						HWND hParent,
//						HMENU hMenu,
//						HINSTANCE hInstance,
//						LPVOID lpCreateParams )
// ������
//	IN dwExStyle-��չ��񣬰�����
//			WS_EX_TOPMOST-���㴰��
//	IN lpClassName-����
//	IN lpText-����
//	IN dwMainStyle-���
//			WS_OVERLAPPED-���ص�ʽ
//			WS_POPUP-����ʽ
//			WS_CHILD-�Ӵ���
//			WS_CLIPSIBLINGS-�ü��ֵܴ���
//			WS_CLIPCHILDREN-�ü��Ӵ���
//			WS_CAPTION-�б���
//			WS_BORDER-�е��߿�
//			WS_DLGFRAME-�Ի���߿�
//			WS_VSCROLL-��ֱ������
//			WS_HSCROLL-ˮƽ������
//			WS_SYSMENU-ϵͳ�˵�
//			WS_THICKFRAME-�����ű߿�
//			WS_MINIMIZEBOX-��С�ߴ��
//			WS_MAXIMIZEBOX-���ߴ��
//			WS_GROUP-���־
//			WS_TABSTOP-����TAB�����ܼ��̽���
//			WS_OVERLAPPEDWINDOW-��Ϸ��(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX |WS_MAXIMIZEBOX )
//			WS_POPUPWINDOW-��Ϸ��( WS_POPUP | WS_BORDER | WS_SYSMENU )
//			WS_CHILDWINDOW-��Ϸ��( WS_CHILD )
//			WS_VISIBLE-��ʼ����
//			WS_DISABLED-��ʼ������
//			WS_MINIMIZE-��ʼ��С��
//			WS_MAXIMIZE-��ʼ���
//			WS_EX_TOPMOST-���㴰��
//	IN x-���Ͻ�X����
//	IN y-���Ͻ�Y����
//	IN dx-���ڿ��
//	IN dy-���ڸ߶�
//	IN hParent-������
//	IN hMenu-�˵�
//	IN hInstance-ʵ��
//	IN lpCreateParam-��������

// ����ֵ��
//	�ɹ������ش��ھ��
//	���򣺷���NULL
// ����������
//	��������
// ����: 
//	ϵͳAPI
// ************************************************

#define DEBUG_WIN_CREATEEX 0
HWND WINAPI Win_CreateEx(                 
                    DWORD dwExStyle,
                    LPCBYTE lpClassName,
		            LPCBYTE lpText,
                    DWORD dwMainStyle,
		            int x, int y, int dx, int dy,
		            HWND hParent,
		            HMENU hMenu,
		            HINSTANCE hInstance,
		            LPVOID lpCreateParams )
{
    _LPWINDATA lpws, lpwsParent = NULL;
    HANDLE hmem;
    CREATESTRUCT cs, *lpcs;
    BOOL bVisible;
	_LPREGCLASS lpClass;
	//������
	if( lpClassName == NULL )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}

	if( dx < 0 || dy < 0 )
	{
		dx = 0; dy = 0;
		DEBUGMSG( DEBUG_WIN_CREATEEX, ( "CreateWindow Warn: window width(%d) or height(%d) < 0,class(%s),title(%s).\r\n", dx, dy, lpClassName ? (char*)lpClassName : "", lpText ? (char*)lpText : "" ) );
	}

    DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b1.\r\n" ) );

	bVisible = (dwMainStyle & WS_VISIBLE) ? 1 : 0;
    dwMainStyle &= ~WS_VISIBLE;
	if( hParent )
		lpwsParent = _GetHWNDPtr( hParent );
	
    if( (dwMainStyle & WS_POPUP) && lpwsParent )//hParent )
    {
        lpwsParent = _GetSafeOwner( lpwsParent );//, 0 );
    }
    else if( dwMainStyle & WS_CHILD )
    {    // �Ӵ��� child window

		if( dwExStyle & WS_EX_LAYERED )
		{
			ASSERT_NOTIFY( 0, "CreateWindow Error: child window can't has WS_EX_LAYERED.\r\n" );
			SetLastError( ERROR_INVALID_PARAMETER );
            return NULL;
		}

        if( lpwsParent == NULL )
		{
			ASSERT_NOTIFY( 0, "CreateWindow Error: child window no parent\r\n" );
			SetLastError( ERROR_INVALID_PARAMETER );
            return NULL;
		}
    }
    else
    {    // ws_overlapp
        lpwsParent = NULL;
    }

	DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b3.\r\n" ) );
	//�õ������
	lpClass = GetClassPtr( (LPCSTR)lpClassName, hInstance );
	if( lpClass == NULL )
	{	//��Ч������಻����
		WARNMSG( DEBUG_WIN_CREATEEX, ("Invalid class or the class not exist!:(ptr=0x%x.),%s.\r\n", lpClassName, lpClassName ? lpClassName : "" ) );
		SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}

	DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b4.\r\n" ) );

	// ���䴰�ڶ��� alloc winstruct mem, align to long( 4 byte )
    lpws = (_LPWINDATA)_AllocWinObjectHandle( sizeof (_WINDATA) + (lpClass->wc.cbWndExtra + 3) / 4 * 4 );
    if( lpws == NULL )
	{
        return NULL;
	}
	/*  ��Ҫʱ��alloc
	if( dwExStyle & WS_EX_LAYERED )
	{	// 
		lpws->lpLayer = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_LAYERED_DATA) );
		if( lpws->lpLayer == NULL )
			_DeleteWinObject( lpws );
		return NULL;
	}
	*/

	// �õ����ڶ���ľ�� 2004-02-24
	hmem = (HANDLE)__GET_OBJECT_HANDLE( lpws );//PTR_TO_HANDLE( lpws );

    DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b5.\r\n" ) );
   
	//��ʼ�����ڶ��� ================now init the lpws struct begin============
	lpws->lpClass = lpClass;
	lpws->hThis = hmem;
    lpws->lpfnWndProc = lpClass->wc.lpfnWndProc;
	lpws->hOwnerProcess = GetCallerProcess();
	lpws->hSmallIcon = lpClass->wc.hIcon;
	lpws->hBigIcon = NULL; // reserve for future;
	//��ʼ�������ṹ
    cs.x = x;
    cs.y = y;
    cs.cx = dx; cs.cy = dy;
    cs.lpszClass = (LPCSTR)lpClassName;
    cs.lpszName = (LPCSTR)lpText;
    cs.style = dwMainStyle;
    cs.dwExStyle = dwExStyle;
    cs.hInstance = hInstance;
    cs.hMenu = hMenu;
    cs.hParent = lpwsParent ? lpwsParent->hThis : NULL;
    cs.lpCreateParams = lpCreateParams;
	//��ʼ�����ڶ���
    _SetWinParam( lpws, &cs );

    DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b6.\r\n" ) );

	// �����ڲ��봰������ insert this window to parent
	LockCSWindow();

	if( dwMainStyle & WS_CHILD )
    {	//�Ӵ���
		Link_InsertToParent( (_LPWINDATA)HWND_BOTTOM, lpws, lpwsParent );//
    }
    else
    {	//
        if( (lpws->dwMainStyle & WS_POPUP) && hParent )
        {	//����ʽ����
			if( lpwsParent->dwExStyle & WS_EX_TOPMOST )
                lpws->dwExStyle |= WS_EX_TOPMOST;
			Interlock_Increment( (LPLONG)&lpwsParent->uOwnCount );
        }
        Link_InsertWindowToScreen( lpws );
		lpws->lpwsOwner = lpws->lpwsParent = lpwsParent;
    }
	UnlockCSWindow();

	DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b7.\r\n" ) );
	//���Ӷ��������
	AddClassRef( lpClass );

	// ���� WS_NCCREATE and WS_CREATE to wnd proc
	lpcs = (CREATESTRUCT*)MapPtrToProcess( &cs, hgwmeProcess );
	if( WinMsg_Send( hmem, WM_NCCREATE, 0, (LPARAM)lpcs ) == TRUE )
    {   // get client bound
		RECT rc = lpws->rectWindow;
		//�õ��ͻ�����
        _CalcClientBound( hmem, &rc );
		lpws->rectClient = rc;
		DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b9.\r\n" ) );
		//���� WM_CREATE ��Ϣ
        if( WinMsg_Send( hmem, WM_CREATE, 0, (LPARAM)lpcs ) == 0 )
        {
			DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b10.\r\n" ) );
            if( bVisible )
            {	//��ʾ
				if( lpws->dwMainStyle & WS_CHILD )
                    Win_Show( hmem, SW_SHOWNORMAL );
				else
					Win_SetForeground( hmem );
            }
			DEBUGMSG( DEBUG_WIN_CREATEEX, ( "b11.\r\n" ) );
            return hmem;
        }
		else
		{
			WARNMSG( DEBUG_WIN_CREATEEX, ( "WM_CREATE: return !0.\r\n" ) );
		}
    }
	else
	{
		WARNMSG( DEBUG_WIN_CREATEEX, ( "WM_NCCREATE: return FALSE,lpClassName(%s).\r\n", (DWORD)lpClassName ) );
	}
	//ʧ��
	WARNMSG( DEBUG_WIN_CREATEEX, ( "b12.\r\n" ) );
    Win_Destroy( hmem );
    hmem = NULL;
	ASSERT( 0 );
    return hmem;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

//static BOOL _DestroyPopupWindow( HWND hOwner, DWORD dwCurThreadID )
static BOOL _DestroyPopupWindow( _LPWINDATA lpwsOwner, DWORD dwCurThreadID, UINT uiFlag )
{
    
	//EnterCriticalSection( &csWindow );  //
	LockCSWindow();
	{
		_LPWINDATA lpwsPopup = hwndScreen.lpwsChild;//hwndScreen.hChild;//Win_Get( HWND_DESKTOP, GW_CHILD );
		_LPWINDATA lpwsTemp;
		//_LPWINDATA lpwsOwner = _GetHWNDPtr( hOwner );
		
		while( lpwsPopup && lpwsOwner->uOwnCount )
		{
			//_LPWINDATA lpws = _GetHWNDPtr( hwndPopup );
			if( lpwsPopup->lpwsOwner == lpwsOwner )
			{   // yes, destroy it
				lpwsTemp = lpwsPopup->lpwsNext;

				//LeaveCriticalSection( &csWindow );
				UnlockCSWindow();

				_Win_Destroy( lpwsPopup, dwCurThreadID, WDF_HIDE | uiFlag );//TRUE );
				
				//EnterCriticalSection( &csWindow );
				LockCSWindow();

				//lpwsPopuphwndPopup = hwndTemp;
				if( Win_IsWindow( lpwsTemp->hThis ) == FALSE )
					lpwsPopup = hwndScreen.lpwsChild;  // retry all
				else
					lpwsPopup = lpwsTemp;
			}
			else
				lpwsPopup = lpwsPopup->lpwsNext;
		}
		ASSERT( lpwsOwner->uOwnCount == 0 );
	}
	//LeaveCriticalSection( &csWindow );
	UnlockCSWindow();
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL _Win_Destroy( _LPWINDATA lpws, DWORD dwCurThreadID, UINT uiFlag )//BOOL bHide )
{
    {
		LPGWEDATA lpgwe;
		
		if( lpws->dwThreadID != dwCurThreadID )
		{
			ASSERT( lpws->dwThreadID != dwCurThreadID );
			return FALSE;
		}
		if( (uiFlag & WDF_CLEAR) == 0 )
		{
			WinMsg_Send( lpws->hThis, WM_DESTROY, 0, 0 );
			WinMsg_Send( lpws->hThis, WM_NCDESTROY, 0, 0 );
		}
		
			
		if( (uiFlag & WDF_HIDE ) && 
			IS_SHOWENABLE( lpws )  // 2003-08-05, Ϊ ShowDesktop�޸�
		  )
		{
			Win_SetPos( 
			    lpws->hThis, 
			    NULL,
			    0, 0, 0, 0,
			    SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW | SWP_NOACTIVATE );
		}

        while( lpws->lpwsChild )
        {   // the parent is hided, not need hide child window again
            _Win_Destroy( lpws->lpwsChild, dwCurThreadID, (uiFlag & WDF_CLEAR) );
        }

        if( lpws->uOwnCount )
            _DestroyPopupWindow( lpws, dwCurThreadID, (uiFlag & WDF_CLEAR) );

        ASSERT( lpws->dwThreadID );
		lpgwe = GetGWEPtr( lpws->dwThreadID );

		if( lpgwe )
		{			
		    if( lpgwe->hwndActive == lpws->hThis )  //LN:2003-04-23,����
			{
		        //Interlock_CompareExchange( &dwForegroundThreadID, 0, lpws->dwThreadID );//LN:2003-04-23,����, //LN,2003-06-03, DEL
				ExchangeForegroundThreadId( 0, lpws->dwThreadID );//LN,2003-06-03, ADD
			}

			if( lpgwe->hwndFocus == lpws->hThis )
				lpgwe->hwndFocus = NULL;
			if( lpgwe->hwndActive == lpws->hThis )
				lpgwe->hwndActive = NULL;
			if( lpgwe->hwndCapture == lpws->hThis )
				lpgwe->hwndCapture = NULL;
		}

		//Interlock_CompareExchange( &dwForegroundThreadID, 0, lpws->dwThreadID );//LN:2003-04-23,ɾ��

        if( (lpws->dwMainStyle & WS_POPUP) && lpws->lpwsOwner )
        {
			Interlock_Decrement( (LPLONG)&lpws->lpwsOwner->uOwnCount );
        }
        // clear message relate to hWnd
		LockCSWindow();
        Link_RemoveWindow( lpws );		
		UnlockCSWindow();
		DecClassRef( lpws->lpClass );
		if( lpws->hrgnWindow )
			DeleteObject( lpws->hrgnWindow );
		if( lpws->lpLayer )
		{
			if( lpws->lpLayer->hBitmap )
				DeleteObject( lpws->lpLayer->hBitmap );
			BlockHeap_Free( hgwmeBlockHeap, 0, lpws->lpLayer, sizeof(_LAYERED_DATA) );
		}

        _DeleteWinObject( lpws );		
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_Destroy( HWND hWnd )
{
	_LPWINDATA lpws;
	DWORD dwThreadID = GetCurrentThreadId();
	BOOL bRetv = FALSE;
	HWND hwndFore = NULL;//Win_GetForegroundWindow

	//EnterCriticalSection( &csWindow );

	lpws = _GetHWNDPtr( hWnd );
	if( lpws )
	{
		if( !(lpws->dwMainStyle & WS_CHILD) &&
			IS_SHOWENABLE( lpws ) && // 2003-08-05, Ϊ ShowDesktop�޸�
			!(lpws->dwMainStyle & WS_DISABLED) )
		{
			hwndFore = Win_GetForeground();
		}
		
		if( dwThreadID == lpws->dwThreadID )
		{
			DWORD dwStyle = lpws->dwMainStyle;
			HWND hwndDesktop = GetDesktopExplore();
			
			bRetv = _Win_Destroy( lpws, dwThreadID, WDF_HIDE );
			if( hwndDesktop != hWnd && !(dwStyle & WS_CHILD) )
			{
				AddMsgToThreadQueue( hwndDesktop, WM_SHELLNOTIFY, SN_SETFOREGROUNDWINDOW, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
			}
		}
		if( hwndFore == hWnd )
		{   //���������µ�ǰ������
			//EnterCriticalSection( &csWindow );
			LockCSWindow();
			
			lpws = hwndScreen.lpwsChild;
			while( lpws )
			{
				if( !(lpws->dwMainStyle & WS_CHILD) &&
					IS_SHOWENABLE( lpws ) && // 2003-08-05, Ϊ ShowDesktop�޸�
					!(lpws->dwMainStyle & WS_DISABLED) )
				{
					break;
				}
				lpws = lpws->lpwsNext;
			}
			
			//LeaveCriticalSection( &csWindow );
			UnlockCSWindow();
			if( lpws )
				Win_SetForeground( lpws->hThis );
		}
	}
	// LN, 2003.05.14-end
	return bRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_Enable( HWND hWnd, BOOL bEnable )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    DWORD b = FALSE;
    
    if( lpws )
    {
		LPGWEDATA lpgwe = GetGWEPtr( lpws->dwThreadID );
		ASSERT( lpws->dwThreadID );
		if( lpgwe )
		{
			
			DWORD dwStyle = lpws->dwMainStyle;
			b = dwStyle & WS_DISABLED;
			if( bEnable )
				dwStyle &= ~WS_DISABLED;
			else
				dwStyle |= WS_DISABLED;
			//_SetState( lpws, WS_DISABLED, !bEnable );
			if( dwStyle != lpws->dwMainStyle )
			{
				//Win_SetLong( hWnd, GWL_STYLE, dwStyle );
				lpws->dwMainStyle = dwStyle;
				WinMsg_Send( hWnd, WM_NCPAINT, 1, NULL );

				if( bEnable == FALSE )
				{	
					//if( hwndActive && (hwndFocusWindow == hWnd || Win_IsChild( hWnd, hwndFocusWindow )) )
					if( lpgwe->hwndFocus == hWnd ||
						( lpgwe->hwndFocus && Win_IsChild( hWnd, lpgwe->hwndFocus ) ) )
						Win_SetFocus( NULL );
					if(  lpgwe->hwndActive == hWnd ||
						( lpgwe->hwndActive && Win_IsChild( hWnd, lpgwe->hwndActive ) ) )
						Win_SetActive( NULL );
					if( lpgwe->hwndCapture == hWnd || 
						( lpgwe->hwndCapture && Win_IsChild( hWnd, lpgwe->hwndCapture ) ) )
						Win_SetCapture( NULL );
				}
				WinMsg_Send( hWnd, WM_ENABLE, (WPARAM)bEnable, 0 );
			}
		}
    }
    return b;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL _Win_Enum( _LPWINDATA lpwsChild, BOOL bEnumChild, CALLBACKDATA * lpcd, LPARAM lParam )
{
    _LPWINDATA lpwsNext;
	BOOL bRetv = TRUE;

    while( lpwsChild )
    {
		if( lpwsChild == NULL )
		{
			bRetv = FALSE;
			SetLastError( ERROR_INVALID_HANDLE );
			break;			
		}

        lpwsNext = lpwsChild->lpwsNext;

		//LeaveCriticalSection( &csWindow );	//2004-03-31, call by caller
		if( lpcd->hProcess )
		{
			lpcd->dwArg0 = (DWORD)lpwsChild->hThis;
			bRetv = Sys_ImplementCallBack4( lpcd, lParam );
		}
		else  // inner call
			bRetv = ((WNDENUMPROC)lpcd->lpfn)( lpwsChild->hThis, lParam );

		//EnterCriticalSection( &csWindow );//2004-03-31, call by caller
	    
		if( bRetv == FALSE )
			break;

		if( bEnumChild && 
			lpwsChild->lpwsChild &&
			Win_IsWindow( lpwsChild->hThis ) )
		{
			bRetv = _Win_Enum( lpwsChild->lpwsChild, bEnumChild, lpcd, lParam );
		}

		if( bRetv == TRUE )
        {   // if return TRUE, contine enumerate            
            if( lpwsNext )
            {
                if( Win_IsWindow( lpwsNext->hThis ) )
                    lpwsChild = lpwsNext;
                else // retry
                    break;//lpws = hwndScreen.lpwsChild;
            }
            else
			{
                break;
			}
        }
		else
			break;
    }

	//LeaveCriticalSection( &csWindow );//2004-03-31, call by caller
    
    return bRetv;//TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL DoEnumWindow( _LPWINDATA lpwsChild, BOOL bEnumChild, CALLBACKDATA * lpcd, LPARAM lParam )
{
	BOOL bRetv = FALSE;
	if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
	{
	    bRetv = _Win_Enum( lpwsChild, bEnumChild, lpcd, lParam );
		Sys_ReleaseException();
	}
	return bRetv;
}

// **************************************************
// ������BOOL WINAPI _Win_EnumChildWithProcess( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam, HANDLE hProcess )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ú���ֻ�ܱ��ڲ�����
// ����: 
//	
// ************************************************

BOOL WINAPI _Win_EnumChildWithProcess( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam, HANDLE hProcess )
{
	_LPWINDATA lpws = _GetHWNDPtr( hParent );
    CALLBACKDATA cb;
	
	cb.hProcess = hProcess;
	cb.lpfn = (FARPROC)lpEnumFunc;

	//EnterCriticalSection( &csWindow );
	LockCSWindow();
	
	if( lpws == NULL )
		lpws = hwndScreen.lpwsChild;
	else
		lpws = lpws->lpwsChild;

	DoEnumWindow( lpws, TRUE, &cb, lParam );
		
	//LeaveCriticalSection( &csWindow );
	UnlockCSWindow();

	return TRUE;
}

// **************************************************
// ������BOOL WINAPI _Win_EnumChildInside( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ú���ֻ�ܱ��ڲ�����
// ����: 
//	
// ************************************************

BOOL WINAPI _Win_EnumChildInside( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam )
{
	return _Win_EnumChildWithProcess( hParent, lpEnumFunc, lParam, NULL );
}


// **************************************************
// ������BOOL WINAPI Win_EnumChild( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ú���ֻ�ܱ�ϵͳ���ã��ڲ����ܵ��øú���
// ����: 
//	
// ************************************************

BOOL WINAPI Win_EnumChild( HWND hParent, WNDENUMPROC lpEnumFunc, LPARAM lParam )
{
	return _Win_EnumChildWithProcess( hParent, lpEnumFunc, lParam, GetCallerProcess() );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_EnumTask( HTASK hTask, WNDENUMPROC lpEnumFunc, LPARAM lParam )
{
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL DoEnumTopLevelWindow( WNDENUMPROC lpProc, LPARAM lParam, HANDLE hOwnerProcess )
{
	CALLBACKDATA cd;
	BOOL bRetv;
	
	cd.hProcess = hOwnerProcess;
	cd.lpfn = (FARPROC)lpProc;
	
	//EnterCriticalSection( &csWindow );  //�п��ܵ�������

	bRetv = DoEnumWindow( hwndScreen.lpwsChild, FALSE, &cd, lParam );

	//LeaveCriticalSection( &csWindow );

	return bRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ú���ֻ�ܱ��ڲ�����
// ����: 
//	
// ************************************************

BOOL WINAPI _Win_EnumInside( WNDENUMPROC lpEnumFunc, LPARAM lParam )
{
	return DoEnumTopLevelWindow( lpEnumFunc, lParam, NULL );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ú���ֻ�ܱ�ϵͳ���ã��ڲ����ܵ��øú���
// ����: 
//	
// ************************************************

BOOL WINAPI Win_Enum( WNDENUMPROC lpEnumFunc, LPARAM lParam )
{
	return DoEnumTopLevelWindow( lpEnumFunc, lParam, GetCallerProcess() );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_Flash( HWND hWnd, BOOL bInvert )
{
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

struct DOFIND{
	LPCTSTR lpcClassName;
	LPCTSTR lpcWindowName;
	HWND hWnd;
};

static BOOL CALLBACK DoFind(HWND hWnd, LPARAM lParam)
{
    _LPWINDATA lpws;
	struct DOFIND * lpFind = (struct DOFIND *)lParam;

    lpws = _GetHWNDPtr( hWnd );
	if( lpFind->lpcClassName == NULL ||
		stricmp( lpws->lpClass->wc.lpszClassName, lpFind->lpcClassName ) == 0 )
	{
		if( lpFind->lpcWindowName )
		{	
			DWORD dwOldPerm = SetProcPermissions( ~0 );  //�õ��Խ��̵Ĵ�ȡȨ��
			if( lpws->lpWinText &&
				stricmp( lpws->lpWinText, lpFind->lpcWindowName ) == 0 )
			{
				SetProcPermissions( dwOldPerm );  //�ָ��Խ��̵Ĵ�ȡȨ��
				lpFind->hWnd = hWnd;
				return FALSE;   // stop enmu
			}
			SetProcPermissions( dwOldPerm );  //�ָ��Խ��̵Ĵ�ȡȨ��
		}
		else
		{
			lpFind->hWnd = hWnd;
			return FALSE;   // stop emu
		}
	}
	return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_Find( LPCTSTR lpcClassName, LPCTSTR lpcWindowName )
{
	struct DOFIND findData;

	CALLBACKDATA cd;


	cd.hProcess = NULL;
	cd.lpfn = (FARPROC)DoFind;

	findData.hWnd = NULL;
	findData.lpcClassName = lpcClassName;
	findData.lpcWindowName = lpcWindowName;

	//EnterCriticalSection( &csWindow );
	LockCSWindow();
	
	DoEnumWindow( hwndScreen.lpwsChild, FALSE, &cd, (LPARAM)&findData );

	//LeaveCriticalSection( &csWindow );
	UnlockCSWindow();
	

	return findData.hWnd;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_GetClientRect( HWND hWnd, LPRECT lpRect )
{
    _LPWINDATA lpws;
    
	lpws = (_LPWINDATA)_GetHWNDPtr( hWnd );

    if( lpws )
    {
        lpRect->left = lpRect->top = 0;
        lpRect->right = lpws->rectClient.right - lpws->rectClient.left;
        lpRect->bottom = lpws->rectClient.bottom - lpws->rectClient.top;
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetDesktop( void )
{
	return hwndScreen.hThis;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetFocus( void )
{
	LPGWEDATA lpgwe = GetGWEPtr( GetCurrentThreadId() );

	if( lpgwe )
        return lpgwe->hwndFocus;
	return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetParent( HWND hWnd )
{
    _LPWINDATA lpws = (_LPWINDATA)_GetHWNDPtr( hWnd );
    if( lpws )
    {
        if( lpws->lpwsParent )
		    return (lpws->lpwsParent->hThis);
    }
    return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetNext( HWND hWnd, UINT wCmd )
{
	_LPWINDATA lpwsWndRetv = NULL;
	_LPWINDATA lpws = _GetHWNDPtr(hWnd);

	if( lpws )
	{
		//EnterCriticalSection( &csWindow );
		LockCSWindow();
		if( lpws )
		{
			if( wCmd == GW_HWNDNEXT )
				lpwsWndRetv = lpws->lpwsNext;
			else
				lpwsWndRetv = lpws->lpwsPrev;
		}
		//LeaveCriticalSection( &csWindow );
		UnlockCSWindow();
	}
	if( lpwsWndRetv )
	    return lpwsWndRetv->hThis;
	return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetTop( HWND hWnd )
{
    _LPWINDATA lpws = (_LPWINDATA)_GetHWNDPtr( hWnd );
    if( lpws && lpws->lpwsChild )
	{
        return lpws->lpwsChild->hThis;
	}
    return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_Get( HWND hWnd, UINT uCmd )
{
    _LPWINDATA lpws;
	_LPWINDATA lpwsWndRetv = NULL;

    //EnterCriticalSection( &csWindow );
	LockCSWindow();

    lpws = _GetHWNDPtr( hWnd );
    
    if( lpws )
    {
        if( uCmd == GW_CHILD )
            lpwsWndRetv = lpws->lpwsChild;
        else if( uCmd == GW_HWNDFIRST )
        {
            lpwsWndRetv = GET_Z_ORDER_UP( lpws )->lpwsChild;
        }
        else if( uCmd == GW_HWNDLAST )
        {
            while( lpws->lpwsNext )
            {
        // if next window is exist and next window has WS_CHILD style, continue
                lpws = lpws->lpwsNext;
            }
			lpwsWndRetv = lpws;
        }
        else if( uCmd == GW_HWNDNEXT )
        {
            lpwsWndRetv = lpws->lpwsNext;//_NextWindow( hWnd );
        }
        else if( uCmd == GW_HWNDPREV )
        {
            lpwsWndRetv = lpws->lpwsPrev;//_PrevWindow( hWnd );
        }
        else if( uCmd == GW_OWNER )
        {
            lpwsWndRetv = lpws->lpwsOwner;
        }
        else
        {
            ASSERT( 0 );
        }
    }

    //LeaveCriticalSection( &csWindow );
	UnlockCSWindow();
	if( lpwsWndRetv )
        return lpwsWndRetv->hThis;
	return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

LONG WINAPI Win_GetLong( HWND hWnd, int nIndex )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
		//        WNDCLASS wc;
		
        switch( nIndex )
        {
        case GWL_MAINSTYLE:
            return lpws->dwMainStyle;
        case GWL_EXSTYLE:
            return lpws->dwExStyle;
		case GWL_TEXT_PTR:
			return (LONG)UnMapPtr( lpws->lpWinText );
		case GWL_ID:
			return (LONG)lpws->hMenu;
		case GWL_VSCROLL:
			return (LONG)lpws->lpvScrollBar;
		case GWL_HSCROLL:
			return (LONG)lpws->lphScrollBar;
		case GWL_HICONSM:
			return (LONG)lpws->hSmallIcon;
		case GWL_HICON:
			return (LONG)lpws->hBigIcon;
		case GWL_HWNDPARENT:
			return (LONG)( lpws->lpwsParent ? lpws->lpwsParent->hThis : NULL );
		case GWL_USERDATA:
			return lpws->dwUserData;
		case GWL_HINSTANCE:
			return (LONG)lpws->hinst;
        case GWL_WNDPROC:
			if( lpws->lpfnWndProc == lpws->lpClass->wc.lpfnWndProc )
				return -1;
			else
                return (LONG)(lpws->lpfnWndProc);
		default:
			
			if( lpws->lpClass == NULL )
				return 0;
			if( (int)( nIndex + sizeof( LONG ) ) > lpws->lpClass->wc.cbWndExtra )
				return 0;
			else
				return _GetLong( (LPBYTE)lpws + sizeof( _WINDATA ) + nIndex );
        }
    }
    return 0;
}

// **************************************************
// ������LONG WINAPI _Win_SetLong( HWND hWnd, int nIndex, LONG dwNewLong )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static LONG WINAPI Win_SetLongWithProcess( HWND hWnd, int nIndex, LONG dwNewLong, HANDLE hCallerProcess )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        LONG oldLong = 0;
// check valid
    
        switch( nIndex )
        {
            case GWL_MAINSTYLE:
				{
					STYLESTRUCT ss, * lpss;

					if( lpws->dwMainStyle != (DWORD)dwNewLong )
					{
						ss.styleNew = lpws->dwExStyle;
						ss.styleOld = dwNewLong;
						
						lpss = (STYLESTRUCT*)MapPtrToProcess( &ss, hgwmeProcess );
						
						WinMsg_Send( hWnd, WM_STYLECHANGING, GWL_STYLE, (LPARAM)lpss );
						
						oldLong = lpws->dwMainStyle;
						lpws->dwMainStyle = ss.styleOld;
						WinMsg_Send( hWnd, WM_STYLECHANGED, GWL_STYLE, (LPARAM)lpss );

						if( (oldLong & 0xffff0000) != (lpws->dwMainStyle & 0xffff0000) )
							Win_SetPos( hWnd, NULL, 0, 0, 0, 0, 
								          SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |SWP_NOACTIVATE | SWP_FRAMECHANGED );

					}
					else
						oldLong = lpws->dwMainStyle;
				}
                break;
			case GWL_VSCROLL:
				oldLong = (LONG)lpws->lpvScrollBar;
				lpws->lpvScrollBar = (LPVOID)dwNewLong;
				break;
			case GWL_HSCROLL:
				oldLong = (LONG)lpws->lphScrollBar;
				lpws->lphScrollBar = (LPVOID)dwNewLong;
				break;
            case GWL_USERDATA:
                oldLong = lpws->dwUserData;
                lpws->dwUserData = dwNewLong;
                break;
            case GWL_ID:
                oldLong = (long)lpws->hMenu;
                lpws->hMenu = (HMENU)dwNewLong;
                break;
            case GWL_EXSTYLE:
				{
					STYLESTRUCT ss, *lpss;
					
					if( lpws->dwExStyle != (DWORD)dwNewLong )
					{
						ss.styleNew = dwNewLong;
						ss.styleOld = lpws->dwMainStyle;
						
						lpss = (STYLESTRUCT*)MapPtrToProcess( &ss, hgwmeProcess );
						
						WinMsg_Send( hWnd, WM_STYLECHANGING, GWL_EXSTYLE, (LPARAM)lpss );
						
						oldLong = lpws->dwExStyle;
						lpws->dwExStyle = ss.styleNew;						
						WinMsg_Send( hWnd, WM_STYLECHANGED, GWL_EXSTYLE, (LPARAM)lpss );

						if( (DWORD)oldLong != lpws->dwExStyle )
							Win_SetPos( hWnd, NULL, 0, 0, 0, 0, 
								          SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |SWP_NOACTIVATE | SWP_FRAMECHANGED );

					}
					else
						oldLong = lpws->dwExStyle;
				}
                break;
		    case GWL_TEXT_PTR:
				if( hCallerProcess == lpws->hOwnerProcess )
				{
					oldLong = (LONG)UnMapPtr( lpws->lpWinText );
					
					lpws->lpWinText = MapPtrToProcess( (LPVOID)dwNewLong, hCallerProcess );
				}
				else
				{
					ASSERT( 0 );
				}
				break;
			case GWL_HICONSM:
				oldLong = (LONG)lpws->hSmallIcon;
				lpws->hSmallIcon = (HICON)dwNewLong;
				break;
			case GWL_HICON:
				oldLong = (LONG)lpws->hBigIcon;
				lpws->hBigIcon = (HICON)dwNewLong;
				break;
            case GWL_WNDPROC:
				if( lpws->hOwnerProcess == hCallerProcess ||
					hCallerProcess == hgwmeProcess	//2004-10-13 �ض����ڲ��ĵ��ã�������Ǵ��
					)
				//2004-10-13-end
				{
                    oldLong = (LONG)(lpws->lpfnWndProc);
					// EnterCS  ???? same thread ????
					if( dwNewLong == -1 ) // LN, 2003.05.30
					{
						lpws->uState &= ~WINS_INUSERPROC;// LN, 2003.05.30
						dwNewLong = (LONG)lpws->lpClass->wc.lpfnWndProc;
					}
					else
						lpws->uState |= WINS_INUSERPROC;// LN, 2003.05.30

                    lpws->lpfnWndProc = (WNDPROC)dwNewLong;
					//LeaveCS

				}
				//else if( hCallerProcess == hgwmeProcess )
				//{  
				//}
                break;
            case GWL_HINSTANCE:
                oldLong = (LONG)lpws->hinst;
                lpws->hinst = (HINSTANCE)dwNewLong;
                break;
            default:
				if( lpws->lpClass == NULL )
                    return 0;
                if( (int)(nIndex + sizeof( LONG )) > (int)(lpws->lpClass->wc.cbWndExtra) )
                    return 0;
                else
                {
                    oldLong = _GetLong( (LPBYTE)lpws + sizeof( _WINDATA ) + nIndex );
                    _SetLong( (LPBYTE)lpws + sizeof( _WINDATA ) + nIndex, dwNewLong );
                }

        }   
        return oldLong;
    }
    return 0;
}

// **************************************************
// ������LONG WINAPI Win_SetLongInside( HWND hWnd, int nIndex, LONG dwNewLong )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ڲ�����
// ����: 
//	
// ************************************************
LONG WINAPI Win_SetLongInside( HWND hWnd, int nIndex, LONG dwNewLong )
{
	return Win_SetLongWithProcess( hWnd, nIndex, dwNewLong, hgwmeProcess );
}

// **************************************************
// ������LONG WINAPI Win_SetLongInside( HWND hWnd, int nIndex, LONG dwNewLong )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ϵͳ����
// ����: 
//	
// ************************************************
LONG WINAPI Win_SetLong( HWND hWnd, int nIndex, LONG dwNewLong )
{
	return Win_SetLongWithProcess( hWnd, nIndex, dwNewLong, GetCallerProcess() );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_GetWindowRect( HWND hWnd, LPRECT lpRect )
{
    _LPWINDATA lpws  = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        *lpRect = lpws->rectWindow;
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

int WINAPI Win_GetText( HWND hWnd, LPSTR lpstr, int nMaxCount )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    int retv = 0;
    *lpstr = 0;
	if( lpws )
    {
		if( lpws->hOwnerProcess == GetCallerProcess() )  
		{
			WinMsg_SendTimeout( hWnd, WM_GETTEXT, nMaxCount, (LPARAM)lpstr, SMTO_BLOCK, 5000, &retv );
		}
		else if( lpws->lpWinText )
		{
			DWORD dwOldPerm;
			dwOldPerm = SetProcPermissions( ~0 );  //�õ��Խ��̵Ĵ�ȡȨ��
			strncpy( lpstr, lpws->lpWinText, nMaxCount );
			retv = strlen( lpstr );
			SetProcPermissions( dwOldPerm );  //�ָ��Խ��̵Ĵ�ȡȨ��
		}
    }
    return retv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

int WINAPI Win_GetTextLength( HWND hWnd )
{
    return (int)WinMsg_Send( hWnd, WM_GETTEXTLENGTH, 0, 0 );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL _AddPaintRegion( HRGN hrgnUpdate )
{
    extern int CALLBACK __PutPaintRgn( HRGN hrgn );
    __PutPaintRgn( hrgnUpdate );
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

void _AddPaintWndRgn( _LPWINDATA lpws, HRGN hrgnUpdate, BOOL fUpdateChild, DWORD uiThreadID )
{
    HRGN hrgn;
//    int retv;    

    if( lpws && !(lpws->dwMainStyle & WS_CHILD) )
    {
        HRGN hsys;
		if( lpws->dwThreadID == uiThreadID )
		{
			int xOffset = lpws->rectWindow.left;
			int yOffset = lpws->rectWindow.top;

			// 2003-09-16, ���Ӵ���, hrgn��screen ����, ƫ�Ƶ�client
			WinRgn_Offset( hrgnUpdate, -xOffset, -yOffset );
			//
            WinMsg_Send( lpws->hThis, WM_NCPAINT, (WPARAM)hrgnUpdate, 0 );

			// 2003-09-16, ���Ӵ���, �ָ�
			WinRgn_Offset( hrgnUpdate, xOffset, yOffset );
			//
		}
		else
		{
			AddMsgToThreadQueue( lpws->hThis, WM_NCPAINT, 1, 0, ATQ_ONLYONE | QS_POSTMESSAGE );			
		}
			
        hrgn  = _GetNClientRegion( lpws );
        WinRgn_Combine( hrgn, hrgn, hrgnUpdate, RGN_AND );

		__LockRepaintRgn( &hsys );  

        WinRgn_Combine( hsys, hsys, hrgn, RGN_DIFF );

		__UnlockRepaintRgn();  

        WinGdi_DeleteObject( hrgn );
    }
	

    if( lpws )
    {
        AddMsgToThreadQueue( lpws->hThis, WM_PAINT, 0, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
    }
    if( fUpdateChild )
    {   // put child windows to paint queue
        _LPWINDATA lpwsChild;

		//EnterCriticalSection( &csWindow );
		LockCSWindow();
        
        if( lpws  )
            lpwsChild = lpws->lpwsChild;
        else
            lpwsChild = hwndScreen.lpwsChild;            

        if( lpwsChild )
        {
            do
            {            
				lpws = lpwsChild;

				if( 
					IS_SHOWENABLE( lpws ) && // 2003-08-05, Ϊ ShowDesktop�޸�
					( lpws->dwExStyle & WS_EX_LAYERED ) == 0 &&
                    WinRgn_RectInRegion(hrgnUpdate, &lpws->rectWindow) == TRUE )
                {
					int retv;
                    hrgn = WinRgn_CreateRect( lpws->rectWindow.left, lpws->rectWindow.top, lpws->rectWindow.right, lpws->rectWindow.bottom );
					if( lpws->hrgnWindow )
						WinRgn_Combine( hrgn, hrgn, lpws->hrgnWindow, RGN_AND );

                    retv = WinRgn_Combine( hrgn, hrgn, hrgnUpdate, RGN_AND );
                    if( retv == NULLREGION )
						goto _NEXT;
                        
                    //if( !(lpws->dwMainStyle & WS_CHILD) )
                        //WinRgn_SetRect( hrgn, lpws->rectWindow.left, lpws->rectWindow.top, lpws->rectWindow.right, lpws->rectWindow.bottom );
                    retv = WinRgn_Combine( hrgnUpdate, hrgnUpdate, hrgn, RGN_DIFF ); //!!!!!!! remove to top line

					//LeaveCriticalSection( &csWindow );

                    if( lpws->dwMainStyle & WS_CHILD )
                    {
                        if( lpws->lpwsChild )
						{
                            _AddPaintWndRgn( lpws, hrgn, TRUE, uiThreadID );
						}
                        else
						{
                            AddMsgToThreadQueue( lpws->hThis, WM_PAINT, 0, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
						}
                    }
                    else
					{
                        _AddPaintWndRgn( lpws, hrgn, lpws->lpwsChild != 0, uiThreadID );
					}

					//EnterCriticalSection( &csWindow );

					//!!!!!!! remove to top line-2005-09-25W
                    //if( !(lpws->dwMainStyle & WS_CHILD) )
                      //  WinRgn_SetRect( hrgn, lpws->rectWindow.left, lpws->rectWindow.top, lpws->rectWindow.right, lpws->rectWindow.bottom );
                    //retv = WinRgn_Combine( hrgnUpdate, hrgnUpdate, hrgn, RGN_DIFF ); //!!!!!!! remove to top line
					//
                    WinGdi_DeleteObject( hrgn );
                    if( retv == NULLREGION )
                        break;
                }
_NEXT:

                lpwsChild = lpws->lpwsNext;
            }while( lpwsChild );
        }

		//LeaveCriticalSection( &csWindow );
		UnlockCSWindow();
    }
	
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_InvalidateRect(HWND hWnd, LPCRECT lprc, BOOL bErase)
{
	
	if( hWnd )
	{
		_LPWINDATA lpws = _GetHWNDPtr( hWnd );
		if( lpws )
		{
			HRGN hrgnExpose, hrgn = 0;
			int retv = FALSE;
			RECT rectClip;
			
			if( IS_SHOWENABLE( lpws ) )  // 2003-08-05, Ϊ ShowDesktop�޸�
			{
				DWORD fClip = (lpws->dwMainStyle & WS_CLIPCHILDREN) ? DCX_CLIPCHILDREN : 0;
				fClip |= DCX_CLIPSIBLINGS;
				
				GetExposeRgn( hWnd, fClip, &hrgnExpose );
				
				ASSERT( hrgnExpose );
				retv = _GetRgnInfo( hrgnExpose );
				if( retv == NULLREGION || retv == ERROR )
				{
					WinGdi_DeleteObject(  hrgnExpose );
					return TRUE;
				}
				
				if( lprc )
				{
					rectClip = *lprc;
					// map to screen coordinate
					OffsetRect( &rectClip,
						lpws->rectClient.left + lpws->rectWindow.left,
						lpws->rectClient.top + lpws->rectWindow.top );
					hrgn = WinRgn_CreateRectIndirect( &rectClip );
					ASSERT( hrgn );
					retv = WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_AND );
					WinGdi_DeleteObject( hrgn );
					
				}
				if( retv != NULLREGION && retv != ERROR )
				{
					_AddPaintRegion( hrgnExpose );
					_AddPaintWndRgn( lpws, hrgnExpose, !(lpws->dwMainStyle & WS_CLIPCHILDREN), GetCurrentThreadId() );
				}
				
				
				if( bErase )
				{
					HDC hdc;
					rectClip = lpws->rectClient;
					OffsetRect( &rectClip, lpws->rectWindow.left, lpws->rectWindow.top );
					
					hdc = GetPaintDC( hWnd, &rectClip, hrgnExpose, fClip );
					WinMsg_Send( hWnd, WM_ERASEBKGND, (WPARAM)hdc, 0 );
					ReleasePaintDC( hWnd, hdc );
				}
				else  // the _ReleasePaintDC above will do WinGdi_DeleteObject(  hrgnExpose );, so if bErase is TRUE , I do'nt delete hrgnExpose
					WinGdi_DeleteObject(  hrgnExpose );
			}
			
			return !(retv == ERROR);
		}
	}
	else
	{   // system invalidates and redraws all windows
		RECT rc = hwndScreen.rectWindow;
		HRGN hrgnExpose;
		if( lprc )
			IntersectRect( &rc, &rc, lprc );
		
	    hrgnExpose = WinRgn_CreateRectIndirect( &rc );
		_AddPaintRegion( hrgnExpose );
		_AddPaintWndRgn( NULL, hrgnExpose, TRUE, GetCurrentThreadId() );
		WinGdi_DeleteObject(  hrgnExpose );
		return TRUE;
	}
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_InvalidateRgn(HWND hWnd, HRGN hrgn, BOOL bErase)
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        HRGN  hrgnExpose;
        RECT rect;
        int retv = FALSE;
   
		if( IS_SHOWENABLE( lpws ) ) // 2003-08-05, Ϊ ShowDesktop�޸�
        {
			GetExposeRgn( hWnd, DCX_CLIPSIBLINGS, &hrgnExpose );

            ASSERT( hrgnExpose );
            // map hrgnExpose to client coordinate
            WinRgn_Offset( hrgnExpose,
                       -(lpws->rectClient.left + lpws->rectWindow.left),
                       -(lpws->rectClient.top + lpws->rectWindow.top) );

            if( hrgn == 0 )
            {
                Win_GetClientRect( hWnd, &rect );
                hrgn = WinRgn_CreateRectIndirect( &rect );
                ASSERT( hrgn );
                retv = WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_AND );
                WinGdi_DeleteObject( hrgn );
            }
            else
                retv = WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_AND );

            if( retv != NULLREGION && retv != ERROR )
            { // map to screen coordinate
                WinRgn_Offset( hrgnExpose,
                          (lpws->rectClient.left + lpws->rectWindow.left),
                          (lpws->rectClient.top + lpws->rectWindow.top) );
                _AddPaintRegion(hrgnExpose);
                _AddPaintWndRgn( lpws, hrgnExpose, !(lpws->dwMainStyle & WS_CLIPCHILDREN), GetCurrentThreadId() );
            }

            if( bErase )
            {
                HDC hdc;
                rect = lpws->rectClient;
                OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
    
                hdc = GetPaintDC( hWnd, &rect, hrgnExpose, DCX_CLIPSIBLINGS );
                WinMsg_Send( hWnd, WM_ERASEBKGND, (WPARAM)hdc, 0 );
                ReleasePaintDC( hWnd, hdc );
            }
            else  // the _ReleasePaintDC above will do WinGdi_DeleteObject(  hrgnExpose );, so if bErase is TRUE , I do'nt delete hrgnExpose
                WinGdi_DeleteObject(  hrgnExpose );
        }
        return !(retv == ERROR);
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static HRGN _GetNClientRegion( _LPWINDATA lpws )
{

    HRGN hrgn0, hrgn1;
    RECT rect;

    hrgn0 = WinRgn_CreateRectIndirect( &lpws->rectWindow );
	if( lpws->hrgnWindow )
		WinRgn_Combine( hrgn0, hrgn0, lpws->hrgnWindow, RGN_AND );
    rect = lpws->rectClient;
    OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
    hrgn1 = WinRgn_CreateRectIndirect( &rect );
    ASSERT( hrgn0 && hrgn1 );
    WinRgn_Combine( hrgn0, hrgn0, hrgn1, RGN_DIFF );
    WinGdi_DeleteObject( hrgn1 );
    return hrgn0;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_ValidateRect( HWND hWnd, const RECT * lpRect )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );

    if( lpws )
    {
        HRGN hrgn, hrgnClient, hrgnRepaint;    
        RECT rect;

        {
            rect = *lpRect;
            OffsetRect( &rect,
                        lpws->rectWindow.left+lpws->rectClient.left,
                        lpws->rectWindow.top+lpws->rectClient.top );
            hrgnClient = WinRgn_CreateRectIndirect( &rect );
            ASSERT( hrgnClient );
			
			GetExposeRgn( hWnd, DCX_CLIPSIBLINGS, &hrgn );

            WinRgn_Combine( hrgn, hrgn, hrgnClient, RGN_AND );

			if( __LockRepaintRgn( &hrgnRepaint ) )  //2003.1.25
                WinRgn_Combine( hrgnRepaint, hrgnRepaint, hrgn, RGN_DIFF );
			__UnlockRepaintRgn();  //2003.1.25

          
            WinGdi_DeleteObject( hrgnClient );
            WinGdi_DeleteObject( hrgn );
        }

        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_ValidateRgn( HWND hWnd, HRGN hrgn )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        HRGN hrgnExpose, hrgnRepaint;
		
        {
            // map to screen
            WinRgn_Offset( hrgn,
                        lpws->rectWindow.left+lpws->rectClient.left,
                        lpws->rectWindow.top+lpws->rectClient.top );

			GetExposeRgn( hWnd, DCX_CLIPSIBLINGS, &hrgnExpose );

            WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_AND );

			if( __LockRepaintRgn( &hrgnRepaint ) )  
                WinRgn_Combine( hrgnRepaint, hrgnRepaint, hrgnExpose, RGN_DIFF );
			__UnlockRepaintRgn();   

            WinGdi_DeleteObject( hrgnExpose );
            WinRgn_Offset( hrgn,
                       -(lpws->rectWindow.left+lpws->rectClient.left),
                       -(lpws->rectWindow.top+lpws->rectClient.top) );
        }
		

        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_IsChild( HWND hParent, HWND hWnd )
{
	BOOL bRetv = FALSE;
	{
		_LPWINDATA lpws = (_LPWINDATA)_GetHWNDPtr( hWnd );
		_LPWINDATA lpwsParent = (_LPWINDATA)_GetHWNDPtr( hParent );
		
		if( lpws && lpwsParent )
		{
			//LockCSWindow();  //2004-12-28, 1.û�б�Ҫ(read only)��2.queue ����øú��������ܵ���lock

			while( (lpws->dwMainStyle & WS_CHILD) && lpws->lpwsParent )
			{
				if( lpws->lpwsParent == lpwsParent )
				{
					bRetv = TRUE;
					break;
				}
				lpws = lpws->lpwsParent;
			}

			//UnlockCSWindow();//2004-12-28, û�б�Ҫ��queue ����øú��������ܵ���lock
		}
	}
    return bRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_IsIconic( HWND hWnd )
{
    if( _GetHWNDPtr( hWnd )->dwMainStyle & WS_MINIMIZE )
        return TRUE;
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_IsWindow( HWND hWnd )
{
	_LPWINDATA lpws = __GET_OBJECT_ADDR( hWnd );

    if( lpws )
    {
        if( lpws->objType == OBJ_WINDOW && lpws->hThis == hWnd )
		{	
			return TRUE;
		}
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_IsEnabled( HWND hWnd )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        return (lpws->dwMainStyle & WS_DISABLED ) == 0;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_IsVisible( HWND hWnd )
{
    BOOL bRetv = FALSE;
	_LPWINDATA lpws = _GetHWNDPtr( hWnd );

    if( lpws )
    {
		LockCSWindow();
        do{
            if( !(lpws->dwMainStyle & WS_VISIBLE) )
                break;
            if( lpws->dwMainStyle & WS_CHILD )
                lpws = lpws->lpwsParent;
            else
			{
				bRetv = TRUE;
                break;
			}
        }while( lpws );
		UnlockCSWindow();
    }
    return bRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_IsZoomed( HWND hWnd )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
        return (lpws->dwMainStyle & WS_MAXIMIZE ) == 0;
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

DWORD WINAPI Win_MapPoints(HWND hwndFrom, HWND hwndTo, LPPOINT lppt, UINT cpt)
{
    POINT ptFrom, ptTo = { 0, 0 };
    int xoffset, yoffset;
    UINT i;

    ptFrom = ptTo;
    _MapAbsCoordinate( hwndFrom, &ptFrom );
    _MapAbsCoordinate( hwndTo, &ptTo );

    xoffset = ptFrom.x - ptTo.x;
    yoffset = ptFrom.y - ptTo.y;
    for( i = 0; i < cpt; i++ )
    {
        (lppt + i)->x += xoffset;
        (lppt + i)->y += yoffset;
    }
    return MAKELONG( xoffset, yoffset );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_Move( HWND hWnd, int x, int y, int cx, int cy, BOOL bRepaint )
{
    if( bRepaint )
        return Win_SetPos( hWnd, 0, x, y, cx, cy, SWP_NOZORDER );
    else
        return Win_SetPos( hWnd, 0, x, y, cx, cy, SWP_NOREDRAW|SWP_NOZORDER );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_ReleaseCapture(void)
{
    HWND hOld = NULL;
#ifdef EML_WIN32
	extern void ReleaseEmlCapture(void);
#endif

    LPGWEDATA lpgwe = GetGWEPtr( GetCurrentThreadId() );

	if( lpgwe )
	{
		if( (hOld = lpgwe->hwndCapture) != NULL )
		{
			lpgwe->hwndCapture = NULL;
			WinMsg_Send( hOld, WM_CAPTURECHANGED, 0, 0 );
		}
	}
#ifdef EML_WIN32
	ReleaseEmlCapture();
#endif
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_ScreenToClient( HWND hWnd, LPPOINT lpPoint )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        lpPoint->x -= (lpws->rectWindow.left + lpws->rectClient.left);
        lpPoint->y -= (lpws->rectWindow.top + lpws->rectClient.top);
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetForeground(void)
{
	DWORD dwID = LockForegroundThreadId();
    LPGWEDATA lpgwe = GetGWEPtr( dwID );
	HWND hWndRetv = NULL;

	if( lpgwe )
	{
		if( lpgwe->hwndActive != NULL )
		     hWndRetv = lpgwe->hwndActive;
		else if( lpgwe->hwndFocus != NULL )
			 hWndRetv = lpgwe->hwndFocus;
	}
	UnlockForegroundThreadId();
	return hWndRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_SetForeground(HWND hWnd)
{
    _LPWINDATA lpws;
	LPGWEDATA lpgwe;
	HWND hOldActive;
	HWND hActive;
	
	
    
    lpws = _GetHWNDPtr( hWnd );
	
	if( lpws == NULL )
		return FALSE;

    hActive = hWnd = Win_GetTopLevelParent(hWnd);
	lpws = _GetHWNDPtr( hWnd );

	if( lpws == NULL || (lpws->dwExStyle & WS_EX_INPUTWINDOW) ) 
		return FALSE;

	if( lpws->dwThreadID != GetCurrentThreadId() )
	{  //����ͬһ���߳� 
		WinMsg_Post( hWnd, WM_SYSSETFOREGROUND, 0, 0 );
		return TRUE;
	}
	// ��ͬ�̵߳ĵ���
    //LN:2003-04-10 begin ��dwForegroundThreadID���ı�ʱ��WM_KILLFOCUS��Ϣ��
	//��ǰ�Ļ����
	// 
	//
	{  
		DWORD dwId = LockForegroundThreadId();
		if( dwId &&
			dwId != lpws->dwThreadID  )
		{	//ʧȥǰ����һ���߳��뱾�̲߳���ͬһ���߳�
			LPGWEDATA lpgwe = GetGWEPtr( dwId );

			UnlockForegroundThreadId();  // 2003-07-30, ��lock, ��
			                             //  �������п��ܵ��»���
			
			if( lpgwe )
			{
				if( lpgwe->hwndActive )
					DoSetActive( lpgwe, NULL, FALSE );	//��ͬ��
				if( lpgwe->hwndFocus )
					DoSetFocus( lpgwe, NULL, FALSE );	//��ͬ��
			}
		}
		else
		{
			UnlockForegroundThreadId();
		}
		
	}
	//LN:2003-04-10 end

	lpgwe = GetGWEPtr( lpws->dwThreadID );
	ASSERT( lpws->dwThreadID );
	if( lpgwe )
	{
		hOldActive = lpgwe->hwndActive;
		Win_SetPos( hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW );
		
		if( lpws->dwMainStyle & WS_DISABLED )
		{
			if( lpgwe->hwndActive )
			{
				WinMsg_Send( lpgwe->hwndActive, WM_NCACTIVATE, TRUE, 0 );
				WinMsg_Send( lpgwe->hwndActive, WM_ACTIVATE, 
					MAKELONG( WA_ACTIVE, _IsMinimized( lpgwe->hwndActive ) ), 
					NULL );
				hActive = lpgwe->hwndActive;
			}
		}
		else if( hOldActive == hWnd )
		{   // if hOldActive == hWnd , i must active myself
			WinMsg_Send( hWnd, WM_NCACTIVATE, TRUE, 0 );
			WinMsg_Send( hWnd, WM_ACTIVATE, MAKELONG( WA_ACTIVE, _IsMinimized( hWnd ) ), NULL );
		}
		
		SetForegroundThreadId( lpws->dwThreadID );
	}

	return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static HWND DoSetActive( LPGWEDATA lpgwe, _LPWINDATA lpws, BOOL bSync )
{
	HWND hwndOld = lpgwe->hwndActive;
	HWND hwndNewActive = lpws ? lpws->hThis : NULL;
	
	if( hwndOld == hwndNewActive )
	{
		// get desktop chance to get the change
		AddMsgToThreadQueue( GetDesktopExplore(), WM_SHELLNOTIFY, SN_SETFOREGROUNDWINDOW, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
		return NULL;
	}
	// active diff
	if( hwndOld )
	{
		if( bSync )
		{
			WinMsg_Send( hwndOld, WM_NCACTIVATE, FALSE, 0 );
			WinMsg_Send( hwndOld, WM_ACTIVATE, MAKELONG( WA_INACTIVE, _IsMinimized( hwndOld ) ), (LPARAM)hwndNewActive );
		}
		else
		{
			WinMsg_Post( hwndOld, WM_NCACTIVATE, FALSE, 0 );
			WinMsg_Post( hwndOld, WM_ACTIVATE, MAKELONG( WA_INACTIVE, _IsMinimized( hwndOld ) ), (LPARAM)hwndNewActive );
		}
	}
    lpgwe->hwndActive = hwndNewActive;
	if( hwndNewActive )
	{
		if( bSync )
		{
            WinMsg_Send( hwndNewActive, WM_NCACTIVATE, TRUE, 0 );
            WinMsg_Send( hwndNewActive, WM_ACTIVATE, MAKELONG( WA_ACTIVE, _IsMinimized( hwndNewActive ) ), (LPARAM)hwndOld );
		}
		else
		{
            WinMsg_Post( hwndNewActive, WM_NCACTIVATE, TRUE, 0 );
            WinMsg_Post( hwndNewActive, WM_ACTIVATE, MAKELONG( WA_ACTIVE, _IsMinimized( hwndNewActive ) ), (LPARAM)hwndOld );
		}
	}

    AddMsgToThreadQueue( GetDesktopExplore(), WM_SHELLNOTIFY, SN_SETFOREGROUNDWINDOW, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
	return hwndOld;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_SetActive(HWND hWnd)
{
    HWND hwndOld = NULL;
	DWORD dwThreadID;
	LPGWEDATA lpgwe;

    if( hWnd )
    {
        _LPWINDATA lpws = _GetHWNDPtr( hWnd );

		if( lpws == 0 || 
			(lpws->dwMainStyle & WS_CHILD) ||
			(lpws->dwMainStyle & WS_DISABLED) ||
			(lpws->dwMainStyle & WS_VISIBLE) == 0 )
			return NULL;  // error

		dwThreadID = GetCurrentThreadId();

		if( dwThreadID != lpws->dwThreadID )
		{
			return NULL;  // not same thread
		}

		lpgwe = GetGWEPtr( dwThreadID );
		if( lpgwe )
		{
			hwndOld = DoSetActive( lpgwe, lpws, TRUE );
		}
    }
	else
	{
		lpgwe = GetGWEPtr( GetCurrentThreadId() );

        if( lpgwe )
		{
			hwndOld = DoSetActive( lpgwe, NULL, TRUE );
		}
	}
    return hwndOld;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetActive(void)
{
	LPGWEDATA lpgwe = GetGWEPtr( GetCurrentThreadId() );

	if( lpgwe )
        return lpgwe->hwndActive;
	return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_SetCapture( HWND hWnd )
{
#ifdef EML_WIN32
    extern void SetEmlCapture(void);
#endif
    HWND hOld = NULL;

	DWORD dwThreadID = GetCurrentThreadId();
	LPGWEDATA lpgwe = GetGWEPtr( dwThreadID );

	if( hWnd )
	{
		_LPWINDATA lpws = _GetHWNDPtr( hWnd );	
		if( lpws == NULL || lpws->dwThreadID != dwThreadID )
			return NULL;
	}
	if( lpgwe )
	{
		if( lpgwe->hwndCapture )
			WinMsg_Send( lpgwe->hwndCapture, WM_CAPTURECHANGED, 0, (LPARAM)hWnd );
		hOld = lpgwe->hwndCapture;
	    lpgwe->hwndCapture = hWnd;
	}

#ifdef EML_WIN32
	SetEmlCapture();
#endif
    return hOld;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetCapture( void )
{
	DWORD dwThreadID = GetCurrentThreadId();
	LPGWEDATA lpgwe = GetGWEPtr( dwThreadID );

	if( lpgwe )
        return lpgwe->hwndCapture;
	return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static HWND DoSetFocus( LPGWEDATA lpgwe, _LPWINDATA lpws, BOOL bSync )
{
	HWND hOldFocus = lpgwe->hwndFocus;
	HWND hNewFocus;

	if( lpws && (lpws->dwExStyle & WS_EX_NOFOCUS) )		//����ҪҪ���ý���
		return hOldFocus;
	hNewFocus = lpws ? lpws->hThis : NULL;
	//

	if ( hNewFocus == lpgwe->hwndFocus )
		return lpgwe->hwndFocus; // same,nothing to do
	
	if( lpgwe->hwndFocus )
	{
		if( bSync )
	        WinMsg_Send( hOldFocus, WM_KILLFOCUS, (WPARAM)hNewFocus,  0 );
		else
			WinMsg_Post( hOldFocus, WM_KILLFOCUS, (WPARAM)hNewFocus,  0 );
	}
	
	lpgwe->hwndFocus = hNewFocus;
	if( hNewFocus )
	{
		if( bSync )
	        WinMsg_Send( hNewFocus, WM_SETFOCUS, (WPARAM)hOldFocus, 0 );
		else
			WinMsg_Post( hNewFocus, WM_SETFOCUS, (WPARAM)hOldFocus, 0 );
	}
	return hOldFocus;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_SetFocus( HWND hWnd )
{
    HWND hOld = NULL;
	_LPWINDATA lpws = NULL;
	DWORD dwThreadID = GetCurrentThreadId();
	LPGWEDATA lpgwe = GetGWEPtr( dwThreadID );

	if( hWnd )
	{
		lpws = _GetHWNDPtr( hWnd );
		if( lpws == NULL || lpws->dwThreadID != dwThreadID )
			return NULL;
	}

	if( lpgwe )
	{		
		hOld = DoSetFocus( lpgwe, lpws, TRUE );
	}
    return hOld;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_SetParent( HWND hChild, HWND hNewParent )
{
    _LPWINDATA lpws = _GetHWNDPtr( hChild );
	_LPWINDATA lpwsParent = _GetHWNDPtr( hNewParent );
    if( lpws && lpwsParent )
    {
        _LPWINDATA lpwsOld;

        Win_Show( hChild, SW_HIDE );

		lpwsOld = lpws->lpwsParent;
        if( lpws->dwMainStyle & WS_CHILD )
        {
			LockCSWindow();
            if( Link_RemoveWindow( lpws ) )
                Link_InsertToParent( (_LPWINDATA)HWND_TOP, lpws, lpwsParent );//hNewParent );
			UnlockCSWindow();
        }
        else
        {
			if( lpws->lpwsOwner )
				Interlock_Decrement( (LPLONG)&lpws->lpwsOwner->uOwnCount );

            lpws->lpwsParent = lpwsParent;
            lpws->lpwsOwner = lpwsParent;

			Interlock_Increment( (LPLONG)&lpwsParent->uOwnCount );

        }
        Win_Show( hChild, SW_SHOW );
        return lpwsOld ? lpwsOld->hThis : NULL;
    }
    return 0;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

typedef struct _RESTORE_DATA
{
	DWORD dwThreadId;
}RESTORE_DATA;
static BOOL CALLBACK DoRestore( HWND hWnd, LPARAM lParam )
{
	{
		RESTORE_DATA * lprd = (RESTORE_DATA*)lParam;
		_LPWINDATA lpws;
		lpws = _GetHWNDPtr( hWnd );
		
		if( lpws && 
			lpws->dwThreadID == lprd->dwThreadId )
		{
			_SetState( lpws, WS_MINIMIZE, FALSE );
		}
	}
	return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_Show( HWND hWnd, int cmd )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        BOOL ret = lpws->dwMainStyle & WS_VISIBLE;
		if( cmd == SW_SHOWNORMAL )
		{
			//RETAILMSG( 1, ( "c1\r\n" ) );
			Win_SetPos( hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
			//RETAILMSG( 1, ( "c2\r\n" ) );
		}
		else if( cmd == SW_RESTORE )
		{
			CALLBACKDATA cd;
			RESTORE_DATA rd;
			
			cd.hProcess = NULL;
			cd.lpfn = (FARPROC)DoRestore;
			rd.dwThreadId = lpws->dwThreadID;
			
			LockCSWindow();
			
			DoEnumWindow( hwndScreen.lpwsChild, FALSE, &cd, (LPARAM)&rd );
			
			UnlockCSWindow();
			Win_SetForeground( hWnd );

			Win_InvalidateRect( NULL, NULL, FALSE );

		}
		else if( cmd == SW_SHOW ||
			     cmd == SW_HIDE )
		{
			if( ( (lpws->dwMainStyle & WS_VISIBLE) && cmd == SW_HIDE ) ||
				( (lpws->dwMainStyle & WS_VISIBLE) == 0 && cmd == SW_SHOW ) )
				WinMsg_Send( hWnd, WM_SHOWWINDOW, (cmd & SW_SHOW), 0 );
		}
        return ret;
    }
    return FALSE;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL CALLBACK MoveChild( HWND hWnd, LPARAM lParam )
{
    OffsetRect( &_GetHWNDPtr( hWnd )->rectWindow, (short)LOWORD( lParam ), (short)HIWORD( lParam ) );
    return TRUE;
}

// **************************************************
// ������static _LPWINDATA GetTopOwner( _LPWINDATA lpws, UINT uiNeedLevel, UINT * lpuiRealLevel )
// ������
// 	IN lpws - ���ڶ���ָ��
//	IN uiNeedLevel - ��Ҫ�ļ������Ϊ -1,����
//	OUT lpuiRealLevel - ʵ�ʵļ���, ����Ϊ NULL
// ����ֵ��
//	����ɹ������ش��ڶ���ָ�룻���򣬷���NULL
// ����������
//	�õ�����ӵ���ߣ� �����߱��� call LockCSWindow();
// ����: 
//	
// ************************************************

static _LPWINDATA GetTopOwner( _LPWINDATA lpws, UINT uiNeedLevel, UINT * lpuiRealLevel )
{
	UINT i = 0;
    while( lpws->lpwsOwner && i < uiNeedLevel )
    {
        lpws = lpws->lpwsOwner;
		i++;
    }
	if( lpuiRealLevel )
		*lpuiRealLevel = i;
    return lpws;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static _LPWINDATA IsTopOwner( _LPWINDATA lpws, _LPWINDATA lpwsOwner )
{
    while( lpws->lpwsOwner && lpws->lpwsOwner != lpwsOwner )
    {
        lpws = lpws->lpwsOwner;
    }

    return lpws->lpwsOwner;
}

//����z order �� ��Ч����
static VOID UpdateZOrder( _LPWINDATA lpwsWnd, _LPWINDATA lpwsPrev, HRGN hOldExpose, HRGN hNewExpose )
{
	HRGN hrgn;
	//���� zorder
	if( hOldExpose )
	{
		GetExposeRgn( lpwsWnd->hThis, DCX_WINDOW | DCX_CLIPSIBLINGS, &hrgn );
		
		WinRgn_Combine( hOldExpose, hOldExpose, hrgn, RGN_OR );
		WinGdi_DeleteObject( hrgn );
	}
	if( lpwsWnd != lpwsPrev )
	{
		//�Ƴ�
		Link_RemoveWindow( lpwsWnd );
		//����
		Link_InsertWindowAfter( lpwsPrev, lpwsWnd, NULL );
	}

	if( hNewExpose )
	{
		GetExposeRgn( lpwsWnd->hThis, DCX_WINDOW | DCX_CLIPSIBLINGS, &hrgn );
		
		WinRgn_Combine( hNewExpose, hNewExpose, hrgn, RGN_OR );
		WinGdi_DeleteObject( hrgn );
	}
}

static _LPWINDATA GetBottomWindow( VOID )
{
	_LPWINDATA lpwsNext, lpwsPrev;

	lpwsNext = hwndScreen.lpwsChild;
	lpwsPrev = NULL;
	while( lpwsNext )
	{
		lpwsPrev = lpwsNext;
		lpwsNext = lpwsNext->lpwsNext;
	}
	return lpwsPrev;
}
//�ж� �Ƿ� lpwsPrev �� lpws��ǰ�� lpwsPrev-lpNext == lpws
static BOOL IsAbove( _LPWINDATA lpwsPrev, _LPWINDATA lpws )
{
	while( lpwsPrev )
	{
		if( lpwsPrev->lpwsNext == lpws )
			return TRUE;
		lpwsPrev = lpwsPrev->lpwsNext;
	}
	return FALSE;
}


//�õ�һ�����Ӵ��ڵ�������

static VOID SetGroupChangeFlag( _LPWINDATA lpwsThis, UINT uiLevel, BOOL bSet  )
{
	_LPWINDATA lpwsTopOwner = GetTopOwner( lpwsThis, uiLevel, &uiLevel );
	_LPWINDATA lpws = hwndScreen.lpwsChild;	
	
	while( lpws )
	{
		if( IsTopOwner( lpws, lpwsTopOwner ) )
		{
			if( bSet )
				lpws->uState |= WINS_ZORDER_CHANGING;
			else
				lpws->uState &= ~WINS_ZORDER_CHANGING;
		}
		lpws = lpws->lpwsNext;
	}
}


//������Ч����
static VOID GetGroupExposeRegion( HRGN hrgnExpose )
{
	HRGN hrgn;
	_LPWINDATA lpws = hwndScreen.lpwsChild;	
	
	while( lpws )
	{
		if( lpws->uState & WINS_ZORDER_CHANGING )
		{
			GetExposeRgn( lpws->hThis, DCX_WINDOW | DCX_CLIPSIBLINGS, &hrgn );
			
			WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_OR );
			WinGdi_DeleteObject( hrgn );
		}
		lpws = lpws->lpwsNext;
	}
}
//�����ƶ����뵽 lpwsDest֮��
static VOID RemoveOrderLinkTo( _LPWINDATA lpwsDest, _LPWINDATA lpwsStart, _LPWINDATA lpwsEnd, _LPWINDATA lpwsOwner )
{
	_LPWINDATA lpwsPrev = lpwsEnd;
	//�ӵ����ϱ���
	do
	{
		lpwsEnd = lpwsPrev;
		lpwsPrev = lpwsEnd->lpwsPrev;
		if( lpwsEnd != lpwsDest )
		{	
			if( lpwsEnd == lpwsOwner || IsTopOwner( lpwsEnd, lpwsOwner ) )
			{
				Link_RemoveWindow( lpwsEnd );
				Link_InsertWindowAfter( lpwsDest, lpwsEnd, NULL );
			}
		}
	}while( lpwsStart != lpwsEnd );
}


//�õ�ӵ����������ĵ�һ������
static _LPWINDATA GetGroupStart( _LPWINDATA lpwsOwner )
{
	_LPWINDATA lpws = hwndScreen.lpwsChild;

	if( lpwsOwner->uOwnCount )
	{
		while( lpws )
		{
			//_LPWINDATA lpwsTemp;
			if( IsTopOwner( lpws, lpwsOwner ) )
			{
				return lpws;
			}
			lpws = lpws->lpwsNext;
		}
		return lpws;
	}
	else
		return lpwsOwner;
}

//�ж� popup ���� lpwsPrev �Ƿ��� lpws ֮ǰ
static BOOL IsNeedChangeZOrder( _LPWINDATA lpwsPrev, _LPWINDATA lpws )
{
	BOOL bPrev;
	BOOL bRetv = TRUE;
	_LPWINDATA lpwsTemp;
	if( lpwsPrev == NULL )
		lpwsPrev = hwndScreen.lpwsChild;

	lpwsTemp = lpwsPrev;
	bPrev = FALSE;
	// Z order is prev ?
	while( lpwsTemp )
	{
		if( lpwsTemp == lpws )
		{
			bPrev = TRUE;
			break;
		}
		lpwsTemp = lpwsTemp->lpwsNext;
	}
	//�м��Ƿ��б�Ĵ��ڣ�
	if( bPrev )
	{
		lpwsTemp = lpwsPrev;
		bPrev = FALSE;
		while( lpwsTemp )
		{
			if( lpwsTemp == lpws )
			{
				bRetv = FALSE;//����Ҫ����
				break;
			}
			//�ô����Ǳ�ӵ�еĴ�����
			if( IS_SHOWENABLE( lpwsTemp ) )
			{	//��
				if( IsTopOwner( lpwsTemp, lpws ) == NULL )
					break;//��ӵ�еĴ���,����Ҫ����
			}
			lpwsTemp = lpwsTemp->lpwsNext;
		}
	}
	return bRetv;
}


//�õ�ӵ��������������һ������
static _LPWINDATA GetGroupEnd( _LPWINDATA lpwsMember, _LPWINDATA lpwsOwner )
{
	_LPWINDATA lpwsRetv = lpwsMember;
	//ASSERT( lpwsMember->lpwsOwner == lpwsOwner );
	while( lpwsMember && lpwsMember->lpwsOwner == lpwsOwner )
	{
		lpwsRetv = lpwsMember;
		lpwsMember = lpwsMember->lpwsNext;
	}
	return lpwsRetv;
}

//lpwsPrev �� lpwsThis
static VOID SetGroupOrderAfter( _LPWINDATA lpwsPrev, _LPWINDATA lpwsThis, UINT level )
{
	if( lpwsPrev != lpwsThis )
	{
		_LPWINDATA lpwsGrpEnd = GetTopOwner( lpwsThis, level, &level );
		_LPWINDATA lpwsGrpStart = GetGroupStart( lpwsGrpEnd );
		//������֦
		//RemoveOrderLink( lpwsGrpStart, lpwsGrpEnd );
		//������֦
		//InsertOrderLink( lpwsPrev, lpwsGrpStart, lpwsGrpEnd );
		RemoveOrderLinkTo( lpwsPrev, lpwsGrpStart, lpwsGrpEnd, lpwsGrpEnd );
		if( level )
			SetGroupOrderAfter( lpwsPrev, lpwsThis, --level );
	}
}


//�õ� lpwsThis �� lpwsPrev �Ĺ�ͬӵ���ߣ� ���� lpwsThis �� ��ͬӵ����֮��㼶
static UINT GetShareOwnerLevel( _LPWINDATA lpwsThis, _LPWINDATA lpwsDiff, _LPWINDATA * lppwsOwner )
{
	UINT i = 0;
    while( lpwsThis->lpwsOwner )
    {
		i++;
		if( IsTopOwner( lpwsDiff, lpwsThis->lpwsOwner ) )
			break;
        lpwsThis = lpwsThis->lpwsOwner;		
    }
	if( lppwsOwner )
	{
		*lppwsOwner = lpwsThis->lpwsOwner;
	}
	return i;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ƶ����ڵ�ָ����λ��
// ����: 
//	
// ************************************************

BOOL RemovePopupWindow( _LPWINDATA lpwsThis, _LPWINDATA lpwsAfter, HRGN hOldExpose, HRGN hNewExpose )
{
    _LPWINDATA lpwsPrev, lpwsNext;
//	_LPWINDATA lpws;
	DWORD dwOldTopMost;//, dwNewTopMost;
	UINT level = -1;

	LockCSWindow();

	dwOldTopMost = lpwsThis->dwExStyle & WS_EX_TOPMOST;
	lpwsPrev = lpwsNext = NULL;
	if( lpwsAfter == (_LPWINDATA)HWND_TOP || 
		lpwsAfter == (_LPWINDATA)HWND_TOPMOST )
    {		
        if( hwndScreen.lpwsChild == lpwsThis )
		{	//�Ѿ���TOP
			UnlockCSWindow();
            return TRUE;
		}
		if( dwOldTopMost )
		{	//TOPMOST ����
			lpwsNext = hwndScreen.lpwsChild;
		}
		else
		{	//�� TOPMOST ����
			if( lpwsAfter == (_LPWINDATA)HWND_TOPMOST )
			{  //��Ҫ����Ϊ TOPMOST style;
				lpwsThis->dwExStyle |= WS_EX_TOPMOST;
				lpwsNext = hwndScreen.lpwsChild;
			}
			else
			{  //��ͨ TOP����,����TOPMOST ����֮��
				lpwsNext = hwndScreen.lpwsChild;
				while( lpwsNext )
				{
					if( lpwsNext->dwExStyle & WS_EX_TOPMOST )
					{
						lpwsPrev = lpwsNext;
						lpwsNext = lpwsNext->lpwsNext;
					}                
					else
						break;
				}
			}
		}
    }
	
    else if( lpwsAfter == (_LPWINDATA)HWND_BOTTOM )
    {
        lpwsNext = hwndScreen.lpwsChild;
		//����
		if( dwOldTopMost )
		{	//ȥ�� TOPMOST ���
			lpwsThis->dwExStyle &= ~WS_EX_TOPMOST;
		}
        while( lpwsNext )
        {
            lpwsPrev = lpwsNext;
            lpwsNext = lpwsNext->lpwsNext;
        }
    }
	else
	{
		_LPWINDATA lpwsOwner;
		lpwsPrev = lpwsAfter;
		if( (lpwsPrev->dwExStyle & WS_EX_TOPMOST) == 0 )
		{	//ǰһ�������Ƿ�TOPMOST�ģ��ô��ڱ���ı�Ϊ��ͨ����
			lpwsThis->dwExStyle &= ~WS_EX_TOPMOST;
		}
		level = GetShareOwnerLevel( lpwsThis, lpwsPrev, &lpwsOwner );
		//���ܲ��� lpwsPrev �����м�
		lpwsPrev = GetGroupEnd( lpwsPrev, lpwsOwner );
	}

    if( lpwsPrev && lpwsPrev->lpwsNext == lpwsThis )
	{
		UnlockCSWindow();
        return TRUE;
	}	

	if( IsNeedChangeZOrder( lpwsPrev, lpwsThis ) )
	{
		SetGroupChangeFlag( lpwsThis, level, TRUE );
		GetGroupExposeRegion( hOldExpose );
		
		SetGroupOrderAfter( lpwsPrev, lpwsThis, level );
		
		GetGroupExposeRegion( hNewExpose );
		SetGroupChangeFlag( lpwsThis, level, FALSE );
	}
	
	UnlockCSWindow();
	return TRUE;
}


// **************************************************
// ������BOOL WINAPI Win_SetPos( HWND hWnd, HWND hWndInsertAfter,
//                               int x, int y, int cx, int cy,
//                               UINT uFlags )

// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************
BOOL WINAPI Win_SetPos( HWND hWnd, HWND hWndInsertAfter,
                   int x, int y, int cx, int cy,
                   UINT uFlags )
{
	BOOL bRetv;
	_LPWINDATA lpws;

    lpws = _GetHWNDPtr( hWnd );
	if( lpws )
	{
		bRetv = _Win_SetPos( lpws, hWndInsertAfter, x, y, cx, cy, uFlags );
	}
	else
		bRetv = FALSE;

	return bRetv;
}

static BOOL WINAPI _Win_SetPos( _LPWINDATA lpws, HWND hWndInsertAfter,
                   int x, int y, int cx, int cy,
                   UINT uFlags )
{
    HRGN hOldRgn = 0, hNewRgn = 0;
    WINDOWPOS winpos, *lpwinpos;
    LRESULT lresult;
    RECT rect;
    int x0, y0, xoff, yoff;
	BOOL bRetv = TRUE;

    if( lpws == NULL )
	{
		bRetv = FALSE;
		goto _SETPOSRET;
	}

    // give user a chance to change something
    winpos.x = x;
    winpos.y = y;
    winpos.cx = cx;
    winpos.cy = cy;
    winpos.hwnd = lpws->hThis;
    winpos.hwndInsertAfter = hWndInsertAfter;
    winpos.flags = uFlags;
    lresult = 0;
	//�ֲ������ǲ���Ҫӳ��� ???
	lpwinpos = (WINDOWPOS*)MapPtrToProcess( &winpos, hgwmeProcess );

    if( !(winpos.flags & SWP_NOSENDCHANGING) )
        lresult = WinMsg_Send( winpos.hwnd, WM_WINDOWPOSCHANGING, 0, (LPARAM)lpwinpos );
    
    winpos.hwnd = lpws->hThis;

    if( (winpos.flags & SWP_NOREDRAW) == 0 )
    {
		GetExposeRgn( winpos.hwnd, DCX_WINDOW | DCX_CLIPSIBLINGS, &hOldRgn );

        hNewRgn = WinRgn_CreateRect( 0, 0, 0, 0 );
    }

	if( ( winpos.flags & SWP_NOSIZE) == 0 ||
		( winpos.flags & SWP_NOMOVE) == 0 ||
		( winpos.flags & SWP_NOZORDER) == 0 ||
		( winpos.flags & SWP_NOZORDER) == 0 ||
		( winpos.flags & (SWP_FRAMECHANGED|SWP_SHOWWINDOW|SWP_HIDEWINDOW ) )
      )
	{
		UpdateDCState( TRUE );
	}

    if( (winpos.flags & SWP_NOZORDER) == 0 )
    {   // hWndInsertAfter is valid param, change Z order
        // repaint entire client
		LockCSWindow();
        if( lpws->dwMainStyle & WS_CHILD )
        {
            Link_RemoveWindow( lpws );//winpos.hwnd );
            if( winpos.hwndInsertAfter != HWND_TOP &&
                winpos.hwndInsertAfter != HWND_BOTTOM &&
                winpos.hwndInsertAfter != HWND_TOPMOST )
			{
				_LPWINDATA lpwsAfter = _GetHWNDPtr( winpos.hwndInsertAfter );// _GetHWNDPtr(  )
				if( lpwsAfter )
                    Link_InsertToParent( lpwsAfter, lpws, lpwsAfter->lpwsParent );
			}
            else
            {
                Link_InsertToParent( (_LPWINDATA)winpos.hwndInsertAfter, lpws, lpws->lpwsParent );//_GetHWNDPtr( winpos.hwnd )->hParent );
            }
        }
        else
        {   // popup window or overlapp window
			_LPWINDATA lpwsAfter = (_LPWINDATA)winpos.hwndInsertAfter;
            if( winpos.hwndInsertAfter != HWND_TOP &&
                winpos.hwndInsertAfter != HWND_BOTTOM &&
                winpos.hwndInsertAfter != HWND_TOPMOST
                )
			{
				lpwsAfter = _GetHWNDPtr( winpos.hwndInsertAfter );
                if( lpwsAfter && (lpwsAfter->dwMainStyle & WS_CHILD) )
				{
					UnlockCSWindow();
                    bRetv = FALSE;
		            goto _SETPOSRET;
				}
			}
			RemovePopupWindow( lpws, lpwsAfter, hOldRgn, hNewRgn );
        }
		UnlockCSWindow();
    }

    if( winpos.flags & (SWP_HIDEWINDOW | SWP_SHOWWINDOW ) )
	{
		BOOL bShow = ( (winpos.flags & SWP_SHOWWINDOW) != 0 );
        _SetState( lpws, WS_VISIBLE, bShow );
		if( bShow )
		    _SetState( lpws, WS_MINIMIZE, FALSE  );
		else
		{	//2004-09-09������,������������Ϣ
			ClearThreadQueue( lpws->dwThreadID, NULL, WM_MOUSEFIRST, WM_MOUSELAST );
		}
	}
    if( (winpos.flags & SWP_NOMOVE) == 0 ||
        (winpos.flags & SWP_NOSIZE) == 0 )
    {  // x, y or cx, cy is valid param, do move or size
       // initial winpos struct data
        _LPWINDATA lpwsZUp;
        lpwsZUp = GET_Z_ORDER_UP( lpws );
        // get window parent's position
        x0 = lpwsZUp->rectWindow.left + lpwsZUp->rectClient.left;
        y0 = lpwsZUp->rectWindow.top + lpwsZUp->rectClient.top;

        xoff = lpws->rectWindow.left;
        yoff = lpws->rectWindow.top;

        if( (winpos.flags & SWP_NOSIZE) == 0 )
        {   // do resize, cx, cy is valid
            cx = winpos.cx;
            cy = winpos.cy;
        }
        else
        {
            cx = lpws->rectWindow.right - lpws->rectWindow.left;
            cy = lpws->rectWindow.bottom - lpws->rectWindow.top;
        }

        if( (winpos.flags & SWP_NOMOVE) == 0 )
        {    // do move, x, y is valid
            x = x0 + winpos.x;
            y = y0 + winpos.y;
        }
        else
        {
            x = lpws->rectWindow.left;
            y = lpws->rectWindow.top;
        }

        if( lpws->hrgnWindow )
		{
			OffsetRgn( lpws->hrgnWindow, -lpws->rectWindow.left, -lpws->rectWindow.top );
		}

		lpws->rectWindow.left = x;
        lpws->rectWindow.top = y;
        lpws->rectWindow.right = x + cx;
        lpws->rectWindow.bottom = y + cy;

        if( lpws->hrgnWindow )
		{
			OffsetRgn( lpws->hrgnWindow, x, y );
		}

        // get client rect
		rect = lpws->rectWindow;
		if( _CalcClientBound( winpos.hwnd, &rect ) != 0 )
		{
			bRetv = FALSE;
			goto _SETPOSRET;
		}
		lpws->rectClient = rect;
		//

        if( (winpos.flags & SWP_NOMOVE) == 0 )
        {
            // enum all child to change window rect
            xoff = lpws->rectWindow.left - xoff;
            yoff = lpws->rectWindow.top - yoff;
            if( (winpos.flags & SWP_NOMOVE) == 0 )
			{
				CALLBACKDATA cb;
				cb.hProcess = NULL;
				cb.lpfn = (FARPROC)MoveChild;

				DoEnumWindow( lpws->lpwsChild, TRUE, &cb, MAKELONG( xoff, yoff ) );

			}
        }
    }
	else if( winpos.flags & SWP_FRAMECHANGED )
	{
        // get client rect
		rect = lpws->rectWindow;
		if( _CalcClientBound( winpos.hwnd, &rect ) != 0 )
		{
			bRetv = FALSE;
			goto _SETPOSRET;
		}
		lpws->rectClient = rect;
		//
	}

	if( (winpos.flags & SWP_NOACTIVATE) == 0 &&     //��Ҫ����
		(winpos.flags & SWP_HIDEWINDOW ) == 0       //����ʾ��
	  )
	{
        Win_SetActive( winpos.hwnd );
    }


    if( (winpos.flags & SWP_HIDEWINDOW ) )
    {	//��������
		LPGWEDATA lpgwe = GetGWEPtr( lpws->dwThreadID );
        if( lpgwe && lpgwe->hwndFocus )
		{   //��ǰ�Ǹô��ڵ��Ӵ���
			if( Win_IsChild( lpws->hThis, lpgwe->hwndFocus ) )
			{	//�������ͬ�̣߳���ͬ�������򣬲�ͬ��
				DoSetFocus( lpgwe, NULL, lpws->dwThreadID == GetCurrentThreadId() );
			}
		}
		//2005-09-28, add code here
		if( lpgwe && lpgwe->hwndActive == lpws->hThis )
		{
			DoSetActive( lpgwe, NULL, lpws->dwThreadID == GetCurrentThreadId() );
		}
		//
    }

    if( (winpos.flags & SWP_NOREDRAW) == 0 )
    {   // get expose rgn after changed
        HRGN hrgn;

		GetExposeRgn( winpos.hwnd, DCX_WINDOW | DCX_CLIPSIBLINGS, &hrgn );

        if( hrgn )
        {
            WinRgn_Combine( hNewRgn, hNewRgn, hrgn, RGN_OR );

            WinGdi_DeleteObject( hrgn );
        }
        if( (winpos.flags & SWP_NOSIZE) == 0 ||
            (winpos.flags & SWP_NOMOVE) == 0 ||
			(winpos.flags & SWP_FRAMECHANGED) )
        {
            if( WinRgn_Combine( hNewRgn, hNewRgn, hOldRgn, RGN_OR ) != NULLREGION ) 
            {
                _AddPaintRegion(hNewRgn);
                if( lpws->dwMainStyle & WS_CHILD )
                {
                    _AddPaintWndRgn( lpws->lpwsParent,hNewRgn, TRUE, GetCurrentThreadId() );
                }
                else
                {
                     _AddPaintWndRgn(0,hNewRgn, TRUE, GetCurrentThreadId() );
                }

            }
        }
        else
        {
            int retv;

            hrgn = WinRgn_CreateRect( 0, 0, 0, 0 );

            retv = WinRgn_Combine( hrgn, hNewRgn, hOldRgn, RGN_DIFF );

            if( retv == NULLREGION )
                retv = WinRgn_Combine( hrgn, hOldRgn, hNewRgn, RGN_DIFF );

            WinGdi_DeleteObject( hOldRgn );

            WinGdi_DeleteObject( hNewRgn );

            hOldRgn = hNewRgn = 0;
            if( retv != NULLREGION )
            {
                _AddPaintRegion(hrgn);

                if( lpws->dwMainStyle & WS_CHILD )
				{
                    _AddPaintWndRgn(lpws->lpwsParent,hrgn,TRUE, GetCurrentThreadId() );
				}
                else
				{
                    _AddPaintWndRgn( 0 , hrgn, TRUE, GetCurrentThreadId() );
				}

            }

            WinGdi_DeleteObject( hrgn );

        }
    }

	if( (winpos.flags & SWP_HIDEWINDOW) == 0 ) 
	{
		// the case is supprt win_clear option, if you change it, 
		// you must change the win_clear option too.
	    WinMsg_Send( winpos.hwnd, WM_WINDOWPOSCHANGED, 0, (LPARAM)lpwinpos );
		
		// add for layered window
		if( lpws->dwExStyle & WS_EX_LAYERED )
		{
			//WinMsg_Send( lpws->hThis, WM_NCPAINT, 1, NULL );			
			//AddMsgToThreadQueue( lpws->hThis, WM_NCPAINT, 1, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
			//AddMsgToThreadQueue( lpws->hThis, WM_PAINT, 0, 0, ATQ_ONLYONE | QS_POSTMESSAGE );

		}
	}

_SETPOSRET:
	if( hOldRgn )
        WinGdi_DeleteObject( hOldRgn );

	if( hNewRgn )
        WinGdi_DeleteObject( hNewRgn );

    return bRetv;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_SetText( HWND hWnd, LPCSTR lpcstr )
{
	LRESULT lretv = FALSE;
	if( WinMsg_SendTimeout( hWnd, WM_SETTEXT, 0, (LPARAM)lpcstr, SMTO_BLOCK, 5000, &lretv ) )
		return lretv;
	else
		return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND _Win_FromPoint( POINT pt, int * lpfDisable )
{
    _LPWINDATA lpws = NULL;
	LockCSWindow();
	lpws = hwndScreen.lpwsChild;
    while( lpws )
    {
			if(               
				IS_SHOWENABLE( lpws ) &&  // 2003-08-05, Ϊ ShowDesktop�޸�
				PtInRect( &lpws->rectWindow, pt ) 
			   )
			{
				if( lpws->hrgnWindow )
				{
					if( PtInRegion( lpws->hrgnWindow, pt.x, pt.y ) )
						break;
				}
				else
					break;
			}
			lpws = lpws->lpwsNext;
    }
    if( lpws )
	{
		if( lpfDisable )
            *lpfDisable = (lpws->dwMainStyle & WS_DISABLED);
	}
	else
		lpws = NULL;  // invalid window handle
	UnlockCSWindow();
    return lpws ? lpws->hThis : NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

// beause the CALL API ����������������ƽ̨��
HWND WINAPI Win_FromPoint( int x, int y )
{
    int fDisable;
	POINT point;
	HWND hWnd;
	point.x = x;
	point.y = y;
    
	hWnd = _Win_FromPoint( point, &fDisable );
    if( fDisable )
        return 0;
    else
        return hWnd;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************
#define DEBUG_Win_Update 1
BOOL WINAPI Win_Update( HWND hWnd )
{
    _MSGDATA msgData;
	DWORD dwThreadId = GetCurrentThreadId();
	BOOL retv = FALSE;
	_LPWINDATA lpws = _GetHWNDPtr( hWnd );

	if( lpws )
	{
		if( lpws->dwThreadID == dwThreadId )
		{
			while( CheckThreadQueue( &msgData, hWnd, WM_PAINT, WM_PAINT, PM_REMOVE, 1 ) )
			{
				_LPWINDATA lpws = _GetHWNDPtr( msgData.msg.hwnd );
				if( lpws )
				{
					_CallWindowProc( lpws, NULL, WM_PAINT, 0, 0 );
					//WinMsg_SendMessage( msg.hwnd, WM_PAINT, 0, 0 );
					retv = TRUE;
				}
			}
		}
		else 
		{  //��ͬ���̣߳� �������
			UINT nTry = 100; // ����
			DEBUGMSG( DEBUG_Win_Update, ( "Win_Update:diff thread++.\r\n" ) );
			while( nTry && CheckThreadQueue( &msgData, hWnd, WM_PAINT, WM_PAINT, PM_NOREMOVE, 1 ) )
			{
				Sleep(50);
				nTry--;
			}
			retv = nTry ? TRUE : FALSE;
			DEBUGMSG( DEBUG_Win_Update, ( "Win_Update:diff thread--.\r\n" ) );			
		}
	}
	return retv;
}



// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_GetUpdateRect( HWND hWnd, LPRECT lpRect, BOOL bErase )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
	BOOL bRetv = FALSE;
    if( lpws )
    {    
        HRGN hrgn;
        RECT rect;
        HDC hdc;

        SetRectEmpty( lpRect );

        if( __LockRepaintRgn( &hrgn ) ) 
        {
            rect = lpws->rectClient;
            OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
			if( lpws->hrgnWindow )
			{
				RECT rcWindow;
				WinRgn_GetBox( lpws->hrgnWindow, &rcWindow );
				IntersectRect( &rect, &rect, &rcWindow );				
			}
            if( WinRgn_RectInRegion( hrgn, &rect ) )
            {
                WinRgn_GetBox( hrgn, lpRect );
				__UnlockRepaintRgn(); 

                IntersectRect( lpRect, lpRect, &rect );
                OffsetRect( lpRect,
                            -(lpws->rectWindow.left+lpws->rectClient.left),
                            -(lpws->rectWindow.top+lpws->rectClient.top) );
                if( bErase )
                {
                    hdc = WinGdi_GetClientDC( hWnd );
                    WinMsg_Send( hWnd, WM_ERASEBKGND, (WPARAM)hdc, 0 );
                    WinGdi_ReleaseDC( hWnd, hdc );
                }
				return TRUE;
            }
		}
		__UnlockRepaintRgn(); 
    }
    return bRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

int WINAPI Win_GetClassName( HWND hWnd, LPTSTR lpszClassName, int nMaxCount )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
		lpszClassName[nMaxCount-1] = 0;
		if( strncpy( lpszClassName, lpws->lpClass->wc.lpszClassName, nMaxCount - 1 ) )
		{
			return strlen( lpszClassName );
		}
	}
	return 0;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

int WINAPI Win_GetUpdateRgn( HWND hWnd, HRGN hrgn, BOOL bErase )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    if( lpws )
    {
        HRGN hrgnExpose;
        DWORD flags = 0;
        HRGN hRepaintRgn;
        int retv = NULLREGION;

        {
            if( lpws->dwMainStyle & WS_CLIPCHILDREN )
                flags |= DCX_CLIPCHILDREN;
            if( lpws->dwMainStyle & WS_CLIPSIBLINGS )
                flags |= DCX_CLIPSIBLINGS;

			GetExposeRgn( hWnd, flags, &hrgnExpose );

            ASSERT( hrgnExpose );
            if( hrgnExpose )
            {				
				__LockRepaintRgn( &hRepaintRgn ); 
                retv = WinRgn_Combine( hrgn, hrgnExpose, hRepaintRgn, RGN_AND );
				__UnlockRepaintRgn(); 			

                WinGdi_DeleteObject( hrgnExpose );
                WinRgn_Offset( hrgn, 
                           -(lpws->rectWindow.left+lpws->rectClient.left),
                           -(lpws->rectWindow.top+lpws->rectClient.top) );
            }
        }
        return retv;
    }
    return ERROR;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static void _SetUpdateRect( int dx, int dy, LPCRECT lprect, LPRECT lpUpdateRect )
{
    // set lpUpdateRect
    if( dx == 0 )
    {
        lpUpdateRect->left = lprect->left;
        lpUpdateRect->right = lprect->right;
        if( dy < 0 )
        {
            lpUpdateRect->top = lprect->bottom + dy;
            lpUpdateRect->bottom = lprect->bottom;
        }
        else
        {   // dy > 0
            lpUpdateRect->top = lprect->top;
            lpUpdateRect->bottom = lprect->top + dy;
        }
    }
    else
    {   // dy == 0
        lpUpdateRect->top = lprect->top;
        lpUpdateRect->bottom = lprect->bottom;
        if( dx < 0 )
        {
            lpUpdateRect->left = lprect->right + dx;
            lpUpdateRect->right = lprect->right;
        }
        else
        {   // dx > 0
            lpUpdateRect->left = lprect->left;
            lpUpdateRect->right = lprect->left + dx;
        }
    }
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

int WINAPI Win_Scroll( HWND hWnd,
                    int dx,
                    int dy,
                    LPCRECT lpScrollRect,
                    LPCRECT lpClipRect,
                    HRGN hrgnUpdate,
                    LPRECT lpUpdateRect,
                    UINT flags )
{
    RECT rect;
    HDC hdc;
    HRGN hrgnRemove;
    int retv = ERROR;
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );

	if( lpws == NULL )
		return retv;


    if( lpScrollRect == 0 )
        Win_GetClientRect( hWnd, &rect );
    else
        *((LPRECT)&rect) = *lpScrollRect;
	if( lpClipRect )
		if( !IntersectRect( &rect, &rect, lpClipRect ) )
			return retv;

    if( !(dx == 0 && dy == 0) )
    {
		hdc = WinGdi_GetClientDC( hWnd );

		// scroll window
		retv = NULLREGION;
		WinGdi_BitBlt( hdc,
			rect.left + dx, rect.top + dy,
			rect.right - rect.left, rect.bottom - rect.top,
			hdc,
			rect.left, rect.top,
			SRCCOPY );
		WinGdi_ReleaseDC( hWnd, hdc );

		if( flags & SW_SCROLLCHILDREN )
		{
			_LPWINDATA lpwsChild;

			int xParent =lpws->rectWindow.left + lpws->rectClient.left;
			int yParent = lpws->rectWindow.top + lpws->rectClient.top;

			// enter criti
			LockCSWindow();
			lpwsChild = lpws->lpwsChild;
			while( lpwsChild )
			{   
				RECT rcTemp, rc = lpwsChild->rectWindow;

				OffsetRect( &rc, -xParent, -yParent );

				if( lpScrollRect && IntersectRect( &rcTemp, &rc, &rect ) )
				    _Win_SetPos( lpwsChild, 0, rc.left + dx, rc.top + dy, 0, 0, SWP_NOREDRAW|SWP_NOZORDER|SWP_NOSIZE );
				else
					_Win_SetPos( lpwsChild, 0, rc.left + dx, rc.top + dy, 0, 0, SWP_NOREDRAW|SWP_NOZORDER|SWP_NOSIZE );


				lpwsChild = lpwsChild->lpwsNext;
			}
			// leave criti
			UnlockCSWindow();
		}

		if( hrgnUpdate )
		{
			WinRgn_SetRect( hrgnUpdate, rect.left, rect.top, rect.right, rect.bottom );
			OffsetRect( &rect, dx, dy );
			hrgnRemove = WinRgn_CreateRectIndirect( &rect );
			ASSERT( hrgnRemove );
			retv = WinRgn_Combine( hrgnUpdate, hrgnUpdate, hrgnRemove, RGN_DIFF );
			WinGdi_DeleteObject( hrgnRemove );
		}
		
		if( lpUpdateRect )
		{
			if( hrgnUpdate )
				WinRgn_GetBox( hrgnUpdate, lpUpdateRect );
			else if( dx == 0 || dy == 0 )
			{
				_SetUpdateRect( dx, dy, &rect, lpUpdateRect );
				retv = SIMPLEREGION;
			}
			else
			{
				*lpUpdateRect = rect;
				retv = SIMPLEREGION;
			}
		}
		if( flags & SW_INVALIDATE )
		{
			Win_InvalidateRgn( hWnd, hrgnUpdate, FALSE );
			if( flags & SW_ERASE )
			{
				HDC hdc = WinGdi_GetClientDC( hWnd );
				WinMsg_Send( hWnd, WM_ERASEBKGND, (WPARAM)hdc, 0 );
				WinGdi_ReleaseDC( hWnd, hdc );
			}
		}
		
	}
    return retv;
}

//===========================================================================
// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static HANDLE _AllocWinObjectHandle( int s )
{
    void * p = BlockHeap_AllocString( hgwmeBlockHeap, 0, s );

	if( p )
		memset( p, 0, s );
    return (HANDLE)p;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL _DeleteWinObject( _LPWINDATA lpws )
{
    lpws->objType = OBJ_NULL;
	lpws->hThis = 0;

    BlockHeap_FreeString( hgwmeBlockHeap, 0, lpws ); 

    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static _LPWINDATA _BottomChildWindow( _LPWINDATA lpws )
{
	LockCSWindow();
    while( lpws->lpwsNext &&
           ( lpws->lpwsNext->dwMainStyle & WS_CHILD) )
        // if next window is exist and next window has WS_CHILD style, continue
        lpws = lpws->lpwsNext;

	UnlockCSWindow();
    return lpws;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static LRESULT _CalcClientBound( HWND hWnd, LPRECT lprect )
{
    RECT rect = *lprect;
	LRESULT lresult;
    int cx, cy;

    lresult = WinMsg_Send( hWnd, WM_NCCALCSIZE, FALSE, (LPARAM)MapPtrToProcess(lprect, hgwmeProcess) );
    cx = lprect->right - lprect->left;
    cy = lprect->bottom - lprect->top;
    lprect->left -= rect.left;
    lprect->top -= rect.top;
    lprect->right = lprect->left + cx;
    lprect->bottom = lprect->top + cy;
	return lresult;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL _ChangeBound( HWND hWnd, LPRECT lpWindowRect, LPRECT lpClientRect )
{
    _GetHWNDPtr( hWnd )->rectWindow = *lpWindowRect;
    _GetHWNDPtr( hWnd )->rectClient = *lpClientRect;
    return TRUE;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

//===================== get expose rgn =================================
static int __ExcludeChildrens( _LPWINDATA lpwsChild, HRGN hrgnExpose, LPRECT lprcClip )
{
    HRGN hrgnExclude;
    int retv = COMPLEXREGION;
    RECT rc;

    while( lpwsChild )
    {
		if( IS_SHOWENABLE( lpwsChild ) )  // 2003-08-05, Ϊ ShowDesktop�޸�
        {                                          
            if( IntersectRect( &rc, lprcClip, &lpwsChild->rectWindow ) )
            {
                hrgnExclude = WinRgn_CreateRectIndirect( &rc );
				if( lpwsChild->hrgnWindow )
					WinRgn_Combine( hrgnExclude, hrgnExclude, lpwsChild->hrgnWindow, RGN_AND );
                retv = WinRgn_Combine( hrgnExpose, hrgnExpose, hrgnExclude, RGN_DIFF );
                WinGdi_DeleteObject( hrgnExclude );
                if( retv == NULLREGION || retv == ERROR )
                    break;
            }
        }
        lpwsChild = lpwsChild->lpwsNext;
    }
    return retv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int __ExcludeBrother( _LPWINDATA lpws, HRGN hrgnExpose, _LPWINDATA lpwsZUp )
{
    HRGN hrgnExclude;
    _LPWINDATA lpwsBrother;
    int retv = COMPLEXREGION;
	
	hrgnExclude = WinRgn_CreateRect( 0, 0, 0, 0 );

    lpwsBrother = lpwsZUp->lpwsChild;
    while( lpwsBrother != lpws )
    {
		if( IS_SHOWENABLE( lpwsBrother ) &&
			(lpwsBrother->dwExStyle & WS_EX_LAYERED) == 0 )  // 2003-08-05, Ϊ ShowDesktop�޸�
        {                                            
            //hrgnExclude = WinRgn_CreateRectIndirect( &lpwsBrother->rectWindow );
			SetRectRgn( hrgnExclude, lpwsBrother->rectWindow.left, lpwsBrother->rectWindow.top, lpwsBrother->rectWindow.right, lpwsBrother->rectWindow.bottom );
			if( lpwsBrother->hrgnWindow )
				WinRgn_Combine( hrgnExclude, hrgnExclude, lpwsBrother->hrgnWindow, RGN_AND );
            ASSERT( hrgnExclude );
            retv = WinRgn_Combine( hrgnExpose, hrgnExpose, hrgnExclude, RGN_DIFF );
            
            if( retv == NULLREGION || retv == ERROR )
                break;
        }
        lpwsBrother = lpwsBrother->lpwsNext;
    }
	WinGdi_DeleteObject( hrgnExclude );

    return retv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

int GetExposeRgn( HWND hWnd, DWORD flags, HRGN * lphrgn )
{
    RECT rect;
    HRGN hrgnExpose, hrgnIntersect;

    _LPWINDATA lpwsZUp, lpws = _GetHWNDPtr( hWnd );
    int retv;

    // the hWnd's rectClient or rectWindow is default expose rgn
    if( lpws == NULL || 
		IS_SHOWENABLE( lpws ) == 0  // 2003-08-05, Ϊ ShowDesktop�޸�
	  )
    {
		*lphrgn = WinRgn_CreateRect( 0, 0, 0, 0 );
		if( *lphrgn )
			return NULLREGION;
		else
			return ERROR;
    }
	/*
	else
	{  // �п����� LAYERED Window�� ���� DCX_LAYERED ��־ʱ��������Ҫ�õ��� ��¶����
		if( ( lpws->dwExStyle & WS_EX_LAYERED ) &&
			( flags & DCX_NOLAYERED ) == 0 )
		{	// 
			*lphrgn = WinRgn_CreateRect( 0, 0, 0, 0 );
			if( *lphrgn )
				return NULLREGION;
			else
				return ERROR;
		}
	}
	*/

    if( flags & DCX_WINDOW )
        rect = lpws->rectWindow;
    else
    {   // use client rect
        rect = lpws->rectClient;
        OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
    }

    *lphrgn = hrgnExpose = WinRgn_CreateRectIndirect( &rect );
	if( lpws->hrgnWindow )
		WinRgn_Combine( hrgnExpose, hrgnExpose, lpws->hrgnWindow, RGN_AND );
    ASSERT( hrgnExpose );

	LockCSWindow();

	if( (flags & DCX_CLIPCHILDREN) && lpws->lpwsChild )
    {   // exclude child window
        RECT rcClip = lpws->rectClient;
        OffsetRect( &rcClip, lpws->rectWindow.left, lpws->rectWindow.top );
        retv = __ExcludeChildrens( lpws->lpwsChild, hrgnExpose, &rcClip );
        if( retv == NULLREGION || retv == ERROR )
			goto _RET;
    }

    lpwsZUp = GET_Z_ORDER_UP( lpws );

    if( flags & DCX_CLIPSIBLINGS )
    {   // exclude brother window
        retv = __ExcludeBrother( lpws, hrgnExpose, lpwsZUp );
        if( retv == NULLREGION || retv == ERROR )
		{
			goto _RET;
		}
    }

    do{ // intersect expose rgn with parent's client rect
        lpws = lpwsZUp;

		if( IS_SHOWENABLE( lpws ) )  // 2003-08-05, Ϊ ShowDesktop�޸�
        {                                     // add 2000.06.07
            rect = lpws->rectClient;
            OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
        }                                     // add 2000.06.07
        else                                  // add 2000.06.07
        {                                     // add 2000.06.07
            rect.left = rect.top = rect.right = rect.bottom = 0; // add 2000.06.07
        }                                     // add 2000.06.07
        hrgnIntersect = WinRgn_CreateRectIndirect( &rect );
		if( lpws->hrgnWindow )
			WinRgn_Combine( hrgnIntersect, hrgnIntersect, lpws->hrgnWindow, RGN_AND );
        ASSERT( hrgnIntersect );
        retv = WinRgn_Combine( hrgnExpose, hrgnExpose, hrgnIntersect, RGN_AND );
        WinGdi_DeleteObject( hrgnIntersect );
        if( retv == NULLREGION || retv == ERROR || lpwsZUp == &hwndScreen )
            break;
        // exclude parent's brother        

        lpwsZUp = GET_Z_ORDER_UP( lpws );
        retv = __ExcludeBrother( lpws, hrgnExpose, lpwsZUp );
        if( retv == NULLREGION || retv == ERROR )
            break;

    }while( 1 );

_RET:

	UnlockCSWindow();

    return retv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _GetTopHeight( DWORD dwMainStyle, DWORD dwSubStyle )
{
    return 10;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _GetLeftBorderWidth( DWORD dwMainStyle, DWORD dwSubStyle )
{
    return 1;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _GetBottomHight( DWORD dwMainStyle, DWORD dwSubStyle )
{
    return 1;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _GetRightWidth( DWORD dwMainStyle, DWORD dwSubStyle )
{
    return 1;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _GetMenuHeight( void )
{
    return 0;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

//�ṩ�ⲿ hook
PERROR_WINDOW_HOOK _lpWindowErrorHook = NULL;


#ifdef __DEBUG
_LPWINDATA __GetHWNDPtr( HWND hWnd, char * file, int line )
#else
_LPWINDATA _GetHWNDPtr( HWND hWnd )
#endif
{
    _LPWINDATA lpws;
    if( hWnd )
    {
		lpws = __GET_OBJECT_ADDR( hWnd );		

		if( lpws )
		{
			if( lpws->objType == OBJ_WINDOW &&  lpws->hThis == hWnd )
				return lpws; 
		}
    }
#ifdef __DEBUG
	WARNMSG( 1, ("error: invalid window handle=0x%x, file:%s, line:%d.\r\n", hWnd, file, line ) );
#else
	WARNMSG( 1, ("error: invalid window handle=0x%x\r\n", hWnd ) );
#endif
	SetLastError( ERROR_INVALID_HANDLE );	
	if( _lpWindowErrorHook )
		_lpWindowErrorHook( hWnd );
    return NULL;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL Link_InsertToParent( _LPWINDATA lpwsInsertAfter, _LPWINDATA lpws, _LPWINDATA lpwsParent )
{
	{
		Link_InsertWindowAfter( lpwsInsertAfter, lpws, lpwsParent );
		lpws->lpwsParent = lpwsParent;
		lpws->lpwsOwner = NULL;
	}
	
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����߱��� LockCSWindow();
// ����: 
//	
// ************************************************

static BOOL Link_InsertWindowAfter( _LPWINDATA lpwsAfter, _LPWINDATA lpws, _LPWINDATA lpwsTop )
{
	{
		_LPWINDATA lpwsPrev, lpwsTemp, *lphwndFirst;
		
		if( lpwsTop )
			;
		else
			lpwsTop = &hwndScreen;

		ASSERT( lpws->lpwsNext == NULL );
		ASSERT( lpws->lpwsPrev == NULL );
		
		lphwndFirst = &lpwsTop->lpwsChild;

		if( lpwsAfter == (_LPWINDATA)HWND_BOTTOM )
		{
			if( *lphwndFirst == NULL )
				*lphwndFirst = lpws;
			else
			{
				lpwsPrev = lpwsTemp = *lphwndFirst;
				while( (lpwsTemp = lpwsTemp->lpwsNext ) != NULL )
					lpwsPrev = lpwsTemp;

				lpwsPrev->lpwsNext = lpws;

				lpws->lpwsPrev = lpwsPrev;
			}
		}
		else if( lpwsAfter == (_LPWINDATA)HWND_TOP )
		{
			if( *lphwndFirst == NULL )
				*lphwndFirst = lpws;
			else
			{
				lpws->lpwsNext = *lphwndFirst;
				( *lphwndFirst )->lpwsPrev = lpws;

				*lphwndFirst = lpws;
			}
		}
		else
		{
			lpwsTemp = lpwsAfter->lpwsNext;
			lpwsAfter->lpwsNext = lpws;
			lpws->lpwsNext = lpwsTemp;
			lpws->lpwsPrev = lpwsAfter;
			if( lpwsTemp )
				lpwsTemp->lpwsPrev = lpws;
		}
	}
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����߱��� LockCSWindow();
// ����: 
//	
// ************************************************

static BOOL Link_InsertWindowToScreen( _LPWINDATA lpws )
{
	BOOL bRetv = TRUE;

    if( lpws->dwExStyle & WS_EX_TOPMOST )
        bRetv =  Link_InsertWindowAfter( (_LPWINDATA)HWND_TOP, lpws, NULL );
    else
    {
        _LPWINDATA lpwsNext; 

		lpwsNext = hwndScreen.lpwsChild;
        
        if( lpwsNext )
        {
            _LPWINDATA lpwsAfter = (_LPWINDATA)HWND_TOP;
            while( lpwsNext )
            {
                if( lpwsNext->dwExStyle & WS_EX_TOPMOST )
                {
                    lpwsAfter = lpwsNext;
                    lpwsNext = lpwsNext->lpwsNext;
                }
                else
                    break;
            }
            bRetv = Link_InsertWindowAfter( lpwsAfter, lpws, NULL );
        }
        else
            hwndScreen.lpwsChild = lpws;
    }
	 
    return bRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static void _MapAbsCoordinate( HWND hWnd, POINT * lppt )
{
    lppt->x += _GetHWNDPtr( hWnd )->rectWindow.left;
    lppt->y += _GetHWNDPtr( hWnd )->rectWindow.top;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����߱��� LockCSWindow();
// ����: 
//	
// ************************************************

static BOOL Link_RemoveWindow( _LPWINDATA lpws )
{
    _LPWINDATA lpTop;
	
    lpTop = GET_Z_ORDER_UP( lpws );
	
	if( lpws->lpwsPrev )
	{
		lpws->lpwsPrev->lpwsNext = lpws->lpwsNext;
	}
	if( lpws->lpwsNext )
	{
		lpws->lpwsNext->lpwsPrev = lpws->lpwsPrev;
	}
	if( lpws->lpwsPrev == NULL )
	{   // top window
		ASSERT( lpws == lpTop->lpwsChild );
		lpTop->lpwsChild = lpws->lpwsNext;
	}
	
	lpws->lpwsNext = NULL;
	lpws->lpwsPrev = NULL;
	
	
	return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

void FreeStr( LPSTR lpstr )
{
    if( lpstr )
        free( lpstr );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

LPSTR NewStr( LPCSTR lpstr )
{
    LPSTR p;
    if( lpstr == 0 )
        return 0;
    p = (LPSTR)malloc( strlen(lpstr) + 1 );
    if( p )
        strcpy( p, lpstr );
    return  p;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL _SetWinParam( _LPWINDATA lpws, LPCREATESTRUCT lpcs )
{
    _LPWINDATA lpParent;
    int xOffset = 0;
    int yOffset = 0;

	lpws->objType = OBJ_WINDOW;
    if( lpcs->style & WS_CHILD )
    {
        lpParent = _GetHWNDPtr( lpcs->hParent );
        xOffset = lpParent->rectWindow.left + lpParent->rectClient.left;
        yOffset = lpParent->rectWindow.top + lpParent->rectClient.top;
    }

    lpws->rectWindow.left = lpcs->x;
    lpws->rectWindow.top = lpcs->y;
    lpws->rectWindow.right = lpcs->x + lpcs->cx;
    lpws->rectWindow.bottom = lpcs->y + lpcs->cy;
    OffsetRect( &lpws->rectWindow, xOffset, yOffset );
    lpws->lpWinText = NULL;
    lpws->dwMainStyle = lpcs->style;
    lpws->dwExStyle = lpcs->dwExStyle;
    lpws->hMenu = lpcs->hMenu;
    lpws->hinst = lpcs->hInstance;
    lpws->uOwnCount = 0;

	lpws->dwThreadID = GetCurrentThreadId();
    return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

HWND WINAPI Win_GetTopLevelParent( HWND hWnd )
{
	_LPWINDATA lpws;

	LockCSWindow();

    lpws = _GetHWNDPtr( hWnd );

	while( lpws && (lpws->dwMainStyle & WS_CHILD) &&
		   lpws->lpwsParent )
		lpws = lpws->lpwsParent;

	if( lpws )
	    hWnd = lpws->hThis;
	else
		hWnd = NULL;

	UnlockCSWindow();
	return hWnd;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL _IsMinimized( HWND hWnd )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
	
	if( lpws )
        return (lpws->dwMainStyle & WS_MINIMIZE) == WS_MINIMIZE;
	else
		return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static int _SetState( _LPWINDATA lpws, DWORD style, BOOL bEnable )
{
    if( bEnable )
        lpws->dwMainStyle |= style;
    else
        lpws->dwMainStyle &= ~style;
    return 1;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static _LPWINDATA _GetSafeOwner( _LPWINDATA lpwsParent )
{
	LockCSWindow();

    while( lpwsParent && 
		   ( lpwsParent->dwMainStyle & WS_CHILD ) &&
		   lpwsParent->lpwsParent )
        lpwsParent = lpwsParent->lpwsParent;

	UnlockCSWindow();

    return lpwsParent;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

WORD _GetWindowWord( HWND hWnd, int pos )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );

    return *( (WORD*)(lpws+1) + pos );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

WORD _SetWindowWord( HWND hWnd, int pos, WORD v )
{
    _LPWINDATA lpws = _GetHWNDPtr( hWnd );
    WORD old;

    old = *( (WORD*)(lpws+1) + pos );
    *( (WORD*)(lpws+1) + pos ) = v;
    return old;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL _Win_IsClass( HWND hWnd, LPCBYTE lpcName )
{
   _LPWINDATA lpws = _GetHWNDPtr( hWnd );
   if( lpws )
   {
       return !stricmp( lpws->lpClass->wc.lpszClassName, (char*)lpcName );
   }
   return FALSE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

DWORD WINAPI Win_GetThreadProcessId( HWND hWnd, DWORD * lpProcessID )
{
   _LPWINDATA lpws = _GetHWNDPtr( hWnd );
   if( lpws )
   {
       if( lpProcessID )
	   {
           *lpProcessID = GetProcessId( lpws->hOwnerProcess );
	   }
       return lpws->dwThreadID;
   }
   return 0;
}


// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

// clear all window owned by thread when thread exit
// call by thread exit or process exit
BOOL WINAPI Win_Clear( DWORD dwThreadID )
{
    _LPWINDATA lpws;

    _LPWINDATA lpwsPrev, lpwsNext;

	if( !API_IsReady( API_GWE ) )
		return TRUE;


	LockCSWindow();

	lpws = hwndScreen.lpwsChild;

    while( lpws )
    {
			lpwsPrev = lpws->lpwsPrev;
			lpwsNext = lpws->lpwsNext;

			if( lpws->dwThreadID == dwThreadID )
			{
				DWORD dwOldPerm;

				ASSERT( lpws->lpClass );

				
				dwOldPerm = SetProcPermissions( ~0 );  //�õ��Խ��̵Ĵ�ȡȨ��
				EdbgOutputDebugString( "Error: ����δ�ƻ���Class=%s, Title=%s !\r\n", lpws->lpClass->wc.lpszClassName, lpws->lpWinText ? lpws->lpWinText : "" );
				SetProcPermissions( dwOldPerm );  //�ָ��Խ��̵Ĵ�ȡȨ��
                
				if( _Win_Destroy( lpws, dwThreadID, WDF_HIDE | WDF_CLEAR ) )
				{
					ASSERT (0);
				}

			    lpws = hwndScreen.lpwsChild;// again

			}
			else
				lpws = lpws->lpwsNext;
	}
	UnlockCSWindow();

	MQ_FreeThreadQueue();

	return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static BOOL CALLBACK DoHide( HWND hWnd, LPARAM lParam )
{
    if( (HWND)lParam != hWnd )
	{
		_LPWINDATA lpws;
		lpws = _GetHWNDPtr( hWnd );
		
		if( lpws )
		{
			_SetState( lpws, WS_MINIMIZE, TRUE );
		}
	}
	return TRUE;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

BOOL WINAPI Win_ShowDesktop( UINT uiFlag )
{
	CALLBACKDATA cd;
	HWND hDesktop;

	cd.hProcess = NULL;
	cd.lpfn = (FARPROC)DoHide;
	ASSERT( uiFlag == 0 );

	LockCSWindow();
	
	DoEnumWindow( hwndScreen.lpwsChild, FALSE, &cd, (LPARAM)(hDesktop = GetDesktopExplore()) );

	UnlockCSWindow();

	Win_InvalidateRect( hDesktop, NULL, TRUE );

	return TRUE;	
}

// **************************************************
// ������int WINAPI Win_SetRgn( HWND hWnd,   //���ھ��
//					   HRGN hrgn,   //��Ҫ���õ���
//					   BOOL bRedraw //�Ƿ������ػ�
//						)
// ������
//	IN hWnd -		//���ھ��
//	IN HRGN hrgn -  //��Ҫ���õ���
//	IN bRedraw -	//�Ƿ������ػ�
// ����ֵ��
//	����ɹ������ط�0�����򣬷���0
// ����������
//	  ���ô��ڵ���ʾ��
// ����: 
//	
// ************************************************

int WINAPI Win_SetRgn( HWND hWnd,   //���ھ��
					   HRGN hrgn,   //��Ҫ���õ���
					   BOOL bRedraw //�Ƿ������ػ�
						)
{
	_LPWINDATA lpws;
	lpws = _GetHWNDPtr( hWnd );
	
	if( lpws )
	{
		HRGN hrgnExposeOld, hrgnExposeNew;
		int retv = FALSE;
		DWORD fClip = (lpws->dwMainStyle & WS_CLIPCHILDREN) ? DCX_CLIPCHILDREN : 0;

		fClip |= DCX_CLIPSIBLINGS | DCX_WINDOW;

		
		hrgnExposeOld = hrgnExposeNew = NULL;

		if( IS_SHOWENABLE( lpws ) )
		{	//����
			// �õ�����֮ǰ�Ŀɼ�����
			GetExposeRgn( hWnd, fClip, &hrgnExposeOld );
		}
		// ������ܣ��ͷŵ�ǰ��
		if( lpws->hrgnWindow )
			DeleteObject( lpws->hrgnWindow );
		lpws->hrgnWindow = NULL;
		if( hrgn )
		{	//���µ�����������Ϊ ��Ļ����
			if( WinRgn_Offset( hrgn, lpws->rectWindow.left, lpws->rectWindow.top ) != ERROR )
			{  //��Ч�� hrgn
				lpws->hrgnWindow = hrgn;
			}			
		}

		if( IS_SHOWENABLE( lpws ) )
		{	//	����
			//  �õ�����֮��Ŀɼ�����
			GetExposeRgn( hWnd, fClip, &hrgnExposeNew );
		}

		if( hrgnExposeNew && hrgnExposeOld )
		{
			CombineRgn( hrgnExposeNew, hrgnExposeNew, hrgnExposeOld, RGN_OR );
			DeleteObject( hrgnExposeOld );
		}
		else if( hrgnExposeOld )
		{
			hrgnExposeNew = hrgnExposeOld;
		}
		if( hrgnExposeNew )
		{	// ������Ч����ϵͳ
			_AddPaintRegion( hrgnExposeNew );
			_AddPaintWndRgn( NULL, hrgnExposeNew, TRUE, GetCurrentThreadId() );
			DeleteObject( hrgnExposeNew );
		}

		if( bRedraw )
			Win_Update( hWnd );  //��������
		return 1;
	}
	return 0;
}

// **************************************************
// ������int WINAPI Win_SetRgn( HWND hWnd,   //���ھ��
//					   HRGN hrgn,   //��Ҫ���õ���
//						)
// ������
//	IN hWnd -		//���ھ��
//	IN/OUT HRGN hrgn -  //��Ҫ�õ�����, ������������
// ����ֵ��
//	NULLREGION - ����
//	SIMPLEREGION - һ����
//	COMPLEXREGION - ����һ����
//	ERROR - ����
// ����������
//	  ���ô��ڵ���ʾ��
// ����: 
//	
// ************************************************

int WINAPI Win_GetRgn(
					  HWND hWnd,  //���ھ��
					  HRGN hRgn	  //��Ҫ�����õ���
					)
{
	_LPWINDATA lpws;
	lpws = _GetHWNDPtr( hWnd );
	
	if( lpws )
	{
		if( lpws->hrgnWindow == NULL )
		{
			SetRectRgn( hRgn, 0, 0, 0, 0 );
			return NULLREGION;
		}
		else
		{
			if( CombineRgn( hRgn, lpws->hrgnWindow, NULL, RGN_COPY ) != ERROR )
			{
				return OffsetRgn( hRgn, -lpws->rectWindow.left, -lpws->rectWindow.top );
			}
		}
	}
	return ERROR;
}

