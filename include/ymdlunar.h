/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __YMDLUNAR_H
#define __YMDLUNAR_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//ũ���㷨

//��������������� ����
//
/*************************************************
Lunar_GetYearRange
���ܣ���ȡ��ũ���㷨���귶Χ
ԭ�ͣ�void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax )
������
	pwYearMin - ���������
	pwYearMax - ���������
����ֵ����
************************************************/
extern	void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax );

/*************************************************
Lunar_CheckYear
���ܣ����wYear�꣬�Ƿ��ڸ�ũ���㷨���귶Χ��
ԭ�ͣ�BOOL	Lunar_CheckYear( WORD wYear )
������
	wYear - ��������
����ֵ��TRUE��ʾ�ڸ÷�Χ��
************************************************/
extern	BOOL	Lunar_CheckYear( WORD wYear );

//��������������� ����
//
/*************************************************
Lunar_GetYMDFromAD
���ܣ����ݹ��������գ��������Ӧ��ũ�������գ����Ƿ�ũ������
ԭ�ͣ�BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap )
������
	wYear - �����Ĺ�����
	wMonth - �����Ĺ�����
	wDay - �����Ĺ�����
	pwLYear - ����õ���ũ����
	pwLMonth - ����õ���ũ����
	pwLDay - ����õ���ũ����
	pbLeap - ����õ����Ƿ�ũ������
����ֵ��TRUE��ʾ����ɹ�
************************************************/
extern	BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap );

/*************************************************
Lunar_GetYMDsFromADMonth
���ܣ����ݹ������º�����µ�������������ڸù������¶�Ӧ��ũ��������
ԭ�ͣ�BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] )
������
	wYear - �����Ĺ�����
	wMonth - �����Ĺ�����
	wDaysOfMonth - �����Ĺ������е�����
	arrLunarYMD - ����õ���ũ�������յ�����
����ֵ��TRUE��ʾ����ɹ�
************************************************/
extern	BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] );

//��������������� ũ��
//
/*************************************************
Lunar_GetLeapMonth
���ܣ��õ����������
ԭ�ͣ�WORD	Lunar_GetLeapMonth( WORD wYear )
������
	wYear - ������ũ����
����ֵ��
	�õ���������£����û�оͷ���0
************************************************/
extern	WORD	Lunar_GetLeapMonth( WORD wYear );

/*************************************************
Lunar_GetDaysOfMonth
���ܣ��õ������µ�����
ԭ�ͣ�WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap )
������
	wYear - ������ũ����
	wMonth - ������ũ����
	bLeap - ������������Ƿ�Ϊ���£���2���£�
����ֵ���õ������µ�����
************************************************/
extern	WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap );

/*************************************************
Lunar_GetNameYear
���ܣ��õ�����ļ��ӱ�ʾ
ԭ�ͣ�void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp )
������
	wYear - ������ũ����
	pszTemp - ����õ���ũ����ļ���String
����ֵ����
************************************************/
extern	void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp );

/*************************************************
Lunar_GetNameMonth
���ܣ��õ����µ����ı�ű�ʾ����������������... ʮ������������
ԭ�ͣ�void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp )
������
	wMonth - ������ũ����
	pszTemp - ����õ���ũ���µ����ı�ű�ʾString
����ֵ����
************************************************/
extern	void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp );

/*************************************************
Lunar_GetNameDay
���ܣ��õ�����ָ���յ����ı�ű�ʾ������һ ... ʮ�� ... ��ʮ����
ԭ�ͣ�void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp )
������
	wMonth - ������ũ����
	wDay - ������ũ����
	pszTemp - ����õ���ũ���յ����ı�ű�ʾString
����ֵ����
************************************************/
extern	void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp );

/*************************************************
Lunar_GetNameYMD
���ܣ��õ��������յ����ı�ű�ʾ���������� ���� ��һ����
ԭ�ͣ�void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, TCHAR *szTemp )
������
	wYear - ������ũ����
	wMonth - ������ũ����
	wDay - ������ũ����
	pszTemp - ����õ���ũ�������յ����ı�ű�ʾString
����ֵ����
************************************************/
extern	void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, TCHAR *szTemp );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__YMDLUNAR_H
