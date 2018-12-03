/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EREG_H
#define __EREG_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

//#ifndef KINGMOS
//#define  ROOT_MICROSOFT
//#endif

//#ifndef ROOT_MICROSOFT
//#endif

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;

// 注册键存取权
//

#define KEY_QUERY_VALUE         0x0001
#define KEY_SET_VALUE           0x0002
#define KEY_CREATE_SUB_KEY      0x0004
#define KEY_ENUMERATE_SUB_KEYS  0x0008
#define KEY_NOTIFY              0x0010
#define KEY_CREATE_LINK         0x0020

#define KEY_READ                ( ( STANDARD_RIGHTS_READ       |\
                                    KEY_QUERY_VALUE            |\
                                    KEY_ENUMERATE_SUB_KEYS     |\
                                    KEY_NOTIFY )                 \
                                    &                           \
                                    (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))


#define REG_NONE                    ( 0 )   
#define REG_SZ                      ( 1 )   
#define REG_EXPAND_SZ               ( 2 )   
                                            
#define REG_BINARY                  ( 3 )   
#define REG_DWORD                   ( 4 )   
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   
#define REG_DWORD_BIG_ENDIAN        ( 5 )   
#define REG_LINK                    ( 6 )   
#define REG_MULTI_SZ                ( 7 )   
#define REG_RESOURCE_LIST           ( 8 )   
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )


#define REG_CREATED_NEW_KEY         (0x00000001L)   
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   


#ifdef ROOT_MICROSOFT

#define HKEY_CLASSES_ROOT           (( HKEY ) 0x80000000 )
#define HKEY_CURRENT_USER           (( HKEY ) 0x80000001 )
#define HKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define HKEY_USERS                  (( HKEY ) 0x80000003 )

#else

//#define HKEY_ROOT					(( HKEY ) 0x80000000 )
#define HKEY_HARDWARE_ROOT			(( HKEY ) 0x80000000 )
#define HKEY_SOFTWARE_ROOT			(( HKEY ) 0x80000001 )


#endif


LONG WINAPI Reg_CloseKey( HKEY hKey ); 

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
							); 

LONG WINAPI Reg_DeleteKey( 
						  HKEY hKey, 
						  LPCTSTR lpSubKey 
						  );

LONG WINAPI Reg_DeleteValue( 
							HKEY hKey, 
							LPCTSTR lpValueName 
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
						  ); 
 
LONG WINAPI Reg_FlushKey( 
				  HKEY hKey 
				  );

LONG WINAPI Reg_OpenKeyEx( 
						  HKEY hKey, 
						  LPCTSTR lpSubKey, 
						  DWORD ulOptions, 
						  REGSAM samDesired, 
						  PHKEY phkResult 
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
							 ); 

LONG WINAPI Reg_QueryValueEx( 
							 HKEY hKey, 
							 LPCTSTR lpValueName, 
							 LPDWORD lpReserved, 
							 LPDWORD lpType, 
							 LPBYTE lpData, 
							 LPDWORD lpcbData 
							 ); 

LONG WINAPI Reg_SetValueEx( 
						   HKEY hKey, 
						   LPCTSTR lpValueName, 
						   DWORD Reserved, 
						   DWORD dwType, 
						   const BYTE *lpData, 
						   DWORD cbData 
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
				   );

#define RegCloseKey Reg_CloseKey
#define RegCreateKeyEx Reg_CreateKeyEx
#define RegDeleteKey Reg_DeleteKey
#define RegDeleteValue Reg_DeleteValue
#define RegEnumKeyEx Reg_EnumKeyEx
#define RegFlushKey Reg_FlushKey
#define RegOpenKeyEx Reg_OpenKeyEx
#define RegQueryInfoKey Reg_QueryInfoKey
#define RegQueryValueEx Reg_QueryValueEx
#define RegSetValueEx Reg_SetValueEx
#define RegEnumValue Reg_EnumValue


#define ERROR_BADKEY                     1010L
#define ERROR_NO_LOG_SPACE               1019L
#define ERROR_CANTOPEN                   1011L
#define ERROR_KEY_DELETED                1018L

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif // __EREG_H
