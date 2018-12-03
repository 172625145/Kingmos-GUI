/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __SMILSTRUCT_H
#define __SMILSTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ewindows.h"


typedef struct METASTRUCT{
	int a;
}META ,*LPMETA;

// REGION结构
typedef struct REGIONSTRUCT{
	LPTSTR id; // 保存当前REGION的ID号
	WORD top; // 当前region离最顶部的距离，以点为单位
	WORD left; // 当前region离最左边的距离，以点为单位
	WORD width; // 当前region的宽度，以点为单位
	WORD height; // 当前region 的高度，以点为单位、
	WORD fit;
	WORD fill;
	WORD hidden;
	WORD meet;
	WORD slice;
	struct REGIONSTRUCT *next;
}REGION, *LPREGION;

//ROOT-LAYOUT结构
#define SMIL_DEFAILTWIDTH	160
#define SMIL_DEFAILTHEIGHT  120


typedef struct ROOTLAYOUTSTRUCT{
	WORD width; // 当前SMIL的宽度，以点为单位
	WORD height; // 当前par的高度，以点为单位、
}ROOTLAYOUT ,*LPROOTLAYOUT;


// LAYOUT 结构
typedef struct LAYOUTSTRUCT{
	LPROOTLAYOUT lpRootLayout; // 当前SMIL的宽与高
	LPREGION lpRegion; // 每一个REGION的属性
}LAYOUT ,*LPLAYOUT;

// HEAD 结构
typedef struct HEADSTRUCT{
	LPMETA	lpMeta; // 当前消息的meta-information
	LPLAYOUT lpLayout; // 当前消息的REGION
}HEAD ,*LPHEAD;

// 图象结构
typedef struct IMGSTRUCT{
	LPTSTR lpRegion; // IMG指定的REGION
	LPTSTR lpSrc; // IMG指定的图象文件
	LPTSTR lpAlt; // 替代文本
	struct PARSTRUCT *next; // 下一幅图象指针
}IMG ,*LPIMG;

// 声音结构
typedef struct ADUOSTRUCT{
	LPTSTR lpSrc; // AUDIO指定的声音文件
	LPTSTR lpAlt; // 替代文本
	struct PARSTRUCT *next; // 下一个声音
}AUDIO ,*LPAUDIO;

// 文本结构
typedef struct TEXTSTRUCT{
	LPTSTR lpRegion; // TEXT指定的REGION
	LPTSTR lpSrc; // TEXT指定的文本文件名
	LPTSTR lpAlt; // 替代文本
	struct PARSTRUCT *next;
}TEXT ,*LPTEXT;

// REF 结构
typedef struct REFSTRUCT{
	LPTSTR lpSrc; //REF指定的文件
	LPTSTR lpAlt; // 替换文本
	struct PARSTRUCT *next;
}REF ,*LPREF;

// 幻灯片(PAR)结构
typedef struct PARSTRUCT{
	DWORD startTime; // 进入幻灯片后开始播放幻灯片的时间，默认为0
	DWORD durTime; // 当前播放幻灯片要延续的时间
	DWORD endTime; // 结束幻灯片的时间

	LPIMG lpImg; // 图象结构
	LPAUDIO lpAudio; // 声音结构
	LPTEXT	lpText; // 文本结构
	LPREF lpRef; // REF结构
	struct PARSTRUCT *next; // 指向下一个幻灯片的数据
}PAR ,*LPPAR;

// BODY 结构
typedef struct BODYSTRUCT{
	LPPAR lpPar; // 指向幻灯片结构
}BODY ,*LPBODY;
	
// SMIL结构
typedef struct SMILSTRUCT{
	LPHEAD lpHead; // HEAD结构
	LPBODY lpBody; // 文本结构
}SMIL ,*LPSMIL;


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif //__SMILSTRUCT_H
