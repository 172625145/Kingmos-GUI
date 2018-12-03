/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef _EPTRLIST_H
#define _EPTRLIST_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分


#ifndef __EDEF_H
#include <edef.h>
#endif

typedef int ccIndex ;
struct PtrListStruct {
    void **items;
    ccIndex count;
    ccIndex limit;
    ccIndex delta;
    BOOL shouldDelete;
};
typedef struct PtrListStruct PTRLIST;
typedef struct PtrListStruct * LPPTRLIST;
//typedef LPPTRLIST HIMAGELIST;

typedef BOOL (*ccTestFunc)(void *,void *);
typedef void (*ccAppFunc)(void *,void *);

#define maxCollectionSize 0x7fff

extern void PtrListCreate(LPPTRLIST lpPtrList, ccIndex aLimit, ccIndex aDelta ) ;
extern void PtrListDestroy(LPPTRLIST lpPtrList);
extern void PtrListShutDown(LPPTRLIST lpPtrList);
extern void *PtrListAt(LPPTRLIST lpPtrList, ccIndex index );
extern void PtrListAtRemove( LPPTRLIST lpPtrList,ccIndex index );
extern void PtrListAtFree( LPPTRLIST lpPtrList,ccIndex index );
extern BOOL PtrListAtInsert(LPPTRLIST lpPtrList,ccIndex index, void *item);
extern void PtrListAtPut( LPPTRLIST lpPtrList,ccIndex index, void *item );
extern void PtrListRemove( LPPTRLIST lpPtrList,void *item );
extern void PtrListRemoveAll(LPPTRLIST lpPtrList);
extern void *PtrListfirstThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg );
extern void *PtrListlastThat( LPPTRLIST lpPtrList,ccTestFunc Test, void *arg );
extern void PtrListforEach( LPPTRLIST lpPtrList,ccAppFunc action, void *arg );
extern void PtrListFree( LPPTRLIST lpPtrList,void *item );
extern void PtrListFreeAll(LPPTRLIST lpPtrList);
extern void PtrListFreeItem( LPPTRLIST lpPtrList,void *item );
extern ccIndex PtrListIndexOf(LPPTRLIST lpPtrList,void *item);
extern BOOL PtrListInsert( LPPTRLIST lpPtrList,void *item );
extern void PtrListPack(LPPTRLIST lpPtrList);
extern void PtrListSetLimit(LPPTRLIST lpPtrList,ccIndex aLimit);


#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //_EPTRLIST_H

