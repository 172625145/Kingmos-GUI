/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：系统设定程序管理，用户界面部分
版本号：1.0.0
开发时期：2003-06-19
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

#define ID_SYSTEM		1001
#define ID_DISPLAY		1002
#define ID_EMAILSET		1003
#define ID_IEOPTION		1004
#define ID_STYLUS		1005
#define ID_VOLUME		1006
#define ID_POWER		1007
#define ID_DATETIME		1008
#define ID_GPRSSET		1009
#define ID_NETSETTING	1010

typedef struct SystemSettingStruct{
	DWORD dwIndex;
	LPTSTR lpApTitle;
	UINT   idICON;
} SYSTEMSETTINGITEM;

static SYSTEMSETTINGITEM SystemSettingList[] ={
	{ID_SYSTEM,"系统",IDI_SYSTEM},
	{ID_DISPLAY,"显示",IDI_DISPLAY},
	{ID_EMAILSET,"邮件配置",IDI_EMAILSET},
	{ID_IEOPTION,"IE选项",IDI_INTERNET},
	{ID_STYLUS,"笔针",IDI_STYLUS},
	{ID_GPRSSET,"GPRS",IDI_GPRSSET},
	{ID_POWER,"电源",IDI_POWER},
	{ID_DATETIME,"日期时间",IDI_DATETIME},
	{ID_VOLUME,"音量",IDI_VOLUME},
	{ID_NETSETTING,"网络",IDI_NETSET}
};

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "系统设定";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "SystemSetting";								// The title bar text

static ATOM RegisterSystemSettingClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK SystemSettingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);


static HWND CreateListView(HWND  hWnd);
static void	LoadSystemSettingImage(HWND hWnd,HWND hListView);
static BOOL CreateListViewColumn( HWND  hListView);
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,DWORD dwIndex,int iImage);

static void CreateSysProperty(HINSTANCE hInstqance,HWND hWnd,DWORD dwIndex);

// ********************************************************************
extern void DoSystemProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoDisplayProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoEmailProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoIEOption(HINSTANCE hInstance,HWND hWnd);
extern void DoStylusProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoSoundProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoPowerProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoDateTimeProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoGPRSSet(HINSTANCE hInstance,HWND hWnd);
extern void DoNetSetting(HINSTANCE hInstance,HWND hWnd);


// ********************************************************************
// 声明：void WinMain_SystemSetting(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
//    IN hPrevInstance   - 前一个应用程序的实例句柄
//    IN lpCmdLine   - 调用应用程序时的参数
//    IN nCmdShow  - 应用程序显示命令
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：装载应用程序
// 引用: 被 系统 调用
// ********************************************************************
#ifdef INLINE_PROGRAM
int WINAPI WinMain_SystemSetting(HINSTANCE hInstance,
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

	if ((lpCmdLine != NULL) && strlen(lpCmdLine) != 0)
	{ // 有参数
		InitCommonControls(); // 初始化通用控件
		if (stricmp(lpCmdLine,"datetime") == 0)
		{ // 要求运行“日期/时间”功能
			CreateSysProperty(hInstance,NULL,ID_DATETIME); // 创建“日期/时间”
		}
		if (stricmp(lpCmdLine,"BatteryProperty") == 0)
		{ // 要求运行“电池属性”
			CreateSysProperty(hInstance,NULL,ID_POWER); // 创建“电池属性”
		}			
		if (stricmp(lpCmdLine,"EmailProperty") == 0)
		{ // 要求运行“邮件属性”
			CreateSysProperty(hInstance,NULL,ID_EMAILSET); // 创建邮件属性
		}			
		return 1; // 不要求运行系统设置主画面
	}
	hWnd = FindWindow( szWindowClass, NULL ) ;  // 系统设置是否已经运行
	if( hWnd != 0 )
	{ // 已经运行
		SetForegroundWindow( hWnd ) ; // 设置到前台
		return FALSE ;
	}
	RegisterSystemSettingClass(hInstance); // 注册系统设置窗口类

	InitCommonControls(); // 初始化通用控件
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

	return msg.wParam;
}

// ********************************************************************
// 声明：ATOM RegisterSystemSettingClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：注册当前应用程序的类
// 引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterSystemSettingClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)SystemSettingWndProc; // 窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_SETTING),IMAGE_ICON,16,16,0); // 应用程序图标
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH); // 背景刷
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 窗口类名

	return RegisterClass(&wc); // 注册窗口类
}


// ********************************************************************
// 声明：static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
//    IN nCmdShow  - 应用程序显示命令
// 返回值：
//	成功返回TRUE, 失败返回FALSE
// 功能描述：初始化当前实例，即创建当前主窗口
// 引用: 
// ********************************************************************
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;


   hInst = hInstance; // 保留当前的实例句柄
	// 创建主窗口
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, szTitle, WS_SYSMENU | WS_VISIBLE,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // 创建失败
      return FALSE;
   }


   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 更新窗口

   return TRUE;
}

// ********************************************************************
// 声明：static LRESULT CALLBACK SystemSettingWndProc(HWND , UINT , WPARAM , LPARAM )
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	不同的消息有不同的返回值，具体看消息本身
// 功能描述：应用程序窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK SystemSettingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
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
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// ********************************************************************
// 声明：static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回0，将继续创建窗口，返回-1，则会破坏窗口
// 功能描述：应用程序处理创建消息
// 引用: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CreateListView(hWnd); // 创建LISTVIEW
	return 0;
}

// ********************************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理通知消息
// 引用: 
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
// 声明：static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理 EXN_ACTIVEITEM 通知消息
// 引用: 
// ********************************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMITEMACTIVATE lpnmia;
	LVITEM	lvItem;
	DWORD dwIndex;
	HINSTANCE hInstance;

		lpnmia=(LPNMITEMACTIVATE)lParam; // 得到条目激活的结构
		
		lvItem.mask  = LVIF_PARAM; // 要求得到条目参数
		lvItem.iItem=lpnmia->iItem; // 得到激活的条目
		lvItem.iSubItem=0;
		SendMessage(lpnmia->hdr.hwndFrom,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // 得到条目内容
		dwIndex = lvItem.lParam; // 得到属性的索引，条目参数就是属性索引，加入条目时设置
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		CreateSysProperty(hInstance,hWnd,dwIndex); // 创建系统属性窗口
		return 0;
}


// **************************************************
// 声明：static void CreateSysProperty(HINSTANCE hInstance,HWND hWnd,DWORD dwIndex)
// 参数：
// 	IN hInstance -- 实例句柄
// 	IN hWnd -- 要创建的属性的父窗口句柄
// 	IN dwIndex -- 属性索引
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static void CreateSysProperty(HINSTANCE hInstance,HWND hWnd,DWORD dwIndex)
{
		switch (dwIndex)
		{
			case ID_SYSTEM: // 创建系统属性
				DoSystemProperty(hInstance,hWnd);
				break;
			case ID_DISPLAY: // 创建显示属性
				DoDisplayProperty(hInstance,hWnd);
				break;
			case ID_EMAILSET: // 创建邮件设置属性
				DoEmailProperty(hInstance,hWnd);
				break;
			case ID_IEOPTION: // 创建IE选项
				DoIEOption(hInstance,hWnd);
				break;
			case ID_STYLUS: // 创建点笔属性
				DoStylusProperty(hInstance,hWnd);
				break;
			case ID_VOLUME: // 创建声音属性
				DoSoundProperty(hInstance,hWnd);
				break;
			case ID_POWER: // 创建电源属性
				DoPowerProperty(hInstance,hWnd);
				break;
			case ID_DATETIME: // 创建日期时间属性
				DoDateTimeProperty(hInstance,hWnd);
				break;
			case ID_GPRSSET: // 创建GPRS属性
				DoGPRSSet(hInstance,hWnd);
				break;
			case ID_NETSETTING: // 创建网络设置属性
				DoNetSetting(hInstance,hWnd);
				break;
		}
}


// ********************************************************************
// 声明：static HWND  CreateListView(HWND  hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	成功返回ListView的窗口句柄，否则返回NULL
// 功能描述：创建一个ListView控件
// 引用: 
// ********************************************************************
static HWND  CreateListView(HWND  hWnd)
{
	RECT						rt;
	HWND						hListView;
	CTLCOLORSTRUCT stCtlColor;
	int nApNumber,i;

//	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt); // 得到窗口客户区域
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



//	RETAILMSG(1,("Will Set ListView specing...\r\n"));
	SendMessage(hListView,LVM_SETICONSPACING,0,MAKELONG(20,13)); // 设置间隔
	// 设置颜色
//	RETAILMSG(1,("Will Set ListView Color...\r\n"));
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR ;//| CLF_TITLECOLOR | CLF_TITLEBKCOLOR;
	stCtlColor.cl_Text = RGB(78,81,78);
//	stCtlColor.cl_TextBk = RGB(247,255,247);
	stCtlColor.cl_TextBk = RGB(255,255,255);
	stCtlColor.cl_Selection = RGB(255,255,255);
	stCtlColor.cl_SelectionBk = RGB(77,166,255);
//	stCtlColor.cl_Title = RGB(78,81,78);
//	stCtlColor.cl_TitleBk = RGB(247,255,247);
	SendMessage(hListView,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

//	RETAILMSG(1,("Will LoadImage...\r\n"));
	LoadSystemSettingImage(hWnd,hListView); // 装载系统设置图象列表
//	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView ); // 创建LISTVIEW列数据
//	RETAILMSG(1,("Will Insert Data...\r\n"));

	nApNumber = sizeof(SystemSettingList)/sizeof(SYSTEMSETTINGITEM); // 得到要插入的条目个数
	for (i=0; i<nApNumber; i++)
	{
		InsertListViewItem(hListView,SystemSettingList[i].lpApTitle, SystemSettingList[i].dwIndex,i); // 插入条目
	}
//	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView;
}

// ********************************************************************
// 声明：static void	LoadSystemSettingImage(HWND hWnd,HWND hListView)
// 参数：
//	IN hWnd - FileBrowser的窗口句柄
//	IN hListView - ListView的窗口句柄
// 返回值：
//	无
// 功能描述：给ListView控件中添加一个 ImageList
// 引用: 
// ********************************************************************
static void	LoadSystemSettingImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
	HINSTANCE hInstance;
	HIMAGELIST hImageList;
	int nApNumber,i;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄


		hImageList = ImageList_Create(32,32,ICL_ICON,8,8); // 创建图象列表

		if (hImageList == NULL) //创建ImageList失败
				return ;
		
		nApNumber = sizeof(SystemSettingList)/sizeof(SYSTEMSETTINGITEM); // 得到条目个数
		for (i=0; i<nApNumber; i++)
		{	// 装载图标
			hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( SystemSettingList[i].idICON ), IMAGE_ICON,
				32, 32, 0 ) ;
			//if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ; // 插入图标到图象列表
		}


		SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL, (LPARAM)hImageList ); // 设置图象列表到LISTVIEW

}

// ********************************************************************
// 声明：static BOOL  CreateListViewColumn( HWND  hListView )
// 参数：
//	IN hListView - ListView的窗口句柄
// 返回值：
//	无
// 功能描述：在ListView控件中添加列项目
// 引用: 
// ********************************************************************
static BOOL  CreateListViewColumn( HWND  hListView)
{
	LV_COLUMN		lvcolumn;

	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE; // 要求有图标和文本左对齐
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax =1;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 100;     // width of column.
	if( SendMessage(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn)==-1 ) // 插入一列
	{ // 插入列失败
		return FALSE;
	}
	return TRUE;
}
// ********************************************************************
// 声明：static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage)
// 参数：
//	IN hListView- ListView的窗口句柄
//    IN lpApTitle  - 要添加应用程序的标题
//    IN lpApName   - 要添加应用程序的名称
//    IN iImage  - 要添加条目的Image位置
// 返回值：
//	返回插入条目的Index
// 功能描述：在ListView控件中添加一个条目
// 引用: 
// ********************************************************************
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,DWORD dwIndex,int iImage)
{
	LV_ITEM				lvitem;
	int iItem;

	lvitem.mask			= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iItem		= 0xffff;     // the item index,where the item shows. 
	lvitem.iSubItem		= 0;
	lvitem.pszText		= lpApTitle; // 设置条目文本
	lvitem.cchTextMax	= strlen(lpApTitle );
	lvitem.iImage		= iImage; // 设置条目图象索引
	lvitem.lParam		= dwIndex; // 设置属性索引

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // 插入条目
	
	return iItem; // 返回条目索引
}


// **************************************************
// 声明：BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// 参数：
// 	IN hRoot -- 根键
// 	IN lpKey -- 键名
// 	IN lpValueName -- 键值
// 	OUT lpData -- 数据缓存
// 	IN dwDataLen -- 数据缓存的大小
// 	IN dwType -- 数据类型
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：读取指定的键值的值。
// 引用: 
// **************************************************
BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegOpenKeyEx(hRoot, lpKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) // 打开指定的键
	 { // 打开成功
		 if (RegQueryValueEx(hKey, lpValueName, NULL, &dwType, (LPBYTE)lpData, &dwDataLen) == ERROR_SUCCESS) // 得到键值的值
		 { // 得到数据成功
			RegCloseKey(hKey); // 关闭键
			return TRUE;
		 }
		 RegCloseKey(hKey); // 关闭键
	 }
	 return FALSE;
}

// **************************************************
// 声明：BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// 参数：
// 	IN hRoot -- 根键
// 	IN lpKey -- 键名
// 	IN lpValueName -- 键值
// 	IN lpData -- 数据缓存
// 	IN dwDataLen -- 数据缓存的大小
// 	IN dwType -- 数据类型
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：读取指定的键值的值。
// 引用: 
// **************************************************
BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegCreateKeyEx(hRoot, lpKey, 0,NULL,0, KEY_ALL_ACCESS, NULL,&hKey,0) == ERROR_SUCCESS) // 打开指定的键
	 { // 打开成功
		 if (RegSetValueEx(hKey, lpValueName, NULL, (DWORD)dwType, (LPBYTE)lpData, dwDataLen) == ERROR_SUCCESS) // 设置指定键值的值
		 { // 设置成功
			 RegCloseKey(hKey); // 关闭键值
			 return TRUE;
		 }
		 RegCloseKey(hKey); // 关闭键值
	 }
	 return FALSE;
}
