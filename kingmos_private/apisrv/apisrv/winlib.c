/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>
#include <epcore.h>

//#define ENTER_SERVER( idServer )  
///	CALLSTACK cs;
//	 = 

//#define LEAVE_SERVER( 

//extern const PFNVOID lpWindowAPI[];

static UINT IsSysClass( LPCTSTR lpszClassName )
{
	/*
	if( stricmp( lpszClassName, classBUTTON ) )
	{
		return TRUE;
	}
	if( stricmp( lpszClassName, classSTATIC ) )
	{
		return TRUE;
	}
	if( stricmp( lpszClassName, classCOMBOBOX ) )
	{
		return TRUE;
	}
	if( stricmp( lpszClassName, classEDIT ) )
	{
		return TRUE;
	}
	if( stricmp( lpszClassName, classLISTBOX ) )
	{
		return TRUE;
	}
	if( stricmp( lpszClassName, classCOMBOLISTBOX ) )
	{
		return TRUE;
	}
	if( stricmp( lpszClassName, classMENU ) )
	{
		return TRUE;
	}
	return FALSE;
*/
	/*
	if( stricmp( lpszClassName, classBUTTON ) )
	{
		return WSC_BUTTON;
	}
	if( stricmp( lpszClassName, classSTATIC ) )
	{
		return WSC_STATIC;
	}
	if( stricmp( lpszClassName, classCOMBOBOX ) )
	{
		return WSC_COMBOBOX;
	}
	if( stricmp( lpszClassName, classEDIT ) )
	{
		return WSC_EDIT;
	}
	if( stricmp( lpszClassName, classLISTBOX ) )
	{
		return WSC_LISTBOX;
	}
	if( stricmp( lpszClassName, classCOMBOLISTBOX ) )
	{
		return WSC_COMBOLISTBOX;
	}
	if( stricmp( lpszClassName, classMENU ) )
	{
		return WSC_MENU;
	}
	*/
}

typedef HWND ( WINAPI * PWND_CREATEEX )( DWORD dwExStyle,
                     LPCTSTR lpcClassName,
		             LPCTSTR lpcText,
		             DWORD dwMainStyle,
		             int x, int y, int dx, int dy,
		             HWND hParent,
		             HMENU hMenu,
		             HINSTANCE hInstance,
		             LPVOID lpCreateParam );
 
HWND WINAPI Wnd_CreateEx(   
					 DWORD dwExStyle,
                     LPCTSTR lpcClassName,
		             LPCTSTR lpcText,
		             DWORD dwMainStyle,
		             int x, int y, int dx, int dy,
		             HWND hParent,
		             HMENU hMenu,
		             HINSTANCE hInstance,
		             LPVOID lpCreateParam )
{
	HWND hWnd;
	int loop = 0;
	
	

_again:

#ifdef CALL_TRAP
	{
		CALLTRAP cs;
		cs.apiInfo = CALL_API( API_WND, WND_CREATEEX, 12 );
		cs.arg0 = (DWORD)dwExStyle;
		hWnd = (HWND)CALL_SERVER( &cs, lpcClassName, lpcText, dwMainStyle, x, y, dx, dy, hParent, hMenu, hInstance, lpCreateParam );
	}
#else

	{
		//PWND_CREATEEX lpCreateEx;// = (PWND_CREATEEX)lpWindowAPI[WND_CREATEEX];
		CALLSTACK cs;
		//HWND hWnd = NULL;
		PWND_CREATEEX lpCreateEx;
		
		if( API_Enter( API_WND, WND_CREATEEX, &lpCreateEx, &cs ) )
		{
			lpcClassName = MapProcessPtr( lpcClassName, (LPPROCESS)cs.lpvData );
			lpcText = MapProcessPtr( lpcText, (LPPROCESS)cs.lpvData );
			
			hWnd = lpCreateEx( dwExStyle, lpcClassName, lpcText, dwMainStyle, x, y, dx, dy, hParent, hMenu, hInstance, lpCreateParam );
			API_Leave(  );
		}
	}
	//return hWnd;
#endif

	if( hWnd == NULL && loop == 0 )
	{
		DEBUGMSG( 0, ( "init sys class++.\r\n" ) );
		//if( IsSysClass(lpcClassName) )
		{
			extern BOOL InitialSystemWindowClass( HINSTANCE hInst );
			WNDCLASS wc;
			HINSTANCE hInst;
			//RETAILMSG( 1, ( "init sys class001.\r\n" ) );
			hInst = (HINSTANCE)GetModuleHandle( NULL );
			//RETAILMSG( 1, ( "init sys class002.\r\n" ) );
			
			if( GetClassInfo( hInst, classBUTTON, &wc ) == FALSE )
			{  //系统类没有初始化或不存在
				//RETAILMSG( 1, ( "init sys class003.\r\n" ) );
				InitialSystemWindowClass(hInst);				
				loop ++;
				//RETAILMSG( 1, ( "init sys class--.\r\n" ) );
				goto _again;
			}
		}
	}

	return hWnd;
}

typedef int ( WINAPI * PWND_GETCLASSNAME )( HWND hWnd, LPTSTR lpszClassName, int nMaxCount );
int WINAPI Wnd_GetClassName( HWND hWnd, LPTSTR lpszClassName, int nMaxCount )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETCLASSNAME, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpszClassName, nMaxCount );
#else

	PWND_GETCLASSNAME lpGetClassName;

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_WND, WND_GETCLASSNAME, &lpGetClassName, &cs ) )
	{
		lpszClassName = MapProcessPtr( lpszClassName, (LPPROCESS)cs.lpvData );

		retv = lpGetClassName( hWnd, lpszClassName, nMaxCount );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PWND_ISWINDOW )( HWND );
BOOL WINAPI Wnd_IsWindow( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ISWINDOW, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_ISWINDOW pIsWindow;// = (PWND_ISWINDOW)lpWindowAPI[WND_ISWINDOW];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_WND, WND_ISWINDOW, &pIsWindow, &cs ) )
	{
		bRetv = pIsWindow( hWnd ); 
		API_Leave(  );
	}
	return bRetv;
#endif
}

/*
typedef BOOL ( WINAPI * PWND_SHOWSCROLLBAR )( HWND, int, BOOL );
BOOL WINAPI Wnd_ShowScrollBar( HWND hWnd, int fnBar, BOOL bShow )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SHOWSCROLLBAR, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, fnBar, bShow );
#else

	PWND_SHOWSCROLLBAR pShowScrollBar;// = (PWND_SHOWSCROLLBAR)lpWindowAPI[WND_SHOWSCROLLBAR];

	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_WND, WND_SHOWSCROLLBAR, &pShowScrollBar, &cs ) )
	{
		bRetv = pShowScrollBar( hWnd, fnBar, bShow );
		API_Leave(  );
	}
	return bRetv;
#endif
}
*/

/*
typedef LRESULT ( WINAPI * PWND_DEFPROC )( HWND, UINT, WPARAM, LPARAM );
LRESULT WINAPI Wnd_DefProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_DEFPROC, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, msg, wParam, lParam );
#else

	PWND_DEFPROC pDefProc;// = (PWND_DEFPROC)lpWindowAPI[WND_DEFPROC];

	CALLSTACK cs;
	LRESULT  bRetv = -1;

	if( API_Enter( API_WND, WND_DEFPROC, &pDefProc, &cs ) )
	{
		bRetv = pDefProc( hWnd, msg, wParam, lParam );
		API_Leave(  );
	}
	return bRetv;
#endif
}
*/

typedef BOOL ( WINAPI * PWND_DESTROY )( HWND );
BOOL WINAPI Wnd_Destroy( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_DESTROY, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_DESTROY pDestroy;// = (PWND_DESTROY)lpWindowAPI[WND_DESTROY];
	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_WND, WND_DESTROY, &pDestroy, &cs ) )
	{
		bRetv = pDestroy( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_ISCHILD )( HWND, HWND );
BOOL WINAPI Wnd_IsChild( HWND hwndParent, HWND hwndChild )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ISCHILD, 2 );
    cs.arg0 = (DWORD)hwndParent;
    return (DWORD)CALL_SERVER( &cs, hwndChild );
#else

	PWND_ISCHILD pIsChild;// = (PWND_ISCHILD)lpWindowAPI[WND_ISCHILD];
	CALLSTACK cs;
	BOOL  bRetv = FALSE;

	if( API_Enter( API_WND, WND_ISCHILD, &pIsChild, &cs ) )
	{
		bRetv = pIsChild( hwndParent, hwndChild );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETPARENT )( HWND );
HWND WINAPI Wnd_GetParent( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETPARENT, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETPARENT pGetParent;// = (PWND_GETPARENT)lpWindowAPI[WND_GETPARENT];
	CALLSTACK cs;
	HWND  hWndRet = NULL;

	if( API_Enter( API_WND, WND_GETPARENT, &pGetParent, &cs ) )
	{
		hWndRet = pGetParent( hWnd );
		API_Leave(  );
	}
	return hWndRet;
#endif
}

typedef HWND ( WINAPI * PWND_SETPARENT )( HWND hwndChild, HWND hwndParent );
HWND WINAPI Wnd_SetParent( HWND hwndChild, HWND hwndParent )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETPARENT, 2 );
    cs.arg0 = (DWORD)hwndChild;
    return (HWND)CALL_SERVER( &cs, hwndParent );
#else

	PWND_SETPARENT pSetParent;// = (PWND_SETPARENT)lpWindowAPI[WND_SETPARENT];

	CALLSTACK cs;
	HWND  hWndRet = NULL;

	if( API_Enter( API_WND, WND_SETPARENT, &pSetParent, &cs ) )
	{
		hWndRet = pSetParent( hwndChild, hwndParent );
		API_Leave(  );
	}
	return hWndRet;
#endif	
}

typedef BOOL ( WINAPI * PWND_ISVISIBLE )( HWND );
BOOL WINAPI Wnd_IsVisible( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ISVISIBLE, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_ISVISIBLE pIsVisible;// = (PWND_ISVISIBLE)lpWindowAPI[WND_ISVISIBLE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ISVISIBLE, &pIsVisible, &cs ) )
	{
		bRetv = pIsVisible( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_SHOW )( HWND, int );
BOOL WINAPI Wnd_Show(HWND hWnd, int nCmdShow)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SHOW, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, nCmdShow );
#else

	PWND_SHOW pShow;// = (PWND_SHOW)lpWindowAPI[WND_SHOW];
	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SHOW, &pShow, &cs ) )
	{
		bRetv = pShow( hWnd, nCmdShow );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_ENABLE )( HWND, BOOL );
BOOL WINAPI Wnd_Enable( HWND hWnd, BOOL bEnable )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ENABLE, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, bEnable );
#else

	PWND_ENABLE pEnable;// = (PWND_ENABLE)lpWindowAPI[WND_ENABLE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ENABLE, &pEnable, &cs ) )
	{
		bRetv = pEnable( hWnd, bEnable );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PWND_ISENABLED )( HWND );
BOOL WINAPI Wnd_IsEnabled( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ISENABLED, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_ISENABLED pIsEnable;// = (PWND_ISENABLED)lpWindowAPI[WND_ISENABLED];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ISENABLED, &pIsEnable, &cs ) )
	{
		bRetv = pIsEnable( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_SETTEXT )( HWND, LPCSTR );
BOOL WINAPI Wnd_SetText(HWND hWnd, LPCSTR lpcstr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETTEXT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpcstr );
#else

	PWND_SETTEXT pSetText;// = (PWND_SETTEXT)lpWindowAPI[WND_SETTEXT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETTEXT, &pSetText, &cs ) )
	{
		lpcstr = MapProcessPtr( lpcstr, (LPPROCESS)cs.lpvData );

		bRetv = pSetText( hWnd, lpcstr );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PWND_GETTEXT )( HWND, LPSTR, int );
int WINAPI Wnd_GetText( HWND hWnd, LPSTR lpstr, int nMax )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETTEXT, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpstr, nMax );
#else

	PWND_GETTEXT pGetText;// = (PWND_GETTEXT)lpWindowAPI[WND_GETTEXT];

	int  iRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETTEXT, &pGetText, &cs ) )
	{
		lpstr = MapProcessPtr( lpstr, (LPPROCESS)cs.lpvData );

		iRetv = pGetText( hWnd, lpstr, nMax );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef int ( WINAPI * PWND_GETTEXTLENGTH )( HWND );
int WINAPI Wnd_GetTextLength( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETTEXTLENGTH, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_GETTEXTLENGTH pGetTextLength;// = (PWND_GETTEXTLENGTH)lpWindowAPI[WND_GETTEXTLENGTH];

	int  iRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETTEXTLENGTH, &pGetTextLength, &cs ) )
	{
		iRetv = pGetTextLength( hWnd );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_GETCLIENTRECT )( HWND, LPRECT );
BOOL WINAPI Wnd_GetClientRect( HWND hWnd, LPRECT lprc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETCLIENTRECT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lprc );
#else

	PWND_GETCLIENTRECT pGetClientRect;// = (PWND_GETCLIENTRECT)lpWindowAPI[WND_GETCLIENTRECT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETCLIENTRECT, &pGetClientRect, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pGetClientRect( hWnd, lprc );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_GETWINDOWRECT )( HWND, LPRECT );
BOOL WINAPI Wnd_GetWindowRect( HWND hWnd, LPRECT lprc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETWINDOWRECT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lprc );
#else

	PWND_GETWINDOWRECT pGetWindowRect;// = (PWND_GETWINDOWRECT)lpWindowAPI[WND_GETWINDOWRECT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETWINDOWRECT, &pGetWindowRect, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pGetWindowRect( hWnd, lprc );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_SETPOS )( HWND, HWND, int, int, int, int, UINT );
BOOL WINAPI Wnd_SetPos( HWND hWnd, 
					    HWND hwndAfter, 
						int x, 
						int y, 
						int width, 
						int height, 
						UINT uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETPOS, 7 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hwndAfter, x, y, width, height, uFlags );
#else

	PWND_SETPOS pSetPos;// = (PWND_SETPOS)lpWindowAPI[WND_SETPOS];
	
	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETPOS, &pSetPos, &cs ) )
	{
		bRetv = pSetPos( hWnd, hwndAfter, x, y, width, height, uFlags );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_MOVE )( HWND, int, int, int, int, BOOL );
BOOL WINAPI Wnd_Move( HWND hWnd, int x, int y, int width, int height, BOOL bRepaint )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_MOVE, 6 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, x, y, width, height, bRepaint );
#else

	PWND_MOVE pMove;// = (PWND_MOVE)lpWindowAPI[WND_MOVE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_MOVE, &pMove, &cs ) )
	{
		bRetv = pMove( hWnd, x, y, width, height, bRepaint );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_BRINGTOTOP )( HWND );
BOOL WINAPI Wnd_BringToTop( HWND hWnd)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_BRINGTOTOP, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_BRINGTOTOP pBringToTop;// = (PWND_BRINGTOTOP)lpWindowAPI[WND_BRINGTOTOP];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_BRINGTOTOP, &pBringToTop, &cs ) )
	{
		bRetv = pBringToTop( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}


typedef BOOL ( WINAPI * PWND_ADJUSTRECTEX )( LPRECT, DWORD, BOOL, DWORD ); 
BOOL WINAPI Wnd_AdjustRectEx( LPRECT lprc, DWORD dwMainStyle, BOOL fMenu, DWORD dwExStyle )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ADJUSTRECTEX, 4 );
    cs.arg0 = (DWORD)lprc;
    return (DWORD)CALL_SERVER( &cs, dwMainStyle, fMenu, dwExStyle );
#else

	PWND_ADJUSTRECTEX pAdjustRectEx;// = (PWND_ADJUSTRECTEX)lpWindowAPI[WND_ADJUSTRECTEX];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ADJUSTRECTEX, &pAdjustRectEx, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pAdjustRectEx( lprc, dwMainStyle, fMenu, dwExStyle );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_FLASH )( HWND, BOOL );
BOOL WINAPI Wnd_Flash(HWND hWnd, BOOL  bInvert )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_FLASH, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, bInvert );
#else

	PWND_FLASH pFlash;// = (PWND_FLASH)lpWindowAPI[WND_FLASH];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_FLASH, &pFlash, &cs ) )
	{
		bRetv = pFlash( hWnd, bInvert );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_CLOSE )( HWND );
BOOL WINAPI Wnd_Close(HWND hWnd)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_CLOSE, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_CLOSE pClose;// = (PWND_CLOSE)lpWindowAPI[WND_CLOSE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_CLOSE, &pClose, &cs ) )
	{
		bRetv = pClose( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}
///////////////////////////////////////////////////////////////////////////
typedef BOOL ( WINAPI * PWND_ISICONIC )( HWND );
BOOL WINAPI Wnd_IsIconic( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ISICONIC, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_ISICONIC pIsIconic;// = (PWND_ISICONIC)lpWindowAPI[WND_ISICONIC];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ISICONIC, &pIsIconic, &cs ) )
	{
		bRetv = pIsIconic( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_ISZOOMED )( HWND );
BOOL WINAPI Wnd_IsZoomed( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ISZOOMED, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_ISZOOMED pIsZoomed;// = (PWND_ISZOOMED)lpWindowAPI[WND_ISZOOMED];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ISZOOMED, &pIsZoomed, &cs ) )
	{
		bRetv = pIsZoomed( hWnd );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef LONG ( WINAPI * PWND_GETLONG )( HWND, int );
LONG WINAPI Wnd_GetLong( HWND hWnd, int nIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETLONG, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, nIndex );
#else

	PWND_GETLONG pGetLong;// = (PWND_GETLONG)lpWindowAPI[WND_GETLONG];

	LONG  lRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETLONG, &pGetLong, &cs ) )
	{
		lRetv = pGetLong( hWnd, nIndex );
		API_Leave(  );
	}
	return lRetv;
#endif
}

typedef LONG ( WINAPI * PWND_SETLONG )( HWND, int, LONG );
LONG WINAPI Wnd_SetLong( HWND hWnd, int nIndex, LONG dwNewLong )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETLONG, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, nIndex, dwNewLong );
#else

	PWND_SETLONG pSetLong;// = (PWND_SETLONG)lpWindowAPI[WND_SETLONG];

	LONG  lRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETLONG, &pSetLong, &cs ) )
	{
		lRetv = pSetLong( hWnd, nIndex, dwNewLong );
		API_Leave(  );
	}
	return lRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETDESKTOP )( void );
HWND WINAPI Wnd_GetDesktop( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETDESKTOP, 0 );
    cs.arg0 = 0;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETDESKTOP pGetDesktop;// = (PWND_GETDESKTOP)lpWindowAPI[WND_GETDESKTOP];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETDESKTOP, &pGetDesktop, &cs ) )
	{
		hWndRetv =  pGetDesktop();
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_FIND )( LPCSTR, LPCSTR );
HWND WINAPI Wnd_Find( LPCSTR lpcClassName, LPCSTR lpcWindowName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_FIND, 2 );
    cs.arg0 = (DWORD)lpcClassName;
    return (HWND)CALL_SERVER( &cs, lpcWindowName );
#else

	PWND_FIND pFind;// = (PWND_FIND)lpWindowAPI[WND_FIND];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_FIND, &pFind, &cs ) )
	{
		lpcClassName = MapProcessPtr( lpcClassName, (LPPROCESS)cs.lpvData );
		lpcWindowName = MapProcessPtr( lpcWindowName, (LPPROCESS)cs.lpvData );

		hWndRetv = pFind( lpcClassName, lpcWindowName );
		API_Leave(  );
	}
	return hWndRetv;

#endif
}

typedef BOOL ( WINAPI * PWND_UPDATE )( HWND );
BOOL WINAPI Wnd_Update( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_UPDATE, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_UPDATE pUpdate;// = (PWND_UPDATE)lpWindowAPI[WND_UPDATE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_UPDATE, &pUpdate, &cs ) )
	{
		bRetv = pUpdate( hWnd );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PWND_GETUPDATERECT )( HWND, LPRECT, BOOL );
BOOL WINAPI Wnd_GetUpdateRect( HWND hWnd, LPRECT lprc, BOOL bErase )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETUPDATERECT, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lprc, bErase );
#else

	PWND_GETUPDATERECT pUpdateRect;// = (PWND_GETUPDATERECT)lpWindowAPI[WND_GETUPDATERECT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETUPDATERECT, &pUpdateRect, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pUpdateRect( hWnd, lprc, bErase );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PWND_GETUPDATERGN )( HWND, HRGN, BOOL );
int WINAPI Wnd_GetUpdateRgn( HWND hWnd, HRGN hrgn, BOOL bErase )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETUPDATERGN, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hrgn, bErase );
#else

    PWND_GETUPDATERGN pGetUpdateRgn;// = (PWND_GETUPDATERGN)lpWindowAPI[WND_GETUPDATERGN];

	int iRetv = ERROR;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETUPDATERGN, &pGetUpdateRgn, &cs ) )
	{
		iRetv = pGetUpdateRgn( hWnd, hrgn, bErase );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_ENUM )( WNDENUMPROC, LPARAM );
BOOL WINAPI Wnd_Enum( WNDENUMPROC lpProc, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ENUM, 2 );
    cs.arg0 = (DWORD)lpProc;
    return (DWORD)CALL_SERVER( &cs, lParam );
#else

	PWND_ENUM pEnum;// = (PWND_ENUM)lpWindowAPI[WND_ENUM];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ENUM, &pEnum, &cs ) )
	{
		bRetv = pEnum( lpProc, lParam );
		API_Leave(  );
	}
	return bRetv;

#endif
}

typedef BOOL ( WINAPI * PWND_ENUMCHILD )( HWND, WNDENUMPROC, LPARAM );
BOOL WINAPI Wnd_EnumChild( HWND hwndParent, WNDENUMPROC lpProc, LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ENUMCHILD, 3 );
    cs.arg0 = (DWORD)hwndParent;
    return (DWORD)CALL_SERVER( &cs, lpProc, lParam );
#else

	PWND_ENUMCHILD pEnumChild;// = (PWND_ENUMCHILD)lpWindowAPI[WND_ENUMCHILD];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ENUMCHILD, &pEnumChild, &cs ) )
	{
		bRetv = pEnumChild( hwndParent, lpProc, lParam );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETTOP )( HWND );
HWND WINAPI Wnd_GetTop( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETTOP, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETTOP pGetTop;// = (PWND_GETTOP)lpWindowAPI[WND_GETTOP];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETTOP, &pGetTop, &cs ) )
	{
		hWndRetv = pGetTop( hWnd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GET )( HWND, UINT );
HWND WINAPI Wnd_Get( HWND hWnd, UINT uCmd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GET, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (HWND)CALL_SERVER( &cs, uCmd );
#else

	PWND_GET pGet;// = (PWND_GET)lpWindowAPI[WND_GET];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GET, &pGet, &cs ) )
	{
		hWndRetv =  pGet( hWnd, uCmd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETNEXT )( HWND, UINT );
HWND WINAPI Wnd_GetNext(HWND hWnd, UINT uCmd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETNEXT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (HWND)CALL_SERVER( &cs, uCmd );
#else

	PWND_GETNEXT pGetNext;// = (PWND_GETNEXT)lpWindowAPI[WND_GETNEXT];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETNEXT, &pGetNext, &cs ) )
	{
		hWndRetv = pGetNext( hWnd, uCmd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_CLIENTTOSCREEN )( HWND, LPPOINT );
BOOL WINAPI Wnd_ClientToScreen( HWND hWnd, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_CLIENTTOSCREEN, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lppt );
#else

	PWND_CLIENTTOSCREEN pClientToScreen;// = (PWND_CLIENTTOSCREEN)lpWindowAPI[WND_CLIENTTOSCREEN];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_CLIENTTOSCREEN, &pClientToScreen, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		bRetv = pClientToScreen( hWnd, lppt );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_SCREENTOCLIENT )( HWND, LPPOINT );
BOOL WINAPI Wnd_ScreenToClient( HWND hWnd, LPPOINT lppt )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SCREENTOCLIENT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lppt );
#else

	PWND_SCREENTOCLIENT pScreenToClient;// = (PWND_SCREENTOCLIENT)lpWindowAPI[WND_SCREENTOCLIENT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SCREENTOCLIENT, &pScreenToClient, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		bRetv = pScreenToClient( hWnd, lppt );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef DWORD ( WINAPI * PWND_MAPPOINTS )( HWND, HWND, LPPOINT, UINT );
DWORD WINAPI Wnd_MapPoints(HWND hwndFrom, HWND hwndTo, LPPOINT lppt, UINT cpt)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_MAPPOINTS, 4 );
    cs.arg0 = (DWORD)hwndFrom;
    return (DWORD)CALL_SERVER( &cs, hwndTo, lppt, cpt );
#else

	PWND_MAPPOINTS pMapPoints;// = (PWND_MAPPOINTS)lpWindowAPI[WND_MAPPOINTS];

	DWORD  dwRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_MAPPOINTS, &pMapPoints, &cs ) )
	{
		lppt = MapProcessPtr( lppt, (LPPROCESS)cs.lpvData );

		dwRetv = pMapPoints( hwndFrom, hwndTo, lppt, cpt );
		API_Leave(  );
	}
	return dwRetv;
#endif
}

typedef HWND ( WINAPI * PWND_FROMPOINT )( POINT pt );
HWND WINAPI Wnd_FromPoint(POINT pt)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_FROMPOINT, 2 );
    cs.arg0 = (DWORD)pt.x;
    return (HWND)CALL_SERVER( &cs, pt.y );
#else

	PWND_FROMPOINT pFromPoint;// = (PWND_FROMPOINT)lpWindowAPI[WND_FROMPOINT];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_FROMPOINT, &pFromPoint, &cs ) )
	{
		hWndRetv = pFromPoint( pt );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_CHILDFROMPOINT )( HWND, POINT, UINT );
HWND WINAPI Wnd_ChildFromPoint(HWND hWnd, POINT pt, UINT uFlags)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_CHILDFROMPOINT, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (HWND)CALL_SERVER( &cs, pt.x, pt.y, uFlags );
#else

	PWND_CHILDFROMPOINT pChildFromPoint;// = (PWND_CHILDFROMPOINT)lpWindowAPI[WND_CHILDFROMPOINT];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_CHILDFROMPOINT, &pChildFromPoint, &cs ) )
	{
		hWndRetv = pChildFromPoint( hWnd, pt, uFlags );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETFOREGROUND )( void );
HWND WINAPI Wnd_GetForeground(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETFOREGROUND, 0 );
    cs.arg0 = 0;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETFOREGROUND pGetForeground;// = (PWND_GETFOREGROUND)lpWindowAPI[WND_GETFOREGROUND];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETFOREGROUND, &pGetForeground, &cs ) )
	{
		hWndRetv = pGetForeground();
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_SETFOREGROUND )( HWND );
BOOL WINAPI Wnd_SetForeground( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETFOREGROUND, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_SETFOREGROUND pSetForeground;// = (PWND_SETFOREGROUND)lpWindowAPI[WND_SETFOREGROUND];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETFOREGROUND, &pSetForeground, &cs ) )
	{
		bRetv = pSetForeground( hWnd );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HWND ( WINAPI * PWND_SETACTIVE )( HWND );
HWND WINAPI Wnd_SetActive(HWND hWnd)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETACTIVE, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_SETACTIVE pSetActive;// = (PWND_SETACTIVE)lpWindowAPI[WND_SETACTIVE];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETACTIVE, &pSetActive, &cs ) )
	{
		hWndRetv = pSetActive( hWnd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef UINT ( WINAPI * PWND_SETTIMER )( HWND, UINT, UINT, TIMERPROC );
UINT WINAPI Wnd_SetTimer(HWND hWnd, UINT uID, UINT uElapse, TIMERPROC lpTimerProc)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETTIMER, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, uID, uElapse, lpTimerProc );
#else

	PWND_SETTIMER pSetTimer;// = (PWND_SETTIMER)lpWindowAPI[WND_SETTIMER];

	UINT  uiRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETTIMER, &pSetTimer, &cs ) )
	{
		uiRetv = pSetTimer( hWnd, uID, uElapse, lpTimerProc );
		API_Leave(  );
	}
	return uiRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_KILLTIMER )( HWND, UINT );
BOOL WINAPI Wnd_KillTimer( HWND hWnd, UINT uID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_KILLTIMER, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, uID );
#else

	PWND_KILLTIMER pKillTimer;// = (PWND_KILLTIMER)lpWindowAPI[WND_KILLTIMER];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_KILLTIMER, &pKillTimer, &cs ) )
	{
		bRetv = pKillTimer( hWnd, uID );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef UINT ( WINAPI * PWND_SETSYSTIMER )( HWND, UINT, UINT, TIMERPROC );
UINT WINAPI Wnd_SetSysTimer( HWND hWnd, UINT uID, UINT uElapse, TIMERPROC lpTimerProc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETSYSTIMER, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, uID, uElapse, lpTimerProc );
#else

	PWND_SETSYSTIMER pSetSysTimer;// = (PWND_SETSYSTIMER)lpWindowAPI[WND_SETSYSTIMER];

	UINT  uiRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETSYSTIMER, &pSetSysTimer, &cs ) )
	{
		uiRetv = pSetSysTimer( hWnd, uID, uElapse, lpTimerProc );
		API_Leave(  );
	}
	return uiRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_KILLSYSTIMER )( HWND, UINT );
BOOL WINAPI Wnd_KillSysTimer( HWND hWnd, UINT uID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_KILLSYSTIMER, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, uID );
#else

	PWND_KILLSYSTIMER pKillSysTimer;// = (PWND_KILLSYSTIMER)lpWindowAPI[WND_KILLSYSTIMER];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_KILLSYSTIMER, &pKillSysTimer, &cs ) )
	{
		bRetv = pKillSysTimer( hWnd, uID );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETACTIVE )( void );
HWND WINAPI Wnd_GetActive( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETACTIVE, 0 );
    cs.arg0 = 0;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETACTIVE pGetActive;// = (PWND_GETACTIVE)lpWindowAPI[WND_GETACTIVE];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETACTIVE, &pGetActive, &cs ) )
	{
		hWndRetv = pGetActive();
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETTOPLEVELPARENT )( HWND );
HWND WINAPI Wnd_GetTopLevelParent( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETTOPLEVELPARENT, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETTOPLEVELPARENT pGetTopLevelParent;// = (PWND_GETTOPLEVELPARENT)lpWindowAPI[WND_GETTOPLEVELPARENT];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETTOPLEVELPARENT, &pGetTopLevelParent, &cs ) )
	{
		hWndRetv = pGetTopLevelParent( hWnd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_SETFOCUS )( HWND );
HWND WINAPI Wnd_SetFocus( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETFOCUS, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_SETFOCUS pSetFocus;// = (PWND_SETFOCUS)lpWindowAPI[WND_SETFOCUS];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETFOCUS, &pSetFocus, &cs ) )
	{
		hWndRetv = pSetFocus( hWnd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETFOCUS )( void );
HWND WINAPI Wnd_GetFocus(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETFOCUS, 0 );
    cs.arg0 = 0;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETFOCUS pGetFocus;// = (PWND_GETFOCUS)lpWindowAPI[WND_GETFOCUS];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETFOCUS, &pGetFocus, &cs ) )
	{
		hWndRetv = pGetFocus();
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HWND ( WINAPI * PWND_SETCAPTURE )( HWND );
HWND WINAPI Wnd_SetCapture(HWND hWnd)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETCAPTURE, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_SETCAPTURE pSetCapture;// = (PWND_SETCAPTURE)lpWindowAPI[WND_SETCAPTURE];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETCAPTURE, &pSetCapture, &cs ) )
	{
		hWndRetv = pSetCapture( hWnd );
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_RELEASECAPTURE )( void );
BOOL WINAPI Wnd_ReleaseCapture(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_RELEASECAPTURE, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_RELEASECAPTURE pReleaseCapture;// = (PWND_RELEASECAPTURE)lpWindowAPI[WND_RELEASECAPTURE];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_RELEASECAPTURE, &pReleaseCapture, &cs ) )
	{
		bRetv = pReleaseCapture();
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HWND ( WINAPI * PWND_GETCAPTURE )( void );
HWND WINAPI Wnd_GetCapture(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETCAPTURE, 0 );
    cs.arg0 = 0;
    return (HWND)CALL_SERVER( &cs );
#else

	PWND_GETCAPTURE pGetCapture;// = (PWND_GETCAPTURE)lpWindowAPI[WND_GETCAPTURE];

	HWND  hWndRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETCAPTURE, &pGetCapture, &cs ) )
	{
		hWndRetv = pGetCapture();
		API_Leave(  );
	}
	return hWndRetv;
#endif
}

typedef HDC ( WINAPI * PWND_BEGINPAINT )( HWND, LPPAINTSTRUCT );
HDC WINAPI Wnd_BeginPaint( HWND hWnd, LPPAINTSTRUCT lpps )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_BEGINPAINT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (HDC)CALL_SERVER( &cs, lpps );
#else

	PWND_BEGINPAINT pBeginPaint;// = (PWND_BEGINPAINT)lpWindowAPI[WND_BEGINPAINT];

	HDC  hdcRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_BEGINPAINT, &pBeginPaint, &cs ) )
	{
		lpps = MapProcessPtr( lpps, (LPPROCESS)cs.lpvData );

		hdcRetv = pBeginPaint( hWnd, lpps );
		API_Leave(  );
	}
	return hdcRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_ENDPAINT )( HWND, const PAINTSTRUCT * );
BOOL WINAPI Wnd_EndPaint( HWND hWnd, const PAINTSTRUCT * lpps )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ENDPAINT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpps );
#else

	PWND_ENDPAINT pEndPaint;// = (PWND_ENDPAINT)lpWindowAPI[WND_ENDPAINT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ENDPAINT, &pEndPaint, &cs ) )
	{
		lpps = MapProcessPtr( lpps, (LPPROCESS)cs.lpvData );

		bRetv = pEndPaint( hWnd, lpps );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef HDC ( WINAPI * PWND_GETCLIENTDC )( HWND );
HDC WINAPI Wnd_GetClientDC( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETCLIENTDC, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HDC)CALL_SERVER( &cs );
#else

	PWND_GETCLIENTDC pGetClientDC;// = (PWND_GETCLIENTDC)lpWindowAPI[WND_GETCLIENTDC];

	HDC  hdcRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETCLIENTDC, &pGetClientDC, &cs ) )
	{
		hdcRetv = pGetClientDC( hWnd );
		API_Leave(  );
	}
	return hdcRetv;
#endif
}

typedef int ( WINAPI * PWND_RELEASEDC )( HWND, HDC );
int WINAPI Wnd_ReleaseDC( HWND hWnd, HDC hdc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_RELEASEDC, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hdc );
#else

	PWND_RELEASEDC pReleaseDC;// = (PWND_RELEASEDC)lpWindowAPI[WND_RELEASEDC];

	int  iRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_RELEASEDC, &pReleaseDC, &cs ) )
	{
		iRetv = pReleaseDC( hWnd, hdc );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef HDC ( WINAPI * PWND_GETWINDOWDC )( HWND );
HDC WINAPI Wnd_GetWindowDC( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETWINDOWDC, 1 );
    cs.arg0 = (DWORD)hWnd ;
    return (HDC)CALL_SERVER( &cs );
#else

	PWND_GETWINDOWDC pGetWindowDC;// = (PWND_GETWINDOWDC)lpWindowAPI[WND_GETWINDOWDC];

	HDC  hdcRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETWINDOWDC, &pGetWindowDC, &cs ) )
	{
		hdcRetv = pGetWindowDC( hWnd );
		API_Leave(  );
	}
	return hdcRetv;
#endif
}

/*typedef BOOL ( WINAPI * PWND_DRAWCAPTION )( HWND, HDC, LPCRECT, UINT );
BOOL WINAPI Wnd_DrawCaption( HWND hWnd, HDC hdc, LPCRECT lprc, UINT uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_DRAWCAPTION, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hdc, lprc, uFlags );
#else

	PWND_DRAWCAPTION pDrawCaption;// = (PWND_DRAWCAPTION)lpWindowAPI[WND_DRAWCAPTION];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_DRAWCAPTION, &pDrawCaption, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pDrawCaption( hWnd, hdc, lprc, uFlags );
		API_Leave(  );
	}
	return bRetv;

#endif
}
*/

typedef int ( WINAPI * PWND_SCROLL )( HWND, int, int, LPCRECT, LPCRECT, HRGN, LPRECT, UINT );
int WINAPI Wnd_Scroll(   
					HWND hWnd,
                    int dx, 
					int dy,
                    LPCRECT lpcScrollRect, 
					LPCRECT lpcClipRect,
                    HRGN hrgnUpdate,
                    LPRECT lpUpdateRect,
                    UINT uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SCROLL, 8 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, dx, dy, lpcScrollRect, lpcClipRect, hrgnUpdate, lpUpdateRect, uFlags );
#else

	PWND_SCROLL pScroll;// = (PWND_SCROLL)lpWindowAPI[WND_SCROLL];

	int  iRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SCROLL, &pScroll, &cs ) )
	{
		lpcScrollRect = MapProcessPtr( lpcScrollRect, (LPPROCESS)cs.lpvData );
		lpcClipRect = MapProcessPtr( lpcClipRect, (LPPROCESS)cs.lpvData );
		lpUpdateRect = MapProcessPtr( lpUpdateRect, (LPPROCESS)cs.lpvData );

		iRetv = pScroll( hWnd, dx, dy, lpcScrollRect, lpcClipRect, hrgnUpdate, lpUpdateRect, uFlags );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef HDC ( WINAPI * PWND_GETDCEX )( HWND, HRGN hrgnClip, DWORD );
HDC WINAPI Wnd_GetDCEx( HWND hWnd, HRGN hrgnClip, DWORD dwFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETDCEX, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (HDC)CALL_SERVER( &cs, hrgnClip, dwFlags );
#else

	PWND_GETDCEX pGetDCEx;// = (PWND_GETDCEX)lpWindowAPI[WND_GETDCEX];

	HDC  hdcRetv = NULL;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETDCEX, &pGetDCEx, &cs ) )
	{
		hdcRetv = pGetDCEx( hWnd, hrgnClip, dwFlags );
		API_Leave(  );
	}
	return hdcRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_INVALIDATERECT )( HWND, LPCRECT, BOOL );
BOOL WINAPI Wnd_InvalidateRect( HWND hWnd, LPCRECT lprc, BOOL bErase )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_INVALIDATERECT, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lprc, bErase );
#else

	PWND_INVALIDATERECT pInvalidateRect;// = (PWND_INVALIDATERECT)lpWindowAPI[WND_INVALIDATERECT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_INVALIDATERECT, &pInvalidateRect, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		bRetv = pInvalidateRect( hWnd, lprc, bErase );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_INVALIDATERGN )( HWND, HRGN, BOOL );
BOOL WINAPI Wnd_InvalidateRgn(HWND hWnd, HRGN hrgn, BOOL bErase)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_INVALIDATERGN, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hrgn, bErase );
#else

	PWND_INVALIDATERGN pInvalidateRgn;// = (PWND_INVALIDATERGN)lpWindowAPI[WND_INVALIDATERGN];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_INVALIDATERGN, &pInvalidateRgn, &cs ) )
	{
		bRetv = pInvalidateRgn( hWnd, hrgn, bErase );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_VALIDATERECT )( HWND, LPCRECT );
BOOL WINAPI Wnd_ValidateRect( HWND hWnd, LPCRECT lpcrc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_VALIDATERECT, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpcrc );
#else

	PWND_VALIDATERECT pValidateRect;// = (PWND_VALIDATERECT)lpWindowAPI[WND_VALIDATERECT];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_VALIDATERECT, &pValidateRect, &cs ) )
	{
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		bRetv = pValidateRect( hWnd, lpcrc );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef BOOL ( WINAPI * PWND_VALIDATERGN )( HWND, HRGN );
BOOL WINAPI Wnd_ValidateRgn( HWND hWnd, HRGN hrgn )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_VALIDATERGN, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hrgn );
#else

	PWND_VALIDATERGN pValidateRgn;// = (PWND_VALIDATERGN)lpWindowAPI[WND_VALIDATERGN];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_VALIDATERGN, &pValidateRgn, &cs ) )
	{
		bRetv = pValidateRgn( hWnd, hrgn );
		API_Leave(  );
	}
	return bRetv;
#endif
}

typedef DWORD ( WINAPI * PWND_GETTHREADPROCESSID )( HWND, DWORD * );
DWORD WINAPI Wnd_GetThreadProcessId( HWND hWnd, DWORD * lpProcessID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETTHREADPROCESSID, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpProcessID );
#else

	PWND_GETTHREADPROCESSID pGetThreadProcessId;// = (PWND_GETTHREADPROCESSID)lpWindowAPI[WND_GETTHREADPROCESSID];

	DWORD dwRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETTHREADPROCESSID, &pGetThreadProcessId, &cs ) )
	{
		lpProcessID = MapProcessPtr( lpProcessID, (LPPROCESS)cs.lpvData );

		dwRetv = pGetThreadProcessId( hWnd, lpProcessID );
		API_Leave(  );
	}
	return dwRetv;
#endif
}

/*
typedef BOOL ( WINAPI * PWND_ENABLESCROLLBAR )( HWND, UINT, UINT );
BOOL WINAPI Wnd_EnableScrollBar( HWND hWnd, UINT uFlags, UINT uArrows )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_ENABLESCROLLBAR, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, uFlags, uArrows );
#else

	PWND_ENABLESCROLLBAR pEnableScrollBar;// = (PWND_ENABLESCROLLBAR)lpWindowAPI[WND_ENABLESCROLLBAR];

	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_ENABLESCROLLBAR, &pEnableScrollBar, &cs ) )
	{
		bRetv = pEnableScrollBar( hWnd, uFlags, uArrows );
		API_Leave(  );
	}
	return bRetv;
#endif
}
*/

/*

typedef int ( WINAPI * PWND_GETSCROLLINFO )( HWND, int, LPSCROLLINFO );
int WINAPI Wnd_GetScrollInfo( HWND hWnd, int fnBar, LPSCROLLINFO lpsi )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETSCROLLINFO, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, fnBar, lpsi );
#else

	PWND_GETSCROLLINFO pGetScrollInfo;// = (PWND_GETSCROLLINFO)lpWindowAPI[WND_GETSCROLLINFO];

	int  iRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETSCROLLINFO, &pGetScrollInfo, &cs ) )
	{
		lpsi = MapProcessPtr( lpsi, (LPPROCESS)cs.lpvData );

		iRetv = pGetScrollInfo( hWnd, fnBar, lpsi );
		API_Leave(  );
	}
	return iRetv;
#endif
}

typedef int ( WINAPI * PWND_SETSCROLLINFO )( HWND, int, LPCSCROLLINFO, BOOL );
int WINAPI Wnd_SetScrollInfo( HWND hWnd, int fnBar, LPCSCROLLINFO lpcsi, BOOL fRedraw )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETSCROLLINFO, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, fnBar, lpcsi, fRedraw );
#else

	PWND_SETSCROLLINFO pSetScrollInfo;// = (PWND_SETSCROLLINFO)lpWindowAPI[WND_SETSCROLLINFO];

	int  iRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETSCROLLINFO, &pSetScrollInfo, &cs ) )
	{
		lpcsi = MapProcessPtr( lpcsi, (LPPROCESS)cs.lpvData );

		iRetv = pSetScrollInfo( hWnd, fnBar, lpcsi, fRedraw );
		API_Leave(  );
	}
	return iRetv;
#endif
}
*/

////////////////////////////////////
typedef BOOL ( WINAPI * PWND_CLEAR )( DWORD );
BOOL WINAPI Wnd_Clear( DWORD dwThreadID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_CLEAR, 1 );
    cs.arg0 = (DWORD)dwThreadID ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_CLEAR pClear;
	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_CLEAR, &pClear, &cs ) )
	{
		bRetv = pClear( dwThreadID );
		API_Leave(  );
	}
	return bRetv;
#endif
}
/////////////////////////////


typedef BOOL ( WINAPI * PWND_SHOWDESKTOP )( UINT uiFlag );
BOOL WINAPI Wnd_ShowDesktop( UINT uiFlag )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SHOWDESKTOP, 1 );
    cs.arg0 = (DWORD)uiFlag ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PWND_SHOWDESKTOP pfn;
	BOOL  bRetv = FALSE;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SHOWDESKTOP, &pfn, &cs ) )
	{
		bRetv = pfn( uiFlag );
		API_Leave();
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PWND_SETRGN )( HWND hWnd, HRGN hrgn, BOOL bRedraw );
int WINAPI Wnd_SetRgn( HWND hWnd, HRGN hrgn, BOOL bRedraw )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_SETRGN, 3 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hrgn, bRedraw );
#else

	PWND_SETRGN pfn;
	int  bRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_SETRGN, &pfn, &cs ) )
	{
		bRetv = pfn( hWnd, hrgn, bRedraw );
		API_Leave();
	}
	return bRetv;
#endif
}

typedef int ( WINAPI * PWND_GETRGN )( HWND hWnd, HRGN hrgn );
int WINAPI Wnd_GetRgn( HWND hWnd, HRGN hrgn )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, WND_GETRGN, 2 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, hrgn );
#else

	PWND_GETRGN pfn;
	int  bRetv = 0;
	CALLSTACK cs;

	if( API_Enter( API_WND, WND_GETRGN, &pfn, &cs ) )
	{
		bRetv = pfn( hWnd, hrgn );
		API_Leave();
	}
	return bRetv;
#endif
}

