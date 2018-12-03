/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>


typedef ATOM ( WINAPI * PCLASS_REGISTER )( LPCWNDCLASS );
ATOM WINAPI Class_Register( LPCWNDCLASS lpwc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, CLASS_REGISTER, 1 );
    cs.arg0 = (DWORD)lpwc ;
    return (ATOM)CALL_SERVER( &cs );
#else

	PCLASS_REGISTER pRegister;// = (PCLASS_REGISTER)lpClassAPI[CLASS_REGISTER];

	CALLSTACK cs;
	ATOM   retv = 0;

	if( API_Enter( API_WND, CLASS_REGISTER, &pRegister, &cs ) )
	{
		lpwc = MapProcessPtr( lpwc, (LPPROCESS)cs.lpvData );

		retv = pRegister( lpwc );
		API_Leave(  );
	}
	return retv;

//	if( pRegister )
//	{
//		return pRegister( lpwc );
//	}
//	return 0;
#endif
}

typedef BOOL ( WINAPI * PCLASS_GETINFO )( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpwc );
BOOL WINAPI Class_GetInfo( HINSTANCE hInstance , LPCSTR lpClassName, LPWNDCLASS lpwc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, CLASS_GETINFO, 3 );
    cs.arg0 = (DWORD)hInstance;
    return (DWORD)CALL_SERVER( &cs, lpClassName, lpwc );
#else

	PCLASS_GETINFO pGetInfo;// = (PCLASS_GETINFO)lpClassAPI[CLASS_GETINFO];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_WND, CLASS_GETINFO, &pGetInfo, &cs ) )
	{
		lpClassName = MapProcessPtr( lpClassName, (LPPROCESS)cs.lpvData );
		lpwc = MapProcessPtr( lpwc, (LPPROCESS)cs.lpvData );

		retv = pGetInfo( hInstance, lpClassName, lpwc );
		API_Leave(  );
	}
	return retv;

//	if( pGetInfo )
//	{
//		return pGetInfo( hInstance, lpClassName, lpwc );
//	}
//	return FALSE;
#endif
}

typedef int ( WINAPI * PCLASS_UNREGISTER )( LPCSTR lpcClassName, HINSTANCE hInstance );
int WINAPI Class_Unregister( LPCSTR lpcClassName, HINSTANCE hInstance )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, CLASS_UNREGISTER, 2 );
    cs.arg0 = (DWORD)lpcClassName;
    return (DWORD)CALL_SERVER( &cs, hInstance );
#else

	PCLASS_UNREGISTER pUnregister;// = (PCLASS_UNREGISTER)lpClassAPI[CLASS_UNREGISTER];

	CALLSTACK cs;
	int   retv = 0;

	if( API_Enter( API_WND, CLASS_UNREGISTER, &pUnregister, &cs ) )
	{
		lpcClassName = MapProcessPtr( lpcClassName, (LPPROCESS)cs.lpvData );

		retv = pUnregister( lpcClassName, hInstance );
		API_Leave(  );
	}
	return retv;

//	if( pUnregister )
//	{
//		return pUnregister( lpcClassName, hInstance );
//	}
//	return 0;
#endif
}


typedef DWORD ( WINAPI * PCLASS_GETLONG )( HWND, int );
DWORD WINAPI Class_GetLong( HWND hWnd, int nIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, CLASS_GETLONG, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, nIndex );
#else

	PCLASS_GETLONG pGetLong;// = (PCLASS_GETLONG)lpClassAPI[CLASS_GETLONG];

	CALLSTACK cs;
	DWORD   retv = 0;

	if( API_Enter( API_WND, CLASS_GETLONG, &pGetLong, &cs ) )
	{
		retv = pGetLong( hWnd, nIndex );
		API_Leave(  );
	}
	return retv;

//	if( pGetLong )
//	{
//		return pGetLong( hWnd, nIndex );
//	}
//	return 0;
#endif
}

typedef DWORD ( WINAPI * PCLASS_SETLONG )( HWND, int, LONG lNewValue );
DWORD WINAPI Class_SetLong( HWND hWnd, int nIndex, LONG lNewValue )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, CLASS_SETLONG, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, nIndex, lNewValue );
#else

	PCLASS_SETLONG pSetLong;// = (PCLASS_GETLONG)lpClassAPI[CLASS_GETLONG];

	CALLSTACK cs;
	DWORD   retv = 0;

	if( API_Enter( API_WND, CLASS_SETLONG, &pSetLong, &cs ) )
	{
		retv = pSetLong( hWnd, nIndex, lNewValue );
		API_Leave(  );
	}
	return retv;

#endif
}

/*
typedef BOOL ( CALLBACK * PCLASS_CLEAR )( HINSTANCE );
BOOL CALLBACK Class_Clear( HINSTANCE hInst )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, CLASS_CLEAR, 1 );
    cs.arg0 = (DWORD)hInst ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PCLASS_CLEAR pClear;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_WND, CLASS_CLEAR, &pClear, &cs ) )
	{
		retv = pClear( hInst );
		API_Leave(  );
	}
	return retv;
#endif
}
*/