/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：中断服务API，提供系统中断服务
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>

#include <sysintr.h>
#include <oemfunc.h>
#include <eobjcall.h>
#include <coresrv.h>
#include <epcore.h>
/**************************************************
声明：void CALLBACK INTR_ISRFun( DWORD dwISRHandle );
参数：
	IN dwISRHandle - 中断回调传递的参数，该参数是ISR_RegisterServer时
                     所传入的参数值
  
返回值：
	无
功能描述：
	执行中断回调功能，这里仅仅简单的设置中断事件
引用: 
************************************************/

static void CALLBACK INTR_ISRFun( DWORD lpEvent )
{
	//RETAILMSG( 1, ( "INTR_ISRFun:%x.\r\n" ) );
	//KL_SetEvent( (HANDLE)dwISRHandle );
	_SemaphoreRelease( (LPSEMAPHORE)lpEvent, 1, NULL );
}

/**************************************************
声明：BOOL WINAPI KL_IntrInit( 
                  DWORD idInt, 
				  HANDLE hIntrEvent,
				  LPVOID lpvData, 
				  DWORD dwSize )

参数：
	IN idInt - 中断ID
	IN hIntrEvent - 中断事件句柄
	IN lpvData - 传递给OEM_InterruptEnable的参数
	IN dwSize - lpvData缓存大小（以byte为单位）

返回值：
	假如成功，返回TRUE;否则返回FALSE
功能描述：
	将一个中断ID与中断事件句柄关联，注册中断服务例程，打开中断ID对应的中断
引用: 
	系统调用，被中断服务线程调用
************************************************/
BOOL WINAPI KL_IntrInit( DWORD idInt, 
				  HANDLE hIntrEvent,
				  LPVOID lpvData, 
				  DWORD dwSize )
{
	if( idInt < SYSINTR_MAXIMUM )
	{   // 注册中断服务例程
		LPVOID lpvEvent = HandleToPtr( hIntrEvent, OBJ_EVENT );
		if( lpvEvent )
		{
			if( ISR_RegisterServer( idInt, INTR_ISRFun, (DWORD)lpvEvent ) )
			{   // 打开中断ID对应的中断
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
声明：void WINAPI KL_IntrDone( DWORD idInt );
参数：
	IN idInt - 中断ID
返回值：
	无
功能描述：
	中断线程已处理完相关的事务，应该调用该函数去重新
    打开中断ID对应的中断
引用: 
	系统调用，被中断服务线程调用
************************************************/

void WINAPI KL_IntrDone( DWORD idInt )
{
	OEM_InterruptDone( idInt );
}

/**************************************************
声明：void WINAPI KL_IntrDisable( DWORD idInt );
参数：
	IN idInt - 中断ID
返回值：
	无
功能描述：
	中断线程调用该函数去关掉中断ID对应的中断
引用: 
	系统调用，被中断服务线程调用
************************************************/

void WINAPI KL_IntrDisable( DWORD idInt )
{
	OEM_InterruptDisable( idInt );
}


/**************************************************
声明：void WINAPI KL_IntrEnable( DWORD idInt, 
                                 LPVOID lpvData,
                                 DWORD dwSize );
参数：
	IN idInt - 中断ID
	IN lpvData - 传递给OEM_InterruptEnable的参数
	IN dwSize - lpvData缓存大小（以byte为单位）
返回值：
	无
功能描述：
	中断线程调用该函数去打开中断ID对应的中断
引用: 
	系统调用，被中断服务线程调用
************************************************/
void WINAPI KL_IntrEnable( DWORD idInt, LPVOID lpvData, DWORD dwSize )

{
	OEM_InterruptEnable( idInt, lpvData, dwSize  );
}
