//-------


#include <ewindows.h>
#include <gif2bmp.h>


HINSTANCE hInst;
HWND hWnd;

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

	HANDLE hDrawGifThread;
	BOOL bExit;
}IMGBROWSER, * LPIMGBROWSER;

static LPIMGBROWSER lpIMGBrowser = NULL;

#define Demogif_ClassName TEXT("Class_DemogifMain")
#define WM_USERGIF  8654

static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT  DoGifFile(HWND hWnd);
static LRESULT OnGifPaint( HWND hWnd );
static LRESULT DoGifCreate(HWND hWnd);
static DWORD WINAPI ShowImageThread(VOID * pParam);
static LRESULT DoGifReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ShowCurImage(HWND hWnd, HDC hdc);
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize);
static int GetTotalPic(HGIF hGif);
static HDC CreateMemoryDC(HWND hWnd, HBITMAP *lpNewBitmap);
static int GetNextPicTime(HGIF hGif,int iIndex);
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg);

static	ATOM	Main_RegClass(HINSTANCE hInstance)
{
	WNDCLASS	wcex;

	wcex.style			= 0;
	wcex.lpfnWndProc	= (WNDPROC)Main_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground	= NULL;//(HBRUSH)(BLACK_BRUSH);//GetStockObject(WHITE_BRUSH)
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= Demogif_ClassName;
	
	return RegisterClass(&wcex);
}
LRESULT CALLBACK Demogif_WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	MSG msg;
	HWND hWnd;

	hWnd = FindWindow( Demogif_ClassName, NULL ) ;  // 查找窗口是否存在
	if( hWnd != 0 )
	{ // 窗口已经存在
		SetForegroundWindow( hWnd ) ; // 设置窗口到前台
		return FALSE ;
	}

	Main_RegClass(hInstance);

	if (!Main_InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

static BOOL Main_InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int width,heigh;
   
	hInst = hInstance;
   

    width = GetSystemMetrics(SM_CXSCREEN);
	heigh = GetSystemMetrics(SM_CYSCREEN);

	hWnd = CreateWindowEx(WS_EX_CLOSEBOX,Demogif_ClassName, NULL, WS_VISIBLE,0, 0,width,  heigh, NULL, NULL, hInstance, NULL);
	
	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);
	return TRUE;
}

static LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:
			OnGifPaint(hWnd);
			break;
		case WM_CREATE:
			DoGifCreate(hWnd);
			DoGifFile(hWnd);
			break;
/*		case WM_KEYUP:
			{
				if(wParam == VK_BACK)
					DestroyWindow(hWnd);
			}
			break;
*/		case WM_USERGIF:
			DestroyWindow(hWnd);
			break;
		case WM_DESTROY:
			DoGifReleaseControl(hWnd,wParam,lParam);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


static LRESULT  DoGifFile(HWND hWnd)
{

	LPTSTR lpFileName;
	HINSTANCE hInstance;
	RECT rect;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);

		GetClientRect(hWnd,&rect); // 得到窗口的大小
		// Add By Jami chen in 2004.07.20
		FillRect(lpIMGBrowser->hMemoryDC,&rect,GetStockObject(BLACK_BRUSH));  // 清空背景
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

#ifdef LINUX_KERNEL
	lpFileName = "./kingmos/mms.gif";
#else
	lpFileName = "\\kingmos\\mms.gif";
#endif

	if (lpFileName == NULL)
	{
		//不需要打开任何图象
		return 0;
	}
	
	lpIMGBrowser->hGif = Gif_LoadFromFile( lpFileName ) ; // 装载GIF图象
	if (lpIMGBrowser->hGif != NULL)
	{
		// 成功装载
		lpIMGBrowser->iIndex = 0; // gif图象使用，当前正在播放的图象索引
		lpIMGBrowser->iDelayTime = 0; // gif 图象使用，播放下一个索引图象还需要等待的时间
		lpIMGBrowser->iImageNum = GetTotalPic(lpIMGBrowser->hGif); // 得到当前GIF的图象总数
		GetGIFImageSize(lpIMGBrowser->hGif,&lpIMGBrowser->Size);
		lpIMGBrowser->point.x = ((rect.right - rect.left) - lpIMGBrowser->Size.cx ) /2; // 显示图象的位置
		lpIMGBrowser->point.y = ((rect.bottom - rect.top) - lpIMGBrowser->Size.cy ) /2 ; // 显示图象的位置
		ShowCurImage(hWnd,lpIMGBrowser->hMemoryDC); // 绘制GIF文件
	}
	InvalidateRect(hWnd,NULL,TRUE); // 刷新画面
	return TRUE;
}


static LRESULT OnGifPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	RECT rect;

	GetClientRect(hWnd,&rect);
	hdc = BeginPaint( hWnd, &ps );  // 开始绘制

	// 将内存DC中的画面刷新到屏幕
	BitBlt(hdc,0,0,rect.right,rect.bottom,lpIMGBrowser->hMemoryDC,0,0,SRCCOPY);

//	ShowCurText(hWnd,hdc); //将要显示的文本显示到屏幕

	EndPaint( hWnd, &ps ); // 结束绘制
	return 0;
}

static LRESULT DoGifCreate(HWND hWnd)
{
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
	//SetWindowLong(hWnd,0,(LONG)lpIMGBrowser);

	lpIMGBrowser->hDrawGifThread = NULL;
	lpIMGBrowser->bExit = FALSE;
	lpIMGBrowser->hDrawGifThread = CreateThread(NULL, 0, ShowImageThread, (void *)hWnd, 0, 0 );  // 创建下载管理线程
	return 0;
}
static DWORD WINAPI ShowImageThread(VOID * pParam)
{
	HWND hWnd;
	
		hWnd = (HWND)pParam;  // 得到参数
		if (hWnd == NULL)
			return FALSE;

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
		SendMessage(hWnd,WM_USERGIF,0,0);
		return 0;	
}

static LRESULT DoGifReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
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
		lpIMGBrowser = NULL;
		return TRUE;
}

static void ShowCurImage(HWND hWnd, HDC hdc)
{
		if (lpIMGBrowser->hGif == NULL)
			return ;
		//需要显示
		DrawGifImage(hWnd,hdc,lpIMGBrowser->hGif,lpIMGBrowser->iIndex,&lpIMGBrowser->point);


		if (lpIMGBrowser->iImageNum > 1)
		{	//  只有超过一幅图象是才需要设定下一幅图象的显示时间
			lpIMGBrowser->iDelayTime = GetNextPicTime(lpIMGBrowser->hGif,lpIMGBrowser->iIndex); // 得到到下一幅图象的时间
			lpIMGBrowser->iIndex ++; // 下一幅图象
			if (lpIMGBrowser->iIndex >= lpIMGBrowser->iImageNum)
			{
				lpIMGBrowser->iIndex = 0; // 已经到最后一幅图象, 下次开始显示第一幅图象
				lpIMGBrowser->bExit = TRUE;//退出
			}
		}

}
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

static int GetTotalPic(HGIF hGif)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // 设置结构大小
		//得到GIF的相关信息
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
			return gifInfo.nIndexNum; // 返回当前GIF的图象数
		return 0; // 没有得到图象数
}
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
static int GetNextPicTime(HGIF hGif,int iIndex)
{
	DWORD dwDelay;
	Gif_GetDelay(hGif,iIndex, &dwDelay ); // 得到下一幅图的时间延续

	return (dwDelay * 10 ) - 20; // Delay 是0.01s 为单位，转化成MS
}
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
{
		// 绘制当前指定索引的图象
		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);
}

