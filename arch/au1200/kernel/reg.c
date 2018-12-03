#include <ewindows.h>

LONG WINAPI Reg_CloseKey( HKEY hKey )
{
	return !ERROR_SUCCESS;
}

LONG WINAPI Reg_QueryValueEx( 
							 HKEY hKey, 
							 LPCTSTR lpValueName, 
							 LPDWORD lpReserved, 
							 LPDWORD lpType, 
							 LPBYTE lpData, 
							 LPDWORD lpcbData 
							 )
{
	return !ERROR_SUCCESS;
}


LONG WINAPI Reg_SetValueEx( 
						   HKEY hKey, 
						   LPCTSTR lpValueName, 
						   DWORD Reserved, 
						   DWORD dwType, 
						   const BYTE *lpData, 
						   DWORD cbData 
						   )
{
	return !ERROR_SUCCESS;
}

LONG WINAPI Reg_OpenKeyEx( 
						  HKEY hKey, 
						  LPCTSTR lpSubKey, 
						  DWORD ulOptions, 
						  REGSAM samDesired, 
						  PHKEY phkResult 
						  )
{
	return !ERROR_SUCCESS;
}
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
	return !ERROR_SUCCESS;
}