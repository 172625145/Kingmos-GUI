/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����IP��ַ���봰����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-08-08
���ߣ�
�޸ļ�¼��
******************************************************/

#include <eWindows.h>
#include <eassert.h>
//#include <IPAddress.h>

#define ID_EDIT  230

#define MAX_SUBITEMLEN  4
/*
typedef struct IPAddressClassStruct
{
}IPADDRESSCLASS,* LPIPADDRESSCLASS;
*/

static const char classIPADDRESS[]="IPADDRESS";

//#define _MAPPOINTER

static LRESULT WINAPI IPAddressEditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);


static LRESULT WINAPI IPAddressWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static VOID DoPaint(HWND hWnd,HDC hdc);
static void DoEditNotify(HWND hWnd,HWND hEdit,int wmEvent);
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam);
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static void ToNextEdit(HWND hWnd,HWND hEdit);
static BOOL GetSubItemtext(LPTSTR lpsz,int iIndex,LPTSTR lpSubItemText,WORD wMaxlen);

// **************************************************
// ������ATOM RegisterIPAddressClass(HINSTANCE hInst)
// ������
// 	IN hInst -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��IP��ַ�ࡣ
// ����: 
// **************************************************
ATOM RegisterIPAddressClass(HINSTANCE hInst)
{
   WNDCLASS wc;
// register IPAddress class
   wc.hInstance=hInst;
   wc.lpszClassName= classIPADDRESS; // ��������
// the proc is class function
   wc.lpfnWndProc=(WNDPROC)IPAddressWndProc; // ���ڹ��̺���
   wc.style=0;
   wc.hIcon= 0;
// at pen window, no cursor
   wc.hCursor= LoadCursor(NULL,IDC_ARROW);
// to auto erase background, must set a valid brush
// if 0, you must erase background yourself
   wc.hbrBackground = GetStockObject( WHITE_BRUSH ); // ����ˢ

   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
// !!! it's important to save state , align to long
   wc.cbWndExtra=sizeof( long );
   return RegisterClass(&wc); // ע����
}
// this all message handle center
// **************************************************
// ������LRESULT WINAPI IPAddressWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN message -- ��Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ��������Ϣ�Ĵ�����
// ����������IP��ַ�ര�ڹ��̺�����
// ����: 
// **************************************************
LRESULT WINAPI IPAddressWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	    case WM_PAINT: // ������Ϣ
			 // Paint this window
			 hdc = BeginPaint(hWnd, &ps);  // Get hdc
			 DoPaint(hWnd,hdc);
			 EndPaint(hWnd, &ps);  //release hdc
           break;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case ID_EDIT: // �༭��֪ͨ��Ϣ
				case ID_EDIT + 1:
				case ID_EDIT + 2:
				case ID_EDIT + 3:
					DoEditNotify(hWnd,(HWND)lParam,wmEvent);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_SETTEXT:
			return DoSetText(hWnd,wParam,lParam); // �����ı�
		case WM_GETTEXT:
			return DoGetText(hWnd,wParam,lParam); // �õ��ı�
		case WM_STYLECHANGED:
			return DoStyleChanged(hWnd,wParam,lParam); // ���ı�
  		case WM_CREATE: // ������Ϣ
           return DoCreate(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ���Ϣ
			return DoDestroyWindow(hWnd);
      default:
	         // It's important to do this
	         // if your do'nt handle message, you must call DefwindowProc
	         return DefWindowProc(hWnd,message,wParam,lParam);
	}
  return 0;
}

// ********************************************************************
// ������static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0���������������ڣ�����-1������ƻ�����
// ����������Ӧ�ó���������Ϣ
// ����: 
// ********************************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rectClient;
	HINSTANCE hInstance;
	HWND hEdit;
	DWORD dwWndProc;
	int i;
	int iCharWidth;
	int iEditPos = 0;
	HDC hdc;

	hdc = GetDC(hWnd);
	// get ASCII code width
	GetCharWidth(hdc,0x41,0x41,&iCharWidth);

	ReleaseDC(hWnd,hdc);

	GetClientRect(hWnd,&rectClient); // �õ��ͻ�����

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����

	// ����4������IP��ַ�ı༭��
	for (i = 0; i < 4; i++)
	{   // �����༭��
		iEditPos = 4 * iCharWidth * i;
		hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,iEditPos,0,iCharWidth * 3+ 3,rectClient.bottom,hWnd,(HMENU)(ID_EDIT + i),hInstance,0);
		SendMessage(hEdit,EM_SETLIMITTEXT,3,0); // �����ı����ƣ����3���ַ���255��
		dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC); // �õ�ԭ���ı༭���ڹ��̺���
		SetWindowLong(hEdit,GWL_USERDATA,dwWndProc); // ����ԭ���Ĺ��̺������û�����
		SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc); // �����µĹ��̺���
	}
/*
	hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,32,0,27,rectClient.bottom,hWnd,ID_EDIT + 1,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,3,0);
	dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC);
	SetWindowLong(hEdit,GWL_USERDATA,dwWndProc);
	SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc);
	
	hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,64,0,27,rectClient.bottom,hWnd,ID_EDIT + 2,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,3,0);
	dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC);
	SetWindowLong(hEdit,GWL_USERDATA,dwWndProc);
	SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc);
	
	hEdit = CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|ES_NUMBER|ES_RIGHT,96,0,27,rectClient.bottom,hWnd,ID_EDIT + 3,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,3,0);
	dwWndProc = GetWindowLong(hEdit,GWL_WNDPROC);
	SetWindowLong(hEdit,GWL_USERDATA,dwWndProc);
	SetWindowLong(hEdit,GWL_WNDPROC,(DWORD)IPAddressEditWndProc);
*/
    return 0;
}
// ********************************************************************
// ������static LRESULT DoDestroyWindow(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	����TRUE���������ƻ����ڣ�����FALSE�����ƻ�����
// ����������Ӧ�ó������ƻ����ڵ���Ϣ
// ����: 
// ********************************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
	return TRUE;
}
// ********************************************************************
// ������static VOID DoPaint(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN hdc  - �豸���
// ����ֵ��
//	
// ����������Ӧ�ó����� WM_PAINT ��Ϣ
// ����: 
// ********************************************************************
static VOID DoPaint(HWND hWnd,HDC hdc)
{
  BOOL bEnable;
  int iDotPos = 0;
  int iCharWidth;
		// get ASCII code width
		GetCharWidth(hdc,0x41,0x41,&iCharWidth);
		// get window style
		bEnable = IsWindowEnabled(hWnd);
		SetBkColor(hdc,GetSysColor(COLOR_WINDOW)); // ���ñ�����ɫ
		if (bEnable == FALSE)
			SetTextColor(hdc,GetSysColor(COLOR_GRAYTEXT)); // ������ɫ

		// ���������ָ���
		iDotPos += 3 * iCharWidth + 1;
		TextOut(hdc,iDotPos,0 ,".",1);
		iDotPos += 4 * iCharWidth;
		TextOut(hdc,iDotPos,0 ,".",1);
		iDotPos += 4 * iCharWidth;
		TextOut(hdc,iDotPos,0 ,".",1);
}

// ********************************************************************
// ������static void ToNextEdit(HWND hWnd,HWND hEdit)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN hEdit  - ��ǰ��ı༭��
// ����ֵ��
//	
// ������������������һ���༭��
// ����: 
// ********************************************************************
static void ToNextEdit(HWND hWnd,HWND hEdit)
{
	UINT idCur;
	HWND hNewEdit;

		idCur = GetWindowLong(hEdit,GWL_ID); // �õ���ǰ�༭���ID��

		if (idCur == ID_EDIT + 3)
		{
			// Had Last Edit
			return ;
		}
		idCur ++ ; // ����һ���༭��
		hNewEdit = GetDlgItem(hWnd,idCur); // �õ����ھ��
		SetFocus(hNewEdit); // ���ý���
}

// ********************************************************************
// ������static void DoEditNotify(HWND hWnd,HWND hEdit,int wmEvent)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN hEdit  - ��ǰ��ı༭��
//    IN wmEvent - �༭�����ص�֪ͨ����
// ����ֵ��
//	
// ��������������༭����֪ͨ��Ϣ
// ����: 
// ********************************************************************
static void DoEditNotify(HWND hWnd,HWND hEdit,int wmEvent)
{
	if (wmEvent == EN_MAXTEXT)
	{ // �Ѿ�����
		ToNextEdit(hWnd,hEdit); // ������һ���༭��
	}
}

// ********************************************************************
// ������static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	
// ��������������WM_SETTEXT��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
	LPCTSTR lpsz;
	TCHAR lpSubItemText[MAX_SUBITEMLEN];
	HWND hEdit;
	int i;

		lpsz= (LPCTSTR)lParam; // address of window-text string
		for (i = 0; i < 4; i++)
		{
			if (GetSubItemtext((LPTSTR)lpsz,i,lpSubItemText,MAX_SUBITEMLEN) == FALSE) // �õ�ÿ���Ӵ��ڵ��ִ�
				break;
			hEdit = GetDlgItem(hWnd,ID_EDIT + i); // �õ���Ӧ�Ĵ��ھ��
			SetWindowText(hEdit,lpSubItemText); // ��������
		}
		return TRUE;
}
// ********************************************************************
// ������static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	
// ��������������WM_SETTEXT��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoGetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
{
	LPTSTR lpsz;
//	WORD wMaxLen;
	TCHAR lpSubItemText[MAX_SUBITEMLEN];
	HWND hEdit;
	int i;

		lpsz= (LPTSTR)lParam; // address of window-text string
		for (i = 0; i < 4; i++)
		{
			if (i != 0)
			{
				strcat(lpsz,(const char *)"."); // ��ӷָ���
			}
			hEdit = GetDlgItem(hWnd,ID_EDIT + i); // �õ�ָ���༭��Ĵ��ھ��
			GetWindowText(hEdit,lpSubItemText,MAX_SUBITEMLEN); // �õ�����
			strcat(lpsz,lpSubItemText); // �ӵ��û��ִ���
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoSetText(HWND hWnd , WPARAM wParam ,LPARAM lParam)
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	
// ��������������WM_SETTEXT��Ϣ
// ����: 
// ********************************************************************
static BOOL GetSubItemtext(LPTSTR lpsz,int iIndex,LPTSTR lpSubItemText,WORD wMaxlen)
{
	int iCurIndex =0;
	int iCurLen = 0;
	
	// �õ�ָ���������ı���ʼλ��
	while(iCurIndex != iIndex)
	{
		if (*lpsz == '.') // �Ƿָ���
			iCurIndex ++; // ����һ������
		lpsz ++;
	}
	while(1)
	{
		if (iCurLen >= wMaxlen -1)
			break; // �Ƿ�����
		if (*lpsz >'9' || *lpsz < '0')
			break; // �Ƿ�����
		*lpSubItemText++ = *lpsz++; // �õ�����
		iCurLen ++;
	}
	*lpSubItemText = 0;
	return TRUE;
}

// **************************************************
// ������static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ����
// ���������������ڷ��ı����Ϣ��
// ����: 
// **************************************************
static LRESULT DoStyleChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
  BOOL bEnable;
	HWND hEdit;
	int i;

		// get window style
		bEnable = IsWindowEnabled(hWnd); // �õ����ڵ�״̬


		for (i = 0; i < 4; i++)
		{
			hEdit = GetDlgItem(hWnd,ID_EDIT + i);
			EnableWindow(hEdit,bEnable); // ��ÿ���༭���״̬���óɵ�ǰ���ڵ�״̬
		}

		return DefWindowProc(hWnd, WM_STYLECHANGED, wParam, lParam);
}


// **************************************************
// ������static LRESULT DoEditKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ����
// �����������༭���������������Ϣ��
// ����: 
// **************************************************
static LRESULT DoEditKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;

		nVirtKey = (int) wParam;    // virtual-key code
		//RETAILMSG(1,(" Get Keydown = %x \r\n",nVirtKey));
		if (nVirtKey == VK_PERIOD)
		{ // ��ǰ�������һ����
			ToNextEdit(GetParent(hWnd),hWnd); // ������һ���༭��
		}
		return 0;
}

// **************************************************
// ������static LRESULT WINAPI IPAddressEditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN message -- ��Ϣ����
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ��������Ϣ������
// ����������IP��ַ����༭ȥ���̺�����
// ����: 
// **************************************************
static LRESULT WINAPI IPAddressEditWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	WNDPROC lpPrevWndFunc;
	lpPrevWndFunc = (WNDPROC)GetWindowLong(hWnd,GWL_USERDATA);
	switch( message )
	{
		case WM_KEYDOWN: // �������������Ϣ
			DoEditKeyDown(hWnd,wParam,lParam); 
			break;
	}
	return CallWindowProc(lpPrevWndFunc,hWnd,message,wParam,lParam); // ����ԭ���Ĵ��ڹ��̺���
}
/**********************************************************************************/
