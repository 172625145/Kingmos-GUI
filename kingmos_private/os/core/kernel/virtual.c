/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵���������ڴ����
�汾�ţ�2.0.0
����ʱ�ڣ�2002
���ߣ�����
�޸ļ�¼��
	2005-01-18 : ���Ӷ� �ں˽��̵����ַ�εĴ�������ȥ�� dwSegIndex < MAX_SEGMENTS ��
******************************************************/

#include <eframe.h>
#include <cpu.h>
#include <pagemgr.h>
#include <coresrv.h>
#include <virtual.h>
#include <epalloc.h>
#include <epcore.h>

//  The user mode virtual address space is 2GB split into 64 32M sections
//  of 512 64K blocks of 16 4K pages.
//  2GB�����ַ�ռ䱻����Ϊ 64����,ÿ���Σ�32M��������Ϊ512���飬ÿ���飨64K ��������Ϊ16��ҳ(ÿҳ4K)
//  Virtual address format:
//  3322222 222221111 1111 110000000000
//  1098765 432109876 5432 109876543210
//  zSSSSSS BBBBBBBBB PPPP oooooooooooo


//static LPSEGMENT KernelSegments[MAX_SEGMENTS];

#define PAGE_IN_RANGE( page, minPage, maxPage ) ( (page) >= minPage && (page) < maxPage )

static CRITICAL_SECTION csVirtualMem;

BOOL Seg_DecommitPages( LPPROCESS_SEGMENTS lpProcessSeg, LPSEGMENT lpSeg, int idxStartBlk, int idxStartPages, int iPages );
static LPPROCESS_SEGMENTS Seg_FindProcessSegment( DWORD dwAddress );

// ********************************************************************
// ������BOOL InitialVirtualMgr( void )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		��ʼ�������ڴ������
// ���ã�
//		system.c 
// ********************************************************************
BOOL InitialVirtualMgr( void )
{
	KC_InitializeCriticalSection( &csVirtualMem );
	return TRUE;
}

// ********************************************************************
// ������BOOL IsPageInCache( LPPROCESS_SEGMENTS lpProcessSeg, 
//							 UINT uiPageIndex )
// ������
//		IN lpProcessSeg - ���̶���
//		IN uiPageIndex - ҳ�ڶ��ڵ�����
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ж�һ��ҳ�Ƿ��� ҳ����
// ���ã�
//		system.c 
// ********************************************************************
/*
static BOOL IsPageInCache( LPPROCESS_SEGMENTS lpProcessSeg, UINT uiPageIndex )
{
	if( uiPageIndex > lpProcessSeg->uiPageTableStart && 
		uiPageIndex < lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA )
		return TRUE;
	return FALSE;
}
*/

// ********************************************************************
// ������BOOL GetPagesInCacheRange( LPPROCESS_SEGMENTS lpProcessSeg,
//							        UINT * lpuiPageIndexStart, UINT * lpuiPageIndexEnd )
// ������
//		IN lpProcessSeg - ���̶���
//		IN/OUT lpuiPageIndexStart - ҳ�ڶ��ڵ�����
//		IN/OUT lpuiPageIndexStart - ҳ�ڶ��ڵ�����
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ж�һ��ҳ�Ƿ��� ҳ����
// ���ã�
//		system.c 
// ********************************************************************
static BOOL GetPagesInCacheRange( LPPROCESS_SEGMENTS lpProcessSeg,
					             UINT * lpuiPageIndexStart, 
								 UINT * lpuiPageIndexEnd )
{
	UINT uStartCachePage, uEndCachePage, minPage, maxPage;

	if( lpProcessSeg ) //&& lpProcessSeg->lpdwSecondPageTable )
	{
		uStartCachePage = lpProcessSeg->uiPageTableStart;
		uEndCachePage = uStartCachePage + lpProcessSeg->uiPageTableCount * PAGES_PER_MEGA;
	
		//ASSERT( lpProcessSeg->lpdwSecondPageTable );
		
		minPage = MAX( uStartCachePage, *lpuiPageIndexStart );
		maxPage = MIN( uEndCachePage, *lpuiPageIndexEnd );
		if( minPage < maxPage )
		{
			*lpuiPageIndexStart = minPage;
			*lpuiPageIndexEnd = maxPage;
			return TRUE;
		}
	}
	return FALSE;
}


// ********************************************************************
// ������LPSEGMENT Seg_Alloc( void )
// ������
//		��
// ����ֵ��
//		��ָ��
// ����������
//		����һ���νṹָ��
// ���ã�
//		
// ********************************************************************
LPSEGMENT Seg_Alloc( void )
{
	LPSEGMENT lpSeg = (LPSEGMENT)KHeap_Alloc( sizeof( SEGMENT ) );
	
	if( lpSeg )
	{
		memset( lpSeg, 0, sizeof( SEGMENT ) );
	}
	return lpSeg;
}

// ********************************************************************
// ������void Seg_Free ( LPSEGMENT lpSeg ) 
// ������
//		IN  lpSeg - �νṹָ��
// ����ֵ��
//		��
// ����������
//		�ͷŶνṹָ�뼰����ռ�õ�����ҳ
// ���ã�
//		
// ********************************************************************
void Seg_Free ( LPSEGMENT lpSeg ) 
{
	LPMEMBLOCK * lppBlks;
	int i;
	BOOL bFlush = 0;

   // ASSERT( (DWORD)lpSeg != dwProcessSlots[0] );
	FlushCacheAndClearTLB();  // ��ˢ��CACHE

	lppBlks = lpSeg->lpBlks;
	// ����ÿһ���飬�����δ�ͷŵ�����ҳ�����ͷ�����
	for( i = 0; i < BLOCKS_PER_SEGMENT; i++, lppBlks++ )
	{
		if( *lppBlks > RESERVED_BLOCK ) // �Ƿ��п�ṹ ��
		{   // ��
//			ASSERT( i != 0 );
			// �ͷŸöθÿ������ҳ
			bFlush |= Seg_DecommitPages( NULL, lpSeg, i, 0, PAGES_PER_BLOCK );
			// �ͷŸÿ�
			KHeap_Free( *lppBlks, sizeof(MEMBLOCK) );
		}
	}
	// �ͷŶνṹ
	KHeap_Free( lpSeg, sizeof(SEGMENT) );
	if( bFlush )  // �ͷ��꣬ˢ��CACHE
	    FlushCacheAndClearTLB();
}

// ********************************************************************
// ������static int Seg_SearchFirstBlock( LPSEGMENT lpSeg, int index )
// ������
//		IN lpSeg - �νṹָ��
//		IN index - �������������е�һ�����ڶ��ڵ����� 
// ����ֵ��
//		����ɹ������ؿ�����ֵ�����򣬷���NOT_FIND_FIRST
// ����������
//		�õ�һ����������ĵ�һ����
// ���ã�
//		
// ********************************************************************
static int Seg_SearchFirstBlock( LPSEGMENT lpSeg, int index ) 
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + index;
	if( *lppBlks != NULL_BLOCK && index >= 0 )
	{   // 
		if( *lppBlks > RESERVED_BLOCK )  // ��ֵ�ǵ�һ��������ֵ�� ��
			return (*lppBlks)->idxFirst;  // �����ڴ��ṹ����ԱidxFirstָ���һ����
		else
			return (int)*lppBlks;  // �ǣ�ֱ�ӷ��ظ�ֵ
	}
	return NOT_FIND_FIRST;
}
// ********************************************************************
// ������void Seg_GetInfo( LPSEGMENT lpSeg, 
//				  LPDWORD lpdwAvailVirtual,
//				  LPDWORD lpdwReserve,
//				  LPDWORD lpdwCommit )

// ������
//		IN lpSeg - �νṹָ��
//		OUT lpdwAvailVirtual - ���ڽ��ܿ�������ռ� 
//		OUT lpdwReserve - ���ڽ����ѱ���������ռ�
//		OUT lpdwCommit - ���ڽ������ύ�����䣩�Ŀռ�
// ����ֵ��
//		��
// ����������
//		�õ�����Ϣ
// ���ã�
//		
// ********************************************************************
void Seg_GetInfo( LPSEGMENT lpSeg, 
				  LPDWORD lpdwAvailVirtual,
				  LPDWORD lpdwReserve,
				  LPDWORD lpdwCommit )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks+1;  // ÿ���εĵ�0���Ǳ�������
	int idxStart = 1;
	DWORD dwAvailVirtual = 0;
	DWORD dwReserve = 0;
	DWORD dwCommit = 0;

	// ����ÿһ����
	for(  ; idxStart < BLOCKS_PER_SEGMENT; idxStart++, lppBlks++ )
	{	// 
		if( *lppBlks != NULL_BLOCK )  // �Ƿ�ʹ�ã���������� ��
		{   // �ǣ�has alloc
			if( *lppBlks <= RESERVED_BLOCK )  // �Ǳ����� ��
				dwReserve += BLOCK_SIZE;  // ��
			else
			{   // �Ѿ��ύ�Ŀ�
				int i;
				LPUINT lpuiPage = (*lppBlks)->uiPages;
				// ͳ��ÿ�����ڵ�ҳ��
				for( i = 0; i < PAGES_PER_BLOCK; i++, lpuiPage++ )
				{
					if( *lpuiPage == 0 )  // δ�ύ/����
						dwReserve += PAGE_SIZE;
					else if( *lpuiPage != INVALID_PAGE )  // ���������Чҳ���Ǳ������ҳ
					{  // commited pages
						dwCommit += PAGE_SIZE;
					}
				}
			}
		}
		else
		{   // �ÿ�δʹ��
			dwAvailVirtual += BLOCK_SIZE;
		}
	}	
	*lpdwAvailVirtual = dwAvailVirtual;
	*lpdwReserve = dwReserve;
	*lpdwCommit = dwCommit;
}
// ********************************************************************
// ������static int Seg_GetFirstBlockPos( LPSEGMENT lpSeg, int idxStart, int iNeedPages, BOOL bAutoNext )
// ������
//		IN lpSeg - �νṹָ��
//		IN idxStart - ��ʼ��
//		IN iNeedPages - ��Ҫ��ҳ��
//		IN bAutoNext - �Ƿ��Զ�������һ��λ��
// ����ֵ��
//		����ɹ������ص�һ���������ֵ�����򣬷���0
// ����������
//		�õ�һ����СΪiNeedPahes��������δʹ�õĿ��飬�����ظ���ĵ�һ���������ֵ
// ���ã�
//		
// ********************************************************************

static int Seg_GetFirstBlockPos( LPSEGMENT lpSeg, int idxStart, int iNeedPages, BOOL bAutoNext )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks+idxStart;
	int idxFirst = idxStart;
	int iPages = iNeedPages;

	//RETAILMSG( 1, ( "idxStart=%d, iNeedPages=%d, bAutoNext=%d.\r\n", idxStart, iNeedPages, bAutoNext ) );

	for(  ; idxStart < BLOCKS_PER_SEGMENT; idxStart++, lppBlks++ )
	{
		if( *lppBlks != NULL_BLOCK )  // �Ƿ��ѱ�ռ��
		{   // �ǣ����鲻���á�has alloc
			if( bAutoNext )  // �Ƿ�����Զ�����Ѱ��
			{   // �ǣ����²���
				iPages = iNeedPages;
				idxFirst = idxStart + 1;
			}
			else
				return 0;  // ��Ҫ����Ѱ��, ����
		}
		else
		{   // δռ�ã�������
			if( (iPages -= PAGES_PER_BLOCK) <= 0 ) // ����ȫ��������Ҫ�� ��
				return idxFirst; // �ǣ�������������yes , i get it
		}
	}
	//RETAILMSG( 1, ( "idxStar_end=%d.\r\n", idxStart ) );

	return 0;
}

// ********************************************************************
// ������static int Seg_ReserveNeedPages( 
//								LPSEGMENT lpSeg,
//								int idxStart,
//								int iNeedPages, 
//								BOOL bAutoCommit, 
//								DWORD dwProtect )
// ������
//		IN lpSeg - �νṹָ��
//		IN idxStart - ��ʼ������
//		IN iNeedPages - ��Ҫ��ҳ��
//		IN bAutoCommit - �Ƿ��Զ��ύ
//		IN dwProtect
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ӿ�ʼ�鱣����Ҫ��ҳ
// ���ã�
//		
// ********************************************************************
static int Seg_ReserveNeedPages( LPSEGMENT lpSeg, int idxStart, int iNeedPages, BOOL bAutoCommit, DWORD dwProtect )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks+idxStart;
	LPMEMBLOCK lpStartBlk, lpBlk;//, lpBlkEnd;

	lpBlk = lpStartBlk = (LPMEMBLOCK)KHeap_Alloc( sizeof( MEMBLOCK ) );//�����ṹ

	if( lpStartBlk )
	{
		// ��ʼ����һ����
		memset( lpStartBlk, 0, sizeof( MEMBLOCK ) );

		lpStartBlk->idxFirst = idxStart;
		lpStartBlk->uiKey = 0;
		lpStartBlk->wLockCount = 0;
		lpStartBlk->uiFlag = bAutoCommit ? ( MF_AUTO_COMMIT | GetAttribFromProtect( dwProtect ) ) : 0;
		*lppBlks++ = lpStartBlk;
		
		iNeedPages -= PAGES_PER_BLOCK;

		//RETAILMSG( 1, ( "Seg_ReserveNeedPages: lpSeg=%x,idxStart=%d,*lppBlks=%x,iNeedPages=%d.\r\n", lpSeg, idxStart, *lppBlks, iNeedPages ) );
        
		// ����������
		for(  ; iNeedPages > 0; iNeedPages -= PAGES_PER_BLOCK, lppBlks++ )
		{	// 
			if( bAutoCommit || iNeedPages < PAGES_PER_BLOCK )
			{   // ������Զ��ύ �� ���һ���飬�����һ����ṹ
	            lpBlk = (LPMEMBLOCK)KHeap_Alloc( sizeof( MEMBLOCK ) );
				if( lpBlk )
				{   // ��ʼ����ṹ
		            memset( lpBlk, 0, sizeof( MEMBLOCK ) );
					lpBlk->idxFirst = idxStart;//
					lpBlk->uiKey = lpStartBlk->uiKey;
					lpBlk->wLockCount = lpStartBlk->wLockCount;
					lpBlk->uiFlag = lpStartBlk->uiFlag;
					*lppBlks = lpBlk;
				}
                else
				{	// û���ڴ棬�ͷ�֮ǰ����Ŀ�ṹ��no enough memory, clear
					LPMEMBLOCK * lppBlkStart = lpSeg->lpBlks+idxStart;
					//RETAILMSG( 1, ( "Seg_ReserveNeedPages:no enough memory, clear.\r\n" ) );
					while( lppBlkStart < lppBlks  )
					{
						if( *lppBlkStart > RESERVED_BLOCK ) // ��ָ���Ƿ���ģ��ͷ���
						{
							KHeap_Free( *lppBlkStart, sizeof( MEMBLOCK ) );
						}
						*lppBlkStart++ = NULL_BLOCK;
					}
					lpStartBlk = NULL; // use by retv
					iNeedPages = 0;
				}
			}
			else
			{   // ����Ҫ�Զ��ύ�������м�Ŀ顣���ñ�����־ set reserve flag
			    *lppBlks = (LPMEMBLOCK)idxStart;// ������־�Ǹ���ĵ�һ����
			}
		}
		if( iNeedPages )  // �Ƿ��������е�ҳ�������� ��
		{   // ���ǣ������һЩҳ����Ч�ġ�����������Ч��־��not all pages is valid, set invalid page flag
			LPUINT lpuiPage;

			iNeedPages += PAGES_PER_BLOCK;
			lpuiPage = &lpBlk->uiPages[iNeedPages];
			for( ; iNeedPages < PAGES_PER_BLOCK; iNeedPages++ )
			{
				*lpuiPage++ = INVALID_PAGE;  //������Ч��־
			}
		}
		//RETAILMSG( 1, ( "Seg_ReserveNeedPages-end: lpSeg=%x,idxStart=%d,*lppBlks=%x.\r\n", lpSeg, idxStart, *(lpSeg->lpBlks+idxStart) ) );
	}
	return lpStartBlk ? TRUE : FALSE;
}

// ********************************************************************
// ������static void Seg_FreeRegion( LPSEGMENT lpSeg, int idxFirstBlk )
// ������
//		IN lpSeg - �νṹָ��
//		IN idxFirstBlk - ��ʼ��
// ����ֵ��
//		��
// ����������
//		�ͷſ��飨�ڴ�����
// ���ã�
//		
// ********************************************************************
static void Seg_FreeRegion( LPSEGMENT lpSeg, int idxFirstBlk )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxFirstBlk;  // ��ʼ���ַ
	LPMEMBLOCK * lppBlkEnd = lpSeg->lpBlks + BLOCKS_PER_SEGMENT; // ���������ַ
	LPMEMBLOCK lpBlk;

	while( lppBlks < lppBlkEnd )
	{
		lpBlk = *lppBlks;
		if( lpBlk == NULL_BLOCK )
			break;  // ����
		else if( lpBlk <= RESERVED_BLOCK )
		{   // ��ΪNULL_BLOCK
			*lppBlks = NULL_BLOCK;
		}
		else if( lpBlk->idxFirst != idxFirstBlk )
		{   // ���� end
			break;
		}
		else
		{   // �ѷ���Ŀ飬�ͷ���commited block
			KHeap_Free( lpBlk, sizeof(MEMBLOCK) );//_kfree( lpBlk );
			*lppBlks = NULL_BLOCK;
		}
		lppBlks++;  // ��һ����
	}
}

// ********************************************************************
// ������int Seg_ReviewRegion( LPSEGMENT lpSeg, 
//					 int idxFirstBlk,
//					 int idxReviewBlk,
//					 int idxStartPage,
//					 int iReviewPages,
//					 int * lpiCountPages )
// ������
//		IN lpSeg - �νṹָ��
//		IN idxFirstBlk - �ڴ����һ����
//		IN idxReviewBlk - �ڴ�������Ҫ���ӵ���ʼ��
//		IN idxStartPage - �ڴ�������Ҫ���ӵ���ʼ���еĿ�ʼҳ. ����Ϊ-1,����ӵ�����Ľ���
//		IN iReviewPages - ��Ҫ���ӵ�ҳ��
//		IN/OUT lpiCountPages - ���ڽ��ܼ��ӵĺϷ�ҳ��, ͬʱҲ˵���ú����Ƿ����ִ��ͳ�ƹ��ܡ�
//							���ΪNULL����Ϊÿһ�����������һ����ṹ
// ����ֵ��
//		����ɹ���������Ҫ���������ҳ�������󷵻�-1
// ����������
//		��ָ������λ�ÿ�ʼ����һ����ҳ��������lpiCountPagesΪNULL,��Ϊÿһ�������Ŀ�λ�÷���һ����ṹ
// ���ã�
//		
// ********************************************************************
// returen need alloc physical page
#define DEBUG_SEG_REVIEWREGION 0
int Seg_ReviewRegion( LPSEGMENT lpSeg, 
					 int idxFirstBlk,
					 int idxReviewBlk,
					 int idxStartPage,
					 int iReviewPages,
					 int * lpiCountPages )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxReviewBlk; // ��ʼ���ַ
	LPMEMBLOCK lpBlk, lpStartBlk;
	int iPages = 0;
	int iAllocPages = 0;


    lpStartBlk = lpSeg->lpBlks[idxFirstBlk];
	if( iReviewPages == -1 )  // ��Ҫ���ӵ������� ��
	{	// �ǣ�����Ϊ���
		iReviewPages = (BLOCKS_PER_SEGMENT - idxReviewBlk) * PAGES_PER_BLOCK - idxStartPage;//BLOCKS_PER_SEGMENT * PAGES_PER_BLOCK;
	}
	DEBUGMSG( DEBUG_SEG_REVIEWREGION, ( "idxFirstBlk=%d,idxReviewBlk=%d,idxStartPage=%d,iReviewPages=%d.\r\n", idxFirstBlk,idxReviewBlk,idxStartPage,iReviewPages ) );
	while( iPages < iReviewPages )
	{
		lpBlk = *lppBlks;
		DEBUGMSG( DEBUG_SEG_REVIEWREGION, ( "Seg_ReviewRegion:lpBlk=%x.\r\n", lpBlk ) );
		if( lpBlk <= RESERVED_BLOCK && lpBlk != NULL_BLOCK )  // �ÿ��ѱ���������û��Ϊ������ṹ�� ��
		{   // �ѱ�������������ܣ�����һ����ṹ��now to commit a reserver block if need
			int nAlloc;
			if( lpiCountPages == NULL )   // ����Ϊ��ͳ���� ��
			{    // ����Ҫ����һ����ṹ
				lpBlk = KHeap_Alloc( sizeof( MEMBLOCK ) );
				if( lpBlk )
				{   // ��ʼ����ṹ
					memset( lpBlk, 0, sizeof( MEMBLOCK ) );					
					lpBlk->idxFirst = lpStartBlk->idxFirst;  // ָ���ڴ���ĵ�һ����
					lpBlk->uiKey = lpStartBlk->uiKey;
					lpBlk->wLockCount = lpStartBlk->wLockCount;	
					*lppBlks = lpBlk;
				}
				else //no memory
				{   // û���ڴ�
					ERRORMSG( 1, ( "Seg_ReviewRegion error: no enough memory.\r\n" ) );
					KL_SetLastError( ERROR_NOT_ENOUGH_MEMORY );
					goto _err_return;//break;
				}
			}
			nAlloc = PAGES_PER_BLOCK - idxStartPage; // �ÿ��ҳ��
			if( iPages + nAlloc > iReviewPages )  // �Ƿ���ͳ�������е�ҳ��
			{   // ��
				iAllocPages += iReviewPages - iPages; // ��ʵ������ҳ
				iPages = iReviewPages;	// ͳ���꣬�����˳�ѭ������				
			}
			else
			{	// 
				iPages += nAlloc;//�Ѽ��ӵ�ҳ��
				iAllocPages += nAlloc;//��Ҫ������ҳ��
			}
		}
		else if( lpBlk == NULL_BLOCK || lpBlk->idxFirst != idxFirstBlk )
		{	// ���ڴ����ѽ���
			if( lpiCountPages )
				break;  // ���������Ϊ��ͳ�ƣ������˳���end count
			// ����Ϊ��ͳ�ƣ��д��󣡣���
			ERRORMSG( 1, ( "Seg_ReviewRegion error: invalid lpBlk=0x%x,lpBlk->idxFirst=%d,idxFirstBlk=%d.\r\n", lpBlk, (lpBlk) ? lpBlk->idxFirst : 0, idxFirstBlk ) );
			KL_SetLastError( ERROR_INVALID_PARAMETER );
			goto _err_return;
		}
		else
		{   // �ÿ���һ���ڴ��ṹ��������ҳ
			LPUINT lpuiPage = &lpBlk->uiPages[idxStartPage];
			DEBUGMSG( DEBUG_SEG_REVIEWREGION, ( "Seg_ReviewRegion:Commit Blk,iPages=%d.\r\n", iPages ) );
			while( iPages < iReviewPages && idxStartPage < PAGES_PER_BLOCK ) //
			{
				UINT uiPage;
				if( (uiPage = *lpuiPage) == 0 )  // ��û���ύ����ҳ�𡡣�
				{   // û�У�����
					iAllocPages++;
				}
				else if( uiPage == INVALID_PAGE )
				{   // ��ҳ��һ����Чҳ�����ڴ���Ľ����߽硣error
					if( lpiCountPages )  // �����Ϊ��ͳ�ƣ���ȷ����
						goto _return;
					// ����û�дﵽָ���ļ���ҳ��
			        KL_SetLastError( ERROR_INVALID_PARAMETER );
					ERRORMSG( 1, ( "Seg_ReviewRegion error: invalid page.\r\n" ) );
					goto _err_return;
				}
				idxStartPage++;
				lpuiPage++;
				iPages++;
			}
		}
		idxStartPage = 0;  // ��һ����Ŀ�ʼҳ
		lppBlks++;	// ��һ����
	}
_return:
	if( lpiCountPages )
		*lpiCountPages = iPages;
	return iAllocPages;
_err_return:
	return -1;
}

// ********************************************************************
// ������BOOL Seg_ScanCommitRegion( LPSEGMENT lpSeg, 
//					 int idxFirstBlk,
//					 int idxScanBlk,
//					 int idxStartPage,
//					 int iScanPages )

// ������
//		IN lpSeg - �νṹָ��
//		IN idxFirstBlk - �ڴ����һ����
//		IN idxScanBlk - �ڴ�������Ҫɨ�����ʼ��
//		IN idxStartPage - �ڴ�������Ҫɨ�����ʼ���еĿ�ʼҳ
//		IN iScanPages - ��Ҫɨ���ҳ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ɨ��ָ�����ڴ����Ƿ���ָ����(iReviewPages)�Ѿ��ύ������ҳ�������У�����TRUE.
// ���ã�
//		
// ********************************************************************
// returen need alloc physical page
BOOL Seg_ScanCommitRegion( LPSEGMENT lpSeg, 
					 int idxFirstBlk,
					 int idxScanBlk,
					 int idxStartPage,
					 int iScanPages )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxScanBlk;  // ��ʼɨ���
	LPMEMBLOCK lpBlk;
	int iPages = 0;
	BOOL bRetv = TRUE;

	while( iPages < iScanPages )
	{
		lpBlk = *lppBlks;
		// 2004-01-28, this is a error ?, i will check it
		//if( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst != idxFirstBlk )
		if( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst == idxFirstBlk )
		// 
		{
			LPUINT lpuiPage = &lpBlk->uiPages[idxStartPage];
			while( iPages < iScanPages && idxStartPage < PAGES_PER_BLOCK )
			{
				UINT uiPage;
				if( (uiPage = *lpuiPage) == 0 ||
					 uiPage == INVALID_PAGE )   //  ҳ�Ѿ��ύ���� ��
				{	// û�У�����
					bRetv = FALSE;
			        KL_SetLastError( ERROR_INVALID_PARAMETER );
					goto _return;
				}
				idxStartPage++;
				lpuiPage++;  // ��һҳ
				iPages++;  // ��ͳ�Ƶ�ҳ��
			}
		}
		else
		{	// �зǸ��ڴ���Ŀ���з��ύҳ������
			bRetv = FALSE;
	        KL_SetLastError( ERROR_INVALID_PARAMETER );
			goto _return;
		}

		idxStartPage = 0;
		lppBlks++;	//��һ����
	}
_return:
	return bRetv;
}

// ********************************************************************
// ������BOOL Seg_ResetPageAttrib( 
//					    LPPROCESS_SEGMENTS lpProcessSeg,
//						LPSEGMENT lpSeg, 
//						int idxFirstBlk,
//						int idxReviewBlk,
//						int idxStartPage,
//						int iReviewPages,
//						DWORD dwNewAttrib )

// ������
//		IN lpProcessSeg - ���̶���
//		IN lpSeg - �νṹָ��
//		IN idxFirstBlk - �ڴ����һ����
//		IN idxReviewBlk - �ڴ�������Ҫ�������ʼ��
//		IN idxStartPage - �ڴ�������Ҫ�������ʼ���еĿ�ʼҳ
//		IN iReviewPages - ��Ҫ�����ҳ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�����趨ҳ������
// ���ã�
//		
// ********************************************************************
//
BOOL Seg_ResetPageAttrib( 
						 LPPROCESS_SEGMENTS lpProcessSeg,
						 LPSEGMENT lpSeg, 
						 int idxFirstBlk,
						 int idxReviewBlk,
						 int idxStartPage,
						 int iReviewPages,
						 DWORD dwNewAttrib )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxReviewBlk;  // ��ʼҳ
	LPMEMBLOCK lpBlk;
	int iPages = 0;
	BOOL bRetv = TRUE;
	UINT uStartPage; 
	BOOL bWriteToSecondPage;
	UINT uMinPage, uMaxPage;

	DWORD dwPhysAddrMask = GetPhyPageAdrMask();

	//��ҪCACHE ?-2005-02-02
	//bWriteToSecondPage = FALSE;
	//if( lpProcessSeg->lpdwSecondPageTable )
	//{
	uMinPage = uStartPage = idxReviewBlk * PAGES_PER_BLOCK + idxStartPage;
	uMaxPage = uStartPage + iReviewPages;

	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );
	//}
	//

	while( iPages < iReviewPages )
	{
		LPUINT lpuiPage;

		lpBlk = *lppBlks;
		// 2004-01-28�� this is a error, i will test it
		// ASSERT( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst != idxFirstBlk );
		ASSERT( lpBlk > RESERVED_BLOCK && lpBlk->idxFirst == idxFirstBlk );
		//
		//�����趨�ÿ��ҳ������ 
		lpuiPage = &lpBlk->uiPages[idxStartPage];
		while( iPages < iReviewPages && idxStartPage < PAGES_PER_BLOCK )
		{
			ASSERT( *lpuiPage != 0 && *lpuiPage != INVALID_PAGE );
			*lpuiPage = ( *lpuiPage & dwPhysAddrMask ) | dwNewAttrib;

			//����ҳ��-2005-02-03
			if( bWriteToSecondPage && 
				PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage ) )
			{
				lpProcessSeg->lpdwSecondPageTable[uStartPage] = *lpuiPage;
			}
			uStartPage++;
			//

			lpuiPage++;  // ��һҳ
			idxStartPage++;
			iPages++;
		}

		idxStartPage = 0;
		lppBlks++;	//��һ����
	}
	return TRUE;
}

// ********************************************************************
// ������BOOL Seg_DecommitPages( 
//							LPPROCESS_SEGMENTS lpProcessSeg,
//							LPSEGMENT lpSeg,
//							int idxStartBlk,
//							int idxStartPages,
//							int iPages 
//							)
// ������
//		IN lpProcessSeg - ���̶���
//		IN lpSeg - �νṹָ��
//		IN idxStartBlk - �ڴ�������Ҫ�������ʼ��
//		IN idxStartPages - �ڴ�������Ҫ�������ʼ���еĿ�ʼҳ
//		IN iPages - ��Ҫ�����ҳ��
// ����ֵ��
//		�������ͷ�����ҳ������TRUE�����򣬷���FALSE
// ����������
//		�ͷ�ָ���ڴ��������ҳ
// ���ã�
//		
// ********************************************************************
#define DEBUG_SEG_DECOMMITPAGES 0
BOOL Seg_DecommitPages( LPPROCESS_SEGMENTS lpProcessSeg, 
					    LPSEGMENT lpSeg, 
						int idxStartBlk, 
						int idxStartPages, 
						int iPages )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxStartBlk; // ��ʼ��
	BOOL bFlush = FALSE;
	UINT uStartPage, uMinPage, uMaxPage;
	BOOL bWriteToSecondPage;

	//��ҪCACHE ?-2005-02-02
	uMinPage = uStartPage = idxStartBlk * PAGES_PER_BLOCK + idxStartPages;
	uMaxPage = uStartPage + iPages;
	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );

	/*bWriteToSecondPage = FALSE;
	if( lpProcessSeg && lpProcessSeg->lpdwSecondPageTable )
	{
		uStartPage = idxStartBlk * PAGES_PER_BLOCK + idxStartPages;
		//ҳ�ڷ�Χ��
		if( uStartPage >= lpProcessSeg->uiPageTableStart &&
			uStartPage < (lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA) )
		{
			bWriteToSecondPage = TRUE;
		}		
	}
	*/
	//

	while( iPages > 0 )
	{
		ASSERT( *lppBlks != NULL_BLOCK );
		if( *lppBlks <= RESERVED_BLOCK )
		{	// �Ǳ����飬��ȥ���е�ҳ��
			int n = PAGES_PER_BLOCK - idxStartPages;
			iPages -= n;

			uStartPage += n;
		}
		else
		{	// �ڴ��
			UINT *lpui;
			// ���ҿ����Ƿ�������ҳ
			lpui = &(*lppBlks)->uiPages[idxStartPages];
			while( iPages && idxStartPages < PAGES_PER_BLOCK )
			{
				if( *lpui && *lpui != INVALID_PAGE )
				{	// ����Ч������ҳ���ͷ���
					DEBUGMSG( DEBUG_SEG_DECOMMITPAGES, ( "Phy=%x,b=%d,p=%d.\r\n", *lpui, idxStartBlk, idxStartPages ) );
					Page_Free( (LPVOID)_GetVirtualPageAdr( *lpui ) );
					*lpui = 0;

					//����ҳ��-2005-02-03
					if( bWriteToSecondPage && 
						PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage ) )
					{
						lpProcessSeg->lpdwSecondPageTable[uStartPage] = *lpui;
					}

					bFlush = TRUE; //ˢ��CACHE
				}
				uStartPage++;

				iPages--;
				lpui++;
				idxStartPages++;				
			}
		}
		idxStartPages = 0;
		lppBlks++;//��һ����
	}
	return bFlush;
}


// ********************************************************************
// ������BOOL AutoCommitPage( LPMEMBLOCK lpBlk, int idxStartPage )
// ������
//		IN lpBlk - MEMBLOCK���ڴ�飩�ṹָ��
//		IN idxStartPage - ���ڵĿ�ʼҳ
// ����ֵ��
//		�����ύ�ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�Զ��ύ����ҳ��ͨ�����쳣�������
// ���ã�
//		
// ********************************************************************
// handle by exception handler
#define DEBUG_AUTOCOMMITPAGE 0
//BOOL AutoCommitPage( LPMEMBLOCK lpBlk, int idxStartPage )
BOOL AutoCommitPage( LPMEMBLOCK lpBlk, DWORD dwAddress )
{
	UINT uiPhyPage;
	DWORD dwPhyFlagMask = GetPhyPageFlagMask();
	int idxStartPage = PAGE_INDEX( dwAddress );
	LPPROCESS_SEGMENTS lpProcessSeg;

	ASSERT( lpBlk->uiPages[idxStartPage] == 0 );
	uiPhyPage = (UINT)Page_Alloc( TRUE );//cache ��ַ
	if( uiPhyPage )
	{
#ifdef __DEBUG
		// �����Ƿ���һ������ ��������ΪuiPhyPage���ں�CACHE��ַ>0x80000000��
		// ����ҳ���Ҫ���õ�������������һ��CACHE��ַ < 0x80000000 ����.
		// �������UNCACHE��ַȥ���
	    //memset( (LPVOID)uiPhyPage, 0xCCCCCCCC, PAGE_SIZE );
		//
		memset( (LPVOID)CACHE_TO_UNCACHE(uiPhyPage), 0xCCCCCCCC, PAGE_SIZE );
		//
#endif
		// �õ���Ӧ������ҳ���������
		uiPhyPage = _GetPhysicalPageAdr( uiPhyPage ) | (lpBlk->uiFlag & dwPhyFlagMask);
	    lpBlk->uiPages[idxStartPage] = uiPhyPage;
		if( ( lpProcessSeg = Seg_FindProcessSegment( dwAddress ) ) )
		{
			ASSERT( lpProcessSeg->lpdwSecondPageTable );
			if( PAGE_INDEX_IN_SEGMENT( dwAddress ) >= lpProcessSeg->uiPageTableStart &&
				PAGE_INDEX_IN_SEGMENT( dwAddress ) < lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA )
			{
				lpProcessSeg->lpdwSecondPageTable[PAGE_INDEX_IN_MEGA( dwAddress )] = uiPhyPage;
			}
		}
		DEBUGMSG( DEBUG_AUTOCOMMITPAGE, ( "auto commit sucess=%x.\r\n", uiPhyPage ) );
	}
	else
	{
#ifdef __DEBUG
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		RETAILMSG( 1, ( "auto commit error: no physical page!!! .\r\n" ) );
		while(1);//ASSERT( 0 );  // no physical page!!!
#else
		return FALSE;
#endif
	}
	return TRUE;
}

// ********************************************************************
// ������BOOL Seg_MapPhyPages( 
//					  LPPROCESS_SEGMENTS lpProcessSeg,
//					  LPSEGMENT lpSeg, 
//					  int idxCommitBlk, 
//					  int idxPageOffset, 
//					  int iNeedPages, 
//					  int iAllocPages,
//					  UINT uiAllocType,
//					  DWORD * lpdwPhycialPages,
//					  DWORD dwProtect
//					  )
// ������
//		IN lpProcessSeg - ���̶���
//		IN lpSeg - �νṹָ�� 
//		IN idxCommitBlk - ��Ҫ�ύ�Ŀ�ʼ��
//		IN idxPageOffset - ��ʼ���ڵĿ�ʼҳ
//		IN iNeedPages - ��Ҫ���ҵ�ҳ
//		IN iAllocPages - ��Ҫ�ύ��ҳ
//		IN uiAllocType - �������ͣ�Ϊ����ֵ��
//					MEM_PHYSICAL - lpdwPhycialPages ��������ҳ�����û��MEM_CONTIGUOUS���ԣ�
//									�� lpdwPhycialPages[n] ������n������ҳ��
//									���û�и����ԣ����ɸú�����������ҳ
//					MEM_CONTIGUOUS - ����ҳ�������ģ�����lpdwPhycialPages�����ʾ����ҳ��ַ�Ŀ�ʼ
//		IN lpdwPhycialPages - ָ������ҳ������ҳ��ַ��������uiAllocType����
//		IN dwProtect - ��������
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		������ҳӳ�䵽��Ӧ����ռ�
// ���ã�
//		
// ********************************************************************
#define DEBUG_SEG_MAPPHYPAGES 0
BOOL Seg_MapPhyPages(
					  LPPROCESS_SEGMENTS lpProcessSeg,
					  LPSEGMENT lpSeg, 
					  int idxCommitBlk, 
					  int idxPageOffset, 
					  int iNeedPages, 
					  int iAllocPages,
					  UINT uiAllocType,
					  DWORD * lpdwPhycialPages,
					  DWORD dwProtect
					  )
{
	LPMEMBLOCK * lppBlks = lpSeg->lpBlks + idxCommitBlk; // ��ʼҳ
	LPMEMBLOCK lpBlk;
	int iPages = 0;
    BOOL bFlush = FALSE;
	UINT uStartPage, uMinPage, uMaxPage; 
	BOOL bWriteToSecondPage;

	if( (uiAllocType & MEM_PHYSICAL) == 0 )	//�ɱ�������������ҳ�� ��
	{	// ��
	    if( Page_Lock( iAllocPages ) == FALSE )  // ��ס��Ҫ��ҳ
		{	// ���ɹ�
			WARNMSG( DEBUG_SEG_MAPPHYPAGES, ( "Seg_MapPhyPages: failure to LockPages=%d.\r\n", iAllocPages ) );
		    return FALSE;
		}
	}
	// �õ�CPU��Ӧ��ҳ����
	dwProtect = GetAttribFromProtect( dwProtect );
	
	//��ҪCACHE ?-2005-02-02
	uMinPage = uStartPage = idxCommitBlk * PAGES_PER_BLOCK + idxPageOffset;
	uMaxPage = uStartPage + iNeedPages;
	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );
/*
	bWriteToSecondPage = FALSE;
	if( lpProcessSeg->lpdwSecondPageTable )
	{
		uStartPage = idxCommitBlk * PAGES_PER_BLOCK + idxPageOffset;
		//ҳ�ڷ�Χ��
		if( uStartPage >= lpProcessSeg->uiPageTableStart &&
			uStartPage < (lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA) )
		{
			bWriteToSecondPage = TRUE;
		}		
	}
*/
	//
	while( iPages < iNeedPages )
	{
		lpBlk = *lppBlks++;
		ASSERT( lpBlk && lpBlk > RESERVED_BLOCK );

		// �������ڵ�����ҳ��search the block's all pages
		while( idxPageOffset < PAGES_PER_BLOCK &&
			   iPages < iNeedPages )
		{
			if( lpBlk->uiPages[idxPageOffset] == 0 ) // ����������ҳ�� ��
			{   // û�У�Ϊ�����һ����now, to alloc a physical page
				int iTryCount = 0;
				UINT uiPhyPage;
				// ��Ȼ��Ӧ�ó���û������ҳ�������֮ǰ����Page_Lock��ס��Ҫ��ҳ�棬�����Լ���Ҳ�޷�
				// �����������Ҿ���Ӧ�ý�ѭ�����Ĵ��� �Ƶ� Page_Lock ���֣�������
				//	�ı����Ժ�
				for( ; iTryCount < 10; iTryCount++ )
				{
					if( (uiAllocType & MEM_PHYSICAL) == 0 )
					{	// �Զ�����
					    uiPhyPage = (UINT)Page_Alloc( FALSE );
						if( uiPhyPage )
						    uiPhyPage = _GetPhysicalPageAdr( uiPhyPage );
					}
					else
					{   //�ò������������ַ MEM_PHYSICAL
						if( uiAllocType & MEM_CONTIGUOUS )
						{	// �����ĵ�ַ��lpdwPhycialPages�����������ҳ��ַ
							uiPhyPage = (UINT)lpdwPhycialPages; // ��ǰ������ҳ��ַ
							lpdwPhycialPages = (DWORD*)NEXT_PHYSICAL_PAGE( lpdwPhycialPages );
						}
						else
						{   // ������������ҳ��physical pages is not contigous, put in the pointer
							uiPhyPage = *lpdwPhycialPages;
							lpdwPhycialPages++; // ��һ������ҳ
						}
					}

					if( uiPhyPage )
					{
						uiPhyPage |= dwProtect; // ���ϱ�������
						lpBlk->uiPages[idxPageOffset] = uiPhyPage;
						bFlush = TRUE;  // ˢ��CACHE
						//2005-02-02�Ƿ�CACHE
						if( bWriteToSecondPage && 
							PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage ) )
						{
							lpProcessSeg->lpdwSecondPageTable[uStartPage] = uiPhyPage;
						}
						//
						break;
					}
					WARNMSG( DEBUG_SEG_MAPPHYPAGES, ("Seg_MapPhyPages, entry sleep.\r\n" ) );
					Sleep(200);
				}
				if( uiPhyPage == 0 )
				{	// ��Ӧ�÷�����ĳЩ����
					ERRORMSG( DEBUG_SEG_MAPPHYPAGES, ( "error in Seg_MapPhyPages: fault error!, no enough phypage.\r\n" ) );
				}
			}
			idxPageOffset++; // ���ڵ���һ��ҳ
			iPages++;
			uStartPage++;
		}
		idxPageOffset = 0;
	}
	//
	// �Ҽٶ��������ڴ�����е��ͷŹ��ܽ������FlushCacheAndClearTLB���ܣ�
	// �������ڷ��书���ﲻ����FlushCacheAndClearTLB����
	// �����Ϊʲô��ȥ�������µĴ���
	//if( bFlush )
	  //  FlushCacheAndClearTLB();
	return TRUE;
}

// ********************************************************************
// ������LPSEGMENT Seg_FindSegmentByIndex( UINT index )
// ������
//		IN index - ������
// ����ֵ��
//		�νṹָ��
// ����������
//		�ɶ������õ������ڵĶ�
// ���ã�
//		
// ********************************************************************
LPSEGMENT Seg_FindSegmentByIndex( UINT index )
{
	if( index < 64 )
	{	// �û��ռ� user space
		if( lppProcessSegmentSlots[index] )
		    return (LPSEGMENT)lppProcessSegmentSlots[index]->lpSeg;
	}
	else 
	{	// �ں˿ռ� 0x80000000
		if( index == KERNEL_SEGMENT_INDEX )
		{
			return InitKernelProcess.lpProcessSegments->lpSeg;
		}
	}
	return NULL;
}


// ********************************************************************
// ������LPSEGMENT Seg_FindSegment( DWORD dwAddress )
// ������
//		IN dwAddress - ���ַ
// ����ֵ��
//		�νṹָ��
// ����������
//		�����ַ�õ������ڵĶ�
// ���ã�
//		
// ********************************************************************
#define DEBUG_SEG_FINDSEGMENT 0
static LPSEGMENT Seg_FindSegment( DWORD dwAddress )
{
	return Seg_FindSegmentByIndex( GET_SEGMENT_INDEX( dwAddress ) );
}

// ********************************************************************
// ������LPPROCESS_SEGMENTS Seg_FindProcessSegment( DWORD dwAddress )
// ������
//		IN dwAddress - ���ַ
// ����ֵ��
//		�νṹָ��
// ����������
//		�����ַ�õ������ڵĶ�
// ���ã�
//		
// ********************************************************************
#define DEBUG_SEG_FINDSEGMENT 0
static LPPROCESS_SEGMENTS Seg_FindProcessSegment( DWORD dwAddress )
{
	UINT index = GET_SEGMENT_INDEX( dwAddress );
	if( index < 64 )
	{	// �û��ռ� user space
		return lppProcessSegmentSlots[index];
	}
	else 
	{	// �ں˿ռ� 0x80000000
		if( index == KERNEL_SEGMENT_INDEX )
		{
			return InitKernelProcess.lpProcessSegments;
		}
	}
	return NULL;

}


// ********************************************************************
// ������BOOL Seg_Copy( 
//				LPPROCESS_SEGMENTS lpProcessSeg,
//				LPSEGMENT lpSegDst, 
//			    UINT idxDstStartBlk,
//				UINT idxStartPage,
//				DWORD dwCopyPages,
//		        LPSEGMENT lpSegSrc,
//				UINT idxSrcStartBlk,
//				DWORD dwProtect )
// ������
//		IN lpProcessSeg - ���̶���
//		IN lpSegDst - Ŀ��νṹָ�� 
//		IN idxDstStartBlk - Ŀ��ο�ʼ��
//		IN idxStartPage - Ŀ��ο��ڿ�ʼҳ
//		IN dwCopyPages - ��Ҫ������ҳ
//		IN lpSegSrc - Դ�νṹָ��
//		IN idxSrcStartBlk - Դ�ο�ʼ��
//		IN dwProtect - ��������
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		��Դ���������ҳӳ�䵽Ŀ������
// ���ã�
//		
// ********************************************************************
BOOL Seg_Copy( 
			    LPPROCESS_SEGMENTS lpProcessSeg,
				LPSEGMENT lpSegDst, 
			    UINT idxDstStartBlk,
				UINT idxStartPage,
				DWORD dwCopyPages,
		        LPSEGMENT lpSegSrc,
				UINT idxSrcStartBlk,
				DWORD dwProtect )
{
	LPMEMBLOCK * lppDstBlks, * lppSrcBlks;
	DWORD dwPhysAddrMask = GetPhyPageAdrMask();
	BOOL bWriteToSecondPage;
	UINT uStartPage, uMinPage, uMaxPage; 
	
	dwProtect = GetAttribFromProtect( dwProtect );//�õ�CPU������
    lppDstBlks = lpSegDst->lpBlks + idxDstStartBlk; // ��ʼĿ���
	lppSrcBlks = lpSegSrc->lpBlks + idxSrcStartBlk;//��ʼԴ��


	//��ҪCACHE ?-2005-02-03, add
	uMinPage = uStartPage = idxDstStartBlk * PAGES_PER_BLOCK + idxStartPage;
	uMaxPage = uStartPage + dwCopyPages;
	bWriteToSecondPage = GetPagesInCacheRange( lpProcessSeg, &uMinPage, &uMaxPage );

	/*
	bWriteToSecondPage = FALSE;
	if( lpProcessSeg->lpdwSecondPageTable )
	{
		uStartPage = idxDstStartBlk * PAGES_PER_BLOCK + idxStartPage;
		//ҳ�ڷ�Χ��
		if( uStartPage >= lpProcessSeg->uiPageTableStart &&
			uStartPage < (lpProcessSeg->uiPageTableStart + PAGES_PER_MEGA) )
		{
			bWriteToSecondPage = TRUE;
		}		
	}
	*/
	//

	while( dwCopyPages )
	{
		UINT * lpuiDstPage = &(*lppDstBlks)->uiPages[idxStartPage];//��ʼĿ��ҳ
		UINT * lpuiSrcPage = &(*lppSrcBlks)->uiPages[idxStartPage];//��ʼԴҳ

		lppDstBlks++; lppSrcBlks++;

		while( idxStartPage < PAGES_PER_BLOCK &&
			   dwCopyPages )
		{
			DWORD dwPhy = *lpuiSrcPage & dwPhysAddrMask; //�õ������ַ
			// ����
			Page_Duplicate( (LPVOID)_GetVirtualPageAdr( dwPhy ) );
			// 
			*lpuiDstPage = dwPhy | dwProtect;
			//����ҳ��-2005-02-03
			if( bWriteToSecondPage &&
				PAGE_IN_RANGE( uStartPage, uMinPage, uMaxPage) )
			{
				lpProcessSeg->lpdwSecondPageTable[uStartPage] = *lpuiDstPage;
			}
			uStartPage++;
			//			

			// ��һҳ
			lpuiDstPage++;
			lpuiSrcPage++;
			idxStartPage++;
			dwCopyPages--;
		}
		idxStartPage = 0;
	}
	return dwCopyPages ? FALSE : TRUE;
}


// ********************************************************************
// ������static LPVOID DoVirtualAlloc( 
//						LPVOID lpvAddress, 
//						DWORD dwSize, 
//						DWORD dwAllocType, 
//						DWORD dwProtect,
//						DWORD * lpdwPhycialPages )
// ������
//		IN lpvAddress - ���ַ
//		IN dwSize - ��Ҫ����Ĵ�С����byteΪ��λ��
//		IN uiAllocType - �������ͣ�Ϊ����ֵ����ϣ�
//					MEM_RESERVE - �������ַ
//					MEM_COMMIT - �ύ����ҳ
//
//					MEM_PHYSICAL - lpdwPhycialPages ��������ҳ�����û��MEM_CONTIGUOUS���ԣ�
//									�� lpdwPhycialPages[n] ������n������ҳ��
//									���û�и����ԣ����ɸú�����������ҳ
//					MEM_CONTIGUOUS - ����ҳ�������ģ�����lpdwPhycialPages�����ʾ����ҳ��ַ�Ŀ�ʼ
//		IN dwProtect - ��������
//		IN lpdwPhycialPages - ָ������ҳ������ҳ��ַ��������uiAllocType����
// ����ֵ��
//		����ɹ������ط�NULLָ�룻���򷵻�NULL
// ����������
//		�����û����ַ������uiAllocType��MEM_COMMIT��û��MEM_RESERVE����õ�ַ��Χ������֮ǰ�Ѿ���������
// ���ã�
//		
// ********************************************************************
#define DEBUG_DOVIRTUALALLOC 0
static LPVOID DoVirtualAlloc( 
						LPVOID lpvAddress, 
						DWORD dwSize, 
						DWORD dwAllocType, 
						DWORD dwProtect,
						DWORD * lpdwPhycialPages )
{
	DWORD dwStart = (DWORD)lpvAddress;
	DWORD dwEnd = dwStart + dwSize;
	DWORD dwSegIndex = GET_SEGMENT_INDEX( dwStart );
	LPSEGMENT lpSeg;
	LPPROCESS_SEGMENTS lpProcessSeg;
	DWORD dwError = 0;
	DWORD dwRetv = NULL;


	DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtualAlloc:lpvAddress=%x,dwSize=%d.\r\n", lpvAddress,dwSize ) );
	// ��������check param
	if( (dwAllocType & MEM_PHYSICAL) && lpdwPhycialPages == NULL )
	{
        KL_SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	KL_EnterCriticalSection( &csVirtualMem );

	//ASSERT( dwSegIndex < MAX_SEGMENTS && dwSize ); //2005-01-18
//	ASSERT( dwSize );
	
	//if( dwSegIndex < MAX_SEGMENTS && dwSize )
	if( dwSize )
	{
		if( ( lpProcessSeg = Seg_FindProcessSegment( dwStart ) ) &&
			( lpSeg = lpProcessSeg->lpSeg ) )  // ���Ҷ�Ӧ�Ķ�
		{
			int iNeedPages;
			int iPages;
			int idxFirst, idxCommit, idxCommitPage;
			BOOL bReserved = FALSE;

			DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtualAlloc:lpSeg=%x.\r\n", lpSeg ) );

		    idxFirst = -1;
			//�Ƿ���Ҫ���������� ��
			if( (dwAllocType & MEM_RESERVE) ||
				( lpvAddress == NULL && (dwAllocType & MEM_COMMIT) ) 
			  )
			{   // ��Ҫȥ������Ӧ��ҳ to get enough free block( NULL_BLOCK )
				// ��Ҫ��ҳ��
			    iPages = iNeedPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwStart / PAGE_SIZE );

				dwAllocType |= MEM_RESERVE;
				idxFirst = 0;

				if( dwSegIndex )
				{
					dwStart &= SEGMENT_MASK;  // 32M size
				}
				if( dwStart ) // �û�ָ����ַ ��
				{  // ��
					dwStart &= 0xffff0000; // ���ᵽ64k�߽� round down 64k
					idxFirst = BLOCK_INDEX( dwStart );
				}
				else // �񣬴�1�鿪ʼ��0��ϵͳ������
					idxFirst = 1;  // first block reserved by sys
				
				// search enough blocks
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:idxFirst=%d,iNeedPages=%d.\r\n", idxFirst, iNeedPages ) );
				//�õ��ڴ���Ŀ�ʼ��
				idxCommit = idxFirst = Seg_GetFirstBlockPos( lpSeg, idxFirst, iNeedPages, dwStart == NULL );
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:Reserve idxFirst=%d.\r\n", idxFirst ) );
				// ������Ҫ��ҳ
				if( idxFirst &&
					Seg_ReserveNeedPages( lpSeg, idxFirst, iNeedPages, (dwAllocType & MEM_AUTO_COMMIT), dwProtect ) )
				{   // now to reserve
					bReserved = TRUE;
					dwStart = idxFirst << BLOCK_SHIFT;
				}
				else
				{
					WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc: failuer to ReserveNeedPages(%d), idxFirst(%d), dwStart(0x%x).\r\n", iNeedPages, idxFirst, dwStart ) );
					dwError = ERROR_NOT_ENOUGH_MEMORY;
					goto _err_return;
				}
			}
			else if( dwStart && (dwAllocType & MEM_COMMIT) )
			{   // ֱ���ύ����ҳ��to commit blocks which have reserved
				idxCommit = idxFirst = BLOCK_INDEX( dwStart );  // ��ʼ��
				// align to page
				//��Ҫ��ҳ��
			    iNeedPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwStart / PAGE_SIZE );
				dwStart &= ~PAGE_MASK;
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:dwStart=%x,idxFirst=%d.\r\n", dwStart, idxFirst ) );
                //�õ�����Ŀ�ʼ��
				idxFirst = Seg_SearchFirstBlock( lpSeg, idxFirst );
				if( idxFirst == NOT_FIND_FIRST )
				{   // ���ĵ�ַ����Ч�ģ�error param
					WARNMSG( DEBUG_DOVIRTUALALLOC, ( "warn in DoVirtaulAlloc:NOT_FIND_FIRST,idxFirst=%d.\r\n", idxFirst ) );
//					ASSERT( 0 );
					dwError = ERROR_INVALID_PARAMETER;
					goto _err_return;
				}
			}
			//���´������ύ����ҳ����
			// now to commit page if need
			if( dwAllocType & MEM_COMMIT )  ////��Ҫ�ύ����ҳ��
			{   //��Ҫ�ύ����ҳ
				int iAllocPages;

				idxCommitPage = PAGE_INDEX( dwStart );
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:To Commit,idxCommit=%d,idxCommitPage=%d,iNeedPages=%d.\r\n", idxCommit, idxCommitPage, iNeedPages ) );
				//������Ҫ�ύ��ҳ����������Ӧ�Ŀ�ṹ
				if( ( iAllocPages = Seg_ReviewRegion( lpSeg, idxFirst, idxCommit, idxCommitPage, iNeedPages, NULL ) ) != -1 )
				{   // �ɹ����ύ����ҳ��ok , success to commit��now, alloc the physical pages for commit page
					DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc: iAllocPages=%d.\r\n", iAllocPages ) );
					if( Seg_MapPhyPages( lpProcessSeg, lpSeg, idxCommit, idxCommitPage, iNeedPages, iAllocPages,  dwAllocType, lpdwPhycialPages, dwProtect ) )
					{
						DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:success to commit=%x.\r\n", dwStart ) );
						dwRetv = dwStart;
						FlushCacheAndClearTLB(); //ˢ��CACHE
						goto _return;						
					}
					else
					{
						WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:failuer to MapPhyPages.\r\n" ) );
					}
				}
				else
				{
					WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:failuer to ReviewRegion.\r\n" ) );
				}
				// ������ĳЩ���������������some error happen, now clear reserved
				WARNMSG( DEBUG_DOVIRTUALALLOC, ( "Failuer to Commit.\r\n" ) );
				if( bReserved )
				{
					Seg_FreeRegion( lpSeg, idxFirst );
				}
			}
			else //����Ҫ�ύ����ҳ������������no commit , only reserve
			{
				DEBUGMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:success to reserve=%x.\r\n", dwStart ) );
				dwRetv = dwStart;
				goto _return;
			}
		}
		else
		{
		    WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc: not find the segment addr=(0x%x), index=%d, knx=%d.\r\n", dwStart, dwSegIndex, KERNEL_SEGMENT_INDEX ) );
			ASSERT( 0 );
		}
	}
_err_return:
	KL_SetLastError( dwError );
	WARNMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:failure alloc address(%x).\r\n", dwStart ) );
_return:
	KL_LeaveCriticalSection( &csVirtualMem );
	if( dwRetv )
	    dwRetv = dwRetv | ( dwSegIndex << SEGMENT_SHIFT );
	RETAILMSG( DEBUG_DOVIRTUALALLOC, ( "DoVirtaulAlloc:dwRetv(0x%x).\r\n", dwRetv ) );
	return (LPVOID)dwRetv;
}

// ********************************************************************
// ������LPVOID WINAPI KL_VirtualAlloc( 
//							LPVOID lpvAddress, 
//							DWORD dwSize, 
//							DWORD dwAllocType, 
//							DWORD dwProtect )
// ������
//		IN lpvAddress - ���ַ
//		IN dwSize - ��Ҫ����Ĵ�С����byteΪ��λ��
//		IN dwAllocType - �������ͣ�Ϊ����ֵ����ϣ�
//					MEM_RESERVE - �������ַ
//					MEM_COMMIT - �ύ����ҳ
//		IN dwProtect - ��������
// ����ֵ��
//		����ɹ������ط�NULLָ�룻���򷵻�NULL
// ����������
//		�����û����ַ������uiAllocType��MEM_COMMIT��û��MEM_RESERVE����õ�ַ��Χ������֮ǰ�Ѿ���������
// ���ã�
//		ϵͳAPI		
// ********************************************************************
#define DEBUG_KL_VirtualAlloc 0
LPVOID WINAPI KL_VirtualAlloc( 
							LPVOID lpvAddress, 
							DWORD dwSize, 
							DWORD dwAllocType, 
							DWORD dwProtect )
{
	DEBUGMSG( DEBUG_KL_VirtualAlloc, ( "KL_VirtualAlloc entry,lpvAddress=0x%x,dwSize=%d,dwAllocType=0x%x.\r\n", lpvAddress, dwSize, dwAllocType ) );
	if( (DWORD)lpvAddress < 0x80000000 )
		return DoVirtualAlloc( lpvAddress, dwSize, dwAllocType, dwProtect, NULL );
	else
		return NULL;
}

// ********************************************************************
// ������LPVOID WINAPI KC_VirtualAlloc( 
//							LPVOID lpvAddress, 
//							DWORD dwSize, 
//							DWORD dwAllocType, 
//							DWORD dwProtect )
// ������
//		IN lpvAddress - ���ַ
//		IN dwSize - ��Ҫ����Ĵ�С����byteΪ��λ��
//		IN dwAllocType - �������ͣ�Ϊ����ֵ����ϣ�
//					MEM_RESERVE - �������ַ
//					MEM_COMMIT - �ύ����ҳ
//		IN dwProtect - ��������
// ����ֵ��
//		����ɹ������ط�NULLָ�룻���򷵻�NULL
// ����������
//		�ں˰棬�����û����ַ������uiAllocType��MEM_COMMIT��û��MEM_RESERVE����õ�ַ��Χ������֮ǰ�Ѿ���������
// ���ã�
//		�ں˰�
// ********************************************************************
#define DEBUG_KC_VirtualAlloc 0
LPVOID WINAPI KC_VirtualAlloc( 
							LPVOID lpvAddress, 
							DWORD dwSize, 
							DWORD dwAllocType, 
							DWORD dwProtect )
{
	DEBUGMSG( DEBUG_KC_VirtualAlloc, ( "KC_VirtualAlloc entry,lpvAddress=0x%x,dwSize=%d.\r\n", lpvAddress, dwSize ) );
	return DoVirtualAlloc( lpvAddress, dwSize, dwAllocType, dwProtect, NULL );
}


// ********************************************************************
// ������BOOL DoVirtualCopy( 
//						DWORD dwDstAddress, 
//						DWORD dwSrcAddress,
//						DWORD dwSize, 
//						DWORD dwProtect
//						)
// ������
//		IN dwDstAddress - Ŀ���ַ
//		IN dwSrcAddress - Դ��ַ
//		IN dwSize - ��Ҫ�����Ĵ�С
//		IN dwProtect - ��������
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		��Դ��ַ������ҳӳ�䵽Ŀ���ַ
// ���ã�
//		
// ********************************************************************

#define DEBUG_VIRTUALCOPY 0
static BOOL DoVirtualCopy( 
						DWORD dwDstAddress, 
						DWORD dwSrcAddress,
						DWORD dwSize, 
						DWORD dwProtect
						)
{
	DWORD dwDstStart = dwDstAddress;
	DWORD dwDstEnd = dwDstStart + dwSize;
	DWORD dwSegIndex = GET_SEGMENT_INDEX( dwDstStart );
	LPSEGMENT lpSegDst;
	LPPROCESS_SEGMENTS lpProcessSegDst;
	DWORD dwError = 0;
	DWORD dwRetv = NULL;
	DWORD dwPhyAdr = 0;
	UINT uiNeedPages;
	
	BOOL bVirtualAdr = FALSE;
	BOOL bRetv = FALSE;

	if( dwSize == 0 || dwDstStart == 0 || dwSrcAddress == 0 || dwSegIndex >= MAX_SEGMENTS )
		goto _RET_INVALID;

	uiNeedPages = ( ( dwDstEnd + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwDstStart / PAGE_SIZE );

	if( dwProtect & PAGE_PHYSICAL ) // dwSrcAddress�������ַ�� ��
	{	//��
		dwPhyAdr = (dwSrcAddress << 8) & ~(PAGE_SIZE-1); // align to page
	    if( ( dwPhyAdr & PAGE_MASK ) != ( dwDstStart & PAGE_MASK ) )  // Դ��Ŀ��Ӧ�ö���ҳ�߽�
			goto _RET_INVALID;

		dwProtect &= ~PAGE_PHYSICAL;
	}
	else if( IsKernelVirtualAddress( dwSrcAddress ) )  // Դ���ں˵�ַ�� ��
	{	//��
		dwPhyAdr = _GetPhysicalPageAdr( dwSrcAddress ); // �õ���Ӧ�������ַ
	    if( ( dwPhyAdr & PAGE_MASK ) != ( dwDstStart & PAGE_MASK ) ) // Դ��Ŀ��Ӧ�ö���ҳ�߽�
			goto _RET_INVALID;
	}
	else
	{   // Դ���û���ַ�ռ䡣from other virtual address
		bVirtualAdr = TRUE;
	}

	KL_EnterCriticalSection( &csVirtualMem );  // �����ͻ��

	//lpSegDst = Seg_FindSegment( dwDstStart );  // ����Ŀ���
	lpProcessSegDst = Seg_FindProcessSegment( dwDstStart );
	if( lpProcessSegDst )
		lpSegDst = lpProcessSegDst->lpSeg;
	else
		lpSegDst = NULL;

	if( lpSegDst )
	{	//
		UINT idxDstStartBlk;
		UINT idxDstStartPage;
		UINT idxFirstBlk;
		UINT uiNeedAllocPages;

        idxDstStartBlk = BLOCK_INDEX( dwDstAddress );
       	idxDstStartPage = PAGE_INDEX( dwDstAddress );

 	    idxFirstBlk = Seg_SearchFirstBlock( lpSegDst, idxDstStartBlk );		

		if( idxFirstBlk == NOT_FIND_FIRST )
			goto _RET_INVALID_LEAVE;

		// ���Ŀ�����򣬱�֤������Ҫ������page�ѱ�������	
	    uiNeedAllocPages = Seg_ReviewRegion( lpSegDst, idxFirstBlk, idxDstStartBlk, idxDstStartPage, uiNeedPages, NULL );
		if( uiNeedAllocPages != uiNeedPages )
			goto _RET_INVALID_LEAVE;
		if( bVirtualAdr )
		{   // Դ���û���ַ�ռ䣬from other virtual address
			LPSEGMENT lpSegSrc;
			UINT idxSrcStartBlk;
			UINT idxSrcStartPage;
			UINT idxFirstBlk;
			UINT uiAllocPages;
			UINT uiCountPages;
			
			idxSrcStartPage = PAGE_INDEX( dwSrcAddress );
			if( idxSrcStartPage != idxDstStartPage )
				goto _RET_INVALID_LEAVE;

			idxSrcStartBlk = BLOCK_INDEX( dwSrcAddress );


			lpSegSrc = Seg_FindSegment( dwSrcAddress );
			if( lpSegSrc == NULL )
				goto _RET_INVALID_LEAVE;
			
			idxFirstBlk = Seg_SearchFirstBlock( lpSegSrc, idxSrcStartBlk );
		    if( idxFirstBlk == NOT_FIND_FIRST )
			    goto _RET_INVALID_LEAVE;
			
			// ��֤������Ҫ��page�ѱ��ύ��
			
			uiAllocPages = Seg_ReviewRegion( lpSegSrc, idxFirstBlk, idxSrcStartBlk, idxSrcStartPage, uiNeedPages, &uiCountPages );
			if( uiAllocPages || uiCountPages != uiNeedPages )
				goto _RET_INVALID_LEAVE;
			bRetv = Seg_Copy( lpProcessSegDst, lpSegDst, idxDstStartBlk, idxDstStartPage, uiNeedPages, lpSegSrc, idxSrcStartBlk, dwProtect );
		}
		else
		{   // Դ�������ַ��ֱ��ӳ�䡣phy address
			bRetv = Seg_MapPhyPages( lpProcessSegDst, lpSegDst, idxDstStartBlk, idxDstStartPage, uiNeedPages, uiNeedAllocPages, MEM_PHYSICAL | MEM_CONTIGUOUS, (DWORD*)dwPhyAdr, dwProtect );
		}
	}

	if( bRetv )
		FlushCacheAndClearTLB(); // ��Ϊ������ӳ�䣬����ˢ��CACHE

	KL_LeaveCriticalSection( &csVirtualMem );//�뿪��ͻ��
	return bRetv;

_RET_INVALID_LEAVE:
	KL_LeaveCriticalSection( &csVirtualMem );
_RET_INVALID:
	KL_SetLastError( ERROR_INVALID_PARAMETER );
	WARNMSG( DEBUG_VIRTUALCOPY, ( "DoVirtaulCopy:failure alloc.\r\n" ) );
	return bRetv;
}

// ********************************************************************
// ������BOOL WINAPI KL_VirtualCopy( 
//							 LPVOID lpvDest,
//							 LPVOID lpvSrc,
//							 DWORD cbSize,
//							 DWORD fdwProtect )
// ������
//		IN dwDstAddress - Ŀ���ַ
//		IN dwSrcAddress - Դ��ַ
//		IN dwSize - ��Ҫ�����Ĵ�С
//		IN dwProtect - ��������
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		��Դ��ַ������ҳӳ�䵽Ŀ���ַ
// ���ã�
//		ϵͳAPI	
// ********************************************************************
#define DEBUG_KL_VirtualCopy 0
BOOL WINAPI KL_VirtualCopy( 
							 LPVOID lpvDest,
							 LPVOID lpvSrc,
							 DWORD cbSize,
							 DWORD fdwProtect )
{
	DEBUGMSG( DEBUG_KL_VirtualCopy, ( "KL_VirtualAlloc entry,lpvDest=0x%x,lpvSrc=0x%x,cbSize=%d.\r\n", lpvDest, lpvSrc, cbSize ) );
	return DoVirtualCopy( (DWORD)lpvDest, (DWORD)lpvSrc, cbSize, fdwProtect ) ? TRUE : FALSE;
}

// ********************************************************************
// ������static BOOL DoVirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
// ������
//		IN lpAddress - ���ַ
//		IN dwSize - ���ڴ��С
//		IN dwFreeType - �ͷ����ͣ�����Ϊ��
//				MEM_DECOMMIT - �ͷ�һ�����������ҳ
//				MEM_RELEASE - �ͷ�������dwSize����Ϊ0
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ͷ����ڴ���/��ַ
// ���ã�
//		
// ********************************************************************
static BOOL DoVirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
	DWORD dwStart = (DWORD)lpAddress;
	DWORD dwSegIndex;
	int idxFirstBlk, idxStartBlk, idxPage;
	int iPages;
	LPSEGMENT lpSeg;
	LPPROCESS_SEGMENTS lpProcessSeg;
	BOOL bRetv = FALSE;
	BOOL bFlush = FALSE;

	// check param
	// �������Ƿ���Ч
	if( lpAddress == NULL || 
		( (dwFreeType & MEM_RELEASE) && dwSize ) )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	KL_EnterCriticalSection( &csVirtualMem );

	dwSegIndex = GET_SEGMENT_INDEX( dwStart );

	if( ( lpProcessSeg = Seg_FindProcessSegment( dwStart ) ) &&
		( lpSeg = lpProcessSeg->lpSeg ) )  // �õ���ַ��Ӧ�Ķ�ָ��
	{
		idxStartBlk = BLOCK_INDEX( dwStart );
		idxFirstBlk = Seg_SearchFirstBlock( lpSeg, idxStartBlk ); // �õ�����ĵ�һ����
		idxPage = PAGE_INDEX( dwStart );
		if( idxFirstBlk != NOT_FIND_FIRST )  // ����Ч�� ��
		{	// ��Ч
			int iReserved;
			int iCountPages = 0;
			if( dwFreeType == MEM_DECOMMIT && dwSize )
			{	// �ͷ�һ������
                iPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) >> PAGE_SHIFT ) - ( dwStart >> PAGE_SHIFT );
				// ͳ�Ʊ�����ҳ�� �� �ѷ����ҳ��
				iReserved = Seg_ReviewRegion( lpSeg, idxFirstBlk, idxStartBlk, idxPage, iPages, &iCountPages );
				if( iCountPages >= iPages )
				{
					if( iReserved != iPages )
					{   //��ˢ��CACHE 
						FlushCacheAndClearTLB();
						bFlush |= Seg_DecommitPages( lpProcessSeg, lpSeg, idxStartBlk, idxPage, iPages );
						bRetv = TRUE;
						goto _return;
					}
				}
				else
				{   //error
					ASSERT( 0 );
				}
			}
			else if( dwFreeType & MEM_RELEASE )
			{	//�ͷ�������
                if( idxPage == 0 && idxFirstBlk == idxStartBlk )
                {	// ͳ�Ʊ�����ҳ�� �� �ѷ����ҳ��
					iReserved = Seg_ReviewRegion( lpSeg, idxFirstBlk, idxStartBlk, 0, -1, &iCountPages );
				    if( iReserved != iCountPages )
					{
						FlushCacheAndClearTLB();
						bFlush |= Seg_DecommitPages( lpProcessSeg, lpSeg, idxStartBlk, 0, iCountPages );
					}
					// �ͷŸ����Ѷ�λ�Ŀ�
					Seg_FreeRegion( lpSeg, idxStartBlk );
					bRetv = TRUE;
					goto _return;
				}
			}
		}
	}
_return:
	//����Seg_DecommitPages֮ǰ�Ѿ�������FlushCacheAndClearTLB�������Ҫ�� ��
	if( bFlush )
        FlushCacheAndClearTLB();
	KL_LeaveCriticalSection( &csVirtualMem );
	return bRetv;
}

// ********************************************************************
// ������BOOL WINAPI KL_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
//		IN lpAddress - ���ַ
//		IN dwSize - ���ڴ��С
//		IN dwFreeType - �ͷ����ͣ�����Ϊ��
//				MEM_DECOMMIT - �ͷ�һ�����������ҳ
//				MEM_RELEASE - �ͷ�������dwSize����Ϊ0
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ͷ����ڴ���/��ַ
// ���ã�
//		ϵͳAPI	
// ********************************************************************
#define DEBUG_KL_VirtualFree 0
BOOL WINAPI KL_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
	DEBUGMSG( DEBUG_KL_VirtualFree, ( "KL_VirtualFree entry,lpAddress=0x%x,dwSize=0x%x.\r\n", lpAddress, dwSize ) );
	if( (DWORD)lpAddress < 0x80000000 )
		return DoVirtualFree( lpAddress, dwSize, dwFreeType );
	else
		return FALSE;
}

// ********************************************************************
// ������BOOL WINAPI KC_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
//		IN lpAddress - ���ַ
//		IN dwSize - ���ڴ��С
//		IN dwFreeType - �ͷ����ͣ�����Ϊ��
//				MEM_DECOMMIT - �ͷ�һ�����������ҳ
//				MEM_RELEASE - �ͷ�������dwSize����Ϊ0
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ͷ����ڴ���/��ַ
// ���ã�
//		�ں˰�
// ********************************************************************
#define DEBUG_KC_VirtualFree 0
BOOL WINAPI KC_VirtualFree( LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType )
{
	DEBUGMSG( DEBUG_KC_VirtualFree, ( "KC_VirtualFree entry,lpAddress=0x%x,dwSize=0x%x.\r\n", lpAddress, dwSize ) );
	return DoVirtualFree( lpAddress, dwSize, dwFreeType );
}


// ********************************************************************
// ������BOOL WINAPI KL_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
// ������
//		IN lpAddress - ���ַ
//		IN dwSize - ���ڴ��С
//		IN flNewProtect - �µı�������
//		IN lpfOldProtect - ֮ǰ�ı�������
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		���������ڴ����ҳ����
// ���ã�
//		ϵͳAPI
// ********************************************************************
#define DEBUG_KL_VirtualProtect 0
BOOL WINAPI KL_VirtualProtect( LPVOID lpvAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect )
{
	DWORD dwStart;// = (DWORD)lpvAddress;
	DWORD dwSegIndex;// = GET_SEGMENT_INDEX( (DWORD)dwStart );
	LPSEGMENT lpSeg;
	DWORD dwError = 0;
	LPPROCESS_SEGMENTS lpProcessSeg;

	DEBUGMSG( DEBUG_KL_VirtualProtect, ( "KL_VirtualProtect entry,lpvAddress=0x%x,dwSize=0x%x.\r\n", lpvAddress, dwSize ) );

	// ������
	if( dwSize == 0 || 
		lpflOldProtect == NULL ||
		lpvAddress == NULL )
	{
        KL_SetLastError(  ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	dwStart = (DWORD)lpvAddress;
	dwSegIndex = GET_SEGMENT_INDEX( (DWORD)dwStart );

	KL_EnterCriticalSection( &csVirtualMem );

	if( dwSegIndex < MAX_SEGMENTS )
	{
		if( ( lpProcessSeg = Seg_FindProcessSegment( dwStart ) ) &&
			( lpSeg = lpProcessSeg->lpSeg ) )  // �õ����ַ��Ӧ�Ķ�ָ��
		//if( ( lpSeg = Seg_FindSegment( dwStart ) ) ) // �õ����ַ���ڵĶ�
		{
			DWORD idxFirstBlk, idxStartBlk = BLOCK_INDEX( dwStart );
			DWORD idxStartPage;
			DWORD dwPages;
			// 2004-01-28�� this is a error, i will test it
			//idxStartBlk = Seg_SearchFirstBlock( lpSeg, idxFirstBlk );
			idxFirstBlk = Seg_SearchFirstBlock( lpSeg, idxStartBlk ); // �õ����ڴ���ĵ�һ����
			//if( idxStartBlk != NOT_FIND_FIRST )
			if( idxFirstBlk != NOT_FIND_FIRST )
			// 2004-01-28
			{
				idxStartPage = PAGE_INDEX( dwStart );
				dwPages = ( ( dwStart + dwSize + PAGE_SIZE - 1 ) / PAGE_SIZE ) - ( dwStart / PAGE_SIZE );
				// ɨ����Ҫ��ҳ�Ƿ��Ѿ��ύ
				if( Seg_ScanCommitRegion( lpSeg, idxFirstBlk, idxStartBlk, idxStartPage, dwPages ) )
				{	//��
					DWORD dwNewAttrib;
					//�õ���ǰ������
					*lpflOldProtect = GetProtectFromAttrib( lpSeg->lpBlks[idxStartBlk]->uiPages[idxStartPage] );
					dwNewAttrib = GetAttribFromProtect( flNewProtect );
					if( dwNewAttrib )
					{	//�����µ�����
					    Seg_ResetPageAttrib( lpProcessSeg, lpSeg, idxFirstBlk, idxStartBlk, idxStartPage, dwPages, dwNewAttrib );
						FlushCacheAndClearTLB(); //ˢ��CACHE
					}
				}
			}
			else
			{   // ���ַ��Ч
				//ASSERT( 0 );
				dwError = ERROR_INVALID_PARAMETER;
				WARNMSG( DEBUG_KL_VirtualProtect, ( "KL_VirtualProtect: invalid address=0x%x,dwSize=0x%x.\r\n", lpvAddress, dwSize ) );
				goto _err_return;
			}
		}
	}
_err_return:
	if( dwError )
		KL_SetLastError( dwError );
	KL_LeaveCriticalSection( &csVirtualMem );
	return dwError ? FALSE : TRUE;
}

// ********************************************************************
// ������LPVOID WINAPI KL_AllocPhysMem( 
//						   DWORD dwSize,
//						   DWORD fdwProtect,
//						   DWORD dwAlignmentMask,
//						   DWORD dwFlags,
//						   ULONG * pPhysicalAddress
//						   )
// ������
//		IN dwSize - ��Ҫ����Ĵ�С
//		IN fdwProtect - ��������
//		IN dwAlignmentMask - ��ʼ��ַ����ģʽ
//		IN dwFlags - ��־
//		OUT pPhysicalAddress - ���ڽ��ܷ���������ַ
// ����ֵ��
//		����ɹ������ط�NULL�����ַ��pPhysicalAddress���ض�Ӧ�������ַ�����򣬷���NULL
// ����������
//		��������������ҳ
// ���ã�
//		ϵͳAPI
// ********************************************************************
#define DEBUG_KL_AllocPhysMem 0
LPVOID WINAPI KL_AllocPhysMem( 
						   DWORD dwSize,
						   DWORD fdwProtect,
						   DWORD dwAlignmentMask,
						   DWORD dwFlags,
						   ULONG * pPhysicalAddress
						   )
{
   	void * p;
	DWORD dwPageSize = ALIGN_PAGE_UP( dwSize );
    LPVOID lpVAdr = NULL;

	DEBUGMSG( DEBUG_KL_AllocPhysMem, ( "KL_AllocPhysMem entry,dwSize=0x%x.\r\n", dwSize ) );

	KL_EnterCriticalSection( &csVirtualMem );
	// �õ�������ҳ
	p = Page_GetContiguous( dwPageSize / PAGE_SIZE, dwAlignmentMask );
	if( p )
	{	//
	    DWORD dwPhysicalAddress = _GetPhysicalPageAdr( (DWORD)p ); // �õ�p��Ӧ�������ַ
		//��pӳ�䵽�û��ռ�
		lpVAdr = DoVirtualAlloc( NULL, 
								dwPageSize, 
								MEM_COMMIT | MEM_RESERVE | MEM_PHYSICAL | MEM_CONTIGUOUS, 
								fdwProtect | PAGE_NOCACHE, 
								(LPVOID)dwPhysicalAddress );
        if( lpVAdr )
		{
			*pPhysicalAddress = dwPhysicalAddress;
		}
		else
		{	//ʧ�ܡ�
			Page_ReleaseContiguous( p, dwPageSize / PAGE_SIZE );
		}
	}

	KL_LeaveCriticalSection( &csVirtualMem );
	return lpVAdr; 
}

// ********************************************************************
// ������BOOL WINAPI KL_FreePhysMem( LPVOID lpvAddress )
// ������
//		IN lpvAddress - ���ַ����KL_AllocPhysMem��������ַ��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�ͷ�֮ǰ�� KL_AllocPhysMem ����ĵ�ַ
// ���ã�
//		ϵͳAPI
// ********************************************************************
#define DEBUG_KL_FreePhysMem 0
BOOL WINAPI KL_FreePhysMem( LPVOID lpvAddress )
{
	DEBUGMSG( DEBUG_KL_FreePhysMem, ( "KL_FreePhysMem entry,lpvAddress=0x%x.\r\n", lpvAddress ) );
	return DoVirtualFree( lpvAddress, 0, MEM_RELEASE );
}



