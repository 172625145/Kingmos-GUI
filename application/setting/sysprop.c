/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：系统属性，用户界面部分
版本号：1.0.0
开发时期：2003-06-24
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <eprogres.h>

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_SystemProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_SystemProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,70,210,185,0,0,"系统属性" },
    };

static LPTSTR strGeneralClassName = "GENERALCLASS";
static LPTSTR strVersionClassName = "VERSIONCLASS";

#define IDC_TAB		300

#define IDC_GENERAL  400
#define IDC_VERSION  401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET SystemPropertySheet[] = {
	{"常规","GENERALCLASS",IDC_GENERAL},
	{"版权","VERSIONCLASS",IDC_VERSION},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK SystemPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

ATOM RegisterGeneralWindow(HINSTANCE hInstance);
ATOM RegisterVersionWindow(HINSTANCE hInstance);

/***************  函数实现 *****************/

// **************************************************
// 声明：void DoSystemProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：系统属性入口函数。
// 引用: 
// **************************************************
void DoSystemProperty(HINSTANCE hInstance,HWND hWnd)
{
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_SystemProperty,hWnd, (DLGPROC)SystemPropertyProc); // 创建系统属性对话框
}

// ********************************************************************
// 声明：static LRESULT CALLBACK SystemPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：系统属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK SystemPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

		RegisterGeneralWindow(hInstance); // 注册通用窗口
		RegisterVersionWindow(hInstance); // 注册版本窗口
		
		numItem = sizeof(SystemPropertySheet) / sizeof(TABSHEET); // 得到条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = SystemPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入一个TAB条目
			CreateWindow(SystemPropertySheet[i].lpClassName,"",WS_CHILD,0,30,210,140,hDlg,(HMENU)SystemPropertySheet[i].id,hInstance,0); // 创建一个相对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个条目为当前条目
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,SystemPropertySheet[iIndex].id),SW_SHOW); // 显示当前索引对应的窗口
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
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前索引
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SystemPropertySheet[iIndex].id),SW_HIDE); // 隐藏索引对应的窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SystemPropertySheet[iIndex].id),SW_SHOW); // 显示索引对应的窗口
				 }
				 return 0;
		}
		return 0;
}


/****************************************************************************/
//  “常规” 界面
/****************************************************************************/
#define ID_PROGRESS		107
#define ID_USEMEMORY	106
#define ID_TOTALMEMORY	105

static LRESULT CALLBACK GeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DisplaySystemGeneralWindow(HWND hWnd,HDC hdc);
static LRESULT DoSystemGeneralCreate(HWND hWnd);
static LRESULT ReFreshMemory(HWND hWnd);

// **************************************************
// 声明：static ATOM RegisterGeneralWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册通用窗口类。
// 引用: 
// **************************************************
static ATOM RegisterGeneralWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strGeneralClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)GeneralWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK GeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：系统属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK GeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"General",7);
			//DisplaySystemGeneralWindow(hWnd,hdc);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_TIMER: // 定时器消息
			ReFreshMemory(hWnd);
			return 0;
		case WM_CREATE: // 创建消息
			return DoSystemGeneralCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			KillTimer(hWnd,1);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static void DisplaySystemGeneralWindow(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 窗口句柄
//    IN hdc - 设备
// 返回值：
//	消息处理后返回的结果
// 功能描述：绘制系统属性常规对话框
// 引用: 
// ********************************************************************
/*
static void DisplaySystemGeneralWindow(HWND hWnd,HDC hdc)
{
	RECT rect;
	TCHAR lpUseMemory[64];
	DWORD dwTotalSize,dwUseSize,dwUsePer;
	HBRUSH hBrush;
	DWORD dwEnd;

		SetBkMode( hdc, TRANSPARENT ) ;
		TextOut(hdc,10,5,"系统:巨果系统(Kingmos) 2.0.0",strlen("系统:巨果系统(Kingmos) 2.0.0"));

//		dwTotalSize = GetTotalMemorySize();
		dwTotalSize = 16 * 1024 * 1024;

		sprintf(lpUseMemory,"内存总量: %d",dwTotalSize);
		TextOut(hdc,10,30,lpUseMemory,strlen(lpUseMemory));
//		dwUseSize = GetUseMemorySize();
		dwUseSize = 7 * 1024 * 1024;
		dwUsePer = (dwUseSize * 100 ) / dwTotalSize;
		sprintf(lpUseMemory,"已使用: %d%%",dwUsePer);
		
		TextOut(hdc,10,55,lpUseMemory,strlen(lpUseMemory));

		rect.left = 10;
		rect.top = 80;
		rect.right = rect.left + 170;
		rect.bottom = rect.top + 20;
		DrawEdge(hdc,&rect,BDR_SUNKENINNER,BF_RECT);
		InflateRect(&rect,-2,-2);
		
		dwEnd = rect.right;
		rect.right = dwUsePer * (rect.right-rect.left) / 100;
		hBrush = CreateSolidBrush(RGB(77,166,255));
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);

		rect.left = rect.right ;
		rect.right = dwEnd;
		hBrush = GetStockObject(WHITE_BRUSH);
		FillRect(hdc,&rect,hBrush);

}
*/
// ********************************************************************
// 声明：static LRESULT DoSystemGeneralCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建系统常规属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoSystemGeneralCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hProgress;
	DWORD dwTotalSize,dwUseSize,dwUsePer;
	TCHAR lpUseMemory[64];
	MEMORYSTATUS MemoryStatus;
/*
typedef struct _MEMORYSTATUS
{
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORD dwTotalPhys;
    DWORD dwAvailPhys;
    DWORD dwTotalPageFile;
    DWORD dwAvailPageFile;
    DWORD dwTotalVirtual;
    DWORD dwAvailVirtual;
} MEMORYSTATUS, FAR *LPMEMORYSTATUS;
*/
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		// 创建系统窗口
		CreateWindow(classSTATIC,"系统:",WS_CHILD | WS_VISIBLE ,15,5,40,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,"巨果·Kingmos",WS_CHILD | WS_VISIBLE ,60,5,120,18,hWnd,(HMENU)0xffff,hInstance,0);
		// 创建版本
		CreateWindow(classSTATIC,"版本:",WS_CHILD | WS_VISIBLE ,15,30,40,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,"2.0.0",WS_CHILD | WS_VISIBLE ,60,30,120,18,hWnd,(HMENU)0xffff,hInstance,0);

		MemoryStatus.dwLength = sizeof(MemoryStatus);
		GlobalMemoryStatus(&MemoryStatus); // 得到当前内存状况
		dwTotalSize = MemoryStatus.dwTotalPhys /1024; // 得到总内存
	//	dwTotalSize = 16 * 1024 * 1024;值
		sprintf(lpUseMemory,"%d KB",dwTotalSize);
		// 显示内存总量
		CreateWindow(classSTATIC,"内存总量:",WS_CHILD | WS_VISIBLE ,15,55,74,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,90,55,90,18,hWnd,(HMENU)ID_TOTALMEMORY,hInstance,0);
		// 得到使用内存
		dwUseSize = MemoryStatus.dwTotalPhys - MemoryStatus.dwAvailPhys;
	//	dwUseSize = 7 * 1024 * 1024;
//		dwUsePer = (dwUseSize * 100 ) / dwTotalSize;
		dwUsePer = MemoryStatus.dwMemoryLoad;
		sprintf(lpUseMemory,"%d%%",dwUsePer);
		// 显示使用内存
		CreateWindow(classSTATIC,"已使用:",WS_CHILD | WS_VISIBLE ,15,80,60,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,75,80,100,18,hWnd,(HMENU)ID_USEMEMORY,hInstance,0);
		// 创建显示当前内存的进度条
		hProgress = CreateWindowEx(WS_EX_CLIENTEDGE,classPROGRESS,"",WS_CHILD | WS_VISIBLE | PBS_SMOOTH,15,105,160,20,hWnd,(HMENU)ID_PROGRESS,hInstance,0);

		SendMessage(hProgress,PBM_SETRANGE,0,MAKELPARAM(0,100)); // 设置范围
		SendMessage(hProgress,PBM_SETPOS,dwUsePer,0); // 设置当前值
		SendMessage(hProgress,PBM_SETBARCOLOR,0,RGB(77,166,255)); // 设置颜色

		SetTimer(hWnd,1,2000,NULL); // 启动定时器
	return 0;
}

// ********************************************************************
// 声明：static LRESULT ReFreshMemory(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：刷新当前的内存显示
// 引用: 
// ********************************************************************
static LRESULT ReFreshMemory(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	DWORD dwTotalSize,dwUseSize,dwUsePer;
	TCHAR lpUseMemory[64];
	MEMORYSTATUS MemoryStatus;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄


		MemoryStatus.dwLength = sizeof(MemoryStatus);
		GlobalMemoryStatus(&MemoryStatus); // 得到当前的内存值

		RETAILMSG(1,("MemoryStatus.dwTotalPhys = %d\r\n",MemoryStatus.dwTotalPhys));
		RETAILMSG(1,("MemoryStatus.dwAvailPhys = %d\r\n",MemoryStatus.dwAvailPhys));
		RETAILMSG(1,("MemoryStatus.dwMemoryLoad = %d\r\n",MemoryStatus.dwMemoryLoad));
		// 得到总内存
		dwTotalSize = MemoryStatus.dwTotalPhys /1024;
	//	dwTotalSize = 16 * 1024 * 1024;
		sprintf(lpUseMemory,"%d KB",dwTotalSize);

		//CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,90,55,90,18,hWnd,(HMENU)0xffff,hInstance,0);
		hChild = GetDlgItem(hWnd,ID_TOTALMEMORY);
		SetWindowText(hChild,lpUseMemory);  // 设置总内存值
		// 得到已经使用的内存
		dwUseSize = MemoryStatus.dwTotalPhys - MemoryStatus.dwAvailPhys;
		RETAILMSG(1,("dwUseSize = %d\r\n",dwUseSize));
		dwUsePer = MemoryStatus.dwMemoryLoad;
		sprintf(lpUseMemory,"%d%%",dwUsePer);

//		CreateWindow(classSTATIC,"已使用:",WS_CHILD | WS_VISIBLE ,15,80,60,18,hWnd,(HMENU)0xffff,hInstance,0);
//		CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,75,80,100,18,hWnd,(HMENU)0xffff,hInstance,0);
		hChild = GetDlgItem(hWnd,ID_USEMEMORY);
		SetWindowText(hChild,lpUseMemory); // 设置已经使用的内存

//		hProgress = CreateWindowEx(WS_EX_CLIENTEDGE,classPROGRESS,"",WS_CHILD | WS_VISIBLE | PBS_SMOOTH,15,105,160,20,hWnd,(HMENU)ID_PROGRESS,hInstance,0);
		hChild = GetDlgItem(hWnd,ID_PROGRESS);

		SendMessage(hChild,PBM_SETPOS,dwUsePer,0); // 设置进度条当前的位置

	return 0;
}


/****************************************************************************/
//  “版本” 界面
/****************************************************************************/
static LRESULT CALLBACK VersionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DisplaySystemVersionWindow(HWND hWnd,HDC hdc);

// **************************************************
// 声明：static ATOM RegisterVersionWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册版本窗口类。
// 引用: 
// **************************************************
static ATOM RegisterVersionWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strVersionClassName; // 版本窗口类名
   wc.lpfnWndProc	=(WNDPROC)VersionWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK VersionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：系统属性对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK VersionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"Version",7);
			DisplaySystemVersionWindow(hWnd,hdc);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			break;
		case WM_DESTROY: // 破坏消息
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// ********************************************************************
// 声明：static void DisplaySystemVersionWindow(HWND hWnd,HDC hdc)
// 参数：
//	IN hWnd - 窗口句柄
//    IN hdc - 设备
// 返回值：
//	消息处理后返回的结果
// 功能描述：绘制系统属性常规对话框
// 引用: 
// ********************************************************************
static void DisplaySystemVersionWindow(HWND hWnd,HDC hdc)
{
	RECT rect = {5,45,205,115};
	HICON hIcon;
	LPTSTR lpShowString1 = "深圳微逻辑电子有限公司";
	LPTSTR lpShowString2 = "版权所有(C)1998-2003";
	LPTSTR lpShowString3 = "保留所有权利";
	
		SetBkMode( hdc, TRANSPARENT ) ; // 设置显示模式
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SYSLOGO ), IMAGE_ICON,32, 32, 0 ) ;
		DrawIcon(hdc,90,5,hIcon); // 绘制系统图标
		DestroyIcon(hIcon);
		// 绘制版本信息
		DrawText(hdc,lpShowString1 ,strlen(lpShowString1 ),&rect,DT_CENTER);
		rect.top += 25;
		DrawText(hdc,lpShowString2 ,strlen(lpShowString2 ),&rect,DT_CENTER);
		rect.top += 25;
		DrawText(hdc,lpShowString3 ,strlen(lpShowString3 ),&rect,DT_CENTER);
//		DrawText(hdc,"Copyright (C)1998-2003 \n深圳市微逻辑电子有限公司.  \nAll rights reserved0",strlen("系统：Kingmos(巨果) 1.0"),&rect,DT_CENTER);
//		DrawText(hdc,"Copyright (C)1998-2003 \n深圳市微逻辑电子有限公司.  \nAll rights reserved0",strlen("系统：Kingmos(巨果) 1.0"),&rect,DT_CENTER);
}

