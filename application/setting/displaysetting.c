/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������ʾ���ԣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-25
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <SliderCtrl.h>

/*****************************************************/
// �ִ�������
/*****************************************************/
#define STR_DISPLAYPROC			"��ʾ����"
#define STR_BRIGHT				"����"
#define STR_BACKLIGHT			"����"
#define STR_ADJUST				"�ƶ��ֱ�,�ɽ������ȵ���"
#define STR_DARK				"��"
#define STR_LIGHT				"��"
#define STR_LIGHTOFF_BATTERY	"ʹ�õ��ʱ�Զ��رձ���"
#define STR_MINUTE_1			"1 ����"
#define STR_MINUTE_3			"3 ����"
#define STR_MINUTE_5			"5 ����"
#define STR_MINUTE_10			"10 ����"
#define STR_MINUTE_30			"30 ����"
#define STR_LIGHTOFF_UNUSED		"ϵͳ����          ��ر�"
#define STR_LIGHTOFF_EXTERN		"ʹ���ⲿ��Դʱ�رձ���"
/*****************************************************/

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_DisplayProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_DisplayProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,160,0,0,STR_DISPLAYPROC },
    };

static LPTSTR strBrightClassName = "BRIGHTCLASS";
static LPTSTR strBackLightClassName = "BACKLIGHTCLASS";

#define IDC_TAB		300

#define IDC_BRIGHT  400
#define IDC_BACKLIGHT  401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET DisplayPropertySheet[] = {
	{STR_BRIGHT,"BRIGHTCLASS",IDC_BRIGHT},
	{STR_BACKLIGHT,"BACKLIGHTCLASS",IDC_BACKLIGHT},
};

/***************  �������� *****************/
static LRESULT CALLBACK DisplayPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDisplayPropertyOK(HWND hDlg);

ATOM RegisterBrightWindow(HINSTANCE hInstance);
ATOM RegisterBackLightWindow(HINSTANCE hInstance);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoDisplayProperty(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ������������ʾ������ں�����
// ����: 
// **************************************************
void DoDisplayProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_DisplayProperty,hWnd, (DLGPROC)DisplayPropertyProc); // �����Ի���
}

// ********************************************************************
// ������static LRESULT CALLBACK DisplayPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������ʾ���ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK DisplayPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
/*			switch (wmId)
			{
				case IDC_OK:
					EndDialog(hDlg,TRUE);
					return 0;
			}	
			*/
			return 0;
		case WM_NOTIFY: // ֪ͨ��Ϣ
			return DoNotify(hDlg,wParam,lParam);
		case WM_INITDIALOG: // ��ʼ���Ի���
			return DoInitDialog(hDlg);
		case WM_CLOSE: // �رմ�����Ϣ
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK��Ϣ
			DoDisplayPropertyOK(hDlg);
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}

// **************************************************
// ������static LRESULT DoInitDialog(HWND hDlg)
// ������
// 	IN hDlg -- ���ھ��
// 
// ����ֵ���ɹ�����0�����򷵻�-1
// ������������ʼ���Ի���
// ����: 
// **************************************************
static LRESULT DoInitDialog(HWND hDlg)
{
	HINSTANCE hInstance;
	HWND hTab;
	TCITEM tcItem;
	int numItem;
	int i,iIndex;
	CTLCOLORSTRUCT stCtlColor;

		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // �õ�ʵ�����
		// ����TAB����
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);
		// ������ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterBrightWindow(hInstance);  // ע�����ȴ�����
		RegisterBackLightWindow(hInstance); // ע�ᱳ�ⴰ����

		numItem = sizeof(DisplayPropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = DisplayPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����һ��TAB��Ŀ
			CreateWindow(DisplayPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,100,hDlg,(HMENU)DisplayPropertySheet[i].id,hInstance,0); // ����һ������Ŀ��صĴ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0);  // ���õ�һ����ĿΪѡ����Ŀ
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,DisplayPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ŀ��Ӧ�Ĵ���
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ��������Ϣ������
// ��������������֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	NMHDR   *hdr=(NMHDR   *)lParam;    
	int iIndex;
	HWND hTab;

		hTab = GetDlgItem(hWnd,IDC_TAB); // �õ�TAB���ھ��
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // ѡ�����ڸı䣬�ı�֮ǰ
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ�ѡ����Ŀ����
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,DisplayPropertySheet[iIndex].id),SW_HIDE); // ������Ŀ��Ӧ����
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı䣬�ı�֮��
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ�ѡ����Ŀ����
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,DisplayPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ŀ��Ӧ����
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoDisplayPropertyOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ������������ʾ����WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoDisplayPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(DisplayPropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,DisplayPropertySheet[i].id); // �õ���Ŀ��Ӧ�Ĵ���
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����WM_OK��Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  �����ȡ� ����
/****************************************************************************/
#define IDC_SLIDER   201

static LRESULT CALLBACK BrightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoPaintBright(HWND hWnd,HDC hdc);
static LRESULT DoBrightCreate(HWND hWnd);
static LRESULT DoBrightOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterBrightWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע�����ȴ����ࡣ
// ����: 
// **************************************************
static ATOM RegisterBrightWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strBrightClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)BrightWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK BrightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������ʾ�������ȶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK BrightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;
//	HWND hSlider;
//	HICON hIcon;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Bright",6);
			DoPaintBright(hWnd,hdc);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoBrightCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoBrightOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// **************************************************
// ������static void DoPaintBright(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ����
// �����������������ȴ��ڡ�
// ����: 
// **************************************************
static void DoPaintBright(HWND hWnd,HDC hdc)
{
/*
	HPEN hPen;
	int i;

		SetBkMode( hdc, TRANSPARENT ) ;
		TextOut(hdc,10,13,"�ƶ��ֱ�,�ɽ������ȵ���",23);
		TextOut(hdc,14,48,"��",2);
		TextOut(hdc,170,48,"��",2);
*/
/*		Rectangle(hdc,10,43,180,63);
		for (i=80;i<=255;i++)
		{
			hPen = CreatePen(PS_SOLID,5,RGB(i,i,i));
			hPen = (HPEN)SelectObject(hdc,hPen);
			MoveToEx(hdc,(i-80)+11,45,NULL);
			LineTo(hdc,(i-80)+11,62);
			hPen = (HPEN)SelectObject(hdc,hPen);
			DeleteObject(hPen);
		}
*/
}
// ********************************************************************
// ������static LRESULT DoBrightCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ���������������������Դ���
// ����: 
// ********************************************************************
static LRESULT DoBrightCreate(HWND hWnd)
{
	HWND hSlider;
	HICON hIcon;
	HINSTANCE hInstance;
	int iBrightLevel = 0 ,iBrightLevelNum = 0;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		// �������ȹ�����
		hSlider = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|WS_VISIBLE | TBS_AUTOTICKS |TBS_HORZ |TBS_TOP,30,45,140,50,hWnd,(HMENU)IDC_SLIDER,hInstance,0);
		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_ARROW ), IMAGE_ICON,16, 16, 0 ) ;
		SendMessage(hSlider,TBM_SETTHUMBICON,MAKELONG(16,16),(LPARAM)hIcon);

		// Get Bright level Num
		iBrightLevelNum = 7;
		SendMessage(hSlider,TBM_SETRANGE,FALSE,MAKELONG(0,iBrightLevelNum -1));

		// GetCurrent Bright Level

		if (regReadData(HKEY_SOFTWARE_ROOT,
				"Display\\Bright", "Bright",&iBrightLevel, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iBrightLevel = 3;
		}
		SendMessage(hSlider,TBM_SETPOS,TRUE,iBrightLevel); // ���õ�ǰ������

		SendMessage(hSlider,TBM_SETPAGESIZE,0,1); // ����һ��PAGE�Ĵ�С
		
		// ������Ǵ���
		CreateWindow(classSTATIC,STR_ADJUST,WS_CHILD | WS_VISIBLE ,10,13,190,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,STR_DARK,WS_CHILD | WS_VISIBLE ,14,48,18,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,STR_LIGHT,WS_CHILD | WS_VISIBLE ,170,48,18,18,hWnd,(HMENU)0xffff,hInstance,0);

		return 0;
}
// ********************************************************************
// ������static LRESULT DoBrightOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ������������ʾ��������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoBrightOK(HWND hDlg)
{
	HWND hSlider;
	int iBrightLevel = 0;

		hSlider = GetDlgItem(hDlg,IDC_SLIDER); // �õ�����Ĵ��ھ��
		if (hSlider)
		{
			iBrightLevel = SendMessage(hSlider,TBM_GETPOS,0,0); // �õ���ǰ������

			// Set Bright
			regWriteData(HKEY_SOFTWARE_ROOT,
					"Display\\Bright", "Bright",&iBrightLevel, sizeof(DWORD),REG_DWORD);
		}
		return TRUE;
}


/****************************************************************************/
//  �����⡱ ����
/****************************************************************************/
#define IDC_BATTERYENABLE	101
#define IDC_BATTERYTIME		102
#define IDC_EXTERNALENABLE	103
#define IDC_EXTERNALTIME	104

static LRESULT CALLBACK BackLightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoBackLightCreate(HWND hWnd);
static LRESULT DoBackLightOK(HWND hDlg);
static LRESULT DoBatteryEnable(HWND hWnd);
static LRESULT DoExternalEnable(HWND hWnd);

// **************************************************
// ������static ATOM RegisterBackLightWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע�ᱳ�ⴰ���ࡣ
// ����: 
// **************************************************
static ATOM RegisterBackLightWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strBackLightClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)BackLightWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK BackLightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������ʾ���Ա���Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK BackLightWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"BackLight",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_BATTERYENABLE: // ʹ�õ�ص�״̬�ı�
					DoBatteryEnable(hWnd);
					return 0;
				case IDC_EXTERNALENABLE: // ʹ���ⲿ��Դ��״̬�ı�
					DoExternalEnable(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // ������Ϣ
			return DoBackLightCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			DoBackLightOK(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoBackLightCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// �����������������ⴰ��
// ����: 
// ********************************************************************
static LRESULT DoBackLightCreate(HWND hWnd)
{
	HWND hChild;
	HINSTANCE hInstance;
	CTLCOLORSTRUCT             CCS ;
	BOOL bEnable;
	int iIndex;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����

//		bEnable  = GetAutoBackLightOff(USEBATTERY);
		// �õ�ʹ�õ���Զ��رյ�״̬
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		hChild = CreateWindow(classBUTTON,  STR_LIGHTOFF_BATTERY,WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,5,201,18,hWnd,(HMENU)IDC_BATTERYENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0); // ����״̬
		// ����ѡ��ʹ�õ�عر�ϵͳ��ʱ��Ĵ���
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,30,80,68,hWnd,(HMENU)IDC_BATTERYTIME,hInstance,0);
		// ������ɫ
		CCS.fMask = CLF_TEXTBKCOLOR;//|CLF_TITLECOLOR;

		CCS.cl_TextBk = RGB( 255,255,255);
//		CCS.cl_Title = RGB( 132,132,132);
//		CCS.cl_Title = RGB( 0,0,0);
		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		// ����ѡ����
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_1);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_3);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_5);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_10);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_30);
		// �õ�ע����ʱ������
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // ���õ�ǰѡ��
		EnableWindow(hChild,bEnable);

		// ��������رյĴ���
		hChild = CreateWindow(classSTATIC,STR_LIGHTOFF_UNUSED,WS_CHILD | WS_VISIBLE ,5,30,201,18,hWnd,(HMENU)0xffff,hInstance,0);

//		bEnable  = GetAutoBackLightOff(USEEXTERNAL);
		// ��ȡ����رյ�״̬
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		hChild = CreateWindow(classBUTTON,  STR_LIGHTOFF_EXTERN,WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,55,201,18,hWnd,(HMENU)IDC_EXTERNALENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0);

		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,80,80,68,hWnd,(HMENU)IDC_EXTERNALTIME,hInstance,0);

		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		// ����ʱ���ѡ��
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_1);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_3);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_5);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_10);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)STR_MINUTE_30);
		// ��ȡ��ǰ��ʱ������
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // ���õ�ǰ��ѡ��
		EnableWindow(hChild,bEnable);

		hChild = CreateWindow(classSTATIC,STR_LIGHTOFF_UNUSED,WS_CHILD | WS_VISIBLE ,5,80,201,18,hWnd,(HMENU)0xffff,hInstance,0);

		return 0;
}
// ********************************************************************
// ������static LRESULT DoBackLightOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ������������ʾ���Ա���WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoBackLightOK(HWND hDlg)
{
	BOOL bEnable;
	HWND hChild ;
	DWORD iIndex ;

		hChild = GetDlgItem(hDlg,IDC_BATTERYENABLE); // �õ�����״̬�Ĵ���
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // �õ�״̬
		// д��ע���ǰ��״̬
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{
			hChild = GetDlgItem(hDlg,IDC_BATTERYTIME); // �õ�ʱ��Ĵ��ھ��
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0); // �õ���ǰ��ʱ��ѡ��
			// д��ע����ǰ��ʱ������
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Display\\Back light", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}


		hChild = GetDlgItem(hDlg,IDC_EXTERNALENABLE); // �õ�ʹ���ⲿ״̬�Ĵ���
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // �õ���ǰ״̬
		// д�뵱ǰ��״̬��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Display\\Back light", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{
			hChild = GetDlgItem(hDlg,IDC_EXTERNALTIME); // �õ�ʹ���ⲿ��Դ��ʱ��Ĵ��ھ��
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0); // �õ���ǰ��ʱ��ѡ��
			// д�뵱ǰ��ʱ��ѡ��ע���
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Display\\Back light", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}

		return TRUE;
}

// ********************************************************************
// ������static LRESULT DoBatteryEnable(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �������������⴦�� BATTERYENABLE ���ܸı�
// ����: 
// ********************************************************************
static LRESULT DoBatteryEnable(HWND hWnd)
{
	HWND hBatteryEnable ;
	BOOL bEnable;
	HWND hComboBox;

	hBatteryEnable = GetDlgItem(hWnd,IDC_BATTERYENABLE); // �õ�ʹ�õ�ص�״̬����
	bEnable = SendMessage(hBatteryEnable,BM_GETCHECK,0,0); // �õ�״̬
	hComboBox = GetDlgItem(hWnd,IDC_BATTERYTIME); // �õ�ѡ��ʱ��Ĵ���
	EnableWindow(hComboBox,bEnable); // ����״̬
	return 0;
}

// ********************************************************************
// ������static LRESULT DoExternalEnable(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �������������⴦�� EXTERNALENABLE ���ܸı�
// ����: 
// ********************************************************************
static LRESULT DoExternalEnable(HWND hWnd)
{
	HWND hExternalEnable ;
	BOOL bEnable;
	HWND hComboBox;

	hExternalEnable = GetDlgItem(hWnd,IDC_EXTERNALENABLE); // �õ��ⲿʹ���ⲿ��Դ��״̬����
	bEnable = SendMessage(hExternalEnable,BM_GETCHECK,0,0); // �õ�״̬
	hComboBox = GetDlgItem(hWnd,IDC_EXTERNALTIME); // �õ�ѡ��ʱ��Ĵ���
	EnableWindow(hComboBox,bEnable); // ����״̬
	return 0;
}
