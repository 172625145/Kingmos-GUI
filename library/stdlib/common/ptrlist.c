/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����ָ�����鴦��
�汾�ţ�1.0.0.456
����ʱ�ڣ�2002-03-29
���ߣ�JAMI CHEN �½���
�޸ļ�¼��
**************************************************/
//#include "stdafx.h"
#include "eframe.h"
#include "eptrlist.h"
#include "ealloc.h"
#include "eassert.h"

static void PtrListerror(void);

// **************************************************
// ������void PtrListCreate(LPPTRLIST lpPtrList, ccIndex aLimit, ccIndex aDelta ) 
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
//	 IN aLimit -- ��ʼ��������������
//	 IN aDelta -- ��ָ������������ֵ���Զ������Ĵ�С������
// 
// ����ֵ����
// ��������������ָ���б�
// ����: 
// **************************************************
void PtrListCreate(LPPTRLIST lpPtrList, ccIndex aLimit, ccIndex aDelta ) 
{
    lpPtrList->count= 0 ;
    lpPtrList->items= 0 ,
    lpPtrList->limit=0;
    lpPtrList->delta= aDelta ; // ������������
    lpPtrList->shouldDelete= TRUE ;
    PtrListSetLimit(lpPtrList,aLimit ); // ���õ�ǰ��ָ�������������
}


// **************************************************
// ������void PtrListDestroy(LPPTRLIST lpPtrList)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 
// ����ֵ����
// �����������ƻ�ָ����ָ�����顣
// ����: 
// **************************************************
void PtrListDestroy(LPPTRLIST lpPtrList)
{
		if (lpPtrList->items)
		{
			//_HeapCheck();
		    free(lpPtrList->items); // �ͷ�ָ������
			//_HeapCheck();
		}
}

// **************************************************
// ������void PtrListShutDown(LPPTRLIST lpPtrList)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 
// ����ֵ����
// �����������رո�ָ������
// ����: 
// **************************************************
void PtrListShutDown(LPPTRLIST lpPtrList)
{
    if( lpPtrList->shouldDelete ) // �Ƿ���Ҫɾ������
        PtrListFreeAll(lpPtrList); // �ͷ�������Ŀ
    PtrListSetLimit(lpPtrList,0); // ���õ�ǰ���������Ϊ0
}

// **************************************************
// ������void *PtrListAt(LPPTRLIST lpPtrList, ccIndex index )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN index -- Ҫ�õ�ָ�������
// 
// ����ֵ������ָ��������ָ��ֵ��
// �����������õ�ָ��������ָ��ֵ��
// ����: 
// **************************************************
void *PtrListAt(LPPTRLIST lpPtrList, ccIndex index )
{
    if( index < 0 || index >= lpPtrList->count )
	{ // ��������Ч
        //PtrListerror();
			return NULL; 
	}
	if (lpPtrList->items)
  	  return lpPtrList->items[index]; // ����ָ��������ָ��
	return NULL;
}

// **************************************************
// ������void PtrListAtRemove( LPPTRLIST lpPtrList,ccIndex index )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN index -- Ҫɾ��ָ�������
// 
// ����ֵ����
// ������������������ɾ��ָ��������ָ�롣
// ����: 
// **************************************************
void PtrListAtRemove( LPPTRLIST lpPtrList,ccIndex index )
{
    if( index < 0 || index >= lpPtrList->count )
		{ // ������Ч
        PtrListerror();
				return;
		}
		if (lpPtrList->items==NULL) // ����û������
			return ;
    lpPtrList->count--; // ɾ��һ��ָ��
    // ��������ָ���Ƶ���ǰ������λ��
    memmove( &lpPtrList->items[index], &lpPtrList->items[index+1], (lpPtrList->count-index)*sizeof(void *) );
}

// **************************************************
// ������void PtrListAtFree( LPPTRLIST lpPtrList,ccIndex index )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN index -- Ҫ�ͷ�ָ�������
// 
// ����ֵ����
// �����������ͷ�ָ��������ָ��
// ����: 
// **************************************************
void PtrListAtFree( LPPTRLIST lpPtrList,ccIndex index )
{
    void *item = PtrListAt(lpPtrList, index ); // �õ�ָ��������ָ��
    PtrListAtRemove(lpPtrList, index ); // ɾ����ָ��
    PtrListFreeItem(lpPtrList, item ); // �ͷŸ�ָ��
}

// **************************************************
// ������BOOL PtrListAtInsert(LPPTRLIST lpPtrList,ccIndex index, void *item)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN index -- Ҫ����ָ�������
// 	IN item -- Ҫ�����ָ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ������������ָ������λ�ò���һ��ָ�롣
// ����: 
// **************************************************
BOOL PtrListAtInsert(LPPTRLIST lpPtrList,ccIndex index, void *item)
{
    if( index < 0 )
		{ // ָ��������Ч
        PtrListerror();
				return FALSE;
		}
    if( lpPtrList->count == lpPtrList->limit )
        PtrListSetLimit(lpPtrList,lpPtrList->count + lpPtrList->delta); // �������ֵ�������������

		if (lpPtrList->items==NULL) // ������Ч
				return FALSE;
    memmove( &lpPtrList->items[index+1], &lpPtrList->items[index], (lpPtrList->count-index)*sizeof(void *) ); // ��ָ��λ�õ���������ƶ�һ��λ��
    lpPtrList->count++; // ���һ������

    lpPtrList->items[index] = item; // ����Ҫ�����ָ��

		return TRUE;
}

// **************************************************
// ������void PtrListAtPut( LPPTRLIST lpPtrList,ccIndex index, void *item )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN index -- Ҫ����ָ�������
// 	IN item -- Ҫ���õ�ָ��
// 
// ����ֵ����
// ��������������ָ����������ָ��ֵ��
// ����: 
// **************************************************
void PtrListAtPut( LPPTRLIST lpPtrList,ccIndex index, void *item )
{
    if( index >= lpPtrList->count )
		{ // ������Ч
        PtrListerror();
				return ;
		}
		if (lpPtrList->items)
			lpPtrList->items[index] = item; // �趨ָ��������ָ��
}

// **************************************************
// ������void PtrListRemove( LPPTRLIST lpPtrList,void *item )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN item -- Ҫɾ����ָ��
// 
// ����ֵ����
// ����������ɾ��ָ����ָ�롣
// ����: 
// **************************************************
void PtrListRemove( LPPTRLIST lpPtrList,void *item )
{
	if (item)
	  PtrListAtRemove( lpPtrList,PtrListIndexOf(lpPtrList,item) ); // ɾ����ָ�����Ŀ
}

// **************************************************
// ������void PtrListRemoveAll(LPPTRLIST lpPtrList)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 
// ����ֵ����
// ����������ɾ�����е����ݡ�
// ����: 
// **************************************************
void PtrListRemoveAll(LPPTRLIST lpPtrList)
{
    lpPtrList->count = 0; // ���õ�ǰ�����ݸ�����0
}

// **************************************************
// ������void PtrListerror(void)
// ������
// 	��
// ����ֵ����
// ����������ָ���������ô���
// ����: 
// **************************************************
void PtrListerror(void)
{
    ASSERT( 0 );
}

// **************************************************
// ������void *PtrListfirstThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN Test -- Ҫ���Եĺ���
// 	IN arg -- ���Բ���
// 
// ����ֵ�����ص�һ��������Ҫ��ָ��
// �����������õ���һ�����Գɹ���ָ�롣
// ����: 
// **************************************************
void *PtrListfirstThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
{
	ccIndex i ;
		if (lpPtrList->items==NULL)
			return 0;
    for( i = 0; i < lpPtrList->count; i++ )
        {
        if( Test( lpPtrList->items[i], arg ) == TRUE ) // ���ò��Ժ���
            return lpPtrList->items[i]; // ������Ҫ��ָ��
        }
    return 0;
}

// **************************************************
// ������void *PtrListlastThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN Test -- Ҫ���Եĺ���
// 	IN arg -- ���Բ���
// 
// ����ֵ���������һ����Ҫ��ָ��
// �����������õ����һ��������Ե�ָ��
// ����: 
// **************************************************
void *PtrListlastThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
{
	ccIndex i ;
		if (lpPtrList->items==NULL)
			return 0;
    for( i = lpPtrList->count; i > 0; i-- )
        {
        if( Test( lpPtrList->items[i-1], arg ) == TRUE ) // ���ò��Ժ���
            return lpPtrList->items[i-1]; // ��������������ָ��
        }
    return 0;
}

// **************************************************
// ������void PtrListforEach( LPPTRLIST lpPtrList,ccAppFunc action, void *arg )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN action -- Ҫ���Եĺ���
// 	IN arg -- ���Բ���
// 
// ����ֵ����
// �����������������е�ָ��
// ����: 
// **************************************************
void PtrListforEach( LPPTRLIST lpPtrList,ccAppFunc action, void *arg )
{
	ccIndex i ;
		if (lpPtrList->items==NULL)
				return;
    for( i = 0; i < lpPtrList->count; i++ )
        action( lpPtrList->items[i], arg ); // ���ò��Ժ���
}

// **************************************************
// ������void PtrListFree( LPPTRLIST lpPtrList,void *item )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN item -- Ҫ�ͷŵ�ָ��
// 
// ����ֵ����
// �����������ͷ�ָ������Ŀ
// ����: 
// **************************************************
void PtrListFree( LPPTRLIST lpPtrList,void *item )
{
    PtrListRemove( lpPtrList,item ); // ɾ��ָ����ָ��
    PtrListFreeItem( lpPtrList,item ); // �ͷŸ�ָ��
}

// **************************************************
// ������void PtrListFreeAll(LPPTRLIST lpPtrList)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 
// ����ֵ����
// �����������ͷ����е�ָ�롣
// ����: 
// **************************************************
void PtrListFreeAll(LPPTRLIST lpPtrList)
{
	ccIndex i ;
    for( i =  0; i < lpPtrList->count; i++ )
        PtrListFreeItem( lpPtrList,PtrListAt(lpPtrList,i) ); // �ͷ����е�ָ��
    lpPtrList->count = 0; // ���õ�ǰ�����ݸ���Ϊ0
}

// **************************************************
// ������void PtrListFreeItem( LPPTRLIST lpPtrList,void *item )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN item -- Ҫ�ͷŵ�ָ��
// 
// ����ֵ����
// �����������ͷ�ָ����ָ�롣
// ����: 
// **************************************************
void PtrListFreeItem( LPPTRLIST lpPtrList,void *item )
{
		if (item)
			free(item); // �ͷ�ָ��
}

// **************************************************
// ������ccIndex PtrListIndexOf(LPPTRLIST lpPtrList,void *item)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN item -- Ҫ�õ�������ָ��
// 
// ����ֵ������ָ��ָ�������
// �����������õ�ָ��ָ���������
// ����: 
// **************************************************
ccIndex PtrListIndexOf(LPPTRLIST lpPtrList,void *item)
{
	ccIndex i ;
	
		if (lpPtrList->items==NULL)
			return -1;
	  for( i = 0; i < lpPtrList->count; i++ )
        if( item == lpPtrList->items[i] ) // ����ָ��ָ�������
            return i;
		// û���ҵ�
       PtrListerror();
		return -1;
}

// **************************************************
// ������BOOL PtrListInsert( LPPTRLIST lpPtrList,void *item )
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 	IN item -- Ҫ�ͷŵ�ָ��
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������һ��ָ�뵽���
// ����: 
// **************************************************
BOOL PtrListInsert( LPPTRLIST lpPtrList,void *item )
{
//    ccIndex loc = lpPtrList->count;
    return PtrListAtInsert(lpPtrList, lpPtrList->count, item ); // ����һ��ָ�뵽����ĩβ
}

// **************************************************
// ������void PtrListPack(LPPTRLIST lpPtrList)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
// 
// ����ֵ����
// ����������ѹ����ָ�롣
// ����: 
// **************************************************
void PtrListPack(LPPTRLIST lpPtrList)
{
	void ** curDst,**curSrc ,**last;

		if (lpPtrList->items==NULL)
			return;
		curDst = lpPtrList->items;
    curSrc = lpPtrList->items;
    last = lpPtrList->items + lpPtrList->count;
    while( curSrc < last )
        { // ����ǿ�ָ�룬��Ҫ��ָ��
        if( *curSrc != 0 )
            *curDst++ = *curSrc;
        curSrc++; 
        }
}

// **************************************************
// ������void PtrListSetLimit(LPPTRLIST lpPtrList,ccIndex aLimit)
// ������
// 	IN lpPtrList -- һ��ָ��PTRLIST�ṹ��ָ��
//	 IN aLimit -- Ҫ���õ��������
// 
// ����ֵ����
// �����������������������������
// ����: 
// **************************************************
void PtrListSetLimit(LPPTRLIST lpPtrList,ccIndex aLimit)
{
    if( aLimit < lpPtrList->count )
        aLimit =  lpPtrList->count;
    if( aLimit > maxCollectionSize)
        aLimit = maxCollectionSize;
    if( aLimit != lpPtrList->limit )
        { // Ҫ���õ���������뵱ǰ��������Ʋ�ͬ��Ҫ��������
					void **aItems;
					if (aLimit == 0 )
					{ // ����Ϊ0�����ͷŵ�ǰ��ָ��
							if (lpPtrList->items)
								free(lpPtrList->items);
							lpPtrList->items =  0;
							lpPtrList->limit =  aLimit;
    			} 
					else
					{ // ���·�������Ĵ�С
							if (lpPtrList->items)
							{ // ԭ��������
								aItems = realloc(lpPtrList->items,aLimit*sizeof(void *)); // ���·���
							}
							else
							{ // ԭ��û����
								aItems = malloc(aLimit*sizeof(void *)); // ����ָ����С������
							}
							if( aItems)
							{ // ����ɹ�
								lpPtrList->items =  aItems;
								lpPtrList->limit =  aLimit;
							}
					}
        }
}




