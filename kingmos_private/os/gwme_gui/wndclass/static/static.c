/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������̬�� static class
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <estatic.h>
//#include <gwmesrv.h>

// ����

typedef struct _STATIC_ATTRIB
{
	HANDLE   hImage;
    COLORREF cl_Text;//�����ı���ǰ��
    COLORREF cl_TextBk;     //�����ı���ǰ���뱳��ɫ
	COLORREF cl_Disable;    // ��Ч�ı���ǰ��ɫ
	COLORREF cl_DisableBk;    // ��Ч�ı��ı���ɫ
}STATIC_ATTRIB, * PSTATIC_ATTRIB;

static const char strClassStatic[]="STATIC";

static LRESULT CALLBACK StaticWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static void DoPAINT( HWND hwnd );
static VOID _SetStaticColor( PSTATIC_ATTRIB pAttrib );


// **************************************************
// ������ATOM RegisterButtonClass(HINSTANCE hInst)
// ������
//		IN hInst - ʵ�����
// ����ֵ��
//		����ɹ������ط�0ֵ��ʧ�ܣ�����0
// ����������
//		ע�ᰴť��
// ����: 
//		��sysclass.c ����
// ************************************************
ATOM RegisterStaticClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = StaticWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( PSTATIC_ATTRIB );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = 0;//LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = NULL;//GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = strClassStatic;

    return (BOOL)(RegisterClass( &wc ));
}

// **************************************************
// ������static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// ������
//	IN hdc - ��ʾ�豸���
//	IN lprc - RECT �ṹָ�룬ָ����Ҫ���ľ���
//	IN clr - RGB��ɫֵ
// ����ֵ��
//	��
// ����������
//	�ù̶���ɫ�����о���
// ����: 
//	
// ************************************************
static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

// **************************************************
// ������static void DoDraw( HWND hwnd, HDC hdc, DWORD dwStyle )
// ������
//	IN hwnd - ���ھ��
//	IN hdc - ��ʾ�豸���
//	IN dwStyle - ���ڷ��
// ����ֵ��
//	��
// ����������
//	�ù̶���ɫ�����о���
// ����: 
//	
// ************************************************
static void DoDraw( HWND hwnd, HDC hdc, DWORD dwStyle )
{
	HBITMAP hBitmap;
	RECT rect;
	HDC hMemDC;
	HPEN hPen;
    BITMAP bitmap;
    int i;
    DWORD dwDrawStyle;//prefix;
	PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hwnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	if( (dwStyle & 0x1f) == SS_OWNERDRAW  )
    {	// �Ի���
	    DRAWITEMSTRUCT di;
        di.CtlID = (WORD)GetWindowLong( hwnd, GWL_ID );
        di.CtlType = ODT_STATIC;
        di.hDC = hdc;
        di.hwndItem = hwnd;
        di.itemAction = ODA_DRAWENTIRE;
        di.itemData = 0;
        di.itemID = 0;
        di.itemState = ODS_DEFAULT;
        GetClientRect( hwnd, &di.rcItem );	// �õ����ڿͻ�����������
		// �򸸴��ڷ���֪ͨ��Ϣ
        SendMessage( GetParent( hwnd ), WM_DRAWITEM, (WPARAM)di.CtlID, (LPARAM)&di );
        return;
    }
	GetClientRect( hwnd, &rect );// �õ����ڿͻ�����������
    
    if( dwStyle & SS_NOPREFIX )	// �ؼ�����ǰ׺�ַ��� �����磺"&Hello" ��'&'
        dwDrawStyle = DT_NOPREFIX;	// û��ǰ׺�ַ�
    else
        dwDrawStyle = 0;
	dwDrawStyle |= DT_END_ELLIPSIS;	// ��ʡ�Ժű�ʾ�޷���ʾ���֣����磺"Hello,W..."

	switch( (dwStyle & 0x1f) )
	{
	case SS_CENTER:			// �ı�����
	    dwDrawStyle |= DT_CENTER | DT_WORDBREAK;
		break;
	case SS_LEFT:			// �ı������
        dwDrawStyle	|= DT_LEFT | DT_WORDBREAK;
		break;
	case SS_RIGHT:			// �ı��Ҷ���
		dwDrawStyle	|= DT_RIGHT | DT_WORDBREAK;
		break;
	case SS_LEFTNOWORDWRAP:		// 
		dwDrawStyle	|= DT_LEFT;
	}

	switch( dwStyle & 0x1f )
	{
	case SS_BITMAP:			// λͼ���
		hBitmap = pAttrib->hImage;
        GetObject( hBitmap, sizeof(bitmap), &bitmap );
		hMemDC =  CreateCompatibleDC( hdc );
		hBitmap = SelectObject( hMemDC, hBitmap );
        BitBlt( hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY );
        hBitmap = SelectObject( hMemDC, hBitmap );
		DeleteDC( hMemDC );
		break;
    case SS_ICON:			// ͼ����
        DrawIcon( hdc, 0, 0, pAttrib->hImage );
        break;
	case SS_BLACKFRAME:		// ��ɫ�߿�
		SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
		break;
	case SS_BLACKRECT:		// ��ɫ����
        FillRect( hdc, &rect, GetStockObject( BLACK_BRUSH ) );
        break;
	case SS_WHITEFRAME:		// ��ɫ�߿�
		SelectObject( hdc, GetStockObject( WHITE_PEN ) );
        SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
        break;
	case SS_WHITERECT:		// ��ɫ����
		FillRect( hdc, &rect, GetStockObject( WHITE_BRUSH ) );
        break;
	case SS_GRAYFRAME:		// ��ɫ�߿�
        hPen = CreatePen( PS_SOLID, 1, CL_LIGHTGRAY );
        hPen = SelectObject( hdc, hPen );
        SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
		Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
        hPen = SelectObject( hdc, hPen );
        DeleteObject( hPen );
        break;
	case SS_GRAYRECT:		// ��ɫ����
        FillRect( hdc, &rect, GetStockObject( GRAY_BRUSH ) );
		break;
	case SS_CENTER:
    case SS_LEFT:
	case SS_RIGHT:
	case SS_LEFTNOWORDWRAP:
		SetBkMode( hdc, TRANSPARENT );		// �����ı��ı���Ϊ͸��ģʽ
		i = GetWindowTextLength(hwnd);
		i += sizeof(TCHAR); //+0������
		if( i )
		{
			TCHAR szText[128];
			LPTSTR lpstr;
			if( i <= sizeof(szText) )
				lpstr = szText;
		    else
			    lpstr = (LPSTR)malloc( i );   // ��Ҫ�Ժ�Ľ�
			if( lpstr )
			{
				i = GetWindowText( hwnd, lpstr, i );	// �õ��ı�
				// ������Ҫ��ʾ���ı���ɫ
				if( dwStyle & WS_DISABLED )			// 
					SetTextColor( hdc, pAttrib->cl_Disable );
				else
				    SetTextColor( hdc, pAttrib->cl_Text );
				// ���ı�
				DrawText( hdc,
					lpstr,
					i,
					&rect,
					dwDrawStyle );
				if( lpstr != szText )
				    free(lpstr);	//��malloc�����
			}
		}
		break;
	}
}

// **************************************************
// ������static LRESULT DoLButtonDown( HWND hWnd ) //, short x, short y )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	���� WM_LBUTTONDOWN ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoLButtonDown( HWND hWnd ) //, short x, short y )
{
	HWND hParent = GetParent( hWnd );
	if( hParent )
	{	// �õ��ؼ����
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		if( dwStyle & SS_NOTIFY )	// �Ƿ�֪ͨ��Ϣ ��
		{	// ��
			WPARAM wParam = (WPARAM)GetWindowLong( hWnd, GWL_ID ); // �ؼ�ID
			wParam = MAKELONG( wParam, STN_CLICKED );		// ֪ͨ��Ϣ����
			//	�򸸴��ڷ�֪ͨ��Ϣ
			return SendMessage( hParent, WM_COMMAND, wParam, (LPARAM)hWnd );
		}
	}
	return 0;
}

// **************************************************
// ������static LRESULT DoEnable( HWND hWnd, BOOL bEnable )
// ������
//	IN hWnd - ���ھ��
//	IN bEnable - �ؼ��Ƿ�����Ч״̬
// ����ֵ��
//	����0
// ����������
//	���� WM_ENABLE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoEnable( HWND hWnd, BOOL bEnable )
{
	HWND hParent = GetParent( hWnd );
	if( hParent )
	{	// �õ��ؼ����
		DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
		HDC hdc = GetDC( hWnd );
		DoDraw( hWnd, hdc, dwStyle );
		ReleaseDC( hWnd, hdc );
		if( dwStyle & SS_NOTIFY )	// �Ƿ�֪ͨ��Ϣ ��
		{
			WPARAM wParam = (WPARAM)GetWindowLong( hWnd, GWL_ID );// �ؼ�ID
			// ֪ͨ��Ϣ����
			if( bEnable )
				wParam = MAKELONG( wParam, STN_ENABLE );
			else
				wParam = MAKELONG( wParam, STN_DISABLE );
			//	�򸸴��ڷ�֪ͨ��Ϣ
			return SendMessage( hParent, WM_COMMAND, wParam, (LPARAM)hWnd );
		}
	}
	return 0;
}

// **************************************************
// ������static LRESULT SetImage( HWND hwnd, UINT dwStyle, HANDLE hImage )
// ������
//	IN hWnd - ���ھ��
//	IN dwStyle - ���ڷ��
//	IN hImage - ͼ�ξ��
// ����ֵ��
//	����֮ǰ��ͼ�ξ��
// ����������
//	���ÿؼ���ʾӳ��
// ����: 
//	
// ************************************************

static LRESULT SetImage( HWND hwnd, DWORD dwStyle, HANDLE hImage )
{
    HANDLE handle;
    //DWORD dwStyle;
	PSTATIC_ATTRIB pAttrib;

    pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hwnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	handle = pAttrib->hImage;
	pAttrib->hImage = hImage;

	//dwStyle = GetWindowLong( hwnd, GWL_STYLE ); // �õ����ڷ�� 
	// ������ܣ����»���
	if( (dwStyle & WS_VISIBLE) == WS_VISIBLE )	// ������ ��
	{	// ���ӣ����ƴ��ڿͻ���
		HDC hdc;
		// 
        hdc = GetDC( hwnd );	// �õ���ʾ�豸DC
		DoDraw( hwnd, hdc, dwStyle );	// ʵ�ʵĻ���
		ReleaseDC( hwnd, hdc );	// �ͷ�DC
	}
    return (LRESULT)handle;
}

// **************************************************
// ������static LRESULT DoSetImage( HWND hwnd, UINT uiType, HANDLE hImage )
// ������
//	IN hwnd - ���ھ��
//	IN uiType - ͼ�ζ��������
//	IN hImage - ͼ�ξ��
// ����ֵ��
//	����ɹ�������֮ǰ��ͼ�ξ�������򣬷���NULL
// ����������
//	���ÿؼ���ʾӳ�󣬴��� STM_SETICON �� STM_SETIMAGE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoSetImage( HWND hwnd, UINT uiType, HANDLE hImage )
{
	DWORD dwStyle;

	dwStyle = GetWindowLong( hwnd, GWL_STYLE );// �õ����ڷ��
	switch( uiType )
	{
	case IMAGE_BITMAP:
		if( dwStyle & SS_BITMAP )	// ӳ�������봰�ڷ���Ƿ�һ�� ��
		{	// ��
			return SetImage( hwnd, dwStyle, hImage );
		}
		break;
//	case IMAGE_CURSOR:
	case IMAGE_ICON:
		if( dwStyle & SS_ICON )	// ӳ�������봰�ڷ���Ƿ�һ�� ��
		{	// ��
			return SetImage( hwnd, dwStyle, hImage );
		}
		break;
	}
	return NULL;	// 
}

// **************************************************
// ������static LRESULT GetImage( HWND hwnd, UINT uiType )
// ������
//	IN hwnd - ���ھ��
//	IN uiType - ͼ�ζ��������
// ����ֵ��
//	����ɹ������ص�ǰ��ͼ�ξ�������򣬷���NULL
// ����������
//	�õ��ؼ���ʾӳ��
// ����: 
//	
// ************************************************

static LRESULT GetImage( HWND hwnd, UINT uiType )
{
    return (LRESULT)((PSTATIC_ATTRIB)GetWindowLong( hwnd, 0 ))->hImage;
}

// **************************************************
// ������static LRESULT DoGetImage( HWND hwnd, UINT uiType )
// ������
//	IN hwnd - ���ھ��
//	IN uiType - ͼ�ζ��������
// ����ֵ��
//	����ɹ������ص�ǰ��ͼ�ξ�������򣬷���NULL
// ����������
//	�õ��ؼ���ʾӳ�󣬴��� STM_GETICON �� STM_GETIMAGE ��Ϣ
// ����: 
//	
// ************************************************
static LRESULT DoGetImage( HWND hwnd, UINT uiType )
{    
	DWORD dwStyle;

	dwStyle = GetWindowLong( hwnd, GWL_STYLE );// �õ����ڷ��
	switch( uiType )
	{
	case IMAGE_BITMAP:
		if( dwStyle & SS_BITMAP )// ӳ�������봰�ڷ���Ƿ�һ�� ��
		{
			return GetImage( hwnd, uiType );
		}
		break;
//	case IMAGE_CURSOR:
	case IMAGE_ICON:
		if( dwStyle & SS_ICON )	// ӳ�������봰�ڷ���Ƿ�һ�� ��
		{
			return GetImage( hwnd, uiType );
		}
		break;
	}
	return NULL;
}

// **************************************************
// ������static void DoPAINT( HWND hwnd )
// ������
//	IN hwnd - ���ھ��
// ����ֵ��
//	��
// ����������
//	���ƿͻ��������� WM_PAINT ��Ϣ
// ����: 
//	
// ************************************************
static void DoPAINT( HWND hwnd )
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint( hwnd, &ps );	// �õ�����DC
	DoDraw( hwnd, hdc, GetWindowLong( hwnd, GWL_STYLE ) );
	EndPaint( hwnd, &ps );	// �ͷ�DC
}

// **************************************************
// ������static void DoEraseBkground( HWND hWnd, HDC hdc, DWORD dwStyle )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
// ����ֵ��
//	��
// ����������
//	���ƿͻ�������
// ����: 
//	
// ************************************************
static void DoEraseBkground( HWND hWnd, HDC hdc, DWORD dwStyle  )
{
	RECT rcClient;
	PSTATIC_ATTRIB psa = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );
	//DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	
	GetClientRect( hWnd, &rcClient );
	//FillRect( (HDC)wParam, &rcClient, GetSysColorBrush(COLOR_STATIC) );
	if( dwStyle & WS_DISABLED )
		FillSolidRect( hdc, &rcClient, psa->cl_DisableBk );
	else
		FillSolidRect( hdc, &rcClient, psa->cl_TextBk );
}

// **************************************************
// ������static void RedrawWindow( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	��
// ����������
//	���»��ƿͻ���
// ����: 
//	
// ************************************************

static void RedrawWindow( HWND hWnd )
{
	HDC hdc = GetDC( hWnd );
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	DoEraseBkground( hWnd, hdc, dwStyle );
	DoDraw( hWnd, hdc, dwStyle );
	ReleaseDC( hWnd, hdc );
}

// **************************************************
// ������static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// ������
//	IN hWnd - ���ھ��
//	IN lpccs - �ؼ��ṹ��������ɫֵ
// ����ֵ��
//	�ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���ÿؼ���ɫ,���� WM_SETCTLCOLOR ��Ϣ
// ����: 
//	
// ************************************************
static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );

		if( lpccs->fMask & CLF_TEXTCOLOR )
		{
			pAttrib->cl_Text = lpccs->cl_Text;	// ��Ч�ı���ɫ
		}
		if( lpccs->fMask & CLF_TEXTBKCOLOR )
		{
			pAttrib->cl_TextBk = lpccs->cl_TextBk;	// ��Ч�ı�������ɫ
		}
		if( lpccs->fMask & CLF_DISABLECOLOR )
			pAttrib->cl_Disable = lpccs->cl_Disable;	// ��Ч�ı���ɫ 
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )
			pAttrib->cl_DisableBk = lpccs->cl_DisableBk;	// ��Ч�ı�������ɫ
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// ������
//	IN hWnd - ���ھ��
//	IN lpccs - �ؼ��ṹ��������ɫֵ
// ����ֵ��
//	�ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�õ��ؼ���ɫ������ WM_GETCTLCOLOR ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );

		if( lpccs->fMask & CLF_TEXTCOLOR )	// ��Ч�ı���ɫ
			lpccs->cl_Text = pAttrib->cl_Text;
		if( lpccs->fMask & CLF_TEXTBKCOLOR )	// ��Ч�ı�������ɫ
			lpccs->cl_TextBk = pAttrib->cl_TextBk;
		if( lpccs->fMask & CLF_DISABLECOLOR )	// ��Ч�ı���ɫ
			lpccs->cl_Disable = pAttrib->cl_Disable;
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )	// ��Ч�ı�������ɫ
			 lpccs->cl_DisableBk = pAttrib->cl_DisableBk;
		return TRUE;
	}
	return FALSE;	
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
    PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );

	_SetStaticColor( pAttrib );
	return 0;
}

// **************************************************
// ������static VOID _SetStaticColor( PSTATIC_ATTRIB pAttrib )
// ������
//	IN pAttrib - ��̬�����ݽṹ
// ����ֵ��
//	��
// ����������
//	���ô�����ʾ��ɫ
// ����: 
//	
// ************************************************

static VOID _SetStaticColor( PSTATIC_ATTRIB pAttrib )
{
	pAttrib->cl_Text = GetSysColor( COLOR_STATICTEXT );
	pAttrib->cl_TextBk = GetSysColor( COLOR_STATIC );
	pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
	pAttrib->cl_DisableBk = pAttrib->cl_TextBk;
}

// **************************************************
// ������static LRESULT DoCreate( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�������0; ���򣬷���-1
// ����������
//	��ʼ���������ݡ����� WM_CREATE ��Ϣ
// ����: 
//	
// ************************************************
static LRESULT DoCreate( HWND hWnd )
{	// ����ؼ�˽�нṹ
	PSTATIC_ATTRIB pAttrib = malloc( sizeof(STATIC_ATTRIB) );
	if( pAttrib )
	{
		memset( pAttrib, 0, sizeof(STATIC_ATTRIB) );
		_SetStaticColor( pAttrib );
		//pAttrib->cl_Text = GetSysColor( COLOR_STATICTEXT );
		//pAttrib->cl_TextBk = GetSysColor( COLOR_STATIC );
		//pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
		//pAttrib->cl_DisableBk = pAttrib->cl_TextBk;
		SetWindowLong( hWnd, 0, (LONG)pAttrib );
		return 0;  // �ɹ�
	}
	return -1;  // ʧ��
}

// **************************************************
// ������static LRESULT DoDestroy( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	�ƻ��������ݡ����� WM_DESTROY ��Ϣ
// ����: 
//	
// ************************************************
static LRESULT DoDestroy( HWND hWnd )
{	// �ͷſؼ�˽�нṹ��֮ǰ�� DoCreate �ﴴ����
	PSTATIC_ATTRIB pAttrib = (PSTATIC_ATTRIB)GetWindowLong( hWnd, 0 );
	free( pAttrib );
	return 0;
}

// **************************************************
// ������LRESULT WINAPI StaticWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
//  IN hWnd - ���ھ��
//	IN message - ��Ϣ
//	IN wParam - ��һ����Ϣ
//	IN lParam - �ڶ�����Ϣ
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	��ť���ڴ��������
// ����: 
//	
// ************************************************

LRESULT CALLBACK StaticWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_PAINT:			// ���ƿͻ�����Ϣ
		DoPAINT( hwnd );
		return 0;
    case WM_LBUTTONDOWN:			// ����갴����Ϣ
        return DoLButtonDown( hwnd );//, LOWORD( lParam ), HIWORD( lParam ) );
    case WM_ENABLE:
        return DoEnable( hwnd, (BOOL)wParam );
	case STM_SETICON:	
        return DoSetImage( hwnd, IMAGE_ICON, (HANDLE)wParam );
	case STM_SETIMAGE:
        return DoSetImage( hwnd, wParam, (HANDLE)lParam );
	case STM_GETICON:
		return DoGetImage( hwnd, IMAGE_ICON );
	case STM_GETIMAGE:
		return DoGetImage( hwnd, wParam );
    case WM_SETTEXT:
        DefWindowProc( hwnd, uMsg, wParam, lParam );
        //InvalidateRect( hwnd, 0, TRUE );  // ��Ч�ͻ������ػ�
		RedrawWindow( hwnd );
        return 0;
	case WM_ERASEBKGND:		// ����ͻ�������
		{			
			DoEraseBkground( hwnd, (HDC)wParam, GetWindowLong( hwnd, GWL_STYLE ) );
		}
		return 1;
    case WM_SETCTLCOLOR:
		return DoSetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hwnd, (LPCTLCOLORSTRUCT)lParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hwnd );
	case WM_CREATE:
		return DoCreate( hwnd );
	case WM_DESTROY:
		return DoDestroy( hwnd );
	default:
	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// ��Ҫ��ʾ�������㲻��Ҫ������Ϣ��������Ϣ����ϵͳ����Ĭ�ϴ�����ȥ����
	// 
    	return DefWindowProc( hwnd, uMsg, wParam, lParam );	// ϵͳĬ�ϴ���
	}
	return 0;
}
