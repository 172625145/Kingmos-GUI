/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_FRAGMENT_H_
#define	_FRAGMENT_H_

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef	struct	_FRAGMENT
{
	LPBYTE		pData;
	LONG		nLen;

} FRAGMENT;

//˳��ķ�Ƭ����
typedef	struct	_FRAGSETA
{
	//DWORD		dwBytes;
	LONG		nCnt;
	FRAGMENT	pFrag[1];

} FRAGSETA;

extern	BOOL	FgA_CopyToBuf( FRAGSETA* pFragSetA, OUT LPBYTE pData );

//����ķ�Ƭ����
typedef	struct	_FRAGSETD
{
	//LONG		nCnt;
	DWORD		dwBytes;
	LONG		nCntAll;
	LONG		nCntUsed;
	FRAGMENT	pFrag[1];

} FRAGSETD;

extern	BOOL	FgD_CopyToBuf( FRAGSETD* pFragSetD, OUT LPBYTE pData );


#ifdef __cplusplus
}	
#endif

#endif	//_FRAGMENT_H_
