

#include <ewindows.h>

static HINSTANCE hInst;
static HWND hWnd;
static HDC hdcdown,hdctemp,hdcup;
static HBITMAP hBitmap1,hBitmap2,hBitmap3;
static HBITMAP downhBitmap = NULL,uphBitmap = NULL;

#define Democontipic_ClassName TEXT("Class_DemoContiPicMain")
#define IDC_USER 9888

#define PICWIDTH 840
#define PICHEIGH 480 

#define PICXPOS	((840 - PICWIDTH)/2)  
#define PICYPOS	((480 - PICHEIGH)/2)

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void converpic(LPTSTR filedown,LPTSTR fileup);
static void movepic(LPTSTR filedown,LPTSTR fileup);
static void picalphatoin(LPTSTR filename);
static void picalphatoout(LPTSTR filename);

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
	wcex.lpszClassName	= Democontipic_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK Democontipic_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( Democontipic_ClassName, NULL ) ;  // 查找窗口是否存在
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

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,Democontipic_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
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
			hdc = GetDC( hWnd );
			{
				RECT rect = {0,0,PICWIDTH,PICHEIGH};

				hdcdown = CreateCompatibleDC(NULL);
				hBitmap1 = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdcdown, (HGDIOBJ)hBitmap1);
				FillRect(hdcdown,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

				hdctemp = CreateCompatibleDC(NULL);
				hBitmap2 = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdctemp, (HGDIOBJ)hBitmap2);
				FillRect(hdctemp,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

				hdcup = CreateCompatibleDC(NULL);
				hBitmap3 = CreateCompatibleBitmap(hdc,rect.right, rect.bottom);
				SelectObject(hdcup, (HGDIOBJ)hBitmap3);
				FillRect(hdcup,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
			}
			ReleaseDC( hWnd, hdc );
			PostMessage(hWnd,IDC_USER,0,0);
			break;
		case IDC_USER:
#ifdef LINUX_KERNEL
			picalphatoout("./kingmos/pic1.bmp");
			picalphatoin("./kingmos/pic2.bmp");
			converpic("./kingmos/pic2.bmp","./kingmos/pic1.bmp");
			converpic("./kingmos/pic1.bmp","./kingmos/pic2.bmp");
			converpic("./kingmos/pic2.bmp","./kingmos/pic1.bmp");
			movepic("./kingmos/pic1.bmp","./kingmos/pic2.bmp");
#else
			picalphatoout("\\kingmos\\pic1.bmp");
			picalphatoin("\\kingmos\\pic2.bmp");
			converpic("\\kingmos\\pic2.bmp","\\kingmos\\pic1.bmp");
			converpic("\\kingmos\\pic1.bmp","\\kingmos\\pic2.bmp");
			converpic("\\kingmos\\pic2.bmp","\\kingmos\\pic1.bmp");
			movepic("\\kingmos\\pic1.bmp","\\kingmos\\pic2.bmp");
#endif

			if(uphBitmap != NULL)
				DeleteObject(uphBitmap);
			if(downhBitmap != NULL)
				DeleteObject(downhBitmap);
			uphBitmap = NULL;
			downhBitmap = NULL;
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			DeleteDC(hdctemp);
			DeleteDC(hdcdown);
			DeleteDC(hdcup);
			DeleteObject(hBitmap1);
			DeleteObject(hBitmap2);
			DeleteObject(hBitmap3);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

#define WIDTHRECT 60
#define HEIGHRECT 60
#define	RECTWSUM   (PICWIDTH/WIDTHRECT)
#define RECTHSUM   (PICHEIGH/HEIGHRECT)

static void converpic(LPTSTR filedown,LPTSTR fileup)
{
	HDC hdc;
	int i,j,n = 0;
	DWORD tmpx,tmpy;
	int arr[RECTWSUM][RECTHSUM];

	for(i = 0;i < RECTWSUM ;i++)
		for(j = 0;j < RECTHSUM ;j++)
			arr[i][j] = 0;

	downhBitmap = (HBITMAP)LoadImage(hInst,filedown, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(downhBitmap == NULL)
		return;
	SelectObject(hdcdown, downhBitmap);

	uphBitmap = (HBITMAP)LoadImage(hInst,fileup, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(uphBitmap == NULL)
		return;
	SelectObject(hdctemp, uphBitmap);

	hdc = GetDC(hWnd);

	BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);
	while(n < RECTWSUM*RECTHSUM)
	{
		tmpx = random();
		tmpy = random();
		
		tmpx %= RECTWSUM;
		tmpy %= RECTHSUM;

		while(arr[tmpx][tmpy] == 1 && tmpx < RECTWSUM && tmpy < RECTHSUM)
		{
			if(tmpy < (RECTHSUM - 1))
			{
				tmpy++;
			}
			else
			{
				tmpy = 0;
				tmpx++;
				if(tmpx == RECTWSUM)
					tmpx = 0;
			}
			
		}
		arr[tmpx][tmpy] = 1;

		BitBlt(hdcdown, tmpx*WIDTHRECT,tmpy*HEIGHRECT,WIDTHRECT, HEIGHRECT, hdctemp,tmpx*WIDTHRECT,tmpy*HEIGHRECT,SRCCOPY);
		BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTH, PICHEIGH, hdcdown,0,0,SRCCOPY);
		n++;
	}
	
	ReleaseDC( hWnd, hdc );

	DeleteObject(downhBitmap);
	DeleteObject(uphBitmap);
	downhBitmap = NULL;
	uphBitmap = NULL;
}

static void movepic(LPTSTR filedown,LPTSTR fileup)
{
	HDC hdc;
	int width = PICWIDTH,heigh = PICHEIGH;
	int flag = 0;
	RECT rect = {0,0,PICWIDTH,PICHEIGH};

	downhBitmap = (HBITMAP)LoadImage(hInst,filedown, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(downhBitmap == NULL)
		return;
	SelectObject(hdctemp, downhBitmap);

	uphBitmap = (HBITMAP)LoadImage(hInst,fileup, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(uphBitmap == NULL)
		return;
	SelectObject(hdcdown, uphBitmap);

	hdc = GetDC(hWnd);

	while(flag != 2)
	{
		width -= 2;

		if(flag == 0)
		{
			BitBlt(hdcup, PICWIDTH - width,0, width, PICHEIGH, hdctemp,0,0,SRCCOPY);

			BitBlt(hdcup, 0,0,PICWIDTH - width, PICHEIGH, hdcdown,width,0,SRCCOPY);
		}
		else
		{
			BitBlt(hdcup, PICWIDTH - width,0, width, PICHEIGH,hdcdown ,0,0,SRCCOPY);

			BitBlt(hdcup, 0,0,PICWIDTH - width, PICHEIGH, hdctemp,width,0,SRCCOPY);
		}
		if(width == 0)
		{
			width = PICWIDTH;
			flag ++;
		}
		
		BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY);
	}

	ReleaseDC( hWnd, hdc );
	
	DeleteObject(downhBitmap);
	DeleteObject(uphBitmap);
	downhBitmap = NULL;
	uphBitmap = NULL;

}

static void picalphatoin(LPTSTR filename)
{
	HDC hdc;
	int i = 0;
	RECT rect = {0,0,PICWIDTH,PICHEIGH};

	BLENDFUNCTION aa ;
	aa.BlendOp= AC_SRC_OVER;
	aa.BlendFlags = 0;
	aa.AlphaFormat = AC_SRC_ALPHA;

	downhBitmap = (HBITMAP)LoadImage(hInst,filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(downhBitmap == NULL)
		return;
	SelectObject(hdcdown, downhBitmap);

	hdc = GetDC(hWnd);
	
	FillRect(hdcup,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

	BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY); 
	
	while( i <= 255 )
	{
		aa.SourceConstantAlpha = i;

		FillRect(hdcup,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		AlphaBlend(hdcup,0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,PICWIDTH, PICHEIGH,aa);
	
		BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY); 
	
		i += 5;
	}
	ReleaseDC( hWnd, hdc );

	DeleteObject(downhBitmap);
	downhBitmap = NULL;

}

static void picalphatoout(LPTSTR filename)
{
	HDC hdc;
	int i = 255;
	RECT rect = {0,0,PICWIDTH,PICHEIGH};

	BLENDFUNCTION aa ;
	aa.BlendOp= AC_SRC_OVER;
	aa.BlendFlags = 0;
	aa.AlphaFormat = AC_SRC_ALPHA;

	downhBitmap = (HBITMAP)LoadImage(hInst,filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE); //调入文件
	if(downhBitmap == NULL)
		return;
	SelectObject(hdcdown, downhBitmap);

	hdc = GetDC(hWnd);

	while( i >= 0 )
	{
		aa.SourceConstantAlpha = i;

		FillRect(hdcup,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		AlphaBlend(hdcup,0,0,PICWIDTH, PICHEIGH, hdcdown,0,0,PICWIDTH, PICHEIGH,aa);
	
		BitBlt(hdc, PICXPOS,PICYPOS,PICWIDTH, PICHEIGH, hdcup,0,0,SRCCOPY); 
	
		i -= 5;
	}
	ReleaseDC( hWnd, hdc );

	DeleteObject(downhBitmap);
	downhBitmap = NULL;

}


