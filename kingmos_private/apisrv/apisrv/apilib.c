/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <edef.h>
#include <ecore.h>
#include <eapisrv.h>

#ifdef CALL_TRAP

HANDLE WINAPI API_Register( UINT uiServerId, const PFNVOID * lpfn, UINT uiOptions )
{
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_APISRV, API_REGISTER, 3 );
    cs.arg0 = (DWORD)uiServerId;
	return (HANDLE)CALL_SERVER( &cs, lpfn, uiOptions );
} 

HANDLE WINAPI API_RegisterEx( UINT uiServerId, const PFNVOID * lpfn, const DWORD * lpdwArgs, UINT uiOptions )
{
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_APISRV, API_REGISTEREX, 4 );
    cs.arg0 = (DWORD)uiServerId;
	return (HANDLE)CALL_SERVER( &cs, lpfn, lpdwArgs, uiOptions );
}

BOOL WINAPI API_Unregister( HANDLE hAPI )
{
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_APISRV, API_UNREGISTER, 1 );
    cs.arg0 = (DWORD)hAPI;
	return CALL_SERVER( &cs );
}

BOOL WINAPI API_IsReady( UINT uiAPI )
{
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_APISRV, API_ISREADY, 1 );
    cs.arg0 = (DWORD)uiAPI;
	return CALL_SERVER( &cs );
}

BOOL WINAPI API_SetReady( UINT uiAPI )
{
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_APISRV, API_SETREADY, 1 );
    cs.arg0 = (DWORD)uiAPI;
	return CALL_SERVER( &cs );
//	return ( (PAPI_SETREADY)CALL_API( API_APISRV, API_SETREADY, 1 ) )( uiAPI );
}

#else
// static link
HANDLE WINAPI KL_RegisterAPI( UINT uiAPIId, const PFNVOID * lpfn, UINT uiOptions );
HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, const PFNVOID * lpfn, const DWORD * lpArgs, UINT uiOptions );

BOOL WINAPI KL_UnregisterAPI( HANDLE hAPI );
BOOL WINAPI KL_IsAPIReady( UINT uiAPI );
BOOL WINAPI KL_SetAPIReady( UINT uiAPI );
BOOL WINAPI KL_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle );
BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs );
void WINAPI KL_LeaveAPI( void );

const PAPI_REGISTER API_Register = KL_RegisterAPI;
const PAPI_REGISTEREX API_RegisterEx = KL_RegisterAPIEx;
const PAPI_UNREGISTER API_Unregister = KL_UnregisterAPI;
const PAPI_ENTER API_Enter = KL_EnterAPI;
const PAPI_ENTERHANDLE API_EnterHandle = KL_EnterAPIHandle;
const PAPI_LEAVE API_Leave = KL_LeaveAPI;
const PAPI_ISREADY API_IsReady = KL_IsAPIReady;
const PAPI_SETREADY API_SetReady = KL_SetAPIReady;


#endif


