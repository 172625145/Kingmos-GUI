/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：应用程序管理，用户界面部分
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

typedef struct ApManageStruct{
	LPTSTR lpApName;
	LPTSTR lpApTitle;
	UINT   idICON;
} APMANAGEITEM;

#ifdef INLINE_PROGRAM
static APMANAGEITEM ApManageList[] = {
	{"PlanDay","日程计划",IDI_CALENDAR},
	{"contactbook","联系本",IDI_NAMECARD},
	{"NoteBook","记事本",IDI_NOTEBOOK},
	{"IExplore","IE浏览",IDI_INTERNET},
	{"Alarm","闹铃",IDI_ALARM},
	{"Mobilephone","移动电话",IDI_PHONE},
	{"EasyBox","邮箱",IDI_EASYBOX},
	{"calculate","计算器",IDI_CALCULATOR},
	{"Dict","英汉字典",IDI_DICTIONARY},
	{"MPlayer","MP3播放",IDI_MP3},
	{"RussBlk","溜方块",IDI_RUSSBLOCK},
	{"FlyBat","飞霸",IDI_PLANE},
	{"Syncsrv","同步服务",IDI_SYNCSRV},
	{"midp","J2ME",IDI_J2ME},
};
#else
static APMANAGEITEM ApManageList[] = {
	{"\\kingmos\\PlanDay.exe","日程计划",IDI_CALENDAR},
	{"\\kingmos\\contactbook.exe","联系本",IDI_NAMECARD},
	{"\\kingmos\\NoteBook.exe","记事本",IDI_NOTEBOOK},
	{"\\kingmos\\IExplore.exe","IE浏览",IDI_INTERNET},
	{"\\kingmos\\Alarm.exe","闹铃",IDI_ALARM},
	{"\\kingmos\\Mobilephone.exe","移动电话",IDI_PHONE},
	{"\\kingmos\\EasyBox.exe","邮箱",IDI_EASYBOX},
	{"\\kingmos\\calca.exe","计算器",IDI_CALCULATOR},
	{"\\kingmos\\Dict.exe","英汉字典",IDI_DICTIONARY},
	{"\\kingmos\\Mp3Player.exe","MP3播放",IDI_MP3},
	{"\\kingmos\\RussBlk.exe","溜方块",IDI_RUSSBLOCK},
	{"\\kingmos\\FlyBat.exe","飞霸",IDI_PLANE},
	{"\\kingmos\\Syncsrv.exe","同步服务",IDI_SYNCSRV},
	{"\\kingmos\\midp.exe","J2ME",IDI_J2ME},
};
#endif

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "应用程序";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "ApplicationManage";								// The title bar text

static ATOM RegisterApplicationManageClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK ApplicationManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);


static HWND CreateListView(HWND  hWnd);
static void	LoadApplicationManageImage(HWND hWnd,HWND hListView);
static BOOL CreateListViewColumn( HWND  hListView);
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,LPTSTR lpApName,int iImage);

// ********************************************************************
// 声明：void WinMain_ApplicationManage(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_ApplicationManage(HINSTANCE hInstance,
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

	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找应用程序是否存在
	if( hWnd != 0 )
	{ // 应用程序已经存在
		SetForegroundWindow( hWnd ) ; // 设置到最前台
		return FALSE ;
	}
	InitCommonControls(); // 初始化通用控件
	RegisterApplicationManageClass(hInstance); // 注册应用程序类

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
// 声明：ATOM RegisterApplicationManageClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：注册当前应用程序的类
// 引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterApplicationManageClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)ApplicationManageWndProc; // 应用程序过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance; // 实例句柄
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_APMANAGE),IMAGE_ICON,16,16,0); // 应用程序图标
//	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APPLICATION),IMAGE_ICON,16,16,0);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 应用程序类名

	return RegisterClass(&wc); // 注册应用程序类
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
   // 创建应用程序主窗口
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, szTitle, WS_SYSMENU | WS_VISIBLE,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // 创建窗口失败
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);

   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 更新窗口

   return TRUE;
}

// ********************************************************************
// 声明：static LRESULT CALLBACK ApplicationManageWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK ApplicationManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_DESTROY: // 破坏窗口
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
	CreateListView(hWnd); // 创建一个LISTVIEW
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
	LPTSTR lpApName;

		lpnmia=(LPNMITEMACTIVATE)lParam; // 得到条目激活的结构参数
		
		lvItem.mask  = LVIF_PARAM; // 要求得到条目参数
		lvItem.iItem=lpnmia->iItem; // 得到条目索引
		lvItem.iSubItem=0;
		SendMessage(lpnmia->hdr.hwndFrom,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // 得到条目参数
		lpApName = (LPTSTR)lvItem.lParam; // 是应用程序的名称，有用户在创建的时候设置
		if (lpApName)
		{
/*			if (strcmp(lpApName, ApManageList[9].lpApName) == 0)
			{
				LoadApplication(lpApName,NULL);
				return 0;
			}
*/			
#ifdef INLINE_PROGRAM
			LoadApplication(lpApName,NULL); // 运行应用程序
#else
			CreateProcess(lpApName,0,0,0,0,0,0,0,0,0); // 运行应用程序
#endif
		}
		return 0;
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

	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt); // 得到窗口客户矩形
	// 创建LISTVIEW窗口
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
	SendMessage(hListView,LVM_SETICONSPACING,0,MAKELONG(20,13)); // 设置间隔
	// 设置颜色
	RETAILMSG(1,("Will Set ListView Color...\r\n"));
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
	LoadApplicationManageImage(hWnd,hListView); // 装载应用程序图标到LISTVIEW
	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView ); // 创建LISTVIEW列
	RETAILMSG(1,("Will Insert Data...\r\n"));
	// 将各个应用程序插入到LISTVIEW
	nApNumber = sizeof(ApManageList)/sizeof(APMANAGEITEM);
	for (i=0; i<nApNumber; i++)
	{
		InsertListViewItem(hListView,ApManageList[i].lpApTitle, ApManageList[i].lpApName,i); // 插入条目
	}
	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView; // 返回窗口句柄
}

// ********************************************************************
// 声明：static void	LoadApplicationManageImage(HWND hWnd,HWND hListView)
// 参数：
//	IN hWnd - FileBrowser的窗口句柄
//	IN hListView - ListView的窗口句柄
// 返回值：
//	无
// 功能描述：给ListView控件中添加一个 ImageList
// 引用: 
// ********************************************************************
static void	LoadApplicationManageImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
	HINSTANCE hInstance;
	HIMAGELIST hImageList;
	int nApNumber,i;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄


		hImageList = ImageList_Create(32,32,ICL_ICON,8,8); // 创建图象列表

		if (hImageList == NULL) //创建ImageList失败
				return ;
		
		nApNumber = sizeof(ApManageList)/sizeof(APMANAGEITEM); // 得到应用程序个数
		// 装载应用程序图标
		for (i=0; i<nApNumber; i++)
		{
//			hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( ApManageList[i].idICON ), IMAGE_ICON,
			hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( ApManageList[i].idICON ), IMAGE_ICON,
				32, 32, 0 ) ; // 装载应用程序图标
			if( hIcon != 0 )
				ImageList_AddIcon( hImageList  , hIcon ) ; // 插入到图象列表
		}


		SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL, (LPARAM)hImageList ); // 发送消息个LISTVIEW

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
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE; // 有图象和文本左对齐
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax =1;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 100;     // width of column.
	if( SendMessage(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn)==-1 ) // 插入列
	{
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
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,LPTSTR lpApName,int iImage)
{
	LV_ITEM				lvitem;
	int iItem;

	lvitem.mask			= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iItem		= 0xffff;     // the item index,where the item shows. 
	lvitem.iSubItem		= 0;
	lvitem.pszText		= lpApTitle; // 条目文本
	lvitem.cchTextMax	= strlen(lpApTitle ); // 文本长度
	lvitem.iImage		= iImage; // 图象索引
	lvitem.lParam		= (LPARAM)lpApName; // 应用程序名称

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // 插入条目
	
	return iItem; // 返回条目索引
}
