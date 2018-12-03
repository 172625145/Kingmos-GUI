/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��Ļģʽ��IE�����
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-02-11
���ߣ��½��� JAMI
�޸ļ�¼��
	2004.05.26 �����ʾGIF�ļ������Ĳ���,�����е�ͼ����ר�ŵ��߳�����ʾ
	2004.08.19 ��ʾ����д���ڴ�DC��Ȼ���Ƶ��豸DC
	2004.09.09 �޸ĺϳ�һ��INTERNET�����·���ĵ�ַ
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
// ��������
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
	LPHTMLCONTROL lpControl; // Ҫ��ʾ��ͼ��Ŀؼ�ָ��
	HGIF hGif ; // Gif ͼ��ľ��
	HBITMAP hBmp ; // bitmap , jpeg ͼ��ľ��
	UINT iIndex; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
	UINT iDelayTime; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
	UINT iImageNum ; //gif ͼ��ʹ�ã���ǰһ���ж��ٷ�ͼ��
	struct ShowImageItem *next;  // ָ����һ��GIF����
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

	LPSHOWIMAGEITEM lpShowImageList; // ������ʾ��ͼ���б�, add by jami chen in 2004.05.26

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
// ����������
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
// ������ATOM RegisterSMIE_BrowersClass( HINSTANCE hInst )
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ע����Ľ��
// ����������ע��С��Ļģʽ��IE�����
// ����: 
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
    wc.lpfnWndProc = SMIE_BrowersProc;  // ���ù��̾��
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // ���ñ���
    wc.lpszMenuName = 0;
    wc.lpszClassName = classSMIE_BROWERS;  // ��������

    return (BOOL)(RegisterClass( &wc ));  // ��ϵͳע��
}


// **************************************************
// ������LRESULT CALLBACK SMIE_BrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN uMsg -- ������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ�����ظ���Ϣ�Ĵ�����
// �����������ؼ���Ϣ���̺���
// ����: 
// **************************************************
LRESULT CALLBACK SMIE_BrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_PAINT:  // ����
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

	case HM_SETFONTSIZE: // ���������С
		return DoSetFontSize(hWnd,wParam,lParam);
	case HM_SETDISPLAYMODE: // ������ʾģʽ
		return DoSetDisplayMode(hWnd,wParam,lParam);
	case WM_SIZE: // ���ڴ�С�����ı�
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
// ������static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����0�����򷵻�-1��
// ���������������ؼ�������WM_CREATE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	HANDLE handle;
	TEXTMETRIC TextMetric;
	HDC hdc;
//	int i;

//		RETAILMSG(1,(TEXT("Create IE Control ...\r\n")));
	    // ����ṹ
		lpHtmlView=(LPHTMLVIEW)malloc(sizeof(HTMLVIEW));
		if (lpHtmlView == NULL)
			return -1;
		SetWindowLong(hWnd,0,(long)lpHtmlView); // ��HTML�ṹ���õ�����

		// ��ʼ���ṹ
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
		lpHtmlView->lpCurPath=BufferAssignTChar(TEXT("\\"));  // ���ø�·��Ϊ��ǰ·��

#ifdef INTERNET_SUPPORT
		// ��ʼ��INTERNET��������
		lpHtmlView->hInternetSiteInit=NULL;
		lpHtmlView->lpDownLoadList=NULL;
		lpHtmlView->lpRunningList = NULL;
#endif
#ifdef LOADANIMATION
		// ��ʼ����������
		lpHtmlView->hAnimation = NULL;
		lpHtmlView->bExitAnimation = FALSE;
#endif


		lpHtmlView->iRunThreadCount = 0;
		lpHtmlView->bExit = FALSE;
		lpHtmlView->bStop = FALSE;

		lpHtmlView->hRasConn = NULL; 
		// ����Ĭ�ϵ�����	
		lpHtmlView->iFontSize = FONT_BIGGER; // Ĭ��Ϊ������
		hdc=GetDC(hWnd);

		// get current text property
		GetTextMetrics(hdc,&TextMetric);
		ReleaseDC(hWnd,hdc);
		lpHtmlView->iTextHeight = TextMetric.tmHeight;

		//����Ĭ�ϵ���ʾģʽ
		lpHtmlView->iDisplayMode = DISPLAY_TEXTIMAGE; 
		// ��ʼ���ٽ����
		INITLOCK(lpHtmlView->DownLoadCriticalSection);
		INITLOCK(lpHtmlView->RunningCriticalSection);
		
		lpHtmlView->lpShowImageList = NULL; // ��ʼû��Ҫ��ʾ��ͼ��ؼ�

		CreateMemoryDC(hWnd);

		SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������

		lpHtmlView->hDownloadEvent = CreateEvent(NULL,TRUE,FALSE,"Download");
		lpHtmlView->hExitEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

		handle = CreateThread(NULL, 0, DownLoadmanageThread, (void *)hWnd, 0, 0 );  // �������ع����߳�
		CloseHandle(handle);  // �رվ�����߳��Ѿ����������û������

#ifdef USE_TIMERTOSHOWIMG
		SetTimer(hWnd,100,TIMEINTERVAL,NULL);
#else
		handle = CreateThread(NULL, 0, ShowImageThread, (void *)hWnd, 0, 0 );  // �������ع����߳�
//		SetThreadPriority(handle,THREAD_PRIORITY_ABOVE_NORMAL);
		CloseHandle(handle);  // �رվ�����߳��Ѿ����������û������
#endif
		SetFocus(hWnd);  // ���ý��㵽��ǰ����

		JPEG_Init(); // JPEG ϵͳ��ʼ������������ʾJPEG�ļ�

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
// ������static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �����������ƻ��ؼ�������WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;

		lpHtmlView->bExit = TRUE;  // �����˳�ϵͳ
		SetEvent(lpHtmlView->hExitEvent);
//		EndAnimation(hWnd);
//		RETAILMSG(1,("Stop Current DownLoad\r\n"));
		DoStop(hWnd,0,0);  // ֹͣ��ǰ�������е��߳�

//		RETAILMSG(1,("Will HandDown Internet [%x]\r\n",lpHtmlView->hRasConn));
		HandDownInternet(hWnd,lpHtmlView->hRasConn); // �Ҷ����������ϵ

		if (lpHtmlView->hInternetSiteInit)
		{  // �ر�INTERNET
			InternetCloseHandle(lpHtmlView->hInternetSiteInit);
			WSACleanup();
		}
//		RETAILMSG(1,("###################Sleep\r\n"));
//		Sleep(10000);
		WaitExitThread(hWnd);  // �ȴ������߳��˳�

		if (lpHtmlView->hMemoryDC)
			DeleteDC(lpHtmlView->hMemoryDC);

		if (lpHtmlView->hMemoryBitmap)
			DeleteObject(lpHtmlView->hMemoryBitmap);

		if (lpHtmlView->lpUrl!=NULL)
		{  // �ͷŵ�ǰ��ҳ�Ļ���
			free(lpHtmlView->lpUrl);
			lpHtmlView->lpUrl=NULL;
		}
		if (lpHtmlView->lpCurPath)
		{  // �ͷŵ�ǰ·���Ļ���
			free(lpHtmlView->lpCurPath);
			lpHtmlView->lpCurPath=NULL;
		}
		if (lpHtmlView->lpCurFile!=NULL)
		{  // �ͷŵ�ǰ�ļ��Ļ���
			free(lpHtmlView->lpCurFile);
			lpHtmlView->lpCurFile=NULL;
		}
		if (lpHtmlView->lpMark!=NULL)
		{  // �ͷŵ�ǰ��ǵĻ���
			free(lpHtmlView->lpMark);
			lpHtmlView->lpMark=NULL;
		}
		if (lpHtmlView->hHtml)
		{  // �ͷ�HTML
			ReleaseAnimationList(hWnd); // �ͷ�������ʾ���б�
			SMIE_ReleaseHtml(lpHtmlView->hHtml);
			lpHtmlView->hHtml=NULL;
		}
		if (lpHtmlView->lpUrlHead)
			SMIE_DeleteUrlList(lpHtmlView->lpUrlHead);  // ɾ��URL�б�

#ifdef INTERNET_SUPPORT
//		while(lpHtmlView->lpDownLoadList)
//		{
//			DeleteDownList(hWnd);
//		}
#endif
		//�ͷ��ٽ����
		DEINITLOCK(lpHtmlView->DownLoadCriticalSection);  
		DEINITLOCK(lpHtmlView->RunningCriticalSection);

		// �ͷ�HTML�ṹ
		free(lpHtmlView);
	    SetWindowLong(hWnd,0,0);  // ����NULL������
		JPEG_Deinit(); // �ͷ�JPEGϵͳ

		PostQuitMessage(0);  // ��������������������������������
		return 0;
}
// **************************************************
// ������static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint)
// ������
// 	IN hWnd -- ���ھ��
//	 IN hdc -- �豸���
// 	IN rcPaint -- Ҫ���»��Ƶ�����
// 
// ����ֵ����
// �������������ƿؼ�������WM_PAINT��Ϣ��
// ����: 
// **************************************************
static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint)
{
//	LPHTMLOBJECT lpHead;
//	LPHTMLVIEW lpHtmlView;
//	POINT ptOrg;
//		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
//		if (lpHtmlView==NULL)
//			return;

// !!! modified By Jami chen in 2004.08.19
/*
		if (lpHtmlView->iFontSize == FONT_SMALLLER)
		{// ��С����
			SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
		}
//#ifndef __WCE_DEFINE
//		SetWindowOrgEx(hdc,lpHtmlView->x_Org,lpHtmlView->y_Org,NULL);
//#else
//		SetViewportOrgEx(hdc,(0-lpHtmlView->x_Org),(0-lpHtmlView->y_Org),NULL);
//#endif
		ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
		ptOrg.y=lpHtmlView->y_Org;
 		SMIE_ShowHtml(hWnd,hdc,lpHtmlView->hHtml,ptOrg);  // ����HTML
*/
		DrawToScreen(hWnd,hdc);
// !!! modified end By Jami chen in 2004.08.19
}
// **************************************************
// ������static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ��������������ؼ��Ķ�ʱ��������WM_TIMER��Ϣ��
// ����: 
// **************************************************
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
#ifdef USE_TIMERTOSHOWIMG
		ShowImage(hWnd);
#endif
		return 0;
}

// **************************************************
// ������static LRESULT DoGo(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR Ҫ�򿪵���ҳ�ĵ�ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ������������һ��ָ������ҳ������HM_GO��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGo(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpUrl,lpFullUrl;
	LRESULT lResult;
	LPHTMLVIEW lpHtmlView;


//		RETAILMSG(1,("############################\r\n"));
//		RETAILMSG(1,("############################\r\n"));

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTMLVIEW �ṹ
		if (lpHtmlView==NULL)
			return 0;

//		EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);
//		EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
		
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ�뵽��ǰ����
#endif
		lpUrl=(TCHAR *)lParam;
		if (lpHtmlView->lpUrl!=NULL)
		{
			SendMessage(hWnd,HM_STOP,0,0);  // ֹͣ��ǰ����ҳ����
			free(lpHtmlView->lpUrl);  // �ͷŵ�ǰ��ҳ
			lpHtmlView->lpUrl=NULL;
		}
		//  if is locate file ,then add the directory
		RETAILMSG(1, ("Befor get full (%s) \r\n", lpUrl));
		lpFullUrl = SMIE_GetFullUrl(hWnd,lpUrl);  // �õ�VIEW�ĵ�ǰ��ҳ
		RETAILMSG(1, ("after get full (%s) \r\n", lpFullUrl));
		if (IsInternetUrl(lpFullUrl) == TRUE)
		{
			// ��ǰURL��һ����ҳ������ҳ
			lpHtmlView->lpUrl = lpFullUrl;
			RETAILMSG(1, ("SMIE 1111111111111(%s)\r\n", lpFullUrl));
			lResult=SMIE_GoUrl(hWnd,lpHtmlView->lpUrl);  // �򿪵�ǰ��ҳ
		}
		else
		{
			// ��ǰ�򿪵���һ�������ļ�����������ļ�
			RETAILMSG(1, ("SMIE 22222\r\n"));
			lpHtmlView->bStop = FALSE;  // ����ֹͣ����ΪFALSE
			SMIE_DownLoadSpecialFile(hWnd,lpFullUrl);
		}
		return lResult;
}
// **************************************************
// ������static LRESULT SMIE_GoUrl(HWND hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- Ҫ�򿪵���ҳ��ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ������������ָ������ҳ��
// ����: 
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

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTMLVIEW �ṹ
		if (lpHtmlView==NULL)
			return 0;

		// ������г��������أ���ֹͣ����
		DoStop(hWnd,0,0);  // ֹͣ��ǰ�������е��߳�

		lpHtmlView->bStop = FALSE;  // ����ֹͣ����ΪFALSE
//		lpFileName=(TCHAR *)lParam;


		// !!! Add By Jami chen in 2004.08.07
		if (strlen(lpUrl) == 0)
		{
			// �ǿհ���ҳ
			if (lpHtmlView->lpCurFile)
				free(lpHtmlView->lpCurFile);  // ɾ��ԭ���ĵ�ǰ�ļ�
			lpHtmlView->lpCurFile = NULL;//SMIE_BufferAssignTChar(lpUrl);  // ���õ�ǰ����ҳΪ��ǰ�ļ�

			if (lpHtmlView->lpMark)
				free(lpHtmlView->lpMark);  // ɾ��ԭ���ı��
			lpHtmlView->lpMark = NULL;//SMIE_BufferAssignTChar(lpUrl);  // ���õ�ǰ�ı��
			if (lpHtmlView->hHtml)
			{  // �ͷ�ԭ����HTML
				ReleaseAnimationList(hWnd); // �ͷ�������ʾ���б�
				SMIE_ReleaseHtml(lpHtmlView->hHtml);
				lpHtmlView->hHtml=NULL;
			}
			EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);  // ��Ч������
			EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������
			lpHtmlView->x_Org=0;  // ��ʾԭ��ص�(0,0)
			lpHtmlView->y_Org=0;
// !!! modified By Jami chen in 2004.08.19
			ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
			ptOrg.y=lpHtmlView->y_Org;
			if (lpHtmlView->uShowMode == COMPACT_MODE)
	 			SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
//			else
//	 			ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
			InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
			return 0;
		}
		// !!! Add End By Jami chen in 2004.08.07


//		JAMIMSG(DBG_FUNCTION,(TEXT("Enter DoLoadFile\r\n")));
//		JAMIMSG(DBG_FUNCTION,(TEXT("Will Load FileName is \"%s\"\r\n"),lpFileName));

//		SetWindowText(GetDlgItem(hWnd,IDC_ADDRESS),lpUrl);
		
		RETAILMSG(1, ("URL :(%s)...\r\n", lpUrl));
		SMIE_ApartMark(lpUrl,&lpPureUrl,&lpMark);  // ������ҳ�еı������ַ
		RETAILMSG(1, ("URL :(%s) after apart...\r\n", lpUrl));

		in_file=CreateFile(lpPureUrl,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);  // ����ַָ�����ļ�
		if (in_file==INVALID_HANDLE_VALUE)
		{  // This is not Locale file
			if (lpHtmlView->lpMark) 
				free(lpHtmlView->lpMark);  // ɾ��ԭ���ı��
			lpHtmlView->lpMark=lpMark;//SMIE_BufferAssignTChar(lpUrl);  // ���õ�ǰ�ı��

#ifdef INTERNET_SUPPORT
			iStartTick = GetTickCount();
//			RETAILMSG(1,("Will Load  \"%s\"\r\n",lpPureUrl));
			SMIE_StartNewFtpSite(hWnd,lpPureUrl);  // ��ָ������ҳ
			if (lpPureUrl)
				free(lpPureUrl);  // �ͷŲ��õĻ���
			return TRUE;
#else
			if (lpPureUrl)
				free(lpPureUrl); // �ͷŲ��õĻ���
			if (lpHtmlView->lpCurFile)
				free(lpHtmlView->lpCurFile);  // �ͷŲ��õĻ���
			lpHtmlView->lpCurFile=NULL;   // ���õ�ǰ�ļ�ΪNULL
			PostMessage(hWnd,HM_REFRESH,0,0);  // ˢ�»���
			return FALSE;
#endif
//			in_file=CreateFile(lpTempFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
//			if (in_file==INVALID_HANDLE_VALUE)
		}
		SMIE_SetCurPath(hWnd,lpPureUrl);  // ���õ�ǰ·�����ļ���·��
		CloseHandle(in_file);

		if (lpHtmlView->lpCurFile)
			free(lpHtmlView->lpCurFile);  // ɾ��ԭ���ĵ�ǰ�ļ�
		lpHtmlView->lpCurFile=lpPureUrl;//SMIE_BufferAssignTChar(lpUrl);  // ���õ�ǰ����ҳΪ��ǰ�ļ�

		if (lpHtmlView->lpMark)
			free(lpHtmlView->lpMark);  // ɾ��ԭ���ı��
		lpHtmlView->lpMark=lpMark;//SMIE_BufferAssignTChar(lpUrl);  // ���õ�ǰ�ı��

//		RETAILMSG(1,("M222MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
		PostMessage(hWnd,HM_REFRESH,0,0);  // ˢ�»���
		return TRUE;
}
// **************************************************
// ������static LRESULT DoRefresh(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ����������ˢ�µ�ǰҳ�棬����HM_REFRESH��Ϣ��
// ����: 
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

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
//		RETAILMSG(1,(TEXT("DoRefresh!!!\r\n")));
		if (lpHtmlView->lpCurFile==NULL)
		{  // û�б����ļ�
			if (lpHtmlView->lpUrl)
			{  // �Ƿ�Ҫ��һ��ָ������ҳ
				MessageBox(hWnd,lpHtmlView->lpUrl,"����ҳʧ��",MB_OK);
				free(lpHtmlView->lpUrl);
				lpHtmlView->lpUrl=NULL;
			}
			return FALSE;
		}
//		RETAILMSG(1,(TEXT("Will Open File <%s>\r\n"),lpHtmlView->lpCurFile));
		in_file=CreateFile(lpHtmlView->lpCurFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);  // ���ļ�
		if (lpHtmlView->lpUrl)
		{  // ����ҳ����Ӧ�ı����ļ���ӵ��Ѿ��򿪵��ļ��б���
			SMIE_InsertUrlList(lpHtmlView,lpHtmlView->lpUrl,lpHtmlView->lpCurFile);
			free(lpHtmlView->lpUrl);  // �ͷ���ҳ����
			lpHtmlView->lpUrl=NULL;
		}
		if (in_file==INVALID_HANDLE_VALUE)
		{  // ���ļ�ʧ��
			MessageBox(hWnd,lpHtmlView->lpCurFile,"����ҳʧ��",MB_OK);
			return FALSE;
		}

		//RETAILMSG(1,(TEXT("Open File Success!!!\r\n")));
//#ifdef INTERNET_SUPPORT
		SMIE_SendInternetNotifyMessage(hWnd,INM_ADDRESSCHANGE,lpHtmlView->lpCurUrl->lpUrl);  // ������Ϣ�������ڣ���֪��ǰ��ҳ�Ѿ��ı�
		EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);  // ��Ч������
		EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
//#endif

// !!! Add By Jami chen in 2003.09.09
		if (IsImageFile(lpHtmlView->lpCurFile))
		{  // �򿪵���һ��ͼ���ļ�
			CloseHandle(in_file);  // �ر��ļ����
			lpHtmlView->iFileStyle = IMAGE_FILE;
			if (lpHtmlView->hHtml)
			{  // �ͷ�ԭ���ļ���HTML���
				ReleaseAnimationList(hWnd); // �ͷ�������ʾ���б�
				SMIE_ReleaseHtml(lpHtmlView->hHtml);
				lpHtmlView->hHtml=NULL;
			}
			lpHtmlView->hHtml = SMIE_LoadImage(hWnd,lpHtmlView->lpCurFile);  // ����ͼ���ļ�
//	!!! modified By Jami chen in 2004.08.19
//			hdc=GetDC(hWnd);
//			if (lpHtmlView->iFontSize == FONT_SMALLLER)
//			{// ��С����
//				SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//			}
//			SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // ������Ҫ�ĳߴ�
//			ReleaseDC(hWnd,hdc);
//			SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // ������Ҫ�ĳߴ�
//	!!! modified End By Jami chen in 2004.08.19

			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������
			lpHtmlView->x_Org=0;  // ��ʾԭ��ص�(0,0)
			lpHtmlView->y_Org=0;
// !!! modified By Jami chen in 2004.08.19
//			InvalidateRect(hWnd,NULL,TRUE);
			ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
			ptOrg.y=lpHtmlView->y_Org;
	 		SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
			InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
//			ShowImageThread((VOID * )hWnd);
			return TRUE;  // �ɹ�����
		}
// !!! Add End By Jami chen in 2003.09.09
		lpHtmlView->iFileStyle = HTML_FILE;  // һ��HTML�ļ�

//		RETAILMSG(1,(TEXT("N11\r\n")));
		dwFileLen=GetFileSize(in_file,NULL);  // �õ��ļ���С
//		RETAILMSG(1,(TEXT("N22\r\n")));
		lpFileContent=(char *)malloc(dwFileLen+1);  // ���仺��������ļ�����
//		RETAILMSG(1,(TEXT("N33\r\n")));
		if (lpFileContent==NULL)
		{  // �����ڴ�ʧ��
			CloseHandle(in_file);  // �ر��ļ����
//			CloseHandle(in_file);
			return FALSE;
		}
//		RETAILMSG(1,(TEXT("N44\r\n")));
		ReadFile(in_file,lpFileContent,dwFileLen,&dwReadLen,NULL);  // ��ȡ�ļ�����
		if (dwReadLen == 0)
		{
			DWORD err = GetLastError();
			CloseHandle(in_file);  // �ر��ļ����
			return FALSE;
		}
//		RETAILMSG(1,(TEXT("N55\r\n")));
		lpFileContent[dwReadLen]=0;  // ��ӽ�β��
//		RETAILMSG(1,(TEXT("N66\r\n")));
//		CloseHandle(in_file);  // �ر��ļ����
		CloseHandle(in_file);  // �ر��ļ����

//		RETAILMSG(1,(TEXT("Read File OK!!!\r\n")));

		if (lpHtmlView->hHtml)
		{  // �ͷ�ԭ����HTML
			ReleaseAnimationList(hWnd); // �ͷ�������ʾ���б�
			SMIE_ReleaseHtml(lpHtmlView->hHtml);
			lpHtmlView->hHtml=NULL;
		}

		lpHtmlView->hHtml=SMIE_ParseHtml(hWnd,lpFileContent);  // ������ǰ�ļ�

//		RETAILMSG(1,(TEXT("SMIE_ParseHtml OK!!!\r\n")));
// !!! modified by jami chen in 2004.08.19
//		hdc=GetDC(hWnd);
//		if (lpHtmlView->iFontSize == FONT_SMALLLER)
//		{// ��С����
//			SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//		}
//		SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // �����С
//		ReleaseDC(hWnd,hdc);
		if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // �Ƿ���ͼ���ļ�, �����ͼ���ļ�������Ҫ���¼���ߴ�
			SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // ������Ҫ�ĳߴ�
// !!! modified end By Jami chen in 2004.08.19
//		RETAILMSG(1,(TEXT("SMIE_ReCalcSize OK!!!\r\n")));
		SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������
		lpHtmlView->x_Org=0;
		lpHtmlView->y_Org=0;
//		bReCalcSize=FALSE;
		free(lpFileContent);  // �ͷ��ļ����ݵĻ���
// !!! modified By Jami chen in 2004.08.19
//		InvalidateRect(hWnd,NULL,TRUE);
		ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
		ptOrg.y=lpHtmlView->y_Org;
	 	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
		InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19

		if (lpHtmlView->lpMark)  // �����Ҫ��λ��һ��ָ���ı��
			SendMessage(hWnd,HM_GOMARK,0,(LPARAM)lpHtmlView->lpMark);
		return TRUE;  // �ɹ�����
}

// **************************************************
// ������static LRESULT DoBackWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �������������ص�ǰһ��ҳ�棬����HM_GOBACK��Ϣ��
// ����: 
// **************************************************
static LRESULT DoBackWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;
		if (lpHtmlView->lpCurUrl==NULL)  // ��ǰҳ��Ϊ��
			return 0;
		if (lpHtmlView->lpUrlHead==lpHtmlView->lpCurUrl)  // �Ѿ�����һҳ
			return 0;
		lpHtmlView->lpCurUrl=lpHtmlView->lpCurUrl->prev;  // ����ǰ��ҳ�趨��ǰһ����ҳ
		if (lpHtmlView->lpCurUrl->lpUrl)
		{
			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpLocateFile);  // ��ǰһ����ҳ
//			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpUrl);
//			PostMessage(hWnd,HM_REFRESH,0,0);

//			SendMessage(hWnd,HM_GO,0,(LPARAM)lpHtmlView->lpCurUrl->lpUrl);
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ����������ǰ������һ��ҳ�棬����HM_FORWARDS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;
		if (lpHtmlView->lpCurUrl==NULL)  // ��ǰ��ҳΪ��
			return 0;
		if (lpHtmlView->lpCurUrl->next==NULL)  // �Ѿ������һ����ҳ
			return 0;
		lpHtmlView->lpCurUrl=lpHtmlView->lpCurUrl->next;  // ����ǰ��ҳ�趨����һ����ҳ
		if (lpHtmlView->lpCurUrl->lpUrl)
		{
			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpLocateFile);  // �򿪵�ǰ��ҳ
//			SMIE_GoUrl(hWnd,lpHtmlView->lpCurUrl->lpUrl);
//			SendMessage(hWnd,HM_GO,0,(LPARAM)lpHtmlView->lpCurUrl->lpUrl);
//			PostMessage(hWnd,HM_REFRESH,0,0);
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��ǰ�İ���״̬
//				  HIWORD �����Ĵ�С					
// 	IN lParam -- LOWORD ��ǰ��ˮƽλ��
//				  HIWORD ��ǰ�Ĵ�ֱλ��
// 
// ����ֵ����
// ���������������ֹ���������WM_MOUSEWHEEL��Ϣ��
// ����: 
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

	iPos=zDelta/STEPHEIGHT;  // �����Ĵ�С
	SMIE_VScrollWindow(hWnd,iPos);  // ��������
	return TRUE;
}

// **************************************************
// ������static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ���¼��������ֵ
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�� ���򷵻�FALSE��
// �����������м����£�����WM_KEYDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
	LPHTMLVIEW lpHtmlView;
	RECT rect;
	short nPage;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;
		nVirtKey = (int) wParam;    // virtual-key code
		GetClientRect(hWnd,&rect);  // �õ����ڿͻ������С
        
		nPage=(rect.bottom-rect.top)/STEPHEIGHT;  //�õ�һҳҪ����������

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
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ��ǰ����λ��
// 
// ����ֵ����
// �������������������£�����WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpUrl=NULL;
	DWORD x,y;
	LPHTMLVIEW lpHtmlView;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
	if (lpHtmlView==NULL)
		return 0;
	x=(short)LOWORD(lParam)+lpHtmlView->x_Org;  // �õ���ǰ���ĵ�
	y=(short)HIWORD(lParam)+lpHtmlView->y_Org;
	SMIE_GetUrl(lpHtmlView->hHtml,x,y,&lpUrl);  // �õ����λ�õ�URL
//	RETAILMSG(1,("Get New Url is <%s>\r\n",lpUrl));
	if (lpUrl)
	{ // �д�����Ч��URL
		if (*lpUrl=='#')
		{  // ��һ�����
			SendMessage(hWnd,HM_GOMARK,0,(LPARAM)lpUrl+1);  // ���е��б�ǵ�λ��
			free(lpUrl); // �ͷ�URL
		}
		else
		{
			if (lpHtmlView->lpCurUrl->lpUrl)
			{  // һ��ָ����ҳ��URL
				if (strnicmp(lpUrl,MAILTO_HEAD,7)==0)
				{
					// ��һ���ʼ���ַ
					// ���ʼ���lpUrl + 7
//					RETAILMSG(1,(TEXT("Will Send a Email to : %s\r\n"),lpUrl + 7));
					free(lpUrl);  // �ͷ�URL
					return 0;
				}
				if (strnicmp(lpUrl,INTERNET_HEAD,5)!=0)
				{  // ����һ��ȫ·������ҳ
					// is a child string , must add the current url
					LPTSTR lpFullUrl;
					DWORD dwlen;
// !!! Delete By Jami chen in 2004.09.09
//					lpFullUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpUrl)+4);  // ������ҳ����Ҫ�Ļ���
//					if (lpFullUrl==NULL)  // ����ʧ��
//						return FALSE;
// !!! Delete End By Jami chen in 2004.09.09
					// !!! Add By Jami chen in 2003.09.09
					if (SMIE_IsLocateFile(hWnd,lpUrl))
					{  // ��һ�������ļ�
						lpFullUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurPath)+strlen(lpUrl)+4);  // ������ҳ����Ҫ�Ļ���
						if (lpFullUrl==NULL)  // ����ʧ��
							return FALSE;
						strcpy(lpFullUrl,lpHtmlView->lpCurPath); // ����ǰ·��������
						dwlen = strlen(lpFullUrl);
						if (lpFullUrl[ dwlen - 1]!='\\')
						{
						// !!! Add By Jami chen in 2003.09.09
							lpFullUrl[ dwlen]='\\';
						// !!! Add By Jami chen in 2003.09.09
							lpFullUrl[ dwlen + 1]=0;
						}
						strcat(lpFullUrl,lpUrl);  // ��ӵõ���URL��Ϊ��·��
						free(lpUrl);
						lpUrl = lpFullUrl;
					}
					else
					// !!! Add End By Jami chen in 2003.09.09
					{  // ��һ��WWW��ַ
// !!! Modified  By Jami chen in 2004.09.09
						/*
						strcpy(lpFullUrl,lpHtmlView->lpCurUrl->lpUrl);  // ����ǰ����ҳURL��������ǰ·��
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
					//strcat(lpFullUrl,lpUrl);  // ��ӵõ���URL��Ϊ��·��
					//free(lpUrl);
					//lpUrl = lpFullUrl;
// !!! Delete End By Jami chen in 2004.09.09
				}
			}
	//		MessageBox(hWnd,lpUrl,TEXT("Get URL"),MB_OK);
//			RETAILMSG(1,("The New Url is <%s>\r\n",lpUrl));
			if (lpUrl == NULL)
				return 0;
			SendMessage(hWnd,HM_GO,0,(LPARAM)lpUrl);  // �򿪵�ǰURL
			free(lpUrl);  // �ͷ�URL
		}
	}
	SetFocus(hWnd);
	return 0;
}

// **************************************************
// ������static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��������
// 	IN lParam -- ����
// 
// ����ֵ����
// ������������ֱ�������ڣ�����WM_VSCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{

    short nScroll,nPage;
    short nPos;

//	DWORD widthWindow,heightWindow;
	SCROLLINFO ScrollInfo;

	RECT rect;
        
		GetClientRect(hWnd,&rect);  //�õ����ڵĿͻ�����
        
		nPage=(rect.bottom-rect.top)/STEPHEIGHT;
		ScrollInfo.cbSize=sizeof(SCROLLINFO);
		ScrollInfo.fMask=SIF_POS;
		ScrollInfo.nPos=0;
	    GetScrollInfo(hWnd,SB_VERT,&ScrollInfo);  // �õ���ǰ�Ĺ���������
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
// ������static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��������
// 	IN lParam -- ����
// 
// ����ֵ����
// ����������ˮƽ�������ڣ�����WM_HSCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    short nScroll,nPage;
    short nPos;

//	DWORD widthWindow,heightWindow;
	SCROLLINFO ScrollInfo;

	RECT rect;


		GetClientRect(hWnd,&rect);  // �õ��ͻ�����
        
		nPage=(rect.right-rect.left)/STEPWIDTH;
		ScrollInfo.cbSize=sizeof(SCROLLINFO);
		ScrollInfo.fMask=SIF_POS;
		ScrollInfo.nPos=0;
	    GetScrollInfo(hWnd,SB_HORZ,&ScrollInfo);  // �õ�������������
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
// ������static LRESULT DoGoMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR ָ���ı��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ��������������ǰλ������ָ���ı�ţ�����HM_GOMARK��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGoMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	int xPos,yPos;
	LPTSTR lpMark;
	SCROLLINFO ScrollInfo;
	POINT ptOrg;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // // �õ�HTML�ṹ
	if (lpHtmlView==NULL)
		return 0;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ���ַ����ǰ����
#endif
	lpMark=(LPTSTR)lParam;  // �õ����
	if (SMIE_GetMarkPos(lpHtmlView->hHtml,&xPos,&yPos,lpMark)==FALSE)  // �õ���ǵ�λ��
		return FALSE;

	lpHtmlView->y_Org=yPos;
//	lpHtmlView->y_Org=600;
// !!! modified By Jami chen in 2004.08.19
//	InvalidateRect(hWnd,NULL,TRUE);
	ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
	ptOrg.y=lpHtmlView->y_Org;
	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
	InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19

	ScrollInfo.cbSize=sizeof(SCROLLINFO);
	ScrollInfo.fMask=SIF_POS;
//	ScrollInfo.nPos=0;
//	GetScrollInfo(hWnd,SB_VERT,&ScrollInfo);

//	ScrollInfo.nPos+=nScroll;
//	ScrollInfo.nPos=lpHtmlView->y_Org/STEPHEIGHT;
	ScrollInfo.nPos=(lpHtmlView->y_Org+STEPHEIGHT-1)/STEPHEIGHT;
	SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);  // ���������ʵ��
	return TRUE;
}
// **************************************************
// ������static LRESULT DoReadData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ���������ֽ���
// 	IN lParam -- LPTSTR Ҫ�������ݵĻ���
// 
// ����ֵ���ɹ��������ݵĴ�С�����򷵻�-1��
// �����������õ���ǰ��ҳ�����ݣ�����HM_READDATA��Ϣ��
// ����: 
// **************************************************
static LRESULT DoReadData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	int dwMaxBuffer;
	LPTSTR lpBuffer;
	HANDLE in_file;
	DWORD dwFileLen;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;

		if (lpHtmlView->lpCurFile == NULL)  // û�б����ļ����˳�
			return -1;

		dwMaxBuffer = (int)wParam;
		lpBuffer = (LPTSTR)lParam;

		// ���ļ�
		in_file=CreateFile(lpHtmlView->lpCurFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if (in_file==INVALID_HANDLE_VALUE)
		{  // This is not Locale file
			return -1;
		}

		dwFileLen=GetFileSize(in_file,NULL);  // �õ��ļ�����
		if ( dwMaxBuffer == 0 && lpBuffer == NULL)
		{// Ҫ��õ��ļ���С
//			CloseHandle(in_file);
			CloseHandle(in_file);  // �ر��ļ����
			return dwFileLen;
		}
		if (dwFileLen > (DWORD)dwMaxBuffer)
		{  // ����̫С
//			CloseHandle(in_file);
			CloseHandle(in_file);  // �ر��ļ����
			return -1;
		}
		ReadFile(in_file,lpBuffer,dwMaxBuffer,&dwMaxBuffer,NULL);  // ��ȡ����
		CloseHandle(in_file);  // �ر��ļ�
		return dwMaxBuffer;  
}
// **************************************************
// ������static BOOL SMIE_ResetScrollBar(HWND hWnd,HHTML hHtml)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hHtml --  ��ǰ��ҳ���
// 
// ����ֵ����
// ���������������������
// ����: 
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
		SMIE_GetHtmlSize(hHtml,&size);  // �õ�HTML�Ĵ�С
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		widthWindow=rect.right-rect.left;
		heightWindow=rect.bottom-rect.top;

		if (widthWindow<(DWORD)size.cx)
		{  // ��Ҫˮƽ������
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

			SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);  // ���ù���������

		}
		else
		{  // ����Ҫˮƽ������
			EnableScrollBar(hWnd,SB_HORZ, ESB_DISABLE_BOTH);  
		}

		if (heightWindow<(DWORD)size.cy)
		{  // ��Ҫ��ֱ������
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

			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);  // ���ô�ֱ����������
		}
		else
		{  // ����Ҫ��ֱ������
			EnableScrollBar(hWnd,SB_VERT, ESB_DISABLE_BOTH);
		}
		return TRUE;
}
// **************************************************
// ������static void SMIE_HScrollWindow(HWND hWnd,short nScroll)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nScroll -- ������С
// 
// ����ֵ����
// ����������ˮƽ�������ڡ�
// ����: 
// **************************************************
static void SMIE_HScrollWindow(HWND hWnd,short nScroll)
{
	short nScrollSize,widthWindow;
	RECT rect;
	LPHTMLVIEW lpHtmlView;
	SIZE size;
//	SCROLLINFO ScrollInfo;
	POINT ptOrg;


	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
	if (lpHtmlView==NULL)
		return ;

	GetClientRect(hWnd,&rect);  // �õ����ڿͻ�����
	widthWindow=(short)(rect.right-rect.left);

	SMIE_GetHtmlSize(lpHtmlView->hHtml,&size);  // �õ�HTML�Ĵ�С


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
//	InvalidateRect(hWnd,NULL,TRUE);  // �ػ�����
// !!! modified By Jami chen in 2004.08.19
//	InvalidateRect(hWnd,NULL,TRUE);
	ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
	ptOrg.y=lpHtmlView->y_Org;
	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
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
	SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);  // ����ˮƽ����������

  */
	SetCurrentScrollPos(hWnd,SB_HORZ);

}
// **************************************************
// ������static void SMIE_VScrollWindow(HWND hWnd,short nScroll)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nScroll -- ������С
// 
// ����ֵ����
// ������������ֱ�������ڡ�
// ����: 
// **************************************************
static void SMIE_VScrollWindow(HWND hWnd,short nScroll)
{
	short nScrollSize,heightWindow;
	RECT rect;
	LPHTMLVIEW lpHtmlView;
	SIZE size;
//	SCROLLINFO ScrollInfo;
	POINT ptOrg;


	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); // �õ�HTML�ṹ
	if (lpHtmlView==NULL)
		return ;

	GetClientRect(hWnd,&rect);  // �õ����ڿͻ�����
	heightWindow=(short)(rect.bottom-rect.top);

	if (lpHtmlView->hHtml == NULL)
	{
		// ��û�д��κε���ҳ
		size.cx = size.cy = 0;
	}
	else
	{
		SMIE_GetHtmlSize(lpHtmlView->hHtml,&size);  // �õ�HTML�Ĵ�С
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
//	InvalidateRect(hWnd,NULL,TRUE);  // �ػ洰��
// !!! modified By Jami chen in 2004.08.19
//	InvalidateRect(hWnd,NULL,TRUE);
	ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
	ptOrg.y=lpHtmlView->y_Org;
	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
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
// ������static void SMIE_SendInternetNotifyMessage(HWND hWnd,UINT code,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN code -- ֪ͨ��Ϣ����
// 	IN lpUrl -- ��ǰ�򿪵���ҳ��ַ
// 
// ����ֵ����
// ��������������һ��֪ͨ��Ϣ�������ڡ�
// ����: 
// **************************************************
static void SMIE_SendInternetNotifyMessage(HWND hWnd,UINT code,LPTSTR lpUrl)
{
	NMINTERNET nmInternet;

		nmInternet.hdr.hwndFrom=hWnd;
		nmInternet.hdr.idFrom=GetWindowLong(hWnd,GWL_ID);
		nmInternet.hdr.code=code;
		nmInternet.lpUrl=lpUrl;

		// ����һ��INTERNET��֪ͨ��Ϣ��������
		SendMessage(GetParent(hWnd),WM_NOTIFY,(WPARAM)nmInternet.hdr.idFrom,(LPARAM)&nmInternet);
		return ;
}

//#endif


// **************************************************
// ������static void SMIE_InsertUrlList(LPHTMLVIEW lpHtmlView, LPTSTR lpUrl,LPTSTR lpLocateFile)
// ������
// 	IN lpHtmlView -- ��ҳ�ӽṹ
// 	IN lpUrl -- Ҫ�������ҳ��ַ
// 	IN lpLocateFile -- ָ����ҳ�������ļ�
// 
// ����ֵ����
// ��������������һ���Ѿ��򿪵���ҳ���б�
// ����: 
// **************************************************
static void SMIE_InsertUrlList(LPHTMLVIEW lpHtmlView, LPTSTR lpUrl,LPTSTR lpLocateFile)
{
	LPURLLIST lpUrlList;

		lpUrlList=(LPURLLIST)malloc(sizeof(URLLIST));  // ����һ���б���ṹ
		if (lpUrlList == NULL)
			return ;// ����ʧ��
		
		// ��������
		lpUrlList->next=NULL;
		lpUrlList->prev=NULL;
//		lpUrlList->lpUrl=SMIE_BufferAssignTChar(lpUrl);
		lpUrlList->lpUrl=BufferAssignTChar(lpUrl);
//		lpUrlList->lpLocateFile=SMIE_BufferAssignTChar(lpLocateFile);
		lpUrlList->lpLocateFile=BufferAssignTChar(lpLocateFile);
		// ������
		if (lpHtmlView->lpUrlHead==NULL)
		{
			lpHtmlView->lpUrlHead=lpHtmlView->lpCurUrl=lpUrlList;
		}
		else
		{
			SMIE_DeleteUrlList(lpHtmlView->lpCurUrl->next);  // ɾ����ǰ������Ľ��
			// �ѵ�ǰ���嵽���
			lpHtmlView->lpCurUrl->next=lpUrlList;
			lpUrlList->prev=lpHtmlView->lpCurUrl;
			lpHtmlView->lpCurUrl=lpUrlList;
		}
}
// **************************************************
// ������static void SMIE_DeleteUrlList(LPURLLIST lpUrlList)
// ������
// 	IN lpUrlList -- Ҫ���б���ɾ������ҳ
// 
// ����ֵ����
// �������������б���ɾ��һ��ָ������ҳ��
// ����: 
// **************************************************
static void SMIE_DeleteUrlList(LPURLLIST lpUrlList)
{
	LPURLLIST lpCur,lpNext;
	if (lpUrlList==NULL)
		return;
	lpCur=lpUrlList;
	while(lpCur)
	{
		lpNext=lpCur->next;  // �õ���һ�����
		// �ͷŽ��Ļ���
		if (lpCur->lpUrl)
			free(lpCur->lpUrl);  
		if (lpCur->lpLocateFile)
			free(lpCur->lpLocateFile);
		free(lpCur);
		lpCur=lpNext;  // ָ����һ�����
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
// ������void SMIE_SetCurPath(HWND hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- Ҫ���õ�ָ���ļ�
// 
// ����ֵ����
// ������������ָ���ļ�����Ŀ¼����Ϊ��ǰĿ¼��
// ����: 
// **************************************************
void SMIE_SetCurPath(HWND hWnd,LPTSTR lpUrl)
{
	char drive[MAX_PATH+8],dir[MAX_PATH];
	LPHTMLVIEW lpHtmlView;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
	if (lpHtmlView==NULL)
		return ;

//	GetCurrentDirectory(MAX_PATH+8,drive);
	_splitpath(lpUrl,drive,dir,NULL,NULL);   // ����·��
//	strcat(drive,dir);
	if (strlen(dir))
	{
		if (lpHtmlView->lpCurPath)
		{  // �ͷŵ�ǰ·���Ļ���
			free(lpHtmlView->lpCurPath);
			lpHtmlView->lpCurPath=NULL;
		}
//		lpHtmlView->lpCurPath=SMIE_BufferAssignTChar(dir);
		lpHtmlView->lpCurPath=BufferAssignTChar(dir);  // �������õ�ǰ·��
	}
//	if(strlen(drive))
//		SetCurrentDirectory(drive);
}

// **************************************************
// ������static void SMIE_DisplayMessage(UINT uMsg)
// ������
// 	IN uMsg -- Ҫ��ʾ�ı��
// 
// ����ֵ����
// ������������ʾһ��ָ���ı�š�
// ����: 
// **************************************************
static void SMIE_DisplayMessage(UINT uMsg)
{
	HDC hdc;
	char buffer[128];

		hdc=GetDC(NULL);  // �õ�HDC
		sprintf(buffer,"The Msg is %d  %08X",uMsg,uMsg);  // �õ��ִ�
		TextOut(hdc,10,10,buffer,strlen(buffer));  // ����ִ�
		ReleaseDC(NULL,hdc);  // �ͷ�HDC
}

// **************************************************
// ������static LPTSTR SMIE_GetFullUrl(HWND hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- ָ���ĵ�ַ
// 
// ����ֵ�����������ĵ�ַ
// �����������õ�ָ���ĵ�ַ��������ַ��
// ����: 
// **************************************************
static LPTSTR SMIE_GetFullUrl(HWND hWnd,LPTSTR lpUrl)
{
	char drive[MAX_PATH+8],dir[MAX_PATH];
	UINT iNewUrlLen;
	LPTSTR lpNewUrl;
	LPHTMLVIEW lpHtmlView;


	if (strlen(lpUrl) == 0)
		return BufferAssignTChar("");

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
	if (lpHtmlView==NULL)
		return 0;
	
	if (SMIE_IsLocateFile(hWnd,lpUrl))
	{  // �Ǳ����ļ�

	//	GetCurrentDirectory(MAX_PATH+8,drive);
		RETAILMSG(1, ("SMIE_GetFullUrl 11111 (%s)", lpUrl));
		_splitpath(lpUrl,drive,dir,NULL,NULL);  // ����·��
		RETAILMSG(1, ("SMIE_GetFullUrl 222 (dir:%s, ulr:%s) \r\n", dir, lpUrl));
		if (dir[0]=='\\' || dir[0] == '/')
		{  // ��ȫ·��
			// will the full directory
			if (lpHtmlView->lpCurPath)
			{  // ɾ��ԭ���ĵ�ǰ·��
				free(lpHtmlView->lpCurPath);
				lpHtmlView->lpCurPath=NULL;
			}
//			lpHtmlView->lpCurPath=SMIE_BufferAssignTChar(dir);
			RETAILMSG(1, ("URL:(%s) \r\n", lpUrl));
			lpHtmlView->lpCurPath=BufferAssignTChar(dir);  // �����µĵ�ǰ·��
			
//			return SMIE_BufferAssignTChar(lpUrl);
			return BufferAssignTChar(lpUrl);  // ���ص�ǰ��URL
		}
		else
		{  // ����ȫ·���ļ�
			// the Relative directory
	/*		if (lpHtmlView->lpCurPath)
			{
				free(lpHtmlView->lpCurPath);
				lpHtmlView->lpCurPath=NULL;
			}*/
	//		iNewpathLen=strlen(lpHtmlView->lpCurPath)+strlen(dir)+2;
			RETAILMSG(1, ("SMIE_GetFullUrl 333333333333"));
			iNewUrlLen=strlen(lpHtmlView->lpCurPath)+strlen(lpUrl)+2;  // �õ�����·������
        
			lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));  // ����·������

			if (lpNewUrl==NULL)
			{  // ����ʧ��
				return NULL;
			}
			strcpy(lpNewUrl,lpHtmlView->lpCurPath);  // ���Ƶ�ǰ·��
			strcat(lpNewUrl,lpUrl); // ��ӵ�ǰ·��

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
	{  // ���Ǳ����ļ�
		RETAILMSG(1, ("SMIE_GetFullUrl 444444444"));
				if (strnicmp(lpUrl,INTERNET_HEAD,5)==0)
				{ // Is "http:"
					lpUrl += 5;
				}
				while(1)
				{ // ɾ����ǰ��ĸ�
					if (*lpUrl == '\\' || *lpUrl == '/')
						lpUrl++;
					else
						break;
				}
				iNewUrlLen=strlen(INTERNET_HEAD)+strlen(lpUrl)+2;  // �õ�����URL����
        
				lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));  // ���仺��

				if (lpNewUrl==NULL)
				{  // ����ʧ��
					return NULL;
				}
				strcpy(lpNewUrl,INTERNET_HEAD);  // COPY ��http://�� to URL
				strcat(lpNewUrl,lpUrl);  // COPY ��ǰURL ���µ� URL

				return lpNewUrl;
	}
//	strcat(drive,dir);
//	if(strlen(drive))
//		SetCurrentDirectory(drive);
}

// **************************************************
// ������LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpImage -- Ҫ���ص�ͼ��
// 
// ����ֵ������ͼ���������ַ��
// �����������õ�Ҫ����ͼ���������ַ��
// ����: 
// **************************************************
LPTSTR SMIE_GetFullImage(HWND hWnd,LPTSTR lpImage)
{
	char drive[MAX_PATH+8],dir[MAX_PATH];
//	UINT iNewUrlLen;
	LPTSTR lpNewUrl,lpFileName;//,lpTemp;
	LPHTMLVIEW lpHtmlView;
	int i=0,iFileNameLength=0;

	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
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
	_splitpath(lpImage,drive,dir,NULL,NULL);  // ����·��
	if (dir[0]=='\\')
	{  // ��ȫ·��
//		return SMIE_BufferAssignTChar(lpImage);
		return BufferAssignTChar(lpImage);  // ֱ�Ӹ���
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
//			lpNewUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpImage)+4);  // �����ڴ�
//			if (lpNewUrl==NULL)
//				return FALSE;
// !!! Delete End By Jami chen in 2004.09.09
			if (strnicmp(lpImage,INTERNET_HEAD,5))
			{  //�����·��
				if (SMIE_IsLocateFile(hWnd,lpHtmlView->lpCurUrl->lpUrl))
				{  // �Ǳ����ļ�
					lpNewUrl=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpImage)+4);  // �����ڴ�
					if (lpNewUrl==NULL)
						return FALSE;
					strcpy(lpNewUrl,lpHtmlView->lpCurPath);  // �ȸ��Ƶ�ǰ·��
					strcat(lpNewUrl,lpImage);  // �������·��
					return lpNewUrl;
				}
				else
				{  // ����ҳ
					/*
					strcpy(lpNewUrl,lpHtmlView->lpCurUrl->lpUrl);  // ���Ƶ�ǰ��ҳ
					i=strlen(lpNewUrl);
					while(lpNewUrl[i]!='/') i--;
					lpNewUrl[i+1]=0;  // ��ӵ�ǰλ��
				//	strcat(lpFullFileName,"/");
					strcat(lpNewUrl,lpImage);
					*/
					lpNewUrl = MakeInternetRelativePath(lpHtmlView->lpCurUrl->lpUrl,lpImage);
				}
			}
			else
			{  // �Ǿ���·��
// !!! modified By Jami chen in 2004.09.09
//				strcpy(lpNewUrl,lpImage);
				lpNewUrl = BufferAssignTChar(lpImage);
// !!! modified End By Jami chen in 2004.09.09
			}
			if (lpNewUrl==NULL)
				return NULL;
			lpFileName = GetFileNameFromUrl(hWnd,lpNewUrl);  // �õ��ļ���
			free(lpNewUrl);
			return lpFileName;  // �����ļ���
	}
}
// **************************************************
// ������static BOOL SMIE_ApartMark(LPTSTR lpUrl,LPTSTR *lpPureUrl,LPTSTR *lpMark)
// ������
// 	IN lpUrl -- ָ������ҳ
// 	OUT lpPureUrl -- �����˱�ŵ���ҳ��ַ
// 	OUT lpMark -- ��������ı��
// 
// ����ֵ�����ط����˱�ŵ���ҳ��ַ
// ��������������ҳ��ַ���ŷ��롣
// ����: 
// **************************************************
static BOOL SMIE_ApartMark(LPTSTR lpUrl,LPTSTR *lpPureUrl,LPTSTR *lpMark)
{
	LPTSTR lpMarkPos;
	if (lpUrl==NULL)
	{  // URL Ϊ��
		*lpPureUrl=NULL;
		*lpMark=NULL;
		return FALSE;
	}
	// ���ҷָ�����#��
	lpMarkPos=lpUrl;
	while(*lpMarkPos)
	{
		if (*lpMarkPos=='#')
			break;
		lpMarkPos++;
	}
	// �õ����
	*lpMark=NULL;
	if (*lpMarkPos)
	{
		*lpMark=(LPTSTR)malloc(strlen(lpMarkPos+1)+1);  // �����ڴ�
		if (*lpMark)
		{
			strcpy(*lpMark,(lpMarkPos+1));
		}
	}
	// URLɾ�����
	*lpPureUrl=(LPTSTR)malloc(lpMarkPos-lpUrl+1);  // �����ڴ�
	if (*lpPureUrl)
	{
		strncpy(*lpPureUrl,lpUrl,(lpMarkPos-lpUrl));
		*(*lpPureUrl+(lpMarkPos-lpUrl))=0;
	}
	return TRUE;
}

// **************************************************
// ������BOOL SMIE_IsLocateFile(HWND hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- ָ������ҳ��ַ
// 
// ����ֵ���Ǳ����ļ�����TRUE�����򷵻�FALSE��
// ����������ָ������ҳ��ַ�Ƿ���һ�������ļ�
// ����: 
// **************************************************
BOOL SMIE_IsLocateFile(HWND hWnd,LPTSTR lpUrl)
{
	HANDLE in_file;

	if (*lpUrl == '\\' || *lpUrl == '/')
	{
		// �Ǹ�Ŀ¼
		in_file=CreateFile(lpUrl,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
		if (in_file==INVALID_HANDLE_VALUE)
			return FALSE;  // ���ܴ��ļ������ز����ڴ��ļ�
		CloseHandle(in_file);  // �ر��ļ����
		return TRUE;  // �Ǳ����ļ�
	}
	else
	{
		LPTSTR lpNewUrl;
		DWORD iNewUrlLen;
		LPHTMLVIEW lpHtmlView;

		if (strnicmp(lpUrl,INTERNET_HEAD,5)==0)
		{  
			// ��ǰ�ļ���һ������·��
			return FALSE;
		}

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)  
			return FALSE;

		// ����Ҫ�жϵ�ǰ�򿪵��ļ��Ƿ���һ�������ļ��������ǰ�򿪵�
		// ����һ�������ļ��������Ӧ���ļ�Ҳ��Ӧ����һ�������ļ�
		if (lpHtmlView->lpCurUrl)
		{
			if (lpHtmlView->lpCurUrl->lpUrl)
			{
				if (strcmp(lpUrl, lpHtmlView->lpCurUrl->lpUrl) != 0)
				{
					// ���ǵ�ǰ�򿪵���ҳ
					if (SMIE_IsLocateFile(hWnd,lpHtmlView->lpCurUrl->lpUrl) == FALSE)
					{
						return FALSE;
					}
				}
			}
		}
		// �õ�ȫ·��
		iNewUrlLen=strlen(lpHtmlView->lpCurPath)+strlen(lpUrl)+2;
    
		lpNewUrl=(LPTSTR)malloc(iNewUrlLen*sizeof(TCHAR));  // �����ڴ�

		if (lpNewUrl==NULL)
		{
			return NULL;
		}
		strcpy(lpNewUrl,lpHtmlView->lpCurPath);
		strcat(lpNewUrl,lpUrl);

		in_file = CreateFile(lpNewUrl,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);  // ���ļ�
		free(lpNewUrl);
		if (in_file==INVALID_HANDLE_VALUE)
			return FALSE;  // ���ļ�ʧ�ܣ����Ǳ����ļ�
		CloseHandle(in_file);  // �ر��ļ����
		return TRUE;  // �Ǳ����ļ�
	}
	return TRUE;
}

// **************************************************
// ������LPTSTR GetFileNameFromUrl(HWND hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- ָ������ҳ��ַ
// 
// ����ֵ�����صõ����ļ����ĵ�ַ
// ����������������ҳ��ַ�õ�Ҫ������ļ����ļ�����
// ����: 
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
	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
	if (lpHtmlView==NULL)
		return 0;
	strcpy(FileName,"\\��ʱ");  // �õ����ظ�Ŀ¼
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
	{  // �õ�Ŀ¼��
		FileName[i++]=*lpUrl++;
	}

	while(*lpUrl!=0&&*lpUrl!='/')
	{  // ���������ַ�
		*lpUrl++;
	}

	// �õ��ļ���
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
	{  // û�еõ��ļ���
		if (FileName[i-1] != '\\')
		{  // ���·������
			FileName[i++] = '\\';
			FileName[i] = 0;
		}
		strcat(FileName,"index.htm");  // ���Ĭ���ļ���
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
			{   // ���ļ����в����Գ��ֵ��ַ��á�_������
				if( FileName[i] == cNotAllowed[j] )
					FileName[i] = '_';
			}
		}
		i++;
	}
//	lpFileName=SMIE_BufferAssignTChar(FileName);
	lpFileName=BufferAssignTChar(FileName);  // �����ļ���


	return lpFileName;  // �����ļ���
}

// **************************************************
// ������BOOL CreateMultiDirectory(  LPCTSTR lpPathName,   LPSECURITY_ATTRIBUTES lpSecurityAttributes  )
// ������
// 	IN lpPathName -- Ҫ����Ŀ¼��ǰ·���ļ���
// 	IN lpSecurityAttributes -- Ҫ������Ŀ¼������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������ָ���ļ�����Ҫ��·����
// ����: 
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
		lpDirectory=(LPTSTR)malloc(size);  // ����·������
		if (lpDirectory==NULL)
		{
			return FALSE;
		}
		memset(lpDirectory,0,size);  // ��ʼ������
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
// ������static LRESULT DoGetFileType(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ�����ص�ǰ���ļ�������
// �����������õ���ǰ���ļ������ͣ�����HM_GETFILETYPE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetFileType(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return -1;

		if (lpHtmlView->lpCurFile == NULL)  // û�б����ļ�
			return -1;
		
		if (IsImageFile(lpHtmlView->lpCurFile))  // �Ƿ���ͼ���ļ�
			return IMAGEFILE;

		return HTMLFILE;
}
// !!! Add End By Jami chen in 2003.09.12

// !!! Add By Jami chen in 2003.09.13
// **************************************************
// ������static LRESULT DoGetLocateFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- �����С
// 	IN lParam -- ����ļ����Ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ����ҳ����Ӧ�ı����ļ����ļ���������HM_GETLOCATEFILE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetLocateFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	LPTSTR lpFileName;
	int iMaxLength;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		if (lpHtmlView->lpCurFile == NULL)  // û�б����ļ�
			return FALSE;

		iMaxLength = (int)wParam;
		lpFileName =(LPTSTR)lParam;
		if (iMaxLength <= (int)strlen(lpHtmlView->lpCurFile) || lpFileName == NULL)
		{  // ���治��
			return FALSE;
		}

		strcpy(lpFileName,lpHtmlView->lpCurFile);  // �����ļ���
		return TRUE;
}
// !!! Add End By Jami chen in 2003.09.13

// !!! Add By Jami chen in 2003.09.27
// **************************************************
// ������static LRESULT DoStop(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ����������ֹͣ��ǰ���ڴ򿪵���ҳ������HM_STOP��Ϣ��
// ����: 
// **************************************************
static LRESULT DoStop(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	LPDOWNLOADITEM lpDownLoadItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

//		RETAILMSG(1,("########### Stop Current Connect... \r\n"));
		lpHtmlView->bStop = TRUE;
//		RETAILMSG(1,("########### Delete Download List  \r\n"));
		DeleteDownList(hWnd);  // ɾ���ȴ������б�

//		RETAILMSG(1,("########### Stop Running List  \r\n"));
		CloseRunningList(hWnd);  // �ر����������б�
//		RETAILMSG(1,("#########################################################\r\n"));
		return FALSE;
}
// !!! Add End By Jami chen in 2003.09.27


// **************************************************
// ������static void WaitExitThread(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������ȴ��߳��˳�
// ����: 
// **************************************************
static void WaitExitThread(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return ;
//		RETAILMSG(1,("########### Will Exit All Thread... \r\n"));
//		lpHtmlView->bExit = TRUE;
		while(lpHtmlView->bExit)
		{  // �ȴ��˳�
			Sleep(100);
		}
//		RETAILMSG(1,("########### Had success Exit All Thread\r\n"));
}

// **************************************************
// ������static LRESULT SMIE_InterentEvnetHandler(HWND hWnd, WPARAM  wParam, LPARAM lParam  )
// ������
// 	IN  hWnd -- ���ھ��
// 	IN  wParam -- �¼�����
// 	IN  lParam -- ����
// 
// ����ֵ����
// ����������Internet�¼�������
// ����: 
// **************************************************
static LRESULT SMIE_InterentEvnetHandler(HWND hWnd, WPARAM  wParam, LPARAM lParam  )
{
	switch( wParam )
	{
	case INTERNET_STATUS_RESOLVING_NAME        :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "����Ѱ����ַ..." );
		break;
	case INTERNET_STATUS_NAME_RESOLVED         :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "�Ѿ��ҵ���ַ!" );
		break;

	case INTERNET_STATUS_CONNECTING_TO_SERVER  :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "����������ַ..." );
		break;
	case INTERNET_STATUS_CONNECTED_TO_SERVER   :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "�Ѿ����ӵ���ַ!" );
		break;

	case INTERNET_STATUS_SENDING_REQUEST       :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "���ڷ�������..." );
		break;
	case INTERNET_STATUS_REQUEST_SENT          :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "�����������!" );
		break;

	case INTERNET_STATUS_RECEIVING_RESPONSE    :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "���ڽ���Ӧ��..." );
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED     :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "Ӧ��������!" );
		break;

	case INTERNET_STATUS_CTL_RESPONSE_RECEIVED :
		break;
	case INTERNET_STATUS_PREFETCH              :
		break;

	case INTERNET_STATUS_CLOSING_CONNECTION    :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "���ڹر�����..." );
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED     :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "�����Ѿ��ر�!" );
		break;

	case INTERNET_STATUS_HANDLE_CREATED        :
		break;
	case INTERNET_STATUS_HANDLE_CLOSING        :
		break;

	case INTERNET_STATUS_REQUEST_COMPLETE      :
//		SetDlgItemText( hWnd, IDC_DOWNTIP, "�����Ѿ����!" );
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
// ������static LRESULT DoReAdjustSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �������������µ�����ǰ�ļ���λ�ã�����HM_READJUSTSIZE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoReAdjustSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	POINT ptOrg;
//	HDC hdc;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
		
		if (lpHtmlView->hHtml)
		{
// !!! modified By Jami chen in 2004.08.19
//			hdc=GetDC(hWnd);
//			if (lpHtmlView->iFontSize == FONT_SMALLLER)
//			{// ��С����
//				SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//			}
//			SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // �����С
//			ReleaseDC(hWnd,hdc);
			if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // �Ƿ���ͼ���ļ�, �����ͼ���ļ�������Ҫ���¼���ߴ�
				SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // ������Ҫ�ĳߴ�
// !!! modified end By Jami chen in 2004.08.19
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������
			SetCurrentScrollPos(hWnd,SB_HORZ);
			SetCurrentScrollPos(hWnd,SB_VERT);
		}

		// �ػ洰��
		ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
		ptOrg.y=lpHtmlView->y_Org;
	 	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
		InvalidateRect(hWnd,NULL,TRUE);

		return TRUE;
}


// **************************************************
// ������static LRESULT DoSetFontSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- (int)iFontSize; // ��ǰҪ���õ������С
// 	IN lParam -- (BOOL)bShow;  // �Ƿ�Ҫ������ʾ
// 
// ����ֵ����
// ����������������ʾ����Ĵ�С������HM_SETFONTSIZE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetFontSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
//	HDC hdc;
	int iFontSize;
	BOOL bShow;
	TEXTMETRIC TextMetric;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		iFontSize = (int)wParam;
		bShow = (BOOL)lParam;

		if ((iFontSize != FONT_BIGGER) && (iFontSize != FONT_SMALLLER))
		{
			// ����ʶ������
			return FALSE;
		}

		if (iFontSize == lpHtmlView->iFontSize)
		{
			// ����û�з����ı�
			return TRUE;
		}

		lpHtmlView->iFontSize = iFontSize; // �����µ�����
// !!! modified By Jami chen in 2004.08.19
//		hdc=GetDC(hWnd);
		if (lpHtmlView->iFontSize == FONT_SMALLLER)
		{// ��С����
//			SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
			SelectObject(lpHtmlView->hMemoryDC, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
		}
		else
		{  // �Ǵ�����
			SelectObject(lpHtmlView->hMemoryDC, GetStockObject( SYSTEM_FONT_CHINESE24X24 ) );
		}

		// ���µõ��ı��߶�
		GetTextMetrics(lpHtmlView->hMemoryDC,&TextMetric);
		lpHtmlView->iTextHeight = TextMetric.tmHeight;


		if (lpHtmlView->hHtml)
		{ // ��ǰ�д򿪵���ҳ������Ҫ������ʾ
			// Ĭ��Ϊ������
			if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // �Ƿ���ͼ���ļ�, �����ͼ���ļ�������Ҫ���¼���ߴ�
				SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // �����С

			lpHtmlView->x_Org=0;  // ��ʾԭ��ص�(0,0)
			lpHtmlView->y_Org=0;
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������

			if (bShow == TRUE)
			{
// !!! modified By Jami chen in 2004.08.19
//				InvalidateRect(hWnd,NULL,TRUE);
				ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
				ptOrg.y=lpHtmlView->y_Org;
	 			SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
				InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
			}
		}
//		ReleaseDC(hWnd,hdc);

		return TRUE;
}



// **************************************************
// ������static LRESULT DoSetDisplayMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- (int)iDisplayMode; // ��ǰҪ���õ���ʾģʽ
// 	IN lParam -- (BOOL)bShow;  // �Ƿ�Ҫ������ʾ
// 
// ����ֵ����
// ����������������ʾ����Ĵ�С������HM_SETFONTSIZE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetDisplayMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
//	HDC hdc;
	int iDisplayMode;
	BOOL bShow;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		iDisplayMode = (int)wParam;
		bShow = (BOOL)lParam;

		if ((iDisplayMode != DISPLAY_TEXTIMAGE) && (iDisplayMode != DISPLAY_ONLYTEXT))
		{
			// ����ʶ����ʾģʽ
			return FALSE;
		}

		if (iDisplayMode == lpHtmlView->iDisplayMode)
		{
			// ����û�з����ı�
			return TRUE;
		}

		lpHtmlView->iDisplayMode = iDisplayMode; // �����µ���ʾģʽ
		
		if (lpHtmlView->hHtml)
		{ // ��ǰ�д򿪵���ҳ������Ҫ������ʾ
// !!! modified By Jami chen in 2004.08.19
//			hdc=GetDC(hWnd);
//			if (lpHtmlView->iFontSize == FONT_SMALLLER)
//			{// ��С����
//				SelectObject(hdc, GetStockObject( SYSTEM_FONT_CHINESE16X16 ) );
//			}
//			SMIE_ReCalcSize(hWnd,hdc,lpHtmlView->hHtml);  // �����С
//			ReleaseDC(hWnd,hdc);
			if (IsImageFile(lpHtmlView->lpCurFile) == FALSE)  // �Ƿ���ͼ���ļ�, �����ͼ���ļ�������Ҫ���¼���ߴ�
				SMIE_ReCalcSize(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml);  // �����С
// !!! modified By Jami chen in 2004.08.19

			lpHtmlView->x_Org=0;  // ��ʾԭ��ص�(0,0)
			lpHtmlView->y_Org=0;
			SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������

			if (bShow == TRUE)
			{
// !!! modified By Jami chen in 2004.08.19
//				InvalidateRect(hWnd,NULL,TRUE);
				ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
				ptOrg.y=lpHtmlView->y_Org;
	 			SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
				InvalidateRect(hWnd,NULL,TRUE);
// !!! modified End By Jami chen in 2004.08.19
			}
		}

		return TRUE;
}



// ********************************************************************
// ������static HDC CreateMemoryDC(HWND hWnd)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ��������������һ���ڴ��豸���
// ����: 
// ********************************************************************
static HDC CreateMemoryDC(HWND hWnd)
{
	HDC hdc;
	RECT rect;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return NULL;
		
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		hdc = GetDC(hWnd); // �õ��豸���
		lpHtmlView->hMemoryDC=CreateCompatibleDC(hdc); // ���������ڴ��豸���
		if (lpHtmlView->hMemoryDC == NULL)
			return NULL;
		lpHtmlView->hMemoryBitmap=CreateCompatibleBitmap(hdc,(rect.right-rect.left),(rect.bottom-rect.top)); // ��������λͼ
		ReleaseDC(hWnd,hdc); /// �ͷ��豸���
		if (lpHtmlView->hMemoryBitmap == NULL)
		{ // ��������λͼʧ��
			DeleteDC(lpHtmlView->hMemoryDC);
			lpHtmlView->hMemoryDC= NULL;
			return NULL;
		}
		SelectObject(lpHtmlView->hMemoryDC,lpHtmlView->hMemoryBitmap); // ѡ��λͼ���ڴ��豸���

		FillRect(lpHtmlView->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // ������
		return lpHtmlView->hMemoryDC; // ���سɹ��������ڴ�DC
}

// ********************************************************************
// ������static void DrawToScreen(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN hdc	- �豸���
// ����ֵ��
//	
// ��������������һ���ڴ��豸���
// ����: 
// ********************************************************************
static void DrawToScreen(HWND hWnd,HDC hdc)
{
	LPHTMLVIEW lpHtmlView;
	RECT rect;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return ;

		GetClientRect(hWnd,&rect);
		BitBlt(hdc,0,0,rect.right,rect.bottom,lpHtmlView->hMemoryDC,0,0,SRCCOPY);
}


// ********************************************************************
// ������static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ��
//	
// �������������ڴ�С�����ı�
// ����: 
// ********************************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPHTMLVIEW lpHtmlView;
	HBITMAP hBitmap;
	RECT rect;
	HDC hdc;
	POINT ptOrg;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return 0;

		GetClientRect(hWnd,&rect);
		// ���ڴ�С�����ı䣬�ڴ�DC��λͼҲҪ�ı�
		hdc = GetDC(hWnd);
		hBitmap=CreateCompatibleBitmap(hdc,(rect.right-rect.left),(rect.bottom-rect.top)); // ��������λͼ
		ReleaseDC(hWnd,hdc); /// �ͷ��豸���
		if (hBitmap== NULL)
		{ // ��������λͼʧ��
			return 0;
		}
		lpHtmlView->hMemoryBitmap = hBitmap; 
		hBitmap = SelectObject(lpHtmlView->hMemoryDC,hBitmap); // ѡ����λͼ���ڴ��豸���
		if (hBitmap)
			DeleteObject(hBitmap);

		SMIE_ResetScrollBar(hWnd,lpHtmlView->hHtml);  // ���������
		SetCurrentScrollPos(hWnd,SB_HORZ);
		SetCurrentScrollPos(hWnd,SB_VERT);
		// �ػ洰��
		ptOrg.x=lpHtmlView->x_Org;  // �õ�VIEW��ԭ��
		ptOrg.y=lpHtmlView->y_Org;
	 	SMIE_ShowHtml(hWnd,lpHtmlView->hMemoryDC,lpHtmlView->hHtml,ptOrg);  // ����HTML
		InvalidateRect(hWnd,NULL,TRUE);
		
		return 0;
}


// ********************************************************************
// ������static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPWINDOWPOS ָ�� 
// ����ֵ��
//	
// ��������������λ�÷����ı�
// ����: 
// ********************************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpWindowPos;

		lpWindowPos = (LPWINDOWPOS)lParam;

		if ((lpWindowPos->flags & SWP_NOSIZE) == 0)
		{
			// ���ڴ�С�����ı�
			DoSize(hWnd,0,0);
		}

		return 0;
}

// ********************************************************************
// ������static void SetCurrentScrollPos(HWND hWnd,int fnBar)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// �������������õ�ǰ�Ĺ�����λ��
// ����: 
// ********************************************************************
static void SetCurrentScrollPos(HWND hWnd,int fnBar)
{
	SCROLLINFO ScrollInfo;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return ;

		ScrollInfo.cbSize=sizeof(SCROLLINFO);
		ScrollInfo.fMask=SIF_POS;

		if (fnBar == SB_VERT)
		{
			ScrollInfo.nPos=(lpHtmlView->y_Org+STEPHEIGHT-1)/STEPHEIGHT;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);  // ���ô�ֱ������������
		}
		else
		{
			ScrollInfo.nPos=(lpHtmlView->x_Org+STEPWIDTH-1)/STEPWIDTH;
			SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);  // ����ˮƽ����������
		}
}

// !!! Add By Jami chen in 2004.09.09
// ********************************************************************
// ������LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl)
// ������
// 	IN lpOpenUrl - ��ǰ�Ѿ��򿪵�INTERNET URL
//  IN lpRelativeUrl -- ��ǰҪ�򿪵���Ե�ַ
// ����ֵ��
//	
// �������������õ�ǰ�Ĺ�����λ��
// ����: 
// ********************************************************************
LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl)
{
	LPTSTR lpNewUrl;
	int i;
	int iPathlenght,iCurrentPos;
	int iProxyNameLenght; // ��ҳ�ĵ�ַ���ȣ�����ҳ�൱���ļ��ĸ�Ŀ¼
		
		if ( lpOpenUrl == NULL || lpRelativeUrl == NULL)
			return NULL;

		iPathlenght = strlen(lpOpenUrl)+strlen(lpRelativeUrl)+4;
		lpNewUrl=(LPTSTR)malloc(iPathlenght);  // �����ڴ�
		if (lpNewUrl==NULL)
			return NULL;

		memset(lpNewUrl,0,iPathlenght);

		iProxyNameLenght = strlen(INTERNET_HEAD); // ��http://�ĺ��濪ʼ��ѯ

		// �õ���ǰ����ַ�ĳ���
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
		// iProxyNameLenght ����������/

		// Add By Jami chen in 2004.12.27
		if (lpRelativeUrl[0] == '/')
		{
			// ȡ��ҳ��Ŀ¼�µ�����
			strncpy(lpNewUrl,lpOpenUrl,iProxyNameLenght);  // ���Ƶ�ǰ��ҳ
			lpNewUrl[iProxyNameLenght] = 0;
			strcat(lpNewUrl,lpRelativeUrl);
			return lpNewUrl;
		}
		// Add End Jami chen in 2004.12.27

		iPathlenght = strlen(lpOpenUrl); // �õ���ǰ�򿪵���ҳ�ĳ���
		while( lpOpenUrl[iPathlenght] != '/' ) 
		{
			iPathlenght--;
			if (iPathlenght < iProxyNameLenght)
				break;
		}

		if (iPathlenght < iProxyNameLenght)  
		{
			// ��ǰֻ��һ����ַ
			strncpy(lpNewUrl,lpOpenUrl,iProxyNameLenght);  // ���Ƶ�ǰ��ҳ
			strcat(lpNewUrl,"/");
		}
		else
		{
			i = iPathlenght;
			// ������û���ļ��ָ���
			while(1)
			{
				if (( lpOpenUrl[i] == 0))
				{
					// û���ҵ��ļ��ָ�������ǰ�򿪵���һ��Ŀ¼
					iPathlenght = i;
					strcpy(lpNewUrl,lpOpenUrl);  // ���Ƶ�ǰ��ҳ
					if (( lpOpenUrl[i-1] != '/'))
 						strcat(lpNewUrl,"/"); //  ���û��Ŀ¼�ָ���
					break;
				}
				if (lpOpenUrl[i] == '.' ) 
				{
					// ���ļ��ָ�������ǰ��������ַ��һ���ļ�
					strncpy(lpNewUrl,lpOpenUrl,iPathlenght);  // ���Ƶ�ǰ��ҳ
					strcat(lpNewUrl,"/");
					break;
				}
				i++;
			}
		}
		// ������
		while(1)
		{
			if (strncmp(lpRelativeUrl,"..\\",3) == 0  || strncmp(lpRelativeUrl,"../",3) == 0)
			{
				// ��Ҫ��λ����һ��Ŀ¼
				lpRelativeUrl += 3;

				iCurrentPos = strlen(lpNewUrl) -1;
				iCurrentPos --; // ���һ����Ŀ¼�ָ���������Ҫ��ǰһ��Ŀ¼�ָ���

				while( lpNewUrl[iCurrentPos] != '/' ) 
				{
					iCurrentPos--;
					if (iCurrentPos < iProxyNameLenght)
						break;
				}
				if (iCurrentPos < iProxyNameLenght)
				{
					// û���ҵ���һ��Ŀ¼
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
// ������LPTSTR MakeInternetRelativePath(LPTSTR lpOpenUrl,LPTSTR lpRelativeUrl)
// ������
// 	IN lpOpenUrl - ��ǰ�Ѿ��򿪵�INTERNET URL
//  IN lpRelativeUrl -- ��ǰҪ�򿪵���Ե�ַ
// ����ֵ��
//	
// �����������жϵ�ǰ�򿪵�URL�Ƿ���һ���û����Դ򿪵���ҳ
// ����: 
// ********************************************************************
static BOOL IsInternetUrl(LPTSTR lpUrl)
{
//	LPTSTR lpNewUrl;
	int i;
	int iPathlenght;
	int iProxyNameLenght; // ��ҳ�ĵ�ַ���ȣ�����ҳ�൱���ļ��ĸ�Ŀ¼
		
		if ( lpUrl == NULL )
			return FALSE;

		iProxyNameLenght = strlen(INTERNET_HEAD); // ��http://�ĺ��濪ʼ��ѯ

		// �õ���ǰ����ַ�ĳ���
		while( 1 )
		{
			if (lpUrl[iProxyNameLenght] == 0 ) 
			{  // ������һ����ַ
				return TRUE;
			}
			if (lpUrl[iProxyNameLenght] == '/' ) 
			{
				break;
			}
			iProxyNameLenght ++;
		}
		// iProxyNameLenght ����������/

		iPathlenght = strlen(lpUrl); // �õ���ǰ�򿪵���ҳ�ĳ���
		while( lpUrl[iPathlenght] != '/' ) 
		{
			iPathlenght--;
			if (iPathlenght < iProxyNameLenght)
				break;
		}

		if (iPathlenght < iProxyNameLenght)  
		{
			// ��ǰֻ��һ����ַ
			return TRUE;
		}
		else
		{
			// iPathlenght �����һ��/
			i = iPathlenght;
			// ������û���ļ��ָ���
			while(1)
			{
				if (( lpUrl[i] == 0))
				{
					// û���ҵ��ļ��ָ�������ǰ�򿪵���һ��Ŀ¼
					return TRUE;
				}
				if (lpUrl[i] == '.' ) 
				{
					// ���ļ��ָ�������ǰ��������ַ��һ���ļ�
					LPTSTR lpExt;

					lpExt = lpUrl + i + 1;
					if (stricmp(lpExt,"htm") == 0 ||
						stricmp(lpExt,"html") == 0 ||
						stricmp(lpExt,"shtml") == 0 ||
						stricmp(lpExt,"asp") == 0 ||
						strnicmp(lpExt,"htm?",4) == 0 ||  //��������Ϊ��ĳ����ҳ��������
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
