/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：多条目编辑类
版本号：1.0.0.456
开发时期：2001-12-26
作者：Jami chen 陈建明
修改记录：
  2004.07.19 增加消息功能WM_SYSCOLORCHANGE, 用于在在系统颜色重新
			 设置后，需要重新装载新的颜色
**************************************************/
//#include "StdAfx.h"
#include "eframe.h"
#include "eWindows.h"
//#include "Resource.h"
#include <estring.h>
//#include <memory.h>
//#include <CTYPE.h>
//#include "Show.h"
#include "eedit.h"
#include "eebase.h"
//#include <gwmesrv.h>
#include <eobjcall.h>


//#define _MAPPOINTER

// define const
#define MULTIITEMEDIT classMULTIEDIT
static const char classMULTIEDIT[]={"MultiItemEdit"};

// This function for debug
//void ErrorDisplay(LPCTSTR lpCaption);

// function declare area
LRESULT CALLBACK MultiItemEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
ATOM RegisterMultiItemEditClass(HINSTANCE hInstance);

extern int GetTextHeight(HWND hWnd);


static void ReleaseMultiItemEdit(HWND hWnd);
//static void DoSizeChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ReSetItemPos(HWND hWnd,int iChangedSequence,int nChangedLine,BOOL bRedraw);
static void ClearBottomBlankArea(HWND hWnd);
static int GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem);
static int VScrollMultiItemEdit(HWND hWnd,int nScrollLine);
static void HScrollMultiItemEdit(HWND hWnd,int xScrollPos);
static BOOL AdjustCaretPosition(HWND hWnd);
static void SetMIEditVScrollRange(HWND hWnd,int nTotalLine,int nWindowLine);
static void SetMIEditScrollPage(HWND hWnd);
static void SetMIEditVScrollPos(HWND hWnd,int nNewScrollPos);
static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static int GetPageLine(HWND hWnd);
static void SetMIEditHScrollPos(HWND hWnd,int nNewScrollPos);
//static void DoDisplayChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem);
//static BOOL MoveCaret(HWND hWnd,int nScrollLine);
static BOOL MoveCaret(HWND hWnd,int nScrollLine,int iHorzPos);

static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,int iItemStartPos);

static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT InitialMultiItemEdit(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoNoticeSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetFirstVisibleLine(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam);
static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoSetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetItemTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoGetActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoGetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam);

/**************************************************
声明：ATOM RegisterMultiItemEditClass(HINSTANCE hInstance)
参数：
	IN hInstance -- 实例句柄
返回值：返回系统注册结果
功能描述：注册多条目编辑类
引用: 
************************************************/
ATOM RegisterMultiItemEditClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	// size of the struct 'WNDCLASSEX'
//	wcex.cbSize = sizeof(WNDCLASSEX);

	// the class propertiy
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	// this class window procedure
	wcex.lpfnWndProc	= (WNDPROC)MultiItemEditProc;
	// no class extra
	wcex.cbClsExtra		= 0;
	// window extra 4 byte to save address
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	// no icon
	wcex.hIcon			= NULL;
	// I-Beam Cursor
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  // no background
  wcex.hbrBackground	= 0;
	// no menu
	wcex.lpszMenuName	= NULL; // no menu ,add to future
	// class name
	wcex.lpszClassName	= MULTIITEMEDIT;
	// no small icon
//	wcex.hIconSm		= NULL;

	// register this class
	return RegisterClass(&wcex);
}

/**************************************************
声明：LRESULT CALLBACK MultiItemEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN uMsg -- 要处理的消息
	IN wParam -- 消息参数
	IN lParam -- 消息参数
返回值：消息处理结果
功能描述：处理多条目编辑类消息。
引用: 
************************************************/
LRESULT CALLBACK MultiItemEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
 
	switch (message)
	{
		case WM_CREATE:  // 创建窗口
			// Create a window use this class
		  return InitialMultiItemEdit(hWnd,wParam,lParam);
		case WM_COMMAND:  // 命令消息
			break;
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
//			ClearBottomBlankArea(hWnd);
  	      DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
	    case WM_SIZE:  // 改变窗口大小
 	      return DoSize(hWnd,wParam,lParam);
		case WM_WINDOWPOSCHANGED:
			return DoWindowPosChanged(hWnd,wParam,lParam);
		case WM_SETFOCUS: // 设置焦点
		  return DoSetFocus(hWnd,wParam,lParam);
		case WM_KILLFOCUS: // 杀死焦点
		  return DoKillFocus(hWnd,wParam,lParam);
			case WM_LBUTTONDOWN:  // 鼠标左键按下
		  return DoLButtonDown(hWnd,wParam,lParam);
		case WM_MOUSEMOVE: // 鼠标移动
		  return DoMouseMove(hWnd,wParam,lParam);
		case WM_LBUTTONUP: // 鼠标左键弹起
		  return DoLButtonUp(hWnd,wParam,lParam);
		case WM_CHAR: // 字符输入
		  return DoProcessChar(hWnd,wParam,lParam);
		case WM_KEYDOWN: // 键盘消息
		  return DoKeyDown(hWnd,wParam,lParam);
		case WM_DESTROY: // 破坏窗口
				// close this window
				ReleaseMultiItemEdit(hWnd);
				break;
		case WM_VSCROLL:  // 垂直滚动
		  DoVScrollWindow(hWnd,wParam,lParam);
		  break;
		case WM_HSCROLL:  // 水平滚动
		  DoHScrollWindow(hWnd,wParam,lParam);
		  break;
		case WM_PRINTCLIENT: // 打印客户区数据
		  DoPrintClient(hWnd,wParam,lParam);
		  break;

// !!! Add By Jami chen in 2004.07.19
		case WM_SYSCOLORCHANGE:
			return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.07.19

		case EM_CHARFROMPOS:  // 得到指定点的字符
			return DoCharFromPos(hWnd,wParam,lParam);
		case EM_GETFIRSTVISIBLELINE:  // 得到第一个可见行的索引
			return DoGetFirstVisibleLine(hWnd,wParam,lParam);
		case EM_GETLIMITTEXT: // 得到指定条目的文本限制
			return DoGetLimitText(hWnd,wParam,lParam);
		case EM_GETLINE: // 得到指定行的数据
			return DoGetLine(hWnd,wParam,lParam);
		case EM_GETLINECOUNT: // 得到控件的总行数
			return DoGetLineCount(hWnd,wParam,lParam);
		case EM_GETMODIFY: // 返回修改标志
			return DoGetModify(hWnd,wParam,lParam);
		case EM_GETSEL: // 得到编辑区的选择区域
			return DoGetSel(hWnd,wParam,lParam);
		case EM_GETPASSWORDCHAR: // 得到条目的密码字符
			return DoGetPasswordChar(hWnd,wParam,lParam);
		case EM_GETTHUMB: // 得到垂直滚动条滚动块的位置
			return DoGetThumb(hWnd,wParam,lParam);
		case EM_LINEFROMCHAR: // 返回指定条目的指定索引字符的行索引
			return DoLineFromChar(hWnd,wParam,lParam);
		case EM_LINEINDEX: // 得到指定条目指定行索引所对应的字符索引
				return DoLineIndex(hWnd,wParam,lParam);
		case EM_LINELENGTH: // 得到指定条目指定字符所在行的字符个数
				return DoLinelength(hWnd,wParam,lParam);
		case EM_LINESCROLL: // 滚动窗口
				return DoLineScroll(hWnd,wParam,lParam);
		case EM_POSFROMCHAR: // 得到指定条目指定字符的坐标
			return DoPosFromChar(hWnd,wParam,lParam);
		case EM_REPLACESEL: // 用指定的字符串替换选择字符
			return DoReplaceSel(hWnd,wParam,lParam);
		case EM_SCROLL: // 垂直滚动屏幕
			return DoScroll(hWnd,wParam,lParam);
		case EM_SCROLLCARET: // 滚动光标到可见位置
			return DoScrollCaret(hWnd,wParam,lParam);
		case EM_SETLIMITTEXT: // 设置指定条目的文本限制
			return DoSetLimitText(hWnd,wParam,lParam);
		case EM_SETMODIFY: // 设置指定条目新的修改标志
			return DoSetModify(hWnd,wParam,lParam);
		case EM_SETPASSWORDCHAR: // 设置指定条目新的修改标志
			return DoSetPasswordChar(hWnd,wParam,lParam);
		case EM_SETREADONLY: // 设置指定条目编辑区的只读属性
			return DoSetReadOnly(hWnd,wParam,lParam);
		case EM_SETSEL:  // 设置选择区域
			return DoSetSel(hWnd,wParam,lParam);
		case EM_EMPTY: // 清空编辑区
			return DoEmpty(hWnd,wParam,lParam);
// new message
		case EM_INSERTITEM: // 在指定条目下插入一个条目
			return DoInsertItem(hWnd,wParam,lParam);
		case EM_SETITEM:  // 设置指定条目的文本
			return DoSetItem(hWnd,wParam,lParam);
		case EM_DELETEITEM: // 删除指定条目
			return DoDeleteItem(hWnd,wParam,lParam);
		case WM_COPY: // 拷贝选择区域字符到剪切板
			return DoCopy(hWnd,wParam,lParam);
		case WM_PASTE:  // 粘帖
			return DoPaste(hWnd,wParam,lParam);
		case WM_CUT:  // 剪切
			return DoCut(hWnd,wParam,lParam);
		case WM_CLEAR:  // 清除
			return DoClear(hWnd,wParam,lParam);
		case WM_SETTEXT:  // 设置激活编辑区文本
			return DoSetText(hWnd,wParam,lParam);
		case WM_GETTEXT:  // 得到激活编辑区的文本
			return DoGetText(hWnd,wParam,lParam);
	    case WM_GETTEXTLENGTH:  // 得到激活编辑区的文本长度
			 return DoGetTextLength(hWnd , wParam ,lParam);

		
		case EM_GETITEMTEXT:  // 得到指定条目编辑区的文本
			return DoGetItemText(hWnd,wParam,lParam);
		case EM_SETITEMTEXT:  // 设置指定条目的文本
			return DoSetItemText(hWnd,wParam,lParam);
	    case EM_GETITEMTEXTLENGTH: // 得到指定条目编辑区的文本长度
			 return DoGetItemTextLength(hWnd , wParam ,lParam);
		case EM_SETITEMCALLBACK: // 设置指定条目的回调函数指针
			 return DoSetItemCallBack(hWnd,wParam,lParam);

		case EM_GETACTIVEITEM:  // 得到激活条目的索引
			return DoGetActiveItem(hWnd,wParam,lParam);
//		case EM_SETCOLOR:
//			return DoSetColor(hWnd,wParam,lParam);
//		case EM_GETCOLOR:
//			return DoGetColor(hWnd,wParam,lParam);
		case WM_SETCTLCOLOR	: // 设置窗口颜色
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	:  // 得到当前窗口的颜色
			return DoGetColor(hWnd,wParam,lParam);
		case EM_GETITEMSTYLE:  // 得到指定条目的风格
			return DoGetItemStyle(hWnd,wParam,lParam);
		case EM_SETITEMSTYLE: // 设定指定条目的风格
			return DoSetItemStyle(hWnd,wParam,lParam);

    case EM_GETCARETPOS:  // 得到光标位置
	      DoGetCaretPosition(hWnd,wParam,lParam);
		  break;
	//2005-09-20, add for WS_GROUP
	case DLGC_WANTALLKEYS:
		return DLGC_WANTALLKEYS;
	//
	default:
			// default procedure
		return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
/**************************************************
声明：static LRESULT InitialPDAEdit(HWND hWnd,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN lParam -- 指向CREATESTRUCT的指针，可以为NULL。
返回值：成功返回0，否则返回-1。
功能描述：创建多条目编辑控件
引用: 
************************************************/
static LRESULT InitialMultiItemEdit(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	PDAEDITPROPERTY PDAEditProperty;
  RECT ClientRect;
  int iNextWndPos=0,nTextHeigth;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nClientLine;
	LPCREATESTRUCT lpcs;
  LPMULTIITEMEDITPROPERTY lpMultiItemEditProperty;
  LPEDITITEMPROPERTY lpEditItemProperty;
  int i;
  LPEDITITEM lpEditItem;
 // int nFormatLen;
  DWORD dwStyle;

	
#ifdef _MAPPOINTER
	  lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif

	  lpcs = (LPCREATESTRUCT) lParam; // structure with creation data
	  lpMultiItemEditProperty=(LPMULTIITEMEDITPROPERTY)lpcs->lpCreateParams; // 得到创建结构指针

// !!! Add By Jami chen in 2003.07.31
#ifdef _MAPPOINTER
	  if (lpMultiItemEditProperty)
			lpEditItemProperty = (LPEDITITEMPROPERTY)MapPtrToProcess( (LPVOID)lpMultiItemEditProperty->lpEditItemProperty, GetCallerProcess() ); 
#else
	  if (lpMultiItemEditProperty)
			lpEditItemProperty = lpMultiItemEditProperty->lpEditItemProperty;
#endif
// !!! Add End By Jami chen in 2003.07.31

      lpMultiItemEditStruct=malloc(sizeof(MULTIITEMEDITSTRUCT));  // 分配空间
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      dwStyle|=ES_MULTIITEMEDIT|ES_LINESEPARATE;
      SetWindowLong(hWnd,GWL_STYLE,dwStyle);
      if (lpMultiItemEditStruct==NULL)
			{ // 分配失败
				//SendNotificationMsg(hWnd,EN_ERRSPACE);
				return -1;
			}
      if (lpMultiItemEditProperty==NULL)
      { // 没有初始化条目
        lpMultiItemEditStruct->nItemNum=0;
      }
      else
      { // 设定初始化条目个数
        lpMultiItemEditStruct->nItemNum=lpMultiItemEditProperty->nItemNum;
      }
/*
      lpMultiItemEditStruct->hItemWndList=malloc((lpMultiItemEditProperty->nItemNum)*sizeof(HWND));
      if (lpMultiItemEditStruct->hItemWndList==NULL)
      {
        free(lpMultiItemEditStruct);
				SendNotificationMsg(hWnd,EN_ERRSPACE);
        return -1;
      }
*/
      if (lpMultiItemEditStruct->nItemNum)
      {
        lpMultiItemEditStruct->lpItemList=malloc((lpMultiItemEditStruct->nItemNum)*sizeof(EDITITEM)); // 分配条目内存
        if (lpMultiItemEditStruct->lpItemList==NULL)
        {
          free(lpMultiItemEditStruct);
				  //SendNotificationMsg(hWnd,EN_ERRSPACE);
          return -1;
        }
      }
      else
        lpMultiItemEditStruct->lpItemList=NULL;
      lpMultiItemEditStruct->nDisplayLine=0;
      lpMultiItemEditStruct->xDisplayPos=0;
      lpMultiItemEditStruct->nItemStartLineList=malloc((lpMultiItemEditStruct->nItemNum+1)*sizeof(int)); // 分配条目开始位置列表
      if (lpMultiItemEditStruct->nItemStartLineList==NULL)
      {
        free(lpMultiItemEditStruct->lpItemList);
        free(lpMultiItemEditStruct);
				//SendNotificationMsg(hWnd,EN_ERRSPACE);
        return -1;
      }
//      lpMultiItemEditStruct->nItemNum=lpMultiItemEditProperty->nItemNum;


      nTextHeigth=GetTextHeight(hWnd); // 得到文本高度
      GetClientRect(hWnd,&ClientRect);
      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      { // 初始化条目
          lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
// !!! Modified By Jami chen in 2003.07.31
//          if (SetEditItem(hWnd,lpEditItem,&lpMultiItemEditProperty->lpEditItemProperty[i],iNextWndPos)==FALSE)
          if (SetEditItem(hWnd,lpEditItem,&lpEditItemProperty[i],iNextWndPos)==FALSE)
// !!! Modified End By Jami chen in 2003.07.31
          { // 设置条目内容失败
            for (i--;i>=0;i--)
            { // 清除其他条目，及内存空间
                lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
                if (lpEditItem->lpPDAControlBuffer)
                  free(lpEditItem->lpPDAControlBuffer);
                if (lpEditItem->lpPDASaveEditBuffer)
                  free(lpEditItem->lpPDASaveEditBuffer);
                if (lpEditItem->lpFormat)
                  free(lpEditItem->lpFormat);
            }
            free(lpMultiItemEditStruct->nItemStartLineList);
            free(lpMultiItemEditStruct->lpItemList);
            free(lpMultiItemEditStruct);
            //SendNotificationMsg(hWnd,EN_ERRSPACE);
            return -1;
          }
/*
          {
//             if (lpPDAEditProperty!=NULL)
             {
	             // get the text limit
	             if ((lpMultiItemEditProperty->lpEditItemProperty[i]).cLimited!=0)
		             lpEditItem->cbEditLimitLen=(lpMultiItemEditProperty->lpEditItemProperty[i]).cLimited;
	             else
		             lpEditItem->cbEditLimitLen=INITIALEDITSIZE;

	             // get the edit title text
	             if ((lpMultiItemEditProperty->lpEditItemProperty[i]).lpTitle!=NULL)
		             lpEditItem->cbTitleLen=strlen((lpMultiItemEditProperty->lpEditItemProperty[i]).lpTitle);
	             else
		             lpEditItem->cbTitleLen=0;
             }

             lpEditItem->cbEditBufferLen=lpEditItem->cbEditLimitLen+1;

             lpEditItem->cbControlBufferLen=lpEditItem->cbEditBufferLen+lpEditItem->cbTitleLen;
             // allocate memory
             lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen);

             if (lpEditItem->lpPDAControlBuffer==NULL)
             {  // allocate failare then free struct EDITITEM and return FALSe
	             free(lpEditItem);
	             lpEditItem->cbEditBufferLen=0;
	             lpEditItem->cbControlBufferLen=0;
	             SendNotificationMsg(hWnd,EN_ERRSPACE);
	             return -1;
             }

             lpEditItem->lpPDASaveEditBuffer=(LPTSTR)malloc(lpEditItem->cbEditBufferLen);
             if (lpEditItem->lpPDASaveEditBuffer==NULL)
             {  // allocate failare then free struct EDITITEM and return FALSe
	             free(lpEditItem->lpPDAControlBuffer);
	             free(lpEditItem);
	             lpEditItem->cbEditBufferLen=0;
	             lpEditItem->cbControlBufferLen=0;
	             SendNotificationMsg(hWnd,EN_ERRSPACE);
	             return -1;
             }

             lpEditItem->lpPDAEditBuffer=lpEditItem->lpPDAControlBuffer+lpEditItem->cbTitleLen;
             lpEditItem->dwStyle=((lpMultiItemEditProperty->lpEditItemProperty[i]).dwItemStyle|ES_AUTOEXTEND|ES_MULTIITEMEDIT);

             // clear struct EDITITEM
//             Clear(lpEditItem);
             ClearEditItemStruct(lpEditItem);

             // read control title text
             if (lpEditItem->cbTitleLen)
	             memcpy(lpEditItem->lpPDAControlBuffer,(lpMultiItemEditProperty->lpEditItemProperty[i]).lpTitle, lpEditItem->cbTitleLen );		// Read Text Success

             // read Window text
             if ((lpMultiItemEditProperty->lpEditItemProperty[i]).lpItemText!=NULL)
             {
	             lpEditItem->cbEditLen=strlen((lpMultiItemEditProperty->lpEditItemProperty[i]).lpItemText);
	             // the window text can put the default buffer ??
	             if (lpEditItem->cbEditLen>lpEditItem->cbEditLimitLen)
	             {
		             lpEditItem->cbEditLen=0;
		             SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1));
	             }
	             if (lpEditItem->dwStyle&ES_PASSWORD)
	             {
		             lpEditItem->dwStyle&=~ES_MULTILINE;
//		             SetWindowLong(hWnd,GWL_STYLE,lpEditItem->dwStyle);
		             // read Window text
		             FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
	             }
	             else
	             {
		             GetEditText(lpEditItem->lpPDAEditBuffer,(lpMultiItemEditProperty->lpEditItemProperty[i]).lpItemText,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
	             }
	             SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpcs->lpszName,(int)(lpEditItem->cbEditLen+1));
             }
             else
             {
	             lpEditItem->cbEditLen=0;
	             SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1));
             }
             // Read Text Success

             // get edit format
             lpEditItem->lpFormat=NULL;
             if (lpEditItem->dwStyle&ES_FORMAT)
             {
	             if ((lpMultiItemEditProperty->lpEditItemProperty[i]).lpFormat!=NULL)
	             {
		             lpEditItem->dwStyle&=~ES_MULTILINE;
//		             SetWindowLong(hWnd,GWL_STYLE,lpEditItem->dwStyle);
		             nFormatLen=strlen((lpMultiItemEditProperty->lpEditItemProperty[i]).lpFormat)+1;
		             if (nFormatLen)
		             {
			             lpEditItem->lpFormat=(LPTSTR)malloc(nFormatLen);
			             if (lpEditItem->lpFormat)
				             memcpy(lpEditItem->lpFormat,(lpMultiItemEditProperty->lpEditItemProperty[i]).lpFormat, nFormatLen );		// Read Text Success
			             else
				             SendNotificationMsg(hWnd,EN_ERRSPACE);
		             }
	             }
	             else
	             {
		             lpEditItem->dwStyle&=~ES_FORMAT;
//		             SetWindowLong(hWnd,GWL_STYLE,lpEditItem->dwStyle);
	             }
             }
//             lpEditItem->nTotalLine=GetTotalLine(hWnd,lpEditItem);
             lpEditItem->iStartPos=iNextWndPos;
             lpEditItem->rect.top=0;
             lpEditItem->rect.left=0;
             lpEditItem->rect.right=ClientRect.right;
             lpEditItem->nTotalLine=GetTotalLine(hWnd,lpEditItem);
             lpEditItem->rect.bottom=lpEditItem->rect.top+lpEditItem->nTotalLine*nTextHeigth;
//             SetWindowPos(hWnd,0,0,0,ClientRect.right,ClientRect.top+nTextHeight*nTotalLine,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOZORDER);

          }
          */
//          GetClientRect(hItemWnd,&rect);
//          lpMultiItemEditStruct->hItemWndList[i]=hItemWnd;
          lpMultiItemEditStruct->nItemStartLineList[i]=iNextWndPos/nTextHeigth;
//          lpMultiItemEditStruct->nItemStartPosList[i]=iNextWndPos;
          iNextWndPos+=(int)lpEditItem->rect.bottom;  // 下一个条目的开始位置
      }

      lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iNextWndPos/nTextHeigth;
      lpMultiItemEditStruct->nContextBottomPos=iNextWndPos;

	  /*
	  lpMultiItemEditStruct->cl_NormalText = CL_BLACK;
	  lpMultiItemEditStruct->cl_NormalBkColor = CL_WHITE;

	  lpMultiItemEditStruct->cl_InvertText = CL_WHITE;
	  lpMultiItemEditStruct->cl_InvertBkColor = CL_BLACK;

	  lpMultiItemEditStruct->cl_DisableText = CL_LIGHTGRAY;
	  lpMultiItemEditStruct->cl_DisableBkColor = CL_WHITE;
	  */
      // 初始化显示颜色
	  lpMultiItemEditStruct->cl_NormalText = GetSysColor(COLOR_WINDOWTEXT);
	  lpMultiItemEditStruct->cl_NormalBkColor = GetSysColor(COLOR_WINDOW);

	  lpMultiItemEditStruct->cl_InvertText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	  lpMultiItemEditStruct->cl_InvertBkColor = GetSysColor(COLOR_HIGHLIGHT);

	  lpMultiItemEditStruct->cl_DisableText = GetSysColor(COLOR_GRAYTEXT);
	  lpMultiItemEditStruct->cl_DisableBkColor = GetSysColor(COLOR_WINDOW);

	  lpMultiItemEditStruct->cl_ReadOnly = lpMultiItemEditStruct->cl_NormalText;
	  lpMultiItemEditStruct->cl_ReadOnlyBk = lpMultiItemEditStruct->cl_NormalBkColor;

	  lpMultiItemEditStruct->cl_Title = lpMultiItemEditStruct->cl_NormalText;
	  lpMultiItemEditStruct->cl_TitleBk = lpMultiItemEditStruct->cl_NormalBkColor;

      SetWindowLong(hWnd,0,(LONG)lpMultiItemEditStruct);
      nClientLine=ClientRect.bottom/nTextHeigth;
//      SetMIEditScrollPage(hWnd);
      SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);

			lpMultiItemEditStruct->iActiveItem=0;
	  SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // 统治父窗口
//      AdjustCaretPosition(hWnd);
      return 0;
}
/**************************************************
声明：static void ReleaseMultiItemEdit(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：释放窗口资源。
引用: 
************************************************/
static void ReleaseMultiItemEdit(HWND hWnd)
{
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
    LPEDITITEM lpEditItem;
    int nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return;
			if (lpMultiItemEditStruct->nItemStartLineList)
				free(lpMultiItemEditStruct->nItemStartLineList); // 释放开始数据列表

      for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
      {  // 释放条目数据
         lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

			   if (lpEditItem->lpPDASaveEditBuffer)
				   free(lpEditItem->lpPDASaveEditBuffer);
			   if (lpEditItem->lpPDAControlBuffer)
				   free(lpEditItem->lpPDAControlBuffer);
			   if (lpEditItem->lpFormat)
				   free(lpEditItem->lpFormat);
      }

			if (lpMultiItemEditStruct->lpItemList)
				free(lpMultiItemEditStruct->lpItemList); // 释放条目列表
			free(lpMultiItemEditStruct);

	    DeletePDACaret(hWnd);  // 删除光标
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);  // 发送通知消息
			return;
}
/*static void DoSizeChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    HWND hChangedWnd;
    int nChangedLine,nCaretLine;
    int iActiveItem;
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return;

      hChangedWnd=(HWND)wParam;
      nChangedLine=(int)HIWORD(lParam);
      nCaretLine=(int)LOWORD(lParam);
//!!      iActiveItem=GetItemSequence(hWnd,hChangedWnd);
      ReSetItemPos(hWnd,iActiveItem,nChangedLine);
      nCaretLine+=lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
      AdjustCaretPosition(hWnd);
} */
/**************************************************
声明：void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,int nChangedLine,BOOL bRedraw)
参数：
	IN hWnd -- 窗口句柄
	IN lpEditItem --要改变的编辑条目的编辑结构指针
	IN nChangedLine -- 改变大小
	IN bRedraw -- 重绘标志
返回值：无
功能描述：改变一个编辑条目的大小
引用: 
************************************************/
void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,int nChangedLine,BOOL bRedraw)
{
  int iActiveItem;

      iActiveItem=GetItemSequence(hWnd,lpEditItem);
      ReSetItemPos(hWnd,iActiveItem,nChangedLine,bRedraw); // 重设条目位置
}
/**************************************************
声明：static void ReSetItemPos(HWND hWnd,int iChangedSequence,int nChangedLine,BOOL bRedraw)
参数：
	IN hWnd -- 窗口句柄
	IN iChangedSequence -- 改变条目的次序
	IN nChangedLine -- 改变大小
	IN bRedraw -- 重绘标志
返回值：无
功能描述：重设各个编辑条目的位置。
引用: 
************************************************/
static void ReSetItemPos(HWND hWnd,int iChangedSequence,int nChangedLine,BOOL bRedraw)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;
  int nTextHeight,nNewStartPos;
  int nClientLine;
  RECT rcClientRect;
  HDC hdc;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return;
      nTextHeight=GetTextHeight(hWnd);  // 得到文本高度
      i=iChangedSequence+1;
      hdc=GetDC(hWnd); // 得到设备句柄
      for (;i<lpMultiItemEditStruct->nItemNum;i++)
      { // 显示条目
        lpMultiItemEditStruct->nItemStartLineList[i]+=nChangedLine;
        nNewStartPos=(lpMultiItemEditStruct->nItemStartLineList[i]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
        lpMultiItemEditStruct->lpItemList[i].iStartPos=nNewStartPos;
        if (bRedraw)
         DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[i],PE_SHOWCLIENT);
      }
      ReleaseDC(hWnd,hdc); // 释放条目
      lpMultiItemEditStruct->nItemStartLineList[i]+=nChangedLine;
      lpMultiItemEditStruct->nContextBottomPos+=nChangedLine*nTextHeight;
      if (bRedraw)
        ClearBottomBlankArea(hWnd); // 清除底部空白

      GetClientRect(hWnd,&rcClientRect); // 得到客户矩形
      nClientLine=rcClientRect.bottom/nTextHeight;
      SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
}

/**************************************************
声明：static void ClearBottomBlankArea(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：清除窗口底部没有数据的区域。
引用: 
************************************************/
static void ClearBottomBlankArea(HWND hWnd)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  RECT rcClientRect,rcClearRect;
  HDC hdc;
	int nBottomPos;
	HBRUSH hBrush;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return;
		nBottomPos=lpMultiItemEditStruct->nContextBottomPos;
		GetClientRect(hWnd,&rcClientRect);
    if (nBottomPos<rcClientRect.bottom)
    { // 需要清除底部空白
	    rcClearRect.left=0;
	    rcClearRect.top=nBottomPos;
	    rcClearRect.right=rcClientRect.right;
	    rcClearRect.bottom =rcClientRect.bottom;
	    // clear ClearRect
	    hdc=GetDC(hWnd);
//	    FillRect(hdc,&rcClearRect,(HBRUSH)(COLOR_WINDOWFRAME));
//	    FillRect(hdc,&rcClearRect,GetStockObject(WHITE_BRUSH));
		hBrush = GetBkBrush(hWnd,NULL);  // 得到背景画刷
	    FillRect(hdc,&rcClearRect,hBrush);  // 清除画面
		DeleteObject(hBrush);
	    ReleaseDC(hWnd,hdc);
    }
}
/*static int GetTextHeight(HWND hWnd)
{
	HDC hdc;
	TEXTMETRIC TextMetric;
		hdc=GetWindowDC(hWnd);
		GetTextMetrics(hdc,&TextMetric);
    ReleaseDC(hWnd,hdc);
		return TextMetric.tmHeight;
}*/
/*
static BOOL DoMoveCaret(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
  HWND hActiveItemWnd;
  int nCaretLine,xPos,nItemCaretLine;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nPageLine;
  int iActiveItem;
//  RECT rcClientRect;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;

    hActiveItemWnd=(HWND)wParam;
    xPos=(int)LOWORD(lParam);
    nItemCaretLine=(int)HIWORD(lParam);

//!!    iActiveItem=GetItemSequence(hWnd,hActiveItemWnd);
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+nItemCaretLine;

    nPageLine=GetPageLine(hWnd);
    switch(message)
    {
      case EN_DOWNPAGE:
        if (nCaretLine==lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1)
          return FALSE;
        nCaretLine+=nPageLine;
        break;
      case EN_UPPAGE:
        if(nCaretLine==0) return FALSE;
        nCaretLine-=nPageLine;
        break;
      case EN_DOWNLINE:
        if (nCaretLine==lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1)
          return FALSE;
        nCaretLine++;
        break;
      case EN_UPLINE:
        if(nCaretLine==0) return FALSE;
        nCaretLine--;
        break;
        // not process
      case EN_END:
      case EN_HOME:
      case EN_LEFT:
      case EN_RIGHT:
        return  FALSE;
    }
    AdjustCaretPosition(hWnd);

    if (nCaretLine<0) nCaretLine=0;
    if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
      nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1;

// get new active item
    for (iActiveItem=0;iActiveItem<lpMultiItemEditStruct->nItemNum;iActiveItem++)
    {
      if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[iActiveItem]&&
        nCaretLine<lpMultiItemEditStruct->nItemStartLineList[iActiveItem+1])
        break;
    }
    // set active item is last item
    if (iActiveItem>=lpMultiItemEditStruct->nItemNum) iActiveItem=lpMultiItemEditStruct->nItemNum-1;
    nItemCaretLine=nCaretLine-lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,nItemCaretLine,xPos);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
    return TRUE;
}
*/
/**************************************************
声明：static int GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEditItem -- 指定编辑条目的结构指针
返回值：返回指定编辑条目的次序
功能描述：得到指定编辑条目的次序。
引用: 
************************************************/
static int GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return 0;
      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {  // 判断条目是否是指定条目
//        if (hItemWnd==lpMultiItemEditStruct->hItemWndList[i]) break;
          if (lpEditItem==&(lpMultiItemEditStruct->lpItemList[i])) break;
      }
      return i;
}
/**************************************************
声明：static int VScrollMultiItemEdit(HWND hWnd,int nScrollLine)
参数：
	IN hWnd -- 窗口句柄
	IN nScrollLine -- 滚动的行数
返回值：返回实际滚动的行数
功能描述：垂直滚动控件。
引用: 
************************************************/
static int VScrollMultiItemEdit(HWND hWnd,int nScrollLine)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i,iRealScrollLines;
  int nTextHeight,nNewStartPos;
  int nPageLine,nOldDisplayLine;
//	RECT rcRedraw;
//  LPEDITITEM lpActiveEditItem;



      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return 0;
      nPageLine=GetPageLine(hWnd); // 得到一页的行数
      if (nScrollLine==0) return 0;
      if (nScrollLine>0)
      {  // 往下滚
        if ((lpMultiItemEditStruct->nDisplayLine+nPageLine)>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
          return 0;
      }
      if (nScrollLine<0)
      { // 往上滚
        if (lpMultiItemEditStruct->nDisplayLine==0)
          return 0;
      }
			nOldDisplayLine=lpMultiItemEditStruct->nDisplayLine;  // 显示行
      lpMultiItemEditStruct->nDisplayLine+=nScrollLine; // 设定新的显示行
      nPageLine=GetPageLine(hWnd);
      if ((lpMultiItemEditStruct->nDisplayLine+nPageLine)>lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
        lpMultiItemEditStruct->nDisplayLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-nPageLine;
      if (lpMultiItemEditStruct->nDisplayLine<0)
        lpMultiItemEditStruct->nDisplayLine=0;
      nTextHeight=GetTextHeight(hWnd); // 得到文本高度

			iRealScrollLines=(lpMultiItemEditStruct->nDisplayLine-nOldDisplayLine);  // 得到实际滚动行数
	//		GetClientRect(hWnd,&rcRedraw);

      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {  // 设定条目的新的开始位置
        nNewStartPos=(lpMultiItemEditStruct->nItemStartLineList[i]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
        lpMultiItemEditStruct->lpItemList[i].iStartPos=nNewStartPos;
//        SetWindowPos(lpMultiItemEditStruct->hItemWndList[i],0,0,nNewStartPos,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);
        //        InvalidateRect(lpMultiItemEditStruct->hItemWndList[i],NULL,TRUE);
      }
//      lpMultiItemEditStruct->nContextBottomPos+=nScrollLine*nTextHeight;
      InvalidateRect(hWnd,NULL,TRUE);  // 无效窗口
      lpMultiItemEditStruct->nContextBottomPos=(lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
//      ClearBottomBlankArea(hWnd);

      SetMIEditVScrollPos(hWnd,lpMultiItemEditStruct->nDisplayLine);  // 设置滚动条

//      lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
		  SetPDACaret(hWnd);  // 设置光标
			return (int)iRealScrollLines;
}
/**************************************************
声明：static void HScrollMultiItemEdit(HWND hWnd,int xScrollPos)
参数：
	IN hWnd -- 窗口句柄
	IN xScrollPos -- 滚动的点数
返回值：无
功能描述：水平滚动控件。
引用: 
************************************************/
static void HScrollMultiItemEdit(HWND hWnd,int xScrollPos)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
 
      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return;
      lpMultiItemEditStruct->xDisplayPos+=xScrollPos;  // 滚动后新位置
      if (lpMultiItemEditStruct->xDisplayPos<0) lpMultiItemEditStruct->xDisplayPos=0;;
      SetMIEditHScrollPos(hWnd,(int)((lpMultiItemEditStruct->xDisplayPos)/HSCROLLWIDTH));  // 滚动屏幕
}

/**************************************************
声明：static BOOL AdjustCaretPosition(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：调整光标到可见区域。
引用: 
************************************************/
static BOOL AdjustCaretPosition(HWND hWnd)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nScrollLine,nPageLine;
//  RECT rcClientRect;
  int nCaretLine;
  BOOL bResult=FALSE;
  LPEDITITEM lpActiveEditItem;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;
    if (lpMultiItemEditStruct->nItemNum==0) return FALSE;  // 没有条目
    lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到激活条目
    AdjustCaretInEditItem(hWnd,lpActiveEditItem);  // 调整激活条目的光标位置
//    nTextHeight=GetTextHeight(hWnd);
//    GetClientRect(hWnd,&rcClientRect);
//    nPageLine=rcClientRect.bottom/nTextHeight;
    nPageLine=GetPageLine(hWnd); 
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpActiveEditItem->nCaretLine; // 得到光标的位置
    if (nCaretLine<0) nCaretLine=0;   // 需要调整到开头
    if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
    { // 需要调整到结尾
      nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1;
      bResult=TRUE;
    }

    if (nCaretLine<lpMultiItemEditStruct->nDisplayLine)
    { // 需要向下滚动窗口
      nScrollLine=nCaretLine-lpMultiItemEditStruct->nDisplayLine;
//      lpMultiItemEditStruct->nDisplayLine=nCaretLine;
      VScrollMultiItemEdit(hWnd,nScrollLine); // 滚动窗口
      bResult=TRUE;
    }
    if (nCaretLine>=lpMultiItemEditStruct->nDisplayLine+nPageLine)
    { // 需要向上滚动窗口
      nScrollLine=nCaretLine-(lpMultiItemEditStruct->nDisplayLine+nPageLine)+1;
//      lpMultiItemEditStruct->nDisplayLine=nCaretLine-nPageLine+1;
      VScrollMultiItemEdit(hWnd,nScrollLine); // 滚动窗口
      bResult=TRUE;
    }
    return bResult;
}
/**************************************************
声明：static void SetMIEditVScrollRange(HWND hWnd,int nTotalLine,int nWindowLine)
参数：
	IN hWnd -- 窗口句柄
	IN nTotalLine -- 控件总行数
	IN nWindowLine -- 控件窗口的行数
返回值：无
功能描述：设置垂直滚动条的范围。
引用: 
************************************************/
static void SetMIEditVScrollRange(HWND hWnd,int nTotalLine,int nWindowLine)
{
  int nMinPos,nMaxPos;
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
//      if (!(dwStyle&WS_VSCROLL)) return;
	  if (nWindowLine==0)
		  nWindowLine=1;
      if (nTotalLine<=nWindowLine)
      {
// !!! Add By Jami chen in 2004.08.21
		  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
		  int nScrollLine;

		  lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
		  if (lpMultiItemEditStruct==NULL) return;
		  // 如果要删除滚动条，则要从0 开始显示
		  nScrollLine=0-lpMultiItemEditStruct->nDisplayLine;
	//      lpMultiItemEditStruct->nDisplayLine=nCaretLine;
		  VScrollMultiItemEdit(hWnd,nScrollLine); // 滚动窗口
//		  lpMultiItemEditStruct->nDisplayLine = 0;
// !!! Add End By Jami chen in 2004.08.21
			if ((dwStyle&WS_VSCROLL))
			{  // 删除滚动条
				ShowScrollBar(hWnd,SB_VERT,FALSE);
				DoSize(hWnd,0,0);  // 调整窗口大小
//						SetHScrollBar(hWnd);
			}
//        EnableScrollBar(hWnd,SB_VERT,ESB_DISABLE_BOTH);
      }
      else
      {
        nMinPos=0;
        nMaxPos=nTotalLine-1;//-nWindowLine+1;
		if ((dwStyle&WS_VSCROLL)==0)
		{ // 需要显示窗口
			ShowScrollBar(hWnd,SB_VERT,TRUE);
			DoSize(hWnd,0,0); // 调整窗口大小
	        Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE); // 设置滚动范围
			SetMIEditScrollPage(hWnd);  // 设置滚动页
		}
		else
			Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
        EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH);  // 滚动条ENABLE
      }
}
/**************************************************
声明：static void SetMIEditScrollPage(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：无
功能描述：设置水平滚动条的范围。
引用: 
************************************************/
static void SetMIEditScrollPage(HWND hWnd)
{
   int nPageLine;
//   int nTextHeight;
   //RECT rcClientRect;
   SCROLLINFO ScrollInfo;
   DWORD dwStyle;
      
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格

      nPageLine=GetPageLine(hWnd);  // 得到页的行数

      ScrollInfo.cbSize=sizeof(SCROLLINFO);
      ScrollInfo.fMask=SIF_PAGE;
      // Set Vertical Scroll Page
      if (dwStyle&WS_VSCROLL)
      {
        ScrollInfo.nPage =nPageLine;
        SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);
      }

      // Set horizonta Scroll Page
      if (dwStyle&WS_HSCROLL)
      {
          ScrollInfo.nPage =1;
          SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);
      }
}
/**************************************************
声明：static void SetMIEditVScrollPos(HWND hWnd,int nNewScrollPos)
参数：
	IN hWnd -- 窗口句柄
	IN nNewScrollPos -- 新的滚动条的位置
返回值：无
功能描述：设置垂直滚动条的位置。
引用: 
************************************************/
static void SetMIEditVScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE);
}
/**************************************************
声明：static void SetMIEditHScrollPos(HWND hWnd,int nNewScrollPos)
参数：
	IN hWnd -- 窗口句柄
	IN nNewScrollPos -- 新的滚动条的位置
返回值：无
功能描述：设置水平滚动条的位置。
引用: 
************************************************/
static void SetMIEditHScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
      if (!(dwStyle&WS_HSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE);
}
/**************************************************
声明：static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- LOWORD，滚动类型
	IN lParam -- 保留
返回值：成功返回实际的滚动行数，否则返回0
功能描述：垂直滚动屏幕，处理WM_VSCROLL消息。
引用: 
************************************************/
static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nWindowLine;
    RECT rcClientRect;
    int nTextHeight,yPos;
        
      GetClientRect(hWnd,&rcClientRect);  // 得到客户矩形
      nTextHeight=GetTextHeight(hWnd);  // 得到文本高度
      nWindowLine=rcClientRect.bottom/nTextHeight;
      yPos=Edit_GetScrollPos(hWnd,SB_VERT); // 得到滑块位置
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP: // 上翻页
          nScrollLine=0-nWindowLine;  
          break;
        case SB_PAGEDOWN:  // 下翻页
          nScrollLine=nWindowLine;
          break;
        case SB_LINEUP:  // 上滚行
          nScrollLine=-1;
          break;
        case SB_LINEDOWN:  // 下滚行
          nScrollLine=1;
          break;
        case SB_THUMBTRACK:  // 滑块移动
          nScrollLine=HIWORD(wParam)-yPos;
          break;
        default:
          nScrollLine=0;
          return 0;
        }
        return VScrollMultiItemEdit(hWnd,nScrollLine);  // 滚动窗口
}

/**************************************************
声明：static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- LOWORD，滚动类型
	IN lParam -- 保留
返回值：无
功能描述：水平滚动窗口，处理WM_HSCROLL消息。
引用: 
************************************************/
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int xScrollPos,xWindowWidth;
    RECT rcClientRect;
    int xPos;
        
      GetClientRect(hWnd,&rcClientRect);  // 得到客户矩形
      xWindowWidth=(rcClientRect.right/HSCROLLWIDTH)*HSCROLLWIDTH;
      xPos=Edit_GetScrollPos(hWnd,SB_HORZ); // 得到滑块位置
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // 上翻页
          xScrollPos=0-xWindowWidth;
          break;
        case SB_PAGEDOWN:  // 下翻页
          xScrollPos=xWindowWidth;
          break;
        case SB_LINEUP:  // 上滚行
          xScrollPos=0-HSCROLLWIDTH;
          break;
        case SB_LINEDOWN:  // 下滚行
          xScrollPos=HSCROLLWIDTH;
          break;
        case SB_THUMBTRACK:  // 移动滑块
          xScrollPos=(HIWORD(wParam)-xPos)*HSCROLLWIDTH;
//          Print(100,10,xScrollPos);
          break;
        default:
          return;

        }
 
      HScrollMultiItemEdit(hWnd,xScrollPos); // 水平滚动窗口
}

/**************************************************
声明：static int GetPageLine(HWND hWnd)
参数：
	IN hWnd -- 窗口句柄
返回值：返回一页的行数
功能描述：得到一页的行数
引用: 
************************************************/
// delete By Jami chen in 2004.06.12
/*static int GetPageLine(HWND hWnd)
{
  int nTextHeight,nPageLine;
  RECT rcClientRect;
    nTextHeight=GetTextHeight(hWnd);  // 得到文本高度
    GetClientRect(hWnd,&rcClientRect); // 得到窗口客户矩形
    nPageLine=rcClientRect.bottom/nTextHeight; // 得到页行数
	if (nPageLine==0)
		nPageLine=1; // 最少可显示一行
    return nPageLine;
}
*/
/*
static void DoDisplayChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int nDisplayx,nCaretLine;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;
  HWND hActiveWnd;
  int iActiveItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return;

      hActiveWnd=(HWND)wParam;
      nDisplayx=LOWORD(lParam);
      nCaretLine=HIWORD(lParam);
      if (lpMultiItemEditStruct->xDisplayPos!=nDisplayx)
      {
          lpMultiItemEditStruct->xDisplayPos=nDisplayx;

          for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
          {

//            PostMessage(lpMultiItemEditStruct->hItemWndList[i],MIEM_SETXDISPLAYPOS,lpMultiItemEditStruct->xDisplayPos,0);
          }
          SetMIEditHScrollPos(hWnd,(int)((lpMultiItemEditStruct->xDisplayPos)/HSCROLLWIDTH));
      }
// !!      iActiveItem=GetItemSequence(hWnd,hActiveWnd);
      nCaretLine+=lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
      AdjustCaretPosition(hWnd);
}
*/
/**************************************************
声明：void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem)
参数：
	IN hWnd -- 窗口句柄
	IN lpEditItem -- 当前的编辑条目结构指针
返回值：无
功能描述：设置光标到下一个条目。
引用: 
************************************************/
void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpNewEditItem;
  int iActiveItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return;
    iActiveItem=GetItemSequence(hWnd,lpEditItem);  // 得到条目索引
    iActiveItem++; // 得到下一个条目索引
    if (iActiveItem>=lpMultiItemEditStruct->nItemNum)
      return;
    ClearInvert(hWnd,lpEditItem,TRUE);
    lpMultiItemEditStruct->iActiveItem=iActiveItem;  // 激活条目
    lpNewEditItem=&lpMultiItemEditStruct->lpItemList[iActiveItem];
		SetPDACaretPosition(hWnd,lpNewEditItem,0,0);  // 设置光标到条目开始
    if (AdjustCaretPosition(hWnd)==TRUE);  // 调整光标位置
//		{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
//				InvalidateRect(hWnd,NULL,TRUE);
//		}

		// set caret position
		SetPDACaret(hWnd);  // 设置光标

    // show caret
    ShowPDACaret(hWnd);  // 显示光标
	SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // 通知父窗口
//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,0,0);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
}
/**************************************************
声明：static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT ，指定条目
	IN lParam -- MAKELPARAM(x,y),指定的坐标
返回值：低WORD返回指定坐标所在的字符索引位置，高WORD返回该字符所在的行。
功能描述：得到指定点的字符，处理EM_CHARFROMPOS消息。
引用: 
************************************************/
static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
  int xPos,yPos;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

	  nItem=(int)wParam; // 得到条目索引
	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
    xPos=LOWORD(lParam);
    yPos=HIWORD(lParam);
    return GetCharFromPos(hWnd,lpEditItem,xPos,yPos);  // 得到指定位置的字符

//	return SendMessage(lpMultiItemEditStruct->hItemWndList[nItem],EM_CHARFROMPOS,0,lParam);
}
/**************************************************
声明：static LRESULT DoGetFirstVisibleLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回第一个可见行的索引
功能描述：得到第一个可见行的索引，处理EM_GETFIRSTVISIBLELINE消息。
引用: 
************************************************/
static LRESULT DoGetFirstVisibleLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

	return lpMultiItemEditStruct->nDisplayLine;  // 得到显示行索引
}
/**************************************************
声明：static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT ，指定条目
	IN lParam -- 保留
返回值：返回指定条目的文本限制
功能描述：得到指定条目的文本限制，处理EM_GETLIMITTEXT消息。
引用: 
************************************************/
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

	  nItem=(int)wParam; // 得到条目索引
	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
	  return lpEditItem->cbEditLimitLen; // 返回条目文本限制
}
/**************************************************
声明：static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定行的索引
	IN lParam -- LPSTR 存放指定行数据的缓存，前2个字节存放缓存的大小
返回值：返回拷贝字符的个数
功能描述：得到指定行的数据，处理EM_GETLINE消息。
引用: 
************************************************/
static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nLine,nLineLen,nBufferMaxLen;
  LPSTR lpch;
  LPSTR lpLineAddress;
  int nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return 0;

	  if (lpMultiItemEditStruct->nItemNum==0) return 0;  // 没有条目

	  nLine=(int)wParam; // 得到行索引
#ifdef _MAPPOINTER
	  lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
	  lpch=(LPSTR)lParam; // 得到缓存
	  nItem=lpMultiItemEditStruct->nItemNum-1;
	  for (;nItem>=0;nItem--)
	  {
		  if (lpMultiItemEditStruct->nItemStartLineList[nItem]<=nLine)
			  break;
	  }
	  nLine-=lpMultiItemEditStruct->nItemStartLineList[nItem];
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到指定行的条目结构

		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);  // 得到行地址
		nLineLen=GetLineLength(hWnd,lpEditItem,(LPCTSTR)lpLineAddress,NULL); // 得到行字符
		nBufferMaxLen=MAKEWORD(*lpch,*(lpch+1));
		if (nLineLen>=nBufferMaxLen)
			return 0;
		memcpy((void *)lpch,(const void *)lpLineAddress,nLineLen);  // 复制数据到缓存
		lpch[nLineLen]=0;
		return nLineLen;
}
/**************************************************
声明：static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：成功返回控件的总行数，否则返回0
功能描述：得到控件的总行数，处理EM_GETLINECOUNT消息。
引用: 
************************************************/
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
		if (lpMultiItemEditStruct->nItemNum==0) return 0;

		return lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]; // 得到总行数
}
/**************************************************
声明：static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定编辑条目
	IN lParam -- 保留
返回值：如果指定条目是-1，则返回控件的修改标志，否则返回指定条目的修改标志。
功能描述：返回修改标志，处理EM_GETMODIFY消息。
引用: 
************************************************/
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nItem;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;
	if (lpMultiItemEditStruct->nItemNum==0) return FALSE; // 没有数据
	
	nItem=(int)wParam;  // 得到条目索引

	if (nItem==-1)
	{  // 得到编辑区的编辑标志，是所有条目的标志综合
		for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum-1;nItem++)
		{
			lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
			if (lpEditItem->fModified)
			{  // 有条目编辑过，则当前编辑区为已经编辑过
						return TRUE;
			}
		}
	}
	else
	{  // 得到指定条目的编辑标志
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return FALSE;
		lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		if (lpEditItem->fModified)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**************************************************
声明：static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目的次序
	IN lParam -- 保留
返回值：返回指定条目的密码字符
功能描述：得到条目的密码字符，处理EM_GETPASSWORDCHAR消息。
引用: 
************************************************/
static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;  // 得到条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到条目结构
		if (lpEditItem->dwStyle&ES_PASSWORD)
				return lpEditItem->chPassWordWord;  // 返回密码字符
		else
				return 0;
}
/**************************************************
声明：static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 要设定的文本限制
	IN lParam -- INT 指定条目的次序
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置指定条目的文本限制，处理EM_SETLIMITTEXT消息。
引用: 
************************************************/
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 int cchMax;
   int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		cchMax=(int)wParam;  // 得到限制文本
		nItem=(int)lParam; // 得到条目限制
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构

    return SetLimitText(hWnd,lpEditItem,cchMax);  // 设定条目限制
}

/**************************************************
声明：static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- int ，字符索引
	IN lParam -- INT , 指定条目
返回值：返回指定条目的指定索引字符的行索引。
功能描述：返回指定条目的指定索引字符的行索引，处理EM_LINEFROMCHAR消息。
引用: 
************************************************/
static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
	DWORD dwStyle;
	LPTSTR lpCurPos;
	int ich,nLine;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		ich=(int)wParam;  // 得到字符索引
		nItem=(int)lParam; // 得到条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
		dwStyle=lpEditItem->dwStyle;  // 得到条目风格
		if (ich==-1)
		{ // 没有指定字符
				if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
				{ // 没有选择
						lpCurPos=lpEditItem->lpCaretPosition;
				}
				else
				{  // 选择开始位置
						lpCurPos=lpEditItem->lpInvertStart;
				}
		}
		else
		{ // 有指定字符
				if (ich>lpEditItem->cbEditLen)
					ich=lpEditItem->cbEditLen;
				lpCurPos=lpEditItem->lpPDAEditBuffer+ich;  // 得到字符指针
		}
    GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);  // 得到指定字符的坐标位置
		return (LRESULT)nLine;
}

/**************************************************
声明：static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT ，指定行的索引
	IN lParam -- INT ，指定条目
返回值：返回指定条目指定行索引所对应的字符索引
功能描述：得到指定条目指定行索引所对应的字符索引，处理EM_LINEINDEX消息。
引用: 
************************************************/
static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	LPTSTR lpCurPos;
	int nLine;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		nLine=(int)wParam;  // 得到行索引
		nItem=(int)lParam;  // 得到条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
		if (nLine==-1)
		{ // 没有指定行索引
				nLine=lpEditItem->nCaretLine;
		}
		lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine);  // 得到行开始地址
		return (LRESULT)(lpCurPos-lpEditItem->lpPDAEditBuffer); // 得到行开始位置索引
}
/**************************************************
声明：static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定字符的索引
	IN lParam -- INT 指定条目
返回值：返回指定条目指定字符所在行的字符个数
功能描述：得到指定条目指定字符所在行的字符个数，处理EM_LINELENGTH消息。
引用: 
************************************************/
static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int ich;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		ich=(int)wParam;  // 字符索引
		nItem=(int)lParam; // 条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
    return GetLineLengthFromPos(hWnd,lpEditItem,ich); // 得到指定字符所在行的长度
}
/**************************************************
声明：static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 水平滚动字符数
	IN lParam -- INT 垂直滚动行数
返回值：成功返回TRUE，否则返回FALSE
功能描述：滚动窗口，处理EM_LINESCROLL消息。
引用: 
************************************************/
static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int cxScroll,cyScroll;

		cxScroll=(int)wParam;
		cyScroll=(int)lParam;
		if (cxScroll)
		{
//				cxScroll=cxScroll*HSCROLLWIDTH;
				cxScroll=cxScroll*8; // 8--> the width of the char
				HScrollMultiItemEdit(hWnd,cxScroll);  // 水平滚动
		}
		if (cyScroll)
				VScrollMultiItemEdit(hWnd,cyScroll);  // 垂直滚动
		return TRUE;
}
/**************************************************
声明：static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定的字符索引
	IN lParam -- INT 指定条目
返回值：返回指定条目指定字符的坐标,MAKERESULT(x,y)
功能描述：得到指定条目指定字符的坐标，处理EM_POSFROMCHAR消息。
引用: 
************************************************/
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	int wCharIndex;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		wCharIndex=(int)wParam; // 字符索引
		nItem=(int)lParam;  // 条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
    return GetPosFromChar(hWnd,lpEditItem,wCharIndex);  // 得到字符位置
}
/**************************************************
声明：static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目，如果指定条目为-1，则为当前激活条目
	IN lParam -- LPTSTR ,指定的字符串
返回值：无
功能描述：用指定的字符串替换选择字符。
引用: 
************************************************/
static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPCTSTR lpReplace;
	int cchLen;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;  // 得到条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

		if (nItem<-1)
			return 0;

		if (nItem==-1)
			nItem=lpMultiItemEditStruct->iActiveItem;
	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		lpReplace=(LPCTSTR)lParam;  // 要替换的字符
		DeleteInvert(hWnd,lpEditItem); // 删除选择字符
		cchLen=strlen(lpReplace);
		InsertString(hWnd,lpEditItem,lpReplace,cchLen); // 插入替换字符
		SetCaretCoordinate(hWnd,lpEditItem); // 设定光标位置
		AdjustCaretPosition(hWnd); // 调整光标位置
//		InvalidateRect(hWnd,NULL,TRUE);
		SetPDACaret(hWnd);  // 设置光标
		return 0;
}

/**************************************************
声明：static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 滚动类型
	IN lParam -- 保留
返回值：滚动的行数
功能描述：垂直滚动屏幕，处理EM_SCROLL消息。
引用: 
************************************************/
static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	int nScrollLine;
		switch(wParam)
		{
		case SB_LINEDOWN:
		case SB_LINEUP:
		case SB_PAGEDOWN:
		case SB_PAGEUP:
			break;
		default:
			return FALSE;
		}
    nScrollLine=DoVScrollWindow(hWnd,wParam,lParam);  // 滚动窗口
		lResult=MAKELRESULT(nScrollLine,1);
		return lResult;
}
/**************************************************
声明：static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：滚动光标到可见位置，处理EM_SCROLLCARET消息。
引用: 
************************************************/
static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
 LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;
//		iActiveItem=lpMultiItemEditStruct->iActiveItem;
//		SendMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_GETCARETPOS,(WPARAM)&nCaretLine,(LPARAM)&xPos);
//    nCaretLine+=lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
	  AdjustCaretPosition(hWnd); // 调整光标
		return TRUE;
}
/*
static LRESULT DoNoticeSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	HWND hActiveWnd;
      hActiveWnd=(HWND)lParam;
			lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
			if (lpMultiItemEditStruct==NULL) return 0;

//!!      lpMultiItemEditStruct->iActiveItem=GetItemSequence(hWnd,hActiveWnd);
			return TRUE;
}
*/
/**************************************************
声明：static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- UINT，要设置的修改标志
	IN lParam -- INT ，指定条目，如果条目为-1 ，则设置所有的条目
返回值：无
功能描述：设置指定条目新的修改标志，处理EM_SETMODIFY消息。
引用: 
************************************************/
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	UINT  fModified;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

    fModified=(UINT)wParam;  // 得到编辑标志
		nItem=(int)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<-1)
			return 0;
		if (nItem==-1)
		{ // 设置所有的条目为指定的编辑标志
			for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
			{
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     		lpEditItem->fModified=fModified;
			}
		}
		else
		{ // 设置指定条目的编辑标志
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		    lpEditItem->fModified=fModified;
		}
		return 0;
}
/**************************************************
声明：static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- TCHAR 新的密码字符
	IN lParam -- INT 指定条目
返回值：成功返回TRUE ，否则返回FALSE。
功能描述：设置新的密码字符，处理EM_SETPASSWORDCHAR消息。
引用: 
************************************************/
static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	DWORD dwStyle;
	TCHAR ch;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		ch=(TCHAR)wParam;  // 得到密码字符
		nItem=(int)lParam;  // 得到要设定的条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构

		dwStyle=lpEditItem->dwStyle;  // 得到风格
		if (dwStyle&ES_MULTILINE)
			return 0;
    return SetPasswordChar(hWnd,lpEditItem,ch);  // 设置密码字符
}

/**************************************************
声明：static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- BOOL，是否要设置为只读的标志
	IN lParam -- INT 指定条目
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置指定条目编辑区的只读属性，处理EM_SETREADONLY消息。
引用: 
************************************************/
static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
	BOOL fReadOnly;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		fReadOnly=(BOOL)wParam;  // 得到只读属性
		nItem=(int)lParam;  
		// Maybe Add Function .nItem == -1, then set all item are Read Only
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;
		 lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
		if (fReadOnly)
		{  // 设置为只读
			lpEditItem->dwStyle|=ES_READONLY;
		}
		else
		{ // 清除只读属性
			lpEditItem->dwStyle&=~ES_READONLY;
		}
		return TRUE;
}
/**************************************************
声明：static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：成功返回拷贝字符个数，否则返回0
功能描述：拷贝选择区域字符到剪切板，处理WM_COPY消息。
引用: 
************************************************/
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到激活条目的条目结构

    return CopyToClipboard(hWnd,lpEditItem);  // 复制选择到剪切板
}
/**************************************************
声明：static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：
功能描述：粘帖，处理WM_PASTE消息。
引用: 
************************************************/
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到激活条目结构

    PasteFromClipboard(hWnd,lpEditItem); // 从剪切板粘贴数据
		AdjustCaretPosition(hWnd);  // 调整光标
		SetPDACaret(hWnd); // 设置光标
    return 0;
}
/**************************************************
声明：static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：剪切，处理WM_CUT消息。
引用: 
************************************************/
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		DoCopy(hWnd,0,0);  // 复制
		DoClear(hWnd,0,0); // 清除
		return TRUE;
}

/**************************************************
声明：static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：清除，处理WM_CLEAR消息。
引用: 
************************************************/
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到条目结构

		DeleteInvert(hWnd,lpEditItem);  // 删除选择
		SetCaretCoordinate(hWnd,lpEditItem);  // 设置光标位置
		AdjustCaretPosition(hWnd); // 调整光标
		SetPDACaret(hWnd); // 设置光标
		return TRUE;
}
/**************************************************
声明：static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- 保留
返回值：无
功能描述：清空编辑区，处理EM_EMPTY消息。
引用: 
************************************************/
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  nItem=(int)wParam; // 得到条目索引
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<-1)
			return 0;
	  if (nItem==-1)
	  {  // 没有指定条目
		  for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
			{ // 清除所有条目的内容
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  ClearEditItemStruct(lpEditItem);

				SetWindowRect(hWnd,lpEditItem,FALSE);
			}
		    lpMultiItemEditStruct->iActiveItem=0;  // 设置第一个条目为激活条目
			SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // 发送统治消息
	  }
	  else
	  {  // 清除指定条目的内容
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  ClearEditItemStruct(lpEditItem);
		  SetWindowRect(hWnd,lpEditItem,FALSE);
	  }
    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到激活条目的结构
	SetPDACaretPosition(hWnd,lpEditItem,0,0); // 设置光标位置
    AdjustCaretPosition(hWnd); // 调整光标
	SetPDACaret(hWnd); // 设置光标
    SetFocus(hWnd); // 设置焦点
//    ShowCaret(hWnd);
//		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
}
/**************************************************
声明：static LRESULT DoPaint(HWND hWnd,HDC hdc)
参数：
	IN hWnd -- 窗口句柄
	IN hdc -- 设备句柄
返回值：无
功能描述：绘制控件，处理WM_PAINT消息。
引用: 
************************************************/
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
    for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    { // 绘制指定条目
       DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[nItem],PE_SHOWCLIENT);
    }
		ClearBottomBlankArea(hWnd); // 清除底部空白
    return 0;
}
/*****************************************************************/

/**************************************************
声明：static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- HDC 指定设备的设备句柄
	IN lParam -- 保留
返回值：无
功能描述：将编辑区打印到指定的设备，处理WM_PRINTCLIENT消息。
引用: 
************************************************/
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;		
	LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	int nItem;
    int nDisplayLine,nDisplayx;//,iStartPos;

	hdc=(HDC)wParam; // 得到设备句柄
    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;
	
   nDisplayLine=lpMultiItemEditStruct->nDisplayLine;  // 得到显示开始行
   nDisplayx=lpMultiItemEditStruct->xDisplayPos; // 得到开始位置
   lpMultiItemEditStruct->nDisplayLine=0; // 设置从头开始显示
   lpMultiItemEditStruct->xDisplayPos=0;
	for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    { // 绘制条目到指定设备句柄
       DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[nItem],PE_SHOWALL);
    }
   lpMultiItemEditStruct->nDisplayLine=nDisplayLine;  // 恢复开始显示位置
   lpMultiItemEditStruct->xDisplayPos=nDisplayx;
   return 0;
}
/*****************************************************************/

// !!! Add By Jami chen in 2004.07.19
// **************************************************
// 声明：static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值： 无
// 功能描述：系统颜色改变，处理WM_SYSCOLORHANGED消息。
// 引用: 
// **************************************************
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

	    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
		if (lpMultiItemEditStruct==NULL) return 0;
		
		//重新设定当前文本的颜色
		lpMultiItemEditStruct->cl_NormalText = GetSysColor(COLOR_WINDOWTEXT);
		lpMultiItemEditStruct->cl_NormalBkColor = GetSysColor(COLOR_WINDOW);

		lpMultiItemEditStruct->cl_InvertText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		lpMultiItemEditStruct->cl_InvertBkColor = GetSysColor(COLOR_HIGHLIGHT);

		lpMultiItemEditStruct->cl_DisableText = GetSysColor(COLOR_GRAYTEXT);
		lpMultiItemEditStruct->cl_DisableBkColor = GetSysColor(COLOR_WINDOW);

		lpMultiItemEditStruct->cl_ReadOnly = lpMultiItemEditStruct->cl_NormalText;
		lpMultiItemEditStruct->cl_ReadOnlyBk = lpMultiItemEditStruct->cl_NormalBkColor;

		lpMultiItemEditStruct->cl_Title = lpMultiItemEditStruct->cl_NormalText;
		lpMultiItemEditStruct->cl_TitleBk = lpMultiItemEditStruct->cl_NormalBkColor;

		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
// !!! Add End By Jami chen in 2004.07.19

/**************************************************
声明：static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：改变窗口大小，处理WM_SIZE消息。
引用: 
************************************************/
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  RECT rectNew;
  int i;
  int xPos,nLine;
  int nTextHeigth,nClientLine;

       lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
       if (lpMultiItemEditStruct==NULL) return 0;

       GetClientRect(hWnd,&rectNew);  // 得到客户矩形
       for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
       { // 重设大小
          lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
          lpEditItem->rect.right=rectNew.right;
          // Add by Jami 2000-06-02  -- Must Reset The Item Height
          SetWindowRect(hWnd,lpEditItem,FALSE);
//          TotalLine(
       }
       
	   lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到激活条目结构
       GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,lpEditItem->dwStyle&ES_MULTILINE);// 得到光标位置
	   lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;  // 设置光标
	   lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine; 
	   AdjustCaretPosition(hWnd);  // 调整光标
// !!! Add By Jami chen in 2004.08.21
       nTextHeigth=GetTextHeight(hWnd);
       nClientLine=rectNew.bottom/nTextHeigth;
       SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
       SetMIEditVScrollPos(hWnd,lpMultiItemEditStruct->nDisplayLine);  // 设置滚动条
// !!! Add End By Jami chen in 2004.08.21
	   InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
       return 0;
}

#ifdef DITHERING_DEAL  // 抖动处理
static int old_x = -1,old_y = -1;
#endif
/**************************************************
声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
返回值：无
功能描述：处理WM_LBUTTONDOWN消息。
引用: 
************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem,nOldActiveItem;
//	DWORD fwKeys;
	int xPos,yPos;


			// Get Virtal Key with LButton Down
//	    fwKeys=wParam;
			// Get LButton down Position
			xPos=(int)(short)LOWORD(lParam);
			yPos=(int)(short)HIWORD(lParam);

#ifdef DITHERING_DEAL  // 抖动处理
			old_x = xPos;
			old_y = yPos;
#endif
		  // Get struct EDITITEM data
		  lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
		  if(lpMultiItemEditStruct==NULL) return FALSE;
			if (lpMultiItemEditStruct->nItemNum==0) return 0;

		  nItem=GetItem(hWnd,xPos,yPos); // 得到指定位置的条目索引
		  if (nItem==-1)
			 return 0;
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到激活条目结构
		  ClearInvert(hWnd,lpEditItem,TRUE);  // 清除选择

		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到指定条目结构
		  if (lpEditItem->dwStyle&WS_DISABLED)
			  return 0;
		  nOldActiveItem = lpMultiItemEditStruct->iActiveItem;
		  lpMultiItemEditStruct->iActiveItem=nItem; // 设置指定条目为激活条目
		  yPos-=lpEditItem->iStartPos;
			
			if (lpEditItem->dwStyle&ES_CALLBACK)
			{ // 当前条目有回调功能
				if (InCallBackPos(hWnd,lpEditItem,xPos,yPos)==TRUE)
				{ // 回调函数
					if (lpEditItem->lpCallBack!=NULL)
					{
//						lpEditItem->lpCallBack(hWnd,(short)nItem);
#ifdef _MAPPOINTER
						CALLBACKDATA  CallbackData;
						
							CallbackData.hProcess = GetCallerProcess();  // 设置应用程序进程句柄
							CallbackData.lpfn = (FARPROC)lpEditItem->lpCallBack; // 设置回调函数指针
							CallbackData.dwArg0 = (DWORD)hWnd; // 设置声音结构
							
							Sys_ImplementCallBack( &CallbackData, nItem);
#else
						EDITCALLBACK  lpeditCallBack;

						lpeditCallBack = (EDITCALLBACK)lpEditItem->lpCallBack;
						lpeditCallBack(hWnd,(short)nItem);
#endif
					}
					return 0;
				}
			}
			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos); // 设置光标函数

			//  is or not get facus ???
			if (GetFocus()!=hWnd)
			{  // if not active then get focus
				SetFocus(hWnd);
			}

			// Clear Invert and redraw
			ClearInvert(hWnd, lpEditItem,TRUE);
			//// set caret position
			//SetPDACaret(hWnd);

			// Capture the mouse
			if (GetCapture()!=hWnd)
				SetCapture(hWnd);

			// set Invert Area
			SetInvertArea(hWnd,lpEditItem);

      if (AdjustCaretPosition(hWnd)==TRUE)
			{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
//				InvalidateRect(hWnd,NULL,TRUE);
			}

			// set caret position
			SetPDACaret(hWnd);

      // show caret
      ShowPDACaret(hWnd);
	  if (nOldActiveItem != lpMultiItemEditStruct->iActiveItem)
	  {
			SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE);
	  }
      return 0;
}
/**************************************************
声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
返回值：无
功能描述：处理WM_MOUSEMOVE消息。
引用: 
************************************************/
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	int xPos,yPos;
//  int nItem;

	// if the mouse be capture by this window
	if (GetCapture()==hWnd)
	{

			// Get LButton down Position
			xPos=(int)(short)LOWORD(lParam);
			yPos=(int)(short)HIWORD(lParam);

#ifdef DITHERING_DEAL  // 抖动处理
			if (((old_x - xPos) > -3 && (old_x - xPos) < 3) && 
				((old_y - yPos) > -3 && (old_y - yPos) < 3))
			{
				return 0;
			}

			old_x = xPos;
			old_y = yPos;
#endif
		  // Get struct EDITITEM data
		  lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if(lpMultiItemEditStruct==NULL) return FALSE;
	    if (lpMultiItemEditStruct->nItemNum==0) return 0;

      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到条目结构
      yPos-=lpEditItem->iStartPos;


			// set Caret position to LButton Down Position
// !!! Modified By jami chen in 2004.10.18
//			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);
			if (SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos) == FALSE)
			{
				// 光标位置没有发生改变
				return 0;
			}
// !!! Modified End

			//  the caret coordinate position is or not in the
			// client rect ???
			// if not in the client rect ,then adjust and return TRUE
			if (AdjustCaretPosition(hWnd)==TRUE)
			{
				// set caret position
				SetPDACaret(hWnd);
				// Set Invert Area
				SetInvertArea(hWnd,lpEditItem);
				// get  hdc
//				DrawPDAEditControl(hWnd);
//				InvalidateRect(hWnd,NULL,TRUE);
			}
			else
			{
				// set caret position
				SetPDACaret(hWnd);
				// set invert area
				SetInvertArea(hWnd,lpEditItem);
				// redraw the changed area
				DisplayInvertChange(hWnd,lpEditItem);
			}
	}
  return 0;
}
/**************************************************
声明：static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
返回值：无
功能描述：处理WM_LBUTTONUP消息。
引用: 
************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture
	SetCapture(NULL);
  return 0;
}
/**************************************************
声明：static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：设置焦点，处理WM_SETFOCUS消息。
引用: 
************************************************/
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	HWND hParentWnd;
	DWORD dwStyle;
	LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
//  int nItem;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
		if (lpMultiItemEditStruct==NULL) return 0;
		if (lpMultiItemEditStruct->nItemNum==0) return 0;
		CreatePDACaret(hWnd,&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]);
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (dwStyle&ES_MULTIITEMEDIT)
		{
		//				hParentWnd=GetParent(hWnd);
		SendNotificationMsg( hWnd,EN_SETFOCUS);
		}
		SetPDACaret(hWnd);
		// show caret
		ShowPDACaret(hWnd);
		return 0;
}

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
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;

       lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
       if (lpMultiItemEditStruct==NULL) return 0;
       if (lpMultiItemEditStruct->nItemNum==0) return 0;
	    // get struct EDITITEM data
		lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

// !!! Modified By jami chen in 2005.01.12 
//      ClearInvert(hWnd,lpEditItem,TRUE); // 清除选择
// !!! to  By Jami chen in 2005.01.12
	    if (!(lpEditItem->dwStyle & ES_NOHIDESEL ))
		{
	        ClearInvert(hWnd,lpEditItem,TRUE); // 清除选择
		}
// !!! Modified  End
		ClearInvert(hWnd,lpEditItem,TRUE);
	    DeletePDACaret(hWnd);
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);
	    return 0;
}

/**************************************************
声明：static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：字符输入，处理WM_CHAR消息。
引用: 
************************************************/
static LRESULT DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
	DWORD dwStyle;
	BOOL bResult;
	DWORD dwShowFlag;
	LPTSTR lpShowAddress;
	TCHAR chCharCode;
	int nShowLine,nShowPos;
	HDC hdc;
	int xPos,nLine;
	int bDeleteResult;
//  BOOL bDeleteInvert;
	DWORD lKeyData;
	BYTE repeat;

	  lKeyData=(DWORD)lParam;
	  repeat=(BYTE)(lKeyData&0xff);

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return 0;
	    if (lpMultiItemEditStruct->nItemNum==0) return 0;
	   // get struct EDITITEM data
		 lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
     if (lpEditItem==NULL) return 0;
		// get window style
//	   dwStyle=GetWindowLong(hWnd,GWL_STYLE);
	   dwStyle=lpEditItem->dwStyle;
     if (dwStyle&ES_READONLY) return 0;
     if (dwStyle&WS_DISABLED) return 0;
	   // get will be redrawed start address
	   lpShowAddress=lpEditItem->lpCaretPosition;

     // get input char code
		chCharCode=(TCHAR)wParam;
		if (chCharCode==ENTERCODE)
		{ // the input char is enter code
			if (PickCharacter(0x00,chCharCode,dwStyle)==FALSE) return 0;
   		//Delete Invert Area
//      if (DeleteInvert(hWnd,lpEditItem)) bDeleteInvert=TRUE;
      DeleteInvert(hWnd,lpEditItem);

	    lpShowAddress=lpEditItem->lpCaretPosition;
      // insert enter code
			bResult=InsertChar(hWnd,lpEditItem,0x0d,0x0a);
			// will redraw to the end of the text from the caret position
			dwShowFlag=PE_SHOWTEXTEND;
		}
		else if(chCharCode==BACKSPACE)
		{   // the input char is Backspace

      //Delete Invert Area
      if (DeleteInvert(hWnd,lpEditItem))
      {
//        bDeleteInvert=TRUE;
			  bResult=TRUE;
      }
      else
      {
			  // Do backspace operation
			  bDeleteResult=BackSpaceChar(hWnd,lpEditItem);
			  if (bDeleteResult==0)
			  { // the operation failare
				  bResult=FALSE;
			  }
			  else
			  { // the operation success
				  if (bDeleteResult==1) // normal operation
					  dwShowFlag=PE_SHOWLINEEND;
				  else // delete the enter code
					  dwShowFlag=PE_SHOWTEXTEND;
				  bResult=TRUE;
			  }
      }
			// set redraw start addresss
		  lpShowAddress=lpEditItem->lpCaretPosition;
		}
		else
		{ // the input char is normal char
//			if (chCharCode<0)
			if (chCharCode<0 || chCharCode > 0x7F)
			{ // the input char
				if (lpEditItem->HalfByte==FALSE)
				{ // the input char is chinese high byte
					lpEditItem->HiByte=chCharCode;
					lpEditItem->HalfByte=TRUE;
					return 0;
				}
				else
				{ // the input char is chinese low byte
					lpEditItem->LoByte=chCharCode;

					if (PickCharacter(lpEditItem->HiByte,lpEditItem->LoByte,dwStyle)==FALSE) return 0;
					//Delete Invert Area
//      if (DeleteInvert(hWnd,lpEditItem)) bDeleteInvert=TRUE;
          DeleteInvert(hWnd,lpEditItem);
	        lpShowAddress=lpEditItem->lpCaretPosition;
					// Insert chinese word
					bResult=InsertChar(hWnd,lpEditItem,lpEditItem->HiByte,lpEditItem->LoByte);
					// clear chinese high and low byte
					lpEditItem->HiByte=0;
					lpEditItem->LoByte=0;
					lpEditItem->HalfByte=FALSE;
					// must redraw to the end of the line from the caret position
					dwShowFlag=PE_SHOWLINEEND;
				}
			}
			else
			{
				if (PickCharacter(0,chCharCode,dwStyle)==FALSE) return 0;
				if (dwStyle&ES_UPPERCASE)
					chCharCode=toupper(chCharCode);
				if (dwStyle&ES_LOWERCASE)
					chCharCode=tolower(chCharCode);
     		//Delete Invert Area
//        if (DeleteInvert(hWnd,lpEditItem)) bDeleteInvert=TRUE;
        DeleteInvert(hWnd,lpEditItem);

	      lpShowAddress=lpEditItem->lpCaretPosition;
        // the input char is english char or symbol char
				bResult=InsertChar(hWnd,lpEditItem,0x00,chCharCode);
				// must redraw to the end of the line from the caret position
				dwShowFlag=PE_SHOWLINEEND;
			}
		}
		// get caret coordinate
        GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,dwStyle&ES_MULTILINE);
		lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;
		lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
		if (bResult)
		{ // the insert operation success ,must the redraw the window
			// add code : if Caret out of the client area then must
			// Reset Display Position
	       if (AdjustCaretPosition(hWnd)==TRUE)
			{
			    lpShowAddress=NULL;
				  nShowPos=0;
				  nShowLine=0;
				  dwShowFlag=PE_SHOWCLIENT;
			}
			else
			{
				// get redraw start coordinate
	      GetCoordinate(hWnd,lpEditItem,lpShowAddress,&xPos,&nLine,dwStyle&ES_MULTILINE);
				nShowPos=xPos;
				nShowLine=nLine;
			}
/*
      if (bDeleteInvert==TRUE)
      {
		    lpShowAddress=NULL;
				nShowPos=0;
				nShowLine=0;
				dwShowFlag=PE_SHOWCLIENT;
      }
*/
			SendNotificationMsg(hWnd,EN_CHANGE);
			// redraw the window
			hdc=GetDC(hWnd);
	        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,nShowPos,nShowLine,0,dwShowFlag);
			ReleaseDC(hWnd,hdc);
			// reset thw caret
		}
		SetPDACaret(hWnd);
    return 1;
}
/**************************************************
声明：static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 虚键值
	IN lParam -- 保留
返回值：无
功能描述：处理WM_KEYDOWN消息。
引用: 
************************************************/
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
	int nVirtKey ;
//	DWORD lKeyData ;
  int nScrollLine,nWindowLine,iHorzPos;


     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return 0;
	   if (lpMultiItemEditStruct->nItemNum==0) return 0;
	  // get struct EDITITEM data
	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
    if (lpEditItem==NULL) return 0;

		nVirtKey = (int) wParam;    // virtual-key code
//		lKeyData = lParam;          // key data


	    nWindowLine=GetPageLine(hWnd);
        nScrollLine=0;
		iHorzPos=MOTIONLESS;
		switch(nVirtKey)
		{
			case VK_DELETE: // delete char
				DeleteChar(hWnd,lpEditItem);
				return 0;
			case VK_PRIOR:// page up
        nScrollLine=0-nWindowLine;
				break;
			case VK_NEXT:// page down
        nScrollLine=nWindowLine;
				break;
			case VK_END://  to the text end
		iHorzPos=MOVETOEND;
				break;
			case VK_HOME:// to the text start
		iHorzPos=MOVETOHOME;
				break;
			case VK_LEFT:// to the left Char
		iHorzPos=MOVETOLEFT;
				break;
			case VK_UP:// to the up line
        nScrollLine=-1;
				break;
			case VK_RIGHT://  to the right Char
		iHorzPos=MOVETORIGHT;
				break;
			case VK_DOWN:// to the line down
        nScrollLine=1;
				break;
			case VK_INSERT: // Insert char
				{
/*					BYTE lpKeyState[256];
					int iShiftState,iCtrlState;
					iShiftState=GetKeyState(VK_SHIFT);
					iCtrlState=GetKeyState(VK_CONTROL);
					if ((iCtrlState&0x8000)&&(iShiftState&0x8000)==0)
						DoCopy(hWnd,0,0);
					if ((iCtrlState&0x8000)==0&&(iShiftState&0x8000))
						DoPaste(hWnd,0,0);*/
					return 0;
				}
			default :
				return 0;
		};
//    return VScrollMultiItemEdit(hWnd,nScrollLine);
     MoveCaret(hWnd,nScrollLine,iHorzPos);
     return 1;
}
/**************************************************
声明：static BOOL MoveCaret(HWND hWnd,int nScrollLine,int iHorzPos)
参数：
	IN hWnd -- 窗口句柄
	IN nScrollLine -- 要滚动的行数
	IN iHorzPos -- 水平滚动的标志
返回值：无
功能描述：移动光标
引用: 
************************************************/
static BOOL MoveCaret(HWND hWnd,int nScrollLine,int iHorzPos)
{
//  HWND hActiveItemWnd;
  int nCaretLine,xPos,nItemCaretLine;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
  int nTextHeight;
  int iActiveItem;
//  RECT rcClientRect;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;


	lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到条目结构

	if (nScrollLine==0)
	{
		if (iHorzPos!=MOTIONLESS)
		{ // 水平移动光标
			HorzMoveCaret(hWnd,lpEditItem,iHorzPos);
		}
		return TRUE;
	}
	// 得到光标坐标位置
    xPos=lpEditItem->nCaretx;
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpEditItem->nCaretLine;  // 得到光标所在行

    nCaretLine+=nScrollLine; // 滚动光标


    if (nCaretLine<0) nCaretLine=0;
    if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
      nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1;

// get new active item
    for (iActiveItem=0;iActiveItem<lpMultiItemEditStruct->nItemNum;iActiveItem++)
    {
      if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[iActiveItem]&&
        nCaretLine<lpMultiItemEditStruct->nItemStartLineList[iActiveItem+1])
        break;
    }
    // set active item is last item
    if (iActiveItem>=lpMultiItemEditStruct->nItemNum) iActiveItem=lpMultiItemEditStruct->nItemNum-1;

    nItemCaretLine=nCaretLine-lpMultiItemEditStruct->nItemStartLineList[iActiveItem]; // 得到光标在指定条目中的行索引

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到激活条目
    ClearInvert(hWnd,lpEditItem,TRUE);  // 清除选择

    lpMultiItemEditStruct->iActiveItem=iActiveItem; // 设置新的激活条目

    nTextHeight=GetTextHeight(hWnd);
	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到激活条目结构
		SetPDACaretPosition(hWnd,lpEditItem,xPos,(int)(nItemCaretLine*nTextHeight)); // 设置光标
    if (AdjustCaretPosition(hWnd)==TRUE)  // 调整光标位置
		{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
//				InvalidateRect(hWnd,NULL,TRUE);
		}

		// set caret position
		SetPDACaret(hWnd); 

    // show caret
    ShowPDACaret(hWnd);
	SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // 通知父窗口

//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,nItemCaretLine,xPos);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
    return TRUE;
}
/**************************************************
声明：static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回垂直滚动条滚动块的位置
功能描述：得到垂直滚动条滚动块的位置，处理EM_GETTHUMB消息。
引用: 
************************************************/
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
		if (!(dwStyle&WS_VSCROLL)) return 0;
    wParam++;
    lParam++;
		return Edit_GetScrollPos(hWnd,SB_VERT); // 得到滑块位置
}
/**************************************************
声明：static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- LPDWORD ， 存放选择区域的起始位置字符索引
	IN lParam -- LPDOWRD ， 存放选择区域的结束位置字符索引
返回值：成功返回MAKERESULT(START,END),否则返回0。
功能描述：得到编辑区的选择区域，处理EM_GETSEL消息。
引用: 
************************************************/
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 int nStart , nEnd;
	 LPDWORD lpdwStart,lpdwEnd;
	 LRESULT lResult;



    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  // Get struct EDITITEM data
	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // 映射指针
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif

		lpdwStart=(LPDWORD)wParam;  // 得到缓存
		lpdwEnd=(LPDWORD)lParam;
		if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
			return -1;
		nStart=lpEditItem->lpInvertStart-lpEditItem->lpPDAEditBuffer; // 得到选择位置
		nEnd=lpEditItem->lpInvertEnd-lpEditItem->lpPDAEditBuffer;
		if (lpdwStart)
			*lpdwStart=(DWORD)nStart;
		if (lpdwEnd)
			*lpdwEnd=(DWORD)nEnd;
		lResult=MAKELRESULT(nStart,nEnd);
		return lResult;
}

/**************************************************
声明：static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT ，选择区域起始位置
	IN lParam -- INT ，选择区域结束位置
返回值：
功能描述：设置选择区域，处理EM_SETSEL消息。
引用: 
************************************************/
static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   int nStart , nEnd;



    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到条目结构

		nStart=(int)wParam;
		nEnd=(int)lParam;

    return SetSel(hWnd,lpEditItem,nStart,nEnd);  // 设置选择
}

/**************************************************
声明：static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTSTR 要设置的文本
返回值：成功返回TRUE，否则返回FALSE。
功能描述：设置激活编辑区文本，处理WM_SETTEXT消息。
引用: 
************************************************/
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPCTSTR lpsz;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;

    lpsz= (LPCTSTR)lParam; // address of window-text string
    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到激活条目结构

    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)  // 设置文本
		return FALSE;
	AdjustCaretPosition(hWnd); // 调整光标
	SetPDACaret(hWnd); // 设置光标
	InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
	return TRUE;
}

/**************************************************
声明：static LRESULT DoSetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- LPCTSTR 要设置的文本指针
返回值：
功能描述：设置指定条目的文本，处理EM_SETITEMTEXT消息。
引用: 
************************************************/
static LRESULT DoSetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPCTSTR lpsz;
   int nItem;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return FALSE;

		nItem=(int)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
    lpsz= (LPCTSTR)lParam; // address of window-text string
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到条目结构

//    return SetText(hWnd,lpEditItem,lpsz);
    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)  // 设置文本
		return FALSE;
	AdjustCaretPosition(hWnd);  // 调整光标
	SetPDACaret(hWnd); // 设置光标
	InvalidateRect(hWnd,NULL,TRUE);
	return TRUE;
}

/**************************************************
声明：static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 缓存的大小
	IN lParam -- LPTSTR 存放文本的缓存
返回值：成功返回拷贝字符的个数，否则返回0
功能描述：得到激活编辑区的文本，处理WM_GETTEXT消息。
引用: 
************************************************/
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPTSTR lpsz;
   int cMaxlen;
//   int nItem;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return 0;


			lpsz= (LPTSTR)lParam; // address of window-text string
			cMaxlen=(int)(wParam);

	        lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到激活条目结构

			// !!! modified by jami chen in 2004.05.27
//			if (cMaxlen<lpEditItem->cbEditLen)
//				return FALSE;
//		    GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
//			return lpEditItem->cbEditLen;

			if (cMaxlen>lpEditItem->cbEditLen)
				cMaxlen = lpEditItem->cbEditLen + 1; // +1 is read end code 'null'
			// 得到编辑文本
		    GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,cMaxlen); // +1 is read end code 'null'
			return cMaxlen;// lpEditItem->cbEditLen;  返回实际复制数据的大小
			// !!! modified by jami chen
}
/**************************************************
声明：static LRESULT DoGetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- HIWORD 指定缓存的大小
				 LOWORD 指定条目
	IN lParam -- LPTSTR 存放数据的缓存
返回值：成功返回拷贝字符的个数，否则返回0
功能描述：得到指定条目编辑区的文本，处理EM_GETITEMTEXT消息。
引用: 
************************************************/
static LRESULT DoGetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPTSTR lpsz;
   int cMaxlen;
   int nItem;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return 0;


#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射句柄
#endif
		lpsz= (LPTSTR)lParam; // address of window-text string
		cMaxlen=(int)HIWORD(wParam);
		nItem=(int)LOWORD(wParam);

		  if (nItem>=lpMultiItemEditStruct->nItemNum)
			  return 0;
		  if (nItem<0)
			  return 0;

      lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构句柄
			if (cMaxlen<lpEditItem->cbEditLen)
				return FALSE;
			// 得到文本
		  GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
			return lpEditItem->cbEditLen;
}
/**************************************************
声明：static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回激活编辑区的文本长度。
功能描述：得到激活编辑区的文本长度，处理WM_GETTEXTLENGTH消息。
引用: 
************************************************/
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return 0;

 	  // Get struct EDITITEM data
      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // 得到条目结构
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;  // 得到文本长度
}
/**************************************************
声明：static LRESULT DoGetItemTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- 保留
返回值：返回指定条目编辑区的文本长度。
功能描述：得到指定条目编辑区的文本长度，处理EM_GETITEMTEXTLENGTH消息。
引用: 
************************************************/
static LRESULT DoGetItemTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   int nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
      if (lpMultiItemEditStruct==NULL) return 0;

      nItem=(int)wParam;

		  if (nItem>=lpMultiItemEditStruct->nItemNum)
			  return 0;
		  if (nItem<0)
			  return 0;

			// Get struct EDITITEM data
      lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;  // 得到文本长度
}
/**************************************************
声明：static LRESULT DoSetItemCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- EDITCALLBACK * 回调函数的指针
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置指定条目的回调函数指针，处理EM_SETITEMCALLBACK消息。
引用: 
************************************************/
static LRESULT DoSetItemCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   int nItem;
   EDITCALLBACK *lpCallBack;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return 0;


#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
	  lpCallBack= (EDITCALLBACK *)lParam; // address of window-text string
      nItem=(int)LOWORD(wParam);

	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
	  if (nItem<0)
		  return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // 得到条目结构
	  lpEditItem->lpCallBack=lpCallBack;  // 设置回调函数
	  return TRUE;
}
/**************************************************
声明：static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- 指向EDITITEMPROPERTY的指针，条目的属性
返回值：成功返回0，否则返回-1
功能描述：在指定条目下插入一个条目，处理EM_INSERTITEM消息。
引用: 
************************************************/
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPEDITITEMPROPERTY lpEditItemProperty;
   int nItem,i;
   int iItemStartPos;
   void *lpTemp;
   int nTextHeigth,nClientLine;
   RECT ClientRect;
   char *pSrc,*pObj;
   int iMoveLen;

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);

     nItem=(int)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
     lpEditItemProperty=(LPEDITITEMPROPERTY)lParam; // 得到条目属性

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return -1;

     lpMultiItemEditStruct->nItemNum++;  // 增加一个条目

     // 分配条目结构
     if (lpMultiItemEditStruct->lpItemList)
       lpTemp=realloc(lpMultiItemEditStruct->lpItemList,(lpMultiItemEditStruct->nItemNum)*sizeof(EDITITEM));
     else
       lpTemp=malloc((lpMultiItemEditStruct->nItemNum)*sizeof(EDITITEM));
     if (lpTemp==NULL)
     {
			 SendNotificationMsg(hWnd,EN_ERRSPACE);
       return -1;
     }
     lpMultiItemEditStruct->lpItemList=lpTemp;

     // 分配条目开始位置列表
     if (lpMultiItemEditStruct->nItemStartLineList)
        lpTemp=(void *)realloc(lpMultiItemEditStruct->nItemStartLineList,(lpMultiItemEditStruct->nItemNum+1)*sizeof(int));
     else
        lpTemp=(void *)malloc((lpMultiItemEditStruct->nItemNum+1)*sizeof(int));
     if (lpTemp==NULL)
     {
			 SendNotificationMsg(hWnd,EN_ERRSPACE);
       return -1;
     }
     lpMultiItemEditStruct->nItemStartLineList=lpTemp;

/*     if (nItem)
     {
       lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem-1];
       iItemStartPos=lpEditItem->iStartPos+lpEditItem->rect.bottom;
     }
     else
     {
       iItemStartPos=0;
     }*/
     if (nItem>=(lpMultiItemEditStruct->nItemNum-1))
     { // 加入到最后一个条目
        nItem=lpMultiItemEditStruct->nItemNum-1;
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
        iItemStartPos=lpMultiItemEditStruct->nContextBottomPos;
     }
     else
     {  // 插入到指定条目位置
         lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到指定条目的结构
         iItemStartPos=lpEditItem->iStartPos; // 得到开始位置
         // 将该条目移动到下一个条目位置
         pSrc=(char *)lpEditItem;
         pObj=pSrc+sizeof(EDITITEM);
         iMoveLen=(lpMultiItemEditStruct->nItemNum-nItem-1)*sizeof(EDITITEM);
         memmove(pObj,pSrc,iMoveLen);

         // 将该条目开始位置移动到下一个条目位置
         pSrc=(char *)&lpMultiItemEditStruct->nItemStartLineList[nItem];
         pObj=pSrc+sizeof(int);
         iMoveLen=(lpMultiItemEditStruct->nItemNum-nItem)*sizeof(int);
         memmove(pObj,pSrc,iMoveLen);
     }

     SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos); // 设置当前条目
     lpMultiItemEditStruct->nItemStartLineList[nItem]=iItemStartPos/nTextHeigth;

     iItemStartPos+=(int)lpEditItem->rect.bottom;
     for (i=nItem+1;i<lpMultiItemEditStruct->nItemNum;i++)
     { // 重新调整开始位置
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(int)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine); // 滚动窗口
//     AdjustCaretPosition(hWnd);
		 SetPDACaretPosition(hWnd,lpEditItem,0,0); // 设置光标位置
		 SetPDACaret(hWnd); // 设置光标
     InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
     return 0;
}
/**************************************************
声明：static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- 指向EDITITEMPROPERTY的指针，条目的属性
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置指定条目的属性，处理EM_SETITEM消息。
引用: 
************************************************/
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   EDITITEM OldEditItem;
   LPEDITITEMPROPERTY lpEditItemProperty;
   int nItem,i;
   int iItemStartPos;
//   void *lpTemp;
   int nTextHeigth,nClientLine;
   RECT ClientRect;
//   char *pSrc,*pObj;
//   int iMoveLen;

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect); // 得到客户矩形

     nItem=(int)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
     lpEditItemProperty=(LPEDITITEMPROPERTY)lParam; // 得到条目属性

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return FALSE;

     if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<0)
        return FALSE;
     lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到条目结构
     OldEditItem=*lpEditItem;
     iItemStartPos=lpEditItem->iStartPos;
     if (SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos)==FALSE) // 设置条目
     {
         *lpEditItem=OldEditItem;
         return FALSE;
     }

     // 释放原来分配的指针
     if (OldEditItem.lpPDAControlBuffer)
       free(OldEditItem.lpPDAControlBuffer);
     if (OldEditItem.lpPDASaveEditBuffer)
       free(OldEditItem.lpPDASaveEditBuffer);
     if (OldEditItem.lpFormat)
       free(OldEditItem.lpFormat);

     // 重新调整开始位置
     iItemStartPos+=(int)lpEditItem->rect.bottom;
     for (i=nItem+1;i<lpMultiItemEditStruct->nItemNum;i++)
     {
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(int)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     // 滚动窗口
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
     AdjustCaretPosition(hWnd); // 调整光标
     InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
     return TRUE;
}

/**************************************************
声明：static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- 保留
返回值：成功返回TRUE，否则返回FALSE
功能描述：删除指定条目，处理EM_DELETEITEM消息。
引用: 
************************************************/
static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   EDITITEM OldEditItem;
   int nItem,i;
   int iItemStartPos;
//   void *lpTemp;
   int nTextHeigth,nClientLine;
   RECT ClientRect;
//   char *pSrc,*pObj;
//   int iMoveLen;

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);

     nItem=(int)wParam;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
     if (lpMultiItemEditStruct==NULL) return FALSE;

     if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<0)
        return FALSE;
     OldEditItem=lpMultiItemEditStruct->lpItemList[nItem]; // 得到原来的条目结构
     iItemStartPos=(int)OldEditItem.iStartPos;
     lpMultiItemEditStruct->nItemNum--;
     for (i=nItem;i<lpMultiItemEditStruct->nItemNum;i++)
     { // 删除该条目
        lpMultiItemEditStruct->lpItemList[i]=lpMultiItemEditStruct->lpItemList[i+1];
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(int)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     // 设置滚动条
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
     InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
/*     lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     OldEditItem=*lpEditItem;
     iItemStartPos=lpEditItem->iStartPos;
     if (SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos)==FALSE)
     {
         *lpEditItem=OldEditItem;
         return FALSE;
     }
*/
     // 释放指针
     if (OldEditItem.lpPDAControlBuffer)
       free(OldEditItem.lpPDAControlBuffer);
     if (OldEditItem.lpPDASaveEditBuffer)
       free(OldEditItem.lpPDASaveEditBuffer);
     if (OldEditItem.lpFormat)
       free(OldEditItem.lpFormat);

     return TRUE;
}
/**************************************************
声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTLCOLORSTRUCT，要设置的颜色值
返回值：无
功能描述：设置窗口颜色，处理WM_SETCTLCOLOR消息。
引用: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
//	COLORREF cl_NewColor;
	LPCTLCOLORSTRUCT lpCtlColor;
		
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

		 lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
		 if (lpMultiItemEditStruct==NULL) 
			 return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // 设置文本颜色
			 lpMultiItemEditStruct->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景
				lpMultiItemEditStruct->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // 设置选择文本颜色
				lpMultiItemEditStruct->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // 设置选择文本背景色
				lpMultiItemEditStruct->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // 设置DISABLE 文本颜色
				lpMultiItemEditStruct->cl_DisableText = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // 设置DIASBLE 文本背景色
				lpMultiItemEditStruct->cl_DisableBkColor = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 { // 设置只读文本颜色
				lpMultiItemEditStruct->cl_ReadOnly = lpCtlColor->cl_ReadOnly;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 { // 设置只读文本背景色
				lpMultiItemEditStruct->cl_ReadOnlyBk  = lpCtlColor->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 设置标题文本颜色
				lpMultiItemEditStruct->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // 设置标题文本背景色
				lpMultiItemEditStruct->cl_TitleBk = lpCtlColor->cl_TitleBk ;
		 }
/*		dwColorType = (UINT)wParam;
		cl_NewColor = (COLORREF)lParam;
		switch(dwColorType)
		{
			case NORMALTEXTCOLOR:
				lpMultiItemEditStruct->cl_NormalText = cl_NewColor;
				break;
			case NORMALTEXTBKCOLOR:
				lpMultiItemEditStruct->cl_NormalBkColor = cl_NewColor;
				break;
			case NORMALINVERTCOLOR:
				lpMultiItemEditStruct->cl_InvertText = cl_NewColor;
				break;
			case NORMALINVERTBKCOLOR:
				lpMultiItemEditStruct->cl_InvertBkColor = cl_NewColor;
				break;
			case NORMALDISABLECOLOR:
				lpMultiItemEditStruct->cl_DisableText = cl_NewColor;
				break;
			case NORMALDISABLEBKCOLOR:
				lpMultiItemEditStruct->cl_DisableBkColor = cl_NewColor;
				break;
			default:
				return FALSE;
		}
*/
		 InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
/**************************************************
声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTLCOLORSTRUCT，存放得到的颜色值
返回值：成功返回TRUE，否则返回FALSE
功能描述：得到当前窗口的颜色，处理WM_GETCTLCOLOR消息。
引用: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
		
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPCTLCOLORSTRUCT lpCtlColor;

		 lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
		 if (lpMultiItemEditStruct==NULL) 
			return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // 得到文本颜色
			 lpCtlColor->cl_Text = lpMultiItemEditStruct->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 得到文本背景色
				lpCtlColor->cl_TextBk = lpMultiItemEditStruct->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // 得到选择文本颜色
				lpCtlColor->cl_Selection = lpMultiItemEditStruct->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // 得到选择文本背景色
				lpCtlColor->cl_SelectionBk = lpMultiItemEditStruct->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // 得到DISABLE文本颜色
				lpCtlColor->cl_Disable= lpMultiItemEditStruct->cl_DisableText ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // 得到DISABLE文本背景色
				lpCtlColor->cl_DisableBk = lpMultiItemEditStruct->cl_DisableBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 { // 得到只读文本颜色
				lpCtlColor->cl_ReadOnly= lpMultiItemEditStruct->cl_ReadOnly ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 { // 得到只读文本背景色
				lpCtlColor->cl_ReadOnlyBk = lpMultiItemEditStruct->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 得到标题文本颜色
				lpCtlColor->cl_Title= lpMultiItemEditStruct->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // 得到标题文本背景色
				lpCtlColor->cl_TitleBk = lpMultiItemEditStruct->cl_TitleBk ;
		 }
		 return TRUE;
/*		dwColorType = (UINT)wParam;
		switch(dwColorType)
		{
			case NORMALTEXTCOLOR:
				return lpMultiItemEditStruct->cl_NormalText;
			case NORMALTEXTBKCOLOR:
				return lpMultiItemEditStruct->cl_NormalBkColor;
			case NORMALINVERTCOLOR:
				return lpMultiItemEditStruct->cl_InvertText;
			case NORMALINVERTBKCOLOR:
				return lpMultiItemEditStruct->cl_InvertBkColor;
			case NORMALDISABLECOLOR:
				return lpMultiItemEditStruct->cl_DisableText;
			case NORMALDISABLEBKCOLOR:
				return lpMultiItemEditStruct->cl_DisableBkColor;
			default:
				break;
		}
*/
}


/**************************************************
声明：static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,int iItemStartPos)
参数：
	IN hWnd -- 窗口句柄
	IN lpEdititem -- 编辑条目结构指针
	IN lpEditItemProperty -- 编辑条目属性结构指针
	IN iItemStartPos -- 条目开始位置
返回值：成功返回TRUE，否则返回FALSE
功能描述：设置条目属性。
引用: 
************************************************/
static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,int iItemStartPos)
{
   RECT ClientRect;
   int nTextHeigth;
   int nFormatLen;
   LPTSTR lpTitle,lpItemText,lpFormat;

// !!! Add By Jami chen in 2003.07.31
#ifdef _MAPPOINTER
	lpTitle = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpTitle, GetCallerProcess() );  // 映射指针
	lpItemText = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpItemText, GetCallerProcess() ); 
	lpFormat = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpFormat, GetCallerProcess() ); 
#else
	lpTitle = lpEditItemProperty->lpTitle;  // 映射指针
	lpItemText = lpEditItemProperty->lpItemText; 
	lpFormat = lpEditItemProperty->lpFormat; 
#endif
// !!! Add End By Jami chen in 2003.07.31

      nTextHeigth=GetTextHeight(hWnd); // 得到文本高度
      GetClientRect(hWnd,&ClientRect); // 得到客户矩形
	   // get the text limit
	   if (lpEditItemProperty->cLimited!=0)
		   lpEditItem->cbEditLimitLen=lpEditItemProperty->cLimited;
	   else
		   lpEditItem->cbEditLimitLen=INITIALEDITSIZE;

	   // get the edit title text
	   if (lpEditItemProperty->lpTitle!=NULL)
// !!! Modified By Jami chen in 2003.07.31
//		   lpEditItem->cbTitleLen=strlen(lpEditItemProperty->lpTitle);
		   lpEditItem->cbTitleLen=strlen(lpTitle);
// !!! Modified End  By Jami chen in 2003.07.31
	   else
		   lpEditItem->cbTitleLen=0;

     lpEditItem->cbEditBufferLen=lpEditItem->cbEditLimitLen+1;

     lpEditItem->cbControlBufferLen=lpEditItem->cbEditBufferLen+lpEditItem->cbTitleLen;
     // allocate memory
     lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen); // 分配编辑缓存
	 memset(lpEditItem->lpPDAControlBuffer,0,lpEditItem->cbControlBufferLen); // 

     if (lpEditItem->lpPDAControlBuffer==NULL)
     {  // allocate failare then free struct EDITITEM and return FALSe
//	      free(lpEditItem);
	      lpEditItem->cbEditBufferLen=0;
	      lpEditItem->cbControlBufferLen=0;
//	      SendNotificationMsg(hWnd,EN_ERRSPACE);
	      return FALSE;
     }

     lpEditItem->lpPDASaveEditBuffer=(LPTSTR)malloc(lpEditItem->cbEditBufferLen);
	 memset(lpEditItem->lpPDASaveEditBuffer,0,lpEditItem->cbEditBufferLen);
     if (lpEditItem->lpPDASaveEditBuffer==NULL)
     {  // allocate failare then free struct EDITITEM and return FALSe
	      free(lpEditItem->lpPDAControlBuffer);
//	      free(lpEditItem);
	      lpEditItem->cbEditBufferLen=0;
	      lpEditItem->cbControlBufferLen=0;
//	      SendNotificationMsg(hWnd,EN_ERRSPACE);
	      return FALSE;
     }

     lpEditItem->lpPDAEditBuffer=lpEditItem->lpPDAControlBuffer+lpEditItem->cbTitleLen;
     lpEditItem->dwStyle=lpEditItemProperty->dwItemStyle|ES_AUTOEXTEND|ES_MULTIITEMEDIT;

     // clear struct EDITITEM
     ClearEditItemStruct(lpEditItem);

     // read control title text
     if (lpEditItem->cbTitleLen)
// !!! Modified By Jami chen in 2003.07.31
//	      memcpy(lpEditItem->lpPDAControlBuffer,lpEditItemProperty->lpTitle, lpEditItem->cbTitleLen );		// Read Text Success
	      memcpy(lpEditItem->lpPDAControlBuffer,lpTitle, lpEditItem->cbTitleLen );		// Read Text Success
// !!! Modified End By Jami chen in 2003.07.31

     // read Window text
// !!! Modified By Jami chen in 2003.07.31
//     if (lpEditItemProperty->lpItemText!=NULL)
     if (lpItemText!=NULL)
// !!! Modified By Jami chen in 2003.07.31
     {
// !!! Modified By Jami chen in 2003.07.31
//	       lpEditItem->cbEditLen=strlen(lpEditItemProperty->lpItemText);
	       lpEditItem->cbEditLen=strlen(lpItemText);
// !!! Modified By Jami chen in 2003.07.31
	       //  the window text can put the default buffer???
	       if (lpEditItem->cbEditLen>lpEditItem->cbEditLimitLen)
	       {
		         lpEditItem->cbEditLen=0;
		         SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1));
	       }
         else
         {
// !!! Modified By Jami chen in 2003.07.31
//	          SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItemProperty->lpItemText,(int)(lpEditItem->cbEditLen+1));
	          SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpItemText,(int)(lpEditItem->cbEditLen+1));
// !!! Modified By Jami chen in 2003.07.31
         }
	       if (lpEditItem->dwStyle&ES_PASSWORD)
	       {
		         lpEditItem->dwStyle&=~ES_MULTILINE;
		         // read Window text
		         FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
	       }
	       else
	       {
//		         GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItemProperty->lpItemText,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
		         GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
		       }
//	       SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItemProperty->lpItemText,(int)(lpEditItem->cbEditLen+1));
     }
     else
     {
	       lpEditItem->cbEditLen=0;
	       SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1));
     }
     // Read Text Success

     // get edit format
     lpEditItem->lpFormat=NULL;
     if (lpEditItem->dwStyle&ES_FORMAT)
     {
// !!! Modified By Jami chen in 2003.07.31
//	      if (lpEditItemProperty->lpFormat!=NULL)
	      if (lpFormat!=NULL)
// !!! Modified By Jami chen in 2003.07.31
	      {
		       lpEditItem->dwStyle&=~ES_MULTILINE;
// !!! Modified By Jami chen in 2003.07.31
//		       nFormatLen=strlen(lpEditItemProperty->lpFormat)+1;
		       nFormatLen=strlen(lpFormat)+1;
// !!! Modified By Jami chen in 2003.07.31
		       if (nFormatLen)
		       {
			        lpEditItem->lpFormat=(LPTSTR)malloc(nFormatLen);
			        if (lpEditItem->lpFormat)
// !!! Modified By Jami chen in 2003.07.31
//				         memcpy(lpEditItem->lpFormat,lpEditItemProperty->lpFormat, nFormatLen );		// Read Text Success
				         memcpy(lpEditItem->lpFormat,lpFormat, nFormatLen );		// Read Text Success
// !!! Modified By Jami chen in 2003.07.31
			        else
				         SendNotificationMsg(hWnd,EN_ERRSPACE);
		       }
	      }
	      else
	      {
		       lpEditItem->dwStyle&=~ES_FORMAT;
	      }
     }
	 // 得到条目位置
     lpEditItem->iStartPos=iItemStartPos;
     lpEditItem->rect.top=0;
     lpEditItem->rect.left=0;
     lpEditItem->rect.right=ClientRect.right;
	 lpEditItem->bNeedCalcTotalLine = TRUE;
     lpEditItem->nTotalLine=GetTotalLine(hWnd,lpEditItem);
	 lpEditItem->bNeedCalcTotalLine = FALSE;
     lpEditItem->rect.bottom=lpEditItem->rect.top+lpEditItem->nTotalLine*nTextHeigth;
	 lpEditItem->lpCallBack=NULL;
     return TRUE;
//   SetWindowPos(hWnd,0,0,0,ClientRect.right,ClientRect.top+nTextHeight*nTotalLine,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOZORDER);

//   GetClientRect(hItemWnd,&rect);
//   lpMultiItemEditStruct->hItemWndList[i]=hItemWnd;
//     lpMultiItemEditStruct->nItemStartLineList[i]=iNextWndPos/nTextHeigth;
//   lpMultiItemEditStruct->nItemStartPosList[i]=iNextWndPos;
//     iNextWndPos+=(int)lpEditItem->rect.bottom;
}
/************************************************************************************/
/**************************************************
声明：int GetItem(HWND hWnd,int xPos,int yPos)
参数：
	IN hWnd -- 窗口句柄
	IN xPos -- x坐标
	IN yPos -- y坐标
返回值：返回条目索引
功能描述：得到指定坐标的条目索引。
引用: 
************************************************/
int GetItem(HWND hWnd,int xPos,int yPos)
{
 LPMULTIITEMEDITSTRUCT   lpMultiItemEditStruct;
 int nItem;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if(lpMultiItemEditStruct==NULL) return -1;
    if (lpMultiItemEditStruct->nItemNum==0) return -1;

    for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    { // 得到指定位置的条目
      if (yPos<lpMultiItemEditStruct->lpItemList[nItem].iStartPos)
        return nItem-1;
    }
    return nItem-1;

}

/**************************************************
声明：static LRESULT DoGetActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：返回激活条目的索引
功能描述：得到激活条目的索引，处理EM_GETACTIVEITEM消息。
引用: 
************************************************/
static LRESULT DoGetActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPMULTIITEMEDITSTRUCT   lpMultiItemEditStruct;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
	    if(lpMultiItemEditStruct==NULL) 
			return -1;
		if (lpMultiItemEditStruct->nItemNum==0) 
			return -1;
		return lpMultiItemEditStruct->iActiveItem; // 返回激活条目索引
}

/**************************************************
声明：static LRESULT DoGetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- 保留
返回值：返回指定条目的风格
功能描述：得到指定条目的风格，处理EM_GETITEMSTYLE消息。
引用: 
************************************************/
static LRESULT DoGetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;
		if (nItem < 0 || nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
	    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到条目结构
		return lpEditItem->dwStyle; // 返回条目风格
}
/**************************************************
声明：static LRESULT DoSetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- INT 指定条目
	IN lParam -- DWORD 要设置的风格
返回值：无
功能描述：设定指定条目的风格，处理EM_SETITEMSTYLE消息。
引用: 
************************************************/
static LRESULT DoSetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
  DWORD dwStyle;
  HDC hdc;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;
		dwStyle = (DWORD)lParam;
		if (nItem < 0 || nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
	    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // 得到条目结构
		lpEditItem->dwStyle = dwStyle; //设置条目风格
		hdc = GetDC(hWnd);
        DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWALL); // 绘制该条目
		ReleaseDC(hWnd,hdc);
		return 0;
}




// ********************************************************************
// 声明：static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd - 应用程序的窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPWINDOWPOS 指针 
// 返回值：
//	
// 功能描述：窗口位置发生改变
// 引用: 
// ********************************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpWindowPos;

		lpWindowPos = (LPWINDOWPOS)lParam;

		if ((lpWindowPos->flags & SWP_NOSIZE) == 0)
		{
			// 窗口大小发生改变
			DoSize(hWnd,0,0);
		}

		return 0;
}

// **************************************************
// 声明：static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	OUT wParam -- LPINT 指向INT的指针，存放光标所在的行。
// 	OUT lParam -- LPINT 指向INT的指针，存放光标所在的X坐标。
// 返回值：无
// 功能描述：得到当前光标的位置，处理EM_GETCARETPOS消息。
// 引用: 
// **************************************************
static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nCaretLine;
  LPEDITITEM lpActiveEditItem;
  int *lpnLine,*lpxPos;

#ifdef _MAPPOINTER
		wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // 映射指针
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif


        lpnLine=(int *)wParam;
        lpxPos=(int *)lParam;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // 得到编辑窗口数据
		if (lpMultiItemEditStruct==NULL) return ;
		if (lpMultiItemEditStruct->nItemNum==0) return ;  // 没有条目
		lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // 得到激活条目
		nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpActiveEditItem->nCaretLine; // 得到光标的位置

		if (lpnLine)
			*lpnLine=nCaretLine;  // 得到光标的行数
		if (lpxPos)
			*lpxPos=lpActiveEditItem->nCaretx+lpActiveEditItem->nDisplayx;  // 得到光标的水平位置
}
