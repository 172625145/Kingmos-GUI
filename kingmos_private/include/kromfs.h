/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ں�kromfsͷ�ļ�
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-06-18
���ߣ�    �ܱ�
�޸ļ�¼��
******************************************************/

#ifndef __KROMFS_H_
#define __KROMFS_H_

//#include <ewindows.h>
//#include "resource.h"
//#include "Eassert.h"
//#include "efsdmgr.h"

BOOL   ROM_CloseFile(HANDLE hFile);
HANDLE ROM_CreateFile( LPCTSTR pwsFileName, DWORD dwAccess, DWORD dwShareMode,
					   PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate,DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
BOOL   ROM_DeviceIoControl(HANDLE hFile, DWORD dwIoControlCode, LPVOID pInBuf, DWORD nInBufSize, LPVOID pOutBuf, 
						   DWORD nOutBufSize, LPDWORD pBytesReturned, LPOVERLAPPED lpOverlapped );
BOOL   ROM_FindClose(HANDLE hSearch);
//HANDLE  ROM_FindFirstFileW(PVOL pVolume, HANDLE hProc, LPCTSTR pwsFileSpec, FILE_FIND_DATA *  pfd);
//BOOL   ROM_FindNextFileW(PFIND pSearchBuffer, FILE_FIND_DATA *  pfd);
DWORD  ROM_GetFileAttributes(LPCTSTR pwsFileName);
//DWORD  ROM_GetFileSize(PFILE pFileIn, LPDWORD pFileSizeHigh);
//BOOL  ROM_GetFileTime(PFILE pF, FILETIME *pCreation, FILETIME *pLastAccess, FILETIME *pLastWrite);
//BOOL  ROM_MountDisk(HDSK hDisk);
BOOL   ROM_ReadFile(HANDLE hFile, PVOID pBufferOut, DWORD cbRead, LPDWORD pcbRead, LPOVERLAPPED pOverlapped);
//BOOL    ROM_RegisterFileSystemFunction(PVOL pVolume, SHELLFILECHANGEFUNC pfn);
DWORD  ROM_SetFilePointer(HANDLE hFile, LONG lDistanceToMove, LPLONG pDistanceToMoveHigh, DWORD dwMoveMethod);
//BOOL  ROM_UnmountDisk(HDSK hDisk);


#endif
