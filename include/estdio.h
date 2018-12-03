/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ESTDIO_H
#define __ESTDIO_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _FILE
{
	int fileno;
}FILE;

#define stdin	((FILE*)0)
#define stdout	((FILE*)1)
#define stderr  ((FILE*)2)

#define	START_FILE_NO	stderr

//extern void EdbgOutputDebugString(const unsigned char *lpsz, ...);
//#define fprintf(cont,printf_exp)   EdbgOutputDebugString (printf_exp)

int std_fprintf(FILE * stream, const char * format, ...);
#define	fprintf std_fprintf

//void * std_calloc( size_t num, size_t size );
//#define calloc std_calloc


#ifdef  __cplusplus
}
#endif


#endif  /* _INC_STDIO */
