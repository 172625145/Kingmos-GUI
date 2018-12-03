/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：延迟处理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eobjtype.h>
#include <eassert.h>
#include <epcore.h>
#include <epalloc.h>

extern DWORD OEM_TimeToJiffies( DWORD dwMilliseconds, DWORD dwNanoseconds );

// ********************************************************************
// 声明：VOID WINAPI KL_Sleep( DWORD dwMilliseconds )
// 参数：
//		IN dwMilliseconds - 延迟时间（毫秒）可以为0 和 INFINITE
// 返回值：
//		无
// 功能描述：
//		延迟一段时间
// 引用：
//		系统API
// ********************************************************************
VOID WINAPI KL_Sleep( DWORD dwMilliseconds )
{
	if( dwMilliseconds == 0 )
	{	//放弃本线程的时间片		
		lpCurThread->nTickCount  = 0;
		lpCurThread->nBoost = 0;
	}


	ScheduleTimeout( dwMilliseconds );
}
	