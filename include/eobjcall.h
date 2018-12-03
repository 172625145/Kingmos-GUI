/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EOBJCALL_H
#define __EOBJCALL_H

#ifndef __EUCORE_H
#include <eucore.h>
#endif

#ifndef __EAPISRV_H
#include <eapisrv.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

// �ı�ýṹ������ͬʱ�ı�swi.s�� ImplementCallBack��ImplementCallBack4
typedef struct _CALLBACKDATA
{
	HANDLE  hProcess;
	FARPROC lpfn;
	DWORD   dwArg0;
}CALLBACKDATA, FAR * LPCALLBACKDATA;

#ifndef INLINE_PROGRAM
//typedef LRESULT (* PSYS_IMPLEMENTCALLBACK )( LPCALLBACKDATA, ... );
//#define Sys_ImplementCallBack  ( (PSYS_IMPLEMENTCALLBACK)CALL_API(API_KERNEL, SYS_IMPLEMENTCALLBACK, MAX_ARGS ) )
// max 4 param to call
//#define Sys_ImplementCallBack4  ( (PSYS_IMPLEMENTCALLBACK)CALL_API(API_KERNEL, SYS_IMPLEMENTCALLBACK4, 4) )
LRESULT Sys_ImplementCallBack( LPCALLBACKDATA, ... );
LRESULT Sys_ImplementCallBack4( LPCALLBACKDATA, ... );

#else
#define Sys_ImplementCallBack KL_ImplementCallBack
LRESULT KL_ImplementCallBack( LPCALLBACKDATA, ... );
// max 4 param to call
#define Sys_ImplementCallBack4 KL_ImplementCallBack4
LRESULT KL_ImplementCallBack4( LPCALLBACKDATA, ... );
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif   //__EOBJLIST_H

