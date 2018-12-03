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
#include <EWindows.h>
#include <eComCtrl.h>
#include <Etoolbar.h>
#include "resource.h"

/***************  ȫ���� ���壬 ���� *****************/
#define MAX_LOADSTRING  100
#define MAX_SEARCHSTRINGLEN	64

#define ID_LISTVIEW  301

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "�豸����";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "DeviceManage";								// The title bar text

ATOM RegisterDeviceManageClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK DeviceManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);


static HWND  CreateListView(HWND  hWnd);
static void	LoadDeviceMangeImage(HWND hWnd,HWND hListView);
static BOOL  CreateListViewColumn( HWND  hListView);
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage);

// ********************************************************************
//������void WinMain_DeviceManage(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_DeviceManage(HINSTANCE hInstance,
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

	hWnd = FindWindow( szWindowClass, NULL ) ;  // ���ҵ�ǰ�����Ƿ��Ѿ�����
	if( hWnd != 0 )
	{ // �Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ���ǰ��
		return FALSE ;
	}
	InitCommonControls(); // ��ʼ��ͨ�ÿؼ�
	RegisterDeviceManageClass(hInstance); // ע���豸������

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
//������ATOM RegisterDeviceManageClass(HINSTANCE hInstance)
//������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
//����������ע�ᵱǰӦ�ó������
//����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterDeviceManageClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)DeviceManageWndProc; // �豸�����ڹ��̺���
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


   hInst = hInstance; // ������ǰ��ʵ�����

   // ��������
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, szTitle, WS_SYSMENU | WS_VISIBLE,
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
//������static LRESULT CALLBACK DeviceManageWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK DeviceManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_NOTIFY: // ֪ͨ��Ϣ
			return DoNotify(hWnd,wParam,lParam);
		case WM_CREATE: // ������Ϣ
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ���Ϣ
//			DoDestroyWindow(hWnd);
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
	CreateListView(hWnd); // ����һ����ʽ��
	return 0;
}

// ********************************************************************
//������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//  IN wParam  - ��Ϣ����
//  IN lParam  - ��Ϣ����
//����ֵ��
//	
//����������Ӧ�ó�����֪ͨ��Ϣ
//����: 
// ********************************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	NMLISTVIEW *pnmvl;
	NMHDR   *hdr=(NMHDR   *)lParam;    

		switch(hdr->code)
		{
			 case LVN_ITEMACTIVATE: // ��Ŀ����
				 DoActiveItem(hWnd,wParam,lParam);
				 break;
		}
		return 0;
}

// ********************************************************************
//������static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
//������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//  IN wParam  - ��Ϣ����
//  IN lParam  - ��Ϣ����
//����ֵ��
//	
//����������Ӧ�ó����� EXN_ACTIVEITEM ֪ͨ��Ϣ
//����: 
// ********************************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMITEMACTIVATE lpnmia;

		lpnmia=(LPNMITEMACTIVATE)lParam;
		switch(lpnmia->iItem)
		{
			case 0: // �ļ��������// �����ļ����Ӧ�ó���
#ifdef INLINE_PROGRAM
				LoadApplication("FileView",NULL); 
#else
				CreateProcess("\\kingmos\\FileView.exe",0,0,0,0,0,0,0,0,0);
#endif
				break;
			case 1: // Ӧ�ó����������Ӧ�ó���������
#ifdef INLINE_PROGRAM
				LoadApplication("ApManage",NULL);
#else
				CreateProcess("\\kingmos\\ApManage.exe",0,0,0,0,0,0,0,0,0);
#endif
				break;
			case 2: // ϵͳ�趨������ϵͳ�趨Ӧ�ó���
#ifdef INLINE_PROGRAM
				LoadApplication("Setting",NULL);
#else
				CreateProcess("\\kingmos\\Setting.exe",0,0,0,0,0,0,0,0,0);
#endif
				break;
		}
		return 0;
}



// ********************************************************************
//������static HWND  CreateListView(HWND  hWnd)
//������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//����ֵ��
//	�ɹ�����ListView�Ĵ��ھ�������򷵻�NULL
//��������������һ��ListView�ؼ�
//����: 
// ********************************************************************
static HWND  CreateListView(HWND  hWnd)
{
	RECT						rt;
	HWND						hListView;
	CTLCOLORSTRUCT stCtlColor;

	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt); // �õ����ڿͻ�����
	// ����LISTVIEW
	hListView=CreateWindow( classLISTCTRL,"",
				WS_VISIBLE|WS_CHILD|LVS_ICON|LVS_SHOWSELALWAYS|LVS_SINGLESEL| WS_VSCROLL | WS_HSCROLL,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_LISTVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	if (hListView == NULL)
	{ // ����ʧ��
		RETAILMSG(1,("CreateListView Failure\r\n"));
		return NULL;
	}



	RETAILMSG(1,("Will Set ListView specing...\r\n"));
	SendMessage(hListView,LVM_SETICONSPACING,0,MAKELONG(20,10)); // ����ͼ����Ŀ֮��ļ��

	RETAILMSG(1,("Will Set ListView Color...\r\n"));
	// ���ÿؼ���ɫ
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR ;//| CLF_TITLECOLOR | CLF_TITLEBKCOLOR;
	stCtlColor.cl_Text = RGB(78,81,78);
//	stCtlColor.cl_TextBk = RGB(247,255,247);
	stCtlColor.cl_TextBk = RGB(255,255,255);
	stCtlColor.cl_Selection = RGB(255,255,255);
	stCtlColor.cl_SelectionBk = RGB(77,166,255);
//	stCtlColor.cl_Title = RGB(78,81,78);
//	stCtlColor.cl_TitleBk = RGB(247,255,247);
	SendMessage(hListView,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

	RETAILMSG(1,("Will LoadImage...\r\n"));
	LoadDeviceMangeImage(hWnd,hListView); // װ���豸����ͼ��
	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView ); // ����LISTVIEW����
	RETAILMSG(1,("Will Insert Data...\r\n"));
	InsertListViewItem(hListView,0, 0,"�ļ�����",0); // ���롰�ļ�������Ŀ
	InsertListViewItem(hListView,1, 0,"Ӧ�ó���",1); // ���롰Ӧ�ó�����Ŀ
	InsertListViewItem(hListView,2, 0,"ϵͳ�趨",2); // ���롰ϵͳ�趨����Ŀ
	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView; // ���ؾ��
}

// ********************************************************************
//������static void	LoadDeviceMangeImage(HWND hWnd,HWND hListView)
//������
//	IN hWnd - FileBrowser�Ĵ��ھ��
//	IN hListView - ListView�Ĵ��ھ��
//����ֵ��
//	��
//������������ListView�ؼ������һ�� ImageList
//����: 
// ********************************************************************
static void	LoadDeviceMangeImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
	HINSTANCE hInstance;
	HIMAGELIST hImageList;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // �õ�ʵ�����


		hImageList = ImageList_Create(32,32,ICL_ICON,8,8); // ����ͼ���б�

		if (hImageList == NULL) //����ImageListʧ��
				return ;
		// �õ��ļ������ͼ��
//		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_FILEMANAGE ), IMAGE_ICON,
//			32, 32, 0 ) ;
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_FILEMANAGE ), IMAGE_ICON,
			32, 32, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ;

		// �õ�Ӧ�ó�������ͼ��
//		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_APPMANAGE ), IMAGE_ICON,
//			32, 32, 0 ) ;
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APPLICATION ), IMAGE_ICON,
			32, 32, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ;

		// �õ�ϵͳ�趨��ͼ��
//		hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( IDI_SYSTEMSETTING ), IMAGE_ICON,
//			32, 32, 0 ) ;
		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_CONTROL ), IMAGE_ICON,
			32, 32, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ;

		SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL, (LPARAM)hImageList ); // ����ͼ���б�LISTVIEW

}

// ********************************************************************
//������static BOOL  CreateListViewColumn( HWND  hListView )
//������
//	IN hListView - ListView�Ĵ��ھ��
//����ֵ��
//	��
//������������ListView�ؼ����������Ŀ
//����: 
// ********************************************************************
static BOOL  CreateListViewColumn( HWND  hListView)
{
	LV_COLUMN		lvcolumn;

	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE; // ��ͼ���ı������
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax =1;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 100;     // width of column.
	if( SendMessage(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn)==-1 ) // ����һ����Ŀ
	{
		return FALSE;
	}
	return TRUE;
}
// ********************************************************************
//������static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage)
//������
//	IN hListView- ListView�Ĵ��ھ��
//  IN iItem  - Ҫ�����Ŀ����Ŀλ��
//  IN itemSub - Ҫ��ӵ���Ŀ������Ŀλ��
//  IN lpItem  - Ҫ�����Ŀ������
//  IN iImage  - Ҫ�����Ŀ��Imageλ��
//����ֵ��
//	���ز�����Ŀ��Index
//������������ListView�ؼ������һ����Ŀ
//����: 
// ********************************************************************
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,
							   TCHAR* lpItem,int iImage)
{
	LV_ITEM				lvitem;
	lvitem.mask			= LVIF_TEXT | LVIF_IMAGE; // �ı���ͼ����Ч
	lvitem.iItem		= iItem;     // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
	lvitem.cchTextMax	= strlen(lpItem ); // �ı�����
	lvitem.iImage		= iImage; // ͼ������

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // ������Ŀ
	
	return iItem;
}
