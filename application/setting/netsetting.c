/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：网络设置，用户界面部分
版本号：1.0.0
开发时期：2003-08-07
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
#define	STR_NETSETTING		"网络设置"
#define STR_IPADDRESS		"IP地址"
#define STR_SERVER			"服务器"

#define STR_IPFROMDHCP		"通过DHCP获得IP地址"
#define STR_IPFROMUSER		"指定一个IP地址"

#define STR_IPADDRESS_STATIC		"IP地址:"
#define STR_SUBNETMASK_STATIC		"子网掩码:"
#define STR_DEFAULTNETWAY_STATIC	"默认网关:"

#define STR_NETDHCPDECLARE1_STATIC	"如果启用了DHCP,那么服务"
#define STR_NETDHCPDECLARE2_STATIC	"器地址可以自动分配.您也"
#define STR_NETDHCPDECLARE3_STATIC	"可以指定DNS服务器."
#define STR_MASTERDNS_STATIC		"主控DNS:"
#define STR_ASSISTDNS_STATIC		"辅助DNS:"
/*****************************************************/

/***************  全局区 定义， 声明 *****************/
const struct dlgIDD_NetSetting{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_NetSetting = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,63,210,185,0,0,STR_NETSETTING },
    };

static LPTSTR strIPAddressClassName = "NETIPADDRESS";
static LPTSTR strNetServerClassName = "NETSERVER";

#define IDC_TAB		300

#define IDC_NETIPADDRESS	500
#define IDC_NETSERVER		501

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;

static const TABSHEET NetSettingSheet[] = {
	{STR_IPADDRESS,"NETIPADDRESS",IDC_NETIPADDRESS},
	{STR_SERVER,"NETSERVER",IDC_NETSERVER},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK NetSettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNetSettingOK(HWND hDlg);

ATOM RegisterIPAddressWindow(HINSTANCE hInstance);
ATOM RegisterNetServerWindow(HINSTANCE hInstance);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  函数实现 *****************/

//extern ATOM RegisterIPAddressClass(HINSTANCE hInst);

// **************************************************
// 声明：void DoNetSetting(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：网络设置入口。
// 引用: 
// **************************************************
void DoNetSetting(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		RegisterIPAddressClass(hInstance); // 注册IP地址类
//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		// 创建网络设置对话框
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_NetSetting,hWnd, (DLGPROC)NetSettingProc);
}

// ********************************************************************
//声明：static LRESULT CALLBACK NetSettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//参数：
//	IN hWnd - 窗口句柄
//  IN message - 需要处理的消息
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：网络设置对话框的窗口处理过程
//引用: 
// ********************************************************************
static LRESULT CALLBACK NetSettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_INITDIALOG: // 对话框初始化消息
			return DoInitDialog(hDlg);
		case WM_CLOSE: // 关闭窗口消息
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK消息
			DoNetSettingOK(hDlg);
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
// 功能描述：初始化对话框
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
		// 创建一个TAB窗口
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);
		// 设置TAB窗口颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterIPAddressWindow(hInstance); // 注册IP地址窗口
		RegisterNetServerWindow(hInstance); // 注册网络服务窗口

		numItem = sizeof(NetSettingSheet) / sizeof(TABSHEET); // 得到需要的TAB页
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = NetSettingSheet[i].lpName; // 得到该页的名称
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入一个TAB页
			// 创建一个对应的窗口，用于与该TAB页同时显示
			CreateWindow(NetSettingSheet[i].lpClassName,"",WS_CHILD,0,27,210,125,hDlg,(HMENU)NetSettingSheet[i].id,hInstance,0);
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前的选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一页为选择页
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,NetSettingSheet[iIndex].id),SW_SHOW); // 设置对应该页的窗口可见
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
// 返回值：无
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
			 case TCN_SELCHANGING: // 选择正在改变，改变之前发送的消息
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到选择的索引
				 if (iIndex != -1)
				 { // 将该索引所对应的窗口隐藏
					ShowWindow(GetDlgItem(hWnd,NetSettingSheet[iIndex].id),SW_HIDE);
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择改变完成，改变之后发送的消息
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到选择的索引
				 if (iIndex != -1)
				 { // 将该索引对应的窗口显示
					ShowWindow(GetDlgItem(hWnd,NetSettingSheet[iIndex].id),SW_SHOW);
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
//声明：static LRESULT DoNetSettingOK(HWND hDlg)
//参数：
//	IN hWnd - 窗口句柄
//返回值：
//	成功返回 TRUE，否则返回FALSE
//功能描述：网络设置WM_OK处理
//引用: 
// ********************************************************************
static LRESULT DoNetSettingOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(NetSettingSheet) / sizeof(TABSHEET); // 得到网络设置的页数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,NetSettingSheet[i].id); // 得到该页的窗口句柄
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 给该页发送WM_OK的消息
		}
		return TRUE;
}


/****************************************************************************/
//  “IP地址” 界面
/****************************************************************************/
#define IDC_IPDHCP		201
#define IDC_IPUSER		202

#define	ID_IPADDRESS			203
#define	ID_IPADDRESS_TITLE		204
#define ID_SUBNETMASK			205
#define ID_SUBNETMASK_TITLE		206
#define ID_DEFAULTNETWAY		207
#define ID_DEFAULTNETWAY_TITLE	208

static LRESULT CALLBACK IPAddressWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoIPAddressCreate(HWND hWnd);
static LRESULT DoIPAddressOK(HWND hDlg);
static LRESULT DoIPDHCP(HWND hWnd);


// **************************************************
// 声明：static ATOM RegisterIPAddressWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册IP地址窗口。
// 引用: 
// **************************************************
static ATOM RegisterIPAddressWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strIPAddressClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)IPAddressWndProc; // 程序过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
//声明：static LRESULT CALLBACK IPAddressWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//参数：
//	IN hWnd - 窗口句柄
//  IN message - 需要处理的消息
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：网络设置IP地址对话框的窗口处理过程
//引用: 
// ********************************************************************
static LRESULT CALLBACK IPAddressWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_IPDHCP: // DHCP功能有效
					DoIPDHCP(hWnd);
					return 0;
				case IDC_IPUSER: // 用户输入IP地址有效
					DoIPDHCP(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // 创建窗口消息
			return DoIPAddressCreate(hWnd);
		case WM_DESTROY: //破坏窗口消息
			break;
		case WM_OK: // OK消息
			return DoIPAddressOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
//声明：static LRESULT DoIPAddressCreate(HWND hWnd)
//参数：
//	IN hWnd - 窗口句柄
//返回值：
//	成功返回0，否则返回-1
//功能描述：创建“IP地址”对话框
//引用: 
// ********************************************************************
static LRESULT DoIPAddressCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	BOOL bIpDhcpEnable = TRUE;
	TCHAR lpIPAddress[64];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄

	// 得到DHCP选择的窗口
	hChild = CreateWindow(classBUTTON,  STR_IPFROMDHCP,WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,5,8,201,18,hWnd,(HMENU)IDC_IPDHCP,hInstance,0);

	// 读取注册表中DHCPEnable的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "DHCPEnable",&bIpDhcpEnable, sizeof(DWORD) ,REG_DWORD) == TRUE)
	{
	}
	SendMessage(hChild,BM_SETCHECK,bIpDhcpEnable,0); // 设置DHCP的窗口选择


	bIpDhcpEnable = ~bIpDhcpEnable; // 用户输入IP地址的选择与DHCP互斥
	// 得到用户输入IP地址选择的窗口
	hChild = CreateWindow(classBUTTON,  STR_IPFROMUSER,WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,5,31,201,18,hWnd,(HMENU)IDC_IPUSER,hInstance,0);
	SendMessage(hChild,BM_SETCHECK,bIpDhcpEnable,0); // 设置窗口的选择
	// 创建用户输入IP地址的窗口
	hChild = CreateWindow(classSTATIC,STR_IPADDRESS_STATIC,WS_CHILD | WS_VISIBLE ,5,55,72,18,hWnd,(HMENU)ID_IPADDRESS_TITLE,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,55,125,18,hWnd,(HMENU)ID_IPADDRESS,hInstance,0);

	// 读取IP地址的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "IPAddress",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // 设置IP地址
	}
	// 创建用户输入子网掩码的窗口
	hChild = CreateWindow(classSTATIC,STR_SUBNETMASK_STATIC,WS_CHILD | WS_VISIBLE ,5,79,72,18,hWnd,(HMENU)ID_SUBNETMASK_TITLE,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,79,125,18,hWnd,(HMENU)ID_SUBNETMASK,hInstance,0);
	// 读取子网掩码的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "SubNetmask",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // 设置子网掩码
	}
	// 创建用户输入默认网关的窗口
	hChild = CreateWindow(classSTATIC,STR_DEFAULTNETWAY_STATIC,WS_CHILD | WS_VISIBLE ,5,103,72,18,hWnd,(HMENU)ID_DEFAULTNETWAY_TITLE,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,103,125,18,hWnd,(HMENU)ID_DEFAULTNETWAY,hInstance,0);
	// 读取默认网关的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "DefaultNetWay",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // 设置默认网关
	}

	DoIPDHCP(hWnd); // 设置窗口的ENABLE
	return 0;
}

// ********************************************************************
//声明：static LRESULT DoIPAddressOK(HWND hDlg)
//参数：
//	IN hWnd - 窗口句柄
//返回值：
//	成功返回 TRUE，否则返回FALSE
//功能描述：IP地址WM_OK处理
//引用: 
// ********************************************************************
static LRESULT DoIPAddressOK(HWND hDlg)
{
	TCHAR lpIPAddress[64];
	HWND hChild;
	BOOL bIpDhcpEnable;

		hChild = GetDlgItem(hDlg,IDC_IPDHCP); // 得到窗口句柄
		
		bIpDhcpEnable = SendMessage(hChild,BM_GETCHECK,0,0); // 得到是否选择DHCP
		// 将当前的DHCP的值写入到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "DHCPEnable",&bIpDhcpEnable, sizeof(DWORD),REG_DWORD);

		hChild = GetDlgItem(hDlg,ID_IPADDRESS); // 得到IP地址的窗口

		GetWindowText(hChild, lpIPAddress, 64); // 得到IP地址
		// 写入IP地址到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "IPAddress",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		hChild = GetDlgItem(hDlg,ID_SUBNETMASK); // 得到子网掩码的窗口

		GetWindowText(hChild, lpIPAddress, 64); // 得到子网掩码
		// 写入子网掩码到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "SubNetmask",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		hChild = GetDlgItem(hDlg,ID_DEFAULTNETWAY); // 得到默认网关的窗口

		GetWindowText(hChild, lpIPAddress, 64); // 得到默认网关的值
		// 写入默认网关到注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "DefaultNetWay",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);
		return TRUE;
}

// ********************************************************************
//声明：static LRESULT DoIPDHCP(HWND hWnd)
//参数：
//	IN hWnd - 窗口句柄
//返回值：
//	成功返回 TRUE，否则返回FALSE
//功能描述：背光处理 IDC_IPDHCP 功能的设置
//引用: 
// ********************************************************************
static LRESULT DoIPDHCP(HWND hWnd)
{
	HWND hChild ;
	BOOL bEnable;

	hChild = GetDlgItem(hWnd,IDC_IPUSER); // 得到用户输入IP地址的选择窗口
	bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // 得到是否可以用户输入IP地址
	// 设置输入IP地址窗口ENABLE属性
	hChild = GetDlgItem(hWnd,ID_IPADDRESS_TITLE);
	EnableWindow(hChild,bEnable);
	hChild = GetDlgItem(hWnd,ID_IPADDRESS);
	EnableWindow(hChild,bEnable);
	// 设置输入子网掩码窗口ENABLE属性
	hChild = GetDlgItem(hWnd,ID_SUBNETMASK_TITLE);
	EnableWindow(hChild,bEnable);
	hChild = GetDlgItem(hWnd,ID_SUBNETMASK);
	EnableWindow(hChild,bEnable);
	// 设置输入默认网关窗口ENABLE属性
	hChild = GetDlgItem(hWnd,ID_DEFAULTNETWAY_TITLE);
	EnableWindow(hChild,bEnable);
	hChild = GetDlgItem(hWnd,ID_DEFAULTNETWAY);
	EnableWindow(hChild,bEnable);
	return 0;
}

/****************************************************************************/
//  “网络服务器” 界面
/****************************************************************************/
#define ID_MASTERDNS   300
#define ID_ASSISTDNS   301

static LRESULT CALLBACK NetServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoNetServerCreate(HWND hWnd);
static LRESULT DoNetServerOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterNetServerWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回系统注册结果
// 功能描述：注册网络服务窗口。
// 引用: 
// **************************************************
static ATOM RegisterNetServerWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strNetServerClassName; // 网络服务窗口类
   wc.lpfnWndProc	=(WNDPROC)NetServerWndProc; // 过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
//声明：static LRESULT CALLBACK NetServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//参数：
//	IN hWnd - 窗口句柄
//  IN message - 需要处理的消息
//  IN wParam - 消息参数
//  IN lParam - 消息参数
//返回值：
//	消息处理后返回的结果
//功能描述：网络设置“服务器”对话框的窗口处理过程
//引用: 
// ********************************************************************
static LRESULT CALLBACK NetServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoNetServerCreate(hWnd);
		case WM_DESTROY: // 破坏窗口
			break;
		case WM_OK: // OK消息
			return DoNetServerOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
//声明：static LRESULT DoNetServerCreate(HWND hWnd)
//参数：
//	IN hWnd - 窗口句柄
//返回值：
//	成功返回0，否则返回-1
//功能描述：创建“网络服务器”对话框
//引用: 
// ********************************************************************
static LRESULT DoNetServerCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	TCHAR lpIPAddress[64];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	// 创建声明内容
	hChild = CreateWindow(classSTATIC,STR_NETDHCPDECLARE1_STATIC,WS_CHILD | WS_VISIBLE ,5,5,201,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindow(classSTATIC,STR_NETDHCPDECLARE2_STATIC,WS_CHILD | WS_VISIBLE ,5,29,201,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindow(classSTATIC,STR_NETDHCPDECLARE3_STATIC,WS_CHILD | WS_VISIBLE ,5,53,201,18,hWnd,(HMENU)0xffff,hInstance,0);
	// 创建主控DNS
	hChild = CreateWindow(classSTATIC,STR_MASTERDNS_STATIC,WS_CHILD | WS_VISIBLE ,5,79,72,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,79,125,18,hWnd,(HMENU)ID_MASTERDNS,hInstance,0);
	// 得到注册表中主控DNS的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "MasterDNS",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // 设置内容
	}
	// 创建辅控DNS
	hChild = CreateWindow(classSTATIC,STR_ASSISTDNS_STATIC,WS_CHILD | WS_VISIBLE ,5,103,72,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,103,125,18,hWnd,(HMENU)ID_ASSISTDNS,hInstance,0);
	// 得到注册表中辅控DNS的值
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "AssistDNS",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // 设置内容
	}

	return 0;
}
// ********************************************************************
//声明：static LRESULT DoNetServerOK(HWND hDlg)
//参数：
//	IN hWnd - 窗口句柄
//返回值：
//	成功返回 TRUE，否则返回FALSE
//功能描述：网络服务器WM_OK处理
//引用: 
// ********************************************************************
static LRESULT DoNetServerOK(HWND hDlg)
{
	TCHAR lpIPAddress[64];
	HWND hChild;

		hChild = GetDlgItem(hDlg,ID_MASTERDNS); // 得到主控DNS的窗口句柄

		GetWindowText(hChild, lpIPAddress, 64); // 得到内容
		// 写入注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "MasterDNS",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		hChild = GetDlgItem(hDlg,ID_ASSISTDNS); // 得到辅控DNS的窗口句柄

		GetWindowText(hChild, lpIPAddress, 64); // 得到内容
		// 写入注册表
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "AssistDNS",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		return TRUE;
}

