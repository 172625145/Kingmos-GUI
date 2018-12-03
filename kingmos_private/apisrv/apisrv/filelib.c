/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/
#include <eframe.h>
#include <efile.h>
#include <eufile.h>
#include <eapisrv.h>

#include <epcore.h>


typedef BOOL ( WINAPI * PFILE_CREATEDIRECTORY )( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes);
BOOL WINAPI File_CreateDirectory( LPCTSTR lpcszPathName,
								  PSECURITY_ATTRIBUTES pSecurityAttributes)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_CREATEDIRECTORY, 2 );
    cs.arg0 = (DWORD)lpcszPathName;
    return (DWORD)CALL_SERVER( &cs, pSecurityAttributes );
#else

	PFILE_CREATEDIRECTORY pCreateDirectory;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_CREATEDIRECTORY, &pCreateDirectory, &cs ) )
	{
		lpcszPathName = MapProcessPtr( lpcszPathName, (LPPROCESS)cs.lpvData );
		pSecurityAttributes = MapProcessPtr( pSecurityAttributes, (LPPROCESS)cs.lpvData );

		retv = pCreateDirectory( lpcszPathName, pSecurityAttributes);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HANDLE ( WINAPI * PFILE_CREATEFILE )( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
HANDLE WINAPI File_CreateFile( LPCTSTR lpcszFileName,
							   DWORD dwAccess, 
							   DWORD dwShareMode, 
							   PSECURITY_ATTRIBUTES pSecurityAttributes, 
							   DWORD dwCreate, 
							   DWORD dwFlagsAndAttributes, 
							   HANDLE hTemplateFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_CREATEFILE, 7 );
    cs.arg0 = (DWORD)lpcszFileName;
    return (HANDLE)CALL_SERVER( &cs, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
#else

	PFILE_CREATEFILE pCreateFile;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_FILESYS, FILE_CREATEFILE, &pCreateFile, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );
		pSecurityAttributes = MapProcessPtr( pSecurityAttributes, (LPPROCESS)cs.lpvData );

		retv = pCreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_CLOSEFILE )( HFILE );
BOOL WINAPI File_CloseFile( HANDLE hFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_CLOSEFILE, 1 );
    cs.arg0 = (DWORD)hFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_CLOSEFILE pCloseFile;
	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_CLOSEFILE, &pCloseFile, &cs, &hFile ) )
	{
		retv = pCloseFile( hFile );//HandleToPtr(hFile, OBJ_FILE) );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_DELETEANDRENAME )( LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName );
BOOL WINAPI File_DeleteAndRename( LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_DELETEANDRENAME, 2 );
    cs.arg0 = (DWORD)lpcszNewFileName;
    return (DWORD)CALL_SERVER( &cs, lpcszOldFileName );
#else

	PFILE_DELETEANDRENAME pDeleteAndRename;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_DELETEANDRENAME, &pDeleteAndRename, &cs ) )
	{
		lpcszNewFileName = MapProcessPtr( lpcszNewFileName, (LPPROCESS)cs.lpvData );
		lpcszOldFileName = MapProcessPtr( lpcszOldFileName, (LPPROCESS)cs.lpvData );

		retv = pDeleteAndRename( lpcszNewFileName, lpcszOldFileName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_DELETE )( LPCTSTR lpcszFileName );
BOOL WINAPI File_Delete( LPCTSTR lpcszFileName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_DELETE, 1 );
    cs.arg0 = (DWORD)lpcszFileName ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_DELETE pDelete;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_DELETE, &pDelete, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );

		retv = pDelete( lpcszFileName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_DEVICEIOCONTROL )( HANDLE hFile, DWORD dwIoControlCode,LPVOID lpInBuf,DWORD nInBufSize,LPVOID lpOutBuf,DWORD nOutBufSize,LPDWORD lpdwBytesReturned,LPOVERLAPPED lpOverlapped );
BOOL WINAPI File_DeviceIoControl( 
                          HANDLE hFile,
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
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_DEVICEIOCONTROL, 8 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
#else

	PFILE_DEVICEIOCONTROL pDeviceIoControl;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_DEVICEIOCONTROL, &pDeviceIoControl, &cs, &hFile ) )
	{
        lpInBuf = MapProcessPtr( lpInBuf, (LPPROCESS)cs.lpvData );
		lpOutBuf = MapProcessPtr( lpOutBuf, (LPPROCESS)cs.lpvData );
		lpdwBytesReturned = MapProcessPtr( lpdwBytesReturned, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );

		retv = pDeviceIoControl( hFile, dwIoControlCode, lpInBuf, nInBufSize, lpOutBuf, nOutBufSize, lpdwBytesReturned, lpOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_FINDCLOSE )( HANDLE hFind );
BOOL WINAPI File_FindClose( HANDLE hFind )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_FINDCLOSE, 1 );
    cs.arg0 = (DWORD)hFind ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_FINDCLOSE pFindClose;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_FINDCLOSE, &pFindClose, &cs ) )
	{
		retv = pFindClose( hFind );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef HANDLE ( WINAPI * PFILE_FINDFIRST )( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd );
HANDLE WINAPI File_FindFirst( LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_FINDFIRST, 2 );
    cs.arg0 = (DWORD)lpcszFileSpec;
    return (HANDLE)CALL_SERVER( &cs, pfd );
#else

	PFILE_FINDFIRST pFindFirst;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	HANDLE   retv = NULL;

	if( API_Enter( API_FILESYS, FILE_FINDFIRST, &pFindFirst, &cs ) )
	{
		lpcszFileSpec = MapProcessPtr( lpcszFileSpec, (LPPROCESS)cs.lpvData );
		pfd = MapProcessPtr( pfd, (LPPROCESS)cs.lpvData );

		retv = pFindFirst( lpcszFileSpec, pfd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_FINDNEXT )( HANDLE hFind, FILE_FIND_DATA * pfd );
BOOL WINAPI File_FindNext( HANDLE hFind, FILE_FIND_DATA * pfd )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_FINDNEXT, 2 );
    cs.arg0 = (DWORD)hFind;
    return (DWORD)CALL_SERVER( &cs, pfd );
#else

	PFILE_FINDNEXT pFindNext;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_FINDNEXT, &pFindNext, &cs ) )
	{
		pfd = MapProcessPtr( pfd, (LPPROCESS)cs.lpvData );

		retv = pFindNext( hFind, pfd );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_FLUSHBUFFERS )( HANDLE hFile );
BOOL WINAPI File_FlushBuffers( HANDLE hFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_FLUSHBUFFERS, 1 );
    cs.arg0 = (DWORD)hFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_FLUSHBUFFERS pFlushBuffers;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_FLUSHBUFFERS, &pFlushBuffers, &cs, &hFile ) )
	{
		retv = pFlushBuffers( hFile );//HandleToPtr(hFile, OBJ_FILE) );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_GETDISKFREESPACEEX )( 
						    LPCTSTR lpcszPathName, 
                            LPDWORD lpdwFreeBytesAvailableToCaller, 
                            LPDWORD lpdwTotalNumberOfBytes, 
                            LPDWORD lpdwTotalNumberOfFreeBytes );
BOOL WINAPI File_GetDiskFreeSpaceEx( 
						    LPCTSTR lpcszPathName, 
                            LPDWORD lpdwFreeBytesAvailableToCaller, 
                            LPDWORD lpdwTotalNumberOfBytes, 
                            LPDWORD lpdwTotalNumberOfFreeBytes )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_GETDISKFREESPACEEX, 4 );
    cs.arg0 = (DWORD)lpcszPathName;
    return (DWORD)CALL_SERVER( &cs, lpdwFreeBytesAvailableToCaller, lpdwTotalNumberOfBytes, lpdwTotalNumberOfFreeBytes );
#else

	PFILE_GETDISKFREESPACEEX pGetDiskFreeSpaceEx;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_GETDISKFREESPACEEX, &pGetDiskFreeSpaceEx, &cs ) )
	{
		lpcszPathName = MapProcessPtr( lpcszPathName, (LPPROCESS)cs.lpvData );
		lpdwFreeBytesAvailableToCaller = MapProcessPtr( lpdwFreeBytesAvailableToCaller, (LPPROCESS)cs.lpvData );
		lpdwTotalNumberOfBytes = MapProcessPtr( lpdwTotalNumberOfBytes, (LPPROCESS)cs.lpvData );
		lpdwTotalNumberOfFreeBytes = MapProcessPtr( lpdwTotalNumberOfFreeBytes, (LPPROCESS)cs.lpvData );

		retv = pGetDiskFreeSpaceEx( lpcszPathName, lpdwFreeBytesAvailableToCaller,lpdwTotalNumberOfBytes, lpdwTotalNumberOfFreeBytes);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PFILE_GETATTRIBUTES )( LPCTSTR lpcszFileName );
DWORD WINAPI File_GetAttributes( LPCTSTR lpcszFileName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_GETATTRIBUTES, 1 );
    cs.arg0 = (DWORD)lpcszFileName ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_GETATTRIBUTES pGetAttributes;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	DWORD   retv = -1;

	if( API_Enter( API_FILESYS, FILE_GETATTRIBUTES, &pGetAttributes, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );

		retv = pGetAttributes( lpcszFileName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PFILE_GETSIZE )( HANDLE hFile, LPDWORD pFileSizeHigh );
DWORD WINAPI File_GetSize( HANDLE hFile, LPDWORD pFileSizeHigh )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_GETSIZE, 2 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, pFileSizeHigh );
#else

	PFILE_GETSIZE pGetSize;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	DWORD   retv = INVALID_FILE_SIZE;

	if( API_EnterHandle( API_FILESYS, FILE_GETSIZE, &pGetSize, &cs, &hFile ) )
	{
		pFileSizeHigh = MapProcessPtr( pFileSizeHigh, (LPPROCESS)cs.lpvData );

		retv = pGetSize( hFile, pFileSizeHigh );//HandleToPtr(hFile, OBJ_FILE), pFileSizeHigh );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_GETINFORMATION )( HANDLE hFile, FILE_INFORMATION * pfi );
BOOL WINAPI File_GetInformation( HANDLE hFile, FILE_INFORMATION * pfi )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_GETINFORMATION, 2 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, pfi );
#else

	PFILE_GETINFORMATION pGetInformation;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_GETINFORMATION, &pGetInformation, &cs, &hFile ) )
	{
		pfi = MapProcessPtr( pfi, (LPPROCESS)cs.lpvData );

		retv = pGetInformation( hFile, pfi );//HandleToPtr(hFile, OBJ_FILE), pfi );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_GETTIME )( HANDLE hFile, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite );
BOOL WINAPI File_GetTime( HANDLE hFile, 
						  FILETIME * pCreate, 
						  FILETIME * pLastAccess, 
						  FILETIME * pLastWrite )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_GETTIME, 4 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, pCreate, pLastAccess, pLastWrite );
#else

	PFILE_GETTIME pGetTime;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_GETTIME, &pGetTime, &cs, &hFile ) )
	{
		pCreate = MapProcessPtr( pCreate, (LPPROCESS)cs.lpvData );
		pLastAccess = MapProcessPtr( pLastAccess, (LPPROCESS)cs.lpvData );
		pLastWrite = MapProcessPtr( pLastWrite, (LPPROCESS)cs.lpvData );

		retv = pGetTime( hFile, pCreate, pLastAccess, pLastWrite );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_MOVE )( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName );
BOOL WINAPI File_Move( LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_MOVE, 2 );
    cs.arg0 = (DWORD)lpcszOldFileName;
    return (DWORD)CALL_SERVER( &cs, lpcszNewFileName );
#else

	PFILE_MOVE pMove;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_MOVE, &pMove, &cs ) )
	{
		lpcszOldFileName = MapProcessPtr( lpcszOldFileName, (LPPROCESS)cs.lpvData );
		lpcszNewFileName = MapProcessPtr( lpcszNewFileName, (LPPROCESS)cs.lpvData );

		retv = pMove( lpcszOldFileName, lpcszNewFileName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_READ )( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );
BOOL WINAPI File_Read( HANDLE hFile, 
					   LPVOID lpvBuffer, 
					   DWORD dwNumToRead, 
					   LPDWORD lpdwNumRead, 
					   LPOVERLAPPED lpOverlapped )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_READ, 5 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
#else

	PFILE_READ pRead;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;
	//RETAILMSG( 1, ( "File_Read0:cur=%x.\r\n", GetCurrentProcess() ) );

	if( API_EnterHandle( API_FILESYS, FILE_READ, &pRead, &cs, &hFile ) )
	{
		lpvBuffer = MapProcessPtr( lpvBuffer, (LPPROCESS)cs.lpvData );
		lpdwNumRead = MapProcessPtr( lpdwNumRead, (LPPROCESS)cs.lpvData );
		lpOverlapped = MapProcessPtr( lpOverlapped, (LPPROCESS)cs.lpvData );

		retv = pRead( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_READWITHSEEK )( HANDLE hFile, LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,DWORD dwLowOffset,DWORD dwHighOffset );
BOOL WINAPI File_ReadWithSeek( HANDLE hFile, 
							   LPVOID pBuffer, 
							   DWORD cbRead, 
							   LPDWORD pcbRead,
							   LPOVERLAPPED pOverlapped,
							   DWORD dwLowOffset,
							   DWORD dwHighOffset )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_READWITHSEEK, 7 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, pBuffer, cbRead, pcbRead, pOverlapped, dwLowOffset, dwHighOffset );
#else

	PFILE_READWITHSEEK pReadWithSeek;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_READWITHSEEK, &pReadWithSeek, &cs, &hFile ) )
	{
		pBuffer = MapProcessPtr( pBuffer, (LPPROCESS)cs.lpvData );
		pcbRead = MapProcessPtr( pcbRead, (LPPROCESS)cs.lpvData );
		pOverlapped = MapProcessPtr( pOverlapped, (LPPROCESS)cs.lpvData );

		retv = pReadWithSeek( hFile, pBuffer, cbRead, pcbRead, pOverlapped, dwLowOffset, dwHighOffset );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_REMOVEDIRECTORY )( LPCTSTR lpcszPathName );
BOOL WINAPI File_RemoveDirectory( LPCTSTR lpcszPathName )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_REMOVEDIRECTORY, 1 );
    cs.arg0 = (DWORD)lpcszPathName ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_REMOVEDIRECTORY pRemoveDirectory;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_REMOVEDIRECTORY, &pRemoveDirectory, &cs ) )
	{
		lpcszPathName = MapProcessPtr( lpcszPathName, (LPPROCESS)cs.lpvData );

		retv = pRemoveDirectory( lpcszPathName );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_SETEND )( HANDLE hFile );
BOOL WINAPI File_SetEnd( HANDLE hFile )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_SETEND, 1 );
    cs.arg0 = (DWORD)hFile ;
    return (DWORD)CALL_SERVER( &cs );
#else

	PFILE_SETEND pSetEnd;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_SETEND, &pSetEnd, &cs, &hFile ) )
	{
		//retv = pSetEnd( HandleToPtr(hFile, OBJ_FILE) );
		retv = pSetEnd( hFile );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_SETATTRIBUTES )( LPCTSTR lpcszFileName, DWORD dwFileAttributes );
BOOL WINAPI File_SetAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_SETATTRIBUTES, 2 );
    cs.arg0 = (DWORD)lpcszFileName;
    return (DWORD)CALL_SERVER( &cs, dwFileAttributes );
#else

	PFILE_SETATTRIBUTES pSetAttributes;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_SETATTRIBUTES, &pSetAttributes, &cs ) )
	{
		lpcszFileName = MapProcessPtr( lpcszFileName, (LPPROCESS)cs.lpvData );

		retv = pSetAttributes( lpcszFileName, dwFileAttributes );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef DWORD ( WINAPI * PFILE_SETPOINTER )( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
DWORD WINAPI File_SetPointer( HANDLE hFile, 
							  LONG lDistanceToMove, 
							  LPLONG lpDistanceToMoveHigh, 
							  DWORD dwMoveMethod )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_SETPOINTER, 4 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
#else

	PFILE_SETPOINTER pSetPointer;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_SETPOINTER, &pSetPointer, &cs, &hFile ) )
	{
		lpDistanceToMoveHigh = MapProcessPtr( lpDistanceToMoveHigh, (LPPROCESS)cs.lpvData );
		retv = pSetPointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_SETTIME )( HANDLE hFile, const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite );
BOOL WINAPI File_SetTime( HANDLE hFile, 
						  const FILETIME * pCreate, 
						  const FILETIME * pLastAccess, 
						  const FILETIME * pLastWrite )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_SETTIME, 4 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, pCreate, pLastAccess, pLastWrite );
#else

	PFILE_SETTIME pSetTime;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_SETTIME, &pSetTime, &cs, &hFile ) )
	{
		pCreate = MapProcessPtr( pCreate, (LPPROCESS)cs.lpvData );
		pLastAccess = MapProcessPtr( pLastAccess, (LPPROCESS)cs.lpvData );
		pLastWrite = MapProcessPtr( pLastWrite, (LPPROCESS)cs.lpvData );

		retv = pSetTime( hFile, pCreate, pLastAccess, pLastWrite );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_WRITE )( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );
BOOL WINAPI File_Write( HANDLE hFile, 
					    LPCVOID lpvBuffer, 
						DWORD dwNumToWrite, 
						LPDWORD lpdwNumWrite, 
						LPOVERLAPPED pOverlapped )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_WRITE, 5 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
#else

	PFILE_WRITE pWrite;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_WRITE, &pWrite, &cs, &hFile ) )
	{
		lpvBuffer = MapProcessPtr( lpvBuffer, (LPPROCESS)cs.lpvData );
		lpdwNumWrite = MapProcessPtr( lpdwNumWrite, (LPPROCESS)cs.lpvData );
		pOverlapped = MapProcessPtr( pOverlapped, (LPPROCESS)cs.lpvData );

		retv = pWrite( hFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_WRITEWITHSEEK )( HANDLE hFile, LPCVOID lpcvBuffer, DWORD dwWrite, LPDWORD lpdwWritten, DWORD dwLowOffset, DWORD dwHighOffset );
BOOL WINAPI File_WriteWithSeek( HANDLE hFile, 
							   LPCVOID lpcvBuffer, 
							   DWORD dwWrite, 
							   LPDWORD lpdwWritten, 
							   DWORD dwLowOffset, 
							   DWORD dwHighOffset )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API_HANDLE( API_FILESYS, FILE_WRITEWITHSEEK, 6 );
    cs.arg0 = (DWORD)hFile;
    return (DWORD)CALL_SERVER( &cs, lpcvBuffer, dwWrite, lpdwWritten, dwLowOffset, dwHighOffset );
#else

	PFILE_WRITEWITHSEEK pWriteWithSeek;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_EnterHandle( API_FILESYS, FILE_WRITEWITHSEEK, &pWriteWithSeek, &cs, &hFile ) )
	{
		lpcvBuffer = MapProcessPtr( lpcvBuffer, (LPPROCESS)cs.lpvData );
		lpdwWritten = MapProcessPtr( lpdwWritten, (LPPROCESS)cs.lpvData );

		retv = pWriteWithSeek( hFile, lpcvBuffer, dwWrite, lpdwWritten, dwLowOffset, dwHighOffset );
		API_Leave(  );
	}
	return retv;
#endif
}

typedef BOOL ( WINAPI * PFILE_COPY )( LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists );
BOOL WINAPI File_Copy( LPCTSTR lpExistingFileName, 
					   LPCTSTR lpNewFileName, 
					   BOOL bFailIfExists )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_FILESYS, FILE_COPY, 3 );
    cs.arg0 = (DWORD)lpExistingFileName;
    return (DWORD)CALL_SERVER( &cs, lpNewFileName, bFailIfExists );
#else

	PFILE_COPY pCopy;// = (PRGN_EQUAL)lpRgnAPI[RGN_EQUAL];

	CALLSTACK cs;
	BOOL   retv = FALSE;

	if( API_Enter( API_FILESYS, FILE_COPY, &pCopy, &cs ) )
	{
		lpExistingFileName = MapProcessPtr( lpExistingFileName, (LPPROCESS)cs.lpvData );
		lpNewFileName = MapProcessPtr( lpNewFileName, (LPPROCESS)cs.lpvData );

		retv = pCopy( lpExistingFileName, lpNewFileName, bFailIfExists );
		API_Leave(  );
	}
	return retv;
#endif
}
