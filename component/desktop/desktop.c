/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ϵͳ���棬�û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-03-18
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
#include <ekeybrd.h>
#include "eabout.h"
#include "resource.h"
#include "emmsys.h"
#include <keybdsrv.h>
#include <softkey.h>

#include "desktop.h"
#include <eoemkey.h>
#include <jpegapi.h>
/***************  ȫ���� ���壬 ���� *****************/

char   classDESKTOP[20] ;
#define DESKTOPCLASSNAME  "_DESKTOP_CLASS_"
static char classKEYBOARDWINDOW[] = "_KEYBOARD_WINDOW_CLASS_";
static HINSTANCE hInst;

extern BOOL InitStateBar( HINSTANCE hInstance );
static int	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem );
static void SetFocusButton( HWND hWnd, int idx );
static void RunFocusFunction( void );

#define FOCUS_DESKTOP       0
#define FOCUS_LIST          1


static HBITMAP g_hDesktopBitmap = NULL;
static HWND g_hwndDesktop = NULL;
static UINT g_FocusState = FOCUS_DESKTOP;

static HBITMAP	hbmpNULL = 0;
static HBITMAP	hHomeBitMap;
static HBITMAP	hbmpHDD;
static HBITMAP	hbmpSDCard;
static HBITMAP	hbmpCFCard;
static HBITMAP  hbmpTextBarNormal;
static HBITMAP  hbmpTextBarSelect;

static HBITMAP	hbmpBackup[5];
static HBITMAP  hbmpBackupState[3];  //

static HBITMAP	hbmpPlayback[5];
static HBITMAP  hbmpPlaybackState[3];  //

static HBITMAP	hbmpPlaysound[5];
static HBITMAP  hbmpPlaysoundState[3];  //

static HBITMAP	hbmpPlayMovie[5];
static HBITMAP  hbmpPlayMovieState[3];  //

static HBITMAP	hbmpSetup[2];
static HBITMAP  hbmpSetupState[3];  //




enum{
	    ID_LIST_BACKUP      =0,
		ID_LIST_PLAYBACK,
		ID_LIST_PLAYSOUND,
		ID_LIST_PLAYMOVIE,
		ID_LIST_SETUP,
		ID_LIST_MAX,
		ID_EXIT
};

static PTRLIST ptrList[ID_LIST_MAX];


#define ID_STATE_BAR        (ID_LIST_MAX+0)
//#define ID_LISTBOX          101


typedef struct DesktopFunStruct{
	RECT rectFunc;
	RECT rectIcon;
	LPCTSTR lpFuncName;	
	LPCTSTR lpTitle;	
	UINT idFunc;
	UINT idSubItem;
	HBITMAP *lpbmpIcon;
	HBITMAP *lpbmpState;
}DESKTOPFUNC , *LPDESKTOPFUNC;

typedef struct DESKTOP_ITEM_DATA{
	HBITMAP hbmpIcon;
	UINT cTitleLength;
	char szTitle[1];
}DESKTOP_ITEM_DATA;

#define DESKTOP_CONTROL_NUM 8

#define STATE_NORMAL   0
#define STATE_FOCUS    1
#define STATE_SET      2

typedef struct _DESK_INFO{
	UINT uCurrentSelectIndex;
	UINT uCurrentSelectState;
	DESKTOPFUNC deskFun[DESKTOP_CONTROL_NUM];
}DESK_INFO, FAR * LPDESK_INFO;

#define COLOR_TASKBAR  RGB(205,205,205)
#define COLOR_DESKTOPBK  RGB(240,240,240)

//#define ID_LOGO     0x2038
//#define ID_TIME     0x2043
//#define ID_SHOW     0x2044
//#define ID_KEYBD    0x2045
//#define ID_MYCOMPUTER  0x2046
//#define ID_START	0x100


#ifdef INLINE_PROGRAM
#define ICON_WIDTH 32
#define ICON_HEIGHT 32
#define FUN_WIDTH 101
#define FUN_HEIGHT 24

#define ICON_COL_WIDTH 32
#define ICON_ROW_HEIGHT 40

#define ICON_OFFSET_X   500
#define ICON_OFFSET_Y   0


static DESK_INFO diDesktopInfo ={
	0, STATE_FOCUS,
	{
		{   
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*0, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*0+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*0, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*0+ICON_HEIGHT },
		      "demo_alpha", "alpha", 0, 0, hbmpBackup, hbmpBackupState
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*1, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*1+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*1, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*1+ICON_HEIGHT },
			"demo_normal", "normal", 0, 0, hbmpPlayback, hbmpPlaybackState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*2, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*2+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*2, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*2+ICON_HEIGHT },
			"demo_rgnwin","rgnwin", 0, 0, hbmpPlaysound, hbmpPlaysoundState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*3, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*3+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*3, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*3+ICON_HEIGHT },
			"demo_font1", "font1", 0, 0, hbmpPlayMovie, hbmpPlayMovieState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*4, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*4+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*4, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*4+ICON_HEIGHT },
			"demo_Gears", "OpenGL-3D", 0, 0, hbmpSetup, hbmpSetupState 
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*5, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*5+FUN_HEIGHT },
			{35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*5, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*5+ICON_HEIGHT },
			"demo_Layered", "Layered", 0, 0, hbmpSetup, hbmpSetupState 
		},


		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*6, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*6+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*6, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*6+ICON_HEIGHT },
			"demo_gradfill", "�������", 0, 0, hbmpSetup, hbmpSetupState
		},
		{ 
			{ 12+ICON_COL_WIDTH*0, 64+ICON_ROW_HEIGHT*7, 12+ICON_COL_WIDTH*0+FUN_WIDTH, 64+ICON_ROW_HEIGHT*7+FUN_HEIGHT },
			{ 35+ICON_COL_WIDTH*0, 70+ICON_ROW_HEIGHT*7, 35+ICON_COL_WIDTH*0+ICON_WIDTH, 70+ICON_ROW_HEIGHT*7+ICON_HEIGHT },
			"", "exit", ID_EXIT, 0, hbmpSetup, hbmpSetupState
		}
	},
	
}; 


#else
static DESKTOPFUNC DeskTopFuncList[] ={
	{{  8,  2, 24, 18},"\\kingmos\\Func Logo.exe",ID_LOGO},
	{{  0, 20, 30, 50},"\\kingmos\\contactbook.exe",ID_FUNC1},
	{{  0, 58, 30, 88},"\\kingmos\\PlanDay.exe",ID_FUNC2},
	{{  0, 96, 30,126},"\\kingmos\\Notebook.exe",ID_FUNC3},
	{{  0,134, 30,164},"\\kingmos\\Mobilephone.exe",ID_FUNC4},
	{{  0,172, 30,202},"\\kingmos\\FlyBat.exe",ID_FUNC5},
	{{  0,210, 30,240},"\\kingmos\\IExplore.exe",ID_FUNC6},
	{{  7,248, 23,264},"\\kingmos\\EasyBox.exe",ID_ICON1},
	{{  7,265, 23,281},"\\kingmos\\Alarm.exe",ID_ICON2},
	{{  7,282, 23,298},"\\kingmos\\Setting.exe",ID_ICON3},
	{{  7,299, 23,315},"\\kingmos\\rasdial.exe",ID_ICON4},
	{{203,302,219,318},"\\kingmos\\Func12.exe",ID_SHOW},
	{{ 30,303,160,319},"\\kingmos\\Setting.exe",ID_TIME},
	{{221,302,237,318},"\\kingmos\\keyboard.exe",ID_KEYBD},
	{{ 49, 20, 81, 52},"\\kingmos\\manage.exe",ID_MYCOMPUTER},
};
#endif

#define BANK_POSITION -1

static UINT g_indexButtonDown = BANK_POSITION;

#define ID_TIMESHOW 1

static HWND g_hKey= NULL;


/******************************************************/
// ����������
/******************************************************/
static ATOM RegisterDesktopClass( HINSTANCE hInstance );
static LRESULT CALLBACK DesktopWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnDesktopPaint( HWND hWnd );
static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc );
static void GetWallPaperPic(LPTSTR lpDeskTopName);
static void DoDesktopLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoDesktopLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static UINT GetButtonID(POINTS ptCursor);
static int DoDesktopCreate(HWND hWnd);
static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void ShowTime(HWND hWnd,HDC hdc);
static void DoDesktopClose(HWND hWnd);
static void GetIDRect(UINT id,LPRECT lpRect);
static void DoShowKeyboard(void);
static LRESULT DoWindowPosChanging(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoShowApplication(HWND hWnd);

static void DrawWallPaper(HWND hWnd,HDC hdc);
static void DrawDesktopControl(HWND hWnd,HDC hdc);
static void DrawTaskBar(HWND hWnd,HDC hdc);
static HWND GetDesktop(void);

static void DrawBitMap(HDC hdc,HBITMAP hBitmap, LPRECT lprect,UINT xOffset,UINT yOffset,DWORD dwRop );

static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam );

static HWND CreateSystemKeyboard(HWND hParent);

static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void OnAboutPaint( HWND hWnd );


static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance );
static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoKeyboardWndCreate(HWND hWnd);

static void SplashLogo( void )
{
	
//#ifdef LINUX_KERNEL
	HBITMAP hbmp = LoadImage( NULL, "./kingmos/desktop/logo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
//#else
//	HBITMAP hbmp = LoadImage( NULL, ".\\kingmos\\bg_splash.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
//#endif
	HDC hdc = GetDC( NULL );
	HDC hMemDC = CreateCompatibleDC( hdc );
	
	SelectObject( hMemDC, hbmp );

	BitBlt( hdc, 0, 0, 640, 480, hMemDC, 0, 0, SRCCOPY );

	DeleteDC( hMemDC );
	ReleaseDC( NULL, hdc );

	DeleteObject( hbmp );
	Sleep( 3000 );
}

static VOID InitDesktopData( HINSTANCE hInst )
{
}

//������Ϣ����, ��Ҫ��� LEFT, RIGHT , UP, DOWN
static BOOL FilterMessage( LPMSG lpMsg )
{
	if( g_FocusState == FOCUS_LIST )
	{
		if( lpMsg->message == WM_KEYDOWN )
		{
			if( lpMsg->wParam == VK_LEFT )
			{
				if( g_FocusState == FOCUS_LIST )
				{
					SendMessage( GetDlgItem( g_hwndDesktop, diDesktopInfo.uCurrentSelectIndex ), LB_SETCURSEL, -1, 0 );

					g_FocusState = FOCUS_DESKTOP;
					//SetFocus( g_hwndDesktop );					
				}
				return TRUE;
			}
		}
	}
	return FALSE;
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
int WINAPI WinMain_Desktop(HINSTANCE hInstance,
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

		RETAILMSG( 1, ( "Start Desktop ...\r\n" ) );

		RETAILMSG( 1, ( "TouchCalibrate(): %x \r\n", TouchCalibrate) );

		//SplashLogo();
		//InitStateBar( hInstance );
		InitDesktopData( hInstance );
		 
//		TouchCalibrate();  // ���л���У��

		RegisterDesktopClass(hInstance); // ע�����������
		RegisterKeyboardWndClass(hInstance);
		hInst = hInstance;
		// �������洰��
		g_hwndDesktop = CreateWindowEx( WS_EX_NOFOCUS, classDESKTOP, "����", WS_VISIBLE|WS_CLIPCHILDREN,
			0, 0,
			GetSystemMetrics( SM_CXSCREEN ),
			GetSystemMetrics( SM_CYSCREEN ),
			0,0,hInstance,0 );		


		//LoadApplication( "demo_gps", 0 );
		//LoadApplication( "demo_gradfill", 0 );
#if 0
		LoadApplication( "demo_alpha", 0 );
		Sleep( 1000 );
		LoadApplication( "demo_font1", 0 );
		Sleep( 3000 );
//		LoadApplication( "demo_Gears", 0 );
//		Sleep( 50 );
		LoadApplication( "demo_Layered", 0 );
		Sleep(1000);
#endif 	
//		sndPlaySound("\\kingmos\\kingmos.wav",SND_ASYNC); // ���ſ�������
		{
			//extern void LoadAllDemoAp(HINSTANCE hInstance);
			//LoadAllDemoAp(hInstance);
		}

		while (GetMessage(&msg, NULL, 0, 0)) 
		{	// ��Ϣ����
			//
			if( FilterMessage( &msg ) == 0 )
			{	//û�д���
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		// ��������Ѿ��˳������˳�ϵͳ
		ExitSystem(EXS_SHUTDOWN,0); // �˳�ϵͳ
		return 0;
}

static int DoKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
	BOOL bUpdate = FALSE;
	HWND hwndChild;
	UINT oldSelect = diDesktopInfo.uCurrentSelectIndex;
	switch( wParam )
	{
	case VK_DOWN:
		diDesktopInfo.uCurrentSelectIndex = (diDesktopInfo.uCurrentSelectIndex + 1) % DESKTOP_CONTROL_NUM;
		diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
		bUpdate = TRUE;
		break;
	case VK_UP:
		if( diDesktopInfo.uCurrentSelectIndex )
			diDesktopInfo.uCurrentSelectIndex = (diDesktopInfo.uCurrentSelectIndex - 1) % DESKTOP_CONTROL_NUM;
		else
			diDesktopInfo.uCurrentSelectIndex = DESKTOP_CONTROL_NUM - 1;
		diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
		bUpdate = TRUE;
		break;
	//case VK_RIGHT:
	case VK_RETURN:
		diDesktopInfo.uCurrentSelectState = STATE_SET;
		//g_FocusState = FOCUS_LIST;
		//hwndChild = GetDlgItem( hWnd, diDesktopInfo.uCurrentSelectIndex );
		//SetFocus( hwndChild );
		//SendMessage( hwndChild, LB_SETCURSEL, 0, 0 );
		
		//bUpdate = TRUE;
		RunFocusFunction();
		break;
	}

	SetFocusButton( hWnd, diDesktopInfo.uCurrentSelectIndex );
	/*
	if( oldSelect != diDesktopInfo.uCurrentSelectIndex )
	{
		ShowWindow( GetDlgItem( hWnd, oldSelect ), SW_HIDE );
		ShowWindow( GetDlgItem( hWnd, diDesktopInfo.uCurrentSelectIndex ), SW_SHOW );
	}

	if( bUpdate )
	{
		HDC hdc = GetDC( hWnd );
		//InvalidateRect( hWnd, NULL, FALSE );
		DrawDesktopControl(hWnd,hdc); // �����������
		ReleaseDC( hWnd, hdc );
	}
	*/
	return 0;
}

//���µõ�����
static int DoSetFocus( HWND hWnd )
{
	HDC hdc = GetDC( hWnd );

	diDesktopInfo.uCurrentSelectState = STATE_FOCUS;	
	//InvalidateRect( hWnd, NULL, FALSE );
	DrawDesktopControl(hWnd,hdc); // �����������
	ReleaseDC( hWnd, hdc );
	return 0;
}

// ********************************************************************
// ������static ATOM RegisterDesktopClass( HINSTANCE hInstance );
// ������
//	IN hInstance - ����ϵͳ��ʵ�����
// ����ֵ��
// 	��
// ����������ע��ϵͳ������
// ����: 
// ********************************************************************
static ATOM RegisterDesktopClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	strcpy( classDESKTOP,  DESKTOPCLASSNAME) ; // ��������

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)DesktopWndProc;  // ������̺���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);//CreateSolidBrush(COLOR_DESKTOPBK);//GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classDESKTOP;
	return RegisterClass(&wcex); // ��ϵͳע��������
}

// ********************************************************************
// ������static LRESULT CALLBACK DesktopWndProc(HWND , UINT , WPARAM , LPARAM )
// ������
//	IN hWnd- ����ϵͳ�Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��
// ����������ϵͳ���洰�ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK DesktopWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // �滭��Ϣ
			OnDesktopPaint( hWnd );
			return 0;
		case WM_KEYDOWN:
			return DoKeyDown( hWnd, wParam, lParam );
		case WM_ERASEBKGND:  // ���Ʊ���
			OnDesktopEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_DRAWITEM:
			return DoDrawItem( hWnd, wParam, lParam );
		case WM_LBUTTONDOWN: // �����ť���£���TOUCH�ʰ���
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_MOUSEMOVE: // �����ť���£���TOUCH�ʰ���
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_COMMAND:
			if( wParam == 1 && lParam == 0 )
			{	//��Ϣ������menu item, or list item, VK_RETURN
				HWND hFocusWnd = GetFocus();
				if( GetParent( hFocusWnd ) == hWnd )
				{  //���ҵ��Ӵ���
					UINT id = GetWindowLong( hFocusWnd, GWL_ID );
					if( id < ID_LIST_MAX )
					{  // yes, listbox
						
						DESKTOP_ITEM_DATA * lpItem;
						UINT indexSel = SendMessage( hFocusWnd, LB_GETCURSEL, 0, 0 );
						LPPTRLIST lpPtrList = GetWindowLong( hFocusWnd, GWL_USERDATA );
						lpItem = (DESKTOP_ITEM_DATA*)PtrListAt( lpPtrList, indexSel );
						if( diDesktopInfo.deskFun[id].idFunc == ID_EXIT )
						{
							exit(0);
						}
						else
							LoadApplication( diDesktopInfo.deskFun[id].lpFuncName, lpItem->szTitle );
					}
				}
			}
			return 0;
		case WM_LBUTTONUP:// �����ť���𣬻�TOUCH���뿪
			DoDesktopLButtonUp(hWnd,wParam,lParam);
			return 0;
		case WM_TIMER: // ��ʱ��
			//SetFocus( hWnd );
//			DoDesktopTimer(hWnd,wParam,lParam);
			return 0;
		case WM_WINDOWPOSCHANGING: // ����λ�øı�
			return DoWindowPosChanging(hWnd,wParam,lParam);
		case WM_SETFOCUS:
			return DoSetFocus( hWnd );
//            return 0;
		case WM_CLOSE:  // �ر���Ϣ
			DoDesktopClose(hWnd);
			return 0;
		case WM_ACTIVATEAPP:
			SetForegroundWindow( hWnd );
			/*
			if( g_FocusState == FOCUS_LIST )
			{
		        SetFocus( GetDlgItem( hWnd, diDesktopInfo.uCurrentSelectIndex ) );
			}
			else
			{  // desktop
				SetFocus( hWnd );
			}
			*/
			return 0;
		case WM_DESTROY: // �ƻ���Ϣ
			PostQuitMessage(0);
			return 0;
		case WM_CREATE: // ������Ϣ
			return DoDesktopCreate(hWnd);
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // ����Ĭ����Ϣ
   }
   return FALSE;
}

//����������, ��ΪҪ�ı�clip rgn , ���Ա���������
static void DrawDiskSpace( HWND hWnd, HDC hdc )
{
	const RECT rc = { 404, 440, 626, 470 };
	UINT uDiskUsed = 11;
	UINT uDiskSpace = 30;
	char szBuf[128];
	HRGN hrgn;

	sprintf( szBuf, "%d GB / %d GB", uDiskUsed, uDiskSpace );

	
	hrgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
	SelectObject( hdc, hrgn );
	SetBkMode( hdc, TRANSPARENT );
	SetTextAlign( hdc, TA_RIGHT );
	DrawWallPaper( hWnd, hdc );
	ExtTextOut( hdc, 626, 446, 0, &rc, szBuf, strlen(szBuf), NULL );	

	SetTextAlign( hdc, TA_LEFT );

	DeleteObject( hrgn );	

}

// ********************************************************************
// ������static void OnDesktopPaint( HWND hWnd )
// ������
//	IN hWnd- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// ��������������ϵͳ���洰�ڻ滭����
// ����: 
// ********************************************************************
static void OnDesktopPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;

		hdc = BeginPaint( hWnd, &ps );  // ��ʼ����
		
		DrawDesktopControl(hWnd,hdc); // �����������
		// ����������
		//DrawDiskSpace( hWnd, hdc );
//		ShowTime(hWnd,hdc);
		EndPaint( hWnd, &ps ); // ��������
}

// ********************************************************************
// ������static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
// ������
//	IN hWnd -- ����ϵͳ�Ĵ��ھ��
//	IN hdc -- ����ϵͳ�Ļ滭���
// ����ֵ��
//	��
// ��������������ϵͳ���洰�ڱ���ˢ�¹���
// ����: 
// ********************************************************************

static void OnDesktopEraseBkgnd( HWND hWnd, HDC hdc )
{
//	RECT				rt;
	
//	HBRUSH hBrush;


//	char lpDeskTopName[MAX_PATH];

	
	DrawWallPaper( hWnd, hdc );
	
	//rt.left += 30; // ������Ӧ�ó�����
	//rt.bottom -= 20; // ������ϵͳ״̬��
	//hBrush = CreateSolidBrush(COLOR_DESKTOPBK); // ��������ˢ
	//FillRect( hdc, &rt, hBrush ) ; // ��䱱��
	//DeleteObject(hBrush); // ɾ������ˢ
}

// ********************************************************************
// ������static void GetWallPaperPic(LPTSTR lpDeskTopName)
// ������
//	IN/OUT lpDeskTopName -- ������汳��ͼ���λͼ�ļ����Ļ���
// ����ֵ��
//	��
// �����������õ����汳��ͼ���λͼ�ļ���
// ����: 
// ********************************************************************
static void GetWallPaperPic(LPTSTR lpDeskTopName)
{
//#ifdef LINUX_KERNEL
	strcpy(lpDeskTopName,"./kingmos/desktop/desktop_bk.bmp"); // ����ָ�������汳��λͼ
//#else
//	strcpy(lpDeskTopName,".\\kingmos\\bg_home.bmp"); // ����ָ�������汳��λͼ
//#endif
}

static void SetFocusButton( HWND hWnd, int idx )
{
	HDC hdc;
	diDesktopInfo.uCurrentSelectIndex = idx;
	diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
	
	hdc = GetDC( hWnd );
	DrawDesktopControl(hWnd,hdc); // �����������
	ReleaseDC( hWnd, hdc );
}

static void RunFocusFunction( void )
{
	if( diDesktopInfo.deskFun[diDesktopInfo.uCurrentSelectIndex].idFunc == ID_EXIT )
	{
		exit(0);
	}
	else
		LoadApplication( diDesktopInfo.deskFun[diDesktopInfo.uCurrentSelectIndex].lpFuncName, NULL );
}


// ********************************************************************
// ������static void DoDesktopLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
//    IN wParam  -- ��Ϣ����
//    IN lParam  -- ��Ϣ����
// ����ֵ��
//	��
// ������������������������������Ϣ
// ����: 
// ********************************************************************
static void DoDesktopLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINTS ptCursor;
	UINT indexButton;
	
	ptCursor = MAKEPOINTS(lParam);  // �õ���갴�µ�λ��
	indexButton = GetButtonID(ptCursor); // �õ�����λ�õİ�ťID
	
	//		g_indexButtonDown = indexButton; // ���ð��°�ť
	if( indexButton != -1 )
	{
		if( diDesktopInfo.uCurrentSelectIndex != indexButton )
		{
			SetFocusButton( hWnd, indexButton );
//			HDC hdc;
//			diDesktopInfo.uCurrentSelectIndex = indexButton;
//			diDesktopInfo.uCurrentSelectState = STATE_FOCUS;
//			
//			hdc = GetDC( hWnd );
//			DrawDesktopControl(hWnd,hdc); // �����������
//			ReleaseDC( hWnd, hdc );
		}
		else
		{
			RunFocusFunction();
			/*
			if( diDesktopInfo.deskFun[indexButton].idFunc == ID_EXIT )
			{
				exit(0);
			}
			else
				LoadApplication( diDesktopInfo.deskFun[indexButton].lpFuncName, NULL );
				*/
		}
	}
	
	g_FocusState = FOCUS_DESKTOP;
	//SetFocus( hWnd );		
}

// ********************************************************************
// ������static void DoDesktopLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
//    IN wParam  -- ��Ϣ����
//    IN lParam  -- ��Ϣ����
// ����ֵ��
//	��
// ������������������������������Ϣ
// ����: 
// ********************************************************************
static void DoDesktopLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
}

// ********************************************************************
// ������static UINT GetButtonID(POINTS ptCursor)
// ������
//	IN	ptCursor - Ҫ�õ���ָ��λ�õ�ID�ŵ�����λ��
// ����ֵ��
//	����ָ��λ�õİ�ťID
// �����������õ�ָ��λ�õİ�ťID
// ����: 
// ********************************************************************
static UINT GetButtonID(POINTS ptCursor)
{
	int i;
	POINT pt = { ptCursor.x, ptCursor.y };
	for( i = 0; i < DESKTOP_CONTROL_NUM; i++ )
	{
		RECT rc = diDesktopInfo.deskFun[i].rectFunc;
		OffsetRect( &rc, ICON_OFFSET_X, ICON_OFFSET_Y );
		if( PtInRect( &rc, pt ) )
			return i;
	}
	return -1;
}

//��ʼ��������LIST data
//static BOOL InitListData( HWND hParent, HWND hListWnd, int idList )
static BOOL InitListData( int idList )
{
	int i;
	const char ** lppszText;
	const HBITMAP ** lpphbmpIcon;
	DESKTOP_ITEM_DATA * lpItemData;
	int num = 0;
	LPPTRLIST lpPtrList = NULL;
	if( idList == ID_LIST_BACKUP )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_BACKUP];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_PLAYBACK )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_PLAYBACK];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_PLAYSOUND )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_PLAYSOUND];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_PLAYMOVIE )
	{
		const char * lpszStockText[] = { "HDD", "CF Card", "SD Card", "", "", "" };
		const HBITMAP * lphStockIcon[] = { &hbmpHDD, &hbmpCFCard, &hbmpSDCard, &hbmpNULL, &hbmpNULL, &hbmpNULL };
		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		lpPtrList = &ptrList[ID_LIST_PLAYMOVIE];//_Backup;
		num = sizeof(lpszStockText) / sizeof( char* );
	}
	else if( idList == ID_LIST_SETUP )
	{
		const char * lpszStockText[] = { "Display", "System", "Backup behavior", "Playback behavior", "Languages", "Date & Time" };
		const HBITMAP * lphStockIcon[] = { &hbmpNULL, &hbmpNULL, &hbmpNULL, &hbmpNULL, &hbmpNULL, &hbmpNULL };

		lppszText = lpszStockText;
		lpphbmpIcon = lphStockIcon;
		num = sizeof(lpszStockText) / sizeof( char* );
		//lpPtrList = &ptrList_Setup;
		lpPtrList = &ptrList[ID_LIST_SETUP];
	}


	if( lpPtrList )
	{
		PtrListCreate( lpPtrList, 8, 16 );
		
		for( i = 0; i < num; i++ )
		{
			UINT len;
			len = strlen( lppszText[i] );
			lpItemData = malloc( sizeof( DESKTOP_ITEM_DATA ) +  len );
			if( lpItemData )
			{
				lpItemData->cTitleLength = len;
				lpItemData->hbmpIcon = *lpphbmpIcon[i];
				strcpy( lpItemData->szTitle, lppszText[i] );
				PtrListInsert( lpPtrList, lpItemData );
			}
		}
	}
	return TRUE;
}

static BOOL InitAllListData( VOID )
{
	int i = 0;
	for( i = 0; i < ID_LIST_MAX; i++ )
		InitListData( i );
	return TRUE;
}

// ********************************************************************
// ������static void DoDesktopCreate(HWND hWnd)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// �����������������洴����Ϣ
// ����: 
// ********************************************************************
static int DoCreateListWindow(HWND hParent, UINT id, HINSTANCE hInst)
{
	return 0;

}

// ********************************************************************
// ������static void DoDesktopCreate(HWND hWnd)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// �����������������洴����Ϣ
// ����: 
// ********************************************************************
static int DoDesktopCreate(HWND hWnd)
{
	//SetTimer( hWnd, 0, 2000, NULL );
	//SetFocus( hWnd );
	return 0;
}

// ********************************************************************
// ������static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
//    IN wParam  -- ��Ϣ����
//    IN lParam  -- ��Ϣ����
// ����ֵ��
//	��
// ������������������Timer��Ϣ
// ����: 
// ********************************************************************
static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HDC hdc;
		hdc = GetDC(hWnd); // �õ�HDC
		ShowTime(hWnd ,hdc); // ��ʾ��ǰʱ��
		ReleaseDC(hWnd,hdc); // �ͷ�HDC
}

// ********************************************************************
// ������static void ShowTime(HWND hWnd,LPTSTR lpTime)
// ������
//	IN  hWnd -- ����ϵͳ�Ĵ��ھ��
//	IN  hdc -- ����ϵͳ�Ļ滭���
// ����ֵ��
//	��
// ��������������������ʾ��ǰʱ��
// ����: 
// ********************************************************************
static void ShowTime(HWND hWnd,HDC hdc)
{
}

// ********************************************************************
// ������static void DoDesktopClose(HWND hWnd)
// ������
//	IN	hWnd - ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// ������������������ر���Ϣ
// ����: 
// ********************************************************************
static void DoDesktopClose(HWND hWnd)
{
	BOOL bExistOtherWindow;
	// Ҫ��ȴ����еĴ����˳�
	while(1)
	{
		bExistOtherWindow = FALSE;
		EnumWindows(EnumExistWindowsProc,(LPARAM)&bExistOtherWindow); // ö�ٵ�ǰ�Ĵ���

		if (bExistOtherWindow == FALSE) 
			break; // û�д��ڴ���

		Sleep(1000);
	}


	if (g_hDesktopBitmap)
	{ 
		DeleteObject(g_hDesktopBitmap);  // ɾ������λͼ
		g_hDesktopBitmap = NULL;
	}
	if (g_hKey)
	{
		DestroyWindow(g_hKey);  // �ƻ�����
	}
	KillTimer(hWnd, ID_TIMESHOW); // ɱ����ʱ��
	DestroyWindow(hWnd); // �ƻ����洰��
}

// ********************************************************************
// ������static void GetIDRect(UINT id,LPRECT lpRect)
// ������
//	IN	id - Ҫ��õ���Χ��ָ���ɣĺ�
//	IN/OUT 	lpRect - ����ָ���ɣĺŵľ��δ�С
// ����ֵ��
//	��
// �����������õ�ָ��ID�ľ��δ�С
// ����: 
// ********************************************************************
static void GetIDRect(UINT id,LPRECT lpRect)
{
}

// ********************************************************************
// ������static void DoShowKeyboard(void)
// ������
//    ��
// ����ֵ��
//	��
// ������������ʾ�����ؼ���
// ����: 
// ********************************************************************
static void DoShowKeyboard(void)
{
//	BOOL bShow = FALSE;
	DWORD dwStyle;//,dwExStyle;

	dwStyle = GetWindowLong(g_hKey,GWL_STYLE);  // �õ����̵ķ��

	if (dwStyle & WS_VISIBLE)
	{  // ��ǰ�����Ѿ���ʾ
		ShowWindow(g_hKey,SW_HIDE);  // ���ؼ���
	}
	else
	{
		ShowWindow(g_hKey,SW_SHOW);
	}
}

// ********************************************************************
// ������static void DoDesktopTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN	hWnd - ����ϵͳ�Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
// 	��
// ������������������WM_WINDOWPOSCHANGING��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoWindowPosChanging(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpwp = (LPWINDOWPOS) lParam;

	if( !(lpwp->flags & SWP_NOZORDER) )
	{  // ���кŸı�
		lpwp->hwndInsertAfter = GetWindow(hWnd, GW_HWNDLAST); // �����洰�ڷŵ������
	}
	lpwp->flags |= SWP_NOACTIVATE;
	return DefWindowProc( hWnd, WM_WINDOWPOSCHANGING, wParam, lParam );  // ����Ĭ����Ϣ
}

// ********************************************************************
// ������static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hDlg- ����ϵͳAbout���ڵĴ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��
// ����������ϵͳ���洰��About���ڵĹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // ������Ϣ
			OnAboutPaint(hDlg);
			return TRUE;
		case WM_CLOSE:  // �ر���Ϣ
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}
// ********************************************************************
//������static void OnAboutPaint( HWND hWnd )
//������
//	IN hWnd- ����ϵͳAbout���ڵĴ��ھ��
//����ֵ��
//	��
//����������ϵͳ���洰��About���ڴ���滭��Ϣ
//����: 
// ********************************************************************
static void OnAboutPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
	HICON hIcon;

		hdc = BeginPaint( hWnd, &ps );  // ��ʼ����
		hIcon = LoadImage( NULL, MAKEINTRESOURCE(OIC_SYSLOGO), IMAGE_ICON, 32, 32, 0 ) ; // �õ�ϵͳ��־ͼ��
		if( hIcon == 0 )
		{  // ��ͼ��ʧ��
			EndPaint( hWnd, &ps );
			return ;
		}
		DrawIcon(hdc,88,8,hIcon);// ����ͼ��		//DeleteObject(hBitMap);
		DestroyIcon(hIcon); // �ƻ�ͼ��
		EndPaint( hWnd, &ps ); // ��������
}

#define IDC_SHOWDESKTOP 1000
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam );

typedef struct EnumWindowStruct{
	HMENU hMenu;
	LPPTRLIST hWndList;
	int iIndex;
}ENUMWINDOW;

// **************************************************
// ������static void DoShowApplication(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ�� ��
// ������������ʾӦ�ó���
// ����: 
// **************************************************
static void DoShowApplication(HWND hWnd)
{
}

// **************************************************
// ������BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������ö�������Ѿ��򿪵Ĵ���
// ����: 
// **************************************************
BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam )
{
}


// **************************************************
// ������static void DrawWallPaper(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ�� ��
// ��������������ǽֽ��
// ����: 
// **************************************************
static void DrawWallPaper(HWND hWnd,HDC hdc)
{
	RECT rect;
	BITMAP bitmap;
	char lpDeskTopName[MAX_PATH];

	GetWallPaperPic(lpDeskTopName); // �õ�ǽֽη;

	if (g_hDesktopBitmap == NULL)
	{  // û��װ��ǽֽ
//		RETAILMSG(1,("***** Start Load Bitmap File \r\n"));
		g_hDesktopBitmap = LoadImage( 0, lpDeskTopName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE ) ; // װ��ǽֽ
		//g_hDesktopBitmap = JPEG_LoadByName( "./kingmos/desktop/mapbj.jpg", NULL ) ; // װ��ǽֽ

//		RETAILMSG(1,("***** Load Bitmap Success \r\n"));
		if( g_hDesktopBitmap == NULL )
		{  // װ��ǽֽʧ��
			return ;
		}
	}

	GetObject(g_hDesktopBitmap,sizeof(bitmap),&bitmap);  // �õ�λͼ�ṹ
	// �õ�λͼ��С
	rect.left = 0;
	rect.right = 640;
	rect.top = 0;
	rect.bottom = 480 ;
	
	DrawBitMap( hdc, g_hDesktopBitmap, &rect, 0, 0 ,SRCCOPY);  // ����λͼ
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

//static LPPTRLIST GetCurrentPtrList( HWND hWnd )
//{
//	return ptrList[diDesktopInfo.uCurrentSelectIndex];	
//}


// ********************************************************************
//������BOOL	DoDrawItem_ListBox( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN idCtl - ��Ϣ����
//  IN pDrawItem - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_DRAWITEM���б�� �������
//����: 
// ********************************************************************
#define	TIME_WIDTH		110
BOOL	DoDrawItem_ListBox( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
{
	HDC			hdc = pDrawItem->hDC;
	LPPTRLIST   lpPtrList = (LPPTRLIST)GetWindowLong( pDrawItem->hwndItem, GWL_USERDATA );   
    DESKTOP_ITEM_DATA * lpItem = PtrListAt( lpPtrList, pDrawItem->itemID );
	RECT rcIcon;

	//
	SetBkMode( hdc, TRANSPARENT );
	if( (pDrawItem->itemState & ODS_SELECTED) 
		 )		//�õ� Select
	{
		//FillRect( hdc, &pDrawItem->rcItem, GetStockObject( WHITE_BRUSH ) );
		//FillSolidRect( hdc, &pDrawItem->rcItem, RGB( 0xff, 203, 90 ) );
		DrawBitMap( hdc, hbmpTextBarSelect, &pDrawItem->rcItem, 0, 0 ,SRCCOPY);  // ����λͼ
		//DrawBitmap( hdc, ,   
		
		if( lpItem )
		{		
			SetTextColor( hdc, RGB( 13, 13, 13 ) );
			TextOut( hdc, pDrawItem->rcItem.left + 61, pDrawItem->rcItem.top + 12, lpItem->szTitle, lpItem->cTitleLength );
		}
		if( lpItem->hbmpIcon )
		{
			rcIcon = pDrawItem->rcItem;
			rcIcon.left += 10;
			rcIcon.top += 3;
			rcIcon.right = rcIcon.left + 40;
			rcIcon.bottom = rcIcon.top + 40;
			DrawTransparentBitMap( hdc, lpItem->hbmpIcon,  &rcIcon, 0, 0, CLR_TRANSPARENT );
		}
	}
	else
	{
		//FillRect( hdc, &pDrawItem->rcItem, GetStockObject( GRAY_BRUSH ) );
		DrawBitMap( hdc, hbmpTextBarNormal, &pDrawItem->rcItem, 0, 0 ,SRCCOPY);  // ����λͼ
		
		if( lpItem )
		{		
			SetTextColor( hdc, RGB( 0xff, 0xff, 0xff ) );
			TextOut( hdc, pDrawItem->rcItem.left + 61, pDrawItem->rcItem.top + 12, lpItem->szTitle, lpItem->cTitleLength );
		}
		if( lpItem->hbmpIcon )
		{
			rcIcon = pDrawItem->rcItem;
			rcIcon.left += 10;
			rcIcon.top += 3;
			rcIcon.right = rcIcon.left + 40;
			rcIcon.bottom = rcIcon.top + 40;
			
			DrawTransparentBitMap( hdc, lpItem->hbmpIcon,  &rcIcon, 0, 0, CLR_TRANSPARENT );
		}

	}

	return TRUE;
}

// ********************************************************************
//������BOOL	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
//������
//	IN hWnd - Ӧ�ó���Ĵ��ھ��
//  IN idCtl - ��Ϣ����
//  IN pDrawItem - ��Ϣ����
//����ֵ��
//	��Ϣ����󷵻صĽ��
//����������WM_DRAWITEM �������
//����: 
// ********************************************************************
static int	DoDrawItem( HWND hWnd, UINT idCtl, DRAWITEMSTRUCT* pDrawItem )
{
	if( pDrawItem->CtlType==ODT_LISTBOX )
	{
		return DoDrawItem_ListBox( hWnd, idCtl, pDrawItem );
	}
	return 0;
}



// **************************************************
// ������static void DrawDesktopControl(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ����
// ������������������ؼ���
// ����: 
// **************************************************

#define HILIGHT_COLOR  RGB( 180, 180, 20 )
#define NORMAL_COLOR  RGB( 80, 80, 80 )
static void DrawDesktopControl(HWND hWnd,HDC hdc)
{
	//int iButtonCount = sizeof(DeskTopFuncList)/sizeof(DESKTOPFUNC);  // �õ���ť����
	//int i;
	
	//SetBkColor( hdc, RGB(0xff, 0, 0 ) );
	//for (i=0;i<iButtonCount;i++)
	//{
	//	DrawText( hdc, DeskTopFuncList[i].lpFuncName, -1, &DeskTopFuncList[i].rectFunc, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
	//}
	UINT i;
//	return 0;

	
	SetBkMode( hdc, TRANSPARENT );
	SelectObject( hdc, GetStockObject(  BLACK_BRUSH ) );
	
	
	for( i = 0; i < DESKTOP_CONTROL_NUM; i++ )
	{
		RECT rc = diDesktopInfo.deskFun[i].rectFunc;
		OffsetRect( &rc, ICON_OFFSET_X, ICON_OFFSET_Y );

		
		if( i == diDesktopInfo.uCurrentSelectIndex )
		{
			HPEN hPen = CreatePen( PS_SOLID, 1, HILIGHT_COLOR );

			hPen = SelectObject( hdc, hPen );
			SetTextColor( hdc, HILIGHT_COLOR );
			RoundRect( hdc, rc.left, rc.top, rc.right, rc.bottom, 18, 18 );
			DrawText( hdc, diDesktopInfo.deskFun[i].lpTitle, strlen(diDesktopInfo.deskFun[i].lpTitle), &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
			hPen = SelectObject( hdc, hPen );
			DeleteObject( hPen );
			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpState[diDesktopInfo.uCurrentSelectState],  &diDesktopInfo.deskFun[i].rectFunc, 0, 0, CLR_TRANSPARENT );
			
			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpIcon[0],  &diDesktopInfo.deskFun[i].rectIcon, 0, 0, CLR_TRANSPARENT );
		}
		else
		{
			HPEN hPen = CreatePen( PS_SOLID, 1, NORMAL_COLOR );

			hPen = SelectObject( hdc, hPen );
			SetTextColor( hdc, NORMAL_COLOR );
			RoundRect( hdc, rc.left, rc.top, rc.right, rc.bottom, 18, 18  );
			DrawText( hdc, diDesktopInfo.deskFun[i].lpTitle, strlen(diDesktopInfo.deskFun[i].lpTitle), &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

			hPen = SelectObject( hdc, hPen );
			DeleteObject( hPen );

			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpState[0],  &diDesktopInfo.deskFun[i].rectFunc, 0, 0, CLR_TRANSPARENT );
			
			//DrawTransparentBitMap( hdc, diDesktopInfo.deskFun[i].lpbmpIcon[0],  &diDesktopInfo.deskFun[i].rectIcon, 0, 0, CLR_TRANSPARENT );
		}		
	}


}

// **************************************************
// ������static void DrawTaskBar(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ����
// ����������������������
// ����: 
// **************************************************
static void DrawTaskBar(HWND hWnd,HDC hdc)
{
}


// **************************************************
// ������static HWND GetDesktop(void)
// ������
// 	��
// ����ֵ�� ��������Ĵ��ھ��
// �����������õ�����Ĵ��ھ����
// ����: 
// **************************************************
static HWND GetDesktop(void)
{
	return FindWindow(classDESKTOP,NULL);
}



// **************************************************
// ������static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lParam -- ���̲���
// 
// ����ֵ���д��ڴ��ڷ���TRUE�����򷵻�FALSE
// ����������ö���Ѿ����ڵĴ��ڡ�
// ����: 
// **************************************************
static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam )
{
	BOOL * lpbExistOtherWindow;
//    MENUITEMINFO		menu_info;
//	TCHAR lpAppName[100];
	HWND hParent;
	HWND hChild;
		
		if (hWnd == NULL)
			return FALSE;  // �Ѿ�û�д���
		hParent = GetParent(hWnd);
		if (hParent != NULL)
			return TRUE;  // ���д���
		hChild = GetDesktop();//GetDesktopWindow();
		if (hChild == hWnd)
			return TRUE; // ��ǰ���������洰�ڣ����ƣ����д���

		lpbExistOtherWindow = (BOOL *)lParam;
		
		*lpbExistOtherWindow = TRUE; // ���û��д��ڴ���
		return FALSE; // ��Ҫ����ö��
}

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
// ������static HWND CreateSystemKeyboard(HWND hParent)
// ������
// 	IN hParent -- ���ھ��
// 
// ����ֵ�����ؼ��̵Ĵ��ھ����
// ��������������ϵͳ���̡�
// ����: 
// **************************************************
static HWND CreateSystemKeyboard(HWND hParent)
{
	HWND hKey;
	// ����ϵͳ���̡�

#define KEYBOARDSTARTX  30
#define KEYBOARDSTARTY  (320-20-KEYBOARDHEIGHT)

    hKey = CreateWindowEx(WS_EX_TOPMOST|WS_EX_INPUTWINDOW,  
						classKEYBOARDWINDOW,
						"����",
						WS_POPUP,//|WS_VISIBLE,//|WS_CAPTION,
						KEYBOARDSTARTX,
						KEYBOARDSTARTY,
						KEYBOARDWIDTH,
						KEYBOARDHEIGHT,
						hParent,
						(HMENU)101,
						(HINSTANCE)GetWindowLong(hParent,GWL_HINSTANCE),
						NULL);
	return hKey;
}


// ********************************************************************
// ������static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance )
// ������
//	IN hInstance - ����ϵͳ��ʵ�����
// ����ֵ��
// 	��
// ����������ע��ϵͳ������
// ����: 
// ********************************************************************
static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)KeyboardWndProc;  // ������̺���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classKEYBOARDWINDOW;
	return RegisterClass(&wcex); // ��ϵͳע��������
}

// ********************************************************************
// ������static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd- ����ϵͳ�Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��
// ����������ϵͳ���洰�ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:  // �滭��Ϣ
			hdc = BeginPaint( hWnd, &ps );  // ��ʼ����
			EndPaint( hWnd, &ps ); // ��������
			return 0;
		case WM_CREATE: // ������Ϣ
			DoKeyboardWndCreate(hWnd);
			return 0;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // ����Ĭ����Ϣ
   }
   return FALSE;
}

// ********************************************************************
// ������static void DoKeyboardWndCreate(HWND hWnd)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// �����������������洴����Ϣ
// ����: 
// ********************************************************************
static void DoKeyboardWndCreate(HWND hWnd)
{
	HWND hKey;
	hKey = CreateKeyboard((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE),hWnd,0);
	if (hKey)
		ShowKeyboard(hKey,SIP_SHOW); // ��ʾ����
}
