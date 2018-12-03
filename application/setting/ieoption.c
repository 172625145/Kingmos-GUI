/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：IE选项，用户界面部分
版本号：1.0.0
开发时期：2003-06-25
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"

#define STR_IEOPTION  "IE选项"

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_IEOption{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_IEOption = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,110,0,0,"IE选项" },
    };

static LPTSTR strIEGeneralClassName = "IEGENERALCLASS";
static LPTSTR strIELinkClassName = "IELINKCLASS";

#define IDC_TAB		300

#define IDC_IEGENERAL  400
#define IDC_IELINK     401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET IEOptionSheet[] = {
	{"常规","IEGENERALCLASS",IDC_IEGENERAL},
//	{"连接","IELINKCLASS",IDC_IELINK},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK IEOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoIEOptionOK(HWND hDlg);

ATOM RegisterIEGeneralWindow(HINSTANCE hInstance);
ATOM RegisterIELinkWindow(HINSTANCE hInstance);


extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoIEOption(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：IE选项入口函数
// 引用: 
// **************************************************
void DoIEOption(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

	HWND hDlg;

		hDlg = FindWindow( NULL, STR_IEOPTION ) ;  // 查找窗口是否已经打开
		if( hDlg != 0 )
		{ // 已经打开
			SetForegroundWindow( hDlg ) ; // 设置到前台
			return ;
		}
//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_IEOption,hWnd, (DLGPROC)IEOptionProc); // 创建IE选项对话框
}

// ********************************************************************
// 声明：static LRESULT CALLBACK IEOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：IE选项对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK IEOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
/*			switch (wmId)
			{
				case IDC_OK:
					EndDialog(hDlg,TRUE);
					return 0;
			}	
			*/
			return 0;
		case WM_NOTIFY: // 通知消息
			return DoNotify(hDlg,wParam,lParam);
		case WM_INITDIALOG: // 初始化对话框
			return DoInitDialog(hDlg);
		case WM_CLOSE: // 关闭消息
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK 消息
			DoIEOptionOK(hDlg);
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}

// **************************************************
// 声明：static LRESULT DoInitDialog(HWND hDlg)
// 参数：
// 	IN hDlg -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：初始化对话框。
// 引用: 
// **************************************************
static LRESULT DoInitDialog(HWND hDlg)
{
	HINSTANCE hInstance;
	HWND hTab;
	TCITEM tcItem;
	int numItem;
	int i,iIndex;
	CTLCOLORSTRUCT stCtlColor;

		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // 得到实例句柄
		// 创建TAB窗口
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);
		// 设置颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterIEGeneralWindow(hInstance); // 注册IE通用窗口类
		RegisterIELinkWindow(hInstance); // 注册IE链接窗口类

		numItem = sizeof(IEOptionSheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = IEOptionSheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入TAB条目
			CreateWindow(IEOptionSheet[i].lpClassName,"",WS_CHILD,0,27,210,90,hDlg,(HMENU)IEOptionSheet[i].id,hInstance,0); // 创建对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前选择
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,IEOptionSheet[iIndex].id),SW_SHOW); // 显示对应的窗口
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：返回消息处理结果
// 功能描述：处理通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	NMHDR   *hdr=(NMHDR   *)lParam;    
	int iIndex;
	HWND hTab;

		hTab = GetDlgItem(hWnd,IDC_TAB); // 得到TAB窗口句柄
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // 选择正在改变，改变之前
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,IEOptionSheet[iIndex].id),SW_HIDE); // 隐藏对应窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,IEOptionSheet[iIndex].id),SW_SHOW); // 显示对应窗口
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoIEOptionOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：IE选项WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoIEOptionOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(IEOptionSheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,IEOptionSheet[i].id); // 得到条目对应的窗口句柄
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送OK消息
		}
		return TRUE;
}


/****************************************************************************/
//  “常规” 界面
/****************************************************************************/
#define IDC_HOMEPAGE  101
#define HOMEPAGE_MAXLEN 32

static LRESULT CALLBACK IEGeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoIEGeneralCreate(HWND hWnd);
static LRESULT DoIEGeneralOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterIEGeneralWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册IE通用窗口。
// 引用: 
// **************************************************
static ATOM RegisterIEGeneralWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strIEGeneralClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)IEGeneralWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK IEGeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：IE选项常规对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK IEGeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"IEGeneral",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoIEGeneralCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoIEGeneralOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoIEGeneralCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建IE常规属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoIEGeneralCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	TCHAR lpHomePage[HOMEPAGE_MAXLEN];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄


	// 读取注册表IE的主页地址
	if (regReadData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\IExplorer", "Home Page",lpHomePage, HOMEPAGE_MAXLEN,REG_SZ) == FALSE)
	{ // 没有找到
//		strcpy(lpHomePage,"www.mlg.com");
		strcpy(lpHomePage,"\\kingmos\\kingmos.htm"); // 设置系统的网页为主页
	}

	// 主页
	CreateWindow(classSTATIC,  "主页:",WS_CHILD | WS_VISIBLE ,5,15,42,18,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,  lpHomePage ,WS_CHILD | WS_VISIBLE ,50,15,150,18,hWnd,(HMENU)IDC_HOMEPAGE,hInstance,0);

	// 搜索
//	CreateWindow(classSTATIC,  "搜索:",WS_CHILD | WS_VISIBLE ,5,30,42,18,hWnd,(HMENU)0xffff,hInstance,0);
//	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,  "www.google.com",WS_CHILD | WS_VISIBLE | ES_NUMBER,50,30,150,18,hWnd,(HMENU)102,hInstance,0);

	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoIEGeneralOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：IE常规WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoIEGeneralOK(HWND hDlg)
{
	TCHAR lpHomePage[HOMEPAGE_MAXLEN];
	HWND hHomepage;

		hHomepage = GetDlgItem(hDlg,IDC_HOMEPAGE); // 得到输入主页的窗口

		GetWindowText(hHomepage , lpHomePage, HOMEPAGE_MAXLEN); // 得到主页
		// 写入注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\IExplorer", "Home Page",lpHomePage, strlen(lpHomePage)+1,REG_SZ);
		return TRUE;
}


/****************************************************************************/
//  “连接” 界面
/****************************************************************************/
static LRESULT CALLBACK IELinkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoIELinkOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterIELinkWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册IE连接窗口类。
// 引用: 
// **************************************************
static ATOM RegisterIELinkWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strIELinkClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)IELinkWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK IELinkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：IE选项连接对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK IELinkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			TextOut(hdc,10,10,"IELink",6);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			break;
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoIELinkOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoIELinkOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：IE连接WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoIELinkOK(HWND hDlg)
{
		return TRUE;
}
