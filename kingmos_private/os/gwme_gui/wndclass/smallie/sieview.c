/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类
版本号：1.0.0.456
开发时期：2001-02-11
作者：陈建明 JAMI
修改记录：
	2004.05.26 添加显示GIF文件动画的部分,将所有的图象有专门的线程来显示
	2004.08.19 显示是先写到内存DC，然后复制到设备DC
	2004.09.09 修改合成一个INTERNET的相对路经的地址
**************************************************/
//#include <myalloc.h>
//#include <math.h>

#ifdef EML_DOS
#include <dos.h>
#include <CONIO.H>
#include <STDIO.H>
#endif

//#include "eframe.h"
//#include "ewindows.h"
#include "eHtmView.h"
#include "Control.h"
//#include "HtmlLocate.h"
#include "SIETable.h"
#include "efilepth.h"
#include "ViewDef.h"
#include "jpegapi.h"
#include <dialdlg.h>


// **************************************************
// 定义区域
// **************************************************

/*

//#define _MAPPOINTER
#define INTERNET_HEAD  "http://"


#define MAX_FILENAME MAX_PATH
#define INTERNET_SUPPORT
//#undef INTERNET_SUPPORT

#define LOADANIMATION
//#undef LOADANIMATION

#ifdef INTERNET_SUPPORT

#include "internet.h"

#define TEMPFILE  "\\Internet.tmp"
#define INTERNET_EVENT  3

#endif


#define WM_MOUSEWHEEL                   0x020A
#define THREADNUM		5


#ifdef INTERNET_SUPPORT
typedef struct DownLoadItem{
	HINTERNET   hUrl;
	LPTSTR		lpUrl;
	HHTMCTL hControl;
	struct DownLoadItem *next;
}DOWNLOADITEM,*LPDOWNLOADITEM;

typedef struct RunningItem{
	HINTERNET   hUrl;
//	LPTSTR		lpUrl;
	struct RunningItem *next;
}RUNNINGITEM,*LPRUNNINGITEM;
#endif

// !!! Add By Jami chen in 2004.05.26 for GIFAnimation

typedef struct ShowImageItem{
	LPHTMLCONTROL lpControl; // 要显示的图象的控件指针
	HGIF hGif ; // Gif 图象的句柄
	HBITMAP hBmp ; // bitmap , jpeg 图象的句柄
	UINT iIndex; // gif图象使用，当前正在播放的图象索引
	UINT iDelayTime; // gif 图象使用，播放下一个索引图象还需要等待的时间
	UINT iImageNum ; //gif 图象使用，当前一共有多少幅图象
	struct ShowImageItem *next;  // 指向下一幅GIF动画
}SHOWIMAGEITEM,*LPSHOWIMAGEITEM;

// !!! Add End

typedef struct structUrlList{
	struct structUrlList *next;
	struct structUrlList *prev;
	LPTSTR lpUrl;
	LPTSTR lpLocateFile;
}URLLIST,*LPURLLIST;

typedef struct structHtmlView{
	LONG  x_Org;
	LONG  y_Org;
	DWORD width;
	DWORD height;
	HHTML hHtml;
	LPURLLIST  lpUrlHead;
	LPURLLIST  lpCurUrl;

	LPTSTR lpCurFile;
	LPTSTR lpUrl;
	LPTSTR lpMark;

	LPTSTR lpCurPath;

#ifdef INTERNET_SUPPORT
	HINTERNET  hInternetSiteInit;
	LPDOWNLOADITEM lpDownLoadList;
	LPRUNNINGITEM  lpRunningList;
#endif

#ifdef LOADANIMATION
	HANDLE hAnimation;
	BOOL bExitAnimation;
#endif

	int iRunThreadCount;
	BOOL bExit;
	BOOL bStop;
	CRITICAL_SECTION DownLoadCriticalSection;
	CRITICAL_SECTION RunningCriticalSection;

	LPSHOWIMAGEITEM lpShowImageList; // 用来显示的图象列表, add by jami chen in 2004.05.26

}HTMLVIEW,*LPHTMLVIEW;

typedef struct structDownLoadParam{
	HWND hWnd;
//	HINTERNET hUrl;
//	HHTMCTL hControl;
//	LPTSTR lpUrl;
//	BOOL bRefresh;
	LPDOWNLOADITEM lpDownloadItem;
}DOWNLOADPARAM,*LPDOWNLOADPARAM;



#define INITLOCK(CriticalSection)		InitializeCriticalSection(&CriticalSection)
#define LOCK(CriticalSection)			EnterCriticalSection(&CriticalSection)
#define UNLOCK(CriticalSection)		LeaveCriticalSection(&CriticalSection) 
#define DEINITLOCK(CriticalSection)	DeleteCriticalSection(&CriticalSection)

*/

static const char classSMIE_BROWERS[] = TEXT("SMIE_BROWERS");

#define STEPWIDTH  32
#define STEPHEIGHT 16


static int iStartTick = 0;
//extern TCHAR* SMIE_BufferAssignTChar(TCHAR *pSrc);

// **************************************************
// 函数声明区
// **************************************************

//#ifdef INTERNET_SUPPORT
//static  void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl);
//static BOOL InsertDownList(	HWND hWnd,HINTERNET hUrl ,LPTSTR lpUrl,HHTMCTL hControl);
//static BOOL InsertDownList(	HWND hWnd,LPTSTR lpUrl,HHTMCTL hControl);
//static BOOL DeleteDownList(	HWND hWnd,HINTERNET  hUrl);
//static BOOL DeleteDownList(	HWND hWnd);
//static LPTSTR GetDownLoadUrl(HWND hWnd,HINTERNET  hUrl);
//static LPDOWNLOADITEM GetDownLoadUrl(HWND hWnd);
//static HHTMCTL GetControl(HWND hWnd,HINTERNET  hUrl);
//#endif

LRESULT CALLBACK SMIE_BrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);


static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint);
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoLoadFile(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGo(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoBackWards(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGoMark(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoReadData(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoStop(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static BOOL SMIE_ResetScrollBar(HWND hWnd,HHTML hHtml);
static void SMIE_HScrollWindow(HWND hWnd,short nScroll);
static void SMIE_VScrollWindow(HWND hWnd,short nScroll);
static void SetCurrentScrollPos(HWND hWnd,int fnBar);

//#ifdef INTERNET_SUPPORT
static void SMIE_SendInternetNotifyMessage(HWND hWnd,UINT code,LPTSTR lpUrl);
//#endif

static LRESULT SMIE_GoUrl(HWND hWnd,LPTSTR lpUrl);
//static void SMIE_InsertUrlList(LPHTMLVIEW lpHtmlView, LPTSTR lpUrl);
static void SMIE_InsertUrlList(LPHTMLVIEW lpHtmlView, LPTSTR lpUrl,LPTSTR lpLocateFile);
static void SMIE_DeleteUrlList(LPURLLIST lpUrlList);

static LPTSTR  SMIE_GetFullUrl(HWND hWnd,LPTSTR lpUrl);
static void SMIE_DisplayMessage(UINT uMsg);
static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoRefresh(HWND hWnd,WPARAM wParam,LPARAM lParam);

static BOOL SMIE_ApartMark(LPTSTR lpUrl,LPTSTR *lpPureUrl,LPTSTR *lpMark);


static LRESULT SMIE_InterentEvnetHandler(HWND hWnd, WPARAM  wParam, LPARAM lParam  );


// !!! Add By Jami chen in 2003.09.12
static LRESULT DoGetFileType(HWND hWnd,WPARAM wParam,LPARAM lParam);
// !!! Add End By Jami chen in 2003.09.12

// !!! Add By Jami chen in 2003.09.13
static LRESULT DoGetLocateFile(HWND hWnd,WPARAM wParam,LPARAM lParam);
// !!! Add End By Jami chen in 2003.09.13

static LRESULT DoReAdjustSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFontSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDisplayMode(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void WaitExitThread(HWND hWnd);

static void DrawToScreen(HWND hWnd,HDC hdc);
static HDC CreateMemoryDC(HWND hWnd);

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );

static BOOL IsInternetUrl(LPTSTR lpUrl);
//**************************************************************

// **************************************************
// 声明：ATOM RegisterSMIE_BrowersClass( HINSTANCE hInst )
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回注册类的结果
// 功能描述：注册小屏幕模式的IE浏览类
// 引用: 
// **************************************************
ATOM RegisterSMIE_BrowersClass( HINSTANCE hInst )
{
    WNDCLASS wc;

/*#ifdef INTERNET_SUPPORT
	WSADATA   wsdata;

	int  iRet = WSAStartup ( 0x0201 , &wsdata);  // start internet
#endif
*/

    wc.style = 0;
    wc.lpfnWndProc = SMIE_BrowersProc;  // 设置过程句柄
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // 设置背景
    wc.lpszMenuName = 0;
    wc.lpszClassName = classSMIE_BROWERS;  // 设置类名

    return (BOOL)(RegisterClass( &wc ));  // 向系统注册
}


// **************************************************
// 声明：LRESULT CALLBACK SMIE_BrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN uMsg -- 窗口消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：返回各消息的处理结果
// 功能描述：控件消息过程函数
// 引用: 
// **************************************************
LRESULT CALLBACK SMIE_BrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_PAINT:  // 绘制
			hdc = BeginPaint(hWnd, &ps);
			DoPAINT( hWnd ,hdc,ps.rcPaint);
			EndPaint(hWnd, &ps);
		  return 0;
   	case WM_LBUTTONDOWN:
//			StartAnimation(hWnd);
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE:
//			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP:
//			return DoLButtonUp(hWnd,wParam,lParam);
		break;
	case WM_MOUSEWHEEL:
		return DoMouseWheel(hWnd,wParam,lParam);
	case WM_KEYDOWN:
		DoKeyDown(hWnd,wParam,lParam);
		break;
   case WM_VSCROLL:
	  DoVScrollWindow(hWnd,wParam,lParam);
	  break;
	case WM_HSCROLL:
	  DoHScrollWindow(hWnd,wParam,lParam);
	  break;
    case WM_KILLFOCUS:
//      return DoKillFocus(hWnd,wParam,lParam);
		break;
	case WM_TIMER:
		DoTimer(hWnd,wParam,lParam);
		break;
	  case WM_CREATE:
	      DoCreate(hWnd,wParam,lParam);
		  return 0;
	case WM_DESTROY:
      return DoDestory(hWnd,wParam,lParam);

//	case HM_LOADFILE:
//		return DoLoadFile(hWnd,wParam,lParam);
	case HM_GO:
		return DoGo(hWnd,wParam,lParam);
	case HM_GOBACK:
		return DoBackWards(hWnd,wParam,lParam);
	case HM_FORWARDS:
		return DoForWards(hWnd,wParam,lParam);
	case HM_REFRESH:
		return DoRefresh(hWnd,wParam,lParam);
	case HM_GOMARK:
		return DoGoMark(hWnd,wParam,lParam);
	case HM_READDATA:
		return DoReadData(hWnd,wParam,lParam);
// !!! Add By Jami chen in 2003.09.12
	case HM_GETFILETYPE:
		return DoGetFileType(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2003.09.12
// !!! Add By Jami chen in 2003.09.13
	case HM_GETLOCATEFILE:
		return DoGetLocateFile(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2003.09.13

	case HM_READJUSTSIZE:
		return DoReAdjustSize(hWnd,wParam,lParam);

	case HM_STOP:
		return DoStop(hWnd,wParam,lParam);
#ifdef INTERNET_SUPPORT
	case WM_INTERNET_EVENT:
		return SMIE_InterentEvnetHandler( hWnd, wParam, lParam);
//		return DoInternetEvent(hWnd,wParam,lParam);
#endif

	case HM_SETFONTSIZE: // 设置字体大小
		return DoSetFontSize(hWnd,wParam,lParam);
	case HM_SETDISPLAYMODE: // 设置显示模式
		return DoSetDisplayMode(hWnd,wParam,lParam);
	case WM_SIZE: // 窗口大小发生改变
		return DoSize(hWnd,wParam,lParam);
	case WM_WINDOWPOSCHANGED:
		return DoWindowPosChanged(hWnd,wParam,lParam);
	default:
		  //DisplayMessage(uMsg);
    	  return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：成功返回0，否则返回-1。
// 功能描述：创建控件，处理WM_CREATE消息。
// 引用: 
// **************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	HANDLE handle;
	TEXTMETRIC TextMetric;
	HDC hdc;
//	int i;

//		RETAILMSG(1,(TEXT("Create IE Control ...\r\n")));
	    // 分配结构
		lpHtmlView=(LPHTMLVIEW)malloc(sizeof(HTMLVIEW));
		if (lpHtmlView == NULL)
			return -1;
		SetWindowLong(hWnd,0,(long)lpHtmlView); // 将HTML结构设置到窗口

		// 初始化结构
		lpHtmlView->hHtml=NULL;
		lpHtmlView->x_Org=0;
		lpHtmlView->y_Org=0;
		lpHtmlView->lpUrlHead=NULL;
		lpHtmlView->lpCurUrl=NULL;
		lpHtmlView->lpCurFile=NULL;
		lpHtmlView->lpUrl=NULL;
		lpHtmlView->lpMark=NULL;


//		lpHtmlView->uShowMode = NORMAL_MODE ; //COMPACT_MODE;
		lpHtmlView->uShowMode = COMPACT_MODE;
//		lpHtmlView->lpCurPath=SMIE_BufferAssignTChar(TEXT("\\"));
		lpHtmlView->lpCurPath=BufferAssignTChar(TEXT("\\"));  // 设置根路径为当前路径

#ifdef INTERNET_SUPPORT
		// 初始化INTERNET变量参数
		lpHtmlView->hInternetSiteInit=NULL;
		lpHtmlView->lpDownLoadList=NULL;
		lpHtmlView->lpRunningList = NULL;
#endif
#ifdef LOADANIMATION
		// 初始化动画变量
		lpHtmlView->hAnimation = NULL;
		lpHtmlView->bExitAnimation = FALSE;
#endif


		lpHtmlView->iRunThreadCount = 0;
		lpHtmlView->bExit = FALSE;
		lpHtmlView->bStop = FALSE;

		lpHtmlView->hRasConn = NULL; 
		// 设置默认的字体	
		lpHtmlView->iFontSize = FONT_BIGGER; // 默认为大字体
		hdc=GetDC(hWnd);

		// get current text property
		GetTextMetrics(hdc,&TextMetric);
		ReleaseDC(hWnd,hdc);
		lpHtmlView->iTextHeight = TextMetric.tmHeight;

		//设置默认的显示模式
		lpHtmlView->iDisplayMode = DISPLAY_TEXTIMAGE; 
		// 初始化临界变量
		INITLOCK(lpHtmlView->DownLoadCriticalSection);
		INITLOCK(lpHtmlView->RunningCriticalSection);
		
		lpHtmlView->lpShowImageList = NULL; // 开始没有要显示的图象控件

		CreateMemoryDC(hWnd);

		SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条

		lpHtmlView->hDownloadEvent = CreateEvent(NULL,TRUE,FALSE,"Download");
		lpHtmlView->hExitEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

		handle = CreateThread(NULL, 0, DownLoadmanageThread, (void *)hWnd, 0, 0 );  // 创建下载管理线程
		CloseHandle(handle);  // 关闭句柄，线程已经创建，句柄没有作用

#ifdef USE_TIMERTOSHOWIMG
		SetTimer(hWnd,100,TIMEINTERVAL,NULL);
#else
		handle = CreateThread(NULL, 0, ShowImageThread, (void *)hWnd, 0, 0 );  // 创建下载管理线程
//		SetThreadPriority(handle,THREAD_PRIORITY_ABOVE_NORMAL);
		CloseHandle(handle);  // 关闭句柄，线程已经创建，句柄没有作用
#endif
		SetFocus(hWnd);  // 设置焦点到当前窗口

		JPEG_Init(); // JPEG 系统初始化，可以西显示JPEG文件

/*		{
			LPTSTR lpNewUrl;

				lpNewUrl = MakeInternetRelativePath("http://www.163.com","index.gif");
				free(lpNewUrl);
				lpNewUrl = MakeInternetRelativePath("http://www.163.com/download.htm","index.gif");
				free(lpNewUrl);
				lpNewUrl = MakeInternetRelativePath("http://www.163.com/download","index.gif");
				free(lpNewUrl);
				lpNewUrl = MakeInternetRelativePath("http://www.163.com/download/","image/index.gif");
				free(lpNewUrl);
				lpNewUrl = MakeInternetRelativePath("http://www.163.com/download/","../image/index.gif");
				free(lpNewUrl);
		}
*/
//		RETAILMSG(1,(TEXT("Create IE Control OK\r\n")));
		return 0;
}

// **************************************************
// 声明：static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：破坏控件，处理WM_DESTROY消息。
// 引用: 
// **************************************************
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;

		lpHtmlView->bExit = TRUE;  // 设置退出系统
		SetEvent(lpHtmlView->hExitEvent);
//		EndAnimation(hWnd);
//		RETAILMSG(1,("Stop Current DownLoad\r\n"));
		DoStop(hWnd,0,0);  // 停止当前正在运行的线程

//		RETAILMSG(1,("Will HandDown Internet [%x]\r\n",lpHtmlView->hRasConn));
		HandDownInternet(hWnd,lpHtmlView->hRasConn); // 挂断与网络的联系

		if (lpHtmlView->hInternetSiteInit)
		{  // 关闭INTERNET
			InternetCloseHandle(lpHtmlView->hInternetSiteInit);
			WSACleanup();
		}
//		RETAILMSG(1,("###################Sleep\r\n"));
//		Sleep(10000);
		WaitExitThread(hWnd);  // 等待其他线程退出

		if (lpHtmlView->hMemoryDC)
			DeleteDC(lpHtmlView->hMemoryDC);

		if (lpHtmlView->hMemoryBitmap)
			DeleteObject(lpHtmlView->hMemoryBitmap);

		if (lpHtmlView->lpUrl!=NULL)
		{  // 释放当前网页的缓存
			free(lpHtmlView->lpUrl);
			lpHtmlView->lpUrl=NULL;
		}
		if (lpHtmlView->lpCurPath)
		{  // 释放当前路径的缓存
			free(lpHtmlView->lpCurPath);
			lpHtmlView->lpCurPath=NULL;
		}
		if (lpHtmlView->lpCurFile!=NULL)
		{  // 释放当前文件的缓存
			free(lpHtmlView->lpCurFile);
			lpHtmlView->lpCurFile=NULL;
		}
		if (lpHtmlView->lpMark!=NULL)
		{  // 释放当前标记的缓存
			free(lpHtmlView->lpMark);
			lpHtmlView->lpMark=NULL;
		}
		if (lpHtmlView->hHtml)
		{  // 释放HTML
			ReleaseAnimationList(hWnd); // 释放正在显示的列表
			SMIE_ReleaseHtml(lpHtmlView->hHtml);
			lpHtmlView->hHtml=NULL;
		}
		if (lpHtmlView->lpUrlHead)
			SMIE_DeleteUrlList(lpHtmlView->lpUrlHead);  // 删除URL列表

#ifdef INTERNET_SUPPORT
//		while(lpHtmlView->lpDownLoadList)
//		{
//			DeleteDownList(hWnd);
//		}
#endif
		//释放临界变量
		DEINITLOCK(lpHtmlView->DownLoadCriticalSection);  
		DEINITLOCK(lpHtmlView->RunningCriticalSection);

		// 释放HTML结构
		free(lpHtmlView);
	    SetWindowLong(hWnd,0,0);  // 设置NULL到窗口
		JPEG_Deinit(); // 释放JPEG系统

		PostQuitMessage(0);  // ？？？？？？？？？？？？？？？？
		return 0;
}
// **************************************************
// 声明：static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint)
// 参数：
// 	IN hWnd -- 窗口句柄
//	 IN hdc -- 设备句柄
// 	IN rcPaint -- 要重新绘制的区域
// 
// 返回值：无
// 功能描述：绘制控件，处理WM_PAINT消息。
// 引用: 
// **************************************************
static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint)
{
//	LPHTMLOBJECT lpHead;
//	LPHTMLVIEW lpHtmlView;
//	POINT ptOrg;
//		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
//		if (lpHtmlView==NULL)
//			return;

// !!! modified By Jami chen in 2004.08.19
/*
		if (lpHtmlView->iFontSize == FONT_SMALLLER)
		{// 是小字体
			SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
		}
//#ifndef __WCE_DEFINE
//		SetWindowOrgEx(hdc,lpHtmlView->x_Org,lpHtmlView->y_Org,NULL);
//#else
//		SetViewportOrgEx(hdc,(0-lpHtmlView->x_Org),(0-lpHtmlView->y_Org),NULL);
//#endif
		ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
		ptOrg.y=lpHtmlView->y_Org;
 		SMIE_ShowHtml(hWnd,hdc,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
*/
		DrawToScreen(hWnd,hdc);
// !!! modified end By Jami chen in 2004.08.19
}
// **************************************************
// 声明：static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：处理控件的定时器，处理WM_TIMER消息。
// 引用: 
// **************************************************
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
#ifdef USE_TIMERTOSHOWIMG
		ShowImage(hWnd);
#endif
		return 0;
}

// **************************************************
// 声明：static LRESULT DoGo(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR 要打开的网页的地址
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：打开一个指定的网页，处理HM_GO消息。
// 引用: 
// **************************************************
static LRESULT DoGo(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpUrl,lpFullUrl;
	LRESULT lResult;
	LPHTMLVIEW lpHtmlView;


//		RETAILMSG(1,("############################\r\n"));
//		RETAILMSG(1,("############################\r\n"));

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTMLVIEW 结构
		if (lpHtmlView==NULL)
			return 0;

//		EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);
//		EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
		
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针到当前进程
#endif
		lpUrl=(TCHAR *)lParam;
		if (lpHtmlView->lpUrl!=NULL)
		{
			SendMessage(hWnd,HM_STOP,0,0);  // 停止当前的网页下载
			free(lpHtmlView->lpUrl);  // 释放当前网页
			lpHtmlView->lpUrl=NULL;
		}
		//  if is locate file ,then add the directory
		RETAILMSG(1, ("Befor get full (%s) \r\n", lpUrl));
		lpFullUrl = SMIE_GetFullUrl(hWnd,lpUrl);  // 得到VIEW的当前网页
		RETAILMSG(1, ("after get full (%s) \r\n", lpFullUrl));
		if (IsInternetUrl(lpFullUrl) == TRUE)
		{
			// 当前URL是一个网页，打开网页
			lpHtmlView->lpUrl = lpFullUrl;
			RETAILMSG(1, ("SMIE 1111111111111(%s)\r\n", lpFullUrl));
			lResult=SMIE_GoUrl(hWnd,lpHtmlView->lpUrl);  // 打开当前网页
		}
		else
		{
			// 当前打开的是一个其他文件，下载这个文件
			RETAILMSG(1, ("SMIE 22222\r\n"));
			lpHtmlView->bStop = FALSE;  // 设置停止变量为FALSE
			SMIE_DownLoadSpecialFile(hWnd,lpFullUrl);
		}
		return lResult;
}
// **************************************************
// 声明：static LRESULT SMIE_GoUrl(HWND hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 要打开的网页地址
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：打开指定的网页。
// 引用: 
// **************************************************
static LRESULT SMIE_GoUrl(HWND hWnd,LPTSTR lpUrl)
{
	HANDLE in_file;
//	DWORD dwFileLen;//,dwReadLen;
//	char *lpFileContent;//,*lpPtr;
//	char pPropertyContent[1024];
//	TCHAR *lpFileName;
//	HDC hdc;
	LPTSTR lpPureUrl,lpMark;
	POINT ptOrg;

	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTMLVIEW 结构
		if (lpHtmlView==NULL)
			return 0;

		// 如果还有程序在下载，先停止下载
		DoStop(hWnd,0,0);  // 停止当前正在运行的线程

		lpHtmlView->bStop = FALSE;  // 设置停止变量为FALSE
//		lpFileName=(TCHAR *)lParam;


		// !!! Add By Jami chen in 2004.08.07
		if (strlen(lpUrl) == 0)
		{
			// 是空白网页
			if (lpHtmlView->lpCurFile)
				free(lpHtmlView->lpCurFile);  // 删除原来的当前文件
			lpHtmlView->lpCurFile = NULL;//SMIE_BufferAssignTChar(lpUrl);  // 设置当前的网页为当前文件

			if (lpHtmlView->lpMark)
				free(lpHtmlView->lpMark);  // 删除原来的标记
			lpHtmlView->lpMark = NULL;//SMIE_BufferAssignTChar(lpUrl);  // 设置当前的标记
			if (lpHtmlView->hHtml)
			{  // 释放原来的HTML
				ReleaseAnimationList(hWnd); // 释放正在显示的列表
				SMIE_ReleaseHtml(lpHtmlView->hHtml);
				lpHtmlView->hHtml=NULL;
			}
			EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);  // 无效滚动条
			EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条
			lpHtmlView->x_Org=0;  // 显示原点回到(0,0)
			lpHtmlView->y_Org=0;
// !!! modified By Jami chen in 2004.08.19
			ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
			ptOrg.y=lpHtmlView->y_Org;
			if (lpHtmlView->uShowMode == COMPACT_MODE)
	 			SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
//			else
//	 			ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
			InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
			return 0;
		}
		// !!! Add End By Jami chen in 2004.08.07


//		JAMIMSG(DBG_FUNCTION,(TEXT("Enter DoLoadFile\r\n")));
//		JAMIMSG(DBG_FUNCTION,(TEXT("Will Load FileName is \"%s\"\r\n"),lpFileName));

//		SetWindowText(GetDlgItem(hWnd,IDC_ADDRESS),lpUrl);
		
		RETAILMSG(1, ("URL :(%s)...\r\n", lpUrl));
		SMIE_ApartMark(lpUrl,&lpPureUrl,&lpMark);  // 分离网页中的标记与网址
		RETAILMSG(1, ("URL :(%s) after apart...\r\n", lpUrl));

		in_file=CreateFile(lpPureUrl,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);  // 打开网址指定的文件
		if (in_file==INVALID_HANDLE_VALUE)
		{  // This is not Locale file
			if (lpHtmlView->lpMark) 
				free(lpHtmlView->lpMark);  // 删除原来的标记
			lpHtmlView->lpMark=lpMark;//SMIE_BufferAssignTChar(lpUrl);  // 设置当前的标记

#ifdef INTERNET_SUPPORT
			iStartTick = GetTickCount();
//			RETAILMSG(1,("Will Load  \"%s\"\r\n",lpPureUrl));
			SMIE_StartNewFtpSite(hWnd,lpPureUrl);  // 打开指定的网页
			if (lpPureUrl)
				free(lpPureUrl);  // 释放不用的缓存
			return TRUE;
#else
			if (lpPureUrl)
				free(lpPureUrl); // 释放不用的缓存
			if (lpHtmlView->lpCurFile)
				free(lpHtmlView->lpCurFile);  // 释放不用的缓存
			lpHtmlView->lpCurFile=NULL;   // 设置当前文件为NULL
			PostMessage(hWnd,HM_REFRESH,0,0);  // 刷新画面
			return FALSE;
#endif
//			in_file=CreateFile(lpTempFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
//			if (in_file==INVALID_HANDLE_VALUE)
		}
		SMIE_SetCurPath(hWnd,lpPureUrl);  // 设置当前路径到文件的路径
		CloseHandle(in_file);

		if (lpHtmlView->lpCurFile)
			free(lpHtmlView->lpCurFile);  // 删除原来的当前文件
		lpHtmlView->lpCurFile=lpPureUrl;//SMIE_BufferAssignTChar(lpUrl);  // 设置当前的网页为当前文件

		if (lpHtmlView->lpMark)
			free(lpHtmlView->lpMark);  // 删除原来的标记
		lpHtmlView->lpMark=lpMark;//SMIE_BufferAssignTChar(lpUrl);  // 设置当前的标记

//		RETAILMSG(1,("M222MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
		PostMessage(hWnd,HM_REFRESH,0,0);  // 刷新画面
		return TRUE;
}
// **************************************************
// 声明：static LRESULT DoRefresh(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：刷新当前页面，处理HM_REFRESH消息。
// 引用: 
// **************************************************
static LRESULT DoRefresh(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	HANDLE in_file;
	DWORD dwFileLen,dwReadLen;
	char *lpFileContent;//,*lpPtr;
//	char pPropertyContent[1024];
//	HDC hdc;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return FALSE;
//		RETAILMSG(1,(TEXT("DoRefresh!!!\r\n")));
		if (lpHtmlView->lpCurFile==NULL)
		{  // 没有本地文件
			if (lpHtmlView->lpUrl)
			{  // 是否要打开一个指定的网页
				MessageBox(hWnd,lpHtmlView->lpUrl,"打开网页失败",MB_OK);
				free(lpHtmlView->lpUrl);
				lpHtmlView->lpUrl=NULL;
			}
			return FALSE;
		}
//		RETAILMSG(1,(TEXT("Will Open File <%s>\r\n"),lpHtmlView->lpCurFile));
		in_file=CreateFile(lpHtmlView->lpCurFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);  // 打开文件
		if (lpHtmlView->lpUrl)
		{  // 将网页及对应的本地文件添加到已经打开的文件列表中
			SMIE_InsertUrlList(lpHtmlView,lpHtmlView->lpUrl,lpHtmlView->lpCurFile);
			free(lpHtmlView->lpUrl);  // 释放网页缓存
			lpHtmlView->lpUrl=NULL;
		}
		if (in_file==INVALID_HANDLE_VALUE)
		{  // 打开文件失败
			MessageBox(hWnd,lpHtmlView->lpCurFile,"打开网页失败",MB_OK);
			return FALSE;
		}

		//RETAILMSG(1,(TEXT("Open File Success!!!\r\n")));
//#ifdef INTERNET_SUPPORT
		SMIE_SendInternetNotifyMessage(hWnd,INM_ADDRESSCHANGE,lpHtmlView->lpCurUrl->lpUrl);  // 发送消息给父窗口，告知当前网页已经改变
		EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);  // 无效滚动条
		EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
//#endif

// !!! Add By Jami chen in 2003.09.09
		if (IsImageFile(lpHtmlView->lpCurFile))
		{  // 打开的是一个图象文件
			CloseHandle(in_file);  // 关闭文件句柄
			lpHtmlView->iFileStyle = IMAGE_FILE;
			if (lpHtmlView->hHtml)
			{  // 释放原来文件的HTML句柄
				ReleaseAnimationList(hWnd); // 释放正在显示的列表
				SMIE_ReleaseHtml(lpHtmlView->hHtml);
				lpHtmlView->hHtml=NULL;
			}
			lpHtmlView->hHtml = SMIE_LoadImage(hWnd,lpHtmlView->lpCurFile);  // 加载图象文件
//	!!! modified By Jami chen in 2004.08.19
//			hdc=GetDC(hWnd);
//			if (lpHtmlView->iFontSize == FONT_SMALLLER)
//			{// 是小字体
//				SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//			}
//			SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // 计算需要的尺寸
//			ReleaseDC(hWnd,hdc);
//			SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // 计算需要的尺寸
//	!!! modified End By Jami chen in 2004.08.19

			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条
			lpHtmlView->x_Org=0;  // 显示原点回到(0,0)
			lpHtmlView->y_Org=0;
// !!! modified By Jami chen in 2004.08.19
//			InvalidateRect(hWnd,NULL,TRUE);
			ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
			ptOrg.y=lpHtmlView->y_Org;
	 		SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
			InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
//			ShowImageThread((VOID * )hWnd);
			return TRUE;  // 成功返回
		}
// !!! Add End By Jami chen in 2003.09.09
		lpHtmlView->iFileStyle = HTML_FILE;  // 一般HTML文件

//		RETAILMSG(1,(TEXT("N11\r\n")));
		dwFileLen=GetFileSize(in_file,NULL);  // 得到文件大小
//		RETAILMSG(1,(TEXT("N22\r\n")));
		lpFileContent=(char *)malloc(dwFileLen+1);  // 分配缓存来存放文件内容
//		RETAILMSG(1,(TEXT("N33\r\n")));
		if (lpFileContent==NULL)
		{  // 分配内存失败
			CloseHandle(in_file);  // 关闭文件句柄
//			CloseHandle(in_file);
			return FALSE;
		}
//		RETAILMSG(1,(TEXT("N44\r\n")));
		ReadFile(in_file,lpFileContent,dwFileLen,&dwReadLen,NULL);  // 读取文件内容
		if (dwReadLen == 0)
		{
			DWORD err = GetLastError();
			CloseHandle(in_file);  // 关闭文件句柄
			return FALSE;
		}
//		RETAILMSG(1,(TEXT("N55\r\n")));
		lpFileContent[dwReadLen]=0;  // 添加结尾符
//		RETAILMSG(1,(TEXT("N66\r\n")));
//		CloseHandle(in_file);  // 关闭文件句柄
		CloseHandle(in_file);  // 关闭文件句柄

//		RETAILMSG(1,(TEXT("Read File OK!!!\r\n")));

		if (lpHtmlView->hHtml)
		{  // 释放原来的HTML
			ReleaseAnimationList(hWnd); // 释放正在显示的列表
			SMIE_ReleaseHtml(lpHtmlView->hHtml);
			lpHtmlView->hHtml=NULL;
		}

		lpHtmlView->hHtml=SMIE_ParseHtml(hWnd,lpFileContent);  // 分析当前文件

//		RETAILMSG(1,(TEXT("SMIE_ParseHtml OK!!!\r\n")));
// !!! modified by jami chen in 2004.08.19
//		hdc=GetDC(hWnd);
//		if (lpHtmlView->iFontSize == FONT_SMALLLER)
//		{// 是小字体
//			SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//		}
//		SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // 计算大小
//		ReleaseDC(hWnd,hdc);
		if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // 是否是图形文件, 如果是图象文件，则不需要重新计算尺寸
			SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // 计算需要的尺寸
// !!! modified end By Jami chen in 2004.08.19
//		RETAILMSG(1,(TEXT("SMIE_ReCalcSize OK!!!\r\n")));
		SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条
		lpHtmlView->x_Org=0;
		lpHtmlView->y_Org=0;
//		bReCalcSize=FALSE;
		free(lpFileContent);  // 释放文件内容的缓存
// !!! modified By Jami chen in 2004.08.19
//		InvalidateRect(hWnd,NULL,TRUE);
		ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
		ptOrg.y=lpHtmlView->y_Org;
	 	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
		InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19

		if (lpHtmlView->lpMark)  // 如果需要定位到一个指定的标记
			SendMessage(hWnd,HM_GOMARK,0,(LPARAM)lpHtmlView->lpMark);
		return TRUE;  // 成功返回
}

// **************************************************
// 声明：static LRESULT DoBackWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：返回到前一个页面，处理HM_GOBACK消息。
// 引用: 
// **************************************************
static LRESULT DoBackWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;
		if (lpHtmlView->lpCurUrl==NULL)  // 当前页面为空
			return 0;
		if (lpHtmlView->lpUrlHead==lpHtmlView->lpCurUrl)  // 已经到第一页
			return 0;
		lpHtmlView->lpCurUrl=lpHtmlView->lpCurUrl->prev;  // 将当前网页设定到前一个网页
		if (lpHtmlView->lpCurUrl->lpUrl)
		{
			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpLocateFile);  // 打开前一个网页
//			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpUrl);
//			PostMessage(hWnd,HM_REFRESH,0,0);

//			SendMessage(hWnd,HM_GO,0,(LPARAM)lpHtmlView->lpCurUrl->lpUrl);
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：前进到下一个页面，处理HM_FORWARDS消息。
// 引用: 
// **************************************************
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;
		if (lpHtmlView->lpCurUrl==NULL)  // 当前网页为空
			return 0;
		if (lpHtmlView->lpCurUrl->next==NULL)  // 已经到最后一个网页
			return 0;
		lpHtmlView->lpCurUrl=lpHtmlView->lpCurUrl->next;  // 将当前网页设定到下一个网页
		if (lpHtmlView->lpCurUrl->lpUrl)
		{
			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpLocateFile);  // 打开当前网页
//			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpUrl);
//			SendMessage(hWnd,HM_GO,0,(LPARAM)lpHtmlView->lpCurUrl->lpUrl);
//			PostMessage(hWnd,HM_REFRESH,0,0);
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 当前的按键状态
//				  HIWORD 滚动的大小					
// 	IN lParam -- LOWORD 当前的水平位置
//				  HIWORD 当前的垂直位置
// 
// 返回值：无
// 功能描述：鼠标滚轮滚动，处理WM_MOUSEWHEEL消息。
// 引用: 
// **************************************************
static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	unsigned short fwKeys;
	short zDelta,xPos,yPos;
	short iPos;

	fwKeys = LOWORD(wParam);    // key flags
	zDelta = (short) HIWORD(wParam);    // wheel rotation
	xPos = (short) LOWORD(lParam);    // horizontal position of pointer
	yPos = (short) HIWORD(lParam);    // vertical position of pointer

	iPos=zDelta/STEPHEIGHT;  // 滚动的大小
	SMIE_VScrollWindow(hWnd,iPos);  // 滚动窗口
	return TRUE;
}

// **************************************************
// 声明：static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 按下键的虚键键值
// 	IN lParam -- 保留
// 
// 返回值：成功返回TRUE， 否则返回FALSE。
// 功能描述：有键按下，处理WM_KEYDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
	LPHTMLVIEW lpHtmlView;
	RECT rect;
	short nPage;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;
		nVirtKey = (int) wParam;    // virtual-key code
		GetClientRect(hWnd,&rect);  // 得到窗口客户区域大小
        
		nPage=(rect.bottom-rect.top)/STEPHEIGHT;  //得到一页要滚动的行数

		switch(nVirtKey)
		{
			case VK_UP:// to the pre Block
				SMIE_VScrollWindow(hWnd,-1);
				break;
			case VK_DOWN:// to the next Block
				SMIE_VScrollWindow(hWnd,1);
				break;
			case VK_LEFT:// to the left block
				SMIE_HScrollWindow(hWnd,-1);
				break;
			case VK_RIGHT:// to the right Block
				SMIE_HScrollWindow(hWnd,1);
				break;
			case VK_NEXT:// to the left block
				SMIE_VScrollWindow(hWnd,(short)( nPage -1 ));
				break;
			case VK_PRIOR:// to the right Block
				SMIE_VScrollWindow(hWnd,(short)(0 - nPage + 1));
				break;
			default :
				return TRUE;
		};
		return TRUE;
}

// **************************************************
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 当前鼠标的位置
// 
// 返回值：无
// 功能描述：鼠标左键按下，处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpUrl=NULL;
	DWORD x,y;
	LPHTMLVIEW lpHtmlView;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
	if (lpHtmlView==NULL)
		return 0;
	x=(short)LOWORD(lParam)+lpHtmlView->x_Org;  // 得到当前鼠标的点
	y=(short)HIWORD(lParam)+lpHtmlView->y_Org;
	SMIE_GetUrl(lpHtmlView->hHtml,x,y,&lpUrl);  // 得到鼠标位置的URL
//	RETAILMSG(1,("Get New Url is <%s>\r\n",lpUrl));
	if (lpUrl)
	{ // 有存在有效的URL
		if (*lpUrl=='#')
		{  // 是一个标记
			SendMessage(hWnd,HM_GOMARK,0,(LPARAM)lpUrl+1);  // 运行到有标记的位置
			free(lpUrl); // 释放URL
		}
		else
		{
			if (lpHtmlView->lpCurUrl->lpUrl)
			{  // 一个指向网页的URL
				if (strnicmp(lpUrl,MAILTO_HEAD,7)==0)
				{
					// 是一个邮件地址
					// 发邮件到lpUrl + 7
//					RETAILMSG(1,(TEXT("Will Send a Email to : %s\r\n"),lpUrl + 7));
					free(lpUrl);  // 释放URL
					return 0;
				}
				if (strnicmp(lpUrl,INTERNET_HEAD,5)!=0)
				{  // 不是一个全路径的网页
					// is a child string , must add the current url
					LPTSTR lpFullUrl;
					DWORD dwlen;
// !!! Delete By Jami chen in 2004.09.09
//					lpFullUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpUrl)+4);  // 分配网页所需要的缓存
//					if (lpFullUrl==NULL)  // 分配失败
//						return FALSE;
// !!! Delete End By Jami chen in 2004.09.09
					// !!! Add By Jami chen in 2003.09.09
					if (SMIE_IsLocateFile(hWnd,lpUrl))
					{  // 是一个本地文件
						lpFullUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurPath)+strlen(lpUrl)+4);  // 分配网页所需要的缓存
						if (lpFullUrl==NULL)  // 分配失败
							return FALSE;
						strcpy(lpFullUrl,lpHtmlView->lpCurPath); // 将当前路径拷贝到
						dwlen = strlen(lpFullUrl);
						if (lpFullUrl[ dwlen - 1]!='\\')
						{
						// !!! Add By Jami chen in 2003.09.09
							lpFullUrl[ dwlen]='\\';
						// !!! Add By Jami chen in 2003.09.09
							lpFullUrl[ dwlen + 1]=0;
						}
						strcat(lpFullUrl,lpUrl);  // 添加得到的URL作为子路径
						free(lpUrl);
						lpUrl = lpFullUrl;
					}
					else
					// !!! Add End By Jami chen in 2003.09.09
					{  // 是一个WWW地址
// !!! Modified  By Jami chen in 2004.09.09
						/*
						strcpy(lpFullUrl,lpHtmlView->lpCurUrl->lpUrl);  // 将当前的网页URL拷贝到当前路径
						//i=strlen(lpFullUrl);
						//while(lpFullUrl[i]!='/') i--;
						//lpFullUrl[i+1]=0;
						dwlen = strlen(lpFullUrl);
						if (lpFullUrl[ dwlen - 1]!='/')
						{
						// !!! Add By Jami chen in 2003.09.09
							lpFullUrl[ dwlen]='/';
						// !!! Add By Jami chen in 2003.09.09
							lpFullUrl[ dwlen + 1]=0;
						}
						*/
						lpFullUrl = MakeInternetRelativePath(lpHtmlView->lpCurUrl->lpUrl,lpUrl);
						free(lpUrl);
						lpUrl = lpFullUrl;
// !!! Modified  By Jami chen in 2004.09.09
					}
// !!! Delete By Jami chen in 2004.09.09
					//strcat(lpFullUrl,lpUrl);  // 添加得到的URL作为子路径
					//free(lpUrl);
					//lpUrl = lpFullUrl;
// !!! Delete End By Jami chen in 2004.09.09
				}
			}
	//		MessageBox(hWnd,lpUrl,TEXT("Get URL"),MB_OK);
//			RETAILMSG(1,("The New Url is <%s>\r\n",lpUrl));
			if (lpUrl == NULL)
				return 0;
			SendMessage(hWnd,HM_GO,0,(LPARAM)lpUrl);  // 打开当前URL
			free(lpUrl);  // 释放URL
		}
	}
	SetFocus(hWnd);
	return 0;
}

// **************************************************
// 声明：static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 滚动类型
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：垂直滚动窗口，处理WM_VSCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{

    short nScroll,nPage;
    short nPos;

//	DWORD widthWindow,heightWindow;
	SCROLLINFO ScrollInfo;

	RECT rect;
        
		GetClientRect(hWnd,&rect);  //得到窗口的客户区域
        
		nPage=(rect.bottom-rect.top)/STEPHEIGHT;
		ScrollInfo.cbSize=sizeof(SCROLLINFO);
		ScrollInfo.fMask=SIF_POS;
		ScrollInfo.nPos=0;
	    GetScrollInfo(hWnd,SB_VERT,&ScrollInfo);  // 得到当前的滚动条属性
		nPos=ScrollInfo.nPos;
//      nPageLines=GetPageLine(hWnd);
//      yPos=PDA_GetScrollPos(hWnd,SB_VERT);
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          nScroll=0-nPage + 1;
          break;
        case SB_PAGEDOWN:  //page down
          nScroll=nPage -1;
          break;
        case SB_LINEUP:  // line up
          nScroll=-1;
          break;
        case SB_LINEDOWN:  // line down
          nScroll=1;
          break;
        case SB_THUMBTRACK: // drag thumb track
          nScroll=HIWORD(wParam)-nPos;
          break;
        default:
          nScroll=0;
          return 0;
        }
				// vertical scroll window
        SMIE_VScrollWindow(hWnd,nScroll);

		return 0;
}
// **************************************************
// 声明：static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 滚动类型
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：水平滚动窗口，处理WM_HSCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    short nScroll,nPage;
    short nPos;

//	DWORD widthWindow,heightWindow;
	SCROLLINFO ScrollInfo;

	RECT rect;


		GetClientRect(hWnd,&rect);  // 得到客户区域
        
		nPage=(rect.right-rect.left)/STEPWIDTH;
		ScrollInfo.cbSize=sizeof(SCROLLINFO);
		ScrollInfo.fMask=SIF_POS;
		ScrollInfo.nPos=0;
	    GetScrollInfo(hWnd,SB_HORZ,&ScrollInfo);  // 得到滚动条的属性
		nPos=ScrollInfo.nPos;
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          nScroll=0-nPage;
          break;
        case SB_PAGEDOWN:  //page down
          nScroll=nPage;
          break;
        case SB_LINEUP:  // line up
          nScroll=-1;
          break;
        case SB_LINEDOWN:  // line down
          nScroll=1;
          break;
        case SB_THUMBTRACK: // drag thumb track
          nScroll=HIWORD(wParam)-nPos;
          break;
        default:
          nScroll=0;
          return 0;
        }
				// vertical scroll window
	SMIE_HScrollWindow(hWnd,nScroll);
	return 0;
}
// **************************************************
// 声明：static LRESULT DoGoMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR 指定的标号
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：将当前位置跳到指定的标号，处理HM_GOMARK消息。
// 引用: 
// **************************************************
static LRESULT DoGoMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	int xPos,yPos;
	LPTSTR lpMark;
	SCROLLINFO ScrollInfo;
	POINT ptOrg;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // // 得到HTML结构
	if (lpHtmlView==NULL)
		return 0;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射地址到当前进程
#endif
	lpMark=(LPTSTR)lParam;  // 得到标记
	if (SMIE_GetMarkPos(lpHtmlView->hHtml,&xPos,&yPos,lpMark)==FALSE)  // 得到标记的位置
		return FALSE;

	lpHtmlView->y_Org=yPos;
//	lpHtmlView->y_Org=600;
// !!! modified By Jami chen in 2004.08.19
//	InvalidateRect(hWnd,NULL,TRUE);
	ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
	ptOrg.y=lpHtmlView->y_Org;
	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
	InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19

	ScrollInfo.cbSize=sizeof(SCROLLINFO);
	ScrollInfo.fMask=SIF_POS;
//	ScrollInfo.nPos=0;
//	GetScrollInfo(hWnd,SB_VERT,&ScrollInfo);

//	ScrollInfo.nPos+=nScroll;
//	ScrollInfo.nPos=lpHtmlView->y_Org/STEPHEIGHT;
	ScrollInfo.nPos=(lpHtmlView->y_Org+STEPHEIGHT-1)/STEPHEIGHT;
	SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);  // 重设滚动条实行
	return TRUE;
}
// **************************************************
// 声明：static LRESULT DoReadData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 缓存的最大字节数
// 	IN lParam -- LPTSTR 要保留数据的缓存
// 
// 返回值：成功返回数据的大小，否则返回-1。
// 功能描述：得到当前网页的数据，处理HM_READDATA消息。
// 引用: 
// **************************************************
static LRESULT DoReadData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	int dwMaxBuffer;
	LPTSTR lpBuffer;
	HANDLE in_file;
	DWORD dwFileLen;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
		if (lpHtmlView==NULL)
			return 0;

		if (lpHtmlView->lpCurFile == NULL)  // 没有本地文件，退出
			return -1;

		dwMaxBuffer = (int)wParam;
		lpBuffer = (LPTSTR)lParam;

		// 打开文件
		in_file=CreateFile(lpHtmlView->lpCurFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if (in_file==INVALID_HANDLE_VALUE)
		{  // This is not Locale file
			return -1;
		}

		dwFileLen=GetFileSize(in_file,NULL);  // 得到文件长度
		if ( dwMaxBuffer == 0 && lpBuffer == NULL)
		{// 要求得到文件大小
//			CloseHandle(in_file);
			CloseHandle(in_file);  // 关闭文件句柄
			return dwFileLen;
		}
		if (dwFileLen > (DWORD)dwMaxBuffer)
		{  // 缓存太小
//			CloseHandle(in_file);
			CloseHandle(in_file);  // 关闭文件句柄
			return -1;
		}
		ReadFile(in_file,lpBuffer,dwMaxBuffer,&dwMaxBuffer,NULL);  // 读取数据
		CloseHandle(in_file);  // 关闭文件
		return dwMaxBuffer;  
}
// **************************************************
// 声明：static BOOL SMIE_ResetScrollBar(HWND hWnd,HHTML hHtml)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hHtml --  当前网页句柄
// 
// 返回值：无
// 功能描述：重设滚动条。
// 引用: 
// **************************************************
static BOOL SMIE_ResetScrollBar(HWND hWnd,HHTML hHtml)
{
	SIZE size;
	RECT rect;
	DWORD widthWindow,heightWindow;
	SCROLLINFO ScrollInfo;
	WORD nMinPos,nMaxPos,nPage;


		if (hHtml == NULL)
		{
			EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
			return FALSE;
		}
		SMIE_GetHtmlSize(hHtml,&size);  // 得到HTML的大小
		GetClientRect(hWnd,&rect); // 得到客户区域
		widthWindow=rect.right-rect.left;
		heightWindow=rect.bottom-rect.top;

		if (widthWindow<(DWORD)size.cx)
		{  // 需要水平滚动条
			nMinPos=0;
			nMaxPos=size.cx/STEPWIDTH - 1;
			nPage=(WORD)(widthWindow/STEPWIDTH);
//			ShowScrollBar(hWnd,SB_HORZ,TRUE);
			EnableScrollBar(hWnd,SB_HORZ, ESB_ENABLE_BOTH);

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
			// set horz scroll page
			ScrollInfo.nPage =nPage;
			// set horz scroll range
			ScrollInfo.nMin=nMinPos;
			ScrollInfo.nMax=nMaxPos;
			// set horz scroll position
			ScrollInfo.nPos=0;

			SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);  // 设置滚动条属性

		}
		else
		{  // 不需要水平滚动条
			EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);  
		}

		if (heightWindow<(DWORD)size.cy)
		{  // 需要垂直滚动条
			nMinPos=0;
			nMaxPos=size.cy/STEPHEIGHT -1;
			nPage=(WORD)(heightWindow/STEPHEIGHT);
			//ShowScrollBar(hWnd,SB_VERT,TRUE);
			EnableScrollBar(hWnd,SB_VERT, ESB_ENABLE_BOTH);

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE|SIF_RANGE|SIF_POS;
			// set horz scroll page
			ScrollInfo.nPage =nPage;
			// set horz scroll range
			ScrollInfo.nMin=nMinPos;
			ScrollInfo.nMax=nMaxPos;
			// set horz scroll position
			ScrollInfo.nPos=0;

			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);  // 设置垂直滚动条属性
		}
		else
		{  // 不需要垂直滚动条
			EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
		}
		return TRUE;
}
// **************************************************
// 声明：static void SMIE_HScrollWindow(HWND hWnd,short nScroll)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nScroll -- 滚动大小
// 
// 返回值：无
// 功能描述：水平滚动窗口。
// 引用: 
// **************************************************
static void SMIE_HScrollWindow(HWND hWnd,short nScroll)
{
	short nScrollSize,widthWindow;
	RECT rect;
	LPHTMLVIEW lpHtmlView;
	SIZE size;
//	SCROLLINFO ScrollInfo;
	POINT ptOrg;


	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTML结构
	if (lpHtmlView==NULL)
		return ;

	GetClientRect(hWnd,&rect);  // 得到窗口客户区域
	widthWindow=(short)(rect.right-rect.left);

	SMIE_GetHtmlSize(lpHtmlView->hHtml,&size);  // 得到HTML的大小


	if (nScroll>0)
	{  // scroll to right
		if (lpHtmlView->x_Org+widthWindow>=size.cx)  // have not scroll to right
			return ;
		nScrollSize=nScroll*STEPWIDTH;

		if (lpHtmlView->x_Org+widthWindow+nScrollSize>size.cx)  // have not scroll to right all size
		{
			nScrollSize=size.cx-(lpHtmlView->x_Org+widthWindow);
		}

	}
	else
	{	// scroll to left
		if (lpHtmlView->x_Org==0)
		{
			return;
		}
		nScrollSize=nScroll*STEPWIDTH;
		if (lpHtmlView->x_Org+nScrollSize<0)  // lpHtmlView->x_Org > 0 ,and ScrollSize <0
		{
			nScrollSize=(short)(0-lpHtmlView->x_Org);
		}
	}
//	ScrollWindowEx(hWnd,(0-nScrollSize),0,NULL,&rect,NULL,NULL,SW_SCROLLCHILDREN|SW_INVALIDATE);
	lpHtmlView->x_Org+=nScrollSize;
//	InvalidateRect(hWnd,NULL,TRUE);  // 重画窗口
// !!! modified By Jami chen in 2004.08.19
//	InvalidateRect(hWnd,NULL,TRUE);
	ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
	ptOrg.y=lpHtmlView->y_Org;
	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
	InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19

	/*
	ScrollInfo.cbSize=sizeof(SCROLLINFO);
	ScrollInfo.fMask=SIF_POS;
//	ScrollInfo.nPos=0;
//	GetScrollInfo(hWnd,SB_HORZ,&ScrollInfo);

//	ScrollInfo.nPos+=nScroll;
//	ScrollInfo.nPos=lpHtmlView->x_Org/STEPWIDTH;
	ScrollInfo.nPos=(lpHtmlView->x_Org+STEPWIDTH-1)/STEPWIDTH;
	SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);  // 设置水平滚动条属性

  */
	SetCurrentScrollPos(hWnd,SB_HORZ);

}
// **************************************************
// 声明：static void SMIE_VScrollWindow(HWND hWnd,short nScroll)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nScroll -- 滚动大小
// 
// 返回值：无
// 功能描述：垂直滚动窗口。
// 引用: 
// **************************************************
static void SMIE_VScrollWindow(HWND hWnd,short nScroll)
{
	short nScrollSize,heightWindow;
	RECT rect;
	LPHTMLVIEW lpHtmlView;
	SIZE size;
//	SCROLLINFO ScrollInfo;
	POINT ptOrg;


	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); // 得到HTML结构
	if (lpHtmlView==NULL)
		return ;

	GetClientRect(hWnd,&rect);  // 得到窗口客户区域
	heightWindow=(short)(rect.bottom-rect.top);

	if (lpHtmlView->hHtml == NULL)
	{
		// 还没有打开任何的网页
		size.cx = size.cy = 0;
	}
	else
	{
		SMIE_GetHtmlSize(lpHtmlView->hHtml,&size);  // 得到HTML的大小
	}


	if (nScroll>0)
	{  // scroll to down
		if (lpHtmlView->y_Org+heightWindow>=size.cy)  // have not scroll to down
			return ;
		nScrollSize=nScroll*STEPHEIGHT;

		if (lpHtmlView->y_Org+heightWindow+nScrollSize>size.cy)  // have not scroll to down all size
		{
			nScrollSize=size.cy-(lpHtmlView->y_Org+heightWindow);
		}

	}
	else
	{	// scroll to Up
		if (lpHtmlView->y_Org==0)
		{
			return;
		}
		nScrollSize=nScroll*STEPHEIGHT;
		if (lpHtmlView->y_Org+nScrollSize<0)  // lpHtmlView->y_Org > 0 ,and ScrollSize <0
		{
			nScrollSize=(short)(0-lpHtmlView->y_Org);
		}
	}
	//ScrollWindowEx(hWnd,0,(0-nScrollSize),NULL,&rect,NULL,NULL,SW_SCROLLCHILDREN|SW_INVALIDATE);
	lpHtmlView->y_Org+=nScrollSize;
//	InvalidateRect(hWnd,NULL,TRUE);  // 重绘窗口
// !!! modified By Jami chen in 2004.08.19
//	InvalidateRect(hWnd,NULL,TRUE);
	ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
	ptOrg.y=lpHtmlView->y_Org;
	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
	InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
/*	
	{
		HDC hdc;
		hdc=GetDC(hWnd);
 		SMIE_ShowHtml(hWnd,hdc,lpHtmlView->hHtml);
		ReleaseDC(hWnd,hdc);
	}
*/
	SetCurrentScrollPos(hWnd,SB_VERT);

}
//#ifdef INTERNET_SUPPORT
// **************************************************
// 声明：static void SMIE_SendInternetNotifyMessage(HWND hWnd,UINT code,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN code -- 通知消息代码
// 	IN lpUrl -- 当前打开的网页地址
// 
// 返回值：无
// 功能描述：发送一个通知消息到父窗口。
// 引用: 
// **************************************************
static void SMIE_SendInternetNotifyMessage(HWND hWnd,UINT code,LPTSTR lpUrl)
{
	NMINTERNET nmInternet;

		nmInternet.hdr.hwndFrom=hWnd;
		nmInternet.hdr.idFrom=GetWindowLong(hWnd,GWL_ID);
		nmInternet.hdr.code=code;
		nmInternet.lpUrl=lpUrl;

		// 发送一个INTERNET的通知消息到父窗口
		SendMessage(GetParent(hWnd),WM_NOTIFY,(WPARAM)nmInternet.hdr.idFrom,(LPARAM)&nmInternet);
		return ;
}

//#endif


// **************************************************
// 声明：static void SMIE_InsertUrlList(LPHTMLVIEW lpHtmlView, LPTSTR lpUrl,LPTSTR lpLocateFile)
// 参数：
// 	IN lpHtmlView -- 网页视结构
// 	IN lpUrl -- 要插入的网页地址
// 	IN lpLocateFile -- 指定网页的数据文件
// 
// 返回值：无
// 功能描述：插入一个已经打开的网页到列表。
// 引用: 
// **************************************************
static void SMIE_InsertUrlList(LPHTMLVIEW lpHtmlView, LPTSTR lpUrl,LPTSTR lpLocateFile)
{
	LPURLLIST lpUrlList;

		lpUrlList=(LPURLLIST)malloc(sizeof(URLLIST));  // 分配一个列表结点结构
		if (lpUrlList == NULL)
			return ;// 分配失败
		
		// 设置数据
		lpUrlList->next=NULL;
		lpUrlList->prev=NULL;
//		lpUrlList->lpUrl=SMIE_BufferAssignTChar(lpUrl);
		lpUrlList->lpUrl=BufferAssignTChar(lpUrl);
//		lpUrlList->lpLocateFile=SMIE_BufferAssignTChar(lpLocateFile);
		lpUrlList->lpLocateFile=BufferAssignTChar(lpLocateFile);
		// 插入结点
		if (lpHtmlView->lpUrlHead==NULL)
		{
			lpHtmlView->lpUrlHead=lpHtmlView->lpCurUrl=lpUrlList;
		}
		else
		{
			SMIE_DeleteUrlList(lpHtmlView->lpCurUrl->next);  // 删除当前结点后面的结点
			// 把当前结点插到最后
			lpHtmlView->lpCurUrl->next=lpUrlList;
			lpUrlList->prev=lpHtmlView->lpCurUrl;
			lpHtmlView->lpCurUrl=lpUrlList;
		}
}
// **************************************************
// 声明：static void SMIE_DeleteUrlList(LPURLLIST lpUrlList)
// 参数：
// 	IN lpUrlList -- 要从列表中删除的网页
// 
// 返回值：无
// 功能描述：从列表中删除一个指定的网页。
// 引用: 
// **************************************************
static void SMIE_DeleteUrlList(LPURLLIST lpUrlList)
{
	LPURLLIST lpCur,lpNext;
	if (lpUrlList==NULL)
		return;
	lpCur=lpUrlList;
	while(lpCur)
	{
		lpNext=lpCur->next;  // 得到下一个结点
		// 释放结点的缓存
		if (lpCur->lpUrl)
			free(lpCur->lpUrl);  
		if (lpCur->lpLocateFile)
			free(lpCur->lpLocateFile);
		free(lpCur);
		lpCur=lpNext;  // 指向下一个结点
	}
}

//#ifdef INTERNET_SUPPORT
//HINTERNET  SMIE_hInternetSiteInit=NULL;
/*
typedef struct DownLoadItem{
	HINTERNET   hCurrentUrl;
	LPTSTR lpurl;
	struct DownLoadItem *next;
}DOWNLOADITEM,LPDOWNLOADITEM;
*/
//LPDOWNLOADITEM lpDownLoadList=NULL;



// **************************************************
// 声明：void SMIE_SetCurPath(HWND hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 要设置的指定文件
// 
// 返回值：无
// 功能描述：将指定文件所在目录设置为当前目录。
// 引用: 
// **************************************************
void SMIE_SetCurPath(HWND hWnd,LPTSTR lpUrl)
{
	char drive[MAX_PATH+8],dir[MAX_PATH];
	LPHTMLVIEW lpHtmlView;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
	if (lpHtmlView==NULL)
		return ;

//	GetCurrentDirectory(MAX_PATH+8,drive);
	_splitpath(lpUrl,drive,dir,NULL,NULL);   // 分离路径
//	strcat(drive,dir);
	if (strlen(dir))
	{
		if (lpHtmlView->lpCurPath)
		{  // 释放当前路径的缓存
			free(lpHtmlView->lpCurPath);
			lpHtmlView->lpCurPath=NULL;
		}
//		lpHtmlView->lpCurPath=SMIE_BufferAssignTChar(dir);
		lpHtmlView->lpCurPath=BufferAssignTChar(dir);  // 重新设置当前路径
	}
//	if(strlen(drive))
//		SetCurrentDirectory(drive);
}

// **************************************************
// 声明：static void SMIE_DisplayMessage(UINT uMsg)
// 参数：
// 	IN uMsg -- 要显示的标号
// 
// 返回值：无
// 功能描述：显示一个指定的标号。
// 引用: 
// **************************************************
static void SMIE_DisplayMessage(UINT uMsg)
{
	HDC hdc;
	char buffer[128];

		hdc=GetDC(NULL);  // 得到HDC
		sprintf(buffer,"The Msg is %d  %08X",uMsg,uMsg);  // 得到字串
		TextOut(hdc,10,10,buffer,strlen(buffer));  // 输出字串
		ReleaseDC(NULL,hdc);  // 释放HDC
}

// **************************************************
// 声明：static LPTSTR SMIE_GetFullUrl(HWND hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 指定的地址
// 
// 返回值：返回完睁的地址
// 功能描述：得到指定的地址的完整地址。
// 引用: 
// **************************************************
static LPTSTR SMIE_GetFullUrl(HWND hWnd,LPTSTR lpUrl)
{
	char drive[MAX_PATH+8],dir[MAX_PATH];
	UINT iNewUrlLen;
	LPTSTR lpNewUrl;
	LPHTMLVIEW lpHtmlView;


	if (strlen(lpUrl) == 0)
		return BufferAssignTChar("");

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
	if (lpHtmlView==NULL)
		return 0;
	
	if (SMIE_IsLocateFile(hWnd,lpUrl))
	{  // 是本地文件

	//	GetCurrentDirectory(MAX_PATH+8,drive);
		RETAILMSG(1, ("SMIE_GetFullUrl 11111 (%s)", lpUrl));
		_splitpath(lpUrl,drive,dir,NULL,NULL);  // 分离路径
		RETAILMSG(1, ("SMIE_GetFullUrl 222 (dir:%s, ulr:%s) \r\n", dir, lpUrl));
		if (dir[0]=='\\' || dir[0] == '/')
		{  // 是全路径
			// will the full directory
			if (lpHtmlView->lpCurPath)
			{  // 删除原来的当前路径
				free(lpHtmlView->lpCurPath);
				lpHtmlView->lpCurPath=NULL;
			}
//			lpHtmlView->lpCurPath=SMIE_BufferAssignTChar(dir);
			RETAILMSG(1, ("URL:(%s) \r\n", lpUrl));
			lpHtmlView->lpCurPath=BufferAssignTChar(dir);  // 设置新的当前路径
			
//			return SMIE_BufferAssignTChar(lpUrl);
			return BufferAssignTChar(lpUrl);  // 返回当前的URL
		}
		else
		{  // 不是全路径文件
			// the Relative directory
	/*		if (lpHtmlView->lpCurPath)
			{
				free(lpHtmlView->lpCurPath);
				lpHtmlView->lpCurPath=NULL;
			}*/
	//		iNewpathLen=strlen(lpHtmlView->lpCurPath)+strlen(dir)+2;
			RETAILMSG(1, ("SMIE_GetFullUrl 333333333333"));
			iNewUrlLen=strlen(lpHtmlView->lpCurPath)+strlen(lpUrl)+2;  // 得到最大的路径长度
        
			lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));  // 分配路径缓存

			if (lpNewUrl==NULL)
			{  // 分配失败
				return NULL;
			}
			strcpy(lpNewUrl,lpHtmlView->lpCurPath);  // 复制当前路径
			strcat(lpNewUrl,lpUrl); // 添加当前路径

	/*		lpNewPath=(LPTSTR)realloc(lpHtmlView->lpCurPath,iNewpathLen*sizeof(TCHAR));
			if (lpNewPath!=NULL)
			{
				lpHtmlView->lpCurPath=lpNewPath;
				strcat(lpHtmlView->lpCurPath,dir);
			}
	*/
			return lpNewUrl;
		}
	}
	else
	{  // 不是本地文件
		RETAILMSG(1, ("SMIE_GetFullUrl 444444444"));
				if (strnicmp(lpUrl,INTERNET_HEAD,5)==0)
				{ // Is "http:"
					lpUrl += 5;
				}
				while(1)
				{ // 删除最前面的根
					if (*lpUrl == '\\' || *lpUrl == '/')
						lpUrl++;
					else
						break;
				}
				iNewUrlLen=strlen(INTERNET_HEAD)+strlen(lpUrl)+2;  // 得到最大的URL长度
        
				lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));  // 分配缓存

				if (lpNewUrl==NULL)
				{  // 分配失败
					return NULL;
				}
				strcpy(lpNewUrl,INTERNET_HEAD);  // COPY “http://” to URL
				strcat(lpNewUrl,lpUrl);  // COPY 当前URL 到新的 URL

				return lpNewUrl;
	}
//	strcat(drive,dir);
//	if(strlen(drive))
//		SetCurrentDirectory(drive);
}

// **************************************************
// 声明：LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpImage -- 要下载的图象
// 
// 返回值：返回图象的完整地址。
// 功能描述：得到要下载图象的完整地址。
// 引用: 
// **************************************************
LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage)
{
	char drive[MAX_PATH+8],dir[MAX_PATH];
//	UINT iNewUrlLen;
	LPTSTR lpNewUrl,lpFileName;//,lpTemp;
	LPHTMLVIEW lpHtmlView;
	int i=0,iFileNameLength=0;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
	if (lpHtmlView==NULL)
		return 0;
/*
	lpTemp=lpImage;
	while(*lpTemp)
	{
		if (*lpTemp=='/')
			*lpTemp='\\';
		lpTemp++;
	}
*/
	_splitpath(lpImage,drive,dir,NULL,NULL);  // 分离路径
	if (dir[0]=='\\')
	{  // 是全路径
//		return SMIE_BufferAssignTChar(lpImage);
		return BufferAssignTChar(lpImage);  // 直接复制
	}
	else
	{
		// the Relative directory
// !!! Modified By Jami chen in 2003.09.25
		/*
		iNewUrlLen=strlen(lpHtmlView->lpCurPath)+strlen(lpImage)+2;
        
		lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));

		if (lpNewUrl==NULL)
		{
			return NULL;
		}
		strcpy(lpNewUrl,lpHtmlView->lpCurPath);
//		strcat(lpNewUrl,lpImage);
		i=strlen(lpNewUrl);
		while (*lpImage!=0)
		{
			while(*lpImage=='/') lpImage++;  // jump //
			if (lpNewUrl[i-1]!='\\')
				lpNewUrl[i++]='\\';
			// exist directory
			iFileNameLength=0;
			while(*lpImage!=0&&*lpImage!='/')
			{
				if (*lpImage=='.')
					iFileNameLength=0;
				if (iFileNameLength>=MAX_FILENAME)
				{  // give up the tail of the file or directory name
					lpImage++;
				}
				else
				{	// save current file or directory name
					iFileNameLength++;
					lpNewUrl[i++]=*lpImage++;
				}
			}
		}

		lpNewUrl[i]=0;
		return lpNewUrl;
		*/
// !!! Delete By Jami chen in 2004.09.09
//			lpNewUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpImage)+4);  // 分配内存
//			if (lpNewUrl==NULL)
//				return FALSE;
// !!! Delete End By Jami chen in 2004.09.09
			if (strnicmp(lpImage,INTERNET_HEAD,5))
			{  //是相对路径
				if (SMIE_IsLocateFile(hWnd,lpHtmlView->lpCurUrl->lpUrl))
				{  // 是本地文件
					lpNewUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpImage)+4);  // 分配内存
					if (lpNewUrl==NULL)
						return FALSE;
					strcpy(lpNewUrl,lpHtmlView->lpCurPath);  // 先复制当前路径
					strcat(lpNewUrl,lpImage);  // 复制相对路径
					return lpNewUrl;
				}
				else
				{  // 是网页
					/*
					strcpy(lpNewUrl,lpHtmlView->lpCurUrl->lpUrl);  // 复制当前网页
					i=strlen(lpNewUrl);
					while(lpNewUrl[i]!='/') i--;
					lpNewUrl[i+1]=0;  // 添加当前位置
				//	strcat(lpFullFileName,"/");
					strcat(lpNewUrl,lpImage);
					*/
					lpNewUrl = MakeInternetRelativePath(lpHtmlView->lpCurUrl->lpUrl,lpImage);
				}
			}
			else
			{  // 是绝对路径
// !!! modified By Jami chen in 2004.09.09
//				strcpy(lpNewUrl,lpImage);
				lpNewUrl = BufferAssignTChar(lpImage);
// !!! modified End By Jami chen in 2004.09.09
			}
			if (lpNewUrl==NULL)
				return NULL;
			lpFileName = GetFileNameFromUrl(hWnd,lpNewUrl);  // 得到文件名
			free(lpNewUrl);
			return lpFileName;  // 返回文件名
	}
}
// **************************************************
// 声明：static BOOL SMIE_ApartMark(LPTSTR lpUrl,LPTSTR *lpPureUrl,LPTSTR *lpMark)
// 参数：
// 	IN lpUrl -- 指定的网页
// 	OUT lpPureUrl -- 分离了标号的网页地址
// 	OUT lpMark -- 分离出来的标号
// 
// 返回值：返回分离了标号的网页地址
// 功能描述：将网页地址与标号分离。
// 引用: 
// **************************************************
static BOOL SMIE_ApartMark(LPTSTR lpUrl,LPTSTR *lpPureUrl,LPTSTR *lpMark)
{
	LPTSTR lpMarkPos;
	if (lpUrl==NULL)
	{  // URL 为空
		*lpPureUrl=NULL;
		*lpMark=NULL;
		return FALSE;
	}
	// 查找分隔符“#”
	lpMarkPos=lpUrl;
	while(*lpMarkPos)
	{
		if (*lpMarkPos=='#')
			break;
		lpMarkPos++;
	}
	// 得到标号
	*lpMark=NULL;
	if (*lpMarkPos)
	{
		*lpMark=(LPTSTR)malloc(strlen(lpMarkPos+1)+1);  // 分配内存
		if (*lpMark)
		{
			strcpy(*lpMark,(lpMarkPos+1));
		}
	}
	// URL删除标号
	*lpPureUrl=(LPTSTR)malloc(lpMarkPos-lpUrl+1);  // 分配内存
	if (*lpPureUrl)
	{
		strncpy(*lpPureUrl,lpUrl,(lpMarkPos-lpUrl));
		*(*lpPureUrl+(lpMarkPos-lpUrl))=0;
	}
	return TRUE;
}

// **************************************************
// 声明：BOOL SMIE_IsLocateFile(HWND hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 指定的网页地址
// 
// 返回值：是本地文件返回TRUE，否则返回FALSE。
// 功能描述：指定的网页地址是否是一个本地文件
// 引用: 
// **************************************************
BOOL SMIE_IsLocateFile(HWND hWnd,LPTSTR lpUrl)
{
	HANDLE in_file;

	if (*lpUrl == '\\' || *lpUrl == '/')
	{
		// 是根目录
		in_file=CreateFile(lpUrl,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if (in_file==INVALID_HANDLE_VALUE)
			return FALSE;  // 不能打开文件，本地不存在此文件
		CloseHandle(in_file);  // 关闭文件句柄
		return TRUE;  // 是本地文件
	}
	else
	{
		LPTSTR lpNewUrl;
		DWORD iNewUrlLen;
		LPHTMLVIEW lpHtmlView;

		if (strnicmp(lpUrl,INTERNET_HEAD,5)==0)
		{  
			// 当前文件是一个网络路径
			return FALSE;
		}

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)  
			return FALSE;

		// 首先要判断当前打开的文件是否是一个本地文件，如果当前打开的
		// 不是一个本地文件，则其对应的文件也不应该是一个本地文件
		if (lpHtmlView->lpCurUrl)
		{
			if (lpHtmlView->lpCurUrl->lpUrl)
			{
				if (strcmp(lpUrl, lpHtmlView->lpCurUrl->lpUrl) != 0)
				{
					// 不是当前打开的网页
					if (SMIE_IsLocateFile(hWnd,lpHtmlView->lpCurUrl->lpUrl) == FALSE)
					{
						return FALSE;
					}
				}
			}
		}
		// 得到全路径
		iNewUrlLen=strlen(lpHtmlView->lpCurPath)+strlen(lpUrl)+2;
    
		lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));  // 分配内存

		if (lpNewUrl==NULL)
		{
			return NULL;
		}
		strcpy(lpNewUrl,lpHtmlView->lpCurPath);
		strcat(lpNewUrl,lpUrl);

		in_file = CreateFile(lpNewUrl,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);  // 打开文件
		free(lpNewUrl);
		if (in_file==INVALID_HANDLE_VALUE)
			return FALSE;  // 打开文件失败，不是本地文件
		CloseHandle(in_file);  // 关闭文件句柄
		return TRUE;  // 是本地文件
	}
	return TRUE;
}

// **************************************************
// 声明：LPTSTR GetFileNameFromUrl(HWND hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 指定的网页地址
// 
// 返回值：返回得到的文件名的地址
// 功能描述：根据网页地址得到要保存的文件的文件名。
// 引用: 
// **************************************************
LPTSTR GetFileNameFromUrl(HWND hWnd,LPTSTR lpUrl)
{
	LPTSTR lpFileName;
	LPHTMLVIEW lpHtmlView;
	TCHAR FileName[MAX_PATH];
	int i=0,j ,iFileNameLength = 0;
	char	cNotAllowed[]={ '/',  ':',  '*',  '?',  '\"',  '<',  '>', '|'};

	if (lpUrl==NULL)
		return NULL;
	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
	if (lpHtmlView==NULL)
		return 0;
	strcpy(FileName,"\\临时");  // 得到下载根目录
	i=strlen(FileName);
//  http://www.yahoo.com/ --> yahoo
//  http://Jami.mlg.com/  --> Jami    MAXLEN is 8 bytes
	if (strnicmp(lpUrl,INTERNET_HEAD,strlen(INTERNET_HEAD))==0)
	{  // jump http:
		lpUrl+=strlen(INTERNET_HEAD);
	}
	while(*lpUrl=='/') lpUrl++;  // jump //
	if (strnicmp(lpUrl,"www",strlen("www"))==0)
	{  // jump http:
		lpUrl+=strlen("www");
	}
	while(*lpUrl=='.') lpUrl++;  // jump //

	FileName[i++]='\\';
	while(*lpUrl!='.'&&*lpUrl!='/')
	{  // 得到目录名
		FileName[i++]=*lpUrl++;
	}

	while(*lpUrl!=0&&*lpUrl!='/')
	{  // 调过其他字符
		*lpUrl++;
	}

	// 得到文件名
	while (*lpUrl!=0)
	{
		while(*lpUrl=='/') lpUrl++;  // jump //
		FileName[i++]='\\';
		// exist directory
		iFileNameLength=0;
		while(*lpUrl!=0&&*lpUrl!='/')
		{
			if (*lpUrl=='.')
				iFileNameLength=0;
			if (iFileNameLength>=MAX_FILENAME)
			{  // give up the tail of the file or directory name
				lpUrl++;
			}
			else
			{	// save current file or directory name
				iFileNameLength++;
				FileName[i++]=*lpUrl++;
			}
		}
	}
	FileName[i]=0;
	if (iFileNameLength==0)
	{  // 没有得到文件名
		if (FileName[i-1] != '\\')
		{  // 添加路径符号
			FileName[i++] = '\\';
			FileName[i] = 0;
		}
		strcat(FileName,"index.htm");  // 添加默认文件名
	}
	i = 0;
//	iFileNameLength = 
	while( 1) 
	{

		if (FileName[i] == 0)
			break;

		if( FileName[i] < 127 )
		{
			for( j=0; j< sizeof( cNotAllowed); j++ )
			{   // 将文件名中不可以出现的字符用“_”代替
				if( FileName[i] == cNotAllowed[j] )
					FileName[i] = '_';
			}
		}
		i++;
	}
//	lpFileName=SMIE_BufferAssignTChar(FileName);
	lpFileName=BufferAssignTChar(FileName);  // 复制文件名


	return lpFileName;  // 返回文件名
}

// **************************************************
// 声明：BOOL CreateMultiDirectory(  LPCTSTR lpPathName,   LPSECURITY_ATTRIBUTES lpSecurityAttributes  )
// 参数：
// 	IN lpPathName -- 要创建目录的前路径文件名
// 	IN lpSecurityAttributes -- 要创建的目录的属性
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：创建指定文件所需要的路径。
// 引用: 
// **************************************************
BOOL CreateMultiDirectory(
  LPCTSTR lpPathName,                         // pointer to directory path string
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // pointer to security descriptor
)
{
	LPTSTR lpDirectory;
	int i=0,size;


		if (*lpPathName!='\\')
			return FALSE;  // the directory must be from root directory('\') start
		 
		size=strlen(lpPathName)+1;
		lpDirectory=(LPTSTR)malloc(size);  // 分配路径缓存
		if (lpDirectory==NULL)
		{
			return FALSE;
		}
		memset(lpDirectory,0,size);  // 初始化缓存
		lpDirectory[i++]=*lpPathName++;
		while(*lpPathName)
		{
			if (*lpPathName=='\\')
			{// a directory complete ,mult create this directory
				CreateDirectory(lpDirectory,NULL);
			}
			lpDirectory[i++]=*lpPathName++;
		}
		free(lpDirectory);
		return TRUE;
}



// !!! Add By Jami chen in 2003.09.12
// **************************************************
// 声明：static LRESULT DoGetFileType(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：返回当前打开文件的类型
// 功能描述：得到当前打开文件的类型，处理HM_GETFILETYPE消息。
// 引用: 
// **************************************************
static LRESULT DoGetFileType(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // 得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return -1;

		if (lpHtmlView->lpCurFile == NULL)  // 没有本地文件
			return -1;
		
		if (IsImageFile(lpHtmlView->lpCurFile))  // 是否是图形文件
			return IMAGEFILE;

		return HTMLFILE;
}
// !!! Add End By Jami chen in 2003.09.12

// !!! Add By Jami chen in 2003.09.13
// **************************************************
// 声明：static LRESULT DoGetLocateFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 缓存大小
// 	IN lParam -- 存放文件名的缓存
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前打开网页所对应的本地文件的文件名，处理HM_GETLOCATEFILE消息。
// 引用: 
// **************************************************
static LRESULT DoGetLocateFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	LPTSTR lpFileName;
	int iMaxLength;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		if (lpHtmlView->lpCurFile == NULL)  // 没有本地文件
			return FALSE;

		iMaxLength = (int)wParam;
		lpFileName =(LPTSTR)lParam;
		if (iMaxLength <= (int)strlen(lpHtmlView->lpCurFile) || lpFileName == NULL)
		{  // 缓存不足
			return FALSE;
		}

		strcpy(lpFileName,lpHtmlView->lpCurFile);  // 拷贝文件名
		return TRUE;
}
// !!! Add End By Jami chen in 2003.09.13

// !!! Add By Jami chen in 2003.09.27
// **************************************************
// 声明：static LRESULT DoStop(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：停止当前正在打开的网页，处理HM_STOP消息。
// 引用: 
// **************************************************
static LRESULT DoStop(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	LPDOWNLOADITEM lpDownLoadItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

//		RETAILMSG(1,("########### Stop Current Connect... \r\n"));
		lpHtmlView->bStop = TRUE;
//		RETAILMSG(1,("########### Delete Download List  \r\n"));
		DeleteDownList(hWnd);  // 删除等待下载列表

//		RETAILMSG(1,("########### Stop Running List  \r\n"));
		CloseRunningList(hWnd);  // 关闭正在下载列表
//		RETAILMSG(1,("#########################################################\r\n"));
		return FALSE;
}
// !!! Add End By Jami chen in 2003.09.27


// **************************************************
// 声明：static void WaitExitThread(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：等待线程退出
// 引用: 
// **************************************************
static void WaitExitThread(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return ;
//		RETAILMSG(1,("########### Will Exit All Thread... \r\n"));
//		lpHtmlView->bExit = TRUE;
		while(lpHtmlView->bExit)
		{  // 等待退出
			Sleep(100);
		}
//		RETAILMSG(1,("########### Had success Exit All Thread\r\n"));
}

// **************************************************
// 声明：static LRESULT SMIE_InterentEvnetHandler(HWND hWnd, WPARAM  wParam, LPARAM lParam  )
// 参数：
// 	IN  hWnd -- 窗口句柄
// 	IN  wParam -- 事件名称
// 	IN  lParam -- 保留
// 
// 返回值：无
// 功能描述：Internet事件处理函数
// 引用: 
// **************************************************
static LRESULT SMIE_InterentEvnetHandler(HWND hWnd, WPARAM  wParam, LPARAM lParam  )
{
	switch( wParam )
	{
	case INTERNET_STATUS_RESOLVING_NAME        :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "正在寻找网址..." );
		break;
	case INTERNET_STATUS_NAME_RESOLVED         :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "已经找到网址!" );
		break;

	case INTERNET_STATUS_CONNECTING_TO_SERVER  :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "正在连接网址..." );
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER   :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "已经连接到网址!" );
		break;

	case INTERNET_STATUS_SENDING_REQUEST       :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "正在发送请求..." );
		break;
	case INTERNET_STATUS_REQUEST_SENT          :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "发送请求完成!" );
		break;

	case INTERNET_STATUS_RECEIVING_RESPONSE    :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "正在接收应答..." );
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED     :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "应答接收完成!" );
		break;

	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED :
		break;
	case INTERNET_STATUS_PREFETCH              :
		break;

	case INTERNET_STATUS_CLOSING_CONNECTION    :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "正在关闭连接..." );
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED     :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "连接已经关闭!" );
		break;

	case INTERNET_STATUS_HANDLE_CREATED        :
		break;
	case INTERNET_STATUS_HANDLE_CLOSING        :
		break;

	case INTERNET_STATUS_REQUEST_COMPLETE      :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "请求已经完成!" );
		break;
	case INTERNET_STATUS_REDIRECT              :
		break;
	case INTERNET_STATUS_INTERMEDIATE_RESPONSE :
		break;
	case INTERNET_STATUS_STATE_CHANGE          :
		break;

	default :
		break;
	}

	return 0;
}

// **************************************************
// 声明：static LRESULT DoReAdjustSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：重新调整当前文件的位置，处理HM_READJUSTSIZE消息。
// 引用: 
// **************************************************
static LRESULT DoReAdjustSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	POINT ptOrg;
//	HDC hdc;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;
		
		if (lpHtmlView->hHtml)
		{
// !!! modified By Jami chen in 2004.08.19
//			hdc=GetDC(hWnd);
//			if (lpHtmlView->iFontSize == FONT_SMALLLER)
//			{// 是小字体
//				SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//			}
//			SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // 计算大小
//			ReleaseDC(hWnd,hdc);
			if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // 是否是图形文件, 如果是图象文件，则不需要重新计算尺寸
				SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // 计算需要的尺寸
// !!! modified end By Jami chen in 2004.08.19
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条
			SetCurrentScrollPos(hWnd,SB_HORZ);
			SetCurrentScrollPos(hWnd,SB_VERT);
		}

		// 重绘窗口
		ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
		ptOrg.y=lpHtmlView->y_Org;
	 	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
		InvalidateRect(hWnd,NULL,TRUE);

		return TRUE;
}


// **************************************************
// 声明：static LRESULT DoSetFontSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- (int)iFontSize; // 当前要设置的字体大小
// 	IN lParam -- (BOOL)bShow;  // 是否要重新显示
// 
// 返回值：无
// 功能描述：设置显示字体的大小，处理HM_SETFONTSIZE消息。
// 引用: 
// **************************************************
static LRESULT DoSetFontSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
//	HDC hdc;
	int iFontSize;
	BOOL bShow;
	TEXTMETRIC TextMetric;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		iFontSize = (int)wParam;
		bShow = (BOOL)lParam;

		if ((iFontSize != FONT_BIGGER) && (iFontSize != FONT_SMALLLER))
		{
			// 不认识的字体
			return FALSE;
		}

		if (iFontSize == lpHtmlView->iFontSize)
		{
			// 字体没有发生改变
			return TRUE;
		}

		lpHtmlView->iFontSize = iFontSize; // 设置新的字体
// !!! modified By Jami chen in 2004.08.19
//		hdc=GetDC(hWnd);
		if (lpHtmlView->iFontSize == FONT_SMALLLER)
		{// 是小字体
//			SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
			SelectObject(lpHtmlView->hMemoryDC, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
		}
		else
		{  // 是大字体
			SelectObject(lpHtmlView->hMemoryDC, GetStockObject( SYSTEM_FONT_CHINESE24X24 ) );
		}

		// 重新得到文本高度
		GetTextMetrics(lpHtmlView->hMemoryDC,&TextMetric);
		lpHtmlView->iTextHeight = TextMetric.tmHeight;


		if (lpHtmlView->hHtml)
		{ // 当前有打开的网页，则需要重新显示
			// 默认为大字体
			if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // 是否是图形文件, 如果是图象文件，则不需要重新计算尺寸
				SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // 计算大小

			lpHtmlView->x_Org=0;  // 显示原点回到(0,0)
			lpHtmlView->y_Org=0;
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条

			if (bShow == TRUE)
			{
// !!! modified By Jami chen in 2004.08.19
//				InvalidateRect(hWnd,NULL,TRUE);
				ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
				ptOrg.y=lpHtmlView->y_Org;
	 			SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
				InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
			}
		}
//		ReleaseDC(hWnd,hdc);

		return TRUE;
}



// **************************************************
// 声明：static LRESULT DoSetDisplayMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- (int)iDisplayMode; // 当前要设置的显示模式
// 	IN lParam -- (BOOL)bShow;  // 是否要重新显示
// 
// 返回值：无
// 功能描述：设置显示字体的大小，处理HM_SETFONTSIZE消息。
// 引用: 
// **************************************************
static LRESULT DoSetDisplayMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
//	HDC hdc;
	int iDisplayMode;
	BOOL bShow;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		iDisplayMode = (int)wParam;
		bShow = (BOOL)lParam;

		if ((iDisplayMode != DISPLAY_TEXTIMAGE) && (iDisplayMode != DISPLAY_ONLYTEXT))
		{
			// 不认识的显示模式
			return FALSE;
		}

		if (iDisplayMode == lpHtmlView->iDisplayMode)
		{
			// 字体没有发生改变
			return TRUE;
		}

		lpHtmlView->iDisplayMode = iDisplayMode; // 设置新的显示模式
		
		if (lpHtmlView->hHtml)
		{ // 当前有打开的网页，则需要重新显示
// !!! modified By Jami chen in 2004.08.19
//			hdc=GetDC(hWnd);
//			if (lpHtmlView->iFontSize == FONT_SMALLLER)
//			{// 是小字体
//				SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//			}
//			SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // 计算大小
//			ReleaseDC(hWnd,hdc);
			if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // 是否是图形文件, 如果是图象文件，则不需要重新计算尺寸
				SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // 计算大小
// !!! modified By Jami chen in 2004.08.19

			lpHtmlView->x_Org=0;  // 显示原点回到(0,0)
			lpHtmlView->y_Org=0;
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条

			if (bShow == TRUE)
			{
// !!! modified By Jami chen in 2004.08.19
//				InvalidateRect(hWnd,NULL,TRUE);
				ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
				ptOrg.y=lpHtmlView->y_Org;
	 			SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
				InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
			}
		}

		return TRUE;
}



// ********************************************************************
// 声明：static HDC CreateMemoryDC(HWND hWnd)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：创建一个内存设备句柄
// 引用: 
// ********************************************************************
static HDC CreateMemoryDC(HWND hWnd)
{
	HDC hdc;
	RECT rect;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return NULL;
		
		GetClientRect(hWnd,&rect); // 得到客户矩形
		hdc = GetDC(hWnd); // 得到设备句柄
		lpHtmlView->hMemoryDC=CreateCompatibleDC(hdc); // 创建兼容内存设备句柄
		if (lpHtmlView->hMemoryDC == NULL)
			return NULL;
		lpHtmlView->hMemoryBitmap=CreateCompatibleBitmap(hdc,(rect.right-rect.left),(rect.bottom-rect.top)); // 创建兼容位图
		ReleaseDC(hWnd,hdc); /// 释放设备句柄
		if (lpHtmlView->hMemoryBitmap == NULL)
		{ // 创建兼容位图失败
			DeleteDC(lpHtmlView->hMemoryDC);
			lpHtmlView->hMemoryDC= NULL;
			return NULL;
		}
		SelectObject(lpHtmlView->hMemoryDC,lpHtmlView->hMemoryBitmap); // 选择位图到内存设备句柄

		FillRect(lpHtmlView->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // 画背景
		return lpHtmlView->hMemoryDC; // 返回成功创建的内存DC
}

// ********************************************************************
// 声明：static void DrawToScreen(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
//  IN hdc	- 设备句柄
// 返回值：
//	
// 功能描述：创建一个内存设备句柄
// 引用: 
// ********************************************************************
static void DrawToScreen(HWND hWnd,HDC hdc)
{
	LPHTMLVIEW lpHtmlView;
	RECT rect;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return ;

		GetClientRect(hWnd,&rect);
		BitBlt(hdc,0,0,rect.right,rect.bottom,lpHtmlView->hMemoryDC,0,0,SRCCOPY);
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
	LPHTMLVIEW lpHtmlView;
	HBITMAP hBitmap;
	RECT rect;
	HDC hdc;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return 0;

		GetClientRect(hWnd,&rect);
		// 窗口大小发生改变，内存DC的位图也要改变
		hdc = GetDC(hWnd);
		hBitmap=CreateCompatibleBitmap(hdc,(rect.right-rect.left),(rect.bottom-rect.top)); // 创建兼容位图
		ReleaseDC(hWnd,hdc); /// 释放设备句柄
		if (hBitmap== NULL)
		{ // 创建兼容位图失败
			return 0;
		}
		lpHtmlView->hMemoryBitmap = hBitmap; 
		hBitmap = SelectObject(lpHtmlView->hMemoryDC,hBitmap); // 选择新位图到内存设备句柄
		if (hBitmap)
			DeleteObject(hBitmap);

		SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // 重设滚动条
		SetCurrentScrollPos(hWnd,SB_HORZ);
		SetCurrentScrollPos(hWnd,SB_VERT);
		// 重绘窗口
		ptOrg.x=lpHtmlView->x_Org;  // 得到VIEW的原点
		ptOrg.y=lpHtmlView->y_Org;
	 	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // 绘制HTML
		InvalidateRect(hWnd,NULL,TRUE);
		
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

// ********************************************************************
// 声明：static void SetCurrentScrollPos(HWND hWnd,int fnBar)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
// 返回值：
//	
// 功能描述：设置当前的滚动条位置
// 引用: 
// ********************************************************************
static void SetCurrentScrollPos(HWND hWnd,int fnBar)
{
	SCROLLINFO ScrollInfo;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return ;

		ScrollInfo.cbSize=sizeof(SCROLLINFO);
		ScrollInfo.fMask=SIF_POS;

		if (fnBar == SB_VERT)
		{
			ScrollInfo.nPos=(lpHtmlView->y_Org+STEPHEIGHT-1)/STEPHEIGHT;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);  // 设置垂直滚动条的属性
		}
		else
		{
			ScrollInfo.nPos=(lpHtmlView->x_Org+STEPWIDTH-1)/STEPWIDTH;
			SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);  // 设置水平滚动条属性
		}
}

// !!! Add By Jami chen in 2004.09.09
// ********************************************************************
// 声明：LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl)
// 参数：
// 	IN lpOpenUrl - 当前已经打开的INTERNET URL
//  IN lpRelativeUrl -- 当前要打开的相对地址
// 返回值：
//	
// 功能描述：设置当前的滚动条位置
// 引用: 
// ********************************************************************
LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl)
{
	LPTSTR lpNewUrl;
	int i;
	int iPathlenght,iCurrentPos;
	int iProxyNameLenght; // 网页的地址长度，该网页相当于文件的根目录
		
		if ( lpOpenUrl == NULL || lpRelativeUrl == NULL)
			return NULL;

		iPathlenght = strlen(lpOpenUrl)+strlen(lpRelativeUrl)+4;
		lpNewUrl=(LPTSTR)malloc(iPathlenght);  // 分配内存
		if (lpNewUrl==NULL)
			return NULL;

		memset(lpNewUrl,0,iPathlenght);

		iProxyNameLenght = strlen(INTERNET_HEAD); // 从http://的后面开始查询

		// 得到当前的网址的长度
		while( 1 )
		{
			if (lpOpenUrl[iProxyNameLenght] == 0 ) 
			{
				break;
			}
			if (lpOpenUrl[iProxyNameLenght] == '/' ) 
			{
				break;
			}
			iProxyNameLenght ++;
		}
		// iProxyNameLenght 不包含最后的/

		// Add By Jami chen in 2004.12.27
		if (lpRelativeUrl[0] == '/')
		{
			// 取网页根目录下的内容
			strncpy(lpNewUrl,lpOpenUrl,iProxyNameLenght);  // 复制当前网页
			lpNewUrl[iProxyNameLenght] = 0;
			strcat(lpNewUrl,lpRelativeUrl);
			return lpNewUrl;
		}
		// Add End Jami chen in 2004.12.27

		iPathlenght = strlen(lpOpenUrl); // 得到当前打开的网页的长度
		while( lpOpenUrl[iPathlenght] != '/' ) 
		{
			iPathlenght--;
			if (iPathlenght < iProxyNameLenght)
				break;
		}

		if (iPathlenght < iProxyNameLenght)  
		{
			// 当前只是一个网址
			strncpy(lpNewUrl,lpOpenUrl,iProxyNameLenght);  // 复制当前网页
			strcat(lpNewUrl,"/");
		}
		else
		{
			i = iPathlenght;
			// 查找有没有文件分隔符
			while(1)
			{
				if (( lpOpenUrl[i] == 0))
				{
					// 没有找到文件分隔符，当前打开的是一个目录
					iPathlenght = i;
					strcpy(lpNewUrl,lpOpenUrl);  // 复制当前网页
					if (( lpOpenUrl[i-1] != '/'))
 						strcat(lpNewUrl,"/"); //  最后没有目录分隔符
					break;
				}
				if (lpOpenUrl[i] == '.' ) 
				{
					// 有文件分隔符，当前当开的网址是一个文件
					strncpy(lpNewUrl,lpOpenUrl,iPathlenght);  // 复制当前网页
					strcat(lpNewUrl,"/");
					break;
				}
				i++;
			}
		}
		// 最后添加
		while(1)
		{
			if (strncmp(lpRelativeUrl,"..\\",3) == 0  || strncmp(lpRelativeUrl,"../",3) == 0)
			{
				// 需要定位到上一级目录
				lpRelativeUrl += 3;

				iCurrentPos = strlen(lpNewUrl) -1;
				iCurrentPos --; // 最后一个是目录分隔符，现在要找前一个目录分隔符

				while( lpNewUrl[iCurrentPos] != '/' ) 
				{
					iCurrentPos--;
					if (iCurrentPos < iProxyNameLenght)
						break;
				}
				if (iCurrentPos < iProxyNameLenght)
				{
					// 没有找到上一级目录
					free(lpNewUrl);
					return NULL;
				}
				lpNewUrl[iCurrentPos + 1] = 0;
				continue;
			}
			break;
		}
		strcat(lpNewUrl,lpRelativeUrl);

		return lpNewUrl;
}

// ********************************************************************
// 声明：LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl)
// 参数：
// 	IN lpOpenUrl - 当前已经打开的INTERNET URL
//  IN lpRelativeUrl -- 当前要打开的相对地址
// 返回值：
//	
// 功能描述：判断当前打开的URL是否是一个用户可以打开的网页
// 引用: 
// ********************************************************************
static BOOL IsInternetUrl(LPTSTR lpUrl)
{
//	LPTSTR lpNewUrl;
	int i;
	int iPathlenght;
	int iProxyNameLenght; // 网页的地址长度，该网页相当于文件的根目录
		
		if ( lpUrl == NULL )
			return FALSE;

		iProxyNameLenght = strlen(INTERNET_HEAD); // 从http://的后面开始查询

		// 得到当前的网址的长度
		while( 1 )
		{
			if (lpUrl[iProxyNameLenght] == 0 ) 
			{  // 仅仅是一个网址
				return TRUE;
			}
			if (lpUrl[iProxyNameLenght] == '/' ) 
			{
				break;
			}
			iProxyNameLenght ++;
		}
		// iProxyNameLenght 不包含最后的/

		iPathlenght = strlen(lpUrl); // 得到当前打开的网页的长度
		while( lpUrl[iPathlenght] != '/' ) 
		{
			iPathlenght--;
			if (iPathlenght < iProxyNameLenght)
				break;
		}

		if (iPathlenght < iProxyNameLenght)  
		{
			// 当前只是一个网址
			return TRUE;
		}
		else
		{
			// iPathlenght 是最后一个/
			i = iPathlenght;
			// 查找有没有文件分隔符
			while(1)
			{
				if (( lpUrl[i] == 0))
				{
					// 没有找到文件分隔符，当前打开的是一个目录
					return TRUE;
				}
				if (lpUrl[i] == '.' ) 
				{
					// 有文件分隔符，当前当开的网址是一个文件
					LPTSTR lpExt;

					lpExt = lpUrl + i + 1;
					if (stricmp(lpExt,"htm") == 0 ||
						stricmp(lpExt,"html") == 0 ||
						stricmp(lpExt,"shtml") == 0 ||
						stricmp(lpExt,"asp") == 0 ||
						strnicmp(lpExt,"htm?",4) == 0 ||  //以下内容为打开某个网页并有条件
						strnicmp(lpExt,"html?",5) == 0 ||
						strnicmp(lpExt,"shtml?",6) == 0 ||
						strnicmp(lpExt,"asp?",4) == 0
						)
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
				i++;
			}
		}
		return FALSE;
}
// !!! Add By Jami chen in 2004.09.09
