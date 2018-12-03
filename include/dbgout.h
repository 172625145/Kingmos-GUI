/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _DBG_OUT_XYG_H_
#define _DBG_OUT_XYG_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//#define DEBUG_DBGOUT
#ifdef DEBUG_DBGOUT
extern	void	DebugOut( LPCTSTR lpszFormat, ... );
#else
extern	void EdbgOutputDebugString(const char *lpsz, ...);
#define DebugOut(printf_exp)   ((EdbgOutputDebugString printf_exp),1)
#endif

	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_DBG_OUT_XYG_H_
