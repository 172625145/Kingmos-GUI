#include <eframe.h>
#include <eComCtrl.h>

//BOOL InitCommonControlsEx( LPINITCOMMONCONTROLSEX lpInitCtrls );
extern ATOM RegisterComboListBoxClass( HINSTANCE );
//extern WORD RegisterDeskTopClass( HINSTANCE );
extern ATOM RegisterStartBarClass( HINSTANCE );
extern ATOM RegisterPDAListCtrlClass(HINSTANCE);
extern ATOM RegisterComboBoxClass(HINSTANCE hInstance);
extern ATOM RegisterUpDownClass(HINSTANCE hInst);
//extern ATOM RegisterSheetClass(HINSTANCE hInst);
//extern ATOM RegisterSetDateClass( HINSTANCE hInst );
extern ATOM RegisterDateSelClass( HINSTANCE hInst );
extern ATOM RegisterDateShowClass( HINSTANCE hInst );
extern ATOM RegisterEditTimeClass(HINSTANCE hInstance);
extern ATOM RegisterCanvasClass( HINSTANCE hInst );
extern ATOM RegisterHtmlViewClass( HINSTANCE hInst );
extern ATOM RegisterSMIE_BrowersClass( HINSTANCE hInst );
extern ATOM RegisterFileBrowserClass(HINSTANCE hInstance);
extern ATOM RegisterTabCtrlClass(HINSTANCE hInstance);
extern ATOM RegisterSliderCtrlClass(HINSTANCE hInstance);
extern ATOM RegisterTrackbarClass(HINSTANCE hInstance);
extern ATOM RegisterProgressClass(HINSTANCE hInstance);
extern ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
extern ATOM RegisterMMSBrowserClass( HINSTANCE hInstance );
extern ATOM RegisterIPAddressClass( HINSTANCE hInstance );


#define USE_INTERNET_CLASSES

BOOL WINAPI InitCommonControlsEx( LPINITCOMMONCONTROLSEX lpInitCtrls )
{
	HINSTANCE hInstance;
	
	hInstance = (HINSTANCE)GetModuleHandle(NULL);

	if (lpInitCtrls->dwICC & ICC_LISTVIEW_CLASSES)
	{
		if( RegisterPDAListCtrlClass( hInstance ) == FALSE )
			return FALSE;
	}
	if (lpInitCtrls->dwICC & (ICC_UPDOWN_CLASS | ICC_DATE_CLASSES))
	{
	    if (RegisterUpDownClass( hInstance ) ==FALSE)
		    return FALSE;
	}
/*	
	if (lpInitCtrls->dwICC & ICC_SHEET_CLASS)
	{
		if (RegisterSheetClass( hInstance ) == FALSE)
		   return FALSE;
	}
*/
	/*
	if (lpInitCtrls->dwICC & ICC_DATE_CLASSES)
	{
//	    if( RegisterSetDateClass( hInstance ) == FALSE )
//		    return FALSE;
		if (RegisterDateSelClass( hInstance ) == FALSE) 
			return FALSE;
		if (RegisterDateShowClass( hInstance ) == FALSE) 
			return FALSE;
		if (RegisterEditTimeClass( hInstance ) == FALSE)
			return FALSE;
	}
	*/

	if (lpInitCtrls->dwICC & ICC_CANVAS_CLASS)
	{
		if( RegisterCanvasClass( hInstance ) == FALSE )
		   return FALSE;
	}

	/*	
#ifdef USE_INTERNET_CLASSES
	if (lpInitCtrls->dwICC & ICC_INTERNET_CLASSES)
	{
		if (RegisterHtmlViewClass( hInstance ) ==FALSE)
			return FALSE;
		if (RegisterSMIE_BrowersClass( hInstance ) ==FALSE)
			return FALSE;
	}
#endif
*/
	if (lpInitCtrls->dwICC & ICC_TAB_CLASSES)
	{
		if( RegisterTabCtrlClass( hInstance ) == FALSE )
		   return FALSE;
	}
	if (lpInitCtrls->dwICC & ICC_BAR_CLASSES)
	{
		if(RegisterSliderCtrlClass(hInstance) == FALSE)
			return FALSE;
		if (RegisterTrackbarClass(hInstance) == FALSE)
			return FALSE;
		if (RegisterProgressClass(hInstance) == FALSE)
			return FALSE;
	}
//	if (RegisterFileBrowserClass(hInstance) == FALSE)
//		return FALSE;

	if (RegisterIMGBrowserClass(hInstance) == FALSE)
		return FALSE;

	if (RegisterIPAddressClass(hInstance) == FALSE)
		return FALSE;
//	if (RegisterMMSBrowserClass(hInstance) == FALSE)
//		return FALSE;

    return TRUE;
}

BOOL WINAPI InitCommonControls( void )
{
	INITCOMMONCONTROLSEX InitCtrls;

		InitCtrls.dwSize = sizeof (INITCOMMONCONTROLSEX);
		InitCtrls.dwICC = 0xffffffff; // Initial all control
		return InitCommonControlsEx(&InitCtrls);
}
