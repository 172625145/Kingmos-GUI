/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __DEVSRV_H
#define __DEVSRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#ifndef __MISCELLANY_H
#include <miscellany.h>
#endif

API_TYPE HANDLE WINAPI Device_RegisterDevice( 
									LPCTSTR lpDevName, 
									UINT index, 
									LPCTSTR lpszLib, 
									LPVOID lpParam );

API_TYPE BOOL WINAPI Device_Deregister( HANDLE handle );
//API_TYPE BOOL WINAPI Device_GetName( HANDLE hDevInit, LPTSTR lpszBuf, UINT uSizeLimit );

//#undef Device_RegisterFSD
//#define Dev_RegisterFSD  Device_RegisterFSD
API_TYPE BOOL WINAPI Device_RegisterFSD( LPCTSTR lpcszFSDName, HANDLE hFSD, const FSDINITDRV * lpfsd );

//#undef Dev_UnregisterFSD
//#define Dev_UnregisterFSD Device_UnregisterFSD
API_TYPE BOOL WINAPI Device_UnregisterFSD( LPCTSTR lpcszFSDName );

//#undef Dev_LoadFSD
//#define Dev_LoadFSD Device_LoadFSD
API_TYPE BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName );

API_TYPE DWORD WINAPI Device_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen );
API_TYPE BOOL WINAPI Device_UnloadFSD ( HANDLE hDevice );

////////////////////////////////////////////////////////////////////////////////
// device file server

//#undef Dev_CreateFile
//#define Dev_CreateFile Device_CreateFile
API_TYPE HANDLE WINAPI Device_CreateFile( LPCTSTR lpszName, DWORD dwAccess, DWORD dwShareMode, HANDLE hProc );

//#undef Dev_CloseFile
//#define Dev_CloseFile Device_CloseFile
API_TYPE BOOL WINAPI Device_CloseFile( HANDLE hOpenFile );

//#undef Dev_ReadFile
//#define Dev_ReadFile Device_ReadFile
API_TYPE BOOL WINAPI Device_ReadFile( HANDLE hOpenFile, LPVOID lpvBuffer, DWORD dwBytesToRead, LPDWORD lpdwNumBytesRead, LPOVERLAPPED lpOverlapped );

//#undef Dev_WriteFile
//#define Dev_WriteFile Device_WriteFile
API_TYPE BOOL WINAPI Device_WriteFile( HANDLE hOpenFile, LPCVOID lpvBuffer, DWORD dwBytesToWrite, LPDWORD lpdwNumBytesWritten, LPOVERLAPPED lpOverlapped );

API_TYPE BOOL WINAPI Device_FlushFileBuffers( HANDLE hOpenFile );
API_TYPE BOOL WINAPI Device_SetFileTime( HANDLE hOpenFile, CONST FILETIME *lpCreation, CONST FILETIME *lpLastAccess, CONST FILETIME *lpLastWrite );
API_TYPE BOOL WINAPI Device_GetFileTime( HANDLE hOpenFile, LPFILETIME lpftCreation, LPFILETIME lpftLastAccess, LPFILETIME lpftLastWrite );
API_TYPE DWORD WINAPI Device_GetFileSize( HANDLE hOpenFile, LPDWORD lpdwFileSizeHigh );
API_TYPE BOOL WINAPI Device_SetEndOfFile( HANDLE hOpenFile );
API_TYPE BOOL WINAPI Device_GetFileInformationByHandle( HANDLE hOpenFile, LPBY_HANDLE_FILE_INFORMATION lpFileInfo );

//#undef Dev_IoControl
//#define Dev_IoControl Device_IoControl
API_TYPE BOOL WINAPI Device_IoControl( HANDLE hOpenFile, DWORD  dwIoControlCode, LPVOID lpvInBuf, DWORD dwInBufSize, LPVOID lpvOutBuf, DWORD dwOutBufSize, LPDWORD lpdwBytesReturned, LPOVERLAPPED lpOverlapped );

//#undef Dev_SetFilePointer
//#define Dev_SetFilePointer Device_SetFilePointer
API_TYPE DWORD WINAPI Device_SetFilePointer( HANDLE hOpenFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );

DWORD WINAPI Device_ServerHandler( HANDLE hServer, DWORD dwServerCode, DWORD dwParam, LPVOID lpParam );

BOOL WINAPI Device_GetSystemPowerStatusEx(
				PSYSTEM_POWER_STATUS_EX pstatus, 
				BOOL fUpdate );


#ifdef __cplusplus
}
#endif  /* __cplusplus */

///////////////////////////////////////////////////

#endif   //__DEVSRV_H

