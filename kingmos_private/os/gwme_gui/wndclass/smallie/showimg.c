/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����С��Ļģʽ��IE�����, ͼ����ʾ
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-05-26
���ߣ��½��� JAMI
�޸ļ�¼��
**************************************************/
#ifdef EML_DOS
#include <dos.h>
#include <CONIO.H>
#include <STDIO.H>
#endif

//#include "eframe.h"
//#include "ewindows.h"
#include "eHtmView.h"
#include "Control.h"
//#include "HtmlLocate.h"
#include "SIETable.h"
#include "efilepth.h"
#include "ViewDef.h"
#include "jpegapi.h"


#define TYPE_NOTIMGFILE	0
#define TYPE_BMPFILE	1
#define TYPE_JPEGFILE	2
#define TYPE_GIFFILE	3

typedef struct structFILETYPE{
	LPTSTR lpFileExt;
	int iFileType;
}FILETYPE;

static FILETYPE tabImageType[] = {
	{ "*.gif" , TYPE_GIFFILE },
	{ "*.bmp" , TYPE_BMPFILE },
	{ "*.jpg" , TYPE_JPEGFILE },
	{ "*.jpeg", TYPE_JPEGFILE },
};



extern LPTSTR GetControlImg(HHTMCTL lpControl);
extern BOOL GetControlPos(HHTMCTL hControl,LPPOINT ppoint);
extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );;
extern BOOL SetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize);
extern BOOL GetControlSize(HWND hWnd,HHTMCTL hControl,LPSIZE lpImageSize);


static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg);
static int GetNextPicTime(HGIF hGif,int iIndex);
static UINT GetImageType(LPTSTR lpFileName);
static int GetTotalPic(HGIF hGif);
static void LoadImageFile(HWND hWnd,LPSHOWIMAGEITEM lpCurItem);
static void DrawBitMap(HDC hdc,HBITMAP hBitmap,LPRECT lprect);
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize);
static BOOL GetBMPImageSize(HBITMAP hBmp, LPSIZE lpImgSize);


//#ifdef USE_TIMERTOSHOWIMG

// **************************************************
// ������DWORD WINAPI ShowImageThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// ����������ͼ����ʾ�̡߳�
// ����: 
// **************************************************
DWORD WINAPI ShowImage(HWND hWnd)
{
	LPHTMLVIEW lpHtmlView;
	LPSHOWIMAGEITEM lpCurItem;
	POINT ptOrg,ptControl;
//	HDC hdc;
	RECT rect;
	SIZE sizeImage;
	BOOL bRefreshScreen = FALSE;
//	static int tick1 = 0,tick2 = 0;

//		tick2 = GetTickCount();
//		RETAILMSG(1,(TEXT("Delay Time = %d"),tick2 - tick1));
//		tick1 = tick2;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

//		hdc = GetDC(hWnd);

		lpCurItem = lpHtmlView->lpShowImageList;  // �õ���һ��ͼ��
		
		if (lpHtmlView->iDisplayMode != DISPLAY_ONLYTEXT || lpHtmlView->iFileStyle == IMAGE_FILE)
		{
			while(lpCurItem)
			{
				if (lpCurItem->bDownloadOK == TRUE)
				{  // ��ǰ�ļ��Ѿ�����
					// ��ǰͼ����Ч
					if (lpCurItem->bShow)
					{
						// Ҫ��ʾ��ͼ��
						if (lpCurItem->hGif == NULL && lpCurItem->hBmp == NULL)
						{
							// ��û�д򿪵�ǰ�ļ����ȴ򿪵�ǰ�ļ�
							LoadImageFile(hWnd,lpCurItem);
							lpCurItem->bShow = FALSE; // ��ʾ���
						}
/*						if (lpCurItem->hGif)
						{
							// ��gif �ļ�
							ptOrg.x = lpHtmlControl->x - lpHtmlView->x_Org;
							ptOrg.y = lpHtmlControl->y - lpHtmlView->y_Org;
							DrawGifImage(hWnd,hdc,lpCurItem->hGif,lpCurItem->iIndex,ptOrg);
						}
*/
						if (lpCurItem->hBmp)
						{
							// �� bmp �ļ� ���� jpeg �ļ�
							GetControlPos(lpCurItem->hControl,&ptControl);
							GetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
							rect.left = ptControl.x - lpHtmlView->x_Org;
							rect.top = ptControl.y - lpHtmlView->y_Org;
							rect.right = rect.left + sizeImage.cx;
							rect.bottom = rect.top + sizeImage.cy;
							DrawBitMap(lpHtmlView->hMemoryDC,lpCurItem->hBmp,&rect); // ��ʾλͼ�ļ�
							lpCurItem->bShow = FALSE; // ��ʾ���
							bRefreshScreen = TRUE; // ��Ҫˢ����Ļ
						}
					}

					// �鿴�Ƿ���GIF�ļ������Ƿ���Ҫ��ʾ��һ֡ͼ��
					if (lpCurItem->hGif)
					{
						// ��gif �ļ�
						if (lpCurItem->iImageNum == 1)
						{
							// ֻ��һ��ͼ
							if (lpCurItem->bShow) 
							{
								// ֻ������Ҫ��ʾʱ����ʾ
								GetControlPos(lpCurItem->hControl,&ptControl);
								ptOrg.x = ptControl.x - lpHtmlView->x_Org;
								ptOrg.y = ptControl.y - lpHtmlView->y_Org;
								DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,0,&ptOrg);
								bRefreshScreen = TRUE; // ��Ҫˢ����Ļ
							}
						}
						else
						{
							if (lpCurItem->bShow)
							{
								// Ҫ��������ʾ
								lpCurItem->iDelayTime = 0;
							}
							if (lpCurItem->iDelayTime <= 0)
							{
								//��Ҫ��ʾ
								GetControlPos(lpCurItem->hControl,&ptControl);
//								tick1 = GetTickCount();
								ptOrg.x = ptControl.x - lpHtmlView->x_Org;
								ptOrg.y = ptControl.y - lpHtmlView->y_Org;
								DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,lpCurItem->iIndex,&ptOrg);
								lpCurItem->iDelayTime = GetNextPicTime(lpCurItem->hGif,lpCurItem->iIndex); // �õ�����һ��ͼ���ʱ��
								lpCurItem->iIndex ++; // ��һ��ͼ��
								if (lpCurItem->iIndex >= lpCurItem->iImageNum)
									lpCurItem->iIndex = 0; // �Ѿ������һ��ͼ��, �´ο�ʼ��ʾ��һ��ͼ��
								bRefreshScreen = TRUE; // ��Ҫˢ����Ļ
							}
							else
							{
								lpCurItem->iDelayTime -= TIMEINTERVAL; // ��ȥ�Ѿ��ȴ���ʱ��
							}
						}
						lpCurItem->bShow = FALSE; // ��ʾ���
					}
				}

				lpCurItem = lpCurItem->next; // �õ���һ��ͼ���ļ�
			}
		}
		if (bRefreshScreen == TRUE)
			InvalidateRect(hWnd,NULL,TRUE);
//		ReleaseDC(hWnd,hdc);
		return 0;
}
//#else
// **************************************************
// ������DWORD WINAPI ShowImageThread(VOID * pParam)
// ������
// 	IN pParam -- �̲߳���
// 
// ����ֵ����
// ����������ͼ����ʾ�̡߳�
// ����: 
// **************************************************
DWORD WINAPI ShowImageThread(VOID * pParam)
{
	HWND hWnd;
	LPHTMLVIEW lpHtmlView;
	LPSHOWIMAGEITEM lpCurItem;
	POINT ptOrg,ptControl;
//	HDC hdc;
	RECT rect;
	SIZE sizeImage;
	BOOL bRefreshScreen = FALSE;
//	int tick1 = 0,tick2 = 0;

	
		hWnd = (HWND)pParam;  // �õ�����
		if (hWnd == NULL)
			return FALSE;
		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  //�õ�HTMLVIEW�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

	//	hdc = GetDC(hWnd);
		while(1)
		{
			if (lpHtmlView->bExit)
				break;
			bRefreshScreen = FALSE; // ��Ҫˢ����Ļ
			lpCurItem = lpHtmlView->lpShowImageList;  // �õ���һ��ͼ��
			
//			if (lpHtmlView->iDisplayMode != DISPLAY_ONLYTEXT)
		    if (lpHtmlView->iDisplayMode != DISPLAY_ONLYTEXT || lpHtmlView->iFileStyle == IMAGE_FILE)
			{
				while(lpCurItem)
				{
					if (lpCurItem->bDownloadOK == TRUE)
					{  // ��ǰ�ļ��Ѿ�����
						// ��ǰͼ����Ч
						if (lpCurItem->bShow)
						{
							// Ҫ��ʾ��ͼ��
							if (lpCurItem->hGif == NULL && lpCurItem->hBmp == NULL)
							{
								// ��û�д򿪵�ǰ�ļ����ȴ򿪵�ǰ�ļ�
								LoadImageFile(hWnd,lpCurItem);
								lpCurItem->bShow = FALSE; // ��ʾ���
							}
	/*						if (lpCurItem->hGif)
							{
								// ��gif �ļ�
								ptOrg.x = lpHtmlControl->x - lpHtmlView->x_Org;
								ptOrg.y = lpHtmlControl->y - lpHtmlView->y_Org;
								DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,lpCurItem->iIndex,ptOrg);
							}
	*/
							if (lpCurItem->hBmp)
							{
								// �� bmp �ļ� ���� jpeg �ļ�
								GetControlPos(lpCurItem->hControl,&ptControl);
								GetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
								rect.left = ptControl.x - lpHtmlView->x_Org;
								rect.top = ptControl.y - lpHtmlView->y_Org;
								rect.right = rect.left + sizeImage.cx;
								rect.bottom = rect.top + sizeImage.cy;
								DrawBitMap(lpHtmlView->hMemoryDC,lpCurItem->hBmp,&rect); // ��ʾλͼ�ļ�
								lpCurItem->bShow = FALSE; // ��ʾ���
								bRefreshScreen = TRUE; // ��Ҫˢ����Ļ
							}
						}

						// �鿴�Ƿ���GIF�ļ������Ƿ���Ҫ��ʾ��һ֡ͼ��
						if (lpCurItem->hGif)
						{
							// ��gif �ļ�
							if (lpCurItem->iImageNum == 1)
							{
								// ֻ��һ��ͼ
								if (lpCurItem->bShow) 
								{
									// ֻ������Ҫ��ʾʱ����ʾ
									GetControlPos(lpCurItem->hControl,&ptControl);
									ptOrg.x = ptControl.x - lpHtmlView->x_Org;
									ptOrg.y = ptControl.y - lpHtmlView->y_Org;
									DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,0,&ptOrg);
									bRefreshScreen = TRUE; // ��Ҫˢ����Ļ
								}
							}
							else
							{
								if (lpCurItem->bShow)
								{
									// Ҫ��������ʾ
									lpCurItem->iDelayTime = 0;
								}
								if (lpCurItem->iDelayTime <= 0)
								{
									//��Ҫ��ʾ
									GetControlPos(lpCurItem->hControl,&ptControl);
									ptOrg.x = ptControl.x - lpHtmlView->x_Org;
									ptOrg.y = ptControl.y - lpHtmlView->y_Org;
									DrawGifImage(hWnd,lpHtmlView->hMemoryDC,lpCurItem->hGif,lpCurItem->iIndex,&ptOrg);
									lpCurItem->iDelayTime = GetNextPicTime(lpCurItem->hGif,lpCurItem->iIndex); // �õ�����һ��ͼ���ʱ��
									lpCurItem->iIndex ++; // ��һ��ͼ��
									if (lpCurItem->iIndex >= lpCurItem->iImageNum)
										lpCurItem->iIndex = 0; // �Ѿ������һ��ͼ��, �´ο�ʼ��ʾ��һ��ͼ��

									bRefreshScreen = TRUE; // ��Ҫˢ����Ļ
								}
								else
								{
									lpCurItem->iDelayTime -= TIMEINTERVAL; // ��ȥ�Ѿ��ȴ���ʱ��
								}
							}
							lpCurItem->bShow = FALSE; // ��ʾ���
						}
					}

					lpCurItem = lpCurItem->next; // �õ���һ��ͼ���ļ�
				}
			}
			if (bRefreshScreen == TRUE)
			{	HDC hdc;
//				InvalidateRect(hWnd,NULL,TRUE);
				hdc = GetDC(hWnd);
				GetClientRect(hWnd,&rect);
				BitBlt(hdc,0,0,rect.right,rect.bottom,lpHtmlView->hMemoryDC,0,0,SRCCOPY);
				ReleaseDC(hWnd,hdc);
			}
//			tick1 = GetTickCount();
			Sleep(TIMEINTERVAL);
//			tick2 = GetTickCount();
//			RETAILMSG(1,(TEXT("Delay Time = %d"),tick2 - tick1));
//			tick1 = tick2;
		}

//		ReleaseDC(hWnd,hdc);

		return 0;
}

//#endif

// **************************************************
// ������BOOL InsertAnimationList(HWND hWnd,HHTMCTL hControl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpHtmlControl -- Ҫ����ͼ��Ŀؼ����
// 
// ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
// ��������������һ��ͼ��ؼ�����ʾͼ���б�
// ����: 
// **************************************************
BOOL InsertAnimationList(HWND hWnd,HHTMCTL hControl)
{
	LPSHOWIMAGEITEM lpNewImageItem;
	LPSHOWIMAGEITEM lpCurItem;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;

		lpNewImageItem = (LPSHOWIMAGEITEM)malloc(sizeof(SHOWIMAGEITEM)); // ����һ����Ŀ�ռ�
		if (lpNewImageItem == NULL)
		{ // �����ڴ�ʧ��
			return FALSE;
		}

		// ��ʼ���ṹ
		lpNewImageItem->hControl = hControl;  // ���ÿؼ����
		lpNewImageItem->lpLocatePath = NULL;  // Ŀǰ��û�ж�Ӧ�ı����ļ���
		lpNewImageItem->hGif = NULL; // Gif ͼ��ľ��
		lpNewImageItem->hBmp  = NULL; // bitmap , jpeg ͼ��ľ��
		lpNewImageItem->iIndex = 0; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
		lpNewImageItem->iDelayTime = 0; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
		lpNewImageItem->iImageNum = 0 ; //gif ͼ��ʹ�ã���ǰһ���ж��ٷ�ͼ��
		lpNewImageItem->next = NULL;  // ָ����һ��GIF����
		lpNewImageItem->bShow = FALSE; // ���ڻ�����Ҫ��ʾ
		lpNewImageItem->bDownloadOK = FALSE;  // ��û�����ظ��ļ�

		// ���뵱ǰͼ����ʾͼ���б�

		if (lpHtmlView->lpShowImageList == NULL)
		{  //��ǰ�б��л�û������
			lpHtmlView->lpShowImageList = lpNewImageItem; // ֱ�Ӳ��뵽�б��
		}
		else
		{
			// �Ѿ���ͼ����ڣ����뵽���
			lpCurItem = lpHtmlView->lpShowImageList;

			// �����һ����ĿΪ�գ����¼���Ŀ��ӵ���ǰ��Ŀ����һ����Ŀ
			while(lpCurItem->next)
			{ //��ǰ��Ŀ����һ����Ŀ������
				lpCurItem = lpCurItem->next;  // ָ����һ����Ŀ
			}
			lpCurItem->next = lpNewImageItem; // ����������Ŀ
		}

		return TRUE; // ���سɹ�
}


// **************************************************
// ������BOOL DownloadImageOK(HWND hWnd, HHTMCTL hControl,LPTSTR lpFileName)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hControl -- �Ѿ�������ص�ͼ��Ŀؼ����
//	IN lpFileName -- ���ص������ļ����ļ���
// 
// ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
// �������������һ��ͼ��ؼ������ء�
// ����: 
// **************************************************
BOOL DownloadImageOK(HWND hWnd, HHTMCTL hControl,LPTSTR lpFileName)
{
	LPSHOWIMAGEITEM lpCurItem;
	LPHTMLVIEW lpHtmlView;
	SIZE sizeImage;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		// �Ѿ���ͼ����ڣ����뵽���
		lpCurItem = lpHtmlView->lpShowImageList;

		// �����һ����ĿΪ�գ����¼���Ŀ��ӵ���ǰ��Ŀ����һ����Ŀ
		while(lpCurItem)
		{ //��ǰ��Ŀ��Ч
			if (lpCurItem->hControl == hControl)
			{
				// ������صĿؼ����ǵ�ǰ�ؼ�
				lpCurItem->bDownloadOK = TRUE; // ��ǰ�ؼ��Ѿ��������
				if (lpFileName)
				{
					//�б����ļ�����
					lpCurItem->lpLocatePath = BufferAssignTChar(lpFileName);  // ������ǰ�ļ���
				}
				// װ��ͼ��
				if (lpCurItem->hGif == NULL && lpCurItem->hBmp == NULL)
				{
					// ��û�д򿪵�ǰ�ļ����ȴ򿪵�ǰ�ļ�
					LoadImageFile(hWnd,lpCurItem);
				}
				if (lpCurItem->hGif)
				{
					// ��GIF�ļ�
					if (GetGIFImageSize(lpCurItem->hGif,&sizeImage) == TRUE)
					{
						// �õ��ߴ�ɹ�
						SetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
					}
				}
				if (lpCurItem->hBmp)
				{
					// ��BMP�ļ�
					if (GetBMPImageSize(lpCurItem->hBmp,&sizeImage) == TRUE)
					{
						// �õ��ߴ�ɹ�
						SetControlSize(hWnd,lpCurItem->hControl,&sizeImage);
					}
				}
				break;
			}
			lpCurItem = lpCurItem->next;
		}
		return TRUE;
}

// **************************************************
// ������BOOL ShowImageControl(HWND hWnd,HHTMCTL hControl)
// ������
// 	IN hWnd -- ���ھ��
// 	IN lpHtmlControl -- Ҫ����ͼ��Ŀؼ����
// 
// ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
// ��������������һ��ͼ��ؼ�����ʾͼ���б�
// ����: 
// **************************************************
BOOL ShowImageControl(HWND hWnd,HHTMCTL hControl)
{
	LPSHOWIMAGEITEM lpCurItem;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return FALSE;

		// �Ѿ���ͼ����ڣ����뵽���
		lpCurItem = lpHtmlView->lpShowImageList;

		// �����һ����ĿΪ�գ����¼���Ŀ��ӵ���ǰ��Ŀ����һ����Ŀ
		while(lpCurItem)
		{ //��ǰ��Ŀ��Ч
			if (lpCurItem->hControl == hControl)
			{
				// ��Ҫ��ʾ�Ŀؼ����ǵ�ǰ�ؼ�
				lpCurItem->bShow = TRUE; // ��ǰ�ؼ�Ҫ����ʾ
				break;
			}
			lpCurItem = lpCurItem->next;
		}
		return TRUE;
}


// **************************************************
// ������static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
// ������
// 	IN hWnd -- ���ھ��
// 	IN hdc -- �豸���
// 	IN hGif -- Ҫ��ʾ��ͼ����
//  IN iIndex -- Ҫ��ʾ��ͼ������
//  IN ptOrg -- ͼ��ʼ��ʾλ��
// 
// ����ֵ����
// ��������������һ��ͼ�ξ����ʾͼ��
// ����: 
// **************************************************
static void DrawGifImage(HWND hWnd,HDC hdc,HGIF hGif,UINT iIndex,LPPOINT ptOrg)
{
//	GIF_IMAGE gifimage;

//		gifimage.dwSize = sizeof(GIF_IMAGE);
//		Gif_GetImage(hGif,iIndex,&gifimage); // �õ���ǰ����ͼ�����Ϣ
		// ���Ƶ�ǰָ��������ͼ��
		Gif_DrawIndex(hGif,iIndex,hdc,ptOrg->x,ptOrg->y);
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
// ������static void LoadImageFile(HWND hWnd,LPSHOWIMAGEITEM lpCurItem)
// ������
//  IN  hWnd -- ���ھ��
//	IN	lpCurItem -- ��ǰҪ���ص�ͼ��ؼ�
// 
// ����ֵ����
// ���������� װ�ص�ǰͼ��.
// ����: 
// **************************************************
static void LoadImageFile(HWND hWnd,LPSHOWIMAGEITEM lpCurItem)
{
	LPTSTR lpImageFile;
	UINT uType;

		if (lpCurItem->lpLocatePath)
		{
			lpImageFile= lpCurItem->lpLocatePath;
		}
		else
		{
			lpImageFile= GetControlImg(lpCurItem->hControl);
		}

		uType = GetImageType(lpImageFile);  // �õ�ͼ������

		if (uType == TYPE_GIFFILE)
		{
			// ��ǰ��GIF�ļ�
			lpCurItem->hGif = Gif_LoadFromFile( lpImageFile ) ; // װ��GIFͼ��
			if (lpCurItem->hGif != NULL)
			{
				// �ɹ�װ��
				lpCurItem->iIndex = 0; // gifͼ��ʹ�ã���ǰ���ڲ��ŵ�ͼ������
				lpCurItem->iDelayTime = 0; // gif ͼ��ʹ�ã�������һ������ͼ����Ҫ�ȴ���ʱ��
				lpCurItem->iImageNum = GetTotalPic(lpCurItem->hGif); // �õ���ǰGIF��ͼ������
			}
		}
		if (uType == TYPE_BMPFILE)
		{
			lpCurItem->hBmp = LoadImage(0,lpImageFile,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);  // װ��BMPͼ��
		}
		if (uType == TYPE_JPEGFILE)
		{
			lpCurItem->hBmp = JPEG_LoadByName(lpImageFile,NULL);  // װ��JPEGͼ��
		}
}



// **************************************************
// ������static UINT GetImageType(LPTSTR lpFileName)
// ������
//	IN	lpFileName -- ָ�����ļ���
// 
// ����ֵ������ָ���ļ����ļ�����
// ���������� �õ���ǰͼ���ļ����ļ�����.
// ����: 
// **************************************************
static UINT GetImageType(LPTSTR lpFileName)
{
	int iImageTypeNum , i;

		if (lpFileName == NULL)
			return TYPE_NOTIMGFILE; // ������֪��ͼ���ļ�

		iImageTypeNum = sizeof (tabImageType) / sizeof (FILETYPE);
		for (i = 0 ; i < iImageTypeNum; i++ )
		{
			if (FileNameCompare( tabImageType[i].lpFileExt, strlen( tabImageType[i].lpFileExt ), lpFileName, strlen(lpFileName)) == TRUE)
			{ // �Ƚϵ�ǰ�ļ����ļ�����
				return  tabImageType[i].iFileType;  // ���ص�ǰ���ļ�����
			}
		}
		return TYPE_NOTIMGFILE; //������֪��ͼ���ļ�
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
// ������BOOL ReleaseAnimationList(HWND hWnd)
// ������
// 	IN hWnd -- ���ھ��
// 
// ����ֵ���ɹ�����TRUE��ʧ�ܷ���FALSE��
// �����������ͷ���ʾͼ���б�
// ����: 
// **************************************************
BOOL ReleaseAnimationList(HWND hWnd)
{
	LPSHOWIMAGEITEM lpCurItem,lpNextItem;
	LPHTMLVIEW lpHtmlView;

		lpHtmlView=(LPHTMLVIEW)GetWindowLong(hWnd,0);  // �õ�HTML�ṹ
		if (lpHtmlView==NULL)
			return 0;



		lpCurItem = lpHtmlView->lpShowImageList; //�õ���һ��ͼ����Ŀ

		// �����һ����ĿΪ�գ����¼���Ŀ��ӵ���ǰ��Ŀ����һ����Ŀ
		while(lpCurItem)
		{ //��ǰ��Ŀ����һ����Ŀ������
			lpNextItem = lpCurItem->next;  // �õ���һ����Ŀ 
			// �ͷŵ�ǰ��Ŀ
			if (lpCurItem->lpLocatePath)
			{
				// �Ѿ�������һ�������ļ������棬�ͷ�
				free(lpCurItem->lpLocatePath);
			}
			if (lpCurItem->hGif)
			{
				// �Ѿ�����һ��GIF�ļ�
				Gif_Destroy( lpCurItem->hGif ) ; // �ر�GIF�ļ�
			}

			if (lpCurItem->hBmp)
			{ // �Ѿ�����BMP ���� JPEG�ļ�
				DeleteObject(lpCurItem->hBmp);  // ɾ��λͼ���
			}

			free(lpCurItem); // �ͷ���Ŀ�ṹ
			lpCurItem = lpNextItem;  // ָ����һ����Ŀ
		}

		lpHtmlView->lpShowImageList = NULL; //�����Ŀ
		return TRUE; // ���سɹ�
}


// **************************************************
// ������static void DrawBitMap(HDC hdc,HBITMAP hBitmap,LPRECT lprect)
// ������
// 	IN hdc -- �豸���
// 	IN hBitmap -- λͼ���
// 	IN rect -- λͼ��С
// 	IN xOffset -- Xƫ��
// 	IN yOffset -- Yƫ��
// 	IN dwRop -- ����ģʽ
// 
// ����ֵ����
// ��������������λͼ��
// ����: 
// **************************************************
static void DrawBitMap(HDC hdc,HBITMAP hBitmap,LPRECT lprect)
{
		HDC hMemoryDC;
    HBITMAP hOldBitmap;

		hMemoryDC=CreateCompatibleDC(hdc); // ����һ�����ݵ��ڴ���
		hOldBitmap=SelectObject(hMemoryDC,hBitmap); // ��Ҫ���Ƶ�λͼ���õ��ڴ�����
		BitBlt( hdc, // handle to destination device context
			(short)lprect->left,  // x-coordinate of destination rectangle's upper-left
									 // corner
			(short)lprect->top,  // y-coordinate of destination rectangle's upper-left
									 // corner
			(short)(lprect->right-lprect->left),  // width of destination rectangle
			(short)(lprect->bottom-lprect->top), // height of destination rectangle
			hMemoryDC,  // handle to source device context
			0,   // x-coordinate of source rectangle's upper-left
									 // corner
			0,   // y-coordinate of source rectangle's upper-left
									 // corner

			SRCCOPY
			);

		SelectObject(hMemoryDC,hOldBitmap); // �ָ��ڴ���
		DeleteDC(hMemoryDC); // ɾ�����
}

// **************************************************
// ������static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
// ������
// 	IN hGif --	GIF�ļ����
//  OUT lpImgSize -- ����ͼ���С
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ���������� �õ���ǰ��GIF��ͼ���С.
// ����: 
// **************************************************
static BOOL GetGIFImageSize(HGIF hGif, LPSIZE lpImgSize)
{
	GIF_INFO gifInfo;

		gifInfo.dwSize = sizeof(GIF_INFO); // ���ýṹ��С
		//�õ�GIF�������Ϣ
		if (Gif_GetInfo( hGif, &gifInfo ) == TRUE)
		{
			// �õ���Ϣ�ɹ�
			lpImgSize->cx = gifInfo.nWidth;
			lpImgSize->cy = gifInfo.nHeight;
			return TRUE; // ���ص�ǰGIF��ͼ����
		}
		return FALSE; // û�еõ�ͼ����
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

// !!! Add By Jami chen in 2003.09.09
// **************************************************
// ������BOOL IsImageFile(LPTSTR lpFileName)
// ������
// 	IN lpFileName -- ָ���ļ�
// 
// ����ֵ����ͼ���ļ����򷵻�TRUE�����򷵻�FALSE
// �����������ж�ָ���ļ��Ƿ���ͼ���ļ���
// ����: 
// **************************************************
BOOL IsImageFile(LPTSTR lpFileName)
{
	UINT  uImgType;

		uImgType = GetImageType(lpFileName) ; // 

		if (uImgType == TYPE_NOTIMGFILE)
		{
			// ����ͼ���ļ�
			return FALSE;
		}

		return TRUE; // ��ͼ���ļ�
}
// !!! Add By Jami chen in 2003.09.09
