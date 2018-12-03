#include <EWindows.h>
#include <EDialog.h>

#include "eprsht.h"
#define IDD_PROPERTY 0
static const struct dlgIDD_PROPERTY{
    DLG_TEMPLATE dlg;
}dlgIDD_PROPERTY = {
    { 0x90000000L,0,0,0,240,295,0,0,"" }
};

typedef struct tagPROPERTYDATA{
	LPPTRLIST				lpPageList ;
	LPPROPSHEETHEADER		lppsh;
	int						iIndex ;// index of current item in tab control
	int						iStart ;
    HWND					hDlg;          // the dialog for this instance data
    HWND					hwndCurPage;   // current page hwnd  
	HWND					hLeftButton;
	HWND					hRightButton;
//    int						idDefaultFallback; // the default id to set as DEFID if page doesn't have one
//    int nReturn;
//    UINT nRestart;

    int						xSubDlg, ySubDlg;       // dimensions of sub dialog
    int						cxSubDlg, cySubDlg;

    BOOL					fFlags;
}_PROPERTDATA,*_LPPROPERTDATA;

typedef struct _PSP {
    DWORD				dwFlags;
    DWORD				dwState;
	UINT				id;
	HWND				hWnd;
    PROPSHEETPAGE		psp;
    // NOTE: the above member can be variable size so don't add any
    // structure members here
} _SHEETPAGEDATA,  *_LPSHEETPAGEDATA;//,  *HPROPSHEETPAGE;

#define PAGE_DATA_CHANGED				0x00000001
#define CHAR_WIDTH						9
#define PROPERTY_SHEET_HEIGH			25
#define LEFT_BUTTON						100
#define RIGHT_BUTTON					101
#define PTM_SETDATA						( WM_USER + 1 )

const TCHAR			szProPertyClass[] = "PROPERTY_CLASS___";
//=======================================================
static _LPPROPERTDATA GetPrshtPtr( HWND hWnd );
ATOM RegistPropertyClass( HINSTANCE hInstance );
static LRESULT CALLBACK PropertyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//=======================================================
static BOOL OnPrshtCreate( HWND hWnd );
static void OnPrshtDestroy( HWND hWnd );
static BOOL GetTableButtonRect( HWND hWnd,  LPRECT lprt, int iIndex, BOOL isAll );
static void OnPrshtLButtonDown( HWND hWnd, WPARAM wParam, short xPos, short yPos );
static void OnPrshtPaint( HWND hWnd );
static void OnPrshtDrawItem( HWND hWnd, WPARAM wParam, LPDRAWITEMSTRUCT lpdis );
static void OnPrshtCommand( HWND hWnd, WPARAM wParam, HWND hCtrl );
static BOOL OnPrshtAddPage( HWND hWnd, _LPSHEETPAGEDATA hPage );
static void OnPrshtChangeState( HWND hWnd, HWND hPage, BOOL isChanged );
static HWND OnPrshtGetCurrentPageHwnd( HWND hWnd );
static BOOL OnPrshtIsDialogMessage( HWND hWnd, MSG *pmsg );
static void OnPrshtRemovePage( HWND hWnd, WPARAM wParam,  _LPSHEETPAGEDATA hPage );
static void FlashPrshtPages( HWND hWnd, int iOld, int iNew );
static BOOL OnPrshtSetCursel( HWND hWnd, WPARAM wParam, _LPSHEETPAGEDATA hPage);
static void OnPrshtSetTitle( HWND hWnd, WPARAM wParam, LPCSTR lpcstrText );
//=======================================================
static void GetTwoButtonRect( HWND hWnd, LPRECT lprt )
{
	GetClientRect( hWnd, lprt );
//	if( lppd->lppsh->dwFlags&CCS_TOP )
//	{
//		lprt->bottom = lprt->top + PROPERTY_SHEET_HEIGH ;
//	}else
//	{
		lprt->top = lprt->bottom - PROPERTY_SHEET_HEIGH  ;
		lprt->left = lprt->right - 30 ;
//	}
}
static BOOL OnPrshtCreate( HWND hWnd  )
{
	_LPPROPERTDATA			lppd;
	RECT					rt;
	HINSTANCE				hIns;
	lppd = ( _LPPROPERTDATA )malloc( sizeof( _PROPERTDATA ) );
	if( lppd == NULL )
		return FALSE;
	memset( lppd, 0, sizeof( _PROPERTDATA ) );

	lppd->lpPageList = ( LPPTRLIST )malloc( sizeof( PTRLIST ) );
	if( lppd->lpPageList == NULL )
		return FALSE ;
	PtrListCreate( lppd->lpPageList, 3, 3 );

	lppd->lppsh = ( LPPROPSHEETHEADER )malloc( sizeof( PROPSHEETHEADER ) );
	if( lppd->lppsh == NULL )
		return FALSE ;
	memset( lppd->lppsh, 0, sizeof( PROPSHEETHEADER ) );

	GetTwoButtonRect( hWnd, &rt );
	hIns = ( HINSTANCE )GetWindowLong( hWnd, GWL_HINSTANCE );
	lppd->hLeftButton = CreateWindow( classBUTTON, "", WS_CHILD,//|BS_OWNERDRAW,
		rt.left, rt.top + 10, 15, 15, hWnd, NULL, hIns, NULL );
	if( lppd->hLeftButton == 0 )
		return FALSE;
	SetWindowLong( lppd->hLeftButton, GWL_ID , LEFT_BUTTON);

	lppd->hRightButton = CreateWindow( classBUTTON, "", WS_CHILD,//|BS_OWNERDRAW,
		rt.left + 15 , rt.top + 10 , 15, 15, hWnd, NULL, hIns, NULL );
	if( lppd->hRightButton == 0 )
		return FALSE;
	SetWindowLong( lppd->hRightButton, GWL_ID, RIGHT_BUTTON );

	GetClientRect( hWnd, &rt );
	lppd->xSubDlg = 0 ;
	lppd->ySubDlg = 0 ;
	lppd->cxSubDlg = rt.right - rt.left ;
	lppd->cySubDlg = rt.bottom - rt.top - PROPERTY_SHEET_HEIGH ;
	SetWindowLong( hWnd, GWL_USERDATA, ( long )lppd );
	return TRUE ;
}
static void OnPrshtDestroy( HWND hWnd ){
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA			hpsp;
	int						i;
	PSHNOTIFY				pshny;
	lppd = GetPrshtPtr( hWnd );

	for( i = 0 ; i < lppd->lpPageList->count ; i++ )
	{
		hpsp = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, i ) ;
		ASSERT( hpsp != NULL );
		if( hpsp->psp.dwFlags&PSP_USECALLBACK )
		{
			ASSERT( hpsp->psp.pfnCallback != NULL );
			hpsp->psp.pfnCallback( NULL, PSPCB_RELEASE, &( hpsp->psp ) );
		}
		pshny.hdr.hwndFrom = hpsp->hWnd ;
		pshny.hdr.code = PSN_RESET ;
		pshny.hdr.idFrom = GetWindowLong( hpsp->hWnd, GWL_ID );
		SendMessage( hpsp->hWnd, WM_NOTIFY, 0, ( LPARAM )&pshny );
		free( hpsp->psp.pszTitle );
		free( hpsp );
	}
	PtrListDestroy( lppd->lpPageList );
	free( lppd->lpPageList );

	free( lppd->lppsh );
	free( lppd );

}
static BOOL GetTableButtonRect( HWND hWnd,  LPRECT lprt, int iIndex, BOOL isAll )
{
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA			hpsp;
	RECT					rt;
	int						i;
	int						iWidth = 0;
	GetClientRect( hWnd, &rt );
	lppd = GetPrshtPtr( hWnd );
	if( isAll == TRUE )
		i = 0 ;
	else
		i = lppd->iStart ;
	for( ; i < iIndex ; i ++ )
	{
		hpsp = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, i );
		ASSERT( hpsp != NULL );
		iWidth += ( strlen( hpsp->psp.pszTitle ) + 1 ) * CHAR_WIDTH ;
	}
	lprt->left = rt.left + iWidth ;
	hpsp = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, iIndex );
	ASSERT( hpsp != NULL );
	iWidth += ( strlen( hpsp->psp.pszTitle ) + 1 ) * CHAR_WIDTH ;
	lprt->right = rt.left + iWidth ;
	lprt->bottom = rt.bottom ;
//	if( lppd->lppsh->dwFlags&CCS_TOP )
//	{
//		lprt->bottom = lprt->top + PROPERTY_SHEET_HEIGH ;
//	}else
//	{
		lprt->top = lprt->bottom - PROPERTY_SHEET_HEIGH ;
//	}
	if( isAll == TRUE )
		return TRUE ;
	if( rt.right < iWidth )
		return FALSE ;
	return TRUE ;
}
static void OnPrshtLButtonDown( HWND hWnd, WPARAM wParam, short xPos, short yPos )
{
	_LPPROPERTDATA			lppd;
	RECT					rt;
	POINT					pt;
	int						i;
	lppd = GetPrshtPtr( hWnd );
	pt.x = xPos ;
	pt.y = yPos ;
	for( i = lppd->iStart ; i < lppd->lpPageList->count ; i ++ )
	{
		if( GetTableButtonRect( hWnd, &rt, i, FALSE ) == FALSE )
		{
			if( i < ( lppd->lpPageList->count ) )
			{
				lppd->iStart ++ ;
				FlashPrshtPages( hWnd, lppd->iIndex, i );
			}
		}
		if( PtInRect( &rt, pt ) == TRUE )
		{
//			if( i == lppd->iIndex )
//				return ;
			FlashPrshtPages( hWnd, lppd->iIndex, i );
			return ;
		}
	}
}
static void OnPrshtPaint( HWND hWnd )
{
	PAINTSTRUCT				ps;
	HDC						hdc;
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA			hpsp;
	RECT					rt;
	int						i;

	lppd = GetPrshtPtr( hWnd );
	hdc = BeginPaint( hWnd, &ps );
	GetClientRect( hWnd, &rt );
	FillRect( hdc, &rt, GetStockObject( LTGRAY_BRUSH ) );
	rt.top = rt.bottom - PROPERTY_SHEET_HEIGH ;
	rt.bottom -= 1;
	MoveTo( hdc, rt.left, rt.top );
	LineTo( hdc, rt.right, rt.top );

	MoveTo( hdc, rt.left, rt.bottom );
	LineTo( hdc, rt.right, rt.bottom );
	for( i = lppd->iStart ; i < lppd->lpPageList->count ; i ++ )
	{
		GetTableButtonRect( hWnd, &rt, i, FALSE );
		if( lppd->iIndex == i )
		{
			rt.bottom -= 1 ;
			FillRect( hdc, &rt, GetStockObject( WHITE_BRUSH ) );

			SelectObject( hdc, GetStockObject( WHITE_PEN ) );
			rt.top -= 1 ;
			MoveTo( hdc, rt.left, rt.top );
			LineTo( hdc, rt.right, rt.top );
			SelectObject( hdc, GetStockObject( BLACK_PEN ) );
		}
		if( i != lppd->iStart )
		{
			MoveTo( hdc, rt.left, rt.top );
			LineTo( hdc, rt.left, rt.bottom );
		}	
		MoveTo( hdc, rt.right, rt.top );
		LineTo( hdc, rt.right, rt.bottom );

		hpsp = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, i );
		ASSERT( hpsp != NULL );
		SetBkMode( hdc , TRANSPARENT );
		DrawText( hdc, hpsp->psp.pszTitle, strlen( hpsp->psp.pszTitle ), &rt,
			DT_SINGLELINE|DT_VCENTER|DT_CENTER );
	}
	EndPaint( hWnd, &ps );
}
static void MoveSheetButton( HWND hWnd, HWND hCtrl, BOOL isLeft )
{
	_LPPROPERTDATA			lppd;
	RECT					rt;
	int						i;
	lppd = GetPrshtPtr( hWnd );
	if( isLeft ==  TRUE )
	{
		if( lppd->iStart > 0 )
		{
			lppd->iStart -- ;
			if( GetTableButtonRect( hWnd, &rt, lppd->iIndex, FALSE ) == FALSE )
			{
				for( i = lppd->iStart ; i < lppd->iIndex ; i++ )
				{
					if( GetTableButtonRect( hWnd, &rt, i, FALSE ) == FALSE )
						break;
				}
//				FlashPrshtPages( hWnd, lppd->iIndex, i - 1 ) ;
//				return  ;
			}
		}
	}else
	{	
		if( GetTableButtonRect( hWnd, &rt, lppd->lpPageList->count - 1, FALSE ) == FALSE )
		{
			lppd->iStart ++ ;
			if( lppd->iIndex < lppd->iStart )
			{
//				FlashPrshtPages( hWnd, lppd->iIndex, lppd->iStart ) ;
//				return ;
			}
		}
	}
	GetClientRect( hWnd, &rt );
	InvalidateRect( hWnd, &rt, FALSE );
}
static void OnPrshtCommand( HWND hWnd, WPARAM wParam, HWND hCtrl )
{
	switch( LOWORD( wParam ) )
	{
	case LEFT_BUTTON:
		MoveSheetButton( hWnd, hCtrl, TRUE );
		return ;
	case RIGHT_BUTTON:
		MoveSheetButton( hWnd, hCtrl, FALSE );
		return ;
	}
}
static void OnPrshtDrawItem( HWND hWnd, WPARAM wParam, LPDRAWITEMSTRUCT lpdis )
{
	int				iWidth, iHeight;
	HBRUSH			hBrush ;
	iWidth = lpdis->rcItem.right - lpdis->rcItem.left ;
	iHeight = lpdis->rcItem.bottom - lpdis->rcItem.top ;
	switch( LOWORD( wParam ) )
	{
	case LEFT_BUTTON:
		if( lpdis->itemState&ODS_SELECTED )
		{
			hBrush = SelectObject( lpdis->hDC, GetStockObject( WHITE_BRUSH ) );
			Rectangle( lpdis->hDC, 0, 0, iWidth, iHeight );
			SelectObject( lpdis->hDC, hBrush );
		}else
		{
			hBrush = SelectObject( lpdis->hDC, GetStockObject( LTGRAY_BRUSH ) );
			Rectangle( lpdis->hDC, 0, 0, iWidth, iHeight );
			SelectObject( lpdis->hDC, hBrush );
		}
		return ;
	case RIGHT_BUTTON:
		if( lpdis->itemState&ODS_SELECTED )
		{
			hBrush = SelectObject( lpdis->hDC, GetStockObject( WHITE_BRUSH ) );
			Rectangle( lpdis->hDC, 0, 0, iWidth, iHeight );
			SelectObject( lpdis->hDC, hBrush );
		}else
		{
			hBrush = SelectObject( lpdis->hDC, GetStockObject( LTGRAY_BRUSH ) );
			Rectangle( lpdis->hDC, 0, 0, iWidth, iHeight );
			SelectObject( lpdis->hDC, hBrush );
		}
		return ;
	}
}
static BOOL OnPrshtAddPage( HWND hWnd, _LPSHEETPAGEDATA hPage )
{
	_LPPROPERTDATA			lppd;
	HWND					hPrshpage;
	RECT					rtb, rt ;
	ASSERT( hPage != NULL );
	lppd = GetPrshtPtr( hWnd );
	PtrListInsert( lppd->lpPageList, hPage );
	GetTableButtonRect( hWnd, &rtb, lppd->lpPageList->count - 1, TRUE ) ;
	GetClientRect( hWnd, &rt ) ;
	if( rt.right < rtb.right )
	{
		ShowWindow( lppd->hLeftButton, SW_SHOW );
		ShowWindow( lppd->hRightButton, SW_SHOW );
	}
	hPrshpage = CreateDialogIndirect( hPage->psp.hInstance, hPage->psp.pResource, hWnd,
				hPage->psp.pfnDlgProc );
	if( hPrshpage == 0 )
		return FALSE;
	MoveWindow( hPrshpage, lppd->xSubDlg, lppd->ySubDlg, lppd->cxSubDlg, lppd->cySubDlg, TRUE );
	if( lppd->lpPageList->count != 1 )// the first one must be visible !!
		ShowWindow( hPrshpage, SW_HIDE );
	hPage->hWnd = hPrshpage ;

	return TRUE;
}
static void OnPrshtChangeState( HWND hWnd, HWND hPage, BOOL isChanged )
{
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA		lpsd;
	int						i;
	ASSERT( hPage != NULL );
	lppd = GetPrshtPtr( hWnd );
	for( i = 0; i < lppd->lpPageList->count ; i ++ )
	{
		lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, i );
		ASSERT( lpsd != NULL );
		if( hPage == lpsd->hWnd )
		{
			if( isChanged == TRUE )
				lpsd->dwState |= PAGE_DATA_CHANGED ;
			else
				lpsd->dwState &= ~PAGE_DATA_CHANGED ;
			return ;
		}
	}
}
static HWND OnPrshtGetCurrentPageHwnd( HWND hWnd )
{
	_LPPROPERTDATA			lppd;
//	_LPSHEETPAGEDATA		lpsd;
	lppd = GetPrshtPtr( hWnd );
//	if( ( lppd->iIndex < 0 )||( lppd->iIndex >= lppd->lpPageList->count ) )
//		return 0;
//	lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, lppd->iIndex );
//	return lpsd->hWnd ;
	return lppd->hwndCurPage ;
}
static BOOL OnPrshtIsDialogMessage( HWND hWnd, MSG *pmsg )
{
	return SendMessage( hWnd, pmsg->message, pmsg->wParam, pmsg->lParam );
}
static void OnPrshtRemovePage( HWND hWnd, WPARAM wParam,  _LPSHEETPAGEDATA hPage )
{
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA		lpsd;
	RECT					rt, rtb ;
	ASSERT( hPage != NULL );
	lppd = GetPrshtPtr( hWnd );
	if( wParam == lppd->iIndex )
	{
		if( wParam > 0 )
		{
			FlashPrshtPages( hWnd, lppd->iIndex, ( lppd->iIndex - 1 ) );
		}else if( lppd->lpPageList->count > 1 )
		{
			FlashPrshtPages( hWnd, lppd->iIndex, ( lppd->iIndex + 1 ) );
		}
	}else if( wParam < lppd->iIndex )
	{
		lppd->iIndex -- ;
	}
	lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, wParam );
	PtrListAtRemove( lppd->lpPageList, wParam );
	PtrListPack( lppd->lpPageList );
	DestroyPropertySheetPage( hPage );
	lppd->lppsh->nPages -- ;
	GetTableButtonRect( hWnd, &rtb, lppd->lpPageList->count - 1, TRUE ) ;
	GetClientRect( hWnd, &rt ) ;
	if( rt.right > rtb.right )
	{
		ShowWindow( lppd->hLeftButton, SW_HIDE );
		ShowWindow( lppd->hRightButton, SW_HIDE );
	}
}
static void FlashPrshtPages( HWND hWnd, int iOld, int iNew )
{
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA		lpsd;
	PSHNOTIFY				pshny;
	RECT					rt;
	lppd = GetPrshtPtr( hWnd );
	if( iOld == iNew )
	{
		return ;
	}
	lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, lppd->iIndex );
	ASSERT( lpsd != NULL );
	pshny.hdr.hwndFrom = hWnd ;
	pshny.hdr.code = PSN_KILLACTIVE ;
	pshny.hdr.idFrom = GetWindowLong( hWnd, GWL_ID );
	SendMessage( lpsd->hWnd, WM_NOTIFY, 0, ( LPARAM )&pshny );
	ShowWindow( lpsd->hWnd, SW_HIDE );
	
	lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, iNew );
	ASSERT( lpsd != NULL );
	pshny.hdr.hwndFrom = hWnd ;
	pshny.hdr.code = PSN_SETACTIVE ;
	pshny.hdr.idFrom = GetWindowLong( hWnd, GWL_ID );
	SendMessage( lpsd->hWnd, WM_NOTIFY, 0, ( LPARAM )&pshny );
	lppd->iIndex = iNew ;
	lppd->hwndCurPage = lpsd->hWnd ;
	ShowWindow( lpsd->hWnd, SW_SHOW );

	if( iNew < lppd->iStart )
	{
		ASSERT( 1 );
//		lppd->iStart = iNew ;
	}
	GetClientRect( hWnd, &rt );
//	if( lppd->lppsh->dwFlags&CCS_TOP )
//	{
//		rt.bottom = rt.top + PROPERTY_SHEET_HEIGH ;
//	}else
//	{
		rt.top = rt.bottom - PROPERTY_SHEET_HEIGH ;
//	}
	InvalidateRect( hWnd, &rt, FALSE );
}
static BOOL OnPrshtSetCursel( HWND hWnd, WPARAM wParam, _LPSHEETPAGEDATA hPage)
{
	_LPPROPERTDATA			lppd;
	_LPSHEETPAGEDATA		lpsd;
	_LPSHEETPAGEDATA		hsd;
	int						i;
	ASSERT( hPage != NULL );
	lppd = GetPrshtPtr( hWnd );
	if( ( wParam >= 0 )&&( wParam <= lppd->lpPageList->count ) )
	{
		lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList , wParam );
		ASSERT( lpsd != NULL );
		FlashPrshtPages( hWnd, lppd->iIndex, wParam );
		return TRUE ;
	}
	hsd = ( _LPSHEETPAGEDATA )hPage;
	if( hsd == NULL )
		return FALSE;
	for( i = 0 ; i < lppd->lpPageList->count ; i++ )
	{
		lpsd = ( _LPSHEETPAGEDATA )PtrListAt( lppd->lpPageList, i );
		ASSERT( lpsd != NULL );
		if( lpsd->hWnd == hsd->hWnd )
		{
			FlashPrshtPages( hWnd, lppd->iIndex, i );
			return TRUE;
		}
	}
	return FALSE ;
}
static void OnPrshtSetTitle( HWND hWnd, WPARAM wParam, LPCSTR lpcstrText )
{
	TCHAR		lpText[MAX_PATH];
	if( wParam&PSH_PROPTITLE )
	{
		strcpy( lpText, "Properties for" );
		strcat( lpText, lpcstrText );
	}else
	{
		strcpy( lpText, lpcstrText );
	}
	SetWindowText( hWnd, lpText );
}
static void OnPrshtSetData( HWND hWnd, LPCPROPSHEETHEADER lpph ){
	_LPPROPERTDATA			lppd;
	ASSERT( lpph != NULL );
	lppd = GetPrshtPtr( hWnd );
	memcpy( lppd->lppsh, lpph, lpph->dwSize );
}
static LRESULT CALLBACK PropertyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE:
			if( OnPrshtCreate( hWnd ) == FALSE )
				return 1;
			return 0; 
		case WM_COMMAND:
			OnPrshtCommand( hWnd, wParam, ( HWND )lParam );
			return 0;
		case WM_LBUTTONDOWN:
			OnPrshtLButtonDown( hWnd, wParam, LOWORD( lParam ), HIWORD( lParam ) );
			return 0;
		case WM_DRAWITEM:
			OnPrshtDrawItem( hWnd, wParam, ( LPDRAWITEMSTRUCT )lParam );
			return TRUE ;
		case WM_PAINT:
			OnPrshtPaint( hWnd );
			return 0;
		case WM_DESTROY:
			OnPrshtDestroy( hWnd );
			return 0;
		case PTM_SETDATA:
			OnPrshtSetData( hWnd, ( LPCPROPSHEETHEADER )wParam ); 
			return 0;
		case PSM_ADDPAGE:
			return OnPrshtAddPage( hWnd, ( _LPSHEETPAGEDATA ) lParam );
		case PSM_APPLY:
			return 0;
		case PSM_CANCELTOCLOSE:
			return 0;
		case PSM_CHANGED:
			OnPrshtChangeState( hWnd, ( HWND )wParam, TRUE );
			return 0;
		case PSM_GETCURRENTPAGEHWND:
			return ( long )OnPrshtGetCurrentPageHwnd( hWnd );
//		case PSM_GETTABCONTROL:
		case PSM_ISDIALOGMESSAGE:
			return OnPrshtIsDialogMessage( hWnd, ( MSG * )lParam );
		case PSM_PRESSBUTTON:
			return 0;
//		case PSM_QUERYSIBLINGS:
//		case PSM_REBOOTSYSTEM:
		case PSM_REMOVEPAGE:
			OnPrshtRemovePage( hWnd, wParam, ( _LPSHEETPAGEDATA ) lParam );
			return 0;
//		case PSM_RESTARTWINDOWS:
		case PSM_SETCURSEL:
			return OnPrshtSetCursel( hWnd, wParam, ( _LPSHEETPAGEDATA ) lParam );
//		case PSM_SETCURSELID:
//		case PSM_SETFINISHTEXT:
		case PSM_SETTITLE:
			OnPrshtSetTitle( hWnd, wParam, ( LPCSTR )lParam );
			return 0;
//		case PSM_SETWIZBUTTONS:
		case PSM_UNCHANGED:
			OnPrshtChangeState( hWnd, ( HWND )wParam, FALSE );
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
   return 0;
}
ATOM RegistPropertyClass(HINSTANCE hInstance)
{
	WNDCLASS    wc;
	wc.style			= CS_HREDRAW | CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc		= (WNDPROC)PropertyProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject( 0 );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szProPertyClass;
	
	return RegisterClass(&wc);
}
//====================================================
static _LPPROPERTDATA GetPrshtPtr( HWND hWnd )
{
	_LPPROPERTDATA		lppd ;
	lppd = ( _LPPROPERTDATA )GetWindowLong( hWnd, GWL_USERDATA );
	ASSERT( lppd != NULL );
	return lppd ;
}
HPROPSHEETPAGE WINAPI CreatePropertySheetPage( LPCPROPSHEETPAGE lpPrshpg )
{
	_LPSHEETPAGEDATA			hPrshtp;
	ASSERT( lpPrshpg != NULL );
//	if( lpPrshpg->dwFlags&
	hPrshtp = ( _LPSHEETPAGEDATA )malloc( sizeof( _SHEETPAGEDATA ) );
	if( hPrshtp == NULL )
		return 0;
	memcpy( &( hPrshtp->psp ), lpPrshpg, lpPrshpg->dwSize );
	hPrshtp->psp.pszTitle = ( TCHAR * )malloc( sizeof( TCHAR ) * ( strlen( lpPrshpg->pszTitle ) + 1 ) );
	if( hPrshtp->psp.pszTitle == NULL ){
		free( hPrshtp );
		return 0;
	}
	strcpy( hPrshtp->psp.pszTitle, lpPrshpg->pszTitle );
	if( lpPrshpg->dwFlags&PSP_USECALLBACK )
	{
		ASSERT( lpPrshpg->pfnCallback != NULL );
		if( lpPrshpg->pfnCallback( NULL, PSPCB_CREATE, &( hPrshtp->psp ) ) == 0 )
		{
			free( hPrshtp->psp.pszTitle );
			free( hPrshtp );
			return 0;
		}
	}
	return hPrshtp ;
}
BOOL   WINAPI DestroyPropertySheetPage( HPROPSHEETPAGE hPrshp)
{
	_LPSHEETPAGEDATA		lpstpd;
	PSHNOTIFY				pshny;

	lpstpd = ( _LPSHEETPAGEDATA )hPrshp;
	ASSERT( lpstpd != NULL );
	if( lpstpd->psp.dwFlags&PSP_USECALLBACK )
	{
		ASSERT( lpstpd->psp.pfnCallback != NULL );
		lpstpd->psp.pfnCallback( NULL, PSPCB_RELEASE, &( lpstpd->psp ) );
	}
	pshny.hdr.hwndFrom = lpstpd->hWnd ;
	pshny.hdr.code = PSN_RESET ;
	pshny.hdr.idFrom = GetWindowLong( lpstpd->hWnd, GWL_ID );
	SendMessage( lpstpd->hWnd, WM_NOTIFY, 0, ( LPARAM )&pshny );
	DestroyWindow( lpstpd->hWnd );
	free( lpstpd->psp.pszTitle );
	free( lpstpd );
	return TRUE ;
}
int     WINAPI PropertySheet( LPCPROPSHEETHEADER lpPrshHeader )
{
	HWND						hWnd ;
	RECT						rt;
	unsigned int				i;
	HPROPSHEETPAGE				hPsp;
	MSG							msg;
	GetClientRect( lpPrshHeader->hwndParent, &rt );
	if( !RegistPropertyClass( lpPrshHeader->hInstance ) )
		return FALSE;
	if( lpPrshHeader->dwFlags&PSH_USECALLBACK )
	{
		lpPrshHeader->pfnCallback( NULL, PSCB_INITIALIZED, 0 );
		lpPrshHeader->pfnCallback( NULL, PSCB_PRECREATE, (long)&dlgIDD_PROPERTY );
	}
	if( lpPrshHeader->dwFlags&PSH_MODELESS )
	{
		rt.bottom -= 25 ;
	}
	hWnd = CreateWindow( szProPertyClass, "" ,WS_VISIBLE|WS_CHILD, 
		rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top,
		lpPrshHeader->hwndParent, NULL, lpPrshHeader->hInstance, NULL );
	if (!hWnd)
	{
		return FALSE;
	}
	SendMessage( hWnd, PTM_SETDATA, ( WPARAM )lpPrshHeader, 0 );
	if( lpPrshHeader->dwFlags&PSH_PROPSHEETPAGE )
	{
		for( i = 0 ; i < lpPrshHeader->nPages ; i++ )
		{
			hPsp = CreatePropertySheetPage( &( lpPrshHeader->ppsp[i] ) );
			PropSheet_AddPage( hWnd, hPsp );
		}
	}else{
		for( i = 0 ; i < lpPrshHeader->nPages ; i++ )
		{
			PropSheet_AddPage( hWnd, lpPrshHeader->phpage[i] );
		}
	}
	if( lpPrshHeader->dwFlags&PSH_MODELESS )
	{
		return ( int )hWnd ;
	}else
	{
		while (GetMessage(&msg, 0, 0, 0)) 
		{	
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return TRUE;
	}
}
