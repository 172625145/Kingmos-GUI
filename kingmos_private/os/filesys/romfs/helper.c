/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：巨果.Kingmos rom file sys
版本号：2.0.0
开发时期：2000
作者：周兵
修改记录：
	李林，2004-10-09 - HELP_FindFirstFile 增加对指针的释放
******************************************************/



#include <ewindows.h>
#include <efsdmgr.h>
#include <epalloc.h>
#include <edevice.h>
#include <Eassert.h>
#include <romfs.h>
#include <romheader.h>

extern TCHAR        FolderName[];
extern PROMVOLUME	pVolumeList;

//The pointer pTOC  contain all information of modules and files.
extern  ROMHDR *const pTOC ;
BOOL Help_FindNextFile(LPCTSTR pFileName, UINT *index, UINT *FileMode,LPCTSTR pPreFileName);

BOOL HELP_RegisterVolume( PROMVOLUME pVolume ) 
{
	DWORD			dwFolderLen=0; 
	TCHAR			wFolder[20];
	HVOL			VolumeIdentifer=0;


	VolumeIdentifer=FSDMGR_RegisterVolume( pVolume->hDevice,FolderName, (DWORD) pVolume );

	if( VolumeIdentifer==0) {
		RETAILMSG(ZONE_ERROR, (TEXT("romfs: registervolume fail!\r\n")));
		free( pVolume);
		return FALSE;
	}
	RETAILMSG(ROM_HELP, (TEXT("romfs: registervolume success!\r\n")));

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

	if( HELP_IsVolume(pVolume) ){

		if( dwState==HELP_MEDIA_OUT){

			v_FSDMGR_DeregisterVolume( pVolume->hVolume );

		}
	}
	HELP_GetNameAndRegister(pVolume);
	RETAILMSG(ZONE_MOUNT, (TEXT("CDMFSD: HELP_DeviceCallBack enterd %x \r\n"),context));
}*/

PROMVOLUME CreateVolume( HDSK hDisk )
{
	PROMVOLUME pVol=(PROMVOLUME)malloc(sizeof( ROMVOLUME) );
	if( pVol==NULL) 
		return NULL;

	memset(pVol, 0, sizeof(ROMVOLUME));

	pVol->hDevice=hDisk;
	InitializeCriticalSection(  &(pVol->cs_Volume) );
	return pVol;
}

BOOL HELP_IsVolume( PROMVOLUME pVolume )
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


TCHAR *  GetFileName ( TCHAR *  pcwFolder)
{
	WORD		wPathLong=strlen(pcwFolder);
	WORD		i;

	//changed by zb...2003-07-12
//	while( *pcwFolder )	     {  pcwFolder++;  };
	pcwFolder += wPathLong;

	for( i=0;i<wPathLong && *pcwFolder !='\\'; i++)
			pcwFolder--;

	if( *pcwFolder =='\\' )
	   ++pcwFolder;

	return pcwFolder;
}

/* // LN , 2003-07-04, replace with FileNameCompare
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
*/


//BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen )
BOOL HELP_FindFile(LPCTSTR pFileName, UINT *index, UINT *FileMode)
{
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	UINT			i;
	LPCTSTR lpFN = (LPCTSTR)GetFileName( (TCHAR*)pFileName);
	UINT uiFNSize = strlen( lpFN );

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: < %s\r\n"), pFileName)); 
	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: filename: %s,filelength:%x\r\n"), lpFN, uiFNSize)); 
	
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
	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: 00\r\n"))); 

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: numXipfiles-%x\r\n"),pTOC->nummods)); 


	for(i = 0; i < pTOC->nummods; i++)
	{
		//pTocentry += i;
		RETAILMSG(ZONE_TEST, (TEXT("lpszFileName:%s\r\n"), pTocentry->lpszFileName));
		//if(IsDesiredFile( (TCHAR *)pFileName, pTocentry->lpszFileName)) //LN
		if( FileNameCompare( lpFN, uiFNSize, pTocentry->lpszFileName, 0 ) )
		{
			*index = i;
			*FileMode = FILE_MODE_MOD;
			RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: > mod ok\r\n"))); 
			return TRUE;
		}
		pTocentry ++;
	}

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: 11\r\n"))); 

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: numfiles-%x\r\n"),pTOC->numfiles)); 

	pFileentry = (LPFILESentry)pTOC->ulFileOffset;
	for(i = 0; i < pTOC->numfiles; i++)
	{
		//pFileentry += i;
		RETAILMSG(ROM_HELP, (TEXT("lpszFileName:%s\r\n"), pFileentry->lpszFileName));
		//if(IsDesiredFile( (TCHAR *)pFileName, pFileentry->lpszFileName))  //LN
        if( FileNameCompare( lpFN, uiFNSize, pFileentry->lpszFileName, 0 ) )
		{
			*index = i;
			*FileMode = FILE_MODE_FILE;
			RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: > file ok\r\n"))); 
			return TRUE;
		}
		pFileentry ++;
	}
	RETAILMSG(ZONE_ERROR, (TEXT("No romFile: %s>\r\n"), pFileName)); 
	return FALSE;
}

BOOL Help_FindNextFile(LPCTSTR pFileName, UINT *index, UINT *FileMode,LPCTSTR pPreFileName)
{
	UINT			PreIndex, PreFileMode;
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	UINT			i;
	BOOL			bRet;
	LPCTSTR lpFN = (LPCTSTR)GetFileName( (TCHAR*)pFileName);
	UINT uiFNSize = strlen( lpFN );

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: < %s\r\n"), pFileName)); 
	
	bRet = HELP_FindFile(pPreFileName, &PreIndex, &PreFileMode);
	
	ASSERT(bRet);
	
	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: PreIndex:%x, PreFileMode:%x \r\n"), PreIndex,PreFileMode)); 

	if(PreFileMode == FILE_MODE_FILE){
		PreIndex += 1;	//follow don't +1;
	}


	if(PreFileMode == FILE_MODE_MOD){

		pTocentry = (LPTOCentry)pTOC->ulModOffset;
		
		RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: 0000000\r\n"))); 

		for(i = PreIndex + 1; i < pTOC->nummods; i++)
		{
			//pTocentry += i;
			RETAILMSG(ZONE_TEST, (TEXT("lpszFileName:%s\r\n"), pTocentry->lpszFileName));
			//if(IsDesiredFile( (TCHAR *)pFileName, pTocentry->lpszFileName)) //LN
			if( FileNameCompare( lpFN, uiFNSize, pTocentry->lpszFileName, 0 ) )
			{
				*index = i;
				*FileMode = FILE_MODE_MOD;
				RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: > mod ok\r\n"))); 
				return TRUE;
			}
			pTocentry ++;
		}
		

		PreFileMode = FILE_MODE_FILE;
		PreIndex = 0;
		//return FALSE;
	}


	if(PreFileMode == FILE_MODE_FILE){
		
		RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: 11\r\n"))); 
		RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: numfiles-%x\r\n"),pTOC->numfiles)); 
		
		pFileentry = (LPFILESentry)pTOC->ulFileOffset;
		for(i = PreIndex; i < pTOC->numfiles; i++)
		{
			//pFileentry += i;
			RETAILMSG(ROM_HELP, (TEXT("lpszFileName:%s\r\n"), pFileentry->lpszFileName));
			//if(IsDesiredFile( (TCHAR *)pFileName, pFileentry->lpszFileName))  //LN
			if( FileNameCompare( lpFN, uiFNSize, pFileentry->lpszFileName, 0 ) )
			{
				*index = i;
				*FileMode = FILE_MODE_FILE;
				RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: > file ok\r\n"))); 
				return TRUE;
			}
			pFileentry ++;
		}
	}
	RETAILMSG(ZONE_ERROR, (TEXT("No romFile: %s>\r\n"), pFileName)); 
	return FALSE;
}

//HANDLE  HELP_FindFirstFile(PVOL pVolume, LPCTSTR pwsFileSpec, FILE_FIND_DATA * pfd , UINT flag)
HANDLE  HELP_FindFirstFile(PVOL pVolume, LPCTSTR pwsFileSpec, FILE_FIND_DATA * pfd , UINT flag,
						   LPCTSTR pwsPreFile)
{
	LPTOCentry		pTocentry;
	LPFILESentry	pFileentry;
	PROMSEARCH		pSearch = NULL;
	UINT			index, FileMode;

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFirstFile:  enter!\r\n"))); 

	FormatFilePath( (TCHAR *)pwsFileSpec);

	pfd->cFileName[0] = 0;

    pSearch =malloc( sizeof( ROMSEARCH ));
	if( pSearch==NULL )
		goto _error_return;//  INVALID_HANDLE_VALUE;
	memset(pSearch, 0, sizeof(ROMSEARCH));
		
	//Zb changed 2003-07-12...
//	if(!HELP_FindFile(pwsFileSpec, &index, &FileMode))
//		return INVALID_HANDLE_VALUE;	
	if(pwsPreFile != NULL){
		RETAILMSG(ROM_HELP, (TEXT("Help_FindNextFile: pwsPreFile:%s\r\n"),pwsPreFile)); 

		if(!Help_FindNextFile(pwsFileSpec, &index, &FileMode,pwsPreFile))
			goto _error_return;//return INVALID_HANDLE_VALUE;
	}else{
		RETAILMSG(ROM_HELP, (TEXT("HELP_FindFile: \r\n"))); 
		if(!HELP_FindFile(pwsFileSpec, &index, &FileMode))
			goto _error_return;//return INVALID_HANDLE_VALUE;
	}
	//Zb changed 2003-07-12...


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

		//Zb changed 2003-07-12...Add the line for fix bug
		strcpy(pfd->cFileName, pTocentry->lpszFileName);				
		strcpy(pSearch->wPreFindedFile, pTocentry->lpszFileName);		

		//FormatFileName( pFile->cFileName);

	}
	else if(FileMode == FILE_MODE_FILE)
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

		//Zb changed 2003-07-12...Add the line for fix bug
		strcpy(pfd->cFileName, pFileentry->lpszFileName);
		strcpy(pSearch->wPreFindedFile, pFileentry->lpszFileName);		

		//FormatFileName( pFile->cFileName);
	}	

	RETAILMSG(ROM_HELP, (TEXT("HELP_FindFirstFile:  leave!\r\n"))); 	

	return pSearch;

	//2004-10-09 - 增加对指针的释放
_error_return:
	if( pSearch )
	{
		free( pSearch );
	}
	return INVALID_HANDLE_VALUE;
	//

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
