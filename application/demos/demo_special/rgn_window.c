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


static TCHAR szWindowClass[] = "rgnap";								// The title bar text

static ATOM RegistertestClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);

static LRESULT CALLBACK RgnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCloseWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);



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
int WINAPI WinMain_DemoRgnWindow(HINSTANCE hInstance,
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

// ********************************************************************
//������ATOM RegistertestClass(HINSTANCE hInstance)
//������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
//����������ע�ᵱǰӦ�ó������
//����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
static ATOM RegistertestClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)RgnWndProc; // �豸�����ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
//	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_DEVICEMANAGE),IMAGE_ICON,16,16,0);
	wc.hIcon			= NULL;//LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// װ��Ӧ�ó���ͼ��
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= CreateSolidBrush( RGB( 0, 0, 0xff ) );//GetStockObject(WHITE_BRUSH); // ���ñ���ˢ
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // �豸����������

	return RegisterClass(&wc); // ע����

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
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX,szWindowClass, "��Ч����", WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE,
      60, 40, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // ��������ʧ��
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow); // ��ʾ����
   UpdateWindow(hWnd); // ��ʾ����

   return TRUE;
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
static LRESULT DoCreateRgnWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT rt, rcClient;
	HRGN hrgn, hrgnDiff, hrgnOr;
	POINT pt;
	int xmp, ymp;

	GetWindowRect(hWnd, &rt); // �õ�������Ļ����

	GetClientRect(hWnd, &rcClient );
	pt.x = pt.y = 0;
	ClientToScreen( hWnd, &pt );
	//OffsetRect( &rcClient, pt.x, pt.y );

	OffsetRect( &rcClient, pt.x - rt.left, pt.y - rt.top );


	
	OffsetRect( &rt, -rt.left, -rt.top ); // log cor

	hrgn = CreateRectRgn( 0, 0, 0, 0 );//rt.left, rt.top, rt.right, rt.bottom );
	GetWindowRgn( hWnd, hrgn );

	hrgnDiff = CreateRectRgn( rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );

	CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	DeleteObject( hrgnDiff );

	xmp = ( rcClient.right + rcClient.left ) / 2;
	ymp = ( rcClient.bottom + rcClient.top ) / 2;

	hrgnOr = CreateRectRgn( xmp - 16 , ymp - 16, xmp + 16, ymp + 16 );

	CombineRgn( hrgn, hrgn, hrgnOr, RGN_OR );
	DeleteObject( hrgnOr );

	SetWindowRgn( hWnd, hrgn, FALSE );

	
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
static LRESULT CALLBACK RgnWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
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
			return DoCreateRgnWindow(hWnd,wParam,lParam);
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

