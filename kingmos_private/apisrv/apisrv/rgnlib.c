/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>



typedef HRGN ( WINAPI * PRGN_CREATERECT )( int l, int t, int r, int b );
HRGN WINAPI Rgn_CreateRect( int l, int t, int r, int b )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_CREATERECT, 4 );
    cs.arg0 = (DWORD)l;
    return (HRGN)CALL_SERVER( &cs, t, r, b );
#else

	PRGN_CREATERECT pCreateRect;// = (PRGN_CREATERECT)lpRgnAPI[RGN_CREATERECT];

	CALLSTACK cs;
	HRGN   retv = NULL;

	if( API_Enter( API_GDI, RGN_CREATERECT, &pCreateRect, &cs ) )
	{
		retv = pCreateRect( l, t, r, b );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef HRGN ( WINAPI * PRGN_CREATERECTINDIRECT )( LPCRECT );
HRGN WINAPI Rgn_CreateRectIndirect( LPCRECT lpcrc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_CREATERECTINDIRECT, 1 );
    cs.arg0 = (DWORD)lpcrc ;
    return (HRGN)CALL_SERVER( &cs );
#else

	PRGN_CREATERECTINDIRECT pCreateRectIndirect;// = (PRGN_CREATERECTINDIRECT)lpRgnAPI[RGN_CREATERECTINDIRECT];

	CALLSTACK cs;
	HRGN   retv = NULL;

	if( API_Enter( API_GDI, RGN_CREATERECTINDIRECT, &pCreateRectIndirect, &cs ) )
	{
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		retv = pCreateRectIndirect( lpcrc );
		API_Leave(  );
	}
	return retv;
#endif	
}

typedef int ( WINAPI * PRGN_COMBINE )(HRGN, HRGN, HRGN, int);
int WINAPI Rgn_Combine( HRGN hrgnDest, HRGN hrgn0, HRGN hrgn1, int iMode )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_COMBINE, 4 );
    cs.arg0 = (DWORD)hrgnDest;
    return (DWORD)CALL_SERVER( &cs, hrgn0, hrgn1, iMode );
#else

	PRGN_COMBINE pCombine;// = (PRGN_COMBINE)lpRgnAPI[RGN_COMBINE];

	CALLSTACK cs;
	int   retv = ERROR;

	if( API_Enter( API_GDI, RGN_COMBINE, &pCombine, &cs ) )
	{
		retv = pCombine( hrgnDest, hrgn0, hrgn1, iMode );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PRGN_EQUAL )(HRGN, HRGN);
BOOL WINAPI Rgn_Equal( HRGN hrgn0, HRGN hrgn1 )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_EQUAL, 2 );
    cs.arg0 = (DWORD)hrgn0;
    return (DWORD)CALL_SERVER( &cs, hrgn1 );
#else

	PRGN_EQUAL pEqual;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GDI, RGN_EQUAL, &pEqual, &cs ) )
	{
		retv = pEqual( hrgn0, hrgn1 );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PRGN_OFFSET )(HRGN hrgn, int xOffset, int yOffset );
int WINAPI Rgn_Offset(HRGN hrgn, int xOffset, int yOffset )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_OFFSET, 3 );
    cs.arg0 = (DWORD)hrgn;
    return (DWORD)CALL_SERVER( &cs, xOffset, yOffset );
#else

	PRGN_OFFSET pOffset;// = (PRGN_OFFSET)lpRgnAPI[RGN_OFFSET];

	CALLSTACK cs;
	int   retv = ERROR;

	if( API_Enter( API_GDI, RGN_OFFSET, &pOffset, &cs ) )
	{
		retv = pOffset( hrgn, xOffset, yOffset );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PRGN_GETBOX )( HRGN hrgn, RECT FAR* lprc );
int WINAPI Rgn_GetBox( HRGN hrgn , RECT FAR* lprc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_GETBOX, 2 );
    cs.arg0 = (DWORD)hrgn;
    return (DWORD)CALL_SERVER( &cs, lprc );
#else

	PRGN_GETBOX pGetBox;// = (PRGN_GETBOX)lpRgnAPI[RGN_GETBOX];

	CALLSTACK cs;
	int   retv = 0;

	if( API_Enter( API_GDI, RGN_GETBOX, &pGetBox, &cs ) )
	{
		lprc = MapProcessPtr( lprc, (LPPROCESS)cs.lpvData );

		retv = pGetBox( hrgn, lprc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PRGN_RECTINREGION )(HRGN, const RECT * );
BOOL WINAPI Rgn_RectInRegion( HRGN hrgn, const RECT * lpcrc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_RECTINREGION, 2 );
    cs.arg0 = (DWORD)hrgn;
    return (DWORD)CALL_SERVER( &cs, lpcrc );
#else

	PRGN_RECTINREGION pRectInRegion;// = (PRGN_RECTINREGION)lpRgnAPI[RGN_RECTINREGION];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GDI, RGN_RECTINREGION, &pRectInRegion, &cs ) )
	{
		lpcrc = MapProcessPtr( lpcrc, (LPPROCESS)cs.lpvData );

		retv = pRectInRegion( hrgn, lpcrc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PRGN_PTINREGION )( HRGN hrgn, int x, int y );
BOOL WINAPI Rgn_PtInRegion( HRGN hrgn, int x, int y )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_PTINREGION, 3 );
    cs.arg0 = (DWORD)hrgn;
    return (DWORD)CALL_SERVER( &cs, x, y );
#else

	PRGN_PTINREGION pPtInRegion;// = (PRGN_PTINREGION)lpRgnAPI[RGN_PTINREGION];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GDI, RGN_PTINREGION, &pPtInRegion, &cs ) )
	{
		retv = pPtInRegion( hrgn, x, y );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PRGN_SETRECT )(HRGN, int, int, int, int );
BOOL WINAPI Rgn_SetRect( HRGN hrgn, int l, int t, int r, int b )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GDI, RGN_SETRECT, 5 );
    cs.arg0 = (DWORD)hrgn;
    return (DWORD)CALL_SERVER( &cs, l, t, r, b );
#else

	PRGN_SETRECT pSetRect;// = (PRGN_SETRECT)lpRgnAPI[RGN_SETRECT];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_GDI, RGN_SETRECT, &pSetRect, &cs ) )
	{
		retv = pSetRect( hrgn, l, t, r, b );
		API_Leave(  );
	}
	return retv;
#endif
}
