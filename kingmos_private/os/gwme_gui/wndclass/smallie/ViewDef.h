/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
#ifndef __VIEWDEF_H
#define __VIEWDEF_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// ����������й���������


//#define USE_TIMERTOSHOWIMG
#define TIMEINTERVAL	30  // ÿ��50ms�鿴һ���Ƿ���Ҫ��ʾͼ��

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
	HHTMCTL hControl; // Ҫ��ʾ��ͼ��Ŀؼ�ָ��
	LPTSTR lpLocatePath; // ��ǰͼ���ڱ��ص��ļ���
	HGIF hGif ; // Gif ͼ��ľ��
	HBITMAP hBmp ; // bitmap , jpeg ͼ��ľ��
	UINT iIndex; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
	int iDelayTime; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
	UINT iImageNum ; //gif ͼ��ʹ�ã���ǰһ���ж��ٷ�ͼ��
	BOOL bShow; // �Ƿ�Ҫ������ʾ��ͼ��
	BOOL bDownloadOK; // �Ƿ��Ѿ��ɹ�����
	struct ShowImageItem *next;  // ָ����һ��GIF����
}SHOWIMAGEITEM,*LPSHOWIMAGEITEM;

// !!! Add End

typedef struct structUrlList{
	struct structUrlList *next;
	struct structUrlList *prev;
	LPTSTR lpUrl;
	LPTSTR lpLocateFile;
}URLLIST,*LPURLLIST;

#define NORMAL_MODE		0	// ����ģʽ
#define COMPACT_MODE	1	// ����ģʽ


#define HTML_FILE		0
#define IMAGE_FILE		1
typedef struct structHtmlView{
	
	UINT uShowMode;  // NORMAL_MODE , ����ģʽ
					 // COMPACT_MODE�� ����ģʽ
	UINT iFileStyle; // ��ǰ�򿪵��ļ�����

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

	HANDLE hExitEvent;  // �˳��¼���Ҫ�˳���ǰϵͳ
	HANDLE hDownloadEvent; // �����¼������ļ���Ҫ����

	LPSHOWIMAGEITEM lpShowImageList; // ������ʾ��ͼ���б�, add by jami chen in 2004.05.26

	int iFontSize;  // ѡ��ǰ����Ĵ�С
	int iTextHeight;

	int iDisplayMode; // ��ǰ����ʾģʽ

	HDC hMemoryDC; // �ڴ�DC ���
	HBITMAP hMemoryBitmap; // �ڴ�λͼ

	HANDLE hRasConn; // ���ž��
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


//  �ļ�����
DWORD WINAPI DownLoadmanageThread(VOID * pParam);
BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl);
BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl);
BOOL CloseRunningList(	HWND hWnd);
BOOL DeleteDownList(	HWND hWnd);
void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl);

//��ʾͼ��
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
