/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：电源属性，用户界面部分
版本号：1.0.0
开发时期：2003-06-25
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <eprogres.h>


#define STR_POWERPROPERTY	"电源属性"
/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_PowerProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_PowerProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,160,0,0,STR_POWERPROPERTY },
    };

static LPTSTR strBatteryClassName = "POWERBATTERYCLASS";
static LPTSTR strPowerOffClassName = "POWEROFFCLASS";

#define IDC_TAB		300

#define IDC_BATTERY    400
#define IDC_POWEROFF   401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET PowerPropertySheet[] = {
	{"电池","POWERBATTERYCLASS",IDC_BATTERY},
	{"关机","POWEROFFCLASS",IDC_POWEROFF},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK PowerPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPowerPropertyOK(HWND hDlg);

static ATOM RegisterBatteryWindow(HINSTANCE hInstance);
static ATOM RegisterPowerOffWindow(HINSTANCE hInstance);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoPowerProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
void DoPowerProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;
	HWND hDlg;

		hDlg = FindWindow( NULL, STR_POWERPROPERTY ) ;  // 查找当前窗口是否已经存在
		if( hDlg != 0 )
		{ // 已经存在
			SetForegroundWindow( hDlg ) ; // 设置窗口到前台
			return ;
		}

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_PowerProperty,hWnd, (DLGPROC)PowerPropertyProc); // 创建对话框
}

// ********************************************************************
// 声明：static LRESULT CALLBACK PowerPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：电源属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK PowerPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			DoPowerPropertyOK(hDlg);
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
// 返回值：成功返回TRUE，否则返回FALSE。
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

		RegisterBatteryWindow(hInstance); // 注册电池窗口
		RegisterPowerOffWindow(hInstance); // 注册关机窗口

		numItem = sizeof(PowerPropertySheet) / sizeof(TABSHEET); // 得到条目数目
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = PowerPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入一个TAB条目
			CreateWindow(PowerPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,113,hDlg,(HMENU)PowerPropertySheet[i].id,hInstance,0); // 创建一个对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前选择
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,PowerPropertySheet[iIndex].id),SW_SHOW); // 显示对应的窗口
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

		hTab = GetDlgItem(hWnd,IDC_TAB);
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // 选择正在改变，改变之前
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前的选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,PowerPropertySheet[iIndex].id),SW_HIDE); // 隐藏对应的窗口
				 }
				 return 0;
			 case TCN_SELCHANGE:// 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,PowerPropertySheet[iIndex].id),SW_SHOW); // 显示当前对应窗口
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoPowerPropertyOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：电源属性WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoPowerPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(PowerPropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,PowerPropertySheet[i].id); // 得到条目窗口句柄
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送WM_OK消息
		}
		return TRUE;
}


/****************************************************************************/
//  “电池” 界面
/****************************************************************************/
static LRESULT CALLBACK BatteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoBatteryCreate(HWND hWnd);
static LRESULT DoBatteryOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterBatteryWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册电池窗口类。
// 引用: 
// **************************************************
static ATOM RegisterBatteryWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strBatteryClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)BatteryWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK BatteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：电源属性电池对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK BatteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Battery",6);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoBatteryCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoBatteryOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoBatteryCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建电池属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoBatteryCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hProgress;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	// 创建标记
	CreateWindow(classSTATIC,"状态:",WS_CHILD | WS_VISIBLE ,15,20,40,18,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindow(classSTATIC,"电池",WS_CHILD | WS_VISIBLE ,60,20,100,18,hWnd,(HMENU)106,hInstance,0);
	// 创建显示电池电量的进度条
	hProgress = CreateWindowEx(WS_EX_CLIENTEDGE,classPROGRESS,"",WS_CHILD | WS_VISIBLE | PBS_SMOOTH,15,45,160,24,hWnd,(HMENU)107,hInstance,0);

	SendMessage(hProgress,PBM_SETRANGE,0,MAKELPARAM(0,100)); // 设置范围
	SendMessage(hProgress,PBM_SETPOS,50,0); // 设置当前值
	SendMessage(hProgress,PBM_SETBARCOLOR,0,RGB(77,166,255)); // 设置颜色
	// 创建数字显示电池电量
	CreateWindow(classSTATIC,"电池剩余电量:",WS_CHILD | WS_VISIBLE ,15,80,110,18,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindow(classSTATIC,"50%",WS_CHILD | WS_VISIBLE ,130,80,40,18,hWnd,(HMENU)108,hInstance,0);

	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoBatteryOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：电池WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoBatteryOK(HWND hDlg)
{
		return TRUE;
}


/****************************************************************************/
//  “关机” 界面
/****************************************************************************/

#define IDC_BATTERYENABLE	101
#define IDC_BATTERYTIME		102
#define IDC_EXTERNALENABLE	103
#define IDC_EXTERNALTIME	104

static LRESULT CALLBACK PowerOffWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoPowerOffCreate(HWND hWnd);
static LRESULT DoPowerOffOK(HWND hDlg);
static LRESULT DoBatteryEnable(HWND hWnd);
static LRESULT DoExternalEnable(HWND hWnd);

// **************************************************
// 声明：static ATOM RegisterPowerOffWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述： 注册关机窗口类名
// 引用: 
// **************************************************
static ATOM RegisterPowerOffWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strPowerOffClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)PowerOffWndProc; // 窗口过程
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK PowerOffWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：电源属性关机对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK PowerOffWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"PowerOff",8);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_BATTERYENABLE: // 电池状态改变
					DoBatteryEnable(hWnd);
					return 0;
				case IDC_EXTERNALENABLE: // 外部电源状态改变
					DoExternalEnable(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // 创建消息
			DoPowerOffCreate(hWnd);
			break;
		case WM_DESTROY: // 破坏窗口消息
			break;
		case WM_OK: // OK 消息
			return DoPowerOffOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoPowerOffCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建关机窗口
// 引用: 
// ********************************************************************
static LRESULT DoPowerOffCreate(HWND hWnd)
{
	HWND hChild;
	HINSTANCE hInstance;
	CTLCOLORSTRUCT             CCS ;
	BOOL bEnable;
	int iIndex;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

//		bEnable  = GetAutoPowerOff(USEBATTERY);
		// 从注册表读取使用电池关机的状态
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		// 创建使用电池关机的状态窗口
		hChild = CreateWindow(classBUTTON,  "使用电池时系统自动关机",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,5,201,18,hWnd,(HMENU)IDC_BATTERYENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0); // 设置状态

		// 创建使用电池关机时间的选择窗口
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,30,80,68,hWnd,(HMENU)IDC_BATTERYTIME,hInstance,0);
		CCS.fMask = CLF_TEXTBKCOLOR;//|CLF_TITLECOLOR;
		// 设置颜色
		CCS.cl_TextBk = RGB( 255,255,255);
//		CCS.cl_Title = RGB( 132,132,132);
		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		// 设置选择项
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"1 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"3 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"5 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"10 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"30 分钟");
		// 从注册表读取使用电池关机的时间
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // 设置当前时间
		EnableWindow(hChild,bEnable);

		hChild = CreateWindow(classSTATIC,"系统闲置          后关机",WS_CHILD | WS_VISIBLE ,5,30,201,18,hWnd,(HMENU)0xffff,hInstance,0);

//		bEnable  = GetAutoPowerOff(USEEXTERNALPOWER);
		// 从注册表中读取使用外部电源关机的状态
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		// 创建使用外部电源关机的状态窗口
		hChild = CreateWindow(classBUTTON,  "使用外部电源时自动关机",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,55,201,18,hWnd,(HMENU)IDC_EXTERNALENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0); // 设置当前状态值
		// 创建时间选择窗口
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,80,80,68,hWnd,(HMENU)IDC_EXTERNALTIME,hInstance,0);
		// 设置选择项
		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"1 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"3 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"5 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"10 分钟");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"30 分钟");
		// 读取当前时间
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // 设置当前时间
		EnableWindow(hChild,bEnable);

		hChild = CreateWindow(classSTATIC,"系统闲置          后关机",WS_CHILD | WS_VISIBLE ,5,80,201,18,hWnd,(HMENU)0xffff,hInstance,0);

		return 0;
}


// ********************************************************************
// 声明：static LRESULT DoPowerOffOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：关机WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoPowerOffOK(HWND hDlg)
{
	BOOL bEnable;
	HWND hChild ;
	DWORD iIndex ;

		hChild = GetDlgItem(hDlg,IDC_BATTERYENABLE); // 得到使用电池状态窗口
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // 得到使用电池状态
		// 写到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{	// 得到当前时间
			hChild = GetDlgItem(hDlg,IDC_BATTERYTIME);
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0);
			// 写到注册表
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Battery\\PowerOff", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}

		// 得到使用外部电源的状态
		hChild = GetDlgItem(hDlg,IDC_EXTERNALENABLE);
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0);
		// 写入到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{   // 得到当前时间
			hChild = GetDlgItem(hDlg,IDC_EXTERNALTIME);
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0);
			// 写入到注册表
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Battery\\PowerOff", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}

		return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoBatteryEnable(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：关机处理 BATTERYENABLE 功能改变
// 引用: 
// ********************************************************************
static LRESULT DoBatteryEnable(HWND hWnd)
{
	HWND hBatteryEnable ;
	BOOL bEnable;
	HWND hComboBox;
	// 得到使用电池的状态
	hBatteryEnable = GetDlgItem(hWnd,IDC_BATTERYENABLE); 
	bEnable = SendMessage(hBatteryEnable,BM_GETCHECK,0,0);
	hComboBox = GetDlgItem(hWnd,IDC_BATTERYTIME); // 得到时间选择窗口
	EnableWindow(hComboBox,bEnable); // 设置时间选择窗口与得到的状态一致
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoExternalEnable(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：关机处理 EXTERNALENABLE 功能改变
// 引用: 
// ********************************************************************
static LRESULT DoExternalEnable(HWND hWnd)
{
	HWND hExternalEnable ;
	BOOL bEnable;
	HWND hComboBox;

	// 得到使用电池的状态
	hExternalEnable = GetDlgItem(hWnd,IDC_EXTERNALENABLE);
	bEnable = SendMessage(hExternalEnable,BM_GETCHECK,0,0);
	hComboBox = GetDlgItem(hWnd,IDC_EXTERNALTIME); // 得到时间选择窗口
	EnableWindow(hComboBox,bEnable); // 设置与得到的状态一致
	return 0;
}
