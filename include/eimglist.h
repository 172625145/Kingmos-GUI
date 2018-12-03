/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef _EIMGLIST_H
#define _EIMGLIST_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分

#include <edef.h>
#include <ealloc.h>
#include <eassert.h>
#include <erect.h>

typedef LPVOID HIMAGELIST;
typedef LPBYTE LPSTREAM;


typedef struct _IMAGELISTDRAWPARAMS {
    DWORD       cbSize;
    HIMAGELIST  himl;
    int         i;
    HDC         hdcDst;
    int         x;
    int         y;
    int         cx;
    int         cy;
    int         xBitmap;        // x offest from the upperleft of bitmap
    int         yBitmap;        // y offset from the upperleft of bitmap
    COLORREF    rgbBk;
    COLORREF    rgbFg;
    UINT        fStyle;
    DWORD       dwRop;
} IMAGELISTDRAWPARAMS, FAR * LPIMAGELISTDRAWPARAMS;

typedef struct _IMAGEINFO
{
    HBITMAP hbmImage;
    HBITMAP hbmMask;
    int     Unused1;
    int     Unused2;
    RECT    rcImage;
} IMAGEINFO, FAR *LPIMAGEINFO;


#define ILC_COLOR		0x00000001
#define ILC_COLOR4		0x00000002
#define ILC_COLOR8		0x00000004
#define ILC_COLOR16		0x00000008
#define ILC_COLOR24		0x00000010
#define ILC_COLOR32		0x00000020
#define ILC_COLORDDB	0x00000040
#define ILC_MASK		0x00000080
#define ICL_ICON		0x00000100
#define ILC_ICON		ICL_ICON

int ImageList_Add(
    HIMAGELIST himl,
    HBITMAP hbmImage,
    HBITMAP hbmMask
   );	


int ImageList_AddMasked(
    HIMAGELIST himl,
    HBITMAP hbmImage,
    COLORREF crMask
   );	


BOOL ImageList_BeginDrag(
    HIMAGELIST himlTrack, 	
    int iTrack, 	
    int dxHotspot, 	
    int dyHotspot 	
   );	


BOOL ImageList_Copy(
    HIMAGELIST himlDst,
    int        iDst,
    HIMAGELIST himlSrc,
    int        iSrc,
    UINT       uFlags);



HIMAGELIST ImageList_Create(
    int cx, 	
    int cy, 	
    UINT flags, 	
    int cInitial, 	
    int cGrow	
   );	


BOOL ImageList_Destroy(
    HIMAGELIST himl 	
   );	


BOOL ImageList_DragEnter(
    HWND hwndLock, 	
    int x, 	
    int y	
   );	


BOOL ImageList_DragLeave(
    HWND hwndLock 	
   );	


BOOL ImageList_DragMove(
    int x, 	
    int y 	
   );	


BOOL ImageList_DragShowNolock(
    BOOL fShow	
   );	


BOOL ImageList_Draw(
    HIMAGELIST himl,
    int i,
    HDC hdcDst,
    int x,
    int y,
    UINT fStyle
   );	


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
   );	


BOOL ImageList_DrawIndirect(
    IMAGELISTDRAWPARAMS* pimldp);


HIMAGELIST ImageList_Duplicate(
    HIMAGELIST himl);


BOOL ImageList_EndDrag(VOID);


COLORREF ImageList_GetBkColor(
    HIMAGELIST himl 	
   );	


HIMAGELIST ImageList_GetDragImage(
    POINT FAR *ppt, 	
    POINT FAR *pptHotspot 	
   );	


HICON ImageList_GetIcon(
    HIMAGELIST himl,
    int i,  	
    UINT flags 	
   );	


BOOL ImageList_GetIconSize(
    HIMAGELIST himl, 	
    int FAR *cx, 	
    int FAR *cy 	
   );	


int ImageList_GetImageCount(
    HIMAGELIST himl	
   );	


BOOL ImageList_GetImageInfo(
    HIMAGELIST himl, 	
    int i, 	
    IMAGEINFO FAR *pImageInfo	
   );	


HIMAGELIST ImageList_LoadImage(
    HINSTANCE hi,
    LPCTSTR lpbmp,
    int cx,
    int cGrow,
    COLORREF crMask,
    UINT uType,
    UINT uFlags
   );	


HIMAGELIST ImageList_Merge(
    HIMAGELIST himl1, 	
    int i1, 	
    HIMAGELIST himl2, 	
    int i2, 	
    int dx, 	
    int dy	
   );	


HIMAGELIST ImageList_Read(
    LPSTREAM pstm	
   );	


BOOL ImageList_Remove(
    HIMAGELIST himl, 	
    int i	
   );	


BOOL ImageList_Replace(
    HIMAGELIST himl, 	
    int i, 	
    HBITMAP hbmImage, 	
    HBITMAP hbmMask	
   );	

int ImageList_ReplaceIcon(
    HIMAGELIST himl, 	
    int i, 	
    HICON hicon	
   );	

COLORREF ImageList_SetBkColor(
    HIMAGELIST himl, 	
    COLORREF clrBk	
   );	


BOOL ImageList_SetDragCursorImage(
    HIMAGELIST himlDrag, 	
    int iDrag, 	
    int dxHotspot, 	
    int dyHotspot	
   );	


BOOL ImageList_SetIconSize(
    HIMAGELIST himl, 	
    int cx, 	
    int cy	
   );	


BOOL ImageList_SetImageCount(
    HIMAGELIST himl,
    UINT       uNewCount);


BOOL ImageList_SetOverlayImage(
    HIMAGELIST himl, 	
    int iImage, 	
    int iOverlay	
   );	


BOOL ImageList_Write(
    HIMAGELIST himl,
    LPSTREAM pstm
   );	


#define  ImageList_AddIcon(himl, hicon) ImageList_ReplaceIcon(himl, -1, hicon)
#define INDEXTOOVERLAYMASK(i) ((i) << 8)
 


#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //_EIMGLIST_H
