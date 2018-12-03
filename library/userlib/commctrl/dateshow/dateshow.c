/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：日期设置类
版本号：  1.0.0
开发时期：2003-06-06
作者：    肖远钢
******************************************************/

#include <eframe.h>
#include <ettime.h>
#include <ecomctrl.h>
#include <edateshow.h>

#ifdef EML_DOS
#include "Dos.h"
#include <stdio.h>
#endif

//---------------------------------------------------
#define YEAR_MIN		0
#define YEAR_MAX		9999

#define	W_APP			105						//AP 最小宽度
#define	H_APP			(20)					//AP 最小高度

//ToDown 按钮
#define	CX_TODOWN			16
#define	CY_TODOWN			16

//日期选择控件
#define	CX_DATE	138
#define	CY_DATE	135


#define	WM_USERSELDATE		(WM_USER+9731)

//---------------------------------------------------
#define IDC_DATESHOW				103

#define IDC_DATESEL					104

//---------------------------------------------------
#define	LoadIcon_xy( hInst, uID, x, y )	\
	LoadImage( (hInst), MAKEINTRESOURCE(uID), IMAGE_ICON, x, y, 0 )

#define	RECT_CX( prt )				((prt)->right - (prt)->left)
#define	RECT_CY( prt )				((prt)->bottom - (prt)->top)

//---------------------------------------------------
typedef	struct	_DATESHOWINFO
{
	HWND		hWndDateSel;
	HICON		hIconDown[3];	//down, down-down, down-gray
	HWND		hWndUserSel;		//用户点击控件之外区域时 发出消息
	UINT		uMsgUserSel;

	BOOL		fIsModified;
	SYSTEMTIME	stDateShow;
	WORD		wCaleYearMin;		//年 Min
	WORD		wCaleYearMax;		//年 Max

	//
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_Disable;
	COLORREF cl_DisableBk;    // 无效文本的前景与背景色

	//save for datesel
	DWORD	dwDateStyle;
	SHORT	cxDate;
	SHORT	cyDate;
	WORD	wFlagFont;			//选择 显示日期的字体
	WORD	wXXX;

} DATESHOWINFO, *LPDATESHOWINFO;


//---------------------------------------------------
static	LRESULT	CALLBACK	WndProc_DateShow( HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static	void	DoPaint( HWND hWnd, HDC hdc, const RECT* lprtPaint );
static	BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem );
static	LRESULT	DoCommand( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	void	DoShowDateSel( HWND hWnd, LPDATESHOWINFO lpInfo );
static	void	DoUserSelDate( HWND hWnd, HWND hWndDate, BOOL fSelect );
static	LRESULT	DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoDestroy( HWND hWnd );

static	void	DoEnable( HWND hWnd, BOOL fEnable );
static	LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs );
static	LRESULT	DoGetDateTime( HWND hWnd, WPARAM wParam, LPARAM lParam );

static	LRESULT	DoSetDateTime( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoSetRange( HWND hWnd, WPARAM wParam, LPARAM lParam );

static	LRESULT	DoSetDateFont( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoSetDateStyle( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoSetDateSize( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoGetModify( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoSetModify( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoSetUserNotify( HWND hWnd, WPARAM wParam, LPARAM lParam );

//---------------------------------------------------
ATOM RegisterDateShowClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = WndProc_DateShow;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classDATESHOW;
	
    return (BOOL)(RegisterClass( &wc ));
}

//---------------------------------------------------
LRESULT	CALLBACK	WndProc_DateShow( HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	//
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc, &ps.rcPaint);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DRAWITEM:
		return DoDrawItem( hWnd, (UINT)wParam, (DRAWITEMSTRUCT*) lParam );

    case WM_COMMAND:
		return DoCommand(hWnd,wParam,lParam);
	case WM_USERSELDATE:
		DoUserSelDate( hWnd, (HWND)wParam, (BOOL)lParam );
		break;
	//only dateshow
//	case WM_STYLECHANGING:
//		break;
//	case WM_STYLECHANGED:
//		DoStyleChanged( hWnd, wParam, lParam );
//		break;
	case WM_ENABLE:
		DoEnable( hWnd, (BOOL)wParam );
		break;
	case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
	case DSM_GETDATETIME:
		return DoGetDateTime(hWnd,wParam,lParam);

	//dateshow and datesel
	case DSM_SETDATETIME:
		return DoSetDateTime(hWnd,wParam,lParam);
	case DSM_SETYEARRANGE:
		return DoSetRange(hWnd,wParam,lParam);
	//only datesel
	case DSM_SETDAYSFONT:
		return DoSetDateFont(hWnd,wParam,lParam);
	case DSM_SETDATESTYLE:
		DoSetDateStyle( hWnd, wParam, lParam );
		break;
	case DSM_SETDATESIZE:
		DoSetDateSize( hWnd, wParam, lParam );
		break;
	case DSM_GETMODIFY:
		return DoGetModify( hWnd, wParam, lParam );
	case DSM_SETMODIFY:
		DoSetModify( hWnd, wParam, lParam );
		break;
	case DSM_SETUSERSELNOTIFY:
		DoSetUserNotify( hWnd, wParam, lParam );
		break;

    case WM_CREATE:
		return DoCreate(hWnd,wParam,lParam);
	case WM_DESTROY:
		DoDestroy(hWnd);
		break;
	default:
		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

void	DoPaint( HWND hWnd, HDC hdc, const RECT* lprtPaint )
{
	LPDATESHOWINFO	lpInfo;
	SYSTEMTIME*		pstDateShow;

	RECT		rtClient;
	HDC			hdcDraw;
	HBITMAP		hBmpDraw;
	HBITMAP		hBmpOldDraw;

	int			iModeOld;
	COLORREF	colOld;
	COLORREF	colText;
	COLORREF	colTextBk;
	TCHAR		szTmp[16];

	lpInfo = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return ;

	GetClientRect( hWnd, &rtClient );
	hdcDraw = CreateCompatibleDC( hdc );
	hBmpDraw   = CreateCompatibleBitmap( hdc, RECT_CX(&rtClient), RECT_CY(&rtClient) );
	hBmpOldDraw= (HBITMAP)SelectObject( hdcDraw, hBmpDraw );
	ASSERT( hdcDraw && hBmpDraw );

	//
	if( IsWindowEnabled(hWnd) )
	{
		colText   = lpInfo->cl_Text;
		colTextBk = lpInfo->cl_TextBk;
	}
	else
	{
		colText   = lpInfo->cl_Disable;
		colTextBk = lpInfo->cl_DisableBk;
	}

	//绘制背景
	{
		HPEN		hPen;
		HPEN		hPenOld;
		HBRUSH		hBrh;
		HBRUSH		hBrhOld;

		hPen = CreatePen( PS_SOLID, 1, colTextBk );
		hBrh = CreateSolidBrush( colTextBk );
		ASSERT( hPen && hBrh );
		hPenOld = SelectObject( hdcDraw, (HPEN)hPen );
		hBrhOld = SelectObject( hdcDraw, (HPEN)hBrh );

		Rectangle( hdcDraw, rtClient.left, rtClient.top, rtClient.right, rtClient.bottom );

		SelectObject( hdcDraw, (HPEN)hBrhOld );
		SelectObject( hdcDraw, (HPEN)hPenOld );
		DeleteObject( hPen );
		DeleteObject( hBrh );
	}
	//绘制---年月日
	iModeOld = SetBkMode( hdcDraw, TRANSPARENT );
	colOld = SetTextColor( hdcDraw, colText );

	pstDateShow = &lpInfo->stDateShow;
	sprintf( szTmp, "%0004d-%02d-%02d", pstDateShow->wYear, pstDateShow->wMonth, pstDateShow->wDay );
#define	YMD1_DRAW	(DT_SINGLELINE|DT_VCENTER|DT_LEFT)
	rtClient.left += 2;
	DrawText( hdcDraw, szTmp, -1, &rtClient, YMD1_DRAW );

	SetTextColor( hdcDraw, colOld );
	SetBkMode( hdcDraw, iModeOld );
	//
	BitBlt( hdc, lprtPaint->left, lprtPaint->top, RECT_CX(lprtPaint), RECT_CY(lprtPaint), hdcDraw, lprtPaint->left, lprtPaint->top, SRCCOPY );
	//
	SelectObject( hdcDraw, (HBITMAP)hBmpOldDraw );
	DeleteObject( hBmpDraw );
	DeleteDC( hdcDraw );
}

BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
{
	LPDATESHOWINFO	lpInfo;

	BOOL			fSuccess = FALSE;
	HICON			hMap;
	int				nState;
	COLORREF		colTextBk;

#ifdef	PAD_DRAWFOCUS
	BOOL			fFocus = FALSE;
#endif

	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;
	//check
	//ASSERT( pDrawItem->CtlType==ODT_BUTTON );
	if( pDrawItem->CtlType!=ODT_BUTTON || (idCtl!=IDC_DATESHOW) )
		return FALSE;

	if( IsWindowEnabled(hWnd) )
		colTextBk = lpInfo->cl_TextBk;
	else
		colTextBk = lpInfo->cl_DisableBk;
	//
	nState = 0;
	// 通用绘制
	if( pDrawItem->itemAction == ODA_DRAWENTIRE )		
	{
		//nState = 0;
		if( pDrawItem->itemState & ODS_DISABLED )		// disable
		{
			nState = 2;	//attention here
		}
	}
	// 选中绘制	
	else if( pDrawItem->itemAction == ODA_SELECT )	
	{
		if( pDrawItem->itemState & ODS_SELECTED )		//得到 Select
		{
			nState = 1;
		}
		//else
		//	nState = 0;								//失去 Select
//#ifdef	PAD_DRAWFOCUS
//		if( pDrawItem->itemState & ODS_FOCUS )		//有焦点、没有焦点
//			fFocus = TRUE;
//#endif
	}
#ifdef	PAD_DRAWFOCUS
	//焦点操作，不用绘制
	else if( pDrawItem->itemAction & ODA_FOCUS )
	{
		fFocus = TRUE;
	}
#endif

	hMap = lpInfo->hIconDown[nState];
	if( hMap )
	{
		HPEN			hPen;
		HPEN			hPenOld;
		HBRUSH			hBrh;
		HBRUSH			hBrhOld;
		RECT*		prtItem;
		
		prtItem = &pDrawItem->rcItem;
		hPen = CreatePen( PS_SOLID, 1, colTextBk );
		hBrh = CreateSolidBrush( colTextBk );
		ASSERT( hBrh && hPen );
		hPenOld = (HPEN)SelectObject( pDrawItem->hDC, hPen );
		hBrhOld = (HBRUSH)SelectObject( pDrawItem->hDC, hBrh );

		Rectangle( pDrawItem->hDC, prtItem->left, prtItem->top, prtItem->right, prtItem->bottom );

		SelectObject( pDrawItem->hDC, hPenOld );
		SelectObject( pDrawItem->hDC, hBrhOld );
		DeleteObject( hPen );
		DeleteObject( hBrh );

		DrawIcon( pDrawItem->hDC, prtItem->left, prtItem->top, hMap );
		fSuccess = TRUE;
	}

#ifdef	PAD_DRAWFOCUS
	if( fFocus )
	{
		DrawFocusRect( pDrawItem->hDC, &pDrawItem->rcItem );
		fSuccess = TRUE;
	}
#endif
	
	return fSuccess;
}

LRESULT	DoCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;
	
	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;

	if( HIWORD(wParam)==BN_CLICKED && LOWORD(wParam)==IDC_DATESHOW )
	{
		if( !lpInfo->hWndDateSel )
			DoShowDateSel( hWnd, lpInfo );
	}

	return 0;
}

void	DoShowDateSel( HWND hWnd, LPDATESHOWINFO lpInfo )
{
	HWND			hWndDate;
	HINSTANCE		hInst;
	RECT			rtWnd;
	RECT			rtWndDate;
	int				xPos;
	int				yPos;

	//
	hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );
	hWndDate = CreateWindow(classDATESEL, "", WS_POPUP |WS_BORDER, 0, 0, lpInfo->cxDate, lpInfo->cyDate, hWnd, (HMENU)IDC_DATESEL, hInst, NULL);
	if( !hWndDate )
		return ;
	lpInfo->hWndDateSel = hWndDate;
	//
	SendMessage( hWndDate, DSM_SETYEARRANGE, (WPARAM)MAKELONG(lpInfo->wCaleYearMin, lpInfo->wCaleYearMax), 0 );
	SendMessage( hWndDate, DSM_SETDATETIME, (WPARAM)0, (LPARAM)&lpInfo->stDateShow );
	SendMessage( hWndDate, DSM_SETUSERSELNOTIFY, (WPARAM)hWnd, (LPARAM)WM_USERSELDATE );

	SendMessage( hWndDate, DSM_SETDAYSFONT, (WPARAM)0, (LPARAM)lpInfo->wFlagFont );
	SendMessage( lpInfo->hWndDateSel, DSM_SETDATESTYLE, lpInfo->dwDateStyle, 0 );
	//
	GetWindowRect( hWnd, &rtWnd );
	GetWindowRect( hWndDate, &rtWndDate );
	xPos = rtWnd.right - RECT_CX(&rtWndDate);
	yPos = rtWnd.bottom + 1;
	SetWindowPos( hWndDate, NULL, xPos, yPos, 0, 0, SWP_NOSIZE |SWP_NOZORDER |SWP_SHOWWINDOW );
}

void	DoUserSelDate( HWND hWnd, HWND hWndDate, BOOL fSelect )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return ;

	if( hWndDate )
	{
		if( fSelect )
		{
			lpInfo->fIsModified = TRUE;
			SendMessage( hWndDate, DSM_GETDATETIME, NULL, (LPARAM)&lpInfo->stDateShow );

			if( lpInfo->hWndUserSel )
			{
				PostMessage( lpInfo->hWndUserSel, lpInfo->uMsgUserSel, 0, 0 );
			}
			InvalidateRect( hWnd, NULL, FALSE );
		}

		DestroyWindow( hWndDate );
		lpInfo->hWndDateSel = NULL;
	}
}


LRESULT	DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;

	HINSTANCE		hInst;
	BOOL			fSetSize;
	RECT			rtClient;
	RECT			rtWnd;
	HWND			hWndChild;
	DWORD			dwStyle;

    SetWindowLong( hWnd, 0, 0 );
	//重新设置  位置和大小
	hInst = (HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE );
	GetWindowRect( hWnd, &rtWnd );
	fSetSize = FALSE;
	if( RECT_CX(&rtWnd) < W_APP )
	{
		fSetSize = TRUE;
		rtWnd.right = rtWnd.left + W_APP;
	}
	if( RECT_CY(&rtWnd) < H_APP )
	{
		fSetSize = TRUE;
		rtWnd.bottom = rtWnd.top + H_APP;
	}
	if( fSetSize )
	{
		SetWindowPos( hWnd, NULL, 0, 0, RECT_CX(&rtWnd), RECT_CY(&rtWnd), SWP_NOMOVE |SWP_NOZORDER );
	}
	//
	GetClientRect( hWnd, &rtClient );
	hWndChild = CreateWindow( classBUTTON, TEXT(""), BS_OWNERDRAW |WS_VISIBLE |WS_CHILD, rtClient.right-CX_TODOWN-3, ((rtClient.bottom-CY_TODOWN+3)/2), 
		CX_TODOWN, CY_TODOWN, hWnd, (HMENU)IDC_DATESHOW, hInst, NULL);
	if( !hWndChild )
	{
		return -1;
	}
	//
    lpInfo = (LPDATESHOWINFO)malloc(sizeof(DATESHOWINFO));
    if(lpInfo==NULL)
		return -1;	
	memset( lpInfo, 0, sizeof(DATESHOWINFO) );
    SetWindowLong(hWnd,0,(DWORD)lpInfo);
	//
	lpInfo->hIconDown[0] = LoadIcon_xy(NULL, OIC_DATESHOW_DOWN, 17, 12);
	lpInfo->hIconDown[1] = LoadIcon_xy(NULL, OIC_DATESHOW_DOWNDOWN, 17, 12);
	lpInfo->hIconDown[2] = LoadIcon_xy(NULL, OIC_DATESHOW_DOWNGRAY, 17, 12);
	if( !lpInfo->hIconDown[0] ||!lpInfo->hIconDown[1] ||!lpInfo->hIconDown[2] )
		return -1;
	//
    GetLocalTime( &lpInfo->stDateShow );
	lpInfo->wCaleYearMin= YEAR_MIN;
	lpInfo->wCaleYearMax= YEAR_MAX;
	//
	lpInfo->cl_Text        = GetSysColor( COLOR_WINDOWTEXT );
	lpInfo->cl_TextBk      = GetSysColor( COLOR_BACKGROUND );     //正常文本的前景与背景色
	lpInfo->cl_Disable     = GetSysColor( COLOR_GRAYTEXT );
	lpInfo->cl_DisableBk   = lpInfo->cl_TextBk;//GetSysColor( COLOR_BACKGROUND );    // 无效文本的前景与背景色
	//save for datesel
	lpInfo->wFlagFont      = DAYSFONT_DEFAULT;
	lpInfo->dwDateStyle    = DSS_USERSEL;
	lpInfo->cxDate = CX_DATE;
	lpInfo->cyDate = CY_DATE;

	//
	dwStyle = (DWORD)GetWindowLong( hWnd, GWL_STYLE );
	if( dwStyle & WS_DISABLED )
	{
		EnableWindow( hWndChild, FALSE );
	}

	//
    return 0;
}

LRESULT	DoDestroy( HWND hWnd )
{
	LPDATESHOWINFO	lpInfo;
	int				i;
	
	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;
	for( i=0; i<3; i++ )
	{
		if( lpInfo->hIconDown[i] )
			DestroyIcon( lpInfo->hIconDown[i] );
	}
	if( lpInfo->hWndDateSel )
		DestroyWindow( lpInfo->hWndDateSel );

	free( lpInfo );
	return 0;
}

void	DoEnable( HWND hWnd, BOOL fEnable )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;

	EnableWindow( GetDlgItem(hWnd, IDC_DATESHOW), fEnable );
	if( lpInfo->hWndDateSel )
		EnableWindow( lpInfo->hWndDateSel, fEnable );
}

/*
void	DoStyleChanged( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;
	LPSTYLESTRUCT	lpStyle;

	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;
	if( wParam==GWL_STYLE )
	{
		lpStyle= (LPSTYLESTRUCT)lParam;

		EnableWindow( GetDlgItem(hWnd, IDC_DATESEL), (lpStyle->styleNew & WS_DISABLED)?FALSE:TRUE );
		//if( lpInfo->hWndDateSel )
		{
			EnableWindow( lpInfo->hWndDateSel, (lpStyle->styleNew & WS_DISABLED)?FALSE:TRUE );
			//dwStyle = GetWindowLong( lpInfo->hWndDateSel, GWL_STYLE );
			//SetWindowLong( lpInfo->hWndDateSel, GWL_STYLE, (LONG)lpInfo->dwStyle );
		}
		//InvalidateRect( hWnd, NULL, FALSE );
	}
}
*/

LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	LPDATESHOWINFO	lpInfo;
	UINT			fMask;

	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return FALSE;

	fMask = lpccs->fMask;
	if( fMask & CLF_TEXTCOLOR		 )
		lpInfo->cl_Text			= lpccs->cl_Text		;
	if( fMask & CLF_TEXTBKCOLOR		 )
		lpInfo->cl_TextBk		= lpccs->cl_TextBk		;
	if( fMask & CLF_DISABLECOLOR	 )
		lpInfo->cl_Disable		= lpccs->cl_Disable		;
	if( fMask & CLF_DISABLEBKCOLOR	 )
		lpInfo->cl_DisableBk	= lpccs->cl_DisableBk	;
	//
	InvalidateRect( hWnd, NULL, FALSE );
	return TRUE;
}

LRESULT	DoGetDateTime( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;
	LPSYSTEMTIME	lpDateTime;

	lpInfo=(LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return FALSE;
	lpDateTime=(LPSYSTEMTIME)lParam;

	lpDateTime->wYear      =lpInfo->stDateShow.wYear     ;
	lpDateTime->wMonth     =lpInfo->stDateShow.wMonth    ;
	lpDateTime->wDay       =lpInfo->stDateShow.wDay      ;
	lpDateTime->wDayOfWeek =lpInfo->stDateShow.wDayOfWeek;

	return TRUE;
}


LRESULT	DoSetDateTime( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPSYSTEMTIME	lpDateTime;
	LPDATESHOWINFO	lpInfo;
	
	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
	lpDateTime = (LPSYSTEMTIME)lParam;
    if( lpDateTime==NULL || lpInfo==NULL )
		return FALSE;

	//check lpDateTime
	if( lpDateTime->wMonth>12 || lpDateTime->wMonth<1 )
		return FALSE;
	if( lpDateTime->wYear>lpInfo->wCaleYearMax || lpDateTime->wYear<lpInfo->wCaleYearMin )
		return FALSE;
	//
	lpInfo->stDateShow.wYear      =lpDateTime->wYear     ;
	lpInfo->stDateShow.wMonth     =lpDateTime->wMonth    ;
	lpInfo->stDateShow.wDay       =lpDateTime->wDay      ;
	lpInfo->stDateShow.wDayOfWeek =lpDateTime->wDayOfWeek;
	//
	if( lpInfo->hWndDateSel )
	{
		SendMessage( lpInfo->hWndDateSel, DSM_SETDATETIME, 0, (LPARAM)&lpInfo->stDateShow );
	}
	//
	InvalidateRect( hWnd, NULL, FALSE );
	return TRUE;
}


LRESULT	DoSetRange( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;
	SYSTEMTIME*		pstDateShow;
	BOOL			fReDraw;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	//check 
	lpInfo->wCaleYearMin = LOWORD(wParam);
	lpInfo->wCaleYearMax = HIWORD(wParam);
	if( lpInfo->hWndDateSel )
		SendMessage( lpInfo->hWndDateSel, DSM_SETYEARRANGE, wParam, 0 );
	//
	fReDraw = FALSE;
	pstDateShow = &lpInfo->stDateShow;
	if( pstDateShow->wYear > lpInfo->wCaleYearMax )
	{
		pstDateShow->wYear = lpInfo->wCaleYearMax;
		fReDraw = TRUE;
	}
	else if( pstDateShow->wYear < lpInfo->wCaleYearMin )
	{
		pstDateShow->wYear = lpInfo->wCaleYearMin;
		fReDraw = TRUE;
	}
	if( fReDraw )
	{
		InvalidateRect( hWnd, NULL, FALSE );
	}
	return TRUE;
}

LRESULT	DoSetDateFont( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	lpInfo->wFlagFont = (WORD)lParam;
	if( lpInfo->hWndDateSel )
		SendMessage( lpInfo->hWndDateSel, DSM_SETDAYSFONT, 0, (LPARAM)lParam );

	return TRUE;
}

LRESULT	DoSetDateStyle( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	lpInfo->dwDateStyle = (DWORD)wParam;
	if( lpInfo->hWndDateSel )
	{
		SendMessage( lpInfo->hWndDateSel, DSM_SETDATESTYLE, lpInfo->dwDateStyle, 0 );
	}

	return 0;
}

LRESULT	DoSetDateSize( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;
	SHORT			cxDate;
	SHORT			cyDate;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;
	cxDate = LOWORD(wParam);
	cyDate = HIWORD(wParam);
	if( cxDate > CX_DATE )
		lpInfo->cxDate = cxDate;
	if( cyDate > CY_DATE )
		lpInfo->cyDate = cyDate;
	if( lpInfo->hWndDateSel )
	{
		RECT	rtWnd;
		int		xPos;
		int		yPos;

		GetWindowRect( hWnd, &rtWnd );
		xPos = rtWnd.right - lpInfo->cxDate;
		yPos = rtWnd.bottom + 1;
		SetWindowPos( lpInfo->hWndDateSel, NULL, xPos, yPos, lpInfo->cxDate, lpInfo->cyDate, SWP_NOZORDER |SWP_SHOWWINDOW );
	}

	return 0;
}

LRESULT	DoGetModify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return -1;

	return lpInfo->fIsModified;
}

LRESULT	DoSetModify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;

	if( wParam )
		lpInfo->fIsModified = TRUE;
	else
		lpInfo->fIsModified = FALSE;

	return 0;
}

LRESULT	DoSetUserNotify( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPDATESHOWINFO	lpInfo;

	lpInfo     = (LPDATESHOWINFO)GetWindowLong(hWnd,0);
    if( lpInfo==NULL )
		return FALSE;

	lpInfo->hWndUserSel = (HWND)wParam;
	lpInfo->uMsgUserSel = (UINT)lParam;
	return TRUE;
}
