/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵������/�����ļ��Ի����û����沿��
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-02
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/
#include <EWindows.h>
#include <eComCtrl.h>
#include "Explorer.h"
#include "efilepth.h"
#include <eCommDlg.h>
#include "eClipbrd.h"

/***************  ȫ���� ���壬 ���� *****************/

#define OPENFILEFLAG	1
#define SAVEFILEFLAG	2
typedef struct OpenFileStruct{
	LPOPENFILENAME lpofn;
	UINT uFileOpenFlag;  // �� (OPENFILEFLAG)  �� ���� (SAVEFILEFLAG)
}OPENFILESTRUCT , *LPOPENFILESTRUCT;

#define ID_NEWFOLDER		101
#define ID_TOHIGHERLEVEL	102
//#define ID_SHOWMODE			103
#define IDC_FILENAME		105
#define ID_OK				106
#define IDC_FILETYPE		107
#define ID_CANCEL			108
#define IDC_CURPATH			109
#define ID_BROWSER			120


static BOOL CALLBACK OpenDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort);

static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDrawItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
//static ATOM MyRegisterClass(HINSTANCE hInstance);
//static HWND InitInstance(HINSTANCE hInstance, LPOPENFILENAME lpofn);
//static BOOL FillListCtrl(HWND hWnd,LPOPENFILENAME lpofn);
static void FillFileType(HWND hWnd,LPOPENFILENAME lpofn);
//static void InsertListItem(HWND hWnd,FILE_FIND_DATA   find,BOOL bDir);
//static void RemoveAllItem(HWND hWnd);
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL GetSelectFileName(HWND hWnd);
static void DestroyOwnerData(HWND hWnd);
static void RefreshListView(HWND hWnd);
static LRESULT DoFileTypeChange(HWND hWnd,WORD wmEvent,LPARAM lParam);
static LRESULT SetShowMode(HWND hWnd);
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoToHeigherLevel(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void SetCurrentPath(HWND hWnd, LPTSTR lpCurpath);
static void ShowCurrentPath(HWND hWnd);
static void SetCurrentSelFile(HWND hWnd,LPTSTR lpFileName);

#define LOADIMAGE

static HBITMAP LoadOwnerBitmap(HWND hWnd,UINT idBitmap,UINT cx,UINT cy);
static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight);


//static void SetFileOpenCallBack(HWND hWnd,FILEOPENSTATUS lpfnFileOpen);
static BOOL FileOpenCall(HWND hwnd, LPTSTR lpFileName  );
static BOOL FileSaveCall(HWND hwnd, LPTSTR lpFileName  );

#define FILE_ATTR_DIR  0x0001   //�жϵ�ǰ�ļ���������·���Ƿ����
#define FILE_ATTR_FILE 0x0002   //�жϵ�ǰ�ļ��Ƿ����
static BOOL IsExistOfTheFile(LPTSTR lpFileName,UINT uFileType);

static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam);

const static struct dlgOpenTemplate{
    DLG_TEMPLATE_EX dlg;
//    DLG_ITEMTEMPLATE_EX item[1];
}dlgOpenTemplate = {
    { WS_EX_NOMOVE | WS_EX_OKBOX | WS_EX_CLOSEBOX |WS_EX_TITLE, WS_VISIBLE|WS_POPUP,0,30,20,210,280,0,0,"Open" },
};


// ********************************************************************
// ������BOOL  WINAPI  CDlg_ GetOpenFileName(LPOPENFILENAME lpofn)
// ������
// 	IN lpofn - ָ��ṹOPENFILENAME��ָ��
// ����ֵ��
// 	�ɹ�����TRUE�����򷵻�FALSE
// ��������������һ�����û�ѡ��һ����򿪵��ļ��ĶԻ���
// ����: ��Ӧ�ó�����Ҫʹ��"���ļ��Ի���"ʱ����
// ********************************************************************
BOOL  WINAPI  CDlg_GetOpenFileName(LPOPENFILENAME lpofn)
{
	HINSTANCE hInstance;
	LPOPENFILESTRUCT lpOpenFileStruct;
//	BOOL bRet;

//	RETAILMSG(1,("Enter GetOpenFileName\r\n"));
	InitCommonControls();  // ��ʼ��ͨ�ÿؼ�
	hInstance = (HINSTANCE)GetWindowLong(lpofn->hwndOwner,GWL_HINSTANCE); // �õ�������ʵ�����

	lpOpenFileStruct = (LPOPENFILESTRUCT)malloc(sizeof(OPENFILESTRUCT));  // ������ļ��ṹ�ڴ�

	if (lpOpenFileStruct == NULL)  // �����ڴ�ʧ��
		return FALSE;

	lpOpenFileStruct->lpofn = lpofn;  //���ýṹ
	lpOpenFileStruct->uFileOpenFlag = OPENFILEFLAG;  // ���ô򿪱�־

//	RETAILMSG(1,("Create Dialog\r\n"));
	return DialogBoxIndirectParamEx( hInstance, (LPDLG_TEMPLATE_EX)&dlgOpenTemplate, lpofn->hwndOwner, OpenDlgProc, (LPARAM)lpOpenFileStruct);  // �����Ի���

/*
    MyRegisterClass(hInstance);
	if (!(hWnd=InitInstance (hInstance, lpofn)) ) 
	{
		return FALSE;
	}
	SetFileOpenCallBack(hWnd,FileOpenCall);
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
*/

}



// ********************************************************************
// ������BOOL  WINAPI  CDlg_GetSaveFileName(LPOPENFILENAME lpofn)
// ������
// 	IN lpofn - ָ��ṹOPENFILENAME��ָ��
// ����ֵ��
// 	�ɹ�����TRUE�����򷵻�FALSE
// ��������������һ�����û�ѡ��һ���뱣����ļ��ĶԻ���
// ����: ��Ӧ�ó�����Ҫʹ��"�����ļ��Ի���"ʱ����
// ********************************************************************
BOOL  WINAPI  CDlg_GetSaveFileName(LPOPENFILENAME lpofn)
{
	HINSTANCE hInstance;
//	HWND hWnd;
//	MSG msg;
	LPOPENFILESTRUCT lpOpenFileStruct;

	InitCommonControls();  // ��ʼ��ͨ�ÿؼ�
	hInstance = (HINSTANCE)GetWindowLong(lpofn->hwndOwner,GWL_HINSTANCE);// �õ�������ʵ�����

	lpOpenFileStruct = (LPOPENFILESTRUCT)malloc(sizeof(OPENFILESTRUCT));// ������ļ��ṹ�ڴ�

	if (lpOpenFileStruct == NULL)  // �����ڴ�ʧ��
		return FALSE;

	lpOpenFileStruct->lpofn = lpofn;//���ýṹ
	lpOpenFileStruct->uFileOpenFlag = SAVEFILEFLAG; // ����Ϊ�����ļ�

	return DialogBoxIndirectParamEx( hInstance, (LPDLG_TEMPLATE_EX)&dlgOpenTemplate, lpofn->hwndOwner, OpenDlgProc, (LPARAM)lpOpenFileStruct); // �����Ի���
	/*
    MyRegisterClass(hInstance);
	if (!(hWnd=InitInstance (hInstance, lpofn)) ) 
	{
		return FALSE;
	}

	SetFileOpenCallBack(hWnd,FileSaveCall);

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return bReturn;
	*/
}



/*
static LPTSTR szWindowClass = "OpenDlgClass";


static ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASS wc;


	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)OpenDlgProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 4;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= NULL;
	wc.hbrBackground	= GetStockObject(GRAY_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= szWindowClass;

	return RegisterClass(&wc);
}
*/
//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
/*
static HWND InitInstance(HINSTANCE hInstance, LPOPENFILENAME lpofn)
{
   HWND hWnd;

   hWnd = CreateWindow(szWindowClass, lpofn->lpstrTitle, WS_VISIBLE|WS_POPUP ,//|WS_SYSMENU,
      0, 24, 240, 294, lpofn->hwndOwner, NULL, hInstance, lpofn);

   if (!hWnd)
   {
      return NULL;
   }

   ShowWindow(hWnd, TRUE);
   UpdateWindow(hWnd);

   return hWnd;
}
*/

// ********************************************************************
// ������BOOL CALLBACK OpenDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// ������
//   IN hWnd- �Ի���Ĵ��ھ��
//   IN message - ������Ϣ
//   IN wParam  - ��Ϣ����
//   IN lParam  - ��Ϣ����
// ����ֵ��
//	��
// ������������/���洰�ڵĹ��̺���
// ����: 
// ********************************************************************
static BOOL CALLBACK OpenDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
//	PAINTSTRUCT ps;
//	HDC hdc;

	switch (message) 
	{
//		case WM_CREATE:
		case WM_INITDIALOG:
			return DoCreate(hWnd,wParam,lParam);  // �����ʼ���Ի�����Ϣ
		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
				case IDC_FILETYPE:
					DoFileTypeChange(hWnd,(WORD)wmEvent,lParam);  // �ļ����͸ı�
					break;
//				case ID_SHOWMODE:
//					DoChangeShowMode(hWnd,wParam,lParam);
//					break;
				case ID_NEWFOLDER:
					DoNewFolder(hWnd,wParam,lParam);  // �����µ��ļ���
					break;
				case ID_TOHIGHERLEVEL:
					DoToHeigherLevel(hWnd,wParam,lParam);  // ����һ��Ŀ¼
					break;
			}
			break;

	   case WM_NOTIFY:
			return DoNotify(hWnd,wParam,lParam);  // ֪ͨ��Ϣ
	
	   case WM_DRAWITEM:
			return DoDrawItem(hWnd,wParam,lParam);  // ������Ŀ

//		case WM_CLOSE:
//			DestroyWindow(hWnd);
//			break;

//       case WM_DESTROY:
//		    DestroyOwnerData(hWnd);
//			PostQuitMessage(0);
//			break;
		 case WM_OK:
			if (GetSelectFileName(hWnd) == FALSE)  // ��ѡ��OK����
				break;
			DestroyOwnerData(hWnd);  // �ͷ�����
			EndDialog(hWnd,TRUE); // �رնԻ���
			break;
		 case WM_CLOSE:
			DestroyOwnerData(hWnd);  // �ͷ�����
			EndDialog(hWnd,FALSE); // �رնԻ���
			break;

	   default:
			return FALSE;
			//return DefWindowProc(hWnd, message, wParam, lParam);
   }
 	return TRUE;
}

// ********************************************************************
// ������static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- �Ի���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
// 	�ɹ����� 0 �����򷵻� -1
// �����������Ի��򴰿ڵĴ��� WM_INITDIALOG ����
// ����: 
// ********************************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HINSTANCE hInstance;
	LPOPENFILESTRUCT lpOpenFileStruct;
//	LPOPENFILENAME lpOpenFileName;

//	lpOpenFileName = (LPOPENFILENAME)lParam;

//	lpcs = (LPCREATESTRUCT) lParam;
//	lpOpenFileStruct->lpofn = lpOpenFileName;
//	RETAILMSG(1,("Enter Initial Dialog\r\n"));

	lpOpenFileStruct = (LPOPENFILESTRUCT)lParam;  // �õ��ṹ���
	if (lpOpenFileStruct == NULL)
		return -1;

//	RETAILMSG(1,("111\r\n"));
	hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);  // �õ�ʵ�����

//	RETAILMSG(1,("222 hWnd = %x ,hInstance = %x\r\n",hWnd,hInstance));
//	CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|WS_BORDER,
//	  5 , 3, 156 , 20,hWnd,(HMENU)IDC_CURPATH,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD|WS_VISIBLE, // �������뵱ǰĿ¼�ı༭��
	  5 , 3, 156 , 20,hWnd,(HMENU)IDC_CURPATH,hInstance,0);
//	RETAILMSG(1,("XX 000\r\n"));
	CreateWindow(classBUTTON,"+",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_OWNERDRAW,  // ����������һ��Ŀ¼���İ�ť
	  164,3,20,20,hWnd,(HMENU)ID_TOHIGHERLEVEL,hInstance,NULL);
//	RETAILMSG(1,("XX 111\r\n"));
	CreateWindow(classBUTTON,"+",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_OWNERDRAW,  // �������������ļ��С��İ�ť
	  186,3,20,20,hWnd,(HMENU)ID_NEWFOLDER,hInstance,NULL);
//	CreateWindow(classBUTTON,"+",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON|BS_OWNERDRAW,
//	  186,3,20,20,hWnd,(HMENU)ID_SHOWMODE,hInstance,NULL);

//	RETAILMSG(1,("XX 222\r\n"));

//	CreateWindow(classLISTCTRL,"+",WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_ICON|WS_VSCROLL|WS_HSCROLL,
//	  10,25,220,200,hWnd,(HMENU)ID_LISTVIEW,hInstance,NULL);
//	CreateWindow("","+",WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_SINGLESEL|LVS_SHOWSELALWAYS|LVS_ICON|WS_VSCROLL|WS_HSCROLL,
//	  10,25,200,160,hWnd,(HMENU)ID_LISTVIEW,hInstance,NULL);

//	CreateWindow(classFileBrowser,"",WS_CHILD|WS_VISIBLE|WS_BORDER,5,25,190,160,hWnd,(HMENU)ID_BROWSER,hInstance,NULL);
	CreateWindowEx(WS_EX_CLIENTEDGE,classFileBrowser,"",WS_CHILD|WS_VISIBLE,5,25,200,160,hWnd,(HMENU)ID_BROWSER,hInstance,NULL); // �����ļ�����ؼ�
//	RETAILMSG(1,("XX 333\r\n"));

	CreateWindow(classSTATIC ,TEXT("�ļ���:") ,WS_VISIBLE|WS_CHILD , 
	  5 , 195 , 54 , 20 , hWnd , (HMENU)NULL, (HINSTANCE)(GetWindowLong(hWnd , GWL_HINSTANCE)) , NULL); // �����ı����ļ�����
//	RETAILMSG(1,("XX 444\r\n"));
//	CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|WS_BORDER,
//	  60 , 195, 145 , 20,hWnd,(HMENU)IDC_FILENAME,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classEDIT,"",WS_CHILD|WS_VISIBLE,
	  60 , 195, 145 , 20,hWnd,(HMENU)IDC_FILENAME,hInstance,0);  // ���������ļ����ı༭��
//	RETAILMSG(1,("XX 555\r\n"));

//	CreateWindow(classBUTTON,"��",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
//	  162 , 195, 48, 20,hWnd,(HMENU)ID_OK,hInstance,NULL);

	CreateWindow(classSTATIC ,TEXT("��  ��:") ,WS_VISIBLE|WS_CHILD ,   // ���������͡��ı�
	  5 , 220 , 54 , 20 , hWnd , (HMENU)NULL, (HINSTANCE)(GetWindowLong(hWnd , GWL_HINSTANCE)) , NULL);
//	RETAILMSG(1,("XX 666\r\n"));
//	CreateWindow(classEDIT,"",WS_CHILD|WS_VISIBLE|WS_BORDER,
//	  60 , 160 , 110 , 20,hWnd,(HMENU)IDC_FILETYPE,hInstance,0);
//	CreateWindow(classCOMBOBOX,"",WS_CHILD|WS_VISIBLE|WS_BORDER|CBS_DROPDOWNLIST,
//	  60 , 220 , 145 , 90,hWnd,(HMENU)IDC_FILETYPE,hInstance,0);
	CreateWindowEx(WS_EX_CLIENTEDGE,classCOMBOBOX,"",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,  // ����ѡ���ļ����͵���Ͽ�
	  60 , 220 , 145 , 90,hWnd,(HMENU)IDC_FILETYPE,hInstance,0);

//	CreateWindow(classBUTTON,"ȡ��",WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
//	  165, 220 , 48 , 20,hWnd,(HMENU)ID_CANCEL,hInstance,NULL);

//	RETAILMSG(1,("333\r\n"));

	SetShowMode(hWnd);  // ������ʾģʽ

//	lpOpenFileStruct->lpofn = (LPOPENFILENAME)lpcs->lpCreateParams;
/*	lpOpenFileStruct->lpofn = (LPOPENFILENAME)lParam;
	strcpy(lpOpenFileStruct->curPath,"\\");

//	FillFileType(hWnd,(LPOPENFILENAME)lpcs->lpCreateParams);
	FillFileType(hWnd,(LPOPENFILENAME)lParam);

//	FillListCtrl(hWnd,(LPOPENFILENAME)lpcs->lpCreateParams);
	FillListCtrl(hWnd,(LPOPENFILENAME)lParam);
*/
//	RETAILMSG(1,("444\r\n"));
	SetWindowLong(hWnd,GWL_USERDATA,(DWORD)lpOpenFileStruct);  // ���ô����û�����
//	RETAILMSG(1,("555\r\n"));
	SetWindowText(hWnd,lpOpenFileStruct->lpofn->lpstrTitle); // ���ô��ڱ���
//	RETAILMSG(1,("666\r\n"));
	if (lpOpenFileStruct->lpofn->lpstrInitialDir)
	{
		SetCurrentPath(hWnd,(LPTSTR)lpOpenFileStruct->lpofn->lpstrInitialDir);  // ���õ�ǰ·��
	}
	if (lpOpenFileStruct->lpofn->lpstrFile)
	{
		SetCurrentSelFile(hWnd,(LPTSTR)lpOpenFileStruct->lpofn->lpstrFile); // ���õ�ǰ��ѡ���ļ�
	}
//	RETAILMSG(1,("777\r\n"));
	FillFileType(hWnd,lpOpenFileStruct->lpofn);  // �����ļ�����
//	RETAILMSG(1,("888\r\n"));
	ShowCurrentPath(hWnd); // ��ʾ��ǰĿ¼
//	RETAILMSG(1,("999\r\n"));
	RefreshListView(hWnd); // ˢ���ļ���ʾ��

//	RETAILMSG(1,("Initial Dialog OK !!!\r\n"));
	return 0;
}


// ********************************************************************
// ������static LRESULT DoDrawItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- �Ի���Ĵ��ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
//	���� 0 
// �����������Ի��򴰿ڵĴ��� WM_DRAWITEM ����
// ����: 
// ********************************************************************
static LRESULT DoDrawItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	UINT idCtl;
	LPDRAWITEMSTRUCT lpdis;
	HBITMAP hBitmap;
	RECT rect = {0,0,20,20};
	int iStartPos;

		idCtl=(UINT) wParam;  // �õ�Ҫ���Ƶ�ID
		lpdis = (LPDRAWITEMSTRUCT) lParam; // ������Ŀ�Ľṹ
		switch(idCtl)
		{
			case ID_TOHIGHERLEVEL:
				hBitmap = LoadOwnerBitmap(hWnd,OBM_TOOL_UPFOLDER,16,16);  // �õ���һ��Ŀ¼��λͼ
				break;
			case ID_NEWFOLDER:
				hBitmap = LoadOwnerBitmap(hWnd,OBM_TOOL_NEW,16,16);  // �õ�������Ŀ¼��λͼ
				break;
//			case ID_SHOWMODE:
//				hBitmap = LoadOwnerBitmap(hWnd,104,16,16);
//				break;
			default:
				return 0;
				
		}
		if (lpdis->itemState&ODS_SELECTED)
		{ // ��ť�Ѿ�������
			DrawEdge(lpdis->hDC,&rect,BDR_SUNKENOUTER,BF_RECT|BF_MIDDLE);  // ���°��߿�
			iStartPos = 2; // λͼ����ʼλ��
		}
		else
		{ // ��ťû�а���
			DrawEdge(lpdis->hDC,&rect,BDR_RAISEDOUTER,BF_MIDDLE);   // ����͹�߿�
			iStartPos = 1; // λͼ����ʼλ��
		}
		if (hBitmap)
		{
			DrawBitmap(lpdis->hDC,hBitmap,iStartPos,iStartPos,16,16); // ����ť��λͼ

			DeleteObject(hBitmap);  // ɾ��λͼ
		}
		return 0;
}

// ********************************************************************
// ������static HBITMAP LoadOwnerBitmap(HWND hWnd,UINT idBitmap,UINT cx,UINT cy)
// ������
//	IN hWnd- �Ի���Ĵ��ھ��
//    IN idBitmap - λͼ��id��
//    IN cx - λͼ�Ŀ�
//    IN cy - λͼ�ĸ�
// ����ֵ��
// 	�ɹ�����λͼ��������򷵻� NULL
// ����������װ��ָ��id�ŵ�λͼ
// ����: 
// ********************************************************************
static HBITMAP LoadOwnerBitmap(HWND hWnd,UINT idBitmap,UINT cx,UINT cy)
{
//	HINSTANCE hInstance;

//		hInstance = GetWindowLong(hWnd,GWL_HINSTANCE);

		return LoadImage(NULL,MAKEINTRESOURCE(idBitmap),IMAGE_BITMAP ,cx,cy,0);  // ��ϵͳ��Դ�ļ�װ��λͼ

}
// ********************************************************************
// ������static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight)
// ������
//	IN hdc- �豸���
//    IN hBitmap - λͼ���
//    IN x ��y - ��ʾλͼ��λ��
//    IN nWidth - λͼ�Ŀ�
//    IN nHeight - λͼ�ĸ�
// ����ֵ��
//	
// ������������ָ��λ����ʾλͼ
// ����: 
// ********************************************************************
static void DrawBitmap(HDC hdc,HBITMAP hBitmap,int x,int y,int nWidth,int nHeight)
{
	HDC hCompDC=CreateCompatibleDC(hdc );  // ����һ����ʱDC
	HBITMAP hOldMap;

	hOldMap=(HBITMAP)SelectObject(hCompDC,(HGDIOBJ)hBitmap);  // ��λͼװ�ص���ʱDC
	BitBlt(hdc, x, y, nWidth, nHeight, hCompDC,0, 0, SRCCOPY); // ��λͼ
	SelectObject(hCompDC,hOldMap); // �ָ�DC
	DeleteDC(hCompDC); // ɾ��DC
}
/*
static BOOL FillListCtrl(HWND hWnd,LPOPENFILENAME lpofn)
{
	LV_COLUMN		lvcolumn;
	TCHAR			rgtsz[3][10] = {"����","��С","ʱ��"};
	HWND hListWnd;
//	HIMAGELIST hImageList;
	HICON hIcon;
	LPOPENFILESTRUCT lpOpenFileStruct;
	HINSTANCE hInstance;

	hInstance = GetWindowLong(hWnd,GWL_HINSTANCE);

	lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,0);
	if (lpOpenFileStruct == NULL)
		return FALSE;

	hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);

	lpOpenFileStruct->hImageList =ImageList_Create(32 ,32 ,ICL_ICON ,2 ,1);
	hIcon=(HICON)LoadImage(NULL, MAKEINTRESOURCE(104) ,IMAGE_ICON , 32 ,32 ,0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hImageList, hIcon);

	hIcon = (HICON)LoadImage(NULL, MAKEINTRESOURCE(105), IMAGE_ICON , 32, 32, 0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hImageList ,hIcon);
	
	lpOpenFileStruct->hImageList = (HIMAGELIST)SendMessage(hListWnd, LVM_SETIMAGELIST,LVSIL_NORMAL ,(LPARAM)lpOpenFileStruct->hImageList);


	lpOpenFileStruct->hSmallImageList =ImageList_Create(16 ,16 ,ICL_ICON ,2 ,1);
	hIcon=(HICON)LoadImage(hInstance, MAKEINTRESOURCE(108) ,IMAGE_ICON , 16 ,16 ,0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hSmallImageList, hIcon);

	hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(109), IMAGE_ICON , 16, 16, 0);
	if (hIcon == NULL)
		return FALSE;

	ImageList_AddIcon(lpOpenFileStruct->hSmallImageList ,hIcon);
	
	lpOpenFileStruct->hSmallImageList = (HIMAGELIST)SendMessage(hListWnd, LVM_SETIMAGELIST,LVSIL_SMALL,(LPARAM)lpOpenFileStruct->hSmallImageList);


	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT | LVCFMT_IMAGE ;
	lvcolumn.pszText = rgtsz[0];
	lvcolumn.iSubItem = 0;
	lvcolumn.cx = 60;  
	lvcolumn.iImage= 0;
	SendMessage(hListWnd,LVM_INSERTCOLUMN,0,(LPARAM)&lvcolumn);


	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.pszText = rgtsz[1];
	lvcolumn.iSubItem = 1;
	lvcolumn.cx = 60;  // The Width of the column
	lvcolumn.iImage= 0;
	SendMessage(hListWnd,LVM_INSERTCOLUMN,1,(LPARAM)&lvcolumn);

	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_RIGHT;
	lvcolumn.pszText = rgtsz[2];
	lvcolumn.iSubItem = 2;
	lvcolumn.cx = 100;  // The Width of the column
	lvcolumn.iImage= 0;
	SendMessage(hListWnd,LVM_INSERTCOLUMN,2,(LPARAM)&lvcolumn);

	{

		if (lpofn->lpstrInitialDir == NULL || strlen(lpofn->lpstrInitialDir) == 0)
			strcpy(lpOpenFileStruct->curPath,"\\");
		else
		{
			if (lpofn->lpstrInitialDir[strlen(lpofn->lpstrInitialDir)-1] == '\\')
			{
				strcpy(lpOpenFileStruct->curPath,lpofn->lpstrInitialDir);
			}
			else
			{
				strcpy(lpOpenFileStruct->curPath,lpofn->lpstrInitialDir);
				strcat(lpOpenFileStruct->curPath,"\\");
			}
		}
		RefreshListView(hWnd);
	}
	return TRUE;
}
*/

// ********************************************************************
// ������static void FillFileType(HWND hWnd,LPOPENFILENAME lpofn)
// ������
// 	IN hWnd- ���ھ��
// 	IN lpofn - ָ��ṹOPENFILENAME��ָ��
// ����ֵ��
// 	
// �������������Ҫ�򿪵��ļ����͵�ѡ���
// ����: 
// ********************************************************************
static void FillFileType(HWND hWnd,LPOPENFILENAME lpofn)
{
	LPTSTR lpStrFilter;
	HWND hComboBox;
	UINT iTypeNum =0;

		lpStrFilter = (LPTSTR)lpofn->lpstrFilter; // �õ��ļ����˵�ָ��
		if (lpStrFilter == NULL)
			return;  // û���ļ�����

		hComboBox = GetDlgItem(hWnd,IDC_FILETYPE);  // �õ���Ͽ�ľ��
		
		while (strlen(lpStrFilter))
		{
			SendMessage(hComboBox,CB_ADDSTRING,0,(LPARAM)lpStrFilter);  // ����ļ����˴�����Ͽ�
			lpStrFilter +=strlen(lpStrFilter) +1;  // �õ���һ�����˴�
			iTypeNum ++;
		}

		if (iTypeNum < 4)
		{ // ����ļ����˴�С��4����Ҫ����������Ͽ�Ĵ�С
			RECT rect; 
			int iTitleHeight = GetSystemMetrics(SM_CYCAPTION);

			GetWindowRect(hComboBox,&rect); // �õ�ԭ����Ͽ�Ĵ�С
			SetWindowPos(hComboBox,NULL,0,0,rect.right-rect.left,(iTypeNum + 1)* iTitleHeight + 2,SWP_NOMOVE|SWP_NOZORDER); // ������Ͽ�Ĵ�С
		}
		SendMessage(hComboBox,CB_SETCURSEL,0,0); // Set index = 0 to current sel
}
/*
static void RemoveAllItem(HWND hWnd)
{
	HWND hListWnd;
//	SYSTEMTIME SystemTime;

		hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);
		SendMessage(hListWnd,LVM_DELETEALLITEMS,0,0);
		return;
}
*/
/*
static void InsertListItem(HWND hWnd,FILE_FIND_DATA   find,BOOL bDir)
{
	LV_ITEM			lvitem;
	HWND hListWnd;
	UINT iItem;
	TCHAR string[64];
	SYSTEMTIME SystemTime;

		hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);
		lvitem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvitem.iItem = 0x7ffff ;
		lvitem.iSubItem = 0;
		lvitem.pszText = find.cFileName;
		if (bDir)
			lvitem.iImage = 0;
		else
			lvitem.iImage = 1;

		iItem = SendMessage(hListWnd,LVM_INSERTITEM,0,(LPARAM)&lvitem);

		if (bDir == 0) // if is directory ,then is not file size
		{
			sprintf(string,"%d",find.nFileSizeLow);
			lvitem.mask = LVIF_TEXT ;
			lvitem.iItem = iItem ;
			lvitem.iSubItem = 1;
			lvitem.pszText = string;

			SendMessage(hListWnd,LVM_SETITEM,0,(LPARAM)&lvitem);
		}

		FileTimeToSystemTime( &find.ftLastWriteTime,&SystemTime ); 
		sprintf(string,"%d-%02d-%02d %02d:%02d",SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute);
		lvitem.mask = LVIF_TEXT ;
		lvitem.iItem = iItem ;
		lvitem.iSubItem = 2;
		lvitem.pszText = string;

		SendMessage(hListWnd,LVM_SETITEM,0,(LPARAM)&lvitem);
}
*/

//static Distance=TRUE;

// ********************************************************************
// ������static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����
//    IN lParam  - ��Ϣ����
// ����ֵ��
// 	���� 0 
// �����������Ի��򴰿ڵĴ��� WM_NOTIFY ����
// ����: 
// ********************************************************************
static LRESULT DoNotify(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	NMLISTVIEW *pnmvl;
	NMHDR   *hdr=(NMHDR   *)lParam;    

		switch(hdr->code)
		{
//			 case LVN_COLUMNCLICK:
//				 pnmvl=(NMLISTVIEW *)lParam;
//				 SendMessage(pnmvl->hdr.hwndFrom,LVM_SORTITEMS,(WPARAM)pnmvl->iSubItem,(LPARAM)CompareFunc);
//				 Distance=1-Distance;
//				 break;
			 case EXN_ACTIVEITEM:
				 DoActiveItem(hWnd,wParam,lParam);  // ������Ŀ��֪ͨ��Ϣ
				 break;
			 case EXN_SELCHANGED:
				 DoItemChanged(hWnd,wParam,lParam); // ѡ��ı�
				 break;
			 case EXN_DATACHANGED:
				 DoDataChanged(hWnd,wParam,lParam); // ���ݸı�
				 break;
		}
		return 0;
}

// ********************************************************************
// ������static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����������
//    IN lParam  - ��Ϣ����������
// ����ֵ��
//	���� 0 
// �����������Ի��򴰿ڵĴ��� EXN_ACTIVEITEM ����
// ����: 
// ********************************************************************
static LRESULT DoActiveItem(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	char lpFileName[MAX_PATH];//,lpFullName[MAX_PATH];
	HWND  hBrowser;
		hBrowser = GetDlgItem(hWnd,ID_BROWSER);  // �õ��ļ�����ؼ��ľ��
//		if (SendMessage(hBrowser,EXM_GETSELFILENAME,MAX_PATH,lpFileName) == TRUE)
//		{
			if (GetSelectFileName(hWnd) == FALSE) // �õ���ǰ��ѡ���ļ�
				return 0;
//			if (StrAsteriskCmp("*.htm",lpFileName) == 0)
//			{
//				MessageBox(hWnd,"Will Open File",lpFileName,MB_OK);
//			}
//		}
		EndDialog(hWnd,TRUE);
        return 0;
}
// ********************************************************************
// ������static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����������
//    IN lParam  - ��Ϣ����������
// ����ֵ��
//	���� 0 
// �����������Ի��򴰿ڵĴ��� EXN_SELCHANGED ����
// ����: 
// ********************************************************************
static LRESULT DoItemChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	/*
	LPNMLISTVIEW pnmv;
	LVITEM	lvItem;
	TCHAR lpFileName[MAX_PATH];
	HWND hListWnd;
	HWND hEdit;

		pnmv = (LPNMLISTVIEW)lParam;

		if((pnmv->uOldState&LVIS_SELECTED) != (pnmv->uNewState&LVIS_SELECTED))
		{
			if (pnmv->uNewState&LVIS_SELECTED)
			{
				// The Selected to be set
				hListWnd = GetDlgItem(hWnd,ID_LISTVIEW);
				lvItem.pszText = lpFileName;
				lvItem.cchTextMax = MAX_PATH;
				lvItem.iSubItem=0;
				lvItem.mask = LVIF_TEXT | LVIF_IMAGE ;
				lvItem.iItem = pnmv->iItem;
//				SendMessage(hListWnd,LVM_GETITEMTEXT,(WPARAM)pnmv->iItem,(LPARAM)&lvItem);
				SendMessage(hListWnd,LVM_GETITEM,(WPARAM)pnmv->iItem,(LPARAM)&lvItem);
				if (lvItem.iImage == 0)
				{
					return FALSE;
				}
				
				hEdit = GetDlgItem(hWnd,IDC_FILENAME);
				SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpFileName);
			}
		}
*/
	HWND  hBrowser;
	TCHAR lpFileName[MAX_PATH];
	HWND hEdit;
	LPTSTR lpNewPos;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����ؼ��ľ��
		if (SendMessage(hBrowser,EXM_GETSELFILENAME,MAX_PATH,(LPARAM)lpFileName) == TRUE)  // �õ���ǰ��ѡ���ļ�
		{  
			// �õ���ǰ���ļ���
			lpNewPos = lpFileName;
			while(*lpNewPos) lpNewPos++;
			while(lpNewPos>lpFileName)
			{
				if (*lpNewPos == '\\')  // �ҵ����һ����\\������������ݾ����ļ���
				{
					lpNewPos++;
					break;
				}
				lpNewPos--;
			}
//			_splitpath(lpFileName,NULL,NULL,lpFileName,NULL);
			hEdit = GetDlgItem(hWnd,IDC_FILENAME);  // �õ��ļ����ı༭����
			SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpNewPos); // �����µ��ļ���
		}
		return TRUE;
}

// ********************************************************************
// ������static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����������
//    IN lParam  - ��Ϣ����������
// ����ֵ��
//	���� 0 
// �����������Ի��򴰿ڵĴ��� EXN_DATACHANGED ����
// ����: 
// ********************************************************************
static LRESULT DoDataChanged(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
		ShowCurrentPath(hWnd);  // ���õ�ǰ·��
		return 0;
}
/*
int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,LPARAM lParamSort)
{
	     return 0;
}
*/

// ********************************************************************
// ������static LRESULT DoFileTypeChange(HWND hWnd,WORD wmEvent,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����������
//    IN lParam  - ��Ϣ����������
// ����ֵ��
//	���� 0 
// �����������Ի��򴰿ڵĴ��� IDC_FILETYPE ����
// ����: 
// ********************************************************************
static LRESULT DoFileTypeChange(HWND hWnd,WORD wmEvent,LPARAM lParam)
{
	switch(wmEvent)
	{
		case CBN_SELCHANGE:
			RefreshListView(hWnd);  // ����ˢ���ļ�����ؼ�������
			break;
		default :
			break;
	}
	return 0;
}


// **************************************************
// ������static LRESULT SetShowMode(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������������ʾģʽ��
// ����: 
// **************************************************
static LRESULT SetShowMode(HWND hWnd)
{
	HWND hListWnd;
	HWND hBrowser;
	DWORD dwStyle;

//	DWORD newListViewType;
//	DWORD tbListViewType[]={LVS_ICON,LVS_SMALLICON,LVS_LIST,LVS_REPORT};

/*
		hListWnd=GetDlgItem(hWnd,ID_LISTVIEW);

		dwStyle &= ~LVS_TYPEMASK; // clear ListView Type
		dwStyle |= newListViewType;

		SetWindowLong(hListWnd,GWL_STYLE,dwStyle );
*/		
		hBrowser = (HWND)GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����ؼ��ľ��
		hListWnd = (HWND)SendMessage(hBrowser,EXM_GETLISTVIEW,0,0); // �õ��б��ľ��

		dwStyle = GetWindowLong(hListWnd,GWL_STYLE);  // �õ��б��ԭ���Ĵ��ڷ��
		dwStyle &= ~LVS_TYPEMASK; // clear ListView Type 
		dwStyle |= LVS_LIST; // ���õ�ǰ���ΪLIST
		
		SetWindowLong(hListWnd,GWL_STYLE,dwStyle ); // �������ô��ڷ��
		return TRUE;

}

// ********************************************************************
// ������static LRESULT DoToHeigherLevel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����������
//    IN lParam  - ��Ϣ����������
// ����ֵ��
//	���� 0 
// ��������������һ��Ŀ¼���Ի��򴰿ڵĴ��� ID_TOHIGHERLEVEL ����
// ����: 
// ********************************************************************
static LRESULT DoToHeigherLevel(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hBrowser;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER);  // �õ��ļ�����ؼ��Ĵ��ھ��
		SendMessage(hBrowser,EXM_UPPATH,0,0);	 // ������Ϣ֪ͨ�ؼ�����һ��Ŀ¼

		ShowCurrentPath(hWnd); // ��ʾ��ǰ·��
		return TRUE;
}

// ********************************************************************
// ������static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
//	IN hWnd- ���ھ��
//    IN wParam  - ��Ϣ����������
//    IN lParam  - ��Ϣ����������
// ����ֵ��
//	���� 0 
// �����������������ļ��У��Ի��򴰿ڵĴ��� ID_NEWFOLDER ����
// ����: 
// ********************************************************************
static LRESULT DoNewFolder(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	HWND hBrowser;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����ؼ��Ĵ��ھ��
		SendMessage(hBrowser,EXM_NEWFOLDER,0,0); // ֪ͨ�ļ�����ؼ������µ��ļ���
		return TRUE;
}

// **************************************************
// ������static BOOL GetSelectFileName(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ���ǰѡ���ļ����ļ�����
// ����: 
// **************************************************
static BOOL GetSelectFileName(HWND hWnd)
{
	LPOPENFILENAME lpofn;
	LPOPENFILESTRUCT lpOpenFileStruct;
	HWND hBrowser;
	HWND hEdit;
	TCHAR lpFileName[MAX_PATH],lpCurPath[MAX_PATH];


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA); // �õ����ļ��ṹָ��
		if (lpOpenFileStruct == NULL)  // ָ����Ч
			return FALSE;
		lpofn = lpOpenFileStruct->lpofn;  // �õ����ļ����ṹ
		if (lpofn == NULL)  // �ṹ��Ч
			return FALSE;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����ؼ��ľ��
//		return SendMessage(hBrowser,EXM_GETSELFILENAME,lpofn->nMaxFile,lpofn->lpstrFile);

		hEdit = GetDlgItem(hWnd,IDC_FILENAME);  // �õ��ļ����༭���ľ��

		if (GetWindowTextLength(hEdit) ==0) // �õ��ļ�������
			return FALSE;
		GetWindowText(hEdit,lpFileName,MAX_PATH); // �õ��ļ���

		if (lpFileName[0] == '\\')
		{ // �ļ�����ȫ·��
			if (strlen(lpFileName) +1 >= lpofn->nMaxFile)
				return FALSE; // ����ռ䲻��
			strcpy(lpofn->lpstrFile,lpFileName); // �����ļ���
		}
		else
		{ 
			// if ("..\\")

			while(1)
			{  // �Ƿ���Ҫ����һ��Ŀ¼��Ҫ��
				if (strncmp(lpFileName,"..\\",3) == 0)
				{  // ����һ��Ŀ¼
					DoToHeigherLevel(hWnd,0,0);
					strcpy(lpFileName,lpFileName+3); // ȥ������һ��Ŀ¼�ı�ʶ
					continue;
				}
				break;
			}

			SendMessage(hBrowser,EXM_GETCURPATH,0,(LPARAM)lpCurPath);  // �õ���ǰĿ¼

//			strcpy(lpofn->lpstrFile,lpCurPath);
//			strcat(lpofn->lpstrFile,lpFileName);
			if (strlen(lpCurPath) + strlen(lpFileName) +1 >= lpofn->nMaxFile)
				return FALSE;  // ָ���Ļ���ռ䲻��

			_makepath(lpofn->lpstrFile,NULL,lpCurPath,lpFileName,NULL);  // ����ļ���

		}
		if (lpOpenFileStruct->uFileOpenFlag == OPENFILEFLAG)
			return FileOpenCall(hWnd,lpofn->lpstrFile);  // ���д��ļ�
		else
			return FileSaveCall(hWnd,lpofn->lpstrFile);  // ���б����ļ�
//		_makepath(lpofn->lpstrFile,NULL,lpCurPath,lpFileName,NULL);
		
//		return TRUE;
/*
	short iItem;//,iFileNo;
	HWND hListWnd, hEdit;
	LVITEM	lvItem;
//	LPNMITEMACTIVATE lpnmia;
	LPOPENFILENAME lpofn;
	LPOPENFILESTRUCT lpOpenFileStruct;
	TCHAR lpFileName[MAX_PATH],OldPath[MAX_PATH];


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,0);
		if (lpOpenFileStruct == NULL)
			return FALSE;
//		lpofn= (LPOPENFILENAME)GetWindowLong(hWnd,0);
		lpofn = lpOpenFileStruct->lpofn;

		hEdit = GetDlgItem(hWnd,IDC_FILENAME);
//		hListWnd = GetDlgItem(hWnd,ID_LISTVIEW);

		iItem=(short)SendMessage(hListWnd,LVM_GETNEXTITEM ,(WPARAM)-1,MAKELPARAM(LVNI_SELECTED,0));
		if (iItem==-1)
		{
			//return FALSE;
		}
		else
		{
			lvItem.mask= LVIF_IMAGE | LVIF_TEXT;

			lvItem.iSubItem =0;
			lvItem.iItem =iItem;
			lvItem.pszText = lpFileName;
			lvItem.cchTextMax = MAX_PATH;
			SendMessage(hListWnd,LVM_GETITEM,(WPARAM)iItem,(LPARAM)&lvItem);
			if (lvItem.iImage == 0)
			{
//				strcat(lpOpenFileStruct->curPath,lpFileName);
//				strcat(lpOpenFileStruct->curPath,"\\");
				// Will Refresh ListView
//				RefreshListView(hWnd);
			
				ShowCurrentPath(hWnd);

				return FALSE;
			}
		}
		if (GetWindowTextLength(hEdit) ==0)
			return FALSE;
		GetWindowText(hEdit,lpFileName,MAX_PATH);

//		strcpy(OldPath,lpOpenFileStruct->curPath);
//		if (lpFileName[0] == '\\')
//		{
//			strcpy(lpofn->lpstrFile,lpFileName);
//		}
//		else
//		{ 
			// if ("..\\")
//			while(1)
//			{
//				if (strncmp(lpFileName,"..\\",3) == 0)
//				{
//					DoToHeigherLevel(hWnd,0,0);
//					strcpy(lpFileName,lpFileName+3);
//					continue;
//				}
//				break;
//			}

//			strcpy(lpofn->lpstrFile,lpOpenFileStruct->curPath);
//			strcat(lpofn->lpstrFile,lpFileName);
//		}

//		_splitpath(lpofn->lpstrFile,NULL,lpOpenFileStruct->curPath,NULL,NULL);  // get the file directory
//		strcpy(lpFileName,lpofn->lpstrFile+strlen(lpOpenFileStruct->curPath)); // get the file name  include ext
		
//		if (IsExistOfTheFile(lpOpenFileStruct->curPath,FILE_ATTR_DIR) == FALSE)
//		{
//			strcpy(lpOpenFileStruct->curPath,OldPath);
//
//			MessageBox(hWnd,"·�������ڡ�\r\n����������·���Ƿ���ȷ��","��",MB_OK);
//			return FALSE;
//		}

//		if (lpOpenFileStruct ->lpfnFileOpen(hWnd,lpofn->lpstrFile) == FALSE)
//		{
//			SetWindowText(hEdit,lpFileName);
//			RefreshListView(hWnd);
//			ShowCurrentPath(hWnd);
//			return FALSE;
//		}
*/
		return TRUE;
}

// **************************************************
// ������static void DestroyOwnerData(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// �����������ͷ����ݡ�
// ����: 
// **************************************************
static void DestroyOwnerData(HWND hWnd)
{
		LPOPENFILESTRUCT lpOpenFileStruct;

		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA);
		if (lpOpenFileStruct != NULL)
		{
//			ImageList_Destroy(lpOpenFileStruct->hImageList);
//			ImageList_Destroy(lpOpenFileStruct->hSmallImageList);
			free(lpOpenFileStruct); // �ͷŽṹ�ռ�
		}
}

// **************************************************
// ������static void RefreshListView(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������ˢ���ļ�����ؼ���
// ����: 
// **************************************************
static void RefreshListView(HWND hWnd)
{
	TCHAR lpFilter[MAX_PATH];
	HWND  hBrowser,hFileType;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER);  // �õ��ļ�����ؼ����

		// Search Match File
		hFileType = GetDlgItem(hWnd,IDC_FILETYPE); // �õ��ļ����˴��ľ��
		GetWindowText(hFileType,lpFilter,MAX_PATH); // �õ��ļ����˴�
		SendMessage(hBrowser,EXM_SETFILTER,0,(LPARAM)lpFilter); // �����ļ����˱�־
		return;
}
// **************************************************
// ������static void SetCurrentPath(HWND hWnd, LPTSTR lpCurPath)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpCurPath -- Ҫ���õĵ�ǰ·��
// 
// ����ֵ�� ��
// �������������õ�ǰ·����
// ����: 
// **************************************************
static void SetCurrentPath(HWND hWnd, LPTSTR lpCurPath)
{
	HWND hBrowser;

		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����ؼ��ľ��
		SendMessage(hBrowser,EXM_SETCURPATH,0,(LPARAM)lpCurPath); // ���õ�ǰĿ¼���ļ�����ؼ�
}

// **************************************************
// ������static void ShowCurrentPath(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ�� 
// 
// ����ֵ����
// ������������ʾ��ǰ·��
// ����: 
// **************************************************
static void ShowCurrentPath(HWND hWnd)
{
//	LPOPENFILESTRUCT lpOpenFileStruct;
	HWND hCurPathWnd;
	HWND hBrowser;
	TCHAR lpCurPath[MAX_PATH];


//		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA);
//		if (lpOpenFileStruct == NULL)
//			return ;
//		lpOpenFileStruct->curPath;

		hCurPathWnd=GetDlgItem(hWnd,IDC_CURPATH);  // �õ���ǰ·���Ĵ��ھ��
		hBrowser = GetDlgItem(hWnd,ID_BROWSER); // �õ��ļ�����ؼ��ľ��
		SendMessage(hBrowser,EXM_GETCURPATH,0,(LPARAM)lpCurPath); // �õ���ǰ·��
		SetWindowText(hCurPathWnd,lpCurPath); // ���õ�ǰ·���ĵ�ǰ·������
}
/*
static void SetFileOpenCallBack(HWND hWnd,FILEOPENSTATUS lpfnFileOpen)
{
	LPOPENFILESTRUCT lpOpenFileStruct;
//	HWND hCurPathWnd;


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,0);
		if (lpOpenFileStruct == NULL)
			return ;

		lpOpenFileStruct ->lpfnFileOpen = lpfnFileOpen;
}
*/

// **************************************************
// ������static BOOL FileOpenCall(HWND hWnd, LPTSTR lpFileName  )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpFileName -- Ҫ�򿪵��ļ���
// 
// ����ֵ�����Դ򿪷���TRUE�����򷵻�FALSE
// �������������ļ��ص���
// ����: 
// **************************************************
static BOOL FileOpenCall(HWND hWnd, LPTSTR lpFileName  )
{
	LPOPENFILESTRUCT lpOpenFileStruct;
	LPOPENFILENAME lpofn;
	DWORD flags;
	BOOL bExist = FALSE;


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA);
		if (lpOpenFileStruct == NULL)
			return FALSE;

		lpofn = lpOpenFileStruct->lpofn;
		flags = lpofn ->Flags;

//		if (IsExistOfTheFile(lpOpenFileStruct->curPath,FILE_ATTR_DIR) == FALSE)
//		{
//			strcpy(lpOpenFileStruct->curPath,OldPath);
//
//			MessageBox(hWnd,"·�������ڡ�\r\n����������·���Ƿ���ȷ��","��",MB_OK);
//			return FALSE;
//		}
		if (flags & OFN_PATHMUSTEXIST)
		{  // Ҫ��·���������
			bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_DIR);  // �ж�·���Ƿ����
			if (bExist == FALSE)
			{ // ·�������ڣ����ܴ�
			    MessageBox(hWnd,"·�������ڡ�\r\n����������·���Ƿ���ȷ��","��",MB_OK);
				return FALSE;
			}
		}
		if (flags & OFN_FILEMUSTEXIST)
		{  // Ҫ���ļ��������
			bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_FILE);  // �ж��ļ��Ƿ����
			if (bExist == FALSE)
			{ // �ļ������ڣ����ܴ�
				MessageBox(hWnd,"�Ҳ����ļ���\r\n�����������ļ����Ƿ���ȷ��","��",MB_OK);
				return FALSE;
			}
		}
		if (flags & OFN_CREATEPROMPT)
		{ // ����ļ������ڣ�����ʾ�Ƿ���Ҫ����
			if (bExist == FALSE)
			{ // �ļ�������
				if (MessageBox(hWnd,"�Ҳ����ļ���\r\n�Ƿ񴴽����ļ���","��",MB_YESNO)==IDYES) // ��ʾ�Ƿ���Ҫ����
				{    // ��Ҫ����
					HANDLE hFile;
						
						hFile=CreateFile(lpFileName,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_NEW,FILE_ATTRIBUTE_ARCHIVE,NULL); // �������ļ�
						if (hFile==INVALID_HANDLE_VALUE)
						{ // ����ʧ��
							MessageBox(hWnd,"���ܴ����ļ���","��",MB_OK);
							return FALSE;
						}
						CloseHandle(hFile); // �ر��ļ�
						return TRUE;
				}
				return FALSE;
			}
		}
		return TRUE;
}

// **************************************************
// ������static BOOL FileSaveCall(HWND hWnd, LPTSTR lpFileName  )
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpFileName -- Ҫ������ļ���
// 
// ����ֵ�����Ա��淵��TRUE�����򷵻�FALSE
// ���������������ļ��ص���
// ����: 
// **************************************************
static BOOL FileSaveCall(HWND hWnd, LPTSTR lpFileName  )
{
	LPOPENFILESTRUCT lpOpenFileStruct;
	LPOPENFILENAME lpofn;
	DWORD flags;
	BOOL bExist;


		lpOpenFileStruct = (LPOPENFILESTRUCT)GetWindowLong(hWnd,GWL_USERDATA); // �õ����ļ��ṹ��ָ��
		if (lpOpenFileStruct == NULL)
			return FALSE;  // �ṹ��Ч

		lpofn = lpOpenFileStruct->lpofn;  // �õ����ļ����Ľṹ
		flags = lpofn ->Flags;

		if (flags & OFN_PATHMUSTEXIST)
		{ // ·���������
			bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_DIR);  // ·���Ƿ����
			if (bExist == FALSE)
			{ // �ļ�������
			    MessageBox(hWnd,"·�������ڡ�\r\n����������·���Ƿ���ȷ��","����",MB_OK);
				return FALSE;
			}
		}

		bExist = IsExistOfTheFile(lpFileName,FILE_ATTR_FILE);  // �ļ��Ƿ����
		if (flags & OFN_OVERWRITEPROMPT)
		{ //�ļ����ڣ���ʾ�Ƿ񸲸�
			if (bExist == TRUE)
			{ // �ļ�����
				if (MessageBox(hWnd,"�ļ��Ѿ����ڡ��Ƿ�Ҫ���Ǹ��ļ���","����",MB_YESNO)==IDYES) // ��ʾ�Ƿ񸲸�
				{  // ���Ը���
						return TRUE;
				}
				return FALSE; // ��Ҫ���ǣ�����ʧ��
			}
		}
		return TRUE;
}


// **************************************************
// ������static BOOL IsExistOfTheFile(LPTSTR lpFileName,UINT uFileType)
// ������
// 	IN lpFileName -- �ļ���
// 	IN uFileType -- �ļ�����
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
static BOOL IsExistOfTheFile(LPTSTR lpFileName,UINT uFileType)
{
	FILE_FIND_DATA   find;
	HANDLE hfind;


/*		if (uFileType == FILE_ATTR_DIR)
		{
			if (strlen(lpFileName) == 1 && lpFileName[0] == '\\')
				return TRUE;
			*(lpFileName + strlen(lpFileName)-1 ) = 0;
		}
		*/
		hfind = FindFirstFile( lpFileName, &find ); // �����ļ�
		if( hfind != INVALID_HANDLE_VALUE )
		{  // The File is Exist
			FindClose( hfind );

			if (uFileType == FILE_ATTR_DIR)
			{
				//����ļ����ڣ���·��һ�����ڡ�
/*				strcat(lpFileName , "\\");
				if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					return TRUE;
				else
					return FALSE;
*/
				return TRUE;
			}
			else if (uFileType == FILE_ATTR_FILE)
			{ // ���Ҫ���ļ��Ƿ����
				if (find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  // ���ҵ�����·��
					return FALSE; // �ļ�������
				else
					return TRUE;  // �ļ�����
			}
			else
				return FALSE;
		}
		else
		{
			// The File is Not Exist
			if (uFileType == FILE_ATTR_DIR)
			{  // Ҫ��õ�Ŀ¼�Ƿ����
				DWORD err = GetLastError();  // �õ�������Ϣ
				if (err == 0x0000003)  
//				if (err == ERROR_PATH_NOT_FOUND)
					return FALSE; // Ŀ¼������
				else
					return TRUE; // Ŀ¼����
				}
			else if (uFileType == FILE_ATTR_FILE)
			{ // û�в鵽���ļ��϶�������
				return FALSE;
			}
			else
				return FALSE;
		}
}


// **************************************************
// ������static void SetCurrentSelFile(HWND hWnd,LPTSTR lpFileName)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpFileName -- Ҫ����Ϊ��ǰѡ���ļ����ļ���
// 
// ����ֵ����
// �������������õ�ǰ��ѡ���ļ���
// ����: 
// **************************************************
static void SetCurrentSelFile(HWND hWnd,LPTSTR lpFileName)
{
	HWND hEdit;

		hEdit = GetDlgItem(hWnd,IDC_FILENAME); // �õ��༭���ľ��
		SendMessage(hEdit,WM_SETTEXT,0,(LPARAM)lpFileName);// ���õ�ǰ�ļ����༭��

		return;
}
