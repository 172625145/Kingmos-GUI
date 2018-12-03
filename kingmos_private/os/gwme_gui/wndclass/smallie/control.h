/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef __CONTROL_H
#define __CONTROL_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分


//#include "Windows.h"
#include "Locate.h"
#include "SIETable.h"
#include "Gif2Bmp.h"

typedef void * HHTMCTL;
// !!! Modified By Jami chen for WCE
//typedef char * NAME;  
//typedef char * IMAGE; 
//typedef char * URL;  
//typedef COLORREF COLOR ; 
//typedef char * FONT;
typedef TCHAR * CONTENT;  
typedef TCHAR * NAME;  
typedef TCHAR * IMAGE; 
typedef TCHAR * URL;  
typedef COLORREF COLOR ; 
typedef TCHAR * FONT;
// !!! Modified End

#define SHTM_EXISTURL  0x00000001

typedef struct{
	DWORD x;   // the x_position of the next Control
	DWORD y;	  // the y_position of the next Control
	FONT font; // the font of current state
	URL  url;   // the url of the current state
	NAME  name;   // the name of the current state
	COLOR color;  // the color of the current state
	BOOL bPreProcess;
}CONTROLSTATE,*LPCONTROLSTATE;


#define CONTROLTYPE_TEXT    0
#define CONTROLTYPE_INPUT   1
#define CONTROLTYPE_SELECT  2

typedef struct{					//								|<----width-------->|
												//						|<--------->| --> widthFirstRow
	WORD type;         //  TEXT:  The Normal Text
					   //  INPUT:  The Input Type,include Edit and Button
					   //  SELECT:  The ComBox,can be Selected by user 

	CONTENT content;
	DWORD  x;	 // The Position of the control					-------------------------		
	DWORD  y;	//												|		Example a Te|  ^
	DWORD  width;  // the width of this control					|xt control.abcdefgh|  |
	DWORD  height;  // the height of this control				|dfhaskdfhkasdhflgfk| height
	NAME  name;   // the Name of the Position					|dfsdfas			|  _
	IMAGE image;  // The Image to be Show of the control		|					|
//	HGIF  hGif;  // The Image to be Show of the control	    	|					|
	URL   url;    // the url to be link of the control			|					|
	COLOR color;  // the color to be show of the control		|					|
	FONT font;    // the font to be showed of the control		---------------------
	DWORD widthFirstRow;  // the width with the first row		|<----->| --> widthLastRow
	DWORD widthLastRow;  // the width with the last row
	DWORD heightLine;
}HTMLCONTROL,*LPHTMLCONTROL;

HHTMCTL SMIE_CreateHtmlControl(	POINT point,
						  NAME  name,
						  IMAGE image,	
						  URL   url,
						  COLOR color);
void SMIE_ShowHtmlControl(HWND hWnd,HDC hdc,HHTMCTL hHtmlCtl,RECT rect,POINT ptOrg);

LPHTMLOBJECT SMIE_ParseHtmlControl(HWND hParentWnd,TAGID tagID,char **stream,	LPCONTROLSTATE lpControlState);
//BOOL ReCalcSize_Control(HDC hdc,HHTMCTL lpControl,WORD iMaxWidth);
BOOL SMIE_ReCalcSize_Control(HWND hWnd,HDC hdc,HHTMCTL lpControl,DWORD iStart,RECT rect) ;

//BOOL ReLocate_Control(HHTMCTL lpControl,LPWORD lpx,LPWORD lpy,RECT rect,HHTMCTL lpPreControl);
//BOOL ReLocate_Control(HHTMCTL lpControl,LPWORD lpx,LPWORD lpy,RECT rect,WORD iRowHeight);
BOOL SMIE_ReLocate_Control(HWND hwnd,HHTMCTL lpControl,LPDWORD lpx,LPDWORD lpy,RECT rect,DWORD iRowHeight);
void SMIE_ReleaseControl(HHTMCTL lpControl);
BOOL SMIE_GetUrlInControl(HHTMCTL lpControl,DWORD x,DWORD y,LPTSTR *lppUrl);
BOOL SMIE_GetMarkPosInControl(HHTMCTL lpControl,LPINT lpxPos,LPINT lpyPos,LPTSTR lpMark);


// !!! Modified By Jami chen for WCE
//char * BufferAssign(const char *pSrc);
// !!!!
//TCHAR* SMIE_BufferAssign(const char *pSrc);
// !!!! Modified End
DWORD SMIE_HexToDword(char *ptr);
COLOR SMIE_GetColor(char *lpColor);
WORD SMIE_GetFontHeight(HWND hWnd,TCHAR *font);

TCHAR* BufferAssign(const char *pSrc);
TCHAR* BufferAssignTChar(TCHAR *pSrc);

// !!! Add By Jami chen in 2003.09.09
LPHTMLOBJECT SMIE_ControlLoadImage(HWND hWnd,LPTSTR lpImageFile);
// !!! Add End By Jami chen in 2003.09.09

void SMIE_DownLoadSpecialFile( HWND  hWnd,LPTSTR lpUrl);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //__CONTROL_H
