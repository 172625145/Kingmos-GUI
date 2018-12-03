/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：编辑类
版本号：1.0.0.456
开发时期：2001-12-26
作者：Jami chen 陈建明
修改记录：
  2004.07.19 增加消息功能WM_SYSCOLORCHANGE, 用于在在系统颜色重新
			 设置后，需要重新装载新的颜色
**************************************************/
//#include "StdAfx.h"
//#include <ctype.h>
#include "eframe.h"
//#include "Resource.h"
//#include <memory.h>
//#include <wingdi.h>
#include <estring.h>

//#include "Show.h"
#include "eedit.h"
#include "eeBase.h"
#include "eclipbrd.h"
//#include <gwmesrv.h>


// define const 
#define EDIT classEDIT
static const char classEDIT[]="Edit";

#define DOT '.'

//#define _MAPPOINTER

//#define INITIALEDITSIZE 64
#define INITIALEDITSIZE 64
//#define LINEHEIGHT      16
//#define CHINESEWIDTH    16

#define NORMALCOLOR      0
#define INVERTCOLOR      1

/*
#define PE_SHOWCLIENT         0x00000000
#define PE_SHOWLINEEND     0x00000001
#define PE_SHOWTEXTEND     0x00000002
#define PE_SHOWSPECIALLEN  0x00000003
*/

#define ENTERCODE        0x0D
#define BACKSPACE		 0x08
// static function declare area
LRESULT CALLBACK PDAEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
ATOM RegisterEditClass(HINSTANCE hInstance);

static LRESULT DoDeleteChar(HWND hWnd);

static LRESULT InitialPDAEdit(HWND hwnd,LPARAM lParam);  // initial PDAEdit State data
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoSetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoSetxDisplayPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem);

static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetFirstVisibleLine(HWND hWnd);
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam);
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
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSearchString(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void ReleasePDAEdit(HWND hWnd);
static void SetPDAEditVScrollPos(HWND hWnd,int nNewScrollPos);
static void SetPDAEditHScrollPos(HWND hWnd,int nNewScrollPos);

static int VScrollWindowInPDAEdit(HWND hWnd,int nScrollLine);
static void HScrollWindowInPDAEdit(HWND hWnd,int nHScrollPos);

static char * ab_strstr( const char * str1, const char * str2 );



// **************************************************
// 声明：ATOM RegisterPDAEditClass(HINSTANCE hInstance)
// 参数：
// 	IN hInstance -- 实例句柄
// 返回值：返回系统注册结果
// 功能描述：注册编辑类
// 引用: 
// **************************************************
ATOM RegisterPDAEditClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	// size of the struct 'WNDCLASSEX'
//	wcex.cbSize = sizeof(WNDCLASSEX);

	// the class propertiy
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	// this class window procedure
	wcex.lpfnWndProc	= (WNDPROC)PDAEditProc;
	// no class extra
	wcex.cbClsExtra		= 0;
	// window extra 4 byte to save address
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	// no icon
	wcex.hIcon			= NULL;
	// I-Beam Cursor
	wcex.hCursor		= LoadCursor(NULL, IDC_IBEAM);
	// background color
	wcex.hbrBackground	= 0;
	// no menu
	wcex.lpszMenuName	= NULL; // no menu ,add to future
	// class name
	wcex.lpszClassName	= EDIT;
	// no small icon
//	wcex.hIconSm		= NULL;

	// register this class
	return RegisterClass(&wcex);
}

// **************************************************
// 声明：LRESULT CALLBACK PDAEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN uMsg -- 要处理的消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 返回值：消息处理结果
// 功能描述：处理编辑类消息。
// 引用: 
// **************************************************
LRESULT CALLBACK PDAEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_CREATE:
			// Create a window use this class
			return InitialPDAEdit(hWnd,lParam);  // initial PDAEdit State data
		case WM_COMMAND:
			break;
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
		    DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
	    case WM_SIZE:  // 窗口尺寸改变
 	     return DoSize(hWnd,wParam,lParam);
		case WM_WINDOWPOSCHANGED:
			return DoWindowPosChanged(hWnd,wParam,lParam);
   	    case WM_LBUTTONDOWN:
			// the mouse left button be pressed down
			DoLButtonDown(hWnd,wParam,lParam);
			break;
   	    case WM_MOUSEMOVE:
			// the mouse move
			DoMouseMove(hWnd,wParam,lParam);
			break;
   	    case WM_LBUTTONUP:
			// the mouse left button be released
			DoLButtonUp(hWnd,wParam,lParam);
			break;
		case WM_SETFOCUS:
			// this window get focus
			// Create new caret
			return DoSetFocus(hWnd,wParam,lParam);
		case WM_KILLFOCUS:
			// this window lose foucs
			// kill caret
//      DeletePDACaret(hWnd);
			return DoKillFocus(hWnd,wParam,lParam);
		case WM_CHAR:
			// input char
			DoProcessChar(hWnd,wParam,lParam);
			break;
		case WM_KEYDOWN:
			// have key be pressed
			DoKeyDown(hWnd,wParam,lParam);
			break;
		case WM_DESTROY:
			// close this window
			ReleasePDAEdit(hWnd);
//			PostQuitMessage(0);
			break;
    case EM_GETCARETPOS:  // 得到光标位置
      DoGetCaretPosition(hWnd,wParam,lParam);
	  break;
    case EM_SETCARETPOS: // 设置光标位置
      DoSetCaretPosition(hWnd,wParam,lParam);
      break;
    case MIEM_SETXDISPLAYPOS: // 设置水平方向的显示位置
      DoSetxDisplayPos(hWnd,wParam,lParam);
      break;
    case WM_VSCROLL: // 垂直滚动
      DoVScrollWindow(hWnd,wParam,lParam);
      break;
    case WM_HSCROLL:  // 水平滚动
      DoHScrollWindow(hWnd,wParam,lParam);
      break;
    case EM_CHARFROMPOS:  //得到指定点的字符
      return DoCharFromPos(hWnd,wParam,lParam);
    case EM_GETFIRSTVISIBLELINE: //得到第一个可见行
      return DoGetFirstVisibleLine(hWnd);
		case EM_GETTHUMB: // 得到垂直滚动条滚动块的位置
			return DoGetThumb(hWnd,wParam,lParam);
		case EM_GETLIMITTEXT: // 得到当前编辑区的文本限制
			return DoGetLimitText(hWnd,wParam,lParam);
		case EM_GETLINE: // 得到指定行的数据
			return DoGetLine(hWnd,wParam,lParam);
		case EM_GETLINECOUNT: //得到编辑区的总行数
			return DoGetLineCount(hWnd,wParam,lParam);
		case EM_GETMODIFY: // 得到当前编辑区的修改标志
			return DoGetModify(hWnd,wParam,lParam);
		case EM_GETPASSWORDCHAR:// 得到当前编辑区的密码字符
			return DoGetPasswordChar(hWnd,wParam,lParam);	
		case EM_GETSEL: // 得到编辑区的选择区域
			return DoGetSel(hWnd,wParam,lParam);	
		case EM_LINEFROMCHAR: // 返回指定索引字符的行索引
			return DoLineFromChar(hWnd,wParam,lParam);
		case EM_LINEINDEX: // 得到指定行索引所对应的字符索引
			return DoLineIndex(hWnd,wParam,lParam);
		case EM_LINELENGTH: // 得到指定字符所在行的字符个数
			return DoLinelength(hWnd,wParam,lParam);
		case EM_LINESCROLL: // 滚动窗口
			return DoLineScroll(hWnd,wParam,lParam);
		case EM_POSFROMCHAR: // 得到指定字符的坐标
			return DoPosFromChar(hWnd,wParam,lParam);
		case EM_REPLACESEL: // 用指定的字符串替换选择字符
			return DoReplaceSel(hWnd,wParam,lParam);
		case EM_SCROLL: // 垂直滚动屏幕
			return DoScroll(hWnd,wParam,lParam);
		case EM_SCROLLCARET: // 滚动光标到可见位置
			return DoScrollCaret(hWnd,wParam,lParam);
		case EM_SETLIMITTEXT: // 设置编辑区的文本限制
			return DoSetLimitText(hWnd,wParam,lParam);
		case EM_SETMODIFY: // 设置新的修改标志
			return DoSetModify(hWnd,wParam,lParam);
		case EM_SETPASSWORDCHAR: // 设置新的密码字符
			return DoSetPasswordChar(hWnd,wParam,lParam);
		case EM_SETREADONLY: // 设置编辑区的只读属性
			return DoSetReadOnly(hWnd,wParam,lParam);
		case EM_SETSEL: // 设置选择区域
			return DoSetSel(hWnd,wParam,lParam);
		case EM_EMPTY: // 清空编辑区
			return DoEmpty(hWnd,wParam,lParam);

		case EM_SEARCHSTRING: // 查找字串
			return DoSearchString(hWnd,wParam,lParam);
// Color Control 
//		case EM_SETCOLOR:
//			return DoSetColor(hWnd,wParam,lParam);
//		case EM_GETCOLOR:
//			return DoGetColor(hWnd,wParam,lParam);
		case WM_SETCTLCOLOR	: // 设置窗口颜色
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // 得到当前窗口的颜色
			return DoGetColor(hWnd,wParam,lParam);


		case WM_COPY: // 拷贝选择区域字符到剪切板
			return DoCopy(hWnd,wParam,lParam);
		case WM_PASTE: // 粘帖
			return DoPaste(hWnd,wParam,lParam);
		case WM_CUT: // 剪切
			return DoCut(hWnd,wParam,lParam);
		case WM_CLEAR: // 清除
			return DoClear(hWnd,wParam,lParam);
		case WM_SETTEXT: // 设置编辑区文本
			return DoSetText(hWnd,wParam,lParam);
		case WM_GETTEXT: // 得到编辑区的文本
			return DoGetText(hWnd,wParam,lParam);
		case WM_GETTEXTLENGTH: // 得到编辑区的文本长度
		  return DoGetTextLength(hWnd,wParam,lParam);
		case WM_PRINTCLIENT: // 将编辑区打印到指定的设备
		  DoPrintClient(hWnd,wParam,lParam);
		  break;
		case WM_STYLECHANGED: // 窗口风格改变
		case WM_ENABLE: // 窗口改变
			return DoStyleChanged(hWnd,wParam,lParam);

// !!! Add By Jami chen in 2004.07.19
		case WM_SYSCOLORCHANGE:
			return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.07.19
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

// **************************************************
// 声明：static LRESULT InitialPDAEdit(HWND hWnd,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN lParam -- 指向CREATESTRUCT的指针，可以为NULL。
// 返回值：成功返回0，否则返回-1。
// 功能描述：创建编辑控件
// 引用: 
// **************************************************
static LRESULT InitialPDAEdit(HWND hWnd,LPARAM lParam)  // initial PDAEdit State data
{
    LPEDITITEM lpEditItem;
	LPCREATESTRUCT lpcs;
	LPPDAEDITPROPERTY lpPDAEditProperty;
  DWORD dwStyle;
  int nFormatLen;
	
		lpcs = (LPCREATESTRUCT) lParam; // structure with creation data  

//		RETAILMSG(1,(TEXT("Enter InitialPDAEdit ...\r\n")));
		// Get Edit Window struct PDAEDITPROPERTY data
		lpPDAEditProperty=(LPPDAEDITPROPERTY)lpcs->lpCreateParams;
    dwStyle=lpcs->style;  // 得到窗口风格

    dwStyle&=~ES_MULTIITEMEDIT;  // 清除多条目编辑的风格

		SetWindowLong(hWnd,0,NULL); // 设置窗口扩展为空

		// allocate memory to struct EDITITEM
		lpEditItem=(LPEDITITEM)malloc(sizeof(EDITITEM));
		// allocate failare then return FALSE
		if (lpEditItem==NULL)
		{
///			SendNotificationMsg(hWnd,EN_ERRSPACE);
			return -1;
		}
		// allocate success

		// allocate memory to edit buffer
		// allocate length is INITIALEDITSIAE

		if (lpPDAEditProperty!=NULL)
		{  // 存在创建属性
			// get the text limit
			if (lpPDAEditProperty->cLimited!=0) // 有编辑文本限制
				lpEditItem->cbEditLimitLen=lpPDAEditProperty->cLimited;
			else // 没有设定文本限制
				lpEditItem->cbEditLimitLen=INITIALEDITSIZE;

			// get the edit title text
			if (lpPDAEditProperty->lpTitle!=NULL) // 有标题
				lpEditItem->cbTitleLen=strlen(lpPDAEditProperty->lpTitle); // 得到标题长度
			else // 没有标题文本，设定当前长度为0
				lpEditItem->cbTitleLen=0;
		}
		else
		{ // 没有创建属性，设置默认参数
			lpEditItem->cbEditLimitLen=INITIALEDITSIZE;
			lpEditItem->cbTitleLen=0;
		}

		lpEditItem->cbEditBufferLen=lpEditItem->cbEditLimitLen+1; // 得到缓存大小

		lpEditItem->cbControlBufferLen=lpEditItem->cbEditBufferLen+lpEditItem->cbTitleLen; // 得到控制缓存大小，包含标题
		// allocate memory
		lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen); // 得到控制缓存

		if (lpEditItem->lpPDAControlBuffer==NULL)
		{  // allocate failare then free struct EDITITEM and return FALSe
			lpEditItem->cbEditBufferLen=0;
			lpEditItem->cbControlBufferLen=0;
			free(lpEditItem); // 释放空间
//			SendNotificationMsg(hWnd,EN_ERRSPACE);
		    return -1;
		}

		lpEditItem->lpPDASaveEditBuffer=(LPTSTR)malloc(lpEditItem->cbEditBufferLen);  // 分配备份缓存
		if (lpEditItem->lpPDASaveEditBuffer==NULL)
		{  // allocate failare then free struct EDITITEM and return FALSe
		  lpEditItem->cbEditBufferLen=0;
		  lpEditItem->cbControlBufferLen=0;
		  free(lpEditItem->lpPDAControlBuffer);
		  free(lpEditItem);
//		  SendNotificationMsg(hWnd,EN_ERRSPACE);
		  return -1;
		}

		lpEditItem->lpPDAEditBuffer=lpEditItem->lpPDAControlBuffer+lpEditItem->cbTitleLen;  // 分配编辑缓存
		
		// clear struct EDITITEM
//		Clear(lpEditItem);
    ClearEditItemStruct(lpEditItem);  // 清除编辑条目结构

		// read control title text
		if (lpEditItem->cbTitleLen)
		  memcpy(lpEditItem->lpPDAControlBuffer,lpPDAEditProperty->lpTitle, lpEditItem->cbTitleLen );		// Read Text Success 

		// read Window text
		if (lpcs->lpszName!=NULL)
		{  // 有编辑文本
			lpEditItem->cbEditLen=strlen(lpcs->lpszName); // 得到编辑文本的大小
			//  the window text can put the default buffer ???
			if (lpEditItem->cbEditLen>lpEditItem->cbEditLimitLen)
			{ // 文本长度大于文本限制
				lpEditItem->cbEditLen=0;
				SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1)); // 设置文本为空
			}
			if (dwStyle&ES_PASSWORD)
			{  // 当前是密码显示
				dwStyle&=~ES_MULTILINE;  // 密码只能是单行编辑
				SetWindowLong(hWnd,GWL_STYLE,dwStyle); // 重新设置风格
				// read Window text
				FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord); // 用密码字符填充编辑文本
			}
			else
			{
				GetEditText(lpEditItem->lpPDAEditBuffer,lpcs->lpszName,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null' // 得到编辑文本
			}
			SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpcs->lpszName,(int)(lpEditItem->cbEditLen+1));// 保存当前编辑文本
		}
		else
		{ // 没有编辑文本，
			lpEditItem->cbEditLen=0;
			SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1)); // 设定编辑文本为空
		}
		// Read Text Success

		// get edit format 
		lpEditItem->lpFormat=NULL;  // 设置编辑格式为空
    if (dwStyle&ES_FORMAT)
		{  // 当前编辑为格式编辑
			if (lpPDAEditProperty != NULL && lpPDAEditProperty->lpFormat!=NULL)
			{ // 存在格式编辑
				dwStyle&=~ES_MULTILINE; // 格式编辑只能是当行编辑
				SetWindowLong(hWnd,GWL_STYLE,dwStyle); // 重设风格
				nFormatLen=strlen(lpPDAEditProperty->lpFormat)+1; // 得到格式编辑串的长度
				if (nFormatLen)
				{ 
					lpEditItem->lpFormat=(LPTSTR)malloc(nFormatLen); // 分配格式编辑串缓存
					if (lpEditItem->lpFormat) // 分配成功
						memcpy(lpEditItem->lpFormat,lpPDAEditProperty->lpFormat, nFormatLen );		// Read Text Success 
					else
						SendNotificationMsg(hWnd,EN_ERRSPACE); // 错误
				}
			}
			else
			{ // 不存在格式编辑
				dwStyle&=~ES_FORMAT; // 取消格式编辑
				SetWindowLong(hWnd,GWL_STYLE,dwStyle);
			}
		}
    lpEditItem->dwStyle=dwStyle;  // 得到编辑风格
		lpEditItem->nTotalLine=-1;
		lpEditItem->bNeedCalcTotalLine = TRUE;

/*		
		lpEditItem->cl_NormalText = CL_BLACK;
		lpEditItem->cl_NormalBkColor = CL_WHITE;

		lpEditItem->cl_InvertText = CL_WHITE;
		lpEditItem->cl_InvertBkColor = CL_BLACK;

		lpEditItem->cl_DisableText = CL_LIGHTGRAY;
		lpEditItem->cl_DisableBkColor = CL_WHITE;
*/
		// 设定当前文本的颜色
		lpEditItem->cl_NormalText = GetSysColor(COLOR_WINDOWTEXT);
		lpEditItem->cl_NormalBkColor = GetSysColor(COLOR_WINDOW);

		lpEditItem->cl_InvertText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		lpEditItem->cl_InvertBkColor = GetSysColor(COLOR_HIGHLIGHT);

		lpEditItem->cl_DisableText = GetSysColor(COLOR_GRAYTEXT);
		lpEditItem->cl_DisableBkColor = GetSysColor(COLOR_WINDOW);

		lpEditItem->cl_ReadOnly = lpEditItem->cl_NormalText;
		lpEditItem->cl_ReadOnlyBk = lpEditItem->cl_NormalBkColor;

		lpEditItem->cl_Title = lpEditItem->cl_NormalText;
		lpEditItem->cl_TitleBk = lpEditItem->cl_NormalBkColor;

		// set Initial State to Edit Control
		SetWindowLong(hWnd,0,(LONG)lpEditItem);

		SetPDAEditScrollPage(hWnd);  // 设定滚动条
		lpEditItem->iStartPos=0; // 设定开始位置
		GetClientRect(hWnd,&lpEditItem->rect); // 得到客户去区
		SetWindowRect(hWnd,lpEditItem,FALSE); // 设置窗口大小
		SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);// 设定垂直滚动条的位置
		SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH)); // 设定水平滚动条的位置

		// !!! Add By Jami chen in 2003.08.08
		// set default  Caret position 
		SetCaretToStart(hWnd,lpEditItem);  // Add By Jami chen in 2005.01.28
		SetPDACaretPosition(hWnd,lpEditItem,0,0);  // 设定光标位置
		// !!! Add End By Jami  chen in 2003.08.08

//		RETAILMSG(1,(TEXT("Leave InitialPDAEdit ...\r\n")));
		// initial success ,return TRUE
		return 0;
}
// **************************************************
// 声明：static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置焦点，处理WM_SETFOCUS消息。
// 引用: 
// *************************************************
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	DWORD dwStyle;
    LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // 得到编辑条目结构指针
      if(lpEditItem==NULL) return FALSE;


	  CreatePDACaret(hWnd,lpEditItem);  // 创建光标
//			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
//			if (dwStyle&ES_MULTIITEMEDIT)
//			{
//				hParentWnd=GetParent(hWnd);
	  SendNotificationMsg( hWnd,EN_SETFOCUS); // 发送通知消息
//			}
      SetPDACaret(hWnd);  // 设置光标位置
      // show caret
      ShowPDACaret(hWnd); // 显示光标
      wParam++;
      lParam++;
			return 0;
}

// **************************************************
// 声明：static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：杀死焦点，处理WM_KILLFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);// 得到编辑条目结构指针
      if(lpEditItem==NULL) return FALSE;

// !!! Modified By jami chen in 2005.01.12 
//      ClearInvert(hWnd,lpEditItem,TRUE); // 清除选择
// !!! to  By Jami chen in 2005.01.12
	  if (!(lpEditItem->dwStyle & ES_NOHIDESEL ))
	  {
	      ClearInvert(hWnd,lpEditItem,TRUE); // 清除选择
	  }
// !!! Modified  End
	    DeletePDACaret(hWnd); // 删除光标
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);  // 发送通知消息
      wParam++;
      lParam++;
    	return 0;
}
/*****************************************************************/
/*****************************************************************/
// **************************************************
// 声明：static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：改变窗口大小，处理WM_SIZE消息。
// 引用: 
// **************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  RECT rectNew;
  int nShowPos,nShowLine;
  LPTSTR lpShowAddress;
  DWORD dwStyle;

		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);// 得到编辑条目结构指针
		if(lpEditItem==NULL) return FALSE;
		GetClientRect(hWnd,&rectNew); // 得到客户区域
		lpEditItem->rect.right=rectNew.right; // 设置新的大小
		lpEditItem->rect.bottom=rectNew.bottom; // 设置新的大小

		// !!! Add By Jami chen in 2004.08.21
/*		if (nTotalLine<=nPageLine)
		{ // not a page ,so not need scroll
			nMinPos=0;
			nMaxPos=nPageLine-1;//-nWindowLine+1;
			Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
			EnableScrollBar(hWnd,SB_VERT,ESB_DISABLE_BOTH);
		}
*/

		SetPDAEditScrollPage(hWnd);
		SetPDAEditVScrollRange(hWnd,lpEditItem->nTotalLine);  // 设置垂直滚动条的范围
		SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
		SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		lpShowAddress=lpEditItem->lpCaretPosition;
		GetCoordinate(hWnd,lpEditItem,lpShowAddress,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
		lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
		lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
		AdjustCaretInEditItem(hWnd,lpEditItem);  // 调整光标位置
		SetPDACaret(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
		// !!! Add End By Jami chen in 2004.08.21
		return 0;
}

#ifdef DITHERING_DEAL  // 抖动处理
static int old_x = -1,old_y = -1;
#endif

// **************************************************
// 声明：static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
// 返回值：无
// 功能描述：处理WM_LBUTTONDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int xPos,yPos;
  LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);// 得到编辑条目结构指针
      if(lpEditItem==NULL) return FALSE;


			// Get LButton down Position
			xPos=(int)(short)LOWORD(lParam);
			yPos=(int)(short)HIWORD(lParam);

#ifdef DITHERING_DEAL  // 抖动处理
			old_x = xPos;
			old_y = yPos;
#endif

			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);  // 设置光标位置

			//  is or not get facus???
			if (GetFocus()!=hWnd)  // 得到焦点窗口是否在当前窗口
			{  // if not active then get focus
				SetFocus(hWnd); // 设置焦点窗口
			}

			// Clear Invert and redraw
			ClearInvert(hWnd,lpEditItem,TRUE);  // 清除选择
			//// set caret position
			//SetPDACaret(hWnd);

			// Capture the mouse
			if (GetCapture()!=hWnd)
				SetCapture(hWnd);

			// set Invert Area
			SetInvertArea(hWnd,lpEditItem);

      if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)  // 调整光标位置
			{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
				InvalidateRect(hWnd,NULL,TRUE); // 需要重绘窗口
			}

			// set caret position
			SetPDACaret(hWnd);  // 设置光标

      // show caret
      ShowPDACaret(hWnd);  // 显示光标
      wParam++;
    return 0;
}
// **************************************************
// 声明：static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
// 返回值：无
// 功能描述：处理WM_MOUSEMOVE消息。
// 引用: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int xPos,yPos;
  LPEDITITEM lpEditItem;


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

		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // 得到编辑条目结构指针
		      if(lpEditItem==NULL) return FALSE;


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
			if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
			{
				// set caret position
				SetPDACaret(hWnd);
				// Set Invert Area
				SetInvertArea(hWnd,lpEditItem);
				// get  hdc
//				DrawPDAEditControl(hWnd);
				InvalidateRect(hWnd,NULL,TRUE);
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
  wParam++;
  return 0;
}
// **************************************************
// 声明：static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- MAKELPARAM(x,y),当前鼠标的位置
// 返回值：无
// 功能描述：处理WM_LBUTTONUP消息。
// 引用: 
// **************************************************
static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture 
	SetCapture(NULL);
	wParam++;
	lParam++;	
}
// **************************************************
// 声明：static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：字符输入，处理WM_CHAR消息。
// 引用: 
// **************************************************
static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	TCHAR chCharCode;
	LPEDITITEM lpEditItem;
	DWORD dwStyle;
	BOOL bResult;
	DWORD dwShowFlag = PE_SHOWTEXTEND;
	LPTSTR lpShowAddress;
	int nShowLine,nShowPos;
	HDC hdc;
	int xPos,nLine;
	int bDeleteResult;
  BOOL bDeleteInvert = FALSE; 

		// get window style
	   dwStyle=GetWindowLong(hWnd,GWL_STYLE);
     if (dwStyle&ES_READONLY) return;
	   // get struct EDITITEM data
		 lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
     if (lpEditItem==NULL) return;
	   // get will be redrawed start address
	   lpShowAddress=lpEditItem->lpCaretPosition;

     // get input char code
		chCharCode=(TCHAR)wParam;
		if (chCharCode==ENTERCODE)
		{ // the input char is enter code
			if (PickCharacter(0x00,chCharCode,dwStyle)==FALSE) return;
   		//Delete Invert Area
      if (DeleteInvert(hWnd,lpEditItem))
      {
        SetCaretCoordinate(hWnd,lpEditItem);  // 设置光标的坐标
      	AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标位置
        bDeleteInvert=TRUE;
      }
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
//      	AdjustCaretInEditItem(hWnd,lpEditItem);
        bDeleteInvert=TRUE;
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
					return ;
				}
				else
				{ // the input char is chinese low byte
					lpEditItem->LoByte=chCharCode;

					if (PickCharacter(lpEditItem->HiByte,lpEditItem->LoByte,dwStyle)==FALSE) return;
					//Delete Invert Area
		            if (DeleteInvert(hWnd,lpEditItem))
		            {
				         SetCaretCoordinate(hWnd,lpEditItem);  // 设定光标的坐标
		                 AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标的位置
		             	bDeleteInvert=TRUE;
		            }
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
			{  // 当前字符是ASCII
				lpEditItem->HalfByte=FALSE;  // 不需要等待半个字符
				if (PickCharacter(0,chCharCode,dwStyle)==FALSE) return;  // 当前字符不需要
				if (dwStyle&ES_UPPERCASE) // 要求大写
					chCharCode=toupper(chCharCode); // 转化为大写
				if (dwStyle&ES_LOWERCASE) // 要求小写
					chCharCode=tolower(chCharCode); // 转化为小写
     		//Delete Invert Area
        if (DeleteInvert(hWnd,lpEditItem))
        {
		       SetCaretCoordinate(hWnd,lpEditItem); // 设定光标坐标
      	   AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标位置
           bDeleteInvert=TRUE;
        }
        // the input char is english char or symbol char
				bResult=InsertChar(hWnd,lpEditItem,0x00,chCharCode);
				// must redraw to the end of the line from the caret position
				dwShowFlag=PE_SHOWLINEEND;
			}
		}
		// get caret coordinate 
        GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,dwStyle&ES_MULTILINE);
		lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx; // 设定光标的位置
		lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
		if (bResult)
		{ // the insert operation success ,must the redraw the window
			// add code : if Caret out of the client area then must 
			// Reset Display Position
			if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
			{// 要求全部显示
			    lpShowAddress=NULL;
				nShowPos=0;
				nShowLine=0;
				dwShowFlag=PE_SHOWCLIENT;
			}
			else
			{
				// get redraw start coordinate
	   		 GetCoordinate(hWnd,lpEditItem,lpShowAddress,&xPos,&nLine,dwStyle&ES_MULTILINE); // 得到光标位置
				nShowPos=xPos;  // 从光标开始显示
				nShowLine=nLine;
			}

      if (bDeleteInvert==TRUE)
      {  // 要求全部显示
		    lpShowAddress=NULL;
				nShowPos=0;
				nShowLine=0;
				dwShowFlag=PE_SHOWCLIENT;
      }

			SendNotificationMsg(hWnd,EN_CHANGE);

			// redraw the window
			hdc=GetDC(hWnd);
	        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,nShowPos,nShowLine,0,dwShowFlag); // 显示要显示的部分
			ReleaseDC(hWnd,hdc);
			// reset thw caret
		}
		SetPDACaret(hWnd);  // 设定光标
    lParam++;
}
// **************************************************
// 声明：static BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle)
// 参数：
// 	IN HiByte -- 高字节
// 	IN LoByte -- 低字节
// 	IN dwStyle -- 编辑区风格
// 返回值：需要字符，返回TRUE，否则返回FALSE。
// 功能描述：选择字符是否符合当前类型的编辑区。
// 引用: 
// **************************************************
// delete By Jami chen in 2004.06.12
/*static BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle)
{
	if (dwStyle&ES_NUMBER)
	{  // the edit control is number ,then Only Receive 0--9
		if (HiByte!=0) return FALSE;
		if (LoByte>'9'||LoByte<'0') return FALSE;
	}
	if ((dwStyle&ES_MULTILINE)==0)
	{ // if this is single line then not receive the entercode
		if (LoByte==ENTERCODE&&HiByte==0) return FALSE;
	}
	if (HiByte==0&&LoByte<0x20)
	{// not receive the character of not display
		if (LoByte!=ENTERCODE) return FALSE;
	}
	return TRUE;
}
*/
// **************************************************
// 声明：static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 虚键值
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理WM_KEYDOWN消息。
// 引用: 
// **************************************************
static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
  int nPageLine,nScrollLine,iHorzPos = MOTIONLESS;

		nVirtKey = (int) wParam;    // virtual-key code
    nPageLine=GetPageLine(hWnd);  // 得到一页的编辑行数
    nScrollLine=0;
		switch(nVirtKey)
		{
			case VK_PRIOR:// page up
				nScrollLine=0-nPageLine;
				break;
			case VK_NEXT:// page down
				nScrollLine=nPageLine;
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
			case VK_DELETE: // delete char
				DoDeleteChar(hWnd);
				return;
			default :
				return;
		};
	if (nScrollLine==0)
	{
		if (iHorzPos!=MOTIONLESS)
		{
	    	LPEDITITEM lpEditItem;

		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
		    if (lpEditItem==NULL)
				return;
			HorzMoveCaret(hWnd,lpEditItem,iHorzPos);  // 水平移动光标
		}
//		return;
	}
    ScrollCaretInPDAEdit(hWnd,nScrollLine); // 滚动光标
    lParam++;
}
// **************************************************
// 声明：int GetTextHeight(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：返回编辑文本高度。
// 功能描述：达到编辑文本高度。
// 引用: 
// **************************************************
int GetTextHeight(HWND hWnd)
{
	HDC hdc;
	TEXTMETRIC TextMetric;
  int nTextHeight;
  DWORD dwStyle;
		hdc=GetDC(hWnd);
		// get current text property
		GetTextMetrics(hdc,&TextMetric);  
    ReleaseDC(hWnd,hdc);
    nTextHeight=(int)TextMetric.tmHeight;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		// add a dot line height , 1 dot
    if (dwStyle&ES_LINESEPARATE)
      nTextHeight+=1;
		return nTextHeight;
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
  int *lpnLine,*lpxPos;
    	LPEDITITEM lpEditItem;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // 映射指针
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

        lpnLine=(int *)wParam;
        lpxPos=(int *)lParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // // 得到编辑条目结构指针
        if (lpEditItem==NULL) return;
				if (lpnLine)
					*lpnLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;  // 得到光标的行数
				if (lpxPos)
					*lpxPos=lpEditItem->nCaretx+lpEditItem->nDisplayx;  // 得到光标的水平位置
}
// **************************************************
// 声明：static void DoSetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT ,光标所在的行
// 	IN lParam -- INT ,光标所在的x坐标
// 返回值：无
// 功能描述：设置光标位置，处理EM_SETCARETPOS消息。
// 引用: 
// **************************************************
static void DoSetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  int nTextHeight,xCaretPos,xPos,yPos,nCaretLine;
  LPEDITITEM lpEditItem;

        xCaretPos=(int )lParam;
        nCaretLine=(int )wParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
        if (lpEditItem==NULL) return;
        nTextHeight=GetTextHeight(hWnd);  // 得到文本高度

        xPos=xCaretPos-lpEditItem->nDisplayx;  // 得到光标的坐标
        yPos=(nCaretLine-lpEditItem->nDisplayLine)*nTextHeight;

        SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);  // 设置光标的坐标
		AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标的位置

  		SetPDACaret(hWnd);  // 设置光标
}

// **************************************************
// 声明：static void DoSetxDisplayPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- int ,水平显示位置。
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置水平方向的显示位置，处理MIEM_SETXDISPLAYPOS消息。
// 引用: 
// **************************************************
static void DoSetxDisplayPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  HDC hdc;
  int xPos,xCaretPos;

        xPos=(int )wParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
        if (lpEditItem==NULL) return;
				// get old caret x position
        xCaretPos=lpEditItem->nDisplayx+lpEditItem->nCaretx;
				// set new display position
        lpEditItem->nDisplayx=xPos;
				// set HSCROLL
        SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
        hdc=GetDC(hWnd);
				// redraw the window
        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,NULL,0,0,0,PE_SHOWCLIENT);
        ReleaseDC(hWnd,hdc);
				// reset the caret position
        lpEditItem->nCaretx=xCaretPos-lpEditItem->nDisplayx;
        SetPDACaret(hWnd);
        lParam++;
}
// **************************************************
// 声明：static void SetPDAEditScrollPage(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：设置编辑区滚动条滚动一页的值
// 引用: 
// **************************************************
// delete By Jami chen in 2004.06.12
/*static void SetPDAEditScrollPage(HWND hWnd)
{
   int nPageLine;
   //int nTextHeight;
   //RECT rcClientRect;
   SCROLLINFO ScrollInfo;
   DWORD dwStyle;

			// get window style 
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			// get page Lines
      nPageLine=GetPageLine(hWnd);

      ScrollInfo.cbSize=sizeof(SCROLLINFO);
      ScrollInfo.fMask=SIF_PAGE;
      // Set Vertical Scroll Page
      if (dwStyle&WS_VSCROLL)
      { // set vertical scroll page
        ScrollInfo.nPage =nPageLine;
        SetScrollInfo(hWnd,SB_VERT,&ScrollInfo,TRUE);
      }

      // Set horizonta Scroll Page
      if (dwStyle&WS_HSCROLL)
      { // set horizon scroll page
          ScrollInfo.nPage =1;
          SetScrollInfo(hWnd,SB_HORZ,&ScrollInfo,TRUE);
      }
}
*/
// delete End By Jami chen in 2004.06.12
// **************************************************
// 声明：static void SetPDAEditVScrollPos(HWND hWnd,int nNewScrollPos)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nNewScrollPos -- 滚动条当前的新位置。
// 返回值：无
// 功能描述：设置编辑区垂直滚动条的当前位置
// 引用: 
// **************************************************
static void SetPDAEditVScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE); // 设置滚动条的位置
}

// **************************************************
// 声明：static void SetPDAEditHScrollPos(HWND hWnd,int nNewScrollPos)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nNewScrollPos -- 滚动条当前的新位置。
// 返回值：
// 功能描述：设置编辑区水平滚动条的当前位置
// 引用: 
// **************************************************
static void SetPDAEditHScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
      if (!(dwStyle&WS_HSCROLL)) return; // 不能滚动
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE); // 设置滚动条的位置
}

// **************************************************
// 声明：static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD,滚动类型
// 	IN lParam -- 保留
// 返回值：成功返回滚动的行数，否则返回0
// 功能描述：垂直滚动屏幕，处理WM_VSCROLL消息。
// 引用: 
// **************************************************
static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nWindowLine;
//    RECT rcClientRect;
    int yPos;
        
			nWindowLine=GetPageLine(hWnd);
      yPos=Edit_GetScrollPos(hWnd,SB_VERT);
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // page up
          nScrollLine=0-nWindowLine;
          break;
        case SB_PAGEDOWN:  //page down
          nScrollLine=nWindowLine;
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
        lParam++;
        return VScrollWindowInPDAEdit(hWnd,nScrollLine);  // 滚动窗口
}
// **************************************************
// 声明：static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LOWORD,滚动类型
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：水平滚动屏幕，处理WM_HSCROLL消息。
// 引用: 
// **************************************************
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nHScrollPos,xWindowWidth;
//    RECT rcClientRect;
    int xPos;
        
	LPEDITITEM lpEditItem;
		// get struct PADEDITSTATE data
		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
		if (lpEditItem==NULL) return;
		// get window width
		xWindowWidth=GetWindowWidth(hWnd,lpEditItem);
		xWindowWidth=(xWindowWidth/HSCROLLWIDTH)*HSCROLLWIDTH;
			// get current x position
      xPos=Edit_GetScrollPos(hWnd,SB_HORZ);
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP: // page left
          nHScrollPos=0-xWindowWidth;
          break;
        case SB_PAGEDOWN: // page right
          nHScrollPos=xWindowWidth;
          break;
        case SB_LINEUP: // move a sect left
          nHScrollPos=0-HSCROLLWIDTH;
          break;
        case SB_LINEDOWN:  // a sect right
          nHScrollPos=HSCROLLWIDTH;
          break;
        case SB_THUMBTRACK:  //drag thumb track
          nHScrollPos=(HIWORD(wParam)-xPos)*HSCROLLWIDTH;
          break;
        default:
          return;

        }
				// horizon scroll window
        lParam++;
        HScrollWindowInPDAEdit(hWnd,nHScrollPos); // 水平滚动窗口
}
// **************************************************
// 声明：static int GetPageLine(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：返回一页的行数
// 功能描述：得到编辑去一页的行数。
// 引用: 
// **************************************************
// delete By Jami chen in 2004.06.12
/*static int GetPageLine(HWND hWnd)
{
  int nTextHeight,nPageLine;
  RECT rcClientRect;
		// get line height
    nTextHeight=GetTextHeight(hWnd);
		// get client rect
    GetClientRect(hWnd,&rcClientRect);
		// get page lines
    nPageLine=rcClientRect.bottom/nTextHeight;
    return nPageLine;
}
*/
// **************************************************
// 声明：static void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nScrillLine -- 滚动的行数
// 返回值：无
// 功能描述：垂直滚动光标。
// 引用: 
// **************************************************
// delete By Jami chen in 2004.06.12
/*static void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine)
{
	LPEDITITEM lpEditItem;
//	int nClientLine;
	//RECT ClientRect;
	int nPointCoordinate,nLine,nCaretPosition;
	LPTSTR lpLineAddress;
//	int nTextHeight;

			// get line height
//	  	nTextHeight=GetTextHeight(hWnd);


		// get struct PADEDITSTATE data
	   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
     if (lpEditItem==NULL) return;
	   // calculate client lines
     if (nScrollLine<0)
     {  // scroll to up
				if (lpEditItem->nCaretLine==0&&lpEditItem->nDisplayLine==0) // alread to the top of the edit control
					return;
     }
     if(nScrollLine>0)
     { // scroll down
				if (lpEditItem->nCaretLine+lpEditItem->nDisplayLine>=(lpEditItem->nTotalLine-1))
					return; // alread  to the bottom of the edit control
     }
		 // set caret to new line
		lpEditItem->nCaretLine+=nScrollLine;
		// Get caret line from the text
		nLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
		// get address of caret line
		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
		// get caret xPosition from text
		nPointCoordinate=lpEditItem->nCaretx+lpEditItem->nDisplayx;
		// get caret position
		lpEditItem->lpCaretPosition=GetLinePosition(hWnd,lpEditItem,lpLineAddress,nPointCoordinate,TRUE,&nCaretPosition);
		if (lpEditItem->lpCaretPosition<lpEditItem->lpPDAEditBuffer)
		{ // the caret position on title
			SetCaretToStart(hWnd,lpEditItem);
		}
		else
		{
			// Get caret x-coordinate form window origin
			lpEditItem->nCaretx=nCaretPosition-lpEditItem->nDisplayx;
		}
		// adjust caret coordinate
		if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
		{
			// clear invert and not redraw
			ClearInvert(hWnd,lpEditItem,FALSE);
			// redraw the windwo
//			DrawPDAEditControl(hWnd);
				InvalidateRect(hWnd,NULL,TRUE);
		}
		else
		{// clear invert and redraw
			ClearInvert(hWnd,lpEditItem,TRUE);
		}
		// reset caret
		SetPDACaret(hWnd);
}
*/
// delete End By Jami chen in 2004.06.12

// **************************************************
// 声明：static int VScrollWindowInPDAEdit(HWND hWnd,int nScrollLine)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nScrillLine -- 滚动的行数
// 返回值：无
// 功能描述：垂直滚动窗口。
// 引用: 
// **************************************************
static int VScrollWindowInPDAEdit(HWND hWnd,int nScrollLine)
{
	LPEDITITEM lpEditItem;
	int nClientLine;
//	RECT ClientRect;
	int nCaretLine,nOldDisplayLine;
//	LPTSTR lpLineAddress;

		// get struct PADEDITSTATE data
	   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
     if (lpEditItem==NULL) return 0;
	   // calculate client lines
	   nClientLine=GetPageLine(hWnd);
     if (nScrollLine<0)
     {  // up ?
				if (lpEditItem->nDisplayLine==0)
					return 0;
     }
     if(nScrollLine>0)
     { // down ?
				if (lpEditItem->nDisplayLine+nClientLine>lpEditItem->nTotalLine)
					return 0;
     }
		 // get old caret position
		nCaretLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
		// get old display position
		nOldDisplayLine=lpEditItem->nDisplayLine;
		// set new display position
		lpEditItem->nDisplayLine+=nScrollLine;
		// is up to the top ?
    if (lpEditItem->nDisplayLine<0)
      lpEditItem->nDisplayLine=0;
		// is down to the bottom ?
    if (lpEditItem->nDisplayLine+nClientLine>lpEditItem->nTotalLine)
      lpEditItem->nDisplayLine=lpEditItem->nTotalLine-nClientLine;
		// set new caret position
    lpEditItem->nCaretLine=nCaretLine-lpEditItem->nDisplayLine;
		// set vertical scroll position
    SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
		// Get caret line from the text
//		ClearInvert(hWnd,lpEditItem,TRUE);
		// reset caret
		SetPDACaret(hWnd);
//    DrawPDAEditControl(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
		// return the lines to be scroll
		return (int)(lpEditItem->nDisplayLine-nOldDisplayLine);
}
// **************************************************
// 声明：static void HScrollWindowInPDAEdit(HWND hWnd,int nHScrollPos)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN nScrillLine -- 水平滚动的点数
// 返回值：无
// 功能描述：水平滚动窗口。
// 引用: 
// **************************************************
static void HScrollWindowInPDAEdit(HWND hWnd,int nHScrollPos)
{
	LPEDITITEM lpEditItem;
	int nCaretPos;

		// get struct PADEDITSTATE data
	   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
     if (lpEditItem==NULL) return;
	   // calculate client lines
     if (nHScrollPos<0)
     {  // to the left
				if (lpEditItem->nDisplayx==0)
          return;
     }
		 // get old caret position
		nCaretPos=lpEditItem->nCaretx+lpEditItem->nDisplayx;
		// set new display position
		lpEditItem->nDisplayx+=nHScrollPos;
     if (lpEditItem->nDisplayx<0)
     {  // tom the left?
				lpEditItem->nDisplayx=0;
     }
		 // set new caret position
    lpEditItem->nCaretx=nCaretPos-lpEditItem->nDisplayx;
		// set herizon scroll position
    SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
		// Get caret line from the text
		//ClearInvert(hWnd,lpEditItem,TRUE);
		// reset caret
		SetPDACaret(hWnd);
		// redraw the window
//    DrawPDAEditControl(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
}

// **************************************************
// 声明：static void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPassWordWord)
// 参数：
// 	IN lpEditBuffer  -- 要填充的缓存
// 	IN nFillLength -- 要填充的长度
// 	IN chPassWordWord -- 要用来填充的密码字符
// 返回值：无
// 功能描述：将缓存填充为密码字符。
// 引用: 
// **************************************************
// delete By Jami chen in 2004.06.12
/*static void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPassWordWord)
{
	// fill the buffer
  memset(lpEditBuffer,chPassWordWord,nFillLength);
	// fill end code
  lpEditBuffer[nFillLength]=0;
}
*/
// **************************************************
// 声明：static void ReleasePDAEdit(HWND hWnd)
// 参数：
// 	IN hWnd --窗口句柄
// 返回值：无
// 功能描述： 释放编辑区，处理WM_DESTROY消息。
// 引用: 
// **************************************************
static void ReleasePDAEdit(HWND hWnd)
{
    LPEDITITEM lpEditItem;

			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return ;

	    DeletePDACaret(hWnd); // 删除光标
//	    SendNotificationMsg(hWnd,EN_KILLFOCUS);

			if (lpEditItem->lpPDASaveEditBuffer)
				free(lpEditItem->lpPDASaveEditBuffer);  // 释放备份缓存
			if (lpEditItem->lpPDAControlBuffer)
				free(lpEditItem->lpPDAControlBuffer);  // 释放控制缓存
			if (lpEditItem->lpFormat)
				free(lpEditItem->lpFormat);  // 释放格式化串
			free(lpEditItem); // 释放条目结构
			SetWindowLong(hWnd,0,0);

}

// **************************************************
// 声明：static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- MAKWWPARAM(X,Y),指定的坐标位置
// 	IN lParam -- 保留
// 返回值：低WORD返回指定坐标所在的字符索引位置，高WORD返回该字符所在的行。
// 功能描述：得到指定点的字符，处理EM_CHARFROMPOS消息。
// 引用: 
// **************************************************
static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int xPos,yPos;
    LPEDITITEM lpEditItem;

      xPos=LOWORD(lParam);
      yPos=HIWORD(lParam);

	    // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return 0;
      return GetCharFromPos(hWnd,lpEditItem,xPos,yPos); // 得到位置的字符
}

// **************************************************
// 声明：static LRESULT DoGetFirstVisibleLine(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：如果是多行编辑区，则返回第一个可见行的行索引，如果
// 		是单行编辑区，则返回第一个可见字符的索引。
// 功能描述：得到第一个可见行，处理EM_GETFIRSTVISIBLELINE消息。
// 引用: 
// **************************************************
static LRESULT DoGetFirstVisibleLine(HWND hWnd)
{
    LPEDITITEM lpEditItem;
  	DWORD dwStyle;
    LPTSTR lpPosition;

		  // Get struct EDITITEM data
		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return 0;

      // Get Edit Control Style
		  dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      if (dwStyle&ES_MULTILINE)
        return lpEditItem->nDisplayLine;
	  lpPosition=GetLinePosition(hWnd,lpEditItem,lpEditItem->lpPDAControlBuffer,0,FALSE,NULL);// 得到第一个可见字符
      return (lpPosition-lpEditItem->lpPDAControlBuffer);
}
// **************************************************
// 声明：static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前编辑区的文本限制数
// 功能描述：得到当前编辑区的文本限制。
// 引用: 
// **************************************************
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
    wParam++;
    lParam++;
	  return lpEditItem->cbEditLimitLen;  // 返回文本限制
}

// **************************************************
// 声明：static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 指定行索引
// 	IN lParam -- LPSTR 存放指定行内容的缓存，缓存中的第一个WORD是缓存的大小。
// 返回值：成功返回COPY字符的个数，否则返回0
// 功能描述：得到指定行的数据，处理EM_GETLINE消息。
// 引用: 
// **************************************************
static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPEDITITEM lpEditItem;
	int nLine,nLineLen,nBufferMaxLen;
	LPSTR lpch;
	LPSTR lpLineAddress;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

	  nLine=(int)wParam;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif

	  lpch=(LPSTR)lParam;

		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);  //得到指定行的地址
		nLineLen=GetLineLength(hWnd,lpEditItem,(LPCTSTR)lpLineAddress,NULL); // 得到指定行的长度
		nBufferMaxLen=MAKEWORD(*lpch,*(lpch+1)); // 得到缓存的大小
		if (nLineLen>=nBufferMaxLen)
			return 0; // 缓存太小
		memcpy((void *)lpch,(const void *)lpLineAddress,nLineLen); // 复制指定行数据
		lpch[nLineLen]=0; // 设置结尾符
		return nLineLen;
}
// **************************************************
// 声明：static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回垂直滚动条滚动块的位置
// 功能描述：得到垂直滚动条滚动块的位置，处理EM_GETTHUMB消息。
// 引用: 
// **************************************************
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (!(dwStyle&WS_VSCROLL)) return 0;
    wParam++;
    lParam++;
		return Edit_GetScrollPos(hWnd,SB_VERT); // 得到滚动条滑块的位置
}
// **************************************************
// 声明：static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回编辑区的总行数
// 功能描述：得到编辑区的总行数，处理EM_GETLINECOUNT消息。
// 引用: 
// **************************************************
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	  if(lpEditItem==NULL) return 0;
      wParam++;
      lParam++;
	  return lpEditItem->nTotalLine; // 得到编辑区的总行数
}
// **************************************************
// 声明：static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前编辑区的修改标志。
// 功能描述：得到当前编辑区的修改标志，处理EM_GETMODIFY消息。
// 引用: 
// **************************************************
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return FALSE;

    wParam++;
    lParam++;
		return lpEditItem->fModified; // 得到编辑区的修改标志
}
// **************************************************
// 声明：static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回当前编辑区的密码字符
// 功能描述：得到当前编辑区的密码字符，处理EM_GETPASSWORDCHAR消息。
// 引用: 
// **************************************************
static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 DWORD dwStyle;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    wParam++;
    lParam++;
		if (dwStyle&ES_PASSWORD)
				return lpEditItem->chPassWordWord; // 得到编辑区的密码字符
		else
				return 0;
}
// **************************************************
// 声明：static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- LPDWORD ， 存放选择区域的起始位置字符索引
// 	IN lParam -- LPDOWRD ， 存放选择区域的结束位置字符索引
// 返回值：成功返回MAKERESULT(START,END),否则返回0。
// 功能描述：得到编辑区的选择区域，处理EM_GETSEL消息。
// 引用: 
// **************************************************
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 int nStart , nEnd;
	 LPDWORD lpdwStart,lpdwEnd;
	 LRESULT lResult;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );   // 映射指针
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
		lpdwStart=(LPDWORD)wParam;
		lpdwEnd=(LPDWORD)lParam;
		if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
		{
// !!! Modified By Jami chen on 2003.09.12
//			return -1;
			nStart=0;
			nEnd=0;
// !!! Modified End By Jami chen on 2003.09.12
		}
		else
		{
			nStart=lpEditItem->lpInvertStart-lpEditItem->lpPDAEditBuffer;  // 得到选择开始位置
			nEnd=lpEditItem->lpInvertEnd-lpEditItem->lpPDAEditBuffer; // 得到选择结束位置
		}
		if (lpdwStart)
			*lpdwStart=(DWORD)nStart; // 赋开始位置
		if (lpdwEnd)
			*lpdwEnd=(DWORD)nEnd; // 赋结束位置
// !!! Modified By Jami chen on 2003.09.12
		if (nEnd > 0xffff || nStart > 0xffff)
			lResult = -1;
		else
			lResult=MAKELRESULT(nStart,nEnd); // 得到返回值
// !!! Modified End By Jami chen on 2003.09.12
		return lResult;
}

// **************************************************
// 声明：static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：设置编辑区的文本限制，处理EM_SETLIMITTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 int cchMax;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		cchMax=(int)wParam;

    return SetLimitText(hWnd,lpEditItem,cchMax);  // 设置文本限制
}

// **************************************************
// 声明：static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- int ，字符索引
// 	IN lParam -- 保留
// 返回值：返回指定索引字符的行索引。
// 功能描述：返回指定索引字符的行索引，处理EM_LINEFROMCHAR消息。
// 引用: 
// **************************************************
static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 DWORD dwStyle;
	 LPTSTR lpCurPos;
	 int ich,nLine;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		ich=(int)wParam;
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
		if (ich==-1)
		{ // 指定索引为-1
				if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
				{ // 没有选择
						lpCurPos=lpEditItem->lpCaretPosition;  // 当前位置为光标位置
				}
				else
				{ // 有选择区域，
						lpCurPos=lpEditItem->lpInvertStart; // 当前位置为选择开始位置
				}
		}
		else
		{ // 有指定索引
				if (ich>lpEditItem->cbEditLen)
					ich=lpEditItem->cbEditLen;
				lpCurPos=lpEditItem->lpPDAEditBuffer+ich; // 得到索引地址
		}
    GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE); // 得到当前位置的行号
		return (LRESULT)nLine; // 返回行号
}

// **************************************************
// 声明：static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT ，指定行的索引
// 	IN lParam -- 保留
// 返回值：返回指定行索引所对应的字符索引
// 功能描述：得到指定行索引所对应的字符索引，处理EM_LINEINDEX消息。
// 引用: 
// **************************************************
static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 LPTSTR lpCurPos;
	 int nLine;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		nLine=(int)wParam;  // 得到行索引
		if (nLine==-1)
		{ // 没有指定行索引
				nLine=lpEditItem->nCaretLine; // 设定为光标位置
		}
		lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine); // 得到行地址
		return (LRESULT)(lpCurPos-lpEditItem->lpPDAEditBuffer); // 得到索引
}

// **************************************************
// 声明：static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 指定字符的索引
// 	IN lParam -- 保留
// 返回值：返回指定字符所在行的字符个数
// 功能描述：得到指定字符所在行的字符个数，处理EM_LINELENGTH消息。
// 引用: 
// **************************************************
static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
   int ich;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		ich=(int)wParam;
    return GetLineLengthFromPos(hWnd,lpEditItem,ich);// 得到索引所在行的字符个数
}
// **************************************************
// 声明：static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 水平滚动字符数
// 	IN lParam -- INT 垂直滚动行数
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：滚动窗口，处理EM_LINESCROLL消息。
// 引用: 
// **************************************************
static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	DWORD dwStyle;
	int cxScroll,cyScroll;
	
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
		if (!(dwStyle&ES_MULTILINE))
			return FALSE; // 不是多行编辑区
		cxScroll=(int)wParam;
		cyScroll=(int)lParam;
		if (cxScroll)
		{
//				cxScroll=cxScroll*HSCROLLWIDTH;
				cxScroll=cxScroll*8;   // 8 --- The Width of the Char
				HScrollWindowInPDAEdit(hWnd,cxScroll); // 水平滚动窗口
		}
		if (cyScroll)  
				VScrollWindowInPDAEdit(hWnd,cyScroll);  // 垂直滚动窗口
		return TRUE;
}

// **************************************************
// 声明：static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 指定的字符索引
// 	IN lParam -- 保留
// 返回值：返回指定字符的坐标,MAKERESULT(x,y)
// 功能描述：得到指定字符的坐标，处理EM_POSFROMCHAR消息。
// 引用: 
// **************************************************
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 int wCharIndex;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		wCharIndex=(int)wParam;
    return GetPosFromChar(hWnd,lpEditItem,wCharIndex);  // 得到指定字符所在的位置
}
// **************************************************
// 声明：static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPTSTR ,指定的字符串
// 返回值：无
// 功能描述：用指定的字符串替换选择字符。
// 引用: 
// **************************************************
static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTSTR lpReplace;
	int cchLen;
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		lpReplace=(LPCTSTR)lParam; // 得到替换文本
		DeleteInvert(hWnd,lpEditItem); // 删除选择文本
		cchLen=strlen(lpReplace); // 得到替换文本的长度
		InsertString(hWnd,lpEditItem,lpReplace,cchLen); // 插入替换文本
		SetCaretCoordinate(hWnd,lpEditItem); // 设置光标坐标
		AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标位置
		InvalidateRect(hWnd,NULL,TRUE); // 重绘编辑区
		SetPDACaret(hWnd); // 设置光标
		return 0;
}
// **************************************************
// 声明：static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 滚动类型
// 	IN lParam -- 保留
// 返回值：滚动的行数
// 功能描述：垂直滚动屏幕，处理EM_SCROLL消息。
// 引用: 
// **************************************************
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
    	nScrollLine=DoVScrollWindow(hWnd,wParam,lParam); // 垂直滚动窗口
		lResult=MAKELRESULT(nScrollLine,1);
		return lResult;
}
// **************************************************
// 声明：static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：滚动光标到可见位置，处理EM_SCROLLCARET消息。
// 引用: 
// **************************************************
static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	  if(lpEditItem==NULL) return FALSE;
		if (AdjustCaretInEditItem(hWnd,lpEditItem))  // 调整光标位置
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		return TRUE;
}
// **************************************************
// 声明：static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- UINT，要设置的修改标志
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置新的修改标志，处理EM_SETMODIFY消息。
// 引用: 
// **************************************************
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 UINT  fModified;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		fModified=(UINT)wParam;  // 得到修改标志
		lpEditItem->fModified=fModified; // 设定修改标志
		return 0;
}
// **************************************************
// 声明：static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- TCHAR 新的密码字符
// 	IN lParam -- 保留
// 返回值：成功返回TRUE ，否则返回FALSE。
// 功能描述：设置新的密码字符，处理EM_SETPASSWORDCHAR消息。
// 引用: 
// **************************************************
static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 DWORD dwStyle;
	 TCHAR ch;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (dwStyle&ES_MULTILINE)
			return 0;
	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		ch=(TCHAR)wParam; // 得到密码字符
    return SetPasswordChar(hWnd,lpEditItem,ch); // 设定密码字符
}
// **************************************************
// 声明：static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- BOOL，是否要设置为只读的标志
// 	IN lParam -- 保留
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：设置编辑区的只读属性，处理EM_SETREADONLY消息。
// 引用: 
// **************************************************
static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 DWORD dwStyle;
	 BOOL fReadOnly;

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return 0;

			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			fReadOnly=(BOOL)wParam;
			if (fReadOnly)
			{ // 需要设定只读属性
				dwStyle|=ES_READONLY;
			}
			else
			{ // 不需要设定只读属性
				dwStyle&=~ES_READONLY;
			}
      lpEditItem->dwStyle=dwStyle;  // 得到编辑风格
			SetWindowLong(hWnd,GWL_STYLE,dwStyle); // 设置编辑风格
			return TRUE;
}
// **************************************************
// 声明：static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT ，选择区域起始位置
// 	IN lParam -- INT ，选择区域结束位置
// 返回值：
// 功能描述：设置选择区域，处理EM_SETSEL消息。
// 引用: 
// **************************************************
static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 int nStart,nEnd;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		nStart=(int)wParam;
		nEnd=(int)lParam;

    return SetSel(hWnd,lpEditItem,nStart,nEnd);  // 设置选择区域
}
// **************************************************
// 声明：void SendNotificationMsg( HWND hWnd, WORD wNotifyCode)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wNotifyCode -- 要发送的通知消息
// 返回值：无
// 功能描述：发送通知消息。
// 引用: 
// **************************************************
void SendNotificationMsg( HWND hWnd, WORD wNotifyCode)
{
	WORD idEditCtrl;
	WPARAM wParam;
	HWND hParentWnd;

		idEditCtrl=(int)GetWindowLong(hWnd,GWL_ID);  // 得到编辑窗口的ID
		wParam=MAKEWPARAM(idEditCtrl,wNotifyCode);// 组合通知消息参数
		hParentWnd=GetParent(hWnd); // 得到父窗口句柄
		SendMessage(hParentWnd,WM_COMMAND,wParam,(LPARAM)hWnd); // 发送消息
}

// **************************************************
// 声明：static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回拷贝字符个数，否则返回0
// 功能描述：拷贝选择区域字符到剪切板，处理WM_COPY消息。
// 引用: 
// **************************************************
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

    return CopyToClipboard(hWnd,lpEditItem); // 复制到剪切板
}
// **************************************************
// 声明：static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：
// 功能描述：粘帖，处理WM_PASTE消息。
// 引用: 
// **************************************************
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

    PasteFromClipboard(hWnd,lpEditItem);  // 从剪切板复制数据
		AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标位置
		InvalidateRect(hWnd,NULL,TRUE); // 设置窗口无效
		SetPDACaret(hWnd);  // 设置光标
    return 0;
}

// **************************************************
// 声明：static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：剪切，处理WM_CUT消息。
// 引用: 
// **************************************************
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		DoCopy(hWnd,0,0);  // 复制
		DoClear(hWnd,0,0); // 清除
		return 0;
}

// **************************************************
// 声明：static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：清除，处理WM_CLEAR消息。
// 引用: 
// **************************************************
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		DeleteInvert(hWnd,lpEditItem); // 删除选择文本
		SetCaretCoordinate(hWnd,lpEditItem); // 设置光标坐标
		AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标位置
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
		SetPDACaret(hWnd); // 设置光标
		return 0;
}
// **************************************************
// 声明：static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTSTR 要设置的文本
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：设置编辑区文本，处理WM_SETTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPCTSTR lpsz;
   LPEDITITEM lpEditItem;


	// Get struct EDITITEM data
	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	if(lpEditItem==NULL) return FALSE;
	lpsz= (LPCTSTR)lParam; // address of window-text string
    if (SetText(hWnd,lpEditItem,lpsz)==FALSE) // 设置文本
	  return FALSE;
    SetPDAEditVScrollRange(hWnd,lpEditItem->nTotalLine);  // 设置垂直滚动条的范围
	lpEditItem->nDisplayx=0;
	lpEditItem->nDisplayLine=0;
	SetEditVScrollPos(hWnd,lpEditItem->nDisplayLine); // 设置垂直滚动条的位置
	SetEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH)); // 设置水平滚动条的位置

	SetCaretToStart(hWnd,lpEditItem);  // Add By Jami chen in 2005.01.28

	SetPDACaretPosition(hWnd,lpEditItem,0,0); // 设置光标位置
	AdjustCaretInEditItem(hWnd,lpEditItem); // 调整光标位置
	InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
	SetPDACaret(hWnd); // 设置光标
	return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 缓存的大小
// 	IN lParam -- LPTSTR 存放文本的缓存
// 返回值：成功返回拷贝字符的个数，否则返回0
// 功能描述：得到编辑区的文本，处理WM_GETTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPTSTR lpsz;
	 int cMaxlen;

   LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return FALSE;
			lpsz= (LPTSTR)lParam; // address of window-text string
			cMaxlen=(int)wParam;  // 得到缓存的大小
			// !!! modified by jami chen in 2004.05.27
//			if (cMaxlen<lpEditItem->cbEditLen)
//				return FALSE;
//		    GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'

			if (cMaxlen>lpEditItem->cbEditLen)
				cMaxlen = lpEditItem->cbEditLen + 1; // +1 is read end code 'null'
			// 得到编辑文本
		    cMaxlen = GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,cMaxlen); // is read end code 'null'
			// !!! modified by jami chen
			return cMaxlen;// lpEditItem->cbEditLen;  返回实际复制数据的大小
}
// **************************************************
// 声明：static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回编辑区的文本长度。
// 功能描述：得到编辑区的文本长度，处理WM_GETTEXTLENGTH消息。
// 引用: 
// **************************************************
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;  // 得到文本长度
}
// **************************************************
// 声明：static void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen)
// 参数：
// 	IN lpSaveBuffer -- 保存数据的缓存
// 	IN lpBuffer -- 要保存的数据缓存
// 	IN cbEditLen -- 要保存的数据长度
// 返回值：无
// 功能描述：保存编辑区的文本。
// 引用: 
// **************************************************
//static void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen)
//{
//	strncpy(lpSaveBuffer,lpBuffer,cbEditLen);  // 保存当前数据到备份缓存
//}
// **************************************************
// 声明：static void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen)
// 参数：
// 	IN lpGetBuffer -- 存放得到数据的缓存
// 	IN lpBuffer -- 原始数据缓存
// 	IN cbEditLen -- 要得到的数据长度
// 返回值：无
// 功能描述：得到编辑区的文本。
// 引用: 
// **************************************************
//static void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen)
//{
//	strncpy(lpGetBuffer,lpBuffer,cbEditLen);  // 从指定缓存中得到数据
//}
// **************************************************
// 声明：static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：清空编辑区，处理EM_EMPTY消息。
// 引用: 
// **************************************************
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return FALSE;
//			Clear(lpEditItem);
      ClearEditItemStruct(lpEditItem); // 清除编辑条目结构

//			SetPDAEditScrollPage(hWnd);
			SetWindowRect(hWnd,lpEditItem,FALSE); // 窗口大小设置
			SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine); // 设定垂直滚动条
			SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH)); // 设置水平滚动条
			SetPDACaret(hWnd); // 设置光标
			InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
			return 0;
}

// **************************************************
// 声明：static LRESULT DoSearchString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 要查找的开始位置
// 	IN lParam -- LPCTSTR 要查找的字串
// 返回值：成功返回查找到的位置，否则返回-1
// 功能描述：查找字串，处理EM_SEARCHSTRING消息。
// 引用: 
// **************************************************
static LRESULT DoSearchString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  LPTSTR lpStart,lpStringPos;
  int nStart,nEnd;
  int iStart;
  LPTSTR lpSearchString;
  LRESULT lResult;
  int xPos,yPos;

	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // 得到编辑条目结构指针
	if (lpEditItem==NULL) 
		return -1;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif

	iStart = (int)wParam; // 得到开始查找位置
	lpSearchString = (LPTSTR)lParam; // 得到查找字串
	if (strlen(lpSearchString) == 0)  // 查找字串长度为0 ，返回
		return -1;

	if (iStart >= lpEditItem->cbEditLen)
		return -1; // 开始位置已经超出文本范围
	if (iStart == -1)
	{ // 设定从头开始查找
		lpStart = lpEditItem->lpPDAEditBuffer;
	}
	else
	{ // 设定从指定位置开始查找
		lpStart = lpEditItem->lpPDAEditBuffer + iStart;
	}
	lpStringPos = ab_strstr(lpStart,lpSearchString); // 查找字串
	if (lpStringPos == NULL) // 没有查到
		return -1;
	nStart = lpStringPos - lpEditItem->lpPDAEditBuffer; // 得到已经查到字串的开始位置
	nEnd = nStart + strlen(lpSearchString); // 得到结束位置
	SetSel(hWnd,lpEditItem,nStart,nEnd); // 将查到的字串设置为选择
	lResult = GetPosFromChar(hWnd,lpEditItem,nEnd); //得到结束位置的坐标
	xPos = LOWORD(lResult);
	yPos = HIWORD(lResult);
	SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos); // 设置光标坐标
    if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)  // 调整光标位置
	{
		// Redraw the window
		InvalidateRect(hWnd,NULL,TRUE); // 无效窗口
	}

	// set caret position
	SetPDACaret(hWnd); // 设置光标

      // show caret
    ShowPDACaret(hWnd);  // 显示光标
	return nStart;
}

// **************************************************
// 声明：static LRESULT DoPaint(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 返回值：无
// 功能描述：绘制窗口，处理WM_PAINT消息。
// 引用: 
// **************************************************
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
  LPEDITITEM lpEditItem;

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // 得到编辑条目结构指针
   if (lpEditItem==NULL) return 0;
   DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWCLIENT);  // 显示编辑文本
   ClearBottomBlankArea(hWnd,hdc,lpEditItem); // 清除下面空白的部分
   return 0;
}
// **************************************************
// 声明：static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- HDC 指定设备的设备句柄
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：将编辑区打印到指定的设备，处理WM_PRINTCLIENT消息。
// 引用: 
// **************************************************
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  HDC hdc;
  int nDisplayLine,nDisplayx;

   hdc=(HDC)wParam;
   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // 得到编辑条目结构指针
   if (lpEditItem==NULL) return 0;
   nDisplayLine=lpEditItem->nDisplayLine;  // 得到开始显示的行数
   nDisplayx=lpEditItem->nDisplayx; // 得到开始显示的水平位置
   lpEditItem->nDisplayLine=0; // 设置开始显示位置为起始位置
   lpEditItem->nDisplayx=0;
   DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWALL); // 绘制指定数据到设备
   //ClearBottomBlankArea(hWnd,hdc,lpEditItem);
   lpEditItem->nDisplayLine=nDisplayLine;  // 恢复原来的显示位置
   lpEditItem->nDisplayx=nDisplayx;
   return 0;
}

// **************************************************
// 声明：static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值： 无
// 功能描述：窗口风格改变，处理WM_STYLECHANGED消息。
// 引用: 
// **************************************************
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  DWORD dwStyle;

		// get window style
   dwStyle = GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // 得到编辑条目结构指针
   if (lpEditItem==NULL)
     return FALSE;
   lpEditItem->dwStyle = dwStyle;  // 设置窗口风格
   return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}


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
  LPEDITITEM lpEditItem;

		// get window style

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // 得到编辑条目结构指针
	    if (lpEditItem==NULL)
		  return FALSE;
		
		//重新设定当前文本的颜色
		lpEditItem->cl_NormalText = GetSysColor(COLOR_WINDOWTEXT);
		lpEditItem->cl_NormalBkColor = GetSysColor(COLOR_WINDOW);

		lpEditItem->cl_InvertText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		lpEditItem->cl_InvertBkColor = GetSysColor(COLOR_HIGHLIGHT);

		lpEditItem->cl_DisableText = GetSysColor(COLOR_GRAYTEXT);
		lpEditItem->cl_DisableBkColor = GetSysColor(COLOR_WINDOW);

		lpEditItem->cl_ReadOnly = lpEditItem->cl_NormalText;
		lpEditItem->cl_ReadOnlyBk = lpEditItem->cl_NormalBkColor;

		lpEditItem->cl_Title = lpEditItem->cl_NormalText;
		lpEditItem->cl_TitleBk = lpEditItem->cl_NormalBkColor;

		InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
// !!! Add End By Jami chen in 2004.07.19

// **************************************************
// 声明：static LRESULT DoDeleteChar(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：删除字符。
// 引用: 
// **************************************************
static LRESULT DoDeleteChar(HWND hWnd)
{
  LPEDITITEM lpEditItem;
  DWORD dwStyle;

		// get window style
   dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
   if (dwStyle&ES_READONLY) return FALSE; // 不能删除

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // 得到编辑条目结构指针
   if (lpEditItem==NULL)
     return FALSE;
   DeleteChar(hWnd,lpEditItem); // 删除指定字符
   return TRUE;
}


// **************************************************
// 声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTLCOLORSTRUCT，要设置的颜色值
// 返回值：无
// 功能描述：设置窗口颜色，处理WM_SETCTLCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
//	COLORREF cl_NewColor;
	LPCTLCOLORSTRUCT lpCtlColor;
		
	LPEDITITEM lpEditItem;

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
		if (lpEditItem==NULL)
			return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // 映射指针
#endif
		lpCtlColor = (LPCTLCOLORSTRUCT)lParam;  // 得到颜色结构指针

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // 设置文本颜色
			 lpEditItem->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // 设置文本背景色
				lpEditItem->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // 设置选择文本颜色
				lpEditItem->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {  // 设置选择文本背景色
				lpEditItem->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // 设置DISBALE的文本颜色
				lpEditItem->cl_DisableText = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // 设置DISABLE的文本背景色
				lpEditItem->cl_DisableBkColor = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 { // 设置只读文本颜色
				lpEditItem->cl_ReadOnly = lpCtlColor->cl_ReadOnly;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 { // 设置只度文本背景色
				lpEditItem->cl_ReadOnlyBk  = lpCtlColor->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 设置标题文本颜色
				lpEditItem->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {  // 设置标题文本背景色
				lpEditItem->cl_TitleBk = lpCtlColor->cl_TitleBk ;
		 }
/*
		dwColorType = (UINT)wParam;
		cl_NewColor = (COLORREF)lParam;
		switch(dwColorType)
		{
			case NORMALTEXTCOLOR:
				lpEditItem->cl_NormalText = cl_NewColor;
				break;
			case NORMALTEXTBKCOLOR:
				lpEditItem->cl_NormalBkColor = cl_NewColor;
				break;
			case NORMALINVERTCOLOR:
				lpEditItem->cl_InvertText = cl_NewColor;
				break;
			case NORMALINVERTBKCOLOR:
				lpEditItem->cl_InvertBkColor = cl_NewColor;
				break;
			case NORMALDISABLECOLOR:
				lpEditItem->cl_DisableText = cl_NewColor;
				break;
			case NORMALDISABLEBKCOLOR:
				lpEditItem->cl_DisableBkColor = cl_NewColor;
				break;
			default:
				return FALSE;
		}
*/
		 InvalidateRect(hWnd,NULL,TRUE);  // 无效窗口
		return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTLCOLORSTRUCT，存放得到的颜色值
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：得到当前窗口的颜色，处理WM_GETCTLCOLOR消息。
// 引用: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
		
	LPEDITITEM lpEditItem;
	LPCTLCOLORSTRUCT lpCtlColor;

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // 得到编辑条目结构指针
		if (lpEditItem==NULL)
			return FALSE;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;  // 得到颜色结构指针

		 if (lpCtlColor == NULL) 
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // 得到编辑文本颜色
			 lpCtlColor->cl_Text = lpEditItem->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {  // 得到编辑文本背景色
				lpCtlColor->cl_TextBk = lpEditItem->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {  // 得到选择文本颜色
				lpCtlColor->cl_Selection = lpEditItem->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {  // 得到选择文本背景色
				lpCtlColor->cl_SelectionBk = lpEditItem->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {  // 得到DISABLE文本颜色
				lpCtlColor->cl_Disable= lpEditItem->cl_DisableText ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {  // 得到DISABLE文本背景色
				lpCtlColor->cl_DisableBk = lpEditItem->cl_DisableBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 {  // 得到只读文本颜色
				lpCtlColor->cl_ReadOnly= lpEditItem->cl_ReadOnly ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 {  // 得到只读文本背景色
				lpCtlColor->cl_ReadOnlyBk = lpEditItem->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {  // 得到标题文本颜色
				lpCtlColor->cl_Title= lpEditItem->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {  // 得到标题文本背景色
				lpCtlColor->cl_TitleBk = lpEditItem->cl_TitleBk ;
		 }

/*		dwColorType = (UINT)wParam;
		switch(dwColorType)
		{
			case NORMALTEXTCOLOR:
				return lpEditItem->cl_NormalText;
			case NORMALTEXTBKCOLOR:
				return lpEditItem->cl_NormalBkColor;
			case NORMALINVERTCOLOR:
				return lpEditItem->cl_InvertText;
			case NORMALINVERTBKCOLOR:
				return lpEditItem->cl_InvertBkColor;
			case NORMALDISABLECOLOR:
				return lpEditItem->cl_DisableText;
			case NORMALDISABLEBKCOLOR:
				return lpEditItem->cl_DisableBkColor;
			default:
				break;
		}
*/
		return TRUE;
}

// **************************************************
// 声明：static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 设备句柄
// 	IN lpEditItem -- 编辑控件结构指针
// 返回值：无
// 功能描述：清除窗口底部没有数据的区域。
// 引用: 
// **************************************************
static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
{
  RECT rcClientRect,rcClearRect;
  int nBottomPos;
  HBRUSH hBrush;


	nBottomPos=(int)lpEditItem->rect.bottom;
	GetClientRect(hWnd,&rcClientRect); // 得到客户区域
    if (nBottomPos<rcClientRect.bottom)
    { // 需要清除空白页
	    rcClearRect.left=0;
	    rcClearRect.top=nBottomPos;
	    rcClearRect.right=rcClientRect.right;
	    rcClearRect.bottom =rcClientRect.bottom;
	    // clear ClearRect
	    //FillRect(hdc,&rcClearRect,GetStockObject(WHITE_BRUSH));
		hBrush = GetBkBrush(hWnd,lpEditItem);
	    FillRect(hdc,&rcClearRect,hBrush);  // 清除空白页
		DeleteObject(hBrush);
    }
}

// **************************************************
// 声明：static char * ab_strstr( const char * str1, const char * str2 )
// 参数：
// 	IN str1 -- 原始字串
// 	IN str2 -- 要查找的字串
// 返回值：返回查找的结果
// 功能描述：字符串查找
// 引用: 
// **************************************************
static char * ab_strstr( const char * str1, const char * str2 )
{
	const char *s1, *s2;
	const char * cp = (const char *)str1;
	
	if ( !*str2 )
		return( (char *)str1 );
	
	while( *cp )
	{
		s1 = cp;
		s2 = (char *)str2;
		
		while( *s1 && *s2 && !( *s1 - *s2 ) )
		{
			s1++;
			s2++;
		}
		
		if( !*s2 )
			return (char*)cp;		

		if (*cp & 0x80)
			cp +=2;
		else
			cp ++;
	}
	
	return (0);
}// ********************************************************************
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

