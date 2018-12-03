//#include <edef.h>
#include <eframe.h>
//#include <gwmesrv.h>

BOOL _InitialSystemWindowClass( HINSTANCE );
extern ATOM RegisterButtonClass( HINSTANCE );
extern ATOM RegisterDialogClass( HINSTANCE );
extern ATOM RegisterMenuClass( HINSTANCE );
extern ATOM RegisterStaticClass( HINSTANCE );
extern ATOM RegisterScrollBarClass( HINSTANCE );
extern ATOM RegisterListBoxClass( HINSTANCE );
extern ATOM RegisterComboListBoxClass( HINSTANCE );
//extern WORD RegisterDeskTopClass( HINSTANCE );
//extern ATOM RegisterStartBarClass( HINSTANCE );
//extern ATOM RegisterPDAListCtrlClass(HINSTANCE);
extern ATOM RegisterPDAEditClass(HINSTANCE hInstance);
extern ATOM RegisterMultiItemEditClass(HINSTANCE hInstance);
extern ATOM RegisterComboBoxClass(HINSTANCE hInstance);
extern BOOL RegisterKeyboardClass( HINSTANCE hInst );
//extern ATOM RegisterUpDownClass(HINSTANCE hInst);
//extern ATOM RegisterSheetClass(HINSTANCE hInst);
//extern ATOM RegisterKeyboardClass( HINSTANCE hInst );
//extern ATOM RegisterSetDateClass( HINSTANCE hInst );
//extern ATOM RegisterCanvasClass( HINSTANCE hInst );
//extern ATOM RegisterHtmlViewClass( HINSTANCE hInst );
//extern ATOM RegisterSMIE_BrowersClass( HINSTANCE hInst );
extern ATOM RegisterTouchCalClass( HINSTANCE hInst );
extern BOOL RegisterKeyboardClassByStruct( WNDCLASS * lpwc, HINSTANCE );

BOOL InitialSystemWindowClass( HINSTANCE hInst )
{
//	WNDCLASS wc;

    if( RegisterScrollBarClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterListBoxClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterComboListBoxClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterStaticClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterButtonClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterDialogClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterMenuClass( hInst ) == FALSE )
        return FALSE;
//    if( RegisterPDAListCtrlClass( 0 ) == FALSE )
//        return FALSE;
    if( RegisterPDAEditClass( hInst ) == FALSE )
        return FALSE;
    if( RegisterMultiItemEditClass( hInst ) ==FALSE )
        return FALSE;
    //if( RegisterKeyboardClass( hInst ) == FALSE)
        //return FALSE;

	//if( RegisterKeyboardClassByStruct( &wc, hInst ) )
	//{
		//if( RegisterClass( &wc ) == FALSE )
			//return FALSE;
	//}

    if (RegisterComboBoxClass( hInst ) == FALSE)
        return FALSE;
	if (RegisterTouchCalClass( hInst ) == FALSE )
		return FALSE;
//    if (RegisterUpDownClass(0) ==FALSE)
//        return FALSE;
//#ifndef ARM_CPU
//    if (RegisterSheetClass(0) == FALSE)
//       return FALSE;
//#endif
//    if( RegisterSetDateClass(0) == FALSE )
//       return FALSE;
//    if( RegisterCanvasClass(0) == FALSE )
//       return FALSE;
//	if (RegisterHtmlViewClass(0) ==FALSE)
//		return FALSE;
//	if (RegisterSMIE_BrowersClass(0) ==FALSE)
//		return FALSE;

    return TRUE;
}

//BOOL _DeInitialSystemWindowClass( void )
//{
//}
