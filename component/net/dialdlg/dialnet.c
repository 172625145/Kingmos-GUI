/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��Ļģʽ��IE�����,���ų���
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-09-14
���ߣ��½��� JAMI
�޸ļ�¼��
**************************************************/
#include <ewindows.h>
#include <eComCtrl.h>
#include <TabCtrl.h>
#include <ras.h>
#include <gprscfg.h>
#include <pcfile.h>
//#include <ztmsgbox.h>
//#include <thirdpart.h>


#define IDC_TIP		100
#define IDC_CANCEL	101
const static struct dlg_Dailing{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[2];
}dlg_Dailing = {
    { WS_EX_NOMOVE,WS_POPUP|WS_VISIBLE|WS_CAPTION,2,0,100,240,130,0,0,"��������" },
    {
		//��ʾ��
		{ 0, WS_CHILD | WS_VISIBLE, 10, 10, 220, 52, IDC_TIP, classSTATIC, "����������...", 0 },
		//ȡ����ť
		{ 0, WS_CHILD | WS_VISIBLE, 80, 64, 80, 20, IDC_CANCEL, classBUTTON, "ȡ��", 0 },
    } 
};

static LRESULT CALLBACK DailingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoDailingInit(HWND hDlg,WPARAM wParam,LPARAM lParam);
static BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType );
static int	Dial_Start( HWND hWnd );
static void	tip_Event( HWND hWnd, WPARAM wParam, LPARAM lParam );
static DWORD	WINAPI	Thrd_RasDial( LPVOID lpParameter );
static void StopDail(HWND hWnd);

// ********************************************************************
// ������BOOL DailToInternet(HWND hWnd)
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	
// �������������ŵ�Internet
// ����: 
// ********************************************************************
BOOL DailToInternet(HWND hWnd,HANDLE *pRasConn,DWORD dwDailNetworkType)
{
	DWORD dwNetworkType;

	// �������
	if (dwDailNetworkType != GPRS_NETTYPE_INTERNET && dwDailNetworkType != GPRS_NETTYPE_WAP)
		return FALSE;
	// �жϵ�ǰ��״̬
	if( GprsCheckDial(&dwNetworkType) )
	{
		if( dwNetworkType!=dwDailNetworkType )
		{
			if (dwNetworkType == GPRS_NETTYPE_INTERNET)
				MessageBox( hWnd, "��Ͽ�INTERNET����", "GprsCheckDial", MB_OK );
			else
				MessageBox( hWnd, "��Ͽ�WAP����", "GprsCheckDial", MB_OK );
			return FALSE;
		}
		else
		{  // �Ѿ����ӵ�����
			return TRUE;
		}
	}
	//2 ����GPRS���ŵ���������
	GprsCfg_SetCurNetType( dwDailNetworkType );
	// ��û�����ӵ����磬��ʼ����
	return DialogBoxIndirectParamEx((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				(LPDLG_TEMPLATE_EX)&dlg_Dailing, hWnd, DailingProc,(LONG)pRasConn);
}


// ********************************************************************
// ������BOOL HandDownInternet(HWND hWnd, HANDLE hRasConn)
// ������
// 	IN hWnd - ���ھ��
//  IN hRasConn -- ���ž��
// ����ֵ��
//	
// �����������Ҷ����������ϵ
// ����: 
// ********************************************************************
BOOL HandDownInternet(HWND hWnd, HANDLE hRasConn)
{
	if (hRasConn == NULL)
		return TRUE;
	RasHangUp( hRasConn );
	return TRUE;
}	

// ********************************************************************
// ������static LRESULT CALLBACK DailingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// �������������Ŵ��ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK DailingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_RASDIALEVENT:
			tip_Event( hWnd, wParam, lParam );
			break;
		case WM_PAINT:// ������Ϣ
			hdc = BeginPaint( hWnd, &ps );
			//TextOut(hdc,10,10,"date time",9);
			EndPaint( hWnd, &ps );
			return TRUE;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch(wmId)
			{
				case IDC_CANCEL:
					StopDail(hWnd);
					break;
			}
			break;
		case WM_INITDIALOG: // ������Ϣ
			return DoDailingInit(hWnd,wParam,lParam);
	}
	return FALSE;
}


// **************************************************
// ������static LRESULT DoDailingInit(HWND hDlg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hDlg -- ���ھ��
// 
// ����ֵ���ɹ�����0�����򷵻�-1
// ������������ʼ���Ի���
// ����: 
// **************************************************
static LRESULT DoDailingInit(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	SetWindowLong(hDlg,GWL_USERDATA,lParam);  // lParam -- pRasConn �� �������浱ǰ���ŵľ��
	Dial_Start( hDlg );
	return 0;
}


// **************************************************
// ������static int Dial_Start( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ�����ص�ǰ�Ĳ���״̬
// ������������ʼ���š�
// ����: 
// **************************************************
static int	Dial_Start( HWND hWnd )
{
	HANDLE	hThrd;
	DWORD	dwThrdID;
	
	hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)Thrd_RasDial, (LPVOID)hWnd, 0, &dwThrdID );
	if( !hThrd )
	{	
		// �����߳�ʧ�ܣ����ܿ�ʼ����
		return -1;
	}
	CloseHandle( hThrd );

	return 0; // ��ʼ����
}

// **************************************************
// ������DWORD	WINAPI	Thrd_RasDial( LPVOID lpParameter )
// ������
// 	IN lpParameter -- �����߳�
// 
// ����ֵ����
// ���������������̡߳�
// ����: 
// **************************************************
static DWORD	WINAPI	Thrd_RasDial( LPVOID lpParameter )
{
	DWORD		dwErr;
	HWND hWnd;
	HANDLE *pRasConn;
#ifdef INLINE_PROGRAM
	DIALPARAM	DialParam;
#endif

	hWnd = (HWND)lpParameter; // �õ����ھ��
	pRasConn = (HANDLE *)GetWindowLong(hWnd,GWL_USERDATA);   // �õ���Ų��ŵľ��
	

#ifdef INLINE_PROGRAM
	memset( &DialParam, 0, sizeof(DIALPARAM) );
	DialParam.dwSize = sizeof(DIALPARAM);
	strcpy(DialParam.szPhoneNumber,"84");
	strcpy(DialParam.szUserName ,"96169");
	strcpy(DialParam.szPassword ,"961691");
	strcpy(DialParam.szDomain ,"");
	dwErr = RasDial( NULL,&DialParam, DIALNOTIFY_WND, (LPVOID)hWnd, pRasConn );
#else
	dwErr = RasDial( NULL,NULL, DIALNOTIFY_WND, (LPVOID)hWnd, pRasConn );
#endif

	return 0;
}


// **************************************************
// ������static void	tip_Event( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
//	IN	wParam -- ����
//	IN	lParam -- ����
// 
// ����ֵ����
// ���������������̡߳�
// ����: 
// **************************************************
static void	tip_Event( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	DWORD	dwEvt = (DWORD)wParam;
	DWORD	dwErr = (DWORD)lParam;
	TCHAR	pszTip[128];

	//
	if( dwErr==RASERR_SUCCESS )
	{
		RasGetStateString( dwEvt, pszTip, sizeof(pszTip) );
		if( dwEvt==RASCS_Connected )
		{
			EndDialog(hWnd,TRUE);
		}
	}
	else
	{
		RasGetErrorString( dwErr, pszTip, sizeof(pszTip) );
		SetWindowText( GetDlgItem(hWnd, IDC_TIP), pszTip );
		UpdateWindow(GetDlgItem(hWnd, IDC_TIP));
		Sleep(2000);
		EndDialog(hWnd,FALSE);
	}
	//
	SetWindowText( GetDlgItem(hWnd, IDC_TIP), pszTip );
}


// **************************************************
// ������static BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType )
// ������
//	OUT	pdwNetworkType -- ���ص�ǰ����������
// 
// ����ֵ��GPRS���Ѿ���������TRUE�����򷵻�FALSE
// ���������������̡߳�
// ����: 
// **************************************************
static BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType )
{
	//1 �Ƿ���GPRS��������
	if( !RasCheckDev( RASDT_CHKLINK, 0 ) )
//	if( !RasIsDevExist( TRUE, RASDT_SUB_GPRS, RASDT_CHKSUB ) )
	{
		return FALSE;
	}
	//2 �õ�GPRS���ŵ���������
	GprsCfg_GetCurNetType( pdwNetworkType );
	return TRUE;
}


// **************************************************
// ������static void StopDail(HWND hWnd)
// ������
//	IN	hWnd -- ���ھ��
// 
// ����ֵ����
// ����������ֹͣ���š�
// ����: 
// **************************************************
static void StopDail(HWND hWnd)
{
	HANDLE *pRasConn;

		pRasConn = (HANDLE *)GetWindowLong(hWnd,GWL_USERDATA);   // �õ���Ų��ŵľ��

		HandDownInternet(hWnd,*pRasConn); // �Ҷ����������ϵ

		pRasConn = NULL;
}

