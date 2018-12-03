/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����������
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-12-23
���ߣ��½��� Jami chen 
�޸ļ�¼��
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
  BOOL bZoom;  // ��û�зŴ�

  HWND hEdit;  // �༭�����ھ��
  HWND hKeyboard;  // ���̾��
//  HWND hZoom;
  BOOL bInputed;  // �Ƿ��ڽ�������
  POINT pointChar; // �����ַ�����λ��

  COLORREF clColor;  // ǰ����ɫ
  COLORREF clBKColor; // ������ɫ

  UINT bModifyFlag;  // �Ƿ����޸ı�־
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
// ������ATOM RegisterCanvasClass( HINSTANCE hInst )
// ������
// 	IN hInst -- ʵ�����
// ����ֵ������ע����
// ����������ע�ử���ࡣ
// ����: 
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
    wc.lpszClassName = classCANVAS; // �����������

    return (BOOL)(RegisterClass( &wc ));
}

// **************************************************
// ������static void DoPAINT( HWND hWnd ,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// ����ֵ����
// �������������ƴ��ڣ�����WM_PAINT��Ϣ��
// ����: 
// **************************************************
static void DoPAINT( HWND hWnd ,HDC hdc)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return ;
      if (lpCanvas->hBitMap==NULL)
        return ;

	  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ浽��Ļ����ʾ

}

// **************************************************
// ������LRESULT CALLBACK CanvasWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN uMsg -- Ҫ�������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// ����ֵ����Ϣ������
// ��������������������Ϣ
// ����: 
// **************************************************
LRESULT CALLBACK CanvasWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
//	LPCREATESTRUCT lpcs;
	PAINTSTRUCT ps;
 //	DWORD dwStyle;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_CREATE: // ������������
//		  EdbgOutputDebugString( "Create Canvas\r\n" );
		  return DoCreate(hWnd,wParam,lParam);
	  case WM_PAINT: // ���ƻ�������
			hdc = BeginPaint(hWnd, &ps);
			//EdbgOutputDebugString( "Canvas-WM_PAINT\r\n" );
		  DoPAINT( hWnd ,hdc);
			EndPaint(hWnd, &ps);
		  return 0;
   	case WM_LBUTTONDOWN: // ��������������
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE: // ��������ƶ�
			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP: // ��������������
			return DoLButtonUp(hWnd,wParam,lParam);

    case WM_KILLFOCUS:  // ʧȥ����
      return DoKillFocus(hWnd,wParam,lParam);
    case WM_VSCROLL: // ��ֱ��������
      return DoVScrollWindow(hWnd,wParam,lParam);
    case WM_HSCROLL: // ˮƽ��������
      return DoHScrollWindow(hWnd,wParam,lParam);
	case WM_SIZE: // �ı䴰�ڵĴ�С
		return DoSize(hWnd,wParam,lParam);

	case WM_WINDOWPOSCHANGED: // ����λ�øı�
		return DoWindowPosChanged(hWnd,wParam,lParam);
	case WM_DESTROY: // �ƻ�����
      return DoDestory(hWnd,wParam,lParam);

    case CM_SETPROPERTY: // ���ô�������
      return DoSetProperty(hWnd,wParam,lParam);
    case CM_SETDRAWMODE: // ���û���ģʽ
      return DoSetDrawMode(hWnd,wParam,lParam);
    case CM_GETDRAWMODE: // �õ�����ģʽ
      return DoGetDrawMode(hWnd,wParam,lParam);
	case CM_SETLINEWIDTH: // �����ߵĿ��
      return DoSetLineWidth(hWnd,wParam,lParam);
	case CM_GETLINEWIDTH: // �õ��ߵĿ��
      return DoGetLineWidth(hWnd,wParam,lParam);
	case CM_SETFILLMODE: // �������ģʽ
      return DoSetFillMode(hWnd,wParam,lParam);
	case CM_GETFILLMODE: // �õ����ģʽ
      return DoGetFillMode(hWnd,wParam,lParam);
	case CM_GETCOLOR: // �õ�������ɫ
      return DoGetColor(hWnd,wParam,lParam);
	case CM_SETCOLOR: // ���ÿ�����ɫ
      return DoSetColor(hWnd,wParam,lParam);
	case CM_GETBKCOLOR: // �õ�������ɫ
      return DoGetBkColor(hWnd,wParam,lParam);
	case CM_SETBKCOLOR: // ���ñ�����ɫ
      return DoSetBkColor(hWnd,wParam,lParam);
//	case CM_GETZOOMWINDOW:
//		return DoGetZoomWindow(hWnd,wParam,lParam);
	case CM_GETMODIFYFLAG: // �õ��޸ı�־
		return DoGetModifyFlag(hWnd,wParam,lParam);
	case CM_SETMODIFYFLAG: // �����޸ı�־
		return DoSetModifyFlag(hWnd,wParam,lParam);
	case CM_GETPIXEL: // �õ�ָ�������ɫ
		return DoGetPixel(hWnd,wParam,lParam);
	  default:
    	  return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
//	return 0;
}

// **************************************************
// ������static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����0�����򷵻�-1��
// �������������������ؼ�
// ����: 
// **************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
//  HDC hdc;

      lpCanvas=malloc(sizeof(CANVASPROPERTY)); // ���仭����Ľṹ
      if (lpCanvas==NULL)
         return -1;
      lpCanvas->hMemoryDC=NULL; // �����ڴ�DCΪ��
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

      SetWindowLong(hWnd,0,(DWORD)lpCanvas); // ���û�����Ľṹ
//	  EdbgOutputDebugString( "Create Canvas OK\r\n" );
      return 0;
}

// **************************************************
// ������static LRESULT DoSetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- MAKELONG(cx,cy)  Ҫ�ڻ����Ͻ��д����λͼ�Ŀ�͸�
// 	IN lParam -- HBITMAP Ҫ�ڻ����Ͻ��д����λͼ
// ����ֵ���ɹ�����TRUE;���򷵻�FALSE��
// �������������û��������ԣ�����CM_SETPROPERTY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  HDC hdc;
  RECT rectRgn;
//  HBITMAP hBitmap;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      // ����λͼ��С
      lpCanvas->rectBmp.left=0;
      lpCanvas->rectBmp.top=0;
      lpCanvas->rectBmp.right=(short)LOWORD(wParam);
      lpCanvas->rectBmp.bottom=(short)HIWORD(wParam);

// !!! Add By Jami chen in 2003.09.15
	  rectRgn = lpCanvas->rectBmp;
// !!! Add End By Jami chen in 2003.09.15

      lpCanvas->hBitMap=(HBITMAP)lParam; // �õ�λͼ���
//      if (lpCanvas->hBitMap==NULL)
//         return FALSE;
		
	  lpCanvas->iStartx=0; // ���ÿ�ʼλ��
	  lpCanvas->iStarty=0;

/*
	  if (lpCanvas->hBitMap)	
	  {
		  DeleteObject(lpCanvas->hBitMap);
	  }
*/
      if (lpCanvas->hBitMap!=NULL)
	  { // ��λͼ���
		  hdc=GetDC(hWnd); // �õ�DC

		  if (lpCanvas->hMemoryDC==NULL)
		  {  // �ڴ�λͼ������
			  lpCanvas->hMemoryDC=CreateCompatibleDC(hdc); // �����ڴ�DC
			  lpCanvas->hOldBitMap=SelectObject(lpCanvas->hMemoryDC,lpCanvas->hBitMap); // ��λͼѡ���ڴ�DC
		  }
		  else
		  {
			  SelectObject(lpCanvas->hMemoryDC,lpCanvas->hBitMap);// ��λͼѡ���ڴ�DC
		  }
	//	  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
// !!! delete by jami chen in 2003.09.03
//		  SetShowBitmap(hWnd,lpCanvas);
// !!! delete end by jami chen in 2003.09.03
		  ReleaseDC(hWnd,hdc); // �ͷ�DC
	  }

      SetHScrollPage(hWnd); // ����ˮƽ������
      SetVScrollPage(hWnd); // ���ô�ֱ������
	  GetClientRect(hWnd,&lpCanvas->rectWindow);

// !!! Add By Jami chen in 2003.09.15
	  IntersectRect(&rectRgn,&lpCanvas->rectWindow,&lpCanvas->rectBmp);
	  lpCanvas->hRgn = CreateRectRgn(rectRgn.left,rectRgn.top,rectRgn.right,rectRgn.bottom);
// !!! Add End By Jami chen in 2003.09.15
	  InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
      return TRUE;
}

// **************************************************
// ������static LRESULT DoSetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ����ģʽ
// 	IN lParam -- ����
// ����ֵ����
// �������������û���ģʽ ������CM_SETDRAWMODE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  int iNewMode;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      iNewMode=(int)wParam; // �õ��µĻ�ͼģʽ
      if (iNewMode==lpCanvas->iDrawMode)
        return FALSE; // û�иı�
	  if (lpCanvas->bInputed==TRUE) // ��ǰ������ģʽ
		 WriteCharToScreen(hWnd,lpCanvas); // д��ǰ�ַ�����Ļ

      if (lpCanvas->iDrawMode==SELECTMODE)
      { // ��ǰ��ѡ��ģʽ
//        HDC hdc;
//           hdc=GetDC(hWnd);
//           DrawNewSelectRectangle(hdc,lpCanvas,point,CANCELACTIVE);
//           DrawNewSelectRectangle(hdc,lpCanvas,NULL,REDRAWOFFSET);
//           ReleaseDC(hWnd,hdc);
           SaveSelectRectToMemory(lpCanvas); // ����ѡ������ڵ�ͼ���ڴ�
           // ����ѡ����Ч
           lpCanvas->rectSelect.left=0;
           lpCanvas->rectSelect.top=0;
           lpCanvas->rectSelect.right=0;
           lpCanvas->rectSelect.bottom=0;
           lpCanvas->pointOffset.x=0;
           lpCanvas->pointOffset.y=0;
           lpCanvas->bMoveSelected=FALSE;
		   InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
      }
	  if (iNewMode==INPUTCHARMODE)
	  { // �µĻ���ģʽ�������ַ�ģʽ
//		  if (lpCanvas->hZoom)
		  if (lpCanvas->bZoom)
			  return FALSE; // �ڷŴ�ģʽ�²���ʹ�������ַ�ģʽ
	  }
      lpCanvas->iDrawMode=iNewMode; // �����µĻ���ģʽ
      return TRUE;
}

// **************************************************
// ������static LRESULT DoSetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT �ߵĿ��
// 	IN lParam -- ����
// ����ֵ����
// �������������û��������ߵĿ�ȣ�����CM_SETLINEWIDTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  int iLineWidth;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return 0;
      iLineWidth=(int)wParam; // �õ��߿�
      if (iLineWidth==lpCanvas->iLineWidth)
        return 0; // û�иı�
	  if (lpCanvas->bInputed==TRUE) // ��ǰ������ģʽ��
		 WriteCharToScreen(hWnd,lpCanvas); // ����ǰ���ַ�д�뵽��Ļ

      lpCanvas->iLineWidth=iLineWidth; // �����߿�
      return 0;
}
// **************************************************
// ������static LRESULT DoGetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ�Ļ���ģʽ��
// �����������õ�����ģʽ���� ������CM_GETDRAWMODE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetDrawMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  
      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return -1;
      return lpCanvas->iDrawMode; // ���ػ���ģʽ
}

// **************************************************
// ������static LRESULT DoSetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT Ҫ�������ģʽ�� 
// 	IN lParam -- ����
// ����ֵ����
// �������������û���������ģʽ������CM_SETFILLMODE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  int iFillMode;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return 0;
      iFillMode=(int)wParam; // �õ����ģʽ
      if (iFillMode==lpCanvas->iFillMode)
        return 0;
	  if (lpCanvas->bInputed==TRUE) // ��ǰ����ģʽ�������ַ�
		 WriteCharToScreen(hWnd,lpCanvas); // ����ǰ�ַ�д����Ļ

      lpCanvas->iFillMode=iFillMode; // �������ģʽ
      return 0;
}
// **************************************************
// ������static LRESULT DoGetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ�Ļ���������ģʽ��
// �����������õ�����������ģʽ������CM_GETFILLMODE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetFillMode(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  
      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return -1;
      return lpCanvas->iFillMode; // ���ص�ǰ�����ģʽ
}

// **************************************************
// ������static LRESULT DoGetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ�������ߵĿ�ȡ�
// �����������õ����������ߵĿ�ȣ�����CM_GETLINEWIDTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetLineWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  
      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return -1;
      return lpCanvas->iLineWidth; // ���ص�ǰ���߿�
}

// **************************************************
// ������static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �����������ƻ����ڣ�����WM_DESTORY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
	  if (lpCanvas->hRgn)
	  { // ɾ������
		  DeleteObject(lpCanvas->hRgn);
		  lpCanvas->hRgn = NULL;
	  }
	  if (lpCanvas->bInputed==TRUE)
		 WriteCharToScreen(hWnd,lpCanvas); // д��ǰ�����ַ�����Ļ
      if (lpCanvas->hMemoryDC)
      { // ɾ���ڴ�DC
        SelectObject(lpCanvas->hMemoryDC,lpCanvas->hOldBitMap); // ��ԭ����λͼ�ָ���DC
		DeleteDC(lpCanvas->hMemoryDC); // ɾ��DC
		lpCanvas->hMemoryDC = NULL;// ����DC���Ϊ��
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

      free(lpCanvas);  // �ͷŻ�����ṹ
      return TRUE;
}
// **************************************************
// ������static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ����������ϵͳҪ�ı�ؼ����ڵĴ�С������WM_SIZE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      GetClientRect(hWnd,&lpCanvas->rectWindow); // ���µõ����ھ��
	  return TRUE;
}	
// **************************************************
// ������static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ����������ϵͳ�Ѿ��ı��˴��ڵ�λ�ã�����WM_WINDOWPOSCHANGED��Ϣ��
// ����: 
// **************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
//  HDC hdc;
//  HBITMAP hBitmap;
//  RECT rectWindow;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
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

      SetHScrollPage(hWnd); // ����ˮƽ������
      SetVScrollPage(hWnd); // ���ô�ֱ������
	  GetClientRect(hWnd,&lpCanvas->rectWindow); // �õ����ھ��
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
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(X,Y) ���λ��
// ����ֵ����
// �������������������£�����WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;
  LPCANVASPROPERTY lpCanvas;
  RECT rect;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      // �õ�����λ��
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
	  if (lpCanvas->bZoom==TRUE)
	  { // ת��Ϊ��С�Ժ��λ��
		point.x=point.x/(ZOOMMUL+1);
		point.y=point.y/(ZOOMMUL+1);
	  }
	  SendMouseNotified(hWnd,point,MOUSE_MOVE); // ��������ƶ���Ϣ

//	  RETAILMSG(1,(TEXT("Down Pos =(%d,%d)"),point.x,point.y));
      if (lpCanvas->hBitMap==NULL) // û��λͼ���
         return FALSE;

      if (GetFocus()!=hWnd) // �õ����㴰��
      { // ��ǰ���ڲ��ǽ��㴰��
        SetFocus(hWnd); // ���ý��㴰��
      }
//      if (point.x<0||point.y<0)
//        return 0;
	  SetCapture(hWnd); // ץס���
      lpCanvas->pointDown=point;

	  lpCanvas->pointOld = lpCanvas->pointCurrent;

      lpCanvas->pointCurrent=point;
      if (lpCanvas->iDrawMode==SELECTMODE)
      { // ��ǰ��ѡ��ģʽ
//        rect=lpCanvas->rectSelect;
//        OffsetRect(&rect,lpCanvas->pointOffset.x,lpCanvas->pointOffset.y);
        AdjustRect(&lpCanvas->rectSelect,&rect);
        if (PtInRect(&rect,point))
        {// ��ǰ����ѡ�����
          lpCanvas->bMoveSelected=TRUE; // ��ʼ�ƶ�ѡ���
          return 0;
        }
        else if (lpCanvas->iDrawMode==INPUTCHARMODE)
		{ // ��ǰ�������ַ�ģʽ
			return 0;
		}
		else
        { // ������ģʽ
           SaveSelectRectToMemory(lpCanvas); // ����ѡ����ε����ݵ��ڴ�
		   InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		   // ����ѡ�������Ч
           lpCanvas->rectSelect.left=0;
           lpCanvas->rectSelect.top=0;
           lpCanvas->rectSelect.right=0;
           lpCanvas->rectSelect.bottom=0;

           lpCanvas->pointOffset.x=point.x+lpCanvas->iStartx; // �����±ʵ��λ��
           lpCanvas->pointOffset.y=point.y+lpCanvas->iStarty;
           lpCanvas->bMoveSelected=FALSE; // �����ƶ�ѡ�����״̬
        }
      }
	  else if (lpCanvas->iDrawMode==ZOOMMODE)
	  { // ��ǰ�ǷŴ�ģʽ
//		  CreateZoomWindow(hWnd,lpCanvas,point);
		  SetZoomStates(hWnd,lpCanvas,point); // ���÷Ŵ�ģʽ״̬
		  return 0;
	  }
/*	  else if (lpCanvas->iDrawMode==GETCOLORMODE)
	  {
		  COLORREF crCurPoint;

			crCurPoint=GetPointColor(hWnd,point);
			SendGetColorModeNotified(hWnd,point,crCurPoint);
	  }
*/
      ToPoint(hWnd,point,MOUSE_DOWN); // ����õ�
      return 0;
}
// **************************************************
// ������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(X,Y) ���λ��
// ����ֵ����
// ��������������ƶ�������WM_MOUSEMOVE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
	  // �õ�����λ��
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
	  if (lpCanvas->bZoom==TRUE)
	  { // �ǷŴ�״̬���õ���С������λ��
		point.x=point.x/(ZOOMMUL+1);
		point.y=point.y/(ZOOMMUL+1);
	  }

	  if ((lpCanvas->pointCurrent.x==point.x)&&(lpCanvas->pointCurrent.y==point.y))
	  { // û�иı�λ��
		  return FALSE;
	  }
	  SendMouseNotified(hWnd,point,MOUSE_MOVE); // ���������Ϣ
	  if (GetCapture()!=hWnd) 
         return 0; // û��ץס���
//	  RETAILMSG(1,(TEXT("Move Pos =(%d,%d)"),point.x,point.y));
//      if (point.x<0||point.y<0)
 //       return 0;
	  ToPoint(hWnd,point,MOUSE_MOVE); // ����õ�
      return 0;
}
// **************************************************
// ������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(X,Y) ���λ��
// ����ֵ����
// �������������������𣬴���WM_LBUTTONUP��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  POINT point;

  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
	  // �õ����λ��
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
	  if (lpCanvas->bZoom==TRUE)
	  { // �õ���С��ĵ�
		point.x=point.x/(ZOOMMUL+1);
		point.y=point.y/(ZOOMMUL+1);
	  }
	  SendMouseNotified(hWnd,point,MOUSE_UP); // ���������Ϣ
//	  RETAILMSG(1,(TEXT("Up Pos =(%d,%d)"),point.x,point.y));


	  if (GetCapture()!=hWnd)
         return 0; // û��ץס���
      SetCapture(0); // �ͷ�ץס���
//      if (point.x<0||point.y<0)
//        return 0;
	  if (lpCanvas->iDrawMode==INPUTCHARMODE)
	  { // ��ǰģʽΪ�����ַ�ģʽ
		  if (lpCanvas->bInputed==FALSE) // ��û������
			InputUserChar(hWnd,lpCanvas,point); // ��ʼ�����ַ�
		  else// �Ѿ���ʼ����
			WriteCharToScreen(hWnd,lpCanvas); // ���������ַ�����Ļ
	  }
	  else if (lpCanvas->iDrawMode==ZOOMMODE)
	  { // ��ǰ�ǷŴ�ģʽ
			SendMagnifierModeNotified(hWnd,point); // ����֪ͨ��Ϣ
	  }
	  else if (lpCanvas->iDrawMode==GETCOLORMODE)
	  { // ��ǰ�ǵõ���ɫģʽ
		  COLORREF crCurPoint;

			crCurPoint=GetPointColor(hWnd,point); // �õ����ģʽ
			SendGetColorModeNotified(hWnd,point,crCurPoint); // ���͵õ������ɫ֪ͨ��Ϣ
	  }
	  else
	  {
		  if (lpCanvas->bMoveSelected==TRUE) //�Ѿ���ʼ�ƶ�ѡ�����
			lpCanvas->bMoveSelected=FALSE; // �����ƶ�
		  else
			ToPoint(hWnd,point,MOUSE_UP); // ����õ�
	  }
      return 0;
}
// **************************************************
// ������static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �����������ؼ�ʧȥ���㣬����WM_KILLFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return 0;
      if (lpCanvas->hBitMap==NULL)
         return FALSE;

      if (lpCanvas->iDrawMode==SELECTMODE)
      { // ��ǰΪѡ��ģʽ
        //HDC hdc;
           //hdc=GetDC(hWnd);
          // DrawNewSelectRectangle(hdc,lpCanvas,NULL,REDRAWOFFSET);
           //ReleaseDC(hWnd,hdc);
           SaveSelectRectToMemory(lpCanvas); // ���浱ǰ��ѡ����ε��ڴ�
           // ����ѡ�������Ч
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
// ������static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��������
// 	IN lParam -- ����
// ����ֵ����
// ������������ֱ�������ڣ�����WM_VSCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int iPaneInPage,iCurPane,iPaneNumber;
  LPCANVASPROPERTY lpCanvas;
  int iOldStarty;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      if (lpCanvas->hBitMap==NULL)
         return FALSE;
	  if (lpCanvas->bInputed==TRUE)
		 WriteCharToScreen(hWnd,lpCanvas);

	  if (lpCanvas->bZoom==TRUE)
	  { // ��ǰ�ڷŴ�״̬
		  RECT rectClient;
//		  iPaneInPage=(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top)/ZOOMMUL;
		  GetClientRect(hWnd,&rectClient); // �õ��ͻ�����
		  iPaneInPage=(rectClient.bottom-rectClient.top)/(ZOOMMUL+1); // �õ�ÿҳ�Ĵ�С
		  iPaneNumber=lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top; // �õ��ܵĴ�С
	  }
	  else
	  {
		  iPaneInPage=(lpCanvas->rectWindow.bottom-lpCanvas->rectWindow.top)/WIDTHSTEP; // �õ�ÿһҳ�Ĵ�С
		  iPaneNumber=(lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top)/WIDTHSTEP; // �õ��ܵĴ�С
	  }
      iPaneNumber-=iPaneInPage; // �����������ֵҪ��ȥһ���Ĵ�С
      iCurPane=CN_GetScrollPos(hWnd,SB_VERT); // �õ���������λ��
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
         iOldStarty=lpCanvas->iStarty; // �õ��ɵĿ�ʼλ��
				// vertical scroll window
        if (iCurPane>=iPaneNumber)
        { // �Ѿ��������ֵ
		  if (lpCanvas->bZoom==TRUE)
		  {
			iCurPane=iPaneNumber; // ���õ����ֵ
			lpCanvas->iStarty=iCurPane; // �����µĿ�ʼλ��
		  }
		  else
		  {
			iCurPane=iPaneNumber; // ���õ����ֵ
			lpCanvas->iStarty=lpCanvas->rectBmp.bottom-lpCanvas->rectWindow.bottom; // �����ĵÿ�ʼλ��
		  }
        }
        else if(iCurPane<0)
        { // �Ѿ�������Сֵ
          iCurPane=0; // ���õ�������
          lpCanvas->iStarty=0;
        }
        else
		{
			// �����µĵ�ǰλ��
		  if (lpCanvas->bZoom==TRUE)
		      lpCanvas->iStarty=iCurPane;
		  else
		      lpCanvas->iStarty=iCurPane*WIDTHSTEP;
		}

        if (lpCanvas->iDrawMode==SELECTMODE)
        {// ƫ��ѡ����ε�λ��
          OffsetRect(&lpCanvas->rectSelect,
                     0,
                     iOldStarty-lpCanvas->iStarty);
//          lpCanvas->pointOffset.y+=iOldStarty-lpCanvas->iStarty;
        }
//		RETAILMSG(1,(TEXT("Start y Pos =%d"),lpCanvas->iStarty));

        CN_SetScrollPos(hWnd,SB_VERT,iCurPane,TRUE); // ���ù����������λ��
// !!! delete by jami chen in 2003.09.03
//		RestoreScreen(NULL,lpCanvas,MEMORYTOSHOWDC);
// !!! delete end by jami chen in 2003.09.03
        InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
				return 0;
}
// **************************************************
// ������static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��������
// 	IN lParam -- ����
// ����ֵ����
// ����������ˮƽ�������ڣ�����WM_HSCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int iPaneInPage,iCurPane,iPaneNumber;
  LPCANVASPROPERTY lpCanvas;
  int iOldStartx;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      if (lpCanvas->hBitMap==NULL)
         return FALSE;

	  if (lpCanvas->bInputed==TRUE) // ��ǰ�����ַ�����״̬
		 WriteCharToScreen(hWnd,lpCanvas); // д����ǰ���ַ�д����Ļ

	  if (lpCanvas->bZoom==TRUE)
	  { // �ǷŴ�ģʽ
		  RECT rectClient;
//		  iPaneInPage=(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left)/ZOOMMUL;
		  GetClientRect(hWnd,&rectClient);
		  iPaneInPage=(rectClient.right-rectClient.left)/(ZOOMMUL+1); // �õ�һ���Ĵ�С
		  iPaneNumber=lpCanvas->rectBmp.right-lpCanvas->rectBmp.left; // �õ��ܵĴ�С
	  }
	  else
	  {
		  iPaneInPage=(lpCanvas->rectWindow.right-lpCanvas->rectWindow.left)/WIDTHSTEP; // �õ�һ���Ĵ�С
		  iPaneNumber=(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left)/WIDTHSTEP; // �õ��ܵĴ�С
	  }
      iPaneNumber-=iPaneInPage; // �����������ֵҪ��ȥһ���Ĵ�С
      iCurPane=CN_GetScrollPos(hWnd,SB_HORZ); // �õ������������λ��
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

        iOldStartx=lpCanvas->iStartx; // ����ɵ�λ��

				// vertical scroll window
        if (iCurPane>=iPaneNumber)
        { // �Ѿ��������ֵ
		  if (lpCanvas->bZoom==TRUE)
		  { // �Ŵ�ģʽ
			iCurPane=iPaneNumber; // ���õ����ֵ
			lpCanvas->iStartx=iCurPane; // �����µĿ�ʼλ��
		  }
		  else
		  {
	          iCurPane=iPaneNumber; // ���õ����ֵ
		      lpCanvas->iStartx=lpCanvas->rectBmp.right-lpCanvas->rectWindow.right; // �����µĿ�ʼλ��
		  }
        }
        else if(iCurPane<0)
        { // �Ѿ�������Сֵ
          iCurPane=0; // ���õ���С
          lpCanvas->iStartx=0; // �����µĿ�ʼλ��
        }
        else
		{
			// �����µĿ�ʼλ��
		  if (lpCanvas->bZoom==TRUE)
	          lpCanvas->iStartx=iCurPane;
		  else
	          lpCanvas->iStartx=iCurPane*WIDTHSTEP;
		}

        if (lpCanvas->iDrawMode==SELECTMODE)
        { // ��ѡ��ģʽ��ƫ��ѡ����ε�λ��
          OffsetRect(&lpCanvas->rectSelect,
                     iOldStartx-lpCanvas->iStartx,
                     0);
//          lpCanvas->pointOffset.x+=iOldStartx-lpCanvas->iStartx;
        }

//		RETAILMSG(1,(TEXT("Start x Pos =%d"),lpCanvas->iStartx));

        CN_SetScrollPos(hWnd,SB_HORZ,iCurPane,TRUE); // ���ù����������λ��
// !!! delete by jami chen in 2003.09.03
//		RestoreScreen(NULL,lpCanvas,MEMORYTOSHOWDC);
// !!! delete end by jami chen in 2003.09.03
        InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
				return 0;
}
// **************************************************
// ������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ������COLORREF(R,G,B) ��ǰ�������ǰ����ɫ��
// �����������õ��������ǰ����ɫ������CM_GETCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;

	  return lpCanvas->clColor; // ���ؿ��Ƶ���ɫ
}
// **************************************************
// ������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- COLORREF(R,G,B) Ҫ���õ���ɫ
// 	IN lParam -- ����
// ����ֵ����
// ���������� ���û������ǰ����ɫ ������CM_SETCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;

	  lpCanvas->clColor=(COLORREF)wParam; // �õ�������ɫ

// !!! Add By jami chen in 2003.09.09	  
	  if (lpCanvas->hEdit)
	  { // �༭���ڴ��ڣ����ַ�����ģʽ
		CTLCOLORSTRUCT stCtlColor;
		  stCtlColor.fMask = CLF_TEXTCOLOR;
		  stCtlColor.cl_Text = lpCanvas->clColor;
		  SendMessage(lpCanvas->hEdit, WM_SETCTLCOLOR, NULL, (LPARAM)&stCtlColor); // ���ñ༭������ɫ
	  }
// !!! Add End By jami chen in 2003.09.09	  
	  return TRUE;
}
// **************************************************
// ������static LRESULT DoGetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ������COLORREF(R,G,B) ��ǰ������ı�����ɫ
// �����������õ�������ı�����ɫ������CM_GETBKCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;

	  return lpCanvas->clBKColor; // ���ػ�������ɫ
}
// **************************************************
// ������static LRESULT DoSetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- COLORREF(R,G,B) Ҫ���õ���ɫ
// 	IN lParam -- ����
// ����ֵ����
// �������������û�����ı�����ɫ������CM_SETBKCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetBkColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;

	  lpCanvas->clBKColor=(COLORREF)wParam; // ���û�������ɫ
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
// ������static LRESULT DoGetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�������ǰ���������޸ģ��򷵻�TRUE�����򷵻�FALSE��
// �����������õ���ǰ���޸ı�־������CM_GETMODIFYFLAG��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;

	  return (LRESULT)lpCanvas->bModifyFlag; // ���ص�ǰ���޸ı�־
}
// **************************************************
// ������static LRESULT DoSetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- BOOL Ҫ���õ��޸ı�־
// 	IN lParam -- ����
// ����ֵ����
// �������������û�������޸ı�־������CM_SETMODIFYFLAG��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetModifyFlag(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;

	  lpCanvas->bModifyFlag=(BOOL)wParam; // ���õ�ǰ���޸ı�־
	  return 0;
}

// **************************************************
// ������static LRESULT DoGetPixel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPPOINT ָ��POINT��ָ�룬��Ҫ�õ���ɫ�ĵ������
// ����ֵ��ָ���ĵ����ɫֵ��COLORREF(R,G,B)
// �����������õ�ָ���ĵ����ɫ������CM_GETPIXEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetPixel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCANVASPROPERTY lpCanvas;
  LPPOINT lpPoint;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return -1;
	
	  lpPoint = (LPPOINT)lParam; // �õ����λ��
	  return GetPixel(lpCanvas->hMemoryDC,lpPoint->x,lpPoint->y); // ���ص����ɫ
}

// **************************************************
// ������static void SetVScrollPage(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �������������ô�ֱ������һҳ�Ĵ�С��
// ����: 
// **************************************************
static void SetVScrollPage(HWND hWnd)
{
	int iPaneNumber,iPaneInPage;
	DWORD dwStyle;
    SCROLLINFO ScrollInfo;
    LPCANVASPROPERTY lpCanvas;
	RECT rectClient;

        lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
        if (lpCanvas==NULL)
           return ;

		GetClientRect(hWnd,&rectClient); // �õ����ڿͻ�����
		if (lpCanvas->bZoom==TRUE)
		{ // ��ǰ�ǷŴ�״̬
			iPaneNumber=lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top; // �õ��ܵĴ�С
			iPaneInPage=(rectClient.bottom-rectClient.top)/(ZOOMMUL+1); // �õ�һҳ�Ĵ�С
		}
		else
		{
			iPaneNumber=(lpCanvas->rectBmp.bottom-lpCanvas->rectBmp.top)/WIDTHSTEP; // �õ��ܵĴ�С
			iPaneInPage=(rectClient.bottom-rectClient.top)/WIDTHSTEP; // �õ�һҳ�Ĵ�С
		}
		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (iPaneNumber>iPaneInPage)
		{// need Scroll
			if (!(dwStyle&WS_VSCROLL))
			{// Is Not exist scroll ,must show scroll
				ShowScrollBar(hWnd,SB_VERT,TRUE);
				SetHScrollPage(hWnd);
			}
//			RETAILMSG(1,(TEXT("The y Page =%d"),iPaneInPage));
//			RETAILMSG(1,(TEXT("The y Range =%d"),iPaneNumber-1));

			CN_SetScrollRange(hWnd,SB_VERT,0,iPaneNumber-1,TRUE); // ���ù�������Χ

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =iPaneInPage;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE); // ���ù�����һҳ�Ĵ�С
	        CN_SetScrollPos(hWnd,SB_VERT,0,FALSE); // ���û����λ��
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH); // ������ENABLE
		}
		else
		{ // ����Ҫ������
			if (dwStyle&WS_VSCROLL)
			{// Is exist scroll ,must hide scroll
				ShowScrollBar(hWnd,SB_VERT,FALSE);
				SetHScrollPage(hWnd); // ����ˮƽ������
			}
		}
}
// **************************************************
// ������static void SetHScrollPage(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// ��������������ˮƽ������һҳ�Ĵ�С��
// ����: 
// **************************************************
static void SetHScrollPage(HWND hWnd)
{
	short iPaneNumber,iPaneInPage;
	DWORD dwStyle;
	SCROLLINFO ScrollInfo;
	LPCANVASPROPERTY lpCanvas;
	RECT rectClient;

        lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
        if (lpCanvas==NULL)
           return ;

		GetClientRect(hWnd,&rectClient); // �õ����ڿͻ�����
		if (lpCanvas->bZoom==TRUE)
		{ // �Ŵ�״̬
			iPaneNumber=(short)(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left); // �õ��������ܵĴ�С
			iPaneInPage=(rectClient.right-rectClient.left)/(ZOOMMUL+1); // �õ�һҳ�Ĵ�С
		}
		else
		{ // һ��״̬
			iPaneNumber=(lpCanvas->rectBmp.right-lpCanvas->rectBmp.left)/WIDTHSTEP; // �õ��ܵĴ�С
			iPaneInPage=(rectClient.right-rectClient.left)/WIDTHSTEP; // �õ�һҳ�Ĵ�С
		}

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (iPaneNumber>iPaneInPage)
		{// need Scroll
			if (!(dwStyle&WS_HSCROLL))
			{// Is Not exist scroll ,must show scroll
				ShowScrollBar(hWnd,SB_HORZ,TRUE);
				SetVScrollPage(hWnd); // ���ô�ֱ������
			}

//			RETAILMSG(1,(TEXT("The x Page =%d"),iPaneInPage));
//			RETAILMSG(1,(TEXT("The x Range =%d"),iPaneNumber-1));

			CN_SetScrollRange(hWnd,SB_HORZ,0,iPaneNumber-1,TRUE); // ���ù�������λ��

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =iPaneInPage;
			SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE); // ���ù�����һҳ�Ĵ�С
	        CN_SetScrollPos(hWnd,SB_HORZ,0,FALSE); // ���û����λ��
			EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH); // ������ENABLE
		}
		else
		{// not need scroll
			if (dwStyle&WS_HSCROLL)
			{// Is exist scroll ,must hide scroll
				ShowScrollBar(hWnd,SB_HORZ,FALSE);
				SetVScrollPage(hWnd); // ���ô�ֱ������
			}
		}
}

// **************************************************
// ������static int ToPoint(HWND hWnd,POINT point,UINT iProcess)
// ������
// 	IN hWnd -- ���ھ��
// 	IN point -- ��ǰ�����ڵĵ������
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ���������������ƶ���ָ���ĵ㡣
// ����: 
// **************************************************
static int ToPoint(HWND hWnd,POINT point,UINT iProcess)
{
  LPCANVASPROPERTY lpCanvas;
  HPEN hPen,hMemoryPen,hNewPen;
  HDC hdc;

      lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
      if (lpCanvas==NULL)
         return FALSE;
      if (lpCanvas->hMemoryDC==NULL) 
        return FALSE; // û���ڴ�DC

      if (lpCanvas->iDrawMode==NODRAWMODE)
        return TRUE; // û�л滭ģʽ

      if (lpCanvas->iDrawMode==INPUTCHARMODE)
        return TRUE; // ��ǰ�Ļ滭ģʽ�������ַ�ģʽ

      hdc=GetDC(hWnd);
// !!! Add By Jami chen in 2003.09.15
	  SelectClipRgn(hdc,lpCanvas->hRgn); // ѡ���������
// !!! Add End By Jami chen in 2003.09.15
      // Create pen
//      hNewPen=CreatePen(PS_SOLID,lpCanvas->iLineWidth,CL_BLACK);
      hNewPen=CreatePen(PS_SOLID,lpCanvas->iLineWidth,lpCanvas->clColor); // ����PEN

// !!! modified by jami chen in 2003.09.03
//      hPen=SelectObject(lpCanvas->hShowDC, hNewPen);
      hPen=SelectObject(hdc, hNewPen); // ѡ�񴴽���PEN��DC
// !!! modified end  by jami chen in 2003.09.03
	  hMemoryPen=SelectObject(lpCanvas->hMemoryDC, hNewPen);

      switch(lpCanvas->iDrawMode)
      {
          case PENMODE: // ���ֻ�
          	// ���ڴ�DC�л��߶�
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
                // ����ĻDC�л��߶�
               MoveTo(hdc,lpCanvas->pointCurrent.x,lpCanvas->pointCurrent.y);
               LineTo(hdc,point.x,point.y);

               break;
          case LINEMODE:
               DrawNewLine(hdc,lpCanvas,point,iProcess); // ����һ���µ���
               break;
          case RECTMODE:
               DrawNewRectangle(hdc,hNewPen,lpCanvas,point,iProcess); // ����һ���µľ���
               break;
          case CIRCLEMODE: 
               DrawNewCircle(hdc,lpCanvas,point,iProcess); // ����һ���µ�Բ
               break;
          case RUBBERMODE:
               Rubber(hdc,lpCanvas,point,iProcess); // ��Ƥ��
               break;
          case SELECTMODE: // ѡ��ģʽ
               if (lpCanvas->bMoveSelected==TRUE)
               { // �Ѿ���ʼ�ƶ�ѡ�����
                 MoveSelectRectangle(hdc,lpCanvas,point); // �ƶ����µ�λ��
               }
               else
               {
				   RECT rectOldSelect,rect;
                  
				   AdjustPoint(lpCanvas,&point); // �������λ��

// !!! modified by jami chen in 2003.09.03
//				  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//                DrawNewSelectRectangle(lpCanvas->hShowDC,lpCanvas,&point,iProcess);
//				  RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
//				  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
					rectOldSelect = lpCanvas->rectSelect; // �õ��ɵ�ѡ�����
					InflateRect(&rectOldSelect,1,1);
				    AdjustRect(&rectOldSelect,&rect); // �������ε�λ��
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
					DrawNewSelectRectangle(hdc,lpCanvas,&point,iProcess); // �����µ�ѡ�����
//				  RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
// !!! modified end by jami chen in 2003.09.03
					SetNewSelectRectangle(lpCanvas,point); // �����µ�ѡ�����
               }
               break;
		  case AIRBRUSHMODE: // ������
				AirBrush(hdc,lpCanvas,point,iProcess);
				break;
		  case FILLAREAMODE: // ���ģʽ
				DrawFillArea(hdc,lpCanvas,point,iProcess);
			    break;
          default:
               break;
      }
	  lpCanvas->pointOld = lpCanvas->pointCurrent; // ����ԭ���ĵ�

      lpCanvas->pointCurrent=point; // ������ǰ�ĵ�

	  lpCanvas->bModifyFlag=TRUE; // �����޸ı�־
// !!! modified  by jami chen in 2003.09.03
//      SelectObject(lpCanvas->hShowDC, hPen);
      SelectObject(hdc, hPen); // ѡ��ԭ����PEN��DC
// !!! modified end by jami chen in 2003.09.03
	  SelectObject(lpCanvas->hMemoryDC, hMemoryPen);
       // Delete Rubber pen
      DeleteObject(hNewPen); // ɾ��PEN

      ReleaseDC(hWnd,hdc); // �ͷ�DC
      return TRUE;
}
// **************************************************
// ������static void DrawNewLine(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// �����������»�һ���ߡ�
// ����: 
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

		GetRectFrom2Pt(lpCanvas->pointDown,lpCanvas->pointOld,lpCanvas->iLineWidth,lpCanvas->rectWindow,&rectRedraw); // ����2��õ�����
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // �ָ��ڴ浽��Ļ
// !!! modified end by jami chen in 2003.09.03
	}
   // Draw new Line
    if (iProcess==MOUSE_UP)
    {
//		RECT rectRedraw;
    	// ���ڴ��л��߶�
       MoveTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+lpCanvas->pointDown.x),
              (short)(lpCanvas->iStarty+lpCanvas->pointDown.y));
       LineTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+point.x),
              (short)(lpCanvas->iStarty+point.y));
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ浽��Ļ
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
    // ֱ������Ļ�ϻ��߶�
    MoveTo(hdc,
           lpCanvas->pointDown.x,
           lpCanvas->pointDown.y);
    LineTo(hdc,
           point.x,
           point.y);
// !!! modified end by jami chen in 2003.09.03
}
// **************************************************
// ������static void DrawNewRectangle(HDC hdc,HPEN hNewPen,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN hNewPen -- ָ���ı�
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// ������������ָ���ı��ڵ�ǰλ�û�һ�����Ρ�
// ����: 
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
	{ // �������
        hOldPen=SelectObject(lpCanvas->hMemoryDC,hNewPen);  // ѡ���
        if (lpCanvas->iFillMode==FILL_EMPTY)
            hBrush=SelectObject(lpCanvas->hMemoryDC, GetStockObject(NULL_BRUSH)); // �����
        else if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{ // �ñ���ɫ���
			if (hNewBrush==NULL)
				hNewBrush=CreateSolidBrush(lpCanvas->clBKColor); // ��������ɫˢ��
            hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush); // ѡ��ˢ�ӵ��ڴ�DC
		}
        else
		{ // ��ǰ��ɫ���
			if (hNewBrush==NULL)
				hNewBrush=CreateSolidBrush(lpCanvas->clColor); // ����ǰ��ɫ��ˢ��
            hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush); // ѡ��ˢ�ӵ�DC
		}
		// �����
        Rectangle(lpCanvas->hMemoryDC,
            (short)(lpCanvas->pointDown.x+lpCanvas->iStartx),
            (short)(lpCanvas->pointDown.y+lpCanvas->iStarty),
            (short)(point.x+lpCanvas->iStartx),
            (short)(point.y+lpCanvas->iStarty));
        
        SelectObject(lpCanvas->hMemoryDC, hBrush); // �ָ�ԭ����ˢ��
        // Select Old Pen to TempDC
        hOldPen=SelectObject(lpCanvas->hMemoryDC,hOldPen); // �ָ�ԭ���ı�
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ浽��Ļ
// !!! modified end by jami chen in 2003.09.03
	}
	else
	{
		RECT rectRedraw;
// !!! modified by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
		GetRectFrom2Pt(lpCanvas->pointDown,lpCanvas->pointCurrent,lpCanvas->iLineWidth,lpCanvas->rectWindow,&rectRedraw); // ����2��õ�����
		RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // �ָ��ڴ浽��Ļ
// !!! modified end by jami chen in 2003.09.03

        hOldPen=SelectObject(lpCanvas->hMemoryDC,hNewPen); // ѡ���
		if (lpCanvas->iFillMode==FILL_EMPTY)
// !!! modified by jami chen in 2003.09.03
//			hBrush=SelectObject(lpCanvas->hShowDC, GetStockObject(NULL_BRUSH));
			hBrush=SelectObject(hdc, GetStockObject(NULL_BRUSH)); // ��Ҫ���
// !!! modified by jami chen in 2003.09.03
		else if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{ // �ñ���ɫ���
			hNewBrush=CreateSolidBrush(lpCanvas->clBKColor);
// !!! modified by jami chen in 2003.09.03
//			hBrush=SelectObject(lpCanvas->hShowDC, hNewBrush);
			hBrush=SelectObject(hdc, hNewBrush);
// !!! modified by jami chen in 2003.09.03
		}
		else
		{ // ��ǰ��ɫ���
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
		// ������
		Rectangle(hdc,
				  (short)(lpCanvas->pointDown.x),
				  (short)(lpCanvas->pointDown.y),
				  (short)(point.x),
				  (short)(point.y));

		SelectObject(hdc, hBrush); // �ָ�ˢ��
// !!! modified end by jami chen in 2003.09.03

        hOldPen=SelectObject(lpCanvas->hMemoryDC,hOldPen); // �ָ�ԭ���ı�

// !!! delete by jami chen in 2003.09.03
//		RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
// !!! delete end by jami chen in 2003.09.03
	}
	
	if (hNewBrush)
		DeleteObject(hNewBrush); // ɾ��ˢ��
}

// **************************************************
// ������static void DrawNewCircle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// �����������ڵ�ǰλ�û�һ��Բ��
// ����: 
// **************************************************
static void DrawNewCircle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
   HBRUSH hBrush;
   HBRUSH hNewBrush=NULL;
        if (iProcess==MOUSE_UP)
		{  // ��굯��
			if (lpCanvas->iFillMode==FILL_EMPTY)
				hBrush=SelectObject(lpCanvas->hMemoryDC, GetStockObject(NULL_BRUSH)); // û�����
			else if (lpCanvas->iFillMode==FILL_BACKGROUND)
			{ // �ñ���ɫ���
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clBKColor);
				hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush);
			}
			else
			{ // ��ǰ��ɫ���
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clColor);
				hBrush=SelectObject(lpCanvas->hMemoryDC, hNewBrush);
			}
			// ��Բ
			Ellipse(lpCanvas->hMemoryDC,
              lpCanvas->pointDown.x,
              lpCanvas->pointDown.y,
              point.x,
              point.y);
	
			SelectObject(lpCanvas->hMemoryDC, hBrush); // �ָ�ˢ��
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ浽��Ļ
		}
		else
		{ // ���û�е���
			RECT rectRedraw;

			if (lpCanvas->iFillMode==FILL_EMPTY)
				hBrush=SelectObject(hdc, GetStockObject(NULL_BRUSH)); // û�����
			else if (lpCanvas->iFillMode==FILL_BACKGROUND)
			{ // �ñ���ɫ���
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clBKColor);
				hBrush=SelectObject(hdc, hNewBrush);
			}
			else
			{ // ��ǰ��ɫ���
				if (hNewBrush==NULL)
					hNewBrush=CreateSolidBrush(lpCanvas->clColor);
				hBrush=SelectObject(hdc, hNewBrush);
			}
			// �õ�Ҫˢ�µķ�Χ
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
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // �ָ��ڴ浽��Ļ
			// ��Բ
			Ellipse(hdc,
              (lpCanvas->pointDown.x-lpCanvas->iStartx),
              (lpCanvas->pointDown.y-lpCanvas->iStarty),
              (point.x-lpCanvas->iStartx),
              (point.y-lpCanvas->iStarty));

			SelectObject(hdc, hBrush); // �ָ�ˢ��
		}
	if (hNewBrush)
		DeleteObject(hNewBrush); // ɾ��ˢ��
			  
}
// **************************************************
// ������static void DrawNewCircle1(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// �����������ڵ�ǰλ�û�һ��Բ��������һ�ֻ�Բ�ķ�����
// ����: 
// **************************************************
static void DrawNewCircle1(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
        if (iProcess==MOUSE_UP)
		{ // ��굯��
			// ���ڴ��л�Բ 
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
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ浽��Ļ
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

				// �õ��뾶
				x=lpCanvas->pointDown.x-lpCanvas->pointOld.x;
				y=lpCanvas->pointDown.y-lpCanvas->pointOld.y;
				radiou=(long)x*x+(long)y*y;
			    radiou=Sqrt(radiou);
				
				if (radiou > 0)
				{ // �õ�Ҫ�ָ��ľ��δ�С
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
					RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,&rectRedraw); // �ָ��ڴ浽��Ļ
				}
				// ��Բ
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
// ������static void Rubber(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// ����������������ǰλ�õ�ͼ��
// ����: 
// **************************************************
static void Rubber(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
   HPEN hPen,hNewPen;

       // Create Rubber pen
       hNewPen=CreatePen(PS_SOLID,5,CL_WHITE); // ������Ƥ��

       // Erase Memory
       hPen=SelectObject(lpCanvas->hMemoryDC, hNewPen); // ѡ����Ƥ�ߵ��ڴ�DC
	   SetROP2(lpCanvas->hMemoryDC,R2_COPYPEN); // ����COPY��
	   // �߳�2��֮�������
       MoveTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+lpCanvas->pointCurrent.x),
              (short)(lpCanvas->iStarty+lpCanvas->pointCurrent.y));
       LineTo(lpCanvas->hMemoryDC,
              (short)(lpCanvas->iStartx+point.x),
              (short)(lpCanvas->iStarty+point.y));
       // �ָ�ԭ���ı�
       SelectObject(lpCanvas->hMemoryDC, hPen);

// !!! modified by jami chen in 2003.09.03
//	   RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//	   RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
//	   RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
       // ɾ����Ļ�ϵıʼ�
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
       DeleteObject(hNewPen); // ɾ����Ƥ��
}
// **************************************************
// ������static void DrawNewSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,LPPOINT lppoint,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN lppoint -- ��ǰ�ʵ�����λ�õ�ָ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// �����������ڵ�ǰλ�����»�һ��ѡ����Ρ�
// ����: 
// **************************************************
static void DrawNewSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,LPPOINT lppoint,UINT iProcess)
{
  HBRUSH hBrush;
  HPEN hPen,hNewPen;
//    if (iProcess!=MOUSE_MOVE)
//        return ;

    hNewPen=CreatePen(PS_DOT,1,CL_BLACK); // ����һ�������ߵı�
    hPen=SelectObject(hdc, hNewPen); // ѡ��ʵ�DC
    hBrush=SelectObject(hdc, GetStockObject(NULL_BRUSH));

//	SetROP2(lpCanvas->hShowDC,R2_XORPEN);

    if (iProcess==REDRAWOFFSET)
    { // �ػ�ԭ����ѡ���
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
       { // �����µ�ѡ���
         Rectangle(hdc,
                   lpCanvas->pointDown.x,
                   lpCanvas->pointDown.y,
                   lppoint->x,
                   lppoint->y);
       }
    }
    SelectObject(hdc, hBrush); // �ָ�ˢ��
    SelectObject(hdc, hPen); // �ָ���
    DeleteObject(hNewPen); // ɾ����
}

// **************************************************
// ������static void SetNewSelectRectangle(LPCANVASPROPERTY lpCanvas,POINT point)
// ������
// 	IN lpCanvas -- ������ṹָ��
// 	IN lppoint -- ��ǰ�ʵ�����λ�õ�ָ��
// ����ֵ����
// ���������������µ�ѡ�����
// ����: 
// **************************************************
static void SetNewSelectRectangle(LPCANVASPROPERTY lpCanvas,POINT point)
{
  RECT rect;

   // �����µ�ѡ�����
   lpCanvas->rectSelect.left=lpCanvas->pointDown.x;
   lpCanvas->rectSelect.top=lpCanvas->pointDown.y;
   lpCanvas->rectSelect.right=point.x;
   lpCanvas->rectSelect.bottom=point.y;

   AdjustRect(&lpCanvas->rectSelect,&rect); // ��������
   // �õ�ѡ����ƫ��
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
// ������static void AdjustRect(LPRECT rectSrc,LPRECT rectObj)
// ������
// 	IN rectSrc -- ����ǰ�ľ���
// 	OUT rectObj -- ������ľ���
// ����ֵ����
// �����������������ε�����״̬����(����) -- (�ң���)��
// ����: 
// **************************************************
static void AdjustRect(LPRECT rectSrc,LPRECT rectObj)
{
   // ����ˮƽ����
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
   // ������ֱ����
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
// ������static void  AdjustPoint(LPCANVASPROPERTY lpCanvas,LPPOINT lppoint)
// ������
// 	IN lpCanvas -- ������ṹָ��
// 	IN/OUT lppoint -- ��ǰ�ʵ�����λ�õ�ָ��
// ����ֵ����
// ��������������ǰ�ĵ������λͼ��Χ֮�ڡ�
// ����: 
// ************************************************/
static void  AdjustPoint(LPCANVASPROPERTY lpCanvas,LPPOINT lppoint)
{
 // Adjust the point is or not in the rectBmp , if not then reduce to the new point.
  POINT pointOfBitmap;
	
    // �õ�����λͼ
    pointOfBitmap.x=lppoint->x+lpCanvas->iStartx;
    pointOfBitmap.y=lppoint->y+lpCanvas->iStarty;

    if (pointOfBitmap.x<0)
    { //�㲻��λͼ��
      lppoint->x=0-lpCanvas->iStartx;
    }
    if (pointOfBitmap.x>lpCanvas->rectBmp.right)
    { //�㲻��λͼ��
      lppoint->x=lpCanvas->rectBmp.right-lpCanvas->iStartx;
    }

    if (pointOfBitmap.y<0)
    { //�㲻��λͼ��
      lppoint->y=0-lpCanvas->iStarty;
    }
    if (pointOfBitmap.y>lpCanvas->rectBmp.bottom)
    { //�㲻��λͼ��
      lppoint->y=lpCanvas->rectBmp.bottom-lpCanvas->iStarty;
    }

}

// **************************************************
// ������static void MoveSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// ����ֵ����
// �����������ƶ�ѡ����ε���ǰ��λ�á�
// ����: 
// **************************************************
static void MoveSelectRectangle(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point)
{
//   POINT pointOffset;
   RECT rectOldSelect,rect;
// !!! modified by jami chen in 2003.09.03
//		  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//		  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
			rectOldSelect = lpCanvas->rectSelect; // �õ��ɵ�ѡ�����
//		    OffsetRect(&rectOldSelect,
//				   lpCanvas->pointOld.x-point.x,
//				   lpCanvas->pointOld.y-point.y);

		    AdjustRect(&rectOldSelect,&rect); // ��������
		    InflateRect(&rect,1,1);
		     // ɾ��ԭ���ĺۼ�
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

		  AdjustRect(&lpCanvas->rectSelect,&rect); // ��������
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
			  // �����µ�����
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
        DrawNewSelectRectangle(hdc,lpCanvas,NULL,REDRAWOFFSET); // �����µľ��ο�
// !!! modified end by jami chen in 2003.09.03
}
// **************************************************
// ������static void SaveSelectRectToMemory(LPCANVASPROPERTY lpCanvas)
// ������
// 	IN lpCanvas -- ������ṹָ��
// ����ֵ����
// ��������������ѡ����ε����ݵ��ڴ档
// ����: 
// **************************************************
static void SaveSelectRectToMemory(LPCANVASPROPERTY lpCanvas)
{
   RECT rect;
//   HBITMAP hMemoryMap=NULL;
   //EBITMAP bmpTemp={OBJ_BITMAP,0,1,1,1,1,1,&rect};
      // The bmpTemp is a temp bitmap for get the hmemory bitmap

        AdjustRect(&lpCanvas->rectSelect,&rect); // ��������
        if (IsRectEmpty(&rect))
          return ; // �վ���

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
        	  // ��ѡ�������ݸ��Ƶ���ǰ��λ��
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
// ������static void Circle(HDC hdc,LPCANVASPROPERTY lpCanvas,int x0,int y0,int x1,int y1,BOOL bCopyToMemory)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN x0 -- ԭ��X����
// 	IN y0 -- ԭ��Y����
// 	IN x1 -- Բ�ܵ��X ����
// 	IN y1 -- Բ�ܵ��Y����
// 	IN bCopyToMemory -- �Ƿ�Ҫ�������ڴ�
// ����ֵ����
// ������������Բ��
// ����: 
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
    radiou=(long)x*x+(long)y*y; // �õ��뾶��ƽ��
    if (radiou<=0)
      return;
   

	x0+=lpCanvas->iStartx;
	y0+=lpCanvas->iStarty;

	radiou=(long)Sqrt(radiou); // �õ��뾶
	Delta=(int)(3-2*radiou);
	radiou_bak=radiou;
	Delta_bak=Delta;

//	hBrush=GetStockObject(BLACK_BRUSH);
	hBrush=CreateSolidBrush(lpCanvas->clColor); // ����ǰ��ˢ
	if (lpCanvas->iFillMode!=FILL_EMPTY)
	{ // �����
	   HPEN hPen,hNewPen;
	   int width;

		if (lpCanvas->iFillMode==FILL_BACKGROUND)
		{ // �ñ������
			// Create Fill pen
			hNewPen=CreatePen(PS_SOLID,1,CL_WHITE);

			// Erase Screen
			hPen=SelectObject(hdc, hNewPen);
		}
		else
		{ // ��ǰ�����
			// Create Current pen
			hNewPen=CreatePen(PS_SOLID,1,lpCanvas->clColor);

			// Erase Screen
			hPen=SelectObject(hdc, hNewPen);
		}
		width=lpCanvas->iLineWidth;
		// ֱ�����������Բ
		for(x=0;x<=radiou;)
		{
			// ����
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
			SelectObject(hdc, hPen); // �ָ�ԭ���ı�

			// Delete Fill pen
			DeleteObject(hNewPen); // ɾ����
		}
	}

	if (lpCanvas->iFillMode!=FILL_FOREGROUND)
	{ //  û�����
		radiou=radiou_bak;
		Delta=Delta_bak;
		// ֱ�ӻ�Բ���ϵĵ�
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

	DeleteObject(hBrush); // ɾ��ˢ��
}

// **************************************************
// ������static void DrawPoint(HDC hdc,HBRUSH hBrush,int x,int y,int width)
// ������
// 	IN hdc -- �豸���
// 	IN hBrush -- ��������Ļ�ˢ
// 	IN x -- ���X����
// 	IN y -- ���Y����
// 	IN width -- ��Ŀ��
// ����ֵ����
// �������������㡣
// ����: 
// **************************************************
static void DrawPoint(HDC hdc,HBRUSH hBrush,int x,int y,int width)
{
	RECT rect;
			 // �õ���Ĵ�С
			 rect.left=x-width/2;
			 rect.right=x+(width-width/2);
			 rect.top=y-width/2;
			 rect.bottom=y+(width-width/2);

			 FillRect(hdc,&rect,hBrush); // ��ˢ�����õ�
}
// **************************************************
// ������static void InputUserChar(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT pt)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpCanvas -- ������ṹָ��
// 	IN pt -- ָ���������
// ����ֵ����
// ������������ָ���������ַ���
// ����: 
// **************************************************
static void InputUserChar(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT pt)
{
	HINSTANCE hInst;
	WORD wScreenWidth,wScreenHeight;
	int cx,cy;
	int iLimited;
	CTLCOLORSTRUCT stCtlColor;

	  lpCanvas->pointChar=pt;
	  
	  wScreenWidth=GetSystemMetrics( SM_CXSCREEN );  // �õ���Ļ���
	  wScreenHeight=GetSystemMetrics( SM_CYSCREEN ); // �õ���Ļ�߶�

	  cx = wScreenWidth > lpCanvas->rectBmp.right ? lpCanvas->rectBmp.right : wScreenWidth; // �õ����ڿ�
	  cy = wScreenHeight > lpCanvas->rectBmp.bottom ? lpCanvas->rectBmp.bottom : wScreenHeight; // �õ����ڸ�

	  cx -= (pt.x -1 ); // �õ����ڵ������
	  cy -= (pt.y -1 ); // �õ����ڵ����߶�

	  if (cx > CAN_EDITWIDTH+2)
		  cx = CAN_EDITWIDTH+2; // �õ����ڵ�ʹ�ÿ��

	  if (cy > CAN_EDITHEIGHT+2)
		  cy = CAN_EDITHEIGHT+2; // �õ����ڵ�ʹ�ø߶�

	  hInst=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	  // �����༭��
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
	  SendMessage(lpCanvas->hEdit,EM_SETLIMITTEXT,(WPARAM)iLimited,0); // ���ñ༭����������
	  stCtlColor.fMask = CLF_TEXTCOLOR;
	  stCtlColor.cl_Text = lpCanvas->clColor;
	  SendMessage(lpCanvas->hEdit, WM_SETCTLCOLOR, NULL, (LPARAM)&stCtlColor); // ������ɫ
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
						"����",
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
      lpCanvas->bInputed=TRUE; // ���ÿ�ʼ�����־

	  if (lpCanvas->hEdit)
		  SetFocus(lpCanvas->hEdit); // ���ý�����༭��
}
// **************************************************
// ������static void WriteCharToScreen(HWND hWnd,LPCANVASPROPERTY lpCanvas)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpCanvas -- ������ṹָ��
// ����ֵ����
// �������������ַ��������Ļ�ϡ�
// ����: 
// **************************************************
static void WriteCharToScreen(HWND hWnd,LPCANVASPROPERTY lpCanvas)
{
	LPTSTR lpString;
	DWORD size;
	HDC hdc;
	int OldMode;
	COLORREF oldColor;

	  if (lpCanvas->hEdit)
	  { // ���ڱ༭��
		  size=SendMessage(lpCanvas->hEdit,WM_GETTEXTLENGTH,0,0); // �õ��༭����ı�����
		  lpString=malloc(size+1); // ����ռ俪�����ı�
		  if (lpString)
		  { // ����ɹ�
			  SendMessage(lpCanvas->hEdit,WM_GETTEXT,(LPARAM)(size+1),(LPARAM)lpString); // �õ��ı�
			  // ���ı�д����Ļ
			  hdc=GetDC(hWnd); // �õ�DC
			  if (lpCanvas->iFillMode==FILL_EMPTY)
			  { // û�����
				OldMode=SetBkMode(lpCanvas->hMemoryDC,TRANSPARENT);
			  }
			  oldColor=SetTextColor(lpCanvas->hMemoryDC,lpCanvas->clColor); // ����ǰ��ɫ
			  // ����ı����ڴ�DC
			  TextOut(lpCanvas->hMemoryDC,
	              (short)(lpCanvas->iStartx+lpCanvas->pointChar.x),
		          (short)(lpCanvas->iStarty+lpCanvas->pointChar.y),
				  lpString,
				  size);
			  SetTextColor(lpCanvas->hMemoryDC,oldColor); // �ָ�ԭ������ɫ
			  if (lpCanvas->iFillMode==FILL_EMPTY)
			  {
				SetBkMode(lpCanvas->hMemoryDC,OldMode); // �ָ�ԭ����дģʽ
			  }
// !!! modified by jami chen in 2003.09.03
//			  RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			  RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			  RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ����ݵ���Ļ
// !!! modified end by jami chen in 2003.09.03

			  ReleaseDC(hWnd,hdc); // �ͷ�DC
			  free(lpString); // �ͷŷ���Ŀռ�
		  }
		  DestroyWindow(lpCanvas->hEdit); // �ƻ��༭��
// !!! Add By Jami chen in 2003.09.09
		  lpCanvas->hEdit = NULL; // ����ÿ�
// !!! Add End By Jami chen in 2003.09.09
	  }
	  if (lpCanvas->hKeyboard)
	  {
		  DestroyWindow(lpCanvas->hKeyboard); // ɾ������
	  }
	  lpCanvas->bInputed=FALSE; // �˳��ı�����״̬
}
// **************************************************
// ������static void AirBrushProcess(HDC hdc,POINT point,COLORREF color)
// ������
// 	IN hdc -- �豸���
// 	IN point -- ָ���ĵ�
// 	IN color -- ָ������ɫ
// ����ֵ����
// ������������ָ���ĵ���������������
// ����: 
// **************************************************
static void AirBrushProcess(HDC hdc,POINT point,COLORREF color)
{
	int x,y;

		for(x=point.x-3;x<=point.x+3;x++)
		{
			for(y=point.y-3;y<=point.y+3;y++)
			{ // ��ָ����Χ��ȡ���ֵ�Ƿ�Ҫ���㣬3 -- 4��ȡһ��
				if ((random()%3)==1)
					SetPixel(hdc,x,y,color);
			}
		}
}

// **************************************************
// ������static void AirBrush(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// ������������������
// ����: 
// **************************************************
static void AirBrush(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
               point.x+=lpCanvas->iStartx;
               point.y+=lpCanvas->iStarty;
			   AirBrushProcess(lpCanvas->hMemoryDC,point,lpCanvas->clColor); // ���ڴ�DC�����
// !!! modified by jami chen in 2003.09.03
//			   RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			   RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			   RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL); // �ָ��ڴ浽��Ļ
// !!! modified end by jami chen in 2003.09.03
}

// **************************************************
// ������static void RestoreScreen(HDC hdc,LPCANVASPROPERTY lpCanvas,DWORD dwFlag,LPRECT lprect)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN dwFlag -- Ҫ�ָ�������
// 	IN lprect -- Ҫ�ָ��ľ���
// ����ֵ����
// �������������ڴ��е����ݻָ�����Ļ��
// ����: 
// **************************************************
static void RestoreScreen(HDC hdc,LPCANVASPROPERTY lpCanvas,DWORD dwFlag,LPRECT lprect)
{
	if (lprect == NULL)
		lprect = &lpCanvas->rectWindow;
	if (dwFlag==MEMORYTOSCREENDC)
	{ // �ָ��ڴ浽��Ļ
		if (lpCanvas->bZoom==FALSE)
		{ // û�зŴ�״̬
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
		{ // �Ŵ�״̬
			ZoomBitmapFromShowDCToScreen(hdc,lpCanvas); // ���ڴ�����ݷŴ���Ļ
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
// ������static void ZoomBitmapFromShowDCToScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// ����ֵ����
// �����������Ŵ�λͼ��
// ����: 
// **************************************************
static void ZoomBitmapFromShowDCToScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
{
	RECT /*rcMagnifier,*/rcPoint;
	WORD i,j,iWidth,iHeight,iBmpWidth,iBmpHeight;
	HBRUSH hBrush=NULL;
	COLORREF crColor,crOldColor=-1;;


//		GetClientRect(hWnd,&rcMagnifier);
		// �õ��Ŵ���Ļ����Ҫλͼ�Ŀ����
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
		// ��λͼ�ϵ�ÿһ����Ŵ��һ��  ZOOMMUL * ZOOMMUL �ľ���
		for (i=0;i<=iWidth && i<iBmpWidth;i++)
		{
			for (j=0;j<=iHeight && j<iBmpHeight;j++)
			{
//				RETAILMSG(1,(TEXT("%d,%d"),i+lpMagnifier->x,j+lpMagnifier->y));
// !!! delete by jami chen in 2003.09.03
//				crColor=GetPixel(lpCanvas->hShowDC,i,j);
				crColor=GetPixel(lpCanvas->hMemoryDC,i,j); // �õ������ɫ
// !!! delete end by jami chen in 2003.09.03
				if (crColor==-1)
					crColor=CL_WHITE;
				// �õ��õ�Ŵ��ľ���
				rcPoint.left=i*(ZOOMMUL+1)+1;
				rcPoint.top=j*(ZOOMMUL+1)+1;
				rcPoint.right=rcPoint.left+ZOOMMUL;
				rcPoint.bottom=rcPoint.top+ZOOMMUL;
				if (crOldColor!=crColor)
				{ // ��ǰ�ĵ����ɫ��ԭ���Ĳ�ͬ����Ҫ���´���ˢ��
					if (hBrush)
					{ // ɾ��ԭ����ˢ��
						DeleteObject(hBrush);
						hBrush=NULL;
					}
					hBrush=CreateSolidBrush(crColor); // �����µ�ˢ��
					crOldColor=crColor; // ������ɫ
				}

//				hBrush=CreateSolidBrush(crColor);
				FillRect(hdc,&rcPoint,hBrush); // ���Ƹõ�
//				DeleteObject(hBrush);
			}
		}

		if (hBrush)
		{
			DeleteObject(hBrush); // ɾ��ˢ��
			hBrush=NULL;
		}

		//ReleaseDC(hMainWnd,hMainDC);
}

// **************************************************
// ������static void SaveScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// ����ֵ����
// ����������������Ļ���ݵ��ڴ档
// ����: 
// **************************************************
static void SaveScreen(HDC hdc,LPCANVASPROPERTY lpCanvas)
{
          // Copy a rectangle of same size with Window from the memory to Temp DC
		// ����Ļ�ϵ�����д���ڴ�DC
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
// ������static void SendMouseNotified(HWND hWnd,POINT point,UINT iProcess)
// ������
// 	IN hWnd -- ���ھ��
// 	IN point -- ��ǰ�ĵ������
// 	IN iProcess -- ��ǰ�ıʵ�״̬
// ����ֵ����
// �����������������֪ͨ��Ϣ��
// ����: 
// **************************************************
static void SendMouseNotified(HWND hWnd,POINT point,UINT iProcess)
{
	NMCANVAS nmCanvas;
	HWND hParent;
	LPCANVASPROPERTY lpCanvas;
  
		lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
		if (lpCanvas==NULL)
           return ;
        point.x+=lpCanvas->iStartx;
		point.y+=lpCanvas->iStarty;

		nmCanvas.hdr.hwndFrom=hWnd;
		nmCanvas.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID); // �õ�ID
		nmCanvas.hdr.code=CN_MOUSECHANGED;
		nmCanvas.iMouseStatus=iProcess;
		nmCanvas.iDrawMode=lpCanvas->iDrawMode;
		nmCanvas.ptAction=point;
		hParent=GetParent(hWnd); // �õ�������
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmCanvas); // ����֪ͨ��Ϣ��������
}
// **************************************************
// ������static void SendGetColorModeNotified(HWND hWnd,POINT point,COLORREF crColor)
// ������
// 	IN hWnd -- ���ھ��
// 	IN point -- ��ǰ�ĵ������
// 	IN crColor -- �õ�����ɫ
// ����ֵ����
// �������������͵õ���ɫģʽ��֪ͨ��Ϣ��
// ����: 
// **************************************************
static void SendGetColorModeNotified(HWND hWnd,POINT point,COLORREF crColor)
{
	NMGETCOLOR nmGetColor;
	HWND hParent;
	LPCANVASPROPERTY lpCanvas;
  
		lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
		if (lpCanvas==NULL)
           return ;
        point.x+=lpCanvas->iStartx;
		point.y+=lpCanvas->iStarty;

		nmGetColor.hdr.hwndFrom=hWnd;
		nmGetColor.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmGetColor.hdr.code=CN_GETPOINTCOLOR;
		nmGetColor.crColor=crColor;
		nmGetColor.ptAction=point;
		hParent=GetParent(hWnd); // �õ�������
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmGetColor); // ����֪ͨ��Ϣ��������
}

// **************************************************
// ������static void SendMagnifierModeNotified(HWND hWnd,POINT point)
// ������
// 	IN hWnd -- ���ھ��
// 	IN point -- ��ǰ�ĵ������
// ����ֵ����
// �������������ͷŴ�λͼ��֪ͨ��Ϣ��
// ����: 
// **************************************************
static void SendMagnifierModeNotified(HWND hWnd,POINT point)
{
	NMMAGNIFIER nmMagnifier;
	HWND hParent;
	LPCANVASPROPERTY lpCanvas;
  
		lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0); // �õ�������Ľṹ
		if (lpCanvas==NULL)
           return ;
        point.x+=lpCanvas->iStartx;
		point.y+=lpCanvas->iStarty;

		nmMagnifier.hdr.hwndFrom=hWnd;
		nmMagnifier.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmMagnifier.hdr.code=CN_MAGNIFIER;
		nmMagnifier.ptAction=point;
		hParent=GetParent(hWnd); // �õ�������
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmMagnifier); // ������Ϣ��������
}

// **************************************************
// ������static LONG Sqrt(LONG Value)
// ������
// 	IN Value -- Ҫ�����ŵ�ֵ
// ����ֵ�����ؿ����ź��ֵ
// ���������������š�
// ����: 
// **************************************************
static LONG Sqrt(LONG Value)
{
	LONG newV=0;

	if (Value<0)
		return 0;
	while(1)
	{
		if (Value<newV*newV)
			return (newV-1); // �õ���ǰֵ�����ź������ֵ
		newV++;
	}
}
// **************************************************
// ������static COLORREF GetPointColor(HWND hWnd,POINT point)
// ������
// 	IN hWnd -- ���ھ��
// 	IN point -- ָ���ĵ������
// ����ֵ��COLORREF(R,G,B), ���صõ�����ɫֵ��
// �����������õ�ָ������ɫ��ֵ��
// ����: 
// **************************************************
static COLORREF GetPointColor(HWND hWnd,POINT point)
{
//	HDC hdc;
	COLORREF crColor;
	LPCANVASPROPERTY lpCanvas;

	lpCanvas=(LPCANVASPROPERTY)GetWindowLong(hWnd,0);  // �õ�������Ľṹ
	if (lpCanvas==NULL)
       return -1;

//	hdc=GetDC(hWnd);
//	crColor=GetPixel(hdc,point.x,point.y);
//	ReleaseDC(hWnd,hdc);
	// �õ������ɫ
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
// ������static LRESULT SetZoomStates(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ĵ������
// ����ֵ����
// �������������÷Ŵ�״̬��
// ����: 
// **************************************************
static LRESULT SetZoomStates(HWND hWnd,LPCANVASPROPERTY lpCanvas,POINT point)
{ 
	if (lpCanvas->bZoom==FALSE)
	{ // ԭ��û�зŴ���������Ҫ�Ŵ�
		lpCanvas->bZoom=TRUE; // ���õ�ǰ�Ŵ�
	}
	else
	{ // ԭ���Ѿ��Ŵ�������Ҫ�ָ�ԭ��
		lpCanvas->iStartx=0;
		lpCanvas->iStarty=0;
		lpCanvas->bZoom=FALSE;
	}
//	SetShowBitmap(hWnd,lpCanvas);
    SetHScrollPage(hWnd); // ����ˮƽ������
    SetVScrollPage(hWnd); // ���ô�ֱ������
	InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
	return 0;
}



// **********************************************************************/
// This is magnifier window �Ŵ󴰿�
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
// ������ATOM RegisterMagnifierClass( HINSTANCE hInst )
// ������
// 	IN hInst -- ʵ�����
// ����ֵ��ע�᷵��ֵ
// ����������ע��Ŵ󴰿��ࡣ
// ����: 
// **************************************************
ATOM RegisterMagnifierClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = MagnifierWndProc; // �Ŵ󴰿ڵĹ��̺���
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;//LoadIcon(NULL, IDI_APPLICATION );
    wc.hCursor = 0;//LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = 0;
    wc.lpszClassName = classMagnifier; // �Ŵ󴰿�����

    return (BOOL)(RegisterClass( &wc )); // ע����
}
// **************************************************
// ������LRESULT CALLBACK MagnifierWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN uMsg -- Ҫ�������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// ����ֵ����Ϣ������
// �����������������Ŵ󴰿�����Ϣ
// ����: 
// **************************************************
LRESULT CALLBACK MagnifierWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_PAINT: // ���ƷŴ󴰿�
			hdc = BeginPaint(hWnd, &ps);
		    DoMagnifierPaint( hWnd ,hdc);
			EndPaint(hWnd, &ps);
		    return 0;
	  case WM_ERASEBKGND: // ɾ������
		    return DoMagnifierEraseBKGnd(hWnd,wParam,lParam);
   	  case WM_LBUTTONDOWN: // ����������
			return DoMagnifierLButtonDown(hWnd,wParam,lParam);
   	  case WM_MOUSEMOVE: // ����ƶ�
			return DoMagnifierMouseMove(hWnd,wParam,lParam);
   	  case WM_LBUTTONUP: // ����������
			return DoMagnifierLButtonUp(hWnd,wParam,lParam);
	  case WM_VSCROLL: // ��ֱ������Ļ
	 	    return DoMagnifierVScrollWindow(hWnd,wParam,lParam);
	  case WM_HSCROLL: // ˮƽ������Ļ
		    return DoMagnifierHScrollWindow(hWnd,wParam,lParam);
	  case WM_CREATE: // �����Ŵ󴰿�
		    DoMagnifierCreate(hWnd,wParam,lParam);
		    return 0;
       case WM_DESTROY: // �ƻ��Ŵ󴰿�
			ReleaseMagnifierWindow(hWnd);
			break;

	  case MM_ASSOCIATE: // �������Ŵ��DC�봰�ھ��
			return DoAssociate(hWnd,wParam,lParam);
	  default:
    	    return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

// **************************************************
// ������static LRESULT DoMagnifierCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����0�����򷵻�-1
// �������������������Ŵ󴰿ڣ�����WM_CREATE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	LPRECT lprectMain;
	LPCREATESTRUCT lpcs;
		
		lpcs = (LPCREATESTRUCT) lParam; // �õ���������

	    lprectMain=lpcs->lpCreateParams; // �õ������ڵĴ�С
		lpMagnifier=(LPMAGNIFIER)malloc(sizeof(MAGNIFIER)); // ����Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return -1;
		// ��ʼ���ṹ
		lpMagnifier->hAssociate=NULL;
		lpMagnifier->hAssociateDC=NULL;
		lpMagnifier->x=0;
		lpMagnifier->y=0;
		if (lprectMain!=NULL)
			lpMagnifier->rect=*lprectMain; // ���ô��ھ���
		else
//			GetWindowRect(GetParent(hWnd),&lpMagnifier->rect);
			GetWindowRect(lpcs->hParent,&lpMagnifier->rect);

		SetWindowLong(hWnd,0,(DWORD)lpMagnifier); // �������ݽṹ������
		return 0;
}
// **************************************************
// ������static LRESULT DoMagnifierEraseBKGnd(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- hdc �豸���
// 	IN lParam -- ����
// ����ֵ����
// ����������ɾ�������Ŵ󴰿ڱ���������WM_ERASEBKGND��Ϣ��
// ����: 
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

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return 0;
		hdc=(HDC)wParam;

		GetClientRect(hWnd,&rcClient); // �õ����ڿͻ�����С
		
		// �õ����ԷŴ��λͼ��С
		iWidth=(rcClient.right-rcClient.left)/(ZOOMMUL+1);  // +1 is must a edge
		iHeight=(rcClient.bottom-rcClient.top)/(ZOOMMUL+1);  // +1 is must a edge
		
		if (iWidth>=(lpMagnifier->rect.right-lpMagnifier->rect.left))
		{ // ���ԷŴ��λͼ����ʵ�ʵ�λͼ
			iWidth=(lpMagnifier->rect.right-lpMagnifier->rect.left-1); // �õ�ʵ�ʵ�λͼ��С
			rcClient.right=rcClient.left+iWidth*(ZOOMMUL+1); // �������ÿͻ����δ�С
		}

		if (iHeight>=(lpMagnifier->rect.bottom-lpMagnifier->rect.top))
		{ // ���ԷŴ��λͼ����ʵ��λͼ
			iHeight=lpMagnifier->rect.bottom-lpMagnifier->rect.top-1; // �ﵽʵ��λͼ��С
			rcClient.bottom=rcClient.top+iHeight*(ZOOMMUL+1); // �������ÿͻ����δ�С
		}
		
		hNewPen=CreatePen(PS_DASH,1,CL_LIGHTGRAY); // �õ�����ߵı�
		hNewPen=SelectObject(hdc,hNewPen); // ѡ��ʵ�DC

		// ����ֱ�����
		for (i=0;i<=iWidth;i++)
		{
               MoveTo(hdc,
                      i*(ZOOMMUL+1),
                      rcClient.top);
               LineTo(hdc,
                      i*(ZOOMMUL+1),
                      rcClient.bottom);
		}
		// ��ˮƽ�����
		for (i=0;i<=iHeight;i++)
		{
               MoveTo(hdc,
					  rcClient.left,
                      i*(ZOOMMUL+1));
               LineTo(hdc,
					  rcClient.right,
                      i*(ZOOMMUL+1));
		}
		hNewPen=SelectObject(hdc,hNewPen); // �ָ�ԭ���ı�
		DeleteObject(hNewPen); // ɾ����
		return 0;
}
// **************************************************
// ������static LRESULT DoMagnifierPaint( HWND hWnd ,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam --  ��
// 	IN lParam -- ��
// ����ֵ����
// �������������ƻ����Ŵ󴰿���Ϣ������WM_PAINT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierPaint( HWND hWnd ,HDC hdc)
{
	LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL) // û�й������
			return 0;
		DrawMagnifier(hWnd,hdc,lpMagnifier); // ���ƷŴ󴰿�
		return 0;
}
// **************************************************
// ������static LRESULT DoMagnifierLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y) �������
// ����ֵ����
// ���������������Ŵ󴰿ڴ���WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	SHORT x,y;
//	HWND hParent;
    
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL)
			return 0;
		// �õ���С����λ�ã���Ϊʵ�ʴ���Ҫ��ԭ����λͼ�ϴ���Ȼ�󽫴�����λͼ�Ŵ�
		x=x/(ZOOMMUL+1)+lpMagnifier->x;
		y=y/(ZOOMMUL+1)+lpMagnifier->y;

//		hParent=GetParent(hWnd);
		SendMessage(lpMagnifier->hAssociate,WM_LBUTTONDOWN,wParam,MAKELPARAM(x,y)); // ֪ͨ�������ڴ�������������
//		if (SendMessage(hParent,CM_GETZOOMWINDOW,0,0)!=NULL)
		if (SendMessage(lpMagnifier->hAssociate,CM_GETZOOMWINDOW,0,0)!=NULL) // ��Ч����
		{
			if (GetCapture()!=hWnd)
				SetCapture(hWnd);
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoMagnifierMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y) �������
// ����ֵ����
// ���������������Ŵ󴰿ڴ���WM_MOUSEMOVE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	SHORT x,y;
    
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL)
			return 0;
		if (GetCapture()!=hWnd)
			return 0;
		// �õ�ʵ��λ��
		x=x/(ZOOMMUL+1)+lpMagnifier->x;
		y=y/(ZOOMMUL+1)+lpMagnifier->y;

		SetCapture(lpMagnifier->hAssociate);
		SendMessage(lpMagnifier->hAssociate,WM_MOUSEMOVE,wParam,MAKELPARAM(x,y)); // ������Ϣ֪ͨ�������ڴ�������ƶ�
		SetCapture(hWnd);
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		return 0;
}
// **************************************************
// ������static LRESULT DoMagnifierLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y) �������
// ����ֵ����
// ���������������Ŵ󴰿ڴ���WM_LBUTTONUP��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;
	SHORT x,y;
    
		x=LOWORD(lParam);
		y=HIWORD(lParam);
		

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return 0;
		if (lpMagnifier->hAssociate==NULL)
			return 0;
		if (GetCapture()!=hWnd)
			return 0;
		// �õ�ʵ��λ��
		x=x/(ZOOMMUL+1)+lpMagnifier->x;
		y=y/(ZOOMMUL+1)+lpMagnifier->y;

		SetCapture(lpMagnifier->hAssociate);
		SendMessage(lpMagnifier->hAssociate,WM_LBUTTONUP,wParam,MAKELPARAM(x,y)); // ֪ͨ�������ڴ�������������
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		return 0;
}

// **************************************************
// ������static LRESULT DoAssociate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- hdc �����豸���
// 	IN lParam -- hWnd �������ھ��
// ����ֵ����
// �������������ù������ڣ�����MM_ASSOCIATE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoAssociate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
			return 0;
		
		lpMagnifier->hAssociate=(HWND)lParam; // ���ù�������
		lpMagnifier->hAssociateDC=(HDC)wParam; // ���ù���DC
		//lpMagnifier->x = LOWORD(wParam);
		//lpMagnifier->y = HIWORD(wParam);
		SetMagnifierHScrollPage(hWnd); // ����ˮƽ������
		SetMagnifierVScrollPage(hWnd); // ���ô�ֱ������
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		return 0;
}


// **************************************************
// ������static void DrawMagnifier(HWND hWnd,HDC hdc,LPMAGNIFIER lpMagnifier)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpMagnifier -- �����Ŵ󴰿ڽṹ
// ����ֵ����
// �������������ƻ����Ŵ󴰿ڡ�
// ����: 
// **************************************************
static void DrawMagnifier(HWND hWnd,HDC hdc,LPMAGNIFIER lpMagnifier)
{
	HWND hMainWnd;
//	HDC hMainDC;
	RECT rcMagnifier,rcPoint;
	WORD i,j,iWidth,iHeight,iBmpWidth,iBmpHeight;
	HBRUSH hBrush;
	COLORREF crColor;

		hMainWnd=lpMagnifier->hAssociate; // �õ��������ھ��
		//hMainDC=GetDC(hMainWnd);

		GetClientRect(hWnd,&rcMagnifier);
		
		// �õ���Ҫ�Ŵ��λͼ��С
		iWidth=(rcMagnifier.right-rcMagnifier.left)/(ZOOMMUL+1);  // +1 is must a edge
		iHeight=(rcMagnifier.bottom-rcMagnifier.top)/(ZOOMMUL+1);  // +1 is must a edge

		// �õ�λͼ��ʵ�ʴ�С
		iBmpWidth=(WORD)(lpMagnifier->rect.right-lpMagnifier->rect.left);
		iBmpHeight=(WORD)(lpMagnifier->rect.bottom-lpMagnifier->rect.top);

//		RETAILMSG(1,(TEXT("The bmp width =%d"),iBmpWidth));
//		RETAILMSG(1,(TEXT("The bmp height =%d"),iBmpHeight));
//		RETAILMSG(1,(TEXT("x Pos =%d"),lpMagnifier->x));
//		RETAILMSG(1,(TEXT("y Pos =%d"),lpMagnifier->y));
		// ����ÿһ����
		for (i=0;i<=iWidth && i<iBmpWidth;i++)
		{
			for (j=0;j<=iHeight && j<iBmpHeight;j++)
			{
//				RETAILMSG(1,(TEXT("%d,%d"),i+lpMagnifier->x,j+lpMagnifier->y));
				crColor=GetPixel(lpMagnifier->hAssociateDC,i+lpMagnifier->x,j+lpMagnifier->y); // �õ������ɫ
				if (crColor==-1)
					crColor=CL_WHITE;
				// �õ��õ�Ĵ�С��λ��
				rcPoint.left=i*(ZOOMMUL+1)+1;
				rcPoint.top=j*(ZOOMMUL+1)+1;
				rcPoint.right=rcPoint.left+ZOOMMUL;
				rcPoint.bottom=rcPoint.top+ZOOMMUL;
				// ���Ƹõ�
				hBrush=CreateSolidBrush(crColor);
				FillRect(hdc,&rcPoint,hBrush);
				DeleteObject(hBrush);
			}
		}
		//ReleaseDC(hMainWnd,hMainDC);
}

// **************************************************
// ������static void SetMagnifierVScrollPage(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �������������û����Ŵ󴰿ڴ�ֱ����ҳ
// ����: 
// **************************************************
static void SetMagnifierVScrollPage(HWND hWnd)
{
	short iPaneNumber,iPaneInPage;
	DWORD dwStyle;
    SCROLLINFO ScrollInfo;
	LPMAGNIFIER lpMagnifier;
	RECT rcMagnifier;//,rcMainWnd;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (dwStyle&WS_VSCROLL)
		{
			lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
			if (lpMagnifier==NULL)
			   return ;
			
			GetClientRect(hWnd,&rcMagnifier); // �õ��ͻ�����
			//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

			iPaneInPage=(rcMagnifier.bottom-rcMagnifier.top)/(ZOOMMUL+1); // �õ�һҳ�Ĵ�С
//			iPaneNumber=(rcMainWnd.bottom-rcMainWnd.top)-iPaneInPage;
//			iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-iPaneInPage;
			iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-1; // �õ��ܵĴ�С

			if (iPaneNumber<0)
			{ // ���ع�����
				ShowScrollBar(hWnd,SB_VERT,FALSE);
				return;
			}
			else
			{ // ��ʾ������
				ShowScrollBar(hWnd,SB_VERT,TRUE);
			}

			RETAILMSG(1,(TEXT("The y Page =%d"),iPaneInPage));
			RETAILMSG(1,(TEXT("The y Range =%d"),iPaneNumber));

			CN_SetScrollRange(hWnd,SB_VERT,0,iPaneNumber,TRUE); // ���ù������ķ�Χ

			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =iPaneInPage;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE); // ���ù�����
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH); // ������ENABLE
		}
}
// **************************************************
// ������static void SetMagnifierHScrollPage(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �������������û����Ŵ󴰿�ˮƽ����ҳ
// ����: 
// **************************************************
static void SetMagnifierHScrollPage(HWND hWnd)
{
	short iPaneNumber,iPaneInPage;
	DWORD dwStyle;
	SCROLLINFO ScrollInfo;
	LPMAGNIFIER lpMagnifier;
	RECT rcMagnifier;//,rcMainWnd;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (dwStyle&WS_HSCROLL)
		{

				lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
				if (lpMagnifier==NULL)
				   return ;
				GetClientRect(hWnd,&rcMagnifier); // �õ����ڿͻ�����
				//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

				iPaneInPage=(rcMagnifier.right-rcMagnifier.left)/(ZOOMMUL+1); // �õ�һҳ�Ĵ�С
//				iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-iPaneInPage;
				iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-1; // �õ��ܵĴ�С

				if (iPaneNumber<0)
				{ // ���ع�����
					ShowScrollBar(hWnd,SB_HORZ,FALSE);
					return;
				}
				else
				{ // ��ʾ������
					ShowScrollBar(hWnd,SB_HORZ,TRUE);
				}
				RETAILMSG(1,(TEXT("The x Page =%d"),iPaneInPage));
				RETAILMSG(1,(TEXT("The x Range =%d"),iPaneNumber));

				CN_SetScrollRange(hWnd,SB_HORZ,0,iPaneNumber,TRUE); // ���ù�������Χ

				ScrollInfo.cbSize=sizeof(SCROLLINFO);
				ScrollInfo.fMask=SIF_PAGE;
			  // set vertical scroll page
				ScrollInfo.nPage =iPaneInPage;
				SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE); // ���ù�����һҳ�Ĵ�С
		        EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH); // ������ENABLE
		}
}
// **************************************************
// ������static LRESULT DoMagnifierVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��������
// 	IN lParam -- ����
// ����ֵ����
// ���������������Ŵ󴰿ڴ�ֱ�������ڣ�����WM_VSCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iPaneInPage,iCurPane,iPaneNumber;
	LPMAGNIFIER lpMagnifier;
	RECT rcMagnifier;//,rcMainWnd;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
		   return 0;
		
		GetClientRect(hWnd,&rcMagnifier); // �õ����ڿͻ�����
		//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

		iPaneInPage=(rcMagnifier.bottom-rcMagnifier.top)/(ZOOMMUL+1); // �õ�һҳ�Ĵ�С
//		iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-iPaneInPage;
		iPaneNumber=(lpMagnifier->rect.bottom-lpMagnifier->rect.top)-1; // �õ��ܵĴ�С

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
        CN_SetScrollPos(hWnd,SB_VERT,iCurPane,TRUE); // ���ù����������λ��
        InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		return 0;
}
// **************************************************
// ������static LRESULT DoMagnifierHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��������
// 	IN lParam -- ����
// ����ֵ����
// ���������������Ŵ󴰿ڴ�ֱ�������ڣ�����WM_HSCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMagnifierHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int iPaneInPage,iCurPane,iPaneNumber;
  LPMAGNIFIER lpMagnifier;
  RECT rcMagnifier;//,rcMainWnd;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
		   return 0;
		GetClientRect(hWnd,&rcMagnifier); // �õ��ͻ�����
		//GetClientRect(lpMagnifier->hAssociate,&rcMainWnd);

		iPaneInPage=(rcMagnifier.right-rcMagnifier.left)/(ZOOMMUL+1); // �õ�һҳ�Ĵ���
//		iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-iPaneInPage;
		iPaneNumber=(lpMagnifier->rect.right-lpMagnifier->rect.left)-1; // �õ��ܵĴ�С
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
			iCurPane=0; // ������ǰ��
		else if((iCurPane+iPaneInPage)>iPaneNumber)
			iCurPane=iPaneNumber-iPaneInPage+1; // ���������

		RETAILMSG(1,(TEXT("The x Pos =%d"),iCurPane));
		if (lpMagnifier->x==iCurPane)
			return 0; // û�иı�
		lpMagnifier->x=iCurPane;
        CN_SetScrollPos(hWnd,SB_HORZ,iCurPane,TRUE); // ���õ�ǰλ��
        InvalidateRect(hWnd,NULL,TRUE);
		return 0;
}
// **************************************************
// ������static LRESULT ReleaseMagnifierWindow(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �����������ͷŻ����Ŵ󴰿�,����WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static LRESULT ReleaseMagnifierWindow(HWND hWnd)
{
  LPMAGNIFIER lpMagnifier;

		lpMagnifier=(LPMAGNIFIER)GetWindowLong(hWnd,0); // �õ��Ŵ󴰿����ݽṹ
		if (lpMagnifier==NULL)
		   return FALSE;
		free(lpMagnifier); // �ͷ����ݽṹ
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
// ������static DrawFillArea(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
// ������
// 	IN hdc -- �豸���
// 	IN lpCanvas -- ������ṹָ��
// 	IN point -- ��ǰ�ʵ�����λ��
// 	IN iProcess -- ��ǰ�ʵ�״̬
// ����ֵ����
// �����������������
// ����: 
// **************************************************
static DrawFillArea(HDC hdc,LPCANVASPROPERTY lpCanvas,POINT point,UINT iProcess)
{
	FILLSTRUCT FillStruct;

	    if (iProcess==MOUSE_UP)
		{ // ��굯��
			// �������ṹ
			FillStruct.bStart = TRUE;
			FillStruct.clFillColor = lpCanvas->clColor;
			FillStruct.clForeColor = GetPixel(hdc,point.x,point.y);
			FillStruct.hFillDC = lpCanvas->hMemoryDC;
			FillStruct.nWidth = lpCanvas->rectBmp.right - lpCanvas->rectBmp.left;
			FillStruct.nHeight = lpCanvas->rectBmp.bottom - lpCanvas->rectBmp.top;
			LineFillArea((short)point.x,(short)point.y,&FillStruct); // �������
// !!! modified  by jami chen in 2003.09.03
//			RestoreScreen(hdc,lpCanvas,MEMORYTOSHOWDC);
//			RestoreScreen(hdc,lpCanvas,SHOWDCTOSCREENDC);
			RestoreScreen(hdc,lpCanvas,MEMORYTOSCREENDC,NULL);
// !!! modified end by jami chen in 2003.09.03
		}
}

// **************************************************
// ������void LineFillArea(short x, short y, PFILLSTRUCT pFillStruct)
// ������
// 	IN x -- ��ʼ�����X����
// 	IN y -- ��ʼ�����Y����
// 	IN pFillStruct -- ���ṹ
// ����ֵ����
// ������������䡣
// ����: 
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
	{ // ��ǰ�ǵ�һ��
		SetPixel(pFillStruct->hFillDC, x, y, pFillStruct->clFillColor); // �õ������ɫ
		if (GetPixel(pFillStruct->hFillDC, x, y) == pFillStruct->clForeColor) // ��������ɫ��ͬ���������
			return ;
		SetPixel(pFillStruct->hFillDC, x, y, pFillStruct->clForeColor); // ���øõ�
		pFillStruct->bStart = FALSE; // ���õ�һ���Ѿ����
	}
	// �������
	while (nLeft >= 0 && GetPixel(pFillStruct->hFillDC, nLeft, y) == pFillStruct->clForeColor)
		SetPixel (pFillStruct->hFillDC, nLeft --, y, pFillStruct->clFillColor);
	// �������
	while (nRight < pFillStruct->nWidth && GetPixel(pFillStruct->hFillDC, nRight, y) == pFillStruct->clForeColor)
		SetPixel (pFillStruct->hFillDC, nRight ++, y, pFillStruct->clFillColor);
		
	nLeft ++;
	nRight --;
	if (nLeft > nRight)
		nLeft = nRight = x;
	// �������
	nTempX = nRight;
	while(nTempX >= nLeft && y - 1 >= 0)
	{
		if (GetPixel(pFillStruct->hFillDC, nTempX, y - 1) == pFillStruct->clForeColor)
		{
			LineFillArea(nTempX, (short)(y - 1), pFillStruct);
		}
		nTempX --;
	}
	// �������
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
// ������static	int CN_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nBar -- Ҫ�ı�λ�õĹ�����
// 	IN nPos -- Ҫ���õĵ�ǰλ��
// 	IN bRedraw -- �Ƿ�Ҫ�ػ�
// ����ֵ����
// �������������ù�������λ�á�
// ����: 
// **************************************************
static	int CN_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			si.nPos=nPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // ���ù������Ļ���λ��
			return 0;
}
// **************************************************
// ������static int CN_GetScrollPos(HWND hWnd, int nBar)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nBar -- Ҫ�õ�λ�õĹ�����
// ����ֵ�����ع�������λ�á�
// �����������õ���������λ�á�
// ����: 
// **************************************************
static int CN_GetScrollPos(HWND hWnd, int nBar)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			GetScrollInfo(hWnd,nBar,&si); // �õ��������Ļ���λ��
			return si.nPos;
}

// **************************************************
// ������static BOOL CN_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
// ������
// 	IN hWnd -- ���ھ��
// 	IN nBar -- Ҫ���ù�������
// 	IN nMinPos -- ����������Сֵ
// 	IN nMaxPos -- �����������ֵ
// 	IN bRedraw -- Ҫ��Ҫ�ػ�
// ����ֵ����
// �������������ù������ķ�Χ��
// ����: 
// **************************************************
static BOOL CN_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_RANGE;
			si.nMin=nMinPos;
			si.nMax=nMaxPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // ���ù������ķ�Χ
			return TRUE;
}

// **************************************************
// ������static void GetRectFrom2Pt(POINT pt1,POINT pt2,int iLineWidth,RECT rectWindow,LPRECT lprect)
// ������
// 	IN pt1 -- ��1 
// 	IN pt2 -- ��2
// 	IN iLineWidth -- �ߵĿ��
// 	IN rectWindow -- ���ھ���
// 	OUT lprect  --�õ����µľ���
// ����ֵ����
// �����������õ�2����֮��ľ��Ρ�
// ����: 
// **************************************************
static void GetRectFrom2Pt(POINT pt1,POINT pt2,int iLineWidth,RECT rectWindow,LPRECT lprect)
{
	// �õ�ˮƽ����ĵ�
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
		
		// �õ���ֱ����ĵ�
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
		// �������������ȷ��
		if (lprect->left < 0 )
			lprect->left = 0; // ���ܳ������ڵ�����
		if (lprect->right > rectWindow.right)
			lprect->right = rectWindow.right; // ���ܳ������ڵ�����
		if (lprect->top < 0 )
			lprect->top = 0; // ���ܳ������ڵ�����
		if (lprect->bottom > rectWindow.bottom)
			lprect->bottom = rectWindow.bottom; // ���ܳ������ڵ�����
}
