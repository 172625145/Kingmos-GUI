#include <ewindows.h>
#include <gif2bmp.h>
#include <gifbmp.h>

static const TCHAR szGifClass[]="_Gif_Test_Class_";
static HGIF	g_hGif = 0 ;

static ATOM GifRegisterClass( HINSTANCE hInstance );

static void DrawBitMap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
	HDC hMemoryDC;
	if( hBitMap == 0)
		return;
	hMemoryDC = CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc, (short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top),hMemoryDC,(short)xOffset,(short)yOffset,dwRop);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WinMain_Gif( HINSTANCE hInstance,HINSTANCE hPrevInstance,
            LPSTR     lpCmdLine,int       nCmdShow)
{
	HWND					hWnd;
	RECT					rt;
	MSG						msg;

	GifRegisterClass( hInstance );
	GetWindowRect( GetDesktopWindow(), &rt );
	hWnd = CreateWindowEx( WS_EX_CLOSEBOX, szGifClass, "", WS_VISIBLE ,
      					rt.left,
						rt.top, 
						rt.right - rt.left,
						rt.bottom - rt.top,
						0, NULL, hInstance, NULL);

	if (!hWnd){
		return 0;
	}

	while (GetMessage(&msg, 0, 0, 0)) {	
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 1;
}
static BOOL OnGifPaint( HWND hWnd )
{
	HDC					hdc ;
	PAINTSTRUCT			ps;
	RECT				rt ;
	HBITMAP				hBitmap ;


	GetClientRect( hWnd, &rt ) ;
	hdc = BeginPaint( hWnd, &ps );	

	g_hGif = LoadGif( "\\a.gif" ) ;
	hBitmap = GetBitmapHandle( g_hGif, 0 ) ;
	if( hBitmap != NULL )
		DrawBitMap( hdc, hBitmap, rt, 0, 0, SRCCOPY ) ;

	EndPaint( hWnd, &ps );
	return TRUE;
}
static BOOL OnGifCreate( HWND hWnd )
{
	// just for test
	g_hGif = LoadGif( "\\a.gif" ) ;
	if( g_hGif != NULL )
		return TRUE ;
	else
		return FALSE ;
}
//=============================================================================================
static LRESULT CALLBACK GifWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message) 
	{
		case WM_CREATE:
			if( OnGifCreate( hWnd ) == FALSE )
				return -1;
			return 0;
		case WM_PAINT:
			OnGifPaint(hWnd);
			return 0;
		case WM_DESTROY:
			if( g_hGif != 0 )
			{
				DestroyGif( g_hGif ) ;
				g_hGif = 0;
			}
			PostQuitMessage( 0 );
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return FALSE;
}
static ATOM GifRegisterClass( HINSTANCE hInstance )
{
	WNDCLASS    wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)GifWndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject(1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szGifClass;

	return RegisterClass(&wc);
}
