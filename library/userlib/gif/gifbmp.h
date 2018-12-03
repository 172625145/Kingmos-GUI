/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __GIFBMP_H
#define __GIFBMP_H

#ifndef __GIF2BMP_H
#include <gif2bmp.h>
#endif

#define IMAGE_CONTROL_BLOCK			0xF9
#define	TYPE_BITMAP					1
#define	TYPE_CONTROL				2


typedef struct _GCONTROL
{
	//DWORD		dwType;
	UINT		uDelayTime;
	DWORD       clrTransColor;
	UCHAR		cPackedField;
	//UCHAR		cTransColorIndex;
	//WORD        dump;//wIndex;
	BYTE        bValid;
	BYTE		dump;//cPrevPackedField;	//前一个图形控制的
    BYTE        dump0;
}GCONTROL, *LPGCONTROL ;

typedef struct _GBITMAP
{
	DWORD		dwType;
	int		    top;
	int		    left;
	int		    width;
	int		    height;

	UINT        uIndex;
	HBITMAP		hBitmap;
	//DWORD       dwOffsetInFile;  //在文件中的偏移
	GCONTROL    ctrl;
}GBITMAP, *LPGBITMAP;


typedef struct _GIF_OBJ
{
	HANDLE hThis;  //check it

    int gifWidth;
	int gifHeight;			       /* Screen dimensions. */
	UINT nTotalImage;
	DWORD dwDelay;
	COLORREF   rgbBackGroundColor;
	UINT uGlobalFlag;
	LPPTRLIST lpGifList;
	HBITMAP hbmpCache; // 用于保存之前的绘制数据
	HDC     hMemDC;
}GIF_OBJ, FAR * LPGIF_OBJ;


#endif //__GIFBMP_H
