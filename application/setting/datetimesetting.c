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
#include "eDateSel.h"
#include "ecomdate.h"

/*****************************************************/
// �ִ�������
/*****************************************************/
#define	STR_DATETIMEPROC	"����ʱ������"
#define STR_DATETIME		"����/ʱ��"
#define STR_TIME			"ʱ��:"
/*****************************************************/

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_DateTimeProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_DateTimeProperty = {
    { 
		WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,40,210,240,0,0,STR_DATETIMEPROC },
    };

static LPTSTR strDateTimeClassName = "DATETIMESETCLASS";

#define IDC_TAB		300

#define IDC_DATETIME  400

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET DateTimePropertySheet[] = {
	{STR_DATETIME,"DATETIMESETCLASS",IDC_DATETIME},
};

/***************  �������� *****************/
static LRESULT CALLBACK DateTimePropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoDateTimeOK(HWND hDlg);

static ATOM RegisterDateTimeWindow(HINSTANCE hInstance);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoDateTimeProperty(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ��������������/ʱ��������ں�����
// ����: 
// **************************************************
void DoDateTimeProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;
	HWND hDlg;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		hDlg = FindWindow( NULL, STR_DATETIMEPROC ) ;  // ���Ҵ����Ƿ��Ѿ���
		if( hDlg != 0 )
		{ // �Ѿ���
			SetForegroundWindow( hDlg ) ; // ���õ�ǰ̨
			return ;
		}
		// ��������ʱ�����ԶԻ���
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_DateTimeProperty,hWnd, (DLGPROC)DateTimePropertyProc);
}

// ********************************************************************
// ������static LRESULT CALLBACK DateTimePropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ��������������ʱ�����ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK DateTimePropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			DoDateTimeOK(hDlg);
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

		hInstance = (HINSTANCE)GetWindowLong(hDlg,GWL_HINSTANCE); // �õ�����ʵ�����
		// ����TAB����
		hTab = CreateWindow(WC_TABCONTROL,"",WS_CHILD|WS_VISIBLE,0,0,210,26,hDlg,(HMENU)IDC_TAB,hInstance,0);

		// ������ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterDateTimeWindow(hInstance); // ע������ʱ�䴰����

		numItem = sizeof(DateTimePropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = DateTimePropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����һ����Ŀ
			CreateWindow(DateTimePropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,193,hDlg,(HMENU)DateTimePropertySheet[i].id,hInstance,0); // ������Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ����ĿΪ��ǰѡ��
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,DateTimePropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ�Ĵ���
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ������Ϣ
// 	IN lParam -- ������Ϣ
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
					ShowWindow(GetDlgItem(hWnd,DateTimePropertySheet[iIndex].id),SW_HIDE); // ���ض�Ӧ�Ĵ���
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı�
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,DateTimePropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ�Ĵ���
				 }
				 return 0;
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoDateTimeOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ��������������ʱ������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoDateTimeOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(DateTimePropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,DateTimePropertySheet[i].id); // �õ���Ŀ��Ӧ�Ĵ��ھ��
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����OK��Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  ������ʱ�䡱 ����
/****************************************************************************/
#define IDC_TIME  200
#define IDC_DATE  201

static LRESULT CALLBACK DateTimeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoDateTimeCreate(HWND hWnd);
static LRESULT DoSetDateTimeOK(HWND hWnd);

// **************************************************
// ������static ATOM RegisterDateTimeWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע������ʱ�䴰���ࡣ
// ����: 
// **************************************************
static ATOM RegisterDateTimeWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strDateTimeClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)DateTimeWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // �õ�����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK DateTimeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ��������������ʱ����������ʱ��Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK DateTimeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:// ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"date time",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoDateTimeCreate(hWnd);

		case WM_OK: // OK��Ϣ
			return DoSetDateTimeOK(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoDateTimeCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ����������������������/ʱ��Ի���
// ����: 
// ********************************************************************
static LRESULT DoDateTimeCreate(HWND hWnd)
{
   HINSTANCE hInstance;
   HWND hChild;
   SYSTEMTIME systemTime;
   CTLCOLORSTRUCT stCtlColor;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		// ����һ������ѡ�����
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classDATESEL,"",WS_CHILD|WS_VISIBLE,15,8,
			180,140,hWnd,(HMENU)IDC_DATE,hInstance,NULL);
		
		// ������ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_TITLECOLOR | CLF_TITLEBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(255,255,255);
		stCtlColor.cl_Title = RGB(50,50,50);
		stCtlColor.cl_TitleBk = RGB(133,166,255);//RGB(255,255,255);
		stCtlColor.cl_Selection = RGB(255,255,255);
		stCtlColor.cl_SelectionBk = RGB(77,166,255);
		SendMessage(hChild,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		GetLocalTime(&systemTime); // �õ���ǰʱ��

		SendMessage(hChild,DSM_SETDATETIME,0,(LPARAM)&systemTime); // ���õ�ǰʱ��
		// ����ʱ����
		CreateWindow(classSTATIC,STR_TIME,WS_CHILD | WS_VISIBLE ,10,159,40,20,hWnd,(HMENU)0xffff,hInstance,0);
		// ����ʱ��༭����
		hChild = CreateWindowEx(WS_EX_CLIENTEDGE,classEDITTIME,"",WS_CHILD|WS_VISIBLE|ET_RUNCLOCK,50,159,
			100,20,hWnd,(HMENU)IDC_TIME,hInstance,NULL);		
		// ����ʱ����Ƶ���ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(255,255,255);
		stCtlColor.cl_Selection = RGB(255,255,255);
		stCtlColor.cl_SelectionBk = RGB(77,166,255);
		SendMessage(hChild,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);


		SendMessage(hChild,ETM_SETTIME,0,(LPARAM)&systemTime); // ���õ�ǰʱ��


		return 0;
}

// **************************************************
// ������static LRESULT DoSetDateTimeOK(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ�� ��Ч
// ������������������ʱ��OK��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetDateTimeOK(HWND hWnd)
{
   SYSTEMTIME systemTime;
   SYSTEMTIME NewTime;
   HWND hChild;


		hChild = GetDlgItem(hWnd,IDC_DATE); // �õ����ڴ��ھ��
		SendMessage(hChild,DSM_GETDATETIME,0,(LPARAM)&NewTime); // �õ���ǰ����

		hChild = GetDlgItem(hWnd,IDC_TIME); // �õ�ʱ�䴰�ھ��
		SendMessage(hChild,ETM_GETTIME,0,(LPARAM)&systemTime); // �õ���ǰʱ��

		NewTime.wHour = systemTime.wHour;
		NewTime.wMinute = systemTime.wMinute;
		SetLocalTime(&NewTime); // �����µĵ�ǰʱ��
		return 0;
}
