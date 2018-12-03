/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：
版本号：  1.0.0
开发时期：2003-03-06
作者：    周兵
修改记录：
******************************************************/


#ifndef _KINGMOS_INC_STDDEF
#define _KINGMOS_INC_STDDEF

#ifdef  __cplusplus
extern "C" {
#endif



/* Define NULL pointer value and the offset() macro */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#define offsetof(s,m)   (size_t)&(((s *)0)->m)


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_STDDEF */
