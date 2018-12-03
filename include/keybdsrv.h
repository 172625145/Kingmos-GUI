/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
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

1.������COMBOTOOLBAR��
	a.�����Ҫ������ʾ/�����л����򴴽�COMBOTOOLBARʱ��ʹ��ԭ����CTS_IMICON
	��񼴿ɣ�COMBOTOOLBAR���Զ��������̣���ԭ���л���ʾʱ��֪ͨ��Ϣ���䡣
	b.�������Ҫ������ʾ/�����л����򴴽�COMBOTOOLBARʱʹ�õ�CTS_IMNEED
	��񼴿ɣ�COMBOTOOLBAR���Զ��������̣�����û�м����л���ͼ�갴ť��
	c.�û���Ҫ�Լ����趨���̵���ʾ״̬��������Ϣ:

		//��ʾ����
		SendMessage(hToolBar,CTB_SHOWKEYBOARD,TRUE,0);

		//���ؼ���
		SendMessage(hToolBar,CTB_SHOWKEYBOARD,FALSE,0);
		
		COMBOTOOLBAR��������ʱĬ��Ϊ��������
	d.�û�Ҫ�õ���ǰ�ļ���״̬��������Ϣ:
		dwKBStatus = SendMessage(hToolbar,CTB_GETKBSTATUS,0,0);

		dwKBStatus == STATUS_KB_SHOW  Ϊ��ǰ����Ϊ��ʾ״̬
		dwKBStatus == STATUS_KB_HIDE  Ϊ��ǰ����Ϊ����״̬
		dwKBStatus == STATUS_KB_NOEXIST  Ϊ��ǰ���̲�����

2.����û��COMBOTOOLBAR������Ҫ�û��Լ�����������Ϊ

	HWND CreateKeyboard(HINSTANCE hInstance, HWND hParent, int yPos);

	hInstance -- ����ʵ�����
	hParent -- ���̴��ڵĸ�����
	yPos -- ���̵�λ�ã���Ļ���꣩�����Ϊ-1��ΪĬ��λ�ã���COMBOTOOLBAR���Ϸ���

	����ֵΪ�����ļ��̴��ھ����

	��Ҫ��ʾ/���ش��ڵ���
	VOID ShowKeyboard(HWND hWnd,DWORD dwFlag);

	hWnd -- ���̴��ھ��
	dwFlag -- ��ʾ״̬��SIP_SHOW -- Ҫ����ʾ���̣�SIP_HIDE -- Ҫ�����ؼ���
	
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


