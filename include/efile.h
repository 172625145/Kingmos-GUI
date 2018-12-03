/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EFILE_H
#define __EFILE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define CREATE_NEW    1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS   4
#define TRUNCATE_EXISTING   5

#define  FILE_SHARE_READ 0x0001
#define  FILE_SHARE_WRITE 0x0002

#define GENERIC_READ  0x0001
#define GENERIC_WRITE 0x0002
#define GENERIC_DATA  0x0004
//#define GENERIC_SYNC  0x8000

#define FILE_BEGIN   0
#define FILE_CURRENT 1
#define FILE_END     2

#define MAX_PATH         260 

//#define FILE_ATTRIBUTE_NORMAL 0
//#define FILE_ATTRIBUTE_ARCHIVE   0x0001
//#define FILE_ATTRIBUTE_DIRECTORY 0x0002
//#define FILE_ATTRIBUTE_HIDDEN    0x0004
//#define FILE_ATTRIBUTE_READONLY  0x0008
//#define FILE_ATTRIBUTE_SYSTEM    0x0010

#define FILE_ATTRIBUTE_READONLY         0x00000001
#define FILE_ATTRIBUTE_HIDDEN           0x00000002
#define FILE_ATTRIBUTE_SYSTEM           0x00000004
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020
#define FILE_ATTRIBUTE_NORMAL           0x00000080


#define FILE_ATTRIBUTE_DEVICE           0x00008000

#define	FILE_ATTRIBUTE_ROMMODULE        0x00004000
#define	FILE_ATTRIBUTE_ROMFILE		    0x00002000

#define FILE_ATTRIBUTE_ROM              (FILE_ATTRIBUTE_ROMMODULE|FILE_ATTRIBUTE_ROMFILE)

#define FILE_ATTRIBUTE_SPARSE_FILE      0x00000200  
#define FILE_ATTRIBUTE_REPARSE_POINT    0x00000400  
#define FILE_ATTRIBUTE_COMPRESSED       0x00000800  
#define FILE_ATTRIBUTE_ENCRYPTED        0x00000040  
//#define FILE_ATTRIBUTE_SYNC             0x80000000  


#define FSNOTIFY_POWER_ON       0x00000001l
#define FSNOTIFY_POWER_OFF      0x00000002l
#define FSNOTIFY_DEVICES_ON     0x00000004l

#define INVALID_FILE_SIZE       (DWORD)0xFFFFFFFF

// for DeviceIoControl
//#define IOCTL_DISK_FORMAT_VOLUME    1
//#define IOCTL_DISK_READONLY         2
/*
typedef struct _FILE_FIND_DATA 
{ 
	DWORD dwFileAttributes; 
	FILETIME ftCreationTime; 
	FILETIME ftLastAccessTime; 
	FILETIME ftLastWriteTime; 
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow; 
	DWORD dwOID; 
	TCHAR cFileName[MAX_PATH]; 
}FILE_FIND_DATA, * PFILE_FIND_DATA, FAR * LPFILE_FIND_DATA; 
*/
 
typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR   cFileName[ MAX_PATH ];
    CHAR   cAlternateFileName[ 14 ];
} WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;

typedef WIN32_FIND_DATA FILE_FIND_DATA;
typedef LPWIN32_FIND_DATA LPFILE_FIND_DATA;

typedef struct _FILE_INFORMATION 
{ 
	DWORD dwFileAttributes; 
	FILETIME ftCreationTime; 
	FILETIME ftLastAccessTime; 
	FILETIME ftLastWriteTime; 
	DWORD dwVolumeSerialNumber; 
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow; 
	DWORD nNumberOfLinks; 
	DWORD nFileIndexHigh; 
	DWORD nFileIndexLow; 
} FILE_INFORMATION, * PFILE_INFORMATION, FAR * LPFILE_INFORMATION;
#define BY_HANDLE_FILE_INFORMATION BY_HANDLE_FILE_INFORMATION
#define PBY_HANDLE_FILE_INFORMATION PFILE_INFORMATION
#define LPBY_HANDLE_FILE_INFORMATION LPFILE_INFORMATION

typedef struct _FILECHANGEINFO 
{
    DWORD cbSize;
    LONG  wEventId;
    ULONG uFlags;
    DWORD dwItem1;            
    DWORD dwItem2;
    DWORD dwAttributes;  
    FILETIME ftModified;   
    ULONG nFileSize;
} FILECHANGEINFO, * PFILECHANGEINFO, FAR * LPFILECHANGEINFO;

#define CreateDirectory File_CreateDirectory
BOOL WINAPI File_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes);

#define CreateFile File_CreateFile
HANDLE WINAPI File_CreateFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );

#define DeleteAndRenameFile File_DeleteAndRename
BOOL WINAPI File_DeleteAndRename( LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName );

#define DeleteFile File_Delete
BOOL WINAPI File_Delete( LPCTSTR lpcszFileName );

#define DeviceIoControl  File_DeviceIoControl
BOOL WINAPI File_DeviceIoControl( 
                          HANDLE hFile,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );

#define FindClose File_FindClose
BOOL WINAPI File_FindClose( HANDLE hFind );

#define FindFirstFile File_FindFirst
HANDLE WINAPI File_FindFirst( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd );

#define FindNextFile File_FindNext
BOOL WINAPI File_FindNext( HANDLE hFind, FILE_FIND_DATA * pfd );

#define FlushFileBuffers File_FlushBuffers
BOOL WINAPI File_FlushBuffers( HANDLE hFile );

#define GetDiskFreeSpaceEx File_GetDiskFreeSpaceEx
BOOL WINAPI File_GetDiskFreeSpaceEx( 
						    LPCTSTR lpcszPathName, 
                            LPDWORD lpdwFreeBytesAvailableToCaller, 
                            LPDWORD lpdwTotalNumberOfBytes, 
                            LPDWORD lpdwTotalNumberOfFreeBytes );

#define GetFileAttributes File_GetAttributes
DWORD WINAPI File_GetAttributes( LPCTSTR lpcszFileName );

#define GetFileInformationByHandle File_GetInformation
BOOL WINAPI File_GetInformation( HANDLE hFile, FILE_INFORMATION * pfi );

#define GetFileSize File_GetSize
DWORD WINAPI File_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh );

#define GetFileTime File_GetTime
BOOL WINAPI File_GetTime( HANDLE hFile, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite );

#define MoveFile File_Move
BOOL WINAPI File_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName );

#define ReadFile File_Read
BOOL WINAPI File_Read( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );

#define ReadFileWithSeek File_ReadWithSeek
BOOL WINAPI File_ReadWithSeek( HANDLE hFile, LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,DWORD dwLowOffset,DWORD dwHighOffset );

#define RemoveDirectory File_RemoveDirectory
BOOL WINAPI File_RemoveDirectory( LPCTSTR lpcszPathName );

#define SetEndOfFile File_SetEnd
BOOL WINAPI File_SetEnd( HANDLE hFile );

#define INVALID_SET_FILE_POINTER		((unsigned int)(-1))

#define SetFileAttributes File_SetAttributes
BOOL WINAPI File_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes );

#define SetFilePointer File_SetPointer
DWORD WINAPI File_SetPointer( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );

#define SetFileTime File_SetTime
BOOL WINAPI File_SetTime( HANDLE hFile, const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite );

#define WriteFile File_Write
BOOL WINAPI File_Write( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );

#define WriteFileWithSeek File_WriteWithSeek
BOOL WINAPI File_WriteWithSeek( HANDLE hFile, LPCVOID lpcvBuffer, DWORD dwWrite, LPDWORD lpdwWritten, DWORD dwLowOffset, DWORD dwHighOffset );

#define CopyFile File_Copy
BOOL WINAPI File_Copy( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists );
 
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //__EFILE_H
