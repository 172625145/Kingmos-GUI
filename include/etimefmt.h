/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __ETIMEFMT_H
#define __ETIMEFMT_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#define TIMEFMT_12HOUR		0x0001// 12Сʱ��ʽ
#define TIMEFMT_24HOUR		0x0000// 24Сʱ��ʽ,ΪĬ�ϸ�ʽ

// �õ���ǰ��ʱ���ʽ
#define GetSysTimeFormat SH_GetSysTimeFormat
DWORD WINAPI SH_GetSysTimeFormat(void);


// �õ���ǰ��ʱ���ʽ
#define SetSysTimeFormat SH_SetSysTimeFormat
BOOL WINAPI SH_SetSysTimeFormat(DWORD dwFlag);


// �õ���ʽ�����ʱ�䴮
int FormatTime(CONST SYSTEMTIME *lpTime, LPTSTR lpTimeStr,int cchTime ,DWORD dwFlag);
// dwFlag ����
#define FT_STANDARD		0x0000	//��׼��ʽ �� HH:MM:SS �� HH:MM ... ��ÿһλ��ʾ��λ  eg 09:38
#define FT_SIMPLIFY		0x0001	//�����ʽ �� H:M:S �� H:M ... ��ÿһλ������ʾһλ  eg 9:38
#define FT_LONGSTYLE	0X0000	//����ʽ   �� ��ʾʱ����
#define FT_SHORTSTYLE	0X0002	//�̸�ʽ   �� ��ʾʱ��

#ifdef __cplusplus
}
#endif  //__cplusplus


#endif  //__ETIMEFMT_H
