// MPlayer.cpp : Defines the entry point for the application.
//
#include <ewindows.h>

static const char strClassName[]="hello";
LRESULT WINAPI TestWndProc( HWND , UINT, WPARAM, LPARAM );

HWND hTestWnd;
HFONT hHanFont;
HFONT hBig5Font;

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

LRESULT WINAPI TestWndProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
	static int i = 0;
	switch( uiMsg )
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			char gb[] = "滚滚长江东逝水！";
			char hwbig5[] = "掌寶移動科技與銷售通路擁有緊密的售後服務網";
			char hwjapan[] = "平素はイメージステーションをご利用いただき、まことにありがとうございます。";

			char korea[] = "滚滚长江东逝水！";
			
			HFONT hOld;
			HDC hdc = BeginPaint( hWnd, &ps );
			TextOut( hdc, 0, 0, gb, sizeof(gb) - 1 );
			TextOut( hdc, 0, 20, hwbig5, sizeof(hwbig5) - 1 );
			TextOut( hdc, 0, 40, hwjapan, sizeof(hwjapan) - 1 );
			
//			hOld = SelectObject( hdc, hHanFont );
//			TextOut( hdc, 0, 60, korea, sizeof(korea) - 1 );
			//SelectObject( hdc, hBig5Font );
			//TextOut( hdc, 0, 40, hwbig5, sizeof(hwbig5) - 1 );

//			SelectObject( hdc, hOld );
			

			EndPaint( hWnd, &ps );
		}
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
	default:
		return DefWindowProc( hWnd, uiMsg, wParam, lParam );
	}
	return 0;
}

extern BOOL WINAPI WinGdi_AlphaBlend(
  HDC hdcDest,
  int nXOriginDest,
  int nYOriginDest,
  int nWidthDest,
  int hHeightDest,
  HDC hdcSrc,
  int nXOriginSrc,
  int nYOriginSrc,
  int nWidthSrc,
  int nHeightSrc,
  LPBLENDFUNCTION lpBlendFunction
);


LRESULT CALLBACK WinMain_Test( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow )
{
	MSG msg;
	LOGFONT lf;
	
	
    RegisterTestClass( hInstance );
    hTestWnd = CreateWindowEx( WS_EX_CLOSEBOX|WS_EX_HELPBOX|WS_EX_OKBOX|WS_EX_TITLE, 
		strClassName, 
		"Test", 
		WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_VSCROLL, 
		AP_XSTART, AP_YSTART, AP_WIDTH, AP_HEIGHT,
		NULL,
		NULL, 
		hInstance,
		0 );

	memset( &lf, 0, sizeof(lf) );
	lf.lfCharSet = HANGUL_CHARSET;
	lf.lfHeight = 16;
	lf.lfWidth = 16;
//	hHanFont = CreateFontIndirect( &lf ); 

	lf.lfCharSet = CHINESEBIG5_CHARSET;
	lf.lfHeight = 16;
	lf.lfWidth = 16;
//	hBig5Font = CreateFontIndirect( &lf ); 

	
	
	//CreateThread( NULL, 0, ThreadProc, 1, 0, 0 );
	
	while( GetMessage( &msg, 0, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}	
    return 0;
}

