/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：图形列式
版本号：1.0.0
开发时期：2003-03-18
作者：陈建明 Jami chen
修改记录：
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
// 声明：int ImageList_Add(HIMAGELIST himl,HBITMAP hbmImage,HBITMAP hbmMask)
// 参数：
// 	IN himl -- 图象列表句柄
//	 IN hbmImage -- 位图句柄
//	 IN hbmMask -- MASK 位图句柄
// 
// 返回值：返回当前图象列表中的图象个数
// 功能描述：插入一个图象到图象列表。
// 引用: 
// **************************************************
int ImageList_Add(
    HIMAGELIST himl,
    HBITMAP hbmImage,
    HBITMAP hbmMask
   )
{
	LPIMAGELIST lpImageList;
	int count;
		
		lpImageList=(LPIMAGELIST)himl; // 得到图象列表句柄

		if (lpImageList == NULL)
			return -1;

		if (lpImageList->dwImageStyle!=IMAGELIST_BITMAP)
			return -1; // 当前只能处理位图

		PtrListInsert(lpImageList->lpImage,hbmImage); // 将位图插入到图象句柄中
		count=lpImageList->lpImage->count; // 得到当前图象列表中的图象个数
		return count; 
}



// **************************************************
// 声明：HIMAGELIST ImageList_Create(int cx,int cy,UINT flags,int cInitial,int cGrow)
// 参数：
// 	IN cx -- 图象宽度
// 	IN cy -- 图象高度
// 	IN flags -- 图象性质
// 	IN cInitial -- 初始化个数
// 	IN cGrow -- 增长个数
// 
// 返回值：返回图象列表的句柄
// 功能描述：创建一个图象列表。
// 引用: 
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


		lpImageList=(LPIMAGELIST)malloc(sizeof(IMAGELIST)); // 分配一个图象列表的数据结构
		if (lpImageList==NULL)
			return NULL;
		lpImageList->cx = cx;
		lpImageList->cy = cy;

		lpImageList->lpImage=(LPPTRLIST)malloc(sizeof(PTRLIST)); // 分配一个存放图象的指针列表
		if (lpImageList->lpImage==NULL)
		{
			free(lpImageList);
			return NULL;
		}
		PtrListCreate(lpImageList->lpImage,cInitial,cGrow); // 初始化指针列表

		// 设置图象类型
		if (flags&ICL_ICON)
		{
			lpImageList->dwImageStyle=IMAGELIST_ICON;
		}
		else
		{
			lpImageList->dwImageStyle=IMAGELIST_BITMAP;
		}
		return lpImageList; // 返回图象列表句柄
}


// **************************************************
// 声明：BOOL ImageList_Destroy(HIMAGELIST himl)
// 参数：
// 	IN himl -- 图象列表句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：破坏图象列表
// 引用: 
// **************************************************
BOOL ImageList_Destroy(
    HIMAGELIST himl 	
   )
{
	LPIMAGELIST lpImageList;
	int i;
		lpImageList=(LPIMAGELIST)himl; // 得到图象列表数据结构
		if (lpImageList==NULL)
			return FALSE;
		if (lpImageList->lpImage)
		{
			if (lpImageList->dwImageStyle==IMAGELIST_ICON)
			{ // 当前的图象是ICON
				for(i=0;i<lpImageList->lpImage->count;i++)
				{
					HICON hIcon;
// !!! Modified By  Jami chen in 2003.08.07
//					DestroyIcon(PtrListAt(lpImageList->lpImage,i));
					hIcon = (HICON)PtrListAt(lpImageList->lpImage,i); // 得到ICON句柄
					if (hIcon)
						DestroyIcon(hIcon); // 破坏ICON
// !!! Modified End By  Jami chen in 2003.08.07
				}
			}
			else
			{ // 当前图象是BITMAP
				for(i=0;i<lpImageList->lpImage->count;i++)
				{
					HBITMAP hBitmap;
// !!! Modified By  Jami chen in 2003.08.07
//					DeleteObject(PtrListAt(lpImageList->lpImage,i));
					hBitmap = (HBITMAP)PtrListAt(lpImageList->lpImage,i); // 得到BITMAP句柄
					if (hBitmap)
						DeleteObject(hBitmap); // 破坏BITMAP
// !!! Modified End By jami chen in 2003.08.07
				}
			}
			PtrListDestroy(lpImageList->lpImage); // 破坏指针列表
			free(lpImageList->lpImage); // 释放指针列表
		}
		free(lpImageList); // 释放图象结构
		return TRUE;
}

// **************************************************
// 声明：BOOL ImageList_DrawEx(HIMAGELIST himl,int i,HDC hdcDst,int x,int y,int dx,int dy,COLORREF rgbBk,COLORREF rgbFg,UINT fStyle)
// 参数：
// 	IN himl -- 图象列表句柄
// 	IN i -- 图象索引
// 	IN hdcDst -- 目标设备句柄
// 	IN x -- X 坐标
// 	IN y -- Y 坐标
// 	IN dx -- 宽度
// 	IN dy -- 高度
// 	IN rgbBk -- 背景颜色
// 	IN rgbFg -- 前景颜色
// 	IN fStyle -- 风格
// 
// 返回值：
// 功能描述：
// 引用: 
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
{ // 目前没有提供此功能
	return TRUE;
}

// **************************************************
// 声明：HICON ImageList_GetIcon( HIMAGELIST himl,int i,UINT flags )
// 参数：
// 	IN himl -- 图象列表句柄
// 	IN i -- 图象索引
// 	IN flags -- 风格
// 
// 返回值：返回得到的ICON句柄
// 功能描述：得到指定索引的ICON句柄
// 引用: 
// **************************************************
HICON ImageList_GetIcon(
    HIMAGELIST himl,
    int i,  	
    UINT flags 	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl; // 得到图象列表数据结构
		if (lpImageList == NULL)
			return NULL;
		return PtrListAt(lpImageList->lpImage,i); // 得到指定索引的图象
}

// **************************************************
// 声明：int ImageList_GetImageCount(HIMAGELIST himl)
// 参数：
// 	IN himl -- 图象列表句柄
// 
// 返回值：返回当前图象的个数
// 功能描述：得到当前图象列表中的图象个数。
// 引用: 
// **************************************************
int ImageList_GetImageCount(
    HIMAGELIST himl	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl;// 得到图象列表数据结构
		if (lpImageList == NULL)
			return 0;
		return lpImageList->lpImage->count; // 返回图象个数
}

// **************************************************
// 声明：BOOL ImageList_Remove( HIMAGELIST himl, int i)
// 参数：
// 	IN himl -- 图象列表句柄
// 	IN i -- 图象索引
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：删除指定索引的图象。
// 引用: 
// **************************************************
BOOL ImageList_Remove(
    HIMAGELIST himl, 	
    int i	
   )
{
	LPIMAGELIST lpImageList;
		lpImageList=(LPIMAGELIST)himl; // 得到图象列表数据结构
		if (lpImageList == NULL)
			return FALSE;
		
		PtrListAtRemove(lpImageList->lpImage,i); // 删除指定索引的图象
		return TRUE;
}

// **************************************************
// 声明：BOOL ImageList_RemoveAll(HIMAGELIST himl)
// 参数：
// 	IN himl -- 图象列表句柄
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：删除图象列表中的所有图象
// 引用: 
// **************************************************
BOOL ImageList_RemoveAll(
    HIMAGELIST himl 	
   )
{
	LPIMAGELIST lpImageList;
//	int i;
		lpImageList=(LPIMAGELIST)himl; // 得到图象列表数据结构
		if (lpImageList == NULL)
			return FALSE;
		
		PtrListRemoveAll(lpImageList->lpImage); // 删除所有的图象
		return TRUE;
}

// **************************************************
// 声明：int ImageList_ReplaceIcon(HIMAGELIST himl,int i,HICON hicon)
// 参数：
// 	IN himl -- 图象列表句柄
// 	IN  i -- 要替换的ICON的索引
// 	IN hicon -- 要替换成的ICON句柄
// 
// 返回值：返回被替换的索引值
// 功能描述：替换指定索引的ICON
// 引用: 
// **************************************************
int ImageList_ReplaceIcon(
    HIMAGELIST himl, 	
    int i, 	
    HICON hicon	
   )
{
	LPIMAGELIST lpImageList;

		lpImageList=(LPIMAGELIST)himl; // 得到图象列表数据结构
		if (lpImageList == NULL)
			return FALSE;
		if (i=-1)
		{ // 要插入一个ICON
			PtrListInsert(lpImageList->lpImage,hicon); // 插入当前的ICON
// Modified by Jami chen in 2002.04.24
// returns The index of the image if success,-1 otherwise.
//			i=lpImageList->lpImage->count;
// Modified to
			i=lpImageList->lpImage->count-1;
// Modified End By  Jami chen in 2002.04.24
		}
		else
		{
			PtrListAtPut( lpImageList->lpImage,i, hicon); // 设置指定的ICON为当前值
		}
		return i;
}


// **************************************************
// 声明：BOOL ImageList_Destroy(HIMAGELIST himl)
// 参数：
// 	IN himl -- 图象列表句柄
//  OUT cx -- 返回图象列表中图象的宽度
//  OUT cy -- 返回图象列表中图象的高度
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：破坏图象列表
// 引用: 
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

