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
#include <ecomctrl.h>
#include <hotkey.h>

#ifdef EML_DOS
#include "Dos.h"
#include <stdio.h>
#endif

//---------------------------------------------------
const char classHOTKEY[]="HOTKEY_class";


//---------------------------------------------------
#define YEAR_MIN		0
#define YEAR_MAX		9999

#define	WM_USERSELDATE		(WM_USER+9731)

//---------------------------------------------------
#define	RECT_CX( prt )				((prt)->right - (prt)->left)
#define	RECT_CY( prt )				((prt)->bottom - (prt)->top)

//---------------------------------------------------
typedef	struct	_HOTKEYINFO
{
	//
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //正常文本的前景与背景色
	COLORREF cl_Disable;
	COLORREF cl_DisableBk;    // 无效文本的前景与背景色

	//
	DWORD	dwStyle;
	SHORT	shCntCaret;
	SHORT	shIsCreate;
	//setting
	WORD	fwCombInv;
	WORD	fwModInv;
	//put key
	WORD	wHKey_VK;		//low-order  byte
	WORD	wHKey_Modifier;	//high-order byte
	//for draw
	TCHAR	pszHotKey[32];	//Ctl + Shift + CAPSLock

} HOTKEYINFO, *LPHOTKEYINFO;


//---------------------------------------------------
static	LRESULT	CALLBACK	WndProc_HotKey( HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static	void	DoPaint( HWND hWnd, HDC hdc, const RECT* lprtPaint );
static	LRESULT	DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoDestroy( HWND hWnd );
static	void	DoEnable( HWND hWnd, BOOL fEnable );
static	LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs );
static	void	DoKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	void	DoKeyUp( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	void	DoDrawCaret( HWND hWnd, BOOL fShow );

static	BOOL	hk_InputKey( HWND hWnd, LPHOTKEYINFO lpInfo, SHORT shVirKey );

static	void	DoSetRules( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	void	DoSetHotKey( HWND hWnd, WPARAM wParam, LPARAM lParam );
static	LRESULT	DoGetHotKey( HWND hWnd );

//---------------------------------------------------
ATOM RegisterHotKeyClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = WndProc_HotKey;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_WINDOW);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classHOTKEY;
	
    return (BOOL)(RegisterClass( &wc ));
}

//---------------------------------------------------
LRESULT	CALLBACK	WndProc_HotKey( HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT	ps;
			HDC			hdc;

			hdc = BeginPaint(hWnd, &ps);
			DoPaint(hWnd,hdc, &ps.rcPaint);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_KILLFOCUS:
		DoDrawCaret( hWnd, FALSE );
		break;
	case WM_SETFOCUS:
		DoDrawCaret( hWnd, TRUE );
		break;
	case WM_LBUTTONDOWN:
		SetFocus( hWnd );
		break;
	case WM_KEYDOWN:
		DoKeyDown( hWnd, wParam, lParam );
		break;
	case WM_KEYUP:
		DoKeyUp( hWnd, wParam, lParam );
		break;

	case HKM_SETRULES:
		DoSetRules( hWnd, wParam, lParam );
		break;
	case HKM_SETHOTKEY:
		DoSetHotKey( hWnd, wParam, lParam );
		break;
	case HKM_GETHOTKEY:
		return DoGetHotKey( hWnd );

	case WM_ENABLE:
		DoEnable( hWnd, (BOOL)wParam );
		break;
	case WM_SETCTLCOLOR:
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );

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
	LPHOTKEYINFO	lpInfo;

	RECT		rtClient;
	HDC			hdcDraw;
	HBITMAP		hBmpDraw;
	HBITMAP		hBmpOldDraw;

	int			iModeOld;
	COLORREF	colOld;
	COLORREF	colText;
	COLORREF	colTextBk;
	DWORD		dwStyle;
	UINT		uFormat;

	lpInfo = (LPHOTKEYINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return ;
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	HideCaret( hWnd );
	lpInfo->shCntCaret --;

	GetClientRect( hWnd, &rtClient );
	hdcDraw = CreateCompatibleDC( hdc );
	hBmpDraw   = CreateCompatibleBitmap( hdc, RECT_CX(&rtClient), RECT_CY(&rtClient) );
	hBmpOldDraw= (HBITMAP)SelectObject( hdcDraw, hBmpDraw );
	ASSERT( hdcDraw && hBmpDraw );
	//
	if( dwStyle & WS_DISABLED )
	{
		colText   = lpInfo->cl_Disable;
		colTextBk = lpInfo->cl_DisableBk;
	}
	else
	{
		colText   = lpInfo->cl_Text;
		colTextBk = lpInfo->cl_TextBk;
	}
	//
	uFormat = DT_SINGLELINE;
	switch( dwStyle & (HKS_LEFT |HKS_CENTER |HKS_RIGHT) )
	{
	case HKS_LEFT:
		uFormat |= DT_LEFT;
		break;
	case HKS_CENTER:
		uFormat |= DT_CENTER;
		break;
	case HKS_RIGHT:
		uFormat |= DT_RIGHT;
		break;
	default :
		uFormat |= DT_LEFT;
		break;
	}
	switch( dwStyle & (HKS_TOP |HKS_VCENTER |HKS_BOTTOM) )
	{
	case HKS_TOP:
		uFormat |= DT_TOP;
		break;
	case HKS_VCENTER:
		uFormat |= DT_VCENTER;
		break;
	case HKS_BOTTOM:
		uFormat |= DT_BOTTOM;
		break;
	default :
		uFormat |= DT_TOP;
		break;
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
	//绘制---Key
	iModeOld = SetBkMode( hdcDraw, TRANSPARENT );
	colOld = SetTextColor( hdcDraw, colText );

	rtClient.left += 2;
	DrawText( hdcDraw, lpInfo->pszHotKey, -1, &rtClient, uFormat );

	SetTextColor( hdcDraw, colOld );
	SetBkMode( hdcDraw, iModeOld );

	//
	BitBlt( hdc, lprtPaint->left, lprtPaint->top, RECT_CX(lprtPaint), RECT_CY(lprtPaint), hdcDraw, lprtPaint->left, lprtPaint->top, SRCCOPY );
	//
	SelectObject( hdcDraw, (HBITMAP)hBmpOldDraw );
	DeleteObject( hBmpDraw );
	DeleteDC( hdcDraw );

	ShowCaret( hWnd );
	lpInfo->shCntCaret ++;
}

LRESULT	DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPHOTKEYINFO	lpInfo;

    SetWindowLong( hWnd, 0, 0 );
	//
    lpInfo = (LPHOTKEYINFO)malloc(sizeof(HOTKEYINFO));
    if(lpInfo==NULL)
		return -1;	
	memset( lpInfo, 0, sizeof(HOTKEYINFO) );
    SetWindowLong(hWnd,0,(DWORD)lpInfo);
	//
	strcpy( lpInfo->pszHotKey, TEXT("无") );
	//
	lpInfo->cl_Text        = GetSysColor( COLOR_WINDOWTEXT );
	lpInfo->cl_TextBk      = GetSysColor( COLOR_BACKGROUND );     //正常文本的前景与背景色
	lpInfo->cl_Disable     = GetSysColor( COLOR_GRAYTEXT );
	lpInfo->cl_DisableBk   = lpInfo->cl_TextBk;//GetSysColor( COLOR_BACKGROUND );    // 无效文本的前景与背景色

	//
    return 0;
}

LRESULT	DoDestroy( HWND hWnd )
{
	LPHOTKEYINFO	lpInfo;
	
	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
	if (lpInfo==NULL)
		return 0;

	free( lpInfo );
	DestroyCaret( );
	return 0;
}

void	DoEnable( HWND hWnd, BOOL fEnable )
{
}

LRESULT	DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	LPHOTKEYINFO	lpInfo;
	UINT			fMask;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
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


//#define	MODIFIER_S		0x01
//#define	MODIFIER_C		0x02
//#define	MODIFIER_A		0x04
//
//SHORT	hlp_GetKeySCA()
//{
//	SHORT	shSCA;
//	SHORT	shState;
//
//	shState = GetKeyState( VK_SHIFT );
//	if( shState & 0x8000 )
//	{
//	}
//}

BOOL	hk_InputKey( HWND hWnd, LPHOTKEYINFO lpInfo, SHORT shVirKey )
{
	LPTSTR			pszHotKey;
	SHORT			shDraw;

	shDraw = 1;
	pszHotKey = lpInfo->pszHotKey;
	//
	//A-Z, 0-9
	if(	(shVirKey>=VK_A && shVirKey<=VK_Z) || (shVirKey>=VK_0 && shVirKey<=VK_9) )
	{
		pszHotKey[0] = (TCHAR)shVirKey;
		pszHotKey[1] = 0;
	}
	//numpad
	else if( (shVirKey>=VK_NUMPAD0 && shVirKey<=VK_DIVIDE) )
	{
		if( shVirKey<VK_MULTIPLY )
			sprintf( pszHotKey, TEXT("Num %d"), (shVirKey-VK_NUMPAD0) );
		else
			sprintf( pszHotKey, TEXT("Num %c"), (TCHAR)(shVirKey-0x40) );
		pszHotKey[5] = 0;
	}
	//enter
	else if( (shVirKey==VK_RETURN) )
	{
		strcpy( pszHotKey, TEXT("Enter") );
	}
	//Back space
	else if( shVirKey==VK_BACK )
	{
		shVirKey = 0;
		strcpy( pszHotKey, TEXT("无") );
	}
	//esc
	else if( (shVirKey==VK_ESCAPE) )
	{
		strcpy( pszHotKey, TEXT("Esc") );
	}
	//space, page4 and dir4
	else if( shVirKey>=VK_SPACE && shVirKey<=VK_DOWN )
	{
		//space
		if( (shVirKey==VK_SPACE) )
		{
			strcpy( pszHotKey, TEXT("Space") );
		}
		//Page
		else if( (shVirKey==VK_PRIOR) )
		{
			strcpy( pszHotKey, TEXT("PageUp") );
		}
		else if( (shVirKey==VK_NEXT) )
		{
			strcpy( pszHotKey, TEXT("PageDown") );
		}
		//
		else if( (shVirKey==VK_HOME) )
		{
			strcpy( pszHotKey, TEXT("Home") );
		}
		else if( (shVirKey==VK_END) )
		{
			strcpy( pszHotKey, TEXT("End") );
		}
		//
		else if( (shVirKey==VK_LEFT) )
		{
			strcpy( pszHotKey, TEXT("Left") );
		}
		else if( (shVirKey==VK_UP) )
		{
			strcpy( pszHotKey, TEXT("Up") );
		}
		else if( (shVirKey==VK_RIGHT) )
		{
			strcpy( pszHotKey, TEXT("Right") );
		}
		else if( (shVirKey==VK_DOWN) )
		{
			strcpy( pszHotKey, TEXT("Down") );
		}
	}
	else if( shVirKey>=VK_SEMICOLON && shVirKey<=VK_SLASH )
	{
		if( shVirKey==VK_SEMICOLON )
		{
			pszHotKey[0] = TEXT(';');
			pszHotKey[1] = 0;
		}
		else
		{
			sprintf( pszHotKey, TEXT("%c"), (TCHAR)(shVirKey-0X90) );
		}
	}
	else if( shVirKey==VK_BACKQUOTE )
	{
		pszHotKey[0] = TEXT('~');
		pszHotKey[1] = 0;
	}
	else if( shVirKey>=VK_LBRACKET && shVirKey<=VK_APOSTROPHE )
	{
		if( shVirKey==VK_APOSTROPHE )
		{
			pszHotKey[0] = TEXT('\"');
			pszHotKey[1] = 0;
		}
		else
		{
			sprintf( pszHotKey, TEXT("%c"), (TCHAR)(shVirKey-0X80) );
		}
	}
	//
	else if( (shVirKey==VK_INSERT) )
	{
		strcpy( pszHotKey, TEXT("Insert") );
	}
	else if( (shVirKey==VK_DELETE) )
	{
		strcpy( pszHotKey, TEXT("Delete") );
	}
	//VK_F1
	else if( (shVirKey>=VK_F1 && shVirKey<=VK_F24) )
	{
		sprintf( pszHotKey, TEXT("F%d"), (shVirKey-VK_F1) );
	}
	//
	else if( (shVirKey==VK_NUMLOCK) )
	{
		strcpy( pszHotKey, TEXT("NumLock") );
	}
	else if( (shVirKey==VK_CAPITAL) )
	{
		strcpy( pszHotKey, TEXT("CAPSLock") );
	}
	//none key
	else
	{
		shVirKey = 0;
		shDraw = 0;
	}
	//
	if( shDraw )
	{
		lpInfo->wHKey_VK = shVirKey;
		//
		InvalidateRect( hWnd, NULL, FALSE );
	}
	return (BOOL)shDraw;
}

void	DoKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPHOTKEYINFO	lpInfo;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;
	
	hk_InputKey( hWnd, lpInfo, (SHORT)wParam );
}

void	DoKeyUp( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPHOTKEYINFO	lpInfo;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;
}

void	DoSetRules( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPHOTKEYINFO	lpInfo;
	WORD			fwCombInv;
	WORD			fwModInv;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;

	fwCombInv = (WORD)wParam;
	fwModInv  = (WORD)LOWORD(lParam);
	lpInfo->fwCombInv = (fwCombInv & HKCOMB_ALL);
	lpInfo->fwModInv  = (fwModInv  & HOTKEYF_ALL);
	//
	return ;
}

void	DoSetHotKey( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	LPHOTKEYINFO	lpInfo;
	SHORT			bVKHotKey;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;

	bVKHotKey = (SHORT)wParam;
	hk_InputKey( hWnd, lpInfo, bVKHotKey );
}

LRESULT	DoGetHotKey( HWND hWnd )
{
	LPHOTKEYINFO	lpInfo;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return MAKELONG(0, 0);
	
	return MAKELONG(lpInfo->wHKey_VK, 0);
}


void	DoDrawCaret( HWND hWnd, BOOL fShow )
{
	LPHOTKEYINFO	lpInfo;
	RECT			rtClient;
//	int				nPosX;
	SHORT			shCntCaret;

	lpInfo=(LPHOTKEYINFO)GetWindowLong(hWnd,0);
    if (lpInfo==NULL)
		return ;

	shCntCaret = lpInfo->shCntCaret;
	if( fShow )
	{
		if( lpInfo->shIsCreate )
		{
			while( shCntCaret<=0 )
			{
				ShowCaret( hWnd );
				shCntCaret ++;
			}
		}
		else
		{
			GetClientRect( hWnd, &rtClient );
			//nPosX = GetSystemMetrics(SM_CXBORDER);
			CreateCaret( hWnd, NULL, 2, (rtClient.bottom - rtClient.top - 4) );
			SetCaretPos( 2+1, 2 );
			lpInfo->shIsCreate = TRUE;
			//
			ShowCaret( hWnd );
			shCntCaret ++;
		}
	}
	else
	{
		DestroyCaret( );
		lpInfo->shIsCreate = FALSE;
		shCntCaret = 0;
	}
	lpInfo->shCntCaret = shCntCaret;
}
