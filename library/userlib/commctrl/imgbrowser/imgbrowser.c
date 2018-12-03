/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：图象浏览器
版本号：1.0.0
开发时期：2004-06-07
作者：陈建明 Jami chen
规划：
修改记录：
******************************************************/
#include <ewindows.h>
#include <jpegapi.h>
#include <gif2bmp.h>
#include <imgbrowser.h>
#include <ecomctrl.h>

/***************  全局区 定义， 声明 *****************/

//char   classIMGBrowser[20] = TEXT("IMGBROWSER");

#define USETHREADSHOWGIF

#define TIMEINTERVAL	50

typedef struct IMGBrowserStruct{
	HDC hMemoryDC;  // 为了防止画面闪烁，使用内存DC,且只用于PAINT
	HBITMAP hMemoryBmp;  // 用于内存DC的位图

	UINT uFileType; // 当前显示的图象类型
	HGIF hGif ; // Gif 图象的句柄
	HBITMAP hBitmap ; // bitmap , jpeg 图象的句柄

	UINT iIndex; // gif图象使用，当前正在播放的图象索引
	int iDelayTime; // gif 图象使用，播放下一个索引图象还需要等待的时间
	UINT iImageNum ; //gif 图象使用，当前一共有多少幅图象

	POINT point ; // 显示图象的位置
	SIZE  Size ;  // 当前图象的大小

	COLORREF cl_Text;   // 文本颜色
	COLORREF cl_BkColor;  // 背景颜色

#ifdef USETHREADSHOWGIF
	HANDLE hDrawGifThread;
	BOOL bExit;
#endif
}IMGBROWSER, * LPIMGBROWSER;


#define ID_GIFSHOW	100

/******************************************************/
// 函数声明区
/******************************************************/
ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
static LRESULT CALLBACK IMGBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnIMGBrowserPaint( HWND hWnd );
static LRESULT DoIMGBrowserCreate(HWND hWnd);
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetIMGFile(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);

static UINT GetImgFileType(LPTSTR lpFileName);
static void SendActiveNotify(HWND hWnd,UINT code,POINT pt);


static int GetTotalPic(HGIF hGif);
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize);
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg);
//static void ShowCurImage(HWND hWnd);
static void ShowCurImage(HWND hWnd,HDC hdc);

static int GetNextPicTime(HGIF hGif,int iIndex);

static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize);
static void DrawBitMap(HWND hWnd,HDC hdc,HBITMAP hBitmap,int x,int y);

static void ShowCurText(HWND hWnd,HDC hdc);
//static HDC CreateMemoryDC(HWND hWnd);
static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap);


static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

#ifdef USETHREADSHOWGIF
static DWORD WINAPI ShowImageThread(VOID * pParam);
#endif
// ********************************************************************
// 声明：ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
// 参数：
//	IN hInstance - 桌面系统的实例句柄
// 返回值：
// 	无
// 功能描述：注册系统桌面类
// 引用: 
// ********************************************************************
ATOM RegisterIMGBrowserClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)IMGBrowserWndProc;  // 桌面过程函数
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classIMGBrowser;
	return RegisterClass(&wcex); // 向系统注册桌面类
}

// ********************************************************************
// 声明：static LRESULT CALLBACK IMGBrowserWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK IMGBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // 绘画消息
			OnIMGBrowserPaint( hWnd );
			return 0;
		case WM_CREATE: // 创建消息
			return DoIMGBrowserCreate(hWnd);
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
		case WM_SETTEXT:
			DefWindowProc( hWnd, message, wParam, lParam );
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;

		case WM_SETCTLCOLOR	: // 设置控制颜色
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // 得到控制颜色
			return DoGetColor(hWnd,wParam,lParam);

		case IMB_SETIMGFILE:  //设置要显示的图形文件
			return DoSetIMGFile(hWnd,wParam,lParam);
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // 窗口默认消息
   }
   return FALSE;
}

// ********************************************************************
// 声明：static void OnIMGBrowserPaint( HWND hWnd )
// 参数：
//	IN hWnd- 桌面系统的窗口句柄
// 返回值：
//	无
// 功能描述：处理系统桌面窗口绘画过程
// 引用: 
// ********************************************************************
static void OnIMGBrowserPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	LPIMGBROWSER lpIMGBrowser;
	RECT rect;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

		GetClientRect(hWnd,&rect);
		hdc = BeginPaint( hWnd, &ps );  // 开始绘制

//		FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // 画背景

//		if (lpIMGBrowser->hBitmap) // 存在bitmap
//			DrawBitMap(hWnd,lpIMGBrowser->hMemoryDC,lpIMGBrowser->hBitmap,lpIMGBrowser->point.x,lpIMGBrowser->point.y);  // 绘制位图文件
//		if (lpIMGBrowser->hGif) // 存在gif图象
//			ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // 绘制GIF文件

//		ShowCurText(hWnd,lpIMGBrowser->hMemoryDC);

		// 将内存DC中的画面刷新到屏幕
		BitBlt(hdc,0,0,rect.right,rect.bottom,lpIMGBrowser->hMemoryDC,0,0,SRCCOPY);

		ShowCurText(hWnd,hdc); //将要显示的文本显示到屏幕

		EndPaint( hWnd, &ps ); // 结束绘制
}


// ********************************************************************
// 声明：static LRESULT DoIMGBrowserCreate(HWND hWnd)
// 参数：
//	IN	hWnd -- 桌面系统的窗口句柄
// 返回值：
//		返回创建的结果
// 功能描述：处理桌面创建消息
// 引用: 
// ********************************************************************
static LRESULT DoIMGBrowserCreate(HWND hWnd)
{
	LPIMGBROWSER lpIMGBrowser;


		lpIMGBrowser = (LPIMGBROWSER)malloc(sizeof(IMGBROWSER)); // 分配IMG浏览器结构指针
		if (lpIMGBrowser == NULL)
		{  // 分配内存失败,返回失败
			return -1;
		}
		
		lpIMGBrowser->hMemoryBmp = NULL;
		lpIMGBrowser->hMemoryDC = CreateMemoryDC(hWnd,&lpIMGBrowser->hMemoryBmp); // 创建内存DC
		if (lpIMGBrowser->hMemoryDC == NULL)
		{
			// 创建内存DC失败
			return -1;
		}

		lpIMGBrowser->uFileType = 0; // 当前显示的图象类型
		lpIMGBrowser->hGif = NULL ; // Gif 图象的句柄
		lpIMGBrowser->hBitmap = NULL ; // bitmap , jpeg 图象的句柄

		lpIMGBrowser->iIndex = 0 ; // gif图象使用，当前正在播放的图象索引
		lpIMGBrowser->iDelayTime = 0 ; // gif 图象使用，播放下一个索引图象还需要等待的时间
		lpIMGBrowser->iImageNum = 0 ; //gif 图象使用，当前一共有多少幅图象

		lpIMGBrowser->point.x = 0 ; // 显示图象的位置
		lpIMGBrowser->point.y = 0 ; // 显示图象的位置

		lpIMGBrowser->cl_Text = GetSysColor(COLOR_WINDOWTEXT);
		lpIMGBrowser->cl_BkColor = GetSysColor(COLOR_WINDOW);
		SetWindowLong(hWnd,0,(LONG)lpIMGBrowser);

#ifdef USETHREADSHOWGIF
		lpIMGBrowser->hDrawGifThread = NULL;
		lpIMGBrowser->bExit = FALSE;
		lpIMGBrowser->hDrawGifThread = CreateThread(NULL, 0, ShowImageThread, (void *)hWnd, 0, 0 );  // 创建下载管理线程
#endif
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

		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		switch (wmId)
		{
			default:
			   return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
		}

		return 0;
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
	LPIMGBROWSER lpIMGBrowser;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

#ifdef USETHREADSHOWGIF
		if (lpIMGBrowser->hDrawGifThread)
		{ // 创建了一个绘画线程
			int iWaitTime = 2000;
			lpIMGBrowser->bExit = TRUE; // 要求线程退出
			Sleep(TIMEINTERVAL);
			// 等待线程退出，2秒钟线程还没有退出，程序强制退出
			while(1)
			{
				if (lpIMGBrowser->bExit == FALSE)
					break; // 线程已经退出
				if (iWaitTime <= 0)
					break; // 时间到
				iWaitTime -= TIMEINTERVAL;
				Sleep(TIMEINTERVAL);
			}
			CloseHandle(lpIMGBrowser->hDrawGifThread);
		}
#else
		KillTimer(hWnd,ID_GIFSHOW); //停止定时器
#endif
		// 释放数据
		// 首先关闭原来的显示
		if (lpIMGBrowser->hGif)
		{  //打开了GIF句柄
			Gif_Destroy(lpIMGBrowser->hGif);
			lpIMGBrowser->hGif = NULL;
		}
		if (lpIMGBrowser->hBitmap)
		{  //打开了BMP句柄
			DeleteObject(lpIMGBrowser->hBitmap);
			lpIMGBrowser->hBitmap = NULL;
		}
		lpIMGBrowser->uFileType = 0; //当前没有打开任何的图象

		if (lpIMGBrowser->hMemoryDC)
			DeleteDC(lpIMGBrowser->hMemoryDC);

		if (lpIMGBrowser->hMemoryBmp)
			DeleteObject(lpIMGBrowser->hMemoryBmp);

		free(lpIMGBrowser);		// 释放浏览结构
		return TRUE;
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

		point.x=LOWORD(lParam);
		point.y=HIWORD(lParam);
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
	LPIMGBROWSER lpIMGBrowser;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

		point.x=LOWORD(lParam);  // 得到当前点
		point.y=HIWORD(lParam);

		SendActiveNotify(hWnd,NMIMG_CLICK,point);
		return 0;
}




/**************************************************
声明：static LRESULT DoSetIMGFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- UINT 图形类型
	IN lParam -- LPTSTR 要显示的文件名
返回值：无
功能描述：设置一个图形文件来显示,如果为空,则清空原来的显示。
引用: 
************************************************/
static LRESULT DoSetIMGFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT uFileType;
	LPTSTR lpFileName;
	LPIMGBROWSER lpIMGBrowser;
	HINSTANCE hInstance;
	RECT rect;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);

		GetClientRect(hWnd,&rect); // 得到窗口的大小
		// Add By Jami chen in 2004.07.20
		FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // 清空背景
		// Add By Jami chen in 2004.07.20
		// 首先关闭原来的显示
		if (lpIMGBrowser->hGif)
		{  //打开了GIF句柄
			Gif_Destroy(lpIMGBrowser->hGif);
			lpIMGBrowser->hGif = NULL;
		}
		if (lpIMGBrowser->hBitmap)
		{  //打开了BMP句柄
			DeleteObject(lpIMGBrowser->hBitmap);
			lpIMGBrowser->hBitmap = NULL;
		}
		lpIMGBrowser->uFileType = 0; //当前没有打开任何的图象

		uFileType = (UINT)wParam;
		lpFileName = (LPTSTR)lParam;

		if (lpFileName == NULL)
		{
			//不需要打开任何图象
			return 0;
		}
		if (uFileType == IMG_UNKNOW)
		{
			uFileType = GetImgFileType(lpFileName);
		}
		
		switch(uFileType)
		{
			case IMG_BMPFILE:
				lpIMGBrowser->hBitmap = LoadImage( hInstance, lpFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载要显示的位图
				GetBMPImageSize(lpIMGBrowser->hBitmap,&lpIMGBrowser->Size);
				lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // 显示图象的位置
				lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // 显示图象的位置
				// 将位图显示到内存DC
//				FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // 画背景

				if (lpIMGBrowser->hBitmap) // 存在bitmap
					DrawBitMap(hWnd,lpIMGBrowser->hMemoryDC,lpIMGBrowser->hBitmap,lpIMGBrowser->point.x,lpIMGBrowser->point.y);  // 绘制位图文件
//				InvalidateRect(hWnd,NULL,TRUE); // 刷新画面
				break;
			case IMG_GIFFILE:
				lpIMGBrowser->hGif = Gif_LoadFromFile( lpFileName ) ; // 装载GIF图象
				if (lpIMGBrowser->hGif != NULL)
				{
					// 成功装载
					lpIMGBrowser->iIndex = 0; // gif图象使用，当前正在播放的图象索引
					lpIMGBrowser->iDelayTime = 0; // gif 图象使用，播放下一个索引图象还需要等待的时间
					lpIMGBrowser->iImageNum = GetTotalPic(lpIMGBrowser->hGif); // 得到当前GIF的图象总数
//					SetTimer(hWnd,ID_GIFSHOW,1,NULL);
					GetGIFImageSize(lpIMGBrowser->hGif,&lpIMGBrowser->Size);
					lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // 显示图象的位置
					lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // 显示图象的位置
//					if (lpIMGBrowser->iImageNum > 1)
//						DoTimer(hWnd,0,0);  // 只有在GIF的图象大于一幅时才需要显示动画
//					ShowCurImage(hWnd, hdc); // 绘制GIF文件
//					FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // 画背景
					ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // 绘制GIF文件
//					ShowCurText(hWnd, hdc);
//					InvalidateRect(hWnd,NULL,TRUE); // 刷新画面
				}
				break;
			case IMG_JPGFILE:
				lpIMGBrowser->hBitmap = JPEG_LoadByName(lpFileName,NULL);  // 装载JPEG图象
				if (lpIMGBrowser->hBitmap)
				{ // 装载位图成功
					GetBMPImageSize(lpIMGBrowser->hBitmap,&lpIMGBrowser->Size);
					//RETAILMSG(1,(TEXT("Bitmap Size [%d,%d]\r\n"),lpIMGBrowser->Size.cx,lpIMGBrowser->Size.cy));
					lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // 显示图象的位置
					lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // 显示图象的位置
					// 将位图显示到内存DC
//					FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(WHITE_BRUSH));  // 画背景

					if (lpIMGBrowser->hBitmap) // 存在bitmap
						DrawBitMap(hWnd,lpIMGBrowser->hMemoryDC,lpIMGBrowser->hBitmap,lpIMGBrowser->point.x,lpIMGBrowser->point.y);  // 绘制位图文件
				}
				break;
		}
		InvalidateRect(hWnd,NULL,TRUE); // 刷新画面
		return TRUE;
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
	HDC hdc;
	LPIMGBROWSER lpIMGBrowser;
	RECT rect;
	
		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

		GetClientRect(hWnd,&rect);

		KillTimer(hWnd,ID_GIFSHOW); //停止定时器
		hdc = GetDC(hWnd);  // 得到设备句柄
//		ShowCurImage(hWnd,hdc); // 绘制GIF文件
		ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // 绘制GIF文件

		// 将内存DC中的画面刷新到屏幕
		BitBlt(hdc,0,0,rect.right,rect.bottom,lpIMGBrowser->hMemoryDC,0,0,SRCCOPY);
		ShowCurText(hWnd,hdc);
		ReleaseDC(hWnd,hdc); //释放设备句柄、
		
	return 0;
}
// **************************************************
// 声明：static void ShowCurImage(HWND hWnd, HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN hdc  -- 设备句柄
// 返回值：无
// 功能描述：显示当前索引的GIF图象。
// 引用: 
// ************************************************
static void ShowCurImage(HWND hWnd, HDC hdc)
{
	LPIMGBROWSER lpIMGBrowser;
	
//	POINT ptOrg;

		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

		if (lpIMGBrowser->hGif == NULL)
			return ;
		//需要显示
//		GetControlPos(lpCurItem->hControl,&ptControl);
//		ptOrg.x = ptControl.x - lpIMGBrowser->point.x;
//		ptOrg.y = ptControl.y - lpIMGBrowser->point.y;
		DrawGifImage(hWnd,hdc,lpIMGBrowser->hGif,lpIMGBrowser->iIndex,&lpIMGBrowser->point);


		if (lpIMGBrowser->iImageNum > 1)
		{	//  只有超过一幅图象是才需要设定下一幅图象的显示时间
			lpIMGBrowser->iDelayTime = GetNextPicTime(lpIMGBrowser->hGif,lpIMGBrowser->iIndex); // 得到到下一幅图象的时间
			lpIMGBrowser->iIndex ++; // 下一幅图象
			if (lpIMGBrowser->iIndex >= lpIMGBrowser->iImageNum)
				lpIMGBrowser->iIndex = 0; // 已经到最后一幅图象, 下次开始显示第一幅图象

#ifndef USETHREADSHOWGIF
			SetTimer(hWnd,ID_GIFSHOW,lpIMGBrowser->iDelayTime,NULL);  // 设定下一幅图象的显示时间
#endif
		}

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
// 声明：static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN hGif -- 要显示的图象句柄
//  IN iIndex -- 要显示的图象索引
//  IN ptOrg -- 图象开始显示位置
// 
// 返回值：无
// 功能描述：根据一个图形句柄显示图象。
// 引用: 
// **************************************************
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
{
//	GIF_IMAGE gifimage;

//		gifimage.dwSize = sizeof(GIF_IMAGE);
//		Gif_GetImage(hGif,iIndex,&gifimage); // 得到当前索引图象的信息
		// 绘制当前指定索引的图象
//		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);

//		gifimage.dwSize = sizeof(GIF_IMAGE);
//		Gif_GetImage(hGif,iIndex,&gifimage); // 得到当前索引图象的信息
		// 绘制当前指定索引的图象
		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);
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

	return (dwDelay * 10 ) - 20; // Delay 是0.01s 为单位，转化成MS
}

// **************************************************
// 声明：static void DrawBitMap(HWND hWnd,HDC hdc,HBITMAP hBitmap,int x,int y)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN x -- 显示位图的x位置
// 	IN y -- 显示位图的y位置
// 
// 返回值：
//		无	
// 功能描述：绘制位图。
// 引用: 
// **************************************************
static void DrawBitMap(HWND hWnd,HDC hdc,HBITMAP hBitmap,int x,int y)
{
	HDC hMemoryDC;
    HBITMAP hOldBitmap;
	SIZE ImgSize;
	DWORD dwStyle;
	RECT rect;

		if (hBitmap == NULL)
			return ; // 没有位图需要显示

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		GetBMPImageSize(hBitmap, &ImgSize);

		hMemoryDC=CreateCompatibleDC(hdc); // 创建内存句柄
//		RETAILMSG(1,("***********************\r\n"));
//		RETAILMSG(1,("hdc = %X \r\n"),hdc);
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将位图选择到内存句柄

		if (dwStyle & IBS_IMGSTRETCH)
		{
			// 需要拉伸
				GetClientRect(hWnd,&rect);
				// 显示位图
				StretchBlt( hdc, // handle to destination device context
					(short)0,  // x-coordinate of destination rectangle's upper-left
											 // corner
					(short)0,  // y-coordinate of destination rectangle's upper-left
											 // corner
					rect.right - rect.left , //	 拉伸后宽度
					rect.bottom - rect.top , //  拉伸后高度
					hMemoryDC,  // handle to source device context
					(short)0,   // x-coordinate of source rectangle's upper-left
											 // corner
					(short)0,   // y-coordinate of source rectangle's upper-left
											 // corner
					(short)ImgSize.cx,  // width of IMAGE rectangle
					(short)ImgSize.cy, // height of IMAGE rectangle
					SRCCOPY
					);
		}
		else
		{
			// 不需要拉伸
				// 显示位图
				BitBlt( hdc, // handle to destination device context
					(short)x,  // x-coordinate of destination rectangle's upper-left
											 // corner
					(short)y,  // y-coordinate of destination rectangle's upper-left
											 // corner
					(short)ImgSize.cx,  // width of destination rectangle
					(short)ImgSize.cy, // height of destination rectangle
					hMemoryDC,  // handle to source device context
					(short)0,   // x-coordinate of source rectangle's upper-left
											 // corner
					(short)0,   // y-coordinate of source rectangle's upper-left
											 // corner

					SRCCOPY
					);
		}

//		RETAILMSG(1,("hMemoryDC = %X , hOldBitmap = %X \r\n",hMemoryDC,hOldBitmap));
		SelectObject(hMemoryDC,hOldBitmap); // 恢复内存设备句柄的位图句柄
		DeleteDC(hMemoryDC); // 删除内存句柄
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
//			RETAILMSG(1,(TEXT("Bitmap Size [%d,%d]\r\n"),bitmap.bmWidth,bitmap.bmHeight));
			return TRUE; //  返回成功
		}
		return FALSE; // 没有得到图象数
}



// **************************************************
// 声明：static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
// 参数：
// 	IN hGif --	GIF文件句柄
//  OUT lpImgSize -- 返回图象大小
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述： 得到当前的GIF的图象大小.
// 引用: 
// **************************************************
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // 设置结构大小
		//得到GIF的相关信息
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
		{
			// 得到信息成功
			lpImgSize->cx = gifInfo.nWidth;
			lpImgSize->cy = gifInfo.nHeight;
			return TRUE; // 返回当前GIF的图象数
		}
		return FALSE; // 没有得到图象数
}




// **************************************************
// 声明：static UINT GetImgFileType(LPTSTR lpFileName)
// 参数：
// 	IN lpFileName -- 要判断图象类型的文件名
// 
// 返回值：无
// 功能描述：得到文件类型。
// 引用: 
// **************************************************

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

const static FILETYPE tabFileType[] = {
	{ "*.bmp" , IMG_BMPFILE },
	{ "*.jpg" , IMG_JPGFILE },
	{ "*.gif" , IMG_GIFFILE },
};

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;

static UINT GetImgFileType(LPTSTR lpFileName)
{

	int iFileTypeNum,i;

		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE); // 得到当前的已知类型数目
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpFileName, strlen(lpFileName)) == TRUE)
			{ // 比较当前文件是否是已知文件
				return tabFileType[i].iFileType;  // 使用已知文件的图标
			}
		}
		return 0;
}


/**************************************************
声明：static void SendActiveNotify(HWND hWnd,UINT code,POINT pt)
参数：
	IN hWnd -- 窗口句柄
	IN code -- 要发送的通知消息代码
	IN pt -- 当前鼠标的位置
返回值：无
功能描述：发送一个通知消息给父窗口
引用: 
************************************************/
static void SendActiveNotify(HWND hWnd,UINT code,POINT pt)
{
	NMIMGBROWSER nmImg;
	HWND hParent;

		// 设置结构参数
		nmImg.hdr.hwndFrom=hWnd;
		nmImg.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmImg.hdr.code=code;
		
		nmImg.ptAction=pt;
		hParent=GetParent(hWnd); // 得到父窗口句柄
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmImg); // 通知父窗口
}


/**************************************************
声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTLCOLORSTRUCT 指向CTLCOLORSTRUCT结构的指针
返回值：无
功能描述：设置控件颜色，处理WM_SETCTLCOLOR消息。
引用: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPIMGBROWSER lpIMGBrowser;

		 lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		 ASSERT(lpIMGBrowser);
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 设置文本颜色
			 lpIMGBrowser->cl_Text = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景色
				lpIMGBrowser->cl_BkColor = lpCtlColor->cl_TextBk;
		 }
		 InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
/**************************************************
声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTLCOLORSTRUCT 指向CTLCOLORSTRUCT结构的指针
返回值：无
功能描述：得到控件颜色，处理WM_GETCTLCOLOR消息。
引用: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPIMGBROWSER lpIMGBrowser;

		 lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		 ASSERT(lpIMGBrowser);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // 得到文本颜色
			 lpCtlColor->cl_Text = lpIMGBrowser->cl_Text ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 得到文本背景色
				lpCtlColor->cl_TextBk = lpIMGBrowser->cl_BkColor ;
		 }
		 return TRUE;
}


// **************************************************
// 声明：static void ShowCurText(HWND hWnd, HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
//  IN hdc  -- 设备句柄
// 返回值：无
// 功能描述：显示当前系统的文本。
// 引用: 
// ************************************************
static void ShowCurText(HWND hWnd, HDC hdc)
{
	LPIMGBROWSER lpIMGBrowser;
	DWORD dwStyle;
	LPTSTR lpText;
	int iTextSize;
	RECT rect;
	DWORD dwDTStyle;

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);

		if (!(dwStyle & IBS_SHOWTEXT))
		{
			// 不需要显示文本
			return ;
		}
		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);

		iTextSize = GetWindowTextLength(hWnd);  // 得到文本的长度

		if (iTextSize <= 0)
		{
			// 没有文本
			return ;
		}
		
		lpText = (LPTSTR)malloc(iTextSize + 1); // 分配窗口文本的缓存

		if (lpText == NULL)
		{
			// 分配缓存失败
			return ;
		}

		GetWindowText(hWnd,lpText,iTextSize +1); // 得到文本

		GetClientRect(hWnd,&rect);  // 得到窗口客户区
		dwDTStyle = 0;

		if (dwStyle & IBS_TEXTLEFT)
		{  // 文本左对齐
			dwDTStyle |= DT_LEFT;
		}
		else if (dwStyle & IBS_TEXTRIGHT)
		{  // 文本右对齐
			dwDTStyle |= DT_RIGHT;
		}
		else
		{  // 文本居中
			dwDTStyle |= DT_CENTER;
		}

		if (dwStyle & IBS_SINGLELINE)
		{
			// 单行显示
			if (dwStyle & IBS_TEXTTOP)
			{  // 文本上对齐
				dwDTStyle |= DT_TOP;
			}
			else if (dwStyle & IBS_TEXTBOTTOM)
			{  // 文本下对齐
				dwDTStyle |= DT_BOTTOM;
			}
			else
			{  // 文本居中
				dwDTStyle |= DT_VCENTER;
			}
		}
		else
		{
			// 多行显示
			if (dwStyle & IBS_TEXTTOP)
			{  // 文本上对齐
				// 不需要做任何事，DrawText 默认从上面开始显示
			}
			else 
			{
				int iWindowHeight,iTextHeight;

				iWindowHeight = rect.bottom - rect.top; // 首先得到窗口高度
				DrawText(hdc,lpText,-1,&rect,dwDTStyle | DT_CALCRECT); // 计算显示文本需要的高度
				iTextHeight = rect.bottom - rect.top; // 得到文本高度
				GetClientRect(hWnd,&rect);  // 得到窗口客户区
				if (dwStyle & IBS_TEXTBOTTOM)
				{  // 文本下对齐
					rect.top = (iWindowHeight - iTextHeight); // 得到显示文本的矩形顶部
				}
				else
				{  // 文本居中
					rect.top = (iWindowHeight - iTextHeight) / 2; // 得到显示文本的矩形顶部
				}

				rect.bottom = rect.top + iTextHeight; // 得到显示文本的矩形底部
			}
		}

		if (dwStyle & IBS_TRANSPARENT)
		{
			// 透明显示
			SetBkMode(hdc,TRANSPARENT);
		}
		
		SetTextColor(hdc,lpIMGBrowser->cl_Text);
		SetBkColor(hdc,lpIMGBrowser->cl_BkColor);
		
		// 显示文本
		DrawText(hdc,lpText,-1,&rect,dwDTStyle); 

}

// ********************************************************************
// 声明：static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
//	OUT lpNewBitmap -- 保存新创建的位图句柄
// 返回值：
//	
// 功能描述：创建一个内存设备句柄
// 引用: 
// ********************************************************************
static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap)
{
	HDC hdc;
	HDC hMemDC;
	HBITMAP hMemBitMap;
	RECT rect;
		
		if (lpNewBitmap == NULL )
			return NULL;
		GetClientRect(hWnd,&rect); // 得到客户矩形
		hdc = GetDC(hWnd); // 得到设备句柄
		hMemDC=CreateCompatibleDC(hdc); // 创建兼容内存设备句柄
		hMemBitMap=CreateCompatibleBitmap(hdc,(rect.right-rect.left),(rect.bottom-rect.top)); // 创建兼容位图
		ReleaseDC(hWnd,hdc); /// 释放设备句柄
		if (hMemBitMap == NULL)
		{ // 创建兼容位图失败
			DeleteDC(hMemDC);
			hMemDC = NULL;
			return NULL;
		}
		*lpNewBitmap = hMemBitMap;  // 内存DC中的内存位图需要我自己删除
		hMemBitMap = SelectObject(hMemDC,hMemBitMap); // 选择位图到内存设备句柄

		FillRect(hMemDC,&rect,GetStockObject(WHITE_BRUSH));  // 画背景
		return hMemDC; // 返回成功创建的内存DC
}

#ifdef USETHREADSHOWGIF

// **************************************************
// 声明：DWORD WINAPI ShowImageThread(VOID * pParam)
// 参数：
// 	IN pParam -- 线程参数
// 
// 返回值：无
// 功能描述：图象显示线程。
// 引用: 
// **************************************************
static DWORD WINAPI ShowImageThread(VOID * pParam)
{
	HWND hWnd;
	LPIMGBROWSER lpIMGBrowser;
	
//		RETAILMSG( 1, ( "ShowImageThread ...\r\n" ) );
		hWnd = (HWND)pParam;  // 得到参数
		if (hWnd == NULL)
			return FALSE;
		lpIMGBrowser = (LPIMGBROWSER)GetWindowLong(hWnd,0); // 得到IMG浏览类结构指针
		ASSERT(lpIMGBrowser);
//		RETAILMSG( 1, ( "lpIMGBrowser = %x \r\n",lpIMGBrowser ) );
		while(1)
		{
			if (lpIMGBrowser->bExit == TRUE) 
				break; // 线程要退出
			if (lpIMGBrowser->iImageNum > 1)
			{
				lpIMGBrowser->iDelayTime -= TIMEINTERVAL; // 减去已经等待的时间
				if (lpIMGBrowser->iDelayTime <= 0)
				{
					ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // 绘制GIF文件
					InvalidateRect(hWnd,NULL,TRUE); // 刷新画面
				}
			}
			Sleep(TIMEINTERVAL);
		}

		lpIMGBrowser->bExit = FALSE; // 退出线程成功
		return 0;	
}
#endif
