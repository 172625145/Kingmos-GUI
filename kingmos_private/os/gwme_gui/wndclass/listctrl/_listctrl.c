/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵�����б�ؼ�
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-12-30
���ߣ�jami chen �½���
�޸ļ�¼��
**************************************************/
//#include "StdAfx.h"
#include "ewindows.h"
//#include "Resource.h"
//#include <memory.h>
//#include <wingdi.h>
#include "elstctrl.h"
#include "eptrlist.h"
#include <eassert.h>

//#define _MAPPOINTER

#define USE_ICON_IMAGE
//#define USE_BITMAP_IMAGE

//#define NOSEPARATE  // Can't drag Column

static const char classLISTCTRL[]="ListView";
#define LISTCTRL classLISTCTRL
#define classLISTVIEW classLISTCTRL

#define SORTASCENDING  0 // default
#define SORTDESCENDING 1

//#define MULTIPLESEL  0  // default
//#define SINGLESEL    1

#define DEFAULTSUBITEMNUM 3
#define DEFAULTITEMNUM    16

//#define CAPTIONHEIGHT   20
//#define USETEXTHEIGHT	20
//#define CAPTIONHEIGHT   28
#define IMAGESPACE		2

#define BLANKSPACE					0
#define CAPTIONITEM         1
#define CAPTIONSEPARATE			2
#define LISTITEM						3
#define LISTBLANKSPACE      4

#define NORMALCOLOR         0
#define INVERTCOLOR         1

#define      ICON_NUMINLINE 3
#define    REPORT_NUMINLINE 1
#define SMALLICON_NUMINLINE 2
#define      LIST_NUMINLINE 1


#define SORTASCENDING  0
#define SORTDESCENDING 1

#define FIELDMAXLEN 64

#define WIDTHSTEP  16
#define ICONWIDTH  40

#define DEFAULTWIDTH 64

#define DEFAULTHORZSPACING 5
#define DEFAULTVERTSPACING 5

#define _PLAN_ONE_
//#define _PLAN_TWO_

struct SubItemStruct {
		LPTSTR lpSubItemCaption;  // The caption of this SubItem
		int iImage;
		UINT CaptionFmt;// The Style of this SubItem ,it may be LVIF_TEXT of LVIF_IMAGE 
		int iSubItemWidth;  // The Width of this sub item 
		PTRLIST plSubItemContent;
		UINT ContentFmt; 
};

typedef struct SubItemStruct SUBITEM;
typedef struct SubItemStruct * LPSUBITEM;


//#define LVM_MOUSEMOVE  0x2000    // The Private Message For itself
/*
struct SortListItemStruct{
	int iItemNo;
	BOOL bSelected;
};

typedef struct SortListItemStruct SORTLISTITEM;
typedef struct SortListItemStruct * LPSORTLISTITEM;
*/

typedef LRESULT (CALLBACK *DefWndProc )(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
struct PDAListCtrlStruct {
				int iMaxItemNum;  // can put maximize item number of current array 
				int iItemNum;    // Exist the item number in current array
				int iMaxSubItemNum; // can set maximize subItem number of current array
				int iSubItemNum; // Exist the subitem number in current array
				LPSUBITEM lpSubItem;  // the content of the subitem
				int iSortSubItem; // if need sort ,then sort by the subItem
//				BYTE bSortOrder; // be sorted base order
//				BOOL bSelectState;// may Select single line or multiple Line
//				BOOL bSort;      // Sort ?
				int cx;   // the start position of this view
				int iActivePos;
				int iActiveItem;
				int iFocusItem;
				int iStartItem;
				int iPenCoordinate;
				int iNewWidth;
				int iDownRow;
				int iDownColumn;
				int iRow;
				int iColumn;
				int iWidth;
				int iHorzSpacing;
				int iVertSpacing;
				int *lpColumnOrderArray;
				LPPTRLIST lpImageList;
				SIZE sizeIcon;
				LPPTRLIST lpImageListSmall;
//				LPSORTLISTITEM lpSortList;
				SIZE sizeSmallIcon;
				BOOL  bPressed;
				BOOL  bActiveItem;
				LPARAM lParam;
				WPARAM wParam;

				COLORREF cl_NormalText;
				COLORREF cl_NormalBkColor;

				COLORREF cl_InvertText;
				COLORREF cl_InvertBkColor;

				COLORREF cl_HeaderText;
				COLORREF cl_HeaderBkColor;
				
				DefWndProc DefaultEditProc;
				int iEditItem;
				HWND hEdit;

				int iLineHeight; // add By Jami in 2004.05.21 
				int iTextHeight; // add By Jami in 2004.05.21 
				int iTextWidth; // add By Jami in 2004.05.21 

};

typedef struct PDAListCtrlStruct PDALISTCTRL;
typedef struct PDAListCtrlStruct * LPPDALISTCTRL;

struct MainItemStruct{
				LPTSTR lpText;
				int iImage;
				UINT State;
				LPARAM lParam;
};

typedef struct MainItemStruct MAINITEM;
typedef struct MainItemStruct * LPMAINITEM;

typedef int (CALLBACK *CompareFunc)(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);


/*********************************************************************************************************/
// Used extern functiom
/*********************************************************************************************************/
ATOM RegisterPDAListCtrlClass(HINSTANCE hInstance);
LRESULT CALLBACK PDAListCtrlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
/*********************************************************************************************************/
// Used intramural functiom
/*********************************************************************************************************/
static void DrawListCtrlCaption(HWND hWnd,HDC hdc);
static void DrawListCtrl(HWND hWnd,HDC hdc);
static void ClearClientArea(HWND hWnd,HDC hdc);
static BOOL ExistCaption(HWND hWnd);
static void DrawCaptionItem(HWND hWnd,HDC hdc,int iCol,BOOL bRaised);
static int PositionInWindow(HWND hWnd,POINTS points,int *CurItem);
static void DrawLineSeparate(HWND hWnd,HDC hdc,int iSubItem);
static int GetPageItemNum(HWND hWnd);
static void DrawReportMode(HWND hWnd,HDC hdc);
static void DrawReportItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine);
static int GetLineHeight(HWND hWnd);
static void SetPDAListColor(HWND hWnd,HDC hdc,int nColorStyle);
static int GetWindowWidth(HWND hWnd);
static int GetWindowHeight(HWND hWnd);
static void ClearLine(HWND hWnd,HDC hdc,int iDrawLine);
static void ClearRect(HWND hWnd,HDC hdc,RECT rect);
static int MakeShortString(HDC hdc, int nColumnLen,LPCTSTR lpLongString,LPTSTR lpShortString);
static int GetTextExtent(HDC hdc,LPCTSTR lpszString, int nCount);
static void DrawListMode(HWND hWnd,HDC hdc);
static void DrawListItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine);
static void InvalidateItem(HWND hWnd,int iCurItem);
static void DrawItem(HWND hWnd,int iCurItem);
#ifdef USE_BITMAP_IMAGE
static void DrawBitmap(HDC hdc,HBITMAP hBitMap,int x,int y,int iWidth,int iHeight,BOOL bInvert);
static void GetBitMapSize(HBITMAP hBitMap,LPSIZE lpSize);
#endif
static void DrawIconMode(HWND hWnd,HDC hdc);
static void DrawIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition);
static void DrawSmallIconMode(HWND hWnd,HDC hdc);
static void DrawSmallIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition);
static void SetVScrollBar(HWND hWnd);
static void SetVScrollPage(HWND hWnd);
static void SetHScrollBar(HWND hWnd);
static void SetHScrollPage(HWND hWnd);
static int GetPageLine(HWND hWnd);
static int GetTotalLine(HWND hWnd);
static int GetItemNumInLine(HWND hWnd);
static void VScrollWindow(HWND hWnd,int nScrollLine);
static void HScrollWindow(HWND hWnd,int nScrollLine);
static void ReleaseListCtrl(HWND hWnd);
static void SendDrawItemMessage(HWND hWnd,HDC hdc,int iCurItem,RECT rcItem);
static int AutoSortItem(LPPDALISTCTRL lpPDAListCtrl,LPMAINITEM lpInsertItem,int order);
static void FreeItem(LPMAINITEM lpMainItem,BOOL bText);
static void RemoveItem(LPPDALISTCTRL lpPDAListCtrl,int iItem);
//static void DrawFocus(HWND hWnd,HDC hdc,int iItem,BOOL bStatus);
static void GetPointPosition(HWND hWnd,POINTS points,int *iRow,int *iColumn);
// !!! Add By Jami chen 2002.04.25
static int GetSubItemPointPosition(HWND hWnd,POINTS points);
// !!! Add End By Jami chen 2002.04.25
static void SetInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn);
static void ClearInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn);
static void SetInvertColumn(HWND hWnd,int iRow,int iStartColumn,int iEndColumn);
static void ClearInvertColumn(HWND hWnd,int iRow,int iStartColumn,int iEndColumn);
static void ClearAllInvert(HWND hWnd);
static void ClearFocus(HWND hWnd);
static int GetListFocus(HWND hWnd);
static void GetDispInfo(HWND hWnd,LPLVITEM lpItem);
static void DrawHorzListLine(HWND hWnd,HDC hdc);
static void DrawVertListLine(HWND hWnd,HDC hdc);
static BOOL MoveInvertLine(HWND hWnd,int iMoveLine);
static BOOL MoveInvertColumn(HWND hWnd,int iMoveLine);

static void SendActiveItem(HWND hWnd,int iItem);
static void SendActiveNotify(HWND hWnd,UINT code,int iItem,POINT pt);
//static  void SendNormalNotify(HWND hWnd,UINT iCode,int iItem,int iSubItem,POINT pt);
static  void SendNormalNotify(HWND hWnd,UINT iCode,int iItem,int iSubItem,UINT uNewState,UINT uOldState,POINT pt);
static  void SendBasicNotify(HWND hWnd,UINT iCode);
static void DoReturnKey(HWND hWnd);

#ifdef _PLAN_ONE_
static void SwapItem(LPPDALISTCTRL lpPDAListCtrl,int iItem1,int iItem2);
#endif
#ifdef _PLAN_TWO_
static int InsertPosition(LPPTRLIST lpPtrList,LPMAINITEM lpMainItem,CompareFunc Compare,LPARAM lParamSort);
#endif

//static WORD GetItemString(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl,WORD iIndex,WORD iSubItem,LPTSTR lpFileFeild);
static int GetItemString(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl,int iIndex,int iSubItem,LPTSTR lpFileFeild);

static void KillEditControl(HWND hWnd);
static void CreateLabelEdit(HWND hWnd,int iItem ,LPTSTR lpString,LPRECT lprect);

static int LV_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw);
static int LV_GetScrollPos(HWND hWnd, int nBar);
static BOOL LV_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw );


static int GetTextHeight(HWND hWnd);
static int GetTextWidth(HWND hWnd);
static int GetCaptionHeight(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl);
/*********************************************************************************************************/
// Message Process function 
/*********************************************************************************************************/

static LRESULT DoCreateListCtrl(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam, LPARAM lParam);
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInsertColumn(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoApproximateViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteColumn(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColumnWidth(HWND hWnd ,WPARAM wParam ,LPARAM lParam);
static LRESULT DoSetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetIconSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEnsureVisible(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFindItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColumnWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCountPerPage(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetImageList(HWND hWnd , WPARAM wParam , LPARAM lParam);
static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemCount(HWND hWnd,WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetNextItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoGetItemPosition(HWND hWnd ,WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemState(HWND hWnd ,WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetOrigin(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelectedCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetStringWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSubItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoRedrawItems(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemState(HWND hWnd ,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSubItemHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSysTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam);
/*********************************************************************************************************/

/**************************************************
������ATOM RegisterPDAListCtrlClass(HINSTANCE hInstance)
������
	IN hInstance -- ʵ�����
����ֵ������ע����
����������ע���б��ࡣ
����: 
************************************************/
ATOM RegisterPDAListCtrlClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	// size of the struct 'WNDCLASSEX' 
//	wcex.cbSize = sizeof(WNDCLASSEX);

	// the class propertiy 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	// this class window procedure
	wcex.lpfnWndProc	= (WNDPROC)PDAListCtrlProc;
	// no class extra
	wcex.cbClsExtra		= 0;
	// window extra 4 byte to save address
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	// no icon
	wcex.hIcon			= NULL;
	// I-Beam Cursor
	wcex.hCursor		= LoadCursor(NULL,IDC_ARROW);
	// background color
	wcex.hbrBackground	= 0;
	// no menu
	wcex.lpszMenuName	= NULL; // no menu ,add to future
	// class name
	wcex.lpszClassName	= LISTCTRL;
	// no small icon
//	wcex.hIconSm		= NULL;

	// register this class
	return RegisterClass(&wcex);
}

/**************************************************
������LRESULT CALLBACK PDAListCtrlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN uMsg -- Ҫ�������Ϣ
	IN wParam -- ��Ϣ����
	IN lParam -- ��Ϣ����
����ֵ����Ϣ������
���������������б�����Ϣ��
����: 
************************************************/
LRESULT CALLBACK PDAListCtrlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_CREATE:  // �����б�ؼ�
			DoCreateListCtrl(hWnd,wParam,lParam);
			break;
		case WM_COMMAND:
			break;
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
   	case WM_LBUTTONDOWN: // ����������
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE: // ����ƶ�
			return DoMouseMove(hWnd,wParam,lParam);
//	case LVM_MOUSEMOVE:
	case WM_SYSTIMER: // ����ϵͳʱ��
			return DoSysTimer(hWnd,wParam,lParam);
//			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP: // ����������
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_SETFOCUS: // ���ý���
			SendBasicNotify(hWnd,NM_SETFOCUS);
			break;
    case WM_KILLFOCUS: // ɱ������
			return DoKillFocus(hWnd,wParam,lParam);
		case WM_CHAR: // �����ַ�����
			break;
		case WM_KEYDOWN: // �����������
			return DoKeyDown(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ�����
			// close this window
			ReleaseListCtrl(hWnd);
//			PostQuitMessage(0);
			break;
    case WM_VSCROLL: // ��ֱ����
      DoVScrollWindow(hWnd,wParam,lParam);
      break;
    case WM_HSCROLL:  // ˮƽ����
      DoHScrollWindow(hWnd,wParam,lParam);
      break;
		case WM_CLEAR:  // ���
			return DoClear(hWnd,wParam,lParam);
		case WM_STYLECHANGED:  // ���ı�
			{
				DoStyleChanged(hWnd,wParam,lParam);
				DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_SETCTLCOLOR	: // ���ÿ�����ɫ
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // �õ�������ɫ
			return DoGetColor(hWnd,wParam,lParam);

/************************************************/
		case LVM_INSERTITEM:  // ����һ����Ŀ
		  return DoInsertItem(hWnd,wParam,lParam);
		case LVM_INSERTCOLUMN: // ����һ��
			return DoInsertColumn(hWnd,wParam,lParam);
		case LVM_SETITEM: // ������Ŀ
			return DoSetItem(hWnd,wParam,lParam);
		case LVM_SETIMAGELIST:  // ����ͼ���б�
			return DoSetImageList(hWnd,wParam,lParam);
		case LVM_SORTITEMS: // ��Ŀ����
			return DoSortItems(hWnd,wParam,lParam);
		case LVM_APPROXIMATEVIEWRECT: // �õ�ָ����������Ŀ����Ŀ�͸�
			return DoApproximateViewRect(hWnd,wParam,lParam);
//		case LVM_ARRANGE:
//			break;
//		case LVM_CREATEDRAGIMAGE :
//			break;
		case LVM_DELETEALLITEMS : // ɾ�����е���Ŀ
			return DoDeleteAllItems(hWnd,wParam,lParam);
		case LVM_DELETECOLUMN : // ɾ��һ��
			return DoDeleteColumn(hWnd,wParam,lParam);
		case LVM_DELETEITEM : // ɾ��ָ����Ŀ
			return DoDeleteItem(hWnd,wParam,lParam);
		case LVM_EDITLABEL : // �༭��ǩ
			return DoEditLabel(hWnd,wParam,lParam);
		case LVM_ENSUREVISIBLE : // ��ָ֤����Ŀ�ɼ�
			return DoEnsureVisible(hWnd,wParam,lParam);
		case LVM_FINDITEM : // ����ָ������Ŀ
			return DoFindItem(hWnd,wParam,lParam);
//		case LVM_GETBKCOLOR :
//			break;
//		case LVM_GETBKIMAGE :
//			break;
//		case LVM_GETCALLBACKMASK :
//			break;
		case LVM_GETCOLUMN : // �õ��е���Ϣ
			return DoGetColumn(hWnd,wParam,lParam);
		case LVM_GETCOLUMNORDERARRAY : // �õ��е���ʾ����
			return DoGetColumnOrderArray(hWnd,wParam,lParam);
		case LVM_GETCOLUMNWIDTH : // �õ�ָ���еĿ��
			return DoGetColumnWidth(hWnd,wParam,lParam);
		case LVM_GETCOUNTPERPAGE : // �õ�ÿһҳ����Ŀ����
			return DoGetCountPerPage(hWnd,wParam,lParam);
//		case LVM_GETEDITCONTROL :
//			break;
//		case LVM_GETEXTENDEDLISTVIEWSTYLE :
//			break;
//		case LVM_GETHEADER :
//			break;
//		case LVM_GETHOTCURSOR :
//			break;
		case LVM_GETHOTITEM  : // �õ�������Ŀ������
			return DoGetHotItem(hWnd,wParam,lParam);
//		case LVM_GETHOVERTIME :
//			break;
		case LVM_GETIMAGELIST : // �õ�ָ�����͵�ͼ���б���
			DoGetImageList(hWnd ,wParam ,lParam);
//		case LVM_GETISEARCHSTRING :
//			break;
		case LVM_GETITEM : // �õ�ָ����Ŀ����
			return DoGetItem(hWnd,wParam,lParam);
		case LVM_GETITEMCOUNT : // �õ���ǰ�ؼ�����Ŀ����
			return DoGetItemCount(hWnd,wParam ,lParam);
//		case LVM_GETITEMPOSITION :
//			return DoGetItemPosition(hWnd ,wParam ,lParam);
		case LVM_GETITEMRECT : // �õ�ָ����Ŀ�ľ��δ�С
			return DoGetItemRect(hWnd,wParam,lParam);
		case LVM_GETITEMSPACING :  // �õ���Ŀ��ļ��
			return DoGetItemSpacing(hWnd,wParam,lParam);
		case LVM_GETITEMSTATE : // �õ�ָ����Ŀ��״̬
			return DoGetItemState(hWnd ,wParam ,lParam);
		case LVM_GETITEMTEXT : // �õ�ָ����Ŀ���ı�
			return DoGetItemText(hWnd,wParam,lParam);
		case LVM_GETNEXTITEM : // ������һ����Ŀ
			return DoGetNextItem(hWnd,wParam,lParam);
//		case LVM_GETNUMBEROFWORKAREAS :
//		break;
		case LVM_GETORIGIN : // �õ��ؼ�ԭ�������
			return DoGetOrigin(hWnd,wParam,lParam);
		case LVM_GETSELECTEDCOUNT : // �õ���ǰ�Ѿ�ѡ�����Ŀ�ĸ���
			return DoGetSelectedCount(hWnd,wParam,lParam);
		case LVM_GETSELECTIONMARK : // �õ�ѡ����Ŀ������
			return DoGetSelectionMark(hWnd,wParam,lParam);
		case LVM_GETSTRINGWIDTH : // �õ�ָ���ַ����Ŀ��
			return DoGetStringWidth(hWnd,wParam,lParam);
		case LVM_GETSUBITEMRECT : // �õ�ָ������Ŀ�ľ��δ�С
			return DoGetSubItemRect(hWnd,wParam,lParam);
//	case LVM_GETTEXTBKCOLOR :
//	break;
//		case LVM_GETTEXTCOLOR :
//			break;
//		case LVM_GETTOOLTIPS :
//			break;
		case LVM_GETTOPINDEX : // �õ���һ���ɼ���Ŀ������
			return DoGetTopIndex(hWnd,wParam,lParam);
//		case LVM_GETUNICODEFORMAT :
//			break;
		case LVM_GETVIEWRECT : // �õ��ؼ�����ͼ��С
			return DoGetViewRect(hWnd,wParam,lParam);
//		case LVM_GETWORKAREAS :
//			break;
		case LVM_HITTEST : // ���Ե��λ��
			return DoHitTest(hWnd,wParam,lParam);
		case LVM_REDRAWITEMS : // �ػ�ָ������Ŀ
			return DoRedrawItems(hWnd,wParam,lParam);
		case LVM_SCROLL : // ��������
			return DoScroll(hWnd,wParam,lParam);
//		case LVM_SETBKCOLOR :
//			break;
//		case LVM_SETBKIMAGE :
//			break;
//		case LVM_SETCALLBACKMASK :
//			break;
		case LVM_SETCOLUMN : // ����ָ������
			return DoSetColumn(hWnd,wParam,lParam);
		case LVM_SETCOLUMNORDERARRAY : // �����е���ʾ����
			return DoSetColumnOrderArray(hWnd,wParam,lParam);
		case LVM_SETCOLUMNWIDTH : // ����ָ���еĿ��
			return DoSetColumnWidth(hWnd ,wParam ,lParam);
//		case LVM_SETEXTENDEDLISTVIEWSTYLE :
//			break;
//		case LVM_SETHOTCURSOR :
//			break;
		case LVM_SETHOTITEM  : // ���ý�������
			return DoSetHotItem(hWnd,wParam,lParam);
//		case LVM_SETHOVERTIME :
//			break;
		case LVM_SETICONSPACING : // ����ͼ��֮��ļ��
			return DoSetIconSpacing(hWnd,wParam,lParam);
		case LVM_SETITEMCOUNT : // ���ÿؼ�����Ŀ����
			return DoSetItemCount(hWnd,wParam,lParam);
//		case LVM_SETITEMPOSITION :
//			break;
//		case LVM_SETITEMPOSITION32 :
//			break;
		case LVM_SETITEMSTATE :  // ������Ŀ״̬
			return DoSetItemState(hWnd,wParam,lParam);
		case LVM_SETITEMTEXT : // ������Ŀ�ı�
			return DoSetItemText(hWnd,wParam,lParam);
	  case LVM_SETSELECTIONMARK : // ����ָ����ĿΪѡ����Ŀ
			return DoSetSelectionMark(hWnd,wParam,lParam);
//		case LVM_SETTEXTBKCOLOR :
//			break;
//		case LVM_SETTEXTCOLOR :
//			break;
//		case LVM_SETTOOLTIPS :
//			break;
//		case LVM_SETUNICODEFORMAT :
			//break;
//		case LVM_SETWORKAREAS :
//			break;
		case LVM_SUBITEMHITTEST : // ���Ե����ڵ������λ��
			return DoSubItemHitTest(hWnd,wParam,lParam);
//		case LVM_UPDATE :
//			break;
    default:
      {
			// default procedure
			LRESULT lResult;
		    lResult=DefWindowProc(hWnd, message, wParam, lParam);
			return lResult;
      }
   }
   return 0;
}

/**************************************************
������static LRESULT DoCreateListCtrl(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
���������������б�ؼ�
����: 
************************************************/
static LRESULT DoCreateListCtrl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		

 			lpPDAListCtrl=(LPPDALISTCTRL)malloc(sizeof(PDALISTCTRL));  // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			lpPDAListCtrl->iMaxSubItemNum=DEFAULTSUBITEMNUM;
			lpPDAListCtrl->iSubItemNum=0;
			lpPDAListCtrl->lpSubItem=(LPSUBITEM)malloc(sizeof(SUBITEM)*lpPDAListCtrl->iMaxSubItemNum);  // ��������Ŀ�б�
			if (lpPDAListCtrl->lpSubItem==NULL)
			{
					free(lpPDAListCtrl);
					lpPDAListCtrl->iMaxSubItemNum=0;
					return 0;
			}
			memset(lpPDAListCtrl->lpSubItem,0,(sizeof(SUBITEM)*lpPDAListCtrl->iMaxSubItemNum));
			lpPDAListCtrl->lpColumnOrderArray=(int *)malloc(sizeof(int)*lpPDAListCtrl->iMaxSubItemNum); // �����д����б�
			if (lpPDAListCtrl->lpColumnOrderArray==NULL)
			{
					free(lpPDAListCtrl->lpSubItem);
					free(lpPDAListCtrl);
					lpPDAListCtrl->iMaxSubItemNum=0;
					return 0;
			}
//			lpPDAListCtrl->bSort=FALSE;
//			lpPDAListCtrl->bSortOrder=SORTASCENDING;
//			lpPDAListCtrl->bSelectState=MULTIPLESEL;
			// ��ʼ������
			lpPDAListCtrl->iMaxItemNum=0;
			lpPDAListCtrl->iItemNum=0;
			lpPDAListCtrl->iSortSubItem=0;
			lpPDAListCtrl->cx=0;
			lpPDAListCtrl->iActivePos=BLANKSPACE;
			lpPDAListCtrl->iActiveItem=-1;
			lpPDAListCtrl->iFocusItem=-1;
			lpPDAListCtrl->iStartItem=0;
			lpPDAListCtrl->iPenCoordinate=0;
			lpPDAListCtrl->iNewWidth=0;
			lpPDAListCtrl->bPressed=FALSE;
			lpPDAListCtrl->bActiveItem=FALSE;
			lpPDAListCtrl->iHorzSpacing=DEFAULTHORZSPACING;
			lpPDAListCtrl->iVertSpacing=DEFAULTVERTSPACING;
			lpPDAListCtrl->lpImageList = NULL;
			lpPDAListCtrl->lpImageListSmall= NULL;

			lpPDAListCtrl->iWidth=0;
			
			// ��ʼ����ɫ
/*
			lpPDAListCtrl->cl_NormalText = CL_BLACK;
			lpPDAListCtrl->cl_NormalBkColor = CL_WHITE;

			lpPDAListCtrl->cl_InvertText = CL_WHITE;
			lpPDAListCtrl->cl_InvertBkColor = CL_BLACK;
*/
			lpPDAListCtrl->cl_NormalText = GetSysColor(COLOR_WINDOWTEXT);
			lpPDAListCtrl->cl_NormalBkColor = GetSysColor(COLOR_WINDOW);

			lpPDAListCtrl->cl_InvertText = GetSysColor(COLOR_HIGHLIGHTTEXT);
			lpPDAListCtrl->cl_InvertBkColor = GetSysColor(COLOR_HIGHLIGHT);

//			lpPDAListCtrl->cl_NormalText = CL_BLUE;
//			lpPDAListCtrl->cl_NormalBkColor = CL_YELLOW;

//			lpPDAListCtrl->cl_InvertText = CL_RED;
//			lpPDAListCtrl->cl_InvertBkColor = CL_BLUE;
			lpPDAListCtrl->cl_HeaderText = lpPDAListCtrl->cl_NormalText;
			lpPDAListCtrl->cl_HeaderBkColor = lpPDAListCtrl->cl_NormalBkColor;

			lpPDAListCtrl->DefaultEditProc = NULL;
			lpPDAListCtrl->iEditItem = -1;
			lpPDAListCtrl->hEdit = NULL;

			lpPDAListCtrl->iLineHeight = 0; // Add By Jami chen in 2004.05.21			
			lpPDAListCtrl->sizeSmallIcon.cx=16;  // Add By Jami chen in 2004.05.22 for initial
			lpPDAListCtrl->sizeSmallIcon.cy=16;
			lpPDAListCtrl->sizeIcon.cx=32;
			lpPDAListCtrl->sizeIcon.cy=32;

			SetWindowLong(hWnd,0,(DWORD)lpPDAListCtrl); // ���ýṹ������
			SetVScrollBar(hWnd); // ���ù�����

			lpPDAListCtrl->iLineHeight = GetLineHeight(hWnd); // Add By Jami chen in 2004.05.21
			lpPDAListCtrl->iTextHeight = GetTextHeight(hWnd); // Add By Jami chen in 2004.05.21
			lpPDAListCtrl->iTextWidth = GetTextWidth(hWnd); // Add By Jami chen in 2004.05.21

			return 0;
}
/**************************************************
������static void ReleaseListCtrl(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
�����������ͷ��б�ؼ�
����: 
************************************************/
static void ReleaseListCtrl(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		int i;
		BOOL bText=TRUE;

		KillEditControl(hWnd); // ɱ���༭�ؼ�
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			if (lpPDAListCtrl->lpSubItem)
			{ // ɾ����������Ŀ
				for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
				{ // ɾ����
					if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
					{
						if(!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_IMAGE))
							PtrListFreeAll(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
						PtrListDestroy(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
					}
					if (lpPDAListCtrl->lpSubItem[i].lpSubItemCaption)
						free(lpPDAListCtrl->lpSubItem[i].lpSubItemCaption);
				}
				if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
					bText=FALSE;
				for (i=0;i<lpPDAListCtrl->iItemNum;i++)
				{ // ɾ����Ŀ
						lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i);
						FreeItem(lpMainItem,bText);
				}
				if (lpPDAListCtrl->lpSubItem[0].lpSubItemCaption)
					free(lpPDAListCtrl->lpSubItem[0].lpSubItemCaption);
				PtrListDestroy(&lpPDAListCtrl->lpSubItem[0].plSubItemContent);
				free(lpPDAListCtrl->lpSubItem);
			}
      if (lpPDAListCtrl->lpColumnOrderArray)
         free(lpPDAListCtrl->lpColumnOrderArray);
      // �ƻ�ͼ���б�
#ifdef USE_ICON_IMAGE
	  ImageList_Destroy(lpPDAListCtrl->lpImageList);
	  ImageList_Destroy(lpPDAListCtrl->lpImageListSmall);
#endif
	  free(lpPDAListCtrl); // �ͷŽṹ

}
/**************************************************
������static LRESULT DoInsertColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ���ɹ�����0�����򷵻�-1
��������������һ�У�����LVM_INSERTCOLUMN��Ϣ��
����: 
************************************************/
static LRESULT DoInsertColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		int  iCol;
		LPLVCOLUMN lplvColumn;
		LPPDALISTCTRL lpPDAListCtrl;
//		LPMAINITEM lpMainItem;
		int cbCaptionLen;
//		DWORD dwStyle;
		int Width,i;
		LPVOID lpReallocPtr;
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return -1;
//			dwStyle=GetWindowLong(hWnd,GWL_STYLE);

			iCol=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
			lplvColumn=(LPLVCOLUMN)lParam; // �õ�������
			if (iCol>lpPDAListCtrl->iSubItemNum) return -1;

	    if(iCol==lpPDAListCtrl->iMaxSubItemNum)
			{// ����һ�е����
				// �����еĸ���
				lpReallocPtr=realloc(	lpPDAListCtrl->lpSubItem,sizeof(SUBITEM)*(lpPDAListCtrl->iMaxSubItemNum+DEFAULTSUBITEMNUM)); // �����µ��б�
				if (lpReallocPtr==NULL)
					return -1;
				lpPDAListCtrl->lpSubItem=lpReallocPtr; 
				lpPDAListCtrl->iMaxSubItemNum+=DEFAULTSUBITEMNUM; // ����������Ŀ����

				lpReallocPtr=realloc(	lpPDAListCtrl->lpColumnOrderArray,sizeof(int)*lpPDAListCtrl->iMaxSubItemNum); // �����µ��д����б�
				if (lpReallocPtr==NULL)
					return -1;
				lpPDAListCtrl->lpColumnOrderArray=lpReallocPtr;
			}
	    memmove( &lpPDAListCtrl->lpSubItem[iCol+1], &lpPDAListCtrl->lpSubItem[iCol], (lpPDAListCtrl->iSubItemNum-iCol)*sizeof(SUBITEM) ); // �ƶ�ԭ����������
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // �����µ��б�����б�����
				if (lpPDAListCtrl->lpColumnOrderArray[i]>=iCol)
					lpPDAListCtrl->lpColumnOrderArray[i]++;
			}
			memmove( &lpPDAListCtrl->lpColumnOrderArray[iCol+1], &lpPDAListCtrl->lpColumnOrderArray[iCol], (lpPDAListCtrl->iSubItemNum-iCol)*sizeof(int) ); // �ƶ�ԭ�����б�����
			
			lpPDAListCtrl->lpColumnOrderArray[iCol]=iCol; // ���õ�ǰ���д���

			lpPDAListCtrl->iSubItemNum++;	 // ����Ŀ��1
			lpPDAListCtrl->lpSubItem[iCol].CaptionFmt=lplvColumn->mask;
			lpPDAListCtrl->lpSubItem[iCol].ContentFmt=0;
			lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption=NULL;
			if (lplvColumn->mask&LVCF_TEXT)
			{ // �õ��еı���
				cbCaptionLen=strlen(lplvColumn->pszText);
				lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption=(LPTSTR)malloc(cbCaptionLen+1);
				strcpy(lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption,lplvColumn->pszText);
			}
			if (lplvColumn->mask&LVCF_WIDTH)
			{ // �����еĿ��
				lpPDAListCtrl->lpSubItem[iCol].iSubItemWidth=lplvColumn->cx;
			}
			else
			{
				lpPDAListCtrl->lpSubItem[iCol].iSubItemWidth=DEFAULTWIDTH;
			}
			// �����еķ��
			if (lplvColumn->mask&LVCF_FMT)
			{
				lpPDAListCtrl->lpSubItem[iCol].ContentFmt=lplvColumn->fmt;
			}
			if (lplvColumn->mask&LVCF_IMAGE)
			{
				lpPDAListCtrl->lpSubItem[iCol].iImage=lplvColumn->iImage;
			}
			//����һ������
			if (iCol==0) // Insert main item 
			{
					lpPDAListCtrl->iMaxItemNum=DEFAULTITEMNUM;
					lpPDAListCtrl->iItemNum=0;
					PtrListCreate(&(lpPDAListCtrl->lpSubItem[0].plSubItemContent),(ccIndex)lpPDAListCtrl->iMaxItemNum,(ccIndex)16);
			}
			else
			{ // ����һ������
					//lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
					if (!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_FILE))
					{
						PtrListCreate((&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent),lpPDAListCtrl->iMaxItemNum,16);
					}
			}
			// �õ��б��ܿ��
			Width=0;
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{
				Width+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
			}
			lpPDAListCtrl->iWidth=Width;
			SetHScrollBar(hWnd);	// ����ˮƽ������
			return 0; 
}

/**************************************************
������static LRESULT DoPaint(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƿ���,����WM_PAINT��Ϣ��
����: 
************************************************/
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
		DrawListCtrlCaption(hWnd,hdc); // ���Ʊ���
		DrawListCtrl(hWnd,hdc); // ��������
		return 0;
}
/**************************************************
������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_LBUTTONDOWN��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINTS points;
		int iPointPosition,CurItem;
		HDC hdc;
		LPPDALISTCTRL lpPDAListCtrl;
		int xCoordinate;
		int i,iSubItem;
		DWORD dwStyle;
		LPMAINITEM lpMainItem;
//		POINT pt;
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
      if (GetFocus()!=hWnd) // �õ�����
			  SetFocus(hWnd); // ���ý���
//			points=MAKEPOINTS(lParam);
      points.x=LOWORD(lParam);
      points.y=HIWORD(lParam);
			SetCapture(hWnd);

			iPointPosition=PositionInWindow(hWnd,points,&CurItem); // �õ������ڵ�λ��
			if (iPointPosition==CAPTIONITEM)
			{  // ���ڱ�����Ŀ
//					SetCapture(hWnd);
					hdc=GetDC(hWnd);
 					DrawCaptionItem(hWnd,hdc,CurItem,FALSE); // ���Ʊ�����Ŀ����
					ReleaseDC(hWnd,hdc);
					lpPDAListCtrl->iActivePos=CAPTIONITEM; // ���õ�ǰ״̬
					lpPDAListCtrl->iActiveItem=CurItem;
					lpPDAListCtrl->bPressed=TRUE;
			}
			if (iPointPosition==CAPTIONSEPARATE)
			{ // ���ڱ�����
//					SetCapture(hWnd);
					hdc=GetDC(hWnd);
					// �õ��ָ��ߵ�λ��
					xCoordinate=lpPDAListCtrl->cx;
					for (i=0;i<=CurItem;i++)
					{
						iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
						xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth; 
					}
//					xCoordinate+=lpPDAListCtrl->lpSubItem[CurItem].iSubItemWidth;
					DrawLineSeparate(hWnd,hdc,xCoordinate); // ���Ʒָ���
					ReleaseDC(hWnd,hdc);
					lpPDAListCtrl->iActivePos=CAPTIONSEPARATE;  // ���õ�ǰ״̬
					lpPDAListCtrl->iActiveItem=CurItem;
					lpPDAListCtrl->iPenCoordinate=(int)points.x;
					lpPDAListCtrl->iNewWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			}
			if (iPointPosition==LISTITEM)
			{ // ������ʽ��Ŀ
					// ���õ�ǰ״̬
					lpPDAListCtrl->iActivePos=LISTITEM;
					lpPDAListCtrl->iActiveItem=CurItem;
				  GetPointPosition(hWnd,points,&lpPDAListCtrl->iDownRow,&lpPDAListCtrl->iDownColumn);
				  GetPointPosition(hWnd,points,&lpPDAListCtrl->iRow,&lpPDAListCtrl->iColumn);
					if (CurItem>=lpPDAListCtrl->iItemNum)
						 return 0;

					dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ���ǰ���
					if (dwStyle&LVS_SINGLESEL)
					{ // ֻ�ܵ�ѡ
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,CurItem); // �õ���ǰ��Ŀ�ṹ
						if (lpPDAListCtrl->iFocusItem==CurItem)
						{ // ��ǰ��Ŀ���ڽ�����Ŀ
							if (SendMessage(hWnd,LVM_GETSELECTEDCOUNT,0,0)==1)
							{ // ��ǰֻ��һ����Ŀѡ��
								lpPDAListCtrl->bActiveItem=TRUE;  // ��ǰ��ĿΪ���Ŀ
								return 0;
							}
						}
						lpPDAListCtrl->bActiveItem=FALSE;
						// clear old Invert
						ClearAllInvert(hWnd);
						ClearFocus(hWnd);
					
						if (lpMainItem)
						{  // ���õ�ǰ��Ŀ�õ������ѡ��
// !!! Add By Jami chen in 2003.01.15
							UINT uOldState = lpMainItem->State;
							POINT pt;
// !!! Add End By Jami chen in 2003.01.15
							lpMainItem->State|=LVIS_SELECTED;
							lpMainItem->State|=LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
							pt.x=0;
							pt.y=0;
							SendNormalNotify(hWnd,LVN_ITEMCHANGED ,CurItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
						}
//						lpPDAListCtrl->iActivePos=LISTITEM;
//						lpPDAListCtrl->iActiveItem=CurItem;

						lpPDAListCtrl->iFocusItem=CurItem;
						DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // ���Ƶ�ǰ��Ŀ
					}
					else
					{ // ��ѡ��Ŀ
						if (lpPDAListCtrl->iFocusItem>=0)
						{// ��ǰ�н��㴰��
							lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem); // �õ�����Ŀ�ṹ
							if (lpMainItem)
							{  // ȡ����ǰ״̬�Ľ�������
// !!! Add By Jami chen in 2003.01.15
								UINT uOldState = lpMainItem->State;
								POINT pt;
// !!! Add End By Jami chen in 2003.01.15
								lpMainItem->State&=~LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
								pt.x=0;
								pt.y=0;
								SendNormalNotify(hWnd,LVN_ITEMCHANGED ,lpPDAListCtrl->iFocusItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
								DrawItem(hWnd,lpPDAListCtrl->iFocusItem); // ���ƽ�����Ŀ
							}
						}
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,CurItem); // �õ���ǰ��Ŀ�Ľṹ
						if (lpMainItem)
						{  // ���õ�ǰ��ĿΪ���㣬ѡ����Ŀ
// !!! Add By Jami chen in 2003.01.15
							UINT uOldState = lpMainItem->State;
							POINT pt;
// !!! Add End By Jami chen in 2003.01.15
							lpMainItem->State^=LVIS_SELECTED;
							lpMainItem->State|=LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
							pt.x=0;
							pt.y=0;
							SendNormalNotify(hWnd,LVN_ITEMCHANGED ,CurItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
						}
//						lpPDAListCtrl->iActivePos=LISTITEM;
//						lpPDAListCtrl->iActiveItem=CurItem;

						lpPDAListCtrl->iFocusItem=CurItem;
						DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // ���Ƶ�ǰ��Ŀ
					}
					PostMessage(hWnd,LVM_ENSUREVISIBLE,(WPARAM)CurItem,(LPARAM)FALSE); // ��֤��ǰ��Ŀ�ɼ�
	
		}
		if (iPointPosition==LISTBLANKSPACE)
		{  // �����б��ֿհ�λ��
			GetPointPosition(hWnd,points,&lpPDAListCtrl->iDownRow,&lpPDAListCtrl->iDownColumn);
			GetPointPosition(hWnd,points,&lpPDAListCtrl->iRow,&lpPDAListCtrl->iColumn);
			ClearAllInvert(hWnd);  // ������е�ѡ��
//			ClearFocus(hWnd);
			lpPDAListCtrl->iActivePos=LISTITEM;
		}
		if (iPointPosition==BLANKSPACE)
		{ // ���ڿհ�λ��
			lpPDAListCtrl->iActivePos=BLANKSPACE;
		}
		return 0;
}
/**************************************************
������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_MOUSEMOVE��Ϣ��
����: 
************************************************/
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	POINTS points;
	int iPointPosition,CurItem,xCoordinate;
	HDC hdc;
	int i,iSubItem;

			if (GetCapture()!=hWnd)  // �Ƿ��Ѿ�ץס���
				return 0;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
				return 0;
			points.x=LOWORD(lParam);
			points.y=HIWORD(lParam);

			if (lpPDAListCtrl->iActivePos==CAPTIONITEM)
			{  // ��ǰ���ڱ�����Ŀ
					iPointPosition=PositionInWindow(hWnd,points,&CurItem); // �õ���ǰ��Ŀ��λ��
					if ((iPointPosition==lpPDAListCtrl->iActivePos)&&(CurItem==lpPDAListCtrl->iActiveItem))
					{ // ��ԭ�����µ���Ŀ
						if (lpPDAListCtrl->bPressed==TRUE) // ����ԭ����λ��
						  return 0;
						// �Ѿ��뿪ԭ����λ�ã��������½���ԭ����λ��
						hdc=GetDC(hWnd);
 						DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iActiveItem,FALSE);
						ReleaseDC(hWnd,hdc);
						lpPDAListCtrl->bPressed=TRUE;
					}
					else
					{ // �뿪ԭ����λ��
						if (lpPDAListCtrl->bPressed==FALSE)  // �Ѿ��뿪
						  return 0;
						// ��û���뿪�������뿪ԭ����λ��
						hdc=GetDC(hWnd);
 						DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iActiveItem,TRUE);
						ReleaseDC(hWnd,hdc);
						lpPDAListCtrl->bPressed=FALSE;
					}
			}
			if (lpPDAListCtrl->iActivePos==CAPTIONSEPARATE)
			{ // ���ڱ��������϶���Ŀ�Ĵ�С
					if (lpPDAListCtrl->iPenCoordinate==points.x)
					  return 0;
				  if (lpPDAListCtrl->iActiveItem<0)
				   return 0;
				  // �õ������λ��
					xCoordinate=lpPDAListCtrl->cx;
					for (i=0;i<lpPDAListCtrl->iActiveItem;i++)
					{
						iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
						xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
					}
					hdc=GetDC(hWnd);
					DrawLineSeparate(hWnd,hdc,(int)(xCoordinate+lpPDAListCtrl->iNewWidth)); // ���Ƽ����

					// �õ��µ�λ��
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[lpPDAListCtrl->iActiveItem];
					lpPDAListCtrl->iNewWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth+(points.x-lpPDAListCtrl->iPenCoordinate);
					// �����µĳ���
					if (lpPDAListCtrl->iNewWidth<0) 
					   lpPDAListCtrl->iNewWidth=0;
					DrawLineSeparate(hWnd,hdc,(int)(xCoordinate+lpPDAListCtrl->iNewWidth)); // �����µ�λ��
					ReleaseDC(hWnd,hdc);
			}
			if (lpPDAListCtrl->iActivePos==LISTITEM)
			{ // ������ʽ��Ŀ��
				int iDownRow,iDownColumn,iOldRow,iOldColumn,iRow,iColumn;
				int i,iItemNumInLine;
				int iCurItem;

//					GetPointPosition(hWnd,lpPDAListCtrl->ptsDown,&iDownRow,&iDownColumn);
//					GetPointPosition(hWnd,lpPDAListCtrl->ptsCurPosition,&iOldRow,&iOldColumn);
				    GetPointPosition(hWnd,points,&iRow,&iColumn);
					iDownRow=lpPDAListCtrl->iDownRow;
					iDownColumn=lpPDAListCtrl->iDownColumn;
					iOldRow=lpPDAListCtrl->iRow;
					iOldColumn=lpPDAListCtrl->iColumn;
					if (iRow==iOldRow&&iColumn==iOldColumn)
						return 0;
					
					// �����µ�ѡ����Ŀ

					// Move up and down
					if (iOldRow!=iRow)
					{
						if (iDownRow>iOldRow)
						{ // to up
							if (iOldRow>iRow)
							{  // to up continue
								for (i=iRow;i<iOldRow;i++)
									SetInvertRow(hWnd,i,iDownColumn,iOldColumn);
							}
							else
							{ // to down 
								if (iDownRow>iRow)
								{// to down but not over button down position
									for (i=iOldRow;i<iRow;i++)
										ClearInvertRow(hWnd,i,iDownColumn,iOldColumn);
								}
								else
								{// to down and over button down position
									for (i=iOldRow;i<iDownRow;i++)
										ClearInvertRow(hWnd,i,iDownColumn,iOldColumn);
									for (i=iDownRow;i<=iRow;i++)
										SetInvertRow(hWnd,i,iDownColumn,iOldColumn);
								}
							}
						}
						else
						{ // to down
							if (iOldRow<iRow)
							{  // to down continue
								for (i=iOldRow;i<=iRow;i++)
									SetInvertRow(hWnd,i,iDownColumn,iOldColumn);
							}
							else
							{ // to up 
								if (iDownRow<iRow)
								{// to up but not over button down position
									for (i=iRow+1;i<=iOldRow;i++)
										ClearInvertRow(hWnd,i,iDownColumn,iOldColumn);
								}
								else
								{// to up and over button down position
									for (i=iDownRow+1;i<=iOldRow;i++)
										ClearInvertRow(hWnd,i,iDownColumn,iOldColumn);
									for (i=iRow;i<=iDownRow;i++)
										SetInvertRow(hWnd,i,iDownColumn,iOldColumn);
								}
							}
						}
						iOldRow=iRow;
					}

					// Move left and right
					if (iOldColumn!=iColumn)
					{
						if (iDownColumn>iOldColumn)
						{ // to left
							if (iOldColumn>iColumn)
							{  // to left continue
								for (i=iColumn;i<iOldColumn;i++)
									SetInvertColumn(hWnd,i,iDownRow,iOldRow);
							}
							else
							{ // to right
								if (iDownColumn>iColumn)
								{// to right but not over button down position
									for (i=iOldColumn;i<iColumn;i++)
										ClearInvertColumn(hWnd,i,iDownRow,iOldRow);
								}
								else
								{// to right and over button down position
									for (i=iOldColumn;i<iDownColumn;i++)
										ClearInvertColumn(hWnd,i,iDownRow,iOldRow);
									for (i=iDownColumn;i<=iColumn;i++)
										SetInvertColumn(hWnd,i,iDownRow,iOldRow);
								}
							}
						}
						else
						{ // to right
							if (iOldColumn<iColumn)
							{  // to right continue
								for (i=iOldColumn;i<=iColumn;i++)
									SetInvertColumn(hWnd,i,iDownRow,iOldRow);
							}
							else
							{ // to left 
								if (iDownColumn<iColumn)
								{// to left but not over button down position
									for (i=iColumn+1;i<=iOldColumn;i++)
										ClearInvertColumn(hWnd,i,iDownRow,iOldRow);
								}
								else
								{// to left and over button down position
									for (i=iDownColumn+1;i<=iOldColumn;i++)
										ClearInvertColumn(hWnd,i,iDownRow,iOldRow);
									for (i=iColumn;i<=iDownColumn;i++)
										SetInvertColumn(hWnd,i,iDownRow,iOldRow);
								}
							}
						}
					}
//					lpPDAListCtrl->ptsCurPosition=points;
					lpPDAListCtrl->iColumn=iColumn;
					lpPDAListCtrl->iRow=iRow;
					iItemNumInLine=GetItemNumInLine(hWnd);
					iCurItem=iRow*iItemNumInLine+iColumn;
					SendMessage(hWnd,LVM_ENSUREVISIBLE,(WPARAM)iCurItem,(LPARAM)FALSE);  // ��֤��ǰ��Ŀ�ɼ�

					lpPDAListCtrl->lParam=lParam;
					lpPDAListCtrl->wParam=wParam;
//					ClientToScreen(hWnd,&points);
//					lParam=MAKELONG(points.x,points.y);
//					PostMessage(hWnd,LVM_MOUSEMOVE,wParam,lParam);
					SetSysTimer(hWnd,1,10,NULL); // ����ϵͳʱ��
			}
		return 0;
}
/**************************************************
������static LRESULT DoSysTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
��������������WM_SYSTIMER��Ϣ��
����: 
************************************************/
static LRESULT DoSysTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return 0;
		return DoMouseMove(hWnd,lpPDAListCtrl->wParam,lpPDAListCtrl->lParam); // �����ƶ�����ǰ��
}

/**************************************************
������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_LBUTTONUP��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINTS points;
		int iPointPosition,CurItem,xCoordinate;
		HDC hdc;
		LPPDALISTCTRL lpPDAListCtrl;
		int i;
    int iSubItem;
//		HWND hParent;
//		NMLISTVIEW nmListView;
		POINT pt;
		DWORD dwStyle;


		KillSysTimer(hWnd,1);  // ɱ��ϵͳʱ��
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return 0;

//			points=MAKEPOINTS(lParam);
//	  SetCapture(0);
	  ReleaseCapture();  // ȡ��ץס���
      points.x=LOWORD(lParam);
      points.y=HIWORD(lParam);

	 // Get Pen up From Position
			iPointPosition=PositionInWindow(hWnd,points,&CurItem);

	      if (lpPDAListCtrl->iActivePos == CAPTIONITEM)
      {  // The Caption Item is Active
		 if (lpPDAListCtrl->bPressed==TRUE)
         {    // Test if have Caption Item be Pressed , Then Release it.
				   hdc=GetDC(hWnd);
 				   DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iActiveItem,TRUE);
				   ReleaseDC(hWnd,hdc);
				   lpPDAListCtrl->bPressed=FALSE;
         }
      }
		if ((iPointPosition==lpPDAListCtrl->iActivePos)&&(CurItem==lpPDAListCtrl->iActiveItem))
		{// The Pen Up Position is same with the active area and the current item is same with active item
			if (iPointPosition==CAPTIONITEM)
			{
				// The Pen Up from the Caption Area
				dwStyle=GetWindowLong(hWnd,GWL_STYLE);

				if (!(dwStyle&LVS_NOSORTHEADER))
				{  // send LVN_COLUMNCLICK notify message
/*					nmListView.hdr.hwndFrom=hWnd;
					nmListView.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
					nmListView.hdr.code=LVN_COLUMNCLICK;
					nmListView.iItem=-1;
					nmListView.iSubItem=CurItem;
					nmListView.uNewState=0;
					nmListView.uOldState=0;
					nmListView.uChanged=0;
					pt.x=0;
					pt.y=0;
					nmListView.ptAction=pt;
					nmListView.lParam=0;
					hParent=GetParent(hWnd);
					SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmListView);
*/
					pt.x=points.x;
					pt.y=points.y;

// !!! Modified By jami chen 2002.04.25
//					SendNormalNotify(hWnd,LVN_COLUMNCLICK,-1,CurItem,pt);
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[CurItem];
					SendNormalNotify(hWnd,LVN_COLUMNCLICK,-1,iSubItem,0,0,pt);
// !!! Modified End By jami chen 2002.04.25

				}
			}
		}
		if (lpPDAListCtrl->iActivePos==CAPTIONSEPARATE)
		{  // the active area is caption separate
		  if (lpPDAListCtrl->iActiveItem>=0)
		  {
			  // reset the width of the caption item
				   xCoordinate=lpPDAListCtrl->cx;
				   for (i=0;i<lpPDAListCtrl->iActiveItem;i++)
				   {
					   iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
					   xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
				   }
				   xCoordinate+=lpPDAListCtrl->iNewWidth;
				   iSubItem=lpPDAListCtrl->lpColumnOrderArray[lpPDAListCtrl->iActiveItem];
				   lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth=lpPDAListCtrl->iNewWidth;
				   hdc=GetDC(hWnd);
				   DrawLineSeparate(hWnd,hdc,xCoordinate);
				   ReleaseDC(hWnd,hdc);
				   lpPDAListCtrl->iWidth=0;
				   for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
				   {
					   lpPDAListCtrl->iWidth+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
				   }
				   SetHScrollBar(hWnd);
				   InvalidateRect(hWnd,NULL,TRUE);
		  }
		}
		if (iPointPosition==LISTITEM)
		{  // ������ʽ��Ŀ
				dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
				if (dwStyle&LVS_SINGLESEL)
				{  // ��ѡ��Ŀ
					if (lpPDAListCtrl->iFocusItem == CurItem)
					{
						pt.x=points.x;
						pt.y=points.y;
// !!! Modified By Jami chen in 2002.05.13
//						SendNormalNotify(hWnd,NM_CLICK,CurItem,0,pt);
						SendActiveNotify(hWnd,NM_CLICK,CurItem,pt);
// !!! Modified End By Jami chen in 2002.05.13
						if (lpPDAListCtrl->bActiveItem==TRUE)
						{
							if (SendMessage(hWnd,LVM_GETSELECTEDCOUNT,0,0)==1)
							{
								SendActiveItem(hWnd,CurItem); // ���ǰ��Ŀ
							}  
						}
					}
				}
		}
		lpPDAListCtrl->iActiveItem=-1;  // ���õ�ǰû�м�����Ŀ
		lpPDAListCtrl->iActivePos=BLANKSPACE;
		SendBasicNotify(hWnd,NM_RELEASEDCAPTURE); // ����֪ͨ��Ϣ
		return 0;
}
/**************************************************
������static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam, LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ָ��LVITEM��ָ��
����ֵ���ɹ����ز�����Ŀ�����������򷵻�-1
��������������һ����Ŀ������LVM_INSERTITEM��Ϣ��
����: 
************************************************/
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam, LPARAM lParam)
{
		LPLVITEM lpItem;
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
//		LPTSTR lpString;
		int cbStringLen;
		int i,iInsertIndex,iPageItemNum;
		DWORD dwStyle;
//		HDC hdc;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return -1;
#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif

			lpItem=(LPLVITEM)lParam;

			lpMainItem=(LPMAINITEM)malloc(sizeof(MAINITEM)); // �õ�����Ŀ
//			lpMainItem->bSelected=FALSE;
			lpMainItem->State=0;
			lpMainItem->lParam=0;
			lpMainItem->iImage=-1;
			lpMainItem->lpText=NULL;
//			lpMainItem->x=0;
//			lpMainItem->y=0;
			if (lpMainItem==NULL) 
			  return -1;
			if (lpItem->mask&LVIF_TEXT)
			{ // �ṹ�ı���Ŀ��Ч
					if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
					{
						lpMainItem->lpText=lpItem->pszText;// this is a file No.
					}
					else
					{  // ����һ���ı�
						cbStringLen=strlen(lpItem->pszText);
						lpMainItem->lpText=(LPTSTR)malloc(cbStringLen+1);
						if (lpMainItem->lpText==NULL)
						{
								free(lpMainItem);
								return -1;
						}
						strcpy(lpMainItem->lpText,lpItem->pszText);
					}
			}
			if (lpItem->mask&LVIF_IMAGE)
			{ // ͼ����Ŀ��Ч
				lpMainItem->iImage=lpItem->iImage;
			}
			if (lpItem->mask&LVIF_STATE)
			{ // ״̬��Ŀ��Ч
				lpMainItem->State=lpItem->state&lpItem->stateMask;
			}
			if (lpItem->mask&LVIF_PARAM)
			{ // ������Ŀ��Ч
				lpMainItem->lParam=lpItem->lParam;
			}

			dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
			if (dwStyle&LVS_SORTASCENDING)
			{// ��������
				 iInsertIndex=AutoSortItem(lpPDAListCtrl,lpMainItem,SORTASCENDING);
			}
			else if (dwStyle&LVS_SORTDESCENDING)
			{ // ��������
				 iInsertIndex=AutoSortItem(lpPDAListCtrl,lpMainItem,SORTDESCENDING);
			}
			else
			{// ������
				// !!! Modified By Jami chen in 2003.07.09
				//iInsertIndex=lpPDAListCtrl->iItemNum;
				// !!!!
				iInsertIndex = lpItem->iItem;
				if (iInsertIndex < 0)
					iInsertIndex = 0;
				else if (iInsertIndex > lpPDAListCtrl->iItemNum)
				{
					iInsertIndex = lpPDAListCtrl->iItemNum;
				}
				// !!!
			}
			if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iInsertIndex,lpMainItem)==FALSE) // ������Ŀ
				return -1;
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{  // ����յ�����
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
				{
					if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iInsertIndex,NULL)==FALSE)
						return -1;
				}
			}
			lpPDAListCtrl->iItemNum++;  // ����һ����Ŀ
			SetVScrollBar(hWnd);
			iPageItemNum=GetPageItemNum(hWnd); // �õ�һҳ����Ŀ����
			if (iInsertIndex>=lpPDAListCtrl->iStartItem)
			{  // Redraw the window
				for (i=iInsertIndex;i<lpPDAListCtrl->iStartItem+iPageItemNum;i++)
				{
				   InvalidateItem(hWnd,i);
				}
//				hdc=GetDC(hWnd);
//				DrawListCtrl(hWnd,hdc);
//				DrawItem(hWnd,iInsertIndex);
//				ReleaseDC(hWnd,hdc);
			}
			return iInsertIndex;
}

/**************************************************
������static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ָ��LVITEM��ָ��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
����������������Ŀ��
����: 
************************************************/
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPLVITEM lpItem;
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		LPTSTR lpString;
		int cbStringLen;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
			lpItem=(LPLVITEM)lParam; // �õ���Ŀ�ṹ
			if (lpItem->iItem>lpPDAListCtrl->iItemNum)
				return FALSE;
			if (lpItem->iSubItem>lpPDAListCtrl->iSubItemNum)
				return FALSE;

			
			if (lpItem->mask&LVIF_TEXT)
			{  // ��Ŀ�ı���Ч
// !!! Add By Jami Chen in 2002.05.09
// when the list is a file prop ,then direct set the pzstext
				if (lpPDAListCtrl->lpSubItem[lpItem->iSubItem].ContentFmt&LVCFMT_FILE)
				{ // �ļ�������Ŀ
					if (lpItem->iSubItem==0)
					{
						lpMainItem=(LPMAINITEM)PtrListAt(&(lpPDAListCtrl->lpSubItem[lpItem->iSubItem]).plSubItemContent,lpItem->iItem);
						if (lpMainItem==NULL)
							return 0;
						lpMainItem->lpText=lpItem->pszText;
					}
					else
						return -1;
				}
				else
				{ // �ı�����
// !!! Add End By Jami chen in 2002.05.09
					cbStringLen=strlen(lpItem->pszText);
					lpString=(LPTSTR)malloc(cbStringLen+1);  //���� 
					if (lpString==NULL)
					{
							return -1;
					}
					strcpy(lpString,lpItem->pszText);
					if (lpItem->iSubItem==0)
					{ // ��ǰ��Ŀ������Ŀ
						lpMainItem=(LPMAINITEM)PtrListAt(&(lpPDAListCtrl->lpSubItem[lpItem->iSubItem]).plSubItemContent,lpItem->iItem);
						if (lpMainItem==NULL)
							return 0;
						if (lpMainItem->lpText)
							free(lpMainItem->lpText);
						lpMainItem->lpText=lpString;
					}
					else
					{  // ��ǰ��Ŀ������Ŀ
						LPTSTR lpOldString;
						lpOldString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);
						if (lpOldString!=NULL)
							free(lpOldString);
						PtrListAtPut(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem,lpString);
					}
				}
			}
			if (lpItem->mask&LVIF_IMAGE)
			{// ͼ����Ŀ��Ч
					if (lpItem->iSubItem==0)
					{ // ��ǰ������Ŀ
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);;
						if (lpMainItem==NULL)
							return 0;
//						if (lpMainItem->lpText)
//							free(lpMainItem->lpText);
//						lpMainItem->lpText=lpString;
						lpMainItem->iImage=lpItem->iImage;
					}
					else
					{ // ��ǰ������Ŀ
// !!! Add By Jami Chen in 2002.05.09
// when the list is a file prop ,then direct set the pzstext
						if (lpPDAListCtrl->lpSubItem[lpItem->iSubItem].ContentFmt&LVCFMT_FILE)
						{
								return -1;
						}
						else
						{
// !!! Add End By Jami chen in 2002.05.09
							PtrListAtPut(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem,(void *)lpItem->iImage);
						}
					}
			}
			if (lpItem->mask&LVIF_STATE)
			{ // ״̬��Ŀ��Ч
					if (lpItem->iSubItem==0)
					{  // ��ǰ������Ŀ
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);;
						if (lpMainItem==NULL)
							return 0;
						lpMainItem->State=lpItem->state&lpItem->stateMask;
					}
			}
			if (lpItem->mask&LVIF_PARAM)
			{ // ������Ŀ��Ч
					if (lpItem->iSubItem==0)
					{
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);;
						if (lpMainItem==NULL)
							return 0;
						lpMainItem->lParam=lpItem->lParam;
					}
			}
            InvalidateItem(hWnd,(int)lpItem->iItem); // ��Ч��Ŀ
			return 0;
}
/**************************************************
������static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ͼ���б������
	IN lParam -- HIMAGELIST ͼ���б���
����ֵ���ɹ�����ԭ��ͼ���б�ľ�������򷵻�NULL
��������������ͼ���б�����LVM_SETIMAGELIST��Ϣ��
����: 
************************************************/
static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iImageList;
		HIMAGELIST himl,hOldiml=NULL;
#ifdef USE_BITMAP_IMAGE
		HBITMAP hBitMap;
#endif
#ifdef USE_ICON_IMAGE
//		HICON  hIcon;
#endif
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iImageList=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif

			himl=(HIMAGELIST)lParam;  // ͼ���б�

			if (iImageList==LVSIL_SMALL)
			{ // Сͼ��
				hOldiml=lpPDAListCtrl->lpImageListSmall;
				lpPDAListCtrl->lpImageListSmall=himl;
#ifdef USE_BITMAP_IMAGE
				hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,0);
//			  GetBitmapDimensionEx(hBitMap,&(lpPDAListCtrl->sizeSmallIcon));
				GetBitMapSize(hBitMap,&(lpPDAListCtrl->sizeSmallIcon));
#endif
#ifdef USE_ICON_IMAGE
				//hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,0);
//				GetIconSize(hIcon,&(lpPDAListCtrl->sizeSmallIcon));
				lpPDAListCtrl->sizeSmallIcon.cx=16;
				lpPDAListCtrl->sizeSmallIcon.cy=16;
#endif
			}
			if (iImageList==LVSIL_NORMAL)
			{ // ��ͼ��
				hOldiml=lpPDAListCtrl->lpImageList;
				lpPDAListCtrl->lpImageList=himl;
#ifdef USE_BITMAP_IMAGE
				hBitMap=PtrListAt(lpPDAListCtrl->lpImageList,0);
//			  GetBitmapDimensionEx(hBitMap,&(lpPDAListCtrl->sizeIcon));
				GetBitMapSize(hBitMap,&(lpPDAListCtrl->sizeIcon));
#endif
#ifdef USE_ICON_IMAGE
				//hIcon=PtrListAt(lpPDAListCtrl->lpImageList,0);
//				GetIconSize(hIcon,&(lpPDAListCtrl->sizeIcon));
				lpPDAListCtrl->sizeIcon.cx=32;
				lpPDAListCtrl->sizeIcon.cy=32;
#endif
			}
			return (LRESULT)hOldiml;
}
/**************************************************
������static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- LOWORD ������־
	IN lParam -- ����
����ֵ����
������������ֱ�������ڣ�����WM_HSCROLL��Ϣ��
����: 
************************************************/
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nPageLines;
    int yPos;
        
		KillEditControl(hWnd);  // ɱ���༭�ؼ�
			nPageLines=GetPageLine(hWnd);
      yPos=LV_GetScrollPos(hWnd,SB_VERT); //�õ�����λ��
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          nScrollLine=0-nPageLines;
          break;
        case SB_PAGEDOWN:  //page down
          nScrollLine=nPageLines;
          break;
        case SB_LINEUP:  // line up
          nScrollLine=-1;
          break;
        case SB_LINEDOWN:  // line down
          nScrollLine=1;
          break;
        case SB_THUMBTRACK: // drag thumb track
          nScrollLine=HIWORD(wParam)-yPos;
          break;
        default:
          nScrollLine=0;
          return 0;
        }
				// vertical scroll window
        VScrollWindow(hWnd,nScrollLine); // ��������
				return 0;
}
/****************************************************************************************************/
/**************************************************
������static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- LOWORD ������־
	IN lParam -- ����
����ֵ����
����������ˮƽ�������ڣ�����WM_HSCROLL��Ϣ��
����: 
************************************************/
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nPageLines;
    int xPos;
        
		KillEditControl(hWnd); // ɱ���༭����
			nPageLines=GetWindowWidth(hWnd)/WIDTHSTEP;
      xPos=LV_GetScrollPos(hWnd,SB_HORZ); // �õ���ǰ������λ��
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          nScrollLine=0-nPageLines;
          break;
        case SB_PAGEDOWN:  //page down
          nScrollLine=nPageLines;
          break;
        case SB_LINEUP:  // line up
          nScrollLine=-1;
          break;
        case SB_LINEDOWN:  // line down
          nScrollLine=1;
          break;
        case SB_THUMBTRACK: // drag thumb track
          nScrollLine=HIWORD(wParam)-xPos;
          break;
        default:
          nScrollLine=0;
          return 0;
        }
				// vertical scroll window
        HScrollWindow(hWnd,nScrollLine);  // ˮƽ��������
				return 0;
}
/****************************************************************************************************/
/****************************************************************************************************/
#ifdef _PLAN_ONE_
/**************************************************
������static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- �Ƚϲ���
	IN lParam -- �ȽϺ���
����ֵ����
������������Ŀ���򣬴���LVM_SORTITEMS��Ϣ��
����: 
************************************************/
static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CompareFunc Compare;
	LPARAM lParamSort;
	LPPDALISTCTRL lpPDAListCtrl;
	int i,j,iPreItem;
	LPARAM  lParam1,lParam2;
	LPMAINITEM lpMainItem;
//	HDC hdc;

			KillEditControl(hWnd);  // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;

			lParamSort=(LPARAM)wParam; 

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
			// 
			Compare=(CompareFunc)lParam;  // �õ��ȽϺ���
			for (i=0;i<lpPDAListCtrl->iItemNum;i++)
			{  // �������е���Ŀ
				iPreItem=i;
				lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iPreItem); // �õ�����1 ����Ŀ�ṹ
				lParam1=lpMainItem->lParam;
				for (j=i+1;j<lpPDAListCtrl->iItemNum;j++)
				{
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,j); // �õ���Ŀ2 ����Ŀ�ṹ
					lParam2=lpMainItem->lParam;
					if (Compare(lParam1,lParam2,lParamSort)>0) // �Ƚ���Ŀ
					{
						iPreItem=j;
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iPreItem);
						lParam1=lpMainItem->lParam;
					}
				}
				if (iPreItem!=i)
				{
					SwapItem(lpPDAListCtrl,i,iPreItem); // ������Ŀ
				}
			}
			lpPDAListCtrl->iStartItem=0;
			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE);
			GetListFocus(hWnd);  // �õ���ʽ����

			InvalidateRect(hWnd,NULL,TRUE);  // ��Ч����
			return 0;
}
/**************************************************
������static void SwapItem(LPPDALISTCTRL lpPDAListCtrl,int iItem1,int iItem2)
������
	IN lpPDAListCtrl -- �б���ƽṹָ��
	IN iItem1 -- ��Ŀ1������
	IN iItem2 -- ��Ŀ2������	
����ֵ����
������������������Ŀ��
����: 
************************************************/
static void SwapItem(LPPDALISTCTRL lpPDAListCtrl,int iItem1,int iItem2)
{
	LPVOID lpTemp1,lpTemp2;
	int i;

			lpTemp1=(LPVOID)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem1); // �õ���Ŀ1 �����ݽṹ
			lpTemp2=(LPVOID)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem2); // �õ���Ŀ2 �����ݽṹ
			PtrListAtPut(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem1,lpTemp2); // ����Ŀ2 ���������õ���Ŀ1 ������
			PtrListAtPut(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem2,lpTemp1); // ����Ŀ1 ���������õ���Ŀ2 ������
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // ������������Ŀ������
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
	            { // ��������ļ���ʽ�����ݣ�����Ҫ��������
						   lpTemp1=(LPVOID)PtrListAt(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iItem1);
						   lpTemp2=(LPVOID)PtrListAt(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iItem2);
						   PtrListAtPut(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iItem1,lpTemp2);
						   PtrListAtPut(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iItem2,lpTemp1);
 	           }
			}
}
#endif

#ifdef _PLAN_TWO_
/*
  It is used "dichotomy" sort.
  It is fast , but need other memory to save old item.
*/
/**************************************************
������static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- �Ƚϲ���
	IN lParam -- �ȽϺ���
����ֵ����
������������Ŀ���򣬴���LVM_SORTITEMS��Ϣ��
����: 
************************************************/
static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CompareFunc Compare;
	LPARAM lParamSort;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
  LPPTRLIST lpPtrList;
  int i,iIndex,iCol,iInsertIndex;
  LPVOID lpSubItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;

			lParamSort=(LPARAM)wParam;  // �õ��������

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			Compare=(CompareFunc)lParam;  //�õ��ȽϺ���

      lpPtrList=malloc(sizeof(PTRLIST)*(lpPDAListCtrl->iSubItemNum+1));  // ����һ��ָ���б�������ԭ��������

      if (lpPtrList==NULL)
      {
        ASSERT(0);
        return FALSE;
      }
      // save old item;
	  for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
      {
        lpPtrList[i]=lpPDAListCtrl->lpSubItem[i].plSubItemContent;
      }

      //Clear old item
      PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[0].plSubItemContent);
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{  // �������Ŀ������
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
				{
           PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
				}
			}

      // Insert item
			for (iIndex=0;iIndex<lpPDAListCtrl->iItemNum;iIndex++)
      {
				 lpMainItem=(LPMAINITEM)PtrListAt(&lpPtrList[0],iIndex); // �õ�Ҫ������Ŀ������
         iInsertIndex=InsertPosition(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpMainItem,Compare,lParamSort); // �õ������λ��
			   if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iInsertIndex,lpMainItem)==FALSE)
				   return FALSE;
			   for (iCol=1;iCol<lpPDAListCtrl->iSubItemNum;iCol++)
			   { // ��������Ŀ
				   if (!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_FILE))
				   {
				     lpSubItem=(LPVOID)PtrListAt(&lpPtrList[iCol],iIndex);
					   if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent,iInsertIndex,lpSubItem)==FALSE)
						   return FALSE;
				   }
         }
      }
      free(lpPtrList); // �ͷŷ�����ڴ�
			lpPDAListCtrl->iStartItem=0;
			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE); // ���ù��������λ��
			GetListFocus(hWnd); // �õ�����

      InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
      return TRUE;
}
/**************************************************
������static int InsertPosition(LPPTRLIST lpPtrList,LPMAINITEM lpMainItem,CompareFunc Compare,LPARAM lParamSort)
������
	IN lpPtrList --  ��Ŀ����
	IN lpMainItem -- Ҫ�������Ŀ
	IN Compare -- �ȽϺ���
	IN lParamSort -- �Ƚϲ���
����ֵ�����ز���λ��
�����������õ���ĿҪ�����λ�á�
����: 
************************************************/
static int InsertPosition(LPPTRLIST lpPtrList,LPMAINITEM lpMainItem,CompareFunc Compare,LPARAM lParamSort)
{
   int start ,end, middle;//,result;
   LPMAINITEM lpItem;
   LPARAM lParam1,lParam2;

     if (lpPtrList->count==0)
     return 0;

     start=0;
     end=lpPtrList->count-1;

     // ʹ�ö��ַ���ѯ�ķ����ҵ�Ҫ�����λ��
     lParam1=lpMainItem->lParam;
     while(1)
     {
       middle=(start+end)/2; // �õ��м������
       if (middle==start||middle==end)
       {
       	 lpItem=(LPMAINITEM)PtrListAt(lpPtrList,end); // �õ����һ����Ŀ������������
         lParam2=lpItem->lParam;
			   if (Compare(lParam1,lParam2,lParamSort)>=0) // �������һ����Ŀ
         {
           return end+1; // �嵽���һ����Ŀ�ĺ���
         }
       	 lpItem=(LPMAINITEM)PtrListAt(lpPtrList,start); // �õ���һ����Ŀ������
         lParam2=lpItem->lParam;
			   if (Compare(lParam1,lParam2,lParamSort)<0) // С�ڵ�һ����Ŀ
         {
           return start; // �嵽��һ����Ŀ��ǰ��
         }
         return end; // �嵽���һ����Ŀ��ǰ��
       }
			 lpItem=(LPMAINITEM)PtrListAt(lpPtrList,middle); // �õ��м���Ŀ������
       lParam2=lpItem->lParam;
			 if (Compare(lParam1,lParam2,lParamSort)>0) // �����м���Ŀ
       {
         start=middle; // ����ʼ��Ŀ�������赽�м���Ŀ��λ��
       }
       else
       {
         end=middle; // ��������Ŀ�赽�м���Ŀ��λ��
       }
     }
}
#endif
/**************************************************
������static LRESULT DoApproximateViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ�ĸ���
	IN lParam -- ����
����ֵ����WORD �����ؿ�ȣ���WORD �����ظ߶ȡ�
�����������õ�ָ����������Ŀ����Ŀ�͸ߣ�����LVM_APPROXIMATEVIEWRECT��Ϣ��
����: 
************************************************/
static LRESULT DoApproximateViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		int iCount,cx,cy;
		int iLineHeight,iItemNumInLine,iLine;
		DWORD dwStyle;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڵķ��
			iCount=(int)wParam; // �õ�Ҫ������Ŀ�ĸ���
			if (iCount==-1)
				iCount=(int)SendMessage(hWnd,LVM_GETITEMCOUNT,0,0); // ����������Ŀ

			if ((dwStyle&LVS_TYPEMASK)==LVS_REPORT)
			{  // ��ǰ״̬��REPORT���
				LPPDALISTCTRL lpPDAListCtrl;
					lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);
					if (lpPDAListCtrl==NULL)
						return 0;
					cx=lpPDAListCtrl->iWidth; // �����REPORT�Ŀ��
					if (ExistCaption(hWnd))
						cy=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT; // ��CAPTION�ĸ߶�
					else
						cy=0;
			}
			else
			{
				cx=GetWindowWidth(hWnd); // ���ڵĿ��
				cy=0;
			}
			iLineHeight=GetLineHeight(hWnd); // �õ��и�
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�п��Է��õ���Ŀ����
			iLine=(iCount+iItemNumInLine-1)/iItemNumInLine; // �õ�Ҫ���õ�����
			cy+=iLine*iLineHeight; // �õ���Ҫ�ĸ߶�
			return MAKELRESULT(cx,cy);
}
/****************************************************************************************************/
/**************************************************
������static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
����������ɱ�����㣬����WM_KILLFOCUS��Ϣ��
����: 
************************************************/
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	DWORD dwStyle;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڵķ��
			ClearFocus(hWnd); // �������
			if (!(dwStyle&LVS_SHOWSELALWAYS))
				ClearAllInvert(hWnd); // �������ѡ��
			SendBasicNotify(hWnd,NM_KILLFOCUS); // ֪ͨ������ 
			return 0;
}
/**************************************************
������static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ��Ҫɾ������Ŀ
	IN lParam -- ����
����ֵ���ɹ�����TRUE�����򷵻�FALSE
����������ɾ��ָ����Ŀ������LVM_DELETEITEM��Ϣ��
����: 
************************************************/
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem,iPageItemNum;
//		NMLISTVIEW nmListView;
//		HDC hdc;
		POINT pt;
//		HWND hParent;
		LPMAINITEM lpMainItem;
			
			KillEditControl(hWnd); // ɱ���༭����
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam; // �õ���Ŀ����

			if ( iItem >= lpPDAListCtrl->iItemNum)
				return 0;

/*
			nmListView.hdr.hwndFrom=hWnd;
			nmListView.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
			nmListView.hdr.code=LVN_DELETEITEM;
			nmListView.iItem=iItem;
			nmListView.iSubItem=0;
			nmListView.uNewState=0;
			nmListView.uOldState=0;
			nmListView.uChanged=0;
			pt.x=0;
			pt.y=0;
			nmListView.ptAction=pt;
			nmListView.lParam=0;
			hParent=GetParent(hWnd);
			SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmListView);
*/
			pt.x=0;
			pt.y=0;
			SendNormalNotify(hWnd,LVN_DELETEITEM,iItem,0,0,0,pt); // ֪ͨ������
			RemoveItem(lpPDAListCtrl,iItem);  // ɾ����Ŀ
			LV_SetScrollPos(hWnd,SB_VERT,(lpPDAListCtrl->iStartItem/GetItemNumInLine(hWnd)),TRUE); // ���ù�����λ��
			SetVScrollBar(hWnd);  // ���ù�����
			iPageItemNum=GetPageItemNum(hWnd);// �õ�ҳ��Ŀ����

			// add by jami chen for the focus item 2001.12.25
			if (iItem==lpPDAListCtrl->iFocusItem)
			{  // the delete item is focus item ,then must set a new item
				if (lpPDAListCtrl->iItemNum==0)
				{  // have no item
					lpPDAListCtrl->iFocusItem=-1;
				}
				else
				{  // set a new focus item
					if (lpPDAListCtrl->iFocusItem>=lpPDAListCtrl->iItemNum)
						lpPDAListCtrl->iFocusItem=lpPDAListCtrl->iItemNum-1;
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem);

					if (lpMainItem)
					{
// !!! Add By Jami chen in 2003.01.15
						UINT uOldState = lpMainItem->State;
						POINT pt;
// !!! Add End By Jami chen in 2003.01.15
						lpMainItem->State|=LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
						pt.x=0;
						pt.y=0;
						SendNormalNotify(hWnd,LVN_ITEMCHANGED ,lpPDAListCtrl->iFocusItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
					}
					lpPDAListCtrl->iActiveItem=lpPDAListCtrl->iFocusItem;
					DrawItem(hWnd,lpPDAListCtrl->iActiveItem);
				}
			}
			if (iItem<lpPDAListCtrl->iFocusItem)
			{  // adjust focus item 
					lpPDAListCtrl->iFocusItem--;
					lpPDAListCtrl->iActiveItem=lpPDAListCtrl->iFocusItem;
			}
			// Add end by jami chen

			if (iItem<lpPDAListCtrl->iStartItem+iPageItemNum)
			{  // Redraw the window
//				hdc=GetDC(hWnd);
//				DrawListCtrl(hWnd,hdc);
//				ReleaseDC(hWnd,hdc);
          InvalidateRect(hWnd,NULL,TRUE);
			}
			return TRUE;
}
/**************************************************
������static LRESULT DoEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������༭��ǩ������LVM_EDITLABEL��Ϣ��
����: 
************************************************/
static LRESULT DoEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iItem;
	LPMAINITEM lpMainItem;
	RECT rectItem,rectListView;
	DWORD dwStyle;
	int iImageWidth;
		
		KillEditControl(hWnd); // ɱ���༭�ؼ�
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return 0;
		iItem=(int)wParam;

		if (SendMessage(hWnd,LVM_GETITEMRECT ,iItem,(LPARAM)&rectItem) == FALSE) // �õ�ָ����Ŀ�ľ���
			return FALSE;
		
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if ( (dwStyle&LVS_TYPEMASK)==LVS_ICON)
		{ 
		}
		else
		{
			lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ�ָ����Ŀ�����ݽṹ
			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // ��ͼ��
				// �õ�ͼ�εĿ��
				if (lpPDAListCtrl->sizeSmallIcon.cx>lpPDAListCtrl->lpSubItem[0].iSubItemWidth)
						iImageWidth=lpPDAListCtrl->lpSubItem[0].iSubItemWidth;
				else
						iImageWidth=(int)lpPDAListCtrl->sizeSmallIcon.cx;
				rectItem.left += iImageWidth +1; // �õ��ı��Ŀ�ʼλ��
				if ( (dwStyle&LVS_TYPEMASK)==LVS_REPORT)
				{
					rectItem.left+= 2*IMAGESPACE;
				}
			}
			// �õ��༭���λ�����С

			// !!! modified By Jami chen in 2004.05.21
			//rectItem.top += (USETEXTHEIGHT - 18) /2;
			//rectItem.bottom = rectItem.top +  18 ;
			//rectItem.right = rectItem.left + strlen(lpMainItem->lpText) * 8 ;
			// !!!
			rectItem.top += (lpPDAListCtrl->iLineHeight - (lpPDAListCtrl->iTextHeight + 2)) /2;
			rectItem.bottom = rectItem.top +  (lpPDAListCtrl->iTextHeight + 2) ;
			rectItem.right = rectItem.left + strlen(lpMainItem->lpText) * lpPDAListCtrl->iTextWidth ;
			// !!! modified End
			
			GetClientRect(hWnd,&rectListView); // �õ����ڵĿͻ�����
			if (rectItem.right >=rectListView.right)
				rectItem.right = rectListView.right -1;

			// �����༭�ؼ�
			CreateLabelEdit(hWnd,iItem,lpMainItem->lpText,&rectItem);
		}
		return TRUE;
}

/**************************************************
������static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ���ɹ�����TRUE�����򷵻�FALSE
����������ɾ�����е���Ŀ������LVM_DELETEALLITEMS��Ϣ��
����: 
************************************************/
static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		int i;
//		HDC hdc;
//		NMLISTVIEW nmListView;
		BOOL bText=TRUE;
		POINT pt;
//		HWND hParent;
//		RECT rect;

			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
/*
			nmListView.hdr.hwndFrom=hWnd;
			nmListView.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
			nmListView.hdr.code=LVN_DELETEALLITEMS;
			nmListView.iItem=-1;
			nmListView.iSubItem=0;
			nmListView.uNewState=0;
			nmListView.uOldState=0;
			nmListView.uChanged=0;
			pt.x=0;
			pt.y=0;
			nmListView.ptAction=pt;
			nmListView.lParam=0;
			hParent=GetParent(hWnd);
			SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmListView);
*/
			pt.x=0;
			pt.y=0;
			SendNormalNotify(hWnd,LVN_DELETEALLITEMS,-1,0,0,0,pt); // ֪ͨ������

			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // ɾ�����е�����Ŀ
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
				{
					if(!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_IMAGE))
						PtrListFreeAll(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
          else
            PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
				}
//				if(!(lpPDAListCtrl->lpSubItem[i].ContentFmt&(LVCFMT_IMAGE|LVCFMT_FILE)))
//					PtrListFreeAll(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
			}
			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
				bText=FALSE;
			// ɾ������Ŀ����
			for (i=0;i<lpPDAListCtrl->iItemNum;i++)
			{ // 
					lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i);
					FreeItem(lpMainItem,bText);
			}
			PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[0].plSubItemContent);
			lpPDAListCtrl->iItemNum=0;
			lpPDAListCtrl->iStartItem=0;
			lpPDAListCtrl->iFocusItem=-1;

			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE); // ���ù�������λ��
			SetVScrollBar(hWnd); // ���ù�����
/*			hdc=GetDC(hWnd);
			DrawListCtrl(hWnd,hdc);
			ReleaseDC(hWnd,hdc);
*/
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
			return TRUE;
}
/**************************************************
������static LRESULT DoDeleteColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ������
	IN lParam -- ����
����ֵ���ɹ�����TRUE�����򷵻�FALSE
����������ɾ��һ�У�����LVM_DELETECOLUMN��Ϣ��
����: 
************************************************/
static LRESULT DoDeleteColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		int  iCol;
//		LPLVCOLUMN lplvColumn;
		LPPDALISTCTRL lpPDAListCtrl;
//		int cbCaptionLen;
//		DWORD dwStyle;
		int Width,i;
//		LPVOID lpSubContent;
		
	KillEditControl(hWnd); // ɱ���༭�ؼ�
	lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
	if (lpPDAListCtrl==NULL)
		return FALSE;
	iCol=(int)wParam;
	if (iCol==0)
		return FALSE; // can't delete the main item
	if (iCol>=lpPDAListCtrl->iSubItemNum)
		return FALSE;
	if(!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_FILE))
	{// ������ļ����û������Ŀ���ݣ�����ɾ��ָ������
		if(!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_IMAGE))
			PtrListFreeAll(&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent);
		else
			PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent);
	}
	// �ƶ�������е�����
	memmove( &lpPDAListCtrl->lpSubItem[iCol], &lpPDAListCtrl->lpSubItem[iCol+1], (lpPDAListCtrl->iSubItemNum-iCol)*sizeof(SUBITEM) );
	lpPDAListCtrl->iSubItemNum--;	 // ����һ��
	// ���¼���ؼ��Ŀ��
	Width=0;
	for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
	{
		Width+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
	}
	lpPDAListCtrl->iWidth=Width;
	SetHScrollBar(hWnd);	// ����ˮƽ������

	return TRUE;
}
/**************************************************
������static LRESULT DoSetColumnWidth(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ���е�����
	IN lParam -- LOWORD �еĿ��
����ֵ����
��������������ָ���еĿ�ȣ�����LVM_SETCOLUMNWIDTH��Ϣ��
����: 
************************************************/
static LRESULT DoSetColumnWidth(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
{
	int iCol,cx,iSubItem;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;
			
			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iCol=(int)wParam;
			cx=LOWORD(lParam);
			if (iCol>=lpPDAListCtrl->iSubItemNum||iCol<0) return 0;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // �õ�ָ���е�����
			if (cx==LVSCW_AUTOSIZE)
			{ // ����Ϊ�Զ����
				cx=DEFAULTWIDTH;
			}
			if (cx==LVSCW_AUTOSIZE_USEHEADER)
			{ // ����Ϊ��ͷ����Ӧ�Ŀ��
				HDC hdc;
					hdc=GetDC(hWnd);
					cx=4;
					if (lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_TEXT)
						cx+=GetTextExtent(hdc,lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption, strlen(lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption));
					if (lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_IMAGE)
						cx+=16;
					ReleaseDC(hWnd,hdc);
			}
			lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth=cx; // ����Ϊָ�����
// !!! Add By Jami chen in 2003.07.31 
// if the Column wdith is change , then the Ctrl width will be change
		    lpPDAListCtrl->iWidth=0;
		    for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{
			   lpPDAListCtrl->iWidth+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
			}
// !!! Add End By Jami chen in 2003.07.31 
// !!! Add By Jami chen in 2003.08.08
			SetHScrollBar(hWnd);	
// !!! Add End By Jami chen in 2003.08.08
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
}
/**************************************************
������static LRESULT DoSetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ����
	IN lParam -- ����
����ֵ���ɹ�����ԭ���Ľ�����Ŀ���������򷵻�-1
�������������ý�������������LVM_SETHOTITEM��Ϣ��
����: 
************************************************/
static LRESULT DoSetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPPDALISTCTRL lpPDAListCtrl;
	 int iIndex,iOldFocus;

			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return -1;
			iIndex=(int)wParam; // �õ�����
			if (iIndex>lpPDAListCtrl->iItemNum)
				return -1;
			iOldFocus=lpPDAListCtrl->iFocusItem;
			lpPDAListCtrl->iFocusItem=iIndex; // ����ָ������Ϊ����
			return iOldFocus;
}
/**************************************************
������static LRESULT DoSetIconSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(CX,CY),Ҫ���õ�ˮƽ���ʹ�ֱ��ࡣ
����ֵ������ԭ����ˮƽ���ʹ�ֱ��ࡣ
��������������ͼ��֮��ļ�࣬����LVM_SETICONSPACING��Ϣ��
����: 
************************************************/
static LRESULT DoSetIconSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPPDALISTCTRL lpPDAListCtrl;
	 int cx,cy,iOldcx,iOldcy;
	
			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);  // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;

			cx=LOWORD(lParam);
			cy=HIWORD(lParam);

			iOldcx=lpPDAListCtrl->iHorzSpacing;
			iOldcy=lpPDAListCtrl->iVertSpacing;

			// ����ˮƽ���
			if (cx==-1)
				lpPDAListCtrl->iHorzSpacing=DEFAULTHORZSPACING;
			else
				lpPDAListCtrl->iHorzSpacing=cx;
			// ���ô�ֱ���
			if (cy==-1)
				lpPDAListCtrl->iVertSpacing=DEFAULTVERTSPACING;
			else
				lpPDAListCtrl->iVertSpacing=cy;
			return MAKELRESULT(iOldcx,iOldcy); // ���ؾɵļ��
}
/**************************************************
������static LRESULT DoEnsureVisible(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ������Ŀ����
	IN lParam -- BOOL ���ֿɼ���־
����ֵ����
������������ָ֤����Ŀ�ɼ�������LVM_ENSUREVISIBLE��Ϣ��
����: 
************************************************/
static LRESULT DoEnsureVisible(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iItem;
	BOOL bPartialOK;
	LPPDALISTCTRL lpPDAListCtrl;
	int iTop,iBottom,iLineHeight,iItemNumInLine,iTopCoordinate;
	RECT rect;
	int iScrollLine;
	
			
		KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);  // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam;
			bPartialOK=(BOOL)lParam; // ���ֿɼ�OK
			GetClientRect(hWnd,&rect);
			

			if (ExistCaption(hWnd)==TRUE)
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT;
			else
				iTopCoordinate=0;

			iLineHeight=GetLineHeight(hWnd); // �õ���Ŀ�߶�
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�е���Ŀ��
			iTop=iTopCoordinate+((iItem-lpPDAListCtrl->iStartItem)/iItemNumInLine)*iLineHeight;
			iBottom=iTop+iLineHeight;
			iScrollLine=0;
			if (iItem<lpPDAListCtrl->iStartItem)
			{// �õ���Ҫ����������
				iScrollLine=(iItem-lpPDAListCtrl->iStartItem-(iItemNumInLine-1))/iItemNumInLine;
			}
			if (bPartialOK)
			{
				if (iTop>rect.bottom)
				{
					iScrollLine=(iTop-rect.bottom+iLineHeight-1)/iLineHeight;
				}
			}
			else
			{
				if (iBottom>rect.bottom)
				{
					iScrollLine=(iBottom-rect.bottom+iLineHeight-1)/iLineHeight;
				}
			}
			if (iScrollLine)
				VScrollWindow(hWnd,iScrollLine); // ��������
			return 0;
}
/**************************************************
������static LRESULT DoFindItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��ʼ��Ŀ����
	IN lParam -- ָ��FINDINFO��ָ��
����ֵ���ҵ���Ŀ��������Ŀ�����������򷵻�-1
��������������ָ������Ŀ������LVM_FINDITEM��Ϣ��
����: 
************************************************/
static LRESULT DoFindItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{

	LPFINDINFO lplvFindInfo;
	int iIndex,iStart;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	LPTSTR lpListString;
	char lpFileFeild[FIELDMAXLEN];
//    LVITEM lvItem; 
	int i;
	int iNeedStart=FALSE;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return -1;
			iStart=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lplvFindInfo=(LPFINDINFO)lParam; // �õ����ҽṹ

			if (iStart<0)
				iIndex=0;
			else
			{
				iIndex=iStart+1;  //The specified item is itself excluded from the search. 
				iNeedStart=TRUE;
			}
			// ����ָ������Ŀ
			while(1)
			{
				for (;iIndex<lpPDAListCtrl->iItemNum;iIndex++)
				{
						lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iIndex);
						if (lpMainItem==NULL)
							continue;
						if (lplvFindInfo->flags&LVFI_PARAM)
						{
							if (lplvFindInfo->lParam!=lpMainItem->lParam)
								continue; // ����������
						}
						if (lplvFindInfo->flags&LVFI_STRING)
						{
							for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
							{
								if (i>0&&lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_IMAGE)
										continue; // ����������
								GetItemString(hWnd,lpPDAListCtrl,iIndex,i,lpFileFeild);
	//							GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,iCurSubItem,lpFileFeild);
								lpListString=lpFileFeild;
								if (lplvFindInfo->flags&LVFI_PARTIAL)
								{
									if (strstr(lpListString,lplvFindInfo->psz)==NULL)
										continue; // ����������
								}
								else
								{
									if (strcmp(lpListString,lplvFindInfo->psz)!=0)
										continue; // ����������

								}
							}
						}
						return iIndex; // ��������
//						if (lplvFindInfo->flags&LVFI_NEARESTXY)
				}
				if (iNeedStart==FALSE)
					break;
				if (lplvFindInfo->flags&LVFI_WRAP)
					iIndex=0;
				iNeedStart=FALSE;
			}
			return -1; // û���ҵ�
}
/**************************************************
������static LRESULT DoGetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ���е�����
	OUT lParam -- ָ��LVCOLUMN��ָ�룬���ָ���е�����
����ֵ���ɹ�����TRUE�����򷵻�FALSE��
�����������õ��е���Ϣ������LVM_GETCOLUMN��Ϣ��
����: 
************************************************/
static LRESULT DoGetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 int iCol,iSubItem;
	 LPLVCOLUMN lpCol;
	 LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iCol=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			
			lpCol=(LPLVCOLUMN)lParam; // �õ���������ݵ����ݽṹ
			if (iCol>=lpPDAListCtrl->iSubItemNum)
				return FALSE;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // �õ�����Ŀ������
			if (lpCol->mask&LVCF_TEXT)
			{ // �����ı�
				strncpy( lpCol->pszText,lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption,(lpCol->cchTextMax-1));
				lpCol->pszText[lpCol->cchTextMax-1]=0;
			}
			if (lpCol->mask&LVCF_WIDTH)
			{ // �õ����
				lpCol->cx=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			}
			if (lpCol->mask&LVCF_IMAGE)
			{ // �õ�ͼ������
					lpCol->iImage=lpPDAListCtrl->lpSubItem[iSubItem].iImage;
			}
			if (lpCol->mask&LVCF_FMT)
			{ // �õ����
				lpCol->fmt=lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt;
			}
			return TRUE;
}
/**************************************************
������static LRESULT DoGetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ����Ĵ�С
	IN lParam -- ָ��INT��ָ�����飬����еĴ���
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������õ��е���ʾ���򣬴���LVM_GETCOLUMNORDERARRAY��Ϣ��
����: 
************************************************/
static LRESULT DoGetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iCount, *lpiArray;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;

			iCount=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
			
			lpiArray=(int *)lParam; // �õ���Ŵ���ı��
			
			if (iCount>lpPDAListCtrl->iSubItemNum)
				iCount=lpPDAListCtrl->iSubItemNum;

			// ���Ƶ�ǰ�Ĵ���
			for (i=0;i<iCount;i++)
				lpiArray[i]=lpPDAListCtrl->lpColumnOrderArray[i];
			return TRUE;
}
/**************************************************
������static LRESULT DoGetColumnWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ���е�����
	IN lParam -- ����
����ֵ������ָ���еĿ��
�����������õ�ָ���еĿ�ȣ�����LVM_GETCOLUMNWIDTH��Ϣ��
����: 
************************************************/
static LRESULT DoGetColumnWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 int iCol,iSubItem;
	 LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iCol=(int)wParam;
			if (iCol>=lpPDAListCtrl->iSubItemNum)
				return 0;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // �õ�����Ŀ������
			return lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth; // �õ�����Ŀ�Ŀ��
}
/**************************************************
������static LRESULT DoGetCountPerPage(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ������ÿһҳ����Ŀ����
�����������õ�ÿһҳ����Ŀ����������LVM_GETCOUNTPERPAGE��Ϣ��
����: 
************************************************/
static LRESULT DoGetCountPerPage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		return GetPageItemNum(hWnd); // �õ�һҳ����Ŀ����
}

/**************************************************
������static LRESULT DoGetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ؽ�����Ŀ������
�����������õ�������Ŀ������������LVM_GETHOTITEM��Ϣ��
����: 
************************************************/
static LRESULT DoGetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return -1;
			return lpPDAListCtrl->iFocusItem; // ���ؽ������Ŀ����
}
/**************************************************
������static LRESULT DoGetImageList(HWND hWnd , WPARAM wParam , LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ͼ���б������
	IN lParam -- ����
����ֵ������ָ�����͵�ͼ���б���
�����������õ�ָ�����͵�ͼ���б���������LVM_GETIMAGELIST��Ϣ��
����: 
************************************************/
static LRESULT DoGetImageList(HWND hWnd , WPARAM wParam , LPARAM lParam)
{
	int iImageList;
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return (LRESULT)NULL;
			iImageList=(int)wParam;
			if (iImageList==LVSIL_NORMAL)
				return (LRESULT)lpPDAListCtrl->lpImageList; // ����ͨ��ͼ���б�

			if (iImageList==LVSIL_SMALL)
				return (LRESULT)lpPDAListCtrl->lpImageListSmall; // ����Сͼ���б�

			return (LRESULT)NULL;
}
/**************************************************
������static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ָ��LVITEM�ĵ�ָ��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������õ�ָ����Ŀ���ݣ�����LVM_GETITEM��Ϣ��
����: 
************************************************/
static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPLVITEM pItem;
	LPMAINITEM lpMainItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			
			pItem=(LPLVITEM)lParam; // �õ���Ŀ�ṹ
			if (pItem->iSubItem==0)
			{ // �õ�����Ŀ������
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[pItem->iSubItem].plSubItemContent,pItem->iItem); // �õ�����Ŀ�ṹ
						if (lpMainItem==NULL)
							return FALSE;
						if (pItem->mask&LVIF_TEXT)
						{ // �����ı�
// !!! Add By Jami Chen 2002.04.01
							if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
							{
								pItem->pszText=lpMainItem->lpText;
							}
							else
// !!! Add End By Jami Chen 2002.04.01
							{
								strncpy(pItem->pszText,lpMainItem->lpText,(pItem->cchTextMax-1));
								pItem->pszText[pItem->cchTextMax-1]=0;
							}
						}
						if (pItem->mask&LVIF_IMAGE)
						{ // �õ�ͼ������
							pItem->iImage=lpMainItem->iImage;
						}
						if (pItem->mask&LVIF_PARAM)
						{ // �õ���Ŀ����
							pItem->lParam=lpMainItem->lParam;
						}
						if (pItem->mask&LVIF_STATE)
						{ // �õ���Ŀ״̬
							pItem->state=lpMainItem->State&pItem->stateMask;
						}
			}
			else
			{ // �õ�����Ŀ����
					pItem->state=0;
					pItem->lParam=0;
					if ((pItem->mask&LVIF_IMAGE)&&(lpPDAListCtrl->lpSubItem[pItem->iSubItem].ContentFmt&LVCFMT_IMAGE))
					{// �õ�ͼ������
							pItem->pszText[0]=0;
							pItem->iImage=(int)PtrListAt(&lpPDAListCtrl->lpSubItem[pItem->iSubItem].plSubItemContent,pItem->iItem);
					}
					else
					{ // �õ��ı�
						if (pItem->mask&LVIF_TEXT)
						{ // ��Ŀ���ı�
// !!! Add By Jami Chen 2002.04.01
							if (lpPDAListCtrl->lpSubItem[pItem->iSubItem].ContentFmt&LVCFMT_FILE)
							{
								// the LVCFMT_FILE ,sub item is not any  message ,it's empty
//								pItem->pszText=lpMainItem->lpText;
//								pItem->pszText=0;
							}
							else
// !!! Add End By Jami chen 2002.04.01
							{ // �����ı�
								LPTSTR lpString;
								lpString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[pItem->iSubItem].plSubItemContent,pItem->iItem);
								if (lpString == NULL)
								{ // ������
									pItem->pszText[0]=0;
								}
								else
								{ // ���Ƶ�ǰ����
									strncpy(pItem->pszText,lpString,(pItem->cchTextMax-1));
									pItem->pszText[pItem->cchTextMax-1]=0;
								}
							}
						}
						else
						{
							pItem->iImage=0;
							pItem->pszText=NULL;
						}
					}
			}
			return TRUE;
}
/**************************************************
������static LRESULT DoGetItemCount(HWND hWnd,WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ص�ǰ�ؼ�����Ŀ����
�����������õ���ǰ�ؼ�����Ŀ����������LVM_GETITEMCOUNT��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemCount(HWND hWnd,WPARAM wParam ,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			return 	lpPDAListCtrl->iItemNum; // ������Ŀ����
}
/****************************************************************************************************/
// Process Message --- LVM_GETITEMPOSITION 
//    wParam = (WPARAM) (int) i; 
//    lParam = (LPARAM) (POINT FAR *) ppt; 
// Retrieves the position of a list view item. You can send this message explicitly or by 
// using the ListView_GetItemPosition macro. 

// i 
//     Index of the list view item. 
// ppt 
//    Address of aPOINT structure that receives the position of the item's upper-left corner, 
//    in view coordinates. 

// Returns TRUE if successful, or FALSE otherwise. 
/****************************************************************************************************/
/*
static LRESULT DoGetItemPosition(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPPOINT lppt;
	int iItem;
	int iLineHeight,iTopCoordinate,iItemNumInLine,iDrawLine,iWindowWidth;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItem=(int)wParam;
			lppt=(LPPOINT)lParam;
			if(iItem>=lpPDAListCtrl->iItemNum)
				return FALSE;
			if (ExistCaption(hWnd)==TRUE)
				iTopCoordinate=CAPTIONHEIGHT;
			else
				iTopCoordinate=0;

			iLineHeight=GetLineHeight(hWnd);
			iItemNumInLine=GetItemNumInLine(hWnd);
			iItem-=lpPDAListCtrl->iStartItem;
			iWindowWidth=GetWindowWidth(hWnd);
			iDrawLine=iItem/iItemNumInLine;
			lppt->x=lpPDAListCtrl->cx+(iWindowWidth/iItemNumInLine)*(iItem%iItemNumInLine);
			lppt->y=iDrawLine*iLineHeight+iTopCoordinate;
			return TRUE;
}
*/
/**************************************************
������static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ������
	IN lParam -- LPRECT �����Ŀ�ľ��δ�С
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������õ�ָ����Ŀ�ľ��δ�С������LVM_GETITEMRECT��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPRECT lprc;
	int iItem;
	int iLineHeight,iTopCoordinate,iItemNumInLine,iDrawLine,iWindowWidth;
	DWORD dwStyle;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItem=(int)wParam; // �õ���Ŀ����

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lprc=(LPRECT)lParam;
			if(iItem>=lpPDAListCtrl->iItemNum)
				return FALSE;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if (ExistCaption(hWnd)==TRUE) // ����CAPTION
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
			else
				iTopCoordinate=0;

			iLineHeight=GetLineHeight(hWnd); // �õ���Ŀ�߶�
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�û�е���Ŀ����
			iItem-=lpPDAListCtrl->iStartItem;
			iWindowWidth=GetWindowWidth(hWnd); // �õ����ڿ��
			iDrawLine=iItem/iItemNumInLine;
			// �õ���Ŀ�ľ���
			lprc->left=lpPDAListCtrl->cx+(iWindowWidth/iItemNumInLine)*(iItem%iItemNumInLine);
			lprc->top=iDrawLine*iLineHeight+iTopCoordinate;
			lprc->bottom=lprc->top+iLineHeight;
			if ( (dwStyle&LVS_TYPEMASK)==LVS_REPORT)
			{
				lprc->right=lprc->left+lpPDAListCtrl->iWidth;
			}
			else
			{
				lprc->right=lprc->left+(iWindowWidth/iItemNumInLine);
			}
			return TRUE;
}
/**************************************************
������static LRESULT DoGetItemSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- BOOL �Ƿ���Сͼ��ı�־
	IN lParam -- ����
����ֵ��LOWORD����ˮƽ��࣬HIWORD���ش�ֱ���
�����������õ���Ŀ��ļ�࣬����LVM_GETITEMSPACING��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	BOOL fSmall;
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			fSmall=(BOOL)wParam;
			if (fSmall)
				return 0;
			return MAKELRESULT(lpPDAListCtrl->iHorzSpacing,lpPDAListCtrl->iVertSpacing); // ����ˮƽ�ʹ�ֱ���
}
/**************************************************
������static LRESULT DoGetItemState(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ������
	IN lParam -- UINT Ҫ�õ���Ŀ��״̬��������
����ֵ������ָ����Ŀ��״̬
�����������õ�ָ����Ŀ��״̬������LVM_GETITEMSTATE��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemState(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem;
		UINT mask;
		LPMAINITEM lpMainItem;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam; // �õ���Ŀ����
			mask=(UINT)lParam;
			if (iItem>=lpPDAListCtrl->iItemNum)
				return 0;
			lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ����
			if (lpMainItem==NULL)
				return 0;
			return (mask&lpMainItem->State); // ���ص�ǰ��Ŀ��ָ��״̬
}
/**************************************************
������static LRESULT DoGetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- LPLVITEM ־��LVITEM��ָ��
����ֵ����
�����������õ�ָ����Ŀ���ı�������LVM_GETITEMTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem;
		LPMAINITEM lpMainItem;
    LPLVITEM pitem; 
		LPTSTR lpSubItemString;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam; // �õ���Ŀ����

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			
			pitem=(LPLVITEM)lParam; // �õ���Ŀָ��

			if (iItem < 0)
				return 0;
			if (iItem>=lpPDAListCtrl->iItemNum)
				return 0;
			if (pitem->iSubItem>=lpPDAListCtrl->iSubItemNum)
				return 0;
			lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ
			if (lpMainItem==NULL)
				return 0;
			if (pitem->iSubItem==0)
			{ // Ҫ�õ�����Ŀ����
				if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
				{ // �õ�FILE NO��
					pitem->pszText=lpMainItem->lpText;
				}
				else
				{ // �õ���Ŀ�ı�
					strncpy( pitem->pszText,lpMainItem->lpText,(pitem->cchTextMax-1));
					pitem->pszText[pitem->cchTextMax-1]=0;
				}
			}
			else
			{ // �õ�����Ŀ����
					if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_FILE)
					{ // �õ�FILE NO��
						pitem->pszText=lpMainItem->lpText;
					}
					else
					{ // �õ�ָ�����ı�����
						if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_IMAGE)
							pitem->pszText=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[pitem->iSubItem].plSubItemContent,iItem);
						else
						{
							lpSubItemString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[pitem->iSubItem].plSubItemContent,iItem);
							strncpy( pitem->pszText,lpSubItemString,(pitem->cchTextMax-1));
							pitem->pszText[pitem->cchTextMax-1]=0;
						}
					}
			}
			return 0;
}
/**************************************************
������static LRESULT DoGetNextItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��ʼ���ҵ���Ŀ����
	IN lParam -- LOWORD ������Ŀ������
����ֵ���ɹ������ҵ���Ŀ�����������򷵻� -1
����������������һ����Ŀ������LVM_GETNEXTITEM��Ϣ��
����: 
************************************************/
static LRESULT DoGetNextItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		int iStart;
		int iItem;
		UINT flags;
		int iItemNumInLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return -1;
			iStart=(int)wParam;
			flags=LOWORD(lParam);
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ��
			if (iStart==-1)
				iItem=0;
			else
				iItem=iStart;

			if (iItem>=lpPDAListCtrl->iItemNum||iItem<0)
				return -1;

			while(1)
			{
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ�ָ����Ŀ������
				if (lpMainItem==NULL)
					return -1;
				if (flags&LVNI_SELECTED)
				{ // Ҫ����ѡ����Ŀ
					if ((lpMainItem->State&LVIS_SELECTED)==0)
						goto _NEXTITEM;// û���ҵ�
				}
				if (flags&LVNI_FOCUSED)
				{ // ������Ŀ
					if ((lpMainItem->State&LVIS_FOCUSED)==0)
						goto _NEXTITEM; // û���ҵ�
				}
				break;

_NEXTITEM:  // ������һ����Ŀ
/*				if (iStart==-1)
				{
					iItem++;
					if (iItem>=lpPDAListCtrl->iItemNum)
						return -1;
				}
				else	*/
				if (flags&LVNI_ABOVE)
				{ // ���ϲ���
					iItem-=iItemNumInLine;
					if (iItem<0)
						return -1;
				}
				else if (flags&LVNI_BELOW)
				{ // ���²���
					iItem+=iItemNumInLine;
					if (iItem>=lpPDAListCtrl->iItemNum)
						return -1;
				}
				else if (flags&LVNI_TOLEFT)
				{ // ��ǰ����
					if ((iItem%iItemNumInLine)==0)
						return -1;
					iItem--;
				}
				else if (flags&LVNI_TORIGHT)
				{ // �������
					if ((iItem%iItemNumInLine)==(iItemNumInLine-1))
						return -1;
					iItem++;
				}
				else
				{ // ����һ����Ŀ
					iItem++;
					if (iItem>=lpPDAListCtrl->iItemNum)
						return -1;
				}
			}
			return iItem;
}
/**************************************************
������static LRESULT DoGetOrigin(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPPOINT ���ԭ�������ָ��
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE��
�����������õ��ؼ�ԭ������꣬����LVM_GETORIGIN��Ϣ��
����: 
************************************************/
static LRESULT DoGetOrigin(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPPOINT lpptOrg;
	int iLineHeight,iItemNumInLine;
	DWORD dwStyle,dwTypeMask;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
			
			lpptOrg=(LPPOINT)lParam;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			dwTypeMask=dwStyle&LVS_TYPEMASK; // �õ���������

			iLineHeight=GetLineHeight(hWnd); // �õ��и߶�
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�һ�е���Ŀ��
			// �õ�ԭʼ�������
			lpptOrg->x=lpPDAListCtrl->cx;
			lpptOrg->y=((0-lpPDAListCtrl->iStartItem)/iItemNumInLine)*iLineHeight;

			if (dwTypeMask==LVS_REPORT||dwTypeMask==LVS_LIST)
				return FALSE;
			return TRUE;
}
/**************************************************
������static LRESULT DoGetSelectedCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ������ѡ����Ŀ����Ŀ
�����������õ���ǰ�Ѿ�ѡ�����Ŀ�ĸ���������LVM_GETSELECTEDCOUNT��Ϣ��
����: 
************************************************/
static LRESULT DoGetSelectedCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int i,iSelectedCount=0;
	LPMAINITEM lpMainItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			for (i=0;i<lpPDAListCtrl->iItemNum;i++)
			{  // �õ�ѡ����Ŀ�ĸ���
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i); // �õ���ǰ��Ŀ������
				if (lpMainItem)
				{
					if (lpMainItem->State&LVIS_SELECTED) // ����Ŀ��ѡ����Ŀ
						iSelectedCount++;
				}
			}
			return iSelectedCount;
}
/**************************************************
������static LRESULT DoGetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ������ѡ����Ŀ������
�����������õ�ѡ����Ŀ������������LVM_GETSELECTIONMARK��Ϣ��
����: 
************************************************/
static LRESULT DoGetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return SendMessage(hWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED); // �õ���һ��ѡ����Ŀ
}

/**************************************************
������static LRESULT DoSetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ��Ҫ����Ϊѡ�����Ŀ����
	IN lParam -- ����
����ֵ��������ǰ��ѡ����Ŀ������
��������������ָ����ĿΪѡ����Ŀ������LVM_SETSELECTIONMARK��Ϣ��
����: 
************************************************/
static LRESULT DoSetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iIndex ,iOldIndex;
    LVITEM item; 

		iIndex = (int)lParam;
		iOldIndex = SendMessage(hWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED); // �õ���һ��ѡ����Ŀ
		item.mask = LVIF_STATE;
		item.iItem = iIndex;
		item.iSubItem = 0;
		item.state = LVIS_SELECTED ;
		item.stateMask = LVIS_SELECTED ;
//		SendMessage(hWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
//		ClearFocus(hWnd);
		ClearAllInvert(hWnd); // ������е�ѡ����Ŀ
		SendMessage(hWnd,LVM_SETITEMSTATE,iIndex,(LPARAM)&item); // ������Ŀ״̬
		return iOldIndex;
}

/**************************************************
������static LRESULT DoGetStringWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTSTR ָ�����ַ���
����ֵ������ָ���ַ����Ŀ��
�����������õ�ָ���ַ����Ŀ�ȣ�����LVM_GETSTRINGWIDTH��Ϣ��
����: 
************************************************/
static LRESULT DoGetStringWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTSTR psz;
	HDC hdc;
	int iStringWidth;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

		psz=(LPCTSTR)lParam; // �õ��ִ�ָ��
		hdc=GetDC(hWnd);
		iStringWidth=GetTextExtent(hdc,psz,strlen(psz)); // �õ�ָ���ַ����Ŀ��
		ReleaseDC(hWnd,hdc);
		return iStringWidth;
}
/**************************************************
������static LRESULT DoGetSubItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ָ����Ŀ
	IN/OUT lParam -- LPRECT��ָ����Ŀ�ľ��δ�С��
					 ����top��Ҫ�õ�λ�õ�����Ŀ��������1��ʼ��left �ǲ鿴����
����ֵ����
�����������õ�ָ������Ŀ�ľ��δ�С������LVM_GETSUBITEMRECT��Ϣ��
����: 
************************************************/
static LRESULT DoGetSubItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iItem,iSubItem,iCurSubItem;
	UINT flags;
	LPRECT lpRect;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;
 
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;

			iItem=(int)wParam; // �õ���Ŀ����
			if (iItem<0||iItem>=lpPDAListCtrl->iItemNum)
				return 0;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lpRect=(LPRECT)lParam;
			iSubItem=(int)lpRect->top-1;
			flags=lpRect->left;
			if (iSubItem<0||iSubItem>=lpPDAListCtrl->iSubItemNum)
				return 0;
			SendMessage(hWnd,LVM_GETITEMRECT,(WPARAM)iItem,(LPARAM)lpRect); // �õ���Ŀ�ľ���

			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // ����ָ��������Ŀ
				iCurSubItem=lpPDAListCtrl->lpColumnOrderArray[i]; // �õ�����Ŀ������
				if (iCurSubItem==iSubItem)
				{ // �ǵ�ǰҪ������Ŀ
					if (flags==LVIR_ICON)
					{  // ICON ģʽ
						if (lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth>lpPDAListCtrl->sizeSmallIcon.cx)
						{ // ��Ŀ��СΪICON�Ĵ�С
							lpRect->right=lpRect->left+lpPDAListCtrl->sizeSmallIcon.cx;
							return TRUE;
						}
					}
					// ��СΪ��Ŀ���
					lpRect->right=lpRect->left+lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
					return TRUE;
				}
				// �õ���һ����Ŀ�Ŀ�ʼλ��
				lpRect->left+=lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
			}
			return 0;
}
/**************************************************
������static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ص�һ���ɼ���Ŀ������
�����������õ���һ���ɼ���Ŀ������������LVM_GETTOPINDEX��Ϣ��
����: 
************************************************/
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			return lpPDAListCtrl->iStartItem; // �õ���ǰҳ�Ŀ�ʼ��Ŀ
}
/**************************************************
������static LRESULT DoGetViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPRECT �����ͼ���εĽṹ
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������õ��ؼ�����ͼ��С������LVM_GETVIEWRECT��Ϣ��
����: 
************************************************/
static LRESULT DoGetViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPRECT prc;
		int iTotalLine,iLineHeight;
		LPPDALISTCTRL lpPDAListCtrl;
		DWORD dwStyle;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			
#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			prc=(LPRECT)lParam;
			prc->left=lpPDAListCtrl->cx;
			iLineHeight=GetLineHeight(hWnd); // �õ���Ŀ�߶�
			iTotalLine=GetTotalLine(hWnd); // �õ�������
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			// �õ��ؼ����
			if ((dwStyle&LVS_TYPEMASK)==LVS_REPORT)
				prc->right=prc->left+lpPDAListCtrl->iWidth;
			else
				prc->right=prc->left+GetWindowWidth(hWnd);;

			// �õ��ؼ��߶�
			prc->top=0;
			if (ExistCaption(hWnd))
			{
				prc->top+=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT;
			}
			prc->top-=lpPDAListCtrl->iStartItem*iLineHeight;
			prc->bottom=prc->top+iTotalLine*iLineHeight;
			return TRUE;
}
/**************************************************
������static LRESULT DoRedrawItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT Ҫ�ػ����Ŀ��ʼ����
	IN lParam -- INT Ҫ�ػ����Ŀ�Ľ�������
����ֵ����
�����������ػ�ָ������Ŀ������LVM_REDRAWITEMS��Ϣ��
����: 
************************************************/
static LRESULT DoRedrawItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iFirst,iLast,i;

		KillEditControl(hWnd); // ɱ���༭�ؼ�
		iFirst=(int)wParam;
		iLast=(int)lParam;
// !!! modified By Jami chen in 2002.04.25
//			for (i=iFirst;i<iLast;i++)
			for (i=iFirst;i<=iLast;i++)
// !!! modified End By Jami chen in 2002.04.25
			{
				DrawItem(hWnd,i); // ����ָ������Ŀ
			}
			return TRUE;
}


/**************************************************
������static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ˮƽ�����Ĵ�С
	IN lParam -- INT ��ֱ�����Ĵ�С
����ֵ����
�����������������ڣ�����LVM_SCROLL��Ϣ��
����: 
************************************************/
static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   int dx,dy;
	 int iVScrollLine,iHScrollLine;

			KillEditControl(hWnd); // ɱ���༭�ؼ�
			dx=(int)wParam;
			dy=(int)lParam;

			iVScrollLine=dy/GetLineHeight(hWnd);
			iHScrollLine=dx/WIDTHSTEP;

			HScrollWindow(hWnd,iHScrollLine); // ˮƽ��������
			VScrollWindow(hWnd,iVScrollLine); // ��ֱ��������
			return TRUE;
}
/**************************************************
������static LRESULT DoSetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT Ҫ���õ��е�����
	IN lParam -- LPLVCOLUMN Ҫ���õ��е�����
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��������������ָ�����У�����LVM_SETCOLUMN��Ϣ��
����: 
************************************************/
static LRESULT DoSetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 int iCol,iSubItem;
	 LPLVCOLUMN lpCol;
	 LPPDALISTCTRL lpPDAListCtrl;

			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iCol=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			lpCol=(LPLVCOLUMN)lParam; // �õ�������
			if (iCol>=lpPDAListCtrl->iSubItemNum)
				return FALSE;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // �õ�ָ���е�����Ŀ����
			if (lpCol->mask&LVCF_TEXT)
			{ // �ı���Ч
				LPTSTR lpTemp;
				int  cbCaptionLen;
					// ����CAPTION�ı�
					cbCaptionLen=strlen(lpCol->pszText);
					lpTemp=(LPTSTR)malloc(cbCaptionLen+1);
					if (lpTemp)
					{
						if (lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption)
							free(lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption);
						lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption=lpTemp;
					}
					else
					{
						return FALSE;
					}
					lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption=lpTemp;
					strcpy(lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption,lpCol->pszText);
			}
			if (lpCol->mask&LVCF_WIDTH)
			{ // �����Ч�������еĿ��
				lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth=lpCol->cx;
// Add By Jami chen in 2003.07.31 
// if the Column wdith is change , then the Ctrl width will be change
				lpPDAListCtrl->iWidth=0;
				for (iCol=0;iCol<lpPDAListCtrl->iSubItemNum;iCol++)
				{
				   lpPDAListCtrl->iWidth+=lpPDAListCtrl->lpSubItem[iCol].iSubItemWidth;
				}
// Add End By Jami chen in 2003.07.31 
			}
			if (lpCol->mask&LVCF_IMAGE)
			{ // ͼ����Ч
					lpPDAListCtrl->lpSubItem[iSubItem].iImage=lpCol->iImage;
			}
			if (lpCol->mask&LVCF_FMT)
			{ // �����Ч
				lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt=lpCol->fmt;
			}
			return TRUE;
}
/****************************************************************************************************/
/**************************************************
������static LRESULT DoSetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT Ҫ���е��еĸ���
	IN lParam -- LPINT ���еĴ���
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
���������������е���ʾ���򣬴���LVM_SETCOLUMNORDERARRAY��Ϣ��
����: 
************************************************/
static LRESULT DoSetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iCount, *lpiArray;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;

			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;

			iCount=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lpiArray=(int *)lParam;
			
			if (iCount>lpPDAListCtrl->iSubItemNum)
				iCount=lpPDAListCtrl->iSubItemNum;
			// ��������ʾ����
			for (i=0;i<iCount;i++)
				lpPDAListCtrl->lpColumnOrderArray[i]=lpiArray[i];
			return TRUE;
}
/**************************************************
������static LRESULT DoSetItemCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT Ҫ���õ���Ŀ����
	IN lParam -- ����
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
�������������ÿؼ�����Ŀ����������LVM_SETITEMCOUNT��Ϣ��
����: 
************************************************/
static LRESULT DoSetItemCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItems,iSubItem;
			
			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iItems=(int)wParam; // �õ���Ŀ����
			if (iItems<lpPDAListCtrl->iMaxItemNum)
				return 0;
			for (iSubItem=0;iSubItem<lpPDAListCtrl->iSubItemNum;iSubItem++)
			{ // ������Ŀ����
				PtrListSetLimit(&lpPDAListCtrl->lpSubItem[iSubItem].plSubItemContent,iItems);
			}
			return 1;
}
/**************************************************
������static LRESULT DoSetItemState(HWND hWnd ,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ����
	IN lParam -- LPLVITEM ָ����Ŀ�����ݣ�state �� stateMask ��Ч
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
����������������Ŀ״̬������LVM_SETITEMSTATE��Ϣ��
����: 
************************************************/
static LRESULT DoSetItemState(HWND hWnd ,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItems,iCurItem;
		LPLVITEM pItem;
		LPMAINITEM lpMainItem;
			
			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItems=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			pItem=(LPLVITEM)lParam; // �õ���Ŀ�ṹ
			
			if (iItems>=lpPDAListCtrl->iItemNum&&iItems<-1)
				return FALSE;
			if (pItem==NULL)
				return FALSE;

			if (iItems==-1)
			{ // �������е���Ŀ
				for (iCurItem=0;iCurItem<lpPDAListCtrl->iItemNum;iCurItem++) 
				{ // ѭ������������Ŀ
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem);
					if (lpMainItem)
					{
//						lpMainItem->State=pItem->state&pItem->stateMask;
						lpMainItem->State &= ~pItem->stateMask; // clear Mask Bit
						lpMainItem->State |=pItem->state&pItem->stateMask;  // Set Maxk Bit
						DrawItem(hWnd,iCurItem); // ���Ƹ���Ŀ
					}
				}
			}
			else
			{ // ����ָ������Ŀ
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItems); // �õ���Ŀ����
					if (lpMainItem)
					{ // ������Ŀ��״̬
						lpMainItem->State &= ~pItem->stateMask; // clear Mask Bit
						lpMainItem->State |=pItem->state&pItem->stateMask;  // Set Maxk Bit
						if (lpPDAListCtrl->iFocusItem != -1)
						{
							ClearFocus(hWnd); // �������
							lpPDAListCtrl->iActiveItem=lpPDAListCtrl->iFocusItem=iItems; // �������ý���
						}
						DrawItem(hWnd,iItems); // ������Ŀ
					}
			}
			return TRUE;
}
/**************************************************
������static LRESULT DoSetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ����
	IN lParam -- LPLVITEM ָ����Ŀ������
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
����������������Ŀ�ı�������LVM_SETITEMTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoSetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem;
		LPMAINITEM lpMainItem;
    LPLVITEM pitem; 
		LPVOID lpReallocPtr;
		int cbStringLen;
			
			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItem=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			pitem=(LPLVITEM)lParam; // �õ���Ŀ�ṹ

			if (pitem==NULL)
				return FALSE;

			if (iItem>=lpPDAListCtrl->iItemNum)
				return FALSE;
			if (pitem->iSubItem>=lpPDAListCtrl->iSubItemNum)
				return FALSE;
			if (pitem->iSubItem==0)
			{ // ��ǰҪ���õ�������Ŀ
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ����
				if (lpMainItem==NULL)
					return FALSE;
				if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
				{ // ���ļ����
					lpMainItem->lpText=pitem->pszText; // ����FILE NO
				}
				else
				{ // ��һ���ı���� �����ı�
					cbStringLen=strlen(pitem->pszText);
					lpReallocPtr=realloc(lpMainItem->lpText,cbStringLen+1);
					if (lpReallocPtr==NULL)
						return FALSE;
					lpMainItem->lpText=lpReallocPtr;
					strcpy(lpMainItem->lpText,pitem->pszText);
				}
			}
			else
			{ // ������Ŀ
					if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_FILE)
					{ // �ļ����û������Ŀ����
						return FALSE;
					}
					else
					{ // ��������Ŀ����
						if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_IMAGE)
						{ // ����ͼ��
							PtrListAtPut(&lpPDAListCtrl->lpSubItem[pitem->iSubItem].plSubItemContent,iItem,pitem->pszText);
						}
						else
						{ // �����ı�
							LPTSTR lpSubContent;
							lpSubContent=PtrListAt(&lpPDAListCtrl->lpSubItem[pitem->iSubItem].plSubItemContent,iItem);
							cbStringLen=strlen(pitem->pszText);
							lpReallocPtr=(LPVOID)realloc((LPVOID)lpSubContent,cbStringLen+1);
							if (lpReallocPtr==NULL)
								return FALSE;
							strcpy(lpReallocPtr,pitem->pszText);

							PtrListAtPut(&lpPDAListCtrl->lpSubItem[pitem->iSubItem].plSubItemContent,iItem,lpReallocPtr);
						}
					}
			}
			return TRUE;
}
/**************************************************
������static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN/OUT lParam -- LPLVHITTESTINFO ���Խṹ��Ϣ
����ֵ�����ص�����Ӧ����Ŀ����
�������������Ե��λ�ã�����LVM_HITTEST��Ϣ��
����: 
************************************************/
// !!! Add By Jami chen 2002.04.25
static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	LPLVHITTESTINFO pInfo;
	int iItem,iRow,iColumn,iCount;
	int iTopCoordinate;
	POINTS pts;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

		pInfo=(LPLVHITTESTINFO)lParam; // �õ�����Խṹ
		SendMessage(hWnd,LVM_GETVIEWRECT,(WPARAM)0,(LPARAM)&rect); // �õ��ؼ�����

		pInfo->iItem=-1;
		pInfo->iSubItem=-1;
		if (PtInRect(&rect,pInfo->pt)==FALSE)
			return -1; // ָ���㲻�ڿؼ���
//		iLineHeight=GetLineHeight(hWnd);
		pts.x=(int)pInfo->pt.x;
		pts.y=(int)pInfo->pt.y;
		GetPointPosition(hWnd,pts,&iRow,&iColumn); // �õ������ڵ��к���
		iItem=iRow*GetItemNumInLine(hWnd)+iColumn; // �õ���Ŀ����
		iCount=(int)SendMessage(hWnd,LVM_GETITEMCOUNT,0,0); // �õ���Ŀ����
		pInfo->flags=0;
		if (iItem>iCount)
			pInfo->flags|=LVHT_NOWHERE; // ����Ŀ������
		else
			pInfo->iItem=iItem; // ��Ŀ����
		if (ExistCaption(hWnd))
			iTopCoordinate=GetCaptionHeight(hWnd,NULL);//CAPTIONHEIGHT;
		else
			iTopCoordinate=0;
		if (pInfo->pt.y<iTopCoordinate)
			pInfo->flags|=LVHT_ABOVE; // ������
		if (pInfo->pt.y>GetWindowHeight(hWnd))
			pInfo->flags|=LVHT_BELOW; // ������
		if (pInfo->pt.x<0)
			pInfo->flags|=LVHT_TOLEFT; // ������
		if (pInfo->pt.x>GetWindowWidth(hWnd))
			pInfo->flags|=LVHT_TORIGHT; // ���ұ�
		return iItem;
}
// !!! Add End By Jami chen 2002.04.25
/**************************************************
������static LRESULT DoSubItemHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN/OUT lParam -- LPLVHITTESTINFO ���Խṹ��Ϣ
����ֵ�����ص�����Ӧ����Ŀ����
�������������Ե����ڵ������λ�ã�����LVM_SUBITEMHITTEST��Ϣ��
����: 
************************************************/
static LRESULT DoSubItemHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	LPLVHITTESTINFO pInfo;
	int iItem,iRow,iColumn,iCount;
	int iTopCoordinate;
	POINTS pts;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif

		pInfo=(LPLVHITTESTINFO)lParam; // �õ����Ե�ṹ
		SendMessage(hWnd,LVM_GETVIEWRECT,(WPARAM)0,(LPARAM)&rect); // �õ��ؼ��ľ���

		pInfo->iItem=-1;
		pInfo->iSubItem=-1;
		if (PtInRect(&rect,pInfo->pt)==FALSE)
			return -1; // ���ڿ�����
//		iLineHeight=GetLineHeight(hWnd);
		pts.x=(int)pInfo->pt.x;
		pts.y=(int)pInfo->pt.y;
		GetPointPosition(hWnd,pts,&iRow,&iColumn); // �õ�����к���
		iItem=iRow*GetItemNumInLine(hWnd)+iColumn; // �õ���Ŀ����
		iCount=(int)SendMessage(hWnd,LVM_GETITEMCOUNT,0,0); // �õ���Ŀ����
		pInfo->flags=0;
		if (iItem>iCount)
			pInfo->flags|=LVHT_NOWHERE;
		else
			pInfo->iItem=iItem;
// !!! Add By Jami chen 2002.04.25
		pInfo->iSubItem=GetSubItemPointPosition(hWnd,pts); // �õ�����Ŀ������
// !!! Add End By Jami chen 2002.04.25

		if (ExistCaption(hWnd)) // �Ƿ����CAPTION
			iTopCoordinate=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT;
		else
			iTopCoordinate=0;
		if (pInfo->pt.y<iTopCoordinate)
			pInfo->flags|=LVHT_ABOVE; // ������
		if (pInfo->pt.y>GetWindowHeight(hWnd))
			pInfo->flags|=LVHT_BELOW; // ������
		if (pInfo->pt.x<0)
			pInfo->flags|=LVHT_TOLEFT; // ������
		if (pInfo->pt.x>GetWindowWidth(hWnd))
			pInfo->flags|=LVHT_TORIGHT; // ������
		return iItem;
}
/**************************************************
������static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
��������������ؼ����ݣ�����WM_CLEAR��Ϣ��
����: 
************************************************/
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iCurItem;
		LPMAINITEM lpMainItem;
			
			KillEditControl(hWnd); // ɱ���༭�ؼ�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
			// ɾ�����е�ѡ����Ŀ
			for (iCurItem=lpPDAListCtrl->iItemNum-1;iCurItem>=0;iCurItem--)
			{
				lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // �õ���Ŀ����
				if (lpMainItem)
				{
						if (lpMainItem->State&LVIS_SELECTED)
						{ // ��ǰ��Ŀ��ѡ����Ŀ
							SendMessage(hWnd,LVM_DELETEITEM,iCurItem,0); // ɾ������Ŀ
						}
				}
			}
			return TRUE;

}
/**************************************************
������static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPSTYLESTRUCT ָ��STYLESTRUCT�ṹ��ָ��
����ֵ����
��������������WM_STYLECHANGED��Ϣ��
����: 
************************************************/
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPSTYLESTRUCT lpStyleStruct = (LPSTYLESTRUCT)lParam;          // new styles

		if ((lpStyleStruct->styleOld & LVS_TYPEMASK) != (lpStyleStruct->styleNew & LVS_TYPEMASK))
		{ // �ؼ������ͷ����ı�
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return FALSE;
//			ClearAllInvert(hWnd);
			lpPDAListCtrl->iStartItem=0;
			lpPDAListCtrl->iLineHeight = 0; // add By Jami chen in 2004.05.21 , will ReCalc height
			lpPDAListCtrl->iLineHeight = GetLineHeight(hWnd); // Add By Jami chen in 2004.05.21
			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE); // ���ù�������λ��
			SetHScrollBar(hWnd); // ���ù�����
			SetVScrollBar(hWnd);
			lpPDAListCtrl->iFocusItem=-1;
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		}
		return TRUE;
}

/**************************************************
������static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ���ֵ
	IN lParam -- ����
����ֵ����
��������������WM_KEYDOWN��Ϣ��
����: 
************************************************/
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
//	DWORD lKeyData ;
	int iPageLines,iItemNumInLine;

		nVirtKey = (int) wParam;    // virtual-key code
//		lKeyData = lParam;          // key data
 		iPageLines=GetPageLine(hWnd); // �õ�ÿһҳ������
 		iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿһ�е���Ŀ��
		KillEditControl(hWnd); // ɱ���༭�ؼ�
		switch(nVirtKey)
		{
			case VK_PRIOR:// page up
				MoveInvertLine(hWnd,(int)(-1*iPageLines));
				break;
			case VK_NEXT:// page down
				MoveInvertLine(hWnd,(int)(1*iPageLines));
				break;
			case VK_END://  to the text end
				MoveInvertColumn(hWnd,(int)(1*iItemNumInLine));
				break;
			case VK_HOME:// to the text start
				MoveInvertColumn(hWnd,(int)(-1*iItemNumInLine));
				break;
			case VK_UP:// to the pre line
				MoveInvertLine(hWnd,-1);
				break;
			case VK_DOWN:// to the next line
				MoveInvertLine(hWnd,1);
				break;
			case VK_LEFT:// to the pre number
//				HScrollWindow(hWnd,-1);
				MoveInvertColumn(hWnd,-1);
				break;
			case VK_RIGHT:// to the next number
//				HScrollWindow(hWnd,1);
				MoveInvertColumn(hWnd,1);
				break;
			case VK_RETURN:// Active Current Item
				DoReturnKey(hWnd);
				break;
//			case VK_DELETE:
//				SendMessage(hWnd,WM_CLEAR,0,0);
//				break;
			default :
				return TRUE;
		};
		return TRUE;
}


/**************************************************
������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT ָ��CTLCOLORSTRUCT�ṹ��ָ��
����ֵ����
�������������ÿؼ���ɫ������WM_SETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
	LPPDALISTCTRL lpPDAListCtrl;
			

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return FALSE;
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �����ı���ɫ
			 lpPDAListCtrl->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�����ɫ
				lpPDAListCtrl->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // ����ѡ���ı���ɫ
				lpPDAListCtrl->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // ����ѡ���ı�����ɫ
				lpPDAListCtrl->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // ���ñ����ı���ɫ
			lpPDAListCtrl->cl_HeaderText = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // ���ñ����ı�����ɫ
			lpPDAListCtrl->cl_HeaderBkColor = lpCtlColor->cl_TitleBk;
		 }
		 InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
/**************************************************
������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT ָ��CTLCOLORSTRUCT�ṹ��ָ��
����ֵ����
�����������õ��ؼ���ɫ������WM_GETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		
	LPCTLCOLORSTRUCT lpCtlColor;
	LPPDALISTCTRL lpPDAListCtrl;
			

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return FALSE;


		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // �õ��ı���ɫ
			 lpCtlColor->cl_Text = lpPDAListCtrl->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �õ��ı�����ɫ
				lpCtlColor->cl_TextBk = lpPDAListCtrl->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // �õ�ѡ���ı���ɫ
				lpCtlColor->cl_Selection = lpPDAListCtrl->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // �õ�ѡ���ı�����ɫ
				lpCtlColor->cl_SelectionBk = lpPDAListCtrl->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // �õ������ı���ɫ
			lpCtlColor->cl_Title = lpPDAListCtrl->cl_HeaderText ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // �õ������ı�����ɫ
			lpCtlColor->cl_TitleBk = lpPDAListCtrl->cl_HeaderBkColor ;
		 }
		 return TRUE;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/**************************************************
������static void DrawListCtrlCaption(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƿؼ�����
����: 
************************************************/
static void DrawListCtrlCaption(HWND hWnd,HDC hdc)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int i;
			
			if (ExistCaption(hWnd)==FALSE) return;  // Is not exist caption , don't dwar caption		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // ����CAPTION��Ŀ
 					DrawCaptionItem(hWnd,hdc,i,TRUE);
			}
 			DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iSubItemNum,TRUE); // �������һ���հױ���
			// Draw Caption Line
			/*
			{
				int x0, x1,y0,y1;
				RECT rect;

				GetClientRect(hWnd,&rect);
				x0=(int)rect.left;
				x1=(int)rect.right;

				// The Caption Line
				y0=y1=CAPTIONHEIGHT-1;
				// draw a line
				MoveToEx(hdc,x0,y0,NULL);
				LineTo(hdc,x1,y1);
			}
			*/
}
/**************************************************
������static void DrawListCtrl(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƿؼ�
����: 
************************************************/
static void DrawListCtrl(HWND hWnd,HDC hdc)
{
	DWORD dwStyle;
	DWORD iDrawType;
		ClearClientArea(hWnd,hdc); // ����ͻ�����
		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		iDrawType=dwStyle&LVS_TYPEMASK; // �õ�LISTCTRL����
		switch(iDrawType)
		{
		case LVS_REPORT:  // REPORT���
				DrawReportMode(hWnd,hdc); // ����REPORTģʽ
				return;
		case LVS_LIST: // LIST���
				DrawListMode(hWnd,hdc);
				return;
		case LVS_ICON: // ICON���
				DrawIconMode(hWnd,hdc);
				return;
		case LVS_SMALLICON:  // SMALLICON ���
				DrawSmallIconMode(hWnd,hdc);
				return;
		}
}
/**************************************************
������static void ClearClientArea(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
��������������ͻ���
����: 
************************************************/
static void ClearClientArea(HWND hWnd,HDC hdc)
{
  RECT rcClientRect;


		GetClientRect(hWnd,&rcClientRect); // �õ��ͻ������С
		if (ExistCaption(hWnd)==TRUE) // �Ƿ���CAPTION
			rcClientRect.top+=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT; // ����Ҫ���CAPTION������
	  // clear ClearRect
//	  FillRect(hdc,&rcClientRect,GetStockObject(WHITE_BRUSH));
		SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ���õ�ǰ��ɫΪ������ɫ
		ClearRect(hWnd,hdc,rcClientRect); // �������
}
/**************************************************
������static BOOL ExistCaption(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ���б��⣬����TRUE�����򷵻�FALSE
�����������鿴�Ƿ���ڱ���
����: 
************************************************/
static BOOL ExistCaption(HWND hWnd)
{
	DWORD dwStyle;	
	DWORD iDrawType;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			iDrawType=dwStyle&LVS_TYPEMASK;
			if (iDrawType!=LVS_REPORT) // �Ƿ���REPORT
					return FALSE;
			if (dwStyle&LVS_NOCOLUMNHEADER)  // �Ƿ���HEADER
			   return FALSE;
			return TRUE;
}

/**************************************************
������static void DrawCaptionItem(HWND hWnd,HDC hdc,int iCol,BOOL bRaised)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iCol -- Ҫ���Ƶ���Ŀ����
	IN bRaised -- ����
����ֵ����
�������������Ʊ�����Ŀ
����: 
************************************************/
static void DrawCaptionItem(HWND hWnd,HDC hdc,int iCol,BOOL bRaised)
{
	RECT rect;
	int cx,width,i;
	int iSubItem;
#ifdef USE_BITMAP_IMAGE
	HBITMAP hBitMap;
#endif
#ifdef USE_ICON_IMAGE
	HICON hIcon;
#endif
    int iBkMode;
	HBRUSH hBkBrush ;
	BOOL bDeleteBrush = TRUE;
	COLORREF oldColor;

	int iCaptionHeight;

		LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			
			iCaptionHeight = GetCaptionHeight(hWnd,lpPDAListCtrl); // add by jami in 2004.05.22
			hBkBrush = CreateSolidBrush(lpPDAListCtrl->cl_HeaderBkColor); // �õ����ⱳ��ɫˢ��
			if (hBkBrush == NULL)
			{
				hBkBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
				bDeleteBrush = FALSE;
			}
			// �õ�ָ������Ŀ�ʼλ��
			cx=lpPDAListCtrl->cx;
			for (i=0;i<iCol;i++)
			{
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
					cx+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			}
			if (iCol>=lpPDAListCtrl->iSubItemNum)
			{ // �����һ���հױ���
					GetClientRect(hWnd,&rect);
					rect.left=cx;
//					rect.bottom=CAPTIONHEIGHT-1;
					rect.bottom= iCaptionHeight ;//CAPTIONHEIGHT;
					rect.right+=4;
//					DrawEdge(hdc,&rect,EDGE_RAISED, BF_RECT|BF_MIDDLE);
					FillRect(hdc,&rect,hBkBrush);
					goto EXIT_DRAWCAPTION;
			}
			else
			{ // �õ��������ڵĴ�С
				iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol];
				width=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
				rect.left=cx;
				rect.top=0;
//				rect.bottom=CAPTIONHEIGHT-1;
				rect.bottom=iCaptionHeight; //CAPTIONHEIGHT;
				rect.right=rect.left+width;
//				if (bRaised)
//					DrawEdge(hdc,&rect,EDGE_RAISED, BF_RECT|BF_MIDDLE);
//				else
//					DrawEdge(hdc,&rect,EDGE_ETCHED, BF_FLAT|BF_MIDDLE);
				FillRect(hdc,&rect,hBkBrush);
			}	
		
			if ((lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_TEXT)
				&&(lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_IMAGE))
			{ // ��������ı���ͼ��
//				SetBkColor(hdc,GetSysColor(COLOR_3DFACE));
                iBkMode = SetBkMode( hdc, TRANSPARENT );
				rect.left+=2;
				rect.top+=2;
				rect.right-=2;
				rect.bottom-=2;
				// ����ͼ��
#ifdef USE_BITMAP_IMAGE
				hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpPDAListCtrl->lpSubItem[iSubItem].iImage);
				if (hBitMap)
				{
						DrawBitmap(hdc,hBitMap,
							(int)rect.left,
							(int)(rect.top+(iCaptionHeight-4-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							(int)lpPDAListCtrl->sizeSmallIcon.cx,
							(int)lpPDAListCtrl->sizeSmallIcon.cy,
							FALSE);
				}
#endif
#ifdef USE_ICON_IMAGE
				//hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpPDAListCtrl->lpSubItem[iSubItem].iImage);
				hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageListSmall,lpPDAListCtrl->lpSubItem[iSubItem].iImage,0);
				if (hIcon)
				{
// !!! Modified By Jami chen in 2002.05,14
/*					DrawIcon(hdc,
							(int)rect.left,
							(int)(rect.top+(iCaptionHeight-4-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							hIcon);
*/
					DrawIconEx(hdc,
							(int)rect.left,
							(int)(rect.top+(iCaptionHeight-4-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							hIcon,
							0,
							0,
							0,
							0,
							DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
				}
#endif
				rect.left+=lpPDAListCtrl->sizeSmallIcon.cx;
//				DrawText(hdc,lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption,strlen(lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption),&rect,DT_LEFT|DT_NOPREFIX);
                SetBkMode( hdc, iBkMode );
				goto EXIT_DRAWCAPTION;
			}
			if (lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_TEXT)
			{  // �����ı�
//				SetBkColor(hdc,GetSysColor(COLOR_3DFACE));
				DWORD dwDrawStyle;
				dwDrawStyle=DT_NOPREFIX|DT_SINGLELINE;
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_RIGHT)
					dwDrawStyle|=DT_RIGHT;
				else if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_CENTER)
					dwDrawStyle|=DT_CENTER;
				else 
					dwDrawStyle|=DT_LEFT;

                iBkMode = SetBkMode( hdc, TRANSPARENT );
				oldColor = SetTextColor(hdc,lpPDAListCtrl->cl_HeaderText);
				rect.left+=2;
				rect.top+=(iCaptionHeight - lpPDAListCtrl->iTextHeight)/2;
				rect.right-=2;
				rect.bottom=rect.top+lpPDAListCtrl->iTextHeight;
				DrawText(hdc,lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption,strlen(lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption),&rect,dwDrawStyle);
				SetTextColor(hdc,oldColor);
                SetBkMode( hdc, iBkMode );
			}
			if (lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_IMAGE)
			{ // ����ͼ��
				rect.left+=2;
				rect.top+=2;
				rect.right-=2;
				rect.bottom-=2;
#ifdef USE_BITMAP_IMAGE
					hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpPDAListCtrl->lpSubItem[iSubItem].iImage);
					if (hBitMap)
					{
							DrawBitmap(hdc,hBitMap,
								(int)rect.left,
								(int)(rect.top+(iCaptionHeight-4-lpPDAListCtrl->sizeSmallIcon.cy)/2),
								(int)lpPDAListCtrl->sizeSmallIcon.cx,
								(int)lpPDAListCtrl->sizeSmallIcon.cy,
								FALSE);
					}
#endif
#ifdef USE_ICON_IMAGE
				//hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpPDAListCtrl->lpSubItem[iSubItem].iImage);
				hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageListSmall,lpPDAListCtrl->lpSubItem[iSubItem].iImage,0);
				if (hIcon)
				{
// !!! Modified By Jami chen in 2002.05.14
/*
					DrawIcon(hdc,
							(int)rect.left,
							(int)(rect.top+(iCaptionHeight-4-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							hIcon);
*/
					DrawIconEx(hdc,
							(int)rect.left,
							(int)(rect.top+(iCaptionHeight-4-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							hIcon,
							0,
							0,
							0,
							0,
							DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
				}
#endif
			}
EXIT_DRAWCAPTION:
			if (hBkBrush  && bDeleteBrush)
			{
				DeleteObject(hBkBrush); // ɾ����ˢ
			}
			return; 
}
/**************************************************
������static int PositionInWindow(HWND hWnd,POINTS points,int *CurItem)
������
	IN hWnd -- ���ھ��
	IN points -- ָ���������
	IN CurItem -- �����ڵ�λ��
����ֵ�����ص��λ��
     BLANKSPACE   : �հ�λ��
     CAPTIONITEM  : ����
     CAPTIONSEPARATE : �����϶
     LISTITEM     : ��Ŀ
�����������õ�ָ�����ڿؼ��е�λ��
����: 
************************************************/
static int PositionInWindow(HWND hWnd,POINTS points,int *CurItem)
{
		LPPDALISTCTRL lpPDAListCtrl;
		BOOL bExistCaption;
		int iCurCoordinate,i,iLineHeight;
		int iItemNumInLine,iCurWidth,iSubItem;
		
      *CurItem=0;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return BLANKSPACE;
			if (points.y<0)
					return BLANKSPACE;
			bExistCaption=ExistCaption(hWnd); // �Ƿ���ڱ���
			if (bExistCaption)
			{
					if (points.y<GetCaptionHeight(hWnd,lpPDAListCtrl))
					{ // �ڱ�����
							iCurCoordinate=lpPDAListCtrl->iWidth+lpPDAListCtrl->cx;
							if (points.x>iCurCoordinate+3) // ���һ���հױ���
								return BLANKSPACE;
							for (i=lpPDAListCtrl->iSubItemNum-1;i>=0;i--)
							{ // �õ�����һ������
									iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
									iCurWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
									if (points.x>iCurCoordinate-3)
									{ // �ڼ����
											if (CurItem)
											  *CurItem=i;
#ifdef NOSEPARATE
											return BLANKSPACE;
#else
											return CAPTIONSEPARATE;
#endif
									}
//									iCurCoordinate-=3;
									if (points.x>iCurCoordinate-(iCurWidth-3))
									{ // �ڱ�����
											if (CurItem)
											  *CurItem=i;
												return CAPTIONITEM;
									}
									iCurCoordinate-=iCurWidth;
							}
							if (points.x>iCurCoordinate)
							{ // �ڵ�һ��������
									if (CurItem)
										*CurItem=0;
										return CAPTIONITEM;
							}
							return BLANKSPACE;
					}
					points.y-=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT;
			}

			iLineHeight=GetLineHeight(hWnd); // �õ���Ŀ�߶�

			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�е���Ŀ����
			
			// �õ���Ŀ������
			*CurItem=(points.y/iLineHeight)*iItemNumInLine; 
			*CurItem+=points.x/(GetWindowWidth(hWnd)/iItemNumInLine);

			*CurItem+=lpPDAListCtrl->iStartItem;
			if (*CurItem<lpPDAListCtrl->iItemNum) 
					return LISTITEM; // ��Ŀ������Ч������Ŀ��
			return LISTBLANKSPACE; // ��Ŀ������Ч
}

/**************************************************
������static void DrawLineSeparate(HWND hWnd,HDC hdc,int xCoordinate)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN xCoordinate -- ��ֱ�����ߵ�λ��
����ֵ����
�������������ƴ�ֱ������
����: 
************************************************/
static void DrawLineSeparate(HWND hWnd,HDC hdc,int xCoordinate)
{
  int x0,y0,x1,y1;
//  HPEN hPen;
//  HGDIOBJ hOldObj;
//  COLORREF clrColor;
	RECT rect;
//	int i;
/*	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);
		if (lpPDAListCtrl==NULL)
				return BLANKSPACE;
*/
		// create pen
//		clrColor=(COLORREF)0;
//    hPen=CreatePen(PS_DOT,clrColor,0);
//    hOldObj=SelectObject(hdc,hPen);
		// set line Position
		x0=x1=xCoordinate; // �õ������λ��
/*		for (i=0;i<iSubItem;i++)
		{
			x0=x1+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
		}
		x0=x1+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;*/
		GetClientRect(hWnd,&rect);
		y0=0;
		y1=(int)rect.bottom; // �õ�����ĸ߶�

//		SetROP2(hdc,R2_NOTXORPEN);
		SetROP2(hdc,R2_NOT);

		// draw a line
		MoveToEx(hdc,x0,y0,NULL);
		LineTo(hdc,x1,y1); // ���Ƽ����

/*		SetROP2(hdc,R2_NOTXORPEN);


    MoveToEx(hdc,x0,y0,NULL);
    LineTo(hdc,x1,y1);*/
		// restore the old set
//    SelectObject(hdc,hOldObj);
		// delete the pen
//    DeleteObject(hPen);
}
/**************************************************
������static int GetPageItemNum(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ������һҳ��������Ŀ��Ŀ
�����������õ�һҳ��������Ŀ��Ŀ
����: 
************************************************/
static int GetPageItemNum(HWND hWnd)
{
	int iLineNum,iItemNumInLine,iItemNumInPage;

			iLineNum=GetWindowHeight(hWnd)/GetLineHeight(hWnd)+1;// �õ�ÿҳ������
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�е���Ŀ��
			iItemNumInPage=iItemNumInLine*iLineNum; // �õ�ÿҳ����Ŀ��
			return iItemNumInPage;
}
/**************************************************
������static void DrawReportMode(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������Ʊ���ģʽ
����: 
************************************************/
static void DrawReportMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;
	DWORD dwStyle;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			iCurItem=lpPDAListCtrl->iStartItem; // �õ���ʼ��Ŀ������
			iPageItem=GetPageItemNum(hWnd); // �õ�ÿҳ����Ŀ��
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ������ɫ
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawReportItem(hWnd,hdc,iCurItem,i); // ����REPORT��Ŀ
				iCurItem++;
			}
		  if (dwStyle&LVS_HORZLINE)
			{
					DrawHorzListLine(hWnd,hdc); // ����ˮƽ��
			}
		  if (dwStyle&LVS_VERTLINE)
			{
					DrawVertListLine(hWnd,hdc); // ���ƴ�ֱ��
			}
}
/**************************************************
������static int GetLineHeight(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ�������и�
�����������õ��и�
����: 
************************************************/
static int GetLineHeight(HWND hWnd)
{
	DWORD iDrawType;
	HDC hdc;
	TEXTMETRIC TextMetric;
  int nTextHeight;
  DWORD dwStyle;

	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return 0;

		if (lpPDAListCtrl->iLineHeight) // Add By Jami chen in 2004.05.21)
			return lpPDAListCtrl->iLineHeight;

		hdc=GetDC(hWnd);

		// get current text property
		GetTextMetrics(hdc,&TextMetric);
		ReleaseDC(hWnd,hdc);
		nTextHeight=(int)TextMetric.tmHeight;

		nTextHeight += 2; // �õ��ı��߶�

//		nTextHeight=USETEXTHEIGHT; // �õ��ı��߶�

	    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڸ߶�
		iDrawType=dwStyle&LVS_TYPEMASK;
		if (iDrawType==LVS_ICON)
		{// ��ICON ������ͼ��ĸ߶ȼ����
			nTextHeight+=3; // separate in the bitmap and caption
			nTextHeight+=(int)lpPDAListCtrl->sizeIcon.cy; // width of the icon
			nTextHeight+=lpPDAListCtrl->iVertSpacing;
		}
//			nTextHeight+=1; // separate in the lines
		if (dwStyle&LVS_HORZLINE)
			nTextHeight+=1; // �����ߵĸ߶�
		return nTextHeight;
}
/**************************************************
������static int GetWindowWidth(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ�����ش��ھ��
�����������õ����ھ��
����: 
************************************************/
static int GetWindowWidth(HWND hWnd)
{
	RECT rect;
		GetClientRect(hWnd,&rect); // �õ����εĴ�С
	return (int)(rect.right-rect.left);
}
static int GetWindowHeight(HWND hWnd)
{
	RECT rect;
	int iWindowHeight;
		GetClientRect(hWnd,&rect); // �õ��ͻ�����
		iWindowHeight=(int)(rect.bottom-rect.top);
		if (ExistCaption(hWnd)==TRUE) // �б���
			iWindowHeight-=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT; 
		if (iWindowHeight<0)
			iWindowHeight=0;
	return iWindowHeight;
}
/**************************************************
������static void DrawReportItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iDrawItem -- ��Ŀ����
	IN iDrawLine -- ��Ŀ���Ƶ�������
����ֵ����
�������������Ʊ�����Ŀ
����: 
************************************************/
static void DrawReportItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine)
{
	  int iCurSubItem,i;
		LPPDALISTCTRL lpPDAListCtrl;
		int iTopCoordinate,iLineHeight;
		LPMAINITEM lpMainItem;
		LPTSTR lpSubItemString;
		RECT rect,rcLine;
#ifdef USE_BITMAP_IMAGE
		HBITMAP hBitMap;
#endif
#ifdef USE_ICON_IMAGE
		HICON hIcon;
#endif

		TCHAR lpDrawString[128];
		int iDrawItemNo,iImage,iImageWidth;
		DWORD dwStyle;
		BOOL bFocus,bInvert;
//		LVITEM lvItem;
		CHAR lpFileFeild[FIELDMAXLEN];
		UINT iDrawStyle;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if (ExistCaption(hWnd)==TRUE) // �Ƿ��б���
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
			else
				iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // �õ���Ŀ�߶�
			// �õ���Ŀ����
			rect.left=rcLine.left=lpPDAListCtrl->cx;
			rect.top=rcLine.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rcLine.bottom=rect.top+iLineHeight;
			rcLine.right=rect.right=rect.left+lpPDAListCtrl->iWidth;
			if (dwStyle&LVS_HORZLINE)
			{// ��ˮƽ��
				rcLine.bottom--;
				rect.bottom--;
			}
			if (dwStyle&LVS_VERTLINE)
			{ // �д�ֱ��
				rect.right--;
				rcLine.right--;
			}
			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // �û��Ի�
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}
			iDrawItemNo=iDrawItem;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // �õ���Ŀ�ṹ
			if (lpMainItem==NULL) return ;
			if (lpMainItem->State&LVIS_SELECTED)
			{ // ��ǰΪѡ����Ŀ
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
				bInvert=TRUE;
			}
			else
			{ // ��ǰΪͨ����Ŀ
				SetPDAListColor(hWnd,hdc,NORMALCOLOR);
				bInvert=FALSE;
			}
			bFocus=lpMainItem->State&LVIS_FOCUSED; // �Ƿ��н���
// !!! draw this item ,first clear this line
// !!! Add By Jami chen 2002.04.24
			ClearRect(hWnd,hdc,rcLine); // �����Ŀ
//			ClearLine(hWnd,hdc,iDrawLine);
// !!! add end By Jami chen 2002.04.24
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // ���Ƹ�����Ŀ
					iCurSubItem=lpPDAListCtrl->lpColumnOrderArray[i]; // �õ���Ŀ����
					rect.right=rect.left+lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
					if (iCurSubItem==0)
					{
						iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,iCurSubItem,lpFileFeild); // �õ���Ŀ�ı�
						lpSubItemString=lpFileFeild;
						if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
						{ // ��ͼ�󣬻���ͼ��
									rect.left+=IMAGESPACE;
#ifdef USE_BITMAP_IMAGE
									hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,iImage);
									if (lpPDAListCtrl->sizeSmallIcon.cx>lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth)
											iImageWidth=lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
									else
											iImageWidth=lpPDAListCtrl->sizeSmallIcon.cx;
									if (hBitMap)
									{
											DrawBitmap(hdc,hBitMap,
												(int)rect.left,
												(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
												(int)iImageWidth,
												(int)lpPDAListCtrl->sizeSmallIcon.cy,
												bInvert);
									}
									rect.left+=iImageWidth+1;
#endif
#ifdef USE_ICON_IMAGE
					//				hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,iImage);
									hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageListSmall,iImage,0);
									if (lpPDAListCtrl->sizeSmallIcon.cx>lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth)
											iImageWidth=lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
									else
											iImageWidth=(int)lpPDAListCtrl->sizeSmallIcon.cx;
									if (hIcon)
									{
// !!! Modified By Jami chen in 2002.05.14
/*										DrawIcon(hdc,
												(int)rect.left,
												(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
												hIcon);
*/
										DrawIconEx(hdc,
												(int)rect.left,
												(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
												hIcon,
												0,
												0,
												0,
												0,
												DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
									}
									rect.left+=iImageWidth+1;
#endif
									rect.left+=IMAGESPACE; // �������
						}
						if (i == 0)
							rcLine.left=rect.left;
// !!! Delete By Jami chen 2002.04.24
//						ClearRect(hWnd,hdc,rcLine);
// !!! Delete End By Jami chen 2002.04.24
						if (lpSubItemString)
						{ // �����ı�
							// !!! modified by Jami chen in 2004.05.21
							//rect.top += (USETEXTHEIGHT - 16) /2;
							//rect.bottom = rect.top +  16 ;
							rect.top += (lpPDAListCtrl->iLineHeight - (lpPDAListCtrl->iTextHeight )) /2;
							rect.bottom = rect.top +  lpPDAListCtrl->iTextHeight ;
							// !!! 

							MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
							iDrawStyle=DT_NOPREFIX|DT_SINGLELINE;
						    if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_RIGHT)
								iDrawStyle|=DT_RIGHT;
							else if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_CENTER)
								iDrawStyle|=DT_CENTER;
							else 
								iDrawStyle|=DT_LEFT;

							DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,iDrawStyle);
						}
					}

					else
					{ // �����ı�
						iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,iCurSubItem,lpFileFeild);
						lpSubItemString=lpFileFeild;

/*						if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_FILE)
						{
							lvItem.iItem=(int)lpMainItem->lpText;
							lvItem.iSubItem=iCurSubItem;	
							if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_IMAGE)
								lvItem.mask=LVIF_IMAGE;	
							else
								lvItem.mask=LVIF_TEXT;	
							lvItem.pszText=lpFileFeild;
							lvItem.cchTextMax=FIELDMAXLEN;
							GetDispInfo(hWnd,&lvItem);
							iImage=lvItem.iImage;
							lpSubItemString=lpFileFeild;
						}
						else
						{
								if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_IMAGE)
									iImage=(int)PtrListAt(&lpPDAListCtrl->lpSubItem[iCurSubItem].plSubItemContent,iDrawItemNo);
								else
									lpSubItemString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[iCurSubItem].plSubItemContent,iDrawItemNo);
						}
*/
						if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_IMAGE)
						{ // ����ͼ��
//							iImage=(int)PtrListAt(&lpPDAListCtrl->lpSubItem[iCurSubItem].plSubItemContent,iDrawItemNo);
#ifdef USE_BITMAP_IMAGE
							hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,iImage);
							if (hBitMap)
							{
									if (lpPDAListCtrl->sizeSmallIcon.cx>lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth)
											iImageWidth=lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
									else
											iImageWidth=lpPDAListCtrl->sizeSmallIcon.cx;
	
									DrawBitmap(hdc,hBitMap,
									(int)rect.left,
									(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
									(int)iImageWidth,
									(int)lpPDAListCtrl->sizeSmallIcon.cy,
									bInvert
									);
							}
#endif
#ifdef USE_ICON_IMAGE
							//hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,iImage);
							hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageListSmall,iImage,0);
							if (lpPDAListCtrl->sizeSmallIcon.cx>lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth)
									iImageWidth=lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
							else
									iImageWidth=(int)lpPDAListCtrl->sizeSmallIcon.cx;
							if (hIcon)
							{
// !!! Modified By Jami chen in 2002.05.14
/*								DrawIcon(hdc,
									(int)rect.left,
									(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
									hIcon);
*/
								DrawIconEx(hdc,
									(int)rect.left,
									(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
									hIcon,
									0,
									0,
									0,
									0,
									DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
							}
#endif
						}
						else
						{ // �����ı�
//							lpSubItemString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[iCurSubItem].plSubItemContent,iDrawItemNo);
							rect.left += 4; // !!! Add By Jami chen in 2003.08.22 for some interval
							if (lpSubItemString)
							{
								MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
								iDrawStyle=DT_NOPREFIX|DT_SINGLELINE;
								if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_RIGHT)
									iDrawStyle|=DT_RIGHT;
								else if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_CENTER)
									iDrawStyle|=DT_CENTER;
								else 
									iDrawStyle|=DT_LEFT;
								DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,iDrawStyle);
							}
						}
					}
					rect.left=rect.right;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,bFocus);
			if (bFocus)
				DrawFocusRect(hdc,&rcLine); // ���ƽ����
}
/**************************************************
������static void SetPDAListColor(HWND hWnd,HDC hdc,int nColorStyle)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN nColorStyle -- ��ɫ����
����ֵ����
�������������ÿؼ���ɫ
����: 
************************************************/
static void SetPDAListColor(HWND hWnd,HDC hdc,int nColorStyle)
{
	LPPDALISTCTRL lpPDAListCtrl;

		  lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		  if (lpPDAListCtrl==NULL)
					return ;

		  if (nColorStyle==NORMALCOLOR)
			{ // Set normal color
				// set Text Color same with normal window
//				SetTextColor(hdc,CL_BLACK);
				SetTextColor(hdc,lpPDAListCtrl->cl_NormalText);
				// set Text background Color same with normal window
//				SetBkColor(hdc,CL_WHITE);
				SetBkColor(hdc,lpPDAListCtrl->cl_NormalBkColor);
			}
			if (nColorStyle==INVERTCOLOR)
			{ // set invert color
				// set Text Color same with Invert Text
//				SetTextColor(hdc,CL_WHITE);
				SetTextColor(hdc,lpPDAListCtrl->cl_InvertText);
				// set Text background Color same with Invert text background
//				SetBkColor(hdc,CL_BLACK);
				SetBkColor(hdc,lpPDAListCtrl->cl_InvertBkColor);
			}
}
/**************************************************
������static void ClearLine(HWND hWnd,HDC hdc,int iDrawLine)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iDrawLine --Ҫ�����������
����ֵ����
�������������ָ����������
����: 
************************************************/
static void ClearLine(HWND hWnd,HDC hdc,int iDrawLine)
{
   RECT ClearRect;
   int nLineHeight;
	 int nWindowWidth;
	 HBRUSH hBrush;

	 //  get line height
	 nLineHeight=GetLineHeight(hWnd); // �õ��и߶�
	 // get window width
	 nWindowWidth=GetWindowWidth(hWnd); // �õ����ڿ��
	
	// calculate clear rect
	ClearRect.left=0;
	if (ExistCaption(hWnd)==TRUE)
			ClearRect.top=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL)+iDrawLine*nLineHeight;
	else
			ClearRect.top=iDrawLine*nLineHeight;
	ClearRect.right=nWindowWidth;
	ClearRect.bottom =ClearRect.top+nLineHeight;
	hBrush=CreateSolidBrush(GetBkColor(hdc));  // ��������ˢ
	FillRect(hdc,&ClearRect,hBrush); // �������
	DeleteObject(hBrush);
//	ReleaseDC(hWnd,hdc);
}
/**************************************************
������static void ClearRect(HWND hWnd,HDC hdc,RECT rect)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN rect -- Ҫ���������
����ֵ����
�������������һ��ָ������
����: 
************************************************/
static void ClearRect(HWND hWnd,HDC hdc,RECT rect)
{
	 HBRUSH hBrush;

	hBrush=CreateSolidBrush(GetBkColor(hdc)); // ��������ˢ
	FillRect(hdc,&rect,hBrush); // �������
	DeleteObject(hBrush);
}
/**************************************************
������static int MakeShortString(HDC hdc, int nColumnLen,LPCTSTR lpLongString,LPTSTR lpShortString)
������
	IN hdc -- �豸���
	IN nColumnLen -- ת�����ַ����������
	IN lpLongString -- ת��ǰ�ĳ��ַ���
	OUT lpShortString -- ת����Ķ��ַ���
����ֵ������ת�����ַ����ĳ���
����������ת�����ַ����Ŀ��
����: 
************************************************/
static int MakeShortString(HDC hdc, int nColumnLen,LPCTSTR lpLongString,LPTSTR lpShortString)
{
	CHAR szThreeDots[] = "...";
  int nStringLen;
	int nAddLen,nStringWidth;
	int i;
	
	nStringLen=strlen(lpLongString);
	if(nStringLen==0)
      lpShortString[0]=0;
	else 
	{
	 if(GetTextExtent(hdc,lpLongString, nStringLen)<= nColumnLen) // �õ��ı�����
	 { // �ı����С��Ҫ���ȣ�����Ҫת��
		    strcpy(lpShortString,lpLongString);
				nStringWidth=GetTextExtent(hdc,lpShortString, strlen(lpShortString));
				return nStringWidth;
	 }
	 nAddLen=GetTextExtent(hdc,szThreeDots, strlen(szThreeDots)); // �õ�...�Ŀ��

	 i=0;
// !!! Modified By Jami chen in 2003.09.06	 
//	 if (lpLongString[0]<0)
	 if (lpLongString[0] & 0x80)
// !!! Modified By Jami chen in 2003.09.06	 
	 { // �Ǻ���
				nAddLen+=GetTextExtent(hdc,&lpLongString[0], 2);
			  lpShortString[i]=lpLongString[i];
				i++;
			  lpShortString[i]=lpLongString[i];
				i++;
	 }
	 else
	 { // ���ַ�
				nAddLen+=GetTextExtent(hdc,&lpLongString[0], 1);
			  lpShortString[i]=lpLongString[i];
				i++;
	 }
   for(; i<nStringLen-1;i++)
	 { // ����ɷ���¸���
// !!! modified by jami chen in 2003.09.06
//		    if (lpLongString[i]<0)
			if (lpLongString[i] & 0x80)
// !!! modified end by jami chen in 2003.09.06
				nAddLen+=GetTextExtent(hdc,&lpLongString[i], 2);
			else
				nAddLen+=GetTextExtent(hdc,&lpLongString[i], 1);
			if(nAddLen>nColumnLen)
			{
				 if(i>0) 
					 lpShortString[i]= 0;
				break;
			}
// !!! modified by jami chen in 2003.09.06
//			if (lpLongString[i]<0)
			if (lpLongString[i] & 0x80)
// !!! modified End by jami chen in 2003.09.06
			{
			  lpShortString[i]=lpLongString[i];
				i++;
			}
			lpShortString[i]=lpLongString[i];
		} 
		lpShortString[i]=0;
		strcat(lpShortString, szThreeDots);
	}
	nStringWidth=GetTextExtent(hdc,lpShortString, strlen(lpShortString)); // �õ�ת������ִ�����
	return nStringWidth;
}
/**************************************************
������static int GetTextExtent(HDC hdc,LPCTSTR lpszString, int nCount)
������
	IN hdc -- �豸���
	IN lpszString -- Ҫ�õ���ȵ��ַ���
	IN nCount -- �ַ�������
����ֵ�������ַ����Ŀ��
�����������õ��ַ����Ŀ��
����: 
************************************************/
static int GetTextExtent(HDC hdc,LPCTSTR lpszString, int nCount)
{
		SIZE size;
		GetTextExtentPoint32(hdc, lpszString, (int)nCount, &size); // �õ��ִ��Ŀ��
		return (int)size.cx;
}
/**************************************************
������static void DrawListMode(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc --�豸���
����ֵ����
�������������ƿؼ�����ʽģʽ
����: 
************************************************/
static void DrawListMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iCurItem=lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // �õ�ÿҳ����Ŀ����
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawListItem(hWnd,hdc,iCurItem,i); // ����LIST��Ŀ
				iCurItem++;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,TRUE);
}
/**************************************************
������static void DrawIconMode(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƿؼ���ͼ��ģʽ
����: 
************************************************/
static void DrawIconMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iCurItem=lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // �õ�ÿҳ����Ŀ����
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawIconItem(hWnd,hdc,iCurItem,i); // ����ICON��Ŀ
				iCurItem++;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,TRUE);
}
/**************************************************
������static void DrawSmallIconMode(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƿؼ���Сͼ��ģʽ
����: 
************************************************/
static void DrawSmallIconMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iCurItem=lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // �õ�ÿҳ����Ŀ����
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawSmallIconItem(hWnd,hdc,iCurItem,i); // ����SMALLICON��Ŀ
				iCurItem++;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,TRUE);
}
/**************************************************
������static void DrawListItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iDrawItem -- ��Ŀ����
	IN iDrawLine -- ������
����ֵ����
����������������ʽ��Ŀ
����: 
************************************************/
static void DrawListItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine)
{
//	  int iCurSubItem;
		LPPDALISTCTRL lpPDAListCtrl;
//		BOOL bExistCaption;
		int iTopCoordinate,iLineHeight;
		LPMAINITEM lpMainItem;
		LPTSTR lpSubItemString;
		RECT rect;
#ifdef USE_BITMAP_IMAGE
		HBITMAP hBitMap;
#endif
#ifdef USE_ICON_IMAGE
		HICON hIcon;
#endif

		TCHAR lpDrawString[128];
		int iDrawItemNo,iDrawStringWidth;
		DWORD dwStyle;
		BOOL bFocus,bInvert;
		int  iImage;
		char lpFileFeild[FIELDMAXLEN];
		HBRUSH hBkBrush = NULL;
		BOOL bDeleteBrush = TRUE;
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // �õ��и߶�
			rect.left=lpPDAListCtrl->cx;
			rect.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rect.top+iLineHeight;
			rect.right=GetWindowWidth(hWnd);

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // �û��Ի�
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}

			iDrawItemNo=iDrawItem;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // �õ���Ŀ�ṹ
			if (lpMainItem==NULL)
				return;
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ������ɫ
			ClearLine(hWnd,hdc,iDrawLine);
			if (lpMainItem->State&LVIS_SELECTED)
			{ // ��ǰΪѡ����Ŀ
				SetPDAListColor(hWnd,hdc,INVERTCOLOR); // ������ɫ
				bInvert=TRUE;
				hBkBrush = CreateSolidBrush(lpPDAListCtrl->cl_InvertBkColor);
				if (hBkBrush == NULL)
				{
					hBkBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
					bDeleteBrush = FALSE;
				}
			}
			else
			{
				SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ������ɫ
				bInvert=FALSE;
			}

			iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,0,lpFileFeild); // �õ���Ŀ�ִ�
			lpSubItemString=lpFileFeild;

			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // ��ͼ��,����ͼ��
#ifdef USE_BITMAP_IMAGE
//				hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpMainItem->iImage);
				hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,iImage);
				if (hBitMap)
				{
					DrawBitmap(hdc,hBitMap,
						(int)rect.left,
						(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
						(int)lpPDAListCtrl->sizeSmallIcon.cx,
						(int)lpPDAListCtrl->sizeSmallIcon.cy,
						bInvert);
				}
#endif
#ifdef USE_ICON_IMAGE
				//hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,iImage);
				hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageListSmall,iImage,0);
				if (hIcon)
				{
// !!! Modified By Jami chen in 2002.05.14
/*					DrawIcon(hdc,
						(int)rect.left,
						(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
						hIcon);
*/
					DrawIconEx(hdc,
						(int)rect.left,
						(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
						hIcon,
						0,
						0,
						0,
						0,
						DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
				}
#endif
				rect.left+=lpPDAListCtrl->sizeSmallIcon.cx+1;
			}
//			lpSubItemString=lpMainItem->lpText;
//			MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			// �õ������ı�
			iDrawStringWidth=MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			rect.right=rect.left+iDrawStringWidth;

//			if (bInvert == TRUE)
//				FillRect(hdc,&rect,hBkBrush);
			// Add By Jami chen
			if (bInvert == TRUE)
				ClearRect(hWnd,hdc,rect);
			// Add End By Jami chen
			// �����ı�
			DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
			bFocus=lpMainItem->State&LVIS_FOCUSED;
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,bFocus);
			if (bFocus)
				DrawFocusRect(hdc,&rect); // ���ƽ������

			if (hBkBrush  && bDeleteBrush)
			{
				DeleteObject(hBkBrush);
			}
}
/**************************************************
������static void InvalidateItem(HWND hWnd,int iCurItem)
������
	IN hWnd -- ���ھ��
	IN iCurItem -- Ҫ��Ч����Ŀ����
����ֵ����
������������Чָ������Ŀ
����: 
************************************************/
static void InvalidateItem(HWND hWnd,int iCurItem)
{
	LPPDALISTCTRL lpPDAListCtrl;
//	int iDrawLine,iPageItem;
	RECT rcItem;
	int iTopCoordinate,iLineHeight,iDrawLine,iDrawxPos,iWindowWidth,iItemNumInLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			if (iCurItem<0)
				return ;
			if (iCurItem>=lpPDAListCtrl->iItemNum)
				return;
			if (ExistCaption(hWnd)==TRUE)
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
			else
				iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // �õ���Ŀ�߶�
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ����
			iWindowWidth=GetWindowWidth(hWnd); // �õ����ڿ��
			iDrawLine=(iCurItem-lpPDAListCtrl->iStartItem)/iItemNumInLine; // �õ�Ҫ���Ƶ�����
			iDrawxPos=(iWindowWidth/iItemNumInLine)*(iCurItem%iItemNumInLine); // �õ�Ҫ��ʼ���ƵĿ�ʼλ��

			rcItem.left=iDrawxPos;
			rcItem.top=iTopCoordinate+iLineHeight*iDrawLine;
			rcItem.bottom=rcItem.top+iLineHeight;
			rcItem.right=rcItem.left+iWindowWidth/iItemNumInLine;
			InvalidateRect(hWnd,&rcItem,TRUE); // ��Ч����
}
/**************************************************
������static void DrawItem(HWND hWnd,int iCurItem)
������
	IN hWnd -- ���ھ��
	IN iCurItem  -- Ҫ���Ƶ���Ŀ����
����ֵ����
��������������ָ����Ŀ
����: 
************************************************/
static void DrawItem(HWND hWnd,int iCurItem)
{
#ifdef CPU_X86
      InvalidateItem(hWnd,iCurItem);
#else
	LPPDALISTCTRL lpPDAListCtrl;
	int iDrawLine,iPageItem;
  DWORD dwStyle,iDrawType;
  LPMAINITEM lpMainItem;
  HDC hdc;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			if (iCurItem<0)
				return ;
			if (iCurItem>=lpPDAListCtrl->iItemNum)
				return;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			iDrawType=dwStyle&LVS_TYPEMASK;
			iDrawLine=iCurItem-lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // �õ�ҳ��Ŀ����
			if (iDrawLine<0||iDrawLine>=iPageItem)
				return;

			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // �õ���Ŀ�ṹ
			if (lpMainItem==NULL)
				return ;
      hdc=GetDC(hWnd);
			switch(iDrawType)
			{
			case LVS_REPORT:
					DrawReportItem(hWnd,hdc,iCurItem,iDrawLine); // ����REPORT��Ŀ
		      if (dwStyle&LVS_VERTLINE)
			    {
					    DrawVertListLine(hWnd,hdc); // ����ˮƽ��
			    }
					break;
			case LVS_LIST:
					DrawListItem(hWnd,hdc,iCurItem,iDrawLine); // ����LIST��Ŀ
					break;
			case LVS_SMALLICON:
					DrawSmallIconItem(hWnd,hdc,iCurItem,iDrawLine); // ����SMALLICON��Ŀ
					break;
			case LVS_ICON:
					DrawIconItem(hWnd,hdc,iCurItem,iDrawLine); // ����ICON��Ŀ
					break;
			}
      ReleaseDC(hWnd,hdc);
#endif
}
#ifdef USE_BITMAP_IMAGE
/**************************************************
������static void DrawBitmap(HDC hdc,HBITMAP hBitMap,int x,int y,int iWidth,int iHeight,BOOL bInvert)
������
	IN hdc -- �豸���
	IN hBitmap -- λͼ���
	IN x    ��λͼ�����X����
	IN y -- ��λͼ�����Y����
	IN iWidth -- λͼ���
	IN iHeight -- λͼ�߶�
	IN bInvert -- �Ƿ�Ҫ��ɫ
����ֵ����
������������λͼ
����: 
************************************************/
static void DrawBitmap(HDC hdc,HBITMAP hBitMap,int x,int y,int iWidth,int iHeight,BOOL bInvert)
{
		HDC hMemoryDC;
		DWORD dwRop=SRCCOPY;

//			if(bInvert)
//				dwRop=SRCINVERT;
			if(bInvert)
			{
				SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ������ɫ
				dwRop=NOTSRCCOPY;
			}
			hMemoryDC=CreateCompatibleDC(hdc); // �õ�����DC
			hBitMap = SelectObject(hMemoryDC,hBitMap); // װ��Ҫ���Ƶ�ͼ��
			// ����ͼ��
			BitBlt( hdc, // handle to destination device context
				x,  // x-coordinate of destination rectangle's upper-left 
										 // corner
				y,  // y-coordinate of destination rectangle's upper-left 
										 // corner  
				iWidth,  // width of destination rectangle
				iHeight, // height of destination rectangle
				hMemoryDC,  // handle to source device context
				0,   // x-coordinate of source rectangle's upper-left 
										 // corner
				0,   // y-coordinate of source rectangle's upper-left 
										 // corner  
				 
//				SRCCOPY // raster operation code
				dwRop
				);
			hBitMap = SelectObject(hMemoryDC,hBitMap);
			DeleteDC(hMemoryDC);
			if(bInvert)
			{
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
			}
}
#endif

/**************************************************
������static void DrawSmallIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iDrawItem -- Ҫ���Ƶ���Ŀ����
	IN iCurPosition -- ��ǰλ��
����ֵ����
��������������Сͼ����Ŀ
����: 
************************************************/
static void DrawSmallIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition)
{
//	  int iCurSubItem;
		LPPDALISTCTRL lpPDAListCtrl;
//		BOOL bExistCaption;
		int iTopCoordinate,iLineHeight,iDrawLine,iDrawxPos;
		LPMAINITEM lpMainItem;
		LPTSTR lpSubItemString;
		RECT rect;
#ifdef USE_BITMAP_IMAGE
		HBITMAP hBitMap;
#endif
#ifdef USE_ICON_IMAGE
		HICON hIcon;
#endif
		TCHAR lpDrawString[128];
		int iDrawItemNo,iDrawStringWidth;
		DWORD dwStyle;
		BOOL bFocus,bInvert;
		char lpFileFeild[FIELDMAXLEN];
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iDrawLine=iCurPosition/SMALLICON_NUMINLINE; // �õ�����
			iDrawxPos=lpPDAListCtrl->cx+(GetWindowWidth(hWnd)/SMALLICON_NUMINLINE)*(iCurPosition%SMALLICON_NUMINLINE); // �õ���ʼλ��
			iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // �õ��и�
			// �õ���Ŀ����
			rect.left=iDrawxPos;
			rect.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rect.top+iLineHeight;
			rect.right=rect.left+GetWindowWidth(hWnd)/SMALLICON_NUMINLINE;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // �û��Ի�
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}
			iDrawItemNo=iDrawItem; 
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // �õ���Ŀ����
			if (lpMainItem==NULL)
				return;
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ������ɫ
			ClearRect(hWnd,hdc,rect); // �������
			if (lpMainItem->State&LVIS_SELECTED)
			{ // ��ǰ��ѡ����Ŀ
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
				bInvert=TRUE;
			}
			else
			{ // ��ǰ��һ����Ŀ
				SetPDAListColor(hWnd,hdc,NORMALCOLOR);
				bInvert=FALSE;
			}

			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // ����ͼ��
#ifdef USE_BITMAP_IMAGE
					hBitMap=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpMainItem->iImage);
					if (hBitMap)
					{
						DrawBitmap(hdc,hBitMap,
							(int)rect.left,
							(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							(int)lpPDAListCtrl->sizeSmallIcon.cx,
							(int)lpPDAListCtrl->sizeSmallIcon.cy,
							bInvert);
					}
#endif
#ifdef USE_ICON_IMAGE
					//hIcon=PtrListAt(lpPDAListCtrl->lpImageListSmall,lpMainItem->iImage);
					hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageListSmall,lpMainItem->iImage,0);
					if (hIcon)
					{
// !!! Modified By Jami chen in 2002.05.14
/*						DrawIcon(hdc,
							(int)rect.left,
							(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							hIcon);
*/
						DrawIconEx(hdc,
							(int)rect.left,
							(int)(rect.top+(iLineHeight-lpPDAListCtrl->sizeSmallIcon.cy)/2),
							hIcon,
							0,
							0,
							0,
							0,
							DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
					}
#endif
					rect.left+=lpPDAListCtrl->sizeSmallIcon.cy+1;
			}
			// �õ���Ŀ�ִ�			
			GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,0,lpFileFeild);
			lpSubItemString=lpFileFeild;

/*			if (lpPDAListCtrl->lpSubItem[iCurSubItem].ContentFmt&LVCFMT_FILE)
			{
				lvItem.iItem=(int)lpMainItem->lpText;
				lvItem.iSubItem=0;	
				lvItem.mask=LVIF_TEXT|LVIF_IMAGE;	
				lvItem.pszText=lpFileFeild;
				lvItem.cchTextMax=FIELDMAXLEN;
				GetDispInfo(hWnd,&lvItem);
				lpSubItemString=lpFileFeild;
			}
			else
			{
				lpSubItemString=lpMainItem->lpText;
			}
*/
//			lpSubItemString=lpMainItem->lpText;
			// �õ�ת���ִ�
			iDrawStringWidth=MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			rect.right=rect.left+iDrawStringWidth;
			// Add By Jami chen
			if (bInvert == TRUE)
				ClearRect(hWnd,hdc,rect);
			// �����ִ�
			// Add End By Jami chen
			DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
			bFocus=lpMainItem->State&LVIS_FOCUSED;
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,bFocus);
			if (bFocus)
				DrawFocusRect(hdc,&rect); // ���ƽ����
}
/**************************************************
������static void DrawIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iDrawItem -- Ҫ���Ƶ���Ŀ����
	IN iCurPosition -- ��ǰλ��
����ֵ����
��������������ͼ����Ŀ
����: 
************************************************/
static void DrawIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition)
{
//	  int iCurSubItem;
		LPPDALISTCTRL lpPDAListCtrl;
//		BOOL bExistCaption;
		int iTopCoordinate,iLineHeight,iDrawLine,iDrawxPos;
		LPMAINITEM lpMainItem;
		LPTSTR lpSubItemString;
		RECT rect;
#ifdef USE_BITMAP_IMAGE
		HBITMAP hBitMap;
#endif
#ifdef USE_ICON_IMAGE
		HICON hIcon;
#endif
		TCHAR lpDrawString[128];
		int iDrawItemNo,iDrawStringWidth,iItemNumInLine;
		DWORD dwStyle;
		BOOL bFocus,bInvert;
		int iImage;
		char lpFileFeild[FIELDMAXLEN];
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ��Ŀ
			// �õ���Ŀλ��
			iDrawLine=iCurPosition/iItemNumInLine;
			iDrawxPos=lpPDAListCtrl->cx+(GetWindowWidth(hWnd)/iItemNumInLine)*(iCurPosition%iItemNumInLine);
			iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd);
			// �õ���Ŀ����
			rect.left=iDrawxPos;
			rect.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rect.top+iLineHeight;
			rect.right=rect.left+GetWindowWidth(hWnd)/iItemNumInLine;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // �û��Ի�
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}
			iDrawItemNo=iDrawItem;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // �õ���Ŀ�ṹ����
			if (lpMainItem==NULL)
				return;
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // ������ɫ
			ClearRect(hWnd,hdc,rect);
			if (lpMainItem->State&LVIS_SELECTED)
			{ // ��ǰΪѡ����Ŀ
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
				bInvert=TRUE;
			}
			else
			{ // ��ǰΪͨ����Ŀ
				SetPDAListColor(hWnd,hdc,NORMALCOLOR);
				bInvert=FALSE;
			}

			rect.top+=lpPDAListCtrl->iVertSpacing; // separate in the bitmap and caption

			// �õ���Ŀ�ִ�
			iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,0,lpFileFeild);
			lpSubItemString=lpFileFeild;
			
			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // ����ͼ��
#ifdef USE_BITMAP_IMAGE
//				hBitMap=PtrListAt(lpPDAListCtrl->lpImageList,lpMainItem->iImage);
				hBitMap=PtrListAt(lpPDAListCtrl->lpImageList,iImage);
				if (hBitMap)
				{  // Draw Bitmap in center
						DrawBitmap(hdc,hBitMap,
							(int)(rect.left+(rect.right-rect.left-lpPDAListCtrl->sizeIcon.cx)/2),
							(int)rect.top,
							(int)lpPDAListCtrl->sizeIcon.cx,
							(int)lpPDAListCtrl->sizeIcon.cy,
							bInvert);
				}
#endif
#ifdef USE_ICON_IMAGE
				//hIcon=PtrListAt(lpPDAListCtrl->lpImageList,iImage);
				hIcon=ImageList_GetIcon(lpPDAListCtrl->lpImageList,iImage,0);
				if (hIcon)
				{
// !!! Modified By Jami chen in 2002.05.14
/*					DrawIcon(hdc,
						(int)(rect.left+(rect.right-rect.left-lpPDAListCtrl->sizeIcon.cx)/2),
						(int)rect.top,
						hIcon);
*/
					DrawIconEx(hdc,
						(int)(rect.left+(rect.right-rect.left-lpPDAListCtrl->sizeIcon.cx)/2),
						(int)rect.top,
						hIcon,
						0,
						0,
						0,
						0,
						DI_NORMAL);
// !!! Modified End By Jami chen in 2002.05.14
				}
#endif
				rect.top+=3; // separate in the bitmap and caption
				rect.top+=lpPDAListCtrl->sizeIcon.cy;
			}
//			lpSubItemString=lpMainItem->lpText;
			// �õ�ת���ִ�
			iDrawStringWidth=MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			rect.left+=((rect.right-rect.left)-iDrawStringWidth)/2;
			rect.right=rect.left+iDrawStringWidth;

			// Add By Jami chen
			if (bInvert == TRUE)
				ClearRect(hWnd,hdc,rect);
			// Add End By Jami chen
			// �����ִ�
			DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,DT_CENTER|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
			bFocus=lpMainItem->State&LVIS_FOCUSED;
			if (bFocus)
				DrawFocusRect(hdc,&rect); // ���ƽ������
}
/**************************************************
������static void SendDrawItemMessage(HWND hWnd,HDC hdc,int iCurItem,RECT rcItem)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
	IN iCurItem -- ��ǰ��Ŀ����
	IN rcItem -- ��Ŀ�ľ��δ�С
����ֵ����
�������������ͻ�����Ŀ����Ϣ���Ի�ʱʹ��
����: 
************************************************/
static void SendDrawItemMessage(HWND hWnd,HDC hdc,int iCurItem,RECT rcItem)
{
		DRAWITEMSTRUCT dis;
		UINT idCtrl;
		LPMAINITEM lpMainItem;
		LPPDALISTCTRL lpPDAListCtrl;
		HWND hParent;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;

			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // �õ���Ŀ�ṹ
			idCtrl=(UINT)GetWindowLong(hWnd,GWL_ID); // �õ��ؼ�ID��
			// �����Ի�ṹ
			dis.CtlType=ODT_LISTVIEW;
			dis.CtlID=idCtrl;
			dis.itemID=iCurItem;
			dis.itemAction=ODA_DRAWENTIRE;
			dis.itemState=lpMainItem->State;
			dis.hwndItem=hWnd;
			dis.hDC=hdc;
			dis.rcItem=rcItem;
			dis.itemData=lpMainItem->lParam;
			hParent=GetParent(hWnd);
			SendMessage(hParent,WM_DRAWITEM,(WPARAM)idCtrl,(LPARAM)&dis); // �����Ի���Ϣ
}
#ifdef USE_BITMAP_IMAGE
/**************************************************
������static void GetBitMapSize(HBITMAP hBitMap,LPSIZE lpSize)
������
	IN hBitMap -- λͼ���
	OUT lpSize -- λͼ�Ĵ�С
����ֵ����
�����������õ�λͼ�Ĵ�С
����: 
************************************************/
static void GetBitMapSize(HBITMAP hBitMap,LPSIZE lpSize)
{
	BITMAP bitmap;

        if (hBitMap==NULL)
        {
		  lpSize->cx=0;
	      lpSize->cy=0;
          return ;
        }

		bitmap.bmBits=NULL;

		GetObject(hBitMap, sizeof(BITMAP), &bitmap); // �õ�λͼ����

		// �õ�λͼ��С
		lpSize->cx=bitmap.bmWidth;
		lpSize->cy=bitmap.bmHeight;

}
#endif
/**************************************************
������static void SetVScrollBar(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
�������������ô�ֱ������
����: 
************************************************/
static void SetVScrollBar(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		DWORD dwStyle;
    int nMinPos,nMaxPos,nPageLine,nTotalLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			// get window style 
		    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			// get page Lines
			nPageLine=GetPageLine(hWnd); // �õ�ÿҳ������
			if (nPageLine == 0)
				nPageLine = 1;
			nTotalLine=GetTotalLine(hWnd); // �õ�������
			if (nTotalLine>nPageLine)
			{ // �д�ֱ������
					if ((dwStyle&WS_VSCROLL)==0)
					{// ��ʾ��ֱ������
						ShowScrollBar(hWnd,SB_VERT,TRUE);
						SetHScrollBar(hWnd);
					}	
					nMinPos=0;
					nMaxPos=nTotalLine-1;
					if (nMaxPos == 0)
						nMaxPos = 1;
					LV_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE); // ���ù������ķ�Χ
					SetVScrollPage(hWnd); // ���ù���ҳ
			}
			else
			{
					if (dwStyle&WS_VSCROLL)
					{ // ���ع�����
						lpPDAListCtrl->iStartItem=0;
						ShowScrollBar(hWnd,SB_VERT,FALSE);
						SetHScrollBar(hWnd);
					}
			}
}

/**************************************************
������static void SetVScrollPage(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
�������������ô�ֱ��������ҳ��С
����: 
************************************************/
static void SetVScrollPage(HWND hWnd)
{
	int nPageLines;
	DWORD dwStyle;
  SCROLLINFO ScrollInfo;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		if (dwStyle&WS_VSCROLL)
		{
			nPageLines=GetPageLine(hWnd); // �õ�ÿҳ������
			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =nPageLines;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,FALSE);
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH); // ����ÿҳ�Ĵ�С
		}
}
/**************************************************
������static void SetHScrollBar(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
��������������ˮƽ������
����: 
************************************************/
static void SetHScrollBar(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		DWORD dwStyle;
    int nMinPos,nMaxPos,nPageLine,nTotalLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			// get window style 
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if ((dwStyle&LVS_TYPEMASK)!=LVS_REPORT)
			{ // ����REPORT���
				if (dwStyle&WS_HSCROLL)
				{// ���ع�����
					lpPDAListCtrl->cx=0;
					ShowScrollBar(hWnd,SB_HORZ,FALSE);
					SetVScrollBar(hWnd);
				}
			   return;
			}
			// get page Lines
			nPageLine=GetWindowWidth(hWnd)/WIDTHSTEP; // �õ�ÿҳ����Ŀ
			if (nPageLine == 0)
				nPageLine = 2;
			nTotalLine=(lpPDAListCtrl->iWidth+WIDTHSTEP-1)/WIDTHSTEP; // �õ��ܿ��
			if (nTotalLine>nPageLine)
			{ // ��Ҫˮƽ������
				if ((dwStyle&WS_HSCROLL)==0)
				{ // û�й�����,��ʾ������
					ShowScrollBar(hWnd,SB_HORZ,TRUE);
					SetVScrollBar(hWnd);
				}
				nMinPos=0;
				nMaxPos=nTotalLine-1;
				if (nMaxPos == 0)
					nMaxPos = 1;
				LV_SetScrollRange(hWnd,SB_HORZ,nMinPos,nMaxPos,TRUE); // ���ù������ķ�Χ
				SetHScrollPage(hWnd); // ���ù�������ҳ����
			}
			else
			{  // ����Ҫ������
				if (dwStyle&WS_HSCROLL)
				{ // �й�����,���ع�����
					lpPDAListCtrl->cx=0;
					ShowScrollBar(hWnd,SB_HORZ,FALSE);
					SetVScrollBar(hWnd);
				}
			}
}
/**************************************************
������static void SetHScrollPage(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
��������������ˮƽ��������ҳ��С
����: 
************************************************/
static void SetHScrollPage(HWND hWnd)
{
	int nPageLines;
	DWORD dwStyle;
    SCROLLINFO ScrollInfo;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			if (dwStyle&WS_HSCROLL) 
			{
				nPageLines=GetWindowWidth(hWnd)/WIDTHSTEP; // �õ�ҳ��С
				ScrollInfo.cbSize=sizeof(SCROLLINFO);
				ScrollInfo.fMask=SIF_PAGE;
			  // set vertical scroll page
				ScrollInfo.nPage =nPageLines;
				SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,FALSE); // ����ҳ����
				EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH);
			}
}
/**************************************************
������static int GetPageLine(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ������һҳ������
�����������õ�һҳ������
����: 
************************************************/
static int GetPageLine(HWND hWnd)
{
	int iLineNum;

			iLineNum=GetWindowHeight(hWnd)/GetLineHeight(hWnd); // �õ�ÿҳ������
			return iLineNum;
}
/**************************************************
������static int GetTotalLine(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ�����ؿؼ�������
�����������õ��ؼ�������
����: 
************************************************/
static int GetTotalLine(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int nTotalLines,iItemNumInLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return 0;
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�е���Ŀ��
			nTotalLines=(lpPDAListCtrl->iItemNum+iItemNumInLine-1)/iItemNumInLine; // �õ�������
			return nTotalLines;
}
/**************************************************
������static int GetItemNumInLine(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ������һ�е���Ŀ����
�����������õ�һ�е���Ŀ����
����: 
************************************************/
static int GetItemNumInLine(HWND hWnd)
{
		DWORD dwStyle,iDrawType;
		int iItemNumInLine;
			// get window style 
      dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
			// get page Lines

			iDrawType=dwStyle&LVS_TYPEMASK; // �õ���ʾ����
			switch(iDrawType)
			{
			case LVS_ICON:
				{  // ICON����
					int iWindowWidth,iSpacing;
					LPPDALISTCTRL lpPDAListCtrl;
						
						lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
						// �õ���϶���
						if(lpPDAListCtrl==NULL)
							iSpacing=0;
						else
							iSpacing=lpPDAListCtrl->iHorzSpacing;
						iWindowWidth=GetWindowWidth(hWnd); // �õ����ڿ��
						iItemNumInLine=iWindowWidth/(ICONWIDTH+iSpacing); // �õ���Ŀ����
					break;
				}
			case LVS_REPORT:
				iItemNumInLine=REPORT_NUMINLINE; // ����REPORTÿ�е���Ŀ��
				break;
			case LVS_SMALLICON:
				iItemNumInLine=SMALLICON_NUMINLINE;  // ����SMALLICONÿ�е���Ŀ��
				break;
			case LVS_LIST:
				iItemNumInLine=LIST_NUMINLINE; // ����LISTÿ�е���Ŀ��
				break;
			}
			return iItemNumInLine;
}
/**************************************************
������static void VScrollWindow(HWND hWnd,int nScrollLine)
������
	IN hWnd -- ���ھ��
	IN nScrollLine -- ����������
����ֵ����
������������ֱ��������
����: 
************************************************/
static void VScrollWindow(HWND hWnd,int nScrollLine)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItemNumInLine,iPageLines,iTotalLines,iCurLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ����Ŀ��
			iTotalLines=GetTotalLine(hWnd); // �õ�������
			iPageLines=GetPageLine(hWnd); // �õ�ÿҳ������
			iCurLine=lpPDAListCtrl->iStartItem/iItemNumInLine; // �õ���ǰ������
			if (iCurLine==0&&nScrollLine<0)
				return; // can not scroll up
			if ((iCurLine+iPageLines>=iTotalLines)&&nScrollLine>0)		
				return; // can not scroll down
			iCurLine+=nScrollLine;
			if (iCurLine<0)
				iCurLine=0;
			if (iCurLine+iPageLines>iTotalLines)
			{ // �Ѿ��������
				iCurLine=iTotalLines-iPageLines;
			}
			if (iCurLine<0) 
				iCurLine=0;
			lpPDAListCtrl->iStartItem=iCurLine*iItemNumInLine; // �õ���ʼ��Ŀ
			LV_SetScrollPos(hWnd,SB_VERT,iCurLine,TRUE); // ���ù�������λ��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
}

/**************************************************
������static void HScrollWindow(HWND hWnd,int nScrollLine)
������
	IN hWnd -- ���ھ��
	IN nScrollLine -- Ҫ����������
����ֵ����
����������ˮƽ��������
����: 
************************************************/
static void HScrollWindow(HWND hWnd,int nScrollLine)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iPageLines,iTotalLines,iCurLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
//			iTotalLines=lpPDAListCtrl->iWidth/WIDTHSTEP;
			iTotalLines=(lpPDAListCtrl->iWidth+WIDTHSTEP-1)/WIDTHSTEP; // �õ�����
			iPageLines=GetWindowWidth(hWnd)/WIDTHSTEP; // �õ�ҳ��Ŀ
			iCurLine=LV_GetScrollPos(hWnd,SB_HORZ); // �õ���ǰλ��
			if (iCurLine==0&&nScrollLine<0)
				return; // can not scroll up
			if ((iCurLine+iPageLines>=iTotalLines)&&nScrollLine>0)				return; // can not scroll down
			iCurLine+=nScrollLine;
			if (iCurLine<0)
				iCurLine=0;
			if (iCurLine+iPageLines>iTotalLines)
			{ // �Ѿ������
				iCurLine=iTotalLines-iPageLines;
			}
			if (iCurLine<0) 
				iCurLine=0;
			lpPDAListCtrl->cx=0-iCurLine*WIDTHSTEP;
			LV_SetScrollPos(hWnd,SB_HORZ,iCurLine,TRUE); // ���ù�������λ��
			InvalidateRect(hWnd,NULL,TRUE);
}
/**************************************************
������static int StrCmp(LPCTSTR lpstr1,LPCTSTR lpstr2)
������
	IN lpstr1 -- �ַ���1
	IN lpstr2 -- �ַ���2
����ֵ���ַ���1 ���� �ַ���2 ���� 1
		�ַ���1 С�� �ַ���2 ���� -1
		�ַ���1 ���� �ַ���2 ���� 0
�����������Ƚ��ַ�����С
����: 
************************************************/
static int StrCmp(LPCTSTR lpstr1,LPCTSTR lpstr2)
{ int iResult;
	
		iResult=strcmp(lpstr1,lpstr2); // �Ƚ��ַ���
		if (iResult>0) return 1;
		if (iResult<0) return -1;
		return 0;
}
/**************************************************
������static int AutoSortItem(LPPDALISTCTRL lpPDAListCtrl,LPMAINITEM lpInsertItem,int order)
������
	IN lpPDAListCtrl -- �ؼ����ݽṹ
	IN lpInsertItem -- Ҫ�������Ŀ
	IN order -- �������
����ֵ������ָ����ĿҪ���������
�����������Զ�������Ŀ
����: 
************************************************/
static int AutoSortItem(LPPDALISTCTRL lpPDAListCtrl,LPMAINITEM lpInsertItem,int order)
{
		LPMAINITEM lpMainItem;
		int iStart,iEnd,iCurItem;
		int upCondition/*,downCondition*/,iResult;
		int ExitCondition=0;

			if (lpPDAListCtrl->iItemNum==0) return 0;
			if(order==SORTASCENDING)
			{
				upCondition=-1;
//				downCondition=1;
			}
			else
			{
				upCondition=1;
//				downCondition=-1;
			}
			iStart=0;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iStart); // �õ���Ŀ�ṹ
			iResult=StrCmp(lpInsertItem->lpText,lpMainItem->lpText); // �Ƚ���Ŀ�Ĵ�С
			if (iResult==upCondition)
				return 0;
			iEnd=lpPDAListCtrl->iItemNum-1;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iEnd); // �õ����һ����Ŀ�Ľṹ
			iResult=StrCmp(lpInsertItem->lpText,lpMainItem->lpText); // �Ƚ���Ŀ��С
			if (iResult!=upCondition)
				return lpPDAListCtrl->iItemNum;
			// ʹ�ö��ַ��õ�������Ŀ��λ��
			while(1)
			{
					iCurItem=(iStart+iEnd)>>1;	
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // �õ���ǰ��Ŀ�Ľṹ
					iResult=StrCmp(lpInsertItem->lpText,lpMainItem->lpText); // �������Ŀ�Ƚϴ�С
					if (iStart==iCurItem)
					{// ��Ŀ���
						ExitCondition++;
						if (ExitCondition>=2)
						{ // ������������
							if (iResult==upCondition)
								return iCurItem;
							else
								return iCurItem+1;
						}
					}
					else
						ExitCondition=0;
					if (iResult==upCondition)
					{
						iEnd=iCurItem;
					}
					else
					{
						iStart=iCurItem;
					}
			}
}
/**************************************************
������static void FreeItem(LPMAINITEM lpMainItem,BOOL bText)
������
	IN lpMainitem -- Ҫ�ͷŵ���Ŀ
	IN bText -- �Ƿ�Ϊ�ı�
����ֵ����
�����������ͷ�ָ����Ŀ
����: 
************************************************/
static void FreeItem(LPMAINITEM lpMainItem,BOOL bText)
{
			if (lpMainItem)
			{
				if (bText)
				{
					if (lpMainItem->lpText)
						free(lpMainItem->lpText); // �ͷ���Ŀ�ı�
				}
				free(lpMainItem); // �ͷ���Ŀ
			}
}
/**************************************************
������static void RemoveItem(LPPDALISTCTRL lpPDAListCtrl,int iItem)
������
	IN lpPDAListCtrl -- �ؼ����ݽṹ
	IN iItem -- Ҫɾ������Ŀ����
����ֵ����
����������ɾ��ָ����Ŀ
����: 
************************************************/
static void RemoveItem(LPPDALISTCTRL lpPDAListCtrl,int iItem)
{
	LPMAINITEM lpMainItem;
	int i;
			lpMainItem= PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ
			PtrListAtRemove(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // ���б���ɾ����Ŀ
			// �ͷ���Ŀ
			if (!(lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE))
				FreeItem(lpMainItem,TRUE); 
			else
				FreeItem(lpMainItem,FALSE);
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // ɾ������Ŀ
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
					PtrListAtFree(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iItem); // �ͷ�����Ŀ
			}
			lpPDAListCtrl->iItemNum--; // ����һ����Ŀ
			if (lpPDAListCtrl->iStartItem>=lpPDAListCtrl->iItemNum)
			{ // ��Ҫ��������
				lpPDAListCtrl->iStartItem=lpPDAListCtrl->iItemNum-1;
				if (lpPDAListCtrl->iStartItem<0)
					lpPDAListCtrl->iStartItem=0;
			}
}
/*
static void DrawFocus(HWND hWnd,HDC hdc,int iItem,BOOL bStatus)
{
		LPPDALISTCTRL lpPDAListCtrl;
		RECT rcFocus;
		int iTopCoordinate,iLineHeight,iDrawLine,iDrawxPos,iWindowWidth,iItemNumInLine;

			if (iItem<0) return;
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);
			if (lpPDAListCtrl==NULL)
					return ;

			if (ExistCaption(hWnd)==TRUE)
				iTopCoordinate=CAPTIONHEIGHT;
			else
				iTopCoordinate=0;

			iLineHeight=GetLineHeight(hWnd);
			iItemNumInLine=GetItemNumInLine(hWnd);
			iWindowWidth=GetWindowWidth(hWnd);
			iDrawLine=iItem/iItemNumInLine;
			iDrawxPos=lpPDAListCtrl->cx+(iWindowWidth/iItemNumInLine)*(iItem%iItemNumInLine);

			rcFocus.left=iDrawxPos;
			rcFocus.top=iTopCoordinate+iLineHeight*iDrawLine;
			rcFocus.bottom=rcFocus.top+iLineHeight;
			rcFocus.right=rcFocus.left+iWindowWidth/iItemNumInLine;
//			hDC=GetDC(hWnd);
			if (bStatus==FALSE)
//			SetROP2(hdc,R2_NOTXORPEN);
		  DrawFocusRect(hdc,&rcFocus);
//			ReleaseDC(hWnd,hDC);
}
*/
/*
static void DrawFocus1(HDC hdc,LPRECT lprc,BOOL bStatus)
{
			if (bStatus==FALSE)
			SetROP2(hdc,R2_NOTXORPEN);
		  DrawFocusRect(hdc,lprc);
}
*/
/**************************************************
������static void GetPointPosition(HWND hWnd,POINTS points,int *iRow,int *iColumn)
������
	IN hWnd -- ���ھ��
	IN points -- ָ���ĵ�
	OUT iRow -- �����ڵ�������
	OUT iColumn -- �����ڵ�������
����ֵ����
�����������õ������ڵĿؼ��е�λ��
����: 
************************************************/
static void GetPointPosition(HWND hWnd,POINTS points,int *iRow,int *iColumn)
{
	int x,y;
	POINT ptOrg;
	int iLineHeight,iWindowWidth,iItemNumInLine,iCaptionHeight;

		SendMessage(hWnd,LVM_GETORIGIN,(WPARAM)0,(LPARAM)&ptOrg); // �õ�ԭ��
		if (ExistCaption(hWnd)) // �Ƿ���ڱ���
			iCaptionHeight=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT;
		else
			iCaptionHeight=0;
		x=points.x-(int)ptOrg.x;
		y=points.y-ptOrg.y-iCaptionHeight;
		iLineHeight=GetLineHeight(hWnd); // �õ��и߶�
		iWindowWidth=GetWindowWidth(hWnd); // �õ����ڿ��
		iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ����Ŀ��
		// �õ���������
		*iRow=y/iLineHeight;
		if (*iRow<0)
			*iRow=0;
		// �õ���������
		*iColumn=x/(iWindowWidth/iItemNumInLine);
		if (*iColumn>=iItemNumInLine)
			*iColumn=iItemNumInLine-1;
}
// !!! Add By Jami chen 2002.04.25
/**************************************************
������static int GetSubItemPointPosition(HWND hWnd,POINTS points)
������
	IN hWnd -- ���ھ��
	IN points -- ָ���������
����ֵ���ɹ�����ָ��������Ŀ�����������򷵻�-1
�����������õ�ָ�������ڵ�����Ŀ
����: 
************************************************/
static int GetSubItemPointPosition(HWND hWnd,POINTS points)
{
	int x;
	POINT ptOrg;
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurCoordinate,i;
	int iCurWidth,iSubItem;
	DWORD dwStyle;
	DWORD iDrawType;


		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return -1;
	    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
		iDrawType=dwStyle&LVS_TYPEMASK; //�õ���ʾ����
		if (iDrawType!=LVS_REPORT)
			return 0; // ����REPORT,û������Ŀ
		SendMessage(hWnd,LVM_GETORIGIN,(WPARAM)0,(LPARAM)&ptOrg); // �õ�ԭ��
		x=points.x-(int)ptOrg.x;
		iCurCoordinate=0;
		for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
		{  // �õ�����Ŀ����
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
			iCurWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			iCurCoordinate+=iCurWidth;
			if (points.x<iCurCoordinate)
			{ // ָ�����ڸ�����Ŀ
				return iSubItem;
			}
		}
		return -1;
}
// !!! Add End By Jami chen 2002.04.25
/**************************************************
������static void SetInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
������
	IN hWnd -- ���ھ��
	IN iRow -- ָ����
	IN iStartColumn -- ��ʼ�е�����
	IN iEndColumn -- �����е�����
����ֵ����
��������������ָ���е�ѡ����Ŀ
����: 
************************************************/
static void SetInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			if (iStartColumn<iEndColumn)
			{ // ��ʼ��С�ڽ�����
				i=iStartColumn;
				iEnd=iEndColumn;
			}
			else
			{ // ��ʼ�д��ڽ�����
				i=iEndColumn;
				iEnd=iStartColumn;
			}
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ��
			iItem=iItemNumInLine*iRow+i; // �õ���ʼ��Ŀ����
//      if (iItem>=lpPDAListCtrl->iItemNum)
//        return;
			for (;i<=iEnd;i++)
			{ // �õ�Ҫ����Ϊѡ�����Ŀ
			    if (iItem>=lpPDAListCtrl->iItemNum)
					return;
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ
				if (lpMainItem)
				{ // ���ø���ĿΪѡ����Ŀ
// !!! Add By Jami chen in 2003.01.15
						UINT uOldState = lpMainItem->State;
						POINT pt;
// !!! Add End By Jami chen in 2003.01.15
						lpMainItem->State|=LVIS_SELECTED;
// !!! Add By Jami chen in 2003.01.15
						pt.x=0;
						pt.y=0;
						SendNormalNotify(hWnd,LVN_ITEMCHANGED ,iItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
				}
				DrawItem(hWnd,iItem); // ���Ƹ���Ŀ
				iItem++;
			}
}
/**************************************************
������static void ClearInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
������
	IN hWnd -- ���ھ��
	IN iRow -- ָ����
	IN iStartColumn -- ��ʼ�е�����
	IN iEndColumn -- �����е�����
����ֵ����
�������������ָ�����е�ѡ����Ŀ
����: 
************************************************/
static void ClearInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			// �õ���ʼ��Ŀ��������Ŀ
			if (iStartColumn<iEndColumn)
			{
				i=iStartColumn;
				iEnd=iEndColumn;
			}
			else
			{
				i=iEndColumn;
				iEnd=iStartColumn;
			}
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ��
			iItem=iItemNumInLine*iRow+i;


			for (;i<=iEnd;i++)
			{ // �����Ŀѡ��
				if (iItem>=lpPDAListCtrl->iItemNum)
					return;
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ
				if (lpMainItem)
				{ // �������Ŀ��ѡ������
// !!! Add By Jami chen in 2003.01.15
					UINT uOldState = lpMainItem->State;
					POINT pt;
// !!! Add End By Jami chen in 2003.01.15
					lpMainItem->State&=~LVIS_SELECTED;
// !!! Add By Jami chen in 2003.01.15
					pt.x=0;
					pt.y=0;
					SendNormalNotify(hWnd,LVN_ITEMCHANGED ,iItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
				}
				DrawItem(hWnd,iItem); // ���Ƹ���Ŀ
				iItem++;
			}
}
/**************************************************
������static void SetInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
������
	IN hWnd -- ���ھ��
	IN iColumn -- ָ����
	IN iStartRow -- ��ʼ�е�����
	IN iEndRow -- �����е�����
����ֵ����
��������������ָ�����е�ѡ����Ŀ
����: 
************************************************/
static void SetInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			// �õ���ʼ�кͽ�����
			if (iStartRow<iEndRow)
			{
				i=iStartRow;
				iEnd=iEndRow;
			}
			else
			{
				i=iEndRow;
				iEnd=iStartRow;
			}
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ��
			for (;i<=iEnd;i++)
			{ // ������ĿΪѡ����Ŀ
				iItem=i*iItemNumInLine+iColumn; // �õ���Ŀ����
			    if (iItem>=lpPDAListCtrl->iItemNum)
				   continue;
				if (lpPDAListCtrl->lpSubItem[0].plSubItemContent.count>iItem)
				{
					lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ
					if (lpMainItem)
					{ // ���ø���ĿΪѡ����Ŀ
// !!! Add By Jami chen in 2003.01.15
							UINT uOldState = lpMainItem->State;
							POINT pt;
// !!! Add End By Jami chen in 2003.01.15
							lpMainItem->State|=LVIS_SELECTED;
// !!! Add By Jami chen in 2003.01.15
							pt.x=0;
							pt.y=0;
							SendNormalNotify(hWnd,LVN_ITEMCHANGED ,iItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
							DrawItem(hWnd,iItem); // ���Ƹ���Ŀ
					}
				}

				
			}
}
/**************************************************
������static void ClearInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
������
	IN hWnd -- ���ھ��
	IN iColumn -- ָ����
	IN iStartRow -- ��ʼ�е�����
	IN iEndRow -- �����е�����
����ֵ����
�������������ָ�����е�ѡ����Ŀ
����: 
************************************************/
static void ClearInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
					return ;
			// �õ�Ҫ�����Ŀ�Ŀ�ʼ�кͽ�����
			if (iStartRow<iEndRow)
			{
				i=iStartRow;
				iEnd=iEndRow;
			}
			else
			{
				i=iEndRow;
				iEnd=iStartRow;
			}
			iItemNumInLine=GetItemNumInLine(hWnd); // �õ�����Ŀ����
			for (;i<=iEnd;i++)
			{ // ���ָ����Ŀ��ѡ��
				iItem=i*iItemNumInLine+iColumn; // �õ���Ŀ����
				if (iItem>=lpPDAListCtrl->iItemNum)
					continue;
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // �õ���Ŀ�ṹ
				if (lpMainItem)
				{ // �������Ŀ��ѡ������
// !!! Add By Jami chen in 2003.01.15
						UINT uOldState = lpMainItem->State;
						POINT pt;
// !!! Add End By Jami chen in 2003.01.15
						lpMainItem->State&=~LVIS_SELECTED;
// !!! Add By Jami chen in 2003.01.15
						pt.x=0;
						pt.y=0;
						SendNormalNotify(hWnd,LVN_ITEMCHANGED ,iItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
				}
				DrawItem(hWnd,iItem); // ���Ƹ���Ŀ
			}
}

/**************************************************
������static void ClearAllInvert(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
����������������е�ѡ��
����: 
************************************************/
static void ClearAllInvert(HWND hWnd)
{
	LPMAINITEM lpMainItem;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;

				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
				if (lpPDAListCtrl==NULL)
						return ;
				for (i=0;i<lpPDAListCtrl->iItemNum;i++)
				{// ���������Ŀ��ѡ������
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i); // �õ���Ŀ�ṹ
					if (lpMainItem)
					{
						if (lpMainItem->State&LVIS_SELECTED)
						{ // ����Ŀ��ѡ����Ŀ,�����
// !!! Add By Jami chen in 2003.01.15
							UINT uOldState = lpMainItem->State;
							POINT pt;
// !!! Add End By Jami chen in 2003.01.15
							lpMainItem->State&=~LVIS_SELECTED;
// !!! Add By Jami chen in 2003.01.15
							pt.x=0;
							pt.y=0;
							SendNormalNotify(hWnd,LVN_ITEMCHANGED ,i,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
							DrawItem(hWnd,i); // ���Ƹ���Ŀ
						}
					}
				}
}
/**************************************************
������static void ClearFocus(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
�����������������
����: 
************************************************/
static void ClearFocus(HWND hWnd)
{
	LPMAINITEM lpMainItem;
	LPPDALISTCTRL lpPDAListCtrl;
	
				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
				if (lpPDAListCtrl==NULL)
						return ;
        if (lpPDAListCtrl->iFocusItem>=0)
        {
				  lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem); // �õ�������Ŀ�ṹ
				  if (lpMainItem)
				  { // �����Ŀ��������
// !!! Add By Jami chen in 2003.01.15
					UINT uOldState = lpMainItem->State;
					POINT pt;
// !!! Add End By Jami chen in 2003.01.15
					  lpMainItem->State&=~LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
					  pt.x=0;
					  pt.y=0;
					  SendNormalNotify(hWnd,LVN_ITEMCHANGED ,lpPDAListCtrl->iFocusItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
					  DrawItem(hWnd,lpPDAListCtrl->iFocusItem); // ���Ƹ���Ŀ
				  }
        }
        lpPDAListCtrl->iFocusItem=-1; // û�н�����Ŀ
}

/**************************************************
������static int GetListFocus(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ���ɹ����ؽ������Ŀ���������򷵻�-1
�����������õ�������Ŀ
����: 
************************************************/
static int GetListFocus(HWND hWnd)
{
	LPMAINITEM lpMainItem;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;
				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
				if (lpPDAListCtrl==NULL)
						return -1;
				for (i=0;i<lpPDAListCtrl->iItemNum;i++)
				{ // ����������Ŀ�к��н������Ŀ
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i); // �õ���Ŀ�ṹ
					if (lpMainItem)
					{ 
						if (lpMainItem->State&LVIS_FOCUSED)
						{ // ����Ŀ�ǽ�����Ŀ
							lpPDAListCtrl->iFocusItem=i;
							return i;
						}
					}
				}
				lpPDAListCtrl->iFocusItem=-1; // û�н�����Ŀ
				return -1;
}

/**************************************************
������static void GetDispInfo(HWND hWnd,LPLVITEM lpItem)
������
	IN hWnd -- ���ھ��
	IN/OUT lpItem -- Ҫ�õ�����Ŀ��Ϣ
����ֵ����
����������֪ͨ������Ҫ�õ���ʾ��Ϣ
����: 
************************************************/
static void GetDispInfo(HWND hWnd,LPLVITEM lpItem)
{
		NMLVDISPINFO nmLVDispInfo;
		HWND hParent;

//						return ;
		// ���ýṹ����		
		nmLVDispInfo.hdr.hwndFrom=hWnd;
		nmLVDispInfo.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmLVDispInfo.hdr.code=LVN_GETDISPINFO;
		nmLVDispInfo.item=*lpItem;
		hParent=GetParent(hWnd); // �õ������ھ��
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmLVDispInfo); // ������Ϣ�õ���Ŀ��Ϣ
		*lpItem=nmLVDispInfo.item;
}
/**************************************************
������static void DrawHorzListLine(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
��������������ˮƽ�ָ���
����: 
************************************************/
static void DrawHorzListLine(HWND hWnd,HDC hdc)
{
  int x0,y0,x1,y1;
	RECT rect;
	int yCoordinate,i;
	int iPageItem,iLineHeight;
		GetClientRect(hWnd,&rect); // �õ����ھ���
		x0=(int)rect.left;
		x1=(int)rect.right;

		iPageItem=GetPageItemNum(hWnd); // �õ�ÿҳ����Ŀ����
		iLineHeight=GetLineHeight(hWnd); // �õ��и߶�
		if (ExistCaption(hWnd)) // �Ƿ���ڱ���
			yCoordinate=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT;
		else
			yCoordinate=0;
		for (i=0;i<iPageItem;i++)
		{   // ����ÿһ�еķָ���
			yCoordinate+=iLineHeight;
			y0=y1=yCoordinate-1;
			// draw a line
			MoveToEx(hdc,x0,y0,NULL);
			LineTo(hdc,x1,y1); // ����ˮƽ��
		}
}
/**************************************************
������static void DrawVertListLine(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƴ�ֱ�ָ���
����: 
************************************************/
static void DrawVertListLine(HWND hWnd,HDC hdc)
{
  int x0,y0,x1,y1;
	RECT rect;
	LPPDALISTCTRL lpPDAListCtrl;
	int i,iSubItem;
	int xCoordinate;
				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
				if (lpPDAListCtrl==NULL)
						return ;
				GetClientRect(hWnd,&rect); // �õ��ͻ�����
				if (ExistCaption(hWnd)) // �Ƿ���ڱ���
					y0=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
				else
					y0=0;
				y1=(int)rect.bottom;

				xCoordinate=lpPDAListCtrl->cx;

				for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
				{ // ����ÿһ�еķָ���
					// draw a line
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
					xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
					x0=x1=xCoordinate-1;
					MoveToEx(hdc,x0,y0,NULL);
					LineTo(hdc,x1,y1); // ���ƴ�ֱ��
				}
}
/**************************************************
������static BOOL MoveInvertLine(HWND hWnd,int iMoveLine)
������
	IN hWnd -- ���ھ��
	IN iMoveLine -- ����������
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
��������������ѡ����
����: 
************************************************/
static BOOL MoveInvertLine(HWND hWnd,int iMoveLine)
{

 	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
 	int iPageLines,iTotalLines,iCurLine;
    int iNewFocusItem;
    int iScrollLine,iItemNumInLine;

  		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return FALSE;
		if (lpPDAListCtrl->iFocusItem==-1)
			 return FALSE; // û�н���
		
 		iPageLines=GetPageLine(hWnd); // �õ�ÿҳ������
		iTotalLines=GetTotalLine(hWnd); // �õ�������
 		iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�е���Ŀ����

		if (iMoveLine==0)
			return FALSE;
		iCurLine=lpPDAListCtrl->iFocusItem/iItemNumInLine; // �õ���ǰ��
		if (iMoveLine<0)
		{  // to the up
			if ((iCurLine)==0)
				return FALSE;  // already to the top
			if ((iCurLine+iMoveLine)<0)
				iMoveLine=0-iCurLine;
			iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine*iItemNumInLine; // �õ��µĽ�����Ŀ
		}
		else
		{
			if ((iCurLine+1)==iTotalLines)  // already to the bottom
				return FALSE;
			if ((iCurLine+iMoveLine)>=iTotalLines)
			{ // �õ��ƶ�������
				iMoveLine=iTotalLines-iCurLine-1;
			}
	      iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine*iItemNumInLine; // �õ��µĽ�����Ŀ
		  if (iNewFocusItem>=lpPDAListCtrl->iItemNum)
			return FALSE;
		}

//      iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine*iItemNumInLine;
//      if (iNewFocusItem<0)
//           return FALSE;
      if (iNewFocusItem<lpPDAListCtrl->iStartItem)
      {          // need scroll up window
         iScrollLine=(iNewFocusItem-lpPDAListCtrl->iStartItem-iItemNumInLine+1)/iItemNumInLine;
//         iScrollLine=iNewFocusItem-lpPDAListCtrl->iStartItem;
         VScrollWindow(hWnd,iScrollLine); // ��ֱ��������
//         iNewFocusItem=lpPDAListCtrl->iStartItem;
      }
 	  if (iNewFocusItem>=(lpPDAListCtrl->iStartItem+iPageLines*iItemNumInLine))
      {        // need scroll down Window
         iScrollLine=(iNewFocusItem-(lpPDAListCtrl->iStartItem+iPageLines*iItemNumInLine))/iItemNumInLine+1;
//         iScrollLine=(iNewFocusItem-lpPDAListCtrl->iStartItem-iPageLines+1);
         VScrollWindow(hWnd,iScrollLine); // ��ֱ��������
//         iNewFocusItem=lpPDAListCtrl->iStartItem+iPageLines-1;
      }
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iNewFocusItem); // �õ�������Ŀ�ṹ
			lpPDAListCtrl->bActiveItem=FALSE;
			// clear old Invert
			ClearAllInvert(hWnd); // ���ѡ��
			ClearFocus(hWnd); // �������

			if (lpMainItem)
			{ // ���õ�ǰ��Ŀ����,ѡ������
// !!! Add By Jami chen in 2003.01.15
					UINT uOldState = lpMainItem->State;
					POINT pt;
// !!! Add End By Jami chen in 2003.01.15
					lpMainItem->State|=LVIS_SELECTED;
					lpMainItem->State|=LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
					pt.x=0;
					pt.y=0;
					SendNormalNotify(hWnd,LVN_ITEMCHANGED ,iNewFocusItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
			}
			lpPDAListCtrl->iActiveItem=lpPDAListCtrl->iFocusItem=iNewFocusItem;
			DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // ���Ƹ���Ŀ
      return TRUE;
}

/**************************************************
������static BOOL MoveInvertColumn(HWND hWnd,int iMoveLine)
������
	IN hWnd -- ���ھ��
	IN iMoveLine -- ҡ������
����ֵ���ɹ�����TRUE�� ���򷵻�FALSE
����������ѡ����Ŀ�����ƶ�
����: 
************************************************/
static BOOL MoveInvertColumn(HWND hWnd,int iMoveLine)
{

 	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
// 	int iPageLines;
  int iNewFocusItem;
  int iItemNumInLine;

  		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return FALSE;
		if (lpPDAListCtrl->iFocusItem==-1)
			 return FALSE;
 		iItemNumInLine=GetItemNumInLine(hWnd); // �õ�ÿ�е���Ŀ����
		
		if (iMoveLine==0)
			return FALSE;
		if (iMoveLine<0)
		{  // to the pre 
			if ((lpPDAListCtrl->iFocusItem%iItemNumInLine)==0)
				return FALSE;  // already to the line head
	        iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine; // �õ��µĽ�����Ŀ
			if (iNewFocusItem<0)
				iNewFocusItem=0;
			if ((iNewFocusItem/iItemNumInLine)!=(lpPDAListCtrl->iFocusItem/iItemNumInLine))
			{  // to the other line ,must adjust the line head
				iNewFocusItem=(lpPDAListCtrl->iFocusItem/iItemNumInLine)*iItemNumInLine;
			}
				
		}
		else
		{  // to the next
			if (lpPDAListCtrl->iFocusItem>=(lpPDAListCtrl->iItemNum-1))
				return FALSE; // already to the all item tail
			if (((lpPDAListCtrl->iFocusItem+1)%iItemNumInLine)==0)
				return FALSE;  // already to the line tail
	        iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine;
			if ((iNewFocusItem/iItemNumInLine)!=(lpPDAListCtrl->iFocusItem/iItemNumInLine))
			{  // to the other line ,must adjust the line tail
				iNewFocusItem=((lpPDAListCtrl->iFocusItem/iItemNumInLine)+1)*iItemNumInLine-1;
			}
			if (iNewFocusItem>=lpPDAListCtrl->iItemNum)
				iNewFocusItem=lpPDAListCtrl->iItemNum-1;
		}

//        iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine;
//        if (iNewFocusItem<0)
//           return FALSE;
//        if (iNewFocusItem>=lpPDAListCtrl->iItemNum)
//			return FALSE;
//		if ((iNewFocusItem/iItemNumInLine)!=(lpPDAListCtrl->iFocusItem/iItemNumInLine))
//			return FALSE;
		lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iNewFocusItem);
		lpPDAListCtrl->bActiveItem=FALSE;
		// clear old Invert
		ClearAllInvert(hWnd);
		ClearFocus(hWnd); // �������

		if (lpMainItem)
		{ // ���õ�ǰ��Ŀ����,ѡ������
// !!! Add By Jami chen in 2003.01.15
			UINT uOldState = lpMainItem->State;
			POINT pt;
// !!! Add End By Jami chen in 2003.01.15
			lpMainItem->State|=LVIS_SELECTED;
			lpMainItem->State|=LVIS_FOCUSED;
// !!! Add By Jami chen in 2003.01.15
			pt.x=0;
			pt.y=0;
			SendNormalNotify(hWnd,LVN_ITEMCHANGED ,iNewFocusItem,0,lpMainItem->State,uOldState,pt);
// !!! Add End By Jami chen in 2003.01.15
		}
		lpPDAListCtrl->iActiveItem=lpPDAListCtrl->iFocusItem=iNewFocusItem;
		DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // ���Ƹ���Ŀ
      return TRUE;
}

/**************************************************
������static int GetItemString(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl,int iIndex,int iSubItem,LPTSTR lpFileFeild)
������
	IN hWnd -- ���ھ��
	IN lpPDAListCtrl -- �ؼ����ݽṹ
	IN iIndex -- ָ������Ŀ����
	IN iSubitem -- ָ��������Ŀ����
	OUT lpFileFeild -- ���Ҫ�õ����ı�
����ֵ��������Ŀ����Ӧ��ͼ������
�����������õ���Ŀ���ı�
����: 
************************************************/
static int GetItemString(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl,int iIndex,int iSubItem,LPTSTR lpFileFeild)
{
	LPMAINITEM lpMainItem;
	LPTSTR lpListString=NULL;
	LVITEM lvItem; 
	int iImage=0;

		lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iIndex); // �õ���Ŀ�ṹ
		if (lpMainItem==NULL)
			return NULL;
		if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_FILE)
		{ // ���ļ�����
// !!! Modified By Jami chen By 2002.05.09
// The Used Can Use LVM_GETITEM to get the file no.
//				lvItem.iItem=(int)lpMainItem->lpText;
				lvItem.iItem=(int)iIndex;
// !!! Modified End By Jami chen 2002.05.09
				lvItem.iSubItem=iSubItem;	
// Modified By Jami chen 2002.04.24
// if iSubItem==0 then this item must be exist the text,and may be exist image
// else the item only exist one of the text or image
/*
				lvItem.mask=LVIF_TEXT;	
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
					lvItem.mask|=LVIF_IMAGE;	
*/
// Modified to 
				if (iSubItem==0)
				{
					lvItem.mask=LVIF_TEXT;	
					if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
						lvItem.mask|=LVIF_IMAGE;	
				}
				else
				{
					if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
						lvItem.mask|=LVIF_IMAGE;	
					else
						lvItem.mask=LVIF_TEXT;	
				}
// Modified end by jami chen 2002.04.24
				lvItem.pszText=lpFileFeild;
				lvItem.cchTextMax=FIELDMAXLEN;
				GetDispInfo(hWnd,&lvItem); // �õ���Ŀ��Ϣ
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
					iImage=lvItem.iImage;
		}
		else
		{
			if (iSubItem==0)	
			{// �õ���Ŀ�ı���ͼ��
				lpListString=lpMainItem->lpText;
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
					iImage=lpMainItem->iImage;
			}
			else
			{ // �õ���Ŀ�ı���ͼ��
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
					iImage=(int)PtrListAt(&lpPDAListCtrl->lpSubItem[iSubItem].plSubItemContent,iIndex);
				else
					lpListString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[iSubItem].plSubItemContent,iIndex);
			}
			if (lpListString)
				strncpy(lpFileFeild,lpListString,FIELDMAXLEN);
			else
				strcpy(lpFileFeild,"");
		}
		return iImage;
}

/**************************************************
������static void SendActiveItem(HWND hWnd,int iItem)
������
	IN hWnd -- ���ھ��
	IN iItem -- ָ����Ŀ������
����ֵ����
��������������֪ͨ��Ϣ��ǰ�ļ�����Ŀ
����: 
************************************************/
static void SendActiveItem(HWND hWnd,int iItem)
{
	POINT pt;

		pt.x=0;
		pt.y=0;
		SendActiveNotify(hWnd,LVN_ITEMACTIVATE,iItem,pt); // ������Ŀ��������
}
/**************************************************
������static void SendActiveNotify(HWND hWnd,UINT code,int iItem,POINT pt)
������
	IN hWnd -- ���ھ��
	IN code -- Ҫ���͵�֪ͨ��Ϣ����
	IN iItem -- ��ǰ��Ŀ����
	IN pt -- ��ǰ����λ��
����ֵ����
��������������һ��֪ͨ��Ϣ��������
����: 
************************************************/
static void SendActiveNotify(HWND hWnd,UINT code,int iItem,POINT pt)
{
	NMITEMACTIVATE nmia;
	HWND hParent;

		// ���ýṹ����
		nmia.hdr.hwndFrom=hWnd;
		nmia.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmia.hdr.code=code;
		nmia.iItem=iItem;
		nmia.iSubItem=0;
		nmia.lParam=0;

		
		nmia.ptAction=pt;
		nmia.uChanged=0;
		nmia.uKeyFlags=0;
		nmia.uNewState=0;
		nmia.uOldState=0;
		hParent=GetParent(hWnd); // �õ������ھ��
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmia); // ֪ͨ������
}
/**************************************************
������static  void SendNormalNotify(HWND hWnd,UINT iCode,int iItem,int iSubItem,UINT uNewState,UINT uOldState,POINT pt)
������
	IN hWnd -- ���ھ��
	IN code -- Ҫ���͵�֪ͨ��Ϣ����
	IN iItem -- ��ǰ��Ŀ����
	IN uNewState -- �µ�״̬
	IN uOldState -- �ɵ�״̬
	IN pt -- ��ǰ����λ��
����ֵ����
��������������һ��ͨ��֪ͨ��Ϣ��������
����: 
************************************************/
static  void SendNormalNotify(HWND hWnd,UINT iCode,int iItem,int iSubItem,UINT uNewState,UINT uOldState,POINT pt)
{
	HWND hParent;
	NMLISTVIEW nmListView;

		// ���ýṹ����
		nmListView.hdr.hwndFrom=hWnd;
		nmListView.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmListView.hdr.code=iCode;
		nmListView.iItem=iItem;
		nmListView.iSubItem=iSubItem;
		nmListView.uNewState=uNewState;
		nmListView.uOldState=uOldState;
		nmListView.uChanged=TRUE;
		nmListView.ptAction=pt;
		nmListView.lParam=0;
		hParent=GetParent(hWnd); // �õ������ھ��
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmListView); // ֪ͨ������
		return ;
}
/**************************************************
������static  void SendBasicNotify(HWND hWnd,UINT iCode)
������
	IN hWnd -- ���ھ��
	IN code -- Ҫ���͵�֪ͨ��Ϣ����
����ֵ����
��������������һ������֪ͨ��Ϣ��������
����: 
************************************************/
static  void SendBasicNotify(HWND hWnd,UINT iCode)
{
	HWND hParent;
	//NMLISTVIEW nmListView;
	NMHDR hdr;
		// ���ò������
		hdr.hwndFrom=hWnd;
		hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		hdr.code=iCode;
		hParent=GetParent(hWnd); // �õ������ھ��
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&hdr); // ֪ͨ������
		return ;
}

/**************************************************
������static void DoReturnKey(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
��������������س���
����: 
************************************************/
static void DoReturnKey(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
				return ;
		SendBasicNotify(hWnd,NM_RETURN); // ����֪ͨ��Ϣ
		if (lpPDAListCtrl->iFocusItem==-1)
			return ;
		lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem); // �õ���Ŀ�ṹ

		if (lpMainItem)
		{
			if ((lpMainItem->State&LVIS_SELECTED)&&lpMainItem->State&LVIS_FOCUSED)
				SendActiveItem(hWnd,lpPDAListCtrl->iFocusItem); // ����֪ͨ��Ϣ
		}

}

LRESULT CALLBACK LabelEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoEditKillFocus(HWND hWnd);
/**************************************************
������static void CreateLabelEdit(HWND hWnd,int iItem ,LPTSTR lpString,LPRECT lprect)
������
	IN hWnd -- ���ھ��
	IN iItem -- Ҫ�޸ĵ���Ŀ����
	IN lpString -- ��ʼ���ִ�
	IN lprect -- �༭�ؼ���λ�����С
����ֵ����
���������������޸���Ŀ��ǩ�ı༭����
����: 
************************************************/
static void CreateLabelEdit(HWND hWnd,int iItem ,LPTSTR lpString,LPRECT lprect)
{
	HWND hEdit;
	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
			return ;
		// �����༭����
		hEdit = CreateWindow(classEDIT,
							lpString,
							WS_CHILD|WS_VISIBLE|WS_BORDER,
							lprect->left,
							lprect->top,
							lprect->right - lprect->left,
							lprect->bottom - lprect->top,
							hWnd,
							(HMENU)203,
							GetWindowLong(hWnd,GWL_HINSTANCE),
							NULL);

		lpPDAListCtrl->DefaultEditProc = (DefWndProc)GetWindowLong(hEdit,GWL_WNDPROC); // �õ����ڹ��̾��
		lpPDAListCtrl->iEditItem = iItem;
		lpPDAListCtrl->hEdit = hEdit;
	    SetWindowLong(hEdit,GWL_WNDPROC,(long)LabelEditProc); // �����µĴ��ڹ��̾��
		SetFocus(hEdit); // ���ý���
}
/**************************************************
������LRESULT CALLBACK LabelEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN message -- ��Ϣ
	IN wParam -- ��Ϣ����
	IN lParam -- ��Ϣ����
����ֵ��������Ϣ������
�����������޸���Ŀ��ǩ�ı༭���ڵĴ�����̺���
����: 
************************************************/
LRESULT CALLBACK LabelEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(GetParent(hWnd),0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
			return 0;
#ifdef _MAPPOINTER
		lpPDAListCtrl = (LPARAM)MapPtrToProcess( (LPVOID)lpPDAListCtrl, GetCallerProcess() );  // ӳ��ָ��
#endif
		if (lpPDAListCtrl->DefaultEditProc == NULL)
		{
			Sleep(2000);
			return 0;
		}
		switch (message)
		{
		    case WM_KILLFOCUS:
		       return DoEditKillFocus(hWnd); // ����ɱ������
		}
//		return lpPDAListCtrl->DefaultEditProc(hWnd,message,wParam,lParam);
		return CallWindowProc(lpPDAListCtrl->DefaultEditProc,hWnd,message,wParam,lParam);

}
/**************************************************
������static LRESULT DoEditKillFocus(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
�����������༭���ڴ���ʧȥ������Ϣ��WM_KILLFOCUS��
����: 
************************************************/
static LRESULT DoEditKillFocus(HWND hWnd)
{
	NMLVDISPINFO nmLVDispInfo;
	HWND hParent,hListView;
	LVITEM lvItem;
	TCHAR lpNewFileName[MAX_PATH];
	LPPDALISTCTRL lpPDAListCtrl;



		hListView = GetParent(hWnd); // �õ�LISTVIEW ���
		hParent = GetParent(hListView); // �õ������ھ��

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hListView,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
			return 0;
		

		GetWindowText(hWnd,lpNewFileName,MAX_PATH); // �õ������ı�

		DestroyWindow(hWnd); // �ƻ��༭����
		lpPDAListCtrl->DefaultEditProc = NULL;
		lpPDAListCtrl->hEdit = NULL;

		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = lpNewFileName;
		lvItem.iItem = lpPDAListCtrl->iEditItem;
		lvItem.iSubItem = 0;

		lpPDAListCtrl->iEditItem = -1;

		// ���ýṹ����
		nmLVDispInfo.hdr.hwndFrom=hWnd;
		nmLVDispInfo.hdr.idFrom=(UINT)GetWindowLong(hListView,GWL_ID);
		nmLVDispInfo.hdr.code=LVN_ENDLABELEDIT;
		nmLVDispInfo.item=lvItem;
		// �õ���Ŀ��Ϣ
		if (SendMessage(hParent,WM_NOTIFY,(WPARAM)hListView,(LPARAM)&nmLVDispInfo) == FALSE)
			return 0;
		// ������Ŀ�ı�
		SendMessage(hListView,LVM_SETITEMTEXT ,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);
		return 0;
}

/**************************************************
������static void KillEditControl(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
����������ɱ���༭����
����: 
************************************************/
static void KillEditControl(HWND hWnd)
{
	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
		if (lpPDAListCtrl==NULL)
			return ;
		if (lpPDAListCtrl->hEdit == NULL)
			return ;
		
		DestroyWindow(lpPDAListCtrl->hEdit); // �ƻ�����
		lpPDAListCtrl->DefaultEditProc = NULL;
		lpPDAListCtrl->iEditItem = -1;
		lpPDAListCtrl->hEdit = NULL;
}

/**************************************************
������static int LV_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
������
	IN hWnd -- ���ھ��
	IN nBar -- �Ǵ�ֱ����������ˮƽ������
	IN nPos -- ��ǰλ��
	IN bRedraw -- �ػ��־
����ֵ����
�������������ù�������ǰλ�á�
����: 
************************************************/
static int LV_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			si.nPos=nPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // ���ù�����λ��
			return 0;
}
/**************************************************
������static int LV_GetScrollPos(HWND hWnd, int nBar)
������
	IN hWnd -- ���ھ��
	IN nBar -- �Ǵ�ֱ����������ˮƽ������
����ֵ�����ص�ǰλ��
�����������õ���������ǰλ�á�
����: 
************************************************/
static int LV_GetScrollPos(HWND hWnd, int nBar)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			GetScrollInfo(hWnd,nBar,&si); // �õ�������λ��
			return si.nPos;
}

/**************************************************
������static BOOL LV_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
������
	IN hWnd -- ���ھ��
	IN nBar -- �Ǵ�ֱ����������ˮƽ������
	IN nMinPos -- ��Сλ��
	IN nMaxPos -- ���λ��
	IN bRedraw -- �ػ��־
����ֵ����
�������������ù�������Χ��
����: 
************************************************/
static BOOL LV_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_RANGE;
			si.nMin=nMinPos;
			si.nMax=nMaxPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // ���ù�������Χ
			return TRUE;
}


/**************************************************
������static int GetTextHeight(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ�������и�
�����������õ��и�
����:  Add By Jami chen in 2004.05.21
************************************************/
static int GetTextHeight(HWND hWnd)
{
	HDC hdc;
	TEXTMETRIC TextMetric;
//    int nTextHeight;

		hdc=GetDC(hWnd);

		// get current text property
		GetTextMetrics(hdc,&TextMetric);
		ReleaseDC(hWnd,hdc);
		
		return TextMetric.tmHeight;

}



/**************************************************
������static int GetTextWidth(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ�������ı����,һ��Ӣ���ַ��Ŀ��
�����������õ������ı����,һ��Ӣ���ַ��Ŀ��.
����:  Add By Jami chen in 2004.05.21
************************************************/
static int GetTextWidth(HWND hWnd)
{
	HDC hdc;
	TEXTMETRIC TextMetric;
//    int nTextHeight;

		hdc=GetDC(hWnd);

		// get current text property
		GetTextMetrics(hdc,&TextMetric);
		ReleaseDC(hWnd,hdc);
		
		if (TextMetric.tmAveCharWidth == 0)
			TextMetric.tmAveCharWidth = 12;
		return TextMetric.tmAveCharWidth;

}

/**************************************************
������static int GetCaptionHeight(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl)
������
	IN hWnd -- ���ھ��
	IN lpPDAListCtrl -- LISTCTRL�����ݽṹ
����ֵ�������ı����,һ��Ӣ���ַ��Ŀ��
�����������õ������ı����,һ��Ӣ���ַ��Ŀ��.
����:  Add By Jami chen in 2004.05.21
************************************************/
static int GetCaptionHeight(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl)
{

		if (lpPDAListCtrl == NULL)
		{ // �����û�еõ��ṹ
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // �õ�LISTCTRL�����ݽṹ
			if (lpPDAListCtrl==NULL)
				return 28;
		}

		
		return lpPDAListCtrl->iTextHeight + 4;//TextMetric.tmAveCharWidth;

}
