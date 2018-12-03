/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：系统状态棒
版本号：1.0.0
开发时期：2005-09-21
作者：李林
修改记录：
******************************************************/
#include <ewindows.h>
#include "desktop.h"
/***************  全局区 定义， 声明 *****************/

const char classStateBar[] = "STATE_BAR";
static HBITMAP hACPower;
static HBITMAP hdrHome;
static HBITMAP hBattery[4];



static LRESULT CALLBACK StateBarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


// ********************************************************************
// 声明：static ATOM RegisterDesktopClass( HINSTANCE hInstance );
// 参数：
//	IN hInstance - 桌面系统的实例句柄
// 返回值：
// 	无
// 功能描述：注册系统桌面类
// 引用: 
// ********************************************************************
static ATOM RegisterStateBarClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;	

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)StateBarWndProc;  // 桌面过程函数
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject( BLACK_BRUSH );//
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classStateBar;
	return RegisterClass(&wcex); // 向系统注册桌面类
}

BOOL InitStateBar( HINSTANCE hInstance )
{
	RegisterStateBarClass( hInstance );
//#ifdef LINUX_KERNEL
	hdrHome = LoadImage( 0, "./kingmos/desktop/hdr_home.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
	hACPower = LoadImage( 0, "./kingmos/desktop/hdr_ac.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
	hBattery[0] = LoadImage( 0, "./kingmos/desktop/hdr_bttry0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
	hBattery[1] = LoadImage( 0, "./kingmos/desktop/hdr_bttry1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
	hBattery[2] = LoadImage( 0, "./kingmos/desktop/hdr_bttry2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
	hBattery[3] = LoadImage( 0, "./kingmos/desktop/hdr_bttry3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
    hBattery[4] = LoadImage( 0, "./kingmos/desktop/hdr_bttry4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//#else
//	hdrHome = LoadImage( 0, ".\\kingmos\\hdr_home.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//	hACPower = LoadImage( 0, ".\\kingmos\\hdr_ac.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//	hBattery[0] = LoadImage( 0, ".\\kingmos\\hdr_bttry0.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//	hBattery[1] = LoadImage( 0, ".\\kingmos\\hdr_bttry1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//	hBattery[2] = LoadImage( 0, ".\\kingmos\\hdr_bttry2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//	hBattery[3] = LoadImage( 0, ".\\kingmos\\hdr_bttry3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//	hBattery[4] = LoadImage( 0, ".\\kingmos\\hdr_bttry4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载位图
//#endif
	return TRUE;
}

// **************************************************
// 声明：static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBitmap -- 位图句柄
// 	IN rect -- 位图大小
// 	IN xOffset -- X偏移
// 	IN yOffset -- Y偏移
// 	IN dwRop -- 绘制模式
// 
// 返回值：无
// 功能描述：绘制位图。
// 引用: 
// **************************************************
void DrawTransparentBitMap(HDC hdc,HBITMAP hBitmap, const RECT * lprect,UINT xOffset,UINT yOffset, DWORD clrTranspant )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // 创建一个兼容的内存句柄
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // 将要绘制的位图设置到内存句柄中
		TransparentBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			clrTranspant
			);

		SelectObject(hMemoryDC,hOldBitmap); // 恢复内存句柄
		DeleteDC(hMemoryDC); // 删除句柄
}

// ********************************************************************
// 声明：static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
// 参数：
//	IN hWnd -- 桌面系统的窗口句柄
//	IN hdc -- 桌面系统的绘画句柄
// 返回值：
//	无
// 功能描述：处理系统桌面窗口背景刷新过程
// 引用: 
// ********************************************************************
/*
static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
{
	RECT rect;
	BITMAP bitmap;


	

	if (g_hDesktopBitmap == NULL)
	{  // 没有装载墙纸
//		RETAILMSG(1,("***** Start Load Bitmap File \r\n"));
		g_hDesktopBitmap = LoadImage( 0, lpDeskTopName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // 装载墙纸
//		RETAILMSG(1,("***** Load Bitmap Success \r\n"));
		if( g_hDesktopBitmap == NULL )
		{  // 装载墙纸失败
			return ;
		}
	}

	GetObject(g_hDesktopBitmap,sizeof(bitmap),&bitmap);  // 得到位图结构
	// 得到位图大小
	rect.left = 0;
	rect.right = 640;
	rect.top = 0;
	rect.bottom = 480 ;
	
	DrawBitMap( hdc, g_hDesktopBitmap, rect, 0, 0 ,SRCCOPY);  // 绘制位图

}
*/
// ********************************************************************
// 声明：static void DoDraw( HWND hWnd, HDC hdc )
// 参数：
//	IN hWnd- 窗口句柄
// 返回值：
//	无
// 功能描述：处理窗口绘画过程
// 引用: 
// ********************************************************************
static void DoDraw( HWND hWnd, HDC hdc )
{
	//画电池
	const RECT rcBattery = { 570, 5, 570 + 65, 5 + 40 };
	const RECT rcHome = { 5, 5, 5 + 40, 5 + 40 };

	DrawTransparentBitMap( hdc, hdrHome, &rcHome, 0, 0, RGB(255, 0, 255) );
	DrawTransparentBitMap( hdc, hBattery[0], &rcBattery, 0, 0, RGB(255, 0, 255) );	
	
}

// ********************************************************************
// 声明：static void OnPaint( HWND hWnd )
// 参数：
//	IN hWnd- 窗口句柄
// 返回值：
//	无
// 功能描述：处理窗口绘画过程
// 引用: 
// ********************************************************************
static void OnPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	
	hdc = BeginPaint( hWnd, &ps );  // 开始绘制
	DoDraw( hWnd, hdc ); 
	EndPaint( hWnd, &ps ); // 结束绘制
}

// ********************************************************************
// 声明：static LRESULT CALLBACK DesktopWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK StateBarWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // 绘画消息
			OnPaint( hWnd );
			return 0;
		//case WM_ERASEBKGND:  // 绘制背景
			//OnDesktopEraseBkgnd( hWnd,( HDC )wParam );
		//	return 0;
/*		case WM_LBUTTONDOWN: // 鼠标左按钮按下，或TOUCH笔按下
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_LBUTTONUP:// 鼠标左按钮弹起，或TOUCH笔离开
			DoDesktopLButtonUp(hWnd,wParam,lParam);
			return 0;
		case WM_TIMER: // 定时器
			DoDesktopTimer(hWnd,wParam,lParam);
			return 0;
		case WM_WINDOWPOSCHANGING: // 窗口位置改变
			return DoWindowPosChanging(hWnd,wParam,lParam);
//            return 0;
		case WM_CREATE: // 创建消息
			DoDesktopCreate(hWnd);
			return 0;
		case WM_CLOSE:  // 关闭消息
			DoDesktopClose(hWnd);
			return 0;
		case WM_DESTROY: // 破坏消息
			PostQuitMessage(0);
			return 0;
*/
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // 窗口默认消息
   }
   return FALSE;
}
