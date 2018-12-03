/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小巧IE浏览程序
版本号：1.0.0.456
开发时期：2002-04-01
作者：陈建明 Jami chen
修改记录：
**************************************************/
#include <ewindows.h>
#include <Etoolbar.h>
#include <eHtmView.h>
#include <EShell.h>
#include "eComCtrl.h"
#include "resource.h"
#include <ecommdlg.h>
#include "emailformat.h"

//#define TOOLBAR_ICON_NUMBER			3
#define MAX_LOADSTRING				100
#define IDC_EXIT					101
#define IDC_ADDRESS					103
#define IDC_VIEW					104
#define IDC_GO						105
#define IDC_GOBACK					106
#define IDC_FORWARDS				107
#define IDC_STOP					108
#define IDC_OPEN					109
#define IDC_SAVE					110
#define IDC_FRESH					111
#define IDC_EMAIL					112

#define TOOLBAR_USEIMAGELIST

#define OPENHTML		0x0001
#define SAVEHTML		0x0002
#define SAVEIMG			0x0003

typedef struct tagIExploreData
{
	HWND		hToolbar ; // tool bar handle
	HWND		hAddress ; // address combobox handle
	HWND		hIExplore ; // IExplore handle 
}_IEDATA, *_LPIEDATA ;

const static TCHAR szIETitle[] = "IE浏览" ;
const static TCHAR szHelpTitle[] = "Helper" ;								// The title bar text
const static TCHAR szWindowClass[] = "_class_IExplorer";								// The title bar text

static ATOM MyRegisterClass(HINSTANCE hInstance);
static HWND InitInstance(HINSTANCE hInstance, const TCHAR *lpszTitle );

static BOOL CALLBACK IExploreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL InitIExplore(HINSTANCE hInstance,HWND hParentWnd);
static BOOL DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static int InitialIExploreWindow( HWND hWnd , TCHAR *lpszIndex );
static LRESULT DoGo(HWND hWnd);
static LRESULT DoGoBack(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoOpen(HWND hWnd);
static LRESULT DoSave(HWND hWnd);
static LRESULT DoEmail(HWND hWnd);
static LRESULT DoStop(HWND hWnd);
static void DoAddressChange(HWND hWnd,LPARAM lParam);;
static LRESULT DoAddressNotify(HWND hWnd,WORD wNotifyCode,LPARAM lParam);
static LRESULT DoHtmlViewNotify(HWND hWnd,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL CreateIExploreToolbar( HWND hWnd ) ;

static BOOL IsLocateFile(LPTSTR lpUrl);
static void SetButtonImage(HWND hWnd,UINT id,UINT iImage);
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName,WORD OpenStyle);


//extern ATOM RegisterHtmlViewClass( HINSTANCE hInstance);

static BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
static BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);


// !!! Add By Jami chen in 2003.09.12
static BOOL HaveExtName(LPTSTR lpFileName);
static void AddExtName(LPTSTR lpFileName,LPTSTR lpExtName);
// !!! Add End By jami chen in 2003.09.12



// **************************************************
// 声明：static _LPIEDATA GetIEPtr( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：返回IE浏览结构指针
// 功能描述：得到IE浏览结构指针。
// 引用: 
// **************************************************
static _LPIEDATA GetIEPtr( HWND hWnd )
{
	_LPIEDATA		lpied ;

	lpied = ( _LPIEDATA )GetWindowLong( hWnd, GWL_USERDATA ) ; // 得到IE浏览结构指针
	ASSERT( lpied != NULL ) ;
//	RETAILMSG(1,(TEXT("***lpied = %x\r\n"),lpied));
	
	return lpied ; // 返回指针
}
BOOL LinkNet(HWND h, DWORD dw)
{
	return FALSE;
}
// ********************************************************************
//声明：void WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI SMIExplore(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
	MSG msg;
	HWND			hWnd;

	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找指定类名的窗口是否已经存在
	if( hWnd != 0 )
	{ // 已经存在，说明应用程序已经打开
		SetForegroundWindow( hWnd ) ; // 设置窗口到前台
		if (lpCmdLine != NULL && strlen(lpCmdLine) != 0)
			InitialIExploreWindow( hWnd, lpCmdLine ); // 并初始化参数--即打开指定的文件
		return FALSE ;
	}

	InitCommonControls(); // 初始化通用控件
	RegisterSMIE_BrowersClass( hInstance);
	MyRegisterClass( hInstance ); // 注册应用程序类

	if ( (hWnd=InitInstance (hInstance, szIETitle )) == NULL )  // 初始化应用程序
	{
		return FALSE;
	}
	SetForegroundWindow(hWnd);
//	InitialIExploreWindow( hWnd, "http://jami.mlg.com/" );
	if (lpCmdLine == NULL || strlen(lpCmdLine) == 0)
	{ // 没有参数，打开HOME PAGE
		TCHAR lpHomePage[MAX_PATH];
		// 从注册表打开HOME PAGE
		if (regReadData(HKEY_SOFTWARE_ROOT,
						"SoftWare\\IExplorer", "Home Page",lpHomePage, MAX_PATH,REG_SZ) == FALSE)
		{ // 没有找到
			strcpy(lpHomePage,"\\kingmos\\kingmos.htm"); // 设置系统指定文件为Home Page
			// 设置该Home Page到注册表
			regWriteData(HKEY_SOFTWARE_ROOT,"SoftWare\\IExplorer", "Home Page",lpHomePage, strlen(lpHomePage)+1,REG_SZ);
		}
		// 打开Home Page
		InitialIExploreWindow( hWnd, lpHomePage );
	}
	else
		InitialIExploreWindow( hWnd, lpCmdLine ); // 打开用户指定的文件
//	InitialIExploreWindow( hWnd, "\\MyWeb6\\index.htm" );
	// 消息循环
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
/*
int WINAPI HelpExplore(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG				msg;
	HWND			hWnd ;
	_LPIEDATA		lpied ;

//	RegisterHtmlViewClass( hInstance);
	MyRegisterClass( hInstance );

	if (( hWnd = InitInstance (hInstance, szHelpTitle ) ) == NULL ) 
	{
		return FALSE;
	}
	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针
	ShowWindow( lpied->hAddress, SW_HIDE ) ;
	InitialIExploreWindow( hWnd , "\\help\\index.htm" );
	MoveWindow( lpied->hIExplore, 0, 0, 240, 270, TRUE ) ;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
*/
// **************************************************
// 声明：static ATOM MyRegisterClass(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册值
// 功能描述：向系统注册应用程序类。
// 引用: 
// **************************************************
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)IExploreProc; // 窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_IEXPLORER),IMAGE_ICON,16,16,0); // 应用程序图标
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 应用程序类名

	return RegisterClass(&wc); // 注册类
}
// **************************************************
// 声明：static HWND InitInstance(HINSTANCE hInstance, const TCHAR *lpszTitle )
// 参数：
// 	IN hInstance -- 应用程序实例句柄
// 	IN lpszTitle -- 应用程序标题
// 
// 返回值：返回创建的窗口句柄
// 功能描述：初始化应用程序。
// 引用: 
// **************************************************
static HWND InitInstance(HINSTANCE hInstance, const TCHAR *lpszTitle )
{
	HWND			hWnd;
	int width,heigh;
   

    width = GetSystemMetrics(SM_CXSCREEN);
	heigh = GetSystemMetrics(SM_CYSCREEN);	
//	RECT			rt ;

//	GetWindowRect( GetDesktopWindow(), &rt ) ;
	// 创建应用程序主窗口
	hWnd = CreateWindowEx( WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, lpszTitle, WS_VISIBLE|WS_SYSMENU,
		//rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top,
		0,0,width,heigh,
		NULL, NULL, hInstance, NULL);

	return hWnd ; // 返回窗口句柄
}
// **************************************************
// 声明：static void OnIExplorePaint( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：绘制窗口。
// 引用: 
// **************************************************
static void OnIExplorePaint( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC hdc;
	// 这两个函数是必须的
	hdc = BeginPaint(hWnd, &ps); // 开始绘制
	EndPaint(hWnd, &ps); // 绘制结束
}
// **************************************************
// 声明：static void OnIExploreEraseBkgnd( HWND hWnd, HDC hdc )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述：绘制窗口背景。
// 引用: 
// **************************************************
static void OnIExploreEraseBkgnd( HWND hWnd, HDC hdc )
{
	RECT		rt ;
	HBRUSH hBrush;

	GetClientRect( hWnd, &rt ) ; // 得到窗口客户矩形
	rt.bottom = rt.top + 24 ;
	FillRect( hdc, &rt, GetStockObject(WHITE_BRUSH) ) ; // 上部填充为白色
	rt.top = rt.bottom ;
	rt.bottom = rt.top + 25 ;
	hBrush = CreateSolidBrush(RGB(247,255,247));
	FillRect( hdc, &rt, hBrush ) ; // 下部填充为浅灰色
	DeleteObject(hBrush);
//	MoveTo( hdc, rt.left, rt.bottom ) ;
//	LineTo( hdc, rt.right, rt.bottom ) ;
//	rt.left += 5 ;
//	SetBkMode( hdc, TRANSPARENT ) ;
//	DrawText( hdc, "地址:", strlen("地址:"), &rt, DT_SINGLELINE|DT_LEFT|DT_VCENTER ) ;
}
// **************************************************
// 声明：static BOOL OnIExploreCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：处理窗口命令消息
// 引用: 
// **************************************************
static BOOL OnIExploreCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	WORD wmId, wmEvent;
	wmId    = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	switch (wmId)
	{
	case IDC_GO:// 打开地址栏内的网页
		DoGo(hWnd);
		break;
	case IDC_GOBACK: // 回到上一次打开的网页
		DoGoBack(hWnd,wParam,lParam);
		break;
	case IDC_FORWARDS: // 去到接下来已经打开的网页
		DoForWards(hWnd,wParam,lParam);
		break;
	case IDC_EXIT: // 退出应用程序
		DestroyWindow(hWnd);
		break;
	case IDC_ADDRESS: // 地址栏通知消息
		DoAddressNotify(hWnd,wmEvent,lParam);
		break;
	case IDC_OPEN: // 打开一个指定的文件
		DoOpen(hWnd);
		break;
	case IDC_SAVE: // 将当前打开的网页保存到一个文件
		DoSave(hWnd);
		break;
	case IDC_EMAIL: // 将当前打开的网页用EMAIL发送出去
		DoEmail(hWnd);
		break;
	case IDC_STOP: // 停止打开网页
		DoStop(hWnd);
		break;
	}
	return TRUE ;
}
// **************************************************
// 声明：static void OnIExploreDestroy( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：破坏窗口。
// 引用: 
// **************************************************
static void OnIExploreDestroy( HWND hWnd )
{
	_LPIEDATA	lpied ;
	HWND hItem;
	HICON hIcon;

	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针

	hItem = GetDlgItem(hWnd,IDC_GO); // 得到GO的窗口句柄
	hIcon = (HICON)SendMessage(hItem,BM_GETIMAGE,IMAGE_ICON,0); // 得到GO窗口的ICON句柄

	if (hIcon != NULL)
	{ // 句柄存在
		DestroyIcon(hIcon); // 破坏ICON
		hIcon = NULL;
	}

	free( lpied ) ; // 释放IE浏览结构指
}
// **************************************************
// 声明：BOOL CALLBACK IExploreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN message -- 消息代码
// 	IN wParam -- 参数
// 	IN lParam -- 参数
// 
// 返回值：返回消息处理结果
// 功能描述：IE浏览窗口处理过程。
// 引用: 
// **************************************************
BOOL CALLBACK IExploreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE: // 创建窗口
			if( DoCreate(hWnd,wParam,lParam) == FALSE )
				return -1 ;
			return 0 ;
		case WM_COMMAND: // 命令消息
			OnIExploreCommand( hWnd, wParam, lParam ) ;
			break;
		case WM_PAINT: // 绘制消息
			OnIExplorePaint( hWnd ) ;
			break;
		case WM_ERASEBKGND: // 刷新背景
			OnIExploreEraseBkgnd( hWnd, (HDC)wParam ) ;
			break ;
		case WM_NOTIFY: // 通知消息
			DoNotify(hWnd,wParam,lParam);
			break;
		case WM_DESTROY: // 破坏消息
			OnIExploreDestroy( hWnd ) ;
			break ;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
// 	return TRUE;
}
// **************************************************
// 声明：static int InitialIExploreWindow( HWND hWnd , TCHAR *lpszIndex )
// 参数：
// 	IN hWnd -- 窗口句柄
//	 IN lpszIndex -- 要打开的网页
// 
// 返回值：返回0
// 功能描述： 初始化IE浏览窗口。
// 引用: 
// **************************************************
static int InitialIExploreWindow( HWND hWnd , TCHAR *lpszIndex )
{
//	_LPIEDATA		lpied ;
	UINT			iIndex;
	HWND hAddress,hIExplore;

//	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针
		
/********************************/
	hAddress = GetDlgItem(hWnd,IDC_ADDRESS); // 得到地址栏窗口句柄
	hIExplore = GetDlgItem(hWnd,IDC_VIEW); // 得到IE浏览窗口句柄
	iIndex = SendMessage( hAddress, CB_FINDSTRING, 0, (LPARAM)lpszIndex ); // 查找地址栏是否已经存在此地址
	if ( iIndex == CB_ERR )
	{
//		RETAILMSG(1,(TEXT("Will Add Current Address to ComboBox!!!\r\n")));
		iIndex = SendMessage( hAddress, CB_ADDSTRING, 0, (LPARAM)lpszIndex ); // 插入该地址到地址栏中
	}
//ETAILMSG(1,(TEXT("Set Current Address Select!!!\r\n")));
	SendMessage( hAddress, CB_SETCURSEL, iIndex, 0 ); // 设置该地址为当前地址
/*******************************/
	SendMessage( hIExplore, HM_GO, 0, (LPARAM)lpszIndex ); // 打开该网页

	return 0;
}
// **************************************************
// 声明：static BOOL DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 参数
// 	IN lParam -- 参数
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：创建窗口。
// 引用: 
// **************************************************
static BOOL DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	_LPIEDATA		lpied ;
	HINSTANCE		hInst ;
	RECT rt;
	GetWindowRect(hWnd, &rt);
	lpied = ( _LPIEDATA )malloc( sizeof( _IEDATA ) ) ;  // 分配IE浏览数据结构
	if( lpied == NULL )
		return FALSE ; // 分配失败
//	RETAILMSG(1,(TEXT("******** new *******lpied = %x\r\n"),lpied));
	SetWindowLong( hWnd, GWL_USERDATA, (long)lpied ) ; // 设置结构到窗口

	hInst = ( HINSTANCE )GetWindowLong( hWnd, GWL_HINSTANCE ) ; // 得到实例句柄
	// 创建“地址”静态框
	CreateWindow( "Static",  "地址:", 
		WS_CHILD|WS_VISIBLE,2, 25, 40, 20,
		hWnd, (HMENU)0xFFFF, hInst, NULL);

//	lpied->hAddress = CreateWindow( "ComboBox",  "", 
//		WS_CHILD|WS_VISIBLE|CBS_DROPDOWN|CBS_AUTOHSCROLL,42, 25, 148, 128,
//		hWnd, (HMENU)IDC_ADDRESS, hInst, NULL);
	// 创建地址栏
	lpied->hAddress = CreateWindowEx( WS_EX_CLIENTEDGE,"ComboBox",  "", 
		WS_CHILD|WS_VISIBLE|CBS_DROPDOWN|CBS_AUTOHSCROLL,42, 25, 148, 128,
		hWnd, (HMENU)IDC_ADDRESS, hInst, NULL);
	if( lpied->hAddress == 0 )
		return FALSE ; // 创建失败
	// 创建GO按钮
	CreateWindow( "Button",  "", 
		WS_CHILD|WS_VISIBLE|BS_ICON,192, 25, 18, 18,
		hWnd, (HMENU)IDC_GO, hInst, NULL);
	SetButtonImage(hWnd,IDC_GO,IDI_GO);

	// 创建IE浏览窗口
	lpied->hIExplore = CreateWindow( classSMIE_BROWERS, "",
		WS_CHILD|WS_VISIBLE|WS_VSCROLL,0, 46, rt.right - rt. left, rt.bottom - rt.top - 66,
		hWnd, (HMENU)IDC_VIEW, hInst, NULL);
/*		
	lpied->hIExplore = CreateWindow( classHtmlView, "",
		WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER,0, 45, 240, 235,
		hWnd, (HMENU)IDC_VIEW, hInst, NULL);
*/
	if( lpied->hIExplore == 0 )
		return FALSE ; // 创建失败

	if( CreateIExploreToolbar( hWnd ) == FALSE ) // 创建工具条
		return FALSE ; // 创建失败

//	InitialIExploreWindow( hWnd, "\\help\\test.htm" );
//	InitialIExploreWindow( hWnd, "http://jami.mlg.com/" );

	return TRUE; // 创建成功
}
// **************************************************
// 声明：static LRESULT DoGo(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：打开当前网页。
// 引用: 
// **************************************************
static LRESULT DoGo(HWND hWnd)
{
	_LPIEDATA		lpied ;
	TCHAR			lpAddress[MAX_PATH];
	UINT iIndex;

	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针

	SendMessage( lpied->hAddress, WM_GETTEXT, MAX_PATH, (LPARAM)lpAddress ); // 得到当前网页地址

/********************************/
//	RETAILMSG(1,(TEXT("Will Enter New Address!!!\r\n")));
	iIndex = SendMessage( lpied->hAddress, CB_FINDSTRING, 0, (LPARAM)lpAddress ); // 查找当前网页是否已经存在地址列表中
	if ( iIndex == CB_ERR )
	{ // 没有存在
//		RETAILMSG(1,(TEXT("Will Add Current Address to ComboBox!!!\r\n")));
		iIndex = SendMessage( lpied->hAddress, CB_ADDSTRING, 0, (LPARAM)lpAddress ); // 将当前地址插入到地址列表中
	}
//	RETAILMSG(1,(TEXT("Set Current Address Select!!!\r\n")));
	SendMessage( lpied->hAddress, CB_SETCURSEL, iIndex, 0 ); // 设置要打开的网页为当前地址
/*******************************/
	SendMessage( lpied->hIExplore, HM_GO, 0, (LPARAM)lpAddress ); // 打开网页
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGoBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数，保留
// 	IN lParam -- 消息参数，保留
// 
// 返回值：无
// 功能描述：回到上一次打开的网页。
// 引用: 
// **************************************************
static LRESULT DoGoBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPIEDATA		lpied ;

	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针
	SendMessage( lpied->hIExplore, HM_GOBACK, 0, 0 ); // 打开上一次打开的网页
	return 0;
}
// **************************************************
// 声明：static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数，保留
// 	IN lParam -- 消息参数，保留
// 
// 返回值：无
// 功能描述：打开前一个网页。
// 引用: 
// **************************************************
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPIEDATA		lpied ;

	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针
	SendMessage( lpied->hIExplore, HM_FORWARDS, 0, 0 ) ; // 打开前一个网页
	return 0;
}
// **************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 通知消息参数
// 	IN lParam -- 通知消息参数
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
		case IDC_VIEW: // IE浏览的通知消息
			DoHtmlViewNotify(hWnd,lParam);
			break;
		default:
			break;
	}
	return 0;
}
// **************************************************
// 声明：static void DoAddressChange( HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 参数
// 
// 返回值：无
// 功能描述：地址栏选择发生改变的通知消息。
// 引用: 
// **************************************************
static void DoAddressChange( HWND hWnd, LPARAM lParam )
{
	UINT			iIndex;
	LPTSTR			lpUrl;
	DWORD			size;
	_LPIEDATA		lpied ;

	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针

	iIndex = SendMessage( lpied->hAddress, CB_GETCURSEL, 0, 0 ) ; // 得到当前地址索引
	size = SendMessage( lpied->hAddress, CB_GETLBTEXTLEN, iIndex, 0 ); // 得到当前地址的长度
	lpUrl = (LPTSTR)malloc( size+2 ); // 分配地址空间
	if( lpUrl == NULL )
		return  ; // 分配失败
	size = SendMessage( lpied->hAddress, CB_GETLBTEXT, iIndex, (LPARAM)lpUrl ); // 得到当前的地址
	SendMessage(lpied->hIExplore, HM_GO, 0, (LPARAM)lpUrl ); // 打开当前地址的网页
	free( lpUrl );
} 
// **************************************************
// 声明：static LRESULT DoAddressNotify(HWND hWnd,WORD wNotifyCode,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wNotifyCode -- 通知消息
// 	IN lParam -- 参数
// 
// 返回值：返回消息处理结果
// 功能描述：地址栏的通知消息。
// 引用: 
// **************************************************
static LRESULT DoAddressNotify(HWND hWnd,WORD wNotifyCode,LPARAM lParam)
{
	switch(wNotifyCode)
	{
		case CBN_SELCHANGE: // 地址栏的当前选择发生改变
			DoAddressChange( hWnd, lParam );
			break;
		default :
			break;
	}
	return 0;
}
// **************************************************
// 声明：static LRESULT DoHtmlViewNotify( HWND hWnd, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 通知消息参数
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static LRESULT DoHtmlViewNotify( HWND hWnd, LPARAM lParam )
{
	LPNMINTERNET	lpnmInternet;
	UINT			iIndex;
//	LPTSTR			lpUrl ;//lpBuffer=NULL;
//	_LPIEDATA		lpied ;
	HWND hAddress;

//	lpied = GetIEPtr( hWnd ) ; // 得到IE浏览结构指针

//	return 0;
	lpnmInternet=(LPNMINTERNET)lParam; // 得到通知消息结构句柄

/*	if ( IsLocateFile( lpnmInternet->lpUrl ) )
	{
 		lpUrl = lpnmInternet->lpUrl;
	}
	else
	{
		lpUrl = lpnmInternet->lpUrl;
	}
*/
//	RETAILMSG(1,(TEXT("lpied = %x!!!\r\n"),lpied));
//	RETAILMSG(1,(TEXT("lpied->hAddress = %x!!!\r\n"),lpied->hAddress));

	hAddress = GetDlgItem(hWnd,IDC_ADDRESS); // 得到地址栏句柄
//	RETAILMSG(1,(TEXT("Will Enter New Address!!!\r\n")));
	iIndex = SendMessage( hAddress, CB_FINDSTRING, 0, (LPARAM)lpnmInternet->lpUrl ); // 查找当前打开的网页是否在地址列表中
	if ( iIndex == CB_ERR )
	{ // 没有存在
//		RETAILMSG(1,(TEXT("Will Add Current Address to ComboBox!!!\r\n")));
		iIndex = SendMessage( hAddress, CB_ADDSTRING, 0, (LPARAM)lpnmInternet->lpUrl ); // 将当前打开的网页地址插入到地址列表中
	}
//	RETAILMSG(1,(TEXT("Set Current Address Select!!!\r\n")));
	SendMessage( hAddress, CB_SETCURSEL, iIndex, 0 ); // 设置当前打开的网页地址为当前地址

//	RETAILMSG(1,(TEXT("OK!!!!!!!!!!!!!!\r\n")));
	return 0;
		
}
// **************************************************
// 声明：static BOOL IsLocateFile( LPTSTR lpUrl )
// 参数：
// 	IN lpUrl -- 指定的URL(网页地址)
// 
// 返回值：指定的URL是本地文件返回TRUE，否则返回FALSE
// 功能描述：判断指定的URL是否是本地文件。
// 引用: 
// **************************************************
static BOOL IsLocateFile( LPTSTR lpUrl )
{
	HANDLE in_file;
	// 打开指定的URL
	in_file = CreateFile( lpUrl, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL );
	if ( in_file == INVALID_HANDLE_VALUE )
	{  // 打开失败，不是本地文件
		return FALSE;
	}
	CloseHandle(in_file); // 关闭文件句柄
	return TRUE;
}

// **************************************************
// 声明：static BOOL CreateIExploreToolbar( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄 
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：创建工具条。
// 引用: 
// **************************************************
static BOOL CreateIExploreToolbar( HWND hWnd )
{
	_LPIEDATA	lpied ;
	HWND		hToolbar ;
	TBBUTTON	tb;
	int			i;

typedef struct IEToolBar{
	BOOL bSysIcon;
	UINT ID;
	UINT IDResource;
	LPTSTR lpString;
}IETOOLBAR;
	IETOOLBAR IEToolBar[]={
	{TRUE,IDC_OPEN,OBM_TOOL_OPEN,"Open"}, // 打开文件
	{FALSE,IDC_SAVE,IDB_SAVE,"Save"}, // 保存文件
	{FALSE,IDC_GOBACK,IDB_GOBACK,"GoBack"}, // 打开上一个已经打开过的文件
	{FALSE,IDC_FORWARDS,IDB_FORWARDS,"Forwards"}, // 打开前一个已经打开过的文件
	{FALSE,IDC_FRESH,IDB_FRESH,"Fresh"}, // 刷新当前打开的网页
	{FALSE,IDC_STOP,IDB_STOP,"Stop"}, // 停止打开网页
	{TRUE, IDC_EMAIL,OBM_TOOL_EMAIL,"Email"}, // 发送EMAIL
};
	HIMAGELIST hImageList;
	HBITMAP hBitmap;
	UINT iToolBarNum ;

	lpied = GetIEPtr( hWnd ) ;// 得到IE浏览结构指针

	// 创建一个工具条
	hToolbar = CreateToolbarEx( hWnd, WS_VISIBLE|WS_CHILD|TBSTYLE_FLAT|TBSTYLE_LIST|CCS_TOP,
			11, 0, NULL, 0, NULL,0,20,20,16,16,0 );
	if( hToolbar == 0 )
	{ // 创建失败
		EdbgOutputDebugString("=========Create ToolBar is Failure===========\r\n");
		return 0;
	}
	iToolBarNum = sizeof(IEToolBar) / sizeof(IETOOLBAR ); // 得到工具条按钮的个数
	hImageList=ImageList_Create(16,16,ILC_COLOR8,10,10); // 创建一个图象列表
	for (i=0;i<(int)iToolBarNum;i++)
	{
		// 装入图象
		if (IEToolBar[i].bSysIcon == TRUE)
			hBitmap = LoadImage( NULL, MAKEINTRESOURCE(IEToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;
		else
			hBitmap = LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IEToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;
		// 将图象插入到图象列表中
		if (hBitmap )
			ImageList_Add(hImageList,hBitmap,NULL);
	}

//	SendMessage( hToolbar, TB_LOADIMAGES , IDB_STD_SMALL_COLOR , ( LPARAM )HINST_COMMCTRL ) ;
	hImageList =(HIMAGELIST)SendMessage( hToolbar, TB_SETIMAGELIST , 0, ( LPARAM )hImageList) ; // 插入图象列表到工具条
	
	// 删除原来的ImageList
	if (hImageList)
		ImageList_Destroy(hImageList);

	tb.iString = 0 ;
	tb.fsState = TBSTATE_ENABLED  ;
	tb.fsStyle = TBSTYLE_BUTTON ;
	for( i = 0; i < (int)iToolBarNum; i ++ )
	{
		tb.iBitmap = i;
		tb.idCommand = IEToolBar[i].ID;
		SendMessage( hToolbar, TB_ADDBUTTONS, 1, (LPARAM)&tb ); // 插入每一个按钮
	}
	lpied->hToolbar = hToolbar ; // 保存句柄
	return TRUE;
}
// **************************************************
// 声明：static void SetButtonImage(HWND hWnd,UINT id,UINT iImage)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN id -- 按钮的ID号
// 	IN iImage  -- 图象ID号
// 
// 返回值：无
// 功能描述：给一个指定ID号的按钮设定一个图象。
// 引用: 
// **************************************************
static void SetButtonImage(HWND hWnd,UINT id,UINT iImage)
{
	//HBITMAP hBitmap;
	HICON hIcon;
	HWND hItem;
	HINSTANCE hInstance;


	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	hItem = GetDlgItem(hWnd,id); // 得到按钮句柄
	hIcon = LoadImage(hInstance,MAKEINTRESOURCE(iImage),IMAGE_ICON,16,16,0); // 装载图象
	hIcon = (HICON)SendMessage(hItem,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hIcon);// 设置图象给按钮

	if (hIcon != NULL)
	{ // 原来有图象
		DestroyIcon(hIcon); // 删除原来的图象
		hIcon = NULL;
	}
}


// **************************************************
// 声明：static LRESULT DoOpen(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：打开文件。
// 引用: 
// **************************************************
static LRESULT DoOpen(HWND hWnd)
{
	TCHAR lpFileName[MAX_PATH];

	lpFileName[0] = 0;

	if (GetFileName(hWnd,lpFileName,OPENHTML)== TRUE) // 得到要打开的文件名
	{
		InitialIExploreWindow( hWnd , lpFileName); // 打开指定的文件
		return TRUE; // 返回成功
	}
	return FALSE;
}
// **************************************************
// 声明：static LRESULT DoSave(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：保存当前打开的网页。
// 引用: 
// **************************************************
static LRESULT DoSave(HWND hWnd)
{
	TCHAR lpFileName[MAX_PATH];
	LPTSTR lpData;
	int iDataLen;
	HANDLE hFile;
	HWND hIEBrowser;
	int iFileType;

	lpFileName[0] = 0;

	hIEBrowser = (HWND )GetDlgItem(hWnd,IDC_VIEW); // 得到IE浏览的窗口句柄
	iFileType = SendMessage(hIEBrowser,HM_GETFILETYPE,0,0); // 得到打开的文件类型
	if (iFileType == HTMLFILE)
	{ // 是一个文件
		if (GetFileName(hWnd,lpFileName,SAVEHTML) == FALSE) // 得到要保存的文件名
		{
			return FALSE;
		}
		if (HaveExtName(lpFileName) == FALSE) // 是否存在扩展名
		{
			AddExtName(lpFileName,"htm"); // 加上默认扩展名
		}
	}
	else
	{ // 是一个图象
		if (GetFileName(hWnd,lpFileName,SAVEIMG) == FALSE) // 得到要保存的文件名
		{
			return FALSE;
		}
		if (HaveExtName(lpFileName) == FALSE) // 是否存在扩展名
		{
			AddExtName(lpFileName,"gif"); // 加上默认扩展名
		}
	}
	iDataLen = SendMessage(hIEBrowser,HM_READDATA,0,0); // 得到网页数据的长度
	if (iDataLen == -1)
		return FALSE;
	lpData= (LPTSTR)malloc(iDataLen * sizeof(TCHAR)); // 分配数据空间
	if (lpData)
	{ // 分配成功
		if (SendMessage(hIEBrowser,HM_READDATA,iDataLen,(LPARAM)lpData) > 0) // 得到数据
		{ // 得到数据成功
			hFile = CreateFile(lpFileName,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_ARCHIVE,NULL); // 打开要保存的文件名
			WriteFile( hFile, lpData, iDataLen,&iDataLen,NULL); // 将数据写入到文件
			CloseHandle(hFile); // 关闭文件
		}
		free(lpData); // 释放数据缓存
	}
	return TRUE;
}
// **************************************************
// 声明：static LRESULT DoEmail(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：将当前打开的网页内容用EMAIL发送出去。
// 引用: 
// **************************************************

static LRESULT DoEmail(HWND hWnd)
{
#if 0	
	EmailFormat stEmailFormat;
	LPTSTR lpData;
	int iDataLen;
	HWND hIEBrowser;
	int iFileType;

	hIEBrowser = (HWND )GetDlgItem(hWnd,IDC_VIEW); // 得到IE浏览的窗口句柄

	iFileType = SendMessage(hIEBrowser,HM_GETFILETYPE,0,0); // 得到文件类型
	if (iFileType == HTMLFILE)
	{ // 是HTML文件
		iDataLen = SendMessage(hIEBrowser,HM_READDATA,0,0); // 得到数据长度
		if (iDataLen == -1)
			return FALSE; // 没有数据
		lpData= (LPTSTR)malloc((iDataLen+1) * sizeof(TCHAR)); // 分配数据缓存
		if (lpData)
		{ // 分配成功
			memset(lpData,0,(iDataLen+1) * sizeof(TCHAR)); // 清空缓存
			if (SendMessage(hIEBrowser,HM_READDATA,iDataLen,(LPARAM)lpData) > 0) // 得到数据
			{ // 得到数据成功
				InitEmailFormat( &stEmailFormat ); // 初始化EMIAL结构
				stEmailFormat.Body = lpData; // 设置EMIAL内容
				stEmailFormat.BodyType = 2;  // html file type
				SendEmailTo( &stEmailFormat ); // 发送邮件
				DeInitEmailFormat ( &stEmailFormat );// lpData will Be Free
			}
			else
			{
				free(lpData); // 释放缓存
			}
		}
		return TRUE;
	}
	else if (iFileType == IMAGEFILE)
	{ // 是图象文件，用附件的方式发送出去
		EmailAttach *pemailAttach; 
		lpData= (LPTSTR)malloc(MAX_PATH * sizeof(TCHAR)); // 分配文件路径缓存
		if (lpData)
		{
			memset(lpData,0,MAX_PATH * sizeof(TCHAR)); // 清空缓存
			if (SendMessage(hIEBrowser,HM_GETLOCATEFILE,MAX_PATH,(LPARAM)lpData) > 0) // 得到图象文件的本地文件名
			{ // 成功
				InitEmailFormat( &stEmailFormat ); // 初始化邮件结构
				pemailAttach = (EmailAttach *)malloc(sizeof(EmailAttach)); // 分配邮件附件的结构
				if (pemailAttach == NULL)
				{ // 分配失败
					DeInitEmailFormat ( &stEmailFormat ); // 释放邮件结构
					return FALSE;
				}
				pemailAttach->Name = lpData; // 添加附件文件名
				stEmailFormat.AttachCount= 1; // 附加文件个数
				stEmailFormat.Attach[0] = pemailAttach; // 设置附件结构
				stEmailFormat.BodyType = 3;  // other type
				SendEmailTo( &stEmailFormat ); // 发送邮件
				DeInitEmailFormat ( &stEmailFormat );	// lpData will Be Free
				//free(pemailAttach);
				//free(lpData);
			}
		}
	}
#endif	
	return FALSE;
}

// **************************************************
// 声明：static LRESULT DoStop(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：停止当前网页。
// 引用: 
// **************************************************
static LRESULT DoStop(HWND hWnd)
{
	HWND hIEBrowser;

	hIEBrowser = (HWND )GetDlgItem(hWnd,IDC_VIEW); // 得到IE浏览窗口句柄
	SendMessage(hIEBrowser,HM_STOP,0,0); // 停止当前网页
	return 0;
}

// **************************************************
// 声明：static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName,WORD OpenStyle)
// 参数：
// 	IN hDlg -- 窗口句柄
// 	OUT lpFileName -- 保存得到的文件名
// 	IN  OpenStyle -- 打开类型
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到文件名。
// 引用: 
// **************************************************
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName,WORD OpenStyle)
{
    TCHAR OpenTitle[256];
	TCHAR szFilter[MAX_PATH]; 	



	OPENFILENAME OpenFileName;
	TCHAR lpstrFilter[30];
	TCHAR strFile[MAX_PATH];
	TCHAR TitleText[10];


	if (OpenStyle==OPENHTML)
	{  // 打开一个HTML文件
//		LoadString(hInst,IDS_OPENTITLE,OpenTitle,MAX_LOADSTRING);
		strcpy(OpenTitle,"打开文件"); // 标题
//		LoadString(hInst,IDS_MP3FILTER,szFilter,MAX_LOADSTRING);
		strcpy(szFilter,"*.htm"); // 可以打开的文件类型

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
		sprintf(lpstrFilter+strlen(szFilter)+1, TEXT("%s"), TEXT("*.*"));   
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_FILEMUSTEXIST; // 支持长文件名和文件必须存在
	}
	else if (OpenStyle==SAVEHTML)
	{  // 保存一个HTML文件
//		LoadString(hInst,IDS_SAVETITLE,OpenTitle,MAX_LOADSTRING);
		strcpy(OpenTitle,"保存文件"); // 标题
//		LoadString(hInst,IDS_PLSFILTER,szFilter,MAX_LOADSTRING);
		strcpy(szFilter,"*.htm"); // 可以打开的文件类型

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
		sprintf(lpstrFilter+strlen(szFilter)+1, TEXT("%s"), TEXT("*.*"));   
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // 支持长文件名和覆盖原来文件
	}
	else if (OpenStyle==SAVEIMG)
	{  // 保存图象
//		LoadString(hInst,IDS_SAVETITLE,OpenTitle,MAX_LOADSTRING);
		strcpy(OpenTitle,"保存文件"); // 标题
//		LoadString(hInst,IDS_PLSFILTER,szFilter,MAX_LOADSTRING);
		strcpy(szFilter,"*.gif"); // 可以打开的文件类型

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // 支持长文件名和覆盖原来文件
	}

//	memset(FileName,0,sizeof(FileName));
	memset(strFile,0,sizeof(strFile)); // 清空文件名缓存
	sprintf(TitleText, TEXT("%s"), OpenTitle); // 得到标题

	OpenFileName.lStructSize=sizeof(OPENFILENAME); 
    OpenFileName.hwndOwner=hDlg;
//	OpenFileName.hInstance=0;
    OpenFileName.lpstrFilter=lpstrFilter; //NULL
//	OpenFileName.lpstrCustomFilter=NULL;
//  OpenFileName.nMaxCustFilter=0; 
	OpenFileName.nFilterIndex=1; 
    OpenFileName.lpstrFile=lpFileName; 
	OpenFileName.nMaxFile=MAX_PATH; 
    OpenFileName.lpstrFileTitle=strFile;  //only filename an extension(withou t path information) 
	OpenFileName.nMaxFileTitle=MAX_PATH; 
    OpenFileName.lpstrInitialDir="\\"; 
    OpenFileName.lpstrTitle=TitleText;   
//    OpenFileName.Flags=OFN_CREATEPROMPT;
	OpenFileName.nFileOffset=0; 
    OpenFileName.nFileExtension=0; 
	OpenFileName.lpstrDefExt=NULL; 
	if (OpenStyle==OPENHTML) 
	{ // 打开文件
		if( GetOpenFileName(&OpenFileName) == FALSE) 
			return FALSE; // 得到文件名失败
	}
	else
	{ // 保存文件
		if( GetSaveFileName(&OpenFileName) == FALSE) 
			return FALSE; // 得到文件名失败
	}
//	_wsplitpath(lpFileName,NULL,curPath,NULL,NULL);
//	_splitpath(lpFileName,NULL,curPath,NULL,NULL);
	return TRUE; // 得到文件名成功
}


// **************************************************
// 声明：static BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// 参数：
// 	IN hRoot -- 根键
// 	IN lpKey -- 要打开的键
// 	IN lpValueName -- 键值名
// 	IN lpData -- 得到数据的缓存
// 	IN dwDataLen -- 数据长度
// 	IN dwType -- 数据类型
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：读取注册表。
// 引用: 
// **************************************************
static BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegOpenKeyEx(hRoot, lpKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) // 打开注册键
	 { // 打开成功
		 if (RegQueryValueEx(hKey, lpValueName, NULL, &dwType, (LPBYTE)lpData, &dwDataLen) == ERROR_SUCCESS) // 读取键值数据
		 { // 读取成功
			RegCloseKey(hKey); // 关闭键
			return TRUE; // 返回成功
		 }
		 RegCloseKey(hKey); // 关闭键
	 }
	 return FALSE; // 返回失败
}
// **************************************************
// 声明：static BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// 参数：
// 	IN hRoot -- 根键
// 	IN lpKey -- 要打开的键
// 	IN lpValueName -- 键值名
// 	IN lpData -- 要写入的数据缓存
// 	IN dwDataLen -- 数据长度
// 	IN dwType -- 数据类型
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：读取注册表。
// 引用: 
// **************************************************
static BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegCreateKeyEx(hRoot, lpKey, 0,NULL,0, KEY_ALL_ACCESS, NULL,&hKey,0) == ERROR_SUCCESS) // 打开键
	 { // 成功
		 if (RegSetValueEx(hKey, lpValueName, NULL, (DWORD)dwType, (LPBYTE)lpData, dwDataLen) == ERROR_SUCCESS) // 设置键值数据
		 { // 设置成功
			 RegCloseKey(hKey);
			 return TRUE; // 返回成功
		 }
		 RegCloseKey(hKey);
	 }
	 return FALSE; // 返回失败
}


// !!! Add By Jami chen in 2003.09.12
// **************************************************
// 声明：static BOOL HaveExtName(LPTSTR lpFileName)
// 参数：
// 	IN lpFileName -- 指定的文件名
// 
// 返回值：有扩展名返回TRUE，否则返回FALSE
// 功能描述：得到指定文件是否存在扩展名。
// 引用: 
// **************************************************
static BOOL HaveExtName(LPTSTR lpFileName)
{
	LPTSTR lpCurPos;

	lpCurPos = lpFileName; // 得到文件名头指针
	while(*lpCurPos)
	{
		if (*lpCurPos == '.') // 是否存在扩展名分隔符
			return TRUE; // 有扩展名
		lpCurPos ++;
	}
	return FALSE; // 没有扩展名
}
// **************************************************
// 声明：static void AddExtName(LPTSTR lpFileName,LPTSTR lpExtName)
// 参数：
// 	IN lpFileName -- 指定文件名
// 	IN lpExtName -- 指定扩展名
// 
// 返回值：无
// 功能描述：在指定文件上加入一个扩展名。
// 引用: 
// **************************************************
static void AddExtName(LPTSTR lpFileName,LPTSTR lpExtName)
{
	if ((strlen(lpFileName) + strlen(lpExtName) + 1)  >= MAX_PATH) // 得到是否可以假如扩展名
		return ;
	strcat(lpFileName,"."); // 添加扩展名分隔符
	strcat(lpFileName,lpExtName); // 添加扩展名
	return ;
}
// !!! Add End By Jami chen in 2003.09.12