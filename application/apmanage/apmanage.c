/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����Ӧ�ó�������û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-06-19
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

typedef struct ApManageStruct{
	LPTSTR lpApName;
	LPTSTR lpApTitle;
	UINT   idICON;
} APMANAGEITEM;

#ifdef INLINE_PROGRAM
static APMANAGEITEM ApManageList[] = {
	{"PlanDay","�ճ̼ƻ�",IDI_CALENDAR},
	{"contactbook","��ϵ��",IDI_NAMECARD},
	{"NoteBook","���±�",IDI_NOTEBOOK},
	{"IExplore","IE���",IDI_INTERNET},
	{"Alarm","����",IDI_ALARM},
	{"Mobilephone","�ƶ��绰",IDI_PHONE},
	{"EasyBox","����",IDI_EASYBOX},
	{"calculate","������",IDI_CALCULATOR},
	{"Dict","Ӣ���ֵ�",IDI_DICTIONARY},
	{"MPlayer","MP3����",IDI_MP3},
	{"RussBlk","�﷽��",IDI_RUSSBLOCK},
	{"FlyBat","�ɰ�",IDI_PLANE},
	{"Syncsrv","ͬ������",IDI_SYNCSRV},
	{"midp","J2ME",IDI_J2ME},
};
#else
static APMANAGEITEM ApManageList[] = {
	{"\\kingmos\\PlanDay.exe","�ճ̼ƻ�",IDI_CALENDAR},
	{"\\kingmos\\contactbook.exe","��ϵ��",IDI_NAMECARD},
	{"\\kingmos\\NoteBook.exe","���±�",IDI_NOTEBOOK},
	{"\\kingmos\\IExplore.exe","IE���",IDI_INTERNET},
	{"\\kingmos\\Alarm.exe","����",IDI_ALARM},
	{"\\kingmos\\Mobilephone.exe","�ƶ��绰",IDI_PHONE},
	{"\\kingmos\\EasyBox.exe","����",IDI_EASYBOX},
	{"\\kingmos\\calca.exe","������",IDI_CALCULATOR},
	{"\\kingmos\\Dict.exe","Ӣ���ֵ�",IDI_DICTIONARY},
	{"\\kingmos\\Mp3Player.exe","MP3����",IDI_MP3},
	{"\\kingmos\\RussBlk.exe","�﷽��",IDI_RUSSBLOCK},
	{"\\kingmos\\FlyBat.exe","�ɰ�",IDI_PLANE},
	{"\\kingmos\\Syncsrv.exe","ͬ������",IDI_SYNCSRV},
	{"\\kingmos\\midp.exe","J2ME",IDI_J2ME},
};
#endif

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "Ӧ�ó���";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "ApplicationManage";								// The title bar text

static ATOM RegisterApplicationManageClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK ApplicationManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);


static HWND CreateListView(HWND  hWnd);
static void	LoadApplicationManageImage(HWND hWnd,HWND hListView);
static BOOL CreateListViewColumn( HWND  hListView);
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,LPTSTR lpApName,int iImage);

// ********************************************************************
// ������void WinMain_ApplicationManage(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//    IN hPrevInstance   - ǰһ��Ӧ�ó����ʵ�����
//    IN lpCmdLine   - ����Ӧ�ó���ʱ�Ĳ���
//    IN nCmdShow  - Ӧ�ó�����ʾ����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������װ��Ӧ�ó���
// ����: �� ϵͳ ����
// ********************************************************************
#ifdef INLINE_PROGRAM
int WINAPI WinMain_ApplicationManage(HINSTANCE hInstance,
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

	hWnd = FindWindow( szWindowClass, NULL ) ;  // ����Ӧ�ó����Ƿ����
	if( hWnd != 0 )
	{ // Ӧ�ó����Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���õ���ǰ̨
		return FALSE ;
	}
	InitCommonControls(); // ��ʼ��ͨ�ÿؼ�
	RegisterApplicationManageClass(hInstance); // ע��Ӧ�ó�����

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

	return msg.wParam;
}

// ********************************************************************
// ������ATOM RegisterApplicationManageClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterApplicationManageClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)ApplicationManageWndProc; // Ӧ�ó�����̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance; // ʵ�����
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_APMANAGE),IMAGE_ICON,16,16,0); // Ӧ�ó���ͼ��
//	wc.hIcon			= LoadImage(NULL,MAKEINTRESOURCE(OIC_APPLICATION),IMAGE_ICON,16,16,0);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // Ӧ�ó�������

	return RegisterClass(&wc); // ע��Ӧ�ó�����
}


// ********************************************************************
// ������static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//    IN nCmdShow  - Ӧ�ó�����ʾ����
// ����ֵ��
//	�ɹ�����TRUE, ʧ�ܷ���FALSE
// ������������ʼ����ǰʵ������������ǰ������
// ����: 
// ********************************************************************
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;


   hInst = hInstance; // ������ǰ��ʵ�����
   // ����Ӧ�ó���������
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, szTitle, WS_SYSMENU | WS_VISIBLE,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // ��������ʧ��
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);

   ShowWindow(hWnd, nCmdShow); // ��ʾ����
   UpdateWindow(hWnd); // ���´���

   return TRUE;
}

// ********************************************************************
// ������static LRESULT CALLBACK ApplicationManageWndProc(HWND , UINT , WPARAM , LPARAM )
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
// ����������Ӧ�ó��򴰿ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK ApplicationManageWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
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
		case WM_DESTROY: // �ƻ�����
//			DoDestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}


// ********************************************************************
// ������static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	����0���������������ڣ�����-1������ƻ�����
// ����������Ӧ�ó���������Ϣ
// ����: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CreateListView(hWnd); // ����һ��LISTVIEW
	return 0;
}

// ********************************************************************
// ������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó�����֪ͨ��Ϣ
// ����: 
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
// ������static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó����� EXN_ACTIVEITEM ֪ͨ��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMITEMACTIVATE lpnmia;
	LVITEM	lvItem;
	LPTSTR lpApName;

		lpnmia=(LPNMITEMACTIVATE)lParam; // �õ���Ŀ����Ľṹ����
		
		lvItem.mask  = LVIF_PARAM; // Ҫ��õ���Ŀ����
		lvItem.iItem=lpnmia->iItem; // �õ���Ŀ����
		lvItem.iSubItem=0;
		SendMessage(lpnmia->hdr.hwndFrom,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // �õ���Ŀ����
		lpApName = (LPTSTR)lvItem.lParam; // ��Ӧ�ó�������ƣ����û��ڴ�����ʱ������
		if (lpApName)
		{
/*			if (strcmp(lpApName, ApManageList[9].lpApName) == 0)
			{
				LoadApplication(lpApName,NULL);
				return 0;
			}
*/			
#ifdef INLINE_PROGRAM
			LoadApplication(lpApName,NULL); // ����Ӧ�ó���
#else
			CreateProcess(lpApName,0,0,0,0,0,0,0,0,0); // ����Ӧ�ó���
#endif
		}
		return 0;
}



// ********************************************************************
// ������static HWND  CreateListView(HWND  hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	�ɹ�����ListView�Ĵ��ھ�������򷵻�NULL
// ��������������һ��ListView�ؼ�
// ����: 
// ********************************************************************
static HWND  CreateListView(HWND  hWnd)
{
	RECT						rt;
	HWND						hListView;
	CTLCOLORSTRUCT stCtlColor;
	int nApNumber,i;

	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt); // �õ����ڿͻ�����
	// ����LISTVIEW����
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
	SendMessage(hListView,LVM_SETICONSPACING,0,MAKELONG(20,13)); // ���ü��
	// ������ɫ
	RETAILMSG(1,("Will Set ListView Color...\r\n"));
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
	LoadApplicationManageImage(hWnd,hListView); // װ��Ӧ�ó���ͼ�굽LISTVIEW
	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView ); // ����LISTVIEW��
	RETAILMSG(1,("Will Insert Data...\r\n"));
	// ������Ӧ�ó�����뵽LISTVIEW
	nApNumber = sizeof(ApManageList)/sizeof(APMANAGEITEM);
	for (i=0; i<nApNumber; i++)
	{
		InsertListViewItem(hListView,ApManageList[i].lpApTitle, ApManageList[i].lpApName,i); // ������Ŀ
	}
	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView; // ���ش��ھ��
}

// ********************************************************************
// ������static void	LoadApplicationManageImage(HWND hWnd,HWND hListView)
// ������
//	IN hWnd - FileBrowser�Ĵ��ھ��
//	IN hListView - ListView�Ĵ��ھ��
// ����ֵ��
//	��
// ������������ListView�ؼ������һ�� ImageList
// ����: 
// ********************************************************************
static void	LoadApplicationManageImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
	HINSTANCE hInstance;
	HIMAGELIST hImageList;
	int nApNumber,i;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����


		hImageList = ImageList_Create(32,32,ICL_ICON,8,8); // ����ͼ���б�

		if (hImageList == NULL) //����ImageListʧ��
				return ;
		
		nApNumber = sizeof(ApManageList)/sizeof(APMANAGEITEM); // �õ�Ӧ�ó������
		// װ��Ӧ�ó���ͼ��
		for (i=0; i<nApNumber; i++)
		{
//			hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( ApManageList[i].idICON ), IMAGE_ICON,
			hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( ApManageList[i].idICON ), IMAGE_ICON,
				32, 32, 0 ) ; // װ��Ӧ�ó���ͼ��
			if( hIcon != 0 )
				ImageList_AddIcon( hImageList  , hIcon ) ; // ���뵽ͼ���б�
		}


		SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL, (LPARAM)hImageList ); // ������Ϣ��LISTVIEW

}

// ********************************************************************
// ������static BOOL  CreateListViewColumn( HWND  hListView )
// ������
//	IN hListView - ListView�Ĵ��ھ��
// ����ֵ��
//	��
// ������������ListView�ؼ����������Ŀ
// ����: 
// ********************************************************************
static BOOL  CreateListViewColumn( HWND  hListView)
{
	LV_COLUMN		lvcolumn;

	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE; // ��ͼ����ı������
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax =1;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 100;     // width of column.
	if( SendMessage(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn)==-1 ) // ������
	{
		return FALSE;
	}
	return TRUE;
}
// ********************************************************************
// ������static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage)
// ������
//	IN hListView- ListView�Ĵ��ھ��
//    IN lpApTitle  - Ҫ���Ӧ�ó���ı���
//    IN lpApName   - Ҫ���Ӧ�ó��������
//    IN iImage  - Ҫ�����Ŀ��Imageλ��
// ����ֵ��
//	���ز�����Ŀ��Index
// ������������ListView�ؼ������һ����Ŀ
// ����: 
// ********************************************************************
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,LPTSTR lpApName,int iImage)
{
	LV_ITEM				lvitem;
	int iItem;

	lvitem.mask			= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iItem		= 0xffff;     // the item index,where the item shows. 
	lvitem.iSubItem		= 0;
	lvitem.pszText		= lpApTitle; // ��Ŀ�ı�
	lvitem.cchTextMax	= strlen(lpApTitle ); // �ı�����
	lvitem.iImage		= iImage; // ͼ������
	lvitem.lParam		= (LPARAM)lpApName; // Ӧ�ó�������

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // ������Ŀ
	
	return iItem; // ������Ŀ����
}
