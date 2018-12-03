/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����Kingmos ��������ʼ����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
    2003-05-28: �����Ȩ��Ϣ
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <eassert.h>
#include <coresrv.h>
#include <epcore.h>
#include <romheader.h>
#include <oemfunc.h>
#include <emlos.h>
extern BOOL InitialKingmosSystem( HINSTANCE );
extern BOOL InitScheduler( void );
extern BOOL _InitSysMem( void );

// ********************************************************************
// ������void CpuIdle( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		���ں˳�ʼ����ɺ󣬽���ô����������״̬
// ���ã�
//		
// ********************************************************************
//DWORD dwSysIdleTickCount;
static void CpuIdle( void )
{
	//DWORD dwPreTickCount;
	//BOOL bEnterIdleMode = FALSE;


    EdbgOutputDebugString( "***********************************************\r\n" );
	EdbgOutputDebugString( "*                                             *\r\n" );
	EdbgOutputDebugString( "*              �޹�.Kingmos(R)                *\r\n" );
	EdbgOutputDebugString( "*           Version:%d.%d.%d                  *\r\n", MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION );
	EdbgOutputDebugString( "*                                             *\r\n" );
	EdbgOutputDebugString( "* Copyright(c) 1998-2004 ΢�߼�(WEILUOJI)     *\r\n" );
	EdbgOutputDebugString( "*            All rights reserved              *\r\n" );
	EdbgOutputDebugString( "*           http://www.mlg.com.cn             *\r\n" );
    EdbgOutputDebugString( "* Core Make Date:%s,Time:%s *\r\n", __DATE__, __TIME__ );
	EdbgOutputDebugString( "***********************************************\r\n" );


	//dwSysIdleTickCount = 0;
	bNeedResched = 1;


	Schedule();
	//dwPreTickCount = KL_GetTickCount();
//	dwSysIdleTickCount = KL_GetTickCount();
    while( 1 )
    {		

		//DWORD dwCurTickCount, dwDiff;

//#ifdef EML_WIN32
  //      Win32_Sleep(1);  // ��MS Window ϵͳ����Ȩȥ���������飬����MS Window�����ܴ���½�
		//Win32_SwitchToThread();
		//Win32_GetTickCount();
//#endif

        lpCurThread->nTickCount = 0;

#ifndef TIMER_RESCHE
		// ���ϵͳ�ǲ���ʱ��Ƭ��ִ���Զ����ȹ��̣�������������ش���
		// ȥ���ȣ�����ϵͳ��Զ����������
        if( bNeedResched )	// �Ƿ��е������� ��
        { // �У�ִ�е���
            Schedule();
			//dwPreTickCount = KL_GetTickCount();
        }
#endif
		// �ж��Ƿ����ź� ������в���û�����Σ���ִ������źŴ���
	    if( lpCurThread->dwSignal & ~lpCurThread->dwBlocked )
		{	// ��
		    HandleSignal();
			Schedule();
			//dwPreTickCount = dwCurTickCount = KL_GetTickCount();
		}
		else
		{	// �񣬼���Ƿ��������ģʽ 
			OEM_EnterIdleMode( 0 );
			//dwCurTickCount = KL_GetTickCount();
			//if( dwCurTickCount > dwPreTickCount )
			//{
			//	if( (dwDiff = dwCurTickCount - dwPreTickCount) > 5000 )
			//		bEnterIdleMode = TRUE;
			//}
			//else
			//{
			//	if( (dwDiff=dwPreTickCount - dwCurTickCount) > 5000 )
			//		bEnterIdleMode = TRUE;
			//}
			//if( bEnterIdleMode )
			//{
			
			//	dwPreTickCount = dwCurTickCount = KL_GetTickCount();
			//	bEnterIdleMode = FALSE;
			//}
		}
    }
}
// ********************************************************************
// ������DWORD KingmosStart( LPVOID lParam )
// ������
//		IN lParam - ���� 
// ����ֵ��
//		0
// ����������
//		��CPU��ʼ�����Ժ󣬽���ô����ʼ��Kingmosϵͳ
// ���ã�
//		
// ********************************************************************
DWORD KingmosStart( LPVOID lParam )
{

#ifdef EML_WIN32
	INTR_OFF();	// �������ж�
#endif

	if( InitialKingmosSystem( 0 ) == FALSE )	// ��ʼ��ϵͳ
		return FALSE;

#ifdef EML_WIN32 
	INTR_ON();// ���жϣ�enable intr
#endif

	CpuIdle();
    return 0;
}
