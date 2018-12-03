// MPlayer.cpp : Defines the entry point for the application.
//
#include <ewindows.h>

static const char strClassName[]="LayerTestClass";
static LRESULT WINAPI TestWndProc( HWND , UINT, WPARAM, LPARAM );

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

			//DrawBitMap( hdc, g_hbmpFontBk, &rect, 0, 0 ,SRCCOPY);  // 绘制位图

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
	
//	hWnd = FindWindow( strClassName, NULL ) ;  // 查找当前程序是否已经运行
//	if( hWnd != 0 )
//	{ // 已经运行
//		SetForegroundWindow( hWnd ) ; // 设置窗口到最前面
//		return FALSE ;
//	}

	
	//g_hbmpFontBk = LoadImage( NULL, "./kingmos/demos/font_bk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	
    RegisterTestClass( hInstance );
    hTestWnd = CreateWindowEx( WS_EX_CLOSEBOX | WS_EX_LAYERED, 
		strClassName, 
		"Layered 窗口测试", 
		WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE, 
		100, 50, 240, 320,
		NULL,
		NULL, 
		hInstance,
		0 );

    CreateWindowEx( 0, 
		classEDIT, 
		"子窗口", 
		WS_VISIBLE | WS_CHILD, 
		0, 0, 230, 200,
		hTestWnd,
		NULL, 
		hInstance,
		0 );

    CreateWindowEx( 0, 
		classBUTTON, 
		"子窗口", 
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


