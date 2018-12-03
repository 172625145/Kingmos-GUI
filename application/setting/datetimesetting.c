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
#include "eDateSel.h"
#include "ecomdate.h"

/*****************************************************/
// 字串定义区
/*****************************************************/
#define	STR_DATETIMEPROC	"日期时间属性"
#define STR_DATETIME		"日期/时间"
#define STR_TIME			"时间:"
/*****************************************************/

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_DateTimeProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_DateTimeProperty = {
    { 
		WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,40,210,240,0,0,STR_DATETIMEPROC },
    };

static LPTSTR strDateTimeClassName = "DATETIMESETCLASS";

#define IDC_TAB		300

#define IDC_DATETIME  400

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET DateTimePropertySheet[] = {
	{STR_DATETIME,"DATETIMESETCLASS",IDC_DATETIME},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK DateTimePropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoDateTimeOK(HWND hDlg);

static ATOM RegisterDateTimeWindow(HINSTANCE hInstance);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoDateTimeProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：日期/时间属性入口函数。
// 引用: 
// **************************************************
void DoDateTimeProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;
	HWND hDlg;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		hDlg = FindWindow( NULL, STR_DATETIMEPROC ) ;  // 查找窗口是否已经打开
		if( hDlg != 0 )
		{ // 已经打开
			SetForegroundWindow( hDlg ) ; // 设置到前台
			return ;
		}
		// 创建日期时间属性对话框
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_DateTimeProperty,hWnd, (DLGPROC)DateTimePropertyProc);
}

// ********************************************************************
// 声明：static LRESULT CALLBACK DateTimePropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：日期时间属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK DateTimePropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			DoDateTimeOK(hDlg);
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

		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // 得到窗口实例句柄
		// 创建TAB窗口
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);

		// 设置颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterDateTimeWindow(hInstance); // 注册日期时间窗口类

		numItem = sizeof(DateTimePropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = DateTimePropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入一个条目
			CreateWindow(DateTimePropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,193,hDlg,(HMENU)DateTimePropertySheet[i].id,hInstance,0); // 创建对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前选择
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,DateTimePropertySheet[iIndex].id),SW_SHOW); // 显示对应的窗口
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 参数消息
// 	IN lParam -- 参数消息
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
					ShowWindow(GetDlgItem(hWnd,DateTimePropertySheet[iIndex].id),SW_HIDE); // 隐藏对应的窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,DateTimePropertySheet[iIndex].id),SW_SHOW); // 显示对应的窗口
				 }
				 return 0;
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoDateTimeOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：日期时间属性WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoDateTimeOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(DateTimePropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,DateTimePropertySheet[i].id); // 得到条目对应的窗口句柄
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送OK消息
		}
		return TRUE;
}


/****************************************************************************/
//  “日期时间” 界面
/****************************************************************************/
#define IDC_TIME  200
#define IDC_DATE  201

static LRESULT CALLBACK DateTimeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoDateTimeCreate(HWND hWnd);
static LRESULT DoSetDateTimeOK(HWND hWnd);

// **************************************************
// 声明：static ATOM RegisterDateTimeWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册日期时间窗口类。
// 引用: 
// **************************************************
static ATOM RegisterDateTimeWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strDateTimeClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)DateTimeWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK DateTimeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：日期时间属性日期时间对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK DateTimeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:// 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"date time",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoDateTimeCreate(hWnd);

		case WM_OK: // OK消息
			return DoSetDateTimeOK(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoDateTimeCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建调整日期/时间对话框
// 引用: 
// ********************************************************************
static LRESULT DoDateTimeCreate(HWND hWnd)
{
   HINSTANCE hInstance;
   HWND hChild;
   SYSTEMTIME systemTime;
   CTLCOLORSTRUCT stCtlColor;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		// 创建一个日期选择控制
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classDATESEL,"",WS_CHILD|WS_VISIBLE,15,8,
			180,140,hWnd,(HMENU)IDC_DATE,hInstance,NULL);
		
		// 设置颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_TITLECOLOR | CLF_TITLEBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(255,255,255);
		stCtlColor.cl_Title = RGB(50,50,50);
		stCtlColor.cl_TitleBk = RGB(133,166,255);//RGB(255,255,255);
		stCtlColor.cl_Selection = RGB(255,255,255);
		stCtlColor.cl_SelectionBk = RGB(77,166,255);
		SendMessage(hChild,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		GetLocalTime(&systemTime); // 得到当前时间

		SendMessage(hChild,DSM_SETDATETIME,0,(LPARAM)&systemTime); // 设置当前时间
		// 创建时间标记
		CreateWindow(classSTATIC,STR_TIME,WS_CHILD | WS_VISIBLE ,10,159,40,20,hWnd,(HMENU)0xffff,hInstance,0);
		// 创建时间编辑控制
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classEDITTIME,"",WS_CHILD|WS_VISIBLE|ET_RUNCLOCK,50,159,
			100,20,hWnd,(HMENU)IDC_TIME,hInstance,NULL);		
		// 设置时间控制的颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(255,255,255);
		stCtlColor.cl_Selection = RGB(255,255,255);
		stCtlColor.cl_SelectionBk = RGB(77,166,255);
		SendMessage(hChild,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);


		SendMessage(hChild,ETM_SETTIME,0,(LPARAM)&systemTime); // 设置当前时间


		return 0;
}

// **************************************************
// 声明：static LRESULT DoSetDateTimeOK(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值： 无效
// 功能描述：处理日期时间OK消息。
// 引用: 
// **************************************************
static LRESULT DoSetDateTimeOK(HWND hWnd)
{
   SYSTEMTIME systemTime;
   SYSTEMTIME NewTime;
   HWND hChild;


		hChild = GetDlgItem(hWnd,IDC_DATE); // 得到日期窗口句柄
		SendMessage(hChild,DSM_GETDATETIME,0,(LPARAM)&NewTime); // 得到当前日期

		hChild = GetDlgItem(hWnd,IDC_TIME); // 得到时间窗口句柄
		SendMessage(hChild,ETM_GETTIME,0,(LPARAM)&systemTime); // 得到当前时间

		NewTime.wHour = systemTime.wHour;
		NewTime.wMinute = systemTime.wMinute;
		SetLocalTime(&NewTime); // 设置新的当前时间
		return 0;
}
