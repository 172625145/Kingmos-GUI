/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：内核堆管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
//#include <coreheap.h>
#include <epalloc.h>
#include <epcore.h>
#include <coresrv.h>

//内核动态堆
static HEAP heapKernel;

/**************************************************
声明：void * _kalloc( size_t sizeneeded )
参数：
	sizeneeded - 需要分配的内存大小  
返回值：
	假如成功，返回有效的指针；否则，返回NULL
功能描述：
	从系统堆分配内存
引用: 
	
************************************************/
void * _kalloc( size_t sizeneeded )
{
	void * lpv	= KL_HeapAlloc( &heapKernel, 0, sizeneeded );
	return lpv;
}

/**************************************************
声明：void _kfree( void FAR * lpvUsed )
参数：
	lpvUsed - 之前用_kalloc 或 _krealloc 分配的内存指针  
返回值：
	无
功能描述：
	释放之前用_kalloc 或 _krealloc分配的内存
引用: 
	
************************************************/

void _kfree( void FAR * lpvUsed )
{
	KL_HeapFree( &heapKernel, 0, lpvUsed );
}

/**************************************************
声明：void * _krealloc( void *p, size_t dwResize )
参数：
	p - 之前用_kalloc 或 _krealloc 分配的内存指针 或 NULL(这时等同于_kalloc)
	dwResize - 重新分配的内存大小
返回值：
	假如成功，返回有效的指针；否则，返回NULL

功能描述：
	重新分配之前用_kalloc 或 _krealloc分配的内存
引用: 
	
************************************************/
void * _krealloc( void *p, size_t dwResize )
{
    void * lpv	= KL_HeapReAlloc( &heapKernel, 0, p, dwResize );
	return lpv;
}

/**************************************************
声明：BOOL _InitKernelHeap( void )
参数：
	无
返回值：
	假如成功，返回TRUE；否则，返回FALSE

功能描述：
	内核堆初始化	
引用: 
	系统加载时被system.c调用
************************************************/

BOOL _InitKernelHeap( void )
{
	extern BOOL Heap_Init( LPHEAP lpHeap, DWORD dwOption, ULONG ulInitialSize, ULONG ulLimit );
	return Heap_Init( &heapKernel, 0, 0, 0 );
}
