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
#include <SliderCtrl.h>
#include "emmsys.h"

/***************  ȫ���� ���壬 ���� *****************/
const struct dlgIDD_SoundProperty{
    DLG_TEMPLATE_EX dlg;
}dlgIDD_SoundProperty = {
    { WS_EX_NOMOVE|WS_EX_OKBOX|WS_EX_CLOSEBOX,0x90000000L|WS_CAPTION,0,30,80,210,120,0,0,"��������" },
    };

static LPTSTR strSoundVolumeClassName = "SOUNDVOLUMECLASS";
static LPTSTR strSoundSoundClassName = "SOUNDSOUNDCLASS";

#define IDC_TAB		300

#define IDC_SOUNDVOLUME  400
#define IDC_SOUNDSOUND   401

typedef struct{
	LPTSTR lpName;
	LPTSTR lpClassName;
	UINT id;
}TABSHEET;
static const TABSHEET SoundPropertySheet[] = {
	{"����","SOUNDVOLUMECLASS",IDC_SOUNDVOLUME},
//	{"����","SOUNDSOUNDCLASS",IDC_SOUNDSOUND},
};

/***************  �������� *****************/
static LRESULT CALLBACK SoundPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitDialog(HWND hDlg);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSoundPropertyOK(HWND hDlg);

ATOM RegisterVolumeWindow(HINSTANCE hInstance);
ATOM RegisterSoundWindow(HINSTANCE hInstance);

/***************  ����ʵ�� *****************/

// **************************************************
// ������void DoSoundProperty(HINSTANCE hInstance,HWND hWnd)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ��������������������ں�����
// ����: 
// **************************************************
void DoSoundProperty(HINSTANCE hInstance,HWND hWnd)
{
//	HINSTANCE hInstance;

//		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);
		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_SoundProperty,hWnd, (DLGPROC)SoundPropertyProc); // �����������ԶԻ���
}

// ********************************************************************
// ������static LRESULT CALLBACK SoundPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������������ԶԻ���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK SoundPropertyProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			DoSoundPropertyOK(hDlg);
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
// ���������� ��ʼ���Ի���
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
		// ���õ�ǰ��ɫ
		stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR ;
		stCtlColor.cl_Text = RGB(50,50,50);
		stCtlColor.cl_TextBk = RGB(240,240,240);
		SendMessage(hTab,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

		RegisterVolumeWindow(hInstance); // ע������������
		RegisterSoundWindow(hInstance); // ע������������

		numItem = sizeof(SoundPropertySheet) / sizeof(TABSHEET); // �õ�TAB��Ŀ����
		for( i =0; i<numItem;i++)
		{
			tcItem.mask =  TCIF_TEXT;
			tcItem.dwState = 0;
    		tcItem.dwStateMask = 0;
			tcItem.pszText = SoundPropertySheet[i].lpName;
			tcItem.cchTextMax= 0;
			tcItem.iImage = -1;
			tcItem.lParam = 0;
			TabCtrl_InsertItem(	hTab,i,&tcItem); // ����һ��TAB��Ŀ
			CreateWindow(SoundPropertySheet[i].lpClassName,"",WS_CHILD,0,27,210,73,hDlg,(HMENU)SoundPropertySheet[i].id,hInstance,0); // ����һ����Ӧ�Ĵ���
		}

		iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
		if (iIndex == -1)
		{ // û��ѡ��
			TabCtrl_SetCurSel(hTab,0); // ���õ�һ����ĿΪ��ǰѡ��
		}
		else
		{
			ShowWindow(GetDlgItem(hDlg,SoundPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ŀ��Ӧ�Ĵ���
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
			 case TCN_SELCHANGING: // ѡ�����ڸı䣬�ı���ǰ
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SoundPropertySheet[iIndex].id),SW_HIDE); // ���ض�Ӧ����
				 }
				 return 0;
			 case TCN_SELCHANGE: // ѡ���Ѿ��ı䣬�ı��Ժ�
				 iIndex = TabCtrl_GetCurSel(hTab); // �õ���ǰѡ��
				 if (iIndex != -1)
				 {
					ShowWindow(GetDlgItem(hWnd,SoundPropertySheet[iIndex].id),SW_SHOW); // ��ʾ��Ӧ����
				 }
				 return 0;
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoSoundPropertyOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ������������������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoSoundPropertyOK(HWND hDlg)
{
	int i;
	HWND hSheet;
	int numItem;

		numItem = sizeof(SoundPropertySheet) / sizeof(TABSHEET); // �õ���Ŀ����
		for( i =0; i<numItem;i++)
		{
			hSheet = GetDlgItem(hDlg,SoundPropertySheet[i].id); // �õ���Ŀ��Ӧ�Ĵ���
			if (hSheet != NULL)
				SendMessage(hSheet,WM_OK,0,0); // ����OK��Ϣ
		}
		return TRUE;
}


/****************************************************************************/
//  �������� ����
/****************************************************************************/
#define IDC_SLIDER 201

static LRESULT CALLBACK VolumeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoSoundVolumeCreate(HWND hWnd);
static LRESULT DoVolumeOK(HWND hDlg);

static DWORD GetVolume(void);


// **************************************************
// ������static ATOM RegisterVolumeWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע�����������ࡣ
// ����: 
// **************************************************
static ATOM RegisterVolumeWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strSoundVolumeClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)VolumeWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // ����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ע����
}

// ********************************************************************
// ������static LRESULT CALLBACK VolumeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������������������Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK VolumeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
//			TextOut(hdc,10,10,"Volume",6);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			return DoSoundVolumeCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoVolumeOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
// ********************************************************************
// ������static LRESULT DoSoundVolumeCreate(HWND hWnd)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ�����0�����򷵻�-1
// �������������������������ԶԻ���
// ����: 
// ********************************************************************
static LRESULT DoSoundVolumeCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	HWND hSlider;
	HICON hIcon;
	DWORD dwVolumeLevel,iVolumeLevelNum;

	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����

	// �������
	CreateWindow(classSTATIC,"��",WS_CHILD | WS_VISIBLE ,10,20,20,18,hWnd,(HMENU)106,hInstance,0);
//	hSlider = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|WS_VISIBLE | TBS_AUTOTICKS |TBS_HORZ |TBS_TOP,50,20,100,50,hWnd,(HMENU)IDC_SLIDER,(HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),0);
	// �������������Ļ���
	hSlider = CreateWindow(TRACKBAR_CLASS,"",WS_CHILD|WS_VISIBLE | TBS_AUTOTICKS |TBS_HORZ |TBS_TOP,30,20,140,50,hWnd,(HMENU)IDC_SLIDER,hInstance,0);
	hIcon = (HICON)LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE( IDI_ARROW ), IMAGE_ICON,16, 16, 0 ) ;
	SendMessage(hSlider,TBM_SETTHUMBICON,MAKELONG(16,16),(LPARAM)hIcon); // ���û���ͼ��

	iVolumeLevelNum = 7;
	SendMessage(hSlider,TBM_SETRANGE,FALSE,MAKELONG(0,iVolumeLevelNum -1)); // ���÷�Χ

	dwVolumeLevel = GetVolume(); // �õ���ǰ����
	SendMessage(hSlider,TBM_SETPOS,TRUE,dwVolumeLevel); // ���õ�ǰ����

	SendMessage(hSlider,TBM_SETPAGESIZE,0,1); // ����ҳ�ߴ�

	CreateWindow(classSTATIC,"��",WS_CHILD | WS_VISIBLE ,173,20,20,18,hWnd,(HMENU)106,hInstance,0);

	return 0;
}

// ********************************************************************
// ������static LRESULT DoVolumeOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ��������������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoVolumeOK(HWND hDlg)
{
	HWND hSlider;
	DWORD dwVolume,dwVolumeLevel;

		hSlider = GetDlgItem(hDlg,IDC_SLIDER); // �õ����鴰�ھ��
		if (hSlider)
		{
			dwVolumeLevel = SendMessage(hSlider,TBM_GETPOS,0,0); // �õ���ǰ����λ��
			dwVolume=dwVolumeLevel*0x22222222+0x11111111; // �õ���ǰ����
			RETAILMSG(1,(TEXT("will to be set Volume is %x\r\n"),dwVolume));

#ifndef EML_WIN32
			waveOutSetVolume(0,dwVolume); // ���õ�ǰ����
#endif

		}
		return TRUE;
}
// ********************************************************************
// ������static DWORD GetVolume(void)
// ������
// ����ֵ��
// 	���ص�ǰ��������С
// �����������õ���ǰ��������С
// ����: 
// ********************************************************************
static DWORD GetVolume(void)
{
	DWORD dwVolume=0,dwVolumeToSet;

#ifndef EML_WIN32
	waveOutGetVolume(0,&dwVolume); // �õ���ǰ����
#else
	dwVolume = 0x77777777;
#endif

	RETAILMSG(1,(TEXT("The current Volume is %x\r\n"),dwVolume));

	dwVolumeToSet = dwVolume & 0x0000E000;		// get the kernel volume bit 13-15
	dwVolumeToSet = (dwVolumeToSet >> 13) ;	   // move bit 13-15 to 0-2 

	return dwVolumeToSet;
}


/****************************************************************************/
//  �������� ����
/****************************************************************************/
static LRESULT CALLBACK SoundWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoSoundOK(HWND hDlg);

// **************************************************
// ������static ATOM RegisterSoundWindow(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע����
// ����������ע�����������ࡣ
// ����: 
// **************************************************
static ATOM RegisterSoundWindow(HINSTANCE hInstance)
{
   WNDCLASS wc;

   memset( &wc, 0, sizeof( wc ) );
   wc.style			= CS_HREDRAW | CS_VREDRAW;
   wc.hInstance		= hInstance;
   wc.lpszClassName = strSoundSoundClassName; // ��������
   wc.lpfnWndProc	=(WNDPROC)SoundWndProc; // ���ڹ��̺���
   wc.hIcon= 0;
   wc.hCursor= 0;
   wc.hbrBackground = GetSysColorBrush( COLOR_STATIC ); // �õ�����ˢ
   wc.lpszMenuName=0;
   wc.cbClsExtra=0;
   return RegisterClass(&wc); // ��ϵͳע����
}

// ********************************************************************
// ������static LRESULT CALLBACK SoundWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �����������������������Ի���Ĵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK SoundWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			TextOut(hdc,10,10,"Sound",5);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_CREATE: // ������Ϣ
			break;
		case WM_DESTROY: // �ƻ���Ϣ
			break;
		case WM_OK: // OK��Ϣ
			return DoSoundOK(hWnd);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// ********************************************************************
// ������static LRESULT DoSoundOK(HWND hDlg)
// ������
//	IN hWnd - ���ھ��
// ����ֵ��
//	�ɹ����� TRUE�����򷵻�FALSE
// ��������������WM_OK����
// ����: 
// ********************************************************************
static LRESULT DoSoundOK(HWND hDlg)
{
		return TRUE;
}

