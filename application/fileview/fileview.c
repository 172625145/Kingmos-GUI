/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：文件浏览器，用户界面部分
版本号：1.0.0
开发时期：2003-04-02
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <Etoolbar.h>
#include "resource.h"
#include "Explorer.h"
#include "FindData.h"
#include "eCommdlg.h"
#include "eclipbrd.h"
#include <emmsys.h>

#include <eShell.h>

/***************  全局区 定义， 声明 *****************/
#define MAX_LOADSTRING  100
#define MAX_SEARCHSTRINGLEN	64

#define ID_BROWSER  301
#define ID_TOOLBAR	302

#define IDC_NEWFOLDER	601
#define IDC_COPY		602
#define IDC_PASTE		603
#define IDC_DELETE		604
#define IDC_UP			605
#define IDC_RENAME		606
#define IDC_FIND		607
#define IDC_PROPERTY	608


static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "文件管理";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "FileView";								// The title bar text

typedef struct OpenListStruct{
	LPTSTR lpExtend;
	LPTSTR lpApplication;
} OPENLIST;

typedef OPENLIST * LPOPENLIST;

#ifdef INLINE_PROGRAM
static OPENLIST g_OpenList[] =
{
	{"*.txt",	"NoteBook"},
	{"*.htm",	"IExplore"},
	{"*.bmp",	"NoteBook"},
	{"*.gif",	"IExplore"},
	{"*.mp3",	"MPlayer"},
	{"*.pls",	"MPlayer"},
	{"*.mlg",	"Readbox"},
	{NULL,NULL}
};
#else
static OPENLIST g_OpenList[] =
{
	{"*.txt",	"\\kingmos\\NoteBook.exe"},
	{"*.htm",	"\\kingmos\\IExplore.exe"},
	{"*.bmp",	"\\kingmos\\NoteBook.exe"},
	{"*.gif",	"\\kingmos\\IExplore.exe"},
	{"*.mp3",	"\\kingmos\\Mp3Player.exe"},
	{"*.pls",	"\\kingmos\\Mp3Player.exe"},
	{"*.mlg",	"\\kingmos\\Readbox.exe"},
	{NULL,NULL}
};
#endif

extern ATOM RegisterFileBrowserClass(HINSTANCE hInstance);
extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;


ATOM RegisterFileViewClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK FileViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL CreateFileViewToolbar( HWND hWnd );
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT SetToolBarButtonEnable(HWND hWnd);
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCopy(HWND hWnd);
static LRESULT DoPaste(HWND hWnd);
static LRESULT DoUpPath(HWND hWnd);
static LRESULT DoDelete(HWND hWnd);
static LRESULT DoRename(HWND hWnd);
static LRESULT DoNewFolder(HWND hWnd);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFind(HWND hWnd);
static LRESULT DoProperty(HWND hWnd);
static LRESULT DoShellNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);


static LRESULT CALLBACK FindDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitFindDlg(HWND hDlg,WPARAM wParam,LPARAM lParam);
static LRESULT FindString(HWND hDlg);

static void ShowFileProperty(HWND hWnd,LPFILEPROPERTY lpFileProperty);
static LRESULT CALLBACK ShowPropertyDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitShowProperty(HWND hDlg,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaintShowProperty(HWND hDlg,HDC hdc);


// ********************************************************************
// 声明：void WinMain_FileView(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_FileView(HINSTANCE hInstance,
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

// !!! test
//	SHChangeNotify(SHCNE_UPDATEDIR, 0, 0, 0);
// !!! test end

	hWnd = FindWindow( szWindowClass, NULL ) ;  // 查找窗口是否存在
	if( hWnd != 0 )
	{ // 窗口已经存在
		SetForegroundWindow( hWnd ) ; // 设置窗口到前台
		return FALSE ;
	}
	InitCommonControls(); // 初始化通用控件
	RegisterFileBrowserClass(hInstance); // 注册文件浏览类
	RegisterFileViewClass(hInstance) ; // 注册应用程序类

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
// 声明：ATOM RegisterFileViewClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：注册当前应用程序的类
// 引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterFileViewClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)FileViewWndProc; // 文件浏览窗口过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance; // 实例句柄
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_FILEMANAGE),IMAGE_ICON,16,16,0); // 应用程序位图
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // 应用程序类名

	return RegisterClass(&wc); // 注册类
}


// ********************************************************************
// 声明：static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
//  IN nCmdShow  - 应用程序显示命令
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
   { // 创建失败
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);

   ShowWindow(hWnd, nCmdShow); // 显示窗口
   UpdateWindow(hWnd); // 更新窗口

   return TRUE;
}

// ********************************************************************
// 声明：static LRESULT CALLBACK FileViewWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK FileViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
				case IDC_NEWFOLDER: // 创建一个新的目录
					DoNewFolder(hWnd);
					break;
				case IDC_COPY: // 复制
					DoCopy(hWnd);
					break;
				case IDC_PASTE: // 粘帖
					DoPaste(hWnd);
					break;
				case IDC_UP: // 到上一级目录
					DoUpPath(hWnd);
					break;
				case IDC_DELETE: // 删除
					DoDelete(hWnd);
					break;
				case IDC_RENAME: // 重命名
					DoRename(hWnd);
					break;
				case IDC_FIND: // 查找
					DoFind(hWnd);
					break;
				case IDC_PROPERTY: // 属性
					DoProperty(hWnd);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_KEYDOWN: // 键盘消息
//			DoKeyDown(hWnd,wParam,lParam);
			break;
		case WM_LBUTTONDOWN: // 鼠标左键消息
//			DoLButtonDown(hWnd,wParam,lParam);
			break;
		case WM_NOTIFY: // 通知消息
			return DoNotify(hWnd,wParam,lParam);
		case WM_SHELLNOTIFY: // SHELL通知消息
			return DoShellNotify(hWnd,wParam,lParam);
		case WM_CREATE: // 创建窗口
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏窗口
			DoDestroyWindow(hWnd);
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
//  IN wParam  - 消息参数
//  IN lParam  - 消息参数
// 返回值：
//	返回0，将继续创建窗口，返回-1，则会破坏窗口
// 功能描述：应用程序处理创建消息
// 引用: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CreateFileViewToolbar(hWnd); // 创建工具条
	// 创建文件浏览类
	CreateWindow(classFileBrowser,"",WS_CHILD|WS_VISIBLE,0,TOOLBAR_HIGH,210,255,hWnd,(HMENU)ID_BROWSER,hInst,NULL);
//	CreateWindow(classEDIT,"test abcd",WS_CHILD|WS_VISIBLE|ES_MULTILINE,0,TOOLBAR_HIGH,210,255,hWnd,(HMENU)ID_BROWSER,hInst,NULL);
	return 0;
}

// ********************************************************************
// 声明：static BOOL CreateFileViewToolbar( HWND hWnd )
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	成功 返回TRUE , 否则返回FALSE
// 功能描述：应用程序创建工具栏
// 引用: 
// ********************************************************************
static BOOL CreateFileViewToolbar( HWND hWnd )
{
//	_LPIEDATA	lpied ;
	HWND		hToolbar ;
	TBBUTTON	tb;
	int			i;

typedef struct FileViewToolBar{
	BOOL bSysBmp;
	UINT ID;
	UINT IDResource;
	UINT IDDisRes;
	LPTSTR lpString;
}FILEVIEWTOOLBAR;


	FILEVIEWTOOLBAR FileViewToolBar[]={
		{TRUE,	IDC_NEWFOLDER,	OBM_TOOL_NEW,		0,						"NewFolder"},
		{TRUE,	IDC_COPY,		OBM_TOOL_COPY,		OBM_TOOL_COPY_GRAY,		"Copy"},
		{TRUE,	IDC_PASTE,		OBM_TOOL_PASTE,		OBM_TOOL_PASTE_GRAY,	"Paste"},
		{TRUE,	IDC_DELETE,	OBM_TOOL_DELETE,	OBM_TOOL_DELETE_GRAY,	"Delete"},
		{TRUE,	IDC_UP,		OBM_TOOL_UPFOLDER,	OBM_TOOL_UPFOLDER_GRAY,	"Up"},
		{TRUE,	IDC_RENAME,	OBM_TOOL_RENAME,	OBM_TOOL_RENAME_GRAY,	"Rename"},
		{TRUE,	IDC_FIND,		OBM_TOOL_FIND,		0,						"Find"},
		{FALSE,	IDC_PROPERTY,	IDB_PROPERTY,		0,						"Property"},
	};
	HIMAGELIST hImageList;
	HIMAGELIST hImageList_Disable;
	HBITMAP hBitmap;
	int iToolBarNum ;

//	lpied = GetIEPtr( hWnd ) ;
	// 创建工具条
	hToolbar = CreateToolbarEx( hWnd, WS_VISIBLE|WS_CHILD|TBSTYLE_FLAT|TBSTYLE_LIST|CCS_TOP,
			ID_TOOLBAR, 0, NULL, 0, NULL,0,20,20,16,16,0 );
	if( hToolbar == 0 )
	{ // 创建失败
		EdbgOutputDebugString("=========Create ToolBar is Failure===========\r\n");
		return 0;
	}
	iToolBarNum = sizeof(FileViewToolBar) / sizeof(FILEVIEWTOOLBAR); // 得到工具条目的个数
	hImageList=ImageList_Create(16,16,ILC_COLOR8,10,10); // 创建图象句柄
	hImageList_Disable=ImageList_Create(16,16,ILC_COLOR8,10,10); // 创建灰色图象句柄
	for (i=0;i<iToolBarNum;i++)
	{ // 逐个插入图象到图象句柄
		if (FileViewToolBar[i].bSysBmp == TRUE)
			hBitmap = LoadImage( NULL, MAKEINTRESOURCE(FileViewToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载系统资源
		else
			hBitmap = LoadImage( hInst, MAKEINTRESOURCE(FileViewToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载应用程序资源
		if (hBitmap )
			ImageList_Add(hImageList,hBitmap,NULL); // 插入句柄
		if (FileViewToolBar[i].IDDisRes != 0)
		{
			if (FileViewToolBar[i].bSysBmp == TRUE)
				hBitmap = LoadImage( NULL, MAKEINTRESOURCE(FileViewToolBar[i].IDDisRes), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 装载系统资源
			else
				hBitmap = LoadImage( hInst, MAKEINTRESOURCE(FileViewToolBar[i].IDDisRes), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // 转载应用程序资源
		}
		else
			hBitmap = NULL;
		ImageList_Add(hImageList_Disable,hBitmap,NULL); // 插入句柄
	}

//	SendMessage( hToolbar, TB_LOADIMAGES , IDB_STD_SMALL_COLOR , ( LPARAM )HINST_COMMCTRL ) ;
	hImageList = (HIMAGELIST)SendMessage( hToolbar, TB_SETIMAGELIST , 0, ( LPARAM )hImageList) ; // 将图象句柄设置到工具条
	hImageList_Disable = (HIMAGELIST)SendMessage( hToolbar, TB_SETDISABLEDIMAGELIST , 0, ( LPARAM )hImageList_Disable) ; // 将DISABLE图象句柄设置到工具条
	
	// 删除原来的ImageList
	if (hImageList)
		ImageList_Destroy(hImageList);
	if (hImageList_Disable)
		ImageList_Destroy(hImageList_Disable);

	tb.iString = 0 ;
	tb.fsStyle = TBSTYLE_BUTTON ;
	for( i = 0; i < iToolBarNum; i ++ )
	{ // 插入每个工具按钮
		if (FileViewToolBar[i].IDDisRes != 0)
			tb.fsState = 0 ; // 按钮DISABLE
		else 
			tb.fsState = TBSTATE_ENABLED  ; // 按钮ENABLE
		tb.iBitmap = i; // 图象索引
		tb.idCommand = FileViewToolBar[i].ID; // 按钮命令标号
		SendMessage( hToolbar, TB_ADDBUTTONS, 1, (LPARAM)&tb ); // 插入按钮
	}
	
	if (IsClipboardFormatAvailable(CF_FILE)) // 判断剪切板数据是否有效
		SendMessage(hToolbar,TB_ENABLEBUTTON,IDC_PASTE,TRUE); // 粘贴键EANBLE
	else
		SendMessage(hToolbar,TB_ENABLEBUTTON,IDC_PASTE,FALSE); // 粘贴键DISABLE

	return TRUE;
}
// ********************************************************************
// 声明：static LRESULT SetToolBarButtonEnable(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序设置工具栏的激活状态
// 引用: 
// ********************************************************************
static LRESULT SetToolBarButtonEnable(HWND hWnd)
{
	UINT iSelCount;
	HWND hToolBar;
	HWND hListView;
	HWND hFileBrowser;
	TCHAR lpCurPath[MAX_PATH];
	BOOL bSearch;

		hToolBar = GetDlgItem(hWnd,ID_TOOLBAR); // 得到工具条的窗口句柄
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览窗口的窗口句柄
		hListView = (HWND)SendMessage(hFileBrowser,EXM_GETLISTVIEW,0,0); // 得到LISTVIEW的窗口句柄
		iSelCount = SendMessage(hListView,LVM_GETSELECTEDCOUNT,0,0); // 得到选择数目

		if (iSelCount == 0)
		{ // 没有选择
			// Set Copy , Delete , rename disable
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_COPY,FALSE);
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_DELETE,FALSE);
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_RENAME,FALSE);
		}
		else 
		{ // 有选择
			// Set Delete enable
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_DELETE,TRUE);
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_COPY,TRUE);
			if (iSelCount == 1)
			{
				// Set Copy , rename enable
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_RENAME,TRUE);
			}
			else
			{
				// Set Copy , rename disable
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_RENAME,FALSE);
			}
		}
		bSearch = SendMessage(hFileBrowser,EXM_ISFIND,0,0); // 当前是否是查寻界面
		if (bSearch == FALSE)
		{ // 不是查寻界面
			SendMessage(hFileBrowser,EXM_GETCURPATH,MAX_PATH,(LPARAM)lpCurPath);
			if (strcmp(lpCurPath,"\\") == 0 || strlen(lpCurPath) == 0)
			{// 是根目录
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_UP,FALSE);
			}
			else
			{ // 不是根目录
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_UP,TRUE);
			}
		}
		else
		{ // 是查寻界面
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_UP,TRUE);
		}

		if (IsClipboardFormatAvailable(CF_FILE))// 剪切板数据是否有效
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,TRUE);
		else
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,FALSE);

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
			 case EXN_SELCHANGED: // 选择条目改变
				 DoItemChanged(hWnd,wParam,lParam);
				 break;
			 case EXN_DATACHANGED: // 数据改变
				 DoDataChanged(hWnd,wParam,lParam);
				 break;
			 case EXN_ACTIVEITEM: // 条目激活
				 DoActiveItem(hWnd,wParam,lParam);
				 break;
		}
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理 EXN_SELCHANGED 通知消息
// 引用: 
// ********************************************************************
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{ // 条目选择改变
		SetToolBarButtonEnable(hWnd); // 重新设置工具条的按钮状态
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理 EXN_DATACHANGED 通知消息
// 引用: 
// ********************************************************************
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{ // 数据改变
		SetToolBarButtonEnable(hWnd);// 重新设置工具条的按钮状态
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoCopy(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_COPY 命令
// 引用: 
// ********************************************************************
static LRESULT DoCopy(HWND hWnd)
{
	HWND hFileBrowser;
	HWND hToolBar;


		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览窗口的句柄
		SendMessage(hFileBrowser,WM_COPY,0,0); // 发送复制命令


		hToolBar = GetDlgItem(hWnd,ID_TOOLBAR); // 得到工具条的窗口句柄

		SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,TRUE); // 使“粘贴”按钮有效
		return 0;
/*	HWND hListView;
	int iIndex ;
	LVITEM lvItem;
	HWND hFileBrowser;
	char lpFileName[MAX_PATH];
	HWND hToolBar;
	LPFILECOPYDATA lpFileCopyData = NULL;

		hToolBar = GetDlgItem(hWnd,ID_TOOLBAR);

		SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,TRUE);

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER);
		hListView = SendMessage(hFileBrowser,EXM_GETLISTVIEW,0,0);

		iIndex = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

		while(1)
		{
			if (iIndex == -1)
				break;
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iIndex;
			lvItem.iSubItem = 0;
			lvItem.pszText = lpFileName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView, LVM_GETITEM, 0, &lvItem);
			iIndex = SendMessage(hListView, LVM_GETNEXTITEM, iIndex+1, LVNI_SELECTED);
		}

		return 0;
*/
}
// ********************************************************************
// 声明：static LRESULT DoPaste(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_PASTE 命令
// 引用: 
// ********************************************************************
static LRESULT DoPaste(HWND hWnd)
{
	HWND hFileBrowser;


		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览窗口的句柄
		SendMessage(hFileBrowser,WM_PASTE,0,0); // 发送粘贴命令
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoUpPath(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_UP 命令
// 引用: 
// ********************************************************************
static LRESULT DoUpPath(HWND hWnd)
{
	HWND hFileBrowser;

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件流露窗口的句柄
		SendMessage(hFileBrowser,EXM_UPPATH,0,0);	 // 发送“到上一级目录”的命令
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoDelete(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_DELETE 命令
// 引用: 
// ********************************************************************
static LRESULT DoDelete(HWND hWnd)
{
	HWND hFileBrowser;

		if (MessageBox(hWnd,"  确实要删除吗?  ","文件管理",MB_YESNO) == IDYES) // 询问是否要删除文件
		{ // 要删除文件
			hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览的窗口句柄
			SendMessage(hFileBrowser,EXM_DELETESEL,0,0); // 发送删除选择的命令
		}
		SetToolBarButtonEnable(hWnd); // 重设工具条按钮
		return 0;
}
// ********************************************************************
// 声明：static LRESULT DoRename(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_RENAME 命令
// 引用: 
// ********************************************************************
static LRESULT DoRename(HWND hWnd)
{
	HWND hFileBrowser;
	HWND hListView;
	int iIndex;

		RETAILMSG(1,(" Will Rename File \r\n"));
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览的窗口句柄
//		SendMessage(hFileBrowser,EXM_DELETESEL,0,0);	
		RETAILMSG(1,(" Get FileBrowser hwnd = %x \r\n",hFileBrowser));
		hListView = (HWND)SendMessage(hFileBrowser,EXM_GETLISTVIEW,0,0); // 得到LISTVIEW的窗口句柄

		RETAILMSG(1,(" Get ListView hwnd = %x \r\n",hListView));
		
		iIndex = SendMessage(hListView,LVM_GETSELECTIONMARK,0,0); // 得到当前选择
		if (iIndex == -1)
			return FALSE;
		RETAILMSG(1,(" Send EDITLABEL to ListView  %d \r\n",iIndex));
		SendMessage(hListView,LVM_EDITLABEL,iIndex,0); // 编辑标签
		return TRUE;
}
// ********************************************************************
// 声明：static LRESULT DoFind(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_FIND 命令
// 引用: 
// ********************************************************************

static LRESULT DoFind(HWND hWnd)
{
	HWND hFileBrowser;
//	int iIndex;
	HANDLE hInst;
	LPTSTR lpSearchString;

		lpSearchString = (LPTSTR)malloc(MAX_SEARCHSTRINGLEN); // 分配要查寻的字符串的缓存
		if (lpSearchString == NULL)
			return FALSE;
		hInst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		if (DialogBoxIndirectParamEx( hInst, (LPDLG_TEMPLATE_EX)&dlgFindTemplate, hWnd, FindDialogFunc, (LPARAM)lpSearchString) == FALSE) // 弹出对话框要求用户输入查寻字串
			return TRUE;
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览的窗口句柄
//		SendMessage(hFileBrowser,EXM_FINDFILE,0,"*.bmp;*.cpp");
		SendMessage(hFileBrowser,EXM_FINDFILE,(WPARAM)NULL,(LPARAM)lpSearchString); // 查找字符串
		free(lpSearchString); // 释放分配的空间
		return TRUE;
}
// ********************************************************************
// 声明：static LRESULT DoProperty(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_PROPERTY 命令
// 引用: 
// ********************************************************************
static LRESULT DoProperty(HWND hWnd)
{
	HWND hFileBrowser;
//	HWND hListView;
//	int iIndex;
	FILEPROPERTY stProperty;

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览的窗口句柄
		if (SendMessage(hFileBrowser,EXM_GETPROPERTY,0,(LPARAM)&stProperty) == TRUE) // 发送得到属性的命令
		{ // 得到成功
			RETAILMSG(1,(" Get Property OK!!!\r\n"));
			ShowFileProperty(hWnd,&stProperty); // 显示得到的属性
		}
		else
		{
			RETAILMSG(1,(" Get Property Failure!!!\r\n"));
		}
		return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoNewFolder(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理 IDC_NEWFOLDER 命令
// 引用: 
// ********************************************************************
static LRESULT DoNewFolder(HWND hWnd)
{
	HWND hFileBrowser;
//	int iIndex;

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览的窗口句柄
		SendMessage(hFileBrowser,EXM_NEWFOLDER,0,0); // 发送创建新目录的命令
		return TRUE;
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
//	short iItem,iFileNo;
//	HWND hListWnd;
//	LVITEM	lvItem;
//	LPNMITEMACTIVATE lpnmia;
	char lpFileName[MAX_PATH];//,lpFullName[MAX_PATH];
//	LPBROWSERDATA lpBrowser;
	HWND  hBrowser;
	UINT iIndex;

	/*
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);
		lpnmia=(LPNMITEMACTIVATE)lParam;
		
		hListWnd=lpnmia->hdr.hwndFrom;
		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=lpnmia->iItem;
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
//		SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)iItem,(LPARAM)&lvItem);
		SendMessage(hListWnd,LVM_GETITEM,(WPARAM)iItem,(LPARAM)&lvItem);

		if (lpBrowser->bSearch == TRUE)
		{
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=lpnmia->iItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)iItem,(LPARAM)&lvItem);
			//_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
//			strcat(lpFullName,"\\");
//			strcat(lpFullName,lpFileName);
			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}

		}
		else
		{
			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
		}

		if (lvItem.iImage == TYPE_FOLDER)
		{
			HWND hListView ;

				strcpy(lpBrowser->lpCurDir,lpFullName);
				hListView = GetDlgItem(hWnd,ID_LISTVIEW);
				UpdateListView( hListView );
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);
		}
		else if (lvItem.iImage == TYPE_TEXTFILE )
		{
		}
		else if (lvItem.iImage == TYPE_BMPFILE )
		{
		}
		else
		{
			if (StrAsteriskCmp("*.htm",lpFileName) == 0)
			{
				LoadApplication("IExplore",lpFullName);
			}
		}
*/
		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览的窗口句柄
		SendMessage(hBrowser,EXM_GETSELFILENAME,MAX_PATH,(LPARAM)lpFileName); // 得到当前的选择所对应的文件名
//		if (StrAsteriskCmp("*.htm",lpFileName) == 0)
/*
		if (FileNameCompare( "*.htm", strlen("*.htm"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("IExplore",lpFileName);
		}
		else if (FileNameCompare( "*.wav" , strlen( "*.wav" ), lpFileName, strlen(lpFileName)) == TRUE)
		{
//#ifndef EML_WIN32
			sndPlaySound(lpFileName,0);
//#endif
		}
		else if (FileNameCompare( "*.txt", strlen("*.txt"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		else if (FileNameCompare( "*.bmp", strlen("*.bmp"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		else if (FileNameCompare( "*.gif", strlen("*.gif"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		else if (FileNameCompare( "*.mp3", strlen("*.mp3"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		*/
		if (FileNameCompare( "*.wav" , strlen( "*.wav" ), lpFileName, strlen(lpFileName)) == TRUE)
		{ // 是否是声音文件
//#ifndef EML_WIN32
//			sndPlaySound(lpFileName,SND_ASYNC); // 播放该声音文件
//#endif
			return 0;
		}
		if (FileNameCompare( "*.exe", strlen("*.exe"), lpFileName, strlen(lpFileName)) == TRUE)
		{ // 是可执行文件
#ifndef INLINE_PROGRAM
			CreateProcess(lpFileName,0,0,0,0,0,0,0,0,0); // 执行该文件
#endif
			return 0;
		}
		iIndex = 0;
		while(1)
		{
			if (FileNameCompare( g_OpenList[iIndex].lpExtend, strlen(g_OpenList[iIndex].lpExtend), lpFileName, strlen(lpFileName)) == TRUE)
			{ // 如果是对应已经知道的文件
				RETAILMSG(1,("Will Run %s (%s)\r\n",g_OpenList[iIndex].lpApplication,lpFileName));
				// 用对应的应用程序打开该文件
#ifdef INLINE_PROGRAM
				LoadApplication(g_OpenList[iIndex].lpApplication,lpFileName);
#else
				CreateProcess(g_OpenList[iIndex].lpApplication,lpFileName,0,0,0,0,0,0,0,0);
#endif
				break;
			}
			iIndex ++;
			if (g_OpenList[iIndex].lpExtend == NULL)
				break;
		}
        return 0;
}



// ********************************************************************
// 声明：static LRESULT CALLBACK FindDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hDlg - 应用程序查找对话框的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理查找对话框消息过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK FindDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制窗口
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_COMMAND: // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_OK: // OK按钮
					FindString(hDlg);
					EndDialog(hDlg,TRUE);
					return 0;
				case IDC_CANCEL: // 取消按钮
					EndDialog(hDlg,FALSE);
					return 0;
			}	
			return 0;
		case WM_INITDIALOG: // 初始化对话框
			return DoInitFindDlg(hDlg,wParam,lParam);
		case WM_CLOSE: // 关闭窗口
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}
// ********************************************************************
// 声明：static LRESULT DoInitFindDlg(HWND hDlg,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hDlg - 应用程序查找对话框的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理查找对话框处理 WM_INITDIALOG 消息
// 引用: 
// ********************************************************************
static LRESULT DoInitFindDlg(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	HWND hEdit;
		hEdit = GetDlgItem(hDlg,IDC_FINDTEXT); // 得到输入文本窗口句柄
		SetFocus(hEdit); // 设置焦点到该窗口
		SetWindowLong(hDlg,GWL_USERDATA,lParam); // 设置用户数据
		return 0;
}
// ********************************************************************
// 声明：static LRESULT FindString(HWND hDlg)
// 参数：
//	IN hDlg - 应用程序查找对话框的窗口句柄
// 返回值：
//	
// 功能描述：应用程序处理查找对话框处理 IDC_OK 命令
// 引用: 
// ********************************************************************
static LRESULT FindString(HWND hDlg)
{
	LPTSTR lpSearchString ;
	HWND hFindText;

		lpSearchString = (LPTSTR)GetWindowLong(hDlg,GWL_USERDATA); // 得到要查寻字串的缓存
		if (lpSearchString == NULL)
			return FALSE;
		hFindText = GetDlgItem(hDlg,IDC_FINDTEXT); // 得到查寻条件编辑框的句柄
		GetWindowText(hFindText,lpSearchString,MAX_SEARCHSTRINGLEN); // 得到文本

		return TRUE;

}


// ********************************************************************
// 声明：static void ShowFileProperty(HWND hWnd,LPFILEPROPERTY lpFileProperty)
// 参数：
//	IN hWnd - 应用程序窗口句柄
//    LPFILEPROPERTY lpFileProperty  - 要显示的指定文件的属性
// 返回值：
//	
// 功能描述：应用程序显示指定文件的属性
// 引用: 
// ********************************************************************
static void ShowFileProperty(HWND hWnd,LPFILEPROPERTY lpFileProperty)
{
	HINSTANCE hInst;

		hInst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
		DialogBoxIndirectParamEx( hInst, (LPDLG_TEMPLATE_EX)&dlgShowPropertyTemplate, hWnd, ShowPropertyDialogFunc, (LPARAM)lpFileProperty); // 弹出显示属性对话框
}


// ********************************************************************
// 声明：static LRESULT CALLBACK ShowPropertyDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hDlg - 应用程序显示属性对话框的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序显示属性查找对话框消息过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK ShowPropertyDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // 绘制消息
			hdc = BeginPaint( hDlg, &ps );
			DoPaintShowProperty(hDlg,hdc);
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_INITDIALOG: // 初始化窗口
			DoInitShowProperty(hDlg,wParam,lParam);
			return 0;
		case WM_CLOSE: // 关闭窗口
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}
// ********************************************************************
// 声明：static LRESULT DoInitShowProperty(HWND hDlg,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hDlg - 应用程序显示属性对话框的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	
// 功能描述：应用程序处理显示属性对话框处理 WM_INITDIALOG 消息
// 引用: 
// ********************************************************************
static LRESULT DoInitShowProperty(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	SetWindowLong(hDlg,GWL_USERDATA,lParam); // 设置用户数据
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoPaintShowProperty(HWND hDlg,HDC hdc)
// 参数：
//	IN hDlg - 应用程序显示属性对话框的窗口句柄
//    IN hdc  - 设备句柄
// 返回值：
//	
// 功能描述：应用程序处理显示属性对话框处理 WM_PAINT 消息
// 引用: 
// ********************************************************************
static LRESULT DoPaintShowProperty(HWND hDlg,HDC hdc)
{
	LPFILEPROPERTY lpFileProperty;
	char lpTime[32],lpSize[32];
	SYSTEMTIME tSystemTime;

		lpFileProperty = (LPFILEPROPERTY)GetWindowLong(hDlg,GWL_USERDATA); // 得到文件属性
		if (lpFileProperty == NULL)
			return FALSE;
		SetBkMode( hdc, TRANSPARENT ) ; // 设置绘制方式
		TextOut(hdc,55,10,lpFileProperty->lpName,strlen(lpFileProperty->lpName));
		if (lpFileProperty->dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
		{ // 磁盘目录
			TextOut(hdc,55,35,"磁盘目录",strlen("磁盘目录"));
			sprintf(lpSize,"磁盘空间: %dK",(lpFileProperty->dwTotalSize+1023)/1024);
			TextOut(hdc,10,60,lpSize,strlen(lpSize));
			sprintf(lpSize,"可用空间: %dK",(lpFileProperty->dwFreeSize+1023)/1024);
			TextOut(hdc,10,85,lpSize,strlen(lpSize));
			return TRUE;
		}
		else if (lpFileProperty->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{ // 文件夹
			TextOut(hdc,55,35,"文件夹",strlen("文件夹"));
			sprintf(lpSize,"大小: %d(字节)",lpFileProperty->dwTotalSize);
			TextOut(hdc,10,60,lpSize,strlen(lpSize));
		}
		else
		{ // 一般文件
			TextOut(hdc,55,35,"一般文件",strlen("一般文件"));
			sprintf(lpSize,"大小: %d(字节)",lpFileProperty->dwTotalSize);
			TextOut(hdc,10,60,lpSize,strlen(lpSize));
		}


		// 创建时间
		FileTimeToSystemTime(&lpFileProperty->ftCreationTime,&tSystemTime);
		sprintf(lpTime,"创建时间: %02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
		TextOut(hdc,10,85,lpTime,strlen(lpTime));

		// 修改时间
		FileTimeToSystemTime(&lpFileProperty->ftLastAccessTime,&tSystemTime);
		sprintf(lpTime,"修改时间: %02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
		TextOut(hdc,10,110,lpTime,strlen(lpTime));
		return TRUE;
}


// **************************************************
// 声明：static LRESULT DoShellNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 
// 返回值：无
// 功能描述：处理SHELL发送的通知消息。
// 引用: 
// **************************************************
static LRESULT DoShellNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hFileBrowser;

	if (wParam == SN_REFRESH)
	{  // 要求刷新文件
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览窗口句柄
		SendMessage(hFileBrowser,EXM_REFRESH,0,0); // 刷新窗口
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT DoDestroyWindow(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 
// 返回值：无
// 功能描述：处理破坏窗口。
// 引用: 
// **************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
//			sndPlaySound(NULL,0); // 停止声音发送
			return 0;
}
