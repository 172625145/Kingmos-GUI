/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵�������������
�汾�ţ�1.0.0
����ʱ�ڣ�2003-03-18
���ߣ��½��� Jami chen
�޸ļ�¼��
******************************************************/

#include "ewindows.h"
#include "SipApi.h"

extern const char classKEYBOARD[];

// **************************************************
// ������static HWND GetKeyboardWindow(void)
// ������
// 	��
// ����ֵ�����ؼ��̴��ڵľ��
// �����������õ����ڵľ��
// ����: 
// **************************************************
static HWND GetKeyboardWindow(void)
{
	return FindWindow( classKEYBOARD, NULL ) ;  // ���Ҽ��̴���
}

// **************************************************
// ������BOOL Kingmos_SipShowIM( DWORD dwFlag )
// ������
// 	IN dwFlag
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ʾ/���������
// ����: 
// **************************************************
BOOL Kingmos_SipShowIM( DWORD dwFlag )
{
	HWND hKeyboard;
	
		hKeyboard = GetKeyboardWindow(); // �õ����̴��ھ��
		if (hKeyboard)
		{
			if (dwFlag & SIPF_OFF)
			{ // ���ؼ���
				ShowWindow(hKeyboard,SW_HIDE);
			}
			else
			{ // ��ʾ����
				ShowWindow(hKeyboard,SW_SHOW);
			}
		}
		return TRUE;
}
