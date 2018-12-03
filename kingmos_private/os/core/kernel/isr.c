/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����жϷ������̹���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h> 
#include <estring.h>
#include <eassert.h>
#include <epcore.h>
#include <sysintr.h>

typedef struct _ISR_DATA
{
    LPISR lpISRFun;  //�жϷ����������
	DWORD dwISRHandle;  //�жϷ������̲���
}ISR_DATA;

int iISRActiveCount = 0;
BYTE bISRActives[SYSINTR_MAXIMUM];
BYTE bISRMasks[SYSINTR_MAXIMUM];   // used by DisableISR and EnableISR

#define MAX_MASK_COUNT 255
static BYTE nISRMaskCount[SYSINTR_MAXIMUM];
static ISR_DATA isrData[SYSINTR_MAXIMUM];

/**************************************************
������void CALLBACK ISR_Error( DWORD dwHandle )
������
	IN dwHandle - �жϷ������̲������ò���Ϊ����ISR_RegisterServer�����ݵĲ���
  
����ֵ��
	��
����������
	�жϷ�������Ĭ�ϴ�����
����: 
************************************************/
static void CALLBACK ISR_DefHandler( DWORD dwHandle )
{// a error is hapend
	dwHandle++;
}

/**************************************************
������void ISR_Disable( UINT nIndex )
������
	IN nIndex - �жϷ�����������
  
����ֵ��
	��
����������
	��������ָ����жϷ������̣��������ж�ʱ,�����������ָ����жϷ������̣�
	ÿ����һ�θú������ú��������Ӷ�Ӧ�����μ�����
����: 
************************************************/

void ISR_Disable( UINT nIndex )
{
	ASSERT( nIndex < SYSINTR_MAXIMUM );
    if( nISRMaskCount[nIndex] < MAX_MASK_COUNT ) 
	    nISRMaskCount[nIndex]++;
	bISRMasks[nIndex] = 0;
}

/**************************************************
������void ISR_Enable( UINT nIndex )
������
	IN nIndex - �жϷ�����������
  
����ֵ��
	��
����������
	������ָ����жϷ������̣��������ж�ʱ,�����������ָ����жϷ������̡�
	�ú������ٶ�Ӧ�����μ�����������Ϊ0ʱ�����жϷ���
����: 
************************************************/
void ISR_Enable( UINT nIndex )
{
    ASSERT( nIndex < SYSINTR_MAXIMUM );
	if( nISRMaskCount[nIndex] ) 
	    nISRMaskCount[nIndex]--;
    if( nISRMaskCount[nIndex] == 0 )
    {   // ���жϷ���
	    bISRMasks[nIndex] = 1;
    }
}

/**************************************************
������void ISR_Active( UINT nIndex )
������
	IN nIndex - �жϷ�����������
  
����ֵ��
	��
����������
	��������ָ����жϷ������̣����̽�����ϵͳ���÷��ػ��豸�жϴ����귵��ʱ��鲢����ISR_Handler
����: 
************************************************/
#define DEBUG_ISR_Active 0
void ISR_Active( UINT nIndex )
{
//	ASSERT( nIndex < SYSINTR_MAXIMUM );
	
	if( nIndex < SYSINTR_MAXIMUM )
	{   // �Ƿ������ź�
	    if( bISRActives[nIndex] == 0 )
		{   // �������ź�
		    bISRActives[nIndex] = 1;
	        iISRActiveCount++;
		}
	}
	else
	{
		//ASSERT( 0 );
		ERRORMSG( DEBUG_ISR_Active, ( "error in ISR_Active :nIndex(%d), SYSINTR_MAXIMUM(%d).\r\n", nIndex, SYSINTR_MAXIMUM ) );
	}
}

//int iISREntry = 0;
/**************************************************
������void ISR_Handler( UINT idCurISR )
������
	IN idISR - ��ǰ���±�������жϷ�����������
  
����ֵ��
	��
����������
	�жϷ������̴������ģ��ú���������ϵͳ���÷��ػ��豸�жϴ����귵��ʱ��鲢���ã�
	�����øú���ʱ���ж��ǹص���
����: 
************************************************/
void ISR_Handler( UINT idISR )
{
	static int nISRHandlerCount = 0;
	int idx;	
	LPBYTE lpbMask, lpbActive;

	ISR_DATA * lpisr;


    // ��ֹ����
	if( nISRHandlerCount++ )
		return;
	
    lpCurThread->nLockScheCount++;


	if( idISR < SYSINTR_MAXIMUM )
	{
		if( bISRActives[idISR] && bISRMasks[idISR] )
		{   //�ȴ���ǰ��idISR
			bISRActives[idISR] = 0;
			iISRActiveCount--;
			INTR_ON();
			isrData[idISR].lpISRFun( isrData[idISR].dwISRHandle );
			INTR_OFF();
		}
	}

	//if( idISR == ISR_ALL_INTRS )

	// �Ƿ���û�д�����ж�
	if( iISRActiveCount )
	{   // �У�������� enum all isr		
		//iISRActiveCount = 0;
		
		lpisr = isrData;
		lpbMask = bISRMasks;
		lpbActive = bISRActives;
		for ( idx = 0; idx < SYSINTR_MAXIMUM && iISRActiveCount; idx++ ) 
		{
			if( *lpbMask && *lpbActive )
			{   // û�����β������ź�
				*lpbActive = 0;
				iISRActiveCount--;
				INTR_ON();
				lpisr->lpISRFun( lpisr->dwISRHandle );
				INTR_OFF();
			}
			lpisr++;
			lpbMask++;
			lpbActive++;
		}
	}

	lpCurThread->nLockScheCount--;
	nISRHandlerCount = 0;
	
}

/**************************************************
������BOOL ISR_RegisterServer( UINT nIndex, LPISR lpISRFun, DWORD dwISRHandle )
������
	IN nIndex - �жϷ�����������
	IN lpISRFun - �жϷ������̻ص�����
	IN dwISRHandle - ���ݸ��жϷ������̻ص������Ĳ���
  
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE
����������
	ע��һ���жϷ�������
����: 
************************************************/

BOOL ISR_RegisterServer( UINT nIndex, LPISR lpISRFun, DWORD dwISRHandle )
{
	if( nIndex < SYSINTR_MAXIMUM )
	{
		UINT uiSave;
		LockIRQSave( &uiSave ); // ���� & ���ж�
		isrData[nIndex].lpISRFun = lpISRFun;
		isrData[nIndex].dwISRHandle = dwISRHandle;
		nISRMaskCount[nIndex] = 0;  // enable
		bISRMasks[nIndex] = 1;  // enable
		bISRActives[nIndex] = 0; // no active
		UnlockIRQRestore( &uiSave ); // �ָ�
		return TRUE;
	}
	return FALSE;
}

/**************************************************
������void ISR_Unregister( UINT nIndex )
������
	IN nIndex - �жϷ�����������
	IN lpISRFun - �жϷ������̻ص�����
	IN dwISRHandle - ���ݸ��жϷ������̻ص������Ĳ���
  
����ֵ��
	��
����������
	ע��һ���жϷ�������
����: 
************************************************/

void ISR_Unregister( UINT nIndex )
{
	if( nIndex < SYSINTR_MAXIMUM )
	{
		ISR_Disable( nIndex );   // �ȹص��� disable

		bISRActives[nIndex] = 0; // no active
		isrData[nIndex].lpISRFun = ISR_DefHandler;
		isrData[nIndex].dwISRHandle = 0;
		bISRMasks[nIndex] = 0;
	}
}

/**************************************************
������void ISR_Init( void )
������
	��  
����ֵ��
	��
����������
	�жϷ������̹�������ʼ��
����: 
	��start.c����
************************************************/
BOOL ISR_Init( void )
{
	int i=0;
	for( i = 0; i < SYSINTR_MAXIMUM; i++ )
	{
		nISRMaskCount[i] = 0;
		//lppISR[i] = ISR_DefHandler;
		isrData[i].lpISRFun = ISR_DefHandler;
		isrData[i].dwISRHandle = 0;	
	}
	memset( bISRActives, 0, sizeof( bISRActives ) );
	memset( bISRMasks, 0, sizeof( bISRMasks ) );
	//dwISRActive = 0;
	//dwISRMask = 0;
	iISRActiveCount = 0;
	return TRUE;
}

