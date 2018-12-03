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

//static HINSTANCE hInst;								// current instance
static TCHAR szTitle[] = "test";								// The title bar text
static TCHAR szWindowClass[] = "testap";								// The title bar text

static ATOM RegistertestClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK testWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);



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
int WINAPI WinMain_DemoNormal(HINSTANCE hInstance,
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
ATOM RegistertestClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)testWndProc; // �豸�����ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
//	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_DEVICEMANAGE),IMAGE_ICON,16,16,0);
	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APP_SYSTEM),IMAGE_ICON,16,16,0);// װ��Ӧ�ó���ͼ��
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH); // ���ñ���ˢ
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


   //hInst = hInstance; // ������ǰ��ʵ�����

   // ��������
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX,szWindowClass, "��ͨԲ�Ǵ���", WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // ��������ʧ��
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);
	// �������ڳɹ�
   ShowWindow(hWnd, nCmdShow); // ��ʾ����
   UpdateWindow(hWnd); // ��ʾ����

   return TRUE;
}

// ********************************************************************
//������static LRESULT CALLBACK testWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK testWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	RECT rt;
	GetClientRect(hWnd, &rt); // �õ����ڿͻ�����
	// ����LISTVIEW
	CreateWindow( classEDIT,"������΢�߼��������޹�˾ \r\n      Hello Word!!!",
				WS_VISIBLE|WS_CHILD|ES_MULTILINE,
				rt.left,
				rt.top,
				rt.right-rt.left,
				80,
				hWnd,
				(HMENU)ID_EDIT,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	
	return 0;
}
