/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
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

