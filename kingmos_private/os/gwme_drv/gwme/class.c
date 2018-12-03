/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：窗口类管理
版本号：2.0.0
开发时期：1999
作者：李林
修改记录：
	2004-02-21, WinClass_Register 用 hgwmeProcess 代替 GetCurrentProcess
    2003-05-23, 取消对　某些GetCurrentProcess的处理
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
// 声明：BOOL _InitialWndClassMgr( void )
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化窗口类管理器
// 引用: 
//	被 gwme 开始时加载
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
// 声明：void _DeInitialWndClassMgr( void )
// 参数：
// 	无
// 返回值：
//	无
// 功能描述：
//	与 _InitialWndClassMgr 相反，系统退出时释放相关的资源
// 引用: 
//	
// ************************************************

void _DeInitialWndClassMgr( void )
{
	DeleteCriticalSection( &csWndClass );
}

// **************************************************
// 声明：static _LPREGCLASS AllocClassStruct( int nClassNameLen, int nClsExtra, LPCWNDCLASS lpWndClass )
// 参数：
// 	IN nClassNameLen - 需要的类名长度
//	IN nClsExtra - 需要的类附加扩展长度
//	IN lpWndClass - 类结构指针
// 返回值：
//	假如成功，返回非NULL的 _LPREGCLASS 指针值；否则，返回NULL
// 功能描述：
//	分配类结构
// 引用: 
//	
// ************************************************
static _LPREGCLASS AllocAndInitClassStruct( int nClassNameLen, int nClsExtra, LPCWNDCLASS lpWndClass, HANDLE hCallerProcess )
{
	_LPREGCLASS lpMemClass;
	nClsExtra = (nClsExtra + 3) & (~3);  // 对齐4
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
// 声明：static VOID FreeClassStruct( _LPREGCLASS lpMemClass )
// 参数：
// 	IN lpMemClass - 类结构指针
// 返回值：
//	无
// 功能描述：
//	释放类结构
// 引用: 
//	
// ************************************************
static VOID FreeClassStruct( _LPREGCLASS lpMemClass )
{
	BlockHeap_FreeString( hgwmeBlockHeap, 0, (LPVOID)lpMemClass->wc.lpszClassName );
	BlockHeap_FreeString( hgwmeBlockHeap, 0, lpMemClass );
}


// **************************************************
// 声明：ATOM WINAPI WinClass_Register( LPCWNDCLASS lpWndClass )
// 参数：
// 	IN lpWndClass - WNDCLASS 结构指针，包含类信息
// 返回值：
//	假如成功，返回非0的ID值；否则，返回0
// 功能描述：
//	注册窗口类
// 引用: 
//	系统API
// ************************************************

#define DEBUG_CLASS_REGISTER 0
ATOM WINAPI WinClass_Register( LPCWNDCLASS lpWndClass )
{
    _LPREGCLASS lpMemClass;
	_LPPROCESS_CLASS lpProcessList;
	WNDCLASS wc;
	HANDLE hCallerProcess;
	DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register:Entry.\r\n" ) );
	//检查参数	
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
	//	得到呼叫者进程
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
	//	进入冲突段
    EnterCriticalSection( &csWndClass );

	DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register:_FindClass.\r\n" ) );
	//查找当前是否已经存在该类
	lpMemClass = _FindClass( lpWndClass->lpszClassName, hCallerProcess, lpWndClass->hInstance, &lpProcessList, NULL );
	
	if( lpMemClass == NULL )
	{	//类不存在
//		LOGBRUSH logBrush;
		int len;
		int clsLen = lpWndClass->cbClsExtra;

		DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: class not exist.\r\n" ) );
		len = strlen( lpWndClass->lpszClassName ) + sizeof( TCHAR );		
		if( lpProcessList == NULL )
		{	//该进程类链表未初始化，现在去初始化
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
		//分配一个类结构
		DEBUGMSG( DEBUG_CLASS_REGISTER, ( "WinClass_Register: alloc string.\r\n" ) );
		lpMemClass = (_LPREGCLASS)AllocAndInitClassStruct( len, clsLen, lpWndClass, hCallerProcess );
		
		if( lpMemClass )
		{	//初始化类对象
			//连接到进程类链表
			lpMemClass->lpNext = lpProcessList->lpRegClass;
			lpProcessList->lpRegClass = lpMemClass;
			DEBUGMSG( DEBUG_CLASS_REGISTER, ( "RegisterClass(%s) success,hCallerProcess=%x.\r\n", lpMemClass->wc.lpszClassName, hCallerProcess ) );
		}
	}
	//离开冲突段
	LeaveCriticalSection( &csWndClass );
    return (int)lpMemClass;
}

// **************************************************
// 声明：DWORD WINAPI WinClass_GetLong( HWND hWnd, int nIndex )
// 参数：
//	IN hwnd-窗口句柄
//	IN nIndex-索引值，包含：
//　　　GCL_MENUNAME-菜单名
//　　　GCL_HBRBACKGROUND-背景刷
//　　　GCL_HCURSOR-光标
//　　　GCL_HICON-位图
//　　　GCL_CBWNDEXTRA-窗口扩展尺寸
//　　　GCL_WNDPROC-窗口过程地址
//　　	GCL_STYLE-类风格
// 返回值：
//	成功：返回要求的32bit值
//	否则：返回0
// 功能描述：
//	得到类信息
// 引用: 
//	系统API
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
// 声明：DWORD WINAPI WinClass_SetLong( HWND hWnd, int nIndex, LONG lNewValue )
// 参数：
//	IN hWnd-窗口句柄
//	IN nIndex-索引值，包含：
//　　　GCL_MENUNAME-菜单名
//　　　GCL_HBRBACKGROUND-背景刷
//　　　GCL_HCURSOR-光标
//　　　GCL_HICON-位图
//　　　GCL_CBWNDEXTRA-窗口扩展尺寸
//　　　GCL_WNDPROC-窗口过程地址
//　　	GCL_STYLE-类风格
//	IN lNewValue-新的类值
// 返回值：
//	假如成功，返回之前的值；否则，返回0
// 功能描述：
//	设置类信息
// 引用: 
//	系统API
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
// 声明：BOOL WINAPI WinClass_GetInfo( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpWndClass )
// 参数：
//	IN hInstance-实例句柄
//	IN lpcClassName-类名
//	IN lpwc-WNDCLASS结构指针
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到类信息
// 引用: 
//	系统API
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
// 声明：int WINAPI WinClass_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance )
// 参数：
// 	IN lpcClassName-类名
//	IN hInstance-实例句柄
// 返回值：
//	假如成功，返回非零；否则，返回0
// 功能描述：
//	注销窗口类
// 引用: 
//	系统API
// ************************************************

int WINAPI WinClass_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance )
{
    _LPREGCLASS lpPrev;
	_LPREGCLASS p;
    _LPPROCESS_CLASS lpProcessList;
	BOOL bRetv = FALSE;

	HANDLE hCallerProcess = GetCallerProcess();

	EnterCriticalSection( &csWndClass );
	//查找匹配的窗口类	
	p = _FindClass( lpcClassName, hCallerProcess, hInstance, &lpProcessList, &lpPrev );

	if( p && p->nRefCount == 0 )
    {   //有效并且引用计数为0
		p->objType = OBJ_NULL;
		//从链表中移出
		if( lpPrev )
			lpPrev->lpNext = p->lpNext;
		else
			lpProcessList->lpRegClass = p->lpNext;
		//删除之前分配的资源
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
// 声明：BOOL AddClassRef( _LPREGCLASS lpClass )
// 参数：
// 	IN lpClass - _REGCLASS 结构指针
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	增加对类的引用计数
// 引用: 
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
// 声明：BOOL DecClassRef( _LPREGCLASS lpClass )
// 参数：
// 	IN lpClass - _REGCLASS 结构指针
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	与 AddClassRef 相反，减少对类的引用计数
// 引用: 
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
// 声明：_LPREGCLASS GetClassPtr( LPCSTR lpcClassName, HINSTANCE hInstance )
// 参数：
// 	IN lpcClassName - 类名
//	IN hInstance - 实例句柄
// 返回值：
//	假如成功，返回注册的类对象指针；否则，返回NULL
// 功能描述：
//	查找并返回 类名对应的 类对象指针
// 引用: 
//	
// ************************************************

_LPREGCLASS GetClassPtr( LPCSTR lpcClassName, HINSTANCE hInstance )
{
	_LPREGCLASS p;
	//得到呼叫者进程
	HANDLE hCallerProcess = GetCallerProcess();
	//查找进程里是否存在该类
	p = _FindClass( lpcClassName, hCallerProcess, hInstance, NULL, NULL );

	if( p == NULL )
	{	//没有找到，查找系统类
		extern HINSTANCE hgwmeInst;
		p = _FindClass( lpcClassName, hgwmeProcess, hgwmeInstance, NULL, NULL ); // find in gabol
	}
	return p;

}

// **************************************************
// 声明：static BOOL ClearProcessClass( _LPPROCESS_CLASS lpProcessList )
// 参数：
// 	IN lpProcessList - _PROCESS_CLASS 结构指针
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放所有属于进程的之前已经注册的类
// 引用: 
//	当进程退出后，清除该进程的类
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
		
		//          释放类资源
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
// 声明：BOOL CALLBACK WinClass_Clear( HANDLE hOwnerProcess )
// 参数：
// 	IN hOwnerProcess - 拥有者进程句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放所有属于进程的之前已经注册的类
// 引用: 
//	当进程退出后，清除该进程的类
// ************************************************

#define DEBUG_WINCLASS_CLEAR 0
BOOL CALLBACK WinClass_Clear( HANDLE hOwnerProcess )
{
	_LPPROCESS_CLASS lpPrev, p;

	if( !API_IsReady( API_GWE ) )
		return TRUE;	
	//进入冲突段
	EnterCriticalSection( &csWndClass );

	DEBUGMSG( DEBUG_WINCLASS_CLEAR, ( "Enter WinClass_Clear, hOwnerProcess=%x.\r\n", hOwnerProcess ) );

	p = lpProcessClassList;
	lpPrev = NULL;

	while( p )
	{	//是否属于进程拥有者
		if( p->hProcess == hOwnerProcess )
		{
			DEBUGMSG( DEBUG_WINCLASS_CLEAR, ( "WinClass_Clear:Find Process Class p=%x.\r\n", p ) );
			//清除
			ClearProcessClass( p );
			//从链表中清除
			if( p->lpRegClass == NULL )
			{
				if( lpPrev )
					lpPrev->lpNext = p->lpNext;
				else
					lpProcessClassList = p->lpNext;
				//释放结构
				BlockHeap_Free( hgwmeBlockHeap, 0, p, sizeof(_PROCESS_CLASS) );//free( p );
			}
			break;
		}
		lpPrev = p;
		p = p->lpNext;
	}
	DEBUGMSG( DEBUG_WINCLASS_CLEAR, ( "WinClass_Clear：Leave.\r\n" ) );

	LeaveCriticalSection( &csWndClass );
	return TRUE;
}


// **************************************************
// 声明：static _LPREGCLASS _FindClassInProcess( _LPREGCLASS p, LPCSTR lpClassName, HINSTANCE hInst, _LPREGCLASS * lpPrev )
// 参数：
// 	IN p - _REGCLASS结构指针
//	IN lpClassName - 需要匹配的类名
//	IN hInst - 实例句柄
//	OUT lpPrev - 用于保存已找到对象在链表中的前一个对象

// 返回值：
//	假如成功，返回_REGCLASS结构指针；否则，返回NULL
// 功能描述：
//	在进程类链表中发现 一个类
// 引用: 
//	
// ************************************************

static _LPREGCLASS _FindClassInProcess( _LPREGCLASS p, LPCSTR lpClassName, HINSTANCE hInst, _LPREGCLASS * lpPrev )
{
	while( p )
	{
		if( ( hInst == 0 || p->wc.hInstance == hInst ) && 
			stricmp( lpClassName, p->wc.lpszClassName ) == 0 )
		{	//找到
			return p;
		}
	    if( lpPrev )
		    *lpPrev = p;
		//下一个
		p = p->lpNext;
	}
	return NULL;
}

// **************************************************
// 声明：static _LPREGCLASS _FindClass( 
//								LPCSTR lpClassName,
//								HANDLE hOwnerProcess,
//								HINSTANCE hInst,
//								_LPPROCESS_CLASS * lppProcessClassList,
//								_LPREGCLASS * lpPrev
//							    )
// 参数：
//	IN lpClassName - 类名
//	IN hOwnerProcess - 进程句柄
//	IN hInst - 实例句柄
//	IN lppProcessClassList - 进程窗口类链表
//	OUT lpPrev - 用于保存前链表的前一个对象
// 返回值：
//	假如成功，返回 _REGCLASS 结构指针；否则，返回NULL
// 功能描述：
//	查找一个类，如果找到
// 引用: 
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
	//进入冲突段
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
		{	//匹配实例句柄
			if( ( lpreg = _FindClassInProcess( p->lpRegClass, lpClassName, hInst, lpPrev ) ) )
			{	//找到
				ASSERT( lpreg->hOwnerProcess == hOwnerProcess );
			    break;//return p;
			}
			//不匹配实例句柄
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
