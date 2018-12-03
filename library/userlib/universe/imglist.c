/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����ͼ����ʽ
�汾�ţ�1.0.0
����ʱ�ڣ�2003-03-18
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/

#include "eImgList.h"
#include "eptrList.h"
#include "egdi.h"

#define IMAGELIST_ICON		0x0001
#define IMAGELIST_BITMAP	0x0002


typedef struct {
		LPPTRLIST lpImage;
		int cx;
		int cy;
		DWORD dwImageStyle;  // be Icon Image (IMAGELIST_ICON)or Bitmap Image(IMAGELIST_BITMAP)
}IMAGELIST,*LPIMAGELIST;

// **************************************************
// ������int ImageList_Add(HIMAGELIST himl,HBITMAP hbmImage,HBITMAP hbmMask)
// ������
// 	IN himl -- ͼ���б���
//	 IN hbmImage -- λͼ���
//	 IN hbmMask -- MASK λͼ���
// 
// ����ֵ�����ص�ǰͼ���б��е�ͼ�����
// ��������������һ��ͼ��ͼ���б�
// ����: 
// **************************************************
int ImageList_Add(
    HIMAGELIST himl,
    HBITMAP hbmImage,
    HBITMAP hbmMask
   )
{
	LPIMAGELIST lpImageList;
	int count;
		
		lpImageList=(LPIMAGELIST)himl; // �õ�ͼ���б���

		if (lpImageList == NULL)
			return -1;

		if (lpImageList->dwImageStyle!=IMAGELIST_BITMAP)
			return -1; // ��ǰֻ�ܴ���λͼ

		PtrListInsert(lpImageList->lpImage,hbmImage); // ��λͼ���뵽ͼ������
		count=lpImageList->lpImage->count; // �õ���ǰͼ���б��е�ͼ�����
		return count; 
}



// **************************************************
// ������HIMAGELIST ImageList_Create(int cx,int cy,UINT flags,int cInitial,int cGrow)
// ������
// 	IN cx -- ͼ����
// 	IN cy -- ͼ��߶�
// 	IN flags -- ͼ������
// 	IN cInitial -- ��ʼ������
// 	IN cGrow -- ��������
// 
// ����ֵ������ͼ���б�ľ��
// ��������������һ��ͼ���б�
// ����: 
// **************************************************
HIMAGELIST ImageList_Create(
    int cx, 	
    int cy, 	
    UINT flags, 	
    int cInitial, 	
    int cGrow	
   )
{
//	HIMAGELIST hImageList;
	LPIMAGELIST lpImageList;


		lpImageList=(LPIMAGELIST)malloc(sizeof(IMAGELIST)); // ����һ��ͼ���б�����ݽṹ
		if (lpImageList==NULL)
			return NULL;
		lpImageList->cx = cx;
		lpImageList->cy = cy;

		lpImageList->lpImage=(LPPTRLIST)malloc(sizeof(PTRLIST)); // ����һ�����ͼ���ָ���б�
		if (lpImageList->lpImage==NULL)
		{
			free(lpImageList);
			return NULL;
		}
		PtrListCreate(lpImageList->lpImage,cInitial,cGrow); // ��ʼ��ָ���б�

		// ����ͼ������
		if (flags&ICL_ICON)
		{
			lpImageList->dwImageStyle=IMAGELIST_ICON;
		}
		else
		{
			lpImageList->dwImageStyle=IMAGELIST_BITMAP;
		}
		return lpImageList; // ����ͼ���б���
}


// **************************************************
// ������BOOL ImageList_Destroy(HIMAGELIST himl)
// ������
// 	IN himl -- ͼ���б���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������ƻ�ͼ���б�
// ����: 
// **************************************************
BOOL ImageList_Destroy(
    HIMAGELIST himl 	
   )
{
	LPIMAGELIST lpImageList;
	int i;
		lpImageList=(LPIMAGELIST)himl; // �õ�ͼ���б����ݽṹ
		if (lpImageList==NULL)
			return FALSE;
		if (lpImageList->lpImage)
		{
			if (lpImageList->dwImageStyle==IMAGELIST_ICON)
			{ // ��ǰ��ͼ����ICON
				for(i=0;i<lpImageList->lpImage->count;i++)
				{
					HICON hIcon;
// !!! Modified By  Jami chen in 2003.08.07
//					DestroyIcon(PtrListAt(lpImageList->lpImage,i));
					hIcon = (HICON)PtrListAt(lpImageList->lpImage,i); // �õ�ICON���
					if (hIcon)
						DestroyIcon(hIcon); // �ƻ�ICON
// !!! Modified End By  Jami chen in 2003.08.07
				}
			}
			else
			{ // ��ǰͼ����BITMAP
				for(i=0;i<lpImageList->lpImage->count;i++)
				{
					HBITMAP hBitmap;
// !!! Modified By  Jami chen in 2003.08.07
//					DeleteObject(PtrListAt(lpImageList->lpImage,i));
					hBitmap = (HBITMAP)PtrListAt(lpImageList->lpImage,i); // �õ�BITMAP���
					if (hBitmap)
						DeleteObject(hBitmap); // �ƻ�BITMAP
// !!! Modified End By jami chen in 2003.08.07
				}
			}
			PtrListDestroy(lpImageList->lpImage); // �ƻ�ָ���б�
			free(lpImageList->lpImage); // �ͷ�ָ���б�
		}
		free(lpImageList); // �ͷ�ͼ��ṹ
		return TRUE;
}

// **************************************************
// ������BOOL ImageList_DrawEx(HIMAGELIST himl,int i,HDC hdcDst,int x,int y,int dx,int dy,COLORREF rgbBk,COLORREF rgbFg,UINT fStyle)
// ������
// 	IN himl -- ͼ���б���
// 	IN i -- ͼ������
// 	IN hdcDst -- Ŀ���豸���
// 	IN x -- X ����
// 	IN y -- Y ����
// 	IN dx -- ���
// 	IN dy -- �߶�
// 	IN rgbBk -- ������ɫ
// 	IN rgbFg -- ǰ����ɫ
// 	IN fStyle -- ���
// 
// ����ֵ��
// ����������
// ����: 
// **************************************************
BOOL ImageList_DrawEx(
    HIMAGELIST himl,
    int i,
    HDC hdcDst,
    int x,
    int y,
    int dx,
    int dy,
    COLORREF rgbBk,
    COLORREF rgbFg,
    UINT fStyle
   )
{ // Ŀǰû���ṩ�˹���
	return TRUE;
}

// **************************************************
// ������HICON ImageList_GetIcon( HIMAGELIST himl,int i,UINT flags )
// ������
// 	IN himl -- ͼ���б���
// 	IN i -- ͼ������
// 	IN flags -- ���
// 
// ����ֵ�����صõ���ICON���
// �����������õ�ָ��������ICON���
// ����: 
// **************************************************
HICON ImageList_GetIcon(
    HIMAGELIST himl,
    int i,  	
    UINT flags 	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl; // �õ�ͼ���б����ݽṹ
		if (lpImageList == NULL)
			return NULL;
		return PtrListAt(lpImageList->lpImage,i); // �õ�ָ��������ͼ��
}

// **************************************************
// ������int ImageList_GetImageCount(HIMAGELIST himl)
// ������
// 	IN himl -- ͼ���б���
// 
// ����ֵ�����ص�ǰͼ��ĸ���
// �����������õ���ǰͼ���б��е�ͼ�������
// ����: 
// **************************************************
int ImageList_GetImageCount(
    HIMAGELIST himl	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl;// �õ�ͼ���б����ݽṹ
		if (lpImageList == NULL)
			return 0;
		return lpImageList->lpImage->count; // ����ͼ�����
}

// **************************************************
// ������BOOL ImageList_Remove( HIMAGELIST himl, int i)
// ������
// 	IN himl -- ͼ���б���
// 	IN i -- ͼ������
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������ɾ��ָ��������ͼ��
// ����: 
// **************************************************
BOOL ImageList_Remove(
    HIMAGELIST himl, 	
    int i	
   )
{
	LPIMAGELIST lpImageList;
		lpImageList=(LPIMAGELIST)himl; // �õ�ͼ���б����ݽṹ
		if (lpImageList == NULL)
			return FALSE;
		
		PtrListAtRemove(lpImageList->lpImage,i); // ɾ��ָ��������ͼ��
		return TRUE;
}

// **************************************************
// ������BOOL ImageList_RemoveAll(HIMAGELIST himl)
// ������
// 	IN himl -- ͼ���б���
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������ɾ��ͼ���б��е�����ͼ��
// ����: 
// **************************************************
BOOL ImageList_RemoveAll(
    HIMAGELIST himl 	
   )
{
	LPIMAGELIST lpImageList;
//	int i;
		lpImageList=(LPIMAGELIST)himl; // �õ�ͼ���б����ݽṹ
		if (lpImageList == NULL)
			return FALSE;
		
		PtrListRemoveAll(lpImageList->lpImage); // ɾ�����е�ͼ��
		return TRUE;
}

// **************************************************
// ������int ImageList_ReplaceIcon(HIMAGELIST himl,int i,HICON hicon)
// ������
// 	IN himl -- ͼ���б���
// 	IN  i -- Ҫ�滻��ICON������
// 	IN hicon -- Ҫ�滻�ɵ�ICON���
// 
// ����ֵ�����ر��滻������ֵ
// �����������滻ָ��������ICON
// ����: 
// **************************************************
int ImageList_ReplaceIcon(
    HIMAGELIST himl, 	
    int i, 	
    HICON hicon	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl; // �õ�ͼ���б����ݽṹ
		if (lpImageList == NULL)
			return FALSE;
		if (i=-1)
		{ // Ҫ����һ��ICON
			PtrListInsert(lpImageList->lpImage,hicon); // ���뵱ǰ��ICON
// Modified by Jami chen in 2002.04.24
// returns The index of the image if success,-1 otherwise.
//			i=lpImageList->lpImage->count;
// Modified to
			i=lpImageList->lpImage->count-1;
// Modified End By  Jami chen in 2002.04.24
		}
		else
		{
			PtrListAtPut( lpImageList->lpImage,i, hicon); // ����ָ����ICONΪ��ǰֵ
		}
		return i;
}


// **************************************************
// ������BOOL ImageList_Destroy(HIMAGELIST himl)
// ������
// 	IN himl -- ͼ���б���
//  OUT cx -- ����ͼ���б���ͼ��Ŀ��
//  OUT cy -- ����ͼ���б���ͼ��ĸ߶�
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// �����������ƻ�ͼ���б�
// ����: 
// **************************************************
BOOL ImageList_GetIconSize(
    HIMAGELIST himl, 	
    int FAR *cx, 	
    int FAR *cy 	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl;
		if (lpImageList == NULL)
			return FALSE;
		*cx = lpImageList->cx;
		*cy = lpImageList->cy;
		return TRUE;
}

