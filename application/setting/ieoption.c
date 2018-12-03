/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����IEѡ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-25
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"

#define STR_IEOPTION  "IEѡ��"

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_IEOption{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_IEOption = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,110,0,0,"IEѡ��" },
    };

static LPTSTR strIEGeneralClassName = "IEGENERALCLASS";
static LPTSTR strIELinkClassName = "IELINKCLASS";

#define IDC_TAB		300

#define IDC_IEGENERAL  400
#define IDC_IELINK     401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET IEOptionSheet[] = {
	{"����","IEGENERALCLASS",IDC_IEGENERAL},
//	{"����","IELINKCLASS",IDC_IELINK},
};

/***************  �������� *****************/
static LRESULT CALLBACK IEOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoIEOptionOK(HWND hDlg);

ATOM RegisterIEGeneralWindow(HINSTANCE hInstance);
ATOM RegisterIELinkWindow(HINSTANCE hInstance);


extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoIEOption(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������IEѡ����ں���
// ����: 
// **************************************************
void DoIEOption(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

	HWND hDlg;

		hDlg = FindWindow( NULL, STR_IEOPTION ) ;  // ���Ҵ����Ƿ��Ѿ���
		if( hDlg != 0 )
		{ // �Ѿ���
			SetForegroundWindow( hDlg ) ; // ���õ�ǰ̨
			return ;
		}
//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_IEOption,hWnd, (DLGPROC)IEOptionProc); // ����IEѡ��Ի���
}

// ********************************************************************
// ������static LRESULT CALLBACK IEOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������IEѡ��Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK IEOptionProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_OK: // OK ��Ϣ
			DoIEOptionOK(hDlg);
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
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
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

		RegisterIEGeneralWindow(hInstance); // ע��IEͨ�ô�����
		RegisterIELinkWindow(hInstance); // ע��IE���Ӵ�����

		numItem = sizeof(IEOptionSheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = IEOptionSheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����TAB��Ŀ
			CreateWindow(IEOptionSheet[i].lpClassName,"",WS_CHILD,0,27,210,90,hDlg,(HMENU)IEOptionSheet[i].id,hInstance,0); // ������Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ����ĿΪ��ǰѡ��
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,IEOptionSheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ�Ĵ���
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
					ShowWindow(GetDlgItem(hWnd,IEOptionSheet[iIndex].id),SW_HIDE); // ���ض�Ӧ����
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı䣬�ı�֮��
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,IEOptionSheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ����
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoIEOptionOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ����������IEѡ��WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoIEOptionOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(IEOptionSheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,IEOptionSheet[i].id); // �õ���Ŀ��Ӧ�Ĵ��ھ��
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����OK��Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  �����桱 ����
/****************************************************************************/
#define IDC_HOMEPAGE  101
#define HOMEPAGE_MAXLEN 32

static LRESULT CALLBACK IEGeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoIEGeneralCreate(HWND hWnd);
static LRESULT DoIEGeneralOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterIEGeneralWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��IEͨ�ô��ڡ�
// ����: 
// **************************************************
static ATOM RegisterIEGeneralWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strIEGeneralClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)IEGeneralWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // �õ�����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK IEGeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������IEѡ���Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK IEGeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"IEGeneral",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoIEGeneralCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoIEGeneralOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// ������static LRESULT DoIEGeneralCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ��������������IE�������ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoIEGeneralCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	TCHAR lpHomePage[HOMEPAGE_MAXLEN];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����


	// ��ȡע���IE����ҳ��ַ
	if (regReadData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\IExplorer", "Home Page",lpHomePage, HOMEPAGE_MAXLEN,REG_SZ) == FALSE)
	{ // û���ҵ�
//		strcpy(lpHomePage,"www.mlg.com");
		strcpy(lpHomePage,"\\kingmos\\kingmos.htm"); // ����ϵͳ����ҳΪ��ҳ
	}

	// ��ҳ
	CreateWindow(classSTATIC,  "��ҳ:",WS_CHILD | WS_VISIBLE ,5,15,42,18,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,  lpHomePage ,WS_CHILD | WS_VISIBLE ,50,15,150,18,hWnd,(HMENU)IDC_HOMEPAGE,hInstance,0);

	// ����
//	CreateWindow(classSTATIC,  "����:",WS_CHILD | WS_VISIBLE ,5,30,42,18,hWnd,(HMENU)0xffff,hInstance,0);
//	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,  "www.google.com",WS_CHILD | WS_VISIBLE | ES_NUMBER,50,30,150,18,hWnd,(HMENU)102,hInstance,0);

	return 0;
}
// ********************************************************************
// ������static LRESULT DoIEGeneralOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ����������IE����WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoIEGeneralOK(HWND hDlg)
{
	TCHAR lpHomePage[HOMEPAGE_MAXLEN];
	HWND hHomepage;

		hHomepage = GetDlgItem(hDlg,IDC_HOMEPAGE); // �õ�������ҳ�Ĵ���

		GetWindowText(hHomepage , lpHomePage, HOMEPAGE_MAXLEN); // �õ���ҳ
		// д��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\IExplorer", "Home Page",lpHomePage, strlen(lpHomePage)+1,REG_SZ);
		return TRUE;
}


/****************************************************************************/
//  �����ӡ� ����
/****************************************************************************/
static LRESULT CALLBACK IELinkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoIELinkOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterIELinkWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��IE���Ӵ����ࡣ
// ����: 
// **************************************************
static ATOM RegisterIELinkWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strIELinkClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)IELinkWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // �õ�����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK IELinkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������IEѡ�����ӶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK IELinkWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			TextOut(hdc,10,10,"IELink",6);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			break;
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoIELinkOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoIELinkOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ����������IE����WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoIELinkOK(HWND hDlg)
{
		return TRUE;
}
