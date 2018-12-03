/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵������Դ���ԣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-25
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <eprogres.h>


#define STR_POWERPROPERTY	"��Դ����"
/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_PowerProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_PowerProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,160,0,0,STR_POWERPROPERTY },
    };

static LPTSTR strBatteryClassName = "POWERBATTERYCLASS";
static LPTSTR strPowerOffClassName = "POWEROFFCLASS";

#define IDC_TAB		300

#define IDC_BATTERY    400
#define IDC_POWEROFF   401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET PowerPropertySheet[] = {
	{"���","POWERBATTERYCLASS",IDC_BATTERY},
	{"�ػ�","POWEROFFCLASS",IDC_POWEROFF},
};

/***************  �������� *****************/
static LRESULT CALLBACK PowerPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoPowerPropertyOK(HWND hDlg);

static ATOM RegisterBatteryWindow(HINSTANCE hInstance);
static ATOM RegisterPowerOffWindow(HINSTANCE hInstance);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoPowerProperty(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
void DoPowerProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;
	HWND hDlg;

		hDlg = FindWindow( NULL, STR_POWERPROPERTY ) ;  // ���ҵ�ǰ�����Ƿ��Ѿ�����
		if( hDlg != 0 )
		{ // �Ѿ�����
			SetForegroundWindow( hDlg ) ; // ���ô��ڵ�ǰ̨
			return ;
		}

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_PowerProperty,hWnd, (DLGPROC)PowerPropertyProc); // �����Ի���
}

// ********************************************************************
// ������static LRESULT CALLBACK PowerPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������Դ���ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK PowerPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_CLOSE: // �ر���Ϣ
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK��Ϣ
			DoPowerPropertyOK(hDlg);
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
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
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

		RegisterBatteryWindow(hInstance); // ע���ش���
		RegisterPowerOffWindow(hInstance); // ע��ػ�����

		numItem = sizeof(PowerPropertySheet) / sizeof(TABSHEET); // �õ���Ŀ��Ŀ
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = PowerPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����һ��TAB��Ŀ
			CreateWindow(PowerPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,113,hDlg,(HMENU)PowerPropertySheet[i].id,hInstance,0); // ����һ����Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ����ĿΪ��ǰѡ��
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,PowerPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ�Ĵ���
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

		hTab = GetDlgItem(hWnd,IDC_TAB);
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // ѡ�����ڸı䣬�ı�֮ǰ
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰ��ѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,PowerPropertySheet[iIndex].id),SW_HIDE); // ���ض�Ӧ�Ĵ���
				 }
				 return 0;
			 case TCN_SELCHANGE:// ѡ���Ѿ��ı䣬�ı�֮��
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,PowerPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��ǰ��Ӧ����
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoPowerPropertyOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ������������Դ����WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoPowerPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(PowerPropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,PowerPropertySheet[i].id); // �õ���Ŀ���ھ��
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����WM_OK��Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  ����ء� ����
/****************************************************************************/
static LRESULT CALLBACK BatteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoBatteryCreate(HWND hWnd);
static LRESULT DoBatteryOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterBatteryWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע���ش����ࡣ
// ����: 
// **************************************************
static ATOM RegisterBatteryWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strBatteryClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)BatteryWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK BatteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������Դ���Ե�ضԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK BatteryWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Battery",6);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoBatteryCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoBatteryOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoBatteryCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ��������������������ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoBatteryCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hProgress;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	// �������
	CreateWindow(classSTATIC,"״̬:",WS_CHILD | WS_VISIBLE ,15,20,40,18,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindow(classSTATIC,"���",WS_CHILD | WS_VISIBLE ,60,20,100,18,hWnd,(HMENU)106,hInstance,0);
	// ������ʾ��ص����Ľ�����
	hProgress = CreateWindowEx(WS_EX_CLIENTEDGE,classPROGRESS,"",WS_CHILD | WS_VISIBLE | PBS_SMOOTH,15,45,160,24,hWnd,(HMENU)107,hInstance,0);

	SendMessage(hProgress,PBM_SETRANGE,0,MAKELPARAM(0,100)); // ���÷�Χ
	SendMessage(hProgress,PBM_SETPOS,50,0); // ���õ�ǰֵ
	SendMessage(hProgress,PBM_SETBARCOLOR,0,RGB(77,166,255)); // ������ɫ
	// ����������ʾ��ص���
	CreateWindow(classSTATIC,"���ʣ�����:",WS_CHILD | WS_VISIBLE ,15,80,110,18,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindow(classSTATIC,"50%",WS_CHILD | WS_VISIBLE ,130,80,40,18,hWnd,(HMENU)108,hInstance,0);

	return 0;
}
// ********************************************************************
// ������static LRESULT DoBatteryOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �������������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoBatteryOK(HWND hDlg)
{
		return TRUE;
}


/****************************************************************************/
//  ���ػ��� ����
/****************************************************************************/

#define IDC_BATTERYENABLE	101
#define IDC_BATTERYTIME		102
#define IDC_EXTERNALENABLE	103
#define IDC_EXTERNALTIME	104

static LRESULT CALLBACK PowerOffWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoPowerOffCreate(HWND hWnd);
static LRESULT DoPowerOffOK(HWND hDlg);
static LRESULT DoBatteryEnable(HWND hWnd);
static LRESULT DoExternalEnable(HWND hWnd);

// **************************************************
// ������static ATOM RegisterPowerOffWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ���������� ע��ػ���������
// ����: 
// **************************************************
static ATOM RegisterPowerOffWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strPowerOffClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)PowerOffWndProc; // ���ڹ���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK PowerOffWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������Դ���Թػ��Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK PowerOffWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"PowerOff",8);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_BATTERYENABLE: // ���״̬�ı�
					DoBatteryEnable(hWnd);
					return 0;
				case IDC_EXTERNALENABLE: // �ⲿ��Դ״̬�ı�
					DoExternalEnable(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // ������Ϣ
			DoPowerOffCreate(hWnd);
			break;
		case WM_DESTROY: // �ƻ�������Ϣ
			break;
		case WM_OK: // OK ��Ϣ
			return DoPowerOffOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// ������static LRESULT DoPowerOffCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ���������������ػ�����
// ����: 
// ********************************************************************
static LRESULT DoPowerOffCreate(HWND hWnd)
{
	HWND hChild;
	HINSTANCE hInstance;
	CTLCOLORSTRUCT             CCS ;
	BOOL bEnable;
	int iIndex;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����

//		bEnable  = GetAutoPowerOff(USEBATTERY);
		// ��ע����ȡʹ�õ�عػ���״̬
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		// ����ʹ�õ�عػ���״̬����
		hChild = CreateWindow(classBUTTON,  "ʹ�õ��ʱϵͳ�Զ��ػ�",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,5,201,18,hWnd,(HMENU)IDC_BATTERYENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0); // ����״̬

		// ����ʹ�õ�عػ�ʱ���ѡ�񴰿�
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,30,80,68,hWnd,(HMENU)IDC_BATTERYTIME,hInstance,0);
		CCS.fMask = CLF_TEXTBKCOLOR;//|CLF_TITLECOLOR;
		// ������ɫ
		CCS.cl_TextBk = RGB( 255,255,255);
//		CCS.cl_Title = RGB( 132,132,132);
		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		// ����ѡ����
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"1 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"3 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"5 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"10 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"30 ����");
		// ��ע����ȡʹ�õ�عػ���ʱ��
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // ���õ�ǰʱ��
		EnableWindow(hChild,bEnable);

		hChild = CreateWindow(classSTATIC,"ϵͳ����          ��ػ�",WS_CHILD | WS_VISIBLE ,5,30,201,18,hWnd,(HMENU)0xffff,hInstance,0);

//		bEnable  = GetAutoPowerOff(USEEXTERNALPOWER);
		// ��ע����ж�ȡʹ���ⲿ��Դ�ػ���״̬
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			bEnable  = TRUE;
		}
		// ����ʹ���ⲿ��Դ�ػ���״̬����
		hChild = CreateWindow(classBUTTON,  "ʹ���ⲿ��Դʱ�Զ��ػ�",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,55,201,18,hWnd,(HMENU)IDC_EXTERNALENABLE,hInstance,0);
		SendMessage(hChild,BM_SETCHECK,bEnable,0); // ���õ�ǰ״ֵ̬
		// ����ʱ��ѡ�񴰿�
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,69,80,80,68,hWnd,(HMENU)IDC_EXTERNALTIME,hInstance,0);
		// ����ѡ����
		SendMessage( hChild, WM_SETCTLCOLOR, 0,(LPARAM)&CCS);
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"1 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"3 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"5 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"10 ����");
		SendMessage(hChild,CB_ADDSTRING,0,(LPARAM)"30 ����");
		// ��ȡ��ǰʱ��
		if (regReadData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD) == FALSE)
		{
			iIndex = 3;
		}
		SendMessage(hChild,CB_SETCURSEL,iIndex,0); // ���õ�ǰʱ��
		EnableWindow(hChild,bEnable);

		hChild = CreateWindow(classSTATIC,"ϵͳ����          ��ػ�",WS_CHILD | WS_VISIBLE ,5,80,201,18,hWnd,(HMENU)0xffff,hInstance,0);

		return 0;
}


// ********************************************************************
// ������static LRESULT DoPowerOffOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ػ�WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoPowerOffOK(HWND hDlg)
{
	BOOL bEnable;
	HWND hChild ;
	DWORD iIndex ;

		hChild = GetDlgItem(hDlg,IDC_BATTERYENABLE); // �õ�ʹ�õ��״̬����
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // �õ�ʹ�õ��״̬
		// д��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "Battery AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{	// �õ���ǰʱ��
			hChild = GetDlgItem(hDlg,IDC_BATTERYTIME);
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0);
			// д��ע���
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Battery\\PowerOff", "Battery AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}

		// �õ�ʹ���ⲿ��Դ��״̬
		hChild = GetDlgItem(hDlg,IDC_EXTERNALENABLE);
		bEnable = SendMessage(hChild,BM_GETCHECK,0,0);
		// д�뵽ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
			"Battery\\PowerOff", "External AutoOff",&bEnable, sizeof(DWORD),REG_DWORD);
		if (bEnable == TRUE)
		{   // �õ���ǰʱ��
			hChild = GetDlgItem(hDlg,IDC_EXTERNALTIME);
			iIndex = SendMessage(hChild,CB_GETCURSEL,0,0);
			// д�뵽ע���
			regWriteData(HKEY_SOFTWARE_ROOT,
				"Battery\\PowerOff", "External AutoOff Time",&iIndex, sizeof(DWORD),REG_DWORD);
		}

		return TRUE;
}

// ********************************************************************
// ������static LRESULT DoBatteryEnable(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ػ����� BATTERYENABLE ���ܸı�
// ����: 
// ********************************************************************
static LRESULT DoBatteryEnable(HWND hWnd)
{
	HWND hBatteryEnable ;
	BOOL bEnable;
	HWND hComboBox;
	// �õ�ʹ�õ�ص�״̬
	hBatteryEnable = GetDlgItem(hWnd,IDC_BATTERYENABLE); 
	bEnable = SendMessage(hBatteryEnable,BM_GETCHECK,0,0);
	hComboBox = GetDlgItem(hWnd,IDC_BATTERYTIME); // �õ�ʱ��ѡ�񴰿�
	EnableWindow(hComboBox,bEnable); // ����ʱ��ѡ�񴰿���õ���״̬һ��
	return 0;
}

// ********************************************************************
// ������static LRESULT DoExternalEnable(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ػ����� EXTERNALENABLE ���ܸı�
// ����: 
// ********************************************************************
static LRESULT DoExternalEnable(HWND hWnd)
{
	HWND hExternalEnable ;
	BOOL bEnable;
	HWND hComboBox;

	// �õ�ʹ�õ�ص�״̬
	hExternalEnable = GetDlgItem(hWnd,IDC_EXTERNALENABLE);
	bEnable = SendMessage(hExternalEnable,BM_GETCHECK,0,0);
	hComboBox = GetDlgItem(hWnd,IDC_EXTERNALTIME); // �õ�ʱ��ѡ�񴰿�
	EnableWindow(hComboBox,bEnable); // ������õ���״̬һ��
	return 0;
}
