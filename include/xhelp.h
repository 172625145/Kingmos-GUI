/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _XHELP_2004_03_03_
#define _XHELP_2004_03_03_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ----------------------------------------------------------
// 辅助功能宏 定义
// ----------------------------------------------------------

//数组的个数
#ifndef	_DEF_HELP_ARR_COUNT
#define	ARR_COUNT( arr )			(sizeof(arr)/sizeof(arr[0]))
#endif	//_DEF_HLP_ARR_COUNT

//矩形宽度
#define	RECT_CX( prt )				((prt)->right - (prt)->left)
#define	RECT_CY( prt )				((prt)->bottom - (prt)->top)

//句柄的安全操作
#ifndef	_DEF_HELP_HANDLE
#define	HANDLE_THIS(type)			struct type*	this
#define	HANDLE_CHECK(pp)			( pp && pp==pp->this )
#define	HANDLE_ALLOC(nsize)			malloc(nsize)
#define	HANDLE_INIT(pp, nsize)		memset(pp, 0, nsize); pp->this = pp;
#define	HANDLE_SET(pp)				pp->this = pp
#define	HANDLE_FREE(pp)				pp->this = 0; free(pp);
#endif	//_DEF_HELP_HANDLE

//函数特征
#ifndef	_DEF_HELP_FUN
#define	IN
#define	OUT
#define	OPTION
#define	OPTIONAL
#endif	//_DEF_HELP_FUN

//指针
#ifndef	_DEF_HELP_PTR
#define	PTR_THIS					LPVOID	this
#define	PTR_CHECK(pp)				( pp && pp==pp->this )
#define	PTR_NEXT(type)				struct	type*	pNext
#define	PTR_PREV(type)				struct	type*	pPrev
#define	PTR_GETNEXT(pp)				( (pp)->pNext )
#define	PTR_GETPREV(pp)				( (pp)->pPrev )
#endif	//_DEF_HELP_PTR

//




#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_XHELP_2004_03_03_
