/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef __ESTDARG_H
#define __ESTDARG_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef char *  va_list;

// align to int
#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#define va_arg(ap,type)    ( *(type *)((ap += _INTSIZEOF(type)) - _INTSIZEOF(type)) )
#define va_end(ap)      ( ap = (va_list)0 )

#ifdef  __cplusplus
}
#endif


#endif  //__ESTDARG_H
