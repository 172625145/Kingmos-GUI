/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/

#ifndef __THIRD_CANON_H
#define __THIRD_CANON_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#define STATE_BAR_HEIGHT   68 
#define STATE_BAR_WIDTH    630
#define STATE_BAR_STARTX   5
#define STATE_BAR_STARTY   409

#define TITLE_BAR_HEIGHT   50
#define TITLE_BAR_WIDTH    640

//工作域是指出了顶部的状态条外的其它部分
//屏幕坐标
#define WORK_AREA_HEIGHT   (STATE_BAR_STARTY - TITLE_BAR_HEIGHT)
#define WORK_AREA_WIDTH    640
#define WORK_AREA_STARTX   0
#define WORK_AREA_STARTY   50


// add by alex
#define MAX_LOADSTRING 100

#define RGB_Transparent				  RGB(255, 0,   255)
#define RGB_WHITE					  RGB(255, 255, 255)		
#define RGB_BLUE					  RGB(0, 52, 113)
#define RGB_DARKGRAY				  RGB(13, 13, 13)
#define RGB_ORANGE					  RGB(255, 202, 92)		
#define RGB_GREEN					  RGB(0, 200, 0)
#define RGB_ITEMUNSELECT			  RGB(49, 50, 49)	

#define TEXT_LEFT_ALIGN		(12)
#define TEXT_RIGHT_ALIGN    (12)
#define TEXT_TOP_ALIGN		(10)

#define ITEM_HEIGHT			(47)
#define Y_PITCH				(50)

#define SETUP_ITEM_L_WIDTH	(580)
#define SETUP_ITEM_L_HEIGHT (47)

#define SUB_MENU_ITEM_WIDTH			(293)
#define SUB_MENU_ITEM_HEIGHT		(43)
#define SUB_MENU_ITEM_PITCH			(43)
#define SUB_MENU_ITEM_OFFSETLEFT    (42-12)	//12 is arrow width
#define ARROW_INDICTOR_WIDTH		(12)
#define ARROW_INDICTOR_HEIGHT		(22) 

#define SUB_MENU2_WIDTH	    (337)
#define SUB_MENU2_HEIGHT    (102)	
#define SUB_MENU3_WIDTH		(337)
#define SUB_MENU3_HEIGHT	(143)	

#define WM_SETUP_CMD	(WM_USER+100)
#define WM_REFERSH   	(WM_USER+101)
#define WM_BACK0201001	(WM_USER+200)


//add end


// 定义 alg message box
int RegisterAngelusButtonClass( HANDLE h );
ATOM RegisterCanolstClass(HINSTANCE hInstance);
//VOID InitThirdpartControls( VOID );
VOID ActiveDesktop( VOID );
extern const char classAGL_BUTTON[];
extern const char classAGL_DIALOG[];


#define AGL_INFO_TYPE  0x00000000
#define AGL_WARN_TYPE  0x00000001
#define AGL_UNSC_TYPE  0x00000002

#define AGL_BUTTON_ID_BASE  111
int WINAPI AGL_MessageBox( HWND hParent,   //父窗口
						   LPCTSTR lpText,	//需要显示的文本
						   LPCTSTR lpCaption,		//标题文本
						   UINT uType,		// 风格, = AGL_WARN_TYPE or AGL_INFO_TYPE or AGL_UNSC_TYPE
						   const char * szButtonText[],		//button 文本指针，与 cButton 配合
						   int cButton			// button数
						   );
 

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif  // __THIRD_CANON_H
