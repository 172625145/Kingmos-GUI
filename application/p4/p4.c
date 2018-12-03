
#include <ewindows.h>


static HINSTANCE hInst;
static HWND hWnd;
static HBITMAP hBitmap1,hBitmap2,hBitmap3,filehBitmap;
static HDC hdcscr,hdcmid,hdcsma;

#define DemoP4_ClassName TEXT("Class_DemoP4Main")
#define PICSMAX    220
#define PICSMAY    130
#define PICMIDX    260
#define PICMIDY	   170

#define PICSCRX    840
#define PICSCRY	   480

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnTime(HWND hWnd);
static void DoLoadPic(HWND hWnd);


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
	wcex.lpszClassName	= DemoP4_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK WinMain_P4(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( DemoP4_ClassName, NULL ) ;  // 查找窗口是否存在
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

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,DemoP4_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
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
				RECT rect1 = {0,0,PICSCRX,PICSCRY},rect2 = {0,0,PICMIDX,PICMIDY},rect3 = {0,0,PICSMAX,PICSMAY};

				hdcscr = CreateCompatibleDC(NULL);
				hBitmap1 = CreateCompatibleBitmap(hdc,rect1.right, rect1.bottom);
				SelectObject(hdcscr, (HGDIOBJ)hBitmap1);
				FillRect(hdcscr,&rect1,(HBRUSH)GetStockObject(BLACK_BRUSH));
				
				hdcmid = CreateCompatibleDC(NULL);
				hBitmap2 = CreateCompatibleBitmap(hdc,rect2.right, rect2.bottom);
				SelectObject(hdcmid, (HGDIOBJ)hBitmap2);
				FillRect(hdcmid,&rect2,(HBRUSH)CreateSolidBrush(RGB(255,192,0)));

				hdcsma = CreateCompatibleDC(NULL);
				hBitmap3 = CreateCompatibleBitmap(hdc,rect3.right, rect3.bottom);
				SelectObject(hdcsma, (HGDIOBJ)hBitmap3);
				FillRect(hdcsma,&rect3,(HBRUSH)GetStockObject(WHITE_BRUSH));
			}
			ReleaseDC( hWnd, hdc );
			SetTimer(hWnd,WM_TIMER,1000,NULL);
			DoLoadPic(hWnd);
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
			DeleteObject(hBitmap3);
			DeleteDC(hdcscr); 
			DeleteDC(hdcmid); 
			DeleteDC(hdcsma);
			DeleteObject(filehBitmap);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

#define PICVAL 10
static void DoLoadPic(HWND hWnd)
{
#ifdef LINUX_KERNEL
	LPTSTR filename = "./kingmos/psm1.bmp";
#else
	LPTSTR filename = "\\kingmos\\psm1.bmp";
#endif
	int i,j,posx,posy;

	filehBitmap = (HBITMAP)LoadImage(hInst,filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(filehBitmap == NULL)
		return;
	SelectObject(hdcsma, filehBitmap);

	for(i = 0;i < 3;i++)
	{
		posx = (PICSCRX - PICVAL*2 - PICSMAX*3)/2;
		posy = (PICSCRY - PICVAL*2 - PICSMAY*3)/2 + (PICSMAY + PICVAL)*i;
		for(j = 0;j < 3; j++)
		{
			BitBlt(hdcscr, posx,posy,PICSMAX, PICSMAY, hdcsma,0,0,SRCCOPY);
			posx = posx + (PICSMAX + PICVAL);
		}
	}
}

static void OnTime(HWND hWnd)
{
	HDC hdc;
	int posx,posy,j;
	static int i = 0;

	hdc = GetDC(hWnd);
	
	for(j = 0;j<24;j++)
	{
		if(i >= 6)
			i = 0;

		if(i < 3)
		{
			posx = (PICSCRX - PICVAL*2 - PICSMAX*3)/2 + (PICSMAX + PICVAL)*i - (PICMIDX - PICSMAX)/2;
			posy = (PICSCRY - PICVAL*2 - PICSMAY*3)/2 - (PICMIDY - PICSMAY)/2;
		}
		else
		{
			posx = (PICSCRX - PICVAL*2 - PICSMAX*3)/2 + (PICSMAX + PICVAL)*(5-i) - (PICMIDX - PICSMAX)/2;
			posy = (PICSCRY - PICVAL*2 - PICSMAY*3)/2 + (PICSMAY + PICVAL) - (PICMIDY - PICSMAY)/2;
		}




		BitBlt(hdc,0,0,PICSCRX, PICSCRY, hdcscr,0,0,SRCCOPY);

		StretchBlt(hdcmid,3,3,PICMIDX-6,PICMIDY-6,hdcsma,0,0,PICSMAX,PICSMAY,SRCCOPY);

		BitBlt(hdc,posx,posy,PICMIDX, PICMIDY, hdcmid,0,0,SRCCOPY);

		i++;
		Sleep(700);
	}
	ReleaseDC( hWnd, hdc );


}