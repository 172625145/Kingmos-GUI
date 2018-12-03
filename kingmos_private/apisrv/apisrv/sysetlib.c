/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <eugwme.h>
#include <eapisrv.h>

#include <epcore.h>


typedef int ( WINAPI * PSYS_GETMETRICS )( int );
int WINAPI Sys_GetMetrics( int nIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_GETMETRICS, 1 );
    cs.arg0 = (DWORD)nIndex ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_GETMETRICS pGetMetrics;// = (PSYS_GETMETRICS)lpSysSetAPI[SYS_GETMETRICS];

	CALLSTACK cs;
	int   retv = 0;

	if( API_Enter( API_GWE, SYS_GETMETRICS, &pGetMetrics, &cs ) )
	{
		retv = pGetMetrics( nIndex );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef int ( WINAPI * PSYS_SETMETRICS )( int nIndex, int iNewValue );
int WINAPI Sys_SetMetrics( int nIndex, int iNewValue  )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_SETMETRICS, 2 );
    cs.arg0 = (DWORD)nIndex;
    return (DWORD)CALL_SERVER( &cs, iNewValue );
#else

	PSYS_SETMETRICS pfn;// = (PSYS_GETMETRICS)lpSysSetAPI[SYS_GETMETRICS];

	CALLSTACK cs;
	int   retv = 0;

	if( API_Enter( API_GWE, SYS_SETMETRICS, &pfn, &cs ) )
	{
		retv = pfn( nIndex, iNewValue );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef DWORD ( WINAPI * PSYS_GETCOLOR )( int );
DWORD WINAPI Sys_GetColor( int nIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_GETCOLOR, 1 );
    cs.arg0 = (DWORD)nIndex ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_GETCOLOR pGetColor;// = (PSYS_GETCOLOR)lpSysSetAPI[SYS_GETCOLOR];

	CALLSTACK cs;
	DWORD   retv = 0;

	if( API_Enter( API_GWE, SYS_GETCOLOR, &pGetColor, &cs ) )
	{
		retv = pGetColor( nIndex );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PSYS_SETCOLORS )( int, const int *, const COLORREF * );
BOOL WINAPI Sys_SetColors(int cElements, 
						  const int *lpaElements, 
						  const COLORREF *lpaRgbValues )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_SETCOLORS, 3 );
    cs.arg0 = (DWORD)cElements;
    return (DWORD)CALL_SERVER( &cs, lpaElements, lpaRgbValues );
#else

	PSYS_SETCOLORS pSetColors;// = (PSYS_SETCOLORS)lpSysSetAPI[SYS_SETCOLORS];

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_GWE, SYS_SETCOLORS, &pSetColors, &cs ) )
	{
		lpaElements = MapProcessPtr( lpaElements, (LPPROCESS)cs.lpvData );
		lpaRgbValues = MapProcessPtr( lpaRgbValues, (LPPROCESS)cs.lpvData );

		retv = pSetColors( cElements, lpaElements, lpaRgbValues );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HBRUSH ( WINAPI * PSYS_GETCOLORBRUSH )( int );
HBRUSH WINAPI Sys_GetColorBrush( int nIndex )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_GETCOLORBRUSH, 1 );
    cs.arg0 = (DWORD)nIndex ;
    return (HBRUSH)CALL_SERVER( &cs );
#else

	PSYS_GETCOLORBRUSH pGetColorBrush;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	HBRUSH retv = NULL;

	if( API_Enter( API_GWE, SYS_GETCOLORBRUSH, &pGetColorBrush, &cs ) )
	{
		retv = pGetColorBrush( nIndex );
		API_Leave(  );
	}
	return retv;
#endif
}
/*
typedef void ( WINAPI * PSYS_CLOSEOBJECT )( HANDLE hProcess );
void WINAPI Sys_CloseObject( HANDLE hProcess )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_CLOSEOBJECT, 1 );
    cs.arg0 = (DWORD)hProcess ;
    CALL_SERVER( &cs );
#else

	PSYS_CLOSEOBJECT pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	//RUSH retv = NULL;

	if( API_Enter( API_GWE, SYS_CLOSEOBJECT, &pfn, &cs ) )
	{
		pfn( hProcess );
		API_Leave(  );
	}
#endif
}
*/
/*
typedef BOOL ( WINAPI * PSYS_TOUCHCALIBRATE )( void );
BOOL WINAPI Sys_TouchCalibrate( void )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_TOUCHCALIBRATE, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_TOUCHCALIBRATE pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_GWE, SYS_TOUCHCALIBRATE, &pfn, &cs ) )
	{
		retv = pfn();
		API_Leave(  );
	}
	return retv;
#endif
}
*/


typedef BOOL ( WINAPI * PSYS_SETCALIBRATE_WINDOW )( HWND hWnd );
BOOL WINAPI Sys_SetCalibrateWindow( HWND hWnd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_SETCALIBRATE_WINDOW, 1 );
    cs.arg0 = (DWORD)hWnd;
    return (DWORD)CALL_SERVER( &cs );
#else

	PSYS_SETCALIBRATE_WINDOW pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_GWE, SYS_SETCALIBRATE_WINDOW, &pfn, &cs ) )
	{
		retv = pfn(hWnd);
		API_Leave();
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PSYS_SetCalibratePoints )( int cCalibrationPoints, int *pScreenXBuffer,int *pScreenYBuffer, int *pUncalXBuffer, int *pUncalYBuffer );
BOOL WINAPI Sys_SetCalibratePoints(
    int   cCalibrationPoints,     //@PARM The number of calibration points
    int   *pScreenXBuffer,        //@PARM List of screen X coords displayed
    int   *pScreenYBuffer,        //@PARM List of screen Y coords displayed
    int   *pUncalXBuffer,         //@PARM List of X coords collected
    int   *pUncalYBuffer          //@PARM List of Y coords collected
    )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, SYS_SETCALIBRATE_POINTS, 5 );
    cs.arg0 = cCalibrationPoints;
    return (DWORD)CALL_SERVER( &cs, pScreenXBuffer, pScreenYBuffer, pUncalXBuffer, pUncalYBuffer );
#else

	PSYS_SetCalibratePoints pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_GWE, SYS_SETCALIBRATE_POINTS, &pfn, &cs ) )
	{
		retv = pfn(cCalibrationPoints, pScreenXBuffer, pScreenYBuffer, pUncalXBuffer, pUncalYBuffer );
		API_Leave();
	}
	return retv;
#endif
}


typedef int ( WINAPI * PCP_MULTIBYTETOWIDECHAR )(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPWSTR lpWideCharStr, 
						int cchWideChar 
						);
int WINAPI CP_MultiByteToWideChar(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPWSTR lpWideCharStr, 
						int cchWideChar 
						)
{
	extern int WINAPI WinCP_MultiByteToWideChar(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPWSTR lpWideCharStr, 
						int cchWideChar 
						);
    return WinCP_MultiByteToWideChar(
		                uiCodePage, 
						dwFlags, 
						lpMultiByteStr, 
						cbMultiByte, 
						lpWideCharStr, 
						cchWideChar );

/*
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CP_MULTIBYTETOWIDECHAR, 6 );
    cs.arg0 = (DWORD)
								uiCodePage;
    return (DWORD)CALL_SERVER( &cs, 
								dwFlags, 
								lpMultiByteStr, 
								cbMultiByte, 
								lpWideCharStr, 
								cchWideChar );
#else

	PCP_MULTIBYTETOWIDECHAR pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_GWE, CP_MULTIBYTETOWIDECHAR, &pfn, &cs ) )
	{
		retv = pfn( uiCodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar );
 		API_Leave(  );
	}
	return retv;

#endif
*/
}

typedef int ( WINAPI * PCP_WIDECHARTOMULTIBYTE )(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						int cchWideChar, 
						LPSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL * lpUsedDefaultChar 
						);
int WINAPI CP_WideCharToMultiByte(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						int cchWideChar, 
						LPSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL * lpUsedDefaultChar 
						)
{
	extern int WINAPI WinCP_WideCharToMultiByte(
						UINT uiCodePage, 
						DWORD dwFlags, 
						LPCWSTR lpWideCharStr, 
						int cchWideChar, 
						LPSTR lpMultiByteStr, 
						int cbMultiByte, 
						LPCSTR lpDefaultChar, 
						BOOL * lpUsedDefaultChar 
						);
	return WinCP_WideCharToMultiByte(
						uiCodePage, 
						dwFlags, 
						lpWideCharStr, 
						cchWideChar, 
						lpMultiByteStr, 
						cbMultiByte, 
						lpDefaultChar, 
						lpUsedDefaultChar );

/*
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GWE, CP_WIDECHARTOMULTIBYTE, 8 );
    cs.arg0 = (DWORD)
								uiCodePage;
    return (DWORD)CALL_SERVER( &cs, 
								dwFlags, 
								lpWideCharStr,
								cchWideChar, 
								lpMultiByteStr, 
								cbMultiByte,
						        lpDefaultChar, 
						        lpUsedDefaultChar
								);
#else

	PCP_WIDECHARTOMULTIBYTE pfn;// = (PSYS_GETCOLORBRUSH)lpSysSetAPI[SYS_GETCOLORBRUSH];

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_GWE, CP_WIDECHARTOMULTIBYTE, &pfn, &cs ) )
	{
		retv = pfn(	uiCodePage, 
					dwFlags, 
					lpWideCharStr,
					cchWideChar, 
					lpMultiByteStr, 
					cbMultiByte,
				    lpDefaultChar, 
				    lpUsedDefaultChar );
 		API_Leave(  );
	}
	return retv;

#endif
*/
}
