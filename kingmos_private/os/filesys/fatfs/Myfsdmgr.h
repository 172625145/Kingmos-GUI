#ifndef __MYFSDMGR_H
#define __MYFSDMGR_H


typedef struct _FSD_DISK_DATA
{ 
    DWORD nSectors;
    DWORD nBytesPerSector;
    DWORD nCylinders;
    DWORD nHeadsPerCylinder;
    DWORD nSectorsPerTrack;
    DWORD dwFlags;
} FSD_DISK_DATA, * PFDD;



#define HDSK HANDLE
#define PVOL HANDLE
#define HVOL HANDLE
#define PFIND HANDLE
#define PFILE HANDLE

#ifndef UNDER_CE


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

DWORD WINAPI FSMGR_DeviceIoControl ( 
                          HDSK hDsk,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );
#define FSDMGR_DeviceIoControl FSMGR_DeviceIoControl

#endif

#endif  //__FSDMGR_H
