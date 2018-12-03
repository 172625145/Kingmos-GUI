/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <efile.h>
#include <eufile.h>
#include <eapisrv.h>

#include <epcore.h>

// registry
typedef LONG ( WINAPI * PREG_CLOSEKEY )( HKEY hKey );
LONG WINAPI Reg_CloseKey( HKEY hKey )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_CLOSEKEY, 1 );
    cs.arg0 = (DWORD)hKey ;
    return (DWORD)CALL_SERVER( &cs );
#else
	PREG_CLOSEKEY pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_CLOSEKEY, &pfn, &cs ) )
	{
		retv = pfn( hKey );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_CREATEKEYEX )( 
							HKEY hKey, 
							LPCTSTR lpSubKey, 
							DWORD Reserved, 
							LPTSTR lpClass, 
							DWORD dwOptions, 
							REGSAM samDesired, 
							LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
							PHKEY phkResult, 
							LPDWORD lpdwDisposition 
							);
LONG WINAPI Reg_CreateKeyEx( 
							HKEY hKey, 
							LPCTSTR lpSubKey, 
							DWORD Reserved, 
							LPTSTR lpClass, 
							DWORD dwOptions, 
							REGSAM samDesired, 
							LPSECURITY_ATTRIBUTES lpSecurityAttributes, 
							PHKEY phkResult, 
							LPDWORD lpdwDisposition 
							)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_CREATEKEYEX, 9 );
    cs.arg0 = (DWORD)
 							hKey;
    return (DWORD)CALL_SERVER( &cs, 
							lpSubKey, 
							Reserved, 
							lpClass, 
							dwOptions, 
							samDesired, 
							lpSecurityAttributes, 
							phkResult, 
							lpdwDisposition );
#else
	PREG_CREATEKEYEX pfn;
	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_CREATEKEYEX, &pfn, &cs ) )
	{
		retv = pfn( hKey,
			        lpSubKey, 
			        Reserved, 
			        lpClass, 
			        dwOptions, 
			        samDesired, 
			        lpSecurityAttributes, 
			        phkResult, 
			        lpdwDisposition );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_DELETEKEY )( HKEY hKey, LPCTSTR lpSubKey );
LONG WINAPI Reg_DeleteKey( HKEY hKey, LPCTSTR lpSubKey )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_DELETEKEY, 2 );
    cs.arg0 = (DWORD)hKey;
    return (DWORD)CALL_SERVER( &cs, lpSubKey );
#else

	PREG_DELETEKEY pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_DELETEKEY, &pfn, &cs ) )
	{
		retv = pfn( hKey, lpSubKey );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_DELETEVALUE )( HKEY hKey, LPCTSTR lpValueName );
LONG WINAPI Reg_DeleteValue( HKEY hKey, LPCTSTR lpValueName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_DELETEVALUE, 2 );
    cs.arg0 = (DWORD)hKey;
    return (DWORD)CALL_SERVER( &cs, lpValueName );
#else

	PREG_DELETEVALUE pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_DELETEVALUE, &pfn, &cs ) )
	{
		retv = pfn( hKey, lpValueName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_ENUMKEYEX )(
						  HKEY hKey, 
						  DWORD dwIndex, 
						  LPTSTR lpName, 
						  LPDWORD lpcbName, 
						  LPDWORD lpReserved, 
						  LPTSTR lpClass, 
						  LPDWORD lpcbClass, 
						  PFILETIME lpftLastWriteTime 
						  );
LONG WINAPI Reg_EnumKeyEx(
						  HKEY hKey, 
						  DWORD dwIndex, 
						  LPTSTR lpName, 
						  LPDWORD lpcbName, 
						  LPDWORD lpReserved, 
						  LPTSTR lpClass, 
						  LPDWORD lpcbClass, 
						  PFILETIME lpftLastWriteTime 
						  )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_ENUMKEYEX, 8 );
    cs.arg0 = (DWORD)						  
		                  hKey;
    return (DWORD)CALL_SERVER( &cs, 
						  dwIndex, 
						  lpName, 
						  lpcbName, 
						  lpReserved, 
						  lpClass, 
						  lpcbClass, 
						  lpftLastWriteTime  );
#else

	PREG_ENUMKEYEX pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_ENUMKEYEX, &pfn, &cs ) )
	{
		retv = pfn( 	  hKey,
			              dwIndex, 
						  lpName, 
						  lpcbName, 
						  lpReserved, 
						  lpClass, 
						  lpcbClass, 
						  lpftLastWriteTime  );

		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_FLUSHKEY )( HKEY hKey );
LONG WINAPI Reg_FlushKey( HKEY hKey )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_FLUSHKEY, 1 );
    cs.arg0 = (DWORD)hKey ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PREG_FLUSHKEY pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_FLUSHKEY, &pfn, &cs ) )
	{
		retv = pfn( hKey );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_ENUMVALUE )( 
				   HKEY hKey, 
				   DWORD dwIndex, 
				   LPTSTR lpValueName, 
				   LPDWORD lpcbValueName, 
				   LPDWORD lpReserved, 
				   LPDWORD lpType, 
				   LPBYTE lpData, 
				   LPDWORD lpcbData 
				   );
LONG WINAPI Reg_EnumValue( 
				   HKEY hKey, 
				   DWORD dwIndex, 
				   LPTSTR lpValueName, 
				   LPDWORD lpcbValueName, 
				   LPDWORD lpReserved, 
				   LPDWORD lpType, 
				   LPBYTE lpData, 
				   LPDWORD lpcbData 
				   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_ENUMVALUE, 8 );
    cs.arg0 = (DWORD)
 				   hKey;
    return (DWORD)CALL_SERVER( &cs, 
				   dwIndex, 
				   lpValueName, 
				   lpcbValueName, 
				   lpReserved, 
				   lpType, 
				   lpData, 
				   lpcbData 
				   );
#else

	PREG_ENUMVALUE pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_ENUMVALUE, &pfn, &cs ) )
	{
		retv = pfn( 
 				   hKey, 
				   dwIndex, 
				   lpValueName, 
				   lpcbValueName, 
				   lpReserved, 
				   lpType, 
				   lpData, 
				   lpcbData 
			      );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_OPENKEYEX )( 
				   HKEY hKey, 
				   LPCTSTR lpSubKey, 
				   DWORD ulOptions, 
				   REGSAM samDesired, 
				   PHKEY phkResult 
				   );
LONG WINAPI Reg_OpenKeyEx( 
				   HKEY hKey, 
				   LPCTSTR lpSubKey, 
				   DWORD ulOptions, 
				   REGSAM samDesired, 
				   PHKEY phkResult 
				   )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_OPENKEYEX, 5 );
    cs.arg0 = (DWORD)
				   hKey;
    return (DWORD)CALL_SERVER( &cs, 
				   lpSubKey, 
				   ulOptions, 
				   samDesired, 
				   phkResult 
				   );
#else

	PREG_OPENKEYEX pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_OPENKEYEX, &pfn, &cs ) )
	{
		retv = pfn( 
				   hKey, 
				   lpSubKey, 
				   ulOptions, 
				   samDesired, 
				   phkResult 
			      );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_QUERYINFOKEY )( 
					  HKEY hKey, 
					  LPTSTR lpClass, 
					  LPDWORD lpcbClass, 
					  LPDWORD lpReserved, 
					  LPDWORD lpcSubKeys, 
					  LPDWORD lpcbMaxSubKeyLen, 
					  LPDWORD lpcbMaxClassLen, 
					  LPDWORD lpcValues, 
					  LPDWORD lpcbMaxValueNameLen, 
					  LPDWORD lpcbMaxValueLen, 
					  LPDWORD lpcbSecurityDescriptor, 
					  PFILETIME lpftLastWriteTime 
					  );
LONG WINAPI Reg_QueryInfoKey( 
					  HKEY hKey, 
					  LPTSTR lpClass, 
					  LPDWORD lpcbClass, 
					  LPDWORD lpReserved, 
					  LPDWORD lpcSubKeys, 
					  LPDWORD lpcbMaxSubKeyLen, 
					  LPDWORD lpcbMaxClassLen, 
					  LPDWORD lpcValues, 
					  LPDWORD lpcbMaxValueNameLen, 
					  LPDWORD lpcbMaxValueLen, 
					  LPDWORD lpcbSecurityDescriptor, 
					  PFILETIME lpftLastWriteTime 
					  )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_QUERYINFOKEY, 12 );
    cs.arg0 = (DWORD)					  
		              hKey;
    return (DWORD)CALL_SERVER( &cs, 
					  lpClass, 
					  lpcbClass, 
					  lpReserved, 
					  lpcSubKeys, 
					  lpcbMaxSubKeyLen, 
					  lpcbMaxClassLen, 
					  lpcValues, 
					  lpcbMaxValueNameLen, 
					  lpcbMaxValueLen, 
					  lpcbSecurityDescriptor, 
					  lpftLastWriteTime 
					  );
#else

	PREG_QUERYINFOKEY pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_QUERYINFOKEY, &pfn, &cs ) )
	{
		retv = pfn( 
		              hKey, 
					  lpClass, 
					  lpcbClass, 
					  lpReserved, 
					  lpcSubKeys, 
					  lpcbMaxSubKeyLen, 
					  lpcbMaxClassLen, 
					  lpcValues, 
					  lpcbMaxValueNameLen, 
					  lpcbMaxValueLen, 
					  lpcbSecurityDescriptor, 
					  lpftLastWriteTime 
			      );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_QUERYVALUEEX )( 
					  HKEY hKey, 
					  LPCTSTR lpValueName, 
					  LPDWORD lpReserved, 
					  LPDWORD lpType, 
					  LPBYTE lpData, 
					  LPDWORD lpcbData 
					  );

LONG WINAPI Reg_QueryValueEx( 
					  HKEY hKey, 
					  LPCTSTR lpValueName, 
					  LPDWORD lpReserved, 
					  LPDWORD lpType, 
					  LPBYTE lpData, 
					  LPDWORD lpcbData 
					  )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_QUERYVALUEEX, 6 );
    cs.arg0 = (DWORD)					  
		              hKey;
    return (DWORD)CALL_SERVER( &cs, 
					  lpValueName, 
					  lpReserved, 
					  lpType, 
					  lpData, 
					  lpcbData );
#else

	PREG_QUERYVALUEEX pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_QUERYVALUEEX, &pfn, &cs ) )
	{
		retv = pfn(   hKey, 
					  lpValueName, 
					  lpReserved, 
					  lpType, 
					  lpData, 
					  lpcbData );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef LONG ( WINAPI * PREG_SETVALUEEX )(
					HKEY hKey, 
					LPCTSTR lpValueName, 
					DWORD Reserved, 
					DWORD dwType, 
					const BYTE *lpData, 
					DWORD cbData 
					);
LONG WINAPI Reg_SetValueEx( 
					HKEY hKey, 
					LPCTSTR lpValueName, 
					DWORD Reserved, 
					DWORD dwType, 
					const BYTE *lpData, 
					DWORD cbData 
					)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_REGISTRY, REG_SETVALUEEX, 6 );
    cs.arg0 = (DWORD)
					hKey;
    return (DWORD)CALL_SERVER( &cs, 
					lpValueName, 
					Reserved, 
					dwType, 
					lpData, 
					cbData 
					);
#else

	PREG_SETVALUEEX pfn;

	CALLSTACK cs;
	LONG   retv = ERROR_INVALID_FUNCTION;

	if( API_Enter( API_REGISTRY, REG_SETVALUEEX, &pfn, &cs ) )
	{
		retv = pfn( 
					hKey, 
					lpValueName, 
					Reserved, 
					dwType, 
					lpData, 
					cbData 
			      );
		API_Leave(  );
	}
	return retv;
#endif
}
