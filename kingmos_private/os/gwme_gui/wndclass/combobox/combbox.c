/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������Ͽ���
�汾�ţ�1.0.0
����ʱ�ڣ�2000-01-10
���ߣ��½���
�޸ļ�¼��
    2003-07-07: 1.�� List���¸�Ϊ WS_POPUP

    2003-06-05: 1.ȥ��ĳЩ ��Ҫ��code. 
	            2. �� List��Ϊ WS_CHILD , ԭ����WS_POPUP, by LN
    2003-04-21: ��������ť�ĳߴ��Ϊ��ϵͳ�õ�, by LN
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

//#define TITLEHEIGHT 18   //LN:2003-04-21,ɾ��

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
static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight);//LN, 2003-06-05 , DEL//LN:2003-04-21,�޸�
static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint);
static int GetBorderWidth(HWND hWnd);

/*********************************************************************************************************/
//WndListProc DefaultListProc;

//static LRESULT DoListLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoListLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static LRESULT DoListMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam);
/*********************************************************************************************************/

// **************************************************
// ������ATOM RegisterComboBoxClass(HINSTANCE hInstance)
// ������
//	IN hInstance -- ʵ�����
// ����ֵ������ע����
// ����������ע����Ͽ�
// ����: 
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
// ������LRESULT CALLBACK HookProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN message -- Ҫ�������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// ����ֵ��������Ϣ������
// �����������༭����Ԥ������̺���
// ����: 
// **************************************************
LRESULT CALLBACK HookProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	HWND hParent = GetParent(hWnd);
	LPCOMBOBOXSTRUCT lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong( hParent, 0 ); // �õ���Ͽ�Ľṹ���
	DWORD dwCtrlId;

	if (lpComboBoxStruct == NULL)
		return 0;
	switch( message )
	{
	case WM_KILLFOCUS:
		{
			DoCtrlKillFocus( lpComboBoxStruct, hParent, (HWND)wParam ); // ����ʧȥ�������
		}
	}
	dwCtrlId = GetWindowLong( hWnd, GWL_ID );

	// �ص�ԭ���༭���Ĺ��̺���
	if( dwCtrlId == ID_EDIT )
	    return CallWindowProc( lpComboBoxStruct->lpEditProc, hWnd, message, wParam, lParam );
	else
		return CallWindowProc( lpComboBoxStruct->lpListProc, hWnd, message, wParam, lParam );
}


// **************************************************
// ������LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN message -- Ҫ�������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// ����ֵ��������Ϣ������
// ������������Ͽ򴰿ڴ�����Ϣ���̺���
// ����: 
// **************************************************
LRESULT CALLBACK ComboBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
  WORD wmId,wmEvent;

	switch (message)
	{
		case WM_CREATE: // ������Ͽ�
			return DoCreateComboBox(hWnd,wParam,lParam);
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam); 
      switch(wmId)
      {
         case ID_LIST:
              switch(wmEvent)
              {
                 case LBN_SELCHANGE: // ����ѡ��ı��֪ͨ��Ϣ
                      DoNotifySelChange(hWnd);
                      break;
                 case LBN_DBLCLK: // ����˫����֪ͨ��Ϣ
                      DoNotifyDblClk(hWnd);
                      break;
                 case LBN_ERRSPACE: // ���Ϳռ䲻���֪ͨ��Ϣ
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
                 case EN_CHANGE: //���ͱ༭�ı��ı��֪ͨ��Ϣ
                      DoNotifyEditChange(hWnd);
                      break;
                 case EN_UPDATE: //���ͱ༭�ı����µ�֪ͨ��Ϣ
                      DoNotifyEditUpdate(hWnd);
                      break;
                 case EN_ERRSPACE: //���Ϳռ䲻���֪ͨ��Ϣ
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
   	case WM_LBUTTONDOWN: // ����������
			return DoLButtonDown(hWnd,wParam,lParam);
   	case WM_MOUSEMOVE: // ����ƶ�
			return DoMouseMove(hWnd,wParam,lParam);
   	case WM_LBUTTONUP: // ����������
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_SETFOCUS: // ���ý���
      return DoSetFocus(hWnd,wParam,lParam);
    case WM_KILLFOCUS: // ɱ������
			return DoKillFocus( hWnd, (HWND)wParam );//,lParam);
		case WM_DESTROY:
			// close this window
			ReleaseComboBox(hWnd);
//			PostQuitMessage(0);
			break;
		case WM_CLEAR: // ���
			return DoClear(hWnd,wParam,lParam);
    case WM_GETTEXT: // �õ��ı�
      return DoGetText(hWnd,wParam,lParam);
    case WM_SETTEXT: // �����ı�
      return DoSetText(hWnd,wParam,lParam);
    case WM_GETTEXTLENGTH: // �õ��ı�����
      return DoGetTextLength(hWnd,wParam,lParam);
    case WM_WINDOWPOSCHANGED: // ����λ�øı�
      return DoWindowPosChanged(hWnd,wParam,lParam);

	case WM_SETCTLCOLOR	: // ������ɫ
		return DoSetColor(hWnd,wParam,lParam);
	case WM_GETCTLCOLOR	: // �õ���ɫ
		return DoGetColor(hWnd,wParam,lParam);
	case WM_STYLECHANGED: // ���ı�
		return DoStyleChanged(hWnd,wParam,lParam);

	case CB_GETEDITSEL: // �õ��༭ȥѡ��
      return DoGetEditSel(hWnd,wParam,lParam);

	case CB_LIMITTEXT: // ������Ͽ��б༭�����ı����Ƴ���
      return DoLimitText(hWnd,wParam,lParam);
    case CB_SETEDITSEL: // ���ñ༭�ؼ���ѡ��
      return DoSetEditSel(hWnd,wParam,lParam);
    case CB_ADDSTRING: // ���һ���ַ������ؼ���
      return DoAddString(hWnd,wParam,lParam);
    case CB_DELETESTRING: // �ӿؼ���ɾ��һ���ַ���
      return DoDeleteString(hWnd,wParam,lParam);
    case CB_DIR: // ����
      return DoDir(hWnd,wParam,lParam);
    case CB_GETCOUNT: // �õ���ʾ����Ŀ��Ŀ
      return DoGetCount(hWnd,wParam,lParam);
    case CB_GETCURSEL: // �õ���ʾ�ĵ�ǰ��ѡ����Ŀ������
      return DoGetCurSel(hWnd,wParam,lParam);
    case CB_GETLBTEXT: // �õ�ָ����������Ŀ���ı�
      return DoGetLBText(hWnd,wParam,lParam);
    case CB_GETLBTEXTLEN: // �õ�ָ����������Ŀ���ı�����
      return DoGetLBTextLen(hWnd,wParam,lParam);
    case CB_INSERTSTRING: // ����һ���ַ������ؼ�
      return DoInsertString(hWnd,wParam,lParam);
    case CB_RESETCONTENT: // ����ؼ�����
      return DoResetContent(hWnd,wParam,lParam);
    case CB_FINDSTRING: // ����Ͽ��е���ʾ�д�ָ������Ŀ��ʼ����ָ�����ִ�
      return DoFindString(hWnd,wParam,lParam);
    case CB_SELECTSTRING: // ����Ͽ��е���ʾ�в��Ҳ��趨����ĿΪѡ����Ŀ
      return DoSelectString(hWnd,wParam,lParam);
    case CB_SETCURSEL: // ���õ�ǰ��ѡ����Ŀ
      return DoSetCurSel(hWnd,wParam,lParam);
    case CB_SHOWDROPDOWN: // ��ʾ������ʾ��
      return DoShowDropDown(hWnd,wParam,lParam);
    case CB_GETITEMDATA: // �õ�һ�����û��ṩ�ģ�������ָ������Ŀ������32λֵ
      return DoGetItemData(hWnd,wParam,lParam);
    case CB_SETITEMDATA: // ����һ��������ָ������Ŀ������32λֵ
      return DoSetItemData(hWnd,wParam,lParam);
    case CB_GETDROPPEDCONTROLRECT: // �õ�������ʾ�����Ļ����
      return DoGetDroppedControlRect(hWnd,wParam,lParam);
    case CB_SETITEMHEIGHT: // �趨ָ������Ŀ�ĸ߶�
      return DoSetItemHeight(hWnd,wParam,lParam);
    case CB_GETITEMHEIGHT: // �õ�ָ����Ŀ�ĸ߶�
      return DoGetItemHeight(hWnd,wParam,lParam);
    case CB_SETEXTENDEDUI: // ����
      return DoSetExtendEdui(hWnd,wParam,lParam);
    case CB_GETEXTENDEDUI: // ����
      return DoGetExtendEdui(hWnd,wParam,lParam);
    case CB_GETDROPPEDSTATE: // �õ�������ʾ��״̬
      return DoGetDroppedState(hWnd,wParam,lParam);
    case CB_FINDSTRINGEXACT: // �����ַ���
      return DoFindStringExact(hWnd,wParam,lParam);
    case CB_SETLOCALE: // ����
      return DoSetLocale(hWnd,wParam,lParam);
    case CB_GETLOCALE: // ����
      return DoGetLocale(hWnd,wParam,lParam);
    case CB_GETTOPINDEX: // ����Ͽ��е���ʾ�еõ���һ���ɼ�����Ŀ
      return DoGetTopIndex(hWnd,wParam,lParam);
    case CB_SETTOPINDEX: // ����Ͽ��е���ʾ�����õ�һ���ɼ�����Ŀ
      return DoSetTopIndex(hWnd,wParam,lParam);
    case CB_GETHORIZONTALEXTENT: // ����
      return DoGetHorizontalExtent(hWnd,wParam,lParam);
    case CB_SETHORIZONTALEXTENT: // ����
      return DoSetHorizontalExtent(hWnd,wParam,lParam);
    case CB_GETDROPPEDWIDTH: // �õ�������ʾ��Ŀ��
      return DoGetDroppedWidth(hWnd,wParam,lParam);
    case CB_SETDROPPEDWIDTH: // �趨������ʾ��Ŀ��
      return DoSetDroppedWidth(hWnd,wParam,lParam);
    case CB_INITSTORAGE: // ����
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
// ������static LRESULT DoCreateComboBox(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����0�����򷵻�-1
// ����������������Ͽ򣬴���WM_CREATE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoCreateComboBox(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
	RECT rect,rectWindow;
	DWORD dwStyle;
	HINSTANCE hInst;
	HWND hCtrl;
    BOOL bEnable;

//	int iTitleHeight = GetSystemMetrics(SM_CYCAPTION) -2;//LN:2003-04-21,����
	int iTitleHeight = GetSystemMetrics(SM_CYSYSFONT)  + 2;//LN:2003-04-21,����
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL);//LN:2003-04-21,����

	lpComboBoxStruct=malloc(sizeof(COMBOBOXSTRUCT)); // ����ṹָ��
	if (lpComboBoxStruct==NULL)
		return -1;//FALSE;//LN:2003-04-21,�޸�
	hInst=(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	dwStyle=GetWindowLong(hWnd,GWL_STYLE);
	GetClientRect(hWnd,&rect);

    lpComboBoxStruct->rect=rect;
    lpComboBoxStruct->iDropState=CB_DROPDOWN;
	lpComboBoxStruct->bChangedSize=FALSE;
	lpComboBoxStruct->bRetracted=FALSE;
	// ��ʼ����ɫ
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

	// ��Ͽ�����
    switch(dwStyle&CBS_TYPEMASK)
    {
    case CBS_DROPDOWN: // ������ + �༭��
    	// �����༭��
        hCtrl = CreateWindow(classEDIT,
				              "",
						      WS_CHILD|WS_VISIBLE,//|WS_BORDER,
						      (short)rect.left,
							  (short)rect.top,
							  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,�޸�
							  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
						      hWnd,
						      (HMENU)ID_EDIT,
							  hInst,
						      NULL);

        lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,�޸�
        lpComboBoxStruct->rectButton.top=rect.top;
        lpComboBoxStruct->rectButton.right=rect.right;
        lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,�޸�
        lpComboBoxStruct->state=CB_RAISEDOUTER;
		lpComboBoxStruct->lpEditProc = (WNDPROC)SetWindowLong( hCtrl, GWL_WNDPROC, (LONG)HookProc ); // �õ��༭��ص�
        break;
	case CBS_SIMPLE:  // ��ʽ�� + �༭��
		// �����༭��
		CreateWindow(classEDIT,
			"",
			WS_CHILD|WS_VISIBLE,//|WS_BORDER,
			(short)rect.left,
			(short)rect.top,
			(short)rect.right,
			(short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
			hWnd,
			(HMENU)ID_EDIT,
			hInst,
			NULL);
        break;
	case CBS_DROPDOWNLIST:  // ������ + ��̬��
		// ������̬��
        hCtrl = CreateWindow( classSTATIC,
			"",
						      WS_CHILD|WS_VISIBLE,//|WS_BORDER,
							  (short)rect.left,
							  (short)rect.top,
							  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,�޸�
							  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
							  hWnd,
							  (HMENU)ID_EDIT,
							  hInst,
							  NULL);
        lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,�޸�
        lpComboBoxStruct->rectButton.top=rect.top;
        lpComboBoxStruct->rectButton.right=rect.right;
        lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,�޸�
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
	// �õ���ʽ��ķ��
//						  WS_CHILD|WS_VISIBLE|WS_BORDER|LBS_HASSTRINGS|LBS_NOTIFY,
	{
		DWORD dwListStyle;
		
		if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
			dwListStyle=WS_POPUP|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;//LN, 2003-06-05, ��WS_POPUP ��ΪWS_CHILD, ȥ��WS_VISIBLE
//			dwListStyle=WS_CHILD|WS_BORDER|LBS_NOTIFY|WS_VSCROLL;//LN, 2003-06-05, ��WS_POPUP ��ΪWS_CHILD, ȥ��WS_VISIBLE
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
		  
		// ������ʽ��
		  if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
		  {  
			  GetDropListRect(hWnd,&rectWindow,iTitleHeight);//LN:2003-04-21,�޸�
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
							(short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
							(short)rect.right,
							(short)(rect.bottom-iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
							hWnd,
							(HMENU)ID_LIST,
							hInst,
							hWnd);
		  }

		}

	    SetWindowLong(hWnd,0,(long)lpComboBoxStruct);

		if ((dwStyle&CBS_TYPEMASK)!=CBS_SIMPLE)
		{  // ������ʽ��
			int iBorderWidth;
			lpComboBoxStruct->bChangedSize=TRUE;
			//SetWindowPos(hWnd,0,0,0,rectWindow.right - rectWindow.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,����
			// !!! Add By Jami chen in 2003.07.07
			iBorderWidth = GetBorderWidth(hWnd);
			// !!! Add End By Jami chen in 2003.07.07
			SetWindowPos(hWnd,0,0,0,rect.right - rect.left + 2 * iBorderWidth,iTitleHeight + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,����
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
// ������static void ReleaseComboBox(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �����������ͷ���Ͽ򣬴���WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static void ReleaseComboBox(HWND hWnd)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    
  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);  // �õ���Ͽ�Ľṹ
    if (lpComboBoxStruct==NULL)
       return;
    // �ָ���ʽ��Ĺ��̺���
//	if( lpComboBoxStruct->hListWnd )
//        SetWindowLong( lpComboBoxStruct->hListWnd, GWL_WNDPROC, (LONG)lpComboBoxStruct->lpListProc );
	// �ָ��༭���Ĺ��̺���
	if( GetDlgItem(hWnd,ID_EDIT) )
	    SetWindowLong( GetDlgItem(hWnd,ID_EDIT), GWL_WNDPROC, (LONG)lpComboBoxStruct->lpEditProc );
    free(lpComboBoxStruct); // �ͷŻ���
    SetWindowLong(hWnd,0,0);
}
/****************************************************************************************************/
/****************************************************************************************************/
// **************************************************
// ������static LRESULT DoPaint(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// ����ֵ����
// �������������ƴ��ڣ�����WM_PAINT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
    DrawDropButtonEdge(hWnd,hdc); // �����°��߿�
		return 0;
}
// **************************************************
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
// ����ֵ����
// ��������������WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
      if (lpComboBoxStruct==NULL)
         return 0;

      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      { // ָ�����Ƿ��ڰ�ť��
        lpComboBoxStruct->state=CB_SUNKENOUTER;
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc); // ����������ť�߿�
        ReleaseDC(hWnd,hdc);
      }
      SetCapture(hWnd); // ץס���
      SetFocus(hWnd); // ���ý���
	  return 0;
}
// **************************************************
// ������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
// ����ֵ����
// ��������������WM_MOUSEMOVE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      if (GetCapture()!=hWnd)
        return 0;
      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
      if (lpComboBoxStruct==NULL)
         return 0;
      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      { // ���ڰ�ť��
        if (lpComboBoxStruct->state==CB_SUNKENOUTER)
          return 0;
        lpComboBoxStruct->state=CB_SUNKENOUTER; // �°�
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
      else
      { // �㲻�ڰ�ť��
        if (lpComboBoxStruct->state==CB_RAISEDOUTER)
          return 0;
        lpComboBoxStruct->state=CB_RAISEDOUTER; // ����
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);
      }
		return 0;
}
// **************************************************
// ������static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
// ����ֵ����
// ��������������WM_LBUTTONUP��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		POINT point;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
    HDC hdc;

      if (GetCapture()!=hWnd)
        return 0;
      SetCapture(0);
      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
      if (lpComboBoxStruct==NULL)
         return 0;

      point.x=(short)LOWORD(lParam);
      point.y=(short)HIWORD(lParam);
      if (PtInRect(&lpComboBoxStruct->rectButton,point))
      { // ���ڰ�ť��
        lpComboBoxStruct->state=CB_RAISEDOUTER; // ����
        hdc=GetDC(hWnd);
        DrawDropButtonEdge(hWnd,hdc);
        ReleaseDC(hWnd,hdc);		
		if( lpComboBoxStruct->bRetracted == TRUE )
            PopupList(hWnd); // ����������
		else
			RetractPopList(hWnd);  // �ջ�������
      }
			return 0;
}
// **************************************************
// ������static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �������������ý��㵽��ǰ���ڣ�����WM_SETFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
     hEditWnd=GetDlgItem(hWnd,ID_EDIT); // �õ��༭��
//     SendMessage(hWnd,WM_SETFOCUS,0,(LPARAM)hEditWnd);
     SetFocus(hEditWnd); // ���ý���
     SendNotifiedMsg(hWnd,CBN_SETFOCUS); // ֪ͨ��Ϣ
     return 0;
}
/****************************************************************************************************/

// **************************************************
// ������static LRESULT DoKillFocus( HWND hWnd, HWND hGetWnd )
// ������
// 	IN hWnd -- ���ھ��
// 	IN hGetWnd -- ��ý���Ĵ���
// ����ֵ����
// ��������������ʧȥ���㣬����WM_KILLFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoKillFocus( HWND hWnd, HWND hGetWnd )//WPARAM wParam,LPARAM lParam)
{
    LPCOMBOBOXSTRUCT lpComboBoxStruct;

	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);  // �õ���Ͽ�Ľṹ

	if( (HWND)hGetWnd == (HWND)GetDlgItem(hWnd,ID_EDIT) ||
		(HWND)hGetWnd == lpComboBoxStruct->hListWnd )
	{
		; // ԭ�������ڱ༭������ʽ
	}
    else
	{
		RetractPopList( hWnd ); // �ջ�������
	    SendNotifiedMsg(hWnd,CBN_KILLFOCUS); // ֪ͨ��Ϣ
	}
	return 0;
}

// **************************************************
// ������static LRESULT DoCtrlKillFocus( LPCOMBOBOXSTRUCT lpComboBoxStruct, HWND hParent, HWND hGetWnd ) 
// ������
// 	IN lpComboBoxStruct -- ��Ͽ����ݽṹ
// 	IN hParent --���ؼ������ھ��
// 	IN hGetWnd -- ��ý���Ĵ���
// ����ֵ����
// �����������ӿؼ�ʧȥ���㣬����WM_KILLFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoCtrlKillFocus( LPCOMBOBOXSTRUCT lpComboBoxStruct, HWND hParent, HWND hGetWnd ) 
{
	if( (HWND)hGetWnd == (HWND)GetDlgItem(hParent,ID_EDIT) ||
		(HWND)hGetWnd == lpComboBoxStruct->hListWnd ||
		hGetWnd == hParent )
	{
		; // �õ������ڱ༭������ʽ�򸸴���
	}
    else
	{
		RetractPopList( hParent ); // �ջ�������
		SendNotifiedMsg(hParent,CBN_KILLFOCUS);
	}
	return 0;
}

// **************************************************
// ������static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������༭�ؼ������ݣ�����WM_CLEAR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_CLEAR,wParam,lParam); // ����༭��
}

// **************************************************
// ������static LRESULT DoGetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT * ��ŵ�ǰѡ��Ŀ�ʼλ��
// 	IN lParam -- SHORT * ��ŵ�ǰѡ��Ľ���λ��
// ����ֵ��LOWORD����ǰѡ��Ŀ�ʼλ�ã�HIWORD����ǰѡ��Ľ���λ��
// �����������õ��༭����ѡ��Χ������CB_GETEDITSEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,EM_GETSEL,wParam,lParam); // �༭��������Ϣ
}
// **************************************************
// ������static LRESULT DoLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT �û��������������ַ�����������������Ϊ0�����ı����Ƴ���Ϊ64�ֽ�
// 	IN lParam -- ����
// ����ֵ����
// ����������������Ͽ��б༭�����ı����Ƴ��ȣ�����CB_LIMITTEXT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,EM_LIMITTEXT,wParam,lParam); // �༭��������Ϣ
}
// **************************************************
// ������static LRESULT DoSetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LOWORD Ҫ���õĵ�ǰѡ��Ŀ�ʼλ��
// 				 HIWORD Ҫ���õĵ�ǰѡ��Ľ���λ��
// 
// ����ֵ����
// �������������ñ༭�ؼ���ѡ�񣬴���CB_SETEDITSEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetEditSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   wParam=LOWORD(lParam); //nStart
   lParam=HIWORD(lParam); //nEnd
   return SendMessage(hEditWnd,EM_SETSEL,wParam,lParam); // �༭��������Ϣ
}
// **************************************************
// ������static LRESULT DoGetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ����Ĵ�С
// 	IN lParam -- LPTSTR ����ı��Ļ���
// ����ֵ����
// �����������õ��ؼ��ı�������WM_GETTEXT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_GETTEXT,wParam,lParam); // �༭��������Ϣ
}
// **************************************************
// ������static LRESULT DoSetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTSTR Ҫ���õ��ı�
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �������������ñ༭���ı�������WM_SETTEXT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_SETTEXT,wParam,lParam); // �༭��������Ϣ
}
// **************************************************
// ������static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ر༭�����ı����ȡ�
// �����������õ��༭�����ı����ȣ�����WM_GETTEXTLENGTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetTextLength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  HWND hEditWnd;
   hEditWnd=GetDlgItem(hWnd,ID_EDIT);
   return SendMessage(hEditWnd,WM_GETTEXTLENGTH,wParam,lParam); // �༭��������Ϣ
}

// **************************************************
// ������static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������λ�÷����ı䣬����WM_WINDOWPOSCHANGED��Ϣ��
// ����: 
// **************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	RECT rect;//,rectWindow;
	DWORD dwStyle;
//    int iTitleHeight = GetSystemMetrics(SM_CYCAPTION) - 2;//LN:2003-04-21,����
	int iTitleHeight = GetSystemMetrics(SM_CYSYSFONT)  + 2;//LN:2003-04-21,����
	int iButtonWidth = GetSystemMetrics(SM_CXVSCROLL);//LN:2003-04-21,����


		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return 0;
		if (lpComboBoxStruct->bChangedSize==TRUE)  // �Ѿ��ı�ߴ�
			return 0;
		if ((lpwp->flags&SWP_NOSIZE)==0)
		{// the window size to be changed
			// change the title size
			GetClientRect(hWnd,&rect);

			lpComboBoxStruct->rect=rect;
			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			switch(dwStyle&CBS_TYPEMASK)
			{
			  case CBS_DROPDOWN: // �����༭���Ĵ�С
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,�޸�
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
						  SWP_NOZORDER|SWP_NOACTIVATE);
				lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,�޸�
				lpComboBoxStruct->rectButton.top=rect.top;
				lpComboBoxStruct->rectButton.right=rect.right;
				lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,�޸�
				break;
			  case CBS_SIMPLE: // �����༭���Ĵ�С
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)rect.right,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
						  SWP_NOZORDER|SWP_NOACTIVATE);
				break;
			  case CBS_DROPDOWNLIST: // �����༭���Ĵ�С
				SetWindowPos( 
						  GetDlgItem(hWnd,ID_EDIT),
						  0,
						  (short)rect.left,
						  (short)rect.top,
						  (short)(rect.right-iButtonWidth),//18),//LN:2003-04-21,�޸�
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
						  SWP_NOZORDER|SWP_NOACTIVATE);
						  lpComboBoxStruct->rectButton.left=rect.right-iButtonWidth;//18;//LN:2003-04-21,�޸�
						  lpComboBoxStruct->rectButton.top=rect.top;
						  lpComboBoxStruct->rectButton.right=rect.right;
						  lpComboBoxStruct->rectButton.bottom=rect.top+iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,�޸�
						break;
			}
			lpComboBoxStruct->bChangedSize=TRUE;
			    //GetWindowRect(hWnd,&rectWindow);
//		    SetWindowPos(hWnd,0,0,0,rect.right - rect.left,iTitleHeight,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,����
			// ���ô��ڴ�С
			{
				int iBorderWidth;
				// !!! Add By Jami chen in 2003.07.07
				iBorderWidth = GetBorderWidth(hWnd);
				// !!! Add End By Jami chen in 2003.07.07
				SetWindowPos(hWnd,0,0,0,rect.right - rect.left + 2 * iBorderWidth,iTitleHeight + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,����
			}
			lpComboBoxStruct->bChangedSize=FALSE;
			//change the popup list size
		    SetWindowPos( 
						  lpComboBoxStruct->hListWnd,
						  0,
						  (short)rect.left,
						  (short)(rect.top+iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
						  (short)(rect.right-rect.left),
						  (short)(rect.bottom-rect.top-iTitleHeight),//TITLEHEIGHT),//LN:2003-04-21,�޸�
						  SWP_NOZORDER|SWP_NOACTIVATE);
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoAddString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTSTR Ҫ��ӵ��ַ���
// ����ֵ���ɹ�������Ŀ���������򷵻�CB_ERR
// �������������һ���ַ������ؼ��У�����CB_ADDSTRING��Ϣ��
// ����: 
// **************************************************
static LRESULT DoAddString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
//		HWND hListWnd;
//		hListWnd=GetDlgItem(hWnd,ID_LIST);
//		RETAILMSG(1,(TEXT("Add Address <%s>!!!\r\n"),(LPTSTR)lParam));
		return SendMessage(lpComboBoxStruct->hListWnd,LB_ADDSTRING,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoDeleteString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT Ҫɾ�����ַ���������
// 	IN lParam -- ����
// ����ֵ���ɹ�������ʾ��ʣ�����Ŀ�������򷵻�CB_ERR
// �����������ӿؼ���ɾ��һ���ַ���������CB_DELETESTRING��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDeleteString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_DELETESTRING,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoDir(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_DIR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDir(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_DIR,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ���������ʾ�е���Ŀ��Ŀ,����������󣬷���CB_ERR
// �����������õ���ʾ����Ŀ��Ŀ������CB_GETCOUNT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETCOUNT,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ���������ʾ�еĵ�ǰ��ѡ����Ŀ�����������򣬷���CB_ERR��
// �����������õ���ʾ�ĵ�ǰ��ѡ����Ŀ������������CB_GETCURSEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETCURSEL,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetLBText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT Ҫ�õ����ִ�������ֵ
// 	IN lParam -- LPCTSTR ����ı��Ļ��棬����Ҫ���㹻�Ŀռ䣬������������Ҫ�õ��ִ����ȿ��Ե���CB_GETLBTEXTLEN��Ϣ
// ����ֵ���ɹ��������ִ��ĳ��ȣ����򣬷���CB_ERR��
// �����������õ�ָ����������Ŀ���ı�������CB_GETLBTEXT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetLBText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	    return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTEXT,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetLBTextLen(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORTҪ�õ����ִ�������ֵ
// 	IN lParam -- ����
// ����ֵ���ɹ�������Ҫ�õ����ִ��ĳ��ȣ������������������򣬷���CB_ERR��
// �����������õ�ָ����������Ŀ���ı����ȣ�����CB_GETLBTEXTLEN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetLBTextLen(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0);// �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTEXTLEN,wParam,lParam);  // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoInsertString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT ָ������Ŀ����
// 	IN lParam -- LPCTSTR Ҫ������ִ�
// ����ֵ������ɹ������ز�����ַ�����Ŀ������ֵ�����򣬷���CB_ERR������ǲ����ռ䱣������ӵ��ַ������򷵻�CB_ERRSPACE��
// ��������������һ���ַ������ؼ�������CB_INSERTSTRING��Ϣ��
// ����: 
// **************************************************
static LRESULT DoInsertString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	    return SendMessage(lpComboBoxStruct->hListWnd,LB_INSERTSTRING,wParam,lParam); // ������Ϣ
}
// **************************************************
// ������static LRESULT DoResetContent(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������ؼ����ݣ�����CB_RESETCONTENT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoResetContent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hEditWnd;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	 SendMessage(lpComboBoxStruct->hListWnd,LB_RESETCONTENT,wParam,lParam); // ��ʽ������Ϣ
	 hEditWnd=GetDlgItem(hWnd,ID_EDIT);
	 SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)""); // �����ı�
	 return 0;
}
// **************************************************
// ������static LRESULT DoFindString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT��ʼ���ҵ�ָ������Ŀ����
// 	IN lParam -- LPCSTR Ҫ���ҵ��ִ��������ִ�Сд
// ����ֵ���ɹ�������ƥ�����Ŀ���������򣬷���CB_ERR��
// ��������������Ͽ��е���ʾ�д�ָ������Ŀ��ʼ����ָ�����ִ�������CB_FINDSTRING��Ϣ��
// ����: 
// **************************************************
static LRESULT DoFindString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
		return SendMessage(lpComboBoxStruct->hListWnd,LB_FINDSTRING,wParam,lParam);  // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoSelectString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT��ʼ���ҵ�ָ������Ŀ����
// 	IN lParam -- LPCSTR Ҫ���ҵ��ִ��������ִ�Сд
// ����ֵ���ɹ���������ʾ�е�ѡ����Ŀ�����������򣬷���CB_ERR��
// ��������������Ͽ��е���ʾ�в��Ҳ��趨����ĿΪѡ����Ŀ������CB_SELECTSTRING��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSelectString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	short iIndex;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 iIndex=(short)SendMessage(lpComboBoxStruct->hListWnd,LB_SELECTSTRING,wParam,lParam); // ��ʽ������Ϣ
	 if (iIndex==LB_ERR)
		 return CB_ERR;
	 SetNewEditText(hWnd); // �����µı༭�ı�
     return iIndex;
}
// **************************************************
// ������static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- Ҫ���ó�Ϊ��ǰѡ�����Ŀ�������������Ϊ-1����ȡ�����е�ѡ��
// 	IN lParam -- ����
// ����ֵ���ɹ������ص�ǰѡ�����Ŀ���������򣬷���CB_ERR��
// �������������õ�ǰ��ѡ����Ŀ������CB_SETCURSEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	short iIndex;
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		 lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		 if (lpComboBoxStruct==NULL)
			 return FALSE;
		 iIndex=(short)SendMessage(lpComboBoxStruct->hListWnd,LB_SETCURSEL,wParam,lParam); // ��ʽ������Ϣ
		 if (iIndex==LB_ERR)
			 return CB_ERR;
		 SetNewEditText(hWnd); // �����µı༭�ı�
		 return iIndex;
}
// **************************************************
// ������static LRESULT DoShowDropDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- BOOL ֵΪTRUE ʱ����ʾ������ʾ��ֵ��ΪFALSEʱ������������ʾ��
// 	IN lParam -- ����
// ����ֵ����
// ������������ʾ������ʾ�� ������CB_SHOWDROPDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoShowDropDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  BOOL bShow;
  DWORD dwStyle;
    dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
    if ((dwStyle&CBS_TYPEMASK)==CBS_SIMPLE)
      return TRUE;
    bShow=(BOOL)wParam;
    if (bShow)
    {
      PopupList(hWnd);  // ������ʽ
    }
    else
    {
      RetractPopList(hWnd); // �ջ�������
    }
    return TRUE;
}
// **************************************************
// ������static LRESULT DoGetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT ָ����Ŀ������
// 	IN lParam -- ����
// ����ֵ���ɹ���������ָ����Ŀ������32λֵ,���򣬷���CB_ERR.
// �����������õ�һ�����û��ṩ�ģ�������ָ������Ŀ������32λֵ,����CB_GETITEMDATA��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETITEMDATA,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoSetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT ָ����Ŀ������
// 	IN lParam -- DWORD ָ����32λֵ
// ����ֵ���ɹ�������0�����򣬷���CB_ERR��
// ��������������һ��������ָ������Ŀ������32λֵ������CB_SETITEMDATA��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetItemData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETITEMDATA,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetDroppedControlRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPRECT ������������ֵ
// ����ֵ���ɹ������ط�0�����򣬷���0��
// �����������õ�������ʾ�����Ļ���� ������CB_GETDROPPEDCONTROLRECT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetDroppedControlRect(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  LPRECT lprect;


// !!! modified By Jami chen in 2004.07.13
//    lpComboBoxStruct=malloc(sizeof(COMBOBOXSTRUCT)); // �õ���Ͽ�Ľṹ
	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
// !!! modified end
    if (lpComboBoxStruct==NULL)
      return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif

	lprect=(LPRECT)lParam;
/*    lprect->left=lpComboBoxStruct->rect.left;
    lprect->top=lpComboBoxStruct->rect.top+TITLEHEIGHT;
    lprect->right=lpComboBoxStruct->rect.right;
    lprect->bottom=lpComboBoxStruct->rect.bottom;*/
	GetWindowRect(lpComboBoxStruct->hListWnd,lprect); // �õ�������ľ���
    return TRUE;
}
// **************************************************
// ������static LRESULT DoSetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT ָ����Ŀ������
// 	IN lParam -- SHORTҪ�趨����Ŀ�߶�
// ����ֵ���ɹ�������0�����򣬷���CB_ERR��
// �����������趨ָ������Ŀ�ĸ߶� ������CB_SETITEMHEIGHT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd; 
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETITEMHEIGHT,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT ָ����Ŀ������
// 	IN lParam -- ����
// ����ֵ���ɹ�������ָ����Ŀ�ĸ߶�,���򣬷���CB_ERR.
// �����������õ�ָ����Ŀ�ĸ߶ȣ�����CB_GETITEMHEIGHT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetItemHeight(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETITEMHEIGHT,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoSetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_SETEXTENDEDUI��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    return CB_ERR;
}
// **************************************************
// ������static LRESULT DoGetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_GETEXTENDEDUI��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetExtendEdui(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    return TRUE;
}
// **************************************************
// ������static LRESULT DoGetDroppedState(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�������ʾ���ǿɼ��ģ�����TRUE�����򣬷���FALSE��
// �����������õ�������ʾ��״̬������CB_GETDROPPEDSTATE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetDroppedState(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
    if (lpComboBoxStruct==NULL)
       return FALSE;
    return lpComboBoxStruct->iDropState; // �õ��������״̬
}
// **************************************************
// ������static LRESULT DoFindStringExact(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_FINDSTRINGEXACT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoFindStringExact(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_FINDSTRINGEXACT,wParam,lParam);  // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoSetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_SETLOCALE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETLOCALE,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_GETLOCALE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetLocale(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETLOCALE,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ������ص�һ���ɼ���Ŀ�����������򣬷���CB_ERR��
// ��������������Ͽ��е���ʾ�еõ���һ���ɼ�����Ŀ������CB_GETTOPINDEX��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_GETTOPINDEX,wParam,lParam);  // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoSetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT Ҫ���õ���Ŀ����
// 	IN lParam -- ����
// ����ֵ���ɹ�������0�����򣬷���CB_ERR��
// ��������������Ͽ��е���ʾ�����õ�һ���ɼ�����Ŀ ������CB_SETTOPINDEX��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetTopIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETTOPINDEX,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_GETHORIZONTALEXTENT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	    return SendMessage(lpComboBoxStruct->hListWnd,LB_GETHORIZONTALEXTENT,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoSetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_SETHORIZONTALEXTENT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetHorizontalExtent(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
	LPCOMBOBOXSTRUCT lpComboBoxStruct;

		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return FALSE;
	 return SendMessage(lpComboBoxStruct->hListWnd,LB_SETHORIZONTALEXTENT,wParam,lParam); // ��ʽ������Ϣ
}
// **************************************************
// ������static LRESULT DoGetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�������������ʾ��Ŀ�ȣ����򣬷���CB_ERR��
// �����������õ�������ʾ��Ŀ�� ������CB_GETDROPPEDWIDTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
    if (lpComboBoxStruct==NULL)
       return 0;
    return (LRESULT)(lpComboBoxStruct->rect.right-lpComboBoxStruct->rect.left); // ��ʽ������Ϣ
}

// **************************************************
// ������static LRESULT DoSetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- SHORT Ҫ�趨���µĿ��
// 	IN lParam -- ����
// ����ֵ���ɹ��������µĿ�ȣ����򣬷���CB_ERR��
// �����������趨������ʾ��Ŀ�� ������CB_SETDROPPEDWIDTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetDroppedWidth(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  short iWidth;

    lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
    if (lpComboBoxStruct==NULL)
       return 0;
	iWidth=(short)wParam;
	lpComboBoxStruct->rect.right=lpComboBoxStruct->rect.left+iWidth; // ��ʽ������Ϣ
    return iWidth;
}


// **************************************************
// ������static LRESULT DoInitStorage(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ��������������CB_INITSTORAGE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoInitStorage(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  HWND hListWnd,hEditWnd;
//   hListWnd=GetDlgItem(hWnd,ID_LIST);
//   return SendMessage(hListWnd,LB_INITSTORAGE,wParam,lParam);
    return 0;
}
// **************************************************
// ������static LRESULT DoNotifySelChange(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ��������������ѡ��ı��֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotifySelChange(HWND hWnd)
{
   RetractPopList(hWnd); // �ջ�������
   SetNewEditText(hWnd); // �����µı༭�ı�
   SendNotifiedMsg(hWnd,CBN_SELCHANGE); // ֪ͨ������
   return 0;
}
// **************************************************
// ������static LRESULT DoNotifyDblClk(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ��������������˫����֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotifyDblClk(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_DBLCLK);
   return 0;
}
// **************************************************
// ������static LRESULT DoNotifyEditChange(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �������������ͱ༭�ı��ı��֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotifyEditChange(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
   return 0;
}
// **************************************************
// ������static LRESULT DoNotifyEditUpdate(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �������������ͱ༭�ı����µ�֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotifyEditUpdate(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_EDITUPDATE);
   return 0;
}
// **************************************************
// ������static LRESULT DoNotifyErrSpace(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �������������Ϳռ䲻���֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotifyErrSpace(HWND hWnd)
{
   SendNotifiedMsg(hWnd,CBN_ERRSPACE);
   return 0;
}
/****************************************************************************************************/
// **************************************************
// ������static void PopupList(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������������ʽ�ؼ���
// ����: 
// **************************************************
static void PopupList(HWND hWnd)
{
	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	
    
	lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
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

		GetDropListPosition(hWnd,lpComboBoxStruct->hListWnd,&point); // �õ��������λ��
		GetClientRect(lpComboBoxStruct->hListWnd,&rcClient); // �ﵽ������Ĵ�С
		GetClientRect(hWnd,&rcClient); // �õ���Ͽ�ľ���
		// �����������λ��
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
		SetFocus(lpComboBoxStruct->hListWnd); // ���ý���
		
		lpComboBoxStruct->bRetracted=FALSE;
		// ��Ϣ����
		while(!lpComboBoxStruct->bRetracted)
		{
			if (GetMessage( &msg, 0, 0, 0 )==TRUE)
			{
				if (msg.message==WM_CLOSE)
				{
					RetractPopList(hWnd); // �ջ�������
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
// ������static void RetractPopList(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������ջ���ʽ�ؼ���
// ����: 
// **************************************************
static void RetractPopList(HWND hWnd)
{
//  RECT rect;

	LPCOMBOBOXSTRUCT lpComboBoxStruct;
	DWORD dwStyle;
//	HWND hListWnd;


		lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		if (lpComboBoxStruct==NULL)
			 return;
// !!! add By Jami chen in 2002.04.24
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
		if ((dwStyle&CBS_TYPEMASK)==CBS_SIMPLE)
		{// the simple type is not Retract pop list
			return;
		}
// !!! add end By Jami chen in 2002.04.24
//    GetClientRect(hWnd,&rect);
		lpComboBoxStruct->bChangedSize = TRUE;
		{
			int iBorderWidth;
			int iTitleHeight = GetSystemMetrics(SM_CYSYSFONT)  + 2;//LN:2003-04-21,����
			// !!! Add By Jami chen in 2003.07.07
			iBorderWidth = GetBorderWidth(hWnd);
			//SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right,GetSystemMetrics(SM_CYCAPTION),SWP_NOMOVE|SWP_NOZORDER);
			SetWindowPos(hWnd,0,0,0,lpComboBoxStruct->rect.right - lpComboBoxStruct->rect.left + 2 * iBorderWidth,iTitleHeight  + 2 * iBorderWidth,SWP_NOMOVE|SWP_NOZORDER);//LN:2003-04-21,����
		}
		lpComboBoxStruct->bChangedSize = FALSE;
//		hListWnd=GetDlgItem(hWnd,ID_LIST);
		ShowWindow(lpComboBoxStruct->hListWnd,SW_HIDE); // ���ش���
		lpComboBoxStruct->iDropState=CB_UNDROPDOWN;
		lpComboBoxStruct->bRetracted=TRUE;
//    SetCapture(0);
		SendNotifiedMsg(hWnd,CBN_CLOSEUP); // ֪ͨ������
}

// **************************************************
// ������static void SetNewEditText(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ���������������µı༭�ı���
// ����: 
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
   indexCurSel=(short)SendMessage(hWnd,CB_GETCURSEL,0,0); //�õ���ʽ��ǰ��Ŀ
   if (indexCurSel==CB_ERR)
      return ;
   chTextLen=(short)SendMessage(hWnd,CB_GETLBTEXTLEN,indexCurSel,0); // �õ���ǰ��Ŀ���ı�����
   if (chTextLen==-1)	
	   return;
   lpItemText=malloc(chTextLen+1); // ����ռ�
   if (lpItemText==NULL)
	   return ;
#ifdef _MAPPOINTER
	lpMapPtr = MapPtrToProcess( (LPVOID)lpItemText, GetCurrentProcess() );  // ӳ��ָ��
#else
	lpMapPtr = lpItemText;
#endif
   SendMessage(hWnd,CB_GETLBTEXT,indexCurSel,(LPARAM)lpMapPtr); // �õ���ǰ�ı�
   SendMessage(hEditWnd,WM_SETTEXT,0,(LPARAM)lpMapPtr); // ���ñ༭������ǰ�ı�
   free(lpItemText); // �ͷ�ָ��
//   SendMessage(hWnd,WM_SETTEXT,0,(LPARAM)lpItemText);
//   SendNotifiedMsg(hWnd,CBN_EDITCHANGE);
}
// **************************************************
// ������static void DrawDropButtonEdge(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- ʵ�����
// 
// ����ֵ����
// ��������������������ť�ı߿�
// ����: 
// **************************************************
static void DrawDropButtonEdge(HWND hWnd,HDC hdc)
{
  LPCOMBOBOXSTRUCT lpComboBoxStruct;
  RECT rect;
  DWORD dwStyle;

      lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // // �õ���Ͽ�Ľṹ
      if (lpComboBoxStruct==NULL)
         return;
	  dwStyle = GetWindowLong(hWnd,GWL_STYLE);
	  GetWindowRect(hWnd,&rect); // �õ����ھ���
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
	  //Rectangle( hdc, rect.left-1, rect.top, rect.right, rect.bottom );//LN:2003-04-21,ɾ��
	  // rect.left-1  will use same border with edit or static

// !!! Modified End By Jami chen 2002.04.24	  
	  //InflateRect( &rect, -1, -1 );//LN:2003-04-21,ɾ��
      //SetBkMode(hdc,TRANSPARENT);//LN:2003-04-21,ɾ��
      //DrawText(hdc,"",1,&rect,DT_CENTER);//LN:2003-04-21,ɾ��

	  {  //LN:2003-04-21,����-��ʼ
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
		  if (dwStyle & CBS_FLATBUTTON)  // ƽ����ʾ
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
			  if (dwStyle & CBS_FLATBUTTON)  // ƽ����ʾ
			  {
					DrawEdge( hdc, &rect, 0, BF_RECT | BF_MIDDLE );
			  }
			  else  // ͻ����ʾ
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
	  }  //LN:2003-04-21,����-����

}
/**************************************************************************/
// **************************************************
// ������static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iNotifiedMsg -- Ҫ���͵�֪ͨ��Ϣ�ı�ʶ
// 
// ����ֵ������֪ͨ��Ϣ�Ĵ�������
// ��������������һ��ָ����֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT SendNotifiedMsg(HWND hWnd,WORD iNotifiedMsg)
{
  HWND hParent;
  WPARAM wParam;
  UINT idComboBox;

     hParent=GetParent(hWnd);
     idComboBox=(UINT)GetWindowLong(hWnd,GWL_ID); // �õ���Ͽ��ID��
     wParam=MAKEWPARAM(idComboBox,iNotifiedMsg);
     return SendMessage(hParent,WM_COMMAND,wParam,(LPARAM)hWnd);  // ������Ϣ��������
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
//static void GetDropListRect(HWND hWnd,LPRECT lprect)//LN:2003-04-21,ɾ��
// **************************************************
// ������static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight) //LN:2003-04-21,����
// ������
// 	IN hWnd -- ���ھ��
// 	IN iTitleHeight -- ��Ŀ�߶�
// 	OUT lprect -- ��ŵõ�������������λ��
// 
// ����ֵ����
// �����������õ���������ε�����λ�á�
// ����: 
// **************************************************
static void GetDropListRect(HWND hWnd,LPRECT lprect, int iTitleHeight) //LN:2003-04-21,����
{
	RECT rectWindow;
	int heightDropList;
	
		GetWindowRect(hWnd,&rectWindow);  // �õ����ھ���
		heightDropList=rectWindow.bottom-rectWindow.top-iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,�޸�
		if (rectWindow.bottom>GetSystemMetrics(SM_CYSCREEN))
		{
			// the drop list can't put the combobox under,then over this
			rectWindow.top-=heightDropList;
			rectWindow.bottom=rectWindow.top+heightDropList;
		}
		else
		{
			rectWindow.top+=iTitleHeight;//TITLEHEIGHT;//LN:2003-04-21,�޸�
			//rectWindow.bottom=rectWindow.top+heightDropList;
		}
		*lprect=rectWindow;
		return;
}


// **************************************************
// ������static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint)
// ������
// 	IN -- ���ھ��
// 	IN -- ������Ĵ��ھ��
// 	OUT -- ��ŵõ�����������ε���ʼ����λ��
// 
// ����ֵ����
// �����������õ���������ε���ʼ����λ�á�
// ����: 
// **************************************************
static void GetDropListPosition(HWND hWnd,HWND hListWnd,LPPOINT lppoint)
{
	RECT rectWindow,rectList;
	int heightDropList;
	
		GetWindowRect(hWnd,&rectWindow);  // �õ����ھ���
		GetClientRect(hListWnd,&rectList); // �õ����������
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
// ������static int GetBorderWidth(HWND hWnd)
// ������
// 	IN  hWnd -- ���ھ��
// 
// ����ֵ�����ر߿�Ŀ��
// �����������õ��ؼ��ı߿��ȡ�
// ����: 
// **************************************************
static int GetBorderWidth(HWND hWnd)
{
	RECT rectClient;
	RECT rectWindow;
	int iBorderWidth;

		GetClientRect(hWnd,&rectClient); // �õ��ͻ����ľ���
		GetWindowRect(hWnd,&rectWindow); // �õ����ڵľ���
		iBorderWidth = (rectWindow.right - rectWindow.left) - (rectClient.right - rectClient.left); // �õ������߿�Ŀ��
		iBorderWidth /= 2; // �õ�һ���߿�Ŀ��

		return iBorderWidth;
//	return 1;
}


/**************************************************
������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT,ָ��Ҫ���õ���ɫֵ
����ֵ���ɹ�����TRUE�����򷵻�FALSE��
�������������ÿؼ�����ɫ������WM_SETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;
	HWND hEditWnd;

		  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		  if (lpComboBoxStruct==NULL)
			 return FALSE;
		
	     SendMessage(lpComboBoxStruct->hListWnd,WM_SETCTLCOLOR,wParam,lParam);  // ��ʽ������Ϣ

		 hEditWnd=GetDlgItem(hWnd,ID_EDIT);
	     SendMessage(hEditWnd,WM_SETCTLCOLOR,wParam,lParam); // �༭��������Ϣ

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {  // ����DISABLE�ı���ɫ
				lpComboBoxStruct->cl_Disable = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // ����DISABLE�ı�����ɫ
				lpComboBoxStruct->cl_DisableBk = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // ���ñ����ı���ɫ
				lpComboBoxStruct->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // ���ñ����ı�����ɫ
				lpComboBoxStruct->cl_TitleBk = lpCtlColor->cl_TitleBk ;
		 }
		return TRUE;
}
/**************************************************
������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT,��ŵõ�����ɫֵ
����ֵ����
�����������õ��ؼ�����ɫֵ������WM_GETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTLCOLORSTRUCT lpCtlColor;
    LPCOMBOBOXSTRUCT lpComboBoxStruct;

		  lpComboBoxStruct=(LPCOMBOBOXSTRUCT)GetWindowLong(hWnd,0); // �õ���Ͽ�Ľṹ
		  if (lpComboBoxStruct==NULL)
			 return FALSE;

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // �õ�DISABLE�ı���ɫ
				lpCtlColor->cl_Disable= lpComboBoxStruct->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // �õ�DISABLE�ı�����ɫ
				lpCtlColor->cl_DisableBk = lpComboBoxStruct->cl_DisableBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // �õ������ı���ɫ
				lpCtlColor->cl_Title= lpComboBoxStruct->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // �õ������ı�����ɫ
				lpCtlColor->cl_TitleBk = lpComboBoxStruct->cl_TitleBk ;
		 }

		return TRUE;
}

/**************************************************
������static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
���������������ڷ��ı�ʱ�Ĵ�������WM_STYLECHANGED��Ϣ��
����: 
************************************************/
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//  DWORD wStyleType;
//  LPSTYLESTRUCT lpStyleStruct;
  BOOL bEnable;
  DWORD dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

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

		hEditWnd=GetDlgItem(hWnd,ID_EDIT); // �õ��༭�����
		bEnable = (dwStyle & WS_DISABLED) ? FALSE : TRUE; 
		EnableWindow(hEditWnd,bEnable); // ENABLE RO DISABLE ����
		return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam); // ϵͳ������Ϣ
}
