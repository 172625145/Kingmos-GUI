/******************************************************
Copyright(c) 版权所有，1998-2004微逻辑。保留所有权利。
******************************************************/

#ifndef __EMLOS_H
#define __EMLOS_H
 

LPVOID WINAPI Win32_VirtualAlloc(
    LPVOID lpAddress,
    DWORD dwSize,
    DWORD flAllocationType,
    DWORD flProtect
    );
BOOL WINAPI Win32_VirtualProtect(
    LPVOID lpAddress,
    DWORD dwSize,
    DWORD flNewProtect,
    PDWORD lpflOldProtect
    );

BOOL WINAPI Win32_VirtualFree(
  LPVOID lpAddress,  // address of region of committed pages
  DWORD dwSize,      // size of region
  DWORD dwFreeType   // type of free operation
);

BOOL WINAPI Win32_SetEvent( HANDLE hEvent );
VOID WINAPI Win32_Sleep( UINT dwTicks );
HANDLE WINAPI Win32_CreateEvent(
						 LPSECURITY_ATTRIBUTES lpEventAttributes, 
						 BOOL bManualReset,
						 BOOL bInitialState,
						 LPCTSTR lpName );
DWORD WINAPI Win32_WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds );

HANDLE WINAPI Win32_CreateFile(
  LPCTSTR lpFileName,                         // file name
  DWORD dwDesiredAccess,                      // access mode
  DWORD dwShareMode,                          // share mode
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
  DWORD dwCreationDisposition,                // how to create
  DWORD dwFlagsAndAttributes,                 // file attributes
  HANDLE hTemplateFile                        // handle to template file
);

BOOL WINAPI Win32_ReadFile(
  HANDLE hFile,                // handle to file
  LPVOID lpBuffer,             // data buffer
  DWORD nNumberOfBytesToRead,  // number of bytes to read
  LPDWORD lpNumberOfBytesRead, // number of bytes read
  LPOVERLAPPED lpOverlapped    // overlapped buffer
);

BOOL WINAPI Win32_WriteFile(
  HANDLE hFile,                    // handle to file
  LPCVOID lpBuffer,                // data buffer
  DWORD nNumberOfBytesToWrite,     // number of bytes to write
  LPDWORD lpNumberOfBytesWritten,  // number of bytes written
  LPOVERLAPPED lpOverlapped        // overlapped buffer
);

BOOL WINAPI Win32_CloseHandle(
  HANDLE hObject   // handle to object
);

BOOL WINAPI Win32_GetCommState(
  HANDLE hFile,  // handle to communications device
  LPVOID lpDCB    // device-control block
);

BOOL WINAPI Win32_SetCommState(
  HANDLE hFile,  // handle to communications device
  LPVOID lpDCB    // device-control block
);

BOOL WINAPI Win32_PurgeComm(
  HANDLE hFile,  // handle to communications resource
  DWORD dwFlags  // action to perform
);

BOOL WINAPI Win32_SetCommMask(
  HANDLE hFile,    // handle to communications device
  DWORD dwEvtMask  // mask that identifies enabled events
);

BOOL WINAPI Win32_SetupComm(
  HANDLE hFile,     // handle to communications device
  DWORD dwInQueue,  // size of input buffer
  DWORD dwOutQueue  // size of output buffer
);

BOOL WINAPI Win32_ClearCommError(
  HANDLE hFile,     // handle to communications device
  LPDWORD lpErrors, // error codes
  LPVOID lpStat  // communications status
);

BOOL WINAPI Win32_SetCommTimeouts(
  HANDLE hFile,                  // handle to comm device
  LPVOID lpCommTimeouts  // time-out values
);

BOOL WINAPI Win32_WaitCommEvent(
  HANDLE hFile,                // handle to comm device
  LPDWORD lpEvtMask,           // event type
  LPOVERLAPPED lpOverlapped  // overlapped structure
);

DWORD WINAPI Win32_SetFilePointer(
  HANDLE hFile,                // handle to file
  LONG lDistanceToMove,        // bytes to move pointer
  PLONG lpDistanceToMoveHigh,  // bytes to move pointer
  DWORD dwMoveMethod           // starting point
);

BOOL WINAPI Win32_SetEndOfFile(
  HANDLE hFile   // handle to file
);

//BOOL WINAPI Win32_SwitchToThread( VOID );
DWORD WINAPI Win32_GetTickCount( VOID );

HMODULE WINAPI Win32_LoadLibrary( LPCTSTR lpFileName );
BOOL WINAPI Win32_FreeLibrary( HMODULE hModule );

FARPROC WINAPI Win32_GetProcAddress( HMODULE hModule, LPCSTR lpProcName );
BOOL	WINAPI Win32_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat );

DWORD WINAPI Win32_GetFileSize(
  HANDLE hFile,           // handle to file
  LPDWORD lpFileSizeHigh  // high-order word of file size
);

BOOL WINAPI Win32_CreateDirectory(
	LPCTSTR lpFileName,                         // file name
	LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
);

BOOL WINAPI Win32_SetEndOfFile( HANDLE hFile );

#ifndef MAX_PATH
#define MAX_PATH         260 
#endif
#define MAX_PATH 260
typedef struct _WIN32_FIND_DATA {
  DWORD    dwFileAttributes; 
  FILETIME ftCreationTime; 
  FILETIME ftLastAccessTime; 
  FILETIME ftLastWriteTime; 
  DWORD    nFileSizeHigh; 
  DWORD    nFileSizeLow; 
  DWORD    dwReserved0; 
  DWORD    dwReserved1; 
  TCHAR    cFileName[ MAX_PATH ]; 
  TCHAR    cAlternateFileName[ 14 ]; 
} Win32_WIN32_FIND_DATA, *PWin32_WIN32_FIND_DATA, *LPWin32_WIN32_FIND_DATA; 


HANDLE WINAPI Win32_FindFirstFile(
  LPCTSTR lpFileName,               // file name
  LPWin32_WIN32_FIND_DATA lpFindFileData  // data buffer
);

BOOL WINAPI Win32_FindNextFile(
  HANDLE hFindFile,                // search handle 
  LPWin32_WIN32_FIND_DATA lpFindFileData // data buffer
);

BOOL WINAPI Win32_FindClose(
  HANDLE hFindFile   // file search handle
);

BOOL WINAPI Win32_DeleteFile(
  LPCTSTR lpFileName   // file name
);

DWORD WINAPI Win32_GetLastError( void );
DWORD WINAPI Win32_SetFilePointer(
  HANDLE hFile,                // handle to file
  LONG lDistanceToMove,        // bytes to move pointer
  PLONG lpDistanceToMoveHigh,  // bytes to move pointer
  DWORD dwMoveMethod           // starting point
);

VOID WINAPI Win32_EnterCriticalSection(
  LPCRITICAL_SECTION lpCriticalSection  // critical section
);

VOID WINAPI Win32_LeaveCriticalSection(
  LPCRITICAL_SECTION lpCriticalSection  // critical section
);

#endif
