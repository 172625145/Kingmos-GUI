/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵�������������
�汾�ţ�2.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
	2004-02-21, WinClass_Register �� hgwmeProcess ���� GetCurrentProcess
    2003-05-23, ȡ���ԡ�ĳЩGetCurrentProcess�Ĵ���
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <eapisrv.h>

#include <bheap.h>
#include <gwmeobj.h>
#include <epwin.h>
#include <winsrv.h>
#include <gdisrv.h>

static const COLORREF sysColor[21] = {
    CL_LIGHTGRAY,  //COLOR_SCROLLBAR         0
    CL_BLACK,      //COLOR_BACKGROUND        1
    CL_BLUE,       //COLOR_ACTIVECAPTION     2
    CL_DARKGRAY,   //COLOR_INACTIVECAPTION   3
    CL_LIGHTGRAY,  //COLOR_MENU              4
    CL_LIGHTGRAY,  //COLOR_WINDOW            5
    CL_LIGHTGRAY,  //COLOR_WINDOWFRAME       6
    CL_BLACK,      //COLOR_MENUTEXT          7
    CL_WHITE,      //COLOR_WINDOWTEXT        8
    CL_WHITE,      //COLOR_CAPTIONTEXT       9
    CL_WHITE,      //COLOR_ACTIVEBORDER      10
    CL_LIGHTGRAY,  //COLOR_INACTIVEBORDER    11
    CL_LIGHTGRAY,  //COLOR_APPWORKSPACE      12
    CL_BLUE,       //COLOR_HIGHLIGHT         13
    CL_WHITE,      //COLOR_HIGHLIGHTTEXT     14
    CL_LIGHTGRAY,  //COLOR_BTNFACE           15
    CL_DARKGRAY,   //COLOR_BTNSHADOW         16
    CL_LIGHTGRAY,  //COLOR_GRAYTEXT          17
    CL_BLACK,      //COLOR_BTNTEXT           18
    CL_LIGHTGRAY,  //COLOR_INACTIVECAPTIONTEXT 19
    CL_WHITE      //COLOR_BTNHIGHLIGHT      20
};

/////////////////////////////////////////////////////////////
static _LPPROCESS_CLASS lpProcessClassList = NULL;

static CRITICAL_SECTION csWndClass;

static _LPREGCLASS _FindClass( LPCSTR lpClassName,
							   HANDLE hOwnerProcess,
							   HINSTANCE hInst,
							   _LPPROCESS_CLASS * lppProcessClassList,
							   _LPREGCLASS * lpPrev
							    );


// **************************************************
// ������BOOL _InitialWndClassMgr( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ�������������
// ����: 
//	�� gwme ��ʼʱ����
// ************************************************

BOOL _InitialWndClassMgr( void )
{
	InitializeCriticalSection( &csWndClass );
#ifdef __DEBUG
	csWndClass.lpcsName = "CS-WC";
#endif
	return TRUE;
}

// **************************************************
// ������void _DeInitialWndClassMgr( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	�� _InitialWndClassMgr �෴��ϵͳ�˳�ʱ�ͷ���ص���Դ
// ����: 
//	
// ************************************************

void _DeInitialWndClassMgr( void )
{
	DeleteCriticalSection( &csWndClass );
}

// **************************************************
// ������static _LPREGCLASS AllocClassStruct( int nClassNameLen, int nClsExtra, LPCWNDCLASS lpWndClass )
// ������
// 	IN nClassNameLen - ��Ҫ����������
//	IN nClsExtra - ��Ҫ���฽����չ����
//	IN lpWndClass - ��ṹָ��
// ����ֵ��
//	����ɹ������ط�NULL�� _LPREGCLASS ָ��ֵ�����򣬷���NULL
// ����������
//	������ṹ
// ����: 
//	
// ************************************************
static _LPREGCLASS AllocAndInitClassStruct( int nClassNameLen, int nClsExtra, LPCWNDCLASS lpWndClass, HANDLE hCallerProcess )
{
	_LPREGCLASS lpMemClass;
	nClsExtra = (nClsExtra + 3) & (~3);  // ����4
    lpMemClass = (_LPREGCLASS)BlockHeap_AllocString( hgwmeBlockHeap, BLOCKHEAP_ZERO_MEMORY, sizeof( _REGCLASS ) + nClsExtra );
	
	if( lpMemClass )
	{
		LPTSTR lpszClassName = BlockHeap_AllocString( hgwmeBlockHeap, 0, nClassNameLen );
		if( lpszClassName )
		{
			lpMemClass->objType = OBJ_CLASS;
			lpMemClass->nRefCount = 0;
			lpMemClass->hOwnerProcess = hCallerProcess;
			lpMemClass->wc = *lpWndClass;
			lpMemClass->wc.lpszClassName = lpszClassName;
			strcpy( lpszClassName, lpWndClass->lpszClassName );

			if( lpMemClass->wc.hbrBackground && (ULONG)lpMemClass->wc.hbrBackground <= SYS_COLOR_NUM )
			{
				lpMemClass->wc.hbrBackground = WinSys_GetColorBrush( (ULONG)lpMemClass->wc.hbrBackground - 1 );
			}

			return lpMemClass;
		}
		BlockHeap_FreeString( hgwmeBlockHeap, 0, lpMemClass );
	}
	return NULL;
}

// **************************************************
// ������static VOID FreeClassStruct( _LPREGCLASS lpMemClass )
// ������
// 	IN lpMemClass - ��ṹָ��
// ����ֵ��
//	��
// ����������
//	�ͷ���ṹ
// ����: 
//	
// ************************************************
static VOID FreeClassStruct( _LPREGCLASS lpMemClass )
{
	BlockHeap_FreeString( hgwmeBlockHeap, 0, (LPVOID)lpMemClass->wc.lpszClassName );
	BlockHeap_FreeString( hgwmeBlockHeap, 0, lpMemClass );
}


// **************************************************
// ������ATOM WINAPI WinClass_Register( LPCWNDCLASS lpWndClass )
// ������
// 	IN lpWndClass - WNDCLASS �ṹָ�룬��������Ϣ
// ����ֵ��
//	����ɹ������ط�0��IDֵ�����򣬷���0
// ����������
//	ע�ᴰ����
// ����: 
//	ϵͳAPI
// ************************************************

#define DEBUG_CLASS_REGISTER 0
ATOM WINAPI WinClass_Register( LPCWNDCLASS lpWndClass )
{
    _LPREGCLASS lpMemClass;
	_LPPROCESS_CLASS lpProcessList;
	WNDCLASS wc;
	HANDLE hCallerProcess;
	DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register:Entry.\r\n" ) );
	//������	
	if( ( lpWndClass->hInstance == 0 && 
		  lpWndClass->lpfnWndProc == NULL &&
		  lpWndClass->lpszClassName == NULL ) || 
		  lpWndClass->cbClsExtra > 40 ||
		  (lpWndClass->cbClsExtra & 3) ||
		  lpWndClass->cbWndExtra > 40 ||
		  (lpWndClass->cbWndExtra & 3) )
	{
		ASSERT( 0 );
		SetLastError( ERROR_INVALID_PARAMETER );
		return 0;
	}
	//	�õ������߽���
	hCallerProcess = GetCallerProcess();

	if( hCallerProcess )
	{
		wc = *lpWndClass;
		wc.lpszClassName = MapPtrToProcess( (LPVOID)wc.lpszClassName, hCallerProcess );
		wc.lpszMenuName = MapPtrToProcess( (LPVOID)wc.lpszMenuName, hCallerProcess );
		lpWndClass = &wc;
	}
	else
		hCallerProcess = hgwmeProcess;
	//	�����ͻ��
    EnterCriticalSection( &csWndClass );

	DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register:_FindClass.\r\n" ) );
	//���ҵ�ǰ�Ƿ��Ѿ����ڸ���
	lpMemClass = _FindClass( lpWndClass->lpszClassName, hCallerProcess, lpWndClass->hInstance, &lpProcessList, NULL );
	
	if( lpMemClass == NULL )
	{	//�಻����
//		LOGBRUSH logBrush;
		int len;
		int clsLen = lpWndClass->cbClsExtra;

		DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: class not exist.\r\n" ) );
		len = strlen( lpWndClass->lpszClassName ) + sizeof( TCHAR );		
		if( lpProcessList == NULL )
		{	//�ý���������δ��ʼ��������ȥ��ʼ��
			DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: alloc _PROCESS_CLASS struct.\r\n" ) );
			lpProcessList = (_LPPROCESS_CLASS)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_PROCESS_CLASS) );
			
			if( lpProcessList )
			{
				DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: init lpProcessList.\r\n" ) );
				memset( lpProcessList, 0, sizeof( _PROCESS_CLASS ) );
				lpProcessList->hProcess = hCallerProcess;
				lpProcessList->lpNext = lpProcessClassList;
				lpProcessClassList = lpProcessList;
			}
			else
			{
				WARNMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: alloc _PROCESS_CLASS failure.\r\n" ) );
				LeaveCriticalSection( &csWndClass );
				return 0;
			}
		}
		//����һ����ṹ
		DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: alloc string.\r\n" ) );
		lpMemClass = (_LPREGCLASS)AllocAndInitClassStruct( len, clsLen, lpWndClass, hCallerProcess );
		
		if( lpMemClass )
		{	//��ʼ�������
			//���ӵ�����������
			lpMemClass->lpNext = lpProcessList->lpRegClass;
			lpProcessList->lpRegClass = lpMemClass;
			DEBUGMSG( DEBUG_CLASS_REGISTER, ( "RegisterClass(%s) success,hCallerProcess=%x.\r\n", lpMemClass->wc.lpszClassName, hCallerProcess ) );
		}
	}
	//�뿪��ͻ��
	LeaveCriticalSection( &csWndClass );
    return (int)lpMemClass;
}

// **************************************************
// ������DWORD WINAPI WinClass_GetLong( HWND hWnd, int nIndex )
// ������
//	IN hwnd-���ھ��
//	IN nIndex-����ֵ��������
//������GCL_MENUNAME-�˵���
//������GCL_HBRBACKGROUND-����ˢ
//������GCL_HCURSOR-���
//������GCL_HICON-λͼ
//������GCL_CBWNDEXTRA-������չ�ߴ�
//������GCL_WNDPROC-���ڹ��̵�ַ
//����	GCL_STYLE-����
// ����ֵ��
//	�ɹ�������Ҫ���32bitֵ
//	���򣺷���0
// ����������
//	�õ�����Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

DWORD WINAPI WinClass_GetLong( HWND hWnd, int nIndex )
{
    _LPREGCLASS lpwc;
	_LPWINDATA lpws = _GetHWNDPtr( hWnd );
	
	SetLastError( ERROR_SUCCESS );
	if( lpws )
	{
		lpwc = lpws->lpClass;
		if( lpwc && lpwc->objType == OBJ_CLASS )
		{
			switch( nIndex )
			{
            case GCL_MENUNAME:
                return (DWORD)lpwc->wc.lpszMenuName;
            case GCL_HBRBACKGROUND:
                return (DWORD)lpwc->wc.hbrBackground;
            case GCL_HCURSOR:
                return (DWORD)lpwc->wc.hCursor;
            case GCL_HICON:
                return (DWORD)lpwc->wc.hIcon;
            case GCL_CBWNDEXTRA:
                return (DWORD)lpwc->wc.cbWndExtra;
            case GCL_WNDPROC:
                return (DWORD)lpwc->wc.lpfnWndProc;
            case GCL_STYLE:
                return (DWORD)lpwc->wc.style;
            default:
				if( nIndex >= 0 && (nIndex & 3) && (nIndex + 4) <= lpwc->wc.cbClsExtra )
				{
					LPDWORD lpExtra = (LPDWORD)(&lpwc->wc + 1);
					return lpExtra[nIndex/4];
				}
                ASSERT( 0 );
			}
		}
	}
	SetLastError( ERROR_INVALID_PARAMETER );
    return 0;
}

// **************************************************
// ������DWORD WINAPI WinClass_SetLong( HWND hWnd, int nIndex, LONG lNewValue )
// ������
//	IN hWnd-���ھ��
//	IN nIndex-����ֵ��������
//������GCL_MENUNAME-�˵���
//������GCL_HBRBACKGROUND-����ˢ
//������GCL_HCURSOR-���
//������GCL_HICON-λͼ
//������GCL_CBWNDEXTRA-������չ�ߴ�
//������GCL_WNDPROC-���ڹ��̵�ַ
//����	GCL_STYLE-����
//	IN lNewValue-�µ���ֵ
// ����ֵ��
//	����ɹ�������֮ǰ��ֵ�����򣬷���0
// ����������
//	��������Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

#define DEBUG_CLASS_SETLONG 0
DWORD WINAPI WinClass_SetLong( HWND hWnd, int nIndex, LONG lNewValue )
{
    _LPREGCLASS lpwc;
	_LPWINDATA lpws = _GetHWNDPtr( hWnd );
	DWORD dwOldValue = 0;
	DWORD dwErrorCode = ERROR_SUCCESS;

	if( lpws )
	{
		lpwc = lpws->lpClass;

		if( lpws->hOwnerProcess == lpwc->hOwnerProcess
			&& lpwc->objType == OBJ_CLASS )
		{
			switch( nIndex )
			{
            case GCL_HBRBACKGROUND:
				dwOldValue = (DWORD)lpwc->wc.hbrBackground;
				lpwc->wc.hbrBackground = (HBRUSH)lNewValue;
				break;
            case GCL_HCURSOR:
				dwOldValue = (DWORD)lpwc->wc.hCursor;
				lpwc->wc.hCursor = (HCURSOR)lNewValue;
				break;
            case GCL_HICON:
				dwOldValue = (DWORD)lpwc->wc.hIcon;
				lpwc->wc.hIcon = (HICON)lNewValue;
				break;
            default:
				if( nIndex >= 0 && (nIndex & 3) && (nIndex + 4) <= lpwc->wc.cbClsExtra )
				{
					LPDWORD lpExtra = (LPDWORD)(&lpwc->wc + 1);
					dwOldValue = lpExtra[nIndex/4];
					lpExtra[nIndex/4] = lNewValue;
					break;
				}

                ASSERT( 0 );
				WARNMSG( DEBUG_CLASS_SETLONG, ( "error: SetClassLong not support the index=%x.\r\n", nIndex ) );
				dwErrorCode = ERROR_INVALID_PARAMETER;
                break;
			}
		}
	}
	else
		dwErrorCode = ERROR_INVALID_PARAMETER;
	SetLastError( dwErrorCode );
    return dwOldValue;
}

// **************************************************
// ������BOOL WINAPI WinClass_GetInfo( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpWndClass )
// ������
//	IN hInstance-ʵ�����
//	IN lpcClassName-����
//	IN lpwc-WNDCLASS�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ�����Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinClass_GetInfo( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpWndClass )
{
    _LPREGCLASS p;

	HANDLE hCallerProcess = GetCallerProcess();

	p = _FindClass( lpClassName, hCallerProcess, hInstance, NULL, NULL );
    if( p )
        *lpWndClass = *(&p->wc);
    return p ? TRUE : FALSE;
}

// **************************************************
// ������int WINAPI WinClass_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance )
// ������
// 	IN lpcClassName-����
//	IN hInstance-ʵ�����
// ����ֵ��
//	����ɹ������ط��㣻���򣬷���0
// ����������
//	ע��������
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinClass_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance )
{
    _LPREGCLASS lpPrev;
	_LPREGCLASS p;
    _LPPROCESS_CLASS lpProcessList;
	BOOL bRetv = FALSE;

	HANDLE hCallerProcess = GetCallerProcess();

	EnterCriticalSection( &csWndClass );
	//����ƥ��Ĵ�����	
	p = _FindClass( lpcClassName, hCallerProcess, hInstance, &lpProcessList, &lpPrev );

	if( p && p->nRefCount == 0 )
    {   //��Ч�������ü���Ϊ0
		p->objType = OBJ_NULL;
		//���������Ƴ�
		if( lpPrev )
			lpPrev->lpNext = p->lpNext;
		else
			lpProcessList->lpRegClass = p->lpNext;
		//ɾ��֮ǰ�������Դ
		if( p->wc.hbrBackground )
		    DeleteObject( p->wc.hbrBackground );
		if( p->wc.hCursor )
			WinGdi_DestroyCursor( p->wc.hCursor );
		if( p->wc.hIcon )
			WinGdi_DestroyIcon( p->wc.hIcon );
		FreeClassStruct( p );
        bRetv = TRUE;
    }
	LeaveCriticalSection( &csWndClass );
    return bRetv;
}

// **************************************************
// ������BOOL AddClassRef( _LPREGCLASS lpClass )
// ������
// 	IN lpClass - _REGCLASS �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���Ӷ�������ü���
// ����: 
//	
// ************************************************

BOOL AddClassRef( _LPREGCLASS lpClass )
{
	ASSERT( lpClass->objType == OBJ_CLASS );
	if( lpClass && lpClass->objType == OBJ_CLASS )
	{
		Interlock_Increment( (LPLONG)&lpClass->nRefCount );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������BOOL DecClassRef( _LPREGCLASS lpClass )
// ������
// 	IN lpClass - _REGCLASS �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�� AddClassRef �෴�����ٶ�������ü���
// ����: 
//	
// ************************************************

BOOL DecClassRef( _LPREGCLASS lpClass )
{
	ASSERT( lpClass->objType == OBJ_CLASS );
	if( lpClass && lpClass->objType == OBJ_CLASS && lpClass->nRefCount )
	{
		Interlock_Decrement( (LPLONG)&lpClass->nRefCount );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������_LPREGCLASS GetClassPtr( LPCSTR lpcClassName, HINSTANCE hInstance )
// ������
// 	IN lpcClassName - ����
//	IN hInstance - ʵ�����
// ����ֵ��
//	����ɹ�������ע��������ָ�룻���򣬷���NULL
// ����������
//	���Ҳ����� ������Ӧ�� �����ָ��
// ����: 
//	
// ************************************************

_LPREGCLASS GetClassPtr( LPCSTR lpcClassName, HINSTANCE hInstance )
{
	_LPREGCLASS p;
	//�õ������߽���
	HANDLE hCallerProcess = GetCallerProcess();
	//���ҽ������Ƿ���ڸ���
	p = _FindClass( lpcClassName, hCallerProcess, hInstance, NULL, NULL );

	if( p == NULL )
	{	//û���ҵ�������ϵͳ��
		extern HINSTANCE hgwmeInst;
		p = _FindClass( lpcClassName, hgwmeProcess, hgwmeInstance, NULL, NULL ); // find in gabol
	}
	return p;

}

// **************************************************
// ������static BOOL ClearProcessClass( _LPPROCESS_CLASS lpProcessList )
// ������
// 	IN lpProcessList - _PROCESS_CLASS �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷ��������ڽ��̵�֮ǰ�Ѿ�ע�����
// ����: 
//	�������˳�������ý��̵���
// ************************************************

#define DEBUG_CLEAR_PROCESS_CLASS 0

static BOOL ClearProcessClass( _LPPROCESS_CLASS lpProcessList )
{
	_LPREGCLASS lpNext;
	_LPREGCLASS p = lpProcessList->lpRegClass;

    //EnterCriticalSection( &csWndClass );
	lpProcessList->lpRegClass = NULL;

	lpNext = NULL;
	while( p )
	{
		if( p->nRefCount )
		{
			EdbgOutputDebugString( "Fatal Error: not free all refrence window class!!!" );
		}
		
		lpNext = p->lpNext;
		
		//          �ͷ�����Դ
		if( p->wc.hbrBackground )
			WinGdi_DeleteObject( p->wc.hbrBackground );
		if( p->wc.hCursor )
			WinGdi_DestroyCursor( p->wc.hCursor );
		if( p->wc.hIcon )
			WinGdi_DestroyIcon( p->wc.hIcon );
		p->objType = OBJ_NULL;
		FreeClassStruct( p );
		p = lpNext;
	}
	
	//LeaveCriticalSection( &csWndClass );

	return TRUE;
}

// **************************************************
// ������BOOL CALLBACK WinClass_Clear( HANDLE hOwnerProcess )
// ������
// 	IN hOwnerProcess - ӵ���߽��̾��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷ��������ڽ��̵�֮ǰ�Ѿ�ע�����
// ����: 
//	�������˳�������ý��̵���
// ************************************************

#define DEBUG_WINCLASS_CLEAR 0
BOOL CALLBACK WinClass_Clear( HANDLE hOwnerProcess )
{
	_LPPROCESS_CLASS lpPrev, p;

	if( !API_IsReady( API_GWE ) )
		return TRUE;	
	//�����ͻ��
	EnterCriticalSection( &csWndClass );

	DEBUGMSG( DEBUG_WINCLASS_CLEAR, ( "Enter WinClass_Clear, hOwnerProcess=%x.\r\n", hOwnerProcess ) );

	p = lpProcessClassList;
	lpPrev = NULL;

	while( p )
	{	//�Ƿ����ڽ���ӵ����
		if( p->hProcess == hOwnerProcess )
		{
			DEBUGMSG( DEBUG_WINCLASS_CLEAR, ( "WinClass_Clear:Find Process Class p=%x.\r\n", p ) );
			//���
			ClearProcessClass( p );
			//�����������
			if( p->lpRegClass == NULL )
			{
				if( lpPrev )
					lpPrev->lpNext = p->lpNext;
				else
					lpProcessClassList = p->lpNext;
				//�ͷŽṹ
				BlockHeap_Free( hgwmeBlockHeap, 0, p, sizeof(_PROCESS_CLASS) );//free( p );
			}
			break;
		}
		lpPrev = p;
		p = p->lpNext;
	}
	DEBUGMSG( DEBUG_WINCLASS_CLEAR, ( "WinClass_Clear��Leave.\r\n" ) );

	LeaveCriticalSection( &csWndClass );
	return TRUE;
}


// **************************************************
// ������static _LPREGCLASS _FindClassInProcess( _LPREGCLASS p, LPCSTR lpClassName, HINSTANCE hInst, _LPREGCLASS * lpPrev )
// ������
// 	IN p - _REGCLASS�ṹָ��
//	IN lpClassName - ��Ҫƥ�������
//	IN hInst - ʵ�����
//	OUT lpPrev - ���ڱ������ҵ������������е�ǰһ������

// ����ֵ��
//	����ɹ�������_REGCLASS�ṹָ�룻���򣬷���NULL
// ����������
//	�ڽ����������з��� һ����
// ����: 
//	
// ************************************************

static _LPREGCLASS _FindClassInProcess( _LPREGCLASS p, LPCSTR lpClassName, HINSTANCE hInst, _LPREGCLASS * lpPrev )
{
	while( p )
	{
		if( ( hInst == 0 || p->wc.hInstance == hInst ) && 
			stricmp( lpClassName, p->wc.lpszClassName ) == 0 )
		{	//�ҵ�
			return p;
		}
	    if( lpPrev )
		    *lpPrev = p;
		//��һ��
		p = p->lpNext;
	}
	return NULL;
}

// **************************************************
// ������static _LPREGCLASS _FindClass( 
//								LPCSTR lpClassName,
//								HANDLE hOwnerProcess,
//								HINSTANCE hInst,
//								_LPPROCESS_CLASS * lppProcessClassList,
//								_LPREGCLASS * lpPrev
//							    )
// ������
//	IN lpClassName - ����
//	IN hOwnerProcess - ���̾��
//	IN hInst - ʵ�����
//	IN lppProcessClassList - ���̴���������
//	OUT lpPrev - ���ڱ���ǰ�����ǰһ������
// ����ֵ��
//	����ɹ������� _REGCLASS �ṹָ�룻���򣬷���NULL
// ����������
//	����һ���࣬����ҵ�
// ����: 
//	
// ************************************************
#define DEBUG_FIND_CLASS 0
static _LPREGCLASS _FindClass( LPCSTR lpClassName,
							   HANDLE hOwnerProcess,
							   HINSTANCE hInst,
							   _LPPROCESS_CLASS * lppProcessClassList,
							   _LPREGCLASS * lpPrev
							    )
{
	_LPPROCESS_CLASS p;
	_LPREGCLASS lpreg = NULL;

	ASSERT( lpClassName );
	//�����ͻ��
	EnterCriticalSection( &csWndClass );

	DEBUGMSG( DEBUG_FIND_CLASS, ( "FindClass:Enter ClassName=%s,hOwnerProcess=%x.\r\n", lpClassName, hOwnerProcess ) );
	p = lpProcessClassList;
	if( lpPrev )
		*lpPrev = NULL;
	if( lppProcessClassList )
		*lppProcessClassList = NULL;

	while( p )
	{
		if( p->hProcess == hOwnerProcess )
		{	//ƥ��ʵ�����
			if( ( lpreg = _FindClassInProcess( p->lpRegClass, lpClassName, hInst, lpPrev ) ) )
			{	//�ҵ�
				ASSERT( lpreg->hOwnerProcess == hOwnerProcess );
			    break;//return p;
			}
			//��ƥ��ʵ�����
			lpreg = _FindClassInProcess( p->lpRegClass, lpClassName, 0, lpPrev );
			if( lppProcessClassList )
				*lppProcessClassList = p;
			break;
		}
		p = p->lpNext;
	}
	DEBUGMSG( DEBUG_FIND_CLASS, ( "_FindClass:Leave.\r\n" ) );

	LeaveCriticalSection( &csWndClass );
	return lpreg;
}
