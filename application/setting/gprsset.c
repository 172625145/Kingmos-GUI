/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�����������ԣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-25
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_GPRSSet{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_GPRSSet = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,120,0,0,"GPRS����" },
    };

static LPTSTR strAPNClassName = "APNCLASS";

#define IDC_TAB		300

#define IDC_APN  400

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET GPRSSetSheet[] = {
	{"APN","APNCLASS",IDC_APN},
};

/***************  �������� *****************/
static LRESULT CALLBACK GPRSSetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGPRSSetOK(HWND hDlg);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

static ATOM RegisterAPNWindow(HINSTANCE hInstance);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoGPRSSet(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������GPRS������ں�����
// ����: 
// **************************************************
void DoGPRSSet(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		// ����GPRS���öԻ���
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_GPRSSet,hWnd, (DLGPROC)GPRSSetProc);
}

// ********************************************************************
// ������static LRESULT CALLBACK GPRSSetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������GPRS���öԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK GPRSSetProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			DoGPRSSetOK(hDlg);
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

		RegisterAPNWindow(hInstance); // ע��APN������

		numItem = sizeof(GPRSSetSheet) / sizeof(TABSHEET); // �õ�TAB��Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = GPRSSetSheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����TAB��Ŀ
			CreateWindow(GPRSSetSheet[i].lpClassName,"",WS_CHILD,0,27,210,73,hDlg,(HMENU)GPRSSetSheet[i].id,hInstance,0); // ������Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // ��ǰû��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ����ĿΪ��ǰ��Ŀ
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,GPRSSetSheet[iIndex].id),SW_SHOW); // ��ʾ��ǰ��Ӧ�Ĵ���
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
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,GPRSSetSheet[iIndex].id),SW_HIDE); // ���ض�Ӧ�Ĵ���
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı䣬�ı�֮��
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,GPRSSetSheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ�Ĵ���
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoGPRSSetOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ����������GPRS����WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoGPRSSetOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(GPRSSetSheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,GPRSSetSheet[i].id); // �õ���Ŀ��Ӧ�Ĵ��ھ��
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����OK��Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  ��APN�� ����
/****************************************************************************/

#define IDC_CMNET  102
#define CMNET_MAXLEN	32

static LRESULT CALLBACK APNWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoAPNCreate(HWND hWnd);
static LRESULT DoAPNSettingOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterAPNWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��APN�����ࡣ
// ����: 
// **************************************************
static ATOM RegisterAPNWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strAPNClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)APNWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // �õ�����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK APNWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������GPRS����APN�Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK APNWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"APN",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
//			switch (wmId)
//			{
//			}	
//			break;
		case WM_CREATE: // ������Ϣ
			return DoAPNCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoAPNSettingOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoAPNCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ��������������APN���ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoAPNCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	TCHAR lpCMNet[CMNET_MAXLEN];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	// ��ע���õ�APN��ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\GPRS", "APN",lpCMNet, strlen(lpCMNet)+1,REG_SZ) == FALSE)
	{
		strcpy(lpCMNet,"cmnet");
	}
	// ��������APN�Ĵ���
	CreateWindow(classSTATIC,  "������APN(�����������������Ӧ����ѯ)",WS_CHILD | WS_VISIBLE ,5,5,200,40,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,lpCMNet,WS_CHILD | WS_VISIBLE ,5,45,200,20,hWnd,(HMENU)IDC_CMNET,hInstance,0);

	return 0;
}
// ********************************************************************
// ������static LRESULT DoAPNSettingOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ����������GPRS��APN����WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoAPNSettingOK(HWND hDlg)
{
	TCHAR lpCMNet[CMNET_MAXLEN];
	HWND hCMNet;

		hCMNet = GetDlgItem(hDlg,IDC_CMNET); // �õ�APN�Ĵ��ھ��

		GetWindowText(hCMNet, lpCMNet, CMNET_MAXLEN); // �õ���ǰ��APN
		// ����ǰ��APNд��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\GPRS", "APN",lpCMNet, strlen(lpCMNet)+1,REG_SZ);
		return TRUE;
}

