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
#include <gwmesrv.h>


#define _MAPPOINTER

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
void ReSetItemPos(HWND hWnd,short iChangedSequence,short nChangedLine,BOOL bRedraw);
static void ClearBottomBlankArea(HWND hWnd);
static short GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem);
static short VScrollMultiItemEdit(HWND hWnd,short nScrollLine);
static void HScrollMultiItemEdit(HWND hWnd,short xScrollPos);
static BOOL AdjustCaretPosition(HWND hWnd);
static void SetMIEditVScrollRange(HWND hWnd,short nTotalLine,short nWindowLine);
static void SetMIEditScrollPage(HWND hWnd);
static void SetMIEditVScrollPos(HWND hWnd,short nNewScrollPos);
static short DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static short GetPageLine(HWND hWnd);
static void SetMIEditHScrollPos(HWND hWnd,short nNewScrollPos);
//static void DoDisplayChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem);
//static BOOL MoveCaret(HWND hWnd,short nScrollLine);
static BOOL MoveCaret(HWND hWnd,short nScrollLine,short iHorzPos);

static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,short iItemStartPos);

static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT InitialMultiItemEdit(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
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

/*
    This function is register a PDAEdit Class to System.
	This function must be Transfered once in the start time
*/
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

/*
    This function is PDAEdit Windows procedure
*/
LRESULT CALLBACK MultiItemEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
 
	switch (message)
	{
		case WM_CREATE:
			// Create a window use this class
		  return InitialMultiItemEdit(hWnd,wParam,lParam);
		case WM_COMMAND:
			break;
    case WM_SIZE:
       return DoSize(hWnd,wParam,lParam);
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
//			ClearBottomBlankArea(hWnd);
      DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
		case WM_SETFOCUS:
      return DoSetFocus(hWnd,wParam,lParam);
    case WM_KILLFOCUS:
      return DoKillFocus(hWnd,wParam,lParam);
		case WM_LBUTTONDOWN:
      return DoLButtonDown(hWnd,wParam,lParam);
    case WM_MOUSEMOVE:
      return DoMouseMove(hWnd,wParam,lParam);
    case WM_LBUTTONUP:
      return DoLButtonUp(hWnd,wParam,lParam);
    case WM_CHAR:
      return DoProcessChar(hWnd,wParam,lParam);
    case WM_KEYDOWN:
      return DoKeyDown(hWnd,wParam,lParam);
		case WM_DESTROY:
			// close this window
			ReleaseMultiItemEdit(hWnd);
//			PostQuitMessage(0);
			break;
    case WM_VSCROLL:
      DoVScrollWindow(hWnd,wParam,lParam);
      break;
    case WM_HSCROLL:
      DoHScrollWindow(hWnd,wParam,lParam);
      break;
	case WM_PRINTCLIENT:
	  DoPrintClient(hWnd,wParam,lParam);
	  break;

    case EM_CHARFROMPOS:
      return DoCharFromPos(hWnd,wParam,lParam);
		case EM_GETFIRSTVISIBLELINE:
			return DoGetFirstVisibleLine(hWnd,wParam,lParam);
		case EM_GETLIMITTEXT:
			return DoGetLimitText(hWnd,wParam,lParam);
		case EM_GETLINE:
			return DoGetLine(hWnd,wParam,lParam);
		case EM_GETLINECOUNT:
			return DoGetLineCount(hWnd,wParam,lParam);
		case EM_GETMODIFY:
			return DoGetModify(hWnd,wParam,lParam);
		case EM_GETSEL:
			return DoGetSel(hWnd,wParam,lParam);
		case EM_GETPASSWORDCHAR:
			return DoGetPasswordChar(hWnd,wParam,lParam);
		case EM_GETTHUMB:
			return DoGetThumb(hWnd,wParam,lParam);
		case EM_LINEFROMCHAR:
			return DoLineFromChar(hWnd,wParam,lParam);
		case EM_LINEINDEX:
				return DoLineIndex(hWnd,wParam,lParam);
		case EM_LINELENGTH:
				return DoLinelength(hWnd,wParam,lParam);
		case EM_LINESCROLL:
				return DoLineScroll(hWnd,wParam,lParam);
		case EM_POSFROMCHAR:
			return DoPosFromChar(hWnd,wParam,lParam);
		case EM_REPLACESEL:
			return DoReplaceSel(hWnd,wParam,lParam);
		case EM_SCROLL:
			return DoScroll(hWnd,wParam,lParam);
		case EM_SCROLLCARET:
			return DoScrollCaret(hWnd,wParam,lParam);
		case EM_SETLIMITTEXT:
			return DoSetLimitText(hWnd,wParam,lParam);
		case EM_SETMODIFY:
			return DoSetModify(hWnd,wParam,lParam);
		case EM_SETPASSWORDCHAR:
			return DoSetPasswordChar(hWnd,wParam,lParam);
		case EM_SETREADONLY:
			return DoSetReadOnly(hWnd,wParam,lParam);
    case EM_SETSEL:
      return DoSetSel(hWnd,wParam,lParam);
		case EM_EMPTY:
			return DoEmpty(hWnd,wParam,lParam);
// new message
		case EM_INSERTITEM:
			return DoInsertItem(hWnd,wParam,lParam);
		case EM_SETITEM:
			return DoSetItem(hWnd,wParam,lParam);
    case EM_DELETEITEM:
      return DoDeleteItem(hWnd,wParam,lParam);
		case WM_COPY:
			return DoCopy(hWnd,wParam,lParam);
		case WM_PASTE:
			return DoPaste(hWnd,wParam,lParam);
		case WM_CUT:
			return DoCut(hWnd,wParam,lParam);
		case WM_CLEAR:
			return DoClear(hWnd,wParam,lParam);
		case WM_SETTEXT:
			return DoSetText(hWnd,wParam,lParam);
		case WM_GETTEXT:
			return DoGetText(hWnd,wParam,lParam);
	    case WM_GETTEXTLENGTH:
			 return DoGetTextLength(hWnd , wParam ,lParam);
		case EM_GETITEMTEXT:
			return DoGetItemText(hWnd,wParam,lParam);
		case EM_SETITEMTEXT:
			return DoSetItemText(hWnd,wParam,lParam);
	    case EM_GETITEMTEXTLENGTH:
			 return DoGetItemTextLength(hWnd , wParam ,lParam);
		case EM_SETITEMCALLBACK:
			 return DoSetItemCallBack(hWnd,wParam,lParam);

		case EM_GETACTIVEITEM:
			return DoGetActiveItem(hWnd,wParam,lParam);
//		case EM_SETCOLOR:
//			return DoSetColor(hWnd,wParam,lParam);
//		case EM_GETCOLOR:
//			return DoGetColor(hWnd,wParam,lParam);
		case WM_SETCTLCOLOR	:
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	:
			return DoGetColor(hWnd,wParam,lParam);
		case EM_GETITEMSTYLE:
			return DoGetItemStyle(hWnd,wParam,lParam);
		case EM_SETITEMSTYLE:
			return DoSetItemStyle(hWnd,wParam,lParam);



		default:
			// default procedure
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
static LRESULT InitialMultiItemEdit(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	PDAEDITPROPERTY PDAEditProperty;
  RECT ClientRect;
  short iNextWndPos=0,nTextHeigth;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short nClientLine;
	LPCREATESTRUCT lpcs;
  LPMULTIITEMEDITPROPERTY lpMultiItemEditProperty;
  LPEDITITEMPROPERTY lpEditItemProperty;
  int i;
  LPEDITITEM lpEditItem;
 // short nFormatLen;
  DWORD dwStyle;

	
#ifdef _MAPPOINTER
	  lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

	  lpcs = (LPCREATESTRUCT) lParam; // structure with creation data
	  lpMultiItemEditProperty=(LPMULTIITEMEDITPROPERTY)lpcs->lpCreateParams;

// !!! Add By Jami chen in 2003.07.31
#ifdef _MAPPOINTER
	  if (lpMultiItemEditProperty)
			lpEditItemProperty = (LPEDITITEMPROPERTY)MapPtrToProcess( (LPVOID)lpMultiItemEditProperty->lpEditItemProperty, GetCallerProcess() ); 
#endif
// !!! Add End By Jami chen in 2003.07.31

      lpMultiItemEditStruct=malloc(sizeof(MULTIITEMEDITSTRUCT));
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      dwStyle|=ES_MULTIITEMEDIT|ES_LINESEPARATE;
      SetWindowLong(hWnd,GWL_STYLE,dwStyle);
      if (lpMultiItemEditStruct==NULL)
			{
				//SendNotificationMsg(hWnd,EN_ERRSPACE);
				return -1;
			}
      if (lpMultiItemEditProperty==NULL)
      {
        lpMultiItemEditStruct->nItemNum=0;
      }
      else
      {
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
        lpMultiItemEditStruct->lpItemList=malloc((lpMultiItemEditStruct->nItemNum)*sizeof(EDITITEM));
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
      lpMultiItemEditStruct->nItemStartLineList=malloc((lpMultiItemEditStruct->nItemNum+1)*sizeof(short));
      if (lpMultiItemEditStruct->nItemStartLineList==NULL)
      {
        free(lpMultiItemEditStruct->lpItemList);
        free(lpMultiItemEditStruct);
				//SendNotificationMsg(hWnd,EN_ERRSPACE);
        return -1;
      }
//      lpMultiItemEditStruct->nItemNum=lpMultiItemEditProperty->nItemNum;


      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);
      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {
          lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
// !!! Modified By Jami chen in 2003.07.31
//          if (SetEditItem(hWnd,lpEditItem,&lpMultiItemEditProperty->lpEditItemProperty[i],iNextWndPos)==FALSE)
          if (SetEditItem(hWnd,lpEditItem,&lpEditItemProperty[i],iNextWndPos)==FALSE)
// !!! Modified End By Jami chen in 2003.07.31
          {
            for (i--;i>=0;i--)
            {
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
		             SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(short)(lpEditItem->cbEditLen+1));
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
		             GetEditText(lpEditItem->lpPDAEditBuffer,(lpMultiItemEditProperty->lpEditItemProperty[i]).lpItemText,(short)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
	             }
	             SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpcs->lpszName,(short)(lpEditItem->cbEditLen+1));
             }
             else
             {
	             lpEditItem->cbEditLen=0;
	             SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(short)(lpEditItem->cbEditLen+1));
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
          iNextWndPos+=(short)lpEditItem->rect.bottom;
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
	  SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE);
//      AdjustCaretPosition(hWnd);
      return 0;
}
static void ReleaseMultiItemEdit(HWND hWnd)
{
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
    LPEDITITEM lpEditItem;
    short nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return;
			if (lpMultiItemEditStruct->nItemStartLineList)
				free(lpMultiItemEditStruct->nItemStartLineList);

      for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
      {
         lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

			   if (lpEditItem->lpPDASaveEditBuffer)
				   free(lpEditItem->lpPDASaveEditBuffer);
			   if (lpEditItem->lpPDAControlBuffer)
				   free(lpEditItem->lpPDAControlBuffer);
			   if (lpEditItem->lpFormat)
				   free(lpEditItem->lpFormat);
      }

			if (lpMultiItemEditStruct->lpItemList)
				free(lpMultiItemEditStruct->lpItemList);
			free(lpMultiItemEditStruct);

	    DeletePDACaret(hWnd);
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);
			return;
}
/*static void DoSizeChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    HWND hChangedWnd;
    short nChangedLine,nCaretLine;
    short iActiveItem;
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return;

      hChangedWnd=(HWND)wParam;
      nChangedLine=(short)HIWORD(lParam);
      nCaretLine=(short)LOWORD(lParam);
//!!      iActiveItem=GetItemSequence(hWnd,hChangedWnd);
      ReSetItemPos(hWnd,iActiveItem,nChangedLine);
      nCaretLine+=lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
      AdjustCaretPosition(hWnd);
} */
void ChangeSize(HWND hWnd,LPEDITITEM lpEditItem,short nChangedLine,BOOL bRedraw)
{
  short iActiveItem;

      iActiveItem=GetItemSequence(hWnd,lpEditItem);
      ReSetItemPos(hWnd,iActiveItem,nChangedLine,bRedraw);
}
static void ReSetItemPos(HWND hWnd,short iChangedSequence,short nChangedLine,BOOL bRedraw)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;
  short nTextHeight,nNewStartPos;
  short nClientLine;
  RECT rcClientRect;
  HDC hdc;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return;
      nTextHeight=GetTextHeight(hWnd);
      i=iChangedSequence+1;
      hdc=GetDC(hWnd);
      for (;i<lpMultiItemEditStruct->nItemNum;i++)
      {
        lpMultiItemEditStruct->nItemStartLineList[i]+=nChangedLine;
        nNewStartPos=(lpMultiItemEditStruct->nItemStartLineList[i]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
        lpMultiItemEditStruct->lpItemList[i].iStartPos=nNewStartPos;
        if (bRedraw)
         DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[i],PE_SHOWCLIENT);
      }
      ReleaseDC(hWnd,hdc);
      lpMultiItemEditStruct->nItemStartLineList[i]+=nChangedLine;
      lpMultiItemEditStruct->nContextBottomPos+=nChangedLine*nTextHeight;
      if (bRedraw)
        ClearBottomBlankArea(hWnd);

      GetClientRect(hWnd,&rcClientRect);
      nClientLine=rcClientRect.bottom/nTextHeight;
      SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
}

static void ClearBottomBlankArea(HWND hWnd)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  RECT rcClientRect,rcClearRect;
  HDC hdc;
	short nBottomPos;
	HBRUSH hBrush;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return;
		nBottomPos=lpMultiItemEditStruct->nContextBottomPos;
		GetClientRect(hWnd,&rcClientRect);
    if (nBottomPos<rcClientRect.bottom)
    {
	    rcClearRect.left=0;
	    rcClearRect.top=nBottomPos;
	    rcClearRect.right=rcClientRect.right;
	    rcClearRect.bottom =rcClientRect.bottom;
	    // clear ClearRect
	    hdc=GetDC(hWnd);
//	    FillRect(hdc,&rcClearRect,(HBRUSH)(COLOR_WINDOWFRAME));
//	    FillRect(hdc,&rcClearRect,GetStockObject(WHITE_BRUSH));
		hBrush = GetBkBrush(hWnd);
	    FillRect(hdc,&rcClearRect,hBrush);
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
  short nCaretLine,xPos,nItemCaretLine;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short nPageLine;
  short iActiveItem;
//  RECT rcClientRect;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;

    hActiveItemWnd=(HWND)wParam;
    xPos=(short)LOWORD(lParam);
    nItemCaretLine=(short)HIWORD(lParam);

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
static short GetItemSequence(HWND hWnd,LPEDITITEM lpEditItem)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short i;


      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return 0;
      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {
//        if (hItemWnd==lpMultiItemEditStruct->hItemWndList[i]) break;
          if (lpEditItem==&(lpMultiItemEditStruct->lpItemList[i])) break;
      }
      return i;
}
static short VScrollMultiItemEdit(HWND hWnd,short nScrollLine)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short i,iRealScrollLines;
  short nTextHeight,nNewStartPos;
  short nPageLine,nOldDisplayLine;
//	RECT rcRedraw;
//  LPEDITITEM lpActiveEditItem;



      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return 0;
      nPageLine=GetPageLine(hWnd);
      if (nScrollLine==0) return 0;
      if (nScrollLine>0)
      {
        if ((lpMultiItemEditStruct->nDisplayLine+nPageLine)>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
          return 0;
      }
      if (nScrollLine<0)
      {
        if (lpMultiItemEditStruct->nDisplayLine==0)
          return 0;
      }
			nOldDisplayLine=lpMultiItemEditStruct->nDisplayLine;
      lpMultiItemEditStruct->nDisplayLine+=nScrollLine;
      nPageLine=GetPageLine(hWnd);
      if ((lpMultiItemEditStruct->nDisplayLine+nPageLine)>lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
        lpMultiItemEditStruct->nDisplayLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-nPageLine;
      if (lpMultiItemEditStruct->nDisplayLine<0)
        lpMultiItemEditStruct->nDisplayLine=0;
      nTextHeight=GetTextHeight(hWnd);

			iRealScrollLines=(lpMultiItemEditStruct->nDisplayLine-nOldDisplayLine);
	//		GetClientRect(hWnd,&rcRedraw);

      for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
      {
        nNewStartPos=(lpMultiItemEditStruct->nItemStartLineList[i]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
        lpMultiItemEditStruct->lpItemList[i].iStartPos=nNewStartPos;
//        SetWindowPos(lpMultiItemEditStruct->hItemWndList[i],0,0,nNewStartPos,0,0,SWP_NOSIZE|SWP_NOZORDER|SWP_NOREDRAW);
        //        InvalidateRect(lpMultiItemEditStruct->hItemWndList[i],NULL,TRUE);
      }
//      lpMultiItemEditStruct->nContextBottomPos+=nScrollLine*nTextHeight;
      InvalidateRect(hWnd,NULL,TRUE);
      lpMultiItemEditStruct->nContextBottomPos=(lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-lpMultiItemEditStruct->nDisplayLine)*nTextHeight;
//      ClearBottomBlankArea(hWnd);

      SetMIEditVScrollPos(hWnd,lpMultiItemEditStruct->nDisplayLine);

//      lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
		  SetPDACaret(hWnd);
			return (short)iRealScrollLines;
}
static void HScrollMultiItemEdit(HWND hWnd,short xScrollPos)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
 
      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return;
      lpMultiItemEditStruct->xDisplayPos+=xScrollPos;
      if (lpMultiItemEditStruct->xDisplayPos<0) lpMultiItemEditStruct->xDisplayPos=0;;
      SetMIEditHScrollPos(hWnd,(short)((lpMultiItemEditStruct->xDisplayPos)/HSCROLLWIDTH));
}

static BOOL AdjustCaretPosition(HWND hWnd)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short nScrollLine,nPageLine;
//  RECT rcClientRect;
  short nCaretLine;
  BOOL bResult=FALSE;
  LPEDITITEM lpActiveEditItem;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;
    if (lpMultiItemEditStruct->nItemNum==0) return FALSE;
    lpActiveEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
    AdjustCaretInEditItem(hWnd,lpActiveEditItem);
//    nTextHeight=GetTextHeight(hWnd);
//    GetClientRect(hWnd,&rcClientRect);
//    nPageLine=rcClientRect.bottom/nTextHeight;
    nPageLine=GetPageLine(hWnd);
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpActiveEditItem->nCaretLine;
    if (nCaretLine<0) nCaretLine=0;
    if (nCaretLine>=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum])
    {
      nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]-1;
      bResult=TRUE;
    }

    if (nCaretLine<lpMultiItemEditStruct->nDisplayLine)
    {
      nScrollLine=nCaretLine-lpMultiItemEditStruct->nDisplayLine;
//      lpMultiItemEditStruct->nDisplayLine=nCaretLine;
      VScrollMultiItemEdit(hWnd,nScrollLine);
      bResult=TRUE;
    }
    if (nCaretLine>=lpMultiItemEditStruct->nDisplayLine+nPageLine)
    {
      nScrollLine=nCaretLine-(lpMultiItemEditStruct->nDisplayLine+nPageLine)+1;
//      lpMultiItemEditStruct->nDisplayLine=nCaretLine-nPageLine+1;
      VScrollMultiItemEdit(hWnd,nScrollLine);
      bResult=TRUE;
    }
    return bResult;
}
static void SetMIEditVScrollRange(HWND hWnd,short nTotalLine,short nWindowLine)
{
  short nMinPos,nMaxPos;
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
//      if (!(dwStyle&WS_VSCROLL)) return;
	  if (nWindowLine==0)
		  nWindowLine=1;
      if (nTotalLine<=nWindowLine)
      {
					if ((dwStyle&WS_VSCROLL))
					{
						ShowScrollBar(hWnd,SB_VERT,FALSE);
            DoSize(hWnd,0,0);
//						SetHScrollBar(hWnd);
					}
//        EnableScrollBar(hWnd,SB_VERT,ESB_DISABLE_BOTH);
      }
      else
      {
        nMinPos=0;
        nMaxPos=nTotalLine-1;//-nWindowLine+1;
		if ((dwStyle&WS_VSCROLL)==0)
		{
			ShowScrollBar(hWnd,SB_VERT,TRUE);
			DoSize(hWnd,0,0);
	        Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
			SetMIEditScrollPage(hWnd);
		}
		else
			Edit_SetScrollRange(hWnd,SB_VERT,nMinPos,nMaxPos,TRUE);
        EnableScrollBar(hWnd,SB_VERT,ESB_ENABLE_BOTH);
      }
}
static void SetMIEditScrollPage(HWND hWnd)
{
   short nPageLine;
//   short nTextHeight;
   //RECT rcClientRect;
   SCROLLINFO ScrollInfo;
   DWORD dwStyle;
      
      dwStyle=GetWindowLong(hWnd,GWL_STYLE);

      nPageLine=GetPageLine(hWnd);

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
static void SetMIEditVScrollPos(HWND hWnd,short nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE);
}
static void SetMIEditHScrollPos(HWND hWnd,short nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      if (!(dwStyle&WS_HSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE);
}
static short DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    short nScrollLine,nWindowLine;
    RECT rcClientRect;
    short nTextHeight,yPos;
        
      GetClientRect(hWnd,&rcClientRect);
      nTextHeight=GetTextHeight(hWnd);
      nWindowLine=rcClientRect.bottom/nTextHeight;
      yPos=Edit_GetScrollPos(hWnd,SB_VERT);
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:
          nScrollLine=0-nWindowLine;
          break;
        case SB_PAGEDOWN:
          nScrollLine=nWindowLine;
          break;
        case SB_LINEUP:
          nScrollLine=-1;
          break;
        case SB_LINEDOWN:
          nScrollLine=1;
          break;
        case SB_THUMBTRACK:
          nScrollLine=HIWORD(wParam)-yPos;
          break;
        default:
          nScrollLine=0;
          return 0;
        }
        return VScrollMultiItemEdit(hWnd,nScrollLine);
}

static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    short xScrollPos,xWindowWidth;
    RECT rcClientRect;
    short xPos;
        
      GetClientRect(hWnd,&rcClientRect);
      xWindowWidth=(rcClientRect.right/HSCROLLWIDTH)*HSCROLLWIDTH;
      xPos=Edit_GetScrollPos(hWnd,SB_HORZ);
      switch(LOWORD(wParam))
        {
        case SB_PAGEUP:
          xScrollPos=0-xWindowWidth;
          break;
        case SB_PAGEDOWN:
          xScrollPos=xWindowWidth;
          break;
        case SB_LINEUP:
          xScrollPos=0-HSCROLLWIDTH;
          break;
        case SB_LINEDOWN:
          xScrollPos=HSCROLLWIDTH;
          break;
        case SB_THUMBTRACK:
          xScrollPos=(HIWORD(wParam)-xPos)*HSCROLLWIDTH;
//          Print(100,10,xScrollPos);
          break;
        default:
          return;

        }
 
      HScrollMultiItemEdit(hWnd,xScrollPos);
}

static short GetPageLine(HWND hWnd)
{
  short nTextHeight,nPageLine;
  RECT rcClientRect;
    nTextHeight=GetTextHeight(hWnd);
    GetClientRect(hWnd,&rcClientRect);
    nPageLine=rcClientRect.bottom/nTextHeight;
	if (nPageLine==0)
		nPageLine=1;
    return nPageLine;
}
/*
static void DoDisplayChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  short nDisplayx,nCaretLine;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  int i;
  HWND hActiveWnd;
  short iActiveItem;

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
          SetMIEditHScrollPos(hWnd,(short)((lpMultiItemEditStruct->xDisplayPos)/HSCROLLWIDTH));
      }
// !!      iActiveItem=GetItemSequence(hWnd,hActiveWnd);
      nCaretLine+=lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
      AdjustCaretPosition(hWnd);
}
*/
void SetCaretToNextItem(HWND hWnd,LPEDITITEM lpEditItem)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpNewEditItem;
  short iActiveItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return;
    iActiveItem=GetItemSequence(hWnd,lpEditItem);
    iActiveItem++;
    if (iActiveItem>=lpMultiItemEditStruct->nItemNum)
      return;
    ClearInvert(hWnd,lpEditItem,TRUE);
    lpMultiItemEditStruct->iActiveItem=iActiveItem;
    lpNewEditItem=&lpMultiItemEditStruct->lpItemList[iActiveItem];
		SetPDACaretPosition(hWnd,lpNewEditItem,0,0);
    if (AdjustCaretPosition(hWnd)==TRUE);
//		{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
//				InvalidateRect(hWnd,NULL,TRUE);
//		}

		// set caret position
		SetPDACaret(hWnd);

    // show caret
    ShowPDACaret(hWnd);
	SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE);
//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,0,0);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
}
/**************************************************************************************/
// Do Process Message EM_CHARFROMPOS
// Input :
//    wParam = (short)nItem;                        // the Item Edit of the multiItemEdit
//    lParam = MAKELPARAM(xPos, yPos);   // coordinates of a point
// Return code:
//    The low word of the return value specifies the zero-based index of the character 
//    nearest the specified point. This index is relative to the beginning of the control, 
//    not the beginning of the line. If the specified point is beyond the last character 
//    in the edit control, the return value indicates the last character in the control. 

//    The high word of the return value specifies the zero-based index of the line that 
//    contains the character. For single-line edit controls, this value is zero. The index 
//    indicates the line delimiter if the specified point is beyond the last visible character 
//    in a line.
/************************************************************************************/
static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;
  short xPos,yPos;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

	  nItem=(short)wParam;
	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
    xPos=LOWORD(lParam);
    yPos=HIWORD(lParam);
    return GetCharFromPos(hWnd,lpEditItem,xPos,yPos);

//	return SendMessage(lpMultiItemEditStruct->hItemWndList[nItem],EM_CHARFROMPOS,0,lParam);
}
/************************************************************************************/
// Process Edit Message -- EM_GETFIRSTVISIBLELINE
// Input :
//    wParam = 0;                        // not used
//    lParam = 0;                        // not used
// Return Values
// The return value is the zero-based index of the uppermost visible line 
/************************************************************************************/
static LRESULT DoGetFirstVisibleLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

	return lpMultiItemEditStruct->nDisplayLine;
}
/************************************************************************************/
// Process Edit Message -- EM_GETLIMITTEXT 
// Input :
// wParam = (short)nItem;                        // the Item Edit of the multiItemEdit
// lParam = 0;  // not used, must be zero
// Return Values
// The return value is the text limit of the special item
/************************************************************************************/
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

	  nItem=(short)wParam;
	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
	  return lpEditItem->cbEditLimitLen;
}
/************************************************************************************/
// Process Edit Message -- EM_GETLINE
// An application sends an EM_GETLINE message to copy a line of text from an edit
//control and place it in a specified buffer.
// Input
// wParam = (WPARAM) line;          // line number to retrieve
// lParam = (LPARAM) (LPCSTR) lpch; // address of buffer for line

// Return Values
// The return value is the number of characters copied. The return value is zero
// if the line number specified by the line parameter is greater than the number
//of lines in the edit control.
/************************************************************************************/
static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	short nLine,nLineLen,nBufferMaxLen;
	LPSTR lpch;
	LPSTR lpLineAddress;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  nLine=(short)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
	  lpch=(LPSTR)lParam;
	  nItem=lpMultiItemEditStruct->nItemNum-1;
	  for (;nItem>=0;nItem--)
	  {
		  if (lpMultiItemEditStruct->nItemStartLineList[nItem]<=nLine)
			  break;
	  }
	  nLine-=lpMultiItemEditStruct->nItemStartLineList[nItem];
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);
		nLineLen=GetLineLength(hWnd,lpEditItem,(LPCTSTR)lpLineAddress,NULL);
		nBufferMaxLen=MAKEWORD(*lpch,*(lpch+1));
		if (nLineLen>=nBufferMaxLen)
			return 0;
		memcpy((void *)lpch,(const void *)lpLineAddress,nLineLen);
		lpch[nLineLen]=0;
		return nLineLen;
}
/************************************************************************************/
// Process Edit Message -- EM_GETLINECOUNT
// An application sends an EM_GETLINECOUNT message to retrieve the number of 
// lines in a multiline edit control. 
// Input :
// wParam = 0; // not used; must be zero 
// lParam = 0; // not used; must be zero 
 
// Return Values
// The return value is an integer specifying the number of lines in the multiline 
// edit control. If no text is in the edit control, the return value is 1.
/************************************************************************************/
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
		if (lpMultiItemEditStruct->nItemNum==0) return 0;

		return lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum];
}
/************************************************************************************/
// Process Edit Message -- EM_GETMODIFY 
// An application sends an EM_GETMODIFY message to determine whether the content of 
// an edit control has been modified. 
// Input :
// wParam = 0; // not used; must be zero 
// lParam = 0; // not used; must be zero

// Return Values
// If the content of edit control has been modified, the return value is TRUE; 
// otherwise, it is FALSE. 
/************************************************************************************/
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short nItem;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;
	if (lpMultiItemEditStruct->nItemNum==0) return FALSE;
	
	nItem=(short)wParam;

	if (nItem==-1)
	{
		for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum-1;nItem++)
		{
			lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
			if (lpEditItem->fModified)
			{
						return TRUE;
			}
		}
	}
	else
	{
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

/************************************************************************************/
// Process Edit Message -- EM_GETPASSWORDCHAR
// An application sends an EM_GETPASSWORDCHAR message to retrieve the password
// character displayed in an edit control when the user enters text.
// Input :
// wParam = (short)nItem; // the Item Edit of the multiple Item Edit
// lParam = 0;						// not used; must be zero

// Return Values
// The return value specifies the character to be displayed in place of the
// character typed by the user. The return value is NULL if no password character
// exists.
/************************************************************************************/
static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(short)wParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		if (lpEditItem->dwStyle&ES_PASSWORD)
				return lpEditItem->chPassWordWord;
		else
				return 0;
}
/************************************************************************************/
// Process Edit Message -- EM_LIMITTEXT
// An application sends an EM_LIMITTEXT message to limit the amount of text the 
// user may enter into an edit control.
// Input:
// wParam = (WPARAM) cchMax;   // text length, in characters 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// cchMax 
//    Value of wParam. Specifies the maximum number of characters the user can 
//    enter. If this parameter is zero, then will not to be set.
// Return Values
// This message does not return a value. 
// Remark: The Style is ES_FORMAT ,then this Message is no effect
/************************************************************************************/
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 short cchMax;
   short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		cchMax=(short)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

    return SetLimitText(hWnd,lpEditItem,cchMax);
}

/************************************************************************************/
// Process Edit Message --EM_LINEFROMCHAR
// An application sends an EM_LINEFROMCHAR message to retrieve the index of the line 
// that contains the specified character index in a multiline edit control. A character 
// index is the number of characters from the beginning of the edit control. 
// Input :
// wParam = (WPARAM) ich;  // character index 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// ich 
//    Value of wParam. Specifies the character index of the character contained in the 
//    line whose number is to be retrieved. If the ich parameter is 每1, either the 
//    line number of the current line (the line containing the caret) is retrieved or,
//    if there is a selection, the line number of the line containing the beginning of 
//    the selection is retrieved. 
// Return Values
// The return value is the zero-based line number of the line containing the character
// index specified by ich. 
/************************************************************************************/
static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;
	DWORD dwStyle;
	LPTSTR lpCurPos;
	short ich,nLine;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		ich=(short)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		dwStyle=lpEditItem->dwStyle;
		if (ich==-1)
		{
				if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
				{
						lpCurPos=lpEditItem->lpCaretPosition;
				}
				else
				{
						lpCurPos=lpEditItem->lpInvertStart;
				}
		}
		else
		{
				if (ich>lpEditItem->cbEditLen)
					ich=lpEditItem->cbEditLen;
				lpCurPos=lpEditItem->lpPDAEditBuffer+ich;
		}
    GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE);
		return (LRESULT)nLine;
}

/************************************************************************************/
// Process Edit Message -- EM_LINEINDEX
// An application sends an EM_LINEINDEX message to retrieve the character index of a 
// line in a multiline edit control. The character index is the number of characters 
// from the beginning of the edit control to the specified line. 
// Input :
// wParam = (WPARAM) line; // line number 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// line 
//    Value of wParam. Specifies the zero-based line number. A value of 每1 specifies 
//		the current line number (the line that contains the caret). 
// Return Values
// The return value is the character index of the line specified in the line parameter, 
// or it is 每1 if the specified line number is greater than the number of lines in the 
// edit control.
/************************************************************************************/
static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	LPTSTR lpCurPos;
	short nLine;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nLine=(short)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		if (nLine==-1)
		{
				nLine=lpEditItem->nCaretLine;
		}
		lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine);
		return (LRESULT)(lpCurPos-lpEditItem->lpPDAEditBuffer);
}
/************************************************************************************/
// Process Edit Message -- EM_LINELENGTH
// An application sends an EM_LINELENGTH message to retrieve the length of a line, in 
// characters, in an edit control. 
// Input :
// wParam = (WPARAM) ich;  // character index 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// ich 
//			Value of wParam. Specifies the character index of a character in the line whose 
//			length is to be retrieved when EM_LINELENGTH is sent to a multiline edit control.
//			If this parameter is 每1, the message returns the number of unselected characters
//			on lines containing selected characters. For example, if the selection extended
//			from the fourth character of one line through the eighth character from the end 
//			of the next line, the return value would be 10 (three characters on the first line 
//			and seven on the next). 
// Return Values
// The return value is the length, in characters, of the line specified by the ich parameter 
// when an EM_LINELENGTH message is sent to a multiline edit control. The return value is the
// length, in characters, of the text in the edit control when an EM_LINELENGTH message is 
// sent to a single-line edit control. 
/************************************************************************************/
static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short ich;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		ich=(short)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
    return GetLineLengthFromPos(hWnd,lpEditItem,ich);
}
/************************************************************************************/
// Process Edit Message -- EM_LINESCROLL
// An application sends an EM_LINESCROLL message to scroll the text vertically or 
// horizontally in a multiline edit control. 
// Input:
// wParam = (WPARAM) cxScroll; // characters to scroll horizontally 
// lParam = (LPARAM) cyScroll; // lines to scroll vertically 
 
// Parameters
// cxScroll 
//     Value of wParam. Specifies the number of characters to scroll horizontally. 
// cyScroll
//     Value of lParam. Specifies the number of lines to scroll vertically. 
// Return Values
// If the message is sent to a multiline edit control, the return value is TRUE; if the 
// message is sent to a single-line edit control, the return value is FALSE. 
/************************************************************************************/
static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	short cxScroll,cyScroll;

		cxScroll=(short)wParam;
		cyScroll=(short)lParam;
		if (cxScroll)
		{
//				cxScroll=cxScroll*HSCROLLWIDTH;
				cxScroll=cxScroll*8; // 8--> the width of the char
				HScrollMultiItemEdit(hWnd,cxScroll);
		}
		if (cyScroll)
				VScrollMultiItemEdit(hWnd,cyScroll);
		return TRUE;
}
/************************************************************************************/
// Process Edit Message -- EM_POSFROMCHAR
// An application sends the EM_POSFROMCHAR message to retrieve the coordinates of 
// the specified character in an edit control.
// Input :
// wParam = (WPARAM) wCharIndex;    // zero-based index of character 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// wCharIndex 
//			Value of wParam. Specifies the zero-based index of the character. 
// Return Values
// The return value is the position of the character, (x, y). For a single-line edit 
// control, the y-coordinate is always zero. 
/************************************************************************************/
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	short wCharIndex;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		wCharIndex=(short)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
    return GetPosFromChar(hWnd,lpEditItem,wCharIndex);
}
/************************************************************************************/
// Process Edit Message -- EM_REPLACESEL
// An application sends an EM_REPLACESEL message to replace the current selection in 
// an edit control with the specified text. 
// Input :
// wParam = (short)nItem; // the Item Edit of the multiple Item Edit
// lpszReplace = (LPCTSTR) lParam ;   // pointer to replacement text string 
 
// Parameters
// lpszReplace 
//		Value of lParam. Pointer to a null-terminated string containing the replacement text. 
// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPCTSTR lpReplace;
	short cchLen;
  LPEDITITEM lpEditItem;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(short)wParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;

		if (nItem<-1)
			return 0;

		if (nItem==-1)
			nItem=lpMultiItemEditStruct->iActiveItem;
	  // Get struct EDITITEM data
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
		lpReplace=(LPCTSTR)lParam;
		DeleteInvert(hWnd,lpEditItem);
		cchLen=strlen(lpReplace);
		InsertString(hWnd,lpEditItem,lpReplace,cchLen);
		SetCaretCoordinate(hWnd,lpEditItem);
		AdjustCaretPosition(hWnd);
//		InvalidateRect(hWnd,NULL,TRUE);
		SetPDACaret(hWnd);
		return 0;
}

/************************************************************************************/
// Process Edit Message -- EM_SCROLL
// An application sends an EM_SCROLL message to scroll the text vertically in a
// multiline edit control. This message is equivalent to sending a WM_VSCROLL
// message to the edit control. 
// Input:
// wParam = (WPARAM) (INT) nScroll;    // scroll action
// lParam = 0;                         // not used; must be zero 
 
// Parameters
// nScroll 
//    Value of wParam. Specifies the action the scroll bar is to take. This parameter 
//    may be one of the following values: Value Meaning 
//			SB_LINEDOWN Scrolls down one line. 
//			SB_LINEUP Scrolls up one line. 
//			SB_PAGEDOWN Scrolls down one page. 
//			SB_PAGEUP Scrolls up one page. 

// Return Values
// If the message is successful, the high-order word of the return value is TRUE, 
// and the low-order word is the number of lines that the command scrolls. The number 
// returned may not be the same as the actual number of lines scrolled if the scrolling 
// moves to the beginning or the end of the text. If the nScroll parameter specifies an
// invalid value, the return value is FALSE. 
/************************************************************************************/
static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LRESULT lResult;
	short nScrollLine;
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
    nScrollLine=DoVScrollWindow(hWnd,wParam,lParam);
		lResult=MAKELRESULT(nScrollLine,1);
		return lResult;
}
/************************************************************************************/
// Process Edit Message -- EM_SCROLLCARET
// An application sends an EM_SCROLLCARET message to scroll the caret into view in 
// an edit control. 
// Input :
// wParam = 0 ;    // not used now; reserved for future use; must be zero 
// lParam = 0 ;    // not used now; reserved for future use;  must be zero 
 
// Return Values
// The return value is nonzero if the message is sent to an edit control. 
/************************************************************************************/
static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
 LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;
//		iActiveItem=lpMultiItemEditStruct->iActiveItem;
//		SendMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_GETCARETPOS,(WPARAM)&nCaretLine,(LPARAM)&xPos);
//    nCaretLine+=lpMultiItemEditStruct->nItemStartLineList[iActiveItem];
	  AdjustCaretPosition(hWnd);
		return TRUE;
}
/************************************************************************************/
// Process Edit Message -- EN_SETFOCUS
// the child window be set focus , then will send message EN_SETFOCUS
// Input :

// wParam  (HIWORD)NoticeCode , (LOWORD) identifier
// lParam = (LPARAM)hActiveWnd; // the handle of the child window
// Return Value
// no Return code
/************************************************************************************/
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
/************************************************************************************/
// Process Edit Message -- EM_SETMODIFY
// An application sends an EM_SETMODIFY message to set or clear the modification flag 
// for an edit control. The modification flag indicates whether the text within the 
// edit control has been modified. It is automatically set whenever the user changes
// the text. An EM_GETMODIFY message can be sent to retrieve the value of the modification flag. 
// Input:
// wParam = (WPARAM) (UINT) fModified;    // modification flag
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// fModified
//		Value of wParam. Specifies the new value for the modification flag. A value of TRUE 
//		indicates the text has been modified, and a value of FALSE indicates it has not been modified.
// nItem
//		the special item of the multiple item edit . if the nItem is -1 ,then the all items of the 
//    control will be set
// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	UINT  fModified;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

    fModified=(UINT)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<-1)
			return 0;
		if (nItem==-1)
		{
			for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
			{
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     		lpEditItem->fModified=fModified;
			}
		}
		else
		{
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		    lpEditItem->fModified=fModified;
		}
		return 0;
}
/************************************************************************************/
// Process Edit Message -- EM_SETPASSWORDCHAR
// An application sends an EM_SETPASSWORDCHAR message to set or remove the password 
// character for a single-line edit control when the user types text. When a password 
//character is set, that character is displayed in place of each character the user types.
// Input :
// wParam = (WPARAM) (UINT) ch;    // character to display 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// ch 
//     Value of wParam. Specifies the character to be displayed in place of the 
//     character typed by the user. If this parameter is zero, the characters 
//		 typed by the user are displayed.
// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	DWORD dwStyle;
	TCHAR ch;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		ch=(TCHAR)wParam;
		nItem=(short)lParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

		dwStyle=lpEditItem->dwStyle;
		if (dwStyle&ES_MULTILINE)
			return 0;
    return SetPasswordChar(hWnd,lpEditItem,ch);
}

/************************************************************************************/
// Process Edit Message -- EM_SETREADONLY
// An application sends an EM_SETREADONLY message to set or remove the read-only 
// style (ES_READONLY) of an edit control. 
// Input:
// wParam = (WPARAM) (BOOL) fReadOnly;  // read-only flag 
// lParam = (short)nItem; // the Item Edit of the multiple Item Edit
 
// Parameters
// fReadOnly 
//		Value of wParam. Specifies whether to set or remove the ES_READONLY style. A value 
//		of TRUE sets the ES_READONLY style; a value of FALSE removes the ES_READONLY style. 
// Return Values
// If the operation succeeds, the return value is nonzero; otherwise, it is zero. 
/************************************************************************************/
static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;
	BOOL fReadOnly;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		fReadOnly=(BOOL)wParam;
		nItem=(short)lParam;  
		// Maybe Add Function .nItem == -1, then set all item are Read Only
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;
		 lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		if (fReadOnly)
		{
			lpEditItem->dwStyle|=ES_READONLY;
		}
		else
		{
			lpEditItem->dwStyle&=~ES_READONLY;
		}
		return TRUE;
}
/************************************************************************************/
// Process Edit Message -- WM_COPY
// An application sends the WM_COPY message to an edit control or combo box to copy 
// the current selection to special buffer
// Input:
// wParam = 0;                          // not used; must be zero 
// lParam = 0;                          // not used; must be zero 
 
// Return Values
// the number of the characters to be copied
/************************************************************************************/
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

    return CopyToClipboard(hWnd,lpEditItem);
}
/************************************************************************************/
// Process Edit Message -- WM_PASTE
// An application sends a WM_PASTE message to an edit control or combo box to copy
// the current content of the clipboard to the edit control at the current caret position.
// Input:
//wParam = 0; // not used; must be zero
//lParam = 0; // not used; must be zero

// Return Values
// This message does not return a value.
/************************************************************************************/
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

    PasteFromClipboard(hWnd,lpEditItem);
		AdjustCaretPosition(hWnd);
		SetPDACaret(hWnd);
    return 0;
}
/************************************************************************************/
// Process Edit Message -- WM_CUT
// An application sends a WM_CUT message to an edit control or combo box to delete 
// (cut) the current selection, if any, in the edit control and copy the deleted text 
// to the clipboard. 
// Input:
// wParam = 0; // not used; must be zero 
// lParam = 0; // not used; must be zero 

// Return Values
// This message does not return a value.
/************************************************************************************/
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		DoCopy(hWnd,0,0);
		DoClear(hWnd,0,0);
		return TRUE;
}

/************************************************************************************/
// Process Edit Message --WM_CLEAR
// An application sends a WM_CLEAR message to an edit control or combo box to delete 
// (clear) the current selection, if any, from the edit control. 
// Input :
// wParam = 0; // not used; must be zero 
// lParam = 0; // not used; must be zero 

// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

		DeleteInvert(hWnd,lpEditItem);
		SetCaretCoordinate(hWnd,lpEditItem);
		AdjustCaretPosition(hWnd);
		SetPDACaret(hWnd);
		return TRUE;
}
/************************************************************************************/
// Process Message --- EM_EMPTY
// wParam = (short)nItem;
// lParam = 0;
/************************************************************************************/
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  nItem=(short)wParam;
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<-1)
			return 0;
	  if (nItem==-1)
	  {
		  for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
			{
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  ClearEditItemStruct(lpEditItem);

				SetWindowRect(hWnd,lpEditItem,FALSE);
			}
		    lpMultiItemEditStruct->iActiveItem=0;
			SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE);
	  }
	  else
	  {
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  ClearEditItemStruct(lpEditItem);
		  SetWindowRect(hWnd,lpEditItem,FALSE);
	  }
    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
	SetPDACaretPosition(hWnd,lpEditItem,0,0);
    AdjustCaretPosition(hWnd);
	SetPDACaret(hWnd);
    SetFocus(hWnd);
//    ShowCaret(hWnd);
//		InvalidateRect(hWnd,NULL,TRUE);
		return 0;
}
/*****************************************************************/
/*****************************************************************/
static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
    for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    {
       DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[nItem],PE_SHOWCLIENT);
    }
		ClearBottomBlankArea(hWnd);
    return 0;
}
/*****************************************************************/

/*****************************************************************/
// print the edit context to the in hdc
/*****************************************************************/
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;		
	LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	short nItem;
    short nDisplayLine,nDisplayx;//,iStartPos;

	hdc=(HDC)wParam;
    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;
	
   nDisplayLine=lpMultiItemEditStruct->nDisplayLine;
   nDisplayx=lpMultiItemEditStruct->xDisplayPos;
   lpMultiItemEditStruct->nDisplayLine=0;
   lpMultiItemEditStruct->xDisplayPos=0;
	for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    {
       DrawPDAEditControl(hWnd,hdc,&lpMultiItemEditStruct->lpItemList[nItem],PE_SHOWALL);
    }
   lpMultiItemEditStruct->nDisplayLine=nDisplayLine;
   lpMultiItemEditStruct->xDisplayPos=nDisplayx;
   return 0;
}
/*****************************************************************/

/*****************************************************************/
/*****************************************************************/
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  RECT rectNew;
  short i;
  short xPos,nLine;

       lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
       if (lpMultiItemEditStruct==NULL) return 0;

       GetClientRect(hWnd,&rectNew);
       for (i=0;i<lpMultiItemEditStruct->nItemNum;i++)
       {
          lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
          lpEditItem->rect.right=rectNew.right;
          // Add by Jami 2000-06-02  -- Must Reset The Item Height
          SetWindowRect(hWnd,lpEditItem,FALSE);
//          TotalLine(
       }
       
	   lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
       GetCoordinate(hWnd,lpEditItem,lpEditItem->lpCaretPosition,&xPos,&nLine,lpEditItem->dwStyle&ES_MULTILINE);
	   lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;
	   lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
	   AdjustCaretPosition(hWnd);
	   InvalidateRect(hWnd,NULL,TRUE);
       return 0;
}
/*****************************************************************/
//  process WM_BUTTONDOWN Message

//  fwKeys=wParam;
//	xPos=LOWORD(lParam);
//	yPos=HIWORD(lParam);
/*****************************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem,nOldActiveItem;
//	DWORD fwKeys;
	short xPos,yPos;


			// Get Virtal Key with LButton Down
//	    fwKeys=wParam;
			// Get LButton down Position
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);

		  // Get struct EDITITEM data
		  lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
		  if(lpMultiItemEditStruct==NULL) return FALSE;
			if (lpMultiItemEditStruct->nItemNum==0) return 0;

		  nItem=GetItem(hWnd,xPos,yPos);
		  if (nItem==-1)
			 return 0;
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
		  ClearInvert(hWnd,lpEditItem,TRUE);

		  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		  if (lpEditItem->dwStyle&WS_DISABLED)
			  return 0;
		  nOldActiveItem = lpMultiItemEditStruct->iActiveItem;
		  lpMultiItemEditStruct->iActiveItem=nItem;
		  yPos-=lpEditItem->iStartPos;
			
			if (lpEditItem->dwStyle&ES_CALLBACK)
			{
				if (InCallBackPos(hWnd,lpEditItem,xPos,yPos)==TRUE)
				{
					if (lpEditItem->lpCallBack!=NULL)
						lpEditItem->lpCallBack(hWnd,nItem);
					return 0;
				}
			}
			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);

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
/*****************************************************************/
// process WM_MOUSEMOVE Message

// xPos=LOWORD(lParam)
// yPos=HIWORD(lParam)
/*****************************************************************/
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
	short xPos,yPos;
//  short nItem;

	// if the mouse be capture by this window
	if (GetCapture()==hWnd)
	{

			// Get LButton down Position
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);


		  // Get struct EDITITEM data
		  lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if(lpMultiItemEditStruct==NULL) return FALSE;
	    if (lpMultiItemEditStruct->nItemNum==0) return 0;

      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
      yPos-=lpEditItem->iStartPos;


			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);

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
/*****************************************************************/
// process WM_LBUTTONUP Message

// xPos=LOWORD(lParam)
// yPos=HIWORD(lParam)
/*****************************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture
	SetCapture(NULL);
  return 0;
}
/*****************************************************************/
// process message -- WM_SETFOCUS
// The WM_SETFOCUS message is sent to a window after it has gained the keyboard focus.
// Input :
// hwndLoseFocus = (HWND) wParam; // handle to window losing focus

// Parameters
// hwndLoseFocus
//		Value of wParam. Handle to the window that has lost the keyboard focus (may be NULL).
// Return Values
// An application should return zero if it processes this message.
/*****************************************************************/
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	HWND hParentWnd;
	DWORD dwStyle;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
//  short nItem;

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

/*****************************************************************/
// process message -- WM_KILLFOCUS
// The WM_KILLFOCUS message is sent to a window immediately before it 
// loses the keyboard focus.
// Input :
// hwndGetFocus = (HWND) wParam; // handle to window receiving focus 
 
// Parameters
// hwndGetFocus 
//    Value of wParam. Handle to the window that receives the keyboard focus (may be NULL). 
// Return Values
// An application should return zero if it processes this message. 
/*****************************************************************/
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return 0;
      if (lpMultiItemEditStruct->nItemNum==0) return 0;
	    // get struct EDITITEM data
		  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

      ClearInvert(hWnd,lpEditItem,TRUE);
	    DeletePDACaret(hWnd);
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);
	    return 0;
}

/*********************************************************************/
// process WM_CHAR Message
// Input :  hWnd --- handle of the window
// chCharCode = (TCHAR)wParam
/*********************************************************************/
static LRESULT DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
	DWORD dwStyle;
	BOOL bResult;
	DWORD dwShowFlag;
	LPTSTR lpShowAddress;
	TCHAR chCharCode;
	short nShowLine,nShowPos;
	HDC hdc;
	short xPos,nLine;
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
      // redraw the window
			hdc=GetDC(hWnd);
	        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,nShowPos,nShowLine,0,dwShowFlag);
			ReleaseDC(hWnd,hdc);
			// reset thw caret
		}
		SetPDACaret(hWnd);
    return 1;
}
/*********************************************************************/
// Process WM_KEYDOWM Message
// input :
//		nVirtKey = (int) wParam;    // virtual-key code
//		lKeyData = lParam;          // key data
/*********************************************************************/
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
	int nVirtKey ;
//	DWORD lKeyData ;
  short nScrollLine,nWindowLine,iHorzPos;


     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
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
					short iShiftState,iCtrlState;
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
static BOOL MoveCaret(HWND hWnd,short nScrollLine,short iHorzPos)
{
//  HWND hActiveItemWnd;
  short nCaretLine,xPos,nItemCaretLine;
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPEDITITEM lpEditItem;
  short nTextHeight;
  short iActiveItem;
//  RECT rcClientRect;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;


	lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

	if (nScrollLine==0)
	{
		if (iHorzPos!=MOTIONLESS)
		{
			HorzMoveCaret(hWnd,lpEditItem,iHorzPos);
		}
		return TRUE;
	}
    xPos=lpEditItem->nCaretx;
    nCaretLine=lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->iActiveItem]+lpEditItem->nCaretLine;

    nCaretLine+=nScrollLine;


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

    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
    ClearInvert(hWnd,lpEditItem,TRUE);

    lpMultiItemEditStruct->iActiveItem=iActiveItem;

    nTextHeight=GetTextHeight(hWnd);
	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
		SetPDACaretPosition(hWnd,lpEditItem,xPos,(short)(nItemCaretLine*nTextHeight));
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
	SendNotificationMsg(hWnd,EN_ACTIVEITEMCHANGE);

//    PostMessage(lpMultiItemEditStruct->hItemWndList[iActiveItem],EM_SETCARETPOS,nItemCaretLine,xPos);
//    SetFocus(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
//   	ShowCaret(lpMultiItemEditStruct->hItemWndList[iActiveItem]);
    return TRUE;
}
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (!(dwStyle&WS_VSCROLL)) return 0;
    wParam++;
    lParam++;
		return Edit_GetScrollPos(hWnd,SB_VERT);
}
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 short nStart , nEnd;
	 LPDWORD lpdwStart,lpdwEnd;
	 LRESULT lResult;



    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
	  // Get struct EDITITEM data

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() ); 
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

		lpdwStart=(LPDWORD)wParam;
		lpdwEnd=(LPDWORD)lParam;
		if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
			return -1;
		nStart=lpEditItem->lpInvertStart-lpEditItem->lpPDAEditBuffer;
		nEnd=lpEditItem->lpInvertEnd-lpEditItem->lpPDAEditBuffer;
		if (lpdwStart)
			*lpdwStart=(DWORD)nStart;
		if (lpdwEnd)
			*lpdwEnd=(DWORD)nEnd;
		lResult=MAKELRESULT(nStart,nEnd);
		return lResult;
}

static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
	 short nStart , nEnd;



    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;
	  if (lpMultiItemEditStruct->nItemNum==0) return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

		nStart=(short)wParam;
		nEnd=(short)lParam;

    return SetSel(hWnd,lpEditItem,nStart,nEnd);
}

static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPCTSTR lpsz;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;

    lpsz= (LPCTSTR)lParam; // address of window-text string
    lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];

    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)
		return FALSE;
	AdjustCaretPosition(hWnd);
	SetPDACaret(hWnd);
	InvalidateRect(hWnd,NULL,TRUE);
	return TRUE;
}

static LRESULT DoSetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPCTSTR lpsz;
   short nItem;


    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return FALSE;

		nItem=(short)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
    lpsz= (LPCTSTR)lParam; // address of window-text string
		if (nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
		if (nItem<0)
			return 0;
    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];

//    return SetText(hWnd,lpEditItem,lpsz);
    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)
		return FALSE;
	AdjustCaretPosition(hWnd);
	SetPDACaret(hWnd);
	InvalidateRect(hWnd,NULL,TRUE);
	return TRUE;
}

/************************************************************************************/
// process Message -WM_GETTEXT
// An application sends a WM_GETTEXT message to copy the text that corresponds to a
// window into a buffer provided by the caller.
// Input:
//			cchTextMax=(short)(wParam); // number of characters to copy
//      lParam = (LPARAM) lpszText;     // address of buffer for text

// Parameters
// cchTextMax
//		Value of wParam. Specifies the maximum number of characters to be copied,
//		including the terminating null character.
// lpszText
//		Value of lParam. Pointer to the buffer that is to receive the text.
// Return Values
// The return value is the number of characters copied.
/************************************************************************************/
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPTSTR lpsz;
   short cMaxlen;
//   short nItem;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
     if (lpMultiItemEditStruct==NULL) return 0;


			lpsz= (LPTSTR)lParam; // address of window-text string
			cMaxlen=(short)(wParam);

	      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
			if (cMaxlen<lpEditItem->cbEditLen)
				return FALSE;
		  GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(short)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
			return lpEditItem->cbEditLen;
}
/************************************************************************************/
// process Message -EM_GETITEMTEXT
// An application sends a WM_GETTEXT message to copy the text that corresponds to a
// window into a buffer provided by the caller.
// Input:
//			cchTextMax=(short)HIWORD(wParam); // number of characters to copy
//      nItem=(short)LOWORD(wParam);   // the Item Edit of the multiple Item Edit
//      lParam = (LPARAM) lpszText;     // address of buffer for text

// Parameters
// cchTextMax
//		Value of wParam. Specifies the maximum number of characters to be copied,
//		including the terminating null character.
// lpszText
//		Value of lParam. Pointer to the buffer that is to receive the text.
// Return Values
// The return value is the number of characters copied.
/************************************************************************************/
static LRESULT DoGetItemText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPTSTR lpsz;
   short cMaxlen;
   short nItem;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
     if (lpMultiItemEditStruct==NULL) return 0;


#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
		lpsz= (LPTSTR)lParam; // address of window-text string
		cMaxlen=(short)HIWORD(wParam);
		nItem=(short)LOWORD(wParam);

		  if (nItem>=lpMultiItemEditStruct->nItemNum)
			  return 0;
		  if (nItem<0)
			  return 0;

      lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
			if (cMaxlen<lpEditItem->cbEditLen)
				return FALSE;
		  GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(short)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
			return lpEditItem->cbEditLen;
}
/************************************************************************************/
// process Message -WM_GETTEXTLENGTH
// An application sends a WM_GETTEXTLENGTH message to determine the length,
// in characters, of the text associated with a window. The length does
// not include the terminating null character.

// wParam = (short)nItem;  // the Item Edit of the multiple Item Edit
// lParam = 0; // not used; must be zero

 /************************************************************************************/
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return 0;

 	  // Get struct EDITITEM data
      lpEditItem=&lpMultiItemEditStruct->lpItemList[lpMultiItemEditStruct->iActiveItem];
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;
}
/************************************************************************************/
// process Message -EM_GETITEMTEXTLENGTH
// An application sends a EM_GETITEMTEXTLENGTH message to determine the length,
// in characters, of the text associated with a window. The length does
// not include the terminating null character.

// wParam = (short)nItem;  // the Item Edit of the multiple Item Edit
// lParam = 0; // not used; must be zero

 /************************************************************************************/
static LRESULT DoGetItemTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   short nItem;

      lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
      if (lpMultiItemEditStruct==NULL) return 0;

      nItem=(short)wParam;

		  if (nItem>=lpMultiItemEditStruct->nItemNum)
			  return 0;
		  if (nItem<0)
			  return 0;

			// Get struct EDITITEM data
      lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;
}
/************************************************************************************/
static LRESULT DoSetItemCallBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   short nItem;
   EDITCALLBACK *lpCallBack;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
     if (lpMultiItemEditStruct==NULL) return 0;


#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
	  lpCallBack= (EDITCALLBACK *)lParam; // address of window-text string
      nItem=(short)LOWORD(wParam);

	  if (nItem>=lpMultiItemEditStruct->nItemNum)
		  return 0;
	  if (nItem<0)
		  return 0;

	  lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
	  lpEditItem->lpCallBack=lpCallBack;
	  return TRUE;
}
/************************************************************************************/
static LRESULT DoInsertItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   LPEDITITEMPROPERTY lpEditItemProperty;
   short nItem,i;
   short iItemStartPos;
   void *lpTemp;
   short nTextHeigth,nClientLine;
   RECT ClientRect;
   char *pSrc,*pObj;
   short iMoveLen;

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);

     nItem=(short)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
     lpEditItemProperty=(LPEDITITEMPROPERTY)lParam;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
     if (lpMultiItemEditStruct==NULL) return -1;

     lpMultiItemEditStruct->nItemNum++;

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

     if (lpMultiItemEditStruct->nItemStartLineList)
        lpTemp=(void *)realloc(lpMultiItemEditStruct->nItemStartLineList,(lpMultiItemEditStruct->nItemNum+1)*sizeof(short));
     else
        lpTemp=(void *)malloc((lpMultiItemEditStruct->nItemNum+1)*sizeof(short));
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
     {
        nItem=lpMultiItemEditStruct->nItemNum-1;
        lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
        iItemStartPos=lpMultiItemEditStruct->nContextBottomPos;
     }
     else
     {
         lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
         iItemStartPos=lpEditItem->iStartPos;
         pSrc=(char *)lpEditItem;
         pObj=pSrc+sizeof(EDITITEM);
         iMoveLen=(lpMultiItemEditStruct->nItemNum-nItem-1)*sizeof(EDITITEM);
         memmove(pObj,pSrc,iMoveLen);

         pSrc=(char *)&lpMultiItemEditStruct->nItemStartLineList[nItem];
         pObj=pSrc+sizeof(short);
         iMoveLen=(lpMultiItemEditStruct->nItemNum-nItem)*sizeof(short);
         memmove(pObj,pSrc,iMoveLen);
     }

     SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos);
     lpMultiItemEditStruct->nItemStartLineList[nItem]=iItemStartPos/nTextHeigth;

     iItemStartPos+=(short)lpEditItem->rect.bottom;
     for (i=nItem+1;i<lpMultiItemEditStruct->nItemNum;i++)
     {
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(short)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
//     AdjustCaretPosition(hWnd);
		 SetPDACaretPosition(hWnd,lpEditItem,0,0);
		 SetPDACaret(hWnd);
     InvalidateRect(hWnd,NULL,TRUE);
     return 0;
}
static LRESULT DoSetItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   EDITITEM OldEditItem;
   LPEDITITEMPROPERTY lpEditItemProperty;
   short nItem,i;
   short iItemStartPos;
//   void *lpTemp;
   short nTextHeigth,nClientLine;
   RECT ClientRect;
//   char *pSrc,*pObj;
//   short iMoveLen;

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);

     nItem=(short)wParam;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
     lpEditItemProperty=(LPEDITITEMPROPERTY)lParam;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
     if (lpMultiItemEditStruct==NULL) return FALSE;

     if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<0)
        return FALSE;
     lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     OldEditItem=*lpEditItem;
     iItemStartPos=lpEditItem->iStartPos;
     if (SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos)==FALSE)
     {
         *lpEditItem=OldEditItem;
         return FALSE;
     }

     if (OldEditItem.lpPDAControlBuffer)
       free(OldEditItem.lpPDAControlBuffer);
     if (OldEditItem.lpPDASaveEditBuffer)
       free(OldEditItem.lpPDASaveEditBuffer);
     if (OldEditItem.lpFormat)
       free(OldEditItem.lpFormat);

     iItemStartPos+=(short)lpEditItem->rect.bottom;
     for (i=nItem+1;i<lpMultiItemEditStruct->nItemNum;i++)
     {
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(short)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
     AdjustCaretPosition(hWnd);
     InvalidateRect(hWnd,NULL,TRUE);
     return TRUE;
}

static LRESULT DoDeleteItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
   LPEDITITEM lpEditItem;
   EDITITEM OldEditItem;
   short nItem,i;
   short iItemStartPos;
//   void *lpTemp;
   short nTextHeigth,nClientLine;
   RECT ClientRect;
//   char *pSrc,*pObj;
//   short iMoveLen;

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);

     nItem=(short)wParam;

     lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
     if (lpMultiItemEditStruct==NULL) return FALSE;

     if (nItem>=lpMultiItemEditStruct->nItemNum||nItem<0)
        return FALSE;
     OldEditItem=lpMultiItemEditStruct->lpItemList[nItem];
     iItemStartPos=(short)OldEditItem.iStartPos;
     lpMultiItemEditStruct->nItemNum--;
     for (i=nItem;i<lpMultiItemEditStruct->nItemNum;i++)
     {
        lpMultiItemEditStruct->lpItemList[i]=lpMultiItemEditStruct->lpItemList[i+1];
        lpEditItem=&lpMultiItemEditStruct->lpItemList[i];
        lpEditItem->iStartPos=iItemStartPos;
        lpMultiItemEditStruct->nItemStartLineList[i]=iItemStartPos/nTextHeigth;
        iItemStartPos+=(short)lpEditItem->rect.bottom;
     }
     lpMultiItemEditStruct->nContextBottomPos=iItemStartPos;
     lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum]=iItemStartPos/nTextHeigth;
     nClientLine=ClientRect.bottom/nTextHeigth;
     SetMIEditVScrollRange(hWnd,lpMultiItemEditStruct->nItemStartLineList[lpMultiItemEditStruct->nItemNum],nClientLine);
     InvalidateRect(hWnd,NULL,TRUE);
/*     lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
     OldEditItem=*lpEditItem;
     iItemStartPos=lpEditItem->iStartPos;
     if (SetEditItem(hWnd,lpEditItem,lpEditItemProperty,iItemStartPos)==FALSE)
     {
         *lpEditItem=OldEditItem;
         return FALSE;
     }
*/
     if (OldEditItem.lpPDAControlBuffer)
       free(OldEditItem.lpPDAControlBuffer);
     if (OldEditItem.lpPDASaveEditBuffer)
       free(OldEditItem.lpPDASaveEditBuffer);
     if (OldEditItem.lpFormat)
       free(OldEditItem.lpFormat);

     return TRUE;
}
static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
//	COLORREF cl_NewColor;
	LPCTLCOLORSTRUCT lpCtlColor;
		
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;

		 lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
		 if (lpMultiItemEditStruct==NULL) 
			 return FALSE;

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {
			 lpMultiItemEditStruct->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {
				lpMultiItemEditStruct->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {
				lpMultiItemEditStruct->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {
				lpMultiItemEditStruct->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpMultiItemEditStruct->cl_DisableText = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpMultiItemEditStruct->cl_DisableBkColor = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 {
				lpMultiItemEditStruct->cl_ReadOnly = lpCtlColor->cl_ReadOnly;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 {
				lpMultiItemEditStruct->cl_ReadOnlyBk  = lpCtlColor->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {
				lpMultiItemEditStruct->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {
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
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
		
    LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
	LPCTLCOLORSTRUCT lpCtlColor;

		 lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
		 if (lpMultiItemEditStruct==NULL) 
			return FALSE;

		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {
			 lpCtlColor->cl_Text = lpMultiItemEditStruct->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {
				lpCtlColor->cl_TextBk = lpMultiItemEditStruct->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {
				lpCtlColor->cl_Selection = lpMultiItemEditStruct->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {
				lpCtlColor->cl_SelectionBk = lpMultiItemEditStruct->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpCtlColor->cl_Disable= lpMultiItemEditStruct->cl_DisableText ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpCtlColor->cl_DisableBk = lpMultiItemEditStruct->cl_DisableBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 {
				lpCtlColor->cl_ReadOnly= lpMultiItemEditStruct->cl_ReadOnly ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 {
				lpCtlColor->cl_ReadOnlyBk = lpMultiItemEditStruct->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {
				lpCtlColor->cl_Title= lpMultiItemEditStruct->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {
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
		return -1;
}


static BOOL SetEditItem(HWND hWnd,LPEDITITEM lpEditItem,LPEDITITEMPROPERTY lpEditItemProperty,short iItemStartPos)
{
   RECT ClientRect;
   short nTextHeigth;
   short nFormatLen;
   LPTSTR lpTitle,lpItemText,lpFormat;

// !!! Add By Jami chen in 2003.07.31
#ifdef _MAPPOINTER
	lpTitle = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpTitle, GetCallerProcess() ); 
	lpItemText = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpItemText, GetCallerProcess() ); 
	lpFormat = (LPTSTR)MapPtrToProcess( (LPVOID)lpEditItemProperty->lpFormat, GetCallerProcess() ); 
#endif
// !!! Add End By Jami chen in 2003.07.31

      nTextHeigth=GetTextHeight(hWnd);
      GetClientRect(hWnd,&ClientRect);
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
     lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen);
	 memset(lpEditItem->lpPDAControlBuffer,0,lpEditItem->cbControlBufferLen);

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
		         SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(short)(lpEditItem->cbEditLen+1));
	       }
         else
         {
// !!! Modified By Jami chen in 2003.07.31
//	          SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItemProperty->lpItemText,(short)(lpEditItem->cbEditLen+1));
	          SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpItemText,(short)(lpEditItem->cbEditLen+1));
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
//		         GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItemProperty->lpItemText,(short)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
		         GetEditText(lpEditItem->lpPDAEditBuffer,lpEditItem->lpPDASaveEditBuffer,(short)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
		       }
//	       SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpEditItemProperty->lpItemText,(short)(lpEditItem->cbEditLen+1));
     }
     else
     {
	       lpEditItem->cbEditLen=0;
	       SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(short)(lpEditItem->cbEditLen+1));
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

     lpEditItem->iStartPos=iItemStartPos;
     lpEditItem->rect.top=0;
     lpEditItem->rect.left=0;
     lpEditItem->rect.right=ClientRect.right;
     lpEditItem->nTotalLine=GetTotalLine(hWnd,lpEditItem);
     lpEditItem->rect.bottom=lpEditItem->rect.top+lpEditItem->nTotalLine*nTextHeigth;
	 lpEditItem->lpCallBack=NULL;
     return TRUE;
//   SetWindowPos(hWnd,0,0,0,ClientRect.right,ClientRect.top+nTextHeight*nTotalLine,SWP_NOMOVE|SWP_NOREDRAW|SWP_NOZORDER);

//   GetClientRect(hItemWnd,&rect);
//   lpMultiItemEditStruct->hItemWndList[i]=hItemWnd;
//     lpMultiItemEditStruct->nItemStartLineList[i]=iNextWndPos/nTextHeigth;
//   lpMultiItemEditStruct->nItemStartPosList[i]=iNextWndPos;
//     iNextWndPos+=(short)lpEditItem->rect.bottom;
}
/************************************************************************************/
short GetItem(HWND hWnd,short xPos,short yPos)
{
 LPMULTIITEMEDITSTRUCT   lpMultiItemEditStruct;
 short nItem;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if(lpMultiItemEditStruct==NULL) return -1;
    if (lpMultiItemEditStruct->nItemNum==0) return -1;

    for (nItem=0;nItem<lpMultiItemEditStruct->nItemNum;nItem++)
    {
      if (yPos<lpMultiItemEditStruct->lpItemList[nItem].iStartPos)
        return nItem-1;
    }
    return nItem-1;

}

static LRESULT DoGetActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	 LPMULTIITEMEDITSTRUCT   lpMultiItemEditStruct;

		lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
	    if(lpMultiItemEditStruct==NULL) 
			return -1;
		if (lpMultiItemEditStruct->nItemNum==0) 
			return -1;
		return lpMultiItemEditStruct->iActiveItem;
}

static LRESULT DoGetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(short)wParam;
		if (nItem < 0 || nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
	    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		return lpEditItem->dwStyle;
}
static LRESULT DoSetItemStyle(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPMULTIITEMEDITSTRUCT lpMultiItemEditStruct;
  LPEDITITEM lpEditItem;
  short nItem;
  DWORD dwStyle;
  HDC hdc;

    lpMultiItemEditStruct=(LPMULTIITEMEDITSTRUCT)GetWindowLong(hWnd,0);
    if (lpMultiItemEditStruct==NULL) return 0;

		nItem=(short)wParam;
		dwStyle = (DWORD)lParam;
		if (nItem < 0 || nItem>=lpMultiItemEditStruct->nItemNum)
			return 0;
	    lpEditItem=&lpMultiItemEditStruct->lpItemList[nItem];
		lpEditItem->dwStyle = dwStyle;
		hdc = GetDC(hWnd);
        DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWALL);
		ReleaseDC(hWnd,hdc);
		return 0;
}

