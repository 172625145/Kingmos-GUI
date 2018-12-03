#include <windows.h>

HANDLE WINAPI File_CreateFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	return CreateFile( lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
}

BOOL WINAPI File_Read( HANDLE hFile, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{
	return ReadFile( hFile, lpvBuffer, dwNumToRead, lpdwNumRead, lpOverlapped );
}

BOOL WINAPI File_Write( HANDLE hFile, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped )
{
	return WriteFile( hFile, lpvBuffer, dwNumToWrite, lpdwNumWrite, pOverlapped );
}

DWORD WINAPI File_SetPointer( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod )
{
	return SetFilePointer( hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
}


DWORD WINAPI File_GetSize( HANDLE hFile, LPDWORD lpFileSizeHigh )
{
	return GetFileSize( hFile, lpFileSizeHigh );
}

BOOL WINAPI File_SetEnd( HANDLE hFile )
{
	return SetEndOfFile( hFile );
}

BOOL WINAPI File_CreateDirectory(
  LPCTSTR lpPathName,                         // directory name
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
)
{
	return CreateDirectory( lpPathName, lpSecurityAttributes );
}

BOOL WINAPI File_Delete(
  LPCTSTR lpFileName   // file name
)
{
	return DeleteFile( lpFileName );
}


HANDLE WINAPI File_FindFirst( LPCTSTR lpcszFileSpec, LPWIN32_FIND_DATA pfd )
{
	return FindFirstFile( lpcszFileSpec, pfd );
}


BOOL WINAPI File_FindNext( HANDLE hFind, LPWIN32_FIND_DATA pfd )
{
	return FindNextFile( hFind, pfd );
}

BOOL WINAPI File_FindClose( HANDLE hFind )
{
	return FindClose( hFind );
}

DWORD WINAPI File_GetAttributes(
  LPCTSTR lpFileName   // name of file or directory
)
{
	return GetFileAttributes( lpFileName );
}

BOOL WINAPI File_GetTime(
  HANDLE hFile,                 // handle to file
  LPFILETIME lpCreationTime,    // creation time
  LPFILETIME lpLastAccessTime,  // last access time
  LPFILETIME lpLastWriteTime    // last write time
)
{
	return GetFileTime( hFile, lpCreationTime, lpLastAccessTime, lpLastWriteTime );
}


