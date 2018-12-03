/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：内核romfs辅助文件
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
#include <romheader.h>
#include <coresrv.h>

//extern TCHAR        FolderName[];
extern PROMVOLUME	pVolumeList;

//The pointer pTOC  contain all information of modules and files.
extern  ROMHDR *const pTOC ;


BOOL HELP_RegisterVolume( PROMVOLUME pVolume ) 
{
	DWORD			dwFolderLen=0; 
//	TCHAR			wFolder[20];
	HVOL			VolumeIdentifer=0;


	//VolumeIdentifer=FSDMGR_RegisterVolume( pVolume->hDevice,FolderName, (DWORD) pVolume );

	//if( VolumeIdentifer==0) {
	//	RETAILMSG(ZONE_ERROR, (TEXT("romfs: registervolume fail!\r\n")));
	//	free( pVolume);
	//	return FALSE;
	//}
	//RETAILMSG(ROM_HELP, (TEXT("romfs: registervolume success!\r\n")));

	pVolume->hVolume=VolumeIdentifer;

	EnterCriticalSection( &Crit_Volume);
	pVolume->next=pVolumeList;
	pVolumeList=pVolume;
	LeaveCriticalSection( &Crit_Volume);
	return TRUE;
}


/*void  HELP_DeviceCallBack(DWORD context ,DWORD dwState )
{
	PROMVOLUME  pVolume=(PROMVOLUME)context;

	if( IsVolume(pVolume) ){

		if( dwState==HELP_MEDIA_OUT){

			v_FSDMGR_DeregisterVolume( pVolume->hVolume );

		}
	}
	HELP_GetNameAndRegister(pVolume);
	RETAILMSG(ZONE_MOUNT, (TEXT("CDMFSD: HELP_DeviceCallBack enterd %x \r\n"),context));
}*/

PROMVOLUME CreateVolume( HDSK hDisk )
{
	PROMVOLUME pVol=(PROMVOLUME)KHeap_Alloc( sizeof( ROMVOLUME) );
	if( pVol==NULL) 
		return NULL;

	memset(pVol, 0, sizeof(ROMVOLUME));

	pVol->hDevice=hDisk;
	InitializeCriticalSection(  &(pVol->cs_Volume) );
	return pVol;
}

BOOL ROM_IsVolume( PROMVOLUME pVolume )
{
	PROMVOLUME pVol=pVolumeList;
	BOOL       status=FALSE;

	EnterCriticalSection( &Crit_Volume );
	while( pVol )
	{
		if( pVol == pVolume ){
			status=TRUE;
			break;
		}
		pVol=pVol->next;
	}
	LeaveCriticalSection( &Crit_Volume );
	if( !status )
		SetLastError( ERROR_INVALID_HANDLE );
	return status;
}

BOOL  IsFileOpened( PROMVOLUME pVolume, LPCTSTR pwsName )
{
	PROMFILE pFile=pFileList;
   
	while( pFile) {

		if( stricmp( pFile->cFileName,pwsName )==0 ) { // the file has been opened.
       
			if( pFile->dwShareMode == FILE_NO_SHARE )
				return FALSE;
			break;
		}
		pFile=pFile->next;
	}
	return TRUE;
}

void  FormatFilePath(TCHAR * pcwFile )
{
	while( *pcwFile) {

		if( *pcwFile=='/' ) 
			*pcwFile='\\';
		pcwFile++;
	}
}

TCHAR *  GetFileName (TCHAR *  pcwFolder)
{
	WORD		wPathLong=strlen(pcwFolder);
	WORD		i;

	while( *pcwFolder )	     {  pcwFolder++;  };

	for( i=0;i<wPathLong && *pcwFolder !='\\'; i++)
			pcwFolder--;

	++pcwFolder;

	return pcwFolder;
}

BOOL   IsDesiredFile( TCHAR *  pcwPath, LPTSTR  pcwFileName)
{
	TCHAR *  pcwWild, * pAll ,* pW;
//	LPTSTR  pcwFileName=(LPTSTR)pFile->cFileName;
	TCHAR    pcwAllFile[]=TEXT("*.*");
	WORD     i;
	BOOL	 bReturn = FALSE;

	RETAILMSG(ZONE_TEST, (TEXT("IsDesiredFile: <\r\n")));
	//RETAILMSG(ROM_HELP, (TEXT("IsDesiredFile: 11\r\n")));

	pW = pcwWild = GetFileName(pcwPath);

	//RETAILMSG(ROM_HELP, (TEXT("IsDesiredFile: 12\r\n")));

	if( pcwFileName[0]==0 ) {
		bReturn = FALSE;
		goto IsReturn;
	}

	//RETAILMSG(ZONE_SEARCH,(TEXT("CDMFSD: Is DesiredFile entered %s <=> %s \r\n"),pW,pcwFileName));

    // if the path is "*" or "*.*"  return TRUE;
    if( pW[0]=='*' )
	{
		if( pW[1]==0 ){ 
			bReturn = TRUE;
			goto IsReturn;
		}
		else if( pW[1]=='.' ) 
		{
			if( pW[2]=='*' ){
				bReturn = TRUE;
				goto IsReturn;
			}
			else {  // ex. *.exe ,*.txt ,etc.
				pAll=pcwFileName;
				while(*pAll !='.'){
					pAll++;
					if( *pAll==0 ) // this file has no extend name.
					{
						bReturn = FALSE;
						goto IsReturn;
					}
				}
				pAll++;
				//RETAILMSG(ZONE_SEARCH,(TEXT("CDMFSD: Is DesiredFile Extend %s >< %s \r\n"),pAll,pcwPath));
				// now check whether the extend file name is identical.
				pW+=2;
				for( i=0; i< FILE_EXTEND_NAME_LEN && *pAll; i++ ){
					if( * pW !=*pAll )
						break;
					pW++;
					pAll++;
				}
				if( * pAll==0 && *pW==0 ) 
				{
					bReturn = TRUE;
					goto IsReturn;
				}
				else {
					bReturn = FALSE;
					goto IsReturn;
				}
			}
		}
	}
	//RETAILMSG(ROM_HELP, (TEXT("IsDesiredFile: 13\r\n")));

    // now  we only support *.*  and accurate file name.
	pW=pcwWild;
	pAll=pcwFileName;

	//RETAILMSG(ZONE_SEARCH,(TEXT("CDMFSD: Is DesiredFile %s  %s \r\n"),pW,pAll));
    // now judging whether the file name is identical.
    for( i=0; *pW; i++ ,pW++, pAll++)
	{
		if( *pW != *pAll )
			break;
	}
	if( i >= strlen( pcwWild ) )
	{
		if( *pAll == 0) {
			bReturn = TRUE;
			goto IsReturn;
		}
	}

IsReturn: 

	if(bReturn == TRUE)
		RETAILMSG(ZONE_TEST,(TEXT("IsDesiredFile: > success : %x\r\n"),bReturn));
	else
		RETAILMSG(ZONE_TEST,(TEXT("IsDesiredFile: > fail : %x\r\n"),bReturn));

	return bReturn;
}


BOOL ROM_FindFile(LPCTSTR pFileName, UINT *index, UINT *FileMode)
{
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	UINT			i;

	RETAILMSG(ROM_HELP, (TEXT("FindFile: < %s\r\n"), pFileName)); 
	
	//RETAILMSG(ROM_HELP, (TEXT("HELP_FindFirstFile:  &pTOC:%x, pTOC:%x!\r\n"), &pTOC, pTOC)); 

	pTocentry = (LPTOCentry)pTOC->ulModOffset;

/*	RETAILMSG(ROM_HELP, (TEXT("Tocentry: ...........\r\n")));
	RETAILMSG(ROM_HELP, (TEXT("dwFileAttributes:%x\r\n"), pTocentry->dwFileAttributes));
	RETAILMSG(ROM_HELP, (TEXT("nFileSize:%x\r\n"), pTocentry->nFileSize));
	RETAILMSG(ROM_HELP, (TEXT("lpszFileName:%x\r\n"), pTocentry->lpszFileName));
	RETAILMSG(ROM_HELP, (TEXT("ulExeOffset:%x\r\n"), pTocentry->ulExeOffset));
	RETAILMSG(ROM_HELP, (TEXT("ulObjOffset:%x\r\n"), pTocentry->ulObjOffset));
	RETAILMSG(ROM_HELP, (TEXT("ulLoadOffset:%x\r\n"), pTocentry->ulLoadOffset));
*/	
	for(i = 0; i < pTOC->nummods; i++)
	{
		//pTocentry += i;
		RETAILMSG(ZONE_TEST, (TEXT("lpszFileName:%s\r\n"), pTocentry->lpszFileName));
		if(IsDesiredFile( (TCHAR *)pFileName, pTocentry->lpszFileName)){
			*index = i;
			*FileMode = FILE_MODE_MOD;
			RETAILMSG(ROM_HELP, (TEXT("FindFile: > mod ok\r\n"))); 
			return TRUE;
		}
		pTocentry ++;
	}

	RETAILMSG(ROM_HELP, (TEXT("FindFile: 11\r\n"))); 

	RETAILMSG(ROM_HELP, (TEXT("FindFile: numfiles-%x\r\n"),pTOC->numfiles)); 

	pFileentry = (LPFILESentry)pTOC->ulFileOffset;
	for(i = 0; i < pTOC->numfiles; i++)
	{
		//pFileentry += i;
		RETAILMSG(ROM_HELP, (TEXT("lpszFileName:%s\r\n"), pFileentry->lpszFileName));
		if(IsDesiredFile( (TCHAR *)pFileName, pFileentry->lpszFileName)){
			*index = i;
			*FileMode = FILE_MODE_FILE;
			RETAILMSG(ROM_HELP, (TEXT("FindFile: > file ok\r\n"))); 
			return TRUE;
		}
		pFileentry ++;
	}
	RETAILMSG(ZONE_ERROR, (TEXT("No romFile: %s>\r\n"), pFileName)); 
	return FALSE;
}

HANDLE  HELP_FindFirstFile(PVOL pVolume, LPCTSTR pwsFileSpec, FILE_FIND_DATA * pfd , UINT flag)
{
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	PROMSEARCH		pSearch;
	UINT			index, FileMode;

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFirstFile:  enter!\r\n"))); 

	FormatFilePath( (TCHAR *)pwsFileSpec);

	pfd->cFileName[0] = 0;

    pSearch =KHeap_Alloc( sizeof( ROMSEARCH ));
	if( pSearch==NULL )
		return  INVALID_HANDLE_VALUE;
	memset(pSearch, 0, sizeof(ROMSEARCH));
		
	

	if(!ROM_FindFile(pwsFileSpec, &index, &FileMode))
		return INVALID_HANDLE_VALUE;

	if(FileMode == FILE_MODE_MOD)
	{
		pTocentry = (LPTOCentry)pTOC->ulModOffset;
		pTocentry += index;
//		pfd->dwFileSize = pTocentry->nFileSize;
		pfd->nFileSizeLow = pTocentry->nFileSize;
		pfd->nFileSizeHigh = 0;
		pfd->dwOID		   = 0;	

		pfd->dwFileAttributes = pTocentry->dwFileAttributes;
		memcpy(&(pfd->ftCreationTime), &pTocentry->ftTime,8);
		memcpy(&(pfd->ftLastAccessTime), &pfd->ftCreationTime,8);
		memcpy(&(pfd->ftLastWriteTime), &pfd->ftCreationTime,8);

		if(flag == FIND_CREATE_CALL)
		{
			((PROMFILE)pfd)->dwFileBase = pTocentry->ulLoadOffset;		
		}
		pSearch->pFileBase = pTocentry->ulLoadOffset;

		//strcpy(pfd->cFileName, pTocentry->lpszFileName);
		
		//FormatFileName( pFile->cFileName);
	}else if(FileMode == FILE_MODE_FILE)
	{
		pFileentry = (LPFILESentry)pTOC->ulFileOffset;
		pFileentry += index;
		//pfd->dwFileSize = pFileentry->nFileSize;
		pfd->nFileSizeLow = pFileentry->nRealFileSize;
		pfd->nFileSizeHigh = 0;
		pfd->dwOID		   = 0;	

		pfd->dwFileAttributes = pFileentry->dwFileAttributes;
		memcpy(&(pfd->ftCreationTime), &pFileentry->ftTime,8);
		memcpy(&(pfd->ftLastAccessTime), &pfd->ftCreationTime,8);
		memcpy(&(pfd->ftLastWriteTime), &pfd->ftCreationTime,8);
		
		if(flag == FIND_CREATE_CALL)
		{
			((PROMFILE)pfd)->dwFileBase = pFileentry->ulLoadOffset;			
		}
		pSearch->pFileBase = pFileentry->ulLoadOffset;			

		//strcpy(pfd->cFileName, pFileentry->lpszFileName);
	}

	

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFirstFile:  leave!\r\n"))); 	

	return pSearch;
}


BOOL  HELP_IsFile( PROMFILE pFile  )
{
	PROMFILE  pF	=pFileList;
	BOOL      status=FALSE;

	EnterCriticalSection( &Crit_File );
	while( pF )
	{
		if( pF == pFile ){
			status=TRUE;
			break;
		}
		pF=pF->next;
	}
	LeaveCriticalSection( &Crit_File );
	if (!status )
		SetLastError( ERROR_INVALID_HANDLE);
	return status;
}
BOOL  HELP_IsSearch( PROMSEARCH pSearch  )
{
	PROMSEARCH  pS=pSearchList;
	BOOL       status=FALSE;

	EnterCriticalSection( &Crit_Search );
	while( pS )
	{
		if( pS == pSearch ){
			status=TRUE;
			break;
		}
		pS=pS->next;
	}
	LeaveCriticalSection( &Crit_Search );
	if( !status )
		SetLastError( ERROR_INVALID_HANDLE );

	return status;
}
