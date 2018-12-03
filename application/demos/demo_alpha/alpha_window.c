/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：设备管理，用户界面部分
版本号：1.0.0
开发时期：2003-06-18
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <ewindows.h>

/***************  全局区 定义， 声明 *****************/
#define MAX_SEARCHSTRINGLEN	64

#define ID_EDIT		100

TCHAR szWindowClass[] = "AlphaClass";								// The title bar text

static HBITMAP hbmpBackground = NULL;
static HBITMAP hbmpKingmoGUI = NULL;
static HBITMAP hbmpMake = NULL;

static ATOM RegistertestClass(HINSTANCE hInstance);

static LRESULT CALLBACK AlphaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);



// ********************************************************************
//声明：ATOM RegisterAlphaClass(HINSTANCE hInstance)
//参数：
//	IN hInstance - 当前应用程序的实例句柄
//返回值：
//	成功，返回非零，不成功，返回零。
//功能描述：注册当前应用程序的类
//引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegistertestClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
//	HBITMAP hBitmap;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)AlphaWndProc; // 设备管理窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// 装载应用程序图标
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= CreateSolidBrush( RGB( 0, 0, 255 ) );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 设备管理窗口类名

	return RegisterClass(&wc); // 注册类

}

// ********************************************************************
//声明：static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	返回0，将继续创建窗口，返回-1，则会破坏窗口
//功能描述：应用程序处理创建消息
//引用: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if( hbmpBackground == NULL )
		hbmpBackground = LoadImage( NULL, "./kingmos/demos/alpha_bk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if( hbmpKingmoGUI == NULL )
		hbmpKingmoGUI = LoadImage( NULL, "./kingmos/demos/kingmosgui.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if( hbmpMake == NULL )
	{
		hbmpMake = CreateCompatibleBitmap( NULL, 240, 320 );
	}
	SetTimer( hWnd, 0, 50, NULL );

	return 0;
}

#define MARK_WIDTH 103
#define MARK_HEIGHT 102
static int DoPaint( HWND hWnd, HDC hdc, int x, int y, int alpha )
{
	HDC hMemMakeDC = CreateCompatibleDC( hdc );
	HDC hMemDC = CreateCompatibleDC( hdc );
	BLENDFUNCTION_EX bfex;

	SelectObject( hMemMakeDC, hbmpMake );
	SelectObject( hMemDC, hbmpBackground );
	BitBlt( hMemMakeDC, 0, 0, 240, 320, hMemDC, 0, 0, SRCCOPY );

	SelectObject( hMemDC, hbmpKingmoGUI );
	bfex.bf.AlphaFormat = AC_SRC_ALPHA;
	bfex.bf.BlendFlags = 0;
	bfex.bf.BlendOp = AC_SRC_OVER;
	bfex.bf.SourceConstantAlpha = alpha;

	bfex.rgbTransparent = RGB( 0xff, 0, 0xff );


	AlphaBlendEx( hMemMakeDC, x, y, MARK_WIDTH, MARK_HEIGHT, hMemDC, 0, 0, MARK_WIDTH, MARK_HEIGHT, &bfex );

	BitBlt( hdc, 0, 0, 240, 320, hMemMakeDC, 0, 0, SRCCOPY );

	DeleteDC( hMemDC );
	DeleteDC( hMemMakeDC );
	return 0;
}

// ********************************************************************
//声明：static LRESULT CALLBACK RgnWndProc(HWND , UINT , WPARAM , LPARAM )
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN message - 过程消息
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	不同的消息有不同的返回值，具体看消息本身
//功能描述：应用程序窗口过程函数
//引用: 
// ********************************************************************
static LRESULT CALLBACK AlphaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_TIMER:
		{
			static int x = 0;
			static int y = 0;
			static int xdir = 1;
			static int ydir = 1;
			static int alpha = 0;
			static int alphadir = 5;
			HDC hdc = GetDC( hWnd );
			
			
			x += xdir;
			y += ydir;
			alpha += alphadir;

			DoPaint( hWnd, hdc, x, y, alpha );

			if( x >= 230 || x < 0 )
				xdir = -xdir;
			if( y >= 280 || y < 0 )
				ydir = -ydir;
			if( alpha >= 255 || alpha <= 0 )
				alphadir = -alphadir;
			ReleaseDC( hWnd, hdc );			
		}
		return 0;
	case WM_PAINT: // 绘制窗口
		hdc = BeginPaint(hWnd, &ps);
		//			DoPaint(hWnd,hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND: // 命令消息
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE: // 创建消息
		return DoCreateWindow(hWnd,wParam,lParam);
	case WM_DESTROY: // 破坏消息
		PostQuitMessage(0); // 退出程序
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
   return 0;
}


// ********************************************************************
//声明：static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//参数：
//	IN hInstance - 当前应用程序的实例句柄
//  IN nCmdShow  - 应用程序显示命令
//返回值：
//	成功返回TRUE, 失败返回FALSE
//功能描述：初始化当前实例，即创建当前主窗口
//引用: 
// ********************************************************************
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   // 创建窗口
   hWnd = CreateWindowEx( WS_EX_CLOSEBOX,szWindowClass, "Alpha窗口", WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE,
      0, 0, 240, 320, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // 创建窗口失败
      return FALSE;
   }

	// 创建窗口成功
   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 显示窗口

   return TRUE;
}

// ********************************************************************
//声明：void WinMain_test(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
//参数：
//	IN hInstance - 当前应用程序的实例句柄
//  IN hPrevInstance   - 前一个应用程序的实例句柄
//  IN lpCmdLine   - 调用应用程序时的参数
//  IN nCmdShow  - 应用程序显示命令
//返回值：
//	成功，返回非零，不成功，返回零。
//功能描述：装载应用程序
//引用: 被 系统 调用
// ********************************************************************
#ifdef INLINE_PROGRAM
int WINAPI WinMain_DemoAlpha(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
 	// TODO: Place code here.
	MSG msg;
	HWND hWnd;
//	char buf[128];

	//hInst = hInstance;
	
	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找当前程序是否已经运行
	if( hWnd != 0 )
	{ // 已经运行
		SetForegroundWindow( hWnd ) ; // 设置窗口到最前面
		return FALSE ;
	}
	RegistertestClass(hInstance); // 注册设备管理类

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))  // 初始化应用程序
	{
		return FALSE;
	}


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam; // 退出应用程序
}
