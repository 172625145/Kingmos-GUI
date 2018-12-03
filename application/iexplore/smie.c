/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��IE�������
�汾�ţ�1.0.0.456
����ʱ�ڣ�2002-04-01
���ߣ��½��� Jami chen
�޸ļ�¼��
**************************************************/
#include <ewindows.h>
#include <Etoolbar.h>
#include <eHtmView.h>
#include <EShell.h>
#include "eComCtrl.h"
#include "resource.h"
#include <ecommdlg.h>
#include "emailformat.h"

//#define TOOLBAR_ICON_NUMBER			3
#define MAX_LOADSTRING				100
#define IDC_EXIT					101
#define IDC_ADDRESS					103
#define IDC_VIEW					104
#define IDC_GO						105
#define IDC_GOBACK					106
#define IDC_FORWARDS				107
#define IDC_STOP					108
#define IDC_OPEN					109
#define IDC_SAVE					110
#define IDC_FRESH					111
#define IDC_EMAIL					112

#define TOOLBAR_USEIMAGELIST

#define OPENHTML		0x0001
#define SAVEHTML		0x0002
#define SAVEIMG			0x0003

typedef struct tagIExploreData
{
	HWND		hToolbar ; // tool bar handle
	HWND		hAddress ; // address combobox handle
	HWND		hIExplore ; // IExplore handle 
}_IEDATA, *_LPIEDATA ;

const static TCHAR szIETitle[] = "IE���" ;
const static TCHAR szHelpTitle[] = "Helper" ;								// The title bar text
const static TCHAR szWindowClass[] = "_class_IExplorer";								// The title bar text

static ATOM MyRegisterClass(HINSTANCE hInstance);
static HWND InitInstance(HINSTANCE hInstance, const TCHAR *lpszTitle );

static BOOL CALLBACK IExploreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL InitIExplore(HINSTANCE hInstance,HWND hParentWnd);
static BOOL DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static int InitialIExploreWindow( HWND hWnd , TCHAR *lpszIndex );
static LRESULT DoGo(HWND hWnd);
static LRESULT DoGoBack(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoOpen(HWND hWnd);
static LRESULT DoSave(HWND hWnd);
static LRESULT DoEmail(HWND hWnd);
static LRESULT DoStop(HWND hWnd);
static void DoAddressChange(HWND hWnd,LPARAM lParam);;
static LRESULT DoAddressNotify(HWND hWnd,WORD wNotifyCode,LPARAM lParam);
static LRESULT DoHtmlViewNotify(HWND hWnd,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL CreateIExploreToolbar( HWND hWnd ) ;

static BOOL IsLocateFile(LPTSTR lpUrl);
static void SetButtonImage(HWND hWnd,UINT id,UINT iImage);
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName,WORD OpenStyle);


//extern ATOM RegisterHtmlViewClass( HINSTANCE hInstance);

static BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);
static BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType);


// !!! Add By Jami chen in 2003.09.12
static BOOL HaveExtName(LPTSTR lpFileName);
static void AddExtName(LPTSTR lpFileName,LPTSTR lpExtName);
// !!! Add End By jami chen in 2003.09.12



// **************************************************
// ������static _LPIEDATA GetIEPtr( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ������IE����ṹָ��
// �����������õ�IE����ṹָ�롣
// ����: 
// **************************************************
static _LPIEDATA GetIEPtr( HWND hWnd )
{
	_LPIEDATA		lpied ;

	lpied = ( _LPIEDATA )GetWindowLong( hWnd, GWL_USERDATA ) ; // �õ�IE����ṹָ��
	ASSERT( lpied != NULL ) ;
//	RETAILMSG(1,(TEXT("***lpied = %x\r\n"),lpied));
	
	return lpied ; // ����ָ��
}
BOOL LinkNet(HWND h, DWORD dw)
{
	return FALSE;
}
// ********************************************************************
//������void WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
//������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//  IN hPrevInstance   - ǰһ��Ӧ�ó����ʵ�����
//  IN lpCmdLine   - ����Ӧ�ó���ʱ�Ĳ���
//  IN nCmdShow  - Ӧ�ó�����ʾ����
//����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
//����������װ��Ӧ�ó���
//����: �� ϵͳ ����
// ********************************************************************
#ifdef INLINE_PROGRAM
int WINAPI SMIExplore(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
	MSG msg;
	HWND			hWnd;

	hWnd = FindWindow( szWindowClass, NULL ) ;  // ����ָ�������Ĵ����Ƿ��Ѿ�����
	if( hWnd != 0 )
	{ // �Ѿ����ڣ�˵��Ӧ�ó����Ѿ���
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ�ǰ̨
		if (lpCmdLine != NULL && strlen(lpCmdLine) != 0)
			InitialIExploreWindow( hWnd, lpCmdLine ); // ����ʼ������--����ָ�����ļ�
		return FALSE ;
	}

	InitCommonControls(); // ��ʼ��ͨ�ÿؼ�
	RegisterSMIE_BrowersClass( hInstance);
	MyRegisterClass( hInstance ); // ע��Ӧ�ó�����

	if ( (hWnd=InitInstance (hInstance, szIETitle )) == NULL )  // ��ʼ��Ӧ�ó���
	{
		return FALSE;
	}
	SetForegroundWindow(hWnd);
//	InitialIExploreWindow( hWnd, "http://jami.mlg.com/" );
	if (lpCmdLine == NULL || strlen(lpCmdLine) == 0)
	{ // û�в�������HOME PAGE
		TCHAR lpHomePage[MAX_PATH];
		// ��ע����HOME PAGE
		if (regReadData(HKEY_SOFTWARE_ROOT,
						"SoftWare\\IExplorer", "Home Page",lpHomePage, MAX_PATH,REG_SZ) == FALSE)
		{ // û���ҵ�
			strcpy(lpHomePage,"\\kingmos\\kingmos.htm"); // ����ϵͳָ���ļ�ΪHome Page
			// ���ø�Home Page��ע���
			regWriteData(HKEY_SOFTWARE_ROOT,"SoftWare\\IExplorer", "Home Page",lpHomePage, strlen(lpHomePage)+1,REG_SZ);
		}
		// ��Home Page
		InitialIExploreWindow( hWnd, lpHomePage );
	}
	else
		InitialIExploreWindow( hWnd, lpCmdLine ); // ���û�ָ�����ļ�
//	InitialIExploreWindow( hWnd, "\\MyWeb6\\index.htm" );
	// ��Ϣѭ��
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
/*
int WINAPI HelpExplore(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG				msg;
	HWND			hWnd ;
	_LPIEDATA		lpied ;

//	RegisterHtmlViewClass( hInstance);
	MyRegisterClass( hInstance );

	if (( hWnd = InitInstance (hInstance, szHelpTitle ) ) == NULL ) 
	{
		return FALSE;
	}
	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��
	ShowWindow( lpied->hAddress, SW_HIDE ) ;
	InitialIExploreWindow( hWnd , "\\help\\index.htm" );
	MoveWindow( lpied->hIExplore, 0, 0, 240, 270, TRUE ) ;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}
*/
// **************************************************
// ������static ATOM MyRegisterClass(HINSTANCE hInstance)
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ϵͳע��ֵ
// ������������ϵͳע��Ӧ�ó����ࡣ
// ����: 
// **************************************************
static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)IExploreProc; // ���ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_IEXPLORER),IMAGE_ICON,16,16,0); // Ӧ�ó���ͼ��
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // Ӧ�ó�������

	return RegisterClass(&wc); // ע����
}
// **************************************************
// ������static HWND InitInstance(HINSTANCE hInstance, const TCHAR *lpszTitle )
// ������
// 	IN hInstance -- Ӧ�ó���ʵ�����
// 	IN lpszTitle -- Ӧ�ó������
// 
// ����ֵ�����ش����Ĵ��ھ��
// ������������ʼ��Ӧ�ó���
// ����: 
// **************************************************
static HWND InitInstance(HINSTANCE hInstance, const TCHAR *lpszTitle )
{
	HWND			hWnd;
	int width,heigh;
   

    width = GetSystemMetrics(SM_CXSCREEN);
	heigh = GetSystemMetrics(SM_CYSCREEN);	
//	RECT			rt ;

//	GetWindowRect( GetDesktopWindow(), &rt ) ;
	// ����Ӧ�ó���������
	hWnd = CreateWindowEx( WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, lpszTitle, WS_VISIBLE|WS_SYSMENU,
		//rt.left, rt.top, rt.right-rt.left, rt.bottom-rt.top,
		0,0,width,heigh,
		NULL, NULL, hInstance, NULL);

	return hWnd ; // ���ش��ھ��
}
// **************************************************
// ������static void OnIExplorePaint( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �������������ƴ��ڡ�
// ����: 
// **************************************************
static void OnIExplorePaint( HWND hWnd )
{
	PAINTSTRUCT ps;
	HDC hdc;
	// �����������Ǳ����
	hdc = BeginPaint(hWnd, &ps); // ��ʼ����
	EndPaint(hWnd, &ps); // ���ƽ���
}
// **************************************************
// ������static void OnIExploreEraseBkgnd( HWND hWnd, HDC hdc )
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ����
// �������������ƴ��ڱ�����
// ����: 
// **************************************************
static void OnIExploreEraseBkgnd( HWND hWnd, HDC hdc )
{
	RECT		rt ;
	HBRUSH hBrush;

	GetClientRect( hWnd, &rt ) ; // �õ����ڿͻ�����
	rt.bottom = rt.top + 24 ;
	FillRect( hdc, &rt, GetStockObject(WHITE_BRUSH) ) ; // �ϲ����Ϊ��ɫ
	rt.top = rt.bottom ;
	rt.bottom = rt.top + 25 ;
	hBrush = CreateSolidBrush(RGB(247,255,247));
	FillRect( hdc, &rt, hBrush ) ; // �²����Ϊǳ��ɫ
	DeleteObject(hBrush);
//	MoveTo( hdc, rt.left, rt.bottom ) ;
//	LineTo( hdc, rt.right, rt.bottom ) ;
//	rt.left += 5 ;
//	SetBkMode( hdc, TRANSPARENT ) ;
//	DrawText( hdc, "��ַ:", strlen("��ַ:"), &rt, DT_SINGLELINE|DT_LEFT|DT_VCENTER ) ;
}
// **************************************************
// ������static BOOL OnIExploreCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������������������Ϣ
// ����: 
// **************************************************
static BOOL OnIExploreCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	WORD wmId, wmEvent;
	wmId    = LOWORD(wParam);
	wmEvent = HIWORD(wParam);
	switch (wmId)
	{
	case IDC_GO:// �򿪵�ַ���ڵ���ҳ
		DoGo(hWnd);
		break;
	case IDC_GOBACK: // �ص���һ�δ򿪵���ҳ
		DoGoBack(hWnd,wParam,lParam);
		break;
	case IDC_FORWARDS: // ȥ���������Ѿ��򿪵���ҳ
		DoForWards(hWnd,wParam,lParam);
		break;
	case IDC_EXIT: // �˳�Ӧ�ó���
		DestroyWindow(hWnd);
		break;
	case IDC_ADDRESS: // ��ַ��֪ͨ��Ϣ
		DoAddressNotify(hWnd,wmEvent,lParam);
		break;
	case IDC_OPEN: // ��һ��ָ�����ļ�
		DoOpen(hWnd);
		break;
	case IDC_SAVE: // ����ǰ�򿪵���ҳ���浽һ���ļ�
		DoSave(hWnd);
		break;
	case IDC_EMAIL: // ����ǰ�򿪵���ҳ��EMAIL���ͳ�ȥ
		DoEmail(hWnd);
		break;
	case IDC_STOP: // ֹͣ����ҳ
		DoStop(hWnd);
		break;
	}
	return TRUE ;
}
// **************************************************
// ������static void OnIExploreDestroy( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������ƻ����ڡ�
// ����: 
// **************************************************
static void OnIExploreDestroy( HWND hWnd )
{
	_LPIEDATA	lpied ;
	HWND hItem;
	HICON hIcon;

	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��

	hItem = GetDlgItem(hWnd,IDC_GO); // �õ�GO�Ĵ��ھ��
	hIcon = (HICON)SendMessage(hItem,BM_GETIMAGE,IMAGE_ICON,0); // �õ�GO���ڵ�ICON���

	if (hIcon != NULL)
	{ // �������
		DestroyIcon(hIcon); // �ƻ�ICON
		hIcon = NULL;
	}

	free( lpied ) ; // �ͷ�IE����ṹָ
}
// **************************************************
// ������BOOL CALLBACK IExploreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN message -- ��Ϣ����
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ��������Ϣ������
// ����������IE������ڴ�����̡�
// ����: 
// **************************************************
BOOL CALLBACK IExploreProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_CREATE: // ��������
			if( DoCreate(hWnd,wParam,lParam) == FALSE )
				return -1 ;
			return 0 ;
		case WM_COMMAND: // ������Ϣ
			OnIExploreCommand( hWnd, wParam, lParam ) ;
			break;
		case WM_PAINT: // ������Ϣ
			OnIExplorePaint( hWnd ) ;
			break;
		case WM_ERASEBKGND: // ˢ�±���
			OnIExploreEraseBkgnd( hWnd, (HDC)wParam ) ;
			break ;
		case WM_NOTIFY: // ֪ͨ��Ϣ
			DoNotify(hWnd,wParam,lParam);
			break;
		case WM_DESTROY: // �ƻ���Ϣ
			OnIExploreDestroy( hWnd ) ;
			break ;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
// 	return TRUE;
}
// **************************************************
// ������static int InitialIExploreWindow( HWND hWnd , TCHAR *lpszIndex )
// ������
// 	IN hWnd -- ���ھ��
//	 IN lpszIndex -- Ҫ�򿪵���ҳ
// 
// ����ֵ������0
// ���������� ��ʼ��IE������ڡ�
// ����: 
// **************************************************
static int InitialIExploreWindow( HWND hWnd , TCHAR *lpszIndex )
{
//	_LPIEDATA		lpied ;
	UINT			iIndex;
	HWND hAddress,hIExplore;

//	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��
		
/********************************/
	hAddress = GetDlgItem(hWnd,IDC_ADDRESS); // �õ���ַ�����ھ��
	hIExplore = GetDlgItem(hWnd,IDC_VIEW); // �õ�IE������ھ��
	iIndex = SendMessage( hAddress, CB_FINDSTRING, 0, (LPARAM)lpszIndex ); // ���ҵ�ַ���Ƿ��Ѿ����ڴ˵�ַ
	if ( iIndex == CB_ERR )
	{
//		RETAILMSG(1,(TEXT("Will Add Current Address to ComboBox!!!\r\n")));
		iIndex = SendMessage( hAddress, CB_ADDSTRING, 0, (LPARAM)lpszIndex ); // ����õ�ַ����ַ����
	}
//ETAILMSG(1,(TEXT("Set Current Address Select!!!\r\n")));
	SendMessage( hAddress, CB_SETCURSEL, iIndex, 0 ); // ���øõ�ַΪ��ǰ��ַ
/*******************************/
	SendMessage( hIExplore, HM_GO, 0, (LPARAM)lpszIndex ); // �򿪸���ҳ

	return 0;
}
// **************************************************
// ������static BOOL DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������������ڡ�
// ����: 
// **************************************************
static BOOL DoCreate( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	_LPIEDATA		lpied ;
	HINSTANCE		hInst ;
	RECT rt;
	GetWindowRect(hWnd, &rt);
	lpied = ( _LPIEDATA )malloc( sizeof( _IEDATA ) ) ;  // ����IE������ݽṹ
	if( lpied == NULL )
		return FALSE ; // ����ʧ��
//	RETAILMSG(1,(TEXT("******** new *******lpied = %x\r\n"),lpied));
	SetWindowLong( hWnd, GWL_USERDATA, (long)lpied ) ; // ���ýṹ������

	hInst = ( HINSTANCE )GetWindowLong( hWnd, GWL_HINSTANCE ) ; // �õ�ʵ�����
	// ��������ַ����̬��
	CreateWindow( "Static",  "��ַ:", 
		WS_CHILD|WS_VISIBLE,2, 25, 40, 20,
		hWnd, (HMENU)0xFFFF, hInst, NULL);

//	lpied->hAddress = CreateWindow( "ComboBox",  "", 
//		WS_CHILD|WS_VISIBLE|CBS_DROPDOWN|CBS_AUTOHSCROLL,42, 25, 148, 128,
//		hWnd, (HMENU)IDC_ADDRESS, hInst, NULL);
	// ������ַ��
	lpied->hAddress = CreateWindowEx( WS_EX_CLIENTEDGE,"ComboBox",  "", 
		WS_CHILD|WS_VISIBLE|CBS_DROPDOWN|CBS_AUTOHSCROLL,42, 25, 148, 128,
		hWnd, (HMENU)IDC_ADDRESS, hInst, NULL);
	if( lpied->hAddress == 0 )
		return FALSE ; // ����ʧ��
	// ����GO��ť
	CreateWindow( "Button",  "", 
		WS_CHILD|WS_VISIBLE|BS_ICON,192, 25, 18, 18,
		hWnd, (HMENU)IDC_GO, hInst, NULL);
	SetButtonImage(hWnd,IDC_GO,IDI_GO);

	// ����IE�������
	lpied->hIExplore = CreateWindow( classSMIE_BROWERS, "",
		WS_CHILD|WS_VISIBLE|WS_VSCROLL,0, 46, rt.right - rt. left, rt.bottom - rt.top - 66,
		hWnd, (HMENU)IDC_VIEW, hInst, NULL);
/*		
	lpied->hIExplore = CreateWindow( classHtmlView, "",
		WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_BORDER,0, 45, 240, 235,
		hWnd, (HMENU)IDC_VIEW, hInst, NULL);
*/
	if( lpied->hIExplore == 0 )
		return FALSE ; // ����ʧ��

	if( CreateIExploreToolbar( hWnd ) == FALSE ) // ����������
		return FALSE ; // ����ʧ��

//	InitialIExploreWindow( hWnd, "\\help\\test.htm" );
//	InitialIExploreWindow( hWnd, "http://jami.mlg.com/" );

	return TRUE; // �����ɹ�
}
// **************************************************
// ������static LRESULT DoGo(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������򿪵�ǰ��ҳ��
// ����: 
// **************************************************
static LRESULT DoGo(HWND hWnd)
{
	_LPIEDATA		lpied ;
	TCHAR			lpAddress[MAX_PATH];
	UINT iIndex;

	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��

	SendMessage( lpied->hAddress, WM_GETTEXT, MAX_PATH, (LPARAM)lpAddress ); // �õ���ǰ��ҳ��ַ

/********************************/
//	RETAILMSG(1,(TEXT("Will Enter New Address!!!\r\n")));
	iIndex = SendMessage( lpied->hAddress, CB_FINDSTRING, 0, (LPARAM)lpAddress ); // ���ҵ�ǰ��ҳ�Ƿ��Ѿ����ڵ�ַ�б���
	if ( iIndex == CB_ERR )
	{ // û�д���
//		RETAILMSG(1,(TEXT("Will Add Current Address to ComboBox!!!\r\n")));
		iIndex = SendMessage( lpied->hAddress, CB_ADDSTRING, 0, (LPARAM)lpAddress ); // ����ǰ��ַ���뵽��ַ�б���
	}
//	RETAILMSG(1,(TEXT("Set Current Address Select!!!\r\n")));
	SendMessage( lpied->hAddress, CB_SETCURSEL, iIndex, 0 ); // ����Ҫ�򿪵���ҳΪ��ǰ��ַ
/*******************************/
	SendMessage( lpied->hIExplore, HM_GO, 0, (LPARAM)lpAddress ); // ����ҳ
	return 0;
}
// **************************************************
// ������static LRESULT DoGoBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����������
// 	IN lParam -- ��Ϣ����������
// 
// ����ֵ����
// �����������ص���һ�δ򿪵���ҳ��
// ����: 
// **************************************************
static LRESULT DoGoBack(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPIEDATA		lpied ;

	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��
	SendMessage( lpied->hIExplore, HM_GOBACK, 0, 0 ); // ����һ�δ򿪵���ҳ
	return 0;
}
// **************************************************
// ������static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����������
// 	IN lParam -- ��Ϣ����������
// 
// ����ֵ����
// ������������ǰһ����ҳ��
// ����: 
// **************************************************
static LRESULT DoForWards(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	_LPIEDATA		lpied ;

	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��
	SendMessage( lpied->hIExplore, HM_FORWARDS, 0, 0 ) ; // ��ǰһ����ҳ
	return 0;
}
// **************************************************
// ������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ֪ͨ��Ϣ����
// 	IN lParam -- ֪ͨ��Ϣ����
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	switch(wParam)
	{
		case IDC_VIEW: // IE�����֪ͨ��Ϣ
			DoHtmlViewNotify(hWnd,lParam);
			break;
		default:
			break;
	}
	return 0;
}
// **************************************************
// ������static void DoAddressChange( HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ����
// 
// ����ֵ����
// ������������ַ��ѡ�����ı��֪ͨ��Ϣ��
// ����: 
// **************************************************
static void DoAddressChange( HWND hWnd, LPARAM lParam )
{
	UINT			iIndex;
	LPTSTR			lpUrl;
	DWORD			size;
	_LPIEDATA		lpied ;

	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��

	iIndex = SendMessage( lpied->hAddress, CB_GETCURSEL, 0, 0 ) ; // �õ���ǰ��ַ����
	size = SendMessage( lpied->hAddress, CB_GETLBTEXTLEN, iIndex, 0 ); // �õ���ǰ��ַ�ĳ���
	lpUrl = (LPTSTR)malloc( size+2 ); // �����ַ�ռ�
	if( lpUrl == NULL )
		return  ; // ����ʧ��
	size = SendMessage( lpied->hAddress, CB_GETLBTEXT, iIndex, (LPARAM)lpUrl ); // �õ���ǰ�ĵ�ַ
	SendMessage(lpied->hIExplore, HM_GO, 0, (LPARAM)lpUrl ); // �򿪵�ǰ��ַ����ҳ
	free( lpUrl );
} 
// **************************************************
// ������static LRESULT DoAddressNotify(HWND hWnd,WORD wNotifyCode,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wNotifyCode -- ֪ͨ��Ϣ
// 	IN lParam -- ����
// 
// ����ֵ��������Ϣ������
// ������������ַ����֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoAddressNotify(HWND hWnd,WORD wNotifyCode,LPARAM lParam)
{
	switch(wNotifyCode)
	{
		case CBN_SELCHANGE: // ��ַ���ĵ�ǰѡ�����ı�
			DoAddressChange( hWnd, lParam );
			break;
		default :
			break;
	}
	return 0;
}
// **************************************************
// ������static LRESULT DoHtmlViewNotify( HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ֪ͨ��Ϣ����
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static LRESULT DoHtmlViewNotify( HWND hWnd, LPARAM lParam )
{
	LPNMINTERNET	lpnmInternet;
	UINT			iIndex;
//	LPTSTR			lpUrl ;//lpBuffer=NULL;
//	_LPIEDATA		lpied ;
	HWND hAddress;

//	lpied = GetIEPtr( hWnd ) ; // �õ�IE����ṹָ��

//	return 0;
	lpnmInternet=(LPNMINTERNET)lParam; // �õ�֪ͨ��Ϣ�ṹ���

/*	if ( IsLocateFile( lpnmInternet->lpUrl ) )
	{
 		lpUrl = lpnmInternet->lpUrl;
	}
	else
	{
		lpUrl = lpnmInternet->lpUrl;
	}
*/
//	RETAILMSG(1,(TEXT("lpied = %x!!!\r\n"),lpied));
//	RETAILMSG(1,(TEXT("lpied->hAddress = %x!!!\r\n"),lpied->hAddress));

	hAddress = GetDlgItem(hWnd,IDC_ADDRESS); // �õ���ַ�����
//	RETAILMSG(1,(TEXT("Will Enter New Address!!!\r\n")));
	iIndex = SendMessage( hAddress, CB_FINDSTRING, 0, (LPARAM)lpnmInternet->lpUrl ); // ���ҵ�ǰ�򿪵���ҳ�Ƿ��ڵ�ַ�б���
	if ( iIndex == CB_ERR )
	{ // û�д���
//		RETAILMSG(1,(TEXT("Will Add Current Address to ComboBox!!!\r\n")));
		iIndex = SendMessage( hAddress, CB_ADDSTRING, 0, (LPARAM)lpnmInternet->lpUrl ); // ����ǰ�򿪵���ҳ��ַ���뵽��ַ�б���
	}
//	RETAILMSG(1,(TEXT("Set Current Address Select!!!\r\n")));
	SendMessage( hAddress, CB_SETCURSEL, iIndex, 0 ); // ���õ�ǰ�򿪵���ҳ��ַΪ��ǰ��ַ

//	RETAILMSG(1,(TEXT("OK!!!!!!!!!!!!!!\r\n")));
	return 0;
		
}
// **************************************************
// ������static BOOL IsLocateFile( LPTSTR lpUrl )
// ������
// 	IN lpUrl -- ָ����URL(��ҳ��ַ)
// 
// ����ֵ��ָ����URL�Ǳ����ļ�����TRUE�����򷵻�FALSE
// �����������ж�ָ����URL�Ƿ��Ǳ����ļ���
// ����: 
// **************************************************
static BOOL IsLocateFile( LPTSTR lpUrl )
{
	HANDLE in_file;
	// ��ָ����URL
	in_file = CreateFile( lpUrl, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL );
	if ( in_file == INVALID_HANDLE_VALUE )
	{  // ��ʧ�ܣ����Ǳ����ļ�
		return FALSE;
	}
	CloseHandle(in_file); // �ر��ļ����
	return TRUE;
}

// **************************************************
// ������static BOOL CreateIExploreToolbar( HWND hWnd )
// ������
// 	IN hWnd -- ���ھ�� 
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������������������
// ����: 
// **************************************************
static BOOL CreateIExploreToolbar( HWND hWnd )
{
	_LPIEDATA	lpied ;
	HWND		hToolbar ;
	TBBUTTON	tb;
	int			i;

typedef struct IEToolBar{
	BOOL bSysIcon;
	UINT ID;
	UINT IDResource;
	LPTSTR lpString;
}IETOOLBAR;
	IETOOLBAR IEToolBar[]={
	{TRUE,IDC_OPEN,OBM_TOOL_OPEN,"Open"}, // ���ļ�
	{FALSE,IDC_SAVE,IDB_SAVE,"Save"}, // �����ļ�
	{FALSE,IDC_GOBACK,IDB_GOBACK,"GoBack"}, // ����һ���Ѿ��򿪹����ļ�
	{FALSE,IDC_FORWARDS,IDB_FORWARDS,"Forwards"}, // ��ǰһ���Ѿ��򿪹����ļ�
	{FALSE,IDC_FRESH,IDB_FRESH,"Fresh"}, // ˢ�µ�ǰ�򿪵���ҳ
	{FALSE,IDC_STOP,IDB_STOP,"Stop"}, // ֹͣ����ҳ
	{TRUE, IDC_EMAIL,OBM_TOOL_EMAIL,"Email"}, // ����EMAIL
};
	HIMAGELIST hImageList;
	HBITMAP hBitmap;
	UINT iToolBarNum ;

	lpied = GetIEPtr( hWnd ) ;// �õ�IE����ṹָ��

	// ����һ��������
	hToolbar = CreateToolbarEx( hWnd, WS_VISIBLE|WS_CHILD|TBSTYLE_FLAT|TBSTYLE_LIST|CCS_TOP,
			11, 0, NULL, 0, NULL,0,20,20,16,16,0 );
	if( hToolbar == 0 )
	{ // ����ʧ��
		EdbgOutputDebugString("=========Create ToolBar is Failure===========\r\n");
		return 0;
	}
	iToolBarNum = sizeof(IEToolBar) / sizeof(IETOOLBAR ); // �õ���������ť�ĸ���
	hImageList=ImageList_Create(16,16,ILC_COLOR8,10,10); // ����һ��ͼ���б�
	for (i=0;i<(int)iToolBarNum;i++)
	{
		// װ��ͼ��
		if (IEToolBar[i].bSysIcon == TRUE)
			hBitmap = LoadImage( NULL, MAKEINTRESOURCE(IEToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;
		else
			hBitmap = LoadImage( (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IEToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;
		// ��ͼ����뵽ͼ���б���
		if (hBitmap )
			ImageList_Add(hImageList,hBitmap,NULL);
	}

//	SendMessage( hToolbar, TB_LOADIMAGES , IDB_STD_SMALL_COLOR , ( LPARAM )HINST_COMMCTRL ) ;
	hImageList =(HIMAGELIST)SendMessage( hToolbar, TB_SETIMAGELIST , 0, ( LPARAM )hImageList) ; // ����ͼ���б�������
	
	// ɾ��ԭ����ImageList
	if (hImageList)
		ImageList_Destroy(hImageList);

	tb.iString = 0 ;
	tb.fsState = TBSTATE_ENABLED  ;
	tb.fsStyle = TBSTYLE_BUTTON ;
	for( i = 0; i < (int)iToolBarNum; i ++ )
	{
		tb.iBitmap = i;
		tb.idCommand = IEToolBar[i].ID;
		SendMessage( hToolbar, TB_ADDBUTTONS, 1, (LPARAM)&tb ); // ����ÿһ����ť
	}
	lpied->hToolbar = hToolbar ; // ������
	return TRUE;
}
// **************************************************
// ������static void SetButtonImage(HWND hWnd,UINT id,UINT iImage)
// ������
// 	IN hWnd -- ���ھ��
// 	IN id -- ��ť��ID��
// 	IN iImage  -- ͼ��ID��
// 
// ����ֵ����
// ������������һ��ָ��ID�ŵİ�ť�趨һ��ͼ��
// ����: 
// **************************************************
static void SetButtonImage(HWND hWnd,UINT id,UINT iImage)
{
	//HBITMAP hBitmap;
	HICON hIcon;
	HWND hItem;
	HINSTANCE hInstance;


	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
	hItem = GetDlgItem(hWnd,id); // �õ���ť���
	hIcon = LoadImage(hInstance,MAKEINTRESOURCE(iImage),IMAGE_ICON,16,16,0); // װ��ͼ��
	hIcon = (HICON)SendMessage(hItem,BM_SETIMAGE,IMAGE_ICON,(LPARAM)hIcon);// ����ͼ�����ť

	if (hIcon != NULL)
	{ // ԭ����ͼ��
		DestroyIcon(hIcon); // ɾ��ԭ����ͼ��
		hIcon = NULL;
	}
}


// **************************************************
// ������static LRESULT DoOpen(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������ļ���
// ����: 
// **************************************************
static LRESULT DoOpen(HWND hWnd)
{
	TCHAR lpFileName[MAX_PATH];

	lpFileName[0] = 0;

	if (GetFileName(hWnd,lpFileName,OPENHTML)== TRUE) // �õ�Ҫ�򿪵��ļ���
	{
		InitialIExploreWindow( hWnd , lpFileName); // ��ָ�����ļ�
		return TRUE; // ���سɹ�
	}
	return FALSE;
}
// **************************************************
// ������static LRESULT DoSave(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������浱ǰ�򿪵���ҳ��
// ����: 
// **************************************************
static LRESULT DoSave(HWND hWnd)
{
	TCHAR lpFileName[MAX_PATH];
	LPTSTR lpData;
	int iDataLen;
	HANDLE hFile;
	HWND hIEBrowser;
	int iFileType;

	lpFileName[0] = 0;

	hIEBrowser = (HWND )GetDlgItem(hWnd,IDC_VIEW); // �õ�IE����Ĵ��ھ��
	iFileType = SendMessage(hIEBrowser,HM_GETFILETYPE,0,0); // �õ��򿪵��ļ�����
	if (iFileType == HTMLFILE)
	{ // ��һ���ļ�
		if (GetFileName(hWnd,lpFileName,SAVEHTML) == FALSE) // �õ�Ҫ������ļ���
		{
			return FALSE;
		}
		if (HaveExtName(lpFileName) == FALSE) // �Ƿ������չ��
		{
			AddExtName(lpFileName,"htm"); // ����Ĭ����չ��
		}
	}
	else
	{ // ��һ��ͼ��
		if (GetFileName(hWnd,lpFileName,SAVEIMG) == FALSE) // �õ�Ҫ������ļ���
		{
			return FALSE;
		}
		if (HaveExtName(lpFileName) == FALSE) // �Ƿ������չ��
		{
			AddExtName(lpFileName,"gif"); // ����Ĭ����չ��
		}
	}
	iDataLen = SendMessage(hIEBrowser,HM_READDATA,0,0); // �õ���ҳ���ݵĳ���
	if (iDataLen == -1)
		return FALSE;
	lpData= (LPTSTR)malloc(iDataLen * sizeof(TCHAR)); // �������ݿռ�
	if (lpData)
	{ // ����ɹ�
		if (SendMessage(hIEBrowser,HM_READDATA,iDataLen,(LPARAM)lpData) > 0) // �õ�����
		{ // �õ����ݳɹ�
			hFile = CreateFile(lpFileName,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_ARCHIVE,NULL); // ��Ҫ������ļ���
			WriteFile( hFile, lpData, iDataLen,&iDataLen,NULL); // ������д�뵽�ļ�
			CloseHandle(hFile); // �ر��ļ�
		}
		free(lpData); // �ͷ����ݻ���
	}
	return TRUE;
}
// **************************************************
// ������static LRESULT DoEmail(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������ǰ�򿪵���ҳ������EMAIL���ͳ�ȥ��
// ����: 
// **************************************************

static LRESULT DoEmail(HWND hWnd)
{
#if 0	
	EmailFormat stEmailFormat;
	LPTSTR lpData;
	int iDataLen;
	HWND hIEBrowser;
	int iFileType;

	hIEBrowser = (HWND )GetDlgItem(hWnd,IDC_VIEW); // �õ�IE����Ĵ��ھ��

	iFileType = SendMessage(hIEBrowser,HM_GETFILETYPE,0,0); // �õ��ļ�����
	if (iFileType == HTMLFILE)
	{ // ��HTML�ļ�
		iDataLen = SendMessage(hIEBrowser,HM_READDATA,0,0); // �õ����ݳ���
		if (iDataLen == -1)
			return FALSE; // û������
		lpData= (LPTSTR)malloc((iDataLen+1) * sizeof(TCHAR)); // �������ݻ���
		if (lpData)
		{ // ����ɹ�
			memset(lpData,0,(iDataLen+1) * sizeof(TCHAR)); // ��ջ���
			if (SendMessage(hIEBrowser,HM_READDATA,iDataLen,(LPARAM)lpData) > 0) // �õ�����
			{ // �õ����ݳɹ�
				InitEmailFormat( &stEmailFormat ); // ��ʼ��EMIAL�ṹ
				stEmailFormat.Body = lpData; // ����EMIAL����
				stEmailFormat.BodyType = 2;  // html file type
				SendEmailTo( &stEmailFormat ); // �����ʼ�
				DeInitEmailFormat ( &stEmailFormat );// lpData will Be Free
			}
			else
			{
				free(lpData); // �ͷŻ���
			}
		}
		return TRUE;
	}
	else if (iFileType == IMAGEFILE)
	{ // ��ͼ���ļ����ø����ķ�ʽ���ͳ�ȥ
		EmailAttach *pemailAttach; 
		lpData= (LPTSTR)malloc(MAX_PATH * sizeof(TCHAR)); // �����ļ�·������
		if (lpData)
		{
			memset(lpData,0,MAX_PATH * sizeof(TCHAR)); // ��ջ���
			if (SendMessage(hIEBrowser,HM_GETLOCATEFILE,MAX_PATH,(LPARAM)lpData) > 0) // �õ�ͼ���ļ��ı����ļ���
			{ // �ɹ�
				InitEmailFormat( &stEmailFormat ); // ��ʼ���ʼ��ṹ
				pemailAttach = (EmailAttach *)malloc(sizeof(EmailAttach)); // �����ʼ������Ľṹ
				if (pemailAttach == NULL)
				{ // ����ʧ��
					DeInitEmailFormat ( &stEmailFormat ); // �ͷ��ʼ��ṹ
					return FALSE;
				}
				pemailAttach->Name = lpData; // ��Ӹ����ļ���
				stEmailFormat.AttachCount= 1; // �����ļ�����
				stEmailFormat.Attach[0] = pemailAttach; // ���ø����ṹ
				stEmailFormat.BodyType = 3;  // other type
				SendEmailTo( &stEmailFormat ); // �����ʼ�
				DeInitEmailFormat ( &stEmailFormat );	// lpData will Be Free
				//free(pemailAttach);
				//free(lpData);
			}
		}
	}
#endif	
	return FALSE;
}

// **************************************************
// ������static LRESULT DoStop(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������ֹͣ��ǰ��ҳ��
// ����: 
// **************************************************
static LRESULT DoStop(HWND hWnd)
{
	HWND hIEBrowser;

	hIEBrowser = (HWND )GetDlgItem(hWnd,IDC_VIEW); // �õ�IE������ھ��
	SendMessage(hIEBrowser,HM_STOP,0,0); // ֹͣ��ǰ��ҳ
	return 0;
}

// **************************************************
// ������static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName,WORD OpenStyle)
// ������
// 	IN hDlg -- ���ھ��
// 	OUT lpFileName -- ����õ����ļ���
// 	IN  OpenStyle -- ������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ��ļ�����
// ����: 
// **************************************************
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName,WORD OpenStyle)
{
    TCHAR OpenTitle[256];
	TCHAR szFilter[MAX_PATH]; 	



	OPENFILENAME OpenFileName;
	TCHAR lpstrFilter[30];
	TCHAR strFile[MAX_PATH];
	TCHAR TitleText[10];


	if (OpenStyle==OPENHTML)
	{  // ��һ��HTML�ļ�
//		LoadString(hInst,IDS_OPENTITLE,OpenTitle,MAX_LOADSTRING);
		strcpy(OpenTitle,"���ļ�"); // ����
//		LoadString(hInst,IDS_MP3FILTER,szFilter,MAX_LOADSTRING);
		strcpy(szFilter,"*.htm"); // ���Դ򿪵��ļ�����

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
		sprintf(lpstrFilter+strlen(szFilter)+1, TEXT("%s"), TEXT("*.*"));   
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_FILEMUSTEXIST; // ֧�ֳ��ļ������ļ��������
	}
	else if (OpenStyle==SAVEHTML)
	{  // ����һ��HTML�ļ�
//		LoadString(hInst,IDS_SAVETITLE,OpenTitle,MAX_LOADSTRING);
		strcpy(OpenTitle,"�����ļ�"); // ����
//		LoadString(hInst,IDS_PLSFILTER,szFilter,MAX_LOADSTRING);
		strcpy(szFilter,"*.htm"); // ���Դ򿪵��ļ�����

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
		sprintf(lpstrFilter+strlen(szFilter)+1, TEXT("%s"), TEXT("*.*"));   
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // ֧�ֳ��ļ����͸���ԭ���ļ�
	}
	else if (OpenStyle==SAVEIMG)
	{  // ����ͼ��
//		LoadString(hInst,IDS_SAVETITLE,OpenTitle,MAX_LOADSTRING);
		strcpy(OpenTitle,"�����ļ�"); // ����
//		LoadString(hInst,IDS_PLSFILTER,szFilter,MAX_LOADSTRING);
		strcpy(szFilter,"*.gif"); // ���Դ򿪵��ļ�����

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // ֧�ֳ��ļ����͸���ԭ���ļ�
	}

//	memset(FileName,0,sizeof(FileName));
	memset(strFile,0,sizeof(strFile)); // ����ļ�������
	sprintf(TitleText, TEXT("%s"), OpenTitle); // �õ�����

	OpenFileName.lStructSize=sizeof(OPENFILENAME); 
    OpenFileName.hwndOwner=hDlg;
//	OpenFileName.hInstance=0;
    OpenFileName.lpstrFilter=lpstrFilter; //NULL
//	OpenFileName.lpstrCustomFilter=NULL;
//  OpenFileName.nMaxCustFilter=0; 
	OpenFileName.nFilterIndex=1; 
    OpenFileName.lpstrFile=lpFileName; 
	OpenFileName.nMaxFile=MAX_PATH; 
    OpenFileName.lpstrFileTitle=strFile;  //only filename an extension(withou t path information) 
	OpenFileName.nMaxFileTitle=MAX_PATH; 
    OpenFileName.lpstrInitialDir="\\"; 
    OpenFileName.lpstrTitle=TitleText;   
//    OpenFileName.Flags=OFN_CREATEPROMPT;
	OpenFileName.nFileOffset=0; 
    OpenFileName.nFileExtension=0; 
	OpenFileName.lpstrDefExt=NULL; 
	if (OpenStyle==OPENHTML) 
	{ // ���ļ�
		if( GetOpenFileName(&OpenFileName) == FALSE) 
			return FALSE; // �õ��ļ���ʧ��
	}
	else
	{ // �����ļ�
		if( GetSaveFileName(&OpenFileName) == FALSE) 
			return FALSE; // �õ��ļ���ʧ��
	}
//	_wsplitpath(lpFileName,NULL,curPath,NULL,NULL);
//	_splitpath(lpFileName,NULL,curPath,NULL,NULL);
	return TRUE; // �õ��ļ����ɹ�
}


// **************************************************
// ������static BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// ������
// 	IN hRoot -- ����
// 	IN lpKey -- Ҫ�򿪵ļ�
// 	IN lpValueName -- ��ֵ��
// 	IN lpData -- �õ����ݵĻ���
// 	IN dwDataLen -- ���ݳ���
// 	IN dwType -- ��������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ȡע���
// ����: 
// **************************************************
static BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegOpenKeyEx(hRoot, lpKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) // ��ע���
	 { // �򿪳ɹ�
		 if (RegQueryValueEx(hKey, lpValueName, NULL, &dwType, (LPBYTE)lpData, &dwDataLen) == ERROR_SUCCESS) // ��ȡ��ֵ����
		 { // ��ȡ�ɹ�
			RegCloseKey(hKey); // �رռ�
			return TRUE; // ���سɹ�
		 }
		 RegCloseKey(hKey); // �رռ�
	 }
	 return FALSE; // ����ʧ��
}
// **************************************************
// ������static BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// ������
// 	IN hRoot -- ����
// 	IN lpKey -- Ҫ�򿪵ļ�
// 	IN lpValueName -- ��ֵ��
// 	IN lpData -- Ҫд������ݻ���
// 	IN dwDataLen -- ���ݳ���
// 	IN dwType -- ��������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ȡע���
// ����: 
// **************************************************
static BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegCreateKeyEx(hRoot, lpKey, 0,NULL,0, KEY_ALL_ACCESS, NULL,&hKey,0) == ERROR_SUCCESS) // �򿪼�
	 { // �ɹ�
		 if (RegSetValueEx(hKey, lpValueName, NULL, (DWORD)dwType, (LPBYTE)lpData, dwDataLen) == ERROR_SUCCESS) // ���ü�ֵ����
		 { // ���óɹ�
			 RegCloseKey(hKey);
			 return TRUE; // ���سɹ�
		 }
		 RegCloseKey(hKey);
	 }
	 return FALSE; // ����ʧ��
}


// !!! Add By Jami chen in 2003.09.12
// **************************************************
// ������static BOOL HaveExtName(LPTSTR lpFileName)
// ������
// 	IN lpFileName -- ָ�����ļ���
// 
// ����ֵ������չ������TRUE�����򷵻�FALSE
// �����������õ�ָ���ļ��Ƿ������չ����
// ����: 
// **************************************************
static BOOL HaveExtName(LPTSTR lpFileName)
{
	LPTSTR lpCurPos;

	lpCurPos = lpFileName; // �õ��ļ���ͷָ��
	while(*lpCurPos)
	{
		if (*lpCurPos == '.') // �Ƿ������չ���ָ���
			return TRUE; // ����չ��
		lpCurPos ++;
	}
	return FALSE; // û����չ��
}
// **************************************************
// ������static void AddExtName(LPTSTR lpFileName,LPTSTR lpExtName)
// ������
// 	IN lpFileName -- ָ���ļ���
// 	IN lpExtName -- ָ����չ��
// 
// ����ֵ����
// ������������ָ���ļ��ϼ���һ����չ����
// ����: 
// **************************************************
static void AddExtName(LPTSTR lpFileName,LPTSTR lpExtName)
{
	if ((strlen(lpFileName) + strlen(lpExtName) + 1)  >= MAX_PATH) // �õ��Ƿ���Լ�����չ��
		return ;
	strcat(lpFileName,"."); // �����չ���ָ���
	strcat(lpFileName,lpExtName); // �����չ��
	return ;
}
// !!! Add End By Jami chen in 2003.09.12