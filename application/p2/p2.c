
#include <ewindows.h>


static HINSTANCE hInst;
static HWND hWnd;
static HBITMAP hBitmap1,hBitmap2,downhBitmap = NULL,uphBitmap = NULL;
static HDC hdcdown,hdcup;

#define DemoP2_ClassName TEXT("Class_DemoP2Main")
#define PICWIDTH  840
#define PICHEIGH  480

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoLoadBmp(HWND hWnd,LPTSTR downFile,LPTSTR upfile);
static void OnTime(HWND hWnd);


static	ATOM	Main_RegClass(HINSTANCE hInstance)
{
	WNDCLASS	wcex;

	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)Main_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground	= NULL;//GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= DemoP2_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK WinMain_P2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( DemoP2_ClassName, NULL ) ;  // 查找窗口是否存在
	if( hWnd != 0 )
	{ // 窗口已经存在
		SetForegroundWindow( hWnd ) ; // 设置窗口到前台
		return FALSE ;
	}

	Main_RegClass(hInstance);

	if (!Main_InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int width,heigh;
   
	hInst = hInstance;
   

    width = GetSystemMetrics(SM_CXSCREEN);
	heigh = GetSystemMetrics(SM_CYSCREEN);

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,DemoP2_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);
	return TRUE;
}

static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;


	switch (message) 
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;
		case WM_CREATE:
			hdc = GetDC(hWnd);
			{
				RECT rect = {0,0,PICWIDTH,PICHEIGH};

				hdcdown = CreateCompatibleDC(NULL);
				hBitmap1 = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdcdown, (HGDIOBJ)hBitmap1);
				FillRect(hdcdown,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
				
				hdcup = CreateCompatibleDC(NULL);
				hBitmap2 = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdcup, (HGDIOBJ)hBitmap2);
				FillRect(hdcup,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
			}
			ReleaseDC( hWnd, hdc );
			SetTimer(hWnd,WM_TIMER,1000,NULL);
			//DoLoadBmp(hWnd,"./kingmos/pic1.bmp","./kingmos/pic2.bmp");

#ifdef LINUX_KERNEL
			DoLoadBmp(hWnd,"./kingmos/pic1.bmp","./kingmos/pic2.bmp");
#else
			DoLoadBmp(hWnd,"\\kingmos\\pic1.bmp","\\kingmos\\pic2.bmp");
#endif

			break;
		case WM_TIMER:
			OnTime(hWnd);
			DestroyWindow(hWnd);
			break;
/*		case WM_KEYUP:
			if(wParam == VK_BACK)
				DestroyWindow(hWnd);
			break;
*/		case WM_DESTROY:
			KillTimer(hWnd,WM_TIMER);
			DeleteObject(hBitmap1);
			DeleteObject(hBitmap2);
			DeleteDC(hdcdown); 
			DeleteDC(hdcup);
			DeleteObject(downhBitmap);
			DeleteObject(uphBitmap);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

static void DoLoadBmp(HWND hWnd,LPTSTR downFile,LPTSTR upfile)
{
	downhBitmap = (HBITMAP)LoadImage(hInst,downFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(downhBitmap == NULL)
		return;

	uphBitmap = (HBITMAP)LoadImage(hInst,upfile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(uphBitmap == NULL)
		return;
	SelectObject(hdcdown, downhBitmap);
	SelectObject(hdcup, uphBitmap);

}

#define MOVEXVAL 8
#define MOVEYVAL 5

static void OnTime(HWND hWnd)
{
	HDC hdc;
	static int ix = -14;
	static int iy = 0;
	static int flag = 0;
	int j = 0;

	hdc = GetDC(hWnd);
	if(ix == 0)
		BitBlt(hdc, 0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);

	for(j = 0;j<(48*8);j++)
	{

		if(ix > PICWIDTH)
		{
			ix = -14;
			iy = 0;
			flag++;
			flag %= 2;
		}
			
		ix += MOVEXVAL;
		iy += MOVEYVAL;


		if(flag == 0)
			BitBlt(hdc, PICWIDTH/2 - ix,PICHEIGH/2 - iy,ix * 2, iy * 2, hdcup,PICWIDTH/2 - ix,PICHEIGH/2 - iy,SRCCOPY);
		else
			BitBlt(hdc, PICWIDTH/2 - ix,PICHEIGH/2 - iy,ix * 2, iy * 2, hdcdown,PICWIDTH/2 - ix,PICHEIGH/2 - iy,SRCCOPY);

	}


	ReleaseDC( hWnd, hdc );

}