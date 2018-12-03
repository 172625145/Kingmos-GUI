/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ϵͳ���ԣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-24
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include "resource.h"
#include <eprogres.h>

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_SystemProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_SystemProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,70,210,185,0,0,"ϵͳ����" },
    };

static LPTSTR strGeneralClassName = "GENERALCLASS";
static LPTSTR strVersionClassName = "VERSIONCLASS";

#define IDC_TAB		300

#define IDC_GENERAL  400
#define IDC_VERSION  401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET SystemPropertySheet[] = {
	{"����","GENERALCLASS",IDC_GENERAL},
	{"��Ȩ","VERSIONCLASS",IDC_VERSION},
};

/***************  �������� *****************/
static LRESULT CALLBACK SystemPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);

ATOM RegisterGeneralWindow(HINSTANCE hInstance);
ATOM RegisterVersionWindow(HINSTANCE hInstance);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoSystemProperty(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������ϵͳ������ں�����
// ����: 
// **************************************************
void DoSystemProperty(HINSTANCE hInstance,HWND hWnd)
{
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_SystemProperty,hWnd, (DLGPROC)SystemPropertyProc); // ����ϵͳ���ԶԻ���
}

// ********************************************************************
// ������static LRESULT CALLBACK SystemPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������ϵͳ���ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK SystemPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

		RegisterGeneralWindow(hInstance); // ע��ͨ�ô���
		RegisterVersionWindow(hInstance); // ע��汾����
		
		numItem = sizeof(SystemPropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = SystemPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����һ��TAB��Ŀ
			CreateWindow(SystemPropertySheet[i].lpClassName,"",WS_CHILD,0,30,210,140,hDlg,(HMENU)SystemPropertySheet[i].id,hInstance,0); // ����һ�����Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ����ĿΪ��ǰ��Ŀ
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,SystemPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��ǰ������Ӧ�Ĵ���
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
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰ����
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SystemPropertySheet[iIndex].id),SW_HIDE); // ����������Ӧ�Ĵ���
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı䣬�ı�֮��
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SystemPropertySheet[iIndex].id),SW_SHOW); // ��ʾ������Ӧ�Ĵ���
				 }
				 return 0;
		}
		return 0;
}


/****************************************************************************/
//  �����桱 ����
/****************************************************************************/
#define ID_PROGRESS		107
#define ID_USEMEMORY	106
#define ID_TOTALMEMORY	105

static LRESULT CALLBACK GeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DisplaySystemGeneralWindow(HWND hWnd,HDC hdc);
static LRESULT DoSystemGeneralCreate(HWND hWnd);
static LRESULT ReFreshMemory(HWND hWnd);

// **************************************************
// ������static ATOM RegisterGeneralWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��ͨ�ô����ࡣ
// ����: 
// **************************************************
static ATOM RegisterGeneralWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strGeneralClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)GeneralWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK GeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������ϵͳ���ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK GeneralWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"General",7);
			//DisplaySystemGeneralWindow(hWnd,hdc);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_TIMER: // ��ʱ����Ϣ
			ReFreshMemory(hWnd);
			return 0;
		case WM_CREATE: // ������Ϣ
			return DoSystemGeneralCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			KillTimer(hWnd,1);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static void DisplaySystemGeneralWindow(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - ���ھ��
//    IN hdc - �豸
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ��������������ϵͳ���Գ���Ի���
// ����: 
// ********************************************************************
/*
static void DisplaySystemGeneralWindow(HWND hWnd,HDC hdc)
{
	RECT rect;
	TCHAR lpUseMemory[64];
	DWORD dwTotalSize,dwUseSize,dwUsePer;
	HBRUSH hBrush;
	DWORD dwEnd;

		SetBkMode( hdc, TRANSPARENT ) ;
		TextOut(hdc,10,5,"ϵͳ:�޹�ϵͳ(Kingmos) 2.0.0",strlen("ϵͳ:�޹�ϵͳ(Kingmos) 2.0.0"));

//		dwTotalSize = GetTotalMemorySize();
		dwTotalSize = 16 * 1024 * 1024;

		sprintf(lpUseMemory,"�ڴ�����: %d",dwTotalSize);
		TextOut(hdc,10,30,lpUseMemory,strlen(lpUseMemory));
//		dwUseSize = GetUseMemorySize();
		dwUseSize = 7 * 1024 * 1024;
		dwUsePer = (dwUseSize * 100 ) / dwTotalSize;
		sprintf(lpUseMemory,"��ʹ��: %d%%",dwUsePer);
		
		TextOut(hdc,10,55,lpUseMemory,strlen(lpUseMemory));

		rect.left = 10;
		rect.top = 80;
		rect.right = rect.left + 170;
		rect.bottom = rect.top + 20;
		DrawEdge(hdc,&rect,BDR_SUNKENINNER,BF_RECT);
		InflateRect(&rect,-2,-2);
		
		dwEnd = rect.right;
		rect.right = dwUsePer * (rect.right-rect.left) / 100;
		hBrush = CreateSolidBrush(RGB(77,166,255));
		FillRect(hdc,&rect,hBrush);
		DeleteObject(hBrush);

		rect.left = rect.right ;
		rect.right = dwEnd;
		hBrush = GetStockObject(WHITE_BRUSH);
		FillRect(hdc,&rect,hBrush);

}
*/
// ********************************************************************
// ������static LRESULT DoSystemGeneralCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ��������������ϵͳ�������ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoSystemGeneralCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hProgress;
	DWORD dwTotalSize,dwUseSize,dwUsePer;
	TCHAR lpUseMemory[64];
	MEMORYSTATUS MemoryStatus;
/*
typedef struct _MEMORYSTATUS
{
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORD dwTotalPhys;
    DWORD dwAvailPhys;
    DWORD dwTotalPageFile;
    DWORD dwAvailPageFile;
    DWORD dwTotalVirtual;
    DWORD dwAvailVirtual;
} MEMORYSTATUS, FAR *LPMEMORYSTATUS;
*/
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		// ����ϵͳ����
		CreateWindow(classSTATIC,"ϵͳ:",WS_CHILD | WS_VISIBLE ,15,5,40,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,"�޹���Kingmos",WS_CHILD | WS_VISIBLE ,60,5,120,18,hWnd,(HMENU)0xffff,hInstance,0);
		// �����汾
		CreateWindow(classSTATIC,"�汾:",WS_CHILD | WS_VISIBLE ,15,30,40,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,"2.0.0",WS_CHILD | WS_VISIBLE ,60,30,120,18,hWnd,(HMENU)0xffff,hInstance,0);

		MemoryStatus.dwLength = sizeof(MemoryStatus);
		GlobalMemoryStatus(&MemoryStatus); // �õ���ǰ�ڴ�״��
		dwTotalSize = MemoryStatus.dwTotalPhys /1024; // �õ����ڴ�
	//	dwTotalSize = 16 * 1024 * 1024;ֵ
		sprintf(lpUseMemory,"%d KB",dwTotalSize);
		// ��ʾ�ڴ�����
		CreateWindow(classSTATIC,"�ڴ�����:",WS_CHILD | WS_VISIBLE ,15,55,74,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,90,55,90,18,hWnd,(HMENU)ID_TOTALMEMORY,hInstance,0);
		// �õ�ʹ���ڴ�
		dwUseSize = MemoryStatus.dwTotalPhys - MemoryStatus.dwAvailPhys;
	//	dwUseSize = 7 * 1024 * 1024;
//		dwUsePer = (dwUseSize * 100 ) / dwTotalSize;
		dwUsePer = MemoryStatus.dwMemoryLoad;
		sprintf(lpUseMemory,"%d%%",dwUsePer);
		// ��ʾʹ���ڴ�
		CreateWindow(classSTATIC,"��ʹ��:",WS_CHILD | WS_VISIBLE ,15,80,60,18,hWnd,(HMENU)0xffff,hInstance,0);
		CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,75,80,100,18,hWnd,(HMENU)ID_USEMEMORY,hInstance,0);
		// ������ʾ��ǰ�ڴ�Ľ�����
		hProgress = CreateWindowEx(WS_EX_CLIENTEDGE,classPROGRESS,"",WS_CHILD | WS_VISIBLE | PBS_SMOOTH,15,105,160,20,hWnd,(HMENU)ID_PROGRESS,hInstance,0);

		SendMessage(hProgress,PBM_SETRANGE,0,MAKELPARAM(0,100)); // ���÷�Χ
		SendMessage(hProgress,PBM_SETPOS,dwUsePer,0); // ���õ�ǰֵ
		SendMessage(hProgress,PBM_SETBARCOLOR,0,RGB(77,166,255)); // ������ɫ

		SetTimer(hWnd,1,2000,NULL); // ������ʱ��
	return 0;
}

// ********************************************************************
// ������static LRESULT ReFreshMemory(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// ����������ˢ�µ�ǰ���ڴ���ʾ
// ����: 
// ********************************************************************
static LRESULT ReFreshMemory(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hChild;
	DWORD dwTotalSize,dwUseSize,dwUsePer;
	TCHAR lpUseMemory[64];
	MEMORYSTATUS MemoryStatus;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����


		MemoryStatus.dwLength = sizeof(MemoryStatus);
		GlobalMemoryStatus(&MemoryStatus); // �õ���ǰ���ڴ�ֵ

		RETAILMSG(1,("MemoryStatus.dwTotalPhys = %d\r\n",MemoryStatus.dwTotalPhys));
		RETAILMSG(1,("MemoryStatus.dwAvailPhys = %d\r\n",MemoryStatus.dwAvailPhys));
		RETAILMSG(1,("MemoryStatus.dwMemoryLoad = %d\r\n",MemoryStatus.dwMemoryLoad));
		// �õ����ڴ�
		dwTotalSize = MemoryStatus.dwTotalPhys /1024;
	//	dwTotalSize = 16 * 1024 * 1024;
		sprintf(lpUseMemory,"%d KB",dwTotalSize);

		//CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,90,55,90,18,hWnd,(HMENU)0xffff,hInstance,0);
		hChild = GetDlgItem(hWnd,ID_TOTALMEMORY);
		SetWindowText(hChild,lpUseMemory);  // �������ڴ�ֵ
		// �õ��Ѿ�ʹ�õ��ڴ�
		dwUseSize = MemoryStatus.dwTotalPhys - MemoryStatus.dwAvailPhys;
		RETAILMSG(1,("dwUseSize = %d\r\n",dwUseSize));
		dwUsePer = MemoryStatus.dwMemoryLoad;
		sprintf(lpUseMemory,"%d%%",dwUsePer);

//		CreateWindow(classSTATIC,"��ʹ��:",WS_CHILD | WS_VISIBLE ,15,80,60,18,hWnd,(HMENU)0xffff,hInstance,0);
//		CreateWindow(classSTATIC,lpUseMemory,WS_CHILD | WS_VISIBLE ,75,80,100,18,hWnd,(HMENU)0xffff,hInstance,0);
		hChild = GetDlgItem(hWnd,ID_USEMEMORY);
		SetWindowText(hChild,lpUseMemory); // �����Ѿ�ʹ�õ��ڴ�

//		hProgress = CreateWindowEx(WS_EX_CLIENTEDGE,classPROGRESS,"",WS_CHILD | WS_VISIBLE | PBS_SMOOTH,15,105,160,20,hWnd,(HMENU)ID_PROGRESS,hInstance,0);
		hChild = GetDlgItem(hWnd,ID_PROGRESS);

		SendMessage(hChild,PBM_SETPOS,dwUsePer,0); // ���ý�������ǰ��λ��

	return 0;
}


/****************************************************************************/
//  ���汾�� ����
/****************************************************************************/
static LRESULT CALLBACK VersionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DisplaySystemVersionWindow(HWND hWnd,HDC hdc);

// **************************************************
// ������static ATOM RegisterVersionWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע��汾�����ࡣ
// ����: 
// **************************************************
static ATOM RegisterVersionWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strVersionClassName; // �汾��������
   wc.lpfnWndProc	=(WNDPROC)VersionWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK VersionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������ϵͳ���ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK VersionWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"Version",7);
			DisplaySystemVersionWindow(hWnd,hdc);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			break;
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// ********************************************************************
// ������static void DisplaySystemVersionWindow(HWND hWnd,HDC hdc)
// ������
//	IN hWnd - ���ھ��
//    IN hdc - �豸
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ��������������ϵͳ���Գ���Ի���
// ����: 
// ********************************************************************
static void DisplaySystemVersionWindow(HWND hWnd,HDC hdc)
{
	RECT rect = {5,45,205,115};
	HICON hIcon;
	LPTSTR lpShowString1 = "����΢�߼��������޹�˾";
	LPTSTR lpShowString2 = "��Ȩ����(C)1998-2003";
	LPTSTR lpShowString3 = "��������Ȩ��";
	
		SetBkMode( hdc, TRANSPARENT ) ; // ������ʾģʽ
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SYSLOGO ), IMAGE_ICON,32, 32, 0 ) ;
		DrawIcon(hdc,90,5,hIcon); // ����ϵͳͼ��
		DestroyIcon(hIcon);
		// ���ư汾��Ϣ
		DrawText(hdc,lpShowString1 ,strlen(lpShowString1 ),&rect,DT_CENTER);
		rect.top += 25;
		DrawText(hdc,lpShowString2 ,strlen(lpShowString2 ),&rect,DT_CENTER);
		rect.top += 25;
		DrawText(hdc,lpShowString3 ,strlen(lpShowString3 ),&rect,DT_CENTER);
//		DrawText(hdc,"Copyright (C)1998-2003 \n������΢�߼��������޹�˾.  \nAll rights reserved0",strlen("ϵͳ��Kingmos(�޹�) 1.0"),&rect,DT_CENTER);
//		DrawText(hdc,"Copyright (C)1998-2003 \n������΢�߼��������޹�˾.  \nAll rights reserved0",strlen("ϵͳ��Kingmos(�޹�) 1.0"),&rect,DT_CENTER);
}

