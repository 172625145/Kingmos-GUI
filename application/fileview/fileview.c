/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�����ļ���������û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-02
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include <Etoolbar.h>
#include "resource.h"
#include "Explorer.h"
#include "FindData.h"
#include "eCommdlg.h"
#include "eclipbrd.h"
#include <emmsys.h>

#include <eShell.h>

/***************  ȫ���� ���壬 ���� *****************/
#define MAX_LOADSTRING  100
#define MAX_SEARCHSTRINGLEN	64

#define ID_BROWSER  301
#define ID_TOOLBAR	302

#define IDC_NEWFOLDER	601
#define IDC_COPY		602
#define IDC_PASTE		603
#define IDC_DELETE		604
#define IDC_UP			605
#define IDC_RENAME		606
#define IDC_FIND		607
#define IDC_PROPERTY	608


static HINSTANCE hInst;								// current instance
static TCHAR szTitle[MAX_LOADSTRING] = "�ļ�����";								// The title bar text
static TCHAR szWindowClass[MAX_LOADSTRING] = "FileView";								// The title bar text

typedef struct OpenListStruct{
	LPTSTR lpExtend;
	LPTSTR lpApplication;
} OPENLIST;

typedef OPENLIST * LPOPENLIST;

#ifdef INLINE_PROGRAM
static OPENLIST g_OpenList[] =
{
	{"*.txt",	"NoteBook"},
	{"*.htm",	"IExplore"},
	{"*.bmp",	"NoteBook"},
	{"*.gif",	"IExplore"},
	{"*.mp3",	"MPlayer"},
	{"*.pls",	"MPlayer"},
	{"*.mlg",	"Readbox"},
	{NULL,NULL}
};
#else
static OPENLIST g_OpenList[] =
{
	{"*.txt",	"\\kingmos\\NoteBook.exe"},
	{"*.htm",	"\\kingmos\\IExplore.exe"},
	{"*.bmp",	"\\kingmos\\NoteBook.exe"},
	{"*.gif",	"\\kingmos\\IExplore.exe"},
	{"*.mp3",	"\\kingmos\\Mp3Player.exe"},
	{"*.pls",	"\\kingmos\\Mp3Player.exe"},
	{"*.mlg",	"\\kingmos\\Readbox.exe"},
	{NULL,NULL}
};
#endif

extern ATOM RegisterFileBrowserClass(HINSTANCE hInstance);
extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;


ATOM RegisterFileViewClass(HINSTANCE hInstance);
static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK FileViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL CreateFileViewToolbar( HWND hWnd );
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT SetToolBarButtonEnable(HWND hWnd);
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCopy(HWND hWnd);
static LRESULT DoPaste(HWND hWnd);
static LRESULT DoUpPath(HWND hWnd);
static LRESULT DoDelete(HWND hWnd);
static LRESULT DoRename(HWND hWnd);
static LRESULT DoNewFolder(HWND hWnd);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFind(HWND hWnd);
static LRESULT DoProperty(HWND hWnd);
static LRESULT DoShellNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);


static LRESULT CALLBACK FindDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitFindDlg(HWND hDlg,WPARAM wParam,LPARAM lParam);
static LRESULT FindString(HWND hDlg);

static void ShowFileProperty(HWND hWnd,LPFILEPROPERTY lpFileProperty);
static LRESULT CALLBACK ShowPropertyDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT DoInitShowProperty(HWND hDlg,WPARAM wParam,LPARAM lParam);
static LRESULT DoPaintShowProperty(HWND hDlg,HDC hdc);


// ********************************************************************
// ������void WinMain_FileView(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR   lpCmdLine,int       nCmdShow)
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
int WINAPI WinMain_FileView(HINSTANCE hInstance,
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

// !!! test
//	SHChangeNotify(SHCNE_UPDATEDIR, 0, 0, 0);
// !!! test end

	hWnd = FindWindow( szWindowClass, NULL ) ;  // ���Ҵ����Ƿ����
	if( hWnd != 0 )
	{ // �����Ѿ�����
		SetForegroundWindow( hWnd ) ; // ���ô��ڵ�ǰ̨
		return FALSE ;
	}
	InitCommonControls(); // ��ʼ��ͨ�ÿؼ�
	RegisterFileBrowserClass(hInstance); // ע���ļ������
	RegisterFileViewClass(hInstance) ; // ע��Ӧ�ó�����

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
// ������ATOM RegisterFileViewClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterFileViewClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)FileViewWndProc; // �ļ�������ڹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance; // ʵ�����
	wc.hIcon			= LoadImage(hInstance,MAKEINTRESOURCE(IDI_FILEMANAGE),IMAGE_ICON,16,16,0); // Ӧ�ó���λͼ
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass; // Ӧ�ó�������

	return RegisterClass(&wc); // ע����
}


// ********************************************************************
// ������static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
//  IN nCmdShow  - Ӧ�ó�����ʾ����
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
   { // ����ʧ��
      return FALSE;
   }

//   CreateWindow(classSETDATE,"",WS_CHILD|WS_VISIBLE,10,10,
//		100,100,hWnd,NULL,hInst,NULL);

   ShowWindow(hWnd, nCmdShow); // ��ʾ����
   UpdateWindow(hWnd); // ���´���

   return TRUE;
}

// ********************************************************************
// ������static LRESULT CALLBACK FileViewWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK FileViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
				case IDC_NEWFOLDER: // ����һ���µ�Ŀ¼
					DoNewFolder(hWnd);
					break;
				case IDC_COPY: // ����
					DoCopy(hWnd);
					break;
				case IDC_PASTE: // ճ��
					DoPaste(hWnd);
					break;
				case IDC_UP: // ����һ��Ŀ¼
					DoUpPath(hWnd);
					break;
				case IDC_DELETE: // ɾ��
					DoDelete(hWnd);
					break;
				case IDC_RENAME: // ������
					DoRename(hWnd);
					break;
				case IDC_FIND: // ����
					DoFind(hWnd);
					break;
				case IDC_PROPERTY: // ����
					DoProperty(hWnd);
					break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_KEYDOWN: // ������Ϣ
//			DoKeyDown(hWnd,wParam,lParam);
			break;
		case WM_LBUTTONDOWN: // ��������Ϣ
//			DoLButtonDown(hWnd,wParam,lParam);
			break;
		case WM_NOTIFY: // ֪ͨ��Ϣ
			return DoNotify(hWnd,wParam,lParam);
		case WM_SHELLNOTIFY: // SHELL֪ͨ��Ϣ
			return DoShellNotify(hWnd,wParam,lParam);
		case WM_CREATE: // ��������
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_DESTROY: // �ƻ�����
			DoDestroyWindow(hWnd);
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
//  IN wParam  - ��Ϣ����
//  IN lParam  - ��Ϣ����
// ����ֵ��
//	����0���������������ڣ�����-1������ƻ�����
// ����������Ӧ�ó���������Ϣ
// ����: 
// ********************************************************************
static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	CreateFileViewToolbar(hWnd); // ����������
	// �����ļ������
	CreateWindow(classFileBrowser,"",WS_CHILD|WS_VISIBLE,0,TOOLBAR_HIGH,210,255,hWnd,(HMENU)ID_BROWSER,hInst,NULL);
//	CreateWindow(classEDIT,"test abcd",WS_CHILD|WS_VISIBLE|ES_MULTILINE,0,TOOLBAR_HIGH,210,255,hWnd,(HMENU)ID_BROWSER,hInst,NULL);
	return 0;
}

// ********************************************************************
// ������static BOOL CreateFileViewToolbar( HWND hWnd )
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	�ɹ� ����TRUE , ���򷵻�FALSE
// ����������Ӧ�ó��򴴽�������
// ����: 
// ********************************************************************
static BOOL CreateFileViewToolbar( HWND hWnd )
{
//	_LPIEDATA	lpied ;
	HWND		hToolbar ;
	TBBUTTON	tb;
	int			i;

typedef struct FileViewToolBar{
	BOOL bSysBmp;
	UINT ID;
	UINT IDResource;
	UINT IDDisRes;
	LPTSTR lpString;
}FILEVIEWTOOLBAR;


	FILEVIEWTOOLBAR FileViewToolBar[]={
		{TRUE,	IDC_NEWFOLDER,	OBM_TOOL_NEW,		0,						"NewFolder"},
		{TRUE,	IDC_COPY,		OBM_TOOL_COPY,		OBM_TOOL_COPY_GRAY,		"Copy"},
		{TRUE,	IDC_PASTE,		OBM_TOOL_PASTE,		OBM_TOOL_PASTE_GRAY,	"Paste"},
		{TRUE,	IDC_DELETE,	OBM_TOOL_DELETE,	OBM_TOOL_DELETE_GRAY,	"Delete"},
		{TRUE,	IDC_UP,		OBM_TOOL_UPFOLDER,	OBM_TOOL_UPFOLDER_GRAY,	"Up"},
		{TRUE,	IDC_RENAME,	OBM_TOOL_RENAME,	OBM_TOOL_RENAME_GRAY,	"Rename"},
		{TRUE,	IDC_FIND,		OBM_TOOL_FIND,		0,						"Find"},
		{FALSE,	IDC_PROPERTY,	IDB_PROPERTY,		0,						"Property"},
	};
	HIMAGELIST hImageList;
	HIMAGELIST hImageList_Disable;
	HBITMAP hBitmap;
	int iToolBarNum ;

//	lpied = GetIEPtr( hWnd ) ;
	// ����������
	hToolbar = CreateToolbarEx( hWnd, WS_VISIBLE|WS_CHILD|TBSTYLE_FLAT|TBSTYLE_LIST|CCS_TOP,
			ID_TOOLBAR, 0, NULL, 0, NULL,0,20,20,16,16,0 );
	if( hToolbar == 0 )
	{ // ����ʧ��
		EdbgOutputDebugString("=========Create ToolBar is Failure===========\r\n");
		return 0;
	}
	iToolBarNum = sizeof(FileViewToolBar) / sizeof(FILEVIEWTOOLBAR); // �õ�������Ŀ�ĸ���
	hImageList=ImageList_Create(16,16,ILC_COLOR8,10,10); // ����ͼ����
	hImageList_Disable=ImageList_Create(16,16,ILC_COLOR8,10,10); // ������ɫͼ����
	for (i=0;i<iToolBarNum;i++)
	{ // �������ͼ��ͼ����
		if (FileViewToolBar[i].bSysBmp == TRUE)
			hBitmap = LoadImage( NULL, MAKEINTRESOURCE(FileViewToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��ϵͳ��Դ
		else
			hBitmap = LoadImage( hInst, MAKEINTRESOURCE(FileViewToolBar[i].IDResource), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��Ӧ�ó�����Դ
		if (hBitmap )
			ImageList_Add(hImageList,hBitmap,NULL); // ������
		if (FileViewToolBar[i].IDDisRes != 0)
		{
			if (FileViewToolBar[i].bSysBmp == TRUE)
				hBitmap = LoadImage( NULL, MAKEINTRESOURCE(FileViewToolBar[i].IDDisRes), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // װ��ϵͳ��Դ
			else
				hBitmap = LoadImage( hInst, MAKEINTRESOURCE(FileViewToolBar[i].IDDisRes), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE ) ; // ת��Ӧ�ó�����Դ
		}
		else
			hBitmap = NULL;
		ImageList_Add(hImageList_Disable,hBitmap,NULL); // ������
	}

//	SendMessage( hToolbar, TB_LOADIMAGES , IDB_STD_SMALL_COLOR , ( LPARAM )HINST_COMMCTRL ) ;
	hImageList = (HIMAGELIST)SendMessage( hToolbar, TB_SETIMAGELIST , 0, ( LPARAM )hImageList) ; // ��ͼ�������õ�������
	hImageList_Disable = (HIMAGELIST)SendMessage( hToolbar, TB_SETDISABLEDIMAGELIST , 0, ( LPARAM )hImageList_Disable) ; // ��DISABLEͼ�������õ�������
	
	// ɾ��ԭ����ImageList
	if (hImageList)
		ImageList_Destroy(hImageList);
	if (hImageList_Disable)
		ImageList_Destroy(hImageList_Disable);

	tb.iString = 0 ;
	tb.fsStyle = TBSTYLE_BUTTON ;
	for( i = 0; i < iToolBarNum; i ++ )
	{ // ����ÿ�����߰�ť
		if (FileViewToolBar[i].IDDisRes != 0)
			tb.fsState = 0 ; // ��ťDISABLE
		else 
			tb.fsState = TBSTATE_ENABLED  ; // ��ťENABLE
		tb.iBitmap = i; // ͼ������
		tb.idCommand = FileViewToolBar[i].ID; // ��ť������
		SendMessage( hToolbar, TB_ADDBUTTONS, 1, (LPARAM)&tb ); // ���밴ť
	}
	
	if (IsClipboardFormatAvailable(CF_FILE)) // �жϼ��а������Ƿ���Ч
		SendMessage(hToolbar,TB_ENABLEBUTTON,IDC_PASTE,TRUE); // ճ����EANBLE
	else
		SendMessage(hToolbar,TB_ENABLEBUTTON,IDC_PASTE,FALSE); // ճ����DISABLE

	return TRUE;
}
// ********************************************************************
// ������static LRESULT SetToolBarButtonEnable(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó������ù������ļ���״̬
// ����: 
// ********************************************************************
static LRESULT SetToolBarButtonEnable(HWND hWnd)
{
	UINT iSelCount;
	HWND hToolBar;
	HWND hListView;
	HWND hFileBrowser;
	TCHAR lpCurPath[MAX_PATH];
	BOOL bSearch;

		hToolBar = GetDlgItem(hWnd,ID_TOOLBAR); // �õ��������Ĵ��ھ��
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�������ڵĴ��ھ��
		hListView = (HWND)SendMessage(hFileBrowser,EXM_GETLISTVIEW,0,0); // �õ�LISTVIEW�Ĵ��ھ��
		iSelCount = SendMessage(hListView,LVM_GETSELECTEDCOUNT,0,0); // �õ�ѡ����Ŀ

		if (iSelCount == 0)
		{ // û��ѡ��
			// Set Copy , Delete , rename disable
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_COPY,FALSE);
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_DELETE,FALSE);
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_RENAME,FALSE);
		}
		else 
		{ // ��ѡ��
			// Set Delete enable
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_DELETE,TRUE);
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_COPY,TRUE);
			if (iSelCount == 1)
			{
				// Set Copy , rename enable
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_RENAME,TRUE);
			}
			else
			{
				// Set Copy , rename disable
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_RENAME,FALSE);
			}
		}
		bSearch = SendMessage(hFileBrowser,EXM_ISFIND,0,0); // ��ǰ�Ƿ��ǲ�Ѱ����
		if (bSearch == FALSE)
		{ // ���ǲ�Ѱ����
			SendMessage(hFileBrowser,EXM_GETCURPATH,MAX_PATH,(LPARAM)lpCurPath);
			if (strcmp(lpCurPath,"\\") == 0 || strlen(lpCurPath) == 0)
			{// �Ǹ�Ŀ¼
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_UP,FALSE);
			}
			else
			{ // ���Ǹ�Ŀ¼
				SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_UP,TRUE);
			}
		}
		else
		{ // �ǲ�Ѱ����
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_UP,TRUE);
		}

		if (IsClipboardFormatAvailable(CF_FILE))// ���а������Ƿ���Ч
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,TRUE);
		else
			SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,FALSE);

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
			 case EXN_SELCHANGED: // ѡ����Ŀ�ı�
				 DoItemChanged(hWnd,wParam,lParam);
				 break;
			 case EXN_DATACHANGED: // ���ݸı�
				 DoDataChanged(hWnd,wParam,lParam);
				 break;
			 case EXN_ACTIVEITEM: // ��Ŀ����
				 DoActiveItem(hWnd,wParam,lParam);
				 break;
		}
		return 0;
}
// ********************************************************************
// ������static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó����� EXN_SELCHANGED ֪ͨ��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{ // ��Ŀѡ��ı�
		SetToolBarButtonEnable(hWnd); // �������ù������İ�ť״̬
		return 0;
}
// ********************************************************************
// ������static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó����� EXN_DATACHANGED ֪ͨ��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{ // ���ݸı�
		SetToolBarButtonEnable(hWnd);// �������ù������İ�ť״̬
		return 0;
}

// ********************************************************************
// ������static LRESULT DoCopy(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_COPY ����
// ����: 
// ********************************************************************
static LRESULT DoCopy(HWND hWnd)
{
	HWND hFileBrowser;
	HWND hToolBar;


		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�������ڵľ��
		SendMessage(hFileBrowser,WM_COPY,0,0); // ���͸�������


		hToolBar = GetDlgItem(hWnd,ID_TOOLBAR); // �õ��������Ĵ��ھ��

		SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,TRUE); // ʹ��ճ������ť��Ч
		return 0;
/*	HWND hListView;
	int iIndex ;
	LVITEM lvItem;
	HWND hFileBrowser;
	char lpFileName[MAX_PATH];
	HWND hToolBar;
	LPFILECOPYDATA lpFileCopyData = NULL;

		hToolBar = GetDlgItem(hWnd,ID_TOOLBAR);

		SendMessage(hToolBar,TB_ENABLEBUTTON,IDC_PASTE,TRUE);

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER);
		hListView = SendMessage(hFileBrowser,EXM_GETLISTVIEW,0,0);

		iIndex = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);

		while(1)
		{
			if (iIndex == -1)
				break;
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iIndex;
			lvItem.iSubItem = 0;
			lvItem.pszText = lpFileName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView, LVM_GETITEM, 0, &lvItem);
			iIndex = SendMessage(hListView, LVM_GETNEXTITEM, iIndex+1, LVNI_SELECTED);
		}

		return 0;
*/
}
// ********************************************************************
// ������static LRESULT DoPaste(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_PASTE ����
// ����: 
// ********************************************************************
static LRESULT DoPaste(HWND hWnd)
{
	HWND hFileBrowser;


		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�������ڵľ��
		SendMessage(hFileBrowser,WM_PASTE,0,0); // ����ճ������
		return 0;
}
// ********************************************************************
// ������static LRESULT DoUpPath(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_UP ����
// ����: 
// ********************************************************************
static LRESULT DoUpPath(HWND hWnd)
{
	HWND hFileBrowser;

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ���¶���ڵľ��
		SendMessage(hFileBrowser,EXM_UPPATH,0,0);	 // ���͡�����һ��Ŀ¼��������
		return 0;
}
// ********************************************************************
// ������static LRESULT DoDelete(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_DELETE ����
// ����: 
// ********************************************************************
static LRESULT DoDelete(HWND hWnd)
{
	HWND hFileBrowser;

		if (MessageBox(hWnd,"  ȷʵҪɾ����?  ","�ļ�����",MB_YESNO) == IDYES) // ѯ���Ƿ�Ҫɾ���ļ�
		{ // Ҫɾ���ļ�
			hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����Ĵ��ھ��
			SendMessage(hFileBrowser,EXM_DELETESEL,0,0); // ����ɾ��ѡ�������
		}
		SetToolBarButtonEnable(hWnd); // ���蹤������ť
		return 0;
}
// ********************************************************************
// ������static LRESULT DoRename(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_RENAME ����
// ����: 
// ********************************************************************
static LRESULT DoRename(HWND hWnd)
{
	HWND hFileBrowser;
	HWND hListView;
	int iIndex;

		RETAILMSG(1,(" Will Rename File \r\n"));
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����Ĵ��ھ��
//		SendMessage(hFileBrowser,EXM_DELETESEL,0,0);	
		RETAILMSG(1,(" Get FileBrowser hwnd = %x \r\n",hFileBrowser));
		hListView = (HWND)SendMessage(hFileBrowser,EXM_GETLISTVIEW,0,0); // �õ�LISTVIEW�Ĵ��ھ��

		RETAILMSG(1,(" Get ListView hwnd = %x \r\n",hListView));
		
		iIndex = SendMessage(hListView,LVM_GETSELECTIONMARK,0,0); // �õ���ǰѡ��
		if (iIndex == -1)
			return FALSE;
		RETAILMSG(1,(" Send EDITLABEL to ListView  %d \r\n",iIndex));
		SendMessage(hListView,LVM_EDITLABEL,iIndex,0); // �༭��ǩ
		return TRUE;
}
// ********************************************************************
// ������static LRESULT DoFind(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_FIND ����
// ����: 
// ********************************************************************

static LRESULT DoFind(HWND hWnd)
{
	HWND hFileBrowser;
//	int iIndex;
	HANDLE hInst;
	LPTSTR lpSearchString;

		lpSearchString = (LPTSTR)malloc(MAX_SEARCHSTRINGLEN); // ����Ҫ��Ѱ���ַ����Ļ���
		if (lpSearchString == NULL)
			return FALSE;
		hInst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		if (DialogBoxIndirectParamEx( hInst, (LPDLG_TEMPLATE_EX)&dlgFindTemplate, hWnd, FindDialogFunc, (LPARAM)lpSearchString) == FALSE) // �����Ի���Ҫ���û������Ѱ�ִ�
			return TRUE;
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����Ĵ��ھ��
//		SendMessage(hFileBrowser,EXM_FINDFILE,0,"*.bmp;*.cpp");
		SendMessage(hFileBrowser,EXM_FINDFILE,(WPARAM)NULL,(LPARAM)lpSearchString); // �����ַ���
		free(lpSearchString); // �ͷŷ���Ŀռ�
		return TRUE;
}
// ********************************************************************
// ������static LRESULT DoProperty(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_PROPERTY ����
// ����: 
// ********************************************************************
static LRESULT DoProperty(HWND hWnd)
{
	HWND hFileBrowser;
//	HWND hListView;
//	int iIndex;
	FILEPROPERTY stProperty;

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����Ĵ��ھ��
		if (SendMessage(hFileBrowser,EXM_GETPROPERTY,0,(LPARAM)&stProperty) == TRUE) // ���͵õ����Ե�����
		{ // �õ��ɹ�
			RETAILMSG(1,(" Get Property OK!!!\r\n"));
			ShowFileProperty(hWnd,&stProperty); // ��ʾ�õ�������
		}
		else
		{
			RETAILMSG(1,(" Get Property Failure!!!\r\n"));
		}
		return TRUE;
}

// ********************************************************************
// ������static LRESULT DoNewFolder(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó����� IDC_NEWFOLDER ����
// ����: 
// ********************************************************************
static LRESULT DoNewFolder(HWND hWnd)
{
	HWND hFileBrowser;
//	int iIndex;

		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����Ĵ��ھ��
		SendMessage(hFileBrowser,EXM_NEWFOLDER,0,0); // ���ʹ�����Ŀ¼������
		return TRUE;
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
//	short iItem,iFileNo;
//	HWND hListWnd;
//	LVITEM	lvItem;
//	LPNMITEMACTIVATE lpnmia;
	char lpFileName[MAX_PATH];//,lpFullName[MAX_PATH];
//	LPBROWSERDATA lpBrowser;
	HWND  hBrowser;
	UINT iIndex;

	/*
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);
		lpnmia=(LPNMITEMACTIVATE)lParam;
		
		hListWnd=lpnmia->hdr.hwndFrom;
		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=lpnmia->iItem;
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
//		SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)iItem,(LPARAM)&lvItem);
		SendMessage(hListWnd,LVM_GETITEM,(WPARAM)iItem,(LPARAM)&lvItem);

		if (lpBrowser->bSearch == TRUE)
		{
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=lpnmia->iItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)iItem,(LPARAM)&lvItem);
			//_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
//			strcat(lpFullName,"\\");
//			strcat(lpFullName,lpFileName);
			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}

		}
		else
		{
			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
		}

		if (lvItem.iImage == TYPE_FOLDER)
		{
			HWND hListView ;

				strcpy(lpBrowser->lpCurDir,lpFullName);
				hListView = GetDlgItem(hWnd,ID_LISTVIEW);
				UpdateListView( hListView );
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);
		}
		else if (lvItem.iImage == TYPE_TEXTFILE )
		{
		}
		else if (lvItem.iImage == TYPE_BMPFILE )
		{
		}
		else
		{
			if (StrAsteriskCmp("*.htm",lpFileName) == 0)
			{
				LoadApplication("IExplore",lpFullName);
			}
		}
*/
		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����Ĵ��ھ��
		SendMessage(hBrowser,EXM_GETSELFILENAME,MAX_PATH,(LPARAM)lpFileName); // �õ���ǰ��ѡ������Ӧ���ļ���
//		if (StrAsteriskCmp("*.htm",lpFileName) == 0)
/*
		if (FileNameCompare( "*.htm", strlen("*.htm"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("IExplore",lpFileName);
		}
		else if (FileNameCompare( "*.wav" , strlen( "*.wav" ), lpFileName, strlen(lpFileName)) == TRUE)
		{
//#ifndef EML_WIN32
			sndPlaySound(lpFileName,0);
//#endif
		}
		else if (FileNameCompare( "*.txt", strlen("*.txt"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		else if (FileNameCompare( "*.bmp", strlen("*.bmp"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		else if (FileNameCompare( "*.gif", strlen("*.gif"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		else if (FileNameCompare( "*.mp3", strlen("*.mp3"), lpFileName, strlen(lpFileName)) == TRUE)
		{
			LoadApplication("NoteBook",lpFileName);
		}
		*/
		if (FileNameCompare( "*.wav" , strlen( "*.wav" ), lpFileName, strlen(lpFileName)) == TRUE)
		{ // �Ƿ��������ļ�
//#ifndef EML_WIN32
//			sndPlaySound(lpFileName,SND_ASYNC); // ���Ÿ������ļ�
//#endif
			return 0;
		}
		if (FileNameCompare( "*.exe", strlen("*.exe"), lpFileName, strlen(lpFileName)) == TRUE)
		{ // �ǿ�ִ���ļ�
#ifndef INLINE_PROGRAM
			CreateProcess(lpFileName,0,0,0,0,0,0,0,0,0); // ִ�и��ļ�
#endif
			return 0;
		}
		iIndex = 0;
		while(1)
		{
			if (FileNameCompare( g_OpenList[iIndex].lpExtend, strlen(g_OpenList[iIndex].lpExtend), lpFileName, strlen(lpFileName)) == TRUE)
			{ // ����Ƕ�Ӧ�Ѿ�֪�����ļ�
				RETAILMSG(1,("Will Run %s (%s)\r\n",g_OpenList[iIndex].lpApplication,lpFileName));
				// �ö�Ӧ��Ӧ�ó���򿪸��ļ�
#ifdef INLINE_PROGRAM
				LoadApplication(g_OpenList[iIndex].lpApplication,lpFileName);
#else
				CreateProcess(g_OpenList[iIndex].lpApplication,lpFileName,0,0,0,0,0,0,0,0);
#endif
				break;
			}
			iIndex ++;
			if (g_OpenList[iIndex].lpExtend == NULL)
				break;
		}
        return 0;
}



// ********************************************************************
// ������static LRESULT CALLBACK FindDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hDlg - Ӧ�ó�����ҶԻ���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó�������ҶԻ�����Ϣ����
// ����: 
// ********************************************************************
static LRESULT CALLBACK FindDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ���ƴ���
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_COMMAND: // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_OK: // OK��ť
					FindString(hDlg);
					EndDialog(hDlg,TRUE);
					return 0;
				case IDC_CANCEL: // ȡ����ť
					EndDialog(hDlg,FALSE);
					return 0;
			}	
			return 0;
		case WM_INITDIALOG: // ��ʼ���Ի���
			return DoInitFindDlg(hDlg,wParam,lParam);
		case WM_CLOSE: // �رմ���
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}
// ********************************************************************
// ������static LRESULT DoInitFindDlg(HWND hDlg,WPARAM wParam,LPARAM lParam)
// ������
//	IN hDlg - Ӧ�ó�����ҶԻ���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó�������ҶԻ����� WM_INITDIALOG ��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoInitFindDlg(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	HWND hEdit;
		hEdit = GetDlgItem(hDlg,IDC_FINDTEXT); // �õ������ı����ھ��
		SetFocus(hEdit); // ���ý��㵽�ô���
		SetWindowLong(hDlg,GWL_USERDATA,lParam); // �����û�����
		return 0;
}
// ********************************************************************
// ������static LRESULT FindString(HWND hDlg)
// ������
//	IN hDlg - Ӧ�ó�����ҶԻ���Ĵ��ھ��
// ����ֵ��
//	
// ����������Ӧ�ó�������ҶԻ����� IDC_OK ����
// ����: 
// ********************************************************************
static LRESULT FindString(HWND hDlg)
{
	LPTSTR lpSearchString ;
	HWND hFindText;

		lpSearchString = (LPTSTR)GetWindowLong(hDlg,GWL_USERDATA); // �õ�Ҫ��Ѱ�ִ��Ļ���
		if (lpSearchString == NULL)
			return FALSE;
		hFindText = GetDlgItem(hDlg,IDC_FINDTEXT); // �õ���Ѱ�����༭��ľ��
		GetWindowText(hFindText,lpSearchString,MAX_SEARCHSTRINGLEN); // �õ��ı�

		return TRUE;

}


// ********************************************************************
// ������static void ShowFileProperty(HWND hWnd,LPFILEPROPERTY lpFileProperty)
// ������
//	IN hWnd - Ӧ�ó��򴰿ھ��
//    LPFILEPROPERTY lpFileProperty  - Ҫ��ʾ��ָ���ļ�������
// ����ֵ��
//	
// ����������Ӧ�ó�����ʾָ���ļ�������
// ����: 
// ********************************************************************
static void ShowFileProperty(HWND hWnd,LPFILEPROPERTY lpFileProperty)
{
	HINSTANCE hInst;

		hInst = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
		DialogBoxIndirectParamEx( hInst, (LPDLG_TEMPLATE_EX)&dlgShowPropertyTemplate, hWnd, ShowPropertyDialogFunc, (LPARAM)lpFileProperty); // ������ʾ���ԶԻ���
}


// ********************************************************************
// ������static LRESULT CALLBACK ShowPropertyDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hDlg - Ӧ�ó�����ʾ���ԶԻ���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó�����ʾ���Բ��ҶԻ�����Ϣ����
// ����: 
// ********************************************************************
static LRESULT CALLBACK ShowPropertyDialogFunc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT: // ������Ϣ
			hdc = BeginPaint( hDlg, &ps );
			DoPaintShowProperty(hDlg,hdc);
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_INITDIALOG: // ��ʼ������
			DoInitShowProperty(hDlg,wParam,lParam);
			return 0;
		case WM_CLOSE: // �رմ���
			EndDialog(hDlg,TRUE);
			return 0;
		default:
			return 0;
	}
}
// ********************************************************************
// ������static LRESULT DoInitShowProperty(HWND hDlg,WPARAM wParam,LPARAM lParam)
// ������
//	IN hDlg - Ӧ�ó�����ʾ���ԶԻ���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	
// ����������Ӧ�ó�������ʾ���ԶԻ����� WM_INITDIALOG ��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoInitShowProperty(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	SetWindowLong(hDlg,GWL_USERDATA,lParam); // �����û�����
	return 0;
}
// ********************************************************************
// ������static LRESULT DoPaintShowProperty(HWND hDlg,HDC hdc)
// ������
//	IN hDlg - Ӧ�ó�����ʾ���ԶԻ���Ĵ��ھ��
//    IN hdc  - �豸���
// ����ֵ��
//	
// ����������Ӧ�ó�������ʾ���ԶԻ����� WM_PAINT ��Ϣ
// ����: 
// ********************************************************************
static LRESULT DoPaintShowProperty(HWND hDlg,HDC hdc)
{
	LPFILEPROPERTY lpFileProperty;
	char lpTime[32],lpSize[32];
	SYSTEMTIME tSystemTime;

		lpFileProperty = (LPFILEPROPERTY)GetWindowLong(hDlg,GWL_USERDATA); // �õ��ļ�����
		if (lpFileProperty == NULL)
			return FALSE;
		SetBkMode( hdc, TRANSPARENT ) ; // ���û��Ʒ�ʽ
		TextOut(hdc,55,10,lpFileProperty->lpName,strlen(lpFileProperty->lpName));
		if (lpFileProperty->dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
		{ // ����Ŀ¼
			TextOut(hdc,55,35,"����Ŀ¼",strlen("����Ŀ¼"));
			sprintf(lpSize,"���̿ռ�: %dK",(lpFileProperty->dwTotalSize+1023)/1024);
			TextOut(hdc,10,60,lpSize,strlen(lpSize));
			sprintf(lpSize,"���ÿռ�: %dK",(lpFileProperty->dwFreeSize+1023)/1024);
			TextOut(hdc,10,85,lpSize,strlen(lpSize));
			return TRUE;
		}
		else if (lpFileProperty->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{ // �ļ���
			TextOut(hdc,55,35,"�ļ���",strlen("�ļ���"));
			sprintf(lpSize,"��С: %d(�ֽ�)",lpFileProperty->dwTotalSize);
			TextOut(hdc,10,60,lpSize,strlen(lpSize));
		}
		else
		{ // һ���ļ�
			TextOut(hdc,55,35,"һ���ļ�",strlen("һ���ļ�"));
			sprintf(lpSize,"��С: %d(�ֽ�)",lpFileProperty->dwTotalSize);
			TextOut(hdc,10,60,lpSize,strlen(lpSize));
		}


		// ����ʱ��
		FileTimeToSystemTime(&lpFileProperty->ftCreationTime,&tSystemTime);
		sprintf(lpTime,"����ʱ��: %02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
		TextOut(hdc,10,85,lpTime,strlen(lpTime));

		// �޸�ʱ��
		FileTimeToSystemTime(&lpFileProperty->ftLastAccessTime,&tSystemTime);
		sprintf(lpTime,"�޸�ʱ��: %02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
		TextOut(hdc,10,110,lpTime,strlen(lpTime));
		return TRUE;
}


// **************************************************
// ������static LRESULT DoShellNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// 
// ����ֵ����
// ��������������SHELL���͵�֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoShellNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hFileBrowser;

	if (wParam == SN_REFRESH)
	{  // Ҫ��ˢ���ļ�
		hFileBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�������ھ��
		SendMessage(hFileBrowser,EXM_REFRESH,0,0); // ˢ�´���
	}
	return 0;
}

// **************************************************
// ������static LRESULT DoDestroyWindow(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// 
// ����ֵ����
// ���������������ƻ����ڡ�
// ����: 
// **************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
//			sndPlaySound(NULL,0); // ֹͣ��������
			return 0;
}
