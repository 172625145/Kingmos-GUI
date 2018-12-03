/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <emenu.h>
//#include <equeue.h>
#include <eugwme.h>
#include <eapisrv.h>
#include <epcore.h>
/*
typedef HMENU ( WINAPI * PMENU_CREATE )( void );
HMENU WINAPI Menu_Create(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_CREATE, 0 );
    cs.arg0 = 0;
    return (HMENU)CALL_SERVER( &cs );
#else

	PMENU_CREATE pfn;

	CALLSTACK cs;
	HMENU   retv = NULL;

	if( API_Enter( API_GWE, MENU_CREATE, &pfn, &cs ) )
	{
		retv = pfn( );
		API_Leave();
	}
	return retv;
#endif
}

typedef HMENU ( WINAPI * PMENU_CREATEPOPUP )(void);
HMENU WINAPI Menu_CreatePopup(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_CREATEPOPUP, 0 );
    cs.arg0 = 0;
    return (HMENU)CALL_SERVER( &cs );
#else

	PMENU_CREATEPOPUP pfn;

	CALLSTACK cs;
	HMENU   retv = NULL;

	if( API_Enter( API_GWE, MENU_CREATEPOPUP, &pfn, &cs ) )
	{
		retv = pfn( );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_DESTROY )( HMENU hMenu );
BOOL WINAPI Menu_Destroy( HMENU hMenu )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_DESTROY, 1 );
    cs.arg0 = (DWORD)hMenu ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMENU_DESTROY pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_DESTROY, &pfn, &cs ) )
	{
		retv = pfn( hMenu );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_REMOVE )( HMENU hMenu, DWORD uPosition, DWORD uFlags );
BOOL WINAPI Menu_Remove( HMENU hMenu, DWORD uPosition, DWORD uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_REMOVE, 3 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uPosition, uFlags );
#else

	PMENU_REMOVE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_REMOVE, &pfn, &cs ) )
	{
		retv = pfn( hMenu, uPosition, uFlags );
		API_Leave();
	}
	return retv;
#endif
}
///////////////////////
typedef BOOL ( WINAPI * PMENU_DELETE )( HMENU hMenu, DWORD uPosition, DWORD uFlags );
BOOL WINAPI Menu_Delete( HMENU hMenu, DWORD uPosition, DWORD uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_DELETE, 3 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uPosition, uFlags );
#else

	PMENU_DELETE pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_DELETE, &pfn, &cs ) )
	{
		retv = pfn( hMenu, uPosition, uFlags );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_INSERT )( HMENU hMenu,  DWORD uPosition,  DWORD uFlags,  DWORD uIDNewItem,  LPCTSTR lpNewItem );
BOOL WINAPI Menu_Insert(  HMENU hMenu,  
						  DWORD uPosition,
						  DWORD uFlags,
						  DWORD uIDNewItem,
						  LPCTSTR lpNewItem )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_INSERT, 5 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uPosition, uFlags, uIDNewItem, lpNewItem );
#else

	PMENU_INSERT pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_INSERT, &pfn, &cs ) )
	{
		lpNewItem = MapProcessPtr( lpNewItem, (LPPROCESS)cs.lpvData );
		retv = pfn( hMenu, uPosition, uFlags, uIDNewItem, lpNewItem );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_INSERTITEM )( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );
BOOL WINAPI Menu_InsertItem( HMENU hMenu, 
							 DWORD uItem, 
							 BOOL fByPosition,
							 LPMENUITEMINFO lpmii )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_INSERTITEM, 4 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uItem, fByPosition, lpmii );
#else

	PMENU_INSERTITEM pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_INSERTITEM, &pfn, &cs ) )
	{
		lpmii = MapProcessPtr( lpmii, (LPPROCESS)cs.lpvData );
		retv = pfn( hMenu, uItem, fByPosition, lpmii );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_ENABLEITEM )( HMENU hMenu, DWORD uPosition, DWORD uEnable );
BOOL WINAPI Menu_EnableItem( HMENU hMenu, DWORD uPosition, DWORD uEnable )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_ENABLEITEM, 3 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uPosition, uEnable );
#else

	PMENU_ENABLEITEM pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_ENABLEITEM, &pfn, &cs ) )
	{
		retv = pfn( hMenu, uPosition, uEnable );
		API_Leave();
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PMENU_GETDEFAULTITEM )( HMENU, DWORD fByPos, DWORD uFlags );
DWORD WINAPI Menu_GetDefaultItem( HMENU hMenu, DWORD fByPos, DWORD uFlags )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_GETDEFAULTITEM, 3 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, fByPos, uFlags );
#else

	PMENU_GETDEFAULTITEM pfn;

	CALLSTACK cs;
	DWORD   retv = -1;

	if( API_Enter( API_GWE, MENU_GETDEFAULTITEM, &pfn, &cs ) )
	{
		retv = pfn( hMenu, fByPos, uFlags );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_SETDEFAULTITEM )( HMENU, DWORD uItem, DWORD fByPos );
BOOL WINAPI Menu_SetDefaultItem( HMENU hMenu, DWORD uItem, DWORD fByPos )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_SETDEFAULTITEM, 3 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uItem, fByPos );
#else

	PMENU_SETDEFAULTITEM pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_SETDEFAULTITEM, &pfn, &cs ) )
	{
		retv = pfn( hMenu, uItem, fByPos );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_SETITEMINFO )( HMENU, DWORD uItem, BOOL fByPos, LPMENUITEMINFO lpmii );
BOOL WINAPI Menu_SetItemInfo( HMENU hMenu, 
							  DWORD uItem, 
							  BOOL fByPos, 
							  LPMENUITEMINFO lpmii )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_SETITEMINFO, 4 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uItem, fByPos, lpmii );
#else

	PMENU_SETITEMINFO pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_SETITEMINFO, &pfn, &cs ) )
	{
		lpmii = MapProcessPtr( lpmii, (LPPROCESS)cs.lpvData );
		retv = pfn( hMenu, uItem, fByPos, lpmii );
		API_Leave();
	}
	return retv;
#endif
}

typedef int ( WINAPI * PMENU_GETITEMCOUNT )( HMENU hMenu );
int WINAPI Menu_GetItemCount( HMENU hMenu )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_GETITEMCOUNT, 1 );
    cs.arg0 = (DWORD)hMenu ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PMENU_GETITEMCOUNT pfn;

	CALLSTACK cs;
	int   retv = -1;

	if( API_Enter( API_GWE, MENU_GETITEMCOUNT, &pfn, &cs ) )
	{
		retv = pfn( hMenu );
		API_Leave();
	}
	return retv;
#endif
}

typedef int ( WINAPI * PMENU_GETITEMID )( HMENU hMenu, int nPos );
int WINAPI Menu_GetItemID( HMENU hMenu, int nPos )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_GETITEMID, 2 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, nPos );
#else

	PMENU_GETITEMID pfn;

	CALLSTACK cs;
	int   retv = -1;

	if( API_Enter( API_GWE, MENU_GETITEMID, &pfn, &cs ) )
	{
		retv = pfn( hMenu, nPos );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_GETITEMINFO )( HMENU hMenu, DWORD uItem, BOOL fByPosition, LPMENUITEMINFO lpmii );
BOOL WINAPI Menu_GetItemInfo( HMENU hMenu, 
							  DWORD uItem,
							  BOOL fByPosition,
							  LPMENUITEMINFO lpmii )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_GETITEMINFO, 4 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uItem, fByPosition, lpmii );
#else

	PMENU_GETITEMINFO pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_GETITEMINFO, &pfn, &cs ) )
	{
		lpmii = MapProcessPtr( lpmii, (LPPROCESS)cs.lpvData );
		retv = pfn( hMenu, uItem, fByPosition, lpmii );
		API_Leave();
	}
	return retv;
#endif
}

typedef HMENU ( WINAPI * PMENU_GETSUB )( HMENU hMenu, int nPos );
HMENU WINAPI Menu_GetSub( HMENU hMenu, int nPos )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_GETSUB, 2 );
    cs.arg0 = (DWORD)hMenu;
    return (HMENU)CALL_SERVER( &cs, nPos );
#else

	PMENU_GETSUB pfn;

	CALLSTACK cs;
	HMENU   retv = NULL;

	if( API_Enter( API_GWE, MENU_GETSUB, &pfn, &cs ) )
	{
		retv = pfn( hMenu, nPos );
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PMENU_TRACKPOPUP )( HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hwndOwner, LPCRECT lpcRect );
BOOL WINAPI Menu_TrackPopup( HMENU hMenu,
							 UINT uFlags,
							 int x,
							 int y,
							 int nReserved,
							 HWND hwndOwner,
							 LPCRECT lpcRect )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, MENU_TRACKPOPUP, 7 );
    cs.arg0 = (DWORD)hMenu;
    return (DWORD)CALL_SERVER( &cs, uFlags, x, y, nReserved, hwndOwner, lpcRect );
#else

	PMENU_TRACKPOPUP pfn;

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GWE, MENU_TRACKPOPUP, &pfn, &cs ) )
	{
		lpcRect = MapProcessPtr( lpcRect, (LPPROCESS)cs.lpvData );
		retv = pfn( hMenu, uFlags, x, y, nReserved, hwndOwner, lpcRect );
		API_Leave();
	}
	return retv;
#endif
}
*/