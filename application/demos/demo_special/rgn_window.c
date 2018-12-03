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


static TCHAR szWindowClass[] = "rgnap";								// The title bar text

static ATOM RegistertestClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

static LRESULT CALLBACK RgnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCloseWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);



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
int WINAPI WinMain_DemoRgnWindow(HINSTANCE hInstance,
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

// ********************************************************************
//声明：ATOM RegistertestClass(HINSTANCE hInstance)
//参数：
//	IN hInstance - 当前应用程序的实例句柄
//返回值：
//	成功，返回非零，不成功，返回零。
//功能描述：注册当前应用程序的类
//引用: 被 应用程序入口程序 调用
// ********************************************************************
static ATOM RegistertestClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)RgnWndProc; // 设备管理窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
//	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_DEVICEMANAGE),IMAGE_ICON,16,16,0);
	wc.hIcon			= NULL;//LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// 装载应用程序图标
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= CreateSolidBrush( RGB( 0, 0, 0xff ) );//GetStockObject(WHITE_BRUSH); // 设置背景刷
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 设备管理窗口类名

	return RegisterClass(&wc); // 注册类

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
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX,szWindowClass, "特效窗口", WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE,
      60, 40, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // 创建窗口失败
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 显示窗口

   return TRUE;
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
static LRESULT DoCreateRgnWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rt, rcClient;
	HRGN hrgn, hrgnDiff, hrgnOr;
	POINT pt;
	int xmp, ymp;

	GetWindowRect(hWnd, &rt); // 得到窗口屏幕矩形

	GetClientRect(hWnd, &rcClient );
	pt.x = pt.y = 0;
	ClientToScreen( hWnd, &pt );
	//OffsetRect( &rcClient, pt.x, pt.y );

	OffsetRect( &rcClient, pt.x - rt.left, pt.y - rt.top );


	
	OffsetRect( &rt, -rt.left, -rt.top ); // log cor

	hrgn = CreateRectRgn( 0, 0, 0, 0 );//rt.left, rt.top, rt.right, rt.bottom );
	GetWindowRgn( hWnd, hrgn );

	hrgnDiff = CreateRectRgn( rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );

	CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	DeleteObject( hrgnDiff );

	xmp = ( rcClient.right + rcClient.left ) / 2;
	ymp = ( rcClient.bottom + rcClient.top ) / 2;

	hrgnOr = CreateRectRgn( xmp - 16 , ymp - 16, xmp + 16, ymp + 16 );

	CombineRgn( hrgn, hrgn, hrgnOr, RGN_OR );
	DeleteObject( hrgnOr );

	SetWindowRgn( hWnd, hrgn, FALSE );

	
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
static LRESULT CALLBACK RgnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
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
			return DoCreateRgnWindow(hWnd,wParam,lParam);
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

