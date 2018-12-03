/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_PCFILE_H_
#define	_PCFILE_H_

#ifdef __cplusplus
extern "C" {
#endif


#ifdef	VC386//EML_WIN32
//在KINGMOS PC版上

//
extern	HMODULE	pc_LoadLibrary( LPCTSTR lpFileName );
extern	BOOL	pc_FreeLibrary( HMODULE hModule );
extern	FARPROC pc_GetProcAddress( HMODULE hModule, LPCSTR lpProcName );

//
extern	HANDLE	pc_CreateFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
extern	BOOL	pc_ReadFile( HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped );
extern	BOOL	pc_WriteFile( HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped );
extern	DWORD	pc_SetFilePointer( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
extern	DWORD	pc_GetFileSize( HANDLE hFile, LPDWORD lpFileSizeHigh );
extern	BOOL	pc_CloseHandle( HANDLE hObject );

//
extern	BOOL	pc_GetCommState( HANDLE hFile, LPDCB lpDCB );
extern	BOOL	pc_SetCommState( HANDLE hFile, LPDCB lpDCB );
extern	BOOL	pc_GetCommTimeouts( HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts );
extern	BOOL	pc_SetCommTimeouts( HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts );
extern	BOOL	pc_SetCommMask( HANDLE hFile, DWORD dwEvtMask );
extern	BOOL	pc_SetupComm( HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue );
extern	BOOL	pc_PurgeComm( HANDLE hFile, DWORD dwFlags );
extern	BOOL	pc_ClearCommError( HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat );

extern	BOOL	pc_TransmitCommChar(HANDLE hFile, char cChar );
extern	BOOL	pc_WaitCommEvent(HANDLE hFile, LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped );
extern	BOOL	pc_SetCommBreak(HANDLE hFile );
extern	BOOL	pc_ClearCommBreak(HANDLE hFile );
extern	BOOL	pc_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat );
extern	BOOL	pc_GetCommProperties(HANDLE hFile, LPCOMMPROP lpCommProp );
extern	BOOL	pc_EscapeCommFunction(HANDLE hFile, DWORD dwFunc );

#define	ver_LoadLibrary			pc_LoadLibrary		
#define	ver_FreeLibrary			pc_FreeLibrary			
#define	ver_GetProcAddress		pc_GetProcAddress		

#define	ver_CreateFile			pc_CreateFile		
#define	ver_ReadFile			pc_ReadFile			
#define	ver_WriteFile			pc_WriteFile		
#define	ver_SetFilePointer		pc_SetFilePointer		
#define	ver_GetFileSize			pc_GetFileSize		
#define	ver_CloseHandle			pc_CloseHandle		

#define	ver_GetCommState		pc_GetCommState		
#define	ver_SetCommState		pc_SetCommState		
#define	ver_GetCommTimeouts		pc_GetCommTimeouts	
#define	ver_SetCommTimeouts		pc_SetCommTimeouts	
#define	ver_SetCommMask			pc_SetCommMask		
#define	ver_SetupComm			pc_SetupComm		
#define	ver_PurgeComm			pc_PurgeComm		
#define	ver_ClearCommError		pc_ClearCommError		

#define	ver_TransmitCommChar	pc_TransmitCommChar
#define	ver_WaitCommEvent		pc_WaitCommEvent
#define	ver_SetCommBreak		pc_SetCommBreak
#define	ver_ClearCommBreak		pc_ClearCommBreak
#define	ver_GetCommModemStatus	pc_GetCommModemStatus
#define	ver_GetCommProperties	pc_GetCommProperties
#define	ver_EscapeCommFunction	pc_EscapeCommFunction

#else
//在KINGMOS 版上

#define	ver_LoadLibrary			LoadLibrary		
#define	ver_FreeLibrary			FreeLibrary			
#define	ver_GetProcAddress		GetProcAddress		

#define	ver_CreateFile			CreateFile		
#define	ver_ReadFile			ReadFile			
#define	ver_WriteFile			WriteFile		
#define	ver_SetFilePointer		SetFilePointer		
#define	ver_GetFileSize			GetFileSize		
#define	ver_CloseHandle			CloseHandle		

#define	ver_GetCommState		GetCommState		
#define	ver_SetCommState		SetCommState		
#define	ver_GetCommTimeouts		GetCommTimeouts	
#define	ver_SetCommTimeouts		SetCommTimeouts	
#define	ver_SetCommMask			SetCommMask		
#define	ver_SetupComm			SetupComm		
#define	ver_PurgeComm			PurgeComm		
#define	ver_ClearCommError		ClearCommError		

#define	ver_TransmitCommChar	TransmitCommChar
#define	ver_WaitCommEvent		WaitCommEvent
#define	ver_SetCommBreak		SetCommBreak
#define	ver_ClearCommBreak		ClearCommBreak
#define	ver_GetCommModemStatus	GetCommModemStatus
#define	ver_GetCommProperties	GetCommProperties
#define	ver_EscapeCommFunction	EscapeCommFunction

#endif



#ifdef __cplusplus
}	
#endif


#endif	//_PCFILE_H_


