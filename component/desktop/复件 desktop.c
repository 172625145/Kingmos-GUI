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
//#include "sipapi.h"
#include "resource.h"
#include "emmsys.h"
#include <keybdsrv.h>
#include <softkey.h>

/***************  ȫ���� ���壬 ���� *****************/

char   classDESKTOP[20] ;
#define DESKTOPCLASSNAME  "_DESKTOP_CLASS_"

char classKEYBOARDWINDOW[] = "_KEYBOARD_WINDOW_CLASS_";

HINSTANCE hInst;

static HBITMAP g_hDesktopBitmap = NULL;
typedef struct DesktopFunStruct{
	RECT rectFunc;
	LPCTSTR lpFuncName;
	UINT idFunc;
}DESKTOPFUNC , *LPDESKTOPFUNC;

#define COLOR_TASKBAR  RGB(205,205,205)
#define COLOR_DESKTOPBK  RGB(240,240,240)

#define ID_LOGO     0x2038
#define ID_FUNC1    0x2039
#define ID_FUNC2    0x203A
#define ID_FUNC3    0x203B
#define ID_FUNC4    0x203C
#define ID_FUNC5    0x203D
#define ID_FUNC6    0x203E
#define ID_ICON1	0x203F
#define ID_ICON2	0x2040
#define ID_ICON3	0x2041  // Battery property
#define ID_ICON4	0x2042
#define ID_TIME     0x2043
#define ID_SHOW     0x2044
#define ID_KEYBD    0x2045
#define ID_MYCOMPUTER  0x2046

#define ID_START	0x100

#ifdef INLINE_PROGRAM
static DESKTOPFUNC DeskTopFuncList[] ={
	{{  8,  2, 24, 18},"Func Logo",ID_LOGO},
	{{  0, 20, 30, 50},"contactbook",ID_FUNC1},
	{{  0, 58, 30, 88},"PlanDay",ID_FUNC2},
	{{  0, 96, 30,126},"Notebook",ID_FUNC3},
	{{  0,134, 30,164},"Mobilephone",ID_FUNC4},
//	{{  0,172, 30,202},"FlyBat",ID_FUNC5},
	{{  0,172, 30,202},"HWChess",ID_FUNC5},
	{{  0,210, 30,240},"IExplore",ID_FUNC6},
	{{  7,248, 23,264},"EasyBox",ID_ICON1},
	{{  7,265, 23,281},"Alarm",ID_ICON2},
	{{  7,282, 23,298},"Setting",ID_ICON3},
	{{  7,299, 23,315},"TEST",ID_ICON4},
	{{203,302,219,318},"Func12",ID_SHOW},
	{{ 30,303,160,319},"Setting",ID_TIME},
	{{221,302,237,318},"keyboard",ID_KEYBD},
	{{ 49, 20, 81, 52},"Manage",ID_MYCOMPUTER},
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
	{{ 49, 20, 81, 52},"\\kingmos\\test.exe",ID_MYCOMPUTER},
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
static void DoDesktopCreate(HWND hWnd);
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

static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop);

static BOOL CALLBACK EnumExistWindowsProc(HWND hWnd, LPARAM lParam );

static HWND CreateSystemKeyboard(HWND hParent);

static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static void OnAboutPaint( HWND hWnd );


static ATOM RegisterKeyboardWndClass( HINSTANCE hInstance );
static LRESULT CALLBACK KeyboardWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DoKeyboardWndCreate(HWND hWnd);


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
//		TouchCalibrate();  // ���л���У��

		RegisterDesktopClass(hInstance); // ע�����������
		RegisterKeyboardWndClass(hInstance);
		hInst = hInstance;
		// �������洰��
		CreateWindow( classDESKTOP, "����", WS_VISIBLE|WS_CLIPCHILDREN,
			0, 0,
			GetSystemMetrics( SM_CXSCREEN ),
			GetSystemMetrics( SM_CYSCREEN ),
			0,0,hInstance,0 );		


		//sndPlaySound("\\kingmos\\kingmos.wav",SND_ASYNC); // ���ſ�������

		while (GetMessage(&msg, NULL, 0, 0)) 
		{	// ��Ϣ����
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// ��������Ѿ��˳������˳�ϵͳ
		ExitSystem(EXS_SHUTDOWN,0); // �˳�ϵͳ
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
	wcex.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
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
		case WM_ERASEBKGND:  // ���Ʊ���
			OnDesktopEraseBkgnd( hWnd,( HDC )wParam );
			return 0;
		case WM_LBUTTONDOWN: // �����ť���£���TOUCH�ʰ���
			DoDesktopLButtonDown(hWnd,wParam,lParam);
			return 0;
		case WM_LBUTTONUP:// �����ť���𣬻�TOUCH���뿪
			DoDesktopLButtonUp(hWnd,wParam,lParam);
			return 0;
		case WM_TIMER: // ��ʱ��
			DoDesktopTimer(hWnd,wParam,lParam);
			return 0;
		case WM_WINDOWPOSCHANGING: // ����λ�øı�
			return DoWindowPosChanging(hWnd,wParam,lParam);
//            return 0;
		case WM_CREATE: // ������Ϣ
			DoDesktopCreate(hWnd);
			return 0;
		case WM_CLOSE:  // �ر���Ϣ
			DoDesktopClose(hWnd);
			return 0;
		case WM_DESTROY: // �ƻ���Ϣ
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // ����Ĭ����Ϣ
   }
   return FALSE;
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
	RECT				rt;
//	HBITMAP				hBitMap ;
	HBRUSH hBrush;

//	char lpDeskTopName[MAX_PATH];

	GetClientRect( hWnd, &rt ); // �õ��ͻ�����
	rt.left += 30; // ������Ӧ�ó�����
	rt.bottom -= 20; // ������ϵͳ״̬��
	hBrush = CreateSolidBrush(COLOR_DESKTOPBK); // ��������ˢ
	FillRect( hdc, &rt, hBrush ) ; // ��䱱��
	DeleteObject(hBrush); // ɾ������ˢ
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
	strcpy(lpDeskTopName,"./kingmos/mlgsign.bmp"); // ����ָ�������汳��λͼ
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
		g_indexButtonDown = indexButton; // ���ð��°�ť
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
	POINTS ptCursor;
	UINT indexButton;

		if (g_indexButtonDown == BANK_POSITION)
			return ; // ����λ��û�а�ť
		ptCursor = MAKEPOINTS(lParam); // �õ���굯��λ��
		indexButton = GetButtonID(ptCursor); // �õ�����λ�õİ�ťID

		if (g_indexButtonDown == indexButton)
		{ // �밴��ʱ��λ��һ��
			//����ָ����Ӧ�ó���
			switch(DeskTopFuncList[g_indexButtonDown].idFunc)
			{
				case ID_KEYBD: // ���̰�ť
					DoShowKeyboard();  // ��ʾ����
					break;
				case ID_SHOW: // ��ʾ��ť
					DoShowApplication(hWnd); // ��ʾӦ�ó���
					break;
				case ID_LOGO:  // ��־��ť
					DialogBoxIndirectEx(hInst,(LPDLG_TEMPLATE_EX)&dlgIDD_ABOUT,NULL, (DLGPROC)About); // �������ڰ�ť
					break;
				case ID_MYCOMPUTER:  
				case ID_ICON3:  
				case ID_TIME:  
#ifdef INLINE_PROGRAM
					LoadApplication(DeskTopFuncList[g_indexButtonDown].lpFuncName,NULL);
#else
					CreateProcess(DeskTopFuncList[g_indexButtonDown].lpFuncName,NULL,0,0,0,0,0,0,0,0);  // ����ָ����Ӧ�ó���
#endif
					break;
				default:
					MessageBox(hWnd,DeskTopFuncList[g_indexButtonDown].lpFuncName,"Error",MB_OK);
					break;
			}
		}
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
	UINT idButton = BANK_POSITION;
	int iButtonCount = sizeof(DeskTopFuncList)/sizeof(DESKTOPFUNC);  // �õ���ť����
	POINT pt;
	int i;

		pt.x = ptCursor.x;
		pt.y = ptCursor.y;

		for (i=0;i<iButtonCount;i++)
		{
			if (PtInRect(&DeskTopFuncList[i].rectFunc,pt))  // �жϵ�ǰָ��λ���Ƿ���ָ����ť��
			{
				idButton = i;
				return idButton; // ���ص�ǰ�İ�ťID
			}
		}
		return idButton; // ���ؿհ�λ��
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
static void DoDesktopCreate(HWND hWnd)
{
	g_hKey = CreateSystemKeyboard(hWnd);  // ����ϵͳ����
	SetTimer(hWnd, ID_TIMESHOW,1000,NULL); // ������ʾʱ�䶨ʱ��
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
	RECT				rt;
	SYSTEMTIME SystemTime;
	TCHAR lpLocalTime[20];
	TCHAR *Week[] = {"��","һ","��","��","��","��","��",""};
	HBRUSH hBrush;
	

		GetIDRect(ID_TIME,&rt);  // �õ���ʾʱ��ľ��δ�С
		hBrush = CreateSolidBrush(COLOR_TASKBAR); // �õ��������ı�����ɫ
		FillRect(hdc,&rt,hBrush); // ���ʱ����
		DeleteObject(hBrush); // ɾ����ˢ
		//�õ���ǰ��ʱ��
		GetLocalTime(&SystemTime); // �õ���ǰʱ��
		// ��ǰʱ��ת��Ϊ�ִ�
		sprintf(lpLocalTime,"%02d-%02d %02d:%02d(%s)",SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,Week[SystemTime.wDayOfWeek]); 

		SetBkMode( hdc, TRANSPARENT ) ; // ����͸��ģʽ
		DrawText(hdc,lpLocalTime,strlen(lpLocalTime),&rt,DT_CENTER|DT_VCENTER|DT_SINGLELINE); // ��ʾ��ǰʱ��

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
	UINT idButton = BANK_POSITION;
	int iButtonCount = sizeof(DeskTopFuncList)/sizeof(DESKTOPFUNC);  // �õ���ť������
	int i;

		for (i=0;i<iButtonCount;i++)
		{
			if (DeskTopFuncList[i].idFunc == id)
			{  // ���ҵ���ǰ��ť����Ҫ���ҵİ�ť
				*lpRect = DeskTopFuncList[i].rectFunc; // ���ظð�ť�ľ���
				return ;
			}
		}
		return ;
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
    MENUITEMINFO		menu_info;
	HMENU		hMenu = CreatePopupMenu();  // ����һ���˵�
	int iIndex = 0, id_Menu;
//	TCHAR lpAppName[100];
	HWND hBrother;
//	RECT rect;
//	int x = 140,y= 200;
	PTRLIST hWndList;
	ENUMWINDOW hEnumWindow;

	PtrListCreate(&hWndList,(ccIndex)10,(ccIndex)6); // ����һ��ָ���б�
	// ��ʼ���˵��ṹ
	menu_info.cbSize  = sizeof( MENUITEMINFO );  
	menu_info.fMask   = MIIM_TYPE | MIIM_ID|MIIM_DATA;
	menu_info.fType   = MFT_STRING  ;
	
	menu_info.wID =  iIndex + ID_START;
	menu_info.dwTypeData = "��ʾ����" ;
	menu_info.cch = 8 ;
	if( !InsertMenuItem( hMenu, 0, TRUE, &menu_info )  )  // ����һ������ʾ���桱�˵���Ŀ
	{
		EdbgOutputDebugString( "===InitManiMenu   failed===\r\n" );
		goto MENUEND;
	}
	iIndex ++;
	PtrListInsert(&hWndList, NULL); // ����һ����ָ�뵽�����б�
	hEnumWindow.hMenu = hMenu;
	hEnumWindow.hWndList = &hWndList;
	hEnumWindow.iIndex = iIndex;
	EnumWindows(EnumWindowsProc,(LPARAM)&hEnumWindow);  // ö�ٴ���

	id_Menu = TrackPopupMenu( hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN |TPM_NONOTIFY | TPM_RETURNCMD, 240, 300, 0, hWnd, NULL ); // �����˵�
	if (id_Menu != 0)
	{  // ��ѡ����Ŀ 
		iIndex = id_Menu - ID_START ; // �õ���Ŀ����
		if (iIndex == 0)
		{ //ѡ�����Ŀ�ǡ���ʾ���桱
			ShowDesktop(0);
		}
		else
		{  // ��Ӧ�ó���
			hBrother = PtrListAt(&hWndList, iIndex );// �õ����ھ��
			ShowWindow(hBrother,SW_RESTORE); // �ָ�ѡ��Ĵ�����ʾ
		}
	}

MENUEND:
	PtrListDestroy(&hWndList); // �ƻ��б�
	DestroyMenu(hMenu); // �ƻ��˵�
	return ;

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
	ENUMWINDOW *lpEnumWindow;
    MENUITEMINFO		menu_info;
	TCHAR lpAppName[100];
	HWND hParent;
	HWND hChild;
		
		if (hWnd == NULL)
			return FALSE;  // ��Ч���ھ��
		hParent = GetParent(hWnd); // �õ�������
		if (hParent != NULL)
			return TRUE; // û�и�����
		hChild = GetDesktop();//GetDesktopWindow(); // �õ����洰�ھ��
		if (hChild == hWnd)  //��ǰ���ھ������洰�ڣ�������ֱ�ӷ���
			return TRUE;

		lpEnumWindow = (ENUMWINDOW *)lParam;  //�õ�ö�ٴ��ڽṹ

		if (lpEnumWindow->iIndex == 1 )
		{  // ����ǵ�һ��Ӧ�ó�����Ҫ����һ��SEPARATOR
			menu_info.fMask   = MIIM_TYPE ;
			menu_info.fType   = MFT_SEPARATOR  ;
			if( !InsertMenuItem( lpEnumWindow->hMenu, 0, TRUE, &menu_info )  ) {
				EdbgOutputDebugString( "===InitManiMenu   failed===\r\n" );
				return FALSE;
			}
		}

		PtrListInsert(lpEnumWindow->hWndList, hWnd); // �����ҵ��Ĵ��ھ�����б�
		GetWindowText(hWnd,lpAppName,100); // �õ�Ӧ�ó�����

		menu_info.fMask   = MIIM_TYPE | MIIM_ID|MIIM_DATA;
		menu_info.fType   = MFT_STRING  ;
		menu_info.wID =  lpEnumWindow->iIndex + ID_START;
		menu_info.dwTypeData = lpAppName ;
		menu_info.cch = strlen( lpAppName ) ;
		if( !InsertMenuItem( lpEnumWindow->hMenu, 0, TRUE, &menu_info )  )  // ���뵱ǰӦ�ó��򵽲˵�
		{ 
			return FALSE;
		}
		lpEnumWindow->iIndex ++;
		return TRUE;
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
//		RETAILMSG(1,("***** Load Bitmap Success \r\n"));
		if( g_hDesktopBitmap == NULL )
		{  // װ��ǽֽʧ��
			return ;
		}
	}

	GetObject(g_hDesktopBitmap,sizeof(bitmap),&bitmap);  // �õ�λͼ�ṹ
	// �õ�λͼ��С
	rect.left = AP_XSTART + (AP_WIDTH - bitmap.bmWidth) /2 ;
	rect.right = rect.left + bitmap.bmWidth ;
	rect.top = AP_YSTART + (AP_HEIGHT - bitmap.bmHeight) /2 ;
	rect.bottom = rect.top + bitmap.bmHeight ;
	
	DrawBitMap( hdc, g_hDesktopBitmap, rect, 0, 0 ,SRCCOPY);  // ����λͼ
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
static void DrawDesktopControl(HWND hWnd,HDC hdc)
{
	HINSTANCE hInstance;
	HBITMAP hBitmap;
	HICON hIcon;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // �õ�ʵ�����

//		RETAILMSG(1,("***** Start Draw App Bar \r\n"));
		// װ��Ӧ�ó�����λͼ
		hBitmap = LoadImage( hInstance, MAKEINTRESOURCE(IDB_APPBAR), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ;
		if( hBitmap )
		{ // װ��λͼ�ɹ�
			RECT rectApBar = {0,0,30,320}; // ����Ӧ�ó������Ĵ�С
			DrawBitMap( hdc, hBitmap, rectApBar, 0, 0 ,SRCCOPY); // ����λͼ
			DeleteObject(hBitmap); // ɾ��λͼ���
		}

//		RETAILMSG(1,("***** Start Draw Task Bar \r\n"));
		DrawTaskBar(hWnd,hdc); // ����������
//		RETAILMSG(1,("***** Start Draw WallPaper \r\n"));
		DrawWallPaper(hWnd,hdc); // ����ǽֽ

		// װ�ء��ҵĵ��ԡ�ͼ��
		hIcon = LoadImage( hInstance, MAKEINTRESOURCE(IDI_COMPUTER), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE ) ;
		RETAILMSG(1,("***** Load Icon (%x)File OK \r\n",hIcon));
		if( hIcon )
		{ // װ�سɹ�
			RECT rect;
			GetIDRect(ID_MYCOMPUTER,&rect);  // �õ�λ��
			DrawIcon(hdc,rect.left,rect.top,hIcon); // ����ͼ��
			DestroyIcon(hIcon);  // �ƻ�ͼ��
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
	HINSTANCE hInstance;
//	HBITMAP hBitmap;
	HICON hIcon;
	HBRUSH hBrush;
	RECT rectTaskBar = {30,300,240,320};  // ��������λ��

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����

		hBrush = CreateSolidBrush(COLOR_TASKBAR); // �õ��������ı�����ɫ
		FillRect(hdc,&rectTaskBar,hBrush); // ��䱳��
		DeleteObject(hBrush); // ɾ����ˢ
		// װ�ز˵���ť
		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE(IDI_MENU), IMAGE_ICON, 16, 16, 0 ) ;
		{
			RECT rect;
			GetIDRect(ID_SHOW,&rect);  // �õ��˵���λ��
			DrawIcon(hdc,rect.left,rect.top,hIcon); // ����ͼ��
			DestroyIcon(hIcon); // �ƻ�ͼ��
		}

		// װ�ؼ��̰�ť
		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE(IDI_KEYBOARD), IMAGE_ICON, 16, 16, 0 ) ;
		{
			RECT rect;
			GetIDRect(ID_KEYBD,&rect); // �õ����̵�λ��
			DrawIcon(hdc,rect.left,rect.top,hIcon);// ����ͼ��
			DestroyIcon(hIcon); // �ƻ�ͼ��
		}

		ShowTime(hWnd,hdc);  // ��ʾʱ��
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
static void DrawBitMap(HDC hdc,HBITMAP hBitmap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
		HDC hMemoryDC;
    HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // ����һ�����ݵ��ڴ���
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼ���õ��ڴ�����
		BitBlt( hdc, // handle to destination device context
			(short)rect.left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)rect.top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(rect.right-rect.left),  // width of destination rectangle
			(short)(rect.bottom-rect.top), // height of destination rectangle
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
