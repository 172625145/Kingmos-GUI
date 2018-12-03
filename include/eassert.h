/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EASSERT_H
#define __EASSERT_H

#ifndef __EDEF_H
#include <edef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define WARN_ZONE  1
#define ERROR_ZONE  1

#ifdef __DEBUG
	void __AssertFail( char *__msg, char *__cond, char *__file, int __line, char * __notify );

	#define ASSERT_NOTIFY( v, notify ) ( (v) ? (void)0 : (void) __AssertFail( \
                    "Assertion failed: %s, file %s, line %d.\r\n", \
                    #v, __FILE__, __LINE__, (notify) ) )
	#define ASSERT( v ) ASSERT_NOTIFY( v, NULL )
	#define _ASSERT ASSERT

	#ifdef EML_DOS
		void __ShowDebugStr( int x, int y, char *lpstr );
		void __ShowDebugValue( int x, int y, int v );
	#endif

	extern int _Heap_Check( char *__file, int __line );
	#define _HeapCheck() _Heap_Check( __FILE__, __LINE__ )

	extern int __CheckAPIHeap( UINT idAPI, char *__file, int __line );
	#define _CheckAPIHeap( idAPI ) __CheckAPIHeap( (idAPI), __FILE__, __LINE__ )


	void EdbgOutputDebugString(const char *lpsz, ...);
	void EdbgOutputWarnString(const char *lpsz, ...);
	#define RETAILMSG(cond,printf_exp)   \
		((cond)?(EdbgOutputDebugString printf_exp),1:0)

	#ifdef VC386
		#define WARNMSG(cond,printf_exp)   \
			((cond)|WARN_ZONE?(EdbgOutputWarnString printf_exp),1:0)
	#define ERRORMSG(cond,printf_exp)   \
			((cond)|ERROR_ZONE?(EdbgOutputWarnString printf_exp),1:0)

	#endif

	#ifdef ARM
		#define WARNMSG(cond,printf_exp)   \
			((cond)|WARN_ZONE?(EdbgOutputDebugString printf_exp),1:0)
		#define ERRORMSG(cond,printf_exp)   \
			((cond)|ERROR_ZONE?(EdbgOutputDebugString printf_exp),1:0)

	#endif

	#ifdef ARM_CPU

		#define WARNMSG(cond,printf_exp)   \
				((cond)|WARN_ZONE?(EdbgOutputDebugString printf_exp),1:0)
		#define ERRORMSG(cond,printf_exp)   \
				((cond)|ERROR_ZONE?(EdbgOutputDebugString printf_exp),1:0)

	#endif


	#define DEBUGMSG(cond,printf_exp)   \
		((cond)?(EdbgOutputDebugString printf_exp),1:0)

#else
    // no define __DEBUG
    #define ASSERT( v )     ((void)0)
    #define _ASSERT( v )    ((void)0)
    #define DEBUGMSG(cond,printf_exp)   ((void)0)
	#define WARNMSG(cond,printf_exp)    ((void)0)
	#define ERRORMSG(cond,printf_exp)   ((void)0)

#endif // __DEBUG

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  // __EASSERT_H

