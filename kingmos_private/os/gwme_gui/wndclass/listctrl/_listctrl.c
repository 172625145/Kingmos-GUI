/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：列表控件
版本号：1.0.0.456
开发时期：2001-12-30
作者：jami chen 陈建明
修改记录：
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
声明：ATOM RegisterPDAListCtrlClass(HINSTANCE hInstance)
参数：
	IN hInstance -- 实例句柄
返回值：返回注册结果
功能描述：注册列表类。
引用: 
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
声明：LRESULT CALLBACK PDAListCtrlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN uMsg -- 要处理的消息
	IN wParam -- 消息参数
	IN lParam -- 消息参数
返回值：消息处理结果
功能描述：处理列表类消息。
引用: 
************************************************/
LRESULT CALLBACK PDAListCtrlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_CREATE:  // 创建列表控件
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
   	case WM_LBUTTONDOWN: // 鼠标左键按下
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE: // 鼠标移动
			return DoMouseMove(hWnd,wParam,lParam);
//	case LVM_MOUSEMOVE:
	case WM_SYSTIMER: // 处理系统时钟
			return DoSysTimer(hWnd,wParam,lParam);
//			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP: // 鼠标左键弹起
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_SETFOCUS: // 设置焦点
			SendBasicNotify(hWnd,NM_SETFOCUS);
			break;
    case WM_KILLFOCUS: // 杀死焦点
			return DoKillFocus(hWnd,wParam,lParam);
		case WM_CHAR: // 处理字符输入
			break;
		case WM_KEYDOWN: // 处理键盘输入
			return DoKeyDown(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏窗口
			// close this window
			ReleaseListCtrl(hWnd);
//			PostQuitMessage(0);
			break;
    case WM_VSCROLL: // 垂直滚动
      DoVScrollWindow(hWnd,wParam,lParam);
      break;
    case WM_HSCROLL:  // 水平滚动
      DoHScrollWindow(hWnd,wParam,lParam);
      break;
		case WM_CLEAR:  // 清除
			return DoClear(hWnd,wParam,lParam);
		case WM_STYLECHANGED:  // 风格改变
			{
				DoStyleChanged(hWnd,wParam,lParam);
				DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_SETCTLCOLOR	: // 设置控制颜色
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // 得到控制颜色
			return DoGetColor(hWnd,wParam,lParam);

/************************************************/
		case LVM_INSERTITEM:  // 插入一个条目
		  return DoInsertItem(hWnd,wParam,lParam);
		case LVM_INSERTCOLUMN: // 插入一列
			return DoInsertColumn(hWnd,wParam,lParam);
		case LVM_SETITEM: // 设置条目
			return DoSetItem(hWnd,wParam,lParam);
		case LVM_SETIMAGELIST:  // 设置图形列表
			return DoSetImageList(hWnd,wParam,lParam);
		case LVM_SORTITEMS: // 项目排序
			return DoSortItems(hWnd,wParam,lParam);
		case LVM_APPROXIMATEVIEWRECT: // 得到指定个数的条目所需的宽和高
			return DoApproximateViewRect(hWnd,wParam,lParam);
//		case LVM_ARRANGE:
//			break;
//		case LVM_CREATEDRAGIMAGE :
//			break;
		case LVM_DELETEALLITEMS : // 删除所有的条目
			return DoDeleteAllItems(hWnd,wParam,lParam);
		case LVM_DELETECOLUMN : // 删除一列
			return DoDeleteColumn(hWnd,wParam,lParam);
		case LVM_DELETEITEM : // 删除指定条目
			return DoDeleteItem(hWnd,wParam,lParam);
		case LVM_EDITLABEL : // 编辑标签
			return DoEditLabel(hWnd,wParam,lParam);
		case LVM_ENSUREVISIBLE : // 保证指定条目可见
			return DoEnsureVisible(hWnd,wParam,lParam);
		case LVM_FINDITEM : // 查找指定的条目
			return DoFindItem(hWnd,wParam,lParam);
//		case LVM_GETBKCOLOR :
//			break;
//		case LVM_GETBKIMAGE :
//			break;
//		case LVM_GETCALLBACKMASK :
//			break;
		case LVM_GETCOLUMN : // 得到列的信息
			return DoGetColumn(hWnd,wParam,lParam);
		case LVM_GETCOLUMNORDERARRAY : // 得到列的显示次序
			return DoGetColumnOrderArray(hWnd,wParam,lParam);
		case LVM_GETCOLUMNWIDTH : // 得到指定列的宽度
			return DoGetColumnWidth(hWnd,wParam,lParam);
		case LVM_GETCOUNTPERPAGE : // 得到每一页的条目个数
			return DoGetCountPerPage(hWnd,wParam,lParam);
//		case LVM_GETEDITCONTROL :
//			break;
//		case LVM_GETEXTENDEDLISTVIEWSTYLE :
//			break;
//		case LVM_GETHEADER :
//			break;
//		case LVM_GETHOTCURSOR :
//			break;
		case LVM_GETHOTITEM  : // 得到焦点条目的索引
			return DoGetHotItem(hWnd,wParam,lParam);
//		case LVM_GETHOVERTIME :
//			break;
		case LVM_GETIMAGELIST : // 得到指定类型的图形列表句柄
			DoGetImageList(hWnd ,wParam ,lParam);
//		case LVM_GETISEARCHSTRING :
//			break;
		case LVM_GETITEM : // 得到指定条目数据
			return DoGetItem(hWnd,wParam,lParam);
		case LVM_GETITEMCOUNT : // 得到当前控件的条目总数
			return DoGetItemCount(hWnd,wParam ,lParam);
//		case LVM_GETITEMPOSITION :
//			return DoGetItemPosition(hWnd ,wParam ,lParam);
		case LVM_GETITEMRECT : // 得到指定条目的矩形大小
			return DoGetItemRect(hWnd,wParam,lParam);
		case LVM_GETITEMSPACING :  // 得到条目间的间距
			return DoGetItemSpacing(hWnd,wParam,lParam);
		case LVM_GETITEMSTATE : // 得到指定条目的状态
			return DoGetItemState(hWnd ,wParam ,lParam);
		case LVM_GETITEMTEXT : // 得到指定条目的文本
			return DoGetItemText(hWnd,wParam,lParam);
		case LVM_GETNEXTITEM : // 查找下一个条目
			return DoGetNextItem(hWnd,wParam,lParam);
//		case LVM_GETNUMBEROFWORKAREAS :
//		break;
		case LVM_GETORIGIN : // 得到控件原点的坐标
			return DoGetOrigin(hWnd,wParam,lParam);
		case LVM_GETSELECTEDCOUNT : // 得到当前已经选择的条目的个数
			return DoGetSelectedCount(hWnd,wParam,lParam);
		case LVM_GETSELECTIONMARK : // 得到选择条目的索引
			return DoGetSelectionMark(hWnd,wParam,lParam);
		case LVM_GETSTRINGWIDTH : // 得到指定字符串的宽度
			return DoGetStringWidth(hWnd,wParam,lParam);
		case LVM_GETSUBITEMRECT : // 得到指定字条目的矩形大小
			return DoGetSubItemRect(hWnd,wParam,lParam);
//	case LVM_GETTEXTBKCOLOR :
//	break;
//		case LVM_GETTEXTCOLOR :
//			break;
//		case LVM_GETTOOLTIPS :
//			break;
		case LVM_GETTOPINDEX : // 得到第一个可见条目的索引
			return DoGetTopIndex(hWnd,wParam,lParam);
//		case LVM_GETUNICODEFORMAT :
//			break;
		case LVM_GETVIEWRECT : // 得到控件的视图大小
			return DoGetViewRect(hWnd,wParam,lParam);
//		case LVM_GETWORKAREAS :
//			break;
		case LVM_HITTEST : // 测试点的位置
			return DoHitTest(hWnd,wParam,lParam);
		case LVM_REDRAWITEMS : // 重绘指定的条目
			return DoRedrawItems(hWnd,wParam,lParam);
		case LVM_SCROLL : // 滚动窗口
			return DoScroll(hWnd,wParam,lParam);
//		case LVM_SETBKCOLOR :
//			break;
//		case LVM_SETBKIMAGE :
//			break;
//		case LVM_SETCALLBACKMASK :
//			break;
		case LVM_SETCOLUMN : // 设置指定的列
			return DoSetColumn(hWnd,wParam,lParam);
		case LVM_SETCOLUMNORDERARRAY : // 设置列的显示次序
			return DoSetColumnOrderArray(hWnd,wParam,lParam);
		case LVM_SETCOLUMNWIDTH : // 设置指定列的宽度
			return DoSetColumnWidth(hWnd ,wParam ,lParam);
//		case LVM_SETEXTENDEDLISTVIEWSTYLE :
//			break;
//		case LVM_SETHOTCURSOR :
//			break;
		case LVM_SETHOTITEM  : // 设置焦点索引
			return DoSetHotItem(hWnd,wParam,lParam);
//		case LVM_SETHOVERTIME :
//			break;
		case LVM_SETICONSPACING : // 设置图标之间的间距
			return DoSetIconSpacing(hWnd,wParam,lParam);
		case LVM_SETITEMCOUNT : // 设置控件的条目总数
			return DoSetItemCount(hWnd,wParam,lParam);
//		case LVM_SETITEMPOSITION :
//			break;
//		case LVM_SETITEMPOSITION32 :
//			break;
		case LVM_SETITEMSTATE :  // 设置条目状态
			return DoSetItemState(hWnd,wParam,lParam);
		case LVM_SETITEMTEXT : // 设置条目文本
			return DoSetItemText(hWnd,wParam,lParam);
	  case LVM_SETSELECTIONMARK : // 设置指定条目为选择条目
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
		case LVM_SUBITEMHITTEST : // 测试点所在的子项的位置
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
声明：static LRESULT DoCreateListCtrl(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：创建列表控件
引用: 
************************************************/
static LRESULT DoCreateListCtrl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		

 			lpPDAListCtrl=(LPPDALISTCTRL)malloc(sizeof(PDALISTCTRL));  // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			lpPDAListCtrl->iMaxSubItemNum=DEFAULTSUBITEMNUM;
			lpPDAListCtrl->iSubItemNum=0;
			lpPDAListCtrl->lpSubItem=(LPSUBITEM)malloc(sizeof(SUBITEM)*lpPDAListCtrl->iMaxSubItemNum);  // 分配子条目列表
			if (lpPDAListCtrl->lpSubItem==NULL)
			{
					free(lpPDAListCtrl);
					lpPDAListCtrl->iMaxSubItemNum=0;
					return 0;
			}
			memset(lpPDAListCtrl->lpSubItem,0,(sizeof(SUBITEM)*lpPDAListCtrl->iMaxSubItemNum));
			lpPDAListCtrl->lpColumnOrderArray=(int *)malloc(sizeof(int)*lpPDAListCtrl->iMaxSubItemNum); // 分配列次序列表
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
			// 初始化参数
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
			
			// 初始化颜色
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

			SetWindowLong(hWnd,0,(DWORD)lpPDAListCtrl); // 设置结构到窗口
			SetVScrollBar(hWnd); // 设置滚动条

			lpPDAListCtrl->iLineHeight = GetLineHeight(hWnd); // Add By Jami chen in 2004.05.21
			lpPDAListCtrl->iTextHeight = GetTextHeight(hWnd); // Add By Jami chen in 2004.05.21
			lpPDAListCtrl->iTextWidth = GetTextWidth(hWnd); // Add By Jami chen in 2004.05.21

			return 0;
}
/**************************************************
声明：static void ReleaseListCtrl(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：释放列表控件
引用: 
************************************************/
static void ReleaseListCtrl(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		int i;
		BOOL bText=TRUE;

		KillEditControl(hWnd); // 杀死编辑控件
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			if (lpPDAListCtrl->lpSubItem)
			{ // 删除所有子条目
				for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
				{ // 删除列
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
				{ // 删除条目
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
      // 破坏图象列表
#ifdef USE_ICON_IMAGE
	  ImageList_Destroy(lpPDAListCtrl->lpImageList);
	  ImageList_Destroy(lpPDAListCtrl->lpImageListSmall);
#endif
	  free(lpPDAListCtrl); // 释放结构

}
/**************************************************
声明：static LRESULT DoInsertColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：成功返回0，否则返回-1
功能描述：插入一列，处理LVM_INSERTCOLUMN消息。
引用: 
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
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return -1;
//			dwStyle=GetWindowLong(hWnd,GWL_STYLE);

			iCol=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
			lplvColumn=(LPLVCOLUMN)lParam; // 得到列数据
			if (iCol>lpPDAListCtrl->iSubItemNum) return -1;

	    if(iCol==lpPDAListCtrl->iMaxSubItemNum)
			{// 插入一列到最后
				// 增加列的个数
				lpReallocPtr=realloc(	lpPDAListCtrl->lpSubItem,sizeof(SUBITEM)*(lpPDAListCtrl->iMaxSubItemNum+DEFAULTSUBITEMNUM)); // 分配新的列表
				if (lpReallocPtr==NULL)
					return -1;
				lpPDAListCtrl->lpSubItem=lpReallocPtr; 
				lpPDAListCtrl->iMaxSubItemNum+=DEFAULTSUBITEMNUM; // 设置最大的条目个数

				lpReallocPtr=realloc(	lpPDAListCtrl->lpColumnOrderArray,sizeof(int)*lpPDAListCtrl->iMaxSubItemNum); // 分配新的列次序列表
				if (lpReallocPtr==NULL)
					return -1;
				lpPDAListCtrl->lpColumnOrderArray=lpReallocPtr;
			}
	    memmove( &lpPDAListCtrl->lpSubItem[iCol+1], &lpPDAListCtrl->lpSubItem[iCol], (lpPDAListCtrl->iSubItemNum-iCol)*sizeof(SUBITEM) ); // 移动原来的列数据
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 设置新的列表次序列表数据
				if (lpPDAListCtrl->lpColumnOrderArray[i]>=iCol)
					lpPDAListCtrl->lpColumnOrderArray[i]++;
			}
			memmove( &lpPDAListCtrl->lpColumnOrderArray[iCol+1], &lpPDAListCtrl->lpColumnOrderArray[iCol], (lpPDAListCtrl->iSubItemNum-iCol)*sizeof(int) ); // 移动原来的列表数据
			
			lpPDAListCtrl->lpColumnOrderArray[iCol]=iCol; // 设置当前的列次序

			lpPDAListCtrl->iSubItemNum++;	 // 列条目加1
			lpPDAListCtrl->lpSubItem[iCol].CaptionFmt=lplvColumn->mask;
			lpPDAListCtrl->lpSubItem[iCol].ContentFmt=0;
			lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption=NULL;
			if (lplvColumn->mask&LVCF_TEXT)
			{ // 得到列的标题
				cbCaptionLen=strlen(lplvColumn->pszText);
				lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption=(LPTSTR)malloc(cbCaptionLen+1);
				strcpy(lpPDAListCtrl->lpSubItem[iCol].lpSubItemCaption,lplvColumn->pszText);
			}
			if (lplvColumn->mask&LVCF_WIDTH)
			{ // 设置列的宽度
				lpPDAListCtrl->lpSubItem[iCol].iSubItemWidth=lplvColumn->cx;
			}
			else
			{
				lpPDAListCtrl->lpSubItem[iCol].iSubItemWidth=DEFAULTWIDTH;
			}
			// 设置列的风格
			if (lplvColumn->mask&LVCF_FMT)
			{
				lpPDAListCtrl->lpSubItem[iCol].ContentFmt=lplvColumn->fmt;
			}
			if (lplvColumn->mask&LVCF_IMAGE)
			{
				lpPDAListCtrl->lpSubItem[iCol].iImage=lplvColumn->iImage;
			}
			//插入一个主列
			if (iCol==0) // Insert main item 
			{
					lpPDAListCtrl->iMaxItemNum=DEFAULTITEMNUM;
					lpPDAListCtrl->iItemNum=0;
					PtrListCreate(&(lpPDAListCtrl->lpSubItem[0].plSubItemContent),(ccIndex)lpPDAListCtrl->iMaxItemNum,(ccIndex)16);
			}
			else
			{ // 插入一个子列
					//lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
					if (!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_FILE))
					{
						PtrListCreate((&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent),lpPDAListCtrl->iMaxItemNum,16);
					}
			}
			// 得到列表总宽度
			Width=0;
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{
				Width+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
			}
			lpPDAListCtrl->iWidth=Width;
			SetHScrollBar(hWnd);	// 设置水平滚动条
			return 0; 
}

/**************************************************
声明：static LRESULT DoPaint(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制控制,处理WM_PAINT消息。
引用: 
************************************************/
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
		DrawListCtrlCaption(hWnd,hdc); // 绘制标题
		DrawListCtrl(hWnd,hdc); // 绘制数据
		return 0;
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
		POINTS points;
		int iPointPosition,CurItem;
		HDC hdc;
		LPPDALISTCTRL lpPDAListCtrl;
		int xCoordinate;
		int i,iSubItem;
		DWORD dwStyle;
		LPMAINITEM lpMainItem;
//		POINT pt;
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
      if (GetFocus()!=hWnd) // 得到焦点
			  SetFocus(hWnd); // 设置焦点
//			points=MAKEPOINTS(lParam);
      points.x=LOWORD(lParam);
      points.y=HIWORD(lParam);
			SetCapture(hWnd);

			iPointPosition=PositionInWindow(hWnd,points,&CurItem); // 得到点所在的位置
			if (iPointPosition==CAPTIONITEM)
			{  // 点在标题条目
//					SetCapture(hWnd);
					hdc=GetDC(hWnd);
 					DrawCaptionItem(hWnd,hdc,CurItem,FALSE); // 绘制标题条目按下
					ReleaseDC(hWnd,hdc);
					lpPDAListCtrl->iActivePos=CAPTIONITEM; // 设置当前状态
					lpPDAListCtrl->iActiveItem=CurItem;
					lpPDAListCtrl->bPressed=TRUE;
			}
			if (iPointPosition==CAPTIONSEPARATE)
			{ // 点在标题间隔
//					SetCapture(hWnd);
					hdc=GetDC(hWnd);
					// 得到分隔线的位置
					xCoordinate=lpPDAListCtrl->cx;
					for (i=0;i<=CurItem;i++)
					{
						iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
						xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth; 
					}
//					xCoordinate+=lpPDAListCtrl->lpSubItem[CurItem].iSubItemWidth;
					DrawLineSeparate(hWnd,hdc,xCoordinate); // 绘制分隔线
					ReleaseDC(hWnd,hdc);
					lpPDAListCtrl->iActivePos=CAPTIONSEPARATE;  // 设置当前状态
					lpPDAListCtrl->iActiveItem=CurItem;
					lpPDAListCtrl->iPenCoordinate=(int)points.x;
					lpPDAListCtrl->iNewWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			}
			if (iPointPosition==LISTITEM)
			{ // 点在列式条目
					// 设置当前状态
					lpPDAListCtrl->iActivePos=LISTITEM;
					lpPDAListCtrl->iActiveItem=CurItem;
				  GetPointPosition(hWnd,points,&lpPDAListCtrl->iDownRow,&lpPDAListCtrl->iDownColumn);
				  GetPointPosition(hWnd,points,&lpPDAListCtrl->iRow,&lpPDAListCtrl->iColumn);
					if (CurItem>=lpPDAListCtrl->iItemNum)
						 return 0;

					dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到当前风格
					if (dwStyle&LVS_SINGLESEL)
					{ // 只能单选
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,CurItem); // 得到当前条目结构
						if (lpPDAListCtrl->iFocusItem==CurItem)
						{ // 当前条目等于焦点条目
							if (SendMessage(hWnd,LVM_GETSELECTEDCOUNT,0,0)==1)
							{ // 当前只有一个条目选择
								lpPDAListCtrl->bActiveItem=TRUE;  // 当前条目为活动条目
								return 0;
							}
						}
						lpPDAListCtrl->bActiveItem=FALSE;
						// clear old Invert
						ClearAllInvert(hWnd);
						ClearFocus(hWnd);
					
						if (lpMainItem)
						{  // 设置当前条目得到焦点和选择
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
						DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // 绘制当前条目
					}
					else
					{ // 多选条目
						if (lpPDAListCtrl->iFocusItem>=0)
						{// 当前有焦点窗口
							lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem); // 得到主条目结构
							if (lpMainItem)
							{  // 取消当前状态的焦点属性
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
								DrawItem(hWnd,lpPDAListCtrl->iFocusItem); // 绘制焦点条目
							}
						}
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,CurItem); // 得到当前条目的结构
						if (lpMainItem)
						{  // 设置当前条目为焦点，选择条目
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
						DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // 绘制当前条目
					}
					PostMessage(hWnd,LVM_ENSUREVISIBLE,(WPARAM)CurItem,(LPARAM)FALSE); // 保证当前条目可见
	
		}
		if (iPointPosition==LISTBLANKSPACE)
		{  // 点在列表部分空白位置
			GetPointPosition(hWnd,points,&lpPDAListCtrl->iDownRow,&lpPDAListCtrl->iDownColumn);
			GetPointPosition(hWnd,points,&lpPDAListCtrl->iRow,&lpPDAListCtrl->iColumn);
			ClearAllInvert(hWnd);  // 清除所有的选择
//			ClearFocus(hWnd);
			lpPDAListCtrl->iActivePos=LISTITEM;
		}
		if (iPointPosition==BLANKSPACE)
		{ // 点在空白位置
			lpPDAListCtrl->iActivePos=BLANKSPACE;
		}
		return 0;
}
/**************************************************
声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(X,Y),鼠标坐标
返回值：无
功能描述：处理WM_MOUSEMOVE消息。
引用: 
************************************************/
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	POINTS points;
	int iPointPosition,CurItem,xCoordinate;
	HDC hdc;
	int i,iSubItem;

			if (GetCapture()!=hWnd)  // 是否已经抓住鼠标
				return 0;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
				return 0;
			points.x=LOWORD(lParam);
			points.y=HIWORD(lParam);

			if (lpPDAListCtrl->iActivePos==CAPTIONITEM)
			{  // 当前点在标题条目
					iPointPosition=PositionInWindow(hWnd,points,&CurItem); // 得到当前条目的位置
					if ((iPointPosition==lpPDAListCtrl->iActivePos)&&(CurItem==lpPDAListCtrl->iActiveItem))
					{ // 在原来按下的条目
						if (lpPDAListCtrl->bPressed==TRUE) // 仍在原来的位置
						  return 0;
						// 已经离开原来的位置，设置重新进入原来的位置
						hdc=GetDC(hWnd);
 						DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iActiveItem,FALSE);
						ReleaseDC(hWnd,hdc);
						lpPDAListCtrl->bPressed=TRUE;
					}
					else
					{ // 离开原来的位置
						if (lpPDAListCtrl->bPressed==FALSE)  // 已经离开
						  return 0;
						// 还没有离开，设置离开原来的位置
						hdc=GetDC(hWnd);
 						DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iActiveItem,TRUE);
						ReleaseDC(hWnd,hdc);
						lpPDAListCtrl->bPressed=FALSE;
					}
			}
			if (lpPDAListCtrl->iActivePos==CAPTIONSEPARATE)
			{ // 点在标题间隔，拖动条目的大小
					if (lpPDAListCtrl->iPenCoordinate==points.x)
					  return 0;
				  if (lpPDAListCtrl->iActiveItem<0)
				   return 0;
				  // 得到间隔的位置
					xCoordinate=lpPDAListCtrl->cx;
					for (i=0;i<lpPDAListCtrl->iActiveItem;i++)
					{
						iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
						xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
					}
					hdc=GetDC(hWnd);
					DrawLineSeparate(hWnd,hdc,(int)(xCoordinate+lpPDAListCtrl->iNewWidth)); // 绘制间隔线

					// 得到新的位置
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[lpPDAListCtrl->iActiveItem];
					lpPDAListCtrl->iNewWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth+(points.x-lpPDAListCtrl->iPenCoordinate);
					// 设置新的长度
					if (lpPDAListCtrl->iNewWidth<0) 
					   lpPDAListCtrl->iNewWidth=0;
					DrawLineSeparate(hWnd,hdc,(int)(xCoordinate+lpPDAListCtrl->iNewWidth)); // 绘制新的位置
					ReleaseDC(hWnd,hdc);
			}
			if (lpPDAListCtrl->iActivePos==LISTITEM)
			{ // 点在列式条目上
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
					
					// 设置新的选择条目

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
					SendMessage(hWnd,LVM_ENSUREVISIBLE,(WPARAM)iCurItem,(LPARAM)FALSE);  // 保证当前条目可见

					lpPDAListCtrl->lParam=lParam;
					lpPDAListCtrl->wParam=wParam;
//					ClientToScreen(hWnd,&points);
//					lParam=MAKELONG(points.x,points.y);
//					PostMessage(hWnd,LVM_MOUSEMOVE,wParam,lParam);
					SetSysTimer(hWnd,1,10,NULL); // 设置系统时钟
			}
		return 0;
}
/**************************************************
声明：static LRESULT DoSysTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：处理WM_SYSTIMER消息。
引用: 
************************************************/
static LRESULT DoSysTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return 0;
		return DoMouseMove(hWnd,lpPDAListCtrl->wParam,lpPDAListCtrl->lParam); // 发送移动到当前点
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


		KillSysTimer(hWnd,1);  // 杀死系统时钟
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return 0;

//			points=MAKEPOINTS(lParam);
//	  SetCapture(0);
	  ReleaseCapture();  // 取消抓住鼠标
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
		{  // 点在列式条目
				dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
				if (dwStyle&LVS_SINGLESEL)
				{  // 单选条目
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
								SendActiveItem(hWnd,CurItem); // 激活当前条目
							}  
						}
					}
				}
		}
		lpPDAListCtrl->iActiveItem=-1;  // 设置当前没有激活条目
		lpPDAListCtrl->iActivePos=BLANKSPACE;
		SendBasicNotify(hWnd,NM_RELEASEDCAPTURE); // 发送通知消息
		return 0;
}
/**************************************************
声明：static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam, LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 指向LVITEM的指针
返回值：成功返回插入条目的索引，否则返回-1
功能描述：插入一个条目，处理LVM_INSERTITEM消息。
引用: 
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
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return -1;
#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif

			lpItem=(LPLVITEM)lParam;

			lpMainItem=(LPMAINITEM)malloc(sizeof(MAINITEM)); // 得到主条目
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
			{ // 结构文本条目有效
					if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
					{
						lpMainItem->lpText=lpItem->pszText;// this is a file No.
					}
					else
					{  // 插入一个文本
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
			{ // 图形条目有效
				lpMainItem->iImage=lpItem->iImage;
			}
			if (lpItem->mask&LVIF_STATE)
			{ // 状态条目有效
				lpMainItem->State=lpItem->state&lpItem->stateMask;
			}
			if (lpItem->mask&LVIF_PARAM)
			{ // 参数条目有效
				lpMainItem->lParam=lpItem->lParam;
			}

			dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
			if (dwStyle&LVS_SORTASCENDING)
			{// 升序排列
				 iInsertIndex=AutoSortItem(lpPDAListCtrl,lpMainItem,SORTASCENDING);
			}
			else if (dwStyle&LVS_SORTDESCENDING)
			{ // 降序排列
				 iInsertIndex=AutoSortItem(lpPDAListCtrl,lpMainItem,SORTDESCENDING);
			}
			else
			{// 不排序
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
			if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iInsertIndex,lpMainItem)==FALSE) // 插入条目
				return -1;
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{  // 插入空的子项
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
				{
					if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iInsertIndex,NULL)==FALSE)
						return -1;
				}
			}
			lpPDAListCtrl->iItemNum++;  // 增加一个条目
			SetVScrollBar(hWnd);
			iPageItemNum=GetPageItemNum(hWnd); // 得到一页的条目个数
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
声明：static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 指向LVITEM的指针
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置条目。
引用: 
************************************************/
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPLVITEM lpItem;
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		LPTSTR lpString;
		int cbStringLen;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
			lpItem=(LPLVITEM)lParam; // 得到条目结构
			if (lpItem->iItem>lpPDAListCtrl->iItemNum)
				return FALSE;
			if (lpItem->iSubItem>lpPDAListCtrl->iSubItemNum)
				return FALSE;

			
			if (lpItem->mask&LVIF_TEXT)
			{  // 条目文本有效
// !!! Add By Jami Chen in 2002.05.09
// when the list is a file prop ,then direct set the pzstext
				if (lpPDAListCtrl->lpSubItem[lpItem->iSubItem].ContentFmt&LVCFMT_FILE)
				{ // 文件属性条目
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
				{ // 文本属性
// !!! Add End By Jami chen in 2002.05.09
					cbStringLen=strlen(lpItem->pszText);
					lpString=(LPTSTR)malloc(cbStringLen+1);  //分配 
					if (lpString==NULL)
					{
							return -1;
					}
					strcpy(lpString,lpItem->pszText);
					if (lpItem->iSubItem==0)
					{ // 当前条目是主条目
						lpMainItem=(LPMAINITEM)PtrListAt(&(lpPDAListCtrl->lpSubItem[lpItem->iSubItem]).plSubItemContent,lpItem->iItem);
						if (lpMainItem==NULL)
							return 0;
						if (lpMainItem->lpText)
							free(lpMainItem->lpText);
						lpMainItem->lpText=lpString;
					}
					else
					{  // 当前条目是子条目
						LPTSTR lpOldString;
						lpOldString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);
						if (lpOldString!=NULL)
							free(lpOldString);
						PtrListAtPut(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem,lpString);
					}
				}
			}
			if (lpItem->mask&LVIF_IMAGE)
			{// 图形条目有效
					if (lpItem->iSubItem==0)
					{ // 当前是主条目
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);;
						if (lpMainItem==NULL)
							return 0;
//						if (lpMainItem->lpText)
//							free(lpMainItem->lpText);
//						lpMainItem->lpText=lpString;
						lpMainItem->iImage=lpItem->iImage;
					}
					else
					{ // 当前是子条目
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
			{ // 状态条目有效
					if (lpItem->iSubItem==0)
					{  // 当前是主条目
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);;
						if (lpMainItem==NULL)
							return 0;
						lpMainItem->State=lpItem->state&lpItem->stateMask;
					}
			}
			if (lpItem->mask&LVIF_PARAM)
			{ // 参数条目有效
					if (lpItem->iSubItem==0)
					{
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[lpItem->iSubItem].plSubItemContent,lpItem->iItem);;
						if (lpMainItem==NULL)
							return 0;
						lpMainItem->lParam=lpItem->lParam;
					}
			}
            InvalidateItem(hWnd,(int)lpItem->iItem); // 无效条目
			return 0;
}
/**************************************************
声明：static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 图形列表的类型
	IN lParam -- HIMAGELIST 图形列表句柄
返回值：成功返回原来图形列表的句柄，否则返回NULL
功能描述：设置图形列表，处理LVM_SETIMAGELIST消息。
引用: 
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
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iImageList=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif

			himl=(HIMAGELIST)lParam;  // 图象列表

			if (iImageList==LVSIL_SMALL)
			{ // 小图表
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
			{ // 大图标
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
声明：static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- LOWORD 滚动标志
	IN lParam -- 保留
返回值：无
功能描述：垂直滚动窗口，处理WM_HSCROLL消息。
引用: 
************************************************/
static LRESULT DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nPageLines;
    int yPos;
        
		KillEditControl(hWnd);  // 杀死编辑控件
			nPageLines=GetPageLine(hWnd);
      yPos=LV_GetScrollPos(hWnd,SB_VERT); //得到滚动位置
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
        VScrollWindow(hWnd,nScrollLine); // 滚动窗口
				return 0;
}
/****************************************************************************************************/
/**************************************************
声明：static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- LOWORD 滚动标志
	IN lParam -- 保留
返回值：无
功能描述：水平滚动窗口，处理WM_HSCROLL消息。
引用: 
************************************************/
static LRESULT DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nPageLines;
    int xPos;
        
		KillEditControl(hWnd); // 杀死编辑控制
			nPageLines=GetWindowWidth(hWnd)/WIDTHSTEP;
      xPos=LV_GetScrollPos(hWnd,SB_HORZ); // 得到当前滚动块位置
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
        HScrollWindow(hWnd,nScrollLine);  // 水平滚动窗口
				return 0;
}
/****************************************************************************************************/
/****************************************************************************************************/
#ifdef _PLAN_ONE_
/**************************************************
声明：static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 比较参数
	IN lParam -- 比较函数
返回值：无
功能描述：项目排序，处理LVM_SORTITEMS消息。
引用: 
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

			KillEditControl(hWnd);  // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;

			lParamSort=(LPARAM)wParam; 

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
			// 
			Compare=(CompareFunc)lParam;  // 得到比较函数
			for (i=0;i<lpPDAListCtrl->iItemNum;i++)
			{  // 搜索所有的条目
				iPreItem=i;
				lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iPreItem); // 得到参数1 的条目结构
				lParam1=lpMainItem->lParam;
				for (j=i+1;j<lpPDAListCtrl->iItemNum;j++)
				{
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,j); // 得到条目2 的条目结构
					lParam2=lpMainItem->lParam;
					if (Compare(lParam1,lParam2,lParamSort)>0) // 比较条目
					{
						iPreItem=j;
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iPreItem);
						lParam1=lpMainItem->lParam;
					}
				}
				if (iPreItem!=i)
				{
					SwapItem(lpPDAListCtrl,i,iPreItem); // 交换条目
				}
			}
			lpPDAListCtrl->iStartItem=0;
			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE);
			GetListFocus(hWnd);  // 得到列式焦点

			InvalidateRect(hWnd,NULL,TRUE);  // 无效窗口
			return 0;
}
/**************************************************
声明：static void SwapItem(LPPDALISTCTRL lpPDAListCtrl,int iItem1,int iItem2)
参数：
	IN lpPDAListCtrl -- 列表控制结构指针
	IN iItem1 -- 条目1的索引
	IN iItem2 -- 条目2的索引	
返回值：无
功能描述：交换二条目。
引用: 
************************************************/
static void SwapItem(LPPDALISTCTRL lpPDAListCtrl,int iItem1,int iItem2)
{
	LPVOID lpTemp1,lpTemp2;
	int i;

			lpTemp1=(LPVOID)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem1); // 得到条目1 的数据结构
			lpTemp2=(LPVOID)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem2); // 得到条目2 的数据结构
			PtrListAtPut(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem1,lpTemp2); // 将条目2 的数据设置到条目1 的索引
			PtrListAtPut(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem2,lpTemp1); // 将条目1 的数据设置到条目2 的索引
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 交换所有子条目的数据
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
	            { // 如果不是文件格式的数据，就需要交换数据
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
声明：static LRESULT DoSortItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 比较参数
	IN lParam -- 比较函数
返回值：无
功能描述：项目排序，处理LVM_SORTITEMS消息。
引用: 
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

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;

			lParamSort=(LPARAM)wParam;  // 得到排序参数

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			Compare=(CompareFunc)lParam;  //得到比较函数

      lpPtrList=malloc(sizeof(PTRLIST)*(lpPDAListCtrl->iSubItemNum+1));  // 分配一个指针列表来保存原来的数据

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
			{  // 清除子条目的数据
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
				{
           PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[i].plSubItemContent);
				}
			}

      // Insert item
			for (iIndex=0;iIndex<lpPDAListCtrl->iItemNum;iIndex++)
      {
				 lpMainItem=(LPMAINITEM)PtrListAt(&lpPtrList[0],iIndex); // 得到要插入条目的数据
         iInsertIndex=InsertPosition(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpMainItem,Compare,lParamSort); // 得到插入的位置
			   if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iInsertIndex,lpMainItem)==FALSE)
				   return FALSE;
			   for (iCol=1;iCol<lpPDAListCtrl->iSubItemNum;iCol++)
			   { // 插入子条目
				   if (!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_FILE))
				   {
				     lpSubItem=(LPVOID)PtrListAt(&lpPtrList[iCol],iIndex);
					   if (PtrListAtInsert(&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent,iInsertIndex,lpSubItem)==FALSE)
						   return FALSE;
				   }
         }
      }
      free(lpPtrList); // 释放分配的内存
			lpPDAListCtrl->iStartItem=0;
			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE); // 设置滚动滑块的位置
			GetListFocus(hWnd); // 得到焦点

      InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
      return TRUE;
}
/**************************************************
声明：static int InsertPosition(LPPTRLIST lpPtrList,LPMAINITEM lpMainItem,CompareFunc Compare,LPARAM lParamSort)
参数：
	IN lpPtrList --  条目数组
	IN lpMainItem -- 要插入的条目
	IN Compare -- 比较函数
	IN lParamSort -- 比较参数
返回值：返回插入位置
功能描述：得到条目要插入的位置。
引用: 
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

     // 使用二分法查询的方法找到要插入的位置
     lParam1=lpMainItem->lParam;
     while(1)
     {
       middle=(start+end)/2; // 得到中间的索引
       if (middle==start||middle==end)
       {
       	 lpItem=(LPMAINITEM)PtrListAt(lpPtrList,end); // 得到最后一个条目的索引的数据
         lParam2=lpItem->lParam;
			   if (Compare(lParam1,lParam2,lParamSort)>=0) // 大于最后一个条目
         {
           return end+1; // 插到最后一个条目的后面
         }
       	 lpItem=(LPMAINITEM)PtrListAt(lpPtrList,start); // 得到第一个条目的数据
         lParam2=lpItem->lParam;
			   if (Compare(lParam1,lParam2,lParamSort)<0) // 小于第一个条目
         {
           return start; // 插到第一个条目的前面
         }
         return end; // 插到最后一个条目的前面
       }
			 lpItem=(LPMAINITEM)PtrListAt(lpPtrList,middle); // 得到中间条目的数据
       lParam2=lpItem->lParam;
			 if (Compare(lParam1,lParam2,lParamSort)>0) // 大于中间条目
       {
         start=middle; // 将开始条目的索引设到中间条目的位置
       }
       else
       {
         end=middle; // 将结束条目设到中间条目的位置
       }
     }
}
#endif
/**************************************************
声明：static LRESULT DoApproximateViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目的个数
	IN lParam -- 保留
返回值：低WORD ，返回宽度，高WORD ，返回高度。
功能描述：得到指定个数的条目所需的宽和高，处理LVM_APPROXIMATEVIEWRECT消息。
引用: 
************************************************/
static LRESULT DoApproximateViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		int iCount,cx,cy;
		int iLineHeight,iItemNumInLine,iLine;
		DWORD dwStyle;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口的风格
			iCount=(int)wParam; // 得到要计算条目的个数
			if (iCount==-1)
				iCount=(int)SendMessage(hWnd,LVM_GETITEMCOUNT,0,0); // 计算所有条目

			if ((dwStyle&LVS_TYPEMASK)==LVS_REPORT)
			{  // 当前状态是REPORT风格
				LPPDALISTCTRL lpPDAListCtrl;
					lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);
					if (lpPDAListCtrl==NULL)
						return 0;
					cx=lpPDAListCtrl->iWidth; // 宽度是REPORT的宽度
					if (ExistCaption(hWnd))
						cy=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT; // 有CAPTION的高度
					else
						cy=0;
			}
			else
			{
				cx=GetWindowWidth(hWnd); // 窗口的宽度
				cy=0;
			}
			iLineHeight=GetLineHeight(hWnd); // 得到行高
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行可以放置的条目个数
			iLine=(iCount+iItemNumInLine-1)/iItemNumInLine; // 得到要放置的行数
			cy+=iLine*iLineHeight; // 得到需要的高度
			return MAKELRESULT(cx,cy);
}
/****************************************************************************************************/
/**************************************************
声明：static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：杀死焦点，处理WM_KILLFOCUS消息。
引用: 
************************************************/
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	DWORD dwStyle;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口的风格
			ClearFocus(hWnd); // 清除焦点
			if (!(dwStyle&LVS_SHOWSELALWAYS))
				ClearAllInvert(hWnd); // 清除所有选择
			SendBasicNotify(hWnd,NM_KILLFOCUS); // 通知父窗口 
			return 0;
}
/**************************************************
声明：static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定要删除的条目
	IN lParam -- 保留
返回值：成功返回TRUE，否则返回FALSE
功能描述：删除指定条目，处理LVM_DELETEITEM消息。
引用: 
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
			
			KillEditControl(hWnd); // 杀死编辑控制
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam; // 得到条目索引

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
			SendNormalNotify(hWnd,LVN_DELETEITEM,iItem,0,0,0,pt); // 通知父窗口
			RemoveItem(lpPDAListCtrl,iItem);  // 删除条目
			LV_SetScrollPos(hWnd,SB_VERT,(lpPDAListCtrl->iStartItem/GetItemNumInLine(hWnd)),TRUE); // 设置滚动条位置
			SetVScrollBar(hWnd);  // 设置滚动条
			iPageItemNum=GetPageItemNum(hWnd);// 得到页条目个数

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
声明：static LRESULT DoEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：成功返回TRUE，否则返回FALSE
功能描述：编辑标签，处理LVM_EDITLABEL消息。
引用: 
************************************************/
static LRESULT DoEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iItem;
	LPMAINITEM lpMainItem;
	RECT rectItem,rectListView;
	DWORD dwStyle;
	int iImageWidth;
		
		KillEditControl(hWnd); // 杀死编辑控件
		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return 0;
		iItem=(int)wParam;

		if (SendMessage(hWnd,LVM_GETITEMRECT ,iItem,(LPARAM)&rectItem) == FALSE) // 得到指定条目的矩形
			return FALSE;
		
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if ( (dwStyle&LVS_TYPEMASK)==LVS_ICON)
		{ 
		}
		else
		{
			lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到指定条目的数据结构
			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // 有图形
				// 得到图形的宽度
				if (lpPDAListCtrl->sizeSmallIcon.cx>lpPDAListCtrl->lpSubItem[0].iSubItemWidth)
						iImageWidth=lpPDAListCtrl->lpSubItem[0].iSubItemWidth;
				else
						iImageWidth=(int)lpPDAListCtrl->sizeSmallIcon.cx;
				rectItem.left += iImageWidth +1; // 得到文本的开始位置
				if ( (dwStyle&LVS_TYPEMASK)==LVS_REPORT)
				{
					rectItem.left+= 2*IMAGESPACE;
				}
			}
			// 得到编辑框的位置与大小

			// !!! modified By Jami chen in 2004.05.21
			//rectItem.top += (USETEXTHEIGHT - 18) /2;
			//rectItem.bottom = rectItem.top +  18 ;
			//rectItem.right = rectItem.left + strlen(lpMainItem->lpText) * 8 ;
			// !!!
			rectItem.top += (lpPDAListCtrl->iLineHeight - (lpPDAListCtrl->iTextHeight + 2)) /2;
			rectItem.bottom = rectItem.top +  (lpPDAListCtrl->iTextHeight + 2) ;
			rectItem.right = rectItem.left + strlen(lpMainItem->lpText) * lpPDAListCtrl->iTextWidth ;
			// !!! modified End
			
			GetClientRect(hWnd,&rectListView); // 得到窗口的客户区域
			if (rectItem.right >=rectListView.right)
				rectItem.right = rectListView.right -1;

			// 创建编辑控件
			CreateLabelEdit(hWnd,iItem,lpMainItem->lpText,&rectItem);
		}
		return TRUE;
}

/**************************************************
声明：static LRESULT DoDeleteAllItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：成功返回TRUE，否则返回FALSE
功能描述：删除所有的条目，处理LVM_DELETEALLITEMS消息。
引用: 
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

			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
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
			SendNormalNotify(hWnd,LVN_DELETEALLITEMS,-1,0,0,0,pt); // 通知父窗口

			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 删除所有的子条目
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
			// 删除主条目数据
			for (i=0;i<lpPDAListCtrl->iItemNum;i++)
			{ // 
					lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i);
					FreeItem(lpMainItem,bText);
			}
			PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[0].plSubItemContent);
			lpPDAListCtrl->iItemNum=0;
			lpPDAListCtrl->iStartItem=0;
			lpPDAListCtrl->iFocusItem=-1;

			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE); // 设置滚动条的位置
			SetVScrollBar(hWnd); // 设置滚动条
/*			hdc=GetDC(hWnd);
			DrawListCtrl(hWnd,hdc);
			ReleaseDC(hWnd,hdc);
*/
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
			return TRUE;
}
/**************************************************
声明：static LRESULT DoDeleteColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定的列
	IN lParam -- 保留
返回值：成功返回TRUE，否则返回FALSE
功能描述：删除一列，处理LVM_DELETECOLUMN消息。
引用: 
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
		
	KillEditControl(hWnd); // 杀死编辑控件
	lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
	if (lpPDAListCtrl==NULL)
		return FALSE;
	iCol=(int)wParam;
	if (iCol==0)
		return FALSE; // can't delete the main item
	if (iCol>=lpPDAListCtrl->iSubItemNum)
		return FALSE;
	if(!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_FILE))
	{// 如果是文件风格，没有子条目数据，否则删除指定的列
		if(!(lpPDAListCtrl->lpSubItem[iCol].ContentFmt&LVCFMT_IMAGE))
			PtrListFreeAll(&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent);
		else
			PtrListRemoveAll(&lpPDAListCtrl->lpSubItem[iCol].plSubItemContent);
	}
	// 移动后面的列的数据
	memmove( &lpPDAListCtrl->lpSubItem[iCol], &lpPDAListCtrl->lpSubItem[iCol+1], (lpPDAListCtrl->iSubItemNum-iCol)*sizeof(SUBITEM) );
	lpPDAListCtrl->iSubItemNum--;	 // 减少一列
	// 重新计算控件的宽度
	Width=0;
	for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
	{
		Width+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
	}
	lpPDAListCtrl->iWidth=Width;
	SetHScrollBar(hWnd);	// 设置水平滚动条

	return TRUE;
}
/**************************************************
声明：static LRESULT DoSetColumnWidth(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定列的索引
	IN lParam -- LOWORD 列的宽度
返回值：无
功能描述：设置指定列的宽度，处理LVM_SETCOLUMNWIDTH消息。
引用: 
************************************************/
static LRESULT DoSetColumnWidth(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
{
	int iCol,cx,iSubItem;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;
			
			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iCol=(int)wParam;
			cx=LOWORD(lParam);
			if (iCol>=lpPDAListCtrl->iSubItemNum||iCol<0) return 0;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // 得到指定列的索引
			if (cx==LVSCW_AUTOSIZE)
			{ // 设置为自动宽度
				cx=DEFAULTWIDTH;
			}
			if (cx==LVSCW_AUTOSIZE_USEHEADER)
			{ // 设置为与头相适应的宽度
				HDC hdc;
					hdc=GetDC(hWnd);
					cx=4;
					if (lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_TEXT)
						cx+=GetTextExtent(hdc,lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption, strlen(lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption));
					if (lpPDAListCtrl->lpSubItem[iSubItem].CaptionFmt&LVCF_IMAGE)
						cx+=16;
					ReleaseDC(hWnd,hdc);
			}
			lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth=cx; // 设置为指定宽度
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
声明：static LRESULT DoSetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目索引
	IN lParam -- 保留
返回值：成功返回原来的焦点条目索引，否则返回-1
功能描述：设置焦点索引，处理LVM_SETHOTITEM消息。
引用: 
************************************************/
static LRESULT DoSetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPPDALISTCTRL lpPDAListCtrl;
	 int iIndex,iOldFocus;

			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return -1;
			iIndex=(int)wParam; // 得到索引
			if (iIndex>lpPDAListCtrl->iItemNum)
				return -1;
			iOldFocus=lpPDAListCtrl->iFocusItem;
			lpPDAListCtrl->iFocusItem=iIndex; // 设置指定索引为焦点
			return iOldFocus;
}
/**************************************************
声明：static LRESULT DoSetIconSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(CX,CY),要设置的水平间距和垂直间距。
返回值：返回原来的水平间距和垂直间距。
功能描述：设置图标之间的间距，处理LVM_SETICONSPACING消息。
引用: 
************************************************/
static LRESULT DoSetIconSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPPDALISTCTRL lpPDAListCtrl;
	 int cx,cy,iOldcx,iOldcy;
	
			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);  // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;

			cx=LOWORD(lParam);
			cy=HIWORD(lParam);

			iOldcx=lpPDAListCtrl->iHorzSpacing;
			iOldcy=lpPDAListCtrl->iVertSpacing;

			// 设置水平间隔
			if (cx==-1)
				lpPDAListCtrl->iHorzSpacing=DEFAULTHORZSPACING;
			else
				lpPDAListCtrl->iHorzSpacing=cx;
			// 设置垂直间隔
			if (cy==-1)
				lpPDAListCtrl->iVertSpacing=DEFAULTVERTSPACING;
			else
				lpPDAListCtrl->iVertSpacing=cy;
			return MAKELRESULT(iOldcx,iOldcy); // 返回旧的间隔
}
/**************************************************
声明：static LRESULT DoEnsureVisible(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定的条目索引
	IN lParam -- BOOL 部分可见标志
返回值：无
功能描述：保证指定条目可见，处理LVM_ENSUREVISIBLE消息。
引用: 
************************************************/
static LRESULT DoEnsureVisible(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iItem;
	BOOL bPartialOK;
	LPPDALISTCTRL lpPDAListCtrl;
	int iTop,iBottom,iLineHeight,iItemNumInLine,iTopCoordinate;
	RECT rect;
	int iScrollLine;
	
			
		KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0);  // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam;
			bPartialOK=(BOOL)lParam; // 部分可见OK
			GetClientRect(hWnd,&rect);
			

			if (ExistCaption(hWnd)==TRUE)
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT;
			else
				iTopCoordinate=0;

			iLineHeight=GetLineHeight(hWnd); // 得到条目高度
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行的条目数
			iTop=iTopCoordinate+((iItem-lpPDAListCtrl->iStartItem)/iItemNumInLine)*iLineHeight;
			iBottom=iTop+iLineHeight;
			iScrollLine=0;
			if (iItem<lpPDAListCtrl->iStartItem)
			{// 得到需要滚动的行数
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
				VScrollWindow(hWnd,iScrollLine); // 滚动窗口
			return 0;
}
/**************************************************
声明：static LRESULT DoFindItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 开始条目索引
	IN lParam -- 指向FINDINFO的指针
返回值：找到条目，返回条目的索引，否则返回-1
功能描述：查找指定的条目，处理LVM_FINDITEM消息。
引用: 
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

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return -1;
			iStart=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lplvFindInfo=(LPFINDINFO)lParam; // 得到查找结构

			if (iStart<0)
				iIndex=0;
			else
			{
				iIndex=iStart+1;  //The specified item is itself excluded from the search. 
				iNeedStart=TRUE;
			}
			// 查找指定的条目
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
								continue; // 不满足条件
						}
						if (lplvFindInfo->flags&LVFI_STRING)
						{
							for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
							{
								if (i>0&&lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_IMAGE)
										continue; // 不满足条件
								GetItemString(hWnd,lpPDAListCtrl,iIndex,i,lpFileFeild);
	//							GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,iCurSubItem,lpFileFeild);
								lpListString=lpFileFeild;
								if (lplvFindInfo->flags&LVFI_PARTIAL)
								{
									if (strstr(lpListString,lplvFindInfo->psz)==NULL)
										continue; // 不满足条件
								}
								else
								{
									if (strcmp(lpListString,lplvFindInfo->psz)!=0)
										continue; // 不满足条件

								}
							}
						}
						return iIndex; // 满足条件
//						if (lplvFindInfo->flags&LVFI_NEARESTXY)
				}
				if (iNeedStart==FALSE)
					break;
				if (lplvFindInfo->flags&LVFI_WRAP)
					iIndex=0;
				iNeedStart=FALSE;
			}
			return -1; // 没有找到
}
/**************************************************
声明：static LRESULT DoGetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定列的索引
	OUT lParam -- 指向LVCOLUMN的指针，存放指定列的数据
返回值：成功返回TRUE，否则返回FALSE。
功能描述：得到列的信息，处理LVM_GETCOLUMN消息。
引用: 
************************************************/
static LRESULT DoGetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 int iCol,iSubItem;
	 LPLVCOLUMN lpCol;
	 LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iCol=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			
			lpCol=(LPLVCOLUMN)lParam; // 得到存放列数据的数据结构
			if (iCol>=lpPDAListCtrl->iSubItemNum)
				return FALSE;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // 得到子条目的索引
			if (lpCol->mask&LVCF_TEXT)
			{ // 复制文本
				strncpy( lpCol->pszText,lpPDAListCtrl->lpSubItem[iSubItem].lpSubItemCaption,(lpCol->cchTextMax-1));
				lpCol->pszText[lpCol->cchTextMax-1]=0;
			}
			if (lpCol->mask&LVCF_WIDTH)
			{ // 得到宽度
				lpCol->cx=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			}
			if (lpCol->mask&LVCF_IMAGE)
			{ // 得到图象索引
					lpCol->iImage=lpPDAListCtrl->lpSubItem[iSubItem].iImage;
			}
			if (lpCol->mask&LVCF_FMT)
			{ // 得到风格
				lpCol->fmt=lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt;
			}
			return TRUE;
}
/**************************************************
声明：static LRESULT DoGetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 数组的大小
	IN lParam -- 指向INT的指针数组，存放列的次序
返回值：成功返回TRUE，否则返回FALSE
功能描述：得到列的显示次序，处理LVM_GETCOLUMNORDERARRAY消息。
引用: 
************************************************/
static LRESULT DoGetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iCount, *lpiArray;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;

			iCount=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
			
			lpiArray=(int *)lParam; // 得到存放次序的表格
			
			if (iCount>lpPDAListCtrl->iSubItemNum)
				iCount=lpPDAListCtrl->iSubItemNum;

			// 复制当前的次序
			for (i=0;i<iCount;i++)
				lpiArray[i]=lpPDAListCtrl->lpColumnOrderArray[i];
			return TRUE;
}
/**************************************************
声明：static LRESULT DoGetColumnWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定列的索引
	IN lParam -- 保留
返回值：返回指定列的宽度
功能描述：得到指定列的宽度，处理LVM_GETCOLUMNWIDTH消息。
引用: 
************************************************/
static LRESULT DoGetColumnWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 int iCol,iSubItem;
	 LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iCol=(int)wParam;
			if (iCol>=lpPDAListCtrl->iSubItemNum)
				return 0;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // 得到子条目的索引
			return lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth; // 得到该条目的宽度
}
/**************************************************
声明：static LRESULT DoGetCountPerPage(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回每一页的条目个数
功能描述：得到每一页的条目个数，处理LVM_GETCOUNTPERPAGE消息。
引用: 
************************************************/
static LRESULT DoGetCountPerPage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		return GetPageItemNum(hWnd); // 得到一页的条目个数
}

/**************************************************
声明：static LRESULT DoGetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回焦点条目的索引
功能描述：得到焦点条目的索引，处理LVM_GETHOTITEM消息。
引用: 
************************************************/
static LRESULT DoGetHotItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return -1;
			return lpPDAListCtrl->iFocusItem; // 返回焦点的条目索引
}
/**************************************************
声明：static LRESULT DoGetImageList(HWND hWnd , WPARAM wParam , LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 图形列表的类型
	IN lParam -- 保留
返回值：返回指定类型的图形列表句柄
功能描述：得到指定类型的图形列表句柄，处理LVM_GETIMAGELIST消息。
引用: 
************************************************/
static LRESULT DoGetImageList(HWND hWnd , WPARAM wParam , LPARAM lParam)
{
	int iImageList;
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return (LRESULT)NULL;
			iImageList=(int)wParam;
			if (iImageList==LVSIL_NORMAL)
				return (LRESULT)lpPDAListCtrl->lpImageList; // 返回通常图象列表

			if (iImageList==LVSIL_SMALL)
				return (LRESULT)lpPDAListCtrl->lpImageListSmall; // 返回小图象列表

			return (LRESULT)NULL;
}
/**************************************************
声明：static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 指向LVITEM的的指针
返回值：成功返回TRUE，否则返回FALSE
功能描述：得到指定条目数据，处理LVM_GETITEM消息。
引用: 
************************************************/
static LRESULT DoGetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPLVITEM pItem;
	LPMAINITEM lpMainItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			
			pItem=(LPLVITEM)lParam; // 得到条目结构
			if (pItem->iSubItem==0)
			{ // 得到主条目的数据
						lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[pItem->iSubItem].plSubItemContent,pItem->iItem); // 得到主条目结构
						if (lpMainItem==NULL)
							return FALSE;
						if (pItem->mask&LVIF_TEXT)
						{ // 复制文本
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
						{ // 得到图象索引
							pItem->iImage=lpMainItem->iImage;
						}
						if (pItem->mask&LVIF_PARAM)
						{ // 得到条目参数
							pItem->lParam=lpMainItem->lParam;
						}
						if (pItem->mask&LVIF_STATE)
						{ // 得到条目状态
							pItem->state=lpMainItem->State&pItem->stateMask;
						}
			}
			else
			{ // 得到子条目数据
					pItem->state=0;
					pItem->lParam=0;
					if ((pItem->mask&LVIF_IMAGE)&&(lpPDAListCtrl->lpSubItem[pItem->iSubItem].ContentFmt&LVCFMT_IMAGE))
					{// 得到图象索引
							pItem->pszText[0]=0;
							pItem->iImage=(int)PtrListAt(&lpPDAListCtrl->lpSubItem[pItem->iSubItem].plSubItemContent,pItem->iItem);
					}
					else
					{ // 得到文本
						if (pItem->mask&LVIF_TEXT)
						{ // 条目有文本
// !!! Add By Jami Chen 2002.04.01
							if (lpPDAListCtrl->lpSubItem[pItem->iSubItem].ContentFmt&LVCFMT_FILE)
							{
								// the LVCFMT_FILE ,sub item is not any  message ,it's empty
//								pItem->pszText=lpMainItem->lpText;
//								pItem->pszText=0;
							}
							else
// !!! Add End By Jami chen 2002.04.01
							{ // 复制文本
								LPTSTR lpString;
								lpString=(LPTSTR)PtrListAt(&lpPDAListCtrl->lpSubItem[pItem->iSubItem].plSubItemContent,pItem->iItem);
								if (lpString == NULL)
								{ // 空数据
									pItem->pszText[0]=0;
								}
								else
								{ // 复制当前数据
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
声明：static LRESULT DoGetItemCount(HWND hWnd,WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回当前控件的条目总数
功能描述：得到当前控件的条目总数，处理LVM_GETITEMCOUNT消息。
引用: 
************************************************/
static LRESULT DoGetItemCount(HWND hWnd,WPARAM wParam ,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			return 	lpPDAListCtrl->iItemNum; // 返回条目个数
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
声明：static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目的索引
	IN lParam -- LPRECT 存放条目的矩形大小
返回值：成功返回TRUE，否则返回FALSE
功能描述：得到指定条目的矩形大小，处理LVM_GETITEMRECT消息。
引用: 
************************************************/
static LRESULT DoGetItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPRECT lprc;
	int iItem;
	int iLineHeight,iTopCoordinate,iItemNumInLine,iDrawLine,iWindowWidth;
	DWORD dwStyle;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItem=(int)wParam; // 得到条目索引

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lprc=(LPRECT)lParam;
			if(iItem>=lpPDAListCtrl->iItemNum)
				return FALSE;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if (ExistCaption(hWnd)==TRUE) // 存在CAPTION
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
			else
				iTopCoordinate=0;

			iLineHeight=GetLineHeight(hWnd); // 得到条目高度
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到没行的条目个数
			iItem-=lpPDAListCtrl->iStartItem;
			iWindowWidth=GetWindowWidth(hWnd); // 得到窗口宽度
			iDrawLine=iItem/iItemNumInLine;
			// 得到条目的矩形
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
声明：static LRESULT DoGetItemSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- BOOL 是否是小图标的标志
	IN lParam -- 保留
返回值：LOWORD返回水平间距，HIWORD返回垂直间距
功能描述：得到条目间的间距，处理LVM_GETITEMSPACING消息。
引用: 
************************************************/
static LRESULT DoGetItemSpacing(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	BOOL fSmall;
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			fSmall=(BOOL)wParam;
			if (fSmall)
				return 0;
			return MAKELRESULT(lpPDAListCtrl->iHorzSpacing,lpPDAListCtrl->iVertSpacing); // 返回水平和垂直间隔
}
/**************************************************
声明：static LRESULT DoGetItemState(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目的索引
	IN lParam -- UINT 要得到条目的状态的屏蔽码
返回值：返回指定条目的状态
功能描述：得到指定条目的状态，处理LVM_GETITEMSTATE消息。
引用: 
************************************************/
static LRESULT DoGetItemState(HWND hWnd ,WPARAM wParam ,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem;
		UINT mask;
		LPMAINITEM lpMainItem;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam; // 得到条目索引
			mask=(UINT)lParam;
			if (iItem>=lpPDAListCtrl->iItemNum)
				return 0;
			lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构数据
			if (lpMainItem==NULL)
				return 0;
			return (mask&lpMainItem->State); // 返回当前条目的指定状态
}
/**************************************************
声明：static LRESULT DoGetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- LPLVITEM 志向LVITEM的指针
返回值：无
功能描述：得到指定条目的文本，处理LVM_GETITEMTEXT消息。
引用: 
************************************************/
static LRESULT DoGetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem;
		LPMAINITEM lpMainItem;
    LPLVITEM pitem; 
		LPTSTR lpSubItemString;
			
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iItem=(int)wParam; // 得到条目索引

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			
			pitem=(LPLVITEM)lParam; // 得到条目指针

			if (iItem < 0)
				return 0;
			if (iItem>=lpPDAListCtrl->iItemNum)
				return 0;
			if (pitem->iSubItem>=lpPDAListCtrl->iSubItemNum)
				return 0;
			lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构
			if (lpMainItem==NULL)
				return 0;
			if (pitem->iSubItem==0)
			{ // 要得到主条目数据
				if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
				{ // 得到FILE NO。
					pitem->pszText=lpMainItem->lpText;
				}
				else
				{ // 得到条目文本
					strncpy( pitem->pszText,lpMainItem->lpText,(pitem->cchTextMax-1));
					pitem->pszText[pitem->cchTextMax-1]=0;
				}
			}
			else
			{ // 得到子条目数据
					if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_FILE)
					{ // 得到FILE NO。
						pitem->pszText=lpMainItem->lpText;
					}
					else
					{ // 得到指定的文本数据
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
声明：static LRESULT DoGetNextItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 开始查找的条目索引
	IN lParam -- LOWORD 查找条目的条件
返回值：成功返回找到条目的索引，否则返回 -1
功能描述：查找下一个条目。处理LVM_GETNEXTITEM消息。
引用: 
************************************************/
static LRESULT DoGetNextItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;
		int iStart;
		int iItem;
		UINT flags;
		int iItemNumInLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return -1;
			iStart=(int)wParam;
			flags=LOWORD(lParam);
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目数
			if (iStart==-1)
				iItem=0;
			else
				iItem=iStart;

			if (iItem>=lpPDAListCtrl->iItemNum||iItem<0)
				return -1;

			while(1)
			{
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到指定条目的数据
				if (lpMainItem==NULL)
					return -1;
				if (flags&LVNI_SELECTED)
				{ // 要查找选择条目
					if ((lpMainItem->State&LVIS_SELECTED)==0)
						goto _NEXTITEM;// 没有找到
				}
				if (flags&LVNI_FOCUSED)
				{ // 焦点条目
					if ((lpMainItem->State&LVIS_FOCUSED)==0)
						goto _NEXTITEM; // 没有找到
				}
				break;

_NEXTITEM:  // 查找下一个条目
/*				if (iStart==-1)
				{
					iItem++;
					if (iItem>=lpPDAListCtrl->iItemNum)
						return -1;
				}
				else	*/
				if (flags&LVNI_ABOVE)
				{ // 往上查找
					iItem-=iItemNumInLine;
					if (iItem<0)
						return -1;
				}
				else if (flags&LVNI_BELOW)
				{ // 往下查找
					iItem+=iItemNumInLine;
					if (iItem>=lpPDAListCtrl->iItemNum)
						return -1;
				}
				else if (flags&LVNI_TOLEFT)
				{ // 往前查找
					if ((iItem%iItemNumInLine)==0)
						return -1;
					iItem--;
				}
				else if (flags&LVNI_TORIGHT)
				{ // 往后查找
					if ((iItem%iItemNumInLine)==(iItemNumInLine-1))
						return -1;
					iItem++;
				}
				else
				{ // 到下一个条目
					iItem++;
					if (iItem>=lpPDAListCtrl->iItemNum)
						return -1;
				}
			}
			return iItem;
}
/**************************************************
声明：static LRESULT DoGetOrigin(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPPOINT 存放原点坐标的指针
返回值：成功返回TRUE， 否则返回FALSE。
功能描述：得到控件原点的坐标，处理LVM_GETORIGIN消息。
引用: 
************************************************/
static LRESULT DoGetOrigin(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPPOINT lpptOrg;
	int iLineHeight,iItemNumInLine;
	DWORD dwStyle,dwTypeMask;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
			
			lpptOrg=(LPPOINT)lParam;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			dwTypeMask=dwStyle&LVS_TYPEMASK; // 得到窗口类型

			iLineHeight=GetLineHeight(hWnd); // 得到行高度
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到一行的条目数
			// 得到原始点的坐标
			lpptOrg->x=lpPDAListCtrl->cx;
			lpptOrg->y=((0-lpPDAListCtrl->iStartItem)/iItemNumInLine)*iLineHeight;

			if (dwTypeMask==LVS_REPORT||dwTypeMask==LVS_LIST)
				return FALSE;
			return TRUE;
}
/**************************************************
声明：static LRESULT DoGetSelectedCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回选择条目的数目
功能描述：得到当前已经选择的条目的个数，处理LVM_GETSELECTEDCOUNT消息。
引用: 
************************************************/
static LRESULT DoGetSelectedCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int i,iSelectedCount=0;
	LPMAINITEM lpMainItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			for (i=0;i<lpPDAListCtrl->iItemNum;i++)
			{  // 得到选择条目的个数
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i); // 得到当前条目的数据
				if (lpMainItem)
				{
					if (lpMainItem->State&LVIS_SELECTED) // 该条目是选择条目
						iSelectedCount++;
				}
			}
			return iSelectedCount;
}
/**************************************************
声明：static LRESULT DoGetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回选择条目的索引
功能描述：得到选择条目的索引，处理LVM_GETSELECTIONMARK消息。
引用: 
************************************************/
static LRESULT DoGetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	return SendMessage(hWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED); // 得到下一个选择条目
}

/**************************************************
声明：static LRESULT DoSetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定要设置为选择的条目索引
	IN lParam -- 保留
返回值：返回以前的选择条目的索引
功能描述：设置指定条目为选择条目，处理LVM_SETSELECTIONMARK消息。
引用: 
************************************************/
static LRESULT DoSetSelectionMark(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iIndex ,iOldIndex;
    LVITEM item; 

		iIndex = (int)lParam;
		iOldIndex = SendMessage(hWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED); // 得到下一个选择条目
		item.mask = LVIF_STATE;
		item.iItem = iIndex;
		item.iSubItem = 0;
		item.state = LVIS_SELECTED ;
		item.stateMask = LVIS_SELECTED ;
//		SendMessage(hWnd,LVM_GETNEXTITEM,-1,LVNI_SELECTED);
//		ClearFocus(hWnd);
		ClearAllInvert(hWnd); // 清除所有的选择条目
		SendMessage(hWnd,LVM_SETITEMSTATE,iIndex,(LPARAM)&item); // 设置条目状态
		return iOldIndex;
}

/**************************************************
声明：static LRESULT DoGetStringWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTSTR 指定的字符串
返回值：返回指定字符串的宽度
功能描述：得到指定字符串的宽度，处理LVM_GETSTRINGWIDTH消息。
引用: 
************************************************/
static LRESULT DoGetStringWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTSTR psz;
	HDC hdc;
	int iStringWidth;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

		psz=(LPCTSTR)lParam; // 得到字串指针
		hdc=GetDC(hWnd);
		iStringWidth=GetTextExtent(hdc,psz,strlen(psz)); // 得到指定字符串的宽度
		ReleaseDC(hWnd,hdc);
		return iStringWidth;
}
/**************************************************
声明：static LRESULT DoGetSubItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 指定条目
	IN/OUT lParam -- LPRECT，指定条目的矩形大小，
					 其中top是要得到位置的字条目索引，以1开始。left 是查看属性
返回值：无
功能描述：得到指定字条目的矩形大小，处理LVM_GETSUBITEMRECT消息。
引用: 
************************************************/
static LRESULT DoGetSubItemRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iItem,iSubItem,iCurSubItem;
	UINT flags;
	LPRECT lpRect;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;
 
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;

			iItem=(int)wParam; // 得到条目索引
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
			SendMessage(hWnd,LVM_GETITEMRECT,(WPARAM)iItem,(LPARAM)lpRect); // 得到条目的矩形

			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 查找指定的子条目
				iCurSubItem=lpPDAListCtrl->lpColumnOrderArray[i]; // 得到子条目的索引
				if (iCurSubItem==iSubItem)
				{ // 是当前要的子条目
					if (flags==LVIR_ICON)
					{  // ICON 模式
						if (lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth>lpPDAListCtrl->sizeSmallIcon.cx)
						{ // 条目大小为ICON的大小
							lpRect->right=lpRect->left+lpPDAListCtrl->sizeSmallIcon.cx;
							return TRUE;
						}
					}
					// 大小为条目宽度
					lpRect->right=lpRect->left+lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
					return TRUE;
				}
				// 得到下一个条目的开始位置
				lpRect->left+=lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
			}
			return 0;
}
/**************************************************
声明：static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回第一个可见条目的索引
功能描述：得到第一个可见条目的索引，处理LVM_GETTOPINDEX消息。
引用: 
************************************************/
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			return lpPDAListCtrl->iStartItem; // 得到当前页的开始条目
}
/**************************************************
声明：static LRESULT DoGetViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPRECT 存放视图矩形的结构
返回值：成功返回TRUE，否则返回FALSE
功能描述：得到控件的视图大小，处理LVM_GETVIEWRECT消息。
引用: 
************************************************/
static LRESULT DoGetViewRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPRECT prc;
		int iTotalLine,iLineHeight;
		LPPDALISTCTRL lpPDAListCtrl;
		DWORD dwStyle;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			
#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			prc=(LPRECT)lParam;
			prc->left=lpPDAListCtrl->cx;
			iLineHeight=GetLineHeight(hWnd); // 得到条目高度
			iTotalLine=GetTotalLine(hWnd); // 得到总行数
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			// 得到控件宽度
			if ((dwStyle&LVS_TYPEMASK)==LVS_REPORT)
				prc->right=prc->left+lpPDAListCtrl->iWidth;
			else
				prc->right=prc->left+GetWindowWidth(hWnd);;

			// 得到控件高度
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
声明：static LRESULT DoRedrawItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 要重绘的条目开始索引
	IN lParam -- INT 要重绘的条目的结束索引
返回值：无
功能描述：重绘指定的条目，处理LVM_REDRAWITEMS消息。
引用: 
************************************************/
static LRESULT DoRedrawItems(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iFirst,iLast,i;

		KillEditControl(hWnd); // 杀死编辑控件
		iFirst=(int)wParam;
		iLast=(int)lParam;
// !!! modified By Jami chen in 2002.04.25
//			for (i=iFirst;i<iLast;i++)
			for (i=iFirst;i<=iLast;i++)
// !!! modified End By Jami chen in 2002.04.25
			{
				DrawItem(hWnd,i); // 绘制指定的条目
			}
			return TRUE;
}


/**************************************************
声明：static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 水平滚动的大小
	IN lParam -- INT 垂直滚动的大小
返回值：无
功能描述：滚动窗口，处理LVM_SCROLL消息。
引用: 
************************************************/
static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   int dx,dy;
	 int iVScrollLine,iHScrollLine;

			KillEditControl(hWnd); // 杀死编辑控件
			dx=(int)wParam;
			dy=(int)lParam;

			iVScrollLine=dy/GetLineHeight(hWnd);
			iHScrollLine=dx/WIDTHSTEP;

			HScrollWindow(hWnd,iHScrollLine); // 水平滚动窗口
			VScrollWindow(hWnd,iVScrollLine); // 垂直滚动窗口
			return TRUE;
}
/**************************************************
声明：static LRESULT DoSetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 要设置的列的索引
	IN lParam -- LPLVCOLUMN 要设置的列的数据
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置指定的列，处理LVM_SETCOLUMN消息。
引用: 
************************************************/
static LRESULT DoSetColumn(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 int iCol,iSubItem;
	 LPLVCOLUMN lpCol;
	 LPPDALISTCTRL lpPDAListCtrl;

			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iCol=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
			lpCol=(LPLVCOLUMN)lParam; // 得到列数据
			if (iCol>=lpPDAListCtrl->iSubItemNum)
				return FALSE;
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[iCol]; // 得到指定列的子条目索引
			if (lpCol->mask&LVCF_TEXT)
			{ // 文本有效
				LPTSTR lpTemp;
				int  cbCaptionLen;
					// 设置CAPTION文本
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
			{ // 宽度有效，重设列的宽度
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
			{ // 图象有效
					lpPDAListCtrl->lpSubItem[iSubItem].iImage=lpCol->iImage;
			}
			if (lpCol->mask&LVCF_FMT)
			{ // 风格有效
				lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt=lpCol->fmt;
			}
			return TRUE;
}
/****************************************************************************************************/
/**************************************************
声明：static LRESULT DoSetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 要排列的列的个数
	IN lParam -- LPINT 排列的次序
返回值：成功返回TRUE， 否则返回FALSE
功能描述：设置列的显示次序，处理LVM_SETCOLUMNORDERARRAY消息。
引用: 
************************************************/
static LRESULT DoSetColumnOrderArray(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int iCount, *lpiArray;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;

			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;

			iCount=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			lpiArray=(int *)lParam;
			
			if (iCount>lpPDAListCtrl->iSubItemNum)
				iCount=lpPDAListCtrl->iSubItemNum;
			// 设置列显示次序
			for (i=0;i<iCount;i++)
				lpPDAListCtrl->lpColumnOrderArray[i]=lpiArray[i];
			return TRUE;
}
/**************************************************
声明：static LRESULT DoSetItemCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 要设置的条目总数
	IN lParam -- 保留
返回值：成功返回TRUE， 否则返回FALSE
功能描述：设置控件的条目总数，处理LVM_SETITEMCOUNT消息。
引用: 
************************************************/
static LRESULT DoSetItemCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItems,iSubItem;
			
			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iItems=(int)wParam; // 得到条目个数
			if (iItems<lpPDAListCtrl->iMaxItemNum)
				return 0;
			for (iSubItem=0;iSubItem<lpPDAListCtrl->iSubItemNum;iSubItem++)
			{ // 设置条目个数
				PtrListSetLimit(&lpPDAListCtrl->lpSubItem[iSubItem].plSubItemContent,iItems);
			}
			return 1;
}
/**************************************************
声明：static LRESULT DoSetItemState(HWND hWnd ,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目索引
	IN lParam -- LPLVITEM 指定条目的数据，state 和 stateMask 有效
返回值：成功返回TRUE， 否则返回FALSE
功能描述：设置条目状态，处理LVM_SETITEMSTATE消息。
引用: 
************************************************/
static LRESULT DoSetItemState(HWND hWnd ,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItems,iCurItem;
		LPLVITEM pItem;
		LPMAINITEM lpMainItem;
			
			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItems=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			pItem=(LPLVITEM)lParam; // 得到条目结构
			
			if (iItems>=lpPDAListCtrl->iItemNum&&iItems<-1)
				return FALSE;
			if (pItem==NULL)
				return FALSE;

			if (iItems==-1)
			{ // 设置所有的条目
				for (iCurItem=0;iCurItem<lpPDAListCtrl->iItemNum;iCurItem++) 
				{ // 循环设置所有条目
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem);
					if (lpMainItem)
					{
//						lpMainItem->State=pItem->state&pItem->stateMask;
						lpMainItem->State &= ~pItem->stateMask; // clear Mask Bit
						lpMainItem->State |=pItem->state&pItem->stateMask;  // Set Maxk Bit
						DrawItem(hWnd,iCurItem); // 绘制该条目
					}
				}
			}
			else
			{ // 设置指定的条目
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItems); // 得到条目数据
					if (lpMainItem)
					{ // 设置条目的状态
						lpMainItem->State &= ~pItem->stateMask; // clear Mask Bit
						lpMainItem->State |=pItem->state&pItem->stateMask;  // Set Maxk Bit
						if (lpPDAListCtrl->iFocusItem != -1)
						{
							ClearFocus(hWnd); // 清楚焦点
							lpPDAListCtrl->iActiveItem=lpPDAListCtrl->iFocusItem=iItems; // 重新设置焦点
						}
						DrawItem(hWnd,iItems); // 绘制条目
					}
			}
			return TRUE;
}
/**************************************************
声明：static LRESULT DoSetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目索引
	IN lParam -- LPLVITEM 指定条目的数据
返回值：成功返回TRUE， 否则返回FALSE
功能描述：设置条目文本，处理LVM_SETITEMTEXT消息。
引用: 
************************************************/
static LRESULT DoSetItemText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItem;
		LPMAINITEM lpMainItem;
    LPLVITEM pitem; 
		LPVOID lpReallocPtr;
		int cbStringLen;
			
			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			iItem=(int)wParam;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

			pitem=(LPLVITEM)lParam; // 得到条目结构

			if (pitem==NULL)
				return FALSE;

			if (iItem>=lpPDAListCtrl->iItemNum)
				return FALSE;
			if (pitem->iSubItem>=lpPDAListCtrl->iSubItemNum)
				return FALSE;
			if (pitem->iSubItem==0)
			{ // 当前要设置的是主条目
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目数据
				if (lpMainItem==NULL)
					return FALSE;
				if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE)
				{ // 是文件风格
					lpMainItem->lpText=pitem->pszText; // 设置FILE NO
				}
				else
				{ // 是一般文本风格， 设置文本
					cbStringLen=strlen(pitem->pszText);
					lpReallocPtr=realloc(lpMainItem->lpText,cbStringLen+1);
					if (lpReallocPtr==NULL)
						return FALSE;
					lpMainItem->lpText=lpReallocPtr;
					strcpy(lpMainItem->lpText,pitem->pszText);
				}
			}
			else
			{ // 是子条目
					if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_FILE)
					{ // 文件风格没有子条目数据
						return FALSE;
					}
					else
					{ // 设置子条目数据
						if (lpPDAListCtrl->lpSubItem[pitem->iSubItem].ContentFmt&LVCFMT_IMAGE)
						{ // 设置图象
							PtrListAtPut(&lpPDAListCtrl->lpSubItem[pitem->iSubItem].plSubItemContent,iItem,pitem->pszText);
						}
						else
						{ // 设置文本
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
声明：static LRESULT DoHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN/OUT lParam -- LPLVHITTESTINFO 测试结构信息
返回值：返回点所对应的条目索引
功能描述：测试点的位置，处理LVM_HITTEST消息。
引用: 
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

		pInfo=(LPLVHITTESTINFO)lParam; // 得到点测试结构
		SendMessage(hWnd,LVM_GETVIEWRECT,(WPARAM)0,(LPARAM)&rect); // 得到控件矩形

		pInfo->iItem=-1;
		pInfo->iSubItem=-1;
		if (PtInRect(&rect,pInfo->pt)==FALSE)
			return -1; // 指定点不在控件中
//		iLineHeight=GetLineHeight(hWnd);
		pts.x=(int)pInfo->pt.x;
		pts.y=(int)pInfo->pt.y;
		GetPointPosition(hWnd,pts,&iRow,&iColumn); // 得到点所在的行和列
		iItem=iRow*GetItemNumInLine(hWnd)+iColumn; // 得到条目索引
		iCount=(int)SendMessage(hWnd,LVM_GETITEMCOUNT,0,0); // 得到条目个数
		pInfo->flags=0;
		if (iItem>iCount)
			pInfo->flags|=LVHT_NOWHERE; // 该条目不存在
		else
			pInfo->iItem=iItem; // 条目存在
		if (ExistCaption(hWnd))
			iTopCoordinate=GetCaptionHeight(hWnd,NULL);//CAPTIONHEIGHT;
		else
			iTopCoordinate=0;
		if (pInfo->pt.y<iTopCoordinate)
			pInfo->flags|=LVHT_ABOVE; // 在上面
		if (pInfo->pt.y>GetWindowHeight(hWnd))
			pInfo->flags|=LVHT_BELOW; // 在下面
		if (pInfo->pt.x<0)
			pInfo->flags|=LVHT_TOLEFT; // 在左面
		if (pInfo->pt.x>GetWindowWidth(hWnd))
			pInfo->flags|=LVHT_TORIGHT; // 在右边
		return iItem;
}
// !!! Add End By Jami chen 2002.04.25
/**************************************************
声明：static LRESULT DoSubItemHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN/OUT lParam -- LPLVHITTESTINFO 测试结构信息
返回值：返回点所对应的条目索引
功能描述：测试点所在的子项的位置，处理LVM_SUBITEMHITTEST消息。
引用: 
************************************************/
static LRESULT DoSubItemHitTest(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rect;
	LPLVHITTESTINFO pInfo;
	int iItem,iRow,iColumn,iCount;
	int iTopCoordinate;
	POINTS pts;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif

		pInfo=(LPLVHITTESTINFO)lParam; // 得到测试点结构
		SendMessage(hWnd,LVM_GETVIEWRECT,(WPARAM)0,(LPARAM)&rect); // 得到控件的矩形

		pInfo->iItem=-1;
		pInfo->iSubItem=-1;
		if (PtInRect(&rect,pInfo->pt)==FALSE)
			return -1; // 不在控制内
//		iLineHeight=GetLineHeight(hWnd);
		pts.x=(int)pInfo->pt.x;
		pts.y=(int)pInfo->pt.y;
		GetPointPosition(hWnd,pts,&iRow,&iColumn); // 得到点的行和列
		iItem=iRow*GetItemNumInLine(hWnd)+iColumn; // 得到条目索引
		iCount=(int)SendMessage(hWnd,LVM_GETITEMCOUNT,0,0); // 得到条目个数
		pInfo->flags=0;
		if (iItem>iCount)
			pInfo->flags|=LVHT_NOWHERE;
		else
			pInfo->iItem=iItem;
// !!! Add By Jami chen 2002.04.25
		pInfo->iSubItem=GetSubItemPointPosition(hWnd,pts); // 得到子条目的索引
// !!! Add End By Jami chen 2002.04.25

		if (ExistCaption(hWnd)) // 是否存在CAPTION
			iTopCoordinate=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT;
		else
			iTopCoordinate=0;
		if (pInfo->pt.y<iTopCoordinate)
			pInfo->flags|=LVHT_ABOVE; // 在上面
		if (pInfo->pt.y>GetWindowHeight(hWnd))
			pInfo->flags|=LVHT_BELOW; // 在下面
		if (pInfo->pt.x<0)
			pInfo->flags|=LVHT_TOLEFT; // 在左面
		if (pInfo->pt.x>GetWindowWidth(hWnd))
			pInfo->flags|=LVHT_TORIGHT; // 在右面
		return iItem;
}
/**************************************************
声明：static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：成功返回TRUE， 否则返回FALSE
功能描述：清除控件内容，处理WM_CLEAR消息。
引用: 
************************************************/
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iCurItem;
		LPMAINITEM lpMainItem;
			
			KillEditControl(hWnd); // 杀死编辑控件
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
			// 删除所有的选择条目
			for (iCurItem=lpPDAListCtrl->iItemNum-1;iCurItem>=0;iCurItem--)
			{
				lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // 得到条目数据
				if (lpMainItem)
				{
						if (lpMainItem->State&LVIS_SELECTED)
						{ // 当前条目是选择条目
							SendMessage(hWnd,LVM_DELETEITEM,iCurItem,0); // 删除该条目
						}
				}
			}
			return TRUE;

}
/**************************************************
声明：static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPSTYLESTRUCT 指向STYLESTRUCT结构的指针
返回值：无
功能描述：处理WM_STYLECHANGED消息。
引用: 
************************************************/
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;
	LPSTYLESTRUCT lpStyleStruct = (LPSTYLESTRUCT)lParam;          // new styles

		if ((lpStyleStruct->styleOld & LVS_TYPEMASK) != (lpStyleStruct->styleNew & LVS_TYPEMASK))
		{ // 控件的类型发生改变
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return FALSE;
//			ClearAllInvert(hWnd);
			lpPDAListCtrl->iStartItem=0;
			lpPDAListCtrl->iLineHeight = 0; // add By Jami chen in 2004.05.21 , will ReCalc height
			lpPDAListCtrl->iLineHeight = GetLineHeight(hWnd); // Add By Jami chen in 2004.05.21
			LV_SetScrollPos(hWnd,SB_VERT,0,TRUE); // 设置滚动条的位置
			SetHScrollBar(hWnd); // 设置滚动条
			SetVScrollBar(hWnd);
			lpPDAListCtrl->iFocusItem=-1;
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		}
		return TRUE;
}

/**************************************************
声明：static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 虚键值
	IN lParam -- 保留
返回值：无
功能描述：处理WM_KEYDOWN消息。
引用: 
************************************************/
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
//	DWORD lKeyData ;
	int iPageLines,iItemNumInLine;

		nVirtKey = (int) wParam;    // virtual-key code
//		lKeyData = lParam;          // key data
 		iPageLines=GetPageLine(hWnd); // 得到每一页的行数
 		iItemNumInLine=GetItemNumInLine(hWnd); // 得到每一行的条目数
		KillEditControl(hWnd); // 杀死编辑控件
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
	LPPDALISTCTRL lpPDAListCtrl;
			

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return FALSE;
		
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 设置文本颜色
			 lpPDAListCtrl->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景色
				lpPDAListCtrl->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // 设置选择文本颜色
				lpPDAListCtrl->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // 设置选择文本背景色
				lpPDAListCtrl->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 设置标题文本颜色
			lpPDAListCtrl->cl_HeaderText = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // 设置标题文本背景色
			lpPDAListCtrl->cl_HeaderBkColor = lpCtlColor->cl_TitleBk;
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
	LPPDALISTCTRL lpPDAListCtrl;
			

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return FALSE;


		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // 得到文本颜色
			 lpCtlColor->cl_Text = lpPDAListCtrl->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 得到文本背景色
				lpCtlColor->cl_TextBk = lpPDAListCtrl->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // 得到选择文本颜色
				lpCtlColor->cl_Selection = lpPDAListCtrl->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // 得到选择文本背景色
				lpCtlColor->cl_SelectionBk = lpPDAListCtrl->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 得到标题文本颜色
			lpCtlColor->cl_Title = lpPDAListCtrl->cl_HeaderText ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // 得到标题文本背景色
			lpCtlColor->cl_TitleBk = lpPDAListCtrl->cl_HeaderBkColor ;
		 }
		 return TRUE;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/**************************************************
声明：static void DrawListCtrlCaption(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制控件标题
引用: 
************************************************/
static void DrawListCtrlCaption(HWND hWnd,HDC hdc)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int i;
			
			if (ExistCaption(hWnd)==FALSE) return;  // Is not exist caption , don't dwar caption		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 绘制CAPTION条目
 					DrawCaptionItem(hWnd,hdc,i,TRUE);
			}
 			DrawCaptionItem(hWnd,hdc,lpPDAListCtrl->iSubItemNum,TRUE); // 绘制最后一个空白标题
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
声明：static void DrawListCtrl(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制控件
引用: 
************************************************/
static void DrawListCtrl(HWND hWnd,HDC hdc)
{
	DWORD dwStyle;
	DWORD iDrawType;
		ClearClientArea(hWnd,hdc); // 清除客户区域
		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		iDrawType=dwStyle&LVS_TYPEMASK; // 得到LISTCTRL类型
		switch(iDrawType)
		{
		case LVS_REPORT:  // REPORT风格
				DrawReportMode(hWnd,hdc); // 绘制REPORT模式
				return;
		case LVS_LIST: // LIST风格
				DrawListMode(hWnd,hdc);
				return;
		case LVS_ICON: // ICON风格
				DrawIconMode(hWnd,hdc);
				return;
		case LVS_SMALLICON:  // SMALLICON 风格
				DrawSmallIconMode(hWnd,hdc);
				return;
		}
}
/**************************************************
声明：static void ClearClientArea(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：清除客户区
引用: 
************************************************/
static void ClearClientArea(HWND hWnd,HDC hdc)
{
  RECT rcClientRect;


		GetClientRect(hWnd,&rcClientRect); // 得到客户区域大小
		if (ExistCaption(hWnd)==TRUE) // 是否有CAPTION
			rcClientRect.top+=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT; // 不需要清除CAPTION的区域
	  // clear ClearRect
//	  FillRect(hdc,&rcClientRect,GetStockObject(WHITE_BRUSH));
		SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置当前颜色为正常颜色
		ClearRect(hWnd,hdc,rcClientRect); // 清除矩形
}
/**************************************************
声明：static BOOL ExistCaption(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：有标题，返回TRUE，否则返回FALSE
功能描述：查看是否存在标题
引用: 
************************************************/
static BOOL ExistCaption(HWND hWnd)
{
	DWORD dwStyle;	
	DWORD iDrawType;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			iDrawType=dwStyle&LVS_TYPEMASK;
			if (iDrawType!=LVS_REPORT) // 是否是REPORT
					return FALSE;
			if (dwStyle&LVS_NOCOLUMNHEADER)  // 是否有HEADER
			   return FALSE;
			return TRUE;
}

/**************************************************
声明：static void DrawCaptionItem(HWND hWnd,HDC hdc,int iCol,BOOL bRaised)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iCol -- 要绘制的条目索引
	IN bRaised -- 保留
返回值：无
功能描述：绘制标题条目
引用: 
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

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			
			iCaptionHeight = GetCaptionHeight(hWnd,lpPDAListCtrl); // add by jami in 2004.05.22
			hBkBrush = CreateSolidBrush(lpPDAListCtrl->cl_HeaderBkColor); // 得到标题背景色刷子
			if (hBkBrush == NULL)
			{
				hBkBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
				bDeleteBrush = FALSE;
			}
			// 得到指定标题的开始位置
			cx=lpPDAListCtrl->cx;
			for (i=0;i<iCol;i++)
			{
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
					cx+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			}
			if (iCol>=lpPDAListCtrl->iSubItemNum)
			{ // 是最后一个空白标题
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
			{ // 得到标题所在的大小
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
			{ // 标题包含文本和图象
//				SetBkColor(hdc,GetSysColor(COLOR_3DFACE));
                iBkMode = SetBkMode( hdc, TRANSPARENT );
				rect.left+=2;
				rect.top+=2;
				rect.right-=2;
				rect.bottom-=2;
				// 绘制图象
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
			{  // 包含文本
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
			{ // 包含图象
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
				DeleteObject(hBkBrush); // 删除画刷
			}
			return; 
}
/**************************************************
声明：static int PositionInWindow(HWND hWnd,POINTS points,int *CurItem)
参数：
	IN hWnd -- 窗口句柄
	IN points -- 指定点的坐标
	IN CurItem -- 点所在的位置
返回值：返回点的位置
     BLANKSPACE   : 空白位置
     CAPTIONITEM  : 标题
     CAPTIONSEPARATE : 标题间隙
     LISTITEM     : 条目
功能描述：得到指定点在控件中的位置
引用: 
************************************************/
static int PositionInWindow(HWND hWnd,POINTS points,int *CurItem)
{
		LPPDALISTCTRL lpPDAListCtrl;
		BOOL bExistCaption;
		int iCurCoordinate,i,iLineHeight;
		int iItemNumInLine,iCurWidth,iSubItem;
		
      *CurItem=0;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return BLANKSPACE;
			if (points.y<0)
					return BLANKSPACE;
			bExistCaption=ExistCaption(hWnd); // 是否存在标题
			if (bExistCaption)
			{
					if (points.y<GetCaptionHeight(hWnd,lpPDAListCtrl))
					{ // 在标题上
							iCurCoordinate=lpPDAListCtrl->iWidth+lpPDAListCtrl->cx;
							if (points.x>iCurCoordinate+3) // 最后一个空白标题
								return BLANKSPACE;
							for (i=lpPDAListCtrl->iSubItemNum-1;i>=0;i--)
							{ // 得到在那一个标题
									iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
									iCurWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
									if (points.x>iCurCoordinate-3)
									{ // 在间隔上
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
									{ // 在标题上
											if (CurItem)
											  *CurItem=i;
												return CAPTIONITEM;
									}
									iCurCoordinate-=iCurWidth;
							}
							if (points.x>iCurCoordinate)
							{ // 在第一个标题上
									if (CurItem)
										*CurItem=0;
										return CAPTIONITEM;
							}
							return BLANKSPACE;
					}
					points.y-=GetCaptionHeight(hWnd,lpPDAListCtrl);//CAPTIONHEIGHT;
			}

			iLineHeight=GetLineHeight(hWnd); // 得到条目高度

			iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行的条目个数
			
			// 得到条目的索引
			*CurItem=(points.y/iLineHeight)*iItemNumInLine; 
			*CurItem+=points.x/(GetWindowWidth(hWnd)/iItemNumInLine);

			*CurItem+=lpPDAListCtrl->iStartItem;
			if (*CurItem<lpPDAListCtrl->iItemNum) 
					return LISTITEM; // 条目索引有效，在条目上
			return LISTBLANKSPACE; // 条目索引无效
}

/**************************************************
声明：static void DrawLineSeparate(HWND hWnd,HDC hdc,int xCoordinate)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN xCoordinate -- 垂直隔离线的位置
返回值：无
功能描述：绘制垂直隔离线
引用: 
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
		x0=x1=xCoordinate; // 得到间隔的位置
/*		for (i=0;i<iSubItem;i++)
		{
			x0=x1+=lpPDAListCtrl->lpSubItem[i].iSubItemWidth;
		}
		x0=x1+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;*/
		GetClientRect(hWnd,&rect);
		y0=0;
		y1=(int)rect.bottom; // 得到间隔的高度

//		SetROP2(hdc,R2_NOTXORPEN);
		SetROP2(hdc,R2_NOT);

		// draw a line
		MoveToEx(hdc,x0,y0,NULL);
		LineTo(hdc,x1,y1); // 绘制间隔线

/*		SetROP2(hdc,R2_NOTXORPEN);


    MoveToEx(hdc,x0,y0,NULL);
    LineTo(hdc,x1,y1);*/
		// restore the old set
//    SelectObject(hdc,hOldObj);
		// delete the pen
//    DeleteObject(hPen);
}
/**************************************************
声明：static int GetPageItemNum(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回一页包含的条目数目
功能描述：得到一页包含的条目数目
引用: 
************************************************/
static int GetPageItemNum(HWND hWnd)
{
	int iLineNum,iItemNumInLine,iItemNumInPage;

			iLineNum=GetWindowHeight(hWnd)/GetLineHeight(hWnd)+1;// 得到每页的行数
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行的条目数
			iItemNumInPage=iItemNumInLine*iLineNum; // 得到每页的条目数
			return iItemNumInPage;
}
/**************************************************
声明：static void DrawReportMode(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制报告模式
引用: 
************************************************/
static void DrawReportMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;
	DWORD dwStyle;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			iCurItem=lpPDAListCtrl->iStartItem; // 得到开始条目的索引
			iPageItem=GetPageItemNum(hWnd); // 得到每页的条目数
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置颜色
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawReportItem(hWnd,hdc,iCurItem,i); // 绘制REPORT条目
				iCurItem++;
			}
		  if (dwStyle&LVS_HORZLINE)
			{
					DrawHorzListLine(hWnd,hdc); // 绘制水平线
			}
		  if (dwStyle&LVS_VERTLINE)
			{
					DrawVertListLine(hWnd,hdc); // 绘制垂直线
			}
}
/**************************************************
声明：static int GetLineHeight(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回行高
功能描述：得到行高
引用: 
************************************************/
static int GetLineHeight(HWND hWnd)
{
	DWORD iDrawType;
	HDC hdc;
	TEXTMETRIC TextMetric;
  int nTextHeight;
  DWORD dwStyle;

	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return 0;

		if (lpPDAListCtrl->iLineHeight) // Add By Jami chen in 2004.05.21)
			return lpPDAListCtrl->iLineHeight;

		hdc=GetDC(hWnd);

		// get current text property
		GetTextMetrics(hdc,&TextMetric);
		ReleaseDC(hWnd,hdc);
		nTextHeight=(int)TextMetric.tmHeight;

		nTextHeight += 2; // 得到文本高度

//		nTextHeight=USETEXTHEIGHT; // 得到文本高度

	    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口高度
		iDrawType=dwStyle&LVS_TYPEMASK;
		if (iDrawType==LVS_ICON)
		{// 是ICON ，加上图象的高度及间隔
			nTextHeight+=3; // separate in the bitmap and caption
			nTextHeight+=(int)lpPDAListCtrl->sizeIcon.cy; // width of the icon
			nTextHeight+=lpPDAListCtrl->iVertSpacing;
		}
//			nTextHeight+=1; // separate in the lines
		if (dwStyle&LVS_HORZLINE)
			nTextHeight+=1; // 加上线的高度
		return nTextHeight;
}
/**************************************************
声明：static int GetWindowWidth(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回窗口句柄
功能描述：得到窗口句柄
引用: 
************************************************/
static int GetWindowWidth(HWND hWnd)
{
	RECT rect;
		GetClientRect(hWnd,&rect); // 得到矩形的大小
	return (int)(rect.right-rect.left);
}
static int GetWindowHeight(HWND hWnd)
{
	RECT rect;
	int iWindowHeight;
		GetClientRect(hWnd,&rect); // 得到客户矩形
		iWindowHeight=(int)(rect.bottom-rect.top);
		if (ExistCaption(hWnd)==TRUE) // 有标题
			iWindowHeight-=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT; 
		if (iWindowHeight<0)
			iWindowHeight=0;
	return iWindowHeight;
}
/**************************************************
声明：static void DrawReportItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iDrawItem -- 条目索引
	IN iDrawLine -- 条目绘制的行索引
返回值：无
功能描述：绘制报告条目
引用: 
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

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if (ExistCaption(hWnd)==TRUE) // 是否有标题
				iTopCoordinate=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
			else
				iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // 得到条目高度
			// 得到条目矩形
			rect.left=rcLine.left=lpPDAListCtrl->cx;
			rect.top=rcLine.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rcLine.bottom=rect.top+iLineHeight;
			rcLine.right=rect.right=rect.left+lpPDAListCtrl->iWidth;
			if (dwStyle&LVS_HORZLINE)
			{// 有水平线
				rcLine.bottom--;
				rect.bottom--;
			}
			if (dwStyle&LVS_VERTLINE)
			{ // 有垂直线
				rect.right--;
				rcLine.right--;
			}
			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // 用户自绘
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}
			iDrawItemNo=iDrawItem;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // 得到条目结构
			if (lpMainItem==NULL) return ;
			if (lpMainItem->State&LVIS_SELECTED)
			{ // 当前为选择条目
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
				bInvert=TRUE;
			}
			else
			{ // 当前为通常条目
				SetPDAListColor(hWnd,hdc,NORMALCOLOR);
				bInvert=FALSE;
			}
			bFocus=lpMainItem->State&LVIS_FOCUSED; // 是否有焦点
// !!! draw this item ,first clear this line
// !!! Add By Jami chen 2002.04.24
			ClearRect(hWnd,hdc,rcLine); // 清除条目
//			ClearLine(hWnd,hdc,iDrawLine);
// !!! add end By Jami chen 2002.04.24
			for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 绘制个子条目
					iCurSubItem=lpPDAListCtrl->lpColumnOrderArray[i]; // 得到条目索引
					rect.right=rect.left+lpPDAListCtrl->lpSubItem[iCurSubItem].iSubItemWidth;
					if (iCurSubItem==0)
					{
						iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,iCurSubItem,lpFileFeild); // 得到条目文本
						lpSubItemString=lpFileFeild;
						if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
						{ // 有图象，绘制图象
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
									rect.left+=IMAGESPACE; // 调过间隔
						}
						if (i == 0)
							rcLine.left=rect.left;
// !!! Delete By Jami chen 2002.04.24
//						ClearRect(hWnd,hdc,rcLine);
// !!! Delete End By Jami chen 2002.04.24
						if (lpSubItemString)
						{ // 绘制文本
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
					{ // 绘制文本
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
						{ // 绘制图象
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
						{ // 绘制文本
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
				DrawFocusRect(hdc,&rcLine); // 绘制焦点框
}
/**************************************************
声明：static void SetPDAListColor(HWND hWnd,HDC hdc,int nColorStyle)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN nColorStyle -- 颜色类型
返回值：无
功能描述：设置控件颜色
引用: 
************************************************/
static void SetPDAListColor(HWND hWnd,HDC hdc,int nColorStyle)
{
	LPPDALISTCTRL lpPDAListCtrl;

		  lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
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
声明：static void ClearLine(HWND hWnd,HDC hdc,int iDrawLine)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iDrawLine --要清除的行索引
返回值：无
功能描述：清除指定的索引行
引用: 
************************************************/
static void ClearLine(HWND hWnd,HDC hdc,int iDrawLine)
{
   RECT ClearRect;
   int nLineHeight;
	 int nWindowWidth;
	 HBRUSH hBrush;

	 //  get line height
	 nLineHeight=GetLineHeight(hWnd); // 得到行高度
	 // get window width
	 nWindowWidth=GetWindowWidth(hWnd); // 得到窗口宽度
	
	// calculate clear rect
	ClearRect.left=0;
	if (ExistCaption(hWnd)==TRUE)
			ClearRect.top=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL)+iDrawLine*nLineHeight;
	else
			ClearRect.top=iDrawLine*nLineHeight;
	ClearRect.right=nWindowWidth;
	ClearRect.bottom =ClearRect.top+nLineHeight;
	hBrush=CreateSolidBrush(GetBkColor(hdc));  // 创建背景刷
	FillRect(hdc,&ClearRect,hBrush); // 清除矩形
	DeleteObject(hBrush);
//	ReleaseDC(hWnd,hdc);
}
/**************************************************
声明：static void ClearRect(HWND hWnd,HDC hdc,RECT rect)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN rect -- 要清除的区域
返回值：无
功能描述：清除一个指定区域
引用: 
************************************************/
static void ClearRect(HWND hWnd,HDC hdc,RECT rect)
{
	 HBRUSH hBrush;

	hBrush=CreateSolidBrush(GetBkColor(hdc)); // 创建背景刷
	FillRect(hdc,&rect,hBrush); // 清除矩形
	DeleteObject(hBrush);
}
/**************************************************
声明：static int MakeShortString(HDC hdc, int nColumnLen,LPCTSTR lpLongString,LPTSTR lpShortString)
参数：
	IN hdc -- 设备句柄
	IN nColumnLen -- 转化后字符串的最大宽度
	IN lpLongString -- 转化前的长字符串
	OUT lpShortString -- 转化后的短字符串
返回值：返回转化后字符串的长度
功能描述：转化成字符串的宽度
引用: 
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
	 if(GetTextExtent(hdc,lpLongString, nStringLen)<= nColumnLen) // 得到文本长度
	 { // 文本宽度小于要求宽度，不需要转化
		    strcpy(lpShortString,lpLongString);
				nStringWidth=GetTextExtent(hdc,lpShortString, strlen(lpShortString));
				return nStringWidth;
	 }
	 nAddLen=GetTextExtent(hdc,szThreeDots, strlen(szThreeDots)); // 得到...的宽度

	 i=0;
// !!! Modified By Jami chen in 2003.09.06	 
//	 if (lpLongString[0]<0)
	 if (lpLongString[0] & 0x80)
// !!! Modified By Jami chen in 2003.09.06	 
	 { // 是汉字
				nAddLen+=GetTextExtent(hdc,&lpLongString[0], 2);
			  lpShortString[i]=lpLongString[i];
				i++;
			  lpShortString[i]=lpLongString[i];
				i++;
	 }
	 else
	 { // 是字符
				nAddLen+=GetTextExtent(hdc,&lpLongString[0], 1);
			  lpShortString[i]=lpLongString[i];
				i++;
	 }
   for(; i<nStringLen-1;i++)
	 { // 计算可否放下该字
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
	nStringWidth=GetTextExtent(hdc,lpShortString, strlen(lpShortString)); // 得到转化后的字串长度
	return nStringWidth;
}
/**************************************************
声明：static int GetTextExtent(HDC hdc,LPCTSTR lpszString, int nCount)
参数：
	IN hdc -- 设备句柄
	IN lpszString -- 要得到宽度的字符串
	IN nCount -- 字符串长度
返回值：返回字符串的宽度
功能描述：得到字符串的宽度
引用: 
************************************************/
static int GetTextExtent(HDC hdc,LPCTSTR lpszString, int nCount)
{
		SIZE size;
		GetTextExtentPoint32(hdc, lpszString, (int)nCount, &size); // 得到字串的宽度
		return (int)size.cx;
}
/**************************************************
声明：static void DrawListMode(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc --设备句柄
返回值：无
功能描述：绘制控件的列式模式
引用: 
************************************************/
static void DrawListMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iCurItem=lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // 得到每页的条目个数
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawListItem(hWnd,hdc,iCurItem,i); // 绘制LIST条目
				iCurItem++;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,TRUE);
}
/**************************************************
声明：static void DrawIconMode(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制控件的图标模式
引用: 
************************************************/
static void DrawIconMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iCurItem=lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // 得到每页的条目个数
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawIconItem(hWnd,hdc,iCurItem,i); // 绘制ICON条目
				iCurItem++;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,TRUE);
}
/**************************************************
声明：static void DrawSmallIconMode(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制控件的小图标模式
引用: 
************************************************/
static void DrawSmallIconMode(HWND hWnd,HDC hdc)
{
	LPPDALISTCTRL lpPDAListCtrl;
	int iCurItem;
	int i,iPageItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iCurItem=lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // 得到每页的条目个数
			for (i=0;i<iPageItem;i++)
			{
				if (iCurItem>=lpPDAListCtrl->iItemNum) break;
				DrawSmallIconItem(hWnd,hdc,iCurItem,i); // 绘制SMALLICON条目
				iCurItem++;
			}
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,TRUE);
}
/**************************************************
声明：static void DrawListItem(HWND hWnd,HDC hdc,int iDrawItem,int iDrawLine)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iDrawItem -- 条目索引
	IN iDrawLine -- 行索引
返回值：无
功能描述：绘制列式条目
引用: 
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
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // 得到行高度
			rect.left=lpPDAListCtrl->cx;
			rect.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rect.top+iLineHeight;
			rect.right=GetWindowWidth(hWnd);

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // 用户自绘
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}

			iDrawItemNo=iDrawItem;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // 得到条目结构
			if (lpMainItem==NULL)
				return;
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置颜色
			ClearLine(hWnd,hdc,iDrawLine);
			if (lpMainItem->State&LVIS_SELECTED)
			{ // 当前为选择条目
				SetPDAListColor(hWnd,hdc,INVERTCOLOR); // 设置颜色
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
				SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置颜色
				bInvert=FALSE;
			}

			iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,0,lpFileFeild); // 得到条目字串
			lpSubItemString=lpFileFeild;

			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // 是图象,绘制图象
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
			// 得到绘制文本
			iDrawStringWidth=MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			rect.right=rect.left+iDrawStringWidth;

//			if (bInvert == TRUE)
//				FillRect(hdc,&rect,hBkBrush);
			// Add By Jami chen
			if (bInvert == TRUE)
				ClearRect(hWnd,hdc,rect);
			// Add End By Jami chen
			// 绘制文本
			DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
			bFocus=lpMainItem->State&LVIS_FOCUSED;
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,bFocus);
			if (bFocus)
				DrawFocusRect(hdc,&rect); // 绘制焦点矩形

			if (hBkBrush  && bDeleteBrush)
			{
				DeleteObject(hBkBrush);
			}
}
/**************************************************
声明：static void InvalidateItem(HWND hWnd,int iCurItem)
参数：
	IN hWnd -- 窗口句柄
	IN iCurItem -- 要无效的条目索引
返回值：无
功能描述：无效指定的条目
引用: 
************************************************/
static void InvalidateItem(HWND hWnd,int iCurItem)
{
	LPPDALISTCTRL lpPDAListCtrl;
//	int iDrawLine,iPageItem;
	RECT rcItem;
	int iTopCoordinate,iLineHeight,iDrawLine,iDrawxPos,iWindowWidth,iItemNumInLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
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
			iLineHeight=GetLineHeight(hWnd); // 得到条目高度
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目个数
			iWindowWidth=GetWindowWidth(hWnd); // 得到窗口宽度
			iDrawLine=(iCurItem-lpPDAListCtrl->iStartItem)/iItemNumInLine; // 得到要绘制的行数
			iDrawxPos=(iWindowWidth/iItemNumInLine)*(iCurItem%iItemNumInLine); // 得到要开始绘制的开始位置

			rcItem.left=iDrawxPos;
			rcItem.top=iTopCoordinate+iLineHeight*iDrawLine;
			rcItem.bottom=rcItem.top+iLineHeight;
			rcItem.right=rcItem.left+iWindowWidth/iItemNumInLine;
			InvalidateRect(hWnd,&rcItem,TRUE); // 无效矩形
}
/**************************************************
声明：static void DrawItem(HWND hWnd,int iCurItem)
参数：
	IN hWnd -- 窗口句柄
	IN iCurItem  -- 要绘制的条目索引
返回值：无
功能描述：绘制指定条目
引用: 
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

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			if (iCurItem<0)
				return ;
			if (iCurItem>=lpPDAListCtrl->iItemNum)
				return;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			iDrawType=dwStyle&LVS_TYPEMASK;
			iDrawLine=iCurItem-lpPDAListCtrl->iStartItem;
			iPageItem=GetPageItemNum(hWnd); // 得到页条目个数
			if (iDrawLine<0||iDrawLine>=iPageItem)
				return;

			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // 得到条目结构
			if (lpMainItem==NULL)
				return ;
      hdc=GetDC(hWnd);
			switch(iDrawType)
			{
			case LVS_REPORT:
					DrawReportItem(hWnd,hdc,iCurItem,iDrawLine); // 绘制REPORT条目
		      if (dwStyle&LVS_VERTLINE)
			    {
					    DrawVertListLine(hWnd,hdc); // 绘制水平线
			    }
					break;
			case LVS_LIST:
					DrawListItem(hWnd,hdc,iCurItem,iDrawLine); // 绘制LIST条目
					break;
			case LVS_SMALLICON:
					DrawSmallIconItem(hWnd,hdc,iCurItem,iDrawLine); // 绘制SMALLICON条目
					break;
			case LVS_ICON:
					DrawIconItem(hWnd,hdc,iCurItem,iDrawLine); // 绘制ICON条目
					break;
			}
      ReleaseDC(hWnd,hdc);
#endif
}
#ifdef USE_BITMAP_IMAGE
/**************************************************
声明：static void DrawBitmap(HDC hdc,HBITMAP hBitMap,int x,int y,int iWidth,int iHeight,BOOL bInvert)
参数：
	IN hdc -- 设备句柄
	IN hBitmap -- 位图句柄
	IN x    画位图的起点X坐标
	IN y -- 画位图的起点Y坐标
	IN iWidth -- 位图宽度
	IN iHeight -- 位图高度
	IN bInvert -- 是否要反色
返回值：无
功能描述：画位图
引用: 
************************************************/
static void DrawBitmap(HDC hdc,HBITMAP hBitMap,int x,int y,int iWidth,int iHeight,BOOL bInvert)
{
		HDC hMemoryDC;
		DWORD dwRop=SRCCOPY;

//			if(bInvert)
//				dwRop=SRCINVERT;
			if(bInvert)
			{
				SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置颜色
				dwRop=NOTSRCCOPY;
			}
			hMemoryDC=CreateCompatibleDC(hdc); // 得到兼容DC
			hBitMap = SelectObject(hMemoryDC,hBitMap); // 装载要绘制的图象
			// 绘制图象
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
声明：static void DrawSmallIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iDrawItem -- 要绘制的条目索引
	IN iCurPosition -- 当前位置
返回值：无
功能描述：绘制小图标条目
引用: 
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
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iDrawLine=iCurPosition/SMALLICON_NUMINLINE; // 得到行数
			iDrawxPos=lpPDAListCtrl->cx+(GetWindowWidth(hWnd)/SMALLICON_NUMINLINE)*(iCurPosition%SMALLICON_NUMINLINE); // 得到开始位置
			iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd); // 得到行高
			// 得到条目矩形
			rect.left=iDrawxPos;
			rect.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rect.top+iLineHeight;
			rect.right=rect.left+GetWindowWidth(hWnd)/SMALLICON_NUMINLINE;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // 用户自绘
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}
			iDrawItemNo=iDrawItem; 
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // 得到条目数据
			if (lpMainItem==NULL)
				return;
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置颜色
			ClearRect(hWnd,hdc,rect); // 清除矩形
			if (lpMainItem->State&LVIS_SELECTED)
			{ // 当前是选择条目
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
				bInvert=TRUE;
			}
			else
			{ // 当前是一般条目
				SetPDAListColor(hWnd,hdc,NORMALCOLOR);
				bInvert=FALSE;
			}

			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // 绘制图象
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
			// 得到条目字串			
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
			// 得到转化字串
			iDrawStringWidth=MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			rect.right=rect.left+iDrawStringWidth;
			// Add By Jami chen
			if (bInvert == TRUE)
				ClearRect(hWnd,hdc,rect);
			// 绘制字串
			// Add End By Jami chen
			DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,DT_LEFT|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
			bFocus=lpMainItem->State&LVIS_FOCUSED;
//			DrawFocus(hWnd,hdc,lpPDAListCtrl->iFocusItem,bFocus);
			if (bFocus)
				DrawFocusRect(hdc,&rect); // 绘制焦点框
}
/**************************************************
声明：static void DrawIconItem(HWND hWnd,HDC hdc,int iDrawItem,int iCurPosition)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iDrawItem -- 要绘制的条目索引
	IN iCurPosition -- 当前位置
返回值：无
功能描述：绘制图标条目
引用: 
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
		
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目数目
			// 得到条目位置
			iDrawLine=iCurPosition/iItemNumInLine;
			iDrawxPos=lpPDAListCtrl->cx+(GetWindowWidth(hWnd)/iItemNumInLine)*(iCurPosition%iItemNumInLine);
			iTopCoordinate=0;
			iLineHeight=GetLineHeight(hWnd);
			// 得到条目矩形
			rect.left=iDrawxPos;
			rect.top=iTopCoordinate+iLineHeight*iDrawLine;
			rect.bottom=rect.top+iLineHeight;
			rect.right=rect.left+GetWindowWidth(hWnd)/iItemNumInLine;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if (dwStyle&LVS_OWNERDRAWFIXED)
			{ // 用户自绘
				SendDrawItemMessage(hWnd,hdc,iDrawItem,rect);
				return;
			}
			iDrawItemNo=iDrawItem;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iDrawItemNo); // 得到条目结构数据
			if (lpMainItem==NULL)
				return;
			SetPDAListColor(hWnd,hdc,NORMALCOLOR); // 设置颜色
			ClearRect(hWnd,hdc,rect);
			if (lpMainItem->State&LVIS_SELECTED)
			{ // 当前为选择条目
				SetPDAListColor(hWnd,hdc,INVERTCOLOR);
				bInvert=TRUE;
			}
			else
			{ // 当前为通常条目
				SetPDAListColor(hWnd,hdc,NORMALCOLOR);
				bInvert=FALSE;
			}

			rect.top+=lpPDAListCtrl->iVertSpacing; // separate in the bitmap and caption

			// 得到条目字串
			iImage=GetItemString(hWnd,lpPDAListCtrl,iDrawItemNo,0,lpFileFeild);
			lpSubItemString=lpFileFeild;
			
			if (lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_IMAGE)
			{ // 绘制图象
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
			// 得到转化字串
			iDrawStringWidth=MakeShortString(hdc,rect.right-rect.left,lpSubItemString,lpDrawString);
			rect.left+=((rect.right-rect.left)-iDrawStringWidth)/2;
			rect.right=rect.left+iDrawStringWidth;

			// Add By Jami chen
			if (bInvert == TRUE)
				ClearRect(hWnd,hdc,rect);
			// Add End By Jami chen
			// 绘制字串
			DrawText(hdc,lpDrawString,strlen(lpDrawString),&rect,DT_CENTER|DT_NOPREFIX|DT_SINGLELINE|DT_VCENTER);
			bFocus=lpMainItem->State&LVIS_FOCUSED;
			if (bFocus)
				DrawFocusRect(hdc,&rect); // 绘制焦点矩形
}
/**************************************************
声明：static void SendDrawItemMessage(HWND hWnd,HDC hdc,int iCurItem,RECT rcItem)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
	IN iCurItem -- 当前条目索引
	IN rcItem -- 条目的矩形大小
返回值：无
功能描述：发送绘制条目的消息，自绘时使用
引用: 
************************************************/
static void SendDrawItemMessage(HWND hWnd,HDC hdc,int iCurItem,RECT rcItem)
{
		DRAWITEMSTRUCT dis;
		UINT idCtrl;
		LPMAINITEM lpMainItem;
		LPPDALISTCTRL lpPDAListCtrl;
		HWND hParent;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;

			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // 得到条目结构
			idCtrl=(UINT)GetWindowLong(hWnd,GWL_ID); // 得到控件ID号
			// 设置自绘结构
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
			SendMessage(hParent,WM_DRAWITEM,(WPARAM)idCtrl,(LPARAM)&dis); // 发送自绘消息
}
#ifdef USE_BITMAP_IMAGE
/**************************************************
声明：static void GetBitMapSize(HBITMAP hBitMap,LPSIZE lpSize)
参数：
	IN hBitMap -- 位图句柄
	OUT lpSize -- 位图的大小
返回值：无
功能描述：得到位图的大小
引用: 
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

		GetObject(hBitMap, sizeof(BITMAP), &bitmap); // 得到位图参数

		// 得到位图大小
		lpSize->cx=bitmap.bmWidth;
		lpSize->cy=bitmap.bmHeight;

}
#endif
/**************************************************
声明：static void SetVScrollBar(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：设置垂直滚动条
引用: 
************************************************/
static void SetVScrollBar(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		DWORD dwStyle;
    int nMinPos,nMaxPos,nPageLine,nTotalLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			// get window style 
		    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			// get page Lines
			nPageLine=GetPageLine(hWnd); // 得到每页的行数
			if (nPageLine == 0)
				nPageLine = 1;
			nTotalLine=GetTotalLine(hWnd); // 得到总行数
			if (nTotalLine>nPageLine)
			{ // 有垂直滚动条
					if ((dwStyle&WS_VSCROLL)==0)
					{// 显示垂直滚动条
						ShowScrollBar(hWnd,SB_VERT,TRUE);
						SetHScrollBar(hWnd);
					}	
					nMinPos=0;
					nMaxPos=nTotalLine-1;
					if (nMaxPos == 0)
						nMaxPos = 1;
					LV_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE); // 设置滚动条的范围
					SetVScrollPage(hWnd); // 设置滚动页
			}
			else
			{
					if (dwStyle&WS_VSCROLL)
					{ // 隐藏滚动条
						lpPDAListCtrl->iStartItem=0;
						ShowScrollBar(hWnd,SB_VERT,FALSE);
						SetHScrollBar(hWnd);
					}
			}
}

/**************************************************
声明：static void SetVScrollPage(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：设置垂直滚动条的页大小
引用: 
************************************************/
static void SetVScrollPage(HWND hWnd)
{
	int nPageLines;
	DWORD dwStyle;
  SCROLLINFO ScrollInfo;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		if (dwStyle&WS_VSCROLL)
		{
			nPageLines=GetPageLine(hWnd); // 得到每页的行数
			ScrollInfo.cbSize=sizeof(SCROLLINFO);
			ScrollInfo.fMask=SIF_PAGE;
		  // set vertical scroll page
			ScrollInfo.nPage =nPageLines;
			SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,FALSE);
			EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH); // 设置每页的大小
		}
}
/**************************************************
声明：static void SetHScrollBar(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：设置水平滚动条
引用: 
************************************************/
static void SetHScrollBar(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		DWORD dwStyle;
    int nMinPos,nMaxPos,nPageLine,nTotalLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			// get window style 
			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if ((dwStyle&LVS_TYPEMASK)!=LVS_REPORT)
			{ // 不是REPORT风格
				if (dwStyle&WS_HSCROLL)
				{// 隐藏滚动条
					lpPDAListCtrl->cx=0;
					ShowScrollBar(hWnd,SB_HORZ,FALSE);
					SetVScrollBar(hWnd);
				}
			   return;
			}
			// get page Lines
			nPageLine=GetWindowWidth(hWnd)/WIDTHSTEP; // 得到每页的数目
			if (nPageLine == 0)
				nPageLine = 2;
			nTotalLine=(lpPDAListCtrl->iWidth+WIDTHSTEP-1)/WIDTHSTEP; // 得到总宽度
			if (nTotalLine>nPageLine)
			{ // 需要水平滚动条
				if ((dwStyle&WS_HSCROLL)==0)
				{ // 没有滚动条,显示滚动条
					ShowScrollBar(hWnd,SB_HORZ,TRUE);
					SetVScrollBar(hWnd);
				}
				nMinPos=0;
				nMaxPos=nTotalLine-1;
				if (nMaxPos == 0)
					nMaxPos = 1;
				LV_SetScrollRange(hWnd,SB_HORZ,nMinPos,nMaxPos,TRUE); // 设置滚动条的范围
				SetHScrollPage(hWnd); // 设置滚动条的页数据
			}
			else
			{  // 不需要滚动条
				if (dwStyle&WS_HSCROLL)
				{ // 有滚动条,隐藏滚动条
					lpPDAListCtrl->cx=0;
					ShowScrollBar(hWnd,SB_HORZ,FALSE);
					SetVScrollBar(hWnd);
				}
			}
}
/**************************************************
声明：static void SetHScrollPage(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：设置水平滚动条的页大小
引用: 
************************************************/
static void SetHScrollPage(HWND hWnd)
{
	int nPageLines;
	DWORD dwStyle;
    SCROLLINFO ScrollInfo;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			if (dwStyle&WS_HSCROLL) 
			{
				nPageLines=GetWindowWidth(hWnd)/WIDTHSTEP; // 得到页大小
				ScrollInfo.cbSize=sizeof(SCROLLINFO);
				ScrollInfo.fMask=SIF_PAGE;
			  // set vertical scroll page
				ScrollInfo.nPage =nPageLines;
				SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,FALSE); // 设置页数据
				EnableScrollBar(hWnd,SB_HORZ,ESB_ENABLE_BOTH);
			}
}
/**************************************************
声明：static int GetPageLine(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回一页的行数
功能描述：得到一页的行数
引用: 
************************************************/
static int GetPageLine(HWND hWnd)
{
	int iLineNum;

			iLineNum=GetWindowHeight(hWnd)/GetLineHeight(hWnd); // 得到每页的行数
			return iLineNum;
}
/**************************************************
声明：static int GetTotalLine(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回控件总行数
功能描述：得到控件总行数
引用: 
************************************************/
static int GetTotalLine(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int nTotalLines,iItemNumInLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return 0;
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行的条目数
			nTotalLines=(lpPDAListCtrl->iItemNum+iItemNumInLine-1)/iItemNumInLine; // 得到总行数
			return nTotalLines;
}
/**************************************************
声明：static int GetItemNumInLine(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回一行的条目个数
功能描述：得到一行的条目个数
引用: 
************************************************/
static int GetItemNumInLine(HWND hWnd)
{
		DWORD dwStyle,iDrawType;
		int iItemNumInLine;
			// get window style 
      dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
			// get page Lines

			iDrawType=dwStyle&LVS_TYPEMASK; // 得到显示类型
			switch(iDrawType)
			{
			case LVS_ICON:
				{  // ICON类型
					int iWindowWidth,iSpacing;
					LPPDALISTCTRL lpPDAListCtrl;
						
						lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
						// 得到间隙宽度
						if(lpPDAListCtrl==NULL)
							iSpacing=0;
						else
							iSpacing=lpPDAListCtrl->iHorzSpacing;
						iWindowWidth=GetWindowWidth(hWnd); // 得到窗口宽度
						iItemNumInLine=iWindowWidth/(ICONWIDTH+iSpacing); // 得到条目个数
					break;
				}
			case LVS_REPORT:
				iItemNumInLine=REPORT_NUMINLINE; // 返回REPORT每行的条目数
				break;
			case LVS_SMALLICON:
				iItemNumInLine=SMALLICON_NUMINLINE;  // 返回SMALLICON每行的条目数
				break;
			case LVS_LIST:
				iItemNumInLine=LIST_NUMINLINE; // 返回LIST每行的条目数
				break;
			}
			return iItemNumInLine;
}
/**************************************************
声明：static void VScrollWindow(HWND hWnd,int nScrollLine)
参数：
	IN hWnd -- 窗口句柄
	IN nScrollLine -- 滚动的行数
返回值：无
功能描述：垂直滚动窗口
引用: 
************************************************/
static void VScrollWindow(HWND hWnd,int nScrollLine)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iItemNumInLine,iPageLines,iTotalLines,iCurLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行条目数
			iTotalLines=GetTotalLine(hWnd); // 得到总行数
			iPageLines=GetPageLine(hWnd); // 得到每页的行数
			iCurLine=lpPDAListCtrl->iStartItem/iItemNumInLine; // 得到当前的行数
			if (iCurLine==0&&nScrollLine<0)
				return; // can not scroll up
			if ((iCurLine+iPageLines>=iTotalLines)&&nScrollLine>0)		
				return; // can not scroll down
			iCurLine+=nScrollLine;
			if (iCurLine<0)
				iCurLine=0;
			if (iCurLine+iPageLines>iTotalLines)
			{ // 已经超过最后
				iCurLine=iTotalLines-iPageLines;
			}
			if (iCurLine<0) 
				iCurLine=0;
			lpPDAListCtrl->iStartItem=iCurLine*iItemNumInLine; // 得到开始条目
			LV_SetScrollPos(hWnd,SB_VERT,iCurLine,TRUE); // 设置滚动条的位置
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
}

/**************************************************
声明：static void HScrollWindow(HWND hWnd,int nScrollLine)
参数：
	IN hWnd -- 窗口句柄
	IN nScrollLine -- 要滚动的行数
返回值：无
功能描述：水平滚动窗口
引用: 
************************************************/
static void HScrollWindow(HWND hWnd,int nScrollLine)
{
		LPPDALISTCTRL lpPDAListCtrl;
		int iPageLines,iTotalLines,iCurLine;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
//			iTotalLines=lpPDAListCtrl->iWidth/WIDTHSTEP;
			iTotalLines=(lpPDAListCtrl->iWidth+WIDTHSTEP-1)/WIDTHSTEP; // 得到总数
			iPageLines=GetWindowWidth(hWnd)/WIDTHSTEP; // 得到页数目
			iCurLine=LV_GetScrollPos(hWnd,SB_HORZ); // 得到当前位置
			if (iCurLine==0&&nScrollLine<0)
				return; // can not scroll up
			if ((iCurLine+iPageLines>=iTotalLines)&&nScrollLine>0)				return; // can not scroll down
			iCurLine+=nScrollLine;
			if (iCurLine<0)
				iCurLine=0;
			if (iCurLine+iPageLines>iTotalLines)
			{ // 已经到最后
				iCurLine=iTotalLines-iPageLines;
			}
			if (iCurLine<0) 
				iCurLine=0;
			lpPDAListCtrl->cx=0-iCurLine*WIDTHSTEP;
			LV_SetScrollPos(hWnd,SB_HORZ,iCurLine,TRUE); // 设置滚动条的位置
			InvalidateRect(hWnd,NULL,TRUE);
}
/**************************************************
声明：static int StrCmp(LPCTSTR lpstr1,LPCTSTR lpstr2)
参数：
	IN lpstr1 -- 字符串1
	IN lpstr2 -- 字符串2
返回值：字符串1 大于 字符串2 返回 1
		字符串1 小于 字符串2 返回 -1
		字符串1 等于 字符串2 返回 0
功能描述：比较字符串大小
引用: 
************************************************/
static int StrCmp(LPCTSTR lpstr1,LPCTSTR lpstr2)
{ int iResult;
	
		iResult=strcmp(lpstr1,lpstr2); // 比较字符串
		if (iResult>0) return 1;
		if (iResult<0) return -1;
		return 0;
}
/**************************************************
声明：static int AutoSortItem(LPPDALISTCTRL lpPDAListCtrl,LPMAINITEM lpInsertItem,int order)
参数：
	IN lpPDAListCtrl -- 控件数据结构
	IN lpInsertItem -- 要插入的条目
	IN order -- 排序次序
返回值：返回指定条目要插入的索引
功能描述：自动排列条目
引用: 
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
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iStart); // 得到条目结构
			iResult=StrCmp(lpInsertItem->lpText,lpMainItem->lpText); // 比较条目的大小
			if (iResult==upCondition)
				return 0;
			iEnd=lpPDAListCtrl->iItemNum-1;
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iEnd); // 得到最后一个条目的结构
			iResult=StrCmp(lpInsertItem->lpText,lpMainItem->lpText); // 比较条目大小
			if (iResult!=upCondition)
				return lpPDAListCtrl->iItemNum;
			// 使用二分法得到插入条目的位置
			while(1)
			{
					iCurItem=(iStart+iEnd)>>1;	
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iCurItem); // 得到当前条目的结构
					iResult=StrCmp(lpInsertItem->lpText,lpMainItem->lpText); // 与插入条目比较大小
					if (iStart==iCurItem)
					{// 条目相等
						ExitCondition++;
						if (ExitCondition>=2)
						{ // 结束条件成立
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
声明：static void FreeItem(LPMAINITEM lpMainItem,BOOL bText)
参数：
	IN lpMainitem -- 要释放的条目
	IN bText -- 是否为文本
返回值：无
功能描述：释放指定条目
引用: 
************************************************/
static void FreeItem(LPMAINITEM lpMainItem,BOOL bText)
{
			if (lpMainItem)
			{
				if (bText)
				{
					if (lpMainItem->lpText)
						free(lpMainItem->lpText); // 释放条目文本
				}
				free(lpMainItem); // 释放条目
			}
}
/**************************************************
声明：static void RemoveItem(LPPDALISTCTRL lpPDAListCtrl,int iItem)
参数：
	IN lpPDAListCtrl -- 控件数据结构
	IN iItem -- 要删除的条目索引
返回值：无
功能描述：删除指定条目
引用: 
************************************************/
static void RemoveItem(LPPDALISTCTRL lpPDAListCtrl,int iItem)
{
	LPMAINITEM lpMainItem;
	int i;
			lpMainItem= PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构
			PtrListAtRemove(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 从列表中删除条目
			// 释放条目
			if (!(lpPDAListCtrl->lpSubItem[0].ContentFmt&LVCFMT_FILE))
				FreeItem(lpMainItem,TRUE); 
			else
				FreeItem(lpMainItem,FALSE);
			for (i=1;i<lpPDAListCtrl->iSubItemNum;i++)
			{ // 删除子条目
				if (!(lpPDAListCtrl->lpSubItem[i].ContentFmt&LVCFMT_FILE))
					PtrListAtFree(&lpPDAListCtrl->lpSubItem[i].plSubItemContent,iItem); // 释放子条目
			}
			lpPDAListCtrl->iItemNum--; // 减少一个条目
			if (lpPDAListCtrl->iStartItem>=lpPDAListCtrl->iItemNum)
			{ // 需要滚动窗口
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
声明：static void GetPointPosition(HWND hWnd,POINTS points,int *iRow,int *iColumn)
参数：
	IN hWnd -- 窗口句柄
	IN points -- 指定的点
	OUT iRow -- 点所在的行索引
	OUT iColumn -- 点所在的列索引
返回值：无
功能描述：得到点所在的控件中的位置
引用: 
************************************************/
static void GetPointPosition(HWND hWnd,POINTS points,int *iRow,int *iColumn)
{
	int x,y;
	POINT ptOrg;
	int iLineHeight,iWindowWidth,iItemNumInLine,iCaptionHeight;

		SendMessage(hWnd,LVM_GETORIGIN,(WPARAM)0,(LPARAM)&ptOrg); // 得到原点
		if (ExistCaption(hWnd)) // 是否存在标题
			iCaptionHeight=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT;
		else
			iCaptionHeight=0;
		x=points.x-(int)ptOrg.x;
		y=points.y-ptOrg.y-iCaptionHeight;
		iLineHeight=GetLineHeight(hWnd); // 得到行高度
		iWindowWidth=GetWindowWidth(hWnd); // 得到窗口宽度
		iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行条目数
		// 得到点所在行
		*iRow=y/iLineHeight;
		if (*iRow<0)
			*iRow=0;
		// 得到点所在列
		*iColumn=x/(iWindowWidth/iItemNumInLine);
		if (*iColumn>=iItemNumInLine)
			*iColumn=iItemNumInLine-1;
}
// !!! Add By Jami chen 2002.04.25
/**************************************************
声明：static int GetSubItemPointPosition(HWND hWnd,POINTS points)
参数：
	IN hWnd -- 窗口句柄
	IN points -- 指定点的坐标
返回值：成功返回指定的子条目的索引，否则返回-1
功能描述：得到指定点所在的子条目
引用: 
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


		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return -1;
	    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
		iDrawType=dwStyle&LVS_TYPEMASK; //得到显示类型
		if (iDrawType!=LVS_REPORT)
			return 0; // 不是REPORT,没有子条目
		SendMessage(hWnd,LVM_GETORIGIN,(WPARAM)0,(LPARAM)&ptOrg); // 得到原点
		x=points.x-(int)ptOrg.x;
		iCurCoordinate=0;
		for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
		{  // 得到子条目索引
			iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
			iCurWidth=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
			iCurCoordinate+=iCurWidth;
			if (points.x<iCurCoordinate)
			{ // 指定点在该子条目
				return iSubItem;
			}
		}
		return -1;
}
// !!! Add End By Jami chen 2002.04.25
/**************************************************
声明：static void SetInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
参数：
	IN hWnd -- 窗口句柄
	IN iRow -- 指定行
	IN iStartColumn -- 开始列的索引
	IN iEndColumn -- 结束列的索引
返回值：无
功能描述：设置指定行的选择条目
引用: 
************************************************/
static void SetInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;

			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			if (iStartColumn<iEndColumn)
			{ // 开始列小于结束列
				i=iStartColumn;
				iEnd=iEndColumn;
			}
			else
			{ // 开始列大于结束列
				i=iEndColumn;
				iEnd=iStartColumn;
			}
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目数
			iItem=iItemNumInLine*iRow+i; // 得到开始条目索引
//      if (iItem>=lpPDAListCtrl->iItemNum)
//        return;
			for (;i<=iEnd;i++)
			{ // 得到要设置为选择的条目
			    if (iItem>=lpPDAListCtrl->iItemNum)
					return;
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构
				if (lpMainItem)
				{ // 设置该条目为选择条目
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
				DrawItem(hWnd,iItem); // 绘制该条目
				iItem++;
			}
}
/**************************************************
声明：static void ClearInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
参数：
	IN hWnd -- 窗口句柄
	IN iRow -- 指定行
	IN iStartColumn -- 开始列的索引
	IN iEndColumn -- 结束列的索引
返回值：无
功能描述：清除指定行中的选择条目
引用: 
************************************************/
static void ClearInvertRow(HWND hWnd,int iRow,int iStartColumn,int iEndColumn)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			// 得到开始条目及结束条目
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
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目数
			iItem=iItemNumInLine*iRow+i;


			for (;i<=iEnd;i++)
			{ // 清除条目选择
				if (iItem>=lpPDAListCtrl->iItemNum)
					return;
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构
				if (lpMainItem)
				{ // 清除该条目的选择属性
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
				DrawItem(hWnd,iItem); // 绘制该条目
				iItem++;
			}
}
/**************************************************
声明：static void SetInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
参数：
	IN hWnd -- 窗口句柄
	IN iColumn -- 指定列
	IN iStartRow -- 开始行的索引
	IN iEndRow -- 结束行的索引
返回值：无
功能描述：设置指定列中的选择条目
引用: 
************************************************/
static void SetInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			// 得到开始行和结束行
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
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目数
			for (;i<=iEnd;i++)
			{ // 设置条目为选择条目
				iItem=i*iItemNumInLine+iColumn; // 得到条目索引
			    if (iItem>=lpPDAListCtrl->iItemNum)
				   continue;
				if (lpPDAListCtrl->lpSubItem[0].plSubItemContent.count>iItem)
				{
					lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构
					if (lpMainItem)
					{ // 设置该条目为选择条目
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
							DrawItem(hWnd,iItem); // 绘制该条目
					}
				}

				
			}
}
/**************************************************
声明：static void ClearInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
参数：
	IN hWnd -- 窗口句柄
	IN iColumn -- 指定列
	IN iStartRow -- 开始行的索引
	IN iEndRow -- 结束行的索引
返回值：无
功能描述：清除指定列中的选择条目
引用: 
************************************************/
static void ClearInvertColumn(HWND hWnd,int iColumn,int iStartRow,int iEndRow)
{
	int iItemNumInLine;
	int i,iEnd;
	int iItem;
	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
	
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
					return ;
			// 得到要清除条目的开始行和结束行
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
			iItemNumInLine=GetItemNumInLine(hWnd); // 得到行条目个数
			for (;i<=iEnd;i++)
			{ // 清除指定条目的选择
				iItem=i*iItemNumInLine+iColumn; // 得到条目索引
				if (iItem>=lpPDAListCtrl->iItemNum)
					continue;
				lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iItem); // 得到条目结构
				if (lpMainItem)
				{ // 清除该条目的选择属性
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
				DrawItem(hWnd,iItem); // 绘制该条目
			}
}

/**************************************************
声明：static void ClearAllInvert(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：清除所有的选择
引用: 
************************************************/
static void ClearAllInvert(HWND hWnd)
{
	LPMAINITEM lpMainItem;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;

				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
				if (lpPDAListCtrl==NULL)
						return ;
				for (i=0;i<lpPDAListCtrl->iItemNum;i++)
				{// 清除所有条目的选择属性
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i); // 得到条目结构
					if (lpMainItem)
					{
						if (lpMainItem->State&LVIS_SELECTED)
						{ // 该条目是选择条目,清除它
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
							DrawItem(hWnd,i); // 绘制该条目
						}
					}
				}
}
/**************************************************
声明：static void ClearFocus(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：清除焦点
引用: 
************************************************/
static void ClearFocus(HWND hWnd)
{
	LPMAINITEM lpMainItem;
	LPPDALISTCTRL lpPDAListCtrl;
	
				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
				if (lpPDAListCtrl==NULL)
						return ;
        if (lpPDAListCtrl->iFocusItem>=0)
        {
				  lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem); // 得到焦点条目结构
				  if (lpMainItem)
				  { // 清楚条目焦点属性
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
					  DrawItem(hWnd,lpPDAListCtrl->iFocusItem); // 绘制该条目
				  }
        }
        lpPDAListCtrl->iFocusItem=-1; // 没有焦点条目
}

/**************************************************
声明：static int GetListFocus(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：成功返回焦点的条目索引，否则返回-1
功能描述：得到焦点条目
引用: 
************************************************/
static int GetListFocus(HWND hWnd)
{
	LPMAINITEM lpMainItem;
	LPPDALISTCTRL lpPDAListCtrl;
	int i;
				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
				if (lpPDAListCtrl==NULL)
						return -1;
				for (i=0;i<lpPDAListCtrl->iItemNum;i++)
				{ // 搜索所有条目中含有焦点的条目
					lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,i); // 得到条目结构
					if (lpMainItem)
					{ 
						if (lpMainItem->State&LVIS_FOCUSED)
						{ // 该条目是焦点条目
							lpPDAListCtrl->iFocusItem=i;
							return i;
						}
					}
				}
				lpPDAListCtrl->iFocusItem=-1; // 没有焦点条目
				return -1;
}

/**************************************************
声明：static void GetDispInfo(HWND hWnd,LPLVITEM lpItem)
参数：
	IN hWnd -- 窗口句柄
	IN/OUT lpItem -- 要得到的条目信息
返回值：无
功能描述：通知父窗口要得到显示信息
引用: 
************************************************/
static void GetDispInfo(HWND hWnd,LPLVITEM lpItem)
{
		NMLVDISPINFO nmLVDispInfo;
		HWND hParent;

//						return ;
		// 设置结构数据		
		nmLVDispInfo.hdr.hwndFrom=hWnd;
		nmLVDispInfo.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		nmLVDispInfo.hdr.code=LVN_GETDISPINFO;
		nmLVDispInfo.item=*lpItem;
		hParent=GetParent(hWnd); // 得到父窗口句柄
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmLVDispInfo); // 发送消息得到条目信息
		*lpItem=nmLVDispInfo.item;
}
/**************************************************
声明：static void DrawHorzListLine(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制水平分隔线
引用: 
************************************************/
static void DrawHorzListLine(HWND hWnd,HDC hdc)
{
  int x0,y0,x1,y1;
	RECT rect;
	int yCoordinate,i;
	int iPageItem,iLineHeight;
		GetClientRect(hWnd,&rect); // 得到窗口矩形
		x0=(int)rect.left;
		x1=(int)rect.right;

		iPageItem=GetPageItemNum(hWnd); // 得到每页的条目个数
		iLineHeight=GetLineHeight(hWnd); // 得到行高度
		if (ExistCaption(hWnd)) // 是否存在标题
			yCoordinate=GetCaptionHeight(hWnd,(LPPDALISTCTRL)NULL); //CAPTIONHEIGHT;
		else
			yCoordinate=0;
		for (i=0;i<iPageItem;i++)
		{   // 绘制每一行的分隔线
			yCoordinate+=iLineHeight;
			y0=y1=yCoordinate-1;
			// draw a line
			MoveToEx(hdc,x0,y0,NULL);
			LineTo(hdc,x1,y1); // 绘制水平线
		}
}
/**************************************************
声明：static void DrawVertListLine(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制垂直分隔线
引用: 
************************************************/
static void DrawVertListLine(HWND hWnd,HDC hdc)
{
  int x0,y0,x1,y1;
	RECT rect;
	LPPDALISTCTRL lpPDAListCtrl;
	int i,iSubItem;
	int xCoordinate;
				lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
				if (lpPDAListCtrl==NULL)
						return ;
				GetClientRect(hWnd,&rect); // 得到客户矩形
				if (ExistCaption(hWnd)) // 是否存在标题
					y0=GetCaptionHeight(hWnd,lpPDAListCtrl); //CAPTIONHEIGHT;
				else
					y0=0;
				y1=(int)rect.bottom;

				xCoordinate=lpPDAListCtrl->cx;

				for (i=0;i<lpPDAListCtrl->iSubItemNum;i++)
				{ // 绘制每一列的分隔线
					// draw a line
					iSubItem=lpPDAListCtrl->lpColumnOrderArray[i];
					xCoordinate+=lpPDAListCtrl->lpSubItem[iSubItem].iSubItemWidth;
					x0=x1=xCoordinate-1;
					MoveToEx(hdc,x0,y0,NULL);
					LineTo(hdc,x1,y1); // 绘制垂直线
				}
}
/**************************************************
声明：static BOOL MoveInvertLine(HWND hWnd,int iMoveLine)
参数：
	IN hWnd -- 窗口句柄
	IN iMoveLine -- 滚动的行数
返回值：成功返回TRUE， 否则返回FALSE
功能描述：滚动选择行
引用: 
************************************************/
static BOOL MoveInvertLine(HWND hWnd,int iMoveLine)
{

 	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
 	int iPageLines,iTotalLines,iCurLine;
    int iNewFocusItem;
    int iScrollLine,iItemNumInLine;

  		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return FALSE;
		if (lpPDAListCtrl->iFocusItem==-1)
			 return FALSE; // 没有焦点
		
 		iPageLines=GetPageLine(hWnd); // 得到每页的行数
		iTotalLines=GetTotalLine(hWnd); // 得到总行数
 		iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行的条目个数

		if (iMoveLine==0)
			return FALSE;
		iCurLine=lpPDAListCtrl->iFocusItem/iItemNumInLine; // 得到当前行
		if (iMoveLine<0)
		{  // to the up
			if ((iCurLine)==0)
				return FALSE;  // already to the top
			if ((iCurLine+iMoveLine)<0)
				iMoveLine=0-iCurLine;
			iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine*iItemNumInLine; // 得到新的焦点条目
		}
		else
		{
			if ((iCurLine+1)==iTotalLines)  // already to the bottom
				return FALSE;
			if ((iCurLine+iMoveLine)>=iTotalLines)
			{ // 得到移动的行数
				iMoveLine=iTotalLines-iCurLine-1;
			}
	      iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine*iItemNumInLine; // 得到新的焦点条目
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
         VScrollWindow(hWnd,iScrollLine); // 垂直滚动窗口
//         iNewFocusItem=lpPDAListCtrl->iStartItem;
      }
 	  if (iNewFocusItem>=(lpPDAListCtrl->iStartItem+iPageLines*iItemNumInLine))
      {        // need scroll down Window
         iScrollLine=(iNewFocusItem-(lpPDAListCtrl->iStartItem+iPageLines*iItemNumInLine))/iItemNumInLine+1;
//         iScrollLine=(iNewFocusItem-lpPDAListCtrl->iStartItem-iPageLines+1);
         VScrollWindow(hWnd,iScrollLine); // 垂直滚动窗口
//         iNewFocusItem=lpPDAListCtrl->iStartItem+iPageLines-1;
      }
			lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iNewFocusItem); // 得到焦点条目结构
			lpPDAListCtrl->bActiveItem=FALSE;
			// clear old Invert
			ClearAllInvert(hWnd); // 清除选择
			ClearFocus(hWnd); // 清除焦点

			if (lpMainItem)
			{ // 设置当前条目焦点,选择属性
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
			DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // 绘制该条目
      return TRUE;
}

/**************************************************
声明：static BOOL MoveInvertColumn(HWND hWnd,int iMoveLine)
参数：
	IN hWnd -- 窗口句柄
	IN iMoveLine -- 摇动行数
返回值：成功返回TRUE， 否则返回FALSE
功能描述：选择条目横向移动
引用: 
************************************************/
static BOOL MoveInvertColumn(HWND hWnd,int iMoveLine)
{

 	LPPDALISTCTRL lpPDAListCtrl;
	LPMAINITEM lpMainItem;
// 	int iPageLines;
  int iNewFocusItem;
  int iItemNumInLine;

  		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return FALSE;
		if (lpPDAListCtrl->iFocusItem==-1)
			 return FALSE;
 		iItemNumInLine=GetItemNumInLine(hWnd); // 得到每行的条目个数
		
		if (iMoveLine==0)
			return FALSE;
		if (iMoveLine<0)
		{  // to the pre 
			if ((lpPDAListCtrl->iFocusItem%iItemNumInLine)==0)
				return FALSE;  // already to the line head
	        iNewFocusItem=lpPDAListCtrl->iFocusItem+iMoveLine; // 得到新的焦点条目
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
		ClearFocus(hWnd); // 清除焦点

		if (lpMainItem)
		{ // 设置当前条目焦点,选择属性
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
		DrawItem(hWnd,lpPDAListCtrl->iActiveItem); // 绘制该条目
      return TRUE;
}

/**************************************************
声明：static int GetItemString(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl,int iIndex,int iSubItem,LPTSTR lpFileFeild)
参数：
	IN hWnd -- 窗口句柄
	IN lpPDAListCtrl -- 控件数据结构
	IN iIndex -- 指定的条目索引
	IN iSubitem -- 指定的子条目索引
	OUT lpFileFeild -- 存放要得到的文本
返回值：返回条目所对应的图象索引
功能描述：得到条目的文本
引用: 
************************************************/
static int GetItemString(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl,int iIndex,int iSubItem,LPTSTR lpFileFeild)
{
	LPMAINITEM lpMainItem;
	LPTSTR lpListString=NULL;
	LVITEM lvItem; 
	int iImage=0;

		lpMainItem=PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,iIndex); // 得到条目结构
		if (lpMainItem==NULL)
			return NULL;
		if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_FILE)
		{ // 是文件属性
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
				GetDispInfo(hWnd,&lvItem); // 得到条目信息
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
					iImage=lvItem.iImage;
		}
		else
		{
			if (iSubItem==0)	
			{// 得到条目文本与图象
				lpListString=lpMainItem->lpText;
				if (lpPDAListCtrl->lpSubItem[iSubItem].ContentFmt&LVCFMT_IMAGE)
					iImage=lpMainItem->iImage;
			}
			else
			{ // 得到条目文本或图象
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
声明：static void SendActiveItem(HWND hWnd,int iItem)
参数：
	IN hWnd -- 窗口句柄
	IN iItem -- 指定条目的索引
返回值：无
功能描述：发送通知消息当前的激活条目
引用: 
************************************************/
static void SendActiveItem(HWND hWnd,int iItem)
{
	POINT pt;

		pt.x=0;
		pt.y=0;
		SendActiveNotify(hWnd,LVN_ITEMACTIVATE,iItem,pt); // 发送条目激活属性
}
/**************************************************
声明：static void SendActiveNotify(HWND hWnd,UINT code,int iItem,POINT pt)
参数：
	IN hWnd -- 窗口句柄
	IN code -- 要发送的通知消息代码
	IN iItem -- 当前条目索引
	IN pt -- 当前鼠标的位置
返回值：无
功能描述：发送一个通知消息给父窗口
引用: 
************************************************/
static void SendActiveNotify(HWND hWnd,UINT code,int iItem,POINT pt)
{
	NMITEMACTIVATE nmia;
	HWND hParent;

		// 设置结构参数
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
		hParent=GetParent(hWnd); // 得到父窗口句柄
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmia); // 通知父窗口
}
/**************************************************
声明：static  void SendNormalNotify(HWND hWnd,UINT iCode,int iItem,int iSubItem,UINT uNewState,UINT uOldState,POINT pt)
参数：
	IN hWnd -- 窗口句柄
	IN code -- 要发送的通知消息代码
	IN iItem -- 当前条目索引
	IN uNewState -- 新的状态
	IN uOldState -- 旧的状态
	IN pt -- 当前鼠标的位置
返回值：无
功能描述：发送一个通用通知消息给父窗口
引用: 
************************************************/
static  void SendNormalNotify(HWND hWnd,UINT iCode,int iItem,int iSubItem,UINT uNewState,UINT uOldState,POINT pt)
{
	HWND hParent;
	NMLISTVIEW nmListView;

		// 设置结构参数
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
		hParent=GetParent(hWnd); // 得到父窗口句柄
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmListView); // 通知父窗口
		return ;
}
/**************************************************
声明：static  void SendBasicNotify(HWND hWnd,UINT iCode)
参数：
	IN hWnd -- 窗口句柄
	IN code -- 要发送的通知消息代码
返回值：无
功能描述：发送一个基本通知消息给父窗口
引用: 
************************************************/
static  void SendBasicNotify(HWND hWnd,UINT iCode)
{
	HWND hParent;
	//NMLISTVIEW nmListView;
	NMHDR hdr;
		// 设置参数句柄
		hdr.hwndFrom=hWnd;
		hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);
		hdr.code=iCode;
		hParent=GetParent(hWnd); // 得到父窗口句柄
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&hdr); // 通知父窗口
		return ;
}

/**************************************************
声明：static void DoReturnKey(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：处理回车键
引用: 
************************************************/
static void DoReturnKey(HWND hWnd)
{
		LPPDALISTCTRL lpPDAListCtrl;
		LPMAINITEM lpMainItem;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
				return ;
		SendBasicNotify(hWnd,NM_RETURN); // 发送通知消息
		if (lpPDAListCtrl->iFocusItem==-1)
			return ;
		lpMainItem=(LPMAINITEM)PtrListAt(&lpPDAListCtrl->lpSubItem[0].plSubItemContent,lpPDAListCtrl->iFocusItem); // 得到条目结构

		if (lpMainItem)
		{
			if ((lpMainItem->State&LVIS_SELECTED)&&lpMainItem->State&LVIS_FOCUSED)
				SendActiveItem(hWnd,lpPDAListCtrl->iFocusItem); // 发送通知消息
		}

}

LRESULT CALLBACK LabelEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoEditKillFocus(HWND hWnd);
/**************************************************
声明：static void CreateLabelEdit(HWND hWnd,int iItem ,LPTSTR lpString,LPRECT lprect)
参数：
	IN hWnd -- 窗口句柄
	IN iItem -- 要修改的条目索引
	IN lpString -- 初始化字串
	IN lprect -- 编辑控件的位置与大小
返回值：无
功能描述：创建修改条目标签的编辑窗口
引用: 
************************************************/
static void CreateLabelEdit(HWND hWnd,int iItem ,LPTSTR lpString,LPRECT lprect)
{
	HWND hEdit;
	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
			return ;
		// 创建编辑窗口
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

		lpPDAListCtrl->DefaultEditProc = (DefWndProc)GetWindowLong(hEdit,GWL_WNDPROC); // 得到窗口过程句柄
		lpPDAListCtrl->iEditItem = iItem;
		lpPDAListCtrl->hEdit = hEdit;
	    SetWindowLong(hEdit,GWL_WNDPROC,(long)LabelEditProc); // 设置新的窗口过程句柄
		SetFocus(hEdit); // 设置焦点
}
/**************************************************
声明：LRESULT CALLBACK LabelEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN message -- 消息
	IN wParam -- 消息参数
	IN lParam -- 消息参数
返回值：返回消息处理结果
功能描述：修改条目标签的编辑窗口的处理过程函数
引用: 
************************************************/
LRESULT CALLBACK LabelEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(GetParent(hWnd),0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
			return 0;
#ifdef _MAPPOINTER
		lpPDAListCtrl = (LPARAM)MapPtrToProcess( (LPVOID)lpPDAListCtrl, GetCallerProcess() );  // 映射指针
#endif
		if (lpPDAListCtrl->DefaultEditProc == NULL)
		{
			Sleep(2000);
			return 0;
		}
		switch (message)
		{
		    case WM_KILLFOCUS:
		       return DoEditKillFocus(hWnd); // 处理杀死焦点
		}
//		return lpPDAListCtrl->DefaultEditProc(hWnd,message,wParam,lParam);
		return CallWindowProc(lpPDAListCtrl->DefaultEditProc,hWnd,message,wParam,lParam);

}
/**************************************************
声明：static LRESULT DoEditKillFocus(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：编辑窗口处理失去焦点消息，WM_KILLFOCUS。
引用: 
************************************************/
static LRESULT DoEditKillFocus(HWND hWnd)
{
	NMLVDISPINFO nmLVDispInfo;
	HWND hParent,hListView;
	LVITEM lvItem;
	TCHAR lpNewFileName[MAX_PATH];
	LPPDALISTCTRL lpPDAListCtrl;



		hListView = GetParent(hWnd); // 得到LISTVIEW 句柄
		hParent = GetParent(hListView); // 得到父窗口句柄

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hListView,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
			return 0;
		

		GetWindowText(hWnd,lpNewFileName,MAX_PATH); // 得到窗口文本

		DestroyWindow(hWnd); // 破坏编辑窗口
		lpPDAListCtrl->DefaultEditProc = NULL;
		lpPDAListCtrl->hEdit = NULL;

		lvItem.mask = LVIF_TEXT;
		lvItem.pszText = lpNewFileName;
		lvItem.iItem = lpPDAListCtrl->iEditItem;
		lvItem.iSubItem = 0;

		lpPDAListCtrl->iEditItem = -1;

		// 设置结构参数
		nmLVDispInfo.hdr.hwndFrom=hWnd;
		nmLVDispInfo.hdr.idFrom=(UINT)GetWindowLong(hListView,GWL_ID);
		nmLVDispInfo.hdr.code=LVN_ENDLABELEDIT;
		nmLVDispInfo.item=lvItem;
		// 得到条目信息
		if (SendMessage(hParent,WM_NOTIFY,(WPARAM)hListView,(LPARAM)&nmLVDispInfo) == FALSE)
			return 0;
		// 设置条目文本
		SendMessage(hListView,LVM_SETITEMTEXT ,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);
		return 0;
}

/**************************************************
声明：static void KillEditControl(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：杀死编辑窗口
引用: 
************************************************/
static void KillEditControl(HWND hWnd)
{
	LPPDALISTCTRL lpPDAListCtrl;

		lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
		if (lpPDAListCtrl==NULL)
			return ;
		if (lpPDAListCtrl->hEdit == NULL)
			return ;
		
		DestroyWindow(lpPDAListCtrl->hEdit); // 破坏窗口
		lpPDAListCtrl->DefaultEditProc = NULL;
		lpPDAListCtrl->iEditItem = -1;
		lpPDAListCtrl->hEdit = NULL;
}

/**************************************************
声明：static int LV_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
参数：
	IN hWnd -- 窗口句柄
	IN nBar -- 是垂直滚动条还是水平滚动条
	IN nPos -- 当前位置
	IN bRedraw -- 重绘标志
返回值：无
功能描述：设置滚动条当前位置。
引用: 
************************************************/
static int LV_SetScrollPos(HWND hWnd, int nBar,int nPos,BOOL bRedraw)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			si.nPos=nPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // 设置滚动条位置
			return 0;
}
/**************************************************
声明：static int LV_GetScrollPos(HWND hWnd, int nBar)
参数：
	IN hWnd -- 窗口句柄
	IN nBar -- 是垂直滚动条还是水平滚动条
返回值：返回当前位置
功能描述：得到滚动条当前位置。
引用: 
************************************************/
static int LV_GetScrollPos(HWND hWnd, int nBar)
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_POS;
			GetScrollInfo(hWnd,nBar,&si); // 得到滚动条位置
			return si.nPos;
}

/**************************************************
声明：static BOOL LV_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
参数：
	IN hWnd -- 窗口句柄
	IN nBar -- 是垂直滚动条还是水平滚动条
	IN nMinPos -- 最小位置
	IN nMaxPos -- 最大位置
	IN bRedraw -- 重绘标志
返回值：无
功能描述：设置滚动条范围。
引用: 
************************************************/
static BOOL LV_SetScrollRange(HWND hWnd,int nBar,int nMinPos,int nMaxPos,BOOL bRedraw )
{
	SCROLLINFO si;

			si.cbSize=sizeof(SCROLLINFO);
			si.fMask=SIF_RANGE;
			si.nMin=nMinPos;
			si.nMax=nMaxPos;
			SetScrollInfo(hWnd,nBar,&si,bRedraw); // 设置滚动条范围
			return TRUE;
}


/**************************************************
声明：static int GetTextHeight(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回行高
功能描述：得到行高
引用:  Add By Jami chen in 2004.05.21
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
声明：static int GetTextWidth(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回文本宽度,一个英文字符的宽度
功能描述：得到返回文本宽度,一个英文字符的宽度.
引用:  Add By Jami chen in 2004.05.21
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
声明：static int GetCaptionHeight(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl)
参数：
	IN hWnd -- 窗口句柄
	IN lpPDAListCtrl -- LISTCTRL的数据结构
返回值：返回文本宽度,一个英文字符的宽度
功能描述：得到返回文本宽度,一个英文字符的宽度.
引用:  Add By Jami chen in 2004.05.21
************************************************/
static int GetCaptionHeight(HWND hWnd,LPPDALISTCTRL lpPDAListCtrl)
{

		if (lpPDAListCtrl == NULL)
		{ // 如果还没有得到结构
			lpPDAListCtrl=(LPPDALISTCTRL)GetWindowLong(hWnd,0); // 得到LISTCTRL的数据结构
			if (lpPDAListCtrl==NULL)
				return 28;
		}

		
		return lpPDAListCtrl->iTextHeight + 4;//TextMetric.tmAveCharWidth;

}
