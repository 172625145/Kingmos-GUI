/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：实现FAT文件系统中查找有关的函数。
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙
修改记录：
	1.2004-06-22 lilin 在查找过程中我返回了 "." 和 ".."
******************************************************/
#include "fatfs.h"
#include "fatfmt.h"

//static BOOL IsWantedFile( unsigned char*  pcsSrc, unsigned char* pcsName );
static DWORD GetFileInfo( DWORD dwCluster, PDIR_ENTRY	pDirEntry, DWORD dwMaxBuf, FILE_FIND_DATA * pFindData, PDIR_ENTRY_FIND pEntry_Find);
static void CopyShortName( char * cName, PDIR_ENTRY  pDirEntry );
static BOOL FileNameCompare( unsigned char* lpcszMask, int iMaskLen, unsigned char* lpcszSrc, int iSrcLen );

//	*********************************************************************************
//	FindFile... this function is  called only by FAT_FindFirstFile.
//	*********************************************************************************
HANDLE	FindFile( PVOLUME_FAT pVol, char* lpcszFileSpec, FILE_FIND_DATA * pfd )
{
	HANDLE hFind=INVALID_HANDLE_VALUE;
	PUCHAR	 lpFile= (PUCHAR)lpcszFileSpec;
	BOOL	 bFirst=TRUE;
	FIND_RESULT	 find_result;
	DWORD	dwCurrCluster;

	//RETAILMSG( DEBUG_FIND,(TEXT("FAT FindFile entered %s\r\n"),lpcszFileSpec));

	if( lpcszFileSpec ==NULL || pfd==NULL ){

		SetLastError( ERROR_INVALID_PARAMETER);
		return INVALID_HANDLE_VALUE;
	}
	dwCurrCluster=pVol->fat_info.dwRootCluster;//先从要目录开始

	//先找到其父目录之内容开始的cluster.
	//请在FindFileInDirectory函数的入口处查看其不同的用法。
	while( IsPath(lpFile ) ){

		dwCurrCluster=FindFileInDirectory( pVol, dwCurrCluster ,lpFile,pfd, NULL, TRUE);

		if(dwCurrCluster ==-1 ) //没有找到这个目录
		{
			SetLastError(ERROR_FILE_NOT_FOUND);
			return INVALID_HANDLE_VALUE;
		}

		lpFile=CutCurrentPath( lpFile );
		//dwCurrCluster=pfd->
	}
	//find_result.dwOffset=0;
	//然后在父目录的内容中查找文件名（目录名）
	memset( &find_result, 0, sizeof( find_result ) );
	//find_result.dwOffset=0;
	if(FindFileInDirectory( pVol, dwCurrCluster, lpFile ,pfd ,&find_result,TRUE) !=-1 ){ //success

		//insert the find struct to the head of our list.
		//如果找到了这个文件（目录），就将其插入我们的链表，并给用户程序返回一个句柄。
		PFIND_RESULT	pFind= (PFIND_RESULT)malloc( sizeof(FIND_RESULT) );

		if( !pFind )
			return INVALID_HANDLE_VALUE;
		
		memcpy(pFind, &find_result, sizeof( FIND_RESULT) );

		pFind->dwParentStart= dwCurrCluster;

		pFind->pNext=pVol->pFindListHead;
		pVol->pFindListHead=pFind;
		SetLastError(ERROR_FILE_EXISTS);

		return pFind;
	}

	SetLastError(ERROR_FILE_NOT_FOUND);
	return INVALID_HANDLE_VALUE;
}

void GetCurrentPathName( char * csTemp, unsigned char* lpFile )
{
	int i;
	// modify by lilin-2004-08-03
	//if(*lpFile == '\\' || *lpFile=='/' )
	if( IS_PATH_MARK( lpFile ) )  
		lpFile++;
	//

	i=0;
	while( *lpFile ){

		csTemp[i]=*lpFile;
		// modify by lilin-2004-08-03
		//if(*lpFile == '\\' || *lpFile =='/' ){
		if( IS_PATH_MARK( lpFile ) ){

			csTemp[i]=0;
			break;
		}
		//
		i++;
		lpFile++;
	}
	csTemp[i]=0;
}
//  ******************************************************************************************
//	FindFileInDirectory:
//	Parameter:	
//		dwCurCluster:	当前的搜索应从这个cluster开始。
//		lpFile:			文件/路径名
//		pFindData：		用户程序提供的存储文件信息的结构
//		pFind:			NULL时：函数返回当前目录/文件的内容开始的cluster.
//						非NULL：函数返回当前目录/文件名所在的cluster.
//		bLongName:		要求提供长文件名，一般为TRUE
//		return: success: .
//				fail:	-1.
//	此函数从dwCurCluster开始，查找用户想要的文件/目录，它先将dwCurCluster读入缓冲，调用GetFileInfo
//	去解析一个文件记录，再判断该记录是否为用户想要的，如不是则继续调用GetFileInfo解析，在这个过程中，
//	如果缓冲的数据已经用完，就调用GetNextCluster去得到下一个cluster,直到找到了文件记录或失败。
//  ******************************************************************************************
DWORD FindFileInDirectory( PVOLUME_FAT  pVol, DWORD dwCurCluster,  char* lpFile, 
						  FILE_FIND_DATA * pFindData, PFIND_RESULT pFind ,BOOL	bLongName)
{
	char csTemp[256];
	DWORD	dwDirPassed  ,dwDirTotalPassed;

	PCLUSTER_CACHE		pCache= &pVol->ccClusterCache;
	PDIR_ENTRY			pDirEntry;
	DWORD				dwMaxBuf;
	DIR_ENTRY_FIND		entry_find;

	DWORD				dwOffsetInCluster=0;
	DWORD				dwCurrentPos;

	RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory entered \r\n")));

	dwCurrentPos= CheckClusterCache(  pVol ,  dwCurCluster );
	if( dwCurrentPos ==-1)
		return -1;

	if( pFind){

		dwOffsetInCluster=pFind->dwOffset;
	}
	pDirEntry=(PDIR_ENTRY) ( pCache->pBufStart + dwCurrentPos * pVol->dwClusterSize ) +dwOffsetInCluster;

	dwMaxBuf=(DWORD) (pCache->pBufStart + (dwCurrentPos+1) * pVol->dwClusterSize );

	RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory 1111111111\r\n")));
	if( lpFile) {

		GetCurrentPathName( csTemp, lpFile );

	}else{

		strcpy( csTemp, pFind->csCondition );
	}
	memset(pFindData,0,sizeof( FILE_FIND_DATA ));
	pFindData->nFileSizeHigh=-1; //我们先将这个值设为－1，在GetFileInfo中如果找到了一个文件记录，这将会被设成0，这样我们.
	//在这个函数中检查这个值，如果仍是－1，说明在当前cluster中没有找到文件，要从一个新的cluster开始查找，否则就以上一次
	//的查找结果为基础继续查找。

	memset( &entry_find,0, sizeof(entry_find) );

	entry_find.bLongName =bLongName;
	entry_find.dwCurEntryIndex=-1;
	entry_find.bValid=FALSE;
	entry_find.dwLongNameCluster= dwCurCluster ;
	entry_find.dwLongNameOffset =0;

	if( pFind ) {
		entry_find.dwLongNameCluster= dwCurCluster ;
		entry_find.dwLongNameOffset = pFind->dwOffset;
	}

	dwDirTotalPassed=0;

	RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory 222222222\r\n")));

	for( ;1;){

		if( ((DWORD)pDirEntry) >=dwMaxBuf ){

			dwCurCluster=GetNextCluster( pVol, dwCurCluster );

			if( INVALID_CLUSTER( pVol, dwCurCluster) ){

				RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory 33333333\r\n")));
				return -1;
			}

			if( (dwCurrentPos= CheckClusterCache(  pVol, dwCurCluster) ) == -1 ){
			
				RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory 444444444\r\n")));
				return -1;
			}
			if( pFind ){
			
				pFind->dwCluster=dwCurCluster;
				pFind->dwOffset= 0;
				dwDirTotalPassed=0;
			}
			RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory 5555555\r\n")));

			pDirEntry=(PDIR_ENTRY) (pVol->ccClusterCache.pBufStart +dwCurrentPos *pVol->dwClusterSize );
			dwMaxBuf =((DWORD)pDirEntry) +pVol->dwClusterSize;
			if( pFindData->nFileSizeHigh ==-1 ){

				entry_find.dwLongNameCluster= dwCurCluster;
				entry_find.dwLongNameOffset =0;
			}
		}
		RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory 666666666\r\n")));

		dwDirPassed=GetFileInfo(  dwCurCluster, pDirEntry ,dwMaxBuf, pFindData,&entry_find);
		if( dwDirPassed ==-1 ){

			RETAILMSG( DEBUG_FIND,(TEXT("FindFileInDirectory return -1\r\n")));
			//dwCurCluster=GetNextCluster( pVol, dwCurCluster );
			return -1;
		}
		dwDirTotalPassed+=dwDirPassed;
		pDirEntry+=dwDirPassed;

		if( pFindData->nFileSizeHigh !=-1 ){
			RETAILMSG( DEBUG_FIND,(TEXT("\r\nFAT Find %a (in %a) "),csTemp,pFindData->cFileName ));
			//lilin 2004-06-22, 我返回了 "." 和 ".."
			//if( IsWantedFile( csTemp, pFindData->cFileName) ){
			//if( (*( (PCHAR)&pFindData->cFileName[0])!='.' ) &&  
			//FileNameCompare( csTemp, 0, (PUCHAR)pFindData->cFileName, 0)){
			if( FileNameCompare( csTemp, 0, (PUCHAR)pFindData->cFileName, 0)){
			// 2004-06-22
				if( pFind ){

					if( lpFile )
						strcpy(pFind->csCondition,csTemp );

					pFind->dwCluster=dwCurCluster;
					pFind->dwOffset += dwDirTotalPassed;

					if( pFind->dwOffset <1 ){

						DWORD  error =GetLastError( );
					}
					//pFind->dwOffset--;
					pFind->pVolume= pVol;

					pFind->dwLongNameOffset  =entry_find.dwLongNameOffset;

					pFind->dwLongNameCluster =entry_find.dwLongNameCluster;

					//pFind->dwLongNameCluster +=pVol->dwRootDirClusterNum;

					pFindData->dwOID += pVol->dwRootDirClusterNum;
					//Braden added 2003-4-30...for FAT16 easy computation cluster ajusting.

				}else{ //the caller wants the context of the folder.
						//but now we point to the cluster containing the folder.
					dwCurCluster =pFindData->dwOID;
					dwCurCluster+=pVol->dwRootDirClusterNum;
					//dwCurCluster+=pVol->dwRootDirClusterNum-2; //
				}
				//Create find result struct.
				RETAILMSG( DEBUG_FIND,(TEXT("SUCCESS\r\n")));
				return dwCurCluster;
			}
			RETAILMSG( DEBUG_FIND,(TEXT("FAILED\r\n")));
			//Here, a file is found, but it's not the user wanted, so we should reset some parameter,otherwise,
			//our program will failed.
			entry_find.dwLongNameCluster= dwCurCluster ;
			entry_find.dwLongNameOffset = dwDirTotalPassed;

			entry_find.dwCurEntryIndex=-1;
			pFindData->nFileSizeHigh=-1;
			entry_find.bValid=FALSE;
		}else{
		}
	}
	return -1;
}
//	****************************************************************************************************************
//	GetFileInfo::从pDirEntry指向的buffer中,读出文件/目录名，存入 pFindData，并更改与查找相关的结构 pEntry_Find
//	
//	****************************************************************************************************************

DWORD GetFileInfo(  DWORD dwCluster, PDIR_ENTRY	pDirEntry, DWORD dwMaxBuf, FILE_FIND_DATA * pFindData, PDIR_ENTRY_FIND pEntry_Find)
{
	DWORD dwDirPassed=0;
	PLONG_NAME_ENTRY	pLongEntry=(PLONG_NAME_ENTRY)pDirEntry;
	DWORD	dwCurLongDirIndex;
	DWORD   i;


	for( i=0;(DWORD)pDirEntry<dwMaxBuf;  i++ ,	pDirEntry++ , pLongEntry++ ){

		if( pLongEntry->cNameOrder ==DIR_ENTRY_FREE ){
			
			RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo,, FREE entry found\r\n")));
			continue;
			
		}else if( pLongEntry->cNameOrder == DIR_ENTRY_EOF){ //no more files.
			
			RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo,, NO MORE FILE\r\n")));
			SetLastError( ERROR_NO_MORE_FILES);
			return -1;
			
		}else if( pLongEntry->cAttr == FILE_ATTR_LONG_NAME ) {

			//RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo,, LONG NAME\r\n")));
			
			if( (pLongEntry->cNameOrder & LAST_LONG_ENTRY )== LAST_LONG_ENTRY ) { // the last long name recorder.

				//Braden masked 2003-4-30.... here, there are incorrect directory recorder on the disk, just ignore them,
				//and go on.
				//if( pEntry_Find->dwCurEntryIndex != -1 ) //error
				//	continue;
					//return -1;
				dwCurLongDirIndex= (pLongEntry ->cNameOrder & ~LAST_LONG_ENTRY ) -1;

				if( dwCurLongDirIndex > MAX_LONG_NAME_ENTRY ) {//this folder take a invalid recorder, maybe it's damaged.

					pEntry_Find->bValid=FALSE;
					RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo, 000000000000\r\n")));
					continue;
				}
				memcpy(pEntry_Find->csName+ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY , pLongEntry->cLongName1 ,10);

				memcpy(pEntry_Find->csName+ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY +10, pLongEntry->cLongName2 ,12);

				memcpy(pEntry_Find->csName+ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY +22, pLongEntry->cLongName3 ,4);

				pEntry_Find->csName[ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY +26]=0;
				pEntry_Find->csName[ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY +27]=0;
				/*2004-09-13, aysn to fatsys_whl {//如果文件系统在写入这个文件记录时忘记了结尾的0，这时就可以弥补。
					int k;
					WORD	*pTemp=(WORD*)&pEntry_Find->csName[ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY];

					for( k=0; k<26;k++ ,pTemp++ ){

						if( *pTemp==0xFFFF )
							*pTemp=0;
					}
				}*/

				pEntry_Find->dwCurEntryIndex=dwCurLongDirIndex;
				pEntry_Find->cChecksum =pLongEntry->cChecksum;
				pEntry_Find->bValid    =TRUE;

				pEntry_Find->dwLongNameOffset= pEntry_Find->dwLongNameOffset+i;
				pEntry_Find->dwLongNameCluster=dwCluster;


			}else if( pLongEntry->cNameOrder < MAX_LONG_NAME_ENTRY ){

				if( (DWORD)(pLongEntry->cNameOrder ) != pEntry_Find->dwCurEntryIndex || pEntry_Find->bValid ==FALSE){

					pEntry_Find->bValid=FALSE;
					RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo, 1111111111\r\n")));
					continue;
				}
				dwCurLongDirIndex= pLongEntry->cNameOrder-1;

				memcpy(pEntry_Find->csName+ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY , pLongEntry->cLongName1 ,10);

				memcpy(pEntry_Find->csName+ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY +10, pLongEntry->cLongName2 ,12);
				
				memcpy(pEntry_Find->csName+ dwCurLongDirIndex * NAME_SIZE_ONE_ENTRY +22, pLongEntry->cLongName3 ,4);

				pEntry_Find->dwCurEntryIndex=pLongEntry->cNameOrder-1;
				
				if( pEntry_Find->cChecksum != pLongEntry->cChecksum ){
					
					pEntry_Find->bValid=FALSE;
				}

			}else{

				pEntry_Find->bValid=FALSE;

				RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo, 22222222222\r\n")));
				continue;
			}
		}else{

			if( pEntry_Find->bValid ){

				if( ShortNameCheckSum( pDirEntry->cShortName ) !=pEntry_Find->cChecksum ){

					pEntry_Find->bValid=FALSE;

					RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo, 333333333\r\n")));
				}
				//Delete the invalid long name.
			}
			if( pEntry_Find->bLongName && pEntry_Find->bValid ){

				int iLen;
				//wcstombs(pFindData->cFileName, (WCHAR*)pEntry_Find->csName, MAX_PATH );

				iLen=WideCharToMultiByte( CP_ACP,0,(WCHAR*)pEntry_Find->csName,WideStrLen( (WCHAR*)pEntry_Find->csName ),
									(PUCHAR)pFindData->cFileName, MAX_PATH, "_" ,FALSE);
				//iLen=WideCharToMultiByte(CP_ACP,0,(WCHAR*)pEntry_Find->csName,wcslen( (WCHAR*)pEntry_Find->csName ),
				//					pFindData->cFileName, MAX_PATH, "_" ,FALSE);
				*( (PUCHAR)pFindData->cFileName +iLen) =0;
				*( (PUCHAR)pFindData->cFileName +iLen+1) =0;
				{
					int i, iNameLen=WideStrLen( (WCHAR*)pEntry_Find->csName );
					WORD	*pTemp=(WORD*)pEntry_Find->csName;

					RETAILMSG( DEBUG_FIND,(TEXT("\r\nFAT find file ")));
					for( i=0; i< iNameLen; i++) {
					
						RETAILMSG( DEBUG_FIND,(TEXT("%c"),*pTemp++));
					}
					RETAILMSG( DEBUG_FIND,(TEXT("\r\nFAT file after parse %a "), pFindData->cFileName));
				}
				//wcstombs(pFindData->cFileName, (WCHAR*)pEntry_Find->csName, wcslen((WCHAR*)pEntry_Find->csName) );
				//RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo, 4444444444\r\n")));

			}else{
				///******************************************************
				CopyShortName( (PUCHAR)pFindData->cFileName, pDirEntry );
			}
			pFindData->dwFileAttributes=pDirEntry->cAttr;
			ParseFileTime( &pFindData->ftCreationTime, &pDirEntry->mDateCreated, &pDirEntry->mTimeCreated );
			ParseFileTime( &pFindData->ftLastAccessTime,&pDirEntry->mLastAccessDate,&pDirEntry->mTimeLastWrite);
			ParseFileTime( &pFindData->ftLastWriteTime, &pDirEntry->mDateLastWrite ,&pDirEntry->mTimeLastWrite);
			pFindData->nFileSizeHigh=0;
			pFindData->nFileSizeLow=pDirEntry->dwFileSize;
			pFindData->dwOID= FAT_MAKDWORD(pDirEntry->wFirstClusterHigh,pDirEntry->wFirstClusterLow );

			RETAILMSG( DEBUG_FIND,(TEXT("FAT GetFileInfo, NAME %s\r\n"),pFindData->cFileName));
			return i+1;
		}
	}
	return i;
}

static BOOL FileNameCompare( unsigned char* lpcszMask, int iMaskLen, unsigned char* lpcszSrc, int iSrcLen )
{
	PUCHAR lpcMaskEnd, lpcSrcEnd;

	if( iMaskLen == 0 )
		iMaskLen = strlen( lpcszMask );
	if( iSrcLen == 0 )
		iSrcLen = strlen( lpcszSrc );

	lpcMaskEnd = lpcszMask + iMaskLen;
	lpcSrcEnd = lpcszSrc + iSrcLen;
	while( (DWORD)lpcszMask < (DWORD)lpcMaskEnd && (DWORD)lpcszSrc < (DWORD)lpcSrcEnd )
	{
		if( *lpcszMask == '*' )
		{   // skip , get next name
			BOOL bContinue = FALSE;

			lpcszMask++;
			while( (DWORD)lpcszMask < (DWORD)lpcMaskEnd )
			{
				if( *lpcszMask++ == '.' )
				{
					bContinue = TRUE;
					break;
				}
			}

			if( bContinue == FALSE )
				return TRUE;  //no any char need to match
			
			while( (DWORD)lpcszSrc < (DWORD)lpcSrcEnd )
			{
				if( *lpcszSrc++ == '.' )
					break;
			}
			continue;
		}
		else if( *lpcszMask == '?' || *lpcszMask == *lpcszSrc )
		{
			;  // continue next char
		}
		else if( *lpcszMask >= 'A' && *lpcszMask <= 'Z' )
		{
			if( ((int)*lpcszMask - (int)*lpcszSrc) != ('A' - 'a') )
				return FALSE;
		}
		else if( *lpcszMask >= 'a' && *lpcszMask <= 'z' )
		{
			if( ((int)*lpcszMask - (int)*lpcszSrc) != ('a' - 'A') )
				return FALSE;
		}
		else
			return FALSE;
		lpcszSrc++; 
		lpcszMask++;
	}

	if( (DWORD)lpcszMask == (DWORD)lpcMaskEnd && (DWORD)lpcszSrc == (DWORD)lpcSrcEnd )
	{  // compare all char
		return TRUE;
	}

	if( lpcszSrc == lpcSrcEnd )
	{
		while( lpcszMask < lpcMaskEnd )
		{
			if( *lpcszMask == '?' || *lpcszMask == '*' || *lpcszMask == '.' )
			{
				lpcszMask++;
				continue;
			}
			else
				return FALSE;
		}
		return TRUE;
	}

	//ASSERT( lpcszMask == lpcMaskEnd );
	return FALSE;
}

// **********************************************************************************************
// CopyShortName:::从一个PDIR_ENTRY结构中拷贝短文件名到csName中。
// **********************************************************************************************
void CopyShortName( char * cName, PDIR_ENTRY  pDirEntry ) 
{
	int i ,iMinNull=8;
	BOOL	bExtend=FALSE;
	memcpy( cName, pDirEntry->cShortName,8 );

	for(i=1; i<=8; i++ ){

		if( cName[8-i] ==' ' ){

			cName[8-i]=0;
			iMinNull=8-i;

		}else{  //在文件名中间的空格应该被保留下来。
		  	break;
		}
	}
	for( i=0; i<3 ; i++ ){

		if( pDirEntry->cShortName[8+i] !=' '){

			cName[iMinNull+1+i]= pDirEntry->cShortName[8+i];
			bExtend=TRUE;

		}else{
			cName[iMinNull+1+i]=0;
		}
	}
	cName[iMinNull+1+i]=0;

	if( bExtend ){

		cName[iMinNull]='.';
	}else{

		cName[iMinNull]=0;
	}
}


BOOL  IsSearch( PVOLUME_FAT pVol, PFIND_RESULT pSearch  )
{
	PFIND_RESULT  pFind= pVol->pFindListHead;
	BOOL       bStatus=FALSE;

	while( pFind ){

		if( pFind == pSearch ){
			bStatus=TRUE;
			break;
		}
		pFind=pFind->pNext;
	}
	if( !bStatus )
		SetLastError( ERROR_INVALID_HANDLE );

	return bStatus;
}


BOOL	DeleteSearchHandle( PVOLUME_FAT pVol, PFIND_RESULT pSearch  )
{
	PFIND_RESULT  pFind= pVol->pFindListHead;
	BOOL       bStatus=FALSE;

	if( pFind == pSearch ){

		bStatus=TRUE;
		pVol->pFindListHead= pFind->pNext;

	}else{

		while( pFind->pNext ){

			if( pFind->pNext == pSearch ){

				bStatus=TRUE;
				pFind->pNext=pFind->pNext->pNext;
				break;
			}
			pFind=pFind->pNext;
		}
	}
	if( !bStatus ){

		SetLastError( ERROR_INVALID_HANDLE );

	}else{

		free( pSearch );
	}
	return bStatus;
}
