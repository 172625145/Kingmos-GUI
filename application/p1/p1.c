
#include <ewindows.h>


static HINSTANCE hInst;
static HWND hWnd;
static HBITMAP hBitmap1,hBitmap2,downhBitmap = NULL,uphBitmap = NULL;
static HDC hdcdown,hdcup;

#define DemoP1_ClassName TEXT("Class_DemoP1Main")
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
//	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= DemoP1_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK WinMain_P1(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;
	RETAILMSG(1, ("p1 000000000 \r\n"));
	hWnd = FindWindow( DemoP1_ClassName, NULL ) ;  // 查找窗口是否存在
	if( hWnd != 0 )
	{ // 窗口已经存在
		SetForegroundWindow( hWnd ) ; // 设置窗口到前台
		return FALSE ;
	}

	ASSERT(Main_RegClass(hInstance));
	RETAILMSG(1, ("p1 11111 \r\n"));
	if (!Main_InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
	RETAILMSG(1, ("p1 22222 \r\n"));
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

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,DemoP1_ClassName, NULL, WS_VISIBLE | WS_POPUP,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
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
	{
		ASSERT(0);
		return;
	}

	uphBitmap = (HBITMAP)LoadImage(hInst,upfile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(uphBitmap == NULL)
	{
		ASSERT(0);
		return;
	}
	SelectObject(hdcdown, downhBitmap);
	SelectObject(hdcup, uphBitmap);

}

#define MOVEVAL 5
static void OnTime(HWND hWnd)
{
	HDC hdc;
	static int i = 0,j;
	static int flag = 0;

	hdc = GetDC(hWnd);
	RETAILMSG(1, ("p1 timer 11111 \r\n"));
	if(i == 0)
	{
		BitBlt(hdc, 0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);
	}
	for(j = 0;j < (84*4);j++)
	{

//		Sleep(1000);
		if(i > PICWIDTH)
		{
			i = 0;
			flag++;
			flag %= 2;
		}
//		RETAILMSG(1, ("p1 timer draw \r\n"));			
		i += MOVEVAL;

		if(flag == 0)
			BitBlt(hdc, PICWIDTH - i,0,i, PICHEIGH, hdcup,PICWIDTH - i,0,SRCCOPY);
		else
			BitBlt(hdc, PICWIDTH - i,0,i, PICHEIGH, hdcdown,PICWIDTH - i,0,SRCCOPY);
	}
	RETAILMSG(1, ("p1 timer 22222 \r\n"));
	ReleaseDC( hWnd, hdc );

}