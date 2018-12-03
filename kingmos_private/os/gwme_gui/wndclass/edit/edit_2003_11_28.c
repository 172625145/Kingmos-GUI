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
#include <gwmesrv.h>


// define const 
#define EDIT classEDIT
static const char classEDIT[]="Edit";

#define DOT '.'

#define _MAPPOINTER

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
static short DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem);

static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaint(HWND hWnd,HDC hdc);
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);


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

static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam);
/*
    This function is register a PDAEdit Class to System.
	This function must be Transfered once in the start time
*/
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

/*
    This function is PDAEdit Windows procedure
*/
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
    case WM_SIZE:
      return DoSize(hWnd,wParam,lParam);
		case WM_PAINT:
			// Paint this window
			hdc = BeginPaint(hWnd, &ps);  // Get hdc
      DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);  //release hdc
			break;
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
    case EM_GETCARETPOS:
      DoGetCaretPosition(hWnd,wParam,lParam);
    case EM_SETCARETPOS:
      DoSetCaretPosition(hWnd,wParam,lParam);
      break;
    case MIEM_SETXDISPLAYPOS:
      DoSetxDisplayPos(hWnd,wParam,lParam);
      break;
    case WM_VSCROLL:
      DoVScrollWindow(hWnd,wParam,lParam);
      break;
    case WM_HSCROLL:
      DoHScrollWindow(hWnd,wParam,lParam);
      break;
    case EM_CHARFROMPOS:
      return DoCharFromPos(hWnd,wParam,lParam);
    case EM_GETFIRSTVISIBLELINE:
      return DoGetFirstVisibleLine(hWnd);
		case EM_GETTHUMB:
			return DoGetThumb(hWnd,wParam,lParam);
		case EM_GETLIMITTEXT:
			return DoGetLimitText(hWnd,wParam,lParam);
		case EM_GETLINE:
			return DoGetLine(hWnd,wParam,lParam);
		case EM_GETLINECOUNT:
			return DoGetLineCount(hWnd,wParam,lParam);
		case EM_GETMODIFY:
			return DoGetModify(hWnd,wParam,lParam);
		case EM_GETPASSWORDCHAR:
			return DoGetPasswordChar(hWnd,wParam,lParam);	
		case EM_GETSEL:
			return DoGetSel(hWnd,wParam,lParam);	
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

		case EM_SEARCHSTRING:
			return DoSearchString(hWnd,wParam,lParam);
// Color Control 
//		case EM_SETCOLOR:
//			return DoSetColor(hWnd,wParam,lParam);
//		case EM_GETCOLOR:
//			return DoGetColor(hWnd,wParam,lParam);
		case WM_SETCTLCOLOR	:
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	:
			return DoGetColor(hWnd,wParam,lParam);


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
      return DoGetTextLength(hWnd,wParam,lParam);
	case WM_PRINTCLIENT:
	  DoPrintClient(hWnd,wParam,lParam);
	  break;
	case WM_STYLECHANGED:
		return DoStyleChanged(hWnd,wParam,lParam);
    default:
			// default procedure
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

/**************************************************/
// the window will be create by class "PDAEdit"
// so must initial some variable
// hWnd --- the handle of the window
//lpcs = (LPCREATESTRUCT) lParam; // structure with creation data
/*************************************************/
static LRESULT InitialPDAEdit(HWND hWnd,LPARAM lParam)  // initial PDAEdit State data
{
    LPEDITITEM lpEditItem;
	LPCREATESTRUCT lpcs;
	LPPDAEDITPROPERTY lpPDAEditProperty;
  DWORD dwStyle;
  short nFormatLen;
	
		lpcs = (LPCREATESTRUCT) lParam; // structure with creation data  

		// Get Edit Window struct PDAEDITPROPERTY data
		lpPDAEditProperty=(LPPDAEDITPROPERTY)lpcs->lpCreateParams;
    dwStyle=lpcs->style;

    dwStyle&=~ES_MULTIITEMEDIT;

		SetWindowLong(hWnd,0,NULL);

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
		{
			// get the text limit
			if (lpPDAEditProperty->cLimited!=0)
				lpEditItem->cbEditLimitLen=lpPDAEditProperty->cLimited;
			else
				lpEditItem->cbEditLimitLen=INITIALEDITSIZE;

			// get the edit title text
			if (lpPDAEditProperty->lpTitle!=NULL)
				lpEditItem->cbTitleLen=strlen(lpPDAEditProperty->lpTitle);
			else
				lpEditItem->cbTitleLen=0;
		}
		else
		{
			lpEditItem->cbEditLimitLen=INITIALEDITSIZE;
			lpEditItem->cbTitleLen=0;
		}

		lpEditItem->cbEditBufferLen=lpEditItem->cbEditLimitLen+1;

		lpEditItem->cbControlBufferLen=lpEditItem->cbEditBufferLen+lpEditItem->cbTitleLen;
		// allocate memory
		lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen);

		if (lpEditItem->lpPDAControlBuffer==NULL)
		{  // allocate failare then free struct EDITITEM and return FALSe
			lpEditItem->cbEditBufferLen=0;
			lpEditItem->cbControlBufferLen=0;
			free(lpEditItem);
//			SendNotificationMsg(hWnd,EN_ERRSPACE);
		    return -1;
		}

		lpEditItem->lpPDASaveEditBuffer=(LPTSTR)malloc(lpEditItem->cbEditBufferLen);
		if (lpEditItem->lpPDASaveEditBuffer==NULL)
		{  // allocate failare then free struct EDITITEM and return FALSe
		  lpEditItem->cbEditBufferLen=0;
		  lpEditItem->cbControlBufferLen=0;
		  free(lpEditItem->lpPDAControlBuffer);
		  free(lpEditItem);
//		  SendNotificationMsg(hWnd,EN_ERRSPACE);
		  return -1;
		}

		lpEditItem->lpPDAEditBuffer=lpEditItem->lpPDAControlBuffer+lpEditItem->cbTitleLen;
		
		// clear struct EDITITEM
//		Clear(lpEditItem);
    ClearEditItemStruct(lpEditItem);

		// read control title text
		if (lpEditItem->cbTitleLen)
		  memcpy(lpEditItem->lpPDAControlBuffer,lpPDAEditProperty->lpTitle, lpEditItem->cbTitleLen );		// Read Text Success

		// read Window text
		if (lpcs->lpszName!=NULL)
		{
			lpEditItem->cbEditLen=strlen(lpcs->lpszName);
			//  the window text can put the default buffer ???
			if (lpEditItem->cbEditLen>lpEditItem->cbEditLimitLen)
			{
				lpEditItem->cbEditLen=0;
				SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(short)(lpEditItem->cbEditLen+1));
			}
			if (dwStyle&ES_PASSWORD)
			{
				dwStyle&=~ES_MULTILINE;
				SetWindowLong(hWnd,GWL_STYLE,dwStyle);
				// read Window text
				FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord);
			}
			else
			{
				GetEditText(lpEditItem->lpPDAEditBuffer,lpcs->lpszName,(short)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'
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
    if (dwStyle&ES_FORMAT)
		{
			if (lpPDAEditProperty->lpFormat!=NULL)
			{
				dwStyle&=~ES_MULTILINE;
				SetWindowLong(hWnd,GWL_STYLE,dwStyle);
				nFormatLen=strlen(lpPDAEditProperty->lpFormat)+1;
				if (nFormatLen)
				{
					lpEditItem->lpFormat=(LPTSTR)malloc(nFormatLen);
					if (lpEditItem->lpFormat)
						memcpy(lpEditItem->lpFormat,lpPDAEditProperty->lpFormat, nFormatLen );		// Read Text Success
					else
						SendNotificationMsg(hWnd,EN_ERRSPACE);
				}
			}
			else
			{
				dwStyle&=~ES_FORMAT;
				SetWindowLong(hWnd,GWL_STYLE,dwStyle);
			}
		}
    lpEditItem->dwStyle=dwStyle;
		lpEditItem->nTotalLine=-1;

/*		
		lpEditItem->cl_NormalText = CL_BLACK;
		lpEditItem->cl_NormalBkColor = CL_WHITE;

		lpEditItem->cl_InvertText = CL_WHITE;
		lpEditItem->cl_InvertBkColor = CL_BLACK;

		lpEditItem->cl_DisableText = CL_LIGHTGRAY;
		lpEditItem->cl_DisableBkColor = CL_WHITE;
*/

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

		SetPDAEditScrollPage(hWnd);
		lpEditItem->iStartPos=0;
		GetClientRect(hWnd,&lpEditItem->rect);
		SetWindowRect(hWnd,lpEditItem,FALSE);
		SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
		SetPDAEditHScrollPos(hWnd,(short)((lpEditItem->nDisplayx)/HSCROLLWIDTH));

		// !!! Add By Jami chen in 2003.08.08
		// setdefault  Caret position 
		SetPDACaretPosition(hWnd,lpEditItem,0,0);
		// !!! Add End By Jami  chen in 2003.08.08

		// initial success ,return TRUE
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
	DWORD dwStyle;
    LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return FALSE;


			CreatePDACaret(hWnd,lpEditItem);
			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
			if (dwStyle&ES_MULTIITEMEDIT)
			{
//				hParentWnd=GetParent(hWnd);
				SendNotificationMsg( hWnd,EN_SETFOCUS);
			}
      SetPDACaret(hWnd);
      // show caret
      ShowPDACaret(hWnd);
      wParam++;
      lParam++;
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
    LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return FALSE;

      ClearInvert(hWnd,lpEditItem,TRUE);
	    DeletePDACaret(hWnd);
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);
      wParam++;
      lParam++;
    	return 0;
}
/*****************************************************************/
/*****************************************************************/
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  RECT rectNew;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return FALSE;
      GetClientRect(hWnd,&rectNew);
      lpEditItem->rect.right=rectNew.right;
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
	short xPos,yPos;
  LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return FALSE;


			// Get LButton down Position
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);

			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);

			//  is or not get facus???
			if (GetFocus()!=hWnd)
			{  // if not active then get focus
				SetFocus(hWnd);
			}

			// Clear Invert and redraw
			ClearInvert(hWnd,lpEditItem,TRUE);
			//// set caret position
			//SetPDACaret(hWnd);

			// Capture the mouse
			if (GetCapture()!=hWnd)
				SetCapture(hWnd);

			// set Invert Area
			SetInvertArea(hWnd,lpEditItem);

      if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
			{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
				InvalidateRect(hWnd,NULL,TRUE);
			}

			// set caret position
			SetPDACaret(hWnd);

      // show caret
      ShowPDACaret(hWnd);
      wParam++;
    return 0;
}
/*****************************************************************/
// process WM_MOUSEMOVE Message

// xPos=LOWORD(lParam)
// yPos=HIWORD(lParam)
/*****************************************************************/
static LRESULT DoMouseMove(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	short xPos,yPos;
  LPEDITITEM lpEditItem;


	// if the mouse be capture by this window
	if (GetCapture()==hWnd)
	{

			// Get LButton down Position
			xPos=LOWORD(lParam);
			yPos=HIWORD(lParam);

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return FALSE;


			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);

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
/*****************************************************************/
// process WM_LBUTTONUP Message

// xPos=LOWORD(lParam)
// yPos=HIWORD(lParam)
/*****************************************************************/
static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture 
	SetCapture(NULL);
  wParam++;
  lParam++;	
}
/*********************************************************************/
// process WM_CHAR Message
// Input :  hWnd --- handle of the window
// chCharCode = (TCHAR)wParam
/*********************************************************************/
static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	TCHAR chCharCode;
	LPEDITITEM lpEditItem;
	DWORD dwStyle;
	BOOL bResult;
	DWORD dwShowFlag;
	LPTSTR lpShowAddress;
	short nShowLine,nShowPos;
	HDC hdc;
	short xPos,nLine;
	int bDeleteResult;
  BOOL bDeleteInvert; 

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
        SetCaretCoordinate(hWnd,lpEditItem);
      	AdjustCaretInEditItem(hWnd,lpEditItem);
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
		         SetCaretCoordinate(hWnd,lpEditItem);
             AdjustCaretInEditItem(hWnd,lpEditItem);
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
			{ 
				lpEditItem->HalfByte=FALSE;
				if (PickCharacter(0,chCharCode,dwStyle)==FALSE) return;
				if (dwStyle&ES_UPPERCASE)
					chCharCode=toupper(chCharCode);
				if (dwStyle&ES_LOWERCASE)
					chCharCode=tolower(chCharCode);
     		//Delete Invert Area
        if (DeleteInvert(hWnd,lpEditItem))
        {
		       SetCaretCoordinate(hWnd,lpEditItem);
      	   AdjustCaretInEditItem(hWnd,lpEditItem);
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
		lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx;
		lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
		if (bResult)
		{ // the insert operation success ,must the redraw the window
			// add code : if Caret out of the client area then must 
			// Reset Display Position
			if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
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

      if (bDeleteInvert==TRUE)
      {
		    lpShowAddress=NULL;
				nShowPos=0;
				nShowLine=0;
				dwShowFlag=PE_SHOWCLIENT;
      }

      // redraw the window
			hdc=GetDC(hWnd);
	        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,nShowPos,nShowLine,0,dwShowFlag);
			ReleaseDC(hWnd,hdc);
			// reset thw caret
		}
		SetPDACaret(hWnd);
    lParam++;
}
/*********************************************************************/
// Pick character ,
// Input:
//          HiByte : the high byte of the Insert word 
//					if  the high byte is zero then this is a english or symbol
//          LoByte : the low byte of the insert word
// return : if the character is not beyond the style ,then return FALSE
//          else retrun TRUE
/*********************************************************************/
static BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle)
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
/*********************************************************************/
// Process WM_KEYDOWM Message for along edit
// input :
//		nVirtKey = (int) wParam;    // virtual-key code
//		lKeyData = lParam;          // key data
/*********************************************************************/
static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
  short nPageLine,nScrollLine,iHorzPos;

		nVirtKey = (int) wParam;    // virtual-key code
    nPageLine=GetPageLine(hWnd);
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
			HorzMoveCaret(hWnd,lpEditItem,iHorzPos);
		}
//		return;
	}
    ScrollCaretInPDAEdit(hWnd,nScrollLine);
    lParam++;
}
/***************************************************************************/
// Get Window Text height
// input : hWnd -- handle of the window
// return Value:
//       return window text Height
/***************************************************************************/
int GetTextHeight(HWND hWnd)
{
	HDC hdc;
	TEXTMETRIC TextMetric;
  short nTextHeight;
  DWORD dwStyle;
		hdc=GetDC(hWnd);
		// get current text property
		GetTextMetrics(hdc,&TextMetric);
    ReleaseDC(hWnd,hdc);
    nTextHeight=(short)TextMetric.tmHeight;

    dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		// add a dot line height , 1 dot
    if (dwStyle&ES_LINESEPARATE)
      nTextHeight+=1;
		return nTextHeight;
}
/********************************************************************************/
// process mwssage EM_GETCARETPOSITION
// lpnLine =(short *)wParam  // return the caret line
// lpxPos =(short *)lParam   // return the caret x position
/********************************************************************************/
static void DoGetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  short *lpnLine,*lpxPos;
    	LPEDITITEM lpEditItem;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() ); 
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

        lpnLine=(short *)wParam;
        lpxPos=(short *)lParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
        if (lpEditItem==NULL) return;
				if (lpnLine)
					*lpnLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;
				if (lpxPos)
					*lpxPos=lpEditItem->nCaretx+lpEditItem->nDisplayx;
}
/********************************************************************************/
// process mwssage EM_SETCARETPOSITION
// nCaretLine=(short )wParam;  // the caret line
// xCaretPos=(short )lParam;   // the caret x position
/********************************************************************************/
static void DoSetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  short nTextHeight,xCaretPos,xPos,yPos,nCaretLine;
  LPEDITITEM lpEditItem;

        xCaretPos=(short )lParam;
        nCaretLine=(short )wParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
        if (lpEditItem==NULL) return;
        nTextHeight=GetTextHeight(hWnd);

        xPos=xCaretPos-lpEditItem->nDisplayx;
        yPos=(nCaretLine-lpEditItem->nDisplayLine)*nTextHeight;

        SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);
			  AdjustCaretInEditItem(hWnd,lpEditItem);

  			SetPDACaret(hWnd);
}

/********************************************************************************/
// process mwssage MIEM_SETXDISPLAYPOS
// xPos=(short )wParam;   // the new display x position
/********************************************************************************/
static void DoSetxDisplayPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  HDC hdc;
  short xPos,xCaretPos;

        xPos=(short )wParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
        if (lpEditItem==NULL) return;
				// get old caret x position
        xCaretPos=lpEditItem->nDisplayx+lpEditItem->nCaretx;
				// set new display position
        lpEditItem->nDisplayx=xPos;
				// set HSCROLL
        SetPDAEditHScrollPos(hWnd,(short)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
        hdc=GetDC(hWnd);
				// redraw the window
        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,NULL,0,0,0,PE_SHOWCLIENT);
        ReleaseDC(hWnd,hdc);
				// reset the caret position
        lpEditItem->nCaretx=xCaretPos-lpEditItem->nDisplayx;
        SetPDACaret(hWnd);
        lParam++;
}
/*****************************************************************************/
// set edit Scroll page
// input : hWnd ---- handle of the window
/*****************************************************************************/
static void SetPDAEditScrollPage(HWND hWnd)
{
   short nPageLine;
   //short nTextHeight;
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
/*****************************************************************************/
// set vertical scroll position
// input :
//         hWnd ---- handle of the window
//				 nNewScrollPos --- will to be set new position
/*****************************************************************************/
static void SetPDAEditVScrollPos(HWND hWnd,short nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE);
}
/*****************************************************************************/
// set horizon scroll position
// input :
//         hWnd ---- handle of the window
//				 nNewScrollPos --- will to be set new position
/*****************************************************************************/
static void SetPDAEditHScrollPos(HWND hWnd,short nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);
      if (!(dwStyle&WS_HSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE);
}
/*****************************************************************************/
// process message WM_VSCROLL
/*****************************************************************************/
static short DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    short nScrollLine,nWindowLine;
//    RECT rcClientRect;
    short yPos;
        
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
        return VScrollWindowInPDAEdit(hWnd,nScrollLine);
}
/*****************************************************************************/
// process message WM_HSCROLL
/*****************************************************************************/
static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    short nHScrollPos,xWindowWidth;
//    RECT rcClientRect;
    short xPos;
        
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
        HScrollWindowInPDAEdit(hWnd,nHScrollPos);
}
/*****************************************************************************/
// get lines of a page 
/*****************************************************************************/
static short GetPageLine(HWND hWnd)
{
  short nTextHeight,nPageLine;
  RECT rcClientRect;
		// get line height
    nTextHeight=GetTextHeight(hWnd);
		// get client rect
    GetClientRect(hWnd,&rcClientRect);
		// get page lines
    nPageLine=rcClientRect.bottom/nTextHeight;
    return nPageLine;
}
/*****************************************************************************/
// vertical scroll caret
/*****************************************************************************/
static void ScrollCaretInPDAEdit(HWND hWnd,short nScrollLine)
{
	LPEDITITEM lpEditItem;
//	short nClientLine;
	//RECT ClientRect;
	short nPointCoordinate,nLine,nCaretPosition;
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
/*****************************************************************************/
// vertical scroll window
// Input:
//         hWnd --- handle of the window
//				 nscrollLine --- the lines to be scroll
/*****************************************************************************/
static short VScrollWindowInPDAEdit(HWND hWnd,short nScrollLine)
{
	LPEDITITEM lpEditItem;
	short nClientLine;
//	RECT ClientRect;
	short nCaretLine,nOldDisplayLine;
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
		return (short)(lpEditItem->nDisplayLine-nOldDisplayLine);
}
/*****************************************************************************/
// horizon scroll window
// Input:
//         hWnd --- handle of the window 
//				 nHScrollPos --- the width to be scroll
/*****************************************************************************/
static void HScrollWindowInPDAEdit(HWND hWnd,short nHScrollPos)
{
	LPEDITITEM lpEditItem;
	short nCaretPos;

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
    SetPDAEditHScrollPos(hWnd,(short)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
		// Get caret line from the text
		//ClearInvert(hWnd,lpEditItem,TRUE);
		// reset caret
		SetPDACaret(hWnd);
		// redraw the window
//    DrawPDAEditControl(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
}
/*******************************************************************************/
// fill the buffer by chPassWord
// Input :   
//           lpEditBuffer --- the buffer will to be filled
//           nFillLength  --- the length of the fill
//           chPassWordWord --- the character will to be filled   
/*******************************************************************************/
static void FillPassWordChar(LPTSTR lpEditBuffer,short nFillLength,TCHAR chPassWordWord)
{
	// fill the buffer
  memset(lpEditBuffer,chPassWordWord,nFillLength);
	// fill end code
  lpEditBuffer[nFillLength]=0;
}
/***********************************************************************************************/
// release the edit memory
/***********************************************************************************************/
static void ReleasePDAEdit(HWND hWnd)
{
    LPEDITITEM lpEditItem;

			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return ;

	    DeletePDACaret(hWnd);
//	    SendNotificationMsg(hWnd,EN_KILLFOCUS);

			if (lpEditItem->lpPDASaveEditBuffer)
				free(lpEditItem->lpPDASaveEditBuffer);
			if (lpEditItem->lpPDAControlBuffer)
				free(lpEditItem->lpPDAControlBuffer);
			if (lpEditItem->lpFormat)
				free(lpEditItem->lpFormat);
			free(lpEditItem);
			SetWindowLong(hWnd,0,0);

}

/************************************************************************************/
// Process Edit Message -- EM_CHARFROMPOS
// Input :
//    wParam = 0;                        // not used
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
    short xPos,yPos;
    LPEDITITEM lpEditItem;

      xPos=LOWORD(lParam);
      yPos=HIWORD(lParam);

	    // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return 0;
      return GetCharFromPos(hWnd,lpEditItem,xPos,yPos);
}

/************************************************************************************/
// Process Edit Message -- EM_GETFIRSTVISIBLELINE
// Input :
//    wParam = 0;                        // not used
//    lParam = 0;                        // not used
// Return Values
// The return value is the zero-based index of the uppermost visible line in a 
// multiline edit control. For single-line edit controls, the return value is 
// the zero-based index of the first visible character ,include title. 
/************************************************************************************/
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
			lpPosition=GetLinePosition(hWnd,lpEditItem,lpEditItem->lpPDAControlBuffer,0,FALSE,NULL);
      return (lpPosition-lpEditItem->lpPDAControlBuffer);
}
/************************************************************************************/
// Process Edit Message -- EM_GETLIMITTEXT 
// Input :
// wParam = 0;  // not used, must be zero 
// lParam = 0;  // not used, must be zero 
// Return Values
// The return value is the text limit. 
/************************************************************************************/
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
    wParam++;
    lParam++;
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
    LPEDITITEM lpEditItem;
	short nLine,nLineLen,nBufferMaxLen;
	LPSTR lpch;
	LPSTR lpLineAddress;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

	  nLine=(short)wParam;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

	  lpch=(LPSTR)lParam;

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
// Process Edit Message -- EM_GETTHUMB
// An application sends the EM_GETTHUMB message to retrieve the position of the scroll 
// box (thumb) in a multiline edit control. 
// Input:
// wParam = 0;  // not used, must be zero 
// lParam = 0;  // not used, must be zero 

// Return Values
// The return value is the position of the scroll box. 
/************************************************************************************/
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (!(dwStyle&WS_VSCROLL)) return 0;
    wParam++;
    lParam++;
		return Edit_GetScrollPos(hWnd,SB_VERT);
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
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	  if(lpEditItem==NULL) return 0;
      wParam++;
      lParam++;
	  return lpEditItem->nTotalLine;
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
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return FALSE;

    wParam++;
    lParam++;
		return lpEditItem->fModified;
}
/************************************************************************************/
// Process Edit Message -- EM_GETPASSWORDCHAR
// An application sends an EM_GETPASSWORDCHAR message to retrieve the password 
// character displayed in an edit control when the user enters text. 
// Input :
// wParam = 0; // not used; must be zero 
// lParam = 0; // not used; must be zero 
 
// Return Values
// The return value specifies the character to be displayed in place of the 
// character typed by the user. The return value is NULL if no password character 
// exists. 
/************************************************************************************/
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
				return lpEditItem->chPassWordWord;
		else
				return 0;
}
/************************************************************************************/
// Process Edit Message -- EM_GETSEL
// An application sends an EM_GETSEL message to get the starting and ending 
// character positions of the current selection in an edit control. 
// Input :
// wParam = (WPARAM) (LPDWORD) lpdwStart; // receives starting position 
// lParam = (LPARAM) (LPDWORD) lpdwEnd;   // receives ending position 
 
// Parameters
// lpdwStart 
//    Value of wParam. Pointer to a 32-bit value that receives the starting position 
//    of the selection. This parameter can be NULL. 
// lpdwEnd 
// Value of lParam. Pointer to a 32-bit value that receives the position of the 
// first nonselected character after the end of the selection. This parameter can 
// be NULL. 
// Return Values
// The return value is a zero-based 32-bit value with the starting position of the 
// selection in the low-order word and the position of the first character after 
// the last selected character in the high-order word. If either of these values 
// exceeds 65, 535, the return value is -1.
/************************************************************************************/
static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 short nStart , nEnd;
	 LPDWORD lpdwStart,lpdwEnd;
	 LRESULT lResult;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() ); 
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
			nStart=lpEditItem->lpInvertStart-lpEditItem->lpPDAEditBuffer;
			nEnd=lpEditItem->lpInvertEnd-lpEditItem->lpPDAEditBuffer;
		}
		if (lpdwStart)
			*lpdwStart=(DWORD)nStart;
		if (lpdwEnd)
			*lpdwEnd=(DWORD)nEnd;
// !!! Modified By Jami chen on 2003.09.12
		if (nEnd > 0xffff || nStart > 0xffff)
			lResult = -1;
		else
			lResult=MAKELRESULT(nStart,nEnd);
// !!! Modified End By Jami chen on 2003.09.12
		return lResult;
}

/************************************************************************************/
// Process Edit Message -- EM_LIMITTEXT
// An application sends an EM_LIMITTEXT message to limit the amount of text the 
// user may enter into an edit control.
// Input:
// wParam = (WPARAM) cchMax;   // text length, in characters 
// lParam = 0;                 // not used; must be zero 
 
// Parameters
// cchMax 
//    Value of wParam. Specifies the maximum number of characters the user can 
//    enter. If this parameter is zero, then will not to be set.
// Return Values
// if SetLimitText success , then return nozero ,other return zero
// Remark: The Style is ES_FORMAT ,then this Message is no effect
/************************************************************************************/
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 short cchMax;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		cchMax=(short)wParam;

    return SetLimitText(hWnd,lpEditItem,cchMax);
}

/************************************************************************************/
// Process Edit Message --EM_LINEFROMCHAR
// An application sends an EM_LINEFROMCHAR message to retrieve the index of the line 
// that contains the specified character index in a multiline edit control. A character 
// index is the number of characters from the beginning of the edit control. 
// Input :
// wParam = (WPARAM) ich;  // character index 
// lParam = 0;             // not used; must be zero 
 
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
   LPEDITITEM lpEditItem;
	 DWORD dwStyle;
	 LPTSTR lpCurPos;
	 short ich,nLine;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		ich=(short)wParam;
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
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
// lParam = 0;             // not used; must be zero 
 
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
   LPEDITITEM lpEditItem;
	 LPTSTR lpCurPos;
	 short nLine;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		nLine=(short)wParam;
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
// lParam = 0;             // not used; must be zero 
 
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
   LPEDITITEM lpEditItem;
   short ich;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		ich=(short)wParam;
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
	DWORD dwStyle;
	short cxScroll,cyScroll;
	
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (!(dwStyle&ES_MULTILINE))
			return FALSE;
		cxScroll=(short)wParam;
		cyScroll=(short)lParam;
		if (cxScroll)
		{
//				cxScroll=cxScroll*HSCROLLWIDTH;
				cxScroll=cxScroll*8;   // 8 --- The Width of the Char
				HScrollWindowInPDAEdit(hWnd,cxScroll);
		}
		if (cyScroll)
				VScrollWindowInPDAEdit(hWnd,cyScroll);
		return TRUE;
}

/************************************************************************************/
// Process Edit Message -- EM_POSFROMCHAR
// An application sends the EM_POSFROMCHAR message to retrieve the coordinates of 
// the specified character in an edit control.
// Input :
// wParam = (WPARAM) wCharIndex;    // zero-based index of character 
// lParam = 0;                      // not used
 
// Parameters
// wCharIndex 
//			Value of wParam. Specifies the zero-based index of the character. 
// Return Values
// The return value is the position of the character, (x, y). For a single-line edit 
// control, the y-coordinate is always zero. 
/************************************************************************************/
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 short wCharIndex;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		wCharIndex=(short)wParam;
    return GetPosFromChar(hWnd,lpEditItem,wCharIndex);
}
/************************************************************************************/
// Process Edit Message -- EM_REPLACESEL
// An application sends an EM_REPLACESEL message to replace the current selection in 
// an edit control with the specified text. 
// Input :
// wParam = 0;				  							// not used
// lpszReplace = (LPCTSTR) lParam ;   // pointer to replacement text string 
 
// Parameters
// lpszReplace 
//		Value of lParam. Pointer to a null-terminated string containing the replacement text. 
// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPCTSTR lpReplace;
	short cchLen;
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif
		lpReplace=(LPCTSTR)lParam;
		DeleteInvert(hWnd,lpEditItem);
		cchLen=strlen(lpReplace);
		InsertString(hWnd,lpEditItem,lpReplace,cchLen);
		SetCaretCoordinate(hWnd,lpEditItem);
		AdjustCaretInEditItem(hWnd,lpEditItem);
		InvalidateRect(hWnd,NULL,TRUE);
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
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return FALSE;
		if (AdjustCaretInEditItem(hWnd,lpEditItem))
			InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
/************************************************************************************/
// Process Edit Message -- EM_SETMODIFY
// An application sends an EM_SETMODIFY message to set or clear the modification flag for an edit control. The modification flag indicates whether the text within the edit control has been modified. It is automatically set whenever the user changes the text. An EM_GETMODIFY message can be sent to retrieve the value of the modification flag. 
// Input:
// wParam = (WPARAM) (UINT) fModified;    // modification flag 
// lParam = 0;                            // not used; must be zero 
 
// Parameters
// fModified 
//		Value of wParam. Specifies the new value for the modification flag. A value of TRUE indicates the text has been modified, and a value of FALSE indicates it has not been modified. 
// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 UINT  fModified;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		fModified=(UINT)wParam;
		lpEditItem->fModified=fModified;
		return 0;
}
/************************************************************************************/
// Process Edit Message -- EM_SETPASSWORDCHAR
// An application sends an EM_SETPASSWORDCHAR message to set or remove the password 
// character for a single-line edit control when the user types text. When a password 
//character is set, that character is displayed in place of each character the user types. 
// Input :
// wParam = (WPARAM) (UINT) ch;    // character to display 
// lParam = 0;  									 //	 not to used
 
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
   LPEDITITEM lpEditItem;
	 DWORD dwStyle;
	 TCHAR ch;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (dwStyle&ES_MULTILINE)
			return 0;
	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		ch=(TCHAR)wParam;
    return SetPasswordChar(hWnd,lpEditItem,ch);
}
/************************************************************************************/
// Process Edit Message -- EM_SETREADONLY
// An application sends an EM_SETREADONLY message to set or remove the read-only 
// style (ES_READONLY) of an edit control. 
// Input:
// wParam = (WPARAM) (BOOL) fReadOnly;  // read-only flag 
// lParam = 0;                          // not used; must be zero 

// Parameters
// fReadOnly
//		Value of wParam. Specifies whether to set or remove the ES_READONLY style. A value
//		of TRUE sets the ES_READONLY style; a value of FALSE removes the ES_READONLY style.
// Return Values
// If the operation succeeds, the return value is nonzero; otherwise, it is zero.
/************************************************************************************/
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
			{
				dwStyle|=ES_READONLY;
			}
			else
			{
				dwStyle&=~ES_READONLY;
			}
      lpEditItem->dwStyle=dwStyle;
			SetWindowLong(hWnd,GWL_STYLE,dwStyle);
			return TRUE;
}
/************************************************************************************/
// Process Edit Message -- EM_SETSEL
// An application sends an EM_SETSEL message to select a range of characters in an edit control. 
// Input:
// wParam = (WPARAM) (INT) nStart;    // starting position 
// lParam = (LPARAM) (INT) nEnd;      // ending position 
 
// Parameters
// nStart 
//		Value of wParam. Specifies the starting character position of the selection. 
// nEnd 
//		Specifies the ending character position of the selection. 
// Return Values
// This message does not return a value. 
/************************************************************************************/
static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 short nStart,nEnd;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		nStart=(short)wParam;
		nEnd=(short)lParam;

    return SetSel(hWnd,lpEditItem,nStart,nEnd);
}
/************************************************************************************/
// This function is send a notification message to the parent window
// Input :
// hWnd ---- the handle of the window
// wNotifyCode ---- the notification message to be send
// no return code
/************************************************************************************/
void SendNotificationMsg( HWND hWnd, WORD wNotifyCode)
{
	WORD idEditCtrl;
	WPARAM wParam;
	HWND hParentWnd;

		idEditCtrl=(int)GetWindowLong(hWnd,GWL_ID);
		wParam=MAKEWPARAM(idEditCtrl,wNotifyCode);
		hParentWnd=GetParent(hWnd);
		SendMessage(hParentWnd,WM_COMMAND,wParam,(LPARAM)hWnd);
}

/************************************************************************************/
// Process Edit Message -- WM_COPY
// An application sends the WM_COPY message to an edit control or combo box to copy 
// the current selection to special buffer
// Input:
// wParam = 0;                          // not used; must be zero 
// lParam = 0;                          // not used; must be zero 

// Parameters
// This message has no parameters.

// Return Values
// This message does not return a value.
/************************************************************************************/
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

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
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

    PasteFromClipboard(hWnd,lpEditItem);
		AdjustCaretInEditItem(hWnd,lpEditItem);
		InvalidateRect(hWnd,NULL,TRUE);
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
		return 0;
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
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		DeleteInvert(hWnd,lpEditItem);
		SetCaretCoordinate(hWnd,lpEditItem);
		AdjustCaretInEditItem(hWnd,lpEditItem);
		InvalidateRect(hWnd,NULL,TRUE);
		SetPDACaret(hWnd);
		return 0;
}
/************************************************************************************/
// process Message -WM_SETTEXT
// An application sends a WM_SETTEXT message to set the text of a window. 
// Input :
// wParam = 0;                     // not used; must be zero 
// lParam = (LPARAM)(LPCTSTR)lpsz; // address of window-text string 
// Parameters
// lpsz 
//    Value of lParam. Pointer to a null-terminated string that is the window text. 
// Return Values
// The return value is TRUE if the text is set. It is FALSE if insufficient space 
// is available to set the text in the edit control.
/************************************************************************************/
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPCTSTR lpsz;
   LPEDITITEM lpEditItem;


	// Get struct EDITITEM data
	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	if(lpEditItem==NULL) return FALSE;
	lpsz= (LPCTSTR)lParam; // address of window-text string
    if (SetText(hWnd,lpEditItem,lpsz)==FALSE)
	  return FALSE;
    SetPDAEditVScrollRange(hWnd,lpEditItem->nTotalLine);
	lpEditItem->nDisplayx=0;
	lpEditItem->nDisplayLine=0;
	SetEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
	SetEditHScrollPos(hWnd,(short)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
	SetPDACaretPosition(hWnd,lpEditItem,0,0);
	AdjustCaretInEditItem(hWnd,lpEditItem);
	InvalidateRect(hWnd,NULL,TRUE);
	SetPDACaret(hWnd);
	return TRUE;
}
/************************************************************************************/
// process Message -WM_GETTEXT
// An application sends a WM_GETTEXT message to copy the text that corresponds to a
// window into a buffer provided by the caller.
// Input:
// wParam = (WPARAM) cchTextMax;   // number of characters to copy
// lParam = (LPARAM) lpszText;     // address of buffer for text

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
   LPTSTR lpsz;
	 short cMaxlen;

   LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return FALSE;
			lpsz= (LPTSTR)lParam; // address of window-text string
			cMaxlen=(short)wParam;
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

// wParam = 0; // not used; must be zero
// lParam = 0; // not used; must be zero
/************************************************************************************/
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;
}
/************************************************************************************/
// WILL save the lpBuffer text to the lpSaveBuffer
// Input : lpSaveBuffer ---- the object address to be saved
//				 lpBuffer  ---- the source address will to be saved
//				 cbEditLen ---- the length of the text be saved
/************************************************************************************/
static void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,short cbEditLen)
{
	strncpy(lpSaveBuffer,lpBuffer,cbEditLen);
}
/************************************************************************************/
// WILL get the lpGetBuffer text from the lpBuffer
// Input : lpGetBuffer ---- the object address to be get
//				 lpBuffer  ---- the source address will to be get
//				 cbEditLen ---- the length of the text be get
/************************************************************************************/
static void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,short cbEditLen)
{
	strncpy(lpGetBuffer,lpBuffer,cbEditLen);
}
/************************************************************************************/
// Process Message --- EM_EMPTY
// wParam = 0;
// lParam = 0;
/************************************************************************************/
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return FALSE;
//			Clear(lpEditItem);
      ClearEditItemStruct(lpEditItem);

//			SetPDAEditScrollPage(hWnd);
			SetWindowRect(hWnd,lpEditItem,FALSE);
			SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
			SetPDAEditHScrollPos(hWnd,(short)((lpEditItem->nDisplayx)/HSCROLLWIDTH));
			SetPDACaret(hWnd);
			InvalidateRect(hWnd,NULL,TRUE);
			return 0;
}

/************************************************************************************/
// Process Message --- EM_SEARCHSTRING
// wParam = iStart;
// lParam = (LPTSTR)lpSearchString;
/************************************************************************************/
static LRESULT DoSearchString(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  LPTSTR lpStart,lpStringPos;
  short nStart,nEnd;
  int iStart;
  LPTSTR lpSearchString;
  LRESULT lResult;
  short xPos,yPos;

	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	if (lpEditItem==NULL) 
		return -1;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

	iStart = (int)wParam;
	lpSearchString = (LPTSTR)lParam;
	if (strlen(lpSearchString) == 0)
		return -1;

	if (iStart >= lpEditItem->cbEditLen)
		return -1;
	if (iStart == -1)
	{
		lpStart = lpEditItem->lpPDAEditBuffer;
	}
	else
	{
		lpStart = lpEditItem->lpPDAEditBuffer + iStart;
	}
	lpStringPos = strstr(lpStart,lpSearchString);
	if (lpStringPos == NULL)
		return -1;
	nStart = lpStringPos - lpEditItem->lpPDAEditBuffer;
	nEnd = nStart + strlen(lpSearchString);
	SetSel(hWnd,lpEditItem,nStart,nEnd);
	lResult = GetPosFromChar(hWnd,lpEditItem,nEnd);
	xPos = LOWORD(lResult);
	yPos = HIWORD(lResult);
	SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);
    if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
	{
		// Redraw the window
		InvalidateRect(hWnd,NULL,TRUE);
	}

	// set caret position
	SetPDACaret(hWnd);

      // show caret
    ShowPDACaret(hWnd);
	return nStart;
}

static LRESULT DoPaint(HWND hWnd,HDC hdc)
{
  LPEDITITEM lpEditItem;

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
   if (lpEditItem==NULL) return 0;
   DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWCLIENT);
   ClearBottomBlankArea(hWnd,hdc,lpEditItem);
   return 0;
}
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  HDC hdc;
  short nDisplayLine,nDisplayx;

   hdc=(HDC)wParam;
   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
   if (lpEditItem==NULL) return 0;
   nDisplayLine=lpEditItem->nDisplayLine;
   nDisplayx=lpEditItem->nDisplayx;
   lpEditItem->nDisplayLine=0;
   lpEditItem->nDisplayx=0;
   DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWALL);
   //ClearBottomBlankArea(hWnd,hdc,lpEditItem);
   lpEditItem->nDisplayLine=nDisplayLine;
   lpEditItem->nDisplayx=nDisplayx;
   return 0;
}

static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  DWORD dwStyle;

		// get window style
   dwStyle = GetWindowLong(hWnd,GWL_STYLE);

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
   if (lpEditItem==NULL)
     return FALSE;
   lpEditItem->dwStyle = dwStyle;
   return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}


static LRESULT DoDeleteChar(HWND hWnd)
{
  LPEDITITEM lpEditItem;
  DWORD dwStyle;

		// get window style
   dwStyle=GetWindowLong(hWnd,GWL_STYLE);
   if (dwStyle&ES_READONLY) return FALSE;

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
   if (lpEditItem==NULL)
     return FALSE;
   DeleteChar(hWnd,lpEditItem);
   return TRUE;
}


static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
//	COLORREF cl_NewColor;
	LPCTLCOLORSTRUCT lpCtlColor;
		
	LPEDITITEM lpEditItem;

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
		if (lpEditItem==NULL)
			return FALSE;
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {
			 lpEditItem->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {
				lpEditItem->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {
				lpEditItem->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {
				lpEditItem->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpEditItem->cl_DisableText = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpEditItem->cl_DisableBkColor = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 {
				lpEditItem->cl_ReadOnly = lpCtlColor->cl_ReadOnly;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 {
				lpEditItem->cl_ReadOnlyBk  = lpCtlColor->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {
				lpEditItem->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {
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
		 InvalidateRect(hWnd,NULL,TRUE);
		return TRUE;
}
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
		
	LPEDITITEM lpEditItem;
	LPCTLCOLORSTRUCT lpCtlColor;

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
		if (lpEditItem==NULL)
			return -1;
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {
			 lpCtlColor->cl_Text = lpEditItem->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {
				lpCtlColor->cl_TextBk = lpEditItem->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {
				lpCtlColor->cl_Selection = lpEditItem->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {
				lpCtlColor->cl_SelectionBk = lpEditItem->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {
				lpCtlColor->cl_Disable= lpEditItem->cl_DisableText ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {
				lpCtlColor->cl_DisableBk = lpEditItem->cl_DisableBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 {
				lpCtlColor->cl_ReadOnly= lpEditItem->cl_ReadOnly ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 {
				lpCtlColor->cl_ReadOnlyBk = lpEditItem->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {
				lpCtlColor->cl_Title= lpEditItem->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {
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

static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
{
  RECT rcClientRect,rcClearRect;
  short nBottomPos;
  HBRUSH hBrush;


	nBottomPos=(short)lpEditItem->rect.bottom;
	GetClientRect(hWnd,&rcClientRect);
    if (nBottomPos<rcClientRect.bottom)
    {
	    rcClearRect.left=0;
	    rcClearRect.top=nBottomPos;
	    rcClearRect.right=rcClientRect.right;
	    rcClearRect.bottom =rcClientRect.bottom;
	    // clear ClearRect
	    //FillRect(hdc,&rcClearRect,GetStockObject(WHITE_BRUSH));
		hBrush = GetBkBrush(hWnd);
	    FillRect(hdc,&rcClearRect,hBrush);
		DeleteObject(hBrush);
    }
}

