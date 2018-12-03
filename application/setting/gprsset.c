/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：笔针属性，用户界面部分
版本号：1.0.0
开发时期：2003-06-25
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_GPRSSet{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_GPRSSet = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,120,0,0,"GPRS设置" },
    };

static LPTSTR strAPNClassName = "APNCLASS";

#define IDC_TAB		300

#define IDC_APN  400

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET GPRSSetSheet[] = {
	{"APN","APNCLASS",IDC_APN},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK GPRSSetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGPRSSetOK(HWND hDlg);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

static ATOM RegisterAPNWindow(HINSTANCE hInstance);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoGPRSSet(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：GPRS设置入口函数。
// 引用: 
// **************************************************
void DoGPRSSet(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		// 创建GPRS设置对话框
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_GPRSSet,hWnd, (DLGPROC)GPRSSetProc);
}

// ********************************************************************
// 声明：static LRESULT CALLBACK GPRSSetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：GPRS设置对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK GPRSSetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_OK: // OK消息
			DoGPRSSetOK(hDlg);
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
// 返回值：成功返回0，否则返回-1
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

		RegisterAPNWindow(hInstance); // 注册APN窗口类

		numItem = sizeof(GPRSSetSheet) / sizeof(TABSHEET); // 得到TAB条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = GPRSSetSheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入TAB条目
			CreateWindow(GPRSSetSheet[i].lpClassName,"",WS_CHILD,0,27,210,73,hDlg,(HMENU)GPRSSetSheet[i].id,hInstance,0); // 创建对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 当前没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前条目
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,GPRSSetSheet[iIndex].id),SW_SHOW); // 显示当前对应的窗口
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
					ShowWindow(GetDlgItem(hWnd,GPRSSetSheet[iIndex].id),SW_HIDE); // 隐藏对应的窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,GPRSSetSheet[iIndex].id),SW_SHOW); // 显示对应的窗口
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoGPRSSetOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：GPRS设置WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoGPRSSetOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(GPRSSetSheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,GPRSSetSheet[i].id); // 得到条目对应的窗口句柄
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送OK消息
		}
		return TRUE;
}


/****************************************************************************/
//  “APN” 界面
/****************************************************************************/

#define IDC_CMNET  102
#define CMNET_MAXLEN	32

static LRESULT CALLBACK APNWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoAPNCreate(HWND hWnd);
static LRESULT DoAPNSettingOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterAPNWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册APN窗口类。
// 引用: 
// **************************************************
static ATOM RegisterAPNWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strAPNClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)APNWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK APNWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：GPRS设置APN对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK APNWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"APN",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
//			switch (wmId)
//			{
//			}	
//			break;
		case WM_CREATE: // 创建消息
			return DoAPNCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoAPNSettingOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoAPNCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建APN属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoAPNCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	TCHAR lpCMNet[CMNET_MAXLEN];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	// 从注册表得到APN的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\GPRS", "APN",lpCMNet, strlen(lpCMNet)+1,REG_SZ) == FALSE)
	{
		strcpy(lpCMNet,"cmnet");
	}
	// 创建输入APN的窗口
	CreateWindow(classSTATIC,  "请输入APN(可以向您的网络服务供应商咨询)",WS_CHILD | WS_VISIBLE ,5,5,200,40,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,lpCMNet,WS_CHILD | WS_VISIBLE ,5,45,200,20,hWnd,(HMENU)IDC_CMNET,hInstance,0);

	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoAPNSettingOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：GPRS的APN设置WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoAPNSettingOK(HWND hDlg)
{
	TCHAR lpCMNet[CMNET_MAXLEN];
	HWND hCMNet;

		hCMNet = GetDlgItem(hDlg,IDC_CMNET); // 得到APN的窗口句柄

		GetWindowText(hCMNet, lpCMNet, CMNET_MAXLEN); // 得到当前的APN
		// 将当前的APN写入注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\GPRS", "APN",lpCMNet, strlen(lpCMNet)+1,REG_SZ);
		return TRUE;
}

