/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>
/*

typedef HWND ( WINAPI * PDLG_CREATEINDIRECT )( HINSTANCE hInst, LPCDLG_TEMPLATE lpTemplate, HWND hParent, DLGPROC lpDialogFunc );
HWND WINAPI Dlg_CreateIndirect( HINSTANCE hInst, 
							    LPCDLG_TEMPLATE lpTemplate, 
								HWND hParent, 
								DLGPROC lpDialogFunc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_CREATEINDIRECT, 4 );
    cs.arg0 = (DWORD)hInst;
    return (HWND)CALL_SERVER( &cs, lpTemplate, hParent, lpDialogFunc );
#else

	PDLG_CREATEINDIRECT pfn;

	CALLSTACK cs;
	HWND   retv = NULL;

	if( API_Enter( API_WND, DLG_CREATEINDIRECT, &pfn, &cs ) )
	{
		lpTemplate = MapProcessPtr( lpTemplate, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpTemplate, hParent, lpDialogFunc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HWND ( WINAPI * PDLG_CREATEINDIRECTPARAM )( HINSTANCE hInst, LPCDLG_TEMPLATE lpName, HWND hParent, DLGPROC lpDialogFunc, LPARAM lParamInit );
HWND WINAPI Dlg_CreateIndirectParam( HINSTANCE hInst, 
									 LPCDLG_TEMPLATE lpName, 
									 HWND hParent, 
									 DLGPROC lpDialogFunc, 
									 LPARAM lParamInit )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_CREATEINDIRECTPARAM, 5 );
    cs.arg0 = (DWORD)hInst;
    return (HWND)CALL_SERVER( &cs, lpName, hParent, lpDialogFunc, lParamInit );
#else

	PDLG_CREATEINDIRECTPARAM pfn;

	CALLSTACK cs;
	HWND   retv = NULL;

	if( API_Enter( API_WND, DLG_CREATEINDIRECTPARAM, &pfn, &cs ) )
	{
		lpName = MapProcessPtr( lpName, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpName, hParent, lpDialogFunc, lParamInit );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_BOXINDIRECT )( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc );
int WINAPI Dlg_BoxIndirect( HANDLE hInst, 
						    LPDLG_TEMPLATE lpTemplate, 
							HWND hWndParent, 
							DLGPROC lpDialogFunc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_BOXINDIRECT, 4 );
    cs.arg0 = (DWORD)hInst;
    return (DWORD)CALL_SERVER( &cs, lpTemplate, hWndParent, lpDialogFunc );
#else

	PDLG_BOXINDIRECT pfn;

	CALLSTACK cs;
	int   retv = -1;

	if( API_Enter( API_WND, DLG_BOXINDIRECT, &pfn, &cs ) )
	{
		lpTemplate = MapProcessPtr( lpTemplate, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpTemplate, hWndParent, lpDialogFunc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_BOXINDIRECTPARAM )( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit );
int WINAPI Dlg_BoxIndirectParam( HANDLE hInst, 
								 LPDLG_TEMPLATE lpTemplate, 
								 HWND hWndParent, 
								 DLGPROC lpDialogFunc, 
								 LPARAM lParamInit )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_BOXINDIRECTPARAM, 5 );
    cs.arg0 = (DWORD)hInst;
    return (DWORD)CALL_SERVER( &cs, lpTemplate, hWndParent, lpDialogFunc, lParamInit );
#else

	PDLG_BOXINDIRECTPARAM pfn;

	CALLSTACK cs;
	int   retv = -1;

	if( API_Enter( API_WND, DLG_BOXINDIRECTPARAM, &pfn, &cs ) )
	{
		lpTemplate = MapProcessPtr( lpTemplate, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit );
		API_Leave(  );
	}
	return retv;
#endif
}

// ex
typedef HWND ( WINAPI * PDLG_CREATEINDIRECT_EX )( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpTemplate, HWND hParent, DLGPROC lpDialogFunc );
HWND WINAPI Dlg_CreateIndirectEx( HINSTANCE hInst, 
								  LPCDLG_TEMPLATE_EX lpTemplate, 
								  HWND hParent, 
								  DLGPROC lpDialogFunc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_CREATEINDIRECT_EX, 4 );
    cs.arg0 = (DWORD)hInst;
    return (HWND)CALL_SERVER( &cs, lpTemplate, hParent, lpDialogFunc );
#else

	PDLG_CREATEINDIRECT_EX pfn;

	CALLSTACK cs;
	HWND   retv = NULL;

	if( API_Enter( API_WND, DLG_CREATEINDIRECT_EX, &pfn, &cs ) )
	{
		lpTemplate = MapProcessPtr( lpTemplate, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpTemplate, hParent, lpDialogFunc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HWND ( WINAPI * PDLG_CREATEINDIRECTPARAM_EX )( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpName, HWND hParent, DLGPROC lpDialogFunc, LPARAM lParamInit );
HWND WINAPI Dlg_CreateIndirectParamEx( HINSTANCE hInst, 
									   LPCDLG_TEMPLATE_EX lpName, 
									   HWND hParent, 
									   DLGPROC lpDialogFunc, 
									   LPARAM lParamInit )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_CREATEINDIRECTPARAM_EX, 5 );
    cs.arg0 = (DWORD)hInst;
    return (HWND)CALL_SERVER( &cs, lpName, hParent, lpDialogFunc, lParamInit );
#else

	PDLG_CREATEINDIRECTPARAM_EX pfn;

	CALLSTACK cs;
	HWND   retv = NULL;

	if( API_Enter( API_WND, DLG_CREATEINDIRECTPARAM_EX, &pfn, &cs ) )
	{
		lpName = MapProcessPtr( lpName, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpName, hParent, lpDialogFunc, lParamInit );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_BOXINDIRECT_EX )( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc );
int WINAPI Dlg_BoxIndirectEx( HANDLE hInst, 
							  LPDLG_TEMPLATE_EX lpTemplate, 
							  HWND hWndParent, 
							  DLGPROC lpDialogFunc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_BOXINDIRECT_EX, 4 );
    cs.arg0 = (DWORD)hInst;
    return (DWORD)CALL_SERVER( &cs, lpTemplate, hWndParent, lpDialogFunc );
#else

	PDLG_BOXINDIRECT_EX pfn;

	CALLSTACK cs;
	int   retv = -1;

	if( API_Enter( API_WND, DLG_BOXINDIRECT_EX, &pfn, &cs ) )
	{
		lpTemplate = MapProcessPtr( lpTemplate, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpTemplate, hWndParent, lpDialogFunc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_BOXINDIRECTPARAM_EX )( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit );
int WINAPI Dlg_BoxIndirectParamEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_BOXINDIRECTPARAM_EX, 5 );
    cs.arg0 = (DWORD)hInst;
    return (DWORD)CALL_SERVER( &cs, lpTemplate, hWndParent, lpDialogFunc, lParamInit );
#else

	PDLG_BOXINDIRECTPARAM_EX pfn;

	CALLSTACK cs;
	int   retv = -1;

	if( API_Enter( API_WND, DLG_BOXINDIRECTPARAM_EX, &pfn, &cs ) )
	{
		lpTemplate = MapProcessPtr( lpTemplate, (LPPROCESS)cs.lpvData );

		retv = pfn( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit );
		API_Leave(  );
	}
	return retv;
#endif
}
// ex-end

typedef BOOL ( WINAPI * PDLG_END )( HWND hDlg, int nResult );
BOOL WINAPI Dlg_End( HWND hDlg, int nResult )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_END, 2 );
    cs.arg0 = (DWORD)hDlg;
    return (DWORD)CALL_SERVER( &cs, nResult );
#else

	PDLG_END pfn;

	CALLSTACK cs;
	int   retv = FALSE;

	if( API_Enter( API_WND, DLG_END, &pfn, &cs ) )
	{
		retv = pfn( hDlg, nResult );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_GETCTRLID )( HWND hwndCtl );
int WINAPI Dlg_GetCtrlID( HWND hwndCtl )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_GETCTRLID, 1 );
    cs.arg0 = (DWORD)hwndCtl ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDLG_GETCTRLID pfn;

	CALLSTACK cs;
	int  retv = 0;

	if( API_Enter( API_WND, DLG_GETCTRLID, &pfn, &cs ) )
	{
		retv = pfn( hwndCtl );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HWND ( WINAPI * PDLG_GETITEM )( HWND hDlg, int nID );
HWND WINAPI Dlg_GetItem( HWND hDlg, int nID )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_GETITEM, 2 );
    cs.arg0 = (DWORD)hDlg;
    return (HWND)CALL_SERVER( &cs, nID );
#else

	PDLG_GETITEM pfn;

	CALLSTACK cs;
	HWND  retv = NULL;

	if( API_Enter( API_WND, DLG_GETITEM, &pfn, &cs ) )
	{
		retv = pfn( hDlg, nID );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef UINT ( WINAPI * PDLG_GETITEMTEXT )( HWND hDlg, int nID, LPSTR lpstr, int nMaxCount );
UINT WINAPI Dlg_GetItemText( HWND hDlg, int nID, LPTSTR lpstr, int nMaxCount )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_GETITEMTEXT, 4 );
    cs.arg0 = (DWORD)hDlg;
    return (DWORD)CALL_SERVER( &cs, nID, lpstr, nMaxCount );
#else

	PDLG_GETITEMTEXT pfn;

	CALLSTACK cs;
	UINT  retv = 0;

	if( API_Enter( API_WND, DLG_GETITEMTEXT, &pfn, &cs ) )
	{
		lpstr = MapProcessPtr( lpstr, (LPPROCESS)cs.lpvData );

		retv = pfn( hDlg, nID, lpstr, nMaxCount );
		API_Leave(  );
	}
	return retv;
#endif
}

//BOOL Dlg_SetItemText( HWND hDlg, int nIDDlgItem, LPCTSTR lpString );
typedef BOOL ( WINAPI * PDLG_SETITEMTEXT )( HWND hDlg, int nID, LPCTSTR lpString );
BOOL WINAPI Dlg_SetItemText( HWND hDlg, int nID, LPCTSTR lpString )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_SETITEMTEXT, 3 );
    cs.arg0 = (DWORD)hDlg;
    return (DWORD)CALL_SERVER( &cs, nID, lpString );
#else

	PDLG_SETITEMTEXT pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_WND, DLG_SETITEMTEXT, &pfn, &cs ) )
	{
		lpString = MapProcessPtr( lpString, (LPPROCESS)cs.lpvData );

		retv = pfn( hDlg, nID, lpString );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef LONG ( WINAPI * PDLG_SENDITEMMESSAGE )( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam );
LONG WINAPI Dlg_SendItemMessage( HWND hDlg, 
								 int nID, 
								 UINT msg, 
								 WPARAM wParam, 
								 LPARAM lParam )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_SENDITEMMESSAGE, 5 );
    cs.arg0 = (DWORD)hDlg;
    return (DWORD)CALL_SERVER( &cs, nID, msg, wParam, lParam );
#else

	PDLG_SENDITEMMESSAGE pfn;

	CALLSTACK cs;
	LONG  retv = 0;

	if( API_Enter( API_WND, DLG_SENDITEMMESSAGE, &pfn, &cs ) )
	{
		retv = pfn( hDlg, nID, msg, wParam, lParam );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_MESSAGEBOX )( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType );
int WINAPI Dlg_MessageBox( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_MESSAGEBOX, 4 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs, lpText, lpCaption, uType );
#else

	PDLG_MESSAGEBOX pfn;

	CALLSTACK cs;
	int  retv = 0;

	if( API_Enter( API_WND, DLG_MESSAGEBOX, &pfn, &cs ) )
	{
		lpText = MapProcessPtr( lpText, (LPPROCESS)cs.lpvData );
		lpCaption = MapProcessPtr( lpCaption, (LPPROCESS)cs.lpvData );

		retv = pfn( hWnd, lpText, lpCaption, uType );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_SETITEMINT )( HWND hDlg, int nID, UINT uValue, BOOL bSigned );
BOOL WINAPI Dlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_SETITEMINT, 4 );
    cs.arg0 = (DWORD)hDlg;
    return (DWORD)CALL_SERVER( &cs, nID, uValue, bSigned );
#else

	PDLG_SETITEMINT pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_WND, DLG_SETITEMINT, &pfn, &cs ) )
	{
		retv = pfn( hDlg, nID, uValue, bSigned );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PDLG_GETITEMINT )( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned );
UINT WINAPI Dlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_WND, DLG_GETITEMINT, 4 );
    cs.arg0 = (DWORD)hDlg;
    return (DWORD)CALL_SERVER( &cs, nID, lpTranslated, bSigned );
#else

	PDLG_GETITEMINT pfn;

	CALLSTACK cs;
	UINT  retv = 0;

	if( API_Enter( API_WND, DLG_GETITEMINT, &pfn, &cs ) )
	{
		retv = pfn( hDlg, nID, lpTranslated, bSigned );
		API_Leave(  );
	}
	return retv;
#endif
}
*/