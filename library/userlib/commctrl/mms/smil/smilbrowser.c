/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����SMIL�����
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-04-29
���ߣ��½��� JAMI
�޸ļ�¼��
**************************************************/
#include "esmilbrowser.h"
#include "smilstruct.h"
//#include "DebugWnd.h"
#include "parsesmil.h"
//#include "ASSERT.H"
#include "gif2bmp.h"
#include "efilepth.h"
#include "jpegapi.h"
#include "playamr.h"



// **************************************************
LPTSTR classSMILBROWSER = TEXT("SMILBROWSER");

typedef struct SMILBROWSERSTRUCT{
	LPSMIL lpSmil ; // SMIL �ṹָ�룬���浱ǰ��SMIL����
	BOOL bExitSmil ; // �Ƿ�Ҫ�˳���ǰ�ģӣͣɣ���ʾ
	LPTSTR lpSmilFileName; // ��ǰ�򿪵�SMIL�ļ�
	int iCurParIndex; // ��ǰ������ʾ�Ļõ�����
	HDC hMemoryDC; // ��ŵ�ǰ����Ļ״̬
	HBITMAP hMemoryBitmap; // �ڴ�λͼ,����ڴ�DC����ŵ�ǰ����Ļ״̬
	BOOL bThreadExist ;  //��ǰ�Ƿ�����߳�

}SMILBROWSER,*LPSMILBROWSER;

// �����ѯ�ȴ����

#define TIMEINTERVAL	10 // ��msΪ��λ

// ����ͼ���ļ�������
#define IMGTYPE_GIF		1000	// GIF�ļ�
#define IMGTYPE_JPEG	1001	// JPEG�ļ�
#define IMGTYPE_UNKNOW	1002	// ����ʶ��ͼ���ļ�

// ���������ļ�������
#define AUDIOTYPE_AMR		1000	// AMR�ļ�
#define AUDIOTYPE_MIDI		1001	// MIDI�ļ�
#define AUDIOTYPE_UNKNOW	1002	// ����ʶ��ͼ���ļ�

// **************************************************
// **************************************************
// ��������
// **************************************************
extern LPTSTR SMILBufferAssign(const LPTSTR pSrc);


LRESULT CALLBACK SMILBrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint);
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam);


static LRESULT LoadSmilFile(HWND hWnd,WPARAM wParam,LPARAM lParam);
static LPPAR GetIndexPar(LPSMIL lpSmil,int iIndex);
static void ShowAPar(HWND hWnd,LPPAR lpCurPar);
static LPREGION GetRegion(HWND hWnd,LPTSTR lpRegionID);

DWORD WINAPI ShowParThread(LPVOID lpVoid);
static void DrawParImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static void DrawParText(HWND hWnd,LPTEXT lpText);
static LPTSTR GetFullName(HWND hWnd,LPTSTR lpFileName);

static int GetNextPicTime(HGIF hGif,int iIndex);
static BOOL NeedExitThread(HWND hWnd);
static int GetTotalPic(HGIF hGif);
static BOOL GetPicSize(HGIF hGif,LPSIZE lpSize);
static void ClearWindow(HWND hWnd);
static void ShowCurStatus(HWND hWnd,HDC hdc);
static void AdjustWindowSize(HWND hWnd,LPSMIL lpSmil);
static void AdjustRegionSize(HWND hWnd,LPSMIL lpSmil);

static void DrawParImageABC(HWND hWnd,LPIMG lpImg,int iDelayTime);

static UINT GetImageType(LPTSTR lpImg);
static void DrawParGifImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static void DrawParJpegImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static void DrawUnknowImage(HWND hWnd,LPIMG lpImg,int iDelayTime);
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize);


static UINT GetAudioType(LPTSTR lpAudio);
static void PlayParAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay);
static void PlayParAmrAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay);
static void StopPlayThread(LPSMILBROWSER lpSmilBrowser);

static void DrawSmilBitmap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop);

// **************************************************
// ������ATOM RegisterSMILBrowersClass( HINSTANCE hInst )
// ������
// 	IN hInstance -- ʵ�����
// 
// ����ֵ������ע����Ľ��
// ����������ע��SMIL�����
// ����: 
// **************************************************
ATOM RegisterSMILBrowersClass( HINSTANCE hInst )
{
    WNDCLASS wc;

    wc.style = 0;
    wc.lpfnWndProc = SMILBrowersProc;  // ���ù��̾��
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = hInst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW );
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(LTGRAY_BRUSH);  // ���ñ���
    wc.lpszMenuName = 0;
    wc.lpszClassName = classSMILBROWSER;  // ��������

    return (BOOL)(RegisterClass( &wc ));  // ��ϵͳע��
}


// **************************************************
// ������LRESULT CALLBACK SMILBrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN uMsg -- ������Ϣ
// 	IN wParam -- ��Ϣ����
// 	IN lParam -- ��Ϣ����
// 
// ����ֵ�����ظ���Ϣ�Ĵ�����
// �����������ؼ���Ϣ���̺���
// ����: 
// **************************************************
LRESULT CALLBACK SMILBrowersProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( uMsg )
	{
	  case WM_PAINT:  // ����
		  hdc = BeginPaint(hWnd, &ps);
		  ShowCurStatus(hWnd,hdc);
		  EndPaint(hWnd, &ps);
		  return 0;
   	  case WM_LBUTTONDOWN:
		  return DoLButtonDown(hWnd,wParam,lParam);
//	  case WM_MOUSEWHEEL:
//		  return DoMouseWheel(hWnd,wParam,lParam);
	  case WM_KEYDOWN:
		  DoKeyDown(hWnd,wParam,lParam);
		  break;
	  case WM_CREATE:
	      DoCreate(hWnd,wParam,lParam);
		  return 0;
	  case WM_DESTROY:
		  return DoDestory(hWnd,wParam,lParam);


	  case SMM_LOADSMIL: // װ��һ��SMIL�ļ�
		  return LoadSmilFile(hWnd,wParam,lParam);
	  default:
    	  return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	return 0;
}

// **************************************************
// ������static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����0�����򷵻�-1��
// ���������������ؼ�������WM_CREATE��Ϣ��
// ����: 
// **************************************************
static LRESULT DoCreate(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	HDC hdc;
	RECT rect;
	
		lpSmilBrowser = (LPSMILBROWSER)malloc(sizeof(SMILBROWSER)); // ������������ݻ���

		if (lpSmilBrowser == NULL)
		{
			return -1 ; // �����ڴ�ʧ��
		}

		// ��ʼ��lpSmilBrowser�ṹ
		lpSmilBrowser->bExitSmil = FALSE;
		lpSmilBrowser->lpSmil = NULL;
		lpSmilBrowser->lpSmilFileName = NULL; // ��ǰ�򿪵�SMIL�ļ�
		lpSmilBrowser->iCurParIndex = 0; // ��ǰ������ʾ�Ļõ�����
		lpSmilBrowser->bThreadExist = FALSE;  // ��û���߳�

			
		// �õ���ǰ��Ļ�ĳ�ʼ��״̬

		GetClientRect(hWnd,&rect); // �õ����ھ���

		hdc = GetDC(hWnd); // �õ����ڵ��豸���
		lpSmilBrowser->hMemoryDC = CreateCompatibleDC(hdc); // ����һ���ڴ�DC
		lpSmilBrowser->hMemoryBitmap = CreateCompatibleBitmap(hdc,rect.right,rect.bottom); // ������ǰ���ڴ�λͼ
		lpSmilBrowser->hMemoryBitmap = SelectObject(lpSmilBrowser->hMemoryDC,lpSmilBrowser->hMemoryBitmap); // ����ǰ��λͼ��Ϣѡ���ڴ�DC��������ԭ����λͼ

		FillRect(lpSmilBrowser->hMemoryDC,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH)); // ��ʼ������Ϊ��ɫ
//		FillRect(lpSmilBrowser->hMemoryDC,&rect,(HBRUSH)GetStockObject(DKGRAY_BRUSH)); // ��ʼ������Ϊ��ɫ

		ReleaseDC(hWnd,hdc); // �ͷ��豸���

		SetWindowLong(hWnd,0,(LONG)lpSmilBrowser); // ����lpSmilBrowserָ�뵽����

		JPEG_Init(); // ��ʼ��JPEG
		return 0;
}

// **************************************************
// ������static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ����
// 
// ����ֵ����
// �����������ƻ��ؼ�������WM_DESTROY��Ϣ��
// ����: 
// **************************************************
static LRESULT DoDestory(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
		ASSERT(lpSmilBrowser);

		StopPlayThread(lpSmilBrowser);

		
		lpSmilBrowser->hMemoryBitmap = SelectObject(lpSmilBrowser->hMemoryDC,lpSmilBrowser->hMemoryBitmap); // ����ǰ��λͼ��Ϣѡ���ڴ�DC��������ԭ����λͼ

		DeleteObject( lpSmilBrowser->hMemoryBitmap ); // ɾ���������ڴ�λͼ
		DeleteDC(lpSmilBrowser->hMemoryDC);
		free(lpSmilBrowser);

		JPEG_Deinit(); // ����JPEG
		return 0;
}
// **************************************************
// ������static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint)
// ������
// 	IN hWnd -- ���ھ��
//	 IN hdc -- �豸���
// 	IN rcPaint -- Ҫ���»��Ƶ�����
// 
// ����ֵ����
// �������������ƿؼ�������WM_PAINT��Ϣ��
// ����: 
// **************************************************
static void DoPAINT( HWND hWnd ,HDC hdc,RECT rcPaint)
{
	RECT rect;
		
		GetClientRect(hWnd,&rect);
		DrawText(hdc,"abcdefg",7,&rect,DT_CENTER);
}
// **************************************************
// ������static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- LOWORD ��ǰ�İ���״̬
//				  HIWORD �����Ĵ�С					
// 	IN lParam -- LOWORD ��ǰ��ˮƽλ��
//				  HIWORD ��ǰ�Ĵ�ֱλ��
// 
// ����ֵ����
// ���������������ֹ���������WM_MOUSEWHEEL��Ϣ��
// ����: 
// **************************************************
//static LRESULT DoMouseWheel(HWND hWnd,WPARAM wParam,LPARAM lParam)
//{
//	return TRUE;
//}

// **************************************************
// ������static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ���¼��������ֵ
// 	IN lParam -- ����
// 
// ����ֵ���ɹ�����TRUE�� ���򷵻�FALSE��
// �����������м����£�����WM_KEYDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	int nVirtKey ;


		nVirtKey = (int) wParam;    // virtual-key code
		return TRUE;
}

// **************************************************
// ������static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- ��ǰ����λ��
// 
// ����ֵ����
// �������������������£�����WM_LBUTTONDOWN��Ϣ��
// ����: 
// **************************************************
static LRESULT DoLButtonDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	DWORD x,y;
	x=(short)LOWORD(lParam);  // �õ���ǰ���ĵ�
	y=(short)HIWORD(lParam);
	return 0;
}



// **************************************************
// ������static LRESULT LoadSmilFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
// ������
// 	IN hWnd -- ���ھ��
// 	IN wParam -- ����
// 	IN lParam -- LPTSTR Ҫװ�ص�SMIL���ļ���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������װ�ص�SMIL�ļ�������SMM_LOADSMIL��Ϣ��
// ����: 
// **************************************************
static LRESULT LoadSmilFile(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	LPTSTR lpSmilFile; // Ҫ�򿪵�SMIL�ļ���
	DWORD dwFileSize; // �ļ�����
	HANDLE hFile ; //�ļ����
	LPTSTR lpContent; // ���ڶ�ȡ�ļ�����
	DWORD dwRead = 0;// �������ļ����ݵĳ���
	LPSMIL lpSmil = NULL; // SMIL �ṹָ�룬���浱ǰ��SMIL����
	HANDLE hThread; // ������ʾ�õƵ��߳̾��
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������

	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
	ASSERT(lpSmilBrowser);

	if (lpSmilBrowser->bThreadExist)
		StopPlayThread(lpSmilBrowser);  // ��ǰ��һ���߳����ڲ��ţ���ֹͣ��ǰ����

	if (lParam == NULL)
	{
		// ��ǰû��Ҫװ�ص�SMIL
		return FALSE;
	}
	RETAILMSG(1,(TEXT("+LoadSmilFile\r\n")));
	lpSmilFile = (LPTSTR)lParam; // �õ�Ҫװ�ص��ļ���
	RETAILMSG(1,(TEXT("Will Load FileName %s\r\n"),lpSmilFile));

	// ��ָ�����ļ�
	hFile=CreateFile(lpSmilFile,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile==INVALID_HANDLE_VALUE)
	{ // ���ļ�ʧ��
		RETAILMSG(1,(TEXT("Open File Failure\r\n")));
		return FALSE;
	}

	lpSmilBrowser->lpSmilFileName = SMILBufferAssign(lpSmilFile);

	dwFileSize = GetFileSize(hFile,NULL); // �õ��ļ�����
	
	lpContent = (LPTSTR)malloc((dwFileSize +1) * sizeof(TCHAR)) ; // ���仺������ȡ�ļ�����
	
	if (lpContent == NULL)
	{
		// �����ڴ�ʧ��
		CloseHandle(hFile); // �ر��ļ�
		return FALSE; //����ʧ��
	}

	ReadFile(hFile,lpContent,dwFileSize,&dwRead,NULL); // ��ȡ�ļ�����
	if (dwRead != dwFileSize)
	{
		// ��ȡ�ļ�ʧ��
		CloseHandle(hFile); // �ر��ļ�
		return FALSE; //����ʧ��
	}
	CloseHandle(hFile); // �Ѿ���ȡ�ļ����ݣ��ر��ļ�
	lpSmil = ParseSMIL(lpContent); // �����ļ����ݣ��ɹ�����һ����������SMILָ��

	if (lpSmil == NULL)
	{
		// ��������ʧ��
		return FALSE;
	}

//	AdjustWindowSize(hWnd,lpSmil); // ����SMIL��Ҫ�ĳߴ�������ڵĴ�С

	lpSmilBrowser->lpSmil = lpSmil; // ����SMILָ�뵽����


	hThread = CreateThread(NULL, 0, ShowParThread, (LPVOID)hWnd, 0, NULL); //�����߳�
	if(hThread)
	{ // �����̳߳ɹ����߳̾��������Ҫ�����ȹر�
		CloseHandle(hThread);  // �ر��߳̾��
	}

	RETAILMSG(1,(TEXT("-LoadSmilFile\r\n")));
	return TRUE; // ���سɹ�
}


// ******************************************************************
// ������DWORD WINAPI ShowParThread(LPVOID lpVoid)
// ������
//	IN  lpVoid - ָ�򴰿ھ��
// ����ֵ��0
// �������������߳�����ʾ�õƣ���ʾ��ɺ��˳�
// *******************************************************************
DWORD WINAPI ShowParThread(LPVOID lpVoid)
{
	HWND hWnd ;
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	LPPAR lpCurPar;
	int iIndex = 0;


	RETAILMSG(1,(TEXT("***************had Enter Thread***************\r\n")));
	hWnd = (HWND)lpVoid;
	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
	ASSERT(lpSmilBrowser);

	lpSmilBrowser->bThreadExist = TRUE;  // �߳��Ѿ��ɹ�����
	lpSmilBrowser->bExitSmil = FALSE;

	lpCurPar = GetIndexPar(lpSmilBrowser->lpSmil,0); // �õ���һ���õ�����
	lpSmilBrowser->iCurParIndex = 0;

	while(lpCurPar && (!lpSmilBrowser->bExitSmil)) // ��ǰ�Ļõ�������
	{
		RETAILMSG(1,(TEXT("Will Show %d PAR"),lpSmilBrowser->iCurParIndex));
		ShowAPar(hWnd,lpCurPar); // ��ʾ��ǰ�õ�
		lpCurPar = lpCurPar->next; // �õ���һ���õ�����
		lpSmilBrowser->iCurParIndex ++;
	}
	RETAILMSG(1,(TEXT("***************had Exit Thread***************\r\n")));
	lpSmilBrowser->bThreadExist = FALSE;  // �˳��߳�
	return 0;
}


// **************************************************
// ������static LPPAR GetIndexPar(LPSMIL lpSmil,int iIndex)
// ������
// 	IN lpSmil -- SMIL �ṹָ��
// 
// ����ֵ���ɹ�����ָ�������Ļõ�Ƭ�����ݣ����򷵻�NULL
// ���������� �õ�ָ�������õ����ݡ�
// ����: 
// **************************************************
static LPPAR GetIndexPar(LPSMIL lpSmil,int iIndex)
{
	LPPAR lpCurPar;
	int iCurIndex = 0;

	if (lpSmil == NULL)
		return NULL; // û��SMIL����

	if (lpSmil->lpBody == NULL)
		return NULL; // û��BODY����

	lpCurPar = lpSmil->lpBody->lpPar;

	while(lpCurPar)
	{
		if (iCurIndex == iIndex)
			break; // �Ѿ��ҵ�ָ�������Ļõ�Ƭ����
		lpCurPar =  lpCurPar->next ; // �õ���һ���õ�Ƭ����
	}

	return lpCurPar; // ����ָ�������õ�Ƭ����
}


// **************************************************
// ������static void ShowAPar(HWND hWnd,LPPAR lpCurPar)
// ������
// 	IN hWnd --	���ھ��
//  IN LPCurPar -- ��ǰ�Ļõ�Ƭ����ָ��
// 
// ����ֵ����
// ���������� ��ʾ��ǰ�õơ�
// ����: 
// **************************************************
static void ShowAPar(HWND hWnd,LPPAR lpCurPar)
{
	int iDelayTime = 10000;

	RETAILMSG(1,(TEXT("***************had enter Show Par***************\r\n")));
	if (lpCurPar == NULL)
		return ; // û�лõ�Ƭ����

	// �����Ļ
	ClearWindow(hWnd);
	
	// �ȴ���ʼʱ��
	if (lpCurPar->startTime)
	{	// ����õ�Ƭ����Ҫ�ȴ�startTimeʱ���ʼ���Żõ�Ƭ
		iDelayTime = lpCurPar->startTime;
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // �Ƿ���Ҫ�˳��߳�
				break;  // �˳��߳�
			Sleep(TIMEINTERVAL); // ��ʱͣ��
			iDelayTime -= TIMEINTERVAL; // ����������ʱ��
		}
	}
		
	iDelayTime = lpCurPar->durTime; // ���㵱ǰ�õ���Ҫ���ŵ�����ʱ��
	
	// ��������
	PlayParAudio(hWnd,lpCurPar->lpAudio,TRUE);
	
	// ��ʾ�ı�
	DrawParText(hWnd,lpCurPar->lpText);

	// ��ʾͼ��
	DrawParImage(hWnd,lpCurPar->lpImg, iDelayTime);
//	DrawParImageABC(hWnd,lpCurPar->lpImg, iDelayTime);

	// �ر�����
	PlayParAudio(hWnd,lpCurPar->lpAudio,FALSE);

	// �ȴ�����ʱ��
	if (lpCurPar->endTime != -1)
	{										 // �Ѿ����ʱ��			
		iDelayTime = (lpCurPar->endTime -  (lpCurPar->startTime  + lpCurPar->durTime) );
		if (iDelayTime)
		{	// �ȴ��õ�Ƭ���Ž�������Ҫ�ȴ�ʱ����뿪�õ�Ƭ
			while(iDelayTime > 0)
			{
				if (NeedExitThread(hWnd)) // �Ƿ���Ҫ�˳��߳�
					break;  // �˳��߳�
				Sleep(TIMEINTERVAL); // ��ʱͣ��
				iDelayTime -= TIMEINTERVAL; // ����������ʱ��
			}
		}
	}

	RETAILMSG(1,(TEXT("***************had exit Show Par***************\r\n")));

}

// **************************************************
// ������static void DrawParImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// ������
// 	IN hWnd --	���ھ��
//  IN lpImg -- ��ǰҪ��ʾ��ͼ��
//	IN iDelayTime -- ��ʾ��ǰλͼҪ������ʱ��
// 
// ����ֵ����
// ���������� ��ʾͼ��
// ����: 
// **************************************************
static void DrawParImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	UINT iImgType;
	// ͼ��֧��JPEG , WBMP , GIF
	
	if (lpImg == NULL)
	{
		// ����ʱ��
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // �Ƿ���Ҫ�˳��߳�
				break;  // �˳��߳�
			Sleep(TIMEINTERVAL); // ��ʱͣ��
			iDelayTime -= TIMEINTERVAL; // �Ѿ����10ms��ͣ��
		}
		return ; // û��ͼ����Ҫ��ʾ
	}
	iImgType = GetImageType(lpImg->lpSrc);
	switch(iImgType)
	{
			case IMGTYPE_GIF:
				 // GIF ͼ��
				DrawParGifImage(hWnd,lpImg,iDelayTime);
				break;
			case IMGTYPE_JPEG:
				 // JPEG ͼ��
				DrawParJpegImage(hWnd,lpImg,iDelayTime);
				break;
			default:
				DrawUnknowImage(hWnd,lpImg,iDelayTime);
				break;
	}
}

// **************************************************
// ������static void DrawParGifImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// ������
// 	IN hWnd --	���ھ��
//  IN lpImg -- ��ǰҪ��ʾ��ͼ��
//	IN iDelayTime -- ��ʾ��ǰλͼҪ������ʱ��
// 
// ����ֵ����
// ���������� ��ʾGIFͼ��
// ����: 
// **************************************************
static void DrawParGifImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	// GIF ͼ��
	HGIF hGif;
	LPTSTR lpFullImage; // ȫ·���ļ���
	LPREGION lpRegion;
	HDC hdc;
	int iNextPicTime = 0; // ����һ��ͼ������������ʱ��
	int iIndex = 0; // ��ǰͼ������
	int iTotalPic; // �õ��ܵ�ͼ�����
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	int iOrgx,iOrgy;
	SIZE Size;
	RECT rect;
	

		if (lpImg == NULL)
			return ; // û��ͼ����Ҫ��ʾ
	
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
		ASSERT(lpSmilBrowser);

		lpFullImage=GetFullName(hWnd,lpImg->lpSrc); // �õ�ͼ���ļ���ȫ·����
		if (lpFullImage==NULL)
			return ; // û�еõ�ȫ·���ļ���
		hGif = Gif_LoadFromFile( lpFullImage ) ; // װ��ͼ��
		if (hGif==NULL)
		{  // װ��ʧ��
			return;
		}
		iTotalPic = GetTotalPic(hGif); // �õ���ǰGIF��ͼ������
		GetPicSize(hGif,&Size);  // �õ�ͼ��ߴ�
		GetClientRect(hWnd,&rect); // �õ����ڳߴ�
		iOrgx = ((rect.right - rect.left ) - Size.cx ) /2 ; // ʹͼ����ʾ����Ļ������
		iOrgy = 0; //  ͼ����ʾ�ڶ���

		hdc = GetDC(hWnd); // �õ��豸���
		lpRegion = GetRegion(hWnd,lpImg->lpRegion); // �õ�ָ����REGION
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // �Ƿ���Ҫ�˳��߳�
				break;  // �˳��߳�
			if (iNextPicTime <= 0)
			{ // ��ʱ�������һ��ͼ��
				GIF_IMAGE gifimage;

					gifimage.dwSize = sizeof(GIF_IMAGE);
					Gif_GetImage(hGif,iIndex,&gifimage); // �õ���ǰ����ͼ�����Ϣ
					// ���Ƶ�ǰָ��������ͼ��
					Gif_DrawIndexEx(hGif,
					iIndex,
					lpSmilBrowser->hMemoryDC,
					lpRegion->left + gifimage.left + iOrgx,
					lpRegion->top + gifimage.top + iOrgy,
					lpRegion->width - gifimage.left,  // width of destination rectangle
					lpRegion->height - gifimage.top, // height of destination rectangle
					0,   // x-coordinate of source rectangle's upper-left 
							   // corner
					0,   // y-coordinate of source rectangle's upper-left 
							   // corner
					SRCCOPY  // raster operation code
				    );

					ShowCurStatus(hWnd,hdc); // ��ʾ��ǰ״̬����Ļ
				
					iNextPicTime = GetNextPicTime(hGif,iIndex); // �õ�����һ��ͼ���ʱ��
					iIndex ++; // ��һ��ͼ��
					if (iIndex >= iTotalPic)
						iIndex = 0; // �Ѿ������һ��ͼ��
			}
			Sleep(TIMEINTERVAL); // ��ʱͣ��
			iDelayTime -= TIMEINTERVAL; // ����������ʱ��
			iNextPicTime -= TIMEINTERVAL; // ���ٵ���һ��ͼ�������ʱ��
		}
		if (hGif)
			Gif_Destroy( hGif ) ; // �ͷ�GIF���
		ReleaseDC(hWnd,hdc); // �ͷ��豸���
}


// **************************************************
// ������static void DrawParJpegImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// ������
// 	IN hWnd --	���ھ��
//  IN lpImg -- ��ǰҪ��ʾ��ͼ��
//	IN iDelayTime -- ��ʾ��ǰλͼҪ������ʱ��
// 
// ����ֵ����
// ���������� ��ʾͼ��
// ����: 
// **************************************************
static void DrawParJpegImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	// JPEG ͼ��
	LPTSTR lpFullImage; // ȫ·���ļ���
	LPREGION lpRegion;
	HDC hdc;
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	RECT rect;
	HBITMAP hBitmap;
	int iOrgx,iOrgy;
	SIZE Size;
	
		if (lpImg == NULL)
			return ; // û��ͼ����Ҫ��ʾ

		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
		ASSERT(lpSmilBrowser);

		lpFullImage=GetFullName(hWnd,lpImg->lpSrc); // �õ�ͼ���ļ���ȫ·����
		if (lpFullImage==NULL)
			return ; // û�еõ�ȫ·���ļ���
		hdc = GetDC(hWnd); // �õ��豸���
		lpRegion = GetRegion(hWnd,lpImg->lpRegion); // �õ�ָ����REGION
		// ��ʾJPEGͼ��
		
		hBitmap = JPEG_LoadByName(lpFullImage,NULL); // �õ���ѹ�����JPEGͼ��

		GetBMPImageSize(hBitmap,&Size);  // �õ�ͼ��ߴ�
		GetClientRect(hWnd,&rect);
		iOrgx = ((rect.right - rect.left ) - Size.cx ) /2 ; // ʹͼ����ʾ����Ļ������
		iOrgy = 0; //  ͼ����ʾ�ڶ���
		
		if (hBitmap)
		{
			rect.left = lpRegion->left + iOrgx;
			rect.top = lpRegion->top + iOrgy;
			rect.right = rect.left + lpRegion->width;
			rect.bottom = rect.top + lpRegion->height;

			DrawSmilBitmap(lpSmilBrowser->hMemoryDC,hBitmap,rect,0,0,SRCCOPY);
			ShowCurStatus(hWnd,hdc); // ��ʾ��ǰ��״̬
		}
		
		DeleteObject(hBitmap);
		
		ReleaseDC(hWnd,hdc); // �ͷ��豸���

		// ����ʱ��
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // �Ƿ���Ҫ�˳��߳�
				break;  // �˳��߳�
			Sleep(TIMEINTERVAL); // ��ʱͣ��
			iDelayTime -= TIMEINTERVAL; // �Ѿ����10ms��ͣ��
		}
}

// **************************************************
// ������static void DrawUnknowImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
// ������
// 	IN hWnd --	���ھ��
//  IN lpImg -- ��ǰҪ��ʾ��ͼ��
//	IN iDelayTime -- ��ʾ��ǰλͼҪ������ʱ��
// 
// ����ֵ����
// ���������� ��ʾͼ��
// ����: 
// **************************************************
static void DrawUnknowImage(HWND hWnd,LPIMG lpImg,int iDelayTime)
{
	// JPEG ͼ��
	LPREGION lpRegion;
	HDC hdc;
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	
		if (lpImg == NULL)
			return ; // û��ͼ����Ҫ��ʾ

		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
		ASSERT(lpSmilBrowser);

		hdc = GetDC(hWnd); // �õ��豸���
		lpRegion = GetRegion(hWnd,lpImg->lpRegion); // �õ�ָ����REGION
		// ��ʾALT��������ı�

		ReleaseDC(hWnd,hdc); // �ͷ��豸���

		// ����ʱ��
		while(iDelayTime > 0)
		{
			if (NeedExitThread(hWnd)) // �Ƿ���Ҫ�˳��߳�
				break;  // �˳��߳�
			Sleep(TIMEINTERVAL); // ��ʱͣ��
		}
}


// **************************************************
// ������static void DrawParText(HWND hWnd,LPTEXT lpText)
// ������
// 	IN hWnd --	���ھ��
//  IN lpText -- ��ǰҪ��ʾ���ı�
// 
// ����ֵ����
// ���������� ��ʾ�ı�.
// ����: 
// **************************************************
static void DrawParText(HWND hWnd,LPTEXT lpText)
{
	LPTSTR lpFullImage;
	LPREGION lpRegion;
	HDC hdc;
	RECT rect;
	DWORD dwFileSize; // �ļ�����
	HANDLE hFile ; //�ļ����
	LPTSTR lpContent; // ���ڶ�ȡ�ļ�����
	DWORD dwRead = 0;// �������ļ����ݵĳ���
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	
		if (lpText == NULL)
			return ; // 
		
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
		ASSERT(lpSmilBrowser);

		lpFullImage=GetFullName(hWnd,lpText->lpSrc); // �õ��ı��ļ���ȫ·����
		if (lpFullImage==NULL)
			return ; // û�еõ�ȫ·���ļ���

		// ��ָ�����ı��ļ�
		hFile=CreateFile(lpFullImage,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (hFile==INVALID_HANDLE_VALUE)
		{ // ���ļ�ʧ��
			RETAILMSG(1,(TEXT("Open File Failure\r\n")));
			return ;
		}


		dwFileSize = GetFileSize(hFile,NULL); // �õ��ļ�����
		
		lpContent = (LPTSTR)malloc((dwFileSize +1) * sizeof(TCHAR)) ; // ���仺������ȡ�ļ�����

		if (lpContent == NULL)
		{
			// �����ڴ�ʧ��
			CloseHandle(hFile); // �ر��ļ�
			return ; //����ʧ��
		}
		memset(lpContent,0,((dwFileSize +1) * sizeof(TCHAR))); // ��ջ���

		ReadFile(hFile,lpContent,dwFileSize,&dwRead,NULL); // ��ȡ�ļ�����
		if (dwRead != dwFileSize)
		{
			// ��ȡ�ļ�ʧ��
			CloseHandle(hFile); // �ر��ļ�
			return ; //����ʧ��
		}
		CloseHandle(hFile); // �Ѿ���ȡ�ļ����ݣ��ر��ļ�

		hdc = GetDC(hWnd); // �õ��豸���
		lpRegion = GetRegion(hWnd,lpText->lpRegion); // �õ���REGION�����Ӧ��REGION�ṹ
		
		// �õ���ǰREGIONָ���ľ���
		rect.left = lpRegion->left;
		rect.top = lpRegion->top;
		rect.right = lpRegion->left + lpRegion->width;
		rect.bottom = lpRegion->top + lpRegion->height;

		DrawText(lpSmilBrowser->hMemoryDC,lpContent,strlen(lpContent),&rect,DT_CENTER); // ��ָ����Χ����ʾ�ı�
		ShowCurStatus(hWnd,hdc); // ��ʾ��ǰ��״̬

		ReleaseDC(hWnd,hdc); // �ͷ��豸���
		free(lpContent); // �ͷ��ı�����
}


// **************************************************
// ������static LPREGION GetRegion(HWND hWnd,LPTSTR lpRegionID)
// ������
// 	IN hWnd --	���ھ��
//  IN lpRegionID -- ָ����REGION ID
// 
// ����ֵ���ɹ�����ָ��ID��REGIONָ��,���򷵻�NULL
// ���������� �õ�ָ��ID��REGIONָ��.
// ����: 
// **************************************************
static LPREGION GetRegion(HWND hWnd,LPTSTR lpRegionID)
{
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	LPREGION lpRegion;

	
	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
		
	ASSERT(lpSmilBrowser);

	if (lpSmilBrowser->lpSmil == NULL)
		return NULL; // û��SMIL����

	if (lpSmilBrowser->lpSmil->lpHead == NULL)
		return NULL; // û��HEAD����

	if (lpSmilBrowser->lpSmil->lpHead->lpLayout == NULL)
		return NULL; // û��LAYOUT����

	lpRegion = lpSmilBrowser->lpSmil->lpHead->lpLayout->lpRegion; // �õ���һ��REGION�Ľṹ

	while(lpRegion)
	{
		if (stricmp(lpRegion->id,lpRegionID) == 0)
		{
			// �ҵ���Ӧ��REGION
			break; // ���ص�ǰ��REGION
		}
		// ��ǰREGION����Ҫ�ҵ�REGION
		lpRegion = lpRegion->next; // ָ����һ��REGION
	}
	return lpRegion;
}


// **************************************************
// ������static LPTSTR GetFullName(HWND hWnd,LPTSTR lpFileName)
// ������
// 	IN hWnd --	���ھ��
//  IN lpImage -- ��ǰ��ͼ���ļ�
// 
// ����ֵ���ɹ�ͼ���ļ���ȫ·����
// ���������� �õ�ͼ���ļ���ȫ·����.
// ����: 
// **************************************************
static LPTSTR GetFullName(HWND hWnd,LPTSTR lpFileName)
{
	LPTSTR lpFullName;
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	
		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
		ASSERT(lpSmilBrowser);

		lpFullName = (LPTSTR)malloc(MAX_PATH); // ����һ�����ȫ·���ļ����Ļ���
		if (lpFullName == NULL)
		{ 
			return NULL; // ����ʧ�ܣ����ؿ�
		}
		_splitpath(lpSmilBrowser->lpSmilFileName,NULL,lpFullName,NULL,NULL); // �õ�SMIL�ļ���·��

		strcat(lpFullName,lpFileName); // ��·���ӵ���ǰ���ļ�����

		return lpFullName; // ����ȫ·���ļ���
}


// **************************************************
// ������static int GetNextPicTime(HGIF hGif,int iIndex)
// ������
// 	IN hGif --	GIF�ļ����
//  IN iIndex -- ��ǰ��ͼ�������
// 
// ����ֵ�����ص�ǰͼ����һ��ͼ��ļ��ʱ��
// ���������� �õ���ǰͼ����һ��ͼ��ļ��ʱ��.
// ����: 
// **************************************************
static int GetNextPicTime(HGIF hGif,int iIndex)
{
	DWORD dwDelay;
	Gif_GetDelay(hGif,iIndex, &dwDelay ); // �õ���һ��ͼ��ʱ������

	return (dwDelay * 10 ); // Delay ��0.01s Ϊ��λ��ת����MS
}

// **************************************************
// ������static BOOL NeedExitThread(HWND hWnd)
// ������
// 	IN hWnd --	���ھ��
// 
// ����ֵ����Ҫ�˳��߳�ʱ����TRUE�����򷵻�FALSE
// ���������� �õ���ǰ�߳��Ƿ���Ҫ�˳�.
// ����: 
// **************************************************
static BOOL NeedExitThread(HWND hWnd)
{
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������

	
	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������

	ASSERT(lpSmilBrowser);

	return lpSmilBrowser->bExitSmil; // ���ص�ǰ���˳�����
}

// **************************************************
// ������static int GetTotalPic(HGIF hGif)
// ������
// 	IN hGif --	GIF�ļ����
// 
// ����ֵ�����ص�ǰ��GIF��ͼ�����
// ���������� �õ���ǰ��GIF��ͼ�����.
// ����: 
// **************************************************
static int GetTotalPic(HGIF hGif)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // ���ýṹ��С
		//�õ�GIF�������Ϣ
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
			return gifInfo.nIndexNum; // ���ص�ǰGIF��ͼ����
		return 0; // û�еõ�ͼ����
}

// **************************************************
// ������static BOOL GetPicSize(HGIF hGif,LPSIZE lpSize)
// ������
// 	IN hGif --	GIF�ļ����
//  OUT lpSize -- Ҫ���ص�ͼ��ĳߴ�
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE��
// ���������� �õ���ǰ��GIF��ͼ��ߴ�.
// ����: 
// **************************************************
static BOOL GetPicSize(HGIF hGif,LPSIZE lpSize)
{
	GIF_INFO gifInfo;

		if (lpSize == NULL)
			return 0;
		gifInfo.dwSize = sizeof(GIF_INFO); // ���ýṹ��С
		//�õ�GIF�������Ϣ
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
		{
			lpSize->cx = gifInfo.nWidth;
			lpSize->cy = gifInfo.nHeight;
			return TRUE; // ���سɹ�
		}
		return 0; // ����ʧ��
}

// **************************************************
// ������static void ShowCurStatus(HWND hWnd,HDC hdc)
// ������
// 	IN hWnd --	���ھ��
//  IN hdc -- �豸���
// 
// ����ֵ����
// ���������� ���������Ҫ��ʾ��ǰ���ڲ��ŵĻõƵ�ǰ��״̬��
// ����: 
// **************************************************
static void ShowCurStatus(HWND hWnd,HDC hdc)
{
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	RECT rect;


	lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������
	
	ASSERT(lpSmilBrowser);

	GetClientRect(hWnd,&rect); // �õ����ڴ�С
	// ���ڴ�DC�е����ݸ��Ƶ���ǰ��Ļ
	BitBlt( hdc, 0,0,rect.right,rect.bottom,
			lpSmilBrowser->hMemoryDC,0,0,SRCCOPY);
//	DoPAINT(hWnd,hdc,rect);
}


// **************************************************
// ������static void ClearWindow(HWND hWnd)
// ������
// 	IN hWnd --	���ھ��
// 
// ����ֵ����
// ���������� �����ǰ�Ĵ��ڡ�
// ����: 
// **************************************************
static void ClearWindow(HWND hWnd)
{
	LPSMILBROWSER lpSmilBrowser; //��ǰ�����������
	RECT rect;
	HDC hdc;


		lpSmilBrowser = (LPSMILBROWSER)GetWindowLong(hWnd,0); // �õ�SMIL���������

		ASSERT(lpSmilBrowser);

		GetClientRect(hWnd,&rect); // �õ����ھ���
		FillRect(lpSmilBrowser->hMemoryDC,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH)); // ��ʼ������Ϊ��ɫ
		
		hdc = GetDC(hWnd);
		ShowCurStatus(hWnd,hdc); // ����ǰ״̬��ʾ����Ļ
		ReleaseDC(hWnd,hdc);
}



// **************************************************
// ������static void AdjustWindowSize(HWND hWnd,LPSMIL lpSmil) 
// ������
// 	IN hWnd --	���ھ��
//	IN lpSmil -- SMIL�ṹָ��
// 
// ����ֵ����
// ����������  ����SMIL��Ҫ�ĳߴ�������ڵĴ�С��
// ����: 
// **************************************************
static void AdjustWindowSize(HWND hWnd,LPSMIL lpSmil) 
{
	UINT width = SMIL_DEFAILTWIDTH,height = SMIL_DEFAILTHEIGHT; // Ĭ��SMIL��С
	UINT x=0,y=0;

		if (lpSmil)
		{
			if (lpSmil->lpHead)
			{
				if (lpSmil->lpHead->lpLayout)
				{
					if (lpSmil->lpHead->lpLayout->lpRootLayout)
					{ // �õ��µ�SMIL�ߴ�
						width = lpSmil->lpHead->lpLayout->lpRootLayout->width;
						height = lpSmil->lpHead->lpLayout->lpRootLayout->height;
					}
				}
			}
		}

		if (width > SMIL_DEFAILTWIDTH)
		{
			width = SMIL_DEFAILTWIDTH;
			x = 0;
		}
		else
		{
			x = (SMIL_DEFAILTWIDTH - width) / 2;
		}
		if (height > SMIL_DEFAILTHEIGHT)
		{
			height = SMIL_DEFAILTHEIGHT;
		}

		SetWindowPos(hWnd,NULL,x,y,width,height,SWP_NOZORDER);

		AdjustRegionSize(hWnd,lpSmil);
}

// **************************************************
// ������static void AdjustRegionSize(HWND hWnd,LPSMIL lpSmil) 
// ������
// 	IN hWnd --	���ھ��
//	IN lpSmil -- SMIL�ṹָ��
// 
// ����ֵ����
// ����������  ����SMIL��Ҫ�ĳߴ����region�Ĵ�С��
// ����: 
// **************************************************
static void AdjustRegionSize(HWND hWnd,LPSMIL lpSmil) 
{
	LPREGION lpRegion;
	int width;
	RECT rect;
	
	if (lpSmil == NULL)
		return ; // û��SMIL����

	if (lpSmil->lpHead == NULL)
		return ; // û��HEAD����

	if (lpSmil->lpHead->lpLayout == NULL)
		return ; // û��LAYOUT����

	lpRegion = lpSmil->lpHead->lpLayout->lpRegion; // �õ���һ��REGION�Ľṹ

	GetClientRect(hWnd,&rect);

	width = rect.right - rect.left; // �õ����ڿ��

	while(lpRegion)
	{
		// ����REGION��Ҫ����ˮƽ����,ʹ���ڴ�������ʾ
		if (lpRegion->left + lpRegion->width > width)
		{
			// REGION���ұ߿���ڴ��ڵĿ��
			if (lpRegion->width > width)
			{
				// REGION�Ŀ�ȴ��ڴ��ڵĿ��
				lpRegion->left = 0;
				lpRegion->width = width;
			}
			else
			{
				// REGION�Ŀ��С�ڴ��ڵĿ��,ʹREGION����
				lpRegion->left = 0;
				lpRegion->width = width;
			}
		}
		// ��ǰREGION�Ѿ��������
		lpRegion = lpRegion->next; // ָ����һ��REGION
	}
	return;
}

// **************************************************
// ������static UINT GetImageType(LPTSTR lpImg)
// ������
//	IN lpImg -- ָ����ͼ���ļ�
// 
// ����ֵ������ָ����ͼ���ļ���ͼ������
// ����������  ָ����ͼ���ļ���ͼ�����͡�
// ����: 
// **************************************************
static UINT GetImageType(LPTSTR lpImg)
{
	TCHAR lpExt[MAX_PATH];
		
	_splitpath(lpImg,NULL,NULL,NULL,lpExt); // �õ�IMG�ļ�����չ��

	if (stricmp(lpExt,".GIF") ==0)
	{
		// ��GIF��չ��,��GIF�ļ�
		return IMGTYPE_GIF;
	}
	if ((stricmp(lpExt,".JPG") ==0) || (stricmp(lpExt,".JPEG") ==0))
	{
		// ��JPG �� JPEG ��չ��,��JPEG�ļ�
		return IMGTYPE_JPEG;
	}
	return IMGTYPE_UNKNOW;
}


// **************************************************
// ������static void DrawSmilBitmap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ����ʾ��Χ
//  IN xOffset - X����ƫ��
//  IN yOffset -- Y����ƫ��
//  IN dwRop -- ��ʾ״̬
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawSmilBitmap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
	HDC hMemoryDC;
	if(hBitMap==0)
		return;
	hMemoryDC=CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc, (short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top),hMemoryDC,(short)xOffset,(short)yOffset,dwRop);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}

// **************************************************
// ������static void PlayParAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
// ������
// 	IN hWnd --	���ھ��
//  IN lpAudio -- ��ǰҪ���ŵ�����
//  IN bPlay -- ��ʼ������������ֹͣ��������
// ����ֵ����
// ���������� ����������
// ����: 
// **************************************************
static void PlayParAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
{
	UINT iAudioType;
	// ����֧��AMR, ��ЩҲ����֧��MIDI
	
	if (lpAudio == NULL)
		return ; // û��������Ҫ����
	iAudioType = GetAudioType(lpAudio->lpSrc);
	switch(iAudioType)
	{
			case AUDIOTYPE_AMR:
				 // AMR �ļ�
				PlayParAmrAudio(hWnd,lpAudio,bPlay); // ��ʼ����
				break;
	}
}

// **************************************************
// ������static UINT GetAudioType(LPTSTR lpAudio)
// ������
//	IN lpAudio -- ָ���������ļ�
// 
// ����ֵ������ָ���������ļ�������
// ����������  ָ���������ļ������͡�
// ����: 
// **************************************************
static UINT GetAudioType(LPTSTR lpAudio)
{
	TCHAR lpExt[MAX_PATH];
		
	_splitpath(lpAudio,NULL,NULL,NULL,lpExt); // �õ�IMG�ļ�����չ��

	if ((stricmp(lpExt,".AMR") ==0) || (stricmp(lpExt,".3GP") ==0))
	{
		// �� AMR �� 3GP ��չ��,��AMR�ļ�
		return AUDIOTYPE_AMR;
	}
	if ((stricmp(lpExt,".MID") ==0) || (stricmp(lpExt,".MIDI") ==0))
	{
		// �� MID �� MIDI ��չ��,��MIDI�ļ�
		return AUDIOTYPE_MIDI;
	}
	return AUDIOTYPE_UNKNOW;
}

// **************************************************
// ������static void PlayParAmrAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
// ������
// 	IN hWnd --	���ھ��
//  IN lpAudio -- ��ǰҪ���ŵ������ļ�
//	IN bPlay -- �Ƿ�Ҫ��������
// 
// ����ֵ����
// ���������� ����AMR������
// ����: 
// **************************************************
static void PlayParAmrAudio(HWND hWnd,LPAUDIO lpAudio,BOOL bPlay)
{
	LPTSTR lpFullImage;

	if (lpAudio == NULL)
	{ // û�������ļ�
		return;
	}

	lpFullImage=GetFullName(hWnd,lpAudio->lpSrc); // �õ������ļ���ȫ·����
	if (lpFullImage==NULL)
		return ; // û�еõ�ȫ·���ļ���
	if (bPlay)
		PlayAMRFile(hWnd ,lpFullImage); // ����ָ����AMR�ļ�
	else
		PlayAMRFile(hWnd ,NULL); // ָֹͣ����AMR�ļ�
	
	free(lpFullImage); // �ͷ��ļ���
}


// **************************************************
// ������static void StopPlayThread(LPSMILBROWSER lpSmilBrowser)
// ������
// 	IN lpSmilBrowser --	SMIL������ṹָ��
// 
// ����ֵ����
// ���������� ֹͣ�����̡߳�
// ����: 
// **************************************************
static void StopPlayThread(LPSMILBROWSER lpSmilBrowser)
{
		lpSmilBrowser->bExitSmil = TRUE;

		// �ȴ��߳����
		while(1)
		{
			if (lpSmilBrowser->bThreadExist == FALSE)
				break; // �߳���ɺ����ò���Ϊ FALSE
			Sleep(10);
		}
}

// **************************************************
// ������static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
// ������
// 	IN hBmp --	BMP�ļ����
//  OUT lpImgSize -- ����ͼ���С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ���������� �õ���ǰ��BMP��ͼ���С.
// ����: 
// **************************************************
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize)
{
	BITMAP bitmap;

		//װ�سɹ�
		if (GetObject(hBmp,sizeof(BITMAP),&bitmap))
		{
			//�õ���Ϣ�ɹ�
			lpImgSize->cx = bitmap.bmWidth;
			lpImgSize->cy = bitmap.bmHeight;
			return TRUE; //  ���سɹ�
		}
		return FALSE; // û�еõ�ͼ����
}
