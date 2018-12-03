/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __COMBOTOOLBAR_H
#define __COMBOTOOLBAR_H

#include "edef.h"
#include <eToolBar.h>

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus 


#define CTB_ADDBITMAP				(WM_OS + 520)
#define CTB_ADDBUTTONS				(WM_OS + 521)
#define CTB_ADDSTRING				(WM_OS + 522)
#define CTB_AUTOSIZE				(WM_OS + 523)
#define CTB_BUTTONCOUNT				(WM_OS + 524)
#define CTB_CHANGEBITMAP			(WM_OS + 525)
#define CTB_CHECKBUTTON				(WM_OS + 526)
#define CTB_COMMANDTOINDEX			(WM_OS + 527)
#define CTB_DELETEBUTTON			(WM_OS + 528)
#define CTB_ENABLEBUTTON			(WM_OS + 529)
#define CTB_GETBITMAP				(WM_OS + 530)
#define CTB_PRESSBUTTON				(WM_OS + 531)
#define CTB_SETBUTTONINFO			(WM_OS + 532)
#define CTB_SETCMDID				(WM_OS + 533)
#define CTB_SETDISABLEDIMAGELIST	(WM_OS + 534)
#define CTB_SETIMAGELIST			(WM_OS + 535)
#define CTB_LOADIMAGES				(WM_OS + 536)


#define CTB_SETMENU					(WM_OS + 537)
#define CTB_GETMENU					(WM_OS + 538)
#define CTB_HIDEBUTTON              (WM_OS + 539) 
#define CTB_SHOWKEYBOARD            (WM_OS + 540) 


#define STATUS_KB_NOEXIST	0
#define STATUS_KB_SHOW		1
#define STATUS_KB_HIDE		2

// 参数： 无
// 返回：STATUS_KB_SHOW  -- 键盘在显示状态
//		 STATUS_KB_HIDE  -- 键盘在隐藏状态
//		 STATUS_KB_NOEXIST -- 键盘不存在
#define CTB_GETKBSTATUS				(WM_OS + 541) 


#define CTS_TOOLMENU				0x0001
#define CTS_IMICON					0x0002
#define CTS_ICONTOOLBAR				0x0004
#define	CTS_IMNEED					0x0008


typedef struct _NM_COMBOTOOLBAR {
    NMHDR hdr;
	BOOL bShow;  // TRUE, 显示键盘，否则隐藏键盘
} NM_COMBOTOOLBAR, * LPNM_COMBOTOOLBAR;

#define CTN_IMSHOWCHANGED			(0-1200)
// IM的显示状态发生改变


#ifdef __cplusplus
}                       
#endif  // __cplusplus


#endif // __COMBOTOOLBAR_H
