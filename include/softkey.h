/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __SOFTKEY_H
#define __SOFTKEY_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 

#define KEYBOARDWIDTH  240
#define KEYBOARDHEIGHT 82

// 设置到下一个键盘
#define KM_SETNEXTKEYBD                   (WM_USER+0)  // 将键盘切换到下一个


// 设置一个命名键盘
#define KB_SPELL		0x00    // 拼音键盘
#define KB_ENGLISH		0x01	// 英文键盘
#define KB_SYMBOL		0x02	// 符号键盘
#define KB_HANDWRITE	0x03	// 手写键盘


#define KB_NUMBERIC		0x00
#define KB_SYMBOL1		0x01
#define KB_SYMBOL2		0x02
//  参数说明 
//	wParam  -- UINT uKeyType  == KB_SPELL | KB_ENGLISH | KB_SYMBOL | KB_HANDWRITE
//	lParam  =  UINT uKeyType  == KB_NUMBERIC | KB_SYMBOL1 | KB_SYMBOL2
#define KM_SETNAMEDKEYBD						(WM_USER+1)  // 设置一个指定的键盘


// 设置到原来的键盘
//  参数说明 
//	wParam  = 0
//	lParam  = 0
#define KM_GOBACKBD						(WM_USER+2)  
#ifdef __cplusplus
}           
#endif  // __cplusplus
#endif  //__SOFTKEY_H
