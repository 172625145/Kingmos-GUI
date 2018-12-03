
#include <ewindows.h>


static HINSTANCE hInst;
static HWND hWnd;
static HBITMAP hBitmap1,hBitmap2,downhBitmap = NULL,uphBitmap = NULL;
static HDC hdcdown,hdcup;

#define DemoP3_ClassName TEXT("Class_DemoP3Main")
#define PICWIDTH  840
#define PICHEIGH  480

#define WM_USERP3  9999

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoLoadBmp(HWND hWnd,LPTSTR downFile,LPTSTR upfile);
static void OnTime(HWND hWnd);


static	inline	ATOM	Main_RegClass(HINSTANCE hInstance)
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
	wcex.lpszClassName	= DemoP3_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK WinMain_P3(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( DemoP3_ClassName, NULL ) ;  // ���Ҵ����Ƿ����
	if( hWnd != 0 )
	{ // �����Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ�ǰ̨
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

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,DemoP3_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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
				SelectObject(hdcup, (HGDIOBJ)hBitmap1);
				FillRect(hdcup,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
			}
			ReleaseDC( hWnd, hdc );
			DoLoadBmp(hWnd,"./kingmos/pic1.bmp","./kingmos/pic2.bmp");
			PostMessage(hWnd,WM_USERP3,0,0);
			break;
		case WM_USERP3:
			OnTime(hWnd);
			break;
		case WM_KEYUP:
			if(wParam == VK_BACK)
				DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			KillTimer(hWnd,WM_TIMER);
			DeleteObject(hBitmap1);
			DeleteObject(hBitmap2);
			DeleteDC(hdcdown); 
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
	downhBitmap = (HBITMAP)LoadImage(hInst,downFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //�����ļ�
	if(downhBitmap == NULL)
		return;

	uphBitmap = (HBITMAP)LoadImage(hInst,upfile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //�����ļ�
	if(uphBitmap == NULL)
		return;
	SelectObject(hdcdown, downhBitmap);
	SelectObject(hdcup, uphBitmap);

}

static void OnTime(HWND hWnd)
{
	HDC hdc;
	DWORD time3,time1,time2;
	hdc = GetDC(hWnd);
	while(1)
	{
		time1 = GetTickCount();
		BitBlt(hdc, 0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);
		time2 = GetTickCount();
		BitBlt(hdc, 0,0,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY);
		time3 = GetTickCount();

		RETAILMSG(1,(TEXT("pic1 tick is %d,pic2 tick is %d \r\n"),(time2 - time1),(time3 - time2)));
	}
	ReleaseDC( hWnd, hdc );
}