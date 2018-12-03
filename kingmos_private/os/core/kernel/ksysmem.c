/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：内存分配管理，该功能以页为单位分配内存，实用于没有虚存的系统（有
          虚存的系统也可以调用该函数，这时，这些函数相当于VirtualAlloc & VirtualFree）
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <pagemgr.h>
#include <oemfunc.h>
#include <coresrv.h>

#include <cpu.h>
#include <epcore.h>

// ********************************************************************
//声明：LPVOID DoAllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
//参数：
// IN dwNeedSize - 需要的内存尺寸（以字节为单位）
// OUT lpdwRealSize - 用于接受真实的字节数
// IN uiFlag - 保留
//返回值：
// 假如成功，返回有效的对齐页边界的指针，lpdwRealSize保存真实的字节数；负责返回NULL
//功能描述：
// 已页为单位分配内存
//
// ********************************************************************
#define DEBUG_DOALLOCPAGEMEM 0
LPVOID DoAllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
{
	void * p = Page_GetContiguous( ALIGN_PAGE_UP( dwNeedSize ) / PAGE_SIZE, 0 );

	if( lpdwRealSize )
		*lpdwRealSize = 0;

	if( p )
	{
		if( lpdwRealSize )
		    *lpdwRealSize = ALIGN_PAGE_UP( dwNeedSize );
	}
	else
	{
		WARNMSG( DEBUG_DOALLOCPAGEMEM, ( "error: no enough memory.\r\n" ) );
		KL_SetLastError( ERROR_NOT_ENOUGH_MEMORY );
	}
	
	return p;
}

// ********************************************************************
//声明：LPVOID WINAPI KL_AllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
//参数：
// IN dwNeedSize - 需要的内存尺寸（以字节为单位）
// OUT lpdwRealSize - 用于接受真实的字节数
// IN uiFlag - 保留
//返回值：
// 假如成功，返回有效的对齐页边界的指针，lpdwRealSize保存真实的字节；负责返回NULL
//功能描述：
// 以页为单位分配内存
//引用:
//	系统API
// ********************************************************************

LPVOID WINAPI KL_AllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
{
#ifdef VIRTUAL_MEM
	void * p = 0;
	if( lpdwRealSize )
		*lpdwRealSize = 0;

	if( uiFlag & PM_SHARE )
		p = (LPVOID)SHARE_MEM_BASE;


    p =  KL_VirtualAlloc( p, dwNeedSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if( p && lpdwRealSize )
	{
		*lpdwRealSize = ALIGN_PAGE_UP( dwNeedSize );
	}
	return p;
#else
    return DoAllocPageMem( dwNeedSize, lpdwRealSize, uiFlag );
#endif
}

// ********************************************************************
//声明：BOOL DoFreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
//参数：
// IN lpvMemAdrStart - 需要释放的内存
// IN uiLen - 释放长度
//返回值：
// 假如成功，返回TRUE; 负责返回FALSE
//功能描述：
// 释放之前用DoAllocPageMem 分配的内存
//引用:
//	
// ********************************************************************

BOOL DoFreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
{
	extern BOOL Page_ReleaseContiguous( LPVOID lpAdr, DWORD dwPages );

    uiLen = ALIGN_PAGE_UP( uiLen ) / PAGE_SIZE; 
    return Page_ReleaseContiguous( lpvMemAdrStart, uiLen ); 
}

// ********************************************************************
//声明：BOOL WINAPI KL_FreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
//参数：
// IN lpvMemAdrStart - 需要释放的内存
// IN uiLen - 释放长度
//返回值：
// 假如成功，返回TRUE; 负责返回FALSE
//功能描述：
// 释放之前用 KL_AllocPageMem 分配的内存，当为VIRTUAL_MEM版时，释放所有的
//引用:
//	系统API
// ********************************************************************

BOOL WINAPI KL_FreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
{
#ifdef VIRTUAL_MEM
	return KL_VirtualFree( lpvMemAdrStart, 0, MEM_RELEASE );
#else
	return DoFreePageMem( lpvMemAdrStart, uiLen );
#endif
}
