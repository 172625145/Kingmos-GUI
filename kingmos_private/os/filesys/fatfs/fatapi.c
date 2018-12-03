/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：FAT文件系统的所有API，
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙。
修改记录：
	2004-06-17, WriteFile  & ReadFile 等没有对存取权限进行判断
******************************************************/

#include "fatfs.h"
#include "fatfmt.h"
#include "eshell.h"
#include <diskio.h>
#include <devdrv.h>

#ifdef	UNDER_CE
#include "Winbase.h"
#endif
// ********************************************************************
//	Mount. Unmount is implemented in mount.cpp.
// ********************************************************************
static DWORD	FatAttributeToFileSystem(DWORD	dwFatAttr);
static DWORD	FileSysAttributeToFat(DWORD	dwSysAttr);

#define DEBUG_AccessTest 0
static BOOL AccessTest( PVOLUME_FAT pVolume )
{
	// 卷是否可写。check volume is write enable ?
	if( pVolume->fda.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // 不可写。error 
		WARNMSG( DEBUG_AccessTest, ( "error in AccessTest ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//
	return TRUE;
}

BOOL FAT_MountDisk( HDSK hDsk )
{
	return MountFatDisk( hDsk );
}

BOOL FAT_UnmountDisk( HDSK hdsk )
{  
	return UnmountFatDisk(hdsk);
}
// ********************************************************************

BOOL FAT_FindClose( PFIND pSearch )
{  
	PFIND_RESULT  pFind =(PFIND_RESULT)pSearch;
	PVOLUME_FAT pVolume ;
	BOOL	bSuccess=FALSE;

	if( pFind ){

		pVolume=pFind->pVolume;

		if( IsVolume(  pVolume ) ){

			EnterCriticalSection( &pVolume->csVolCrit );
			bSuccess=DeleteSearchHandle( pVolume, pFind );
			LeaveCriticalSection( &pVolume->csVolCrit );
		}
	}
	return bSuccess;
}
#ifdef UNDER_CE
HANDLE FAT_FindFirstFile( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileSpec, PWIN32_FIND_DATAW  pfd )
#else
HANDLE FAT_FindFirstFile( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd )
#endif
{
	HANDLE hFind =INVALID_HANDLE_VALUE;
	PVOLUME_FAT pVolume= (PVOLUME_FAT)pVol;
	char   *pFileName=(char*)lpcszFileSpec;

#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszFileSpec,WideStrLen( lpcszFileSpec),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif
	RETAILMSG( DEBUG_FAT,(TEXT("FAT FindFirstFile %s\r\n"),lpcszFileSpec));
	
	if( IsVolume(  pVolume ) ){

		EnterCriticalSection( &pVolume->csVolCrit );

		hFind=FindFile(pVolume,pFileName,  (FILE_FIND_DATA*)pfd );

		LeaveCriticalSection( &pVolume->csVolCrit );
	}else{

		RETAILMSG( DEBUG_FAT,(TEXT("FAT FindFirstFile Volume invalid\r\n")));
	}
	if(  hFind != INVALID_HANDLE_VALUE ){
		
		pfd->dwFileAttributes=FatAttributeToFileSystem(pfd->dwFileAttributes);
		
#ifdef UNDER_CE
		
		hFind=FSDMGR_CreateSearchHandle( pVolume->hVolume, hProc,  (PSEARCH)hFind );
		if( hFind != INVALID_HANDLE_VALUE ){
			
			iLen=MultiByteToWideChar( CP_ACP, 0, (char*)pfd->cFileName, strlen((char*)pfd->cFileName),(WORD*)&csName[0],(MAX_PATH)/2);
			csName[iLen*2]=0;
			csName[iLen*2+1]=0;
			wcscpy((WORD*)pfd->cFileName, (WORD*)&csName[0]);
			
		}else{
			EnterCriticalSection( &pVolume->csVolCrit );
			DeleteSearchHandle( pVolume, hFind );
			LeaveCriticalSection( &pVolume->csVolCrit );
		}
#endif
	}
	RETAILMSG( DEBUG_FAT,(TEXT("FAT FindFirstFile leaved %s %x\r\n"),pfd->cFileName, hFind));
	return hFind;
}
#ifdef UNDER_CE
BOOL FAT_FindNextFile( PFIND hFind, PWIN32_FIND_DATAW  pfd )
#else
BOOL FAT_FindNextFile( PFIND hFind, FILE_FIND_DATA * pfd )
#endif
{
	PVOLUME_FAT pVolume;
	PFIND_RESULT  pFind =(PFIND_RESULT)hFind;
	BOOL	 bSuccess=FALSE;
#ifdef UNDER_CE
	WORD	csName[MAX_PATH];
	int		iLen;
#endif
	RETAILMSG( DEBUG_FAT,(TEXT("FAT FindNextFile entered %x\r\n"),hFind));
	if( pFind  ){

		pVolume= pFind->pVolume;

		if( IsVolume(  pVolume ) ){

			EnterCriticalSection( &pVolume->csVolCrit );

			if( IsSearch( pVolume, pFind ) ){

				if(FindFileInDirectory( pVolume, pFind->dwCluster, NULL,(FILE_FIND_DATA*)pfd ,pFind,TRUE) !=-1 ){ //success

					bSuccess=TRUE;
				}
			}
			LeaveCriticalSection( &pVolume->csVolCrit );
		}
	}
#ifdef UNDER_CE
	iLen=MultiByteToWideChar( CP_ACP, 0, (char*)pfd->cFileName, strlen((char*)pfd->cFileName),csName,(MAX_PATH));
	csName[iLen]=0;
	wcscpy((WORD*)pfd->cFileName, csName);
	pfd->dwFileAttributes=FatAttributeToFileSystem(pfd->dwFileAttributes);
#endif
	RETAILMSG( DEBUG_FAT,(TEXT("FAT FindNextFile leaved %s %x\r\n"),pfd->cFileName, bSuccess));
	return bSuccess;
}

BOOL FAT_CloseVolume( PVOL pVol )
{  
	PVOLUME_FAT pVolume= (PVOLUME_FAT)pVol;

	if( IsVolume(  pVolume ) ){

		PFIND_RESULT	 pFind ,pFindFree;
		PFILE_FAT		 pFile, pFileFree;

		EnterCriticalSection( &pVolume->csVolCrit );

		free( pVolume->ccClusterCache.pBufStart );
		free( pVolume->scFatCache.pBuf);

		// free find describer.
		pFind=pVolume->pFindListHead;
		while( pFind ){

			pFindFree=pFind;
			pFind=pFind->pNext;
			free( pFindFree );
		}
		pVolume->pFindListHead=NULL;

		/// Free the file describer.
		pFile=pVolume->pFileListHead;
		while( pFile ){

			pFileFree=pFile;
			pFile=pFile->pNext;
			FlushFileBuffer( pFileFree );

			free( pFileFree );
		}
		pVolume->pFileListHead=NULL;


		LeaveCriticalSection( &pVolume->csVolCrit );
		DeleteCriticalSection( &pVolume->csVolCrit);
		return TRUE;
	}
	return FALSE;
}

HANDLE FAT_CreateFile( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileName, DWORD dwAccess, 
					  DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate,
					  DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{  
	PVOLUME_FAT pVolume= (PVOLUME_FAT)pVol;
	HANDLE		hFileCreated =INVALID_HANDLE_VALUE;
	PUCHAR		pFileName=(PUCHAR)lpcszFileName;
#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszFileName,WideStrLen( lpcszFileName),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif

	RETAILMSG( DEBUG_FILE,(TEXT("FAT CreatFile  %s.. %x  %x, %x\r\n"),lpcszFileName,dwCreate,dwAccess,dwShareMode));

	if( IsVolume(  pVolume )  ){
		if( ( dwAccess & GENERIC_WRITE ) && !AccessTest( pVolume ) )
		{		
			return INVALID_HANDLE_VALUE;
		}

		EnterCriticalSection(&pVolume->csVolCrit);

		hFileCreated=CreateFatFile( pVolume, pFileName, dwAccess, dwShareMode, dwCreate,dwFlagsAndAttributes ,TRUE);
		
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FILE,(TEXT("FAT CreatFile leaved %x (%d)\r\n"),hFileCreated ,GetLastError( ) ));
	if( hFileCreated !=INVALID_HANDLE_VALUE ){

		DWORD	dwErr=GetLastError();

		if( dwErr==ERROR_INVALID_FUNCTION) {

			SetLastError( ERROR_SUCCESS );
			NotifyShell( pVolume, lpcszFileName, NULL, FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_ARCHIVE , 
						0, SHCNE_CREATE  );
		}
	}
#ifdef UNDER_CE
	return FSDMGR_CreateFileHandle( pVolume->hVolume, hProc,hFileCreated );
#else
	return hFileCreated;
#endif
}
BOOL FAT_CloseFile( PFILE pf )
{ 
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL	bSuccess =FALSE;

	RETAILMSG( DEBUG_FILE,(TEXT("FAT CloseFile entered %x\r\n"),pf));

	if( IsVolume(  pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);
		//FlushFileBuffer( pFile);
		bSuccess=DeleteFileHandle( pVolume, pFile ,(pFile->dwAccess & GENERIC_WRITE) );

		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FILE,(TEXT("FAT CloseFile leaved %x\r\n"),bSuccess));
	return bSuccess;
}

DWORD FAT_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	DWORD	dwFileSize =-1;

	RETAILMSG( DEBUG_FILE,(TEXT("FAT_GetFileSize\r\n")));
	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return -1;
		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {

			dwFileSize =pFile->dwSize;
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FILE,(TEXT("FAT_GetFileSize leaved %x\r\n"),dwFileSize));
	return dwFileSize;
}

BOOL FAT_GetFileInformationByHandle( PFILE pf, FILE_INFORMATION * pfi )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	RETAILMSG( DEBUG_FILE,(TEXT(" FAT_GetFileInformationByHandle entered %x\r\n"),pf));
	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {

			pfi->dwFileAttributes=pFile->dwAttr;
			pfi->dwVolumeSerialNumber;

			pfi->ftCreationTime.dwHighDateTime =pFile->ftCreate.dwHighDateTime;
			pfi->ftCreationTime.dwLowDateTime  =pFile->ftCreate.dwLowDateTime;

			pfi->ftLastAccessTime.dwHighDateTime =pFile->ftLastAccess.dwHighDateTime;
			pfi->ftLastAccessTime.dwLowDateTime  =pFile->ftLastAccess.dwLowDateTime;

			pfi->ftLastWriteTime.dwHighDateTime =pFile->ftLastWrite.dwHighDateTime;
			pfi->ftLastWriteTime.dwLowDateTime  =pFile->ftLastWrite.dwLowDateTime;

			pfi->nFileIndexHigh=0;
			pfi->nFileIndexLow =pFile->dwId;
			pfi->nFileSizeHigh=0;
			pfi->nFileSizeLow= pFile->dwSize;
			pfi->nNumberOfLinks=1;
			bSuccess=TRUE;
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FILE,(TEXT(" FAT_GetFileInformationByHandle leaved %x\r\n"),pf));
	return bSuccess;
}

BOOL FAT_GetFileTime( PFILE pf, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL	 bSuccess=FALSE;

	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_GetFileTime entered %x\r\n"),pf));

	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {

			if( pCreate ){
				pCreate->dwHighDateTime =  pFile->ftCreate.dwHighDateTime;
				pCreate->dwLowDateTime  =  pFile->ftCreate.dwLowDateTime;
			}
			if( pLastAccess ){
				pLastAccess->dwHighDateTime =  pFile->ftLastAccess.dwHighDateTime;
				pLastAccess->dwLowDateTime  =  pFile->ftLastAccess.dwLowDateTime;
			}
			if( pLastWrite ){
				pLastWrite->dwHighDateTime =  pFile->ftLastWrite.dwHighDateTime;
				pLastWrite->dwLowDateTime  =  pFile->ftLastWrite.dwLowDateTime;
			}
			bSuccess=TRUE;
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_GetFileTime leaved %x\r\n"),pf));
	return bSuccess;
}


DWORD FAT_GetFileAttributes( PVOL pVol, LPCTSTR lpcszFileName )
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	DWORD		dwAttr=-1;
	FILE_FIND_DATA	 find_data;
	PFIND_RESULT	pFind;
	PUCHAR		pFileName=(PUCHAR)lpcszFileName;
#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszFileName,WideStrLen( lpcszFileName),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif

	RETAILMSG( DEBUG_FAT,(TEXT("FAT Get Attr  %s\r\n"),lpcszFileName));	

	if( IsVolume(  pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		pFind= (PFIND_RESULT)FindFile(  pVolume, pFileName , &find_data);

		if( pFind == INVALID_HANDLE_VALUE){

			dwAttr=-1;
			//SetLastError(ERROR_FILE_NOT_FOUND); //findfile set it

		}else{

			dwAttr= find_data.dwFileAttributes;
			DeleteSearchHandle( pVolume, pFind );

			dwAttr=FatAttributeToFileSystem( dwAttr );
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FAT,(TEXT("FAT Get Attr  %s leaved %x\r\n"),lpcszFileName,dwAttr));
	return dwAttr;
}

BOOL FAT_SetFileAttributes( PVOL pVol, LPCTSTR lpcszFileName, DWORD dwFileAttributes )
{  
	PFIND_RESULT	 pFind;
	FILE_FIND_DATA	 find_data;
	BOOL			bSuccess=FALSE;
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	DWORD			dwAttr=0;

	PUCHAR		pFileName=(PUCHAR)lpcszFileName;
#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszFileName,WideStrLen( lpcszFileName),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif

	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_SetFileAttributes entered %s (%x)\r\n"),lpcszFileName ,dwFileAttributes));

	//return TRUE;
	if( IsVolume(  pVolume ) && AccessTest( pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		pFind =(PFIND_RESULT)FindFile(  pVolume, pFileName, &find_data);

		if( pFind != INVALID_HANDLE_VALUE ) {

			FILE_FAT	file_fat;

			DeleteSearchHandle( pVolume,  pFind  );

			file_fat.dwParentStart	= pFind->dwParentStart;
			file_fat.dwClusterParent= pFind->dwCluster;
			if( pFind->dwOffset )
				file_fat.dwClusterOffset= pFind->dwOffset-1;

			file_fat.dwAttr     =find_data.dwFileAttributes;
			dwFileAttributes=FileSysAttributeToFat(dwFileAttributes);
			if( file_fat.dwAttr & FILE_ATTR_DIRECTORY )
				dwFileAttributes|=FILE_ATTR_DIRECTORY;

			bSuccess=TRUE;
			if( dwFileAttributes!=file_fat.dwAttr ){ //The attribute should be changed only when it's not identical to the new value.

				bSuccess=SetFileAttributesByHandle( pVolume,&file_fat, dwFileAttributes );
			}
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_SetFileAttributes leaved %d \r\n"),bSuccess));

	if( bSuccess){

		NotifyShell( pVolume, lpcszFileName, NULL, dwFileAttributes,find_data.nFileSizeLow, SHCNE_ATTRIBUTES  );
	}
	return bSuccess;
}

DWORD FAT_SetFilePointer( PFILE pf, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{ 
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	long			lPos =-1;

	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_SetFilePointer entered\r\n")));
	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {
		
			switch( dwMoveMethod) {
				case FILE_CURRENT:
					lPos= pFile->dwPointer;
					break;
				case FILE_BEGIN:
					lPos= 0;
					break;
				case FILE_END:
					lPos=pFile->dwSize;
					break;
				default:
					lPos=-1;
					break;
			}
			if( lPos != -1 ){

				lPos+= lDistanceToMove ;
				if( lPos <0 )
					lPos=-1;
			}
			if( lPos != -1 ){

				if( lPos / pVolume->dwClusterSize != pFile->dwPointer /pVolume->dwClusterSize )
					pFile->dwClusterCurrent=-1; //新的指针和旧指针指向了不同的 cluster,将 dwClusterCurrent无效。

				pFile->dwPointer =lPos;
				//Braden masked the below line, it's error.
				//pFile->dwCachedCluster=-1; //set this number to -1, so that it can't be indentical with anyone.
			}else{

				SetLastError( ERROR_INVALID_PARAMETER);
			}
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_SetFilePointer leaved\r\n")));

	return lPos;
}

BOOL FAT_ReadFile( PFILE pf, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	RETAILMSG( DEBUG_READ,(TEXT("FAT ReadFile entered %d\r\n"),dwNumToRead));

	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {
			if( pFile->dwAccess & GENERIC_READ )   // lilin, add 2004-06-17
			    bSuccess=ReadFatFile( pFile,(char*)lpvBuffer, dwNumToRead, lpdwNumRead);
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_READ,(TEXT("FAT ReadFile leaved %d ,%d\r\n"),*lpdwNumRead,bSuccess));
	return bSuccess;
}


BOOL FAT_ReadFileWithSeek(PFILE pf,LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,
						  DWORD dwLowOffset,DWORD dwHighOffset)
{  
	//if( pFile->dwAccess & GENERIC_READ )   // lilin, add 2004-06-17
			//;
//	SetLastError(ERROR_NOT_SUPPORTED);
	return FALSE;
}
BOOL FAT_WriteFileWithSeek( PFILE pf, LPCVOID lpcvBuffer, DWORD cbWrite, LPDWORD pcbWritten, DWORD dwLowOffset, DWORD dwHighOffset )
{  
	//if( pFile->dwAccess & GENERIC_WRITE )   // lilin, add 2004-06-17
//	SetLastError(ERROR_NOT_SUPPORTED);
	return FALSE;
}


BOOL FAT_SetFileTime(PFILE pf,const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	RETAILMSG( DEBUG_FAT,(TEXT(" SetFileTime entered\r\n")));

	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {

			bSuccess=FatSetFileTime( pFile, pCreate, pLastAccess, pLastWrite );
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FAT,(TEXT(" SetFileTime leaved\r\n")));
	return bSuccess;
}

BOOL FAT_DeleteFile( PVOL pVol, LPCTSTR lpcszFileName )
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL		bSuccess=FALSE;
	HANDLE	hFile;

	PUCHAR		pFileName=(PUCHAR)lpcszFileName;
#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszFileName,WideStrLen( lpcszFileName),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif
	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_DeleteFile entered\r\n")));
	if( IsVolume(  pVolume ) && AccessTest( pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		hFile=CreateFatFile( pVolume, pFileName,GENERIC_WRITE|GENERIC_READ, 0, OPEN_EXISTING,0, TRUE);

		if( hFile != INVALID_HANDLE_VALUE) {

			bSuccess=FatDeleteFileByHanle( hFile ,TRUE);
			if( !bSuccess ){

				RETAILMSG( DEBUG_ERROR,(TEXT(" FAT_DeleteFile Can't Delete\r\n")));
			}
			DeleteFileHandle( pVolume, (PFILE_FAT)hFile ,FALSE);
		}else{

			RETAILMSG( DEBUG_ERROR,(TEXT(" FAT_DeleteFile Can't Create\r\n")));
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_FAT,(TEXT(" FAT_DeleteFile leaved %d\r\n"),bSuccess));
	if( bSuccess ){

		NotifyShell( pVolume, lpcszFileName, NULL, 0,0, SHCNE_DELETE   );
	}
	return bSuccess;
}


BOOL FAT_GetDiskFreeSpace( PVOL pVol, LPCTSTR lpcszPathName, 
						  LPDWORD pSectorsPerCluster, LPDWORD pBytesPerSector,
						  LPDWORD pFreeClusters, LPDWORD pClusters )
{
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL		bSuccess=FALSE;

	if( IsVolume(  pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		*pSectorsPerCluster =pVolume->fat_info.dwSecPerCluster;
		*pBytesPerSector	=pVolume->fat_info.dwBytesPerSec;
		//2004-08-25, modify by lilin
	    //*pFreeClusters      =pVolume->fat_info.dwFreeCluster-pVolume->dwRootDirClusterNum;
		*pFreeClusters      =pVolume->fat_info.dwFreeCluster;//-pVolume->dwRootDirClusterNum;
		//
		*pClusters			=pVolume->fat_info.dwClusterCount -pVolume->dwRootDirClusterNum;

		LeaveCriticalSection(&pVolume->csVolCrit);

		RETAILMSG( DEBUG_FAT,(TEXT(" FAT_GetDiskSpace SecPerClu %d.. BytesPerSec %d .. Free %d .. Total %d\r\n")
			, *pSectorsPerCluster,*pBytesPerSector,  *pFreeClusters, *pClusters	));
		return TRUE;
	}
	return FALSE;
}

/*
BOOL FAT_GetDiskFreeSpace( 
PVOL	 pVol, LPCTSTR lpcszPathName, 
DWORD*   lpFreeBytesAvailableToCaller, 
DWORD*   lpTotalNumberOfBytes, 
DWORD*   lpTotalNumberOfFreeBytes )
{

	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL		bSuccess=FALSE;

	if( IsVolume(  pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		*lpFreeBytesAvailableToCaller=*lpTotalNumberOfFreeBytes =pVolume->dwClusterSize * pVolume->fat_info.dwFreeCluster;

		*lpTotalNumberOfBytes=pVolume->dwClusterSize * pVolume->fat_info.dwClusterCount;

		LeaveCriticalSection(&pVolume->csVolCrit);
		return TRUE;
	}
	return FALSE;
}
*/
BOOL FAT_CreateDirectory( PVOL pVol, LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes)
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL	 bSuccess = FALSE;

	PUCHAR		pFileName=(PUCHAR)lpcszPathName;
#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszPathName,WideStrLen( lpcszPathName),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif

	if( IsVolume(  pVolume ) && AccessTest( pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		bSuccess=CreateFatDirectory( pVolume, pFileName);

		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	if( bSuccess )
		NotifyShell( pVolume, lpcszPathName, NULL,0,0, SHCNE_MKDIR  );
	return bSuccess;
}

BOOL FAT_RemoveDirectory( PVOL pVol, LPCTSTR lpcszPathName )
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL	 bSuccess = FALSE;

	PUCHAR		pFileName=(PUCHAR)lpcszPathName;

#ifdef UNDER_CE
	char	csName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszPathName,WideStrLen( lpcszPathName),
									csName, MAX_PATH, "_" ,FALSE);
	csName[iLen]=0;
	pFileName=csName;
#endif

	if( IsVolume(  pVolume ) && AccessTest( pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		bSuccess=DeleteFatDirectory( pVolume, pFileName);

		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	if( bSuccess ){

		NotifyShell( pVolume, lpcszPathName, NULL, 0 ,0, SHCNE_RMDIR );
	}
	return bSuccess;
}


BOOL FAT_MoveFile( PVOL pVol, LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName )
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL	 bSuccess = FALSE;

	PUCHAR		pOldFileName=(PUCHAR)lpcszOldFileName;
	PUCHAR		pNewFileName=(PUCHAR)lpcszNewFileName;

#ifdef UNDER_CE
	char	csOldName[MAX_PATH];
	char	csNewName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszOldFileName,WideStrLen( lpcszOldFileName),
									csOldName, MAX_PATH, "_" ,FALSE);
	csOldName[iLen]=0;
	pOldFileName=csOldName;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszNewFileName,WideStrLen( lpcszNewFileName),
									csNewName, MAX_PATH, "_" ,FALSE);
	csNewName[iLen]=0;
	pNewFileName=csNewName;
#endif

	if( IsVolume( pVolume ) && AccessTest( pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		bSuccess=FatFileSysMoveFile( pVolume, pOldFileName, pNewFileName );

		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	return bSuccess;
}

BOOL FAT_CopyFile( PVOL pVol, LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists )
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL	 bSuccess = FALSE;
	PUCHAR		pOldFileName=(PUCHAR)lpExistingFileName;
	PUCHAR		pNewFileName=(PUCHAR)lpNewFileName;

#ifdef UNDER_CE
	char	csOldName[MAX_PATH];
	char	csNewName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpExistingFileName,WideStrLen( lpExistingFileName),
									csOldName, MAX_PATH, "_" ,FALSE);
	csOldName[iLen]=0;
	pOldFileName=csOldName;

	iLen=WideCharToMultiByte( CP_ACP,0,lpNewFileName,WideStrLen( lpNewFileName),
									csNewName, MAX_PATH, "_" ,FALSE);
	csNewName[iLen]=0;
	pNewFileName=csNewName;
#endif

	if( IsVolume(  pVolume ) && AccessTest( pVolume ) ){
		DWORD dwCreateFlag;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( bFailIfExists )
			dwCreateFlag= CREATE_NEW;
		else
			dwCreateFlag= CREATE_ALWAYS;

		bSuccess=FatFileSysFileCopy( pVolume, pOldFileName, pNewFileName  , dwCreateFlag);

		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	return bSuccess;
}

BOOL FAT_WriteFile( PFILE pf, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	RETAILMSG( DEBUG_WRITE,(TEXT("FAT WriteFile entered %x\r\n"),pf));
	*lpdwNumWrite = 0;

	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {
			if( pFile->dwAccess & GENERIC_WRITE )   // lilin, add 2004-06-17
			    bSuccess=WriteFatFile( pFile,(char*)lpvBuffer, dwNumToWrite, lpdwNumWrite);
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	RETAILMSG( DEBUG_WRITE,(TEXT("FAT WriteFile leaved %x\r\n"),pf));
	return bSuccess;
}

BOOL FAT_SetEndOfFile( PFILE pf )
{  
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);

		if( IsFileHandle( pVolume,  pFile ) ) {
			if( pFile->dwAccess & GENERIC_WRITE )   // lilin, add 2004-06-17
			    bSuccess=FatSetEndOfFile( pFile );
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	return bSuccess;
}

#define DEBUG_FAT_FlushFileBuffers 1

BOOL FAT_FlushFileBuffers( PFILE pf )
{
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	if( IsVolume(  pVolume ) ){

		if( pFile->dwAttr == FILE_ATTR_VOLUME)
			return FALSE;

		EnterCriticalSection(&pVolume->csVolCrit);
		DEBUGMSG( DEBUG_FAT_FlushFileBuffers, (TEXT(" FAT_FlushFileBuffers entry.\r\n") ) );
		if( IsFileHandle( pVolume,  pFile ) ) {
			if( pFile->dwAccess & GENERIC_WRITE ){   // lilin, add 2004-06-17
				bSuccess=FlushFileBuffer( pFile );
				// 2004-09-15, lilin, add code
				if(  pFile->pVolume->bFlushed == FALSE )
					FlushFAT( pFile->pVolume );
				// 2004-09-15, lilin, add code-end
			}
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	return bSuccess;
}



BOOL FAT_DeleteAndRenameFile( PVOL pVol, LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName )
{ 
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;
	BOOL	 bSuccess = FALSE;

	PUCHAR		pOldFileName=(PUCHAR)lpcszOldFileName;
	PUCHAR		pNewFileName=(PUCHAR)lpcszNewFileName;

#ifdef UNDER_CE
	char	csOldName[MAX_PATH];
	char	csNewName[MAX_PATH];
	int		iLen;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszOldFileName,WideStrLen( lpcszOldFileName),
									csOldName, MAX_PATH, "_" ,FALSE);
	csOldName[iLen]=0;
	pOldFileName=csOldName;

	iLen=WideCharToMultiByte( CP_ACP,0,lpcszNewFileName,WideStrLen( lpcszNewFileName),
									csNewName, MAX_PATH, "_" ,FALSE);
	csNewName[iLen]=0;
	pNewFileName=csNewName;
#endif

	if( IsVolume(  pVolume ) && AccessTest( pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		if( FatFileSysFileCopy( pVolume, pOldFileName, pNewFileName, OPEN_EXISTING ) )
			bSuccess= FAT_DeleteFile( (PVOL)pVolume, lpcszOldFileName);

		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	if( bSuccess ){

		NotifyShell( pVolume, lpcszOldFileName, NULL, 0, 0, SHCNE_DELETE );
	}
	return bSuccess;
}

BOOL FAT_RegisterFileSystemFunction( PVOL pVol, SHELLFILECHANGEFUNC pft )
{  
	PVOLUME_FAT	pVolume= (PVOLUME_FAT) pVol;

	if( IsVolume(  pVolume ) ){

		EnterCriticalSection(&pVolume->csVolCrit);

		pVolume->FuncShellNotify= pft;

		LeaveCriticalSection(&pVolume->csVolCrit);
		return TRUE;
	}
	return FALSE;
}


void FAT_Notify( PVOL pVol, DWORD dwFlags )
{

}

#define DEBUG_FAT_DeviceIoControl 0
BOOL FAT_DeviceIoControl( PFILE pf, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpdwBytesReturned )
{ 
	PFILE_FAT	pFile=(PFILE_FAT)pf;
	PVOLUME_FAT	pVolume= pFile->pVolume;
	BOOL		bSuccess=FALSE;

	DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl entry.\r\n" ) );
	if( IsVolume(  pVolume ) ){		
		
		DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl 01.\r\n" ) );

		if( pFile->dwAttr != FILE_ATTR_VOLUME)
			return FALSE;
		
		DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl 02.\r\n" ) );
		switch (dwIoControlCode)
		{
			case  IOCTL_DISK_READONLY:
				//	设置磁盘为只读（内部使用）
				if( lpInBuf && nInBufSize == sizeof(DWORD) )
				{
					if( *((LPDWORD)lpInBuf) == READONLY_SET )
					{
						pVolume->fda.dwFlags |= DISK_INFO_FLAG_READONLY; // 先关闭写功能
						FlushFAT( pVolume );
					}
					else if( *((LPDWORD)lpInBuf) == READONLY_CLEAR )
					{						
						pVolume->fda.dwFlags &= ~DISK_INFO_FLAG_READONLY;
					}
					else
					{
						SetLastError( ERROR_INVALID_PARAMETER);
						return FALSE;
					}
				}
				return TRUE;
			case IOCTL_DISK_FORMAT_VOLUME:
				if( !AccessTest( pVolume ) )
					return FALSE;
		}

		// format disk now!!
		DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl 03.\r\n" ) );

		EnterCriticalSection(&pVolume->csVolCrit);
		if( pVolume->pFileListHead == 0 && pVolume->pFindListHead ==0 ){
		
			bSuccess=TRUE;
		
		}else if( pVolume->pFileListHead == pFile && pFile->pNext ==NULL ){
			
			bSuccess=TRUE;
		}
		
		DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl bSuccess=%d.\r\n", bSuccess ) );
		if( bSuccess ){
			DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl go1.\r\n" ) );
			bSuccess=FormatTheVolume( pVolume );
			if( bSuccess){
			
				pVolume->dwClusterSize= pVolume->fat_info.dwBytesPerSec * pVolume->fat_info.dwSecPerCluster;
				bSuccess=InitializeCaches(  pVolume );
			}

		}else{
			DEBUGMSG( DEBUG_FAT_DeviceIoControl, ( "FAT_DeviceIoControl go2.\r\n" ) );
			if( MessageBox( NULL, "该磁盘有打开的文件，你确定吗！？", "格式化", MB_OKCANCEL ) == IDOK )
			{
				FormatTheVolume( pVolume );
				MessageBox( NULL, "格式化完成，你需要重启机器", "", 0 );
				Thread_LockSchedule();
				while(1);
			}

			SetLastError( ERROR_ACCESS_DENIED );
		}
		LeaveCriticalSection(&pVolume->csVolCrit);
	}
	return bSuccess;
}

DWORD	FatAttributeToFileSystem(DWORD	dwFatAttr)
{
	DWORD	dwAttr=FILE_ATTRIBUTE_ARCHIVE;

	if( dwFatAttr& FILE_ATTR_READ_ONLY)
		dwAttr|= FILE_ATTRIBUTE_READONLY;
	if( dwFatAttr& FILE_ATTR_HIDDEN)
		dwAttr|= FILE_ATTRIBUTE_HIDDEN;
	if( dwFatAttr& FILE_ATTR_SYSTEM)
		dwAttr|= FILE_ATTRIBUTE_SYSTEM;
	if( dwFatAttr& FILE_ATTR_DIRECTORY)
		dwAttr|= FILE_ATTRIBUTE_DIRECTORY;
	//if( dwFatAttr& FILE_ATTR_VOLUME_ID)
	//	dwAttr|= ;
	if( dwFatAttr &FILE_ATTRIBUTE_DIRECTORY )
		dwAttr&=~FILE_ATTRIBUTE_ARCHIVE;

	return dwAttr;
}
DWORD	FileSysAttributeToFat(DWORD	dwSysAttr)
{
	DWORD	dwAttr=0;

	if( dwSysAttr& FILE_ATTRIBUTE_READONLY)
		dwAttr|=  FILE_ATTR_READ_ONLY;
	if( dwSysAttr& FILE_ATTRIBUTE_HIDDEN)
		dwAttr|= FILE_ATTR_HIDDEN;
	if( dwSysAttr& FILE_ATTRIBUTE_SYSTEM)
		dwAttr|= FILE_ATTR_SYSTEM;
	if( dwSysAttr& FILE_ATTRIBUTE_DIRECTORY)
		dwAttr|= FILE_ATTR_DIRECTORY;

	return dwAttr;
}


BOOL WINAPI  DllEntry(HINSTANCE DllInstance, int Reason, LPVOID Reserved)
{
    switch(Reason) {
        case DLL_PROCESS_ATTACH:
            RETAILMSG(1, (TEXT("FATSYS: DLL_PROCESS_DETACH\r\n")));
			InitializeFATSys( );
			break;
   
        case DLL_PROCESS_DETACH:
            RETAILMSG(1, (TEXT("FATSYS: DLL_PROCESS_DETACH\r\n")));
            break;
    }
	return TRUE;
}

