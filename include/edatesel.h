/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __EDATESEL_H
#define __EDATESEL_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//---------------------------------------------------
//������Ϣ									//wParam, lParam
//
//��ȡ����
#define DSM_GETDATETIME			(WM_USER+3)	//0, (SYSTEMTIME*)

//��������
#define DSM_SETDATETIME			(WM_USER+4)	//0, (SYSTEMTIME*)

//ѡ����֪ͨ�û�(��ʧȥ���� ��2�ε��� ʱ)
#define	DSM_SETUSERSELNOTIFY	(WM_USER+5)	//hWnd, uMsg

//�����귶Χ
#define	DSM_SETYEARRANGE		(WM_USER+6)	//(Low-Min, High-Max), 0

//����Draw Days �������С
#define DSM_SETDAYSFONT			(WM_USER+7)	//0, 0/1/2(����lParam ������)
#define DAYSFONT_DEFAULT		0
#define DAYSFONT_8X8			1
#define DAYSFONT_8X6			2

//�����ض����ܵķ��
#define DSM_SETDATESTYLE		(WM_USER+8)	//DateStyle, 0
#define DSS_SHOWMPREV			0x00000001L	//
#define DSS_SHOWMNEXT			0x00000002L	//
#define DSS_USERSEL				0x00000004L	//2�ε���--ѡ�е�֪ͨ������Ǳ�����������--ȡ��ѡ���֪ͨ(����DSM_SETUSERSELNOTIFY)
//

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EDATESEL_H
