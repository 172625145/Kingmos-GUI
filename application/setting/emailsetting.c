/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：邮件属性，用户界面部分
版本号：1.0.0
开发时期：2003-06-25
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include "MailBoxApi.h"
/***************  全局区 定义， 声明 *****************/
#define STR_EMAILPROC "邮件配置"

static const struct dlgIDD_EmailProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_EmailProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,160,0,0,STR_EMAILPROC },
    };

static LPTSTR strEmailServerClassName = "EMAILSERVERCLASS";
static LPTSTR strEmailUserClassName = "EMAILUSERCLASS";
static LPTSTR strEmailSendRevClassName = "EMAILSENDREVCLASS";
static LPTSTR strEmailRulerClassName = "EMAILRULERCLASS";

CONFIGINFO g_EmailCfg;
#define IDC_TAB		300

#define IDC_EMAILSERVER   400
#define IDC_EMAILUSER     401
#define IDC_EMAILSENDREV  402
#define IDC_EMAILRULER    403

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET EmailPropertySheet[] = {
	{"服务器","EMAILSERVERCLASS",IDC_EMAILSERVER},
	{"用户","EMAILUSERCLASS",IDC_EMAILUSER},
	{"收发","EMAILSENDREVCLASS",IDC_EMAILSENDREV},
	{"规则","EMAILRULERCLASS",IDC_EMAILRULER},
};

/***************  函数声明 *****************/
static LRESULT CALLBACK EmailPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEmailPropertyOK(HWND hDlg);

ATOM RegisterEmailServerWindow(HINSTANCE hInstance);
ATOM RegisterEmailUserWindow(HINSTANCE hInstance);
ATOM RegisterEmailSendRevWindow(HINSTANCE hInstance);
ATOM RegisterEmailRuler(HINSTANCE hInstance);
/***************  函数实现 *****************/

// **************************************************
// 声明：void DoEmailProperty(HINSTANCE hInstance,HWND hWnd)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：邮件属性配置入口。
// 引用: 
// **************************************************
void DoEmailProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;
	HWND hDlg;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		hDlg = FindWindow( NULL, STR_EMAILPROC ) ;  // 查找窗口是否已经打开
		if( hDlg != 0 )
		{ // 已经打开
			SetForegroundWindow( hDlg ) ; // 设置到前台
			return ;
		}
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_EmailProperty,hWnd, (DLGPROC)EmailPropertyProc); // 创建对话框
}

// ********************************************************************
// 声明：static LRESULT CALLBACK EmailPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：邮件配置对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK EmailPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_CLOSE: // 关闭窗口
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK消息
			DoEmailPropertyOK(hDlg);
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
   
	
	InitEmailConfig( &g_EmailCfg );//初始化邮件配置信息
	GetEmailConfig( &g_EmailCfg ); //获得配置信息
		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // 得到实例句柄
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0); // 创建TAB
		// 设置颜色
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterEmailServerWindow(hInstance); // 注册邮件服务窗口
		RegisterEmailUserWindow(hInstance); // 注册邮件用户窗口
		RegisterEmailSendRevWindow(hInstance); // 注册收发窗口
        RegisterEmailRuler(hInstance);// 注册邮件规则窗口
		numItem = sizeof(EmailPropertySheet) / sizeof(TABSHEET); // 得到TAB条目个数
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = EmailPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // 插入TAB条目
			CreateWindow(EmailPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,100,hDlg,(HMENU)EmailPropertySheet[i].id,hInstance,0); // 创建对应的窗口
		}

		iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
		if (iIndex == -1)
		{ // 没有选择
			TabCtrl_SetCurSel(hTab,0); // 设置第一个TAB条目为当前选择
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,EmailPropertySheet[iIndex].id),SW_SHOW); // 显示当前选择所对应的窗口
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

		hTab = GetDlgItem(hWnd,IDC_TAB); // 得到TAB的窗口句柄
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // 选择正在改变，改变之前
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,EmailPropertySheet[iIndex].id),SW_HIDE); // 隐藏选择所对应的窗口
				 }
				 return 0;
			 case TCN_SELCHANGE: // 选择已经改变，改变之后
				 iIndex = TabCtrl_GetCurSel(hTab); // 得到当前选择
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,EmailPropertySheet[iIndex].id),SW_SHOW); // 显示选择所对应的窗口
				 }
				 return 0;
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoEmailPropertyOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：显示邮件WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoEmailPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(EmailPropertySheet) / sizeof(TABSHEET); // 得到TAB的条目个数
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,EmailPropertySheet[i].id); // 得到条目所对应的窗口句柄
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // 发送WM_OK消息
		}
        SaveEmailConfig( &g_EmailCfg ); // 保存邮件配置
        DeInitEmailConfig( &g_EmailCfg ); // 释放邮件配置
		return TRUE;
}


/****************************************************************************/
//  “服务器” 界面
/****************************************************************************/
static LRESULT CALLBACK EmailServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoServerCreate(HWND hWnd);
static LRESULT DoEmailServerOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterEmailServerWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回注册结果
// 功能描述：注册邮件服务器的窗口
// 引用: 
// **************************************************
static ATOM RegisterEmailServerWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailServerClassName; // 邮件服务器窗口类名 
   wc.lpfnWndProc	=(WNDPROC)EmailServerWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 得到背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK EmailServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：邮件属性服务器对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK EmailServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{ 
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"EmailServer",11);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoServerCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoEmailServerOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoServerCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建邮件服务器属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoServerCreate(HWND hWnd)
{
	HINSTANCE hInstance;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	//接收
	CreateWindow(classSTATIC,"接收(POP3):",WS_CHILD | WS_VISIBLE ,5,5,90,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,95,5,100,20,hWnd,(HMENU)101,hInstance,0);
	//发送
	CreateWindow(classSTATIC,"发送(SMTP):",WS_CHILD | WS_VISIBLE ,5,30,90,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,95,30,100,20,hWnd,(HMENU)102,hInstance,0);
	//帐号
	CreateWindow(classSTATIC,"帐号:",WS_CHILD | WS_VISIBLE ,5,55,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,50,55,145,20,hWnd,(HMENU)103,hInstance,0);
	//密码
	CreateWindow(classSTATIC,"密码:",WS_CHILD | WS_VISIBLE ,5,80,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE | ES_PASSWORD,50,80,145,20,hWnd,(HMENU)104,hInstance,0);
    //设置接收POP3服务器
	if ( g_EmailCfg.pop3server && (int)strlen(g_EmailCfg.pop3server) ) SetWindowText( GetDlgItem( hWnd,101 ),g_EmailCfg.pop3server );
    //设置接收smtp服务器
    if ( g_EmailCfg.smtpserver && (int)strlen(g_EmailCfg.smtpserver) ) SetWindowText( GetDlgItem( hWnd,102 ),g_EmailCfg.smtpserver );
    // 设置接受帐号
	if ( g_EmailCfg.account && (int)strlen(g_EmailCfg.account) )   SetWindowText( GetDlgItem( hWnd,103 ),g_EmailCfg.account );
	// 设置密码
    if ( g_EmailCfg.password && (int)strlen(g_EmailCfg.password))  SetWindowText( GetDlgItem( hWnd,104 ),g_EmailCfg.password );

	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoEmailServerOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：邮件服务器WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoEmailServerOK(HWND hDlg)
{
	    int iLen ;
		HWND hTemp;
		
		hTemp = GetDlgItem( hDlg, 101 ); //得到pop3的服务器窗口句柄
		iLen = GetWindowTextLength( hTemp ); //得到文本长度
		free( g_EmailCfg.pop3server );// 释放原来的服务器名
        g_EmailCfg.pop3server = NULL;
		if ( iLen >0 )
		{
			g_EmailCfg.pop3server = malloc( iLen + 2 ); // 分配存放新的服务器的缓存
			memset( g_EmailCfg.pop3server, 0,iLen+2 ); // 清空内容
			GetWindowText( hTemp, g_EmailCfg.pop3server ,iLen+2); // 得到新的服务器名
		}
		
		hTemp = GetDlgItem( hDlg, 102 ); // 得到SMTP服务器的窗口句柄
		iLen = GetWindowTextLength( hTemp ); // 得到文本长度
		free( g_EmailCfg.smtpserver );// 释放原来的服务器名
        g_EmailCfg.smtpserver = NULL; 
		if ( iLen >0 )
		{
			g_EmailCfg.smtpserver = malloc( iLen + 2 ); // 分配新的缓存
			memset( g_EmailCfg.smtpserver, 0,iLen+2 ); // 清空缓存
			GetWindowText( hTemp, g_EmailCfg.smtpserver ,iLen+2); // 得到新的服务器名
		}

		hTemp = GetDlgItem( hDlg, 103 ); // 得到帐号的窗口句柄
		iLen = GetWindowTextLength( hTemp ); // 得到文本长度
		free( g_EmailCfg.account ); // 释放原来的帐号
        g_EmailCfg.account = NULL;
		if ( iLen >0 )
		{
			g_EmailCfg.account = malloc( iLen + 2 ); // 分配新的缓存
			memset( g_EmailCfg.account, 0,iLen+2 ); // 清空缓存
			GetWindowText( hTemp, g_EmailCfg.account ,iLen+2); // 得到新的帐号
		}

		hTemp = GetDlgItem( hDlg, 104 ); // 得到密码窗口
		iLen = GetWindowTextLength( hTemp ); // 得到文本长度
		free( g_EmailCfg.password ); // 释放原来的密码
        g_EmailCfg.password = NULL;
		if ( iLen >0 )
		{
			g_EmailCfg.password = malloc( iLen + 2 ); // 分配新的缓存
			memset( g_EmailCfg.password, 0,iLen+2 ); // 清空缓存
			GetWindowText( hTemp, g_EmailCfg.password ,iLen+2); // 得到新的密码
		}             
		return TRUE;
}


/****************************************************************************/
//  “用户” 界面
/****************************************************************************/
static LRESULT CALLBACK EmailUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoEmailUserCreate(HWND hWnd);
static LRESULT DoEmailUserOK(HWND hDlg);

// **************************************************
// 声明：static ATOM RegisterEmailUserWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回注册结果
// 功能描述：注册邮件用户窗口类。
// 引用: 
// **************************************************
static ATOM RegisterEmailUserWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailUserClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)EmailUserWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK EmailUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：邮件属性用户对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK EmailUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"EmailUser",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoEmailUserCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoEmailUserOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoEmailUserCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建邮件用户属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoEmailUserCreate(HWND hWnd)
{
	HINSTANCE hInstance;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	//姓名
	CreateWindow(classSTATIC,"姓名:",WS_CHILD | WS_VISIBLE ,5,5,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,50,5,145,20,hWnd,(HMENU)101,hInstance,0);
	//单位
	CreateWindow(classSTATIC,"单位:",WS_CHILD | WS_VISIBLE ,5,30,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,50,30,145,20,hWnd,(HMENU)102,hInstance,0);
	//邮件地址
	CreateWindow(classSTATIC,"邮件地址:",WS_CHILD | WS_VISIBLE ,5,55,75,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,80,55,110,20,hWnd,(HMENU)103,hInstance,0);
	//回复地址
	CreateWindow(classSTATIC,"回复地址:",WS_CHILD | WS_VISIBLE ,5,80,75,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,80,80,110,20,hWnd,(HMENU)104,hInstance,0);
	// 设置姓名
    if ( g_EmailCfg.user  && (int)strlen( g_EmailCfg.user )> 0)   SetWindowText( GetDlgItem( hWnd,101 ),g_EmailCfg.user );
    // 设置单位
	if ( g_EmailCfg.Company  && (int)strlen( g_EmailCfg.Company ) > 0) SetWindowText( GetDlgItem( hWnd,102 ),g_EmailCfg.Company );
	// 设置邮件地址
	if ( g_EmailCfg.From  && (int)strlen( g_EmailCfg.From )>0 )  SetWindowText( GetDlgItem( hWnd,103 ),g_EmailCfg.From );
	// 设置回复地址
    if ( g_EmailCfg.Reply  && (int)strlen( g_EmailCfg.Reply )>0 )  SetWindowText( GetDlgItem( hWnd,104 ),g_EmailCfg.Reply );
	
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoEmailUserOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：邮件用户WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoEmailUserOK(HWND hDlg)
{
	HWND hTemp ;
	int iLen;

	hTemp = GetDlgItem( hDlg, 101 ); // 得到姓名的窗口句柄
	iLen = GetWindowTextLength( hTemp ); // 得到文本长度
	free( g_EmailCfg.user ); // 释放原来的姓名
	g_EmailCfg.user = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.user = malloc( iLen +2 ); // 分配新的缓存
		memset( g_EmailCfg.user, 0, iLen+2 ); // 清空缓存
		GetWindowText( hTemp, g_EmailCfg.user ,iLen ); // 得到新的姓名
	}

	hTemp = GetDlgItem( hDlg, 102 ); // 得到单位的窗口句柄
	iLen = GetWindowTextLength( hTemp ); // 得到文本长度
	free( g_EmailCfg.Company ); // 释放原来的单位
	g_EmailCfg.Company = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.Company = malloc( iLen +2 ); // 分配新的缓存
		memset( g_EmailCfg.Company, 0, iLen+2 ); // 清空缓存
		GetWindowText( hTemp, g_EmailCfg.Company,iLen ); // 得到新的单位
	}

	hTemp = GetDlgItem( hDlg, 103 ); // 得到邮件地址的窗口句柄
	iLen = GetWindowTextLength( hTemp ); // 得到文本长度
	free( g_EmailCfg.From ); // 释放原来的邮件地址
	g_EmailCfg.From = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.From = malloc( iLen +2 ); // 分配新的地址
		memset( g_EmailCfg.From, 0, iLen+2 ); // 清空缓存
		GetWindowText( hTemp, g_EmailCfg.From,iLen ); // 得到新的邮件地址
	}

	hTemp = GetDlgItem( hDlg, 104 ); // 得到回复邮件的窗口句柄
	iLen = GetWindowTextLength( hTemp ); // 得到文本长度
	free( g_EmailCfg.Reply ); // 释放原来的回复地址
	g_EmailCfg.Reply = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.Reply = malloc( iLen +2 ); // 分配新的缓存
		memset( g_EmailCfg.Reply, 0, iLen+2 ); // 清空缓存
		GetWindowText( hTemp, g_EmailCfg.Reply,iLen ); // 得到新的回复地址
	}

	return TRUE;
}

/****************************************************************************/
//  “收发” 界面
/****************************************************************************/
#define IDC_AUTORECEIVEENABLE	101
#define IDC_AUTORECEIVETIME		102
#define IDC_UPDOWN				103
#define IDC_COMBOBOX			104
#define IDC_BACKUPNUMENABLE		105
#define IDC_DOWNLOADALLEMAIL	106

static LRESULT CALLBACK EmailSendRevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoEmailSendRevCreate(HWND hWnd);
static LRESULT DoEmailSendRevOK(HWND hDlg);
static LRESULT DoAutoReceiveEnable(HWND hWnd);
static LRESULT DoBackupNumEnable(HWND hWnd);

// **************************************************
// 声明：static ATOM RegisterEmailSendRevWindow(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值： 返回系统注册结果
// 功能描述：注册邮件收发窗口类。
// 引用: 
// **************************************************
static ATOM RegisterEmailSendRevWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) ); // 清空结构
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailSendRevClassName; // 邮件收发窗口类
   wc.lpfnWndProc	=(WNDPROC)EmailSendRevWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK EmailSendRevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：邮件属性收发对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK EmailSendRevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"EmailSendRev",12);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_AUTORECEIVEENABLE: // 自动接受邮件功能改变
					DoAutoReceiveEnable(hWnd);
					return 0;
				case IDC_BACKUPNUMENABLE: // 备份邮件数目改变
					DoBackupNumEnable(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // 创建消息
			return DoEmailSendRevCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoEmailSendRevOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoEmailSendRevCreate(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回0，否则返回-1
// 功能描述：创建邮件收发属性对话框
// 引用: 
// ********************************************************************
static LRESULT DoEmailSendRevCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	BOOL bEnable;
	HWND hEdit,hUpDown,hComboBox;
	int iIndex , ibackupNum;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 实例句柄

	// 创建自动邮件收取的时间间隔
	if (g_EmailCfg.Interval== 0)
		bEnable = FALSE;
	else
		bEnable = TRUE;

	hChild = CreateWindow(classBUTTON,  "每隔",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,5,50,18,hWnd,(HMENU)IDC_AUTORECEIVEENABLE,hInstance,0);
	// 创建输入时间间隔的窗口	
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,  "10",WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_READONLY,60,5,20,18,hWnd,(HMENU)IDC_AUTORECEIVETIME,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,2,0);
	// 创建上下窗口
	hUpDown = CreateWindow(classUPDOWN32,  "",WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT,80,5,12,18,hWnd,(HMENU)IDC_UPDOWN,hInstance,0);
	// 设置UODOWN的范围
	SendMessage(hUpDown,UDM_SETRANGE,0,MAKELPARAM(30,5));
	SendMessage(hUpDown,UDM_SETPOS,11,0);
	SendMessage(hUpDown,UDM_SETBUDDY,(WPARAM)hEdit,0);
	
	EnableWindow(hEdit,bEnable);
	EnableWindow(hUpDown,bEnable);

	CreateWindow(classSTATIC,  "分钟收取邮件",WS_CHILD | WS_VISIBLE ,95,5,100,18,hWnd,(HMENU)0xffff,hInstance,0);

	// 下载邮件
//	bEnable = GetEmailDownLoadAll();
	bEnable = g_EmailCfg.bDownAll;
	hChild = CreateWindow(classBUTTON,  "下载所有邮件(不选此项,",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,30,200,18,hWnd,(HMENU)IDC_DOWNLOADALLEMAIL,hInstance,0);
	
	CreateWindow(classSTATIC,  "系统默认只收取新邮件)",WS_CHILD | WS_VISIBLE ,20,55,185,18,hWnd,(HMENU)0XFFFF,hInstance,0);

	// 邮件备份
//	bEnable = GetEmailBackup();
	if (g_EmailCfg.iSaveNum== 0)
		bEnable = FALSE;
	else
		bEnable = TRUE;
	bEnable = TRUE;
	hChild = CreateWindow(classBUTTON,  "备份",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,80,50,18,hWnd,(HMENU)IDC_BACKUPNUMENABLE,hInstance,0);
	
	hComboBox = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,  "",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,58,80,40,68,hWnd,(HMENU)IDC_COMBOBOX,hInstance,0);
	// 设置备份邮件数目的可选项
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"1封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"2封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"3封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"4封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"5封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"6封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"7封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"8封");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"9封");
	
	//ibackupNum = GetEmailBackupNum();
	ibackupNum = 5; // 设置初始化化的数目

	iIndex = ibackupNum - 1;

	SendMessage(hComboBox,CB_SETCURSEL,iIndex,0);
	
	CreateWindow(classSTATIC,  "发送过的邮件",WS_CHILD | WS_VISIBLE ,100,80,100,18,hWnd,(HMENU)0xffff,hInstance,0);
    bEnable = FALSE;
	if ( g_EmailCfg.Interval )
	{
		CHAR szText[20];
		bEnable = TRUE;
		sprintf(szText,"%d",g_EmailCfg.Interval );
		SetWindowText( GetDlgItem(hWnd,IDC_AUTORECEIVETIME),szText ); // 设置自动收发的时间
	}	
	SendMessage(GetDlgItem( hWnd,IDC_AUTORECEIVEENABLE ),BM_SETCHECK,bEnable,0);  // 设置自动收发的状态
	EnableWindow(GetDlgItem( hWnd,IDC_UPDOWN) ,bEnable);
	
	if ( g_EmailCfg.bDownAll )
		SendMessage(GetDlgItem( hWnd,IDC_DOWNLOADALLEMAIL ),BM_SETCHECK,TRUE,0); // 设置自动下载邮件的状态
    
	bEnable = FALSE;
	if ( g_EmailCfg.iSaveNum )
	{
		SendMessage( GetDlgItem ( hWnd,IDC_COMBOBOX ),CB_SETCURSEL,g_EmailCfg.iSaveNum-1 ,NULL ); // 设置备份邮件的数目
		bEnable = TRUE;
		SendMessage(GetDlgItem( hWnd,IDC_BACKUPNUMENABLE ),BM_SETCHECK,bEnable,0); // 设置邮件备份的状态
	}
	EnableWindow(GetDlgItem( hWnd,IDC_COMBOBOX) ,bEnable);
    	

	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoEmailSendRevOK(HWND hDlg)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：邮件收发WM_OK处理
// 引用: 
// ********************************************************************
static LRESULT DoEmailSendRevOK(HWND hDlg)
{   
  	CHAR szText[5];
	HWND hTemp;
    int i ;
    hTemp = GetDlgItem( hDlg, IDC_AUTORECEIVEENABLE ); // 得到自动收发的状态
	
	if (SendMessage( hTemp , BM_GETCHECK ,0,0 ) ){
		memset( szText, 0, 5 );
        GetWindowText( GetDlgItem( hDlg, IDC_AUTORECEIVETIME),szText,4); // 得到自动收发的时间
	    g_EmailCfg.Interval = atoi( szText );	 // 设置时间
	}
	else {
	    g_EmailCfg.Interval = 0;
	}
    
	if ( SendMessage( GetDlgItem( hDlg, IDC_DOWNLOADALLEMAIL),BM_GETCHECK ,0,0 ) ) // 得到下载全部下载邮件的状态
	{
	    g_EmailCfg.bDownAll = TRUE;
	}
	else
		g_EmailCfg.bDownAll = FALSE;

     hTemp = GetDlgItem( hDlg, IDC_BACKUPNUMENABLE ); // 得到备份邮件数目的状态的窗口
	
	if (SendMessage( hTemp , BM_GETCHECK ,0,0 ) ) // 得到备份邮件数目的状态
	{
		if ( (i = (int)SendMessage(GetDlgItem(hDlg,IDC_COMBOBOX),CB_GETCURSEL,0,0 )) != CB_ERR ) // 得到邮件的备份数目
			g_EmailCfg.iSaveNum = i+1; // 设置数目
	}
	else {
	    g_EmailCfg.iSaveNum = 0;
	}

		return TRUE;
}


// ********************************************************************
// 声明：static LRESULT DoAutoReceiveEnable(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：邮件收发处理AUTORECEIVEENABLE 功能改变
// 引用: 
// ********************************************************************
static LRESULT DoAutoReceiveEnable(HWND hWnd)
{
	HWND hAutoReceiveEnable ;
	BOOL bEnable;
	HWND hEdit, hUpDown;

	hAutoReceiveEnable = GetDlgItem(hWnd,IDC_AUTORECEIVEENABLE); // 得到自动收发邮件的状态窗口
	bEnable = SendMessage(hAutoReceiveEnable,BM_GETCHECK,0,0); // 得到自动收发邮件的状态
	hEdit = GetDlgItem(hWnd,IDC_AUTORECEIVETIME); // 得到接收时间的窗口
	hUpDown = GetDlgItem(hWnd,IDC_UPDOWN); // 得到UODOWN的窗口
	
	EnableWindow(hEdit,bEnable); // 设置接收时间的窗口属性与自动收发邮件的状态一致
	EnableWindow(hUpDown,bEnable); // 设置UODOWN的窗口属性与自动收发邮件的状态一致
	return 0;
}


// ********************************************************************
// 声明：static LRESULT DoBackupNumEnable(HWND hWnd)
// 参数：
//	IN hWnd - 窗口句柄
// 返回值：
//	成功返回 TRUE，否则返回FALSE
// 功能描述：邮件收发处理 BACKUPNUMENABLE 功能改变
// 引用: 
// ********************************************************************
static LRESULT DoBackupNumEnable(HWND hWnd)
{
	HWND hBackupNumEnable ;
	BOOL bEnable;
	HWND hComboBox;

	hBackupNumEnable = GetDlgItem(hWnd,IDC_BACKUPNUMENABLE); // 得到备份邮件数目的状态窗口
	bEnable = SendMessage(hBackupNumEnable,BM_GETCHECK,0,0);  // 得到备份邮件数目的状态
	hComboBox = GetDlgItem(hWnd,IDC_COMBOBOX); // 得到组合框的窗口
	EnableWindow(hComboBox,bEnable); // 设置组合框的窗口属性与备份邮件数目的状态一致
	return 0;
}
//================================= 邮件规则=========================================
#define IDC_SERVERSAVE   101
#define IDC_SIZELIMIT    102
#define IDC_SIZESELITEM  103

// **************************************************
// 声明：static LRESULT DoEmailRulerCreate( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回0，否则返回-1
// 功能描述：创建邮件规则窗口。
// 引用: 
// **************************************************
static LRESULT DoEmailRulerCreate( HWND hWnd )
{
    HINSTANCE hInstance;
	HWND hComboBox;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	// 创建服务器保留邮件备份的状态窗口
	CreateWindow( "button","服务器中保留邮件备份",WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX,
		          5,5,200,18,hWnd,(HMENU)IDC_SERVERSAVE,hInstance,NULL);
	// 创建限制邮件下载的选择窗口
	CreateWindow( "button","限制",WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX,
		          5,30,50,20,hWnd,(HMENU)IDC_SIZELIMIT,hInstance,NULL );
	hComboBox = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,  "",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,56,30,50,70,hWnd,(HMENU)IDC_SIZESELITEM,hInstance,0);
	CreateWindow( "STATIC","的邮件完全",WS_VISIBLE|WS_CHILD,110,30,100,20,hWnd,NULL,
		          hInstance,NULL );
	CreateWindow( "STATIC","下载,只保留邮件头信息",WS_VISIBLE|WS_CHILD,10,50,200,20,hWnd,NULL,
		          hInstance,NULL );
	// 设置服务器保留邮件备份的状态
	if ( g_EmailCfg.bDel )
		SendMessage( GetDlgItem( hWnd,IDC_SERVERSAVE ),BM_SETCHECK,FALSE ,0);
	else
		SendMessage( GetDlgItem( hWnd,IDC_SERVERSAVE ),BM_SETCHECK,TRUE ,0);
	// 设置限制选择的大小
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"200k");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"500k");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"1m");
	SendMessage(hComboBox,CB_SETCURSEL,0,0); // 设置当前选择
	
	// 设置限制大小的状态
	if ( g_EmailCfg.iSizeLimit == 0 )
	{ // 没有限制
	   SendMessage(GetDlgItem( hWnd,IDC_SIZELIMIT ),BM_SETCHECK,FALSE,0);
	   EnableWindow( hComboBox,TRUE );
	}
	else
	{ // 有限制
  	 SendMessage(GetDlgItem( hWnd,IDC_SIZELIMIT),BM_SETCHECK,TRUE,0); // 设置有限制
	   EnableWindow(hComboBox,TRUE);
	   SendMessage( hComboBox,CB_SETCURSEL,(g_EmailCfg.iSizeLimit-1)%3,0); // 设置限制大小
	}
	return 0;
}
// **************************************************
// 声明：static BOOL DoEmailRulerOK( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：处理OK消息。
// 引用: 
// **************************************************
static BOOL DoEmailRulerOK( HWND hWnd )
{
	int i =0;
	if (SendMessage( GetDlgItem( hWnd,IDC_SERVERSAVE) , BM_GETCHECK ,0,0 ) ) // 得到服务器保留邮件备份的状态
	{ // 要保留
		g_EmailCfg.bDel = FALSE;
	}
	else 
	{ // 不要保留
	    g_EmailCfg.bDel = TRUE;
	}
	if ( SendMessage( GetDlgItem( hWnd,IDC_SIZELIMIT) , BM_GETCHECK ,0,0 ) ) // 得到限制邮件大小的状态
	{  // 要限制
	   	if ( (i = (int)SendMessage(GetDlgItem(hWnd,IDC_SIZESELITEM ),CB_GETCURSEL,0,0 )) != CB_ERR ) // 得到限制大小
			g_EmailCfg.iSizeLimit = i+1; // 设置限制
	}
	else 
	{ // 不要限制
	    g_EmailCfg.iSizeLimit = 0;
	}
	
	return TRUE;
}
// ********************************************************************
// 声明：static LRESULT CALLBACK EmailRulerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：邮件规则对话框的窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK EmailRulerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"EmailUser",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // 创建消息
			return DoEmailRulerCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			break;
		case WM_OK: // OK消息
			return DoEmailRulerOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// **************************************************
// 声明：static ATOM RegisterEmailRuler(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值： 返回系统注册结果
// 功能描述：注册邮件规则窗口类。
// 引用: 
// **************************************************
static ATOM RegisterEmailRuler(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailRulerClassName; // 窗口类名
   wc.lpfnWndProc	=(WNDPROC)EmailRulerWndProc; // 窗口过程函数
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // 背景刷
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // 注册类
}
//================================= 邮件规则=========================================