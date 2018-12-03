#ifndef __EPDEV_H
#define __EPDEV_H

#ifndef __DEVSRV_H
#include <devdrv.h>
#endif
#ifndef __DISKIO_H
#include <diskio.h>
#endif


typedef struct _DEVICE_INFO
{
    //LPCSTR lpcsName;
    const DEVICE_DRIVER * lpDriver;
    DWORD handle;
}DEVICE_INFO, FAR * LPDEVICE_INFO;

//BOOL _FindDevice( LPCTSTR lpszName, LPDEVICE_INFO lpdi );
//BOOL _GetDeviceName( HANDLE handle, LPTSTR lpszBuf, UINT uSizeLimit );

BOOL WINAPI Dev_GetName( HANDLE hDevInit, LPTSTR lpszBuf, UINT uSizeLimit );

HANDLE WINAPI Dev_CreateFile( LPCTSTR lpszName, DWORD dwAccess, DWORD dwShareMode, HANDLE hProc );
BOOL WINAPI Dev_CloseFile( HANDLE hOpenFile );
BOOL WINAPI Dev_ReadFile( HANDLE hOpenFile, LPVOID lpvBuffer, DWORD dwBytesToRead, LPDWORD lpdwNumBytesRead, LPOVERLAPPED lpOverlapped );
BOOL WINAPI Dev_WriteFile( HANDLE hOpenFile, LPCVOID lpvBuffer, DWORD dwBytesToWrite, LPDWORD lpdwNumBytesWritten, LPOVERLAPPED lpOverlapped );
BOOL WINAPI Dev_FlushFileBuffers( HANDLE hOpenFile );
BOOL WINAPI Dev_SetFileTime( HANDLE hOpenFile, CONST FILETIME *lpCreation, CONST FILETIME *lpLastAccess, CONST FILETIME *lpLastWrite );
BOOL WINAPI Dev_GetFileTime( HANDLE hOpenFile, LPFILETIME lpftCreation, LPFILETIME lpftLastAccess, LPFILETIME lpftLastWrite );
DWORD WINAPI Dev_GetFileSize( HANDLE hOpenFile, LPDWORD lpdwFileSizeHigh );
BOOL WINAPI Dev_SetEndOfFile( HANDLE hOpenFile );
DWORD WINAPI Dev_SetFilePointer( HANDLE hOpenFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
BOOL WINAPI Dev_GetFileInformationByHandle( HANDLE hOpenFile, LPBY_HANDLE_FILE_INFORMATION lpFileInfo );
BOOL WINAPI Dev_IoControl( HANDLE hOpenFile, DWORD  dwIoControlCode, LPVOID lpvInBuf, DWORD dwInBufSize, LPVOID lpvOutBuf, DWORD dwOutBufSize, LPDWORD lpdwBytesReturned, LPOVERLAPPED lpOverlapped );


#endif  // __EPDEV_H


