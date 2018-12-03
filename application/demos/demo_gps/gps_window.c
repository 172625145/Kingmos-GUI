// MPlayer.cpp : Defines the entry point for the application.
//
#include <ewindows.h>

static const char strClassName[]="GPSTestClass";
static LRESULT WINAPI GPSWndProc( HWND , UINT, WPARAM, LPARAM );
static int g_AlphaVlue = 100;

extern BOOL WINAPI Win_SetLayeredAttributes(
  HWND hWnd,           // 窗口句柄
  COLORREF clrTransparentKey,      // 透明颜色
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // 需要设置的动作： LWA_COLORKEY, clrTransparentKey 有效
					 	//				   LWA_ALPHA,  bAlpha 有效
);


//static HBITMAP g_hbmpFontBk;
static ATOM RegisterTestClass(HINSTANCE hInst)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.hInstance=hInst;
   wc.lpszClassName = strClassName;
   wc.lpfnWndProc=(WNDPROC)GPSWndProc;
   wc.style=CS_DBLCLKS|CS_CLASSDC;
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = CreateSolidBrush( RGB(0xff, 0xff, 0) );

   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc);
}
// **************************************************
// 声明：static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN rect -- 位图大小
// 	IN xOffset -- X偏移
// 	IN yOffset -- Y偏移
// 	IN dwRop -- 绘制模式
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
static void DrawBitMap( HDC hdc,HBITMAP hBitmap, LPRECT lprect,UINT xOffset,UINT yOffset,DWORD dwRop )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // 创建一个兼容的内存句柄
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将要绘制的位图设置到内存句柄中
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

		SelectObject(hMemoryDC,hOldBitmap); // 恢复内存句柄
		DeleteDC(hMemoryDC); // 删除句柄
}

LRESULT WINAPI GPSWndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	static int i = 0;
	switch( uiMsg )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint( hWnd, &ps );
			RECT rect;
			HPEN hOld, hPen = CreatePen( PS_SOLID, 3, RGB( 0, 0, 0 ) );
			HBRUSH hBrush = CreateSolidBrush( RGB( 50, 50, 50 ) );

			GetClientRect( hWnd, &rect );
			hOld = SelectObject( hdc, hPen );
			SelectObject( hdc, hBrush );
			
			RoundRect( hdc, rect.left + 3, rect.top + 3, rect.right - 3, rect.bottom - 3 - 40, 18, 18 );
			SetBkMode( hdc, TRANSPARENT );
			SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
			TextOut( hdc, 10, 10, "经度：123.4567", sizeof("经度：123.4567") );
			TextOut( hdc, 200, 10, "亮度：左/右", sizeof("亮度：左/右") );
			TextOut( hdc, 10, 40, "纬度：25.1234", sizeof("纬度：25.1234") );
			TextOut( hdc, 10, 70, "速度：30米/秒", sizeof("速度：30米/秒") );
			TextOut( hdc, 10, 110, "方向：东南", sizeof("方向：东南") );

			SelectObject( hdc, hOld );

			DeleteObject( hBrush );
			hBrush = CreateSolidBrush( RGB( 0xff, 0, 0 ) );
			SelectObject( hdc, hBrush );

			Rectangle( hdc, rect.left + 3, rect.bottom - 3 - 40 + 20, rect.right - 3, rect.bottom );//, 9, 9 );
			TextOut( hdc, rect.left + 3 + 10, rect.bottom - 3 - 40 + 20 + 2, "退出", sizeof("退出") );

			DeleteObject( hPen );
			DeleteObject( hBrush );
			

			EndPaint( hWnd, &ps );
		}
		return 0;
	case WM_KEYDOWN:
		if( wParam == VK_RIGHT )
		{
			g_AlphaVlue+=30;
			if( g_AlphaVlue > 255 )
				g_AlphaVlue = 255;
			Win_SetLayeredAttributes( hWnd, RGB(0xff, 0xff, 0), g_AlphaVlue, LWA_ALPHA | LWA_COLORKEY );
		}
		else if( wParam == VK_LEFT )
		{
			g_AlphaVlue-=30;
			if( g_AlphaVlue < 0 )
				g_AlphaVlue = 0;
			Win_SetLayeredAttributes( hWnd, RGB(0xff, 0xff, 0), g_AlphaVlue, LWA_ALPHA | LWA_COLORKEY );
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

LRESULT CALLBACK WinMain_DemoGPS( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
{
	MSG msg;

	HWND hWnd;
	HWND hTestWnd;
	
//	hWnd = FindWindow( strClassName, NULL ) ;  // 查找当前程序是否已经运行
//	if( hWnd != 0 )
//	{ // 已经运行
//		SetForegroundWindow( hWnd ) ; // 设置窗口到最前面
//		return FALSE ;
//	}

	
	//g_hbmpFontBk = LoadImage( NULL, "./kingmos/demos/font_bk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	
    RegisterTestClass( hInstance );
    hTestWnd = CreateWindowEx( WS_EX_LAYERED, 
		strClassName, 
		"GPS 状态", 
		WS_POPUP | WS_VISIBLE, 
		20, 20, 320, 200,
		NULL,
		NULL, 
		hInstance,
		0 );

	Win_SetLayeredAttributes( hTestWnd, RGB(0xff, 0xff, 0), g_AlphaVlue, LWA_ALPHA | LWA_COLORKEY );
	
	while( GetMessage( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}	
	//DeleteObject( g_hbmpFontBk );
    return 0;
}


