/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵�����Ի������
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
2004-08-27, ȥ�� AP_XSTART, AP_YSTART, AP_WIDTH, AP_HEIGHT������ϵͳ����
2003-10-07: LN,��ģʽ�Ի���������е���Ϣ 
2003-08-29��LN,���Ӷ� hWnd��DestroyWindow������
2003-08-26: LN,���Ӷ� Window Title Ptr �� lpClasName��ӳ�䣨MapPtrToProcess��
2003-05-22: LN,�� DS_CENTER ���� AP_XSTART, AP_YSTART, AP_WIDTH, AP_HEIGHT
2003-04-10: LN,������WM_INITDIALOG���޷����ý���
           (1)����DoActive �� hCurCtlȥ����Dialog�ĵ�ǰ����
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <edialog.h>

//#include <eapisrv.h>
//#include <winsrv.h>
//#include <gdisrv.h>
//#include <epwin.h>
//#include <eobjcall.h>
//#define _MAPPOINTER

static int _RunModalLoop( HWND hWnd );
static BOOL _EnableModal( HWND hWnd );
static HWND _PreModal( HWND );
static LRESULT CALLBACK DialogWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
static const char classDIALOG[] = "DIALOG";

static BOOL DoDlgPreHandle( HWND hDlg, LPMSG lpMsg );

//#define DWL_MSGRESULT   0
//#define DWL_DLGPROC     4
//#define DWL_USER        8

//�Ի���˽�����ݽṹ
typedef struct {
	DWORD iResult;	
    DLGPROC lpDialogFunc;
	DWORD dwUserData;
    DWORD dwState;
    HWND  hCurCtl;  //LN:2003-04-10
}DLG_WNDEX;

//#define DWL_MSGRESULT		0
//#define DWL_DLGPROC		4
//#define DWL_USER			8
#define DWL_STATE			12
#define DWL_CURCTL			16

#define BUTTON_HEIGHT 20

// **************************************************
// ������ATOM RegisterDialogClass( HINSTANCE hInst )
// ������
// 	IN hInst - ʵ�����
// ����ֵ��
//	����ɹ�������ע�����ԭ�ӣ����򣬷���0
// ����������
//	ע��Ի�����
// ����: 
//	��ϵͳ��ʼ��ϵͳ��ʱ������øú���
// ************************************************

ATOM RegisterDialogClass( HINSTANCE hInst )
{
    WNDCLASS wc;
	//��ʼ����ṹ
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = DialogWndProc;
    wc.hInstance = hInst;
    wc.hIcon = NULL;//LoadIcon( NULL, IDI_APPLICATION );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DLG_WNDEX );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;//GetStockObject( LTGRAY_BRUSH );
    wc.lpszMenuName = 0;
    wc.lpszClassName = classDIALOG;
	//ע��
    return RegisterClass( &wc );
}

// **************************************************
// ������static HWND DoCreateDialog( 
//							HINSTANCE hInst,
//							LPCDLG_TEMPLATE lpcdt,
//							HWND hParent,
//							DLGPROC lpDialogFunc,
//							LPARAM lParamInit,
//							BOOL * lpbVisible,
//							HANDLE hCallerProcess )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ��ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	OUT lpbVisible - ���ڽ����Ƿ����
//	IN hCallerProcess - ���иù��ܵĽ���
// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������ʼ���Ի��򴰿�
// ����: 
//	
// ************************************************

static HWND DoCreateDialog( HINSTANCE hInst,
						   LPCDLG_TEMPLATE lpcdt,
						   HWND hParent,
						   DLGPROC lpDialogFunc,
						   LPARAM lParamInit,
						   BOOL * lpbVisible,
						   HANDLE hCallerProcess )
{
    HWND hWnd, hChild;
    LPCDLG_ITEMTEMPLATE lpcdi = (LPCDLG_ITEMTEMPLATE)(lpcdt + 1);
    int i, x, y;
    BOOL bVisible;
    const char * lpClassName;

	*lpbVisible = FALSE;
	//������ܣ�ӳ��ָ��
    if( lpcdt->lpcClassName )
	{
#ifdef _MAPPOINTER
        lpClassName = (const char *)MapPtrToProcess( (LPVOID)lpcdt->lpcClassName, hCallerProcess );
#else
		lpClassName = (const char *)lpcdt->lpcClassName;
#endif
	}
    else
        lpClassName = classDIALOG;

    bVisible = (lpcdt->dwStyle & WS_VISIBLE) ? 1 : 0;

	x = lpcdt->x; y = lpcdt->y; 
	if( lpcdt->dwStyle & DS_CENTER )
	{	//�Ի�����Ҫ����
		RECT rect;

		if( hParent && (lpcdt->dwStyle & WS_CHILD)  )
		{	//�õ����Ŀͻ�������
			GetClientRect( hParent, &rect );
		}
		else
		{
			rect.left = GetSystemMetrics( SM_XCENTER );//AP_XSTART;
			rect.top = GetSystemMetrics( SM_YCENTER );//AP_YSTART;
			rect.right = rect.left + GetSystemMetrics( SM_CXCENTER );//AP_WIDTH;
			rect.bottom = rect.top + GetSystemMetrics( SM_CYCENTER );//AP_HEIGHT;
		}
		x = ( (rect.right-rect.left) - lpcdt->cx ) / 2 + rect.left;
		y = ( (rect.bottom-rect.top) - lpcdt->cy ) / 2 + rect.top;
	}
	//�����Ի��򴰿�
	hWnd = CreateWindowEx( 0, 
	                     (LPCBYTE)lpClassName,
#ifdef _MAPPOINTER
                         MapPtrToProcess( (LPVOID)lpcdt->lpcWindowName, hCallerProcess ),
#else
						 lpcdt->lpcWindowName,
#endif
                         lpcdt->dwStyle & (~WS_VISIBLE),
                         x,
                         y,
                         lpcdt->cx,
                         lpcdt->cy,
                         hParent,
                         (HMENU)lpcdt->id,
                         hInst,
                         (LPVOID)lpDialogFunc );
    if( hWnd )
    {	//���������ڳɹ�
        i = lpcdt->cdit;
		//�����Ӵ���
		while( i )
		{
			hChild = CreateWindowEx( 		
				0,
#ifdef _MAPPOINTER
				MapPtrToProcess( (LPVOID)lpcdi->lpcClassName, hCallerProcess ),
				MapPtrToProcess( (LPVOID)lpcdi->lpcWindowName, hCallerProcess ),
#else
				lpcdi->lpcClassName,
				lpcdi->lpcWindowName,
#endif
				lpcdi->dwStyle|WS_CHILD,
				lpcdi->x,
				lpcdi->y,
				lpcdi->cx,
				lpcdi->cy,
				hWnd,
				(HMENU)lpcdi->id,
				hInst,
				lpcdi->lpParam );
			if( hChild == NULL )
			{	//����һ���Ӵ���ʧ��
				DestroyWindow( hWnd );
				hWnd = NULL;
				break;
			}
			i--;
			lpcdi++;
		}

	    *lpbVisible = bVisible;
    }

    return hWnd;
}

// **************************************************
// ������static HWND DoCreateIndirectParam( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit, 
//								  HANDLE hCallerProcess )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ��ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	IN hCallerProcess - ���иù��ܵĽ���
// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������ʼ���Ի��򴰿�
// ����: 
//	
// ************************************************

static HWND DoCreateIndirectParam( 
								  HINSTANCE hInst,
								  LPCDLG_TEMPLATE lpcdt,
								  HWND hParent,
								  DLGPROC lpDialogFunc,
								  LPARAM lParamInit, 
								  HANDLE hCallerProcess )
{
	BOOL bVisible;
	//��������
    HWND hWnd = DoCreateDialog( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );
	if( hWnd )
	{
        SendMessage( hWnd, WM_INITDIALOG, 0, lParamInit );

        if( bVisible )
		{
            ShowWindow( hWnd, SW_SHOWNORMAL );
		}
	}
	return hWnd;
}

// **************************************************
// ������HWND WINAPI Dlg_CreateIndirectParam( 
//								HINSTANCE hInst,
//                              LPCDLG_TEMPLATE lpcdt,
//                              HWND hParent,
//                              DLGPROC lpDialogFunc,
//                              LPARAM lParamInit )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ��ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����

// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������ʼ���Ի��򴰿�
// ����: 
//	ϵͳAPI
// ************************************************

HWND WINAPI Dlg_CreateIndirectParam( HINSTANCE hInst,
                                LPCDLG_TEMPLATE lpcdt,
                                HWND hParent,
                                DLGPROC lpDialogFunc,
                                LPARAM lParamInit )
{
	return DoCreateIndirectParam( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// ������HWND WINAPI Dlg_CreateIndirect( 
//						HINSTANCE hInst, 
//						LPCDLG_TEMPLATE lpcdt, 
//						HWND hParent, 
//						DLGPROC lpDialogFunc )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ��ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����

// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	����������������ʼ���Ի��򴰿�
// ����: 
//	ϵͳAPI
// ************************************************

HWND WINAPI Dlg_CreateIndirect( HINSTANCE hInst, LPCDLG_TEMPLATE lpcdt, HWND hParent, DLGPROC lpDialogFunc )
{
    return DoCreateIndirectParam( hInst, lpcdt, hParent, lpDialogFunc, 0, GetCallerProcess() );
}

// **************************************************
// ������static LRESULT DoDialogModal( HWND hWnd, HWND hWndParent, LPARAM lParamInit, BOOL bVisible )
// ������
// 	IN hWnd - ���ھ��
//	IN hWndParent - ������
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	IN bVisible
// ����ֵ��
//	���ؽ��ֵ
// ����������
//	����ģʽ�Ի���
// ����: 
//	
// ************************************************
#define DEBUG_DO_DIALOG_MODAL 0
static LRESULT DoDialogModal( HWND hWnd, HWND hWndParent, LPARAM lParamInit, BOOL bVisible )
{
    HWND hwndTop;
	BOOL bEnable = FALSE;
 
	DEBUGMSG( DEBUG_DO_DIALOG_MODAL, ( "DoDialogModal��Enter.\r\n" ) );

    if( bVisible )
		ShowWindow( hWnd, SW_SHOWNORMAL );
	SendMessage( hWnd, WM_INITDIALOG, 0, lParamInit );

	if( bVisible && _EnableModal( hWnd ) )
	{
		if( (GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD) == 0 )
		    SetForegroundWindow( hWnd );
	}
    

	//�Ƿ��˳� ��
	if( _EnableModal( hWnd ) )
	{	//��
		//������������
		hwndTop = _PreModal( hWndParent );
		if( hwndTop && IsWindowEnabled( hwndTop ) )
		{	//��Ч��������
			EnableWindow( hwndTop, FALSE );
			bEnable = TRUE;
		}
		DEBUGMSG( DEBUG_DO_DIALOG_MODAL, ( "DoDialogModal:_RunModalLoop Enter.\r\n" ) );
		_RunModalLoop( hWnd );
		
		DEBUGMSG( DEBUG_DO_DIALOG_MODAL, ( "DoDialogModal:_RunModalLoop Leave.\r\n" ) );
		//���ش���
		ShowWindow( hWnd, SW_HIDE );
		if( bEnable )
		{
			EnableWindow( hwndTop, TRUE );
		}
		if( hwndTop ) //&& GetActiveWindow() == hWnd )
		{
			SetActiveWindow( hwndTop );	//�ָ�֮ǰ��״̬			
		}

	}	

	//���ؽ��ֵ
	return (int)GetWindowLong( hWnd, DWL_MSGRESULT );
}

// **************************************************
// ������static int DoBoxIndirectParam( 
//							HANDLE hInst, 
//							LPDLG_TEMPLATE lpTemplate, 
//							HWND hWndParent,
//							DLGPROC lpDialogFunc, 
//							LPARAM lParamInit, 
//							HANDLE hCallerProcess )
// ������
//	IN hInst - ʵ�����
//	IN lpTemplate - �Ի���ģ��ṹ
//	IN hWndParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	IN hCallerProcess - �����߽���

// ����ֵ��
//	����ɹ������ش����������򣬷��� -1
// ����������
//	��������ʼ���Ի��򴰿ڣ�������ģʽ�������
// ����: 
//	ϵͳAPI
// ************************************************

static int DoBoxIndirectParam( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit, HANDLE hCallerProcess )
{
    HWND hWnd;//, hwndTop;
    BOOL bEnable = FALSE;
    int result = -1;
	BOOL bVisible;
	MSG msg;
	//������е������Ϣ
	//ClearThreadQueue( 0, NULL, WM_MOUSEFIRST,WM_MOUSELAST  );
	while( PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) )
		;
	//������еļ�����Ϣ
	//ClearThreadQueue( 0, NULL, WM_KEYFIRST,WM_KEYLAST  );
	while( PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) )
		;

	//�����Ի���
    hWnd = DoCreateDialog( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );

    if( hWnd )
    {	//����ģʽ����
		result = DoDialogModal( hWnd, hWndParent, lParamInit, bVisible );
		//�ƻ�����
		DestroyWindow( hWnd );
    }
    return result;
}

// **************************************************
// ������int WINAPI Dlg_BoxIndirectParam( 
//							HANDLE hInst,
//							LPDLG_TEMPLATE lpTemplate,
//							HWND hWndParent,
//							DLGPROC lpDialogFunc,
//							LPARAM lParamInit )
// ������
//	IN hInst - ʵ�����
//	IN lpTemplate - �Ի���ģ��ṹ
//	IN hWndParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
// ����ֵ��
//	����ɹ������ش����������򣬷��� -1
// ����������
//	��������������ʼ���Ի��򴰿ڣ�������ģʽ�������
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI Dlg_BoxIndirectParam( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit )
{
	return DoBoxIndirectParam( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// ������int WINAPI Dlg_BoxIndirect( 
//							HANDLE hInst,
//							LPDLG_TEMPLATE lpTemplate,
//							HWND hWndParent,
//							DLGPROC lpDialogFunc )
// ������
//	IN hInst - ʵ�����
//	IN lpTemplate - �Ի���ģ��ṹ
//	IN hWndParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
// ����ֵ��
//	����ɹ������ش����������򣬷��� -1
// ����������
//	����������������ʼ���Ի��򴰿ڣ�������ģʽ�������
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI Dlg_BoxIndirect( HANDLE hInst, LPDLG_TEMPLATE lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc )
{
    return DoBoxIndirectParam( hInst, lpTemplate, hWndParent, lpDialogFunc, 0, GetCallerProcess() );
}


//ex-begin
// **************************************************
// ������static HWND DoCreateDialogEx( HINSTANCE hInst, 
//							 LPCDLG_TEMPLATE_EX lpcdt, 
//							 HWND hParent, 
//							 DLGPROC lpDialogFunc, 
//							 LPARAM lParamInit,
//							 BOOL *lpbVisible,
//							 HANDLE hCallerProcess )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ��ṹ��DLG_TEMPLATE_EX�ṹָ�룩
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	OUT lpbVisible - ���ڽ����Ƿ����
//	IN hCallerProcess - ���иù��ܵĽ���
// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������ʼ���Ի��򴰿ڣ���DoCreateDialog��ͬ��������չ���
// ����: 
//	
// ************************************************

#define DEBUG_DO_CREATE_DIALOG_EX 0
static HWND DoCreateDialogEx( HINSTANCE hInst, 
							 LPCDLG_TEMPLATE_EX lpcdt, 
							 HWND hParent, 
							 DLGPROC lpDialogFunc, 
							 LPARAM lParamInit,
							 BOOL *lpbVisible,
							 HANDLE hCallerProcess )
{
    HWND hWnd, hChild;
    LPCDLG_ITEMTEMPLATE_EX lpcdi = (LPCDLG_ITEMTEMPLATE_EX)(lpcdt + 1);
    int i, x, y;
    BOOL bVisible;
    const char * lpClassName;

	*lpbVisible = FALSE;
	//ӳ��ָ�뵽�����߽���
	if( lpcdt->lpcClassName ) 
	{
#ifdef _MAPPOINTER		
        lpClassName = (const char *)MapPtrToProcess( (LPVOID)lpcdt->lpcClassName, hCallerProcess );
#else
		lpClassName = lpcdt->lpcClassName;
#endif
	}
    else
        lpClassName = classDIALOG;
    bVisible = (lpcdt->dwStyle & WS_VISIBLE) ? 1 : 0;

	x = lpcdt->x; y = lpcdt->y; 
	if( lpcdt->dwStyle & DS_CENTER )
	{
		RECT rect;
		//������ʾ���κ�λ��
		if( hParent && (lpcdt->dwStyle & WS_CHILD)  )
		{
			GetClientRect( hParent, &rect );
		}
		else
		{
			//rect.left = AP_XSTART;
			//rect.top = AP_YSTART;
			//rect.right = rect.left + AP_WIDTH;
			//rect.bottom = rect.top + AP_HEIGHT;
			rect.left = GetSystemMetrics( SM_XCENTER );//AP_XSTART;
			rect.top = GetSystemMetrics( SM_YCENTER );//AP_YSTART;
			rect.right = rect.left + GetSystemMetrics( SM_CXCENTER );//AP_WIDTH;
			rect.bottom = rect.top + GetSystemMetrics( SM_CYCENTER );//AP_HEIGHT;

		}
		x = ( (rect.right-rect.left) - lpcdt->cx ) / 2 + rect.left;
		y = ( (rect.bottom-rect.top) - lpcdt->cy ) / 2 + rect.top;
	}
	DEBUGMSG( DEBUG_DO_CREATE_DIALOG_EX, ( "DoCreateDialogEx: Create Main Window.\r\n" ) );
	//����������
	hWnd = CreateWindowEx( lpcdt->dwExStyle, 
		                 (LPCBYTE)lpClassName,
#ifdef _MAPPOINTER
                         MapPtrToProcess( (LPVOID)lpcdt->lpcWindowName, hCallerProcess ),
#else
						 lpcdt->lpcWindowName,
#endif
                         lpcdt->dwStyle & (~WS_VISIBLE),
                         x,
                         y,
                         lpcdt->cx,
                         lpcdt->cy,
                         hParent,
                         (HMENU)lpcdt->id,//hMenu,
                         hInst,
                         (LPVOID)lpDialogFunc );	
    if( hWnd )
    {
        //�����Ӵ���
		i = lpcdt->cdit;
		DEBUGMSG( DEBUG_DO_CREATE_DIALOG_EX, ( "DoCreateDialogEx: Create Sub Window( cdit=%d ).\r\n", i ) );
		while( i )
		{
			hChild = CreateWindowEx( 
				lpcdi->dwExStyle, 
#ifdef _MAPPOINTER
				MapPtrToProcess( (LPVOID)lpcdi->lpcClassName, hCallerProcess ),
				MapPtrToProcess( (LPVOID)lpcdi->lpcWindowName, hCallerProcess ),
#else
				lpcdi->lpcClassName,
				lpcdi->lpcWindowName,
#endif
				lpcdi->dwStyle|WS_CHILD,
				lpcdi->x,
				lpcdi->y,
				lpcdi->cx,
				lpcdi->cy,
				hWnd,
				(HMENU)lpcdi->id,
				hInst,
				lpcdi->lpParam );
			if( hChild == NULL )
			{	//����һ���Ӵ���ʧ��
				DestroyWindow( hWnd );
				hWnd = NULL;
				DEBUGMSG( DEBUG_DO_CREATE_DIALOG_EX, ( "DoCreateDialogEx: Create Sub Window(class=%s,title=%s) Failure( cdit=%d ).\r\n", lpcdi->lpcClassName ? lpcdi->lpcClassName : "", lpcdi->lpcWindowName ? lpcdi->lpcWindowName : "" ) );
				break;
			}
			i--;
			lpcdi++;
		}		
	    *lpbVisible = bVisible;
    }

    return hWnd;
}

// **************************************************
// ������static HWND DoCreateIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit, 
//								  HANDLE hCallerProcess )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ����չ�ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	IN hCallerProcess - ���иù��ܵĽ���
// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������������ʼ���Ի��򴰿ڣ��������ڵ���չ�ָ�
// ����: 
//	
// ************************************************
static HWND DoCreateIndirectParamEx( HINSTANCE hInst, 
									LPCDLG_TEMPLATE_EX lpcdt, 
									HWND hParent, 
									DLGPROC lpDialogFunc, 
									LPARAM lParamInit,
									HANDLE hCallerProcess )
{
	BOOL bVisible;
    HWND hWnd = DoCreateDialogEx( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );
	if( hWnd )
	{
        SendMessage( hWnd, WM_INITDIALOG, 0, lParamInit );

        if( bVisible )
		{	//��ʾ����
            ShowWindow( hWnd, SW_SHOWNORMAL );
		}
	}
	return hWnd;
}

// **************************************************
// ������HWND WINAPI Dlg_CreateIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit
//								  )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ����չ�ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������������ʼ���Ի��򴰿ڣ��������ڵ���չ�ָ�
// ����: 
//	ϵͳAPI
// ************************************************

HWND WINAPI Dlg_CreateIndirectParamEx( HINSTANCE hInst, 
										  LPCDLG_TEMPLATE_EX lpcdt, 
										  HWND hParent, 
										  DLGPROC lpDialogFunc, 
										  LPARAM lParamInit )
{
	return DoCreateIndirectParamEx( hInst, lpcdt, hParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// ������HWND WINAPI Dlg_CreateIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpcdt,
//								  HWND hParent,
//								  DLGPROC lpDialogFunc
//								  )
// ������
//	IN hInst - ʵ�����
//	IN lpcdt - �Ի���ģ����չ�ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
// ����ֵ��
//	����ɹ������ش����Ĵ��ھ�������򣬷��� NULL
// ����������
//	��������ʼ���Ի��򴰿ڣ��������ڵ���չ�ָ�
// ����: 
//	ϵͳAPI
// ************************************************

HWND WINAPI Dlg_CreateIndirectEx( HINSTANCE hInst, LPCDLG_TEMPLATE_EX lpTemplate, HWND hParent, DLGPROC lpDialogFunc )
{
	return DoCreateIndirectParamEx( hInst, lpTemplate, hParent, lpDialogFunc, 0, GetCallerProcess() );
}

// **************************************************
// ������int DoBoxIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpTemplate,
//								  HWND hWndParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit,
//								  HANDLE hCallerProcess
//								  )
// ������
//	IN hInst - ʵ�����
//	IN lpTemplate - �Ի���ģ����չ�ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
//	IN hCallerProcess - �����߽���
// ����ֵ��
//	����ɹ������ضԻ��򷵻ص�ֵ�����򣬷��� -1
// ����������
//	1).��������������ʼ���Ի��򴰿ڣ��������ڵ���չ�ָ�
//	2).����ģʽ����
// ����: 
//	
// ************************************************

static int DoBoxIndirectParamEx( HANDLE hInst, 
								LPDLG_TEMPLATE_EX lpTemplate, 
								HWND hWndParent, 
								DLGPROC lpDialogFunc, 
								LPARAM lParamInit, 
								HANDLE hCallerProcess )
{
    HWND hWnd;
    BOOL bEnable = FALSE;
    int result = -1;
	BOOL bVisible;
	MSG msg;
	//��������̶߳����е������Ϣ�ͼ�����Ϣ
	//������е������Ϣ
	//ClearThreadQueue( 0, NULL, WM_MOUSEFIRST,WM_MOUSELAST  );
	while( PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE ) )
		;
	//������еļ�����Ϣ
	//ClearThreadQueue( 0, NULL, WM_KEYFIRST,WM_KEYLAST  );
	while( PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ) )
		;
	//��������ʼ������
	hWnd = DoCreateDialogEx( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, &bVisible, hCallerProcess );

    if( hWnd )
    {	//����ģʽ
	    result = DoDialogModal( hWnd, hWndParent, lParamInit, bVisible );
		DestroyWindow( hWnd );    
	}
    return result;
}

// **************************************************
// ������int WINAPI Dlg_BoxIndirectParamEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpTemplate,
//								  HWND hWndParent,
//								  DLGPROC lpDialogFunc,
//								  LPARAM lParamInit
//								  )
// ������
//	IN hInst - ʵ�����
//	IN lpTemplate - �Ի���ģ����չ�ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
//	IN lParamInit - ���ݸ� WM_INITDIALOG ��Ϣ��LPARAM����
// ����ֵ��
//	����ɹ������ضԻ��򷵻ص�ֵ�����򣬷��� -1
// ����������
//	1).��������������ʼ���Ի��򴰿ڣ��������ڵ���չ�ָ�
//	2).����ģʽ����
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI Dlg_BoxIndirectParamEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc, LPARAM lParamInit )
{
	return DoBoxIndirectParamEx( hInst, lpTemplate, hWndParent, lpDialogFunc, lParamInit, GetCallerProcess() );
}

// **************************************************
// ������int WINAPI Dlg_BoxIndirectEx( 
//								  HINSTANCE hInst,
//								  LPCDLG_TEMPLATE_EX lpTemplate,
//								  HWND hWndParent,
//								  DLGPROC lpDialogFunc,
//								  )
// ������
//	IN hInst - ʵ�����
//	IN lpTemplate - �Ի���ģ����չ�ṹ
//	IN hParent - �Ի���ĸ�����
//	IN lpDialogFunc - �Ի���ص�����
// ����ֵ��
//	����ɹ������ضԻ��򷵻ص�ֵ�����򣬷��� -1
// ����������
//	1).��������ʼ���Ի��򴰿ڣ��������ڵ���չ�ָ�
//	2).����ģʽ����
// ����: 
//	ϵͳAPI
// ************************************************
int WINAPI Dlg_BoxIndirectEx( HANDLE hInst, LPDLG_TEMPLATE_EX lpTemplate, HWND hWndParent, DLGPROC lpDialogFunc )
{	
	return DoBoxIndirectParamEx( hInst, lpTemplate, hWndParent, lpDialogFunc, 0, GetCallerProcess() );
}

//ex-end

// **************************************************
// ������BOOL WINAPI Dlg_End( HWND hDlg, int nResult )
// ������
// 	IN hDlg - �Ի��򴰿ھ��
//	IN nResult - ���ݸ��Ի���ķ��ش���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����Ի���ģʽ�������öԻ��򷵻ش���
// ����: 
//	ϵͳAPI 
// ************************************************

BOOL WINAPI Dlg_End( HWND hDlg, int nResult )
{
	//���÷��ش���
    SetWindowLong( hDlg, DWL_MSGRESULT, nResult );
	//���÷��ر�־
	SetWindowLong( hDlg, DWL_STATE, 1 );   // set end flag
	//����п��ܶԻ�����  GetMessage ��ȴ���û�г�����
	//���Ի��򷢸���Ϣ��Ŀ���������GetMessage ����������˳�����
	// get a no oprate message to notify dlg message loop to exit;
	PostMessage( hDlg, WM_NULL, 0, 0 );
    return TRUE;
}

// **************************************************
// ������int WINAPI Dlg_GetCtrlID( HWND hwndCtl )
// ������
// 	IN hwndCtl - �ؼ����ھ��
// ����ֵ��
//	����ɹ������ؿؼ�ID�����򣬷���0
// ����������
//	�õ��ؼ�/����ID
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI Dlg_GetCtrlID( HWND hwndCtl )
{
    return (int)GetWindowLong( hwndCtl, GWL_ID );
}

// **************************************************
// ������HWND WINAPI Dlg_GetItem( HWND hDlg, int nID )
// ������
// 	IN hDlg - �Ի��򴰿ھ��
//	IN nID - �Ӵ���ID
// ����ֵ��
//	����ɹ��������Ӵ��ڴ��ھ�������򣬷���NULL
// ����������
//	�ɿؼ�ID�õ��Ի����Ӵ��ڴ��ھ��
// ����: 
//	ϵͳAPI
// ************************************************

HWND WINAPI Dlg_GetItem( HWND hDlg, int nID )
{
    HWND hWnd = GetWindow( hDlg, GW_CHILD );	//�õ���һ���Ӵ���

    while( hWnd )
    {
        if( GetWindowLong( hWnd, GWL_ID ) == nID )
            return hWnd;
		//�õ���һ���Ӵ���
        hWnd = GetWindow( hWnd, GW_HWNDNEXT );
    }
    return hWnd;
}

// **************************************************
// ������UINT WINAPI Dlg_GetItemText( HWND hDlg, int nID, LPSTR lpstr, int nMaxCount )
// ������
// 	IN hDlg - �Ի��򴰿ھ��
//	IN nID - �Ӵ���ID
//	IN/OUT lpstr - ���ڽ����ı��Ļ���
//	IN nMaxCount - lpstr����Ĵ�С
// ����ֵ��
//	����ɹ������ؿ�����lpstr����ַ���������0�������������򣬷���0
// ����������
//	�õ��Ի���ؼ��ı�
// ����: 
//	ϵͳAPI
// ************************************************

UINT WINAPI Dlg_GetItemText( HWND hDlg, int nID, LPSTR lpstr, int nMaxCount )
{
    HWND hWnd = Dlg_GetItem( hDlg, nID );
    if( hWnd )
        return (UINT)SendMessage( hWnd, WM_GETTEXT, (WPARAM)nMaxCount, (LPARAM)lpstr );
    else
        return 0;
}

// **************************************************
// ������LONG WINAPI Dlg_SendItemMessage( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hDlg - �Ի�����
//	IN nID  - �Ӵ���ID
//	IN msg - ��Ҫ���͵���Ϣ
//	IN wParam - WPARAM ��Ϣ����
//	IN lParam - LPARAM ��Ϣ����
// ����ֵ��
//	����ɹ������ظ���Ϣ��Ӧ�ķ���ֵ�����򣬷���0
// ����������
//	��Ի���ؼ�������Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

LONG WINAPI Dlg_SendItemMessage( HWND hDlg, int nID, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HWND hWnd = Dlg_GetItem( hDlg, nID );
    _ASSERT( hWnd );
    if( hWnd )
        return SendMessage( hWnd, msg, wParam, lParam );
    else
        return 0;
}

// **************************************************
// ������BOOL WINAPI Dlg_SetItemText( HWND hDlg, int nID, LPCTSTR lpstr )
// ������
// 	IN hDlg - �Ի��򴰿ھ��
//	IN nID - �Ӵ���/�ؼ�ID
//	IN lpstr - ��Ҫ���õ��Ӵ���/�ؼ����ı�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����Ӵ���/�ؼ��Ĳֿ��ı�
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Dlg_SetItemText( HWND hDlg, int nID, LPCTSTR lpstr )
{
    HWND hWnd = Dlg_GetItem( hDlg, nID );
    if( hWnd )
        return (BOOL)SendMessage( hWnd, WM_SETTEXT, 0, (LPARAM)lpstr );
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI Dlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned )
// ������
// 	IN hDlg - �Ի��򴰿ھ��
//	IN nID - �Ӵ���/�ؼ�ID
//	IN uValue - ����ֵ
//	IN bSigned - ˵��nValue�Ǵ�����(ΪTRUE)���ǲ������ŵ�(ΪFALSE)
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ֵ uValue ת��Ϊ�ַ����ı�
//	���ַ����ı�����Ϊ�Ӵ���/�ؼ�ID�Ĵ����ı�
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Dlg_SetItemInt( HWND hDlg, int nID, UINT uValue, BOOL bSigned )
{
	HWND	hCtl;
	TCHAR	pszTmp[20];

	hCtl = Dlg_GetItem( hDlg, nID );
	if( hCtl )
	{	//��Ч�� ����
		//��ֵ uValue ת��Ϊ�ַ����ı�
		if( bSigned )
			sprintf( pszTmp, TEXT("%d"), uValue );
		else
			sprintf( pszTmp, TEXT("%u"), uValue );
		//���ô����ı�
		return SetWindowText( hCtl, pszTmp );
	}
	return FALSE;
}

// **************************************************
// ������UINT WINAPI Dlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned )
// ������
// 	IN hDlg - �Ի��򴰿ھ��
//	IN nID - �Ӵ���/�ؼ�ID
//	OUT lpTranslated - ���ڽ����Ƿ�ת���ɹ�
//	IN bSigned - ˵������ֵ�Ǵ�����(ΪTRUE)���ǲ������ŵ�(ΪFALSE)
// ����ֵ��
//	����ɹ���lpTranslated��ֵΪTRUE������lpTranslated��ֵΪFALSE
// ����������
//	�õ��Ӵ���/�ؼ��Ĵ����ı�
//	���õ��Ĵ����ı�ת��Ϊ����ֵ
// ����: 
//	ϵͳAPI
// ************************************************

UINT WINAPI Dlg_GetItemInt( HWND hDlg, int nID, BOOL *lpTranslated, BOOL bSigned )
{
	TCHAR	pszNum[20];
	HWND	hCtl;
	UINT uiRetv = 0;

	if( lpTranslated )
		*lpTranslated =FALSE;
	hCtl = Dlg_GetItem( hDlg, nID );
	//��Ч���� ��
	if( hCtl )
	{	//��Ч���õ��䳤��
		UINT len = SendMessage( hCtl, WM_GETTEXTLENGTH, 0, 0 );
		if( len <= 19 )
		{	//���ٷ���
			SendMessage( hCtl, WM_GETTEXT, 20, (LPARAM)pszNum );
			if( bSigned )
			    uiRetv = (UINT)atoi( pszNum );	//������
			else
				uiRetv = (UINT)strtoul( pszNum, NULL, 10 );	//��������
	        if( lpTranslated )
		       *lpTranslated = TRUE;
		}
		else
		{	//���ٷ������ȷ���Ҫ���С��ָ��
			TCHAR * p = malloc( (len + 1)* sizeof(TCHAR) );
			if( p )
			{	//�õ��ı�
				SendMessage( hCtl, WM_GETTEXT, (len + 1)* sizeof(TCHAR), (LPARAM)p );
				if( bSigned )
					uiRetv = (UINT)atoi( p );
				else
					uiRetv = (UINT)strtoul( p, NULL, 10 );				
				free( p );
	            if( lpTranslated )
		           *lpTranslated = TRUE;
			}
		}
	}
	return uiRetv;
}

// **************************************************
// ������static BOOL _EnableModal( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	�������ģʽ��������TRUE�����򣬷���FALSE
// ����������
//	�ж϶Ի���򴰿��Ƿ���Ҫ��������ģʽ����
// ����: 
//	
// ************************************************

static BOOL _EnableModal( HWND hWnd )
{   
	return ( IsWindow( hWnd ) && !GetWindowLong( hWnd, DWL_STATE ) );
}

// **************************************************
// ������static int _RunModalLoop( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ�������ģʽ������ֵ�����򣬷���0
// ����������
//	����ģʽ�Ի������
// ����: 
//	
// ************************************************

#define DEBUG_MODAL_LOOP 0
static int _RunModalLoop( HWND hWnd )
{
    DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
    BOOL bIdle = (BOOL)!(dwStyle & DS_NOIDLEMSG);
    MSG msg;
    BOOL bContinue = TRUE;
    HWND hWndParent = GetParent( hWnd );
    ULONG ulIdleCount = 0;

    DEBUGMSG( DEBUG_MODAL_LOOP, ( "_RunModalLoop: entry.\r\n" ) );
	SetCapture( NULL );
	//ѭ��������Ϣֱ���˳�
	while( bContinue )
    {	//�õ���ǰģʽ״̬
        bContinue = _EnableModal( hWnd );// enable continue modal loop
		//�ж��Ƿ�����Ϣ
        while( bContinue && bIdle && !PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) )
        {	//û����Ϣ���Ƿ���Ҫ���� IDLE ��Ϣ ��
            if( !(dwStyle & DS_NOIDLEMSG) && hWndParent && ulIdleCount == 0 )
            {	//��
                SendMessage( hWndParent, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)hWnd );
            }
            bIdle = SendMessage( hWnd, WM_KICKIDLE, MSGF_DIALOGBOX, ulIdleCount++ );
			bContinue = _EnableModal( hWnd );
        }
		//
        while( bContinue )
        {	//�ж��Ƿ�����Ϣ
            if( GetMessage( &msg, 0, 0, 0 ) )
            {	//��
                TranslateMessage( &msg );
				// 2005-09-20�� ���Ӷ� WS_GROUP �Ĵ���
				if( DoDlgPreHandle( hWnd, &msg ) == FALSE )
					DispatchMessage( &msg );
				//
            }
            else
            {	//�쳣�˳�
                ASSERT( 0 );
                PostQuitMessage(0);
                return -1;
            }
			//��������Ϣ������Ƿ��и������Ϣ ���Ƿ��˳�ģʽ����
            bContinue = _EnableModal( hWnd );
			if( bContinue && PeekMessage( &msg, 0, 0, 0, PM_NOREMOVE ) )
				continue;
			else
				break;
        };

        if( !(dwStyle & DS_NOIDLEMSG) )
        {
            bIdle = TRUE;
            ulIdleCount = 0;
        }
    }
	DEBUGMSG( DEBUG_MODAL_LOOP, ( "_RunModalLoop: leave.\r\n" ) );
	return (int)GetWindowLong( hWnd, DWL_MSGRESULT );
}

// **************************************************
// ������static HWND _PreModal( HWND hWndParent )
// ������
// 	IN hWndParent - ���ھ��
// ����ֵ��
//	����ǰһ��ģʽ����
// ����������
//	�õ�ǰһ��ģʽ����
// ����: 
//	
// ************************************************

static HWND _PreModal( HWND hWndParent )
{
    HWND hWnd = hWndParent;
    while( hWnd && ( GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD ) )
        hWnd = GetParent( hWnd );
    return hWnd;
}

// **************************************************
// ������static HWND FindFocusWindow( HWND hWndDialog )
// ������
// 	IN hWndDialog - ���ھ��
// ����ֵ��
//	����ɹ���������Ҫ���ý���Ĵ��ھ�������򣬷���NULL
// ����������
//	������Ҫ���ý���Ĵ��ھ��
// ����: 
//	
// ************************************************

static HWND FindFocusWindow( HWND hWndDialog )
{
	HWND hChild = GetWindow( hWndDialog, GW_CHILD );

	while( hChild )
	{
		DWORD dwStyle = GetWindowLong( hChild, GWL_STYLE );
        if( (dwStyle & WS_VISIBLE) && 
			(dwStyle & WS_DISABLED ) == 0 &&
			(dwStyle & WS_TABSTOP ) )
			break;
		hChild = GetWindow( hChild, GW_HWNDNEXT );		
	}
	return hChild;
}

// **************************************************
// ������static LRESULT DoActive( HWND hWnd, WPARAM wParam )
// ������
// 	IN hWnd - ���ھ��
//	IN wParam - WM_ACTIVATE ��Ϣ�� WPARAM ����
// ����ֵ��
//	����0
// ����������
//	���� WM_ACTIVATE ��Ϣ
// ����: 
//	
// ************************************************

static LRESULT DoActive( HWND hWnd, WPARAM wParam )
{
	if( LOWORD( wParam ) == WA_INACTIVE )
	{	//��Ҫʹ����Ϊ�ǻ״̬
		HWND hFocus = GetFocus();
		
		if( hFocus && GetParent( hFocus ) == hWnd )
			SetWindowLong( hWnd, DWL_CURCTL, (LONG)hFocus );
		else
			SetWindowLong( hWnd, DWL_CURCTL, NULL );
	}
	else if( LOWORD( wParam ) == WA_ACTIVE )
	{	//��Ҫʹ����Ϊ�״̬
		HWND hFocus = (HWND)GetWindowLong( hWnd, DWL_CURCTL );
		
		if( hFocus == NULL )
			hFocus = FindFocusWindow( hWnd );	//Ԥ��һ��

		if( hFocus )
			SetFocus( hFocus );
	}
	return 0;
}

// **************************************************
// ������LRESULT WINAPI DialogWndProc(
//									HWND hWnd,
//									UINT msg,
//									WPARAM wParam,
//									LPARAM lParam)
// ������
//  IN hWnd - ���ھ��
//	IN msg - ��Ϣ
//	IN wParam - ��һ����Ϣ
//	IN lParam - �ڶ�����Ϣ
// ����ֵ��
//	�����ھ������Ϣ
// ����������
//	�Ի��򴰿ڴ��������
// ����: 
//	
// ************************************************


LRESULT CALLBACK DialogWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    LPCREATESTRUCT lpcs;
    DLGPROC lpDialogFunc=0;
    BOOL retv = TRUE;

    lpDialogFunc = (DLGPROC)GetWindowLong( hWnd, DWL_DLGPROC );
    if( lpDialogFunc )
    {	//���û��������ȴ���
/*
		_LPWINDATA lpws = _GetHWNDPtr( hWnd );
		if( lpws )
		{
			if( lpws->hinst != hgwmeInstance )
			{	//�����
				CALLBACKDATA cd;
				cd.hProcess = lpws->hOwnerProcess;
				cd.lpfn = (FARPROC)lpDialogFunc;
				cd.dwArg0 = (DWORD)hWnd;
				retv = Sys_ImplementCallBack4( &cd, msg, wParam, lParam );
			}
			else
				retv = lpDialogFunc( hWnd, msg, wParam, lParam );
		}
		else
			return 0;
		*/
		retv = lpDialogFunc( hWnd, msg, wParam, lParam );
		
		if( IsWindow( hWnd ) )
		{
			if( retv && ( msg != WM_INITDIALOG ) )
			{
				return GetWindowLong( hWnd, DWL_MSGRESULT );	//�û��Ѵ��� ����Ҫ������Ĵ���
			}
		}
		else
		{	//��Ч�Ĵ���
			return 0;
		}

    }
	//�Ի���Ĭ�ϴ���
    switch( msg )
    {
    case WM_COMMAND:
       if( HIWORD( wParam ) == BN_CLICKED &&
           ( LOWORD( wParam ) == IDOK || LOWORD( wParam ) == IDCANCEL ) )
           Dlg_End( hWnd, LOWORD( wParam ) );
       break;
	case WM_ERASEBKGND:		//��������
		{
			RECT rcClient;

			GetClientRect( hWnd, &rcClient );
			FillRect( (HDC)wParam, &rcClient, GetSysColorBrush(COLOR_3DFACE) );
			return 0;
		}
	case WM_ACTIVATE:		//���ڼ���
		return DoActive( hWnd, wParam );
    case WM_CLOSE:			//���ڹر�
        Dlg_End( hWnd, 0 );
        break;
    case WM_CREATE:
        lpcs = (LPCREATESTRUCT)lParam;
        SetWindowLong( hWnd, DWL_DLGPROC, (DWORD)lpcs->lpCreateParams );
        SetWindowLong( hWnd, DWL_MSGRESULT, 0 );
		SetWindowLong( hWnd, DWL_STATE, 0 );
		SetWindowLong( hWnd, DWL_CURCTL, 0 );

        break;
    case WM_INITDIALOG:
        if( retv == TRUE )
        {    // set focus control
            if( wParam )
                SetFocus( (HWND)wParam );			
        }
		SetWindowLong( hWnd, DWL_MSGRESULT, 0 );
        break;
    default:
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }
    return 0;
}

// 2005-09-20�� ���Ӷ� WS_GROUP �Ĵ���
HWND WINAPI Dlg_GetNextGroupItem(
								HWND hDlg,  // ������
								HWND hCtl,  // ��ǰ�ؼ�
								BOOL bPrevious  // ��ǰ���������..
								)
{
	HWND hNewCtrl = hCtl;
	HWND hStartCtrl = hCtl;
	BOOL bIsInvert = FALSE;
	if( hDlg )
	{
		if( hCtl )
		{
			while( 1 )
			{
				if( bPrevious )
				{
					hNewCtrl = GetWindow( hStartCtrl, GW_HWNDPREV );
				}
				else
				{
					hNewCtrl = GetWindow( hStartCtrl, GW_HWNDNEXT );
				}
				if( hNewCtrl )
				{  //����
					DWORD dwStyle = GetWindowLong( hNewCtrl, GWL_STYLE );
					if( (dwStyle & WS_VISIBLE) &&
						(dwStyle & WS_DISABLED) == 0 )
					{   //�ɼ����ҿ��ܵ�
						if( ( dwStyle & WS_GROUP ) == 0 )
						{	//��������
							if( !bIsInvert )
							    return hNewCtrl;
						}
						else
						{   //�µ� group ��ʼ����Ҫ��ת���򣬵õ���һ�� WS_GROUP
							if( bIsInvert )
							{ //�Ѿ���ת��һ��,����Ҫ����..
								return hStartCtrl;
							}
							else
							{  //��תһ��
								bPrevious = !bPrevious;
								bIsInvert = TRUE;
								hStartCtrl = hCtl;
								continue;
							}
						}
					}					
				}
				else  //�ҵ�û��
				{
					if( bIsInvert == TRUE )
					{//�Ѿ���ת��һ��
						return hStartCtrl;
					}
					//��תһ��
					bIsInvert = TRUE;
					bPrevious = !bPrevious;
					hStartCtrl = hCtl;
					continue;
				}
				hStartCtrl = hNewCtrl;  //��һ��
			}
		}
	}
	return hNewCtrl;
}

// 2005-09-20�� ���Ӷ� WS_GROUP �Ĵ���
static BOOL DoDlgPreHandle( 
						   HWND hDlg,      // �ؼ����ڵĶԻ���
						   LPMSG lpMsg     // ��Ϣ�ṹ
						   )
{
	WPARAM wParam;
	switch( lpMsg->message )
	{
	case WM_KEYDOWN:
		wParam = lpMsg->wParam;
		if( wParam == VK_LEFT  ||
			wParam == VK_RIGHT ||
			wParam == VK_UP ||
			wParam == VK_DOWN )
		{	//�����
			if( IsChild( hDlg, lpMsg->hwnd ) )
			{
				LRESULT lret = SendMessage( lpMsg->hwnd, WM_GETDLGCODE, 0, 0 );
				HWND hCtrl;
				if( lret != DLGC_WANTALLKEYS ||
					lret != DLGC_WANTARROWS )
				{	// control ����Ҫ������Щ��Ϣ
					BOOL bPrevious = ( wParam == VK_LEFT || wParam == VK_UP );
					hCtrl = GetNextDlgGroupItem( hDlg, lpMsg->hwnd, bPrevious );
					if( hCtrl )
					{   // �����µĽ���
						SetFocus( hCtrl );
						//SendMessage( hCtrl, WM_SETFOCUS, (WPARAM)lpMsg->hwnd, 0 );
						return TRUE;
					}
				}
			}
		}
		break;
	}
	return FALSE;
}