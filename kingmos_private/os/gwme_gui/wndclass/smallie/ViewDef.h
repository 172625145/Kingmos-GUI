/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef __VIEWDEF_H
#define __VIEWDEF_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分


//#define USE_TIMERTOSHOWIMG
#define TIMEINTERVAL	30  // 每隔50ms查看一次是否需要显示图象

#define INTERNET_HEAD  "http://"
#define MAILTO_HEAD  "mailto:"


#define MAX_FILENAME MAX_PATH
#define INTERNET_SUPPORT
//#undef INTERNET_SUPPORT

//#define LOADANIMATION
#undef LOADANIMATION

#ifdef INTERNET_SUPPORT

#include "internet.h"

#define TEMPFILE  "\\Internet.tmp"
#define INTERNET_EVENT  3

#endif


#define WM_MOUSEWHEEL                   0x020A
//#define THREADNUM		5
#define THREADNUM		2

typedef struct structURLHANDLE{
	HANDLE			hOpenCnn;
	HANDLE			hOpenReq;
}URLHANDLE, *LPURLHANDLE;

#ifdef INTERNET_SUPPORT
typedef struct DownLoadItem{
//	HINTERNET   hUrl;
	LPTSTR		lpUrl;
	LPTSTR		lpFileName;
	HHTMCTL hControl;
	struct DownLoadItem *next;
}DOWNLOADITEM,*LPDOWNLOADITEM;

typedef struct RunningItem{
//	HINTERNET   hUrl;
	LPURLHANDLE   hUrl;
//	LPTSTR		lpUrl;
	struct RunningItem *next;
}RUNNINGITEM,*LPRUNNINGITEM;
#endif

// !!! Add By Jami chen in 2004.05.26 for GIFAnimation

typedef struct ShowImageItem{
	HHTMCTL hControl; // 要显示的图象的控件指针
	LPTSTR lpLocatePath; // 当前图象在本地的文件名
	HGIF hGif ; // Gif 图象的句柄
	HBITMAP hBmp ; // bitmap , jpeg 图象的句柄
	UINT iIndex; // gif图象使用，当前正在播放的图象索引
	int iDelayTime; // gif 图象使用，播放下一个索引图象还需要等待的时间
	UINT iImageNum ; //gif 图象使用，当前一共有多少幅图象
	BOOL bShow; // 是否要立即显示该图象
	BOOL bDownloadOK; // 是否已经成功下载
	struct ShowImageItem *next;  // 指向下一幅GIF动画
}SHOWIMAGEITEM,*LPSHOWIMAGEITEM;

// !!! Add End

typedef struct structUrlList{
	struct structUrlList *next;
	struct structUrlList *prev;
	LPTSTR lpUrl;
	LPTSTR lpLocateFile;
}URLLIST,*LPURLLIST;

#define NORMAL_MODE		0	// 正常模式
#define COMPACT_MODE	1	// 紧凑模式


#define HTML_FILE		0
#define IMAGE_FILE		1
typedef struct structHtmlView{
	
	UINT uShowMode;  // NORMAL_MODE , 正常模式
					 // COMPACT_MODE， 紧凑模式
	UINT iFileStyle; // 当前打开的文件类型

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

	HANDLE hExitEvent;  // 退出事件，要退出当前系统
	HANDLE hDownloadEvent; // 下载事件，有文件需要下载

	LPSHOWIMAGEITEM lpShowImageList; // 用来显示的图象列表, add by jami chen in 2004.05.26

	int iFontSize;  // 选择当前字体的大小
	int iTextHeight;

	int iDisplayMode; // 当前的显示模式

	HDC hMemoryDC; // 内存DC 句柄
	HBITMAP hMemoryBitmap; // 内存位图

	HANDLE hRasConn; // 拨号句柄
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


//  文件下载
DWORD WINAPI DownLoadmanageThread(VOID * pParam);
BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl);
BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl);
BOOL CloseRunningList(	HWND hWnd);
BOOL DeleteDownList(	HWND hWnd);
void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl);

//显示图象
DWORD WINAPI ShowImageThread(VOID * pParam);
BOOL DownloadImageOK(HWND hWnd, HHTMCTL hControl,LPTSTR lpFileName);
BOOL ReleaseAnimationList(HWND hWnd);
BOOL IsImageFile(LPTSTR lpFileName);


// general
LPTSTR GetFileNameFromUrl(HWND hWnd,LPTSTR lpUrl);
BOOL CreateMultiDirectory(
  LPCTSTR lpPathName,                         // pointer to directory path string
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // pointer to security descriptor
);
void SMIE_SetCurPath(HWND hWnd,LPTSTR lpUrl);
BOOL SMIE_IsLocateFile(HWND hWnd,LPTSTR lpUrl);

DWORD WINAPI ShowImage(HWND hWnd);

BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize);

LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl);

LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage);

BOOL IsTextControl(HHTMCTL hControl);


#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //__VIEWDEF_H
