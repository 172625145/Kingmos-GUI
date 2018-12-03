/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：实现创建文件名记录的函数。
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙
修改记录：
******************************************************/
#include "fatfs.h"
#include "fatfmt.h"

static BOOL	DevideFileName(PUCHAR  pSrcName, int iSrcNameLen, PUCHAR pszFileName,  PUCHAR pszShortName );
static BOOL IsValidFatFileName( PUCHAR pszName );
static int	GenerateNameSufix(  PVOLUME_FAT pVol, DWORD dwCluster, PUCHAR  pszName );
static void	MakeShortName(PUCHAR pszShortName, PUCHAR pszOldName , int iNum);
static BOOL WriteNameRecord(PVOLUME_FAT pVolume, PAPPEND_RECORD pAppendRec );
static BOOL	InitDirectory( PVOLUME_FAT	pVolume, DWORD dwParentCluster, DWORD dwCluster);
static BOOL	GernerateFileName(PVOLUME_FAT, PUCHAR pszName, int  iNameLen, WCHAR* wcsLongName, PUCHAR csShortName ,PAPPEND_RECORD pAppRec);
static DWORD  AllocDirectoryCluster( PVOLUME_FAT pVol);

static void SetFileEntry(PVOLUME_FAT pVol, PDIR_ENTRY    pDirEntry, SYSTEMTIME *	pTime,  DWORD dwAttr, DWORD dwCluster, DWORD dwFileSize );


BOOL CreateFatDirectory( PVOLUME_FAT pVolume ,char* lpcszPathName )
{
	APPEND_RECORD	append;
	append.bFile=FALSE;	//不是一个文件
	append.dwAttr=0;
	append.dwFileStartClus=-1; //这里和CreateFile时一定要将这个数设为－1。

	return AddNameRec( pVolume ,(PUCHAR)lpcszPathName, &append, NULL);
}

// ******************************************************************************************************************
// AddNameRec: 创建一个目录/文件 lpszPathName，
//			pAppendRec.... bFile:TRUE,创建一个文件，FALSE，创建一个目录。
//			dwAttr.........文件目录的初始属性。
//			dwFileStartClus..必须为-1。
//			pFile......创建一个文件时会给它填入文件的相关信息
// ******************************************************************************************************************
BOOL AddNameRec( PVOLUME_FAT pVolume ,PUCHAR lpszPathName, 	PAPPEND_RECORD	pAppendRec,PFILE_FAT  pFile)
{
	FILE_FIND_DATA	 find_data;
	char			 csPathName[MAX_PATH];
	char			 csShortName[50];
	WCHAR			 wcsLongName[MAX_PATH];
	PUCHAR			 pszName;
	int				 iPathLen,i , iNameLen;
	DWORD			dwCurrCluster;
	FIND_RESULT	    find_result;
	DWORD			dwFolderDepth=0;
	BOOL			bFolderInit;

	strcpy(csPathName,lpszPathName);
	iPathLen=strlen( csPathName );

	RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec entered\r\n")));

	for( i= iPathLen-1; i>=0; i-- ){

		//if( csPathName[i] =='\\' || csPathName[i] =='/' ){
		if( IS_PATH_MARK( &csPathName[i] ) ){

			csPathName[i]=0;
			iNameLen= iPathLen- i;
			pszName= csPathName + i+1;
			break;
		}
	}
	RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec before IsValidFatFileName\r\n")));
	if( !IsValidFatFileName( pszName ) ){

		SetLastError( ERROR_INVALID_NAME );
		return FALSE;
	}

	dwCurrCluster=pVolume->fat_info.dwRootCluster;

	RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec before IsPath\r\n")));

	dwFolderDepth=0;

	while( IsPath(lpszPathName ) ){

		RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec before FindFileInDirectory %d\r\n"),dwCurrCluster));

		dwCurrCluster=FindFileInDirectory( pVolume, dwCurrCluster ,lpszPathName,&find_data, NULL, TRUE);

		if(dwCurrCluster ==-1 ){ //the path is invalid
			
			SetLastError( ERROR_PATH_NOT_FOUND );
			return FALSE;
		}

		lpszPathName=CutCurrentPath( lpszPathName );
		dwFolderDepth++;
	}

	RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec before FindFileInDirectory-0000\r\n")));

	memset( &find_result, 0, sizeof( find_result ) );
	if(FindFileInDirectory( pVolume, dwCurrCluster, lpszPathName ,&find_data ,&find_result,TRUE) !=-1 ){ //success

		SetLastError( ERROR_FILE_EXISTS );
		return FALSE;
	}
	pAppendRec->dwCluster=dwCurrCluster;

	memset( csShortName,0, sizeof( csShortName) );
	memset( wcsLongName,0, sizeof( wcsLongName) );

	RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec before Generate file name\r\n")));

	if( !GernerateFileName( pVolume,pszName, iPathLen- i-1, wcsLongName, csShortName ,pAppendRec  ) )
		return FALSE;

	if( pAppendRec->dwFileStartClus ==-1 )
		bFolderInit=TRUE;
	else
		bFolderInit=FALSE;

	RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec before CreateNameRecord\r\n")));
	if( CreateNameRecord(pVolume, pAppendRec) ){
		
		RETAILMSG( DEBUG_FILE,(TEXT("AddNameRec after CreateNameRecord\r\n")));
		if( !WriteNameRecord(pVolume, pAppendRec) ) {

			ReleaseClusterChain( pVolume , pAppendRec->dwFileStartClus);
			RETAILMSG( DEBUG_ERROR,(TEXT("WriteNameRecord failed %d is freed\r\n"),pAppendRec->dwFileStartClus));
			return FALSE;
		}
		//Now,file name is createed successfully.

		if( !pAppendRec->bFile ){ //directory,,,not a file, we should create the . & .. directory.

			if( bFolderInit ) {

				if( dwFolderDepth ==0 )
					dwCurrCluster=0;
				if(! InitDirectory( pVolume, dwCurrCluster , pAppendRec->dwFileStartClus ) )
					return FALSE;
			}
		}
		if( pFile ){

			pFile->pVolume=pVolume;

			pFile->ftCreate.dwHighDateTime =   pAppendRec->ftTime.dwHighDateTime;
			pFile->ftCreate.dwLowDateTime =   pAppendRec->ftTime.dwLowDateTime;

			pFile->ftLastAccess.dwHighDateTime =   pAppendRec->ftTime.dwHighDateTime;
			pFile->ftLastAccess.dwLowDateTime =   pAppendRec->ftTime.dwLowDateTime;

			pFile->ftLastWrite .dwHighDateTime =   pAppendRec->ftTime.dwHighDateTime;
			pFile->ftLastWrite .dwLowDateTime =   pAppendRec->ftTime.dwLowDateTime;
			strcpy( pFile->csName, pAppendRec->csFileName);
			//memcpy( pFile->csName ,pAppendRec->pwszLongName, pAppendRec->iLongNameLen +1 );
			pFile->dwAttr= pAppendRec->dwAttr;
			pFile->dwCachedCluster=-1;
			//注意：如果我们的新文件名跨越了两个 cluster,则 dwCluster 与dwLastCluster必然不同，否则，两个值是相同的，
			//而短文件名必然在后面，所以我们不用做更多的判断。
			pFile->dwClusterParent= pAppendRec->dwCluster;
			pFile->dwLongNameCluster=pAppendRec->dwLastCluster;
			
			pFile->dwLongNameOffset =pAppendRec->dwLongNameOffset;
			pFile->dwClusterOffset  =pAppendRec->dwOffsetInCluster;

			pFile->dwParentStart =dwCurrCluster;
			pFile->dwId=GetTickCount( );
			pFile->dwPointer=0;
			//pFile->dwClusIndex=0;
			pFile->dwSize=0;
			pFile->dwClusterCurrent= pAppendRec->dwFileStartClus;
			pFile->dwStartCluster = pAppendRec->dwFileStartClus;
			pFile->dwLastValidClus=pAppendRec->dwFileStartClus;
		}
		return TRUE;
	}
	return FALSE;
}
// *********************************************************************************************
// GernerateFileName :从pszName中产生长文件名和短文件名，长文件名是UNICODE编码的。
// *********************************************************************************************
BOOL	GernerateFileName(PVOLUME_FAT pVolume, PUCHAR pszName, int  iNameLen, WCHAR* wcsLongName, PUCHAR csShortName,PAPPEND_RECORD pAppRec)
{
	BOOL    bNeedLongName;
	//char	csFileName[MAX_PATH];
	char	csTemp[20];

	bNeedLongName=DevideFileName( pszName, iNameLen, pAppRec->csFileName, csShortName );

	if(pAppRec->bFile ){

		pAppRec->dwAttr |=FILE_ATTR_ARCHIVE;
		pAppRec->dwAttr &=~FILE_ATTR_DIRECTORY;
	}else{

		pAppRec->dwAttr |=FILE_ATTR_DIRECTORY;
	}
	// 2005-02-25 , remove by lilin, i test ms' code, it's ok to do the two bits 
	//pAppRec->dwAttr&=~0x3 ; // The least two bits are reserved bits,should always be 0.
	//

	pAppRec->pwszLongName=0;
	pAppRec->pwszLongName=NULL;
	pAppRec->iLongNameLen=0;

	if( bNeedLongName ){

		int	iSufixName=	GenerateNameSufix( pVolume, pAppRec->dwCluster ,csShortName);
		if( iSufixName ==-1 )
			return FALSE;  //Never go here.
		strcpy( csTemp, csShortName );
		MakeShortName(csShortName, csTemp, iSufixName );
	}
	pAppRec->pszShortName=csShortName;
	//pAppRec->pszShortName=_strupr(csShortName);

	iNameLen=MultiByteToWideChar( CP_ACP, 0, pAppRec->csFileName, strlen(pAppRec->csFileName),wcsLongName,(MAX_PATH)/2);
	//iNameLen=MultiByteToWideChar( CP_OEMCP, 0, csFileName, strlen(csFileName),wcsLongName,MAX_PATH-1);
	wcsLongName[iNameLen]=0;
	pAppRec->pwszLongName=wcsLongName;
	pAppRec->iLongNameLen=iNameLen+1;// wcslen( wcsLongName ) +1;//长文件名是一个0结尾的字符串。

	return TRUE;
}

void	MakeShortName(PUCHAR pszShortName, PUCHAR pszOldName , int iNum) 
{
	int  i , iMaxName;

	if( iNum >9 ) 
		iMaxName=5;
	else
		iMaxName=6;

	for( i=0; i< iMaxName; i++ ){

		pszShortName[i]=pszOldName[i];
	} 
	pszShortName[iMaxName]='~';

	if( iNum >9 ){

		pszShortName[iMaxName+1]=iNum/10 +'0';
		pszShortName[iMaxName+2]=iNum%10 +'0';

	}else{

		pszShortName[iMaxName+1]=iNum +'0';
	}
	for( i=8; i<=11; i++ ){ //copy the reset of the string including NULL.

		pszShortName[i]= pszOldName[i];
	}
	pszShortName[i]=0;
}


int	GenerateNameSufix(  PVOLUME_FAT pVol, DWORD dwCluster, PUCHAR  pszName ) 
{
	char	csName[20];
	FILE_FIND_DATA	 find_data;
	FIND_RESULT	 find_result;
	int		i ;

	find_result.dwCluster=dwCluster;
	find_result.dwOffset=0;
	//find_result.
	for( i=1; i<100 ;i++)
	{ 
		MakeShortName( csName, pszName , i);
		if( FindFileInDirectory( pVol, dwCluster, csName,&find_data ,&find_result,FALSE) ==-1 )
			break;
	}
	if( i>=100 )
		i=-1;

	return i;
}

BOOL IsValidFatFileName( PUCHAR pszName )
{
	int i;
	char	cNotAllowed[]={ '\\',  '/',  ':',  '*',  '?',  '\"',  '<',  '>', '|'};

	while( pszName && *pszName =='.' ) {

		pszName ++;
	}
	while( pszName && *pszName ) {

		if( *pszName < 127 ){

			for( i=0; i< sizeof( cNotAllowed); i++ ){

				if( *pszName ==cNotAllowed[i] )
					return FALSE;
			}
		}
		pszName++;
	}
	return TRUE;
}
// ***************************************************************************************************
// DevideFileName:
//		将用户给的长文件名 pSrcName　修改，使其符合FAT文件系统中文件名的格式。(切掉前后的空格和点号)
//		产生长文件名 pszFileName和短文件名 pszShortName,
//　返回值：
//			true:长文件名与短文件名是相同的，不用更多处理， false:长文件名与短文件名不相同，要产生数字后缀。
// ***************************************************************************************************
BOOL	DevideFileName(PUCHAR  pSrcName, int iSrcNameLen, PUCHAR pszFileName,  PUCHAR pszShortName )
{
	int		iLen,  i  ,k;
	PUCHAR	psDotPos;
	BOOL	bNeedNum =TRUE;


	iLen=iSrcNameLen;
	//iLen=min( 8, iSrcNameLen );
	for( i=0; i< iLen; i++, pSrcName++ ){

		if( *pSrcName !=' ' && *pSrcName !='.' ){

			break;
		}
	}
	iLen -=i;

	for( i=iLen-1; i>=0; i-- ) {
		
		if( pSrcName[i] ==' ' || pSrcName[i] =='.' ){

			pSrcName[i]=0;
			iLen=i;
		}else{
			break;
		}
	}
	iSrcNameLen=iLen;

	strcpy( pszFileName, pSrcName );
	
	for( i=0; i< iLen; i++ ) {

		if( pszFileName[i] =='.' ){

			iLen=i;
		 	break;
		}
	}
	psDotPos= &pszFileName[iLen];

	if( iLen <=8 ){

		bNeedNum=FALSE;
	}

	iLen=min( iLen, 8 );
	for( i=0; i< iLen; i++ ){

		pszShortName[i]=pSrcName[i];
	}
	pszShortName[i]='.';  //如果文件名没有扩展名，则这个点号会被下而的结尾符覆盖，所以不用其它判断。

	for( k=0; k<3; k++ ){

		if( *psDotPos ){
	
			pszShortName[++i]= *(++psDotPos);
		}else{
		 	break;
		}
	}
	if( k==3 )
		i++;
	pszShortName[i]=0;

	return bNeedNum;
}

// ******************************************************************************************************
// CreateNameRecord:创建新的文件/目录名记录
//	pAppendRec->dwCluster　为该文件的父目录之内容开始的cluster,这个函数会从这个cluster开始，寻找第一个能
//	够容纳它该文件名记录的一段空间，记录这个空间所在的位置(cluster,以及在这个cluster中的偏移).如果没有更多
//　空间，就分配一个cluster。
// ******************************************************************************************************

BOOL CreateNameRecord(PVOLUME_FAT pVolume, PAPPEND_RECORD pAppendRec) 
{
	PDIR_ENTRY			pDirEntry;
	PLONG_NAME_ENTRY	pLongEntry;
	DWORD				dwNeedRec ,dwFreeRec=0;
	DWORD				dwCurrentPos ,i;
	DWORD				dwLastCluster;
	DWORD				dwMaxOffset;
	DWORD				dwCluster=pAppendRec->dwCluster;

	dwNeedRec= (pAppendRec->iLongNameLen*2 +sizeof(DIR_ENTRY) -1 )/sizeof(DIR_ENTRY) +1;

	dwLastCluster=dwCluster;
	dwMaxOffset= pVolume->dwClusterSize / sizeof( DIR_ENTRY);

	for( ;1;){

		dwCurrentPos= CheckClusterCache(  pVolume , dwCluster );
		if( dwCurrentPos ==-1)
			return FALSE;

		pDirEntry=(PDIR_ENTRY) ( pVolume->ccClusterCache.pBufStart + dwCurrentPos * pVolume->dwClusterSize ) ;

		pLongEntry=(PLONG_NAME_ENTRY)pDirEntry;
		for( i=0;i<dwMaxOffset;  i++ ,	pDirEntry++ , pLongEntry++ ){

			if( pLongEntry->cNameOrder ==DIR_ENTRY_FREE ){
				
				dwFreeRec++;
				if( dwFreeRec >=dwNeedRec ){
					
					if( dwFreeRec >i+1 ){ //空的记录位置跨越了两个cluster(这时候i指向了第二个cluster,所以很小).

						pAppendRec->dwLastCluster =dwLastCluster;
						pAppendRec->dwCluster= dwCluster;
						pAppendRec->dwOffsetInCluster= dwMaxOffset - (dwFreeRec - i);

						goto CreateRec_Success;
					}else{

						pAppendRec->dwLastCluster =dwCluster;
						pAppendRec->dwCluster= dwCluster;
						pAppendRec->dwOffsetInCluster= i-dwNeedRec+1;

						goto CreateRec_Success;
					}
				}
			}else if( pLongEntry->cNameOrder == DIR_ENTRY_EOF){ //no more files.

				dwLastCluster= dwCluster;

				if( i+dwNeedRec > dwMaxOffset ){

					dwCluster=AllocDirectoryCluster( pVolume);

					if( dwCluster ==-1 )
						return FALSE;

					LinkCluster( pVolume,  dwLastCluster,  dwCluster );
				}
				pAppendRec->dwLastCluster =dwLastCluster;
				pAppendRec->dwCluster= dwCluster;
				pAppendRec->dwOffsetInCluster= i;

				goto CreateRec_Success;

			}else{

				dwFreeRec=0;
			}
		}
		//if( i >= dwMaxOffset ) { //Braden masked 2003-5-13,, this condition is always true.

			dwLastCluster=dwCluster;

			dwCluster=GetNextCluster( pVolume, dwCluster );

			if( INVALID_CLUSTER(pVolume,dwCluster)   )
				dwCluster=AllocDirectoryCluster(  pVolume);

			if( dwCluster ==-1)
				return FALSE;

			LinkCluster( pVolume,  dwLastCluster,  dwCluster );
		//}
	}
CreateRec_Success:
	//不论是文件还是目录，都先分配一个cluster,这样，在以后给文件写入数据时就不有去更改文件的目录记录。
	if( pAppendRec->dwFileStartClus == -1 )
	{
		pAppendRec->dwFileStartClus=FatAllocCluster(pVolume,1,NULL);
		//pAppendRec->dwFileStartClus=0;//FatAllocCluster(pVolume,1,NULL);
	}

	if(pAppendRec->dwFileStartClus ==-1)
		return FALSE;

	return TRUE;
}

// ******************************************************************************************************
// GenerateActualShortName:: 产生真正要写到磁盘上去的短文件名。
// ******************************************************************************************************
void GenerateActualShortName( PAPPEND_RECORD pAppendRec , char * pszShortName )
{
	int	i,k;
	for( i=0; i<8;i++ ){

		if( pAppendRec->pszShortName[i] =='.' || pAppendRec->pszShortName[i] ==0){

			break;
		}
		pszShortName [i]=pAppendRec->pszShortName[i];
	}
	k=i;
	for( ;i<8;i++ ){

		pszShortName [i]=' ';
	}
	for( i=8;i<11; i++  ){

		if( pAppendRec->pszShortName[k] ){

			pszShortName [i]=pAppendRec->pszShortName[++k];
			//k++;
		}else{

			pszShortName [i]=' ';
		}
	}
}
// ******************************************************************************************************
// WriteNameRecord:: 将文件/目录名写入到磁盘上。
// ******************************************************************************************************
BOOL WriteNameRecord(PVOLUME_FAT pVolume, PAPPEND_RECORD pAppendRec ) 
{
	DWORD  i ,dwMaxOffset ,dwCurNameIndex ;
	PLONG_NAME_ENTRY	pLongEntry;
	PDIR_ENTRY			pDirEntry;
	BOOL	bFirst=TRUE;
	UCHAR	ucCheckSum;
	char	csShortName[20];
	DWORD	dwCurrentPos;
	DWORD	dwNameLast;
	DWORD	dwNameCurr;

	dwCurrentPos= CheckClusterCache(  pVolume ,  pAppendRec->dwLastCluster);
	if( dwCurrentPos ==-1){
		RETAILMSG( DEBUG_ERROR,(TEXT("WriteNameRecord.. can't cache cluster aaaaaaaa\r\n")));
		return FALSE;
	}

	pDirEntry =(PDIR_ENTRY) ( pVolume->ccClusterCache.pBufStart + dwCurrentPos * pVolume->dwClusterSize ) ;
	pDirEntry +=pAppendRec->dwOffsetInCluster;
	pLongEntry=(PLONG_NAME_ENTRY)pDirEntry;

	//dwCurNameIndex=( pAppendRec->iLongNameLen *2 +sizeof(DIR_ENTRY) -1 )/sizeof(DIR_ENTRY);
	dwCurNameIndex=  ( pAppendRec->iLongNameLen *2 + NAME_SIZE_ONE_ENTRY -1 )/NAME_SIZE_ONE_ENTRY;
	dwNameLast= (pAppendRec->iLongNameLen*2) % NAME_SIZE_ONE_ENTRY;

	dwMaxOffset=pVolume->dwClusterSize/sizeof( DIR_ENTRY );
	//dwMaxOffset= min( dwMaxOffset, pAppendRec->dwOffsetInCluster + dwCurNameIndex);

	GenerateActualShortName( pAppendRec , csShortName );
	ucCheckSum=ShortNameCheckSum( (UCHAR*)csShortName );

	pAppendRec->dwLongNameOffset=pAppendRec->dwOffsetInCluster;

	do{
		for( i= pAppendRec->dwOffsetInCluster; i< dwMaxOffset &&dwCurNameIndex ; i++ ,pLongEntry++ , pDirEntry++, dwCurNameIndex-- ){

			pLongEntry->cAttr=FILE_ATTR_LONG_NAME;
			pLongEntry->cChecksum=ucCheckSum;
			pLongEntry->cType=0;
			pLongEntry->cNameOrder= (UCHAR)dwCurNameIndex;
			pLongEntry->wFirstClusterLow=0;

			if( bFirst ){
 
				if( dwNameLast ==0 )
					dwNameLast=NAME_SIZE_ONE_ENTRY;

				bFirst=FALSE;
				pLongEntry->cNameOrder |= LAST_LONG_ENTRY ;

				memset(pLongEntry->cLongName1, 0xFF, 10);
				memset(pLongEntry->cLongName2, 0xFF, 12);
				memset(pLongEntry->cLongName3, 0xFF, 4);

				dwNameCurr=min( 10, dwNameLast );
				memcpy(pLongEntry->cLongName1, pAppendRec->pwszLongName+(dwCurNameIndex-1)*NAME_SIZE_WORD, dwNameCurr);

				DEBUGMSG( DEBUG_MOUNT,(TEXT("WriteNameRecord..name1 %s (%d %d)\r\n"),
							pLongEntry->cLongName1 ,dwNameCurr ,dwNameLast));
				
				dwNameLast-=dwNameCurr;
				dwNameCurr=min( 12, dwNameLast );
				memcpy(pLongEntry->cLongName2, pAppendRec->pwszLongName+(dwCurNameIndex-1)*NAME_SIZE_WORD +10/2 , dwNameCurr);

				DEBUGMSG( DEBUG_MOUNT,(TEXT("WriteNameRecord..name2 %s (%d %d)\r\n"),
							pLongEntry->cLongName2 ,dwNameCurr ,dwNameLast));
							
				dwNameLast-=dwNameCurr;
				dwNameCurr=min( 4, dwNameLast );
				//memcpy(pLongEntry->cLongName3, pAppendRec->pwszLongName+(dwCurNameIndex-1)+(10+12)/2 , dwNameCurr);
				memcpy(pLongEntry->cLongName3, pAppendRec->pwszLongName+(dwCurNameIndex-1)*NAME_SIZE_WORD+(10+12)/2 , dwNameCurr);
				
				DEBUGMSG( DEBUG_MOUNT,(TEXT("WriteNameRecord..name3 %s (%d %d)\r\n"),
							pLongEntry->cLongName1 ,dwNameCurr ,dwNameLast));


			}else{
				memcpy(pLongEntry->cLongName1, pAppendRec->pwszLongName+(dwCurNameIndex-1)*NAME_SIZE_WORD , 10);
				memcpy(pLongEntry->cLongName2, pAppendRec->pwszLongName+(dwCurNameIndex-1)*NAME_SIZE_WORD+10/2, 12);
				memcpy(pLongEntry->cLongName3, pAppendRec->pwszLongName+(dwCurNameIndex-1)*NAME_SIZE_WORD+(10+12)/2 ,4);
			}
		}
		if(	i>= dwMaxOffset ){

			if(! WriteCluster(pVolume, dwCurrentPos,  FLUSH_CLUSTER) ){

				ERRORMSG( DEBUG_ERROR,(TEXT("WriteNameRecord.. can't write cluster \r\n")));
				return FALSE;
			}

			dwCurrentPos= CheckClusterCache(  pVolume ,  pAppendRec->dwCluster);
			if( dwCurrentPos ==-1){

				ERRORMSG( DEBUG_ERROR,(TEXT("WriteNameRecord.. can't cache cluster!!!!.\r\n")));
				return FALSE;
			}

			pAppendRec->dwOffsetInCluster=0;

			pDirEntry=(PDIR_ENTRY) ( pVolume->ccClusterCache.pBufStart + dwCurrentPos * pVolume->dwClusterSize ) +pAppendRec->dwOffsetInCluster;
			pLongEntry=(PLONG_NAME_ENTRY)pDirEntry;
			i=0;
		}
		if( dwCurNameIndex==0 ){ ///write short name recorder;

			SYSTEMTIME	sys_time;

			GetLocalTime( &sys_time);
			SystemTimeToFileTime(&sys_time,&pAppendRec->ftTime);
			SetFileEntry( pVolume, pDirEntry, &sys_time, pAppendRec->dwAttr,  pAppendRec->dwFileStartClus, 0 );
			memcpy( pDirEntry->cShortName, csShortName ,11 );
			//write dwCluster.
			break;
		}
	}while( dwCurNameIndex);

	pAppendRec->dwOffsetInCluster=i;

	if(! WriteCluster(pVolume, dwCurrentPos,  FLUSH_CLUSTER) ){

		ERRORMSG( DEBUG_ERROR,(TEXT("WriteNameRecord.. can't write cluster\r\n")));
		return FALSE;
	}
	return TRUE;
}

// ******************************************************************************************************
// InitDirectory:::一个目录之内容的前两个记录分别为.与..目录。这个函数生成这两个目录。
//	Cluster:　　　这个目录内容开始的cluster.
//  dwParentCluster:该目录的父目录所在的cluster.
// ******************************************************************************************************
BOOL	InitDirectory( PVOLUME_FAT	pVolume, DWORD dwParentCluster, DWORD dwCluster)
{
	PDIR_ENTRY     pDirEntry;
	char	csName[]="..         ";
	SYSTEMTIME	sys_time;
	DWORD		dwPos;

	dwPos=CheckClusterCache(  pVolume ,  dwCluster);
	if( dwPos ==-1 )
		return FALSE;

	memset( pVolume->ccClusterCache.pBufStart+ dwPos*pVolume->dwClusterSize,
			0,
			pVolume->dwClusterSize );
	GetLocalTime( &sys_time);
	
	pDirEntry=(PDIR_ENTRY ) (pVolume->ccClusterCache.pBufStart+dwPos*pVolume->dwClusterSize);

	SetFileEntry( pVolume, pDirEntry, &sys_time, FILE_ATTR_DIRECTORY , dwCluster, 0 );
	memcpy( pDirEntry->cShortName, csName, 11 );
	pDirEntry->cShortName[1]=' ';

	pDirEntry++;
	SetFileEntry(pVolume,  pDirEntry, &sys_time, FILE_ATTR_DIRECTORY , dwParentCluster, 0 );
	memcpy( pDirEntry->cShortName, csName, 11 );

	//bSuccess=WriteCluster(pVolume, dwCluster,  pBufTemp, pVolume->dwClusterSize);
	if(! WriteCluster(pVolume, dwPos,  FLUSH_CLUSTER) ){

		return FALSE;
	}
	return TRUE;
}


static void SetFileEntry(PVOLUME_FAT pVol, PDIR_ENTRY    pDirEntry, SYSTEMTIME *	pTime,  DWORD dwAttr, DWORD dwCluster, DWORD dwFileSize  )
{
	if( dwCluster >0 )
		dwCluster-= pVol->dwRootDirClusterNum;

    pDirEntry->cAttr= (UCHAR)dwAttr;
	pDirEntry->cRsved=0;

	pDirEntry->cTimeTenth=0;
	pDirEntry->dwFileSize=dwFileSize;

	pDirEntry->mDateCreated.nDay	=pTime->wDay;
	pDirEntry->mDateCreated.nMonth	=pTime->wMonth;
	pDirEntry->mDateCreated.nYearCount= pTime->wYear-1980;

	pDirEntry->mDateLastWrite.nDay	  =pTime->wDay;
	pDirEntry->mDateLastWrite.nMonth  =pTime->wMonth;
	pDirEntry->mDateLastWrite.nYearCount  =pTime->wYear -1980;

	pDirEntry->mLastAccessDate.nDay		=pTime->wDay;
	pDirEntry->mLastAccessDate.nMonth	=pTime->wMonth;
	pDirEntry->mLastAccessDate.nYearCount=pTime->wYear -1980;

	pDirEntry->mTimeCreated.nHour=pTime->wHour ;
	pDirEntry->mTimeCreated.nMinute=pTime->wMinute;
	pDirEntry->mTimeCreated.nTwiceSecond=pTime->wSecond/2;

	pDirEntry->mTimeLastWrite.nHour		= pTime->wHour;
	pDirEntry->mTimeLastWrite.nMinute	= pTime->wMinute;
	pDirEntry->mTimeLastWrite.nTwiceSecond= pTime->wSecond/2;

	pDirEntry->wFirstClusterHigh= (WORD)(( dwCluster >>16)  &0xFFFF );
	pDirEntry->wFirstClusterLow = (WORD)dwCluster;
}



BOOL DeleteFatDirectory(PVOLUME_FAT pVolume, PUCHAR pszPathName)
{
	char	szPath[MAX_PATH];
	FILE_FIND_DATA	 find_data;
	BOOL	bSuccess=FALSE;
	PFIND_RESULT	 pFind;
	HANDLE	hFile;
	LPSTR	lpName;
	
	lpName=GetFileNameFromPath( pszPathName);
	if( lpName[0] =='.')
		return FALSE;

	strcpy( szPath, pszPathName);
	strcat( szPath, "\\*.*");
	pFind =(PFIND_RESULT)FindFile(  pVolume, szPath, &find_data);
	if( pFind !=INVALID_HANDLE_VALUE ){

		do{

			if( find_data.cFileName[0]!='.'){

				SetLastError( ERROR_DIR_NOT_EMPTY );
				goto DeleteFolderEnd;
			}

		}while( FindFileInDirectory( pVolume, pFind->dwCluster, NULL,&find_data ,pFind,TRUE) !=-1 );

		DeleteSearchHandle( pVolume, pFind );
	}
	hFile=CreateFatFile( pVolume, pszPathName,0, 0, OPEN_EXISTING,0, FALSE);

	if( hFile != INVALID_HANDLE_VALUE) {

		bSuccess=FatDeleteFileByHanle( hFile ,TRUE);
		// remove by lilin, 2004-08-09
		//if( bSuccess ) {
		//	DeleteFileHandle( pVolume, (PFILE_FAT) hFile ,FALSE);
		//}
		//
		DeleteFileHandle( pVolume, (PFILE_FAT) hFile ,FALSE);
	}
	return bSuccess;
DeleteFolderEnd:
	DeleteSearchHandle( pVolume, pFind );
	return bSuccess;
}
// ******************************************************************************************************
//AllocDirectoryCluster::::为一个目录分配一个新的cluster,与文件的一个cluster不同，directory没有表示其大小
//	的记录，所以我们在分配一个cluster以后要将其内容全部置为0,否则，我们会在这个目录下看到很多奇怪的文件。
//	而对文件分配的cluster不需要置0。
// ******************************************************************************************************

DWORD  AllocDirectoryCluster( PVOLUME_FAT pVol)
{
	DWORD dwCluster=-1;
	DWORD dwPos;

	dwCluster=FatAllocCluster(pVol,1,NULL);

	if( dwCluster !=-1 ){

		//char * pBuf=(char * )malloc(pVol->dwClusterSize);
		dwPos=CheckClusterCache(  pVol , dwCluster );
		if( dwPos ==-1 )
			return -1;

		memset( pVol->ccClusterCache.pBufStart+ dwPos*pVol->dwClusterSize , 
			    0,
				pVol->dwClusterSize );

		if(! WriteCluster(pVol, dwPos,  FLUSH_CLUSTER) ){

			return -1;
		}
	}
	return dwCluster;
}
