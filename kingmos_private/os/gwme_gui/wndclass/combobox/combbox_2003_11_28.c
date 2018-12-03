/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：组合框类
版本号：1.0.0
开发时期：2000-01-10
作者：陈建明
修改记录：
    2003-07-07: 1.将 List重新改为 WS_POPUP

    2003-06-05: 1.去掉某些 不要的code. 
	            2. 将 List改为 WS_CHILD , 原来是WS_POPUP, by LN
    2003-04-21: 将下拉按钮的尺寸改为从系统得到, by LN
******************************************************/

#include <eframe.h>
#include <ecombo.h>
#include <eedit.h>
#include <edialog.h>
#include <esymbols.h>
#include <gwmesrv.h>

#define COMBOBOX classCOMBOBOX

static const char classCOMBOBOX[] = "ComboBox";

#define _MAPPOINTER

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
				WNDPROC lpEditProc;
				WNDPROC lpListProc;

				COLORREF cl_Title;
				COLORREF cl_TitleBk;

				COLORREF cl_Disable;
				COLORREF cl_DisableBk;
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
static LRESULT DoSetText(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKillFocus( HWND hWnd, HWND hGetWnd );
static LRESULT DoCtrlKillFocus( LPCOMBOBOXSTRUCT lpComboBoxStruct, HWND hParent, HWND hGetWnd ) ;
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
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

/*********************************************************************************************************/

static void PopupList(HWND hWnd);
static void RetractPopList(HWND hWnd);
static void SetNewEditText(HWND hWnd);
static void DrawDropButtonEdge(HWND hWnd,HDC hdc);
static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg);
static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight);//LN, 2003-06-05 , DEL//LN:2003-04-21,修改
static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint);
static int GetBorderWidth(HWND hWnd);

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
//	wcex.hbrBackground	= 0;
    wcex.hbrBackground = GetStockObject( COLOR_STATIC );
	// no menu
	wcex.lpszMenuName	= NULL; // no menu ,add to future
	// class name
	wcex.lpszClassName	= COMBOBOX;
	// no small icon
//	wcex.hIconSm		= NULL;

	// register this class
	return RegisterClass(&wcex);
}

LRESULT CALLBACK HookProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HWND hParent = GetParent(hWnd);
	LPCOMBOBOXSTRUCT lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong( hParent, 0 );
	DWORD dwCtrlId;

	switch( message )
	{
	case WM_KILLFOCUS:
		{
			DoCtrlKillFocus( lpComboBoxStruct, hParent, (HWND)wParam );
		}
	}
	dwCtrlId = GetWindowLong( hWnd, GWL_ID );

	if( dwCtrlId == ID_EDIT )
	    return CallWindowProc( lpComboBoxStruct->lpEditProc, hWnd, message, wParam, lParam );
	else
		return CallWindowProc( lpComboBoxStruct->lpListProc, hWnd, message, wParam, lParam );
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
				 case LBN_KILLFOCUS:
					  ;//DoNotifyKillFocus( hWnd )
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
			return DoKillFocus( hWnd, (HWND)wParam );//,lParam);
		case WM_DESTROY:
			// close this window
			ReleaseComboBox(hWnd);
//			PostQuitMessage(0);
			break;
		case WM_CLEAR:
			return DoClear(hWnd,wParam,lParam);
    case WM_GETTEXT:
      return DoGetText(hWnd,wParam,lParam);
    case WM_SETTEXT:
      return DoSetText(hWnd,wParam,lParam);
    case WM_GETTEXTLENGTH:
      return DoGetTextLength(hWnd,wParam,lParam);
    case WM_WINDOWPOSCHANGED:
      return DoWindowPosChanged(hWnd,wParam,lParam);

	case WM_SETCTLCOLOR	:
		return DoSetColor(hWnd,wParam,lParam);
	case WM_GETCTLCOLOR	:
		return DoGetColor(hWnd,wParam,lParam);
	case WM_STYLECHANGED:
		return DoStyleChanged(hWnd,wParam,lParam);

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
	HWND hCtrl;
    BOOL bEnable;

	int iTitleHeight = GetSystemMetrics(SM_CYCAPTION) -2;//LN:2003-04-21,增加
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
	lpComboBoxStruct->bRetracted=FALSE;
// !!! Modified By Jami chen in 2003.08.12
//	lpComboBoxStruct->cl_Title  = RGB(132,132,132);
	lpComboBoxStruct->cl_Title  = RGB(0,0,0);
// !!! Modified End By Jami chen in 2003.08.12
	lpComboBoxStruct->cl_TitleBk = RGB(240,240,240);

// !!! Modified By Jami chen in 2003.08.12
//	lpComboBoxStruct->cl_Disable = RGB(200,200,200);
	lpComboBoxStruct->cl_Disable = RGB(206,206,206);
// !!! Modified By Jami chen in 2003.08.12
	lpComboBoxStruct->cl_DisableBk = RGB(240,240,240);

    switch(dwStyle&CBS_TYPEMASK)
    {
    case CBS_DROPDOWN:
        hCtrl = CreateWindow(classEDIT,
				              "",
						      WS_CHILD|WS_VISIBLE,//|WS_BORDER,
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
		lpComboBoxStruct->lpEditProc = (WNDPROC)SetWindowLong( hCtrl, GWL_WNDPROC, (LONG)HookProc );
        break;
	case CBS_SIMPLE:
		CreateWindow(classEDIT,
			"",
			WS_CHILD|WS_VISIBLE,//|WS_BORDER,
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
        hCtrl = CreateWindow( classSTATIC,
			"",
						      WS_CHILD|WS_VISIBLE,//|WS_BORDER,
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
		lpComboBoxStruct->lpEditProc = (WNDPROC)SetWindowLong( hCtrl, GWL_WNDPROC, (LONG)HookProc );
		break;
	default :
		return -1;
	}
// !!! Add By Jami chen in 2003 . 08.30
//  if the comboBox is disable , then the child control must be disable
	bEnable = (dwStyle & WS_DISABLED) ? FALSE : TRUE; 
	EnableWindow(hCtrl,bEnable);
// !!! Add End By Jami chen in 2003 . 08.30

//						  WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_HASSTRINGS|LBS_NOTIFY,
	{
		DWORD dwListStyle;
		
		if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
			dwListStyle=WS_POPUP|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;//LN, 2003-06-05, 将WS_POPUP 改为WS_CHILD, 去掉WS_VISIBLE
//			dwListStyle=WS_CHILD|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;//LN, 2003-06-05, 将WS_POPUP 改为WS_CHILD, 去掉WS_VISIBLE
		else
			dwListStyle=WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;
		
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
		  

		  if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
		  {
			  GetDropListRect(hWnd,&rectWindow,iTitleHeight);//LN:2003-04-21,修改
			  lpComboBoxStruct->hListWnd=CreateWindow("COMBOLISTBOX",
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
		  }
		  else
		  {
			lpComboBoxStruct->hListWnd=CreateWindow("COMBOLISTBOX",
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

	    SetWindowLong(hWnd,0,(long)lpComboBoxStruct);

		if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
		{
			int iBorderWidth;
			lpComboBoxStruct->bChangedSize=TRUE;
			//SetWindowPos(hWnd,0,0,0,rectWindow.right - rectWindow.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			// !!! Add By Jami chen in 2003.07.07
			iBorderWidth = GetBorderWidth(hWnd);
			// !!! Add End By Jami chen in 2003.07.07
			SetWindowPos(hWnd,0,0,0,rect.right - rect.left + 2 * iBorderWidth,iTitleHeight + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			lpComboBoxStruct->bChangedSize=FALSE;
			lpComboBoxStruct->lpListProc = (WNDPROC)SetWindowLong( lpComboBoxStruct->hListWnd, GWL_WNDPROC, (LONG)HookProc );			
			lpComboBoxStruct->iDropState=CB_UNDROPDOWN;
			lpComboBoxStruct->bRetracted=TRUE;
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
	if( lpComboBoxStruct->hListWnd )
        SetWindowLong( lpComboBoxStruct->hListWnd, GWL_WNDPROC, (LONG)lpComboBoxStruct->lpListProc );
	if( GetDlgItem(hWnd,ID_EDIT) )
	    SetWindowLong( GetDlgItem(hWnd,ID_EDIT), GWL_WNDPROC, (LONG)lpComboBoxStruct->lpEditProc );
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
		if( lpComboBoxStruct->bRetracted == TRUE )
            PopupList(hWnd);
		else
			RetractPopList(hWnd);
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

static LRESULT DoKillFocus( HWND hWnd, HWND hGetWnd )//WPARAM wParam,LPARAM lParam)
{
    LPCOMBOBOXSTRUCT lpComboBoxStruct;

	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);

	if( (HWND)hGetWnd == (HWND)GetDlgItem(hWnd,ID_EDIT) ||
		(HWND)hGetWnd == lpComboBoxStruct->hListWnd )
	{
		;
	}
    else
	{
		RetractPopList( hWnd );
	    SendNotifiedMsg(hWnd,CBN_KILLFOCUS);
	}
	return 0;
}

static LRESULT DoCtrlKillFocus( LPCOMBOBOXSTRUCT lpComboBoxStruct, HWND hParent, HWND hGetWnd ) 
{
	if( (HWND)hGetWnd == (HWND)GetDlgItem(hParent,ID_EDIT) ||
		(HWND)hGetWnd == lpComboBoxStruct->hListWnd ||
		hGetWnd == hParent )
	{
		;
	}
    else
	{
		RetractPopList( hParent );
		SendNotifiedMsg(hParent,CBN_KILLFOCUS);
	}
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
static LRESULT DoSetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_SETTEXT,wParam,lParam);
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
	RECT rect;//,rectWindow;
	DWORD dwStyle;
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL) - 2;//LN:2003-04-21,增加
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
			    //GetWindowRect(hWnd,&rectWindow);
//		    SetWindowPos(hWnd,0,0,0,rect.right - rect.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			{
				int iBorderWidth;
				// !!! Add By Jami chen in 2003.07.07
				iBorderWidth = GetBorderWidth(hWnd);
				// !!! Add End By Jami chen in 2003.07.07
				SetWindowPos(hWnd,0,0,0,rect.right - rect.left + 2 * iBorderWidth,iTitleHeight + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			}
			lpComboBoxStruct->bChangedSize=FALSE;
			//change the popup list size
		    SetWindowPos( 
						  lpComboBoxStruct->hListWnd,
						  0,
						  (short)rect.left,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  (short)(rect.right-rect.left),
						  (short)(rect.bottom-rect.top-iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
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
//		RETAILMSG(1,(TEXT("Add Address <%s>!!!\r\n"),(LPTSTR)lParam));
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
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
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
	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	
    
	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
    if (lpComboBoxStruct==NULL)
		return;
/*	// LN, 2003-06-05, ADD-begin
	lpComboBoxStruct->iDropState=CB_DROPDOWN;
	lpComboBoxStruct->bChangedSize = TRUE;
	SetWindowPos( hWnd, NULL, 0, 0, 
		          lpComboBoxStruct->rect.right - lpComboBoxStruct->rect.left,
				  lpComboBoxStruct->rect.bottom - lpComboBoxStruct->rect.top,
				  SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE );
	lpComboBoxStruct->bChangedSize = FALSE;
    SendNotifiedMsg(hWnd,CBN_DROPDOWN);
	ShowWindow(lpComboBoxStruct->hListWnd, SW_SHOW );
	SetFocus(lpComboBoxStruct->hListWnd);	
    lpComboBoxStruct->bRetracted=FALSE;

	// LN, 2003-06-05, ADD-end
*/
    // LN, 2003-06-05, DEL-begin
	{
		MSG msg;
		RECT rect, rcClient;
		POINT point;

		GetDropListPosition(hWnd,lpComboBoxStruct->hListWnd,&point);
		GetClientRect(lpComboBoxStruct->hListWnd,&rcClient);
		GetClientRect(hWnd,&rcClient);
		SetWindowPos( 
			lpComboBoxStruct->hListWnd,
			0,
			point.x,
			point.y,
			0,
			0,
			SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOACTIVATE);
		
		lpComboBoxStruct->iDropState=CB_DROPDOWN;
		SendNotifiedMsg(hWnd,CBN_DROPDOWN);
		SetFocus(lpComboBoxStruct->hListWnd);
		
		lpComboBoxStruct->bRetracted=FALSE;
		while(!lpComboBoxStruct->bRetracted)
		{
			if (GetMessage( &msg, 0, 0, 0 )==TRUE)
			{
				if (msg.message==WM_CLOSE)
				{
					RetractPopList(hWnd);
					break;
				}
				if (msg.message==WM_LBUTTONDOWN||
					msg.message==WM_NCLBUTTONDOWN)
				{
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
	// LN, 2003-06-05, DEL-end
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
		lpComboBoxStruct->bChangedSize = TRUE;
		{
			int iBorderWidth;
			// !!! Add By Jami chen in 2003.07.07
			iBorderWidth = GetBorderWidth(hWnd);
			//SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right,GetSystemMetrics(SM_CYCAPTION),SWP_NOMOVE|SWP_NOZORDER);
			SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right - lpComboBoxStruct->rect.left + 2 * iBorderWidth,GetSystemMetrics(SM_CYCAPTION) - 2  + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
		}
		lpComboBoxStruct->bChangedSize = FALSE;
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
  char *lpItemText,*lpMapPtr;

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
#ifdef _MAPPOINTER
	lpMapPtr = MapPtrToProcess( (LPVOID)lpItemText, GetCurrentProcess() ); 
#endif
   SendMessage(hWnd,CB_GETLBTEXT,indexCurSel,(LPARAM)lpMapPtr);
   SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)lpMapPtr);
   free(lpItemText);
//   SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)lpItemText);
//   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
}
static void DrawDropButtonEdge(HWND hWnd,HDC hdc)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  RECT rect;
  DWORD dwStyle;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
      if (lpComboBoxStruct==NULL)
         return;
	  dwStyle = GetWindowLong(hWnd,GWL_STYLE);
	  GetWindowRect(hWnd,&rect);
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
//		  HBRUSH hBrush;
		  // draw button face
		  //FillRect( hdc, &rect, GetSysColorBrush(COLOR_BTNFACE) );
// !!! Modified By Jami chen in 2003.07.07
/*		  hBrush = SelectObject( hdc, GetSysColorBrush(COLOR_BTNFACE) );
		  Rectangle( hdc, rect.left, rect.top, rect.right, rect.bottom );
		  SelectObject( hdc, hBrush );
*/
		  if( lpComboBoxStruct->state & CB_SUNKENOUTER )  // highlight state
				DrawEdge( hdc, &rect, BDR_SUNKENOUTER, BF_RECT | BF_MIDDLE );
		  else  // normal state
				DrawEdge( hdc, &rect, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE );
		  InflateRect( &rect, -1, -1 );
// !!! Modified By Jami chen in 2003.07.07
		  // draw button text
		  if (dwStyle & WS_DISABLED)
		  {
		 	 SetTextColor(hdc,lpComboBoxStruct->cl_Disable);
		  }
		  else
		  {
			 SetTextColor(hdc,lpComboBoxStruct->cl_Title);
		  }
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
// LN, 2003-06-05 , DEL
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


static int GetBorderWidth(HWND hWnd)
{
	return 1;
}


static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
	HWND hEditWnd;

		  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		  if (lpComboBoxStruct==NULL)
			 return FALSE;
		
	     SendMessage(lpComboBoxStruct->hListWnd,WM_SETCTLCOLOR,wParam,lParam);

		 hEditWnd=GetDlgItem(hWnd,ID_EDIT);
	     SendMessage(hEditWnd,WM_SETCTLCOLOR,wParam,lParam);

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpComboBoxStruct->cl_Disable = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpComboBoxStruct->cl_DisableBk = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {
				lpComboBoxStruct->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {
				lpComboBoxStruct->cl_TitleBk = lpCtlColor->cl_TitleBk ;
		 }
		return TRUE;
}
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;

		  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);
		  if (lpComboBoxStruct==NULL)
			 return FALSE;

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpCtlColor->cl_Disable= lpComboBoxStruct->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpCtlColor->cl_DisableBk = lpComboBoxStruct->cl_DisableBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {
				lpCtlColor->cl_Title= lpComboBoxStruct->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {
				lpCtlColor->cl_TitleBk = lpComboBoxStruct->cl_TitleBk ;
		 }

		return TRUE;
}

static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  DWORD wStyleType;
//  LPSTYLESTRUCT lpStyleStruct;
  BOOL bEnable;
  DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE);

/*  
	wStyleType = wParam;
	if (wStyleType == GWL_STYLE)
	{
	   lpStyleStruct = (LPSTYLESTRUCT)lParam;
	   if ((lpStyleStruct->styleNew & WS_DISABLED) != (lpStyleStruct->styleOld & WS_DISABLED))
	   {
			HWND hEditWnd;

				hEditWnd=GetDlgItem(hWnd,ID_EDIT);
				bEnable = (lpStyleStruct->styleNew & WS_DISABLED) ? FALSE : TRUE; 
				EnableWindow(hEditWnd,bEnable);
	   }
	}
*/
	HWND hEditWnd;

		hEditWnd=GetDlgItem(hWnd,ID_EDIT);
		bEnable = (dwStyle & WS_DISABLED) ? FALSE : TRUE; 
		EnableWindow(hEditWnd,bEnable);
		return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}
