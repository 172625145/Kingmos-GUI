/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EOBJTYPE_H
#define __EOBJTYPE_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define OBJ_READ_DISABLE     0x80000000L
#define OBJ_WRITE_DISABLE    0x40000000L
#define OBJ_FREE_DISABLE     0x20000000L

#define GET_OBJ_TYPE( pobj ) ( *( (DWORD*)(pobj) ) )

#define OBJ_NULL            0
#define OBJ_PEN             1
#define OBJ_BRUSH           2
#define OBJ_DC              3
#define OBJ_METADC          4
#define OBJ_PAL             5
#define OBJ_FONT            6
#define OBJ_BITMAP          7
#define OBJ_REGION          8
#define OBJ_METAFILE        9
#define OBJ_MEMDC           10
#define OBJ_ICON            11
#define OBJ_WINDOW          12
#define OBJ_MENU            13 
#define OBJ_CLASS           14


#define OBJ_DEV             50
#define OBJ_DEVOPEN         51

#define OBJ_PROCESS         100
#define OBJ_THREAD          101
#define OBJ_SEMAPHORE       102
#define OBJ_EVENT           103
#define OBJ_MUTEX           104 
#define OBJ_MODULE          105


#define OBJ_APPLICATION     152

#define OBJ_FILE            200
#define OBJ_FINDFILE        201

#define OBJ_TABLE           300
#define OBJ_FINDTABLE       301

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EOBJTYPE_H
