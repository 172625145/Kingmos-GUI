/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：rom文件系统
版本号：1.0.0
开发时期：2003-04-40
作者：ZB
修改记录：
	1. 2003-09-13...zb:  add function ROM_GetDiskFreeSpace for shell .
遗留问题：
	1. 2003-09-13...zb:  如何防止其他程序copy XIP文件
******************************************************/

#include <ewindows.h>
#include <efsdmgr.h>
#include <epalloc.h>
#include <edevice.h>
#include <Eassert.h>
#include <romfs.h>

#include <romheader.h>
#include <devdrv.h>
//#include <Rom_m.h>
//#include <VCDioctr.h>
//#include <ROMdisk.h>

//#define FSDAPI VCDFSD

//#include "CDMDAta.h"
//#include "CDMFSD.h"
//#include "extern.h"




PROMVOLUME			pVolumeList=NULL;
PROMSEARCH			pSearchList=NULL;
HANDLE				hDll=NULL;
PROMFILE			pFileList=NULL;

CRITICAL_SECTION    Crit_Volume, Crit_File, Crit_Search;

FILECHANGEINFO		FileChanger;
TCHAR               FolderName[]="kingmos";
TCHAR				FsdName[] = "ROMFS";

const ROMHDR * pTOC;


struct _FSDDRV RomFSDDrv = {
	&ROM_CloseFile,
//	&ROM_CloseVolume,
	NULL,	
//	&ROM_CreateDirectoryW,
	NULL,
	&ROM_CreateFileW,
//	&ROM_DeleteAndRenameFileW,
	NULL,
//	&ROM_DeleteFileW,
	NULL,
	&ROM_DeviceIoControl,	//have not implement.	
	&ROM_FindClose,
	&ROM_FindFirstFileW,
	&ROM_FindNextFileW,
//	&ROM_FlushFileBuffers,
	NULL,
	&ROM_GetDiskFreeSpace,
//	NULL,
	&ROM_GetFileAttributesW,
//	&ROM_GetFileInformationByHandle,
	NULL,
	&ROM_GetFileSize,
	&ROM_GetFileTime,
	&ROM_MountDisk,
//	&ROM_MoveFileW,
	NULL,
//	&ROM_Notify,				//have not implement.
	NULL,
	&ROM_ReadFile,
//	&ROM_ReadFileWithSeek,	//don't have.
	NULL,
	&ROM_RegisterFileSystemFunction,
//	&ROM_RemoveDirectoryW,
	NULL,
//	&ROM_SetEndOfFile,
	NULL,
//	&ROM_SetFileAttributesW,
	NULL,
	&ROM_SetFilePointer,
//	&ROM_SetFileTime,
	NULL,
	&ROM_UnmountDisk,
//	&ROM_WriteFile,
	NULL,
//	&ROM_WriteFileWithSeek,
	NULL,
	//&ROM_CopyFile			//don't have.
	NULL
};

BOOL RegisterRomFSD(void )
{
	if(!FSDMGR_RegisterFSD(FsdName, &RomFSDDrv)){
		RETAILMSG(ZONE_ERROR, (TEXT("Rom FSD: register Rom FSD fail!\r\n")));
		return FALSE;
	}else{
		RETAILMSG(ZONE_ERROR, (TEXT("Rom FSD: register Rom FSD success!\r\n")));
		return TRUE;
	}		
}

void DeinitRomFsd(void)
{
	DeleteCriticalSection( &Crit_Volume);
	DeleteCriticalSection( &Crit_File  );
	DeleteCriticalSection( &Crit_Search );

}

void InitRomFsd(void)
{
	//
	//Add for get pToc by zb...2003-7-1.
	//
	

	pTOC = (const ROMHDR *)GetSysTOC();
	//RETAILMSG(1, (TEXT("InitRomFsd,pTOC=0x%x!\r\n"), pTOC));

	InitializeCriticalSection( &Crit_Volume );
	InitializeCriticalSection( &Crit_File   );
	InitializeCriticalSection( &Crit_Search );

	if(!RegisterRomFSD())
		DeinitRomFsd();

	//return TRUE;
}

BOOL  ROM_MountDisk(HDSK hDisk)
{
	PROMVOLUME		pVolume;
	HVOL			VolumeIdentifer=0;
//	CDCALLBACK      dwCallBackFunc;
	WORD			wFolderLen=0; 
	

	RETAILMSG(ZONE_MOUNT, (TEXT("romfs: MountDisk entered %x\r\n"),hDisk));

	pVolume = CreateVolume(hDisk  );
	if( pVolume ==NULL )
		return FALSE;

    return HELP_RegisterVolume(  pVolume );

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL  ROM_UnmountDisk(HDSK hDisk)
{
	PROMVOLUME pVol , pVolDel;
	HVOL              hVol;
	BOOL       status=FALSE;

	RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk entered %x\r\n"),hDisk));
    
	EnterCriticalSection( &Crit_Volume);
	RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk after critical section\r\n")));

	pVol=pVolumeList;
	if( pVolumeList !=NULL ){

		if( pVolumeList->hDevice ==hDisk ){

			RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk   device should unMount\r\n")));
			pVolDel=pVolumeList;
			pVolumeList=pVolumeList->next;
			status=TRUE;
		}else{
		
			RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk   there are more than 2 device\r\n")));
			while( pVol->next){

				if( pVol->next->hDevice ==hDisk)
				{
					RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk device%x (%x) has destroyed\r\n"),hDisk,pVol->hVolume));
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

	RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk  after Crit_Volume\r\n"),hDisk));

	if( status ) {
		CRITICAL_SECTION  crit_this=pVolDel->cs_Volume;
		hVol=pVolDel->hVolume;

		EnterCriticalSection(&crit_this );
		free( pVolDel );
		LeaveCriticalSection(&crit_this );

		DeleteCriticalSection(&crit_this);
		FSDMGR_DeregisterVolume( hVol );
		 return TRUE;

	}else{
		SetLastError( ERROR_INVALID_HANDLE);
		RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk  failed\r\n"),hDisk));
	}
	RETAILMSG(ZONE_MOUNT, (TEXT("romfs: UnMountDisk  leaved\r\n")));
	return status;
}


BOOL ROM_GetDiskFreeSpace( PVOL pVolume, LPCTSTR lpcszPathName, LPDWORD pSectorsPerCluster, LPDWORD pBytesPerSector, LPDWORD pFreeClusters, LPDWORD pClusters )
{

	PROMVOLUME			pVol=(PROMVOLUME)pVolume;

	RETAILMSG(ZONE_FILE, (TEXT("romfs:  ROM_GetDiskFreeSpace\r\n")));

	//RETAILMSG(1, ("romfs: %s\r\n",lpcszPathName));

	if(stricmp(lpcszPathName,"\\") != 0){
		RETAILMSG(1, ("romfs: not the kingmos path!\r\n"));
		return FALSE;
	}

	if( ! HELP_IsVolume(pVol ) ){

		RETAILMSG(ZONE_ERROR, (TEXT("romfs: ROM_GetDiskFreeSpace volume  %x is invalid\r\n"),pVol));
		return  FALSE;
	}
	
 
	*pSectorsPerCluster	= 1;
	*pFreeClusters		= 0;
	*pBytesPerSector	= 512;
	*pClusters = (pTOC->physlast - pTOC->physfirst) / *pBytesPerSector;
	

	RETAILMSG(ZONE_DISK, (TEXT("CDMFSD:  GetDiskFreeSpace succeeded!   \r\n")));
	return TRUE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL    ROM_RegisterFileSystemFunction(PVOL pVolume, SHELLFILECHANGEFUNC pfn)
{
	PROMVOLUME  pVol=(PROMVOLUME)pVolume;
	
	RETAILMSG(ZONE_MOUNT, (TEXT("romfs:  ROM_RegisterFileSystemFunction entered %x\r\n"),pfn));

	EnterCriticalSection( &Crit_Volume);
	pVol->pNotifyFunction = pfn;
	LeaveCriticalSection( &Crit_Volume);

	(*pfn)(&FileChanger);

	return TRUE;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void    ROM_Notify(PVOL pVolume, DWORD dwFlags)
{
	RETAILMSG(ZONE_OUT, (TEXT("romfs:  The function isn't implement\r\n")));
}

BOOL DEV_AllocFile(PROMFILE pFile, LPDWORD pFileBase, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT				index, FileMode;
	LPFILESentry		pFileentry;

	if(!HELP_FindFile(pFile->cFileName, &index, &FileMode))
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

	if(!HELP_FindFile(pFile->cFileName, &index, &FileMode))
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

	if(!HELP_FindFile(pFile->cFileName, &index, &FileMode))
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

	if(!HELP_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_MOD)
		return FALSE;
	
	pTocentry = (LPTOCentry)pTOC->ulModOffset;
	pTocentry += index;

	RETAILMSG(ZONE_DEBUG, (TEXT("elf info:.........\r\n")));
	RETAILMSG(ZONE_DEBUG, (TEXT("ulEntry: %x,ulFileBase:%x\r\n"), pTocentry->ElfInfo.ulEntry, pTocentry->ElfInfo.ulFileBase));

	memcpy(pElfInfo, &pTocentry->ElfInfo, sizeof(ELF_INFO));
	*pBytesReturned = sizeof(ELF_INFO);

	return TRUE;		
}

BOOL DEV_GetSegInfo(PROMFILE pFile, PMODULE_SEG pElfInfo, DWORD nOutBufSize, LPDWORD pBytesReturned)
{
	UINT	index, FileMode;
	LPTOCentry		pTocentry;

	if(!HELP_FindFile(pFile->cFileName, &index, &FileMode))
		return FALSE;

	if(FileMode != FILE_MODE_MOD)
		return FALSE;
	
	pTocentry = (LPTOCentry)pTOC->ulModOffset;
	pTocentry += index;

	RETAILMSG(ZONE_DEBUG, (TEXT("elf info:.........\r\n")));
	RETAILMSG(ZONE_DEBUG, (TEXT("ulEntry: %x,ulFileBase:%x\r\n"), pTocentry->ElfInfo.ulEntry, pTocentry->ElfInfo.ulFileBase));

	if(pTocentry->bValidRes == TRUE){
		memcpy(pElfInfo, &pTocentry->ResSeg, sizeof(MODULE_SEG));
		*pBytesReturned = sizeof(MODULE_SEG);
		RETAILMSG(ZONE_DEBUG, (TEXT("romfs:### %x:%x\r\n"),sizeof(MODULE_SEG),*pBytesReturned ));
		return TRUE;		
	}
	//if the module no res, set flag: 0;
	*pBytesReturned = 0;
	RETAILMSG(ZONE_DEBUG, (TEXT("romfs:!!!### %x\r\n"),*pBytesReturned ));
	return TRUE;

	//return FALSE;	
}


BOOL   ROM_DeviceIoControl(
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

	RETAILMSG(ZONE_OUT, (TEXT("ROM_DeviceIoControl: < (Control code %x)\r\n"),dwIoControlCode));

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
		RETAILMSG(ZONE_DEBUG, (TEXT("ROMFS: unknown iocontrolcode...\r\n")));
	}

	//do action ....
	switch(dwIoControlCode)
	{
	case IOCTL_ROM_GET_ELF_INFO:
		RETAILMSG(ZONE_DEBUG, (TEXT("ROMFS: addr:%x\r\n"), pOutBuf));

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
		RETAILMSG(ZONE_DEBUG, (TEXT("ROMFS: unknown iocontrolcode...\r\n")));
	}

	RETAILMSG(ZONE_OUT, (TEXT("ROM_DeviceIoControl: > %x\r\n"),bStatus));
	return bStatus;
}


HANDLE ROM_CreateFileW( 
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

	RETAILMSG(ZONE_FILE, (TEXT("romfs: CreatFile entered  %s\r\n"),pwsFileName));
//  now, check the actual parameters.
	if( !(dwCreate == OPEN_EXISTING || dwCreate == OPEN_ALWAYS) || 
		 pwsFileName == NULL || 
		 dwAccess != GENERIC_READ ) 
	{		
		RETAILMSG(ZONE_ERROR, (TEXT("romfs: CreatFile  take error parameter\r\n")));
		SetLastError( ERROR_INVALID_PARAMETER);
		return INVALID_HANDLE_VALUE;
	}

	if( ! HELP_IsVolume(pVolume ) ){

		RETAILMSG(ZONE_ERROR, (TEXT("romfs: CreatFile volume  %x is invalid\r\n"),pVol));
		return  INVALID_HANDLE_VALUE;
	}
	if( ! IsFileOpened( pVolume,pwsFileName ) ){
		SetLastError( ERROR_SHARING_VIOLATION );  // share violation.
		return INVALID_HANDLE_VALUE;
	}
    EnterCriticalSection( &pVolume->cs_Volume );

	//pFile=LocalAlloc(LPTR, sizeof( ROMFILE));
	pFile = malloc(sizeof( ROMFILE));
	if( pFile != NULL ){

		//RETAILMSG(ZONE_ERROR,(TEXT("rom:3\r\n")));

		pSearch = HELP_FindFirstFile(pVol, pwsFileName, (FILE_FIND_DATA *)pFile, FIND_CREATE_CALL,NULL);
		if(pSearch == INVALID_HANDLE_VALUE)
			goto  Error_Return;
		
		free(pSearch);

		//The line is removed by zb...2003-07-12...
		//HELP_FindFirstFile have <strcpy>!
		//strcpy(pFile->cFileName, pwsFileName);
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
		
		RETAILMSG(ZONE_FILE, (TEXT("romfs: CreatFile leaved  %x\r\n"),pFile));
		return (HANDLE)pFile;			   
	}
	   
Error_Return:
	free(pFile);
	

	RETAILMSG(ZONE_FILE|ZONE_ERROR, (TEXT("romfs: CreatFile  Failed\r\n")));

	LeaveCriticalSection( &pVolume->cs_Volume );
 	return INVALID_HANDLE_VALUE;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DWORD  ROM_GetFileAttributesW(PVOL pVolume, LPCTSTR pwsFileName)
{
    FILE_FIND_DATA		File;
	PROMSEARCH			pSearch;
	DWORD				dwAttr=0xFFFFFFFF;
	PROMVOLUME			pVol=(PROMVOLUME)pVolume;

	RETAILMSG(ZONE_FILE, (TEXT("romfs:  ROM_GetFileAttributesW entered %s \r\n"),pwsFileName));
	//RETAILMSG(ZONE_ERROR, (TEXT("romfs: %s\r\n"),pwsFileName));

	//if((strcmp(pwsFileName, "\\hellores.exe") != 0) && 
	//	(strcmp(pwsFileName, "\\libtestdll1.so") != 0))
	//	ASSERT(0);

	if( ! HELP_IsVolume(pVol ) ){

		RETAILMSG(ZONE_ERROR, (TEXT("romfs: GetFileAttribute volume  %x is invalid\r\n"),pVol));
		return  0xFFFFFFFF;
	}
	//RETAILMSG(ZONE_FILE, (TEXT("romfs:  ROM_GetFileAttributesW  %s  before Crit\r\n"),pwsFileName));

    EnterCriticalSection( &pVol->cs_Volume );
	
//	RETAILMSG(ZONE_ERROR,(TEXT("rom:4\r\n")));

	pSearch=( PROMSEARCH )HELP_FindFirstFile(pVolume, pwsFileName, &File, FIND_NORMAL_CALL,NULL);
	
	if((HANDLE) pSearch != INVALID_HANDLE_VALUE ) {
		
		dwAttr=File.dwFileAttributes;
		
		free(pSearch);
		
		//RETAILMSG(ZONE_FILE, (TEXT("romfs:  ROM_GetFileAttributesW leaved %x\r\n"),dwAttr));
	}
	
	LeaveCriticalSection ( &pVol->cs_Volume );

	RETAILMSG(ZONE_FILE, (TEXT("romfs:  ROM_GetFileAttributesW leaved %x\r\n"),dwAttr));
	return dwAttr;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL   ROM_ReadFile(PFILE pF, PVOID pBufferOut, DWORD cbRead, LPDWORD pcbRead, LPOVERLAPPED pOverlapped)
{
	PROMFILE   pFile	=	(PROMFILE)pF;
	PUCHAR     pOut		=	(PUCHAR)pBufferOut;
	WORD       wSectorSize;
	DWORD      dwReadSectorNum;    // this time, the how many sectors should be read out.
	DWORD      dwReadStartSector;  // which sector should  start read from this time ?
	WORD       wBytesBeforeSector; // before read the dwReadStartSector, how many bytes should be read,
	WORD       wBytesAfterSector;//after read the dwReadStartSector, how many bytes should be read,
	BOOL       status	=	TRUE;

	RETAILMSG(ZONE_READ, (TEXT("romfs:  ReadFile: < %x  %x\r\n"),pFile,cbRead ));


	if( ! HELP_IsFile( pFile) ){
		RETAILMSG(ZONE_READ, (TEXT("romfs:  ReadFile :> %x  %x\r\n"),pFile,cbRead ));
		return   FALSE;
	}
	if( pBufferOut ==NULL || pcbRead==NULL )
	{
		SetLastError( ERROR_INVALID_PARAMETER);
		RETAILMSG(ZONE_READ, (TEXT("romfs:  ReadFile :> %x  %x\r\n"),pFile,cbRead ));

		return   FALSE;
	}
	EnterCriticalSection( & (pFile->cs_File) );

	if( pFile->dwFilePointer >= pFile->nFileSizeLow ) {

		RETAILMSG(ZONE_READ, (TEXT("romfs:  ReadFile reach EOF \r\n") ));
		if( pcbRead )
			*pcbRead = 0;
		goto  Error_Return;
	}


	cbRead = min( cbRead,  ( pFile->nFileSizeLow - pFile->dwFilePointer ));

	RETAILMSG(ZONE_READ, (TEXT("romfs:  ReadFile  cbRead=%x \r\n"),cbRead ));
	
	memcpy(  pOut, (const void*)(pFile->dwFileBase + pFile->dwFilePointer), cbRead);

	if( pcbRead != NULL ) {
		*pcbRead = cbRead;
	}
    pFile->dwFilePointer += cbRead;

	if( pFile->dwFilePointer > pFile->nFileSizeLow)
		pFile->dwFilePointer = pFile->nFileSizeLow;

Error_Return:
	LeaveCriticalSection( &pFile->cs_File);

	RETAILMSG(ZONE_READ, (TEXT("romfs:  ReadFile leaved %x ,%x\r\n"),*pcbRead ,status ));

	return status;
	//return TRUE;
}

DWORD  ROM_SetFilePointer(PFILE pFileIn, LONG lDistanceToMove, LPLONG pDistanceToMoveHigh, DWORD dwMoveMethod)
{
	DWORD			dwPointer;
	PROMFILE		pFile=(PROMFILE)pFileIn;
	BOOL            status=TRUE;

	RETAILMSG(ZONE_FILE, (TEXT("romfs:SetFilePointer %d \r\n"),lDistanceToMove));

	if( ! HELP_IsFile(pFile) ){
		return  0xFFFFFFFF;
	}

	EnterCriticalSection( &(pFile->cs_File) );
	switch( dwMoveMethod ) {
	
		case FILE_CURRENT:

			RETAILMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer in CURRENT \r\n")));
			if( lDistanceToMove < 0 ){
				if( (DWORD)labs(lDistanceToMove) > pFile->dwFilePointer){
					SetLastError( ERROR_NEGATIVE_SEEK );
					status=FALSE;
					break;
				}				
			}
			dwPointer=pFile->dwFilePointer + lDistanceToMove;
			
			break;
		case FILE_BEGIN:
			RETAILMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer in BEGIN \r\n")));
			if( lDistanceToMove < 0 ){
				status=FALSE;
				SetLastError( ERROR_NEGATIVE_SEEK );
			}else{
				dwPointer=lDistanceToMove;
			}
			break;
		case FILE_END:
			RETAILMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer in END \r\n")));

			if( lDistanceToMove > 0 ){
				dwPointer=pFile->nFileSizeLow;
			}else{
				if( (DWORD)labs(lDistanceToMove) > pFile->nFileSizeLow){
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
	RETAILMSG(ZONE_FILE, (TEXT("romfs: FileSize-%x, dwPointer-%x \r\n"), pFile->nFileSizeLow, dwPointer));

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
		RETAILMSG(ZONE_FILE, (TEXT("romfs:SetFilePointer not support pDistanceToMoveHigh parm!\r\n")));
		
/*		if(*pDistanceToMoveHigh !=0 ) {
		
			dwPointer =pFile->nFileSizeLow;
			pFile->dwFilePointer= dwPointer;
			*pDistanceToMoveHigh=0;
		}
*/
	}

	LeaveCriticalSection( &(pFile->cs_File));
	RETAILMSG(ZONE_FILE, (TEXT("romfs:  SetFilePointer leaved %d\r\n"),pFile->dwFilePointer));

	return dwPointer;
}

DWORD  ROM_GetFileSize(PFILE pFileIn, LPDWORD pFileSizeHigh)
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
}

//The function don't support now.//
/*BOOL   ROM_GetFileInformationByHandle(PFILE pFileIn, FILE_INFORMATION *pFileInfo)
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

BOOL  ROM_GetFileTime(PFILE pF, FILETIME *pCreation, FILETIME *pLastAccess, FILETIME *pLastWrite)
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
}

#define DEBUG_ROM_CloseFile 0
BOOL   ROM_CloseFile(PFILE pFileClose)
{
	PROMFILE  pFile=(PROMFILE)pFileClose;
	PROMFILE  pF   =pFileList;
	BOOL      status = FALSE;

	DEBUGMSG(DEBUG_ROM_CloseFile, (TEXT("romfs: CloseFile entered\r\n")));

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

		free( pFile);
		LeaveCriticalSection( &crit_this);
		DeleteCriticalSection(&crit_this);
	}else{
		SetLastError(  ERROR_INVALID_HANDLE);
		WARNMSG(ZONE_FILE|DEBUG_ROM_CloseFile, (TEXT("romfs: error in CloseFile.\r\n")));
	}
	DEBUGMSG(ZONE_FILE|DEBUG_ROM_CloseFile, (TEXT("romfs: CloseFile leaved %x\r\n"),status));
    return status;
}


HANDLE  ROM_FindFirstFileW(PVOL pVolume, HANDLE hProc, LPCTSTR pwsFileSpec, FILE_FIND_DATA *  pfd)
{
	PROMVOLUME     pVol=(PROMVOLUME)pVolume;
	PROMSEARCH     pSearch;
//	HANDLE         ret;

	//RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW the pSearch path_%s \r\n"), pwsFileSpec));
	//RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW volume %x ,disk %x \r\n"),pVolume, pVol->hDevice));
	
	if( ! HELP_IsVolume(pVol) ){
		RETAILMSG(ZONE_SEARCH,(TEXT("romfs: FindFirstFileW volume %x is invalid\r\n"),pVolume));
		return INVALID_HANDLE_VALUE;
	}
	if( pwsFileSpec == NULL || pfd == NULL ){
		SetLastError( ERROR_INVALID_PARAMETER);
		return INVALID_HANDLE_VALUE;
	}
    EnterCriticalSection( &pVol->cs_Volume );
	
	//RETAILMSG(ZONE_ERROR,(TEXT("rom:1\r\n")));

	pSearch=(PROMSEARCH)HELP_FindFirstFile( pVolume, pwsFileSpec, pfd, FIND_NORMAL_CALL ,NULL);
	
	//RETAILMSG(ZONE_SEARCH,(TEXT("romfs:FindFirstFileW %s \r\n"),pfd->cFileName));
	
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
BOOL   ROM_FindNextFileW(PFIND pSearchBuffer, FILE_FIND_DATA *  pfd)
{
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	PROMFILE		pFile;

	PROMSEARCH    pSearch= (PROMSEARCH)pSearchBuffer;
	PROMSEARCH    pSearchTemp = NULL;
	WORD		  wOffset;
	WORD          wSectorSize;
	WORD          wOffsetInSector;
	PROMVOLUME    pCdVol;
	BOOL          status=FALSE;
	UINT		  index, FileMode;

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs: ROM_FindNextFileW <\r\n")));

	if(pSearch->pVol == NULL){
		RETAILMSG(ZONE_SEARCH, (TEXT("romfs: ROM_FindNextFileW parm error!\r\n")));
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

		//pSearch = HELP_FindFirstFile((PVOL)pSearch->pVol, pSearch->wFileSpecial, (FILE_FIND_DATA *)pFile, FIND_CREATE_CALL,pSearch->wPreFindedFile);
		//if(pSearch == INVALID_HANDLE_VALUE){
		pSearchTemp = HELP_FindFirstFile((PVOL)pSearch->pVol, pSearch->wFileSpecial, (FILE_FIND_DATA *)pFile, FIND_CREATE_CALL,pSearch->wPreFindedFile);
		if(pSearchTemp == INVALID_HANDLE_VALUE){		

			free(pFile);	
			//RETAILMSG(ZONE_ERROR,(TEXT("rom:1111111111\r\n")));
			return FALSE;
		}
		strcpy(pSearch->wPreFindedFile,pSearchTemp->wPreFindedFile);

		//Zb changed to below for fix bug...2003-07-12...
		/*if(pFile->dwFileBase != pSearch->pFileBase){
			memcpy(pfd, pFile, sizeof(FILE_FIND_DATA));
		}else{
			RETAILMSG(ZONE_DEBUG, (TEXT("romfs: There is no file!\r\n")));
		}*/
		memcpy(pfd, pFile, sizeof(FILE_FIND_DATA));
		//Zb changed to below for fix bug...2003-07-12...
		
		//free(pSearch);
		free(pSearchTemp);
		free(pFile);

		//RETAILMSG(ZONE_ERROR,(TEXT("rom:2222222222\r\n")));
		status = TRUE;
	}
	
	LeaveCriticalSection( &pSearch->cs_Search);

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs: FindNextFileW leaved %s\r\n"),pfd->cFileName));
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL   ROM_FindClose(PFIND pSearchIn)
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
				break;
			}
			pSh = pSh->next;
		}
	}
	LeaveCriticalSection( &Crit_Search );
    if( status ) {
		CRITICAL_SECTION  crit_this = pSearch->cs_Search;
	
		EnterCriticalSection( &crit_this );

		LeaveCriticalSection( &crit_this);
		DeleteCriticalSection(&crit_this);
		// 2004-10-22, add
		free(pSearch);
		//

	}else{
		SetLastError(  ERROR_INVALID_HANDLE);
		WARNMSG(ZONE_SEARCH, (TEXT("romfs: CloseSearchHandle failed\r\n")));
	}

	RETAILMSG(ZONE_SEARCH, (TEXT("romfs: CloseSearchHandle leaved %d\r\n"),status));
	return status;
}
