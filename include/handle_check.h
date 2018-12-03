/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _HANDLE_CHK_2004_H_
#define _HANDLE_CHK_2004_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//数组的个数
#define	ARR_COUNT( arr )			(sizeof(arr)/sizeof(arr[0]))


//句柄的安全操作
#define	HANDLE_THIS(type)			struct type*	this
#define	HANDLE_ALLOC(nsize)			malloc(nsize)
#define	HANDLE_INIT(pp, nsize)		memset(pp, 0, nsize); pp->this = pp;
#define	HANDLE_SET(pp)				pp->this = pp
#define	HANDLE_FREE(pp)				pp->this = 0; free(pp);
#define	HANDLE_RESET(pp)			pp->this = 0

#define	HANDLE_CHECK(pp)			( pp && (pp==pp->this) )
#define	HANDLE_F_OK(pp, wF)			( pp && (pp==pp->this) && !(pp->wFlag & wF) )
#define	HANDLE_F_FAIL(pp, wF)		( (pp==NULL) || (pp!=pp->this) || (pp->wFlag & wF) )

//
#define	PTR_THIS					LPVOID	this
#define	PTR_CHECK(pp)				( pp && (pp==pp->this) )
#define	PTR_NEXT(type)				struct	type*	pNext
#define	PTR_PREV(type)				struct	type*	pPrev
#define	PTR_GETNEXT(pp)				( (pp)->pNext )
#define	PTR_GETPREV(pp)				( (pp)->pPrev )


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_HANDLE_CHK_2004_H_
