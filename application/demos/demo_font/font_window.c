// MPlayer.cpp : Defines the entry point for the application.
//
#include <ewindows.h>

static const char strClassName[]="FontTest";
static LRESULT WINAPI TestWndProc( HWND , UINT, WPARAM, LPARAM );

static HWND hTestWnd;
static HFONT hHanFont;
static HFONT hBig5Font;
static HFONT hGB2312;
static HFONT hJapan;
static HBITMAP g_hbmpFontBk;
static ATOM RegisterTestClass(HINSTANCE hInst)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.hInstance=hInst;
   wc.lpszClassName = strClassName;
   wc.lpfnWndProc=(WNDPROC)TestWndProc;
   wc.style=CS_DBLCLKS|CS_CLASSDC;
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = NULL;//GetSysColorBrush( COLOR_WINDOW );

   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc);
}
// **************************************************
// ������static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ��С
// 	IN xOffset -- Xƫ��
// 	IN yOffset -- Yƫ��
// 	IN dwRop -- ����ģʽ
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawBitMap( HDC hdc,HBITMAP hBitmap, LPRECT lprect,UINT xOffset,UINT yOffset,DWORD dwRop )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // ����һ�����ݵ��ڴ���
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼ���õ��ڴ�����
		BitBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			dwRop
			);

		SelectObject(hMemoryDC,hOldBitmap); // �ָ��ڴ���
		DeleteDC(hMemoryDC); // ɾ�����
}

LRESULT WINAPI TestWndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	static int i = 0;
	switch( uiMsg )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			char eng[] = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz";
			char gb[] = "�޹���Kingmosϵͳ, ����΢�߼�";
			//char hwbig5[] = "�x�_���ʬ�� �E M o b i N o t e ";
			char hwbig5[] = "�c�餤�媩��";
			char korea[] = "�Ｚ�� ������ ö��";//
			char japan[] = "�g�ь����A�h�x���`���[�Q�[���T";//
			
			HFONT hOld;
			HDC hdc = BeginPaint( hWnd, &ps );
			RECT rect;

			GetClientRect( hWnd, &rect );

			DrawBitMap( hdc, g_hbmpFontBk, &rect, 0, 0 ,SRCCOPY);  // ����λͼ

			SetBkMode( hdc, TRANSPARENT );
			SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
			
			hOld = SelectObject( hdc, hGB2312 );

			//SetBkColor( hdc, CL_LIGHTGRAY );
			TextOut( hdc, 10, 20, eng, sizeof(eng) - 1 );			
			TextOut( hdc, 10, 60, gb, sizeof(gb) - 1 );
			//TextOut( hdc, 0, 80, hwbig5, sizeof(hwbig5) - 1 );
			//TextOut( hdc, 0, 120, hwjapan, sizeof(hwjapan) - 1 );
			SelectObject( hdc, hOld );

			
			hOld = SelectObject( hdc, hHanFont );
			TextOut( hdc, 10, 100, korea, sizeof(korea) - 1 );
			
			hOld = SelectObject( hdc, hBig5Font );
			TextOut( hdc, 10, 140, hwbig5, sizeof(hwbig5) - 1 );

			hOld = SelectObject( hdc, hJapan );
			TextOut( hdc, 10, 180, japan, sizeof(japan) - 1 );

			SelectObject( hdc, hOld );
			

			EndPaint( hWnd, &ps );
		}
		return 0;
	case WM_CLOSE:
		DestroyWindow( hWnd );
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc( hWnd, uiMsg, wParam, lParam );
	}
	return 0;
}

LRESULT CALLBACK WinMain_DemoFont( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
{
	MSG msg;
	LOGFONT lf;

	HWND hWnd;
	
	hWnd = FindWindow( strClassName, NULL ) ;  // ���ҵ�ǰ�����Ƿ��Ѿ�����
	if( hWnd != 0 )
	{ // �Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ���ǰ��
		return FALSE ;
	}

	
	g_hbmpFontBk = LoadImage( NULL, "./kingmos/demos/font_bk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	
    RegisterTestClass( hInstance );
    hTestWnd = CreateWindowEx( WS_EX_CLOSEBOX, 
		strClassName, 
		"�������", 
		WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE, 
		50, 200, 570, 360,
		NULL,
		NULL, 
		hInstance,
		0 );

	memset( &lf, 0, sizeof(lf) );
	lf.lfCharSet = HANGUL_CHARSET;
	lf.lfHeight = 16;
	lf.lfWidth = 16;
	hHanFont = CreateFontIndirect( &lf ); 

	lf.lfCharSet = CHINESEBIG5_CHARSET;
	lf.lfHeight = 24;
	lf.lfWidth = 24;
	hBig5Font = CreateFontIndirect( &lf ); 

	lf.lfCharSet = GB2312_CHARSET;
	lf.lfHeight = 32;
	lf.lfWidth = 32;
	hGB2312 = CreateFontIndirect( &lf ); 
	

	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfHeight = 40;
	lf.lfWidth = 40;
	hJapan = CreateFontIndirect( &lf ); 
	
	//CreateThread( NULL, 0, ThreadProc, 1, 0, 0 );
	
	while( GetMessage( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}	
	DeleteObject( hGB2312 );
	DeleteObject( hBig5Font );
	DeleteObject( hHanFont );
	DeleteObject( hJapan );
	DeleteObject( g_hbmpFontBk );
    return 0;
}

