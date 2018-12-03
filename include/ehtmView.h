/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
#ifndef _EHTMVIEW_H_20000702_JAMI_

#define _EHTMVIEW_H_20000702_JAMI_
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// ����������й���������


#include "eWindows.h"

//#define __WCE_DEFINE
//#define __JAMI_DEBUG
#undef __JAMI_DEBUG

#ifdef __JAMI_DEBUG
#define JAMIMSG  RETAILMSG

#define DBG_FUNCTION   1
#define DBG_CONTROL    1
#define DBG_PROCTEXT   1
#else
#define JAMIMSG  //

#define DBG_FUNCTION   0
#define DBG_CONTROL    0
#define DBG_PROCTEXT   0
#endif

#define HM_GO			0x3000
#define HM_GOBACK		0x3001
#define HM_FORWARDS		0x3002
#define HM_REFRESH      0x3003
#define HM_GOMARK		0x3004

/*
wParam  -- MAX Buffer Size
lParam  -- Buffer pointer

return --  success the return the Data Size , else return -1

remark -- if the MAX Buffer Size == 0 and The Buffer Pointer == 0 then return the Data Size

*/
#define HM_READDATA		0x3005

/*
wParam  -- 0
lParam  -- 0

return --  if return HTMLFILE ,then the open file is a html file.
		   if return IMAGEFILE , then the open file is a image file.
		   else failure return -1

*/
#define HTMLFILE     0x0000
#define IMAGEFILE    0x0001

#define HM_GETFILETYPE	0x3006


/*
wParam  -- BUffer max size
lParam  -- the pointer of the buffer

return --  if success then return TRUE 
		   if failure then return FALSE
*/
#define HM_GETLOCATEFILE	0x3007

/*
wParam  -- 0
lParam  -- 0

return --  if success then return TRUE 
		   if failure then return FALSE
*/
#define HM_STOP				0x3008

/*
wParam  -- 0
lParam  -- 0

return --  if success then return TRUE 
		   if failure then return FALSE
*/
#define HM_READJUSTSIZE		0x3009

typedef LPVOID HHTML;

//const TCHAR classHtmlView[];
//const TCHAR classSMIE_BROWERS[];

#define INM_ADDRESSCHANGE   0x3100

typedef struct {
	NMHDR   hdr;
    LPTSTR lpUrl;
}NMINTERNET, *LPNMINTERNET;



// ���������С
#define FONT_BIGGER		0  // ������
#define FONT_SMALLLER	1  // С����

/*
��������Ĵ�С
wParam  -- iFontSize
lParam  -- (BOOL)bShow;  // �Ƿ�Ҫ������ʾ

return --  if success then return TRUE 
		   if failure then return FALSE
*/
#define HM_SETFONTSIZE		0x300A


#define DISPLAY_TEXTIMAGE	0  // ��ʾͼ����ı�
#define DISPLAY_ONLYTEXT	1  // ֻ��ʾ�ı�
/*
// ������ʾģʽ
wParam  -- iDisplayMode; // ��ʾģʽ
lParam  -- (BOOL)bShow;  // �Ƿ�Ҫ������ʾ

return --  if success then return TRUE 
		   if failure then return FALSE
*/
#define HM_SETDISPLAYMODE		0x300B


#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //_EHTMVIEW_H_20000702_JAMI_
