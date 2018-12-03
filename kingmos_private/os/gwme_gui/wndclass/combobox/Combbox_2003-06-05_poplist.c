/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：组合框类
版本号：1.0.0
开发时期：2000-01-10
作者：陈建明
修改记录：
    2003-04-21:将下拉按钮的尺寸改为从系统得到, by LN
******************************************************/

#include <eframe.h>
#include <ecombo.h>
#include <eedit.h>
#include <edialog.h>
#include <esymbols.h>

#define COMBOBOX classCOMBOBOX

const char classCOMBOBOX[] = "ComboBox";

//#define _MAPPOINTER

#define ID_EDIT   100
//#define ID_BUTTON 101
#define ID_LIST   102

//#define TITLEHEIGHT 18   //LN:2003-04-21,删除

#define CB_RAISEDOUTER 0x0001
#define CB_SUNKENOUTER 0x0002

#define CB_DROPDOWN    TRUE
#define CB_UNDROPDOWN  FALSE
/*********************************************************************************************************/
// Used extern functiom
/*********************************************************************************************************/
ATOM RegisterComboBoxClass(HINSTANCE hInstance);

LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ComboListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef LRESULT (CALLBACK *WndListProc )(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/*********************************************************************************************************/
// Used intramural functiom
/*********************************************************************************************************/
typedef struct {
				HWND hListWnd;
				RECT rect;
        RECT rectButton;
        DWORD state;
        short iDropState;
        BOOL bRetracted;
		BOOL bChangedSize;
}COMBOBOXSTRUCT ,*LPCOMBOBOXSTRUCT;
/*********************************************************************************************************/
// Message Process function
/*********************************************************************************************************/

static LRESULT DoCreateComboBox(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ReleaseComboBox(HWND hWnd);
//static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam, LPARAM lParam);
//static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoInsertColumn(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoSetImageList(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoAddString(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteString(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDir(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCount(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLBText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLBTextLen(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInsertString(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoResetContent(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFindString(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSelectString(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoShowDropDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetDroppedControlRect(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetDroppedState(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFindStringExact(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoInitStorage(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotifySelChange(HWND hWnd);
static LRESULT DoNotifyDblClk(HWND hWnd);
static LRESULT DoNotifyEditChange(HWND hWnd);
static LRESULT DoNotifyEditUpdate(HWND hWnd);
static LRESULT DoNotifyErrSpace(HWND hWnd);

/*********************************************************************************************************/

static void PopupList(HWND hWnd);
static void RetractPopList(HWND hWnd);
static void SetNewEditText(HWND hWnd);
static void DrawDropButtonEdge(HWND hWnd,HDC hdc);
static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg);
static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight);//LN:2003-04-21,修改
static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint);

/*********************************************************************************************************/
//WndListProc DefaultListProc;

//static LRESULT DoListLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoListLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoListMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
/*********************************************************************************************************/

ATOM RegisterComboBoxClass(HINSTANCE hInstance)
{
	WNDCLASS wcex;

	// size of the struct 'WNDCLASSEX' 
//	wcex.cbSize = sizeof(WNDCLASSEX);

	// the class propertiy 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	// this class window procedure
	wcex.lpfnWndProc	= (WNDPROC)ComboBoxProc;
	// no class extra
	wcex.cbClsExtra		= 0;
	// window extra 4 byte to save address
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	// no icon
	wcex.hIcon			= NULL;
	// I-Beam Cursor
	wcex.hCursor		= NULL;
	// background color
	wcex.hbrBackground	= 0;
	// no menu
	wcex.lpszMenuName	= NULL; // no menu ,add to future
	// class name
	wcex.lpszClassName	= COMBOBOX;
	// no small icon
//	wcex.hIconSm		= NULL;

	// register this class
	return RegisterClass(&wcex);
}

/*
    This function is PDAEdit Windows procedure
*/
LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
  WORD wmId,wmEvent;

	switch (message)
	{
		case WM_CREATE:
			return DoCreateComboBox(hWnd,wParam,lParam);
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam); 
      switch(wmId)
      {
         case ID_LIST:
              switch(wmEvent)
              {
                 case LBN_SELCHANGE:
                      DoNotifySelChange(hWnd);
                      break;
                 case LBN_DBLCLK:
                      DoNotifyDblClk(hWnd);
                      break;
                 case LBN_ERRSPACE:
                      DoNotifyErrSpace(hWnd);
                      break;
                 default:
                      break;
              }
              break;
         case ID_EDIT:
              switch(wmEvent)
              {
                 case EN_CHANGE:
                      DoNotifyEditChange(hWnd);
                      break;
                 case EN_UPDATE:
                      DoNotifyEditUpdate(hWnd);
                      break;
                 case EN_ERRSPACE:
                      DoNotifyErrSpace(hWnd);
                      break;
                 default:
                      break;
              }
         default:
              break;
      }
			break;
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
   	case WM_LBUTTONDOWN:
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE:
			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP:
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_SETFOCUS:
      return DoSetFocus(hWnd,wParam,lParam);
    case WM_KILLFOCUS:
			return DoKillFocus(hWnd,wParam,lParam);
		case WM_DESTROY:
			// close this window
			ReleaseComboBox(hWnd);
//			PostQuitMessage(0);
			break;
		case WM_CLEAR:
			return DoClear(hWnd,wParam,lParam);
    case WM_GETTEXT:
      return DoGetText(hWnd,wParam,lParam);
    case WM_GETTEXTLENGTH:
      return DoGetTextLength(hWnd,wParam,lParam);
    case WM_WINDOWPOSCHANGED:
      return DoWindowPosChanged(hWnd,wParam,lParam);

	case CB_GETEDITSEL:
      return DoGetEditSel(hWnd,wParam,lParam);

	case CB_LIMITTEXT:
      return DoLimitText(hWnd,wParam,lParam);
    case CB_SETEDITSEL:
      return DoSetEditSel(hWnd,wParam,lParam);
    case CB_ADDSTRING:
      return DoAddString(hWnd,wParam,lParam);
    case CB_DELETESTRING:
      return DoDeleteString(hWnd,wParam,lParam);
    case CB_DIR:
      return DoDir(hWnd,wParam,lParam);
    case CB_GETCOUNT:
      return DoGetCount(hWnd,wParam,lParam);
    case CB_GETCURSEL:
      return DoGetCurSel(hWnd,wParam,lParam);
    case CB_GETLBTEXT:
      return DoGetLBText(hWnd,wParam,lParam);
    case CB_GETLBTEXTLEN:
      return DoGetLBTextLen(hWnd,wParam,lParam);
    case CB_INSERTSTRING:
      return DoInsertString(hWnd,wParam,lParam);
    case CB_RESETCONTENT:
      return DoResetContent(hWnd,wParam,lParam);
    case CB_FINDSTRING:
      return DoFindString(hWnd,wParam,lParam);
    case CB_SELECTSTRING:
      return DoSelectString(hWnd,wParam,lParam);
    case CB_SETCURSEL:
      return DoSetCurSel(hWnd,wParam,lParam);
    case CB_SHOWDROPDOWN:
      return DoShowDropDown(hWnd,wParam,lParam);
    case CB_GETITEMDATA:
      return DoGetItemData(hWnd,wParam,lParam);
    case CB_SETITEMDATA:
      return DoSetItemData(hWnd,wParam,lParam);
    case CB_GETDROPPEDCONTROLRECT:
      return DoGetDroppedControlRect(hWnd,wParam,lParam);
    case CB_SETITEMHEIGHT:
      return DoSetItemHeight(hWnd,wParam,lParam);
    case CB_GETITEMHEIGHT:
      return DoGetItemHeight(hWnd,wParam,lParam);
    case CB_SETEXTENDEDUI:
      return DoSetExtendEdui(hWnd,wParam,lParam);
    case CB_GETEXTENDEDUI:
      return DoGetExtendEdui(hWnd,wParam,lParam);
    case CB_GETDROPPEDSTATE:
      return DoGetDroppedState(hWnd,wParam,lParam);
    case CB_FINDSTRINGEXACT:
      return DoFindStringExact(hWnd,wParam,lParam);
    case CB_SETLOCALE:
      return DoSetLocale(hWnd,wParam,lParam);
    case CB_GETLOCALE:
      return DoGetLocale(hWnd,wParam,lParam);
    case CB_GETTOPINDEX:
      return DoGetTopIndex(hWnd,wParam,lParam);
    case CB_SETTOPINDEX:
      return DoSetTopIndex(hWnd,wParam,lParam);
    case CB_GETHORIZONTALEXTENT:
      return DoGetHorizontalExtent(hWnd,wParam,lParam);
    case CB_SETHORIZONTALEXTENT:
      return DoSetHorizontalExtent(hWnd,wParam,lParam);
    case CB_GETDROPPEDWIDTH:
      return DoGetDroppedWidth(hWnd,wParam,lParam);
    case CB_SETDROPPEDWIDTH:
      return DoSetDroppedWidth(hWnd,wParam,lParam);
    case CB_INITSTORAGE:
      return DoInitStorage(hWnd,wParam,lParam);
    default:
			// default procedure
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
/*
LRESULT CALLBACK ComboListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
       return 0;
	  switch (message)
	  {
   	  case WM_LBUTTONDOWN:
			  if (DoListLButtonDown(hWnd,wParam,lParam)==0)
          return 0;
        break;
   	  case WM_MOUSEMOVE:
			  if ( DoListMouseMove(hWnd,wParam,lParam)==0)
          return 0;
        break;
   	  case WM_LBUTTONUP:
			  if (DoListLButtonUp(hWnd,wParam,lParam)==0)
          return 0;
        DefaultListProc(hWnd,message,wParam,lParam);
//        DoChangedSelect(hWnd);
//        SetNewEditText(hWnd);
//        RetractPopList(GetParent(hWnd));
        break;
//     case WM_KILLFOCUS:
//        RetractPopList(GetParent(hWnd));
//        break;
    }
    return DefaultListProc(hWnd,message,wParam,lParam);
}
*/
static LRESULT DoCreateComboBox(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
	RECT rect,rectWindow;
	DWORD dwStyle;
	HINSTANCE hInst;
	int iTitleHeight = GetSystemMetrics(SM_CYCAPTION);//LN:2003-04-21,增加
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL);//LN:2003-04-21,增加

	lpComboBoxStruct=malloc(sizeof(COMBOBOXSTRUCT));
	if (lpComboBoxStruct==NULL)
		return -1;//FALSE;//LN:2003-04-21,修改
	hInst=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
	dwStyle=GetWindowLong(hWnd,GWL_STYLE);
	GetClientRect(hWnd,&rect);

    lpComboBoxStruct->rect=rect;
    lpComboBoxStruct->iDropState=CB_DROPDOWN;
	lpComboBoxStruct->bChangedSize=FALSE;
    switch(dwStyle&CBS_TYPEMASK)
    {
    case CBS_DROPDOWN:
        CreateWindow(classEDIT,
				              "",
						      WS_CHILD|WS_VISIBLE|WS_BORDER,
						      (short)rect.left,
							  (short)rect.top,
							  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,修改
							  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						      hWnd,
						      (HMENU)ID_EDIT,
									hInst,
						      NULL);
        lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,修改
        lpComboBoxStruct->rectButton.top=rect.top;
        lpComboBoxStruct->rectButton.right=rect.right;
        lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,修改
        lpComboBoxStruct->state=CB_RAISEDOUTER;
        break;
	case CBS_SIMPLE:
		CreateWindow(classEDIT,
			"",
			WS_CHILD|WS_VISIBLE|WS_BORDER,
			(short)rect.left,
			(short)rect.top,
			(short)rect.right,
			(short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
			hWnd,
			(HMENU)ID_EDIT,
			hInst,
			NULL);
        break;
	case CBS_DROPDOWNLIST:
        CreateWindow(classSTATIC,
			"",
						      WS_CHILD|WS_VISIBLE|WS_BORDER,
							  (short)rect.left,
							  (short)rect.top,
							  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,修改
							  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
							  hWnd,
							  (HMENU)ID_EDIT,
							  hInst,
							  NULL);
							  /*        CreateWindow(classBUTTON,
							  "",
							  WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON|WS_BORDER,
							  rect.right-18,rect.top,18,TITLEHEIGHT,
							  hWnd,
							  (HMENU)ID_BUTTON,
							  0,
		NULL);*/
        lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,修改
        lpComboBoxStruct->rectButton.top=rect.top;
        lpComboBoxStruct->rectButton.right=rect.right;
        lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,修改
		lpComboBoxStruct->state=CB_RAISEDOUTER;
		break;
	default :
		return -1;
	}
//						  WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_HASSTRINGS|LBS_NOTIFY,
	{
		DWORD dwListStyle;
		//		  DWORD dwExStyle;
		if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
			dwListStyle=WS_POPUP|WS_VISIBLE|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;
		else
			dwListStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;
		//		  dwExStyle=WS_EX_TOPMOST;
		if (dwStyle&CBS_OWNERDRAWFIXED)
			dwListStyle|=LBS_OWNERDRAWFIXED;
		
		if (dwStyle&CBS_OWNERDRAWVARIABLE)
			dwStyle|=LBS_OWNERDRAWVARIABLE;
		
		//		  if (dwStyle&CBS_NOINTEGRALHEIGHT)
		//			  dwListStyle|=LBS_NOINTEGRALHEIGHT;
		
		if (dwStyle&CBS_SORT)
			dwListStyle|=LBS_SORT;
		
		if (dwStyle&CBS_HASSTRINGS)
			dwListStyle|=LBS_HASSTRINGS;
		  

/*		 lpComboBoxStruct->hListWnd=CreateWindowEx(
							dwExStyle,
							classCOMBOLISTBOX,
									"",
							dwListStyle,
							(short)rect.left,
							(short)(rect.top+TITLEHEIGHT),
							(short)(rect.right-rect.left),
							(short)(rect.bottom-rect.top-TITLEHEIGHT),
							hWnd,
							(HMENU)ID_LIST,
							hInst,
							hWnd);
*/
		  if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
		  {
// !!! Modified By Jami chen 2002.04.24
/*
			  lpComboBoxStruct->hListWnd=CreateWindow(classCOMBOLISTBOX,
									"",
							dwListStyle,
							(short)rectWindow.left,
							(short)(rectWindow.top+TITLEHEIGHT),
							(short)(rectWindow.right-rectWindow.left),
							(short)(rectWindow.bottom-rectWindow.top-TITLEHEIGHT),
							hWnd,
							(HMENU)ID_LIST,
							hInst,
							hWnd);
*/
			  GetDropListRect(hWnd,&rectWindow,iTitleHeight);//LN:2003-04-21,修改
			  lpComboBoxStruct->hListWnd=CreateWindow(classCOMBOLISTBOX,
									"",
							dwListStyle,
							(short)rectWindow.left,
							(short)(rectWindow.top),
							(short)(rectWindow.right-rectWindow.left),
							(short)(rectWindow.bottom-rectWindow.top),
							hWnd,
							(HMENU)ID_LIST,
							hInst,
							hWnd);
// Modified End By Jami chen 2002.04.24
		  }
		  else
		  {
			lpComboBoxStruct->hListWnd=CreateWindow(classCOMBOLISTBOX,
									"",
							dwListStyle,
							(short)rect.left,
							(short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
							(short)rect.right,
							(short)(rect.bottom-iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
							hWnd,
							(HMENU)ID_LIST,
							hInst,
							hWnd);
		  }

		}
//    DefaultListProc=(WndListProc )GetWindowLong(hListWnd,GWL_WNDPROC);
//    SetWindowLong(hListWnd,GWL_WNDPROC,(long)ComboListProc);
		SetWindowLong(hWnd,0,(long)lpComboBoxStruct);
//		SetWindowPos(hWnd,0,0,0,rect.right,TITLEHEIGHT,SWP_NOMOVE|SWP_NOZORDER);
		if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
		{
			lpComboBoxStruct->bChangedSize=TRUE;
			//SetWindowPos(hWnd,0,0,0,rect.right,TITLEHEIGHT,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,删除
			SetWindowPos(hWnd,0,0,0,rectWindow.right - rectWindow.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			lpComboBoxStruct->bChangedSize=FALSE;
			ShowWindow(lpComboBoxStruct->hListWnd,SW_HIDE);
			lpComboBoxStruct->iDropState=CB_UNDROPDOWN;
		}
    return 0;
}
/*********************************************************************************************************/
// ReleaseComboBoxMemory
/*********************************************************************************************************/
static void ReleaseComboBox(HWND hWnd)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
       return;
    free(lpComboBoxStruct);
    SetWindowLong(hWnd,0,0);
}
/****************************************************************************************************/
/****************************************************************************************************/
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
    DrawDropButtonEdge(hWnd,hdc);
		return 0;
}
/****************************************************************************************************/
// Process Message --- WM_LBUTTONDOWN
// The WM_LBUTTONDOWN message is posted when the user presses the left mouse button while the cursor
// is in the client area of a window. If the mouse is not captured, the message is posted to the
// window beneath the cursor. Otherwise, the message is posted to the window that has captured the mouse. 
// Input
// fwKeys = wParam;        // key flags 
// xPos = LOWORD(lParam);  // horizontal position of cursor
// yPos = HIWORD(lParam);  // vertical position of cursor 

// Parameters
// fwKeys 
//    Value of wParam. Indicates whether various virtual keys are down. This parameter can be any 
//    combination of the following values: Value Description
//				MK_CONTROL Set if the ctrl key is down. 
//				MK_LBUTTON Set if the left mouse button is down.
//				MK_MBUTTON Set if the middle mouse button is down. 
//				MK_RBUTTON Set if the right mouse button is down. 
//				MK_SHIFT Set if the shift key is down.

// xPos 
//    Value of the low-order word of lParam. Specifies the x-coordinate of the cursor. The coordinate
//    is relative to the upper-left corner of the client area. 
// yPos
//    Value of the high-order word of lParam. Specifies the y-coordinate of the cursor. The coordinate 
//    is relative to the upper-left corner of the client area.
// Return Values
// If an application processes this message, it should return zero.

// Remarks
// An application can use the MAKEPOINTS macro to convert the lParam parameter to a POINTS structure.

/****************************************************************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
      if (lpComboBoxStruct==NULL)
         return 0;

      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      {
        lpComboBoxStruct->state=CB_SUNKENOUTER;
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
      SetCapture(hWnd);
      SetFocus(hWnd);
	  return 0;
}
/****************************************************************************************************/
// Process Message --- WM_MOUSEMOVE
// The WM_MOUSEMOVE message is posted to a window when the cursor moves. If the mouse is not captured,
// the message is posted to the window that contains the cursor. Otherwise, the message is posted to
// the window that has captured the mouse.

// Input 
// fwKeys = wParam;        // key flags
// xPos = LOWORD(lParam);  // horizontal position of cursor 
// yPos = HIWORD(lParam);  // vertical position of cursor
 

// Parameters
// fwKeys
//    Value of wParam. Indicates whether various virtual keys are down. This parameter can be any 
//    combination of the following values: Value Description
//				MK_CONTROL Set if the ctrl key is down.
//				MK_LBUTTON Set if the left mouse button is down. 
//				MK_MBUTTON Set if the middle mouse button is down. 
//				MK_RBUTTON Set if the right mouse button is down. 
//				MK_SHIFT Set if the shift key is down. 

// xPos 
//    Value of the low-order word of lParam. Specifies the x-coordinate of the cursor. The coordinate 
//    is relative to the upper-left corner of the client area.
// yPos 
//    Value of the high-order word of lParam. Specifies the y-coordinate of the cursor. The coordinate 
//    is relative to the upper-left corner of the client area.
// Return Values
// If an application processes this message, it should return zero.

// Remarks
// An application can use the MAKEPOINTS macro to convert the lParam parameter to a POINTS structure.

/****************************************************************************************************/
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      if (GetCapture()!=hWnd)
        return 0;
      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
      if (lpComboBoxStruct==NULL)
         return 0;
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      {
        if (lpComboBoxStruct->state==CB_SUNKENOUTER)
          return 0;
        lpComboBoxStruct->state=CB_SUNKENOUTER;
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
      else
      {
        if (lpComboBoxStruct->state==CB_RAISEDOUTER)
          return 0;
        lpComboBoxStruct->state=CB_RAISEDOUTER;
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
		return 0;
}
/****************************************************************************************************/
// Process Message --- WM_LBUTTONUP
// The WM_LBUTTONUP message is posted when the user releases the left mouse button while the cursor
// is in the client area of a window. If the mouse is not captured, the message is posted to the
// window beneath the cursor. Otherwise, the message is posted to the window that has captured the mouse.
// Input
// fwKeys = wParam;        // key flags
// xPos = LOWORD(lParam);  // horizontal position of cursor
// yPos = HIWORD(lParam);  // vertical position of cursor

// Parameters
// fwKeys
//    Value of wParam. Indicates whether various virtual keys are down. This parameter can be any
//    combination of the following values: Value Description
//				MK_CONTROL Set if the ctrl key is down.
//				MK_LBUTTON Set if the left mouse button is down. 
//				MK_MBUTTON Set if the middle mouse button is down.
//				MK_RBUTTON Set if the right mouse button is down. 
//				MK_SHIFT Set if the shift key is down. 

// xPos 
//    Value of the low-order word of lParam. Specifies the x-coordinate of the cursor. The coordinate 
//    is relative to the upper-left corner of the client area.
// yPos 
//    Value of the high-order word of lParam. Specifies the y-coordinate of the cursor. The coordinate 
//    is relative to the upper-left corner of the client area.
// Return Values
// If an application processes this message, it should return zero. 

// Remarks
// An application can use the MAKEPOINTS macro to convert the lParam parameter to a POINTS structure.

/****************************************************************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      if (GetCapture()!=hWnd)
        return 0;
      SetCapture(0);
      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
      if (lpComboBoxStruct==NULL)
         return 0;

      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      {
        lpComboBoxStruct->state=CB_RAISEDOUTER;
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
        PopupList(hWnd);
      }
			return 0;
}
/****************************************************************************************************/
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
     hEditWnd=GetDlgItem(hWnd,ID_EDIT);
//     SendMessage(hWnd,WM_SETFOCUS,0,(LPARAM)hEditWnd);
     SetFocus(hEditWnd);
     SendNotifiedMsg(hWnd,CBN_SETFOCUS);
     return 0;
}
/****************************************************************************************************/

static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
     SendNotifiedMsg(hWnd,CBN_KILLFOCUS);
		 return 0;
}
/****************************************************************************************************/
// Process Message WM_CLEAR
// An application sends a WM_CLEAR message to delete (clear) the current selection.

// wParam = 0; // not used; must be zero
// lParam = 0; // not used; must be zero

// Return Values
// This message does not return a value.
/****************************************************************************************************/
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_CLEAR,wParam,lParam);
}

/****************************************************************************************************/
static LRESULT DoGetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,EM_GETSEL,wParam,lParam);
}
static LRESULT DoLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,EM_LIMITTEXT,wParam,lParam);
}
static LRESULT DoSetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   wParam=LOWORD(lParam); //nStart
   lParam=HIWORD(lParam); //nEnd
   return SendMessage(hEditWnd,EM_SETSEL,wParam,lParam);
}
static LRESULT DoGetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_GETTEXT,wParam,lParam);
}
static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_GETTEXTLENGTH,wParam,lParam);
}

static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	RECT rect,rectWindow;
	DWORD dwStyle;
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL);//LN:2003-04-21,增加
    int iTitleHeight = GetSystemMetrics(SM_CYCAPTION);//LN:2003-04-21,增加


		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return 0;
		if (lpComboBoxStruct->bChangedSize==TRUE)
			return 0;
		if ((lpwp->flags&SWP_NOSIZE)==0)
		{// the window size to be changed
			// change the title size
			GetClientRect(hWnd,&rect);

			lpComboBoxStruct->rect=rect;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			switch(dwStyle&CBS_TYPEMASK)
			{
			  case CBS_DROPDOWN:
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,修改
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  SWP_NOZORDER|SWP_NOACTIVATE);
				lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,修改
				lpComboBoxStruct->rectButton.top=rect.top;
				lpComboBoxStruct->rectButton.right=rect.right;
				lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,修改
				break;
			  case CBS_SIMPLE:
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)rect.right,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  SWP_NOZORDER|SWP_NOACTIVATE);
				break;
			  case CBS_DROPDOWNLIST:
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,修改
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  SWP_NOZORDER|SWP_NOACTIVATE);
						  lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,修改
						  lpComboBoxStruct->rectButton.top=rect.top;
						  lpComboBoxStruct->rectButton.right=rect.right;
						  lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,修改
						break;
			}
			lpComboBoxStruct->bChangedSize=TRUE;
			//SetWindowPos(hWnd,0,0,0,rect.right,TITLEHEIGHT,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,删除
//			SetWindowPos(hWnd,0,0,0,rect.right,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			GetWindowRect(hWnd,&rectWindow);
			SetWindowPos(hWnd,0,0,0,rectWindow.right - rectWindow.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加

			lpComboBoxStruct->bChangedSize=FALSE;
			//change the popup list size
/*			SetWindowPos( 
						  lpComboBoxStruct->hListWnd,
						  0,
						  (short)rect.left,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  (short)(rect.right-rect.left),
						  (short)(rect.bottom-rect.top-iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  SWP_NOZORDER|SWP_NOACTIVATE);
*/
			SetWindowPos( 
						  lpComboBoxStruct->hListWnd,
						  0,
						  (short)rect.left,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  (short)(rectWindow.right-rectWindow.left),
						  (short)(rectWindow.bottom-rectWindow.top-iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  SWP_NOZORDER|SWP_NOACTIVATE);
		}
		return 0;
}
static LRESULT DoAddString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
//		HWND hListWnd;
//		hListWnd=GetDlgItem(hWnd,ID_LIST);
		return SendMessage(lpComboBoxStruct->hListWnd,LB_ADDSTRING,wParam,lParam);
}
static LRESULT DoDeleteString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_DELETESTRING,wParam,lParam);
}
static LRESULT DoDir(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_DIR,wParam,lParam);
}
static LRESULT DoGetCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETCOUNT,wParam,lParam);
}
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETCURSEL,wParam,lParam);
}
static LRESULT DoGetLBText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTEXT,wParam,lParam);
}
static LRESULT DoGetLBTextLen(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTEXTLEN,wParam,lParam);
}
static LRESULT DoInsertString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_INSERTSTRING,wParam,lParam);
}
static LRESULT DoResetContent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hEditWnd;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	 SendMessage(lpComboBoxStruct->hListWnd,LB_RESETCONTENT,wParam,lParam);
	 hEditWnd=GetDlgItem(hWnd,ID_EDIT);
	 SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)"");
	 return 0;
}
static LRESULT DoFindString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_FINDSTRING,wParam,lParam);
}
static LRESULT DoSelectString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	short iIndex;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 iIndex=(short)SendMessage(lpComboBoxStruct->hListWnd,LB_SELECTSTRING,wParam,lParam);
	 if (iIndex==LB_ERR)
		 return CB_ERR;
	 SetNewEditText(hWnd);
     return iIndex;
}
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	short iIndex;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		 lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		 if (lpComboBoxStruct==NULL)
			 return FALSE;
		 iIndex=(short)SendMessage(lpComboBoxStruct->hListWnd,LB_SETCURSEL,wParam,lParam);
		 if (iIndex==LB_ERR)
			 return CB_ERR;
		 SetNewEditText(hWnd);
		 return iIndex;
}
static LRESULT DoShowDropDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  BOOL bShow;
  DWORD dwStyle;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
    if ((dwStyle&CBS_TYPEMASK)==CBS_SIMPLE)
      return TRUE;
    bShow=(BOOL)wParam;
    if (bShow)
    {
      PopupList(hWnd);
    }
    else
    {
      RetractPopList(hWnd);
    }
    return TRUE;
}
static LRESULT DoGetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETITEMDATA,wParam,lParam);
}
static LRESULT DoSetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETITEMDATA,wParam,lParam);
}
static LRESULT DoGetDroppedControlRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  LPRECT lprect;


    lpComboBoxStruct=malloc(sizeof(COMBOBOXSTRUCT));
    if (lpComboBoxStruct==NULL)
      return FALSE;

#ifdef _MAPPOINTER
	lParam = MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

	lprect=(LPRECT)lParam;
/*    lprect->left=lpComboBoxStruct->rect.left;
    lprect->top=lpComboBoxStruct->rect.top+TITLEHEIGHT;
    lprect->right=lpComboBoxStruct->rect.right;
    lprect->bottom=lpComboBoxStruct->rect.bottom;*/
	GetWindowRect(lpComboBoxStruct->hListWnd,lprect);
    return TRUE;
}
static LRESULT DoSetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd; 
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETITEMHEIGHT,wParam,lParam);
}
static LRESULT DoGetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETITEMHEIGHT,wParam,lParam);
}
static LRESULT DoSetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    return CB_ERR;
}
static LRESULT DoGetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    return TRUE;
}
static LRESULT DoGetDroppedState(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
       return FALSE;
    return lpComboBoxStruct->iDropState;
}
static LRESULT DoFindStringExact(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_FINDSTRINGEXACT,wParam,lParam);
}
static LRESULT DoSetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETLOCALE,wParam,lParam);
}
static LRESULT DoGetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETLOCALE,wParam,lParam);
}
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTOPINDEX,wParam,lParam);
}
static LRESULT DoSetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETTOPINDEX,wParam,lParam);
}
static LRESULT DoGetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETHORIZONTALEXTENT,wParam,lParam);
}
static LRESULT DoSetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETHORIZONTALEXTENT,wParam,lParam);
}
static LRESULT DoGetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
       return 0;
    return (LRESULT)(lpComboBoxStruct->rect.right-lpComboBoxStruct->rect.left);
}
static LRESULT DoSetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  short iWidth;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
       return 0;
	iWidth=(short)wParam;
	lpComboBoxStruct->rect.right=lpComboBoxStruct->rect.left+iWidth;
    return iWidth;
}


static LRESULT DoInitStorage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd,hEditWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
//   return SendMessage(hListWnd,LB_INITSTORAGE,wParam,lParam);
    return 0;
}
static LRESULT DoNotifySelChange(HWND hWnd)
{
   RetractPopList(hWnd);
   SetNewEditText(hWnd);
   SendNotifiedMsg(hWnd,CBN_SELCHANGE);
   return 0;
}
static LRESULT DoNotifyDblClk(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_DBLCLK);
   return 0;
}
static LRESULT DoNotifyEditChange(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
   return 0;
}
static LRESULT DoNotifyEditUpdate(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_EDITUPDATE);
   return 0;
}
static LRESULT DoNotifyErrSpace(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_ERRSPACE);
   return 0;
}
/****************************************************************************************************/
static void PopupList(HWND hWnd)
{
//  RECT rect;
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
//  HWND hListWnd;
  MSG msg;
  RECT rect, rcClient;
  POINT point;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
       return;
//    hListWnd=GetDlgItem(hWnd,ID_LIST);
// !!! Modified By jami chen in 2002.04.24
//    GetWindowRect(hWnd,&rect);
// !!! Modified to
	GetDropListPosition(hWnd,lpComboBoxStruct->hListWnd,&point);
// !!! Modified end By Jami chen 2002.04.24
    GetClientRect(hWnd,&rcClient);
// !!! Modified By jami chen in 2002.04.24
	/*
    SetWindowPos( 
                  lpComboBoxStruct->hListWnd,
                  0,
                  rect.left,
                  rect.bottom,
                  0,
                  0,
                  SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOACTIVATE);
	*/
    SetWindowPos( 
                  lpComboBoxStruct->hListWnd,
                  0,
                  point.x,
                  point.y,
                  0,
                  0,
                  SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOACTIVATE);
// !!! Modified End By Jami chen 2002.04.24
//                  rect.right-rect.left,
//                  rcClient.bottom,
//                  SWP_NOZORDER|SWP_NOREDRAW);


	//ShowWindow(lpComboBoxStruct->hListWnd,SW_SHOW);
	lpComboBoxStruct->iDropState=CB_DROPDOWN;
    SendNotifiedMsg(hWnd,CBN_DROPDOWN);
//    SetCapture(hListWnd);
	SetFocus(lpComboBoxStruct->hListWnd);

    lpComboBoxStruct->bRetracted=FALSE;
    while(!lpComboBoxStruct->bRetracted)
    {
//      if (PeekMessage(&msg, 0, 0, 0 ,PM_NOREMOVE)==TRUE)
      if (GetMessage( &msg, 0, 0, 0 )==TRUE)
      {
//        GetMessage( &msg, 0, 0, 0 );
        if (msg.message==WM_CLOSE)
        {
            RetractPopList(hWnd);
            break;
        }
        if (msg.message==WM_LBUTTONDOWN||
            msg.message==WM_NCLBUTTONDOWN)
        {
//          point.x=LOWORD(msg.lParam);
//          point.y=HIWORD(msg.lParam);
			GetWindowRect(lpComboBoxStruct->hListWnd,&rect);
          if (PtInRect(&rect,msg.pt)==0)
          {
            RetractPopList(hWnd);
            break;
          }
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
      }
    }
}
static void RetractPopList(HWND hWnd)
{
//  RECT rect;

	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	DWORD dwStyle;
//	HWND hListWnd;


		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		if (lpComboBoxStruct==NULL)
			 return;
// !!! add By Jami chen in 2002.04.24
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if ((dwStyle&CBS_TYPEMASK)==CBS_SIMPLE)
		{// the simple type is not Retract pop list
			return;
		}
// !!! add end By Jami chen in 2002.04.24
//    GetClientRect(hWnd,&rect);
//    SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right,TITLEHEIGHT,SWP_NOMOVE|SWP_NOZORDER);
//		hListWnd=GetDlgItem(hWnd,ID_LIST);
		ShowWindow(lpComboBoxStruct->hListWnd,SW_HIDE);
		lpComboBoxStruct->iDropState=CB_UNDROPDOWN;
		lpComboBoxStruct->bRetracted=TRUE;
//    SetCapture(0);
		SendNotifiedMsg(hWnd,CBN_CLOSEUP);
}

static void SetNewEditText(HWND hWnd)
{
  HWND hEditWnd;
  short indexCurSel;
  short chTextLen;
  char *lpItemText;

//   hParentWnd=GetParent(hWnd);
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);

//   RetractPopList(hParentWnd);
   indexCurSel=(short)SendMessage(hWnd,CB_GETCURSEL,0,0);
   if (indexCurSel==CB_ERR)
      return ;
   chTextLen=(short)SendMessage(hWnd,CB_GETLBTEXTLEN,indexCurSel,0);
   if (chTextLen==-1)	
	   return;
   lpItemText=malloc(chTextLen+1);
   if (lpItemText==NULL)
	   return ;
   SendMessage(hWnd,CB_GETLBTEXT,indexCurSel,(LPARAM)lpItemText);
   SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)lpItemText);
   free(lpItemText);
//   SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)lpItemText);
//   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
}
static void DrawDropButtonEdge(HWND hWnd,HDC hdc)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  RECT rect;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
      if (lpComboBoxStruct==NULL)
         return;
      rect=lpComboBoxStruct->rectButton;
	  // !!! Modified By Jami chen 2002.04.24
	  /*
	  Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
	  InflateRect( &rect, -1, -1 );
      if( lpComboBoxStruct->state & CB_SUNKENOUTER )  // highlight state
			DrawEdge( hdc, &rect, BDR_SUNKENOUTER, BF_RECT | BF_MIDDLE );
	  else  // normal state
			DrawEdge( hdc, &rect, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE );
	  */
	  //Rectangle( hdc, rect.left-1, rect.top, rect.right, rect.bottom );//LN:2003-04-21,删除
	  // rect.left-1  will use same border with edit or static

// !!! Modified End By Jami chen 2002.04.24	  
	  //InflateRect( &rect, -1, -1 );//LN:2003-04-21,删除
      //SetBkMode(hdc,TRANSPARENT);//LN:2003-04-21,删除
      //DrawText(hdc,"",1,&rect,DT_CENTER);//LN:2003-04-21,删除

	  {  //LN:2003-04-21,增加-开始
		  BYTE bDownArrow = SYM_DOWN;
		  HFONT hfSave;
		  HBRUSH hBrush;
		  // draw button face
		  //FillRect( hdc, &rect, GetSysColorBrush(COLOR_BTNFACE) );
		  hBrush = SelectObject( hdc, GetSysColorBrush(COLOR_BTNFACE) );
		  Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
		  SelectObject( hdc, hBrush );
		  // draw button text
		  hfSave = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL16X16) );
		  SetBkMode(hdc,TRANSPARENT);
		  DrawText( hdc, &bDownArrow, 1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		  SelectObject( hdc, hfSave );
	  }  //LN:2003-04-21,增加-结束

}
/**************************************************************************/
static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg)
{
  HWND hParent;
  WPARAM wParam;
  UINT idComboBox;

     hParent=GetParent(hWnd);
     idComboBox=(UINT)GetWindowLong(hWnd,GWL_ID);
     wParam=MAKEWPARAM(idComboBox,iNotifiedMsg);
     return SendMessage(hParent,WM_COMMAND,wParam,(LPARAM)hWnd);
}

/**************************************************************************/
/*static LRESULT DoListLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINT point;
    HWND hParentWnd;
    RECT rect;

      return 0;
      point.x=LOWORD(lParam);
      point.y=HIWORD(lParam);

      if (GetCapture()==hWnd)
      {
        GetClientRect(hWnd,&rect);
//        GetWindowRect(hWnd,&rect);
        if (PtInRect(&rect,point)==0)
        {
//          hParentWnd=GetParent(hWnd);
//          RetractPopList(hParentWnd);
          return 0;
        }
      }
		  return 1;
}
*/
/****************************************************************************************************/
/*
static LRESULT DoListMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	POINTS points;
//      points.x=LOWORD(lParam);
//      points.y=HIWORD(lParam);
		return 1;
}
*/
/****************************************************************************************************/
/*
static LRESULT DoListLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//		POINTS points;

//      points.x=LOWORD(lParam);
//      points.y=HIWORD(lParam);

			return 1;
}
*/
/**************************************************************************/

//static void GetDropListRect(HWND hWnd,LPRECT lprect)//LN:2003-04-21,删除
static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight) //LN:2003-04-21,增加
{
	RECT rectWindow;
	int heightDropList;
	
		GetWindowRect(hWnd,&rectWindow);
		heightDropList=rectWindow.bottom-rectWindow.top-iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,修改
		if (rectWindow.bottom>GetSystemMetrics(SM_CYSCREEN))
		{
			// the drop list can't put the combobox under,then over this
			rectWindow.top-=heightDropList;
			rectWindow.bottom=rectWindow.top+heightDropList;
		}
		else
		{
			rectWindow.top+=iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,修改
			//rectWindow.bottom=rectWindow.top+heightDropList;
		}
		*lprect=rectWindow;
		return;
}
static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint)
{
	RECT rectWindow,rectList;
	int heightDropList;
	
		GetWindowRect(hWnd,&rectWindow);
		GetClientRect(hListWnd,&rectList);
		heightDropList=rectList.bottom-rectList.top;
		if (rectWindow.bottom+heightDropList>GetSystemMetrics(SM_CYSCREEN))
		{
			// the drop list can't put the combobox under,then over this
			lppoint->x=rectWindow.left;
			lppoint->y=rectWindow.top-heightDropList;
		}
		else
		{
			lppoint->x=rectWindow.left;
			lppoint->y=rectWindow.bottom;
		}
		return;
}
