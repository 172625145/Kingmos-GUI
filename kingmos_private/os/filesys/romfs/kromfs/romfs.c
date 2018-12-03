/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：内核romfs主文件
版本号：  1.0.0
开发时期：2003-06-18
作者：    周兵
修改记录：
******************************************************/
 
#include <ewindows.h>
#include <efsdmgr.h>
#include <epalloc.h>
#include <edevice.h>
#include <Eassert.h>
#include <romfs.h>
#include <kromfs.h>

#include <romheader.h>
#include <coresrv.h>
#include <devdrv.h>

#define ABS( v ) ( (v) < 0 ? -(v) : (v) )
PROMVOLUME			pVolumeList=NULL;
PROMSEARCH			pSearchList=NULL;
HANDLE				hDll=NULL;
PROMFILE			pFileList=NULL;

CRITICAL_SECTION    Crit_Volume, Crit_File, Crit_Search;

FILECHANGEINFO		FileChanger;
//TCHAR               FolderName[]="kingmos";
//TCHAR				FsdName[] = "ROMFS";

extern  ROMHDR *const pTOC ;

//#define		DEF_PVOL	0x11
#define		DEF_PROC	((HANDLE)0x22)
#define		DEF_HDISK	((HDSK)0x33)

PROMVOLUME	PDEF_ROMVOLUME;

static BOOL   DoROM_CloseFile(PFILE pFileClose);
static DWORD  DoROM_SetFilePointer(PFILE pFileIn, LONG lDistanceToMove, LPLONG pDistanceToMoveHigh, DWORD dwMoveMethod);
static BOOL   DoROM_ReadFile(PFILE pF, PVOID pBufferOut, DWORD cbRead, LPDWORD pcbRead, LPOVERLAPPED pOverlapped);
static DWORD  DoROM_GetFileAttributes(PVOL pVolume, LPCTSTR pwsFileName);
static BOOL  DoROM_MountDisk(HDSK hDisk);
static BOOL  DoROM_UnmountDisk(HDSK hDisk);
static HANDLE DoROM_CreateFile( 
		PVOL pVol, 
		HANDLE hProc, 
		LPCTSTR pwsFileName, 
		DWORD dwAccess, 
		DWORD dwShareMode,
		PSECURITY_ATTRIBUTES pSecurityAttributes, 
		DWORD dwCreate,
		DWORD dwFlagsAndAttributes, 
		HANDLE hTemplateFile );
static BOOL   DoROM_DeviceIoControl(
	PFILE pFile, 
	DWORD dwIoControlCode, 
	LPVOID pInBuf, 
	DWORD nInBufSize, 
	LPVOID pOutBuf, 
	DWORD nOutBufSize, 
	LPDWORD pBytesReturned);

/*struct _FSDDRV RomFSDDrv = {
	&DoROM_CloseFile,
//	&DoROM_CloseVolume,
	NULL,	
//	&DoROM_CreateDirectoryW,
	NULL,
	&DoROM_CreateFile,
//	&DoROM_DeleteAndRenameFileW,
	NULL,
//	&DoROM_DeleteFileW,
	NULL,
	&DoROM_DeviceIoControl,	//have not implement.	
//	&DoROM_FindClose,
//	&DoROM_FindFirstFileW,
//	&DoROM_FindNextFileW,
	NULL,
	NULL,
	NULL,
//	&DoROM_FlushFileBuffers,
	NULL,
//	&DoROM_GetDiskFreeSpaceW,
	NULL,
	&DoROM_GetFileAttributes,
//	&DoROM_GetFileInformationByHandle,
	NULL,
//	&DoROM_GetFileSize,
	NULL,
//	&DoROM_GetFileTime,
	NULL,
	&DoROM_MountDisk,
//	&DoROM_MoveFileW,
	NULL,
//	&DoROM_Notify,				//have not implement.
	NULL,
	&DoROM_ReadFile,
//	&DoROM_ReadFileWithSeek,	//don't have.
	NULL,
//	&DoROM_RegisterFileSystemFunction,
	NULL,
//	&DoROM_RemoveDirectoryW,
	NULL,
//	&DoROM_SetEndOfFile,
	NULL,
//	&DoROM_SetFileAttributesW,
	NULL,
	&DoROM_SetFilePointer,
//	&DoROM_SetFileTime,
	NULL,
	&DoROM_UnmountDisk,
//	&DoROM_WriteFile,
	NULL,
//	&DoROM_WriteFileWithSeek,
	NULL,
	//&DoROM_CopyFile			//don't have.
	NULL
};*/

/*BOOL RegisterRomFSD(void )
{
	if(!FSDMGR_RegisterFSD(FsdName, &RomFSDDrv)){
		RETAILMSG(ZONE_ERROR, (TEXT("Rom FSD: register Rom FSD fail!\r\n")));
		return FALSE;
	}else{
		RETAILMSG(ZONE_ERROR, (TEXT("Rom FSD: register Rom FSD success!\r\n")));
		return TRUE;
	}		
}*/


//
//功能：内核启动时调用，初始化内核的romfs.
//
//
void InitRomFsd(void)
{

	InitializeCriticalSection( &Crit_Volume );
	InitializeCriticalSection( &Crit_File   );
	InitializeCriticalSection( &Crit_Search );

	DoROM_MountDisk( DEF_HDISK );
	//if(!RegisterRomFSD())
	//	DeinitRomFsd();

	//return TRUE;
}

void DeinitRomFsd(void)
{
	DeleteCriticalSection( &Crit_Volume);
	DeleteCriticalSection( &Crit_File  );
	DeleteCriticalSection( &Crit_Search );

	DoROM_UnmountDisk(DEF_HDISK);
}


BOOL  DoROM_MountDisk(HDSK hDisk)
{
	PROMVOLUME		pVolume;
	HVOL			VolumeIdentifer=0;
//	CDCALLBACK      dwCallBackFunc;
	WORD			wFolderLen=0; 
	

	DEBUGMSG(ZONE_MOUNT, (TEXT("romfs: MountDisk entered %x\r\n"),hDisk));

	pVolume = CreateVolume(hDisk  );
	if( pVolume ==NULL ){
		ERRORMSG(ZONE_MOUNT, (TEXT("romfs: MountDisk fail! \r\n")));
		return FALSE;
	}
	PDEF_ROMVOLUME = pVolume;

    return HELP_RegisterVolume(  pVolume );
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL  DoROM_UnmountDisk(HDSK hDisk)
{
	PROMVOLUME pVol , pVolDel;
//	HVOL              hVol;
	BOOL       status=FALSE;

	DEBUGMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk entered %x\r\n"),hDisk));
    
	EnterCriticalSection( &Crit_Volume);
	DEBUGMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk after critical section\r\n")));

	pVol=pVolumeList;
	if( pVolumeList !=NULL ){

		if( pVolumeList->hDevice ==hDisk ){

			WARNMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk   device should unMount\r\n")));
			pVolDel=pVolumeList;
			pVolumeList=pVolumeList->next;
			status=TRUE;
		}else{
		
			WARNMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk   there are more than 2 device\r\n")));
			while( pVol->next){

				if( pVol->next->hDevice ==hDisk)
				{
					WARNMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk device%x (%x) has destroyed\r\n"),hDisk,pVol->hVolume));
					pVolDel=pVol->next;
					pVol->next=pVol->next->next;
					status=TRUE;
					break;
				}
				pVol= pVol->next;
			}
		}
	}
	LeaveCriticalSection( &Crit_Volume);

	DEBUGMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk  after Crit_Volume\r\n"),hDisk));

	if( status ) {
		CRITICAL_SECTION  crit_this=pVolDel->cs_Volume;
		//hVol=pVolDel->hVolume;

		EnterCriticalSection(&crit_this );
		KHeap_Free( pVolDel, sizeof(ROMVOLUME) );
		LeaveCriticalSection(&crit_this );

		DeleteCriticalSection(&crit_this);
		//FSDMGR_DeregisterVolume( hVol );
		 return TRUE;

	}else{
		SetLastError( ERROR_INVALID_HANDLE);
		WARNMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk  failed\r\n"),hDisk));
	}
	DEBUGMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk  leaved\r\n")));
	return status;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*BOOL    DoROM_RegisterFileSystemFunction(PVOL pVolume, SHELLFILECHANGEFUNC pfn)
{
	PROMVOLUME  pVol=(PROMVOLUME)pVolume;
	
	RETAILMSG(ZONE_MOUNT, (TEXT("romfs:  DoROM_RegisterFileSystemFunction entered %x\r\n"),pfn));

	EnterCriticalSection( &Crit_Volume);
	pVol->pNotifyFunction = pfn;
	LeaveCriticalSection( &Crit_Volume);

	(*pfn)(&FileChanger);

	return TRUE;
}
*/
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*void    DoROM_Notify(PVOL pVolume, DWORD dwFlags)
{
	RETAILMSG(ZONE_OUT, (TEXT("romfs:  The function isn't implement\r\n")));
}*/

BOOL DEV_AllocFile(PROMFILE pFile, LPDWORD pFileBase, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT				index, FileMode;
	LPFILESentry		pFileentry;

	if(!ROM_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_FILE)
		return FALSE;
	
	pFileentry = (LPFILESentry)pTOC->ulModOffset;
	pFileentry += index;

	*pFileBase = pFileentry->ulLoadOffset;
	*pBytesReturned = sizeof(UINT);

	return TRUE;		
}

BOOL DEV_GetExeInfo(PROMFILE pFile, EXE_ROM *pExeInfo, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT	index, FileMode;
	LPTOCentry		pTocentry;

	if(!ROM_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_MOD)
		return FALSE;
	
	pTocentry = (LPTOCentry)pTOC->ulModOffset;
	pTocentry += index;

	pExeInfo = (EXE_ROM *)pTocentry->ulExeOffset;
	*pBytesReturned = sizeof(UINT);

	return TRUE;		
}

BOOL DEV_GetObjInfo(PROMFILE pFile, OBJ_ROM *pObjInfo, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT	index, FileMode;
	LPTOCentry		pTocentry;

	if(!ROM_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_MOD)
		return FALSE;
	
	pTocentry = (LPTOCentry)pTOC->ulModOffset;
	pTocentry += index;

	pObjInfo = (OBJ_ROM *)pTocentry->ulObjOffset;
	*pBytesReturned = sizeof(UINT);

	return TRUE;		
}

BOOL DEV_GetElfInfo(PROMFILE pFile, PELF_INFO pElfInfo, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT	index, FileMode;
	LPTOCentry		pTocentry;

	if(!ROM_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_MOD)
		return FALSE;
	
	pTocentry = (LPTOCentry)pTOC->ulModOffset;
	pTocentry += index;

	DEBUGMSG(ZONE_DEBUG, (TEXT("elf info:.........\r\n")));
	DEBUGMSG(ZONE_DEBUG, (TEXT("ulEntry: %x,ulFileBase:%x\r\n"), pTocentry->ElfInfo.ulEntry, pTocentry->ElfInfo.ulFileBase));

	memcpy(pElfInfo, &pTocentry->ElfInfo, sizeof(ELF_INFO));
	*pBytesReturned = sizeof(ELF_INFO);

	return TRUE;		
}

BOOL DEV_GetSegInfo(PROMFILE pFile, PMODULE_SEG pElfInfo, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT	index, FileMode;
	LPTOCentry		pTocentry;

	if(!ROM_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_MOD)
		return FALSE;
	
	pTocentry = (LPTOCentry)pTOC->ulModOffset;
	pTocentry += index;

	DEBUGMSG(ZONE_DEBUG, (TEXT("elf info:.........\r\n")));
	DEBUGMSG(ZONE_DEBUG, (TEXT("ulEntry: %x,ulFileBase:%x\r\n"), pTocentry->ElfInfo.ulEntry, pTocentry->ElfInfo.ulFileBase));

	if(pTocentry->bValidRes == TRUE){
		memcpy(pElfInfo, &pTocentry->ResSeg, sizeof(MODULE_SEG));
		*pBytesReturned = sizeof(MODULE_SEG);
		return TRUE;		
	}
	//if the module no res, set flag: 0;
	*pBytesReturned = 0;
	return TRUE;

	//return FALSE;	
}

BOOL ROM_DeviceIoControl(
  HANDLE hDevice,              // handle to device
  DWORD dwIoControlCode,       // operation
  LPVOID lpInBuffer,           // input data buffer
  DWORD nInBufferSize,         // size of input data buffer
  LPVOID lpOutBuffer,          // output data buffer
  DWORD nOutBufferSize,        // size of output data buffer
  LPDWORD lpBytesReturned,     // byte count
  LPOVERLAPPED lpOverlapped    // overlapped information
)
{
	return DoROM_DeviceIoControl((PFILE)hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer,
							nOutBufferSize, lpBytesReturned);
}

static BOOL   DoROM_DeviceIoControl(
	PFILE pFile, 
	DWORD dwIoControlCode, 
	LPVOID pInBuf, 
	DWORD nInBufSize, 
	LPVOID pOutBuf, 
	DWORD nOutBufSize, 
	LPDWORD pBytesReturned)
{
	PROMFILE   pF	=	(PROMFILE)pFile;
	BOOL		bStatus =	FALSE;

	DEBUGMSG(ZONE_OUT, (TEXT("DoROM_DeviceIoControl: < (Control code %x)\r\n"),dwIoControlCode));

	if(pF == NULL)
		return FALSE;

	//Check parms ....
	switch(dwIoControlCode)
	{
	case IOCTL_ROM_ALLOC_FILE:
		break;
	case IOCTL_ROM_GET_EXE_INFO:
		break;
	case IOCTL_ROM_GET_OBJ_INFO:
		break;
	case IOCTL_ROM_GET_ELF_INFO:
		break;
	case IOCTL_ROM_GET_SEG_RES:
		break;
	default:
		WARNMSG(ZONE_DEBUG, (TEXT("ROMFS: unknown iocontrolcode...\r\n")));
	}

	//do action ....
	switch(dwIoControlCode)
	{
	case IOCTL_ROM_GET_ELF_INFO:
		DEBUGMSG(ZONE_DEBUG, (TEXT("ROMFS: addr:%x\r\n"), pOutBuf));

		bStatus = DEV_GetElfInfo(pF, pOutBuf, nOutBufSize, pBytesReturned);
		break;
	case IOCTL_ROM_ALLOC_FILE:
		bStatus = DEV_AllocFile(pF, pOutBuf, nOutBufSize, pBytesReturned);
		break;
	case IOCTL_ROM_GET_EXE_INFO:
		bStatus = DEV_GetExeInfo(pF, pOutBuf, nOutBufSize, pBytesReturned);
		break;
	case IOCTL_ROM_GET_OBJ_INFO:
		bStatus = DEV_GetObjInfo(pF, pOutBuf, nOutBufSize, pBytesReturned);
		break;
	case IOCTL_ROM_GET_SEG_RES:
		bStatus = DEV_GetSegInfo(pF, pOutBuf, nOutBufSize, pBytesReturned);
		break;

	default:
		WARNMSG(ZONE_DEBUG, (TEXT("ROMFS: unknown iocontrolcode...\r\n")));
	}

	DEBUGMSG(ZONE_OUT, (TEXT("DoROM_DeviceIoControl: > %x\r\n"),bStatus));
	return bStatus;
}


HANDLE ROM_CreateFile(
  LPCTSTR lpFileName,                         // file name
  DWORD dwDesiredAccess,                      // access mode
  DWORD dwShareMode,                          // share mode
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // SD
  DWORD dwCreationDisposition,                // how to create
  DWORD dwFlagsAndAttributes,                 // file attributes
  HANDLE hTemplateFile                        // handle to template file
  )
{
	LPVOID lpv = DoROM_CreateFile( (PVOL)PDEF_ROMVOLUME, DEF_PROC, lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
							dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);	
	// 2003-06-27, ADD-begin by ln
	if( (HANDLE)lpv != INVALID_HANDLE_VALUE )
        return (HANDLE)Handle_Alloc( 0, lpv, OBJ_FILE );
	else
		return INVALID_HANDLE_VALUE;
	// 2003-06-27, ADD-end by ln
}

static HANDLE DoROM_CreateFile( 
		PVOL pVol, 
		HANDLE hProc, 
		LPCTSTR pwsFileName, 
		DWORD dwAccess, 
		DWORD dwShareMode,
		PSECURITY_ATTRIBUTES pSecurityAttributes, 
		DWORD dwCreate,
		DWORD dwFlagsAndAttributes, 
		HANDLE hTemplateFile )
{
	PROMSEARCH  pSearch;
	PROMFILE    pFile;
	HANDLE      ret=INVALID_HANDLE_VALUE;
	PROMVOLUME     pVolume=(PROMVOLUME)pVol;

	DEBUGMSG(ZONE_FILE, (TEXT("romfs: CreatFile entered  %s\r\n"),pwsFileName));
//  now, check the actual parameters.
	if( !(dwCreate == OPEN_EXISTING || dwCreate == OPEN_ALWAYS) || 
		 pwsFileName == NULL || 
		 dwAccess != GENERIC_READ ) 
	{		
		WARNMSG(ZONE_ERROR, (TEXT("romfs0: CreatFile  take error parameter\r\n")));
		SetLastError( ERROR_INVALID_PARAMETER);
		return INVALID_HANDLE_VALUE;
	}
	if( ! ROM_IsVolume(pVolume ) ){

		WARNMSG(ZONE_ERROR, (TEXT("romfs: CreatFile volume  %x is invalid\r\n"),pVol));
		return  INVALID_HANDLE_VALUE;
	}
	if( ! IsFileOpened( pVolume,pwsFileName ) ){
		SetLastError( ERROR_SHARING_VIOLATION );  // share violation.
		return INVALID_HANDLE_VALUE;
	}
    EnterCriticalSection( &pVolume->cs_Volume );

	//pFile=LocalAlloc(LPTR, sizeof( ROMFILE));
	pFile = KHeap_Alloc( sizeof( ROMFILE) );
	if( pFile != NULL ){

		//RETAILMSG(ZONE_ERROR,(TEXT("rom:3\r\n")));

		pSearch = HELP_FindFirstFile(pVol, pwsFileName, (FILE_FIND_DATA *)pFile, FIND_CREATE_CALL);
		if(pSearch == INVALID_HANDLE_VALUE)
			goto  Error_Return;
		
		//free(pSearch);
		KHeap_Free( pSearch, sizeof( ROMSEARCH ));

		strcpy(pFile->cFileName, pwsFileName);
		pFile->pVolume		= pVolume;
		pFile->dwFilePointer= 0;
		pFile->dwShareMode	=dwShareMode;
		InitializeCriticalSection(& (pFile->cs_File) );
		
		LeaveCriticalSection( &pVolume->cs_Volume );
		
		//RETAILMSG(ZONE_FILE, (TEXT("romfs: CreatFile before CriticalSection\r\n")));
		
		EnterCriticalSection( &Crit_File ); // insert this node into list.
		pFile->next	=	pFileList;
		pFileList	=	pFile;
		LeaveCriticalSection( &Crit_File);
		
		DEBUGMSG(ZONE_FILE, (TEXT("romfs: CreatFile leaved  %x\r\n"),pFile));
		return (HANDLE)pFile;			   
	}
	   
Error_Return:
	if( pFile )
	    KHeap_Free( pFile, sizeof( ROMFILE) );
	//free(pFile);
	

	WARNMSG(ZONE_FILE, (TEXT("romfs: CreatFile  Failed\r\n")));

	LeaveCriticalSection( &pVolume->cs_Volume );
 	return INVALID_HANDLE_VALUE;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD ROM_GetFileAttributes(
  LPCTSTR lpFileName   // name of file or directory
)
{
	return DoROM_GetFileAttributes( (PVOL)PDEF_ROMVOLUME, lpFileName);
}

static DWORD  DoROM_GetFileAttributes(PVOL pVolume, LPCTSTR pwsFileName)
{
    FILE_FIND_DATA		File;
	PROMSEARCH			pSearch;
	DWORD				dwAttr=0xFFFFFFFF;
	PROMVOLUME			pVol=(PROMVOLUME)pVolume;

	DEBUGMSG(ZONE_FILE, (TEXT("romfs:  DoROM_GetFileAttributes entered %s \r\n"),pwsFileName));
	//RETAILMSG(ZONE_ERROR, (TEXT("romfs: %s\r\n"),pwsFileName));

	//if((strcmp(pwsFileName, "\\hellores.exe") != 0) && 
	//	(strcmp(pwsFileName, "\\libtestdll1.so") != 0))
	//	ASSERT(0);

	if( ! ROM_IsVolume(pVol ) ){

		WARNMSG(ZONE_ERROR, (TEXT("romfs: GetFileAttribute volume  %x is invalid\r\n"),pVol));
		return  0xFFFFFFFF;
	}
	//RETAILMSG(ZONE_FILE, (TEXT("romfs:  DoROM_GetFileAttributesW  %s  before Crit\r\n"),pwsFileName));

    EnterCriticalSection( &pVol->cs_Volume );
	
	DEBUGMSG(ZONE_FILE,(TEXT("rom:4\r\n")));

	pSearch=( PROMSEARCH )HELP_FindFirstFile(pVolume, pwsFileName, &File, FIND_NORMAL_CALL);
	
	if((HANDLE) pSearch != INVALID_HANDLE_VALUE ) {
		
		dwAttr=File.dwFileAttributes;
		
		KHeap_Free(pSearch, sizeof( ROMSEARCH ));

		DEBUGMSG(1, (TEXT("romfs00:  %x\r\n"),dwAttr ));
		
		//RETAILMSG(ZONE_FILE, (TEXT("romfs:  DoROM_GetFileAttributesW leaved %x\r\n"),dwAttr));
	}
	DEBUGMSG(1, (TEXT("romfs11:  %x\r\n"),dwAttr ));
	
	LeaveCriticalSection ( &pVol->cs_Volume );

	DEBUGMSG(ZONE_FILE, (TEXT("romfs:  DoROM_GetFileAttributesW leaved %x\r\n"),dwAttr ));
	return dwAttr;
}

BOOL ROM_ReadFile(
  HANDLE hFile,                // handle to file
  LPVOID lpBuffer,             // data buffer
  DWORD nNumberOfBytesToRead,  // number of bytes to read
  LPDWORD lpNumberOfBytesRead, // number of bytes read
  LPOVERLAPPED lpOverlapped    // overlapped buffer
)
{
	return DoROM_ReadFile((PFILE)hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static BOOL   DoROM_ReadFile(PFILE pF, PVOID pBufferOut, DWORD cbRead, LPDWORD pcbRead, LPOVERLAPPED pOverlapped)
{
	PROMFILE   pFile	=	(PROMFILE)pF;
	PUCHAR     pOut		=	(PUCHAR)pBufferOut;
//	WORD       wSectorSize;
//	DWORD      dwReadSectorNum;    // this time, the how many sectors should be read out.
//	DWORD      dwReadStartSector;  // which sector should  start read from this time ?
//	WORD       wBytesBeforeSector; // before read the dwReadStartSector, how many bytes should be read,
//	WORD       wBytesAfterSector;//after read the dwReadStartSector, how many bytes should be read,
	BOOL       status	=	TRUE;

	DEBUGMSG(ZONE_READ, (TEXT("romfs:  ReadFile: < %x  %x\r\n"),pFile,cbRead ));


	if( ! HELP_IsFile( pFile) ){
		WARNMSG(ZONE_READ, (TEXT("romfs:  ReadFile :> %x  %x\r\n"),pFile,cbRead ));
		return   FALSE;
	}
	if( pBufferOut ==NULL || pcbRead==NULL )
	{
		SetLastError( ERROR_INVALID_PARAMETER);
		WARNMSG(ZONE_READ, (TEXT("romfs:  ReadFile :> %x  %x\r\n"),pFile,cbRead ));

		return   FALSE;
	}
	EnterCriticalSection( & (pFile->cs_File) );

	if( pFile->dwFilePointer >= pFile->nFileSizeLow ) {

		DEBUGMSG(ZONE_READ, (TEXT("romfs:  ReadFile reach EOF \r\n") ));
		if( pcbRead )
			*pcbRead = 0;
		goto  Error_Return;
	}


	cbRead = min( cbRead,  ( pFile->nFileSizeLow - pFile->dwFilePointer ));

	DEBUGMSG(ZONE_READ, (TEXT("romfs:  ReadFile  cbRead=%x \r\n"),cbRead ));
	
	memcpy(  pOut, (const void*)(pFile->dwFileBase + pFile->dwFilePointer), cbRead);

	if( pcbRead != NULL ) {
		*pcbRead = cbRead;
	}
    pFile->dwFilePointer += cbRead;

	if( pFile->dwFilePointer > pFile->nFileSizeLow)
		pFile->dwFilePointer = pFile->nFileSizeLow;

Error_Return:
	LeaveCriticalSection( &pFile->cs_File);

	DEBUGMSG(ZONE_READ, (TEXT("romfs:  ReadFile leaved %x ,%x\r\n"),*pcbRead ,status ));

	return status;
	//return TRUE;
}

DWORD ROM_SetFilePointer(
  HANDLE hFile,                // handle to file
  LONG lDistanceToMove,        // bytes to move pointer
  PLONG lpDistanceToMoveHigh,  // bytes to move pointer
  DWORD dwMoveMethod           // starting point
)
{
	return DoROM_SetFilePointer((PFILE)hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
}

static DWORD  DoROM_SetFilePointer(PFILE pFileIn, LONG lDistanceToMove, LPLONG pDistanceToMoveHigh, DWORD dwMoveMethod)
{
	DWORD			dwPointer;
	PROMFILE		pFile=(PROMFILE)pFileIn;
	BOOL            status=TRUE;

	DEBUGMSG(ZONE_FILE, (TEXT("romfs:SetFilePointer %d \r\n"),lDistanceToMove));

	if( ! HELP_IsFile(pFile) ){
		return  0xFFFFFFFF;
	}

	EnterCriticalSection( &(pFile->cs_File) );
	switch( dwMoveMethod ) {
	
		case FILE_CURRENT:

			DEBUGMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer in CURRENT \r\n")));
			if( lDistanceToMove < 0 ){
				if( (DWORD)ABS(lDistanceToMove) > pFile->dwFilePointer){
					SetLastError( ERROR_NEGATIVE_SEEK );
					status=FALSE;
					break;
				}				
			}
			dwPointer=pFile->dwFilePointer + lDistanceToMove;
			
			break;
		case FILE_BEGIN:
			DEBUGMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer in BEGIN \r\n")));
			if( lDistanceToMove < 0 ){
				status=FALSE;
				SetLastError( ERROR_NEGATIVE_SEEK );
			}else{
				dwPointer=lDistanceToMove;
			}
			break;
		case FILE_END:
			DEBUGMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer in END \r\n")));

			if( lDistanceToMove > 0 ){
				dwPointer=pFile->nFileSizeLow;
			}else{
				if( (DWORD)ABS(lDistanceToMove) > pFile->nFileSizeLow){
					SetLastError( ERROR_NEGATIVE_SEEK );
					status=FALSE;
				}else{
					dwPointer=pFile->nFileSizeLow + lDistanceToMove;
				}
			}
			break;
		default:
			status=FALSE;
			break;
	}
	DEBUGMSG(ZONE_FILE, (TEXT("romfs: FileSize-%x, dwPointer-%x \r\n"), pFile->nFileSizeLow, dwPointer));

	if( status) {

		if( dwPointer > pFile->nFileSizeLow )
			dwPointer = pFile->nFileSizeLow;

		pFile->dwFilePointer = dwPointer;
		SetLastError( 0 );
	}else{
		dwPointer=0xFFFFFFFF;
	}

	if( pDistanceToMoveHigh != NULL ) {// because the ISO9660 documents only defines the file size as 
						              // DWORD, this DWORD value is ignored.
//		RETAILMSG(ZONE_FILE, (TEXT("romfs:SetFilePointer  High move is %d  \r\n"),*pDistanceToMoveHigh));
		DEBUGMSG(ZONE_FILE, (TEXT("romfs:SetFilePointer not support pDistanceToMoveHigh parm!\r\n")));
		
/*		if(*pDistanceToMoveHigh !=0 ) {
		
			dwPointer =pFile->nFileSizeLow;
			pFile->dwFilePointer= dwPointer;
			*pDistanceToMoveHigh=0;
		}
*/
	}

	LeaveCriticalSection( &(pFile->cs_File));
	DEBUGMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer leaved %d\r\n"),pFile->dwFilePointer));

	return dwPointer;
}

/*DWORD  DoROM_GetFileSize(PFILE pFileIn, LPDWORD pFileSizeHigh)
{
	PROMFILE		pFile=(PROMFILE)pFileIn;
	DWORD           dwSize;

	RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileSize entered\r\n")));

	if( ! HELP_IsFile (pFile ) ) {
		return 0xFFFFFFFF;
	}
	if( pFileSizeHigh != NULL ) {  //the files in CDROM can take only less than DWORD size.

		RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileSize  High size is no NULL\r\n")));
		*pFileSizeHigh = 0;
	}

	EnterCriticalSection( &pFile->cs_File );
	dwSize = pFile->nFileSizeLow;
	LeaveCriticalSection( &pFile->cs_File );

	RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileSize leaved %x\r\n"),dwSize));

	return dwSize;	
}*/

//The function don't support now.//
/*BOOL   DoROM_GetFileInformationByHandle(PFILE pFileIn, FILE_INFORMATION *pFileInfo)
{
	PROMFILE		pFile=(PROMFILE)pFileIn;

	RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileInformationByHandle entered\r\n")));
	if( ! HELP_IsFile (pFile ) ) {
		return FALSE;
	}
	if( pFileInfo ==NULL ){
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}
	EnterCriticalSection( &pFile->cs_File );

	memcpy( (PUCHAR)pFileInfo, (PUCHAR)pFile, sizeof( FILE_FIND_DATA ) );
	pFileInfo->nNumberOfLinks=1;
	pFileInfo->nFileSizeHigh=0;
	pFileInfo->nFileSizeLow =pFile->nFileSizeLow;

	LeaveCriticalSection( &pFile->cs_File );
	RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileInformationByHandle leaved\r\n")));
    return TRUE;
}*/

/*BOOL  DoROM_GetFileTime(PFILE pF, FILETIME *pCreation, FILETIME *pLastAccess, FILETIME *pLastWrite)
{
	PROMFILE pFile=(PROMFILE) pF;
	RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileTime entered %x\r\n"),pF));

	if( ! HELP_IsFile( pFile) ){
		return   FALSE;
	}

	RETAILMSG(ZONE_DEBUG, (TEXT("romfs:  GetFileTime entered %x create %x ,Access %x, Write %x\r\n")
		             ,pF,pCreation,pLastAccess,pLastWrite));
    
	EnterCriticalSection( &pFile->cs_File );
	
	if( pCreation!=NULL)
		*pCreation	= pFile->ftCreationTime;
	if( pLastAccess !=NULL)
		*pLastAccess=pFile->ftLastAccessTime;
	if( pLastWrite != NULL)
		*pLastWrite	= pFile->ftLastWriteTime;

	LeaveCriticalSection( &pFile->cs_File );
	RETAILMSG(ZONE_FILE, (TEXT("romfs:  GetFileTime leaved\r\n")));
	return TRUE;
}*/

BOOL   ROM_CloseFile(HANDLE pFileClose)
{
	return DoROM_CloseFile((PFILE)pFileClose);
}

static BOOL   DoROM_CloseFile(PFILE pFileClose)
{
	PROMFILE  pFile=(PROMFILE)pFileClose;
	PROMFILE  pF   =pFileList;
	BOOL      status = FALSE;

	DEBUGMSG(ZONE_FILE, (TEXT("romfs: CloseFile entered\r\n")));

	EnterCriticalSection( &Crit_File );
	if( pFile == pFileList){
       
		pFileList = pFile->next;  // delete the node specified by pFile
		DEBUGMSG(ZONE_FILE, (TEXT("romfs: CloseFile succeed\r\n")));
		status=TRUE;
	}else{
		while (pF->next) 
		{
			if( pF->next==pFile ){

				pF->next=pFile->next; // delete the node specified by pFile
				DEBUGMSG(ZONE_FILE, (TEXT("romfs: CloseFile succeed\r\n")));
				status=TRUE;
				break;
			}
			pF = pF->next;
		}
	}
	LeaveCriticalSection( &Crit_File );

	if( status  ) { // free this node.
		CRITICAL_SECTION  crit_this=pFile->cs_File;

		EnterCriticalSection( &crit_this );

		KHeap_Free( pFile, sizeof( ROMFILE) );
		//free( pFile);

		LeaveCriticalSection( &crit_this);
		DeleteCriticalSection(&crit_this);
	}else{
		SetLastError(  ERROR_INVALID_HANDLE);
	}
	DEBUGMSG(ZONE_FILE, (TEXT("romfs: CloseFile leaved %x\r\n"),status));
    return status;
}

/*
HANDLE  DoROM_FindFirstFileW(PVOL pVolume, HANDLE hProc, LPCTSTR pwsFileSpec, FILE_FIND_DATA *  pfd)
{
	PROMVOLUME     pVol=(PROMVOLUME)pVolume;
	PROMSEARCH     pSearch;
//	HANDLE         ret;

	//RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW the pSearch path_%s \r\n"), pwsFileSpec));
	//RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW volume %x ,disk %x \r\n"),pVolume, pVol->hDevice));
	
	if( ! IsVolume(pVol) ){
		RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW volume %x is invalid\r\n"),pVolume));
		return INVALID_HANDLE_VALUE;
	}
	if( pwsFileSpec == NULL || pfd == NULL ){
		SetLastError( ERROR_INVALID_PARAMETER);
		return INVALID_HANDLE_VALUE;
	}
    EnterCriticalSection( &pVol->cs_Volume );
	
	//RETAILMSG(ZONE_ERROR,(TEXT("rom:1\r\n")));

	pSearch=(PROMSEARCH)HELP_FindFirstFile( pVolume, pwsFileSpec, pfd, FIND_NORMAL_CALL );
	
	RETAILMSG(ZONE_SEARCH,(TEXT("romfs:FindFirstFileW %s \r\n"),pfd->cFileName));
	
	if( (HANDLE)pSearch !=INVALID_HANDLE_VALUE ) {		
		
		// insert this handle to the search list table head.
		strcpy(pSearch->wFileSpecial,pwsFileSpec );
		pSearch->pVol = pVol;
		
		RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW the pSearch path %s \r\n"),pSearch->wFileSpecial));
		
		RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW before initialize section\r\n")));
		InitializeCriticalSection( &pSearch->cs_Search );
		RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW after initialize section\r\n")));
		
		LeaveCriticalSection( &pVol->cs_Volume );
		
		EnterCriticalSection(&Crit_Search);
		pSearch->next = pSearchList; // insert this node into list.
		
		pSearchList=pSearch;
		LeaveCriticalSection(&Crit_Search);
		
		return (HANDLE)pSearch;
		
	}
	
	RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW  failed %d \r\n"),GetLastError( ) ));
Error_Search:
	LeaveCriticalSection(&pVol->cs_Volume);
	return INVALID_HANDLE_VALUE;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL   DoROM_FindNextFileW(PFIND pSearchBuffer, FILE_FIND_DATA *  pfd)
{
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	PROMFILE		pFile;

	PROMSEARCH    pSearch= (PROMSEARCH)pSearchBuffer;
	WORD		  wOffset;
	WORD          wSectorSize;
	WORD          wOffsetInSector;
	PROMVOLUME    pCdVol;
	BOOL          status=FALSE;
	UINT		  index, FileMode;

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs: DoROM_FindNextFileW <\r\n")));

	if(pSearch->pVol == NULL){
		RETAILMSG(ZONE_SEARCH, (TEXT("romfs: DoROM_FindNextFileW parm error!\r\n")));
		return FALSE;
	}

	memset(pfd->cFileName, 0, sizeof(pfd->cFileName));

	if( ! HELP_IsSearch (pSearch ) ) {
		RETAILMSG(ZONE_ERROR,(TEXT("romfs:  FindNextFileW  pSearch  %x is invalid\r\n"),pSearch));
		return FALSE;
	}
	if( pfd ==NULL ) {
		RETAILMSG(ZONE_ERROR,(TEXT("romfs:  FindNextFileW  pfd (NULL) is invalid\r\n")));
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	EnterCriticalSection( &pSearch->cs_Search);

//	FormatFilePath( (TCHAR *)pSearch->wFileSpecial);

	pFile = malloc(sizeof( ROMFILE));
	if( pFile != NULL ){

		//RETAILMSG(ZONE_ERROR,(TEXT("rom:2\r\n")));

		pSearch = HELP_FindFirstFile((PVOL)pSearch->pVol, pSearch->wFileSpecial, (FILE_FIND_DATA *)pFile, FIND_CREATE_CALL);
		if(pSearch == INVALID_HANDLE_VALUE){
			free(pFile);	
			return FALSE;
		}
		if(pFile->dwFileBase != pSearch->pFileBase){
			memcpy(pfd, pFile, sizeof(FILE_FIND_DATA));
		}else{
			RETAILMSG(ZONE_DEBUG, (TEXT("romfs: There is no file!\r\n")));
		}
		
		free(pSearch);
		free(pFile);
	}
	
	LeaveCriticalSection( &pSearch->cs_Search);

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs: FindNextFileW leaved %s\r\n"),pfd->cFileName));
	return status;
}*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*BOOL   DoROM_FindClose(PFIND pSearchIn)
{
	PROMSEARCH pSearch= (PROMSEARCH)pSearchIn;
	PROMSEARCH pSh=pSearchList;
	BOOL       status=FALSE;

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs:  FindClose entered %x \r\n"),pSearchIn));

	EnterCriticalSection( &Crit_Search );

	if( pSearch == pSearchList){

		pSearchList = pSearch->next;
		status = TRUE;
		RETAILMSG(ZONE_SEARCH, (TEXT("romfs: CloseSearchHandle succeed\r\n")));
	}else{
		while (pSh->next) 
		{
			if( pSh->next == pSearch )
			{
				pSh->next = pSearch->next;
				status	  = TRUE;
				RETAILMSG(ZONE_SEARCH, (TEXT("romfs: CloseSearchHandle succeed\r\n")));
			}
		}
	}
	LeaveCriticalSection( &Crit_Search );
    if( status ) {
		CRITICAL_SECTION  crit_this = pSearch->cs_Search;
	
		EnterCriticalSection( &crit_this );

		LeaveCriticalSection( &crit_this);
		DeleteCriticalSection(&crit_this);

	}else{
		SetLastError(  ERROR_INVALID_HANDLE);
		RETAILMSG(ZONE_SEARCH, (TEXT("romfs: CloseSearchHandle failed\r\n")));
	}

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs: CloseSearchHandle leaved %d\r\n"),status));
	return status;
}
*/
