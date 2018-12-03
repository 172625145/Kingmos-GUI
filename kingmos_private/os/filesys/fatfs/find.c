/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����ʵ��FAT�ļ�ϵͳ�в����йصĺ�����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-4-18
���ߣ�κ����
�޸ļ�¼��
	1.2004-06-22 lilin �ڲ��ҹ������ҷ����� "." �� ".."
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
	dwCurrCluster=pVol->fat_info.dwRootCluster;//�ȴ�ҪĿ¼��ʼ

	//���ҵ��丸Ŀ¼֮���ݿ�ʼ��cluster.
	//����FindFileInDirectory��������ڴ��鿴�䲻ͬ���÷���
	while( IsPath(lpFile ) ){

		dwCurrCluster=FindFileInDirectory( pVol, dwCurrCluster ,lpFile,pfd, NULL, TRUE);

		if(dwCurrCluster ==-1 ) //û���ҵ����Ŀ¼
		{
			SetLastError(ERROR_FILE_NOT_FOUND);
			return INVALID_HANDLE_VALUE;
		}

		lpFile=CutCurrentPath( lpFile );
		//dwCurrCluster=pfd->
	}
	//find_result.dwOffset=0;
	//Ȼ���ڸ�Ŀ¼�������в����ļ�����Ŀ¼����
	memset( &find_result, 0, sizeof( find_result ) );
	//find_result.dwOffset=0;
	if(FindFileInDirectory( pVol, dwCurrCluster, lpFile ,pfd ,&find_result,TRUE) !=-1 ){ //success

		//insert the find struct to the head of our list.
		//����ҵ�������ļ���Ŀ¼�����ͽ���������ǵ����������û����򷵻�һ�������
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
//		dwCurCluster:	��ǰ������Ӧ�����cluster��ʼ��
//		lpFile:			�ļ�/·����
//		pFindData��		�û������ṩ�Ĵ洢�ļ���Ϣ�Ľṹ
//		pFind:			NULLʱ���������ص�ǰĿ¼/�ļ������ݿ�ʼ��cluster.
//						��NULL���������ص�ǰĿ¼/�ļ������ڵ�cluster.
//		bLongName:		Ҫ���ṩ���ļ�����һ��ΪTRUE
//		return: success: .
//				fail:	-1.
//	�˺�����dwCurCluster��ʼ�������û���Ҫ���ļ�/Ŀ¼�����Ƚ�dwCurCluster���뻺�壬����GetFileInfo
//	ȥ����һ���ļ���¼�����жϸü�¼�Ƿ�Ϊ�û���Ҫ�ģ��粻�����������GetFileInfo����������������У�
//	�������������Ѿ����꣬�͵���GetNextClusterȥ�õ���һ��cluster,ֱ���ҵ����ļ���¼��ʧ�ܡ�
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
	pFindData->nFileSizeHigh=-1; //�����Ƚ����ֵ��Ϊ��1����GetFileInfo������ҵ���һ���ļ���¼���⽫�ᱻ���0����������.
	//����������м�����ֵ��������ǣ�1��˵���ڵ�ǰcluster��û���ҵ��ļ���Ҫ��һ���µ�cluster��ʼ���ң����������һ��
	//�Ĳ��ҽ��Ϊ�����������ҡ�

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
			//lilin 2004-06-22, �ҷ����� "." �� ".."
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
//	GetFileInfo::��pDirEntryָ���buffer��,�����ļ�/Ŀ¼�������� pFindData���������������صĽṹ pEntry_Find
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
				/*2004-09-13, aysn to fatsys_whl {//����ļ�ϵͳ��д������ļ���¼ʱ�����˽�β��0����ʱ�Ϳ����ֲ���
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
// CopyShortName:::��һ��PDIR_ENTRY�ṹ�п������ļ�����csName�С�
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

		}else{  //���ļ����м�Ŀո�Ӧ�ñ�����������
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
