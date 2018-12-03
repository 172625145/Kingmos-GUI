/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：画布类
版本号：1.0.0.456
开发时期：2001-12-23
作者：陈建明 Jami chen 
修改记录：
 		2002.05.24 -- Modified By Jami chen  for Zoom Function 
					   Modify the Zoom Window to Only Display 
**************************************************/

#ifdef EML_DOS
#include <dos.h>
#include <CONIO.H>
#include <STDIO.H>
#endif

//#include <math.h>
#include "eframe.h"
#include "ecanvas.h"
#include "eEdit.h"
#include "eKeybrd.h"

//#define _MAPPOINTER

#define CAN_EDITWIDTH    128
#define CAN_EDITHEIGHT  16

#define MEMORYTOSCREENDC   0x00000000
#define MEMORYTOSHOWDC     0x00000001
#define SHOWDCTOSCREENDC   0x00000002

#define ZOOMMUL  5


typedef struct CanvasPropertyStruct
{
//  HDC hShowDC;     // the Temp dc for Copy to the Screen DC
//  HBITMAP hShowBitMap; // for Show DC
  HDC hMemoryDC;       // The Memory DC of the bitmap
  RECT rectBmp;       // The Rect Of the bitmap
  RECT rectWindow;    // The Rect of the window
//  RECT rectShow;      // the rect of the show dc
  HBITMAP hBitMap;  // The Bitmap In Using
  HBITMAP hOldBitMap; // The Old Bitmap before Selected Current Bitmap
  int iStartx;
  int iStarty;      // the Coordinate of the window Startpoint in the bitmap

// !!! Add By Jami chen in 2003.09.15
  HRGN hRgn;
// !!! Add End By Jami chen in 2003.09.15

  POINT pointDown;     // form the current window Startpoint
  POINT pointCurrent;  // form the current window Startpoint
  POINT pointOld;     // form the prev point

  int iDrawMode;      // The Pen's Draw Mode include Pen ,Line ,Rect,Circle ,rubber and Select Mode
  int iLineWidth;     // The Line Width
  int iFillMode;      // The Fill Mode when Draw Rect and Circle

  RECT rectSelect;   // form the current window Startpoint
  POINT pointOffset;  // from the bitmap Startpoint
  BOOL bMoveSelected;  // Is or Not Move The Selected Rect
  BOOL bZoom;  // 有没有放大

  HWND hEdit;  // 编辑区窗口句柄
  HWND hKeyboard;  // 键盘句柄
//  HWND hZoom;
  BOOL bInputed;  // 是否在进行输入
  POINT pointChar; // 输入字符窗口位置

  COLORREF clColor;  // 前景颜色
  COLORREF clBKColor; // 背景颜色

  UINT bModifyFlag;  // 是否有修改标志
} CANVASPROPERTY,*LPCANVASPROPERTY;

typedef struct _FILLSTRUCT{
	HDC hFillDC;
	int nWidth;
	int nHeight;
	BOOL bStart;
	COLORREF clForeColor;
	COLORREF clFillColor;
}FILLSTRUCT, *PFILLSTRUCT;


static const char classCANVAS[] = "CANVAS";
static const char classMagnifier[]="MAGNIFIER";

static LRESULT CALLBACK CanvasWndProc( HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static void DoPAINT( HWND hwnd ,HDC hdc);
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoGetZoomWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetPixel(HWND hWnd,WPARAM wParam,LPARAM lParam);

static int ToPoint(HWND hWnd,POINT point,UINT iProcess);
static void DrawNewLine(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess);
static void DrawNewRectangle(HDC hdc,HPEN hNewPen,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess);
static void DrawNewCircle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess);
static void Rubber(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess);
static void DrawNewSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,LPPOINT lppoint,UINT iProcess);
static void SetNewSelectRectangle(LPCANVASPROPERTY lpCanvas,POINT point);
static void AdjustRect(LPRECT rectSrc,LPRECT rectObj);
static void  AdjustPoint(LPCANVASPROPERTY lpCanvas,LPPOINT lppoint);
static void MoveSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point);
static void SaveSelectRectToMemory(LPCANVASPROPERTY lpCanvas);
static void Circle(HDC hdc,LPCANVASPROPERTY lpCanvas,int x0,int y0,int x1,int y1,BOOL bCopyToMemory);
static void DrawPoint(HDC hdc,HBRUSH hBrush,int x,int y,int width);
static void RestoreScreen(HDC hdc,LPCANVASPROPERTY lpCanvas,DWORD dwFlag,LPRECT lprect);
static void SaveScreen(HDC hdc,LPCANVASPROPERTY lpCanvas);
static BOOL SetShowBitmap(HWND hWnd,LPCANVASPROPERTY lpCanvas);
static void AirBrush(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess);
static void ZoomBitmapFromShowDCToScreen(HDC hdc,LPCANVASPROPERTY lpCanvas);

static DrawFillArea(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess);
void LineFillArea(short x, short y, PFILLSTRUCT pFillStruct);


static void SendMouseNotified(HWND hWnd,POINT point,UINT iProcess);
static void SendGetColorModeNotified(HWND hWnd,POINT point,COLORREF crColor);
static void SendMagnifierModeNotified(HWND hWnd,POINT point);

//static void Circle0(HDC hdc,int x0,int y0,int x1,int y1);
//static void Circle2(HDC hdc,LPCANVASPROPERTY lpCanvas,int x0,int y0,int x1,int y1,BOOL bCopyToMemory);
static void SetHScrollPage(HWND hWnd);
static void SetVScrollPage(HWND hWnd);
static void InputUserChar(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT pt);
static void WriteCharToScreen(HWND hWnd,LPCANVASPROPERTY lpCanvas);

static LONG Sqrt(LONG Value);
static COLORREF GetPointColor(HWND hWnd,POINT point);
//static LRESULT CreateZoomWindow(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point);
static LRESULT SetZoomStates(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point);

// public extern area
static int CN_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw);
static int CN_GetScrollPos(HWND hWnd, int nBar);
static BOOL CN_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw );

static void GetRectFrom2Pt(POINT pt1,POINT pt2,int iLineWidth,RECT rectWindow,LPRECT lprect);


#define MOUSE_DOWN   0
#define MOUSE_MOVE   1
#define MOUSE_UP     2
#define CANCELACTIVE 3
#define REDRAWOFFSET 4


#define WIDTHSTEP    8
//*************************TEST*********************************
/*
void TestCircleSpeed(HDC hdc)
{
  int i,j;
  int x0,y0,x1,y1;
  struct  time t1,t2;
     gotoxy(10,15);
     printf("Begin Circle0");
     gettime(&t1);
     x0=0;y0=0;
     for (i=0;i<3000;i++)
     {
        x0++;
        y0++;
        for (j=0;j<30;j++)
        {
            x1=x0+j;
            y1=y0+j;
            Circle0(hdc,x0,y0,x1,y1);
        }
     }
     gotoxy(10,16);
     gettime(&t2);
     printf("End Circle0");
     gotoxy(10,17);
     printf("The Used time is: %2d:%02d:%02d.%02d\n",
       t2.ti_hour-t1.ti_hour, t2.ti_min-t1.ti_min, t2.ti_sec-t1.ti_sec, t2.ti_hund-t1.ti_hund);


     gotoxy(10,19);
     printf("Begin Circle");
     gettime(&t1);
     x0=0;y0=0;
     for (i=0;i<3000;i++)
     {
        x0++;
        y0++;
        for (j=0;j<30;j++)
        {
            x1=x0+j;
            y1=y0+j;
            Circle0(hdc,x0,y0,x1,y1);
        }
     }
     gotoxy(10,20);
     gettime(&t2);
     printf("End Circle");
     gotoxy(10,21);
     printf("The Used time is: %2d:%02d:%02d.%02d\n",
       t2.ti_hour-t1.ti_hour, t2.ti_min-t1.ti_min, t2.ti_sec-t1.ti_sec, t2.ti_hund-t1.ti_hund);
}
*/
//**************************************************************

// **************************************************
// 声明：ATOM RegisterCanvasClass( HINSTANCE hInst )
// 参数：
// 	IN hInst -- 实例句柄
// 返回值：返回注册结果
// 功能描述：注册画布类。
// 引用: 
// **************************************************
ATOM RegisterCanvasClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = CanvasWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = 0;//LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject(GRAY_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName = classCANVAS; // 画布类的类名

    return (BOOL)(RegisterClass( &wc ));
}

// **************************************************
// 声明：static void DoPAINT( HWND hWnd ,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 返回值：无
// 功能描述：绘制窗口，处理WM_PAINT消息。
// 引用: 
// **************************************************
static void DoPAINT( HWND hWnd ,HDC hdc)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return ;
      if (lpCanvas->hBitMap==NULL)
        return ;

	  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存到屏幕的显示

}

// **************************************************
// 声明：LRESULT CALLBACK CanvasWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN uMsg -- 要处理的消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 返回值：消息处理结果
// 功能描述：处理画布类消息
// 引用: 
// **************************************************
LRESULT CALLBACK CanvasWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
//	LPCREATESTRUCT lpcs;
	PAINTSTRUCT ps;
 //	DWORD dwStyle;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_CREATE: // 创建画布窗口
//		  EdbgOutputDebugString( "Create Canvas\r\n" );
		  return DoCreate(hWnd,wParam,lParam);
	  case WM_PAINT: // 绘制画布窗口
			hdc = BeginPaint(hWnd, &ps);
			//EdbgOutputDebugString( "Canvas-WM_PAINT\r\n" );
		  DoPAINT( hWnd ,hdc);
			EndPaint(hWnd, &ps);
		  return 0;
   	case WM_LBUTTONDOWN: // 处理鼠标左键按下
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE: // 处理鼠标移动
			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP: // 处理鼠标左键弹起
			return DoLButtonUp(hWnd,wParam,lParam);

    case WM_KILLFOCUS:  // 失去焦点
      return DoKillFocus(hWnd,wParam,lParam);
    case WM_VSCROLL: // 垂直滚动窗口
      return DoVScrollWindow(hWnd,wParam,lParam);
    case WM_HSCROLL: // 水平滚动窗口
      return DoHScrollWindow(hWnd,wParam,lParam);
	case WM_SIZE: // 改变窗口的大小
		return DoSize(hWnd,wParam,lParam);

	case WM_WINDOWPOSCHANGED: // 窗口位置改变
		return DoWindowPosChanged(hWnd,wParam,lParam);
	case WM_DESTROY: // 破坏窗口
      return DoDestory(hWnd,wParam,lParam);

    case CM_SETPROPERTY: // 设置窗口属性
      return DoSetProperty(hWnd,wParam,lParam);
    case CM_SETDRAWMODE: // 设置绘制模式
      return DoSetDrawMode(hWnd,wParam,lParam);
    case CM_GETDRAWMODE: // 得到绘制模式
      return DoGetDrawMode(hWnd,wParam,lParam);
	case CM_SETLINEWIDTH: // 设置线的宽度
      return DoSetLineWidth(hWnd,wParam,lParam);
	case CM_GETLINEWIDTH: // 得到线的宽度
      return DoGetLineWidth(hWnd,wParam,lParam);
	case CM_SETFILLMODE: // 设置填充模式
      return DoSetFillMode(hWnd,wParam,lParam);
	case CM_GETFILLMODE: // 得到填充模式
      return DoGetFillMode(hWnd,wParam,lParam);
	case CM_GETCOLOR: // 得到控制颜色
      return DoGetColor(hWnd,wParam,lParam);
	case CM_SETCOLOR: // 设置控制颜色
      return DoSetColor(hWnd,wParam,lParam);
	case CM_GETBKCOLOR: // 得到背景颜色
      return DoGetBkColor(hWnd,wParam,lParam);
	case CM_SETBKCOLOR: // 设置背景颜色
      return DoSetBkColor(hWnd,wParam,lParam);
//	case CM_GETZOOMWINDOW:
//		return DoGetZoomWindow(hWnd,wParam,lParam);
	case CM_GETMODIFYFLAG: // 得到修改标志
		return DoGetModifyFlag(hWnd,wParam,lParam);
	case CM_SETMODIFYFLAG: // 设置修改标志
		return DoSetModifyFlag(hWnd,wParam,lParam);
	case CM_GETPIXEL: // 得到指定点的颜色
		return DoGetPixel(hWnd,wParam,lParam);
	  default:
    	  return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
//	return 0;
}

// **************************************************
// 声明：static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回0，否则返回-1。
// 功能描述：创建画布控件
// 引用: 
// **************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
//  HDC hdc;

      lpCanvas=malloc(sizeof(CANVASPROPERTY)); // 分配画布类的结构
      if (lpCanvas==NULL)
         return -1;
      lpCanvas->hMemoryDC=NULL; // 设置内存DC为空
      GetClientRect(hWnd,&lpCanvas->rectWindow);

//	  hdc=GetDC(hWnd);
// !!! delete by jami chen in 2003.09.03
/*	  lpCanvas->hShowDC=CreateCompatibleDC(NULL);
	  if (lpCanvas->hShowDC==NULL)
	  {
		  free(lpCanvas);
		  return FALSE;
	  }
*/
// !!! delete end by jami chen in 2003.09.03

	  lpCanvas->rectBmp.left=0;
	  lpCanvas->rectBmp.right=0;
	  lpCanvas->rectBmp.top=0;
	  lpCanvas->rectBmp.bottom=0;

// !!! delete by jami chen in 2003.09.03
//	  lpCanvas->rectShow.left=0;
//	  lpCanvas->rectShow.right=0;
//	  lpCanvas->rectShow.top=0;
//	  lpCanvas->rectShow.bottom=0;
//	  lpCanvas->hShowBitMap=NULL;
//	  SetShowBitmap(hWnd,lpCanvas);
// !!! delete by jami chen in 2003.09.03

/*
      lpCanvas->hShowBitMap=CreateCompatibleBitmap(hdc,lpCanvas->rectWindow.right,lpCanvas->rectWindow.bottom);
	  if (lpCanvas->hShowBitMap==NULL)
	  {
		  free(lpCanvas);
		  return FALSE;
	  }

	  lpCanvas->hShowDC=CreateCompatibleDC(hdc);
	  if (lpCanvas->hShowDC==NULL)
	  {
		  DeleteObject(lpCanvas->hShowBitMap);
		  free(lpCanvas);
		  return FALSE;
	  }
	  lpCanvas->hShowBitMap=SelectObject(lpCanvas->hShowDC,lpCanvas->hShowBitMap);
	  FillRect(lpCanvas->hShowDC,&lpCanvas->rectWindow,(HBRUSH)GetStockObject(GRAY_BRUSH));
	  ReleaseDC(hWnd,hdc);
*/
      lpCanvas->hBitMap=NULL;
//	  lpCanvas->hOldBitMap=SelectObject(lpCanvas->hMemoryDC,lpCanvas->hBitMap);
      lpCanvas->hOldBitMap=NULL;
      lpCanvas->iStartx=0;
      lpCanvas->iStarty=0;

      lpCanvas->iDrawMode=PENMODE;
	  lpCanvas->iLineWidth=1;
	  lpCanvas->iFillMode=FILL_EMPTY;

      lpCanvas->bMoveSelected=FALSE;

      lpCanvas->hEdit=NULL;
      lpCanvas->hKeyboard=NULL;
      lpCanvas->bInputed=FALSE;

//	  lpCanvas->hZoom=NULL;
	  lpCanvas->bZoom=FALSE;
	  lpCanvas->bModifyFlag=FALSE;

      lpCanvas->clColor=CL_BLACK;
      lpCanvas->clBKColor=CL_WHITE;

      lpCanvas->rectSelect.left=0;
      lpCanvas->rectSelect.top=0;
      lpCanvas->rectSelect.right=0;
      lpCanvas->rectSelect.bottom=0;
	
	  lpCanvas->hRgn = NULL;

      SetWindowLong(hWnd,0,(DWORD)lpCanvas); // 设置画布类的结构
//	  EdbgOutputDebugString( "Create Canvas OK\r\n" );
      return 0;
}

// **************************************************
// 声明：static LRESULT DoSetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- MAKELONG(cx,cy)  要在画布上进行处理的位图的宽和高
// 	IN lParam -- HBITMAP 要在画布上进行处理的位图
// 返回值：成功返回TRUE;否则返回FALSE。
// 功能描述：设置画布的属性，处理CM_SETPROPERTY消息。
// 引用: 
// **************************************************
static LRESULT DoSetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  HDC hdc;
  RECT rectRgn;
//  HBITMAP hBitmap;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      // 设置位图大小
      lpCanvas->rectBmp.left=0;
      lpCanvas->rectBmp.top=0;
      lpCanvas->rectBmp.right=(short)LOWORD(wParam);
      lpCanvas->rectBmp.bottom=(short)HIWORD(wParam);

// !!! Add By Jami chen in 2003.09.15
	  rectRgn = lpCanvas->rectBmp;
// !!! Add End By Jami chen in 2003.09.15

      lpCanvas->hBitMap=(HBITMAP)lParam; // 得到位图句柄
//      if (lpCanvas->hBitMap==NULL)
//         return FALSE;
		
	  lpCanvas->iStartx=0; // 设置开始位置
	  lpCanvas->iStarty=0;

/*
	  if (lpCanvas->hBitMap)	
	  {
		  DeleteObject(lpCanvas->hBitMap);
	  }
*/
      if (lpCanvas->hBitMap!=NULL)
	  { // 有位图句柄
		  hdc=GetDC(hWnd); // 得到DC

		  if (lpCanvas->hMemoryDC==NULL)
		  {  // 内存位图不存在
			  lpCanvas->hMemoryDC=CreateCompatibleDC(hdc); // 创建内存DC
			  lpCanvas->hOldBitMap=SelectObject(lpCanvas->hMemoryDC,lpCanvas->hBitMap); // 将位图选择到内存DC
		  }
		  else
		  {
			  SelectObject(lpCanvas->hMemoryDC,lpCanvas->hBitMap);// 将位图选择到内存DC
		  }
	//	  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
// !!! delete by jami chen in 2003.09.03
//		  SetShowBitmap(hWnd,lpCanvas);
// !!! delete end by jami chen in 2003.09.03
		  ReleaseDC(hWnd,hdc); // 释放DC
	  }

      SetHScrollPage(hWnd); // 设置水平滚动条
      SetVScrollPage(hWnd); // 设置垂直滚动条
	  GetClientRect(hWnd,&lpCanvas->rectWindow);

// !!! Add By Jami chen in 2003.09.15
	  IntersectRect(&rectRgn,&lpCanvas->rectWindow,&lpCanvas->rectBmp);
	  lpCanvas->hRgn = CreateRectRgn(rectRgn.left,rectRgn.top,rectRgn.right,rectRgn.bottom);
// !!! Add End By Jami chen in 2003.09.15
	  InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
      return TRUE;
}

// **************************************************
// 声明：static LRESULT DoSetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 画笔模式
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置画笔模式 ，处理CM_SETDRAWMODE消息。
// 引用: 
// **************************************************
static LRESULT DoSetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  int iNewMode;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      iNewMode=(int)wParam; // 得到新的绘图模式
      if (iNewMode==lpCanvas->iDrawMode)
        return FALSE; // 没有改变
	  if (lpCanvas->bInputed==TRUE) // 以前是输入模式
		 WriteCharToScreen(hWnd,lpCanvas); // 写当前字符到屏幕

      if (lpCanvas->iDrawMode==SELECTMODE)
      { // 当前是选择模式
//        HDC hdc;
//           hdc=GetDC(hWnd);
//           DrawNewSelectRectangle(hdc,lpCanvas,point,CANCELACTIVE);
//           DrawNewSelectRectangle(hdc,lpCanvas,NULL,REDRAWOFFSET);
//           ReleaseDC(hWnd,hdc);
           SaveSelectRectToMemory(lpCanvas); // 保存选择矩形内的图象到内存
           // 设置选择无效
           lpCanvas->rectSelect.left=0;
           lpCanvas->rectSelect.top=0;
           lpCanvas->rectSelect.right=0;
           lpCanvas->rectSelect.bottom=0;
           lpCanvas->pointOffset.x=0;
           lpCanvas->pointOffset.y=0;
           lpCanvas->bMoveSelected=FALSE;
		   InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
      }
	  if (iNewMode==INPUTCHARMODE)
	  { // 新的绘制模式是输入字符模式
//		  if (lpCanvas->hZoom)
		  if (lpCanvas->bZoom)
			  return FALSE; // 在放大模式下不能使用输入字符模式
	  }
      lpCanvas->iDrawMode=iNewMode; // 设置新的绘制模式
      return TRUE;
}

// **************************************************
// 声明：static LRESULT DoSetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 线的宽度
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置画布类中线的宽度，处理CM_SETLINEWIDTH消息。
// 引用: 
// **************************************************
static LRESULT DoSetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  int iLineWidth;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return 0;
      iLineWidth=(int)wParam; // 得到线宽
      if (iLineWidth==lpCanvas->iLineWidth)
        return 0; // 没有改变
	  if (lpCanvas->bInputed==TRUE) // 当前在输入模式下
		 WriteCharToScreen(hWnd,lpCanvas); // 将当前的字符写入到屏幕

      lpCanvas->iLineWidth=iLineWidth; // 设置线宽
      return 0;
}
// **************************************************
// 声明：static LRESULT DoGetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前的画笔模式。
// 功能描述：得到画笔模式参数 ，处理CM_GETDRAWMODE消息。
// 引用: 
// **************************************************
static LRESULT DoGetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  
      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return -1;
      return lpCanvas->iDrawMode; // 返回绘制模式
}

// **************************************************
// 声明：static LRESULT DoSetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 要设置填充模式。 
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置画布类的填充模式，处理CM_SETFILLMODE消息。
// 引用: 
// **************************************************
static LRESULT DoSetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  int iFillMode;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return 0;
      iFillMode=(int)wParam; // 得到填充模式
      if (iFillMode==lpCanvas->iFillMode)
        return 0;
	  if (lpCanvas->bInputed==TRUE) // 当前绘制模式是输入字符
		 WriteCharToScreen(hWnd,lpCanvas); // 将当前字符写到屏幕

      lpCanvas->iFillMode=iFillMode; // 设置填充模式
      return 0;
}
// **************************************************
// 声明：static LRESULT DoGetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前的画布类的填充模式。
// 功能描述：得到画布类的填充模式，处理CM_GETFILLMODE消息。
// 引用: 
// **************************************************
static LRESULT DoGetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  
      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return -1;
      return lpCanvas->iFillMode; // 返回当前的填充模式
}

// **************************************************
// 声明：static LRESULT DoGetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前画布类线的宽度。
// 功能描述：得到画布类中线的宽度，处理CM_GETLINEWIDTH消息。
// 引用: 
// **************************************************
static LRESULT DoGetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  
      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return -1;
      return lpCanvas->iLineWidth; // 返回当前的线宽
}

// **************************************************
// 声明：static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：破坏窗口，处理WM_DESTORY消息。
// 引用: 
// **************************************************
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
	  if (lpCanvas->hRgn)
	  { // 删除矩形
		  DeleteObject(lpCanvas->hRgn);
		  lpCanvas->hRgn = NULL;
	  }
	  if (lpCanvas->bInputed==TRUE)
		 WriteCharToScreen(hWnd,lpCanvas); // 写当前输入字符到屏幕
      if (lpCanvas->hMemoryDC)
      { // 删除内存DC
        SelectObject(lpCanvas->hMemoryDC,lpCanvas->hOldBitMap); // 将原来的位图恢复到DC
		DeleteDC(lpCanvas->hMemoryDC); // 删除DC
		lpCanvas->hMemoryDC = NULL;// 设置DC句柄为空
      }
// !!! delete by jami chen in 2003.09.03
/*      if (lpCanvas->hShowDC)
	  {
//        lpCanvas->hShowBitMap=SelectObject(lpCanvas->hShowDC,lpCanvas->hShowBitMap);
		DeleteDC(lpCanvas->hShowDC);
//		DeleteObject(lpCanvas->hShowBitMap);
	  }
*/
// !!! delete end by jami chen in 2003.09.03

      free(lpCanvas);  // 释放画布类结构
      return TRUE;
}
// **************************************************
// 声明：static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：系统要改变控件窗口的大小，处理WM_SIZE消息。
// 引用: 
// **************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      GetClientRect(hWnd,&lpCanvas->rectWindow); // 重新得到窗口句柄
	  return TRUE;
}	
// **************************************************
// 声明：static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：系统已经改变了窗口的位置，处理WM_WINDOWPOSCHANGED消息。
// 引用: 
// **************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
//  HDC hdc;
//  HBITMAP hBitmap;
//  RECT rectWindow;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

/*	
	  if (EqualRect(&lpCanvas->rectWindow,&rectWindow)!=0)
		  return TRUE;

	  lpCanvas->rectWindow=rectWindow;
	  hdc=GetDC(hWnd);
      hBitmap=CreateCompatibleBitmap(hdc,lpCanvas->rectWindow.right,lpCanvas->rectWindow.bottom);
	  if (hBitmap==NULL)
	  {
		  MessageBox(hWnd,"Error","NoMemory",MB_OK);
		  return FALSE;
	  }

	  hBitmap=SelectObject(lpCanvas->hShowDC,hBitmap);
	  if (hBitmap)
		DeleteObject(hBitmap);
	  FillRect(lpCanvas->hShowDC,&lpCanvas->rectWindow,(HBRUSH)GetStockObject(GRAY_BRUSH));
	  if (lpCanvas->hMemoryDC)
		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
	  ReleaseDC(hWnd,hdc);
*/
// !!! delete by jami chen in 2003.09.03
//	  SetShowBitmap(hWnd,lpCanvas);
// !!! delete end by jami chen in 2003.09.03

      SetHScrollPage(hWnd); // 设置水平滚动条
      SetVScrollPage(hWnd); // 设置垂直滚动条
	  GetClientRect(hWnd,&lpCanvas->rectWindow); // 得到窗口句柄
/*
	  if (lpCanvas->hZoom)
	  {
	      GetWindowRect(hWnd,&rectWindow);
		  SetWindowPos(lpCanvas->hZoom,
					 NULL,
					 rectWindow.left,
					 rectWindow.top, 
					 rectWindow.right-rectWindow.left, 
					 rectWindow.bottom-rectWindow.top,
					 SWP_NOZORDER);
	  }
*/
	  return TRUE;
}

// **************************************************
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(X,Y) 鼠标位置
// 返回值：无
// 功能描述：左鼠标键按下，处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;
  LPCANVASPROPERTY lpCanvas;
  RECT rect;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      // 得到鼠标的位置
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
	  if (lpCanvas->bZoom==TRUE)
	  { // 转化为缩小以后的位置
		point.x=point.x/(ZOOMMUL+1);
		point.y=point.y/(ZOOMMUL+1);
	  }
	  SendMouseNotified(hWnd,point,MOUSE_MOVE); // 发送鼠标移动消息

//	  RETAILMSG(1,(TEXT("Down Pos =(%d,%d)"),point.x,point.y));
      if (lpCanvas->hBitMap==NULL) // 没有位图句柄
         return FALSE;

      if (GetFocus()!=hWnd) // 得到焦点窗口
      { // 当前窗口不是焦点窗口
        SetFocus(hWnd); // 设置焦点窗口
      }
//      if (point.x<0||point.y<0)
//        return 0;
	  SetCapture(hWnd); // 抓住鼠标
      lpCanvas->pointDown=point;

	  lpCanvas->pointOld = lpCanvas->pointCurrent;

      lpCanvas->pointCurrent=point;
      if (lpCanvas->iDrawMode==SELECTMODE)
      { // 当前是选择模式
//        rect=lpCanvas->rectSelect;
//        OffsetRect(&rect,lpCanvas->pointOffset.x,lpCanvas->pointOffset.y);
        AdjustRect(&lpCanvas->rectSelect,&rect);
        if (PtInRect(&rect,point))
        {// 当前点在选择框内
          lpCanvas->bMoveSelected=TRUE; // 开始移动选择框
          return 0;
        }
        else if (lpCanvas->iDrawMode==INPUTCHARMODE)
		{ // 当前是输入字符模式
			return 0;
		}
		else
        { // 是其他模式
           SaveSelectRectToMemory(lpCanvas); // 保存选择矩形的内容到内存
		   InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		   // 设置选择矩形无效
           lpCanvas->rectSelect.left=0;
           lpCanvas->rectSelect.top=0;
           lpCanvas->rectSelect.right=0;
           lpCanvas->rectSelect.bottom=0;

           lpCanvas->pointOffset.x=point.x+lpCanvas->iStartx; // 设置下笔点的位置
           lpCanvas->pointOffset.y=point.y+lpCanvas->iStarty;
           lpCanvas->bMoveSelected=FALSE; // 不在移动选择矩形状态
        }
      }
	  else if (lpCanvas->iDrawMode==ZOOMMODE)
	  { // 当前是放大模式
//		  CreateZoomWindow(hWnd,lpCanvas,point);
		  SetZoomStates(hWnd,lpCanvas,point); // 设置放大模式状态
		  return 0;
	  }
/*	  else if (lpCanvas->iDrawMode==GETCOLORMODE)
	  {
		  COLORREF crCurPoint;

			crCurPoint=GetPointColor(hWnd,point);
			SendGetColorModeNotified(hWnd,point,crCurPoint);
	  }
*/
      ToPoint(hWnd,point,MOUSE_DOWN); // 处理该点
      return 0;
}
// **************************************************
// 声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(X,Y) 鼠标位置
// 返回值：无
// 功能描述：鼠标移动，处理WM_MOUSEMOVE消息。
// 引用: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
	  // 得到鼠标的位置
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
	  if (lpCanvas->bZoom==TRUE)
	  { // 是放大状态，得到缩小后的鼠标位置
		point.x=point.x/(ZOOMMUL+1);
		point.y=point.y/(ZOOMMUL+1);
	  }

	  if ((lpCanvas->pointCurrent.x==point.x)&&(lpCanvas->pointCurrent.y==point.y))
	  { // 没有改变位置
		  return FALSE;
	  }
	  SendMouseNotified(hWnd,point,MOUSE_MOVE); // 发送鼠标消息
	  if (GetCapture()!=hWnd) 
         return 0; // 没有抓住鼠标
//	  RETAILMSG(1,(TEXT("Move Pos =(%d,%d)"),point.x,point.y));
//      if (point.x<0||point.y<0)
 //       return 0;
	  ToPoint(hWnd,point,MOUSE_MOVE); // 处理该点
      return 0;
}
// **************************************************
// 声明：static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(X,Y) 鼠标位置
// 返回值：无
// 功能描述：左鼠标键弹起，处理WM_LBUTTONUP消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;

  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
	  // 得到鼠标位置
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
	  if (lpCanvas->bZoom==TRUE)
	  { // 得到缩小后的点
		point.x=point.x/(ZOOMMUL+1);
		point.y=point.y/(ZOOMMUL+1);
	  }
	  SendMouseNotified(hWnd,point,MOUSE_UP); // 发送鼠标消息
//	  RETAILMSG(1,(TEXT("Up Pos =(%d,%d)"),point.x,point.y));


	  if (GetCapture()!=hWnd)
         return 0; // 没有抓住鼠标
      SetCapture(0); // 释放抓住鼠标
//      if (point.x<0||point.y<0)
//        return 0;
	  if (lpCanvas->iDrawMode==INPUTCHARMODE)
	  { // 当前模式为输入字符模式
		  if (lpCanvas->bInputed==FALSE) // 还没有输入
			InputUserChar(hWnd,lpCanvas,point); // 开始输入字符
		  else// 已经开始输入
			WriteCharToScreen(hWnd,lpCanvas); // 保存输入字符到屏幕
	  }
	  else if (lpCanvas->iDrawMode==ZOOMMODE)
	  { // 当前是放大模式
			SendMagnifierModeNotified(hWnd,point); // 发送通知消息
	  }
	  else if (lpCanvas->iDrawMode==GETCOLORMODE)
	  { // 当前是得到颜色模式
		  COLORREF crCurPoint;

			crCurPoint=GetPointColor(hWnd,point); // 得到点的模式
			SendGetColorModeNotified(hWnd,point,crCurPoint); // 发送得到点的颜色通知消息
	  }
	  else
	  {
		  if (lpCanvas->bMoveSelected==TRUE) //已经开始移动选择矩形
			lpCanvas->bMoveSelected=FALSE; // 结束移动
		  else
			ToPoint(hWnd,point,MOUSE_UP); // 处理该点
	  }
      return 0;
}
// **************************************************
// 声明：static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：控件失去焦点，处理WM_KILLFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return 0;
      if (lpCanvas->hBitMap==NULL)
         return FALSE;

      if (lpCanvas->iDrawMode==SELECTMODE)
      { // 当前为选择模式
        //HDC hdc;
           //hdc=GetDC(hWnd);
          // DrawNewSelectRectangle(hdc,lpCanvas,NULL,REDRAWOFFSET);
           //ReleaseDC(hWnd,hdc);
           SaveSelectRectToMemory(lpCanvas); // 保存当前的选择矩形到内存
           // 设置选择举行无效
           lpCanvas->rectSelect.left=0;
           lpCanvas->rectSelect.top=0;
           lpCanvas->rectSelect.right=0;
           lpCanvas->rectSelect.bottom=0;
           lpCanvas->pointOffset.x=0;
           lpCanvas->pointOffset.y=0;
           lpCanvas->bMoveSelected=FALSE;
      }
      return 0;
}

// **************************************************
// 声明：static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 滚动类型
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：垂直滚动窗口，处理WM_VSCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int iPaneInPage,iCurPane,iPaneNumber;
  LPCANVASPROPERTY lpCanvas;
  int iOldStarty;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      if (lpCanvas->hBitMap==NULL)
         return FALSE;
	  if (lpCanvas->bInputed==TRUE)
		 WriteCharToScreen(hWnd,lpCanvas);

	  if (lpCanvas->bZoom==TRUE)
	  { // 当前在放大状态
		  RECT rectClient;
//		  iPaneInPage=(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top)/ZOOMMUL;
		  GetClientRect(hWnd,&rectClient); // 得到客户矩形
		  iPaneInPage=(rectClient.bottom-rectClient.top)/(ZOOMMUL+1); // 得到每页的大小
		  iPaneNumber=lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top; // 得到总的大小
	  }
	  else
	  {
		  iPaneInPage=(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top)/WIDTHSTEP; // 得到每一页的大小
		  iPaneNumber=(lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top)/WIDTHSTEP; // 得到总的大小
	  }
      iPaneNumber-=iPaneInPage; // 滚动条的最大值要减去一屏的大小
      iCurPane=CN_GetScrollPos(hWnd,SB_VERT); // 得到滚动条的位置
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          iCurPane-=iPaneInPage;
          break;
        case SB_PAGEDOWN:  //page down
          iCurPane+=iPaneInPage;
          break;
        case SB_LINEUP:  // line up
          iCurPane--;
          break;
        case SB_LINEDOWN:  // line down
          iCurPane++;
          break;
        case SB_THUMBTRACK: // drag thumb track
          iCurPane=(short)HIWORD(wParam);
          break;
        default:
          return 0;
        }
         iOldStarty=lpCanvas->iStarty; // 得到旧的开始位置
				// vertical scroll window
        if (iCurPane>=iPaneNumber)
        { // 已经超过最大值
		  if (lpCanvas->bZoom==TRUE)
		  {
			iCurPane=iPaneNumber; // 设置到最大值
			lpCanvas->iStarty=iCurPane; // 设置新的开始位置
		  }
		  else
		  {
			iCurPane=iPaneNumber; // 设置到最大值
			lpCanvas->iStarty=lpCanvas->rectBmp.bottom-lpCanvas->rectWindow.bottom; // 设置心得开始位置
		  }
        }
        else if(iCurPane<0)
        { // 已经超过最小值
          iCurPane=0; // 设置到最上面
          lpCanvas->iStarty=0;
        }
        else
		{
			// 设置新的当前位置
		  if (lpCanvas->bZoom==TRUE)
		      lpCanvas->iStarty=iCurPane;
		  else
		      lpCanvas->iStarty=iCurPane*WIDTHSTEP;
		}

        if (lpCanvas->iDrawMode==SELECTMODE)
        {// 偏移选择矩形的位置
          OffsetRect(&lpCanvas->rectSelect,
                     0,
                     iOldStarty-lpCanvas->iStarty);
//          lpCanvas->pointOffset.y+=iOldStarty-lpCanvas->iStarty;
        }
//		RETAILMSG(1,(TEXT("Start y Pos =%d"),lpCanvas->iStarty));

        CN_SetScrollPos(hWnd,SB_VERT,iCurPane,TRUE); // 设置滚动条滑块的位置
// !!! delete by jami chen in 2003.09.03
//		RestoreScreen(NULL,lpCanvas,MEMORYTOSHOWDC);
// !!! delete end by jami chen in 2003.09.03
        InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
				return 0;
}
// **************************************************
// 声明：static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 滚动类型
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：水平滚动窗口，处理WM_HSCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int iPaneInPage,iCurPane,iPaneNumber;
  LPCANVASPROPERTY lpCanvas;
  int iOldStartx;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      if (lpCanvas->hBitMap==NULL)
         return FALSE;

	  if (lpCanvas->bInputed==TRUE) // 当前正在字符输入状态
		 WriteCharToScreen(hWnd,lpCanvas); // 写将当前的字符写到屏幕

	  if (lpCanvas->bZoom==TRUE)
	  { // 是放大模式
		  RECT rectClient;
//		  iPaneInPage=(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left)/ZOOMMUL;
		  GetClientRect(hWnd,&rectClient);
		  iPaneInPage=(rectClient.right-rectClient.left)/(ZOOMMUL+1); // 得到一屏的大小
		  iPaneNumber=lpCanvas->rectBmp.right-lpCanvas->rectBmp.left; // 得到总的大小
	  }
	  else
	  {
		  iPaneInPage=(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left)/WIDTHSTEP; // 得到一屏的大小
		  iPaneNumber=(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left)/WIDTHSTEP; // 得到总的大小
	  }
      iPaneNumber-=iPaneInPage; // 滚动条的最大值要减去一屏的大小
      iCurPane=CN_GetScrollPos(hWnd,SB_HORZ); // 得到滚动条滑块的位置
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          iCurPane-=iPaneInPage;
          break;
        case SB_PAGEDOWN:  //page down
          iCurPane+=iPaneInPage;
          break;
        case SB_LINEUP:  // line up
          iCurPane--;
          break;
        case SB_LINEDOWN:  // line down
          iCurPane++;
          break;
        case SB_THUMBTRACK: // drag thumb track
          iCurPane=(short)HIWORD(wParam);
          break;
        default:
          return 0;
        }

        iOldStartx=lpCanvas->iStartx; // 保存旧的位置

				// vertical scroll window
        if (iCurPane>=iPaneNumber)
        { // 已经超过最大值
		  if (lpCanvas->bZoom==TRUE)
		  { // 放大模式
			iCurPane=iPaneNumber; // 设置到最大值
			lpCanvas->iStartx=iCurPane; // 设置新的开始位置
		  }
		  else
		  {
	          iCurPane=iPaneNumber; // 设置到最大值
		      lpCanvas->iStartx=lpCanvas->rectBmp.right-lpCanvas->rectWindow.right; // 设置新的开始位置
		  }
        }
        else if(iCurPane<0)
        { // 已经超过最小值
          iCurPane=0; // 设置到最小
          lpCanvas->iStartx=0; // 设置新的开始位置
        }
        else
		{
			// 设置新的开始位置
		  if (lpCanvas->bZoom==TRUE)
	          lpCanvas->iStartx=iCurPane;
		  else
	          lpCanvas->iStartx=iCurPane*WIDTHSTEP;
		}

        if (lpCanvas->iDrawMode==SELECTMODE)
        { // 是选择模式，偏移选择矩形的位置
          OffsetRect(&lpCanvas->rectSelect,
                     iOldStartx-lpCanvas->iStartx,
                     0);
//          lpCanvas->pointOffset.x+=iOldStartx-lpCanvas->iStartx;
        }

//		RETAILMSG(1,(TEXT("Start x Pos =%d"),lpCanvas->iStartx));

        CN_SetScrollPos(hWnd,SB_HORZ,iCurPane,TRUE); // 设置滚动条滑块的位置
// !!! delete by jami chen in 2003.09.03
//		RestoreScreen(NULL,lpCanvas,MEMORYTOSHOWDC);
// !!! delete end by jami chen in 2003.09.03
        InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
				return 0;
}
// **************************************************
// 声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回COLORREF(R,G,B) 当前画布类的前景颜色。
// 功能描述：得到画布类的前景颜色，处理CM_GETCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

	  return lpCanvas->clColor; // 返回控制的颜色
}
// **************************************************
// 声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- COLORREF(R,G,B) 要设置的颜色
// 	IN lParam -- 保留
// 返回值：无
// 功能描述： 设置画布类的前景颜色 ，处理CM_SETCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

	  lpCanvas->clColor=(COLORREF)wParam; // 得到控制颜色

// !!! Add By jami chen in 2003.09.09	  
	  if (lpCanvas->hEdit)
	  { // 编辑窗口存在，在字符输入模式
		CTLCOLORSTRUCT stCtlColor;
		  stCtlColor.fMask = CLF_TEXTCOLOR;
		  stCtlColor.cl_Text = lpCanvas->clColor;
		  SendMessage(lpCanvas->hEdit, WM_SETCTLCOLOR, NULL, (LPARAM)&stCtlColor); // 设置编辑区的颜色
	  }
// !!! Add End By jami chen in 2003.09.09	  
	  return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回COLORREF(R,G,B) 当前画布类的背景颜色
// 功能描述：得到画布类的背景颜色，处理CM_GETBKCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoGetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

	  return lpCanvas->clBKColor; // 返回画布背景色
}
// **************************************************
// 声明：static LRESULT DoSetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- COLORREF(R,G,B) 要设置的颜色
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置画布类的背景颜色，处理CM_SETBKCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoSetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

	  lpCanvas->clBKColor=(COLORREF)wParam; // 设置画布背景色
	  return TRUE;
}
/*
static LRESULT DoGetZoomWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0);
      if (lpCanvas==NULL)
         return FALSE;

	  return (LRESULT)lpCanvas->hZoom;
}
*/

// **************************************************
// 声明：static LRESULT DoGetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：如果当前画布类有修改，则返回TRUE，否则返回FALSE。
// 功能描述：得到当前的修改标志，处理CM_GETMODIFYFLAG消息。
// 引用: 
// **************************************************
static LRESULT DoGetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

	  return (LRESULT)lpCanvas->bModifyFlag; // 返回当前的修改标志
}
// **************************************************
// 声明：static LRESULT DoSetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- BOOL 要设置的修改标志
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置画布类的修改标志，处理CM_SETMODIFYFLAG消息。
// 引用: 
// **************************************************
static LRESULT DoSetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;

	  lpCanvas->bModifyFlag=(BOOL)wParam; // 设置当前的修改标志
	  return 0;
}

// **************************************************
// 声明：static LRESULT DoGetPixel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPPOINT 指向POINT的指针，是要得到颜色的点的坐标
// 返回值：指定的点的颜色值，COLORREF(R,G,B)
// 功能描述：得到指定的点的颜色，处理CM_GETPIXEL消息。
// 引用: 
// **************************************************
static LRESULT DoGetPixel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  LPPOINT lpPoint;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return -1;
	
	  lpPoint = (LPPOINT)lParam; // 得到点的位置
	  return GetPixel(lpCanvas->hMemoryDC,lpPoint->x,lpPoint->y); // 返回点的颜色
}

// **************************************************
// 声明：static void SetVScrollPage(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：设置垂直滚动条一页的大小。
// 引用: 
// **************************************************
static void SetVScrollPage(HWND hWnd)
{
	int iPaneNumber,iPaneInPage;
	DWORD dwStyle;
    SCROLLINFO ScrollInfo;
    LPCANVASPROPERTY lpCanvas;
	RECT rectClient;

        lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
        if (lpCanvas==NULL)
           return ;

		GetClientRect(hWnd,&rectClient); // 得到窗口客户矩形
		if (lpCanvas->bZoom==TRUE)
		{ // 当前是放大状态
			iPaneNumber=lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top; // 得到总的大小
			iPaneInPage=(rectClient.bottom-rectClient.top)/(ZOOMMUL+1); // 得到一页的大小
		}
		else
		{
			iPaneNumber=(lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top)/WIDTHSTEP; // 得到总的大小
			iPaneInPage=(rectClient.bottom-rectClient.top)/WIDTHSTEP; // 得到一页的大小
		}
		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (iPaneNumber>iPaneInPage)
		{// need Scroll
			if (!(dwStyle&WS_VSCROLL))
			{// Is Not exist scroll ,must show scroll
				ShowScrollBar(hWnd,SB_VERT,TRUE);
				SetHScrollPage(hWnd);
			}
//			RETAILMSG(1,(TEXT("The y Page =%d"),iPaneInPage));
//			RETAILMSG(1,(TEXT("The y Range =%d"),iPaneNumber-1));

			CN_SetScrollRange(hWnd,SB_VERT,0,iPaneNumber-1,TRUE); // 设置滚动条范围

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =iPaneInPage;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE); // 设置滚动条一页的大小
	        CN_SetScrollPos(hWnd,SB_VERT,0,FALSE); // 设置滑块的位置
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH); // 滚动条ENABLE
		}
		else
		{ // 不需要滚动条
			if (dwStyle&WS_VSCROLL)
			{// Is exist scroll ,must hide scroll
				ShowScrollBar(hWnd,SB_VERT,FALSE);
				SetHScrollPage(hWnd); // 重设水平滚动条
			}
		}
}
// **************************************************
// 声明：static void SetHScrollPage(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：设置水平滚动条一页的大小。
// 引用: 
// **************************************************
static void SetHScrollPage(HWND hWnd)
{
	short iPaneNumber,iPaneInPage;
	DWORD dwStyle;
	SCROLLINFO ScrollInfo;
	LPCANVASPROPERTY lpCanvas;
	RECT rectClient;

        lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
        if (lpCanvas==NULL)
           return ;

		GetClientRect(hWnd,&rectClient); // 得到窗口客户矩形
		if (lpCanvas->bZoom==TRUE)
		{ // 放大状态
			iPaneNumber=(short)(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left); // 得到滚动条总的大小
			iPaneInPage=(rectClient.right-rectClient.left)/(ZOOMMUL+1); // 得到一页的大小
		}
		else
		{ // 一般状态
			iPaneNumber=(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left)/WIDTHSTEP; // 得到总的大小
			iPaneInPage=(rectClient.right-rectClient.left)/WIDTHSTEP; // 得到一页的大小
		}

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (iPaneNumber>iPaneInPage)
		{// need Scroll
			if (!(dwStyle&WS_HSCROLL))
			{// Is Not exist scroll ,must show scroll
				ShowScrollBar(hWnd,SB_HORZ,TRUE);
				SetVScrollPage(hWnd); // 设置垂直滚动条
			}

//			RETAILMSG(1,(TEXT("The x Page =%d"),iPaneInPage));
//			RETAILMSG(1,(TEXT("The x Range =%d"),iPaneNumber-1));

			CN_SetScrollRange(hWnd,SB_HORZ,0,iPaneNumber-1,TRUE); // 设置滚动条的位置

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =iPaneInPage;
			SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE); // 设置滚动条一页的大小
	        CN_SetScrollPos(hWnd,SB_HORZ,0,FALSE); // 设置滑块的位置
			EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH); // 滚动条ENABLE
		}
		else
		{// not need scroll
			if (dwStyle&WS_HSCROLL)
			{// Is exist scroll ,must hide scroll
				ShowScrollBar(hWnd,SB_HORZ,FALSE);
				SetVScrollPage(hWnd); // 设置垂直滚动条
			}
		}
}

// **************************************************
// 声明：static int ToPoint(HWND hWnd,POINT point,UINT iProcess)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN point -- 当前笔所在的点的坐标
// 	IN iProcess -- 当前笔的状态
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：将笔移动到指定的点。
// 引用: 
// **************************************************
static int ToPoint(HWND hWnd,POINT point,UINT iProcess)
{
  LPCANVASPROPERTY lpCanvas;
  HPEN hPen,hMemoryPen,hNewPen;
  HDC hdc;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
      if (lpCanvas==NULL)
         return FALSE;
      if (lpCanvas->hMemoryDC==NULL) 
        return FALSE; // 没有内存DC

      if (lpCanvas->iDrawMode==NODRAWMODE)
        return TRUE; // 没有绘画模式

      if (lpCanvas->iDrawMode==INPUTCHARMODE)
        return TRUE; // 当前的绘画模式是输入字符模式

      hdc=GetDC(hWnd);
// !!! Add By Jami chen in 2003.09.15
	  SelectClipRgn(hdc,lpCanvas->hRgn); // 选择剪切区域
// !!! Add End By Jami chen in 2003.09.15
      // Create pen
//      hNewPen=CreatePen(PS_SOLID,lpCanvas->iLineWidth,CL_BLACK);
      hNewPen=CreatePen(PS_SOLID,lpCanvas->iLineWidth,lpCanvas->clColor); // 创建PEN

// !!! modified by jami chen in 2003.09.03
//      hPen=SelectObject(lpCanvas->hShowDC, hNewPen);
      hPen=SelectObject(hdc, hNewPen); // 选择创建的PEN到DC
// !!! modified end  by jami chen in 2003.09.03
	  hMemoryPen=SelectObject(lpCanvas->hMemoryDC, hNewPen);

      switch(lpCanvas->iDrawMode)
      {
          case PENMODE: // 随手画
          	// 在内存DC中画线段
               MoveTo(lpCanvas->hMemoryDC,
                      (short)(lpCanvas->iStartx+lpCanvas->pointCurrent.x),
                      (short)(lpCanvas->iStarty+lpCanvas->pointCurrent.y));
               LineTo(lpCanvas->hMemoryDC,
                      (short)(lpCanvas->iStartx+point.x),
                      (short)(lpCanvas->iStarty+point.y));
/*
			   RETAILMSG(1,("Line (%d,%d) -- (%d,%d)",
                      (short)(lpCanvas->iStartx+lpCanvas->pointCurrent.x),
                      (short)(lpCanvas->iStarty+lpCanvas->pointCurrent.y),
                      (short)(lpCanvas->iStartx+point.x),
                      (short)(lpCanvas->iStarty+point.y)));
*/
// !!! modified by jami chen in 2003.09.03
//			   RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			   RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
// !!! modified end by jami chen in 2003.09.03
                // 在屏幕DC中画线段
               MoveTo(hdc,lpCanvas->pointCurrent.x,lpCanvas->pointCurrent.y);
               LineTo(hdc,point.x,point.y);

               break;
          case LINEMODE:
               DrawNewLine(hdc,lpCanvas,point,iProcess); // 绘制一条新的线
               break;
          case RECTMODE:
               DrawNewRectangle(hdc,hNewPen,lpCanvas,point,iProcess); // 绘制一个新的矩形
               break;
          case CIRCLEMODE: 
               DrawNewCircle(hdc,lpCanvas,point,iProcess); // 绘制一个新的圆
               break;
          case RUBBERMODE:
               Rubber(hdc,lpCanvas,point,iProcess); // 橡皮檫
               break;
          case SELECTMODE: // 选择模式
               if (lpCanvas->bMoveSelected==TRUE)
               { // 已经开始移动选择矩形
                 MoveSelectRectangle(hdc,lpCanvas,point); // 移动到新的位置
               }
               else
               {
				   RECT rectOldSelect,rect;
                  
				   AdjustPoint(lpCanvas,&point); // 调整点的位置

// !!! modified by jami chen in 2003.09.03
//				  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//                DrawNewSelectRectangle(lpCanvas->hShowDC,lpCanvas,&point,iProcess);
//				  RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
//				  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
					rectOldSelect = lpCanvas->rectSelect; // 得到旧的选择矩形
					InflateRect(&rectOldSelect,1,1);
				    AdjustRect(&rectOldSelect,&rect); // 调整矩形的位置
		  			BitBlt( hdc, // handle to destination device context
						rect.left,   // x-coordinate of destination rectangle's upper-left
											 // corner
						rect.top,  // y-coordinate of destination rectangle's upper-left
											 // corner
						rect.right-rect.left,  // width of destination rectangle
						rect.bottom-rect.top, // height of destination rectangle
						lpCanvas->hMemoryDC,  // handle to source device context
						rect.left + lpCanvas->iStartx,  // x-coordinate of source rectangle's upper-left
											 // corner
						rect.top + lpCanvas->iStarty,   // y-coordinate of source rectangle's upper-left
											 // corner

						SRCCOPY // raster operation code
					);
					DrawNewSelectRectangle(hdc,lpCanvas,&point,iProcess); // 绘制新的选择矩形
//				  RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
// !!! modified end by jami chen in 2003.09.03
					SetNewSelectRectangle(lpCanvas,point); // 设置新的选择矩形
               }
               break;
		  case AIRBRUSHMODE: // 喷雾器
				AirBrush(hdc,lpCanvas,point,iProcess);
				break;
		  case FILLAREAMODE: // 填充模式
				DrawFillArea(hdc,lpCanvas,point,iProcess);
			    break;
          default:
               break;
      }
	  lpCanvas->pointOld = lpCanvas->pointCurrent; // 保留原来的点

      lpCanvas->pointCurrent=point; // 保留当前的点

	  lpCanvas->bModifyFlag=TRUE; // 设置修改标志
// !!! modified  by jami chen in 2003.09.03
//      SelectObject(lpCanvas->hShowDC, hPen);
      SelectObject(hdc, hPen); // 选择原来的PEN到DC
// !!! modified end by jami chen in 2003.09.03
	  SelectObject(lpCanvas->hMemoryDC, hMemoryPen);
       // Delete Rubber pen
      DeleteObject(hNewPen); // 删除PEN

      ReleaseDC(hWnd,hdc); // 释放DC
      return TRUE;
}
// **************************************************
// 声明：static void DrawNewLine(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：新画一条线。
// 引用: 
// **************************************************
static void DrawNewLine(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
     //Clear Old Line
/*		SetROP2(lpCanvas->hMemoryDC,R2_NOT);
    MoveTo(lpCanvas->hMemoryDC,
           lpCanvas->iStartx+lpCanvas->pointDown.x,
           lpCanvas->iStarty+lpCanvas->pointDown.y);
    LineTo(lpCanvas->hMemoryDC,
           lpCanvas->iStartx+lpCanvas->pointCurrent.x,
           lpCanvas->iStarty+lpCanvas->pointCurrent.y);*/
/*
    if (iProcess!=MOUSE_DOWN)
    {
		   SetROP2(hdc,R2_NOT);
       MoveTo(hdc,
              lpCanvas->pointDown.x,
              lpCanvas->pointDown.y);
       LineTo(hdc,
              lpCanvas->pointCurrent.x,
              lpCanvas->pointCurrent.y);
     }

   // Draw new Line
    if (iProcess==MOUSE_UP)
    {
		   SetROP2(lpCanvas->hMemoryDC,R2_COPYPEN);
       MoveTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+lpCanvas->pointDown.x),
              (short)(lpCanvas->iStarty+lpCanvas->pointDown.y));
       LineTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+point.x),
              (short)(lpCanvas->iStarty+point.y));
		   SetROP2(hdc,R2_COPYPEN);
    }
    else
		   SetROP2(hdc,R2_NOT);
*/
    if (iProcess!=MOUSE_DOWN)
	{
		RECT rectRedraw;
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
		// Will Redraw the rect for pointdown to oldpoint

		GetRectFrom2Pt(lpCanvas->pointDown,lpCanvas->pointOld,lpCanvas->iLineWidth,lpCanvas->rectWindow,&rectRedraw); // 根据2点得到矩形
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // 恢复内存到屏幕
// !!! modified end by jami chen in 2003.09.03
	}
   // Draw new Line
    if (iProcess==MOUSE_UP)
    {
//		RECT rectRedraw;
    	// 在内存中画线段
       MoveTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+lpCanvas->pointDown.x),
              (short)(lpCanvas->iStarty+lpCanvas->pointDown.y));
       LineTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+point.x),
              (short)(lpCanvas->iStarty+point.y));
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存到屏幕
// !!! modified end by jami chen in 2003.09.03
		return;
    }
// !!! modified by jami chen in 2003.09.03
/*    MoveTo(lpCanvas->hShowDC,
           lpCanvas->pointDown.x,
           lpCanvas->pointDown.y);
    LineTo(lpCanvas->hShowDC,
           point.x,
           point.y);
	RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
*/
    // 直接在屏幕上画线段
    MoveTo(hdc,
           lpCanvas->pointDown.x,
           lpCanvas->pointDown.y);
    LineTo(hdc,
           point.x,
           point.y);
// !!! modified end by jami chen in 2003.09.03
}
// **************************************************
// 声明：static void DrawNewRectangle(HDC hdc,HPEN hNewPen,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hNewPen -- 指定的笔
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：用指定的笔在当前位置画一个矩形。
// 引用: 
// **************************************************
static void DrawNewRectangle(HDC hdc,HPEN hNewPen,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
//  int x,y,Delta,Delta_bak;
//  long radiou,radiou_bak;
   HBRUSH hBrush;
//   HDC hTempDC;
//   HBITMAP hNewBitmap,hOldBitmap;
   HPEN hOldPen;
   HBRUSH hNewBrush=NULL;

	
    if (iProcess==MOUSE_UP)
	{ // 弹起鼠标
        hOldPen=SelectObject(lpCanvas->hMemoryDC,hNewPen);  // 选择笔
        if (lpCanvas->iFillMode==FILL_EMPTY)
            hBrush=SelectObject(lpCanvas->hMemoryDC, GetStockObject(NULL_BRUSH)); // 不填充
        else if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{ // 用背景色填充
			if (hNewBrush==NULL)
				hNewBrush=CreateSolidBrush(lpCanvas->clBKColor); // 创建背景色刷子
            hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush); // 选择刷子到内存DC
		}
        else
		{ // 用前景色填充
			if (hNewBrush==NULL)
				hNewBrush=CreateSolidBrush(lpCanvas->clColor); // 创建前景色的刷子
            hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush); // 选择刷子到DC
		}
		// 绘矩形
        Rectangle(lpCanvas->hMemoryDC,
            (short)(lpCanvas->pointDown.x+lpCanvas->iStartx),
            (short)(lpCanvas->pointDown.y+lpCanvas->iStarty),
            (short)(point.x+lpCanvas->iStartx),
            (short)(point.y+lpCanvas->iStarty));
        
        SelectObject(lpCanvas->hMemoryDC, hBrush); // 恢复原来的刷子
        // Select Old Pen to TempDC
        hOldPen=SelectObject(lpCanvas->hMemoryDC,hOldPen); // 恢复原来的笔
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存到屏幕
// !!! modified end by jami chen in 2003.09.03
	}
	else
	{
		RECT rectRedraw;
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
		GetRectFrom2Pt(lpCanvas->pointDown,lpCanvas->pointCurrent,lpCanvas->iLineWidth,lpCanvas->rectWindow,&rectRedraw); // 根据2点得到矩形
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // 恢复内存到屏幕
// !!! modified end by jami chen in 2003.09.03

        hOldPen=SelectObject(lpCanvas->hMemoryDC,hNewPen); // 选择笔
		if (lpCanvas->iFillMode==FILL_EMPTY)
// !!! modified by jami chen in 2003.09.03
//			hBrush=SelectObject(lpCanvas->hShowDC, GetStockObject(NULL_BRUSH));
			hBrush=SelectObject(hdc, GetStockObject(NULL_BRUSH)); // 不要填充
// !!! modified by jami chen in 2003.09.03
		else if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{ // 用背景色填充
			hNewBrush=CreateSolidBrush(lpCanvas->clBKColor);
// !!! modified by jami chen in 2003.09.03
//			hBrush=SelectObject(lpCanvas->hShowDC, hNewBrush);
			hBrush=SelectObject(hdc, hNewBrush);
// !!! modified by jami chen in 2003.09.03
		}
		else
		{ // 用前景色填充
			hNewBrush=CreateSolidBrush(lpCanvas->clColor);
// !!! modified by jami chen in 2003.09.03
//			hBrush=SelectObject(lpCanvas->hShowDC, hNewBrush);
			hBrush=SelectObject(hdc, hNewBrush);
// !!! modified by jami chen in 2003.09.03
		}

// !!! modified by jami chen in 2003.09.03
/*		Rectangle(lpCanvas->hShowDC,
				  (short)(lpCanvas->pointDown.x),
				  (short)(lpCanvas->pointDown.y),
				  (short)(point.x),
				  (short)(point.y));

		SelectObject(lpCanvas->hShowDC, hBrush);
*/
		// 画矩形
		Rectangle(hdc,
				  (short)(lpCanvas->pointDown.x),
				  (short)(lpCanvas->pointDown.y),
				  (short)(point.x),
				  (short)(point.y));

		SelectObject(hdc, hBrush); // 恢复刷子
// !!! modified end by jami chen in 2003.09.03

        hOldPen=SelectObject(lpCanvas->hMemoryDC,hOldPen); // 恢复原来的笔

// !!! delete by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
// !!! delete end by jami chen in 2003.09.03
	}
	
	if (hNewBrush)
		DeleteObject(hNewBrush); // 删除刷子
}

// **************************************************
// 声明：static void DrawNewCircle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：在当前位置画一个圆。
// 引用: 
// **************************************************
static void DrawNewCircle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
   HBRUSH hBrush;
   HBRUSH hNewBrush=NULL;
        if (iProcess==MOUSE_UP)
		{  // 鼠标弹起
			if (lpCanvas->iFillMode==FILL_EMPTY)
				hBrush=SelectObject(lpCanvas->hMemoryDC, GetStockObject(NULL_BRUSH)); // 没有填充
			else if (lpCanvas->iFillMode==FILL_BACKGROUND)
			{ // 用背景色填充
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clBKColor);
				hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush);
			}
			else
			{ // 用前景色填充
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clColor);
				hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush);
			}
			// 画圆
			Ellipse(lpCanvas->hMemoryDC,
              lpCanvas->pointDown.x,
              lpCanvas->pointDown.y,
              point.x,
              point.y);
	
			SelectObject(lpCanvas->hMemoryDC, hBrush); // 恢复刷子
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存到屏幕
		}
		else
		{ // 鼠标没有弹起
			RECT rectRedraw;

			if (lpCanvas->iFillMode==FILL_EMPTY)
				hBrush=SelectObject(hdc, GetStockObject(NULL_BRUSH)); // 没有填充
			else if (lpCanvas->iFillMode==FILL_BACKGROUND)
			{ // 用背景色填充
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clBKColor);
				hBrush=SelectObject(hdc, hNewBrush);
			}
			else
			{ // 用前景色填充
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clColor);
				hBrush=SelectObject(hdc, hNewBrush);
			}
			// 得到要刷新的范围
			if (lpCanvas->pointDown.x > lpCanvas->pointOld.x)
			{
				rectRedraw.left = lpCanvas->pointOld.x - lpCanvas->iLineWidth;
				rectRedraw.right = lpCanvas->pointDown.x + lpCanvas->iLineWidth + 1;
			}
			else
			{
				rectRedraw.left = lpCanvas->pointDown.x - lpCanvas->iLineWidth;
				rectRedraw.right = lpCanvas->pointOld.x + lpCanvas->iLineWidth + 1;
			}
			if (lpCanvas->pointDown.y > lpCanvas->pointOld.y)
			{
				rectRedraw.top = lpCanvas->pointOld.y - lpCanvas->iLineWidth;
				rectRedraw.bottom = lpCanvas->pointDown.y + lpCanvas->iLineWidth + 1;
			}
			else
			{
				rectRedraw.top = lpCanvas->pointDown.y - lpCanvas->iLineWidth;
				rectRedraw.bottom = lpCanvas->pointOld.y + lpCanvas->iLineWidth + 1;
			}

			if (rectRedraw.left < 0 )
				rectRedraw.left = 0;
			if (rectRedraw.right > lpCanvas->rectWindow.right)
				rectRedraw.right = lpCanvas->rectWindow.right;
			if (rectRedraw.top < 0 )
				rectRedraw.top = 0;
			if (rectRedraw.bottom > lpCanvas->rectWindow.bottom)
				rectRedraw.bottom = lpCanvas->rectWindow.bottom;
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // 恢复内存到屏幕
			// 画圆
			Ellipse(hdc,
              (lpCanvas->pointDown.x-lpCanvas->iStartx),
              (lpCanvas->pointDown.y-lpCanvas->iStarty),
              (point.x-lpCanvas->iStartx),
              (point.y-lpCanvas->iStarty));

			SelectObject(hdc, hBrush); // 恢复刷子
		}
	if (hNewBrush)
		DeleteObject(hNewBrush); // 删除刷子
			  
}
// **************************************************
// 声明：static void DrawNewCircle1(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：在当前位置画一个圆。这是另一种画圆的方法。
// 引用: 
// **************************************************
static void DrawNewCircle1(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
        if (iProcess==MOUSE_UP)
		{ // 鼠标弹起
			// 在内存中画圆 
			Circle(lpCanvas->hMemoryDC,
			  lpCanvas,
              lpCanvas->pointDown.x,
              lpCanvas->pointDown.y,
              point.x,
              point.y,
			  TRUE);
// !!! modified by jami chen in 2003.09.03
//			RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存到屏幕
// !!! modified end by jami chen in 2003.09.03
		}
		else
		{
			//RestoreScreen(hdc,lpCanvas);
// !!! modified by jami chen in 2003.09.03

/*
			RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
			Circle(lpCanvas->hShowDC,
			  lpCanvas,
              (lpCanvas->pointDown.x-lpCanvas->iStartx),
              (lpCanvas->pointDown.y-lpCanvas->iStarty),
              (point.x-lpCanvas->iStartx),
              (point.y-lpCanvas->iStarty),
			  FALSE);
			RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
*/
			RECT rectRedraw;
			long radiou,x,y;

				// 得到半径
				x=lpCanvas->pointDown.x-lpCanvas->pointOld.x;
				y=lpCanvas->pointDown.y-lpCanvas->pointOld.y;
				radiou=(long)x*x+(long)y*y;
			    radiou=Sqrt(radiou);
				
				if (radiou > 0)
				{ // 得到要恢复的矩形大小
					rectRedraw.left = lpCanvas->pointDown.x - radiou - lpCanvas->iLineWidth;
					rectRedraw.right = lpCanvas->pointDown.x + radiou + lpCanvas->iLineWidth + 1;
					rectRedraw.top = lpCanvas->pointDown.y - radiou - lpCanvas->iLineWidth;
					rectRedraw.bottom = lpCanvas->pointDown.y + radiou + lpCanvas->iLineWidth + 1;
					if (rectRedraw.left < 0 )
						rectRedraw.left = 0;
					if (rectRedraw.right > lpCanvas->rectWindow.right)
						rectRedraw.right = lpCanvas->rectWindow.right;
					if (rectRedraw.top < 0 )
						rectRedraw.top = 0;
					if (rectRedraw.bottom > lpCanvas->rectWindow.bottom)
						rectRedraw.bottom = lpCanvas->rectWindow.bottom;
					RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // 恢复内存到屏幕
				}
				// 画圆
			Circle(hdc,
			  lpCanvas,
              (lpCanvas->pointDown.x-lpCanvas->iStartx),
              (lpCanvas->pointDown.y-lpCanvas->iStarty),
              (point.x-lpCanvas->iStartx),
              (point.y-lpCanvas->iStarty),
			  FALSE);
// !!! modified end by jami chen in 2003.09.03
		}
			  
}
// **************************************************
// 声明：static void Rubber(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：擦掉当前位置的图象。
// 引用: 
// **************************************************
static void Rubber(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
   HPEN hPen,hNewPen;

       // Create Rubber pen
       hNewPen=CreatePen(PS_SOLID,5,CL_WHITE); // 创建橡皮檫

       // Erase Memory
       hPen=SelectObject(lpCanvas->hMemoryDC, hNewPen); // 选择橡皮檫到内存DC
	   SetROP2(lpCanvas->hMemoryDC,R2_COPYPEN); // 设置COPY笔
	   // 檫除2点之间的内容
       MoveTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+lpCanvas->pointCurrent.x),
              (short)(lpCanvas->iStarty+lpCanvas->pointCurrent.y));
       LineTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+point.x),
              (short)(lpCanvas->iStarty+point.y));
       // 恢复原来的笔
       SelectObject(lpCanvas->hMemoryDC, hPen);

// !!! modified by jami chen in 2003.09.03
//	   RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//	   RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
//	   RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
       // 删除屏幕上的笔迹
       hPen=SelectObject(hdc, hNewPen);
	   SetROP2(hdc,R2_COPYPEN);
       MoveTo(hdc,
              (short)(lpCanvas->pointCurrent.x),
              (short)(lpCanvas->pointCurrent.y));
       LineTo(hdc,
              (short)(point.x),
              (short)(point.y));
       SelectObject(hdc, hPen);
// !!! modified end by jami chen in 2003.09.03

       // Delete Rubber pen
       DeleteObject(hNewPen); // 删除橡皮檫
}
// **************************************************
// 声明：static void DrawNewSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,LPPOINT lppoint,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN lppoint -- 当前笔的坐标位置的指针
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：在当前位置重新画一个选择矩形。
// 引用: 
// **************************************************
static void DrawNewSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,LPPOINT lppoint,UINT iProcess)
{
  HBRUSH hBrush;
  HPEN hPen,hNewPen;
//    if (iProcess!=MOUSE_MOVE)
//        return ;

    hNewPen=CreatePen(PS_DOT,1,CL_BLACK); // 创建一个画虚线的笔
    hPen=SelectObject(hdc, hNewPen); // 选择笔到DC
    hBrush=SelectObject(hdc, GetStockObject(NULL_BRUSH));

//	SetROP2(lpCanvas->hShowDC,R2_XORPEN);

    if (iProcess==REDRAWOFFSET)
    { // 重绘原来的选择框
// !!! modified by jami chen in 2003.09.03
/*       Rectangle(lpCanvas->hShowDC,
              lpCanvas->rectSelect.left,
              lpCanvas->rectSelect.top,
              lpCanvas->rectSelect.right,
              lpCanvas->rectSelect.bottom);
*/
       Rectangle(hdc,
              lpCanvas->rectSelect.left,
              lpCanvas->rectSelect.top,
              lpCanvas->rectSelect.right,
              lpCanvas->rectSelect.bottom);
// !!! modified End by jami chen in 2003.09.03
    }
    else
    {
       // Clear Old Rectangle
/*   
	    Rectangle(lpCanvas->hShowDC,
              lpCanvas->rectSelect.left,
              lpCanvas->rectSelect.top,
              lpCanvas->rectSelect.right,
              lpCanvas->rectSelect.bottom);
*/

       // Draw new Rectangle
       if (lppoint!=NULL)
       { // 绘制新的选择框
         Rectangle(hdc,
                   lpCanvas->pointDown.x,
                   lpCanvas->pointDown.y,
                   lppoint->x,
                   lppoint->y);
       }
    }
    SelectObject(hdc, hBrush); // 恢复刷子
    SelectObject(hdc, hPen); // 恢复笔
    DeleteObject(hNewPen); // 删除笔
}

// **************************************************
// 声明：static void SetNewSelectRectangle(LPCANVASPROPERTY lpCanvas,POINT point)
// 参数：
// 	IN lpCanvas -- 画布类结构指针
// 	IN lppoint -- 当前笔的坐标位置的指针
// 返回值：无
// 功能描述：设置新的选择矩形
// 引用: 
// **************************************************
static void SetNewSelectRectangle(LPCANVASPROPERTY lpCanvas,POINT point)
{
  RECT rect;

   // 设置新的选择矩形
   lpCanvas->rectSelect.left=lpCanvas->pointDown.x;
   lpCanvas->rectSelect.top=lpCanvas->pointDown.y;
   lpCanvas->rectSelect.right=point.x;
   lpCanvas->rectSelect.bottom=point.y;

   AdjustRect(&lpCanvas->rectSelect,&rect); // 调整矩形
   // 得到选择框的偏移
   lpCanvas->pointOffset.x=rect.left+lpCanvas->iStartx;
   lpCanvas->pointOffset.y=rect.top+lpCanvas->iStarty;

#ifdef EML_DOS
   gotoxy(40,15);
   printf("%d",lpCanvas->pointOffset.x);
   gotoxy(40,17);
   printf("%d",lpCanvas->pointOffset.y);

#endif
}
// **************************************************
// 声明：static void AdjustRect(LPRECT rectSrc,LPRECT rectObj)
// 参数：
// 	IN rectSrc -- 调整前的矩形
// 	OUT rectObj -- 调整后的矩形
// 返回值：无
// 功能描述：调整矩形到正常状态，即(左，上) -- (右，下)。
// 引用: 
// **************************************************
static void AdjustRect(LPRECT rectSrc,LPRECT rectObj)
{
   // 调整水平方向
   if (rectSrc->left<=rectSrc->right)
   { 
      rectObj->left=rectSrc->left;
      rectObj->right=rectSrc->right;
   }
   else
   {
      rectObj->left=rectSrc->right;
      rectObj->right=rectSrc->left;
   }
   // 调整垂直方向
   if (rectSrc->top<=rectSrc->bottom)
   {
      rectObj->top=rectSrc->top;
      rectObj->bottom=rectSrc->bottom;
   }
   else
   {
      rectObj->top=rectSrc->bottom;
      rectObj->bottom=rectSrc->top;
   }
}

// **************************************************
// 声明：static void  AdjustPoint(LPCANVASPROPERTY lpCanvas,LPPOINT lppoint)
// 参数：
// 	IN lpCanvas -- 画布类结构指针
// 	IN/OUT lppoint -- 当前笔的坐标位置的指针
// 返回值：无
// 功能描述：将当前的点调整到位图范围之内。
// 引用: 
// ************************************************/
static void  AdjustPoint(LPCANVASPROPERTY lpCanvas,LPPOINT lppoint)
{
 // Adjust the point is or not in the rectBmp , if not then reduce to the new point.
  POINT pointOfBitmap;
	
    // 得到点在位图
    pointOfBitmap.x=lppoint->x+lpCanvas->iStartx;
    pointOfBitmap.y=lppoint->y+lpCanvas->iStarty;

    if (pointOfBitmap.x<0)
    { //点不在位图中
      lppoint->x=0-lpCanvas->iStartx;
    }
    if (pointOfBitmap.x>lpCanvas->rectBmp.right)
    { //点不在位图中
      lppoint->x=lpCanvas->rectBmp.right-lpCanvas->iStartx;
    }

    if (pointOfBitmap.y<0)
    { //点不在位图中
      lppoint->y=0-lpCanvas->iStarty;
    }
    if (pointOfBitmap.y>lpCanvas->rectBmp.bottom)
    { //点不在位图中
      lppoint->y=lpCanvas->rectBmp.bottom-lpCanvas->iStarty;
    }

}

// **************************************************
// 声明：static void MoveSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 返回值：无
// 功能描述：移动选择矩形到当前的位置。
// 引用: 
// **************************************************
static void MoveSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point)
{
//   POINT pointOffset;
   RECT rectOldSelect,rect;
// !!! modified by jami chen in 2003.09.03
//		  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//		  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
			rectOldSelect = lpCanvas->rectSelect; // 得到旧的选择矩形
//		    OffsetRect(&rectOldSelect,
//				   lpCanvas->pointOld.x-point.x,
//				   lpCanvas->pointOld.y-point.y);

		    AdjustRect(&rectOldSelect,&rect); // 调整矩形
		    InflateRect(&rect,1,1);
		     // 删除原来的痕迹
		  	BitBlt( hdc, // handle to destination device context
				rect.left,   // x-coordinate of destination rectangle's upper-left
										 // corner
				rect.top,  // y-coordinate of destination rectangle's upper-left
										 // corner
				rect.right-rect.left,  // width of destination rectangle
				rect.bottom-rect.top, // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				rect.left + lpCanvas->iStartx,  // x-coordinate of source rectangle's upper-left
										 // corner
				rect.top + lpCanvas->iStarty,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCCOPY // raster operation code
				);
// !!! modified end by jami chen in 2003.09.03
         // Move Selected Rectangle
		  OffsetRect(&lpCanvas->rectSelect,
				   point.x-lpCanvas->pointCurrent.x,
				   point.y-lpCanvas->pointCurrent.y);

		  AdjustRect(&lpCanvas->rectSelect,&rect); // 调整矩形
// !!! delete By Jami chen in 2003,09,15
#if 0
		  if (lpCanvas->iFillMode==FILL_EMPTY)
		  {
        // Copy a Select rectangle from the memory to Show DC
// !!! modified by jami chen in 2003.09.03
/*
		  	BitBlt( lpCanvas->hShowDC, // handle to destination device context
				lpCanvas->rectSelect.left,   // x-coordinate of destination rectangle's upper-left
										 // corner
				lpCanvas->rectSelect.top,  // y-coordinate of destination rectangle's upper-left
										 // corner
				lpCanvas->rectSelect.right-lpCanvas->rectSelect.left,  // width of destination rectangle
				lpCanvas->rectSelect.bottom-lpCanvas->rectSelect.top, // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				lpCanvas->pointOffset.x,  // x-coordinate of source rectangle's upper-left
										 // corner
				lpCanvas->pointOffset.y,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCAND // raster operation code
				);
*/
		  	BitBlt( hdc, // handle to destination device context
				rect.left,   // x-coordinate of destination rectangle's upper-left
										 // corner
				rect.top,  // y-coordinate of destination rectangle's upper-left
										 // corner
				rect.right-rect.left,  // width of destination rectangle
				rect.bottom-rect.top, // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				lpCanvas->pointOffset.x,  // x-coordinate of source rectangle's upper-left
										 // corner
				lpCanvas->pointOffset.y,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCAND // raster operation code
				);
// !!! modified end by jami chen in 2003.09.03
		  }
		  else
		  {
#endif
// !!! delete end By Jami chen in 2003,09,15
        // Copy a Select rectangle from the memory to Temp DC
// !!! modified by jami chen in 2003.09.03
/*		  	BitBlt( lpCanvas->hShowDC, // handle to destination device context
				lpCanvas->rectSelect.left,   // x-coordinate of destination rectangle's upper-left
										 // corner
				lpCanvas->rectSelect.top,  // y-coordinate of destination rectangle's upper-left
										 // corner
				lpCanvas->rectSelect.right-lpCanvas->rectSelect.left,  // width of destination rectangle
				lpCanvas->rectSelect.bottom-lpCanvas->rectSelect.top, // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				lpCanvas->pointOffset.x,  // x-coordinate of source rectangle's upper-left
										 // corner
				lpCanvas->pointOffset.y,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCCOPY // raster operation code
				);
*/
			  // 绘制新的内容
		  	BitBlt( hdc, // handle to destination device context
				rect.left,   // x-coordinate of destination rectangle's upper-left
										 // corner
				rect.top,  // y-coordinate of destination rectangle's upper-left
										 // corner
				rect.right-rect.left,  // width of destination rectangle
				rect.bottom-rect.top, // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				lpCanvas->pointOffset.x,  // x-coordinate of source rectangle's upper-left
										 // corner
				lpCanvas->pointOffset.y,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCCOPY // raster operation code
				);
// !!! modified end by jami chen in 2003.09.03
//		  }
        // Redraw the Selected Rectangle
// !!! modified by jami chen in 2003.09.03
//      DrawNewSelectRectangle(lpCanvas->hShowDC,lpCanvas,NULL,REDRAWOFFSET);
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
        DrawNewSelectRectangle(hdc,lpCanvas,NULL,REDRAWOFFSET); // 绘制新的矩形框
// !!! modified end by jami chen in 2003.09.03
}
// **************************************************
// 声明：static void SaveSelectRectToMemory(LPCANVASPROPERTY lpCanvas)
// 参数：
// 	IN lpCanvas -- 画布类结构指针
// 返回值：无
// 功能描述：保存选择矩形的内容到内存。
// 引用: 
// **************************************************
static void SaveSelectRectToMemory(LPCANVASPROPERTY lpCanvas)
{
   RECT rect;
//   HBITMAP hMemoryMap=NULL;
   //EBITMAP bmpTemp={OBJ_BITMAP,0,1,1,1,1,1,&rect};
      // The bmpTemp is a temp bitmap for get the hmemory bitmap

        AdjustRect(&lpCanvas->rectSelect,&rect); // 调整矩形
        if (IsRectEmpty(&rect))
          return ; // 空矩形

// !!! delete By Jami chen in 2003.09.15
/*
		  if (lpCanvas->iFillMode==FILL_EMPTY)
		  {
		  	BitBlt( lpCanvas->hMemoryDC, // handle to destination device context
				(short)(lpCanvas->iStartx+rect.left),   // x-coordinate of destination rectangle's upper-left
										 // corner
				(short)(lpCanvas->iStarty+rect.top),  // y-coordinate of destination rectangle's upper-left
										 // corner
				(short)(rect.right-rect.left),  // width of destination rectangle
				(short)(rect.bottom-rect.top), // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				(short)lpCanvas->pointOffset.x,  // x-coordinate of source rectangle's upper-left
										 // corner
				(short)lpCanvas->pointOffset.y,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCAND // raster operation code
				);
		  }
		  else
		  {
*/
// !!! delete end By Jami chen in 2003.09.15
        	  // 将选择框的内容复制到当前的位置
		  	BitBlt( lpCanvas->hMemoryDC, // handle to destination device context
				(short)(lpCanvas->iStartx+rect.left),   // x-coordinate of destination rectangle's upper-left
										 // corner
				(short)(lpCanvas->iStarty+rect.top),  // y-coordinate of destination rectangle's upper-left
										 // corner
				(short)(rect.right-rect.left),  // width of destination rectangle
				(short)(rect.bottom-rect.top), // height of destination rectangle
				lpCanvas->hMemoryDC,  // handle to source device context
				(short)lpCanvas->pointOffset.x,  // x-coordinate of source rectangle's upper-left
										 // corner
				(short)lpCanvas->pointOffset.y,   // y-coordinate of source rectangle's upper-left
										 // corner

				SRCCOPY // raster operation code
				);
//		  }
}
/*
static void Circle0(HDC hdc,int x0,int y0,int x1,int y1)
{
#define SIN45 0.707106781186548
int i;
long tn;
int x,y,xmax,iRadiou;

   iRadiou=(x0-x1)*(x0-x1)+(y0-y1)*(y0-y1);
   if (iRadiou==0)
     return ;
   iRadiou=sqrt(iRadiou);
	 y=iRadiou;
	 x=0;
	 xmax=(double)iRadiou*SIN45;
	 tn=(1-iRadiou*2);
	 while(x<=xmax+1)
		 {
			 if (tn>=0)
				 {
					 tn+=(6+((x-y)<<2));
					 y--;
				 }
			 else
				 tn+=((x<<2)+2);
       SetPixel(hdc,x0+y,y0+x,0);
			 SetPixel(hdc,x0+x,y0+y,0);
			 SetPixel(hdc,x0-x,y0+y,0);
			 SetPixel(hdc,x0-y,y0+x,0);
			 SetPixel(hdc,x0-y,y0-x,0);
			 SetPixel(hdc,x0-x,y0-y,0);
			 SetPixel(hdc,x0+x,y0-y,0);
			 SetPixel(hdc,x0+y,y0-x,0);
			 x++;
			 }
}
*/
// **************************************************
// 声明：static void Circle(HDC hdc,LPCANVASPROPERTY lpCanvas,int x0,int y0,int x1,int y1,BOOL bCopyToMemory)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN x0 -- 原点X坐标
// 	IN y0 -- 原点Y坐标
// 	IN x1 -- 圆周点的X 坐标
// 	IN y1 -- 圆周点的Y坐标
// 	IN bCopyToMemory -- 是否要拷贝到内存
// 返回值：无
// 功能描述：画圆。
// 引用: 
// **************************************************
static void Circle(HDC hdc,LPCANVASPROPERTY lpCanvas,int x0,int y0,int x1,int y1,BOOL bCopyToMemory)
{
  int x,y,Delta,Delta_bak;
  long radiou,radiou_bak;
  HBRUSH hBrush;
//   HDC hTempDC;
//   HBITMAP hNewBitmap,hOldBitmap;


    x=x0-x1;
    y=y0-y1;
    if (x==0&&y==0)
      return ;
    radiou=(long)x*x+(long)y*y; // 得到半径的平方
    if (radiou<=0)
      return;
   

	x0+=lpCanvas->iStartx;
	y0+=lpCanvas->iStarty;

	radiou=(long)Sqrt(radiou); // 得到半径
	Delta=(int)(3-2*radiou);
	radiou_bak=radiou;
	Delta_bak=Delta;

//	hBrush=GetStockObject(BLACK_BRUSH);
	hBrush=CreateSolidBrush(lpCanvas->clColor); // 创建前景刷
	if (lpCanvas->iFillMode!=FILL_EMPTY)
	{ // 有填充
	   HPEN hPen,hNewPen;
	   int width;

		if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{ // 用背景填充
			// Create Fill pen
			hNewPen=CreatePen(PS_SOLID,1,CL_WHITE);

			// Erase Screen
			hPen=SelectObject(hdc, hNewPen);
		}
		else
		{ // 用前景填充
			// Create Current pen
			hNewPen=CreatePen(PS_SOLID,1,lpCanvas->clColor);

			// Erase Screen
			hPen=SelectObject(hdc, hNewPen);
		}
		width=lpCanvas->iLineWidth;
		// 直接用线来填充圆
		for(x=0;x<=radiou;)
		{
			// 画线
			 MoveTo(hdc,(short)(x0-radiou),(short)(y0+x)); 
			 LineTo(hdc,(short)(x0+radiou),(short)(y0+x));
			 
			 MoveTo(hdc,(short)(x0-x),(short)(y0+radiou));
			 LineTo(hdc,(short)(x0+x),(short)(y0+radiou));
			 
			 MoveTo(hdc,(short)(x0-radiou),(short)(y0-x));
			 LineTo(hdc,(short)(x0+radiou),(short)(y0-x));
			 
			 MoveTo(hdc,(short)(x0-x),(short)(y0-radiou));
			 LineTo(hdc,(short)(x0+x),(short)(y0-radiou));

			 if(Delta<0)
				 Delta+=4*x+6;
			  else
			  {
				 Delta+=(int)(4*(x-radiou)+10);
				 radiou--;
			  }
			  x++;
		}
//		if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{
			SelectObject(hdc, hPen); // 恢复原来的笔

			// Delete Fill pen
			DeleteObject(hNewPen); // 删除笔
		}
	}

	if (lpCanvas->iFillMode!=FILL_FOREGROUND)
	{ //  没有填充
		radiou=radiou_bak;
		Delta=Delta_bak;
		// 直接画圆周上的点
		for(x=0;x<=radiou;)
		{
  
			 DrawPoint(hdc,hBrush,(short)(x0+radiou),(short)(y0+x),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0+x),(short)(y0+radiou),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0-x),(short)(y0+radiou),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0-radiou),(short)(y0+x),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0-radiou),(short)(y0-x),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0-x),(short)(y0-radiou),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0+x),(short)(y0-radiou),lpCanvas->iLineWidth);
			 DrawPoint(hdc,hBrush,(short)(x0+radiou),(short)(y0-x),lpCanvas->iLineWidth);

	//	         SetPixel(hdc,(short)(x0+radiou),(short)(y0+x),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0+x),(short)(y0+radiou),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0-x),(short)(y0+radiou),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0-radiou),(short)(y0+x),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0-radiou),(short)(y0-x),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0-x),(short)(y0-radiou),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0+x),(short)(y0-radiou),CL_BLACK);
	//			 SetPixel(hdc,(short)(x0+radiou),(short)(y0-x),CL_BLACK);

		 if(Delta<0)
			 Delta+=4*x+6;
		  else
		  {
			 Delta+=(int)(4*(x-radiou)+10);
			 radiou--;
		  }
		  x++;
		}
	}

	DeleteObject(hBrush); // 删除刷子
}

// **************************************************
// 声明：static void DrawPoint(HDC hdc,HBRUSH hBrush,int x,int y,int width)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN hBrush -- 用来画点的画刷
// 	IN x -- 点的X坐标
// 	IN y -- 点的Y坐标
// 	IN width -- 点的宽度
// 返回值：无
// 功能描述：画点。
// 引用: 
// **************************************************
static void DrawPoint(HDC hdc,HBRUSH hBrush,int x,int y,int width)
{
	RECT rect;
			 // 得到点的大小
			 rect.left=x-width/2;
			 rect.right=x+(width-width/2);
			 rect.top=y-width/2;
			 rect.bottom=y+(width-width/2);

			 FillRect(hdc,&rect,hBrush); // 用刷子填充该点
}
// **************************************************
// 声明：static void InputUserChar(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT pt)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN pt -- 指定点的坐标
// 返回值：无
// 功能描述：在指定点输入字符。
// 引用: 
// **************************************************
static void InputUserChar(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT pt)
{
	HINSTANCE hInst;
	WORD wScreenWidth,wScreenHeight;
	int cx,cy;
	int iLimited;
	CTLCOLORSTRUCT stCtlColor;

	  lpCanvas->pointChar=pt;
	  
	  wScreenWidth=GetSystemMetrics( SM_CXSCREEN );  // 得到屏幕宽度
	  wScreenHeight=GetSystemMetrics( SM_CYSCREEN ); // 得到屏幕高度

	  cx = wScreenWidth > lpCanvas->rectBmp.right ? lpCanvas->rectBmp.right : wScreenWidth; // 得到窗口宽
	  cy = wScreenHeight > lpCanvas->rectBmp.bottom ? lpCanvas->rectBmp.bottom : wScreenHeight; // 得到窗口高

	  cx -= (pt.x -1 ); // 得到窗口的最大宽度
	  cy -= (pt.y -1 ); // 得到窗口的最大高度

	  if (cx > CAN_EDITWIDTH+2)
		  cx = CAN_EDITWIDTH+2; // 得到窗口的使用宽度

	  if (cy > CAN_EDITHEIGHT+2)
		  cy = CAN_EDITHEIGHT+2; // 得到窗口的使用高度

	  hInst=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	  // 创建编辑框
      lpCanvas->hEdit=CreateWindow(classEDIT,
						"",
						WS_BORDER|WS_VISIBLE|WS_CHILD,
						pt.x-1,
						pt.y-1,
//						CAN_EDITWIDTH+2,
//						CAN_EDITHEIGHT+2,
						cx,
						cy,
						hWnd,
						(HMENU)100,
						hInst,
						NULL);

	  if (lpCanvas->hEdit==NULL)
		  return;
// !!! Modified by Jami chen in 2003.09.09
//	  SendMessage(lpCanvas->hEdit,EM_SETLIMITTEXT,16,0);
	  iLimited = cx / 8;
	  SendMessage(lpCanvas->hEdit,EM_SETLIMITTEXT,(WPARAM)iLimited,0); // 设置编辑框的最大限制
	  stCtlColor.fMask = CLF_TEXTCOLOR;
	  stCtlColor.cl_Text = lpCanvas->clColor;
	  SendMessage(lpCanvas->hEdit, WM_SETCTLCOLOR, NULL, (LPARAM)&stCtlColor); // 设置颜色
// !!! Modified End by Jami chen in 2003.09.09
/*
	  ClientToScreen(hWnd,&pt);

	  if (wScreenWidth-pt.x<KEYBOARDWIDTH+2)
	  {
		  pt.x=wScreenWidth-(KEYBOARDWIDTH+2);
	  }

	  if (wScreenHeight-pt.y<KEYBOARDHEIGHT+20+CAN_EDITHEIGHT+4)
		  pt.y-=KEYBOARDHEIGHT+20+4;
	  else
	      pt.y+=CAN_EDITHEIGHT+4;
      lpCanvas->hKeyboard=CreateWindow(classKEYBOARD,
						"键盘",
						WS_BORDER|WS_VISIBLE|WS_CAPTION|WS_POPUP,
						pt.x,
						pt.y,
						KEYBOARDWIDTH+2,
						KEYBOARDHEIGHT+20,
						hWnd,
						(HMENU)101,
						hInst,
						NULL);
*/
      lpCanvas->bInputed=TRUE; // 设置开始输入标志

	  if (lpCanvas->hEdit)
		  SetFocus(lpCanvas->hEdit); // 设置焦点给编辑框
}
// **************************************************
// 声明：static void WriteCharToScreen(HWND hWnd,LPCANVASPROPERTY lpCanvas)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpCanvas -- 画布类结构指针
// 返回值：无
// 功能描述：将字符输出到屏幕上。
// 引用: 
// **************************************************
static void WriteCharToScreen(HWND hWnd,LPCANVASPROPERTY lpCanvas)
{
	LPTSTR lpString;
	DWORD size;
	HDC hdc;
	int OldMode;
	COLORREF oldColor;

	  if (lpCanvas->hEdit)
	  { // 存在编辑框
		  size=SendMessage(lpCanvas->hEdit,WM_GETTEXTLENGTH,0,0); // 得到编辑框的文本长度
		  lpString=malloc(size+1); // 分配空间开保存文本
		  if (lpString)
		  { // 分配成功
			  SendMessage(lpCanvas->hEdit,WM_GETTEXT,(LPARAM)(size+1),(LPARAM)lpString); // 得到文本
			  // 将文本写到屏幕
			  hdc=GetDC(hWnd); // 得到DC
			  if (lpCanvas->iFillMode==FILL_EMPTY)
			  { // 没有填充
				OldMode=SetBkMode(lpCanvas->hMemoryDC,TRANSPARENT);
			  }
			  oldColor=SetTextColor(lpCanvas->hMemoryDC,lpCanvas->clColor); // 设置前景色
			  // 输出文本到内存DC
			  TextOut(lpCanvas->hMemoryDC,
	              (short)(lpCanvas->iStartx+lpCanvas->pointChar.x),
		          (short)(lpCanvas->iStarty+lpCanvas->pointChar.y),
				  lpString,
				  size);
			  SetTextColor(lpCanvas->hMemoryDC,oldColor); // 恢复原来的颜色
			  if (lpCanvas->iFillMode==FILL_EMPTY)
			  {
				SetBkMode(lpCanvas->hMemoryDC,OldMode); // 恢复原来的写模式
			  }
// !!! modified by jami chen in 2003.09.03
//			  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			  RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存内容到屏幕
// !!! modified end by jami chen in 2003.09.03

			  ReleaseDC(hWnd,hdc); // 释放DC
			  free(lpString); // 释放分配的空间
		  }
		  DestroyWindow(lpCanvas->hEdit); // 破坏编辑框
// !!! Add By Jami chen in 2003.09.09
		  lpCanvas->hEdit = NULL; // 句柄置空
// !!! Add End By Jami chen in 2003.09.09
	  }
	  if (lpCanvas->hKeyboard)
	  {
		  DestroyWindow(lpCanvas->hKeyboard); // 删除键盘
	  }
	  lpCanvas->bInputed=FALSE; // 退出文本输入状态
}
// **************************************************
// 声明：static void AirBrushProcess(HDC hdc,POINT point,COLORREF color)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN point -- 指定的点
// 	IN color -- 指定的颜色
// 返回值：无
// 功能描述：在指定的点作喷雾器操作。
// 引用: 
// **************************************************
static void AirBrushProcess(HDC hdc,POINT point,COLORREF color)
{
	int x,y;

		for(x=point.x-3;x<=point.x+3;x++)
		{
			for(y=point.y-3;y<=point.y+3;y++)
			{ // 在指定范围内取随机值是否要画点，3 -- 4点取一点
				if ((random()%3)==1)
					SetPixel(hdc,x,y,color);
			}
		}
}

// **************************************************
// 声明：static void AirBrush(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：喷雾器。
// 引用: 
// **************************************************
static void AirBrush(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
               point.x+=lpCanvas->iStartx;
               point.y+=lpCanvas->iStarty;
			   AirBrushProcess(lpCanvas->hMemoryDC,point,lpCanvas->clColor); // 在内存DC中喷点
// !!! modified by jami chen in 2003.09.03
//			   RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			   RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			   RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // 恢复内存到屏幕
// !!! modified end by jami chen in 2003.09.03
}

// **************************************************
// 声明：static void RestoreScreen(HDC hdc,LPCANVASPROPERTY lpCanvas,DWORD dwFlag,LPRECT lprect)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN dwFlag -- 要恢复的类型
// 	IN lprect -- 要恢复的矩形
// 返回值：无
// 功能描述：将内存中的内容恢复到屏幕。
// 引用: 
// **************************************************
static void RestoreScreen(HDC hdc,LPCANVASPROPERTY lpCanvas,DWORD dwFlag,LPRECT lprect)
{
	if (lprect == NULL)
		lprect = &lpCanvas->rectWindow;
	if (dwFlag==MEMORYTOSCREENDC)
	{ // 恢复内存到屏幕
		if (lpCanvas->bZoom==FALSE)
		{ // 没有放大状态
          // Copy a rectangle of same size with Window from the memory to Temp DC
			    BitBlt( hdc, // handle to destination device context
				  (short)(lpCanvas->rectWindow.left + lprect->left),  // x-coordinate of destination rectangle's upper-left
										   // corner
				  (short)(lpCanvas->rectWindow.top + lprect->top),  // y-coordinate of destination rectangle's upper-left
										   // corner
//				  (short)(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left),  // width of destination rectangle
//				  (short)(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top), // height of destination rectangle
				  (short)(lprect->right-lprect->left),  // width of destination rectangle
				  (short)(lprect->bottom-lprect->top), // height of destination rectangle
				  lpCanvas->hMemoryDC,  // handle to source device context
				  (short)(lpCanvas->iStartx + lprect->left),   // x-coordinate of source rectangle's upper-left
										   // corner
				  (short)(lpCanvas->iStarty + lprect->top),   // y-coordinate of source rectangle's upper-left
										   // corner

				  SRCCOPY // raster operation code
				  );
				return;
		}
		else
		{ // 放大状态
			ZoomBitmapFromShowDCToScreen(hdc,lpCanvas); // 将内存的内容放大到屏幕
		}
	}
// !!! delete by jami chen in 2003.09.03
/*	if (dwFlag==MEMORYTOSHOWDC)
	{
          // Copy a rectangle of same size with Window from the memory to Temp DC
			    BitBlt( lpCanvas->hShowDC, // handle to destination device context
				  (short)lpCanvas->rectWindow.left,  // x-coordinate of destination rectangle's upper-left
										   // corner
				  (short)lpCanvas->rectWindow.top,  // y-coordinate of destination rectangle's upper-left
										   // corner
				  (short)(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left),  // width of destination rectangle
				  (short)(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top), // height of destination rectangle
				  lpCanvas->hMemoryDC,  // handle to source device context
				  (short)lpCanvas->iStartx,   // x-coordinate of source rectangle's upper-left
										   // corner
				  (short)lpCanvas->iStarty,   // y-coordinate of source rectangle's upper-left
										   // corner

				  SRCCOPY // raster operation code
				  );
				return;
	}
	if (dwFlag==SHOWDCTOSCREENDC)
	{
		if (lpCanvas->bZoom==FALSE)
		{
          // Copy a rectangle of same size with Window from the memory to Temp DC
			    BitBlt( hdc, // handle to destination device context
				  (short)lpCanvas->rectWindow.left,  // x-coordinate of destination rectangle's upper-left
										   // corner
				  (short)lpCanvas->rectWindow.top,  // y-coordinate of destination rectangle's upper-left
										   // corner
				  (short)(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left),  // width of destination rectangle
				  (short)(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top), // height of destination rectangle
				  lpCanvas->hShowDC,  // handle to source device context
				  (short)lpCanvas->rectWindow.left,  // x-coordinate of destination rectangle's upper-left
										   // corner
				  (short)lpCanvas->rectWindow.top,  // y-coordinate of destination rectangle's upper-left
										   // corner

				  SRCCOPY // raster operation code
				  );
				return;
		}
		else
		{
			ZoomBitmapFromShowDCToScreen(hdc,lpCanvas);
		}
	}
*/
// !!! delete end by jami chen in 2003.09.03
}

// **************************************************
// 声明：static void ZoomBitmapFromShowDCToScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 返回值：无
// 功能描述：放大位图。
// 引用: 
// **************************************************
static void ZoomBitmapFromShowDCToScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
{
	RECT /*rcMagnifier,*/rcPoint;
	WORD i,j,iWidth,iHeight,iBmpWidth,iBmpHeight;
	HBRUSH hBrush=NULL;
	COLORREF crColor,crOldColor=-1;;


//		GetClientRect(hWnd,&rcMagnifier);
		// 得到放大屏幕所需要位图的宽与高
		iWidth=(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left)/(ZOOMMUL+1);  // +1 is must a edge
		iHeight=(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top)/(ZOOMMUL+1);  // +1 is must a edge

		iBmpWidth=(WORD)(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left);
		iBmpHeight=(WORD)(lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top);

//		RETAILMSG(1,(TEXT("The bmp width =%d"),iBmpWidth));
//		RETAILMSG(1,(TEXT("The bmp height =%d"),iBmpHeight));
//		RETAILMSG(1,(TEXT("x Pos =%d"),lpMagnifier->x));
//		RETAILMSG(1,(TEXT("y Pos =%d"),lpMagnifier->y));

//		hBrush=CreateSolidBrush(CL_WHITE);
//		FillRect(hdc,&lpCanvas->rectWindow,hBrush);
//		DeleteObject(hBrush);
		// 将位图上的每一个点放大成一个  ZOOMMUL * ZOOMMUL 的矩形
		for (i=0;i<=iWidth && i<iBmpWidth;i++)
		{
			for (j=0;j<=iHeight && j<iBmpHeight;j++)
			{
//				RETAILMSG(1,(TEXT("%d,%d"),i+lpMagnifier->x,j+lpMagnifier->y));
// !!! delete by jami chen in 2003.09.03
//				crColor=GetPixel(lpCanvas->hShowDC,i,j);
				crColor=GetPixel(lpCanvas->hMemoryDC,i,j); // 得到点的颜色
// !!! delete end by jami chen in 2003.09.03
				if (crColor==-1)
					crColor=CL_WHITE;
				// 得到该点放大后的矩形
				rcPoint.left=i*(ZOOMMUL+1)+1;
				rcPoint.top=j*(ZOOMMUL+1)+1;
				rcPoint.right=rcPoint.left+ZOOMMUL;
				rcPoint.bottom=rcPoint.top+ZOOMMUL;
				if (crOldColor!=crColor)
				{ // 当前的点的颜色与原来的不同，需要重新创建刷子
					if (hBrush)
					{ // 删除原来的刷子
						DeleteObject(hBrush);
						hBrush=NULL;
					}
					hBrush=CreateSolidBrush(crColor); // 创建新的刷子
					crOldColor=crColor; // 保存颜色
				}

//				hBrush=CreateSolidBrush(crColor);
				FillRect(hdc,&rcPoint,hBrush); // 绘制该点
//				DeleteObject(hBrush);
			}
		}

		if (hBrush)
		{
			DeleteObject(hBrush); // 删除刷子
			hBrush=NULL;
		}

		//ReleaseDC(hMainWnd,hMainDC);
}

// **************************************************
// 声明：static void SaveScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 返回值：无
// 功能描述：保存屏幕内容到内存。
// 引用: 
// **************************************************
static void SaveScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
{
          // Copy a rectangle of same size with Window from the memory to Temp DC
		// 将屏幕上的内容写到内存DC
			    BitBlt( 
				  lpCanvas->hMemoryDC,  // handle to source device context
				  (short)lpCanvas->iStartx,   // x-coordinate of source rectangle's upper-left
										   // corner
				  (short)lpCanvas->iStarty,   // y-coordinate of source rectangle's upper-left
										   // corner
				  (short)(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left),  // width of destination rectangle
				  (short)(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top), // height of destination rectangle

				  hdc, // handle to destination device context
				  (short)lpCanvas->rectWindow.left,  // x-coordinate of destination rectangle's upper-left
										   // corner
				  (short)lpCanvas->rectWindow.top,  // y-coordinate of destination rectangle's upper-left
										   // corner

				  SRCCOPY // raster operation code
				  );
}

// **************************************************
// 声明：static void SendMouseNotified(HWND hWnd,POINT point,UINT iProcess)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN point -- 当前的点的坐标
// 	IN iProcess -- 当前的笔的状态
// 返回值：无
// 功能描述：发送鼠标通知消息。
// 引用: 
// **************************************************
static void SendMouseNotified(HWND hWnd,POINT point,UINT iProcess)
{
	NMCANVAS nmCanvas;
	HWND hParent;
	LPCANVASPROPERTY lpCanvas;
  
		lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
		if (lpCanvas==NULL)
           return ;
        point.x+=lpCanvas->iStartx;
		point.y+=lpCanvas->iStarty;

		nmCanvas.hdr.hwndFrom=hWnd;
		nmCanvas.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID); // 得到ID
		nmCanvas.hdr.code=CN_MOUSECHANGED;
		nmCanvas.iMouseStatus=iProcess;
		nmCanvas.iDrawMode=lpCanvas->iDrawMode;
		nmCanvas.ptAction=point;
		hParent=GetParent(hWnd); // 得到父窗口
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmCanvas); // 发送通知消息到父窗口
}
// **************************************************
// 声明：static void SendGetColorModeNotified(HWND hWnd,POINT point,COLORREF crColor)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN point -- 当前的点的坐标
// 	IN crColor -- 得到的颜色
// 返回值：无
// 功能描述：发送得到颜色模式的通知消息。
// 引用: 
// **************************************************
static void SendGetColorModeNotified(HWND hWnd,POINT point,COLORREF crColor)
{
	NMGETCOLOR nmGetColor;
	HWND hParent;
	LPCANVASPROPERTY lpCanvas;
  
		lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
		if (lpCanvas==NULL)
           return ;
        point.x+=lpCanvas->iStartx;
		point.y+=lpCanvas->iStarty;

		nmGetColor.hdr.hwndFrom=hWnd;
		nmGetColor.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmGetColor.hdr.code=CN_GETPOINTCOLOR;
		nmGetColor.crColor=crColor;
		nmGetColor.ptAction=point;
		hParent=GetParent(hWnd); // 得到父窗口
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmGetColor); // 发送通知消息到父窗口
}

// **************************************************
// 声明：static void SendMagnifierModeNotified(HWND hWnd,POINT point)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN point -- 当前的点的坐标
// 返回值：无
// 功能描述：发送放大位图的通知消息。
// 引用: 
// **************************************************
static void SendMagnifierModeNotified(HWND hWnd,POINT point)
{
	NMMAGNIFIER nmMagnifier;
	HWND hParent;
	LPCANVASPROPERTY lpCanvas;
  
		lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // 得到画布类的结构
		if (lpCanvas==NULL)
           return ;
        point.x+=lpCanvas->iStartx;
		point.y+=lpCanvas->iStarty;

		nmMagnifier.hdr.hwndFrom=hWnd;
		nmMagnifier.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmMagnifier.hdr.code=CN_MAGNIFIER;
		nmMagnifier.ptAction=point;
		hParent=GetParent(hWnd); // 得到父窗口
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmMagnifier); // 发送消息到父窗口
}

// **************************************************
// 声明：static LONG Sqrt(LONG Value)
// 参数：
// 	IN Value -- 要开根号的值
// 返回值：返回开根号后的值
// 功能描述：开根号。
// 引用: 
// **************************************************
static LONG Sqrt(LONG Value)
{
	LONG newV=0;

	if (Value<0)
		return 0;
	while(1)
	{
		if (Value<newV*newV)
			return (newV-1); // 得到当前值开根号后的整数值
		newV++;
	}
}
// **************************************************
// 声明：static COLORREF GetPointColor(HWND hWnd,POINT point)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN point -- 指定的点的坐标
// 返回值：COLORREF(R,G,B), 返回得到的颜色值。
// 功能描述：得到指定的颜色试值。
// 引用: 
// **************************************************
static COLORREF GetPointColor(HWND hWnd,POINT point)
{
//	HDC hdc;
	COLORREF crColor;
	LPCANVASPROPERTY lpCanvas;

	lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0);  // 得到画布类的结构
	if (lpCanvas==NULL)
       return -1;

//	hdc=GetDC(hWnd);
//	crColor=GetPixel(hdc,point.x,point.y);
//	ReleaseDC(hWnd,hdc);
	// 得到点的颜色
	crColor=GetPixel(lpCanvas->hMemoryDC,
                      (short)(lpCanvas->iStartx+point.x),
                      (short)(lpCanvas->iStarty+point.y));

	return crColor;
}
/*
static LRESULT CreateZoomWindow(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point)
{
	static RECT rectWindow;
	RECT rectClient;

	if (lpCanvas->hZoom==NULL)
	{
		GetWindowRect(hWnd,&rectWindow);
		GetClientRect(hWnd,&rectClient);
//		SetWindowPos(hWnd,NULL,0,0,160,160,SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
		lpCanvas->hZoom=CreateWindow(classMagnifier,
									 "",
									WS_POPUP|WS_VISIBLE|WS_BORDER|WS_VSCROLL|WS_HSCROLL,
									rectWindow.left,
									rectWindow.top, 
									rectWindow.right-rectWindow.left, 
									rectWindow.bottom-rectWindow.top,
									//160,
									//160,
									hWnd,
									(HMENU)0xffff,
									GetWindowLong(hWnd,GWL_HINSTANCE),
									&lpCanvas->rectBmp);
		if (lpCanvas->hZoom)
			SendMessage(lpCanvas->hZoom,MM_ASSOCIATE,(WPARAM)lpCanvas->hShowDC,(LPARAM)hWnd);
	}
	else
	{
		DestroyWindow(lpCanvas->hZoom);
		lpCanvas->hZoom=NULL;
//		SetWindowPos(hWnd,NULL,0,0,rectWindow.right-rectWindow.left,rectWindow.bottom-rectWindow.top,SWP_NOMOVE|SWP_NOZORDER|SWP_NOREDRAW);
	}
	InvalidateRect(GetParent(hWnd),NULL,TRUE);
	return 0;
}
*/
// **************************************************
// 声明：static LRESULT SetZoomStates(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前的点的坐标
// 返回值：无
// 功能描述：设置放大状态。
// 引用: 
// **************************************************
static LRESULT SetZoomStates(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point)
{ 
	if (lpCanvas->bZoom==FALSE)
	{ // 原来没有放大，则现在需要放大
		lpCanvas->bZoom=TRUE; // 设置当前放大
	}
	else
	{ // 原来已经放大，则现在要恢复原样
		lpCanvas->iStartx=0;
		lpCanvas->iStarty=0;
		lpCanvas->bZoom=FALSE;
	}
//	SetShowBitmap(hWnd,lpCanvas);
    SetHScrollPage(hWnd); // 设置水平滚动条
    SetVScrollPage(hWnd); // 设置垂直滚动条
	InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
	return 0;
}



// **********************************************************************/
// This is magnifier window 放大窗口
// **********************************************************************/
typedef struct MagnifierStruct
{
  HWND hAssociate;
  HDC  hAssociateDC;
  short x;
  short y;
  RECT rect;
} MAGNIFIER,*LPMAGNIFIER;

//const char classMagnifier[]="MAGNIFIER";

LRESULT CALLBACK MagnifierWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static LRESULT DoMagnifierCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMagnifierPaint( HWND hWnd ,HDC hdc);
static LRESULT DoMagnifierEraseBKGnd(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMagnifierLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMagnifierMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMagnifierLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoAssociate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMagnifierHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMagnifierVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT ReleaseMagnifierWindow(HWND hWnd);

static void DrawMagnifier(HWND hWnd,HDC hdc,LPMAGNIFIER lpMagnifier);
static void SetMagnifierHScrollPage(HWND hWnd);
static void SetMagnifierVScrollPage(HWND hWnd);



// **************************************************
// 声明：ATOM RegisterMagnifierClass( HINSTANCE hInst )
// 参数：
// 	IN hInst -- 实例句柄
// 返回值：注册返回值
// 功能描述：注册放大窗口类。
// 引用: 
// **************************************************
ATOM RegisterMagnifierClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = MagnifierWndProc; // 放大窗口的过程函数
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = 0;//LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = 0;
    wc.lpszClassName = classMagnifier; // 放大窗口类名

    return (BOOL)(RegisterClass( &wc )); // 注册类
}
// **************************************************
// 声明：LRESULT CALLBACK MagnifierWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN uMsg -- 要处理的消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 返回值：消息处理结果
// 功能描述：处理画布放大窗口类消息
// 引用: 
// **************************************************
LRESULT CALLBACK MagnifierWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_PAINT: // 绘制放大窗口
			hdc = BeginPaint(hWnd, &ps);
		    DoMagnifierPaint( hWnd ,hdc);
			EndPaint(hWnd, &ps);
		    return 0;
	  case WM_ERASEBKGND: // 删除背景
		    return DoMagnifierEraseBKGnd(hWnd,wParam,lParam);
   	  case WM_LBUTTONDOWN: // 鼠标左键按下
			return DoMagnifierLButtonDown(hWnd,wParam,lParam);
   	  case WM_MOUSEMOVE: // 鼠标移动
			return DoMagnifierMouseMove(hWnd,wParam,lParam);
   	  case WM_LBUTTONUP: // 鼠标左键弹起
			return DoMagnifierLButtonUp(hWnd,wParam,lParam);
	  case WM_VSCROLL: // 垂直滚动屏幕
	 	    return DoMagnifierVScrollWindow(hWnd,wParam,lParam);
	  case WM_HSCROLL: // 水平滚动屏幕
		    return DoMagnifierHScrollWindow(hWnd,wParam,lParam);
	  case WM_CREATE: // 创建放大窗口
		    DoMagnifierCreate(hWnd,wParam,lParam);
		    return 0;
       case WM_DESTROY: // 破坏放大窗口
			ReleaseMagnifierWindow(hWnd);
			break;

	  case MM_ASSOCIATE: // 关联被放大的DC与窗口句柄
			return DoAssociate(hWnd,wParam,lParam);
	  default:
    	    return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT DoMagnifierCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回0，否则返回-1
// 功能描述：创建画布放大窗口，处理WM_CREATE消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	LPRECT lprectMain;
	LPCREATESTRUCT lpcs;
		
		lpcs = (LPCREATESTRUCT) lParam; // 得到创建参数

	    lprectMain=lpcs->lpCreateParams; // 得到主窗口的大小
		lpMagnifier=(LPMAGNIFIER)malloc(sizeof(MAGNIFIER)); // 分配放大窗口数据结构
		if (lpMagnifier==NULL)
			return -1;
		// 初始化结构
		lpMagnifier->hAssociate=NULL;
		lpMagnifier->hAssociateDC=NULL;
		lpMagnifier->x=0;
		lpMagnifier->y=0;
		if (lprectMain!=NULL)
			lpMagnifier->rect=*lprectMain; // 设置窗口矩形
		else
//			GetWindowRect(GetParent(hWnd),&lpMagnifier->rect);
			GetWindowRect(lpcs->hParent,&lpMagnifier->rect);

		SetWindowLong(hWnd,0,(DWORD)lpMagnifier); // 设置数据结构到窗口
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMagnifierEraseBKGnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- hdc 设备句柄
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：删除画布放大窗口背景，处理WM_ERASEBKGND消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierEraseBKGnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
//	HBRUSH hBrush;
	RECT rcClient;
	WORD i,iWidth,iHeight;
	HPEN hNewPen;
//	HBRUSH hBrush;

	LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
			return 0;
		hdc=(HDC)wParam;

		GetClientRect(hWnd,&rcClient); // 得到窗口客户区大小
		
		// 得到可以放大的位图大小
		iWidth=(rcClient.right-rcClient.left)/(ZOOMMUL+1);  // +1 is must a edge
		iHeight=(rcClient.bottom-rcClient.top)/(ZOOMMUL+1);  // +1 is must a edge
		
		if (iWidth>=(lpMagnifier->rect.right-lpMagnifier->rect.left))
		{ // 可以放大的位图大于实际的位图
			iWidth=(lpMagnifier->rect.right-lpMagnifier->rect.left-1); // 得到实际的位图大小
			rcClient.right=rcClient.left+iWidth*(ZOOMMUL+1); // 重新设置客户矩形大小
		}

		if (iHeight>=(lpMagnifier->rect.bottom-lpMagnifier->rect.top))
		{ // 可以放大的位图大于实际位图
			iHeight=lpMagnifier->rect.bottom-lpMagnifier->rect.top-1; // 达到实际位图大小
			rcClient.bottom=rcClient.top+iHeight*(ZOOMMUL+1); // 重新设置客户矩形大小
		}
		
		hNewPen=CreatePen(PS_DASH,1,CL_LIGHTGRAY); // 得到间隔线的笔
		hNewPen=SelectObject(hdc,hNewPen); // 选择笔到DC

		// 画垂直间隔线
		for (i=0;i<=iWidth;i++)
		{
               MoveTo(hdc,
                      i*(ZOOMMUL+1),
                      rcClient.top);
               LineTo(hdc,
                      i*(ZOOMMUL+1),
                      rcClient.bottom);
		}
		// 画水平间隔线
		for (i=0;i<=iHeight;i++)
		{
               MoveTo(hdc,
					  rcClient.left,
                      i*(ZOOMMUL+1));
               LineTo(hdc,
					  rcClient.right,
                      i*(ZOOMMUL+1));
		}
		hNewPen=SelectObject(hdc,hNewPen); // 恢复原来的笔
		DeleteObject(hNewPen); // 删除笔
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMagnifierPaint( HWND hWnd ,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam --  无
// 	IN lParam -- 无
// 返回值：无
// 功能描述：绘制画布放大窗口消息，处理WM_PAINT消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierPaint( HWND hWnd ,HDC hdc)
{
	LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL) // 没有关联句柄
			return 0;
		DrawMagnifier(hWnd,hdc,lpMagnifier); // 绘制放大窗口
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMagnifierLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y) 鼠标坐标
// 返回值：无
// 功能描述：画布放大窗口处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	SHORT x,y;
//	HWND hParent;
    
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL)
			return 0;
		// 得到缩小后点的位置，因为实际处理要在原来的位图上处理，然后将处理后的位图放大
		x=x/(ZOOMMUL+1)+lpMagnifier->x;
		y=y/(ZOOMMUL+1)+lpMagnifier->y;

//		hParent=GetParent(hWnd);
		SendMessage(lpMagnifier->hAssociate,WM_LBUTTONDOWN,wParam,MAKELPARAM(x,y)); // 通知关联窗口处理鼠标左键按下
//		if (SendMessage(hParent,CM_GETZOOMWINDOW,0,0)!=NULL)
		if (SendMessage(lpMagnifier->hAssociate,CM_GETZOOMWINDOW,0,0)!=NULL) // 无效处理
		{
			if (GetCapture()!=hWnd)
				SetCapture(hWnd);
		}
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMagnifierMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y) 鼠标坐标
// 返回值：无
// 功能描述：画布放大窗口处理WM_MOUSEMOVE消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	SHORT x,y;
    
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL)
			return 0;
		if (GetCapture()!=hWnd)
			return 0;
		// 得到实际位置
		x=x/(ZOOMMUL+1)+lpMagnifier->x;
		y=y/(ZOOMMUL+1)+lpMagnifier->y;

		SetCapture(lpMagnifier->hAssociate);
		SendMessage(lpMagnifier->hAssociate,WM_MOUSEMOVE,wParam,MAKELPARAM(x,y)); // 发送消息通知关联窗口处理鼠标移动
		SetCapture(hWnd);
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMagnifierLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y) 鼠标坐标
// 返回值：无
// 功能描述：画布放大窗口处理WM_LBUTTONUP消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	SHORT x,y;
    
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL)
			return 0;
		if (GetCapture()!=hWnd)
			return 0;
		// 得到实际位置
		x=x/(ZOOMMUL+1)+lpMagnifier->x;
		y=y/(ZOOMMUL+1)+lpMagnifier->y;

		SetCapture(lpMagnifier->hAssociate);
		SendMessage(lpMagnifier->hAssociate,WM_LBUTTONUP,wParam,MAKELPARAM(x,y)); // 通知关联窗口处理鼠标左键弹起
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		return 0;
}

// **************************************************
// 声明：static LRESULT DoAssociate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- hdc 关联设备句柄
// 	IN lParam -- hWnd 关联窗口句柄
// 返回值：无
// 功能描述：设置关联窗口，处理MM_ASSOCIATE消息。
// 引用: 
// **************************************************
static LRESULT DoAssociate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
			return 0;
		
		lpMagnifier->hAssociate=(HWND)lParam; // 设置关联窗口
		lpMagnifier->hAssociateDC=(HDC)wParam; // 设置关联DC
		//lpMagnifier->x = LOWORD(wParam);
		//lpMagnifier->y = HIWORD(wParam);
		SetMagnifierHScrollPage(hWnd); // 设置水平滚动条
		SetMagnifierVScrollPage(hWnd); // 设置垂直滚动条
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		return 0;
}


// **************************************************
// 声明：static void DrawMagnifier(HWND hWnd,HDC hdc,LPMAGNIFIER lpMagnifier)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpMagnifier -- 画布放大窗口结构
// 返回值：无
// 功能描述：绘制画布放大窗口。
// 引用: 
// **************************************************
static void DrawMagnifier(HWND hWnd,HDC hdc,LPMAGNIFIER lpMagnifier)
{
	HWND hMainWnd;
//	HDC hMainDC;
	RECT rcMagnifier,rcPoint;
	WORD i,j,iWidth,iHeight,iBmpWidth,iBmpHeight;
	HBRUSH hBrush;
	COLORREF crColor;

		hMainWnd=lpMagnifier->hAssociate; // 得到关联窗口句柄
		//hMainDC=GetDC(hMainWnd);

		GetClientRect(hWnd,&rcMagnifier);
		
		// 得到需要放大的位图大小
		iWidth=(rcMagnifier.right-rcMagnifier.left)/(ZOOMMUL+1);  // +1 is must a edge
		iHeight=(rcMagnifier.bottom-rcMagnifier.top)/(ZOOMMUL+1);  // +1 is must a edge

		// 得到位图的实际大小
		iBmpWidth=(WORD)(lpMagnifier->rect.right-lpMagnifier->rect.left);
		iBmpHeight=(WORD)(lpMagnifier->rect.bottom-lpMagnifier->rect.top);

//		RETAILMSG(1,(TEXT("The bmp width =%d"),iBmpWidth));
//		RETAILMSG(1,(TEXT("The bmp height =%d"),iBmpHeight));
//		RETAILMSG(1,(TEXT("x Pos =%d"),lpMagnifier->x));
//		RETAILMSG(1,(TEXT("y Pos =%d"),lpMagnifier->y));
		// 绘制每一个点
		for (i=0;i<=iWidth && i<iBmpWidth;i++)
		{
			for (j=0;j<=iHeight && j<iBmpHeight;j++)
			{
//				RETAILMSG(1,(TEXT("%d,%d"),i+lpMagnifier->x,j+lpMagnifier->y));
				crColor=GetPixel(lpMagnifier->hAssociateDC,i+lpMagnifier->x,j+lpMagnifier->y); // 得到点的颜色
				if (crColor==-1)
					crColor=CL_WHITE;
				// 得到该点的大小与位置
				rcPoint.left=i*(ZOOMMUL+1)+1;
				rcPoint.top=j*(ZOOMMUL+1)+1;
				rcPoint.right=rcPoint.left+ZOOMMUL;
				rcPoint.bottom=rcPoint.top+ZOOMMUL;
				// 绘制该点
				hBrush=CreateSolidBrush(crColor);
				FillRect(hdc,&rcPoint,hBrush);
				DeleteObject(hBrush);
			}
		}
		//ReleaseDC(hMainWnd,hMainDC);
}

// **************************************************
// 声明：static void SetMagnifierVScrollPage(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：设置画布放大窗口垂直滚动页
// 引用: 
// **************************************************
static void SetMagnifierVScrollPage(HWND hWnd)
{
	short iPaneNumber,iPaneInPage;
	DWORD dwStyle;
    SCROLLINFO ScrollInfo;
	LPMAGNIFIER lpMagnifier;
	RECT rcMagnifier;//,rcMainWnd;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (dwStyle&WS_VSCROLL)
		{
			lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
			if (lpMagnifier==NULL)
			   return ;
			
			GetClientRect(hWnd,&rcMagnifier); // 得到客户矩形
			//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

			iPaneInPage=(rcMagnifier.bottom-rcMagnifier.top)/(ZOOMMUL+1); // 得到一页的大小
//			iPaneNumber=(rcMainWnd.bottom-rcMainWnd.top)-iPaneInPage;
//			iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-iPaneInPage;
			iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-1; // 得到总的大小

			if (iPaneNumber<0)
			{ // 隐藏滚动条
				ShowScrollBar(hWnd,SB_VERT,FALSE);
				return;
			}
			else
			{ // 显示滚动条
				ShowScrollBar(hWnd,SB_VERT,TRUE);
			}

			RETAILMSG(1,(TEXT("The y Page =%d"),iPaneInPage));
			RETAILMSG(1,(TEXT("The y Range =%d"),iPaneNumber));

			CN_SetScrollRange(hWnd,SB_VERT,0,iPaneNumber,TRUE); // 设置滚动条的范围

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =iPaneInPage;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE); // 设置滚动条
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH); // 滚动条ENABLE
		}
}
// **************************************************
// 声明：static void SetMagnifierHScrollPage(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：设置画布放大窗口水平滚动页
// 引用: 
// **************************************************
static void SetMagnifierHScrollPage(HWND hWnd)
{
	short iPaneNumber,iPaneInPage;
	DWORD dwStyle;
	SCROLLINFO ScrollInfo;
	LPMAGNIFIER lpMagnifier;
	RECT rcMagnifier;//,rcMainWnd;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (dwStyle&WS_HSCROLL)
		{

				lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
				if (lpMagnifier==NULL)
				   return ;
				GetClientRect(hWnd,&rcMagnifier); // 得到窗口客户矩形
				//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

				iPaneInPage=(rcMagnifier.right-rcMagnifier.left)/(ZOOMMUL+1); // 得到一页的大小
//				iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-iPaneInPage;
				iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-1; // 得到总的大小

				if (iPaneNumber<0)
				{ // 隐藏滚动条
					ShowScrollBar(hWnd,SB_HORZ,FALSE);
					return;
				}
				else
				{ // 显示滚动条
					ShowScrollBar(hWnd,SB_HORZ,TRUE);
				}
				RETAILMSG(1,(TEXT("The x Page =%d"),iPaneInPage));
				RETAILMSG(1,(TEXT("The x Range =%d"),iPaneNumber));

				CN_SetScrollRange(hWnd,SB_HORZ,0,iPaneNumber,TRUE); // 设置滚动条范围

				ScrollInfo.cbSize=sizeof(SCROLLINFO);
				ScrollInfo.fMask=SIF_PAGE;
			  // set vertical scroll page
				ScrollInfo.nPage =iPaneInPage;
				SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE); // 设置滚动条一页的大小
		        EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH); // 滚动条ENABLE
		}
}
// **************************************************
// 声明：static LRESULT DoMagnifierVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 滚动类型
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：画布放大窗口垂直滚动窗口，处理WM_VSCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iPaneInPage,iCurPane,iPaneNumber;
	LPMAGNIFIER lpMagnifier;
	RECT rcMagnifier;//,rcMainWnd;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
		   return 0;
		
		GetClientRect(hWnd,&rcMagnifier); // 得到窗口客户矩形
		//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

		iPaneInPage=(rcMagnifier.bottom-rcMagnifier.top)/(ZOOMMUL+1); // 得到一页的大小
//		iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-iPaneInPage;
		iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-1; // 得到总的大小

  		iCurPane=lpMagnifier->y;
		switch(LOWORD(wParam))
        {
			case SB_PAGEUP:  // page up
			  iCurPane-=iPaneInPage;
			  break;
			case SB_PAGEDOWN:  //page down
			  iCurPane+=iPaneInPage;
			  break;
			case SB_LINEUP:  // line up
			  iCurPane--;
			  break;
			case SB_LINEDOWN:  // line down
			  iCurPane++;
			  break;
			case SB_THUMBTRACK: // drag thumb track
			  iCurPane=(short)HIWORD(wParam);
			  break;
			default:
			  return 0;
        }
		if (iCurPane<0)
			iCurPane=0;
		else if((iCurPane+iPaneInPage)>iPaneNumber)
			iCurPane=iPaneNumber-iPaneInPage+1;
		RETAILMSG(1,(TEXT("The y Pos =%d"),iCurPane));
		if (lpMagnifier->y==iCurPane)
			return 0;
		lpMagnifier->y=iCurPane;
        CN_SetScrollPos(hWnd,SB_VERT,iCurPane,TRUE); // 设置滚动条滑块的位置
        InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		return 0;
}
// **************************************************
// 声明：static LRESULT DoMagnifierHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD 滚动类型
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：画布放大窗口垂直滚动窗口，处理WM_HSCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoMagnifierHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int iPaneInPage,iCurPane,iPaneNumber;
  LPMAGNIFIER lpMagnifier;
  RECT rcMagnifier;//,rcMainWnd;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
		   return 0;
		GetClientRect(hWnd,&rcMagnifier); // 得到客户矩形
		//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

		iPaneInPage=(rcMagnifier.right-rcMagnifier.left)/(ZOOMMUL+1); // 得到一页的打下
//		iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-iPaneInPage;
		iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-1; // 得到总的大小
		iCurPane=lpMagnifier->x;

		switch(LOWORD(wParam))
        {
			case SB_PAGEUP:  // page up
			  iCurPane-=iPaneInPage;
			  break;
			case SB_PAGEDOWN:  //page down
			  iCurPane+=iPaneInPage;
			  break;
			case SB_LINEUP:  // line up
			  iCurPane--;
			  break;
			case SB_LINEDOWN:  // line down
			  iCurPane++;
			  break;
			case SB_THUMBTRACK: // drag thumb track
			  iCurPane=(short)HIWORD(wParam);
			  break;
			default:
			  return 0;
        }

		if (iCurPane<0)
			iCurPane=0; // 滚到最前面
		else if((iCurPane+iPaneInPage)>iPaneNumber)
			iCurPane=iPaneNumber-iPaneInPage+1; // 滚到最后面

		RETAILMSG(1,(TEXT("The x Pos =%d"),iCurPane));
		if (lpMagnifier->x==iCurPane)
			return 0; // 没有改变
		lpMagnifier->x=iCurPane;
        CN_SetScrollPos(hWnd,SB_HORZ,iCurPane,TRUE); // 设置当前位置
        InvalidateRect(hWnd,NULL,TRUE);
		return 0;
}
// **************************************************
// 声明：static LRESULT ReleaseMagnifierWindow(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：释放画布放大窗口,处理WM_DESTROY消息。
// 引用: 
// **************************************************
static LRESULT ReleaseMagnifierWindow(HWND hWnd)
{
  LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // 得到放大窗口数据结构
		if (lpMagnifier==NULL)
		   return FALSE;
		free(lpMagnifier); // 释放数据结构
		return TRUE;
}

// ****************************************************************************************************/
//  Set ShowBitmap
//  The ShowBitmap Rect is RectWindow and RectBmp intersection
// ****************************************************************************************************/
// !!! delete by jami chen in 2003.09.03
/*
static BOOL SetShowBitmap(HWND hWnd,LPCANVASPROPERTY lpCanvas)
{
	HDC hdc;
	RECT rectShow,rectClient;
	HBITMAP hBitmap;

	  GetClientRect(hWnd,&rectClient);
	  if (IntersectRect(&rectShow, &lpCanvas->rectBmp, &rectClient)==0)
		  return FALSE;
	  if (EqualRect(&lpCanvas->rectShow,&rectShow)!=0)
	  {
		  if (lpCanvas->hMemoryDC)
			RestoreScreen(NULL,lpCanvas,MEMORYTOSHOWDC);
		  return TRUE; // the rect is equal
	  }

	  lpCanvas->rectShow=rectShow;
	  hdc=GetDC(hWnd);
      hBitmap=CreateCompatibleBitmap(hdc,lpCanvas->rectShow.right,lpCanvas->rectShow.bottom);
	  if (hBitmap==NULL)
	  {
		  MessageBox(hWnd,"Error","NoMemory",MB_OK);
		  return FALSE;
	  }
	  lpCanvas->hShowBitMap=hBitmap;
	  hBitmap=SelectObject(lpCanvas->hShowDC,hBitmap);
	  if (hBitmap)
		DeleteObject(hBitmap);
//	  FillRect(lpCanvas->hShowDC,&lpCanvas->rectWindow,(HBRUSH)GetStockObject(GRAY_BRUSH));
//	  FillRect(lpCanvas->hShowDC,&lpCanvas->rectShow,(HBRUSH)GetStockObject(GRAY_BRUSH));
	  if (lpCanvas->hMemoryDC)
		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
	  ReleaseDC(hWnd,hdc);
	  return TRUE;
}
*/
// !!! delete end by jami chen in 2003.09.03
/****************************************************************************************************/

// **************************************************
// 声明：static DrawFillArea(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// 参数：
// 	IN hdc -- 设备句柄
// 	IN lpCanvas -- 画布类结构指针
// 	IN point -- 当前笔的坐标位置
// 	IN iProcess -- 当前笔的状态
// 返回值：无
// 功能描述：填充区域。
// 引用: 
// **************************************************
static DrawFillArea(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
	FILLSTRUCT FillStruct;

	    if (iProcess==MOUSE_UP)
		{ // 鼠标弹起
			// 设置填充结构
			FillStruct.bStart = TRUE;
			FillStruct.clFillColor = lpCanvas->clColor;
			FillStruct.clForeColor = GetPixel(hdc,point.x,point.y);
			FillStruct.hFillDC = lpCanvas->hMemoryDC;
			FillStruct.nWidth = lpCanvas->rectBmp.right - lpCanvas->rectBmp.left;
			FillStruct.nHeight = lpCanvas->rectBmp.bottom - lpCanvas->rectBmp.top;
			LineFillArea((short)point.x,(short)point.y,&FillStruct); // 填充区域
// !!! modified  by jami chen in 2003.09.03
//			RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
// !!! modified end by jami chen in 2003.09.03
		}
}

// **************************************************
// 声明：void LineFillArea(short x, short y, PFILLSTRUCT pFillStruct)
// 参数：
// 	IN x -- 开始填充点的X坐标
// 	IN y -- 开始填充点的Y坐标
// 	IN pFillStruct -- 填充结构
// 返回值：无
// 功能描述：填充。
// 引用: 
// **************************************************
void LineFillArea(short x, short y, PFILLSTRUCT pFillStruct)
{
	static int iIndex = 0;
	short nLeft, nRight;
	short nTempX;
	nLeft = x, nRight = x + 1;
	iIndex ++;
	RETAILMSG(1,("%d\r\n",iIndex));
	if (pFillStruct->bStart)
	{ // 当前是第一点
		SetPixel(pFillStruct->hFillDC, x, y, pFillStruct->clFillColor); // 得到点的颜色
		if (GetPixel(pFillStruct->hFillDC, x, y) == pFillStruct->clForeColor) // 与填充的颜色相同，不用填充
			return ;
		SetPixel(pFillStruct->hFillDC, x, y, pFillStruct->clForeColor); // 设置该点
		pFillStruct->bStart = FALSE; // 设置第一点已经完成
	}
	// 往左填充
	while (nLeft >= 0 && GetPixel(pFillStruct->hFillDC, nLeft, y) == pFillStruct->clForeColor)
		SetPixel (pFillStruct->hFillDC, nLeft --, y, pFillStruct->clFillColor);
	// 往右填充
	while (nRight < pFillStruct->nWidth && GetPixel(pFillStruct->hFillDC, nRight, y) == pFillStruct->clForeColor)
		SetPixel (pFillStruct->hFillDC, nRight ++, y, pFillStruct->clFillColor);
		
	nLeft ++;
	nRight --;
	if (nLeft > nRight)
		nLeft = nRight = x;
	// 往上填充
	nTempX = nRight;
	while(nTempX >= nLeft && y - 1 >= 0)
	{
		if (GetPixel(pFillStruct->hFillDC, nTempX, y - 1) == pFillStruct->clForeColor)
		{
			LineFillArea(nTempX, (short)(y - 1), pFillStruct);
		}
		nTempX --;
	}
	// 往下填充
	nTempX = nRight;
	while(nTempX >= nLeft && y + 1 < pFillStruct->nHeight)
	{
		if (GetPixel(pFillStruct->hFillDC, nTempX, y + 1) == pFillStruct->clForeColor)
		{
			LineFillArea(nTempX, (short)(y + 1), pFillStruct);
		}
		nTempX --;
	}
	iIndex --;
}


// **************************************************
// 声明：static	int CN_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nBar -- 要改变位置的滚动条
// 	IN nPos -- 要设置的当前位置
// 	IN bRedraw -- 是否要重画
// 返回值：无
// 功能描述：设置滚动条的位置。
// 引用: 
// **************************************************
static	int CN_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			si.nPos=nPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // 设置滚动条的滑块位置
			return 0;
}
// **************************************************
// 声明：static int CN_GetScrollPos(HWND hWnd, int nBar)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nBar -- 要得到位置的滚动条
// 返回值：返回滚动条的位置。
// 功能描述：得到滚动条的位置。
// 引用: 
// **************************************************
static int CN_GetScrollPos(HWND hWnd, int nBar)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			GetScrollInfo(hWnd,nBar,&si); // 得到滚动条的滑块位置
			return si.nPos;
}

// **************************************************
// 声明：static BOOL CN_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nBar -- 要设置滚动条的
// 	IN nMinPos -- 滚动条的最小值
// 	IN nMaxPos -- 滚动条的最大值
// 	IN bRedraw -- 要不要重画
// 返回值：无
// 功能描述：设置滚动条的范围。
// 引用: 
// **************************************************
static BOOL CN_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_RANGE;
			si.nMin=nMinPos;
			si.nMax=nMaxPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // 设置滚动条的范围
			return TRUE;
}

// **************************************************
// 声明：static void GetRectFrom2Pt(POINT pt1,POINT pt2,int iLineWidth,RECT rectWindow,LPRECT lprect)
// 参数：
// 	IN pt1 -- 点1 
// 	IN pt2 -- 点2
// 	IN iLineWidth -- 线的宽度
// 	IN rectWindow -- 窗口矩形
// 	OUT lprect  --得到的新的矩形
// 返回值：无
// 功能描述：得到2个点之间的矩形。
// 引用: 
// **************************************************
static void GetRectFrom2Pt(POINT pt1,POINT pt2,int iLineWidth,RECT rectWindow,LPRECT lprect)
{
	// 得到水平方向的点
		if (pt2.x > pt1.x)
		{
			lprect->left = pt1.x - iLineWidth;
			lprect->right = pt2.x +iLineWidth;
		}
		else
		{
			lprect->left = pt2.x - iLineWidth;
			lprect->right = pt1.x + iLineWidth;
		}
		
		// 得到垂直方向的点
		if (pt2.y > pt1.y)
		{
			lprect->top = pt1.y - iLineWidth;
			lprect->bottom = pt2.y + iLineWidth;
		}
		else
		{
			lprect->top = pt2.y - iLineWidth;
			lprect->bottom = pt1.y + iLineWidth;
		}
		// 矫正各个点的正确性
		if (lprect->left < 0 )
			lprect->left = 0; // 不能超过窗口的左面
		if (lprect->right > rectWindow.right)
			lprect->right = rectWindow.right; // 不能超过窗口的右面
		if (lprect->top < 0 )
			lprect->top = 0; // 不能超过窗口的上面
		if (lprect->bottom > rectWindow.bottom)
			lprect->bottom = rectWindow.bottom; // 不能超过窗口的下面
}
