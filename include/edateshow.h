/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __EDATESHOW_H
#define __EDATESHOW_H

#include <edatesel.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//---------------------------------------------------
//���µĿ�����Ϣ�Ƿ���  DateShow����Ϣ���뷢��DateSel����Ϣ������ͬ���塣
//
//������Ϣ									//wParam, lParam
//
//��ȡ����
//DSM_GETDATETIME-----�ο� edatesel.h �е���Ϣ�����������

//��������
//DSM_SETDATETIME-----�ο� edatesel.h �е���Ϣ�����������

//ѡ����֪ͨ�û�(��ʧȥ���� ��2�ε��� ʱ)
//DSM_SETUSERSELNOTIFY-�ο� edatesel.h �е���Ϣ�����������

//�����귶Χ
//DSM_SETYEARRANGE----�ο� edatesel.h �е���Ϣ�����������

//����DateSel����Ϣ
//DSM_SETDAYSFONT-----�ο� edatesel.h �е���Ϣ�����������
//DSM_SETDATESTYLE----�ο� edatesel.h �е���Ϣ�����������

#define DSM_SETDATESIZE			(WM_USER+20)	//(cx, cy), 0
#define DSM_GETMODIFY			(WM_USER+21)	//0, 0,  (returned val: fGetModified)
#define DSM_SETMODIFY			(WM_USER+22)	//fSetModified, 0

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EDATESHOW_H
