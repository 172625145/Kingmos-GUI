/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�����ʼ����ԣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-25
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include "MailBoxApi.h"
/***************  ȫ���� ���壬 ���� *****************/
#define STR_EMAILPROC "�ʼ�����"

static const struct dlgIDD_EmailProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_EmailProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,160,0,0,STR_EMAILPROC },
    };

static LPTSTR strEmailServerClassName = "EMAILSERVERCLASS";
static LPTSTR strEmailUserClassName = "EMAILUSERCLASS";
static LPTSTR strEmailSendRevClassName = "EMAILSENDREVCLASS";
static LPTSTR strEmailRulerClassName = "EMAILRULERCLASS";

CONFIGINFO g_EmailCfg;
#define IDC_TAB		300

#define IDC_EMAILSERVER   400
#define IDC_EMAILUSER     401
#define IDC_EMAILSENDREV  402
#define IDC_EMAILRULER    403

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET EmailPropertySheet[] = {
	{"������","EMAILSERVERCLASS",IDC_EMAILSERVER},
	{"�û�","EMAILUSERCLASS",IDC_EMAILUSER},
	{"�շ�","EMAILSENDREVCLASS",IDC_EMAILSENDREV},
	{"����","EMAILRULERCLASS",IDC_EMAILRULER},
};

/***************  �������� *****************/
static LRESULT CALLBACK EmailPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEmailPropertyOK(HWND hDlg);

ATOM RegisterEmailServerWindow(HINSTANCE hInstance);
ATOM RegisterEmailUserWindow(HINSTANCE hInstance);
ATOM RegisterEmailSendRevWindow(HINSTANCE hInstance);
ATOM RegisterEmailRuler(HINSTANCE hInstance);
/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoEmailProperty(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������ʼ�����������ڡ�
// ����: 
// **************************************************
void DoEmailProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;
	HWND hDlg;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		hDlg = FindWindow( NULL, STR_EMAILPROC ) ;  // ���Ҵ����Ƿ��Ѿ���
		if( hDlg != 0 )
		{ // �Ѿ���
			SetForegroundWindow( hDlg ) ; // ���õ�ǰ̨
			return ;
		}
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_EmailProperty,hWnd, (DLGPROC)EmailPropertyProc); // �����Ի���
}

// ********************************************************************
// ������static LRESULT CALLBACK EmailPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������ʼ����öԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK EmailPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_CLOSE: // �رմ���
			EndDialog(hDlg,TRUE);
			return 0;
		case WM_OK: // OK��Ϣ
			DoEmailPropertyOK(hDlg);
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
   
	
	InitEmailConfig( &g_EmailCfg );//��ʼ���ʼ�������Ϣ
	GetEmailConfig( &g_EmailCfg ); //���������Ϣ
		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // �õ�ʵ�����
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0); // ����TAB
		// ������ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterEmailServerWindow(hInstance); // ע���ʼ����񴰿�
		RegisterEmailUserWindow(hInstance); // ע���ʼ��û�����
		RegisterEmailSendRevWindow(hInstance); // ע���շ�����
        RegisterEmailRuler(hInstance);// ע���ʼ����򴰿�
		numItem = sizeof(EmailPropertySheet) / sizeof(TABSHEET); // �õ�TAB��Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = EmailPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����TAB��Ŀ
			CreateWindow(EmailPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,100,hDlg,(HMENU)EmailPropertySheet[i].id,hInstance,0); // ������Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ��TAB��ĿΪ��ǰѡ��
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,EmailPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��ǰѡ������Ӧ�Ĵ���
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

		hTab = GetDlgItem(hWnd,IDC_TAB); // �õ�TAB�Ĵ��ھ��
		switch(hdr->code)
		{
			 case TCN_SELCHANGING: // ѡ�����ڸı䣬�ı�֮ǰ
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,EmailPropertySheet[iIndex].id),SW_HIDE); // ����ѡ������Ӧ�Ĵ���
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı䣬�ı�֮��
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,EmailPropertySheet[iIndex].id),SW_SHOW); // ��ʾѡ������Ӧ�Ĵ���
				 }
				 return 0;
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoEmailPropertyOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ������������ʾ�ʼ�WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoEmailPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(EmailPropertySheet) / sizeof(TABSHEET); // �õ�TAB����Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,EmailPropertySheet[i].id); // �õ���Ŀ����Ӧ�Ĵ��ھ��
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����WM_OK��Ϣ
		}
        SaveEmailConfig( &g_EmailCfg ); // �����ʼ�����
        DeInitEmailConfig( &g_EmailCfg ); // �ͷ��ʼ�����
		return TRUE;
}


/****************************************************************************/
//  ���������� ����
/****************************************************************************/
static LRESULT CALLBACK EmailServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoServerCreate(HWND hWnd);
static LRESULT DoEmailServerOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterEmailServerWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ע����
// ����������ע���ʼ��������Ĵ���
// ����: 
// **************************************************
static ATOM RegisterEmailServerWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailServerClassName; // �ʼ��������������� 
   wc.lpfnWndProc	=(WNDPROC)EmailServerWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // �õ�����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK EmailServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������ʼ����Է������Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK EmailServerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{ 
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"EmailServer",11);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoServerCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoEmailServerOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoServerCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ���������������ʼ����������ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoServerCreate(HWND hWnd)
{
	HINSTANCE hInstance;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	//����
	CreateWindow(classSTATIC,"����(POP3):",WS_CHILD | WS_VISIBLE ,5,5,90,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,95,5,100,20,hWnd,(HMENU)101,hInstance,0);
	//����
	CreateWindow(classSTATIC,"����(SMTP):",WS_CHILD | WS_VISIBLE ,5,30,90,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,95,30,100,20,hWnd,(HMENU)102,hInstance,0);
	//�ʺ�
	CreateWindow(classSTATIC,"�ʺ�:",WS_CHILD | WS_VISIBLE ,5,55,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,50,55,145,20,hWnd,(HMENU)103,hInstance,0);
	//����
	CreateWindow(classSTATIC,"����:",WS_CHILD | WS_VISIBLE ,5,80,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE | ES_PASSWORD,50,80,145,20,hWnd,(HMENU)104,hInstance,0);
    //���ý���POP3������
	if ( g_EmailCfg.pop3server && (int)strlen(g_EmailCfg.pop3server) ) SetWindowText( GetDlgItem( hWnd,101 ),g_EmailCfg.pop3server );
    //���ý���smtp������
    if ( g_EmailCfg.smtpserver && (int)strlen(g_EmailCfg.smtpserver) ) SetWindowText( GetDlgItem( hWnd,102 ),g_EmailCfg.smtpserver );
    // ���ý����ʺ�
	if ( g_EmailCfg.account && (int)strlen(g_EmailCfg.account) )   SetWindowText( GetDlgItem( hWnd,103 ),g_EmailCfg.account );
	// ��������
    if ( g_EmailCfg.password && (int)strlen(g_EmailCfg.password))  SetWindowText( GetDlgItem( hWnd,104 ),g_EmailCfg.password );

	return 0;
}
// ********************************************************************
// ������static LRESULT DoEmailServerOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ʼ�������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoEmailServerOK(HWND hDlg)
{
	    int iLen ;
		HWND hTemp;
		
		hTemp = GetDlgItem( hDlg, 101 ); //�õ�pop3�ķ��������ھ��
		iLen = GetWindowTextLength( hTemp ); //�õ��ı�����
		free( g_EmailCfg.pop3server );// �ͷ�ԭ���ķ�������
        g_EmailCfg.pop3server = NULL;
		if ( iLen >0 )
		{
			g_EmailCfg.pop3server = malloc( iLen + 2 ); // �������µķ������Ļ���
			memset( g_EmailCfg.pop3server, 0,iLen+2 ); // �������
			GetWindowText( hTemp, g_EmailCfg.pop3server ,iLen+2); // �õ��µķ�������
		}
		
		hTemp = GetDlgItem( hDlg, 102 ); // �õ�SMTP�������Ĵ��ھ��
		iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
		free( g_EmailCfg.smtpserver );// �ͷ�ԭ���ķ�������
        g_EmailCfg.smtpserver = NULL; 
		if ( iLen >0 )
		{
			g_EmailCfg.smtpserver = malloc( iLen + 2 ); // �����µĻ���
			memset( g_EmailCfg.smtpserver, 0,iLen+2 ); // ��ջ���
			GetWindowText( hTemp, g_EmailCfg.smtpserver ,iLen+2); // �õ��µķ�������
		}

		hTemp = GetDlgItem( hDlg, 103 ); // �õ��ʺŵĴ��ھ��
		iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
		free( g_EmailCfg.account ); // �ͷ�ԭ�����ʺ�
        g_EmailCfg.account = NULL;
		if ( iLen >0 )
		{
			g_EmailCfg.account = malloc( iLen + 2 ); // �����µĻ���
			memset( g_EmailCfg.account, 0,iLen+2 ); // ��ջ���
			GetWindowText( hTemp, g_EmailCfg.account ,iLen+2); // �õ��µ��ʺ�
		}

		hTemp = GetDlgItem( hDlg, 104 ); // �õ����봰��
		iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
		free( g_EmailCfg.password ); // �ͷ�ԭ��������
        g_EmailCfg.password = NULL;
		if ( iLen >0 )
		{
			g_EmailCfg.password = malloc( iLen + 2 ); // �����µĻ���
			memset( g_EmailCfg.password, 0,iLen+2 ); // ��ջ���
			GetWindowText( hTemp, g_EmailCfg.password ,iLen+2); // �õ��µ�����
		}             
		return TRUE;
}


/****************************************************************************/
//  ���û��� ����
/****************************************************************************/
static LRESULT CALLBACK EmailUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoEmailUserCreate(HWND hWnd);
static LRESULT DoEmailUserOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterEmailUserWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ע����
// ����������ע���ʼ��û������ࡣ
// ����: 
// **************************************************
static ATOM RegisterEmailUserWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailUserClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)EmailUserWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK EmailUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������ʼ������û��Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK EmailUserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"EmailUser",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoEmailUserCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoEmailUserOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoEmailUserCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ���������������ʼ��û����ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoEmailUserCreate(HWND hWnd)
{
	HINSTANCE hInstance;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	//����
	CreateWindow(classSTATIC,"����:",WS_CHILD | WS_VISIBLE ,5,5,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,50,5,145,20,hWnd,(HMENU)101,hInstance,0);
	//��λ
	CreateWindow(classSTATIC,"��λ:",WS_CHILD | WS_VISIBLE ,5,30,42,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,50,30,145,20,hWnd,(HMENU)102,hInstance,0);
	//�ʼ���ַ
	CreateWindow(classSTATIC,"�ʼ���ַ:",WS_CHILD | WS_VISIBLE ,5,55,75,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,80,55,110,20,hWnd,(HMENU)103,hInstance,0);
	//�ظ���ַ
	CreateWindow(classSTATIC,"�ظ���ַ:",WS_CHILD | WS_VISIBLE ,5,80,75,20,hWnd,(HMENU)0xffff,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD | WS_VISIBLE ,80,80,110,20,hWnd,(HMENU)104,hInstance,0);
	// ��������
    if ( g_EmailCfg.user  && (int)strlen( g_EmailCfg.user )> 0)   SetWindowText( GetDlgItem( hWnd,101 ),g_EmailCfg.user );
    // ���õ�λ
	if ( g_EmailCfg.Company  && (int)strlen( g_EmailCfg.Company ) > 0) SetWindowText( GetDlgItem( hWnd,102 ),g_EmailCfg.Company );
	// �����ʼ���ַ
	if ( g_EmailCfg.From  && (int)strlen( g_EmailCfg.From )>0 )  SetWindowText( GetDlgItem( hWnd,103 ),g_EmailCfg.From );
	// ���ûظ���ַ
    if ( g_EmailCfg.Reply  && (int)strlen( g_EmailCfg.Reply )>0 )  SetWindowText( GetDlgItem( hWnd,104 ),g_EmailCfg.Reply );
	
	return 0;
}
// ********************************************************************
// ������static LRESULT DoEmailUserOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ʼ��û�WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoEmailUserOK(HWND hDlg)
{
	HWND hTemp ;
	int iLen;

	hTemp = GetDlgItem( hDlg, 101 ); // �õ������Ĵ��ھ��
	iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
	free( g_EmailCfg.user ); // �ͷ�ԭ��������
	g_EmailCfg.user = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.user = malloc( iLen +2 ); // �����µĻ���
		memset( g_EmailCfg.user, 0, iLen+2 ); // ��ջ���
		GetWindowText( hTemp, g_EmailCfg.user ,iLen ); // �õ��µ�����
	}

	hTemp = GetDlgItem( hDlg, 102 ); // �õ���λ�Ĵ��ھ��
	iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
	free( g_EmailCfg.Company ); // �ͷ�ԭ���ĵ�λ
	g_EmailCfg.Company = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.Company = malloc( iLen +2 ); // �����µĻ���
		memset( g_EmailCfg.Company, 0, iLen+2 ); // ��ջ���
		GetWindowText( hTemp, g_EmailCfg.Company,iLen ); // �õ��µĵ�λ
	}

	hTemp = GetDlgItem( hDlg, 103 ); // �õ��ʼ���ַ�Ĵ��ھ��
	iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
	free( g_EmailCfg.From ); // �ͷ�ԭ�����ʼ���ַ
	g_EmailCfg.From = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.From = malloc( iLen +2 ); // �����µĵ�ַ
		memset( g_EmailCfg.From, 0, iLen+2 ); // ��ջ���
		GetWindowText( hTemp, g_EmailCfg.From,iLen ); // �õ��µ��ʼ���ַ
	}

	hTemp = GetDlgItem( hDlg, 104 ); // �õ��ظ��ʼ��Ĵ��ھ��
	iLen = GetWindowTextLength( hTemp ); // �õ��ı�����
	free( g_EmailCfg.Reply ); // �ͷ�ԭ���Ļظ���ַ
	g_EmailCfg.Reply = NULL ;
	if ( iLen>0 )
	{
        g_EmailCfg.Reply = malloc( iLen +2 ); // �����µĻ���
		memset( g_EmailCfg.Reply, 0, iLen+2 ); // ��ջ���
		GetWindowText( hTemp, g_EmailCfg.Reply,iLen ); // �õ��µĻظ���ַ
	}

	return TRUE;
}

/****************************************************************************/
//  ���շ��� ����
/****************************************************************************/
#define IDC_AUTORECEIVEENABLE	101
#define IDC_AUTORECEIVETIME		102
#define IDC_UPDOWN				103
#define IDC_COMBOBOX			104
#define IDC_BACKUPNUMENABLE		105
#define IDC_DOWNLOADALLEMAIL	106

static LRESULT CALLBACK EmailSendRevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoEmailSendRevCreate(HWND hWnd);
static LRESULT DoEmailSendRevOK(HWND hDlg);
static LRESULT DoAutoReceiveEnable(HWND hWnd);
static LRESULT DoBackupNumEnable(HWND hWnd);

// **************************************************
// ������static ATOM RegisterEmailSendRevWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ�� ����ϵͳע����
// ����������ע���ʼ��շ������ࡣ
// ����: 
// **************************************************
static ATOM RegisterEmailSendRevWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) ); // ��սṹ
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailSendRevClassName; // �ʼ��շ�������
   wc.lpfnWndProc	=(WNDPROC)EmailSendRevWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK EmailSendRevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������ʼ������շ��Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK EmailSendRevWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"EmailSendRev",12);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_AUTORECEIVEENABLE: // �Զ������ʼ����ܸı�
					DoAutoReceiveEnable(hWnd);
					return 0;
				case IDC_BACKUPNUMENABLE: // �����ʼ���Ŀ�ı�
					DoBackupNumEnable(hWnd);
					return 0;
			}	
			break;
		case WM_CREATE: // ������Ϣ
			return DoEmailSendRevCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoEmailSendRevOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// ������static LRESULT DoEmailSendRevCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ���������������ʼ��շ����ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoEmailSendRevCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	BOOL bEnable;
	HWND hEdit,hUpDown,hComboBox;
	int iIndex , ibackupNum;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // ʵ�����

	// �����Զ��ʼ���ȡ��ʱ����
	if (g_EmailCfg.Interval== 0)
		bEnable = FALSE;
	else
		bEnable = TRUE;

	hChild = CreateWindow(classBUTTON,  "ÿ��",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,5,50,18,hWnd,(HMENU)IDC_AUTORECEIVEENABLE,hInstance,0);
	// ��������ʱ�����Ĵ���	
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,  "10",WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_READONLY,60,5,20,18,hWnd,(HMENU)IDC_AUTORECEIVETIME,hInstance,0);
	SendMessage(hEdit,EM_SETLIMITTEXT,2,0);
	// �������´���
	hUpDown = CreateWindow(classUPDOWN32,  "",WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT,80,5,12,18,hWnd,(HMENU)IDC_UPDOWN,hInstance,0);
	// ����UODOWN�ķ�Χ
	SendMessage(hUpDown,UDM_SETRANGE,0,MAKELPARAM(30,5));
	SendMessage(hUpDown,UDM_SETPOS,11,0);
	SendMessage(hUpDown,UDM_SETBUDDY,(WPARAM)hEdit,0);
	
	EnableWindow(hEdit,bEnable);
	EnableWindow(hUpDown,bEnable);

	CreateWindow(classSTATIC,  "������ȡ�ʼ�",WS_CHILD | WS_VISIBLE ,95,5,100,18,hWnd,(HMENU)0xffff,hInstance,0);

	// �����ʼ�
//	bEnable = GetEmailDownLoadAll();
	bEnable = g_EmailCfg.bDownAll;
	hChild = CreateWindow(classBUTTON,  "���������ʼ�(��ѡ����,",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,30,200,18,hWnd,(HMENU)IDC_DOWNLOADALLEMAIL,hInstance,0);
	
	CreateWindow(classSTATIC,  "ϵͳĬ��ֻ��ȡ���ʼ�)",WS_CHILD | WS_VISIBLE ,20,55,185,18,hWnd,(HMENU)0XFFFF,hInstance,0);

	// �ʼ�����
//	bEnable = GetEmailBackup();
	if (g_EmailCfg.iSaveNum== 0)
		bEnable = FALSE;
	else
		bEnable = TRUE;
	bEnable = TRUE;
	hChild = CreateWindow(classBUTTON,  "����",WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,5,80,50,18,hWnd,(HMENU)IDC_BACKUPNUMENABLE,hInstance,0);
	
	hComboBox = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,  "",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,58,80,40,68,hWnd,(HMENU)IDC_COMBOBOX,hInstance,0);
	// ���ñ����ʼ���Ŀ�Ŀ�ѡ��
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"1��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"2��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"3��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"4��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"5��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"6��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"7��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"8��");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"9��");
	
	//ibackupNum = GetEmailBackupNum();
	ibackupNum = 5; // ���ó�ʼ��������Ŀ

	iIndex = ibackupNum - 1;

	SendMessage(hComboBox,CB_SETCURSEL,iIndex,0);
	
	CreateWindow(classSTATIC,  "���͹����ʼ�",WS_CHILD | WS_VISIBLE ,100,80,100,18,hWnd,(HMENU)0xffff,hInstance,0);
    bEnable = FALSE;
	if ( g_EmailCfg.Interval )
	{
		CHAR szText[20];
		bEnable = TRUE;
		sprintf(szText,"%d",g_EmailCfg.Interval );
		SetWindowText( GetDlgItem(hWnd,IDC_AUTORECEIVETIME),szText ); // �����Զ��շ���ʱ��
	}	
	SendMessage(GetDlgItem( hWnd,IDC_AUTORECEIVEENABLE ),BM_SETCHECK,bEnable,0);  // �����Զ��շ���״̬
	EnableWindow(GetDlgItem( hWnd,IDC_UPDOWN) ,bEnable);
	
	if ( g_EmailCfg.bDownAll )
		SendMessage(GetDlgItem( hWnd,IDC_DOWNLOADALLEMAIL ),BM_SETCHECK,TRUE,0); // �����Զ������ʼ���״̬
    
	bEnable = FALSE;
	if ( g_EmailCfg.iSaveNum )
	{
		SendMessage( GetDlgItem ( hWnd,IDC_COMBOBOX ),CB_SETCURSEL,g_EmailCfg.iSaveNum-1 ,NULL ); // ���ñ����ʼ�����Ŀ
		bEnable = TRUE;
		SendMessage(GetDlgItem( hWnd,IDC_BACKUPNUMENABLE ),BM_SETCHECK,bEnable,0); // �����ʼ����ݵ�״̬
	}
	EnableWindow(GetDlgItem( hWnd,IDC_COMBOBOX) ,bEnable);
    	

	return 0;
}
// ********************************************************************
// ������static LRESULT DoEmailSendRevOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ʼ��շ�WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoEmailSendRevOK(HWND hDlg)
{   
  	CHAR szText[5];
	HWND hTemp;
    int i ;
    hTemp = GetDlgItem( hDlg, IDC_AUTORECEIVEENABLE ); // �õ��Զ��շ���״̬
	
	if (SendMessage( hTemp , BM_GETCHECK ,0,0 ) ){
		memset( szText, 0, 5 );
        GetWindowText( GetDlgItem( hDlg, IDC_AUTORECEIVETIME),szText,4); // �õ��Զ��շ���ʱ��
	    g_EmailCfg.Interval = atoi( szText );	 // ����ʱ��
	}
	else {
	    g_EmailCfg.Interval = 0;
	}
    
	if ( SendMessage( GetDlgItem( hDlg, IDC_DOWNLOADALLEMAIL),BM_GETCHECK ,0,0 ) ) // �õ�����ȫ�������ʼ���״̬
	{
	    g_EmailCfg.bDownAll = TRUE;
	}
	else
		g_EmailCfg.bDownAll = FALSE;

     hTemp = GetDlgItem( hDlg, IDC_BACKUPNUMENABLE ); // �õ������ʼ���Ŀ��״̬�Ĵ���
	
	if (SendMessage( hTemp , BM_GETCHECK ,0,0 ) ) // �õ������ʼ���Ŀ��״̬
	{
		if ( (i = (int)SendMessage(GetDlgItem(hDlg,IDC_COMBOBOX),CB_GETCURSEL,0,0 )) != CB_ERR ) // �õ��ʼ��ı�����Ŀ
			g_EmailCfg.iSaveNum = i+1; // ������Ŀ
	}
	else {
	    g_EmailCfg.iSaveNum = 0;
	}

		return TRUE;
}


// ********************************************************************
// ������static LRESULT DoAutoReceiveEnable(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ʼ��շ�����AUTORECEIVEENABLE ���ܸı�
// ����: 
// ********************************************************************
static LRESULT DoAutoReceiveEnable(HWND hWnd)
{
	HWND hAutoReceiveEnable ;
	BOOL bEnable;
	HWND hEdit, hUpDown;

	hAutoReceiveEnable = GetDlgItem(hWnd,IDC_AUTORECEIVEENABLE); // �õ��Զ��շ��ʼ���״̬����
	bEnable = SendMessage(hAutoReceiveEnable,BM_GETCHECK,0,0); // �õ��Զ��շ��ʼ���״̬
	hEdit = GetDlgItem(hWnd,IDC_AUTORECEIVETIME); // �õ�����ʱ��Ĵ���
	hUpDown = GetDlgItem(hWnd,IDC_UPDOWN); // �õ�UODOWN�Ĵ���
	
	EnableWindow(hEdit,bEnable); // ���ý���ʱ��Ĵ����������Զ��շ��ʼ���״̬һ��
	EnableWindow(hUpDown,bEnable); // ����UODOWN�Ĵ����������Զ��շ��ʼ���״̬һ��
	return 0;
}


// ********************************************************************
// ������static LRESULT DoBackupNumEnable(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// �����������ʼ��շ����� BACKUPNUMENABLE ���ܸı�
// ����: 
// ********************************************************************
static LRESULT DoBackupNumEnable(HWND hWnd)
{
	HWND hBackupNumEnable ;
	BOOL bEnable;
	HWND hComboBox;

	hBackupNumEnable = GetDlgItem(hWnd,IDC_BACKUPNUMENABLE); // �õ������ʼ���Ŀ��״̬����
	bEnable = SendMessage(hBackupNumEnable,BM_GETCHECK,0,0);  // �õ������ʼ���Ŀ��״̬
	hComboBox = GetDlgItem(hWnd,IDC_COMBOBOX); // �õ���Ͽ�Ĵ���
	EnableWindow(hComboBox,bEnable); // ������Ͽ�Ĵ��������뱸���ʼ���Ŀ��״̬һ��
	return 0;
}
//================================= �ʼ�����=========================================
#define IDC_SERVERSAVE   101
#define IDC_SIZELIMIT    102
#define IDC_SIZESELITEM  103

// **************************************************
// ������static LRESULT DoEmailRulerCreate( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����0�����򷵻�-1
// ���������������ʼ����򴰿ڡ�
// ����: 
// **************************************************
static LRESULT DoEmailRulerCreate( HWND hWnd )
{
    HINSTANCE hInstance;
	HWND hComboBox;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	// ���������������ʼ����ݵ�״̬����
	CreateWindow( "button","�������б����ʼ�����",WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX,
		          5,5,200,18,hWnd,(HMENU)IDC_SERVERSAVE,hInstance,NULL);
	// ���������ʼ����ص�ѡ�񴰿�
	CreateWindow( "button","����",WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX,
		          5,30,50,20,hWnd,(HMENU)IDC_SIZELIMIT,hInstance,NULL );
	hComboBox = CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,  "",WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,56,30,50,70,hWnd,(HMENU)IDC_SIZESELITEM,hInstance,0);
	CreateWindow( "STATIC","���ʼ���ȫ",WS_VISIBLE|WS_CHILD,110,30,100,20,hWnd,NULL,
		          hInstance,NULL );
	CreateWindow( "STATIC","����,ֻ�����ʼ�ͷ��Ϣ",WS_VISIBLE|WS_CHILD,10,50,200,20,hWnd,NULL,
		          hInstance,NULL );
	// ���÷����������ʼ����ݵ�״̬
	if ( g_EmailCfg.bDel )
		SendMessage( GetDlgItem( hWnd,IDC_SERVERSAVE ),BM_SETCHECK,FALSE ,0);
	else
		SendMessage( GetDlgItem( hWnd,IDC_SERVERSAVE ),BM_SETCHECK,TRUE ,0);
	// ��������ѡ��Ĵ�С
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"200k");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"500k");
	SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)"1m");
	SendMessage(hComboBox,CB_SETCURSEL,0,0); // ���õ�ǰѡ��
	
	// �������ƴ�С��״̬
	if ( g_EmailCfg.iSizeLimit == 0 )
	{ // û������
	   SendMessage(GetDlgItem( hWnd,IDC_SIZELIMIT ),BM_SETCHECK,FALSE,0);
	   EnableWindow( hComboBox,TRUE );
	}
	else
	{ // ������
  	 SendMessage(GetDlgItem( hWnd,IDC_SIZELIMIT),BM_SETCHECK,TRUE,0); // ����������
	   EnableWindow(hComboBox,TRUE);
	   SendMessage( hComboBox,CB_SETCURSEL,(g_EmailCfg.iSizeLimit-1)%3,0); // �������ƴ�С
	}
	return 0;
}
// **************************************************
// ������static BOOL DoEmailRulerOK( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������OK��Ϣ��
// ����: 
// **************************************************
static BOOL DoEmailRulerOK( HWND hWnd )
{
	int i =0;
	if (SendMessage( GetDlgItem( hWnd,IDC_SERVERSAVE) , BM_GETCHECK ,0,0 ) ) // �õ������������ʼ����ݵ�״̬
	{ // Ҫ����
		g_EmailCfg.bDel = FALSE;
	}
	else 
	{ // ��Ҫ����
	    g_EmailCfg.bDel = TRUE;
	}
	if ( SendMessage( GetDlgItem( hWnd,IDC_SIZELIMIT) , BM_GETCHECK ,0,0 ) ) // �õ������ʼ���С��״̬
	{  // Ҫ����
	   	if ( (i = (int)SendMessage(GetDlgItem(hWnd,IDC_SIZESELITEM ),CB_GETCURSEL,0,0 )) != CB_ERR ) // �õ����ƴ�С
			g_EmailCfg.iSizeLimit = i+1; // ��������
	}
	else 
	{ // ��Ҫ����
	    g_EmailCfg.iSizeLimit = 0;
	}
	
	return TRUE;
}
// ********************************************************************
// ������static LRESULT CALLBACK EmailRulerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������ʼ�����Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK EmailRulerWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"EmailUser",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoEmailRulerCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoEmailRulerOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// **************************************************
// ������static ATOM RegisterEmailRuler(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ�� ����ϵͳע����
// ����������ע���ʼ����򴰿��ࡣ
// ����: 
// **************************************************
static ATOM RegisterEmailRuler(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strEmailRulerClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)EmailRulerWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}
//================================= �ʼ�����=========================================