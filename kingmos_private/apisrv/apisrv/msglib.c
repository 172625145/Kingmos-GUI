/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
//#include <equeue.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>

static void MapPtrParam( LPPROCESS lpProcess, UINT message, WPARAM * lpwParam, LPARAM * lplParam )
{
	//HANDLE hProcess = GetCallerProcess();
	if( lpProcess )
	{
		switch ( message )
		{
		case WM_GETTEXT:
		case WM_SETTEXT:
		case WM_WINDOWPOSCHANGING:
		case WM_WINDOWPOSCHANGED:
		case WM_STYLECHANGING:
		case WM_STYLECHANGED:
		case WM_NOTIFY:
		case WM_DRAWITEM:
		case WM_MEASUREITEM:
		case WM_DELETEITEM:
		case WM_NCCALCSIZE:
		case WM_HELP:
		case WM_CREATE:
		case WM_NCCREATE:
			//*lplParam = (LPARAM)MapPtrToProcess( (LPVOID)*lplParam, hProcess );
#ifdef CALL_TRAP
			*lplParam = (LPARAM)MapPtrToProcess( (LPVOID)*lplParam, (HANDLE)lpProcess );
#else
			*lplParam = (LPARAM)MapProcessPtr( *lplParam, lpProcess );
#endif
			break;
		}
	}
}


typedef BOOL ( WINAPI * PMSG_POST )( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL WINAPI Msg_Post( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_POST, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (BOOL)CALL_SERVER( &cs, uMsg, wParam, lParam );
#else

	PMSG_POST pPost;// = (PMSG_POST)lpMessageAPI[MSG_POST];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_POST, &pPost, &cs ) )
	{
		retv = pPost( hWnd, uMsg, wParam, lParam );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMSG_POSTTHREAD )( DWORD idThread, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL WINAPI Msg_PostThread( DWORD idThread, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_POSTTHREAD, 4 );
    cs.arg0 = (DWORD)idThread;
    return (DWORD)CALL_SERVER( &cs, uMsg, wParam, lParam );
#else

	PMSG_POSTTHREAD pPostThread;// = (PMSG_POSTTHREAD)lpMessageAPI[MSG_POSTTHREAD];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_POSTTHREAD, &pPostThread, &cs ) )
	{
		retv = pPostThread( idThread, uMsg, wParam, lParam );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMSG_GET )( LPMSG lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax );
BOOL WINAPI Msg_Get( LPMSG lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_GET, 4 );
    cs.arg0 = (DWORD)lpMsg;
    return (DWORD)CALL_SERVER( &cs, hWnd, wMsgFiltenMin, wMsgFiltenMax );
#else

	PMSG_GET pGet;// = (PMSG_GET)lpMessageAPI[MSG_GET];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_GET, &pGet, &cs ) )
	{
		lpMsg = MapProcessPtr( lpMsg, (LPPROCESS)cs.lpvData );
		retv = pGet( lpMsg, hWnd, wMsgFiltenMin, wMsgFiltenMax );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef  BOOL ( WINAPI * PMSG_PEEK )( LPMSG lpMsg, HWND hWnd, UINT wMsgFiltenMin, UINT wMsgFiltenMax, UINT wRemoveMsg ); 
BOOL WINAPI Msg_Peek( LPMSG lpMsg, 
					  HWND hWnd, 
					  UINT wMsgFiltenMin, 
					  UINT wMsgFiltenMax, 
					  UINT wRemoveMsg )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_PEEK, 5 );
    cs.arg0 = (DWORD)lpMsg;
    return (DWORD)CALL_SERVER( &cs, hWnd, wMsgFiltenMin, wMsgFiltenMax, wRemoveMsg );
#else

	PMSG_PEEK pPeek;// = (PMSG_PEEK)lpMessageAPI[MSG_PEEK];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_PEEK, &pPeek, &cs ) )
	{
		lpMsg = MapProcessPtr( lpMsg, (LPPROCESS)cs.lpvData );

		retv = pPeek( lpMsg, hWnd, wMsgFiltenMin, wMsgFiltenMax, wRemoveMsg );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef void ( WINAPI * PMSG_POSTQUIT )( int nExitCode );
void WINAPI Msg_PostQuit( int nExitCode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_POSTQUIT, 1 );
    cs.arg0 = (DWORD)nExitCode ;
    CALL_SERVER( &cs );
#else

	PMSG_POSTQUIT pPostQuit;// = (PMSG_POSTQUIT)lpMessageAPI[MSG_POSTQUIT];

	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_POSTQUIT, &pPostQuit, &cs ) )
	{
		pPostQuit( nExitCode );
		API_Leave(  );
	}

#endif
}

typedef LRESULT ( WINAPI * PMSG_DISPATCH )( const MSG FAR * lpMsg );
LRESULT WINAPI Msg_Dispatch( const MSG FAR * lpMsg )
{
#ifdef CALL_TRAP
    CALLTRAP cs;

	MSG msg;
	HANDLE hProcess = GetCurrentProcess();
	msg = *lpMsg;
	MapPtrParam( hProcess, msg.message, &msg.wParam, &msg.lParam );

    cs.apiInfo = CALL_API( API_MSG, MSG_DISPATCH, 1 );
    cs.arg0 = (DWORD)&msg ;
    return (LRESULT)CALL_SERVER( &cs );
#else

	PMSG_DISPATCH pDispatch;// = (PMSG_DISPATCH)lpMessageAPI[MSG_DISPATCH];

	CALLSTACK cs;
	LRESULT retv = 0;

	if( API_Enter( API_MSG, MSG_DISPATCH, &pDispatch, &cs ) )
	{
		MSG msg;
		lpMsg = MapProcessPtr( lpMsg, (LPPROCESS)cs.lpvData );
		msg = *lpMsg;
		MapPtrParam( (LPPROCESS)cs.lpvData, msg.message, &msg.wParam, &msg.lParam );

		retv = pDispatch( &msg );
		API_Leave(  );
	}
	return retv;
#endif
}
/*
typedef BOOL ( WINAPI * PMSG_TRANSLATE )( const MSG FAR * lpMsg );
BOOL WINAPI Msg_Translate( const MSG FAR * lpMsg )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_TRANSLATE, 1 );
    cs.arg0 = (DWORD)lpMsg ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMSG_TRANSLATE pTranslate;// = (PMSG_TRANSLATE)lpMessageAPI[MSG_TRANSLATE];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_TRANSLATE, &pTranslate, &cs ) )
	{
		lpMsg = MapProcessPtr( lpMsg, (LPPROCESS)cs.lpvData );

		retv = pTranslate( lpMsg );
		API_Leave(  );
	}
	return retv;
#endif
}
*/
typedef LRESULT ( WINAPI * PMSG_SEND )( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT WINAPI Msg_Send( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
	HANDLE hProcess = GetCurrentProcess();
	MapPtrParam( hProcess, msg, &wParam, &lParam );

    cs.apiInfo = CALL_API( API_MSG, MSG_SEND, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, msg, wParam, lParam );
#else

	PMSG_SEND pSend;// = (PMSG_SEND)lpMessageAPI[MSG_SEND];

	CALLSTACK cs;
	LRESULT  retv = 0;

	if( API_Enter( API_MSG, MSG_SEND, &pSend, &cs ) )
	{
		MapPtrParam( (LPPROCESS)cs.lpvData, msg, &wParam, &lParam );
		retv = pSend( hWnd, msg, wParam, lParam );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LRESULT ( WINAPI * PWND_CALLPROC )( WNDPROC, HWND, UINT, WPARAM, LPARAM );
LRESULT WINAPI Wnd_CallProc( WNDPROC lpwProc, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
	
	HANDLE hProcess = GetCurrentProcess();
	MapPtrParam( hProcess, msg, &wParam, &lParam );

    cs.apiInfo = CALL_API( API_WND, WND_CALLPROC, 5 );
    cs.arg0 = (DWORD)lpwProc;
    return (DWORD)CALL_SERVER( &cs, hWnd, msg, wParam, lParam );
#else

	PWND_CALLPROC pCallProc;// = (PWND_CALLPROC)lpWindowAPI[WND_CALLPROC];

	LRESULT  lrRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_CALLPROC, &pCallProc, &cs ) )
	{
		MapPtrParam( (LPPROCESS)cs.lpvData, msg, &wParam, &lParam );
		lrRetv = pCallProc( lpwProc, hWnd, msg, wParam, lParam );
		API_Leave(  );
	}
	return lrRetv;
#endif
}


typedef BOOL ( WINAPI * PMSG_WAIT )( void );
BOOL WINAPI Msg_Wait( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_WAIT, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMSG_WAIT pWait;// = (PMSG_WAIT)lpMessageAPI[MSG_WAIT];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_WAIT, &pWait, &cs ) )
	{
		retv = pWait();
		API_Leave(  );
	}
	return retv;
#endif	
}

typedef BOOL ( WINAPI * PKEYBD_EVENT )( BYTE bVk, BYTE bScan, DWORD dwFlags, DWORD dwExtraInfo );

VOID WINAPI KeybdEvent( 
				 BYTE bVk, 
				 BYTE bScan, 
				 DWORD dwFlags, 
				 DWORD dwExtraInfo )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, KEYBD_EVENT, 4 );
    cs.arg0 = (DWORD)bVk;
    CALL_SERVER( &cs, bScan, dwFlags, dwExtraInfo );
#else

	PKEYBD_EVENT pfn;
	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_MSG, KEYBD_EVENT, &pfn, &cs ) )
	{
		pfn( bVk, bScan, dwFlags, dwExtraInfo );
		API_Leave(  );
	}
	//return retv;
#endif
}

typedef VOID ( WINAPI * PMOUSE_EVENT )( DWORD dwFlags, DWORD dx, DWORD dy, DWORD dwData, DWORD dwExtraInfo );

VOID WINAPI MouseEvent(
				 DWORD dwFlags, 
				 DWORD dx, 
				 DWORD dy, 
				 DWORD dwData, 
				 DWORD dwExtraInfo )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MOUSE_EVENT, 5 );
    cs.arg0 = (DWORD)dwFlags;
    CALL_SERVER( &cs, dx, dy, dwData, dwExtraInfo );
#else

	PMOUSE_EVENT pfn;
	CALLSTACK cs;
	//BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MOUSE_EVENT, &pfn, &cs ) )
	{
		pfn( dwFlags, dx, dy, dwData, dwExtraInfo );
		API_Leave(  );
	}
	//return retv;
#endif
}


//typedef BOOL ( CALLBACK * PMQ_FREETHREADQUEUE )( _MSGQUEUE * );
//BOOL CALLBACK MQ_FreeThreadQueue( _MSGQUEUE * lpQueue )
//{
//	PMQ_FREETHREADQUEUE pFreeThreadQueue;

//	CALLSTACK cs;
//	BOOL   retv = FALSE;

//	if( API_Enter( API_MSG, MQ_FREETHREADQUEUE, &pFreeThreadQueue, &cs ) )
//	{
//		retv = pFreeThreadQueue(lpQueue);
//		API_Leave(  );
//	}
//	return retv;
//}


typedef DWORD ( WINAPI * PMSG_WAITFORMULTIPLEOBJECTS )(
													   DWORD nCount,
													   LPHANDLE pHandles,
													   BOOL fWaitAll,
													   DWORD dwMilliseconds,
													   DWORD dwWakeMask
													   );

DWORD WINAPI Msg_WaitForMultipleObjects(
									   DWORD nCount,
									   LPHANDLE pHandles,
									   BOOL fWaitAll,
									   DWORD dwMilliseconds,
									   DWORD dwWakeMask
									   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_WAITFORMULTIPLEOBJECTS, 5 );
    cs.arg0 = (DWORD)
		                               nCount;
    return (DWORD)CALL_SERVER( &cs,
									   pHandles,
									   fWaitAll,
									   dwMilliseconds,
									   dwWakeMask );
#else

	PMSG_WAITFORMULTIPLEOBJECTS pfn;// = (PMSG_WAIT)lpMessageAPI[MSG_WAIT];

	CALLSTACK cs;
	DWORD   retv = WAIT_FAILED;

	if( API_Enter( API_MSG, MSG_WAITFORMULTIPLEOBJECTS, &pfn, &cs ) )
	{
		retv = pfn(nCount,
				   pHandles,
				   fWaitAll,
				   dwMilliseconds,
				   dwWakeMask );
		API_Leave();
	}
	return retv;
#endif	
}


typedef BOOL ( WINAPI * PMSG_BEEP )( UINT uType );
BOOL WINAPI Msg_Beep( UINT uType )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_BEEP, 1 );
    cs.arg0 = (DWORD)uType ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMSG_BEEP pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_MSG, MSG_BEEP, &pfn, &cs ) )
	{
		retv = pfn(uType);
		API_Leave(  );
	}
	return retv;
#endif	
}


typedef BOOL ( WINAPI * PMSG_GETKEYSTATE )( int nVirtKey );
SHORT WINAPI Msg_GetKeyState( int nVirtKey )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_GETKEYSTATE, 1 );
    cs.arg0 = (DWORD)nVirtKey ;
    return (SHORT)CALL_SERVER( &cs );
#else

	PMSG_GETKEYSTATE pfn;

	CALLSTACK cs;
	SHORT   retv = 0;

	if( API_Enter( API_MSG, MSG_GETKEYSTATE, &pfn, &cs ) )
	{
		retv = pfn( nVirtKey );
		API_Leave();
	}
	return retv;
#endif	
}

typedef LRESULT ( WINAPI * PMSG_SENDTIMEOUT )( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, PDWORD lpdwResult );
LRESULT WINAPI Msg_SendTimeout( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT fuFlags, UINT uTimeout, PDWORD lpdwResult )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_MSG, MSG_SENDTIMEOUT, 7 );
    cs.arg0 = (DWORD)hWnd ;
    return (SHORT)CALL_SERVER( &cs, message, wParam, lParam, fuFlags, uTimeout, lpdwResult );
#else

	PMSG_SENDTIMEOUT pfn;

	CALLSTACK cs;
	LRESULT   retv = 0;

	if( API_Enter( API_MSG, MSG_SENDTIMEOUT, &pfn, &cs ) )
	{
		retv = pfn( hWnd, message, wParam, lParam, fuFlags, uTimeout, lpdwResult );
		API_Leave();
	}
	return retv;
#endif	
}
