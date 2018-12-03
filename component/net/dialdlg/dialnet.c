/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：小屏幕模式的IE浏览类,拨号程序
版本号：1.0.0.456
开发时期：2004-09-14
作者：陈建明 JAMI
修改记录：
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
    { WS_EX_NOMOVE,WS_POPUP|WS_VISIBLE|WS_CAPTION,2,0,100,240,130,0,0,"连接网络" },
    {
		//提示条
		{ 0, WS_CHILD | WS_VISIBLE, 10, 10, 220, 52, IDC_TIP, classSTATIC, "正在连接中...", 0 },
		//取消按钮
		{ 0, WS_CHILD | WS_VISIBLE, 80, 64, 80, 20, IDC_CANCEL, classBUTTON, "取消", 0 },
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
// 声明：BOOL DailToInternet(HWND hWnd)
// 参数：
// 	IN hWnd - 窗口句柄
// 返回值：
//	
// 功能描述：拨号到Internet
// 引用: 
// ********************************************************************
BOOL DailToInternet(HWND hWnd,HANDLE *pRasConn,DWORD dwDailNetworkType)
{
	DWORD dwNetworkType;

	// 参数检查
	if (dwDailNetworkType != GPRS_NETTYPE_INTERNET && dwDailNetworkType != GPRS_NETTYPE_WAP)
		return FALSE;
	// 判断当前的状态
	if( GprsCheckDial(&dwNetworkType) )
	{
		if( dwNetworkType!=dwDailNetworkType )
		{
			if (dwNetworkType == GPRS_NETTYPE_INTERNET)
				MessageBox( hWnd, "请断开INTERNET网络", "GprsCheckDial", MB_OK );
			else
				MessageBox( hWnd, "请断开WAP网络", "GprsCheckDial", MB_OK );
			return FALSE;
		}
		else
		{  // 已经连接到网络
			return TRUE;
		}
	}
	//2 设置GPRS拨号的网络类型
	GprsCfg_SetCurNetType( dwDailNetworkType );
	// 还没有连接到网络，开始拨号
	return DialogBoxIndirectParamEx((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
				(LPDLG_TEMPLATE_EX)&dlg_Dailing, hWnd, DailingProc,(LONG)pRasConn);
}


// ********************************************************************
// 声明：BOOL HandDownInternet(HWND hWnd, HANDLE hRasConn)
// 参数：
// 	IN hWnd - 窗口句柄
//  IN hRasConn -- 拨号句柄
// 返回值：
//	
// 功能描述：挂断与网络的联系
// 引用: 
// ********************************************************************
BOOL HandDownInternet(HWND hWnd, HANDLE hRasConn)
{
	if (hRasConn == NULL)
		return TRUE;
	RasHangUp( hRasConn );
	return TRUE;
}	

// ********************************************************************
// 声明：static LRESULT CALLBACK DailingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// 参数：
//	IN hWnd - 窗口句柄
//    IN message - 需要处理的消息
//    IN wParam - 消息参数
//    IN lParam - 消息参数
// 返回值：
//	消息处理后返回的结果
// 功能描述：拨号窗口处理过程
// 引用: 
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
		case WM_PAINT:// 绘制消息
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
		case WM_INITDIALOG: // 创建消息
			return DoDailingInit(hWnd,wParam,lParam);
	}
	return FALSE;
}


// **************************************************
// 声明：static LRESULT DoDailingInit(HWND hDlg,WPARAM wParam,LPARAM lParam)
// 参数：
// 	IN hDlg -- 窗口句柄
// 
// 返回值：成功返回0，否则返回-1
// 功能描述：初始化对话框。
// 引用: 
// **************************************************
static LRESULT DoDailingInit(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	SetWindowLong(hDlg,GWL_USERDATA,lParam);  // lParam -- pRasConn ， 将来保存当前拨号的句柄
	Dial_Start( hDlg );
	return 0;
}


// **************************************************
// 声明：static int Dial_Start( HWND hWnd )
// 参数：
// 	IN hWnd -- 窗口句柄
// 
// 返回值：返回当前的拨号状态
// 功能描述：开始拨号。
// 引用: 
// **************************************************
static int	Dial_Start( HWND hWnd )
{
	HANDLE	hThrd;
	DWORD	dwThrdID;
	
	hThrd = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)Thrd_RasDial, (LPVOID)hWnd, 0, &dwThrdID );
	if( !hThrd )
	{	
		// 创建线程失败，不能开始拨号
		return -1;
	}
	CloseHandle( hThrd );

	return 0; // 开始拨号
}

// **************************************************
// 声明：DWORD	WINAPI	Thrd_RasDial( LPVOID lpParameter )
// 参数：
// 	IN lpParameter -- 拨号线程
// 
// 返回值：无
// 功能描述：拨号线程。
// 引用: 
// **************************************************
static DWORD	WINAPI	Thrd_RasDial( LPVOID lpParameter )
{
	DWORD		dwErr;
	HWND hWnd;
	HANDLE *pRasConn;
#ifdef INLINE_PROGRAM
	DIALPARAM	DialParam;
#endif

	hWnd = (HWND)lpParameter; // 得到窗口句柄
	pRasConn = (HANDLE *)GetWindowLong(hWnd,GWL_USERDATA);   // 得到存放拨号的句柄
	

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
// 声明：static void	tip_Event( HWND hWnd, WPARAM wParam, LPARAM lParam )
// 参数：
// 	IN hWnd -- 窗口句柄
//	IN	wParam -- 参数
//	IN	lParam -- 参数
// 
// 返回值：无
// 功能描述：拨号线程。
// 引用: 
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
// 声明：static BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType )
// 参数：
//	OUT	pdwNetworkType -- 返回当前的上网类型
// 
// 返回值：GPRS还已经上网返回TRUE，否则返回FALSE
// 功能描述：拨号线程。
// 引用: 
// **************************************************
static BOOL	GprsCheckDial( OUT DWORD* pdwNetworkType )
{
	//1 是否有GPRS拨号上网
	if( !RasCheckDev( RASDT_CHKLINK, 0 ) )
//	if( !RasIsDevExist( TRUE, RASDT_SUB_GPRS, RASDT_CHKSUB ) )
	{
		return FALSE;
	}
	//2 得到GPRS拨号的网络类型
	GprsCfg_GetCurNetType( pdwNetworkType );
	return TRUE;
}


// **************************************************
// 声明：static void StopDail(HWND hWnd)
// 参数：
//	IN	hWnd -- 窗口句柄
// 
// 返回值：无
// 功能描述：停止拨号。
// 引用: 
// **************************************************
static void StopDail(HWND hWnd)
{
	HANDLE *pRasConn;

		pRasConn = (HANDLE *)GetWindowLong(hWnd,GWL_USERDATA);   // 得到存放拨号的句柄

		HandDownInternet(hWnd,*pRasConn); // 挂断与网络的联系

		pRasConn = NULL;
}

