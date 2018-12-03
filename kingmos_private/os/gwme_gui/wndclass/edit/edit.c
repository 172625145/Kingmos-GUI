/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵�����༭��
�汾�ţ�1.0.0.456
����ʱ�ڣ�2001-12-26
���ߣ�Jami chen �½���
�޸ļ�¼��
  2004.07.19 ������Ϣ����WM_SYSCOLORCHANGE, ��������ϵͳ��ɫ����
			 ���ú���Ҫ����װ���µ���ɫ
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
// ������ATOM RegisterPDAEditClass(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// ����ֵ������ϵͳע����
// ����������ע��༭��
// ����: 
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
// ������LRESULT CALLBACK PDAEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN uMsg -- Ҫ�������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// ����ֵ����Ϣ������
// ��������������༭����Ϣ��
// ����: 
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
	    case WM_SIZE:  // ���ڳߴ�ı�
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
    case EM_GETCARETPOS:  // �õ����λ��
      DoGetCaretPosition(hWnd,wParam,lParam);
	  break;
    case EM_SETCARETPOS: // ���ù��λ��
      DoSetCaretPosition(hWnd,wParam,lParam);
      break;
    case MIEM_SETXDISPLAYPOS: // ����ˮƽ�������ʾλ��
      DoSetxDisplayPos(hWnd,wParam,lParam);
      break;
    case WM_VSCROLL: // ��ֱ����
      DoVScrollWindow(hWnd,wParam,lParam);
      break;
    case WM_HSCROLL:  // ˮƽ����
      DoHScrollWindow(hWnd,wParam,lParam);
      break;
    case EM_CHARFROMPOS:  //�õ�ָ������ַ�
      return DoCharFromPos(hWnd,wParam,lParam);
    case EM_GETFIRSTVISIBLELINE: //�õ���һ���ɼ���
      return DoGetFirstVisibleLine(hWnd);
		case EM_GETTHUMB: // �õ���ֱ�������������λ��
			return DoGetThumb(hWnd,wParam,lParam);
		case EM_GETLIMITTEXT: // �õ���ǰ�༭�����ı�����
			return DoGetLimitText(hWnd,wParam,lParam);
		case EM_GETLINE: // �õ�ָ���е�����
			return DoGetLine(hWnd,wParam,lParam);
		case EM_GETLINECOUNT: //�õ��༭����������
			return DoGetLineCount(hWnd,wParam,lParam);
		case EM_GETMODIFY: // �õ���ǰ�༭�����޸ı�־
			return DoGetModify(hWnd,wParam,lParam);
		case EM_GETPASSWORDCHAR:// �õ���ǰ�༭���������ַ�
			return DoGetPasswordChar(hWnd,wParam,lParam);	
		case EM_GETSEL: // �õ��༭����ѡ������
			return DoGetSel(hWnd,wParam,lParam);	
		case EM_LINEFROMCHAR: // ����ָ�������ַ���������
			return DoLineFromChar(hWnd,wParam,lParam);
		case EM_LINEINDEX: // �õ�ָ������������Ӧ���ַ�����
			return DoLineIndex(hWnd,wParam,lParam);
		case EM_LINELENGTH: // �õ�ָ���ַ������е��ַ�����
			return DoLinelength(hWnd,wParam,lParam);
		case EM_LINESCROLL: // ��������
			return DoLineScroll(hWnd,wParam,lParam);
		case EM_POSFROMCHAR: // �õ�ָ���ַ�������
			return DoPosFromChar(hWnd,wParam,lParam);
		case EM_REPLACESEL: // ��ָ�����ַ����滻ѡ���ַ�
			return DoReplaceSel(hWnd,wParam,lParam);
		case EM_SCROLL: // ��ֱ������Ļ
			return DoScroll(hWnd,wParam,lParam);
		case EM_SCROLLCARET: // ������굽�ɼ�λ��
			return DoScrollCaret(hWnd,wParam,lParam);
		case EM_SETLIMITTEXT: // ���ñ༭�����ı�����
			return DoSetLimitText(hWnd,wParam,lParam);
		case EM_SETMODIFY: // �����µ��޸ı�־
			return DoSetModify(hWnd,wParam,lParam);
		case EM_SETPASSWORDCHAR: // �����µ������ַ�
			return DoSetPasswordChar(hWnd,wParam,lParam);
		case EM_SETREADONLY: // ���ñ༭����ֻ������
			return DoSetReadOnly(hWnd,wParam,lParam);
		case EM_SETSEL: // ����ѡ������
			return DoSetSel(hWnd,wParam,lParam);
		case EM_EMPTY: // ��ձ༭��
			return DoEmpty(hWnd,wParam,lParam);

		case EM_SEARCHSTRING: // �����ִ�
			return DoSearchString(hWnd,wParam,lParam);
// Color Control 
//		case EM_SETCOLOR:
//			return DoSetColor(hWnd,wParam,lParam);
//		case EM_GETCOLOR:
//			return DoGetColor(hWnd,wParam,lParam);
		case WM_SETCTLCOLOR	: // ���ô�����ɫ
			return DoSetColor(hWnd,wParam,lParam);
		case WM_GETCTLCOLOR	: // �õ���ǰ���ڵ���ɫ
			return DoGetColor(hWnd,wParam,lParam);


		case WM_COPY: // ����ѡ�������ַ������а�
			return DoCopy(hWnd,wParam,lParam);
		case WM_PASTE: // ճ��
			return DoPaste(hWnd,wParam,lParam);
		case WM_CUT: // ����
			return DoCut(hWnd,wParam,lParam);
		case WM_CLEAR: // ���
			return DoClear(hWnd,wParam,lParam);
		case WM_SETTEXT: // ���ñ༭���ı�
			return DoSetText(hWnd,wParam,lParam);
		case WM_GETTEXT: // �õ��༭�����ı�
			return DoGetText(hWnd,wParam,lParam);
		case WM_GETTEXTLENGTH: // �õ��༭�����ı�����
		  return DoGetTextLength(hWnd,wParam,lParam);
		case WM_PRINTCLIENT: // ���༭����ӡ��ָ�����豸
		  DoPrintClient(hWnd,wParam,lParam);
		  break;
		case WM_STYLECHANGED: // ���ڷ��ı�
		case WM_ENABLE: // ���ڸı�
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
// ������static LRESULT InitialPDAEdit(HWND hWnd,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ָ��CREATESTRUCT��ָ�룬����ΪNULL��
// ����ֵ���ɹ�����0�����򷵻�-1��
// ���������������༭�ؼ�
// ����: 
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
    dwStyle=lpcs->style;  // �õ����ڷ��

    dwStyle&=~ES_MULTIITEMEDIT;  // �������Ŀ�༭�ķ��

		SetWindowLong(hWnd,0,NULL); // ���ô�����չΪ��

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
		{  // ���ڴ�������
			// get the text limit
			if (lpPDAEditProperty->cLimited!=0) // �б༭�ı�����
				lpEditItem->cbEditLimitLen=lpPDAEditProperty->cLimited;
			else // û���趨�ı�����
				lpEditItem->cbEditLimitLen=INITIALEDITSIZE;

			// get the edit title text
			if (lpPDAEditProperty->lpTitle!=NULL) // �б���
				lpEditItem->cbTitleLen=strlen(lpPDAEditProperty->lpTitle); // �õ����ⳤ��
			else // û�б����ı����趨��ǰ����Ϊ0
				lpEditItem->cbTitleLen=0;
		}
		else
		{ // û�д������ԣ�����Ĭ�ϲ���
			lpEditItem->cbEditLimitLen=INITIALEDITSIZE;
			lpEditItem->cbTitleLen=0;
		}

		lpEditItem->cbEditBufferLen=lpEditItem->cbEditLimitLen+1; // �õ������С

		lpEditItem->cbControlBufferLen=lpEditItem->cbEditBufferLen+lpEditItem->cbTitleLen; // �õ����ƻ����С����������
		// allocate memory
		lpEditItem->lpPDAControlBuffer=(LPTSTR)malloc(lpEditItem->cbControlBufferLen); // �õ����ƻ���

		if (lpEditItem->lpPDAControlBuffer==NULL)
		{  // allocate failare then free struct EDITITEM and return FALSe
			lpEditItem->cbEditBufferLen=0;
			lpEditItem->cbControlBufferLen=0;
			free(lpEditItem); // �ͷſռ�
//			SendNotificationMsg(hWnd,EN_ERRSPACE);
		    return -1;
		}

		lpEditItem->lpPDASaveEditBuffer=(LPTSTR)malloc(lpEditItem->cbEditBufferLen);  // ���䱸�ݻ���
		if (lpEditItem->lpPDASaveEditBuffer==NULL)
		{  // allocate failare then free struct EDITITEM and return FALSe
		  lpEditItem->cbEditBufferLen=0;
		  lpEditItem->cbControlBufferLen=0;
		  free(lpEditItem->lpPDAControlBuffer);
		  free(lpEditItem);
//		  SendNotificationMsg(hWnd,EN_ERRSPACE);
		  return -1;
		}

		lpEditItem->lpPDAEditBuffer=lpEditItem->lpPDAControlBuffer+lpEditItem->cbTitleLen;  // ����༭����
		
		// clear struct EDITITEM
//		Clear(lpEditItem);
    ClearEditItemStruct(lpEditItem);  // ����༭��Ŀ�ṹ

		// read control title text
		if (lpEditItem->cbTitleLen)
		  memcpy(lpEditItem->lpPDAControlBuffer,lpPDAEditProperty->lpTitle, lpEditItem->cbTitleLen );		// Read Text Success 

		// read Window text
		if (lpcs->lpszName!=NULL)
		{  // �б༭�ı�
			lpEditItem->cbEditLen=strlen(lpcs->lpszName); // �õ��༭�ı��Ĵ�С
			//  the window text can put the default buffer ???
			if (lpEditItem->cbEditLen>lpEditItem->cbEditLimitLen)
			{ // �ı����ȴ����ı�����
				lpEditItem->cbEditLen=0;
				SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1)); // �����ı�Ϊ��
			}
			if (dwStyle&ES_PASSWORD)
			{  // ��ǰ��������ʾ
				dwStyle&=~ES_MULTILINE;  // ����ֻ���ǵ��б༭
				SetWindowLong(hWnd,GWL_STYLE,dwStyle); // �������÷��
				// read Window text
				FillPassWordChar(lpEditItem->lpPDAEditBuffer,lpEditItem->cbEditLen,lpEditItem->chPassWordWord); // �������ַ����༭�ı�
			}
			else
			{
				GetEditText(lpEditItem->lpPDAEditBuffer,lpcs->lpszName,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null' // �õ��༭�ı�
			}
			SaveEditText(lpEditItem->lpPDASaveEditBuffer,lpcs->lpszName,(int)(lpEditItem->cbEditLen+1));// ���浱ǰ�༭�ı�
		}
		else
		{ // û�б༭�ı���
			lpEditItem->cbEditLen=0;
			SaveEditText(lpEditItem->lpPDASaveEditBuffer,(LPCTSTR)"",(int)(lpEditItem->cbEditLen+1)); // �趨�༭�ı�Ϊ��
		}
		// Read Text Success

		// get edit format 
		lpEditItem->lpFormat=NULL;  // ���ñ༭��ʽΪ��
    if (dwStyle&ES_FORMAT)
		{  // ��ǰ�༭Ϊ��ʽ�༭
			if (lpPDAEditProperty != NULL && lpPDAEditProperty->lpFormat!=NULL)
			{ // ���ڸ�ʽ�༭
				dwStyle&=~ES_MULTILINE; // ��ʽ�༭ֻ���ǵ��б༭
				SetWindowLong(hWnd,GWL_STYLE,dwStyle); // ������
				nFormatLen=strlen(lpPDAEditProperty->lpFormat)+1; // �õ���ʽ�༭���ĳ���
				if (nFormatLen)
				{ 
					lpEditItem->lpFormat=(LPTSTR)malloc(nFormatLen); // �����ʽ�༭������
					if (lpEditItem->lpFormat) // ����ɹ�
						memcpy(lpEditItem->lpFormat,lpPDAEditProperty->lpFormat, nFormatLen );		// Read Text Success 
					else
						SendNotificationMsg(hWnd,EN_ERRSPACE); // ����
				}
			}
			else
			{ // �����ڸ�ʽ�༭
				dwStyle&=~ES_FORMAT; // ȡ����ʽ�༭
				SetWindowLong(hWnd,GWL_STYLE,dwStyle);
			}
		}
    lpEditItem->dwStyle=dwStyle;  // �õ��༭���
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
		// �趨��ǰ�ı�����ɫ
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

		SetPDAEditScrollPage(hWnd);  // �趨������
		lpEditItem->iStartPos=0; // �趨��ʼλ��
		GetClientRect(hWnd,&lpEditItem->rect); // �õ��ͻ�ȥ��
		SetWindowRect(hWnd,lpEditItem,FALSE); // ���ô��ڴ�С
		SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);// �趨��ֱ��������λ��
		SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH)); // �趨ˮƽ��������λ��

		// !!! Add By Jami chen in 2003.08.08
		// set default  Caret position 
		SetCaretToStart(hWnd,lpEditItem);  // Add By Jami chen in 2005.01.28
		SetPDACaretPosition(hWnd,lpEditItem,0,0);  // �趨���λ��
		// !!! Add End By Jami  chen in 2003.08.08

//		RETAILMSG(1,(TEXT("Leave InitialPDAEdit ...\r\n")));
		// initial success ,return TRUE
		return 0;
}
// **************************************************
// ������static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �������������ý��㣬����WM_SETFOCUS��Ϣ��
// ����: 
// *************************************************
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	DWORD dwStyle;
    LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // �õ��༭��Ŀ�ṹָ��
      if(lpEditItem==NULL) return FALSE;


	  CreatePDACaret(hWnd,lpEditItem);  // �������
//			dwStyle=GetWindowLong(hWnd,GWL_STYLE);
//			if (dwStyle&ES_MULTIITEMEDIT)
//			{
//				hParentWnd=GetParent(hWnd);
	  SendNotificationMsg( hWnd,EN_SETFOCUS); // ����֪ͨ��Ϣ
//			}
      SetPDACaret(hWnd);  // ���ù��λ��
      // show caret
      ShowPDACaret(hWnd); // ��ʾ���
      wParam++;
      lParam++;
			return 0;
}

// **************************************************
// ������static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ����������ɱ�����㣬����WM_KILLFOCUS��Ϣ��
// ����: 
// **************************************************
static LRESULT DoKillFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);// �õ��༭��Ŀ�ṹָ��
      if(lpEditItem==NULL) return FALSE;

// !!! Modified By jami chen in 2005.01.12 
//      ClearInvert(hWnd,lpEditItem,TRUE); // ���ѡ��
// !!! to  By Jami chen in 2005.01.12
	  if (!(lpEditItem->dwStyle & ES_NOHIDESEL ))
	  {
	      ClearInvert(hWnd,lpEditItem,TRUE); // ���ѡ��
	  }
// !!! Modified  End
	    DeletePDACaret(hWnd); // ɾ�����
	    SendNotificationMsg(hWnd,EN_KILLFOCUS);  // ����֪ͨ��Ϣ
      wParam++;
      lParam++;
    	return 0;
}
/*****************************************************************/
/*****************************************************************/
// **************************************************
// ������static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �����������ı䴰�ڴ�С������WM_SIZE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  RECT rectNew;
  int nShowPos,nShowLine;
  LPTSTR lpShowAddress;
  DWORD dwStyle;

		lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);// �õ��༭��Ŀ�ṹָ��
		if(lpEditItem==NULL) return FALSE;
		GetClientRect(hWnd,&rectNew); // �õ��ͻ�����
		lpEditItem->rect.right=rectNew.right; // �����µĴ�С
		lpEditItem->rect.bottom=rectNew.bottom; // �����µĴ�С

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
		SetPDAEditVScrollRange(hWnd,lpEditItem->nTotalLine);  // ���ô�ֱ�������ķ�Χ
		SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine);
		SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH));

		dwStyle = GetWindowLong(hWnd,GWL_STYLE);
		lpShowAddress=lpEditItem->lpCaretPosition;
		GetCoordinate(hWnd,lpEditItem,lpShowAddress,&nShowPos,&nShowLine,dwStyle&ES_MULTILINE);
		lpEditItem->nCaretx=nShowPos-lpEditItem->nDisplayx;
		lpEditItem->nCaretLine=nShowLine-lpEditItem->nDisplayLine;
		AdjustCaretInEditItem(hWnd,lpEditItem);  // �������λ��
		SetPDACaret(hWnd);
		InvalidateRect(hWnd,NULL,TRUE);
		// !!! Add End By Jami chen in 2004.08.21
		return 0;
}

#ifdef DITHERING_DEAL  // ��������
static int old_x = -1,old_y = -1;
#endif

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
	int xPos,yPos;
  LPEDITITEM lpEditItem;

		  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);// �õ��༭��Ŀ�ṹָ��
      if(lpEditItem==NULL) return FALSE;


			// Get LButton down Position
			xPos=(int)(short)LOWORD(lParam);
			yPos=(int)(short)HIWORD(lParam);

#ifdef DITHERING_DEAL  // ��������
			old_x = xPos;
			old_y = yPos;
#endif

			// set Caret position to LButton Down Position
			SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);  // ���ù��λ��

			//  is or not get facus???
			if (GetFocus()!=hWnd)  // �õ����㴰���Ƿ��ڵ�ǰ����
			{  // if not active then get focus
				SetFocus(hWnd); // ���ý��㴰��
			}

			// Clear Invert and redraw
			ClearInvert(hWnd,lpEditItem,TRUE);  // ���ѡ��
			//// set caret position
			//SetPDACaret(hWnd);

			// Capture the mouse
			if (GetCapture()!=hWnd)
				SetCapture(hWnd);

			// set Invert Area
			SetInvertArea(hWnd,lpEditItem);

      if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)  // �������λ��
			{
				// Redraw the window
//				DrawPDAEditControl(hWnd,hdc);
				InvalidateRect(hWnd,NULL,TRUE); // ��Ҫ�ػ洰��
			}

			// set caret position
			SetPDACaret(hWnd);  // ���ù��

      // show caret
      ShowPDACaret(hWnd);  // ��ʾ���
      wParam++;
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
	int xPos,yPos;
  LPEDITITEM lpEditItem;


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

		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // �õ��༭��Ŀ�ṹָ��
		      if(lpEditItem==NULL) return FALSE;


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
// ������static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- MAKELPARAM(x,y),��ǰ����λ��
// ����ֵ����
// ��������������WM_LBUTTONUP��Ϣ��
// ����: 
// **************************************************
static void DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	// Release capture 
	SetCapture(NULL);
	wParam++;
	lParam++;	
}
// **************************************************
// ������static void DoProcessChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �����������ַ����룬����WM_CHAR��Ϣ��
// ����: 
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
        SetCaretCoordinate(hWnd,lpEditItem);  // ���ù�������
      	AdjustCaretInEditItem(hWnd,lpEditItem); // �������λ��
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
				         SetCaretCoordinate(hWnd,lpEditItem);  // �趨��������
		                 AdjustCaretInEditItem(hWnd,lpEditItem); // ��������λ��
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
			{  // ��ǰ�ַ���ASCII
				lpEditItem->HalfByte=FALSE;  // ����Ҫ�ȴ�����ַ�
				if (PickCharacter(0,chCharCode,dwStyle)==FALSE) return;  // ��ǰ�ַ�����Ҫ
				if (dwStyle&ES_UPPERCASE) // Ҫ���д
					chCharCode=toupper(chCharCode); // ת��Ϊ��д
				if (dwStyle&ES_LOWERCASE) // Ҫ��Сд
					chCharCode=tolower(chCharCode); // ת��ΪСд
     		//Delete Invert Area
        if (DeleteInvert(hWnd,lpEditItem))
        {
		       SetCaretCoordinate(hWnd,lpEditItem); // �趨�������
      	   AdjustCaretInEditItem(hWnd,lpEditItem); // �������λ��
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
		lpEditItem->nCaretx=xPos-lpEditItem->nDisplayx; // �趨����λ��
		lpEditItem->nCaretLine=nLine-lpEditItem->nDisplayLine;
		if (bResult)
		{ // the insert operation success ,must the redraw the window
			// add code : if Caret out of the client area then must 
			// Reset Display Position
			if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)
			{// Ҫ��ȫ����ʾ
			    lpShowAddress=NULL;
				nShowPos=0;
				nShowLine=0;
				dwShowFlag=PE_SHOWCLIENT;
			}
			else
			{
				// get redraw start coordinate
	   		 GetCoordinate(hWnd,lpEditItem,lpShowAddress,&xPos,&nLine,dwStyle&ES_MULTILINE); // �õ����λ��
				nShowPos=xPos;  // �ӹ�꿪ʼ��ʾ
				nShowLine=nLine;
			}

      if (bDeleteInvert==TRUE)
      {  // Ҫ��ȫ����ʾ
		    lpShowAddress=NULL;
				nShowPos=0;
				nShowLine=0;
				dwShowFlag=PE_SHOWCLIENT;
      }

			SendNotificationMsg(hWnd,EN_CHANGE);

			// redraw the window
			hdc=GetDC(hWnd);
	        DrawPDAEditSpecialText(hWnd,hdc,lpEditItem,lpShowAddress,nShowPos,nShowLine,0,dwShowFlag); // ��ʾҪ��ʾ�Ĳ���
			ReleaseDC(hWnd,hdc);
			// reset thw caret
		}
		SetPDACaret(hWnd);  // �趨���
    lParam++;
}
// **************************************************
// ������static BOOL PickCharacter(CHAR HiByte,CHAR LoByte,DWORD dwStyle)
// ������
// 	IN HiByte -- ���ֽ�
// 	IN LoByte -- ���ֽ�
// 	IN dwStyle -- �༭�����
// ����ֵ����Ҫ�ַ�������TRUE�����򷵻�FALSE��
// ����������ѡ���ַ��Ƿ���ϵ�ǰ���͵ı༭����
// ����: 
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
// ������static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ���ֵ
// 	IN lParam -- ����
// ����ֵ����
// ��������������WM_KEYDOWN��Ϣ��
// ����: 
// **************************************************
static void DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;
  int nPageLine,nScrollLine,iHorzPos = MOTIONLESS;

		nVirtKey = (int) wParam;    // virtual-key code
    nPageLine=GetPageLine(hWnd);  // �õ�һҳ�ı༭����
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
			HorzMoveCaret(hWnd,lpEditItem,iHorzPos);  // ˮƽ�ƶ����
		}
//		return;
	}
    ScrollCaretInPDAEdit(hWnd,nScrollLine); // �������
    lParam++;
}
// **************************************************
// ������int GetTextHeight(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ�����ر༭�ı��߶ȡ�
// �����������ﵽ�༭�ı��߶ȡ�
// ����: 
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
  int *lpnLine,*lpxPos;
    	LPEDITITEM lpEditItem;

#ifdef _MAPPOINTER
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // ӳ��ָ��
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() ); 
#endif

        lpnLine=(int *)wParam;
        lpxPos=(int *)lParam;
		    // get struct EDITITEM data
		    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // // �õ��༭��Ŀ�ṹָ��
        if (lpEditItem==NULL) return;
				if (lpnLine)
					*lpnLine=lpEditItem->nCaretLine+lpEditItem->nDisplayLine;  // �õ���������
				if (lpxPos)
					*lpxPos=lpEditItem->nCaretx+lpEditItem->nDisplayx;  // �õ�����ˮƽλ��
}
// **************************************************
// ������static void DoSetCaretPosition(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ,������ڵ���
// 	IN lParam -- INT ,������ڵ�x����
// ����ֵ����
// �������������ù��λ�ã�����EM_SETCARETPOS��Ϣ��
// ����: 
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
        nTextHeight=GetTextHeight(hWnd);  // �õ��ı��߶�

        xPos=xCaretPos-lpEditItem->nDisplayx;  // �õ���������
        yPos=(nCaretLine-lpEditItem->nDisplayLine)*nTextHeight;

        SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos);  // ���ù�������
		AdjustCaretInEditItem(hWnd,lpEditItem); // ��������λ��

  		SetPDACaret(hWnd);  // ���ù��
}

// **************************************************
// ������static void DoSetxDisplayPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- int ,ˮƽ��ʾλ�á�
// 	IN lParam -- ����
// ����ֵ����
// ��������������ˮƽ�������ʾλ�ã�����MIEM_SETXDISPLAYPOS��Ϣ��
// ����: 
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
// ������static void SetPDAEditScrollPage(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ����
// �������������ñ༭������������һҳ��ֵ
// ����: 
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
// ������static void SetPDAEditVScrollPos(HWND hWnd,int nNewScrollPos)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nNewScrollPos -- ��������ǰ����λ�á�
// ����ֵ����
// �������������ñ༭����ֱ�������ĵ�ǰλ��
// ����: 
// **************************************************
static void SetPDAEditVScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
      if (!(dwStyle&WS_VSCROLL)) return;
      Edit_SetScrollPos(hWnd,SB_VERT,nNewScrollPos,TRUE); // ���ù�������λ��
}

// **************************************************
// ������static void SetPDAEditHScrollPos(HWND hWnd,int nNewScrollPos)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nNewScrollPos -- ��������ǰ����λ�á�
// ����ֵ��
// �������������ñ༭��ˮƽ�������ĵ�ǰλ��
// ����: 
// **************************************************
static void SetPDAEditHScrollPos(HWND hWnd,int nNewScrollPos)
{
  DWORD dwStyle;

      dwStyle=GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��
      if (!(dwStyle&WS_HSCROLL)) return; // ���ܹ���
      Edit_SetScrollPos(hWnd,SB_HORZ,nNewScrollPos,TRUE); // ���ù�������λ��
}

// **************************************************
// ������static int DoVScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD,��������
// 	IN lParam -- ����
// ����ֵ���ɹ����ع��������������򷵻�0
// ������������ֱ������Ļ������WM_VSCROLL��Ϣ��
// ����: 
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
        return VScrollWindowInPDAEdit(hWnd,nScrollLine);  // ��������
}
// **************************************************
// ������static void DoHScrollWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD,��������
// 	IN lParam -- ����
// ����ֵ����
// ����������ˮƽ������Ļ������WM_HSCROLL��Ϣ��
// ����: 
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
        HScrollWindowInPDAEdit(hWnd,nHScrollPos); // ˮƽ��������
}
// **************************************************
// ������static int GetPageLine(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ������һҳ������
// �����������õ��༭ȥһҳ��������
// ����: 
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
// ������static void ScrollCaretInPDAEdit(HWND hWnd,int nScrollLine)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nScrillLine -- ����������
// ����ֵ����
// ������������ֱ������ꡣ
// ����: 
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
// ������static int VScrollWindowInPDAEdit(HWND hWnd,int nScrollLine)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nScrillLine -- ����������
// ����ֵ����
// ������������ֱ�������ڡ�
// ����: 
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
// ������static void HScrollWindowInPDAEdit(HWND hWnd,int nHScrollPos)
// ������
// 	IN hWnd -- ���ھ��
// 	IN nScrillLine -- ˮƽ�����ĵ���
// ����ֵ����
// ����������ˮƽ�������ڡ�
// ����: 
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
// ������static void FillPassWordChar(LPTSTR lpEditBuffer,int nFillLength,TCHAR chPassWordWord)
// ������
// 	IN lpEditBuffer  -- Ҫ���Ļ���
// 	IN nFillLength -- Ҫ���ĳ���
// 	IN chPassWordWord -- Ҫ�������������ַ�
// ����ֵ����
// �������������������Ϊ�����ַ���
// ����: 
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
// ������static void ReleasePDAEdit(HWND hWnd)
// ������
// 	IN hWnd --���ھ��
// ����ֵ����
// ���������� �ͷű༭��������WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static void ReleasePDAEdit(HWND hWnd)
{
    LPEDITITEM lpEditItem;

			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
      if(lpEditItem==NULL) return ;

	    DeletePDACaret(hWnd); // ɾ�����
//	    SendNotificationMsg(hWnd,EN_KILLFOCUS);

			if (lpEditItem->lpPDASaveEditBuffer)
				free(lpEditItem->lpPDASaveEditBuffer);  // �ͷű��ݻ���
			if (lpEditItem->lpPDAControlBuffer)
				free(lpEditItem->lpPDAControlBuffer);  // �ͷſ��ƻ���
			if (lpEditItem->lpFormat)
				free(lpEditItem->lpFormat);  // �ͷŸ�ʽ����
			free(lpEditItem); // �ͷ���Ŀ�ṹ
			SetWindowLong(hWnd,0,0);

}

// **************************************************
// ������static LRESULT DoCharFromPos(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- MAKWWPARAM(X,Y),ָ��������λ��
// 	IN lParam -- ����
// ����ֵ����WORD����ָ���������ڵ��ַ�����λ�ã���WORD���ظ��ַ����ڵ��С�
// �����������õ�ָ������ַ�������EM_CHARFROMPOS��Ϣ��
// ����: 
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
      return GetCharFromPos(hWnd,lpEditItem,xPos,yPos); // �õ�λ�õ��ַ�
}

// **************************************************
// ������static LRESULT DoGetFirstVisibleLine(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ������Ƕ��б༭�����򷵻ص�һ���ɼ��е������������
// 		�ǵ��б༭�����򷵻ص�һ���ɼ��ַ���������
// �����������õ���һ���ɼ��У�����EM_GETFIRSTVISIBLELINE��Ϣ��
// ����: 
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
	  lpPosition=GetLinePosition(hWnd,lpEditItem,lpEditItem->lpPDAControlBuffer,0,FALSE,NULL);// �õ���һ���ɼ��ַ�
      return (lpPosition-lpEditItem->lpPDAControlBuffer);
}
// **************************************************
// ������static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ�༭�����ı�������
// �����������õ���ǰ�༭�����ı����ơ�
// ����: 
// **************************************************
static LRESULT DoGetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
    wParam++;
    lParam++;
	  return lpEditItem->cbEditLimitLen;  // �����ı�����
}

// **************************************************
// ������static LRESULT DoGetLine(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ָ��������
// 	IN lParam -- LPSTR ���ָ�������ݵĻ��棬�����еĵ�һ��WORD�ǻ���Ĵ�С��
// ����ֵ���ɹ�����COPY�ַ��ĸ��������򷵻�0
// �����������õ�ָ���е����ݣ�����EM_GETLINE��Ϣ��
// ����: 
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
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif

	  lpch=(LPSTR)lParam;

		lpLineAddress=GetLineAddress(hWnd,lpEditItem,nLine);  //�õ�ָ���еĵ�ַ
		nLineLen=GetLineLength(hWnd,lpEditItem,(LPCTSTR)lpLineAddress,NULL); // �õ�ָ���еĳ���
		nBufferMaxLen=MAKEWORD(*lpch,*(lpch+1)); // �õ�����Ĵ�С
		if (nLineLen>=nBufferMaxLen)
			return 0; // ����̫С
		memcpy((void *)lpch,(const void *)lpLineAddress,nLineLen); // ����ָ��������
		lpch[nLineLen]=0; // ���ý�β��
		return nLineLen;
}
// **************************************************
// ������static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ش�ֱ�������������λ��
// �����������õ���ֱ�������������λ�ã�����EM_GETTHUMB��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetThumb(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  DWORD dwStyle;

		dwStyle=GetWindowLong(hWnd,GWL_STYLE);
		if (!(dwStyle&WS_VSCROLL)) return 0;
    wParam++;
    lParam++;
		return Edit_GetScrollPos(hWnd,SB_VERT); // �õ������������λ��
}
// **************************************************
// ������static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ر༭����������
// �����������õ��༭����������������EM_GETLINECOUNT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetLineCount(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	  if(lpEditItem==NULL) return 0;
      wParam++;
      lParam++;
	  return lpEditItem->nTotalLine; // �õ��༭����������
}
// **************************************************
// ������static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ�༭�����޸ı�־��
// �����������õ���ǰ�༭�����޸ı�־������EM_GETMODIFY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return FALSE;

    wParam++;
    lParam++;
		return lpEditItem->fModified; // �õ��༭�����޸ı�־
}
// **************************************************
// ������static LRESULT DoGetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ص�ǰ�༭���������ַ�
// �����������õ���ǰ�༭���������ַ�������EM_GETPASSWORDCHAR��Ϣ��
// ����: 
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
				return lpEditItem->chPassWordWord; // �õ��༭���������ַ�
		else
				return 0;
}
// **************************************************
// ������static LRESULT DoGetSel(HWND hWnd, WPARAM wParam, LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LPDWORD �� ���ѡ���������ʼλ���ַ�����
// 	IN lParam -- LPDOWRD �� ���ѡ������Ľ���λ���ַ�����
// ����ֵ���ɹ�����MAKERESULT(START,END),���򷵻�0��
// �����������õ��༭����ѡ�����򣬴���EM_GETSEL��Ϣ��
// ����: 
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
	wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );   // ӳ��ָ��
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
			nStart=lpEditItem->lpInvertStart-lpEditItem->lpPDAEditBuffer;  // �õ�ѡ��ʼλ��
			nEnd=lpEditItem->lpInvertEnd-lpEditItem->lpPDAEditBuffer; // �õ�ѡ�����λ��
		}
		if (lpdwStart)
			*lpdwStart=(DWORD)nStart; // ����ʼλ��
		if (lpdwEnd)
			*lpdwEnd=(DWORD)nEnd; // ������λ��
// !!! Modified By Jami chen on 2003.09.12
		if (nEnd > 0xffff || nStart > 0xffff)
			lResult = -1;
		else
			lResult=MAKELRESULT(nStart,nEnd); // �õ�����ֵ
// !!! Modified End By Jami chen on 2003.09.12
		return lResult;
}

// **************************************************
// ������static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �������������ñ༭�����ı����ƣ�����EM_SETLIMITTEXT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetLimitText(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 int cchMax;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		cchMax=(int)wParam;

    return SetLimitText(hWnd,lpEditItem,cchMax);  // �����ı�����
}

// **************************************************
// ������static LRESULT DoLineFromChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- int ���ַ�����
// 	IN lParam -- ����
// ����ֵ������ָ�������ַ�����������
// ��������������ָ�������ַ���������������EM_LINEFROMCHAR��Ϣ��
// ����: 
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
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
		if (ich==-1)
		{ // ָ������Ϊ-1
				if (lpEditItem->lpInvertStart==lpEditItem->lpInvertEnd)
				{ // û��ѡ��
						lpCurPos=lpEditItem->lpCaretPosition;  // ��ǰλ��Ϊ���λ��
				}
				else
				{ // ��ѡ������
						lpCurPos=lpEditItem->lpInvertStart; // ��ǰλ��Ϊѡ��ʼλ��
				}
		}
		else
		{ // ��ָ������
				if (ich>lpEditItem->cbEditLen)
					ich=lpEditItem->cbEditLen;
				lpCurPos=lpEditItem->lpPDAEditBuffer+ich; // �õ�������ַ
		}
    GetCoordinate(hWnd,lpEditItem,lpCurPos,NULL,&nLine,dwStyle&ES_MULTILINE); // �õ���ǰλ�õ��к�
		return (LRESULT)nLine; // �����к�
}

// **************************************************
// ������static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ��ָ���е�����
// 	IN lParam -- ����
// ����ֵ������ָ������������Ӧ���ַ�����
// �����������õ�ָ������������Ӧ���ַ�����������EM_LINEINDEX��Ϣ��
// ����: 
// **************************************************
static  LRESULT DoLineIndex(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 LPTSTR lpCurPos;
	 int nLine;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		nLine=(int)wParam;  // �õ�������
		if (nLine==-1)
		{ // û��ָ��������
				nLine=lpEditItem->nCaretLine; // �趨Ϊ���λ��
		}
		lpCurPos=GetLineAddress(hWnd,lpEditItem,nLine); // �õ��е�ַ
		return (LRESULT)(lpCurPos-lpEditItem->lpPDAEditBuffer); // �õ�����
}

// **************************************************
// ������static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ָ���ַ�������
// 	IN lParam -- ����
// ����ֵ������ָ���ַ������е��ַ�����
// �����������õ�ָ���ַ������е��ַ�����������EM_LINELENGTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLinelength(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
   int ich;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		ich=(int)wParam;
    return GetLineLengthFromPos(hWnd,lpEditItem,ich);// �õ����������е��ַ�����
}
// **************************************************
// ������static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ˮƽ�����ַ���
// 	IN lParam -- INT ��ֱ��������
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������������ڣ�����EM_LINESCROLL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLineScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	DWORD dwStyle;
	int cxScroll,cyScroll;
	
		dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
		if (!(dwStyle&ES_MULTILINE))
			return FALSE; // ���Ƕ��б༭��
		cxScroll=(int)wParam;
		cyScroll=(int)lParam;
		if (cxScroll)
		{
//				cxScroll=cxScroll*HSCROLLWIDTH;
				cxScroll=cxScroll*8;   // 8 --- The Width of the Char
				HScrollWindowInPDAEdit(hWnd,cxScroll); // ˮƽ��������
		}
		if (cyScroll)  
				VScrollWindowInPDAEdit(hWnd,cyScroll);  // ��ֱ��������
		return TRUE;
}

// **************************************************
// ������static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ָ�����ַ�����
// 	IN lParam -- ����
// ����ֵ������ָ���ַ�������,MAKERESULT(x,y)
// �����������õ�ָ���ַ������꣬����EM_POSFROMCHAR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoPosFromChar(HWND hWnd, WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 int wCharIndex;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;
		wCharIndex=(int)wParam;
    return GetPosFromChar(hWnd,lpEditItem,wCharIndex);  // �õ�ָ���ַ����ڵ�λ��
}
// **************************************************
// ������static LRESULT DoReplaceSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR ,ָ�����ַ���
// ����ֵ����
// ������������ָ�����ַ����滻ѡ���ַ���
// ����: 
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
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		lpReplace=(LPCTSTR)lParam; // �õ��滻�ı�
		DeleteInvert(hWnd,lpEditItem); // ɾ��ѡ���ı�
		cchLen=strlen(lpReplace); // �õ��滻�ı��ĳ���
		InsertString(hWnd,lpEditItem,lpReplace,cchLen); // �����滻�ı�
		SetCaretCoordinate(hWnd,lpEditItem); // ���ù������
		AdjustCaretInEditItem(hWnd,lpEditItem); // �������λ��
		InvalidateRect(hWnd,NULL,TRUE); // �ػ�༭��
		SetPDACaret(hWnd); // ���ù��
		return 0;
}
// **************************************************
// ������static LRESULT DoScroll(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ��������
// 	IN lParam -- ����
// ����ֵ������������
// ������������ֱ������Ļ������EM_SCROLL��Ϣ��
// ����: 
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
    	nScrollLine=DoVScrollWindow(hWnd,wParam,lParam); // ��ֱ��������
		lResult=MAKELRESULT(nScrollLine,1);
		return lResult;
}
// **************************************************
// ������static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ����������������굽�ɼ�λ�ã�����EM_SCROLLCARET��Ϣ��
// ����: 
// **************************************************
static LRESULT DoScrollCaret(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	  if(lpEditItem==NULL) return FALSE;
		if (AdjustCaretInEditItem(hWnd,lpEditItem))  // �������λ��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		return TRUE;
}
// **************************************************
// ������static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- UINT��Ҫ���õ��޸ı�־
// 	IN lParam -- ����
// ����ֵ����
// ���������������µ��޸ı�־������EM_SETMODIFY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetModify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;
	 UINT  fModified;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		fModified=(UINT)wParam;  // �õ��޸ı�־
		lpEditItem->fModified=fModified; // �趨�޸ı�־
		return 0;
}
// **************************************************
// ������static LRESULT DoSetPasswordChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- TCHAR �µ������ַ�
// 	IN lParam -- ����
// ����ֵ���ɹ�����TRUE �����򷵻�FALSE��
// ���������������µ������ַ�������EM_SETPASSWORDCHAR��Ϣ��
// ����: 
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

		ch=(TCHAR)wParam; // �õ������ַ�
    return SetPasswordChar(hWnd,lpEditItem,ch); // �趨�����ַ�
}
// **************************************************
// ������static LRESULT DoSetReadOnly(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- BOOL���Ƿ�Ҫ����Ϊֻ���ı�־
// 	IN lParam -- ����
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������ñ༭����ֻ�����ԣ�����EM_SETREADONLY��Ϣ��
// ����: 
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
			{ // ��Ҫ�趨ֻ������
				dwStyle|=ES_READONLY;
			}
			else
			{ // ����Ҫ�趨ֻ������
				dwStyle&=~ES_READONLY;
			}
      lpEditItem->dwStyle=dwStyle;  // �õ��༭���
			SetWindowLong(hWnd,GWL_STYLE,dwStyle); // ���ñ༭���
			return TRUE;
}
// **************************************************
// ������static LRESULT DoSetSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ��ѡ��������ʼλ��
// 	IN lParam -- INT ��ѡ���������λ��
// ����ֵ��
// ��������������ѡ�����򣬴���EM_SETSEL��Ϣ��
// ����: 
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

    return SetSel(hWnd,lpEditItem,nStart,nEnd);  // ����ѡ������
}
// **************************************************
// ������void SendNotificationMsg( HWND hWnd, WORD wNotifyCode)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wNotifyCode -- Ҫ���͵�֪ͨ��Ϣ
// ����ֵ����
// ��������������֪ͨ��Ϣ��
// ����: 
// **************************************************
void SendNotificationMsg( HWND hWnd, WORD wNotifyCode)
{
	WORD idEditCtrl;
	WPARAM wParam;
	HWND hParentWnd;

		idEditCtrl=(int)GetWindowLong(hWnd,GWL_ID);  // �õ��༭���ڵ�ID
		wParam=MAKEWPARAM(idEditCtrl,wNotifyCode);// ���֪ͨ��Ϣ����
		hParentWnd=GetParent(hWnd); // �õ������ھ��
		SendMessage(hParentWnd,WM_COMMAND,wParam,(LPARAM)hWnd); // ������Ϣ
}

// **************************************************
// ������static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ���ɹ����ؿ����ַ����������򷵻�0
// ��������������ѡ�������ַ������а壬����WM_COPY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoCopy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

    return CopyToClipboard(hWnd,lpEditItem); // ���Ƶ����а�
}
// **************************************************
// ������static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ��
// ����������ճ��������WM_PASTE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoPaste(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

    PasteFromClipboard(hWnd,lpEditItem);  // �Ӽ��а帴������
		AdjustCaretInEditItem(hWnd,lpEditItem); // �������λ��
		InvalidateRect(hWnd,NULL,TRUE); // ���ô�����Ч
		SetPDACaret(hWnd);  // ���ù��
    return 0;
}

// **************************************************
// ������static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �������������У�����WM_CUT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoCut(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		DoCopy(hWnd,0,0);  // ����
		DoClear(hWnd,0,0); // ���
		return 0;
}

// **************************************************
// ������static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// �������������������WM_CLEAR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoClear(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

	  // Get struct EDITITEM data
	  lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
    if(lpEditItem==NULL) return 0;

		DeleteInvert(hWnd,lpEditItem); // ɾ��ѡ���ı�
		SetCaretCoordinate(hWnd,lpEditItem); // ���ù������
		AdjustCaretInEditItem(hWnd,lpEditItem); // �������λ��
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
		SetPDACaret(hWnd); // ���ù��
		return 0;
}
// **************************************************
// ������static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTSTR Ҫ���õ��ı�
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �������������ñ༭���ı�������WM_SETTEXT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPCTSTR lpsz;
   LPEDITITEM lpEditItem;


	// Get struct EDITITEM data
	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
	if(lpEditItem==NULL) return FALSE;
	lpsz= (LPCTSTR)lParam; // address of window-text string
    if (SetText(hWnd,lpEditItem,lpsz)==FALSE) // �����ı�
	  return FALSE;
    SetPDAEditVScrollRange(hWnd,lpEditItem->nTotalLine);  // ���ô�ֱ�������ķ�Χ
	lpEditItem->nDisplayx=0;
	lpEditItem->nDisplayLine=0;
	SetEditVScrollPos(hWnd,lpEditItem->nDisplayLine); // ���ô�ֱ��������λ��
	SetEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH)); // ����ˮƽ��������λ��

	SetCaretToStart(hWnd,lpEditItem);  // Add By Jami chen in 2005.01.28

	SetPDACaretPosition(hWnd,lpEditItem,0,0); // ���ù��λ��
	AdjustCaretInEditItem(hWnd,lpEditItem); // �������λ��
	InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
	SetPDACaret(hWnd); // ���ù��
	return TRUE;
}
// **************************************************
// ������static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT ����Ĵ�С
// 	IN lParam -- LPTSTR ����ı��Ļ���
// ����ֵ���ɹ����ؿ����ַ��ĸ��������򷵻�0
// �����������õ��༭�����ı�������WM_GETTEXT��Ϣ��
// ����: 
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
			cMaxlen=(int)wParam;  // �õ�����Ĵ�С
			// !!! modified by jami chen in 2004.05.27
//			if (cMaxlen<lpEditItem->cbEditLen)
//				return FALSE;
//		    GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,(int)(lpEditItem->cbEditLen+1)); // +1 is read end code 'null'

			if (cMaxlen>lpEditItem->cbEditLen)
				cMaxlen = lpEditItem->cbEditLen + 1; // +1 is read end code 'null'
			// �õ��༭�ı�
		    cMaxlen = GetEditText(lpsz,(LPCTSTR)lpEditItem->lpPDASaveEditBuffer,cMaxlen); // is read end code 'null'
			// !!! modified by jami chen
			return cMaxlen;// lpEditItem->cbEditLen;  ����ʵ�ʸ������ݵĴ�С
}
// **************************************************
// ������static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�����ر༭�����ı����ȡ�
// �����������õ��༭�����ı����ȣ�����WM_GETTEXTLENGTH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetTextLength(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
   LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return 0;
      return lpEditItem->cbEditLen;  // �õ��ı�����
}
// **************************************************
// ������static void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen)
// ������
// 	IN lpSaveBuffer -- �������ݵĻ���
// 	IN lpBuffer -- Ҫ��������ݻ���
// 	IN cbEditLen -- Ҫ��������ݳ���
// ����ֵ����
// ��������������༭�����ı���
// ����: 
// **************************************************
//static void SaveEditText(LPTSTR lpSaveBuffer,LPCTSTR lpBuffer,int cbEditLen)
//{
//	strncpy(lpSaveBuffer,lpBuffer,cbEditLen);  // ���浱ǰ���ݵ����ݻ���
//}
// **************************************************
// ������static void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen)
// ������
// 	IN lpGetBuffer -- ��ŵõ����ݵĻ���
// 	IN lpBuffer -- ԭʼ���ݻ���
// 	IN cbEditLen -- Ҫ�õ������ݳ���
// ����ֵ����
// �����������õ��༭�����ı���
// ����: 
// **************************************************
//static void GetEditText(LPTSTR lpGetBuffer,LPCTSTR lpBuffer,int cbEditLen)
//{
//	strncpy(lpGetBuffer,lpBuffer,cbEditLen);  // ��ָ�������еõ�����
//}
// **************************************************
// ������static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ����
// ������������ձ༭��������EM_EMPTY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoEmpty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;

			// Get struct EDITITEM data
			lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);
			if(lpEditItem==NULL) return FALSE;
//			Clear(lpEditItem);
      ClearEditItemStruct(lpEditItem); // ����༭��Ŀ�ṹ

//			SetPDAEditScrollPage(hWnd);
			SetWindowRect(hWnd,lpEditItem,FALSE); // ���ڴ�С����
			SetPDAEditVScrollPos(hWnd,lpEditItem->nDisplayLine); // �趨��ֱ������
			SetPDAEditHScrollPos(hWnd,(int)((lpEditItem->nDisplayx)/HSCROLLWIDTH)); // ����ˮƽ������
			SetPDACaret(hWnd); // ���ù��
			InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
			return 0;
}

// **************************************************
// ������static LRESULT DoSearchString(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- INT Ҫ���ҵĿ�ʼλ��
// 	IN lParam -- LPCTSTR Ҫ���ҵ��ִ�
// ����ֵ���ɹ����ز��ҵ���λ�ã����򷵻�-1
// ���������������ִ�������EM_SEARCHSTRING��Ϣ��
// ����: 
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

	lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // �õ��༭��Ŀ�ṹָ��
	if (lpEditItem==NULL) 
		return -1;

#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif

	iStart = (int)wParam; // �õ���ʼ����λ��
	lpSearchString = (LPTSTR)lParam; // �õ������ִ�
	if (strlen(lpSearchString) == 0)  // �����ִ�����Ϊ0 ������
		return -1;

	if (iStart >= lpEditItem->cbEditLen)
		return -1; // ��ʼλ���Ѿ������ı���Χ
	if (iStart == -1)
	{ // �趨��ͷ��ʼ����
		lpStart = lpEditItem->lpPDAEditBuffer;
	}
	else
	{ // �趨��ָ��λ�ÿ�ʼ����
		lpStart = lpEditItem->lpPDAEditBuffer + iStart;
	}
	lpStringPos = ab_strstr(lpStart,lpSearchString); // �����ִ�
	if (lpStringPos == NULL) // û�в鵽
		return -1;
	nStart = lpStringPos - lpEditItem->lpPDAEditBuffer; // �õ��Ѿ��鵽�ִ��Ŀ�ʼλ��
	nEnd = nStart + strlen(lpSearchString); // �õ�����λ��
	SetSel(hWnd,lpEditItem,nStart,nEnd); // ���鵽���ִ�����Ϊѡ��
	lResult = GetPosFromChar(hWnd,lpEditItem,nEnd); //�õ�����λ�õ�����
	xPos = LOWORD(lResult);
	yPos = HIWORD(lResult);
	SetPDACaretPosition(hWnd,lpEditItem,xPos,yPos); // ���ù������
    if (AdjustCaretInEditItem(hWnd,lpEditItem)==TRUE)  // �������λ��
	{
		// Redraw the window
		InvalidateRect(hWnd,NULL,TRUE); // ��Ч����
	}

	// set caret position
	SetPDACaret(hWnd); // ���ù��

      // show caret
    ShowPDACaret(hWnd);  // ��ʾ���
	return nStart;
}

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
  LPEDITITEM lpEditItem;

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // �õ��༭��Ŀ�ṹָ��
   if (lpEditItem==NULL) return 0;
   DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWCLIENT);  // ��ʾ�༭�ı�
   ClearBottomBlankArea(hWnd,hdc,lpEditItem); // �������հ׵Ĳ���
   return 0;
}
// **************************************************
// ������static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- HDC ָ���豸���豸���
// 	IN lParam -- ����
// ����ֵ����
// �������������༭����ӡ��ָ�����豸������WM_PRINTCLIENT��Ϣ��
// ����: 
// **************************************************
static LRESULT DoPrintClient(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  HDC hdc;
  int nDisplayLine,nDisplayx;

   hdc=(HDC)wParam;
   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // �õ��༭��Ŀ�ṹָ��
   if (lpEditItem==NULL) return 0;
   nDisplayLine=lpEditItem->nDisplayLine;  // �õ���ʼ��ʾ������
   nDisplayx=lpEditItem->nDisplayx; // �õ���ʼ��ʾ��ˮƽλ��
   lpEditItem->nDisplayLine=0; // ���ÿ�ʼ��ʾλ��Ϊ��ʼλ��
   lpEditItem->nDisplayx=0;
   DrawPDAEditControl(hWnd,hdc,lpEditItem,PE_SHOWALL); // ����ָ�����ݵ��豸
   //ClearBottomBlankArea(hWnd,hdc,lpEditItem);
   lpEditItem->nDisplayLine=nDisplayLine;  // �ָ�ԭ������ʾλ��
   lpEditItem->nDisplayx=nDisplayx;
   return 0;
}

// **************************************************
// ������static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ�� ��
// �������������ڷ��ı䣬����WM_STYLECHANGED��Ϣ��
// ����: 
// **************************************************
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  LPEDITITEM lpEditItem;
  DWORD dwStyle;

		// get window style
   dwStyle = GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // �õ��༭��Ŀ�ṹָ��
   if (lpEditItem==NULL)
     return FALSE;
   lpEditItem->dwStyle = dwStyle;  // ���ô��ڷ��
   return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}


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
  LPEDITITEM lpEditItem;

		// get window style

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0);  // �õ��༭��Ŀ�ṹָ��
	    if (lpEditItem==NULL)
		  return FALSE;
		
		//�����趨��ǰ�ı�����ɫ
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
// ������static LRESULT DoDeleteChar(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������ɾ���ַ���
// ����: 
// **************************************************
static LRESULT DoDeleteChar(HWND hWnd)
{
  LPEDITITEM lpEditItem;
  DWORD dwStyle;

		// get window style
   dwStyle=GetWindowLong(hWnd,GWL_STYLE);  // �õ����ڷ��
   if (dwStyle&ES_READONLY) return FALSE; // ����ɾ��

   lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // �õ��༭��Ŀ�ṹָ��
   if (lpEditItem==NULL)
     return FALSE;
   DeleteChar(hWnd,lpEditItem); // ɾ��ָ���ַ�
   return TRUE;
}


// **************************************************
// ������static LRESULT DoSetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTLCOLORSTRUCT��Ҫ���õ���ɫֵ
// ����ֵ����
// �������������ô�����ɫ������WM_SETCTLCOLOR��Ϣ��
// ����: 
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
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		lpCtlColor = (LPCTLCOLORSTRUCT)lParam;  // �õ���ɫ�ṹָ��

		 if (lpCtlColor == NULL)
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // �����ı���ɫ
			 lpEditItem->cl_NormalText = lpCtlColor->cl_Text;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 { // �����ı�����ɫ
				lpEditItem->cl_NormalBkColor = lpCtlColor->cl_TextBk;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 { // ����ѡ���ı���ɫ
				lpEditItem->cl_InvertText = lpCtlColor->cl_Selection;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {  // ����ѡ���ı�����ɫ
				lpEditItem->cl_InvertBkColor = lpCtlColor->cl_SelectionBk;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 { // ����DISBALE���ı���ɫ
				lpEditItem->cl_DisableText = lpCtlColor->cl_Disable ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 { // ����DISABLE���ı�����ɫ
				lpEditItem->cl_DisableBkColor = lpCtlColor->cl_DisableBk;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 { // ����ֻ���ı���ɫ
				lpEditItem->cl_ReadOnly = lpCtlColor->cl_ReadOnly;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 { // ����ֻ���ı�����ɫ
				lpEditItem->cl_ReadOnlyBk  = lpCtlColor->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 { // ���ñ����ı���ɫ
				lpEditItem->cl_Title  = lpCtlColor->cl_Title;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {  // ���ñ����ı�����ɫ
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
		 InvalidateRect(hWnd,NULL,TRUE);  // ��Ч����
		return TRUE;
}
// **************************************************
// ������static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPCTLCOLORSTRUCT����ŵõ�����ɫֵ
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰ���ڵ���ɫ������WM_GETCTLCOLOR��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetColor(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	UINT dwColorType;
		
	LPEDITITEM lpEditItem;
	LPCTLCOLORSTRUCT lpCtlColor;

	    lpEditItem=(LPEDITITEM)GetWindowLong(hWnd,0); // �õ��༭��Ŀ�ṹָ��
		if (lpEditItem==NULL)
			return FALSE;
#ifdef _MAPPOINTER
	lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
		 lpCtlColor = (LPCTLCOLORSTRUCT)lParam;  // �õ���ɫ�ṹָ��

		 if (lpCtlColor == NULL) 
			 return FALSE;

		 if (lpCtlColor->fMask & CLF_TEXTCOLOR)
		 {  // �õ��༭�ı���ɫ
			 lpCtlColor->cl_Text = lpEditItem->cl_NormalText ;
		 }
		 if (lpCtlColor->fMask & CLF_TEXTBKCOLOR)
		 {  // �õ��༭�ı�����ɫ
				lpCtlColor->cl_TextBk = lpEditItem->cl_NormalBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONCOLOR)
		 {  // �õ�ѡ���ı���ɫ
				lpCtlColor->cl_Selection = lpEditItem->cl_InvertText ;
		 }
		 if (lpCtlColor->fMask & CLF_SELECTIONBKCOLOR)
		 {  // �õ�ѡ���ı�����ɫ
				lpCtlColor->cl_SelectionBk = lpEditItem->cl_InvertBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLECOLOR)
		 {  // �õ�DISABLE�ı���ɫ
				lpCtlColor->cl_Disable= lpEditItem->cl_DisableText ;
		 }
		 if (lpCtlColor->fMask & CLF_DISABLEBKCOLOR)
		 {  // �õ�DISABLE�ı�����ɫ
				lpCtlColor->cl_DisableBk = lpEditItem->cl_DisableBkColor ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYCOLOR)
		 {  // �õ�ֻ���ı���ɫ
				lpCtlColor->cl_ReadOnly= lpEditItem->cl_ReadOnly ;
		 }
		 if (lpCtlColor->fMask & CLF_READONLYBKCOLOR)
		 {  // �õ�ֻ���ı�����ɫ
				lpCtlColor->cl_ReadOnlyBk = lpEditItem->cl_ReadOnlyBk ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLECOLOR)
		 {  // �õ������ı���ɫ
				lpCtlColor->cl_Title= lpEditItem->cl_Title ;
		 }
		 if (lpCtlColor->fMask & CLF_TITLEBKCOLOR)
		 {  // �õ������ı�����ɫ
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
// ������static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN lpEditItem -- �༭�ؼ��ṹָ��
// ����ֵ����
// ����������������ڵײ�û�����ݵ�����
// ����: 
// **************************************************
static void ClearBottomBlankArea(HWND hWnd,HDC hdc,LPEDITITEM lpEditItem)
{
  RECT rcClientRect,rcClearRect;
  int nBottomPos;
  HBRUSH hBrush;


	nBottomPos=(int)lpEditItem->rect.bottom;
	GetClientRect(hWnd,&rcClientRect); // �õ��ͻ�����
    if (nBottomPos<rcClientRect.bottom)
    { // ��Ҫ����հ�ҳ
	    rcClearRect.left=0;
	    rcClearRect.top=nBottomPos;
	    rcClearRect.right=rcClientRect.right;
	    rcClearRect.bottom =rcClientRect.bottom;
	    // clear ClearRect
	    //FillRect(hdc,&rcClearRect,GetStockObject(WHITE_BRUSH));
		hBrush = GetBkBrush(hWnd,lpEditItem);
	    FillRect(hdc,&rcClearRect,hBrush);  // ����հ�ҳ
		DeleteObject(hBrush);
    }
}

// **************************************************
// ������static char * ab_strstr( const char * str1, const char * str2 )
// ������
// 	IN str1 -- ԭʼ�ִ�
// 	IN str2 -- Ҫ���ҵ��ִ�
// ����ֵ�����ز��ҵĽ��
// �����������ַ�������
// ����: 
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

