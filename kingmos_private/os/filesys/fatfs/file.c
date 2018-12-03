/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：FAT文件系统中存取文件的函数。
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙
修改记录：
	6. 2005-03-08, ReadFatFile & WriteFatFile 的 ->dwClusterCurrent的合法性检查
	5. 2004-12-22, 修改 CreateFatFile
	4. 2004-09-15, lilin, add code for FlushFileBuffer
    3. 2004-08-09, CopyFile 为新文件分配空间和 没有释放文件句柄
	2. 2004-06-21, 修改对 dwLastValidClus 的初始化
	1. 2003-9-19. 新增了875行，原来的会有空间泄露。
******************************************************/
#include "fatfs.h"
#include "fatfmt.h"

static BOOL	SetFileSize( PVOLUME_FAT pVolFat, PFILE_FAT  pFileFat, DWORD dwSize ,BOOL bChanged );
static BOOL CacheFileData( PFILE_FAT pFile, PVOLUME_FAT pFatVol, DWORD dwCluster);
static int FoundFileInHandleList(PVOLUME_FAT pVol,  char * pszFileName, DWORD dwAccess, DWORD dwShareMode,DWORD dwCreate ,PFILE_FAT pFileFound);
static HANDLE CreateRootFile(PVOLUME_FAT  pVol );

HANDLE CreateFatFile( PVOLUME_FAT pVol, char * lpcszFileName, DWORD dwAccess, 
					  DWORD dwShareMode, DWORD dwCreate,DWORD dwFlagsAndAttributes,BOOL bFile )
{
	FILE_FAT	file_fat;
	FILE_FIND_DATA	 find_data;
	PFIND_RESULT	 pFind;
	HANDLE			 hFileCreated=INVALID_HANDLE_VALUE;
	BOOL			 bSuccess=TRUE;
	int				 iFoundInList;

	SetLastError( ERROR_SUCCESS );

	if( strcmp(lpcszFileName, ROOT_NAME ) ==0 ){

		return CreateRootFile( pVol );
	}
	iFoundInList= FoundFileInHandleList( pVol, lpcszFileName, dwAccess, dwShareMode,dwCreate,&file_fat );

	if( iFoundInList ==1 ){
	
		RETAILMSG( DEBUG_FILE,(TEXT("FAT CreatFatFile  1111111111\r\n")));
		SetLastError(ERROR_ACCESS_DENIED);
		bSuccess=TRUE;

	}else if( iFoundInList ==-1 ){

		RETAILMSG( DEBUG_FILE,(TEXT("FAT CreatFatFile  2222222222\r\n")));
		SetLastError( ERROR_SHARING_VIOLATION);
		return INVALID_HANDLE_VALUE;

	}else if( (pFind =(PFIND_RESULT)FindFile(  pVol, lpcszFileName, &find_data)) !=INVALID_HANDLE_VALUE ){
		//the file is existed.

		//SetLastError(ERROR_FILE_EXISTS); //find file set it
		//检查参数的有效性
		switch( dwCreate ){
			
		case OPEN_EXISTING:
			break;
		case CREATE_ALWAYS:
			// 2004-12-22, remove by lilin, 移到后面
			//SetFileAttributesByHandle( pVol,&file_fat, dwFlagsAndAttributes );
			//
			break;
		case OPEN_ALWAYS  :
			break;
		case TRUNCATE_EXISTING:
			/*
			if( (dwAccess & GENERIC_WRITE) == GENERIC_WRITE){
				// 2004-12-22, remove by lilin, 移到后面
				//if( file_fat.dwSize !=0 )
				//	SetFileSize( pVol, &file_fat,0 ,TRUE);
				//
			}else{
				SetLastError( ERROR_INVALID_PARAMETER);
				bSuccess= FALSE;
			}
			*/
			if( (dwAccess & GENERIC_WRITE) != GENERIC_WRITE){
				SetLastError( ERROR_INVALID_PARAMETER);
				bSuccess= FALSE;
			}
			break;
		case CREATE_NEW:
			//SetLastError( ERROR_FILE_EXIST);
			bSuccess= FALSE;
			//2005-02-25, add by lilin
			dwFlagsAndAttributes &= ~FILE_ATTRIBUTE_READONLY;
			//
			break;
		default:
			SetLastError( ERROR_INVALID_PARAMETER);
			bSuccess= FALSE;
			break;
		}
		if( (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && bFile ){

			SetLastError( ERROR_FILE_NOT_FOUND );
			//return INVALID_HANDLE_VALUE;
			bSuccess= FALSE;
		}

		if( bSuccess ){

			file_fat.dwStartCluster = find_data.dwOID;
			///////////***********************************
			file_fat.dwSize			= find_data.nFileSizeLow;
			file_fat.dwParentStart	= pFind->dwParentStart;
			file_fat.dwClusterParent= pFind->dwCluster;

			file_fat.dwClusterOffset= pFind->dwOffset;

			if( pFind->dwOffset>= 1 )
				file_fat.dwClusterOffset= pFind->dwOffset-1;

			file_fat.dwAccess   = dwAccess;
			file_fat.dwShareMode= dwShareMode;
			file_fat.dwPointer  =0;
			//file_fat.dwClusIndex=0;
			file_fat.dwLastValidClus = file_fat.dwStartCluster;
			file_fat.dwClusterCurrent= file_fat.dwStartCluster;

			file_fat.dwAttr     =find_data.dwFileAttributes;
			file_fat.dwId		=GetTickCount( );

			file_fat.dwLongNameCluster= pFind->dwLongNameCluster;
			file_fat.dwLongNameOffset = pFind->dwLongNameOffset;

			file_fat.ftCreate.dwHighDateTime =   find_data.ftCreationTime.dwHighDateTime;
			file_fat.ftCreate.dwLowDateTime =   find_data.ftCreationTime.dwLowDateTime;

			file_fat.ftLastAccess.dwHighDateTime =   find_data.ftLastAccessTime.dwHighDateTime;
			file_fat.ftLastAccess.dwLowDateTime =   find_data.ftLastAccessTime.dwLowDateTime;

			file_fat.ftLastWrite .dwHighDateTime =   find_data.ftLastWriteTime.dwHighDateTime;
			file_fat.ftLastWrite .dwLowDateTime =   find_data.ftLastWriteTime.dwLowDateTime;

			strcpy(file_fat.csName,lpcszFileName);
			//2004-12-22， 增加代码
			switch( dwCreate ){
			case CREATE_ALWAYS:
				SetFileAttributesByHandle( pVol,&file_fat, dwFlagsAndAttributes );
				break;
			case TRUNCATE_EXISTING:
				if( file_fat.dwSize !=0 )
					SetFileSize( pVol, &file_fat,0 ,TRUE);
			}
			//
		}
		DeleteSearchHandle( pVol, pFind );
	}else{

		if( dwCreate ==CREATE_ALWAYS ||  dwCreate ==CREATE_NEW || dwCreate ==OPEN_ALWAYS ){

			APPEND_RECORD	append;

			append.bFile=TRUE;
			append.dwAttr=dwFlagsAndAttributes;
			append.dwFileStartClus=-1; //这里和CreateDirectory时一定要将这个数设为－1。

			SetLastError(  ERROR_INVALID_FUNCTION );

			if( !AddNameRec( pVol, ( char *)lpcszFileName,&append, &file_fat) )
				bSuccess= FALSE;

		}else{

			SetLastError( ERROR_FILE_NOT_FOUND );
			bSuccess= FALSE;
		}
	}
	if( bSuccess ){

		PFILE_FAT	pFatFile= ( PFILE_FAT )malloc( sizeof( FILE_FAT) );

		if( pFatFile ){

			memcpy( pFatFile, &file_fat, sizeof( FILE_FAT) );
			
			pFatFile->pFileCache=(char*)malloc(pVol->dwClusterSize);
			pFatFile->bCacheNeedFlush=FALSE;

			if( pFatFile->pFileCache ){

				pFatFile->dwCachedCluster=-1;
				pFatFile->pVolume= pVol;

				pFatFile->pNext=pVol->pFileListHead;
				pVol->pFileListHead=pFatFile;

				pFatFile->dwPointer  =0;
				//2004-06-21 lilin-remove this line
				//pFatFile->dwLastValidClus = pFatFile->dwStartCluster;
				//
				//2004-06-21 lilin-add this line
				pFatFile->dwLastValidClus = FileGetLastValidCluster( pVol, pFatFile->dwStartCluster );
				//

				pFatFile->dwClusterCurrent= pFatFile->dwStartCluster;

				pFatFile->dwShareMode =dwShareMode;
				pFatFile->dwAccess	=dwAccess;


				hFileCreated=pFatFile;
			}else{
				free( pFatFile);
			}
		}
	}
	return hFileCreated;
}

HANDLE CreateRootFile(PVOLUME_FAT  pVol )
{
	PFILE_FAT	pFile= pVol->pFileListHead;
	PFILE_FAT	pFound=NULL;
	BOOL	    bSuccess=TRUE;
	DWORD		dwNotShared=0;

	while( pFile ){

		if( strcmp(pFile->csName, ROOT_NAME)==0  ){

			pFound=pFile;
			break;
		}
		pFile=pFile->pNext;
	}
	if( pFound )
		return pFound;

	pFound=( PFILE_FAT )malloc( sizeof( FILE_FAT) );
	if( pFound ){

		memset( pFound,0,sizeof(FILE_FAT));
		pFound->pVolume=pVol;
		pFound->dwAttr = FILE_ATTR_VOLUME;
		pFound->pNext=pVol->pFileListHead;
		pVol->pFileListHead=pFound;
	}
	return pFound ? pFound: INVALID_HANDLE_VALUE;
}


int FoundFileInHandleList(PVOLUME_FAT pVol,  char * pszFileName, DWORD dwAccess, DWORD dwShareMode,DWORD dwCreate ,PFILE_FAT pFileFound) 
{
	PFILE_FAT	pFile= pVol->pFileListHead;
	PFILE_FAT	pFound=NULL;
	BOOL	    bSuccess=TRUE;
	DWORD		dwShared=0xFFFFFFFF;


	while( pFile ){
		
		if( stricmp(pFile->csName, pszFileName)==0  ){

			dwShared &= pFile->dwShareMode;
			pFound=pFile;
		} //一个文件可能会被打开多次，所以我们要找到所有已经打开的同名文件，	魏海龙
		pFile=pFile->pNext;
	}
	if( !pFound )
		return 0;

	memcpy(pFileFound,pFound,sizeof( FILE_FAT ));

	switch( dwCreate ){
		
	case OPEN_EXISTING:
		break;
	case CREATE_ALWAYS:
		break;
	case OPEN_ALWAYS  :
		break;
	/*case TRUNCATE_EXISTING:
		if( dwShareMode & GENERIC_WRITE == GENERIC_WRITE){

			if( pFileFound->dwSize !=0 )
				SetFileSize( pVol, pFileFound,0 );
		}else{

			SetLastError( ERROR_INVALID_PARAMETER);
			bSuccess= FALSE;
		}
		break;*/
	case CREATE_NEW:
		//SetLastError( ERROR_FILE_EXIST);
		bSuccess= FALSE;
		break;
	default:
		SetLastError( ERROR_INVALID_PARAMETER);
		bSuccess= FALSE;
		break;
	}

	if(bSuccess ){
		
		if( dwShared & dwShareMode  ){

			if( dwAccess & GENERIC_READ ){

				if( !( dwShared & FILE_SHARE_READ ) ){

					return -1;
				}
			}
			if( dwAccess & GENERIC_WRITE ){

				if( !( dwShared & FILE_SHARE_WRITE ) ){

					return -1;
				}
			}
			return 1;
		}
	}
	return -1;
}

BOOL	DeleteFileHandle( PVOLUME_FAT pVol, PFILE_FAT	pFileDel ,BOOL  bClosed )
{
	PFILE_FAT	pFile= pVol->pFileListHead;
	BOOL       bStatus=FALSE;

	if( pFile == pFileDel ){

		bStatus=TRUE;
		pVol->pFileListHead= pFile->pNext;

	}else{

		while( pFile->pNext ){

			if( pFile->pNext == pFileDel ){

				bStatus=TRUE;
				pFile->pNext=pFile->pNext->pNext;
				break;
			}
			pFile=pFile->pNext;
		}
	}
	if( !bStatus ){

		SetLastError( ERROR_INVALID_HANDLE );

	}else{

		if( bClosed && 
			(pFileDel->dwAttr!=FILE_ATTR_VOLUME) ){
			// 2004-09-15, remove to end-lilin
			//FlushFileBuffer( pFileDel );

			SetFileSize( pVol, pFileDel, pFileDel->dwSize ,TRUE);
			FlushFileBuffer( pFileDel );
			// end
		}
		if( pFileDel->pFileCache )
			free( pFileDel->pFileCache );
		free( pFileDel );
	}
	return bStatus;
}


BOOL  IsFileHandle( PVOLUME_FAT pVol, PFILE_FAT   pFileJudge)
{
	PFILE_FAT	pFile= pVol->pFileListHead;
	BOOL       bStatus=FALSE;

	while( pFile ){

		if( pFile == pFileJudge ){
			bStatus=TRUE;
			break;
		}
		pFile=pFile->pNext;
	}
	if( !bStatus )
		SetLastError( ERROR_INVALID_HANDLE );

	return bStatus;
}

//  ************************************************************************************
//	ReadFatFile. Read out the user requested data.
//	
//	把用户要读的数据分成三部份，1为从 dwPointer开始到 dwPointer在的 cluster，2为从 dwPointer在的下一cluster
//		开始，包括 all cluster 能以 whole cluster　读的。3为剩下的不够一cluster的。
//	dwReadTail : 为1。
//	dwReadHead : 为3。
//  ************************************************************************************
BOOL ReadFatFile( PFILE_FAT	pFile, char * pBuf, DWORD dwNumToRead, LPDWORD lpdwNumRead )
{
	PVOLUME_FAT	pVolFat;
	DWORD	dwStartClus, dwOffInClus;
	DWORD	dwMiddleCluster;  //in cluster.
	DWORD	dwReadTail, dwReadHead; // in bytes.

	*lpdwNumRead=0;

	if( pFile->dwPointer == pFile->dwSize ){
		return TRUE;
	}
	pVolFat=pFile->pVolume;
	// should we adjust the file size ,when the user set the file pointer after the end of the file to enlarge 
	// a file, and make a read request after that ?????????
	dwNumToRead= min( dwNumToRead , pFile->dwSize - pFile->dwPointer );

	dwStartClus = pFile->dwPointer / pVolFat->dwClusterSize;	//first,compute the start cluster opposite the first cluster of the file
	dwOffInClus = pFile->dwPointer % pVolFat->dwClusterSize;
	
	//dwStartClus +=pFile->dwStartCluster; // now, dwStartClus becomes the realy cluster number in the FAT.
	// 2005-03-08, modify by lilin
	//if( pFile->dwClusterCurrent ==-1 ){
	if( INVALID_CLUSTER( pVolFat, pFile->dwClusterCurrent ) ){
	// 2005-03-08
		pFile->dwClusterCurrent=FileGetCurrentCluster( pVolFat,pFile->dwStartCluster, dwStartClus );
		//pFile->dwClusIndex=dwStartClus;
	}
	dwReadHead = min( dwNumToRead , pVolFat->dwClusterSize - dwOffInClus );

	if( dwReadHead == pVolFat->dwClusterSize )
		dwReadHead=0;

	if( dwNumToRead > dwReadHead ){

		dwReadTail = ( dwNumToRead - dwReadHead )% pVolFat->dwClusterSize;
		dwMiddleCluster=( dwNumToRead -dwReadHead )/ pVolFat->dwClusterSize;

	}else{

		dwMiddleCluster=0;
		dwReadTail=0;
	}
	if( dwReadHead ){
	
		if( !CacheFileData( pFile, pVolFat, pFile->dwClusterCurrent ) )
			goto Read_Failed;

		memcpy( pBuf, pFile->pFileCache+ dwOffInClus,  dwReadHead );
		*lpdwNumRead= dwReadHead;

		if ( dwOffInClus + dwReadHead >= pVolFat->dwClusterSize ) {
			pFile->dwClusterCurrent= FileGetCurrentCluster(pVolFat, pFile->dwClusterCurrent, 1);
		}
	}
	if( dwMiddleCluster ){
		
		pFile->dwClusterCurrent=ReadWholeCluster( pVolFat, pBuf+dwReadHead, pFile->dwClusterCurrent, dwMiddleCluster );
		if( pFile->dwClusterCurrent ==-1) 
			goto Read_Failed;

		//pFile->dwClusIndex+=dwMiddleCluster;
		*lpdwNumRead= dwReadHead +dwMiddleCluster* pVolFat->dwClusterSize;
	}
	if( dwReadTail ){

		if( !CacheFileData( pFile, pVolFat, pFile->dwClusterCurrent ) )
			goto Read_Failed;

		memcpy( pBuf+dwReadHead + dwMiddleCluster* pVolFat->dwClusterSize,
				pFile->pFileCache,  dwReadTail );

		*lpdwNumRead= dwReadHead +dwMiddleCluster* pVolFat->dwClusterSize +dwReadTail;
	}
Read_Failed:
	pFile->dwPointer +=*lpdwNumRead;	//Increase the file pointer.
	if( *lpdwNumRead <dwNumToRead && pFile->dwPointer < pFile->dwSize)
		return FALSE;
	else
		return TRUE;
}



BOOL  FatSetFileTime( PFILE_FAT pFile, const FILETIME *pCreate, const FILETIME *pLastAccess,const FILETIME * pLastWrite )
{
	PVOLUME_FAT	pVolume= pFile->pVolume;
	DWORD		dwBufPos;
	SYSTEMTIME		sys_time;

	if(pCreate ){
		pFile->ftCreate.dwHighDateTime= pCreate->dwHighDateTime;
		pFile->ftCreate.dwLowDateTime= pCreate->dwLowDateTime;
	}
	if( pLastAccess ){
		pFile->ftLastAccess.dwHighDateTime=pLastAccess->dwHighDateTime;
		pFile->ftLastAccess.dwLowDateTime=pLastAccess->dwLowDateTime;
	}
	if(pLastWrite){
		pFile->ftLastWrite.dwHighDateTime =pLastWrite->dwHighDateTime;
		pFile->ftLastWrite.dwLowDateTime =pLastWrite->dwLowDateTime;
	}

	if( (dwBufPos =CheckClusterCache( pVolume ,  pFile->dwClusterParent ) ) != -1 ){

		PDIR_ENTRY	pDirEntry= (PDIR_ENTRY) (pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );
		pDirEntry+=pFile->dwClusterOffset;

		if( pCreate){

			FileTimeToSystemTime( pCreate,&sys_time);

			if( sys_time.wYear < 1980 ||  sys_time.wYear >2107 )
				return FALSE;

			pDirEntry->mDateCreated.nDay=sys_time.wDay;
			pDirEntry->mDateCreated.nMonth=sys_time.wMonth;
			pDirEntry->mDateCreated.nYearCount=sys_time.wYear-1980;

			pDirEntry->mTimeCreated.nHour=sys_time.wHour;
			pDirEntry->mTimeCreated.nMinute=sys_time.wMinute;
			pDirEntry->mTimeCreated.nTwiceSecond=sys_time.wSecond/2;

		}
		if( pLastAccess ){

			FileTimeToSystemTime( pLastAccess,&sys_time);

			if( sys_time.wYear < 1980 ||  sys_time.wYear >2107 )
				return FALSE;

			pDirEntry->mLastAccessDate.nDay=sys_time.wDay;
			pDirEntry->mLastAccessDate.nMonth=sys_time.wMonth;
			pDirEntry->mLastAccessDate.nYearCount=sys_time.wYear-1980;

		}
		if( pLastWrite ){

			FileTimeToSystemTime( pLastWrite,&sys_time);

			if( sys_time.wYear < 1980 ||  sys_time.wYear >2107 )
				return FALSE;

			pDirEntry->mDateLastWrite.nDay=sys_time.wDay;
			pDirEntry->mDateLastWrite.nMonth=sys_time.wMonth;
			pDirEntry->mDateLastWrite.nYearCount=sys_time.wYear-1980;

			pDirEntry->mTimeLastWrite.nHour=sys_time.wHour;
			pDirEntry->mTimeLastWrite.nMinute=sys_time.wMinute;
			pDirEntry->mTimeLastWrite.nTwiceSecond=sys_time.wSecond/2;

		}
		return 	WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER);
	}
	return FALSE;
}
//  ************************************************************************************
//	FatDeleteFileByHanle利用文件句柄删除文件
//	bDelete:	为TRUE时，文件会被真正删除，所占有的磁盘空间会被释放。
//				为FALSE时，文件不会被删除，只是删除了文件名记录（用在改名或移动）。
//  ************************************************************************************

BOOL	FatDeleteFileByHanle( HANDLE  hFile ,BOOL bDelete) 
{
	PFILE_FAT	pFile =(PFILE_FAT)hFile;
	DWORD	dwMax, i;
	PLONG_NAME_ENTRY pEntry;
	DWORD	dwBufPos;
	PVOLUME_FAT	 pVolume=(PVOLUME_FAT) pFile->pVolume;
	PDIR_ENTRY	pDirEntry;

	if( pFile->dwAttr &  (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY) ){
		
		return FALSE;
	}
		
	if( pFile->dwLongNameCluster ==-1 ) {  //it's invalid...

		pFile->dwLongNameCluster=pFile->dwClusterParent;
		pFile->dwLongNameOffset =pFile->dwClusterOffset;
	}

	if( (dwBufPos =CheckClusterCache( pVolume , pFile->dwLongNameCluster ) ) != -1 ){
	
		pEntry= (PLONG_NAME_ENTRY) (pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );

		pDirEntry=(PDIR_ENTRY)pEntry;

		pEntry+= pFile->dwLongNameOffset;

		if( pFile->dwLongNameCluster == pFile->dwClusterParent ){
			//
			dwMax=pFile->dwClusterOffset;
		}else{

			dwMax=31; // from 0 --- 31
		}
		for( i=pFile->dwLongNameOffset; i<= dwMax; i++ ){

			pEntry->cNameOrder=DIR_ENTRY_FREE;
			pEntry++;
		}

		if(! WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER) ){

			RETAILMSG( DEBUG_ERROR,(TEXT("FatDeleteFileByHanle Can't WriteCluster 11111\r\n")));
			return FALSE;
		}

		if( pFile->dwLongNameCluster != pFile->dwClusterParent){ 

			if( (dwBufPos =CheckClusterCache( pVolume , pFile->dwClusterParent ) ) == -1 ){

				RETAILMSG( DEBUG_ERROR,(TEXT("FatDeleteFileByHanle CheckClusterCache Failed\r\n")));
				return FALSE;
			}
			pEntry= (PLONG_NAME_ENTRY) (pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );

			pEntry+= pFile->dwClusterOffset;

			dwMax=pFile->dwClusterOffset;

			for( i=0; i<= dwMax; i++ ){

				pEntry->cNameOrder=DIR_ENTRY_FREE;
				pEntry++;
			}

			if(! WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER) ){

				RETAILMSG( DEBUG_ERROR,(TEXT("FatDeleteFileByHanle Can't WriteCluster\r\n")));
				return FALSE;
			}
		}
		if( bDelete ){

			if( !ReleaseClusterChain( pVolume ,pFile->dwStartCluster) )
				RETAILMSG( DEBUG_ERROR,(TEXT("FatDeleteFileByHanle Release Cluster %d failed\r\n"),pFile->dwStartCluster));
		}
		return TRUE;
	}
	return FALSE;
}


BOOL FatFileSysMoveFile( PVOLUME_FAT pVolume,  char * szOldFileName,  char *  szNewFileName ) 
{
	HANDLE  hFile;
	BOOL	bSuccess=FALSE;
	PFILE_FAT  pFile;

	hFile=CreateFatFile( pVolume, szOldFileName,0, 0, OPEN_EXISTING,0, FALSE);

	if( hFile != INVALID_HANDLE_VALUE) {

		APPEND_RECORD	append;
		FILE_FAT	file_fat;

		pFile =(PFILE_FAT)hFile;
		if( pFile->dwAttr &FILE_ATTR_DIRECTORY )
			append.bFile=FALSE;
		else
			append.bFile=TRUE;

		append.dwAttr= pFile->dwAttr;
		append.dwFileStartClus=pFile->dwStartCluster;

		if( !AddNameRec( pVolume, szNewFileName,&append, &file_fat) ){
			DeleteFileHandle( pVolume, pFile,FALSE );
			return FALSE;
		}
		file_fat.dwPointer=pFile->dwSize;
		file_fat.dwSize=pFile->dwSize;
		if( FatSetFileTime( &file_fat, &pFile->ftCreate,&pFile->ftLastAccess,&pFile->ftLastWrite) ){

			bSuccess=SetFileSize(pVolume, &file_fat, file_fat.dwSize, FALSE);
		}
		//bSuccess=FatSetFileTime( &file_fat, &pFile->ftCreate,&pFile->ftLastAccess,&pFile->ftLastWrite);
		//at last, don't forget delete the old file recorder from it's parent's cluster.
		bSuccess=FatDeleteFileByHanle( hFile ,FALSE);
		DeleteFileHandle( pVolume, (PFILE_FAT)hFile,FALSE );

		if( bSuccess ){			
			{
#ifdef UNDER_CE
				TCHAR  szOld[MAX_PATH], szNew[MAX_PATH];
				int		iLen;

				iLen=MultiByteToWideChar( CP_ACP, 0, szOldFileName, strlen(szOldFileName),szOld,MAX_PATH);
				szOld[iLen]=0;
				iLen=MultiByteToWideChar( CP_ACP, 0, szNewFileName , strlen(szNewFileName ),szNew,MAX_PATH);
				szNew[iLen]=0;
				NotifyShell( pVolume, szNew, NULL, pFile->dwAttr,pFile->dwSize, SHCNE_CREATE );
				NotifyShell( pVolume, szOld, NULL, pFile->dwAttr,pFile->dwSize, SHCNE_DELETE );
#endif
			}
		}
	}
	return bSuccess;
}

BOOL FatFileSysFileCopy( PVOLUME_FAT pVol,  char * lpExistingFileName,  char * lpNewFileName, DWORD dwCreateFlag )
{
	BOOL	bSuccess=FALSE;
	PFILE_FAT  pExistFile , pNewFile;
	DWORD	dwBufSize ,dwFileSize ,i ,dwReadSize, dwWriteSize;
	char*   pBuf;
	DWORD	dwRead;

#ifdef UNDER_CE
	TCHAR  szOld[MAX_PATH], szNew[MAX_PATH];
	int		iLen;

	iLen=MultiByteToWideChar( CP_ACP, 0, lpExistingFileName, strlen(lpExistingFileName),szOld,MAX_PATH);
	szOld[iLen]=0;
	iLen=MultiByteToWideChar( CP_ACP, 0, lpNewFileName, strlen(lpNewFileName ),szNew,MAX_PATH);
	szNew[iLen]=0;
#endif

	if( stricmp( lpExistingFileName, lpNewFileName ) ==0 )
		return TRUE;

	pExistFile=(PFILE_FAT)CreateFatFile( pVol, lpExistingFileName,0, 0, OPEN_EXISTING,0, TRUE);

	// 2004-08-09, modify by lilin
	if(	pExistFile != INVALID_HANDLE_VALUE )
	{
		// remove by lilin -2004-08-09, 由下面确定
		//if( pVol->fat_info.dwFreeCluster > (pExistFile->dwSize/pVol->dwClusterSize +2) )
		{		
			//2005-01-19, 直接用参数- lilin
			//DWORD  dwCreateFlag;
			//if( bFailIfExists )
				//dwCreateFlag= CREATE_NEW;
			//else
				//dwCreateFlag= CREATE_ALWAYS;
			//
			
			pNewFile=(PFILE_FAT)CreateFatFile( pVol, lpNewFileName, GENERIC_WRITE, 0, dwCreateFlag,0, TRUE);
			if(pNewFile !=INVALID_HANDLE_VALUE){
                // 为新文件分配文件空间 --2004-08-09
				if( SetFileSize(pVol, pNewFile, pExistFile->dwSize, FALSE) ) {
					//分配成功
#ifdef UNDER_CE
					NotifyShell( pVol, szNew, NULL, pNewFile->dwAttr, 0, SHCNE_CREATE);
#endif
					dwBufSize= pVol->dwClusterSize * BURST_WRITE_CLUSTER;
					dwFileSize= pExistFile->dwSize;
					
					dwBufSize=min( dwFileSize, dwBufSize );
					pBuf=(char*)malloc( dwBufSize );
					dwRead=dwBufSize;
#ifdef	FAT_DEBUG
					//srand(GetTickCount( ));
#endif
					if( pBuf) {
						
						for( i=0; i< dwFileSize;  ){
							
#ifdef	FAT_DEBUG
							//dwRead=rand( );
							//dwRead %=dwBufSize;
#endif
							if (ReadFatFile( pExistFile, pBuf, dwRead, &dwReadSize) ){
								
								if( WriteFatFile(pNewFile,pBuf,dwReadSize,&dwWriteSize ) ){
									
									if( dwWriteSize == dwReadSize ){
										
										i+= dwReadSize;
										continue;
									}
								}
							}
							break;
						}
						if( i>= dwFileSize ){
							
							bSuccess=TRUE;
						}
						free( pBuf);
					}
					if(  bSuccess ) {
						//之前已设 - 去掉 --2004-08-09
						//pNewFile->dwPointer=pExistFile->dwSize;
						//pNewFile->dwSize=pExistFile->dwSize;

						if( FatSetFileTime( pNewFile, &pExistFile->ftCreate,&pExistFile->ftLastAccess,&pExistFile->ftLastWrite) ){
							bSuccess=TRUE;// 之前已设 SetFileSize(pVol, pNewFile, pExistFile->dwSize, FALSE);
						}
						//
#ifdef UNDER_CE
						NotifyShell( pVol, szNew, NULL, pNewFile->dwAttr, pExistFile->dwSize, SHCNE_UPDATEITEM );
#endif
					}else{						
						FatDeleteFileByHanle(pNewFile, TRUE );
					}
				}
				DeleteFileHandle( pVol, pNewFile ,TRUE);				
			}
		}
		DeleteFileHandle( pVol, pExistFile ,FALSE);
	}
	return bSuccess;
}
// *******************************************************************************************
// WriteFatFile:　实际完成文件写操作。
//		先求岀当前要写的位置落在第几个 cluster(文件的第一个cluster为0，)，这和Read时的做法是一样的。
//		再求岀要写的位置落在这个cluster中的位置 offset.
//		如果这个位置能被 cluster size 整除(从一个cluster之开始起写)，并且写的数据多于一个cluster,
//		则这个cluster不被 cache,否则，这个cluster要被cache.
//		在整个过程中，如果需要新的cluster,则从FAT表中去分配(尽可能　分配最多的)。
// *******************************************************************************************
BOOL	WriteFatFile( PFILE_FAT pFile, char* lpBuffer, DWORD dwNumToWrite, DWORD* lpdwNumWrited)
{
	PVOLUME_FAT	pVolume;
	DWORD	dwStartClus ,dwOffInClus;
	DWORD   dwWriteHead ,dwMiddleCluster, dwWriteTail;


	pVolume=pFile->pVolume;
	* lpdwNumWrited=0;

	if( pFile->dwStartCluster == 0 )
	{
		DWORD dwBufPos;
		dwStartClus = FatAllocCluster( pVolume, 1, NULL);

		if( dwStartClus ==-1 )
			return FALSE;
		pFile->dwLastValidClus = pFile->dwStartCluster = dwStartClus;
		pFile->dwClusterCurrent = -1;

		if( (dwBufPos =CheckClusterCache( pVolume, pFile->dwClusterParent ) ) != -1 ){
			
			PDIR_ENTRY	pDirEntry= (PDIR_ENTRY) 
				(pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );
			
			pDirEntry+=pFile->dwClusterOffset;
			
			pDirEntry->wFirstClusterHigh= (WORD)(( pFile->dwStartCluster >>16)  &0xFFFF );
			pDirEntry->wFirstClusterLow = (WORD)pFile->dwStartCluster;		
			
			return 	WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER);
		}
	}


	dwNumToWrite= min( dwNumToWrite , pVolume->fat_info.dwFreeCluster* pVolume->dwClusterSize );

	dwStartClus = pFile->dwPointer / pVolume->dwClusterSize;	//first,compute the start cluster opposite the first cluster of the file
	dwOffInClus = pFile->dwPointer % pVolume->dwClusterSize;
	//2005-03-08, modify by lilin
	//if( pFile->dwClusterCurrent ==-1  ){
	if( INVALID_CLUSTER( pVolume, pFile->dwClusterCurrent ) ){
	//2005-03-08

		pFile->dwClusterCurrent=FileGetCurrentCluster( pVolume,pFile->dwStartCluster, dwStartClus );
		if( INVALID_CLUSTER(pVolume,pFile->dwClusterCurrent) ){
			//2004-06-21, modift this
			//pFile->dwLastValidClus = FileGetCurrentCluster( pVolume,pFile->dwStartCluster, dwStartClus-1 );
			pFile->dwLastValidClus = FileGetLastValidCluster( pVolume, pFile->dwStartCluster );
			//2004-06-21
		}
		//pFile->dwClusIndex=dwStartClus;
	}
	dwWriteHead = min( dwNumToWrite , pVolume->dwClusterSize - dwOffInClus );

	if( dwWriteHead == pVolume->dwClusterSize )
		dwWriteHead=0;

	if( dwNumToWrite > dwWriteHead ){

		dwWriteTail = ( dwNumToWrite - dwWriteHead )% pVolume->dwClusterSize;
		dwMiddleCluster=( dwNumToWrite -dwWriteHead )/ pVolume->dwClusterSize;

	}else{
		dwMiddleCluster=0;
		dwWriteTail=0;
	}
	if( dwWriteHead ){

		if( INVALID_CLUSTER( pVolume, pFile->dwClusterCurrent) ){

			pFile->dwClusterCurrent=FatAllocCluster( pVolume, 1, NULL);
			if( pFile->dwClusterCurrent ==-1 )
				goto Write_Failed;

			LinkCluster( pVolume, pFile->dwLastValidClus, pFile->dwClusterCurrent);
			//2004-06-21, modift this
			pFile->dwLastValidClus = pFile->dwClusterCurrent;
			//pFile->dwLastValidClus = FileGetLastValidCluster( pVolume, pFile->dwClusterCurrent );//;
			//2004-06-21
			//2004-08-26, delete this
			//if( pFile->dwClusterCurrent ==-1 )
				//goto Write_Failed;
			//2004-08-26, delete this
		}
		if( !CacheFileData( pFile, pVolume, pFile->dwClusterCurrent ) )
			goto Write_Failed;

		memcpy( pFile->pFileCache+ dwOffInClus, lpBuffer,  dwWriteHead );

		pFile->bCacheNeedFlush=TRUE;
		*lpdwNumWrited= dwWriteHead;

		if( dwWriteHead+ dwOffInClus >=pVolume->dwClusterSize ){

			pFile->dwClusterCurrent=GetNextCluster( pVolume, pFile->dwClusterCurrent );
		}
	}
	if( dwMiddleCluster ){
		
		pFile->dwClusterCurrent=WriteWholeCluster( pVolume, lpBuffer+dwWriteHead, pFile, dwMiddleCluster );
		if( pFile->dwClusterCurrent ==-1 ){ //没有更多的磁盘空间。

			*lpdwNumWrited= dwWriteHead;
			goto Write_Failed;
		}
		*lpdwNumWrited= dwWriteHead +dwMiddleCluster* pVolume->dwClusterSize;
		/*if( pFile->dwClusIndex ==0 )
			pFile->dwClusIndex+=dwMiddleCluster-1;
		else
			pFile->dwClusIndex+=dwMiddleCluster;
			*/
	}
	if( dwWriteTail ){

		if( INVALID_CLUSTER( pVolume, pFile->dwClusterCurrent ) ){

			pFile->dwClusterCurrent=FatAllocCluster( pVolume, 1, NULL);
			if( INVALID_CLUSTER( pVolume, pFile->dwClusterCurrent ))
				goto Write_Failed;

			LinkCluster( pVolume, pFile->dwLastValidClus, pFile->dwClusterCurrent);
			//2004-06-21, remove by lilin
			//pFile->dwLastValidClus = pFile->dwClusterCurrent;
			pFile->dwLastValidClus = FileGetLastValidCluster( pVolume, pFile->dwClusterCurrent );//;
			//2004-06-21

		}
		if( !CacheFileData( pFile, pVolume, pFile->dwClusterCurrent ) )
			goto Write_Failed;

		memcpy( pFile->pFileCache,
				lpBuffer+dwWriteHead + dwMiddleCluster* pVolume->dwClusterSize,
				dwWriteTail );
		pFile->bCacheNeedFlush=TRUE;

		*lpdwNumWrited= dwWriteHead +dwMiddleCluster* pVolume->dwClusterSize +dwWriteTail;
	}
Write_Failed:
	pFile->dwPointer +=*lpdwNumWrited;	//Increase the file pointer.
	if( pFile->dwSize< pFile->dwPointer )
		pFile->dwSize = pFile->dwPointer;

	if( *lpdwNumWrited ==0 ) //&& pFile->dwPointer < pFile->dwSize)
		return FALSE;
	else
		return TRUE;
}

BOOL FatSetEndOfFile(  PFILE_FAT  pFile )
{
	PVOLUME_FAT	pVolume =pFile->pVolume;

	if( pFile->dwPointer == pFile->dwSize )
		return TRUE;

	return SetFileSize(pVolume,pFile, pFile->dwPointer, TRUE );
}

/* 2005-01-27, lilin modify */
#if 0
BOOL	SetFileSize( PVOLUME_FAT pVolume, PFILE_FAT  pFile, DWORD dwSize ,BOOL bSizeChanged)
{
	DWORD dwClusterSize ,dwClusterEnd ,dwBufPos;

	if( bSizeChanged )	{

		dwClusterSize =  dwSize/pVolume->dwClusterSize;
	    // ？？？？？
    	if (pFile->dwSize <= pVolume->dwClusterSize )
			pFile->dwSize =pVolume->dwClusterSize;
		dwClusterEnd  = pFile->dwSize   /pVolume->dwClusterSize;
		

	    // 2004-08-25 lilin add code
		//if( dwClusterSize != dwClusterEnd )
		{
		// 2004-08-25 lilin	
			if( dwClusterSize <= dwClusterEnd  ) { //file is reduced.
				
				pFile->dwClusterCurrent =FileGetCurrentCluster( pVolume, pFile->dwStartCluster, dwClusterSize );
				
				//在这里应该将文件结尾以后的cluster free掉，否则会造成空间泄露。Braden 2003-9-19.
				ReleaseClusterChain(  pVolume , pFile->dwClusterCurrent ); //Braden 2003-9-19 added.
				//注意，如果　CriticalSection的策略改变，请检查上一句的正确性。
				LinkCluster( pVolume,pFile->dwClusterCurrent, FAT_CLUSTER_EOC );
				
			}else{  //file is enlarged.
				
				pFile->dwClusterCurrent=FileGetCurrentCluster( pVolume, pFile->dwStartCluster, dwClusterEnd );
				
				if( pFile->dwClusterCurrent ==-1 )
					return FALSE;
				
				pFile->dwClusterCurrent=WriteWholeCluster(pVolume,NULL,pFile, dwClusterSize- dwClusterEnd );
				
				if( pFile->dwClusterCurrent ==-1 )
					return FALSE;
				
			}
		}
	}
	pFile->dwSize =dwSize;
	if( (dwBufPos =CheckClusterCache( pVolume ,  pFile->dwClusterParent ) ) != -1 ){

		PDIR_ENTRY	pDirEntry= (PDIR_ENTRY) 
				(pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );

		pDirEntry+=pFile->dwClusterOffset;

		pDirEntry->dwFileSize= pFile->dwSize;

		return 	WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER);
	}
	return FALSE;
}
#endif


BOOL	SetFileSize( PVOLUME_FAT pVolume, PFILE_FAT  pFile, DWORD dwSize ,BOOL bSizeChanged)
{
	DWORD dwNewClusterSize ,dwOldClusterSize ,dwBufPos;

	if( bSizeChanged )	{

		dwNewClusterSize =  (dwSize + pVolume->dwClusterSize - 1) / pVolume->dwClusterSize;
	    // ？？？？？
    	//if (pFile->dwSize <= pVolume->dwClusterSize )
			//pFile->dwSize =pVolume->dwClusterSize;
		dwOldClusterSize  = (pFile->dwSize + pVolume->dwClusterSize - 1) / pVolume->dwClusterSize;
		

	    // 2004-08-25 lilin add code
		//if( dwClusterSize != dwClusterEnd )
		{
		// 2004-08-25 lilin	
			if( dwNewClusterSize < dwOldClusterSize  ) 
			{ //file is reduced.
				DWORD dwFreeCluster;
				dwFreeCluster = FileGetCurrentCluster( pVolume, pFile->dwStartCluster, dwNewClusterSize );
				
				//在这里应该将文件结尾以后的cluster free掉，否则会造成空间泄露。Braden 2003-9-19.
				ReleaseClusterChain( pVolume , dwFreeCluster ); //Braden 2003-9-19 added.
				//注意，如果　CriticalSection的策略改变，请检查上一句的正确性。
				//设置结束标志
				if( dwNewClusterSize )
				{
					pFile->dwClusterCurrent = FileGetCurrentCluster( pVolume, pFile->dwStartCluster, dwNewClusterSize - 1 );
					LinkCluster( pVolume, pFile->dwClusterCurrent, FAT_CLUSTER_EOC );
				}
				else
				{
					ASSERT( dwSize == 0 );
					pFile->dwClusterCurrent = -1;
					pFile->dwStartCluster = 0;
				}
				
			}else if( dwNewClusterSize > dwOldClusterSize ){  //file is enlarged.
				if( pFile->dwStartCluster )
				{
					pFile->dwClusterCurrent=FileGetCurrentCluster( pVolume, pFile->dwStartCluster, dwOldClusterSize );
					
					if( pFile->dwClusterCurrent ==-1 )
					{
						ASSERT( 0 );
						return FALSE;
					}
					
					pFile->dwClusterCurrent=WriteWholeCluster( pVolume,NULL,pFile, dwNewClusterSize- dwOldClusterSize );
					
					if( pFile->dwClusterCurrent ==-1 )
					{
						ASSERT( 0 );
						return FALSE;
					}
				}
				else
				{
					//pFile->dwClusterCurrent=WriteWholeCluster( pVolume,NULL,pFile, dwNewClusterSize- dwOldClusterSize );
					DWORD dwClusAlloced;
					DWORD dwStartCluster;
					dwStartCluster = FatAllocCluster( pVolume, dwNewClusterSize- dwOldClusterSize, &dwClusAlloced);
					if( dwClusAlloced )
					{
						pFile->dwClusterCurrent = pFile->dwStartCluster = dwStartCluster;
						pFile->dwClusterCurrent = WriteWholeCluster( pVolume,NULL,pFile, dwNewClusterSize- dwOldClusterSize );
					}
					else
					{
						ASSERT( 0 );
						return FALSE;
					}
				}
			}
		}
	}
	pFile->dwSize =dwSize;
	if( (dwBufPos =CheckClusterCache( pVolume ,  pFile->dwClusterParent ) ) != -1 ){
		DWORD dwCluster;
		PDIR_ENTRY	pDirEntry= (PDIR_ENTRY) 
				(pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );

		pDirEntry+=pFile->dwClusterOffset;
		pDirEntry->dwFileSize= pFile->dwSize;
		dwCluster = pFile->dwStartCluster;
		if( dwCluster >0 )
			dwCluster-= pVolume->dwRootDirClusterNum;
		pDirEntry->wFirstClusterHigh= (WORD)(( dwCluster >>16)  &0xFFFF );
		pDirEntry->wFirstClusterLow = (WORD)dwCluster;

		return 	WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER);
	}
	return FALSE;
}



BOOL	SetFileAttributesByHandle( PVOLUME_FAT pVolume, PFILE_FAT  pFile, DWORD dwAttr)
{
	DWORD	dwBufPos;
	pFile->dwAttr=dwAttr;

	if( (dwBufPos =CheckClusterCache( pVolume ,  pFile->dwClusterParent ) ) != -1 ){

		PDIR_ENTRY	pDirEntry= (PDIR_ENTRY) (pVolume->ccClusterCache.pBufStart + dwBufPos* pVolume->dwClusterSize );

		pDirEntry+=pFile->dwClusterOffset;
		pDirEntry->cAttr= (UCHAR)pFile->dwAttr;

		return 		WriteCluster(pVolume, dwBufPos,  FLUSH_CLUSTER); 
	}
	return TRUE;
} 


BOOL CacheFileData( PFILE_FAT pFile, PVOLUME_FAT pFatVol, DWORD dwCluster)
{
	BOOL	bSuccess=TRUE;

	if( pFile->dwCachedCluster != dwCluster ) {

		if( pFile->dwCachedCluster !=-1 ){ // our cache takes the valid data.

			if( pFile->bCacheNeedFlush  ){ //we should flush the old buffer if neccessary.

				WriteClusterWithBuf( pFatVol,pFile->dwCachedCluster, pFile->pFileCache);
				pFile->bCacheNeedFlush =FALSE;
			}
		}
		bSuccess=ReadCluster(pFatVol, dwCluster,pFile->pFileCache, pFatVol->dwClusterSize);
		if( bSuccess ){

			pFile->dwCachedCluster=dwCluster;
		}else{

			RETAILMSG( 1,(TEXT("CacheFileData... failed to read disk\r\n")));
		}
	}
	return bSuccess;
}

BOOL	FlushFileBuffer( PFILE_FAT	pFile )
{
	if( pFile->bCacheNeedFlush ){

		WriteClusterWithBuf(pFile->pVolume,pFile->dwCachedCluster, pFile->pFileCache);
		pFile->bCacheNeedFlush =FALSE;
	}

	return TRUE;
}
