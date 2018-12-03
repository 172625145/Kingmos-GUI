/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：系统桌面，用户界面部分
版本号：1.0.0
开发时期：2003-03-18

作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <ewindows.h>
#include <ekeybrd.h>
#include "eabout.h"
//#include "sipapi.h"
#include "resource.h"
#include "emmsys.h"
#include <keybdsrv.h>
#include <softkey.h>

/***************  全局区 定义， 声明 *****************/

char   classDESKTOP[20] ;
#define DESKTOPCLASSNAME  "_DESKTOP_CLASS_"

char classKEYBOARDWINDOW[] = "_KEYBOARD_WINDOW_CLASS_";

HINSTANCE hInst;

static HBITMAP g_hDesktopBitmap = NULL;
typedef struct DesktopFunStruct{
	RECT rectFunc;
	LPCTSTR lpFuncName;
	UINT idFunc;
}DESKTOPFUNC , *LPDESKTOPFUNC;

#define COLOR_TASKBAR  RGB(205,205,205)
#define COLOR_DESKTOPBK  RGB(240,240,240)

#define ID_LOGO     0x2038
#define ID_FUNC1    0x2039
#define ID_FUNC2    0x203A
#define ID_FUNC3    0x203B
#define ID_FUNC4    0x203C
#define ID_FUNC5    0x203D
#define ID_FUNC6    0x203E
#define ID_ICON1	0x203F
#define ID_ICON2	0x2040
#define ID_ICON3	0x2041  // Battery property
#define ID_ICON4	0x2042
#define ID_TIME     0x2043
#define ID_SHOW     0x2044
#define ID_KEYBD    0x2045
#define ID_MYCOMPUTER  0x2046

#define ID_START	0x100

#ifdef INLINE_PROGRAM
static DESKTOPFUNC DeskTopFuncList[] ={
	{{  8,  2, 24, 18},"Func Logo",ID_LOGO},
	{{  0, 20, 30, 50},"contactbook",ID_FUNC1},
	{{  0, 58, 30, 88},"PlanDay",ID_FUNC2},
	{{  0, 96, 30,126},"Notebook",ID_FUNC3},
	{{  0,134, 30,164},"Mobilephone",ID_FUNC4},
//	{{  0,172, 30,202},"FlyBat",ID_FUNC5},
	{{  0,172, 30,202},"HWChess",ID_FUNC5},
	{{  0,210, 30,240},"IExplore",ID_FUNC6},
	{{  7,248, 23,264},"EasyBox",ID_ICON1},
	{{  7,265, 23,281},"Alarm",ID_ICON2},
	{{  7,282, 23,298},"Setting",ID_ICON3},
	{{  7,299, 23,315},"TEST",ID_ICON4},
	{{203,302,219,318},"Func12",ID_SHOW},
	{{ 30,303,160,319},"Setting",ID_TIME},
	{{221,302,237,318},"keyboard",ID_KEYBD},
	{{ 49, 20, 81, 52},"Manage",ID_MYCOMPUTER},
};
#else
static DESKTOPFUNC DeskTopFuncList[] ={
	{{  8,  2, 24, 18},"\\kingmos\\Func Logo.exe",ID_LOGO},
	{{  0, 20, 30, 50},"\\kingmos\\contactbook.exe",ID_FUNC1},
	{{  0, 58, 30, 88},"\\kingmos\\PlanDay.exe",ID_FUNC2},
	{{  0, 96, 30,126},"\\kingmos\\Notebook.exe",ID_FUNC3},
	{{  0,134, 30,164},"\\kingmos\\Mobilephone.exe",ID_FUNC4},
	{{  0,172, 30,202},"\\kingmos\\FlyBat.exe",ID_FUNC5},
	{{  0,210, 30,240},"\\kingmos\\IExplore.exe",ID_FUNC6},
	{{  7,248, 23,264},"\\kingmos\\EasyBox.exe",ID_ICON1},
	{{  7,265, 23,281},"\\kingmos\\Alarm.exe",ID_ICON2},
	{{  7,282, 23,298},"\\kingmos\\Setting.exe",ID_ICON3},
	{{  7,299, 23,315},"\\kingmos\\rasdial.exe",ID_ICON4},
	{{203,302,219,318},"\\kingmos\\Func12.exe",ID_SHOW},
	{{ 30,303,160,319},"\\kingmos\\Setting.exe",ID_TIME},
	{{221,302,237,318},"\\kingmos\\keyboard.exe",ID_KEYBD},
	{{ 49, 20, 81, 52},"\\kingmos\\test.exe",ID_MYCOMPUTER},
};
#endif

#define BANK_POSITION -1

static UINT g_indexButtonDown = BANK_POSITION;

#define ID_TIMESHOW 1

static HWND g_hKey= NULL;

/******************************************************/
// 函数声明区
/******************************************************/
static ATOM RegisterDesktopClass( HINSTANCE hInstance );
static LRESULT CALLBACK DesktopWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnDesktopPaint( HWND hWnd );
static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc );
static void GetWallPaperPic(LPTSTR lpDeskTopName);
static void DoDesktopLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoDesktopLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static UINT GetButtonID(POINTS ptCursor);
static void DoDesktopCreate(HWND hWnd);
static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ShowTime(HWND hWnd,HDC hdc);
static void DoDesktopClose(HWND hWnd);
static void GetIDRect(UINT id,LPRECT lpRect);
static void DoShowKeyboard(void);
static LRESULT DoWindowPosChanging(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoShowApplication(HWND hWnd);

static void DrawWallPaper(HWND hWnd,HDC hdc);
static void DrawDesktopControl(HWND hWnd,HDC hdc);
static void DrawTaskBar(HWND hWnd,HDC hdc);
static HWND GetDesktop(void);

static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop);

static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam );

static HWND CreateSystemKeyboard(HWND hParent);

static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void OnAboutPaint( HWND hWnd );


static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance );
static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoKeyboardWndCreate(HWND hWnd);


// **************************************************
// 声明：LRESULT CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR     lpCmdLine,int       nCmdShow)
// 参数：
// 	IN hInstance -- 应用程序实例句柄
// 	IN hPrevInstance -- 前一个应用程序实例句柄
// 	IN lpCmdLine -- 应用程序命令行参数
// 	IN nCmdShow -- 显示标志
// 
// 返回值：返回TRUE
// 功能描述：应用程序入口
// 引用: 
// **************************************************
#ifdef INLINE_PROGRAM
int WINAPI WinMain_Desktop(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#else
LRESULT CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
    MSG				msg;

		RETAILMSG( 1, ( "Start Desktop ...\r\n" ) );

		RETAILMSG( 1, ( "TouchCalibrate(): %x \r\n", TouchCalibrate) );
//		TouchCalibrate();  // 运行画面校正

		RegisterDesktopClass(hInstance); // 注册桌面程序类
		RegisterKeyboardWndClass(hInstance);
		hInst = hInstance;
		// 创建桌面窗口
		CreateWindow( classDESKTOP, "桌面", WS_VISIBLE|WS_CLIPCHILDREN,
			0, 0,
			GetSystemMetrics( SM_CXSCREEN ),
			GetSystemMetrics( SM_CYSCREEN ),
			0,0,hInstance,0 );		


		//sndPlaySound("\\kingmos\\kingmos.wav",SND_ASYNC); // 播放开机音乐

		while (GetMessage(&msg, NULL, 0, 0)) 
		{	// 消息过程
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 如果桌面已经退出，则退出系统
		ExitSystem(EXS_SHUTDOWN,0); // 退出系统
		return 0;
}
// ********************************************************************
// 声明：static ATOM RegisterDesktopClass( HINSTANCE hInstance );
// 参数：
//	IN hInstance - 桌面系统的实例句柄
// 返回值：
// 	无
// 功能描述：注册系统桌面类
// 引用: 
// ********************************************************************
static ATOM RegisterDesktopClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	strcpy( classDESKTOP,  DESKTOPCLASSNAME) ; // 复制类名

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)DesktopWndProc;  // 桌面过程函数
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classDESKTOP;
	return RegisterClass(&wcex); // 向系统注册桌面类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK DesktopWndProc(HWND , UINT , WPARAM , LPARAM )
// 参数：
//	IN hWnd- 桌面系统的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	无
// 功能描述：系统桌面窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK DesktopWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // 绘画消息
			OnDesktopPaint( hWnd );
			return 0;
		case WM_ERASEBKGND:  // 绘制背景
			OnDesktopEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_LBUTTONDOWN: // 鼠标左按钮按下，或TOUCH笔按下
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_LBUTTONUP:// 鼠标左按钮弹起，或TOUCH笔离开
			DoDesktopLButtonUp(hWnd,wParam,lParam);
			return 0;
		case WM_TIMER: // 定时器
			DoDesktopTimer(hWnd,wParam,lParam);
			return 0;
		case WM_WINDOWPOSCHANGING: // 窗口位置改变
			return DoWindowPosChanging(hWnd,wParam,lParam);
//            return 0;
		case WM_CREATE: // 创建消息
			DoDesktopCreate(hWnd);
			return 0;
		case WM_CLOSE:  // 关闭消息
			DoDesktopClose(hWnd);
			return 0;
		case WM_DESTROY: // 破坏消息
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // 窗口默认消息
   }
   return FALSE;
}

// ********************************************************************
// 声明：static void OnDesktopPaint( HWND hWnd )
// 参数：
//	IN hWnd- 桌面系统的窗口句柄
// 返回值：
//	无
// 功能描述：处理系统桌面窗口绘画过程
// 引用: 
// ********************************************************************
static void OnDesktopPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;

		hdc = BeginPaint( hWnd, &ps );  // 开始绘制
		DrawDesktopControl(hWnd,hdc); // 绘制桌面控制
//		ShowTime(hWnd,hdc);
		EndPaint( hWnd, &ps ); // 结束绘制
}

// ********************************************************************
// 声明：static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
// 参数：
//	IN hWnd -- 桌面系统的窗口句柄
//	IN hdc -- 桌面系统的绘画句柄
// 返回值：
//	无
// 功能描述：处理系统桌面窗口背景刷新过程
// 引用: 
// ********************************************************************
static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
{
	RECT				rt;
//	HBITMAP				hBitMap ;
	HBRUSH hBrush;

//	char lpDeskTopName[MAX_PATH];

	GetClientRect( hWnd, &rt ); // 得到客户矩形
	rt.left += 30; // 左面是应用程序条
	rt.bottom -= 20; // 下面是系统状态条
	hBrush = CreateSolidBrush(COLOR_DESKTOPBK); // 创建背景刷
	FillRect( hdc, &rt, hBrush ) ; // 填充北京
	DeleteObject(hBrush); // 删除背景刷
}

// ********************************************************************
// 声明：static void GetWallPaperPic(LPTSTR lpDeskTopName)
// 参数：
//	IN/OUT lpDeskTopName -- 存放桌面背景图象的位图文件名的缓存
// 返回值：
//	无
// 功能描述：得到桌面背景图象的位图文件名
// 引用: 
// ********************************************************************
static void GetWallPaperPic(LPTSTR lpDeskTopName)
{
	strcpy(lpDeskTopName,"./kingmos/mlgsign.bmp"); // 复制指定的桌面背景位图
}

// ********************************************************************
// 声明：static void DoDesktopLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
//    IN wParam  -- 消息参数
//    IN lParam  -- 消息参数
// 返回值：
//	无
// 功能描述：处理桌面鼠标左键按下消息
// 引用: 
// ********************************************************************
static void DoDesktopLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS ptCursor;
	UINT indexButton;

		ptCursor = MAKEPOINTS(lParam);  // 得到鼠标按下的位置
		indexButton = GetButtonID(ptCursor); // 得到按下位置的按钮ID
		g_indexButtonDown = indexButton; // 设置按下按钮
}

// ********************************************************************
// 声明：static void DoDesktopLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
//    IN wParam  -- 消息参数
//    IN lParam  -- 消息参数
// 返回值：
//	无
// 功能描述：处理桌面鼠标左键弹起消息
// 引用: 
// ********************************************************************
static void DoDesktopLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS ptCursor;
	UINT indexButton;

		if (g_indexButtonDown == BANK_POSITION)
			return ; // 按下位置没有按钮
		ptCursor = MAKEPOINTS(lParam); // 得到鼠标弹起位置
		indexButton = GetButtonID(ptCursor); // 得到弹起位置的按钮ID

		if (g_indexButtonDown == indexButton)
		{ // 与按下时的位置一致
			//运行指定的应用程序
			switch(DeskTopFuncList[g_indexButtonDown].idFunc)
			{
				case ID_KEYBD: // 键盘按钮
					DoShowKeyboard();  // 显示键盘
					break;
				case ID_SHOW: // 显示按钮
					DoShowApplication(hWnd); // 显示应用程序
					break;
				case ID_LOGO:  // 标志按钮
					DialogBoxIndirectEx(hInst,(LPDLG_TEMPLATE_EX)&dlgIDD_ABOUT,NULL, (DLGPROC)About); // 弹出关于按钮
					break;
				case ID_MYCOMPUTER:  
				case ID_ICON3:  
				case ID_TIME:  
#ifdef INLINE_PROGRAM
					LoadApplication(DeskTopFuncList[g_indexButtonDown].lpFuncName,NULL);
#else
					CreateProcess(DeskTopFuncList[g_indexButtonDown].lpFuncName,NULL,0,0,0,0,0,0,0,0);  // 运行指定的应用程序
#endif
					break;
				default:
					MessageBox(hWnd,DeskTopFuncList[g_indexButtonDown].lpFuncName,"Error",MB_OK);
					break;
			}
		}
}

// ********************************************************************
// 声明：static UINT GetButtonID(POINTS ptCursor)
// 参数：
//	IN	ptCursor - 要得到的指定位置的ID号的坐标位置
// 返回值：
//	返回指定位置的按钮ID
// 功能描述：得到指定位置的按钮ID
// 引用: 
// ********************************************************************
static UINT GetButtonID(POINTS ptCursor)
{
	UINT idButton = BANK_POSITION;
	int iButtonCount = sizeof(DeskTopFuncList)/sizeof(DESKTOPFUNC);  // 得到按钮总数
	POINT pt;
	int i;

		pt.x = ptCursor.x;
		pt.y = ptCursor.y;

		for (i=0;i<iButtonCount;i++)
		{
			if (PtInRect(&DeskTopFuncList[i].rectFunc,pt))  // 判断当前指定位置是否在指定按钮上
			{
				idButton = i;
				return idButton; // 返回当前的按钮ID
			}
		}
		return idButton; // 返回空白位置
}

// ********************************************************************
// 声明：static void DoDesktopCreate(HWND hWnd)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
// 返回值：
//	无
// 功能描述：处理桌面创建消息
// 引用: 
// ********************************************************************
static void DoDesktopCreate(HWND hWnd)
{
	g_hKey = CreateSystemKeyboard(hWnd);  // 创建系统键盘
	SetTimer(hWnd, ID_TIMESHOW,1000,NULL); // 设置显示时间定时器
}

// ********************************************************************
// 声明：static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
//    IN wParam  -- 消息参数
//    IN lParam  -- 消息参数
// 返回值：
//	无
// 功能描述：处理桌面Timer消息
// 引用: 
// ********************************************************************
static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
		hdc = GetDC(hWnd); // 得到HDC
		ShowTime(hWnd ,hdc); // 显示当前时间
		ReleaseDC(hWnd,hdc); // 释放HDC
}

// ********************************************************************
// 声明：static void ShowTime(HWND hWnd,LPTSTR lpTime)
// 参数：
//	IN  hWnd -- 桌面系统的窗口句柄
//	IN  hdc -- 桌面系统的绘画句柄
// 返回值：
//	无
// 功能描述：在桌面上显示当前时间
// 引用: 
// ********************************************************************
static void ShowTime(HWND hWnd,HDC hdc)
{
	RECT				rt;
	SYSTEMTIME SystemTime;
	TCHAR lpLocalTime[20];
	TCHAR *Week[] = {"日","一","二","三","四","五","六",""};
	HBRUSH hBrush;
	

		GetIDRect(ID_TIME,&rt);  // 得到显示时间的矩形大小
		hBrush = CreateSolidBrush(COLOR_TASKBAR); // 得到任务条的背景颜色
		FillRect(hdc,&rt,hBrush); // 填充时间区
		DeleteObject(hBrush); // 删除画刷
		//得到当前的时间
		GetLocalTime(&SystemTime); // 得到当前时间
		// 将前时间转化为字串
		sprintf(lpLocalTime,"%02d-%02d %02d:%02d(%s)",SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,Week[SystemTime.wDayOfWeek]); 

		SetBkMode( hdc, TRANSPARENT ) ; // 设置透明模式
		DrawText(hdc,lpLocalTime,strlen(lpLocalTime),&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE); // 显示当前时间

}

// ********************************************************************
// 声明：static void DoDesktopClose(HWND hWnd)
// 参数：
//	IN	hWnd - 桌面系统的窗口句柄
// 返回值：
//	无
// 功能描述：处理桌面关闭消息
// 引用: 
// ********************************************************************
static void DoDesktopClose(HWND hWnd)
{
	BOOL bExistOtherWindow;
	// 要求等待所有的窗口退出
	while(1)
	{
		bExistOtherWindow = FALSE;
		EnumWindows(EnumExistWindowsProc,(LPARAM)&bExistOtherWindow); // 枚举当前的窗口

		if (bExistOtherWindow == FALSE) 
			break; // 没有窗口存在

		Sleep(1000);
	}


	if (g_hDesktopBitmap)
	{ 
		DeleteObject(g_hDesktopBitmap);  // 删除桌面位图
		g_hDesktopBitmap = NULL;
	}
	if (g_hKey)
	{
		DestroyWindow(g_hKey);  // 破坏键盘
	}
	KillTimer(hWnd, ID_TIMESHOW); // 杀掉定时器
	DestroyWindow(hWnd); // 破坏桌面窗口
}

// ********************************************************************
// 声明：static void GetIDRect(UINT id,LPRECT lpRect)
// 参数：
//	IN	id - 要求得到范围的指定ＩＤ号
//	IN/OUT 	lpRect - 返回指定ＩＤ号的矩形大小
// 返回值：
//	无
// 功能描述：得到指定ID的矩形大小
// 引用: 
// ********************************************************************
static void GetIDRect(UINT id,LPRECT lpRect)
{
	UINT idButton = BANK_POSITION;
	int iButtonCount = sizeof(DeskTopFuncList)/sizeof(DESKTOPFUNC);  // 得到按钮的总数
	int i;

		for (i=0;i<iButtonCount;i++)
		{
			if (DeskTopFuncList[i].idFunc == id)
			{  // 查找到当前按钮就是要查找的按钮
				*lpRect = DeskTopFuncList[i].rectFunc; // 返回该按钮的矩形
				return ;
			}
		}
		return ;
}

// ********************************************************************
// 声明：static void DoShowKeyboard(void)
// 参数：
//    无
// 返回值：
//	无
// 功能描述：显示或隐藏键盘
// 引用: 
// ********************************************************************
static void DoShowKeyboard(void)
{
//	BOOL bShow = FALSE;
	DWORD dwStyle;//,dwExStyle;

	dwStyle = GetWindowLong(g_hKey,GWL_STYLE);  // 得到键盘的风格

	if (dwStyle & WS_VISIBLE)
	{  // 当前键盘已经显示
		ShowWindow(g_hKey,SW_HIDE);  // 隐藏键盘
	}
	else
	{
		ShowWindow(g_hKey,SW_SHOW);
	}
}

// ********************************************************************
// 声明：static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN	hWnd - 桌面系统的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
// 	无
// 功能描述：处理桌面WM_WINDOWPOSCHANGING消息
// 引用: 
// ********************************************************************
static LRESULT DoWindowPosChanging(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam;

	if( !(lpwp->flags & SWP_NOZORDER) )
	{  // 序列号改变
		lpwp->hwndInsertAfter = GetWindow(hWnd, GW_HWNDLAST); // 将桌面窗口放到最后面
	}
	lpwp->flags |= SWP_NOACTIVATE;
	return DefWindowProc( hWnd, WM_WINDOWPOSCHANGING, wParam, lParam );  // 窗口默认消息
}

// ********************************************************************
// 声明：static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hDlg- 桌面系统About窗口的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	无
// 功能描述：系统桌面窗口About窗口的过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // 绘制消息
			OnAboutPaint(hDlg);
			return TRUE;
		case WM_CLOSE:  // 关闭消息
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}
// ********************************************************************
//声明：static void OnAboutPaint( HWND hWnd )
//参数：
//	IN hWnd- 桌面系统About窗口的窗口句柄
//返回值：
//	无
//功能描述：系统桌面窗口About窗口处理绘画消息
//引用: 
// ********************************************************************
static void OnAboutPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	HICON hIcon;

		hdc = BeginPaint( hWnd, &ps );  // 开始绘制
		hIcon = LoadImage( NULL, MAKEINTRESOURCE(OIC_SYSLOGO), IMAGE_ICON, 32, 32, 0 ) ; // 得到系统标志图标
		if( hIcon == 0 )
		{  // 打开图标失败
			EndPaint( hWnd, &ps );
			return ;
		}
		DrawIcon(hdc,88,8,hIcon);// 绘制图标		//DeleteObject(hBitMap);
		DestroyIcon(hIcon); // 破坏图标
		EndPaint( hWnd, &ps ); // 结束绘制
}

#define IDC_SHOWDESKTOP 1000
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam );

typedef struct EnumWindowStruct{
	HMENU hMenu;
	LPPTRLIST hWndList;
	int iIndex;
}ENUMWINDOW;

// **************************************************
// 声明：static void DoShowApplication(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值： 无
// 功能描述：显示应用程序。
// 引用: 
// **************************************************
static void DoShowApplication(HWND hWnd)
{
    MENUITEMINFO		menu_info;
	HMENU		hMenu = CreatePopupMenu();  // 创建一个菜单
	int iIndex = 0, id_Menu;
//	TCHAR lpAppName[100];
	HWND hBrother;
//	RECT rect;
//	int x = 140,y= 200;
	PTRLIST hWndList;
	ENUMWINDOW hEnumWindow;

	PtrListCreate(&hWndList,(ccIndex)10,(ccIndex)6); // 创建一个指针列表
	// 初始化菜单结构
	menu_info.cbSize  = sizeof( MENUITEMINFO );  
	menu_info.fMask   = MIIM_TYPE | MIIM_ID|MIIM_DATA;
	menu_info.fType   = MFT_STRING  ;
	
	menu_info.wID =  iIndex + ID_START;
	menu_info.dwTypeData = "显示桌面" ;
	menu_info.cch = 8 ;
	if( !InsertMenuItem( hMenu, 0, TRUE, &menu_info )  )  // 插入一个“显示桌面”菜单条目
	{
		EdbgOutputDebugString( "===InitManiMenu   failed===\r\n" );
		goto MENUEND;
	}
	iIndex ++;
	PtrListInsert(&hWndList, NULL); // 插入一个空指针到窗口列表
	hEnumWindow.hMenu = hMenu;
	hEnumWindow.hWndList = &hWndList;
	hEnumWindow.iIndex = iIndex;
	EnumWindows(EnumWindowsProc,(LPARAM)&hEnumWindow);  // 枚举窗口

	id_Menu = TrackPopupMenu( hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN |TPM_NONOTIFY | TPM_RETURNCMD, 240, 300, 0, hWnd, NULL ); // 弹出菜单
	if (id_Menu != 0)
	{  // 有选择条目 
		iIndex = id_Menu - ID_START ; // 得到条目索引
		if (iIndex == 0)
		{ //选择的条目是“显示桌面”
			ShowDesktop(0);
		}
		else
		{  // 是应用程序
			hBrother = PtrListAt(&hWndList, iIndex );// 得到窗口句柄
			ShowWindow(hBrother,SW_RESTORE); // 恢复选择的窗口显示
		}
	}

MENUEND:
	PtrListDestroy(&hWndList); // 破坏列表
	DestroyMenu(hMenu); // 破坏菜单
	return ;

}

// **************************************************
// 声明：BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 参数
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：枚举所有已经打开的窗口
// 引用: 
// **************************************************
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam )
{
	ENUMWINDOW *lpEnumWindow;
    MENUITEMINFO		menu_info;
	TCHAR lpAppName[100];
	HWND hParent;
	HWND hChild;
		
		if (hWnd == NULL)
			return FALSE;  // 无效窗口句柄
		hParent = GetParent(hWnd); // 得到父窗口
		if (hParent != NULL)
			return TRUE; // 没有父窗口
		hChild = GetDesktop();//GetDesktopWindow(); // 得到桌面窗口句柄
		if (hChild == hWnd)  //当前窗口就是桌面窗口，不处理，直接返回
			return TRUE;

		lpEnumWindow = (ENUMWINDOW *)lParam;  //得到枚举窗口结构

		if (lpEnumWindow->iIndex == 1 )
		{  // 如果是第一个应用程序，则要插入一个SEPARATOR
			menu_info.fMask   = MIIM_TYPE ;
			menu_info.fType   = MFT_SEPARATOR  ;
			if( !InsertMenuItem( lpEnumWindow->hMenu, 0, TRUE, &menu_info )  ) {
				EdbgOutputDebugString( "===InitManiMenu   failed===\r\n" );
				return FALSE;
			}
		}

		PtrListInsert(lpEnumWindow->hWndList, hWnd); // 插入找到的窗口句柄到列表
		GetWindowText(hWnd,lpAppName,100); // 得到应用程序名

		menu_info.fMask   = MIIM_TYPE | MIIM_ID|MIIM_DATA;
		menu_info.fType   = MFT_STRING  ;
		menu_info.wID =  lpEnumWindow->iIndex + ID_START;
		menu_info.dwTypeData = lpAppName ;
		menu_info.cch = strlen( lpAppName ) ;
		if( !InsertMenuItem( lpEnumWindow->hMenu, 0, TRUE, &menu_info )  )  // 插入当前应用程序到菜单
		{ 
			return FALSE;
		}
		lpEnumWindow->iIndex ++;
		return TRUE;
}



// **************************************************
// 声明：static void DrawWallPaper(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值： 无
// 功能描述：绘制墙纸。
// 引用: 
// **************************************************
static void DrawWallPaper(HWND hWnd,HDC hdc)
{
	RECT rect;
	BITMAP bitmap;
	char lpDeskTopName[MAX_PATH];

	GetWallPaperPic(lpDeskTopName); // 得到墙纸畏途

	if (g_hDesktopBitmap == NULL)
	{  // 没有装载墙纸
//		RETAILMSG(1,("***** Start Load Bitmap File \r\n"));
		g_hDesktopBitmap = LoadImage( 0, lpDeskTopName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载墙纸
//		RETAILMSG(1,("***** Load Bitmap Success \r\n"));
		if( g_hDesktopBitmap == NULL )
		{  // 装载墙纸失败
			return ;
		}
	}

	GetObject(g_hDesktopBitmap,sizeof(bitmap),&bitmap);  // 得到位图结构
	// 得到位图大小
	rect.left = AP_XSTART + (AP_WIDTH - bitmap.bmWidth) /2 ;
	rect.right = rect.left + bitmap.bmWidth ;
	rect.top = AP_YSTART + (AP_HEIGHT - bitmap.bmHeight) /2 ;
	rect.bottom = rect.top + bitmap.bmHeight ;
	
	DrawBitMap( hdc, g_hDesktopBitmap, rect, 0, 0 ,SRCCOPY);  // 绘制位图
}

// **************************************************
// 声明：static void DrawDesktopControl(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述：绘制桌面控件。
// 引用: 
// **************************************************
static void DrawDesktopControl(HWND hWnd,HDC hdc)
{
	HINSTANCE hInstance;
	HBITMAP hBitmap;
	HICON hIcon;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // 得到实例句柄

//		RETAILMSG(1,("***** Start Draw App Bar \r\n"));
		// 装载应用程序条位图
		hBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_APPBAR), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;
		if( hBitmap )
		{ // 装载位图成功
			RECT rectApBar = {0,0,30,320}; // 设置应用程序条的大小
			DrawBitMap( hdc, hBitmap, rectApBar, 0, 0 ,SRCCOPY); // 绘制位图
			DeleteObject(hBitmap); // 删除位图句柄
		}

//		RETAILMSG(1,("***** Start Draw Task Bar \r\n"));
		DrawTaskBar(hWnd,hdc); // 绘制任务条
//		RETAILMSG(1,("***** Start Draw WallPaper \r\n"));
		DrawWallPaper(hWnd,hdc); // 绘制墙纸

		// 装载“我的电脑”图标
		hIcon = LoadImage( hInstance, MAKEINTRESOURCE(IDI_COMPUTER), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE ) ;
		RETAILMSG(1,("***** Load Icon (%x)File OK \r\n",hIcon));
		if( hIcon )
		{ // 装载成功
			RECT rect;
			GetIDRect(ID_MYCOMPUTER,&rect);  // 得到位置
			DrawIcon(hdc,rect.left,rect.top,hIcon); // 绘制图标
			DestroyIcon(hIcon);  // 破坏图标
		}
}

// **************************************************
// 声明：static void DrawTaskBar(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述：绘制任务条。
// 引用: 
// **************************************************
static void DrawTaskBar(HWND hWnd,HDC hdc)
{
	HINSTANCE hInstance;
//	HBITMAP hBitmap;
	HICON hIcon;
	HBRUSH hBrush;
	RECT rectTaskBar = {30,300,240,320};  // 任务条的位置

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

		hBrush = CreateSolidBrush(COLOR_TASKBAR); // 得到任务条的背景颜色
		FillRect(hdc,&rectTaskBar,hBrush); // 填充背景
		DeleteObject(hBrush); // 删除画刷
		// 装载菜单按钮
		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE(IDI_MENU), IMAGE_ICON, 16, 16, 0 ) ;
		{
			RECT rect;
			GetIDRect(ID_SHOW,&rect);  // 得到菜单的位置
			DrawIcon(hdc,rect.left,rect.top,hIcon); // 绘制图标
			DestroyIcon(hIcon); // 破坏图标
		}

		// 装载键盘按钮
		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE(IDI_KEYBOARD), IMAGE_ICON, 16, 16, 0 ) ;
		{
			RECT rect;
			GetIDRect(ID_KEYBD,&rect); // 得到键盘的位置
			DrawIcon(hdc,rect.left,rect.top,hIcon);// 绘制图标
			DestroyIcon(hIcon); // 破坏图标
		}

		ShowTime(hWnd,hdc);  // 显示时间
}


// **************************************************
// 声明：static HWND GetDesktop(void)
// 参数：
// 	无
// 返回值： 返回桌面的窗口句柄
// 功能描述：得到桌面的窗口句柄。
// 引用: 
// **************************************************
static HWND GetDesktop(void)
{
	return FindWindow(classDESKTOP,NULL);
}



// **************************************************
// 声明：static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 过程参数
// 
// 返回值：有窗口存在返回TRUE，否则返回FALSE
// 功能描述：枚举已经存在的窗口。
// 引用: 
// **************************************************
static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam )
{
	BOOL * lpbExistOtherWindow;
//    MENUITEMINFO		menu_info;
//	TCHAR lpAppName[100];
	HWND hParent;
	HWND hChild;
		
		if (hWnd == NULL)
			return FALSE;  // 已经没有窗口
		hParent = GetParent(hWnd);
		if (hParent != NULL)
			return TRUE;  // 还有窗口
		hChild = GetDesktop();//GetDesktopWindow();
		if (hChild == hWnd)
			return TRUE; // 当前窗口是桌面窗口，不计，还有窗口

		lpbExistOtherWindow = (BOOL *)lParam;
		
		*lpbExistOtherWindow = TRUE; // 设置还有窗口存在
		return FALSE; // 不要继续枚举
}

// **************************************************
// 声明：static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN rect -- 位图大小
// 	IN xOffset -- X偏移
// 	IN yOffset -- Y偏移
// 	IN dwRop -- 绘制模式
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
		HDC hMemoryDC;
    HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // 创建一个兼容的内存句柄
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将要绘制的位图设置到内存句柄中
		BitBlt( hdc, // handle to destination device context
			(short)rect.left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)rect.top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(rect.right-rect.left),  // width of destination rectangle
			(short)(rect.bottom-rect.top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			dwRop
			);

		SelectObject(hMemoryDC,hOldBitmap); // 恢复内存句柄
		DeleteDC(hMemoryDC); // 删除句柄
}

// **************************************************
// 声明：static HWND CreateSystemKeyboard(HWND hParent)
// 参数：
// 	IN hParent -- 窗口句柄
// 
// 返回值：返回键盘的窗口句柄。
// 功能描述：创建系统键盘。
// 引用: 
// **************************************************
static HWND CreateSystemKeyboard(HWND hParent)
{
	HWND hKey;
	// 创建系统键盘。

#define KEYBOARDSTARTX  30
#define KEYBOARDSTARTY  (320-20-KEYBOARDHEIGHT)

    hKey = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,  
						classKEYBOARDWINDOW,
						"键盘",
						WS_POPUP,//|WS_VISIBLE,//|WS_CAPTION,
						KEYBOARDSTARTX,
						KEYBOARDSTARTY,
						KEYBOARDWIDTH,
						KEYBOARDHEIGHT,
						hParent,
						(HMENU)101,
						(HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE),
						NULL);
	return hKey;
}


// ********************************************************************
// 声明：static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance )
// 参数：
//	IN hInstance - 桌面系统的实例句柄
// 返回值：
// 	无
// 功能描述：注册系统桌面类
// 引用: 
// ********************************************************************
static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)KeyboardWndProc;  // 桌面过程函数
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classKEYBOARDWINDOW;
	return RegisterClass(&wcex); // 向系统注册桌面类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd- 桌面系统的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	无
// 功能描述：系统桌面窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:  // 绘画消息
			hdc = BeginPaint( hWnd, &ps );  // 开始绘制
			EndPaint( hWnd, &ps ); // 结束绘制
			return 0;
		case WM_CREATE: // 创建消息
			DoKeyboardWndCreate(hWnd);
			return 0;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // 窗口默认消息
   }
   return FALSE;
}

// ********************************************************************
// 声明：static void DoKeyboardWndCreate(HWND hWnd)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
// 返回值：
//	无
// 功能描述：处理桌面创建消息
// 引用: 
// ********************************************************************
static void DoKeyboardWndCreate(HWND hWnd)
{
	HWND hKey;
	hKey = CreateKeyboard((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),hWnd,0);
	if (hKey)
		ShowKeyboard(hKey,SIP_SHOW); // 显示键盘
}
