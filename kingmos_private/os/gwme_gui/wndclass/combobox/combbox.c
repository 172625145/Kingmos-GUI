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
//#include <gwmesrv.h>

#define COMBOBOX classCOMBOBOX

static const char classCOMBOBOX[] = "ComboBox";

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

// **************************************************
// 声明：ATOM RegisterComboBoxClass(HINSTANCE hInstance)
// 参数：
//	IN hInstance -- 实例句柄
// 返回值：返回注册结果
// 功能描述：注册组合框
// 引用: 
// **************************************************
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
//  wcex.hbrBackground = GetStockObject( COLOR_STATIC );
	// no menu
	wcex.lpszMenuName	= NULL; // no menu ,add to future
	// class name
	wcex.lpszClassName	= COMBOBOX;
	// no small icon
//	wcex.hIconSm		= NULL;

	// register this class
	return RegisterClass(&wcex);
}

// **************************************************
// 声明：LRESULT CALLBACK HookProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN message -- 要处理的消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 返回值：返回消息处理结果
// 功能描述：编辑窗口预处理过程函数
// 引用: 
// **************************************************
LRESULT CALLBACK HookProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HWND hParent = GetParent(hWnd);
	LPCOMBOBOXSTRUCT lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong( hParent, 0 ); // 得到组合框的结构句柄
	DWORD dwCtrlId;

	if (lpComboBoxStruct == NULL)
		return 0;
	switch( message )
	{
	case WM_KILLFOCUS:
		{
			DoCtrlKillFocus( lpComboBoxStruct, hParent, (HWND)wParam ); // 处理失去焦点过程
		}
	}
	dwCtrlId = GetWindowLong( hWnd, GWL_ID );

	// 回调原来编辑区的过程函数
	if( dwCtrlId == ID_EDIT )
	    return CallWindowProc( lpComboBoxStruct->lpEditProc, hWnd, message, wParam, lParam );
	else
		return CallWindowProc( lpComboBoxStruct->lpListProc, hWnd, message, wParam, lParam );
}


// **************************************************
// 声明：LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN message -- 要处理的消息
// 	IN wParam -- 消息参数
// 	IN lParam -- 消息参数
// 返回值：返回消息处理结果
// 功能描述：组合框窗口处理消息过程函数
// 引用: 
// **************************************************
LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
  WORD wmId,wmEvent;

	switch (message)
	{
		case WM_CREATE: // 创建组合框
			return DoCreateComboBox(hWnd,wParam,lParam);
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam); 
      switch(wmId)
      {
         case ID_LIST:
              switch(wmEvent)
              {
                 case LBN_SELCHANGE: // 发送选择改变的通知消息
                      DoNotifySelChange(hWnd);
                      break;
                 case LBN_DBLCLK: // 发送双击的通知消息
                      DoNotifyDblClk(hWnd);
                      break;
                 case LBN_ERRSPACE: // 发送空间不足的通知消息
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
                 case EN_CHANGE: //发送编辑文本改变的通知消息
                      DoNotifyEditChange(hWnd);
                      break;
                 case EN_UPDATE: //发送编辑文本更新的通知消息
                      DoNotifyEditUpdate(hWnd);
                      break;
                 case EN_ERRSPACE: //发送空间不足的通知消息
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
   	case WM_LBUTTONDOWN: // 鼠标左键按下
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE: // 鼠标移动
			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP: // 鼠标左键弹起
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_SETFOCUS: // 设置焦点
      return DoSetFocus(hWnd,wParam,lParam);
    case WM_KILLFOCUS: // 杀死焦点
			return DoKillFocus( hWnd, (HWND)wParam );//,lParam);
		case WM_DESTROY:
			// close this window
			ReleaseComboBox(hWnd);
//			PostQuitMessage(0);
			break;
		case WM_CLEAR: // 清除
			return DoClear(hWnd,wParam,lParam);
    case WM_GETTEXT: // 得到文本
      return DoGetText(hWnd,wParam,lParam);
    case WM_SETTEXT: // 设置文本
      return DoSetText(hWnd,wParam,lParam);
    case WM_GETTEXTLENGTH: // 得到文本长度
      return DoGetTextLength(hWnd,wParam,lParam);
    case WM_WINDOWPOSCHANGED: // 窗口位置改变
      return DoWindowPosChanged(hWnd,wParam,lParam);

	case WM_SETCTLCOLOR	: // 设置颜色
		return DoSetColor(hWnd,wParam,lParam);
	case WM_GETCTLCOLOR	: // 得到颜色
		return DoGetColor(hWnd,wParam,lParam);
	case WM_STYLECHANGED: // 风格改变
		return DoStyleChanged(hWnd,wParam,lParam);

	case CB_GETEDITSEL: // 得到编辑去选择
      return DoGetEditSel(hWnd,wParam,lParam);

	case CB_LIMITTEXT: // 设置组合框中编辑区的文本限制长度
      return DoLimitText(hWnd,wParam,lParam);
    case CB_SETEDITSEL: // 设置编辑控件的选择
      return DoSetEditSel(hWnd,wParam,lParam);
    case CB_ADDSTRING: // 添加一个字符串到控件中
      return DoAddString(hWnd,wParam,lParam);
    case CB_DELETESTRING: // 从控件中删除一个字符串
      return DoDeleteString(hWnd,wParam,lParam);
    case CB_DIR: // 保留
      return DoDir(hWnd,wParam,lParam);
    case CB_GETCOUNT: // 得到列示的条目数目
      return DoGetCount(hWnd,wParam,lParam);
    case CB_GETCURSEL: // 得到列示的当前的选择条目的索引
      return DoGetCurSel(hWnd,wParam,lParam);
    case CB_GETLBTEXT: // 得到指定索引的条目的文本
      return DoGetLBText(hWnd,wParam,lParam);
    case CB_GETLBTEXTLEN: // 得到指定索引的条目的文本长度
      return DoGetLBTextLen(hWnd,wParam,lParam);
    case CB_INSERTSTRING: // 插入一个字符串到控件
      return DoInsertString(hWnd,wParam,lParam);
    case CB_RESETCONTENT: // 重设控件内容
      return DoResetContent(hWnd,wParam,lParam);
    case CB_FINDSTRING: // 在组合框中的列示中从指定的条目开始查找指定的字串
      return DoFindString(hWnd,wParam,lParam);
    case CB_SELECTSTRING: // 在组合框中的列示中查找并设定该条目为选择条目
      return DoSelectString(hWnd,wParam,lParam);
    case CB_SETCURSEL: // 设置当前的选择条目
      return DoSetCurSel(hWnd,wParam,lParam);
    case CB_SHOWDROPDOWN: // 显示下拉列示框
      return DoShowDropDown(hWnd,wParam,lParam);
    case CB_GETITEMDATA: // 得到一个由用户提供的，用来与指定的条目关联的32位值
      return DoGetItemData(hWnd,wParam,lParam);
    case CB_SETITEMDATA: // 设置一个用来与指定的条目关联的32位值
      return DoSetItemData(hWnd,wParam,lParam);
    case CB_GETDROPPEDCONTROLRECT: // 得到下拉列示框的屏幕坐标
      return DoGetDroppedControlRect(hWnd,wParam,lParam);
    case CB_SETITEMHEIGHT: // 设定指定的条目的高度
      return DoSetItemHeight(hWnd,wParam,lParam);
    case CB_GETITEMHEIGHT: // 得到指定条目的高度
      return DoGetItemHeight(hWnd,wParam,lParam);
    case CB_SETEXTENDEDUI: // 保留
      return DoSetExtendEdui(hWnd,wParam,lParam);
    case CB_GETEXTENDEDUI: // 保留
      return DoGetExtendEdui(hWnd,wParam,lParam);
    case CB_GETDROPPEDSTATE: // 得到下拉列示的状态
      return DoGetDroppedState(hWnd,wParam,lParam);
    case CB_FINDSTRINGEXACT: // 查找字符串
      return DoFindStringExact(hWnd,wParam,lParam);
    case CB_SETLOCALE: // 保留
      return DoSetLocale(hWnd,wParam,lParam);
    case CB_GETLOCALE: // 保留
      return DoGetLocale(hWnd,wParam,lParam);
    case CB_GETTOPINDEX: // 在组合框中的列示中得到第一条可见的条目
      return DoGetTopIndex(hWnd,wParam,lParam);
    case CB_SETTOPINDEX: // 在组合框中的列示中设置第一条可见的条目
      return DoSetTopIndex(hWnd,wParam,lParam);
    case CB_GETHORIZONTALEXTENT: // 保留
      return DoGetHorizontalExtent(hWnd,wParam,lParam);
    case CB_SETHORIZONTALEXTENT: // 保留
      return DoSetHorizontalExtent(hWnd,wParam,lParam);
    case CB_GETDROPPEDWIDTH: // 得到下拉列示框的宽度
      return DoGetDroppedWidth(hWnd,wParam,lParam);
    case CB_SETDROPPEDWIDTH: // 设定下拉列示框的宽度
      return DoSetDroppedWidth(hWnd,wParam,lParam);
    case CB_INITSTORAGE: // 保留
      return DoInitStorage(hWnd,wParam,lParam);
	//2005-09-20, add for WS_GROUP by lilin
	case DLGC_WANTALLKEYS:
		return DLGC_WANTALLKEYS;
	//
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
// **************************************************
// 声明：static LRESULT DoCreateComboBox(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功返回0，否则返回-1
// 功能描述：创建组合框，处理WM_CREATE消息。
// 引用: 
// **************************************************
static LRESULT DoCreateComboBox(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
	RECT rect,rectWindow;
	DWORD dwStyle;
	HINSTANCE hInst;
	HWND hCtrl;
    BOOL bEnable;

//	int iTitleHeight = GetSystemMetrics(SM_CYCAPTION) -2;//LN:2003-04-21,增加
	int iTitleHeight = GetSystemMetrics(SM_CYSYSFONT)  + 2;//LN:2003-04-21,增加
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL);//LN:2003-04-21,增加

	lpComboBoxStruct=malloc(sizeof(COMBOBOXSTRUCT)); // 分配结构指针
	if (lpComboBoxStruct==NULL)
		return -1;//FALSE;//LN:2003-04-21,修改
	hInst=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // 得到实例句柄
	dwStyle=GetWindowLong(hWnd,GWL_STYLE);
	GetClientRect(hWnd,&rect);

    lpComboBoxStruct->rect=rect;
    lpComboBoxStruct->iDropState=CB_DROPDOWN;
	lpComboBoxStruct->bChangedSize=FALSE;
	lpComboBoxStruct->bRetracted=FALSE;
	// 初始化颜色
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

	// 组合框属性
    switch(dwStyle&CBS_TYPEMASK)
    {
    case CBS_DROPDOWN: // 下拉框 + 编辑框
    	// 创建编辑框
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
		lpComboBoxStruct->lpEditProc = (WNDPROC)SetWindowLong( hCtrl, GWL_WNDPROC, (LONG)HookProc ); // 得到编辑框回调
        break;
	case CBS_SIMPLE:  // 列式框 + 编辑框
		// 创建编辑框
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
	case CBS_DROPDOWNLIST:  // 下拉框 + 静态框
		// 创建静态框
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
	// 得到列式框的风格
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
		  
		// 创建列式框
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
		{  // 隐藏列式框
			int iBorderWidth;
			lpComboBoxStruct->bChangedSize=TRUE;
			//SetWindowPos(hWnd,0,0,0,rectWindow.right - rectWindow.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			// !!! Add By Jami chen in 2003.07.07
			iBorderWidth = GetBorderWidth(hWnd);
			// !!! Add End By Jami chen in 2003.07.07
			SetWindowPos(hWnd,0,0,0,rect.right - rect.left + 2 * iBorderWidth,iTitleHeight + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
			lpComboBoxStruct->bChangedSize=FALSE;
//			lpComboBoxStruct->lpListProc = (WNDPROC)SetWindowLong( lpComboBoxStruct->hListWnd, GWL_WNDPROC, (LONG)HookProc );			
			lpComboBoxStruct->iDropState=CB_UNDROPDOWN;
			lpComboBoxStruct->bRetracted=TRUE;
		}

		return 0;
}
/*********************************************************************************************************/
// ReleaseComboBoxMemory
/*********************************************************************************************************/
// **************************************************
// 声明：static void ReleaseComboBox(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 返回值：无
// 功能描述：释放组合框，处理WM_DESTROY消息。
// 引用: 
// **************************************************
static void ReleaseComboBox(HWND hWnd)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    
  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);  // 得到组合框的结构
    if (lpComboBoxStruct==NULL)
       return;
    // 恢复列式框的过程函数
//	if( lpComboBoxStruct->hListWnd )
//        SetWindowLong( lpComboBoxStruct->hListWnd, GWL_WNDPROC, (LONG)lpComboBoxStruct->lpListProc );
	// 恢复编辑区的过程函数
	if( GetDlgItem(hWnd,ID_EDIT) )
	    SetWindowLong( GetDlgItem(hWnd,ID_EDIT), GWL_WNDPROC, (LONG)lpComboBoxStruct->lpEditProc );
    free(lpComboBoxStruct); // 释放缓存
    SetWindowLong(hWnd,0,0);
}
/****************************************************************************************************/
/****************************************************************************************************/
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
    DrawDropButtonEdge(hWnd,hdc); // 绘制下凹边框
		return 0;
}
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
	POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
      if (lpComboBoxStruct==NULL)
         return 0;

      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      { // 指定点是否在按钮上
        lpComboBoxStruct->state=CB_SUNKENOUTER;
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc); // 绘制下拉按钮边框
        ReleaseDC(hWnd,hdc);
      }
      SetCapture(hWnd); // 抓住鼠标
      SetFocus(hWnd); // 设置焦点
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
		POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      if (GetCapture()!=hWnd)
        return 0;
      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
      if (lpComboBoxStruct==NULL)
         return 0;
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      { // 点在按钮上
        if (lpComboBoxStruct->state==CB_SUNKENOUTER)
          return 0;
        lpComboBoxStruct->state=CB_SUNKENOUTER; // 下按
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
      else
      { // 点不在按钮上
        if (lpComboBoxStruct->state==CB_RAISEDOUTER)
          return 0;
        lpComboBoxStruct->state=CB_RAISEDOUTER; // 弹起
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
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
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      if (GetCapture()!=hWnd)
        return 0;
      SetCapture(0);
      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
      if (lpComboBoxStruct==NULL)
         return 0;

      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      { // 点在按钮上
        lpComboBoxStruct->state=CB_RAISEDOUTER; // 弹起
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);		
		if( lpComboBoxStruct->bRetracted == TRUE )
            PopupList(hWnd); // 弹出下拉框
		else
			RetractPopList(hWnd);  // 收回下拉框
      }
			return 0;
}
// **************************************************
// 声明：static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置焦点到当前窗口，处理WM_SETFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
     hEditWnd=GetDlgItem(hWnd,ID_EDIT); // 得到编辑区
//     SendMessage(hWnd,WM_SETFOCUS,0,(LPARAM)hEditWnd);
     SetFocus(hEditWnd); // 设置焦点
     SendNotifiedMsg(hWnd,CBN_SETFOCUS); // 通知消息
     return 0;
}
/****************************************************************************************************/

// **************************************************
// 声明：static LRESULT DoKillFocus( HWND hWnd, HWND hGetWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hGetWnd -- 获得焦点的窗口
// 返回值：无
// 功能描述：窗口失去焦点，处理WM_KILLFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoKillFocus( HWND hWnd, HWND hGetWnd )//WPARAM wParam,LPARAM lParam)
{
    LPCOMBOBOXSTRUCT lpComboBoxStruct;

	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);  // 得到组合框的结构

	if( (HWND)hGetWnd == (HWND)GetDlgItem(hWnd,ID_EDIT) ||
		(HWND)hGetWnd == lpComboBoxStruct->hListWnd )
	{
		; // 原来焦点在编辑区或列式
	}
    else
	{
		RetractPopList( hWnd ); // 收回下拉框
	    SendNotifiedMsg(hWnd,CBN_KILLFOCUS); // 通知消息
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT DoCtrlKillFocus( LPCOMBOBOXSTRUCT lpComboBoxStruct, HWND hParent, HWND hGetWnd ) 
// 参数：
// 	IN lpComboBoxStruct -- 组合框数据结构
// 	IN hParent --　控件父窗口句柄
// 	IN hGetWnd -- 获得焦点的窗口
// 返回值：无
// 功能描述：子控件失去焦点，处理WM_KILLFOCUS消息。
// 引用: 
// **************************************************
static LRESULT DoCtrlKillFocus( LPCOMBOBOXSTRUCT lpComboBoxStruct, HWND hParent, HWND hGetWnd ) 
{
	if( (HWND)hGetWnd == (HWND)GetDlgItem(hParent,ID_EDIT) ||
		(HWND)hGetWnd == lpComboBoxStruct->hListWnd ||
		hGetWnd == hParent )
	{
		; // 得到焦点在编辑区，列式或父窗口
	}
    else
	{
		RetractPopList( hParent ); // 收回下拉框
		SendNotifiedMsg(hParent,CBN_KILLFOCUS);
	}
	return 0;
}

// **************************************************
// 声明：static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：清除编辑控件的内容，处理WM_CLEAR消息。
// 引用: 
// **************************************************
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_CLEAR,wParam,lParam); // 清除编辑区
}

// **************************************************
// 声明：static LRESULT DoGetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT * 存放当前选择的开始位置
// 	IN lParam -- SHORT * 存放当前选择的结束位置
// 返回值：LOWORD：当前选择的开始位置，HIWORD：当前选择的结束位置
// 功能描述：得到编辑区的选择范围，处理CB_GETEDITSEL消息。
// 引用: 
// **************************************************
static LRESULT DoGetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,EM_GETSEL,wParam,lParam); // 编辑区处理消息
}
// **************************************************
// 声明：static LRESULT DoLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 用户可以输入的最大字符个数，如果这个长度为0，则文本限制长度为64字节
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：设置组合框中编辑区的文本限制长度，处理CB_LIMITTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,EM_LIMITTEXT,wParam,lParam); // 编辑区处理消息
}
// **************************************************
// 声明：static LRESULT DoSetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LOWORD 要设置的当前选择的开始位置
// 				 HIWORD 要设置的当前选择的结束位置
// 
// 返回值：无
// 功能描述：设置编辑控件的选择，处理CB_SETEDITSEL消息。
// 引用: 
// **************************************************
static LRESULT DoSetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   wParam=LOWORD(lParam); //nStart
   lParam=HIWORD(lParam); //nEnd
   return SendMessage(hEditWnd,EM_SETSEL,wParam,lParam); // 编辑区处理消息
}
// **************************************************
// 声明：static LRESULT DoGetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- INT 缓存的大小
// 	IN lParam -- LPTSTR 存放文本的缓存
// 返回值：无
// 功能描述：得到控件文本，处理WM_GETTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoGetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_GETTEXT,wParam,lParam); // 编辑区处理消息
}
// **************************************************
// 声明：static LRESULT DoSetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTSTR 要设置的文本
// 返回值：成功返回TRUE，否则返回FALSE。
// 功能描述：设置编辑区文本，处理WM_SETTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoSetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_SETTEXT,wParam,lParam); // 编辑区处理消息
}
// **************************************************
// 声明：static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：返回编辑区的文本长度。
// 功能描述：得到编辑区的文本长度，处理WM_GETTEXTLENGTH消息。
// 引用: 
// **************************************************
static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_GETTEXTLENGTH,wParam,lParam); // 编辑区处理消息
}

// **************************************************
// 声明：static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：窗口位置发生改变，处理WM_WINDOWPOSCHANGED消息。
// 引用: 
// **************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	RECT rect;//,rectWindow;
	DWORD dwStyle;
//    int iTitleHeight = GetSystemMetrics(SM_CYCAPTION) - 2;//LN:2003-04-21,增加
	int iTitleHeight = GetSystemMetrics(SM_CYSYSFONT)  + 2;//LN:2003-04-21,增加
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL);//LN:2003-04-21,增加


		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return 0;
		if (lpComboBoxStruct->bChangedSize==TRUE)  // 已经改变尺寸
			return 0;
		if ((lpwp->flags&SWP_NOSIZE)==0)
		{// the window size to be changed
			// change the title size
			GetClientRect(hWnd,&rect);

			lpComboBoxStruct->rect=rect;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			switch(dwStyle&CBS_TYPEMASK)
			{
			  case CBS_DROPDOWN: // 调整编辑区的大小
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
			  case CBS_SIMPLE: // 调整编辑区的大小
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)rect.right,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,修改
						  SWP_NOZORDER|SWP_NOACTIVATE);
				break;
			  case CBS_DROPDOWNLIST: // 调整编辑区的大小
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
			// 设置窗口大小
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
// **************************************************
// 声明：static LRESULT DoAddString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPCTSTR 要添加的字符串
// 返回值：成功返回条目索引，否则返回CB_ERR
// 功能描述：添加一个字符串到控件中，处理CB_ADDSTRING消息。
// 引用: 
// **************************************************
static LRESULT DoAddString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
//		HWND hListWnd;
//		hListWnd=GetDlgItem(hWnd,ID_LIST);
//		RETAILMSG(1,(TEXT("Add Address <%s>!!!\r\n"),(LPTSTR)lParam));
		return SendMessage(lpComboBoxStruct->hListWnd,LB_ADDSTRING,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoDeleteString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 要删除的字符串的索引
// 	IN lParam -- 保留
// 返回值：成功返回列示中剩余的条目数，否则返回CB_ERR
// 功能描述：从控件中删除一个字符串，处理CB_DELETESTRING消息。
// 引用: 
// **************************************************
static LRESULT DoDeleteString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_DELETESTRING,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoDir(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_DIR消息。
// 引用: 
// **************************************************
static LRESULT DoDir(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_DIR,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功，返回列示中的条目数目,如果发生错误，返回CB_ERR
// 功能描述：得到列示的条目数目，处理CB_GETCOUNT消息。
// 引用: 
// **************************************************
static LRESULT DoGetCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETCOUNT,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功，返回列示中的当前的选择条目的索引，否则，返回CB_ERR。
// 功能描述：得到列示的当前的选择条目的索引，处理CB_GETCURSEL消息。
// 引用: 
// **************************************************
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETCURSEL,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetLBText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 要得到的字串的索引值
// 	IN lParam -- LPCTSTR 存放文本的缓存，必须要有足够的空间，包括结束符，要得到字串长度可以调用CB_GETLBTEXTLEN消息
// 返回值：成功，返回字串的长度，否则，返回CB_ERR。
// 功能描述：得到指定索引的条目的文本，处理CB_GETLBTEXT消息。
// 引用: 
// **************************************************
static LRESULT DoGetLBText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	    return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTEXT,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetLBTextLen(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT要得到的字串的索引值
// 	IN lParam -- 保留
// 返回值：成功，返回要得到的字串的长度，不包括结束符，否则，返回CB_ERR。
// 功能描述：得到指定索引的条目的文本长度，处理CB_GETLBTEXTLEN消息。
// 引用: 
// **************************************************
static LRESULT DoGetLBTextLen(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);// 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTEXTLEN,wParam,lParam);  // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoInsertString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 指定的条目索引
// 	IN lParam -- LPCTSTR 要插入的字串
// 返回值：如果成功，返回插入的字符串条目的索引值，否则，返回CB_ERR，如果是不够空间保存新添加的字符串，则返回CB_ERRSPACE。
// 功能描述：插入一个字符串到控件，处理CB_INSERTSTRING消息。
// 引用: 
// **************************************************
static LRESULT DoInsertString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	    return SendMessage(lpComboBoxStruct->hListWnd,LB_INSERTSTRING,wParam,lParam); // 处理消息
}
// **************************************************
// 声明：static LRESULT DoResetContent(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：重设控件内容，处理CB_RESETCONTENT消息。
// 引用: 
// **************************************************
static LRESULT DoResetContent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hEditWnd;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	 SendMessage(lpComboBoxStruct->hListWnd,LB_RESETCONTENT,wParam,lParam); // 列式处理消息
	 hEditWnd=GetDlgItem(hWnd,ID_EDIT);
	 SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)""); // 设置文本
	 return 0;
}
// **************************************************
// 声明：static LRESULT DoFindString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT开始查找的指定的条目索引
// 	IN lParam -- LPCSTR 要查找的字串，不区分大小写
// 返回值：成功，返回匹配的条目索引，否则，返回CB_ERR。
// 功能描述：在组合框中的列示中从指定的条目开始查找指定的字串，处理CB_FINDSTRING消息。
// 引用: 
// **************************************************
static LRESULT DoFindString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
		return SendMessage(lpComboBoxStruct->hListWnd,LB_FINDSTRING,wParam,lParam);  // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoSelectString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT开始查找的指定的条目索引
// 	IN lParam -- LPCSTR 要查找的字串，不区分大小写
// 返回值：成功，返回列示中的选择条目的索引，否则，返回CB_ERR。
// 功能描述：在组合框中的列示中查找并设定该条目为选择条目，处理CB_SELECTSTRING消息。
// 引用: 
// **************************************************
static LRESULT DoSelectString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	short iIndex;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 iIndex=(short)SendMessage(lpComboBoxStruct->hListWnd,LB_SELECTSTRING,wParam,lParam); // 列式处理消息
	 if (iIndex==LB_ERR)
		 return CB_ERR;
	 SetNewEditText(hWnd); // 设置新的编辑文本
     return iIndex;
}
// **************************************************
// 声明：static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 要设置成为当前选择的条目索引，如果设置为-1，则取消所有的选择
// 	IN lParam -- 保留
// 返回值：成功，返回当前选择的条目索引，否则，返回CB_ERR。
// 功能描述：设置当前的选择条目，处理CB_SETCURSEL消息。
// 引用: 
// **************************************************
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	short iIndex;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		 lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		 if (lpComboBoxStruct==NULL)
			 return FALSE;
		 iIndex=(short)SendMessage(lpComboBoxStruct->hListWnd,LB_SETCURSEL,wParam,lParam); // 列式处理消息
		 if (iIndex==LB_ERR)
			 return CB_ERR;
		 SetNewEditText(hWnd); // 设置新的编辑文本
		 return iIndex;
}
// **************************************************
// 声明：static LRESULT DoShowDropDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- BOOL 值为TRUE 时，显示下拉列示框值，为FALSE时，隐藏下拉列示框
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：显示下拉列示框 ，处理CB_SHOWDROPDOWN消息。
// 引用: 
// **************************************************
static LRESULT DoShowDropDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  BOOL bShow;
  DWORD dwStyle;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格
    if ((dwStyle&CBS_TYPEMASK)==CBS_SIMPLE)
      return TRUE;
    bShow=(BOOL)wParam;
    if (bShow)
    {
      PopupList(hWnd);  // 弹出列式
    }
    else
    {
      RetractPopList(hWnd); // 收回下拉框
    }
    return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 指定条目的索引
// 	IN lParam -- 保留
// 返回值：成功，返回与指定条目关联的32位值,否则，返回CB_ERR.
// 功能描述：得到一个由用户提供的，用来与指定的条目关联的32位值,处理CB_GETITEMDATA消息。
// 引用: 
// **************************************************
static LRESULT DoGetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETITEMDATA,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoSetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 指定条目的索引
// 	IN lParam -- DWORD 指定的32位值
// 返回值：成功，返回0，否则，返回CB_ERR。
// 功能描述：设置一个用来与指定的条目关联的32位值，处理CB_SETITEMDATA消息。
// 引用: 
// **************************************************
static LRESULT DoSetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETITEMDATA,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetDroppedControlRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- LPRECT 用来存放坐标的值
// 返回值：成功，返回非0，否则，返回0。
// 功能描述：得到下拉列示框的屏幕坐标 ，处理CB_GETDROPPEDCONTROLRECT消息。
// 引用: 
// **************************************************
static LRESULT DoGetDroppedControlRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  LPRECT lprect;


// !!! modified By Jami chen in 2004.07.13
//    lpComboBoxStruct=malloc(sizeof(COMBOBOXSTRUCT)); // 得到组合框的结构
	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
// !!! modified end
    if (lpComboBoxStruct==NULL)
      return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // 映射指针
#endif

	lprect=(LPRECT)lParam;
/*    lprect->left=lpComboBoxStruct->rect.left;
    lprect->top=lpComboBoxStruct->rect.top+TITLEHEIGHT;
    lprect->right=lpComboBoxStruct->rect.right;
    lprect->bottom=lpComboBoxStruct->rect.bottom;*/
	GetWindowRect(lpComboBoxStruct->hListWnd,lprect); // 得到下拉框的矩形
    return TRUE;
}
// **************************************************
// 声明：static LRESULT DoSetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 指定条目的索引
// 	IN lParam -- SHORT要设定的条目高度
// 返回值：成功，返回0，否则，返回CB_ERR。
// 功能描述：设定指定的条目的高度 ，处理CB_SETITEMHEIGHT消息。
// 引用: 
// **************************************************
static LRESULT DoSetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd; 
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETITEMHEIGHT,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 指定条目的索引
// 	IN lParam -- 保留
// 返回值：成功，返回指定条目的高度,否则，返回CB_ERR.
// 功能描述：得到指定条目的高度，处理CB_GETITEMHEIGHT消息。
// 引用: 
// **************************************************
static LRESULT DoGetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETITEMHEIGHT,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoSetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_SETEXTENDEDUI消息。
// 引用: 
// **************************************************
static LRESULT DoSetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    return CB_ERR;
}
// **************************************************
// 声明：static LRESULT DoGetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_GETEXTENDEDUI消息。
// 引用: 
// **************************************************
static LRESULT DoGetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    return TRUE;
}
// **************************************************
// 声明：static LRESULT DoGetDroppedState(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：如果列示框是可见的，返回TRUE，否则，返回FALSE。
// 功能描述：得到下拉列示的状态，处理CB_GETDROPPEDSTATE消息。
// 引用: 
// **************************************************
static LRESULT DoGetDroppedState(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
    if (lpComboBoxStruct==NULL)
       return FALSE;
    return lpComboBoxStruct->iDropState; // 得到下拉框的状态
}
// **************************************************
// 声明：static LRESULT DoFindStringExact(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_FINDSTRINGEXACT消息。
// 引用: 
// **************************************************
static LRESULT DoFindStringExact(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_FINDSTRINGEXACT,wParam,lParam);  // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoSetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_SETLOCALE消息。
// 引用: 
// **************************************************
static LRESULT DoSetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETLOCALE,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_GETLOCALE消息。
// 引用: 
// **************************************************
static LRESULT DoGetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETLOCALE,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功，返回第一条可见条目的索引，否则，返回CB_ERR。
// 功能描述：在组合框中的列示中得到第一条可见的条目，处理CB_GETTOPINDEX消息。
// 引用: 
// **************************************************
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTOPINDEX,wParam,lParam);  // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoSetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 要设置的条目索引
// 	IN lParam -- 保留
// 返回值：成功，返回0，否则，返回CB_ERR。
// 功能描述：在组合框中的列示中设置第一条可见的条目 ，处理CB_SETTOPINDEX消息。
// 引用: 
// **************************************************
static LRESULT DoSetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETTOPINDEX,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_GETHORIZONTALEXTENT消息。
// 引用: 
// **************************************************
static LRESULT DoGetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	    return SendMessage(lpComboBoxStruct->hListWnd,LB_GETHORIZONTALEXTENT,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoSetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_SETHORIZONTALEXTENT消息。
// 引用: 
// **************************************************
static LRESULT DoSetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETHORIZONTALEXTENT,wParam,lParam); // 列式处理消息
}
// **************************************************
// 声明：static LRESULT DoGetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：成功，返回下拉列示框的宽度，否则，返回CB_ERR。
// 功能描述：得到下拉列示框的宽度 ，处理CB_GETDROPPEDWIDTH消息。
// 引用: 
// **************************************************
static LRESULT DoGetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
    if (lpComboBoxStruct==NULL)
       return 0;
    return (LRESULT)(lpComboBoxStruct->rect.right-lpComboBoxStruct->rect.left); // 列式处理消息
}

// **************************************************
// 声明：static LRESULT DoSetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- SHORT 要设定的新的宽度
// 	IN lParam -- 保留
// 返回值：成功，返回新的宽度，否则，返回CB_ERR。
// 功能描述：设定下拉列示框的宽度 ，处理CB_SETDROPPEDWIDTH消息。
// 引用: 
// **************************************************
static LRESULT DoSetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  short iWidth;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
    if (lpComboBoxStruct==NULL)
       return 0;
	iWidth=(short)wParam;
	lpComboBoxStruct->rect.right=lpComboBoxStruct->rect.left+iWidth; // 列式处理消息
    return iWidth;
}


// **************************************************
// 声明：static LRESULT DoInitStorage(HWND hWnd,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN wParam -- 保留
// 	IN lParam -- 保留
// 返回值：无
// 功能描述：处理CB_INITSTORAGE消息。
// 引用: 
// **************************************************
static LRESULT DoInitStorage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd,hEditWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
//   return SendMessage(hListWnd,LB_INITSTORAGE,wParam,lParam);
    return 0;
}
// **************************************************
// 声明：static LRESULT DoNotifySelChange(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：发送选择改变的通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotifySelChange(HWND hWnd)
{
   RetractPopList(hWnd); // 收回下拉框
   SetNewEditText(hWnd); // 设置新的编辑文本
   SendNotifiedMsg(hWnd,CBN_SELCHANGE); // 通知父窗口
   return 0;
}
// **************************************************
// 声明：static LRESULT DoNotifyDblClk(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：发送双击的通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotifyDblClk(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_DBLCLK);
   return 0;
}
// **************************************************
// 声明：static LRESULT DoNotifyEditChange(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：发送编辑文本改变的通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotifyEditChange(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
   return 0;
}
// **************************************************
// 声明：static LRESULT DoNotifyEditUpdate(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：发送编辑文本更新的通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotifyEditUpdate(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_EDITUPDATE);
   return 0;
}
// **************************************************
// 声明：static LRESULT DoNotifyErrSpace(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：发送空间不足的通知消息。
// 引用: 
// **************************************************
static LRESULT DoNotifyErrSpace(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_ERRSPACE);
   return 0;
}
/****************************************************************************************************/
// **************************************************
// 声明：static void PopupList(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：弹出列式控件。
// 引用: 
// **************************************************
static void PopupList(HWND hWnd)
{
	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	
    
	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
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

		GetDropListPosition(hWnd,lpComboBoxStruct->hListWnd,&point); // 得到下拉框的位置
		GetClientRect(lpComboBoxStruct->hListWnd,&rcClient); // 达到下拉框的大小
		GetClientRect(hWnd,&rcClient); // 得到组合框的矩形
		// 设置下拉框的位置
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
		SetFocus(lpComboBoxStruct->hListWnd); // 设置焦点
		
		lpComboBoxStruct->bRetracted=FALSE;
		// 消息处理
		while(!lpComboBoxStruct->bRetracted)
		{
			if (GetMessage( &msg, 0, 0, 0 )==TRUE)
			{
				if (msg.message==WM_CLOSE)
				{
					RetractPopList(hWnd); // 收回下拉框
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
// **************************************************
// 声明：static void RetractPopList(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：收回列式控件。
// 引用: 
// **************************************************
static void RetractPopList(HWND hWnd)
{
//  RECT rect;

	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	DWORD dwStyle;
//	HWND hListWnd;


		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		if (lpComboBoxStruct==NULL)
			 return;
// !!! add By Jami chen in 2002.04.24
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // 得到窗口风格
		if ((dwStyle&CBS_TYPEMASK)==CBS_SIMPLE)
		{// the simple type is not Retract pop list
			return;
		}
// !!! add end By Jami chen in 2002.04.24
//    GetClientRect(hWnd,&rect);
		lpComboBoxStruct->bChangedSize = TRUE;
		{
			int iBorderWidth;
			int iTitleHeight = GetSystemMetrics(SM_CYSYSFONT)  + 2;//LN:2003-04-21,增加
			// !!! Add By Jami chen in 2003.07.07
			iBorderWidth = GetBorderWidth(hWnd);
			//SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right,GetSystemMetrics(SM_CYCAPTION),SWP_NOMOVE|SWP_NOZORDER);
			SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right - lpComboBoxStruct->rect.left + 2 * iBorderWidth,iTitleHeight  + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,增加
		}
		lpComboBoxStruct->bChangedSize = FALSE;
//		hListWnd=GetDlgItem(hWnd,ID_LIST);
		ShowWindow(lpComboBoxStruct->hListWnd,SW_HIDE); // 隐藏窗口
		lpComboBoxStruct->iDropState=CB_UNDROPDOWN;
		lpComboBoxStruct->bRetracted=TRUE;
//    SetCapture(0);
		SendNotifiedMsg(hWnd,CBN_CLOSEUP); // 通知父窗口
}

// **************************************************
// 声明：static void SetNewEditText(HWND hWnd)
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：设置新的编辑文本。
// 引用: 
// **************************************************
static void SetNewEditText(HWND hWnd)
{
  HWND hEditWnd;
  short indexCurSel;
  short chTextLen;
  char *lpItemText,*lpMapPtr;

  //   hParentWnd=GetParent(hWnd);
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);

//   RetractPopList(hParentWnd);
   indexCurSel=(short)SendMessage(hWnd,CB_GETCURSEL,0,0); //得到列式当前条目
   if (indexCurSel==CB_ERR)
      return ;
   chTextLen=(short)SendMessage(hWnd,CB_GETLBTEXTLEN,indexCurSel,0); // 得到当前条目的文本长度
   if (chTextLen==-1)	
	   return;
   lpItemText=malloc(chTextLen+1); // 分配空间
   if (lpItemText==NULL)
	   return ;
#ifdef _MAPPOINTER
	lpMapPtr = MapPtrToProcess( (LPVOID)lpItemText, GetCurrentProcess() );  // 映射指针
#else
	lpMapPtr = lpItemText;
#endif
   SendMessage(hWnd,CB_GETLBTEXT,indexCurSel,(LPARAM)lpMapPtr); // 得到当前文本
   SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)lpMapPtr); // 设置编辑区到当前文本
   free(lpItemText); // 释放指针
//   SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)lpItemText);
//   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
}
// **************************************************
// 声明：static void DrawDropButtonEdge(HWND hWnd,HDC hdc)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN hdc -- 实例句柄
// 
// 返回值：无
// 功能描述：绘制下拉按钮的边框。
// 引用: 
// **************************************************
static void DrawDropButtonEdge(HWND hWnd,HDC hdc)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  RECT rect;
  DWORD dwStyle;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // // 得到组合框的结构
      if (lpComboBoxStruct==NULL)
         return;
	  dwStyle = GetWindowLong(hWnd,GWL_STYLE);
	  GetWindowRect(hWnd,&rect); // 得到窗口矩形
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
		  if (dwStyle & CBS_FLATBUTTON)  // 平面显示
		  {
			  HPEN hPen;
				hPen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_ACTIVEBORDER)); // 
				hPen = SelectObject(hdc,hPen);
				MoveToEx(hdc,rect.left,rect.top,NULL);
				LineTo(hdc,rect.left,rect.bottom);
				hPen = SelectObject(hdc,hPen);
				DeleteObject(hPen);
				rect.left ++;
		  }
		  if( lpComboBoxStruct->state & CB_SUNKENOUTER )  // highlight state
				DrawEdge( hdc, &rect, BDR_SUNKENOUTER, BF_RECT | BF_MIDDLE );
		  else  // normal state
		  {
			  if (dwStyle & CBS_FLATBUTTON)  // 平面显示
			  {
					DrawEdge( hdc, &rect, 0, BF_RECT | BF_MIDDLE );
			  }
			  else  // 突起显示
			  {
					DrawEdge( hdc, &rect, BDR_RAISEDOUTER, BF_RECT | BF_MIDDLE );
			  }
		  }
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
//		  hfSave = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL16X16) );
		  hfSave = SelectObject( hdc, GetStockObject(SYSTEM_FONT_SYMBOL) );
		  SetBkMode(hdc,TRANSPARENT);
		  DrawText( hdc, &bDownArrow, 1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		  SelectObject( hdc, hfSave );
	  }  //LN:2003-04-21,增加-结束

}
/**************************************************************************/
// **************************************************
// 声明：static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg)
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iNotifiedMsg -- 要发送的通知消息的标识
// 
// 返回值：返回通知消息的处理结果。
// 功能描述：发送一个指定的通知消息。
// 引用: 
// **************************************************
static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg)
{
  HWND hParent;
  WPARAM wParam;
  UINT idComboBox;

     hParent=GetParent(hWnd);
     idComboBox=(UINT)GetWindowLong(hWnd,GWL_ID); // 得到组合框的ID号
     wParam=MAKEWPARAM(idComboBox,iNotifiedMsg);
     return SendMessage(hParent,WM_COMMAND,wParam,(LPARAM)hWnd);  // 发送消息到父窗口
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
// **************************************************
// 声明：static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight) //LN:2003-04-21,增加
// 参数：
// 	IN hWnd -- 窗口句柄
// 	IN iTitleHeight -- 条目高度
// 	OUT lprect -- 存放得到的下拉框坐标位置
// 
// 返回值：无
// 功能描述：得到下拉框矩形的坐标位置。
// 引用: 
// **************************************************
static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight) //LN:2003-04-21,增加
{
	RECT rectWindow;
	int heightDropList;
	
		GetWindowRect(hWnd,&rectWindow);  // 得到窗口矩形
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


// **************************************************
// 声明：static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint)
// 参数：
// 	IN -- 窗口句柄
// 	IN -- 下拉框的窗口句柄
// 	OUT -- 存放得到的下拉框矩形的起始坐标位置
// 
// 返回值：无
// 功能描述：得到下拉框矩形的起始坐标位置。
// 引用: 
// **************************************************
static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint)
{
	RECT rectWindow,rectList;
	int heightDropList;
	
		GetWindowRect(hWnd,&rectWindow);  // 得到窗口矩形
		GetClientRect(hListWnd,&rectList); // 得到下拉框矩形
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


// **************************************************
// 声明：static int GetBorderWidth(HWND hWnd)
// 参数：
// 	IN  hWnd -- 窗口句柄
// 
// 返回值：返回边框的宽度
// 功能描述：得到控件的边框宽度。
// 引用: 
// **************************************************
static int GetBorderWidth(HWND hWnd)
{
	RECT rectClient;
	RECT rectWindow;
	int iBorderWidth;

		GetClientRect(hWnd,&rectClient); // 得到客户区的矩形
		GetWindowRect(hWnd,&rectWindow); // 得到窗口的矩形
		iBorderWidth = (rectWindow.right - rectWindow.left) - (rectClient.right - rectClient.left); // 得到两个边框的宽度
		iBorderWidth /= 2; // 得到一个边框的宽度

		return iBorderWidth;
//	return 1;
}


/**************************************************
声明：static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTLCOLORSTRUCT,指向要设置的颜色值
返回值：成功返回TRUE，否则返回FALSE。
功能描述：设置控件的颜色，处理WM_SETCTLCOLOR消息。
引用: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
	HWND hEditWnd;

		  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		  if (lpComboBoxStruct==NULL)
			 return FALSE;
		
	     SendMessage(lpComboBoxStruct->hListWnd,WM_SETCTLCOLOR,wParam,lParam);  // 列式处理消息

		 hEditWnd=GetDlgItem(hWnd,ID_EDIT);
	     SendMessage(hEditWnd,WM_SETCTLCOLOR,wParam,lParam); // 编辑区处理消息

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {  // 设置DISABLE文本颜色
				lpComboBoxStruct->cl_Disable = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // 设置DISABLE文本背景色
				lpComboBoxStruct->cl_DisableBk = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 设置标题文本颜色
				lpComboBoxStruct->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // 设置标题文本背景色
				lpComboBoxStruct->cl_TitleBk = lpCtlColor->cl_TitleBk ;
		 }
		return TRUE;
}
/**************************************************
声明：static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- LPCTLCOLORSTRUCT,存放得到的颜色值
返回值：无
功能描述：得到控件的颜色值，处理WM_GETCTLCOLOR消息。
引用: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;

		  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // 得到组合框的结构
		  if (lpComboBoxStruct==NULL)
			 return FALSE;

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // 得到颜色结构

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // 得到DISABLE文本颜色
				lpCtlColor->cl_Disable= lpComboBoxStruct->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // 得到DISABLE文本背景色
				lpCtlColor->cl_DisableBk = lpComboBoxStruct->cl_DisableBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // 得到标题文本颜色
				lpCtlColor->cl_Title= lpComboBoxStruct->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // 得到标题文本背景色
				lpCtlColor->cl_TitleBk = lpComboBoxStruct->cl_TitleBk ;
		 }

		return TRUE;
}

/**************************************************
声明：static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
参数：
	IN hWnd -- 窗口句柄
	IN wParam -- 保留
	IN lParam -- 保留
返回值：无
功能描述：当窗口风格改变时的处理，处理WM_STYLECHANGED消息。
引用: 
************************************************/
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  DWORD wStyleType;
//  LPSTYLESTRUCT lpStyleStruct;
  BOOL bEnable;
  DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE); // 得到窗口风格

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

		hEditWnd=GetDlgItem(hWnd,ID_EDIT); // 得到编辑区句柄
		bEnable = (dwStyle & WS_DISABLED) ? FALSE : TRUE; 
		EnableWindow(hEditWnd,bEnable); // ENABLE RO DISABLE 窗口
		return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam); // 系统处理消息
}
