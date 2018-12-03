/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�������ݣ��û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-03-18
���ߣ�����
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
#include <eoemkey.h>
#include <ecomctrl.h>
#include <eprogres.h>
#include <eoemkey.h>
/***************  ȫ���� ���壬 ���� *****************/

#define CLR_TRANSPARENT ( RGB( 255, 0, 255 ) )

static const char strClassButton[] = "AGL_BUTTON"; 

#define IDC_BUTTON_ALL 100
#define IDC_BUTTON_SEL 101

#define IDC_PRO_FILE   100
#define IDC_PRO_TOTAL  101

#define WM_RUNPROC (WM_USER+1)

#define CLR_BACKGROUND RGB( 13, 13, 13 )

static HBITMAP hbmpBackground = NULL;
static HBITMAP hbmpCopyCFSD = NULL;
static HBITMAP hbmpCopySD = NULL;
static HBITMAP hbmpCopyCF = NULL;
static HBITMAP hbmpCopyHDD = NULL;

//static HWND hwndMainControl;

enum { 
	ID_RUN_SD,
	ID_RUN_CF,
	ID_RUN_CFSD,
	ID_RUN_HDD
};
static UINT uiCurrentRunID;

LRESULT CALLBACK
BackupWndProc(
			  HWND hWnd, 
			  UINT uiMsg, 
			  WPARAM wParam, 
			  LPARAM lParam );


static LRESULT CALLBACK BackupMethodProc(
						HWND hwndDlg,
						UINT uMsg,
						WPARAM wParam,
						LPARAM lParam );
static LRESULT CALLBACK BackupProgressProc(
						HWND hwndDlg,
						UINT uMsg,
						WPARAM wParam,
						LPARAM lParam );

#define BUTTON_HEIGHT  47
#define BUTTON_WIDTH   400

//�Ի���ģ��
static const struct _DIALOGTMP{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[2];
}dlgIDD_BackupMethod = 
{
    { 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE|WS_POPUP, sizeof( ((struct _DIALOGTMP*)0)->item ) / sizeof(DLG_ITEMTEMPLATE_EX), 0, 50,640, 430,0, 0, "" },
    {   
		{ WS_EX_NOFOCUS, BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE, 100, 100, BUTTON_WIDTH, BUTTON_HEIGHT, IDC_BUTTON_ALL, strClassButton,"All",0 },
		{ WS_EX_NOFOCUS, BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE, 100, 200, BUTTON_WIDTH, BUTTON_HEIGHT, IDC_BUTTON_SEL, strClassButton, "Select", 0 }
    } 
};


#define PROGRESS_HEIGHT 12
#define PROGRESS_WIDTH  540

static const struct {
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[2];
}dlgIDD_BackupProgress = 
{
    { 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE|WS_POPUP, sizeof( ((struct _DIALOGTMP*)0)->item ) / sizeof(DLG_ITEMTEMPLATE_EX), 0, 50,640, 430,0, 0, "" },
    {   
		{ 0, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 50, 282 - 50, PROGRESS_WIDTH, PROGRESS_HEIGHT, IDC_PRO_FILE, classPROGRESS,"All",0 },
		{ 0, WS_CHILD | WS_VISIBLE | PBS_SMOOTH, 50, 363 - 50, PROGRESS_WIDTH, PROGRESS_HEIGHT, IDC_PRO_TOTAL, classPROGRESS, "Select", 0 }
    } 
};


// **************************************************
// ������static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ��С
// 	IN xOffset -- Xƫ��
// 	IN yOffset -- Yƫ��
// 	IN dwRop -- ����ģʽ
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawBitMap( HDC hdc,HBITMAP hBitmap, LPRECT lprect,UINT xOffset,UINT yOffset,DWORD dwRop )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // ����һ�����ݵ��ڴ���
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼ���õ��ڴ�����
		BitBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			dwRop
			);

		SelectObject(hMemoryDC,hOldBitmap); // �ָ��ڴ���
		DeleteDC(hMemoryDC); // ɾ�����
}

// **************************************************
// ������static void DrawTransparentBitMap(HDC hdc,HBITMAP hBitmap,RECT * rect,UINT xOffset,UINT yOffset,DWORD clrTranspant )
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ��С
// 	IN xOffset -- Xƫ��
// 	IN yOffset -- Yƫ��
// 	IN clrTranspant -- ͸��ɫ
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawTransparentBitMap(HDC hdc,HBITMAP hBitmap, const RECT * lprect,UINT xOffset,UINT yOffset, DWORD clrTranspant )
{
		HDC hMemoryDC;
        HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // ����һ�����ݵ��ڴ���
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼ���õ��ڴ�����
		TransparentBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			(short)xOffset,   // x-coordinate of source rectangle's upper-left
									 // corner
			(short)yOffset,   // y-coordinate of source rectangle's upper-left
									 // corner

			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			clrTranspant
			);

		SelectObject(hMemoryDC,hOldBitmap); // �ָ��ڴ���
		DeleteDC(hMemoryDC); // ɾ�����
}

// **************************************************
// ������static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
// ������
//	IN hdc - ��ʾ�豸���
//	IN lprc - RECT �ṹָ�룬ָ����Ҫ���ľ���
//	IN clr - RGB��ɫֵ
// ����ֵ��
//	��
// ����������
//	�ù̶���ɫ�����о���
// ����: 
//	
// ************************************************

static void FillSolidRect( HDC hdc, RECT * lprc, COLORREF clr )
{
	SetBkColor( hdc, clr );
	ExtTextOut( hdc, 0, 0, ETO_OPAQUE, lprc, NULL, 0, NULL );
}


// ��ʼ��ȫ������
static VOID InitBackupData( VOID )
{
	if( hbmpBackground == NULL )
	    hbmpBackground = LoadImage( NULL, MAKEINTRESOURCE( OBM_BACKGROUND ), IMAGE_BITMAP, 0, 0, LR_SHARED );
    if( hbmpCopyCFSD == NULL )
	{
		hbmpCopyCFSD = LoadImage( NULL, "./kingmos/backup/sts_cpy_md_cfsd.bmp" , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	}
    if( hbmpCopySD == NULL )
	{
		hbmpCopySD = LoadImage( NULL, "./kingmos/backup/sts_cpy_md_sd.bmp" , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	}
    if( hbmpCopyCF == NULL )
	{
		hbmpCopyCF = LoadImage( NULL, "./kingmos/backup/sts_cpy_md_cf.bmp" , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	}
    if( hbmpCopyHDD == NULL )
	{
		hbmpCopyHDD = LoadImage( NULL, "./kingmos/backup/sts_cpy_md_hdd.bmp" , IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	}

}

// **************************************************
//������static UINT GetRunID( LPCTSTR lpszCommandLine )
//������
//	lpszCommandLine - �����в���
//����ֵ��
//	�ɹ� ��Ҫ���еĴ��� id; 0, ʧ��
//����������
//	�õ���Ҫ���еĴ��� id
//����: 
// ************************************************
static UINT GetRunID( LPCTSTR lpszCommandLine )
{
	UINT retv = 0;
//	TCHAR szValueName[32];
	if( lpszCommandLine )
	{
		if( stricmp( lpszCommandLine, "HDD" ) == 0 )
		{
			return ID_RUN_HDD;
		}
		if( stricmp( lpszCommandLine, "SD Card" ) == 0 )
		{
			return ID_RUN_SD;
		}
		if( stricmp( lpszCommandLine, "CF Card" ) == 0 )
		{
			return ID_RUN_CF;
		}
		if( stricmp( lpszCommandLine, "CFSD Card" ) == 0 )
		{
			return ID_RUN_CFSD;
		}

	}
	return 0; //����
}

// ********************************************************************
// ������ATOM RegisterBackupMainClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
static TCHAR szMainWindowClass[] = "classBackupMainWindow";
ATOM RegisterBackupMainClass(HINSTANCE hInstance)
{
	WNDCLASS wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)BackupWndProc; // �绰���ش��ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG);
	wc.hInstance		= hInstance; // ʵ�����
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szMainWindowClass; // Ӧ�ó�������

	return RegisterClass(&wc); // ע����
}



// **************************************************
// ������LRESULT CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR     lpCmdLine,int       nCmdShow)
// ������
// 	IN hInstance -- Ӧ�ó���ʵ�����
// 	IN hPrevInstance -- ǰһ��Ӧ�ó���ʵ�����
// 	IN lpCmdLine -- Ӧ�ó��������в���
// 	IN nCmdShow -- ��ʾ��־
// 
// ����ֵ������TRUE
// ����������Ӧ�ó������
// ����: 
// **************************************************
#ifdef INLINE_PROGRAM
int WINAPI WinMain_Backup(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
#else
LRESULT CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
    MSG				msg;
    HANDLE hMutex;
	UINT id;
	HWND hWnd;
		   
	RETAILMSG( 1, ( "Start Method ...\r\n" ) );
	   
    id = GetRunID( lpCmdLine );
	   
	   
	hMutex = CreateMutex( NULL, FALSE, "__Backup__");
	if (hMutex)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// �����Ѿ�����
			while( 1 )
			{
				HWND hWnd = FindWindow( szMainWindowClass, NULL );
				if( hWnd )
				{    //�ֻ������Ѿ�����
					PostMessage( hWnd, WM_RUNPROC, id, NULL );
					return 0;
				}
				Sleep( 500 );
			}
		}
	}
	   
	   
    InitBackupData(); 
    InitCommonControls();
    InitThirdpartControls();

	RegisterBackupMainClass( hInstance );
    //�������صĶԻ���
	hWnd = CreateWindow( szMainWindowClass, 
													"",
													0,
													0,
													50,
													640,
													480 - 50,
													NULL,
													NULL,
													hInstance,
													NULL );
	PostMessage( hWnd, WM_RUNPROC, id, NULL );
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		/*
		if( msg.message == WM_KEYDOWN )
		{
			if( msg.hwnd )
			{
				if( GetWindowLong( msg.hwnd, GWL_STYLE ) & WS_CHILD )
				{	//������Ӵ��ڣ�����丸Ҳ��һ��
					SendMessage( GetParent( msg.hwnd ), WM_KEYDOWN, msg.wParam, msg.lParam );
				}
			}
		}
		*/
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}




   
   
	   /*
	   while (GetMessage(&msg, NULL, 0, 0)) 
	   {	// ��Ϣ����
	   //
	   TranslateMessage(&msg);
	   DispatchMessage(&msg);
	   }
	   
   */
   return 0;
}

static void ShowInfo( HWND hWnd,   //
					  HDC hdc, 
					  UINT id   //��Ҫ��ʾ����Ϣ id: all or select
					  )
{
	char * lpszInfo[] = {
		"All of files in memory card will be backed up",   // all
		"Only selected files in memory card will be backed up"    // select
	};
	int i;
	RECT rc;

	if( id == IDC_BUTTON_ALL )
		i = 0;
	else
		i = 1;

	rc.left = 20;
	rc.top = 400 - 50;
	rc.right = 630;
	rc.bottom = 480 - 50;

	FillSolidRect( hdc, &rc, CLR_BACKGROUND );
	SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
	TextOut( hdc, rc.left, rc.top, lpszInfo[i], strlen( lpszInfo[i] ) );

}

// ********************************************************************
// ������static BOOL DoMethodCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//	IN wParam - WM_COMMAND ������ wParam ����
//	IN lParam - WM_COMMAND ������ lParam ����
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE
// ����������
//	���� WM_COMMAND ��Ϣ
// ����:
// ********************************************************************
/*
static BOOL DoMethodCommand( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	BOOL bRetv = TRUE;
	if( HIWORD( wParam ) == BN_SETFOCUS ||
		HIWORD( wParam ) == BN_KILLFOCUS)
	{
		HDC hdc;
		switch( LOWORD( wParam ) )
		{
		case IDC_BUTTON_ALL:	  //
			hdc = GetDC( hWnd );
			ShowInfo( hWnd, hdc, IDC_BUTTON_ALL );
			ReleaseDC( hWnd, hdc );
			
			break;
		case IDC_BUTTON_SEL:    
			hdc = GetDC( hWnd );//
			ShowInfo( hWnd, hdc, IDC_BUTTON_SEL );
			ReleaseDC( hWnd, hdc );
			break;			
		default:
			bRetv = FALSE;
		}
	}
	else if( HIWORD( wParam ) == BN_CLICKED )
	{
		switch( LOWORD( wParam ) )
		{
		case IDC_BUTTON_ALL:	  //
			EndDialog( hWnd, IDC_BUTTON_ALL );
			break;
		case IDC_BUTTON_SEL:       //
			EndDialog( hWnd, IDC_BUTTON_SEL );
			break;
			
		default:
			bRetv = FALSE;
		}
	}
	return bRetv;
}
*/

// ********************************************************************
// ���� WM_KEYDOWN ��Ϣ
static LRESULT DoMethodKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	if( wParam == VK_CANCEL )
	{
		EndDialog( hWnd, 0 );
	}
	else if( wParam == VK_UP || wParam == VK_DOWN )
	{
		HWND hChildWnd = GetDlgItem( hWnd, IDC_BUTTON_ALL );
		if( SendMessage( hChildWnd, BM_GETCHECK, BST_CHECKED, 0 ) == BST_CHECKED )
		{
			SendMessage( hChildWnd, BM_SETCHECK, BST_UNCHECKED, 0 );		    
		}
		else
		{
			HDC hdc;
			SendMessage( hChildWnd, BM_SETCHECK, BST_CHECKED, 0 );			

			hdc = GetDC( hWnd );
			ShowInfo( hWnd, hdc, IDC_BUTTON_ALL );
			ReleaseDC( hWnd, hdc );
		}

		hChildWnd = GetDlgItem( hWnd, IDC_BUTTON_SEL );
		if( SendMessage( hChildWnd, BM_GETCHECK, BST_CHECKED, 0 ) == BST_CHECKED )
		{
		    SendMessage( hChildWnd, BM_SETCHECK, BST_UNCHECKED, 0 );			
		}
		else
		{
			HDC hdc;

			SendMessage( hChildWnd, BM_SETCHECK, BST_CHECKED, 0 );

			hdc = GetDC( hWnd );
			ShowInfo( hWnd, hdc, IDC_BUTTON_SEL );
			ReleaseDC( hWnd, hdc );

		}

	}
	else if( wParam == VK_SET )
	{
		HWND hChildWnd = GetDlgItem( hWnd, IDC_BUTTON_ALL );
		if( SendMessage( hChildWnd, BM_GETCHECK, BST_CHECKED, 0 ) == BST_CHECKED )
			EndDialog( hWnd, IDC_BUTTON_ALL );
		else
		{
			hChildWnd = GetDlgItem( hWnd, IDC_BUTTON_SEL );
			if( SendMessage( hChildWnd, BM_GETCHECK, BST_CHECKED, 0 ) == BST_CHECKED )
				EndDialog( hWnd, IDC_BUTTON_SEL );
		}
	}

	return TRUE;
}

// ********************************************************************
// ������static void DoMethodPaint( HWND hWnd )
// ������
//	IN hWnd- ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// ���������������ڻ滭����
// ����: 
// ********************************************************************

static BOOL DoMethodPaint( HWND hwndDlg )
{
	PAINTSTRUCT ps;
	RECT rc;
	HDC hdc = BeginPaint( hwndDlg, &ps );
	
	GetClientRect( hwndDlg, &rc );
	FillSolidRect( hdc, &rc, CLR_BACKGROUND );
	ShowInfo( hwndDlg, hdc, GetWindowLong( hwndDlg, GWL_USERDATA ) );
	EndPaint( hwndDlg, &ps );
	return TRUE;
}


// ********************************************************************
// ������static LRESULT DoMethodInitDialog( HWND hWnd )
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE
// ����������
//	��ʼ��dialog
// ����: 
// ********************************************************************

static LRESULT DoMethodInitDialog( HWND hWnd )
{
	HWND hChildWnd = GetDlgItem( hWnd, IDC_BUTTON_ALL );
	SetWindowLong( hWnd, GWL_USERDATA, IDC_BUTTON_ALL );
	SendMessage( hChildWnd, BM_SETCHECK, BST_CHECKED, 0 );
	SetFocus( hWnd );
	return FALSE;
}



/******************************************************************
������LRESULT CALLBACK BackupMethodProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
������
	IN  hwndDlg-���ھ����uMsg-��Ϣ��wParam-������lParam-����
����ֵ��
	TRUE/FALSE 
����������
	ͨ������ĶԻ�������
*******************************************************************/
static LRESULT CALLBACK BackupMethodProc(
						HWND hwndDlg,
						UINT uMsg,
						WPARAM wParam,
						LPARAM lParam )
{

	switch (uMsg)
	{
		//case WM_COMMAND:
			//return DoMethodCommand( hwndDlg, wParam, lParam );
		case WM_INITDIALOG:					//��ʼ���Ի���
			return DoMethodInitDialog( hwndDlg );
		case WM_PAINT:
			return DoMethodPaint( hwndDlg );
		case WM_KEYDOWN:
			return DoMethodKeyDown( hwndDlg, wParam, lParam );
	}

	return (FALSE);
}




// ********************************************************************
// ������static LRESULT DoProgressInitDialog( HWND hWnd )
// ������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE
// ����������
//	��ʼ��dialog
// ����: 
// ********************************************************************

static LRESULT DoProgressInitDialog( HWND hWnd )
{
	HWND hChildWnd = GetDlgItem( hWnd, IDC_PRO_FILE );

	SendMessage( hChildWnd, PBM_SETRANGE, 0, MAKELPARAM(0, 100) );
	SendMessage( hChildWnd, PBM_SETPOS, 40, 0 );	
	hChildWnd = GetDlgItem( hWnd, IDC_PRO_TOTAL );
	SendMessage( hChildWnd, PBM_SETRANGE, 0, MAKELPARAM(0, 100) );
	SendMessage( hChildWnd, PBM_SETPOS, 80, 0 );
	SetFocus( hWnd );


	//hChildWnd = CreateWindow( classPROGRESS, NULL, WS_CHILD | WS_VISIBLE | PBS_SMOOTH | PBS_VERTICAL, 600, 20, 12, 400, hWnd, 0, GetWindowLong( hWnd, GWL_HINSTANCE ), 0 );
	//SendMessage( hChildWnd, PBM_SETRANGE, 0, MAKELPARAM(0, 100) );
	//SendMessage( hChildWnd, PBM_SETPOS, 80, 0 );

	return FALSE;
}

// ********************************************************************
// ������static void DoProgressPaint( HWND hWnd )
// ������
//	IN hWnd- ���ھ��
// ����ֵ��
//	��
// ���������������ڻ滭����
// ����: 
// ********************************************************************

#define PROGRESS_BKCOLOR RGB( 56, 56, 56 )
static int DoProgressPaint( HWND hwndDlg )
{
	PAINTSTRUCT ps;
	RECT rc;
	int x, y;
	char * lpszText;
	
	HDC hdc = BeginPaint( hwndDlg, &ps );
	HBRUSH hBrush = CreateSolidBrush( PROGRESS_BKCOLOR );
				
				
	GetClientRect( hwndDlg, &rc );
	//������λͼ
	//DrawBitMap( hdc, hbmpBackground, &rc, 0, 0, SRCCOPY ); 				
				
	SelectObject( hdc, hBrush );
	//��Բ�Ǳ���
	RoundRect( hdc, 10, 10, 10 + 617 , 10 + 348, 18, 18 );// FillSolidRect( hdc, &rc, CLR_BACKGROUND );				
				
	rc.left = 205; rc.top = 128 - 50;
	rc.right = rc.left + 70; rc.bottom = rc.top + 70;
	//��icon
	if( uiCurrentRunID == ID_RUN_SD )
		DrawTransparentBitMap( hdc, hbmpCopySD, &rc, 0, 0, CLR_TRANSPARENT );
	else if( uiCurrentRunID == ID_RUN_CF )
		DrawTransparentBitMap( hdc, hbmpCopyCF, &rc, 0, 0, CLR_TRANSPARENT );
	else if( uiCurrentRunID == ID_RUN_CFSD )
		DrawTransparentBitMap( hdc, hbmpCopyCFSD, &rc, 0, 0, CLR_TRANSPARENT );
	else if( uiCurrentRunID == ID_RUN_HDD )
		DrawTransparentBitMap( hdc, hbmpCopyHDD, &rc, 0, 0, CLR_TRANSPARENT );
				
	rc.left = 205 + 160; rc.top = 128 - 50;
	rc.right = rc.left + 70; rc.bottom = rc.top + 70;
	DrawTransparentBitMap( hdc, hbmpCopyHDD, &rc, 0, 0, CLR_TRANSPARENT );				
		
	DeleteObject( hBrush );				
				
				
	SetBkColor( hdc, PROGRESS_BKCOLOR );
	SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
	//��һ���ı��Ŀ�ʼλ��
	y = dlgIDD_BackupProgress.item[0].y - 12 - 24;
	x = 50;
	lpszText = "Current file";
	TextOut( hdc, x, y, lpszText, strlen( lpszText ) );
				
	//�ڶ����ı��Ŀ�ʼλ��
	y = dlgIDD_BackupProgress.item[1].y - 12 - 24;
	x = 50;
	lpszText = "Progress";
	TextOut( hdc, x, y, lpszText, strlen( lpszText ) );
				
	//�Ҷ���
	SetTextAlign( hdc, TA_RIGHT );
	//��һ���ٷֱ�
	y = dlgIDD_BackupProgress.item[0].y - 12 - 24;
	x = dlgIDD_BackupProgress.item[0].x + dlgIDD_BackupProgress.item[0].cx;
	lpszText = "50 %";
	TextOut( hdc, x, y, lpszText, strlen( lpszText ) );
				
	//�ڶ����ٷֱ�
	y = dlgIDD_BackupProgress.item[1].y - 12 - 24;
	x = dlgIDD_BackupProgress.item[1].x + dlgIDD_BackupProgress.item[1].cx;
	lpszText = "0012 / 0120";
	TextOut( hdc, x, y, lpszText, strlen( lpszText ) );
				
	EndPaint( hwndDlg, &ps ); // ��������
				
	return TRUE;
}


// ********************************************************************
// ������static void DoEraseBkgnd( HWND hWnd, HDC hdc )
// ������
//	IN hWnd -- ���ھ��
//	IN hdc -- �滭���
// ����ֵ��
//	��
// ���������������ڱ���ˢ�¹���
// ����: 
// ********************************************************************

static BOOL DoEraseBkgnd( HWND hWnd, HDC hdc )
{
	RECT rc;
	
	GetClientRect( hWnd, &rc );
	//������λͼ
	DrawBitMap( hdc, hbmpBackground, &rc, 0, 0, SRCCOPY ); 				
	return TRUE;
}


/******************************************************************
������LRESULT CALLBACK BackupProgressProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
������
	IN  hwndDlg-���ھ����uMsg-��Ϣ��wParam-������lParam-����
����ֵ��
	TRUE/FALSE 
����������
	ͨ������ĶԻ�������
*******************************************************************/
static LRESULT CALLBACK BackupProgressProc(
						HWND hwndDlg,
						UINT uMsg,
						WPARAM wParam,
						LPARAM lParam )
{

	switch (uMsg)
	{
		case WM_COMMAND:
			return FALSE;//return DoProgressCommand( hwndDlg, wParam, lParam );
		case WM_INITDIALOG:					//��ʼ���Ի���
			return DoProgressInitDialog( hwndDlg );
		case WM_PAINT:
			return DoProgressPaint( hwndDlg );
		case WM_ERASEBKGND:
			return DoEraseBkgnd( hwndDlg, (HDC)wParam );
		case WM_KEYDOWN:
			if( wParam == VK_CANCEL )
			{
				EndDialog( hwndDlg, 0 );
				return TRUE;
			}
	}

	return (FALSE);
}

//���� WM_RUNPROC ��Ϣ�� 
static int DoRunProc( HWND hWnd, UINT idRun )
{
	HANDLE hInst;
	int retv;

	ShowWindow( hWnd, SW_SHOW );
	uiCurrentRunID = idRun;
	hInst = (HANDLE)GetWindowLong( hWnd, GWL_HINSTANCE );

	retv = DialogBoxIndirectParamEx(
		hInst, 
		(LPDLG_TEMPLATE_EX)&dlgIDD_BackupMethod, 
		NULL, 
		(DLGPROC)BackupMethodProc, 
		(LPARAM)NULL);
	if( retv == IDC_BUTTON_ALL ||
		retv == IDC_BUTTON_SEL )
	{
		DialogBoxIndirectParamEx(
			hInst, 
			(LPDLG_TEMPLATE_EX)&dlgIDD_BackupProgress, 
			NULL, 
			(DLGPROC)BackupProgressProc, 
			(LPARAM)NULL);
	}
    ShowWindow( hWnd, SW_HIDE );
	ActiveDesktop();
	return TRUE;
}


/******************************************************************
������LRESULT WINAPI BackupWndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
������
	IN   hwndDlg-���ھ��, uMsg-��Ϣ, wParam-����, lParam-����
���أ�	
	������ֵ
����������
	���ش��ڴ�����������������ڣ�����ȫ����Ϣ,�ַ�ȫ����Ϣ��ָ���Ĵ���
*******************************************************************/
LRESULT CALLBACK
BackupWndProc(
			  HWND hWnd, 
			  UINT uiMsg, 
			  WPARAM wParam, 
			  LPARAM lParam )
{
	switch( uiMsg )
	{
	case WM_RUNPROC:
		return DoRunProc( hWnd, wParam );
	default:
		return DefWindowProc( hWnd, uiMsg, wParam, lParam );
	}
	return 0;

}
