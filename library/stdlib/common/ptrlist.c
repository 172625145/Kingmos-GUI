/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：指针数组处理
版本号：1.0.0.456
开发时期：2002-03-29
作者：JAMI CHEN 陈建明
修改记录：
**************************************************/
//#include "stdafx.h"
#include "eframe.h"
#include "eptrlist.h"
#include "ealloc.h"
#include "eassert.h"

static void PtrListerror(void);

// **************************************************
// 声明：void PtrListCreate(LPPTRLIST lpPtrList, ccIndex aLimit, ccIndex aDelta ) 
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
//	 IN aLimit -- 初始化的最大数组个数
//	 IN aDelta -- 当指针个数超过最大值后自动增长的大小个数。
// 
// 返回值：无
// 功能描述：创建指针列表。
// 引用: 
// **************************************************
void PtrListCreate(LPPTRLIST lpPtrList, ccIndex aLimit, ccIndex aDelta ) 
{
    lpPtrList->count= 0 ;
    lpPtrList->items= 0 ,
    lpPtrList->limit=0;
    lpPtrList->delta= aDelta ; // 设置增长增量
    lpPtrList->shouldDelete= TRUE ;
    PtrListSetLimit(lpPtrList,aLimit ); // 设置当前的指针数组个数限制
}


// **************************************************
// 声明：void PtrListDestroy(LPPTRLIST lpPtrList)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 
// 返回值：无
// 功能描述：破坏指定的指针数组。
// 引用: 
// **************************************************
void PtrListDestroy(LPPTRLIST lpPtrList)
{
		if (lpPtrList->items)
		{
			//_HeapCheck();
		    free(lpPtrList->items); // 释放指针数组
			//_HeapCheck();
		}
}

// **************************************************
// 声明：void PtrListShutDown(LPPTRLIST lpPtrList)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 
// 返回值：无
// 功能描述：关闭该指针数组
// 引用: 
// **************************************************
void PtrListShutDown(LPPTRLIST lpPtrList)
{
    if( lpPtrList->shouldDelete ) // 是否需要删除数据
        PtrListFreeAll(lpPtrList); // 释放所有条目
    PtrListSetLimit(lpPtrList,0); // 设置当前数组的限制为0
}

// **************************************************
// 声明：void *PtrListAt(LPPTRLIST lpPtrList, ccIndex index )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN index -- 要得到指针的索引
// 
// 返回值：返回指定索引的指针值。
// 功能描述：得到指定索引的指针值。
// 引用: 
// **************************************************
void *PtrListAt(LPPTRLIST lpPtrList, ccIndex index )
{
    if( index < 0 || index >= lpPtrList->count )
	{ // 该索引无效
        //PtrListerror();
			return NULL; 
	}
	if (lpPtrList->items)
  	  return lpPtrList->items[index]; // 返回指定索引的指针
	return NULL;
}

// **************************************************
// 声明：void PtrListAtRemove( LPPTRLIST lpPtrList,ccIndex index )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN index -- 要删除指针的索引
// 
// 返回值：无
// 功能描述：从数组中删除指定索引的指针。
// 引用: 
// **************************************************
void PtrListAtRemove( LPPTRLIST lpPtrList,ccIndex index )
{
    if( index < 0 || index >= lpPtrList->count )
		{ // 索引无效
        PtrListerror();
				return;
		}
		if (lpPtrList->items==NULL) // 数组没有数据
			return ;
    lpPtrList->count--; // 删除一个指针
    // 将其后面的指针移到当前的索引位置
    memmove( &lpPtrList->items[index], &lpPtrList->items[index+1], (lpPtrList->count-index)*sizeof(void *) );
}

// **************************************************
// 声明：void PtrListAtFree( LPPTRLIST lpPtrList,ccIndex index )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN index -- 要释放指针的索引
// 
// 返回值：无
// 功能描述：释放指定索引的指针
// 引用: 
// **************************************************
void PtrListAtFree( LPPTRLIST lpPtrList,ccIndex index )
{
    void *item = PtrListAt(lpPtrList, index ); // 得到指定索引的指针
    PtrListAtRemove(lpPtrList, index ); // 删除该指针
    PtrListFreeItem(lpPtrList, item ); // 释放该指针
}

// **************************************************
// 声明：BOOL PtrListAtInsert(LPPTRLIST lpPtrList,ccIndex index, void *item)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN index -- 要插入指针的索引
// 	IN item -- 要插入的指针
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：在指定索引位置插入一个指针。
// 引用: 
// **************************************************
BOOL PtrListAtInsert(LPPTRLIST lpPtrList,ccIndex index, void *item)
{
    if( index < 0 )
		{ // 指定索引无效
        PtrListerror();
				return FALSE;
		}
    if( lpPtrList->count == lpPtrList->limit )
        PtrListSetLimit(lpPtrList,lpPtrList->count + lpPtrList->delta); // 超过最大值，重设最大限制

		if (lpPtrList->items==NULL) // 数据无效
				return FALSE;
    memmove( &lpPtrList->items[index+1], &lpPtrList->items[index], (lpPtrList->count-index)*sizeof(void *) ); // 将指定位置的内容向后移动一个位置
    lpPtrList->count++; // 添加一个数据

    lpPtrList->items[index] = item; // 设置要插入的指针

		return TRUE;
}

// **************************************************
// 声明：void PtrListAtPut( LPPTRLIST lpPtrList,ccIndex index, void *item )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN index -- 要设置指针的索引
// 	IN item -- 要设置的指针
// 
// 返回值：无
// 功能描述：设置指定的索引的指针值。
// 引用: 
// **************************************************
void PtrListAtPut( LPPTRLIST lpPtrList,ccIndex index, void *item )
{
    if( index >= lpPtrList->count )
		{ // 索引无效
        PtrListerror();
				return ;
		}
		if (lpPtrList->items)
			lpPtrList->items[index] = item; // 设定指定索引的指针
}

// **************************************************
// 声明：void PtrListRemove( LPPTRLIST lpPtrList,void *item )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN item -- 要删除的指针
// 
// 返回值：无
// 功能描述：删除指定的指针。
// 引用: 
// **************************************************
void PtrListRemove( LPPTRLIST lpPtrList,void *item )
{
	if (item)
	  PtrListAtRemove( lpPtrList,PtrListIndexOf(lpPtrList,item) ); // 删除该指针的条目
}

// **************************************************
// 声明：void PtrListRemoveAll(LPPTRLIST lpPtrList)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 
// 返回值：无
// 功能描述：删除所有的数据。
// 引用: 
// **************************************************
void PtrListRemoveAll(LPPTRLIST lpPtrList)
{
    lpPtrList->count = 0; // 设置当前的数据个数是0
}

// **************************************************
// 声明：void PtrListerror(void)
// 参数：
// 	无
// 返回值：无
// 功能描述：指针数组设置错误。
// 引用: 
// **************************************************
void PtrListerror(void)
{
    ASSERT( 0 );
}

// **************************************************
// 声明：void *PtrListfirstThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN Test -- 要测试的函数
// 	IN arg -- 测试参数
// 
// 返回值：返回第一个你所需要的指针
// 功能描述：得到第一个测试成功的指针。
// 引用: 
// **************************************************
void *PtrListfirstThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
{
	ccIndex i ;
		if (lpPtrList->items==NULL)
			return 0;
    for( i = 0; i < lpPtrList->count; i++ )
        {
        if( Test( lpPtrList->items[i], arg ) == TRUE ) // 调用测试函数
            return lpPtrList->items[i]; // 返回需要的指针
        }
    return 0;
}

// **************************************************
// 声明：void *PtrListlastThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN Test -- 要测试的函数
// 	IN arg -- 测试参数
// 
// 返回值：返回最后一个需要的指针
// 功能描述：得到最后一个满足测试的指针
// 引用: 
// **************************************************
void *PtrListlastThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg )
{
	ccIndex i ;
		if (lpPtrList->items==NULL)
			return 0;
    for( i = lpPtrList->count; i > 0; i-- )
        {
        if( Test( lpPtrList->items[i-1], arg ) == TRUE ) // 调用测试函数
            return lpPtrList->items[i-1]; // 返回满足条件的指针
        }
    return 0;
}

// **************************************************
// 声明：void PtrListforEach( LPPTRLIST lpPtrList,ccAppFunc action, void *arg )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN action -- 要测试的函数
// 	IN arg -- 测试参数
// 
// 返回值：无
// 功能描述：测试所有的指针
// 引用: 
// **************************************************
void PtrListforEach( LPPTRLIST lpPtrList,ccAppFunc action, void *arg )
{
	ccIndex i ;
		if (lpPtrList->items==NULL)
				return;
    for( i = 0; i < lpPtrList->count; i++ )
        action( lpPtrList->items[i], arg ); // 调用测试函数
}

// **************************************************
// 声明：void PtrListFree( LPPTRLIST lpPtrList,void *item )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN item -- 要释放的指针
// 
// 返回值：无
// 功能描述：释放指定的条目
// 引用: 
// **************************************************
void PtrListFree( LPPTRLIST lpPtrList,void *item )
{
    PtrListRemove( lpPtrList,item ); // 删除指定的指针
    PtrListFreeItem( lpPtrList,item ); // 释放该指针
}

// **************************************************
// 声明：void PtrListFreeAll(LPPTRLIST lpPtrList)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 
// 返回值：无
// 功能描述：释放所有的指针。
// 引用: 
// **************************************************
void PtrListFreeAll(LPPTRLIST lpPtrList)
{
	ccIndex i ;
    for( i =  0; i < lpPtrList->count; i++ )
        PtrListFreeItem( lpPtrList,PtrListAt(lpPtrList,i) ); // 释放所有的指针
    lpPtrList->count = 0; // 设置当前的数据个数为0
}

// **************************************************
// 声明：void PtrListFreeItem( LPPTRLIST lpPtrList,void *item )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN item -- 要释放的指针
// 
// 返回值：无
// 功能描述：释放指定的指针。
// 引用: 
// **************************************************
void PtrListFreeItem( LPPTRLIST lpPtrList,void *item )
{
		if (item)
			free(item); // 释放指针
}

// **************************************************
// 声明：ccIndex PtrListIndexOf(LPPTRLIST lpPtrList,void *item)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN item -- 要得到索引的指针
// 
// 返回值：返回指定指针的索引
// 功能描述：得到指定指针的索引。
// 引用: 
// **************************************************
ccIndex PtrListIndexOf(LPPTRLIST lpPtrList,void *item)
{
	ccIndex i ;
	
		if (lpPtrList->items==NULL)
			return -1;
	  for( i = 0; i < lpPtrList->count; i++ )
        if( item == lpPtrList->items[i] ) // 查找指定指针的索引
            return i;
		// 没有找到
       PtrListerror();
		return -1;
}

// **************************************************
// 声明：BOOL PtrListInsert( LPPTRLIST lpPtrList,void *item )
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 	IN item -- 要释放的指针
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：插入一个指针到最后。
// 引用: 
// **************************************************
BOOL PtrListInsert( LPPTRLIST lpPtrList,void *item )
{
//    ccIndex loc = lpPtrList->count;
    return PtrListAtInsert(lpPtrList, lpPtrList->count, item ); // 插入一个指针到数组末尾
}

// **************************************************
// 声明：void PtrListPack(LPPTRLIST lpPtrList)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
// 
// 返回值：无
// 功能描述：压缩空指针。
// 引用: 
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
        { // 如果是空指针，则不要该指针
        if( *curSrc != 0 )
            *curDst++ = *curSrc;
        curSrc++; 
        }
}

// **************************************************
// 声明：void PtrListSetLimit(LPPTRLIST lpPtrList,ccIndex aLimit)
// 参数：
// 	IN lpPtrList -- 一个指向PTRLIST结构的指针
//	 IN aLimit -- 要设置的最大限制
// 
// 返回值：无
// 功能描述：设置数组的最大个数限制
// 引用: 
// **************************************************
void PtrListSetLimit(LPPTRLIST lpPtrList,ccIndex aLimit)
{
    if( aLimit < lpPtrList->count )
        aLimit =  lpPtrList->count;
    if( aLimit > maxCollectionSize)
        aLimit = maxCollectionSize;
    if( aLimit != lpPtrList->limit )
        { // 要设置的最大限制与当前的最大限制不同，要重新设置
					void **aItems;
					if (aLimit == 0 )
					{ // 设置为0，即释放当前的指针
							if (lpPtrList->items)
								free(lpPtrList->items);
							lpPtrList->items =  0;
							lpPtrList->limit =  aLimit;
    			} 
					else
					{ // 重新分配数组的大小
							if (lpPtrList->items)
							{ // 原来有数据
								aItems = realloc(lpPtrList->items,aLimit*sizeof(void *)); // 重新分配
							}
							else
							{ // 原来没数据
								aItems = malloc(aLimit*sizeof(void *)); // 分配指定大小的数据
							}
							if( aItems)
							{ // 分配成功
								lpPtrList->items =  aItems;
								lpPtrList->limit =  aLimit;
							}
					}
        }
}




