/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：Kingmos 启动（初始化）
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
    2003-05-28: 加入版权消息
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
// 声明：void CpuIdle( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		当内核初始化完成后，进入该代码进入闲置状态
// 引用：
//		
// ********************************************************************
//DWORD dwSysIdleTickCount;
static void CpuIdle( void )
{
	//DWORD dwPreTickCount;
	//BOOL bEnterIdleMode = FALSE;


    EdbgOutputDebugString( "***********************************************\r\n" );
	EdbgOutputDebugString( "*                                             *\r\n" );
	EdbgOutputDebugString( "*              巨果.Kingmos(R)                *\r\n" );
	EdbgOutputDebugString( "*           Version:%d.%d.%d                  *\r\n", MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION );
	EdbgOutputDebugString( "*                                             *\r\n" );
	EdbgOutputDebugString( "* Copyright(c) 1998-2004 微逻辑(WEILUOJI)     *\r\n" );
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
  //      Win32_Sleep(1);  // 给MS Window 系统控制权去处理别的事情，否则，MS Window会性能大大下降
		//Win32_SwitchToThread();
		//Win32_GetTickCount();
//#endif

        lpCurThread->nTickCount = 0;

#ifndef TIMER_RESCHE
		// 如果系统是不用时间片来执行自动调度过程，这里必须加入相关代码
		// 去调度，否则，系统永远就在这里了
        if( bNeedResched )	// 是否有调度请求 ？
        { // 有，执行调度
            Schedule();
			//dwPreTickCount = KL_GetTickCount();
        }
#endif
		// 判断是否有信号 ？如果有并且没有屏蔽，则执行相关信号处理
	    if( lpCurThread->dwSignal & ~lpCurThread->dwBlocked )
		{	// 是
		    HandleSignal();
			Schedule();
			//dwPreTickCount = dwCurTickCount = KL_GetTickCount();
		}
		else
		{	// 否，检查是否进入闲置模式 
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
// 声明：DWORD KingmosStart( LPVOID lParam )
// 参数：
//		IN lParam - 保留 
// 返回值：
//		0
// 功能描述：
//		当CPU初始化完以后，进入该代码初始化Kingmos系统
// 引用：
//		
// ********************************************************************
DWORD KingmosStart( LPVOID lParam )
{

#ifdef EML_WIN32
	INTR_OFF();	// 不可能中断
#endif

	if( InitialKingmosSystem( 0 ) == FALSE )	// 初始化系统
		return FALSE;

#ifdef EML_WIN32 
	INTR_ON();// 打开中断，enable intr
#endif

	CpuIdle();
    return 0;
}
