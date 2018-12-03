/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：实现MessageBox
版本号：3.0.0
开发时期：2003-03-06
作者：李林
修改记录：
    2003-05-27: 考虑 AP_XSTART, AP_YSTART, AP_WIDTH, AP_HEIGHT
******************************************************/

#include <ewindows.h>
#include <estkstr.h>
#include <eassert.h>
//#include <eapisrv.h>

//#include <winsrv.h>
//#include <gdisrv.h>

//#define cyButtonHeight 20
//#define cxButtonWidth  40
static const char classDIALOG[] = "DIALOG";
static int Dlg_MessageBoxRect( HWND hParent,
                    LPCSTR lpText,
                    LPCSTR lpCaption,
                    UINT uType,
                    LPCRECT lpRect );
static BOOL CALLBACK MessageBoxProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static void  _MakeTextRect( LPRECT lprect, LPCTSTR lpText )
{
    HDC hdc;
	int iScreenX, iScreenY;

	hdc = CreateCompatibleDC( NULL );

	iScreenY = GetSystemMetrics( SM_CYSCREEN );//2003-05-27, DEL
	iScreenX = GetSystemMetrics( SM_CXSCREEN );//2003-05-27, DEL

	//iScreenY = AP_HEIGHT;//2003-05-27, ADD
	//iScreenX = AP_WIDTH;//2003-05-27, ADD

	lprect->left = 0;
    lprect->top = 0;
    lprect->right = iScreenX - 32;
	lprect->bottom = iScreenY - 32;

	// get text 's width and height in bottom and left of rectangle
	DrawText( hdc, lpText, -1, lprect, DT_CALCRECT );

	if( lprect->bottom > iScreenY - 64 )
		lprect->bottom = iScreenY - 64;


	DeleteDC( hdc );

}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static int _GetButtonNum( DWORD uType )
{
    switch( uType & 0xf )
    {
        case MB_OK:
            return 1;
        case MB_OKCANCEL:
        case MB_YESNO:
//        case MB_RETRYCANCEL:
            return 2;
//        case MB_ABORTRETRYIGNORE:
        case MB_YESNOCANCEL:
            return 3;
        default:
            _ASSERT( 0 );
            return 0;
    }
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int WINAPI Dlg_MessageBox( HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
{
	if( lpText == NULL )
		lpText = "";
	if( lpCaption == NULL )
		lpCaption = str_error;
	
	return Dlg_MessageBoxRect( hParent, lpText, lpCaption, uType, NULL );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

typedef struct
{
    DLG_TEMPLATE_EX dlgt;
    DLG_ITEMTEMPLATE_EX dlgi[4];
}MSGBOXCTL;

static int Dlg_MessageBoxRect( HWND hParent,
					   LPCSTR lpText,
					   LPCSTR lpCaption,
					   UINT uType,
					   LPCRECT lpTextRect )
{
    MSGBOXCTL msgctl;
    short xoff, yoff;
    RECT rect, rClient, rcText;
	HINSTANCE hInst;
	int nButton;
	int cyButtonHeight = GetSystemMetrics( SM_CYSYSFONT ) + 6;
	int cxButtonWidth = GetSystemMetrics( SM_CXSYSFONT ) * 6;
//	int iScreenX, iScreenY;


    memset( &msgctl, 0, sizeof(MSGBOXCTL) );
	if( lpTextRect )
		rcText = *lpTextRect;
	else
		_MakeTextRect( &rcText, lpText );

	rect = rcText;

    nButton = 0;
	switch( uType & 0x0f )
    {
        case MB_OK:
			nButton = 1;
			break;
        case MB_OKCANCEL:
			nButton = 2;
            break;
        case MB_YESNOCANCEL:
			nButton = 3;
            break;
        case MB_YESNO:
			nButton = 2;
            break;
		default:
			uType = MB_OK;
			nButton = 1;
			break;
    }

	//rect.right += GetSystemMetrics( SM_CXDLGFRAME ) * 2 + cxButtonWidth + 4 * 4;
	if( rect.right < nButton * cxButtonWidth + (nButton- 1) * 4 )
		rect.right = nButton * cxButtonWidth + (nButton- 1) * 4;

	rect.right += GetSystemMetrics( SM_CXDLGFRAME ) * 2 + 4 * 2;
	rect.bottom += GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYDLGFRAME ) * 2 + cyButtonHeight + 4 * 4;
	//rect.bottom += GetSystemMetrics( SM_CYCAPTION ) + cyButtonHeight * nButton + (nButton - 1 + 2) * 4;

	//LN, 2003-05-27-DEL-begin
	// center the dialog
	//iScreenY = GetSystemMetrics( SM_CYSCREEN );
	//iScreenX = GetSystemMetrics( SM_CXSCREEN );
	//rect.left = (iScreenX - rect.right) / 2;
	//rect.top = (iScreenY - rect.bottom) / 2;
	//rect.right += rect.left;
	//rect.bottom += rect.top;
	//LN, 2003-05-27-DEL-end
		
	//msgctl.dlgt.dwStyle = WS_POPUP | WS_VISIBLE | WS_DLGFRAME | WS_CAPTION;//LN, 2003-05-27-DEL
	msgctl.dlgt.dwStyle = WS_POPUP | WS_VISIBLE | WS_DLGFRAME | WS_CAPTION | DS_CENTER;//LN, 2003-05-27-ADD
	if( uType & MB_TOPMOST )
	{
		msgctl.dlgt.dwExStyle = WS_EX_TOPMOST;
	}

//    msgctl.dwExtendedStyle = 0;
    msgctl.dlgt.cdit = _GetButtonNum( uType ) + 1;
    msgctl.dlgt.x = (short)rect.left;
    msgctl.dlgt.y = (short)rect.top;
    msgctl.dlgt.cx = (short)(rect.right - rect.left);
    msgctl.dlgt.cy = (short)(rect.bottom - rect.top);
    msgctl.dlgt.lpcClassName = (LPCSTR)classDIALOG;
    msgctl.dlgt.lpcWindowName = lpCaption;
    
	// setting child window, button
    rClient = rect;
    xoff = GetSystemMetrics( SM_CXDLGFRAME );
    xoff = -(xoff + xoff);
    yoff = GetSystemMetrics( SM_CYDLGFRAME );
    yoff = -(yoff + yoff);
    InflateRect( &rClient, xoff, yoff );

    rClient.top += GetSystemMetrics( SM_CYCAPTION );
    OffsetRect( &rClient, -rClient.left, -rClient.top );
    InflateRect( &rClient, -4, -4 );
	//rClient.left = rClient.right - cxButtonWidth;
	rClient.top = rClient.bottom - cyButtonHeight;
	rClient.left += ( (rClient.right - rClient.left) - nButton * cxButtonWidth - (nButton- 1) * 4 ) / 2;

//    OffsetRect( &rClient, -rClient.left, -rClient.top );

    msgctl.dlgi[0].dwStyle = WS_CHILD | WS_VISIBLE;// | WS_BORDER;
//    msgctl.dlgi[0].dwExtendedStyle = 0;
    msgctl.dlgi[0].x = (short)(rcText.left + 4);
    msgctl.dlgi[0].y = (short)(rcText.top + 4);
    msgctl.dlgi[0].cx = (short)rcText.right;
    msgctl.dlgi[0].cy = (short)rcText.bottom;
    msgctl.dlgi[0].id = 0;
    msgctl.dlgi[0].lpcClassName = classSTATIC;
    msgctl.dlgi[0].lpcWindowName = lpText;
    msgctl.dlgi[0].lpParam = 0;    

    switch( uType & 0x0f )
    {
        case MB_OK:
        case MB_OKCANCEL:
            // set ok button
            msgctl.dlgi[1].dwStyle = WS_CHILD | WS_VISIBLE;
//            msgctl.dlgi[1].dwExtendedStyle = 0;
            msgctl.dlgi[1].x = (short)rClient.left;
            msgctl.dlgi[1].y = (short)rClient.top;
            msgctl.dlgi[1].cx = cxButtonWidth;
            msgctl.dlgi[1].cy = cyButtonHeight;
            msgctl.dlgi[1].id = IDOK;
            msgctl.dlgi[1].lpcClassName = classBUTTON;
            msgctl.dlgi[1].lpcWindowName = str_ok;
            msgctl.dlgi[1].lpParam = 0;
            // set cancel button
            if( (uType & 0x0f) == MB_OKCANCEL )
            {
                //rClient.top += cyButtonHeight + 4;
				rClient.left += 4 + cxButtonWidth;

                msgctl.dlgi[2].dwStyle = WS_CHILD | WS_VISIBLE;
//                msgctl.dlgi[2].dwExtendedStyle = 0;
                msgctl.dlgi[2].x = (short)rClient.left;
                msgctl.dlgi[2].y = (short)rClient.top;
                msgctl.dlgi[2].cx = cxButtonWidth;
                msgctl.dlgi[2].cy = cyButtonHeight;
                msgctl.dlgi[2].id = IDCANCEL;
                msgctl.dlgi[2].lpcClassName = classBUTTON;
                msgctl.dlgi[2].lpcWindowName = str_cancel;
                msgctl.dlgi[2].lpParam = 0;
            }
            break;
        case MB_YESNOCANCEL:
        case MB_YESNO:
            // set yes button
            msgctl.dlgi[1].dwStyle = WS_CHILD | WS_VISIBLE;
//            msgctl.dlgi[1].dwExtendedStyle = 0;
            msgctl.dlgi[1].x = (short)rClient.left;
            msgctl.dlgi[1].y = (short)rClient.top;
            msgctl.dlgi[1].cx = cxButtonWidth;
            msgctl.dlgi[1].cy = cyButtonHeight;
            msgctl.dlgi[1].id = IDYES;
            msgctl.dlgi[1].lpcClassName = classBUTTON;
            msgctl.dlgi[1].lpcWindowName = str_yes;
            msgctl.dlgi[1].lpParam = 0;
            // set no button
            //rClient.top += cyButtonHeight + 4;
			rClient.left += 4 + cxButtonWidth;

            msgctl.dlgi[2].dwStyle = WS_CHILD | WS_VISIBLE;
  //          msgctl.dlgi[2].dwExtendedStyle = 0;
            msgctl.dlgi[2].x = (short)rClient.left;
            msgctl.dlgi[2].y = (short)rClient.top;
            msgctl.dlgi[2].cx = cxButtonWidth;
            msgctl.dlgi[2].cy = cyButtonHeight;
            msgctl.dlgi[2].id = IDNO;
            msgctl.dlgi[2].lpcClassName = classBUTTON;
            msgctl.dlgi[2].lpcWindowName = str_no;
            msgctl.dlgi[2].lpParam = 0;

            // set cancel button
            if( (uType & 0x0f) == MB_YESNOCANCEL )
            {
                //rClient.top += cyButtonHeight + 4;
				rClient.left += 4 + cxButtonWidth;

                msgctl.dlgi[3].dwStyle = WS_CHILD | WS_VISIBLE;
//                msgctl.dlgi[3].dwExtendedStyle = 0;
                msgctl.dlgi[3].x = (short)rClient.left;
                msgctl.dlgi[3].y = (short)rClient.top;
                msgctl.dlgi[3].cx = cxButtonWidth;
                msgctl.dlgi[3].cy = cyButtonHeight;
                msgctl.dlgi[3].id = IDCANCEL;
                msgctl.dlgi[3].lpcClassName = classBUTTON;
                msgctl.dlgi[3].lpcWindowName = str_cancel;
                msgctl.dlgi[3].lpParam = 0;
            }
            break;
    }
    // if hParent == 0?
	//if( hParent == NULL )
		//hParent = GetDesktopWindow();
	if( hParent == NULL )
		hInst = (HINSTANCE)GetModuleHandle( NULL );///GetCallerProcess();
	else
		hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);

    return Dlg_BoxIndirectParamEx( hInst, (LPDLG_TEMPLATE_EX)&msgctl, hParent, MessageBoxProc, 0 );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static BOOL CALLBACK MessageBoxProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
	{
	case WM_COMMAND:
		if( HIWORD( wParam ) == BN_CLICKED &&
			( LOWORD( wParam ) >= 1  &&  //== IDOK
			  LOWORD( wParam ) <= 9 ) )  //==IDHELP
		    Dlg_End( hWnd, LOWORD( wParam ) ); 
		return TRUE;
	}
	return FALSE;
}
