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

static HANDLE hWarnImage = NULL;
static HANDLE hInfoImage = NULL;


#define CAPTION_HEIGHT 58
#define BORDER_WIDTH 3

#define AGL_WARN  0x80000000
#define AGL_INFO  0x40000000

#define AGLS_WARNING           0x8000
#define AGLS_INFO           0x4000

#define AGL_BUTTON_ID  110

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

	iScreenY = 393;//543;//GetSystemMetrics( SM_CYSCREEN );//2003-05-27, DEL
	iScreenX = 543;//GetSystemMetrics( SM_CXSCREEN );//2003-05-27, DEL

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

//int WINAPI AGL_MessageBox( HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
int WINAPI AGL_MessageBox( HWND hParent,   //父窗口
						   LPCTSTR lpText,	//需要显示的文本
						   LPCTSTR lpCaption,		//标题文本
						   UINT uType,		// 风格
						   const char * szButtonText[],		//button 文本指针，与 cButton 配合
						   int cButton			// button数
						   )
{
	if( lpText == NULL )
		lpText = "";
	if( lpCaption == NULL )
		lpCaption = str_error;
	
	return Dlg_MessageBoxRect( hParent, lpText, lpCaption, uType, NULL, szButtonText, cButton );
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

#define MAX_BUTTONS 6
typedef struct
{
    DLG_TEMPLATE_EX dlgt;
    DLG_ITEMTEMPLATE_EX dlgi[MAX_BUTTONS];
}MSGBOXCTL;

//static int Dlg_MessageBoxRect( HWND hParent,
					   //LPCSTR lpText,
					   //LPCSTR lpCaption,
					   //UINT uType,
					   //LPCRECT lpTextRect )
static int Dlg_MessageBoxRect( HWND hParent,   //父窗口
						   LPCTSTR lpText,	//需要显示的文本
						   LPCTSTR lpCaption,		//标题文本
						   UINT uType,		// 风格
						   LPCRECT lpTextRect,  //如果有的话， 文本矩形
						   const char * szButtonText[],		//button 文本指针，与 cButton 配合
						   int cButton			// button数
						   )

{
    MSGBOXCTL msgctl;
    short xoff, yoff;
    RECT rect, rClient, rcText;
	HINSTANCE hInst;
	int nButton;
	const int cyButtonHeight = 54;//GetSystemMetrics( SM_CYSYSFONT ) + 6;
	const int cxButtonWidth = 465;//GetSystemMetrics( SM_CXSYSFONT ) * 6;
	const int cyButtonSpace = 6;//GetSystemMetrics( SM_CYSYSFONT ) + 6;
	const int cyButtonPos[] = { 198, 198 + 60, 198 + 60 * 2 };
	int nPosIndex;
//	int iScreenX, iScreenY;

	if( cButton >= MAX_BUTTONS )
		return 0;

    memset( &msgctl, 0, sizeof(MSGBOXCTL) );
	/*
	if( lpTextRect )
		rcText = *lpTextRect;
	else
		_MakeTextRect( &rcText, lpText );
		*/
	rect.left = rect.top = 0;
	rect.right = 543;
	rect.bottom = 393;

	//rect = rcText;
/*
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
*/
/*
	if( rect.right < nButton * cxButtonWidth + (nButton- 1) * 4 )
		rect.right = nButton * cxButtonWidth + (nButton- 1) * 4;

	rect.right += GetSystemMetrics( SM_CXDLGFRAME ) * 2 + 4 * 2;
	rect.bottom += GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYDLGFRAME ) * 2 + cyButtonHeight + 4 * 4;
	*/
	if( AGL_WARN & uType )
		msgctl.dlgt.dwStyle = WS_POPUP | WS_VISIBLE | DS_CENTER | AGLS_WARNING;// | WS_CAPTION;//LN, 2003-05-27-ADD
	else
		msgctl.dlgt.dwStyle = WS_POPUP | WS_VISIBLE | DS_CENTER | AGLS_INFO;// | WS_CAPTION;//LN, 2003-05-27-ADD

	if( uType & MB_TOPMOST )
	{
		msgctl.dlgt.dwExStyle = WS_EX_TOPMOST;
	}

//    msgctl.dwExtendedStyle = 0;
    msgctl.dlgt.cdit = cButton;//_GetButtonNum( uType ) + 1;
    msgctl.dlgt.x = (short)rect.left;
    msgctl.dlgt.y = (short)rect.top;
    msgctl.dlgt.cx = (short)(rect.right - rect.left);
    msgctl.dlgt.cy = (short)(rect.bottom - rect.top);
    msgctl.dlgt.lpcClassName = (LPCSTR)classDIALOG;
    msgctl.dlgt.lpcWindowName = lpCaption;

	
	rClient = rect;

	rClient.top = 247;
	rClient.left = 41;
	rClient.right = rClient.left + 465;
	rClient.bottom = rClient.top + 60;

	if( cButton == 1 )
		nPosIndex = 1;
	else if( cButton == 2 )
		nPosIndex = 1;
	else if( cButton == 3 )
		nPosIndex = 0;


	for( nButton = 0; nButton < cButton; nButton++ )
	{
		msgctl.dlgi[nButton].dwStyle = WS_CHILD | WS_VISIBLE;
		
		msgctl.dlgi[nButton].x = (short)rClient.left;
		msgctl.dlgi[nButton].y = (short)cyButtonPos[nPosIndex];
		msgctl.dlgi[nButton].cx = cxButtonWidth;
		msgctl.dlgi[nButton].cy = cyButtonHeight;
		msgctl.dlgi[nButton].id = AGL_BUTTON_ID + nButton;
		msgctl.dlgi[nButton].lpcClassName = "AGL_BUTTON";//classBUTTON;
		msgctl.dlgi[nButton].lpcWindowName = szButtonText[nButton];
		msgctl.dlgi[nButton].lpParam = 0;

		rClient.top += 60;
		nPosIndex = (nPosIndex + 1) % 3;
	}

/*    
	// setting child window, button
    rClient = rect;
    xoff = GetSystemMetrics( SM_CXDLGFRAME );
    xoff = -(xoff + xoff);
    yoff = GetSystemMetrics( SM_CYDLGFRAME );
    yoff = -(yoff + yoff);
    InflateRect( &rClient, xoff, yoff );

    rClient.top += GetSystemMetrics( SM_CYCAPTION );
    //OffsetRect( &rClient, -rClient.left, -rClient.top );
    InflateRect( &rClient, -4, -4 );

	rcText = rClient;

	//rClient.left = rClient.right - cxButtonWidth;
	rClient.top = rClient.bottom - cyButtonHeight;
	rClient.left += ( (rClient.right - rClient.left) - nButton * cxButtonWidth - (nButton- 1) * 4 ) / 2;

//    OffsetRect( &rClient, -rClient.left, -rClient.top );

    msgctl.dlgi[0].dwStyle = WS_CHILD | WS_VISIBLE;// | WS_BORDER;

	rcText.bottom = rClient.top;
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
*/
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
// 声明：static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// 参数：
//	IN hdc - 显示设备句柄
//	IN lprc - RECT 结构指针，指向需要填充的矩形
//	IN clr - RGB颜色值
// 返回值：
//	无
// 功能描述：
//	用固定颜色填充举行矩形
// 引用: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// 声明：static void DrawFrame( HWND hWnd, 
//							  HDC hdc, 
//							  DWORD dwMainStyle,
//							  DWORD dwExStyle,
//							  BOOL bEraseBk )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
//	IN dwMainStyle - 窗口风格
//	IN dwExStyle - 窗口扩展风格
//	IN bEraseBk - 是否清除背景
// 返回值：
//	无
// 功能描述：
//	画窗口边框
// 引用: 
// ************************************************



static void DrawFrame( HWND hWnd,
					   HDC hdc, 
					   DWORD dwMainStyle, 
					   DWORD dwExStyle, 
					   BOOL bEraseBk )
{
    RECT rect;
    HBRUSH hBrush;
    int s, i;
	HPEN hWarnPen = CreatePen( PS_SOLID, 3, RGB( 204, 0, 0 ) );
	HPEN hInfoPen = CreatePen( PS_SOLID, 3, RGB( 217, 217, 217 ) );
	HPEN hShadePen = CreatePen( PS_SOLID, 3, RGB( 0, 0, 0 ) );
	char szTitle[64];
	RECT rectCaption, rectClient;

    GetWindowRect( hWnd, &rect );
    OffsetRect( &rect, -rect.left, -rect.top );	

	rectCaption = rect;
	rectCaption.left += BORDER_WIDTH;
	rectCaption.top += BORDER_WIDTH;
	rectCaption.bottom = rectCaption.top + CAPTION_HEIGHT;
	rectCaption.right -= BORDER_WIDTH;

	rectClient = rect;
	rectClient.left += BORDER_WIDTH;
	rectClient.right -= BORDER_WIDTH;
	rectClient.top = rectCaption.bottom;
	rectClient.bottom -= BORDER_WIDTH;
	
	GetWindowText( hWnd, szTitle, sizeof( szTitle ) );

	SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
	if( dwMainStyle & AGLS_WARNING )
	{
		FillSolidRect( hdc, &rectCaption, RGB( 102, 0, 0 ) );
	}
	else
	{
		FillSolidRect( hdc, &rectCaption, RGB( 0, 0, 0 ) );
	}


	//static HANDLE hWarnImage = NULL;
	//static HANDLE hInfoImage = NULL;
	if( dwMainStyle & AGLS_WARNING )
	{
		if( hWarnImage == NULL )
			hWarnImage = LoadImage( NULL, MAKEINTRESOURCE( OBM_MSG_WARNING ), IMAGE_BITMAP, 0, 0, LR_SHARED );
		if( hWarnImage )
		{
			HDC hMemDC = CreateCompatibleDC( hdc );
			SelectObject( hMemDC, hWarnImage );
			TransparentBlt( hdc, 13, 12, 40, 40, hMemDC, 0, 0, 40, 40, RGB( 255, 0, 255 ) );
			DeleteObject( hMemDC );
		}
	}

	else //if( dwMainStyle & AGLS_INFO )
	{
		if( hInfoImage == NULL )
			hInfoImage = LoadImage( NULL, MAKEINTRESOURCE( OBM_MSG_INFO ), IMAGE_BITMAP, 0, 0, LR_SHARED );
		if( hInfoImage )
		{
			HDC hMemDC = CreateCompatibleDC( hdc );
			SelectObject( hMemDC, hInfoImage );
			TransparentBlt( hdc, 13, 12, 40, 40, hMemDC, 0, 0, 40, 40, RGB( 255, 0, 255 ) );
			DeleteObject( hMemDC );
		}
	}

	rectCaption.left = 65;//9 + 50;
	DrawText( hdc, szTitle, -1, &rectCaption, DT_SINGLELINE | DT_VCENTER );

	FillSolidRect( hdc, &rectClient, RGB( 84, 84, 84 ) );
	
	SelectObject( hdc, GetStockObject( NULL_BRUSH ) );

	SelectObject( hdc, hShadePen );
	RoundRect( hdc, rect.left + 2, rect.top + 2, rect.right - 2, rect.bottom - 2, 18, 18 );

	if( dwMainStyle & AGLS_WARNING )
		SelectObject( hdc, hWarnPen );	
	else
		SelectObject( hdc, hInfoPen );

	RoundRect( hdc, rect.left + 2, rect.top + 2, rect.right - 3, rect.bottom - 3, 18, 18 );
	
	DeleteObject( hWarnPen );
	DeleteObject( hInfoPen );
	DeleteObject( hShadePen );
	//DeleteObject( hPen );
/*
    hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    // draw frame
    if( dwMainStyle & WS_THICKFRAME )
    {	//画粗边框
        s = GetSystemMetrics( SM_CXFRAME );
        DrawEdge( hdc, &rect, EDGE_BUMP, BF_RECT );
		InflateRect( &rect, -s, -s );
    }
    else if( (dwMainStyle & WS_DLGFRAME) == WS_DLGFRAME )
    {	//画对话框风格的边框
        s = GetSystemMetrics( SM_CXDLGFRAME );
        Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
        InflateRect( &rect, -1, -1 );
        for( i = 1; i < s; i++ )  
		{
            DrawEdge( hdc, &rect, BDR_RAISEDOUTER, BF_RECT );
			InflateRect( &rect, -1, -1 );
		}
    }
    else if( dwMainStyle & WS_BORDER )
    {	//画单边框
		s = 1;
        Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
		InflateRect( &rect, -s, -s );
    }
	if( dwExStyle & WS_EX_CLIENTEDGE )
	{	//画客户边框
        if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
		    (dwExStyle & WS_EX_TITLE) )
			rect.top += GetSystemMetrics( SM_CYCAPTION );
		DrawEdge( hdc, &rect, BDR_SUNKENOUTER, BF_RECT );
	}

    SelectObject( hdc, hBrush );
	*/
}

// **************************************************
// 声明：static LRESULT DoNcPaint( HWND hWnd, 
// 								WPARAM wParam,
// 								LPARAM lParam, 
// 								BOOL bEraseBk )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - 第一个参数
//	IN lParam - 第二个参数
//	IN bEraseBk - 是否清除背景
// 返回值：
//	1
// 功能描述：
//	处理 WM_NCPAINT消息
// 引用: 
// ************************************************

//处理 WM_NCPAINT 消息

static LRESULT DoNcPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bEraseBk )
{
    HDC hdc;
    DWORD dwMainStyle;
	DWORD dwExStyle;

    if( wParam == 1 )
        hdc = GetDCEx(hWnd, 0, DCX_WINDOW|DCX_CLIPSIBLINGS);
    else	// wParam是一个裁剪区域，需要与dc的区域进行AND运算
        hdc = GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN|DCX_CLIPSIBLINGS);
  
    dwMainStyle = GetWindowLong( hWnd, GWL_MAINSTYLE );
	dwExStyle = GetWindowLong( hWnd, GWL_SUBSTYLE );

	//ShowCaption( hWnd, hdc, dwMainStyle, GetActiveWindow() == hWnd );	


    // 画边框 draw frame
    DrawFrame( hWnd, hdc, dwMainStyle, dwExStyle, bEraseBk );
    // 画标题栏 draw text or icon or syscommand button
//    if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
//		(dwExStyle & WS_EX_TITLE) )
  //      ShowCaption( hWnd, hdc, dwMainStyle, GetActiveWindow() == hWnd );
    // 画滚动条 draw scrollbar
//    if( (dwMainStyle & (WS_HSCROLL | WS_VSCROLL) ) )
  //      DrawScrollBar( hWnd, hdc, dwMainStyle );
    ReleaseDC( hWnd, hdc );
    return 1;
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
	case WM_NCPAINT:
		return DoNcPaint( hWnd, wParam, lParam, FALSE );
	case WM_ERASEBKGND:
		return 1;

	case WM_COMMAND:
		if( HIWORD( wParam ) == BN_CLICKED &&
			( LOWORD( wParam ) >= 1  &&  //== IDOK
			  LOWORD( wParam ) <= 9 ) )  //==IDHELP
		    Dlg_End( hWnd, LOWORD( wParam ) ); 
		return TRUE;
	}
	return FALSE;
}
