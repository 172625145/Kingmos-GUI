/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�����豸�����û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-18
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <ewindows.h>

/***************  ȫ���� ���壬 ���� *****************/
#define MAX_SEARCHSTRINGLEN	64

#define ID_EDIT		100

TCHAR szWindowClass[] = "AlphaClass";								// The title bar text

static HBITMAP hbmpBackground = NULL;
static HBITMAP hbmpKingmoGUI = NULL;
static HBITMAP hbmpMake = NULL;

static ATOM RegistertestClass(HINSTANCE hInstance);

static LRESULT CALLBACK AlphaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);



// ********************************************************************
//������ATOM RegisterAlphaClass(HINSTANCE hInstance)
//������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
//����������ע�ᵱǰӦ�ó������
//����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegistertestClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
//	HBITMAP hBitmap;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)AlphaWndProc; // �豸�����ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// װ��Ӧ�ó���ͼ��
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= CreateSolidBrush( RGB( 0, 0, 255 ) );
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // �豸����������

	return RegisterClass(&wc); // ע����

}

// ********************************************************************
//������static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//  IN wParam  - ��Ϣ����
//  IN lParam  - ��Ϣ����
//����ֵ��
//	����0���������������ڣ�����-1������ƻ�����
//����������Ӧ�ó���������Ϣ
//����: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	if( hbmpBackground == NULL )
		hbmpBackground = LoadImage( NULL, "./kingmos/demos/alpha_bk.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if( hbmpKingmoGUI == NULL )
		hbmpKingmoGUI = LoadImage( NULL, "./kingmos/demos/kingmosgui.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if( hbmpMake == NULL )
	{
		hbmpMake = CreateCompatibleBitmap( NULL, 240, 320 );
	}
	SetTimer( hWnd, 0, 50, NULL );

	return 0;
}

#define MARK_WIDTH 103
#define MARK_HEIGHT 102
static int DoPaint( HWND hWnd, HDC hdc, int x, int y, int alpha )
{
	HDC hMemMakeDC = CreateCompatibleDC( hdc );
	HDC hMemDC = CreateCompatibleDC( hdc );
	BLENDFUNCTION_EX bfex;

	SelectObject( hMemMakeDC, hbmpMake );
	SelectObject( hMemDC, hbmpBackground );
	BitBlt( hMemMakeDC, 0, 0, 240, 320, hMemDC, 0, 0, SRCCOPY );

	SelectObject( hMemDC, hbmpKingmoGUI );
	bfex.bf.AlphaFormat = AC_SRC_ALPHA;
	bfex.bf.BlendFlags = 0;
	bfex.bf.BlendOp = AC_SRC_OVER;
	bfex.bf.SourceConstantAlpha = alpha;

	bfex.rgbTransparent = RGB( 0xff, 0, 0xff );


	AlphaBlendEx( hMemMakeDC, x, y, MARK_WIDTH, MARK_HEIGHT, hMemDC, 0, 0, MARK_WIDTH, MARK_HEIGHT, &bfex );

	BitBlt( hdc, 0, 0, 240, 320, hMemMakeDC, 0, 0, SRCCOPY );

	DeleteDC( hMemDC );
	DeleteDC( hMemMakeDC );
	return 0;
}

// ********************************************************************
//������static LRESULT CALLBACK RgnWndProc(HWND , UINT , WPARAM , LPARAM )
//������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//  IN message - ������Ϣ
//  IN wParam  - ��Ϣ����
//  IN lParam  - ��Ϣ����
//����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
//����������Ӧ�ó��򴰿ڹ��̺���
//����: 
// ********************************************************************
static LRESULT CALLBACK AlphaWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_TIMER:
		{
			static int x = 0;
			static int y = 0;
			static int xdir = 1;
			static int ydir = 1;
			static int alpha = 0;
			static int alphadir = 5;
			HDC hdc = GetDC( hWnd );
			
			
			x += xdir;
			y += ydir;
			alpha += alphadir;

			DoPaint( hWnd, hdc, x, y, alpha );

			if( x >= 230 || x < 0 )
				xdir = -xdir;
			if( y >= 280 || y < 0 )
				ydir = -ydir;
			if( alpha >= 255 || alpha <= 0 )
				alphadir = -alphadir;
			ReleaseDC( hWnd, hdc );			
		}
		return 0;
	case WM_PAINT: // ���ƴ���
		hdc = BeginPaint(hWnd, &ps);
		//			DoPaint(hWnd,hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND: // ������Ϣ
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE: // ������Ϣ
		return DoCreateWindow(hWnd,wParam,lParam);
	case WM_DESTROY: // �ƻ���Ϣ
		PostQuitMessage(0); // �˳�����
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
   return 0;
}


// ********************************************************************
//������static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//  IN nCmdShow  - Ӧ�ó�����ʾ����
//����ֵ��
//	�ɹ�����TRUE, ʧ�ܷ���FALSE
//������������ʼ����ǰʵ������������ǰ������
//����: 
// ********************************************************************
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   // ��������
   hWnd = CreateWindowEx( WS_EX_CLOSEBOX,szWindowClass, "Alpha����", WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE,
      0, 0, 240, 320, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // ��������ʧ��
      return FALSE;
   }

	// �������ڳɹ�
   ShowWindow(hWnd, nCmdShow); // ��ʾ����
   UpdateWindow(hWnd); // ��ʾ����

   return TRUE;
}

// ********************************************************************
//������void WinMain_test(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_DemoAlpha(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#else
int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#endif
{
 	// TODO: Place code here.
	MSG msg;
	HWND hWnd;
//	char buf[128];

	//hInst = hInstance;
	
	hWnd = FindWindow( szWindowClass, NULL ) ;  // ���ҵ�ǰ�����Ƿ��Ѿ�����
	if( hWnd != 0 )
	{ // �Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ���ǰ��
		return FALSE ;
	}
	RegistertestClass(hInstance); // ע���豸������

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))  // ��ʼ��Ӧ�ó���
	{
		return FALSE;
	}


	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam; // �˳�Ӧ�ó���
}
