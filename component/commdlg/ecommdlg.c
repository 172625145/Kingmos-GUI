/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：打开/保存文件对话框，用户界面部分
版本号：1.0.0
开发时期：2003-04-02
作者：陈建明 Jami chen
修改记录：
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include "Explorer.h"
#include "efilepth.h"
#include <eCommDlg.h>
#include "eClipbrd.h"

/***************  全局区 定义， 声明 *****************/

#define OPENFILEFLAG	1
#define SAVEFILEFLAG	2
typedef struct OpenFileStruct{
	LPOPENFILENAME lpofn;
	UINT uFileOpenFlag;  // 打开 (OPENFILEFLAG)  或 保存 (SAVEFILEFLAG)
}OPENFILESTRUCT , *LPOPENFILESTRUCT;

#define ID_NEWFOLDER		101
#define ID_TOHIGHERLEVEL	102
//#define ID_SHOWMODE			103
#define IDC_FILENAME		105
#define ID_OK				106
#define IDC_FILETYPE		107
#define ID_CANCEL			108
#define IDC_CURPATH			109
#define ID_BROWSER			120


static BOOL CALLBACK OpenDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);

static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDrawItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static ATOM MyRegisterClass(HINSTANCE hInstance);
//static HWND InitInstance(HINSTANCE hInstance, LPOPENFILENAME lpofn);
//static BOOL FillListCtrl(HWND hWnd,LPOPENFILENAME lpofn);
static void FillFileType(HWND hWnd,LPOPENFILENAME lpofn);
//static void InsertListItem(HWND hWnd,FILE_FIND_DATA   find,BOOL bDir);
//static void RemoveAllItem(HWND hWnd);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL GetSelectFileName(HWND hWnd);
static void DestroyOwnerData(HWND hWnd);
static void RefreshListView(HWND hWnd);
static LRESULT DoFileTypeChange(HWND hWnd,WORD wmEvent,LPARAM lParam);
static LRESULT SetShowMode(HWND hWnd);
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoToHeigherLevel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void SetCurrentPath(HWND hWnd, LPTSTR lpCurpath);
static void ShowCurrentPath(HWND hWnd);
static void SetCurrentSelFile(HWND hWnd,LPTSTR lpFileName);

#define LOADIMAGE

static HBITMAP LoadOwnerBitmap(HWND hWnd,UINT idBitmap,UINT cx,UINT cy);
static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight);


//static void SetFileOpenCallBack(HWND hWnd,FILEOPENSTATUS lpfnFileOpen);
static BOOL FileOpenCall(HWND hwnd, LPTSTR lpFileName  );
static BOOL FileSaveCall(HWND hwnd, LPTSTR lpFileName  );

#define FILE_ATTR_DIR  0x0001   //判断当前文件所包含的路径是否存在
#define FILE_ATTR_FILE 0x0002   //判断当前文件是否存在
static BOOL IsExistOfTheFile(LPTSTR lpFileName,UINT uFileType);

static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

const static struct dlgOpenTemplate{
    DLG_TEMPLATE_EX dlg;
//    DLG_ITEMTEMPLATE_EX item[1];
}dlgOpenTemplate = {
    { WS_EX_NOMOVE | WS_EX_OKBOX | WS_EX_CLOSEBOX |WS_EX_TITLE, WS_VISIBLE|WS_POPUP,0,30,20,210,280,0,0,"Open" },
};


// ********************************************************************
// 声明：BOOL  WINAPI  CDlg_ GetOpenFileName(LPOPENFILENAME lpofn)
// 参数：
// 	IN lpofn - 指向结构OPENFILENAME的指针
// 返回值：
// 	成功返回TRUE，否则返回FALSE
// 功能描述：运行一个让用户选择一个想打开的文件的对话框
// 引用: 有应用程序需要使用"打开文件对话框"时调用
// ********************************************************************
BOOL  WINAPI  CDlg_GetOpenFileName(LPOPENFILENAME lpofn)
{
	HINSTANCE hInstance;
	LPOPENFILESTRUCT lpOpenFileStruct;
//	BOOL bRet;

//	RETAILMSG(1,("Enter GetOpenFileName\r\n"));
	InitCommonControls();  // 初始化通用控件
	hInstance = (HINSTANCE)GetWindowLong(lpofn->hwndOwner,GWL_HINSTANCE); // 得到呼叫者实例句柄

	lpOpenFileStruct = (LPOPENFILESTRUCT)malloc(sizeof(OPENFILESTRUCT));  // 分配打开文件结构内存

	if (lpOpenFileStruct == NULL)  // 分配内存失败
		return FALSE;

	lpOpenFileStruct->lpofn = lpofn;  //设置结构
	lpOpenFileStruct->uFileOpenFlag = OPENFILEFLAG;  // 设置打开标志

//	RETAILMSG(1,("Create Dialog\r\n"));
	return DialogBoxIndirectParamEx( hInstance, (LPDLG_TEMPLATE_EX)&dlgOpenTemplate, lpofn->hwndOwner, OpenDlgProc, (LPARAM)lpOpenFileStruct);  // 创建对话框

/*
    MyRegisterClass(hInstance);
	if (!(hWnd=InitInstance (hInstance, lpofn)) ) 
	{
		return FALSE;
	}
	SetFileOpenCallBack(hWnd,FileOpenCall);
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
*/

}



// ********************************************************************
// 声明：BOOL  WINAPI  CDlg_GetSaveFileName(LPOPENFILENAME lpofn)
// 参数：
// 	IN lpofn - 指向结构OPENFILENAME的指针
// 返回值：
// 	成功返回TRUE，否则返回FALSE
// 功能描述：运行一个让用户选择一个想保存的文件的对话框
// 引用: 有应用程序需要使用"保存文件对话框"时调用
// ********************************************************************
BOOL  WINAPI  CDlg_GetSaveFileName(LPOPENFILENAME lpofn)
{
	HINSTANCE hInstance;
//	HWND hWnd;
//	MSG msg;
	LPOPENFILESTRUCT lpOpenFileStruct;

	InitCommonControls();  // 初始化通用控件
	hInstance = (HINSTANCE)GetWindowLong(lpofn->hwndOwner,GWL_HINSTANCE);// 得到呼叫者实例句柄

	lpOpenFileStruct = (LPOPENFILESTRUCT)malloc(sizeof(OPENFILESTRUCT));// 分配打开文件结构内存

	if (lpOpenFileStruct == NULL)  // 分配内存失败
		return FALSE;

	lpOpenFileStruct->lpofn = lpofn;//设置结构
	lpOpenFileStruct->uFileOpenFlag = SAVEFILEFLAG; // 设置为保存文件

	return DialogBoxIndirectParamEx( hInstance, (LPDLG_TEMPLATE_EX)&dlgOpenTemplate, lpofn->hwndOwner, OpenDlgProc, (LPARAM)lpOpenFileStruct); // 创建对话框
	/*
    MyRegisterClass(hInstance);
	if (!(hWnd=InitInstance (hInstance, lpofn)) ) 
	{
		return FALSE;
	}

	SetFileOpenCallBack(hWnd,FileSaveCall);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return bReturn;
	*/
}



/*
static LPTSTR szWindowClass = "OpenDlgClass";


static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)OpenDlgProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 4;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}
*/
//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
/*
static HWND InitInstance(HINSTANCE hInstance, LPOPENFILENAME lpofn)
{
   HWND hWnd;

   hWnd = CreateWindow(szWindowClass, lpofn->lpstrTitle, WS_VISIBLE|WS_POPUP ,//|WS_SYSMENU,
      0, 24, 240, 294, lpofn->hwndOwner, NULL, hInstance, lpofn);

   if (!hWnd)
   {
      return NULL;
   }

   ShowWindow(hWnd, TRUE);
   UpdateWindow(hWnd);

   return hWnd;
}
*/

// ********************************************************************
// 声明：BOOL CALLBACK OpenDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//   IN hWnd- 对话框的窗口句柄
//   IN message - 过程消息
//   IN wParam  - 消息参数
//   IN lParam  - 消息参数
// 返回值：
//	无
// 功能描述：打开/保存窗口的过程函数
// 引用: 
// ********************************************************************
static BOOL CALLBACK OpenDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;

	switch (message) 
	{
//		case WM_CREATE:
		case WM_INITDIALOG:
			return DoCreate(hWnd,wParam,lParam);  // 处理初始化对话框消息
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_FILETYPE:
					DoFileTypeChange(hWnd,(WORD)wmEvent,lParam);  // 文件类型改变
					break;
//				case ID_SHOWMODE:
//					DoChangeShowMode(hWnd,wParam,lParam);
//					break;
				case ID_NEWFOLDER:
					DoNewFolder(hWnd,wParam,lParam);  // 创建新的文件夹
					break;
				case ID_TOHIGHERLEVEL:
					DoToHeigherLevel(hWnd,wParam,lParam);  // 到上一级目录
					break;
			}
			break;

	   case WM_NOTIFY:
			return DoNotify(hWnd,wParam,lParam);  // 通知消息
	
	   case WM_DRAWITEM:
			return DoDrawItem(hWnd,wParam,lParam);  // 绘制条目

//		case WM_CLOSE:
//			DestroyWindow(hWnd);
//			break;

//       case WM_DESTROY:
//		    DestroyOwnerData(hWnd);
//			PostQuitMessage(0);
//			break;
		 case WM_OK:
			if (GetSelectFileName(hWnd) == FALSE)  // 点选“OK”键
				break;
			DestroyOwnerData(hWnd);  // 释放数据
			EndDialog(hWnd,TRUE); // 关闭对话框
			break;
		 case WM_CLOSE:
			DestroyOwnerData(hWnd);  // 释放数据
			EndDialog(hWnd,FALSE); // 关闭对话框
			break;

	   default:
			return FALSE;
			//return DefWindowProc(hWnd, message, wParam, lParam);
   }
 	return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 对话框的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
// 	成功返回 0 ，否则返回 -1
// 功能描述：对话框窗口的处理 WM_INITDIALOG 过程
// 引用: 
// ********************************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HINSTANCE hInstance;
	LPOPENFILESTRUCT lpOpenFileStruct;
//	LPOPENFILENAME lpOpenFileName;

//	lpOpenFileName = (LPOPENFILENAME)lParam;

//	lpcs = (LPCREATESTRUCT) lParam;
//	lpOpenFileStruct->lpofn = lpOpenFileName;
//	RETAILMSG(1,("Enter Initial Dialog\r\n"));

	lpOpenFileStruct = (LPOPENFILESTRUCT)lParam;  // 得到结构句柄
	if (lpOpenFileStruct == NULL)
		return -1;

//	RETAILMSG(1,("111\r\n"));
	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // 得到实例句柄

//	RETAILMSG(1,("222 hWnd = %x ,hInstance = %x\r\n",hWnd,hInstance));
//	CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|WS_BORDER,
//	  5 , 3, 156 , 20,hWnd,(HMENU)IDC_CURPATH,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD|WS_VISIBLE, // 创建输入当前目录的编辑框
	  5 , 3, 156 , 20,hWnd,(HMENU)IDC_CURPATH,hInstance,0);
//	RETAILMSG(1,("XX 000\r\n"));
	CreateWindow(classBUTTON,"+",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_OWNERDRAW,  // 创建“到上一级目录”的按钮
	  164,3,20,20,hWnd,(HMENU)ID_TOHIGHERLEVEL,hInstance,NULL);
//	RETAILMSG(1,("XX 111\r\n"));
	CreateWindow(classBUTTON,"+",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_OWNERDRAW,  // 创建“创建新文件夹”的按钮
	  186,3,20,20,hWnd,(HMENU)ID_NEWFOLDER,hInstance,NULL);
//	CreateWindow(classBUTTON,"+",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_OWNERDRAW,
//	  186,3,20,20,hWnd,(HMENU)ID_SHOWMODE,hInstance,NULL);

//	RETAILMSG(1,("XX 222\r\n"));

//	CreateWindow(classLISTCTRL,"+",WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_ICON|WS_VSCROLL|WS_HSCROLL,
//	  10,25,220,200,hWnd,(HMENU)ID_LISTVIEW,hInstance,NULL);
//	CreateWindow("","+",WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_ICON|WS_VSCROLL|WS_HSCROLL,
//	  10,25,200,160,hWnd,(HMENU)ID_LISTVIEW,hInstance,NULL);

//	CreateWindow(classFileBrowser,"",WS_CHILD|WS_VISIBLE|WS_BORDER,5,25,190,160,hWnd,(HMENU)ID_BROWSER,hInstance,NULL);
	CreateWindowEx(WS_EX_CLIENTEDGE,classFileBrowser,"",WS_CHILD|WS_VISIBLE,5,25,200,160,hWnd,(HMENU)ID_BROWSER,hInstance,NULL); // 创建文件浏览控件
//	RETAILMSG(1,("XX 333\r\n"));

	CreateWindow(classSTATIC ,TEXT("文件名:") ,WS_VISIBLE|WS_CHILD , 
	  5 , 195 , 54 , 20 , hWnd , (HMENU)NULL, (HINSTANCE)(GetWindowLong(hWnd , GWL_HINSTANCE)) , NULL); // 创建文本“文件名”
//	RETAILMSG(1,("XX 444\r\n"));
//	CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|WS_BORDER,
//	  60 , 195, 145 , 20,hWnd,(HMENU)IDC_FILENAME,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD|WS_VISIBLE,
	  60 , 195, 145 , 20,hWnd,(HMENU)IDC_FILENAME,hInstance,0);  // 创建输入文件名的编辑框
//	RETAILMSG(1,("XX 555\r\n"));

//	CreateWindow(classBUTTON,"打开",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
//	  162 , 195, 48, 20,hWnd,(HMENU)ID_OK,hInstance,NULL);

	CreateWindow(classSTATIC ,TEXT("类  型:") ,WS_VISIBLE|WS_CHILD ,   // 创建“类型”文本
	  5 , 220 , 54 , 20 , hWnd , (HMENU)NULL, (HINSTANCE)(GetWindowLong(hWnd , GWL_HINSTANCE)) , NULL);
//	RETAILMSG(1,("XX 666\r\n"));
//	CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|WS_BORDER,
//	  60 , 160 , 110 , 20,hWnd,(HMENU)IDC_FILETYPE,hInstance,0);
//	CreateWindow(classCOMBOBOX,"",WS_CHILD|WS_VISIBLE|WS_BORDER|CBS_DROPDOWNLIST,
//	  60 , 220 , 145 , 90,hWnd,(HMENU)IDC_FILETYPE,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,  // 创建选择文件类型的组合框
	  60 , 220 , 145 , 90,hWnd,(HMENU)IDC_FILETYPE,hInstance,0);

//	CreateWindow(classBUTTON,"取消",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
//	  165, 220 , 48 , 20,hWnd,(HMENU)ID_CANCEL,hInstance,NULL);

//	RETAILMSG(1,("333\r\n"));

	SetShowMode(hWnd);  // 设置显示模式

//	lpOpenFileStruct->lpofn = (LPOPENFILENAME)lpcs->lpCreateParams;
/*	lpOpenFileStruct->lpofn = (LPOPENFILENAME)lParam;
	strcpy(lpOpenFileStruct->curPath,"\\");

//	FillFileType(hWnd,(LPOPENFILENAME)lpcs->lpCreateParams);
	FillFileType(hWnd,(LPOPENFILENAME)lParam);

//	FillListCtrl(hWnd,(LPOPENFILENAME)lpcs->lpCreateParams);
	FillListCtrl(hWnd,(LPOPENFILENAME)lParam);
*/
//	RETAILMSG(1,("444\r\n"));
	SetWindowLong(hWnd,GWL_USERDATA,(DWORD)lpOpenFileStruct);  // 设置窗口用户数据
//	RETAILMSG(1,("555\r\n"));
	SetWindowText(hWnd,lpOpenFileStruct->lpofn->lpstrTitle); // 设置窗口标题
//	RETAILMSG(1,("666\r\n"));
	if (lpOpenFileStruct->lpofn->lpstrInitialDir)
	{
		SetCurrentPath(hWnd,(LPTSTR)lpOpenFileStruct->lpofn->lpstrInitialDir);  // 设置当前路径
	}
	if (lpOpenFileStruct->lpofn->lpstrFile)
	{
		SetCurrentSelFile(hWnd,(LPTSTR)lpOpenFileStruct->lpofn->lpstrFile); // 设置当前的选择文件
	}
//	RETAILMSG(1,("777\r\n"));
	FillFileType(hWnd,lpOpenFileStruct->lpofn);  // 过滤文件类型
//	RETAILMSG(1,("888\r\n"));
	ShowCurrentPath(hWnd); // 显示当前目录
//	RETAILMSG(1,("999\r\n"));
	RefreshListView(hWnd); // 刷新文件显示区

//	RETAILMSG(1,("Initial Dialog OK !!!\r\n"));
	return 0;
}


// ********************************************************************
// 声明：static LRESULT DoDrawItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 对话框的窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	返回 0 
// 功能描述：对话框窗口的处理 WM_DRAWITEM 过程
// 引用: 
// ********************************************************************
static LRESULT DoDrawItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT idCtl;
	LPDRAWITEMSTRUCT lpdis;
	HBITMAP hBitmap;
	RECT rect = {0,0,20,20};
	int iStartPos;

		idCtl=(UINT) wParam;  // 得到要绘制的ID
		lpdis = (LPDRAWITEMSTRUCT) lParam; // 绘制条目的结构
		switch(idCtl)
		{
			case ID_TOHIGHERLEVEL:
				hBitmap = LoadOwnerBitmap(hWnd,OBM_TOOL_UPFOLDER,16,16);  // 得到上一级目录的位图
				break;
			case ID_NEWFOLDER:
				hBitmap = LoadOwnerBitmap(hWnd,OBM_TOOL_NEW,16,16);  // 得到创建新目录的位图
				break;
//			case ID_SHOWMODE:
//				hBitmap = LoadOwnerBitmap(hWnd,104,16,16);
//				break;
			default:
				return 0;
				
		}
		if (lpdis->itemState&ODS_SELECTED)
		{ // 按钮已经被按下
			DrawEdge(lpdis->hDC,&rect,BDR_SUNKENOUTER,BF_RECT|BF_MIDDLE);  // 画下凹边框
			iStartPos = 2; // 位图的起始位置
		}
		else
		{ // 按钮没有按下
			DrawEdge(lpdis->hDC,&rect,BDR_RAISEDOUTER,BF_MIDDLE);   // 画上凸边框
			iStartPos = 1; // 位图的起始位置
		}
		if (hBitmap)
		{
			DrawBitmap(lpdis->hDC,hBitmap,iStartPos,iStartPos,16,16); // 画按钮的位图

			DeleteObject(hBitmap);  // 删除位图
		}
		return 0;
}

// ********************************************************************
// 声明：static HBITMAP LoadOwnerBitmap(HWND hWnd,UINT idBitmap,UINT cx,UINT cy)
// 参数：
//	IN hWnd- 对话框的窗口句柄
//    IN idBitmap - 位图的id号
//    IN cx - 位图的宽
//    IN cy - 位图的高
// 返回值：
// 	成功返回位图句柄，否则返回 NULL
// 功能描述：装载指定id号的位图
// 引用: 
// ********************************************************************
static HBITMAP LoadOwnerBitmap(HWND hWnd,UINT idBitmap,UINT cx,UINT cy)
{
//	HINSTANCE hInstance;

//		hInstance = GetWindowLong(hWnd,GWL_HINSTANCE);

		return LoadImage(NULL,MAKEINTRESOURCE(idBitmap),IMAGE_BITMAP ,cx,cy,0);  // 从系统资源文件装载位图

}
// ********************************************************************
// 声明：static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight)
// 参数：
//	IN hdc- 设备句柄
//    IN hBitmap - 位图句柄
//    IN x ，y - 显示位图的位置
//    IN nWidth - 位图的宽
//    IN nHeight - 位图的高
// 返回值：
//	
// 功能描述：在指定位置显示位图
// 引用: 
// ********************************************************************
static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight)
{
	HDC hCompDC=CreateCompatibleDC(hdc );  // 创建一个临时DC
	HBITMAP hOldMap;

	hOldMap=(HBITMAP)SelectObject(hCompDC,(HGDIOBJ)hBitmap);  // 将位图装载到临时DC
	BitBlt(hdc, x, y, nWidth, nHeight, hCompDC,0, 0, SRCCOPY); // 画位图
	SelectObject(hCompDC,hOldMap); // 恢复DC
	DeleteDC(hCompDC); // 删除DC
}
/*
static BOOL FillListCtrl(HWND hWnd,LPOPENFILENAME lpofn)
{
	LV_COLUMN		lvcolumn;
	TCHAR			rgtsz[3][10] = {"名称","大小","时间"};
	HWND hListWnd;
//	HIMAGELIST hImageList;
	HICON hIcon;
	LPOPENFILESTRUCT lpOpenFileStruct;
	HINSTANCE hInstance;

	hInstance = GetWindowLong(hWnd,GWL_HINSTANCE);

	lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,0);
	if (lpOpenFileStruct == NULL)
		return FALSE;

	hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);

	lpOpenFileStruct->hImageList =ImageList_Create(32 ,32 ,ICL_ICON ,2 ,1);
	hIcon=(HICON)LoadImage(NULL, MAKEINTRESOURCE(104) ,IMAGE_ICON , 32 ,32 ,0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hImageList, hIcon);

	hIcon = (HICON)LoadImage(NULL, MAKEINTRESOURCE(105), IMAGE_ICON , 32, 32, 0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hImageList ,hIcon);
	
	lpOpenFileStruct->hImageList = (HIMAGELIST)SendMessage(hListWnd, LVM_SETIMAGELIST,LVSIL_NORMAL ,(LPARAM)lpOpenFileStruct->hImageList);


	lpOpenFileStruct->hSmallImageList =ImageList_Create(16 ,16 ,ICL_ICON ,2 ,1);
	hIcon=(HICON)LoadImage(hInstance, MAKEINTRESOURCE(108) ,IMAGE_ICON , 16 ,16 ,0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hSmallImageList, hIcon);

	hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(109), IMAGE_ICON , 16, 16, 0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hSmallImageList ,hIcon);
	
	lpOpenFileStruct->hSmallImageList = (HIMAGELIST)SendMessage(hListWnd, LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)lpOpenFileStruct->hSmallImageList);


	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE ;
	lvcolumn.pszText = rgtsz[0];
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 60;  
	lvcolumn.iImage= 0;
	SendMessage(hListWnd,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn);


	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.pszText = rgtsz[1];
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = 60;  // The Width of the column
	lvcolumn.iImage= 0;
	SendMessage(hListWnd,LVM_INSERTCOLUMN,1,(LPARAM)&lvcolumn);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_RIGHT;
	lvcolumn.pszText = rgtsz[2];
	lvcolumn.iSubItem = 2;
	lvcolumn.cx = 100;  // The Width of the column
	lvcolumn.iImage= 0;
	SendMessage(hListWnd,LVM_INSERTCOLUMN,2,(LPARAM)&lvcolumn);

	{

		if (lpofn->lpstrInitialDir == NULL || strlen(lpofn->lpstrInitialDir) == 0)
			strcpy(lpOpenFileStruct->curPath,"\\");
		else
		{
			if (lpofn->lpstrInitialDir[strlen(lpofn->lpstrInitialDir)-1] == '\\')
			{
				strcpy(lpOpenFileStruct->curPath,lpofn->lpstrInitialDir);
			}
			else
			{
				strcpy(lpOpenFileStruct->curPath,lpofn->lpstrInitialDir);
				strcat(lpOpenFileStruct->curPath,"\\");
			}
		}
		RefreshListView(hWnd);
	}
	return TRUE;
}
*/

// ********************************************************************
// 声明：static void FillFileType(HWND hWnd,LPOPENFILENAME lpofn)
// 参数：
// 	IN hWnd- 窗口句柄
// 	IN lpofn - 指向结构OPENFILENAME的指针
// 返回值：
// 	
// 功能描述：填充要打开的文件类型到选择框
// 引用: 
// ********************************************************************
static void FillFileType(HWND hWnd,LPOPENFILENAME lpofn)
{
	LPTSTR lpStrFilter;
	HWND hComboBox;
	UINT iTypeNum =0;

		lpStrFilter = (LPTSTR)lpofn->lpstrFilter; // 得到文件过滤的指针
		if (lpStrFilter == NULL)
			return;  // 没有文件过滤

		hComboBox = GetDlgItem(hWnd,IDC_FILETYPE);  // 得到组合框的句柄
		
		while (strlen(lpStrFilter))
		{
			SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)lpStrFilter);  // 添加文件过滤串到组合框
			lpStrFilter +=strlen(lpStrFilter) +1;  // 得到下一条过滤串
			iTypeNum ++;
		}

		if (iTypeNum < 4)
		{ // 如果文件过滤串小于4，则要重新设置组合框的大小
			RECT rect; 
			int iTitleHeight = GetSystemMetrics(SM_CYCAPTION);

			GetWindowRect(hComboBox,&rect); // 得到原来组合框的大小
			SetWindowPos(hComboBox,NULL,0,0,rect.right-rect.left,(iTypeNum + 1)* iTitleHeight + 2,SWP_NOMOVE|SWP_NOZORDER); // 重设组合框的大小
		}
		SendMessage(hComboBox,CB_SETCURSEL,0,0); // Set index = 0 to current sel
}
/*
static void RemoveAllItem(HWND hWnd)
{
	HWND hListWnd;
//	SYSTEMTIME SystemTime;

		hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);
		SendMessage(hListWnd,LVM_DELETEALLITEMS,0,0);
		return;
}
*/
/*
static void InsertListItem(HWND hWnd,FILE_FIND_DATA   find,BOOL bDir)
{
	LV_ITEM			lvitem;
	HWND hListWnd;
	UINT iItem;
	TCHAR string[64];
	SYSTEMTIME SystemTime;

		hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = 0x7ffff ;
		lvitem.iSubItem = 0;
		lvitem.pszText = find.cFileName;
		if (bDir)
			lvitem.iImage = 0;
		else
			lvitem.iImage = 1;

		iItem = SendMessage(hListWnd,LVM_INSERTITEM,0,(LPARAM)&lvitem);

		if (bDir == 0) // if is directory ,then is not file size
		{
			sprintf(string,"%d",find.nFileSizeLow);
			lvitem.mask = LVIF_TEXT ;
			lvitem.iItem = iItem ;
			lvitem.iSubItem = 1;
			lvitem.pszText = string;

			SendMessage(hListWnd,LVM_SETITEM,0,(LPARAM)&lvitem);
		}

		FileTimeToSystemTime( &find.ftLastWriteTime,&SystemTime ); 
		sprintf(string,"%d-%02d-%02d %02d:%02d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute);
		lvitem.mask = LVIF_TEXT ;
		lvitem.iItem = iItem ;
		lvitem.iSubItem = 2;
		lvitem.pszText = string;

		SendMessage(hListWnd,LVM_SETITEM,0,(LPARAM)&lvitem);
}
*/

//static Distance=TRUE;

// ********************************************************************
// 声明：static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
// 	返回 0 
// 功能描述：对话框窗口的处理 WM_NOTIFY 过程
// 引用: 
// ********************************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	NMLISTVIEW *pnmvl;
	NMHDR   *hdr=(NMHDR   *)lParam;    

		switch(hdr->code)
		{
//			 case LVN_COLUMNCLICK:
//				 pnmvl=(NMLISTVIEW *)lParam;
//				 SendMessage(pnmvl->hdr.hwndFrom,LVM_SORTITEMS,(WPARAM)pnmvl->iSubItem,(LPARAM)CompareFunc);
//				 Distance=1-Distance;
//				 break;
			 case EXN_ACTIVEITEM:
				 DoActiveItem(hWnd,wParam,lParam);  // 激活条目的通知消息
				 break;
			 case EXN_SELCHANGED:
				 DoItemChanged(hWnd,wParam,lParam); // 选择改变
				 break;
			 case EXN_DATACHANGED:
				 DoDataChanged(hWnd,wParam,lParam); // 数据改变
				 break;
		}
		return 0;
}

// ********************************************************************
// 声明：static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数，保留
//    IN lParam  - 消息参数，保留
// 返回值：
//	返回 0 
// 功能描述：对话框窗口的处理 EXN_ACTIVEITEM 过程
// 引用: 
// ********************************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	char lpFileName[MAX_PATH];//,lpFullName[MAX_PATH];
	HWND  hBrowser;
		hBrowser = GetDlgItem(hWnd,ID_BROWSER);  // 得到文件浏览控件的句柄
//		if (SendMessage(hBrowser,EXM_GETSELFILENAME,MAX_PATH,lpFileName) == TRUE)
//		{
			if (GetSelectFileName(hWnd) == FALSE) // 得到当前的选择文件
				return 0;
//			if (StrAsteriskCmp("*.htm",lpFileName) == 0)
//			{
//				MessageBox(hWnd,"Will Open File",lpFileName,MB_OK);
//			}
//		}
		EndDialog(hWnd,TRUE);
        return 0;
}
// ********************************************************************
// 声明：static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数，保留
//    IN lParam  - 消息参数，保留
// 返回值：
//	返回 0 
// 功能描述：对话框窗口的处理 EXN_SELCHANGED 过程
// 引用: 
// ********************************************************************
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	/*
	LPNMLISTVIEW pnmv;
	LVITEM	lvItem;
	TCHAR lpFileName[MAX_PATH];
	HWND hListWnd;
	HWND hEdit;

		pnmv = (LPNMLISTVIEW)lParam;

		if((pnmv->uOldState&LVIS_SELECTED) != (pnmv->uNewState&LVIS_SELECTED))
		{
			if (pnmv->uNewState&LVIS_SELECTED)
			{
				// The Selected to be set
				hListWnd = GetDlgItem(hWnd,ID_LISTVIEW);
				lvItem.pszText = lpFileName;
				lvItem.cchTextMax = MAX_PATH;
				lvItem.iSubItem=0;
				lvItem.mask = LVIF_TEXT | LVIF_IMAGE ;
				lvItem.iItem = pnmv->iItem;
//				SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)pnmv->iItem,(LPARAM)&lvItem);
				SendMessage(hListWnd,LVM_GETITEM,(WPARAM)pnmv->iItem,(LPARAM)&lvItem);
				if (lvItem.iImage == 0)
				{
					return FALSE;
				}
				
				hEdit = GetDlgItem(hWnd,IDC_FILENAME);
				SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpFileName);
			}
		}
*/
	HWND  hBrowser;
	TCHAR lpFileName[MAX_PATH];
	HWND hEdit;
	LPTSTR lpNewPos;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览控件的句柄
		if (SendMessage(hBrowser,EXM_GETSELFILENAME,MAX_PATH,(LPARAM)lpFileName) == TRUE)  // 得到当前的选择文件
		{  
			// 得到当前的文件名
			lpNewPos = lpFileName;
			while(*lpNewPos) lpNewPos++;
			while(lpNewPos>lpFileName)
			{
				if (*lpNewPos == '\\')  // 找到最后一个‘\\’，后面的内容就是文件名
				{
					lpNewPos++;
					break;
				}
				lpNewPos--;
			}
//			_splitpath(lpFileName,NULL,NULL,lpFileName,NULL);
			hEdit = GetDlgItem(hWnd,IDC_FILENAME);  // 得到文件名的编辑框句柄
			SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpNewPos); // 设置新的文件名
		}
		return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数，保留
//    IN lParam  - 消息参数，保留
// 返回值：
//	返回 0 
// 功能描述：对话框窗口的处理 EXN_DATACHANGED 过程
// 引用: 
// ********************************************************************
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		ShowCurrentPath(hWnd);  // 设置当前路径
		return 0;
}
/*
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{
	     return 0;
}
*/

// ********************************************************************
// 声明：static LRESULT DoFileTypeChange(HWND hWnd,WORD wmEvent,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数，保留
//    IN lParam  - 消息参数，保留
// 返回值：
//	返回 0 
// 功能描述：对话框窗口的处理 IDC_FILETYPE 过程
// 引用: 
// ********************************************************************
static LRESULT DoFileTypeChange(HWND hWnd,WORD wmEvent,LPARAM lParam)
{
	switch(wmEvent)
	{
		case CBN_SELCHANGE:
			RefreshListView(hWnd);  // 重新刷新文件浏览控件的内容
			break;
		default :
			break;
	}
	return 0;
}


// **************************************************
// 声明：static LRESULT SetShowMode(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：设置显示模式。
// 引用: 
// **************************************************
static LRESULT SetShowMode(HWND hWnd)
{
	HWND hListWnd;
	HWND hBrowser;
	DWORD dwStyle;

//	DWORD newListViewType;
//	DWORD tbListViewType[]={LVS_ICON,LVS_SMALLICON,LVS_LIST,LVS_REPORT};

/*
		hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);

		dwStyle &= ~LVS_TYPEMASK; // clear ListView Type
		dwStyle |= newListViewType;

		SetWindowLong(hListWnd,GWL_STYLE,dwStyle );
*/		
		hBrowser = (HWND)GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览控件的句柄
		hListWnd = (HWND)SendMessage(hBrowser,EXM_GETLISTVIEW,0,0); // 得到列表框的句柄

		dwStyle = GetWindowLong(hListWnd,GWL_STYLE);  // 得到列表框原来的窗口风格
		dwStyle &= ~LVS_TYPEMASK; // clear ListView Type 
		dwStyle |= LVS_LIST; // 设置当前风格为LIST
		
		SetWindowLong(hListWnd,GWL_STYLE,dwStyle ); // 重新设置窗口风格
		return TRUE;

}

// ********************************************************************
// 声明：static LRESULT DoToHeigherLevel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数，保留
//    IN lParam  - 消息参数，保留
// 返回值：
//	返回 0 
// 功能描述：到上一级目录，对话框窗口的处理 ID_TOHIGHERLEVEL 过程
// 引用: 
// ********************************************************************
static LRESULT DoToHeigherLevel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hBrowser;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER);  // 得到文件浏览控件的窗口句柄
		SendMessage(hBrowser,EXM_UPPATH,0,0);	 // 发送消息通知控件到上一级目录

		ShowCurrentPath(hWnd); // 显示当前路径
		return TRUE;
}

// ********************************************************************
// 声明：static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
//	IN hWnd- 窗口句柄
//    IN wParam  - 消息参数，保留
//    IN lParam  - 消息参数，保留
// 返回值：
//	返回 0 
// 功能描述：创建新文件夹，对话框窗口的处理 ID_NEWFOLDER 过程
// 引用: 
// ********************************************************************
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hBrowser;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览控件的窗口句柄
		SendMessage(hBrowser,EXM_NEWFOLDER,0,0); // 通知文件浏览控件创建新的文件夹
		return TRUE;
}

// **************************************************
// 声明：static BOOL GetSelectFileName(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前选择文件的文件名。
// 引用: 
// **************************************************
static BOOL GetSelectFileName(HWND hWnd)
{
	LPOPENFILENAME lpofn;
	LPOPENFILESTRUCT lpOpenFileStruct;
	HWND hBrowser;
	HWND hEdit;
	TCHAR lpFileName[MAX_PATH],lpCurPath[MAX_PATH];


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA); // 得到打开文件结构指针
		if (lpOpenFileStruct == NULL)  // 指针无效
			return FALSE;
		lpofn = lpOpenFileStruct->lpofn;  // 得到打开文件名结构
		if (lpofn == NULL)  // 结构无效
			return FALSE;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览控件的句柄
//		return SendMessage(hBrowser,EXM_GETSELFILENAME,lpofn->nMaxFile,lpofn->lpstrFile);

		hEdit = GetDlgItem(hWnd,IDC_FILENAME);  // 得到文件名编辑区的句柄

		if (GetWindowTextLength(hEdit) ==0) // 得到文件名长度
			return FALSE;
		GetWindowText(hEdit,lpFileName,MAX_PATH); // 得到文件名

		if (lpFileName[0] == '\\')
		{ // 文件名是全路径
			if (strlen(lpFileName) +1 >= lpofn->nMaxFile)
				return FALSE; // 缓存空间不够
			strcpy(lpofn->lpstrFile,lpFileName); // 复制文件名
		}
		else
		{ 
			// if ("..\\")

			while(1)
			{  // 是否有要求到上一级目录的要求
				if (strncmp(lpFileName,"..\\",3) == 0)
				{  // 到上一级目录
					DoToHeigherLevel(hWnd,0,0);
					strcpy(lpFileName,lpFileName+3); // 去除到上一级目录的标识
					continue;
				}
				break;
			}

			SendMessage(hBrowser,EXM_GETCURPATH,0,(LPARAM)lpCurPath);  // 得到当前目录

//			strcpy(lpofn->lpstrFile,lpCurPath);
//			strcat(lpofn->lpstrFile,lpFileName);
			if (strlen(lpCurPath) + strlen(lpFileName) +1 >= lpofn->nMaxFile)
				return FALSE;  // 指定的缓存空间不够

			_makepath(lpofn->lpstrFile,NULL,lpCurPath,lpFileName,NULL);  // 组合文件名

		}
		if (lpOpenFileStruct->uFileOpenFlag == OPENFILEFLAG)
			return FileOpenCall(hWnd,lpofn->lpstrFile);  // 呼叫打开文件
		else
			return FileSaveCall(hWnd,lpofn->lpstrFile);  // 呼叫保存文件
//		_makepath(lpofn->lpstrFile,NULL,lpCurPath,lpFileName,NULL);
		
//		return TRUE;
/*
	short iItem;//,iFileNo;
	HWND hListWnd, hEdit;
	LVITEM	lvItem;
//	LPNMITEMACTIVATE lpnmia;
	LPOPENFILENAME lpofn;
	LPOPENFILESTRUCT lpOpenFileStruct;
	TCHAR lpFileName[MAX_PATH],OldPath[MAX_PATH];


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,0);
		if (lpOpenFileStruct == NULL)
			return FALSE;
//		lpofn= (LPOPENFILENAME)GetWindowLong(hWnd,0);
		lpofn = lpOpenFileStruct->lpofn;

		hEdit = GetDlgItem(hWnd,IDC_FILENAME);
//		hListWnd = GetDlgItem(hWnd,ID_LISTVIEW);

		iItem=(short)SendMessage(hListWnd,LVM_GETNEXTITEM ,(WPARAM)-1,MAKELPARAM(LVNI_SELECTED,0));
		if (iItem==-1)
		{
			//return FALSE;
		}
		else
		{
			lvItem.mask= LVIF_IMAGE | LVIF_TEXT;

			lvItem.iSubItem =0;
			lvItem.iItem =iItem;
			lvItem.pszText = lpFileName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)iItem,(LPARAM)&lvItem);
			if (lvItem.iImage == 0)
			{
//				strcat(lpOpenFileStruct->curPath,lpFileName);
//				strcat(lpOpenFileStruct->curPath,"\\");
				// Will Refresh ListView
//				RefreshListView(hWnd);
			
				ShowCurrentPath(hWnd);

				return FALSE;
			}
		}
		if (GetWindowTextLength(hEdit) ==0)
			return FALSE;
		GetWindowText(hEdit,lpFileName,MAX_PATH);

//		strcpy(OldPath,lpOpenFileStruct->curPath);
//		if (lpFileName[0] == '\\')
//		{
//			strcpy(lpofn->lpstrFile,lpFileName);
//		}
//		else
//		{ 
			// if ("..\\")
//			while(1)
//			{
//				if (strncmp(lpFileName,"..\\",3) == 0)
//				{
//					DoToHeigherLevel(hWnd,0,0);
//					strcpy(lpFileName,lpFileName+3);
//					continue;
//				}
//				break;
//			}

//			strcpy(lpofn->lpstrFile,lpOpenFileStruct->curPath);
//			strcat(lpofn->lpstrFile,lpFileName);
//		}

//		_splitpath(lpofn->lpstrFile,NULL,lpOpenFileStruct->curPath,NULL,NULL);  // get the file directory
//		strcpy(lpFileName,lpofn->lpstrFile+strlen(lpOpenFileStruct->curPath)); // get the file name  include ext
		
//		if (IsExistOfTheFile(lpOpenFileStruct->curPath,FILE_ATTR_DIR) == FALSE)
//		{
//			strcpy(lpOpenFileStruct->curPath,OldPath);
//
//			MessageBox(hWnd,"路径不存在。\r\n请检查所给的路径是否正确。","打开",MB_OK);
//			return FALSE;
//		}

//		if (lpOpenFileStruct ->lpfnFileOpen(hWnd,lpofn->lpstrFile) == FALSE)
//		{
//			SetWindowText(hEdit,lpFileName);
//			RefreshListView(hWnd);
//			ShowCurrentPath(hWnd);
//			return FALSE;
//		}
*/
		return TRUE;
}

// **************************************************
// 声明：static void DestroyOwnerData(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：释放数据。
// 引用: 
// **************************************************
static void DestroyOwnerData(HWND hWnd)
{
		LPOPENFILESTRUCT lpOpenFileStruct;

		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA);
		if (lpOpenFileStruct != NULL)
		{
//			ImageList_Destroy(lpOpenFileStruct->hImageList);
//			ImageList_Destroy(lpOpenFileStruct->hSmallImageList);
			free(lpOpenFileStruct); // 释放结构空间
		}
}

// **************************************************
// 声明：static void RefreshListView(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：刷新文件浏览控件。
// 引用: 
// **************************************************
static void RefreshListView(HWND hWnd)
{
	TCHAR lpFilter[MAX_PATH];
	HWND  hBrowser,hFileType;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER);  // 得到文件浏览控件句柄

		// Search Match File
		hFileType = GetDlgItem(hWnd,IDC_FILETYPE); // 得到文件过滤串的句柄
		GetWindowText(hFileType,lpFilter,MAX_PATH); // 得到文件过滤串
		SendMessage(hBrowser,EXM_SETFILTER,0,(LPARAM)lpFilter); // 设置文件过滤标志
		return;
}
// **************************************************
// 声明：static void SetCurrentPath(HWND hWnd, LPTSTR lpCurPath)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpCurPath -- 要设置的当前路径
// 
// 返回值： 无
// 功能描述：设置当前路径。
// 引用: 
// **************************************************
static void SetCurrentPath(HWND hWnd, LPTSTR lpCurPath)
{
	HWND hBrowser;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览控件的句柄
		SendMessage(hBrowser,EXM_SETCURPATH,0,(LPARAM)lpCurPath); // 设置当前目录到文件浏览控件
}

// **************************************************
// 声明：static void ShowCurrentPath(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄 
// 
// 返回值：无
// 功能描述：显示当前路径
// 引用: 
// **************************************************
static void ShowCurrentPath(HWND hWnd)
{
//	LPOPENFILESTRUCT lpOpenFileStruct;
	HWND hCurPathWnd;
	HWND hBrowser;
	TCHAR lpCurPath[MAX_PATH];


//		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA);
//		if (lpOpenFileStruct == NULL)
//			return ;
//		lpOpenFileStruct->curPath;

		hCurPathWnd=GetDlgItem(hWnd,IDC_CURPATH);  // 得到当前路径的窗口句柄
		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // 得到文件浏览控件的句柄
		SendMessage(hBrowser,EXM_GETCURPATH,0,(LPARAM)lpCurPath); // 得到当前路径
		SetWindowText(hCurPathWnd,lpCurPath); // 设置当前路径的当前路径窗口
}
/*
static void SetFileOpenCallBack(HWND hWnd,FILEOPENSTATUS lpfnFileOpen)
{
	LPOPENFILESTRUCT lpOpenFileStruct;
//	HWND hCurPathWnd;


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,0);
		if (lpOpenFileStruct == NULL)
			return ;

		lpOpenFileStruct ->lpfnFileOpen = lpfnFileOpen;
}
*/

// **************************************************
// 声明：static BOOL FileOpenCall(HWND hWnd, LPTSTR lpFileName  )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpFileName -- 要打开的文件名
// 
// 返回值：可以打开返回TRUE，否则返回FALSE
// 功能描述：打开文件回调。
// 引用: 
// **************************************************
static BOOL FileOpenCall(HWND hWnd, LPTSTR lpFileName  )
{
	LPOPENFILESTRUCT lpOpenFileStruct;
	LPOPENFILENAME lpofn;
	DWORD flags;
	BOOL bExist = FALSE;


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA);
		if (lpOpenFileStruct == NULL)
			return FALSE;

		lpofn = lpOpenFileStruct->lpofn;
		flags = lpofn ->Flags;

//		if (IsExistOfTheFile(lpOpenFileStruct->curPath,FILE_ATTR_DIR) == FALSE)
//		{
//			strcpy(lpOpenFileStruct->curPath,OldPath);
//
//			MessageBox(hWnd,"路径不存在。\r\n请检查所给的路径是否正确。","打开",MB_OK);
//			return FALSE;
//		}
		if (flags & OFN_PATHMUSTEXIST)
		{  // 要求路径必须存在
			bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_DIR);  // 判断路径是否存在
			if (bExist == FALSE)
			{ // 路径不存在，不能打开
			    MessageBox(hWnd,"路径不存在。\r\n请检查所给的路径是否正确。","打开",MB_OK);
				return FALSE;
			}
		}
		if (flags & OFN_FILEMUSTEXIST)
		{  // 要求文件必须存在
			bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_FILE);  // 判断文件是否存在
			if (bExist == FALSE)
			{ // 文件不存在，不能打开
				MessageBox(hWnd,"找不到文件。\r\n请检查所给的文件名是否正确。","打开",MB_OK);
				return FALSE;
			}
		}
		if (flags & OFN_CREATEPROMPT)
		{ // 如果文件不存在，则提示是否需要创建
			if (bExist == FALSE)
			{ // 文件不存在
				if (MessageBox(hWnd,"找不到文件。\r\n是否创建该文件？","打开",MB_YESNO)==IDYES) // 提示是否需要创建
				{    // 需要创建
					HANDLE hFile;
						
						hFile=CreateFile(lpFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_ARCHIVE,NULL); // 创建新文件
						if (hFile==INVALID_HANDLE_VALUE)
						{ // 创建失败
							MessageBox(hWnd,"不能创建文件。","打开",MB_OK);
							return FALSE;
						}
						CloseHandle(hFile); // 关闭文件
						return TRUE;
				}
				return FALSE;
			}
		}
		return TRUE;
}

// **************************************************
// 声明：static BOOL FileSaveCall(HWND hWnd, LPTSTR lpFileName  )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpFileName -- 要保存的文件名
// 
// 返回值：可以保存返回TRUE，否则返回FALSE
// 功能描述：保存文件回调。
// 引用: 
// **************************************************
static BOOL FileSaveCall(HWND hWnd, LPTSTR lpFileName  )
{
	LPOPENFILESTRUCT lpOpenFileStruct;
	LPOPENFILENAME lpofn;
	DWORD flags;
	BOOL bExist;


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA); // 得到打开文件结构的指针
		if (lpOpenFileStruct == NULL)
			return FALSE;  // 结构无效

		lpofn = lpOpenFileStruct->lpofn;  // 得到打开文件名的结构
		flags = lpofn ->Flags;

		if (flags & OFN_PATHMUSTEXIST)
		{ // 路径必须存在
			bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_DIR);  // 路径是否存在
			if (bExist == FALSE)
			{ // 文件不存在
			    MessageBox(hWnd,"路径不存在。\r\n请检查所给的路径是否正确。","保存",MB_OK);
				return FALSE;
			}
		}

		bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_FILE);  // 文件是否存在
		if (flags & OFN_OVERWRITEPROMPT)
		{ //文件存在，提示是否覆盖
			if (bExist == TRUE)
			{ // 文件存在
				if (MessageBox(hWnd,"文件已经存在。是否要覆盖该文件？","保存",MB_YESNO)==IDYES) // 提示是否覆盖
				{  // 可以覆盖
						return TRUE;
				}
				return FALSE; // 不要覆盖，返回失败
			}
		}
		return TRUE;
}


// **************************************************
// 声明：static BOOL IsExistOfTheFile(LPTSTR lpFileName,UINT uFileType)
// 参数：
// 	IN lpFileName -- 文件名
// 	IN uFileType -- 文件类型
// 
// 返回值：
// 功能描述：
// 引用: 
// **************************************************
static BOOL IsExistOfTheFile(LPTSTR lpFileName,UINT uFileType)
{
	FILE_FIND_DATA   find;
	HANDLE hfind;


/*		if (uFileType == FILE_ATTR_DIR)
		{
			if (strlen(lpFileName) == 1 && lpFileName[0] == '\\')
				return TRUE;
			*(lpFileName + strlen(lpFileName)-1 ) = 0;
		}
		*/
		hfind = FindFirstFile( lpFileName, &find ); // 查找文件
		if( hfind != INVALID_HANDLE_VALUE )
		{  // The File is Exist
			FindClose( hfind );

			if (uFileType == FILE_ATTR_DIR)
			{
				//如果文件存在，则路径一定存在。
/*				strcat(lpFileName , "\\");
				if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					return TRUE;
				else
					return FALSE;
*/
				return TRUE;
			}
			else if (uFileType == FILE_ATTR_FILE)
			{ // 如果要求文件是否存在
				if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  // 查找到的是路径
					return FALSE; // 文件不存在
				else
					return TRUE;  // 文件存在
			}
			else
				return FALSE;
		}
		else
		{
			// The File is Not Exist
			if (uFileType == FILE_ATTR_DIR)
			{  // 要求得到目录是否存在
				DWORD err = GetLastError();  // 得到错误信息
				if (err == 0x0000003)  
//				if (err == ERROR_PATH_NOT_FOUND)
					return FALSE; // 目录不存在
				else
					return TRUE; // 目录存在
				}
			else if (uFileType == FILE_ATTR_FILE)
			{ // 没有查到，文件肯定不存在
				return FALSE;
			}
			else
				return FALSE;
		}
}


// **************************************************
// 声明：static void SetCurrentSelFile(HWND hWnd,LPTSTR lpFileName)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpFileName -- 要设置为当前选择文件的文件名
// 
// 返回值：无
// 功能描述：设置当前的选择文件。
// 引用: 
// **************************************************
static void SetCurrentSelFile(HWND hWnd,LPTSTR lpFileName)
{
	HWND hEdit;

		hEdit = GetDlgItem(hWnd,IDC_FILENAME); // 得到编辑区的句柄
		SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpFileName);// 设置当前文件到编辑区

		return;
}
