/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：romfs头文件
版本号：  1.0.0
开发时期：2003-06-18
作者：    周兵
修改记录：
******************************************************/

#ifndef _ROMFS_H
#define _ROMFS_H

//#include <ewindows.h>
//#include "resource.h"
//#include "Eassert.h"
//#include "efsdmgr.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//				Struct define  here........
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define		_MEMCHECK()				_HeapCheck()
#define		FILE_MODE_FILE			1
#define		FILE_MODE_MOD			2
#define     FILE_EXTEND_NAME_LEN    3
#define		ERROR_NEGATIVE_SEEK		1001
#define		ERROR_WRITE_PROTECT		1002
#define		FIND_CREATE_CALL		1
#define		FIND_NORMAL_CALL		2




#define	FILE_NO_SHARE	0

typedef struct __ROMVOLUME{

	struct __ROMVOLUME			* next;
	HDSK						hDevice;		//handle of the disk which is related to the FSD 												
	HVOL						hVolume;		//handle of the volume, return by FSDMGR_RegisterVolume.
	//HANDLE                      hDisk;			// handle of the disk, which is gotten by CreateFile.
//	VOLUMEDESCRIPTOR			vol_descriptor;
	SHELLFILECHANGEFUNC			pNotifyFunction;//this function is registered by system,and
												// when there is some change in this folder, FSD should call this
												//  function to tell system.
//	CDM_DISK_INFO				cdm_info;	
	CRITICAL_SECTION            cs_Volume;
} ROMVOLUME, * PROMVOLUME;

typedef    struct  __ROMFILE{
     // Note: 
	 //   the fllowing  8 element  is identical with WIN32_FIND_DATA  struct, 
	DWORD		dwFileAttributes; 
	FILETIME	ftCreationTime; 
	FILETIME	ftLastAccessTime; 
	FILETIME	ftLastWriteTime; 
	DWORD		nFileSizeHigh; 
	DWORD		nFileSizeLow; 
	DWORD		dwOID; 
	TCHAR		cFileName[MAX_PATH]; 
    // above are from  WIN32_FIND_DATA  struct, they must not be changed...

	PROMVOLUME     pVolume;			// pinting to  the current Volume,
	HVOL           hVol;			// the handle of current volume.
//	DWORD          dwStartSector;	// which sector the file starts from.
	DWORD		   dwFileBase;		// file start addr in rom.
	DWORD          dwFilePointer;	// points to the current position of the read pointer.
									// which is from 0 to the size of the file,and can chang by SetFilePointer().
	struct  __ROMFILE  *   next; // points to next CDMFILE struct.
//	HANDLE			hFile;       // handle of the current file ,return by FSDMGR_RegisterFilehandle.
//	SECTORBUFFER    sbCurrent;   // see above.
	CRITICAL_SECTION            cs_File;
	DWORD          dwShareMode;  // the share mode of the file.

}ROMFILE, *PROMFILE;

typedef     struct __SECTORBUFFER{
	
	DWORD      dwSector;  //indicates which sector the pBuffer is pointed to.
	PUCHAR     pBuffer; // buffer, hold the data of a sector of the current file, when the
	             // caller only request a little data & in this buffer,just copies data from
				// this buffer.
}
SECTORBUFFER, PSECTORBUFFER;

typedef  struct __ROMSEARCH{

	struct __ROMSEARCH * next;
	PROMVOLUME  pVol;
//	HANDLE		hSearch;		// the handle of search returned by FSDMGR_RegisterSearchHandle.
	DWORD		pFileBase;		// pointer of file start addr.
	//PUCHAR      pFindBuffer;	//buffer which takes the data in sector dwCurrentSector or TOC( not data CD) 
	//DWORD       dwCurrentSector;// 指向当前正在读数据的SECTOR,它的取值范围为: 该目录的开始扇区,
								//到开始扇区+目录大小至.
	CRITICAL_SECTION       cs_Search;

	//WORD		wCurrentRecorder;  //the recorder index of this folder. which is from 1 to 
								// the number of files in this folder.
	//WORD		wOffset;		// Which bytes the current recorder is begining from ,it ranges from
								// 1 to the size of the folder.
    //WORD		wParentSize;	// the size of the current dirctory(file) in bytes.
	//WORD		wFileSpecial[MAX_PATH]; // the full path of the current search.
	TCHAR		wFileSpecial[MAX_PATH+1];

} ROMSEARCH, *PROMSEARCH;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//				Function declear here........
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*BOOL   ROM_CloseFile(PFILE pFileClose);
HANDLE ROM_CreateFileW( PVOL pVol, HANDLE hProc, LPCTSTR pwsFileName, DWORD dwAccess, DWORD dwShareMode,
					   PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate,DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
BOOL   ROM_DeviceIoControl(PFILE pFile, DWORD dwIoControlCode, LPVOID pInBuf, DWORD nInBufSize, LPVOID pOutBuf, 
						   DWORD nOutBufSize, LPDWORD pBytesReturned);
BOOL   ROM_FindClose(PFIND pSearchIn);
HANDLE  ROM_FindFirstFileW(PVOL pVolume, HANDLE hProc, LPCTSTR pwsFileSpec, FILE_FIND_DATA *  pfd);
BOOL   ROM_FindNextFileW(PFIND pSearchBuffer, FILE_FIND_DATA *  pfd);
DWORD  ROM_GetFileAttributesW(PVOL pVolume, LPCTSTR pwsFileName);
DWORD  ROM_GetFileSize(PFILE pFileIn, LPDWORD pFileSizeHigh);
BOOL  ROM_GetFileTime(PFILE pF, FILETIME *pCreation, FILETIME *pLastAccess, FILETIME *pLastWrite);
BOOL  ROM_MountDisk(HDSK hDisk);
BOOL   ROM_ReadFile(PFILE pF, PVOID pBufferOut, DWORD cbRead, LPDWORD pcbRead, LPOVERLAPPED pOverlapped);
BOOL    ROM_RegisterFileSystemFunction(PVOL pVolume, SHELLFILECHANGEFUNC pfn);
DWORD  ROM_SetFilePointer(PFILE pFileIn, LONG lDistanceToMove, LPLONG pDistanceToMoveHigh, DWORD dwMoveMethod);
BOOL  ROM_UnmountDisk(HDSK hDisk);
*/

BOOL HELP_RegisterVolume( PROMVOLUME pVolume ) ;
PROMVOLUME CreateVolume( HDSK hDisk );
BOOL ROM_IsVolume( PROMVOLUME pVolume );
BOOL  IsFileOpened( PROMVOLUME pVolume, LPCTSTR pwsName );
void  FormatFilePath(TCHAR * pcwFile );
//TCHAR *  GetFileName (TCHAR *  pcwFolder);
BOOL   IsDesiredFile( LPCTSTR  pcwPath, LPCTSTR  pcwFileName);
BOOL ROM_FindFile(LPCTSTR pFileName, UINT *index, UINT *FileMode);
HANDLE  HELP_FindFirstFile(PVOL pVolume, LPCTSTR pwsFileSpec, FILE_FIND_DATA * pfd , UINT flag);
BOOL  HELP_IsFile( PROMFILE pFile  );





//
//DEBUG	MESSAGE OPTIONAL
//
#if	0

#define  ZONE_OUT    1
#define  ZONE_MOUNT  1
#define  ZONE_ERROR  1
#define  ZONE_SEARCH 1
#define  ZONE_FILE   1
#define  ZONE_TIME   1
#define  ZONE_READ   1
#define  ZONE_DIRECT   1
#define  ZONE_ENTRY    1
#define  ZONE_DISK     1

#define	 ZONE_TEST		0
#define	 ZONE_DEBUG		1
#define	 ROM_HELP		1

#else

#define  ZONE_OUT    0
#define  ZONE_MOUNT  0
#define  ZONE_ERROR  1
#define  ZONE_SEARCH 0
#define  ZONE_FILE   0
#define  ZONE_TIME   0
#define  ZONE_READ   0
#define  ZONE_DIRECT   0
#define  ZONE_ENTRY    0
#define  ZONE_DISK     0

#define	 ZONE_TEST		0
#define	 ZONE_DEBUG		0
#define	 ROM_HELP		0

#endif

extern PROMVOLUME	pVolumeList;
extern PROMSEARCH	pSearchList;
extern HANDLE		hDll;
extern PROMFILE		pFileList;

extern CRITICAL_SECTION    Crit_Volume, Crit_File, Crit_Search;

extern FILECHANGEINFO		FileChanger;
extern TCHAR               FolderName[];
extern TCHAR				FsdName[];



//#define		AUDIOFILE   TEXT("Track")


#endif
