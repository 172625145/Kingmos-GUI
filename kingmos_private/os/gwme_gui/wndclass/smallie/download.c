/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类,文件下载
版本号：1.0.0.456
开发时期：2004-05-25
作者：陈建明 JAMI
修改记录：
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
// 使用一个管理线程，有文件需要下载，则创建一个线程来下载指定的文件，文件下载完成，线程结束
// #define DOWNLOAD_PROCESS1   
// 使用一个管理线程来创建文件下载线程和等待线程结束，下载线程来管理是否有文件需要下载
// 如果有文件需要下载，则下载该文件，否则等待需要下载的文件
#define DOWNLOAD_PROCESS2

// **************************************************
// 定义区域
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
// 下载窗口函数，变量声明
// *****************************************************
#define DPM_CLOSE			(WM_USER + 1235)  // 关闭窗口
#define DPM_SETURL			(WM_USER + 1236)  // 当前要下载的URL
#define DPM_SETSIZE			(WM_USER + 1237)  // 当前要下载的文件长度
#define DPM_SETDOWNLOADSIZE	(WM_USER + 1238)  // 已经下载的长度
#define DPM_SETSTATUS		(WM_USER + 1239)  // 当前要下载的状态

#define DOWNLOADSTATUS_OPENURL		100
#define DOWNLOADSTATUS_DOWNLOAD		101
#define DOWNLOADSTATUS_COMPLETE		102

static HWND ShowDownloadDialog(HWND hWnd);
static void CloseCopyDialog(HWND hDownloadWnd);
static BOOL GetSaveFile( HWND hwnd,CHAR *szDefault,CHAR *szFileName);

#define DIALOG_DOWNLOAD  // 要显示下载的对话框
#define AUTO_LOGIN  // 要自动登陆

//static HINTERNET IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext );
static LPURLHANDLE IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext );
static void CloseUrlHandle( LPURLHANDLE lpUrlHandle ,LPTSTR lpDump);


//static	const	char	g_szaccept[] = "Accept: image/gif, image/jpeg, */*";
//static	const	char	g_szaccept[] = "Accept: */*";
static	const	char	g_szacceptQuest[] = "*/*";

// *****************************************************

// **************************************************
// 声明：void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 一个指定的站点
// 
// 返回值：无
// 功能描述：开始打开一个新的站点
// 引用: 
// **************************************************
void SMIE_StartNewFtpSite( HWND  hWnd,LPTSTR lpUrl)
{
#ifdef AUTO_LOGIN
	LPHTMLVIEW  lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
		{
			return ;
		}
#if 0
		// 打开网页之前首先要拨号到网络
		if (DailToInternet(hWnd,&lpHtmlView->hRasConn,GPRS_NETTYPE_INTERNET) == FALSE) // 拨号到INTERNET
		{
			// 不能连接到网络
#ifdef ZT_PHONE
			ZTMessageBox(hWnd,"不能连接到网络","错误",MB_OK);
#else
			MessageBox(hWnd,"不能连接到网络","错误",MB_OK);
#endif
			return ;
		}
#else
		if (LinkNet(NULL,GPRS_NETTYPE_INTERNET) == FALSE) // 拨号到INTERNET
		{
			return ;
		}
#endif
#endif
//		RETAILMSG(1,("Sucess Link to Internet [%x]\r\n",lpHtmlView->hRasConn));
		InsertDownList(hWnd,lpUrl,NULL,NULL);  // 插入当前URL到下载列表，开始下载
}
// **************************************************
// 声明：static  void SMIE_DownLoadUrl( HWND  hWnd,HHTMCTL hHtmlCtl,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hHtmlCtl -- 下载内容所属的点
// 	IN lpUrl -- 要下载的地址
// 
// 返回值：无
// 功能描述：下载一个指定的地址内容
// 引用: 
// **************************************************
static  void SMIE_DownLoadUrl( HWND  hWnd,HHTMCTL hHtmlCtl,LPTSTR lpUrl)
{
	InsertDownList(hWnd,lpUrl,hHtmlCtl,NULL);  // 将当前URL添加到下载列表
}


// **************************************************
// 声明：static DWORD DownLoadAFile(HWND hWnd, LPDOWNLOADITEM lpDownloadItem)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 要下载的地址
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：下载一个指定的文件。
// 引用: 
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

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
		{
//		    CloseUrlHandle( hUrl );
			return 0;
		}
		lpUrl = lpDownloadItem->lpUrl;
		if (lpUrl == NULL)  // URL为空，不需要下载
			return 0;
#ifdef NOINTERNET
		return 0;
#endif
//		RETAILMSG(1,("#####################Will Down load <%s>\r\n",lpUrl));
		if (lpDownloadItem->lpFileName == NULL)
		{
			lpFileName=GetFileNameFromUrl(hWnd,lpUrl);  // 根据URL得到临时文件名
		}
		else
		{
			lpFileName = lpDownloadItem->lpFileName;
		}
		CreateMultiDirectory(lpFileName,NULL);  // 创建目录

		if (lpDownloadItem->lpFileName == NULL)
		{
			if (lpHtmlView->lpUrl==NULL || strcmp(lpUrl,lpHtmlView->lpUrl)!=0 )
			{
				// 不是主页
				hFile=CreateFile( lpFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,NULL, 
										 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,NULL);
				if (hFile != INVALID_HANDLE_VALUE)
				{
					// 文件已经存在
					CloseHandle(hFile);
					bNeedDownLoad = FALSE;
					bRet = TRUE;
				}
			}
		}

		if (bNeedDownLoad)
		{
			// 需要下载当前文件
	//	GetFileNameFromUrl(hWnd,lpUrl);

#ifdef DIALOG_DOWNLOAD
		if (lpDownloadItem->lpFileName || lpHtmlView->lpUrl!=NULL )
		{
			if (lpDownloadItem->lpFileName || strcmp(lpUrl,lpHtmlView->lpUrl)==0 )
			{  
				// 判断是当前主网页
				hDownload = ShowDownloadDialog(hWnd);  // 显示下载对话框
				if (hDownload)
				{
					PostMessage(hDownload,DPM_SETURL,0,(LPARAM)lpUrl); // 设置要下载的URL
//					PostMessage(hDownload,DPM_SETSIZE,0,0); // 设置要下载的URL
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
				PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
			}
#endif

				return 0;
			}
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_SETSTATUS,DOWNLOADSTATUS_OPENURL,0); // 设置正在打开的URL
			}
#endif
			// 打开INTERNET
			lpHtmlView->hInternetSiteInit =  InternetOpen( "Kingmos NE/2.0",INTERNET_OPEN_TYPE_DIRECT,
										 NULL, NULL, INTERNET_INVALID_PORT_NUMBER ); 
//			RETAILMSG(1,("InternetOpen = %x OK!!!\r\n",lpHtmlView->hInternetSiteInit));
			InternetSetStatusCallback(  lpHtmlView->hInternetSiteInit, hWnd , 0 );  // 设置回调窗口
		}

//		RETAILMSG(1,("#####################InternetOpen OK !!!\r\n"));

	//	SetTimer(hWnd,INTERNET_EVENT,100,NULL);

		// 打开一个URL
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
				PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
			}
#endif

			return 0;
		}
		//RETAILMSG(1,("#####################InternetOpenUrl <%X>OK !!!\r\n",hCurrentUrl));
		//RETAILMSG(1,("#####################InternetOpenUrl hOpenCnn=<%X>OK !!!\r\n",hCurrentUrl->hOpenCnn));
		//RETAILMSG(1,("#####################InternetOpenUrl hOpenReq=<%X>OK !!!\r\n",hCurrentUrl->hOpenReq));

		if (lpHtmlView->bStop)
		{  // 停止下载
			DeleteRunningList(	hWnd,hCurrentUrl);  // 删除已经下载完成的URL
//		    CloseUrlHandle( hCurrentUrl ,"ttt1");

#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
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
				DeleteRunningList(	hWnd,hCurrentUrl);  // 删除已经下载完成的URL
	//			CloseUrlHandle( hCurrentUrl ,"ttt2");
				hCurrentUrl = NULL;
				//RETAILMSG(1,(TEXT("HttpQueryQuick [%s] Failure\r\n"), lpUrl));
#ifdef DIALOG_DOWNLOAD
				if (hDownload)
				{
					PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
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
				//获取成功
				break;
			case 3:
				//需要重定向,得到重定向地址
				dwContentLen = sizeof(pszContentType);
				//RETAILMSG(1,("HttpQueryQuick 1 hOpenReq =<%X>\r\n",hCurrentUrl->hOpenReq));
				bRet = HttpQueryInfo(hCurrentUrl->hOpenReq, HTTP_QUERY_LOCATION,(LPVOID)pszContentType, &dwContentLen,&dwIndex);
				//RETAILMSG(1,("HttpQueryQuick 1 hOpenReq =<%X> OK\r\n",hCurrentUrl->hOpenReq));
				// 停止当前的下载
				DeleteRunningList(	hWnd,hCurrentUrl);  // 删除已经下载完成的URL
	//			CloseUrlHandle( hCurrentUrl ,"ttt3");
				hCurrentUrl = NULL;
				if (bRet == TRUE)
				{
					//RETAILMSG(1,("Need Rediert\r\n"));
					if ((lpHtmlView->lpUrl!=NULL) && strcmp(lpUrl,lpHtmlView->lpUrl)==0)
					{
						// 是主页, 重新打开一个新的网页
						SendMessage(hWnd,HM_GO,0,(LPARAM)pszContentType);
					}
					else
					{
						// 将重新在新的地址下载
						hCurrentUrl = IE_OpenUrl( hWnd,lpHtmlView->hInternetSiteInit, pszContentType, g_szacceptQuest, -1, 0, (DWORD)NULL );
						if (hCurrentUrl != NULL)
						{
							// 重新获得地址成功,重新查询当前的状况
							goto RETRY_QUERY;
						}
					}
				}
				//RETAILMSG(1,(TEXT("HttpQueryQuick Failure\r\n"), lpUrl));
#ifdef DIALOG_DOWNLOAD
				if (hDownload)
				{
					PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
				}
#endif
				return TRUE;
			default :
				//有错误出现
				DeleteRunningList(	hWnd,hCurrentUrl);  // 删除已经下载完成的URL
	//			CloseUrlHandle( hCurrentUrl ,"ttt4");
				hCurrentUrl = NULL;
				//RETAILMSG(1,(TEXT("HttpQueryQuick [%s] Failure\r\n"), lpUrl));
#ifdef DIALOG_DOWNLOAD
				if (hDownload)
				{
					PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
				}
#endif
				return 0;
			}
		}

#ifdef DIALOG_DOWNLOAD
		if (hDownload)
		{
			PostMessage(hDownload,DPM_SETSTATUS,DOWNLOADSTATUS_DOWNLOAD,0); // 设置正在打开的URL
		}
		dwUrlSize = GetCurrentUrlSize(hCurrentUrl);
		if (hDownload)
		{
			PostMessage(hDownload,DPM_SETSIZE,dwUrlSize,0); // 设置要下载的URL
		}
#endif
		// 插入当前下载过程到正在下载文件列表
//		InsertRunningList(	hWnd,hCurrentUrl);

//		RETAILMSG(1,("********************* jami 2003.09.25 Start ******************\r\n"));

		//RETAILMSG(1,("###########################Start Down load Url <%s>!!!\r\n",lpUrl));
/*		if (lpDownloadItem->lpFileName == NULL)
		{
			lpFileName=GetFileNameFromUrl(hWnd,lpUrl);  // 根据URL得到临时文件名
		}
		else
		{
			lpFileName = lpDownloadItem->lpFileName;
		}

		CreateMultiDirectory(lpFileName,NULL);  // 创建目录
*/
//		lpFileName=TEMPFILE;
		// 打开文件
		hFile=CreateFile( lpFileName, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,NULL, 
								 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
		if (hFile==INVALID_HANDLE_VALUE)
		{
//			RETAILMSG(1,("##################Down Load <%s>Data Failure!!!\r\n",lpUrl));
			//RETAILMSG(1,("##################Create File <%s> Failure!!!\r\n",lpFileName));
			// 打开文件失败
			if (lpDownloadItem->lpFileName == NULL)
			{
				if (lpFileName)
					free(lpFileName);
			}
			DeleteRunningList(	hWnd,hCurrentUrl);  // 从下载列表中删除
//		    CloseUrlHandle( hCurrentUrl ,"ttt5"); // 停止当前URL下载
//			DeleteDownList(hWnd,hUrl);
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
			}
#endif
			return 0;
		}

//		RETAILMSG(1,("Create Temp File <%s> OK!!!\r\n",lpFileName));
		//RETAILMSG(1,("Begin Down Load Data .......\r\n"));
//#ifdef LOADANIMATION
//		StartAnimation(hWnd);
//#endif
		// 开始下载数据
		dwDownloadSize = 0;
		while( dwNumRet )
		{
			 //RETAILMSG(1,(TEXT("Read Data From [%x]\r\n"),hCurrentUrl->hOpenReq));
			 if( !InternetReadFile( hCurrentUrl->hOpenReq,cBuf,sizeof( cBuf) , &dwNumRet ) )
			 {  // 从INTERNET 读取数据
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

			 WriteFile( hFile, cBuf, dwNumRet,&dwWriteRet,NULL);  // 将数据写到本地文件中
			 dwDownloadSize += dwNumRet;
#ifdef DIALOG_DOWNLOAD
			if (hDownload)
			{
				PostMessage(hDownload,DPM_SETDOWNLOADSIZE,dwDownloadSize,0); // 设置要下载的URL
			}
#endif
			 if (dwUrlSize == dwDownloadSize)
				 break;
		}
		//RETAILMSG(1,("InternetReadFile OK\r\n"));
		SetEndOfFile(hFile);
		CloseHandle(hFile);  // 关闭文件
#ifdef DIALOG_DOWNLOAD
		if (hDownload)
		{
			PostMessage(hDownload,DPM_CLOSE,0,0); // 关闭对话框
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
				PostMessage(hDownload,DPM_SETSTATUS,DOWNLOADSTATUS_COMPLETE,0); // 设置正在打开的URL
			}
#endif
		DeleteRunningList(	hWnd,hCurrentUrl);  // 删除已经下载完成的URL
//	    CloseUrlHandle( hCurrentUrl ,"ttt6");  // 关闭句柄
		}  
		//RETAILMSG(1,("Download Complete\r\n"));

		if (bRet == TRUE && lpHtmlView->lpUrl!=NULL)
		{  // 下载成功
			//RETAILMSG(1,("Download Success\r\n"));
			if (strcmp(lpUrl,lpHtmlView->lpUrl)==0)
			{  // 判断是当前主网页
				if (lpHtmlView->lpCurFile)
					free(lpHtmlView->lpCurFile);  // 删除当前文件的设置
//				lpHtmlView->lpCurFile=SMIE_BufferAssignTChar(lpFileName);
				lpHtmlView->lpCurFile=BufferAssignTChar(lpFileName);  // 设置已经下载下来的文件为当前文件
				SMIE_SetCurPath(hWnd,lpHtmlView->lpCurFile);  // 设置当前路径
//				RETAILMSG(1,("M000MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
				PostMessage(hWnd,HM_REFRESH,0,0);  // 刷新窗口
//				RETAILMSG(1,("Load File Time = %d\r\n",GetTickCount() - iStartTick));
//				RETAILMSG(1,("DownLoad  \"%s\" Complete !!!\r\n",lpHtmlView->lpUrl));
//				RETAILMSG(1,("MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
//				Sleep(2000);
				//RETAILMSG(1,("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\r\n"));
			}
		}
// !!! Delete By Jami chen in 2004.09.23
		else 
		{ // 不是主页，要保证文件的完整性，没有下载完成，删除原来文件
			//RETAILMSG(1,("Download Failure\r\n"));
			if (dwUrlSize != dwDownloadSize)
			{
				// 下载失败
				//RETAILMSG(1,("Delete File [%s]\r\n",lpFileName));
				DeleteFile(lpFileName);
			}
		}
// !!! Delete End By Jami chen in 2004.09.23
//				hControl = GetControl(hWnd,hUrl);
		if (bRet == TRUE && lpDownloadItem->hControl)
		{  // 下载控件(图象)成功
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
//		DeleteRunningList(	hWnd,hCurrentUrl);  // 删除已经下载完成的URL
//	    CloseUrlHandle( hCurrentUrl );  // 关闭句柄
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
// 声明：static BOOL InsertDownList(	HWND hWnd,LPTSTR lpUrl,HHTMCTL hControl,LPTSTR lpFileName)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpUrl -- 要下载的地址
// 	IN hControl -- 要下载的地址所属的点
//  IN lpFileName -- 要将下载的文件下载到指定的文件下
// 
// 返回值：成功返回TRUE，否则返回TRUE。
// 功能描述：插入一个要下载的地址到下载列表中。
// 引用: 
// **************************************************
//static BOOL InsertDownList(	HWND hWnd,HINTERNET hUrl ,LPTSTR lpUrl,HHTMCTL hControl)
static BOOL InsertDownList(	HWND hWnd,LPTSTR lpUrl,HHTMCTL hControl,LPTSTR lpFileName)
{
	LPDOWNLOADITEM lpDownLoadItem;
	LPHTMLVIEW lpHtmlView;


//		RETAILMSG(1,("********************* jami 2003.09.25 Start ******************\r\n"));
//		RETAILMSG(1,("Will Insert Url <%s> --> <%X>!!!\r\n",lpUrl,hUrl));
//		RETAILMSG(1,("########### Insert a Url<%s> to Download list\r\n",lpUrl));

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;
	
		LOCK(lpHtmlView->DownLoadCriticalSection);  // 上锁
//		RETAILMSG(1,("Copy Url <%s>!!!\r\n",lpUrl));
		lpDownLoadItem=(LPDOWNLOADITEM)malloc(sizeof(DOWNLOADITEM));  // 分配DOWNLOADITEM内存
		if (lpDownLoadItem==NULL)
		{  // 分配失败
			UNLOCK(lpHtmlView->DownLoadCriticalSection);
			return FALSE;
		}
		
		// 给结构赋值
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
		UNLOCK(lpHtmlView->DownLoadCriticalSection);  // 解锁
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
// 声明：static LPDOWNLOADITEM GetDownLoadUrl(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：要下载的地址，NULL表示没有要下载的地址
// 功能描述：得到下一个要下载的地址。
// 引用: 
// **************************************************
static LPDOWNLOADITEM GetDownLoadUrl(HWND hWnd)
{
	LPDOWNLOADITEM lpDownLoadItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->DownLoadCriticalSection);  // 上锁
		lpDownLoadItem=lpHtmlView->lpDownLoadList;
		if (lpDownLoadItem)
		{  // 将列表指向下一个结点
			lpHtmlView->lpDownLoadList = lpDownLoadItem->next;
		}
		UNLOCK(lpHtmlView->DownLoadCriticalSection);  // 解锁
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
// 声明：BOOL DeleteDownList(	HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：删除等待下载列表。
// 引用: 
// **************************************************
BOOL DeleteDownList(	HWND hWnd)
{
	LPDOWNLOADITEM lpDownLoadItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->DownLoadCriticalSection);  // 上锁
		lpDownLoadItem=lpHtmlView->lpDownLoadList;  // 得到列表头
		while(lpDownLoadItem)
		{
			lpNextItem = lpDownLoadItem->next;  // 得到下一个结点
			if (lpDownLoadItem->lpUrl)  // 删除当前结点
				free(lpDownLoadItem->lpUrl);
			if (lpDownLoadItem->lpFileName)  // 删除当前结点
				free(lpDownLoadItem->lpFileName);
			free(lpDownLoadItem);
			lpDownLoadItem=lpNextItem;  // 指向下一结点
		}
		lpHtmlView->lpDownLoadList = NULL;  // 列表指空
		UNLOCK(lpHtmlView->DownLoadCriticalSection); // 解锁
//#ifdef LOADANIMATION
//		EndAnimation(hWnd);
//#endif
		return TRUE;
}

// **************************************************
// 声明：BOOL DownLoadImage(HWND hWnd,HHTMCTL hHtmlCtl,LPTSTR lpImage)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hHtmlCtl -- 指定图象的控制句柄
// 	IN lpImage -- 要下在载的图象地址
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：下载一个指定的图象。
// 引用: 
// **************************************************
BOOL DownLoadImage(HWND hWnd,HHTMCTL hHtmlCtl,LPTSTR lpImage)
{
	LPHTMLVIEW lpHtmlView;
	LPTSTR lpFullFileName;
//	int i;

//	RETAILMSG(1,("####################################\r\n"));
	//RETAILMSG(1,("Will Download Image <%s>\r\n",lpImage));
	lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //得到HTMLVIEW结构
	if (lpHtmlView==NULL)
		return 0;
	if (lpHtmlView->lpCurUrl->lpUrl==NULL)  // 当前URL不存在
		return FALSE;
	if (SMIE_IsLocateFile(hWnd,lpHtmlView->lpCurUrl->lpUrl)==TRUE)  // 是本地文件
	{  // 超文本文件是本地文件,则图象文件可能是一个本地文件，也可能是一个绝对路径的网络文件
		LPTSTR lpFullImage;

		//RETAILMSG(1,("Open File is local file <%s>\r\n",lpHtmlView->lpCurUrl->lpUrl));
		if (strnicmp(lpImage,INTERNET_HEAD,5) == 0)
		{  // 是一个网络文件，开始下载
			#ifdef INTERNET_SUPPORT
				SMIE_DownLoadUrl(hWnd,hHtmlCtl,lpImage);  // 下载指定的文件
			#endif
				return TRUE;  // 返回成功
		}

		// 图象文件是本地文件
		lpFullImage=SMIE_GetFullImage(hWnd,lpImage); // 得到图象文件的全路径名
		if (lpFullImage==NULL)
			return FALSE; // 没有得到全路径文件名
		DownloadImageOK(hWnd, hHtmlCtl,lpFullImage); 
		free(lpFullImage);
		return FALSE;
	}

	// get the image full address in internet
// !!! Delete By Jami chen in 2004.09.09
//	lpFullFileName=(LPTSTR)malloc(strlen(lpHtmlView->lpCurUrl->lpUrl)+strlen(lpImage)+4);  //得到最大文件长度
//	if (lpFullFileName==NULL)
//		return FALSE;
// !!! Delete By Jami chen in 2004.09.09
//	RETAILMSG(1,("lpImage = %x \r\n",lpImage));
	if (strnicmp(lpImage,INTERNET_HEAD,5))
	{  //是相对路径
		//RETAILMSG(1,("Image is a sub dir <%s>\r\n",lpImage));
		// 得到全路径
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
	{  // 是绝对路径
// !!! modified By Jami chen in 2004.09.09
//		strcpy(lpFullFileName,lpImage);
		lpFullFileName = BufferAssignTChar(lpImage);
// !!! modified End By Jami chen in 2004.09.09
	}
	if (lpFullFileName==NULL)
		return FALSE;
//	SMIE_StartNewFtpSite(hWnd,lpFullFileName);
#ifdef INTERNET_SUPPORT
	SMIE_DownLoadUrl(hWnd,hHtmlCtl,lpFullFileName);  // 下载指定的文件
#endif
	free(lpFullFileName);  // 释放不用的缓存
	return TRUE;  // 返回成功
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
// 声明：static void StartAnimation(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：开始动画。
// 引用: 
// **************************************************
static void StartAnimation(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;

//		RETAILMSG(1,("StartAnimation ...\r\n"));
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return ;
		if (lpHtmlView->hAnimation == NULL)
		{  // 动画还没有启动
			lpHtmlView->bExitAnimation = FALSE;
			lpHtmlView->hAnimation = CreateThread(NULL, 0, LoadAnimateProc, (void *)hWnd, 0, 0 );// 创建显示动画线程
		}
//		RETAILMSG(1,("StartAnimation OK !!!\r\n"));
}
// **************************************************
// 声明：static void EndAnimation(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：结束动画。
// 引用: 
// **************************************************
static void EndAnimation(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;
	RECT rect;

//		RETAILMSG(1,("EndAnimation ...\r\n"));
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return ;
		if (lpHtmlView->hAnimation == NULL)  // 动画没有启动
			return ;
		lpHtmlView->bExitAnimation = TRUE;  // 退出动画标志
		//Sleep(400);
		WaitForSingleObject(lpHtmlView->hAnimation,400);  // 暂停，等待退出
//		RETAILMSG(1,("Wait Exit Thread ...\r\n"));
		if (lpHtmlView->hAnimation)  // 关闭句柄
			CloseHandle(lpHtmlView->hAnimation);
		lpHtmlView->hAnimation = NULL;
		//RETAILMSG(1,(TEXT("******************* Will Redraw Window\r\n")));
//		InvalidateRect(hWnd,NULL,TRUE);
//		Sleep(3000);

		// 需要重新刷新显示动画位置的矩形
		GetClientRect(hWnd,&rect);
		// 得到动画的位置
		rect.left = (rect.right - 32)/2;
		rect.top = (rect.bottom - 32)/2;
		rect.right =  rect.left + 32;
		rect.bottom = rect.top + 32;
		InvalidateRect(hWnd,&rect,TRUE); // 刷新动画位置的窗口
//		RETAILMSG(1,("EndAnimation OK\r\n"));
}
// **************************************************
// 声明：static DWORD WINAPI LoadAnimateProc(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：动画线程过程。
// 引用: 
// **************************************************
static DWORD WINAPI LoadAnimateProc(VOID * pParam)
{
	LPHTMLVIEW lpHtmlView;
	static int iPic = 0;
//	HWND hWnd = (HWND)(*(HWND *)pParam);
	HWND hWnd = (HWND)pParam;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return 0;

		while(lpHtmlView->bExitAnimation == FALSE)
		{
//			 RETAILMSG(1,("******** Show (%d) picture!!!\r\n",iPic));
			 ShowAnimation(hWnd, iPic);  // 显示一幅动画
			 iPic ++;  // 到下一幅
			 if (iPic >= MAX_PICNUM)  // 到最后一幅
				 iPic = 0;  // 回到第一幅
			 Sleep(200);// 暂停
		}
		return 0;
}

// **************************************************
// 声明：static void ShowAnimation(HWND hWnd, int iPic)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iPic -- 要显示的动画的页索引
// 
// 返回值：无
// 功能描述：显示动画的一页。
// 引用: 
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
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // 得到窗口实例句柄
		GetClientRect(hWnd,&rect); // 得到窗口矩形
//		hIcon = LoadImage( hInstance, MAKEINTRESOURCE(iIconID[iPic]), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE) ;
		hBitmap = LoadImage( hInstance, MAKEINTRESOURCE(iIconID[iPic]), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE) ;  // 得到位图
//		if (hIcon)
		if (hBitmap)
		{
			hdc = GetDC(hWnd);  // 得到HDC
			//DrawIcon(hdc,(rect.right - 32)/2,(rect.bottom - 32)/2,hIcon);
			hMemDC =  CreateCompatibleDC( hdc );  //得到内存DC
			hBitmap = SelectObject( hMemDC, hBitmap );  // 将位图选择到内存DC
        
			//GetWindowExtEx( hdc, &size );
			// 绘制位图
			BitBlt( hdc, (rect.right - 32)/2,(rect.bottom - 32)/2, 32, 32, hMemDC, 0, 0, SRCCOPY );
			hBitmap = SelectObject( hMemDC, hBitmap );  //恢复原来的位图
			DeleteDC( hMemDC );
			ReleaseDC(hWnd,hdc);
			//DestroyIcon(hIcon);
			DeleteObject(hBitmap);
		}
		else
		{
			DWORD dwErr=GetLastError();  // 得到错误代码
		}
}
#endif


#ifdef DOWNLOAD_PROCESS1

// !!! Add By Jami chen in 2004.09.16
// **************************************************
// 声明：static DWORD WINAPI DownLoadThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：下载线程
// 引用: 
// **************************************************
static DWORD WINAPI DownLoadThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPDOWNLOADITEM lpDownloadItem;
	LPDOWNLOADPARAM lpDownloadParam;

		lpDownloadParam = (LPDOWNLOADPARAM)pParam;  // 下载参数
		if (lpDownloadParam == NULL)
			return FALSE;
		hWnd = lpDownloadParam->hWnd;
		lpDownloadItem = lpDownloadParam->lpDownloadItem;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;
//		lpHtmlView->iRunThreadCount ++;  // 正在下载线程
//		RETAILMSG(1,("############ Will Download <%s>\r\n",lpDownloadItem->lpUrl));
		if (DownLoadAFile(hWnd, lpDownloadItem))
		{
			// 下载当前文件
//				hControl = GetControl(hWnd,hUrl);
/*			if (!lpHtmlView->bExit && lpDownloadItem->hControl)
			{  // 下载成功
				RECT rect={0,0,0,0};
				HDC hdc;
				POINT ptOrg;

					ptOrg.x = lpHtmlView->x_Org;
					ptOrg.y = lpHtmlView->y_Org;
					// 显示该控件
					hdc = GetDC(hWnd);
					SMIE_ShowHtmlControl(hWnd,hdc,lpDownloadItem->hControl,rect,ptOrg);
					ReleaseDC(hWnd,hdc);
			}
*/
		}
		else
		{
			// 下载文件失败
			if (!lpHtmlView->bExit && lpDownloadItem->hControl == NULL)
			{  // 是主网页，重新刷新网页
				if (lpHtmlView->lpCurFile)
					free(lpHtmlView->lpCurFile);
				lpHtmlView->lpCurFile=NULL;
//				RETAILMSG(1,("M111MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
				PostMessage(hWnd,HM_REFRESH,0,0);
			}
		}
//		RETAILMSG(1,("############ Download <%s> OK\r\n",lpDownloadItem->lpUrl));
		// 释放该结点的空间
		if (lpDownloadItem->lpUrl)
			free(lpDownloadItem->lpUrl);
		if (lpDownloadItem->lpFileName)
			free(lpDownloadItem->lpFileName);
		free(lpDownloadItem);

		free(lpDownloadParam);
		lpHtmlView->iRunThreadCount --;  // 减少一个下载线程
		return 0;
}

// **************************************************
// 声明：DWORD WINAPI DownLoadmanageThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：下载管理线程。
// 引用: 
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
		hWnd = (HWND)pParam;  // 得到参数
		if (hWnd == NULL)
			return FALSE;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
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
				// 开始下载文件
#ifdef LOADANIMATION
				StartAnimation(hWnd);  // 开始动画
#endif
			}
			if (preRunThread > 0 && lpHtmlView->iRunThreadCount == 0)
			{
				// 文件已经下载完毕
#ifdef LOADANIMATION
				EndAnimation(hWnd);  // 结束动画
#endif
			}
			preRunThread = lpHtmlView->iRunThreadCount;

			if (lpHtmlView->iRunThreadCount < THREADNUM)  // 正在下载文件总数 是否小于可以最大下载的 线程数
			{
//				RETAILMSG(1,("########### Search Next Url to Download \r\n"));
				// 可以继续下载下一个文件
				lpDownloadItem = GetDownLoadUrl(hWnd);
				if (lpDownloadItem == NULL)
				{
					// 没有要等待下载的文件
					Sleep(200);
					continue;
				}
//				RETAILMSG(1,("########### Have a File To be Download \r\n"));
				// 添加一个下载
				lpDownloadParam = (LPDOWNLOADPARAM)malloc(sizeof(DOWNLOADPARAM));  // 分配内存
				if (lpDownloadParam)
				{
					lpDownloadParam->hWnd = hWnd;
					lpDownloadParam->lpDownloadItem = lpDownloadItem;
					handle = CreateThread(NULL, 0, DownLoadThread, (void *)lpDownloadParam, 0, 0 );
					CloseHandle(handle);
					lpHtmlView->iRunThreadCount ++;  // 正在下载线程
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
			// 等待所有线程退出
			iTimeout ++;
			//RETAILMSG(1,("########### Have (%d) Download Thread Running!!\r\n",lpHtmlView->iRunThreadCount));
//			if (iTimeout >= 100)
//				break;
			Sleep(200);
		}
		
		lpHtmlView->bExit = FALSE;  // 成功退出
		return 0;
}
#endif

#ifdef DOWNLOAD_PROCESS2
// **************************************************
// 声明：static DWORD WINAPI DownLoadThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：文件下载线程
// 引用: 
// **************************************************
static DWORD WINAPI DownLoadThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPDOWNLOADITEM lpDownloadItem;

		hWnd = (HWND)pParam;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0); //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;
		while(TRUE)
		{
			WaitForSingleObject(lpHtmlView->hDownloadEvent,INFINITE); // 等待需要下载的文件
			if (lpHtmlView->bExit)
				break;
			ResetEvent(lpHtmlView->hDownloadEvent);
			while(TRUE)
			{
				lpDownloadItem = GetDownLoadUrl(hWnd);  // 得到要下载的文件
				if (lpDownloadItem == NULL)
				{
					// 没有要等待下载的文件
					break;
				}
				// 下载文件
				if (DownLoadAFile(hWnd, lpDownloadItem))
				{
					// 下载当前文件成功
				}
				else
				{
					// 下载文件失败
					if (!lpHtmlView->bExit && lpDownloadItem->hControl == NULL)
					{  // 是主网页，重新刷新网页
						if (lpHtmlView->lpCurFile)
							free(lpHtmlView->lpCurFile);
						lpHtmlView->lpCurFile=NULL;
		//				RETAILMSG(1,("M111MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM\r\n"));
						PostMessage(hWnd,HM_REFRESH,0,0);
					}
				}
		//		RETAILMSG(1,("############ Download <%s> OK\r\n",lpDownloadItem->lpUrl));
				// 释放该结点的空间
				if (lpDownloadItem->lpUrl)
					free(lpDownloadItem->lpUrl);
				if (lpDownloadItem->lpFileName)
					free(lpDownloadItem->lpFileName);
				free(lpDownloadItem);
			}
		}
		lpHtmlView->iRunThreadCount --;  // 减少一个下载线程
		return 0;
}

// **************************************************
// 声明：DWORD WINAPI DownLoadmanageThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：下载管理线程。
// 引用: 
// **************************************************
DWORD WINAPI DownLoadmanageThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	int iTimeout = 0;
	HANDLE handle;
	int i;

//		RETAILMSG(1,("########### Will Run Manage Thread... \r\n"));
		hWnd = (HWND)pParam;  // 得到参数
		if (hWnd == NULL)
			return FALSE;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;
//		RETAILMSG(1,("########### Run Manage Thread Success \r\n"));
		for (i = 0; i < THREADNUM; i++)  //  创建指定个数的文件下载线程数
		{
			handle = CreateThread(NULL, 0, DownLoadThread, (void *)hWnd, 0, 0 );
			CloseHandle(handle);
			lpHtmlView->iRunThreadCount ++;  // 正在下载线程
		}

		while(1)
		{
			WaitForSingleObject(lpHtmlView->hExitEvent,INFINITE); // 等待系统退出
			if (lpHtmlView->bExit)
				break;
		}
		SetEvent(lpHtmlView->hDownloadEvent);
		iTimeout = 0;
		while(lpHtmlView->iRunThreadCount)
		{
			// 等待所有线程退出
			iTimeout ++;
			//RETAILMSG(1,("########### Have (%d) Download Thread Running!!\r\n",lpHtmlView->iRunThreadCount));
			Sleep(200);
		}
		
		lpHtmlView->bExit = FALSE;  // 成功退出
		return 0;
}
#endif

// **************************************************
// 声明：BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hUrl -- 指定的URL句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：江一个指定的URL插入到当前正在下载的队列中。
// 引用: 
// **************************************************
BOOL InsertRunningList(	HWND hWnd,LPURLHANDLE hUrl)
{
	LPRUNNINGITEM lpRunningItem;
	LPHTMLVIEW lpHtmlView;



		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;
	
		LOCK(lpHtmlView->RunningCriticalSection);  // 上锁
		lpRunningItem=(LPRUNNINGITEM)malloc(sizeof(RUNNINGITEM));  // 分配内存
		if (lpRunningItem==NULL)
		{  // 分配失败
			UNLOCK(lpHtmlView->RunningCriticalSection);
			return FALSE;
		}
		// 设置数据
		lpRunningItem->hUrl= hUrl;
		// 插入数据到最前面
		lpRunningItem->next=lpHtmlView->lpRunningList;
		lpHtmlView->lpRunningList=lpRunningItem;
		UNLOCK(lpHtmlView->RunningCriticalSection);  // 解锁
		return TRUE;		
}
// **************************************************
// 声明：BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hUrl -- URL句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：从当前正在下载的队列中删除指定的URL。
// 引用: 
// **************************************************
BOOL DeleteRunningList(	HWND hWnd,LPURLHANDLE  hUrl)
{
	LPRUNNINGITEM lpRunningItem,lpPreItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->RunningCriticalSection);  // 上锁
		lpRunningItem=lpHtmlView->lpRunningList;  // 得到第一个运行列表结点
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
		UNLOCK(lpHtmlView->RunningCriticalSection);  // 解锁
		return FALSE;
}
// **************************************************
// 声明：BOOL CloseRunningList(	HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：成功返回TRUE， 否则返回FALSE
// 功能描述：关闭正在下载的队列。
// 引用: 
// **************************************************
BOOL CloseRunningList(	HWND hWnd)
{
	LPRUNNINGITEM lpRunningItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;


		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
			return FALSE;

		LOCK(lpHtmlView->RunningCriticalSection);  // 上锁
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
		lpHtmlView->lpRunningList = NULL;  // 设置正在下载列表为空
		UNLOCK(lpHtmlView->RunningCriticalSection);  // 解锁
		return FALSE;
}

// !!! Add End By Jami chen in 2004.09.16

// **************************************************
// 声明：void SMIE_DownLoadSpecialFile( HWND  hWnd,LPTSTR lpUrl)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN lpUrl -- 指定的要下载的文件URL
// 
// 返回值：无
// 功能描述：下载一个指定的文件。
// 引用: 
// **************************************************
void SMIE_DownLoadSpecialFile( HWND  hWnd,LPTSTR lpUrl)
{
	TCHAR szFileName[MAX_PATH];
	LPTSTR lpOrgFileName = NULL,lpFileName;
	BOOL bRet;
	RETAILMSG(1, ("SMIE_DownLoadSpecialFile (%s) \r\n", lpUrl));
	lpOrgFileName=GetFileNameFromUrl(hWnd,lpUrl);  // 根据URL得到临时文件名
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

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);   //得到HTMLVIEW结构
		if (lpHtmlView==NULL)
		{
			return ;
		}
		// 打开网页之前首先要拨号到网络
#if 0
		if (DailToInternet(hWnd,&lpHtmlView->hRasConn,GPRS_NETTYPE_INTERNET) == FALSE) // 拨号到INTERNET
		{
			// 不能连接到网络
#ifdef ZT_PHONE
			ZTMessageBox(hWnd,"不能连接到网络","错误",MB_OK);
#else
			MessageBox(hWnd,"不能连接到网络","错误",MB_OK);
#endif
			return ;
		}
#else
		if (LinkNet(NULL,GPRS_NETTYPE_INTERNET) == FALSE) // 拨号到INTERNET
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
声明：static void  GetSaveFile( HWND hwnd,CHAR *szDefault,CHAR *szFileName)
参数：
IN    szDefault    - 打开对话筐时的添入的默认文件名
IN    hwnd         - OpenFile 对话框的父窗口句柄
IN    szFileName   - 返回的文件名，为全路径字符串
返回：void    
功能描述:保存附件
引用： 添件加附件时调用 
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
	of.lpstrInitialDir   = TEXT("\\临时");
	of.lpstrTitle        = TEXT("选择保存文件名");//NULL;
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
// 声明：static BOOL InitialInternet(void)
// 参数：
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：初始化Internet。
// 引用: 
// **************************************************
static BOOL InitialInternet(HWND hWnd)
{
	WSADATA		WSAData;
	int			iErrCode;
	
	if( iErrCode=WSAStartup( MAKEWORD(1,1), &WSAData ) )
	{
		TCHAR	pszErr[100];
		
		//严重错误提示，并要求退出
		sprintf( pszErr, "启动Windows Socket功能出错（代码=%d），要求退出程序！", WSAGetLastError() );
		MessageBox( hWnd, pszErr, "Err", MB_OK );
		return FALSE;
	}
	return TRUE;
}

// !!! Add By Jami chen in 2004.09.14






// *******************************************************************************
// 显示下载的线程
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
    { WS_EX_NOMOVE|WS_EX_TITLE,WS_POPUP|WS_VISIBLE|WS_BORDER,PROCESS_ITEMNUM,WORK_AREA_STARTX,30,WORK_AREA_WIDTH,200,0,0,"下载" },
		{
			//下载状态
			{ 0,WS_CHILD | WS_VISIBLE , 10, 4, 240, 30, ID_DOWNSTATUS, classSTATIC, "下载文件...", 0 },
			//文件名
			{ 0,WS_CHILD | WS_VISIBLE | SS_RIGHT, 10, 32, 240, 30, ID_FILENAME, classSTATIC, "", 0 },
			//文件大小
			{ 0,WS_CHILD | WS_VISIBLE , 10, 60, 108, 30, 0XFFFF, classSTATIC, "文件大小：", 0 },
			{ 0,WS_CHILD | WS_VISIBLE , 120, 60, 100, 30, ID_SIZE, classSTATIC, "", 0 },
			//已下载的文件大小
			{ 0,WS_CHILD , 10, 90, 108, 30, ID_DOWNSIZETAG, classSTATIC, "已经下载：", 0 },
			{ 0,WS_CHILD , 120, 90, 100, 30, ID_DOWNSIZE, classSTATIC, "", 0 },

			{ 0,WS_CHILD | PBS_SMOOTH | WS_BORDER, 10, 90, 220, 30, ID_DOWNPROCESS, classPROGRESS, "", 0 },
			//取消按钮
#ifdef ZT_PHONE
			{ 0,WS_CHILD | WS_VISIBLE , 80, 130, 80, 30, ID_CANCEL, classZTBUTTON, "取消", 0 },
#else
			{ 0,WS_CHILD | WS_VISIBLE , 80, 130, 80, 30, ID_CANCEL, classBUTTON, "取消", 0 },
#endif
		} 
    };

typedef struct tagDOWNLOADTHREAD{
	HWND hParent;  // 父窗口句柄
	HWND hDownloadDlg; // 下载窗口句柄
}DOWNLOADTHREAD, *LPDOWNLOADTHREAD;


//static HANDLE g_hThread = NULL;
//static HWND g_hCopyDlg = NULL;

typedef struct tagDOWNLOADDLGDATA{
	HWND hParent;
	int iTotalSize; // 需要下载的尺寸
	int iDownloadedSize; // 已经下载的尺寸
}DOWNLOADDLGDATA, *LPDOWNLOADDLGDATA;

static LRESULT CALLBACK DownloadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI DownloadDialogThread(VOID * pParam);
static LRESULT DoDownloadInitial(HWND hDlg,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDownLoadURL(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDownLoadSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTotalSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDownloadStatus(HWND hWnd,WPARAM wParam,LPARAM lParam);


// **************************************************
// 声明：static HWND ShowDownloadDialog(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：返回下载对话框的窗口句柄
// 功能描述：显示正在复制文件对话框。
// 引用: 
// 提示：显示方法是先创建一个线程，有该线程弹出提示框，并等待复制完成
//	   而当前线程则继续进行复制过程，复制完成，发送消息杀死对话框。
// **************************************************
static HWND ShowDownloadDialog(HWND hWnd)
{
	DOWNLOADTHREAD tagDownloadProcess;
	HANDLE handle;
	int iTimes = 0;

		tagDownloadProcess.hParent = hWnd;
		tagDownloadProcess.hDownloadDlg = NULL;
		handle = CreateThread(NULL, 0, DownloadDialogThread, (void *)&tagDownloadProcess, 0, 0 );  // 创建一个线程
		CloseHandle(handle); // 关闭句柄，创建成功后，该句柄无效。

		while(1)
		{
			if (tagDownloadProcess.hDownloadDlg)
				break; // 窗口已经创建完成
			Sleep(50); // 等待创建窗口
			if (iTimes >= 300)
				break; // 最长等待9 秒
			iTimes ++;
		}
		if (tagDownloadProcess.hDownloadDlg == (HWND)-1)
			return NULL;
		return tagDownloadProcess.hDownloadDlg;
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
static void CloseCopyDialog(HWND hDownloadWnd)
{
		if (hDownloadWnd)
		{ // 已经创建了复制对话框
			//RETAILMSG(1,(TEXT("End Dialog !!!!\r\n")));
			PostMessage(hDownloadWnd,DPM_CLOSE,0,0); // 发送消息退出该对话框
		}
}
// ********************************************************************
// 声明：static LRESULT CALLBACK DownloadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
static LRESULT CALLBACK DownloadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			DoDownloadInitial(hDlg,wParam,lParam);
//			g_hCopyDlg = GetWindow(hDlg,GW_OWNER);
			return 0;
		case DPM_CLOSE: // 关闭对话框
			EndDialog(hDlg,0);
			return TRUE;
		case DPM_SETURL: // 关闭对话框
			DoSetDownLoadURL(hDlg,wParam,lParam);
			return TRUE;
		case DPM_SETSIZE: // 关闭对话框
			DoSetTotalSize(hDlg,wParam,lParam);
			return TRUE;
		case DPM_SETDOWNLOADSIZE: // 关闭对话框
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
								PostMessage(lpDownloadDlgData->hParent,HM_STOP,0,0);  // 停止当前下载
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
// 声明：static LRESULT DoDownloadInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
// 参数：
//	  IN hWnd - 窗口句柄
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：初始化复制显示窗口
// 引用: 
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
// 声明：static DWORD WINAPI DownloadDialogThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：复制窗口线程。
// 引用: 
// **************************************************
static DWORD WINAPI DownloadDialogThread(VOID * pParam)
{
	HINSTANCE hInstance;
	LPDOWNLOADTHREAD lpDownloadProcess;

		lpDownloadProcess = (LPDOWNLOADTHREAD)pParam;
		hInstance = (HINSTANCE)GetWindowLong(lpDownloadProcess->hParent,GWL_HINSTANCE); // 得到实例句柄
		if (DialogBoxIndirectParamEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_DownloadProcess,lpDownloadProcess->hParent, (DLGPROC)DownloadDialogProc,(LONG)pParam) == -1) // 创建对话框
		{
			// 创建对话框失败
			lpDownloadProcess->hDownloadDlg = (HWND)-1;
		}		

		return 0;
}

// **************************************************
// 声明：static LRESULT DoSetDownLoadURL(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- (LPTSTR)当前要下载的URL
// 
// 返回值：无
// 功能描述：设置当前要下载的URL
// 引用: 
// **************************************************
static LRESULT DoSetDownLoadURL(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	SetWindowText(GetDlgItem(hWnd,ID_FILENAME),(LPTSTR)lParam);
	return 0;
}

// **************************************************
// 声明：static LRESULT DoSetTotalSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 当前URL的尺寸
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：设置当前URL的尺寸
// 引用: 
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
			strcpy(lpSize,"未知");
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
// 声明：static LRESULT DoSetDownLoadSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 当前已经下载的尺寸
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：设置当前已经下载的尺寸
// 引用: 
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
// 声明：static LRESULT DoSetDownloadStatus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 当前的状态
// 	IN lParam -- 保留
// 
// 返回值：无
// 功能描述：设置当前的状态
// 引用: 
// **************************************************
static LRESULT DoSetDownloadStatus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hChild;
	DWORD dwStatus;
	LPTSTR lpStatus;

			dwStatus = wParam;

			if (dwStatus == DOWNLOADSTATUS_OPENURL)
			{
				lpStatus = "正在打开...";
			}
			if (dwStatus == DOWNLOADSTATUS_DOWNLOAD)
			{
				lpStatus = "正在下载...";
			}
			
			if (dwStatus == DOWNLOADSTATUS_COMPLETE)
			{
				lpStatus = "下载完成。";
			}

			hChild = GetDlgItem(hWnd,ID_DOWNSTATUS);
			SetWindowText(hChild,lpStatus);
			UpdateWindow(hChild);

			return TRUE;
}


// **************************************************
// 声明：static int GetCurrentUrlSize(LPURLHANDLE hCurrentUrl)
// 参数：
// 	IN hCurrentUrl -- 当前已经打开的URL句柄
// 
// 返回值：当前打开的URL的大小
// 功能描述：得到当前打开的URL的大小
// 引用: 
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
// 声明：static	BOOL INetHlp_LookUrl( LPCSTR pszScheme2, DWORD dwLenScheme2, LPCSTR pszUrl, OUT WORD* pwSrvPort, OUT LPSTR pszSrvName, DWORD dwLenSrv, OUT LPSTR pszPath, DWORD dwLenPath )
// 参数：
// 	IN pszScheme2 -- Scheme字串
//  IN dwLenScheme2 -- Scheme字串的长度
//  IN pszUrl -- 指定的URL
//  OUT pwSrvPort -- 得到服务器的端口
//  OUT pszSrvName -- 得到服务器的名称
//  IN dwLenSrv -- 存放服务器名称的缓存大小
//  OUT pszPath -- 得到当前URL在指定服务器中的路径名
//  IN dwLenPath -- 存放路径名称的缓存大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前指定URL的SERVER
// 引用: 
// **************************************************
static	BOOL	INetHlp_LookUrl( LPCSTR pszScheme2, DWORD dwLenScheme2, LPCSTR pszUrl, OUT WORD* pwSrvPort, 
						OUT LPSTR pszSrvName, DWORD dwLenSrv, OUT LPSTR pszPath, DWORD dwLenPath )
{
	LPSTR				pszTmp;
	DWORD				i;
	DWORD				dwTmp;

	//基本检查
	ASSERT( pszScheme2 && pszSrvName && pszPath && (dwLenSrv>2) && (dwLenPath>2) );
	//判断Scheme
	if( strnicmp( pszUrl, pszScheme2, dwLenScheme2 )==0 )
	{
		//判断和忽略Scheme
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

	//检查出来 服务器名称
	*pwSrvPort = INTERNET_DEFAULT_HTTP_PORT;
	i = 0;
	while( *pszTmp && (*pszTmp!='/') && i<dwLenSrv )
	{
		pszSrvName[i++] = *pszTmp ++;
	}
	pszSrvName[i] = 0;
	if( i>=dwLenSrv )
	{
		//说明空间太小
		return FALSE;
	}
	
	//检查出来 路径名称
	if( *pszTmp=='/' )
	{
		dwTmp = strlen( pszTmp );
		if( dwTmp>=dwLenPath )
		{
			//说明空间太小
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
// 声明：static LPURLHANDLE IE_OpenUrl( HWND hWnd,HINTERNET hInetOpen, LPCSTR lpszUrl, LPCSTR lpszHdrs, DWORD dwLenHdrs, DWORD dwFlags, DWORD dwContext )
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN hInetOpen -- 当前已经打开的INTERNET句柄
//  IN lpszUrl -- 当前要打开的URL地址
//  IN lpszHdrs -- 要打开的URL类型格式
//  IN dwLenHdrs -- 要打开的URL类型格式的长度
//  IN dwFlags -- 打开的标志
//  IN dwContext -- 一个用户自定义的值
// 
// 返回值：当前打开的URL的句柄
// 功能描述：打开指定的URL
// 引用: 
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
	// 插入当前下载过程到正在下载文件列表
	InsertRunningList(	hWnd,lpUrlHandle);

	//RETAILMSG(1,(TEXT("InsertRunningList ok\r\n")));
	//
	lpUrlHandle->hOpenReq = HttpOpenRequest( lpUrlHandle->hOpenCnn, (LPCSTR)HTTP_GET, lpszPath, (LPCSTR)HTTP_VERSION, 0, lpszHdrs, 0, (DWORD)NULL );
	if( !lpUrlHandle->hOpenReq )
	{
		DeleteRunningList(	hWnd,lpUrlHandle);  // 删除已经下载完成的URL
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
		DeleteRunningList(	hWnd,lpUrlHandle);  // 删除已经下载完成的URL
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
// 声明：static void CloseUrlHandle( LPURLHANDLE lpUrlHandle ,LPTSTR lpDump)
// 参数：
// 	IN lpUrlHandle -- 当前已经打开的URL句柄
// 
// 返回值：无
// 功能描述：关闭当前打开的URL句柄
// 引用: 
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
