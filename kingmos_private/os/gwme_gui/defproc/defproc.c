/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：默认的窗口消息处理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2005-01-08,  去掉了 WM_STYLECHANGED 的无效整个窗口的功能, gwme 将发 WM_NCPAINT 消息
    2003-06-13:  增加 WS_EX_CLIENTEDGE
******************************************************/

#include <eframe.h>
#include <ealloc.h>
#include <eassert.h>
#include <esymbols.h>
#include <gwmeobj.h>
#include "..\include\scrollbar.h"
#include <keybdsrv.h>
#include <ekeybrd.h>
//#include <eapisrv.h>
//#include <eobjcall.h>
//#include <epwin.h>
//#include <winsrv.h>
//#include <gdisrv.h>
//#include <gdc.h>
 
#ifdef EML_DOS
    #include <conio.h>
    #include <stdio.h>
    #include <emouse.h>
#endif

static LRESULT CopyText( HWND, LPSTR, int );
static LRESULT SetText( HWND hWnd, LPCSTR lpcstr );
static BOOL TrackWindowRect( HWND hWnd, POINT ptOld, HWND hwndClipTo, LPRECT lprc );
static LRESULT DoNcPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bEraseBk );
BOOL _GetFrameSize( SIZE FAR* lpSize, DWORD dwMainStyle );
static LRESULT GetTextLength( HWND hWnd );

static HBRUSH hbrushLT = NULL;
static HBRUSH hbrushMT = NULL;
static HBRUSH hbrushRT = NULL;
static HBRUSH hbrushL = NULL;
static HBRUSH hbrushR = NULL;
static HBRUSH hbrushLB = NULL;
static HBRUSH hbrushRB = NULL;
static HBRUSH hbrushMB = NULL;

static HBRUSH hbrushLT_NA = NULL;
static HBRUSH hbrushMT_NA = NULL;
static HBRUSH hbrushRT_NA = NULL;
static HBRUSH hbrushL_NA = NULL;
static HBRUSH hbrushR_NA = NULL;
static HBRUSH hbrushLB_NA = NULL;
static HBRUSH hbrushRB_NA = NULL;
static HBRUSH hbrushMB_NA = NULL;


#define IN_RANGE( v, minv, maxv ) ( (v) >= (minv) && (v) < (maxv) )


// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static LPSTR NewStr( LPCSTR lpstr )
{
    LPSTR p;
    if( lpstr == 0 )
        return 0;
    p = (LPSTR)malloc( strlen(lpstr) + 1 );
    if( p )
        strcpy( p, lpstr );
    return  p;
}


// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static void FreeStr( LPSTR lpstr )
{
    if( lpstr )
        free( lpstr );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL _GetFrameSize( SIZE FAR* lpSize, DWORD dwMainStyle )
{
    lpSize->cx = lpSize->cy = 0;
    if( (dwMainStyle & WS_THICKFRAME) == WS_THICKFRAME )
    {
        lpSize->cx = GetSystemMetrics( SM_CXFRAME );
        lpSize->cy = GetSystemMetrics( SM_CYFRAME );
    }
    else if( (dwMainStyle & WS_DLGFRAME) == WS_DLGFRAME )
    {
        lpSize->cx = GetSystemMetrics( SM_CXDLGFRAME );
        lpSize->cy = GetSystemMetrics( SM_CYDLGFRAME );
    }
    else if( (dwMainStyle & WS_BORDER) == WS_BORDER )
    {
        lpSize->cx = GetSystemMetrics( SM_CXBORDER );
        lpSize->cy = GetSystemMetrics( SM_CYBORDER );
    }
    else
        return FALSE;
    return TRUE;
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

static BOOL TrackWindowRect( HWND hWnd, POINT ptOld,
	              HWND hwndClipTo, LPRECT lprc )
{
    RECT rcNew = *lprc;
    HBRUSH hOldBrush;
    int rop2;
    int xoff, yoff;
    BOOL bEraseTrack;
    HWND hwndTrack;
    HDC hdcTrack;
    RECT rcTrack;
	MSG msg;
    
    bEraseTrack = TRUE;
    if ( GetCapture() != NULL)
		return FALSE;

    if( hwndClipTo )
		UpdateWindow( hwndClipTo );	
	UpdateWindow( hWnd );
	
    hwndTrack = hWnd;
    SetCapture(hwndTrack);

	hdcTrack = GetDC(hwndClipTo);

    hOldBrush = SelectObject( hdcTrack, GetStockObject(NULL_BRUSH) );
    rop2 = SetROP2( hdcTrack, R2_NOT );

    Rectangle( hdcTrack, rcNew.left, rcNew.top, rcNew.right, rcNew.bottom );

    rcTrack = rcNew;
    bEraseTrack= FALSE;

	while( 1 )
	{
        {
			if( GetMessage( &msg, NULL, NULL, NULL ) == 0 )
            {
                ASSERT( 0 );
	            break;
            }
    		if( GetCapture() != hWnd )
            {
	            break;
            }
		    switch (msg.message)
		    {
		    case WM_LBUTTONUP:
		    case WM_MOUSEMOVE:
                // erase rectangle            
                Rectangle( hdcTrack, rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.bottom );
				
                bEraseTrack = TRUE;
                xoff = msg.pt.x - ptOld.x;
                yoff = msg.pt.y - ptOld.y;
                rcNew = *lprc;
                OffsetRect( &rcNew, xoff, yoff );
                if( msg.message == WM_LBUTTONUP )
                    goto ExitLoop;
                Rectangle( hdcTrack, rcNew.left, rcNew.top, rcNew.right, rcNew.bottom );
				//SetWindowPos( hWnd, 0, rcNew.left,  rcNew.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE );
                bEraseTrack = FALSE;
                rcTrack = rcNew;
			    break;
		    default:
    			DispatchMessage(&msg);
	    		break;
    		}
        }
	}
ExitLoop:
    if( bEraseTrack == FALSE )
    {
        Rectangle( hdcTrack, rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.bottom );
        bEraseTrack = TRUE;
    }
    hwndTrack = 0;
    SelectObject( hdcTrack, hOldBrush );
    ReleaseDC( hwndClipTo, hdcTrack );
    hdcTrack = 0;

	ReleaseCapture();
    if( EqualRect(lprc, &rcNew) )
        return FALSE;
    else
    {
        *lprc = rcNew;
        return TRUE;
    }
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static LRESULT GetTextLength( HWND hWnd )
{
    //_LPWINDATA lpws = _GetHWNDPtr( hWnd );
	LPCSTR lpWinText = (LPCSTR)GetWindowLong( hWnd, GWL_TEXT_PTR );

    if( lpWinText )
		return strlen( lpWinText );
	return 0;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static LRESULT SetText( HWND hWnd, LPCSTR lpcstr )
{
    //_LPWINDATA lpws = _GetHWNDPtr( hWnd );
    LPSTR p = NewStr( lpcstr );

    if( p )
    {
		DWORD dwMainStyle, dwExStyle;
		LPSTR lpWinText = (LPSTR)SetWindowLong( hWnd, GWL_TEXT_PTR, (LONG)p );
        if( lpWinText )
            free( lpWinText );
        //lpws->lpWinText = p;
		dwMainStyle = GetWindowLong( hWnd, GWL_STYLE );
		dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
        if( (dwMainStyle & WS_CAPTION) == WS_CAPTION || 
			(dwExStyle & WS_EX_TITLE) )
        {
			/*
            RECT rect = lpws->rectClient;
            HRGN hrgn;

            rect.bottom = rect.top;
            rect.top = rect.bottom - GetSystemMetrics( SM_CYCAPTION );
			// 2003-09-16, 删除代码, WM_NCPAINT's hrgn client坐标
            //OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
            hrgn = CreateRgnRect( rect.left, rect.top, rect.right, rect.bottom );

			
            SendMessage( hWnd, WM_NCPAINT, (WPARAM)hrgn, 0 );
			
            DeleteObject( hrgn );
			*/
			DoNcPaint( hWnd, 1, 0, FALSE );
        }
    }
    return (BOOL)(!(p==0));
}


// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static LRESULT CopyText( HWND hWnd, LPSTR lpstr, int nMax )
{
	LPCSTR lpWinText = (LPCSTR)GetWindowLong( hWnd, GWL_TEXT_PTR );
    //_LPWINDATA lpws = _GetHWNDPtr( hWnd );
    int i = 0;	

    ASSERT( lpstr && nMax > 0 );
    if( lpWinText && nMax > 0 )
    {
        LPCTSTR lpcWinStr = lpWinText;
		nMax--;
		
        while( *lpcWinStr && i < nMax )
		{ 
			*lpstr++ = *lpcWinStr++; i++; 
		}
        *lpstr = 0;
    }
    return i;
}


//  创建位图刷子
static HBRUSH CreateBitmapBrush( UINT id )
{
	HBITMAP hbmp;

	hbmp = LoadImage( NULL, MAKEINTRESOURCE( id ), IMAGE_BITMAP, 0, 0, LR_SHARED );
	
	if( hbmp )
	{
		LOGBRUSH lb;
		
		lb.lbColor = 0;
		lb.lbHatch = (LONG)hbmp;
		lb.lbStyle = BS_PATTERN;
		return CreateBrushIndirect ( &lb ); 
	}
	return NULL;
}

static void FillBitmapBrush( HDC hdc, RECT * lprc, HBRUSH hBrush )
{
	//SelectObject( hdc, hBrush );
	SetBrushOrgEx( hdc, lprc->left, lprc->top, NULL );
	//BitBlt( hdc, lprc->left, lprc->top, lprc->right - lprc->left, lprc->bottom - lprc->top, NULL, 0, 0, PATCOPY );
	FillRect( hdc, lprc, hBrush );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL WINAPI Wnd_DrawCaption( HWND hWnd, HDC hdc, LPCRECT lprc, UINT uFlags )
{
    HBRUSH hBrush;
    HICON hIcon;
    int xicon = 0;
	int oldMode;
    //_LPWINDATA lpws = _GetHWNDPtr( hWnd );
	RECT rect;

	if( hbrushLT == NULL )
	{	// 第一次, 初始化
		hbrushLT = CreateBitmapBrush( OBM_CAP_LT );
		hbrushMT = CreateBitmapBrush( OBM_CAP_MT );
		hbrushRT = CreateBitmapBrush( OBM_CAP_RT );

		hbrushL = CreateBitmapBrush( OBM_BORDER_L );
		hbrushR = CreateBitmapBrush( OBM_BORDER_R );
		hbrushLB = CreateBitmapBrush( OBM_BORDER_LB );
		hbrushRB = CreateBitmapBrush( OBM_BORDER_RB );
		hbrushMB = CreateBitmapBrush( OBM_BORDER_MB );

		hbrushLT_NA = CreateBitmapBrush( OBM_CAP_LT_NA );
		hbrushMT_NA = CreateBitmapBrush( OBM_CAP_MT_NA );
		hbrushRT_NA = CreateBitmapBrush( OBM_CAP_RT_NA );

		hbrushL_NA = CreateBitmapBrush( OBM_BORDER_L_NA );
		hbrushR_NA = CreateBitmapBrush( OBM_BORDER_R_NA );
		hbrushLB_NA = CreateBitmapBrush( OBM_BORDER_LB_NA );
		hbrushRB_NA = CreateBitmapBrush( OBM_BORDER_RB_NA );
		hbrushMB_NA = CreateBitmapBrush( OBM_BORDER_MB_NA );

	}


	rect = *lprc;

    //ASSERT( lpws );
    //if( lpws )
    {
        if( uFlags & DC_ICON )
        {
            //hIcon = lpws->hSmallIcon;//(HICON)WinClass_GetLong( hWnd, GCL_HICON );
			hIcon = (HICON)GetWindowLong( hWnd, GWL_HICONSM );

			if( hIcon == NULL )
			{
				if( uFlags & DC_ACTIVE )
					hIcon = GetStockObject(SYS_STOCK_LOGO);//hicoSysMenu;
				else
					hIcon = GetStockObject(SYS_STOCK_LOGO_GRAY);//hicoGraySysMenu;
			}

            if( hIcon )
            {
                DrawIcon( hdc, lprc->left, lprc->top, hIcon );
				// 2004-07-08, modify
                //xicon = GetSystemMetrics( SM_CXICON );
				xicon = GetSystemMetrics( SM_CXSIZE );				
				//
            }
        }

		// show caption 

        if( uFlags & DC_ACTIVE )
        {
			RECT rc = rect;
            SetTextColor( hdc, GetSysColor(COLOR_CAPTIONTEXT) );
            //hBrush = GetSysColorBrush(COLOR_ACTIVECAPTION);
			rc.right = rc.left + 29;
			FillBitmapBrush( hdc, &rc, hbrushLT );

			rc.left = rc.right;
			rc.right = rect.right - 29;
			FillBitmapBrush( hdc, &rc, hbrushMT );
			
			rc.left = rc.right;
			rc.right = rect.right;
			FillBitmapBrush( hdc, &rc, hbrushRT );
        }
        else
        {
			RECT rc = rect;

            SetTextColor( hdc, GetSysColor(COLOR_INACTIVECAPTIONTEXT) );
            //hBrush = GetSysColorBrush(COLOR_INACTIVECAPTION);

			rc.right = rc.left + 29;
			FillBitmapBrush( hdc, &rc, hbrushLT_NA );
			rc.left = rc.right;
			rc.right = rect.right - 29;
			FillBitmapBrush( hdc, &rc, hbrushMT_NA );
			rc.left = rc.right;
			rc.right = rect.right;
			FillBitmapBrush( hdc, &rc, hbrushRT_NA );

        }
		rect.left += xicon;
		//FillRect( hdc, &rect, hBrush );
		//

        if( (uFlags & DC_TEXT) )
        {
			LPCTSTR lpWinText = (LPCTSTR)GetWindowLong( hWnd, GWL_TEXT_PTR );//lpws->lpWinText
			if( lpWinText )
			{
				oldMode = SetBkMode( hdc, TRANSPARENT );
				DrawText( hdc, lpWinText, strlen( lpWinText ), &rect,  DT_SINGLELINE | DT_END_ELLIPSIS ); 
				SetBkMode( hdc, oldMode );
			}
        }

        return TRUE;
    }    
    return FALSE;
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

#define CAPSTATE  0x00000001
#define CAPDOWN   0x00000002

#define SHIFTDOWN 0x00000004 
#define TOUPPER( w ) (w-32)
BOOL WINAPI Msg_Translate( const MSG FAR * lpMsg )
{
    static WORD wKeyboardState=0;

   BOOL bDown ;
   if ( lpMsg->message == WM_KEYDOWN )
	   bDown = TRUE;
   else
	   bDown = FALSE;
    if( lpMsg->message == WM_KEYDOWN )
    {
		WPARAM wParam;
              
		wParam = lpMsg->wParam;
		{
			//extern void SetKeyState( WPARAM, BOOL );
		    SetKeyState( (UINT8)wParam ,bDown );
		}

		if( ( (DWORD)lpMsg->lParam & 0x80000000 ) == 0 )
		{
			wParam = MapVirtualKey( wParam, 2 );
			if( wParam != -1 )
			{
			    PostMessage( lpMsg->hwnd, WM_CHAR, wParam, 0 );
			}
			
		}
    }
	else if( lpMsg->message == WM_KEYUP )
	{
		SetKeyState((UINT8)lpMsg->wParam ,bDown );
	}
    return FALSE;
}


/**************************************************
声明：static int DoScrollMove( HWND hWnd, LONG hitTest, int x, int y )
参数：
	IN hWnd - 窗口句柄
	IN hitTest - 选中的滚动条类型，为以下值：
		SC_HSCROLL - 水平滚动条
		SC_VSCROLL - 垂直滚动条
	IN x - 在滚动条的鼠标x位置（屏幕坐标）
	IN y - 在滚动条的鼠标y位置（屏幕坐标）
返回值：
	返回 0
功能描述：
	当鼠标在滚动条按下时，会调到该函数。该函数会调用
    滚动条处理函数Scrl_DoLBUTTONDOWN
引用: 
	内部使用
************************************************/

static int DoScrollMove( HWND hWnd, LONG hitTest, int x, int y )
{
    RECT rect;
    
    // 将hitTest转化为Scrl_DoLBUTTONDOWN能识别的值
    if( hitTest == SC_HSCROLL )
        hitTest = SB_HORZ;
    else
        hitTest = SB_VERT;
    // 将x,y转化为相对于窗口的坐标
    GetWindowRect( hWnd, &rect );
    x -= rect.left;
    y -= rect.top;
    //

    return (int)Scrl_DoLBUTTONDOWN( hWnd, (int)hitTest, x, y );
}


/**************************************************
声明：static int DoFrameMove( HWND hWnd, int x, int y )
参数：
	IN hWnd - 窗口句柄
	IN x - 在滚动条的鼠标x位置（屏幕坐标）
	IN y - 在滚动条的鼠标y位置（屏幕坐标）
返回值：
	返回0	
功能描述：
	处理WM_SYSCOMMAND - SC_MOVE消息
	实现窗口的移动
引用: 
	内部使用
************************************************/
	                        
static int DoFrameMove( HWND hWnd, int x, int y )
{
    HWND hParent;
    DWORD dwStyle;
    POINT pt;
    RECT rect;

    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    // 得到该窗口的窗口矩形（屏幕坐标）
    GetWindowRect( hWnd, &rect );
    if( dwStyle & WS_CHILD )
    {	//如果是子窗口，将该窗口矩形由屏幕坐标转化为其父窗口坐标
        hParent = GetParent( hWnd );
        ScreenToClient( hParent, (LPPOINT)&rect );
        ScreenToClient( hParent, ( (LPPOINT)&rect+1 ) );
    }
    else
        hParent = 0;

    pt.x = x;
    pt.y = y;
	
	
    // 这里进入鼠标拖动处理过程
    if( TrackWindowRect( hWnd, pt, hParent, &rect ) )
    {   // 鼠标释放（抬笔），将hWnd移到新的位置
			
        SetWindowPos( hWnd, 0, rect.left,  rect.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE );
        // 立即更新窗口内容
        UpdateWindow( hWnd );
    }
    return 0;
}


/**************************************************
声明：static BOOL GetInterFrame( LPRECT lprect, DWORD dwMainStyle )
参数：
	IN/OUT lprect - 输入窗口矩形坐标，并用该指针接收除边框之外的内矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	由窗口矩形得到除边框之外的内矩形
引用: 
************************************************/

static BOOL GetInterFrame( LPRECT lprect, DWORD dwMainStyle )
{
    SIZE size;
    //  得到除边框之外的内矩形的宽度和高度
    if( _GetFrameSize( &size, dwMainStyle ) )
    {   // 转换为屏幕坐标
        lprect->left += size.cx;
        lprect->top += size.cy;
        lprect->right -= size.cx;
        lprect->bottom -= size.cy;
    }
    else
        return FALSE;
    return TRUE;
}

/**************************************************
声明：static BOOL GetCaptionSysBoxRect( LPRECT lprect, DWORD dwMainStyle )
参数：
	IN/OUT lprect - 输入除边框之外的内矩形，并用该指针接收标题栏上的
					系统方框矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	得到标题栏上的系统方框矩形坐标
引用: 
************************************************/

static BOOL GetCaptionSysBoxRect( LPRECT lprect, DWORD dwMainStyle )
{
	lprect->right = lprect->left + GetSystemMetrics( SM_CXSIZE );
	lprect->bottom = lprect->top + GetSystemMetrics( SM_CYSIZE );
    return TRUE;
}

/**************************************************
声明：static BOOL GetCaptionTextBoxRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle )
参数：
	IN/OUT lprect - 输入除边框之外的内矩形，并用该指针接收标题栏上的
					文本方框矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	得到标题栏上的文本方框矩形坐标
引用: 
************************************************/

static BOOL GetCaptionTextBoxRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle )
{
	int cx = GetSystemMetrics( SM_CXSIZE );
	//如果有系统方框，去掉它
	if( dwMainStyle & WS_SYSMENU )
	    lprect->left += cx;
	//如果有关闭方框，去掉它
    if( dwExStyle & WS_EX_CLOSEBOX )
	{
		lprect->right -= cx;
	}
	//如果有确定方框，去掉它	
    if( dwExStyle & WS_EX_OKBOX )
	{
		lprect->right -= cx;
	}
	//如果有帮助方框，去掉它		
    if( dwExStyle & WS_EX_HELPBOX )
	{
		lprect->right -= cx;
	}
	return TRUE;
}

/**************************************************
声明：static BOOL GetCaptionRect( LPRECT lprect, DWORD dwMainStyle )
参数：
	IN/OUT lprect - 输入除边框之外的内矩形，并用该指针接收标题栏方框矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	得到标题栏方框矩形坐标
引用: 
************************************************/

static BOOL GetCaptionRect( LPRECT lprect, DWORD dwMainStyle )
{
    SIZE size;

	_GetFrameSize( &size, dwMainStyle );
	lprect->left += size.cx;
	lprect->top += size.cy;
	lprect->right -= size.cx;

    lprect->bottom = lprect->top + GetSystemMetrics( SM_CYCAPTION );
	return TRUE;
}

/**************************************************
声明：static BOOL GetCaptionHelpBoxRect( LPRECT lprect, DWORD dwExStyle )
参数：
	IN/OUT lprect - 输入除边框之外的内矩形，并用该指针接收标题栏的帮助方框矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	得到标题栏帮助方框矩形坐标
引用: 
************************************************/

static BOOL GetCaptionHelpBoxRect( LPRECT lprect, DWORD dwExStyle )
{
    lprect->bottom = lprect->top + GetSystemMetrics( SM_CYSIZE );
    if( dwExStyle & WS_EX_CLOSEBOX )
    {  // has close box
        lprect->right -= GetSystemMetrics( SM_CXSIZE );
    }
    if( dwExStyle & WS_EX_OKBOX )
        lprect->right -= GetSystemMetrics( SM_CXSIZE );

    lprect->left = lprect->right - GetSystemMetrics( SM_CXSIZE );
    return TRUE;
}

/**************************************************
声明：static BOOL GetCaptionOkBoxRect( LPRECT lprect, DWORD dwExStyle )
参数：
	IN/OUT lprect - 输入除边框之外的内矩形，并用该指针接收标题栏的确定方框矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	得到标题栏确定方框矩形坐标
引用: 
************************************************/

static BOOL GetCaptionOkBoxRect( LPRECT lprect, DWORD dwExStyle )
{
    lprect->bottom = lprect->top + GetSystemMetrics( SM_CYSIZE );
    if( dwExStyle & WS_EX_CLOSEBOX )
    {  // has close box
        lprect->right -= GetSystemMetrics( SM_CXSIZE );
    }
    lprect->left = lprect->right - GetSystemMetrics( SM_CXSIZE );
    return TRUE;
}

/**************************************************
声明：static BOOL GetCaptionCloseBoxRect( LPRECT lprect, DWORD dwExStyle )
参数：
	IN/OUT lprect - 输入除边框之外的内矩形，并用该指针接收标题栏的关闭方框矩形坐标
	IN dwMainStyle - 窗口风格
返回值：
	假如成功，返回TRUE；否则，返回FALSE	
功能描述：
	得到标题栏关闭方框矩形坐标
引用: 
************************************************/

static BOOL GetCaptionCloseBoxRect( LPRECT lprect, DWORD dwExStyle )
{
    lprect->bottom = lprect->top + GetSystemMetrics( SM_CYSIZE );

    lprect->left = lprect->right - GetSystemMetrics( SM_CXSIZE );
    return TRUE;
}

/**************************************************
声明：static LRESULT DoPaint( HWND hWnd )
参数：
	IN hWnd - 窗口句柄
返回值：
	假如成功，返回0；
功能描述：
	处理WM_PAINT消息
引用: 
************************************************/

static LRESULT DoPaint( HWND hWnd )
{
    PAINTSTRUCT ps;

    BeginPaint( hWnd, &ps );
    EndPaint( hWnd, &ps );
    return 0;
}

/**************************************************
声明：static LRESULT DoErasebkgnd( HWND hWnd, HDC hdc )
参数：
	IN hWnd - 窗口句柄
	IN hdc - 绘图DC
返回值：
	假如擦出了背景，返回1；否则返回0
功能描述：
	处理WM_ERASEBACKGROUND消息
引用: 
************************************************/

static LRESULT DoErasebkgnd( HWND hWnd, HDC hdc )
{
    HBRUSH hBrush = (HBRUSH)GetClassLong( hWnd, GCL_HBRBACKGROUND );
    RECT rect;
    if( hBrush )
    {
        GetClientRect( hWnd, &rect );
        FillRect( hdc, &rect, hBrush );
        return (LRESULT)1;
    }
    return (LRESULT)0;
}

/**************************************************
声明：static LRESULT DoErasebkgnd( HWND hWnd )
参数：
	IN hWnd - 窗口句柄
返回值：
	返回0
功能描述：
	处理WM_DESTROY消息
引用: 
************************************************/

static LRESULT DoDestroy( HWND hWnd )
{
    return 0;
}

static LRESULT DoNcDestroy( HWND hWnd )
{
	DWORD dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );//GetWindowLong( hWnd, GWL_VSCROLL );
	LPSTR lpWinText = (LPSTR)SetWindowLong( hWnd, GWL_TEXT_PTR, 0 );
	if( dwStyle & WS_VSCROLL )
	{
		_ReleaseWindowScrollBar( hWnd, SB_VERT );
	}
	if( dwStyle & WS_HSCROLL )
	{
		_ReleaseWindowScrollBar( hWnd, SB_HORZ );
	}
	if( lpWinText )
	{
//		RETAILMSG( 1, ( "free window text(%x).\r\n", lpWinText ) );
		FreeStr( lpWinText );
	}

    return 0;
}

static LRESULT DoNcCreate( HWND hWnd, LPARAM lParam )
{
	LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
	LPSTR lpcsz = NULL;//
	_SCROLLDATA * lpvs = NULL;
	_SCROLLDATA * lphs = NULL;
	//DWORD dwStyle = SetWindowLong( hWnd, GWL_STYLE );
	//LPCSTR lpWinText = SetWindowLong( hWnd, GWL_TEXT_PTR, 0 );

	//if( lpcs->style & WS_DLGFRAME )
          //_SetRoundWindowFrame(hWnd);  //产生一个圆角边框

	if( lpcs->lpszName )
	{		
		lpcsz = (LPSTR)NewStr(lpcs->lpszName);
		if( lpcsz )
			SetWindowLong( hWnd, GWL_TEXT_PTR, (LONG)lpcsz );
		else
			goto _error;
	}

	if( lpcs->style & WS_THICKFRAME )
	{
		_SetRoundWindowFrame( hWnd );
	}
	if( lpcs->style & WS_VSCROLL )
	{
		if( (lpvs = _GetWindowScrollBar( hWnd, SB_VERT )) == NULL )
		{
			goto _error;
		}
	}
	if( lpcs->style & WS_HSCROLL )
	{
		if( (lphs = _GetWindowScrollBar( hWnd, SB_HORZ )) == NULL )
			goto _error;
	}
	return TRUE;

_error:
	if( lpcsz )
		FreeStr( lpcsz );
	if( lpvs )
		_ReleaseWindowScrollBar( hWnd, SB_VERT );
	if( lphs )
		_ReleaseWindowScrollBar( hWnd, SB_HORZ );
	return FALSE;    
}


/**************************************************
声明：static LRESULT DoNcHitTest( HWND hWnd, WPARAM wParam, LPARAM lParam )
参数：
	IN hWnd - 窗口句柄
    wParam - 第一个参数
    lParam - 第二个参数

返回值：
	返回当前鼠标在窗口的哪个区域，为以下值：
		HTCLIENT - 客户区
		HTTOPLEFT - 左顶边框
		HTBOTTOMLEFT - 左底边框
		HTLEFT - 左边框
		HTTOPRIGHT - 右顶边框
		HTTOP - 顶边框
		HTBOTTOMRIGHT - 右底边框
		HTBOTTOM - 底边框
		HTRIGHT - 右边框
		HTERROR - 错误（没有任何区域）
		HTBORDER - 边框（窗口只有边框，没有其他风格）
		HTVSCROLL - 垂直滚动条
		HTHSCROLL - 水平滚动条
		HTGROWBOX - 右下角的缩放框
		HTCLOSEBOX - 标题栏上的关闭方框
		HTOKBOX - 标题栏上的确认方框
		HTHELPBOX - 标题栏上的帮助方框
		HTSYSMENU - 标题栏上的系统方框
		HTCAPTION - 标题栏
		HTCLIENTEDGE - 客户区边框

功能描述：
	处理WM_NCHITTEST消息

引用: 
************************************************/

static LRESULT DoNcHitTest( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    POINT pt, ptClient;
    RECT rect, rectInter;
    RECT rClient;
    DWORD dwMainStyle, dwExStyle;

    int cx, cy;

    pt.x = LOWORD(lParam); // 屏幕x坐标
    pt.y = HIWORD(lParam); // 屏幕y坐标
	ptClient = pt;

    GetClientRect( hWnd, &rClient );
    ScreenToClient( hWnd, &ptClient ); //转化为客户坐标
    if( PtInRect( &rClient, ptClient ) )	//在客户区吗？
        return HTCLIENT;

    GetWindowRect( hWnd, &rect ); //得到该窗口的屏幕矩形

    dwMainStyle = GetWindowLong( hWnd, GWL_MAINSTYLE );
    dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );

    rectInter = rect;
    if( GetInterFrame( &rectInter, dwMainStyle ) )	//得到除边框外的内矩形
    {
        if( !PtInRect( &rectInter, pt ) )	//在内矩形？
        {  //不在，必然在边框上。 pt is within window but not within rectInter
            if( (dwMainStyle & WS_THICKFRAME) == WS_THICKFRAME )	//粗边框吗？
            {	// 是，以下代码测试在那一个边框上。test which border or corner
            	//cx, cy为上部可缩放边框的高度和宽度
                cy = GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYFRAME );                
                cx = GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CXFRAME );
                if( IN_RANGE( pt.x, rect.left, rectInter.left ) )	//左边框？
                {  // 是，x 在左边框 left border
                    if( IN_RANGE( pt.y, rect.top, rect.top + cy ) )
                        return HTTOPLEFT;	//在上左边框的可缩放边框
                    if( IN_RANGE( pt.y, rect.bottom - cy, rect.bottom ) )
                        return HTBOTTOMLEFT;	//在下左边框的可缩放边框
                    else
                        return HTLEFT;	//其他左边框
                }
                else if( IN_RANGE( pt.y, rect.top, rectInter.top ) )	//上边框？
                {  // 是，y 在上边框 top border
                    if( IN_RANGE( pt.x, rect.left, rect.left + cx ) )
                        return HTTOPLEFT;	//在上左边框的可缩放边框
                    if( IN_RANGE( pt.x, rect.right - cx, rect.right ) )
                        return HTTOPRIGHT;	//在上右边框的可缩放边框
                    else
                        return HTTOP;	//其它上边框
                }
                else if( IN_RANGE( pt.x, rectInter.right, rect.right ) )	//右边框
                {   // right border
                    if( IN_RANGE( pt.y, rect.top, rect.top + cy ) )
                        return HTTOPRIGHT;
                    if( IN_RANGE( pt.y, rect.bottom - cy, rect.bottom ) )
                        return HTBOTTOMRIGHT;
                    else
                        return HTRIGHT;
                }
                else if( IN_RANGE( pt.y, rectInter.bottom, rect.bottom ) )	//下边框
                {  // bottom border
                    if( IN_RANGE( pt.x, rect.left, rect.left + cx ) )
                        return HTBOTTOMLEFT;
                    if( IN_RANGE( pt.x, rect.right - cx, rect.right ) )
                        return HTBOTTOMRIGHT;
                    else
                        return HTBOTTOM;
                }
                else
                {
                    ASSERT( 0 ); // 错误！！，不应该发生。 error
                    return HTERROR;
                }
            }
            //非粗边框
            return HTBORDER;
        }
    }
	// 不在边框上。以下代码测试是否在其他区域。test in scroll bar ?
    if( dwMainStyle & (WS_HSCROLL | WS_VSCROLL) )
    {	//窗口有滚动条
        // save rect
    	if( dwMainStyle & WS_VSCROLL )
    	{
            rectInter = rect;
            // 得到垂直滚动条的矩形
            Scrl_GetBarRect( &rectInter, dwMainStyle, dwExStyle, SB_VERT );
            if( PtInRect( &rectInter, pt ) )
                return HTVSCROLL; // 在垂直滚动条上
    	}
    	if( dwMainStyle & WS_HSCROLL )
    	{
            rectInter = rect;
            // 得到水平滚动条的矩形
            Scrl_GetBarRect( &rectInter, dwMainStyle, dwExStyle, SB_HORZ );
            if( PtInRect( &rectInter, pt ) )
                return HTHSCROLL;	//在水平滚动条上
    	}
    	if( (dwMainStyle & (WS_HSCROLL | WS_VSCROLL)) == (WS_HSCROLL | WS_VSCROLL) )
    	{
            rectInter = rect;
            // 得到缩放框的的矩形            
            Scrl_GetBarRect( &rectInter, dwMainStyle, dwExStyle, SB_CORNER );
            if( PtInRect( &rectInter, pt ) )
                return HTGROWBOX;	//在缩放框上
    	}
    }

	// test in caption or icon or syscommand button
    if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
		 ( dwExStyle & WS_EX_TITLE ) )
    {	//窗口有标题栏
        rectInter = rect;
        //得到标题栏的矩形
        GetCaptionRect( &rect, dwMainStyle );
        if( PtInRect( &rect, pt ) )
        {	//在标题栏里
            if( dwExStyle & WS_EX_CLOSEBOX )
            {	//标题栏上有关闭框
                GetCaptionCloseBoxRect( &rect, dwExStyle );
                if( PtInRect( &rect, pt ) )
                    return HTCLOSEBOX;
            }
            
            if( dwExStyle & WS_EX_OKBOX )
            {	//标题栏上有确认框
                rect = rectInter;
                GetCaptionOkBoxRect( &rect, dwExStyle );
                if( PtInRect( &rect, pt ) )
                    return HTOKBOX;
            }
            
            if( dwExStyle & WS_EX_HELPBOX )
            {	//标题栏上有帮助框
                rect = rectInter;
                GetCaptionHelpBoxRect( &rect, dwExStyle );
                if( PtInRect( &rect, pt ) )
                    return HTHELPBOX;
            }

			if( dwMainStyle & WS_SYSMENU )
			{	//标题栏上有系统框
				rect = rectInter;
				GetCaptionSysBoxRect( &rect, dwMainStyle );
				if( PtInRect( &rect, pt ) )
					return HTSYSMENU;
			}
			//在标题栏的文字框里
            return HTCAPTION;
        }
    }

    if( dwExStyle & WS_EX_CLIENTEDGE )
	{   //有客户边框，客户边框只有1点粗，这里将客户矩形扩大一点粗
		InflateRect( &rClient, 1, 1 );
		//客户边框里包含滚动条，因此，如果有滚动条，应相应中间客户边框的大小
        if( dwMainStyle & WS_HSCROLL )
			rClient.bottom += GetSystemMetrics( SM_CYHSCROLL );			
        if( dwMainStyle & WS_VSCROLL )
			rClient.right += GetSystemMetrics( SM_CXVSCROLL );
		if( PtInRect( &rClient, ptClient ) )
			return HTCLIENTEDGE;	//在客户边框上
	}

    ASSERT( 0 );	//不应该执行到这里
    return HTERROR;
}

// **************************************************
// 声明：static void DrawScrollBar( HWND hWnd, HDC hdc, DWORD dwMainStyle )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
//	IN dwMainStyle - 窗口风格
// 返回值：
//	无
// 功能描述：
//	绘制滚动条
// 引用: 
// ************************************************

static void DrawScrollBar( HWND hWnd, HDC hdc, DWORD dwMainStyle )
{
    if( dwMainStyle & WS_VSCROLL )
    {	// 画垂直滚动条
        Scrl_RedrawScrollBar( hWnd, hdc, SB_VERT );
    }
    if( dwMainStyle & WS_HSCROLL )
    {	// 画水平滚动条
        Scrl_RedrawScrollBar( hWnd, hdc, SB_HORZ );
    }
    if( (dwMainStyle & WS_HSCROLL) && (dwMainStyle & WS_VSCROLL) )
    {	//画右下角的缩放框
        Scrl_RedrawScrollBar( hWnd, hdc, SB_CORNER );
    }
}
/*   the version is show text mask of close and help and ok 
// **************************************************
// 声明：static void ShowCaption( HWND hWnd, HDC hdc, DWORD dwMainStyle, BOOL fActive )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
//	IN dwMainStyle - 窗口风格
//	IN fActive - 标题条是否是活动的
// 返回值：
//	无
// 功能描述：
//	画窗口上的整个标题条
// 引用: 
// ************************************************

static void ShowCaption( HWND hWnd, HDC hdc, DWORD dwMainStyle, BOOL fActive )
{
    extern void * _Win_GetTextAdr( HWND hWnd );
    RECT rect;
    DWORD dwExStyle;

    //char buf[32];

    if( (dwMainStyle & WS_CAPTION) == WS_CAPTION )
    {
        GetWindowRect( hWnd, &rect );
        OffsetRect( &rect, -rect.left, -rect.top );
        GetInterFrame( &rect, dwMainStyle );
        rect.bottom = rect.top + GetSystemMetrics( SM_CYCAPTION );
        DrawCaption( hWnd, hdc, &rect, fActive ? DC_ACTIVE|DC_TEXT|DC_ICON : DC_TEXT|DC_ICON );
        dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
        if( dwExStyle & (WS_EX_CLOSEBOX | WS_EX_OKBOX | WS_EX_HELPBOX) )
        {            
			extern const HFONT __hSymbol16x16;
			BYTE bSymbol;
			HFONT hFont = SelectObject( hdc, __hSymbol16x16 );

            SetTextColor(hdc, fActive ? CL_WHITE : CL_BLACK );
            SetBkMode(hdc, TRANSPARENT);
            if( dwExStyle & WS_EX_CLOSEBOX )
            {
				bSymbol = SYM_CLOSE;

                WinGdi_TextOut( hdc, rect.right - 16, rect.top, &bSymbol, 1 );
                rect.right -= 16;
            }
            if( dwExStyle & WS_EX_OKBOX )
            {
				bSymbol = SYM_OK;
                WinGdi_TextOut( hdc, rect.right - 16, rect.top, &bSymbol, 1 );
                rect.right -= 16;
            }
            if( dwExStyle & WS_EX_HELPBOX )
            {
				bSymbol = SYM_HELP;
                WinGdi_TextOut( hdc, rect.right - 16, rect.top, &bSymbol, 1 );
            }
			SelectObject( hdc, hFont );
            SetBkMode(hdc, OPAQUE);
        }
    }
}
*/

// **************************************************
// 声明：static void ShowCaption( HWND hWnd, HDC hdc, DWORD dwMainStyle, BOOL fActive )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
//	IN dwMainStyle - 窗口风格
//	IN fActive - 标题条是否是活动的
// 返回值：
//	无
// 功能描述：
//	画窗口上的整个标题条
// 引用: 
// ************************************************

static void ShowCaption( HWND hWnd, HDC hdc, DWORD dwMainStyle, BOOL fActive )
{
    RECT rtCaption, rt;
    //DWORD dwExStyle;
	//_LPWINDATA lpws = _GetHWNDPtr( hWnd );
	BITMAP bitmap;
	DWORD dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );


    if( //lpws && 
		(  (dwMainStyle & WS_CAPTION) == WS_CAPTION || 
		   (dwExStyle & WS_EX_TITLE) 
		) 
	  )	//有标题栏吗？
    {	//有
        //int yIconHeight, xIconWidth;

		//rtCaption = lpws->rectWindow;
		GetWindowRect( hWnd, &rtCaption );
        OffsetRect( &rtCaption, -rtCaption.left, -rtCaption.top );
        //得到标题栏矩形
		GetCaptionRect( &rtCaption, dwMainStyle );
		//画标题栏背景
		{
			RECT rc = rtCaption;
			rc.top -= GetSystemMetrics( SM_CYFRAME );  // XP like 
			rc.left -= GetSystemMetrics( SM_CXFRAME );  // XP like 
			rc.right += GetSystemMetrics( SM_CXFRAME );  // XP like 
			DrawCaption( hWnd, hdc, &rc, fActive ? DC_ACTIVE : 0 );
		}

		//得到系统栏图标的大小
		//2004-07-08, modify
        //yIconHeight = GetSystemMetrics(SM_CYSMICON);
		//xIconWidth = GetSystemMetrics(SM_CXSMICON);
        //yIconHeight = GetSystemMetrics(SM_CYSIZE);
		//xIconWidth = GetSystemMetrics(SM_CXSIZE);
		//


		// 如果有系统栏,则画之。now , draw sysmenu icon if possible
        if( dwMainStyle & WS_SYSMENU )
        {
			HICON hIcon;
			ICONINFO iconInfo;

            hIcon = (HICON)GetWindowLong( hWnd, GWL_HICONSM );//lpws->hSmallIcon;//小图标
			if( hIcon == NULL )
			{	//如果没有用户提供的小图标，则用系统默认的
				if( fActive )
				{
					hIcon = GetStockObject( SYS_STOCK_LOGO );//hicoSysMenu;//系统活动图标
				}
				else
				{
					hIcon = GetStockObject( SYS_STOCK_LOGO_GRAY );//hicoGraySysMenu;//系统非活动图标
				}
			}
			GetIconInfo( hIcon, &iconInfo );
			GetObject( iconInfo.hbmColor, sizeof(bitmap), &bitmap );
			
		    rt = rtCaption;
		    //得到系统框大小
			GetCaptionSysBoxRect( &rt, dwMainStyle );
			//居中
			//rt.top += ( (rt.bottom - rt.top) - yIconHeight ) >> 1;// align to middle
			//rt.left += ( (rt.right - rt.left) - xIconWidth ) >> 1;// align to middle
			rt.top += ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1;// align to middle
			rt.left += ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1;// align to middle
			//画图标
            DrawIcon( hdc, rt.left, rt.top, hIcon );
        }

        //dwExStyle = lpws->dwExStyle;//GetWindowLong( hWnd, GWL_EXSTYLE );

		// 画标题栏上的文本
		{
			TCHAR szText[128];
			int len;
			len = GetWindowText( hWnd, szText, sizeof(szText)-sizeof(TCHAR) );
			// now , draw caption text if possible
			if( len )//lpws->lpWinText )
			{	//因为已经清除了背景，所以这里画文本用透明模式
				DWORD oldMode = SetBkMode( hdc, TRANSPARENT );	
				
				rt = rtCaption;
				//得到文本矩形
				GetCaptionTextBoxRect( &rt, dwMainStyle, dwExStyle );
				//画之
				DrawText( hdc, szText, len, &rt,  DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS ); 
				SetBkMode( hdc, oldMode );
			}
		}

        // now draw option icon if possible
		// 画标题栏上的功能框
        if( dwExStyle & (WS_EX_CLOSEBOX | WS_EX_OKBOX | WS_EX_HELPBOX) )
        {            
			int xBtWidth;
			HANDLE hbmpSave, hbmp;
			HANDLE hMemDC = CreateCompatibleDC( hdc );
			int x, y;

            rt = rtCaption;
            //得到关闭框矩形
			GetCaptionCloseBoxRect( &rt, dwExStyle );
			xBtWidth = rt.right - rt.left;
			//居中
			//rt.top += ( (rt.bottom - rt.top) - yIconHeight ) >> 1;// align to middle
			//rt.left += ( (rt.right - rt.left) - xIconWidth ) >> 1;// align to middle
			hbmp = GetStockObject(SYS_STOCK_CAPTION_CLOSE);
			GetObject( hbmp, sizeof(bitmap), &bitmap );

			if( dwExStyle & WS_EX_CLOSEBOX )
            {	//画关闭框
				if( fActive )
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_CLOSE);//hbmpClose;//用活动的位图
				}
				else
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_CLOSE_GRAY);//hbmpGrayClose;//用非活动的位图
				}
				//GetObject( hbmp, sizeof(bitmap), &bitmap );
				y = rt.top + ( ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1 );
				x = rt.left + ( ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1 );// align to middle

	            hbmpSave = SelectObject( hMemDC, hbmp );
				//画之
				//BitBlt( hdc, rt.left, rt.top, xIconWidth, yIconHeight, hMemDC, 0, 0, SRCCOPY );
				BitBlt( hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );

	            SelectObject( hMemDC, hbmpSave );

				rt.left -= xBtWidth;//为后面的绘制准备矩形
                rt.right -= xBtWidth;//为后面的绘制准备矩形
            }
            if( dwExStyle & WS_EX_OKBOX )
            {	//画关闭框
				if( fActive )
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_OK);//hbmpOk;//用活动的位图
				}
				else
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_OK_GRAY);//hbmpGrayOk;//用非活动的位图
				}
				//GetObject( hbmp, sizeof(bitmap), &bitmap );
				y = rt.top + ( ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1 );
				x = rt.left + ( ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1 );// align to middle

	            hbmpSave = SelectObject( hMemDC, hbmp );
	            //画之
				//BitBlt( hdc, rt.left, rt.top, xIconWidth, yIconHeight, hMemDC, 0, 0, SRCCOPY );
				BitBlt( hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );
                SelectObject( hMemDC, hbmpSave );
				rt.left -= xBtWidth;//为后面的绘制准备矩形
                rt.right -= xBtWidth;//为后面的绘制准备矩形
            }
            if( dwExStyle & WS_EX_HELPBOX )
            {	//画帮助框
				if( fActive )
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_HELP);//hbmpHelp;
				}
				else
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_HELP_GRAY);//hbmpGrayHelp;
				}
				//GetObject( hbmp, sizeof(bitmap), &bitmap );
				y = rt.top + ( ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1 );
				x = rt.left + ( ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1 );// align to middle

	            hbmpSave = SelectObject( hMemDC, hbmp );
				//BitBlt( hdc, rt.left, rt.top, xIconWidth, yIconHeight, hMemDC, 0, 0, SRCCOPY );
				BitBlt( hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );

                SelectObject( hMemDC, hbmpSave );
            }
			DeleteDC( hMemDC );
        }
    }
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
					   BOOL bEraseBk,
					   BOOL bActive )
{
    RECT rect;
    HBRUSH hBrush;
    int s, i;

	

    GetWindowRect( hWnd, &rect );
    OffsetRect( &rect, -rect.left, -rect.top );

	if( bEraseBk )
	{	//清除背景
	    FillRect( hdc, &rect, (HBRUSH)(COLOR_WINDOW+1) );
	}

    hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    // draw frame
    if( dwMainStyle & WS_THICKFRAME )
    {	//画粗边框
		HPEN hPen, hOld;
		RECT rcBorder = rect;

        s = GetSystemMetrics( SM_CXFRAME );

		rcBorder.top += GetSystemMetrics( SM_CYCAPTION ) + s;
		rcBorder.right = rcBorder.left + s;
		if( bActive )
			FillBitmapBrush( hdc, &rcBorder, hbrushL );
		else
			FillBitmapBrush( hdc, &rcBorder, hbrushL_NA );


		//rcBorder.top += GetSystemMetrics( SM_CYCAPTION );
		rcBorder.right = rect.right;
		rcBorder.left = rcBorder.right - s;
		if( bActive )
			FillBitmapBrush( hdc, &rcBorder, hbrushR );
		else
			FillBitmapBrush( hdc, &rcBorder, hbrushR_NA );

		
		rcBorder.left = rect.left;
		rcBorder.right = rcBorder.left + 4;
		rcBorder.top = rect.bottom - s;
		rcBorder.bottom = rect.bottom;
		if( bActive )
			FillBitmapBrush( hdc, &rcBorder, hbrushLB );
		else
			FillBitmapBrush( hdc, &rcBorder, hbrushLB_NA );

		rcBorder.left = rect.left + 4;
		rcBorder.right = rect.right - 4;
		if( bActive )
			FillBitmapBrush( hdc, &rcBorder, hbrushMB );
		else
			FillBitmapBrush( hdc, &rcBorder, hbrushMB_NA );

		rcBorder.left = rect.right - 4;
		rcBorder.right = rect.right;
		if( bActive )
			FillBitmapBrush( hdc, &rcBorder, hbrushRB );
		else
			FillBitmapBrush( hdc, &rcBorder, hbrushRB_NA );


		/*
		hPen = CreatePen( PS_SOLID, s, RGB( 0, 0, 0xff ) );
        //DrawEdge( hdc, &rect, EDGE_RAISED, BF_RECT );
		hOld = SelectObject( hdc, hPen );
		RoundRect( hdc, rect.left+1, rect.top+1, rect.right-1, rect.bottom-1, 18, 18 );
		 SelectObject( hdc, hOld );
		DeleteObject( hPen );
		*/
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
}

// **************************************************
// 声明：static LRESULT DoNcActivate( HWND hWnd, BOOL fActive )
// 参数：
//	IN hWnd - 窗口句柄
//	IN fActive - 是否窗口是活动的
// 返回值：
//	TRUE
// 功能描述：
//	处理WM_NCACTIVATE消息
// 引用: 
// ************************************************

static LRESULT DoNcActivate( HWND hWnd, BOOL fActive )
{
    HDC hdc;
	DWORD dwStyle, dwExStyle;

    
    dwStyle = (DWORD)GetWindowLong( hWnd, GWL_MAINSTYLE );
	dwExStyle = (DWORD)GetWindowLong( hWnd, GWL_EXSTYLE );
	hdc = GetWindowDC( hWnd );    
    ShowCaption( hWnd, hdc, dwStyle, fActive );

    // 画边框 draw frame
    DrawFrame( hWnd, hdc, dwStyle, dwExStyle, FALSE, fActive );

    ReleaseDC( hWnd, hdc );
    return TRUE;
}

// **************************************************
// 声明：static int DoSysCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - 第一个参数
//	IN lParam - 第二个参数
// 返回值：
//	0
// 功能描述：
//	处理WM_SYSCOMMAND消息
// 引用: 
// ************************************************

static int DoSysCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    if( wParam == SC_MOVE )
        return DoFrameMove( hWnd, LOWORD( lParam ), HIWORD( lParam )  );
    else if( wParam == SC_HSCROLL || wParam == SC_VSCROLL )
        return DoScrollMove( hWnd, wParam, LOWORD( lParam ), HIWORD( lParam ) );
    else if( wParam == SC_CLOSE )
        PostMessage( hWnd, WM_CLOSE, 0, 0 );
    else if( wParam == SC_OK )
        PostMessage( hWnd, WM_OK, 0, 0 );
    else if( wParam == SC_CONTEXTHELP )
        PostMessage( hWnd, WM_HELP, 0, 0 );
    return 0;
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

static LRESULT DoNcPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bEraseBk )
{
    HDC hdc;
    DWORD dwMainStyle;
	DWORD dwExStyle;
	BOOL bActive;

    if( wParam == 1 )
        hdc = GetDCEx(hWnd, 0, DCX_WINDOW|DCX_CLIPSIBLINGS);
    else	// wParam是一个裁剪区域，需要与dc的区域进行AND运算
        hdc = GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN|DCX_CLIPSIBLINGS);
    
    dwMainStyle = GetWindowLong( hWnd, GWL_MAINSTYLE );
	dwExStyle = GetWindowLong( hWnd, GWL_SUBSTYLE );
    // 画标题栏 draw text or icon or syscommand button
	bActive = ( GetActiveWindow() == hWnd );
    //if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
		//(dwExStyle & WS_EX_TITLE) )
    ShowCaption( hWnd, hdc, dwMainStyle, bActive );

    // 画边框 draw frame
    DrawFrame( hWnd, hdc, dwMainStyle, dwExStyle, bEraseBk, bActive );
    // 画滚动条 draw scrollbar
    if( (dwMainStyle & (WS_HSCROLL | WS_VSCROLL) ) )
        DrawScrollBar( hWnd, hdc, dwMainStyle );
    ReleaseDC( hWnd, hdc );
    return 1;
}

// **************************************************
// 声明：LRESULT CALLBACK WinDef_NCPaint( HWND hWnd, 
// 									   WPARAM wParam,
// 									   LPARAM lParam,
// 									   BOOL bEraseBk )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - 第一个参数
//	IN lParam - 第二个参数
//	IN bEraseBk - 是否清除背景
// 返回值：
// 功能描述：
//	对非客户区进行绘制 
// 引用: 
// ************************************************

// call by win.c
LRESULT CALLBACK WinDef_NCPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bEraseBk )
{
	return DoNcPaint( hWnd, wParam, lParam, bEraseBk );
}

// **************************************************
// 声明：static LRESULT DoNcCalcSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
//	IN hWnd - 窗口句柄
//	IN wParam - 第一个参数
//	IN/OUT lParam - 第二个参数（为RECT结构指针,传入当前窗口大小，返回客户区大小）
// 返回值：
//	0
// 功能描述：
//	处理 WM_NCCALCSIZE 消息- 计算客户区的大小
// 引用: 
// ************************************************

static LRESULT DoNcCalcSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LPRECT lprect;
    DWORD dwMainStyle;	
    DWORD dwExStyle;

    if( wParam == 0 )
    {
        lprect = (LPRECT)lParam;
        // 得到风格
        dwMainStyle = GetWindowLong( hWnd, GWL_STYLE );
        dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );
        
        if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
			(dwExStyle & WS_EX_TITLE ) )
        {
            lprect->top += GetSystemMetrics( SM_CYCAPTION );
        }
        if( (dwMainStyle & WS_THICKFRAME) == WS_THICKFRAME )
        {
            lprect->left += GetSystemMetrics( SM_CXFRAME );
            lprect->top += GetSystemMetrics( SM_CYFRAME );
            lprect->right -= GetSystemMetrics( SM_CXFRAME );
            lprect->bottom -= GetSystemMetrics( SM_CYFRAME );
        }
        else if( (dwMainStyle & WS_DLGFRAME) == WS_DLGFRAME )
        {
            lprect->left += GetSystemMetrics( SM_CXDLGFRAME );
            lprect->top += GetSystemMetrics( SM_CYDLGFRAME );
            lprect->right -= GetSystemMetrics( SM_CXDLGFRAME );
            lprect->bottom -= GetSystemMetrics( SM_CYDLGFRAME );
        }
        else if( (dwMainStyle & WS_BORDER) == WS_BORDER )
        {
            lprect->left += GetSystemMetrics( SM_CXBORDER );
            lprect->top += GetSystemMetrics( SM_CYBORDER );
            lprect->right -= GetSystemMetrics( SM_CXBORDER );
            lprect->bottom -= GetSystemMetrics( SM_CYBORDER );
        }
        if( dwMainStyle & WS_HSCROLL )
            lprect->bottom -= GetSystemMetrics( SM_CYHSCROLL );
        if( dwMainStyle & WS_VSCROLL )
            lprect->right -= GetSystemMetrics( SM_CXVSCROLL );  
		if( dwExStyle & WS_EX_CLIENTEDGE )
		{
			lprect->left++;
			lprect->top++;
			lprect->right--;
			lprect->bottom--;
		}
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoActivate( HWND hWnd, BOOL fActivate, BOOL fMinimize )
// 参数：
// 	IN hWnd - 窗口句柄
// 	IN fActivate - 是否活动
// 	IN fMinimize - 是否最小化（不支持）
// 返回值：
// 	0
// 功能描述：
//	处理 WM_ACTIVATE 消息
// 引用: 
// ************************************************

static LRESULT DoActivate( HWND hWnd, BOOL fActivate, BOOL fMinimize )
{
    DWORD dwStyle;
    if( fActivate && fMinimize == FALSE )
    {        
        dwStyle = GetWindowLong( hWnd, GWL_STYLE );
        
        if( !(dwStyle & WS_CHILD) )
        {   // popup and overlappend window
            //SetShellBarMenu( hWnd );
			HWND hwndFocus;
			if( ( hwndFocus = GetFocus() ) )
			{
				SetFocus( hwndFocus );
			}
			else
				SetFocus( hWnd );
        }
    }
    return 0;
}

// **************************************************
// 声明：static LRESULT DoMouseActivate( HWND hWnd, 
// 									  HWND hTopLevel,
// 									  WORD wHitValue,
//  									 WORD wMouse )
// 参数：
// 	IN hWnd - 窗口句柄
// 	IN hTopLevel - 顶级窗口
// 	IN wHitValue - 当前鼠标点中的窗口位置
// 	IN wMouse - 鼠标状态

// 返回值：
// 功能描述：
//	处理 WM_MOUSEACTIVATE 消息
// 引用: 
// ************************************************

static LRESULT DoMouseActivate( HWND hWnd, HWND hTopLevel, WORD wHitValue, WORD wMouse )
{
    HWND hwndTemp;

    if( hTopLevel == hWnd &&
        wMouse == WM_LBUTTONDOWN )
        return MA_ACTIVATE; // 如果自己就是顶级窗口, 激活它
    else
    {
        hwndTemp = GetParent( hWnd ); // 该窗口有父窗口吗？
        if( hwndTemp )	// 有，则给父窗口发该消息
            return SendMessage( hwndTemp, WM_MOUSEACTIVATE, (WPARAM)hTopLevel, MAKELONG( wHitValue, wMouse  ) );
    }
    return MA_ACTIVATE;
}

// **************************************************
// 声明：static LRESULT DoNcLButtonDown( HWND hWnd, WORD wHitValue, LPARAM lParam )
// 参数：
// 	IN hWnd - 窗口句柄
// 	IN wHitValue - 当前鼠标点中的窗口位置
//	 IN lParam - x, y 坐标
// 返回值：
//	0
// 功能描述：
//	处理 WM_NCLBUTTONDOWN 消息
// 引用: 
// ************************************************/

static LRESULT DoNcLButtonDown( HWND hWnd, WORD wHitValue, LPARAM lParam )
{
    // the code is remove to message.c 's HandlePosEvent, 2001.09.30
    //if( GetActiveWindow() != GetTopLevelParent( hWnd ) )
          //SetForegroundWindow( GetTopLevelParent( hWnd ) );//, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE );
    if( wHitValue == HTCAPTION )
	{
		if( !( GetWindowLong( hWnd, GWL_EXSTYLE ) & WS_EX_NOMOVE) )
            SendMessage( hWnd, WM_SYSCOMMAND, SC_MOVE, lParam );
	}
    else if( wHitValue == HTVSCROLL )
        SendMessage( hWnd, WM_SYSCOMMAND, SC_VSCROLL, lParam );
    else if( wHitValue == HTHSCROLL )
        SendMessage( hWnd, WM_SYSCOMMAND, SC_HSCROLL, lParam );
    return 0;
}

// **************************************************
// 声明：static LRESULT DoNcLButtonUp( HWND hWnd, WORD wHitValue, LPARAM lParam )
// 参数：
// 	IN hWnd - 窗口句柄
// 	IN wHitValue - 当前鼠标点中的窗口位置
//	 IN lParam - x, y 坐标
// 返回值：
//	0
// 功能描述：
//	处理 WM_NCLBUTTONUP 消息
// 引用: 
// ************************************************

static LRESULT DoNcLButtonUp( HWND hWnd, WORD wHitValue, LPARAM lParam )
{
    if( wHitValue == HTCLOSEBOX )
        SendMessage( hWnd, WM_SYSCOMMAND, SC_CLOSE, lParam );
    else if( wHitValue == HTOKBOX )
        SendMessage( hWnd, WM_SYSCOMMAND, SC_OK, lParam );
    else if( wHitValue == HTHELPBOX )
        SendMessage( hWnd, WM_SYSCOMMAND, SC_CONTEXTHELP, lParam );

    return 0;
}

// **************************************************
// 声明：static LRESULT DoClose( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	0
// 功能描述：
//	处理 WM_CLOSE 消息
// 引用: 
//	
// ************************************************

static LRESULT DoClose( HWND hWnd )
{
    DestroyWindow( hWnd );
    return 0;
}

// **************************************************
// 声明：static LRESULT DoTimer( HWND hWnd, UINT uID, TIMERPROC lpTimerProc )
// 参数：
//    IN hWnd - 窗口句柄
//	IN uID - 定时器ID
//	IN lpTimerProc - 定时器回调函数
// 返回值：
//	0
// 功能描述：
//	处理 WM_TIMER 消息
// 引用: 
// ************************************************

static LRESULT DoTimer( HWND hWnd, UINT uID, TIMERPROC lpTimerProc )
{
    if( lpTimerProc )
	{
		/*
        _LPWINDATA lpws;
        lpws = _GetHWNDPtr( hWnd );

		if( lpws )
		{
			CALLBACKDATA cd;
			// 准备回调参数
			cd.hProcess = lpws->lpClass->hOwnerProcess;
			cd.lpfn = (FARPROC)lpTimerProc;
			cd.dwArg0 = (DWORD)hWnd;
			// 回调
			Sys_ImplementCallBack4( &cd, WM_TIMER, uID, GetTickCount() );
		}
		*/
		lpTimerProc( hWnd, WM_TIMER, uID, GetTickCount() );

	}
    return 0;
}

// **************************************************
// 声明：static LRESULT DoSysTimer( HWND hWnd, UINT uID, TIMERPROC lpTimerProc )
// 参数：
//    IN hWnd - 窗口句柄
//	IN uID - 定时器ID
//	IN lpTimerProc - 定时器回调函数

// 返回值：
//	0
// 功能描述：
//	处理 WM_SYSTIMER 消息
// 引用: 
// ************************************************

static LRESULT DoSysTimer( HWND hWnd, UINT uID, TIMERPROC lpTimerProc )
{
    if( lpTimerProc )
	{
		/*
        _LPWINDATA lpws;
        lpws = _GetHWNDPtr( hWnd );

		if( lpws )
		{
			CALLBACKDATA cd;
			// 准备回调参数			
			cd.hProcess = lpws->lpClass->hOwnerProcess;
			cd.lpfn = (FARPROC)lpTimerProc;
			cd.dwArg0 = (DWORD)hWnd;
			// 回调	
			Sys_ImplementCallBack4( &cd, WM_SYSTIMER, uID, GetTickCount() );
		}
		*/
		lpTimerProc( hWnd, WM_SYSTIMER, uID, GetTickCount() );
	}
    return 0;
}

// **************************************************
// 声明：static LRESULT DoSetIcon( HWND hWnd, UINT fIconType, HICON hNewIcon )
// 参数：
//    IN hWnd - 窗口句柄
//	IN fIconType - 图标类型
//		ICON_SMALL - 小图标
//	IN hNewIcon - 图标句柄
// 返回值：
//	老的图标句柄
// 功能描述：
//	处理 WM_SETICON 设置窗口的系统图标
// 引用: 
// ************************************************

static LRESULT DoSetIcon( HWND hWnd, UINT fIconType, HICON hNewIcon )
{
	HICON hRetv = NULL;

//	if( hNewIcon && _GetHICONPtr(hNewIcon) == NULL )
		//return NULL;

	if( fIconType == ICON_SMALL )
	{
		/*
        _LPWINDATA lpws;
        lpws = _GetHWNDPtr( hWnd );

		if( lpws )
		{
			hRetv = lpws->hSmallIcon;
			lpws->hSmallIcon = hNewIcon;
		}
		*/
		hRetv = (HICON)SetWindowLong( hWnd, GWL_HICONSM, (LONG)hNewIcon );
	}
	return (LRESULT)hRetv;
}

// **************************************************
// 声明：static LRESULT DoGetIcon( HWND hWnd, UINT fIconType )
// 参数：
//    IN hWnd - 窗口句柄
//	IN fIconType - 图标类型
//		ICON_SMALL - 小图标
// 返回值：
//	图标句柄
// 功能描述：
//	处理 WM_GETICON 消息，得到当前的图标句柄
// 引用: 
// ************************************************

static LRESULT DoGetIcon( HWND hWnd, UINT fIconType )
{
	HICON hRetv = NULL;

	if( fIconType == ICON_SMALL )
	{
		/*
        _LPWINDATA lpws;
        lpws = _GetHWNDPtr( hWnd );

		if( lpws )
			hRetv = lpws->hSmallIcon;
			*/
		hRetv = (HICON)GetWindowLong( hWnd, GWL_HICONSM );
	}
	return (LRESULT)hRetv;
}

// **************************************************
// 声明：static BOOL CALLBACK EnumChildProc( HWND hwnd, LPARAM lParam )
// 参数：
// 	IN hWnd - 窗口句柄
//	IN lParam - LPARAM 参数,这里是代表消息类型
// 返回值：
//	返回 TRUE,继续枚举; FALSE,停止枚举
// 功能描述：
//	枚举子窗口
// 引用: 
// ************************************************

static BOOL CALLBACK EnumChildProc( HWND hWnd, LPARAM lParam )
{
	//RETAILMSG( 1, ( "ec++:0x%x.\r\n", lParam ) );
	SendMessage( hWnd, (UINT)lParam, 0, 0 );
	//RETAILMSG( 1, ( "ec--.\r\n" ) );
	return TRUE;
}

// **************************************************
// 声明：static LRESULT DoSysColorChange( HWND hWnd )
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	返回 0
// 功能描述：
//	处理系统颜色改变
// 引用: 
// ************************************************

static LRESULT DoSysColorChange( HWND hWnd )
{
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	if( (dwStyle & WS_CHILD) == 0 )
	{ //顶层窗口
		//通知所有的子窗口，系统颜色改变
		//RETAILMSG( 1, ( "ds++:.\r\n" ) );
		EnumChildWindows( hWnd, EnumChildProc, (LPARAM)WM_SYSCOLORCHANGE );
		//RETAILMSG( 1, ( "ds--:.\r\n" ) );
	}
	return 0; 
}

// **************************************************
// 声明：LRESULT WINAPI Win_DefProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd - 窗口句柄
// 	IN Msg - 消息
// 	IN wParam - 第一个消息
// 	IN lParam - 第二个消息
// 返回值：
//	依赖于具体的消息
// 功能描述：
//	默认的窗口消息处理程序
// 引用: 
// ************************************************

LRESULT WINAPI Wnd_DefProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
//	RETAILMSG( 1, ( ( "def:arg0=%x,arg1=%x,arg2=%x,arg3=%x\r\n" ),  hWnd, Msg, wParam, lParam ) );
    switch( Msg )
    {
    case WM_SETCURSOR:
        if( LOWORD( lParam ) == 0xfffe && HIWORD( lParam ) == WM_LBUTTONDOWN )
        {
            //SetForegroundWindow( GetTopLevelParent( hWnd ) );
            //SetWindowPos( hWnd, HWND_TOP, 0, 0, 0, 0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
            //SetForegroundWindow( hWnd );
        }
        return 0;
    case WM_NCHITTEST:
        return DoNcHitTest( hWnd, wParam, lParam );
    case WM_NCLBUTTONDOWN:
        return DoNcLButtonDown( hWnd, (WORD)wParam, lParam );
    case WM_NCLBUTTONUP:
        return DoNcLButtonUp( hWnd, (WORD)wParam, lParam );
    case WM_MOUSEACTIVATE:
        return DoMouseActivate( hWnd, (HWND)wParam, LOWORD( lParam ), HIWORD( lParam ) );
    case WM_ACTIVATE:
        return DoActivate( hWnd, LOWORD( wParam ), HIWORD( wParam ) );
    case WM_NCACTIVATE:
        return DoNcActivate( hWnd, (BOOL)wParam );
    case WM_NCPAINT:
        return DoNcPaint( hWnd, wParam, lParam, FALSE );
    case WM_GETTEXT:
        return CopyText( hWnd, (LPSTR)lParam, (int)wParam );
	case WM_GETTEXTLENGTH:
		return GetTextLength( hWnd );
    case WM_SETTEXT:
        return SetText( hWnd, (LPCSTR)lParam );//SetWindowLong( hWnd, GWL_TEXT_PTR, (LPCSTR)lParam );//_Win_SetText( hWnd, (LPCSTR)lParam );
    case WM_SYSCOMMAND:
        return DoSysCommand( hWnd, wParam, lParam );
    case WM_PAINT:
        return DoPaint( hWnd );
    case WM_ERASEBKGND:
        return DoErasebkgnd( hWnd, (HDC)wParam );
    case WM_WINDOWPOSCHANGING:
        // send WM_GETMINMAXINFO
        break;
    case WM_WINDOWPOSCHANGED:
        // send WM_MOVE
        // send WM_SIZE
        break;
    case WM_STYLECHANGED:
	    //SetWindowPos( hWnd, NULL, 0, 0, 0, 0, 
		//		          SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER |SWP_NOACTIVATE | SWP_FRAMECHANGED );
		return 0;
    case WM_NCCALCSIZE:
        return DoNcCalcSize( hWnd, wParam, lParam );
    case WM_SHOWWINDOW:
        if( wParam )
            SetWindowPos( hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW );
        else
            SetWindowPos( hWnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_HIDEWINDOW );
        break;            
	case WM_SYSSETFOREGROUND:
		SetForegroundWindow( hWnd );
		break;
    case WM_VKEYTOITEM:   // use by list-box
    case WM_CHARTOITEM:
        return -1;
    case WM_ACTIVATEAPP:
		if( wParam == FALSE )
		{
		    DoNcActivate( GetActiveWindow(), wParam );
		}
		break;
    case WM_TIMER:
        return DoTimer( hWnd, (UINT)wParam, (TIMERPROC)lParam );
	case WM_SYSTIMER:
		return DoSysTimer( hWnd, (UINT)wParam, (TIMERPROC)lParam );
    case WM_CLOSE:
        DoClose( hWnd );
        break;
	case WM_SETICON:
		return DoSetIcon( hWnd, (UINT)wParam, (HICON)lParam );
	case WM_GETICON:
		return DoGetIcon( hWnd, (UINT)wParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hWnd );
	case WM_POWERBROADCAST:
		return TRUE;
    case WM_CREATE:
        return 0;
    case WM_DESTROY:
        return DoDestroy( hWnd );
    case WM_NCCREATE:
		return DoNcCreate( hWnd, lParam );
        //return TRUE;
    case WM_NCDESTROY:
        return DoNcDestroy( hWnd );
    }
    return 0;
}
