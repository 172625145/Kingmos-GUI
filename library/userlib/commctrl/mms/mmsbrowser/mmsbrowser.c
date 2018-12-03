/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：MMS浏览器
版本号：1.0.0
开发时期：2004-06-07
作者：陈建明 Jami chen
规划：
修改记录：
******************************************************/
#include <ewindows.h>
//#include "resource.h"
#include <ecomctrl.h>
#include <thirdpart.h>
#include <mmsmanage.h>
#include <mmsbrowser.h>
#include <imgbrowser.h>
#include "esmilbrowser.h"
#include "eCommdlg.h"
#include "..\include\mms_content.h"
#include "..\mmsbody\mime_body.h"

/***************  全局区 定义， 声明 *****************/

//static char   classMMSBrowser[20] = TEXT("MMSBROWSER");

#define ID_IMGVIEW	300
#define ID_SMILVIEW	301

#define MMSTYPE_UNKNOWFILE	0
#define MMSTYPE_TEXTFILE	100
#define MMSTYPE_BMPFILE		101
#define MMSTYPE_GIFFILE		102
#define MMSTYPE_JPGFILE		103
#define MMSTYPE_MP3FILE		104
#define MMSTYPE_3GPFILE		105
#define MMSTYPE_AMRFILE		106
#define MMSTYPE_MIDFILE		107
#define MMSTYPE_WAVFILE		108
#define MMSTYPE_SMILFILE	109

#define ID_POPUPMENU			100//设置时钟的标号
#define DELAY_POPUPMENU			1000 // 下笔后要延迟的时间弹出菜单 1 s

#define IDM_PLAY		300
#define IDM_SAVE		301
#define IDM_PREVIEW		302
#define IDM_SMILPLAY	303

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

const static FILETYPE tabFileType[] = {
	{ "*.txt" , MMSTYPE_TEXTFILE },
	{ "*.bmp" , MMSTYPE_BMPFILE },
	{ "*.jpg" , MMSTYPE_JPGFILE },
	{ "*.gif" , MMSTYPE_GIFFILE },
	{ "*.mp3" , MMSTYPE_MP3FILE },
	{ "*.3gp" , MMSTYPE_3GPFILE },
	{ "*.amr" , MMSTYPE_AMRFILE },
	{ "*.mid" , MMSTYPE_MIDFILE },
	{ "*.wav" , MMSTYPE_WAVFILE },
	{ "*.smil", MMSTYPE_SMILFILE },
};

// ＰＡＲＴ显示参数
typedef struct structMMSShowPart{
	int iShowPos;  // 显示位置
	UINT iFileType; // 当前的文件类型
	LPMIME_PART lpMIME_Part; // 当前要显示的PART指针
	LPTSTR lpString; // 当前PART文本
}MMSSHOWPART, *LPMMSSHOWPART;


typedef struct MMSBrowserStruct{
	MIME_BODY MIME_Body;
	LPBODYDETAIL lpBodyDetail;
	LPMMSSHOWPART lpMMSShowPart;
	int iPartNum ;
	int iCurPart;
	BOOL bImgShow;
	BOOL bAudioPlay;
}MMSBROWSER, * LPMMSBROWSER;


#define POS_TEXTSTART	40
#define ICON_PARTWIDTH	40


/******************************************************/
// 函数声明区
/******************************************************/
ATOM RegisterMMSBrowserClass( HINSTANCE hInstance );
static LRESULT CALLBACK MMSBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnMMSBrowserPaint( HWND hWnd );
static LRESULT DoMMSBrowserCreate(HWND hWnd);
static void InsertToolBarItem(HWND hWnd,HWND hToolBar);
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetMMSData(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void OrganizeMMSPart(HWND hWnd);
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam);

static UINT GetPartType(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);

static void DrawMMSPart(HWND hWnd,HDC hdc);
static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart);

static UINT GetPartHeight(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static LPTSTR GetPartText(HWND hWnd ,LPMIME_PART lpMIME_Part);

static void PreViewImgPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static int ShowIconContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart);
//static LPMMSSHOWPART GetPart(HWND hWnd,POINT Point);
static int GetPart(HWND hWnd,POINT Point);

static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL PlayCurrentPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static BOOL SavePartData(HWND hWnd,LPMMSSHOWPART lpMMSShowPart,LPTSTR lpFileName,BOOL bNameFile);
static void ShowImage(HWND hWnd,LPTSTR lpFileName,UINT uImgType);

static HMENU CreateMMSMenu(UINT iFileType);
static LRESULT DoClick(HWND hWnd);


static void PlaySmilPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static void PlaySmil(HWND hWnd,LPTSTR lpFileName);
static void SaveAsPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName);


extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;
//extern BYTE SetWell_Know_Type(LPTSTR lpContent_Type);
//extern ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
extern ATOM RegisterSMILBrowersClass( HINSTANCE hInst );


// ********************************************************************
// 声明：ATOM RegisterMMSBrowserClass( HINSTANCE hInstance );
// 参数：
//	IN hInstance - 桌面系统的实例句柄
// 返回值：
// 	无
// 功能描述：注册系统桌面类
// 引用: 
// ********************************************************************
ATOM RegisterMMSBrowserClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MMSBrowserWndProc;  // 桌面过程函数
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classMMSBrowser;
	return RegisterClass(&wcex); // 向系统注册桌面类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK MMSBrowserWndProc(HWND , UINT , WPARAM , LPARAM )
// 参数：
//	IN hWnd- 桌面系统的窗口句柄
//    IN message - 过程消息
//    IN wParam  - 消息参数
//    IN lParam  - 消息参数
// 返回值：
//	无
// 功能描述：系统桌面窗口过程函数
// 引用: 
// ********************************************************************
static LRESULT CALLBACK MMSBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // 绘画消息
			OnMMSBrowserPaint( hWnd );
			return 0;
//		case WM_NOTIFY: // 通知消息
//			return DoNotify(hWnd,wParam,lParam);
		case WM_CREATE: // 创建消息
			return DoMMSBrowserCreate(hWnd);
		case WM_DESTROY: // 破坏消息
			DoReleaseControl(hWnd,wParam,lParam);
			break;
		case WM_COMMAND:
			DoCommand(hWnd,wParam,lParam);
			break;
	   	case WM_LBUTTONDOWN: // 鼠标左键按下
			return DoLButtonDown(hWnd,wParam,lParam);
   		case WM_LBUTTONUP: // 鼠标左键弹起
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_TIMER:
			return DoTimer(hWnd,wParam,lParam);
		case WM_NOTIFY://通知消息
			{
				NMHDR   *hdr = (NMHDR   *)lParam;
				switch(hdr->code)
				{
					case NMIMG_CLICK://点击通知
					case NMSMIL_CLICK://点击通知
						DoClick(hWnd);
						break;
				}
			}
			break;
		case WMB_SETMMSDATA:
			DoSetMMSData(hWnd,wParam,lParam);
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // 窗口默认消息
   }
   return FALSE;
}

// ********************************************************************
// 声明：static void OnMMSBrowserPaint( HWND hWnd )
// 参数：
//	IN hWnd- 桌面系统的窗口句柄
// 返回值：
//	无
// 功能描述：处理系统桌面窗口绘画过程
// 引用: 
// ********************************************************************
static void OnMMSBrowserPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
//	RECT rt;
//	LPMMSBROWSER lpMMSBrowser;

//		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
//		ASSERT(lpMMSBrowser);

//		GetClientRect(hWnd,&rt);

		hdc = BeginPaint( hWnd, &ps );  // 开始绘制
//		DrawText(hdc, "abcd", 4, &rt, DT_LEFT | DT_VCENTER);
		DrawMMSPart(hWnd,hdc);
		
		EndPaint( hWnd, &ps ); // 结束绘制
}


// ********************************************************************
// 声明：static LRESULT DoMMSBrowserCreate(HWND hWnd)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
// 返回值：
//		返回创建的结果
// 功能描述：处理桌面创建消息
// 引用: 
// ********************************************************************
static LRESULT DoMMSBrowserCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	LPMMSBROWSER lpMMSBrowser;
	RECT rt;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);

		lpMMSBrowser = (LPMMSBROWSER)malloc(sizeof(MMSBROWSER)); // 分配MMS浏览器结构指针
		if (lpMMSBrowser == NULL)
		{  // 分配内存失败,返回失败
			return -1;
		}

		lpMMSBrowser->MIME_Body.lpData = NULL;
		lpMMSBrowser->MIME_Body.iDataSize = 0; 
		lpMMSBrowser->lpBodyDetail = NULL;
		lpMMSBrowser->lpMMSShowPart = NULL;
		lpMMSBrowser->iCurPart = -1; // 没有当前的PART
		lpMMSBrowser->iPartNum  = 0 ; // 目前没有PART
		lpMMSBrowser->bImgShow = FALSE;
		lpMMSBrowser->bAudioPlay = FALSE;

		SetWindowLong(hWnd,0,(LONG)lpMMSBrowser);

//		RegisterIMGBrowserClass(hInstance); // 注册图形显示窗口
		RegisterSMILBrowersClass(hInstance);  // 注册SMIL显示窗口

		GetClientRect(hWnd,&rt);

		CreateWindow(classIMGBrowser,"",WS_CHILD,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_IMGVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

		CreateWindow(classSMILBROWSER,"",WS_CHILD,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_SMILVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

		return 0;
}


// **************************************************
// 声明：static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：无
// 功能描述：处理命令消息。
// 引用: 
// **************************************************
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;
	HWND hIdWnd;

		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		hIdWnd = (HWND)lParam;

		RETAILMSG(1,(TEXT("Receive Command [ID = %d], [Event = %d]\r\n"),wmId,wmEvent));
		RETAILMSG(1,(TEXT("Receive Command [Window = %x]\r\n"),hIdWnd));
		switch (wmId)
		{
			default:
			   return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
		}

		return 0;
}

// **************************************************
// 声明：static LRESULT DoSetMMSData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- (HMMSDATA)数据指针
// 
// 返回值：无
// 功能描述：处理命令消息。
// 引用: 
// 说明: 如果用户传入的数据为空,则表示要清除原来的数据
// **************************************************
static LRESULT DoSetMMSData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMS_CONTENT lpNewData;
	LPMMSBROWSER lpMMSBrowser;
	DATA_DEAL MIMEData;
	BOOL bOver;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->bImgShow == TRUE)
		{
			// 当前正在显示图形,现在显示结束
			ShowImage(hWnd,NULL,0);
			PlaySmil(hWnd,NULL);
		}
		// 首先释放原来的数据
		if (lpMMSBrowser->MIME_Body.lpData)
			free(lpMMSBrowser->MIME_Body.lpData);
		lpMMSBrowser->MIME_Body.lpData = NULL;
		lpMMSBrowser->MIME_Body.iDataSize = 0; 

		if (lpMMSBrowser->lpMMSShowPart)
		{
			int i;

			for ( i = 0; i < lpMMSBrowser->lpBodyDetail->iPartNum; i++)
			{
				if (lpMMSBrowser->lpMMSShowPart[i].lpString)
					free(lpMMSBrowser->lpMMSShowPart[i].lpString);  // 释放已经分配的文本串
			}
			free(lpMMSBrowser->lpMMSShowPart);
			lpMMSBrowser->lpMMSShowPart = NULL;
		}
		if (lpMMSBrowser->lpBodyDetail)
		{
			MIME_Release(lpMMSBrowser->lpBodyDetail);
			lpMMSBrowser->lpBodyDetail = NULL;
			lpMMSBrowser->iPartNum  = 0 ; // 目前没有PART
		}

		lpNewData = (LPMMS_CONTENT)lParam; // 得到新的MIME数据
		if (lpNewData == NULL)
		{  // 没有数据
			return TRUE;
		}

		lpMMSBrowser->MIME_Body.lpData = malloc(lpNewData->iDataSize); //分配空间来存放MIME数据
		if (lpMMSBrowser->MIME_Body.lpData == NULL)
		{  // 分配失败
			return FALSE;
		}

		// 复制数据
		memcpy(lpMMSBrowser->MIME_Body.lpData , lpNewData->lpData,lpNewData->iDataSize);
		lpMMSBrowser->MIME_Body.iDataSize = lpNewData->iDataSize; 

		MIMEData.lpData = lpMMSBrowser->MIME_Body.lpData;
		MIMEData.dwLenTotal = lpMMSBrowser->MIME_Body.iDataSize;
		MIMEData.dwLenDealed = 0;

		lpMMSBrowser->lpBodyDetail = MIME_UnPack(&MIMEData,&bOver);
		lpMMSBrowser->iPartNum  = lpMMSBrowser->lpBodyDetail->iPartNum ; // 得到目前PART数目

		OrganizeMMSPart(hWnd);
		return TRUE;
}


// **************************************************
// 声明：static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 
// 返回值：无
// 功能描述：释放控件内存。
// 引用: 
// **************************************************
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		// 释放数据
		if (lpMMSBrowser->MIME_Body.lpData)
			free(lpMMSBrowser->MIME_Body.lpData);

		lpMMSBrowser->MIME_Body.lpData = NULL;
		lpMMSBrowser->MIME_Body.iDataSize = 0; 

		// 释放显示部分
		if (lpMMSBrowser->lpMMSShowPart)
		{
			int i;

			for ( i = 0; i < lpMMSBrowser->lpBodyDetail->iPartNum; i++)
			{
				if (lpMMSBrowser->lpMMSShowPart[i].lpString)
					free(lpMMSBrowser->lpMMSShowPart[i].lpString);  // 释放已经分配的文本串
			}
			free(lpMMSBrowser->lpMMSShowPart);
			lpMMSBrowser->lpMMSShowPart = NULL;
		}

		// 释放细节
		if (lpMMSBrowser->lpBodyDetail)
		{
			MIME_Release(lpMMSBrowser->lpBodyDetail);
			lpMMSBrowser->lpBodyDetail = NULL;
			lpMMSBrowser->iPartNum  = 0 ; // 目前没有PART
		}


		free(lpMMSBrowser);		// 释放浏览结构
		return TRUE;
}


// **************************************************
// 声明：static void DrawMMSPart(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述：绘制MMS的内容。
// 引用: 
// 说明：在MMS浏览类中，在上部显示多媒体文件图表，下面显示文本文件内容
// **************************************************
static void DrawMMSPart(HWND hWnd,HDC hdc)
{
	LPMMSBROWSER lpMMSBrowser;
//	LPMIME_PART lpMIME_Part;
	LPMMSSHOWPART lpMMSShowPart;
	UINT uPartNum;
	UINT	i;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

//		GetClientRect(hWnd,&rt);

		if (lpMMSBrowser->lpBodyDetail == NULL)
		{  // 目前还没有数据
			return ;
		}
	
//		lpMIME_Part = lpMMSBrowser->lpBodyDetail->lpMIME_Part;  // 得到第一个PART
		uPartNum = lpMMSBrowser->lpBodyDetail->iPartNum;
//		while (lpMIME_Part)
		for (i = 0; i < uPartNum; i++)
		{
			lpMMSShowPart = &lpMMSBrowser->lpMMSShowPart[i]; // 得到要显示的PART指针
//			iFileType = GetPartType(lpMIME_Part);
			switch(lpMMSShowPart->iFileType)
			{
				case MMSTYPE_UNKNOWFILE: // 以下内容的文件多将先显示图标
				case MMSTYPE_BMPFILE:
				case MMSTYPE_GIFFILE:
				case MMSTYPE_JPGFILE:
				case MMSTYPE_MP3FILE:
				case MMSTYPE_3GPFILE:
				case MMSTYPE_AMRFILE:
				case MMSTYPE_MIDFILE:
				case MMSTYPE_WAVFILE:
				case MMSTYPE_SMILFILE:
					ShowIconContent(hWnd,hdc,lpMMSShowPart); //在指定位置显示PART图标
					break;
				case MMSTYPE_TEXTFILE:
					ShowTextContent(hWnd,hdc,lpMMSShowPart); // 在指定位置显示PART的文本
					break;
			}

//			lpMIME_Part = lpMIME_Part->next; // 到下一个PART
		}

}


// **************************************************
// 声明：static UINT GetPartType(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// 参数：
//	IN hWnd  -- 窗口句柄
// 	IN lpMMSShowPart -- 要得到类型的part
// 
// 返回值：无
// 功能描述：得到文件类型。
// 引用: 
// **************************************************
static UINT GetPartType(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	int iFileTypeNum;
	int i;
	BYTE bPartType;
	LPMMSBROWSER lpMMSBrowser;
	LPMIME_PART lpMIME_Part;
	LPTSTR lpData;

		if (lpMMSShowPart == NULL)
		{
			// 没有指定的PART
			return MMSTYPE_UNKNOWFILE;
		}
		lpMIME_Part = lpMMSShowPart->lpMIME_Part;  
		if (lpMIME_Part == NULL)
		{
			// 没有指定的PART
			return MMSTYPE_UNKNOWFILE;
		}
		if (lpMIME_Part->lpContent_Location == NULL)
			goto UNKNOWTYPE;
		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE); // 得到当前的已知类型数目
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpMIME_Part->lpContent_Location, strlen(lpMIME_Part->lpContent_Location)) == TRUE)
			{ // 比较当前文件是否是已知文件
				return tabFileType[i].iFileType;  // 使用已知文件的图标
			}
		}
UNKNOWTYPE:
		if (lpMIME_Part->lpContent_Type)
		{
//			bPartType = SetWell_Know_Type(lpMIME_Part->lpContent_Type);
			CNTTYPE_ASSIGN stCntTypeAssign;
	
			if (strlen(lpMIME_Part->lpContent_Type) >= LEN_WSP_CNTTYPE_NAME)
			{
				// 肯定不是已知的彩信内容类型
				bPartType = 0XFF;
			}
			else
			{
				strcpy(stCntTypeAssign.szName,lpMIME_Part->lpContent_Type);
				//  查找当前彩信类型是否为已知的类型
				if (VarFind_CntTypeAssign( CNTTYPEFIND_NUM_BYNAME, &stCntTypeAssign ) == TRUE)
				{
					// 已经查到，是已知的彩信内容类型
					bPartType = (BYTE)stCntTypeAssign.dwNum;
				}
				else
				{
					// 不是已知的彩信内容类型
					bPartType = 0XFF;
				}
			}

			switch(bPartType)
			{
				case 0x01:  // text/*
				case 0x03:  // text/plain
					return MMSTYPE_TEXTFILE;
				case 0x1D:  // "image/gif"
					return MMSTYPE_GIFFILE;
				case 0x1E:  // "image/jpeg"
					return MMSTYPE_JPGFILE;
			}
			if (stricmp(lpMIME_Part->lpContent_Type,"application/smil") == 0)
			{
					return MMSTYPE_SMILFILE;
			}
		}
		
		// 到这里,仍是位置文件类型

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		lpData = lpMMSBrowser->MIME_Body.lpData + lpMMSShowPart->lpMIME_Part->uPos; // 得到数据开始位置
		if (strncmp(lpData,"GIF",3) == 0)
		{
			// 是GIF文件
			return MMSTYPE_GIFFILE;
		}

		return MMSTYPE_UNKNOWFILE;
}

// **************************************************
// 声明：static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
//  IN lpMMSShowPart -- 要显示的PART
// 
// 返回值：成功返回新的位置否则返回-1
// 功能描述：在指定位置显示PART的文本。
// 引用: 
// **************************************************
static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
{
//	int x,y;
	RECT rect;
//	LPTSTR lpString;
	LPMMSBROWSER lpMMSBrowser;

		if (lpMMSShowPart->lpString == NULL)
			return 0; // 没有文本需要显示

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		// 得到文本显示的开始位置
		GetClientRect(hWnd,&rect);
		rect.left = 0;
		rect.top = POS_TEXTSTART + lpMMSShowPart->iShowPos; 
		
		DrawText(hdc,lpMMSShowPart->lpString,strlen(lpMMSShowPart->lpString),&rect,DT_LEFT);

		return 0;
}


// **************************************************
// 声明：static void DrawMMSPart(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述：绘制MMS的内容。
// 引用: 
// 说明：在MMS浏览类中，在上部显示多媒体文件图表，下面显示文本文件内容
// **************************************************
static void OrganizeMMSPart(HWND hWnd)
{
	LPMMSBROWSER lpMMSBrowser;
	LPMIME_PART lpMIME_Part;
	UINT uIndex = 0;
	UINT iTextPos = 0, iIconPos = 0;
	int iFileType;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->lpBodyDetail == NULL)
		{  // 目前还没有数据
			return ;
		}
		lpMMSBrowser->lpMMSShowPart = (LPMMSSHOWPART)malloc(lpMMSBrowser->lpBodyDetail->iPartNum * sizeof(MMSSHOWPART)); // 分配一个存储空间
		if (lpMMSBrowser->lpMMSShowPart == NULL)
		{  // 分配内存失败
			return ;
		}
	
		lpMIME_Part = lpMMSBrowser->lpBodyDetail->lpMIME_Part;  // 得到第一个PART

		while (lpMIME_Part)
		{
			lpMMSBrowser->lpMMSShowPart[uIndex].lpMIME_Part = lpMIME_Part; // 设定当前的PART指针
			lpMMSBrowser->lpMMSShowPart[uIndex].lpString = NULL; // 初始化当前的文本串
			iFileType = GetPartType(hWnd,&lpMMSBrowser->lpMMSShowPart[uIndex]); // 得到当前PART的文件类型
			lpMMSBrowser->lpMMSShowPart[uIndex].iFileType = iFileType; // 得到文件类型
			switch(iFileType)
			{
				case MMSTYPE_UNKNOWFILE:	// 不认识的文件类型
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_BMPFILE:  // 图形文件
				case MMSTYPE_GIFFILE:
				case MMSTYPE_JPGFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_3GPFILE: // 声音文件
				case MMSTYPE_MP3FILE:  
				case MMSTYPE_AMRFILE:
				case MMSTYPE_MIDFILE:
				case MMSTYPE_WAVFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_SMILFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_TEXTFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].lpString = GetPartText(hWnd,lpMIME_Part); // 初始化当前的文本串
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iTextPos;
					iTextPos += GetPartHeight(hWnd,&lpMMSBrowser->lpMMSShowPart[uIndex]); // 得到当前PART的文本高度
					break;
			}
			lpMIME_Part = lpMIME_Part->next; // 到下一个PART
			uIndex ++;
			if (uIndex >= (UINT)lpMMSBrowser->lpBodyDetail->iPartNum)
				break; // 已经组织完成
		}
}


// **************************************************
// 声明：static UINT GetPartHeight(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpMMSShowPart -- 要得到高度的PART
// 
// 返回值：成功返回当前PART的高度,否则返回0
// 功能描述：得到当前PART的高度。
// 引用: 
// **************************************************
static UINT GetPartHeight(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	RECT rect;
	int iPartHeigth;
	HDC hdc;

	if (lpMMSShowPart->lpString == NULL)
		return 0; // 没有文本串
	GetClientRect(hWnd,&rect) ; // 得到控件的客户区大小

	hdc = GetDC(hWnd); // 得到设备句柄
	iPartHeigth = DrawText(hdc,lpMMSShowPart->lpString,strlen(lpMMSShowPart->lpString),&rect,DT_CALCRECT);
	ReleaseDC(hWnd,hdc); // 释放设备句柄
	return (UINT)rect.bottom; // 返回高度
}


// **************************************************
// 声明：static LPTSTR GetPartText(HWND hWnd ,LPMIME_PART lpMIME_Part)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpMIME_Part -- 要得到文本的PART
// 
// 返回值：成功返回当前PART的文本串指针,否则返回0
// 功能描述：得到当前PART的文本。
// 引用: 
// **************************************************
static LPTSTR GetPartText(HWND hWnd ,LPMIME_PART lpMIME_Part)
{
	LPTSTR lpString;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		lpString = (LPTSTR)malloc(lpMIME_Part->uLen + 1); // 分配一个内存来存放文本
		if (lpString == NULL)
		{  // 分配内存失败
			return NULL;
		}
		memset(lpString,0,lpMIME_Part->uLen + 1);
		memcpy(lpString,lpMMSBrowser->MIME_Body.lpData + lpMIME_Part->uPos,lpMIME_Part->uLen); //得到文本

		return lpString;
}

// **************************************************
// 声明：static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
//  IN lpMMSShowPart -- 要显示的PART
// 
// 返回值：成功返回新的位置否则返回-1
// 功能描述：在指定位置显示PART的文本。
// 引用: 
// **************************************************
static int ShowIconContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
{
//	RECT rect;
	LPMMSBROWSER lpMMSBrowser;
	HICON hIcon;
	int x,y;


		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		switch(lpMMSShowPart->iFileType)
		{
			case MMSTYPE_UNKNOWFILE:	// 不认识的文件类型
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SYSLOGO ), IMAGE_ICON,
					32, 32, 0 ) ;  // 装载指定的图标
				break;

			case MMSTYPE_BMPFILE:  // 图形文件
			case MMSTYPE_GIFFILE:
			case MMSTYPE_JPGFILE:
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_CALENDAR ), IMAGE_ICON,
					32, 32, 0 ) ;  // 装载指定的图标
				break;

			case MMSTYPE_3GPFILE: // 声音文件
			case MMSTYPE_MP3FILE:  
			case MMSTYPE_AMRFILE:
			case MMSTYPE_MIDFILE:
			case MMSTYPE_WAVFILE:
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_MP3 ), IMAGE_ICON,
					32, 32, 0 ) ;  // 装载指定的图标
				break;
			case MMSTYPE_SMILFILE:				
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APPLICATION ), IMAGE_ICON,
					32, 32, 0 ) ;  // 装载指定的图标
				break;
			default:
				return 0;
		}

		x = lpMMSShowPart->iShowPos * ICON_PARTWIDTH;
		y = 4;
		
		// 得到文本显示的开始位置
		DrawIconEx(hdc,
				(int)x,
				(int)y,
				hIcon,
				0,
				0,
				0,
				0,
				DI_NORMAL);

		DestroyIcon(hIcon);  //  破坏图标

		return 0;
}



// **************************************************
// 声明：static int GetPart(HWND hWnd,POINT Point)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN Point -- 当前的点的位置
// 
// 返回值：成功返回所在位置对应PART的的索引,否则返回-1
// 功能描述：得到所在位置对应PART。
// 引用: 
// **************************************************
static int GetPart(HWND hWnd,POINT Point)
{
//	LPMMSSHOWPART lpMMSShowPart;
	RECT rect;
	int i,iPartNum;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->lpBodyDetail == NULL)
			return -1;

		iPartNum = lpMMSBrowser->lpBodyDetail->iPartNum;

		for (i = 0; i < iPartNum ; i++)
		{
			if (lpMMSBrowser->lpMMSShowPart[i].iFileType != MMSTYPE_TEXTFILE)
			{
				// 是 ICON 类型的文件
				rect.left = lpMMSBrowser->lpMMSShowPart[i].iShowPos * ICON_PARTWIDTH + 4;
				rect.top  = 4 ;
				rect.right = rect.left + 32;
				rect.bottom  = rect.top + 32 ;
				if (PtInRect(&rect,Point) != 0)
					return i;
			}
		}

		return -1;
}

/**************************************************
声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(X,Y),鼠标坐标
返回值：无
功能描述：处理WM_LBUTTONDOWN消息。
引用: 
************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
	LPMMSBROWSER lpMMSBrowser;
//	HWND hImgBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		point.x=LOWORD(lParam);
		point.y=HIWORD(lParam);
		

		lpMMSBrowser->iCurPart = GetPart(hWnd,point); // 得到当前点所在的位置
		if (lpMMSBrowser->iCurPart != -1)
		{  // 是有效的PART
			SetCapture(hWnd);
			SetTimer(hWnd,ID_POPUPMENU,DELAY_POPUPMENU,NULL);
		}

		return 0;
}


/**************************************************
声明：static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(X,Y),鼠标坐标
返回值：无
功能描述：处理WM_LBUTTONUP消息。
引用: 
************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
	LPMMSBROWSER lpMMSBrowser;
//	LPMMSSHOWPART lpMMSShowPart;
	int iIndex;

		if (GetCapture() != hWnd)
		{
			//当前窗口没有抓住鼠标,不做任何处理
			return 0;
		}
		KillTimer(hWnd,ID_POPUPMENU);
		ReleaseCapture(); // 释放鼠标

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		point.x=LOWORD(lParam);  // 得到当前点
		point.y=HIWORD(lParam);

		iIndex = GetPart(hWnd,point); // 得到当前点所在的位置
		if (iIndex == -1)
		{
			lpMMSBrowser->iCurPart = -1;
			return 0; // 点到无效的PART
		}
		if (iIndex == lpMMSBrowser->iCurPart)
		{
			// 当前点到有效PART
			PlayCurrentPart(hWnd,&lpMMSBrowser->lpMMSShowPart[iIndex]); // 显示或播放当前的PART
		}
		lpMMSBrowser->iCurPart = -1;

		return 0;
}


// **************************************************
// 声明：static BOOL PlayCurrentPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN lpMMSShowPart -- 要显示的PART
// 
// 返回值：成功返回TRUE,否则返回FALSE
// 功能描述：显示或播放当前的PART
// 引用: 
// **************************************************
static BOOL PlayCurrentPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
//	TCHAR lpFileName[MAX_PATH];	
//	UINT uImgType;

		switch(lpMMSShowPart->iFileType)
		{
				case MMSTYPE_BMPFILE:  // 图形文件
				case MMSTYPE_GIFFILE:
				case MMSTYPE_JPGFILE:
					PreViewImgPart(hWnd,lpMMSShowPart);
					break;
				case MMSTYPE_SMILFILE:
					PlaySmilPart(hWnd,lpMMSShowPart);
					break;
				case MMSTYPE_3GPFILE: // 声音文件
					break;
				case MMSTYPE_AMRFILE:
					break;
				case MMSTYPE_MIDFILE:
					break;
				case MMSTYPE_WAVFILE:
					break;
				case MMSTYPE_MP3FILE:  
					break;
		}
		return TRUE;
}


// **************************************************
// 声明：static BOOL SavePartData(HWND hWnd,LPMMSSHOWPART lpMMSShowPart,LPTSTR lpFileName,BOOL bNameFile)
// 参数：
//	IN hWnd -- 窗口句柄
//	IN lpMMSShowPart -- 要保存的PART指针
//  IN lpFileName -- 要将内容保存的文件名
//  IN bNameFile -- 是否为命名文件,如果是TRUE,则用户已经指定了文件名,否则自动指定文件名,并返回文件名
// 
// 返回值：成功TRUE，否则返回FALSE
// 功能描述：保存当前PART的文件内容到文件。
// 引用: 
// **************************************************
static BOOL SavePartData(HWND hWnd,LPMMSSHOWPART lpMMSShowPart,LPTSTR lpFileName,BOOL bNameFile)
{
	
	HANDLE hFile;
	DWORD dwWriteLen;
	LPTSTR lpData;
	BOOL bFileNameOK = FALSE;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		if (bNameFile == FALSE)
		{
			// 没有指定文件名,需要指定文件名
			strcpy(lpFileName,"\\Mobile\\temp\\"); // 得到文件目录,存放到临时文件夹

			if (lpMMSShowPart->lpMIME_Part->lpContent_Location)
			{  //  PART已经包含文件名
				if (strlen(lpMMSShowPart->lpMIME_Part->lpContent_Location))
				{
					strcat(lpFileName,lpMMSShowPart->lpMIME_Part->lpContent_Location); // 得到文件名
					bFileNameOK = TRUE;
				}
			}
			if (bFileNameOK == FALSE)
			{  //  还没有得到文件名
					strcat(lpFileName,"default"); // 得到文件名
					bFileNameOK = TRUE;
			}
		}

		hFile = CreateFile( lpFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
		if( hFile == INVALID_HANDLE_VALUE )
		{
			return FALSE;
		}
		lpData = lpMMSBrowser->MIME_Body.lpData + lpMMSShowPart->lpMIME_Part->uPos;
		WriteFile(hFile,lpData,lpMMSShowPart->lpMIME_Part->uLen,&dwWriteLen,NULL);
		CloseHandle(hFile);

		return TRUE;
}


// **************************************************
// 声明：static void ShowImage(HWND hWnd,LPTSTR lpFileName,UINT uImgType)
// 参数：
//	IN hWnd -- 窗口句柄
//  IN lpFileName -- 要显示的图象的文件名
//  IN uImgType -- 图象类型
// 
// 返回值：无
// 功能描述：显示一个文件。
// 引用: 
// **************************************************
static void ShowImage(HWND hWnd,LPTSTR lpFileName,UINT uImgType)
{
	HWND hImgBrowser;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		hImgBrowser = GetDlgItem(hWnd,ID_IMGVIEW);
//		SendMessage(hImgBrowser,IMB_SETIMGFILE,(WPARAM)uImgType,(LPARAM)lpFileName);

		if (lpFileName)
		{  //显示一个图象文件
			SendMessage(hImgBrowser,IMB_SETIMGFILE,(WPARAM)uImgType,(LPARAM)lpFileName);
			ShowWindow(hImgBrowser,SW_SHOW);
			UpdateWindow(hImgBrowser);
			lpMMSBrowser->bImgShow = TRUE;
			//SetCapture(hWnd);
		}
		else
		{
			// 没有图象文件要显示
			ShowWindow(hImgBrowser,SW_HIDE);
			SendMessage(hImgBrowser,IMB_SETIMGFILE,(WPARAM)uImgType,(LPARAM)lpFileName);
			lpMMSBrowser->bImgShow = FALSE;
			//ReleaseCapture();
		}
}

/**************************************************
声明：static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 消息参数
返回值：无
功能描述：处理WM_LBUTTONUP消息。
引用: 
************************************************/
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMSBROWSER lpMMSBrowser;
	LPMMSSHOWPART lpMMSShowPart;
	HMENU hMenu;
	POINT point;
	UINT idCommand;


		KillTimer(hWnd,ID_POPUPMENU);  // 杀死时钟
		ReleaseCapture(); // 释放鼠标

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->iCurPart >= lpMMSBrowser->iPartNum || lpMMSBrowser->iCurPart == -1)
		{
			// 无效的索引
			return 0;
		}
		lpMMSShowPart = &lpMMSBrowser->lpMMSShowPart[lpMMSBrowser->iCurPart]; // 显示或播放当前的PART
		
		hMenu = CreateMMSMenu(lpMMSShowPart->iFileType); // 创建一个可以处理当前文件类型的菜单
		
		if (hMenu == NULL)
		{  //  没有成功创建菜单
			return 0;
		}

		point.x = (lpMMSShowPart->iShowPos + 1) * ICON_PARTWIDTH ;// 得到当前要弹出菜单的位置
		point.y =  4 ;// 得到当前要弹出菜单的位置
		ClientToScreen(hWnd,&point); // 将坐标转化为屏幕坐标
		idCommand = TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, 0, hWnd, NULL ); // 弹出菜单
		DestroyMenu( hMenu );
		
		switch(idCommand)
		{
			case IDM_PREVIEW:  // 预览图象文件
				PreViewImgPart(hWnd,lpMMSShowPart);
				break;
			case IDM_PLAY:  // 播放声音文件
				break;
			case IDM_SAVE:
				SaveAsPart(hWnd,lpMMSShowPart);
				break; // 将当前PART的内容保存到文件
			case IDM_SMILPLAY:  // 播放SMIL文件
				PlaySmilPart(hWnd,lpMMSShowPart);
				break; 
		}

		return 0;
}


/**************************************************
声明：static HMENU CreateMMSMenu(UINT iFileType)
参数：
	IN iFileType -- 当前的文件类型
返回值：无
功能描述：创建一个处理PART的菜单消息。
引用: 
************************************************/
static HMENU CreateMMSMenu(UINT iFileType)
{
	HMENU hMenu;
	MENUITEMINFO	InfoItem;

		if (MMSTYPE_TEXTFILE == iFileType)
			return NULL; // 是文本文件，不需要菜单

		hMenu = CreatePopupMenu();

		memset( &InfoItem, 0, sizeof(MENUITEMINFO) );
		InfoItem.cbSize = sizeof(MENUITEMINFO);
		InfoItem.fMask = MIIM_TYPE | MIIM_ID;
		InfoItem.fType = MFT_STRING;

		switch(iFileType)
		{
			case MMSTYPE_UNKNOWFILE: 
				break; // 没有其他操作
			case MMSTYPE_BMPFILE:
			case MMSTYPE_GIFFILE:
			case MMSTYPE_JPGFILE:
				// 是图象文件，可以显示
				InfoItem.dwTypeData = ("预览");
				InfoItem.cch = 4;
				InfoItem.wID = IDM_PREVIEW;
				InsertMenuItem( hMenu, 0, TRUE, &InfoItem );
				break;
			case MMSTYPE_MP3FILE:
			case MMSTYPE_3GPFILE:
			case MMSTYPE_AMRFILE:
			case MMSTYPE_MIDFILE:
			case MMSTYPE_WAVFILE:
				// 是声音文件，可以播放
				InfoItem.dwTypeData = ("播放");
				InfoItem.cch = 4;
				InfoItem.wID = IDM_PLAY;
				InsertMenuItem( hMenu, 0, TRUE, &InfoItem );
				break;
			case MMSTYPE_SMILFILE:
				// 是声音文件，可以播放
				InfoItem.dwTypeData = ("播放");
				InfoItem.cch = 4;
				InfoItem.wID = IDM_SMILPLAY;
				InsertMenuItem( hMenu, 0, TRUE, &InfoItem );
				break;
		}
		////////

		InfoItem.dwTypeData = ("保存");
		InfoItem.cch = 4;
		InfoItem.wID = IDM_SAVE;
		InsertMenuItem( hMenu, 1, TRUE, &InfoItem );

		return hMenu;
}

/**************************************************
声明：static void PreViewImgPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
参数：
	IN iFileType -- 当前的文件类型
返回值：无
功能描述：显示一个图象PART。
引用: 
************************************************/
static void PreViewImgPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	TCHAR lpFileName[MAX_PATH];	
	UINT uImgType;

		switch(lpMMSShowPart->iFileType)
		{
			case MMSTYPE_BMPFILE:  // 图形文件
				uImgType = IMG_BMPFILE;
				break;
			case MMSTYPE_GIFFILE:
				uImgType = IMG_GIFFILE;
				break;
			case MMSTYPE_JPGFILE:
				uImgType = IMG_JPGFILE;
				break;
			default:
				return ;
		}

		SavePartData(hWnd,lpMMSShowPart,lpFileName,FALSE);
		ShowImage(hWnd,lpFileName,uImgType);
}

// **************************************************
// 声明：static void PlaySmilPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// 参数：
//	IN hWnd -- 窗口句柄
//	IN lpMMSShowPart -- 要保存的PART指针
// 
// 返回值：无
// 功能描述：播放一个SMIL片段
// 引用: 
// **************************************************
static void PlaySmilPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	TCHAR lpFileName[MAX_PATH];	
	LPMMSBROWSER lpMMSBrowser;
	LPMMSSHOWPART lpCurPart;
	int i;

		KillTimer(hWnd,ID_POPUPMENU);  // 杀死时钟
		ReleaseCapture(); // 释放鼠标

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		// smil 需要其他所有的PART的数据
		// 首先保存其他PART的内容到文件
		for (i = 0 ; i < lpMMSBrowser->iPartNum; i++)
		{
			lpCurPart = &lpMMSBrowser->lpMMSShowPart[i];
			if (lpCurPart != lpMMSShowPart)
			{
				// 是当前的SMIL PART，在后面保存
				SavePartData(hWnd,lpCurPart,lpFileName,FALSE);
			}
		}
		
		// 先保存当前的SMIL PART
		SavePartData(hWnd,lpMMSShowPart,lpFileName,FALSE);
		PlaySmil(hWnd,lpFileName);
}

// **************************************************
// 声明：static void PlaySmil(HWND hWnd,LPTSTR lpFileName)
// 参数：
//	IN hWnd -- 窗口句柄
//  IN lpFileName -- 要播放的的SMIL的文件名
// 
// 返回值：无
// 功能描述：播放一个SMIL文件。
// 引用: 
// **************************************************
static void PlaySmil(HWND hWnd,LPTSTR lpFileName)
{
	HWND hSMILBrowser;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		hSMILBrowser = GetDlgItem(hWnd,ID_SMILVIEW);
		SendMessage(hSMILBrowser,SMM_LOADSMIL,0,(LPARAM)lpFileName);

		if (lpFileName)
		{  //显示一个图象文件
			ShowWindow(hSMILBrowser,SW_SHOW);
			UpdateWindow(hSMILBrowser);
			lpMMSBrowser->bImgShow = TRUE;
			//SetCapture(hWnd);
		}
		else
		{
			// 没有图象文件要显示
			ShowWindow(hSMILBrowser,SW_HIDE);
			lpMMSBrowser->bImgShow = FALSE;
			//ReleaseCapture();
		}
}


// **************************************************
// 声明：static void SaveAsPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// 参数：
//	IN hWnd -- 窗口句柄
//	IN lpMMSShowPart -- 要保存的PART指针
// 
// 返回值：无
// 功能描述：将当前的PART保存到一个指定的文件。
// 引用: 
// **************************************************
static void SaveAsPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	TCHAR lpFileName[MAX_PATH];	

		memset(lpFileName,0,MAX_PATH);
		if (GetFileName(hWnd,lpFileName) == TRUE)
		{  // 得到要保存的文件名
			SavePartData(hWnd,lpMMSShowPart,lpFileName,TRUE);
		}
}

// **************************************************
// 声明：static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName)
// 参数：
// 	IN hDlg -- 窗口句柄
// 	OUT lpFileName -- 返回得到的文件名
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到文件名。
// 引用: 
// **************************************************
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName)
{
    TCHAR OpenTitle[256];
	TCHAR szFilter[MAX_PATH]; 	
	OPENFILENAME OpenFileName;
	TCHAR lpstrFilter[30];
	TCHAR strFile[MAX_PATH];
	TCHAR TitleText[10];


		strcpy(OpenTitle,"保存文件"); // 设置标题
		strcpy(szFilter,"*.abc"); // 设置过滤器

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
		sprintf(lpstrFilter+strlen(szFilter)+1, TEXT("%s"), TEXT("*.*"));   
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // 设置标记 -- 支持长文件名，覆盖提示

		memset(strFile,0,sizeof(strFile));
		sprintf(TitleText, TEXT("%s"), OpenTitle);

		OpenFileName.lStructSize=sizeof(OPENFILENAME); 
		OpenFileName.hwndOwner=hDlg; // 设置窗口句柄
		OpenFileName.lpstrFilter=lpstrFilter; //NULL
		OpenFileName.nFilterIndex=1; 
		OpenFileName.lpstrFile=lpFileName; 
		OpenFileName.nMaxFile=MAX_PATH; 
		OpenFileName.lpstrFileTitle=strFile;  //only filename an extension(withou t path information) 
		OpenFileName.nMaxFileTitle=MAX_PATH; 
		OpenFileName.lpstrInitialDir="\\mobile"; 
		OpenFileName.lpstrTitle=TitleText;   
		OpenFileName.nFileOffset=0; 
		OpenFileName.nFileExtension=0; 
		OpenFileName.lpstrDefExt=NULL; 
		if( GetSaveFileName(&OpenFileName) == FALSE)  // 得到保存文件名
			return FALSE; // 得到文件名失败
		return TRUE;
}

// **************************************************
// 声明：static LRESULT DoClick(HWND hWnd)
// 参数：
//	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：图象视图点击消息。
// 引用: 
// **************************************************
static LRESULT DoClick(HWND hWnd)
{
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // 得到MMS浏览类结构指针
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->bImgShow == TRUE)
		{
			// 当前正在显示图形,现在显示结束
			ShowImage(hWnd,NULL,0);
			PlaySmil(hWnd,NULL);
			return 0;
		}
		return 0;
}

