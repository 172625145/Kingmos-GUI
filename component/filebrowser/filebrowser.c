/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵�����ļ�����࣬�û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-02
���ߣ��½��� Jami chen
�޸ļ�¼��
		2004.08.24 ���Ӽ��й���
		2004.09.09 ����ⲿ�Ĵ���Ŀ¼�ĸ�Ŀ¼û��. ��.. ����
**************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
//#include "FileBRes.h"
#include "Explorer.h"
//#include "resource.h"
#include <efilepth.h>
#include "eClipbrd.h"
#include "eGlobmem.h"

/***************  ȫ���� ���壬 ���� *****************/
#define ID_LISTVIEW  101

#define TYPE_UNKNOWFILE	0
#define TYPE_FOLDER		1
#define TYPE_BMPFILE	2
#define TYPE_TEXTFILE	3
#define TYPE_MP3FILE	4
#define TYPE_IEFILE		5
#define TYPE_EMAILFILE	6
#define TYPE_EXEFILE	7
#define TYPE_GIFFILE	8
#define TYPE_JPGFILE	9

typedef struct structICONTYPE{
		int iFileType;
		int iIconID;
}ICONTYPE ;

static ICONTYPE tabIconType[] = {
	{ TYPE_UNKNOWFILE ,OIC_FILEEXPLORER_UNKNOWFILE },
	{ TYPE_FOLDER, OIC_FILEEXPLORER_FOLDER },
	{ TYPE_BMPFILE , OIC_FILEEXPLORER_BMPFILE },
	{ TYPE_TEXTFILE , OIC_FILEEXPLORER_TEXTFILE },
	{ TYPE_MP3FILE, OIC_FILEEXPLORER_MP3FILE },
	{ TYPE_IEFILE , OIC_FILEEXPLORER_IEFILE },
	{ TYPE_EMAILFILE , OIC_FILEEXPLORER_EMAILFILE },
	{ TYPE_EXEFILE , OIC_FILEEXPLORER_EXEFILE },
	{ TYPE_GIFFILE , OIC_FILEEXPLORER_GIFFILE },
	{ TYPE_JPGFILE , OIC_FILEEXPLORER_GIFFILE },
};

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

static FILETYPE tabFileType[] = {
	{ "*.bmp" , TYPE_BMPFILE },
	{ "*.txt" , TYPE_TEXTFILE },
	{ "*.mp3" , TYPE_MP3FILE },
	{ "*.pls" , TYPE_MP3FILE },
	{ "*.htm" , TYPE_IEFILE },
	{ "*.mlg" , TYPE_EMAILFILE },
	{ "*.exe" , TYPE_EXEFILE },
	{ "*.gif" , TYPE_GIFFILE },
	{ "*.jpg" , TYPE_JPGFILE },
};

#define SUMITEMNUM 4
#define ID_MAINCOLUMN	0
#define ID_SIZECOLUMN	1
#define ID_DATECOLUMN	2
#define ID_DIRCOLUMN	3

//#define _MAPPOINTER

typedef struct FileCopyData{
	TCHAR lpFileName[MAX_PATH];
	UINT  uFileTYPE;
}FILECOPYDATA, *LPFILECOPYDATA;

static const char classFileBrowser[] = "FileBrowser";// �ļ����������

typedef struct{
	HIMAGELIST hImageList;
	TCHAR lpCurDir[MAX_PATH];
	DWORD iSortIndex;
	BOOL bSearch;
	char  *lpSearch;
	FindFile FindFileFunc;


	UINT iSortSub;

	char *lpFilter;

	HWND hCopyWindow;
	BOOL bCut;  // �Ƿ��Ǽ��У�����Ǽ��У�����ճ����Ҫɾ��ԭ�����ļ�
}BROWSERDATA , *LPBROWSERDATA;

ATOM RegisterFileBrowserClass(HINSTANCE hInstance);
static LRESULT CALLBACK FileBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static LRESULT DoCreateWindow(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDestroyWindow(HWND hWnd);
static HWND  CreateListView(HWND  hWnd);
static BOOL UpdateListView( HWND hListView );
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage);
static BOOL  AddSubItem( HWND hListView, int item,  int itemSub,LPTSTR lpItem );
//static BOOL  CreateListViewColumn( HWND  hListView, DWORD dwStatus );
static BOOL  CreateListViewColumn( HWND  hListView, DWORD idColumn );
static void	LoadFileBrowserImage(HWND hWnd,HWND hListView);
static int InsertDirectoryItem(HWND hWnd,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir);
static void InsertFileItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoColumnClick(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoEndEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static int CALLBACK FileCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetListView(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  void SendNormalNotify(HWND hWnd,UINT iCode,LPNMLISTVIEW lpnmListView);
static void GetCurrentDirectory(HWND hListView,LPTSTR lpstrDir);
static LRESULT DoSetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoUpPath(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDeleteSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoFindFile(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoCopy(HWND hWnd);
static LRESULT DoCut(HWND hWnd);
static LRESULT DoPaste(HWND hWnd);
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoIsFind(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetSelFileName(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoGetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoOpenCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoRenameCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

static BOOL DeleteDirectory(HWND hWnd,LPTSTR lpFullName);
static BOOL SearchDirectory(HWND hWnd,HWND hListView,LPTSTR lpCurDir);
static void GetSearchString(LPTSTR *lpSearch,DWORD *pdwLen);
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName);
static BOOL CopyFolder(HWND hWnd,LPCTSTR lpExistingFolderName, LPCTSTR lpNewFolderName, BOOL bFailIfExists,BOOL bCover);

static BOOL IsChildDir(LPCTSTR lpParentFolderName, LPCTSTR lpChildFolderName);

static BOOL SearchCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
static BOOL StrnStr(const char *s1, const char *s2,int nLen);
static BOOL IsAsterisk(LPCTSTR lpcszMask, int iMaskLen);

static BOOL CallBackFindFile(HWND hWnd ,FILE_FIND_DATA	FindFileData,LPTSTR lpCurDir);

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );

static BOOL MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName);


// !!! Add By Jami chen in 2003.09.09
static LRESULT DoRefresh(HWND hWnd);
// !!! Add End By Jami chen in 2003.09.09
static LPTSTR BackupFileName(LPTSTR lpExistingFolderName);

// !!! Add By Jami chen in 2003.09.23
static BOOL IsExist(LPTSTR lpNewFileName);
static BOOL DeleteSpecialFile(HWND hWnd,LPTSTR lpFileName);
// !!! Add End By Jami chen in 2003.09.23

// !!! Add By Jami chen in 2003.09.25
static void ShowCopyDialog(HWND hWnd);
static void CloseCopyDialog(HWND hWnd);
// !!! Add End By Jami chen in 2003.09.25

// !!! Add By Jami chen in 2003.09.28
static LPTSTR GetOnlyFileName(LPTSTR lpFullName);
// !!! Add End By Jami chen in 2003.09.28

//static HWND g_hCopyDlg = NULL;

// ********************************************************************
// ������ATOM RegisterFileBrowserClass(HINSTANCE hInstance)
// ������
//	IN hInstance - ��ǰӦ�ó����ʵ�����
// ����ֵ��
//	�ɹ������ط��㣬���ɹ��������㡣
// ����������ע�ᵱǰӦ�ó������
// ����: �� Ӧ�ó�����ڳ��� ����
// ********************************************************************
ATOM RegisterFileBrowserClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)FileBrowserWndProc; // �ļ�������ƹ��̺���
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(LONG); // ���ô�����չ���ݳ���
	wc.hInstance		= hInstance;  // ����ʵ�����
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= GetStockObject(WHITE_BRUSH);  // �����౳��ˢ
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= classFileBrowser;  // ��������

	return RegisterClass(&wc);  // ��ϵͳע���ļ������
}


// ********************************************************************
// ������static LRESULT CALLBACK FileBrowserWndProc(HWND , UINT , WPARAM , LPARAM )
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
//    IN message - ������Ϣ
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	��ͬ����Ϣ�в�ͬ�ķ���ֵ�����忴��Ϣ����
// �����������ļ�����ര�ڹ��̺���
// ����: 
// ********************************************************************
static LRESULT CALLBACK FileBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
		case WM_PAINT:  // ������Ϣ
			hdc = BeginPaint(hWnd, &ps);
//			DoPaint(hWnd,hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_COMMAND:  // ������Ϣ
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_NOTIFY:  // ֪ͨ��Ϣ
			return DoNotify(hWnd,wParam,lParam);
		case EXM_GETLISTVIEW: // �õ���ʽ�ӿؼ����
			return DoGetListView(hWnd,wParam,lParam);
		case EXM_SETCURPATH:  // ���õ�ǰ·��
			return DoSetCurPath(hWnd,wParam,lParam);
		case EXM_GETCURPATH:  // �õ���ǰ·��
			return DoGetCurPath(hWnd,wParam,lParam);
		case EXM_UPPATH: // ����һ��Ŀ¼
			return DoUpPath(hWnd,wParam,lParam);
		case EXM_DELETESEL:  // ɾ��ѡ��
			return DoDeleteSel(hWnd,wParam,lParam);
		case EXM_FINDFILE:  // �����ļ�
			return DoFindFile(hWnd,wParam,lParam);
		case EXM_NEWFOLDER:  // ����һ���µ�Ŀ¼
			return DoNewFolder(hWnd,wParam,lParam);
		case EXM_ISFIND: // �Ƿ��ڲ�Ѱ״̬
			return DoIsFind(hWnd,wParam,lParam);
		case EXM_GETSELFILENAME:  // �õ��ļ���
			return DoGetSelFileName(hWnd,wParam,lParam);
		case EXM_GETFILTER: // �õ����˴�
			return DoGetFilter(hWnd,wParam,lParam);
		case EXM_SETFILTER: // ���ù��˴�
			return DoSetFilter(hWnd,wParam,lParam);
		case EXM_GETPROPERTY:  // �õ���ǰ����
			return DoGetProperty(hWnd,wParam,lParam);
		case WM_CREATE:  // �����ļ������
			return DoCreateWindow(hWnd,wParam,lParam);
		case WM_SETFOCUS: // ���ý���
			return DoSetFocus(hWnd,wParam,lParam);
		case WM_DESTROY:  // �ƻ�����
			DoDestroyWindow(hWnd);
			break;
		case WM_COPY:
			DoCopy(hWnd);
			break;
		case WM_PASTE:
			DoPaste(hWnd);
			break;
		case WM_CUT:
			DoCut(hWnd);
			break;
// !!! Add By Jami chen in 2003.09.09
		case EXM_REFRESH:
			DoRefresh(hWnd);
			break;
// !!! Add End By Jami chen in 2003.09.09

		case EXM_OPENCURSEL:
			DoOpenCurSel(hWnd,wParam,lParam);
			break;

		case EXM_RENAME: //��������ǰ�ļ�
			DoRenameCurSel(hWnd,wParam,lParam);
			break;

	case WM_SIZE: // ���ڴ�С�����ı�
			return DoSize(hWnd,wParam,lParam);
	case WM_WINDOWPOSCHANGED:
			return DoWindowPosChanged(hWnd,wParam,lParam);

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
	LPBROWSERDATA lpBrowser = (LPBROWSERDATA)malloc(sizeof(BROWSERDATA));

//	RETAILMSG(1,("Explorer Create ...\r\n"));
	lpBrowser->hImageList = NULL;  // ��ʼ��ͼ���б�Ϊ��
//	lpBrowser->lpCurDir = "\\";
	strcpy(lpBrowser->lpCurDir , "\\");  // ��ǰĿ¼Ϊ��Ŀ¼

	lpBrowser->bSearch = FALSE;  // ���ڲ�Ѱ״̬
	lpBrowser->lpSearch = NULL;
	lpBrowser->FindFileFunc = NULL;

	lpBrowser->lpFilter = NULL;  // û�й��˴�

	lpBrowser->iSortIndex = 0;    // û������
	lpBrowser->hCopyWindow = NULL;

	lpBrowser->bCut = FALSE; 
	SetWindowLong(hWnd,0,(LONG)lpBrowser);  // ����ǰ�������ṹָ����봰��
	CreateListView(hWnd); // ������ʽ
//	RETAILMSG(1,("Explorer Create OK \r\n"));
	return 0;
}
// ********************************************************************
// ������static LRESULT DoDestroyWindow(HWND hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
//	����TRUE���������ƻ����ڣ�����FALSE�����ƻ�����
// ����������Ӧ�ó������ƻ����ڵ���Ϣ
// ����: 
// ********************************************************************
static LRESULT DoDestroyWindow(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;

	lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
	if (lpBrowser->hImageList)
	{ // �ƻ�ͼ���б�
		ImageList_Destroy(lpBrowser->hImageList);
		lpBrowser->hImageList = NULL;
	}
	if (lpBrowser->lpSearch)
	{
		free(lpBrowser->lpSearch);  // �ͷŲ�Ѱ��
		lpBrowser->lpSearch = NULL;
	}
	if (lpBrowser->lpFilter)
	{
		free(lpBrowser->lpFilter);  // �ͷŹ��˴�
		lpBrowser->lpFilter = NULL;
	}

//	OpenClipboard(hWnd);
//	SetClipboardData(CF_FILE,NULL);
//	CloseClipboard();

	return 0;
}

// ********************************************************************
// ������static HWND  CreateListView(HWND  hWnd)
// ������
//	IN hWnd- Ӧ�ó���Ĵ��ھ��
// ����ֵ��
// 	�ɹ�����ListView�Ĵ��ھ�������򷵻�NULL
// ��������������һ��ListView�ؼ�
// ����: 
// ********************************************************************
static HWND  CreateListView(HWND  hWnd)
{
	RECT						rt;
	HWND						hListView;
	CTLCOLORSTRUCT stCtlColor;
	DWORD dwStyle;

//	RETAILMSG(1,("CreateListView ...\r\n"));
	GetClientRect(hWnd, &rt);  // �õ����ھ��δ�С
	// ������ʽ
	hListView=CreateWindow( classLISTCTRL,"",
//				WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL| WS_VSCROLL | WS_HSCROLL,
				WS_VISIBLE|WS_CHILD|LVS_LIST|LVS_SHOWSELALWAYS|LVS_SINGLESEL| WS_VSCROLL | WS_HSCROLL | LVS_HORZLINE,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_LISTVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

	if (hListView == NULL)
	{ // ������ʽʧ��
//		RETAILMSG(1,("CreateListView Failure\r\n"));
		return NULL;
	}


	dwStyle = GetWindowLong(hWnd,GWL_STYLE);

//	RETAILMSG(1,("Will Set ListView Color...\r\n"));
	stCtlColor.fMask = CLF_TEXTCOLOR | CLF_TEXTBKCOLOR | CLF_SELECTIONCOLOR | CLF_SELECTIONBKCOLOR | CLF_TITLECOLOR | CLF_TITLEBKCOLOR;
	stCtlColor.cl_Text = RGB(78,81,78);
	stCtlColor.cl_TextBk = RGB(247,255,247);
	stCtlColor.cl_Selection = RGB(255,255,255);
	stCtlColor.cl_SelectionBk = RGB(77,166,255);
	stCtlColor.cl_Title = RGB(78,81,78);
	stCtlColor.cl_TitleBk = RGB(247,255,247);
	//SendMessage(hListView,WM_SETCTLCOLOR,0,(LPARAM)&stCtlColor);  // ������ʽ��ɫ

//	RETAILMSG(1,("Will LoadImage...\r\n"));
	LoadFileBrowserImage(hWnd,hListView);  // װ���ļ�ͼ���б�
//	RETAILMSG(1,("Will Insert Column...\r\n"));
	CreateListViewColumn( hListView, ID_MAINCOLUMN);  // ��������
	if ((dwStyle & FBS_NOSIZECOLUMN) == 0)
		CreateListViewColumn( hListView, ID_SIZECOLUMN); // �����ߴ���
	if ((dwStyle & FBS_NODATECOLUMN) == 0)
		CreateListViewColumn( hListView, ID_DATECOLUMN); // ����������
//	RETAILMSG(1,("Will Insert Data...\r\n"));
	UpdateListView(hListView);  // ������ʽ����
//	RETAILMSG(1,("CreateListView success\r\n"));
	return hListView;
}


// ********************************************************************
// ������static BOOL UpdateListView( HWND hWnd )
// ������
//	IN hListView- ListView�Ĵ��ھ��
// ����ֵ��
//	�ɹ�����TRUE�����򷵻�FALSE
// ��������������ListView�ؼ�������
// ����: 
// ********************************************************************
static BOOL UpdateListView( HWND hListView )
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						lpstrDir[MAX_PATH];
	HWND hWnd;
	LPBROWSERDATA lpBrowser;
// Add By Jami chen in 2003.09.09
	DWORD unViewAttributes = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ROMMODULE;
// Add End By Jami chen in 2003.09.09
	DWORD dwStyle;


		hWnd				= GetParent( hListView );  // �õ������ھ��
		dwStyle = GetWindowLong(hWnd,GWL_STYLE); // �õ����ڷ��

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ������ṹָ��
		if (lpBrowser == 0)
			return 0; // ���ݴ���
		SendMessage(hListView,LVM_DELETEALLITEMS,0,0);  // ɾ����ǰ��������

		if (lpBrowser->bSearch)
		{  // �ǲ�Ѱ״̬
//			RETAILMSG(1,(" Will Search Need File \r\n"));
			strcpy(lpstrDir,"\\");
			SearchDirectory(hWnd,hListView,lpstrDir);  // ���Ҹ�Ŀ¼
		}
		else
		{  // ���Ǹ�Ŀ¼
			GetCurrentDirectory(hListView,lpstrDir);  // �õ���ǰ���ļ���
//			strcat(lpstrDir,"\\*.*");  // Ҫ�������е��ļ�
/*			if (strcmp(lpstrDir,"\\") == 0)
			{
				strcat(lpstrDir,"*.*");
			}
			else
			{
				strcat(lpstrDir,"\\*.*");
			}
*/
			// Add By Jami chen in 2004.09.09
			if (strcmp(lpstrDir,"\\") != 0)
			{
				// ���Ǹ�Ŀ¼��������. ��.. Ŀ¼
				if (dwStyle & FBS_SHOWSUBDIR)
				{
					InsertListViewItem(hListView,0xffff,0,".",TYPE_FOLDER);  //�����ļ��������
					InsertListViewItem(hListView,0xffff,0,"..",TYPE_FOLDER);  //�����ļ��������
				}
			}
			// Add End By Jami chen in 2004.09.09
			MergeFullFileName(lpstrDir,NULL,"*.*"); // Ҫ�������е��ļ�
//			RETAILMSG(1,(" FindFirstFile %s \r\n",lpstrDir));
			hFind=FindFirstFile(lpstrDir, &FindFileData  );  // ���ҵ�һ���ļ�
			if( hFind == INVALID_HANDLE_VALUE) 
			{// �ļ�������
				return TRUE;
			}
			while(TRUE)
			{  // �ļ�����
				if(FindFileData.cFileName[0]=='.')
				{  // ���ļ���һ����ʾ��ǰĿ¼����Ŀ¼���ļ���������
					// �ж��Ƿ���Ҫ��ʾ��Ŀ¼�ļ�
					// Delete By Jami chen in 2004.09.09
//					if (dwStyle & FBS_SHOWSUBDIR)
//					{
//						InsertDirectoryItem(hListView,FindFileData,NULL);  // ����ǰĿ¼���뵽��ʽ
//					}
					// Delete End By Jami chen in 2004.09.09
				}
				else
				{
					// !!! Add By Jami chen in 2003.09.09
					// �Ƿ���Ҫ���û�����
					if (!( FindFileData.dwFileAttributes&unViewAttributes ))
					{
					// !!! Add End By Jami chen in 2003.09.09
		//				strcpy(lpstrFullName,lped->lpstrCurrentDir);
		//				strcat(lpstrFullName,"\\");
		//				strcat(lpstrFullName,FindFileData.cFileName);
	//					RETAILMSG(1,("Find File < %s >\r\n",FindFileData.cFileName));
						if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
						{
							// ���ҵ�����һ��Ŀ¼
	//						InsertDirectoryItem(hListView,FindFileData,lpBrowser->lpCurDir);
	//						RETAILMSG(1,("Find a directory\r\n"));
							InsertDirectoryItem(hListView,FindFileData,NULL);  // ����ǰĿ¼���뵽��ʽ
		//					InsertListViewItem(hWnd,1,0,FindFileData.cFileName,1);
		//					if(lped->dExploreStatus&EXPS_OPENEXPLORE)
		//					{
		//						strcpy(lped->lpstrCurrentDir,lpstrFullName);
		//						AddFileItemToExp(hWnd);
		//						strcpy(lped->lpstrCurrentDir,lpstrOldDir);
		//					}
						}
						else
						{
							// ���ҵ�����һ���ļ�
							if (lpBrowser->lpFilter != NULL)
							{  // �й��˴����жϵ�ǰ�ļ��Ƿ�������˴�
								LPTSTR lpFilter;
								LPTSTR lpFileName = NULL;
								DWORD dwlen;

	//							RETAILMSG(1,("Will Find File <%s>\r\n",lpBrowser->lpFilter));
								// !!! Add By Jami chen in 2003.09.28
								lpFileName = GetOnlyFileName(FindFileData.cFileName); // �õ����ļ���
								// !!! Add End By Jami chen in 2003.09.28
								lpFilter = lpBrowser->lpFilter;  // �õ����˴�
								while(1)
								{
									GetSearchString(&lpFilter,&dwlen); // �õ�������
									if (dwlen == 0)
									{
	//									RETAILMSG(1,("Find File is not Need\r\n"));
										break;
									}
									// !!! Modified By Jami chen in 2003.09.28
//									if (FileNameCompare( lpFilter, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)
									if (FileNameCompare( lpFilter, dwlen, lpFileName, strlen(lpFileName)) == FALSE)
									// !!! Modified End By Jami chen in 2003.09.28
									{  // ��ǰ�ļ�������������
										lpFilter += dwlen; // ��һ��������
										continue;
									}
									// ��ǰ�ļ���������
	//								RETAILMSG(1,("Find File is Need\r\n"));
									InsertFileItem(hListView,FindFileData,NULL);  // ����ǰ�ļ����뵽��ʽ
									break;
								}
	//							strcat(lpstrDir,lpBrowser->lpFilter);  // Ҫ������Ҫ���ļ�
							}
							else
							{  // û�й��˴���ֱ�ӽ��ļ����뵽��ʽ
								InsertFileItem(hListView,FindFileData,NULL);
							}
							//InsertListViewItem(hWnd,1,0,FindFileData.cFileName,0);
//							InsertFileItem(hListView,FindFileData,lpBrowser->lpCurDir);
						}
					}
				}
				if( FindNextFile(hFind,&FindFileData)==0 )  //������һ���ļ�
				{ // �Ѿ�û���ļ���
					break;
				}
			}
			FindClose(hFind);  // ���ҽ���
		}

//		�����������
		{
			int iCount;
			int iCurItem = 0;
				
				iCount = SendMessage(hListView,LVM_GETITEMCOUNT,0,0); // �õ���ǰ�б����Ŀ����
				if (iCount)
				{
					// ����Ŀ����
					SendMessage(hListView,LVM_SETHOTITEM,iCurItem,0);  // ���õ�һ����ĿΪ������Ŀ
					SendMessage(hListView,LVM_SETSELECTIONMARK,0,iCurItem);  // ���õ�һ����ĿΪѡ����Ŀ
				}
		}
		return TRUE;
}
// ********************************************************************
//������static int InsertListViewItem(HWND hListView,int iItem, int itemSub,TCHAR* lpItem,int iImage)
//������
//	IN hListView- ListView�Ĵ��ھ��
//    IN iItem  - Ҫ�����Ŀ����Ŀλ��
//    IN itemSub - Ҫ��ӵ���Ŀ������Ŀλ��
//    IN lpItem  - Ҫ�����Ŀ������
//    IN iImage  - Ҫ�����Ŀ��Imageλ��
// ����ֵ��
//	���ز�����Ŀ��Index
// ������������ListView�ؼ������һ����Ŀ
// ����: 
// ********************************************************************
static int InsertListViewItem(HWND hListView,int iItem, int itemSub,
							   TCHAR* lpItem,int iImage)
{
	LV_ITEM				lvitem;
	HWND				hWnd;
//	TCHAR				lpText[MAX_PATH];

	LPBROWSERDATA lpBrowser;

	hWnd				= GetParent( hListView );  // �õ������ھ��
	lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ��ļ������ṹָ��
	if (lpBrowser == 0)
		return 0; // ���ݴ���

//	strcpy( lpText, lpItem );
/*	if( GetWindowLong( hWnd, GWL_STYLE )&EXPS_LISTVIEWICON  )
	{
		GetMainFileName( lpText ) ;
	}
*/
	lvitem.mask			= LVIF_TEXT | LVIF_PARAM ;
	if( ! itemSub  )
		lvitem.mask		|= LVIF_IMAGE;
	lvitem.iItem		= iItem;     // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
	lvitem.cchTextMax	= strlen(lpItem );
	lvitem.iImage		= iImage;
	lvitem.lParam		= lpBrowser->iSortIndex;  // ���뵽ָ����λ�ã������������״̬������뵽���

	iItem = SendMessage( hListView, LVM_INSERTITEM, 0, ( LPARAM )&lvitem ); // ����һ����Ŀ����ʽ

	lpBrowser->iSortIndex ++;  // ָ����һ����Ŀ
	
	return iItem;
}
// ********************************************************************
// ������static BOOL  AddSubItem( HWND hListView, int item,  int itemSub,LPTSTR lpItem )
// ������
//	IN hListView- ListView�Ĵ��ھ��
//    IN iItem  - Ҫ�����Ŀ����Ŀλ��
//    IN itemSub - Ҫ��ӵ���Ŀ������Ŀλ��
//    IN lpItem  - Ҫ�����Ŀ������
// ����ֵ��
// 	�ɹ�����TRUE�����򷵻�FALSE.
// ������������ListView�ؼ�ָ������Ŀ�����һ������Ŀ
// ����: 
// ********************************************************************
static BOOL  AddSubItem( HWND hListView, int item,  int itemSub,LPTSTR lpItem )
{
	LV_ITEM				lvitem;
	lvitem.mask			=  LVIF_TEXT;
	lvitem.iItem		= item;   // the item index,where the item shows. 
	lvitem.iSubItem		= itemSub;
	lvitem.pszText		= lpItem;
//	lvitem.cchTextMax	= strlen( lpItem);
	return SendMessage(hListView,LVM_SETITEM,0,(LPARAM)&lvitem);  // ������Ŀ����
}

// ********************************************************************
// ������static BOOL  CreateListViewColumn( HWND  hListView, DWORD idColumn )
// ������
//	IN hListView - ListView�Ĵ��ھ��
//    IN idColumn - Ҫ�������к�
// ����ֵ��
//	��
// ������������ListView�ؼ����������Ŀ
// ����: 
// ********************************************************************
static BOOL  CreateListViewColumn( HWND  hListView, DWORD idColumn )
{
//	short			i;
	LV_COLUMN		lvcolumn;
//	short           column= SUMITEMNUM;
//	DWORD           dwWinAttr;
	
	int			columnwidth[]={240,50,90,280};
//	int			columnwidth[]={148,50,90,280};
//	int			columnwidth[]={110,50,20,10};
	char *pTitle[SUMITEMNUM] = {"����","��С","����","Ŀ¼"};

//	RETAILMSG(1, ("Calling  SendMessage 0\r\n"));
//    dwWinAttr=GetWindowLong( hListView,GWL_STYLE );
//	dwWinAttr &=~LVS_TYPEMASK;
//	dwWinAttr |=dwStatus;
//	SetWindowLong(hListView,GWL_STYLE, dwWinAttr );
//	for ( i = 0; i < SUMITEMNUM; i++ )  
//	{
	if (idColumn >= SUMITEMNUM)
		return FALSE;  // ָ�����ǷǷ���
	lvcolumn.mask = LVCF_FMT | LVCF_WIDTH |LVCF_SUBITEM | LVCF_TEXT ;  // �����нṹ�е���Ч����
	if(idColumn  == 0 || idColumn  == 3)
		lvcolumn.fmt = LVCFMT_LEFT ;
	else
		lvcolumn.fmt = LVCFMT_RIGHT ;
	if( idColumn==0 )
		lvcolumn.fmt|=	LVCFMT_IMAGE;  // �����е���ʾ״̬
	lvcolumn.pszText = pTitle[idColumn];  // �����еı���
	lvcolumn.cchTextMax =strlen (pTitle[idColumn]);
	lvcolumn.iSubItem = idColumn;
	lvcolumn.cx = columnwidth[idColumn];     // width of column.
//		lvcolumn.iImage= 0;
//	RETAILMSG(1, ("Calling  SendMessage\r\n"));
	if( SendMessage(hListView,LVM_INSERTCOLUMN,idColumn,(LPARAM)&lvcolumn)==-1 ) // ����һ���е���ʽ
	{
		return FALSE;
	}
//	}
	return TRUE;
}
// ********************************************************************
// ������static BOOL  DeleteListViewColumn( HWND  hListView, DWORD idColumn )
// ������
//	IN hListView - ListView�Ĵ��ھ��
//    IN idColumn - Ҫɾ�����к�
// ����ֵ��
//	��
// ������������ListView�ؼ���ɾ������Ŀ
// ����: 
// ********************************************************************
static BOOL  DeleteListViewColumn( HWND  hListView, DWORD idColumn )
{
	if( SendMessage(hListView,LVM_DELETECOLUMN,idColumn,0)==FALSE )  // ɾ��һ��ָ������
	{
		return FALSE;
	}
	return TRUE;
}
// ********************************************************************
// ������static void	LoadFileBrowserImage(HWND hWnd,HWND hListView)
// ������
//	IN hWnd - FileBrowser�Ĵ��ھ��
//	IN hListView - ListView�Ĵ��ھ��
// ����ֵ��
//	��
// ������������ListView�ؼ������һ�� ImageList
// ����: 
// ********************************************************************
static void	LoadFileBrowserImage(HWND hWnd,HWND hListView)
{
	HICON					hIcon ;
//	LPREGAPDATA				lprad ;
	int						i ;
	int iIconTypeNum;
	LPBROWSERDATA lpBrowser;
	HINSTANCE hInstance;


		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // �õ����ڵ�ʵ�����
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��

		if (lpBrowser->hImageList == NULL)
		{ //ImageList �ǿյģ����봴��һ��ImageList
			lpBrowser->hImageList = ImageList_Create(16,16,ICL_ICON,8,8); //����һ��ͼ���б�

			if (lpBrowser->hImageList == NULL) //����ImageListʧ��
				return ;
		}
// !!! Modified By Jami chen in 2003.08.22
/*		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_FOLDER ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;

		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_TEXTFILE ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;

		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_BMPFILE ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;

		hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_FILEEXPLORER_UNKNOWFILE ), IMAGE_ICON,
			16, 16, 0 ) ;
		if( hIcon != 0 )
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ;
*/
		iIconTypeNum = sizeof(tabIconType) / sizeof(ICONTYPE);

		for ( i = 0 ; i < iIconTypeNum ; i++ )
		{
			hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( tabIconType[i].iIconID ), IMAGE_ICON,
				16, 16, 0 ) ;  // װ��ָ����ͼ��
			ImageList_AddIcon( lpBrowser->hImageList, hIcon ) ; // ��ͼ����ӵ��б�
		}
// !!! Modified End By Jami chen in 2003.08.22
		//Get all of the Application
	/*	GetRegistApplication( lped->lpExtList, 0, FALSE ) ;
		for( i = 0 ; i < lped->lpExtList->count ; i ++ )
		{
			lprad = ( LPREGAPDATA )PtrListAt( lped->lpExtList, i ) ;
			ASSERT( lprad != 0 ) ;
			if( strcmp( lprad->lpszPostfix, "" ) == 0 )
			{
				PtrListAtFree( lped->lpExtList, i ) ;
				i -- ;
			}else
			{
				hIcon = GetApplicationIcon( lprad->lpszInternal, SMALL_ICON ) ;
				ASSERT( hIcon != 0 ) ;
				ImageList_AddIcon( lped->hImageList, hIcon ) ;
			}
		}
	*/
		lpBrowser->hImageList = (HIMAGELIST)SendMessage(hListView,LVM_SETIMAGELIST,LVSIL_SMALL, (LPARAM)lpBrowser->hImageList);  // ����ͼ���б���ʽ

}

// ********************************************************************
//������static int InsertDirectoryItem(HWND hListView,FILE_FIND_DATA FindFileData)
//������
//	IN hListView - ListView�Ĵ��ھ��
//  IM FindFileData  - ��ǰ�ҵ����ļ�������
//����ֵ��
//	��
//������������ListView�ؼ������һ���ļ�����Ŀ
//����: 
// ********************************************************************
static int InsertDirectoryItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir)
{
	int iItem ;
	char lpTime[16];
	SYSTEMTIME tSystemTime;
	DWORD dwStyle;


		iItem = InsertListViewItem(hListView,0xffff,0,FindFileData.cFileName,TYPE_FOLDER);  //�����ļ��������
// !!! modified  By Jami chen in 2004.06.23
		dwStyle = GetWindowLong(GetParent(hListView),GWL_STYLE);
		if ((dwStyle & FBS_NODATECOLUMN) == 0)
		{
			FileTimeToSystemTime(&FindFileData.ftLastAccessTime,&tSystemTime);
			sprintf(lpTime,"%02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
			AddSubItem(hListView,iItem,2,lpTime);  //�����ļ�ʱ��
		}
// !!! modified end By Jami chen in 2004.06.23

		if (lpCurDir)
			AddSubItem(hListView,iItem,3,lpCurDir);  //����Ŀ¼

		return iItem;
}

// ********************************************************************
//������static void InsertFileItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir)
//������
//	IN hListView - ListView�Ĵ��ھ��
//  IM FindFileData  - ��ǰ�ҵ����ļ�������
//����ֵ��
//	��
//������������ListView�ؼ������һ���ļ���Ŀ
//����: 
// ********************************************************************
static void InsertFileItem(HWND hListView,FILE_FIND_DATA FindFileData,LPTSTR lpCurDir)
{
	int iItem ;
	DWORD dwSize;//,dot;
	char lpSize[16];
	char lpTime[16];
	SYSTEMTIME tSystemTime;
	int iImageIndex = TYPE_UNKNOWFILE;
	int iFileTypeNum , i;
	LPTSTR lpFileName = NULL;
	DWORD dwStyle;


// !!! Modified By Jami chen in 2003.08.22
/*		
//		if (StrAsteriskCmp("*.txt",FindFileData.cFileName) == 0)
		if (FileNameCompare( "*.txt", strlen("*.txt"), FindFileData.cFileName, strlen(FindFileData.cFileName)) == TRUE)
		{
			iImageIndex =  TYPE_TEXTFILE;
		}
//		else if (StrAsteriskCmp("*.bmp",FindFileData.cFileName) == 0)
		else if (FileNameCompare( "*.bmp", strlen("*.bmp"), FindFileData.cFileName, strlen(FindFileData.cFileName)) == TRUE)
		{
			iImageIndex =  TYPE_BMPFILE;
		}

*/
		// !!! Add By Jami chen in 2003.09.28
		lpFileName = GetOnlyFileName(FindFileData.cFileName);  // �õ����ļ���
		// !!! Add End By Jami chen in 2003.09.28
		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE);
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			// !!! Modified  By Jami chen in 2003.09.28
//			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), FindFileData.cFileName, strlen(FindFileData.cFileName)) == TRUE)
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpFileName, strlen(lpFileName)) == TRUE)
			// !!! Modified  End By Jami chen in 2003.09.28
			{ // �Ƚϵ�ǰ�ļ��Ƿ�����֪�ļ�
				iImageIndex =  tabFileType[i].iFileType;  // ʹ����֪�ļ���ͼ��
				break;
			}
		}
// !!! Modified End By Jami chen in 2003.08.22

		iItem = InsertListViewItem(hListView,0xffff,0,FindFileData.cFileName,iImageIndex);  //�����ļ��������

// !!! modified By Jami chen in 2004.06.23

		dwStyle = GetWindowLong(GetParent(hListView),GWL_STYLE);
		if ((dwStyle & FBS_NOSIZECOLUMN) == 0)
		{
			dwSize = FindFileData.nFileSizeLow;  // �õ��ļ��ߴ�
			if (dwSize > (1024 *1024l))
			{  // ����1M���ļ���ʾΪx.xM
				dwSize = ((dwSize+1024 * 1024 / 10l-1)* 10) / (1024 *1024l);
				sprintf(lpSize,"%d.%dM",dwSize/10,dwSize%10);
			}
			else if (dwSize > 0)
			{  // �����ļ���ʾΪxK
				dwSize = (dwSize+1023l) / 1024l;
				sprintf(lpSize,"%dK",dwSize);
			}
			else 
			{ // �ļ�����Ϊ0���ļ���ʾΪ0
				strcpy(lpSize,"0");
			}
			AddSubItem(hListView,iItem,1,lpSize);  //�����ļ�����
		}

		if ((dwStyle & FBS_NODATECOLUMN) == 0)
		{
			FileTimeToSystemTime(&FindFileData.ftLastAccessTime,&tSystemTime);
			sprintf(lpTime,"%02d-%02d-%02d",tSystemTime.wYear%100,tSystemTime.wMonth,tSystemTime.wDay);
			AddSubItem(hListView,iItem,2,lpTime);  //�����ļ�ʱ��
		}

// !!! modified end By Jami chen in 2004.06.23
		if (lpCurDir)
			AddSubItem(hListView,iItem,3,lpCurDir);  //����Ŀ¼
}

// **************************************************
// ������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ��������Ϣ�Ĵ�����
// ��������������֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	NMLISTVIEW *pnmvl;
	NMHDR   *hdr=(NMHDR   *)lParam;    

		switch(hdr->code)
		{
			 case LVN_COLUMNCLICK:  // ��굥����ͷ
//				 pnmvl=(NMLISTVIEW *)lParam;
//				 SendMessage(pnmvl->hdr.hwndFrom,LVM_SORTITEMS,(WPARAM)pnmvl->iSubItem,(LPARAM)FileCompareFunc);
//				 iDistance=1-iDistance;
				 DoColumnClick(hWnd,wParam,lParam);
				 break;
			 case LVN_ITEMACTIVATE:  // ��Ŀ����
//				 DoActiveItem(hWnd,wParam,lParam);
				 DoOpenCurSel(hWnd,0,0);
				 break;
			 case LVN_ITEMCHANGED: // ��Ŀ�ı�
				 DoItemChanged(hWnd,wParam,lParam);
				 break;
			 case LVN_ENDLABELEDIT:  // �༭��Ŀ��ǩ����
				 return DoEndEditLabel(hWnd,wParam,lParam);
		}
		return 0;
}

// **************************************************
// ������static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPNMITEMACTIVATE �ṹָ��
// 
// ����ֵ����
// ����������������Ŀ�����֪ͨ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	int iItem;//,iFileNo;
	HWND hListWnd;
	LVITEM	lvItem;
	LPNMITEMACTIVATE lpnmia;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��
		lpnmia=(LPNMITEMACTIVATE)lParam;  // �õ�LPNMITEMACTIVATE�ṹָ��
		
		hListWnd=lpnmia->hdr.hwndFrom;  // �õ���ʽ�Ĵ��ھ��
		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=lpnmia->iItem;  // �õ�������Ŀ����
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
//		SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)iItem,(LPARAM)&lvItem);
		SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // �õ���Ŀ�ļ���

		if (lpBrowser->bSearch == TRUE)
		{  // ��ǰΪ��Ѱ״̬
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=lpnmia->iItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);  // �õ��ļ�Ŀ¼

/*			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}
*/
			if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE) // ����ļ�ȫ·��
				return 0;
		}
		else
		{
			if (strcmp(lpFileName,".") == 0)
			{
				// ��ǰĿ¼������Ҫ����
				return 0;
			}
			if (strcmp(lpFileName,"..") == 0)
			{
				// ����һ��Ŀ¼
				DoUpPath(hWnd,0,0);
				return 0;
			}
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)  // ����ļ�ȫ·��
				return 0;
		}

		if (lvItem.iImage == TYPE_FOLDER)
		{ // ��ǰ��һ��Ŀ¼
			HWND hListView ;
	
				hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���
				if (lpBrowser->bSearch)
				{  // ��ǰ�ڲ�Ѱ״̬
					lpBrowser->bSearch = FALSE;
					DeleteListViewColumn(hListView,ID_DIRCOLUMN); // ɾ��Ŀ¼��
					if (lpBrowser->lpSearch)
					{
						free(lpBrowser->lpSearch);  // �ͷŲ�Ѱ��
						lpBrowser->lpSearch = NULL;
					}
					lpBrowser->FindFileFunc = NULL;  // ���ûص�Ϊ��
				}
				strcpy(lpBrowser->lpCurDir,lpFullName);  // ���õ�ǰĿ¼Ϊ�����Ŀ¼
				UpdateListView( hListView ); // ������ʽ
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // ����֪ͨ��Ϣ���ݸı�
		}
		else
		{

			SendNormalNotify(hWnd,EXN_ACTIVEITEM,NULL);  // ������Ŀ�����֪ͨ
		}

        return 0;
}
// **************************************************
// ������static LRESULT DoColumnClick(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ָ��NMLISTVIEW��ָ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������������ͷ����
// ����: 
// **************************************************
static LRESULT DoColumnClick(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	NMLISTVIEW *pnmvl;
	LPBROWSERDATA lpBrowser;

		pnmvl=(NMLISTVIEW *)lParam;  // �õ���ʽ֪ͨ�ṹָ��

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == NULL)
			return FALSE; // ���ݴ���
		lpBrowser->iSortSub = pnmvl->iSubItem;  // �õ���������е�����
		SendMessage(pnmvl->hdr.hwndFrom,LVM_SORTITEMS,(WPARAM)hWnd,(LPARAM)FileCompareFunc); // ����������Ϣ����ʽ
		return TRUE;
}

// **************************************************
// ������static int CALLBACK FileCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
// ������
// 	IN lParam1 -- ��Ŀ1�Ĳ���
// 	IN lParam2 -- ��Ŀ2�Ĳ���
// 	IN lParamSort -- ��ʽ����������е���������ָ�����������������ľ��
// 
// ����ֵ������0 ��ʾ��ͬ������0 ��ʾ��Ŀ1 ������Ŀ2��С��0 ��ʾ��Ŀ1 С����Ŀ2
// �����������Ƚ϶���Ŀ�Ĵ�С��
// ����: 
// **************************************************
static int CALLBACK FileCompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{
	HWND hWnd;
	HWND hListView;
	LPBROWSERDATA lpBrowser;
	int iIndex1, iIndex2 ;
	LVFINDINFO lvFinfInfo;
	LVITEM lvItem;
	TCHAR lpText1[MAX_PATH],lpText2[MAX_PATH];
	int iImage1,iImage2;
		
		hWnd = (HWND)lParamSort;  // �õ������Ĵ��ھ��

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������Ľṹָ��
		if (lpBrowser == NULL)
			return FALSE; // ���������޷��Ƚ�

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�ľ��

		iIndex1 = -1;
		lvFinfInfo.flags = LVFI_PARAM;
		lvFinfInfo.lParam = lParam1;
		iIndex1 = SendMessage(hListView,LVM_FINDITEM, iIndex1,(LPARAM)&lvFinfInfo); // �õ���Ŀ1������
		if (iIndex1 == -1) 
			return FALSE; // û���ҵ���Ŀ1 ���޷��Ƚ�

		iIndex2 = -1;
		lvFinfInfo.flags = LVFI_PARAM;
		lvFinfInfo.lParam = lParam2;
		iIndex2 = SendMessage(hListView,LVM_FINDITEM, iIndex2,(LPARAM)&lvFinfInfo); // �ҵ���Ŀ2 ������
		if (iIndex2 == -1)
			return FALSE;  // û���ҵ���Ŀ2

		if (lpBrowser->iSortSub == 0)
		{  // ������Ŀ����
			lvItem.mask = LVIF_TEXT | LVIF_IMAGE; 
			lvItem.iItem = iIndex1; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText1; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem);  // �õ���Ŀ1 ���ı���ͼ������
			iImage1 = lvItem.iImage; 

			lvItem.mask = LVIF_TEXT | LVIF_IMAGE; 
			lvItem.iItem = iIndex2; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText2; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem);  //�õ���Ŀ2 ���ı� ��ͼ������
			iImage2 = lvItem.iImage; 
			if (iImage1 == TYPE_FOLDER)
			{ // ��Ŀ1 ���ļ���
				if (iImage2 != TYPE_FOLDER) 
					return -1; // ��Ŀ2 ���ļ����趨��Ŀ1 С����Ŀ2
			}
			else
			{ // ��Ŀ1 ���ļ�
				if (iImage2 == TYPE_FOLDER)
					return 1;  // ��Ŀ2 ���ļ��� ���趨��Ŀ1 ������Ŀ2
			}
			return stricmp(lpText1,lpText2);  // �Ƚ�����Ŀ�ı��Ĵ�С
		}
		else
		{
			lvItem.mask = LVIF_TEXT ; 
			lvItem.iItem = iIndex1; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText1; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem); // �õ���Ŀ1 ���ı�

			lvItem.mask = LVIF_TEXT ; 
			lvItem.iItem = iIndex2; 
			lvItem.iSubItem = lpBrowser->iSortSub; 
			lvItem.pszText = lpText2; 
			lvItem.cchTextMax = MAX_PATH; 
			SendMessage(hListView,LVM_GETITEM,0,(LPARAM)&lvItem);  // �õ���Ŀ2 ���ı�

			if (lpBrowser->iSortSub == 1)
			{
				//�������ļ��ĳ���
				int ilen1,ilen2;

					ilen1 = strlen(lpText1);
					ilen2 = strlen(lpText2);
					if (ilen1 > ilen2)
						return 1;  // ���ִ�Խ��������Խ��
					if (ilen1 < ilen2)
						return -1; // ���ִ��̣�����С
			}

			return stricmp(lpText1,lpText2); // �Ƚ��ı���С
		}

		return 0; // ������� ���������е�����
}
// **************************************************
// ������static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ������������ʽ��Ŀѡ��ı��ǵ�֪ͨ��
// ����: 
// **************************************************
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMLISTVIEW pnmv;

		pnmv = (LPNMLISTVIEW)lParam;
		SendNormalNotify(hWnd,EXN_SELCHANGED,pnmv);  // ����֪ͨ��Ϣ��������ѡ����Ŀ�ı�

		return 0;
}
// **************************************************
// ������static LRESULT DoGetListView(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ��������ʽ�ӿ��ƾ��
// �����������õ���ʽ�ӿؼ����������EXM_GETLISTVIEW��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetListView(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView ;

	
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�ӿ��ƾ��

		return (LRESULT)hListView;
}
// **************************************************
// ������static  void SendNormalNotify(HWND hWnd,UINT iCode,LPNMLISTVIEW lpnmListView)
// ������
// 	IN hWnd -- ���ھ��
// 	IN iCode -- ֪ͨ����
// 	IN lpnmListView -- ��ʽ֪ͨ�ṹָ��
// 
// ����ֵ����
// ��������������֪ͨ��Ϣ�������ڡ�
// ����: 
// **************************************************
static  void SendNormalNotify(HWND hWnd,UINT iCode,LPNMLISTVIEW lpnmListView)
{
	HWND hParent;
	HWND hListView;
	NMFILEEXPLORER nmFileExplorer;

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);
		nmFileExplorer.hdr.hwndFrom=hWnd; // ����֪ͨ��Ϣ�Ĵ���
		nmFileExplorer.hdr.idFrom=(UINT)GetWindowLong(hWnd,GWL_ID);  // ֪ͨ���ڵ�ID
		nmFileExplorer.hdr.code=iCode; // ֪ͨ����
		nmFileExplorer.lpnmListView = lpnmListView;
		nmFileExplorer.hListView = hListView; // ��ʽ���
		hParent=GetParent(hWnd);  // �õ������ھ��
		SendMessage(hParent,WM_NOTIFY,(WPARAM)hWnd,(LPARAM)&nmFileExplorer);  // ����֪ͨ��Ϣ
		return ;
}
// **************************************************
// ������static void GetCurrentDirectory(HWND hListView,LPTSTR lpstrDir)
// ������
// 	IN hListView -- ��ʽ���
// 	OUT lpstrDir -- �õ���ǰĿ¼�Ļ���
// 
// ����ֵ����
// ������������ʽ�õ���ǰ��Ŀ¼
// ����: 
// **************************************************
static void GetCurrentDirectory(HWND hListView,LPTSTR lpstrDir)
{
	LPBROWSERDATA lpBrowser;
	HWND hWnd;

	hWnd = GetParent(hListView); // �õ������ھ�����������ľ��
	lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ������Ľṹָ��
	strcpy(lpstrDir,lpBrowser->lpCurDir);  // �õ���ǰ���ļ���
}
// **************************************************
// ������static LRESULT DoSetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR Ҫ���õ��µ�·��
// 
// ����ֵ����
// �������������õ�ǰ·��������EXM_SETCURPATH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoSetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView ;
	char *pNewPath;
	LPBROWSERDATA lpBrowser;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		pNewPath = (LPTSTR)lParam;  // �õ��µĵ�ǰ·��
		if (pNewPath == NULL)  
			return 0; // ��ǰ·��Ϊ��
		if (strlen(pNewPath) >= MAX_PATH)
			return 0; // �Ƿ�·����·�����ȴ���ϵͳҪ��Ҫ��·������󳤶�
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ������Ľṹָ��
		strcpy(lpBrowser->lpCurDir,pNewPath);  // ���õ�ǰ���ļ���
			
		hListView = GetDlgItem(hWnd,ID_LISTVIEW); // �õ���ʽ�Ӿ��
		UpdateListView( hListView ); // ���¸�����ʽ��
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // ֪ͨ�����ڵ�ǰ���ݸı�
		return 0;
}
// **************************************************
// ������static LRESULT DoGetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR Ҫ�õ���·���Ļ���
// 
// ����ֵ����
// ���������� �õ���ǰ·���� ����EXM_GETCURPATH��Ϣ
// ����: 
// **************************************************
static LRESULT DoGetCurPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	char *pNewPath;
	LPBROWSERDATA lpBrowser;

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
		pNewPath = (LPTSTR)lParam;  // �õ��洢·���Ķ������ַ
		if (pNewPath == NULL)
			return 0; // ָ��Ϊ�գ��������� 
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ������Ľṹָ��
		strcpy(pNewPath,lpBrowser->lpCurDir);  // �õ���ǰ���ļ���
		return 0;
}
// **************************************************
// ������static LRESULT DoUpPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// ���������� ����һ��Ŀ¼������EXM_UPPATH��Ϣ��
// ����: 
// **************************************************
static LRESULT DoUpPath(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	LPTSTR lpCurPos;
	HWND hListView ;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ������Ľṹָ��
		if (lpBrowser == NULL)
			return 0; // ָ�����
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�Ӿ��
		if (lpBrowser->bSearch)
		{   // �ǲ�Ѱ���
			lpBrowser->bSearch = FALSE; // �˳���Ѱ
			DeleteListViewColumn(hListView,ID_DIRCOLUMN);// ɾ��Ŀ¼�У�һ����ʽ����Ҫ
			if (lpBrowser->lpSearch)
			{  
				free(lpBrowser->lpSearch);  // �ͷŲ�Ѱ�ִ�
				lpBrowser->lpSearch = NULL; 
			}
			lpBrowser->FindFileFunc = NULL; // ����Ѱ�ص������ÿ�
			goto UPPATHEND; // ��ɲ���
		}
		if (strcmp(lpBrowser->lpCurDir,"\\") ==0 )  // �Ƚϵ�ǰĿ¼�ǲ��Ǹ�Ŀ¼
			return 0; // �����ٵ���һ��Ŀ¼
		lpCurPos = lpBrowser->lpCurDir; // �õ���ǰĿ¼ָ��
		while(*lpCurPos) lpCurPos ++; // ָ�����һ���ַ�
		// ��ʼ��ǰ��ѰĿ¼�ָ�����\\��
		while(1)
		{
			if (lpCurPos == lpBrowser->lpCurDir)
			{  // �Ѿ���Ŀ¼�ĵ�һ���ַ�
				*(lpCurPos+1) = 0;
				goto UPPATHEND;  // ����
			}
			if (*lpCurPos == '\\')
			{  // �ҵ�Ŀ¼�ָ���
				*lpCurPos = 0;
				goto UPPATHEND;  // ����
			}
			lpCurPos --;
		}

UPPATHEND:
		UpdateListView( hListView ); // ������ʽ
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // ֪ͨ�����������Ѿ��ı�
		return 0;
}

// **************************************************
// ������static LRESULT DoDeleteSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ���������� ɾ��ѡ�񣬴���EXM_DELETESEL��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDeleteSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	HWND hListView ;
	LV_ITEM				lvItem;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
	int iIndex;
	BOOL bRet;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������ṹָ��
		if (lpBrowser == NULL)
			return 0; // ����ṹ
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�Ӿ��
		iIndex = -1;

		while(1)
		{
			iIndex = SendMessage(hListView,LVM_GETNEXTITEM, iIndex,LVNI_SELECTED);  // �õ���ʽ�е�ѡ������
			if (iIndex == -1)
				break;  // û��ѡ����Ŀ
			lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
			lvItem.iItem=iIndex;
			lvItem.iSubItem=0;
			lvItem.pszText = lpFileName;  
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView,LVM_GETITEM,(WPARAM)0,(LPARAM)&lvItem);// �õ�ѡ����Ŀ���ļ���

			if (lpBrowser->bSearch == TRUE)
			{  // ���ڲ�Ѱ״̬��
				lvItem.mask  = LVIF_TEXT ;
				lvItem.iItem=iIndex;
				lvItem.iSubItem=3;
				lvItem.pszText = lpFullName;
				lvItem.cchTextMax = MAX_PATH;
				SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem);  // �õ�ѡ����Ŀ���ļ�Ŀ¼
				//_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
	//			strcat(lpFullName,"\\");
	//			strcat(lpFullName,lpFileName);

/*				if (strcmp(lpFullName,"\\") ==0 )
				{
					strcat(lpFullName,lpFileName);
				}
				else
				{
					strcat(lpFullName,"\\");
					strcat(lpFullName,lpFileName);
				}
*/
				if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE)  // ��Ŀ¼���ļ����ϳ�һ��ȫ·���ļ�
					return 0;
				
			}
			else
			{
//				_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
				if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE) // ����ǰ·����õ����ļ����ϳ�һ��ȫ·���ļ�
					return 0;
			}
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (lvItem.iImage == TYPE_FOLDER)
			{  // ָ��ѡ����һ��Ŀ¼
				 //ɾ������Ŀ¼
				bRet = DeleteDirectory(hWnd,lpFullName);
				if (bRet == FALSE)
				{  // ɾ������
					TCHAR lpMessage[64];
					DWORD err = GetLastError(); 
					sprintf(lpMessage," ����ɾ���ļ�(%d) \r\n(%s)",err,lpFullName);
					MessageBox(hWnd,lpMessage,"����",MB_OK);  // ��ʾ����
//					return FALSE;
				}
			}
			else
			{
				// ɾ���ļ�
				// !!! Modified By Jami chen in 2003.09.23
				/*
				bRet = DeleteFile(lpFullName);
				if (bRet == FALSE)
				{
					TCHAR lpMessage[64];
					DWORD dwErr = GetLastError();
					sprintf(lpMessage,"����ɾ���ļ�\r\n(%s)",lpFullName);
					MessageBox(hWnd,lpMessage,"����",MB_OK);
					return FALSE;
				}
				*/
				bRet = DeleteSpecialFile(hWnd,lpFullName);  // ɾ���ļ�
//				if (bRet == FALSE)
//				{
//					return FALSE;
//				}
				// !!! Modified By Jami chen in 2003.09.23
			}
			if (bRet == TRUE)
				SendMessage(hListView,LVM_DELETEITEM,(WPARAM)iIndex,0);  // ɾ����ʽ�ж�Ӧ����Ŀ
			else
				iIndex ++;  //��ǰ�ļ�û��ɾ������ȥɾ����һ���ļ�
		}
		return TRUE;
}
// **************************************************
// ������static BOOL DeleteDirectory(HWND hWnd,LPTSTR lpFullName)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpFullName -- Ҫɾ�����ļ��е�·��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������ɾ��ָ�����ļ��С�
// ����: 
// **************************************************
static BOOL DeleteDirectory(HWND hWnd,LPTSTR lpFullName)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						*lpstrDir;
	BOOL bRet = TRUE;

		lpstrDir = (LPTSTR)malloc(MAX_PATH); // ����һ������ļ��еĻ���

		if (lpstrDir == NULL)
			return FALSE;  // ����ʧ��
/*
		strcpy(lpstrDir,lpFullName);
//		strcat(lpstrDir,"\\*.*");  // Ҫ�������е��ļ�
		if (strcmp(lpstrDir,"\\") == 0)
		{
			strcat(lpstrDir,"*.*");
		}
		else
		{
			strcat(lpstrDir,"\\*.*");
		}
*/
		if (MergeFullFileName(lpstrDir,lpFullName,"*.*") == FALSE)  // �ϳ�ָ�����ļ�
			goto DELETE_END; // �ϳ�ʧ��
		hFind=FindFirstFile(lpstrDir, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// �ļ������ڣ����ļ������Ѿ�û���ļ�
			bRet =RemoveDirectory(lpFullName); // ɾ��ָ���ļ���
			goto DELETE_END;
		}
		while(TRUE)
		{  // �ļ����ڣ�����Ҫɾ���ļ����µ��ļ�
			if(FindFileData.cFileName[0]=='.')
			{  // ���ļ���һ����ʾ��ǰĿ¼����Ŀ¼���ļ���������
			}
			else
			{
//				strcpy(lpstrDir,lpFullName);
//				strcat(lpstrDir,"\\");
//				strcat(lpstrDir,FindFileData.cFileName);  
				if (MergeFullFileName(lpstrDir,lpFullName,FindFileData.cFileName) == FALSE) // ���Ҫɾ�����ļ���
					goto DELETE_END;
				if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{
					// ���ҵ�����һ��Ŀ¼
					bRet = DeleteDirectory(hWnd,lpstrDir);  // ɾ����Ŀ¼
				}
				else
				{
					// ���ҵ�����һ���ļ�
					// !!! Modified By Jami chen in 2003.09.23
					/*
					bRet = DeleteFile(lpstrDir);
					if (bRet == FALSE)
					{
						TCHAR lpMessage[64];
						sprintf(lpMessage,"����ɾ���ļ�\r\n(%s)",lpstrDir);
						MessageBox(hWnd,lpMessage,"����",MB_OK);
						goto DELETE_END;
					}
					*/
					bRet = DeleteSpecialFile(hWnd,lpstrDir); // ɾ��ָ�����ļ�
					if (bRet == FALSE)
					{  // ɾ��ʧ��
						goto DELETE_END;
					}
					// !!! Modified End By Jami chen in 2003.09.23
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //������һ���ļ�
			{  // �Ѿ�û���ļ�
				FindClose(hFind); // �رղ�Ѱ���
//				free(lpstrDir);
				bRet =RemoveDirectory(lpFullName);  // ɾ����ǰĿ¼
				goto DELETE_END;
			}
		}

DELETE_END:
		if (lpstrDir)
			free(lpstrDir); // �ͷŷ�����ڴ�
		return bRet; // ���ؽ��
}
// **************************************************
// ������static LRESULT DoEndEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ��ʽ���
// 	IN lParam -- LPNMLVDISPINFO �ṹָ�롣
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ���������������޸ı�ǩ��
// ����: 
// **************************************************
static LRESULT DoEndEditLabel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPNMLVDISPINFO lpnmLVDispInfo;
	TCHAR lpOldFullName[MAX_PATH],lpNewFullName[MAX_PATH],lpCurDir[MAX_PATH];
	LVITEM lvItem;
	LPBROWSERDATA lpBrowser;
	BOOL bRet;
	HWND hListView;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == NULL)
			return FALSE;  // ��������

		lpnmLVDispInfo = (LPNMLVDISPINFO)lParam; // �õ����ݽṹָ��
		if (lpnmLVDispInfo == NULL)
			return FALSE;  // ��������

		SendNormalNotify(hWnd,EXN_ENDRENAME,NULL);  // ֪ͨ�����Ѿ����������

		hListView = (HWND)wParam;  // �õ���ʽ�Ӿ��
		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=lpnmLVDispInfo->item.iItem;
		lvItem.iSubItem=0;
		lvItem.pszText = lpNewFullName;
		lvItem.cchTextMax = MAX_PATH;
		SendMessage(hListView,LVM_GETITEMTEXT,(WPARAM)lpnmLVDispInfo->item.iItem,(LPARAM)&lvItem); //�õ�ָ����Ŀ���ļ���
		if (stricmp(lpNewFullName,lpnmLVDispInfo->item.pszText) == 0)
		{// Դ�ļ���Ŀ���ļ�һ�£�����Ҫ��������
			return TRUE;
		}
		if (lpBrowser->bSearch == TRUE)
		{ // �ڲ�Ѱ״̬
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=lpnmLVDispInfo->item.iItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpCurDir;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);  // �õ�ָ���ļ���·��
			//_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
//			strcat(lpFullName,"\\");
//			strcat(lpFullName,lpFileName);
/*			if (strcmp(lpCurDir,"\\") ==0 )
			{
			}
			else
			{
				strcat(lpCurDir,"\\");
			}
*/
//			strcpy(lpNewFullName,lpOldFullName);
//			strcat(lpCurDir,lpNewFullName);
//			strcat(lpCurDir,lpnmLVDispInfo->item.pszText);
//			_makepath( lpOldFullName, NULL, lpCurDir, lpNewFullName, NULL);
//			_makepath( lpNewFullName, NULL, lpCurDir, lpnmLVDispInfo->item.pszText, NULL);
			if (MergeFullFileName(lpOldFullName,lpCurDir,lpNewFullName) == FALSE)  // ��Ͼ��ļ�
				return FALSE;
			if (MergeFullFileName(lpNewFullName,lpCurDir,lpnmLVDispInfo->item.pszText) == FALSE) // ��ϳ����ļ�
				return FALSE;

		}
		else
		{
//			_makepath( lpOldFullName, NULL, lpBrowser->lpCurDir, lpNewFullName, NULL);
//			_makepath( lpNewFullName, NULL, lpBrowser->lpCurDir, lpnmLVDispInfo->item.pszText, NULL);
			if (MergeFullFileName(lpOldFullName,lpBrowser->lpCurDir,lpNewFullName) == FALSE)  // ��Ͼ��ļ�
				return FALSE;
			if (MergeFullFileName(lpNewFullName,lpBrowser->lpCurDir,lpnmLVDispInfo->item.pszText) == FALSE)  // ������ļ�
				return FALSE;
		}
		//bRet = DeleteAndRenameFile(lpNewFullName, lpOldFullName);
		bRet = MoveFile(lpOldFullName,lpNewFullName); // �������ļ�
		if (bRet == FALSE)
		{ // �������ļ�ʧ��
			TCHAR lpMessage[64];
			DWORD err = GetLastError(); 

				sprintf(lpMessage,"�����������ļ�(%d)\r\n(%s)",err,lpOldFullName);
				MessageBox(hListView,lpMessage,"����",MB_OK);
		}
		return bRet;
}
// **************************************************
// ������static BOOL SearchDirectory(HWND hWnd,HWND hListView,LPTSTR lpCurDir)
// ������
// 	IN hWnd -- �������
// 	IN hListView -- ��ʽ���
// 	IN lpCurDir -- Ҫ���ҵ�Ŀ¼
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �������������ҵ�ǰĿ¼�µ������ļ������������뵽��ʽ��
// ����: 
// **************************************************
static BOOL SearchDirectory(HWND hWnd,HWND hListView,LPTSTR lpCurDir)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	TCHAR						*lpstrDir;
	LPBROWSERDATA lpBrowser;
// Add By Jami chen in 2003.09.09
	DWORD unViewAttributes = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ROMMODULE;
// Add End By Jami chen in 2003.09.09

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0); // �õ������ṹָ��
		if (lpBrowser == 0)
			return FALSE; // ���ݴ���

		lpstrDir = (LPTSTR)malloc(MAX_PATH+1);  // ����һ�������ļ���ָ��
		if (lpstrDir == NULL)
			return FALSE;  // ����ʧ��
/*		strcpy(lpstrDir,lpCurDir);
//		strcat(lpstrDir,"\\*.*");  // Ҫ�������е��ļ�
		if (strcmp(lpstrDir,"\\") == 0)
		{
			strcat(lpstrDir,"*.*");
		}
		else
		{
			strcat(lpstrDir,"\\*.*");
		}
*/
		if (MergeFullFileName(lpstrDir,lpCurDir,"*.*") == FALSE) //����ļ�
		{ // ����ļ�ʧ��
			free(lpstrDir); // �ͷ��ڴ�
			return FALSE;
		}
//		RETAILMSG(1,(" Will Search  File < %s >\r\n" , lpstrDir));
		hFind=FindFirstFile(lpstrDir, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// �ļ�������
			return TRUE;
		}
		while(TRUE)
		{  // �ļ�����
			if(FindFileData.cFileName[0]=='.')
			{  // ���ļ���һ����ʾ��ǰĿ¼����Ŀ¼���ļ���������
			}
			else
			{
				// !!! Add By Jami chen in 2003.09.09
				if (!( FindFileData.dwFileAttributes&unViewAttributes ))
				// !!! Add End By Jami chen in 2003.09.09
				{
					if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
					{
						// ���ҵ�����һ��Ŀ¼
						LPTSTR lpSearch;
						DWORD dwlen;

	//					RETAILMSG(1,("  Find a Directory <%s>\r\n" , FindFileData.cFileName));
						lpSearch = lpBrowser->lpSearch; // �õ������ִ�
						while(1)
						{
							GetSearchString(&lpSearch,&dwlen); //�õ�һ�������ִ�
							if (dwlen == 0)
								break;
	//						if (FileNameCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)
							if (SearchCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)// ��Ŀ¼�Ƚ�
							{  // Ŀ¼��ƥ��
								lpSearch += dwlen;
								continue;
							}
							// Ŀ¼ƥ��
							if (CallBackFindFile(hWnd,FindFileData,lpCurDir) != FALSE) // ����Ӧ�ó���ص�
								InsertDirectoryItem(hListView,FindFileData,lpCurDir); // ���Բ��뵱ǰĿ¼
							break;
						}

	/*					strcpy(lpstrDir,lpCurDir);
						if (strcmp(lpstrDir,"\\"))
							strcat(lpstrDir,"\\");
						strcat(lpstrDir,FindFileData.cFileName);  
	*/
						if (MergeFullFileName(lpstrDir,lpCurDir,FindFileData.cFileName) == FALSE) // ��ϳ��µ�Ŀ¼
						{
							free(lpstrDir);
							return FALSE;
						}
						SearchDirectory(hWnd,hListView,lpstrDir);  // ������Ŀ¼
					}
					else
					{
						// ���ҵ�����һ���ļ�
	//					if (StrAsteriskCmp(lpBrowser->lpSearch,FindFileData.cFileName) == 0)
						LPTSTR lpSearch;
						DWORD dwlen;

	//					RETAILMSG(1,("  Find a File <%s>\r\n" , FindFileData.cFileName));
						lpSearch = lpBrowser->lpSearch; // �õ���Ѱ��
						while(1)
						{
							GetSearchString(&lpSearch,&dwlen);  // �õ�һ����Ѱ��
							if (dwlen == 0)
								break;  // ��Ѱ������
	//						if (FileNameCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE)
							if (SearchCompare( lpSearch, dwlen, FindFileData.cFileName, strlen(FindFileData.cFileName)) == FALSE) // ���ļ��Ƚ�
							{  // ��ƥ��
								lpSearch += dwlen; // ָ����һ����Ѱ��
								continue;
							}
	//						RETAILMSG(1,("  the File is need , will ask caller\r\n" ));
							if (CallBackFindFile(hWnd,FindFileData,lpCurDir) != FALSE) // ���лص�
							{
	//							RETAILMSG(1,("   caller need , will insert it\r\n" ));
								InsertFileItem(hListView,FindFileData,lpCurDir);  // ���Բ�����ļ�
							}
							break;
						}
					}
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //������һ���ļ�
			{ // �Ѿ�û���ļ���
				break;
			}
		}
		free(lpstrDir);  // �ͷ��ڴ�
		FindClose(hFind); // ������Ѱ
		return TRUE;
}


// **************************************************
// ������static LRESULT DoFindFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- FindFile ���ҵ��ļ���Ļص�����
// 	IN lParam -- LPTSTR Ҫ���ҵ��ַ���
// 
// ����ֵ��
// ���������������ļ�������EXM_FINDFILE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoFindFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	LPTSTR lpFindString;
	HWND hListView;

//		RETAILMSG(1,("!!! Find File !!!!\r\n"));
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������ṹָ��
		if (lpBrowser == 0)
			return 0; // ��������
#ifdef _MAPPOINTER
			wParam = (WPARAM)MapPtrToProcess( (LPVOID)wParam, GetCallerProcess() );  // ӳ��ָ��
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
		lpFindString = (LPTSTR)lParam; // Ҫ���ҵ��ַ���
//		RETAILMSG(1,(" The Search String is <%s>, <%X>\r\n",lpFindString,lpFindString));
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�ӵľ��
		if (lpFindString == NULL)
		{  // �����ִ�Ϊ��
			if (lpBrowser->bSearch == TRUE)
			{ // ԭ�����ڲ�Ѱ״̬���˳���Ѱ״̬
				lpBrowser->bSearch = FALSE;
				DeleteListViewColumn(hListView,ID_DIRCOLUMN);  // ɾ��Ŀ¼��
				if (lpBrowser->lpSearch)
				{
					free(lpBrowser->lpSearch); // �ͷŲ����ִ�
					lpBrowser->lpSearch = NULL;
				}
				lpBrowser->FindFileFunc = NULL;  // ���ûص�����Ϊ��
				UpdateListView( hListView ); // ������ʽ
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // ֪ͨ�����Ѿ��ı�
			}
			return 0; // ����
		}
		if (lpBrowser->bSearch == TRUE)
		{  // ԭ�����ڲ�Ѱ״̬�����ԭ��������
			if (lpBrowser->lpSearch)
			{
				free(lpBrowser->lpSearch);  // �ͷ�ԭ���Ĳ����ִ�
				lpBrowser->lpSearch = NULL;
			}
			lpBrowser->FindFileFunc = NULL;  // ���ûص�����Ϊ��
		}
		else
		{   
			// ������ڲ�Ѱ���棬����Ҫ����Ŀ¼����Ŀ
			CreateListViewColumn(hListView,ID_DIRCOLUMN);  
		}
		lpBrowser->bSearch = TRUE;  // ���ý����Ѱ״̬
		lpBrowser->FindFileFunc = (FindFile)wParam; // ���ûص�����ָ��
		lpBrowser->lpSearch = (LPTSTR)malloc(strlen(lpFindString)+1); // ����ռ��������µĲ�ѯ�ִ�
		strcpy(lpBrowser->lpSearch,lpFindString);  // ���Ʋ�Ѱ�ִ�������
		UpdateListView( hListView ); // ������ʽ
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // ֪ͨ�����Ѿ�����
		return 0;
}

// **************************************************
// ������static void GetSearchString(LPTSTR *lpSearch,DWORD *pdwLen)
// ������
// 	IN/OUT lpSearch --��ǰ��Ѱ�ִ���ָ�룬�����ز�Ѱ�ִ��Ŀ�ʼλ��
// 	OUT pdwLen -- ���ز�Ѱ�ִ��ĳ���
// 
// ����ֵ����
// �����������õ�һ����Ѱ�ִ���
// ����: 
// **************************************************
static void GetSearchString(LPTSTR *lpSearch,DWORD *pdwLen)
{
	LPTSTR lpCurPos;
	DWORD dwLen;

		lpCurPos = *lpSearch;
		dwLen = 0;

		// ������ʼ��' '��';'�������ǲ�Ѱ�ִ��ķָ���
		while(*lpCurPos)
		{
			if (*lpCurPos == ' ' || *lpCurPos == ';')
			{
				lpCurPos ++;
				continue;
			}
			break;
		}

		*lpSearch = lpCurPos; // �������ò�Ѱ�ִ���λ��

		// �õ���Ѱ�ִ��ĳ��ȣ�����' '��';'����
		while(*lpCurPos)
		{
			if (*lpCurPos == ' ' || *lpCurPos == ';')
				break;
			lpCurPos ++;
			dwLen ++;
		}
		*pdwLen = dwLen;  // �����ִ�����

}
// **************************************************
// ������static LRESULT DoCopy(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ������������ѡ����Ŀ���ļ����浽���а塣
// ����: 
// **************************************************
static LRESULT DoCopy(HWND hWnd)
{
	HWND hListView;
	int iIndex ;
	LVITEM lvItem;
	char lpFileName[MAX_PATH];
	DWORD *lpData;
	LPFILECOPYDATA lpFileCopyData = NULL;
	int nSelCount, nNumber = 0;
	LPBROWSERDATA lpBrowser;
	DWORD dwDataLen;
	HGLOBAL hData;

	
//		RETAILMSG(1,("Enter Copy Proc ... \r\n"));
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��
		if (lpBrowser == 0)
			return 0; // ��������

//		RETAILMSG(1,("Get Copy Data ... \r\n"));
		lpBrowser->bCut = FALSE; 

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���
		nSelCount = SendMessage(hListView,LVM_GETSELECTEDCOUNT,0,0);  // �õ���ǰѡ����Ŀ����Ŀ

		dwDataLen = nSelCount * sizeof (FILECOPYDATA) + sizeof(DWORD);// ������Ҫ�Ĵ洢�ռ�
		lpData = (DWORD *)malloc(dwDataLen);  // �����ڴ�
		if (lpData == NULL)
			return FALSE; // ����ʧ��
		*lpData = nSelCount; // �ȱ���ѡ����Ŀ�ĸ���
		lpFileCopyData = (LPFILECOPYDATA )(lpData+1);

		iIndex = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);  // �õ���һ��ѡ����Ŀ������

		while(1)
		{
			if (iIndex == -1)  
				break; //�Ѿ�û��ѡ����Ŀ
//			RETAILMSG(1,("Get a File ...\r\n"));
			lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			lvItem.iItem = iIndex;
			lvItem.iSubItem = 0;
			lvItem.pszText = lpFileName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView, LVM_GETITEM, 0, (LPARAM)&lvItem);  // �õ�ѡ����Ŀ���ļ���

			if (lpBrowser->bSearch == TRUE)
			{
				// �ڲ�Ѱ״̬
				lvItem.mask  = LVIF_TEXT ;
				lvItem.iItem=iIndex;
				lvItem.iSubItem=3;
				lvItem.pszText = lpFileCopyData[nNumber].lpFileName;
				lvItem.cchTextMax = MAX_PATH;
				SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem); // �õ�ָ���ļ���Ŀ¼

/*				if (strcmp(lpFileCopyData[nNumber].lpFileName,"\\") ==0 )
				{
					strcat(lpFileCopyData[nNumber].lpFileName,lpFileName);
				}
				else
				{
					strcat(lpFileCopyData[nNumber].lpFileName,"\\");
					strcat(lpFileCopyData[nNumber].lpFileName,lpFileName);
				}
*/
				if (MergeFullFileName(lpFileCopyData[nNumber].lpFileName,NULL,lpFileName) == FALSE) // ��ϳ�ȫ·���ļ���
				{ // ���ʧ��
					free(lpData);  // �ͷ��ڴ�
//					RETAILMSG(1,("Get Copy Data Failure \r\n"));
					return FALSE;
				}
			}
			else
			{
//				_makepath( lpFileCopyData[nNumber].lpFileName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
				if (MergeFullFileName(lpFileCopyData[nNumber].lpFileName,lpBrowser->lpCurDir,lpFileName) == FALSE) // ��ϳ�ȫ·���ļ���
				{ // ���ʧ��
					free(lpData); // �ͷ��ڴ�
					RETAILMSG(1,("Get Copy Data Failure \r\n"));
					return FALSE;
				}
			}

			// �����ļ�����
			if (lvItem.iImage == TYPE_FOLDER)
				lpFileCopyData[nNumber].uFileTYPE  = TYPE_FOLDER;
			else
				lpFileCopyData[nNumber].uFileTYPE  = ~TYPE_FOLDER;
			iIndex = SendMessage(hListView, LVM_GETNEXTITEM, iIndex+1, LVNI_SELECTED); // �õ���һ��ѡ���ļ�
			RETAILMSG(1,("Get a File Over\r\n"));
			nNumber ++;
			if (nNumber >= nSelCount)
				break; // �Ѿ��õ��㹻��ѡ���ļ�
		}

//		RETAILMSG(1,("Get Copy Data Success \r\n"));
//		RETAILMSG(1,("Set Copy Data to clipboard ... \r\n"));
		if (nNumber)
		{
			// ��ѡ���ļ�
//			OpenClipboard(hWnd);
//			SetClipboardData(CF_FILE,lpData);
//			CloseClipboard();
			if (OpenClipboard(hWnd))  // �򿪼��а�
			{
				EmptyClipboard();  // ��ռ��а�
				hData = (HGLOBAL)GlobalAlloc(GMEM_MOVEABLE , dwDataLen); // ����һ��ȫ���ڴ�
				if (hData != NULL)
				{
					LPSTR pszData = (LPSTR) GlobalLock(hData); // �õ�ȫ���ڴ��ַ
					if (pszData)
					{
						memcpy(pszData,lpData,dwDataLen); // ����ǰ�õ������ݸ��Ƶ�ȫ���ڴ�
						GlobalUnlock(hData);
						SetClipboardData(CF_FILE, hData);  // ���õ�ǰ���ݵ����а�
					}
				}

				CloseClipboard(); // �رռ��а�
			}
		}

//		RETAILMSG(1,("Set Copy Data to clipboard OK \r\n"));
//		RETAILMSG(1,("Copy Proc success\r\n"));
		return 0;

}
// **************************************************
// ������static LRESULT DoPaste(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������Ӽ��а���ճ���ļ�����ǰ��·����
// ����: 
// **************************************************
static LRESULT DoPaste(HWND hWnd)
{
	DWORD *lpData;
	LPFILECOPYDATA lpFileCopyData;
	int nCount ;
	char lpOldFileName[MAX_PATH],lpNewFileName[MAX_PATH];
	int i;
	LPBROWSERDATA lpBrowser;
	HWND hListView;
	BOOL bRet = FALSE;
	LPTSTR lpBackupFileName = NULL;
	int bCover = 0;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == 0)
			return 0; // ���ݴ���

//		OpenClipboard(hWnd);
//		lpData = GetClipboardData(CF_FILE);
//		CloseClipboard();
		if (OpenClipboard(hWnd))  // �򿪼��а�
		{
			HGLOBAL hData = GetClipboardData(CF_FILE);  // �õ��ļ�����
			if (hData != NULL)
			{
				lpData = (DWORD *) GlobalLock(hData); // �õ�����ָ��
				if (lpData != NULL)
				{

					ShowCopyDialog(hWnd);  // ��ʾ���ڸ��ƵĶԻ���
					nCount = *lpData;
					lpFileCopyData = (LPFILECOPYDATA )(lpData+1);  // �õ��ļ��������ݽṹָ��
					

					for (i=0; i< nCount;i++)
					{
			//			_splitpath(lpFileCopyData[i].lpFileName,NULL,NULL,lpOldFileName,NULL);
						GetPureFileName(lpFileCopyData[i].lpFileName,lpOldFileName); // �õ����ļ���
			//			_makepath(lpNewFileName,NULL,lpBrowser->lpCurDir,lpOldFileName,NULL);
						if (MergeFullFileName(lpNewFileName,lpBrowser->lpCurDir,lpOldFileName) == FALSE) // ��ϵ�ǰ·��
						{ // ���ʧ�ܣ�����ճ���ļ�
//							MessageBox(hWnd,"�����ļ�ʧ��","����",MB_OK);
							MessageBox(lpBrowser->hCopyWindow,"�����ļ�ʧ��","����",MB_OK);
							
							break;
						}
						if (lpFileCopyData[i].uFileTYPE == TYPE_FOLDER)
						{ // ��һ���ļ���
							bRet = CopyFolder(hWnd,lpFileCopyData[i].lpFileName,lpNewFileName,FALSE,FALSE); // �����ļ���
							if (lpBrowser->bCut == TRUE && bRet == TRUE)
							{
								// ��ǰ�Ǽ���״̬��������ɺ�Ҫɾ��ԭ�����ļ����ļ���
								bRet = DeleteDirectory(hWnd,lpFileCopyData[i].lpFileName);
								if (bRet == FALSE)
									break;
							}
						}
						else
						{
							if (strcmp(lpFileCopyData[i].lpFileName, lpNewFileName) == 0)
							{
								// Դ�ļ���Ŀ���ļ�����ͬ���򱸷ݵ�ǰ�ļ�
								// ���ı�Ŀ���ļ���
								lpBackupFileName = BackupFileName(lpFileCopyData[i].lpFileName); // �õ������ļ���
								if (lpBackupFileName == NULL)
								{  // �õ��ļ���ʧ��
					//				MessageBox(hWnd,"�����ļ�ʧ��","����",MB_OK);
									bRet = FALSE;
								}
								else
								{  // �����ļ��������ļ���
									bRet = CopyFile(lpFileCopyData[i].lpFileName,lpBackupFileName,FALSE);
									free(lpBackupFileName); // �ͷ��ڴ�
									if (lpBrowser->bCut == TRUE && bRet)
									{
										// ��ǰ�Ǽ���״̬��������ɺ�Ҫɾ��ԭ�����ļ����ļ���
										bRet = DeleteSpecialFile(hWnd,lpFileCopyData[i].lpFileName); // ɾ��ָ�����ļ�
										if (bRet == FALSE)
											break;
									}
								}
							}
							else
							{
								// Ŀ¼��ͬ�����ļ����ƹ���
								if (IsExist(lpNewFileName))
								{ // �ļ��Ѿ�����
//									if (MessageBox(hWnd,"�ļ��Ѿ�����,�Ƿ񸲸�?","��ʾ",MB_YESNO) == IDNO)
									if (MessageBox(lpBrowser->hCopyWindow,"�ļ��Ѿ�����,�Ƿ񸲸�?","��ʾ",MB_YESNO) == IDNO)
									{ // �����ǣ��˳�
										break;
									}
								}
								//RETAILMSG(1,("123456\r\n"));
								bRet = CopyFile(lpFileCopyData[i].lpFileName,lpNewFileName,FALSE); // �����ļ�
								if (lpBrowser->bCut == TRUE && bRet)
								{
									// ��ǰ�Ǽ���״̬��������ɺ�Ҫɾ��ԭ�����ļ����ļ���
									bRet = DeleteSpecialFile(hWnd,lpFileCopyData[i].lpFileName); // ɾ��ָ�����ļ�
									if (bRet == FALSE)
										break;
								}
								//RETAILMSG(1,("bRet = %d\r\n",bRet));
							}
						}
						if (bRet == FALSE)
						{  // �����ļ�ʧ��
							DWORD err = GetLastError(); 
							TCHAR lpMessage[64];

							sprintf(lpMessage," �����ļ�ʧ��(%d) ",err);
//							MessageBox(hWnd,lpMessage,"����",MB_OK);
							MessageBox(lpBrowser->hCopyWindow,lpMessage,"����",MB_OK);
							break;
						}
					}

					hListView = GetDlgItem(hWnd,ID_LISTVIEW); // �õ���ʽ���
					UpdateListView(hListView); // ������ʽ
					SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // ֪ͨ�����Ѿ��ı�

					GlobalUnlock(hData); 
					CloseCopyDialog(hWnd);  // �رո��ƶԻ���
				}
			}
			if (lpBrowser->bCut)
			{
				//�Ǽ���״̬����ɺ����
				lpBrowser->bCut = FALSE; 
				SetClipboardData(CF_FILE,NULL);
			}
			CloseClipboard();// �رռ��а�
		}
		return 0;
}
// **************************************************
// ������static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ��������������һ���µ�Ŀ¼������EXM_NEWFOLDER��Ϣ��
// ����: 
// **************************************************
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	HWND hListView;
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	int iIndex = 1;
	char lpFileName[MAX_PATH];
	int iItem;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������Ľṹָ��
		if (lpBrowser == 0)
			return FALSE; // ָ�����
		if (lpBrowser->bSearch)
			return FALSE; // �ڲ�Ѱ״̬�²��ܴ�����Ŀ¼
		while(1)
		{
			if (strcmp(lpBrowser->lpCurDir,"\\") == 0)  // ��ǰĿ¼�Ǹ�Ŀ¼
				sprintf(lpFileName,"%sFolder%d",lpBrowser->lpCurDir,iIndex); // �õ�Ҫ�½���Ŀ¼��
			else
				sprintf(lpFileName,"%s\\Folder%d",lpBrowser->lpCurDir,iIndex);  // �õ�Ҫ�½���Ŀ¼��
			hFind=FindFirstFile(lpFileName, &FindFileData  );  // ���ҵ�һ���ļ�
			if( hFind == INVALID_HANDLE_VALUE) 
			{// �ļ������ڣ����Դ���
				break;
			}
			// ���ļ��Ѿ�����
			FindClose(hFind);  
			iIndex ++; // �õ��µ�����������Ŀ¼��
		}
		CreateDirectory(lpFileName,NULL);  // ����Ŀ¼
		hListView = GetDlgItem(hWnd,ID_LISTVIEW); // �õ���ʽ���
//		UpdateListView(hListView); // ������ʽ
		{			
			hFind=FindFirstFile(lpFileName, &FindFileData  );  // ���ҵ�һ���ļ�
			if( hFind == INVALID_HANDLE_VALUE) 
			{// 
				return FALSE;
			}
			iItem = InsertDirectoryItem(hListView,FindFileData,NULL);  // ����ǰĿ¼���뵽��ʽ
			FindClose(hFind);

			if (iItem >= 0)
			{
				// ����Ŀ����
				SendMessage(hListView,LVM_SETHOTITEM,iItem,0);  // ���õ�һ����ĿΪ������Ŀ
				SendMessage(hListView,LVM_SETSELECTIONMARK,0,iItem);  // ���õ�һ����ĿΪѡ����Ŀ
			}
		}
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL); // ֪ͨ�����Ѿ��ı�
		return TRUE;  // ���سɹ�
}
// **************************************************
// ������static LRESULT DoIsFind(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ�����ص�ǰ�Ĳ�Ѱ״̬���ǲ�Ѱ״̬������TRUE�����򷵻�FALSE��
// �����������Ƿ��ڲ�Ѱ״̬������EXM_ISFIND��Ϣ��
// ����: 
// **************************************************
static LRESULT DoIsFind(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ�����ṹָ��
		if (lpBrowser == 0)
			return FALSE;
		return lpBrowser->bSearch;  // ���ص�ǰ�Ĳ�Ѱ״̬
}
// **************************************************
// ������static LRESULT DoGetSelFileName(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- int Ҫ�õ����ļ�����Ĵ�С��������ҪMAX_PATH
// 	IN lParam -- LPTSTR ����ļ����Ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ��ļ��������� EXM_GETSELFILENAME��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetSelFileName(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	LPTSTR lpFullName;
	TCHAR lpFileName[MAX_PATH];
	int iIndex;
	LVITEM lvItem;
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��
		if (lpBrowser == 0)
			return FALSE; // ���ݴ���

#ifdef _MAPPOINTER
			lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );  // ӳ��ָ��
#endif
		lpFullName = (LPTSTR)lParam;  // �õ��ļ������ָ��
		if (lpFullName == NULL)
			return FALSE; // �����ַΪ��
		if (wParam < MAX_PATH)
			return FALSE; // �����С����

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�Ӿ��

		iIndex = SendMessage(hListView, LVM_GETNEXTITEM, -1, LVNI_SELECTED);  // �õ���ǰ��ѡ����Ŀ����
		if (iIndex == -1)
			return FALSE;  // ��ǰû��ѡ����Ŀ

		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=iIndex;
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
		SendMessage(hListView,LVM_GETITEM,(WPARAM)0,(LPARAM)&lvItem);  // �õ�ѡ����Ŀ���ļ���

		if (lvItem.iImage == TYPE_FOLDER)
			return FALSE; // ��ǰѡ����һ��Ŀ¼

		if (lpBrowser->bSearch == TRUE)
		{  // ��ǰ�ڲ�Ѱ״̬��
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=iIndex;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem);  // �õ��ļ�Ŀ¼
/*			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}
*/
			if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE)  // ����ļ�����Ŀ¼
			{
				return FALSE;  // ���ʧ��
			}
		}
		else
		{
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)  // ����ļ����͵�ǰĿ¼
			{
				return FALSE;  // ���ʧ��
			}
		}
		return TRUE;
}

// **************************************************
// ������static LRESULT DoGetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- int Ҫ��������ִ��Ļ���Ĵ�С
// 	IN lParam -- LPTATR ��������ִ��Ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ����˴�������EXM_GETFILTER��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpNewFilter;
	LPBROWSERDATA lpBrowser;
	UINT cchMax ;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��
		if (lpBrowser == 0)
			return FALSE;  // ���ݴ���

#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		cchMax = (UINT)wParam;  // �õ���������ߴ�
		lpNewFilter = (LPTSTR)lParam;  // �õ�����ĵ�ַ
		if (lpNewFilter == NULL)
			return FALSE;  // �����ַΪ��
		if (lpBrowser->lpFilter)
		{ // ��ǰ���ڹ����ִ�
			if (cchMax <= strlen(lpBrowser->lpFilter))  // �Ƚϻ����Ƿ��㹻
				return FALSE; // �ߴ�̫С
			strcpy(lpNewFilter,lpBrowser->lpFilter);  // ���ƹ��˴�
		}
		else
		{
			strcpy(lpNewFilter,"");  // ���ƿ��ִ�
		}
		return TRUE;  // ���سɹ�
}
// **************************************************
// ������static LRESULT DoSetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR Ҫ���õĹ����ִ�
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// �������������ù��˴������� EXM_SETFILTER ��Ϣ
// ����: 
// **************************************************
static LRESULT DoSetFilter(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpNewFilter;
	LPBROWSERDATA lpBrowser;
	HWND hListView ;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������Ľṹָ��
		if (lpBrowser == 0)
			return FALSE; // ���ݴ���

#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		lpNewFilter = (LPTSTR)lParam;  // �õ������ִ�
		if (lpBrowser->lpFilter)
		{ // ��ǰ�Ѿ��й����ִ�
			free(lpBrowser->lpFilter);  // �ͷ�ԭ���Ĺ����ִ�
			lpBrowser->lpFilter = NULL;
		}
		if (lpNewFilter)
		{ // �µĹ����ִ�����
			lpBrowser->lpFilter = (LPTSTR)malloc(strlen(lpNewFilter)+1); // �������ִ��Ŀռ�
			strcpy(lpBrowser->lpFilter,lpNewFilter); // �����µĹ����ִ�
		}
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�Ӿ��

		UpdateListView( hListView );  // ������ʽ
		SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // ֪ͨ�����Ѿ��ı�
		return TRUE;
}
// **************************************************
// ������static LRESULT DoGetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd --���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPFILEPROPERTY ���Խṹָ��
// 
// ����ֵ��
// �����������õ���ǰ���ԣ����� EXM_GETPROPERTY ��Ϣ��
// ����: 
// **************************************************
static LRESULT DoGetProperty(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPBROWSERDATA lpBrowser;
	LPFILEPROPERTY lpFileProperty;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
//	DWORD dwAttributes;
	DWORD dwFreeBytesAvailable,dwTotalNumberOfBytes,dwTotalNumberOfFreeBytes;
	int iIndex;
	LVITEM lvItem;
	HWND hListView;
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	DWORD dwSelCount;
	UINT nFolderNum = 0,nFileNum = 0;


		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��
		if (lpBrowser == 0)
			return FALSE;  // ���ݴ���

#ifdef _MAPPOINTER
		lParam = (LPARAM)MapPtrToProcess( (LPVOID)lParam, GetCallerProcess() );   // ӳ��ָ��
#endif
		lpFileProperty = (LPFILEPROPERTY)lParam;  // �õ����Խṹָ��

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);

		dwSelCount = SendMessage(hListView, LVM_GETSELECTEDCOUNT, 0, 0); // �õ���ǰ��ѡ�����
		lpFileProperty->dwTotalSize = 0;
		iIndex = -1;
		if (dwSelCount)
		{  // ��ǰ��ѡ��
			while(1)
			{
				iIndex = SendMessage(hListView, LVM_GETNEXTITEM, iIndex, LVNI_SELECTED);  // �õ���ǰѡ�������
				if (iIndex == -1)
				{ // �Ѿ�û��ѡ����
					if (dwSelCount > 1)  
					{  // ��ǰ��ѡ���������1
						sprintf(lpFileProperty->lpName,"%d���ļ���,%d���ļ�",nFolderNum,nFileNum); //���õ�ǰ��ѡ�������ִ�
//						strcpy(lpFileProperty->lpName,lpFileName);
						lpFileProperty->dwFileAttributes = 0;  // û���ļ�����
						return TRUE;
					}
					return FALSE;
				}

				lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
				lvItem.iItem=iIndex;
				lvItem.iSubItem=0;
				lvItem.pszText = lpFileName;
				lvItem.cchTextMax = MAX_PATH;
				SendMessage(hListView,LVM_GETITEM,(WPARAM)0,(LPARAM)&lvItem);  // �õ���ǰѡ����ļ���

				if (lpBrowser->bSearch == TRUE)
				{  // ��ǰ���ڲ�Ѱ״̬
					lvItem.mask  = LVIF_TEXT ;
					lvItem.iItem=iIndex;
					lvItem.iSubItem=3;
					lvItem.pszText = lpFullName;
					lvItem.cchTextMax = MAX_PATH;
					SendMessage(hListView,LVM_GETITEM,(WPARAM)iIndex,(LPARAM)&lvItem);  // �õ�ѡ����Ŀ��Ŀ¼

					strcpy(lpFileProperty->lpPath,lpFullName);  // ���Ƶ����Ե�ǰ�ļ���Ŀ¼
					if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE)  // ��ϳ�Ϊȫ·���ļ�
					{
						return FALSE;
					}
				}
				else
				{  // ��һ��ָ��Ŀ¼�ļ�
					strcpy(lpFileProperty->lpPath,lpBrowser->lpCurDir);// ���Ƶ����Ե�ǰ�ļ���Ŀ¼
					if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)// ��ϳ�Ϊȫ·���ļ�
					{
						return FALSE;
					}
				}
				if (dwSelCount > 1)
				{  // ��ǰѡ���������1
					hFind=FindFirstFile(lpFullName, &FindFileData  );  // ���ҵ�һ���ļ�
					if( hFind == INVALID_HANDLE_VALUE) 
					{// 
						return FALSE;
					}
					else
						FindClose(hFind);
					if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) || (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
					{  // ��ǰѡ����һ���ļ��У��ļ��и�����1
						nFolderNum ++;
					}
					else
					{  // ��ǰѡ����һ���ļ�����ǰ�ļ�����1
						nFileNum ++;
						lpFileProperty->dwTotalSize += FindFileData.nFileSizeLow;  // �����ļ���С
					}
					iIndex ++; // ��һ��ѡ������
					continue;
				}
				break;
			}
		}
		else
		{
			// û��ѡ����ļ�
			if (lpBrowser->bSearch == TRUE || strcmp(lpBrowser->lpCurDir,"\\") == 0)
			{  // �ڲ�Ѱ״̬��ǰ�Ǹ�Ŀ¼���õ�ϵͳ���̵�״̬
//				strcpy(lpFullName,"\\");
//				strcpy(lpFileProperty->lpPath,"\\");
//				strcpy(lpFileName,"ϵͳ����");
				strcpy(lpFileProperty->lpName,"ϵͳ����");
//				lpFileProperty->ftCreationTime = 0;
//				lpFileProperty->ftLastAccessTime = 0;
				lpFileProperty->dwFileAttributes = FILE_ATTRIBUTE_DEVICE;

				if (GetDiskFreeSpaceEx("\\",&dwFreeBytesAvailable,&dwTotalNumberOfBytes,&dwTotalNumberOfFreeBytes)) // �õ����̵Ŀռ��С
				{
					lpFileProperty->dwFreeSize = dwTotalNumberOfFreeBytes;
					lpFileProperty->dwTotalSize = dwTotalNumberOfBytes;
					return TRUE;
				}
				return NULL;
			}
			else
			{
				strcpy(lpFullName,lpBrowser->lpCurDir); // ���Ƶ�ǰĿ¼��
				GetPureFileName(lpFullName,lpFileName);  // �õ�Ŀ¼��
				_splitpath(lpBrowser->lpCurDir,NULL,lpFileProperty->lpPath,NULL,NULL); // �õ���ǰ·��
			}
		}

//		dwAttributes = GetFileAttributes(lpFullName);
//		if (dwAttributes & FILE_ATTRIBUTE_DEVICE)
		hFind=FindFirstFile(lpFullName, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{ // û���ҵ��ļ�
			return FALSE;
		}
		else
			FindClose(hFind);
		strcpy(lpFileProperty->lpName,lpFileName);  // ���Ƶ�ǰ���ļ���
		// �õ��ļ��ĵ�����
		lpFileProperty->ftCreationTime = FindFileData.ftCreationTime;
		lpFileProperty->ftLastAccessTime = FindFileData.ftLastAccessTime;
		lpFileProperty->dwFileAttributes = FindFileData.dwFileAttributes;

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DEVICE)
		{  // ��ǰ��һ���豸
			strcat(lpFullName,"\\");
			if (GetDiskFreeSpaceEx(lpFullName,&dwFreeBytesAvailable,&dwTotalNumberOfBytes,&dwTotalNumberOfFreeBytes)) // �õ��豸����Ϣ
			{
				lpFileProperty->dwFreeSize = dwTotalNumberOfFreeBytes;  // ʣ��ռ�
				lpFileProperty->dwTotalSize = dwTotalNumberOfBytes; // �豸�ܿռ�
				return TRUE;
			}
			return FALSE;
		}
		lpFileProperty->dwFreeSize = 0;  // ����ʣ��ռ�
		lpFileProperty->dwTotalSize = FindFileData.nFileSizeLow; // ��ǰ�ļ��ĳߴ�
		return TRUE;
}

// **************************************************
// ������static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
// ������
// 	IN lpFullFileName -- ȫ�ļ���������·��
// 	OUT lpPureFileName -- ���ļ�����������·��
// 
// ����ֵ����
// ������������һ��ȫ·�����ļ�������ȡһ������·�����ļ���
// ����: 
// **************************************************
static void GetPureFileName(LPTSTR lpFullFileName,LPTSTR lpPureFileName)
{
	const char *ptr = lpFullFileName;
	int length;

//		ptr=path;
		// Search '.'
		// ��ָ��ָ���ļ�����β
		while(*ptr)
		{
			ptr++;
		}
		// Get filename
		length=0;
		while(ptr>lpFullFileName)
		{ // ��û�е����ļ�ͷ
			ptr--;  // not include '\' and '.'
			if (*ptr=='\\')
			{  // �����ļ�·���ָ�
				ptr++;  // not includ��e '\' 
				break; // �˳�������ľ��ǲ�����·�����ļ���
			}
			length++;
		}
		if (lpPureFileName)
		{
			strcpy(lpPureFileName,ptr); // ���ƴ��ļ���������
		}
}
// **************************************************
// ������static BOOL CopyFolder(HWND hWnd,LPCTSTR lpExistingFolderName, LPCTSTR lpNewFolderName, BOOL bFailIfExists,BOOL bCover)
// ������
// 	IN
// 	OUT
// 	IN/OUT
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static BOOL CopyFolder(HWND hWnd,LPCTSTR lpExistingFolderName, LPCTSTR lpNewFolderName, BOOL bFailIfExists,BOOL bCover)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	LPTSTR lpSearchString = NULL;
	LPTSTR lpNewFileName = NULL,lpOldFileName = NULL,lpBackupFileName = NULL;
	BOOL bRet = FALSE;
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == 0)
			return FALSE; // ��������

	
		hFind=FindFirstFile(lpExistingFolderName, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// Դ�ļ��в����ڣ�����Ҫ�ٽ��и���
			return FALSE;
		}
		else
			FindClose(hFind);  // �رղ���

		if (IsChildDir(lpExistingFolderName, lpNewFolderName))
		{ // Ŀ���ļ��в�����Դ�ļ��е���Ŀ¼
			MessageBox(hWnd,"���ܸ��ƣ�Ŀ���ļ�����Դ�ļ��е���Ŀ¼��","����",MB_OK);
			return FALSE;
		}
		if (strcmp(lpExistingFolderName, lpNewFolderName) == 0)
		{
			// Դ�ļ�����Ŀ���ļ�����ͬ���򱸷ݵ�ǰ�ļ���
			// ���ı�Ŀ���ļ�����
			lpBackupFileName = (LPTSTR)BackupFileName((LPTSTR)lpExistingFolderName); // �õ������ļ�����
			if (lpBackupFileName == NULL)
			{  // �����ļ�������
//				MessageBox(hWnd,"�����ļ�ʧ��","����",MB_OK);
				return FALSE;
			}
			lpNewFolderName = lpBackupFileName; // �õ������ļ���
		}
		if ( bCover == FALSE )
		{ // ��Ҫ��ʾ�Ƿ񸲸�
			if (IsExist((LPTSTR)lpNewFolderName))
			{ // �ļ����Ѿ�����
				if (MessageBox(hWnd,"Ŀ¼�Ѿ�����,�Ƿ񸲸�?","��ʾ",MB_YESNO) == IDNO)
				{ // ������
					return TRUE;
				}
			}
		}
		lpSearchString = (LPTSTR)malloc(MAX_PATH); // �����ѯ�ִ�����
		if (lpSearchString == NULL)
			goto COPY_OVER;
		lpNewFileName = (LPTSTR)malloc(MAX_PATH); // �������ļ�������
		if (lpNewFileName == NULL)
			goto COPY_OVER;
		lpOldFileName = (LPTSTR)malloc(MAX_PATH);  // ������ļ�������
		if (lpOldFileName == NULL)
			goto COPY_OVER;

		hFind=FindFirstFile(lpNewFolderName, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// �ļ��в�����
			CreateDirectory(lpNewFolderName,NULL); // ����һ���µ��ļ���
		}
		else
			FindClose(hFind); // �رղ�Ѱ

/*		strcpy(lpSearchString,lpExistingFolderName);
		if (strcmp(lpSearchString,"\\") == 0)
		{
			strcat(lpSearchString,"*.*");
		}
		else
		{
			strcat(lpSearchString,"\\*.*");
		}
*/
		if (MergeFullFileName(lpSearchString,lpExistingFolderName,"*.*") == FALSE)  // �õ�Ҫ��ѯ���ļ���
		{
			bRet = FALSE;
			goto COPY_OVER;
		}
		hFind=FindFirstFile(lpSearchString, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// �ļ��в�����
			bRet = TRUE;
			goto COPY_OVER;
		}
		while(1)
		{
			if(FindFileData.cFileName[0]=='.')
			{  // ���ļ���һ����ʾ��ǰĿ¼����Ŀ¼���ļ���������
			}
			else 
			{
//				_makepath(lpOldFileName,NULL,lpExistingFolderName,FindFileData.cFileName,NULL);
				if (MergeFullFileName(lpOldFileName,lpExistingFolderName,FindFileData.cFileName) == FALSE) // ��Ͼ��ļ���
				{
					bRet = FALSE;
					goto COPY_OVER;
				}
//				_makepath(lpNewFileName,NULL,lpNewFolderName,FindFileData.cFileName,NULL);
				if (MergeFullFileName(lpNewFileName,lpNewFolderName,FindFileData.cFileName) == FALSE)  // ������ļ���
				{
					bRet = FALSE;
					goto COPY_OVER;
				}
				if( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
				{ // ��ǰ���ļ���
					bRet = CopyFolder(hWnd,lpOldFileName,lpNewFileName,bFailIfExists,TRUE); // �����ļ���
					if (bRet == FALSE)
						goto COPY_OVER;
					if (lpBrowser->bCut == TRUE && bRet == TRUE)
					{
						// ��ǰ�Ǽ���״̬��������ɺ�Ҫɾ��ԭ�����ļ����ļ���
						bRet = DeleteDirectory(hWnd,lpOldFileName);
						if (bRet == FALSE)
							goto COPY_OVER;
					}
				}
				else
				{ // ��ǰ���ļ�
					bRet = CopyFile(lpOldFileName,lpNewFileName,bFailIfExists); // �����ļ�
					if (bRet == FALSE)
						goto COPY_OVER;
					if (lpBrowser->bCut == TRUE && bRet)
					{
						// ��ǰ�Ǽ���״̬��������ɺ�Ҫɾ��ԭ�����ļ����ļ���
						bRet = DeleteSpecialFile(hWnd,lpOldFileName); // ɾ��ָ�����ļ�
						if (bRet == FALSE)
							goto COPY_OVER;
//							return FALSE;
					}
				}
			}
			if( FindNextFile(hFind,&FindFileData)==0 )  //������һ���ļ�
			{ // �Ѿ�û���ļ���
				break;
			}
		}
		FindClose(hFind);  // �رղ�Ѱ
		bRet = TRUE;

COPY_OVER:
		// �ͷ��ڴ�ռ�
		if (lpSearchString)
			free(lpSearchString);
		if (lpNewFileName)
			free(lpNewFileName);
		if (lpOldFileName)
			free(lpOldFileName);
		if (lpBackupFileName)
			free(lpBackupFileName);
		return bRet;

}

// **************************************************
// ������static BOOL SearchCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen )
// ������
// 	IN lpcszMask -- Ҫ�Ƚϵ��ִ�
// 	IN iMaskLen -- �ִ�����
// 	IN lpcszSrc -- Ҫ�Ƚϵ��ļ���
// 	IN iSrcLen -- �ļ�������
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static BOOL SearchCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen )
{
	LPTSTR lpFileName = NULL;
	if (IsAsterisk(lpcszMask,iMaskLen)==TRUE) // �Ƿ���ͨ���
	{  // ����ͨ���
		// !!! Add By Jami chen in 2003.09.28
		lpFileName = GetOnlyFileName((LPTSTR)lpcszSrc); // �õ����ļ���
		// !!! Add End By Jami chen in 2003.09.28
//		RETAILMSG(1,(" 11 Compare <%s> and <%s>\r\n",lpcszMask,lpcszSrc));
		// !!! Modified By Jami chen in 2003.09.28
//		return FileNameCompare( lpcszMask, iMaskLen, lpcszSrc, iSrcLen);
		return FileNameCompare( lpcszMask, iMaskLen, lpFileName, iSrcLen); // ͨ����ļ��Ƚ�
		// !!! Modified End By Jami chen in 2003.09.28
	}
	else
	{ // ������ͨ���
//		RETAILMSG(1,(" 00 Compare <%s> and <%s>\r\n",lpcszMask,lpcszSrc));
		return StrnStr(lpcszSrc, lpcszMask, iMaskLen);	 // �Ƚ�ָ���ִ��Ƿ�������ļ�����
	}
}
// **************************************************
// ������static BOOL StrnStr(const char *s1, const char *s2,int nLen)
// ������
// 	IN s1 -- �ִ�1
// 	IN s2 -- �ִ�2
// 	IN nLen -- �ִ�2�ĳ���
// 
// ����ֵ���ִ�1 �����ִ�2 ����TRUE�����򷵻�FALSE
// ������������Ѱ�ִ�1�Ƿ�����ִ�2��
// ����: 
// **************************************************
static BOOL StrnStr(const char *s1, const char *s2,int nLen)
{
    const unsigned char * p1, *p2, *ps;
    unsigned char c;
    int l, i;

    p1 = s1, p2 = s2;
    ps = p1;
//    l = strlen( s2 );
    l = nLen;  // �õ���Ѱ�ִ�����
    while( l )
    {
        // find first match char c
        c = toupper(*p2++); // ���ַ�ת��Ϊ��д�������ִ�Сд
        while( 1 )
        {
            if( c == toupper(*p1) ) // �ҵ���һ�����ִ�2��һ���ַ���ͬ���ַ�
                break;
            if( *p1 == 0 )
                return FALSE;
            p1++;
        }
        ps = p1;  // save first address
        p1++;

        // other chars match ?
        for( i = 1; i < l; i++, p1++, p2++ )
        {
            if( *p2 == 0 )
            {
                p2 = s2;  // reset p2 to start address
                p1 -= l;  //
                ps = p1;  // save p1 address
            }
            if( toupper(*p1) != toupper(*p2) )
                break;
            if( *p1 == 0 )
                return FALSE;
        }
        if( i == l )   // all char is match
            return TRUE;
        p1++;
    }
    return FALSE;
}

// **************************************************
// ������static BOOL IsAsterisk(LPCTSTR lpcszMask, int iMaskLen)
// ������
// 	IN lpcszMask -- �ִ���ָ��
// 	IN iMaskLen -- �ִ�����
// 
// ����ֵ����ͨ���������TRUE�����򷵻�FALSE
// ������������Ѱָ���ִ����Ƿ���ͨ�����
// ����: 
// **************************************************
static BOOL IsAsterisk(LPCTSTR lpcszMask, int iMaskLen)
{
	int i = 0;

		while(1)
		{
			if ( *lpcszMask == '*' || *lpcszMask == '?' ) // ��ǰ�ַ��Ƿ���ͨ���
				return TRUE; // ��ͨ���������TRUE��
			i++ ;
			lpcszMask++ ;
			if (i >= iMaskLen)
				break;
		}	
		return FALSE; //û��ͨ���������FALSE
}

// **************************************************
// ������static BOOL CallBackFindFile(HWND hWnd ,FILE_FIND_DATA	FindFileData,LPTSTR lpCurDir)
// ������
// 	IN hWnd -- ���ھ��
// 	IN FindFileData -- ���ҽ���ṹָ��
// 	IN lpCurDir -- ��ǰ·��
// 
// ����ֵ����Ҫ�����ò��ҽ������TRUE�����򷵻�FALSE
// �������������в��һص�����
// ����: 
// **************************************************
static BOOL CallBackFindFile(HWND hWnd ,FILE_FIND_DATA	FindFileData,LPTSTR lpCurDir)
{
	LPBROWSERDATA lpBrowser;
	char lpFileName[MAX_PATH];

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == 0)
			return FALSE; // ��������

		if (lpBrowser->FindFileFunc != NULL)
		{ // �лص�����
			_makepath( lpFileName, NULL, lpCurDir, FindFileData.cFileName, NULL); // �õ�ȫ·���ļ���
			return lpBrowser->FindFileFunc(lpFileName,&FindFileData); // ����Ӧ�ó���ص�����
		}
		return TRUE;
}

// **************************************************
// ������static BOOL IsChildDir(LPCTSTR lpParentFolderName, LPCTSTR lpChildFolderName)
// ������
// 	IN lpParentFolderName -- ���ļ���
// 	IN lpChildFolderName -- ���ļ���
// 
// ����ֵ��������ļ����Ǹ��ļ��е���Ŀ¼������TRUE�����򷵻�FALSE��
// �����������ж����ļ����Ƿ��Ǹ��ļ��е���Ŀ¼��
// ����: 
// **************************************************
static BOOL IsChildDir(LPCTSTR lpParentFolderName, LPCTSTR lpChildFolderName)
{
	if (strnicmp(lpParentFolderName, lpChildFolderName,strlen(lpParentFolderName)))  // ���ļ����Ƿ�������ļ��е��ִ�
		return FALSE;
//	if (lpChildFolderName[strlen(lpParentFolderName)] == '\\' || lpChildFolderName[strlen(lpParentFolderName)] == 0)
	if (lpChildFolderName[strlen(lpParentFolderName)] == '\\') // ���ļ����ڸ��ļ��еĽ���λ����һ��Ŀ¼�ָ�������ֹ
		return TRUE;										   // �������� \aaa\bbbbb\ccc ������\aaa\bbb ����Ŀ¼������
	return FALSE;
}

/*
	if lpPath == NULL , then the lpFullName has Include the path

*/

// **************************************************
// ������static BOOL MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName)
// ������
// 	OUT lpFullName -- ����ȫ·���ļ����Ļ���
// 	IN lpPath -- ��ǰ·��
// 	IN lpFileName -- ��ǰ�ļ���
// 
// ����ֵ���ɹ�����TRUE,���򷵻�FALSE
// ������������ϵ�ǰ·���͵�ǰ�ļ���Ϊһ��ȫ·���ļ�
// ����: 
// **************************************************
static BOOL MergeFullFileName(LPTSTR lpFullName,LPCTSTR lpPath,LPCTSTR lpFileName)
{
	if (lpPath != NULL)
		strcpy(lpFullName,lpPath);  // ����·����ȫ·����
	if (strcmp(lpFullName,"\\") == 0)
	{ // ��ǰ·���Ǹ�Ŀ¼
		if ((strlen(lpFullName) + strlen(lpFileName)) >= MAX_PATH) // �õ�ȫ·�����ļ����Ƿ񳬹�ϵͳ�����ֵ
			return FALSE;
		strcat(lpFullName,lpFileName);  // �ϳ�ȫ·���ļ�
	}
	else
	{
		if ((strlen(lpFullName) + strlen(lpFileName) +1) >= MAX_PATH) // �õ�ȫ·�����ļ����Ƿ񳬹�ϵͳ�����ֵ
			return FALSE;
		strcat(lpFullName,"\\"); // ���Ŀ¼�ָ���
		strcat(lpFullName,lpFileName); // �ϳ�ȫ·���ļ�
	}
	return TRUE;  // ���سɹ�
}

// !!! Add By Jami chen in 2003.09.09
// **************************************************
// ������static LRESULT DoRefresh(HWND hWnd)
// ������
// 	IN ���ھ��
// 
// ����ֵ����
// �������������¸����ļ�����������
// ����: 
// **************************************************
static LRESULT DoRefresh(HWND hWnd)
{
  HWND hListView;

	hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ�ľ��
	UpdateListView(hListView); // ������ʽ������
	return 0;
}
// !!! Add End By Jami chen in 2003.09.09

// !!! Add By Jami chen in 2003.09.13
// **************************************************
// ������static LPTSTR BackupFileName(LPTSTR lpExistingFolderName)
// ������
// 	IN lpExistingFolderName -- �Ѿ����ڵ��ļ�����
// 
// ����ֵ�����ر����ļ�������ָ��
// �����������õ�һ�������ļ�������
// ����: 
// **************************************************
static LPTSTR BackupFileName(LPTSTR lpExistingFolderName)
{
	LPTSTR lpBackupFileName;
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;
	int iIndex = 1;
	int iDirlen = 0,iFilenameLen = 0;
	TCHAR lpDir[MAX_PATH],lpFileName[MAX_PATH];

		if (strcmp(lpExistingFolderName,"\\") == 0) // �Ƿ��Ǹ�Ŀ¼
			return NULL; // Root Floder can't backup

		lpBackupFileName = (LPTSTR)malloc(MAX_PATH); // ����һ�������ļ����Ļ���
		if (lpBackupFileName == NULL)
			return NULL; // ����ʧ��

		strcpy(lpDir,lpExistingFolderName); // ���Ƶ�ǰ�Ѿ����ڵ��ļ�������Ŀ¼����
		
		// �õ���ǰ�ļ���Ŀ¼
		iIndex = strlen(lpDir); // �õ��ļ��ĳ���
		// �Ӻ���ǰ����Ŀ¼�ָ���"\"
		while(1)
		{
			if (lpDir[iIndex] == '\\')
			{ // �Ƿָ���"\"
				lpDir[iIndex + 1] = 0; // ��������������
				break;
			}
			if (iIndex == 0)
			{ // ���û���ҵ��ָ���������ʧ��
				free(lpBackupFileName);
				return FALSE;
			}
			iIndex --;
		}
		strcpy(lpFileName,lpExistingFolderName + iIndex +1);  // �õ��ļ���
		iDirlen = strlen(lpDir); // �õ�Ŀ¼����
		iFilenameLen = strlen(lpFileName); // �õ��ļ�������
		
		// Ȼ������������һ�����
		iIndex = 1;
		while(1)
		{
			if ((iDirlen + iFilenameLen + 10) >= MAX_PATH)
			{ // �жϱ����ļ����ĳ����Ƿ񳬳�����
				free(lpBackupFileName);
				return FALSE;
			}
			sprintf(lpBackupFileName,"%sBK_%d_%s",lpDir,iIndex,lpFileName);  // �õ������ļ���
			// ���Ҹ��ļ��Ƿ��Ѿ�����
			hFind=FindFirstFile(lpBackupFileName, &FindFileData  );  // ���ҵ�һ���ļ�
			if( hFind == INVALID_HANDLE_VALUE) 
			{// �ļ������ڣ�����ʹ�ø��ļ���
				break;
			}
			FindClose(hFind); // �رղ���
			iIndex ++; // ʹ����һ������
		}
		return lpBackupFileName; // ���ر����ļ���
}
// !!! Add End By Jami chen in 2003.09.13

// !!! Add By Jami chen in 2003.09.23
// **************************************************
// ������static BOOL IsExist(LPTSTR lpNewFileName)
// ������
// 	IN lpNewFileName -- ָ���ļ���
// 
// ����ֵ���ļ����ڷ���TRUE�����򷵻�FALSE��
// �����������ж�ָ���ļ��Ƿ���ڡ�
// ����: 
// **************************************************
static BOOL IsExist(LPTSTR lpNewFileName)
{
    FILE_FIND_DATA				FindFileData;
	HANDLE						hFind;

		hFind=FindFirstFile(lpNewFileName, &FindFileData  );  // ���ҵ�һ���ļ�
		if( hFind == INVALID_HANDLE_VALUE) 
		{// �ļ�������
			return FALSE;
		}
		else
		{ // �ļ�����
			FindClose(hFind);
			return TRUE;
		}
}
// **************************************************
// ������static BOOL DeleteSpecialFile(HWND hWnd,LPTSTR lpFileName)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpFileName -- ָ���ļ�
// 
// ����ֵ���ɹ�ɾ������TRUE�����򷵻�FALSE
// ����������ɾ��ָ���ļ���
// ����: 
// **************************************************
static BOOL DeleteSpecialFile(HWND hWnd,LPTSTR lpFileName)
{
	TCHAR lpMessage[MAX_PATH + 64];
	DWORD dwImportFile = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_ROMMODULE | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY;
	DWORD dwFileAtt;
	BOOL bRet;
	WORD dwRet;

		
		dwFileAtt = GetFileAttributes(lpFileName);  // �õ��ļ�����

		if (dwFileAtt & dwImportFile)
		{ // ָ���ļ�����Ҫ�ļ�����ʾ�Ƿ�Ҫɾ��
			sprintf(lpMessage,"(%s)����Ҫ�ļ���ȷ��Ҫɾ����?",lpFileName);
			if ((dwRet = MessageBox(hWnd,lpMessage,"��ʾ",MB_YESNO)) == IDNO)
				return FALSE; // ��Ҫɾ��
			SetFileAttributes(lpFileName,FILE_ATTRIBUTE_ARCHIVE); // Ҫɾ���������ļ�����Ϊһ���ļ���������ɾ��
		}

		bRet = DeleteFile(lpFileName);  // ɾ���ļ�
		if (bRet == FALSE)
		{ // ɾ���ļ�ʧ��
			DWORD err = GetLastError();  // �õ��������

				sprintf(lpMessage," ����ɾ���ļ�(%d) \r\n(%s)",err,lpFileName);
				MessageBox(hWnd,lpMessage,"����",MB_OK); // ��ʾɾ�����󣬲��������
				return FALSE;
		}
		return TRUE; // ɾ���ɹ�
}
// !!! Add End By Jami chen in 2003.09.23


// !!! Add By Jami chen in 2003.09.25
// For do the copy dialog
const struct dlgIDD_CopyDialog{
    DLG_TEMPLATE_EX dlg;
    DLG_ITEMTEMPLATE_EX item[1];
}dlgIDD_CopyDialog = {
    { WS_EX_NOMOVE,WS_POPUP|WS_CAPTION|WS_VISIBLE,1,0,100,240,104,0,0,"�ļ�����" },
    {
        { 0,WS_CHILD|WS_VISIBLE|SS_CENTER,15,15,220,24,0xFFFF,classSTATIC,"���ڿ����ļ�...",0 },
    } };

#define CPM_CLOSE		(WM_USER + 1235)

//static HANDLE g_hThread = NULL;
//static HWND g_hCopyDlg = NULL;

static LRESULT CALLBACK CopyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
static DWORD WINAPI CopyDialogThread(VOID * pParam);
static LRESULT DoCopyInitial(HWND hDlg,WPARAM wParam,LPARAM lParam);


// **************************************************
// ������static void ShowCopyDialog(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ������������ʾ���ڸ����ļ��Ի���
// ����: 
// ��ʾ����ʾ�������ȴ���һ���̣߳��и��̵߳�����ʾ�򣬲��ȴ��������
//	   ����ǰ�߳���������и��ƹ��̣�������ɣ�������Ϣɱ���Ի���
// **************************************************
static void ShowCopyDialog(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;
	HANDLE handle;
	int iTimes = 0;

		handle = CreateThread(NULL, 0, CopyDialogThread, (void *)hWnd, 0, 0 );  // ����һ���߳�
		CloseHandle(handle); // �رվ���������ɹ��󣬸þ����Ч��

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == 0)
			return ; // ��������
		while(1)
		{
			if (lpBrowser->hCopyWindow)
				break; // �����Ѿ��������
			Sleep(50); // �ȴ���������
			if (iTimes >= 300)
				break; // ��ȴ�9 ��
			iTimes ++;
		}
}
// **************************************************
// ������static void CloseCopyDialog(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������رո��ƶԻ���
// ����: 
// **************************************************
static void CloseCopyDialog(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ������ṹָ��
		if (lpBrowser == 0)
			return ; // ��������
		if (lpBrowser->hCopyWindow)
		{ // �Ѿ������˸��ƶԻ���
			RETAILMSG(1,(TEXT("End Dialog !!!!\r\n")));
			//EndDialog(g_hCopyDlg,0);
			//DestroyWindow(g_hCopyDlg);
			PostMessage(lpBrowser->hCopyWindow,CPM_CLOSE,0,0); // ������Ϣ�˳��öԻ���
			lpBrowser->hCopyWindow = NULL;
	//		CloseHandle(g_hThread);
		}
}
// ********************************************************************
// ������static LRESULT CALLBACK CopyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//	IN hWnd - ���ھ��
//    IN message - ��Ҫ�������Ϣ
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ����������Ӧ�ó�����Ϸ�������ڴ������
// ����: 
// ********************************************************************
static LRESULT CALLBACK CopyDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
//	int wmId, wmEvent;
	HDC hdc;
    PAINTSTRUCT	ps;

	switch (message) 
	{
		case WM_PAINT:  // ������Ϣ
			hdc = BeginPaint( hDlg, &ps );
			EndPaint( hDlg, &ps );
			return TRUE;
		case WM_INITDIALOG:  // ��ʼ���Ի���
			DoCopyInitial(hDlg,wParam,lParam);
//			g_hCopyDlg = GetWindow(hDlg,GW_OWNER);
			return 0;
		case CPM_CLOSE: // �رնԻ���
			EndDialog(hDlg,0);
			return TRUE;
		default:
			return 0;
	}
}
// ********************************************************************
// ������static LRESULT DoCopyInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
// ������
//	  IN hWnd - ���ھ��
//    IN wParam - ��Ϣ����
//    IN lParam - ��Ϣ����
// ����ֵ��
//	��Ϣ����󷵻صĽ��
// ������������ʼ��������ʾ����
// ����: 
// ********************************************************************
static LRESULT DoCopyInitial(HWND hDlg,WPARAM wParam,LPARAM lParam)
{
	HWND hParent;
	LPBROWSERDATA lpBrowser;

		hParent = (HWND)lParam;
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hParent,0);  // �õ������ṹָ��
		if (lpBrowser == 0)
			return -1; // ��������

		lpBrowser->hCopyWindow = hDlg;
		return 0;
}


// **************************************************
// ������static DWORD WINAPI CopyDialogThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// �������������ƴ����̡߳�
// ����: 
// **************************************************
static DWORD WINAPI CopyDialogThread(VOID * pParam)
{
	HINSTANCE hInstance;
	HWND hWnd;

		hWnd = (HWND)pParam; // �õ����ھ��
		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE); // �õ�ʵ�����
//		DialogBoxIndirectEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_CopyDialog,hWnd, (DLGPROC)CopyDialogProc); // �����Ի���
		DialogBoxIndirectParamEx(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_CopyDialog,hWnd, (DLGPROC)CopyDialogProc,(LONG)hWnd); // �����Ի���
//		hDlg = CreateDialogIndirect(hInstance,(LPDLG_TEMPLATE_EX)&dlgIDD_CopyDialog,hWnd, (DLGPROC)CopyDialogProc);
//		UpdateWindow( hDlg );
//	HWND hWnd; 
//			
//		hWnd = (HWND)pParam;
//		WaitForSingleObject(g_hKillDlgEvent,INFINITE);
		return 0;
}
// !!! Add End By Jami chen in 2003.09.25

// !!! Add By Jami chen in 2003.09.28
// **************************************************
// ������static LPTSTR GetOnlyFileName(LPTSTR lpFullName)
// ������
// 	IN lpFullName -- ȫ·���ļ���
// 
// ����ֵ�����ش��ļ�����
// �����������õ���ǰ�ļ��Ĵ��ļ�����
// ����: 
// **************************************************
static LPTSTR GetOnlyFileName(LPTSTR lpFullName)
{
	LPTSTR lpCurPos;

		lpCurPos = lpFullName + strlen(lpFullName); // �õ���ǰ�ļ��������һ���ַ���λ��
		while(lpCurPos!=lpFullName)
		{ // û�е����һ���ַ�
			if (*lpCurPos == '\\')
			{ // �ҵ�Ŀ¼�ָ�������������ݾ��Ǵ��ļ���
				return (lpCurPos+1);
			}
			lpCurPos --;
		}
		return lpFullName; // û���ҵ�Ŀ¼�ָ�������������һ�����ļ�����
}
// !!! Add End By Jami chen in 2003.09.28

/**************************************************
������static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ����
����ֵ����
�������������ý��㣬����WM_SETFOCUS��Ϣ��
����: 
************************************************/
static LRESULT DoSetFocus(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���
		return (LRESULT)SetFocus(hListView);
}



// **************************************************
// ������static LRESULT DoCut(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ������������ѡ����Ŀ���ļ������е����а塣
// ����: 
// **************************************************
static LRESULT DoCut(HWND hWnd)
{
	LPBROWSERDATA lpBrowser;

	
		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��
		if (lpBrowser == 0)
			return 0; // ��������

		DoCopy(hWnd); // ����Ҫ���Ƶ����а�
		//���ü��б�־
		lpBrowser->bCut = TRUE;

		return 0;
}


// **************************************************
// ������static LRESULT DoOpenCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������򿪵�ǰѡ����ļ���
// ����: 
// **************************************************
static LRESULT DoOpenCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListWnd;
	LVITEM	lvItem;
//	LPNMITEMACTIVATE lpnmia;
	char lpFileName[MAX_PATH],lpFullName[MAX_PATH];
	LPBROWSERDATA lpBrowser;
	int iCurSelItem;

		lpBrowser = (LPBROWSERDATA)GetWindowLong(hWnd,0);  // �õ��ļ������Ľṹָ��

		hListWnd = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���
		iCurSelItem = SendMessage(hListWnd, LVM_GETNEXTITEM, -1, LVNI_SELECTED);  // �õ���ǰ��ѡ����Ŀ����
		if (iCurSelItem == -1)
			return 0; // ��ǰû��ѡ����Ŀ

		lvItem.mask  = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem=iCurSelItem;  // �õ�ָ����Ŀ����
		lvItem.iSubItem=0;
		lvItem.pszText = lpFileName;
		lvItem.cchTextMax = MAX_PATH;
//		SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)iItem,(LPARAM)&lvItem);
		SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem); // �õ���Ŀ�ļ���

		if (lpBrowser->bSearch == TRUE)
		{  // ��ǰΪ��Ѱ״̬
			lvItem.mask  = LVIF_TEXT ;
			lvItem.iItem=iCurSelItem;
			lvItem.iSubItem=3;
			lvItem.pszText = lpFullName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)lvItem.iItem,(LPARAM)&lvItem);  // �õ��ļ�Ŀ¼

/*			if (strcmp(lpFullName,"\\") ==0 )
			{
				strcat(lpFullName,lpFileName);
			}
			else
			{
				strcat(lpFullName,"\\");
				strcat(lpFullName,lpFileName);
			}
*/
			if (MergeFullFileName(lpFullName,NULL,lpFileName) == FALSE) // ����ļ�ȫ·��
				return 0;
		}
		else
		{
			if (strcmp(lpFileName,".") == 0)
			{
				// ��ǰĿ¼������Ҫ����
				return 0;
			}
			if (strcmp(lpFileName,"..") == 0)
			{
				// ����һ��Ŀ¼
				DoUpPath(hWnd,0,0);
				return 0;
			}
//			_makepath( lpFullName, NULL, lpBrowser->lpCurDir, lpFileName, NULL);
			if (MergeFullFileName(lpFullName,lpBrowser->lpCurDir,lpFileName) == FALSE)  // ����ļ�ȫ·��
				return 0;
		}

		if (lvItem.iImage == TYPE_FOLDER)
		{ // ��ǰ��һ��Ŀ¼
			HWND hListView ;
	
				hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���
				if (lpBrowser->bSearch)
				{  // ��ǰ�ڲ�Ѱ״̬
					lpBrowser->bSearch = FALSE;
					DeleteListViewColumn(hListView,ID_DIRCOLUMN); // ɾ��Ŀ¼��
					if (lpBrowser->lpSearch)
					{
						free(lpBrowser->lpSearch);  // �ͷŲ�Ѱ��
						lpBrowser->lpSearch = NULL;
					}
					lpBrowser->FindFileFunc = NULL;  // ���ûص�Ϊ��
				}
				strcpy(lpBrowser->lpCurDir,lpFullName);  // ���õ�ǰĿ¼Ϊ�����Ŀ¼
				UpdateListView( hListView ); // ������ʽ
				SendNormalNotify(hWnd,EXN_DATACHANGED,NULL);  // ����֪ͨ��Ϣ���ݸı�
		}
		else
		{

			SendNormalNotify(hWnd,EXN_ACTIVEITEM,NULL);  // ������Ŀ�����֪ͨ
		}

        return 0;
}


// **************************************************
// ������static LRESULT DoRenameCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
//  IN wParam -- ����
//  IN lParam -- ����
// 
// ����ֵ����
// �����������򿪵�ǰѡ����ļ���
// ����: 
// **************************************************
static LRESULT DoRenameCurSel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	int iIndex;

		RETAILMSG(1,(" Will Rename File \r\n"));
		
		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���
		RETAILMSG(1,(" Get ListView hwnd = %x \r\n",hListView));
		
		iIndex = SendMessage(hListView,LVM_GETSELECTIONMARK,0,0); // �õ���ǰѡ��
		if (iIndex == -1)
			return FALSE;
		RETAILMSG(1,(" Send EDITLABEL to ListView  %d \r\n",iIndex));
		SendMessage(hListView,LVM_EDITLABEL,iIndex,0); // �༭��ǩ
		return TRUE;
}


// ********************************************************************
// ������static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// ����ֵ��
//	
// �������������ڴ�С�����ı�
// ����: 
// ********************************************************************
static LRESULT DoSize(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hListView;
	RECT rect;
	int iIndex;

		hListView = GetDlgItem(hWnd,ID_LISTVIEW);  // �õ���ʽ���

		GetClientRect(hWnd,&rect); // �õ���ǰ�ļ�������Ĵ��ڴ�С
		// �����б�Ĵ��ڴ�С���ļ�������Ĵ��ڴ�Сһ��
		SetWindowPos(hListView,NULL,0, 0, rect.right, rect.bottom,SWP_NOZORDER | SWP_NOMOVE | SWP_SHOWWINDOW);
		
		iIndex = SendMessage(hListView,LVM_GETSELECTIONMARK,0,0); // �õ���ǰѡ��
		if (iIndex == -1)
			return FALSE;
		SendMessage(hListView,LVM_ENSUREVISIBLE,iIndex,FALSE); // ����
		return 0;
}


// ********************************************************************
// ������static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd - Ӧ�ó���Ĵ��ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPWINDOWPOS ָ�� 
// ����ֵ��
//	
// ��������������λ�÷����ı�
// ����: 
// ********************************************************************
static LRESULT DoWindowPosChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPWINDOWPOS lpWindowPos;

		lpWindowPos = (LPWINDOWPOS)lParam;

		if ((lpWindowPos->flags & SWP_NOSIZE) == 0)
		{
			// ���ڴ�С�����ı�
			DoSize(hWnd,0,0);
		}

		return 0;
}
