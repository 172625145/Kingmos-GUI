/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __TITLEBAR_H
#define __TITLEBAR_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

extern const TCHAR	classTitleBar[];
//���������Ʒ��
//����ʾ���
#define TBCS_BATTERY            0x0001
//����ʾʱ��
#define TBCS_TIME               0x0002

//��ʾ�رհ�ť������и÷�������ð�ť�󣬻�������ڷ� 
// Message = WM_COMMAND
// wParam = IDCANCEL
// lParam = �ñ��������ھ��
#define TBCS_CANCEL_BOX             0x0004
#define TBCS_OK_BOX                0x0008
#define TBCS_HELP_BOX              0x0010

//����֮ǰ�������ȵ��� InitThirdpartControls �� InitThirdpartControlsEx
#define CreateTitleBar TitleBar_Create
HWND WINAPI TitleBar_Create( 
					 LPTSTR lpszTitle,
					 DWORD dwTitleBarStyle,
					 int x,			//�ڸ����ڵĿ�ʼ x ����
					 int y,			//�ڸ����ڵĿ�ʼ y ����
					 HWND hParent	//�����ھ��
					);

#ifdef __cplusplus
}
#endif      // __cplusplus

#endif  //__TITLEBAR_H
