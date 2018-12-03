/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����Ĭ�ϵĴ�����Ϣ����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2005-01-08,  ȥ���� WM_STYLECHANGED ����Ч�������ڵĹ���, gwme ���� WM_NCPAINT ��Ϣ
    2003-06-13:  ���� WS_EX_CLIENTEDGE
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

static void FreeStr( LPSTR lpstr )
{
    if( lpstr )
        free( lpstr );
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
			// 2003-09-16, ɾ������, WM_NCPAINT's hrgn client����
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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


//  ����λͼˢ��
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
	{	// ��һ��, ��ʼ��
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
������static int DoScrollMove( HWND hWnd, LONG hitTest, int x, int y )
������
	IN hWnd - ���ھ��
	IN hitTest - ѡ�еĹ��������ͣ�Ϊ����ֵ��
		SC_HSCROLL - ˮƽ������
		SC_VSCROLL - ��ֱ������
	IN x - �ڹ����������xλ�ã���Ļ���꣩
	IN y - �ڹ����������yλ�ã���Ļ���꣩
����ֵ��
	���� 0
����������
	������ڹ���������ʱ��������ú������ú��������
    ������������Scrl_DoLBUTTONDOWN
����: 
	�ڲ�ʹ��
************************************************/

static int DoScrollMove( HWND hWnd, LONG hitTest, int x, int y )
{
    RECT rect;
    
    // ��hitTestת��ΪScrl_DoLBUTTONDOWN��ʶ���ֵ
    if( hitTest == SC_HSCROLL )
        hitTest = SB_HORZ;
    else
        hitTest = SB_VERT;
    // ��x,yת��Ϊ����ڴ��ڵ�����
    GetWindowRect( hWnd, &rect );
    x -= rect.left;
    y -= rect.top;
    //

    return (int)Scrl_DoLBUTTONDOWN( hWnd, (int)hitTest, x, y );
}


/**************************************************
������static int DoFrameMove( HWND hWnd, int x, int y )
������
	IN hWnd - ���ھ��
	IN x - �ڹ����������xλ�ã���Ļ���꣩
	IN y - �ڹ����������yλ�ã���Ļ���꣩
����ֵ��
	����0	
����������
	����WM_SYSCOMMAND - SC_MOVE��Ϣ
	ʵ�ִ��ڵ��ƶ�
����: 
	�ڲ�ʹ��
************************************************/
	                        
static int DoFrameMove( HWND hWnd, int x, int y )
{
    HWND hParent;
    DWORD dwStyle;
    POINT pt;
    RECT rect;

    dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    // �õ��ô��ڵĴ��ھ��Σ���Ļ���꣩
    GetWindowRect( hWnd, &rect );
    if( dwStyle & WS_CHILD )
    {	//������Ӵ��ڣ����ô��ھ�������Ļ����ת��Ϊ�丸��������
        hParent = GetParent( hWnd );
        ScreenToClient( hParent, (LPPOINT)&rect );
        ScreenToClient( hParent, ( (LPPOINT)&rect+1 ) );
    }
    else
        hParent = 0;

    pt.x = x;
    pt.y = y;
	
	
    // �����������϶��������
    if( TrackWindowRect( hWnd, pt, hParent, &rect ) )
    {   // ����ͷţ�̧�ʣ�����hWnd�Ƶ��µ�λ��
			
        SetWindowPos( hWnd, 0, rect.left,  rect.top, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE );
        // �������´�������
        UpdateWindow( hWnd );
    }
    return 0;
}


/**************************************************
������static BOOL GetInterFrame( LPRECT lprect, DWORD dwMainStyle )
������
	IN/OUT lprect - ���봰�ھ������꣬���ø�ָ����ճ��߿�֮����ھ�������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�ɴ��ھ��εõ����߿�֮����ھ���
����: 
************************************************/

static BOOL GetInterFrame( LPRECT lprect, DWORD dwMainStyle )
{
    SIZE size;
    //  �õ����߿�֮����ھ��εĿ�Ⱥ͸߶�
    if( _GetFrameSize( &size, dwMainStyle ) )
    {   // ת��Ϊ��Ļ����
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
������static BOOL GetCaptionSysBoxRect( LPRECT lprect, DWORD dwMainStyle )
������
	IN/OUT lprect - ������߿�֮����ھ��Σ����ø�ָ����ձ������ϵ�
					ϵͳ�����������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�õ��������ϵ�ϵͳ�����������
����: 
************************************************/

static BOOL GetCaptionSysBoxRect( LPRECT lprect, DWORD dwMainStyle )
{
	lprect->right = lprect->left + GetSystemMetrics( SM_CXSIZE );
	lprect->bottom = lprect->top + GetSystemMetrics( SM_CYSIZE );
    return TRUE;
}

/**************************************************
������static BOOL GetCaptionTextBoxRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle )
������
	IN/OUT lprect - ������߿�֮����ھ��Σ����ø�ָ����ձ������ϵ�
					�ı������������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�õ��������ϵ��ı������������
����: 
************************************************/

static BOOL GetCaptionTextBoxRect( LPRECT lprect, DWORD dwMainStyle, DWORD dwExStyle )
{
	int cx = GetSystemMetrics( SM_CXSIZE );
	//�����ϵͳ����ȥ����
	if( dwMainStyle & WS_SYSMENU )
	    lprect->left += cx;
	//����йرշ���ȥ����
    if( dwExStyle & WS_EX_CLOSEBOX )
	{
		lprect->right -= cx;
	}
	//�����ȷ������ȥ����	
    if( dwExStyle & WS_EX_OKBOX )
	{
		lprect->right -= cx;
	}
	//����а�������ȥ����		
    if( dwExStyle & WS_EX_HELPBOX )
	{
		lprect->right -= cx;
	}
	return TRUE;
}

/**************************************************
������static BOOL GetCaptionRect( LPRECT lprect, DWORD dwMainStyle )
������
	IN/OUT lprect - ������߿�֮����ھ��Σ����ø�ָ����ձ����������������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�õ������������������
����: 
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
������static BOOL GetCaptionHelpBoxRect( LPRECT lprect, DWORD dwExStyle )
������
	IN/OUT lprect - ������߿�֮����ھ��Σ����ø�ָ����ձ������İ��������������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�õ����������������������
����: 
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
������static BOOL GetCaptionOkBoxRect( LPRECT lprect, DWORD dwExStyle )
������
	IN/OUT lprect - ������߿�֮����ھ��Σ����ø�ָ����ձ�������ȷ�������������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�õ�������ȷ�������������
����: 
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
������static BOOL GetCaptionCloseBoxRect( LPRECT lprect, DWORD dwExStyle )
������
	IN/OUT lprect - ������߿�֮����ھ��Σ����ø�ָ����ձ������Ĺرշ����������
	IN dwMainStyle - ���ڷ��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE	
����������
	�õ��������رշ����������
����: 
************************************************/

static BOOL GetCaptionCloseBoxRect( LPRECT lprect, DWORD dwExStyle )
{
    lprect->bottom = lprect->top + GetSystemMetrics( SM_CYSIZE );

    lprect->left = lprect->right - GetSystemMetrics( SM_CXSIZE );
    return TRUE;
}

/**************************************************
������static LRESULT DoPaint( HWND hWnd )
������
	IN hWnd - ���ھ��
����ֵ��
	����ɹ�������0��
����������
	����WM_PAINT��Ϣ
����: 
************************************************/

static LRESULT DoPaint( HWND hWnd )
{
    PAINTSTRUCT ps;

    BeginPaint( hWnd, &ps );
    EndPaint( hWnd, &ps );
    return 0;
}

/**************************************************
������static LRESULT DoErasebkgnd( HWND hWnd, HDC hdc )
������
	IN hWnd - ���ھ��
	IN hdc - ��ͼDC
����ֵ��
	��������˱���������1�����򷵻�0
����������
	����WM_ERASEBACKGROUND��Ϣ
����: 
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
������static LRESULT DoErasebkgnd( HWND hWnd )
������
	IN hWnd - ���ھ��
����ֵ��
	����0
����������
	����WM_DESTROY��Ϣ
����: 
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
          //_SetRoundWindowFrame(hWnd);  //����һ��Բ�Ǳ߿�

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
������static LRESULT DoNcHitTest( HWND hWnd, WPARAM wParam, LPARAM lParam )
������
	IN hWnd - ���ھ��
    wParam - ��һ������
    lParam - �ڶ�������

����ֵ��
	���ص�ǰ����ڴ��ڵ��ĸ�����Ϊ����ֵ��
		HTCLIENT - �ͻ���
		HTTOPLEFT - �󶥱߿�
		HTBOTTOMLEFT - ��ױ߿�
		HTLEFT - ��߿�
		HTTOPRIGHT - �Ҷ��߿�
		HTTOP - ���߿�
		HTBOTTOMRIGHT - �ҵױ߿�
		HTBOTTOM - �ױ߿�
		HTRIGHT - �ұ߿�
		HTERROR - ����û���κ�����
		HTBORDER - �߿򣨴���ֻ�б߿�û���������
		HTVSCROLL - ��ֱ������
		HTHSCROLL - ˮƽ������
		HTGROWBOX - ���½ǵ����ſ�
		HTCLOSEBOX - �������ϵĹرշ���
		HTOKBOX - �������ϵ�ȷ�Ϸ���
		HTHELPBOX - �������ϵİ�������
		HTSYSMENU - �������ϵ�ϵͳ����
		HTCAPTION - ������
		HTCLIENTEDGE - �ͻ����߿�

����������
	����WM_NCHITTEST��Ϣ

����: 
************************************************/

static LRESULT DoNcHitTest( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    POINT pt, ptClient;
    RECT rect, rectInter;
    RECT rClient;
    DWORD dwMainStyle, dwExStyle;

    int cx, cy;

    pt.x = LOWORD(lParam); // ��Ļx����
    pt.y = HIWORD(lParam); // ��Ļy����
	ptClient = pt;

    GetClientRect( hWnd, &rClient );
    ScreenToClient( hWnd, &ptClient ); //ת��Ϊ�ͻ�����
    if( PtInRect( &rClient, ptClient ) )	//�ڿͻ�����
        return HTCLIENT;

    GetWindowRect( hWnd, &rect ); //�õ��ô��ڵ���Ļ����

    dwMainStyle = GetWindowLong( hWnd, GWL_MAINSTYLE );
    dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE );

    rectInter = rect;
    if( GetInterFrame( &rectInter, dwMainStyle ) )	//�õ����߿�����ھ���
    {
        if( !PtInRect( &rectInter, pt ) )	//���ھ��Σ�
        {  //���ڣ���Ȼ�ڱ߿��ϡ� pt is within window but not within rectInter
            if( (dwMainStyle & WS_THICKFRAME) == WS_THICKFRAME )	//�ֱ߿���
            {	// �ǣ����´����������һ���߿��ϡ�test which border or corner
            	//cx, cyΪ�ϲ������ű߿�ĸ߶ȺͿ��
                cy = GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CYFRAME );                
                cx = GetSystemMetrics( SM_CYCAPTION ) + GetSystemMetrics( SM_CXFRAME );
                if( IN_RANGE( pt.x, rect.left, rectInter.left ) )	//��߿�
                {  // �ǣ�x ����߿� left border
                    if( IN_RANGE( pt.y, rect.top, rect.top + cy ) )
                        return HTTOPLEFT;	//������߿�Ŀ����ű߿�
                    if( IN_RANGE( pt.y, rect.bottom - cy, rect.bottom ) )
                        return HTBOTTOMLEFT;	//������߿�Ŀ����ű߿�
                    else
                        return HTLEFT;	//������߿�
                }
                else if( IN_RANGE( pt.y, rect.top, rectInter.top ) )	//�ϱ߿�
                {  // �ǣ�y ���ϱ߿� top border
                    if( IN_RANGE( pt.x, rect.left, rect.left + cx ) )
                        return HTTOPLEFT;	//������߿�Ŀ����ű߿�
                    if( IN_RANGE( pt.x, rect.right - cx, rect.right ) )
                        return HTTOPRIGHT;	//�����ұ߿�Ŀ����ű߿�
                    else
                        return HTTOP;	//�����ϱ߿�
                }
                else if( IN_RANGE( pt.x, rectInter.right, rect.right ) )	//�ұ߿�
                {   // right border
                    if( IN_RANGE( pt.y, rect.top, rect.top + cy ) )
                        return HTTOPRIGHT;
                    if( IN_RANGE( pt.y, rect.bottom - cy, rect.bottom ) )
                        return HTBOTTOMRIGHT;
                    else
                        return HTRIGHT;
                }
                else if( IN_RANGE( pt.y, rectInter.bottom, rect.bottom ) )	//�±߿�
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
                    ASSERT( 0 ); // ���󣡣�����Ӧ�÷����� error
                    return HTERROR;
                }
            }
            //�Ǵֱ߿�
            return HTBORDER;
        }
    }
	// ���ڱ߿��ϡ����´�������Ƿ�����������test in scroll bar ?
    if( dwMainStyle & (WS_HSCROLL | WS_VSCROLL) )
    {	//�����й�����
        // save rect
    	if( dwMainStyle & WS_VSCROLL )
    	{
            rectInter = rect;
            // �õ���ֱ�������ľ���
            Scrl_GetBarRect( &rectInter, dwMainStyle, dwExStyle, SB_VERT );
            if( PtInRect( &rectInter, pt ) )
                return HTVSCROLL; // �ڴ�ֱ��������
    	}
    	if( dwMainStyle & WS_HSCROLL )
    	{
            rectInter = rect;
            // �õ�ˮƽ�������ľ���
            Scrl_GetBarRect( &rectInter, dwMainStyle, dwExStyle, SB_HORZ );
            if( PtInRect( &rectInter, pt ) )
                return HTHSCROLL;	//��ˮƽ��������
    	}
    	if( (dwMainStyle & (WS_HSCROLL | WS_VSCROLL)) == (WS_HSCROLL | WS_VSCROLL) )
    	{
            rectInter = rect;
            // �õ����ſ�ĵľ���            
            Scrl_GetBarRect( &rectInter, dwMainStyle, dwExStyle, SB_CORNER );
            if( PtInRect( &rectInter, pt ) )
                return HTGROWBOX;	//�����ſ���
    	}
    }

	// test in caption or icon or syscommand button
    if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
		 ( dwExStyle & WS_EX_TITLE ) )
    {	//�����б�����
        rectInter = rect;
        //�õ��������ľ���
        GetCaptionRect( &rect, dwMainStyle );
        if( PtInRect( &rect, pt ) )
        {	//�ڱ�������
            if( dwExStyle & WS_EX_CLOSEBOX )
            {	//���������йرտ�
                GetCaptionCloseBoxRect( &rect, dwExStyle );
                if( PtInRect( &rect, pt ) )
                    return HTCLOSEBOX;
            }
            
            if( dwExStyle & WS_EX_OKBOX )
            {	//����������ȷ�Ͽ�
                rect = rectInter;
                GetCaptionOkBoxRect( &rect, dwExStyle );
                if( PtInRect( &rect, pt ) )
                    return HTOKBOX;
            }
            
            if( dwExStyle & WS_EX_HELPBOX )
            {	//���������а�����
                rect = rectInter;
                GetCaptionHelpBoxRect( &rect, dwExStyle );
                if( PtInRect( &rect, pt ) )
                    return HTHELPBOX;
            }

			if( dwMainStyle & WS_SYSMENU )
			{	//����������ϵͳ��
				rect = rectInter;
				GetCaptionSysBoxRect( &rect, dwMainStyle );
				if( PtInRect( &rect, pt ) )
					return HTSYSMENU;
			}
			//�ڱ����������ֿ���
            return HTCAPTION;
        }
    }

    if( dwExStyle & WS_EX_CLIENTEDGE )
	{   //�пͻ��߿򣬿ͻ��߿�ֻ��1��֣����ｫ�ͻ���������һ���
		InflateRect( &rClient, 1, 1 );
		//�ͻ��߿����������������ˣ�����й�������Ӧ��Ӧ�м�ͻ��߿�Ĵ�С
        if( dwMainStyle & WS_HSCROLL )
			rClient.bottom += GetSystemMetrics( SM_CYHSCROLL );			
        if( dwMainStyle & WS_VSCROLL )
			rClient.right += GetSystemMetrics( SM_CXVSCROLL );
		if( PtInRect( &rClient, ptClient ) )
			return HTCLIENTEDGE;	//�ڿͻ��߿���
	}

    ASSERT( 0 );	//��Ӧ��ִ�е�����
    return HTERROR;
}

// **************************************************
// ������static void DrawScrollBar( HWND hWnd, HDC hdc, DWORD dwMainStyle )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
//	IN dwMainStyle - ���ڷ��
// ����ֵ��
//	��
// ����������
//	���ƹ�����
// ����: 
// ************************************************

static void DrawScrollBar( HWND hWnd, HDC hdc, DWORD dwMainStyle )
{
    if( dwMainStyle & WS_VSCROLL )
    {	// ����ֱ������
        Scrl_RedrawScrollBar( hWnd, hdc, SB_VERT );
    }
    if( dwMainStyle & WS_HSCROLL )
    {	// ��ˮƽ������
        Scrl_RedrawScrollBar( hWnd, hdc, SB_HORZ );
    }
    if( (dwMainStyle & WS_HSCROLL) && (dwMainStyle & WS_VSCROLL) )
    {	//�����½ǵ����ſ�
        Scrl_RedrawScrollBar( hWnd, hdc, SB_CORNER );
    }
}
/*   the version is show text mask of close and help and ok 
// **************************************************
// ������static void ShowCaption( HWND hWnd, HDC hdc, DWORD dwMainStyle, BOOL fActive )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
//	IN dwMainStyle - ���ڷ��
//	IN fActive - �������Ƿ��ǻ��
// ����ֵ��
//	��
// ����������
//	�������ϵ�����������
// ����: 
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
// ������static void ShowCaption( HWND hWnd, HDC hdc, DWORD dwMainStyle, BOOL fActive )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
//	IN dwMainStyle - ���ڷ��
//	IN fActive - �������Ƿ��ǻ��
// ����ֵ��
//	��
// ����������
//	�������ϵ�����������
// ����: 
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
	  )	//�б�������
    {	//��
        //int yIconHeight, xIconWidth;

		//rtCaption = lpws->rectWindow;
		GetWindowRect( hWnd, &rtCaption );
        OffsetRect( &rtCaption, -rtCaption.left, -rtCaption.top );
        //�õ�����������
		GetCaptionRect( &rtCaption, dwMainStyle );
		//������������
		{
			RECT rc = rtCaption;
			rc.top -= GetSystemMetrics( SM_CYFRAME );  // XP like 
			rc.left -= GetSystemMetrics( SM_CXFRAME );  // XP like 
			rc.right += GetSystemMetrics( SM_CXFRAME );  // XP like 
			DrawCaption( hWnd, hdc, &rc, fActive ? DC_ACTIVE : 0 );
		}

		//�õ�ϵͳ��ͼ��Ĵ�С
		//2004-07-08, modify
        //yIconHeight = GetSystemMetrics(SM_CYSMICON);
		//xIconWidth = GetSystemMetrics(SM_CXSMICON);
        //yIconHeight = GetSystemMetrics(SM_CYSIZE);
		//xIconWidth = GetSystemMetrics(SM_CXSIZE);
		//


		// �����ϵͳ��,��֮��now , draw sysmenu icon if possible
        if( dwMainStyle & WS_SYSMENU )
        {
			HICON hIcon;
			ICONINFO iconInfo;

            hIcon = (HICON)GetWindowLong( hWnd, GWL_HICONSM );//lpws->hSmallIcon;//Сͼ��
			if( hIcon == NULL )
			{	//���û���û��ṩ��Сͼ�꣬����ϵͳĬ�ϵ�
				if( fActive )
				{
					hIcon = GetStockObject( SYS_STOCK_LOGO );//hicoSysMenu;//ϵͳ�ͼ��
				}
				else
				{
					hIcon = GetStockObject( SYS_STOCK_LOGO_GRAY );//hicoGraySysMenu;//ϵͳ�ǻͼ��
				}
			}
			GetIconInfo( hIcon, &iconInfo );
			GetObject( iconInfo.hbmColor, sizeof(bitmap), &bitmap );
			
		    rt = rtCaption;
		    //�õ�ϵͳ���С
			GetCaptionSysBoxRect( &rt, dwMainStyle );
			//����
			//rt.top += ( (rt.bottom - rt.top) - yIconHeight ) >> 1;// align to middle
			//rt.left += ( (rt.right - rt.left) - xIconWidth ) >> 1;// align to middle
			rt.top += ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1;// align to middle
			rt.left += ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1;// align to middle
			//��ͼ��
            DrawIcon( hdc, rt.left, rt.top, hIcon );
        }

        //dwExStyle = lpws->dwExStyle;//GetWindowLong( hWnd, GWL_EXSTYLE );

		// ���������ϵ��ı�
		{
			TCHAR szText[128];
			int len;
			len = GetWindowText( hWnd, szText, sizeof(szText)-sizeof(TCHAR) );
			// now , draw caption text if possible
			if( len )//lpws->lpWinText )
			{	//��Ϊ�Ѿ�����˱������������ﻭ�ı���͸��ģʽ
				DWORD oldMode = SetBkMode( hdc, TRANSPARENT );	
				
				rt = rtCaption;
				//�õ��ı�����
				GetCaptionTextBoxRect( &rt, dwMainStyle, dwExStyle );
				//��֮
				DrawText( hdc, szText, len, &rt,  DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS ); 
				SetBkMode( hdc, oldMode );
			}
		}

        // now draw option icon if possible
		// ���������ϵĹ��ܿ�
        if( dwExStyle & (WS_EX_CLOSEBOX | WS_EX_OKBOX | WS_EX_HELPBOX) )
        {            
			int xBtWidth;
			HANDLE hbmpSave, hbmp;
			HANDLE hMemDC = CreateCompatibleDC( hdc );
			int x, y;

            rt = rtCaption;
            //�õ��رտ����
			GetCaptionCloseBoxRect( &rt, dwExStyle );
			xBtWidth = rt.right - rt.left;
			//����
			//rt.top += ( (rt.bottom - rt.top) - yIconHeight ) >> 1;// align to middle
			//rt.left += ( (rt.right - rt.left) - xIconWidth ) >> 1;// align to middle
			hbmp = GetStockObject(SYS_STOCK_CAPTION_CLOSE);
			GetObject( hbmp, sizeof(bitmap), &bitmap );

			if( dwExStyle & WS_EX_CLOSEBOX )
            {	//���رտ�
				if( fActive )
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_CLOSE);//hbmpClose;//�û��λͼ
				}
				else
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_CLOSE_GRAY);//hbmpGrayClose;//�÷ǻ��λͼ
				}
				//GetObject( hbmp, sizeof(bitmap), &bitmap );
				y = rt.top + ( ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1 );
				x = rt.left + ( ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1 );// align to middle

	            hbmpSave = SelectObject( hMemDC, hbmp );
				//��֮
				//BitBlt( hdc, rt.left, rt.top, xIconWidth, yIconHeight, hMemDC, 0, 0, SRCCOPY );
				BitBlt( hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );

	            SelectObject( hMemDC, hbmpSave );

				rt.left -= xBtWidth;//Ϊ����Ļ���׼������
                rt.right -= xBtWidth;//Ϊ����Ļ���׼������
            }
            if( dwExStyle & WS_EX_OKBOX )
            {	//���رտ�
				if( fActive )
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_OK);//hbmpOk;//�û��λͼ
				}
				else
				{
					hbmp = GetStockObject(SYS_STOCK_CAPTION_OK_GRAY);//hbmpGrayOk;//�÷ǻ��λͼ
				}
				//GetObject( hbmp, sizeof(bitmap), &bitmap );
				y = rt.top + ( ( (rt.bottom - rt.top) - bitmap.bmHeight ) >> 1 );
				x = rt.left + ( ( (rt.right - rt.left) - bitmap.bmWidth ) >> 1 );// align to middle

	            hbmpSave = SelectObject( hMemDC, hbmp );
	            //��֮
				//BitBlt( hdc, rt.left, rt.top, xIconWidth, yIconHeight, hMemDC, 0, 0, SRCCOPY );
				BitBlt( hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );
                SelectObject( hMemDC, hbmpSave );
				rt.left -= xBtWidth;//Ϊ����Ļ���׼������
                rt.right -= xBtWidth;//Ϊ����Ļ���׼������
            }
            if( dwExStyle & WS_EX_HELPBOX )
            {	//��������
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
// ������static void DrawFrame( HWND hWnd, 
//							  HDC hdc, 
//							  DWORD dwMainStyle,
//							  DWORD dwExStyle,
//							  BOOL bEraseBk )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
//	IN dwMainStyle - ���ڷ��
//	IN dwExStyle - ������չ���
//	IN bEraseBk - �Ƿ��������
// ����ֵ��
//	��
// ����������
//	�����ڱ߿�
// ����: 
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
	{	//�������
	    FillRect( hdc, &rect, (HBRUSH)(COLOR_WINDOW+1) );
	}

    hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    // draw frame
    if( dwMainStyle & WS_THICKFRAME )
    {	//���ֱ߿�
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
    {	//���Ի�����ı߿�

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
    {	//�����߿�
		s = 1;
        Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
		InflateRect( &rect, -s, -s );
    }
	if( dwExStyle & WS_EX_CLIENTEDGE )
	{	//���ͻ��߿�
        if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
		    (dwExStyle & WS_EX_TITLE) )
			rect.top += GetSystemMetrics( SM_CYCAPTION );
		DrawEdge( hdc, &rect, BDR_SUNKENOUTER, BF_RECT );
	}

    SelectObject( hdc, hBrush );
}

// **************************************************
// ������static LRESULT DoNcActivate( HWND hWnd, BOOL fActive )
// ������
//	IN hWnd - ���ھ��
//	IN fActive - �Ƿ񴰿��ǻ��
// ����ֵ��
//	TRUE
// ����������
//	����WM_NCACTIVATE��Ϣ
// ����: 
// ************************************************

static LRESULT DoNcActivate( HWND hWnd, BOOL fActive )
{
    HDC hdc;
	DWORD dwStyle, dwExStyle;

    
    dwStyle = (DWORD)GetWindowLong( hWnd, GWL_MAINSTYLE );
	dwExStyle = (DWORD)GetWindowLong( hWnd, GWL_EXSTYLE );
	hdc = GetWindowDC( hWnd );    
    ShowCaption( hWnd, hdc, dwStyle, fActive );

    // ���߿� draw frame
    DrawFrame( hWnd, hdc, dwStyle, dwExStyle, FALSE, fActive );

    ReleaseDC( hWnd, hdc );
    return TRUE;
}

// **************************************************
// ������static int DoSysCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - ��һ������
//	IN lParam - �ڶ�������
// ����ֵ��
//	0
// ����������
//	����WM_SYSCOMMAND��Ϣ
// ����: 
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
// ������static LRESULT DoNcPaint( HWND hWnd, 
// 								WPARAM wParam,
// 								LPARAM lParam, 
// 								BOOL bEraseBk )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - ��һ������
//	IN lParam - �ڶ�������
//	IN bEraseBk - �Ƿ��������
// ����ֵ��
//	1
// ����������
//	���� WM_NCPAINT��Ϣ
// ����: 
// ************************************************

static LRESULT DoNcPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bEraseBk )
{
    HDC hdc;
    DWORD dwMainStyle;
	DWORD dwExStyle;
	BOOL bActive;

    if( wParam == 1 )
        hdc = GetDCEx(hWnd, 0, DCX_WINDOW|DCX_CLIPSIBLINGS);
    else	// wParam��һ���ü�������Ҫ��dc���������AND����
        hdc = GetDCEx(hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN|DCX_CLIPSIBLINGS);
    
    dwMainStyle = GetWindowLong( hWnd, GWL_MAINSTYLE );
	dwExStyle = GetWindowLong( hWnd, GWL_SUBSTYLE );
    // �������� draw text or icon or syscommand button
	bActive = ( GetActiveWindow() == hWnd );
    //if( (dwMainStyle & WS_CAPTION) == WS_CAPTION ||
		//(dwExStyle & WS_EX_TITLE) )
    ShowCaption( hWnd, hdc, dwMainStyle, bActive );

    // ���߿� draw frame
    DrawFrame( hWnd, hdc, dwMainStyle, dwExStyle, bEraseBk, bActive );
    // �������� draw scrollbar
    if( (dwMainStyle & (WS_HSCROLL | WS_VSCROLL) ) )
        DrawScrollBar( hWnd, hdc, dwMainStyle );
    ReleaseDC( hWnd, hdc );
    return 1;
}

// **************************************************
// ������LRESULT CALLBACK WinDef_NCPaint( HWND hWnd, 
// 									   WPARAM wParam,
// 									   LPARAM lParam,
// 									   BOOL bEraseBk )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - ��һ������
//	IN lParam - �ڶ�������
//	IN bEraseBk - �Ƿ��������
// ����ֵ��
// ����������
//	�Էǿͻ������л��� 
// ����: 
// ************************************************

// call by win.c
LRESULT CALLBACK WinDef_NCPaint( HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bEraseBk )
{
	return DoNcPaint( hWnd, wParam, lParam, bEraseBk );
}

// **************************************************
// ������static LRESULT DoNcCalcSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - ��һ������
//	IN/OUT lParam - �ڶ���������ΪRECT�ṹָ��,���뵱ǰ���ڴ�С�����ؿͻ�����С��
// ����ֵ��
//	0
// ����������
//	���� WM_NCCALCSIZE ��Ϣ- ����ͻ����Ĵ�С
// ����: 
// ************************************************

static LRESULT DoNcCalcSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
    LPRECT lprect;
    DWORD dwMainStyle;	
    DWORD dwExStyle;

    if( wParam == 0 )
    {
        lprect = (LPRECT)lParam;
        // �õ����
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
// ������static LRESULT DoActivate( HWND hWnd, BOOL fActivate, BOOL fMinimize )
// ������
// 	IN hWnd - ���ھ��
// 	IN fActivate - �Ƿ�
// 	IN fMinimize - �Ƿ���С������֧�֣�
// ����ֵ��
// 	0
// ����������
//	���� WM_ACTIVATE ��Ϣ
// ����: 
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
// ������static LRESULT DoMouseActivate( HWND hWnd, 
// 									  HWND hTopLevel,
// 									  WORD wHitValue,
//  									 WORD wMouse )
// ������
// 	IN hWnd - ���ھ��
// 	IN hTopLevel - ��������
// 	IN wHitValue - ��ǰ�����еĴ���λ��
// 	IN wMouse - ���״̬

// ����ֵ��
// ����������
//	���� WM_MOUSEACTIVATE ��Ϣ
// ����: 
// ************************************************

static LRESULT DoMouseActivate( HWND hWnd, HWND hTopLevel, WORD wHitValue, WORD wMouse )
{
    HWND hwndTemp;

    if( hTopLevel == hWnd &&
        wMouse == WM_LBUTTONDOWN )
        return MA_ACTIVATE; // ����Լ����Ƕ�������, ������
    else
    {
        hwndTemp = GetParent( hWnd ); // �ô����и�������
        if( hwndTemp )	// �У���������ڷ�����Ϣ
            return SendMessage( hwndTemp, WM_MOUSEACTIVATE, (WPARAM)hTopLevel, MAKELONG( wHitValue, wMouse  ) );
    }
    return MA_ACTIVATE;
}

// **************************************************
// ������static LRESULT DoNcLButtonDown( HWND hWnd, WORD wHitValue, LPARAM lParam )
// ������
// 	IN hWnd - ���ھ��
// 	IN wHitValue - ��ǰ�����еĴ���λ��
//	 IN lParam - x, y ����
// ����ֵ��
//	0
// ����������
//	���� WM_NCLBUTTONDOWN ��Ϣ
// ����: 
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
// ������static LRESULT DoNcLButtonUp( HWND hWnd, WORD wHitValue, LPARAM lParam )
// ������
// 	IN hWnd - ���ھ��
// 	IN wHitValue - ��ǰ�����еĴ���λ��
//	 IN lParam - x, y ����
// ����ֵ��
//	0
// ����������
//	���� WM_NCLBUTTONUP ��Ϣ
// ����: 
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
// ������static LRESULT DoClose( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	0
// ����������
//	���� WM_CLOSE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoClose( HWND hWnd )
{
    DestroyWindow( hWnd );
    return 0;
}

// **************************************************
// ������static LRESULT DoTimer( HWND hWnd, UINT uID, TIMERPROC lpTimerProc )
// ������
//    IN hWnd - ���ھ��
//	IN uID - ��ʱ��ID
//	IN lpTimerProc - ��ʱ���ص�����
// ����ֵ��
//	0
// ����������
//	���� WM_TIMER ��Ϣ
// ����: 
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
			// ׼���ص�����
			cd.hProcess = lpws->lpClass->hOwnerProcess;
			cd.lpfn = (FARPROC)lpTimerProc;
			cd.dwArg0 = (DWORD)hWnd;
			// �ص�
			Sys_ImplementCallBack4( &cd, WM_TIMER, uID, GetTickCount() );
		}
		*/
		lpTimerProc( hWnd, WM_TIMER, uID, GetTickCount() );

	}
    return 0;
}

// **************************************************
// ������static LRESULT DoSysTimer( HWND hWnd, UINT uID, TIMERPROC lpTimerProc )
// ������
//    IN hWnd - ���ھ��
//	IN uID - ��ʱ��ID
//	IN lpTimerProc - ��ʱ���ص�����

// ����ֵ��
//	0
// ����������
//	���� WM_SYSTIMER ��Ϣ
// ����: 
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
			// ׼���ص�����			
			cd.hProcess = lpws->lpClass->hOwnerProcess;
			cd.lpfn = (FARPROC)lpTimerProc;
			cd.dwArg0 = (DWORD)hWnd;
			// �ص�	
			Sys_ImplementCallBack4( &cd, WM_SYSTIMER, uID, GetTickCount() );
		}
		*/
		lpTimerProc( hWnd, WM_SYSTIMER, uID, GetTickCount() );
	}
    return 0;
}

// **************************************************
// ������static LRESULT DoSetIcon( HWND hWnd, UINT fIconType, HICON hNewIcon )
// ������
//    IN hWnd - ���ھ��
//	IN fIconType - ͼ������
//		ICON_SMALL - Сͼ��
//	IN hNewIcon - ͼ����
// ����ֵ��
//	�ϵ�ͼ����
// ����������
//	���� WM_SETICON ���ô��ڵ�ϵͳͼ��
// ����: 
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
// ������static LRESULT DoGetIcon( HWND hWnd, UINT fIconType )
// ������
//    IN hWnd - ���ھ��
//	IN fIconType - ͼ������
//		ICON_SMALL - Сͼ��
// ����ֵ��
//	ͼ����
// ����������
//	���� WM_GETICON ��Ϣ���õ���ǰ��ͼ����
// ����: 
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
// ������static BOOL CALLBACK EnumChildProc( HWND hwnd, LPARAM lParam )
// ������
// 	IN hWnd - ���ھ��
//	IN lParam - LPARAM ����,�����Ǵ�����Ϣ����
// ����ֵ��
//	���� TRUE,����ö��; FALSE,ֹͣö��
// ����������
//	ö���Ӵ���
// ����: 
// ************************************************

static BOOL CALLBACK EnumChildProc( HWND hWnd, LPARAM lParam )
{
	//RETAILMSG( 1, ( "ec++:0x%x.\r\n", lParam ) );
	SendMessage( hWnd, (UINT)lParam, 0, 0 );
	//RETAILMSG( 1, ( "ec--.\r\n" ) );
	return TRUE;
}

// **************************************************
// ������static LRESULT DoSysColorChange( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	���� 0
// ����������
//	����ϵͳ��ɫ�ı�
// ����: 
// ************************************************

static LRESULT DoSysColorChange( HWND hWnd )
{
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	if( (dwStyle & WS_CHILD) == 0 )
	{ //���㴰��
		//֪ͨ���е��Ӵ��ڣ�ϵͳ��ɫ�ı�
		//RETAILMSG( 1, ( "ds++:.\r\n" ) );
		EnumChildWindows( hWnd, EnumChildProc, (LPARAM)WM_SYSCOLORCHANGE );
		//RETAILMSG( 1, ( "ds--:.\r\n" ) );
	}
	return 0; 
}

// **************************************************
// ������LRESULT WINAPI Win_DefProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hWnd - ���ھ��
// 	IN Msg - ��Ϣ
// 	IN wParam - ��һ����Ϣ
// 	IN lParam - �ڶ�����Ϣ
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	Ĭ�ϵĴ�����Ϣ�������
// ����: 
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
