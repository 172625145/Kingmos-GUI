/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：设备管理，用户界面部分
版本号：1.0.0
开发时期：2003-06-18
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <Etoolbar.h>
#include "resource.h"

/***************  全局区 定义， 声明 *****************/
#define MAX_LOADSTRING  100
#define MAX_SEARCHSTRINGLEN	64

#define ID_LISTVIEW  301

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "设备管理";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "DeviceManage";								// The title bar text

ATOM RegisterDeviceManageClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK DeviceManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);


static HWND  CreateListView(HWND  hWnd);
static void	LoadDeviceMangeImage(HWND hWnd,HWND hListView);
static BOOL  CreateListViewColumn( HWND  hListView);
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage);

// ********************************************************************
//声明：void WinMain_DeviceManage(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_DeviceManage(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
 	// TODO: Place code here.
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找当前程序是否已经运行
	if( hWnd != 0 )
	{ // 已经运行
		SetForegroundWindow( hWnd ) ; // 设置窗口到最前面
		return FALSE ;
	}
	InitCommonControls(); // 初始化通用控件
	RegisterDeviceManageClass(hInstance); // 注册设备管理类

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))  // 初始化应用程序
	{
		return FALSE;
	}


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam; // 退出应用程序
}

// ********************************************************************
//声明：ATOM RegisterDeviceManageClass(HINSTANCE hInstance)
//参数：
//	IN hInstance - 当前应用程序的实例句柄
//返回值：
//	成功，返回非零，不成功，返回零。
//功能描述：注册当前应用程序的类
//引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterDeviceManageClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)DeviceManageWndProc; // 设备管理窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
//	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_DEVICEMANAGE),IMAGE_ICON,16,16,0);
	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// 装载应用程序图标
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH); // 设置背景刷
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 设备管理窗口类名

	return RegisterClass(&wc); // 注册类
}


// ********************************************************************
//声明：static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//参数：
//	IN hInstance - 当前应用程序的实例句柄
//  IN nCmdShow  - 应用程序显示命令
//返回值：
//	成功返回TRUE, 失败返回FALSE
//功能描述：初始化当前实例，即创建当前主窗口
//引用: 
// ********************************************************************
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;


   hInst = hInstance; // 保留当前的实例句柄

   // 创建窗口
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, szTitle, WS_SYSMENU | WS_VISIBLE,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // 创建窗口失败
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);
	// 创建窗口成功
   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 显示窗口

   return TRUE;
}

// ********************************************************************
//声明：static LRESULT CALLBACK DeviceManageWndProc(HWND , UINT , WPARAM , LPARAM )
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN message - 过程消息
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	不同的消息有不同的返回值，具体看消息本身
//功能描述：应用程序窗口过程函数
//引用: 
// ********************************************************************
static LRESULT CALLBACK DeviceManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // 绘制窗口
			hdc = BeginPaint(hWnd, &ps);
//			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_NOTIFY: // 通知消息
			return DoNotify(hWnd,wParam,lParam);
		case WM_CREATE: // 创建消息
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏消息
//			DoDestroyWindow(hWnd);
			PostQuitMessage(0); // 退出程序
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// ********************************************************************
//声明：static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	返回0，将继续创建窗口，返回-1，则会破坏窗口
//功能描述：应用程序处理创建消息
//引用: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CreateListView(hWnd); // 创建一个列式视
	return 0;
}

// ********************************************************************
//声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	
//功能描述：应用程序处理通知消息
//引用: 
// ********************************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	NMLISTVIEW *pnmvl;
	NMHDR   *hdr=(NMHDR   *)lParam;    

		switch(hdr->code)
		{
			 case LVN_ITEMACTIVATE: // 条目激活
				 DoActiveItem(hWnd,wParam,lParam);
				 break;
		}
		return 0;
}

// ********************************************************************
//声明：static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
//参数：
//	IN hWnd- 应用程序的窗口句柄
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
//返回值：
//	
//功能描述：应用程序处理 EXN_ACTIVEITEM 通知消息
//引用: 
// ********************************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMITEMACTIVATE lpnmia;

		lpnmia=(LPNMITEMACTIVATE)lParam;
		switch(lpnmia->iItem)
		{
			case 0: // 文件浏览处理，// 调用文件浏览应用程序
#ifdef INLINE_PROGRAM
				LoadApplication("FileView",NULL); 
#else
				CreateProcess("\\kingmos\\FileView.exe",0,0,0,0,0,0,0,0,0);
#endif
				break;
			case 1: // 应用程序管理，调用应用程序管理程序
#ifdef INLINE_PROGRAM
				LoadApplication("ApManage",NULL);
#else
				CreateProcess("\\kingmos\\ApManage.exe",0,0,0,0,0,0,0,0,0);
#endif
				break;
			case 2: // 系统设定，调用系统设定应用程序
#ifdef INLINE_PROGRAM
				LoadApplication("Setting",NULL);
#else
				CreateProcess("\\kingmos\\Setting.exe",0,0,0,0,0,0,0,0,0);
#endif
				break;
		}
		return 0;
}



// ********************************************************************
//声明：static HWND  CreateListView(HWND  hWnd)
//参数：
//	IN hWnd- 应用程序的窗口句柄
//返回值：
//	成功返回ListView的窗口句柄，否则返回NULL
//功能描述：创建一个ListView控件
//引用: 
// ********************************************************************
static HWND  CreateListView(HWND  hWnd)
{
	RECT						rt;
	HWND						hListView;
	CTLCOLORSTRUCT stCtlColor;

	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt); // 得到窗口客户矩形
	// 创建LISTVIEW
	hListView=CreateWindow( classLISTCTRL,"",
				WS_VISIBLE|WS_CHILD|LVS_ICON|LVS_SHOWSELALWAYS|LVS_SINGLESEL| WS_VSCROLL | WS_HSCROLL,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_LISTVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	if (hListView == NULL)
	{ // 创建失败
		RETAILMSG(1,("CreateListView Failure\r\n"));
		return NULL;
	}



	RETAILMSG(1,("Will Set ListView specing...\r\n"));
	SendMessage(hListView,LVM_SETICONSPACING,0,MAKELONG(20,10)); // 设置图标条目之间的间距

	RETAILMSG(1,("Will Set ListView Color...\r\n"));
	// 设置控件颜色
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR ;//| CLF_TITLECOLOR | CLF_TITLEBKCOLOR;
	stCtlColor.cl_Text = RGB(78,81,78);
//	stCtlColor.cl_TextBk = RGB(247,255,247);
	stCtlColor.cl_TextBk = RGB(255,255,255);
	stCtlColor.cl_Selection = RGB(255,255,255);
	stCtlColor.cl_SelectionBk = RGB(77,166,255);
//	stCtlColor.cl_Title = RGB(78,81,78);
//	stCtlColor.cl_TitleBk = RGB(247,255,247);
	SendMessage(hListView,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

	RETAILMSG(1,("Will LoadImage...\r\n"));
	LoadDeviceMangeImage(hWnd,hListView); // 装载设备管理图标
	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView ); // 创建LISTVIEW的列
	RETAILMSG(1,("Will Insert Data...\r\n"));
	InsertListViewItem(hListView,0, 0,"文件管理",0); // 插入“文件管理”条目
	InsertListViewItem(hListView,1, 0,"应用程序",1); // 插入“应用程序”条目
	InsertListViewItem(hListView,2, 0,"系统设定",2); // 插入“系统设定”条目
	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView; // 返回句柄
}

// ********************************************************************
//声明：static void	LoadDeviceMangeImage(HWND hWnd,HWND hListView)
//参数：
//	IN hWnd - FileBrowser的窗口句柄
//	IN hListView - ListView的窗口句柄
//返回值：
//	无
//功能描述：给ListView控件中添加一个 ImageList
//引用: 
// ********************************************************************
static void	LoadDeviceMangeImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
	HINSTANCE hInstance;
	HIMAGELIST hImageList;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // 得到实例句柄


		hImageList = ImageList_Create(32,32,ICL_ICON,8,8); // 创建图象列表

		if (hImageList == NULL) //创建ImageList失败
				return ;
		// 得到文件管理的图标
//		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_FILEMANAGE ), IMAGE_ICON,
//			32, 32, 0 ) ;
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_FILEMANAGE ), IMAGE_ICON,
			32, 32, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ;

		// 得到应用程序管理的图标
//		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_APPMANAGE ), IMAGE_ICON,
//			32, 32, 0 ) ;
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APPLICATION ), IMAGE_ICON,
			32, 32, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ;

		// 得到系统设定的图标
//		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_SYSTEMSETTING ), IMAGE_ICON,
//			32, 32, 0 ) ;
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_CONTROL ), IMAGE_ICON,
			32, 32, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ;

		SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL, (LPARAM)hImageList ); // 设置图象列表到LISTVIEW

}

// ********************************************************************
//声明：static BOOL  CreateListViewColumn( HWND  hListView )
//参数：
//	IN hListView - ListView的窗口句柄
//返回值：
//	无
//功能描述：在ListView控件中添加列项目
//引用: 
// ********************************************************************
static BOOL  CreateListViewColumn( HWND  hListView)
{
	LV_COLUMN		lvcolumn;

	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE; // 有图象，文本左对齐
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax =1;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 100;     // width of column.
	if( SendMessage(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn)==-1 ) // 插入一个条目
	{
		return FALSE;
	}
	return TRUE;
}
// ********************************************************************
//声明：static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage)
//参数：
//	IN hListView- ListView的窗口句柄
//  IN iItem  - 要添加条目的条目位置
//  IN itemSub - 要添加的条目的子条目位置
//  IN lpItem  - 要添加条目的内容
//  IN iImage  - 要添加条目的Image位置
//返回值：
//	返回插入条目的Index
//功能描述：在ListView控件中添加一个条目
//引用: 
// ********************************************************************
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,
							   TCHAR* lpItem,int iImage)
{
	LV_ITEM				lvitem;
	lvitem.mask			= LVIF_TEXT | LVIF_IMAGE; // 文本，图象有效
	lvitem.iItem		= iItem;     // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
	lvitem.cchTextMax	= strlen(lpItem ); // 文本长度
	lvitem.iImage		= iImage; // 图象索引

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // 插入条目
	
	return iItem;
}
