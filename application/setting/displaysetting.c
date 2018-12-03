/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：显示属性，用户界面部分
版本号：1.0.0
开发时期：2003-06-25
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <SliderCtrl.h>

/*****************************************************/
// 字串定义区
/*****************************************************/
#define STR_DISPLAYPROC			"显示属性"
#define STR_BRIGHT				"亮度"
#define STR_BACKLIGHT			"背光"
#define STR_ADJUST				"移动手柄,可进行亮度调整"
#define STR_DARK				"暗"
#define STR_LIGHT				"亮"
#define STR_LIGHTOFF_BATTERY	"使用电池时自动关闭背光"
#define STR_MINUTE_1			"1 分钟"
#define STR_MINUTE_3			"3 分钟"
#define STR_MINUTE_5			"5 分钟"
#define STR_MINUTE_10			"10 分钟"
#define STR_MINUTE_30			"30 分钟"
#define STR_LIGHTOFF_UNUSED		"系统闲置          后关闭"
#define STR_LIGHTOFF_EXTERN		"使用外部电源时关闭背光"
/*****************************************************/

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_DisplayProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_DisplayProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,160,0,0,STR_DISPLAYPROC },
    };

static LPTSTR strBrightClassName = "BRIGHTCLASS";
static LPTSTR strBackLightClassName = "BACKLIGHTCLASS";

#define IDC_TAB		300

#define IDC_BRIGHT  400
#define IDC_BACKLIGHT  401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET DisplayPropertySheet[] = {
	{STR_BRIGHT,"BRIGHTCLASS",IDC_BRIGHT},
	{STR_BACKLIGHT,"BACKLIGHTCLASS",IDC_BACKLIGHT},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK DisplayPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDisplayPropertyOK(HWND hDlg);

ATOM RegisterBrightWindow(HINSTANCE hInstance);
ATOM RegisterBackLightWindow(HINSTANCE hInstance);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoDisplayProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：显示属性入口函数。
// 引用: 
// **************************************************
void DoDisplayProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_DisplayProperty,hWnd, (DLGPROC)DisplayPropertyProc); // 创建对话框
}

// ********************************************************************
// 声明：static LRESULT CALLBACK DisplayPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：显示属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK DisplayPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_CLOSE: // 关闭窗口消息
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK消息
			DoDisplayPropertyOK(hDlg);
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

		RegisterBrightWindow(hInstance);  // 注册亮度窗口类
		RegisterBackLightWindow(hInstance); // 注册背光窗口类

		numItem = sizeof(DisplayPropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = DisplayPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入一个TAB条目
			CreateWindow(DisplayPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,100,hDlg,(HMENU)DisplayPropertySheet[i].id,hInstance,0); // 创建一个与条目相关的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0);  // 设置第一个条目为选择条目
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,DisplayPropertySheet[iIndex].id),SW_SHOW); // 显示条目对应的窗口
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
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到选择条目索引
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,DisplayPropertySheet[iIndex].id),SW_HIDE); // 隐藏条目对应窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到选择条目索引
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,DisplayPropertySheet[iIndex].id),SW_SHOW); // 显示条目对应窗口
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoDisplayPropertyOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：显示属性WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoDisplayPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(DisplayPropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,DisplayPropertySheet[i].id); // 得到条目对应的窗口
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送WM_OK消息
		}
		return TRUE;
}


/****************************************************************************/
//  “亮度” 界面
/****************************************************************************/
#define IDC_SLIDER   201

static LRESULT CALLBACK BrightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoPaintBright(HWND hWnd,HDC hdc);
static LRESULT DoBrightCreate(HWND hWnd);
static LRESULT DoBrightOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterBrightWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册亮度窗口类。
// 引用: 
// **************************************************
static ATOM RegisterBrightWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strBrightClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)BrightWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK BrightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：显示属性亮度对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK BrightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;
//	HWND hSlider;
//	HICON hIcon;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Bright",6);
			DoPaintBright(hWnd,hdc);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoBrightCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoBrightOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// **************************************************
// 声明：static void DoPaintBright(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述：绘制亮度窗口。
// 引用: 
// **************************************************
static void DoPaintBright(HWND hWnd,HDC hdc)
{
/*
	HPEN hPen;
	int i;

		SetBkMode( hdc, TRANSPARENT ) ;
		TextOut(hdc,10,13,"移动手柄,可进行亮度调整",23);
		TextOut(hdc,14,48,"暗",2);
		TextOut(hdc,170,48,"亮",2);
*/
/*		Rectangle(hdc,10,43,180,63);
		for (i=80;i<=255;i++)
		{
			hPen = CreatePen(PS_SOLID,5,RGB(i,i,i));
			hPen = (HPEN)SelectObject(hdc,hPen);
			MoveToEx(hdc,(i-80)+11,45,NULL);
			LineTo(hdc,(i-80)+11,62);
			hPen = (HPEN)SelectObject(hdc,hPen);
			DeleteObject(hPen);
		}
*/
}
// ********************************************************************
// 声明：static LRESULT DoBrightCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建亮度属性窗口
// 引用: 
// ********************************************************************
static LRESULT DoBrightCreate(HWND hWnd)
{
	HWND hSlider;
	HICON hIcon;
	HINSTANCE hInstance;
	int iBrightLevel = 0 ,iBrightLevelNum = 0;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		// 创建亮度滚动条
		hSlider = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|WS_VISIBLE | TBS_AUTOTICKS |TBS_HORZ |TBS_TOP,30,45,140,50,hWnd,(HMENU)IDC_SLIDER,hInstance,0);
		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_ARROW ), IMAGE_ICON,16, 16, 0 ) ;
		SendMessage(hSlider,TBM_SETTHUMBICON,MAKELONG(16,16),(LPARAM)hIcon);

		// Get Bright level Num
		iBrightLevelNum = 7;
		SendMessage(hSlider,TBM_SETRANGE,FALSE,MAKELONG(0,iBrightLevelNum -1));

		// GetCurrent Bright Level

		if (regReadData(HKEY_SOFTWARE_ROOT,
				"Display\\Bright", "Bright",&iBrightLevel, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iBrightLevel = 3;
		}
		SendMessage(hSlider,TBM_SETPOS,TRUE,iBrightLevel); // 设置当前的亮度

		SendMessage(hSlider,TBM_SETPAGESIZE,0,1); // 设置一个PAGE的大小
		
		// 创建标记窗口
		CreateWindow(classSTATIC,STR_ADJUST,WS_CHILD | WS_VISIBLE ,10,13,190,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,STR_DARK,WS_CHILD | WS_VISIBLE ,14,48,18,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,STR_LIGHT,WS_CHILD | WS_VISIBLE ,170,48,18,18,hWnd,(HMENU)0xffff,hInstance,0);

		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoBrightOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：显示属性亮度WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoBrightOK(HWND hDlg)
{
	HWND hSlider;
	int iBrightLevel = 0;

		hSlider = GetDlgItem(hDlg,IDC_SLIDER); // 得到滑块的窗口句柄
		if (hSlider)
		{
			iBrightLevel = SendMessage(hSlider,TBM_GETPOS,0,0); // 得到当前的亮度

			// Set Bright
			regWriteData(HKEY_SOFTWARE_ROOT,
					"Display\\Bright", "Bright",&iBrightLevel, sizeof(DWORD),REG_DWORD);
		}
		return TRUE;
}


/****************************************************************************/
//  “背光” 界面
/****************************************************************************/
#define IDC_BATTERYENABLE	101
#define IDC_BATTERYTIME		102
#define IDC_EXTERNALENABLE	103
#define IDC_EXTERNALTIME	104

static LRESULT CALLBACK BackLightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoBackLightCreate(HWND hWnd);
static LRESULT DoBackLightOK(HWND hDlg);
static LRESULT DoBatteryEnable(HWND hWnd);
static LRESULT DoExternalEnable(HWND hWnd);

// **************************************************
// 声明：static ATOM RegisterBackLightWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册背光窗口类。
// 引用: 
// **************************************************
static ATOM RegisterBackLightWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strBackLightClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)BackLightWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK BackLightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：显示属性背光对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK BackLightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"BackLight",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_BATTERYENABLE: // 使用电池的状态改变
					DoBatteryEnable(hWnd);
					return 0;
				case IDC_EXTERNALENABLE: // 使用外部电源的状态改变
					DoExternalEnable(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // 创建消息
			return DoBackLightCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			DoBackLightOK(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoBackLightCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建背光窗口
// 引用: 
// ********************************************************************
static LRESULT DoBackLightCreate(HWND hWnd)
{
	HWND hChild;
	HINSTANCE hInstance;
	CTLCOLORSTRUCT             CCS ;
	BOOL bEnable;
	int iIndex;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

//		bEnable  = GetAutoBackLightOff(USEBATTERY);
		// 得到使用电池自动关闭的状态
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		hChild = CreateWindow(classBUTTON,  STR_LIGHTOFF_BATTERY,WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,5,201,18,hWnd,(HMENU)IDC_BATTERYENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0); // 设置状态
		// 创建选择使用电池关闭系统的时间的窗口
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,30,80,68,hWnd,(HMENU)IDC_BATTERYTIME,hInstance,0);
		// 设置颜色
		CCS.fMask = CLF_TEXTBKCOLOR;//|CLF_TITLECOLOR;

		CCS.cl_TextBk = RGB( 255,255,255);
//		CCS.cl_Title = RGB( 132,132,132);
//		CCS.cl_Title = RGB( 0,0,0);
		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		// 插入选择项
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_1);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_3);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_5);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_10);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_30);
		// 得到注册表的时间索引
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // 设置当前选择
		EnableWindow(hChild,bEnable);

		// 创建背光关闭的窗口
		hChild = CreateWindow(classSTATIC,STR_LIGHTOFF_UNUSED,WS_CHILD | WS_VISIBLE ,5,30,201,18,hWnd,(HMENU)0xffff,hInstance,0);

//		bEnable  = GetAutoBackLightOff(USEEXTERNAL);
		// 读取背光关闭的状态
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		hChild = CreateWindow(classBUTTON,  STR_LIGHTOFF_EXTERN,WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,55,201,18,hWnd,(HMENU)IDC_EXTERNALENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0);

		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,80,80,68,hWnd,(HMENU)IDC_EXTERNALTIME,hInstance,0);

		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		// 设置时间的选项
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_1);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_3);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_5);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_10);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_30);
		// 读取当前的时间设置
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // 设置当前的选择
		EnableWindow(hChild,bEnable);

		hChild = CreateWindow(classSTATIC,STR_LIGHTOFF_UNUSED,WS_CHILD | WS_VISIBLE ,5,80,201,18,hWnd,(HMENU)0xffff,hInstance,0);

		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoBackLightOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：显示属性背光WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoBackLightOK(HWND hDlg)
{
	BOOL bEnable;
	HWND hChild ;
	DWORD iIndex ;

		hChild = GetDlgItem(hDlg,IDC_BATTERYENABLE); // 得到背光状态的窗口
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // 得到状态
		// 写入注册表当前的状态
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{
			hChild = GetDlgItem(hDlg,IDC_BATTERYTIME); // 得到时间的窗口句柄
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0); // 得到当前的时间选择
			// 写入注册表表当前的时间设置
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Display\\Back light", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}


		hChild = GetDlgItem(hDlg,IDC_EXTERNALENABLE); // 得到使用外部状态的窗口
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // 得到当前状态
		// 写入当前的状态到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{
			hChild = GetDlgItem(hDlg,IDC_EXTERNALTIME); // 得到使用外部电源的时间的窗口句柄
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0); // 得到当前的时间选择
			// 写入当前的时间选择到注册表
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Display\\Back light", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}

		return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoBatteryEnable(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：背光处理 BATTERYENABLE 功能改变
// 引用: 
// ********************************************************************
static LRESULT DoBatteryEnable(HWND hWnd)
{
	HWND hBatteryEnable ;
	BOOL bEnable;
	HWND hComboBox;

	hBatteryEnable = GetDlgItem(hWnd,IDC_BATTERYENABLE); // 得到使用电池的状态窗口
	bEnable = SendMessage(hBatteryEnable,BM_GETCHECK,0,0); // 得到状态
	hComboBox = GetDlgItem(hWnd,IDC_BATTERYTIME); // 得到选择时间的窗口
	EnableWindow(hComboBox,bEnable); // 设置状态
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoExternalEnable(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：背光处理 EXTERNALENABLE 功能改变
// 引用: 
// ********************************************************************
static LRESULT DoExternalEnable(HWND hWnd)
{
	HWND hExternalEnable ;
	BOOL bEnable;
	HWND hComboBox;

	hExternalEnable = GetDlgItem(hWnd,IDC_EXTERNALENABLE); // 得到外部使用外部电源的状态窗口
	bEnable = SendMessage(hExternalEnable,BM_GETCHECK,0,0); // 得到状态
	hComboBox = GetDlgItem(hWnd,IDC_EXTERNALTIME); // 得到选择时间的窗口
	EnableWindow(hComboBox,bEnable); // 设置状态
	return 0;
}
