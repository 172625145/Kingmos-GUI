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
#define MAX_LOADSTRING  100
#define MAX_SEARCHSTRINGLEN	64

#define ID_EDIT		100

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "test";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "testap";								// The title bar text

ATOM RegistertestClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK testWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
int WINAPI WinMain_Test(HINSTANCE hInstance,
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


	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找当前程序是否已经运行
	if( hWnd != 0 )
	{ // 已经运行
		SetForegroundWindow( hWnd ) ; // 设置窗口到最前面
		return FALSE ;
	}
	RegisterAngelusButtonClass(hInstance);
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
ATOM RegistertestClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)testWndProc; // 设备管理窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
//	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_DEVICEMANAGE),IMAGE_ICON,16,16,0);
	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// 装载应用程序图标
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(GRAY_BRUSH); // 设置背景刷
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


   hInst = hInstance; // 保留当前的实例句柄

   // 创建窗口
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX,szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // 创建窗口失败
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);
	// 创建窗口成功
   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 显示窗口

   return TRUE;
}

// ********************************************************************
//声明：static LRESULT CALLBACK testWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK testWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HPEN hPen;

	switch (message) 
	{
		case WM_PAINT: // 绘制窗口
			hdc = BeginPaint(hWnd, &ps);
//			DoPaint(hWnd,hdc);
			//hPen = CreatePen( PS_SOLID, 4, RGB( 0xff, 0, 0 ) );
			//SelectObject( hdc, hPen );
			//RoundRect( hdc, 10, 10, 500,300, 18, 18 );
			EndPaint(hWnd, &ps);
			//DeleteObject( hPen );
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
		case WM_CLOSE:
			return DoCloseWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏消息
			PostQuitMessage(0); // 退出程序
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
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
	CreateWindow( "Angelus_Button","微逻辑1",
				WS_VISIBLE|WS_CHILD,
				10,
				10,
				180,
				80,
				hWnd,
				(HMENU)ID_EDIT,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	CreateWindow( "Angelus_Button","微逻辑2",
				WS_VISIBLE|WS_CHILD,
				10,
				100,
				180,
				80,
				hWnd,
				(HMENU)ID_EDIT,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	/*
	RECT rt;
	SCROLLINFO si;
	GetClientRect(hWnd, &rt); // 得到窗口客户矩形
	// 创建LISTVIEW
	CreateWindow( classEDIT,"深圳市微逻辑电子有限公司 \r\n      Hello Word!!!",
				WS_VISIBLE|WS_CHILD|ES_MULTILINE,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_EDIT,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);
	si.cbSize = sizeof( si );
	si.fMask = SIF_ALL;
	si.nMax = 100;
	si.nMin = 0;
	si.nPage = 10;
	si.nPos = 30;
	si.nTrackPos = 0;
	//SetScrollInfo(hWnd, SB_VERT, &si, TRUE );
	*/
	return 0;
}



// ********************************************************************
//声明：static LRESULT DoCloseWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	无
//功能描述：关闭应用程序消息
//引用: 
// ********************************************************************
#define AGLS_WARNING           0x8000
#define AGLS_INFO           0x4000
#define AGL_WARNING  0x80000000
#define AGL_INFO  0x40000000

static LRESULT DoCloseWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iRet;
	const char * szButtonText[] = { "button1", "button2", "button3" };
	extern int WINAPI AGL_MessageBox( HWND hParent, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType, const char * szButtonText[], int cButton  );
		
		iRet = AGL_MessageBox(hWnd,"真的要关闭应用程序??","警告",MB_YESNO|AGL_INFO, szButtonText, 3 );

		if (iRet == IDYES)
		{
			DestroyWindow(hWnd);
		}

		return 0;
}
