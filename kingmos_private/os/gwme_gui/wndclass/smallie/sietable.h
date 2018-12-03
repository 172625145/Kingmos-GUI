/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef __SIETABLE_H
#define __SIETABLE_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分
#include "eWindows.h"


// This is mark the HtmlObject Type
#define TYPE_HEAD    0   // the HtmlObject is a link head ,have no Object
#define TYPE_TABLE   1   // the HtmlObject is a Table
#define TYPE_CONTROL 2   // the HtmlObject is a Control
#define TYPE_CR      3   // the HtmlObject is a CR

#define HEAD_PART   1
#define BODY_PART   2
#define FOOT_PART   3
typedef WORD HTMLTYPE;
typedef LPVOID LPCONTENT;


typedef struct HtmlHeadStruct{
	struct HtmlObjectStruct *next;
	DWORD width;
	DWORD height;
}HTMLHEAD,*LPHTMLHEAD;

typedef struct HtmlObjectStruct{
	struct HtmlObjectStruct *next;
	HTMLTYPE type; // Normal Type, Table Type,...
	LPCONTENT   lpContent;
}HTMLOBJECT,*LPHTMLOBJECT;


typedef struct TDStruct{
	BOOL IsVirtual;     // is TRUE ,this TD is a position
	struct TDStruct *next;		// to the next td
	LPHTMLOBJECT lpItem;  // to the first html control of the td
	DWORD x;       
	DWORD y;         // The TD Start coordinate
	DWORD width;     // the width of the Td
	DWORD percent;
	DWORD height;	// the Height of the Td
	DWORD heightSet; // the height by the user
	COLORREF bgColor;
	DWORD colSpan;
	DWORD rowSpan;
	BOOL NoWrap;
}TD_TABLE,*LPTD_TABLE;

typedef struct TRStruct{
	struct TRStruct *next;		// to the next tr
	LPTD_TABLE td;		// to the first td of the tr
	DWORD x;       
	DWORD y;         // The TR Start coordinate
	DWORD width;     // the width of the TR
	DWORD height;	// the Height of the TR
	COLORREF bgColor;
}TR_TABLE,*LPTR_TABLE;

typedef struct{
	DWORD rows;
	DWORD cols;
	DWORD x;       
	DWORD y;         // The Table Start coordinate
	DWORD percent;
	DWORD width;     // the width of the Table (have cols number)
	DWORD height;	// the Height of the Table	(have rowss number)
	DWORD heightSet; // the height by the user
	LPDWORD heightPreRow;
	LPDWORD widthPreCol;
	LPTR_TABLE body;		// the Body part of the table
	LPTR_TABLE head;		// the head part of the head
	LPTR_TABLE foot;		// the foor part of the foot
	LPHTMLOBJECT lpItem;  // OBJECT
	COLORREF bgColor;
	DWORD cellSpacing;
	DWORD cellPadding;

	DWORD align;
	DWORD frame;
	DWORD rules;

}HTMLTABLE, *LPHTMLTABLE;

HHTML SMIE_ParseHtml(HWND hWnd,char *lpHtmlBuffer);
BOOL SMIE_ReCalcSize(HWND hWnd,HDC hdc,HHTML hHtml);
void SMIE_ShowHtml(HWND hWnd,HDC hdc,HHTML hHtml,POINT ptOrg);
void SMIE_ReleaseHtml(HHTML hHtml);
BOOL SMIE_GetUrl(HHTML hHtml,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL SMIE_GetHtmlSize(HHTML hHtml,LPSIZE lpSize);
TCHAR* BufferAssignTChar(TCHAR *pSrc);
BOOL SMIE_GetMarkPos(HHTML hHtml,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);

// !!! Add By Jami chen in 2003.09.09
HHTML SMIE_LoadImage(HWND hWnd,LPTSTR lpImageFile);
// !!! Add End By Jami chen in 2003.09.09

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //__SIETABLE_H
