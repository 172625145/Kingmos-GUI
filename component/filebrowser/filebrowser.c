/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：文件浏览类，用户界面部分
版本号：1.0.0
开发时期：2003-04-02
作者：陈建明 Jami chen
修改记录：
		2004.08.24 增加剪切功能
		2004.09.09 解决外部的磁盘目录的根目录没有. 和.. 问题
**************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
//#include "FileBRes.h"
#include "Explorer.h"
//#include "resource.h"
#include <efilepth.h>
#include "eClipbrd.h"
#include "eGlobmem.h"

/***************  全局区 定义， 声明 *****************/
#define ID_LISTVIEW  101

#define TYPE_UNKNOWFILE	0
#define TYPE_FOLDER		1
#define TYPE_BMPFILE	2
#define TYPE_TEXTFILE	3
#define TYPE_MP3FILE	4
#define TYPE_IEFILE		5
#define TYPE_EMAILFILE	6
#define TYPE_EXEFILE	7
#define TYPE_GIFFILE	8
#define TYPE_JPGFILE	9

typedef struct structICONTYPE{
		int iFileType;
		int iIconID;
}ICONTYPE ;

static ICONTYPE tabIconType[] = {
	{ TYPE_UNKNOWFILE ,OIC_FILEEXPLORER_UNKNOWFILE },
	{ TYPE_FOLDER, OIC_FILEEXPLORER_FOLDER },
	{ TYPE_BMPFILE , OIC_FILEEXPLORER_BMPFILE },
	{ TYPE_TEXTFILE , OIC_FILEEXPLORER_TEXTFILE },
	{ TYPE_MP3FILE, OIC_FILEEXPLORER_MP3FILE },
	{ TYPE_IEFILE , OIC_FILEEXPLORER_IEFILE },
	{ TYPE_EMAILFILE , OIC_FILEEXPLORER_EMAILFILE },
	{ TYPE_EXEFILE , OIC_FILEEXPLORER_EXEFILE },
	{ TYPE_GIFFILE , OIC_FILEEXPLORER_GIFFILE },
	{ TYPE_JPGFILE , OIC_FILEEXPLORER_GIFFILE },
};

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

static FILETYPE tabFileType[] = {
	{ "*.bmp" , TYPE_BMPFILE },
	{ "*.txt" , TYPE_TEXTFILE },
	{ "*.mp3" , TYPE_MP3FILE },
	{ "*.pls" , TYPE_MP3FILE },
	{ "*.htm" , TYPE_IEFILE },
	{ "*.mlg" , TYPE_EMAILFILE },
	{ "*.exe" , TYPE_EXEFILE },
	{ "*.gif" , TYPE_GIFFILE },
	{ "*.jpg" , TYPE_JPGFILE },
};

#define SUMITEMNUM 4
#define ID_MAINCOLUMN	0
#define ID_SIZECOLUMN	1
#define ID_DATECOLUMN	2
#define ID_DIRCOLUMN	3

//#define _MAPPOINTER

typedef struct FileCopyData{
	TCHAR lpFileName[MAX_PATH];
	UINT  uFileTYPE;
}FILECOPYDATA, *LPFILECOPYDATA;

static const char classFileBrowser[] = "FileBrowser";// 文件浏览的类名

typedef struct{
	HIMAGELIST hImageList;
	TCHAR lpCurDir[MAX_PATH];
	DWORD iSortIndex;
	BOOL bSearch;
	char  *lpSearch;
	FindFile FindFileFunc;


	UINT iSortSub;

	char *lpFilter;

	HWND hCopyWindow;
	BOOL bCut;  // 是否是剪切，如果是剪切，则在粘贴后要删除原来的文件
}BROWSERDATA , *LPBROWSERDATA;

ATOM RegisterFileBrowserClass(HINSTANCE hInstance);
static LRESULT CALLBACK FileBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static HWND  CreateListView(HWND  hWnd);
static BOOL UpdateListView( HWND hListView );
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage);
static BOOL  AddSubItem( HWND hListView, int item,  int itemSub,LPTSTR lpItem );
//static BOOL  CreateListViewColumn( HWND  hListView, DWORD dwStatus );
static BOOL  CreateListViewColumn( HWND  hListView, DWORD idColumn );
static void	LoadFileBrowserImage(HWND hWnd,HWND hListView);
static int InsertDirectoryItem(HWND hWnd,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir);
static void InsertFileItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoColumnClick(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEndEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static int CALLBACK FileCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetListView(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  void SendNormalNotify(HWND hWnd,UINT iCode,LPNMLISTVIEW lpnmListView);
static void GetCurrentDirectory(HWND hListView,LPTSTR lpstrDir);
static LRESULT DoSetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoUpPath(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFindFile(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCopy(HWND hWnd);
static LRESULT DoCut(HWND hWnd);
static LRESULT DoPaste(HWND hWnd);
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoIsFind(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelFileName(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoOpenCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoRenameCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static BOOL DeleteDirectory(HWND hWnd,LPTSTR lpFullName);
static BOOL SearchDirectory(HWND hWnd,HWND hListView,LPTSTR lpCurDir);
static void GetSearchString(LPTSTR *lpSearch,DWORD *pdwLen);
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName);
static BOOL CopyFolder(HWND hWnd,LPCTSTR lpExistingFolderName, LPCTSTR lpNewFolderName, BOOL bFailIfExists,BOOL bCover);

static BOOL IsChildDir(LPCTSTR lpParentFolderName, LPCTSTR lpChildFolderName);

static BOOL SearchCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
static BOOL StrnStr(const char *s1, const char *s2,int nLen);
static BOOL IsAsterisk(LPCTSTR lpcszMask, int iMaskLen);

static BOOL CallBackFindFile(HWND hWnd ,FILE_FIND_DATA	FindFileData,LPTSTR lpCurDir);

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );

static BOOL MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName);


// !!! Add By Jami chen in 2003.09.09
static LRESULT DoRefresh(HWND hWnd);
// !!! Add End By Jami chen in 2003.09.09
static LPTSTR BackupFileName(LPTSTR lpExistingFolderName);

// !!! Add By Jami chen in 2003.09.23
static BOOL IsExist(LPTSTR lpNewFileName);
static BOOL DeleteSpecialFile(HWND hWnd,LPTSTR lpFileName);
// !!! Add End By Jami chen in 2003.09.23

// !!! Add By Jami chen in 2003.09.25
static void ShowCopyDialog(HWND hWnd);
static void CloseCopyDialog(HWND hWnd);
// !!! Add End By Jami chen in 2003.09.25

// !!! Add By Jami chen in 2003.09.28
static LPTSTR GetOnlyFileName(LPTSTR lpFullName);
// !!! Add End By Jami chen in 2003.09.28

//static HWND g_hCopyDlg = NULL;

// ********************************************************************
// 声明：ATOM RegisterFileBrowserClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance - 当前应用程序的实例句柄
// 返回值：
//	成功，返回非零，不成功，返回零。
// 功能描述：注册当前应用程序的类
// 引用: 被 应用程序入口程序 调用
// ********************************************************************
ATOM RegisterFileBrowserClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)FileBrowserWndProc; // 文件浏览控制过程函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG); // 设置窗口扩展数据长度
	wc.hInstance		= hInstance;  // 设置实例句柄
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH);  // 设置类背景刷
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classFileBrowser;  // 设置类名

	return RegisterClass(&wc);  // 向系统注册文件浏览类
}


// ********************************************************************
// 声明：static LRESULT CALLBACK FileBrowserWndProc(HWND , UINT , WPARAM , LPARAM )
// 参数：
//	IN hWnd- 应用程序的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	不同的消息有不同的返回值，具体看消息本身
// 功能描述：文件浏览类窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK FileBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT:  // 绘制消息
			hdc = BeginPaint(hWnd, &ps);
//			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_COMMAND:  // 命令消息
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_NOTIFY:  // 通知消息
			return DoNotify(hWnd,wParam,lParam);
		case EXM_GETLISTVIEW: // 得到列式视控件句柄
			return DoGetListView(hWnd,wParam,lParam);
		case EXM_SETCURPATH:  // 设置当前路径
			return DoSetCurPath(hWnd,wParam,lParam);
		case EXM_GETCURPATH:  // 得到当前路径
			return DoGetCurPath(hWnd,wParam,lParam);
		case EXM_UPPATH: // 到上一级目录
			return DoUpPath(hWnd,wParam,lParam);
		case EXM_DELETESEL:  // 删除选择
			return DoDeleteSel(hWnd,wParam,lParam);
		case EXM_FINDFILE:  // 查找文件
			return DoFindFile(hWnd,wParam,lParam);
		case EXM_NEWFOLDER:  // 创建一个新的目录
			return DoNewFolder(hWnd,wParam,lParam);
		case EXM_ISFIND: // 是否在查寻状态
			return DoIsFind(hWnd,wParam,lParam);
		case EXM_GETSELFILENAME:  // 得到文件名
			return DoGetSelFileName(hWnd,wParam,lParam);
		case EXM_GETFILTER: // 得到过滤串
			return DoGetFilter(hWnd,wParam,lParam);
		case EXM_SETFILTER: // 设置过滤串
			return DoSetFilter(hWnd,wParam,lParam);
		case EXM_GETPROPERTY:  // 得到当前属性
			return DoGetProperty(hWnd,wParam,lParam);
		case WM_CREATE:  // 创建文件浏览类
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_SETFOCUS: // 设置焦点
			return DoSetFocus(hWnd,wParam,lParam);
		case WM_DESTROY:  // 破坏窗口
			DoDestroyWindow(hWnd);
			break;
		case WM_COPY:
			DoCopy(hWnd);
			break;
		case WM_PASTE:
			DoPaste(hWnd);
			break;
		case WM_CUT:
			DoCut(hWnd);
			break;
// !!! Add By Jami chen in 2003.09.09
		case EXM_REFRESH:
			DoRefresh(hWnd);
			break;
// !!! Add End By Jami chen in 2003.09.09

		case EXM_OPENCURSEL:
			DoOpenCurSel(hWnd,wParam,lParam);
			break;

		case EXM_RENAME: //重命名当前文件
			DoRenameCurSel(hWnd,wParam,lParam);
			break;

	case WM_SIZE: // 窗口大小发生改变
			return DoSize(hWnd,wParam,lParam);
	case WM_WINDOWPOSCHANGED:
			return DoWindowPosChanged(hWnd,wParam,lParam);

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
	LPBROWSERDATA lpBrowser = (LPBROWSERDATA)malloc(sizeof(BROWSERDATA));

//	RETAILMSG(1,("Explorer Create ...\r\n"));
	lpBrowser->hImageList = NULL;  // 初始化图象列表为空
//	lpBrowser->lpCurDir = "\\";
	strcpy(lpBrowser->lpCurDir , "\\");  // 当前目录为根目录

	lpBrowser->bSearch = FALSE;  // 不在查寻状态
	lpBrowser->lpSearch = NULL;
	lpBrowser->FindFileFunc = NULL;

	lpBrowser->lpFilter = NULL;  // 没有过滤串

	lpBrowser->iSortIndex = 0;    // 没有排序
	lpBrowser->hCopyWindow = NULL;

	lpBrowser->bCut = FALSE; 
	SetWindowLong(hWnd,0,(LONG)lpBrowser);  // 将当前的浏览类结构指针存入窗口
	CreateListView(hWnd); // 创建列式
//	RETAILMSG(1,("Explorer Create OK \r\n"));
	return 0;
}
// ********************************************************************
// 声明：static LRESULT DoDestroyWindow(HWND hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
//	返回TRUE，将继续破坏窗口，返回FALSE，则不破坏窗口
// 功能描述：应用程序处理破坏窗口的消息
// 引用: 
// ********************************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;

	lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
	if (lpBrowser->hImageList)
	{ // 破坏图象列表
		ImageList_Destroy(lpBrowser->hImageList);
		lpBrowser->hImageList = NULL;
	}
	if (lpBrowser->lpSearch)
	{
		free(lpBrowser->lpSearch);  // 释放查寻串
		lpBrowser->lpSearch = NULL;
	}
	if (lpBrowser->lpFilter)
	{
		free(lpBrowser->lpFilter);  // 释放过滤串
		lpBrowser->lpFilter = NULL;
	}

//	OpenClipboard(hWnd);
//	SetClipboardData(CF_FILE,NULL);
//	CloseClipboard();

	return 0;
}

// ********************************************************************
// 声明：static HWND  CreateListView(HWND  hWnd)
// 参数：
//	IN hWnd- 应用程序的窗口句柄
// 返回值：
// 	成功返回ListView的窗口句柄，否则返回NULL
// 功能描述：创建一个ListView控件
// 引用: 
// ********************************************************************
static HWND  CreateListView(HWND  hWnd)
{
	RECT						rt;
	HWND						hListView;
	CTLCOLORSTRUCT stCtlColor;
	DWORD dwStyle;

//	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt);  // 得到窗口矩形大小
	// 创建列式
	hListView=CreateWindow( classLISTCTRL,"",
//				WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL| WS_VSCROLL | WS_HSCROLL,
				WS_VISIBLE|WS_CHILD|LVS_LIST|LVS_SHOWSELALWAYS|LVS_SINGLESEL| WS_VSCROLL | WS_HSCROLL | LVS_HORZLINE,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_LISTVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	if (hListView == NULL)
	{ // 创建列式失败
//		RETAILMSG(1,("CreateListView Failure\r\n"));
		return NULL;
	}


	dwStyle = GetWindowLong(hWnd,GWL_STYLE);

//	RETAILMSG(1,("Will Set ListView Color...\r\n"));
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR | CLF_TITLECOLOR | CLF_TITLEBKCOLOR;
	stCtlColor.cl_Text = RGB(78,81,78);
	stCtlColor.cl_TextBk = RGB(247,255,247);
	stCtlColor.cl_Selection = RGB(255,255,255);
	stCtlColor.cl_SelectionBk = RGB(77,166,255);
	stCtlColor.cl_Title = RGB(78,81,78);
	stCtlColor.cl_TitleBk = RGB(247,255,247);
	//SendMessage(hListView,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);  // 设置列式颜色

//	RETAILMSG(1,("Will LoadImage...\r\n"));
	LoadFileBrowserImage(hWnd,hListView);  // 装载文件图象列表
//	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView, ID_MAINCOLUMN);  // 创建主列
	if ((dwStyle & FBS_NOSIZECOLUMN) == 0)
		CreateListViewColumn( hListView, ID_SIZECOLUMN); // 创建尺寸列
	if ((dwStyle & FBS_NODATECOLUMN) == 0)
		CreateListViewColumn( hListView, ID_DATECOLUMN); // 创建日期列
//	RETAILMSG(1,("Will Insert Data...\r\n"));
	UpdateListView(hListView);  // 更新列式数据
//	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView;
}


// ********************************************************************
// 声明：static BOOL UpdateListView( HWND hWnd )
// 参数：
//	IN hListView- ListView的窗口句柄
// 返回值：
//	成功返回TRUE，否则返回FALSE
// 功能描述：更新ListView控件的内容
// 引用: 
// ********************************************************************
static BOOL UpdateListView( HWND hListView )
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						lpstrDir[MAX_PATH];
	HWND hWnd;
	LPBROWSERDATA lpBrowser;
// Add By Jami chen in 2003.09.09
	DWORD unViewAttributes = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ROMMODULE;
// Add End By Jami chen in 2003.09.09
	DWORD dwStyle;


		hWnd				= GetParent( hListView );  // 得到父窗口句柄
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到浏览类结构指针
		if (lpBrowser == 0)
			return 0; // 数据错误
		SendMessage(hListView,LVM_DELETEALLITEMS,0,0);  // 删除当前所有内容

		if (lpBrowser->bSearch)
		{  // 是查寻状态
//			RETAILMSG(1,(" Will Search Need File \r\n"));
			strcpy(lpstrDir,"\\");
			SearchDirectory(hWnd,hListView,lpstrDir);  // 查找根目录
		}
		else
		{  // 不是根目录
			GetCurrentDirectory(hListView,lpstrDir);  // 得到当前的文件夹
//			strcat(lpstrDir,"\\*.*");  // 要搜索所有的文件
/*			if (strcmp(lpstrDir,"\\") == 0)
			{
				strcat(lpstrDir,"*.*");
			}
			else
			{
				strcat(lpstrDir,"\\*.*");
			}
*/
			// Add By Jami chen in 2004.09.09
			if (strcmp(lpstrDir,"\\") != 0)
			{
				// 不是根目录，必须有. 和.. 目录
				if (dwStyle & FBS_SHOWSUBDIR)
				{
					InsertListViewItem(hListView,0xffff,0,".",TYPE_FOLDER);  //插入文件名到最后
					InsertListViewItem(hListView,0xffff,0,"..",TYPE_FOLDER);  //插入文件名到最后
				}
			}
			// Add End By Jami chen in 2004.09.09
			MergeFullFileName(lpstrDir,NULL,"*.*"); // 要搜索所有的文件
//			RETAILMSG(1,(" FindFirstFile %s \r\n",lpstrDir));
			hFind=FindFirstFile(lpstrDir, &FindFileData  );  // 查找第一个文件
			if( hFind == INVALID_HANDLE_VALUE) 
			{// 文件不存在
				return TRUE;
			}
			while(TRUE)
			{  // 文件存在
				if(FindFileData.cFileName[0]=='.')
				{  // 该文件是一个表示当前目录是子目录的文件，跳过。
					// 判断是否需要显示子目录文件
					// Delete By Jami chen in 2004.09.09
//					if (dwStyle & FBS_SHOWSUBDIR)
//					{
//						InsertDirectoryItem(hListView,FindFileData,NULL);  // 将当前目录插入到列式
//					}
					// Delete End By Jami chen in 2004.09.09
				}
				else
				{
					// !!! Add By Jami chen in 2003.09.09
					// 是否需要让用户看见
					if (!( FindFileData.dwFileAttributes&unViewAttributes ))
					{
					// !!! Add End By Jami chen in 2003.09.09
		//				strcpy(lpstrFullName,lped->lpstrCurrentDir);
		//				strcat(lpstrFullName,"\\");
		//				strcat(lpstrFullName,FindFileData.cFileName);
	//					RETAILMSG(1,("Find File < %s >\r\n",FindFileData.cFileName));
						if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
						{
							// 查找到的是一个目录
	//						InsertDirectoryItem(hListView,FindFileData,lpBrowser->lpCurDir);
	//						RETAILMSG(1,("Find a directory\r\n"));
							InsertDirectoryItem(hListView,FindFileData,NULL);  // 将当前目录插入到列式
		//					InsertListViewItem(hWnd,1,0,FindFileData.cFileName,1);
		//					if(lped->dExploreStatus&EXPS_OPENEXPLORE)
		//					{
		//						strcpy(lped->lpstrCurrentDir,lpstrFullName);
		//						AddFileItemToExp(hWnd);
		//						strcpy(lped->lpstrCurrentDir,lpstrOldDir);
		//					}
						}
						else
						{
							// 查找到的是一个文件
							if (lpBrowser->lpFilter != NULL)
							{  // 有过滤串，判断当前文件是否满足过滤串
								LPTSTR lpFilter;
								LPTSTR lpFileName = NULL;
								DWORD dwlen;

	//							RETAILMSG(1,("Will Find File <%s>\r\n",lpBrowser->lpFilter));
								// !!! Add By Jami chen in 2003.09.28
								lpFileName = GetOnlyFileName(FindFileData.cFileName); // 得到纯文件名
								// !!! Add End By Jami chen in 2003.09.28
								lpFilter = lpBrowser->lpFilter;  // 得到过滤串
								while(1)
								{
									GetSearchString(&lpFilter,&dwlen); // 得到搜索串
									if (dwlen == 0)
									{
	//									RETAILMSG(1,("Find File is not Need\r\n"));
										break;
									}
									// !!! Modified By Jami chen in 2003.09.28
//									if (FileNameCompare( lpFilter, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)
									if (FileNameCompare( lpFilter, dwlen, lpFileName, strlen(lpFileName)) == FALSE)
									// !!! Modified End By Jami chen in 2003.09.28
									{  // 当前文件不满足搜索串
										lpFilter += dwlen; // 下一个搜索串
										continue;
									}
									// 当前文件满足条件
	//								RETAILMSG(1,("Find File is Need\r\n"));
									InsertFileItem(hListView,FindFileData,NULL);  // 将当前文件插入到列式
									break;
								}
	//							strcat(lpstrDir,lpBrowser->lpFilter);  // 要搜索需要的文件
							}
							else
							{  // 没有过滤串，直接将文件插入到列式
								InsertFileItem(hListView,FindFileData,NULL);
							}
							//InsertListViewItem(hWnd,1,0,FindFileData.cFileName,0);
//							InsertFileItem(hListView,FindFileData,lpBrowser->lpCurDir);
						}
					}
				}
				if( FindNextFile(hFind,&FindFileData)==0 )  //查找下一个文件
				{ // 已经没有文件了
					break;
				}
			}
			FindClose(hFind);  // 查找结束
		}

//		插入数据完成
		{
			int iCount;
			int iCurItem = 0;
				
				iCount = SendMessage(hListView,LVM_GETITEMCOUNT,0,0); // 得到当前列表的条目个数
				if (iCount)
				{
					// 有条目存在
					SendMessage(hListView,LVM_SETHOTITEM,iCurItem,0);  // 设置第一条条目为激活条目
					SendMessage(hListView,LVM_SETSELECTIONMARK,0,iCurItem);  // 设置第一条条目为选择条目
				}
		}
		return TRUE;
}
// ********************************************************************
//声明：static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage)
//参数：
//	IN hListView- ListView的窗口句柄
//    IN iItem  - 要添加条目的条目位置
//    IN itemSub - 要添加的条目的子条目位置
//    IN lpItem  - 要添加条目的内容
//    IN iImage  - 要添加条目的Image位置
// 返回值：
//	返回插入条目的Index
// 功能描述：在ListView控件中添加一个条目
// 引用: 
// ********************************************************************
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,
							   TCHAR* lpItem,int iImage)
{
	LV_ITEM				lvitem;
	HWND				hWnd;
//	TCHAR				lpText[MAX_PATH];

	LPBROWSERDATA lpBrowser;

	hWnd				= GetParent( hListView );  // 得到父窗口句柄
	lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到文件浏览类结构指针
	if (lpBrowser == 0)
		return 0; // 数据错误

//	strcpy( lpText, lpItem );
/*	if( GetWindowLong( hWnd, GWL_STYLE )&EXPS_LISTVIEWICON  )
	{
		GetMainFileName( lpText ) ;
	}
*/
	lvitem.mask			= LVIF_TEXT | LVIF_PARAM ;
	if( ! itemSub  )
		lvitem.mask		|= LVIF_IMAGE;
	lvitem.iItem		= iItem;     // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
	lvitem.cchTextMax	= strlen(lpItem );
	lvitem.iImage		= iImage;
	lvitem.lParam		= lpBrowser->iSortIndex;  // 插入到指定的位置，如果不在排序状态，则插入到最后

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // 插入一个条目到列式

	lpBrowser->iSortIndex ++;  // 指向下一个条目
	
	return iItem;
}
// ********************************************************************
// 声明：static BOOL  AddSubItem( HWND hListView, int item,  int itemSub,LPTSTR lpItem )
// 参数：
//	IN hListView- ListView的窗口句柄
//    IN iItem  - 要添加条目的条目位置
//    IN itemSub - 要添加的条目的子条目位置
//    IN lpItem  - 要添加条目的内容
// 返回值：
// 	成功返回TRUE，否则返回FALSE.
// 功能描述：在ListView控件指定的条目中添加一个子条目
// 引用: 
// ********************************************************************
static BOOL  AddSubItem( HWND hListView, int item,  int itemSub,LPTSTR lpItem )
{
	LV_ITEM				lvitem;
	lvitem.mask			=  LVIF_TEXT;
	lvitem.iItem		= item;   // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
//	lvitem.cchTextMax	= strlen( lpItem);
	return SendMessage(hListView,LVM_SETITEM,0,(LPARAM)&lvitem);  // 设置条目内容
}

// ********************************************************************
// 声明：static BOOL  CreateListViewColumn( HWND  hListView, DWORD idColumn )
// 参数：
//	IN hListView - ListView的窗口句柄
//    IN idColumn - 要创建的列号
// 返回值：
//	无
// 功能描述：在ListView控件中添加列项目
// 引用: 
// ********************************************************************
static BOOL  CreateListViewColumn( HWND  hListView, DWORD idColumn )
{
//	short			i;
	LV_COLUMN		lvcolumn;
//	short           column= SUMITEMNUM;
//	DWORD           dwWinAttr;
	
	int			columnwidth[]={240,50,90,280};
//	int			columnwidth[]={148,50,90,280};
//	int			columnwidth[]={110,50,20,10};
	char *pTitle[SUMITEMNUM] = {"名称","大小","日期","目录"};

//	RETAILMSG(1, ("Calling  SendMessage 0\r\n"));
//    dwWinAttr=GetWindowLong( hListView,GWL_STYLE );
//	dwWinAttr &=~LVS_TYPEMASK;
//	dwWinAttr |=dwStatus;
//	SetWindowLong(hListView,GWL_STYLE, dwWinAttr );
//	for ( i = 0; i < SUMITEMNUM; i++ )  
//	{
	if (idColumn >= SUMITEMNUM)
		return FALSE;  // 指定列是非法列
	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;  // 设置列结构中的有效数据
	if(idColumn  == 0 || idColumn  == 3)
		lvcolumn.fmt = LVCFMT_LEFT ;
	else
		lvcolumn.fmt = LVCFMT_RIGHT ;
	if( idColumn==0 )
		lvcolumn.fmt|=	LVCFMT_IMAGE;  // 设置列的显示状态
	lvcolumn.pszText = pTitle[idColumn];  // 设置列的标题
	lvcolumn.cchTextMax =strlen (pTitle[idColumn]);
	lvcolumn.iSubItem = idColumn;
	lvcolumn.cx = columnwidth[idColumn];     // width of column.
//		lvcolumn.iImage= 0;
//	RETAILMSG(1, ("Calling  SendMessage\r\n"));
	if( SendMessage(hListView,LVM_INSERTCOLUMN,idColumn,(LPARAM)&lvcolumn)==-1 ) // 插入一个列到列式
	{
		return FALSE;
	}
//	}
	return TRUE;
}
// ********************************************************************
// 声明：static BOOL  DeleteListViewColumn( HWND  hListView, DWORD idColumn )
// 参数：
//	IN hListView - ListView的窗口句柄
//    IN idColumn - 要删除的列号
// 返回值：
//	无
// 功能描述：在ListView控件中删除列项目
// 引用: 
// ********************************************************************
static BOOL  DeleteListViewColumn( HWND  hListView, DWORD idColumn )
{
	if( SendMessage(hListView,LVM_DELETECOLUMN,idColumn,0)==FALSE )  // 删除一个指定的列
	{
		return FALSE;
	}
	return TRUE;
}
// ********************************************************************
// 声明：static void	LoadFileBrowserImage(HWND hWnd,HWND hListView)
// 参数：
//	IN hWnd - FileBrowser的窗口句柄
//	IN hListView - ListView的窗口句柄
// 返回值：
//	无
// 功能描述：给ListView控件中添加一个 ImageList
// 引用: 
// ********************************************************************
static void	LoadFileBrowserImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
//	LPREGAPDATA				lprad ;
	int						i ;
	int iIconTypeNum;
	LPBROWSERDATA lpBrowser;
	HINSTANCE hInstance;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // 得到窗口的实例句柄
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针

		if (lpBrowser->hImageList == NULL)
		{ //ImageList 是空的，必须创建一个ImageList
			lpBrowser->hImageList = ImageList_Create(16,16,ICL_ICON,8,8); //创建一个图象列表

			if (lpBrowser->hImageList == NULL) //创建ImageList失败
				return ;
		}
// !!! Modified By Jami chen in 2003.08.22
/*		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_FOLDER ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;

		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_TEXTFILE ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;

		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_BMPFILE ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;

		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_UNKNOWFILE ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;
*/
		iIconTypeNum = sizeof(tabIconType) / sizeof(ICONTYPE);

		for ( i = 0 ; i < iIconTypeNum ; i++ )
		{
			hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( tabIconType[i].iIconID ), IMAGE_ICON,
				16, 16, 0 ) ;  // 装载指定的图标
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ; // 将图标添加到列表
		}
// !!! Modified End By Jami chen in 2003.08.22
		//Get all of the Application
	/*	GetRegistApplication( lped->lpExtList, 0, FALSE ) ;
		for( i = 0 ; i < lped->lpExtList->count ; i ++ )
		{
			lprad = ( LPREGAPDATA )PtrListAt( lped->lpExtList, i ) ;
			ASSERT( lprad != 0 ) ;
			if( strcmp( lprad->lpszPostfix, "" ) == 0 )
			{
				PtrListAtFree( lped->lpExtList, i ) ;
				i -- ;
			}else
			{
				hIcon = GetApplicationIcon( lprad->lpszInternal, SMALL_ICON ) ;
				ASSERT( hIcon != 0 ) ;
				ImageList_AddIcon( lped->hImageList, hIcon ) ;
			}
		}
	*/
		lpBrowser->hImageList = (HIMAGELIST)SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_SMALL, (LPARAM)lpBrowser->hImageList);  // 设置图象列表到列式

}

// ********************************************************************
//声明：static int InsertDirectoryItem(HWND hListView,FILE_FIND_DATA FindFileData)
//参数：
//	IN hListView - ListView的窗口句柄
//  IM FindFileData  - 当前找到的文件的数据
//返回值：
//	无
//功能描述：给ListView控件中添加一个文件夹条目
//引用: 
// ********************************************************************
static int InsertDirectoryItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir)
{
	int iItem ;
	char lpTime[16];
	SYSTEMTIME tSystemTime;
	DWORD dwStyle;


		iItem = InsertListViewItem(hListView,0xffff,0,FindFileData.cFileName,TYPE_FOLDER);  //插入文件名到最后
// !!! modified  By Jami chen in 2004.06.23
		dwStyle = GetWindowLong(GetParent(hListView),GWL_STYLE);
		if ((dwStyle & FBS_NODATECOLUMN) == 0)
		{
			FileTimeToSystemTime(&FindFileData.ftLastAccessTime,&tSystemTime);
			sprintf(lpTime,"%02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
			AddSubItem(hListView,iItem,2,lpTime);  //插入文件时间
		}
// !!! modified end By Jami chen in 2004.06.23

		if (lpCurDir)
			AddSubItem(hListView,iItem,3,lpCurDir);  //插入目录

		return iItem;
}

// ********************************************************************
//声明：static void InsertFileItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir)
//参数：
//	IN hListView - ListView的窗口句柄
//  IM FindFileData  - 当前找到的文件的数据
//返回值：
//	无
//功能描述：给ListView控件中添加一个文件条目
//引用: 
// ********************************************************************
static void InsertFileItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir)
{
	int iItem ;
	DWORD dwSize;//,dot;
	char lpSize[16];
	char lpTime[16];
	SYSTEMTIME tSystemTime;
	int iImageIndex = TYPE_UNKNOWFILE;
	int iFileTypeNum , i;
	LPTSTR lpFileName = NULL;
	DWORD dwStyle;


// !!! Modified By Jami chen in 2003.08.22
/*		
//		if (StrAsteriskCmp("*.txt",FindFileData.cFileName) == 0)
		if (FileNameCompare( "*.txt", strlen("*.txt"), FindFileData.cFileName, strlen(FindFileData.cFileName)) == TRUE)
		{
			iImageIndex =  TYPE_TEXTFILE;
		}
//		else if (StrAsteriskCmp("*.bmp",FindFileData.cFileName) == 0)
		else if (FileNameCompare( "*.bmp", strlen("*.bmp"), FindFileData.cFileName, strlen(FindFileData.cFileName)) == TRUE)
		{
			iImageIndex =  TYPE_BMPFILE;
		}

*/
		// !!! Add By Jami chen in 2003.09.28
		lpFileName = GetOnlyFileName(FindFileData.cFileName);  // 得到纯文件名
		// !!! Add End By Jami chen in 2003.09.28
		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE);
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			// !!! Modified  By Jami chen in 2003.09.28
//			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), FindFileData.cFileName, strlen(FindFileData.cFileName)) == TRUE)
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpFileName, strlen(lpFileName)) == TRUE)
			// !!! Modified  End By Jami chen in 2003.09.28
			{ // 比较当前文件是否是已知文件
				iImageIndex =  tabFileType[i].iFileType;  // 使用已知文件的图标
				break;
			}
		}
// !!! Modified End By Jami chen in 2003.08.22

		iItem = InsertListViewItem(hListView,0xffff,0,FindFileData.cFileName,iImageIndex);  //插入文件名到最后

// !!! modified By Jami chen in 2004.06.23

		dwStyle = GetWindowLong(GetParent(hListView),GWL_STYLE);
		if ((dwStyle & FBS_NOSIZECOLUMN) == 0)
		{
			dwSize = FindFileData.nFileSizeLow;  // 得到文件尺寸
			if (dwSize > (1024 *1024l))
			{  // 大于1M的文件显示为x.xM
				dwSize = ((dwSize+1024 * 1024 / 10l-1)* 10) / (1024 *1024l);
				sprintf(lpSize,"%d.%dM",dwSize/10,dwSize%10);
			}
			else if (dwSize > 0)
			{  // 其他文件显示为xK
				dwSize = (dwSize+1023l) / 1024l;
				sprintf(lpSize,"%dK",dwSize);
			}
			else 
			{ // 文件长度为0的文件显示为0
				strcpy(lpSize,"0");
			}
			AddSubItem(hListView,iItem,1,lpSize);  //插入文件长度
		}

		if ((dwStyle & FBS_NODATECOLUMN) == 0)
		{
			FileTimeToSystemTime(&FindFileData.ftLastAccessTime,&tSystemTime);
			sprintf(lpTime,"%02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
			AddSubItem(hListView,iItem,2,lpTime);  //插入文件时间
		}

// !!! modified end By Jami chen in 2004.06.23
		if (lpCurDir)
			AddSubItem(hListView,iItem,3,lpCurDir);  //插入目录
}

// **************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：返回消息的处理结果
// 功能描述：处理通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	NMLISTVIEW *pnmvl;
	NMHDR   *hdr=(NMHDR   *)lParam;    

		switch(hdr->code)
		{
			 case LVN_COLUMNCLICK:  // 鼠标单击列头
//				 pnmvl=(NMLISTVIEW *)lParam;
//				 SendMessage(pnmvl->hdr.hwndFrom,LVM_SORTITEMS,(WPARAM)pnmvl->iSubItem,(LPARAM)FileCompareFunc);
//				 iDistance=1-iDistance;
				 DoColumnClick(hWnd,wParam,lParam);
				 break;
			 case LVN_ITEMACTIVATE:  // 条目激活
//				 DoActiveItem(hWnd,wParam,lParam);
				 DoOpenCurSel(hWnd,0,0);
				 break;
			 case LVN_ITEMCHANGED: // 条目改变
				 DoItemChanged(hWnd,wParam,lParam);
				 break;
			 case LVN_ENDLABELEDIT:  // 编辑条目标签结束
				 return DoEndEditLabel(hWnd,wParam,lParam);
		}
		return 0;
}

// **************************************************
// 声明：static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPNMITEMACTIVATE 结构指针
// 
// 返回值：无
// 功能描述：处理条目激活的通知消息。
// 引用: 
// **************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	int iItem;//,iFileNo;
	HWND hListWnd;
	LVITEM	lvItem;
	LPNMITEMACTIVATE lpnmia;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针
		lpnmia=(LPNMITEMACTIVATE)lParam;  // 得到LPNMITEMACTIVATE结构指针
		
		hListWnd=lpnmia->hdr.hwndFrom;  // 得到列式的窗口句柄
		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=lpnmia->iItem;  // 得到激活条目索引
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
//		SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)iItem,(LPARAM)&lvItem);
		SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // 得到条目文件名

		if (lpBrowser->bSearch == TRUE)
		{  // 当前为查寻状态
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=lpnmia->iItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);  // 得到文件目录

/*			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}
*/
			if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE) // 组合文件全路径
				return 0;
		}
		else
		{
			if (strcmp(lpFileName,".") == 0)
			{
				// 当前目录，不需要处理
				return 0;
			}
			if (strcmp(lpFileName,"..") == 0)
			{
				// 到上一级目录
				DoUpPath(hWnd,0,0);
				return 0;
			}
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)  // 组合文件全路径
				return 0;
		}

		if (lvItem.iImage == TYPE_FOLDER)
		{ // 当前是一个目录
			HWND hListView ;
	
				hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄
				if (lpBrowser->bSearch)
				{  // 当前在查寻状态
					lpBrowser->bSearch = FALSE;
					DeleteListViewColumn(hListView,ID_DIRCOLUMN); // 删除目录列
					if (lpBrowser->lpSearch)
					{
						free(lpBrowser->lpSearch);  // 释放查寻串
						lpBrowser->lpSearch = NULL;
					}
					lpBrowser->FindFileFunc = NULL;  // 设置回调为空
				}
				strcpy(lpBrowser->lpCurDir,lpFullName);  // 设置当前目录为激活的目录
				UpdateListView( hListView ); // 更新列式
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // 发送通知消息数据改变
		}
		else
		{

			SendNormalNotify(hWnd,EXN_ACTIVEITEM,NULL);  // 发送条目激活的通知
		}

        return 0;
}
// **************************************************
// 声明：static LRESULT DoColumnClick(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 指向NMLISTVIEW的指针
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：鼠标点击到列头处理。
// 引用: 
// **************************************************
static LRESULT DoColumnClick(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	NMLISTVIEW *pnmvl;
	LPBROWSERDATA lpBrowser;

		pnmvl=(NMLISTVIEW *)lParam;  // 得到列式通知结构指针

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == NULL)
			return FALSE; // 数据错误
		lpBrowser->iSortSub = pnmvl->iSubItem;  // 得到鼠标点击的列的索引
		SendMessage(pnmvl->hdr.hwndFrom,LVM_SORTITEMS,(WPARAM)hWnd,(LPARAM)FileCompareFunc); // 发送排序消息给列式
		return TRUE;
}

// **************************************************
// 声明：static int CALLBACK FileCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
// 参数：
// 	IN lParam1 -- 条目1的参数
// 	IN lParam2 -- 条目2的参数
// 	IN lParamSort -- 列式排序参数，有调用排序是指定，在这里是浏览类的句柄
// 
// 返回值：返回0 表示相同，大于0 表示条目1 大于条目2，小于0 表示条目1 小于条目2
// 功能描述：比较二条目的大小。
// 引用: 
// **************************************************
static int CALLBACK FileCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{
	HWND hWnd;
	HWND hListView;
	LPBROWSERDATA lpBrowser;
	int iIndex1, iIndex2 ;
	LVFINDINFO lvFinfInfo;
	LVITEM lvItem;
	TCHAR lpText1[MAX_PATH],lpText2[MAX_PATH];
	int iImage1,iImage2;
		
		hWnd = (HWND)lParamSort;  // 得到浏览类的窗口句柄

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类的结构指针
		if (lpBrowser == NULL)
			return FALSE; // 参数错误，无法比较

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式的句柄

		iIndex1 = -1;
		lvFinfInfo.flags = LVFI_PARAM;
		lvFinfInfo.lParam = lParam1;
		iIndex1 = SendMessage(hListView,LVM_FINDITEM, iIndex1,(LPARAM)&lvFinfInfo); // 得到条目1的索引
		if (iIndex1 == -1) 
			return FALSE; // 没有找到条目1 ，无发比较

		iIndex2 = -1;
		lvFinfInfo.flags = LVFI_PARAM;
		lvFinfInfo.lParam = lParam2;
		iIndex2 = SendMessage(hListView,LVM_FINDITEM, iIndex2,(LPARAM)&lvFinfInfo); // 找到条目2 的索引
		if (iIndex2 == -1)
			return FALSE;  // 没有找到条目2

		if (lpBrowser->iSortSub == 0)
		{  // 按主条目排序
			lvItem.mask = LVIF_TEXT | LVIF_IMAGE; 
			lvItem.iItem = iIndex1; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText1; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem);  // 得到条目1 的文本与图象索引
			iImage1 = lvItem.iImage; 

			lvItem.mask = LVIF_TEXT | LVIF_IMAGE; 
			lvItem.iItem = iIndex2; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText2; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem);  //得到条目2 的文本 与图象索引
			iImage2 = lvItem.iImage; 
			if (iImage1 == TYPE_FOLDER)
			{ // 条目1 是文件夹
				if (iImage2 != TYPE_FOLDER) 
					return -1; // 条目2 是文件，设定条目1 小于条目2
			}
			else
			{ // 条目1 是文件
				if (iImage2 == TYPE_FOLDER)
					return 1;  // 条目2 是文件夹 ，设定条目1 大于条目2
			}
			return stricmp(lpText1,lpText2);  // 比较两条目文本的大小
		}
		else
		{
			lvItem.mask = LVIF_TEXT ; 
			lvItem.iItem = iIndex1; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText1; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem); // 得到条目1 的文本

			lvItem.mask = LVIF_TEXT ; 
			lvItem.iItem = iIndex2; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText2; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem);  // 得到条目2 的文本

			if (lpBrowser->iSortSub == 1)
			{
				//这里是文件的长度
				int ilen1,ilen2;

					ilen1 = strlen(lpText1);
					ilen2 = strlen(lpText2);
					if (ilen1 > ilen2)
						return 1;  // 数字串越长，数字越大
					if (ilen1 < ilen2)
						return -1; // 数字串短，数字小
			}

			return stricmp(lpText1,lpText2); // 比较文本大小
		}

		return 0; // 返回相等 ，不会运行到这里
}
// **************************************************
// 声明：static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：列式条目选择改变是的通知。
// 引用: 
// **************************************************
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMLISTVIEW pnmv;

		pnmv = (LPNMLISTVIEW)lParam;
		SendNormalNotify(hWnd,EXN_SELCHANGED,pnmv);  // 发送通知消息给父窗口选择条目改变

		return 0;
}
// **************************************************
// 声明：static LRESULT DoGetListView(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：返回列式视控制句柄
// 功能描述：得到列式视控件句柄，处理EXM_GETLISTVIEW消息。
// 引用: 
// **************************************************
static LRESULT DoGetListView(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView ;

	
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式视控制句柄

		return (LRESULT)hListView;
}
// **************************************************
// 声明：static  void SendNormalNotify(HWND hWnd,UINT iCode,LPNMLISTVIEW lpnmListView)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iCode -- 通知代码
// 	IN lpnmListView -- 列式通知结构指针
// 
// 返回值：无
// 功能描述：发送通知消息给父窗口。
// 引用: 
// **************************************************
static  void SendNormalNotify(HWND hWnd,UINT iCode,LPNMLISTVIEW lpnmListView)
{
	HWND hParent;
	HWND hListView;
	NMFILEEXPLORER nmFileExplorer;

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);
		nmFileExplorer.hdr.hwndFrom=hWnd; // 发送通知消息的窗口
		nmFileExplorer.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);  // 通知窗口的ID
		nmFileExplorer.hdr.code=iCode; // 通知代码
		nmFileExplorer.lpnmListView = lpnmListView;
		nmFileExplorer.hListView = hListView; // 列式句柄
		hParent=GetParent(hWnd);  // 得到父窗口句柄
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmFileExplorer);  // 发送通知消息
		return ;
}
// **************************************************
// 声明：static void GetCurrentDirectory(HWND hListView,LPTSTR lpstrDir)
// 参数：
// 	IN hListView -- 列式句柄
// 	OUT lpstrDir -- 得到当前目录的缓存
// 
// 返回值：无
// 功能描述：列式得到当前的目录
// 引用: 
// **************************************************
static void GetCurrentDirectory(HWND hListView,LPTSTR lpstrDir)
{
	LPBROWSERDATA lpBrowser;
	HWND hWnd;

	hWnd = GetParent(hListView); // 得到父窗口句柄，即浏览类的句柄
	lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到浏览类的结构指针
	strcpy(lpstrDir,lpBrowser->lpCurDir);  // 得到当前的文件夹
}
// **************************************************
// 声明：static LRESULT DoSetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR 要设置的新的路径
// 
// 返回值：无
// 功能描述：设置当前路径，处理EXM_SETCURPATH消息。
// 引用: 
// **************************************************
static LRESULT DoSetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView ;
	char *pNewPath;
	LPBROWSERDATA lpBrowser;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		pNewPath = (LPTSTR)lParam;  // 得到新的当前路径
		if (pNewPath == NULL)  
			return 0; // 当前路径为空
		if (strlen(pNewPath) >= MAX_PATH)
			return 0; // 非法路径，路径长度大于系统要求要求路径的最大长度
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到浏览类的结构指针
		strcpy(lpBrowser->lpCurDir,pNewPath);  // 设置当前的文件夹
			
		hListView = GetDlgItem(hWnd,ID_LISTVIEW); // 得到列式视句柄
		UpdateListView( hListView ); // 重新更新列式视
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // 通知父窗口当前数据改变
		return 0;
}
// **************************************************
// 声明：static LRESULT DoGetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR 要得到的路径的缓存
// 
// 返回值：无
// 功能描述： 得到当前路径， 处理EXM_GETCURPATH消息
// 引用: 
// **************************************************
static LRESULT DoGetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	char *pNewPath;
	LPBROWSERDATA lpBrowser;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
		pNewPath = (LPTSTR)lParam;  // 得到存储路径的饿缓存地址
		if (pNewPath == NULL)
			return 0; // 指针为空，参数错误 
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到浏览类的结构指针
		strcpy(pNewPath,lpBrowser->lpCurDir);  // 得到当前的文件夹
		return 0;
}
// **************************************************
// 声明：static LRESULT DoUpPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述： 到上一级目录，处理EXM_UPPATH消息。
// 引用: 
// **************************************************
static LRESULT DoUpPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	LPTSTR lpCurPos;
	HWND hListView ;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到浏览类的结构指针
		if (lpBrowser == NULL)
			return 0; // 指针错误
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式视句柄
		if (lpBrowser->bSearch)
		{   // 是查寻结果
			lpBrowser->bSearch = FALSE; // 退出查寻
			DeleteListViewColumn(hListView,ID_DIRCOLUMN);// 删除目录列，一般列式不需要
			if (lpBrowser->lpSearch)
			{  
				free(lpBrowser->lpSearch);  // 释放查寻字串
				lpBrowser->lpSearch = NULL; 
			}
			lpBrowser->FindFileFunc = NULL; // 将查寻回调函数置空
			goto UPPATHEND; // 完成操作
		}
		if (strcmp(lpBrowser->lpCurDir,"\\") ==0 )  // 比较当前目录是不是根目录
			return 0; // 不能再到上一级目录
		lpCurPos = lpBrowser->lpCurDir; // 得到当前目录指针
		while(*lpCurPos) lpCurPos ++; // 指到最后一个字符
		// 开始向前查寻目录分隔符“\\”
		while(1)
		{
			if (lpCurPos == lpBrowser->lpCurDir)
			{  // 已经到目录的第一个字符
				*(lpCurPos+1) = 0;
				goto UPPATHEND;  // 结束
			}
			if (*lpCurPos == '\\')
			{  // 找到目录分隔符
				*lpCurPos = 0;
				goto UPPATHEND;  // 结束
			}
			lpCurPos --;
		}

UPPATHEND:
		UpdateListView( hListView ); // 更新列式
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // 通知父窗口数据已经改变
		return 0;
}

// **************************************************
// 声明：static LRESULT DoDeleteSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述： 删除选择，处理EXM_DELETESEL消息。
// 引用: 
// **************************************************
static LRESULT DoDeleteSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	HWND hListView ;
	LV_ITEM				lvItem;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
	int iIndex;
	BOOL bRet;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类结构指针
		if (lpBrowser == NULL)
			return 0; // 错误结构
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式视句柄
		iIndex = -1;

		while(1)
		{
			iIndex = SendMessage(hListView,LVM_GETNEXTITEM, iIndex,LVNI_SELECTED);  // 得到列式中的选择索引
			if (iIndex == -1)
				break;  // 没有选择条目
			lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
			lvItem.iItem=iIndex;
			lvItem.iSubItem=0;
			lvItem.pszText = lpFileName;  
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView,LVM_GETITEM,(WPARAM)0,(LPARAM)&lvItem);// 得到选择条目的文件名

			if (lpBrowser->bSearch == TRUE)
			{  // 是在查寻状态下
				lvItem.mask  = LVIF_TEXT ;
				lvItem.iItem=iIndex;
				lvItem.iSubItem=3;
				lvItem.pszText = lpFullName;
				lvItem.cchTextMax = MAX_PATH;
				SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem);  // 得到选择条目的文件目录
				//_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
	//			strcat(lpFullName,"\\");
	//			strcat(lpFullName,lpFileName);

/*				if (strcmp(lpFullName,"\\") ==0 )
				{
					strcat(lpFullName,lpFileName);
				}
				else
				{
					strcat(lpFullName,"\\");
					strcat(lpFullName,lpFileName);
				}
*/
				if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE)  // 将目录与文件名合成一个全路径文件
					return 0;
				
			}
			else
			{
//				_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
				if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE) // 将当前路径与得到的文件名合成一个全路径文件
					return 0;
			}
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (lvItem.iImage == TYPE_FOLDER)
			{  // 指定选项是一个目录
				 //删除整个目录
				bRet = DeleteDirectory(hWnd,lpFullName);
				if (bRet == FALSE)
				{  // 删除错误
					TCHAR lpMessage[64];
					DWORD err = GetLastError(); 
					sprintf(lpMessage," 不能删除文件(%d) \r\n(%s)",err,lpFullName);
					MessageBox(hWnd,lpMessage,"错误",MB_OK);  // 提示错误
//					return FALSE;
				}
			}
			else
			{
				// 删除文件
				// !!! Modified By Jami chen in 2003.09.23
				/*
				bRet = DeleteFile(lpFullName);
				if (bRet == FALSE)
				{
					TCHAR lpMessage[64];
					DWORD dwErr = GetLastError();
					sprintf(lpMessage,"不能删除文件\r\n(%s)",lpFullName);
					MessageBox(hWnd,lpMessage,"错误",MB_OK);
					return FALSE;
				}
				*/
				bRet = DeleteSpecialFile(hWnd,lpFullName);  // 删除文件
//				if (bRet == FALSE)
//				{
//					return FALSE;
//				}
				// !!! Modified By Jami chen in 2003.09.23
			}
			if (bRet == TRUE)
				SendMessage(hListView,LVM_DELETEITEM,(WPARAM)iIndex,0);  // 删除列式中对应的条目
			else
				iIndex ++;  //当前文件没有删除，将去删除下一个文件
		}
		return TRUE;
}
// **************************************************
// 声明：static BOOL DeleteDirectory(HWND hWnd,LPTSTR lpFullName)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpFullName -- 要删除的文件夹的路径
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：删除指定的文件夹。
// 引用: 
// **************************************************
static BOOL DeleteDirectory(HWND hWnd,LPTSTR lpFullName)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						*lpstrDir;
	BOOL bRet = TRUE;

		lpstrDir = (LPTSTR)malloc(MAX_PATH); // 分配一个存放文件夹的缓存

		if (lpstrDir == NULL)
			return FALSE;  // 分配失败
/*
		strcpy(lpstrDir,lpFullName);
//		strcat(lpstrDir,"\\*.*");  // 要搜索所有的文件
		if (strcmp(lpstrDir,"\\") == 0)
		{
			strcat(lpstrDir,"*.*");
		}
		else
		{
			strcat(lpstrDir,"\\*.*");
		}
*/
		if (MergeFullFileName(lpstrDir,lpFullName,"*.*") == FALSE)  // 合成指定的文件
			goto DELETE_END; // 合成失败
		hFind=FindFirstFile(lpstrDir, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 文件不存在，该文件夹下已经没有文件
			bRet =RemoveDirectory(lpFullName); // 删除指定文件夹
			goto DELETE_END;
		}
		while(TRUE)
		{  // 文件存在，首先要删除文件夹下的文件
			if(FindFileData.cFileName[0]=='.')
			{  // 该文件是一个表示当前目录是子目录的文件，跳过。
			}
			else
			{
//				strcpy(lpstrDir,lpFullName);
//				strcat(lpstrDir,"\\");
//				strcat(lpstrDir,FindFileData.cFileName);  
				if (MergeFullFileName(lpstrDir,lpFullName,FindFileData.cFileName) == FALSE) // 组合要删除的文件名
					goto DELETE_END;
				if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{
					// 查找到的是一个目录
					bRet = DeleteDirectory(hWnd,lpstrDir);  // 删除该目录
				}
				else
				{
					// 查找到的是一个文件
					// !!! Modified By Jami chen in 2003.09.23
					/*
					bRet = DeleteFile(lpstrDir);
					if (bRet == FALSE)
					{
						TCHAR lpMessage[64];
						sprintf(lpMessage,"不能删除文件\r\n(%s)",lpstrDir);
						MessageBox(hWnd,lpMessage,"错误",MB_OK);
						goto DELETE_END;
					}
					*/
					bRet = DeleteSpecialFile(hWnd,lpstrDir); // 删除指定的文件
					if (bRet == FALSE)
					{  // 删除失败
						goto DELETE_END;
					}
					// !!! Modified End By Jami chen in 2003.09.23
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //查找下一个文件
			{  // 已经没有文件
				FindClose(hFind); // 关闭查寻句柄
//				free(lpstrDir);
				bRet =RemoveDirectory(lpFullName);  // 删除当前目录
				goto DELETE_END;
			}
		}

DELETE_END:
		if (lpstrDir)
			free(lpstrDir); // 释放分配的内存
		return bRet; // 返回结果
}
// **************************************************
// 声明：static LRESULT DoEndEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 列式句柄
// 	IN lParam -- LPNMLVDISPINFO 结构指针。
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：结束修改标签。
// 引用: 
// **************************************************
static LRESULT DoEndEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMLVDISPINFO lpnmLVDispInfo;
	TCHAR lpOldFullName[MAX_PATH],lpNewFullName[MAX_PATH],lpCurDir[MAX_PATH];
	LVITEM lvItem;
	LPBROWSERDATA lpBrowser;
	BOOL bRet;
	HWND hListView;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == NULL)
			return FALSE;  // 错误数据

		lpnmLVDispInfo = (LPNMLVDISPINFO)lParam; // 得到数据结构指针
		if (lpnmLVDispInfo == NULL)
			return FALSE;  // 参数错误

		SendNormalNotify(hWnd,EXN_ENDRENAME,NULL);  // 通知数据已经重命名完成

		hListView = (HWND)wParam;  // 得到列式视句柄
		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=lpnmLVDispInfo->item.iItem;
		lvItem.iSubItem=0;
		lvItem.pszText = lpNewFullName;
		lvItem.cchTextMax = MAX_PATH;
		SendMessage(hListView,LVM_GETITEMTEXT,(WPARAM)lpnmLVDispInfo->item.iItem,(LPARAM)&lvItem); //得到指定条目的文件名
		if (stricmp(lpNewFullName,lpnmLVDispInfo->item.pszText) == 0)
		{// 源文件与目标文件一致，不需要做重命名
			return TRUE;
		}
		if (lpBrowser->bSearch == TRUE)
		{ // 在查寻状态
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=lpnmLVDispInfo->item.iItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpCurDir;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);  // 得到指定文件的路径
			//_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
//			strcat(lpFullName,"\\");
//			strcat(lpFullName,lpFileName);
/*			if (strcmp(lpCurDir,"\\") ==0 )
			{
			}
			else
			{
				strcat(lpCurDir,"\\");
			}
*/
//			strcpy(lpNewFullName,lpOldFullName);
//			strcat(lpCurDir,lpNewFullName);
//			strcat(lpCurDir,lpnmLVDispInfo->item.pszText);
//			_makepath( lpOldFullName, NULL, lpCurDir, lpNewFullName, NULL);
//			_makepath( lpNewFullName, NULL, lpCurDir, lpnmLVDispInfo->item.pszText, NULL);
			if (MergeFullFileName(lpOldFullName,lpCurDir,lpNewFullName) == FALSE)  // 组合旧文件
				return FALSE;
			if (MergeFullFileName(lpNewFullName,lpCurDir,lpnmLVDispInfo->item.pszText) == FALSE) // 组合成新文件
				return FALSE;

		}
		else
		{
//			_makepath( lpOldFullName, NULL, lpBrowser->lpCurDir, lpNewFullName, NULL);
//			_makepath( lpNewFullName, NULL, lpBrowser->lpCurDir, lpnmLVDispInfo->item.pszText, NULL);
			if (MergeFullFileName(lpOldFullName,lpBrowser->lpCurDir,lpNewFullName) == FALSE)  // 组合旧文件
				return FALSE;
			if (MergeFullFileName(lpNewFullName,lpBrowser->lpCurDir,lpnmLVDispInfo->item.pszText) == FALSE)  // 组合新文件
				return FALSE;
		}
		//bRet = DeleteAndRenameFile(lpNewFullName, lpOldFullName);
		bRet = MoveFile(lpOldFullName,lpNewFullName); // 重命名文件
		if (bRet == FALSE)
		{ // 重命名文件失败
			TCHAR lpMessage[64];
			DWORD err = GetLastError(); 

				sprintf(lpMessage,"不能重命名文件(%d)\r\n(%s)",err,lpOldFullName);
				MessageBox(hListView,lpMessage,"错误",MB_OK);
		}
		return bRet;
}
// **************************************************
// 声明：static BOOL SearchDirectory(HWND hWnd,HWND hListView,LPTSTR lpCurDir)
// 参数：
// 	IN hWnd -- 浏览类句柄
// 	IN hListView -- 列式句柄
// 	IN lpCurDir -- 要查找的目录
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：查找当前目录下的所有文件，并将他插入到列式。
// 引用: 
// **************************************************
static BOOL SearchDirectory(HWND hWnd,HWND hListView,LPTSTR lpCurDir)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						*lpstrDir;
	LPBROWSERDATA lpBrowser;
// Add By Jami chen in 2003.09.09
	DWORD unViewAttributes = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ROMMODULE;
// Add End By Jami chen in 2003.09.09

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // 得到浏览类结构指针
		if (lpBrowser == 0)
			return FALSE; // 数据错误

		lpstrDir = (LPTSTR)malloc(MAX_PATH+1);  // 分配一个保存文件的指针
		if (lpstrDir == NULL)
			return FALSE;  // 分配失败
/*		strcpy(lpstrDir,lpCurDir);
//		strcat(lpstrDir,"\\*.*");  // 要搜索所有的文件
		if (strcmp(lpstrDir,"\\") == 0)
		{
			strcat(lpstrDir,"*.*");
		}
		else
		{
			strcat(lpstrDir,"\\*.*");
		}
*/
		if (MergeFullFileName(lpstrDir,lpCurDir,"*.*") == FALSE) //组合文件
		{ // 组合文件失败
			free(lpstrDir); // 释放内存
			return FALSE;
		}
//		RETAILMSG(1,(" Will Search  File < %s >\r\n" , lpstrDir));
		hFind=FindFirstFile(lpstrDir, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 文件不存在
			return TRUE;
		}
		while(TRUE)
		{  // 文件存在
			if(FindFileData.cFileName[0]=='.')
			{  // 该文件是一个表示当前目录是子目录的文件，跳过。
			}
			else
			{
				// !!! Add By Jami chen in 2003.09.09
				if (!( FindFileData.dwFileAttributes&unViewAttributes ))
				// !!! Add End By Jami chen in 2003.09.09
				{
					if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
					{
						// 查找到的是一个目录
						LPTSTR lpSearch;
						DWORD dwlen;

	//					RETAILMSG(1,("  Find a Directory <%s>\r\n" , FindFileData.cFileName));
						lpSearch = lpBrowser->lpSearch; // 得到查找字串
						while(1)
						{
							GetSearchString(&lpSearch,&dwlen); //得到一个查找字串
							if (dwlen == 0)
								break;
	//						if (FileNameCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)
							if (SearchCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)// 与目录比较
							{  // 目录不匹配
								lpSearch += dwlen;
								continue;
							}
							// 目录匹配
							if (CallBackFindFile(hWnd,FindFileData,lpCurDir) != FALSE) // 呼叫应用程序回调
								InsertDirectoryItem(hListView,FindFileData,lpCurDir); // 可以插入当前目录
							break;
						}

	/*					strcpy(lpstrDir,lpCurDir);
						if (strcmp(lpstrDir,"\\"))
							strcat(lpstrDir,"\\");
						strcat(lpstrDir,FindFileData.cFileName);  
	*/
						if (MergeFullFileName(lpstrDir,lpCurDir,FindFileData.cFileName) == FALSE) // 组合成新的目录
						{
							free(lpstrDir);
							return FALSE;
						}
						SearchDirectory(hWnd,hListView,lpstrDir);  // 查找子目录
					}
					else
					{
						// 查找到的是一个文件
	//					if (StrAsteriskCmp(lpBrowser->lpSearch,FindFileData.cFileName) == 0)
						LPTSTR lpSearch;
						DWORD dwlen;

	//					RETAILMSG(1,("  Find a File <%s>\r\n" , FindFileData.cFileName));
						lpSearch = lpBrowser->lpSearch; // 得到查寻串
						while(1)
						{
							GetSearchString(&lpSearch,&dwlen);  // 得到一个查寻串
							if (dwlen == 0)
								break;  // 查寻串结束
	//						if (FileNameCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)
							if (SearchCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE) // 与文件比较
							{  // 不匹配
								lpSearch += dwlen; // 指向下一个查寻串
								continue;
							}
	//						RETAILMSG(1,("  the File is need , will ask caller\r\n" ));
							if (CallBackFindFile(hWnd,FindFileData,lpCurDir) != FALSE) // 呼叫回调
							{
	//							RETAILMSG(1,("   caller need , will insert it\r\n" ));
								InsertFileItem(hListView,FindFileData,lpCurDir);  // 可以插入该文件
							}
							break;
						}
					}
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //查找下一个文件
			{ // 已经没有文件了
				break;
			}
		}
		free(lpstrDir);  // 释放内存
		FindClose(hFind); // 结束查寻
		return TRUE;
}


// **************************************************
// 声明：static LRESULT DoFindFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- FindFile 查找到文件后的回调函数
// 	IN lParam -- LPTSTR 要查找的字符串
// 
// 返回值：
// 功能描述：查找文件，处理EXM_FINDFILE消息。
// 引用: 
// **************************************************
static LRESULT DoFindFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	LPTSTR lpFindString;
	HWND hListView;

//		RETAILMSG(1,("!!! Find File !!!!\r\n"));
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类结构指针
		if (lpBrowser == 0)
			return 0; // 错误数据
#ifdef _MAPPOINTER
			wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // 映射指针
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
		lpFindString = (LPTSTR)lParam; // 要查找的字符串
//		RETAILMSG(1,(" The Search String is <%s>, <%X>\r\n",lpFindString,lpFindString));
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式视的句柄
		if (lpFindString == NULL)
		{  // 查找字串为空
			if (lpBrowser->bSearch == TRUE)
			{ // 原来处于查寻状态，退出查寻状态
				lpBrowser->bSearch = FALSE;
				DeleteListViewColumn(hListView,ID_DIRCOLUMN);  // 删除目录列
				if (lpBrowser->lpSearch)
				{
					free(lpBrowser->lpSearch); // 释放查找字串
					lpBrowser->lpSearch = NULL;
				}
				lpBrowser->FindFileFunc = NULL;  // 设置回调函数为空
				UpdateListView( hListView ); // 更新列式
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // 通知数据已经改变
			}
			return 0; // 返回
		}
		if (lpBrowser->bSearch == TRUE)
		{  // 原来处于查寻状态，清除原来的数据
			if (lpBrowser->lpSearch)
			{
				free(lpBrowser->lpSearch);  // 释放原来的查找字串
				lpBrowser->lpSearch = NULL;
			}
			lpBrowser->FindFileFunc = NULL;  // 设置回调函数为空
		}
		else
		{   
			// 如果不在查寻画面，则需要增加目录的条目
			CreateListViewColumn(hListView,ID_DIRCOLUMN);  
		}
		lpBrowser->bSearch = TRUE;  // 设置进入查寻状态
		lpBrowser->FindFileFunc = (FindFile)wParam; // 设置回调函数指针
		lpBrowser->lpSearch = (LPTSTR)malloc(strlen(lpFindString)+1); // 分配空间来保存新的查询字串
		strcpy(lpBrowser->lpSearch,lpFindString);  // 复制查寻字串的内容
		UpdateListView( hListView ); // 更新列式
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // 通知数据已经更新
		return 0;
}

// **************************************************
// 声明：static void GetSearchString(LPTSTR *lpSearch,DWORD *pdwLen)
// 参数：
// 	IN/OUT lpSearch --当前查寻字串的指针，并返回查寻字串的开始位置
// 	OUT pdwLen -- 返回查寻字串的长度
// 
// 返回值：无
// 功能描述：得到一个查寻字串。
// 引用: 
// **************************************************
static void GetSearchString(LPTSTR *lpSearch,DWORD *pdwLen)
{
	LPTSTR lpCurPos;
	DWORD dwLen;

		lpCurPos = *lpSearch;
		dwLen = 0;

		// 调过开始的' '和';'，他们是查寻字串的分隔符
		while(*lpCurPos)
		{
			if (*lpCurPos == ' ' || *lpCurPos == ';')
			{
				lpCurPos ++;
				continue;
			}
			break;
		}

		*lpSearch = lpCurPos; // 重新设置查寻字串的位置

		// 得到查寻字串的长度，遇到' '和';'结束
		while(*lpCurPos)
		{
			if (*lpCurPos == ' ' || *lpCurPos == ';')
				break;
			lpCurPos ++;
			dwLen ++;
		}
		*pdwLen = dwLen;  // 返回字串长度

}
// **************************************************
// 声明：static LRESULT DoCopy(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：将选择条目的文件名存到剪切板。
// 引用: 
// **************************************************
static LRESULT DoCopy(HWND hWnd)
{
	HWND hListView;
	int iIndex ;
	LVITEM lvItem;
	char lpFileName[MAX_PATH];
	DWORD *lpData;
	LPFILECOPYDATA lpFileCopyData = NULL;
	int nSelCount, nNumber = 0;
	LPBROWSERDATA lpBrowser;
	DWORD dwDataLen;
	HGLOBAL hData;

	
//		RETAILMSG(1,("Enter Copy Proc ... \r\n"));
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针
		if (lpBrowser == 0)
			return 0; // 错误数据

//		RETAILMSG(1,("Get Copy Data ... \r\n"));
		lpBrowser->bCut = FALSE; 

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄
		nSelCount = SendMessage(hListView,LVM_GETSELECTEDCOUNT,0,0);  // 得到当前选择条目的数目

		dwDataLen = nSelCount * sizeof (FILECOPYDATA) + sizeof(DWORD);// 计算需要的存储空间
		lpData = (DWORD *)malloc(dwDataLen);  // 分配内存
		if (lpData == NULL)
			return FALSE; // 分配失败
		*lpData = nSelCount; // 先保存选择条目的个数
		lpFileCopyData = (LPFILECOPYDATA )(lpData+1);

		iIndex = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);  // 得到第一个选择条目的索引

		while(1)
		{
			if (iIndex == -1)  
				break; //已经没有选择条目
//			RETAILMSG(1,("Get a File ...\r\n"));
			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			lvItem.iItem = iIndex;
			lvItem.iSubItem = 0;
			lvItem.pszText = lpFileName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView, LVM_GETITEM, 0, (LPARAM)&lvItem);  // 得到选择条目的文件名

			if (lpBrowser->bSearch == TRUE)
			{
				// 在查寻状态
				lvItem.mask  = LVIF_TEXT ;
				lvItem.iItem=iIndex;
				lvItem.iSubItem=3;
				lvItem.pszText = lpFileCopyData[nNumber].lpFileName;
				lvItem.cchTextMax = MAX_PATH;
				SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem); // 得到指定文件的目录

/*				if (strcmp(lpFileCopyData[nNumber].lpFileName,"\\") ==0 )
				{
					strcat(lpFileCopyData[nNumber].lpFileName,lpFileName);
				}
				else
				{
					strcat(lpFileCopyData[nNumber].lpFileName,"\\");
					strcat(lpFileCopyData[nNumber].lpFileName,lpFileName);
				}
*/
				if (MergeFullFileName(lpFileCopyData[nNumber].lpFileName,NULL,lpFileName) == FALSE) // 组合成全路径文件名
				{ // 组合失败
					free(lpData);  // 释放内存
//					RETAILMSG(1,("Get Copy Data Failure \r\n"));
					return FALSE;
				}
			}
			else
			{
//				_makepath( lpFileCopyData[nNumber].lpFileName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
				if (MergeFullFileName(lpFileCopyData[nNumber].lpFileName,lpBrowser->lpCurDir,lpFileName) == FALSE) // 组合成全路径文件名
				{ // 组合失败
					free(lpData); // 释放内存
					RETAILMSG(1,("Get Copy Data Failure \r\n"));
					return FALSE;
				}
			}

			// 设置文件类型
			if (lvItem.iImage == TYPE_FOLDER)
				lpFileCopyData[nNumber].uFileTYPE  = TYPE_FOLDER;
			else
				lpFileCopyData[nNumber].uFileTYPE  = ~TYPE_FOLDER;
			iIndex = SendMessage(hListView, LVM_GETNEXTITEM, iIndex+1, LVNI_SELECTED); // 得到下一个选择文件
			RETAILMSG(1,("Get a File Over\r\n"));
			nNumber ++;
			if (nNumber >= nSelCount)
				break; // 已经得到足够的选择文件
		}

//		RETAILMSG(1,("Get Copy Data Success \r\n"));
//		RETAILMSG(1,("Set Copy Data to clipboard ... \r\n"));
		if (nNumber)
		{
			// 有选择文件
//			OpenClipboard(hWnd);
//			SetClipboardData(CF_FILE,lpData);
//			CloseClipboard();
			if (OpenClipboard(hWnd))  // 打开剪切板
			{
				EmptyClipboard();  // 清空剪切板
				hData = (HGLOBAL)GlobalAlloc(GMEM_MOVEABLE , dwDataLen); // 分配一块全局内存
				if (hData != NULL)
				{
					LPSTR pszData = (LPSTR) GlobalLock(hData); // 得到全局内存地址
					if (pszData)
					{
						memcpy(pszData,lpData,dwDataLen); // 将当前得到的数据复制到全局内存
						GlobalUnlock(hData);
						SetClipboardData(CF_FILE, hData);  // 设置当前数据到剪切板
					}
				}

				CloseClipboard(); // 关闭剪切板
			}
		}

//		RETAILMSG(1,("Set Copy Data to clipboard OK \r\n"));
//		RETAILMSG(1,("Copy Proc success\r\n"));
		return 0;

}
// **************************************************
// 声明：static LRESULT DoPaste(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：从剪切板中粘帖文件到当前的路径。
// 引用: 
// **************************************************
static LRESULT DoPaste(HWND hWnd)
{
	DWORD *lpData;
	LPFILECOPYDATA lpFileCopyData;
	int nCount ;
	char lpOldFileName[MAX_PATH],lpNewFileName[MAX_PATH];
	int i;
	LPBROWSERDATA lpBrowser;
	HWND hListView;
	BOOL bRet = FALSE;
	LPTSTR lpBackupFileName = NULL;
	int bCover = 0;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == 0)
			return 0; // 数据错误

//		OpenClipboard(hWnd);
//		lpData = GetClipboardData(CF_FILE);
//		CloseClipboard();
		if (OpenClipboard(hWnd))  // 打开剪切板
		{
			HGLOBAL hData = GetClipboardData(CF_FILE);  // 得到文件数据
			if (hData != NULL)
			{
				lpData = (DWORD *) GlobalLock(hData); // 得到数据指针
				if (lpData != NULL)
				{

					ShowCopyDialog(hWnd);  // 显示正在复制的对话框
					nCount = *lpData;
					lpFileCopyData = (LPFILECOPYDATA )(lpData+1);  // 得到文件复制数据结构指针
					

					for (i=0; i< nCount;i++)
					{
			//			_splitpath(lpFileCopyData[i].lpFileName,NULL,NULL,lpOldFileName,NULL);
						GetPureFileName(lpFileCopyData[i].lpFileName,lpOldFileName); // 得到纯文件名
			//			_makepath(lpNewFileName,NULL,lpBrowser->lpCurDir,lpOldFileName,NULL);
						if (MergeFullFileName(lpNewFileName,lpBrowser->lpCurDir,lpOldFileName) == FALSE) // 组合当前路径
						{ // 组合失败，不能粘帖文件
//							MessageBox(hWnd,"复制文件失败","错误",MB_OK);
							MessageBox(lpBrowser->hCopyWindow,"复制文件失败","错误",MB_OK);
							
							break;
						}
						if (lpFileCopyData[i].uFileTYPE == TYPE_FOLDER)
						{ // 是一个文件夹
							bRet = CopyFolder(hWnd,lpFileCopyData[i].lpFileName,lpNewFileName,FALSE,FALSE); // 复制文件夹
							if (lpBrowser->bCut == TRUE && bRet == TRUE)
							{
								// 当前是剪切状态，复制完成后要删除原来的文件或文件夹
								bRet = DeleteDirectory(hWnd,lpFileCopyData[i].lpFileName);
								if (bRet == FALSE)
									break;
							}
						}
						else
						{
							if (strcmp(lpFileCopyData[i].lpFileName, lpNewFileName) == 0)
							{
								// 源文件与目的文件夹相同，则备份当前文件
								// 将改变目的文件名
								lpBackupFileName = BackupFileName(lpFileCopyData[i].lpFileName); // 得到备份文件名
								if (lpBackupFileName == NULL)
								{  // 得到文件名失败
					//				MessageBox(hWnd,"复制文件失败","错误",MB_OK);
									bRet = FALSE;
								}
								else
								{  // 复制文件到备份文件名
									bRet = CopyFile(lpFileCopyData[i].lpFileName,lpBackupFileName,FALSE);
									free(lpBackupFileName); // 释放内存
									if (lpBrowser->bCut == TRUE && bRet)
									{
										// 当前是剪切状态，复制完成后要删除原来的文件或文件夹
										bRet = DeleteSpecialFile(hWnd,lpFileCopyData[i].lpFileName); // 删除指定的文件
										if (bRet == FALSE)
											break;
									}
								}
							}
							else
							{
								// 目录不同，将文件复制过来
								if (IsExist(lpNewFileName))
								{ // 文件已经存在
//									if (MessageBox(hWnd,"文件已经存在,是否覆盖?","提示",MB_YESNO) == IDNO)
									if (MessageBox(lpBrowser->hCopyWindow,"文件已经存在,是否覆盖?","提示",MB_YESNO) == IDNO)
									{ // 不覆盖，退出
										break;
									}
								}
								//RETAILMSG(1,("123456\r\n"));
								bRet = CopyFile(lpFileCopyData[i].lpFileName,lpNewFileName,FALSE); // 复制文件
								if (lpBrowser->bCut == TRUE && bRet)
								{
									// 当前是剪切状态，复制完成后要删除原来的文件或文件夹
									bRet = DeleteSpecialFile(hWnd,lpFileCopyData[i].lpFileName); // 删除指定的文件
									if (bRet == FALSE)
										break;
								}
								//RETAILMSG(1,("bRet = %d\r\n",bRet));
							}
						}
						if (bRet == FALSE)
						{  // 复制文件失败
							DWORD err = GetLastError(); 
							TCHAR lpMessage[64];

							sprintf(lpMessage," 复制文件失败(%d) ",err);
//							MessageBox(hWnd,lpMessage,"错误",MB_OK);
							MessageBox(lpBrowser->hCopyWindow,lpMessage,"错误",MB_OK);
							break;
						}
					}

					hListView = GetDlgItem(hWnd,ID_LISTVIEW); // 得到列式句柄
					UpdateListView(hListView); // 更新列式
					SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // 通知数据已经改变

					GlobalUnlock(hData); 
					CloseCopyDialog(hWnd);  // 关闭复制对话框
				}
			}
			if (lpBrowser->bCut)
			{
				//是剪切状态，完成后清空
				lpBrowser->bCut = FALSE; 
				SetClipboardData(CF_FILE,NULL);
			}
			CloseClipboard();// 关闭剪切板
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：创建一个新的目录，处理EXM_NEWFOLDER消息。
// 引用: 
// **************************************************
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	HWND hListView;
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	int iIndex = 1;
	char lpFileName[MAX_PATH];
	int iItem;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类的结构指针
		if (lpBrowser == 0)
			return FALSE; // 指针错误
		if (lpBrowser->bSearch)
			return FALSE; // 在查寻状态下不能创建新目录
		while(1)
		{
			if (strcmp(lpBrowser->lpCurDir,"\\") == 0)  // 当前目录是根目录
				sprintf(lpFileName,"%sFolder%d",lpBrowser->lpCurDir,iIndex); // 得到要新建的目录名
			else
				sprintf(lpFileName,"%s\\Folder%d",lpBrowser->lpCurDir,iIndex);  // 得到要新建的目录名
			hFind=FindFirstFile(lpFileName, &FindFileData  );  // 查找第一个文件
			if( hFind == INVALID_HANDLE_VALUE) 
			{// 文件不存在，可以创建
				break;
			}
			// 该文件已经存在
			FindClose(hFind);  
			iIndex ++; // 得到新的索引来创建目录名
		}
		CreateDirectory(lpFileName,NULL);  // 创建目录
		hListView = GetDlgItem(hWnd,ID_LISTVIEW); // 得到列式句柄
//		UpdateListView(hListView); // 更新列式
		{			
			hFind=FindFirstFile(lpFileName, &FindFileData  );  // 查找第一个文件
			if( hFind == INVALID_HANDLE_VALUE) 
			{// 
				return FALSE;
			}
			iItem = InsertDirectoryItem(hListView,FindFileData,NULL);  // 将当前目录插入到列式
			FindClose(hFind);

			if (iItem >= 0)
			{
				// 有条目存在
				SendMessage(hListView,LVM_SETHOTITEM,iItem,0);  // 设置第一条条目为激活条目
				SendMessage(hListView,LVM_SETSELECTIONMARK,0,iItem);  // 设置第一条条目为选择条目
			}
		}
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // 通知数据已经改变
		return TRUE;  // 返回成功
}
// **************************************************
// 声明：static LRESULT DoIsFind(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：返回当前的查寻状态，是查寻状态，返回TRUE，否则返回FALSE。
// 功能描述：是否在查寻状态，处理EXM_ISFIND消息。
// 引用: 
// **************************************************
static LRESULT DoIsFind(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览结构指针
		if (lpBrowser == 0)
			return FALSE;
		return lpBrowser->bSearch;  // 返回当前的查寻状态
}
// **************************************************
// 声明：static LRESULT DoGetSelFileName(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- int 要得到的文件缓存的大小，至少需要MAX_PATH
// 	IN lParam -- LPTSTR 存放文件名的缓存
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到文件名，处理 EXM_GETSELFILENAME消息。
// 引用: 
// **************************************************
static LRESULT DoGetSelFileName(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	LPTSTR lpFullName;
	TCHAR lpFileName[MAX_PATH];
	int iIndex;
	LVITEM lvItem;
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针
		if (lpBrowser == 0)
			return FALSE; // 数据错误

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
		lpFullName = (LPTSTR)lParam;  // 得到文件缓存的指针
		if (lpFullName == NULL)
			return FALSE; // 缓存地址为空
		if (wParam < MAX_PATH)
			return FALSE; // 缓存大小不够

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式视句柄

		iIndex = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);  // 得到当前的选择条目索引
		if (iIndex == -1)
			return FALSE;  // 当前没有选择条目

		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=iIndex;
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
		SendMessage(hListView,LVM_GETITEM,(WPARAM)0,(LPARAM)&lvItem);  // 得到选择条目的文件名

		if (lvItem.iImage == TYPE_FOLDER)
			return FALSE; // 当前选择是一个目录

		if (lpBrowser->bSearch == TRUE)
		{  // 当前在查寻状态下
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=iIndex;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem);  // 得到文件目录
/*			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}
*/
			if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE)  // 组合文件名和目录
			{
				return FALSE;  // 组合失败
			}
		}
		else
		{
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)  // 组合文件名和当前目录
			{
				return FALSE;  // 组合失败
			}
		}
		return TRUE;
}

// **************************************************
// 声明：static LRESULT DoGetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- int 要保存过滤字串的缓存的大小
// 	IN lParam -- LPTATR 保存过滤字串的缓存
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到过滤串，处理EXM_GETFILTER消息。
// 引用: 
// **************************************************
static LRESULT DoGetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpNewFilter;
	LPBROWSERDATA lpBrowser;
	UINT cchMax ;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针
		if (lpBrowser == 0)
			return FALSE;  // 数据错误

#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		cchMax = (UINT)wParam;  // 得到缓存的最大尺寸
		lpNewFilter = (LPTSTR)lParam;  // 得到缓存的地址
		if (lpNewFilter == NULL)
			return FALSE;  // 缓存地址为空
		if (lpBrowser->lpFilter)
		{ // 当前存在过滤字串
			if (cchMax <= strlen(lpBrowser->lpFilter))  // 比较缓存是否足够
				return FALSE; // 尺寸太小
			strcpy(lpNewFilter,lpBrowser->lpFilter);  // 复制过滤串
		}
		else
		{
			strcpy(lpNewFilter,"");  // 复制空字串
		}
		return TRUE;  // 返回成功
}
// **************************************************
// 声明：static LRESULT DoSetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR 要设置的过滤字串
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：设置过滤串，处理 EXM_SETFILTER 消息
// 引用: 
// **************************************************
static LRESULT DoSetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpNewFilter;
	LPBROWSERDATA lpBrowser;
	HWND hListView ;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类的结构指针
		if (lpBrowser == 0)
			return FALSE; // 数据错误

#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		lpNewFilter = (LPTSTR)lParam;  // 得到过滤字串
		if (lpBrowser->lpFilter)
		{ // 当前已经有过滤字串
			free(lpBrowser->lpFilter);  // 释放原来的过滤字串
			lpBrowser->lpFilter = NULL;
		}
		if (lpNewFilter)
		{ // 新的过滤字串存在
			lpBrowser->lpFilter = (LPTSTR)malloc(strlen(lpNewFilter)+1); // 分配新字串的空间
			strcpy(lpBrowser->lpFilter,lpNewFilter); // 复制新的过滤字串
		}
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式视句柄

		UpdateListView( hListView );  // 更新列式
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // 通知数据已经改变
		return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd --窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPFILEPROPERTY 属性结构指针
// 
// 返回值：
// 功能描述：得到当前属性，处理 EXM_GETPROPERTY 消息。
// 引用: 
// **************************************************
static LRESULT DoGetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	LPFILEPROPERTY lpFileProperty;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
//	DWORD dwAttributes;
	DWORD dwFreeBytesAvailable,dwTotalNumberOfBytes,dwTotalNumberOfFreeBytes;
	int iIndex;
	LVITEM lvItem;
	HWND hListView;
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	DWORD dwSelCount;
	UINT nFolderNum = 0,nFileNum = 0;


		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针
		if (lpBrowser == 0)
			return FALSE;  // 数据错误

#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		lpFileProperty = (LPFILEPROPERTY)lParam;  // 得到属性结构指针

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);

		dwSelCount = SendMessage(hListView, LVM_GETSELECTEDCOUNT, 0, 0); // 得到当前的选择个数
		lpFileProperty->dwTotalSize = 0;
		iIndex = -1;
		if (dwSelCount)
		{  // 当前有选择
			while(1)
			{
				iIndex = SendMessage(hListView, LVM_GETNEXTITEM, iIndex, LVNI_SELECTED);  // 得到当前选择的索引
				if (iIndex == -1)
				{ // 已经没有选择了
					if (dwSelCount > 1)  
					{  // 当前的选择个数大于1
						sprintf(lpFileProperty->lpName,"%d个文件夹,%d个文件",nFolderNum,nFileNum); //设置当前的选择属性字串
//						strcpy(lpFileProperty->lpName,lpFileName);
						lpFileProperty->dwFileAttributes = 0;  // 没有文件属性
						return TRUE;
					}
					return FALSE;
				}

				lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
				lvItem.iItem=iIndex;
				lvItem.iSubItem=0;
				lvItem.pszText = lpFileName;
				lvItem.cchTextMax = MAX_PATH;
				SendMessage(hListView,LVM_GETITEM,(WPARAM)0,(LPARAM)&lvItem);  // 得到当前选择的文件名

				if (lpBrowser->bSearch == TRUE)
				{  // 当前处于查寻状态
					lvItem.mask  = LVIF_TEXT ;
					lvItem.iItem=iIndex;
					lvItem.iSubItem=3;
					lvItem.pszText = lpFullName;
					lvItem.cchTextMax = MAX_PATH;
					SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem);  // 得到选择条目的目录

					strcpy(lpFileProperty->lpPath,lpFullName);  // 复制到属性当前文件的目录
					if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE)  // 组合成为全路径文件
					{
						return FALSE;
					}
				}
				else
				{  // 是一般指定目录文件
					strcpy(lpFileProperty->lpPath,lpBrowser->lpCurDir);// 复制到属性当前文件的目录
					if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)// 组合成为全路径文件
					{
						return FALSE;
					}
				}
				if (dwSelCount > 1)
				{  // 当前选择个数大于1
					hFind=FindFirstFile(lpFullName, &FindFileData  );  // 查找第一个文件
					if( hFind == INVALID_HANDLE_VALUE) 
					{// 
						return FALSE;
					}
					else
						FindClose(hFind);
					if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{  // 当前选择是一个文件夹，文件夹个数加1
						nFolderNum ++;
					}
					else
					{  // 当前选择是一个文件，当前文件数加1
						nFileNum ++;
						lpFileProperty->dwTotalSize += FindFileData.nFileSizeLow;  // 加上文件大小
					}
					iIndex ++; // 下一个选择索引
					continue;
				}
				break;
			}
		}
		else
		{
			// 没有选择的文件
			if (lpBrowser->bSearch == TRUE || strcmp(lpBrowser->lpCurDir,"\\") == 0)
			{  // 在查寻状态或当前是根目录，得到系统磁盘的状态
//				strcpy(lpFullName,"\\");
//				strcpy(lpFileProperty->lpPath,"\\");
//				strcpy(lpFileName,"系统磁盘");
				strcpy(lpFileProperty->lpName,"系统磁盘");
//				lpFileProperty->ftCreationTime = 0;
//				lpFileProperty->ftLastAccessTime = 0;
				lpFileProperty->dwFileAttributes = FILE_ATTRIBUTE_DEVICE;

				if (GetDiskFreeSpaceEx("\\",&dwFreeBytesAvailable,&dwTotalNumberOfBytes,&dwTotalNumberOfFreeBytes)) // 得到磁盘的空间大小
				{
					lpFileProperty->dwFreeSize = dwTotalNumberOfFreeBytes;
					lpFileProperty->dwTotalSize = dwTotalNumberOfBytes;
					return TRUE;
				}
				return NULL;
			}
			else
			{
				strcpy(lpFullName,lpBrowser->lpCurDir); // 复制当前目录名
				GetPureFileName(lpFullName,lpFileName);  // 得到目录名
				_splitpath(lpBrowser->lpCurDir,NULL,lpFileProperty->lpPath,NULL,NULL); // 得到当前路径
			}
		}

//		dwAttributes = GetFileAttributes(lpFullName);
//		if (dwAttributes & FILE_ATTRIBUTE_DEVICE)
		hFind=FindFirstFile(lpFullName, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{ // 没有找到文件
			return FALSE;
		}
		else
			FindClose(hFind);
		strcpy(lpFileProperty->lpName,lpFileName);  // 复制当前的文件名
		// 得到文件的的属性
		lpFileProperty->ftCreationTime = FindFileData.ftCreationTime;
		lpFileProperty->ftLastAccessTime = FindFileData.ftLastAccessTime;
		lpFileProperty->dwFileAttributes = FindFileData.dwFileAttributes;

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
		{  // 当前是一个设备
			strcat(lpFullName,"\\");
			if (GetDiskFreeSpaceEx(lpFullName,&dwFreeBytesAvailable,&dwTotalNumberOfBytes,&dwTotalNumberOfFreeBytes)) // 得到设备的信息
			{
				lpFileProperty->dwFreeSize = dwTotalNumberOfFreeBytes;  // 剩余空间
				lpFileProperty->dwTotalSize = dwTotalNumberOfBytes; // 设备总空间
				return TRUE;
			}
			return FALSE;
		}
		lpFileProperty->dwFreeSize = 0;  // 设置剩余空间
		lpFileProperty->dwTotalSize = FindFileData.nFileSizeLow; // 当前文件的尺寸
		return TRUE;
}

// **************************************************
// 声明：static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
// 参数：
// 	IN lpFullFileName -- 全文件名，包含路径
// 	OUT lpPureFileName -- 纯文件名，不包含路径
// 
// 返回值：无
// 功能描述：从一个全路径的文件名中提取一个不含路径的文件名
// 引用: 
// **************************************************
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
{
	const char *ptr = lpFullFileName;
	int length;

//		ptr=path;
		// Search '.'
		// 将指针指向文件名结尾
		while(*ptr)
		{
			ptr++;
		}
		// Get filename
		length=0;
		while(ptr>lpFullFileName)
		{ // 还没有到达文件头
			ptr--;  // not include '\' and '.'
			if (*ptr=='\\')
			{  // 遇到文件路径分隔
				ptr++;  // not includ符e '\' 
				break; // 退出，后面的就是不包含路径的文件名
			}
			length++;
		}
		if (lpPureFileName)
		{
			strcpy(lpPureFileName,ptr); // 复制纯文件名到缓存
		}
}
// **************************************************
// 声明：static BOOL CopyFolder(HWND hWnd,LPCTSTR lpExistingFolderName, LPCTSTR lpNewFolderName, BOOL bFailIfExists,BOOL bCover)
// 参数：
// 	IN
// 	OUT
// 	IN/OUT
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static BOOL CopyFolder(HWND hWnd,LPCTSTR lpExistingFolderName, LPCTSTR lpNewFolderName, BOOL bFailIfExists,BOOL bCover)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	LPTSTR lpSearchString = NULL;
	LPTSTR lpNewFileName = NULL,lpOldFileName = NULL,lpBackupFileName = NULL;
	BOOL bRet = FALSE;
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == 0)
			return FALSE; // 错误数据

	
		hFind=FindFirstFile(lpExistingFolderName, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 源文件夹不存在，不需要再进行复制
			return FALSE;
		}
		else
			FindClose(hFind);  // 关闭查找

		if (IsChildDir(lpExistingFolderName, lpNewFolderName))
		{ // 目标文件夹不能是源文件夹的子目录
			MessageBox(hWnd,"不能复制，目标文件夹是源文件夹的子目录。","错误",MB_OK);
			return FALSE;
		}
		if (strcmp(lpExistingFolderName, lpNewFolderName) == 0)
		{
			// 源文件夹与目的文件夹相同，则备份当前文件夹
			// 将改变目的文件夹名
			lpBackupFileName = (LPTSTR)BackupFileName((LPTSTR)lpExistingFolderName); // 得到备份文件夹名
			if (lpBackupFileName == NULL)
			{  // 备份文件名错误
//				MessageBox(hWnd,"复制文件失败","错误",MB_OK);
				return FALSE;
			}
			lpNewFolderName = lpBackupFileName; // 得到备份文件名
		}
		if ( bCover == FALSE )
		{ // 需要提示是否覆盖
			if (IsExist((LPTSTR)lpNewFolderName))
			{ // 文件夹已经存在
				if (MessageBox(hWnd,"目录已经存在,是否覆盖?","提示",MB_YESNO) == IDNO)
				{ // 不覆盖
					return TRUE;
				}
			}
		}
		lpSearchString = (LPTSTR)malloc(MAX_PATH); // 分配查询字串缓存
		if (lpSearchString == NULL)
			goto COPY_OVER;
		lpNewFileName = (LPTSTR)malloc(MAX_PATH); // 分配新文件名缓存
		if (lpNewFileName == NULL)
			goto COPY_OVER;
		lpOldFileName = (LPTSTR)malloc(MAX_PATH);  // 分配旧文件名缓存
		if (lpOldFileName == NULL)
			goto COPY_OVER;

		hFind=FindFirstFile(lpNewFolderName, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 文件夹不存在
			CreateDirectory(lpNewFolderName,NULL); // 创建一个新的文件夹
		}
		else
			FindClose(hFind); // 关闭查寻

/*		strcpy(lpSearchString,lpExistingFolderName);
		if (strcmp(lpSearchString,"\\") == 0)
		{
			strcat(lpSearchString,"*.*");
		}
		else
		{
			strcat(lpSearchString,"\\*.*");
		}
*/
		if (MergeFullFileName(lpSearchString,lpExistingFolderName,"*.*") == FALSE)  // 得到要查询的文件串
		{
			bRet = FALSE;
			goto COPY_OVER;
		}
		hFind=FindFirstFile(lpSearchString, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 文件夹不存在
			bRet = TRUE;
			goto COPY_OVER;
		}
		while(1)
		{
			if(FindFileData.cFileName[0]=='.')
			{  // 该文件是一个表示当前目录是子目录的文件，跳过。
			}
			else 
			{
//				_makepath(lpOldFileName,NULL,lpExistingFolderName,FindFileData.cFileName,NULL);
				if (MergeFullFileName(lpOldFileName,lpExistingFolderName,FindFileData.cFileName) == FALSE) // 组合旧文件名
				{
					bRet = FALSE;
					goto COPY_OVER;
				}
//				_makepath(lpNewFileName,NULL,lpNewFolderName,FindFileData.cFileName,NULL);
				if (MergeFullFileName(lpNewFileName,lpNewFolderName,FindFileData.cFileName) == FALSE)  // 组合新文件名
				{
					bRet = FALSE;
					goto COPY_OVER;
				}
				if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{ // 当前是文件夹
					bRet = CopyFolder(hWnd,lpOldFileName,lpNewFileName,bFailIfExists,TRUE); // 复制文件夹
					if (bRet == FALSE)
						goto COPY_OVER;
					if (lpBrowser->bCut == TRUE && bRet == TRUE)
					{
						// 当前是剪切状态，复制完成后要删除原来的文件或文件夹
						bRet = DeleteDirectory(hWnd,lpOldFileName);
						if (bRet == FALSE)
							goto COPY_OVER;
					}
				}
				else
				{ // 当前是文件
					bRet = CopyFile(lpOldFileName,lpNewFileName,bFailIfExists); // 复制文件
					if (bRet == FALSE)
						goto COPY_OVER;
					if (lpBrowser->bCut == TRUE && bRet)
					{
						// 当前是剪切状态，复制完成后要删除原来的文件或文件夹
						bRet = DeleteSpecialFile(hWnd,lpOldFileName); // 删除指定的文件
						if (bRet == FALSE)
							goto COPY_OVER;
//							return FALSE;
					}
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //查找下一个文件
			{ // 已经没有文件了
				break;
			}
		}
		FindClose(hFind);  // 关闭查寻
		bRet = TRUE;

COPY_OVER:
		// 释放内存空间
		if (lpSearchString)
			free(lpSearchString);
		if (lpNewFileName)
			free(lpNewFileName);
		if (lpOldFileName)
			free(lpOldFileName);
		if (lpBackupFileName)
			free(lpBackupFileName);
		return bRet;

}

// **************************************************
// 声明：static BOOL SearchCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen )
// 参数：
// 	IN lpcszMask -- 要比较的字串
// 	IN iMaskLen -- 字串长度
// 	IN lpcszSrc -- 要比较的文件名
// 	IN iSrcLen -- 文件名长度
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static BOOL SearchCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen )
{
	LPTSTR lpFileName = NULL;
	if (IsAsterisk(lpcszMask,iMaskLen)==TRUE) // 是否含有通配符
	{  // 含有通配符
		// !!! Add By Jami chen in 2003.09.28
		lpFileName = GetOnlyFileName((LPTSTR)lpcszSrc); // 得到纯文件名
		// !!! Add End By Jami chen in 2003.09.28
//		RETAILMSG(1,(" 11 Compare <%s> and <%s>\r\n",lpcszMask,lpcszSrc));
		// !!! Modified By Jami chen in 2003.09.28
//		return FileNameCompare( lpcszMask, iMaskLen, lpcszSrc, iSrcLen);
		return FileNameCompare( lpcszMask, iMaskLen, lpFileName, iSrcLen); // 通配符文件比较
		// !!! Modified End By Jami chen in 2003.09.28
	}
	else
	{ // 不含有通配符
//		RETAILMSG(1,(" 00 Compare <%s> and <%s>\r\n",lpcszMask,lpcszSrc));
		return StrnStr(lpcszSrc, lpcszMask, iMaskLen);	 // 比较指定字串是否包含在文件名中
	}
}
// **************************************************
// 声明：static BOOL StrnStr(const char *s1, const char *s2,int nLen)
// 参数：
// 	IN s1 -- 字串1
// 	IN s2 -- 字串2
// 	IN nLen -- 字串2的长度
// 
// 返回值：字串1 包含字串2 返回TRUE，否则返回FALSE
// 功能描述：查寻字串1是否包含字串2。
// 引用: 
// **************************************************
static BOOL StrnStr(const char *s1, const char *s2,int nLen)
{
    const unsigned char * p1, *p2, *ps;
    unsigned char c;
    int l, i;

    p1 = s1, p2 = s2;
    ps = p1;
//    l = strlen( s2 );
    l = nLen;  // 得到查寻字串长度
    while( l )
    {
        // find first match char c
        c = toupper(*p2++); // 将字符转化为大写，不区分大小写
        while( 1 )
        {
            if( c == toupper(*p1) ) // 找到第一个与字串2第一个字符相同的字符
                break;
            if( *p1 == 0 )
                return FALSE;
            p1++;
        }
        ps = p1;  // save first address
        p1++;

        // other chars match ?
        for( i = 1; i < l; i++, p1++, p2++ )
        {
            if( *p2 == 0 )
            {
                p2 = s2;  // reset p2 to start address
                p1 -= l;  //
                ps = p1;  // save p1 address
            }
            if( toupper(*p1) != toupper(*p2) )
                break;
            if( *p1 == 0 )
                return FALSE;
        }
        if( i == l )   // all char is match
            return TRUE;
        p1++;
    }
    return FALSE;
}

// **************************************************
// 声明：static BOOL IsAsterisk(LPCTSTR lpcszMask, int iMaskLen)
// 参数：
// 	IN lpcszMask -- 字串的指针
// 	IN iMaskLen -- 字串长度
// 
// 返回值：含通配符，返回TRUE，否则返回FALSE
// 功能描述：查寻指定字串中是否含有通配符。
// 引用: 
// **************************************************
static BOOL IsAsterisk(LPCTSTR lpcszMask, int iMaskLen)
{
	int i = 0;

		while(1)
		{
			if ( *lpcszMask == '*' || *lpcszMask == '?' ) // 当前字符是否是通配符
				return TRUE; // 是通配符。返回TRUE。
			i++ ;
			lpcszMask++ ;
			if (i >= iMaskLen)
				break;
		}	
		return FALSE; //没有通配符，返回FALSE
}

// **************************************************
// 声明：static BOOL CallBackFindFile(HWND hWnd ,FILE_FIND_DATA	FindFileData,LPTSTR lpCurDir)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN FindFileData -- 查找结果结构指针
// 	IN lpCurDir -- 当前路径
// 
// 返回值：需要保留该查找结果返回TRUE，否则返回FALSE
// 功能描述：呼叫查找回调函数
// 引用: 
// **************************************************
static BOOL CallBackFindFile(HWND hWnd ,FILE_FIND_DATA	FindFileData,LPTSTR lpCurDir)
{
	LPBROWSERDATA lpBrowser;
	char lpFileName[MAX_PATH];

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == 0)
			return FALSE; // 错误数据

		if (lpBrowser->FindFileFunc != NULL)
		{ // 有回调函数
			_makepath( lpFileName, NULL, lpCurDir, FindFileData.cFileName, NULL); // 得到全路径文件名
			return lpBrowser->FindFileFunc(lpFileName,&FindFileData); // 调用应用程序回调函数
		}
		return TRUE;
}

// **************************************************
// 声明：static BOOL IsChildDir(LPCTSTR lpParentFolderName, LPCTSTR lpChildFolderName)
// 参数：
// 	IN lpParentFolderName -- 父文件夹
// 	IN lpChildFolderName -- 子文件夹
// 
// 返回值：如果子文件夹是父文件夹的子目录，返回TRUE，否则返回FALSE。
// 功能描述：判断子文件夹是否是父文件夹的子目录。
// 引用: 
// **************************************************
static BOOL IsChildDir(LPCTSTR lpParentFolderName, LPCTSTR lpChildFolderName)
{
	if (strnicmp(lpParentFolderName, lpChildFolderName,strlen(lpParentFolderName)))  // 子文件夹是否包含父文件夹的字串
		return FALSE;
//	if (lpChildFolderName[strlen(lpParentFolderName)] == '\\' || lpChildFolderName[strlen(lpParentFolderName)] == 0)
	if (lpChildFolderName[strlen(lpParentFolderName)] == '\\') // 子文件夹在父文件夹的结束位置是一个目录分隔符，防止
		return TRUE;										   // 以下问题 \aaa\bbbbb\ccc 看成是\aaa\bbb 的子目录的现象
	return FALSE;
}

/*
	if lpPath == NULL , then the lpFullName has Include the path

*/

// **************************************************
// 声明：static BOOL MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName)
// 参数：
// 	OUT lpFullName -- 保存全路径文件名的缓存
// 	IN lpPath -- 当前路径
// 	IN lpFileName -- 当前文件名
// 
// 返回值：成功返回TRUE,否则返回FALSE
// 功能描述：组合当前路径和当前文件名为一个全路径文件
// 引用: 
// **************************************************
static BOOL MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName)
{
	if (lpPath != NULL)
		strcpy(lpFullName,lpPath);  // 复制路径到全路径名
	if (strcmp(lpFullName,"\\") == 0)
	{ // 当前路径是根目录
		if ((strlen(lpFullName) + strlen(lpFileName)) >= MAX_PATH) // 得到全路径的文件名是否超过系统的最大值
			return FALSE;
		strcat(lpFullName,lpFileName);  // 合成全路径文件
	}
	else
	{
		if ((strlen(lpFullName) + strlen(lpFileName) +1) >= MAX_PATH) // 得到全路径的文件名是否超过系统的最大值
			return FALSE;
		strcat(lpFullName,"\\"); // 添加目录分隔符
		strcat(lpFullName,lpFileName); // 合成全路径文件
	}
	return TRUE;  // 返回成功
}

// !!! Add By Jami chen in 2003.09.09
// **************************************************
// 声明：static LRESULT DoRefresh(HWND hWnd)
// 参数：
// 	IN 窗口句柄
// 
// 返回值：无
// 功能描述：重新更新文件浏览类的内容
// 引用: 
// **************************************************
static LRESULT DoRefresh(HWND hWnd)
{
  HWND hListView;

	hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式的句柄
	UpdateListView(hListView); // 更新列式的内容
	return 0;
}
// !!! Add End By Jami chen in 2003.09.09

// !!! Add By Jami chen in 2003.09.13
// **************************************************
// 声明：static LPTSTR BackupFileName(LPTSTR lpExistingFolderName)
// 参数：
// 	IN lpExistingFolderName -- 已经存在的文件夹名
// 
// 返回值：返回备份文件夹名的指针
// 功能描述：得到一个备份文件夹名。
// 引用: 
// **************************************************
static LPTSTR BackupFileName(LPTSTR lpExistingFolderName)
{
	LPTSTR lpBackupFileName;
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	int iIndex = 1;
	int iDirlen = 0,iFilenameLen = 0;
	TCHAR lpDir[MAX_PATH],lpFileName[MAX_PATH];

		if (strcmp(lpExistingFolderName,"\\") == 0) // 是否是根目录
			return NULL; // Root Floder can't backup

		lpBackupFileName = (LPTSTR)malloc(MAX_PATH); // 分配一个备份文件名的缓存
		if (lpBackupFileName == NULL)
			return NULL; // 分配失败

		strcpy(lpDir,lpExistingFolderName); // 复制当前已经存在的文件夹名到目录缓存
		
		// 得到当前文件的目录
		iIndex = strlen(lpDir); // 得到文件的长度
		// 从后往前搜索目录分隔符"\"
		while(1)
		{
			if (lpDir[iIndex] == '\\')
			{ // 是分隔符"\"
				lpDir[iIndex + 1] = 0; // 将后面的内容清掉
				break;
			}
			if (iIndex == 0)
			{ // 如果没有找到分隔符，返回失败
				free(lpBackupFileName);
				return FALSE;
			}
			iIndex --;
		}
		strcpy(lpFileName,lpExistingFolderName + iIndex +1);  // 得到文件名
		iDirlen = strlen(lpDir); // 得到目录长度
		iFilenameLen = strlen(lpFileName); // 得到文件名长度
		
		// 然后在其后面添加一个序号
		iIndex = 1;
		while(1)
		{
			if ((iDirlen + iFilenameLen + 10) >= MAX_PATH)
			{ // 判断备份文件名的长度是否超出限制
				free(lpBackupFileName);
				return FALSE;
			}
			sprintf(lpBackupFileName,"%sBK_%d_%s",lpDir,iIndex,lpFileName);  // 得到备份文件名
			// 查找该文件是否已经存在
			hFind=FindFirstFile(lpBackupFileName, &FindFileData  );  // 查找第一个文件
			if( hFind == INVALID_HANDLE_VALUE) 
			{// 文件不存在，可以使用该文件名
				break;
			}
			FindClose(hFind); // 关闭查找
			iIndex ++; // 使用下一个索引
		}
		return lpBackupFileName; // 返回备份文件名
}
// !!! Add End By Jami chen in 2003.09.13

// !!! Add By Jami chen in 2003.09.23
// **************************************************
// 声明：static BOOL IsExist(LPTSTR lpNewFileName)
// 参数：
// 	IN lpNewFileName -- 指定文件名
// 
// 返回值：文件存在返回TRUE，否则返回FALSE。
// 功能描述：判断指定文件是否存在。
// 引用: 
// **************************************************
static BOOL IsExist(LPTSTR lpNewFileName)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;

		hFind=FindFirstFile(lpNewFileName, &FindFileData  );  // 查找第一个文件
		if( hFind == INVALID_HANDLE_VALUE) 
		{// 文件不存在
			return FALSE;
		}
		else
		{ // 文件存在
			FindClose(hFind);
			return TRUE;
		}
}
// **************************************************
// 声明：static BOOL DeleteSpecialFile(HWND hWnd,LPTSTR lpFileName)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpFileName -- 指定文件
// 
// 返回值：成功删除返回TRUE，否则返回FALSE
// 功能描述：删除指定文件。
// 引用: 
// **************************************************
static BOOL DeleteSpecialFile(HWND hWnd,LPTSTR lpFileName)
{
	TCHAR lpMessage[MAX_PATH + 64];
	DWORD dwImportFile = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ROMMODULE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY;
	DWORD dwFileAtt;
	BOOL bRet;
	WORD dwRet;

		
		dwFileAtt = GetFileAttributes(lpFileName);  // 得到文件属性

		if (dwFileAtt & dwImportFile)
		{ // 指定文件是重要文件，提示是否要删除
			sprintf(lpMessage,"(%s)是重要文件，确认要删除吗?",lpFileName);
			if ((dwRet = MessageBox(hWnd,lpMessage,"提示",MB_YESNO)) == IDNO)
				return FALSE; // 不要删除
			SetFileAttributes(lpFileName,FILE_ATTRIBUTE_ARCHIVE); // 要删除，设置文件属性为一般文件，否则不能删除
		}

		bRet = DeleteFile(lpFileName);  // 删除文件
		if (bRet == FALSE)
		{ // 删除文件失败
			DWORD err = GetLastError();  // 得到错误代码

				sprintf(lpMessage," 不能删除文件(%d) \r\n(%s)",err,lpFileName);
				MessageBox(hWnd,lpMessage,"错误",MB_OK); // 显示删除错误，并错误代码
				return FALSE;
		}
		return TRUE; // 删除成功
}
// !!! Add End By Jami chen in 2003.09.23


// !!! Add By Jami chen in 2003.09.25
// For do the copy dialog
const struct dlgIDD_CopyDialog{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[1];
}dlgIDD_CopyDialog = {
    { WS_EX_NOMOVE,WS_POPUP|WS_CAPTION|WS_VISIBLE,1,0,100,240,104,0,0,"文件管理" },
    {
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,15,15,220,24,0xFFFF,classSTATIC,"正在拷贝文件...",0 },
    } };

#define CPM_CLOSE		(WM_USER + 1235)

//static HANDLE g_hThread = NULL;
//static HWND g_hCopyDlg = NULL;

static LRESULT CALLBACK CopyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI CopyDialogThread(VOID * pParam);
static LRESULT DoCopyInitial(HWND hDlg,WPARAM wParam,LPARAM lParam);


// **************************************************
// 声明：static void ShowCopyDialog(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：显示正在复制文件对话框。
// 引用: 
// 提示：显示方法是先创建一个线程，有该线程弹出提示框，并等待复制完成
//	   而当前线程则继续进行复制过程，复制完成，发送消息杀死对话框。
// **************************************************
static void ShowCopyDialog(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;
	HANDLE handle;
	int iTimes = 0;

		handle = CreateThread(NULL, 0, CopyDialogThread, (void *)hWnd, 0, 0 );  // 创建一个线程
		CloseHandle(handle); // 关闭句柄，创建成功后，该句柄无效。

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == 0)
			return ; // 错误数据
		while(1)
		{
			if (lpBrowser->hCopyWindow)
				break; // 窗口已经创建完成
			Sleep(50); // 等待创建窗口
			if (iTimes >= 300)
				break; // 最长等待9 秒
			iTimes ++;
		}
}
// **************************************************
// 声明：static void CloseCopyDialog(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：关闭复制对话框。
// 引用: 
// **************************************************
static void CloseCopyDialog(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到浏览类结构指针
		if (lpBrowser == 0)
			return ; // 错误数据
		if (lpBrowser->hCopyWindow)
		{ // 已经创建了复制对话框
			RETAILMSG(1,(TEXT("End Dialog !!!!\r\n")));
			//EndDialog(g_hCopyDlg,0);
			//DestroyWindow(g_hCopyDlg);
			PostMessage(lpBrowser->hCopyWindow,CPM_CLOSE,0,0); // 发送消息退出该对话框
			lpBrowser->hCopyWindow = NULL;
	//		CloseHandle(g_hThread);
		}
}
// ********************************************************************
// 声明：static LRESULT CALLBACK CopyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：应用程序游戏结束窗口处理过程
// 引用: 
// ********************************************************************
static LRESULT CALLBACK CopyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:  // 绘制消息
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_INITDIALOG:  // 初始化对话框
			DoCopyInitial(hDlg,wParam,lParam);
//			g_hCopyDlg = GetWindow(hDlg,GW_OWNER);
			return 0;
		case CPM_CLOSE: // 关闭对话框
			EndDialog(hDlg,0);
			return TRUE;
		default:
			return 0;
	}
}
// ********************************************************************
// 声明：static LRESULT DoCopyInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
// 参数：
//	  IN hWnd - 窗口句柄
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：初始化复制显示窗口
// 引用: 
// ********************************************************************
static LRESULT DoCopyInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	HWND hParent;
	LPBROWSERDATA lpBrowser;

		hParent = (HWND)lParam;
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hParent,0);  // 得到浏览类结构指针
		if (lpBrowser == 0)
			return -1; // 错误数据

		lpBrowser->hCopyWindow = hDlg;
		return 0;
}


// **************************************************
// 声明：static DWORD WINAPI CopyDialogThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：复制窗口线程。
// 引用: 
// **************************************************
static DWORD WINAPI CopyDialogThread(VOID * pParam)
{
	HINSTANCE hInstance;
	HWND hWnd;

		hWnd = (HWND)pParam; // 得到窗口句柄
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
//		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_CopyDialog,hWnd, (DLGPROC)CopyDialogProc); // 创建对话框
		DialogBoxIndirectParamEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_CopyDialog,hWnd, (DLGPROC)CopyDialogProc,(LONG)hWnd); // 创建对话框
//		hDlg = CreateDialogIndirect(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_CopyDialog,hWnd, (DLGPROC)CopyDialogProc);
//		UpdateWindow( hDlg );
//	HWND hWnd; 
//			
//		hWnd = (HWND)pParam;
//		WaitForSingleObject(g_hKillDlgEvent,INFINITE);
		return 0;
}
// !!! Add End By Jami chen in 2003.09.25

// !!! Add By Jami chen in 2003.09.28
// **************************************************
// 声明：static LPTSTR GetOnlyFileName(LPTSTR lpFullName)
// 参数：
// 	IN lpFullName -- 全路径文件名
// 
// 返回值：返回纯文件名。
// 功能描述：得到当前文件的纯文件名。
// 引用: 
// **************************************************
static LPTSTR GetOnlyFileName(LPTSTR lpFullName)
{
	LPTSTR lpCurPos;

		lpCurPos = lpFullName + strlen(lpFullName); // 得到当前文件名的最后一个字符的位置
		while(lpCurPos!=lpFullName)
		{ // 没有到达第一个字符
			if (*lpCurPos == '\\')
			{ // 找到目录分隔符，后面的内容就是纯文件名
				return (lpCurPos+1);
			}
			lpCurPos --;
		}
		return lpFullName; // 没有找到目录分隔符，本来就是一个纯文件名。
}
// !!! Add End By Jami chen in 2003.09.28

/**************************************************
声明：static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：设置焦点，处理WM_SETFOCUS消息。
引用: 
************************************************/
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄
		return (LRESULT)SetFocus(hListView);
}



// **************************************************
// 声明：static LRESULT DoCut(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：将选择条目的文件名剪切到剪切板。
// 引用: 
// **************************************************
static LRESULT DoCut(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;

	
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针
		if (lpBrowser == 0)
			return 0; // 错误数据

		DoCopy(hWnd); // 首先要复制到剪切板
		//设置剪切标志
		lpBrowser->bCut = TRUE;

		return 0;
}


// **************************************************
// 声明：static LRESULT DoOpenCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：打开当前选择的文件。
// 引用: 
// **************************************************
static LRESULT DoOpenCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListWnd;
	LVITEM	lvItem;
//	LPNMITEMACTIVATE lpnmia;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
	LPBROWSERDATA lpBrowser;
	int iCurSelItem;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // 得到文件浏览类的结构指针

		hListWnd = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄
		iCurSelItem = SendMessage(hListWnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);  // 得到当前的选择条目索引
		if (iCurSelItem == -1)
			return 0; // 当前没有选择条目

		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=iCurSelItem;  // 得到指定条目索引
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
//		SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)iItem,(LPARAM)&lvItem);
		SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // 得到条目文件名

		if (lpBrowser->bSearch == TRUE)
		{  // 当前为查寻状态
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=iCurSelItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);  // 得到文件目录

/*			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}
*/
			if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE) // 组合文件全路径
				return 0;
		}
		else
		{
			if (strcmp(lpFileName,".") == 0)
			{
				// 当前目录，不需要处理
				return 0;
			}
			if (strcmp(lpFileName,"..") == 0)
			{
				// 到上一级目录
				DoUpPath(hWnd,0,0);
				return 0;
			}
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)  // 组合文件全路径
				return 0;
		}

		if (lvItem.iImage == TYPE_FOLDER)
		{ // 当前是一个目录
			HWND hListView ;
	
				hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄
				if (lpBrowser->bSearch)
				{  // 当前在查寻状态
					lpBrowser->bSearch = FALSE;
					DeleteListViewColumn(hListView,ID_DIRCOLUMN); // 删除目录列
					if (lpBrowser->lpSearch)
					{
						free(lpBrowser->lpSearch);  // 释放查寻串
						lpBrowser->lpSearch = NULL;
					}
					lpBrowser->FindFileFunc = NULL;  // 设置回调为空
				}
				strcpy(lpBrowser->lpCurDir,lpFullName);  // 设置当前目录为激活的目录
				UpdateListView( hListView ); // 更新列式
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // 发送通知消息数据改变
		}
		else
		{

			SendNormalNotify(hWnd,EXN_ACTIVEITEM,NULL);  // 发送条目激活的通知
		}

        return 0;
}


// **************************************************
// 声明：static LRESULT DoRenameCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN wParam -- 参数
//  IN lParam -- 参数
// 
// 返回值：无
// 功能描述：打开当前选择的文件。
// 引用: 
// **************************************************
static LRESULT DoRenameCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	int iIndex;

		RETAILMSG(1,(" Will Rename File \r\n"));
		
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄
		RETAILMSG(1,(" Get ListView hwnd = %x \r\n",hListView));
		
		iIndex = SendMessage(hListView,LVM_GETSELECTIONMARK,0,0); // 得到当前选择
		if (iIndex == -1)
			return FALSE;
		RETAILMSG(1,(" Send EDITLABEL to ListView  %d \r\n",iIndex));
		SendMessage(hListView,LVM_EDITLABEL,iIndex,0); // 编辑标签
		return TRUE;
}


// ********************************************************************
// 声明：static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：
//	
// 功能描述：窗口大小发生改变
// 引用: 
// ********************************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	RECT rect;
	int iIndex;

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // 得到列式句柄

		GetClientRect(hWnd,&rect); // 得到当前文件浏览器的窗口大小
		// 设置列表的窗口大小与文件浏览器的窗口大小一致
		SetWindowPos(hListView,NULL,0, 0, rect.right, rect.bottom,SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
		
		iIndex = SendMessage(hListView,LVM_GETSELECTIONMARK,0,0); // 得到当前选择
		if (iIndex == -1)
			return FALSE;
		SendMessage(hListView,LVM_ENSUREVISIBLE,iIndex,FALSE); // 设置
		return 0;
}


// ********************************************************************
// 声明：static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPWINDOWPOS 指针 
// 返回值：
//	
// 功能描述：窗口位置发生改变
// 引用: 
// ********************************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpWindowPos;

		lpWindowPos = (LPWINDOWPOS)lParam;

		if ((lpWindowPos->flags & SWP_NOSIZE) == 0)
		{
			// 窗口大小发生改变
			DoSize(hWnd,0,0);
		}

		return 0;
}
