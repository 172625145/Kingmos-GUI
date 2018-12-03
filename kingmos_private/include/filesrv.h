/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __FILESRV_H
#define __FILESRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

//#undef CreateDirectory
//#define CreateDirectory FileSys_CreateDirectory
API_TYPE BOOL WINAPI FileSys_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes);

//#undef CreateFile
//#define CreateFile FileSys_CreateFile
API_TYPE HANDLE WINAPI FileSys_CreateFile( 
										  LPCTSTR lpcszFileName, 
										  DWORD dwAccess, 
										  DWORD dwShareMode, 
										  PSECURITY_ATTRIBUTES pSecurityAttributes, 
										  DWORD dwCreate, 
										  DWORD dwFlagsAndAttributes, 
										  HANDLE hTemplateFile );
//#undef CloseFile
//#define CloseFile FileSys_CloseFile
API_TYPE BOOL WINAPI FileSys_CloseFile( HANDLE );

//#undef DeleteAndRename
//#define DeleteAndRename FileSys_DeleteAndRename
API_TYPE BOOL WINAPI FileSys_DeleteAndRename( LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName );

//#undef DeleteFile
//#define DeleteFile FileSys_Delete
API_TYPE BOOL WINAPI FileSys_Delete( LPCTSTR lpcszFileName );

//#undef DeviceIoControl
//#define DeviceIoControl FileSys_DeviceIoControl
API_TYPE BOOL WINAPI FileSys_DeviceIoControl( 
                          HANDLE hFile,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );

//#undef FindClose
//#define FindClose FileSys_FindClose
API_TYPE BOOL WINAPI FileSys_FindClose( HANDLE hFind );

//#undef FindFirst
//#define FindFirst FileSys_FindFirst
API_TYPE HANDLE WINAPI FileSys_FindFirst( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd );

//#undef FindNext
//#define FindNext FileSys_FindNext
API_TYPE BOOL WINAPI FileSys_FindNext( HANDLE hFind, FILE_FIND_DATA * pfd );

//#undef FlushBuffers
//#define FlushBuffers FileSys_FlushBuffers
API_TYPE BOOL WINAPI FileSys_FlushBuffers( HANDLE hFile );

API_TYPE BOOL WINAPI FileSys_GetDiskFreeSpaceEx( 
						    LPCTSTR lpcszPathName, 
                            LPDWORD lpdwFreeBytesAvailableToCaller, 
                            LPDWORD lpdwTotalNumberOfBytes, 
                            LPDWORD lpdwTotalNumberOfFreeBytes );
API_TYPE DWORD WINAPI FileSys_GetAttributes( LPCTSTR lpcszFileName );
API_TYPE BOOL WINAPI FileSys_GetInformation( HANDLE hFile, FILE_INFORMATION * pfi );
API_TYPE DWORD WINAPI FileSys_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh );
API_TYPE BOOL WINAPI FileSys_GetTime( HANDLE hFile, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite );
API_TYPE BOOL WINAPI FileSys_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName );
API_TYPE BOOL WINAPI FileSys_Read( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );
API_TYPE BOOL WINAPI FileSys_ReadWithSeek( HANDLE hFile, LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,DWORD dwLowOffset,DWORD dwHighOffset );
API_TYPE BOOL WINAPI FileSys_RemoveDirectory( LPCTSTR lpcszPathName );
API_TYPE BOOL WINAPI FileSys_SetEnd( HANDLE hFile );
API_TYPE BOOL WINAPI FileSys_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes );
API_TYPE DWORD WINAPI FileSys_SetPointer( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
API_TYPE BOOL WINAPI FileSys_SetTime( HANDLE hFile, const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite );
API_TYPE BOOL WINAPI FileSys_Write( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );
API_TYPE BOOL WINAPI FileSys_WriteWithSeek( HANDLE hFile, LPCVOID lpcvBuffer, DWORD dwWrite, LPDWORD lpdwWritten, DWORD dwLowOffset, DWORD dwHighOffset );
API_TYPE BOOL WINAPI FileSys_Copy( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists );


//API_TYPE BOOL WINAPI FileMgr_LoadFSD ( HANDLE hDevice, LPCTSTR lpFSDName );
API_TYPE DWORD WINAPI FileMgr_GetDiskInfo( HDSK hDsk, PFDD pfdd );
API_TYPE DWORD WINAPI FileMgr_ReadDisk(HDSK hDsk, DWORD dwSector, DWORD cSectors, LPVOID pBuffer, DWORD cbBuffer);
API_TYPE DWORD WINAPI FileMgr_WriteDisk(HDSK hDsk, DWORD dwSector, DWORD cSectors, LPCVOID pBuffer, DWORD cbBuffer);
API_TYPE HVOL WINAPI FileMgr_RegisterVolume(HDSK hDsk, LPCTSTR lpszName, PVOL pVol );


API_TYPE BOOL WINAPI FileMgr_RegisterFSD ( LPCTSTR lpcszFSDName, const FSDDRV * lpfsd );

API_TYPE BOOL WINAPI FileMgr_UnregisterFSD ( LPCTSTR lpcszFSDName );
API_TYPE int WINAPI FileMgr_GetVolumeName(HVOL hVol, LPTSTR lpszName, int cchMax);
API_TYPE void WINAPI FileMgr_DeregisterVolume(HVOL hVol);
API_TYPE HANDLE WINAPI FileMgr_CreateFileHandle(HVOL hVol, HANDLE hProc, PFILE pFile);
API_TYPE BOOL WINAPI FileMgr_CloseFileHandle(HANDLE hFile);
API_TYPE HANDLE WINAPI FileMgr_CreateSearchHandle(HVOL hVol, HANDLE hProc, PFIND pfd );
API_TYPE BOOL WINAPI FileMgr_CloseSearchHandle(HANDLE hFile);

API_TYPE BOOL WINAPI FileMgr_DeviceIoControl ( 
                          HDSK hDsk,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );

// registry
#undef RegCloseKey
#define RegCloseKey FileReg_RegCloseKey
LONG WINAPI FileReg_CloseKey( HKEY hKey );

#undef RegCreateKeyEx
#define RegCreateKeyEx FileReg_CreateKeyEx
LONG WINAPI FileReg_CreateKeyEx( 
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
#undef RegDeleteKey
#define RegDeleteKey FileReg_DeleteKey
LONG WINAPI FileReg_DeleteKey( HKEY hKey, LPCTSTR lpSubKey );

#undef RegDeleteValue
#define RegDeleteValue FileReg_DeleteValue
LONG WINAPI FileReg_DeleteValue( HKEY hKey, LPCTSTR lpValueName );

#undef RegEnumKeyEx
#define RegEnumKeyEx FileReg_EnumKeyEx
LONG WINAPI FileReg_EnumKeyEx(
						  HKEY hKey, 
						  DWORD dwIndex, 
						  LPTSTR lpName, 
						  LPDWORD lpcbName, 
						  LPDWORD lpReserved, 
						  LPTSTR lpClass, 
						  LPDWORD lpcbClass, 
						  PFILETIME lpftLastWriteTime 
						  );
#undef RegFlushKey
#define RegFlushKey FileReg_FlushKey
LONG WINAPI FileReg_FlushKey( HKEY hKey );

#undef RegEnumValue
#define RegEnumValue FileReg_EnumValue
LONG WINAPI FileReg_EnumValue( 
				   HKEY hKey, 
				   DWORD dwIndex, 
				   LPTSTR lpValueName, 
				   LPDWORD lpcbValueName, 
				   LPDWORD lpReserved, 
				   LPDWORD lpType, 
				   LPBYTE lpData, 
				   LPDWORD lpcbData 
				   );

#undef RegOpenKeyEx
#define RegOpenKeyEx FileReg_OpenKeyEx
LONG WINAPI FileReg_OpenKeyEx( 
				   HKEY hKey, 
				   LPCTSTR lpSubKey, 
				   DWORD ulOptions, 
				   REGSAM samDesired, 
				   PHKEY phkResult 
				   );

#undef RegQueryInfoKey
#define RegQueryInfoKey FileReg_QueryInfoKey
LONG WINAPI FileReg_QueryInfoKey( 
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

#undef RegQueryValueEx
#define RegQueryValueEx FileReg_QueryValueEx
LONG WINAPI FileReg_QueryValueEx( 
					  HKEY hKey, 
					  LPCTSTR lpValueName, 
					  LPDWORD lpReserved, 
					  LPDWORD lpType, 
					  LPBYTE lpData, 
					  LPDWORD lpcbData 
					  );

#undef RegSetValueEx
#define RegSetValueEx FileReg_SetValueEx
LONG WINAPI FileReg_SetValueEx( 
					HKEY hKey, 
					LPCTSTR lpValueName, 
					DWORD Reserved, 
					DWORD dwType, 
					const BYTE *lpData, 
					DWORD cbData 
					);
// 


#ifdef __cplusplus
}
#endif  /* __cplusplus */

///////////////////////////////////////////////////

#endif   //__FILESRV_H

