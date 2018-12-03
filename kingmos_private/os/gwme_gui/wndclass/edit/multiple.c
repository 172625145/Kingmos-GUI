/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵��������Ŀ�༭��
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-12-26
���ߣ�Jami chen �½���
�޸ļ�¼��
  2004.07.19 ������Ϣ����WM_SYSCOLORCHANGE, ��������ϵͳ��ɫ����
			 ���ú���Ҫ����װ���µ���ɫ
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
������ATOM RegisterMultiItemEditClass(HINSTANCE hInstance)
������
	IN hInstance -- ʵ�����
����ֵ������ϵͳע����
����������ע�����Ŀ�༭��
����: 
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
������LRESULT CALLBACK MultiItemEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN uMsg -- Ҫ�������Ϣ
	IN wParam -- ��Ϣ����
	IN lParam -- ��Ϣ����
����ֵ����Ϣ������
�����������������Ŀ�༭����Ϣ��
����: 
************************************************/
LRESULT CALLBACK MultiItemEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
 
	switch (message)
	{
		case WM_CREATE:  // ��������
			// Create a window use this class
		  return InitialMultiItemEdit(hWnd,wParam,lParam);
		case WM_COMMAND:  // ������Ϣ
			break;
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
//			ClearBottomBlankArea(hWnd);
  	      DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
	    case WM_SIZE:  // �ı䴰�ڴ�С
 	      return DoSize(hWnd,wParam,lParam);
		case WM_WINDOWPOSCHANGED:
			return DoWindowPosChanged(hWnd,wParam,lParam);
		case WM_SETFOCUS: // ���ý���
		  return DoSetFocus(hWnd,wParam,lParam);
		case WM_KILLFOCUS: // ɱ������
		  return DoKillFocus(hWnd,wParam,lParam);
			case WM_LBUTTONDOWN:  // ����������
		  return DoLButtonDown(hWnd,wParam,lParam);
		case WM_MOUSEMOVE: // ����ƶ�
		  return DoMouseMove(hWnd,wParam,lParam);
		case WM_LBUTTONUP: // ����������
		  return DoLButtonUp(hWnd,wParam,lParam);
		case WM_CHAR: // �ַ�����
		  return DoProcessChar(hWnd,wParam,lParam);
		case WM_KEYDOWN: // ������Ϣ
		  return DoKeyDown(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ�����
				// close this window
				ReleaseMultiItemEdit(hWnd);
				break;
		case WM_VSCROLL:  // ��ֱ����
		  DoVScrollWindow(hWnd,wParam,lParam);
		  break;
		case WM_HSCROLL:  // ˮƽ����
		  DoHScrollWindow(hWnd,wParam,lParam);
		  break;
		case WM_PRINTCLIENT: // ��ӡ�ͻ�������
		  DoPrintClient(hWnd,wParam,lParam);
		  break;

// !!! Add By Jami chen in 2004.07.19
		case WM_SYSCOLORCHANGE:
			return DoSysColorChanged(hWnd,wParam,lParam);
// !!! Add End By Jami chen in 2004.07.19

		case EM_CHARFROMPOS:  // �õ�ָ������ַ�
			return DoCharFromPos(hWnd,wParam,lParam);
		case EM_GETFIRSTVISIBLELINE:  // �õ���һ���ɼ��е�����
			return DoGetFirstVisibleLine(hWnd,wParam,lParam);
		case EM_GETLIMITTEXT: // �õ�ָ����Ŀ���ı�����
			return DoGetLimitText(hWnd,wParam,lParam);
		case EM_GETLINE: // �õ�ָ���е�����
			return DoGetLine(hWnd,wParam,lParam);
		case EM_GETLINECOUNT: // �õ��ؼ���������
			return DoGetLineCount(hWnd,wParam,lParam);
		case EM_GETMODIFY: // �����޸ı�־
			return DoGetModify(hWnd,wParam,lParam);
		case EM_GETSEL: // �õ��༭����ѡ������
			return DoGetSel(hWnd,wParam,lParam);
		case EM_GETPASSWORDCHAR: // �õ���Ŀ�������ַ�
			return DoGetPasswordChar(hWnd,wParam,lParam);
		case EM_GETTHUMB: // �õ���ֱ�������������λ��
			return DoGetThumb(hWnd,wParam,lParam);
		case EM_LINEFROMCHAR: // ����ָ����Ŀ��ָ�������ַ���������
			return DoLineFromChar(hWnd,wParam,lParam);
		case EM_LINEINDEX: // �õ�ָ����Ŀָ������������Ӧ���ַ�����
				return DoLineIndex(hWnd,wParam,lParam);
		case EM_LINELENGTH: // �õ�ָ����Ŀָ���ַ������е��ַ�����
				return DoLinelength(hWnd,wParam,lParam);
		case EM_LINESCROLL: // ��������
				return DoLineScroll(hWnd,wParam,lParam);
		case EM_POSFROMCHAR: // �õ�ָ����Ŀָ���ַ�������
			return DoPosFromChar(hWnd,wParam,lParam);
		case EM_REPLACESEL: // ��ָ�����ַ����滻ѡ���ַ�
			return DoReplaceSel(hWnd,wParam,lParam);
		case EM_SCROLL: // ��ֱ������Ļ
			return DoScroll(hWnd,wParam,lParam);
		case EM_SCROLLCARET: // ������굽�ɼ�λ��
			return DoScrollCaret(hWnd,wParam,lParam);
		case EM_SETLIMITTEXT: // ����ָ����Ŀ���ı�����
			return DoSetLimitText(hWnd,wParam,lParam);
		case EM_SETMODIFY: // ����ָ����Ŀ�µ��޸ı�־
			return DoSetModify(hWnd,wParam,lParam);
		case EM_SETPASSWORDCHAR: // ����ָ����Ŀ�µ��޸ı�־
			return DoSetPasswordChar(hWnd,wParam,lParam);
		case EM_SETREADONLY: // ����ָ����Ŀ�༭����ֻ������
			return DoSetReadOnly(hWnd,wParam,lParam);
		case EM_SETSEL:  // ����ѡ������
			return DoSetSel(hWnd,wParam,lParam);
		case EM_EMPTY: // ��ձ༭��
			return DoEmpty(hWnd,wParam,lParam);
// new message
		case EM_INSERTITEM: // ��ָ����Ŀ�²���һ����Ŀ
			return DoInsertItem(hWnd,wParam,lParam);
		case EM_SETITEM:  // ����ָ����Ŀ���ı�
			return DoSetItem(hWnd,wParam,lParam);
		case EM_DELETEITEM: // ɾ��ָ����Ŀ
			return DoDeleteItem(hWnd,wParam,lParam);
		case WM_COPY: // ����ѡ�������ַ������а�
			return DoCopy(hWnd,wParam,lParam);
		case WM_PASTE:  // ճ��
			return DoPaste(hWnd,wParam,lParam);
		case WM_CUT:  // ����
			return DoCut(hWnd,wParam,lParam);
		case WM_CLEAR:  // ���
			return DoClear(hWnd,wParam,lParam);
		case WM_SETTEXT:  // ���ü���༭���ı�
			return DoSetText(hWnd,wParam,lParam);
		case WM_GETTEXT:  // �õ�����༭�����ı�
			return DoGetText(hWnd,wParam,lParam);
	    case WM_GETTEXTLENGTH:  // �õ�����༭�����ı�����
			 return DoGetTextLength(hWnd , wParam ,lParam);

		
		case EM_GETITEMTEXT:  // �õ�ָ����Ŀ�༭�����ı�
			return DoGetItemText(hWnd,wParam,lParam);
		case EM_SETITEMTEXT:  // ����ָ����Ŀ���ı�
			return DoSetItemText(hWnd,wParam,lParam);
	    case EM_GETITEMTEXTLENGTH: // �õ�ָ����Ŀ�༭�����ı�����
			 return DoGetItemTextLength(hWnd , wParam ,lParam);
		case EM_SETITEMCALLBACK: // ����ָ����Ŀ�Ļص�����ָ��
			 return DoSetItemCallBack(hWnd,wParam,lParam);

		case EM_GETACTIVEITEM:  // �õ�������Ŀ������
			return DoGetActiveItem(hWnd,wParam,lParam);
//		case EM_SETCOLOR:
//			return DoSetColor(hWnd,wParam,lParam);
//		case EM_GETCOLOR:
//			return DoGetColor(hWnd,wParam,lParam);
		case WM_SETCTLCOLOR	: // ���ô�����ɫ
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	:  // �õ���ǰ���ڵ���ɫ
			return DoGetColor(hWnd,wParam,lParam);
		case EM_GETITEMSTYLE:  // �õ�ָ����Ŀ�ķ��
			return DoGetItemStyle(hWnd,wParam,lParam);
		case EM_SETITEMSTYLE: // �趨ָ����Ŀ�ķ��
			return DoSetItemStyle(hWnd,wParam,lParam);

    case EM_GETCARETPOS:  // �õ����λ��
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
������static LRESULT InitialPDAEdit(HWND hWnd,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN lParam -- ָ��CREATESTRUCT��ָ�룬����ΪNULL��
����ֵ���ɹ�����0�����򷵻�-1��
������������������Ŀ�༭�ؼ�
����: 
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
	  lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif

	  lpcs = (LPCREATESTRUCT) lParam; // structure with creation data
	  lpMultiItemEditProperty=(LPMULTIITEMEDITPROPERTY)lpcs->lpCreateParams; // �õ������ṹָ��

// !!! Add By Jami chen in 2003.07.31
#ifdef _MAPPOINTER
	  if (lpMultiItemEditProperty)
			lpEditItemProperty = (LPEDITITEMPROPERTY)MapPtrToProcess( (LPVOID)lpMultiItemEditProperty->lpEditItemProperty, GetCallerProcess() ); 
#else
	  if (lpMultiItemEditProperty)
			lpEditItemProperty = lpMultiItemEditProperty->lpEditItemProperty;
#endif
// !!! Add End By Jami chen in 2003.07.31

      lpMultiItemEditStruct=malloc(sizeof(MULTIITEMEDITSTRUCT));  // ����ռ�
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      dwStyle|=ES_MULTIITEMEDIT|ES_LINESEPARATE;
      SetWindowLong(hWnd,GWL_STYLE,dwStyle);
      if (lpMultiItemEditStruct==NULL)
			{ // ����ʧ��
				//SendNotificationMsg(hWnd,EN_ERRSPACE);
				return -1;
			}
      if (lpMultiItemEditProperty==NULL)
      { // û�г�ʼ����Ŀ
        lpMultiItemEditStruct->nItemNum=0;
      }
      else
      { // �趨��ʼ����Ŀ����
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
        lpMultiItemEditStruct->lpItemList=malloc((lpMultiItemEditStruct->nItemNum)*sizeof(EDITITEM)); // ������Ŀ�ڴ�
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
      lpMultiItemEditStruct->nItemStartLineList=malloc((lpMultiItemEditStruct->nItemNum+1)*sizeof(int)); // ������Ŀ��ʼλ���б�
      if (lpMultiItemEditStruct->nItemStartLineList==NULL)
      {
        free(lpMultiItemEditStruct->lpItemList);
        free(lpMultiItemEditStruct);
				//SendNotificationMsg(hWnd,EN_ERRSPACE);
        return -1;
      }
//      lpMultiItemEditStruct->nItemNum=lpMultiItemEditProperty->nItemNum;


      nTextHeigth=GetTextHeight(hWnd); // �õ��ı��߶�
      GetClientRect(hWnd,&ClientRect);
      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      { // ��ʼ����Ŀ
          lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
// !!! Modified By Jami chen in 2003.07.31
//          if (SetEditItem(hWnd,lpEditItem,&lpMultiItemEditProperty->lpEditItemProperty[i],iNextWndPos)==FALSE)
          if (SetEditItem(hWnd,lpEditItem,&lpEditItemProperty[i],iNextWndPos)==FALSE)
// !!! Modified End By Jami chen in 2003.07.31
          { // ������Ŀ����ʧ��
            for (i--;i>=0;i--)
            { // ���������Ŀ�����ڴ�ռ�
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
          iNextWndPos+=(int)lpEditItem->rect.bottom;  // ��һ����Ŀ�Ŀ�ʼλ��
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
      // ��ʼ����ʾ��ɫ
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
	  SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // ͳ�θ�����
//      AdjustCaretPosition(hWnd);
      return 0;
}
/**************************************************
������static void ReleaseMultiItemEdit(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
�����������ͷŴ�����Դ��
����: 
************************************************/
static void ReleaseMultiItemEdit(HWND hWnd)
{
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
    LPEDITITEM lpEditItem;
    int nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return;
			if (lpMultiItemEditStruct->nItemStartLineList)
				free(lpMultiItemEditStruct->nItemStartLineList); // �ͷſ�ʼ�����б�

      for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
      {  // �ͷ���Ŀ����
         lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

			   if (lpEditItem->lpPDASaveEditBuffer)
				   free(lpEditItem->lpPDASaveEditBuffer);
			   if (lpEditItem->lpPDAControlBuffer)
				   free(lpEditItem->lpPDAControlBuffer);
			   if (lpEditItem->lpFormat)
				   free(lpEditItem->lpFormat);
      }

			if (lpMultiItemEditStruct->lpItemList)
				free(lpMultiItemEditStruct->lpItemList); // �ͷ���Ŀ�б�
			free(lpMultiItemEditStruct);

	    DeletePDACaret(hWnd);  // ɾ�����
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);  // ����֪ͨ��Ϣ
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
������void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,int nChangedLine,BOOL bRedraw)
������
	IN hWnd -- ���ھ��
	IN lpEditItem --Ҫ�ı�ı༭��Ŀ�ı༭�ṹָ��
	IN nChangedLine -- �ı��С
	IN bRedraw -- �ػ��־
����ֵ����
�����������ı�һ���༭��Ŀ�Ĵ�С
����: 
************************************************/
void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,int nChangedLine,BOOL bRedraw)
{
  int iActiveItem;

      iActiveItem=GetItemSequence(hWnd,lpEditItem);
      ReSetItemPos(hWnd,iActiveItem,nChangedLine,bRedraw); // ������Ŀλ��
}
/**************************************************
������static void ReSetItemPos(HWND hWnd,int iChangedSequence,int nChangedLine,BOOL bRedraw)
������
	IN hWnd -- ���ھ��
	IN iChangedSequence -- �ı���Ŀ�Ĵ���
	IN nChangedLine -- �ı��С
	IN bRedraw -- �ػ��־
����ֵ����
������������������༭��Ŀ��λ�á�
����: 
************************************************/
static void ReSetItemPos(HWND hWnd,int iChangedSequence,int nChangedLine,BOOL bRedraw)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;
  int nTextHeight,nNewStartPos;
  int nClientLine;
  RECT rcClientRect;
  HDC hdc;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return;
      nTextHeight=GetTextHeight(hWnd);  // �õ��ı��߶�
      i=iChangedSequence+1;
      hdc=GetDC(hWnd); // �õ��豸���
      for (;i<lpMultiItemEditStruct->nItemNum;i++)
      { // ��ʾ��Ŀ
        lpMultiItemEditStruct->nItemStartLineList[i]+=nChangedLine;
        nNewStartPos=(lpMultiItemEditStruct->nItemStartLineList[i]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
        lpMultiItemEditStruct->lpItemList[i].iStartPos=nNewStartPos;
        if (bRedraw)
         DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[i],PE_SHOWCLIENT);
      }
      ReleaseDC(hWnd,hdc); // �ͷ���Ŀ
      lpMultiItemEditStruct->nItemStartLineList[i]+=nChangedLine;
      lpMultiItemEditStruct->nContextBottomPos+=nChangedLine*nTextHeight;
      if (bRedraw)
        ClearBottomBlankArea(hWnd); // ����ײ��հ�

      GetClientRect(hWnd,&rcClientRect); // �õ��ͻ�����
      nClientLine=rcClientRect.bottom/nTextHeight;
      SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
}

/**************************************************
������static void ClearBottomBlankArea(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
����������������ڵײ�û�����ݵ�����
����: 
************************************************/
static void ClearBottomBlankArea(HWND hWnd)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  RECT rcClientRect,rcClearRect;
  HDC hdc;
	int nBottomPos;
	HBRUSH hBrush;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return;
		nBottomPos=lpMultiItemEditStruct->nContextBottomPos;
		GetClientRect(hWnd,&rcClientRect);
    if (nBottomPos<rcClientRect.bottom)
    { // ��Ҫ����ײ��հ�
	    rcClearRect.left=0;
	    rcClearRect.top=nBottomPos;
	    rcClearRect.right=rcClientRect.right;
	    rcClearRect.bottom =rcClientRect.bottom;
	    // clear ClearRect
	    hdc=GetDC(hWnd);
//	    FillRect(hdc,&rcClearRect,(HBRUSH)(COLOR_WINDOWFRAME));
//	    FillRect(hdc,&rcClearRect,GetStockObject(WHITE_BRUSH));
		hBrush = GetBkBrush(hWnd,NULL);  // �õ�������ˢ
	    FillRect(hdc,&rcClearRect,hBrush);  // �������
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
������static int GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem)
������
	IN hWnd -- ���ھ��
	IN lpEditItem -- ָ���༭��Ŀ�Ľṹָ��
����ֵ������ָ���༭��Ŀ�Ĵ���
�����������õ�ָ���༭��Ŀ�Ĵ���
����: 
************************************************/
static int GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return 0;
      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {  // �ж���Ŀ�Ƿ���ָ����Ŀ
//        if (hItemWnd==lpMultiItemEditStruct->hItemWndList[i]) break;
          if (lpEditItem==&(lpMultiItemEditStruct->lpItemList[i])) break;
      }
      return i;
}
/**************************************************
������static int VScrollMultiItemEdit(HWND hWnd,int nScrollLine)
������
	IN hWnd -- ���ھ��
	IN nScrollLine -- ����������
����ֵ������ʵ�ʹ���������
������������ֱ�����ؼ���
����: 
************************************************/
static int VScrollMultiItemEdit(HWND hWnd,int nScrollLine)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i,iRealScrollLines;
  int nTextHeight,nNewStartPos;
  int nPageLine,nOldDisplayLine;
//	RECT rcRedraw;
//  LPEDITITEM lpActiveEditItem;



      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return 0;
      nPageLine=GetPageLine(hWnd); // �õ�һҳ������
      if (nScrollLine==0) return 0;
      if (nScrollLine>0)
      {  // ���¹�
        if ((lpMultiItemEditStruct->nDisplayLine+nPageLine)>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
          return 0;
      }
      if (nScrollLine<0)
      { // ���Ϲ�
        if (lpMultiItemEditStruct->nDisplayLine==0)
          return 0;
      }
			nOldDisplayLine=lpMultiItemEditStruct->nDisplayLine;  // ��ʾ��
      lpMultiItemEditStruct->nDisplayLine+=nScrollLine; // �趨�µ���ʾ��
      nPageLine=GetPageLine(hWnd);
      if ((lpMultiItemEditStruct->nDisplayLine+nPageLine)>lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
        lpMultiItemEditStruct->nDisplayLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-nPageLine;
      if (lpMultiItemEditStruct->nDisplayLine<0)
        lpMultiItemEditStruct->nDisplayLine=0;
      nTextHeight=GetTextHeight(hWnd); // �õ��ı��߶�

			iRealScrollLines=(lpMultiItemEditStruct->nDisplayLine-nOldDisplayLine);  // �õ�ʵ�ʹ�������
	//		GetClientRect(hWnd,&rcRedraw);

      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {  // �趨��Ŀ���µĿ�ʼλ��
        nNewStartPos=(lpMultiItemEditStruct->nItemStartLineList[i]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
        lpMultiItemEditStruct->lpItemList[i].iStartPos=nNewStartPos;
//        SetWindowPos(lpMultiItemEditStruct->hItemWndList[i],0,0,nNewStartPos,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);
        //        InvalidateRect(lpMultiItemEditStruct->hItemWndList[i],NULL,TRUE);
      }
//      lpMultiItemEditStruct->nContextBottomPos+=nScrollLine*nTextHeight;
      InvalidateRect(hWnd,NULL,TRUE);  // ��Ч����
      lpMultiItemEditStruct->nContextBottomPos=(lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
//      ClearBottomBlankArea(hWnd);

      SetMIEditVScrollPos(hWnd,lpMultiItemEditStruct->nDisplayLine);  // ���ù�����

//      lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
		  SetPDACaret(hWnd);  // ���ù��
			return (int)iRealScrollLines;
}
/**************************************************
������static void HScrollMultiItemEdit(HWND hWnd,int xScrollPos)
������
	IN hWnd -- ���ھ��
	IN xScrollPos -- �����ĵ���
����ֵ����
����������ˮƽ�����ؼ���
����: 
************************************************/
static void HScrollMultiItemEdit(HWND hWnd,int xScrollPos)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
 
      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return;
      lpMultiItemEditStruct->xDisplayPos+=xScrollPos;  // ��������λ��
      if (lpMultiItemEditStruct->xDisplayPos<0) lpMultiItemEditStruct->xDisplayPos=0;;
      SetMIEditHScrollPos(hWnd,(int)((lpMultiItemEditStruct->xDisplayPos)/HSCROLLWIDTH));  // ������Ļ
}

/**************************************************
������static BOOL AdjustCaretPosition(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
����������������굽�ɼ�����
����: 
************************************************/
static BOOL AdjustCaretPosition(HWND hWnd)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nScrollLine,nPageLine;
//  RECT rcClientRect;
  int nCaretLine;
  BOOL bResult=FALSE;
  LPEDITITEM lpActiveEditItem;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;
    if (lpMultiItemEditStruct->nItemNum==0) return FALSE;  // û����Ŀ
    lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ�������Ŀ
    AdjustCaretInEditItem(hWnd,lpActiveEditItem);  // ����������Ŀ�Ĺ��λ��
//    nTextHeight=GetTextHeight(hWnd);
//    GetClientRect(hWnd,&rcClientRect);
//    nPageLine=rcClientRect.bottom/nTextHeight;
    nPageLine=GetPageLine(hWnd); 
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpActiveEditItem->nCaretLine; // �õ�����λ��
    if (nCaretLine<0) nCaretLine=0;   // ��Ҫ��������ͷ
    if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
    { // ��Ҫ��������β
      nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1;
      bResult=TRUE;
    }

    if (nCaretLine<lpMultiItemEditStruct->nDisplayLine)
    { // ��Ҫ���¹�������
      nScrollLine=nCaretLine-lpMultiItemEditStruct->nDisplayLine;
//      lpMultiItemEditStruct->nDisplayLine=nCaretLine;
      VScrollMultiItemEdit(hWnd,nScrollLine); // ��������
      bResult=TRUE;
    }
    if (nCaretLine>=lpMultiItemEditStruct->nDisplayLine+nPageLine)
    { // ��Ҫ���Ϲ�������
      nScrollLine=nCaretLine-(lpMultiItemEditStruct->nDisplayLine+nPageLine)+1;
//      lpMultiItemEditStruct->nDisplayLine=nCaretLine-nPageLine+1;
      VScrollMultiItemEdit(hWnd,nScrollLine); // ��������
      bResult=TRUE;
    }
    return bResult;
}
/**************************************************
������static void SetMIEditVScrollRange(HWND hWnd,int nTotalLine,int nWindowLine)
������
	IN hWnd -- ���ھ��
	IN nTotalLine -- �ؼ�������
	IN nWindowLine -- �ؼ����ڵ�����
����ֵ����
�������������ô�ֱ�������ķ�Χ��
����: 
************************************************/
static void SetMIEditVScrollRange(HWND hWnd,int nTotalLine,int nWindowLine)
{
  int nMinPos,nMaxPos;
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
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
		  // ���Ҫɾ������������Ҫ��0 ��ʼ��ʾ
		  nScrollLine=0-lpMultiItemEditStruct->nDisplayLine;
	//      lpMultiItemEditStruct->nDisplayLine=nCaretLine;
		  VScrollMultiItemEdit(hWnd,nScrollLine); // ��������
//		  lpMultiItemEditStruct->nDisplayLine = 0;
// !!! Add End By Jami chen in 2004.08.21
			if ((dwStyle&WS_VSCROLL))
			{  // ɾ��������
				ShowScrollBar(hWnd,SB_VERT,FALSE);
				DoSize(hWnd,0,0);  // �������ڴ�С
//						SetHScrollBar(hWnd);
			}
//        EnableScrollBar(hWnd,SB_VERT,ESB_DISABLE_BOTH);
      }
      else
      {
        nMinPos=0;
        nMaxPos=nTotalLine-1;//-nWindowLine+1;
		if ((dwStyle&WS_VSCROLL)==0)
		{ // ��Ҫ��ʾ����
			ShowScrollBar(hWnd,SB_VERT,TRUE);
			DoSize(hWnd,0,0); // �������ڴ�С
	        Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE); // ���ù�����Χ
			SetMIEditScrollPage(hWnd);  // ���ù���ҳ
		}
		else
			Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
        EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH);  // ������ENABLE
      }
}
/**************************************************
������static void SetMIEditScrollPage(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ����
��������������ˮƽ�������ķ�Χ��
����: 
************************************************/
static void SetMIEditScrollPage(HWND hWnd)
{
   int nPageLine;
//   int nTextHeight;
   //RECT rcClientRect;
   SCROLLINFO ScrollInfo;
   DWORD dwStyle;
      
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��

      nPageLine=GetPageLine(hWnd);  // �õ�ҳ������

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
������static void SetMIEditVScrollPos(HWND hWnd,int nNewScrollPos)
������
	IN hWnd -- ���ھ��
	IN nNewScrollPos -- �µĹ�������λ��
����ֵ����
�������������ô�ֱ��������λ�á�
����: 
************************************************/
static void SetMIEditVScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE);
}
/**************************************************
������static void SetMIEditHScrollPos(HWND hWnd,int nNewScrollPos)
������
	IN hWnd -- ���ھ��
	IN nNewScrollPos -- �µĹ�������λ��
����ֵ����
��������������ˮƽ��������λ�á�
����: 
************************************************/
static void SetMIEditHScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
      if (!(dwStyle&WS_HSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE);
}
/**************************************************
������static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- LOWORD����������
	IN lParam -- ����
����ֵ���ɹ�����ʵ�ʵĹ������������򷵻�0
������������ֱ������Ļ������WM_VSCROLL��Ϣ��
����: 
************************************************/
static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int nScrollLine,nWindowLine;
    RECT rcClientRect;
    int nTextHeight,yPos;
        
      GetClientRect(hWnd,&rcClientRect);  // �õ��ͻ�����
      nTextHeight=GetTextHeight(hWnd);  // �õ��ı��߶�
      nWindowLine=rcClientRect.bottom/nTextHeight;
      yPos=Edit_GetScrollPos(hWnd,SB_VERT); // �õ�����λ��
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP: // �Ϸ�ҳ
          nScrollLine=0-nWindowLine;  
          break;
        case SB_PAGEDOWN:  // �·�ҳ
          nScrollLine=nWindowLine;
          break;
        case SB_LINEUP:  // �Ϲ���
          nScrollLine=-1;
          break;
        case SB_LINEDOWN:  // �¹���
          nScrollLine=1;
          break;
        case SB_THUMBTRACK:  // �����ƶ�
          nScrollLine=HIWORD(wParam)-yPos;
          break;
        default:
          nScrollLine=0;
          return 0;
        }
        return VScrollMultiItemEdit(hWnd,nScrollLine);  // ��������
}

/**************************************************
������static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- LOWORD����������
	IN lParam -- ����
����ֵ����
����������ˮƽ�������ڣ�����WM_HSCROLL��Ϣ��
����: 
************************************************/
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int xScrollPos,xWindowWidth;
    RECT rcClientRect;
    int xPos;
        
      GetClientRect(hWnd,&rcClientRect);  // �õ��ͻ�����
      xWindowWidth=(rcClientRect.right/HSCROLLWIDTH)*HSCROLLWIDTH;
      xPos=Edit_GetScrollPos(hWnd,SB_HORZ); // �õ�����λ��
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:  // �Ϸ�ҳ
          xScrollPos=0-xWindowWidth;
          break;
        case SB_PAGEDOWN:  // �·�ҳ
          xScrollPos=xWindowWidth;
          break;
        case SB_LINEUP:  // �Ϲ���
          xScrollPos=0-HSCROLLWIDTH;
          break;
        case SB_LINEDOWN:  // �¹���
          xScrollPos=HSCROLLWIDTH;
          break;
        case SB_THUMBTRACK:  // �ƶ�����
          xScrollPos=(HIWORD(wParam)-xPos)*HSCROLLWIDTH;
//          Print(100,10,xScrollPos);
          break;
        default:
          return;

        }
 
      HScrollMultiItemEdit(hWnd,xScrollPos); // ˮƽ��������
}

/**************************************************
������static int GetPageLine(HWND hWnd)
������
	IN hWnd -- ���ھ��
����ֵ������һҳ������
�����������õ�һҳ������
����: 
************************************************/
// delete By Jami chen in 2004.06.12
/*static int GetPageLine(HWND hWnd)
{
  int nTextHeight,nPageLine;
  RECT rcClientRect;
    nTextHeight=GetTextHeight(hWnd);  // �õ��ı��߶�
    GetClientRect(hWnd,&rcClientRect); // �õ����ڿͻ�����
    nPageLine=rcClientRect.bottom/nTextHeight; // �õ�ҳ����
	if (nPageLine==0)
		nPageLine=1; // ���ٿ���ʾһ��
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
������void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem)
������
	IN hWnd -- ���ھ��
	IN lpEditItem -- ��ǰ�ı༭��Ŀ�ṹָ��
����ֵ����
�������������ù�굽��һ����Ŀ��
����: 
************************************************/
void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpNewEditItem;
  int iActiveItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return;
    iActiveItem=GetItemSequence(hWnd,lpEditItem);  // �õ���Ŀ����
    iActiveItem++; // �õ���һ����Ŀ����
    if (iActiveItem>=lpMultiItemEditStruct->nItemNum)
      return;
    ClearInvert(hWnd,lpEditItem,TRUE);
    lpMultiItemEditStruct->iActiveItem=iActiveItem;  // ������Ŀ
    lpNewEditItem=&lpMultiItemEditStruct->lpItemList[iActiveItem];
		SetPDACaretPosition(hWnd,lpNewEditItem,0,0);  // ���ù�굽��Ŀ��ʼ
    if (AdjustCaretPosition(hWnd)==TRUE);  // �������λ��
//		{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
//				InvalidateRect(hWnd,NULL,TRUE);
//		}

		// set caret position
		SetPDACaret(hWnd);  // ���ù��

    // show caret
    ShowPDACaret(hWnd);  // ��ʾ���
	SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // ֪ͨ������
//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,0,0);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
}
/**************************************************
������static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��ָ����Ŀ
	IN lParam -- MAKELPARAM(x,y),ָ��������
����ֵ����WORD����ָ���������ڵ��ַ�����λ�ã���WORD���ظ��ַ����ڵ��С�
�����������õ�ָ������ַ�������EM_CHARFROMPOS��Ϣ��
����: 
************************************************/
static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
  int xPos,yPos;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

	  nItem=(int)wParam; // �õ���Ŀ����
	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
    xPos=LOWORD(lParam);
    yPos=HIWORD(lParam);
    return GetCharFromPos(hWnd,lpEditItem,xPos,yPos);  // �õ�ָ��λ�õ��ַ�

//	return SendMessage(lpMultiItemEditStruct->hItemWndList[nItem],EM_CHARFROMPOS,0,lParam);
}
/**************************************************
������static LRESULT DoGetFirstVisibleLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ص�һ���ɼ��е�����
�����������õ���һ���ɼ��е�����������EM_GETFIRSTVISIBLELINE��Ϣ��
����: 
************************************************/
static LRESULT DoGetFirstVisibleLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

	return lpMultiItemEditStruct->nDisplayLine;  // �õ���ʾ������
}
/**************************************************
������static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��ָ����Ŀ
	IN lParam -- ����
����ֵ������ָ����Ŀ���ı�����
�����������õ�ָ����Ŀ���ı����ƣ�����EM_GETLIMITTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

	  nItem=(int)wParam; // �õ���Ŀ����
	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
	  return lpEditItem->cbEditLimitLen; // ������Ŀ�ı�����
}
/**************************************************
������static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ���е�����
	IN lParam -- LPSTR ���ָ�������ݵĻ��棬ǰ2���ֽڴ�Ż���Ĵ�С
����ֵ�����ؿ����ַ��ĸ���
�����������õ�ָ���е����ݣ�����EM_GETLINE��Ϣ��
����: 
************************************************/
static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nLine,nLineLen,nBufferMaxLen;
  LPSTR lpch;
  LPSTR lpLineAddress;
  int nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return 0;

	  if (lpMultiItemEditStruct->nItemNum==0) return 0;  // û����Ŀ

	  nLine=(int)wParam; // �õ�������
#ifdef _MAPPOINTER
	  lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
	  lpch=(LPSTR)lParam; // �õ�����
	  nItem=lpMultiItemEditStruct->nItemNum-1;
	  for (;nItem>=0;nItem--)
	  {
		  if (lpMultiItemEditStruct->nItemStartLineList[nItem]<=nLine)
			  break;
	  }
	  nLine-=lpMultiItemEditStruct->nItemStartLineList[nItem];
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ�ָ���е���Ŀ�ṹ

		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);  // �õ��е�ַ
		nLineLen=GetLineLength(hWnd,lpEditItem,(LPCTSTR)lpLineAddress,NULL); // �õ����ַ�
		nBufferMaxLen=MAKEWORD(*lpch,*(lpch+1));
		if (nLineLen>=nBufferMaxLen)
			return 0;
		memcpy((void *)lpch,(const void *)lpLineAddress,nLineLen);  // �������ݵ�����
		lpch[nLineLen]=0;
		return nLineLen;
}
/**************************************************
������static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ���ɹ����ؿؼ��������������򷵻�0
�����������õ��ؼ���������������EM_GETLINECOUNT��Ϣ��
����: 
************************************************/
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
		if (lpMultiItemEditStruct->nItemNum==0) return 0;

		return lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]; // �õ�������
}
/**************************************************
������static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ���༭��Ŀ
	IN lParam -- ����
����ֵ�����ָ����Ŀ��-1���򷵻ؿؼ����޸ı�־�����򷵻�ָ����Ŀ���޸ı�־��
���������������޸ı�־������EM_GETMODIFY��Ϣ��
����: 
************************************************/
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nItem;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;
	if (lpMultiItemEditStruct->nItemNum==0) return FALSE; // û������
	
	nItem=(int)wParam;  // �õ���Ŀ����

	if (nItem==-1)
	{  // �õ��༭���ı༭��־����������Ŀ�ı�־�ۺ�
		for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum-1;nItem++)
		{
			lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
			if (lpEditItem->fModified)
			{  // ����Ŀ�༭������ǰ�༭��Ϊ�Ѿ��༭��
						return TRUE;
			}
		}
	}
	else
	{  // �õ�ָ����Ŀ�ı༭��־
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
������static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ�Ĵ���
	IN lParam -- ����
����ֵ������ָ����Ŀ�������ַ�
�����������õ���Ŀ�������ַ�������EM_GETPASSWORDCHAR��Ϣ��
����: 
************************************************/
static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;  // �õ���Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ���Ŀ�ṹ
		if (lpEditItem->dwStyle&ES_PASSWORD)
				return lpEditItem->chPassWordWord;  // ���������ַ�
		else
				return 0;
}
/**************************************************
������static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT Ҫ�趨���ı�����
	IN lParam -- INT ָ����Ŀ�Ĵ���
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��������������ָ����Ŀ���ı����ƣ�����EM_SETLIMITTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 int cchMax;
   int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		cchMax=(int)wParam;  // �õ������ı�
		nItem=(int)lParam; // �õ���Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ

    return SetLimitText(hWnd,lpEditItem,cchMax);  // �趨��Ŀ����
}

/**************************************************
������static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- int ���ַ�����
	IN lParam -- INT , ָ����Ŀ
����ֵ������ָ����Ŀ��ָ�������ַ�����������
��������������ָ����Ŀ��ָ�������ַ���������������EM_LINEFROMCHAR��Ϣ��
����: 
************************************************/
static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
	DWORD dwStyle;
	LPTSTR lpCurPos;
	int ich,nLine;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		ich=(int)wParam;  // �õ��ַ�����
		nItem=(int)lParam; // �õ���Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
		dwStyle=lpEditItem->dwStyle;  // �õ���Ŀ���
		if (ich==-1)
		{ // û��ָ���ַ�
				if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
				{ // û��ѡ��
						lpCurPos=lpEditItem->lpCaretPosition;
				}
				else
				{  // ѡ��ʼλ��
						lpCurPos=lpEditItem->lpInvertStart;
				}
		}
		else
		{ // ��ָ���ַ�
				if (ich>lpEditItem->cbEditLen)
					ich=lpEditItem->cbEditLen;
				lpCurPos=lpEditItem->lpPDAEditBuffer+ich;  // �õ��ַ�ָ��
		}
    GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);  // �õ�ָ���ַ�������λ��
		return (LRESULT)nLine;
}

/**************************************************
������static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��ָ���е�����
	IN lParam -- INT ��ָ����Ŀ
����ֵ������ָ����Ŀָ������������Ӧ���ַ�����
�����������õ�ָ����Ŀָ������������Ӧ���ַ�����������EM_LINEINDEX��Ϣ��
����: 
************************************************/
static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	LPTSTR lpCurPos;
	int nLine;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		nLine=(int)wParam;  // �õ�������
		nItem=(int)lParam;  // �õ���Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
		if (nLine==-1)
		{ // û��ָ��������
				nLine=lpEditItem->nCaretLine;
		}
		lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine);  // �õ��п�ʼ��ַ
		return (LRESULT)(lpCurPos-lpEditItem->lpPDAEditBuffer); // �õ��п�ʼλ������
}
/**************************************************
������static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ���ַ�������
	IN lParam -- INT ָ����Ŀ
����ֵ������ָ����Ŀָ���ַ������е��ַ�����
�����������õ�ָ����Ŀָ���ַ������е��ַ�����������EM_LINELENGTH��Ϣ��
����: 
************************************************/
static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int ich;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		ich=(int)wParam;  // �ַ�����
		nItem=(int)lParam; // ��Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
    return GetLineLengthFromPos(hWnd,lpEditItem,ich); // �õ�ָ���ַ������еĳ���
}
/**************************************************
������static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ˮƽ�����ַ���
	IN lParam -- INT ��ֱ��������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������������ڣ�����EM_LINESCROLL��Ϣ��
����: 
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
				HScrollMultiItemEdit(hWnd,cxScroll);  // ˮƽ����
		}
		if (cyScroll)
				VScrollMultiItemEdit(hWnd,cyScroll);  // ��ֱ����
		return TRUE;
}
/**************************************************
������static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ�����ַ�����
	IN lParam -- INT ָ����Ŀ
����ֵ������ָ����Ŀָ���ַ�������,MAKERESULT(x,y)
�����������õ�ָ����Ŀָ���ַ������꣬����EM_POSFROMCHAR��Ϣ��
����: 
************************************************/
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	int wCharIndex;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		wCharIndex=(int)wParam; // �ַ�����
		nItem=(int)lParam;  // ��Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
    return GetPosFromChar(hWnd,lpEditItem,wCharIndex);  // �õ��ַ�λ��
}
/**************************************************
������static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ�����ָ����ĿΪ-1����Ϊ��ǰ������Ŀ
	IN lParam -- LPTSTR ,ָ�����ַ���
����ֵ����
������������ָ�����ַ����滻ѡ���ַ���
����: 
************************************************/
static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPCTSTR lpReplace;
	int cchLen;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;  // �õ���Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

		if (nItem<-1)
			return 0;

		if (nItem==-1)
			nItem=lpMultiItemEditStruct->iActiveItem;
	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		lpReplace=(LPCTSTR)lParam;  // Ҫ�滻���ַ�
		DeleteInvert(hWnd,lpEditItem); // ɾ��ѡ���ַ�
		cchLen=strlen(lpReplace);
		InsertString(hWnd,lpEditItem,lpReplace,cchLen); // �����滻�ַ�
		SetCaretCoordinate(hWnd,lpEditItem); // �趨���λ��
		AdjustCaretPosition(hWnd); // �������λ��
//		InvalidateRect(hWnd,NULL,TRUE);
		SetPDACaret(hWnd);  // ���ù��
		return 0;
}

/**************************************************
������static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��������
	IN lParam -- ����
����ֵ������������
������������ֱ������Ļ������EM_SCROLL��Ϣ��
����: 
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
    nScrollLine=DoVScrollWindow(hWnd,wParam,lParam);  // ��������
		lResult=MAKELRESULT(nScrollLine,1);
		return lResult;
}
/**************************************************
������static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
����������������굽�ɼ�λ�ã�����EM_SCROLLCARET��Ϣ��
����: 
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
	  AdjustCaretPosition(hWnd); // �������
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
������static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- UINT��Ҫ���õ��޸ı�־
	IN lParam -- INT ��ָ����Ŀ�������ĿΪ-1 �����������е���Ŀ
����ֵ����
��������������ָ����Ŀ�µ��޸ı�־������EM_SETMODIFY��Ϣ��
����: 
************************************************/
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	UINT  fModified;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

    fModified=(UINT)wParam;  // �õ��༭��־
		nItem=(int)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<-1)
			return 0;
		if (nItem==-1)
		{ // �������е���ĿΪָ���ı༭��־
			for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
			{
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     		lpEditItem->fModified=fModified;
			}
		}
		else
		{ // ����ָ����Ŀ�ı༭��־
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		    lpEditItem->fModified=fModified;
		}
		return 0;
}
/**************************************************
������static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- TCHAR �µ������ַ�
	IN lParam -- INT ָ����Ŀ
����ֵ���ɹ�����TRUE �����򷵻�FALSE��
���������������µ������ַ�������EM_SETPASSWORDCHAR��Ϣ��
����: 
************************************************/
static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	DWORD dwStyle;
	TCHAR ch;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		ch=(TCHAR)wParam;  // �õ������ַ�
		nItem=(int)lParam;  // �õ�Ҫ�趨����Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ

		dwStyle=lpEditItem->dwStyle;  // �õ����
		if (dwStyle&ES_MULTILINE)
			return 0;
    return SetPasswordChar(hWnd,lpEditItem,ch);  // ���������ַ�
}

/**************************************************
������static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- BOOL���Ƿ�Ҫ����Ϊֻ���ı�־
	IN lParam -- INT ָ����Ŀ
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��������������ָ����Ŀ�༭����ֻ�����ԣ�����EM_SETREADONLY��Ϣ��
����: 
************************************************/
static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
	BOOL fReadOnly;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		fReadOnly=(BOOL)wParam;  // �õ�ֻ������
		nItem=(int)lParam;  
		// Maybe Add Function .nItem == -1, then set all item are Read Only
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;
		 lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
		if (fReadOnly)
		{  // ����Ϊֻ��
			lpEditItem->dwStyle|=ES_READONLY;
		}
		else
		{ // ���ֻ������
			lpEditItem->dwStyle&=~ES_READONLY;
		}
		return TRUE;
}
/**************************************************
������static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ���ɹ����ؿ����ַ����������򷵻�0
��������������ѡ�������ַ������а壬����WM_COPY��Ϣ��
����: 
************************************************/
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ�������Ŀ����Ŀ�ṹ

    return CopyToClipboard(hWnd,lpEditItem);  // ����ѡ�񵽼��а�
}
/**************************************************
������static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ��
����������ճ��������WM_PASTE��Ϣ��
����: 
************************************************/
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ�������Ŀ�ṹ

    PasteFromClipboard(hWnd,lpEditItem); // �Ӽ��а�ճ������
		AdjustCaretPosition(hWnd);  // �������
		SetPDACaret(hWnd); // ���ù��
    return 0;
}
/**************************************************
������static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
�������������У�����WM_CUT��Ϣ��
����: 
************************************************/
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		DoCopy(hWnd,0,0);  // ����
		DoClear(hWnd,0,0); // ���
		return TRUE;
}

/**************************************************
������static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
�������������������WM_CLEAR��Ϣ��
����: 
************************************************/
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ���Ŀ�ṹ

		DeleteInvert(hWnd,lpEditItem);  // ɾ��ѡ��
		SetCaretCoordinate(hWnd,lpEditItem);  // ���ù��λ��
		AdjustCaretPosition(hWnd); // �������
		SetPDACaret(hWnd); // ���ù��
		return TRUE;
}
/**************************************************
������static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- ����
����ֵ����
������������ձ༭��������EM_EMPTY��Ϣ��
����: 
************************************************/
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  nItem=(int)wParam; // �õ���Ŀ����
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<-1)
			return 0;
	  if (nItem==-1)
	  {  // û��ָ����Ŀ
		  for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
			{ // ���������Ŀ������
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  ClearEditItemStruct(lpEditItem);

				SetWindowRect(hWnd,lpEditItem,FALSE);
			}
		    lpMultiItemEditStruct->iActiveItem=0;  // ���õ�һ����ĿΪ������Ŀ
			SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // ����ͳ����Ϣ
	  }
	  else
	  {  // ���ָ����Ŀ������
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  ClearEditItemStruct(lpEditItem);
		  SetWindowRect(hWnd,lpEditItem,FALSE);
	  }
    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ�������Ŀ�Ľṹ
	SetPDACaretPosition(hWnd,lpEditItem,0,0); // ���ù��λ��
    AdjustCaretPosition(hWnd); // �������
	SetPDACaret(hWnd); // ���ù��
    SetFocus(hWnd); // ���ý���
//    ShowCaret(hWnd);
//		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
}
/**************************************************
������static LRESULT DoPaint(HWND hWnd,HDC hdc)
������
	IN hWnd -- ���ھ��
	IN hdc -- �豸���
����ֵ����
�������������ƿؼ�������WM_PAINT��Ϣ��
����: 
************************************************/
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
    for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    { // ����ָ����Ŀ
       DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[nItem],PE_SHOWCLIENT);
    }
		ClearBottomBlankArea(hWnd); // ����ײ��հ�
    return 0;
}
/*****************************************************************/

/**************************************************
������static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- HDC ָ���豸���豸���
	IN lParam -- ����
����ֵ����
�������������༭����ӡ��ָ�����豸������WM_PRINTCLIENT��Ϣ��
����: 
************************************************/
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;		
	LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	int nItem;
    int nDisplayLine,nDisplayx;//,iStartPos;

	hdc=(HDC)wParam; // �õ��豸���
    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;
	
   nDisplayLine=lpMultiItemEditStruct->nDisplayLine;  // �õ���ʾ��ʼ��
   nDisplayx=lpMultiItemEditStruct->xDisplayPos; // �õ���ʼλ��
   lpMultiItemEditStruct->nDisplayLine=0; // ���ô�ͷ��ʼ��ʾ
   lpMultiItemEditStruct->xDisplayPos=0;
	for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    { // ������Ŀ��ָ���豸���
       DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[nItem],PE_SHOWALL);
    }
   lpMultiItemEditStruct->nDisplayLine=nDisplayLine;  // �ָ���ʼ��ʾλ��
   lpMultiItemEditStruct->xDisplayPos=nDisplayx;
   return 0;
}
/*****************************************************************/

// !!! Add By Jami chen in 2004.07.19
// **************************************************
// ������static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�� ��
// ����������ϵͳ��ɫ�ı䣬����WM_SYSCOLORHANGED��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSysColorChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

	    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
		if (lpMultiItemEditStruct==NULL) return 0;
		
		//�����趨��ǰ�ı�����ɫ
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
������static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
�����������ı䴰�ڴ�С������WM_SIZE��Ϣ��
����: 
************************************************/
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  RECT rectNew;
  int i;
  int xPos,nLine;
  int nTextHeigth,nClientLine;

       lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
       if (lpMultiItemEditStruct==NULL) return 0;

       GetClientRect(hWnd,&rectNew);  // �õ��ͻ�����
       for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
       { // �����С
          lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
          lpEditItem->rect.right=rectNew.right;
          // Add by Jami 2000-06-02  -- Must Reset The Item Height
          SetWindowRect(hWnd,lpEditItem,FALSE);
//          TotalLine(
       }
       
	   lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ�������Ŀ�ṹ
       GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,lpEditItem->dwStyle&ES_MULTILINE);// �õ����λ��
	   lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;  // ���ù��
	   lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine; 
	   AdjustCaretPosition(hWnd);  // �������
// !!! Add By Jami chen in 2004.08.21
       nTextHeigth=GetTextHeight(hWnd);
       nClientLine=rectNew.bottom/nTextHeigth;
       SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
       SetMIEditVScrollPos(hWnd,lpMultiItemEditStruct->nDisplayLine);  // ���ù�����
// !!! Add End By Jami chen in 2004.08.21
	   InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
       return 0;
}

#ifdef DITHERING_DEAL  // ��������
static int old_x = -1,old_y = -1;
#endif
/**************************************************
������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
����ֵ����
��������������WM_LBUTTONDOWN��Ϣ��
����: 
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

#ifdef DITHERING_DEAL  // ��������
			old_x = xPos;
			old_y = yPos;
#endif
		  // Get struct EDITITEM data
		  lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
		  if(lpMultiItemEditStruct==NULL) return FALSE;
			if (lpMultiItemEditStruct->nItemNum==0) return 0;

		  nItem=GetItem(hWnd,xPos,yPos); // �õ�ָ��λ�õ���Ŀ����
		  if (nItem==-1)
			 return 0;
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ�������Ŀ�ṹ
		  ClearInvert(hWnd,lpEditItem,TRUE);  // ���ѡ��

		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ�ָ����Ŀ�ṹ
		  if (lpEditItem->dwStyle&WS_DISABLED)
			  return 0;
		  nOldActiveItem = lpMultiItemEditStruct->iActiveItem;
		  lpMultiItemEditStruct->iActiveItem=nItem; // ����ָ����ĿΪ������Ŀ
		  yPos-=lpEditItem->iStartPos;
			
			if (lpEditItem->dwStyle&ES_CALLBACK)
			{ // ��ǰ��Ŀ�лص�����
				if (InCallBackPos(hWnd,lpEditItem,xPos,yPos)==TRUE)
				{ // �ص�����
					if (lpEditItem->lpCallBack!=NULL)
					{
//						lpEditItem->lpCallBack(hWnd,(short)nItem);
#ifdef _MAPPOINTER
						CALLBACKDATA  CallbackData;
						
							CallbackData.hProcess = GetCallerProcess();  // ����Ӧ�ó�����̾��
							CallbackData.lpfn = (FARPROC)lpEditItem->lpCallBack; // ���ûص�����ָ��
							CallbackData.dwArg0 = (DWORD)hWnd; // ���������ṹ
							
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
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos); // ���ù�꺯��

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
������static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
����ֵ����
��������������WM_MOUSEMOVE��Ϣ��
����: 
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

#ifdef DITHERING_DEAL  // ��������
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

      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ���Ŀ�ṹ
      yPos-=lpEditItem->iStartPos;


			// set Caret position to LButton Down Position
// !!! Modified By jami chen in 2004.10.18
//			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);
			if (SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos) == FALSE)
			{
				// ���λ��û�з����ı�
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
������static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
����ֵ����
��������������WM_LBUTTONUP��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture
	SetCapture(NULL);
  return 0;
}
/**************************************************
������static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
�������������ý��㣬����WM_SETFOCUS��Ϣ��
����: 
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
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;

       lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
       if (lpMultiItemEditStruct==NULL) return 0;
       if (lpMultiItemEditStruct->nItemNum==0) return 0;
	    // get struct EDITITEM data
		lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

// !!! Modified By jami chen in 2005.01.12 
//      ClearInvert(hWnd,lpEditItem,TRUE); // ���ѡ��
// !!! to  By Jami chen in 2005.01.12
	    if (!(lpEditItem->dwStyle & ES_NOHIDESEL ))
		{
	        ClearInvert(hWnd,lpEditItem,TRUE); // ���ѡ��
		}
// !!! Modified  End
		ClearInvert(hWnd,lpEditItem,TRUE);
	    DeletePDACaret(hWnd);
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);
	    return 0;
}

/**************************************************
������static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
�����������ַ����룬����WM_CHAR��Ϣ��
����: 
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
������static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ���ֵ
	IN lParam -- ����
����ֵ����
��������������WM_KEYDOWN��Ϣ��
����: 
************************************************/
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
	int nVirtKey ;
//	DWORD lKeyData ;
  int nScrollLine,nWindowLine,iHorzPos;


     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
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
������static BOOL MoveCaret(HWND hWnd,int nScrollLine,int iHorzPos)
������
	IN hWnd -- ���ھ��
	IN nScrollLine -- Ҫ����������
	IN iHorzPos -- ˮƽ�����ı�־
����ֵ����
�����������ƶ����
����: 
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


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;


	lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ���Ŀ�ṹ

	if (nScrollLine==0)
	{
		if (iHorzPos!=MOTIONLESS)
		{ // ˮƽ�ƶ����
			HorzMoveCaret(hWnd,lpEditItem,iHorzPos);
		}
		return TRUE;
	}
	// �õ��������λ��
    xPos=lpEditItem->nCaretx;
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpEditItem->nCaretLine;  // �õ����������

    nCaretLine+=nScrollLine; // �������


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

    nItemCaretLine=nCaretLine-lpMultiItemEditStruct->nItemStartLineList[iActiveItem]; // �õ������ָ����Ŀ�е�������

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ�������Ŀ
    ClearInvert(hWnd,lpEditItem,TRUE);  // ���ѡ��

    lpMultiItemEditStruct->iActiveItem=iActiveItem; // �����µļ�����Ŀ

    nTextHeight=GetTextHeight(hWnd);
	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ�������Ŀ�ṹ
		SetPDACaretPosition(hWnd,lpEditItem,xPos,(int)(nItemCaretLine*nTextHeight)); // ���ù��
    if (AdjustCaretPosition(hWnd)==TRUE)  // �������λ��
		{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
//				InvalidateRect(hWnd,NULL,TRUE);
		}

		// set caret position
		SetPDACaret(hWnd); 

    // show caret
    ShowPDACaret(hWnd);
	SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE); // ֪ͨ������

//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,nItemCaretLine,xPos);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
    return TRUE;
}
/**************************************************
������static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ش�ֱ�������������λ��
�����������õ���ֱ�������������λ�ã�����EM_GETTHUMB��Ϣ��
����: 
************************************************/
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
		if (!(dwStyle&WS_VSCROLL)) return 0;
    wParam++;
    lParam++;
		return Edit_GetScrollPos(hWnd,SB_VERT); // �õ�����λ��
}
/**************************************************
������static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- LPDWORD �� ���ѡ���������ʼλ���ַ�����
	IN lParam -- LPDOWRD �� ���ѡ������Ľ���λ���ַ�����
����ֵ���ɹ�����MAKERESULT(START,END),���򷵻�0��
�����������õ��༭����ѡ�����򣬴���EM_GETSEL��Ϣ��
����: 
************************************************/
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 int nStart , nEnd;
	 LPDWORD lpdwStart,lpdwEnd;
	 LRESULT lResult;



    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  // Get struct EDITITEM data
	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // ӳ��ָ��
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif

		lpdwStart=(LPDWORD)wParam;  // �õ�����
		lpdwEnd=(LPDWORD)lParam;
		if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
			return -1;
		nStart=lpEditItem->lpInvertStart-lpEditItem->lpPDAEditBuffer; // �õ�ѡ��λ��
		nEnd=lpEditItem->lpInvertEnd-lpEditItem->lpPDAEditBuffer;
		if (lpdwStart)
			*lpdwStart=(DWORD)nStart;
		if (lpdwEnd)
			*lpdwEnd=(DWORD)nEnd;
		lResult=MAKELRESULT(nStart,nEnd);
		return lResult;
}

/**************************************************
������static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ��ѡ��������ʼλ��
	IN lParam -- INT ��ѡ���������λ��
����ֵ��
��������������ѡ�����򣬴���EM_SETSEL��Ϣ��
����: 
************************************************/
static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   int nStart , nEnd;



    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ���Ŀ�ṹ

		nStart=(int)wParam;
		nEnd=(int)lParam;

    return SetSel(hWnd,lpEditItem,nStart,nEnd);  // ����ѡ��
}

/**************************************************
������static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTSTR Ҫ���õ��ı�
����ֵ���ɹ�����TRUE�����򷵻�FALSE��
�������������ü���༭���ı�������WM_SETTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPCTSTR lpsz;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;

    lpsz= (LPCTSTR)lParam; // address of window-text string
    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ�������Ŀ�ṹ

    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)  // �����ı�
		return FALSE;
	AdjustCaretPosition(hWnd); // �������
	SetPDACaret(hWnd); // ���ù��
	InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
	return TRUE;
}

/**************************************************
������static LRESULT DoSetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- LPCTSTR Ҫ���õ��ı�ָ��
����ֵ��
��������������ָ����Ŀ���ı�������EM_SETITEMTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoSetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPCTSTR lpsz;
   int nItem;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return FALSE;

		nItem=(int)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
    lpsz= (LPCTSTR)lParam; // address of window-text string
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ���Ŀ�ṹ

//    return SetText(hWnd,lpEditItem,lpsz);
    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)  // �����ı�
		return FALSE;
	AdjustCaretPosition(hWnd);  // �������
	SetPDACaret(hWnd); // ���ù��
	InvalidateRect(hWnd,NULL,TRUE);
	return TRUE;
}

/**************************************************
������static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ����Ĵ�С
	IN lParam -- LPTSTR ����ı��Ļ���
����ֵ���ɹ����ؿ����ַ��ĸ��������򷵻�0
�����������õ�����༭�����ı�������WM_GETTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPTSTR lpsz;
   int cMaxlen;
//   int nItem;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
     if (lpMultiItemEditStruct==NULL) return 0;


			lpsz= (LPTSTR)lParam; // address of window-text string
			cMaxlen=(int)(wParam);

	        lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ�������Ŀ�ṹ

			// !!! modified by jami chen in 2004.05.27
//			if (cMaxlen<lpEditItem->cbEditLen)
//				return FALSE;
//		    GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
//			return lpEditItem->cbEditLen;

			if (cMaxlen>lpEditItem->cbEditLen)
				cMaxlen = lpEditItem->cbEditLen + 1; // +1 is read end code 'null'
			// �õ��༭�ı�
		    GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,cMaxlen); // +1 is read end code 'null'
			return cMaxlen;// lpEditItem->cbEditLen;  ����ʵ�ʸ������ݵĴ�С
			// !!! modified by jami chen
}
/**************************************************
������static LRESULT DoGetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- HIWORD ָ������Ĵ�С
				 LOWORD ָ����Ŀ
	IN lParam -- LPTSTR ������ݵĻ���
����ֵ���ɹ����ؿ����ַ��ĸ��������򷵻�0
�����������õ�ָ����Ŀ�༭�����ı�������EM_GETITEMTEXT��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPTSTR lpsz;
   int cMaxlen;
   int nItem;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
     if (lpMultiItemEditStruct==NULL) return 0;


#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ����
#endif
		lpsz= (LPTSTR)lParam; // address of window-text string
		cMaxlen=(int)HIWORD(wParam);
		nItem=(int)LOWORD(wParam);

		  if (nItem>=lpMultiItemEditStruct->nItemNum)
			  return 0;
		  if (nItem<0)
			  return 0;

      lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ���
			if (cMaxlen<lpEditItem->cbEditLen)
				return FALSE;
			// �õ��ı�
		  GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
			return lpEditItem->cbEditLen;
}
/**************************************************
������static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ؼ���༭�����ı����ȡ�
�����������õ�����༭�����ı����ȣ�����WM_GETTEXTLENGTH��Ϣ��
����: 
************************************************/
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return 0;

 	  // Get struct EDITITEM data
      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem]; // �õ���Ŀ�ṹ
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;  // �õ��ı�����
}
/**************************************************
������static LRESULT DoGetItemTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- ����
����ֵ������ָ����Ŀ�༭�����ı����ȡ�
�����������õ�ָ����Ŀ�༭�����ı����ȣ�����EM_GETITEMTEXTLENGTH��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   int nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
      if (lpMultiItemEditStruct==NULL) return 0;

      nItem=(int)wParam;

		  if (nItem>=lpMultiItemEditStruct->nItemNum)
			  return 0;
		  if (nItem<0)
			  return 0;

			// Get struct EDITITEM data
      lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;  // �õ��ı�����
}
/**************************************************
������static LRESULT DoSetItemCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- EDITCALLBACK * �ص�������ָ��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��������������ָ����Ŀ�Ļص�����ָ�룬����EM_SETITEMCALLBACK��Ϣ��
����: 
************************************************/
static LRESULT DoSetItemCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   int nItem;
   EDITCALLBACK *lpCallBack;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
     if (lpMultiItemEditStruct==NULL) return 0;


#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
	  lpCallBack= (EDITCALLBACK *)lParam; // address of window-text string
      nItem=(int)LOWORD(wParam);

	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
	  if (nItem<0)
		  return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];  // �õ���Ŀ�ṹ
	  lpEditItem->lpCallBack=lpCallBack;  // ���ûص�����
	  return TRUE;
}
/**************************************************
������static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- ָ��EDITITEMPROPERTY��ָ�룬��Ŀ������
����ֵ���ɹ�����0�����򷵻�-1
������������ָ����Ŀ�²���һ����Ŀ������EM_INSERTITEM��Ϣ��
����: 
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
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
     lpEditItemProperty=(LPEDITITEMPROPERTY)lParam; // �õ���Ŀ����

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
     if (lpMultiItemEditStruct==NULL) return -1;

     lpMultiItemEditStruct->nItemNum++;  // ����һ����Ŀ

     // ������Ŀ�ṹ
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

     // ������Ŀ��ʼλ���б�
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
     { // ���뵽���һ����Ŀ
        nItem=lpMultiItemEditStruct->nItemNum-1;
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
        iItemStartPos=lpMultiItemEditStruct->nContextBottomPos;
     }
     else
     {  // ���뵽ָ����Ŀλ��
         lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ�ָ����Ŀ�Ľṹ
         iItemStartPos=lpEditItem->iStartPos; // �õ���ʼλ��
         // ������Ŀ�ƶ�����һ����Ŀλ��
         pSrc=(char *)lpEditItem;
         pObj=pSrc+sizeof(EDITITEM);
         iMoveLen=(lpMultiItemEditStruct->nItemNum-nItem-1)*sizeof(EDITITEM);
         memmove(pObj,pSrc,iMoveLen);

         // ������Ŀ��ʼλ���ƶ�����һ����Ŀλ��
         pSrc=(char *)&lpMultiItemEditStruct->nItemStartLineList[nItem];
         pObj=pSrc+sizeof(int);
         iMoveLen=(lpMultiItemEditStruct->nItemNum-nItem)*sizeof(int);
         memmove(pObj,pSrc,iMoveLen);
     }

     SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos); // ���õ�ǰ��Ŀ
     lpMultiItemEditStruct->nItemStartLineList[nItem]=iItemStartPos/nTextHeigth;

     iItemStartPos+=(int)lpEditItem->rect.bottom;
     for (i=nItem+1;i<lpMultiItemEditStruct->nItemNum;i++)
     { // ���µ�����ʼλ��
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(int)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine); // ��������
//     AdjustCaretPosition(hWnd);
		 SetPDACaretPosition(hWnd,lpEditItem,0,0); // ���ù��λ��
		 SetPDACaret(hWnd); // ���ù��
     InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
     return 0;
}
/**************************************************
������static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- ָ��EDITITEMPROPERTY��ָ�룬��Ŀ������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��������������ָ����Ŀ�����ԣ�����EM_SETITEM��Ϣ��
����: 
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
      GetClientRect(hWnd,&ClientRect); // �õ��ͻ�����

     nItem=(int)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
     lpEditItemProperty=(LPEDITITEMPROPERTY)lParam; // �õ���Ŀ����

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);  // �õ��༭��������
     if (lpMultiItemEditStruct==NULL) return FALSE;

     if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<0)
        return FALSE;
     lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ���Ŀ�ṹ
     OldEditItem=*lpEditItem;
     iItemStartPos=lpEditItem->iStartPos;
     if (SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos)==FALSE) // ������Ŀ
     {
         *lpEditItem=OldEditItem;
         return FALSE;
     }

     // �ͷ�ԭ�������ָ��
     if (OldEditItem.lpPDAControlBuffer)
       free(OldEditItem.lpPDAControlBuffer);
     if (OldEditItem.lpPDASaveEditBuffer)
       free(OldEditItem.lpPDASaveEditBuffer);
     if (OldEditItem.lpFormat)
       free(OldEditItem.lpFormat);

     // ���µ�����ʼλ��
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
     // ��������
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
     AdjustCaretPosition(hWnd); // �������
     InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
     return TRUE;
}

/**************************************************
������static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- ����
����ֵ���ɹ�����TRUE�����򷵻�FALSE
����������ɾ��ָ����Ŀ������EM_DELETEITEM��Ϣ��
����: 
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

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
     if (lpMultiItemEditStruct==NULL) return FALSE;

     if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<0)
        return FALSE;
     OldEditItem=lpMultiItemEditStruct->lpItemList[nItem]; // �õ�ԭ������Ŀ�ṹ
     iItemStartPos=(int)OldEditItem.iStartPos;
     lpMultiItemEditStruct->nItemNum--;
     for (i=nItem;i<lpMultiItemEditStruct->nItemNum;i++)
     { // ɾ������Ŀ
        lpMultiItemEditStruct->lpItemList[i]=lpMultiItemEditStruct->lpItemList[i+1];
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(int)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     // ���ù�����
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
     InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
/*     lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     OldEditItem=*lpEditItem;
     iItemStartPos=lpEditItem->iStartPos;
     if (SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos)==FALSE)
     {
         *lpEditItem=OldEditItem;
         return FALSE;
     }
*/
     // �ͷ�ָ��
     if (OldEditItem.lpPDAControlBuffer)
       free(OldEditItem.lpPDAControlBuffer);
     if (OldEditItem.lpPDASaveEditBuffer)
       free(OldEditItem.lpPDASaveEditBuffer);
     if (OldEditItem.lpFormat)
       free(OldEditItem.lpFormat);

     return TRUE;
}
/**************************************************
������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT��Ҫ���õ���ɫֵ
����ֵ����
�������������ô�����ɫ������WM_SETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
//	COLORREF cl_NewColor;
	LPCTLCOLORSTRUCT lpCtlColor;
		
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

		 lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
		 if (lpMultiItemEditStruct==NULL) 
			 return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // �����ı���ɫ
			 lpMultiItemEditStruct->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�����
				lpMultiItemEditStruct->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // ����ѡ���ı���ɫ
				lpMultiItemEditStruct->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // ����ѡ���ı�����ɫ
				lpMultiItemEditStruct->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // ����DISABLE �ı���ɫ
				lpMultiItemEditStruct->cl_DisableText = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // ����DIASBLE �ı�����ɫ
				lpMultiItemEditStruct->cl_DisableBkColor = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 { // ����ֻ���ı���ɫ
				lpMultiItemEditStruct->cl_ReadOnly = lpCtlColor->cl_ReadOnly;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 { // ����ֻ���ı�����ɫ
				lpMultiItemEditStruct->cl_ReadOnlyBk  = lpCtlColor->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // ���ñ����ı���ɫ
				lpMultiItemEditStruct->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // ���ñ����ı�����ɫ
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
������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- LPCTLCOLORSTRUCT����ŵõ�����ɫֵ
����ֵ���ɹ�����TRUE�����򷵻�FALSE
�����������õ���ǰ���ڵ���ɫ������WM_GETCTLCOLOR��Ϣ��
����: 
************************************************/
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
		
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPCTLCOLORSTRUCT lpCtlColor;

		 lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
		 if (lpMultiItemEditStruct==NULL) 
			return FALSE;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam; // �õ���ɫ�ṹ

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 { // �õ��ı���ɫ
			 lpCtlColor->cl_Text = lpMultiItemEditStruct->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �õ��ı�����ɫ
				lpCtlColor->cl_TextBk = lpMultiItemEditStruct->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // �õ�ѡ���ı���ɫ
				lpCtlColor->cl_Selection = lpMultiItemEditStruct->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 { // �õ�ѡ���ı�����ɫ
				lpCtlColor->cl_SelectionBk = lpMultiItemEditStruct->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // �õ�DISABLE�ı���ɫ
				lpCtlColor->cl_Disable= lpMultiItemEditStruct->cl_DisableText ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // �õ�DISABLE�ı�����ɫ
				lpCtlColor->cl_DisableBk = lpMultiItemEditStruct->cl_DisableBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 { // �õ�ֻ���ı���ɫ
				lpCtlColor->cl_ReadOnly= lpMultiItemEditStruct->cl_ReadOnly ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 { // �õ�ֻ���ı�����ɫ
				lpCtlColor->cl_ReadOnlyBk = lpMultiItemEditStruct->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // �õ������ı���ɫ
				lpCtlColor->cl_Title= lpMultiItemEditStruct->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 { // �õ������ı�����ɫ
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
������static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,int iItemStartPos)
������
	IN hWnd -- ���ھ��
	IN lpEdititem -- �༭��Ŀ�ṹָ��
	IN lpEditItemProperty -- �༭��Ŀ���Խṹָ��
	IN iItemStartPos -- ��Ŀ��ʼλ��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
����������������Ŀ���ԡ�
����: 
************************************************/
static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,int iItemStartPos)
{
   RECT ClientRect;
   int nTextHeigth;
   int nFormatLen;
   LPTSTR lpTitle,lpItemText,lpFormat;

// !!! Add By Jami chen in 2003.07.31
#ifdef _MAPPOINTER
	lpTitle = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpTitle, GetCallerProcess() );  // ӳ��ָ��
	lpItemText = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpItemText, GetCallerProcess() ); 
	lpFormat = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpFormat, GetCallerProcess() ); 
#else
	lpTitle = lpEditItemProperty->lpTitle;  // ӳ��ָ��
	lpItemText = lpEditItemProperty->lpItemText; 
	lpFormat = lpEditItemProperty->lpFormat; 
#endif
// !!! Add End By Jami chen in 2003.07.31

      nTextHeigth=GetTextHeight(hWnd); // �õ��ı��߶�
      GetClientRect(hWnd,&ClientRect); // �õ��ͻ�����
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
     lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen); // ����༭����
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
	 // �õ���Ŀλ��
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
������int GetItem(HWND hWnd,int xPos,int yPos)
������
	IN hWnd -- ���ھ��
	IN xPos -- x����
	IN yPos -- y����
����ֵ��������Ŀ����
�����������õ�ָ���������Ŀ������
����: 
************************************************/
int GetItem(HWND hWnd,int xPos,int yPos)
{
 LPMULTIITEMEDITSTRUCT   lpMultiItemEditStruct;
 int nItem;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if(lpMultiItemEditStruct==NULL) return -1;
    if (lpMultiItemEditStruct->nItemNum==0) return -1;

    for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    { // �õ�ָ��λ�õ���Ŀ
      if (yPos<lpMultiItemEditStruct->lpItemList[nItem].iStartPos)
        return nItem-1;
    }
    return nItem-1;

}

/**************************************************
������static LRESULT DoGetActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ�����ؼ�����Ŀ������
�����������õ�������Ŀ������������EM_GETACTIVEITEM��Ϣ��
����: 
************************************************/
static LRESULT DoGetActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPMULTIITEMEDITSTRUCT   lpMultiItemEditStruct;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
	    if(lpMultiItemEditStruct==NULL) 
			return -1;
		if (lpMultiItemEditStruct->nItemNum==0) 
			return -1;
		return lpMultiItemEditStruct->iActiveItem; // ���ؼ�����Ŀ����
}

/**************************************************
������static LRESULT DoGetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- ����
����ֵ������ָ����Ŀ�ķ��
�����������õ�ָ����Ŀ�ķ�񣬴���EM_GETITEMSTYLE��Ϣ��
����: 
************************************************/
static LRESULT DoGetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;
		if (nItem < 0 || nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
	    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ���Ŀ�ṹ
		return lpEditItem->dwStyle; // ������Ŀ���
}
/**************************************************
������static LRESULT DoSetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- INT ָ����Ŀ
	IN lParam -- DWORD Ҫ���õķ��
����ֵ����
�����������趨ָ����Ŀ�ķ�񣬴���EM_SETITEMSTYLE��Ϣ��
����: 
************************************************/
static LRESULT DoSetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  int nItem;
  DWORD dwStyle;
  HDC hdc;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(int)wParam;
		dwStyle = (DWORD)lParam;
		if (nItem < 0 || nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
	    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem]; // �õ���Ŀ�ṹ
		lpEditItem->dwStyle = dwStyle; //������Ŀ���
		hdc = GetDC(hWnd);
        DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWALL); // ���Ƹ���Ŀ
		ReleaseDC(hWnd,hdc);
		return 0;
}




// ********************************************************************
// ������static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPWINDOWPOS ָ�� 
// ����ֵ��
//	
// ��������������λ�÷����ı�
// ����: 
// ********************************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpWindowPos;

		lpWindowPos = (LPWINDOWPOS)lParam;

		if ((lpWindowPos->flags & SWP_NOSIZE) == 0)
		{
			// ���ڴ�С�����ı�
			DoSize(hWnd,0,0);
		}

		return 0;
}

// **************************************************
// ������static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	OUT wParam -- LPINT ָ��INT��ָ�룬��Ź�����ڵ��С�
// 	OUT lParam -- LPINT ָ��INT��ָ�룬��Ź�����ڵ�X���ꡣ
// ����ֵ����
// �����������õ���ǰ����λ�ã�����EM_GETCARETPOS��Ϣ��
// ����: 
// **************************************************
static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int nCaretLine;
  LPEDITITEM lpActiveEditItem;
  int *lpnLine,*lpxPos;

#ifdef _MAPPOINTER
		wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // ӳ��ָ��
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif


        lpnLine=(int *)wParam;
        lpxPos=(int *)lParam;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0); // �õ��༭��������
		if (lpMultiItemEditStruct==NULL) return ;
		if (lpMultiItemEditStruct->nItemNum==0) return ;  // û����Ŀ
		lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];  // �õ�������Ŀ
		nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpActiveEditItem->nCaretLine; // �õ�����λ��

		if (lpnLine)
			*lpnLine=nCaretLine;  // �õ���������
		if (lpxPos)
			*lpxPos=lpActiveEditItem->nCaretx+lpActiveEditItem->nDisplayx;  // �õ�����ˮƽλ��
}
