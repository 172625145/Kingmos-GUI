/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����MMS�����
�汾�ţ�1.0.0
����ʱ�ڣ�2004-06-07
���ߣ��½��� Jami chen
�滮��
�޸ļ�¼��
******************************************************/
#include <ewindows.h>
//#include "resource.h"
#include <ecomctrl.h>
#include <thirdpart.h>
#include <mmsmanage.h>
#include <mmsbrowser.h>
#include <imgbrowser.h>
#include "esmilbrowser.h"
#include "eCommdlg.h"
#include "..\include\mms_content.h"
#include "..\mmsbody\mime_body.h"

/***************  ȫ���� ���壬 ���� *****************/

//static char   classMMSBrowser[20] = TEXT("MMSBROWSER");

#define ID_IMGVIEW	300
#define ID_SMILVIEW	301

#define MMSTYPE_UNKNOWFILE	0
#define MMSTYPE_TEXTFILE	100
#define MMSTYPE_BMPFILE		101
#define MMSTYPE_GIFFILE		102
#define MMSTYPE_JPGFILE		103
#define MMSTYPE_MP3FILE		104
#define MMSTYPE_3GPFILE		105
#define MMSTYPE_AMRFILE		106
#define MMSTYPE_MIDFILE		107
#define MMSTYPE_WAVFILE		108
#define MMSTYPE_SMILFILE	109

#define ID_POPUPMENU			100//����ʱ�ӵı��
#define DELAY_POPUPMENU			1000 // �±ʺ�Ҫ�ӳٵ�ʱ�䵯���˵� 1 s

#define IDM_PLAY		300
#define IDM_SAVE		301
#define IDM_PREVIEW		302
#define IDM_SMILPLAY	303

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

const static FILETYPE tabFileType[] = {
	{ "*.txt" , MMSTYPE_TEXTFILE },
	{ "*.bmp" , MMSTYPE_BMPFILE },
	{ "*.jpg" , MMSTYPE_JPGFILE },
	{ "*.gif" , MMSTYPE_GIFFILE },
	{ "*.mp3" , MMSTYPE_MP3FILE },
	{ "*.3gp" , MMSTYPE_3GPFILE },
	{ "*.amr" , MMSTYPE_AMRFILE },
	{ "*.mid" , MMSTYPE_MIDFILE },
	{ "*.wav" , MMSTYPE_WAVFILE },
	{ "*.smil", MMSTYPE_SMILFILE },
};

// �У��ң���ʾ����
typedef struct structMMSShowPart{
	int iShowPos;  // ��ʾλ��
	UINT iFileType; // ��ǰ���ļ�����
	LPMIME_PART lpMIME_Part; // ��ǰҪ��ʾ��PARTָ��
	LPTSTR lpString; // ��ǰPART�ı�
}MMSSHOWPART, *LPMMSSHOWPART;


typedef struct MMSBrowserStruct{
	MIME_BODY MIME_Body;
	LPBODYDETAIL lpBodyDetail;
	LPMMSSHOWPART lpMMSShowPart;
	int iPartNum ;
	int iCurPart;
	BOOL bImgShow;
	BOOL bAudioPlay;
}MMSBROWSER, * LPMMSBROWSER;


#define POS_TEXTSTART	40
#define ICON_PARTWIDTH	40


/******************************************************/
// ����������
/******************************************************/
ATOM RegisterMMSBrowserClass( HINSTANCE hInstance );
static LRESULT CALLBACK MMSBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void OnMMSBrowserPaint( HWND hWnd );
static LRESULT DoMMSBrowserCreate(HWND hWnd);
static void InsertToolBarItem(HWND hWnd,HWND hToolBar);
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam);

static LRESULT DoSetMMSData(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void OrganizeMMSPart(HWND hWnd);
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam);

static UINT GetPartType(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);

static void DrawMMSPart(HWND hWnd,HDC hdc);
static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart);

static UINT GetPartHeight(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static LPTSTR GetPartText(HWND hWnd ,LPMIME_PART lpMIME_Part);

static void PreViewImgPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static int ShowIconContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart);
//static LPMMSSHOWPART GetPart(HWND hWnd,POINT Point);
static int GetPart(HWND hWnd,POINT Point);

static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL PlayCurrentPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static BOOL SavePartData(HWND hWnd,LPMMSSHOWPART lpMMSShowPart,LPTSTR lpFileName,BOOL bNameFile);
static void ShowImage(HWND hWnd,LPTSTR lpFileName,UINT uImgType);

static HMENU CreateMMSMenu(UINT iFileType);
static LRESULT DoClick(HWND hWnd);


static void PlaySmilPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static void PlaySmil(HWND hWnd,LPTSTR lpFileName);
static void SaveAsPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart);
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName);


extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;
//extern BYTE SetWell_Know_Type(LPTSTR lpContent_Type);
//extern ATOM RegisterIMGBrowserClass( HINSTANCE hInstance );
extern ATOM RegisterSMILBrowersClass( HINSTANCE hInst );


// ********************************************************************
// ������ATOM RegisterMMSBrowserClass( HINSTANCE hInstance );
// ������
//	IN hInstance - ����ϵͳ��ʵ�����
// ����ֵ��
// 	��
// ����������ע��ϵͳ������
// ����: 
// ********************************************************************
ATOM RegisterMMSBrowserClass( HINSTANCE hInstance )
{
	WNDCLASS wcex;

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)MMSBrowserWndProc;  // ������̺���
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(long);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= classMMSBrowser;
	return RegisterClass(&wcex); // ��ϵͳע��������
}

// ********************************************************************
// ������static LRESULT CALLBACK MMSBrowserWndProc(HWND , UINT , WPARAM , LPARAM )
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
static LRESULT CALLBACK MMSBrowserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
		case WM_PAINT:  // �滭��Ϣ
			OnMMSBrowserPaint( hWnd );
			return 0;
//		case WM_NOTIFY: // ֪ͨ��Ϣ
//			return DoNotify(hWnd,wParam,lParam);
		case WM_CREATE: // ������Ϣ
			return DoMMSBrowserCreate(hWnd);
		case WM_DESTROY: // �ƻ���Ϣ
			DoReleaseControl(hWnd,wParam,lParam);
			break;
		case WM_COMMAND:
			DoCommand(hWnd,wParam,lParam);
			break;
	   	case WM_LBUTTONDOWN: // ����������
			return DoLButtonDown(hWnd,wParam,lParam);
   		case WM_LBUTTONUP: // ����������
			return DoLButtonUp(hWnd,wParam,lParam);
		case WM_TIMER:
			return DoTimer(hWnd,wParam,lParam);
		case WM_NOTIFY://֪ͨ��Ϣ
			{
				NMHDR   *hdr = (NMHDR   *)lParam;
				switch(hdr->code)
				{
					case NMIMG_CLICK://���֪ͨ
					case NMSMIL_CLICK://���֪ͨ
						DoClick(hWnd);
						break;
				}
			}
			break;
		case WMB_SETMMSDATA:
			DoSetMMSData(hWnd,wParam,lParam);
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );  // ����Ĭ����Ϣ
   }
   return FALSE;
}

// ********************************************************************
// ������static void OnMMSBrowserPaint( HWND hWnd )
// ������
//	IN hWnd- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//	��
// ��������������ϵͳ���洰�ڻ滭����
// ����: 
// ********************************************************************
static void OnMMSBrowserPaint( HWND hWnd )
{
	HDC hdc;
    PAINTSTRUCT	ps;
//	RECT rt;
//	LPMMSBROWSER lpMMSBrowser;

//		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
//		ASSERT(lpMMSBrowser);

//		GetClientRect(hWnd,&rt);

		hdc = BeginPaint( hWnd, &ps );  // ��ʼ����
//		DrawText(hdc, "abcd", 4, &rt, DT_LEFT | DT_VCENTER);
		DrawMMSPart(hWnd,hdc);
		
		EndPaint( hWnd, &ps ); // ��������
}


// ********************************************************************
// ������static LRESULT DoMMSBrowserCreate(HWND hWnd)
// ������
//	IN	hWnd -- ����ϵͳ�Ĵ��ھ��
// ����ֵ��
//		���ش����Ľ��
// �����������������洴����Ϣ
// ����: 
// ********************************************************************
static LRESULT DoMMSBrowserCreate(HWND hWnd)
{
	HINSTANCE hInstance;
	LPMMSBROWSER lpMMSBrowser;
	RECT rt;

		hInstance = (HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE);

		lpMMSBrowser = (LPMMSBROWSER)malloc(sizeof(MMSBROWSER)); // ����MMS������ṹָ��
		if (lpMMSBrowser == NULL)
		{  // �����ڴ�ʧ��,����ʧ��
			return -1;
		}

		lpMMSBrowser->MIME_Body.lpData = NULL;
		lpMMSBrowser->MIME_Body.iDataSize = 0; 
		lpMMSBrowser->lpBodyDetail = NULL;
		lpMMSBrowser->lpMMSShowPart = NULL;
		lpMMSBrowser->iCurPart = -1; // û�е�ǰ��PART
		lpMMSBrowser->iPartNum  = 0 ; // Ŀǰû��PART
		lpMMSBrowser->bImgShow = FALSE;
		lpMMSBrowser->bAudioPlay = FALSE;

		SetWindowLong(hWnd,0,(LONG)lpMMSBrowser);

//		RegisterIMGBrowserClass(hInstance); // ע��ͼ����ʾ����
		RegisterSMILBrowersClass(hInstance);  // ע��SMIL��ʾ����

		GetClientRect(hWnd,&rt);

		CreateWindow(classIMGBrowser,"",WS_CHILD,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_IMGVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

		CreateWindow(classSMILBROWSER,"",WS_CHILD,
				rt.left,
				rt.top,
				rt.right-rt.left,
				rt.bottom-rt.top,
				hWnd,
				(HMENU)ID_SMILVIEW,
				(HINSTANCE)GetWindowLong( hWnd, GWL_HINSTANCE),
				NULL);

		return 0;
}


// **************************************************
// ������static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ����
// ��������������������Ϣ��
// ����: 
// **************************************************
static LRESULT DoCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int wmId, wmEvent;
	HWND hIdWnd;

		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		hIdWnd = (HWND)lParam;

		RETAILMSG(1,(TEXT("Receive Command [ID = %d], [Event = %d]\r\n"),wmId,wmEvent));
		RETAILMSG(1,(TEXT("Receive Command [Window = %x]\r\n"),hIdWnd));
		switch (wmId)
		{
			default:
			   return DefWindowProc(hWnd, WM_COMMAND, wParam, lParam);
		}

		return 0;
}

// **************************************************
// ������static LRESULT DoSetMMSData(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- (HMMSDATA)����ָ��
// 
// ����ֵ����
// ��������������������Ϣ��
// ����: 
// ˵��: ����û����������Ϊ��,���ʾҪ���ԭ��������
// **************************************************
static LRESULT DoSetMMSData(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMS_CONTENT lpNewData;
	LPMMSBROWSER lpMMSBrowser;
	DATA_DEAL MIMEData;
	BOOL bOver;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->bImgShow == TRUE)
		{
			// ��ǰ������ʾͼ��,������ʾ����
			ShowImage(hWnd,NULL,0);
			PlaySmil(hWnd,NULL);
		}
		// �����ͷ�ԭ��������
		if (lpMMSBrowser->MIME_Body.lpData)
			free(lpMMSBrowser->MIME_Body.lpData);
		lpMMSBrowser->MIME_Body.lpData = NULL;
		lpMMSBrowser->MIME_Body.iDataSize = 0; 

		if (lpMMSBrowser->lpMMSShowPart)
		{
			int i;

			for ( i = 0; i < lpMMSBrowser->lpBodyDetail->iPartNum; i++)
			{
				if (lpMMSBrowser->lpMMSShowPart[i].lpString)
					free(lpMMSBrowser->lpMMSShowPart[i].lpString);  // �ͷ��Ѿ�������ı���
			}
			free(lpMMSBrowser->lpMMSShowPart);
			lpMMSBrowser->lpMMSShowPart = NULL;
		}
		if (lpMMSBrowser->lpBodyDetail)
		{
			MIME_Release(lpMMSBrowser->lpBodyDetail);
			lpMMSBrowser->lpBodyDetail = NULL;
			lpMMSBrowser->iPartNum  = 0 ; // Ŀǰû��PART
		}

		lpNewData = (LPMMS_CONTENT)lParam; // �õ��µ�MIME����
		if (lpNewData == NULL)
		{  // û������
			return TRUE;
		}

		lpMMSBrowser->MIME_Body.lpData = malloc(lpNewData->iDataSize); //����ռ������MIME����
		if (lpMMSBrowser->MIME_Body.lpData == NULL)
		{  // ����ʧ��
			return FALSE;
		}

		// ��������
		memcpy(lpMMSBrowser->MIME_Body.lpData , lpNewData->lpData,lpNewData->iDataSize);
		lpMMSBrowser->MIME_Body.iDataSize = lpNewData->iDataSize; 

		MIMEData.lpData = lpMMSBrowser->MIME_Body.lpData;
		MIMEData.dwLenTotal = lpMMSBrowser->MIME_Body.iDataSize;
		MIMEData.dwLenDealed = 0;

		lpMMSBrowser->lpBodyDetail = MIME_UnPack(&MIMEData,&bOver);
		lpMMSBrowser->iPartNum  = lpMMSBrowser->lpBodyDetail->iPartNum ; // �õ�ĿǰPART��Ŀ

		OrganizeMMSPart(hWnd);
		return TRUE;
}


// **************************************************
// ������static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ����
// �����������ͷſؼ��ڴ档
// ����: 
// **************************************************
static LRESULT DoReleaseControl(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		// �ͷ�����
		if (lpMMSBrowser->MIME_Body.lpData)
			free(lpMMSBrowser->MIME_Body.lpData);

		lpMMSBrowser->MIME_Body.lpData = NULL;
		lpMMSBrowser->MIME_Body.iDataSize = 0; 

		// �ͷ���ʾ����
		if (lpMMSBrowser->lpMMSShowPart)
		{
			int i;

			for ( i = 0; i < lpMMSBrowser->lpBodyDetail->iPartNum; i++)
			{
				if (lpMMSBrowser->lpMMSShowPart[i].lpString)
					free(lpMMSBrowser->lpMMSShowPart[i].lpString);  // �ͷ��Ѿ�������ı���
			}
			free(lpMMSBrowser->lpMMSShowPart);
			lpMMSBrowser->lpMMSShowPart = NULL;
		}

		// �ͷ�ϸ��
		if (lpMMSBrowser->lpBodyDetail)
		{
			MIME_Release(lpMMSBrowser->lpBodyDetail);
			lpMMSBrowser->lpBodyDetail = NULL;
			lpMMSBrowser->iPartNum  = 0 ; // Ŀǰû��PART
		}


		free(lpMMSBrowser);		// �ͷ�����ṹ
		return TRUE;
}


// **************************************************
// ������static void DrawMMSPart(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ����
// ��������������MMS�����ݡ�
// ����: 
// ˵������MMS������У����ϲ���ʾ��ý���ļ�ͼ��������ʾ�ı��ļ�����
// **************************************************
static void DrawMMSPart(HWND hWnd,HDC hdc)
{
	LPMMSBROWSER lpMMSBrowser;
//	LPMIME_PART lpMIME_Part;
	LPMMSSHOWPART lpMMSShowPart;
	UINT uPartNum;
	UINT	i;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

//		GetClientRect(hWnd,&rt);

		if (lpMMSBrowser->lpBodyDetail == NULL)
		{  // Ŀǰ��û������
			return ;
		}
	
//		lpMIME_Part = lpMMSBrowser->lpBodyDetail->lpMIME_Part;  // �õ���һ��PART
		uPartNum = lpMMSBrowser->lpBodyDetail->iPartNum;
//		while (lpMIME_Part)
		for (i = 0; i < uPartNum; i++)
		{
			lpMMSShowPart = &lpMMSBrowser->lpMMSShowPart[i]; // �õ�Ҫ��ʾ��PARTָ��
//			iFileType = GetPartType(lpMIME_Part);
			switch(lpMMSShowPart->iFileType)
			{
				case MMSTYPE_UNKNOWFILE: // �������ݵ��ļ��ཫ����ʾͼ��
				case MMSTYPE_BMPFILE:
				case MMSTYPE_GIFFILE:
				case MMSTYPE_JPGFILE:
				case MMSTYPE_MP3FILE:
				case MMSTYPE_3GPFILE:
				case MMSTYPE_AMRFILE:
				case MMSTYPE_MIDFILE:
				case MMSTYPE_WAVFILE:
				case MMSTYPE_SMILFILE:
					ShowIconContent(hWnd,hdc,lpMMSShowPart); //��ָ��λ����ʾPARTͼ��
					break;
				case MMSTYPE_TEXTFILE:
					ShowTextContent(hWnd,hdc,lpMMSShowPart); // ��ָ��λ����ʾPART���ı�
					break;
			}

//			lpMIME_Part = lpMIME_Part->next; // ����һ��PART
		}

}


// **************************************************
// ������static UINT GetPartType(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// ������
//	IN hWnd  -- ���ھ��
// 	IN lpMMSShowPart -- Ҫ�õ����͵�part
// 
// ����ֵ����
// �����������õ��ļ����͡�
// ����: 
// **************************************************
static UINT GetPartType(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	int iFileTypeNum;
	int i;
	BYTE bPartType;
	LPMMSBROWSER lpMMSBrowser;
	LPMIME_PART lpMIME_Part;
	LPTSTR lpData;

		if (lpMMSShowPart == NULL)
		{
			// û��ָ����PART
			return MMSTYPE_UNKNOWFILE;
		}
		lpMIME_Part = lpMMSShowPart->lpMIME_Part;  
		if (lpMIME_Part == NULL)
		{
			// û��ָ����PART
			return MMSTYPE_UNKNOWFILE;
		}
		if (lpMIME_Part->lpContent_Location == NULL)
			goto UNKNOWTYPE;
		iFileTypeNum = sizeof (tabFileType) / sizeof (FILETYPE); // �õ���ǰ����֪������Ŀ
		for (i = 0 ; i < iFileTypeNum; i++ )
		{
			if (FileNameCompare( tabFileType[i].lpFileExt, strlen( tabFileType[i].lpFileExt ), lpMIME_Part->lpContent_Location, strlen(lpMIME_Part->lpContent_Location)) == TRUE)
			{ // �Ƚϵ�ǰ�ļ��Ƿ�����֪�ļ�
				return tabFileType[i].iFileType;  // ʹ����֪�ļ���ͼ��
			}
		}
UNKNOWTYPE:
		if (lpMIME_Part->lpContent_Type)
		{
//			bPartType = SetWell_Know_Type(lpMIME_Part->lpContent_Type);
			CNTTYPE_ASSIGN stCntTypeAssign;
	
			if (strlen(lpMIME_Part->lpContent_Type) >= LEN_WSP_CNTTYPE_NAME)
			{
				// �϶�������֪�Ĳ�����������
				bPartType = 0XFF;
			}
			else
			{
				strcpy(stCntTypeAssign.szName,lpMIME_Part->lpContent_Type);
				//  ���ҵ�ǰ���������Ƿ�Ϊ��֪������
				if (VarFind_CntTypeAssign( CNTTYPEFIND_NUM_BYNAME, &stCntTypeAssign ) == TRUE)
				{
					// �Ѿ��鵽������֪�Ĳ�����������
					bPartType = (BYTE)stCntTypeAssign.dwNum;
				}
				else
				{
					// ������֪�Ĳ�����������
					bPartType = 0XFF;
				}
			}

			switch(bPartType)
			{
				case 0x01:  // text/*
				case 0x03:  // text/plain
					return MMSTYPE_TEXTFILE;
				case 0x1D:  // "image/gif"
					return MMSTYPE_GIFFILE;
				case 0x1E:  // "image/jpeg"
					return MMSTYPE_JPGFILE;
			}
			if (stricmp(lpMIME_Part->lpContent_Type,"application/smil") == 0)
			{
					return MMSTYPE_SMILFILE;
			}
		}
		
		// ������,����λ���ļ�����

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		lpData = lpMMSBrowser->MIME_Body.lpData + lpMMSShowPart->lpMIME_Part->uPos; // �õ����ݿ�ʼλ��
		if (strncmp(lpData,"GIF",3) == 0)
		{
			// ��GIF�ļ�
			return MMSTYPE_GIFFILE;
		}

		return MMSTYPE_UNKNOWFILE;
}

// **************************************************
// ������static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
//  IN lpMMSShowPart -- Ҫ��ʾ��PART
// 
// ����ֵ���ɹ������µ�λ�÷��򷵻�-1
// ������������ָ��λ����ʾPART���ı���
// ����: 
// **************************************************
static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
{
//	int x,y;
	RECT rect;
//	LPTSTR lpString;
	LPMMSBROWSER lpMMSBrowser;

		if (lpMMSShowPart->lpString == NULL)
			return 0; // û���ı���Ҫ��ʾ

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		// �õ��ı���ʾ�Ŀ�ʼλ��
		GetClientRect(hWnd,&rect);
		rect.left = 0;
		rect.top = POS_TEXTSTART + lpMMSShowPart->iShowPos; 
		
		DrawText(hdc,lpMMSShowPart->lpString,strlen(lpMMSShowPart->lpString),&rect,DT_LEFT);

		return 0;
}


// **************************************************
// ������static void DrawMMSPart(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 
// ����ֵ����
// ��������������MMS�����ݡ�
// ����: 
// ˵������MMS������У����ϲ���ʾ��ý���ļ�ͼ��������ʾ�ı��ļ�����
// **************************************************
static void OrganizeMMSPart(HWND hWnd)
{
	LPMMSBROWSER lpMMSBrowser;
	LPMIME_PART lpMIME_Part;
	UINT uIndex = 0;
	UINT iTextPos = 0, iIconPos = 0;
	int iFileType;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->lpBodyDetail == NULL)
		{  // Ŀǰ��û������
			return ;
		}
		lpMMSBrowser->lpMMSShowPart = (LPMMSSHOWPART)malloc(lpMMSBrowser->lpBodyDetail->iPartNum * sizeof(MMSSHOWPART)); // ����һ���洢�ռ�
		if (lpMMSBrowser->lpMMSShowPart == NULL)
		{  // �����ڴ�ʧ��
			return ;
		}
	
		lpMIME_Part = lpMMSBrowser->lpBodyDetail->lpMIME_Part;  // �õ���һ��PART

		while (lpMIME_Part)
		{
			lpMMSBrowser->lpMMSShowPart[uIndex].lpMIME_Part = lpMIME_Part; // �趨��ǰ��PARTָ��
			lpMMSBrowser->lpMMSShowPart[uIndex].lpString = NULL; // ��ʼ����ǰ���ı���
			iFileType = GetPartType(hWnd,&lpMMSBrowser->lpMMSShowPart[uIndex]); // �õ���ǰPART���ļ�����
			lpMMSBrowser->lpMMSShowPart[uIndex].iFileType = iFileType; // �õ��ļ�����
			switch(iFileType)
			{
				case MMSTYPE_UNKNOWFILE:	// ����ʶ���ļ�����
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_BMPFILE:  // ͼ���ļ�
				case MMSTYPE_GIFFILE:
				case MMSTYPE_JPGFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_3GPFILE: // �����ļ�
				case MMSTYPE_MP3FILE:  
				case MMSTYPE_AMRFILE:
				case MMSTYPE_MIDFILE:
				case MMSTYPE_WAVFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_SMILFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iIconPos;
					iIconPos ++;
					break;
				case MMSTYPE_TEXTFILE:
					lpMMSBrowser->lpMMSShowPart[uIndex].lpString = GetPartText(hWnd,lpMIME_Part); // ��ʼ����ǰ���ı���
					lpMMSBrowser->lpMMSShowPart[uIndex].iShowPos = iTextPos;
					iTextPos += GetPartHeight(hWnd,&lpMMSBrowser->lpMMSShowPart[uIndex]); // �õ���ǰPART���ı��߶�
					break;
			}
			lpMIME_Part = lpMIME_Part->next; // ����һ��PART
			uIndex ++;
			if (uIndex >= (UINT)lpMMSBrowser->lpBodyDetail->iPartNum)
				break; // �Ѿ���֯���
		}
}


// **************************************************
// ������static UINT GetPartHeight(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpMMSShowPart -- Ҫ�õ��߶ȵ�PART
// 
// ����ֵ���ɹ����ص�ǰPART�ĸ߶�,���򷵻�0
// �����������õ���ǰPART�ĸ߶ȡ�
// ����: 
// **************************************************
static UINT GetPartHeight(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	RECT rect;
	int iPartHeigth;
	HDC hdc;

	if (lpMMSShowPart->lpString == NULL)
		return 0; // û���ı���
	GetClientRect(hWnd,&rect) ; // �õ��ؼ��Ŀͻ�����С

	hdc = GetDC(hWnd); // �õ��豸���
	iPartHeigth = DrawText(hdc,lpMMSShowPart->lpString,strlen(lpMMSShowPart->lpString),&rect,DT_CALCRECT);
	ReleaseDC(hWnd,hdc); // �ͷ��豸���
	return (UINT)rect.bottom; // ���ظ߶�
}


// **************************************************
// ������static LPTSTR GetPartText(HWND hWnd ,LPMIME_PART lpMIME_Part)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpMIME_Part -- Ҫ�õ��ı���PART
// 
// ����ֵ���ɹ����ص�ǰPART���ı���ָ��,���򷵻�0
// �����������õ���ǰPART���ı���
// ����: 
// **************************************************
static LPTSTR GetPartText(HWND hWnd ,LPMIME_PART lpMIME_Part)
{
	LPTSTR lpString;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		lpString = (LPTSTR)malloc(lpMIME_Part->uLen + 1); // ����һ���ڴ�������ı�
		if (lpString == NULL)
		{  // �����ڴ�ʧ��
			return NULL;
		}
		memset(lpString,0,lpMIME_Part->uLen + 1);
		memcpy(lpString,lpMMSBrowser->MIME_Body.lpData + lpMIME_Part->uPos,lpMIME_Part->uLen); //�õ��ı�

		return lpString;
}

// **************************************************
// ������static int ShowTextContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
//  IN lpMMSShowPart -- Ҫ��ʾ��PART
// 
// ����ֵ���ɹ������µ�λ�÷��򷵻�-1
// ������������ָ��λ����ʾPART���ı���
// ����: 
// **************************************************
static int ShowIconContent(HWND hWnd,HDC hdc,LPMMSSHOWPART lpMMSShowPart)
{
//	RECT rect;
	LPMMSBROWSER lpMMSBrowser;
	HICON hIcon;
	int x,y;


		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		switch(lpMMSShowPart->iFileType)
		{
			case MMSTYPE_UNKNOWFILE:	// ����ʶ���ļ�����
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_SYSLOGO ), IMAGE_ICON,
					32, 32, 0 ) ;  // װ��ָ����ͼ��
				break;

			case MMSTYPE_BMPFILE:  // ͼ���ļ�
			case MMSTYPE_GIFFILE:
			case MMSTYPE_JPGFILE:
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_CALENDAR ), IMAGE_ICON,
					32, 32, 0 ) ;  // װ��ָ����ͼ��
				break;

			case MMSTYPE_3GPFILE: // �����ļ�
			case MMSTYPE_MP3FILE:  
			case MMSTYPE_AMRFILE:
			case MMSTYPE_MIDFILE:
			case MMSTYPE_WAVFILE:
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APP_MP3 ), IMAGE_ICON,
					32, 32, 0 ) ;  // װ��ָ����ͼ��
				break;
			case MMSTYPE_SMILFILE:				
				hIcon = (HICON)LoadImage( NULL, MAKEINTRESOURCE( OIC_APPLICATION ), IMAGE_ICON,
					32, 32, 0 ) ;  // װ��ָ����ͼ��
				break;
			default:
				return 0;
		}

		x = lpMMSShowPart->iShowPos * ICON_PARTWIDTH;
		y = 4;
		
		// �õ��ı���ʾ�Ŀ�ʼλ��
		DrawIconEx(hdc,
				(int)x,
				(int)y,
				hIcon,
				0,
				0,
				0,
				0,
				DI_NORMAL);

		DestroyIcon(hIcon);  //  �ƻ�ͼ��

		return 0;
}



// **************************************************
// ������static int GetPart(HWND hWnd,POINT Point)
// ������
// 	IN hWnd -- ���ھ��
// 	IN Point -- ��ǰ�ĵ��λ��
// 
// ����ֵ���ɹ���������λ�ö�ӦPART�ĵ�����,���򷵻�-1
// �����������õ�����λ�ö�ӦPART��
// ����: 
// **************************************************
static int GetPart(HWND hWnd,POINT Point)
{
//	LPMMSSHOWPART lpMMSShowPart;
	RECT rect;
	int i,iPartNum;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->lpBodyDetail == NULL)
			return -1;

		iPartNum = lpMMSBrowser->lpBodyDetail->iPartNum;

		for (i = 0; i < iPartNum ; i++)
		{
			if (lpMMSBrowser->lpMMSShowPart[i].iFileType != MMSTYPE_TEXTFILE)
			{
				// �� ICON ���͵��ļ�
				rect.left = lpMMSBrowser->lpMMSShowPart[i].iShowPos * ICON_PARTWIDTH + 4;
				rect.top  = 4 ;
				rect.right = rect.left + 32;
				rect.bottom  = rect.top + 32 ;
				if (PtInRect(&rect,Point) != 0)
					return i;
			}
		}

		return -1;
}

/**************************************************
������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_LBUTTONDOWN��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
	LPMMSBROWSER lpMMSBrowser;
//	HWND hImgBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		point.x=LOWORD(lParam);
		point.y=HIWORD(lParam);
		

		lpMMSBrowser->iCurPart = GetPart(hWnd,point); // �õ���ǰ�����ڵ�λ��
		if (lpMMSBrowser->iCurPart != -1)
		{  // ����Ч��PART
			SetCapture(hWnd);
			SetTimer(hWnd,ID_POPUPMENU,DELAY_POPUPMENU,NULL);
		}

		return 0;
}


/**************************************************
������static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- MAKELPARAM(X,Y),�������
����ֵ����
��������������WM_LBUTTONUP��Ϣ��
����: 
************************************************/
static LRESULT DoLButtonUp(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	POINT point;
	LPMMSBROWSER lpMMSBrowser;
//	LPMMSSHOWPART lpMMSShowPart;
	int iIndex;

		if (GetCapture() != hWnd)
		{
			//��ǰ����û��ץס���,�����κδ���
			return 0;
		}
		KillTimer(hWnd,ID_POPUPMENU);
		ReleaseCapture(); // �ͷ����

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		point.x=LOWORD(lParam);  // �õ���ǰ��
		point.y=HIWORD(lParam);

		iIndex = GetPart(hWnd,point); // �õ���ǰ�����ڵ�λ��
		if (iIndex == -1)
		{
			lpMMSBrowser->iCurPart = -1;
			return 0; // �㵽��Ч��PART
		}
		if (iIndex == lpMMSBrowser->iCurPart)
		{
			// ��ǰ�㵽��ЧPART
			PlayCurrentPart(hWnd,&lpMMSBrowser->lpMMSShowPart[iIndex]); // ��ʾ�򲥷ŵ�ǰ��PART
		}
		lpMMSBrowser->iCurPart = -1;

		return 0;
}


// **************************************************
// ������static BOOL PlayCurrentPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// ������
// 	IN hWnd -- ���ھ��
//  IN lpMMSShowPart -- Ҫ��ʾ��PART
// 
// ����ֵ���ɹ�����TRUE,���򷵻�FALSE
// ������������ʾ�򲥷ŵ�ǰ��PART
// ����: 
// **************************************************
static BOOL PlayCurrentPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
//	TCHAR lpFileName[MAX_PATH];	
//	UINT uImgType;

		switch(lpMMSShowPart->iFileType)
		{
				case MMSTYPE_BMPFILE:  // ͼ���ļ�
				case MMSTYPE_GIFFILE:
				case MMSTYPE_JPGFILE:
					PreViewImgPart(hWnd,lpMMSShowPart);
					break;
				case MMSTYPE_SMILFILE:
					PlaySmilPart(hWnd,lpMMSShowPart);
					break;
				case MMSTYPE_3GPFILE: // �����ļ�
					break;
				case MMSTYPE_AMRFILE:
					break;
				case MMSTYPE_MIDFILE:
					break;
				case MMSTYPE_WAVFILE:
					break;
				case MMSTYPE_MP3FILE:  
					break;
		}
		return TRUE;
}


// **************************************************
// ������static BOOL SavePartData(HWND hWnd,LPMMSSHOWPART lpMMSShowPart,LPTSTR lpFileName,BOOL bNameFile)
// ������
//	IN hWnd -- ���ھ��
//	IN lpMMSShowPart -- Ҫ�����PARTָ��
//  IN lpFileName -- Ҫ�����ݱ�����ļ���
//  IN bNameFile -- �Ƿ�Ϊ�����ļ�,�����TRUE,���û��Ѿ�ָ�����ļ���,�����Զ�ָ���ļ���,�������ļ���
// 
// ����ֵ���ɹ�TRUE�����򷵻�FALSE
// �������������浱ǰPART���ļ����ݵ��ļ���
// ����: 
// **************************************************
static BOOL SavePartData(HWND hWnd,LPMMSSHOWPART lpMMSShowPart,LPTSTR lpFileName,BOOL bNameFile)
{
	
	HANDLE hFile;
	DWORD dwWriteLen;
	LPTSTR lpData;
	BOOL bFileNameOK = FALSE;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		if (bNameFile == FALSE)
		{
			// û��ָ���ļ���,��Ҫָ���ļ���
			strcpy(lpFileName,"\\Mobile\\temp\\"); // �õ��ļ�Ŀ¼,��ŵ���ʱ�ļ���

			if (lpMMSShowPart->lpMIME_Part->lpContent_Location)
			{  //  PART�Ѿ������ļ���
				if (strlen(lpMMSShowPart->lpMIME_Part->lpContent_Location))
				{
					strcat(lpFileName,lpMMSShowPart->lpMIME_Part->lpContent_Location); // �õ��ļ���
					bFileNameOK = TRUE;
				}
			}
			if (bFileNameOK == FALSE)
			{  //  ��û�еõ��ļ���
					strcat(lpFileName,"default"); // �õ��ļ���
					bFileNameOK = TRUE;
			}
		}

		hFile = CreateFile( lpFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
		if( hFile == INVALID_HANDLE_VALUE )
		{
			return FALSE;
		}
		lpData = lpMMSBrowser->MIME_Body.lpData + lpMMSShowPart->lpMIME_Part->uPos;
		WriteFile(hFile,lpData,lpMMSShowPart->lpMIME_Part->uLen,&dwWriteLen,NULL);
		CloseHandle(hFile);

		return TRUE;
}


// **************************************************
// ������static void ShowImage(HWND hWnd,LPTSTR lpFileName,UINT uImgType)
// ������
//	IN hWnd -- ���ھ��
//  IN lpFileName -- Ҫ��ʾ��ͼ����ļ���
//  IN uImgType -- ͼ������
// 
// ����ֵ����
// ������������ʾһ���ļ���
// ����: 
// **************************************************
static void ShowImage(HWND hWnd,LPTSTR lpFileName,UINT uImgType)
{
	HWND hImgBrowser;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		hImgBrowser = GetDlgItem(hWnd,ID_IMGVIEW);
//		SendMessage(hImgBrowser,IMB_SETIMGFILE,(WPARAM)uImgType,(LPARAM)lpFileName);

		if (lpFileName)
		{  //��ʾһ��ͼ���ļ�
			SendMessage(hImgBrowser,IMB_SETIMGFILE,(WPARAM)uImgType,(LPARAM)lpFileName);
			ShowWindow(hImgBrowser,SW_SHOW);
			UpdateWindow(hImgBrowser);
			lpMMSBrowser->bImgShow = TRUE;
			//SetCapture(hWnd);
		}
		else
		{
			// û��ͼ���ļ�Ҫ��ʾ
			ShowWindow(hImgBrowser,SW_HIDE);
			SendMessage(hImgBrowser,IMB_SETIMGFILE,(WPARAM)uImgType,(LPARAM)lpFileName);
			lpMMSBrowser->bImgShow = FALSE;
			//ReleaseCapture();
		}
}

/**************************************************
������static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
������
	IN hWnd -- ���ھ��
	IN wParam -- ����
	IN lParam -- ��Ϣ����
����ֵ����
��������������WM_LBUTTONUP��Ϣ��
����: 
************************************************/
static LRESULT DoTimer(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPMMSBROWSER lpMMSBrowser;
	LPMMSSHOWPART lpMMSShowPart;
	HMENU hMenu;
	POINT point;
	UINT idCommand;


		KillTimer(hWnd,ID_POPUPMENU);  // ɱ��ʱ��
		ReleaseCapture(); // �ͷ����

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->iCurPart >= lpMMSBrowser->iPartNum || lpMMSBrowser->iCurPart == -1)
		{
			// ��Ч������
			return 0;
		}
		lpMMSShowPart = &lpMMSBrowser->lpMMSShowPart[lpMMSBrowser->iCurPart]; // ��ʾ�򲥷ŵ�ǰ��PART
		
		hMenu = CreateMMSMenu(lpMMSShowPart->iFileType); // ����һ�����Դ���ǰ�ļ����͵Ĳ˵�
		
		if (hMenu == NULL)
		{  //  û�гɹ������˵�
			return 0;
		}

		point.x = (lpMMSShowPart->iShowPos + 1) * ICON_PARTWIDTH ;// �õ���ǰҪ�����˵���λ��
		point.y =  4 ;// �õ���ǰҪ�����˵���λ��
		ClientToScreen(hWnd,&point); // ������ת��Ϊ��Ļ����
		idCommand = TrackPopupMenu( hMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_NONOTIFY, point.x, point.y, 0, hWnd, NULL ); // �����˵�
		DestroyMenu( hMenu );
		
		switch(idCommand)
		{
			case IDM_PREVIEW:  // Ԥ��ͼ���ļ�
				PreViewImgPart(hWnd,lpMMSShowPart);
				break;
			case IDM_PLAY:  // ���������ļ�
				break;
			case IDM_SAVE:
				SaveAsPart(hWnd,lpMMSShowPart);
				break; // ����ǰPART�����ݱ��浽�ļ�
			case IDM_SMILPLAY:  // ����SMIL�ļ�
				PlaySmilPart(hWnd,lpMMSShowPart);
				break; 
		}

		return 0;
}


/**************************************************
������static HMENU CreateMMSMenu(UINT iFileType)
������
	IN iFileType -- ��ǰ���ļ�����
����ֵ����
��������������һ������PART�Ĳ˵���Ϣ��
����: 
************************************************/
static HMENU CreateMMSMenu(UINT iFileType)
{
	HMENU hMenu;
	MENUITEMINFO	InfoItem;

		if (MMSTYPE_TEXTFILE == iFileType)
			return NULL; // ���ı��ļ�������Ҫ�˵�

		hMenu = CreatePopupMenu();

		memset( &InfoItem, 0, sizeof(MENUITEMINFO) );
		InfoItem.cbSize = sizeof(MENUITEMINFO);
		InfoItem.fMask = MIIM_TYPE | MIIM_ID;
		InfoItem.fType = MFT_STRING;

		switch(iFileType)
		{
			case MMSTYPE_UNKNOWFILE: 
				break; // û����������
			case MMSTYPE_BMPFILE:
			case MMSTYPE_GIFFILE:
			case MMSTYPE_JPGFILE:
				// ��ͼ���ļ���������ʾ
				InfoItem.dwTypeData = ("Ԥ��");
				InfoItem.cch = 4;
				InfoItem.wID = IDM_PREVIEW;
				InsertMenuItem( hMenu, 0, TRUE, &InfoItem );
				break;
			case MMSTYPE_MP3FILE:
			case MMSTYPE_3GPFILE:
			case MMSTYPE_AMRFILE:
			case MMSTYPE_MIDFILE:
			case MMSTYPE_WAVFILE:
				// �������ļ������Բ���
				InfoItem.dwTypeData = ("����");
				InfoItem.cch = 4;
				InfoItem.wID = IDM_PLAY;
				InsertMenuItem( hMenu, 0, TRUE, &InfoItem );
				break;
			case MMSTYPE_SMILFILE:
				// �������ļ������Բ���
				InfoItem.dwTypeData = ("����");
				InfoItem.cch = 4;
				InfoItem.wID = IDM_SMILPLAY;
				InsertMenuItem( hMenu, 0, TRUE, &InfoItem );
				break;
		}
		////////

		InfoItem.dwTypeData = ("����");
		InfoItem.cch = 4;
		InfoItem.wID = IDM_SAVE;
		InsertMenuItem( hMenu, 1, TRUE, &InfoItem );

		return hMenu;
}

/**************************************************
������static void PreViewImgPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
������
	IN iFileType -- ��ǰ���ļ�����
����ֵ����
������������ʾһ��ͼ��PART��
����: 
************************************************/
static void PreViewImgPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	TCHAR lpFileName[MAX_PATH];	
	UINT uImgType;

		switch(lpMMSShowPart->iFileType)
		{
			case MMSTYPE_BMPFILE:  // ͼ���ļ�
				uImgType = IMG_BMPFILE;
				break;
			case MMSTYPE_GIFFILE:
				uImgType = IMG_GIFFILE;
				break;
			case MMSTYPE_JPGFILE:
				uImgType = IMG_JPGFILE;
				break;
			default:
				return ;
		}

		SavePartData(hWnd,lpMMSShowPart,lpFileName,FALSE);
		ShowImage(hWnd,lpFileName,uImgType);
}

// **************************************************
// ������static void PlaySmilPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// ������
//	IN hWnd -- ���ھ��
//	IN lpMMSShowPart -- Ҫ�����PARTָ��
// 
// ����ֵ����
// ��������������һ��SMILƬ��
// ����: 
// **************************************************
static void PlaySmilPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	TCHAR lpFileName[MAX_PATH];	
	LPMMSBROWSER lpMMSBrowser;
	LPMMSSHOWPART lpCurPart;
	int i;

		KillTimer(hWnd,ID_POPUPMENU);  // ɱ��ʱ��
		ReleaseCapture(); // �ͷ����

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		// smil ��Ҫ�������е�PART������
		// ���ȱ�������PART�����ݵ��ļ�
		for (i = 0 ; i < lpMMSBrowser->iPartNum; i++)
		{
			lpCurPart = &lpMMSBrowser->lpMMSShowPart[i];
			if (lpCurPart != lpMMSShowPart)
			{
				// �ǵ�ǰ��SMIL PART���ں��汣��
				SavePartData(hWnd,lpCurPart,lpFileName,FALSE);
			}
		}
		
		// �ȱ��浱ǰ��SMIL PART
		SavePartData(hWnd,lpMMSShowPart,lpFileName,FALSE);
		PlaySmil(hWnd,lpFileName);
}

// **************************************************
// ������static void PlaySmil(HWND hWnd,LPTSTR lpFileName)
// ������
//	IN hWnd -- ���ھ��
//  IN lpFileName -- Ҫ���ŵĵ�SMIL���ļ���
// 
// ����ֵ����
// ��������������һ��SMIL�ļ���
// ����: 
// **************************************************
static void PlaySmil(HWND hWnd,LPTSTR lpFileName)
{
	HWND hSMILBrowser;
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		hSMILBrowser = GetDlgItem(hWnd,ID_SMILVIEW);
		SendMessage(hSMILBrowser,SMM_LOADSMIL,0,(LPARAM)lpFileName);

		if (lpFileName)
		{  //��ʾһ��ͼ���ļ�
			ShowWindow(hSMILBrowser,SW_SHOW);
			UpdateWindow(hSMILBrowser);
			lpMMSBrowser->bImgShow = TRUE;
			//SetCapture(hWnd);
		}
		else
		{
			// û��ͼ���ļ�Ҫ��ʾ
			ShowWindow(hSMILBrowser,SW_HIDE);
			lpMMSBrowser->bImgShow = FALSE;
			//ReleaseCapture();
		}
}


// **************************************************
// ������static void SaveAsPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
// ������
//	IN hWnd -- ���ھ��
//	IN lpMMSShowPart -- Ҫ�����PARTָ��
// 
// ����ֵ����
// ��������������ǰ��PART���浽һ��ָ�����ļ���
// ����: 
// **************************************************
static void SaveAsPart(HWND hWnd,LPMMSSHOWPART lpMMSShowPart)
{
	TCHAR lpFileName[MAX_PATH];	

		memset(lpFileName,0,MAX_PATH);
		if (GetFileName(hWnd,lpFileName) == TRUE)
		{  // �õ�Ҫ������ļ���
			SavePartData(hWnd,lpMMSShowPart,lpFileName,TRUE);
		}
}

// **************************************************
// ������static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName)
// ������
// 	IN hDlg -- ���ھ��
// 	OUT lpFileName -- ���صõ����ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������õ��ļ�����
// ����: 
// **************************************************
static BOOL GetFileName(HWND hDlg,TCHAR *lpFileName)
{
    TCHAR OpenTitle[256];
	TCHAR szFilter[MAX_PATH]; 	
	OPENFILENAME OpenFileName;
	TCHAR lpstrFilter[30];
	TCHAR strFile[MAX_PATH];
	TCHAR TitleText[10];


		strcpy(OpenTitle,"�����ļ�"); // ���ñ���
		strcpy(szFilter,"*.abc"); // ���ù�����

		memset(lpstrFilter,0,sizeof(lpstrFilter));
		sprintf(lpstrFilter, TEXT("%s"), szFilter);
		sprintf(lpstrFilter+strlen(szFilter)+1, TEXT("%s"), TEXT("*.*"));   
	    OpenFileName.Flags=OFN_LONGNAMES|OFN_OVERWRITEPROMPT; // ���ñ�� -- ֧�ֳ��ļ�����������ʾ

		memset(strFile,0,sizeof(strFile));
		sprintf(TitleText, TEXT("%s"), OpenTitle);

		OpenFileName.lStructSize=sizeof(OPENFILENAME); 
		OpenFileName.hwndOwner=hDlg; // ���ô��ھ��
		OpenFileName.lpstrFilter=lpstrFilter; //NULL
		OpenFileName.nFilterIndex=1; 
		OpenFileName.lpstrFile=lpFileName; 
		OpenFileName.nMaxFile=MAX_PATH; 
		OpenFileName.lpstrFileTitle=strFile;  //only filename an extension(withou t path information) 
		OpenFileName.nMaxFileTitle=MAX_PATH; 
		OpenFileName.lpstrInitialDir="\\mobile"; 
		OpenFileName.lpstrTitle=TitleText;   
		OpenFileName.nFileOffset=0; 
		OpenFileName.nFileExtension=0; 
		OpenFileName.lpstrDefExt=NULL; 
		if( GetSaveFileName(&OpenFileName) == FALSE)  // �õ������ļ���
			return FALSE; // �õ��ļ���ʧ��
		return TRUE;
}

// **************************************************
// ������static LRESULT DoClick(HWND hWnd)
// ������
//	IN hWnd -- ���ھ��
// 
// ����ֵ����
// ����������ͼ����ͼ�����Ϣ��
// ����: 
// **************************************************
static LRESULT DoClick(HWND hWnd)
{
	LPMMSBROWSER lpMMSBrowser;

		lpMMSBrowser = (LPMMSBROWSER)GetWindowLong(hWnd,0); // �õ�MMS�����ṹָ��
		ASSERT(lpMMSBrowser);

		if (lpMMSBrowser->bImgShow == TRUE)
		{
			// ��ǰ������ʾͼ��,������ʾ����
			ShowImage(hWnd,NULL,0);
			PlaySmil(hWnd,NULL);
			return 0;
		}
		return 0;
}

