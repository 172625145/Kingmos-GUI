/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ӳٴ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <eobjtype.h>
#include <eassert.h>
#include <epcore.h>
#include <epalloc.h>

extern DWORD OEM_TimeToJiffies( DWORD dwMilliseconds, DWORD dwNanoseconds );

// ********************************************************************
// ������VOID WINAPI KL_Sleep( DWORD dwMilliseconds )
// ������
//		IN dwMilliseconds - �ӳ�ʱ�䣨���룩����Ϊ0 �� INFINITE
// ����ֵ��
//		��
// ����������
//		�ӳ�һ��ʱ��
// ���ã�
//		ϵͳAPI
// ********************************************************************
VOID WINAPI KL_Sleep( DWORD dwMilliseconds )
{
	if( dwMilliseconds == 0 )
	{	//�������̵߳�ʱ��Ƭ		
		lpCurThread->nTickCount  = 0;
		lpCurThread->nBoost = 0;
	}


	ScheduleTimeout( dwMilliseconds );
}
	