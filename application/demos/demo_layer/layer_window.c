// MPlayer.cpp : Defines the entry point for the application.
//
#include <ewindows.h>

static const char strClassName[]="LayerTestClass";
static LRESULT WINAPI TestWndProc( HWND , UINT, WPARAM, LPARAM );

extern BOOL WINAPI Win_SetLayeredAttributes(
  HWND hWnd,           // ���ھ��
  COLORREF clrTransparentKey,      // ͸����ɫ
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // ��Ҫ���õĶ����� LWA_COLORKEY, clrTransparentKey ��Ч
					 	//				   LWA_ALPHA,  bAlpha ��Ч
);


//static HBITMAP g_hbmpFontBk;
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
   wc.hbrBackground = GetSysColorBrush( COLOR_WINDOW );

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
			HDC hdc = BeginPaint( hWnd, &ps );
			RECT rect;

			GetClientRect( hWnd, &rect );
			TextOut( hdc, 0, 0, "Hello,World", 11 );

			//DrawBitMap( hdc, g_hbmpFontBk, &rect, 0, 0 ,SRCCOPY);  // ����λͼ

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

LRESULT CALLBACK WinMain_DemoLayer( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
{
	MSG msg;

	HWND hWnd;
	HWND hTestWnd;
	
//	hWnd = FindWindow( strClassName, NULL ) ;  // ���ҵ�ǰ�����Ƿ��Ѿ�����
//	if( hWnd != 0 )
//	{ // �Ѿ�����
//		SetForegroundWindow( hWnd ) ; // ���ô��ڵ���ǰ��
//		return FALSE ;
//	}

	
	//g_hbmpFontBk = LoadImage( NULL, "./kingmos/demos/font_bk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	
    RegisterTestClass( hInstance );
    hTestWnd = CreateWindowEx( WS_EX_CLOSEBOX | WS_EX_LAYERED, 
		strClassName, 
		"Layered ���ڲ���", 
		WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE, 
		100, 50, 240, 320,
		NULL,
		NULL, 
		hInstance,
		0 );

    CreateWindowEx( 0, 
		classEDIT, 
		"�Ӵ���", 
		WS_VISIBLE | WS_CHILD, 
		0, 0, 230, 200,
		hTestWnd,
		NULL, 
		hInstance,
		0 );

    CreateWindowEx( 0, 
		classBUTTON, 
		"�Ӵ���", 
		WS_VISIBLE | WS_CHILD, 
		0, 100, 100, 40,
		hTestWnd,
		NULL, 
		hInstance,
		0 );

	Win_SetLayeredAttributes( hTestWnd, -1, 128, LWA_ALPHA );
	
	while( GetMessage( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}	
	//DeleteObject( g_hbmpFontBk );
    return 0;
}


