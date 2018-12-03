/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	__PCFILE_H
#define	__PCFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <emlos.h>

	
//added by xyg
#ifdef XYG_PC_PRJ	//EML_WIN32

#define	ES_AUTONEWLINE			0

#define AP_XSTART     30
#define AP_YSTART     0
#define AP_WIDTH      210
#define AP_HEIGHT     300

//
#define classBUTTON			"BUTTON"
#define classSTATIC			"STATIC"
#define classDIALOG			"DIALOG"
#define classLISTBOX		"LISTBOX"
#define classCOMBOLISTBOX	"COMBOLISTBOX"	
#define classCOMBOBOX		"COMBOBOX"
#define classEDIT			"EDIT"

#else

#define	ES_AUTOHSCROLL			0

#endif


#if defined(VC386) && !defined(PC_FILE_CANCEL)	//EML_WIN32
//在KINGMOS PC版上

//
	/*
extern	HMODULE	Win32_LoadLibrary( LPCTSTR lpFileName );
extern	BOOL	Win32_FreeLibrary( HMODULE hModule );
extern	FARPROC Win32_GetProcAddress( HMODULE hModule, LPCSTR lpProcName );

//
extern	HANDLE	Win32_CreateFile( LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
extern	BOOL	Win32_ReadFile( HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped );
extern	BOOL	Win32_WriteFile( HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped );
extern	DWORD	Win32_SetFilePointer( HANDLE hFile, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
extern	DWORD	Win32_GetFileSize( HANDLE hFile, LPDWORD lpFileSizeHigh );
extern	BOOL	Win32_CloseHandle( HANDLE hObject );

//
extern	BOOL	Win32_GetCommState( HANDLE hFile, LPDCB lpDCB );
extern	BOOL	Win32_SetCommState( HANDLE hFile, LPDCB lpDCB );
extern	BOOL	Win32_GetCommTimeouts( HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts );
extern	BOOL	Win32_SetCommTimeouts( HANDLE hFile, LPCOMMTIMEOUTS lpCommTimeouts );
extern	BOOL	Win32_SetCommMask( HANDLE hFile, DWORD dwEvtMask );
extern	BOOL	Win32_SetupComm( HANDLE hFile, DWORD dwInQueue, DWORD dwOutQueue );
extern	BOOL	Win32_PurgeComm( HANDLE hFile, DWORD dwFlags );
extern	BOOL	Win32_ClearCommError( HANDLE hFile, LPDWORD lpErrors, LPCOMSTAT lpStat );

extern	BOOL	Win32_TransmitCommChar(HANDLE hFile, char cChar );
extern	BOOL	Win32_WaitCommEvent(HANDLE hFile, LPDWORD lpEvtMask, LPOVERLAPPED lpOverlapped );
extern	BOOL	Win32_SetCommBreak(HANDLE hFile );
extern	BOOL	Win32_ClearCommBreak(HANDLE hFile );
extern	BOOL	Win32_GetCommModemStatus(HANDLE hFile, LPDWORD lpModemStat );
extern	BOOL	Win32_GetCommProperties(HANDLE hFile, LPCOMMPROP lpCommProp );
extern	BOOL	Win32_EscapeCommFunction(HANDLE hFile, DWORD dwFunc );
*/

#define	ver_LoadLibrary			Win32_LoadLibrary		
#define	ver_FreeLibrary			Win32_FreeLibrary			
#define	ver_GetProcAddress		Win32_GetProcAddress		

#define	ver_CreateFile			Win32_CreateFile		
#define	ver_ReadFile			Win32_ReadFile			
#define	ver_WriteFile			Win32_WriteFile		
#define	ver_SetFilePointer		Win32_SetFilePointer		
#define	ver_GetFileSize			Win32_GetFileSize		
#define	ver_CloseHandle			Win32_CloseHandle		

#define	ver_CreateDirectory		Win32_CreateDirectory		
#define	ver_SetEndOfFile		Win32_SetEndOfFile		


#define	ver_GetCommState		Win32_GetCommState		
#define	ver_SetCommState		Win32_SetCommState		
#define	ver_GetCommTimeouts		Win32_GetCommTimeouts	
#define	ver_SetCommTimeouts		Win32_SetCommTimeouts	
#define	ver_SetCommMask			Win32_SetCommMask		
#define	ver_SetupComm			Win32_SetupComm		
#define	ver_PurgeComm			Win32_PurgeComm		
#define	ver_ClearCommError		Win32_ClearCommError		

#define	ver_TransmitCommChar	Win32_TransmitCommChar
#define	ver_WaitCommEvent		Win32_WaitCommEvent
#define	ver_SetCommBreak		Win32_SetCommBreak
#define	ver_ClearCommBreak		Win32_ClearCommBreak
#define	ver_GetCommModemStatus	Win32_GetCommModemStatus
#define	ver_GetCommProperties	Win32_GetCommProperties
#define	ver_EscapeCommFunction	Win32_EscapeCommFunction


#else
//在KINGMOS 版上

#define	ver_LoadLibrary			LoadLibrary		
#define	ver_FreeLibrary			FreeLibrary			
#define	ver_GetProcAddress		GetProcAddress		

#define	ver_CreateFile			CreateFile		
#define	ver_ReadFile			ReadFile			
#define ver_GetLastError		GetLastError
#define	ver_WriteFile			WriteFile		
#define	ver_SetFilePointer		SetFilePointer		
#define	ver_GetFileSize			GetFileSize		
#define	ver_CloseHandle			CloseHandle		

#define	ver_CreateDirectory		CreateDirectory		
#define	ver_SetEndOfFile		SetEndOfFile		


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


