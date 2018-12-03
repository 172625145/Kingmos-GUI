/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ں˶ѹ���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
//#include <coreheap.h>
#include <epalloc.h>
#include <epcore.h>
#include <coresrv.h>

//�ں˶�̬��
static HEAP heapKernel;

/**************************************************
������void * _kalloc( size_t sizeneeded )
������
	sizeneeded - ��Ҫ������ڴ��С  
����ֵ��
	����ɹ���������Ч��ָ�룻���򣬷���NULL
����������
	��ϵͳ�ѷ����ڴ�
����: 
	
************************************************/
void * _kalloc( size_t sizeneeded )
{
	void * lpv	= KL_HeapAlloc( &heapKernel, 0, sizeneeded );
	return lpv;
}

/**************************************************
������void _kfree( void FAR * lpvUsed )
������
	lpvUsed - ֮ǰ��_kalloc �� _krealloc ������ڴ�ָ��  
����ֵ��
	��
����������
	�ͷ�֮ǰ��_kalloc �� _krealloc������ڴ�
����: 
	
************************************************/

void _kfree( void FAR * lpvUsed )
{
	KL_HeapFree( &heapKernel, 0, lpvUsed );
}

/**************************************************
������void * _krealloc( void *p, size_t dwResize )
������
	p - ֮ǰ��_kalloc �� _krealloc ������ڴ�ָ�� �� NULL(��ʱ��ͬ��_kalloc)
	dwResize - ���·�����ڴ��С
����ֵ��
	����ɹ���������Ч��ָ�룻���򣬷���NULL

����������
	���·���֮ǰ��_kalloc �� _krealloc������ڴ�
����: 
	
************************************************/
void * _krealloc( void *p, size_t dwResize )
{
    void * lpv	= KL_HeapReAlloc( &heapKernel, 0, p, dwResize );
	return lpv;
}

/**************************************************
������BOOL _InitKernelHeap( void )
������
	��
����ֵ��
	����ɹ�������TRUE�����򣬷���FALSE

����������
	�ں˶ѳ�ʼ��	
����: 
	ϵͳ����ʱ��system.c����
************************************************/

BOOL _InitKernelHeap( void )
{
	extern BOOL Heap_Init( LPHEAP lpHeap, DWORD dwOption, ULONG ulInitialSize, ULONG ulLimit );
	return Heap_Init( &heapKernel, 0, 0, 0 );
}
