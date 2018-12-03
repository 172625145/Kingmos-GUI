/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <efile.h>
#include <eufile.h>
#include <eapisrv.h>

#include <efsdmgr.h>
#include <epcore.h>

typedef DWORD ( WINAPI * PFSMGR_GETDISKINFO )( HDSK hDsk, PFDD pfdd );
DWORD WINAPI FSMGR_GetDiskInfo( HDSK hDsk, PFDD pfdd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_GETDISKINFO, 2 );
    cs.arg0 = (DWORD)hDsk;
    return (DWORD)CALL_SERVER( &cs, pfdd );
#else

	PFSMGR_GETDISKINFO pGetDiskInfo;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	DWORD  retv = !ERROR_SUCCESS;

	if( API_Enter( API_FSDMGR, FSMGR_GETDISKINFO, &pGetDiskInfo, &cs ) )
	{
		//pfdd = MapCallStackPtr( pfdd, &cs );

		retv = pGetDiskInfo( hDsk, pfdd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PFSMGR_READDISK )(HDSK hDsk, DWORD dwSector, DWORD cSectors, void * pBuffer, DWORD cbBuffer);
DWORD WINAPI FSMGR_ReadDisk(HDSK hDsk, 
							DWORD dwSector, 
							DWORD cSectors, 
							void * pBuffer, 
							DWORD cbBuffer)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_READDISK, 5 );
    cs.arg0 = (DWORD)hDsk;
    return (DWORD)CALL_SERVER( &cs, dwSector, cSectors, pBuffer, cbBuffer );
#else

	PFSMGR_READDISK pReadDisk;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	DWORD  retv = !ERROR_SUCCESS;

	if( API_Enter( API_FSDMGR, FSMGR_READDISK, &pReadDisk, &cs ) )
	{
		retv = pReadDisk( hDsk, dwSector, cSectors, pBuffer, cbBuffer );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PFSMGR_WRITEDISK )(HDSK hDsk, DWORD dwSector, DWORD cSectors, const void * pBuffer, DWORD cbBuffer);
DWORD WINAPI FSMGR_WriteDisk(HDSK hDsk, 
							 DWORD dwSector, 
							 DWORD cSectors, 
							 const void * pBuffer, 
							 DWORD cbBuffer)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_WRITEDISK, 5 );
    cs.arg0 = (DWORD)hDsk;
    return (DWORD)CALL_SERVER( &cs,dwSector,cSectors,pBuffer,cbBuffer );
#else

	PFSMGR_WRITEDISK pWriteDisk;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	DWORD  retv = !ERROR_SUCCESS;

	if( API_Enter( API_FSDMGR, FSMGR_WRITEDISK, &pWriteDisk, &cs ) )
	{
		retv = pWriteDisk( hDsk,dwSector,cSectors,pBuffer,cbBuffer);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HVOL ( WINAPI * PFSMGR_REGISTERVOLUME )(HDSK hDsk, LPCTSTR lpszName, PVOL pVol );
HVOL WINAPI FSMGR_RegisterVolume(HDSK hDsk, LPCTSTR lpszName, PVOL pVol )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_REGISTERVOLUME, 3 );
    cs.arg0 = (DWORD)hDsk;
    return (DWORD)CALL_SERVER( &cs, lpszName, pVol );
#else

	PFSMGR_REGISTERVOLUME pRegisterVolume;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	HVOL  retv = NULL;

	if( API_Enter( API_FSDMGR, FSMGR_REGISTERVOLUME, &pRegisterVolume, &cs ) )
	{
		retv = pRegisterVolume( hDsk, lpszName, pVol );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFSMGR_REGISTERFSD )( LPCTSTR lpcszFSDName, const FSDDRV * lpfsd );
BOOL WINAPI FSMGR_RegisterFSD ( LPCTSTR lpcszFSDName, const FSDDRV * lpfsd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_REGISTERFSD, 2 );
    cs.arg0 = (DWORD)lpcszFSDName;
    return (DWORD)CALL_SERVER( &cs, lpfsd );
#else

	PFSMGR_REGISTERFSD pRegisterFSD ;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_FSDMGR, FSMGR_REGISTERFSD, &pRegisterFSD, &cs ) )
	{
		retv = pRegisterFSD( lpcszFSDName, lpfsd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFSMGR_UNREGISTERFSD )( LPCTSTR lpcszFSDName );
BOOL WINAPI FSMGR_UnregisterFSD ( LPCTSTR lpcszFSDName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_UNREGISTERFSD, 1 );
    cs.arg0 = (DWORD)lpcszFSDName ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFSMGR_UNREGISTERFSD pfn;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_FSDMGR, FSMGR_UNREGISTERFSD, &pfn, &cs ) )
	{
		retv = pfn( lpcszFSDName );
		API_Leave(  );
	}
	return retv;
#endif
}


typedef int ( WINAPI * PFSMGR_GETVOLUMENAME )(HVOL hVol, LPTSTR lpszName, int cchMax);
int WINAPI FSMGR_GetVolumeName(HVOL hVol, LPTSTR lpszName, int cchMax)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_GETVOLUMENAME, 3 );
    cs.arg0 = (DWORD)hVol;
    return (DWORD)CALL_SERVER( &cs, lpszName, cchMax );
#else

	PFSMGR_GETVOLUMENAME pGetVolumeName;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_FSDMGR, FSMGR_GETVOLUMENAME, &pGetVolumeName, &cs ) )
	{
		retv = pGetVolumeName( hVol, lpszName, cchMax );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef void ( WINAPI * PFSMGR_DEREGISTERVOLUME )(HVOL hVol);
void WINAPI FSMGR_DeregisterVolume(HVOL hVol)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_DEREGISTERVOLUME, 1 );
    cs.arg0 = (DWORD)hVol ;
    CALL_SERVER( &cs );
#else

	PFSMGR_DEREGISTERVOLUME pDeregisterVolume;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	int retv = 0;

	if( API_Enter( API_FSDMGR, FSMGR_DEREGISTERVOLUME, &pDeregisterVolume, &cs ) )
	{
		pDeregisterVolume( hVol );
		API_Leave(  );
	}
#endif
}

typedef HANDLE ( WINAPI * PFSMGR_CREATEFILEHANDLE )(HVOL hVol, HANDLE hProc, PFILE pFile);
HANDLE WINAPI FSMGR_CreateFileHandle(HVOL hVol, HANDLE hProc, PFILE pFile)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_CREATEFILEHANDLE, 3 );
    cs.arg0 = (DWORD)hVol;
    return (HANDLE)CALL_SERVER( &cs, hProc, pFile );
#else

	PFSMGR_CREATEFILEHANDLE pCreateFileHandle;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	HANDLE retv = NULL;

	if( API_Enter( API_FSDMGR, FSMGR_CREATEFILEHANDLE, &pCreateFileHandle, &cs ) )
	{
		retv = pCreateFileHandle( hVol, hProc, pFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFSMGR_CLOSEFILEHANDLE )(HANDLE hFile);
BOOL WINAPI FSMGR_CloseFileHandle(HANDLE hFile)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_CLOSEFILEHANDLE, 1 );
    cs.arg0 = (DWORD)hFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFSMGR_CLOSEFILEHANDLE pCloseFileHandle;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_FSDMGR, FSMGR_CLOSEFILEHANDLE, &pCloseFileHandle, &cs ) )
	{
		retv = pCloseFileHandle( hFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HANDLE ( WINAPI * PFSMGR_CREATESEARCHHANDLE )(HVOL hVol, HANDLE hProc, PFIND pfd );
HANDLE WINAPI FSMGR_CreateSearchHandle(HVOL hVol, HANDLE hProc, PFIND pfd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_CREATESEARCHHANDLE, 3 );
    cs.arg0 = (DWORD)hVol;
    return (HANDLE)CALL_SERVER( &cs, hProc, pfd );
#else

	PFSMGR_CREATESEARCHHANDLE pCreateSearchHandle;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	HANDLE retv = NULL;

	if( API_Enter( API_FSDMGR, FSMGR_CREATESEARCHHANDLE, &pCreateSearchHandle, &cs ) )
	{
		retv = pCreateSearchHandle( hVol, hProc, pfd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFSMGR_CLOSESEARCHHANDLE )(HANDLE hFile);
BOOL WINAPI FSMGR_CloseSearchHandle(HANDLE hFile)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_CLOSESEARCHHANDLE, 1 );
    cs.arg0 = (DWORD)hFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFSMGR_CLOSESEARCHHANDLE pCloseSearchHandle;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_FSDMGR, FSMGR_CLOSESEARCHHANDLE, &pCloseSearchHandle, &cs ) )
	{
		retv = pCloseSearchHandle( hFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFSMGR_DEVICEIOCONTROL )( 
                          HDSK hDsk,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped );
BOOL WINAPI FSMGR_DeviceIoControl ( 
                          HDSK hDsk,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FSDMGR, FSMGR_DEVICEIOCONTROL, 8 );
    cs.arg0 = (DWORD)hDsk;
    return (DWORD)CALL_SERVER( &cs, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
#else

	PFSMGR_DEVICEIOCONTROL pfn;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	BOOL retv = FALSE;

	if( API_Enter( API_FSDMGR, FSMGR_DEVICEIOCONTROL, &pfn, &cs ) )
	{
		retv = pfn( hDsk, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}
