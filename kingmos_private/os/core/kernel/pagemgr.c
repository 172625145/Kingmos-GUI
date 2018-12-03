/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����ҳ����ģ�飬��ģ�齫ϵͳ���п����ڴ���ҳΪ��λ��֯������
          ����һ����������ڴ����ģ�飬���������ڴ���亯�����Ӹ�
		  ģ��õ���ҳΪ��λ���ڴ�


�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
******************************************************/


#include <eframe.h>
#include <epcore.h>
#include <pagemgr.h>
#include <oemfunc.h>
#include <cpu.h>

#define FLAG_USED 1
#define FLAG_FREE 0
#define MAX_USED_COUNT 0xff


LPBYTE  lpbSysMainMem;   // ϵͳ���ڴ濪ʼλ��
LPBYTE  lpbSysMainMemEnd; // ϵͳ���ڴ����λ��
DWORD   dwHandleBase;  // ������ڴ����λ��
ULONG   ulSysMainMemLength; //ϵͳ���ڴ��С

#ifdef INLINE_PROGRAM
//extern UINT _uiHeapSize = 0;
#endif

// �ɵ�ַ�͸õ�ַ���ڵĶεõ��õ�ַ��Ӧ��������
#define GET_INDEX( lpSegInfo, lpvMemAdr ) ( ( (LPBYTE)(lpvMemAdr) - (lpSegInfo)->lpbFreePageStart ) / PAGE_SIZE )

// �ڴ�������Ϣ
static MEMINFO memInfo;
// ����ҳ����
static LPPAGELIST lpFreePageList = NULL;
// ����ҳ��
static UINT    uiFreePages = 0;

// ********************************************************************
//������void Page_LinkToList( LPPAGELIST lpList ) 
//������
//	IN lpList - ��PAGELIST�ṹָ���ʾ�Ŀ���ҳ
//����ֵ��
//	��
//����������
//	��һ������ҳ�����������
//���ã�
// ********************************************************************
static void Page_LinkToList( LPPAGELIST lpList ) 
{
	lpList->lpNext = lpFreePageList;
	lpList->lpPrev = NULL;
	if( lpFreePageList )
		lpFreePageList->lpPrev = lpList;
	lpFreePageList = lpList;
}

// ********************************************************************
//������void Page_UnlinkFromList( LPPAGELIST lpList )
//������
//	IN lpList - ��PAGELIST�ṹָ���ʾ�Ŀ���ҳ
//����ֵ��
//	��
//����������
//	��һ������ҳ�Ƴ���������
//���ã�
// ********************************************************************

static void Page_UnlinkFromList( LPPAGELIST lpList )
{
	if( lpList->lpNext )
	{
		ASSERT( lpList->lpNext->lpPrev == lpList );
		lpList->lpNext->lpPrev = lpList->lpPrev;
	}
	if( lpList->lpPrev )
	{
		ASSERT( lpList->lpPrev->lpNext == lpList );
		lpList->lpPrev->lpNext = lpList->lpNext;
	}
	if( lpFreePageList == lpList )
		lpFreePageList = lpList->lpNext;
	lpList->lpNext = lpList->lpPrev = NULL;
}

// ********************************************************************
//������void Page_AddToFreeList( LPPAGELIST lpList )
//������
//	IN lpList - ��PAGELIST�ṹָ���ʾ��ҳ
//����ֵ��
// ��
//����������
//	��һ��ҳ��������������ӿ���ҳ����
//���ã�
// ********************************************************************

static void Page_AddToFreeList( LPPAGELIST lpList )
{
	Page_LinkToList( lpList );
    uiFreePages++;
}

// ********************************************************************
//������void * Page_RemoveFromFreeList( BOOL bUpdateFreePages )
//������
//  IN bUpdateFreePages - �Ƿ���¿���ҳ����

//����ֵ��
// ��
//����������
//	��һ��ҳ�Ƴ���������������Ҫ���ٿ���ҳ�������ڵ��øú���֮ǰ������ס�ж�!!
//���ã�
// ********************************************************************

static void * Page_RemoveFromFreeList( BOOL bUpdateFreePages )
{
	LPPAGELIST lpList = lpFreePageList;
	Page_UnlinkFromList( lpFreePageList );//lpList );
	if( bUpdateFreePages )
	    uiFreePages--;
	return lpList;
}

// ********************************************************************
//������LPSEGMENTINFO Page_GetSeg( void * lpvMemAdr  )
//������
//	IN lpvMemAdr - �ڴ��ַ
//����ֵ��
//	����ɹ������ظõ�ַ���ڵ��ڴ����Ϣ������NULL
//����������
//	�õ�һ����ַ���ڵ��ڴ����Ϣ��һ���ڴ����һ���������ܹ���ȡ���ڴ�ռ�
//���ã�
// ********************************************************************

static LPSEGMENTINFO Page_GetSeg( void * lpvMemAdr  )
{
	LPSEGMENTINFO lpSegInfo = memInfo.lpSectionInfo;
	DWORD dwSections = memInfo.dwSections;

	for( ; dwSections; dwSections-- )
	{
		if( (LPBYTE)lpvMemAdr >= lpSegInfo->lpbFreePageStart &&
			(LPBYTE)lpvMemAdr < lpSegInfo->lpbFreePageEnd )
		{
			return lpSegInfo;
		}
		lpSegInfo++;
	}
	return NULL;
}

// ********************************************************************
//������BOOL Page_Duplicate( void * lpvMemAdr )
//������
//	IN lpvMemAdr - �ڴ��ַ
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	���ӶԸ��ڴ�����ü���
//���ã�
// ********************************************************************

BOOL Page_Duplicate( void * lpvMemAdr )
{
	LPSEGMENTINFO lpSegInfo;
	UINT uiSave;
	int index;

	lpvMemAdr = (LPVOID)CACHE_TO_UNCACHE( lpvMemAdr );

	lpSegInfo = Page_GetSeg( lpvMemAdr );
//	ASSERT( lpSegInfo );

	LockIRQSave( &uiSave );

	index = GET_INDEX( lpSegInfo, lpvMemAdr );
	ASSERT( lpSegInfo->lpbFreeMemMap[index] != FLAG_FREE );
	if( lpSegInfo->lpbFreeMemMap[index] < MAX_USED_COUNT ) 
	    lpSegInfo->lpbFreeMemMap[index]++;

	UnlockIRQRestore( &uiSave );

	return TRUE;
}

// ********************************************************************
//������BOOL Page_GetSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
//������
//	IN lpSegInfo - �ڴ��ָ��
//	IN dwIndex - ҳ������
//����ֵ��
//	����ɹ�������TRUE������FALSE
//����������
//	����һ���ض���ҳ�ǿ��еģ�������Ϊռ��״̬
//���ã�
// ********************************************************************

static BOOL Page_GetSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
{
	UINT uiSave;

    LockIRQSave( &uiSave );

	ASSERT( dwIndex < lpSegInfo->dwTotalPages );
	if( lpSegInfo->lpbMemStart[dwIndex] == FLAG_FREE )
	{	//���е�
		lpSegInfo->lpbMemStart[dwIndex] = FLAG_USED;
		Page_UnlinkFromList( (LPPAGELIST)( lpSegInfo->lpbFreePageStart + dwIndex * PAGE_SIZE ) );
		
		UnlockIRQRestore( &uiSave );
		return TRUE;
	}

    UnlockIRQRestore( &uiSave );
	return FALSE;	
}

// ********************************************************************
//������BOOL Page_ReleaseSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
//������
//	IN lpSegInfo - �ڴ��ָ��
//	IN dwIndex - ҳ������
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	��Page_GetSpecific���Ӧ������һ���ض���ҳ��ռ��״̬��������Ϊ����״̬
//���ã�
// ********************************************************************

static BOOL Page_ReleaseSpecific( LPSEGMENTINFO lpSegInfo, DWORD dwIndex )
{
	UINT uiSave;

    LockIRQSave( &uiSave );

	ASSERT( dwIndex < lpSegInfo->dwTotalPages );
	ASSERT( lpSegInfo->lpbMemStart[dwIndex] == FLAG_USED );

	if( lpSegInfo->lpbMemStart[dwIndex] == FLAG_USED )
	{	// ���ӵ�����
		lpSegInfo->lpbMemStart[dwIndex] = FLAG_FREE;
		Page_LinkToList( (LPPAGELIST)( lpSegInfo->lpbFreePageStart + dwIndex * PAGE_SIZE ) );
	}

	UnlockIRQRestore( &uiSave );
	return TRUE;
}

// ********************************************************************
//������UINT Page_CountFreePages( void )
//������
//	��
//����ֵ��
//	���ؿ���ҳ��
//����������
//	�õ�ϵͳ����ҳ��
//���ã�
// ********************************************************************

UINT Page_CountFreePages( void )
{
	return uiFreePages;
}

// ********************************************************************
//������BOOL Page_Lock( DWORD dwPages )
//������
// IN dwPages - ҳ��
//
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	��ס��Ӧ��ҳ��
//���ã�
// ********************************************************************

BOOL Page_Lock( DWORD dwPages )
{
	BOOL bRetv;
	UINT uiSave;

    LockIRQSave( &uiSave );

	if( uiFreePages >= dwPages )
	{	//	�������ҳ������Ҫ��ҳ�������ȥ��Ҫ��ҳ��
		uiFreePages -= dwPages;
		bRetv = TRUE;
	}
	else
		bRetv = FALSE;


	UnlockIRQRestore( &uiSave );
	return bRetv;
}

// ********************************************************************
//������BOOL Page_Unlock( DWORD dwPages )
//������
//	IN dwPages - ҳ�� 
//
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	��Page_Lock���Ӧ��������Ӧ��ҳ��
//���ã�
//
// ********************************************************************

BOOL Page_Unlock( DWORD dwPages )
{
	UINT uiSave;

    LockIRQSave( &uiSave );

    uiFreePages += dwPages;

    UnlockIRQRestore( &uiSave );

	return TRUE;
}

// ********************************************************************
//������void * Page_Alloc( BOOL bUpdateFreePages )
//������
//	IN bUpdateFreePages - �Ƿ���¿���ҳ����,���ΪTRUE,��Ҫ��� uiFreePages �Ƿ� > 0; ���ΪFALSE, �򲻼�飨֮ǰ��Page_Lock�Ѿ�������uiFreePages��
//
//����ֵ��
//	����ɹ���������Ч�ڴ�ҳ��ַ�����򷵻�NULL
//����������
//	����һ�ڴ�ҳ
//���ã�
// ********************************************************************

void * Page_Alloc( BOOL bUpdateFreePages )
{
    void * lp = NULL;
	UINT uiSave;

    LockIRQSave( &uiSave );

	// ����ҳ���Ƿ�Ϊ�գ�
	if( (bUpdateFreePages && uiFreePages) || 
		(bUpdateFreePages == FALSE && lpFreePageList) )
	//if( lpFreePageList )
	{   // ��Ϊ��
	    LPSEGMENTINFO lpSegInfo;

		//lp = lpFreePageList;
		lp = Page_RemoveFromFreeList( bUpdateFreePages );
		//	�õ���ҳ���ڵ��ڴ��
		lpSegInfo = Page_GetSeg( lp );
//		ASSERT( lpSegInfo );
		//	����ҳ���ڴ����Ӧ��λ��Ϊռ��״̬
		lpSegInfo->lpbFreeMemMap[GET_INDEX( lpSegInfo, lp )] = FLAG_USED;
	}
	UnlockIRQRestore( &uiSave );
	//	�����CACHE���ܣ�����ת��ΪCACHEָ��
	return lp ? (void*)UNCACHE_TO_CACHE( lp ) : NULL;
}

// ********************************************************************
//������BOOL Page_Free( LPVOID lpvMemAdr  )
//������
// IN lpvMemAdr - ��Ҫ�ͷŵ�ҳ
//
//����ֵ��
// ����ɹ�������TRUE�����򷵻�FALSE
//����������
//	��Page_Alloc���Ӧ���ͷ�һ��ҳ�ڴ档
//���ã�
// ********************************************************************
#define DEBUG_Page_Free 0
BOOL Page_Free( LPVOID lpvMemAdr  )
{
	LPSEGMENTINFO lpSegInfo;

//	ASSERT( lpvMemAdr );
	//	�����CACHE���ܣ�����ת��ΪUNCACHEָ��
	lpvMemAdr = (LPVOID)CACHE_TO_UNCACHE( lpvMemAdr );
	//	�õ����ڴ����ڵ�ҳ
	lpSegInfo = Page_GetSeg( lpvMemAdr );

	if( lpSegInfo )
	{
	    UINT uiSave;
		int index;
		// �õ���ҳ���ڴ�ε�������
        index = GET_INDEX( lpSegInfo, lpvMemAdr );
		LockIRQSave( &uiSave );

		//ASSERT( lpSegInfo->lpbFreeMemMap[index] );
		// ���ڴ�ҳ�����ü����Ƿ�Ϊ0
		if( lpSegInfo->lpbFreeMemMap[index] )
		{
			if( --lpSegInfo->lpbFreeMemMap[index] == 0 )
				Page_AddToFreeList( lpvMemAdr );  //�ǣ�������뵽��������
		}
		else
		{
			ERRORMSG( DEBUG_Page_Free, ( "error in Page_Free: lpvMemAdr=0x%x.\r\n", lpvMemAdr ) );
		}

		UnlockIRQRestore( &uiSave );
	}
	else
	{	// ��һ���Ǵ��п����Ǵ���ҳ��Ҳͨ���ú���
		;//RETAILMSG( 1, ( "failure error at Page_Free(0x%x): lpSegInfo = NULL!.\r\n", lpvMemAdr ) );
	}

	return TRUE;
}

// ********************************************************************
//������BOOL Page_Get( LPVOID * lppAdr, DWORD dwPages )
//������
//	IN lppAdr - ָ�����ڽ���ҳ��ַ��ָ�����飬������Ĵ�СӦ�õ���dwPages -> lppAdr[dwPages]
//	IN dwPages - ��Ҫ��ҳ��
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	�õ�һЩҳ�ڴ�
//���ã�
// ********************************************************************

BOOL Page_Get( LPVOID * lppAdr, DWORD dwPages )
{
	// ����ס��Ҫ��ҳ
	if( Page_Lock( dwPages ) )
	{	//��ס�ɹ�
		while( dwPages-- )
		{	//�õ�ÿ��ҳ
			*lppAdr++ = Page_Alloc( FALSE ); 
		}
		return TRUE;
	}    
	return FALSE;
}

// ********************************************************************
//������BOOL Page_Release( LPVOID * lppAdr, DWORD dwPages )
//������
//	IN lppAdr - ָ�����ڱ�����ҳ��ַ��ָ�����飬������Ĵ�СӦ�õ���dwPages -> lppAdr[dwPages]
//	IN dwPages - ��Ҫ�ͷŵ�ҳ��
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	��Page_Get��Ӧ���ͷ�һЩҳ�ڴ�
//���ã�
// ********************************************************************
BOOL Page_Release( LPVOID * lppAdr, DWORD dwPages )
{
	while( dwPages-- )
	{
		//ASSERT( *lppAdr );
		Page_Free( *lppAdr );
		lppAdr++;
	}
	return TRUE;
}

// ********************************************************************
//������LPVOID Page_GetContiguous( DWORD dwPages, DWORD dwAlignmentMask )
//������
//	IN dwPages - ��Ҫ�����ҳ��
//	IN dwAlignmentMask - ҳ��ʼ��ַ����mask
//����ֵ��
//	����ɹ������ط�NULL���ڴ��ַ�����򷵻�NULL
//����������
//	�õ�������ҳ�ڴ�
//���ã�
// ********************************************************************
LPVOID Page_GetContiguous( DWORD dwPages, DWORD dwAlignmentMask )
{
	LPVOID lpvRet = NULL;
	// ��ס��Ҫ��ҳ
	if( Page_Lock( dwPages ) )
	{	// 
		LPSEGMENTINFO lpSegInfo = memInfo.lpSectionInfo;
		LPSEGMENTINFO lpSegInfoEnd = lpSegInfo + memInfo.dwSections;
        // �������е��ڴ�Σ��õ�һ���������ڴ�
		for( ; lpSegInfo < lpSegInfoEnd; lpSegInfo++ )
		{
			LPBYTE lpbMemMapS = lpSegInfo->lpbFreeMemMap;
			LPBYTE lpbMemMapE = lpbMemMapS + lpSegInfo->dwTotalPages - dwPages;
			LPBYTE lpbPageStart = lpSegInfo->lpbFreePageStart;
			DWORD dwIndexBegin = 0;
			// ���ҵ�ǰ�ڴ��
			for( ; lpbMemMapS <= lpbMemMapE; lpbMemMapS++, dwIndexBegin++, lpbPageStart += PAGE_SIZE )
			{	// ��ҳ�Ƿ��ǿ��еĻ��Ƿ�����ַ
				if( *lpbMemMapS ||
					( (DWORD)lpbPageStart & dwAlignmentMask )
					)
				{	// ռ�û�û�ж���
					continue;
				}
				else
				{	// �����������Ӹ�ҳ���±���������Ҫ���ڴ�ҳ
					LPBYTE lpbFirst = lpbMemMapS;
					LPBYTE lpbEnd = lpbFirst + dwPages;
					DWORD dwCurIndex = dwIndexBegin;
					for( ; lpbFirst < lpbEnd; lpbFirst++, dwCurIndex++ )
					{	// �Ƿ��ǿ��� �� ����ɹ���
						if( *lpbFirst ||
							Page_GetSpecific( lpSegInfo, dwCurIndex ) == FALSE )
						{   // ���ɹ����ͷ�֮ǰ�ѷ�����ڴ�ҳ 							
							DWORD n = dwIndexBegin;
							for( ; n < dwCurIndex; n++ )
							{
								Page_ReleaseSpecific( lpSegInfo, n );
							}
							// �����趨������ʼλ��
							lpbMemMapS = lpbFirst + 1;
							dwIndexBegin = dwCurIndex + 1;
							lpbPageStart = lpSegInfo->lpbFreePageStart + dwIndexBegin * PAGE_SIZE;
							break;
						}
					}
					if( lpbFirst == lpbEnd )
					{   // �Ѿ��õ�������Ҫ���ڴ�ҳ //i get all page needed
						lpvRet = lpbPageStart;
						goto _RET;
					}
				}
			}
			// ���ɹ�������
			Page_Unlock( dwPages );
		}
	}
_RET:
	//	�����CACHE���ܣ�����ת��ΪCACHEָ��

	return lpvRet ? (LPVOID)UNCACHE_TO_CACHE(lpvRet) : NULL;
}

// ********************************************************************
//������BOOL Page_ReleaseContiguous( LPVOID lpAdr, DWORD dwPages )
//������
//	IN lpAdr - ��Ҫ�ͷŵ�ҳ��ʼ��ַ
//	IN dwPages - ��Ҫ�ͷŵ�ҳ��
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	��Page_GetContiguous��Ӧ���ͷ�������ҳ�ڴ�
//���ã�
// ********************************************************************
BOOL Page_ReleaseContiguous( LPVOID lpAdr, DWORD dwPages )
{
	for( ; dwPages; dwPages-- )
	{
		//ASSERT( lpAdr );
		Page_Free( lpAdr );
		//	��һҳ��ַ
		lpAdr = (LPVOID)( (DWORD)lpAdr + PAGE_SIZE ); 
	}
	return TRUE;
}

// ********************************************************************
//������BOOL Page_InitSysPages( void )
//������
//	��
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	���ں�����ʱ����ʼ��ϵͳ�ڴ�ҳ���ڵ��øú���֮ǰlpbSysMainMem��ulSysMainMemLength�����ȳ�ʼ��
//���ã�
// ********************************************************************
BOOL Page_InitSysPages( void )
{
	extern void OEM_WriteDebugString( LPCTSTR lpszStr );

    int k, iSegNum;	
	MEM_SEGMENT seg[MAX_SEGMENTS];
	char buf[16];

	// ���������Ϣ - begin
	OEM_WriteDebugString( "Page_InitSysPages:lpbSysMainMem=0x" );
	itoa( (DWORD)lpbSysMainMem, buf, 16 );
	OEM_WriteDebugString( buf );
	OEM_WriteDebugString( ",ulSysMainMemLength=" );
	itoa( (DWORD)ulSysMainMemLength, buf, 10 );
	OEM_WriteDebugString( buf );
	OEM_WriteDebugString( "\r\n" );
	// - end
	
	//	�����MMU,��ˢ��CACHE
#ifdef VIRTUAL_MEM
	FlushCacheAndClearTLB();
#endif
	// ��ʼ�����������ַ
    dwHandleBase = (DWORD)lpbSysMainMem & 0xE0000000;
	lpbSysMainMemEnd = lpbSysMainMem + ulSysMainMemLength;
	//��ʼ��memInfo ��Ϣ
	memInfo.lpbMainMemStart = (LPBYTE)CACHE_TO_UNCACHE( lpbSysMainMem );
	memInfo.lpbMainMemEnd = memInfo.lpbMainMemStart + ulSysMainMemLength;
	memInfo.lpbMainMemFreeStart = memInfo.lpbMainMemStart;
	memInfo.dwMainMemFreeSize = ulSysMainMemLength;
	//������һ���ڴ�νṹ�������ڴ棩
	memInfo.lpSectionInfo = (LPSEGMENTINFO)ALIGN_DWORD( (DWORD)memInfo.lpbMainMemFreeStart );
	//��ʼ����һ���ڴ��
	seg[0].lpbStart = (LPBYTE)( memInfo.lpSectionInfo + MAX_SEGMENTS );
	seg[0].dwSize = memInfo.dwMainMemFreeSize - ( seg[0].lpbStart - memInfo.lpbMainMemFreeStart );
	//�õ�ϵͳ���������ڴ����
	iSegNum = OEM_EnumExtensionDRAM( seg+1, MAX_SEGMENTS-1 );
	iSegNum += 1;  // �ܵ��ڴ����
	//��ʼ��ÿһ���ڴ��
	for( k = 0; k < iSegNum; k++ )
	{
		LPBYTE lpbPageStart, lpbPageEnd;
		DWORD dwPages;

		memInfo.lpSectionInfo[k].lpbMemStart = seg[0].lpbStart;
		memInfo.lpSectionInfo[k].dwMemSize = seg[0].dwSize;
		memInfo.lpSectionInfo[k].lpbFreeMemMap = memInfo.lpSectionInfo[k].lpbMemStart;//
        // �ö�ҳ��ʼλ��
		lpbPageStart = (LPBYTE)ALIGN_PAGE_UP( (DWORD)memInfo.lpSectionInfo[k].lpbMemStart );
		// �ö�ҳ����λ��
		lpbPageEnd = (LPBYTE)ALIGN_PAGE_DOWN( (DWORD)(memInfo.lpSectionInfo[k].lpbMemStart + memInfo.lpSectionInfo[k].dwMemSize) );//.lpbMemStart + 
		// �ö�ҳ��
		dwPages = (lpbPageEnd - lpbPageStart) / PAGE_SIZE;
		// �öο���ҳ��ʼλ��
		memInfo.lpSectionInfo[k].lpbFreePageStart =  (LPBYTE)ALIGN_PAGE_UP( (DWORD)(memInfo.lpSectionInfo[k].lpbFreeMemMap + dwPages) );
        // �öο���ҳ����λ��
		memInfo.lpSectionInfo[k].lpbFreePageEnd = lpbPageEnd; 
		// �ö��ܵĿ���ҳ��
	    memInfo.lpSectionInfo[k].dwTotalPages = (memInfo.lpSectionInfo[k].lpbFreePageEnd - memInfo.lpSectionInfo[k].lpbFreePageStart) / PAGE_SIZE;
		// ��ҳ������Ϊ����״̬
		memset( memInfo.lpSectionInfo[k].lpbFreeMemMap, 0, memInfo.lpSectionInfo[k].dwTotalPages );
	}

	memInfo.dwSections = iSegNum;

    // �������ڴ�εĿ���ҳ�����������now , add free pages to pagelist	
	for( k = 0; k < iSegNum ; k++ )
	{
		LPBYTE lpbPageStart = memInfo.lpSectionInfo[k].lpbFreePageStart;
		LPBYTE lpbPageEnd = memInfo.lpSectionInfo[k].lpbFreePageEnd;
		while( lpbPageStart < lpbPageEnd )
		{
		    Page_AddToFreeList( (LPPAGELIST)lpbPageStart );
		    lpbPageStart += PAGE_SIZE;
		}
	}
	//
	return TRUE;
}

// ********************************************************************
//������BOOL _InitSysMem( void )
//������
//	��
//����ֵ��
//	����ɹ�������TRUE�����򷵻�FALSE
//����������
//	���ں�����ʱ����ʼ��ϵͳ�ڴ�ҳ���ڵ��øú���֮ǰlpbSysMainMem��ulSysMainMemLength�����ȳ�ʼ��
//���ã�
// ********************************************************************
BOOL _InitSysMem( void )
{
    return Page_InitSysPages();
}
