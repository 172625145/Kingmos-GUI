/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef _MEM_MUP_H_
#define _MEM_MUP_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ----------------------------------------------------------
// �ڴ浥Ԫ����
// ----------------------------------------------------------

//
typedef	HANDLE				HPOOL;

#define	MAX_TRYFIRST		1000
#define	MAX_TRYEVERY		1000

#define	WAIT_TRYFIRST		100
#define	WAIT_TRYEVERY		100

//����
extern	HPOOL	MUP_AllocPool( WORD wCntMax, WORD wLenUnit, BOOL fIsClearUnit, WORD wTryEvery, WORD wTryFirst );
extern	void	MUP_FreePool( HPOOL hMUP );
extern	LPVOID	MUP_AllocUnit( HPOOL hMUP );
extern	void	MUP_FreeUnit( LPVOID pBufUnit );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_MEM_MUP_H_
