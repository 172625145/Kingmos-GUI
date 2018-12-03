/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _API_EXE_H_
#define _API_EXE_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus




#ifdef	CALL_TRAP

#define	EXE_API0( ApiID, EApiEnum, PApiType, RetType )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 0 );	\
    cs.arg0 = 0;									\
    return (RetType)CALL_SERVER( &cs );								\
}

#define	EXE_API1_VOID( ApiID, EApiEnum, PApiType, Arg1 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 1 );	\
    cs.arg0 = (DWORD)Arg1;									\
    CALL_SERVER( &cs );								\
}


#define	EXE_API1( ApiID, EApiEnum, PApiType, RetType, Arg1 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 1 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs );				\
}

#define	EXE_API2( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 2 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2 );				\
}

#define	EXE_API2_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 2 );	\
    cs.arg0 = (DWORD)Arg1;									\
    CALL_SERVER( &cs, Arg2 );				\
}

#define	EXE_API3( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 3 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3 );				\
}

#define	EXE_API4( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 4 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4 );				\
}

#define	EXE_API5( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 5 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5 );				\
}

#define	EXE_API6( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 6 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6 );				\
}

#define	EXE_API7( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 7 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 );				\
}

#define	EXE_API8( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 8 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 );				\
}

#define	EXE_API9( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 9 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9 );				\
}

#define	EXE_API10( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 10 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10 );				\
}

#define	EXE_API11( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 11 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11 );				\
}

#define	EXE_API12( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12 )\
{													\
    CALLTRAP	cs;									\
    cs.apiInfo = CALL_API( ApiID, EApiEnum, 12 );	\
    cs.arg0 = (DWORD)Arg1;									\
    return (RetType)CALL_SERVER( &cs, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, Arg10, Arg11, Arg12 );				\
}

#else

#define	EXE_API0( ApiID, EApiEnum, PApiType, RetType )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( );							\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API1( ApiID, EApiEnum, PApiType, RetType, Arg1 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1 );						\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API2( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2 );				\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API3( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2, Arg3 );			\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API4( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2, Arg3, Arg4 );	\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API5( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5 );\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API6( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 );\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API7( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 );\
		API_Leave( );								\
	}												\
	return iRet;									\
}
#define	EXE_API8( ApiID, EApiEnum, PApiType, RetType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	RetType		iRet;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		iRet = pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 );\
		API_Leave( );								\
	}												\
	return iRet;									\
}



#define	EXE_API0_VOID( ApiID, EApiEnum, PApiType )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( );									\
		API_Leave( );								\
	}												\
}
#define	EXE_API1_VOID( ApiID, EApiEnum, PApiType, Arg1 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1 );								\
		API_Leave( );								\
	}												\
}
#define	EXE_API2_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2 );						\
		API_Leave( );								\
	}												\
}
#define	EXE_API3_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2, Arg3 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2, Arg3 );					\
		API_Leave( );								\
	}												\
}
#define	EXE_API4_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2, Arg3, Arg4 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2, Arg3, Arg4 );			\
		API_Leave( );								\
	}												\
}
#define	EXE_API5_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2, Arg3, Arg4, Arg5 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5 );		\
		API_Leave( );								\
	}												\
}
#define	EXE_API6_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6 );\
		API_Leave( );								\
	}												\
}
#define	EXE_API7_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7 );\
		API_Leave( );								\
	}												\
}
#define	EXE_API8_VOID( ApiID, EApiEnum, PApiType, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 )\
{													\
    CALLSTACK	cs;									\
	PApiType	pFnExe;								\
	if( API_Enter( ApiID, EApiEnum, &pFnExe, &cs ) )\
	{												\
		pFnExe( Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8 );\
		API_Leave( );								\
	}												\
}


#endif




#define	CALLBACK_API0( hProc, lpCallProc, RetVal )\
{												\
    CALLBACKDATA	cd;							\
	cd.hProcess = hProc;						\
	cd.lpfn = (FARPROC)lpCallProc;				\
	RetVal = Sys_ImplementCallBack( &cd );		\
}

#define	CALLBACK_API1( hProc, lpCallProc, RetVal, Arg1 )\
{												\
    CALLBACKDATA	cd;							\
	cd.hProcess = hProc;						\
	cd.lpfn = (FARPROC)lpCallProc;				\
	cd.dwArg0 = (DWORD)Arg1;					\
	RetVal = Sys_ImplementCallBack( &cd );		\
}

#define	CALLBACK_API5( hProc, lpCallProc, RetVal, Arg1, Arg2, Arg3, Arg4, Arg5 )\
{												\
    CALLBACKDATA	cd;							\
	cd.hProcess = hProc;						\
	cd.lpfn = (FARPROC)lpCallProc;				\
	cd.dwArg0 = (DWORD)Arg1;					\
	RetVal = Sys_ImplementCallBack( &cd, Arg2, Arg3, Arg4, Arg5 );	\
}


#define	CALLBACK_API5_VOID( hProc, lpCallProc, Arg1, Arg2, Arg3, Arg4, Arg5 )\
{												\
    CALLBACKDATA	cd;							\
	cd.hProcess = hProc;						\
	cd.lpfn = (FARPROC)lpCallProc;				\
	cd.dwArg0 = (DWORD)Arg1;					\
	Sys_ImplementCallBack( &cd, Arg2, Arg3, Arg4, Arg5 );	\
}



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_API_EXE_H_


