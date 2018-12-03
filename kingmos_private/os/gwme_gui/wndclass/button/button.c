/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������ť��
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <esymbols.h>
#include <eassert.h>
//#include <gwmesrv.h>


#define GETTYPE( dwStyle ) ( (WORD)((dwStyle)&0xf) )

#define ICON_SIZE 16
#define GSB_CHECKNORMAL 0
#define GSB_CHECKSELECT 1
#define GSB_RADIONORMAL 2
#define GSB_RADIOSELECT 3

// ��ť��־
#define BTF_DISABLEFOCUS  0x0001

typedef struct _BUTTON_ATTRIB{
    WORD btState;  // OFFSET_STATE 0
	WORD btFlag;   //	��ť��־
    HANDLE hImage;  // OFFSET_IMAGE 4
	COLORREF cl_Text;
	COLORREF cl_TextBk;     //�����ı���ǰ���뱳��ɫ
	COLORREF cl_Disable;
	COLORREF cl_DisableBk;    // ��Ч�ı���ǰ���뱳��ɫ
}BUTTON_ATTRIB, * PBUTTON_ATTRIB;

static void DoOwnerDraw( HDC, HWND hWnd, DWORD state, UINT uiAction );
static void DrawCheckState( HDC, HWND hWnd, DWORD state );
static void DrawPushState( HDC, HWND hWnd, BOOL fDefault, DWORD state );
static void DrawButtonState( HDC, HWND hWnd, DWORD style, DWORD state );

static LRESULT DoPAINT( HWND hWnd );
static LRESULT DoLBUTTONDOWN( HWND hWnd );
static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y );
static LRESULT DoSETFOCUS( HWND hWnd );
static LRESULT DoSETSTATE( HWND hWnd, WPARAM wParam );
static LRESULT DoSETCHECK( HWND hWnd, WPARAM wParam );
static LRESULT WINAPI ButtonWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

// ����
static const char strClassButton[] = "BUTTON"; 

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
// register my define class
ATOM RegisterButtonClass(HINSTANCE hInst)
{
	WNDCLASS wc;
	
	//��ʼ����ṹ
	wc.hInstance=hInst;
	wc.lpszClassName= strClassButton;
	// the proc is class function
	wc.lpfnWndProc=(WNDPROC)ButtonWndProc;
	wc.style=CS_DBLCLKS|CS_CLASSDC;
	wc.hIcon= 0;
	// ���ڱʵĴ��ڣ�û�й��at pen window, no cursor
	wc.hCursor= 0;//LoadCursor(NULL,IDC_ARROW);
	// to auto erase background, must set a valid brush
	// if 0, you must erase background yourself
	wc.hbrBackground = 0;
	wc.lpszMenuName=0;
	wc.cbClsExtra=0;
	// !!! it's important to save state of button, align to long
	wc.cbWndExtra= sizeof( PBUTTON_ATTRIB );
	
	return RegisterClass(&wc);
}

//#define SEND_NOTIFY( hParent, idCtrl, idNotify, hWnd ) \ 
  //      SendMessage( (hParent), WM_COMMAND, MAKELONG( (idCtrl),  (idNotify) ), (LPARAM)(hWnd) )
// **************************************************
// ������static int SendNotify( HWND hParent, UINT uiId, UINT uiNotify, HWND hWnd )
// ������
//	IN hParent - ������
//	IN uiId - ����id
//	IN uiNotify - ֪ͨ��Ϣ
//	IN hWnd - ���ھ��
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	�򸸴��ڷ���֪ͨ��Ϣ
// ����: 
//	
// ************************************************

static int SendNotify( HWND hParent, UINT uiId, UINT uiNotify, HWND hWnd )
{
    return SendMessage( hParent, WM_COMMAND, MAKELONG( uiId, uiNotify ), (LPARAM)hWnd );
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
// ������static void GetDrawRect(	HDC hdc, 
//									LPCRECT lpClientRect, 
//									LPRECT lpTextRect, 
//									LPRECT lpIconRect, 
//									DWORD dwStyle )
// ������
//	IN hdc - ��ʾ�豸���
//	IN lpClientRect - ����ָ�룬ָ�򴰿ڵĿͻ�����
//	OUT lpTextRect - ����ָ�룬���ڽ��ܰ�ť���ı�����
//	OUT lpIconRect - ����ָ�룬���ڽ��ܰ�ť��ͼ�����
//	IN dwStyle - ���ڷ��
// ����ֵ��
//	��
// ����������
//	�õ���ť���ڵ��ı����־��κ�ͼ�겿�־���
// ����: 
//	
// ************************************************

static void GetDrawRect( HDC hdc, LPCRECT lpClientRect, LPRECT lpTextRect, LPRECT lpIconRect, DWORD dwStyle )
{
    int type  = GETTYPE( dwStyle );  // �õ���ť���
    
    *lpTextRect = *lpIconRect = *lpClientRect;
    
    if( type == BS_DEFPUSHBUTTON ||
        type == BS_PUSHBUTTON ||
        (dwStyle & BS_PUSHLIKE) )
    {	// ��ť���
        if( type == BS_DEFPUSHBUTTON )
        {	// Ĭ�ϰ�ť���
            InflateRect( lpTextRect, -1, -1 );
        }
        InflateRect( lpTextRect, -1, -1 );
    }
    else if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON ||
            type == BS_CHECKBOX || type == BS_AUTOCHECKBOX )
    {	// ��ѡ���Զ�����ť �� ��ѡ���Զ�����ť
        if( dwStyle & BS_LEFTTEXT )
        {	// ��ť�ı�������, ��[ ButtonText <icon> ] 
            lpIconRect->left = lpIconRect->right - ICON_SIZE;
            lpTextRect->right = lpIconRect->left;// - 2;
        }
        else
        {	// ��ť�ı�������, ��[ <icon> ButtonText ] 
            lpIconRect->right = lpIconRect->left + ICON_SIZE;
            lpTextRect->left = lpIconRect->right;// + 2;
        }
		// ����ͼ������±�
        lpIconRect->top = ( lpClientRect->bottom + lpClientRect->top) / 2 - ICON_SIZE / 2;
        lpIconRect->bottom = lpIconRect->top + ICON_SIZE;
    }
    else if( type == BS_GROUPBOX )
    {	// ���
        TEXTMETRIC tm;
        GetTextMetrics( hdc, &tm );
 
        lpTextRect->left += 8;
        lpTextRect->right -= 8;
        lpTextRect->bottom = (short)(lpTextRect->top + tm.tmHeight);
    }
}

// **************************************************
// ������static void DrawRadioBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
// ������
//	IN hdc - ��ʾ�豸���
//	IN lprectIcon - ����ѡ��ťicon�ľ���
//	IN state - ��ѡ��ť��״̬
// ����ֵ��
//	��
// ����������
//	����ѡ��ť
// ����: 
//	
// ************************************************

static void DrawRadioBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
{
	int iOldMode;
	BYTE bIcon;

	HFONT hfont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );
    iOldMode = SetBkMode( hdc, TRANSPARENT );

    if( state & BST_CHECKED )
        bIcon = SYM_RADIO_SET;
	else
		bIcon = SYM_RADIO_NOSET;
	DrawText( hdc, (LPCTSTR)&bIcon, 1, (LPRECT)lprectIcon, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
    SetBkMode( hdc, iOldMode );
	SelectObject( hdc, hfont );
/*
	HDC hMemDC;
    HBITMAP hBitmap;

	hMemDC = CreateCompatibleDC( hdc );
	if( state & BST_CHECKED )
	    hBitmap = (HBITMAP)SelectObject( hMemDC, GetStockBitmap( GSB_RADIOSELECT ) );
	else
	    hBitmap = (HBITMAP)SelectObject( hMemDC, GetStockBitmap( GSB_RADIONORMAL ) );
	BitBlt( hdc, lprectIcon->left, lprectIcon->top, 8, 8, hMemDC, 0, 0, SRCCOPY );
    SelectObject( hMemDC, hBitmap );
	DeleteDC( hMemDC );
*/
}

// **************************************************
// ������static void DrawCheckBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
// ������
//  IN hdc -  ��ʾ�豸���
//	IN lprectIcon - ��icon�ľ���
//	IN state - ��ѡ��ť��״̬
// ����ֵ��
//	��
// ����������
//	����ѡ��ť
// ����: 
//	
// ************************************************

static void DrawCheckBox( HDC hdc, LPCRECT lprectIcon, DWORD state )
{
	int iOldMode;
	BYTE bIcon;
	HFONT hfont = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );
    iOldMode = SetBkMode( hdc, TRANSPARENT );

    if( state & BST_CHECKED )
        bIcon = SYM_CHECK_SET;
	else
		bIcon = SYM_CHECK_NOSET;
	DrawText( hdc, (LPCTSTR)&bIcon, 1, (LPRECT)lprectIcon, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	SetBkMode( hdc, iOldMode );
	SelectObject( hdc, hfont );

/*
	HDC hMemDC;
    HBITMAP hBitmap;

	hMemDC = CreateCompatibleDC( hdc );
	if( state & BST_CHECKED )
	    hBitmap = (HBITMAP)SelectObject( hMemDC, GetStockBitmap( GSB_CHECKSELECT ) );
	else
	    hBitmap = (HBITMAP)SelectObject( hMemDC, GetStockBitmap( GSB_CHECKNORMAL ) );
	BitBlt( hdc, lprectIcon->left, lprectIcon->top, 8, 8, hMemDC, 0, 0, SRCCOPY );
    SelectObject( hMemDC, hBitmap );
	DeleteDC( hMemDC );
*/
}

// **************************************************
// ������static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
// ������
//  IN hdc -  ��ʾ�豸���
//	IN hWnd - ���ھ��
//	IN style - ���ڷ��
//	IN state - ״̬
// ����ֵ��
//	��	
// ����������
//	����ť��������check button, radio button...��
// ����: 
//	
// ************************************************

static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
{
	WORD type = GETTYPE( style );
	RECT rectClient, rectText, rectIcon;
	HBRUSH hBrush;
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 ); // �õ���ť���Խṹָ��
	BOOL bEnable = IsWindowEnabled( hWnd ); // 
	char strBuf[128];
	char * buf = NULL;
	int l, shift = 0;
	WORD textStyle = 0;

	GetClientRect( hWnd, &rectClient );  // �õ����ڿͻ�����
	l = GetWindowTextLength( hWnd );	// �õ������ı�����
	if( l < sizeof( strBuf ) )
	{		
	    l = GetWindowText( hWnd, strBuf, sizeof( strBuf ) );	// �õ������ı�
		buf = strBuf;
	}
	else
	{	// �����ı�̫�󣬶�̬����һ��
		l = (l + 1 )* sizeof( char ); 
		buf = malloc( l );
		if( buf )
		{
			GetWindowText( hWnd, buf, l );
		}
		else
			return;
	}

	textStyle = DT_SINGLELINE | DT_VCENTER;
	// �ñ���ɫ���
	if( bEnable )
	    FillSolidRect( hdc, &rectClient, pAttrib->cl_TextBk );  // clear background
	else
		FillSolidRect( hdc, &rectClient, pAttrib->cl_DisableBk );  // clear background
	//	ѡ���ˢ��
	hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    GetDrawRect( hdc, &rectClient, &rectText, &rectIcon, style );
	
    if( type == BS_DEFPUSHBUTTON ||
        type == BS_PUSHBUTTON ||
        (style & BS_PUSHLIKE) )
    {	// ��ť
		textStyle |= DT_CENTER;
		// ���߽����
        if( type == BS_DEFPUSHBUTTON )
        {	
            Rectangle( hdc, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom );
        }
        if( state & (BST_PUSHED|BST_CHECKED) )  // highlight state( sunken or checked )
        {	// 
            DrawEdge( hdc, &rectClient, BDR_SUNKENOUTER, BF_RECT );
            shift = 1;
        }
        else  // normal state
            DrawEdge( hdc, &rectClient, BDR_RAISEDOUTER, BF_RECT );
    }
	else if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON )
		DrawRadioBox( hdc, &rectIcon, state );	// ��ѡ��ť
	else if( type == BS_CHECKBOX || type == BS_AUTOCHECKBOX )
		DrawCheckBox( hdc, &rectIcon, state );	// ��ѡ��ť
    else if( type == BS_GROUPBOX )
    {	// ���
		textStyle |= DT_CENTER;
        rectClient.top += (rectText.bottom - rectText.top) / 2;  
		// ���߿�
        DrawEdge( hdc, &rectClient, BDR_SUNKENOUTER, BF_RECT );	//
        // ��С�߿�һ�����
		InflateRect( &rectClient, -1, -1 );
		// ���߿�
        DrawEdge( hdc, &rectClient, BDR_RAISEDOUTER, BF_RECT );	//
        // ��С�߿�һ�����
        InflateRect( &rectClient, -1, -1 );        
    }

    if( (state & BST_FOCUS) && type != BS_GROUPBOX )
    {   // ��ť�н��� has focus
	    DrawFocusRect( hdc, &rectText );
		//InflateRect( &rectText, -1, -1 ); // 2003-06-19, DEL, ���н�ʹ�ı���������
	}
   	if( shift )  // move down state
	{	// ʹ�ı����������ƶ����Ӿ���
        rectText.left += 1;
        rectText.top += 1;
	}
    // draw text or image
    if( style & (BS_BITMAP | BS_ICON) )
    {	// ͼ��λͼ��ť
        HANDLE hSave, hImage = pAttrib->hImage;
        if( hImage )
        {
            if( style & BS_BITMAP )
            {   // λͼ BITMAP
                HDC hMemDC = CreateCompatibleDC( hdc ); // �����ڴ�DC
                hSave = SelectObject( hMemDC, hImage );  // Ϊ�ڴ�DCѡ�������
                // ��ͼ
				BitBlt(
                    hdc,
                    rectText.left,
                    rectText.top,
                    rectText.right - rectText.left,
                    rectText.bottom - rectText.top,
                    hMemDC,
                    0,
                    0,
                    SRCCOPY
                    ); 
				// �ָ�
                hSave = SelectObject( hMemDC, hSave );
                DeleteDC( hMemDC );
            }
            else
            {  // ͼ�� ICON
                DrawIcon( hdc, rectText.left, rectText.top, hImage );
            }
        }
    }
    else
    {	// �ı���ť
		// ������ģʽ�������ı������ʽ
		if( (style & BS_CENTER) == BS_CENTER )
		{
			textStyle |= DT_CENTER; // ����
		}
		else
		{
			if( style & BS_LEFT )
			{
				textStyle &= ~DT_CENTER;
				textStyle |= DT_LEFT;  // �����
			}			
			else if( style & BS_RIGHT )
			{
				textStyle &= ~DT_CENTER;
				textStyle |= DT_RIGHT;	// �Ҷ���
			}
		}			
		if( (style & BS_VCENTER) == BS_VCENTER )
		{
			textStyle |= DT_VCENTER;	// ��ֱ����
		}
		else
		{
			if( style & BS_TOP )
			{
				textStyle &= ~DT_VCENTER;
				textStyle |= DT_TOP;	// �϶���
			}
			else if( style & BS_BOTTOM )
			{
				textStyle &= ~DT_VCENTER;
				textStyle |= DT_BOTTOM;	// �׶���
			}
		}

        if( (style & BS_MULTILINE) )
            textStyle &= ~DT_SINGLELINE; // �����ı�
        // ���ı� now draw text
        if( type == BS_GROUPBOX )
        {
			;
		}
        else
            SetBkMode( hdc, TRANSPARENT );	// ������͸��ģʽ
        if( bEnable )
        {	// ����ģʽ enable
			SetTextColor( hdc, pAttrib->cl_Text );
        }
        else
        {   // ͣ��ģʽ disable
            //SetTextColor( hdc, GetSysColor(COLOR_BTNTEXT) );
			SetTextColor( hdc, pAttrib->cl_Disable );
        }
		if( (style & BS_MULTILINE) && (style & BS_VCENTER) )
		{   // 
			RECT rc = rectText;
			int cyOrg, cyNew;
			DrawText( hdc, buf, l, &rc, textStyle | DT_CALCRECT );
			cyOrg = rectText.bottom - rectText.top;
			cyNew = rc.bottom - rc.top;
			if( cyNew < cyOrg )
				rectText.top += (cyOrg - cyNew) / 2;			
		}
		// ���ı�
        DrawText( hdc, buf, l, &rectText, textStyle  );
    }
	if( buf && buf != strBuf )
		free( buf );	//�ͷ�֮ǰ����Ļ���

	SelectObject( hdc, hBrush );	//�ָ�
}

/*
static void DrawButtonState( HDC hdc, HWND hWnd, DWORD style, DWORD state )
{
	WORD type = GETTYPE( style );
	RECT rectClient, rectText, rectIcon;
	HBRUSH hBrush;
	char buf[20];
	int l, shift = 0;
	WORD textStyle = 0;

	GetClientRect( hWnd, &rectClient );
	l = GetWindowText( hWnd, buf, sizeof( buf ) );

	textStyle = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
	//hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
    FillRect( hdc, &rectClient, GetStockObject( LTGRAY_BRUSH ) );  // clear background
	hBrush = SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
	if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON ||
		type == BS_CHECKBOX || type == BS_AUTOCHECKBOX )
	{
		rectText = rectClient;
		rectIcon = rectClient;
		if( style & BS_LEFTTEXT )
		{
		    rectIcon.left = rectClient.right - ICON_SIZE;
			rectIcon.right = rectClient.right;
			rectText.right = rectIcon.left - 2;
		}
		else
		{
		    rectIcon.left = rectClient.left;
			rectIcon.right = rectIcon.left + ICON_SIZE;
			rectText.left = rectIcon.right + 2;
		}
		rectIcon.top = (rectClient.bottom + rectClient.top) / 2 - ICON_SIZE / 2;
		rectIcon.bottom = rectIcon.top + ICON_SIZE;
		if( type == BS_RADIOBUTTON || type == BS_AUTORADIOBUTTON )
			DrawRadioBox( hdc, &rectIcon, state );
		else
			DrawCheckBox( hdc, &rectIcon, state );
	}
	else
	{    
		if( type == BS_DEFPUSHBUTTON ||
			type == BS_PUSHBUTTON ||
			(style & BS_PUSHLIKE) )
	    {
			if( type == BS_DEFPUSHBUTTON )
			{
				Rectangle( hdc, rectClient.left, rectClient.top, rectClient.right, rectClient.bottom );
				InflateRect( &rectClient, -1, -1 );
			}
    		if( state & BST_PUSHED )  // highlight state
            {
				DrawEdge( hdc, &rectClient, BDR_SUNKENOUTER, BF_RECT | BF_MIDDLE );
                shift = 1;
            }
			else  // normal state
				DrawEdge( hdc, &rectClient, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE );
			InflateRect( &rectClient, -1, -1 );
            
		}
		rectText = rectClient;

	}
    if( state & BST_FOCUS )
    {   // has focus
	    DrawFocusRect( hdc, &rectText );
		InflateRect( &rectText, -1, -1 );
	}
   	if( shift )  // move down state
	{
        rectText.left += 1;
        rectText.top += 1;
	}

    if( style & BS_LEFT )
	{
		textStyle &= ~DT_CENTER;
		textStyle |= DT_LEFT;
	}
	else if( style & BS_RIGHT )
	{
		textStyle &= ~DT_CENTER;
		textStyle |= DT_RIGHT;
	}
	if( style & BS_TOP )
	{
		textStyle &= ~DT_VCENTER;
		textStyle |= DT_TOP;
	}
	else if( style & BS_BOTTOM )
	{
		textStyle &= ~DT_VCENTER;
		textStyle |= DT_BOTTOM;
	}
	if( (style & BS_MULTILINE) )
	    textStyle &= ~DT_SINGLELINE;
		    // now draw text
	//SetBkColor( hdc,GetSysColor(COLOR_3DFACE) ); 
	SetBkMode( hdc, TRANSPARENT );
	if( !IsWindowEnabled( hWnd ) )
	{	// disable
	    SetTextColor( hdc, CL_DARKGRAY );
	}
	else
	{   // enable
	    SetTextColor( hdc, CL_BLACK );
	}
   	DrawText( hdc, buf, l, &rectText, textStyle  );
	SelectObject( hdc, hBrush );
}
*/

// **************************************************
// ������void DoOwnerDraw( HDC hdc, HWND hWnd, DWORD state, UINT uiAction )
// ������
//  IN hdc -  ��ʾ�豸���
//	IN hWnd - ���ھ��
//	IN state - ��ť״̬
//	IN uiAction - ��ǰ��ȡ���ж���������
//			ODA_DRAWENTIRE - �ؼ�������������Ҫ������. 
//			ODA_FOCUS - �ؼ�ʧȥ��õ��������뽹��. ���� state ���������Ƿ�ʧȥ��õ�. 
//			ODA_SELECT - ѡ��״̬�ı�. ���� state �������µ�״̬

// ����ֵ��
//	��
// ����������
//	�û��Ի洦��
// ����: 
//	
// ************************************************

// if window's style  is BS_OWNERDRAW, send message to owner window
static void DoOwnerDraw( HDC hdc, HWND hWnd, DWORD state, UINT uiAction )
{
	DRAWITEMSTRUCT drawItem;

	// �õ��ؼ��Ŀͻ���
	GetClientRect( hWnd, &drawItem.rcItem );
	// ��ʼ������
	drawItem.CtlType = ODT_BUTTON;
	drawItem.CtlID = (WORD)GetWindowLong( hWnd, GWL_ID );
	drawItem.itemID = 0;
	drawItem.itemAction = uiAction;
	drawItem.hwndItem = hWnd;
	drawItem.hDC = hdc; 
	drawItem.itemData = 0;
	drawItem.itemState = 0;
	// ȷ����ǰ��״̬
	if( state & ( BST_CHECKED | BST_PUSHED ) )
		drawItem.itemState |= ODS_SELECTED;
	if( state & BST_FOCUS )
		drawItem.itemState |= ODS_FOCUS;
	if( !IsWindowEnabled( hWnd ) )
		drawItem.itemState |= ODS_DISABLED;
	// ������Ϣ��������
	SendMessage( GetParent(hWnd), WM_DRAWITEM, GetWindowLong( hWnd, GWL_ID ), (LPARAM)&drawItem );
}

// **************************************************
// ������static void RefreshButton( HWND hWnd, PBUTTON_ATTRIB pAttrib )
// ������
//	IN hWnd - ���ھ��
//	IN pAttrib - �ؼ����Խṹ
// ����ֵ��
//	��
// ����������
//	ˢ�¿ؼ�����ʾ��
// ����: 
//	
// ************************************************

static void RefreshButton( HWND hWnd, PBUTTON_ATTRIB pAttrib )
{
	HDC hdc = GetDC( hWnd );  // ��ϵͳ������ʾDC
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE ); // �õ����ڷ��

	if( pAttrib == NULL )
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );  // �Ӵ���˽���������õ��ؼ�����

	switch( dwStyle & 0xf )
	{
	case BS_OWNERDRAW:		// �Ի���
		DoOwnerDraw( hdc, hWnd, pAttrib->btState, ODA_DRAWENTIRE );
		break;
	default:
		DrawButtonState( hdc, hWnd, dwStyle, pAttrib->btState );
	}

	ReleaseDC( hWnd, hdc );  // �ͷ�DC
}

// **************************************************
// ������static LRESULT DoPAINT( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	����WM_PAINT��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoPAINT( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT ps;
	DWORD state;
	DWORD dwStyle;
	DWORD s;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	// get draw dc
	hdc = BeginPaint( hWnd, &ps );// ��ϵͳ������ʾDC
	// get myself state from extra window bytes, see RegisterMyButton
	state = pAttrib->btState;//
	// get window style
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );	// ���ڷ��

	s = dwStyle & 0xf;
	switch( s )
	{
	case BS_OWNERDRAW:		// �Ի���
		DoOwnerDraw( hdc, hWnd, state, ODA_DRAWENTIRE );
		break;
	default:
		DrawButtonState( hdc, hWnd, dwStyle, state );
	}
    EndPaint( hWnd, &ps );	// �ͷ�DC
	return 0;
}

// **************************************************
// ������static LRESULT DoLBUTTONDOWN( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	���� WM_LBUTTONDOWN ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoLBUTTONDOWN( HWND hWnd )
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����
    //DWORD dwStyle;
	if( (pAttrib->btFlag & BTF_DISABLEFOCUS) == 0 )	
	{
		if( GetFocus() != hWnd )
			SetFocus( hWnd );	// ���ñ�����Ϊ���뽹��
	}
	
	// highlight state if possible
	//2004-05-20, not use remove lilin
    //dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	//

    // if possible, set button state
    DoSETSTATE( hWnd, TRUE );	// ������״̬
	// capture mouse
	SetCapture(hWnd);	// ץס������
	return 0;
}

// **************************************************
// ������static LRESULT DoMouseMove( HWND hWnd, short x, short y )
// ������
//	IN hWnd - ���ھ��
//	IN x - ��ǰ�������豸�ڿؼ��Ŀͻ���x���� 
//	IN y - ��ǰ�������豸�ڿؼ��Ŀͻ���y����
// ����ֵ��
//	����0
// ����������
//	���� WM_MOUSEMOVE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoMouseMove( HWND hWnd, short x, short y )
{
    RECT rect;
    DWORD state;
    int type;
    POINT pt;
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

    type = GETTYPE( GetWindowLong( hWnd, GWL_STYLE ) );    // �õ��ؼ�����

    pt.x = x;
    pt.y = y;

    if( GetCapture() == hWnd )	// ֮ǰ�Ƿ��Ѿ�ץס�������豸 ��
    {	// ��
        GetClientRect( hWnd, &rect );	// �õ��ؼ��ͻ�������

        state = pAttrib->btState;

        if( PtInRect( &rect, pt ) )	// �������豸�ڿؼ��ͻ����� ��
        {	//�ڿؼ��ͻ���
            if( (state & BST_PUSHED) ==  0 )	// ��ǰ�ؼ�״̬Ϊ����״̬�� ��
                DoSETSTATE( hWnd, TRUE );	// ���ǣ��ı�״̬
        }
        else
        {	// ���ڿؼ��ͻ���
            if( state & BST_PUSHED )	// // ��ǰ�ؼ�״̬Ϊ����״̬�� ��
                DoSETSTATE( hWnd, FALSE );// ���ǣ��ı�״̬
        }
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoMouseMove( HWND hWnd, short x, short y )
// ������
//	IN hWnd - ���ھ��
//	IN x - ��ǰ�������豸�ڿؼ��Ŀͻ���x���� 
//	IN y - ��ǰ�������豸�ڿؼ��Ŀͻ���y����
// ����ֵ��
//	����0
// ����������
//	���� WM_LBUTTONUP ��Ϣ
// ����: 
//	
// ************************************************

//extern BOOL _Wnd_IsClass( HWND hWnd, LPCBYTE lpcName );
static LRESULT DoLBUTTONUP( HWND hWnd, short x, short y )
{
	DWORD dwStyle;
	DWORD s, dw;
    RECT rect;
    POINT pt;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

    pt.x = x; pt.y = y;

	if( GetCapture() != hWnd )	// ֮ǰ�Ƿ��Ѿ�ץס�������豸 ��
		return 0;	// �񣬷���
	// release mouse
    ReleaseCapture();	// �ͷŵ������豸

    s = pAttrib->btState;
	DoSETSTATE( hWnd, FALSE );  // ���ð�ťΪ�ͷ�״̬
    GetClientRect( hWnd, &rect );		//// �õ��ؼ��ͻ�������
	
    if( PtInRect( &rect, pt ) )
    {   
	    dwStyle = GetWindowLong( hWnd, GWL_STYLE );	// �õ��ؼ����
	    if( GETTYPE(dwStyle) == BS_AUTOCHECKBOX )	// �Զ���ѡ��ť�� ��
	    {	// �ǣ���ת״̬
			DoSETCHECK( hWnd, (s & BST_CHECKED) ? BST_UNCHECKED : BST_CHECKED );
	    }
        else if( GETTYPE(dwStyle) == BS_AUTORADIOBUTTON && 
                 (s & BST_CHECKED) == FALSE )	// �Զ���ѡ��ť�� ���� ��ǰ״̬Ϊ��ѡ��״��
        {	// ��
            HWND hParent = GetParent( hWnd );	//�õ���ť�ĸ����ھ��
            if( hParent )
            {	// 
                HWND hNext = GetWindow( hParent, GW_CHILD );//�õ������ڵĵ�һ���Ӵ���
				// ���������ڵ�����Ϊ�Զ���ѡ��ť�����Ӵ���
				// ������е�һ����״̬Ϊѡ��״̬��������Ϊ��ѡ��״̬
                while( hNext )
                {
					TCHAR strClass[sizeof( strClassButton )];
					strClass[0] = 0;
					GetClassName( hNext, strClass, sizeof( strClassButton ) );	// �õ�����
                    if( stricmp( strClass, strClassButton ) == 0 )	// �ǰ�ť���� ��
                    {  /// �� yes , is button
                        dwStyle = GetWindowLong( hNext, GWL_STYLE );	// �õ���ť���
                        if( GETTYPE(dwStyle) == BS_AUTORADIOBUTTON )	// �Զ���ѡ��ť�� ? 
                        {	// ��
							PBUTTON_ATTRIB pa = (PBUTTON_ATTRIB)GetWindowLong( hNext, 0 ); // �Ӵ���˽���������õ��ؼ�����
                            s = pa->btState;
                            if( s & BST_CHECKED )
                            {	// Ϊѡ��״̬
                                DoSETCHECK( hNext, BST_UNCHECKED );// ��Ϊ��ѡ��״̬
                                break;
                            }
                        }
                    }
                    hNext = GetWindow( hNext, GW_HWNDNEXT );	// ��һ������
                }
            }
            DoSETCHECK( hWnd, BST_CHECKED );// ����ǰ��ť��Ϊѡ��״̬
        }

	    dw = GetWindowLong( hWnd, GWL_ID );
		SendNotify( GetParent(hWnd), dw, BN_CLICKED, hWnd );	// �븸����֪ͨ��Ϣ
    }

	return 0;
}

// **************************************************
// ������static LRESULT DoSETFOCUS( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	���� WM_SETFOCUS ��Ϣ
// ����: 
//	
// ************************************************
static LRESULT DoSETFOCUS( HWND hWnd )
{
	DWORD s, dwStyle;
//    int type;
	HDC hdc;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	if( pAttrib->btFlag & BTF_DISABLEFOCUS )
		return 0;

	// get current state at extra window bytes
	s = pAttrib->btState;//GetWindowLong( hWnd, OFFSET_STATE );
	s |= BST_FOCUS;  // set focus flag
    // set new state at extra window bytes
	pAttrib->btState = (WORD)s;

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );	// �õ����ڷ��

    if( GETTYPE( dwStyle ) != BS_GROUPBOX )	// ��� ��
    {   // �� get draw dc
        hdc = GetDC( hWnd );	// ��ϵͳ�õ���ʾDC
        if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// �Ի�
            DoOwnerDraw( hdc, hWnd, s, ODA_FOCUS );	// 
        else
        {            
            RECT rect, textRect, iconRect;
            GetClientRect( hWnd, &rect );	// �õ��ͻ���
            GetDrawRect( hdc, &rect, &textRect, &iconRect, dwStyle );        
            DrawFocusRect( hdc, &textRect );	// ������
        }
        ReleaseDC( hWnd, hdc );	// �ͷ�DC
    }
	if( dwStyle & BS_NOTIFY )	// ������ܣ� �򸸷�֪ͨ��Ϣ
	    SendNotify( GetParent(hWnd), GetWindowLong(hWnd, GWL_ID), BN_SETFOCUS, hWnd );
	return 0;
}

// **************************************************
// ������static LRESULT DoKILLFOCUS( HWND hWnd )
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	����0
// ����������
//	���� WM_KILLFOCUS ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoKILLFOCUS( HWND hWnd )
{
	DWORD s, dwStyle;
	HDC hdc;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����


	// get current state
	s = pAttrib->btState;
	s &= ~BST_FOCUS;
	// set new state to extra window bytes
	pAttrib->btState = (WORD)s;

    dwStyle = GetWindowLong( hWnd, GWL_STYLE ); // �õ����ڷ��

    if( GETTYPE( dwStyle ) != BS_GROUPBOX )	// ��� ��
    {
        // get draw dc
        hdc = GetDC( hWnd );// ��ϵͳ�õ���ʾDC
        // get window style
        if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// �Ի�
            DoOwnerDraw( hdc, hWnd, s, ODA_FOCUS );
        else
        {
            RECT rect, textRect, iconRect;
            
            GetClientRect( hWnd, &rect );// �õ��ͻ���
            GetDrawRect( hdc, &rect, &textRect, &iconRect, dwStyle );
            DrawFocusRect( hdc, &textRect );
        }
        ReleaseDC( hWnd, hdc );	// �ͷ�DC
    }
	if( dwStyle & BS_NOTIFY )	// ������ܣ� �򸸷�֪ͨ��Ϣ
	    SendNotify( GetParent(hWnd), GetWindowLong(hWnd, GWL_ID), BN_KILLFOCUS, hWnd );

	return 0;
 }

// **************************************************
// ������static LRESULT DoSETSTATE( HWND hWnd, WARAM wParam )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - �µ�״̬. ����ΪTRUE,����Ϊѡ��״̬������Ϊ��ѡ��״̬
// ����ֵ��
//	����0
// ����������
//	���ð�ť״̬��Ҳ�� BM_SETSTATE ��Ϣ�Ĵ�����
// ����: 
//	
// ************************************************

static LRESULT DoSETSTATE( HWND hWnd, WPARAM wParam )
{
	HDC hdc;
	DWORD dwStyle;
	DWORD state;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	dwStyle = GetWindowLong( hWnd, GWL_STYLE );// �õ����ڷ��

    {
        // get current state
        state = pAttrib->btState;
        if( wParam )
            state |= BST_PUSHED;	//����״̬
        else
            state &= ~BST_PUSHED;
        // set new state
		pAttrib->btState = (WORD)state;
        // get draw dc
        hdc = GetDC( hWnd );		// ��ϵͳ�õ���ʾDC

        if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// �Ի�
            DoOwnerDraw( hdc, hWnd, state, ODA_SELECT );
        else
            DrawButtonState( hdc, hWnd, dwStyle, state );
        
        ReleaseDC( hWnd, hdc );// �ͷ�DC
    }
    return 0;
}

// **************************************************
// ������static LRESULT DoSETCHECK( HWND hWnd, WARAM wParam )
// ������
//	IN hWnd - ���ھ��
//	IN wParam - �µ�״̬. ����ΪTRUE,����Ϊѡ��״̬������Ϊ��ѡ��״̬
// ����ֵ��
//	����0
// ����������
//	���ð�ť״̬��Ҳ�� BM_SETCHECK ��Ϣ�Ĵ�����
// ����: 
//	
// ************************************************

static LRESULT DoSETCHECK( HWND hWnd, WPARAM wParam )
{
	HDC hdc;
	DWORD dwStyle;
	DWORD state;
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	// get current state
	state = pAttrib->btState;

    if( wParam & BST_CHECKED )
        state |= BST_CHECKED;//ѡ��״̬
    else
	    state &= ~BST_CHECKED;
	// set new state
	pAttrib->btState = (WORD)state;

	// get draw dc
	hdc = GetDC( hWnd );// ��ϵͳ�õ���ʾDC
	// get window style
	dwStyle = GetWindowLong( hWnd, GWL_STYLE );// �õ����ڷ��

	if( GETTYPE( dwStyle ) ==  BS_OWNERDRAW )	// �Ի�
		DoOwnerDraw( hdc, hWnd, state, ODA_SELECT );
	else
		DrawButtonState( hdc, hWnd, dwStyle, state );

	ReleaseDC( hWnd, hdc );// �ͷ�DC
	return 0;
}

// **************************************************
// ������static VOID _SetButtonColor( PBUTTON_ATTRIB pAttrib )
// ������
//	IN pAttrib - ��ť�����ݽṹ
// ����ֵ��
//	��
// ����������
//	���ô�����ʾ��ɫ
// ����: 
//	
// ************************************************

static VOID _SetButtonColor( PBUTTON_ATTRIB pAttrib )
{
	pAttrib->cl_Text = GetSysColor( COLOR_BTNTEXT );
	pAttrib->cl_TextBk = GetSysColor( COLOR_BTNFACE );
	pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
	pAttrib->cl_DisableBk = GetSysColor( COLOR_BTNFACE );
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
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)malloc( sizeof(BUTTON_ATTRIB) );	// ����ؼ����Խṹ
	if( pAttrib )
	{	//��ʼ��
		memset( pAttrib, 0, sizeof(BUTTON_ATTRIB) );
		//pAttrib->cl_Text = GetSysColor( COLOR_BTNTEXT );
		//pAttrib->cl_TextBk = GetSysColor( COLOR_BTNFACE );
		//pAttrib->cl_Disable = GetSysColor( COLOR_GRAYTEXT );
		//pAttrib->cl_DisableBk = GetSysColor( COLOR_BTNFACE );
		_SetButtonColor( pAttrib );

		SetWindowLong( hWnd, 0, (LONG)pAttrib );// ���ÿؼ�����ָ�뵽����˽��������
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
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����
	free( pAttrib );	//�ͷ�
	return 0;
}

// **************************************************
// ������static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
// ������
//	IN hWnd - ���ھ��
//	IN lpccs - �ؼ��ṹ��������ɫֵ
// ����ֵ��
//	�ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���ÿؼ���ɫ
// ����: 
//	
// ************************************************

static LRESULT DoSetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

		if( lpccs->fMask & CLF_TEXTCOLOR )	// �ı�
			pAttrib->cl_Text = lpccs->cl_Text;
		if( lpccs->fMask & CLF_TEXTBKCOLOR )	// �ı�����
			pAttrib->cl_TextBk = lpccs->cl_TextBk;
		if( lpccs->fMask & CLF_DISABLECOLOR )	// ��Ч�ı�
			pAttrib->cl_Disable = lpccs->cl_Disable;
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )	// ��Ч�ı�����
			pAttrib->cl_DisableBk = lpccs->cl_DisableBk;
        RefreshButton( hWnd, pAttrib );	// �ػ�
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
//	�õ��ؼ���ɫ
// ����: 
//	
// ************************************************

static LRESULT DoGetCtlColor( HWND hWnd, LPCTLCOLORSTRUCT lpccs )
{
	if( lpccs )
	{
		PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

		if( lpccs->fMask & CLF_TEXTCOLOR )	// �ı�
			lpccs->cl_Text = pAttrib->cl_Text;
		if( lpccs->fMask & CLF_TEXTBKCOLOR )	// �ı�����
			lpccs->cl_TextBk = pAttrib->cl_TextBk;
		if( lpccs->fMask & CLF_DISABLECOLOR )	// ��Ч�ı�
			lpccs->cl_Disable = pAttrib->cl_Disable;
		if( lpccs->fMask & CLF_DISABLEBKCOLOR )	// ��Ч�ı�����
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
    PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����

	_SetButtonColor( pAttrib );
	return 0;
}


static LRESULT DoEnableFocus( HWND hWnd, BOOL bEnable )
{
	PBUTTON_ATTRIB pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );// �Ӵ���˽���������õ��ؼ�����
	if( bEnable )
	{	//enable
		pAttrib->btFlag &= ~BTF_DISABLEFOCUS;
	}
	else
	{	//disbale
		pAttrib->btFlag |= BTF_DISABLEFOCUS;
	}
	return 0;
}

// **************************************************
// ������LRESULT WINAPI ButtonWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
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

// this all message handle center
LRESULT WINAPI ButtonWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	DWORD s;
	PBUTTON_ATTRIB pAttrib;
	switch( message )
	{
	case WM_PAINT:			// ϵͳ������Ϣ
		return DoPAINT( hWnd );
	case WM_LBUTTONDBLCLK:			// �����˫����Ϣ
	case WM_LBUTTONDOWN:			// ����갴����Ϣ
		return DoLBUTTONDOWN( hWnd );
    case WM_MOUSEMOVE:				// ����ƶ���Ϣ
        if( wParam & MK_LBUTTON )
            return DoMouseMove( hWnd, LOWORD( lParam ), HIWORD( lParam ) );
         else
             break;
	case WM_LBUTTONUP:				// ������ͷ���Ϣ
		return DoLBUTTONUP( hWnd, LOWORD( lParam ), HIWORD( lParam ) );
	case WM_SETFOCUS:				// ���ý���
		return DoSETFOCUS( hWnd );
	case WM_KILLFOCUS:				// �ͷŽ���
		return DoKILLFOCUS( hWnd );
	case BM_GETCHECK:					// �õ�ѡ��״̬
		s = GetWindowLong( hWnd, GWL_STYLE );
		s &= 0x0f;
		if( s == BS_RADIOBUTTON ||
			s == BS_CHECKBOX || 
			s == BS_AUTOCHECKBOX ||
			s == BS_AUTORADIOBUTTON ||
			s == BS_AUTO3STATE ||
			s == BS_3STATE )
		{
			pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
			return pAttrib->btState & 0x03;//return (GetWindowLong( hWnd, OFFSET_STATE ) & 0x03 );
		}
		// if this is pushbutton
		return 0;
	case BM_SETCHECK:				// ����ѡ��״̬
		return DoSETCHECK( hWnd, wParam );
	case BM_GETSTATE:				
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
		return pAttrib->btState;
	case BM_SETSTATE:
		return DoSETSTATE( hWnd, wParam );
	case BM_SETSTYLE:				// ���÷��
		s = GetWindowLong( hWnd, GWL_STYLE );
		s &= 0xffff0000l;
		s |= LOWORD( wParam );
		SetWindowLong( hWnd, GWL_STYLE, s );
		if( lParam )
		{  // to redraw button, I invalidate entire client
			//InvalidateRect( hWnd, 0, TRUE );
			RefreshButton( hWnd, NULL );
		}
		return 0;
    case BM_SETIMAGE:			// ������ʾͼƬ
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
        s = (DWORD)pAttrib->hImage;
        pAttrib->hImage = (HANDLE)lParam;
		RefreshButton( hWnd, NULL );	//�ػ�
        return s;     
    case BM_GETIMAGE:
		pAttrib = (PBUTTON_ATTRIB)GetWindowLong( hWnd, 0 );
        return (DWORD)pAttrib->hImage;
	case WM_SETTEXT:
		s = DefWindowProc(hWnd,message,wParam,lParam);
		RefreshButton( hWnd, NULL );	//�ػ�
		return s;
	case WM_ENABLE:
		RefreshButton( hWnd, NULL );	//�ػ�
		return 0;
    case WM_SETCTLCOLOR:		// ������ʾ��ɫ
		return DoSetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
    case WM_GETCTLCOLOR:
		return DoGetCtlColor( hWnd, (LPCTLCOLORSTRUCT)lParam );
	case BM_ENABLEFOCUS:		//�رջ�򿪰�ť�Ľ��㹦��
		return DoEnableFocus( hWnd, wParam );
	case WM_SYSCOLORCHANGE:
		return DoSysColorChange( hWnd );
    case WM_CREATE:				// ���ڴ�����ʼ��
		return DoCreate( hWnd );
    case WM_DESTROY:			// �ƻ�
		return DoDestroy( hWnd );

	}
	// It's important to do this
	// if your do'nt handle message, you must call DefWindowProc
	// ��Ҫ��ʾ�������㲻��Ҫ������Ϣ��������Ϣ����ϵͳ����Ĭ�ϴ�����ȥ����
	// 
	return DefWindowProc(hWnd,message,wParam,lParam);
}
