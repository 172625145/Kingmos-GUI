/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __KEYBDSRV_H
#define __KEYBDSRV_H

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

/*

1.窗口有COMBOTOOLBAR，
	a.如果需要键盘显示/隐藏切换，则创建COMBOTOOLBAR时仍使用原来的CTS_IMICON
	风格即可，COMBOTOOLBAR会自动创建键盘，且原来切换显示时的通知消息不变。
	b.如果不需要键盘显示/隐藏切换，则创建COMBOTOOLBAR时使用的CTS_IMNEED
	风格即可，COMBOTOOLBAR会自动创建键盘，但是没有键盘切换的图标按钮。
	c.用户需要自己来设定键盘的显示状态，发送消息:

		//显示键盘
		SendMessage(hToolBar,CTB_SHOWKEYBOARD,TRUE,0);

		//隐藏键盘
		SendMessage(hToolBar,CTB_SHOWKEYBOARD,FALSE,0);
		
		COMBOTOOLBAR创建键盘时默认为键盘隐藏
	d.用户要得到当前的键盘状态，发送消息:
		dwKBStatus = SendMessage(hToolbar,CTB_GETKBSTATUS,0,0);

		dwKBStatus == STATUS_KB_SHOW  为当前键盘为显示状态
		dwKBStatus == STATUS_KB_HIDE  为当前键盘为隐藏状态
		dwKBStatus == STATUS_KB_NOEXIST  为当前键盘不存在

2.窗口没有COMBOTOOLBAR，则需要用户自己创建，函数为

	HWND CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos);

	hInstance -- 窗口实例句柄
	hParent -- 键盘窗口的父窗口
	yPos -- 键盘的位置（屏幕坐标），如果为-1则为默认位置，即COMBOTOOLBAR的上方。

	返回值为创建的键盘窗口句柄。

	需要显示/隐藏窗口调用
	VOID ShowKeyboard(HWND hWnd,DWORD dwFlag);

	hWnd -- 键盘窗口句柄
	dwFlag -- 显示状态，SIP_SHOW -- 要求显示键盘，SIP_HIDE -- 要求隐藏键盘
	
*/


#define CreateKeyboard KB_CreateKeyboad
HWND WINAPI KB_CreateKeyboad(HINSTANCE hInstance, HWND hWnd, int yPos);

#define SIP_SHOW	1
#define SIP_HIDE	2

#define ShowKeyboard KB_ShowKeyboard
VOID WINAPI KB_ShowKeyboard(HWND hWnd,DWORD dwFlag);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif   // __KEYBDSRV_H


