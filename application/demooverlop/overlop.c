//-------


#include <ewindows.h>


static HINSTANCE hInst;
static HWND hWnd;
static HDC hdcdown,hdcup,hdcTemp;
static HBITMAP  downhBitmap = NULL,uphBitmap = NULL;
static HBITMAP hBitmap1,hBitmap2,hBitmap3;

#define DemoOverLop_ClassName TEXT("Class_DemoOverLopMain")

#define PICWIDTHDOWN 700
#define PICHEIGHDOWN 400
#define PICWIDTHUP   600
#define	PICHEIGHUP   400
#define PICXPOS		 ((840 - PICWIDTHDOWN)/2)	
#define PICYPOS		 ((480 - PICHEIGHDOWN)/2)	

#define WM_USER_OVERLOP 8887 


static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DOoOverLopBmp(HWND hWnd,LPTSTR downFile,LPTSTR upfile);
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
	wcex.hbrBackground	= GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= DemoOverLop_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK DemoOverLop_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( DemoOverLop_ClassName, NULL ) ;  // 查找窗口是否存在
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

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,DemoOverLop_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
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
//			BitBlt(hdc, PICXPOS, PICYPOS,PICWIDTHDOWN, PICHEIGHDOWN, hdcTemp,0,0,SRCCOPY); 
			EndPaint(hWnd, &ps);
			break;
		case WM_CREATE:
			hdc = GetDC( hWnd );
			{
				RECT rect1 = {0,0,PICWIDTHDOWN,PICHEIGHDOWN},rect2 = {0,0,PICWIDTHUP,PICHEIGHUP};

				hdcdown = CreateCompatibleDC(NULL);
				hBitmap1 = CreateCompatibleBitmap(hdc,rect1.right, rect1.bottom);
				SelectObject(hdcdown, (HGDIOBJ)hBitmap1);
				FillRect(hdcdown,&rect1,(HBRUSH)GetStockObject(WHITE_BRUSH));
				
				hdcTemp = CreateCompatibleDC(NULL);
				hBitmap2 = CreateCompatibleBitmap(hdc,rect1.right, rect1.bottom);
				SelectObject(hdcTemp, (HGDIOBJ)hBitmap2);
				FillRect(hdcTemp,&rect1,(HBRUSH)GetStockObject(WHITE_BRUSH));
				
				hdcup = CreateCompatibleDC(NULL);
				hBitmap3 = CreateCompatibleBitmap(hdc,rect2.right, rect2.bottom);
				SelectObject(hdcup, (HGDIOBJ)hBitmap3);
				FillRect(hdcup,&rect2,(HBRUSH)GetStockObject(WHITE_BRUSH));
			}
			ReleaseDC( hWnd, hdc );
			PostMessage(hWnd,WM_USER_OVERLOP,NULL,NULL);
			break;
		case WM_USER_OVERLOP:
#ifdef LINUX_KERNEL
			DOoOverLopBmp(hWnd,"./kingmos/down2.bmp","./kingmos/rnu.bmp");
#else
			DOoOverLopBmp(hWnd,"/kingmos/down2.bmp","/kingmos/rnu.bmp");
#endif
			OnTime(hWnd);
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			DeleteObject(hBitmap1);
			DeleteObject(hBitmap2);
			DeleteObject(hBitmap3);
			DeleteDC(hdcdown); 
			DeleteDC(hdcTemp);
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

//处理24位图像
static void DOoOverLopBmp(HWND hWnd,LPTSTR downFile,LPTSTR upfile)
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

#define MOVEVAL 2
static void OnTime(HWND hWnd)
{
	static int i = 0,j;
	static int sign = 0;
	RECT rect1 = {0,0,PICWIDTHDOWN,PICHEIGHDOWN},rect2 = {0,0,PICWIDTHUP,PICHEIGHUP};
	HDC hdc;

	hdc = GetDC(hWnd);
	for(j = 0;j<200;j++)
	{
		BitBlt(hdcTemp,0,0,PICWIDTHDOWN, PICHEIGHDOWN,hdcdown,0,0,SRCCOPY);

		BitBlt(hdcTemp,i,0,PICWIDTHUP, PICHEIGHUP,hdcup,0,0,SRCAND);
		if(sign == 0)
			i += MOVEVAL;
		else
			i -= MOVEVAL;
		if(i >= (PICWIDTHDOWN - PICWIDTHUP))
			sign = 1;
		else if(i <= 0)
			sign = 0;

		BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTHDOWN, PICHEIGHDOWN, hdcTemp,0,0,SRCCOPY); 
	}
	ReleaseDC( hWnd, hdc );
}
