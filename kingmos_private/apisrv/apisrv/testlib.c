#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>


typedef BOOL ( WINAPI * PCARET_CREATE )( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight );
BOOL WINAPI Caret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_CREATE, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hBitmap, nWidth, nHeight );
#else

	PCARET_CREATE pCreate;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_CREATE, &pCreate, &cs ) )
	{
		retv = pCreate( hWnd, hBitmap, nWidth, nHeight );
		API_Leave(  );
	}
	return retv;
#endif
}

extern DWORD WINAPI SERVER_CALL( DWORD, ... );
BOOL WINAPI DDD( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_CREATE, 4 );
    cs.arg0 = (DWORD)API_Enter( API_GWE;
    return (DWORD)CALL_SERVER( &cs, CARET_CREATE, &pCreate, &cs ) )
	{
		retv = pCreate( hWnd, hBitmap, nWidth, nHeight );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PCARET_DESTROY )( void );
BOOL WINAPI Caret_Destroy( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_DESTROY, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PCARET_DESTROY pDestroy;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_DESTROY, &pDestroy, &cs ) )
	{
		retv = pDestroy();
		API_Leave(  );
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PCARET_GETBLINKTIME )( void );
UINT WINAPI Caret_GetBlinkTime( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_GETBLINKTIME, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PCARET_GETBLINKTIME pGetBlinkTime;
	CALLSTACK cs;
	UINT   retv = 0;

	if( API_Enter( API_GWE, CARET_GETBLINKTIME, &pGetBlinkTime, &cs ) )
	{
	    retv = pGetBlinkTime();
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PCARET_GETPOS )( LPPOINT );
BOOL WINAPI Caret_GetPos( LPPOINT lpPoint )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_GETPOS, 1 );
    cs.arg0 = (DWORD)lpPoint ;
    return (DWORD)CALL_SERVER( &cs );
#else

    PCARET_GETPOS pGetPos;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_GETPOS, &pGetPos, &cs ) )
	{
		lpPoint = MapProcessPtr( lpPoint, (LPPROCESS)cs.lpvData );

	    retv = pGetPos( lpPoint );
		API_Leave(  );
	}
	return retv;
#endif
}

//BOOL WINAPI KL_HideCaret( HWND hWnd, HDC hdc )
//{
//}

typedef BOOL ( WINAPI * PCARET_HIDE )( HWND );
BOOL WINAPI Caret_Hide( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_HIDE, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PCARET_HIDE pHide;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_HIDE, &pHide, &cs ) )
	{
	    retv = pHide( hWnd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PCARET_SETBLINKTIME )( UINT );
BOOL WINAPI Caret_SetBlinkTime( UINT uMSeconds )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_SETBLINKTIME, 1 );
    cs.arg0 = (DWORD)uMSeconds ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PCARET_SETBLINKTIME pSetBlinkTime;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_SETBLINKTIME, &pSetBlinkTime, &cs ) )
	{
	    retv = pSetBlinkTime(uMSeconds);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PCARET_SETPOS )( int x, int y );
BOOL WINAPI Caret_SetPos( int x, int y )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_SETPOS, 2 );
    cs.arg0 = (DWORD)x;
    return (DWORD)CALL_SERVER( &cs, y );
#else

	PCARET_SETPOS pSetPos;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_SETPOS, &pSetPos, &cs ) )
	{
	    retv = pSetPos(x, y);
		API_Leave(  );
	}
	return retv;
#endif
}

//BOOL WINAPI KL_ShowCaret( HWND hWnd, HDC hdc )
//{
//}

typedef BOOL ( WINAPI * PCARET_SHOW )( HWND );
BOOL WINAPI Caret_Show( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CARET_SHOW, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PCARET_SHOW pShow;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, CARET_SHOW, &pShow, &cs ) )
	{
	    retv = pShow(hWnd);
		API_Leave(  );
	}
	return retv;
#endif
}