/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ڴ��������ù�����ҳΪ��λ�����ڴ棬ʵ����û������ϵͳ����
          ����ϵͳҲ���Ե��øú�������ʱ����Щ�����൱��VirtualAlloc & VirtualFree��
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <pagemgr.h>
#include <oemfunc.h>
#include <coresrv.h>

#include <cpu.h>
#include <epcore.h>

// ********************************************************************
//������LPVOID DoAllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
//������
// IN dwNeedSize - ��Ҫ���ڴ�ߴ磨���ֽ�Ϊ��λ��
// OUT lpdwRealSize - ���ڽ�����ʵ���ֽ���
// IN uiFlag - ����
//����ֵ��
// ����ɹ���������Ч�Ķ���ҳ�߽��ָ�룬lpdwRealSize������ʵ���ֽ��������𷵻�NULL
//����������
// ��ҳΪ��λ�����ڴ�
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
//������LPVOID WINAPI KL_AllocPageMem( DWORD dwNeedSize, DWORD * lpdwRealSize, UINT uiFlag )
//������
// IN dwNeedSize - ��Ҫ���ڴ�ߴ磨���ֽ�Ϊ��λ��
// OUT lpdwRealSize - ���ڽ�����ʵ���ֽ���
// IN uiFlag - ����
//����ֵ��
// ����ɹ���������Ч�Ķ���ҳ�߽��ָ�룬lpdwRealSize������ʵ���ֽڣ����𷵻�NULL
//����������
// ��ҳΪ��λ�����ڴ�
//����:
//	ϵͳAPI
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
//������BOOL DoFreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
//������
// IN lpvMemAdrStart - ��Ҫ�ͷŵ��ڴ�
// IN uiLen - �ͷų���
//����ֵ��
// ����ɹ�������TRUE; ���𷵻�FALSE
//����������
// �ͷ�֮ǰ��DoAllocPageMem ������ڴ�
//����:
//	
// ********************************************************************

BOOL DoFreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
{
	extern BOOL Page_ReleaseContiguous( LPVOID lpAdr, DWORD dwPages );

    uiLen = ALIGN_PAGE_UP( uiLen ) / PAGE_SIZE; 
    return Page_ReleaseContiguous( lpvMemAdrStart, uiLen ); 
}

// ********************************************************************
//������BOOL WINAPI KL_FreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
//������
// IN lpvMemAdrStart - ��Ҫ�ͷŵ��ڴ�
// IN uiLen - �ͷų���
//����ֵ��
// ����ɹ�������TRUE; ���𷵻�FALSE
//����������
// �ͷ�֮ǰ�� KL_AllocPageMem ������ڴ棬��ΪVIRTUAL_MEM��ʱ���ͷ����е�
//����:
//	ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_FreePageMem( LPVOID lpvMemAdrStart, UINT uiLen  )
{
#ifdef VIRTUAL_MEM
	return KL_VirtualFree( lpvMemAdrStart, 0, MEM_RELEASE );
#else
	return DoFreePageMem( lpvMemAdrStart, uiLen );
#endif
}
