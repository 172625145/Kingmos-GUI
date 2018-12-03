/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __DEVDRV_H
#define __DEVDRV_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#ifndef __EDEVICE_H
#include <edevice.h>
#endif

#ifndef __DISKIO_H
#include <diskio.h>
#endif

typedef struct _DEVICE_DRIVER {
	// DLL name: XXX_Init
    DWORD (*lpInit)( DWORD dwContext );  
	// DLL name: XXX_Deinit
    BOOL  (*lpDeinit)( DWORD hDevHandle );  // hDevHandle = 调用lpInit的返回值
	// DLL name: XXX_IOControl
    BOOL  (*lpIOControl)( DWORD hOpenHandle, DWORD dwCode, LPVOID lpvInBuf, DWORD nInBufSize, LPVOID lpvOutBuf, DWORD nOutBufSize, LPDWORD lpdwReturned ); //hOpenHandle = 调用lpOpen的返回值
    // DLL name: XXX_Open
	DWORD (*lpOpen)( DWORD hDevHandle, DWORD dwAccess, DWORD dwShareMode );// hDevHandle = 调用lpInit的返回值
    // DLL name: XXX_Close
	BOOL  (*lpClose)( DWORD hOpenHandle );//hOpenHandle = 调用lpOpen的返回值
    // DLL name: XXX_Read
	DWORD (*lpRead)( DWORD hOpenHandle, LPVOID lpBuffer, DWORD dwNumBytes );//hOpenHandle = 调用lpOpen的返回值
    // DLL name: XXX_Write
	DWORD (*lpWrite)( DWORD hOpenHandle, LPCVOID lpBuffer, DWORD dwNumBytes );//hOpenHandle = 调用lpOpen的返回值
    // DLL name: XXX_Seek
	DWORD (*lpSeek)( DWORD hOpenHandle, long lDistance, DWORD dwMoveMethod );//hOpenHandle = 调用lpOpen的返回值
    // DLL name: XXX_Powerup
	VOID  (*lpPowerUp)( DWORD hDevHandle );// hDevHandle = 调用lpInit的返回值
    // DLL name: XXX_PowerDown
	VOID  (*lpPowerDown)( DWORD hDevHandle );// hDevHandle = 调用lpInit的返回值
}DEVICE_DRIVER, FAR * LPDEVICE_DRIVER;

// for IOCTL_DISK_INITIALIZED
typedef struct _DISK_INIT {
    HANDLE hDevice;          // device handle from RegisterDevice
} DISK_INIT, * PDISK_INIT, FAR * LPDISK_INIT;

// for FSD 
typedef struct _FSDINITDRV
{
	HANDLE ( WINAPI * lpAttachDevice )( HANDLE hFSD, LPCTSTR lpcszDiskName );
	BOOL ( WINAPI * lpDetachDevice )( HANDLE hFSDData );
}FSDINITDRV, FAR * LPFSDINITDRV;


typedef struct _RW_BUF {
	BYTE * lpBuf;        // buffer for read write
	DWORD  dwSize;        // length of buffer
} RW_BUF, *PRW_BUF;

// for IOCTL_DISK_READ and IOCTL_DISK_WRITE
typedef struct _DISK_RW {
    DWORD dwStartSector;    // start sector
	DWORD dwSectorNumber;   // number of sectors
	DWORD nrwNum;           // number of RW_BUF LIST
    DWORD dwStatus;         // read & write's status
	RW_BUF rwBufs[1];        // rwbuf array
} DISK_RW, * PDISK_RW;

// for IOCTL_DISK_GETINFO and IOCTL_DISK_SETINFO
typedef struct _DISK_INFO
{ 
    DWORD nTotalSectors;
    DWORD nBytesPerSector;
    DWORD nCylinders;
    DWORD nHeadsPerCylinder;
    DWORD nSectorsPerTrack;
    DWORD dwFlags;
} DISK_INFO, * PDISK_INFO, FAR * LPDISK_INFO;

#define DISK_INFO_FLAG_CHS_UNCERTAIN        0x0001
#define DISK_INFO_FLAG_UNFORMATTED          0x0002
#define DISK_INFO_FLAG_READONLY             0x0004
#define DISK_INFO_FLAG_MBR                  0x0008
#define DISK_INFO_FLAG_PAGEABLE             0x0010 // device can be used for demand paging
#define DISK_INFO_FLAG_HIDDEN               0x0100
#define DISK_INFO_FLAG_AUTOFORMAT           0x0200  //文件系统可以自动决定是否格式化该disk

#define RegisterDevice Dev_RegisterDevice
//
//  lpszLib = 动态连接库名
//
HANDLE WINAPI Dev_RegisterDevice( 
                    LPCTSTR lpDevName, 
                    UINT index, 
                    LPCTSTR lpszLib, 
                    DWORD dwParam );
//
//  静态连接界面
//  lpDriver = 驱动程序接口
//
#define RegisterDriver Dev_RegisterDriver
HANDLE WINAPI Dev_RegisterDriver( 
                    LPCTSTR lpDevName, 
                    UINT index, 
                    const DEVICE_DRIVER FAR * lpDriver, 
                    LPVOID lpParam );

#define DeregisterDevice Dev_Deregister
BOOL WINAPI Dev_Deregister( HANDLE );

#define EnumDevices Dev_Enum
DWORD WINAPI Dev_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen );

BOOL WINAPI Dev_RegisterFSD( LPCTSTR lpcszFSDName, HANDLE hFSD, const FSDINITDRV * lpfsd );
BOOL WINAPI Dev_UnregisterFSD( LPCTSTR lpcszFSDName );

#define FSDMGR_LoadFSD Dev_LoadFSD
BOOL WINAPI Dev_LoadFSD( HANDLE hDevice, LPCTSTR lpFSDName );

#define FSDMGR_UnloadFSD Dev_UnloadFSD
BOOL WINAPI Dev_UnloadFSD ( HANDLE hDevice );

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__EDEVICE_H
