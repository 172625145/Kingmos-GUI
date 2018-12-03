/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����жϷ���API���ṩϵͳ�жϷ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>

#include <sysintr.h>
#include <oemfunc.h>
#include <eobjcall.h>
#include <coresrv.h>
#include <epcore.h>
/**************************************************
������void CALLBACK INTR_ISRFun( DWORD dwISRHandle );
������
	IN dwISRHandle - �жϻص����ݵĲ������ò�����ISR_RegisterServerʱ
                     ������Ĳ���ֵ
  
����ֵ��
	��
����������
	ִ���жϻص����ܣ���������򵥵������ж��¼�
����: 
************************************************/

static void CALLBACK INTR_ISRFun( DWORD lpEvent )
{
	//RETAILMSG( 1, ( "INTR_ISRFun:%x.\r\n" ) );
	//KL_SetEvent( (HANDLE)dwISRHandle );
	_SemaphoreRelease( (LPSEMAPHORE)lpEvent, 1, NULL );
}

/**************************************************
������BOOL WINAPI KL_IntrInit( 
                  DWORD idInt, 
				  HANDLE hIntrEvent,
				  LPVOID lpvData, 
				  DWORD dwSize )

������
	IN idInt - �ж�ID
	IN hIntrEvent - �ж��¼����
	IN lpvData - ���ݸ�OEM_InterruptEnable�Ĳ���
	IN dwSize - lpvData�����С����byteΪ��λ��

����ֵ��
	����ɹ�������TRUE;���򷵻�FALSE
����������
	��һ���ж�ID���ж��¼����������ע���жϷ������̣����ж�ID��Ӧ���ж�
����: 
	ϵͳ���ã����жϷ����̵߳���
************************************************/
BOOL WINAPI KL_IntrInit( DWORD idInt, 
				  HANDLE hIntrEvent,
				  LPVOID lpvData, 
				  DWORD dwSize )
{
	if( idInt < SYSINTR_MAXIMUM )
	{   // ע���жϷ�������
		LPVOID lpvEvent = HandleToPtr( hIntrEvent, OBJ_EVENT );
		if( lpvEvent )
		{
			if( ISR_RegisterServer( idInt, INTR_ISRFun, (DWORD)lpvEvent ) )
			{   // ���ж�ID��Ӧ���ж�
				if( OEM_InterruptEnable( idInt, lpvData, dwSize ) )
				{				
					return TRUE;
				}
				ISR_Unregister( idInt );
			}
		}
	}
	return FALSE;
}

/**************************************************
������void WINAPI KL_IntrDone( DWORD idInt );
������
	IN idInt - �ж�ID
����ֵ��
	��
����������
	�ж��߳��Ѵ�������ص�����Ӧ�õ��øú���ȥ����
    ���ж�ID��Ӧ���ж�
����: 
	ϵͳ���ã����жϷ����̵߳���
************************************************/

void WINAPI KL_IntrDone( DWORD idInt )
{
	OEM_InterruptDone( idInt );
}

/**************************************************
������void WINAPI KL_IntrDisable( DWORD idInt );
������
	IN idInt - �ж�ID
����ֵ��
	��
����������
	�ж��̵߳��øú���ȥ�ص��ж�ID��Ӧ���ж�
����: 
	ϵͳ���ã����жϷ����̵߳���
************************************************/

void WINAPI KL_IntrDisable( DWORD idInt )
{
	OEM_InterruptDisable( idInt );
}


/**************************************************
������void WINAPI KL_IntrEnable( DWORD idInt, 
                                 LPVOID lpvData,
                                 DWORD dwSize );
������
	IN idInt - �ж�ID
	IN lpvData - ���ݸ�OEM_InterruptEnable�Ĳ���
	IN dwSize - lpvData�����С����byteΪ��λ��
����ֵ��
	��
����������
	�ж��̵߳��øú���ȥ���ж�ID��Ӧ���ж�
����: 
	ϵͳ���ã����жϷ����̵߳���
************************************************/
void WINAPI KL_IntrEnable( DWORD idInt, LPVOID lpvData, DWORD dwSize )

{
	OEM_InterruptEnable( idInt, lpvData, dwSize  );
}
