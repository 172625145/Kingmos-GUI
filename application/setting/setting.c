/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ϵͳ�趨��������û����沿��
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

#define ID_SYSTEM		1001
#define ID_DISPLAY		1002
#define ID_EMAILSET		1003
#define ID_IEOPTION		1004
#define ID_STYLUS		1005
#define ID_VOLUME		1006
#define ID_POWER		1007
#define ID_DATETIME		1008
#define ID_GPRSSET		1009
#define ID_NETSETTING	1010

typedef struct SystemSettingStruct{
	DWORD dwIndex;
	LPTSTR lpApTitle;
	UINT   idICON;
} SYSTEMSETTINGITEM;

static SYSTEMSETTINGITEM SystemSettingList[] ={
	{ID_SYSTEM,"ϵͳ",IDI_SYSTEM},
	{ID_DISPLAY,"��ʾ",IDI_DISPLAY},
	{ID_EMAILSET,"�ʼ�����",IDI_EMAILSET},
	{ID_IEOPTION,"IEѡ��",IDI_INTERNET},
	{ID_STYLUS,"����",IDI_STYLUS},
	{ID_GPRSSET,"GPRS",IDI_GPRSSET},
	{ID_POWER,"��Դ",IDI_POWER},
	{ID_DATETIME,"����ʱ��",IDI_DATETIME},
	{ID_VOLUME,"����",IDI_VOLUME},
	{ID_NETSETTING,"����",IDI_NETSET}
};

static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "ϵͳ�趨";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "SystemSetting";								// The title bar text

static ATOM RegisterSystemSettingClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK SystemSettingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);


static HWND CreateListView(HWND  hWnd);
static void	LoadSystemSettingImage(HWND hWnd,HWND hListView);
static BOOL CreateListViewColumn( HWND  hListView);
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,DWORD dwIndex,int iImage);

static void CreateSysProperty(HINSTANCE hInstqance,HWND hWnd,DWORD dwIndex);

// ********************************************************************
extern void DoSystemProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoDisplayProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoEmailProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoIEOption(HINSTANCE hInstance,HWND hWnd);
extern void DoStylusProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoSoundProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoPowerProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoDateTimeProperty(HINSTANCE hInstance,HWND hWnd);
extern void DoGPRSSet(HINSTANCE hInstance,HWND hWnd);
extern void DoNetSetting(HINSTANCE hInstance,HWND hWnd);


// ********************************************************************
// ������void WinMain_SystemSetting(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_SystemSetting(HINSTANCE hInstance,
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

	if ((lpCmdLine != NULL) && strlen(lpCmdLine) != 0)
	{ // �в���
		InitCommonControls(); // ��ʼ��ͨ�ÿؼ�
		if (stricmp(lpCmdLine,"datetime") == 0)
		{ // Ҫ�����С�����/ʱ�䡱����
			CreateSysProperty(hInstance,NULL,ID_DATETIME); // ����������/ʱ�䡱
		}
		if (stricmp(lpCmdLine,"BatteryProperty") == 0)
		{ // Ҫ�����С�������ԡ�
			CreateSysProperty(hInstance,NULL,ID_POWER); // ������������ԡ�
		}			
		if (stricmp(lpCmdLine,"EmailProperty") == 0)
		{ // Ҫ�����С��ʼ����ԡ�
			CreateSysProperty(hInstance,NULL,ID_EMAILSET); // �����ʼ�����
		}			
		return 1; // ��Ҫ������ϵͳ����������
	}
	hWnd = FindWindow( szWindowClass, NULL ) ;  // ϵͳ�����Ƿ��Ѿ�����
	if( hWnd != 0 )
	{ // �Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���õ�ǰ̨
		return FALSE ;
	}
	RegisterSystemSettingClass(hInstance); // ע��ϵͳ���ô�����

	InitCommonControls(); // ��ʼ��ͨ�ÿؼ�
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
// ������ATOM RegisterSystemSettingClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterSystemSettingClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)SystemSettingWndProc; // ���ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_SETTING),IMAGE_ICON,16,16,0); // Ӧ�ó���ͼ��
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH); // ����ˢ
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // ��������

	return RegisterClass(&wc); // ע�ᴰ����
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
	// ����������
   hWnd = CreateWindowEx(WS_EX_CLOSEBOX|WS_EX_NOMOVE|WS_EX_TITLE|WS_EX_HELPBOX,szWindowClass, szTitle, WS_SYSMENU | WS_VISIBLE,
      30, 0, 210, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   { // ����ʧ��
      return FALSE;
   }


   ShowWindow(hWnd, nCmdShow); // ��ʾ����
   UpdateWindow(hWnd); // ���´���

   return TRUE;
}

// ********************************************************************
// ������static LRESULT CALLBACK SystemSettingWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK SystemSettingWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_DESTROY: // �ƻ���Ϣ
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
	CreateListView(hWnd); // ����LISTVIEW
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
	DWORD dwIndex;
	HINSTANCE hInstance;

		lpnmia=(LPNMITEMACTIVATE)lParam; // �õ���Ŀ����Ľṹ
		
		lvItem.mask  = LVIF_PARAM; // Ҫ��õ���Ŀ����
		lvItem.iItem=lpnmia->iItem; // �õ��������Ŀ
		lvItem.iSubItem=0;
		SendMessage(lpnmia->hdr.hwndFrom,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // �õ���Ŀ����
		dwIndex = lvItem.lParam; // �õ����Ե���������Ŀ������������������������Ŀʱ����
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		CreateSysProperty(hInstance,hWnd,dwIndex); // ����ϵͳ���Դ���
		return 0;
}


// **************************************************
// ������static void CreateSysProperty(HINSTANCE hInstance,HWND hWnd,DWORD dwIndex)
// ������
// 	IN hInstance -- ʵ�����
// 	IN hWnd -- Ҫ���������Եĸ����ھ��
// 	IN dwIndex -- ��������
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static void CreateSysProperty(HINSTANCE hInstance,HWND hWnd,DWORD dwIndex)
{
		switch (dwIndex)
		{
			case ID_SYSTEM: // ����ϵͳ����
				DoSystemProperty(hInstance,hWnd);
				break;
			case ID_DISPLAY: // ������ʾ����
				DoDisplayProperty(hInstance,hWnd);
				break;
			case ID_EMAILSET: // �����ʼ���������
				DoEmailProperty(hInstance,hWnd);
				break;
			case ID_IEOPTION: // ����IEѡ��
				DoIEOption(hInstance,hWnd);
				break;
			case ID_STYLUS: // �����������
				DoStylusProperty(hInstance,hWnd);
				break;
			case ID_VOLUME: // ������������
				DoSoundProperty(hInstance,hWnd);
				break;
			case ID_POWER: // ������Դ����
				DoPowerProperty(hInstance,hWnd);
				break;
			case ID_DATETIME: // ��������ʱ������
				DoDateTimeProperty(hInstance,hWnd);
				break;
			case ID_GPRSSET: // ����GPRS����
				DoGPRSSet(hInstance,hWnd);
				break;
			case ID_NETSETTING: // ����������������
				DoNetSetting(hInstance,hWnd);
				break;
		}
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

//	RETAILMSG(1,("CreateListView ...\r\n"));
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



//	RETAILMSG(1,("Will Set ListView specing...\r\n"));
	SendMessage(hListView,LVM_SETICONSPACING,0,MAKELONG(20,13)); // ���ü��
	// ������ɫ
//	RETAILMSG(1,("Will Set ListView Color...\r\n"));
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR ;//| CLF_TITLECOLOR | CLF_TITLEBKCOLOR;
	stCtlColor.cl_Text = RGB(78,81,78);
//	stCtlColor.cl_TextBk = RGB(247,255,247);
	stCtlColor.cl_TextBk = RGB(255,255,255);
	stCtlColor.cl_Selection = RGB(255,255,255);
	stCtlColor.cl_SelectionBk = RGB(77,166,255);
//	stCtlColor.cl_Title = RGB(78,81,78);
//	stCtlColor.cl_TitleBk = RGB(247,255,247);
	SendMessage(hListView,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);

//	RETAILMSG(1,("Will LoadImage...\r\n"));
	LoadSystemSettingImage(hWnd,hListView); // װ��ϵͳ����ͼ���б�
//	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView ); // ����LISTVIEW������
//	RETAILMSG(1,("Will Insert Data...\r\n"));

	nApNumber = sizeof(SystemSettingList)/sizeof(SYSTEMSETTINGITEM); // �õ�Ҫ�������Ŀ����
	for (i=0; i<nApNumber; i++)
	{
		InsertListViewItem(hListView,SystemSettingList[i].lpApTitle, SystemSettingList[i].dwIndex,i); // ������Ŀ
	}
//	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView;
}

// ********************************************************************
// ������static void	LoadSystemSettingImage(HWND hWnd,HWND hListView)
// ������
//	IN hWnd - FileBrowser�Ĵ��ھ��
//	IN hListView - ListView�Ĵ��ھ��
// ����ֵ��
//	��
// ������������ListView�ؼ������һ�� ImageList
// ����: 
// ********************************************************************
static void	LoadSystemSettingImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
	HINSTANCE hInstance;
	HIMAGELIST hImageList;
	int nApNumber,i;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����


		hImageList = ImageList_Create(32,32,ICL_ICON,8,8); // ����ͼ���б�

		if (hImageList == NULL) //����ImageListʧ��
				return ;
		
		nApNumber = sizeof(SystemSettingList)/sizeof(SYSTEMSETTINGITEM); // �õ���Ŀ����
		for (i=0; i<nApNumber; i++)
		{	// װ��ͼ��
			hIcon = (HICON)LoadImage( hInstance, MAKEINTRESOURCE( SystemSettingList[i].idICON ), IMAGE_ICON,
				32, 32, 0 ) ;
			//if( hIcon != 0 )
			ImageList_AddIcon( hImageList  , hIcon ) ; // ����ͼ�굽ͼ���б�
		}


		SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_NORMAL, (LPARAM)hImageList ); // ����ͼ���б�LISTVIEW

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
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE; // Ҫ����ͼ����ı������
	lvcolumn.pszText = "";
	lvcolumn.cchTextMax =1;
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 100;     // width of column.
	if( SendMessage(hListView,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn)==-1 ) // ����һ��
	{ // ������ʧ��
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
static int InsertListViewItem(HWND hListView,LPTSTR lpApTitle,DWORD dwIndex,int iImage)
{
	LV_ITEM				lvitem;
	int iItem;

	lvitem.mask			= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lvitem.iItem		= 0xffff;     // the item index,where the item shows. 
	lvitem.iSubItem		= 0;
	lvitem.pszText		= lpApTitle; // ������Ŀ�ı�
	lvitem.cchTextMax	= strlen(lpApTitle );
	lvitem.iImage		= iImage; // ������Ŀͼ������
	lvitem.lParam		= dwIndex; // ������������

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // ������Ŀ
	
	return iItem; // ������Ŀ����
}


// **************************************************
// ������BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// ������
// 	IN hRoot -- ����
// 	IN lpKey -- ����
// 	IN lpValueName -- ��ֵ
// 	OUT lpData -- ���ݻ���
// 	IN dwDataLen -- ���ݻ���Ĵ�С
// 	IN dwType -- ��������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ȡָ���ļ�ֵ��ֵ��
// ����: 
// **************************************************
BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegOpenKeyEx(hRoot, lpKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) // ��ָ���ļ�
	 { // �򿪳ɹ�
		 if (RegQueryValueEx(hKey, lpValueName, NULL, &dwType, (LPBYTE)lpData, &dwDataLen) == ERROR_SUCCESS) // �õ���ֵ��ֵ
		 { // �õ����ݳɹ�
			RegCloseKey(hKey); // �رռ�
			return TRUE;
		 }
		 RegCloseKey(hKey); // �رռ�
	 }
	 return FALSE;
}

// **************************************************
// ������BOOL regReadData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
// ������
// 	IN hRoot -- ����
// 	IN lpKey -- ����
// 	IN lpValueName -- ��ֵ
// 	IN lpData -- ���ݻ���
// 	IN dwDataLen -- ���ݻ���Ĵ�С
// 	IN dwType -- ��������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ȡָ���ļ�ֵ��ֵ��
// ����: 
// **************************************************
BOOL regWriteData(HKEY hRoot,LPTSTR lpKey, LPTSTR lpValueName,void *lpData, DWORD dwDataLen,DWORD dwType)
{
	HKEY hKey;

	 if (RegCreateKeyEx(hRoot, lpKey, 0,NULL,0, KEY_ALL_ACCESS, NULL,&hKey,0) == ERROR_SUCCESS) // ��ָ���ļ�
	 { // �򿪳ɹ�
		 if (RegSetValueEx(hKey, lpValueName, NULL, (DWORD)dwType, (LPBYTE)lpData, dwDataLen) == ERROR_SUCCESS) // ����ָ����ֵ��ֵ
		 { // ���óɹ�
			 RegCloseKey(hKey); // �رռ�ֵ
			 return TRUE;
		 }
		 RegCloseKey(hKey); // �رռ�ֵ
	 }
	 return FALSE;
}
