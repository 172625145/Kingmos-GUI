/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��Ļģʽ��IE�����,�ļ�����
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-05-25
���ߣ��½��� JAMI
�޸ļ�¼��
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
#include <eprogres.h>

#ifdef ZT_PHONE
#include <ztmsgbox.h>
#include <thirdpart.h>
#endif

#include <ecommdlg.h>
#include <dialdlg.h>
#include <inetstr.h>

#ifndef ZT_PHONE
#define WORK_AREA_HEIGHT   200
#define WORK_AREA_WIDTH    240
#define WORK_AREA_STARTX   0
#define WORK_AREA_STARTY   60
#endif


//#define NOINTERNET
// ʹ��һ�������̣߳����ļ���Ҫ���أ��򴴽�һ���߳�������ָ�����ļ����ļ�������ɣ��߳̽���
// #define DOWNLOAD_PROCESS1   
// ʹ��һ�������߳��������ļ������̺߳͵ȴ��߳̽����������߳��������Ƿ����ļ���Ҫ����
// ������ļ���Ҫ���أ������ظ��ļ�������ȴ���Ҫ���ص��ļ�
#define DOWNLOAD_PROCESS2

// **************************************************
// ��������
// **************************************************

static BOOL InsertDownList(	HWND hWnd,LPTSTR lpUrl,HHTMCTL hControl,LPTSTR lpFileName);

// !!! Add By Jami chen in 2003.09.13
static DWORD DownLoadAFile(HWND hWnd, LPDOWNLOADITEM lpDownloadItem);
static DWORD WINAPI DownLoadThread(VOID * pParam);
DWORD WINAPI DownLoadmanageThread(VOID * pParam);
// !!! Add End By Jami chen in 2003.09.13

BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl);
BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl);
BOOL CloseRunningList(	HWND hWnd);

#ifdef LOADANIMATION
static void StartAnimation(HWND hWnd);
static void EndAnimation(HWND hWnd);
static DWORD WINAPI LoadAnimateProc(VOID * pParam);
static void ShowAnimation(HWND hWnd, int iPic);
#endif


static BOOL InitialInternet(HWND hWnd);

static int GetCurrentUrlSize(LPURLHANDLE hCurrentUrl);


// *****************************************************
// ���ش��ں�������������
// *****************************************************
#define DPM_CLOSE			(WM_USER + 1235)  // �رմ���
#define DPM_SETURL			(WM_USER + 1236)  // ��ǰҪ���ص�URL
#define DPM_SETSIZE			(WM_USER + 1237)  // ��ǰҪ���ص��ļ�����
#define DPM_SETDOWNLOADSIZE	(WM_USER + 1238)  // �Ѿ����صĳ���
#define DPM_SETSTATUS		(WM_USER + 1239)  // ��ǰҪ���ص�״̬

#define DOWNLOADSTATUS_OPENURL		100
#define DOWNLOADSTATUS_DOWNLOAD		101
#define DOWNLOADSTATUS_COMPLETE		102

static HWND ShowDownloadDialog(HWND hWnd);
static void CloseCopyDialog(HWND hDownloadWnd);
static BOOL GetSaveFile( HWND hwnd,CHAR *szDefault,CHAR *szFileName);

#define DIALOG_DOWNLOAD  // Ҫ��ʾ���صĶԻ���
#define AUTO_LOGIN  // Ҫ�Զ���½

//static HINTERNET IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext );
static LPURLHANDLE IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext );
static void CloseUrlHandle( LPURLHANDLE lpUrlHandle ,LPTSTR lpDump);


//static	const	char	g_szaccept[] = "Accept: image/gif, image/jpeg, */*";
//static	const	char	g_szaccept[] = "Accept: */*";
static	const	char	g_szacceptQuest[] = "*/*";

// *****************************************************

// **************************************************
// ������void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- һ��ָ����վ��
// 
// ����ֵ����
// ������������ʼ��һ���µ�վ��
// ����: 
// **************************************************
void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl)
{
#ifdef AUTO_LOGIN
	LPHTMLVIEW  lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
		{
			return ;
		}
#if 0
		// ����ҳ֮ǰ����Ҫ���ŵ�����
		if (DailToInternet(hWnd,&lpHtmlView->hRasConn,GPRS_NETTYPE_INTERNET) == FALSE) // ���ŵ�INTERNET
		{
			// �������ӵ�����
#ifdef ZT_PHONE
			ZTMessageBox(hWnd,"�������ӵ�����","����",MB_OK);
#else
			MessageBox(hWnd,"�������ӵ�����","����",MB_OK);
#endif
			return ;
		}
#else
		if (LinkNet(NULL,GPRS_NETTYPE_INTERNET) == FALSE) // ���ŵ�INTERNET
		{
			return ;
		}
#endif
#endif
//		RETAILMSG(1,("Sucess Link to Internet [%x]\r\n",lpHtmlView->hRasConn));
		InsertDownList(hWnd,lpUrl,NULL,NULL);  // ���뵱ǰURL�������б���ʼ����
}
// **************************************************
// ������static  void SMIE_DownLoadUrl( HWND  hWnd,HHTMCTL hHtmlCtl,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hHtmlCtl -- �������������ĵ�
// 	IN lpUrl -- Ҫ���صĵ�ַ
// 
// ����ֵ����
// ��������������һ��ָ���ĵ�ַ����
// ����: 
// **************************************************
static  void SMIE_DownLoadUrl( HWND  hWnd,HHTMCTL hHtmlCtl,LPTSTR lpUrl)
{
	InsertDownList(hWnd,lpUrl,hHtmlCtl,NULL);  // ����ǰURL��ӵ������б�
}


// **************************************************
// ������static DWORD DownLoadAFile(HWND hWnd, LPDOWNLOADITEM lpDownloadItem)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- Ҫ���صĵ�ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ��������������һ��ָ�����ļ���
// ����: 
// **************************************************
//static DWORD DownLoadAFile(HWND hWnd, HINTERNET hUrl)
static DWORD DownLoadAFile(HWND hWnd, LPDOWNLOADITEM lpDownloadItem)
{
//		HINTERNET   hCurrentUrl;
		LPURLHANDLE   hCurrentUrl;
		DWORD       dwNumRet = 1;
		DWORD       dwWriteRet,dwDownloadSize,dwUrlSize;
		char cBuf[2048];
		HANDLE      hFile;
		LPHTMLVIEW  lpHtmlView;
//		HINTERNET   hUrl;
		LPTSTR		lpUrl;
		LPTSTR		lpFileName = NULL;
//		int iRepeatGetUrl;
//		int i;
//		HHTMCTL hControl;
		BOOL bRet = TRUE;
#ifdef DIALOG_DOWNLOAD
		HWND hDownload = NULL;
#endif
		BOOL bNeedDownLoad = TRUE;


//		KillTimer(hWnd,INTERNET_EVENT);
//		RETAILMSG(1,("receive INTERNET_EVENT_SERVER_REQUEST_SUCESS!!!\r\n"));
		//RETAILMSG(1,("###########################Down load Url <%X>!!!\r\n",hUrl));

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
		{
//		    CloseUrlHandle( hUrl );
			return 0;
		}
		lpUrl = lpDownloadItem->lpUrl;
		if (lpUrl == NULL)  // URLΪ�գ�����Ҫ����
			return 0;
#ifdef NOINTERNET
		return 0;
#endif
//		RETAILMSG(1,("#####################Will Down load <%s>\r\n",lpUrl));
		if (lpDownloadItem->lpFileName == NULL)
		{
			lpFileName=GetFileNameFromUrl(hWnd,lpUrl);  // ����URL�õ���ʱ�ļ���
		}
		else
		{
			lpFileName = lpDownloadItem->lpFileName;
		}
		CreateMultiDirectory(lpFileName,NULL);  // ����Ŀ¼

		if (lpDownloadItem->lpFileName == NULL)
		{
			if (lpHtmlView->lpUrl==NULL || strcmp(lpUrl,lpHtmlView->lpUrl)!=0 )
			{
				// ������ҳ
				hFile=CreateFile( lpFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,NULL, 
										 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					// �ļ��Ѿ�����
					CloseHandle(hFile);
					bNeedDownLoad = FALSE;
					bRet = TRUE;
				}
			}
		}

		if (bNeedDownLoad)
		{
			// ��Ҫ���ص�ǰ�ļ�
	//	GetFileNameFromUrl(hWnd,lpUrl);

#ifdef DIALOG_DOWNLOAD
		if (lpDownloadItem->lpFileName || lpHtmlView->lpUrl!=NULL )
		{
			if (lpDownloadItem->lpFileName || strcmp(lpUrl,lpHtmlView->lpUrl)==0 )
			{  
				// �ж��ǵ�ǰ����ҳ
				hDownload = ShowDownloadDialog(hWnd);  // ��ʾ���ضԻ���
				if (hDownload)
				{
					PostMessage(hDownload,DPM_SETURL,0,(LPARAM)lpUrl); // ����Ҫ���ص�URL
//					PostMessage(hDownload,DPM_SETSIZE,0,0); // ����Ҫ���ص�URL
				}
			}
		}
#endif
		if( ! lpHtmlView->hInternetSiteInit )
		{  
			if (InitialInternet(hWnd) == FALSE)
			{

#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
			}
#endif

				return 0;
			}
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_SETSTATUS,DOWNLOADSTATUS_OPENURL,0); // �������ڴ򿪵�URL
			}
#endif
			// ��INTERNET
			lpHtmlView->hInternetSiteInit =  InternetOpen( "Kingmos NE/2.0",INTERNET_OPEN_TYPE_DIRECT,
										 NULL, NULL, INTERNET_INVALID_PORT_NUMBER ); 
//			RETAILMSG(1,("InternetOpen = %x OK!!!\r\n",lpHtmlView->hInternetSiteInit));
			InternetSetStatusCallback(  lpHtmlView->hInternetSiteInit, hWnd , 0 );  // ���ûص�����
		}

//		RETAILMSG(1,("#####################InternetOpen OK !!!\r\n"));

	//	SetTimer(hWnd,INTERNET_EVENT,100,NULL);

		// ��һ��URL
//		hCurrentUrl=InternetOpenUrl( lpHtmlView->hInternetSiteInit,lpUrl,NULL, 0, 0,(DWORD)NULL);
//		hCurrentUrl = InternetOpenUrl( lpHtmlView->hInternetSiteInit, lpUrl, 0, 0, 0, (DWORD)NULL );
//		hCurrentUrl = InternetOpenUrl( lpHtmlView->hInternetSiteInit, lpUrl, g_szaccept, -1, 0, (DWORD)NULL );
		hCurrentUrl = IE_OpenUrl( hWnd,lpHtmlView->hInternetSiteInit, lpUrl, g_szacceptQuest, -1, 0, (DWORD)NULL );
		if (hCurrentUrl==NULL)
		{   // can't download file to locate machine , so no locate file to allude this url
	//		KillTimer(hWnd,INTERNET_EVENT);
			RETAILMSG(1,("************Download  <%s> Failure (Can't Open Url)\r\n",lpUrl));
/*			if (lpDownLoadparam->bRefresh)
			{
				if (lpHtmlView->lpCurFile)
					free(lpHtmlView->lpCurFile);
				lpHtmlView->lpCurFile=NULL;
				PostMessage(hWnd,HM_REFRESH,0,0);
			}
			*/

#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
			}
#endif

			return 0;
		}
		//RETAILMSG(1,("#####################InternetOpenUrl <%X>OK !!!\r\n",hCurrentUrl));
		//RETAILMSG(1,("#####################InternetOpenUrl hOpenCnn=<%X>OK !!!\r\n",hCurrentUrl->hOpenCnn));
		//RETAILMSG(1,("#####################InternetOpenUrl hOpenReq=<%X>OK !!!\r\n",hCurrentUrl->hOpenReq));

		if (lpHtmlView->bStop)
		{  // ֹͣ����
			DeleteRunningList(	hWnd,hCurrentUrl);  // ɾ���Ѿ�������ɵ�URL
//		    CloseUrlHandle( hCurrentUrl ,"ttt1");

#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
			}
#endif

			return 0;
		}

		{
			DWORD			dwStatusHttp;
			DWORD			dwStatusHttp_class;
			DWORD			dwContentLen;
			char			pszContentType[500];
			DWORD			dwIndex;
			BOOL bRet ;

RETRY_QUERY:
			//RETAILMSG(1,("HttpQueryQuick 0 hOpenReq =<%X>\r\n",hCurrentUrl->hOpenReq));
			if( !HttpQueryQuick( hCurrentUrl->hOpenReq, &dwStatusHttp, &dwContentLen, pszContentType, sizeof(pszContentType), &dwIndex ) )
			{
				DeleteRunningList(	hWnd,hCurrentUrl);  // ɾ���Ѿ�������ɵ�URL
	//			CloseUrlHandle( hCurrentUrl ,"ttt2");
				hCurrentUrl = NULL;
				//RETAILMSG(1,(TEXT("HttpQueryQuick [%s] Failure\r\n"), lpUrl));
#ifdef DIALOG_DOWNLOAD
				if (hDownload)
				{
					PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
				}
#endif
				return 0;
			}
			//RETAILMSG(1,("HttpQueryQuick 0 hOpenReq =<%X> OK\r\n",hCurrentUrl->hOpenReq));
			dwStatusHttp_class = dwStatusHttp / 100;
			//
			switch( dwStatusHttp_class )
			{
			case 2:
				//��ȡ�ɹ�
				break;
			case 3:
				//��Ҫ�ض���,�õ��ض����ַ
				dwContentLen = sizeof(pszContentType);
				//RETAILMSG(1,("HttpQueryQuick 1 hOpenReq =<%X>\r\n",hCurrentUrl->hOpenReq));
				bRet = HttpQueryInfo(hCurrentUrl->hOpenReq, HTTP_QUERY_LOCATION,(LPVOID)pszContentType, &dwContentLen,&dwIndex);
				//RETAILMSG(1,("HttpQueryQuick 1 hOpenReq =<%X> OK\r\n",hCurrentUrl->hOpenReq));
				// ֹͣ��ǰ������
				DeleteRunningList(	hWnd,hCurrentUrl);  // ɾ���Ѿ�������ɵ�URL
	//			CloseUrlHandle( hCurrentUrl ,"ttt3");
				hCurrentUrl = NULL;
				if (bRet == TRUE)
				{
					//RETAILMSG(1,("Need Rediert\r\n"));
					if ((lpHtmlView->lpUrl!=NULL) && strcmp(lpUrl,lpHtmlView->lpUrl)==0)
					{
						// ����ҳ, ���´�һ���µ���ҳ
						SendMessage(hWnd,HM_GO,0,(LPARAM)pszContentType);
					}
					else
					{
						// ���������µĵ�ַ����
						hCurrentUrl = IE_OpenUrl( hWnd,lpHtmlView->hInternetSiteInit, pszContentType, g_szacceptQuest, -1, 0, (DWORD)NULL );
						if (hCurrentUrl != NULL)
						{
							// ���»�õ�ַ�ɹ�,���²�ѯ��ǰ��״��
							goto RETRY_QUERY;
						}
					}
				}
				//RETAILMSG(1,(TEXT("HttpQueryQuick Failure\r\n"), lpUrl));
#ifdef DIALOG_DOWNLOAD
				if (hDownload)
				{
					PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
				}
#endif
				return TRUE;
			default :
				//�д������
				DeleteRunningList(	hWnd,hCurrentUrl);  // ɾ���Ѿ�������ɵ�URL
	//			CloseUrlHandle( hCurrentUrl ,"ttt4");
				hCurrentUrl = NULL;
				//RETAILMSG(1,(TEXT("HttpQueryQuick [%s] Failure\r\n"), lpUrl));
#ifdef DIALOG_DOWNLOAD
				if (hDownload)
				{
					PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
				}
#endif
				return 0;
			}
		}

#ifdef DIALOG_DOWNLOAD
		if (hDownload)
		{
			PostMessage(hDownload,DPM_SETSTATUS,DOWNLOADSTATUS_DOWNLOAD,0); // �������ڴ򿪵�URL
		}
		dwUrlSize = GetCurrentUrlSize(hCurrentUrl);
		if (hDownload)
		{
			PostMessage(hDownload,DPM_SETSIZE,dwUrlSize,0); // ����Ҫ���ص�URL
		}
#endif
		// ���뵱ǰ���ع��̵����������ļ��б�
//		InsertRunningList(	hWnd,hCurrentUrl);

//		RETAILMSG(1,("********************* jami 2003.09.25 Start ******************\r\n"));

		//RETAILMSG(1,("###########################Start Down load Url <%s>!!!\r\n",lpUrl));
/*		if (lpDownloadItem->lpFileName == NULL)
		{
			lpFileName=GetFileNameFromUrl(hWnd,lpUrl);  // ����URL�õ���ʱ�ļ���
		}
		else
		{
			lpFileName = lpDownloadItem->lpFileName;
		}

		CreateMultiDirectory(lpFileName,NULL);  // ����Ŀ¼
*/
//		lpFileName=TEMPFILE;
		// ���ļ�
		hFile=CreateFile( lpFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,NULL, 
								 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
		{
//			RETAILMSG(1,("##################Down Load <%s>Data Failure!!!\r\n",lpUrl));
			//RETAILMSG(1,("##################Create File <%s> Failure!!!\r\n",lpFileName));
			// ���ļ�ʧ��
			if (lpDownloadItem->lpFileName == NULL)
			{
				if (lpFileName)
					free(lpFileName);
			}
			DeleteRunningList(	hWnd,hCurrentUrl);  // �������б���ɾ��
//		    CloseUrlHandle( hCurrentUrl ,"ttt5"); // ֹͣ��ǰURL����
//			DeleteDownList(hWnd,hUrl);
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
			}
#endif
			return 0;
		}

//		RETAILMSG(1,("Create Temp File <%s> OK!!!\r\n",lpFileName));
		//RETAILMSG(1,("Begin Down Load Data .......\r\n"));
//#ifdef LOADANIMATION
//		StartAnimation(hWnd);
//#endif
		// ��ʼ��������
		dwDownloadSize = 0;
		while( dwNumRet )
		{
			 //RETAILMSG(1,(TEXT("Read Data From [%x]\r\n"),hCurrentUrl->hOpenReq));
			 if( !InternetReadFile( hCurrentUrl->hOpenReq,cBuf,sizeof( cBuf) , &dwNumRet ) )
			 {  // ��INTERNET ��ȡ����
				 RETAILMSG(1,(TEXT("Read Data From [%x] Failure\r\n"),hCurrentUrl->hOpenReq));
				 bRet = FALSE;
 				 break;
			 }
			 //RETAILMSG(1,(TEXT("Read Data From [%x] OK\r\n"),hCurrentUrl->hOpenReq));
			 if( ! dwNumRet )
			 {
				 if (dwUrlSize)
				 {
					 RETAILMSG(1,("Down load Data Failure <%d> -- > <%d>!!!\r\n",dwUrlSize,dwDownloadSize));
//					 bRet = FALSE;
				 }
				 break;
			 }

			 WriteFile( hFile, cBuf, dwNumRet,&dwWriteRet,NULL);  // ������д�������ļ���
			 dwDownloadSize += dwNumRet;
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_SETDOWNLOADSIZE,dwDownloadSize,0); // ����Ҫ���ص�URL
			}
#endif
			 if (dwUrlSize == dwDownloadSize)
				 break;
		}
		//RETAILMSG(1,("InternetReadFile OK\r\n"));
		SetEndOfFile(hFile);
		CloseHandle(hFile);  // �ر��ļ�
#ifdef DIALOG_DOWNLOAD
		if (hDownload)
		{
			PostMessage(hDownload,DPM_CLOSE,0,0); // �رնԻ���
		}
#endif

//#ifdef LOADANIMATION
//		EndAnimation(hWnd);
//#endif
//		RETAILMSG(1,("##################Down Load <%s>Data OK!!!\r\n",lpUrl));
//		RETAILMSG(1,("********************* jami 2003.09.25 End ******************\r\n"));
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_SETSTATUS,DOWNLOADSTATUS_COMPLETE,0); // �������ڴ򿪵�URL
			}
#endif
		DeleteRunningList(	hWnd,hCurrentUrl);  // ɾ���Ѿ�������ɵ�URL
//	    CloseUrlHandle( hCurrentUrl ,"ttt6");  // �رվ��
		}  
		//RETAILMSG(1,("Download Complete\r\n"));

		if (bRet == TRUE && lpHtmlView->lpUrl!=NULL)
		{  // ���سɹ�
			//RETAILMSG(1,("Download Success\r\n"));
			if (strcmp(lpUrl,lpHtmlView->lpUrl)==0)
			{  // �ж��ǵ�ǰ����ҳ
				if (lpHtmlView->lpCurFile)
					free(lpHtmlView->lpCurFile);  // ɾ����ǰ�ļ�������
//				lpHtmlView->lpCurFile=SMIE_BufferAssignTChar(lpFileName);
				lpHtmlView->lpCurFile=BufferAssignTChar(lpFileName);  // �����Ѿ������������ļ�Ϊ��ǰ�ļ�
				SMIE_SetCurPath(hWnd,lpHtmlView->lpCurFile);  // ���õ�ǰ·��
//				RETAILMSG(1,("M000MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
				PostMessage(hWnd,HM_REFRESH,0,0);  // ˢ�´���
//				RETAILMSG(1,("Load File Time = %d\r\n",GetTickCount() - iStartTick));
//				RETAILMSG(1,("DownLoad  \"%s\" Complete !!!\r\n",lpHtmlView->lpUrl));
//				RETAILMSG(1,("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
//				Sleep(2000);
				//RETAILMSG(1,("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\r\n"));
			}
		}
// !!! Delete By Jami chen in 2004.09.23
		else 
		{ // ������ҳ��Ҫ��֤�ļ��������ԣ�û��������ɣ�ɾ��ԭ���ļ�
			//RETAILMSG(1,("Download Failure\r\n"));
			if (dwUrlSize != dwDownloadSize)
			{
				// ����ʧ��
				//RETAILMSG(1,("Delete File [%s]\r\n",lpFileName));
				DeleteFile(lpFileName);
			}
		}
// !!! Delete End By Jami chen in 2004.09.23
//				hControl = GetControl(hWnd,hUrl);
		if (bRet == TRUE && lpDownloadItem->hControl)
		{  // ���ؿؼ�(ͼ��)�ɹ�
			//RETAILMSG(1,("Download Control OK\r\n"));
			DownloadImageOK(hWnd, lpDownloadItem->hControl,lpFileName); 
		}
/*
		hControl = GetControl(hWnd,hUrl);
		if (hControl)
		{
			RECT rect={0,0,0,0};
			HDC hdc;
			POINT ptOrg;

				ptOrg.x = lpHtmlView->x_Org;
				ptOrg.y = lpHtmlView->y_Org;
				hdc = GetDC(hWnd);
				SMIE_ShowHtmlControl(hWnd,hdc,hControl,rect,ptOrg);
				ReleaseDC(hWnd,hdc);
		}
		*/
		// had download complete,delete this url from download list
//		DeleteDownList(hWnd);
		// close this download handle
//		DeleteRunningList(	hWnd,hCurrentUrl);  // ɾ���Ѿ�������ɵ�URL
//	    CloseUrlHandle( hCurrentUrl );  // �رվ��
//		if (lpHtmlView->lpCurFile)
//			free(lpHtmlView->lpCurFile);
//		lpHtmlView->lpCurFile=SMIE_BufferAssignTChar(lpFileName);
		//RETAILMSG(1,("Over\r\n"));
		if (lpDownloadItem->lpFileName == NULL)
		{
			if (lpFileName)
				free(lpFileName);
		}
//		PostMessage(hWnd,HM_REFRESH,0,0);
		//RETAILMSG(1,("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\r\n"));
		return bRet;
}

// **************************************************
// ������static BOOL InsertDownList(	HWND hWnd,LPTSTR lpUrl,HHTMCTL hControl,LPTSTR lpFileName)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpUrl -- Ҫ���صĵ�ַ
// 	IN hControl -- Ҫ���صĵ�ַ�����ĵ�
//  IN lpFileName -- Ҫ�����ص��ļ����ص�ָ�����ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�TRUE��
// ��������������һ��Ҫ���صĵ�ַ�������б��С�
// ����: 
// **************************************************
//static BOOL InsertDownList(	HWND hWnd,HINTERNET hUrl ,LPTSTR lpUrl,HHTMCTL hControl)
static BOOL InsertDownList(	HWND hWnd,LPTSTR lpUrl,HHTMCTL hControl,LPTSTR lpFileName)
{
	LPDOWNLOADITEM lpDownLoadItem;
	LPHTMLVIEW lpHtmlView;


//		RETAILMSG(1,("********************* jami 2003.09.25 Start ******************\r\n"));
//		RETAILMSG(1,("Will Insert Url <%s> --> <%X>!!!\r\n",lpUrl,hUrl));
//		RETAILMSG(1,("########### Insert a Url<%s> to Download list\r\n",lpUrl));

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
	
		LOCK(lpHtmlView->DownLoadCriticalSection);  // ����
//		RETAILMSG(1,("Copy Url <%s>!!!\r\n",lpUrl));
		lpDownLoadItem=(LPDOWNLOADITEM)malloc(sizeof(DOWNLOADITEM));  // ����DOWNLOADITEM�ڴ�
		if (lpDownLoadItem==NULL)
		{  // ����ʧ��
			UNLOCK(lpHtmlView->DownLoadCriticalSection);
			return FALSE;
		}
		
		// ���ṹ��ֵ
//		RETAILMSG(1,("Copy Url <%s>!!!\r\n",lpUrl));
//		lpDownLoadItem->hUrl=hUrl;
		lpDownLoadItem->hControl = hControl;
//		lpDownLoadItem->lpUrl=SMIE_BufferAssignTChar(lpUrl);
		lpDownLoadItem->lpUrl=BufferAssignTChar(lpUrl);
//		RETAILMSG(1,("Copy Url <%s> --> <%s>!!!\r\n",lpUrl,lpDownLoadItem->lpUrl));
		lpDownLoadItem->next=lpHtmlView->lpDownLoadList;
		lpDownLoadItem->lpFileName = BufferAssignTChar(lpFileName);
		lpHtmlView->lpDownLoadList=lpDownLoadItem;
//		RETAILMSG(1,("Had Insert Url <%s> --> <%X>!!!\r\n",lpDownLoadItem->lpUrl,lpDownLoadItem->hUrl));
//		RETAILMSG(1,("The Download handle <%x>!!!\r\n",lpDownLoadItem->hUrl));
//		RETAILMSG(1,("********************* jami 2003.09.25 End ******************\r\n"));
//#ifdef LOADANIMATION
//		StartAnimation(hWnd);
//#endif
		SetEvent(lpHtmlView->hDownloadEvent);
		UNLOCK(lpHtmlView->DownLoadCriticalSection);  // ����
		return TRUE;		
}/*
static LPTSTR GetDownLoadUrl(HWND hWnd,HINTERNET  hUrl)
{
	LPDOWNLOADITEM lpDownLoadItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);
		if (lpHtmlView==NULL)
			return FALSE;

		lpDownLoadItem=lpHtmlView->lpDownLoadList;
		while(lpDownLoadItem)
		{
			if (lpDownLoadItem->hUrl==hUrl)
			{  // will delete current node
//				RETAILMSG(1,("Get Url <%s>!!!\r\n",lpDownLoadItem->lpUrl));
				return lpDownLoadItem->lpUrl;		
			}
			lpDownLoadItem=lpDownLoadItem->next;
		}
		return NULL;
}
static HHTMCTL GetControl(HWND hWnd,HINTERNET  hUrl)
{
	LPDOWNLOADITEM lpDownLoadItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);
		if (lpHtmlView==NULL)
			return FALSE;

		lpDownLoadItem=lpHtmlView->lpDownLoadList;
		while(lpDownLoadItem)
		{
			if (lpDownLoadItem->hUrl==hUrl)
			{  // will delete current node
//				RETAILMSG(1,("Get Url <%s>!!!\r\n",lpDownLoadItem->lpUrl));
				return lpDownLoadItem->hControl;		
			}
			lpDownLoadItem=lpDownLoadItem->next;
		}
		return NULL;
}
static BOOL DeleteDownList(	HWND hWnd,HINTERNET  hUrl)
{
	LPDOWNLOADITEM lpDownLoadItem,lpPreItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);
		if (lpHtmlView==NULL)
			return FALSE;

//		if (lpDownLoadList==NULL)
//			return FALSE;
		lpDownLoadItem=lpHtmlView->lpDownLoadList;
		lpPreItem=NULL;
		while(lpDownLoadItem)
		{
			if (lpDownLoadItem->hUrl==hUrl)
			{  // will delete current node
				if (lpPreItem==NULL)
				{  // this is first node
					lpHtmlView->lpDownLoadList=lpDownLoadItem->next;
				}
				else
				{// will set prenode next link current node next
					lpPreItem->next=lpDownLoadItem->next;
				}
				// delete current node
				if (lpDownLoadItem->lpUrl)
					free(lpDownLoadItem->lpUrl);
				free(lpDownLoadItem);
				if (lpHtmlView->lpDownLoadList == NULL)
				{
		#ifdef LOADANIMATION
					EndAnimation(hWnd);
		#endif
//					InvalidateRect(hWnd,NULL,TRUE);
				}
				return TRUE;		
			}
			lpPreItem=lpDownLoadItem;
			lpDownLoadItem=lpDownLoadItem->next;
		}
		if (lpHtmlView->lpDownLoadList == NULL)
		{
#ifdef LOADANIMATION
			EndAnimation(hWnd);
#endif
		}
		return FALSE;
}
*/
// **************************************************
// ������static LPDOWNLOADITEM GetDownLoadUrl(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ��Ҫ���صĵ�ַ��NULL��ʾû��Ҫ���صĵ�ַ
// �����������õ���һ��Ҫ���صĵ�ַ��
// ����: 
// **************************************************
static LPDOWNLOADITEM GetDownLoadUrl(HWND hWnd)
{
	LPDOWNLOADITEM lpDownLoadItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->DownLoadCriticalSection);  // ����
		lpDownLoadItem=lpHtmlView->lpDownLoadList;
		if (lpDownLoadItem)
		{  // ���б�ָ����һ�����
			lpHtmlView->lpDownLoadList = lpDownLoadItem->next;
		}
		UNLOCK(lpHtmlView->DownLoadCriticalSection);  // ����
		return lpDownLoadItem;
/*		while(lpDownLoadItem)
		{
			if (lpDownLoadItem->hUrl==hUrl)
			{  // will delete current node
//				RETAILMSG(1,("Get Url <%s>!!!\r\n",lpDownLoadItem->lpUrl));
				return lpDownLoadItem->lpUrl;		
			}
			lpDownLoadItem=lpDownLoadItem->next;
		}
*/
		return NULL;
}
// **************************************************
// ������BOOL DeleteDownList(	HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ����������ɾ���ȴ������б�
// ����: 
// **************************************************
BOOL DeleteDownList(	HWND hWnd)
{
	LPDOWNLOADITEM lpDownLoadItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->DownLoadCriticalSection);  // ����
		lpDownLoadItem=lpHtmlView->lpDownLoadList;  // �õ��б�ͷ
		while(lpDownLoadItem)
		{
			lpNextItem = lpDownLoadItem->next;  // �õ���һ�����
			if (lpDownLoadItem->lpUrl)  // ɾ����ǰ���
				free(lpDownLoadItem->lpUrl);
			if (lpDownLoadItem->lpFileName)  // ɾ����ǰ���
				free(lpDownLoadItem->lpFileName);
			free(lpDownLoadItem);
			lpDownLoadItem=lpNextItem;  // ָ����һ���
		}
		lpHtmlView->lpDownLoadList = NULL;  // �б�ָ��
		UNLOCK(lpHtmlView->DownLoadCriticalSection); // ����
//#ifdef LOADANIMATION
//		EndAnimation(hWnd);
//#endif
		return TRUE;
}

// **************************************************
// ������BOOL DownLoadImage(HWND hWnd,HHTMCTL hHtmlCtl,LPTSTR lpImage)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hHtmlCtl -- ָ��ͼ��Ŀ��ƾ��
// 	IN lpImage -- Ҫ�����ص�ͼ���ַ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ��������������һ��ָ����ͼ��
// ����: 
// **************************************************
BOOL DownLoadImage(HWND hWnd,HHTMCTL hHtmlCtl,LPTSTR lpImage)
{
	LPHTMLVIEW lpHtmlView;
	LPTSTR lpFullFileName;
//	int i;

//	RETAILMSG(1,("####################################\r\n"));
	//RETAILMSG(1,("Will Download Image <%s>\r\n",lpImage));
	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //�õ�HTMLVIEW�ṹ
	if (lpHtmlView==NULL)
		return 0;
	if (lpHtmlView->lpCurUrl->lpUrl==NULL)  // ��ǰURL������
		return FALSE;
	if (SMIE_IsLocateFile(hWnd,lpHtmlView->lpCurUrl->lpUrl)==TRUE)  // �Ǳ����ļ�
	{  // ���ı��ļ��Ǳ����ļ�,��ͼ���ļ�������һ�������ļ���Ҳ������һ������·���������ļ�
		LPTSTR lpFullImage;

		//RETAILMSG(1,("Open File is local file <%s>\r\n",lpHtmlView->lpCurUrl->lpUrl));
		if (strnicmp(lpImage,INTERNET_HEAD,5) == 0)
		{  // ��һ�������ļ�����ʼ����
			#ifdef INTERNET_SUPPORT
				SMIE_DownLoadUrl(hWnd,hHtmlCtl,lpImage);  // ����ָ�����ļ�
			#endif
				return TRUE;  // ���سɹ�
		}

		// ͼ���ļ��Ǳ����ļ�
		lpFullImage=SMIE_GetFullImage(hWnd,lpImage); // �õ�ͼ���ļ���ȫ·����
		if (lpFullImage==NULL)
			return FALSE; // û�еõ�ȫ·���ļ���
		DownloadImageOK(hWnd, hHtmlCtl,lpFullImage); 
		free(lpFullImage);
		return FALSE;
	}

	// get the image full address in internet
// !!! Delete By Jami chen in 2004.09.09
//	lpFullFileName=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpImage)+4);  //�õ�����ļ�����
//	if (lpFullFileName==NULL)
//		return FALSE;
// !!! Delete By Jami chen in 2004.09.09
//	RETAILMSG(1,("lpImage = %x \r\n",lpImage));
	if (strnicmp(lpImage,INTERNET_HEAD,5))
	{  //�����·��
		//RETAILMSG(1,("Image is a sub dir <%s>\r\n",lpImage));
		// �õ�ȫ·��
// !!! Modified By Jami chen in 2004.09.09
/*		strcpy(lpFullFileName,lpHtmlView->lpCurUrl->lpUrl);
		i=strlen(lpFullFileName);
		while(lpFullFileName[i]!='/') i--;
		lpFullFileName[i+1]=0;
	//	strcat(lpFullFileName,"/");
		strcat(lpFullFileName,lpImage);
*/
		lpFullFileName = MakeInternetRelativePath(lpHtmlView->lpCurUrl->lpUrl,lpImage);
		//RETAILMSG(1,("Will Down File  <%s>\r\n",lpImage));
	}
	else
	{  // �Ǿ���·��
// !!! modified By Jami chen in 2004.09.09
//		strcpy(lpFullFileName,lpImage);
		lpFullFileName = BufferAssignTChar(lpImage);
// !!! modified End By Jami chen in 2004.09.09
	}
	if (lpFullFileName==NULL)
		return FALSE;
//	SMIE_StartNewFtpSite(hWnd,lpFullFileName);
#ifdef INTERNET_SUPPORT
	SMIE_DownLoadUrl(hWnd,hHtmlCtl,lpFullFileName);  // ����ָ�����ļ�
#endif
	free(lpFullFileName);  // �ͷŲ��õĻ���
	return TRUE;  // ���سɹ�
}


#ifdef LOADANIMATION

#define IDB_FLASH1                      120
#define IDB_FLASH2                      121
#define IDB_FLASH3                      122
#define IDB_FLASH4                      123
#define IDB_FLASH5                      124
#define IDB_FLASH6                      125
#define IDB_FLASH7                      126
#define IDB_FLASH8                      127
#define IDB_FLASH9                      128
#define IDB_FLASH10                     129
#define IDB_FLASH11                     130
#define IDB_FLASH12                     131
#define IDB_FLASH13                     132
#define IDB_FLASH14                     133

#define MAX_PICNUM		14

// **************************************************
// ������static void StartAnimation(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ������������ʼ������
// ����: 
// **************************************************
static void StartAnimation(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;

//		RETAILMSG(1,("StartAnimation ...\r\n"));
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return ;
		if (lpHtmlView->hAnimation == NULL)
		{  // ������û������
			lpHtmlView->bExitAnimation = FALSE;
			lpHtmlView->hAnimation = CreateThread(NULL, 0, LoadAnimateProc, (void *)hWnd, 0, 0 );// ������ʾ�����߳�
		}
//		RETAILMSG(1,("StartAnimation OK !!!\r\n"));
}
// **************************************************
// ������static void EndAnimation(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ��������������������
// ����: 
// **************************************************
static void EndAnimation(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;
	RECT rect;

//		RETAILMSG(1,("EndAnimation ...\r\n"));
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return ;
		if (lpHtmlView->hAnimation == NULL)  // ����û������
			return ;
		lpHtmlView->bExitAnimation = TRUE;  // �˳�������־
		//Sleep(400);
		WaitForSingleObject(lpHtmlView->hAnimation,400);  // ��ͣ���ȴ��˳�
//		RETAILMSG(1,("Wait Exit Thread ...\r\n"));
		if (lpHtmlView->hAnimation)  // �رվ��
			CloseHandle(lpHtmlView->hAnimation);
		lpHtmlView->hAnimation = NULL;
		//RETAILMSG(1,(TEXT("******************* Will Redraw Window\r\n")));
//		InvalidateRect(hWnd,NULL,TRUE);
//		Sleep(3000);

		// ��Ҫ����ˢ����ʾ����λ�õľ���
		GetClientRect(hWnd,&rect);
		// �õ�������λ��
		rect.left = (rect.right - 32)/2;
		rect.top = (rect.bottom - 32)/2;
		rect.right =  rect.left + 32;
		rect.bottom = rect.top + 32;
		InvalidateRect(hWnd,&rect,TRUE); // ˢ�¶���λ�õĴ���
//		RETAILMSG(1,("EndAnimation OK\r\n"));
}
// **************************************************
// ������static DWORD WINAPI LoadAnimateProc(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// ���������������̹߳��̡�
// ����: 
// **************************************************
static DWORD WINAPI LoadAnimateProc(VOID * pParam)
{
	LPHTMLVIEW lpHtmlView;
	static int iPic = 0;
//	HWND hWnd = (HWND)(*(HWND *)pParam);
	HWND hWnd = (HWND)pParam;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return 0;

		while(lpHtmlView->bExitAnimation == FALSE)
		{
//			 RETAILMSG(1,("******** Show (%d) picture!!!\r\n",iPic));
			 ShowAnimation(hWnd, iPic);  // ��ʾһ������
			 iPic ++;  // ����һ��
			 if (iPic >= MAX_PICNUM)  // �����һ��
				 iPic = 0;  // �ص���һ��
			 Sleep(200);// ��ͣ
		}
		return 0;
}

// **************************************************
// ������static void ShowAnimation(HWND hWnd, int iPic)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iPic -- Ҫ��ʾ�Ķ�����ҳ����
// 
// ����ֵ����
// ������������ʾ������һҳ��
// ����: 
// **************************************************
static void ShowAnimation(HWND hWnd, int iPic)
{
//	HICON hIcon;
	HDC hdc;
	int iIconID[MAX_PICNUM]={IDB_FLASH1,IDB_FLASH2,IDB_FLASH3,IDB_FLASH4,IDB_FLASH5,IDB_FLASH6,IDB_FLASH7,
					 IDB_FLASH8,IDB_FLASH9,IDB_FLASH10,IDB_FLASH11,IDB_FLASH12,IDB_FLASH13,IDB_FLASH14};
	RECT rect;
	HINSTANCE hInstance;
	HBITMAP hBitmap;
	HDC hMemDC;
		
//		hIcon = LoadImage( NULL, MAKEINTRESOURCE(iIconID[iPic]), IMAGE_ICON, 16, 16, 0) ;
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // �õ�����ʵ�����
		GetClientRect(hWnd,&rect); // �õ����ھ���
//		hIcon = LoadImage( hInstance, MAKEINTRESOURCE(iIconID[iPic]), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE) ;
		hBitmap = LoadImage( hInstance, MAKEINTRESOURCE(iIconID[iPic]), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE) ;  // �õ�λͼ
//		if (hIcon)
		if (hBitmap)
		{
			hdc = GetDC(hWnd);  // �õ�HDC
			//DrawIcon(hdc,(rect.right - 32)/2,(rect.bottom - 32)/2,hIcon);
			hMemDC =  CreateCompatibleDC( hdc );  //�õ��ڴ�DC
			hBitmap = SelectObject( hMemDC, hBitmap );  // ��λͼѡ���ڴ�DC
        
			//GetWindowExtEx( hdc, &size );
			// ����λͼ
			BitBlt( hdc, (rect.right - 32)/2,(rect.bottom - 32)/2, 32, 32, hMemDC, 0, 0, SRCCOPY );
			hBitmap = SelectObject( hMemDC, hBitmap );  //�ָ�ԭ����λͼ
			DeleteDC( hMemDC );
			ReleaseDC(hWnd,hdc);
			//DestroyIcon(hIcon);
			DeleteObject(hBitmap);
		}
		else
		{
			DWORD dwErr=GetLastError();  // �õ��������
		}
}
#endif


#ifdef DOWNLOAD_PROCESS1

// !!! Add By Jami chen in 2004.09.16
// **************************************************
// ������static DWORD WINAPI DownLoadThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// ���������������߳�
// ����: 
// **************************************************
static DWORD WINAPI DownLoadThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPDOWNLOADITEM lpDownloadItem;
	LPDOWNLOADPARAM lpDownloadParam;

		lpDownloadParam = (LPDOWNLOADPARAM)pParam;  // ���ز���
		if (lpDownloadParam == NULL)
			return FALSE;
		hWnd = lpDownloadParam->hWnd;
		lpDownloadItem = lpDownloadParam->lpDownloadItem;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
//		lpHtmlView->iRunThreadCount ++;  // ���������߳�
//		RETAILMSG(1,("############ Will Download <%s>\r\n",lpDownloadItem->lpUrl));
		if (DownLoadAFile(hWnd, lpDownloadItem))
		{
			// ���ص�ǰ�ļ�
//				hControl = GetControl(hWnd,hUrl);
/*			if (!lpHtmlView->bExit && lpDownloadItem->hControl)
			{  // ���سɹ�
				RECT rect={0,0,0,0};
				HDC hdc;
				POINT ptOrg;

					ptOrg.x = lpHtmlView->x_Org;
					ptOrg.y = lpHtmlView->y_Org;
					// ��ʾ�ÿؼ�
					hdc = GetDC(hWnd);
					SMIE_ShowHtmlControl(hWnd,hdc,lpDownloadItem->hControl,rect,ptOrg);
					ReleaseDC(hWnd,hdc);
			}
*/
		}
		else
		{
			// �����ļ�ʧ��
			if (!lpHtmlView->bExit && lpDownloadItem->hControl == NULL)
			{  // ������ҳ������ˢ����ҳ
				if (lpHtmlView->lpCurFile)
					free(lpHtmlView->lpCurFile);
				lpHtmlView->lpCurFile=NULL;
//				RETAILMSG(1,("M111MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
				PostMessage(hWnd,HM_REFRESH,0,0);
			}
		}
//		RETAILMSG(1,("############ Download <%s> OK\r\n",lpDownloadItem->lpUrl));
		// �ͷŸý��Ŀռ�
		if (lpDownloadItem->lpUrl)
			free(lpDownloadItem->lpUrl);
		if (lpDownloadItem->lpFileName)
			free(lpDownloadItem->lpFileName);
		free(lpDownloadItem);

		free(lpDownloadParam);
		lpHtmlView->iRunThreadCount --;  // ����һ�������߳�
		return 0;
}

// **************************************************
// ������DWORD WINAPI DownLoadmanageThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// �������������ع����̡߳�
// ����: 
// **************************************************
DWORD WINAPI DownLoadmanageThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPDOWNLOADITEM lpDownloadItem;
	LPDOWNLOADPARAM lpDownloadParam;
	int preRunThread = 0;
	int iTimeout = 0;
	HANDLE handle;

//		RETAILMSG(1,("########### Will Run Manage Thread... \r\n"));
		hWnd = (HWND)pParam;  // �õ�����
		if (hWnd == NULL)
			return FALSE;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
//		RETAILMSG(1,("########### Run Manage Thread Success \r\n"));
		while(1)
		{
//			RETAILMSG(1,("."));
//			if (lpHtmlView->bExit && lpHtmlView->iRunThreadCount == 0)
//				break;
			if (lpHtmlView->bExit)
				break;
			if (preRunThread == 0 && lpHtmlView->iRunThreadCount > 0)
			{
				// ��ʼ�����ļ�
#ifdef LOADANIMATION
				StartAnimation(hWnd);  // ��ʼ����
#endif
			}
			if (preRunThread > 0 && lpHtmlView->iRunThreadCount == 0)
			{
				// �ļ��Ѿ��������
#ifdef LOADANIMATION
				EndAnimation(hWnd);  // ��������
#endif
			}
			preRunThread = lpHtmlView->iRunThreadCount;

			if (lpHtmlView->iRunThreadCount < THREADNUM)  // ���������ļ����� �Ƿ�С�ڿ���������ص� �߳���
			{
//				RETAILMSG(1,("########### Search Next Url to Download \r\n"));
				// ���Լ���������һ���ļ�
				lpDownloadItem = GetDownLoadUrl(hWnd);
				if (lpDownloadItem == NULL)
				{
					// û��Ҫ�ȴ����ص��ļ�
					Sleep(200);
					continue;
				}
//				RETAILMSG(1,("########### Have a File To be Download \r\n"));
				// ���һ������
				lpDownloadParam = (LPDOWNLOADPARAM)malloc(sizeof(DOWNLOADPARAM));  // �����ڴ�
				if (lpDownloadParam)
				{
					lpDownloadParam->hWnd = hWnd;
					lpDownloadParam->lpDownloadItem = lpDownloadItem;
					handle = CreateThread(NULL, 0, DownLoadThread, (void *)lpDownloadParam, 0, 0 );
					CloseHandle(handle);
					lpHtmlView->iRunThreadCount ++;  // ���������߳�
				}
			}
			else
				Sleep(200);
		}

#ifdef LOADANIMATION
		EndAnimation(hWnd);
#endif
//		RETAILMSG(1,("########### Run Manage Thread Exit \r\n"));
		iTimeout = 0;
		while(lpHtmlView->iRunThreadCount)
		{
			// �ȴ������߳��˳�
			iTimeout ++;
			//RETAILMSG(1,("########### Have (%d) Download Thread Running!!\r\n",lpHtmlView->iRunThreadCount));
//			if (iTimeout >= 100)
//				break;
			Sleep(200);
		}
		
		lpHtmlView->bExit = FALSE;  // �ɹ��˳�
		return 0;
}
#endif

#ifdef DOWNLOAD_PROCESS2
// **************************************************
// ������static DWORD WINAPI DownLoadThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// �����������ļ������߳�
// ����: 
// **************************************************
static DWORD WINAPI DownLoadThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPDOWNLOADITEM lpDownloadItem;

		hWnd = (HWND)pParam;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
		while(TRUE)
		{
			WaitForSingleObject(lpHtmlView->hDownloadEvent,INFINITE); // �ȴ���Ҫ���ص��ļ�
			if (lpHtmlView->bExit)
				break;
			ResetEvent(lpHtmlView->hDownloadEvent);
			while(TRUE)
			{
				lpDownloadItem = GetDownLoadUrl(hWnd);  // �õ�Ҫ���ص��ļ�
				if (lpDownloadItem == NULL)
				{
					// û��Ҫ�ȴ����ص��ļ�
					break;
				}
				// �����ļ�
				if (DownLoadAFile(hWnd, lpDownloadItem))
				{
					// ���ص�ǰ�ļ��ɹ�
				}
				else
				{
					// �����ļ�ʧ��
					if (!lpHtmlView->bExit && lpDownloadItem->hControl == NULL)
					{  // ������ҳ������ˢ����ҳ
						if (lpHtmlView->lpCurFile)
							free(lpHtmlView->lpCurFile);
						lpHtmlView->lpCurFile=NULL;
		//				RETAILMSG(1,("M111MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
						PostMessage(hWnd,HM_REFRESH,0,0);
					}
				}
		//		RETAILMSG(1,("############ Download <%s> OK\r\n",lpDownloadItem->lpUrl));
				// �ͷŸý��Ŀռ�
				if (lpDownloadItem->lpUrl)
					free(lpDownloadItem->lpUrl);
				if (lpDownloadItem->lpFileName)
					free(lpDownloadItem->lpFileName);
				free(lpDownloadItem);
			}
		}
		lpHtmlView->iRunThreadCount --;  // ����һ�������߳�
		return 0;
}

// **************************************************
// ������DWORD WINAPI DownLoadmanageThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// �������������ع����̡߳�
// ����: 
// **************************************************
DWORD WINAPI DownLoadmanageThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	int iTimeout = 0;
	HANDLE handle;
	int i;

//		RETAILMSG(1,("########### Will Run Manage Thread... \r\n"));
		hWnd = (HWND)pParam;  // �õ�����
		if (hWnd == NULL)
			return FALSE;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
//		RETAILMSG(1,("########### Run Manage Thread Success \r\n"));
		for (i = 0; i < THREADNUM; i++)  //  ����ָ���������ļ������߳���
		{
			handle = CreateThread(NULL, 0, DownLoadThread, (void *)hWnd, 0, 0 );
			CloseHandle(handle);
			lpHtmlView->iRunThreadCount ++;  // ���������߳�
		}

		while(1)
		{
			WaitForSingleObject(lpHtmlView->hExitEvent,INFINITE); // �ȴ�ϵͳ�˳�
			if (lpHtmlView->bExit)
				break;
		}
		SetEvent(lpHtmlView->hDownloadEvent);
		iTimeout = 0;
		while(lpHtmlView->iRunThreadCount)
		{
			// �ȴ������߳��˳�
			iTimeout ++;
			//RETAILMSG(1,("########### Have (%d) Download Thread Running!!\r\n",lpHtmlView->iRunThreadCount));
			Sleep(200);
		}
		
		lpHtmlView->bExit = FALSE;  // �ɹ��˳�
		return 0;
}
#endif

// **************************************************
// ������BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hUrl -- ָ����URL���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������һ��ָ����URL���뵽��ǰ�������صĶ����С�
// ����: 
// **************************************************
BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl)
{
	LPRUNNINGITEM lpRunningItem;
	LPHTMLVIEW lpHtmlView;



		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;
	
		LOCK(lpHtmlView->RunningCriticalSection);  // ����
		lpRunningItem=(LPRUNNINGITEM)malloc(sizeof(RUNNINGITEM));  // �����ڴ�
		if (lpRunningItem==NULL)
		{  // ����ʧ��
			UNLOCK(lpHtmlView->RunningCriticalSection);
			return FALSE;
		}
		// ��������
		lpRunningItem->hUrl= hUrl;
		// �������ݵ���ǰ��
		lpRunningItem->next=lpHtmlView->lpRunningList;
		lpHtmlView->lpRunningList=lpRunningItem;
		UNLOCK(lpHtmlView->RunningCriticalSection);  // ����
		return TRUE;		
}
// **************************************************
// ������BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hUrl -- URL���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������ӵ�ǰ�������صĶ�����ɾ��ָ����URL��
// ����: 
// **************************************************
BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl)
{
	LPRUNNINGITEM lpRunningItem,lpPreItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->RunningCriticalSection);  // ����
		lpRunningItem=lpHtmlView->lpRunningList;  // �õ���һ�������б���
		lpPreItem=NULL;
		while(lpRunningItem)
		{
			if (lpRunningItem->hUrl==hUrl)
			{  // will delete current node
				if (lpPreItem==NULL)
				{  // this is first node
					lpHtmlView->lpRunningList=lpRunningItem->next;
				}
				else
				{// will set prenode next link current node next
					lpPreItem->next=lpRunningItem->next;
				}
				// Add 
				CloseUrlHandle( hUrl ,"ttt2");
				// delete current node
				free(lpRunningItem);
				UNLOCK(lpHtmlView->RunningCriticalSection);
				return TRUE;		
			}
			lpPreItem=lpRunningItem;
			lpRunningItem=lpRunningItem->next;
		}
		UNLOCK(lpHtmlView->RunningCriticalSection);  // ����
		return FALSE;
}
// **************************************************
// ������BOOL CloseRunningList(	HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
// �����������ر��������صĶ��С�
// ����: 
// **************************************************
BOOL CloseRunningList(	HWND hWnd)
{
	LPRUNNINGITEM lpRunningItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->RunningCriticalSection);  // ����
		lpRunningItem=lpHtmlView->lpRunningList;
		lpNextItem=NULL;
		while(lpRunningItem)
		{
			lpNextItem = lpRunningItem->next;
//			RETAILMSG(1,("Close Internet handle (%X)\r\n",lpRunningItem->hUrl));
			CloseUrlHandle(lpRunningItem->hUrl,"ttt7");
			// delete current node
			free(lpRunningItem);
			lpRunningItem=lpNextItem;
		}
		lpHtmlView->lpRunningList = NULL;  // �������������б�Ϊ��
		UNLOCK(lpHtmlView->RunningCriticalSection);  // ����
		return FALSE;
}

// !!! Add End By Jami chen in 2004.09.16

// **************************************************
// ������void SMIE_DownLoadSpecialFile( HWND  hWnd,LPTSTR lpUrl)
// ������
// 	IN hWnd -- ���ھ��
//  IN lpUrl -- ָ����Ҫ���ص��ļ�URL
// 
// ����ֵ����
// ��������������һ��ָ�����ļ���
// ����: 
// **************************************************
void SMIE_DownLoadSpecialFile( HWND  hWnd,LPTSTR lpUrl)
{
	TCHAR szFileName[MAX_PATH];
	LPTSTR lpOrgFileName = NULL,lpFileName;
	BOOL bRet;
	RETAILMSG(1, ("SMIE_DownLoadSpecialFile (%s) \r\n", lpUrl));
	lpOrgFileName=GetFileNameFromUrl(hWnd,lpUrl);  // ����URL�õ���ʱ�ļ���
	lpFileName = lpOrgFileName + strlen(lpOrgFileName);
	
	while(lpFileName > lpOrgFileName)
	{
		if (*lpFileName == '\\')
		{
			lpFileName ++;
			break;
		}
		lpFileName --;
	}

	bRet = GetSaveFile( hWnd,lpFileName,szFileName);
	if (lpOrgFileName)
		free(lpOrgFileName);
	if (bRet == TRUE)
	{

#ifdef AUTO_LOGIN
		LPHTMLVIEW  lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
		{
			return ;
		}
		// ����ҳ֮ǰ����Ҫ���ŵ�����
#if 0
		if (DailToInternet(hWnd,&lpHtmlView->hRasConn,GPRS_NETTYPE_INTERNET) == FALSE) // ���ŵ�INTERNET
		{
			// �������ӵ�����
#ifdef ZT_PHONE
			ZTMessageBox(hWnd,"�������ӵ�����","����",MB_OK);
#else
			MessageBox(hWnd,"�������ӵ�����","����",MB_OK);
#endif
			return ;
		}
#else
		if (LinkNet(NULL,GPRS_NETTYPE_INTERNET) == FALSE) // ���ŵ�INTERNET
		{
			return ;
		}
#endif
#endif

		//RETAILMSG(1,("Sucess Link to Internet [%x]  for Down load file\r\n",lpHtmlView->hRasConn));
		InsertDownList(	hWnd,lpUrl,NULL,szFileName);
	}	
}


/*************************************************
������static void  GetSaveFile( HWND hwnd,CHAR *szDefault,CHAR *szFileName)
������
IN    szDefault    - �򿪶Ի���ʱ�������Ĭ���ļ���
IN    hwnd         - OpenFile �Ի���ĸ����ھ��
IN    szFileName   - ���ص��ļ�����Ϊȫ·���ַ���
���أ�void    
��������:���渽��
���ã� ����Ӹ���ʱ���� 
************************************************/
static BOOL  GetSaveFile( HWND hwnd,CHAR *szDefault,CHAR *szFileName)
{    
	 OPENFILENAME of;       // common dialog box structure
     CHAR szFile[256];       // buffer for file name
 
     // Initialize OPENFILENAME
	strcpy( szFile,szDefault );
    of.lStructSize       = sizeof (OPENFILENAME);
	of.hwndOwner         = hwnd;
	of.hInstance         = (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE );
	of.lpstrCustomFilter = NULL;
	of.nMaxCustFilter    = 0;
	of.nFilterIndex      = 0;
	of.lpstrFile         = szFile;
	of.nMaxFile          = MAX_PATH;
	of.lpstrFileTitle    = NULL;
	of.nMaxFileTitle     = 0;
	of.lpstrInitialDir   = TEXT("\\��ʱ");
	of.lpstrTitle        = TEXT("ѡ�񱣴��ļ���");//NULL;
	of.Flags             = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	of.nFileOffset       = 0;
	of.nFileExtension    = 0;
	of.lpstrDefExt       = NULL;
	of.lCustData         = 0;
	of.lpfnHook          = NULL;
	of.lpTemplateName    = NULL;
	
// Display the Open dialog box. 
	of.lpstrFilter       = TEXT("*.*\0");
	if (GetSaveFileName( &of ))
	{
        strcpy(szFileName,of.lpstrFile);
		return TRUE;
	}
	return FALSE;
}


// !!! Add By Jami chen in 2004.09.14
// **************************************************
// ������static BOOL InitialInternet(void)
// ������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ʼ��Internet��
// ����: 
// **************************************************
static BOOL InitialInternet(HWND hWnd)
{
	WSADATA		WSAData;
	int			iErrCode;
	
	if( iErrCode=WSAStartup( MAKEWORD(1,1), &WSAData ) )
	{
		TCHAR	pszErr[100];
		
		//���ش�����ʾ����Ҫ���˳�
		sprintf( pszErr, "����Windows Socket���ܳ�������=%d����Ҫ���˳�����", WSAGetLastError() );
		MessageBox( hWnd, pszErr, "Err", MB_OK );
		return FALSE;
	}
	return TRUE;
}

// !!! Add By Jami chen in 2004.09.14






// *******************************************************************************
// ��ʾ���ص��߳�
// *******************************************************************************

#define ID_FILENAME		100
#define ID_SIZE			101
#define ID_DOWNSIZE		102
#define ID_CANCEL		103
#define ID_DOWNPROCESS	104
#define ID_DOWNSIZETAG	105
#define ID_DOWNSTATUS	106

#define PROCESS_ITEMNUM		8
static const struct dlgIDD_DownloadProcess{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[PROCESS_ITEMNUM];
}dlgIDD_DownloadProcess = {
    { WS_EX_NOMOVE|WS_EX_TITLE,WS_POPUP|WS_VISIBLE|WS_BORDER,PROCESS_ITEMNUM,WORK_AREA_STARTX,30,WORK_AREA_WIDTH,200,0,0,"����" },
		{
			//����״̬
			{ 0,WS_CHILD | WS_VISIBLE , 10, 4, 240, 30, ID_DOWNSTATUS, classSTATIC, "�����ļ�...", 0 },
			//�ļ���
			{ 0,WS_CHILD | WS_VISIBLE | SS_RIGHT, 10, 32, 240, 30, ID_FILENAME, classSTATIC, "", 0 },
			//�ļ���С
			{ 0,WS_CHILD | WS_VISIBLE , 10, 60, 108, 30, 0XFFFF, classSTATIC, "�ļ���С��", 0 },
			{ 0,WS_CHILD | WS_VISIBLE , 120, 60, 100, 30, ID_SIZE, classSTATIC, "", 0 },
			//�����ص��ļ���С
			{ 0,WS_CHILD , 10, 90, 108, 30, ID_DOWNSIZETAG, classSTATIC, "�Ѿ����أ�", 0 },
			{ 0,WS_CHILD , 120, 90, 100, 30, ID_DOWNSIZE, classSTATIC, "", 0 },

			{ 0,WS_CHILD | PBS_SMOOTH | WS_BORDER, 10, 90, 220, 30, ID_DOWNPROCESS, classPROGRESS, "", 0 },
			//ȡ����ť
#ifdef ZT_PHONE
			{ 0,WS_CHILD | WS_VISIBLE , 80, 130, 80, 30, ID_CANCEL, classZTBUTTON, "ȡ��", 0 },
#else
			{ 0,WS_CHILD | WS_VISIBLE , 80, 130, 80, 30, ID_CANCEL, classBUTTON, "ȡ��", 0 },
#endif
		} 
    };

typedef struct tagDOWNLOADTHREAD{
	HWND hParent;  // �����ھ��
	HWND hDownloadDlg; // ���ش��ھ��
}DOWNLOADTHREAD, *LPDOWNLOADTHREAD;


//static HANDLE g_hThread = NULL;
//static HWND g_hCopyDlg = NULL;

typedef struct tagDOWNLOADDLGDATA{
	HWND hParent;
	int iTotalSize; // ��Ҫ���صĳߴ�
	int iDownloadedSize; // �Ѿ����صĳߴ�
}DOWNLOADDLGDATA, *LPDOWNLOADDLGDATA;

static LRESULT CALLBACK DownloadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI DownloadDialogThread(VOID * pParam);
static LRESULT DoDownloadInitial(HWND hDlg,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDownLoadURL(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDownLoadSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTotalSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDownloadStatus(HWND hWnd,WPARAM wParam,LPARAM lParam);


// **************************************************
// ������static HWND ShowDownloadDialog(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���������ضԻ���Ĵ��ھ��
// ������������ʾ���ڸ����ļ��Ի���
// ����: 
// ��ʾ����ʾ�������ȴ���һ���̣߳��и��̵߳�����ʾ�򣬲��ȴ��������
//	   ����ǰ�߳���������и��ƹ��̣�������ɣ�������Ϣɱ���Ի���
// **************************************************
static HWND ShowDownloadDialog(HWND hWnd)
{
	DOWNLOADTHREAD tagDownloadProcess;
	HANDLE handle;
	int iTimes = 0;

		tagDownloadProcess.hParent = hWnd;
		tagDownloadProcess.hDownloadDlg = NULL;
		handle = CreateThread(NULL, 0, DownloadDialogThread, (void *)&tagDownloadProcess, 0, 0 );  // ����һ���߳�
		CloseHandle(handle); // �رվ���������ɹ��󣬸þ����Ч��

		while(1)
		{
			if (tagDownloadProcess.hDownloadDlg)
				break; // �����Ѿ��������
			Sleep(50); // �ȴ���������
			if (iTimes >= 300)
				break; // ��ȴ�9 ��
			iTimes ++;
		}
		if (tagDownloadProcess.hDownloadDlg == (HWND)-1)
			return NULL;
		return tagDownloadProcess.hDownloadDlg;
}
// **************************************************
// ������static void CloseCopyDialog(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������رո��ƶԻ���
// ����: 
// **************************************************
static void CloseCopyDialog(HWND hDownloadWnd)
{
		if (hDownloadWnd)
		{ // �Ѿ������˸��ƶԻ���
			//RETAILMSG(1,(TEXT("End Dialog !!!!\r\n")));
			PostMessage(hDownloadWnd,DPM_CLOSE,0,0); // ������Ϣ�˳��öԻ���
		}
}
// ********************************************************************
// ������static LRESULT CALLBACK DownloadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������Ӧ�ó�����Ϸ�������ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK DownloadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:  // ������Ϣ
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_INITDIALOG:  // ��ʼ���Ի���
			DoDownloadInitial(hDlg,wParam,lParam);
//			g_hCopyDlg = GetWindow(hDlg,GW_OWNER);
			return 0;
		case DPM_CLOSE: // �رնԻ���
			EndDialog(hDlg,0);
			return TRUE;
		case DPM_SETURL: // �رնԻ���
			DoSetDownLoadURL(hDlg,wParam,lParam);
			return TRUE;
		case DPM_SETSIZE: // �رնԻ���
			DoSetTotalSize(hDlg,wParam,lParam);
			return TRUE;
		case DPM_SETDOWNLOADSIZE: // �رնԻ���
			DoSetDownLoadSize(hDlg,wParam,lParam);
			return TRUE;
		case DPM_SETSTATUS:
			DoSetDownloadStatus(hDlg,wParam,lParam);
			return	TRUE;
		case WM_COMMAND:
			{
				int wmId, wmEvent;

				wmId    = LOWORD(wParam); 
				wmEvent = HIWORD(wParam); 
				switch(wmId)
				{
					case ID_CANCEL:
						{
							LPDOWNLOADDLGDATA lpDownloadDlgData;
						
							lpDownloadDlgData = (LPDOWNLOADDLGDATA)GetWindowLong(hDlg,GWL_USERDATA);
							if (lpDownloadDlgData)
								PostMessage(lpDownloadDlgData->hParent,HM_STOP,0,0);  // ֹͣ��ǰ����
//							EndDialog(hDlg,0);
						}
						break;
				}
			}
		default:
			return 0;
	}
}
// ********************************************************************
// ������static LRESULT DoDownloadInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
// ������
//	  IN hWnd - ���ھ��
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������ʼ��������ʾ����
// ����: 
// ********************************************************************
static LRESULT DoDownloadInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	LPDOWNLOADTHREAD lpDownloadProcess;
	LPDOWNLOADDLGDATA lpDownloadDlgData;

		lpDownloadDlgData = (LPDOWNLOADDLGDATA)malloc(sizeof(DOWNLOADDLGDATA));
		if (lpDownloadDlgData == NULL)
			return -1;

		lpDownloadDlgData->iTotalSize = 0;
		lpDownloadDlgData->iDownloadedSize = 0;
		

		lpDownloadProcess = (LPDOWNLOADTHREAD)lParam;

		lpDownloadDlgData->hParent  = lpDownloadProcess->hParent;

		SetWindowLong(hDlg,GWL_USERDATA,(LONG)lpDownloadDlgData);

		lpDownloadProcess->hDownloadDlg = hDlg;

		return 0;
}


// **************************************************
// ������static DWORD WINAPI DownloadDialogThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// �������������ƴ����̡߳�
// ����: 
// **************************************************
static DWORD WINAPI DownloadDialogThread(VOID * pParam)
{
	HINSTANCE hInstance;
	LPDOWNLOADTHREAD lpDownloadProcess;

		lpDownloadProcess = (LPDOWNLOADTHREAD)pParam;
		hInstance = (HINSTANCE)GetWindowLong(lpDownloadProcess->hParent,GWL_HINSTANCE); // �õ�ʵ�����
		if (DialogBoxIndirectParamEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_DownloadProcess,lpDownloadProcess->hParent, (DLGPROC)DownloadDialogProc,(LONG)pParam) == -1) // �����Ի���
		{
			// �����Ի���ʧ��
			lpDownloadProcess->hDownloadDlg = (HWND)-1;
		}		

		return 0;
}

// **************************************************
// ������static LRESULT DoSetDownLoadURL(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- (LPTSTR)��ǰҪ���ص�URL
// 
// ����ֵ����
// �������������õ�ǰҪ���ص�URL
// ����: 
// **************************************************
static LRESULT DoSetDownLoadURL(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	SetWindowText(GetDlgItem(hWnd,ID_FILENAME),(LPTSTR)lParam);
	return 0;
}

// **************************************************
// ������static LRESULT DoSetTotalSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��ǰURL�ĳߴ�
// 	IN lParam -- ����
// 
// ����ֵ����
// �������������õ�ǰURL�ĳߴ�
// ����: 
// **************************************************
static LRESULT DoSetTotalSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDOWNLOADDLGDATA lpDownloadDlgData;
	TCHAR lpSize[32];
	HWND hChild;
	
		lpDownloadDlgData = (LPDOWNLOADDLGDATA)GetWindowLong(hWnd,GWL_USERDATA);
		ASSERT(lpDownloadDlgData);

		lpDownloadDlgData->iTotalSize = (int)wParam;
		if (lpDownloadDlgData->iTotalSize == -1)
		{
			lpDownloadDlgData->iTotalSize = 0;
			strcpy(lpSize,"δ֪");
		}
		else
		{
			sprintf(lpSize,"%d",lpDownloadDlgData->iTotalSize);
		}

		SetWindowText(GetDlgItem(hWnd,ID_SIZE),lpSize);
		UpdateWindow(GetDlgItem(hWnd,ID_SIZE));

		if (lpDownloadDlgData->iTotalSize == 0)
		{
			hChild = GetDlgItem(hWnd,ID_DOWNPROCESS);
			ShowWindow(hChild,SW_HIDE);

			hChild = GetDlgItem(hWnd,ID_DOWNSIZETAG);
			ShowWindow(hChild,SW_SHOW);
			hChild = GetDlgItem(hWnd,ID_DOWNSIZE);
			ShowWindow(hChild,SW_SHOW);
		}
		else
		{
			hChild = GetDlgItem(hWnd,ID_DOWNSIZETAG);
			ShowWindow(hChild,SW_HIDE);
			hChild = GetDlgItem(hWnd,ID_DOWNSIZE);
			ShowWindow(hChild,SW_HIDE);

			hChild = GetDlgItem(hWnd,ID_DOWNPROCESS);
			ShowWindow(hChild,SW_SHOW);

			SendMessage(hChild,PBM_SETRANGE,0,MAKELPARAM(0,100));
		}
		return 0;
}

// **************************************************
// ������static LRESULT DoSetDownLoadSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��ǰ�Ѿ����صĳߴ�
// 	IN lParam -- ����
// 
// ����ֵ����
// �������������õ�ǰ�Ѿ����صĳߴ�
// ����: 
// **************************************************
static LRESULT DoSetDownLoadSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPDOWNLOADDLGDATA lpDownloadDlgData;
	TCHAR lpSize[32];
	HWND hChild;
	
		lpDownloadDlgData = (LPDOWNLOADDLGDATA)GetWindowLong(hWnd,GWL_USERDATA);
		ASSERT(lpDownloadDlgData);

		lpDownloadDlgData->iDownloadedSize = (int)wParam;
		sprintf(lpSize,"%d",lpDownloadDlgData->iDownloadedSize);

		if (lpDownloadDlgData->iTotalSize == 0)
		{
			hChild = GetDlgItem(hWnd,ID_DOWNSIZE);
			SetWindowText(hChild,lpSize);
			UpdateWindow(hChild);
		}
		else
		{
			int iPos;

			iPos = (lpDownloadDlgData->iDownloadedSize * 100) / lpDownloadDlgData->iTotalSize;
			hChild = GetDlgItem(hWnd,ID_DOWNPROCESS);
			SendMessage(hChild,PBM_SETPOS,iPos,0);
		}

		return 0;
}

// **************************************************
// ������static LRESULT DoSetDownloadStatus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��ǰ��״̬
// 	IN lParam -- ����
// 
// ����ֵ����
// �������������õ�ǰ��״̬
// ����: 
// **************************************************
static LRESULT DoSetDownloadStatus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hChild;
	DWORD dwStatus;
	LPTSTR lpStatus;

			dwStatus = wParam;

			if (dwStatus == DOWNLOADSTATUS_OPENURL)
			{
				lpStatus = "���ڴ�...";
			}
			if (dwStatus == DOWNLOADSTATUS_DOWNLOAD)
			{
				lpStatus = "��������...";
			}
			
			if (dwStatus == DOWNLOADSTATUS_COMPLETE)
			{
				lpStatus = "������ɡ�";
			}

			hChild = GetDlgItem(hWnd,ID_DOWNSTATUS);
			SetWindowText(hChild,lpStatus);
			UpdateWindow(hChild);

			return TRUE;
}


// **************************************************
// ������static int GetCurrentUrlSize(LPURLHANDLE hCurrentUrl)
// ������
// 	IN hCurrentUrl -- ��ǰ�Ѿ��򿪵�URL���
// 
// ����ֵ����ǰ�򿪵�URL�Ĵ�С
// �����������õ���ǰ�򿪵�URL�Ĵ�С
// ����: 
// **************************************************
static int GetCurrentUrlSize(LPURLHANDLE hCurrentUrl)
{
	int dwContentSize;
	DWORD dwIndex = 0;
	DWORD dwSize = sizeof(DWORD);
	BOOL bRet;

		 //RETAILMSG(1,("HttpQueryQuick 2 hOpenReq =<%X>\r\n",hCurrentUrl->hOpenReq));
		 bRet = HttpQueryInfo(hCurrentUrl->hOpenReq, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER ,&dwContentSize,&dwSize,&dwIndex);

		 //RETAILMSG(1,("HttpQueryQuick 2 hOpenReq =<%X> OK\r\n",hCurrentUrl->hOpenReq));
		 return dwContentSize;
}
// !!! Add End By Jami chen in 2004.09.15



// **************************************************
// ������static	BOOL INetHlp_LookUrl( LPCSTR pszScheme2, DWORD dwLenScheme2, LPCSTR pszUrl, OUT WORD* pwSrvPort, OUT LPSTR pszSrvName, DWORD dwLenSrv, OUT LPSTR pszPath, DWORD dwLenPath )
// ������
// 	IN pszScheme2 -- Scheme�ִ�
//  IN dwLenScheme2 -- Scheme�ִ��ĳ���
//  IN pszUrl -- ָ����URL
//  OUT pwSrvPort -- �õ��������Ķ˿�
//  OUT pszSrvName -- �õ�������������
//  IN dwLenSrv -- ��ŷ��������ƵĻ����С
//  OUT pszPath -- �õ���ǰURL��ָ���������е�·����
//  IN dwLenPath -- ���·�����ƵĻ����С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰָ��URL��SERVER
// ����: 
// **************************************************
static	BOOL	INetHlp_LookUrl( LPCSTR pszScheme2, DWORD dwLenScheme2, LPCSTR pszUrl, OUT WORD* pwSrvPort, 
						OUT LPSTR pszSrvName, DWORD dwLenSrv, OUT LPSTR pszPath, DWORD dwLenPath )
{
	LPSTR				pszTmp;
	DWORD				i;
	DWORD				dwTmp;

	//�������
	ASSERT( pszScheme2 && pszSrvName && pszPath && (dwLenSrv>2) && (dwLenPath>2) );
	//�ж�Scheme
	if( strnicmp( pszUrl, pszScheme2, dwLenScheme2 )==0 )
	{
		//�жϺͺ���Scheme
		pszTmp = (LPSTR)(pszUrl + dwLenScheme2);
		if( (*pszTmp!=':') ||  (pszTmp[1]!='/') || (pszTmp[2]!='/') )
		{
			return FALSE;
		}
		pszTmp += 3;
		
	}
	else
	{
		if( *pszUrl=='/' ) 
		{
			return FALSE;
		}
		pszTmp = (LPSTR)pszUrl;
	}

	//������ ����������
	*pwSrvPort = INTERNET_DEFAULT_HTTP_PORT;
	i = 0;
	while( *pszTmp && (*pszTmp!='/') && i<dwLenSrv )
	{
		pszSrvName[i++] = *pszTmp ++;
	}
	pszSrvName[i] = 0;
	if( i>=dwLenSrv )
	{
		//˵���ռ�̫С
		return FALSE;
	}
	
	//������ ·������
	if( *pszTmp=='/' )
	{
		dwTmp = strlen( pszTmp );
		if( dwTmp>=dwLenPath )
		{
			//˵���ռ�̫С
			return FALSE;
		}
		memcpy( pszPath, pszTmp, dwTmp );
		pszPath[dwTmp] = 0;
	}
	else
	{
		pszPath[0] = '/';
		pszPath[1] = 0;
	}

	return TRUE;
}


// **************************************************
// ������static LPURLHANDLE IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext )
// ������
// 	IN hWnd -- ���ھ��
//  IN hInetOpen -- ��ǰ�Ѿ��򿪵�INTERNET���
//  IN lpszUrl -- ��ǰҪ�򿪵�URL��ַ
//  IN lpszHdrs -- Ҫ�򿪵�URL���͸�ʽ
//  IN dwLenHdrs -- Ҫ�򿪵�URL���͸�ʽ�ĳ���
//  IN dwFlags -- �򿪵ı�־
//  IN dwContext -- һ���û��Զ����ֵ
// 
// ����ֵ����ǰ�򿪵�URL�ľ��
// ������������ָ����URL
// ����: 
// **************************************************
static LPURLHANDLE IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext )
{
	BOOL			fReqOk;
	char			lpszSrvName[MAX_SRVNAME+4];
	char			lpszPath[MAX_HTTP_PATH+4];
	WORD			wSrvPort;
//	HANDLE			hOpenCnn;
//	HANDLE			hOpenReq;
	LPURLHANDLE		lpUrlHandle;		

//	HWND hDownload;

	//
	//RETAILMSG(1,(TEXT("IE_OpenUrl ...\r\n")));
	if( !INetHlp_LookUrl( (LPCSTR)HTTP_SCHEME2, LEN_HTTP_SCHEME2, lpszUrl, &wSrvPort, lpszSrvName, MAX_SRVNAME, lpszPath, MAX_HTTP_PATH ) )
	{
		return NULL;
	}

	lpUrlHandle = (LPURLHANDLE)malloc(sizeof(URLHANDLE));
	if (lpUrlHandle == NULL)
		return NULL;

	memset( lpUrlHandle, 0, (sizeof(URLHANDLE)) );
	//
	lpUrlHandle->hOpenCnn = InternetConnect( hInetOpen, lpszSrvName, wSrvPort, 0, 0, INTERNET_SERVICE_HTTP, 0, (DWORD)NULL );
	if( !lpUrlHandle->hOpenCnn )
	{
		//RETAILMSG(1,(TEXT("     HttpDn_url_fail3: InternetOpenUrl[%s]\r\n"), lpszUrl));
		free(lpUrlHandle);
		return NULL;
	}
	//RETAILMSG(1,(TEXT("InternetConnect [%x]ok\r\n"),lpUrlHandle->hOpenCnn));
	// ���뵱ǰ���ع��̵����������ļ��б�
	InsertRunningList(	hWnd,lpUrlHandle);

	//RETAILMSG(1,(TEXT("InsertRunningList ok\r\n")));
	//
	lpUrlHandle->hOpenReq = HttpOpenRequest( lpUrlHandle->hOpenCnn, (LPCSTR)HTTP_GET, lpszPath, (LPCSTR)HTTP_VERSION, 0, lpszHdrs, 0, (DWORD)NULL );
	if( !lpUrlHandle->hOpenReq )
	{
		DeleteRunningList(	hWnd,lpUrlHandle);  // ɾ���Ѿ�������ɵ�URL
		//RETAILMSG(1,(TEXT("DeleteRunningList ok\r\n")));
//		CloseUrlHandle( lpUrlHandle,"ttt8");
//		free(lpUrlHandle);
		//RETAILMSG(1,(TEXT("     HttpDn_url_fail2: InternetOpenUrl[%s]\r\n"), lpszUrl));
		return NULL;
	}
	//RETAILMSG(1,(TEXT("HttpOpenRequest [%x]ok\r\n"),lpUrlHandle->hOpenReq));
	//
	fReqOk = HttpSendRequest( lpUrlHandle->hOpenReq, NULL, 0, 0, 0 );
	if( !fReqOk )
	{
		DeleteRunningList(	hWnd,lpUrlHandle);  // ɾ���Ѿ�������ɵ�URL
		//RETAILMSG(1,(TEXT("DeleteRunningList ok\r\n")));
//		CloseUrlHandle( lpUrlHandle,"ttt9");
//		free(lpUrlHandle);
		//RETAILMSG(1,(TEXT("     HttpDn_url_fail1: InternetOpenUrl[%s]\r\n"), lpszUrl));
		return NULL;
	}

	//RETAILMSG(1,(TEXT("HttpSendRequest ok\r\n")));
	// success
	return lpUrlHandle;
}

// **************************************************
// ������static void CloseUrlHandle( LPURLHANDLE lpUrlHandle ,LPTSTR lpDump)
// ������
// 	IN lpUrlHandle -- ��ǰ�Ѿ��򿪵�URL���
// 
// ����ֵ����
// �����������رյ�ǰ�򿪵�URL���
// ����: 
// **************************************************
static void CloseUrlHandle( LPURLHANDLE lpUrlHandle ,LPTSTR lpDump)
{
	if (lpUrlHandle == NULL)
		return ;
	//RETAILMSG(1,(TEXT("%s"),lpDump));
	//RETAILMSG(1,(TEXT("lpUrlHandle->hOpenCnn = %x"),lpUrlHandle->hOpenCnn));
	//RETAILMSG(1,(TEXT("lpUrlHandle->hOpenReq = %x"),lpUrlHandle->hOpenReq));
	InternetCloseHandle( lpUrlHandle->hOpenCnn );
	free(lpUrlHandle);
	//RETAILMSG(1,(TEXT("Close OK!!! \r\n")));
}
