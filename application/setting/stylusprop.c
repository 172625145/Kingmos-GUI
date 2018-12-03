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
const struct dlgIDD_StylusProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_StylusProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,120,0,0,"笔针属性" },
    };

static LPTSTR strCalibrateClassName = "CALIBRATECLASS";

#define IDC_TAB		300

#define IDC_CALIBRATE  400

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET StylusPropertySheet[] = {
	{"校准","CALIBRATECLASS",IDC_CALIBRATE},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK StylusPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

static ATOM RegisterCalibrateWindow(HINSTANCE hInstance);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoStylusProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：笔针属性入口函数。
// 引用: 
// **************************************************
void DoStylusProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		// 创建笔针属性对话框
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_StylusProperty,hWnd, (DLGPROC)StylusPropertyProc);
}

// ********************************************************************
// 声明：static LRESULT CALLBACK StylusPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：笔针属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK StylusPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_INITDIALOG: // 初始化对话框消息
			return DoInitDialog(hDlg);
		case WM_CLOSE: // 关闭消息
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK消息
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
// 返回值：
// 功能描述：
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

		RegisterCalibrateWindow(hInstance); // 注册画面校正窗口类

		numItem = sizeof(StylusPropertySheet) / sizeof(TABSHEET); // 得到TAB条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = StylusPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入TAB条目
			CreateWindow(StylusPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,73,hDlg,(HMENU)StylusPropertySheet[i].id,hInstance,0); // 创建对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当年选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前选择
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,StylusPropertySheet[iIndex].id),SW_SHOW); // 显示条目对应的窗口
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
					ShowWindow(GetDlgItem(hWnd,StylusPropertySheet[iIndex].id),SW_HIDE); // 隐藏对应的窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,StylusPropertySheet[iIndex].id),SW_SHOW); // 显示对应窗口
				 }
				 return 0;
		}
		return 0;
}


/****************************************************************************/
//  “校准” 界面
/****************************************************************************/

#define ID_CALIBRATE  102

static LRESULT CALLBACK CalibrateWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoCalibrateCreate(HWND hWnd);

// **************************************************
// 声明：static ATOM RegisterCalibrateWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值： 返回系统注册结果
// 功能描述：注册画面校正窗口类。
// 引用: 
// **************************************************
static ATOM RegisterCalibrateWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strCalibrateClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)CalibrateWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK CalibrateWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：笔针属性校准对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK CalibrateWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Calibrate",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_CALIBRATE: // 校正命令
					//LoadApplication("TouchCal",NULL);
					RETAILMSG(1,("Will Call TouchCalibrate \r\n"));
					TouchCalibrate(); // 校正当前画面
					RETAILMSG(1,("TouchCalibrate Complete\r\n"));
					return 0;
			}	
			break;
		case WM_CREATE: // 创建消息
			return DoCalibrateCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoCalibrateCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建校准属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoCalibrateCreate(HWND hWnd)
{
	HINSTANCE hInstance;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

	// 创建提示信息
	CreateWindow(classSTATIC,  "如设备没有正常回应您的点击动作,可能需要校准屏幕.",WS_CHILD | WS_VISIBLE ,5,5,200,40,hWnd,(HMENU)0xffff,hInstance,0);
	// 创建校正按钮
	CreateWindow(classBUTTON,  "校准",WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,75,45,50,20,hWnd,(HMENU)ID_CALIBRATE,hInstance,0);

	return 0;
}

