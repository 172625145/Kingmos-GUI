#ifndef __FSDMGR_H
#define __FSDMGR_H


typedef struct _FSD_DISK_DATA
{ 
    DWORD nSectors;
    DWORD nBytesPerSector;
    DWORD nCylinders;
    DWORD nHeadsPerCylinder;
    DWORD nSectorsPerTrack;
    DWORD dwFlags;
} FSD_DISK_DATA, * PFDD;

#define HDSK DWORD
#define PVOL DWORD
#define HVOL DWORD
#define PFIND DWORD
#define PFILE DWORD

typedef BOOL (*SHELLFILECHANGEFUNC)( LPFILECHANGEINFO );

typedef struct _FSDDRV
{
    BOOL (*lpCloseFile)( PFILE pf );
    BOOL (*lpCloseVolume)( PVOL pVol );
    BOOL (*lpCreateDirectory)( PVOL pVol, LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes);
    HANDLE (*lpCreateFile)( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
    BOOL (*lpDeleteAndRenameFile)( PVOL pVol, LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName );
    BOOL (*lpDeleteFile)( PVOL pVol, LPCTSTR lpcszFileName );
    BOOL (*lpDeviceIoControl)( PFILE pf, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpdwBytesReturned );
    BOOL (*lpFindClose)( PFIND pSearch );
    HANDLE (*lpFindFirstFile)( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd );
    BOOL (*lpFindNextFile)( PFIND pFind, FILE_FIND_DATA * pfd );
    BOOL (*lpFlushFileBuffers)( PFILE pf );
    BOOL (*lpGetDiskFreeSpace)( PVOL pVol, LPCTSTR lpcszPathName, LPDWORD pSectorsPerCluster, LPDWORD pBytesPerSector, LPDWORD pFreeClusters, LPDWORD pClusters );
    DWORD (*lpGetFileAttributes)( PVOL pVol, LPCTSTR lpcszFileName );
    BOOL (*lpGetFileInformationByHandle)( PFILE pf, FILE_INFORMATION * pfi );
    DWORD (*lpGetFileSize)( PFILE pf, LPDWORD pFileSizeHigh );
    BOOL (*lpGetFileTime)( PFILE pf, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite );
    BOOL (*lpMountDisk)( HDSK hdsk );
    BOOL (*lpMoveFile)( PVOL pVol, LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName );
    void (*lpNotify)( PVOL pVol, DWORD dwFlags );     
    BOOL (*lpReadFile)( PFILE pf, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );
    BOOL (*lpReadFileWithSeek)(PFILE pf,LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,DWORD dwLowOffset,DWORD dwHighOffset);
    BOOL (*lpRegisterFileSystemFunction)( PVOL pVol, SHELLFILECHANGEFUNC pft );
    BOOL (*lpRemoveDirectory)( PVOL pVol, LPCTSTR lpcszPathName ); 
    BOOL (*lpSetEndOfFile)( PFILE pf );
    BOOL (*lpSetFileAttributes)( PVOL pVol, LPCTSTR lpcszFileName, DWORD dwFileAttributes );
    DWORD (*lpSetFilePointer)( PFILE pf, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);
    BOOL (*lpSetFileTime)( PFILE pf, const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite );
    BOOL (*lpUnmountDisk)( HDSK hdsk );
    BOOL (*lpWriteFile)( PFILE pf, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );
    BOOL (*lpWriteFileWithSeek)( PFILE pf, LPCVOID lpcvBuffer, DWORD cbWrite, LPDWORD pcbWritten, DWORD dwLowOffset, DWORD dwHighOffset );

    BOOL (*lpCopyFile)( PVOL pVol, LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists );

}FSDDRV, * PFSDDRV, FAR * LPFSDDRV;

DWORD WINAPI FSMGR_GetDiskInfo( HDSK hDsk, PFDD pfdd );
#define  FSDMGR_GetDiskInfo FSMGR_GetDiskInfo

DWORD WINAPI FSMGR_ReadDisk(HDSK hDsk, DWORD dwSector, DWORD cSectors, void * pBuffer, DWORD cbBuffer);
#define FSDMGR_ReadDisk FSMGR_ReadDisk

DWORD WINAPI FSMGR_WriteDisk(HDSK hDsk, DWORD dwSector, DWORD cSectors, const void * pBuffer, DWORD cbBuffer);
#define FSDMGR_WriteDisk FSMGR_WriteDisk

//DWORD   FSDMGR_ReadDiskEx(PFSGI pfsgi, PFSGR pfsgr);
//DWORD   FSDMGR_WriteDiskEx(PFSGI pfsgi, PFSGR pfsgr);
HVOL WINAPI FSMGR_RegisterVolume(HDSK hDsk, LPCTSTR lpszName, PVOL pVol );
#define FSDMGR_RegisterVolume FSMGR_RegisterVolume

BOOL WINAPI FSMGR_RegisterFSD ( LPCTSTR lpcszFSDName, const FSDDRV * lpfsd );
#define FSDMGR_RegisterFSD FSMGR_RegisterFSD

BOOL WINAPI FSMGR_UnregisterFSD ( LPCTSTR lpcszFSDName );
#define FSDMGR_UnregisterFSD FSMGR_UnregisterFSD

int WINAPI FSMGR_GetVolumeName(HVOL hVol, LPTSTR lpszName, int cchMax);
#define FSDMGR_GetVolumeName FSMGR_GetVolumeName

void  WINAPI FSMGR_DeregisterVolume(HVOL hVol);
#define FSDMGR_DeregisterVolume FSMGR_DeregisterVolume

HANDLE  WINAPI FSMGR_CreateFileHandle(HVOL hVol, HANDLE hProc, PFILE pFile);
#define FSDMGR_CreateFileHandle FSMGR_CreateFileHandle

BOOL WINAPI FSMGR_CloseFileHandle(HANDLE hFile);
//#define FSDMGR_CreateFileHandle FSMGR_CreateFileHandle

HANDLE WINAPI FSMGR_CreateSearchHandle(HVOL hVol, HANDLE hProc, PFIND pfd );
#define FSDMGR_CreateSearchHandle FSMGR_CreateSearchHandle

BOOL WINAPI FSMGR_CloseSearchHandle(HANDLE hFile);

BOOL WINAPI FSMGR_DeviceIoControl ( 
                          HDSK hDsk,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );
#define FSDMGR_DeviceIoControl FSMGR_DeviceIoControl


#endif  //__FSDMGR_H



