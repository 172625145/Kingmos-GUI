/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __EPACK_H
#define __EPACK_H

#include "ewindows.h"

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

int UnPack( LPBYTE Src, int SrcLen,LPBYTE Obj, int ObjMaxLen);
int Pack( LPBYTE Src, int SrcLen,LPBYTE Obj, int ObjMaxLen);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif     //__EPACK_H

