/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _XYG_DBG_OUT_H_
#define _XYG_DBG_OUT_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//#define DEBUG_CANCEL_XYGDBGOUT

#ifndef DEBUG_CANCEL_XYGDBGOUT

#if defined(XYG_PC_PRJ) || defined(DEBUG_XYGDBGOUT)
extern	void	XygDbgOutToEdit( LPCTSTR lpszFormat, ... );
#define XygDbgOut(cond,printf_exp)   ((cond)?(XygDbgOutToEdit printf_exp),1:0)
#else
extern	void EdbgOutputDebugString(const char *lpsz, ...);
#define XygDbgOut(cond,printf_exp)   ((cond)?(EdbgOutputDebugString printf_exp),1:0)
#endif

#else

#define XygDbgOut(cond,printf_exp)

#endif


	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_XYG_DBG_OUT_H_
