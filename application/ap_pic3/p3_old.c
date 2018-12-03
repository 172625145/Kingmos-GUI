
#include <ewindows.h>
static HINSTANCE hInst;
static HWND hWnd;
static HBITMAP hBitmap1,hBitmap2,hBitmap3,downhBitmap = NULL,uphBitmap = NULL;
static HDC hdcdown,hdcup,hdctemp;

#define DemoP3_ClassName TEXT("Class_DemoP3Main")
#define PICWIDTH  840
#define PICHEIGH  480
#define TIMER_ID  1001
#define WM_USERP3 8989

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoLoadBmp(HWND hWnd,LPTSTR downFile,LPTSTR upfile);
//static void OnTime(HWND hWnd);
static void OnTime(HDC hdc);

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
//	wcex.hbrBackground	= GetStockObject(BLACK_BRUSH);
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= DemoP3_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK WinMain_P3(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;
	extern void mousetest();
	mousetest();
	hWnd = FindWindow( DemoP3_ClassName, NULL ) ;  // 查找窗口是否存在
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
//			TextOut(hdc, 200, 200, "MicroLogical", sizeof("MicroLogical") - 1);
			OnTime(hdc);

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

				hdctemp = CreateCompatibleDC(NULL);
				hBitmap3 = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdctemp, (HGDIOBJ)hBitmap3);
				FillRect(hdctemp,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

			}
			ReleaseDC( hWnd, hdc );
			ASSERT(SetTimer(hWnd, TIMER_ID, 50, NULL));
//			DoLoadBmp(hWnd,"\\kingmos\\pic1.bmp","\\kingmos\\pic2.bmp");
			DoLoadBmp(hWnd,"./kingmos/pic1.bmp","./kingmos/pic2.bmp");
			PostMessage(hWnd, WM_USERP3, 0, 0);
			RETAILMSG(1,TEXT("WM_CREATE is over "));
			
			break;
		case WM_TIMER:
			if((DWORD)wParam == TIMER_ID)
			{
				RETAILMSG(1,TEXT("TIMER enter \r\n"));
//				InvalidateRect(hWnd, NULL, FALSE);
				HDC hdc = GetDC(hWnd);
				OnTime(hdc);
				ReleaseDC(hWnd, hdc);
				RETAILMSG(1,TEXT("TIMER leave \r\n"));
				break;
			}
			else
			{
				return 1;
			}
			
//			OnTime(hWnd);
			
		case WM_USERP3:
//			while(10)
			{
//				OnTime(hWnd);
			}
			break;
		case WM_KEYUP:
			if(wParam == VK_BACK)
				DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			KillTimer(hWnd,TIMER_ID);
			DeleteObject(hBitmap1);
			DeleteObject(hBitmap2);
			DeleteObject(hBitmap3);
			DeleteDC(hdcdown); 
			DeleteDC(hdcup);
			DeleteDC(hdctemp);
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
		RETAILMSG(1,TEXT("LoadImage1 is failure "));
		return;
	}

	
	uphBitmap = (HBITMAP)LoadImage(hInst,upfile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(uphBitmap == NULL)
	{
		RETAILMSG(1,TEXT("LoadImage2 is failure "));
		return;
	}
	SelectObject(hdcdown, downhBitmap);
	SelectObject(hdcup, uphBitmap);

}

#if 1
#define MOVEVAL 20
#define ALPHVAL 32	
static void OnTime(HDC hdc)
{
//	HDC hdc;
	static int i = 680;
	static int flag = 0;
	static int alvalue = 0;

	BLENDFUNCTION aa ;
	aa.BlendOp= AC_SRC_OVER;
	aa.BlendFlags = 0;
	aa.AlphaFormat = AC_SRC_ALPHA;

//	RETAILMSG(1,TEXT("TIMER enter \r\n"));

	alvalue += ALPHVAL;
	if(alvalue >= 256)
		alvalue = 255;

//	hdc = GetDC(hWnd);

	i += MOVEVAL;

	if(i > PICWIDTH)
	{
		i = 680;
		alvalue = 0;
		flag++;
		flag %= 2;
	}
		
	aa.SourceConstantAlpha = alvalue;

	if(flag == 0)
	{
		BitBlt(hdctemp, 0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);
		AlphaBlend(hdctemp,PICWIDTH - i,0,i, PICHEIGH, hdcup,0,0,i, PICHEIGH,aa);
	}
	else
	{
		BitBlt(hdctemp, 0,0,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY);
		AlphaBlend(hdctemp,PICWIDTH - i,0,i, PICHEIGH, hdcdown,0,0,i, PICHEIGH,aa);
	}
	
	BitBlt(hdc, 0,0,PICWIDTH, PICHEIGH, hdctemp,0,0,SRCCOPY);
//	RETAILMSG(1, ("111111111111111111\r\n"));
	TextOut(hdc, 20, 200, "MicroLogical", sizeof("MicroLogical") - 1);
	TextOut(hdc, 20, 220, "滚滚长江东逝水，浪花淘尽英雄。", sizeof("滚滚长江东逝水，浪花淘尽英雄。") - 1);
	TextOut(hdc, 20, 240, "是非成败转头空。", sizeof("是非成败转头空。") - 1);
	TextOut(hdc, 20, 260, "青山依旧在，几度夕阳红。", sizeof("青山依旧在，几度夕阳红。") - 1);
	TextOut(hdc, 20, 280, "白发渔樵江渚上，惯看秋月春风。", sizeof("白发渔樵江渚上，惯看秋月春风。") - 1);
	TextOut(hdc, 20, 300, "一壶浊酒喜相逢。", sizeof("一壶浊酒喜相逢。") - 1);
	TextOut(hdc, 20, 320, "古今多少事，都付笑谈中。", sizeof("古今多少事，都付笑谈中。") - 1);

//	RETAILMSG(1, ("2222222222222222222222222\r\n"));
//	ReleaseDC( hWnd, hdc );
//	RETAILMSG(1,TEXT("TIMER leave \r\n"));
}
#endif
#if 0
#define MOVEVAL 20
#define ALPHVAL 32	
static void OnTime(HWND hWnd)
{
	HDC hdc;
	static int i = 680;
	static int flag = 0;
	static int alvalue = 0;

	BLENDFUNCTION aa ;
	aa.BlendOp= AC_SRC_OVER;
	aa.BlendFlags = 0;
	aa.AlphaFormat = AC_SRC_ALPHA;

	RETAILMSG(1,TEXT("TIMER is enter \r\n"));

	alvalue += ALPHVAL;
	if(alvalue >= 256)
		alvalue = 255;

	hdc = GetDC(hWnd);

	i += MOVEVAL;

	if(i > PICWIDTH)
	{
		i = 680;
		alvalue = 0;
		flag++;
		flag %= 2;
	}
		
	aa.SourceConstantAlpha = alvalue;

	if(flag == 0)
	{
		BitBlt(hdctemp, 0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);

		AlphaBlend(hdctemp,PICWIDTH - i,0,i, PICHEIGH, hdcup,0,0,i, PICHEIGH,aa);
	}
	else
	{
		BitBlt(hdctemp, 0,0,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY);

		AlphaBlend(hdctemp,PICWIDTH - i,0,i, PICHEIGH, hdcdown,0,0,i, PICHEIGH,aa);
	}
	
	BitBlt(hdc, 0,0,PICWIDTH, PICHEIGH, hdctemp,0,0,SRCCOPY);
//	RETAILMSG(1, ("2222222222222222222222222\r\n"));
	ReleaseDC( hWnd, hdc );
}
#endif
