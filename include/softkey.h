/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __SOFTKEY_H
#define __SOFTKEY_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 

#define KEYBOARDWIDTH  240
#define KEYBOARDHEIGHT 82

// ���õ���һ������
#define KM_SETNEXTKEYBD                   (WM_USER+0)  // �������л�����һ��


// ����һ����������
#define KB_SPELL		0x00    // ƴ������
#define KB_ENGLISH		0x01	// Ӣ�ļ���
#define KB_SYMBOL		0x02	// ���ż���
#define KB_HANDWRITE	0x03	// ��д����


#define KB_NUMBERIC		0x00
#define KB_SYMBOL1		0x01
#define KB_SYMBOL2		0x02
//  ����˵�� 
//	wParam  -- UINT uKeyType  == KB_SPELL | KB_ENGLISH | KB_SYMBOL | KB_HANDWRITE
//	lParam  =  UINT uKeyType  == KB_NUMBERIC | KB_SYMBOL1 | KB_SYMBOL2
#define KM_SETNAMEDKEYBD						(WM_USER+1)  // ����һ��ָ���ļ���


// ���õ�ԭ���ļ���
//  ����˵�� 
//	wParam  = 0
//	lParam  = 0
#define KM_GOBACKBD						(WM_USER+2)  
#ifdef __cplusplus
}           
#endif  // __cplusplus
#endif  //__SOFTKEY_H
