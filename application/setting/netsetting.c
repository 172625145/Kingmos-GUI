/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�����������ã��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-08-07
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
#define	STR_NETSETTING		"��������"
#define STR_IPADDRESS		"IP��ַ"
#define STR_SERVER			"������"

#define STR_IPFROMDHCP		"ͨ��DHCP���IP��ַ"
#define STR_IPFROMUSER		"ָ��һ��IP��ַ"

#define STR_IPADDRESS_STATIC		"IP��ַ:"
#define STR_SUBNETMASK_STATIC		"��������:"
#define STR_DEFAULTNETWAY_STATIC	"Ĭ������:"

#define STR_NETDHCPDECLARE1_STATIC	"���������DHCP,��ô����"
#define STR_NETDHCPDECLARE2_STATIC	"����ַ�����Զ�����.��Ҳ"
#define STR_NETDHCPDECLARE3_STATIC	"����ָ��DNS������."
#define STR_MASTERDNS_STATIC		"����DNS:"
#define STR_ASSISTDNS_STATIC		"����DNS:"
/*****************************************************/

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_NetSetting{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_NetSetting = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,63,210,185,0,0,STR_NETSETTING },
    };

static LPTSTR strIPAddressClassName = "NETIPADDRESS";
static LPTSTR strNetServerClassName = "NETSERVER";

#define IDC_TAB		300

#define IDC_NETIPADDRESS	500
#define IDC_NETSERVER		501

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;

static const TABSHEET NetSettingSheet[] = {
	{STR_IPADDRESS,"NETIPADDRESS",IDC_NETIPADDRESS},
	{STR_SERVER,"NETSERVER",IDC_NETSERVER},
};

/***************  �������� *****************/
static LRESULT CALLBACK NetSettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNetSettingOK(HWND hDlg);

ATOM RegisterIPAddressWindow(HINSTANCE hInstance);
ATOM RegisterNetServerWindow(HINSTANCE hInstance);

extern BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
extern BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);

/***************  ����ʵ�� *****************/

//extern ATOM RegisterIPAddressClass(HINSTANCE hInst);

// **************************************************
// ������void DoNetSetting(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ��������������������ڡ�
// ����: 
// **************************************************
void DoNetSetting(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		RegisterIPAddressClass(hInstance); // ע��IP��ַ��
//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		// �����������öԻ���
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_NetSetting,hWnd, (DLGPROC)NetSettingProc);
}

// ********************************************************************
//������static LRESULT CALLBACK NetSettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//������
//	IN hWnd - ���ھ��
//  IN message - ��Ҫ�������Ϣ
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//�����������������öԻ���Ĵ��ڴ������
//����: 
// ********************************************************************
static LRESULT CALLBACK NetSettingProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_INITDIALOG: // �Ի����ʼ����Ϣ
			return DoInitDialog(hDlg);
		case WM_CLOSE: // �رմ�����Ϣ
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK��Ϣ
			DoNetSettingOK(hDlg);
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
		// ����һ��TAB����
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);
		// ����TAB������ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterIPAddressWindow(hInstance); // ע��IP��ַ����
		RegisterNetServerWindow(hInstance); // ע��������񴰿�

		numItem = sizeof(NetSettingSheet) / sizeof(TABSHEET); // �õ���Ҫ��TABҳ
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = NetSettingSheet[i].lpName; // �õ���ҳ������
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����һ��TABҳ
			// ����һ����Ӧ�Ĵ��ڣ��������TABҳͬʱ��ʾ
			CreateWindow(NetSettingSheet[i].lpClassName,"",WS_CHILD,0,27,210,125,hDlg,(HMENU)NetSettingSheet[i].id,hInstance,0);
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰ��ѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һҳΪѡ��ҳ
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,NetSettingSheet[iIndex].id),SW_SHOW); // ���ö�Ӧ��ҳ�Ĵ��ڿɼ�
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
// ����ֵ����
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
			 case TCN_SELCHANGING: // ѡ�����ڸı䣬�ı�֮ǰ���͵���Ϣ
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ�ѡ�������
				 if (iIndex != -1)
				 { // ������������Ӧ�Ĵ�������
					ShowWindow(GetDlgItem(hWnd,NetSettingSheet[iIndex].id),SW_HIDE);
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ��ı���ɣ��ı�֮���͵���Ϣ
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ�ѡ�������
				 if (iIndex != -1)
				 { // ����������Ӧ�Ĵ�����ʾ
					ShowWindow(GetDlgItem(hWnd,NetSettingSheet[iIndex].id),SW_SHOW);
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
//������static LRESULT DoNetSettingOK(HWND hDlg)
//������
//	IN hWnd - ���ھ��
//����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
//������������������WM_OK����
//����: 
// ********************************************************************
static LRESULT DoNetSettingOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(NetSettingSheet) / sizeof(TABSHEET); // �õ��������õ�ҳ��
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,NetSettingSheet[i].id); // �õ���ҳ�Ĵ��ھ��
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����ҳ����WM_OK����Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  ��IP��ַ�� ����
/****************************************************************************/
#define IDC_IPDHCP		201
#define IDC_IPUSER		202

#define	ID_IPADDRESS			203
#define	ID_IPADDRESS_TITLE		204
#define ID_SUBNETMASK			205
#define ID_SUBNETMASK_TITLE		206
#define ID_DEFAULTNETWAY		207
#define ID_DEFAULTNETWAY_TITLE	208

static LRESULT CALLBACK IPAddressWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoIPAddressCreate(HWND hWnd);
static LRESULT DoIPAddressOK(HWND hDlg);
static LRESULT DoIPDHCP(HWND hWnd);


// **************************************************
// ������static ATOM RegisterIPAddressWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��IP��ַ���ڡ�
// ����: 
// **************************************************
static ATOM RegisterIPAddressWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strIPAddressClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)IPAddressWndProc; // ������̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
//������static LRESULT CALLBACK IPAddressWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//������
//	IN hWnd - ���ھ��
//  IN message - ��Ҫ�������Ϣ
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//������������������IP��ַ�Ի���Ĵ��ڴ������
//����: 
// ********************************************************************
static LRESULT CALLBACK IPAddressWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_IPDHCP: // DHCP������Ч
					DoIPDHCP(hWnd);
					return 0;
				case IDC_IPUSER: // �û�����IP��ַ��Ч
					DoIPDHCP(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // ����������Ϣ
			return DoIPAddressCreate(hWnd);
		case WM_DESTROY: //�ƻ�������Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoIPAddressOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
//������static LRESULT DoIPAddressCreate(HWND hWnd)
//������
//	IN hWnd - ���ھ��
//����ֵ��
//	�ɹ�����0�����򷵻�-1
//����������������IP��ַ���Ի���
//����: 
// ********************************************************************
static LRESULT DoIPAddressCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	BOOL bIpDhcpEnable = TRUE;
	TCHAR lpIPAddress[64];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����

	// �õ�DHCPѡ��Ĵ���
	hChild = CreateWindow(classBUTTON,  STR_IPFROMDHCP,WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,5,8,201,18,hWnd,(HMENU)IDC_IPDHCP,hInstance,0);

	// ��ȡע�����DHCPEnable��ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "DHCPEnable",&bIpDhcpEnable, sizeof(DWORD) ,REG_DWORD) == TRUE)
	{
	}
	SendMessage(hChild,BM_SETCHECK,bIpDhcpEnable,0); // ����DHCP�Ĵ���ѡ��


	bIpDhcpEnable = ~bIpDhcpEnable; // �û�����IP��ַ��ѡ����DHCP����
	// �õ��û�����IP��ַѡ��Ĵ���
	hChild = CreateWindow(classBUTTON,  STR_IPFROMUSER,WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,5,31,201,18,hWnd,(HMENU)IDC_IPUSER,hInstance,0);
	SendMessage(hChild,BM_SETCHECK,bIpDhcpEnable,0); // ���ô��ڵ�ѡ��
	// �����û�����IP��ַ�Ĵ���
	hChild = CreateWindow(classSTATIC,STR_IPADDRESS_STATIC,WS_CHILD | WS_VISIBLE ,5,55,72,18,hWnd,(HMENU)ID_IPADDRESS_TITLE,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,55,125,18,hWnd,(HMENU)ID_IPADDRESS,hInstance,0);

	// ��ȡIP��ַ��ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "IPAddress",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // ����IP��ַ
	}
	// �����û�������������Ĵ���
	hChild = CreateWindow(classSTATIC,STR_SUBNETMASK_STATIC,WS_CHILD | WS_VISIBLE ,5,79,72,18,hWnd,(HMENU)ID_SUBNETMASK_TITLE,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,79,125,18,hWnd,(HMENU)ID_SUBNETMASK,hInstance,0);
	// ��ȡ���������ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "SubNetmask",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // ������������
	}
	// �����û�����Ĭ�����صĴ���
	hChild = CreateWindow(classSTATIC,STR_DEFAULTNETWAY_STATIC,WS_CHILD | WS_VISIBLE ,5,103,72,18,hWnd,(HMENU)ID_DEFAULTNETWAY_TITLE,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,103,125,18,hWnd,(HMENU)ID_DEFAULTNETWAY,hInstance,0);
	// ��ȡĬ�����ص�ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "DefaultNetWay",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // ����Ĭ������
	}

	DoIPDHCP(hWnd); // ���ô��ڵ�ENABLE
	return 0;
}

// ********************************************************************
//������static LRESULT DoIPAddressOK(HWND hDlg)
//������
//	IN hWnd - ���ھ��
//����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
//����������IP��ַWM_OK����
//����: 
// ********************************************************************
static LRESULT DoIPAddressOK(HWND hDlg)
{
	TCHAR lpIPAddress[64];
	HWND hChild;
	BOOL bIpDhcpEnable;

		hChild = GetDlgItem(hDlg,IDC_IPDHCP); // �õ����ھ��
		
		bIpDhcpEnable = SendMessage(hChild,BM_GETCHECK,0,0); // �õ��Ƿ�ѡ��DHCP
		// ����ǰ��DHCP��ֵд�뵽ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "DHCPEnable",&bIpDhcpEnable, sizeof(DWORD),REG_DWORD);

		hChild = GetDlgItem(hDlg,ID_IPADDRESS); // �õ�IP��ַ�Ĵ���

		GetWindowText(hChild, lpIPAddress, 64); // �õ�IP��ַ
		// д��IP��ַ��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "IPAddress",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		hChild = GetDlgItem(hDlg,ID_SUBNETMASK); // �õ���������Ĵ���

		GetWindowText(hChild, lpIPAddress, 64); // �õ���������
		// д���������뵽ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "SubNetmask",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		hChild = GetDlgItem(hDlg,ID_DEFAULTNETWAY); // �õ�Ĭ�����صĴ���

		GetWindowText(hChild, lpIPAddress, 64); // �õ�Ĭ�����ص�ֵ
		// д��Ĭ�����ص�ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "DefaultNetWay",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);
		return TRUE;
}

// ********************************************************************
//������static LRESULT DoIPDHCP(HWND hWnd)
//������
//	IN hWnd - ���ھ��
//����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
//�������������⴦�� IDC_IPDHCP ���ܵ�����
//����: 
// ********************************************************************
static LRESULT DoIPDHCP(HWND hWnd)
{
	HWND hChild ;
	BOOL bEnable;

	hChild = GetDlgItem(hWnd,IDC_IPUSER); // �õ��û�����IP��ַ��ѡ�񴰿�
	bEnable = SendMessage(hChild,BM_GETCHECK,0,0); // �õ��Ƿ�����û�����IP��ַ
	// ��������IP��ַ����ENABLE����
	hChild = GetDlgItem(hWnd,ID_IPADDRESS_TITLE);
	EnableWindow(hChild,bEnable);
	hChild = GetDlgItem(hWnd,ID_IPADDRESS);
	EnableWindow(hChild,bEnable);
	// ���������������봰��ENABLE����
	hChild = GetDlgItem(hWnd,ID_SUBNETMASK_TITLE);
	EnableWindow(hChild,bEnable);
	hChild = GetDlgItem(hWnd,ID_SUBNETMASK);
	EnableWindow(hChild,bEnable);
	// ��������Ĭ�����ش���ENABLE����
	hChild = GetDlgItem(hWnd,ID_DEFAULTNETWAY_TITLE);
	EnableWindow(hChild,bEnable);
	hChild = GetDlgItem(hWnd,ID_DEFAULTNETWAY);
	EnableWindow(hChild,bEnable);
	return 0;
}

/****************************************************************************/
//  ������������� ����
/****************************************************************************/
#define ID_MASTERDNS   300
#define ID_ASSISTDNS   301

static LRESULT CALLBACK NetServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoNetServerCreate(HWND hWnd);
static LRESULT DoNetServerOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterNetServerWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��������񴰿ڡ�
// ����: 
// **************************************************
static ATOM RegisterNetServerWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strNetServerClassName; // ������񴰿���
   wc.lpfnWndProc	=(WNDPROC)NetServerWndProc; // ���̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
//������static LRESULT CALLBACK NetServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//������
//	IN hWnd - ���ھ��
//  IN message - ��Ҫ�������Ϣ
//  IN wParam - ��Ϣ����
//  IN lParam - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//�����������������á����������Ի���Ĵ��ڴ������
//����: 
// ********************************************************************
static LRESULT CALLBACK NetServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoNetServerCreate(hWnd);
		case WM_DESTROY: // �ƻ�����
			break;
		case WM_OK: // OK��Ϣ
			return DoNetServerOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
//������static LRESULT DoNetServerCreate(HWND hWnd)
//������
//	IN hWnd - ���ھ��
//����ֵ��
//	�ɹ�����0�����򷵻�-1
//����������������������������Ի���
//����: 
// ********************************************************************
static LRESULT DoNetServerCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	TCHAR lpIPAddress[64];

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	// ������������
	hChild = CreateWindow(classSTATIC,STR_NETDHCPDECLARE1_STATIC,WS_CHILD | WS_VISIBLE ,5,5,201,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindow(classSTATIC,STR_NETDHCPDECLARE2_STATIC,WS_CHILD | WS_VISIBLE ,5,29,201,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindow(classSTATIC,STR_NETDHCPDECLARE3_STATIC,WS_CHILD | WS_VISIBLE ,5,53,201,18,hWnd,(HMENU)0xffff,hInstance,0);
	// ��������DNS
	hChild = CreateWindow(classSTATIC,STR_MASTERDNS_STATIC,WS_CHILD | WS_VISIBLE ,5,79,72,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,79,125,18,hWnd,(HMENU)ID_MASTERDNS,hInstance,0);
	// �õ�ע���������DNS��ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "MasterDNS",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // ��������
	}
	// ��������DNS
	hChild = CreateWindow(classSTATIC,STR_ASSISTDNS_STATIC,WS_CHILD | WS_VISIBLE ,5,103,72,18,hWnd,(HMENU)0xffff,hInstance,0);
	hChild = CreateWindowEx(WS_EX_CLIENTEDGE,"IPADDRESS","",WS_CHILD | WS_VISIBLE,78,103,125,18,hWnd,(HMENU)ID_ASSISTDNS,hInstance,0);
	// �õ�ע����и���DNS��ֵ
	if (regReadData(HKEY_SOFTWARE_ROOT,
				"SoftWare\\Netware\\LAN", "AssistDNS",lpIPAddress, 64 ,REG_SZ) == TRUE)
	{
		SetWindowText(hChild,lpIPAddress); // ��������
	}

	return 0;
}
// ********************************************************************
//������static LRESULT DoNetServerOK(HWND hDlg)
//������
//	IN hWnd - ���ھ��
//����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
//�������������������WM_OK����
//����: 
// ********************************************************************
static LRESULT DoNetServerOK(HWND hDlg)
{
	TCHAR lpIPAddress[64];
	HWND hChild;

		hChild = GetDlgItem(hDlg,ID_MASTERDNS); // �õ�����DNS�Ĵ��ھ��

		GetWindowText(hChild, lpIPAddress, 64); // �õ�����
		// д��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "MasterDNS",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		hChild = GetDlgItem(hDlg,ID_ASSISTDNS); // �õ�����DNS�Ĵ��ھ��

		GetWindowText(hChild, lpIPAddress, 64); // �õ�����
		// д��ע���
		regWriteData(HKEY_SOFTWARE_ROOT,
					"SoftWare\\Netware\\LAN", "AssistDNS",lpIPAddress, strlen(lpIPAddress)+1,REG_SZ);

		return TRUE;
}

