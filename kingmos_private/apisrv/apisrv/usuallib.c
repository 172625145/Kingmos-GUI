/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eapisrv.h>
//#include <usualsrv.h>

#include <eglobmem.h>
#include <eclipbrd.h>
#include <euusual.h>


typedef HGLOBAL (WINAPI * PGLOBALALLOC)(  UINT uFlags,  DWORD dwBytes );
HGLOBAL WINAPI Gbl_Alloc( UINT uFlags,  DWORD dwBytes )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_ALLOC, 2 );
    cs.arg0 = (DWORD)uFlags;
    return (HGLOBAL)CALL_SERVER( &cs,  dwBytes );
#else
	PGLOBALALLOC pGlobalAlloc;

	CALLSTACK cs;
	HGLOBAL     retv = 0;

	if( API_Enter( API_USUAL, GBL_ALLOC, &pGlobalAlloc, &cs ) )
	{
		retv = pGlobalAlloc(uFlags,  dwBytes);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HGLOBAL (WINAPI * PGLOBALDISCARD)(  HGLOBAL hglbMem );
HGLOBAL WINAPI Gbl_Discard( HGLOBAL hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_DISCARD, 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (HGLOBAL)CALL_SERVER( &cs );
#else
	PGLOBALDISCARD pGlobalDiscard;

	CALLSTACK cs;
	HGLOBAL     retv = 0;

	if( API_Enter( API_USUAL, GBL_DISCARD, &pGlobalDiscard, &cs ) )
	{
		retv = pGlobalDiscard( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef UINT (WINAPI * PGLOBALFLAGS)(  HGLOBAL hglbMem );
UINT     WINAPI Gbl_Flags( HGLOBAL hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_FLAGS, 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (DWORD)CALL_SERVER( &cs );
#else
	PGLOBALFLAGS pGlobalFlags;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, GBL_FLAGS, &pGlobalFlags, &cs ) )
	{
		retv = pGlobalFlags( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HGLOBAL (WINAPI * PGLOBALFREE)(  HGLOBAL hglbMem );
HGLOBAL  WINAPI Gbl_Free( HGLOBAL hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_FREE, 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (HGLOBAL)CALL_SERVER( &cs );
#else
	PGLOBALFREE pGlobalFree;

	CALLSTACK cs;
	HGLOBAL     retv = 0;

	if( API_Enter( API_USUAL, GBL_FREE, &pGlobalFree, &cs ) )
	{
		retv = pGlobalFree( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef HGLOBAL (WINAPI * PGLOBALHANDLE)(  LPCVOID  hglbMem );
HGLOBAL  WINAPI Gbl_Handle( LPCVOID  hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_HANDLE , 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (HGLOBAL)CALL_SERVER( &cs );
#else
	PGLOBALHANDLE pGlobalHandle;

	CALLSTACK cs;
	HGLOBAL     retv = 0;

	if( API_Enter( API_USUAL, GBL_HANDLE , &pGlobalHandle, &cs ) )
	{
		retv = pGlobalHandle( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef LPVOID (WINAPI * PGLOBALLOCK)(  HGLOBAL hglbMem );
LPVOID   WINAPI Gbl_Lock( HGLOBAL hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_LOCK , 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (LPVOID)CALL_SERVER( &cs );
#else
	PGLOBALLOCK pGlobalLock;

	CALLSTACK cs;
	LPVOID     retv = 0;

	if( API_Enter( API_USUAL, GBL_LOCK , &pGlobalLock, &cs ) )
	{
		retv = pGlobalLock( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HGLOBAL (WINAPI * PGLOBALREALLOC)(  HGLOBAL hMem,   DWORD dwBytes, UINT uFlags );
HGLOBAL WINAPI Gbl_ReAlloc( HGLOBAL hMem,   DWORD dwBytes, UINT uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_REALLOC , 3 );
    cs.arg0 = (DWORD)hMem;
    return (HGLOBAL)CALL_SERVER( &cs,   dwBytes, uFlags );
#else
	PGLOBALREALLOC pGlobalReAlloc;

	CALLSTACK cs;
	HGLOBAL     retv = 0;

	if( API_Enter( API_USUAL, GBL_REALLOC , &pGlobalReAlloc, &cs ) )
	{
		retv = pGlobalReAlloc( hMem,   dwBytes, uFlags );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD  (WINAPI * PGLOBALSIZE)(  HGLOBAL hglbMem );
DWORD    WINAPI Gbl_Size( HGLOBAL hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_SIZE , 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (DWORD)CALL_SERVER( &cs );
#else
	PGLOBALSIZE pGlobalSize;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, GBL_SIZE , &pGlobalSize, &cs ) )
	{
		retv = pGlobalSize( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef BOOL (WINAPI * PGLOBALUNLOCK)(  HGLOBAL hglbMem );
BOOL  WINAPI Gbl_Unlock( HGLOBAL hglbMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, GBL_UNLOCK , 1 );
    cs.arg0 = (DWORD)hglbMem ;
    return (DWORD)CALL_SERVER( &cs );
#else
	PGLOBALUNLOCK pGlobalUnlock;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, GBL_UNLOCK , &pGlobalUnlock, &cs ) )
	{
		retv = pGlobalUnlock( hglbMem );
		API_Leave(  );
	}
	return retv;
#endif
}

/*******************************************************************************/
// Clipboard function
/*******************************************************************************/
typedef BOOL (WINAPI * POPENCLIPBOARD)(  HWND hWndNewOwner );
BOOL WINAPI Clip_Open(HWND hWndNewOwner)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, CLIP_OPEN, 1 );
    cs.arg0 = (DWORD)hWndNewOwner ;
    return (DWORD)CALL_SERVER( &cs );
#else
	POPENCLIPBOARD pOpenClipboard;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, CLIP_OPEN, &pOpenClipboard, &cs ) )
	{
		retv = pOpenClipboard( hWndNewOwner );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL (WINAPI * PCLOSECLIPBOARD)(  void );
BOOL WINAPI Clip_Close(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, CLIP_CLOSE, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else
	PCLOSECLIPBOARD pCloseClipboard;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, CLIP_CLOSE , &pCloseClipboard, &cs ) )
	{
		retv = pCloseClipboard( );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef HANDLE (WINAPI * PSETCLIPBOARDDATA)(  UINT uFormat, HANDLE hMem  );
HANDLE WINAPI Clip_SetData(UINT uFormat, HANDLE hMem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, CLIP_SETDATA , 2 );
    cs.arg0 = (DWORD)uFormat;
    return (HANDLE)CALL_SERVER( &cs, hMem );
#else
	PSETCLIPBOARDDATA pSetClipboardData;

	CALLSTACK cs;
	HANDLE     retv = 0;

	if( API_Enter( API_USUAL, CLIP_SETDATA , &pSetClipboardData, &cs ) )
	{
		retv = pSetClipboardData( uFormat, hMem );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HANDLE (WINAPI * PGETCLIPBOARDDATA)(  UINT uFormat );
HANDLE WINAPI Clip_GetData(UINT uFormat )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, CLIP_GETDATA , 1 );
    cs.arg0 = (DWORD)uFormat ;
    return (HANDLE)CALL_SERVER( &cs );
#else
	PGETCLIPBOARDDATA pGetClipboardData;

	CALLSTACK cs;
	HANDLE     retv = 0;

	if( API_Enter( API_USUAL, CLIP_GETDATA , &pGetClipboardData, &cs ) )
	{
		retv = pGetClipboardData( uFormat );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL (WINAPI * PEMPTYCLIPBOARD)( void );
BOOL WINAPI Clip_Empty( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, CLIP_EMPTY , 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else
	PEMPTYCLIPBOARD pEmptyClipboard;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, CLIP_EMPTY , &pEmptyClipboard, &cs ) )
	{
		retv = pEmptyClipboard( );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL (WINAPI * PISCLIPBOARDFORMATAVAILABLE)( UINT uFormat );
BOOL WINAPI Clip_IsFormatAvailable(UINT uFormat)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_USUAL, CLIP_ISFORMATAVAILABLE , 0 );
    cs.arg0 = (DWORD)uFormat ;
    return (DWORD)CALL_SERVER( &cs );
#else
	PISCLIPBOARDFORMATAVAILABLE pIsClipboardFormatAvailable;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_USUAL, CLIP_ISFORMATAVAILABLE , &pIsClipboardFormatAvailable, &cs ) )
	{
		retv = pIsClipboardFormatAvailable( uFormat );
		API_Leave(  );
	}
	return retv;
#endif
}
