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
#include "resource.h"
#include "emmsys.h"
#include <keybdsrv.h>
#include <softkey.h>

#include "desktop.h"
#include <eoemkey.h>
#include <jpegapi.h>
/***************  全局区 定义， 声明 *****************/

char   classDESKTOP[20] ;
#define DESKTOPCLASSNAME  "_DESKTOP_CLASS_"
static char classKEYBOARDWINDOW[] = "_KEYBOARD_WINDOW_CLASS_";
static HINSTANCE hInst;

extern BOOL InitStateBar( HINSTANCE hInstance );
static int	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem );
static void SetFocusButton( HWND hWnd, int idx );
static void RunFocusFunction( void );

#define FOCUS_DESKTOP       0
#define FOCUS_LIST          1


static HBITMAP g_hDesktopBitmap = NULL;
static HWND g_hwndDesktop = NULL;
static UINT g_FocusState = FOCUS_DESKTOP;

static HBITMAP	hbmpNULL = 0;
static HBITMAP	hHomeBitMap;
static HBITMAP	hbmpHDD;
static HBITMAP	hbmpSDCard;
static HBITMAP	hbmpCFCard;
static HBITMAP  hbmpTextBarNormal;
static HBITMAP  hbmpTextBarSelect;

static HBITMAP	hbmpBackup[5];
static HBITMAP  hbmpBackupState[3];  //

static HBITMAP	hbmpPlayback[5];
static HBITMAP  hbmpPlaybackState[3];  //

static HBITMAP	hbmpPlaysound[5];
static HBITMAP  hbmpPlaysoundState[3];  //

static HBITMAP	hbmpPlayMovie[5];
static HBITMAP  hbmpPlayMovieState[3];  //

static HBITMAP	hbmpSetup[2];
static HBITMAP  hbmpSetupState[3];  //




enum{
	    ID_LIST_BACKUP      =0,
		ID_LIST_PLAYBACK,
		ID_LIST_PLAYSOUND,
		ID_LIST_PLAYMOVIE,
		ID_LIST_SETUP,
		ID_LIST_MAX,
		ID_EXIT
};

static PTRLIST ptrList[ID_LIST_MAX];


#define ID_STATE_BAR        (ID_LIST_MAX+0)
//#define ID_LISTBOX          101


typedef struct DesktopFunStruct{
	RECT rectFunc;
	RECT rectIcon;
	LPCTSTR lpFuncName;	
	LPCTSTR lpTitle;	
	UINT idFunc;
	UINT idSubItem;
	HBITMAP *lpbmpIcon;
	HBITMAP *lpbmpState;
}DESKTOPFUNC , *LPDESKTOPFUNC;

typedef struct DESKTOP_ITEM_DATA{
	HBITMAP hbmpIcon;
	UINT cTitleLength;
	char szTitle[1];
}DESKTOP_ITEM_DATA;

#define DESKTOP_CONTROL_NUM 8

#define STATE_NORMAL   0
#define STATE_FOCUS    1
#define STATE_SET      2

typedef struct _DESK_INFO{
	UINT uCurrentSelectIndex;
	UINT uCurrentSelectState;
	DESKTOPFUNC deskFun[DESKTOP_CONTROL_NUM];
}DESK_INFO, FAR * LPDESK_INFO;

#define COLOR_TASKBAR  RGB(205,205,205)
#define COLOR_DESKTOPBK  RGB(240,240,240)

//#define ID_LOGO     0x2038
//#define ID_TIME     0x2043
//#define ID_SHOW     0x2044
//#define ID_KEYBD    0x2045
//#define ID_MYCOMPUTER  0x2046
//#define ID_START	0x100


#ifdef INLINE_PROGRAM
#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define FUN_WIDTH 101
#define FUN_HEIGHT 24

#define ICON_COL_WIDTH 32
#define ICON_ROW_HEIGHT 40

#define ICON_OFFSET_X   500
#define ICON_OFFSET_Y   0


static DESK_INFO diDesktopInfo ={
	0, STATE_FOCUS,
	{
		{   
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*0, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*0+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*0, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*0+ICON_HEIGHT },
		      "demo_alpha", "alpha", 0, 0, hbmpBackup, hbmpBackupState
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*1, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*1+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*1, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*1+ICON_HEIGHT },
			"demo_normal", "normal", 0, 0, hbmpPlayback, hbmpPlaybackState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*2, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*2+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*2, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*2+ICON_HEIGHT },
			"demo_rgnwin","rgnwin", 0, 0, hbmpPlaysound, hbmpPlaysoundState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*3, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*3+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*3, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*3+ICON_HEIGHT },
			"demo_font1", "font1", 0, 0, hbmpPlayMovie, hbmpPlayMovieState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*4, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*4+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*4, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*4+ICON_HEIGHT },
			"demo_Gears", "OpenGL-3D", 0, 0, hbmpSetup, hbmpSetupState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*5, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*5+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*5, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*5+ICON_HEIGHT },
			"demo_Layered", "Layered", 0, 0, hbmpSetup, hbmpSetupState 
		},


		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*6, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*6+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*6, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*6+ICON_HEIGHT },
			"demo_gradfill", "渐变填充", 0, 0, hbmpSetup, hbmpSetupState
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*7, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*7+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*7, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*7+ICON_HEIGHT },
			"", "exit", ID_EXIT, 0, hbmpSetup, hbmpSetupState
		}
	},
	
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
	{{ 49, 20, 81, 52},"\\kingmos\\manage.exe",ID_MYCOMPUTER},
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
static int DoDesktopCreate(HWND hWnd);
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

static void DrawBitMap(HDC hdc,HBITMAP hBitmap, LPRECT lprect,UINT xOffset,UINT yOffset,DWORD dwRop );

static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam );

static HWND CreateSystemKeyboard(HWND hParent);

static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void OnAboutPaint( HWND hWnd );


static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance );
static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoKeyboardWndCreate(HWND hWnd);

static void SplashLogo( void )
{
	
//#ifdef LINUX_KERNEL
	HBITMAP hbmp = LoadImage( NULL, "./kingmos/desktop/logo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
//#else
//	HBITMAP hbmp = LoadImage( NULL, ".\\kingmos\\bg_splash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
//#endif
	HDC hdc = GetDC( NULL );
	HDC hMemDC = CreateCompatibleDC( hdc );
	
	SelectObject( hMemDC, hbmp );

	BitBlt( hdc, 0, 0, 640, 480, hMemDC, 0, 0, SRCCOPY );

	DeleteDC( hMemDC );
	ReleaseDC( NULL, hdc );

	DeleteObject( hbmp );
	Sleep( 3000 );
}

static VOID InitDesktopData( HINSTANCE hInst )
{
}

//键盘消息过滤, 主要针对 LEFT, RIGHT , UP, DOWN
static BOOL FilterMessage( LPMSG lpMsg )
{
	if( g_FocusState == FOCUS_LIST )
	{
		if( lpMsg->message == WM_KEYDOWN )
		{
			if( lpMsg->wParam == VK_LEFT )
			{
				if( g_FocusState == FOCUS_LIST )
				{
					SendMessage( GetDlgItem( g_hwndDesktop, diDesktopInfo.uCurrentSelectIndex ), LB_SETCURSEL, -1, 0 );

					g_FocusState = FOCUS_DESKTOP;
					//SetFocus( g_hwndDesktop );					
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}

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

		//SplashLogo();
		//InitStateBar( hInstance );
		InitDesktopData( hInstance );
		 
//		TouchCalibrate();  // 运行画面校正

		RegisterDesktopClass(hInstance); // 注册桌面程序类
		RegisterKeyboardWndClass(hInstance);
		hInst = hInstance;
		// 创建桌面窗口
		g_hwndDesktop = CreateWindowEx( WS_EX_NOFOCUS, classDESKTOP, "桌面", WS_VISIBLE|WS_CLIPCHILDREN,
			0, 0,
			GetSystemMetrics( SM_CXSCREEN ),
			GetSystemMetrics( SM_CYSCREEN ),
			0,0,hInstance,0 );		


		//LoadApplication( "demo_gps", 0 );
		//LoadApplication( "demo_gradfill", 0 );
#if 0
		LoadApplication( "demo_alpha", 0 );
		Sleep( 1000 );
		LoadApplication( "demo_font1", 0 );
		Sleep( 3000 );
//		LoadApplication( "demo_Gears", 0 );
//		Sleep( 50 );
		LoadApplication( "demo_Layered", 0 );
		Sleep(1000);
#endif 	
//		sndPlaySound("\\kingmos\\kingmos.wav",SND_ASYNC); // 播放开机音乐
		{
			//extern void LoadAllDemoAp(HINSTANCE hInstance);
			//LoadAllDemoAp(hInstance);
		}

		while (GetMessage(&msg, NULL, 0, 0)) 
		{	// 消息过程
			//
			if( FilterMessage( &msg ) == 0 )
			{	//没有处理
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// 如果桌面已经退出，则退出系统
		ExitSystem(EXS_SHUTDOWN,0); // 退出系统
		return 0;
}

static int DoKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	BOOL bUpdate = FALSE;
	HWND hwndChild;
	UINT oldSelect = diDesktopInfo.uCurrentSelectIndex;
	switch( wParam )
	{
	case VK_DOWN:
		diDesktopInfo.uCurrentSelectIndex = (diDesktopInfo.uCurrentSelectIndex + 1) % DESKTOP_CONTROL_NUM;
		diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
		bUpdate = TRUE;
		break;
	case VK_UP:
		if( diDesktopInfo.uCurrentSelectIndex )
			diDesktopInfo.uCurrentSelectIndex = (diDesktopInfo.uCurrentSelectIndex - 1) % DESKTOP_CONTROL_NUM;
		else
			diDesktopInfo.uCurrentSelectIndex = DESKTOP_CONTROL_NUM - 1;
		diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
		bUpdate = TRUE;
		break;
	//case VK_RIGHT:
	case VK_RETURN:
		diDesktopInfo.uCurrentSelectState = STATE_SET;
		//g_FocusState = FOCUS_LIST;
		//hwndChild = GetDlgItem( hWnd, diDesktopInfo.uCurrentSelectIndex );
		//SetFocus( hwndChild );
		//SendMessage( hwndChild, LB_SETCURSEL, 0, 0 );
		
		//bUpdate = TRUE;
		RunFocusFunction();
		break;
	}

	SetFocusButton( hWnd, diDesktopInfo.uCurrentSelectIndex );
	/*
	if( oldSelect != diDesktopInfo.uCurrentSelectIndex )
	{
		ShowWindow( GetDlgItem( hWnd, oldSelect ), SW_HIDE );
		ShowWindow( GetDlgItem( hWnd, diDesktopInfo.uCurrentSelectIndex ), SW_SHOW );
	}

	if( bUpdate )
	{
		HDC hdc = GetDC( hWnd );
		//InvalidateRect( hWnd, NULL, FALSE );
		DrawDesktopControl(hWnd,hdc); // 绘制桌面控制
		ReleaseDC( hWnd, hdc );
	}
	*/
	return 0;
}

//重新得到焦点
static int DoSetFocus( HWND hWnd )
{
	HDC hdc = GetDC( hWnd );

	diDesktopInfo.uCurrentSelectState = STATE_FOCUS;	
	//InvalidateRect( hWnd, NULL, FALSE );
	DrawDesktopControl(hWnd,hdc); // 绘制桌面控制
	ReleaseDC( hWnd, hdc );
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
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);//CreateSolidBrush(COLOR_DESKTOPBK);//GetStockObject(WHITE_BRUSH);
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
		case WM_KEYDOWN:
			return DoKeyDown( hWnd, wParam, lParam );
		case WM_ERASEBKGND:  // 绘制背景
			OnDesktopEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_DRAWITEM:
			return DoDrawItem( hWnd, wParam, lParam );
		case WM_LBUTTONDOWN: // 鼠标左按钮按下，或TOUCH笔按下
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_MOUSEMOVE: // 鼠标左按钮按下，或TOUCH笔按下
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_COMMAND:
			if( wParam == 1 && lParam == 0 )
			{	//消息来自于menu item, or list item, VK_RETURN
				HWND hFocusWnd = GetFocus();
				if( GetParent( hFocusWnd ) == hWnd )
				{  //是我的子窗口
					UINT id = GetWindowLong( hFocusWnd, GWL_ID );
					if( id < ID_LIST_MAX )
					{  // yes, listbox
						
						DESKTOP_ITEM_DATA * lpItem;
						UINT indexSel = SendMessage( hFocusWnd, LB_GETCURSEL, 0, 0 );
						LPPTRLIST lpPtrList = GetWindowLong( hFocusWnd, GWL_USERDATA );
						lpItem = (DESKTOP_ITEM_DATA*)PtrListAt( lpPtrList, indexSel );
						if( diDesktopInfo.deskFun[id].idFunc == ID_EXIT )
						{
							exit(0);
						}
						else
							LoadApplication( diDesktopInfo.deskFun[id].lpFuncName, lpItem->szTitle );
					}
				}
			}
			return 0;
		case WM_LBUTTONUP:// 鼠标左按钮弹起，或TOUCH笔离开
			DoDesktopLButtonUp(hWnd,wParam,lParam);
			return 0;
		case WM_TIMER: // 定时器
			//SetFocus( hWnd );
//			DoDesktopTimer(hWnd,wParam,lParam);
			return 0;
		case WM_WINDOWPOSCHANGING: // 窗口位置改变
			return DoWindowPosChanging(hWnd,wParam,lParam);
		case WM_SETFOCUS:
			return DoSetFocus( hWnd );
//            return 0;
		case WM_CLOSE:  // 关闭消息
			DoDesktopClose(hWnd);
			return 0;
		case WM_ACTIVATEAPP:
			SetForegroundWindow( hWnd );
			/*
			if( g_FocusState == FOCUS_LIST )
			{
		        SetFocus( GetDlgItem( hWnd, diDesktopInfo.uCurrentSelectIndex ) );
			}
			else
			{  // desktop
				SetFocus( hWnd );
			}
			*/
			return 0;
		case WM_DESTROY: // 破坏消息
			PostQuitMessage(0);
			return 0;
		case WM_CREATE: // 创建消息
			return DoDesktopCreate(hWnd);
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // 窗口默认消息
   }
   return FALSE;
}

//画磁盘容量, 因为要改变clip rgn , 所以必须放在最后画
static void DrawDiskSpace( HWND hWnd, HDC hdc )
{
	const RECT rc = { 404, 440, 626, 470 };
	UINT uDiskUsed = 11;
	UINT uDiskSpace = 30;
	char szBuf[128];
	HRGN hrgn;

	sprintf( szBuf, "%d GB / %d GB", uDiskUsed, uDiskSpace );

	
	hrgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	SelectObject( hdc, hrgn );
	SetBkMode( hdc, TRANSPARENT );
	SetTextAlign( hdc, TA_RIGHT );
	DrawWallPaper( hWnd, hdc );
	ExtTextOut( hdc, 626, 446, 0, &rc, szBuf, strlen(szBuf), NULL );	

	SetTextAlign( hdc, TA_LEFT );

	DeleteObject( hrgn );	

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
		// 必须最后调用
		//DrawDiskSpace( hWnd, hdc );
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
//	RECT				rt;
	
//	HBRUSH hBrush;


//	char lpDeskTopName[MAX_PATH];

	
	DrawWallPaper( hWnd, hdc );
	
	//rt.left += 30; // 左面是应用程序条
	//rt.bottom -= 20; // 下面是系统状态条
	//hBrush = CreateSolidBrush(COLOR_DESKTOPBK); // 创建背景刷
	//FillRect( hdc, &rt, hBrush ) ; // 填充北京
	//DeleteObject(hBrush); // 删除背景刷
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
//#ifdef LINUX_KERNEL
	strcpy(lpDeskTopName,"./kingmos/desktop/desktop_bk.bmp"); // 复制指定的桌面背景位图
//#else
//	strcpy(lpDeskTopName,".\\kingmos\\bg_home.bmp"); // 复制指定的桌面背景位图
//#endif
}

static void SetFocusButton( HWND hWnd, int idx )
{
	HDC hdc;
	diDesktopInfo.uCurrentSelectIndex = idx;
	diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
	
	hdc = GetDC( hWnd );
	DrawDesktopControl(hWnd,hdc); // 绘制桌面控制
	ReleaseDC( hWnd, hdc );
}

static void RunFocusFunction( void )
{
	if( diDesktopInfo.deskFun[diDesktopInfo.uCurrentSelectIndex].idFunc == ID_EXIT )
	{
		exit(0);
	}
	else
		LoadApplication( diDesktopInfo.deskFun[diDesktopInfo.uCurrentSelectIndex].lpFuncName, NULL );
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
	
	//		g_indexButtonDown = indexButton; // 设置按下按钮
	if( indexButton != -1 )
	{
		if( diDesktopInfo.uCurrentSelectIndex != indexButton )
		{
			SetFocusButton( hWnd, indexButton );
//			HDC hdc;
//			diDesktopInfo.uCurrentSelectIndex = indexButton;
//			diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
//			
//			hdc = GetDC( hWnd );
//			DrawDesktopControl(hWnd,hdc); // 绘制桌面控制
//			ReleaseDC( hWnd, hdc );
		}
		else
		{
			RunFocusFunction();
			/*
			if( diDesktopInfo.deskFun[indexButton].idFunc == ID_EXIT )
			{
				exit(0);
			}
			else
				LoadApplication( diDesktopInfo.deskFun[indexButton].lpFuncName, NULL );
				*/
		}
	}
	
	g_FocusState = FOCUS_DESKTOP;
	//SetFocus( hWnd );		
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
	int i;
	POINT pt = { ptCursor.x, ptCursor.y };
	for( i = 0; i < DESKTOP_CONTROL_NUM; i++ )
	{
		RECT rc = diDesktopInfo.deskFun[i].rectFunc;
		OffsetRect( &rc, ICON_OFFSET_X, ICON_OFFSET_Y );
		if( PtInRect( &rc, pt ) )
			return i;
	}
	return -1;
}

//初始化　各个LIST data
//static BOOL InitListData( HWND hParent, HWND hListWnd, int idList )
static BOOL InitListData( int idList )
{
	int i;
	const char ** lppszText;
	const HBITMAP ** lpphbmpIcon;
	DESKTOP_ITEM_DATA * lpItemData;
	int num = 0;
	LPPTRLIST lpPtrList = NULL;
	if( idList == ID_LIST_BACKUP )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_BACKUP];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_PLAYBACK )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_PLAYBACK];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_PLAYSOUND )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_PLAYSOUND];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_PLAYMOVIE )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_PLAYMOVIE];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_SETUP )
	{
		const char * lpszStockText[] = { "Display", "System", "Backup behavior", "Playback behavior", "Languages", "Date & Time" };
		const HBITMAP * lphStockIcon[] = { &hbmpNULL, &hbmpNULL, &hbmpNULL, &hbmpNULL, &hbmpNULL, &hbmpNULL };

		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		num = sizeof(lpszStockText) / sizeof( char* );
		//lpPtrList = &ptrList_Setup;
		lpPtrList = &ptrList[ID_LIST_SETUP];
	}


	if( lpPtrList )
	{
		PtrListCreate( lpPtrList, 8, 16 );
		
		for( i = 0; i < num; i++ )
		{
			UINT len;
			len = strlen( lppszText[i] );
			lpItemData = malloc( sizeof( DESKTOP_ITEM_DATA ) +  len );
			if( lpItemData )
			{
				lpItemData->cTitleLength = len;
				lpItemData->hbmpIcon = *lpphbmpIcon[i];
				strcpy( lpItemData->szTitle, lppszText[i] );
				PtrListInsert( lpPtrList, lpItemData );
			}
		}
	}
	return TRUE;
}

static BOOL InitAllListData( VOID )
{
	int i = 0;
	for( i = 0; i < ID_LIST_MAX; i++ )
		InitListData( i );
	return TRUE;
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
static int DoCreateListWindow(HWND hParent, UINT id, HINSTANCE hInst)
{
	return 0;

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
static int DoDesktopCreate(HWND hWnd)
{
	//SetTimer( hWnd, 0, 2000, NULL );
	//SetFocus( hWnd );
	return 0;
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
		//g_hDesktopBitmap = JPEG_LoadByName( "./kingmos/desktop/mapbj.jpg", NULL ) ; // 装载墙纸

//		RETAILMSG(1,("***** Load Bitmap Success \r\n"));
		if( g_hDesktopBitmap == NULL )
		{  // 装载墙纸失败
			return ;
		}
	}

	GetObject(g_hDesktopBitmap,sizeof(bitmap),&bitmap);  // 得到位图结构
	// 得到位图大小
	rect.left = 0;
	rect.right = 640;
	rect.top = 0;
	rect.bottom = 480 ;
	
	DrawBitMap( hdc, g_hDesktopBitmap, &rect, 0, 0 ,SRCCOPY);  // 绘制位图
}
// **************************************************
// 声明：static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// 参数：
//	IN hdc - 显示设备句柄
//	IN lprc - RECT 结构指针，指向需要填充的矩形
//	IN clr - RGB颜色值
// 返回值：
//	无
// 功能描述：
//	用固定颜色填充举行矩形
// 引用: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}

//static LPPTRLIST GetCurrentPtrList( HWND hWnd )
//{
//	return ptrList[diDesktopInfo.uCurrentSelectIndex];	
//}


// ********************************************************************
//声明：BOOL	DoDrawItem_ListBox( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN idCtl - 消息参数
//  IN pDrawItem - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：WM_DRAWITEM的列表框 处理过程
//引用: 
// ********************************************************************
#define	TIME_WIDTH		110
BOOL	DoDrawItem_ListBox( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
{
	HDC			hdc = pDrawItem->hDC;
	LPPTRLIST   lpPtrList = (LPPTRLIST)GetWindowLong( pDrawItem->hwndItem, GWL_USERDATA );   
    DESKTOP_ITEM_DATA * lpItem = PtrListAt( lpPtrList, pDrawItem->itemID );
	RECT rcIcon;

	//
	SetBkMode( hdc, TRANSPARENT );
	if( (pDrawItem->itemState & ODS_SELECTED) 
		 )		//得到 Select
	{
		//FillRect( hdc, &pDrawItem->rcItem, GetStockObject( WHITE_BRUSH ) );
		//FillSolidRect( hdc, &pDrawItem->rcItem, RGB( 0xff, 203, 90 ) );
		DrawBitMap( hdc, hbmpTextBarSelect, &pDrawItem->rcItem, 0, 0 ,SRCCOPY);  // 绘制位图
		//DrawBitmap( hdc, ,   
		
		if( lpItem )
		{		
			SetTextColor( hdc, RGB( 13, 13, 13 ) );
			TextOut( hdc, pDrawItem->rcItem.left + 61, pDrawItem->rcItem.top + 12, lpItem->szTitle, lpItem->cTitleLength );
		}
		if( lpItem->hbmpIcon )
		{
			rcIcon = pDrawItem->rcItem;
			rcIcon.left += 10;
			rcIcon.top += 3;
			rcIcon.right = rcIcon.left + 40;
			rcIcon.bottom = rcIcon.top + 40;
			DrawTransparentBitMap( hdc, lpItem->hbmpIcon,  &rcIcon, 0, 0, CLR_TRANSPARENT );
		}
	}
	else
	{
		//FillRect( hdc, &pDrawItem->rcItem, GetStockObject( GRAY_BRUSH ) );
		DrawBitMap( hdc, hbmpTextBarNormal, &pDrawItem->rcItem, 0, 0 ,SRCCOPY);  // 绘制位图
		
		if( lpItem )
		{		
			SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
			TextOut( hdc, pDrawItem->rcItem.left + 61, pDrawItem->rcItem.top + 12, lpItem->szTitle, lpItem->cTitleLength );
		}
		if( lpItem->hbmpIcon )
		{
			rcIcon = pDrawItem->rcItem;
			rcIcon.left += 10;
			rcIcon.top += 3;
			rcIcon.right = rcIcon.left + 40;
			rcIcon.bottom = rcIcon.top + 40;
			
			DrawTransparentBitMap( hdc, lpItem->hbmpIcon,  &rcIcon, 0, 0, CLR_TRANSPARENT );
		}

	}

	return TRUE;
}

// ********************************************************************
//声明：BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
//参数：
//	IN hWnd - 应用程序的窗口句柄
//  IN idCtl - 消息参数
//  IN pDrawItem - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：WM_DRAWITEM 处理过程
//引用: 
// ********************************************************************
static int	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
{
	if( pDrawItem->CtlType==ODT_LISTBOX )
	{
		return DoDrawItem_ListBox( hWnd, idCtl, pDrawItem );
	}
	return 0;
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

#define HILIGHT_COLOR  RGB( 180, 180, 20 )
#define NORMAL_COLOR  RGB( 80, 80, 80 )
static void DrawDesktopControl(HWND hWnd,HDC hdc)
{
	//int iButtonCount = sizeof(DeskTopFuncList)/sizeof(DESKTOPFUNC);  // 得到按钮总数
	//int i;
	
	//SetBkColor( hdc, RGB(0xff, 0, 0 ) );
	//for (i=0;i<iButtonCount;i++)
	//{
	//	DrawText( hdc, DeskTopFuncList[i].lpFuncName, -1, &DeskTopFuncList[i].rectFunc, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	//}
	UINT i;
//	return 0;

	
	SetBkMode( hdc, TRANSPARENT );
	SelectObject( hdc, GetStockObject(  BLACK_BRUSH ) );
	
	
	for( i = 0; i < DESKTOP_CONTROL_NUM; i++ )
	{
		RECT rc = diDesktopInfo.deskFun[i].rectFunc;
		OffsetRect( &rc, ICON_OFFSET_X, ICON_OFFSET_Y );

		
		if( i == diDesktopInfo.uCurrentSelectIndex )
		{
			HPEN hPen = CreatePen( PS_SOLID, 1, HILIGHT_COLOR );

			hPen = SelectObject( hdc, hPen );
			SetTextColor( hdc, HILIGHT_COLOR );
			RoundRect( hdc, rc.left, rc.top, rc.right, rc.bottom, 18, 18 );
			DrawText( hdc, diDesktopInfo.deskFun[i].lpTitle, strlen(diDesktopInfo.deskFun[i].lpTitle), &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
			hPen = SelectObject( hdc, hPen );
			DeleteObject( hPen );
			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpState[diDesktopInfo.uCurrentSelectState],  &diDesktopInfo.deskFun[i].rectFunc, 0, 0, CLR_TRANSPARENT );
			
			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpIcon[0],  &diDesktopInfo.deskFun[i].rectIcon, 0, 0, CLR_TRANSPARENT );
		}
		else
		{
			HPEN hPen = CreatePen( PS_SOLID, 1, NORMAL_COLOR );

			hPen = SelectObject( hdc, hPen );
			SetTextColor( hdc, NORMAL_COLOR );
			RoundRect( hdc, rc.left, rc.top, rc.right, rc.bottom, 18, 18  );
			DrawText( hdc, diDesktopInfo.deskFun[i].lpTitle, strlen(diDesktopInfo.deskFun[i].lpTitle), &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

			hPen = SelectObject( hdc, hPen );
			DeleteObject( hPen );

			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpState[0],  &diDesktopInfo.deskFun[i].rectFunc, 0, 0, CLR_TRANSPARENT );
			
			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpIcon[0],  &diDesktopInfo.deskFun[i].rectIcon, 0, 0, CLR_TRANSPARENT );
		}		
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
static void DrawBitMap( HDC hdc,HBITMAP hBitmap, LPRECT lprect,UINT xOffset,UINT yOffset,DWORD dwRop )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // 创建一个兼容的内存句柄
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将要绘制的位图设置到内存句柄中
		BitBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
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
