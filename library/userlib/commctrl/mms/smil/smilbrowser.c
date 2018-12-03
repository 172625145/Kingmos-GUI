/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：SMIL浏览类
版本号：1.0.0.456
开发时期：2004-04-29
作者：陈建明 JAMI
修改记录：
**************************************************/
#include "esmilbrowser.h"
#include "smilstruct.h"
//#include "DebugWnd.h"
#include "parsesmil.h"
//#include "ASSERT.H"
#include "gif2bmp.h"
#include "efilepth.h"
#include "jpegapi.h"
#include "playamr.h"



// **************************************************
LPTSTR classSMILBROWSER = TEXT("SMILBROWSER");

typedef struct SMILBROWSERSTRUCT{
	LPSMIL lpSmil ; // SMIL 结构指针，保存当前的SMIL内容
	BOOL bExitSmil ; // 是否要退出当前的ＳＭＩＬ显示
	LPTSTR lpSmilFileName; // 当前打开的SMIL文件
	int iCurParIndex; // 当前正在显示的幻灯索引
	HDC hMemoryDC; // 存放当前的屏幕状态
	HBITMAP hMemoryBitmap; // 内存位图,结合内存DC来存放当前的屏幕状态
	BOOL bThreadExist ;  //当前是否存在线程

}SMILBROWSER,*LPSMILBROWSER;

// 定义查询等待间隔

#define TIMEINTERVAL	10 // 以ms为单位

// 定义图象文件的类型
#define IMGTYPE_GIF		1000	// GIF文件
#define IMGTYPE_JPEG	1001	// JPEG文件
#define IMGTYPE_UNKNOW	1002	// 不认识的图形文件

// 定义声音文件的类型
#define AUDIOTYPE_AMR		1000	// AMR文件
#define AUDIOTYPE_MIDI		1001	// MIDI文件
#define AUDIOTYPE_UNKNOW	1002	// 不认识的图形文件

// **************************************************
// **************************************************
// 定义区域
// **************************************************
extern LPTSTR SMILBufferAssign(const LPTSTR pSrc);


LRESULT CALLBACK SMILBrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint);
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT LoadSmilFile(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LPPAR GetIndexPar(LPSMIL lpSmil,int iIndex);
static void ShowAPar(HWND hWnd,LPPAR lpCurPar);
static LPREGION GetRegion(HWND hWnd,LPTSTR lpRegionID);

DWORD WINAPI ShowParThread(LPVOID lpVoid);
static void DrawParImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static void DrawParText(HWND hWnd,LPTEXT lpText);
static LPTSTR GetFullName(HWND hWnd,LPTSTR lpFileName);

static int GetNextPicTime(HGIF hGif,int iIndex);
static BOOL NeedExitThread(HWND hWnd);
static int GetTotalPic(HGIF hGif);
static BOOL GetPicSize(HGIF hGif,LPSIZE lpSize);
static void ClearWindow(HWND hWnd);
static void ShowCurStatus(HWND hWnd,HDC hdc);
static void AdjustWindowSize(HWND hWnd,LPSMIL lpSmil);
static void AdjustRegionSize(HWND hWnd,LPSMIL lpSmil);

static void DrawParImageABC(HWND hWnd,LPIMG lpImg,int iDelayTime);

static UINT GetImageType(LPTSTR lpImg);
static void DrawParGifImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static void DrawParJpegImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static void DrawUnknowImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize);


static UINT GetAudioType(LPTSTR lpAudio);
static void PlayParAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay);
static void PlayParAmrAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay);
static void StopPlayThread(LPSMILBROWSER lpSmilBrowser);

static void DrawSmilBitmap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop);

// **************************************************
// 声明：ATOM RegisterSMILBrowersClass( HINSTANCE hInst )
// 参数：
// 	IN hInstance -- 实例句柄
// 
// 返回值：返回注册类的结果
// 功能描述：注册SMIL浏览类
// 引用: 
// **************************************************
ATOM RegisterSMILBrowersClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = SMILBrowersProc;  // 设置过程句柄
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(LTGRAY_BRUSH);  // 设置背景
    wc.lpszMenuName = 0;
    wc.lpszClassName = classSMILBROWSER;  // 设置类名

    return (BOOL)(RegisterClass( &wc ));  // 向系统注册
}


// **************************************************
// 声明：LRESULT CALLBACK SMILBrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
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
LRESULT CALLBACK SMILBrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_PAINT:  // 绘制
		  hdc = BeginPaint(hWnd, &ps);
		  ShowCurStatus(hWnd,hdc);
		  EndPaint(hWnd, &ps);
		  return 0;
   	  case WM_LBUTTONDOWN:
		  return DoLButtonDown(hWnd,wParam,lParam);
//	  case WM_MOUSEWHEEL:
//		  return DoMouseWheel(hWnd,wParam,lParam);
	  case WM_KEYDOWN:
		  DoKeyDown(hWnd,wParam,lParam);
		  break;
	  case WM_CREATE:
	      DoCreate(hWnd,wParam,lParam);
		  return 0;
	  case WM_DESTROY:
		  return DoDestory(hWnd,wParam,lParam);


	  case SMM_LOADSMIL: // 装载一个SMIL文件
		  return LoadSmilFile(hWnd,wParam,lParam);
	  default:
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
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	HDC hdc;
	RECT rect;
	
		lpSmilBrowser = (LPSMILBROWSER)malloc(sizeof(SMILBROWSER)); // 分配浏览器数据缓存

		if (lpSmilBrowser == NULL)
		{
			return -1 ; // 分配内存失败
		}

		// 初始化lpSmilBrowser结构
		lpSmilBrowser->bExitSmil = FALSE;
		lpSmilBrowser->lpSmil = NULL;
		lpSmilBrowser->lpSmilFileName = NULL; // 当前打开的SMIL文件
		lpSmilBrowser->iCurParIndex = 0; // 当前正在显示的幻灯索引
		lpSmilBrowser->bThreadExist = FALSE;  // 还没有线程

			
		// 得到当前屏幕的初始化状态

		GetClientRect(hWnd,&rect); // 得到窗口矩形

		hdc = GetDC(hWnd); // 得到窗口的设备句柄
		lpSmilBrowser->hMemoryDC = CreateCompatibleDC(hdc); // 创建一个内存DC
		lpSmilBrowser->hMemoryBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom); // 创建当前的内存位图
		lpSmilBrowser->hMemoryBitmap = SelectObject(lpSmilBrowser->hMemoryDC,lpSmilBrowser->hMemoryBitmap); // 将当前的位图信息选择到内存DC，并保存原来的位图

		FillRect(lpSmilBrowser->hMemoryDC,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH)); // 初始化窗口为白色
//		FillRect(lpSmilBrowser->hMemoryDC,&rect,(HBRUSH)GetStockObject(DKGRAY_BRUSH)); // 初始化窗口为白色

		ReleaseDC(hWnd,hdc); // 释放设备句柄

		SetWindowLong(hWnd,0,(LONG)lpSmilBrowser); // 保存lpSmilBrowser指针到窗口

		JPEG_Init(); // 初始化JPEG
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
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
		ASSERT(lpSmilBrowser);

		StopPlayThread(lpSmilBrowser);

		
		lpSmilBrowser->hMemoryBitmap = SelectObject(lpSmilBrowser->hMemoryDC,lpSmilBrowser->hMemoryBitmap); // 将当前的位图信息选择到内存DC，并保存原来的位图

		DeleteObject( lpSmilBrowser->hMemoryBitmap ); // 删除创建的内存位图
		DeleteDC(lpSmilBrowser->hMemoryDC);
		free(lpSmilBrowser);

		JPEG_Deinit(); // 结束JPEG
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
	RECT rect;
		
		GetClientRect(hWnd,&rect);
		DrawText(hdc,"abcdefg",7,&rect,DT_CENTER);
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
//static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam)
//{
//	return TRUE;
//}

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


		nVirtKey = (int) wParam;    // virtual-key code
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
	DWORD x,y;
	x=(short)LOWORD(lParam);  // 得到当前鼠标的点
	y=(short)HIWORD(lParam);
	return 0;
}



// **************************************************
// 声明：static LRESULT LoadSmilFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR 要装载的SMIL的文件名
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：装载的SMIL文件，处理SMM_LOADSMIL消息。
// 引用: 
// **************************************************
static LRESULT LoadSmilFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpSmilFile; // 要打开的SMIL文件名
	DWORD dwFileSize; // 文件长度
	HANDLE hFile ; //文件句柄
	LPTSTR lpContent; // 用于读取文件内容
	DWORD dwRead = 0;// 读到的文件内容的长度
	LPSMIL lpSmil = NULL; // SMIL 结构指针，保存当前的SMIL内容
	HANDLE hThread; // 用于显示幻灯的线程句柄
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据

	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
	ASSERT(lpSmilBrowser);

	if (lpSmilBrowser->bThreadExist)
		StopPlayThread(lpSmilBrowser);  // 当前有一个线程正在播放，先停止以前播放

	if (lParam == NULL)
	{
		// 当前没有要装载的SMIL
		return FALSE;
	}
	RETAILMSG(1,(TEXT("+LoadSmilFile\r\n")));
	lpSmilFile = (LPTSTR)lParam; // 得到要装载的文件名
	RETAILMSG(1,(TEXT("Will Load FileName %s\r\n"),lpSmilFile));

	// 打开指定的文件
	hFile=CreateFile(lpSmilFile,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{ // 打开文件失败
		RETAILMSG(1,(TEXT("Open File Failure\r\n")));
		return FALSE;
	}

	lpSmilBrowser->lpSmilFileName = SMILBufferAssign(lpSmilFile);

	dwFileSize = GetFileSize(hFile,NULL); // 得到文件长度
	
	lpContent = (LPTSTR)malloc((dwFileSize +1) * sizeof(TCHAR)) ; // 分配缓存来读取文件内容
	
	if (lpContent == NULL)
	{
		// 分配内存失败
		CloseHandle(hFile); // 关闭文件
		return FALSE; //返回失败
	}

	ReadFile(hFile,lpContent,dwFileSize,&dwRead,NULL); // 读取文件内容
	if (dwRead != dwFileSize)
	{
		// 读取文件失败
		CloseHandle(hFile); // 关闭文件
		return FALSE; //返回失败
	}
	CloseHandle(hFile); // 已经读取文件内容，关闭文件
	lpSmil = ParseSMIL(lpContent); // 分析文件内容，成功返回一个分析完后的SMIL指针

	if (lpSmil == NULL)
	{
		// 分析内容失败
		return FALSE;
	}

//	AdjustWindowSize(hWnd,lpSmil); // 根据SMIL需要的尺寸调整窗口的大小

	lpSmilBrowser->lpSmil = lpSmil; // 保存SMIL指针到窗口


	hThread = CreateThread(NULL, 0, ShowParThread, (LPVOID)hWnd, 0, NULL); //创建线程
	if(hThread)
	{ // 创建线程成功，线程句柄不再需要，首先关闭
		CloseHandle(hThread);  // 关闭线程句柄
	}

	RETAILMSG(1,(TEXT("-LoadSmilFile\r\n")));
	return TRUE; // 返回成功
}


// ******************************************************************
// 声明：DWORD WINAPI ShowParThread(LPVOID lpVoid)
// 参数：
//	IN  lpVoid - 指向窗口句柄
// 返回值：0
// 功能描述：用线程来显示幻灯，显示完成后退出
// *******************************************************************
DWORD WINAPI ShowParThread(LPVOID lpVoid)
{
	HWND hWnd ;
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	LPPAR lpCurPar;
	int iIndex = 0;


	RETAILMSG(1,(TEXT("***************had Enter Thread***************\r\n")));
	hWnd = (HWND)lpVoid;
	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
	ASSERT(lpSmilBrowser);

	lpSmilBrowser->bThreadExist = TRUE;  // 线程已经成功创建
	lpSmilBrowser->bExitSmil = FALSE;

	lpCurPar = GetIndexPar(lpSmilBrowser->lpSmil,0); // 得到第一个幻灯数据
	lpSmilBrowser->iCurParIndex = 0;

	while(lpCurPar && (!lpSmilBrowser->bExitSmil)) // 当前的幻灯有数据
	{
		RETAILMSG(1,(TEXT("Will Show %d PAR"),lpSmilBrowser->iCurParIndex));
		ShowAPar(hWnd,lpCurPar); // 显示当前幻灯
		lpCurPar = lpCurPar->next; // 得到下一个幻灯数据
		lpSmilBrowser->iCurParIndex ++;
	}
	RETAILMSG(1,(TEXT("***************had Exit Thread***************\r\n")));
	lpSmilBrowser->bThreadExist = FALSE;  // 退出线程
	return 0;
}


// **************************************************
// 声明：static LPPAR GetIndexPar(LPSMIL lpSmil,int iIndex)
// 参数：
// 	IN lpSmil -- SMIL 结构指针
// 
// 返回值：成功返回指定索引的幻灯片的数据，否则返回NULL
// 功能描述： 得到指定索引幻灯数据。
// 引用: 
// **************************************************
static LPPAR GetIndexPar(LPSMIL lpSmil,int iIndex)
{
	LPPAR lpCurPar;
	int iCurIndex = 0;

	if (lpSmil == NULL)
		return NULL; // 没有SMIL数据

	if (lpSmil->lpBody == NULL)
		return NULL; // 没有BODY数据

	lpCurPar = lpSmil->lpBody->lpPar;

	while(lpCurPar)
	{
		if (iCurIndex == iIndex)
			break; // 已经找到指定索引的幻灯片数据
		lpCurPar =  lpCurPar->next ; // 得到下一个幻灯片数据
	}

	return lpCurPar; // 返回指定索引幻灯片数据
}


// **************************************************
// 声明：static void ShowAPar(HWND hWnd,LPPAR lpCurPar)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN LPCurPar -- 当前的幻灯片数据指针
// 
// 返回值：无
// 功能描述： 显示当前幻灯。
// 引用: 
// **************************************************
static void ShowAPar(HWND hWnd,LPPAR lpCurPar)
{
	int iDelayTime = 10000;

	RETAILMSG(1,(TEXT("***************had enter Show Par***************\r\n")));
	if (lpCurPar == NULL)
		return ; // 没有幻灯片数据

	// 清除屏幕
	ClearWindow(hWnd);
	
	// 等待开始时间
	if (lpCurPar->startTime)
	{	// 进入幻灯片后需要等待startTime时间后开始播放幻灯片
		iDelayTime = lpCurPar->startTime;
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // 是否需要退出线程
				break;  // 退出线程
			Sleep(TIMEINTERVAL); // 暂时停顿
			iDelayTime -= TIMEINTERVAL; // 减少总延续时间
		}
	}
		
	iDelayTime = lpCurPar->durTime; // 计算当前幻灯需要播放的延续时间
	
	// 播放声音
	PlayParAudio(hWnd,lpCurPar->lpAudio,TRUE);
	
	// 显示文本
	DrawParText(hWnd,lpCurPar->lpText);

	// 显示图象
	DrawParImage(hWnd,lpCurPar->lpImg, iDelayTime);
//	DrawParImageABC(hWnd,lpCurPar->lpImg, iDelayTime);

	// 关闭声音
	PlayParAudio(hWnd,lpCurPar->lpAudio,FALSE);

	// 等待结束时间
	if (lpCurPar->endTime != -1)
	{										 // 已经完成时间			
		iDelayTime = (lpCurPar->endTime -  (lpCurPar->startTime  + lpCurPar->durTime) );
		if (iDelayTime)
		{	// 等待幻灯片播放结束后需要等待时间后离开幻灯片
			while(iDelayTime > 0)
			{
				if (NeedExitThread(hWnd)) // 是否需要退出线程
					break;  // 退出线程
				Sleep(TIMEINTERVAL); // 暂时停顿
				iDelayTime -= TIMEINTERVAL; // 减少总延续时间
			}
		}
	}

	RETAILMSG(1,(TEXT("***************had exit Show Par***************\r\n")));

}

// **************************************************
// 声明：static void DrawParImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpImg -- 当前要显示的图象
//	IN iDelayTime -- 显示当前位图要延续的时间
// 
// 返回值：无
// 功能描述： 显示图象。
// 引用: 
// **************************************************
static void DrawParImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	UINT iImgType;
	// 图象支持JPEG , WBMP , GIF
	
	if (lpImg == NULL)
	{
		// 延续时间
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // 是否需要退出线程
				break;  // 退出线程
			Sleep(TIMEINTERVAL); // 暂时停顿
			iDelayTime -= TIMEINTERVAL; // 已经完成10ms的停顿
		}
		return ; // 没有图象需要显示
	}
	iImgType = GetImageType(lpImg->lpSrc);
	switch(iImgType)
	{
			case IMGTYPE_GIF:
				 // GIF 图象
				DrawParGifImage(hWnd,lpImg,iDelayTime);
				break;
			case IMGTYPE_JPEG:
				 // JPEG 图象
				DrawParJpegImage(hWnd,lpImg,iDelayTime);
				break;
			default:
				DrawUnknowImage(hWnd,lpImg,iDelayTime);
				break;
	}
}

// **************************************************
// 声明：static void DrawParGifImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpImg -- 当前要显示的图象
//	IN iDelayTime -- 显示当前位图要延续的时间
// 
// 返回值：无
// 功能描述： 显示GIF图象。
// 引用: 
// **************************************************
static void DrawParGifImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	// GIF 图象
	HGIF hGif;
	LPTSTR lpFullImage; // 全路径文件名
	LPREGION lpRegion;
	HDC hdc;
	int iNextPicTime = 0; // 到下一个图象索引的延续时间
	int iIndex = 0; // 当前图象索引
	int iTotalPic; // 得到总的图象个数
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	int iOrgx,iOrgy;
	SIZE Size;
	RECT rect;
	

		if (lpImg == NULL)
			return ; // 没有图象需要显示
	
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
		ASSERT(lpSmilBrowser);

		lpFullImage=GetFullName(hWnd,lpImg->lpSrc); // 得到图象文件的全路径名
		if (lpFullImage==NULL)
			return ; // 没有得到全路径文件名
		hGif = Gif_LoadFromFile( lpFullImage ) ; // 装载图象
		if (hGif==NULL)
		{  // 装载失败
			return;
		}
		iTotalPic = GetTotalPic(hGif); // 得到当前GIF的图象总数
		GetPicSize(hGif,&Size);  // 得到图象尺寸
		GetClientRect(hWnd,&rect); // 得到窗口尺寸
		iOrgx = ((rect.right - rect.left ) - Size.cx ) /2 ; // 使图象显示在屏幕的中央
		iOrgy = 0; //  图象显示在顶部

		hdc = GetDC(hWnd); // 得到设备句柄
		lpRegion = GetRegion(hWnd,lpImg->lpRegion); // 得到指定的REGION
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // 是否需要退出线程
				break;  // 退出线程
			if (iNextPicTime <= 0)
			{ // 到时间绘制下一幅图象
				GIF_IMAGE gifimage;

					gifimage.dwSize = sizeof(GIF_IMAGE);
					Gif_GetImage(hGif,iIndex,&gifimage); // 得到当前索引图象的信息
					// 绘制当前指定索引的图象
					Gif_DrawIndexEx(hGif,
					iIndex,
					lpSmilBrowser->hMemoryDC,
					lpRegion->left + gifimage.left + iOrgx,
					lpRegion->top + gifimage.top + iOrgy,
					lpRegion->width - gifimage.left,  // width of destination rectangle
					lpRegion->height - gifimage.top, // height of destination rectangle
					0,   // x-coordinate of source rectangle's upper-left 
							   // corner
					0,   // y-coordinate of source rectangle's upper-left 
							   // corner
					SRCCOPY  // raster operation code
				    );

					ShowCurStatus(hWnd,hdc); // 显示当前状态到屏幕
				
					iNextPicTime = GetNextPicTime(hGif,iIndex); // 得到到下一幅图象的时间
					iIndex ++; // 下一幅图象
					if (iIndex >= iTotalPic)
						iIndex = 0; // 已经到最后一幅图象
			}
			Sleep(TIMEINTERVAL); // 暂时停顿
			iDelayTime -= TIMEINTERVAL; // 减少总延续时间
			iNextPicTime -= TIMEINTERVAL; // 减少到下一个图象的延续时间
		}
		if (hGif)
			Gif_Destroy( hGif ) ; // 释放GIF句柄
		ReleaseDC(hWnd,hdc); // 释放设备句柄
}


// **************************************************
// 声明：static void DrawParJpegImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpImg -- 当前要显示的图象
//	IN iDelayTime -- 显示当前位图要延续的时间
// 
// 返回值：无
// 功能描述： 显示图象。
// 引用: 
// **************************************************
static void DrawParJpegImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	// JPEG 图象
	LPTSTR lpFullImage; // 全路径文件名
	LPREGION lpRegion;
	HDC hdc;
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	RECT rect;
	HBITMAP hBitmap;
	int iOrgx,iOrgy;
	SIZE Size;
	
		if (lpImg == NULL)
			return ; // 没有图象需要显示

		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
		ASSERT(lpSmilBrowser);

		lpFullImage=GetFullName(hWnd,lpImg->lpSrc); // 得到图象文件的全路径名
		if (lpFullImage==NULL)
			return ; // 没有得到全路径文件名
		hdc = GetDC(hWnd); // 得到设备句柄
		lpRegion = GetRegion(hWnd,lpImg->lpRegion); // 得到指定的REGION
		// 显示JPEG图象
		
		hBitmap = JPEG_LoadByName(lpFullImage,NULL); // 得到解压缩后的JPEG图象

		GetBMPImageSize(hBitmap,&Size);  // 得到图象尺寸
		GetClientRect(hWnd,&rect);
		iOrgx = ((rect.right - rect.left ) - Size.cx ) /2 ; // 使图象显示在屏幕的中央
		iOrgy = 0; //  图象显示在顶部
		
		if (hBitmap)
		{
			rect.left = lpRegion->left + iOrgx;
			rect.top = lpRegion->top + iOrgy;
			rect.right = rect.left + lpRegion->width;
			rect.bottom = rect.top + lpRegion->height;

			DrawSmilBitmap(lpSmilBrowser->hMemoryDC,hBitmap,rect,0,0,SRCCOPY);
			ShowCurStatus(hWnd,hdc); // 显示当前的状态
		}
		
		DeleteObject(hBitmap);
		
		ReleaseDC(hWnd,hdc); // 释放设备句柄

		// 延续时间
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // 是否需要退出线程
				break;  // 退出线程
			Sleep(TIMEINTERVAL); // 暂时停顿
			iDelayTime -= TIMEINTERVAL; // 已经完成10ms的停顿
		}
}

// **************************************************
// 声明：static void DrawUnknowImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpImg -- 当前要显示的图象
//	IN iDelayTime -- 显示当前位图要延续的时间
// 
// 返回值：无
// 功能描述： 显示图象。
// 引用: 
// **************************************************
static void DrawUnknowImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	// JPEG 图象
	LPREGION lpRegion;
	HDC hdc;
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	
		if (lpImg == NULL)
			return ; // 没有图象需要显示

		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
		ASSERT(lpSmilBrowser);

		hdc = GetDC(hWnd); // 得到设备句柄
		lpRegion = GetRegion(hWnd,lpImg->lpRegion); // 得到指定的REGION
		// 显示ALT内容替代文本

		ReleaseDC(hWnd,hdc); // 释放设备句柄

		// 延续时间
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // 是否需要退出线程
				break;  // 退出线程
			Sleep(TIMEINTERVAL); // 暂时停顿
		}
}


// **************************************************
// 声明：static void DrawParText(HWND hWnd,LPTEXT lpText)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpText -- 当前要显示的文本
// 
// 返回值：无
// 功能描述： 显示文本.
// 引用: 
// **************************************************
static void DrawParText(HWND hWnd,LPTEXT lpText)
{
	LPTSTR lpFullImage;
	LPREGION lpRegion;
	HDC hdc;
	RECT rect;
	DWORD dwFileSize; // 文件长度
	HANDLE hFile ; //文件句柄
	LPTSTR lpContent; // 用于读取文件内容
	DWORD dwRead = 0;// 读到的文件内容的长度
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	
		if (lpText == NULL)
			return ; // 
		
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
		ASSERT(lpSmilBrowser);

		lpFullImage=GetFullName(hWnd,lpText->lpSrc); // 得到文本文件的全路径名
		if (lpFullImage==NULL)
			return ; // 没有得到全路径文件名

		// 打开指定的文本文件
		hFile=CreateFile(lpFullImage,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hFile==INVALID_HANDLE_VALUE)
		{ // 打开文件失败
			RETAILMSG(1,(TEXT("Open File Failure\r\n")));
			return ;
		}


		dwFileSize = GetFileSize(hFile,NULL); // 得到文件长度
		
		lpContent = (LPTSTR)malloc((dwFileSize +1) * sizeof(TCHAR)) ; // 分配缓存来读取文件内容

		if (lpContent == NULL)
		{
			// 分配内存失败
			CloseHandle(hFile); // 关闭文件
			return ; //返回失败
		}
		memset(lpContent,0,((dwFileSize +1) * sizeof(TCHAR))); // 清空缓存

		ReadFile(hFile,lpContent,dwFileSize,&dwRead,NULL); // 读取文件内容
		if (dwRead != dwFileSize)
		{
			// 读取文件失败
			CloseHandle(hFile); // 关闭文件
			return ; //返回失败
		}
		CloseHandle(hFile); // 已经读取文件内容，关闭文件

		hdc = GetDC(hWnd); // 得到设备句柄
		lpRegion = GetRegion(hWnd,lpText->lpRegion); // 得到与REGION名相对应的REGION结构
		
		// 得到当前REGION指定的矩形
		rect.left = lpRegion->left;
		rect.top = lpRegion->top;
		rect.right = lpRegion->left + lpRegion->width;
		rect.bottom = lpRegion->top + lpRegion->height;

		DrawText(lpSmilBrowser->hMemoryDC,lpContent,strlen(lpContent),&rect,DT_CENTER); // 在指定范围内显示文本
		ShowCurStatus(hWnd,hdc); // 显示当前的状态

		ReleaseDC(hWnd,hdc); // 释放设备句柄
		free(lpContent); // 释放文本缓存
}


// **************************************************
// 声明：static LPREGION GetRegion(HWND hWnd,LPTSTR lpRegionID)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpRegionID -- 指定的REGION ID
// 
// 返回值：成功返回指定ID的REGION指针,否则返回NULL
// 功能描述： 得到指定ID的REGION指针.
// 引用: 
// **************************************************
static LPREGION GetRegion(HWND hWnd,LPTSTR lpRegionID)
{
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	LPREGION lpRegion;

	
	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
		
	ASSERT(lpSmilBrowser);

	if (lpSmilBrowser->lpSmil == NULL)
		return NULL; // 没有SMIL数据

	if (lpSmilBrowser->lpSmil->lpHead == NULL)
		return NULL; // 没有HEAD数据

	if (lpSmilBrowser->lpSmil->lpHead->lpLayout == NULL)
		return NULL; // 没有LAYOUT数据

	lpRegion = lpSmilBrowser->lpSmil->lpHead->lpLayout->lpRegion; // 得到第一个REGION的结构

	while(lpRegion)
	{
		if (stricmp(lpRegion->id,lpRegionID) == 0)
		{
			// 找到对应的REGION
			break; // 返回当前的REGION
		}
		// 当前REGION不是要找的REGION
		lpRegion = lpRegion->next; // 指向下一个REGION
	}
	return lpRegion;
}


// **************************************************
// 声明：static LPTSTR GetFullName(HWND hWnd,LPTSTR lpFileName)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpImage -- 当前的图象文件
// 
// 返回值：成功图象文件的全路径名
// 功能描述： 得到图象文件的全路径名.
// 引用: 
// **************************************************
static LPTSTR GetFullName(HWND hWnd,LPTSTR lpFileName)
{
	LPTSTR lpFullName;
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
		ASSERT(lpSmilBrowser);

		lpFullName = (LPTSTR)malloc(MAX_PATH); // 分配一个存放全路径文件名的缓存
		if (lpFullName == NULL)
		{ 
			return NULL; // 分配失败，返回空
		}
		_splitpath(lpSmilBrowser->lpSmilFileName,NULL,lpFullName,NULL,NULL); // 得到SMIL文件的路径

		strcat(lpFullName,lpFileName); // 将路径加到当前的文件名中

		return lpFullName; // 返回全路径文件名
}


// **************************************************
// 声明：static int GetNextPicTime(HGIF hGif,int iIndex)
// 参数：
// 	IN hGif --	GIF文件句柄
//  IN iIndex -- 当前的图象的索引
// 
// 返回值：返回当前图象到下一幅图象的间隔时间
// 功能描述： 得到当前图象到下一幅图象的间隔时间.
// 引用: 
// **************************************************
static int GetNextPicTime(HGIF hGif,int iIndex)
{
	DWORD dwDelay;
	Gif_GetDelay(hGif,iIndex, &dwDelay ); // 得到下一幅图的时间延续

	return (dwDelay * 10 ); // Delay 是0.01s 为单位，转化成MS
}

// **************************************************
// 声明：static BOOL NeedExitThread(HWND hWnd)
// 参数：
// 	IN hWnd --	窗口句柄
// 
// 返回值：需要退出线程时返回TRUE，否则返回FALSE
// 功能描述： 得到当前线程是否需要退出.
// 引用: 
// **************************************************
static BOOL NeedExitThread(HWND hWnd)
{
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据

	
	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据

	ASSERT(lpSmilBrowser);

	return lpSmilBrowser->bExitSmil; // 返回当前的退出参数
}

// **************************************************
// 声明：static int GetTotalPic(HGIF hGif)
// 参数：
// 	IN hGif --	GIF文件句柄
// 
// 返回值：返回当前的GIF的图象个数
// 功能描述： 得到当前的GIF的图象个数.
// 引用: 
// **************************************************
static int GetTotalPic(HGIF hGif)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // 设置结构大小
		//得到GIF的相关信息
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
			return gifInfo.nIndexNum; // 返回当前GIF的图象数
		return 0; // 没有得到图象数
}

// **************************************************
// 声明：static BOOL GetPicSize(HGIF hGif,LPSIZE lpSize)
// 参数：
// 	IN hGif --	GIF文件句柄
//  OUT lpSize -- 要返回的图象的尺寸
// 
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述： 得到当前的GIF的图象尺寸.
// 引用: 
// **************************************************
static BOOL GetPicSize(HGIF hGif,LPSIZE lpSize)
{
	GIF_INFO gifInfo;

		if (lpSize == NULL)
			return 0;
		gifInfo.dwSize = sizeof(GIF_INFO); // 设置结构大小
		//得到GIF的相关信息
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
		{
			lpSize->cx = gifInfo.nWidth;
			lpSize->cy = gifInfo.nHeight;
			return TRUE; // 返回成功
		}
		return 0; // 返回失败
}

// **************************************************
// 声明：static void ShowCurStatus(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN hdc -- 设备句柄
// 
// 返回值：无
// 功能描述： 在这里，我们要显示当前正在播放的幻灯当前的状态。
// 引用: 
// **************************************************
static void ShowCurStatus(HWND hWnd,HDC hdc)
{
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	RECT rect;


	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据
	
	ASSERT(lpSmilBrowser);

	GetClientRect(hWnd,&rect); // 得到窗口大小
	// 将内存DC中的内容复制到当前屏幕
	BitBlt( hdc, 0,0,rect.right,rect.bottom,
			lpSmilBrowser->hMemoryDC,0,0,SRCCOPY);
//	DoPAINT(hWnd,hdc,rect);
}


// **************************************************
// 声明：static void ClearWindow(HWND hWnd)
// 参数：
// 	IN hWnd --	窗口句柄
// 
// 返回值：无
// 功能描述： 清除当前的窗口。
// 引用: 
// **************************************************
static void ClearWindow(HWND hWnd)
{
	LPSMILBROWSER lpSmilBrowser; //当前的浏览器数据
	RECT rect;
	HDC hdc;


		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // 得到SMIL浏览器数据

		ASSERT(lpSmilBrowser);

		GetClientRect(hWnd,&rect); // 得到窗口矩形
		FillRect(lpSmilBrowser->hMemoryDC,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH)); // 初始化窗口为白色
		
		hdc = GetDC(hWnd);
		ShowCurStatus(hWnd,hdc); // 将当前状态显示到屏幕
		ReleaseDC(hWnd,hdc);
}



// **************************************************
// 声明：static void AdjustWindowSize(HWND hWnd,LPSMIL lpSmil) 
// 参数：
// 	IN hWnd --	窗口句柄
//	IN lpSmil -- SMIL结构指针
// 
// 返回值：无
// 功能描述：  根据SMIL需要的尺寸调整窗口的大小。
// 引用: 
// **************************************************
static void AdjustWindowSize(HWND hWnd,LPSMIL lpSmil) 
{
	UINT width = SMIL_DEFAILTWIDTH,height = SMIL_DEFAILTHEIGHT; // 默认SMIL大小
	UINT x=0,y=0;

		if (lpSmil)
		{
			if (lpSmil->lpHead)
			{
				if (lpSmil->lpHead->lpLayout)
				{
					if (lpSmil->lpHead->lpLayout->lpRootLayout)
					{ // 得到新的SMIL尺寸
						width = lpSmil->lpHead->lpLayout->lpRootLayout->width;
						height = lpSmil->lpHead->lpLayout->lpRootLayout->height;
					}
				}
			}
		}

		if (width > SMIL_DEFAILTWIDTH)
		{
			width = SMIL_DEFAILTWIDTH;
			x = 0;
		}
		else
		{
			x = (SMIL_DEFAILTWIDTH - width) / 2;
		}
		if (height > SMIL_DEFAILTHEIGHT)
		{
			height = SMIL_DEFAILTHEIGHT;
		}

		SetWindowPos(hWnd,NULL,x,y,width,height,SWP_NOZORDER);

		AdjustRegionSize(hWnd,lpSmil);
}

// **************************************************
// 声明：static void AdjustRegionSize(HWND hWnd,LPSMIL lpSmil) 
// 参数：
// 	IN hWnd --	窗口句柄
//	IN lpSmil -- SMIL结构指针
// 
// 返回值：无
// 功能描述：  根据SMIL需要的尺寸调整region的大小。
// 引用: 
// **************************************************
static void AdjustRegionSize(HWND hWnd,LPSMIL lpSmil) 
{
	LPREGION lpRegion;
	int width;
	RECT rect;
	
	if (lpSmil == NULL)
		return ; // 没有SMIL数据

	if (lpSmil->lpHead == NULL)
		return ; // 没有HEAD数据

	if (lpSmil->lpHead->lpLayout == NULL)
		return ; // 没有LAYOUT数据

	lpRegion = lpSmil->lpHead->lpLayout->lpRegion; // 得到第一个REGION的结构

	GetClientRect(hWnd,&rect);

	width = rect.right - rect.left; // 得到窗口宽度

	while(lpRegion)
	{
		// 调整REGION主要调整水平方向,使其在窗口内显示
		if (lpRegion->left + lpRegion->width > width)
		{
			// REGION的右边框大于窗口的宽度
			if (lpRegion->width > width)
			{
				// REGION的宽度大于窗口的宽度
				lpRegion->left = 0;
				lpRegion->width = width;
			}
			else
			{
				// REGION的宽度小于窗口的宽度,使REGION居中
				lpRegion->left = 0;
				lpRegion->width = width;
			}
		}
		// 当前REGION已经调整完成
		lpRegion = lpRegion->next; // 指向下一个REGION
	}
	return;
}

// **************************************************
// 声明：static UINT GetImageType(LPTSTR lpImg)
// 参数：
//	IN lpImg -- 指定的图象文件
// 
// 返回值：返回指定的图象文件的图象类型
// 功能描述：  指定的图象文件的图象类型。
// 引用: 
// **************************************************
static UINT GetImageType(LPTSTR lpImg)
{
	TCHAR lpExt[MAX_PATH];
		
	_splitpath(lpImg,NULL,NULL,NULL,lpExt); // 得到IMG文件的扩展名

	if (stricmp(lpExt,".GIF") ==0)
	{
		// 是GIF扩展名,是GIF文件
		return IMGTYPE_GIF;
	}
	if ((stricmp(lpExt,".JPG") ==0) || (stricmp(lpExt,".JPEG") ==0))
	{
		// 是JPG 或 JPEG 扩展名,是JPEG文件
		return IMGTYPE_JPEG;
	}
	return IMGTYPE_UNKNOW;
}


// **************************************************
// 声明：static void DrawSmilBitmap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN rect -- 位图的显示范围
//  IN xOffset - X坐标偏移
//  IN yOffset -- Y坐标偏移
//  IN dwRop -- 显示状态
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
static void DrawSmilBitmap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
	HDC hMemoryDC;
	if(hBitMap==0)
		return;
	hMemoryDC=CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc, (short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top),hMemoryDC,(short)xOffset,(short)yOffset,dwRop);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}

// **************************************************
// 声明：static void PlayParAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpAudio -- 当前要播放的声音
//  IN bPlay -- 开始播放声音或者停止播放声音
// 返回值：无
// 功能描述： 播放声音。
// 引用: 
// **************************************************
static void PlayParAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
{
	UINT iAudioType;
	// 声音支持AMR, 有些也可以支持MIDI
	
	if (lpAudio == NULL)
		return ; // 没有声音需要播放
	iAudioType = GetAudioType(lpAudio->lpSrc);
	switch(iAudioType)
	{
			case AUDIOTYPE_AMR:
				 // AMR 文件
				PlayParAmrAudio(hWnd,lpAudio,bPlay); // 开始播放
				break;
	}
}

// **************************************************
// 声明：static UINT GetAudioType(LPTSTR lpAudio)
// 参数：
//	IN lpAudio -- 指定的声音文件
// 
// 返回值：返回指定的声音文件的类型
// 功能描述：  指定的声音文件的类型。
// 引用: 
// **************************************************
static UINT GetAudioType(LPTSTR lpAudio)
{
	TCHAR lpExt[MAX_PATH];
		
	_splitpath(lpAudio,NULL,NULL,NULL,lpExt); // 得到IMG文件的扩展名

	if ((stricmp(lpExt,".AMR") ==0) || (stricmp(lpExt,".3GP") ==0))
	{
		// 是 AMR 或 3GP 扩展名,是AMR文件
		return AUDIOTYPE_AMR;
	}
	if ((stricmp(lpExt,".MID") ==0) || (stricmp(lpExt,".MIDI") ==0))
	{
		// 是 MID 或 MIDI 扩展名,是MIDI文件
		return AUDIOTYPE_MIDI;
	}
	return AUDIOTYPE_UNKNOW;
}

// **************************************************
// 声明：static void PlayParAmrAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
// 参数：
// 	IN hWnd --	窗口句柄
//  IN lpAudio -- 当前要播放的声音文件
//	IN bPlay -- 是否要播放声音
// 
// 返回值：无
// 功能描述： 播放AMR声音。
// 引用: 
// **************************************************
static void PlayParAmrAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
{
	LPTSTR lpFullImage;

	if (lpAudio == NULL)
	{ // 没有声音文件
		return;
	}

	lpFullImage=GetFullName(hWnd,lpAudio->lpSrc); // 得到声音文件的全路径名
	if (lpFullImage==NULL)
		return ; // 没有得到全路径文件名
	if (bPlay)
		PlayAMRFile(hWnd ,lpFullImage); // 播放指定的AMR文件
	else
		PlayAMRFile(hWnd ,NULL); // 停止指定的AMR文件
	
	free(lpFullImage); // 释放文件名
}


// **************************************************
// 声明：static void StopPlayThread(LPSMILBROWSER lpSmilBrowser)
// 参数：
// 	IN lpSmilBrowser --	SMIL浏览器结构指针
// 
// 返回值：无
// 功能描述： 停止播放线程。
// 引用: 
// **************************************************
static void StopPlayThread(LPSMILBROWSER lpSmilBrowser)
{
		lpSmilBrowser->bExitSmil = TRUE;

		// 等待线程完成
		while(1)
		{
			if (lpSmilBrowser->bThreadExist == FALSE)
				break; // 线程完成后设置参数为 FALSE
			Sleep(10);
		}
}

// **************************************************
// 声明：static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
// 参数：
// 	IN hBmp --	BMP文件句柄
//  OUT lpImgSize -- 返回图象大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述： 得到当前的BMP的图象大小.
// 引用: 
// **************************************************
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
{
	BITMAP bitmap;

		//装载成功
		if (GetObject(hBmp,sizeof(BITMAP),&bitmap))
		{
			//得到信息成功
			lpImgSize->cx = bitmap.bmWidth;
			lpImgSize->cy = bitmap.bmHeight;
			return TRUE; //  返回成功
		}
		return FALSE; // 没有得到图象数
}
