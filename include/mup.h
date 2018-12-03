/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _MEM_MUP_H_
#define _MEM_MUP_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ----------------------------------------------------------
// 内存单元链表
// ----------------------------------------------------------

//
typedef	HANDLE				HPOOL;

#define	MAX_TRYFIRST		1000
#define	MAX_TRYEVERY		1000

#define	WAIT_TRYFIRST		100
#define	WAIT_TRYEVERY		100

//操作
extern	HPOOL	MUP_AllocPool( WORD wCntMax, WORD wLenUnit, BOOL fIsClearUnit, WORD wTryEvery, WORD wTryFirst );
extern	void	MUP_FreePool( HPOOL hMUP );
extern	LPVOID	MUP_AllocUnit( HPOOL hMUP );
extern	void	MUP_FreeUnit( LPVOID pBufUnit );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_MEM_MUP_H_
