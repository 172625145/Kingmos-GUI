/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/

#include <eframe.h>
#include <efile.h>
#include <eudevice.h>
#include <edevice.h>
#include <eapisrv.h>
#include <epcore.h>
#include <devdrv.h>
#include <miscellany.h>
//#define CALL_TRAP
//        64M     256S
//FFFF HMMM MMMF SSSS SSSS PPPP 0000 0000
//#define API_MAKE( id, option, arg_num ) ( ( id << 22 ) | ( option << 12 ) | (arg_num << 8) )

//#undef CALL_TRAP
typedef HANDLE ( WINAPI * PDEV_REGISTER )( LPCTSTR lpDevName, UINT index, LPCTSTR lpszLib, DWORD dwParam );
HANDLE WINAPI Dev_RegisterDevice( LPCTSTR lpDevName, UINT index, LPCTSTR lpszLib, DWORD dwParam )
{
//	RETAILMSG( 1, ( "Dev_Register:%s,lpDriver=%x.\r\n", lpDevName, lpDriver ) );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_REGISTER, 4 );
    cs.arg0 = (DWORD)lpDevName;
    return (HANDLE)CALL_SERVER( &cs, index, lpszLib, dwParam );
#else
	PDEV_REGISTER pRegister;// = (PDEV_REGISTER)lpDeviceAPI[DEV_REGISTER];

	CALLSTACK cs;
	HANDLE  retv = NULL;

	if( API_Enter( API_DEV, DEV_REGISTER, &pRegister, &cs ) )
	{
		lpDevName = MapProcessPtr( lpDevName, (LPPROCESS)cs.lpvData );

		retv = pRegister( lpDevName, index, lpszLib, dwParam );

		API_Leave(  );
	}
	return retv;
#endif
}
/*
HANDLE WINAPI Dev_RegisterDriver( 
                    LPCTSTR lpDevName, 
                    UINT index, 
                    const DEVICE_DRIVER FAR * lpDriver, 
                    DWORD dwParam )
{
	return Device_RegisterDriver( lpDevName, index, lpDriver, dwParam );
}
*/

typedef BOOL ( WINAPI * PDEV_DEREGISTER )( HANDLE );
BOOL WINAPI Dev_Deregister( HANDLE handle )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_DEREGISTER, 1 );
    cs.arg0 = (DWORD)handle ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDEV_DEREGISTER pDeregister;// = (PDEV_DEREGISTER)lpDeviceAPI[DEV_DEREGISTER];

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV, DEV_DEREGISTER, &pDeregister, &cs ) )
	{
		retv = pDeregister( handle );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_ENUM )( LPTSTR lpszDevList, LPDWORD lpdwBuflen );
DWORD WINAPI Dev_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_ENUM, 2 );
    cs.arg0 = (DWORD)lpszDevList;
    return (DWORD)CALL_SERVER( &cs, lpdwBuflen );
#else

	PDEV_ENUM pfn;

	CALLSTACK cs;
	BOOL  retv = ERROR_SUCCESS;

	if( API_Enter( API_DEV, DEV_ENUM, &pfn, &cs ) )
	{
		lpszDevList = MapProcessPtr( lpszDevList, (LPPROCESS)cs.lpvData );
		lpdwBuflen = MapProcessPtr( lpdwBuflen, (LPPROCESS)cs.lpvData );

		retv = pfn( lpszDevList, lpdwBuflen );
		API_Leave(  );
	}
	return retv;

#endif
}

typedef BOOL ( WINAPI * PDEV_REGISTERFSD )( LPCTSTR lpcszFSDName, HANDLE hFSD, const FSDINITDRV * lpfsd );
API_TYPE BOOL WINAPI Dev_RegisterFSD( LPCTSTR lpcszFSDName, HANDLE hFSD, const FSDINITDRV * lpfsd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_REGISTERFSD, 3 );
    cs.arg0 = (DWORD)lpcszFSDName;
    return (DWORD)CALL_SERVER( &cs, hFSD, lpfsd );
#else

	PDEV_REGISTERFSD pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV, DEV_REGISTERFSD, &pfn, &cs ) )
	{
		lpcszFSDName = MapProcessPtr( lpcszFSDName, (LPPROCESS)cs.lpvData );

		retv = pfn( lpcszFSDName, hFSD, lpfsd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_UNREGISTERFSD )( LPCTSTR lpcszFSDName );
API_TYPE BOOL WINAPI Dev_UnregisterFSD( LPCTSTR lpcszFSDName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_UNREGISTERFSD, 1 );
    cs.arg0 = (DWORD)lpcszFSDName ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDEV_UNREGISTERFSD pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV, DEV_UNREGISTERFSD, &pfn, &cs ) )
	{
		lpcszFSDName = MapProcessPtr( lpcszFSDName, (LPPROCESS)cs.lpvData );

		retv = pfn( lpcszFSDName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_LOADFSD )( HANDLE hDevice, LPCTSTR lpcszFSDName );
BOOL WINAPI Dev_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_LOADFSD, 2 );
    cs.arg0 = (DWORD)hDevice;
    return (DWORD)CALL_SERVER( &cs, lpcszFSDName );
#else

	PDEV_LOADFSD pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV, DEV_LOADFSD, &pfn, &cs ) )
	{
		lpcszFSDName = MapProcessPtr( lpcszFSDName, (LPPROCESS)cs.lpvData );

		retv = pfn( hDevice, lpcszFSDName );


		API_Leave(  );
		//RETAILMSG( 1, ( "UUUU.\r\n" ) );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_UNLOADFSD )( HANDLE hDevice );
BOOL WINAPI Dev_UnloadFSD ( HANDLE hDevice )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_UNLOADFSD, 1 );
    cs.arg0 = (DWORD)hDevice;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDEV_UNLOADFSD pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV, DEV_UNLOADFSD, &pfn, &cs ) )
	{
		retv = pfn( hDevice );
		API_Leave();
	}
	return retv;
#endif
}


typedef HANDLE ( WINAPI * PDEV_CREATE )( LPCTSTR lpszName, DWORD dwAccess, DWORD dwShareMode, HANDLE hProc );
HANDLE WINAPI Dev_CreateFile( LPCTSTR lpszName, DWORD dwAccess, DWORD dwShareMode, HANDLE hProc )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_CREATEFILE, 4 );
    cs.arg0 = (DWORD)lpszName;
    return (HANDLE)CALL_SERVER( &cs, dwAccess, dwShareMode, hProc );
#else

	PDEV_CREATE pCreate;

	CALLSTACK cs;
	HANDLE  retv = NULL;

	if( API_Enter( API_DEV_FILE, DEV_CREATEFILE, &pCreate, &cs ) )
	{
		lpszName = MapProcessPtr( lpszName, (LPPROCESS)cs.lpvData );

		retv = pCreate( lpszName, dwAccess, dwShareMode, hProc );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_CLOSE )(HANDLE hOpenFile);
BOOL WINAPI Dev_CloseFile( HANDLE hOpenFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_CLOSEFILE, 1 );
    cs.arg0 = (DWORD)hOpenFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDEV_CLOSE pClose;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_CLOSEFILE, &pClose, &cs ) )
	{
		retv = pClose( hOpenFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_READFILE )( HANDLE hOpenFile, LPVOID lpvBuffer, DWORD dwBytesToRead, LPDWORD lpdwNumBytesRead, LPOVERLAPPED lpOverlapped );
BOOL WINAPI Dev_ReadFile( HANDLE hOpenFile, 
						  LPVOID lpvBuffer, 
						  DWORD dwBytesToRead, 
						  LPDWORD lpdwNumBytesRead, 
						  LPOVERLAPPED lpOverlapped )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_READFILE, 5 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lpvBuffer, dwBytesToRead, lpdwNumBytesRead, lpOverlapped );
#else

	PDEV_READFILE pReadFile;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_READFILE, &pReadFile, &cs ) )
	{
		lpvBuffer = MapProcessPtr( lpvBuffer, (LPPROCESS)cs.lpvData );
		lpdwNumBytesRead = MapProcessPtr( lpdwNumBytesRead, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );

		retv = pReadFile( hOpenFile, lpvBuffer, dwBytesToRead, lpdwNumBytesRead, lpOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_WRITEFILE )( HANDLE hOpenFile, LPCVOID lpvBuffer, DWORD dwBytesToWrite, LPDWORD lpdwNumBytesWritten, LPOVERLAPPED lpOverlapped );
BOOL WINAPI Dev_WriteFile( HANDLE hOpenFile,
						   LPCVOID lpvBuffer,
						   DWORD dwBytesToWrite,
						   LPDWORD lpdwNumBytesWritten,
						   LPOVERLAPPED lpOverlapped )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_WRITEFILE, 5 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lpvBuffer, dwBytesToWrite, lpdwNumBytesWritten, lpOverlapped );
#else

    PDEV_WRITEFILE pWriteFile;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_WRITEFILE, &pWriteFile, &cs ) )
	{
		lpvBuffer = MapProcessPtr( lpvBuffer, (LPPROCESS)cs.lpvData );
		lpdwNumBytesWritten = MapProcessPtr( lpdwNumBytesWritten, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );

		retv = pWriteFile( hOpenFile, lpvBuffer, dwBytesToWrite, lpdwNumBytesWritten, lpOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_FLUSHFILEBUFFERS )(HANDLE hOpenFile);
BOOL WINAPI Dev_FlushFileBuffers( HANDLE hOpenFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_FLUSHFILEBUFFERS, 1 );
    cs.arg0 = (DWORD)hOpenFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDEV_FLUSHFILEBUFFERS pFlush;
	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_FLUSHFILEBUFFERS, &pFlush, &cs ) )
	{
		retv = pFlush( hOpenFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_SETFILETIME )(HANDLE hOpenFile, CONST FILETIME *lpCreation, CONST FILETIME *lpLastAccess, CONST FILETIME *lpLastWrite);
BOOL WINAPI Dev_SetFileTime( HANDLE hOpenFile, 
							 CONST FILETIME *lpCreation, 
							 CONST FILETIME *lpLastAccess,
							 CONST FILETIME *lpLastWrite )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_SETFILETIME, 4 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lpCreation, lpLastAccess, lpLastWrite );
#else

	PDEV_SETFILETIME pSetFileTime;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_SETFILETIME, &pSetFileTime, &cs ) )
	{
		lpCreation = MapProcessPtr( lpCreation, (LPPROCESS)cs.lpvData );
		lpLastAccess = MapProcessPtr( lpLastAccess, (LPPROCESS)cs.lpvData );
		lpLastWrite = MapProcessPtr( lpLastWrite, (LPPROCESS)cs.lpvData );

		retv = pSetFileTime( hOpenFile, lpCreation, lpLastAccess, lpLastWrite );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_GETFILETIME )(HANDLE hOpenFile, LPFILETIME lpftCreation, LPFILETIME lpftLastAccess, LPFILETIME lpftLastWrite);
BOOL WINAPI Dev_GetFileTime( HANDLE hOpenFile, 
							 LPFILETIME lpftCreation,
							 LPFILETIME lpftLastAccess,
							 LPFILETIME lpftLastWrite )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_GETFILETIME, 4 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lpftCreation, lpftLastAccess, lpftLastWrite );
#else

	PDEV_GETFILETIME pGetFileTime;

	CALLSTACK cs;
	BOOL  retv = NULL;

	if( API_Enter( API_DEV_FILE, DEV_GETFILETIME, &pGetFileTime, &cs ) )
	{
		lpftCreation = MapProcessPtr( lpftCreation, (LPPROCESS)cs.lpvData );
		lpftLastAccess = MapProcessPtr( lpftLastAccess, (LPPROCESS)cs.lpvData );
		lpftLastWrite = MapProcessPtr( lpftLastWrite, (LPPROCESS)cs.lpvData );

		retv = pGetFileTime( hOpenFile, lpftCreation, lpftLastAccess, lpftLastWrite );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_GETFILESIZE )( HANDLE hOpenFile, LPDWORD lpdwFileSizeHigh );
DWORD Dev_GetFileSize( HANDLE hOpenFile, LPDWORD lpdwFileSizeHigh )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_GETFILESIZE, 2 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lpdwFileSizeHigh );
#else

	PDEV_GETFILESIZE pGetFileSize;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_GETFILESIZE, &pGetFileSize, &cs ) )
	{
		lpdwFileSizeHigh = MapProcessPtr( lpdwFileSizeHigh, (LPPROCESS)cs.lpvData );

		retv = pGetFileSize( hOpenFile, lpdwFileSizeHigh );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_SETENDOFFILE )(HANDLE);
BOOL WINAPI Dev_SetEndOfFile( HANDLE hOpenFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_SETENDOFFILE, 1 );
    cs.arg0 = (DWORD)hOpenFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PDEV_SETENDOFFILE pSetEndOfFile;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_SETENDOFFILE, &pSetEndOfFile, &cs ) )
	{
		retv = pSetEndOfFile( hOpenFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_SETFILEPOINTER )( HANDLE hOpenFile, LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );

DWORD WINAPI Dev_SetFilePointer(
						 HANDLE hOpenFile, 
						 LONG lDistanceToMove, 
						 PLONG lpDistanceToMoveHigh,
						 DWORD dwMoveMethod ) 
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_SETFILEPOINTER, 4 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
#else

	PDEV_SETFILEPOINTER pSetFilePointer;

	CALLSTACK cs;
	DWORD  retv = 0xffffffff;

	if( API_Enter( API_DEV_FILE, DEV_SETFILEPOINTER, &pSetFilePointer, &cs ) )
	{
		lpDistanceToMoveHigh = MapProcessPtr( lpDistanceToMoveHigh, (LPPROCESS)cs.lpvData );

		retv = pSetFilePointer( hOpenFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_GETFILEINFORMATIONBYHANDLE )(HANDLE hOpenFile, LPBY_HANDLE_FILE_INFORMATION lpFileInfo);
BOOL WINAPI Dev_GetFileInformationByHandle( HANDLE hOpenFile, LPBY_HANDLE_FILE_INFORMATION lpFileInfo )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_GETFILEINFORMATIONBYHANDLE, 2 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, lpFileInfo );
#else

	PDEV_GETFILEINFORMATIONBYHANDLE pGetFileInfo;
	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_GETFILEINFORMATIONBYHANDLE, &pGetFileInfo, &cs ) )
	{
	    lpFileInfo = MapProcessPtr( lpFileInfo, (LPPROCESS)cs.lpvData );

		retv = pGetFileInfo( hOpenFile, lpFileInfo );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_IOCONTROL )(HANDLE hOpenFile, DWORD  dwIoControlCode, LPVOID lpvInBuf, DWORD dwInBufSize, LPVOID lpvOutBuf, DWORD dwOutBufSize, LPDWORD lpdwBytesReturned, LPOVERLAPPED lpOverlapped);
BOOL WINAPI Dev_IoControl( HANDLE hOpenFile,
						   DWORD  dwIoControlCode,
						   LPVOID lpvInBuf,
						   DWORD dwInBufSize,
						   LPVOID lpvOutBuf,
						   DWORD dwOutBufSize,
						   LPDWORD lpdwBytesReturned,
						   LPOVERLAPPED lpOverlapped )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV_FILE, DEV_IOCONTROL, 8 );
    cs.arg0 = (DWORD)hOpenFile;
    return (DWORD)CALL_SERVER( &cs, dwIoControlCode, lpvInBuf, dwInBufSize, lpvOutBuf, dwOutBufSize, lpdwBytesReturned, lpOverlapped );
#else

	PDEV_IOCONTROL pIoControl;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV_FILE, DEV_IOCONTROL, &pIoControl, &cs ) )
	{
		lpvInBuf = MapProcessPtr( lpvInBuf, (LPPROCESS)cs.lpvData );
		lpvOutBuf = MapProcessPtr( lpvOutBuf, (LPPROCESS)cs.lpvData );
		lpdwBytesReturned = MapProcessPtr( lpdwBytesReturned, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );
		
		retv = pIoControl( hOpenFile, dwIoControlCode, lpvInBuf, dwInBufSize, lpvOutBuf, dwOutBufSize, lpdwBytesReturned, lpOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PDEV_GetSystemPowerStatusEx )(PSYSTEM_POWER_STATUS_EX pstatus, BOOL fUpdate);
BOOL WINAPI Dev_GetSystemPowerStatusEx(
				PSYSTEM_POWER_STATUS_EX pstatus, 
				BOOL fUpdate )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_DEV, DEV_GETSYSTEMPOWERSTATUSEX, 2 );
    cs.arg0 = (DWORD)pstatus;
    return (DWORD)CALL_SERVER( &cs, fUpdate );
#else

	PDEV_GetSystemPowerStatusEx pfn;

	CALLSTACK cs;
	BOOL  retv = FALSE;

	if( API_Enter( API_DEV, DEV_GETSYSTEMPOWERSTATUSEX, &pfn, &cs ) )
	{
		pstatus = (PSYSTEM_POWER_STATUS_EX)MapProcessPtr( pstatus, (LPPROCESS)cs.lpvData );
	
		retv = pfn( pstatus, fUpdate );
		API_Leave();
	}
	return retv;
#endif
}

