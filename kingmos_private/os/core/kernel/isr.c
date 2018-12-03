/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：中断服务例程管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h> 
#include <estring.h>
#include <eassert.h>
#include <epcore.h>
#include <sysintr.h>

typedef struct _ISR_DATA
{
    LPISR lpISRFun;  //中断服务例程入口
	DWORD dwISRHandle;  //中断服务例程参数
}ISR_DATA;

int iISRActiveCount = 0;
BYTE bISRActives[SYSINTR_MAXIMUM];
BYTE bISRMasks[SYSINTR_MAXIMUM];   // used by DisableISR and EnableISR

#define MAX_MASK_COUNT 255
static BYTE nISRMaskCount[SYSINTR_MAXIMUM];
static ISR_DATA isrData[SYSINTR_MAXIMUM];

/**************************************************
声明：void CALLBACK ISR_Error( DWORD dwHandle )
参数：
	IN dwHandle - 中断服务例程参数，该参数为调用ISR_RegisterServer所传递的参数
  
返回值：
	无
功能描述：
	中断服务例程默认处理函数
引用: 
************************************************/
static void CALLBACK ISR_DefHandler( DWORD dwHandle )
{// a error is hapend
	dwHandle++;
}

/**************************************************
声明：void ISR_Disable( UINT nIndex )
参数：
	IN nIndex - 中断服务例程索引
  
返回值：
	无
功能描述：
	屏蔽索引指向的中断服务例程，当产生中断时,不会调用索引指向的中断服务例程，
	每调用一次该函数，该函数会增加对应的屏蔽计数器
引用: 
************************************************/

void ISR_Disable( UINT nIndex )
{
	ASSERT( nIndex < SYSINTR_MAXIMUM );
    if( nISRMaskCount[nIndex] < MAX_MASK_COUNT ) 
	    nISRMaskCount[nIndex]++;
	bISRMasks[nIndex] = 0;
}

/**************************************************
声明：void ISR_Enable( UINT nIndex )
参数：
	IN nIndex - 中断服务例程索引
  
返回值：
	无
功能描述：
	打开索引指向的中断服务例程，当产生中断时,将会调用索引指向的中断服务例程。
	该函数减少对应的屏蔽计数器，当减为0时，打开中断服务
引用: 
************************************************/
void ISR_Enable( UINT nIndex )
{
    ASSERT( nIndex < SYSINTR_MAXIMUM );
	if( nISRMaskCount[nIndex] ) 
	    nISRMaskCount[nIndex]--;
    if( nISRMaskCount[nIndex] == 0 )
    {   // 打开中断服务
	    bISRMasks[nIndex] = 1;
    }
}

/**************************************************
声明：void ISR_Active( UINT nIndex )
参数：
	IN nIndex - 中断服务例程索引
  
返回值：
	无
功能描述：
	激活索引指向的中断服务例程，例程将会在系统调用返回或当设备中断处理完返回时检查并调用ISR_Handler
引用: 
************************************************/
#define DEBUG_ISR_Active 0
void ISR_Active( UINT nIndex )
{
//	ASSERT( nIndex < SYSINTR_MAXIMUM );
	
	if( nIndex < SYSINTR_MAXIMUM )
	{   // 是否已有信号
	    if( bISRActives[nIndex] == 0 )
		{   // 否，设置信号
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
声明：void ISR_Handler( UINT idCurISR )
参数：
	IN idISR - 当前最新被激活的中断服务例程索引
  
返回值：
	无
功能描述：
	中断服务例程处理中心，该函数将会在系统调用返回或当设备中断处理完返回时检查并调用；
	当调用该函数时，中断是关掉的
引用: 
************************************************/
void ISR_Handler( UINT idISR )
{
	static int nISRHandlerCount = 0;
	int idx;	
	LPBYTE lpbMask, lpbActive;

	ISR_DATA * lpisr;


    // 防止重入
	if( nISRHandlerCount++ )
		return;
	
    lpCurThread->nLockScheCount++;


	if( idISR < SYSINTR_MAXIMUM )
	{
		if( bISRActives[idISR] && bISRMasks[idISR] )
		{   //先处理当前的idISR
			bISRActives[idISR] = 0;
			iISRActiveCount--;
			INTR_ON();
			isrData[idISR].lpISRFun( isrData[idISR].dwISRHandle );
			INTR_OFF();
		}
	}

	//if( idISR == ISR_ALL_INTRS )

	// 是否有没有处理的中断
	if( iISRActiveCount )
	{   // 有，检查它们 enum all isr		
		//iISRActiveCount = 0;
		
		lpisr = isrData;
		lpbMask = bISRMasks;
		lpbActive = bISRActives;
		for ( idx = 0; idx < SYSINTR_MAXIMUM && iISRActiveCount; idx++ ) 
		{
			if( *lpbMask && *lpbActive )
			{   // 没有屏蔽并且有信号
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
声明：BOOL ISR_RegisterServer( UINT nIndex, LPISR lpISRFun, DWORD dwISRHandle )
参数：
	IN nIndex - 中断服务例程索引
	IN lpISRFun - 中断服务例程回调函数
	IN dwISRHandle - 传递给中断服务例程回调函数的参数
  
返回值：
	假如成功，返回TRUE；否则，返回FALSE
功能描述：
	注册一个中断服务例程
引用: 
************************************************/

BOOL ISR_RegisterServer( UINT nIndex, LPISR lpISRFun, DWORD dwISRHandle )
{
	if( nIndex < SYSINTR_MAXIMUM )
	{
		UINT uiSave;
		LockIRQSave( &uiSave ); // 保存 & 关中断
		isrData[nIndex].lpISRFun = lpISRFun;
		isrData[nIndex].dwISRHandle = dwISRHandle;
		nISRMaskCount[nIndex] = 0;  // enable
		bISRMasks[nIndex] = 1;  // enable
		bISRActives[nIndex] = 0; // no active
		UnlockIRQRestore( &uiSave ); // 恢复
		return TRUE;
	}
	return FALSE;
}

/**************************************************
声明：void ISR_Unregister( UINT nIndex )
参数：
	IN nIndex - 中断服务例程索引
	IN lpISRFun - 中断服务例程回调函数
	IN dwISRHandle - 传递给中断服务例程回调函数的参数
  
返回值：
	无
功能描述：
	注销一个中断服务例程
引用: 
************************************************/

void ISR_Unregister( UINT nIndex )
{
	if( nIndex < SYSINTR_MAXIMUM )
	{
		ISR_Disable( nIndex );   // 先关掉它 disable

		bISRActives[nIndex] = 0; // no active
		isrData[nIndex].lpISRFun = ISR_DefHandler;
		isrData[nIndex].dwISRHandle = 0;
		bISRMasks[nIndex] = 0;
	}
}

/**************************************************
声明：void ISR_Init( void )
参数：
	无  
返回值：
	无
功能描述：
	中断服务例程管理器初始化
引用: 
	被start.c调用
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

