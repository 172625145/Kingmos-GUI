/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：FAT文件系统中用到的一些小函数。
版本号：1.0.0
开发时期：2003-4-18
作者：魏海�
修改记录：
	2004-09-13, CheckFATCache del, lilin, 移到开始，没有必要每次都调用（浪费时间）
	2004-08-03, 将 if(*lpFile == '\\' || *lpFile=='/' ) 换为 if( IS_PATH_MARK( lpFile ) )
******************************************************/

#include "fatfs.h"
#include "fatfmt.h"
#include "eshell.h"
static DWORD	CheckFATCache( PVOLUME_FAT pFatVol , DWORD dwSector );
// ************************************************************************************************************
// GetClusterInFAT :: 计算一个给定的cluster在FAT表中的位置(扇区编号及在这个扇区中的偏移)，
// ************************************************************************************************************
static BOOL  GetClusterInFAT( PVOLUME_FAT pVolume , DWORD dwCluster ,DWORD * pdwSectorNum, DWORD * pdwOffsetInSec)
{
	DWORD	dwOffset;

	if( dwCluster < pVolume->dwRootDirClusterNum + 2 ){
		ASSERT( 0 );

		return FALSE;
	}
	//2004-08-25, modify by lilin
	//if( dwCluster > pVolume->fat_info.dwClusterCount ){
	if( dwCluster >= pVolume->fat_info.dwClusterCount + 2 ){
		//
		RETAILMSG( 1, ( "warn in GetClusterInFAT:dwCluster(%d) >= pVolume->fat_info.dwClusterCount(%d) + 2", dwCluster, pVolume->fat_info.dwClusterCount ) );//ASSERT( 0 );
		return FALSE;
	}

	dwCluster-=pVolume->dwRootDirClusterNum;

	if( pVolume->dwVersion ==FAT_VERSION16  ){

		dwOffset=dwCluster*2;
	}else if ( pVolume->dwVersion ==FAT_VERSION32  ){

		dwOffset=dwCluster*4;
	}else if( pVolume->dwVersion ==FAT_VERSION12  ){
		dwOffset = dwCluster + (dwCluster / 2);  //special
	}
	*pdwSectorNum  = dwOffset/pVolume->fat_info.dwBytesPerSec + pVolume->fat_info.dwRsvedSector;
	*pdwOffsetInSec = dwOffset % pVolume->fat_info.dwBytesPerSec;
	return TRUE;
}
// ************************************************************************************************************
// FetchClusEntryVal :: 从pcSecBuff指向的buffer中, 在dwOffsetInSec位置，取出
//
//它通常要与GetClusterInFAT联合使用，先用GetClusterInFAT得到一个cluster在FAT表中的位置（扇区与在扇区中的偏移），
//再将该扇区的内容读入sector buffer,然后调用FetchClusEntryVal去取值。
//该函数很简单，只是需要区分FAT16与FAT32，并且在读出的值为0或EOC（0x0FFFFFFF）时不加dwRootDirClusterNum，
//就是说dwRootDirClusterNum只是在读出的值为一个正常的cluster编号时才加dwRootDirClusterNum。
// ************************************************************************************************************
static DWORD FetchClusEntryVal( PVOLUME_FAT pVolume, 
							    DWORD dwClus,   // Cluster 号
								DWORD dwFATSector, //当前的 pcSecBuff 所对应的 FAT的磁盘Sector
								char *pcSecBuff, 
								DWORD dwOffsetInSec )
{
	DWORD dwFATClusEntryVal;

	if( pVolume->dwVersion ==FAT_VERSION16  ){

		dwFATClusEntryVal = *((WORD *) &pcSecBuff[dwOffsetInSec]);

		if( dwFATClusEntryVal == FAT_CLUSTER_EOC16 )
			return dwFATClusEntryVal;
	}else if( pVolume->dwVersion ==FAT_VERSION32 ){

		dwFATClusEntryVal = (*((DWORD *) &pcSecBuff[dwOffsetInSec])) & 0x0FFFFFFF;

		if( dwFATClusEntryVal == FAT_CLUSTER_EOC )
			return dwFATClusEntryVal;
	}
	//lilin add-2004-09-13-begin
	else if( pVolume->dwVersion == FAT_VERSION12 ){
		dwClus -= pVolume->dwRootDirClusterNum;
		if( dwOffsetInSec == (pVolume->fat_info.dwBytesPerSec - 1)) {
			/* This cluster access spans a sector boundary in the FAT */
			/* There are a number of strategies to handling this. The */
			/* easiest is to always load FAT sectors into memory */
			/* in pairs if the volume is FAT12 (if you want to load */
			/* FAT sector N, you also load FAT sector N+1 immediately */
			/* following it in memory unless sector N is the last FAT */
			/* sector). It is assumed that this is the strategy used here */
			/* which makes this if test for a sector boundary span */
			/* unnecessary. */
			DWORD pos;
			LPBYTE lpNextBuf;
			dwFATClusEntryVal = *( (BYTE*)&pcSecBuff[dwOffsetInSec] );
			if( ( pos = CheckFATCache( pVolume, dwFATSector+1 ) ) == -1 )
			{
				ERRORMSG( 1, ( "FetchClusEntryVal: failure, can't get dwFATSector(=0x%x)!!!.\r\n",dwFATSector ) );
				return -1;
			}
			lpNextBuf = pVolume->scFatCache.pBuf + pos * pVolume->fat_info.dwBytesPerSec;
			dwFATClusEntryVal |= (*lpNextBuf) << 8;
		}
		else{
			dwFATClusEntryVal = ( *( (BYTE*)&pcSecBuff[dwOffsetInSec+1] ) << 8 ) | *( (BYTE*)&pcSecBuff[dwOffsetInSec] );
		}
			//FAT12ClusEntryVal = fatEnryVal;//*((WORD *) &SecBuff[ThisFATEntOffset]);
		if( dwClus & 0x0001 )
			dwFATClusEntryVal = dwFATClusEntryVal >> 4; /* Cluster number is ODD */
		else
			dwFATClusEntryVal = dwFATClusEntryVal & 0x0FFF; /* Cluster number is EVEN */
	}
	//lilin add-2004-09-13-end
	else{
		ASSERT(0);
	}
	//return dwFATClusEntryVal ;
	//Modified by Braden,2003-4-30... the adjust value for FAT16 root dir cluster number should be plused,
	if(dwFATClusEntryVal == 0 )
		return 0;

	return dwFATClusEntryVal +pVolume->dwRootDirClusterNum;
}
// ************************************************************************************************************
// SetClusEntryVal :: 在由iIndex指向的sector buffer中的dwOffsetInSec位置，写入数值dwValWrite.
//
//它的原型本来是下面的一个函数，和FetchClusEntryVal比较类似，但为了pVolume->scFatCache.bNeedFlush[iIndex]设为
//TRUE，就改成了现在这个样子，同样，只有dwValWrite为一个合法的cluster编号时，才将它的值加一个　dwRootDirClusterNum，
//而且必须加这个值，写入硬件。
// ************************************************************************************************************
static void	SetClusEntryVal( PVOLUME_FAT pVolume, 
							 DWORD dwClus,
							 DWORD dwFATSector,
							 int iIndex, 
							 DWORD dwOffsetInSec ,
							 DWORD dwValWrite)
{
	CHAR *pcSecBuff= pVolume->scFatCache.pBuf+iIndex* pVolume->fat_info.dwBytesPerSec;
	DWORD	dwVal;

	dwVal=dwValWrite;

	if( dwValWrite != 0 ){

		if( dwValWrite < pVolume->dwRootDirClusterNum ){

			return ;
		}
		if( dwValWrite !=FAT_CLUSTER_EOC ){

			dwVal=dwValWrite-pVolume->dwRootDirClusterNum;
		}
	}

	if( pVolume->dwVersion ==FAT_VERSION16  ){
		*((WORD *) &pcSecBuff[dwOffsetInSec]) = (WORD)dwVal;
	}else if( pVolume->dwVersion ==FAT_VERSION32  ){
		dwVal = dwVal & 0x0FFFFFFF;

		*((DWORD *) &pcSecBuff[ dwOffsetInSec ]) =
		(*((DWORD *) &pcSecBuff[ dwOffsetInSec ])) & 0xF0000000;
		*((DWORD *) &pcSecBuff[ dwOffsetInSec]) =
		(*((DWORD *) &pcSecBuff[ dwOffsetInSec ])) | dwVal;
	}
	//2004-09-13, add by lilin-begin
	else if( pVolume->dwVersion ==FAT_VERSION12  ){
		LPBYTE lpNextBuf = NULL;
		DWORD dwNextIndex;
		//WORD wVal = dwVal;
		dwClus -= pVolume->dwRootDirClusterNum;
		if( dwOffsetInSec == (pVolume->fat_info.dwBytesPerSec - 1)) {
			/* This cluster access spans a sector boundary in the FAT */
			/* There are a number of strategies to handling this. The */
			/* easiest is to always load FAT sectors into memory */
			/* in pairs if the volume is FAT12 (if you want to load */
			/* FAT sector N, you also load FAT sector N+1 immediately */
			/* following it in memory unless sector N is the last FAT */
			/* sector). It is assumed that this is the strategy used here */
			/* which makes this if test for a sector boundary span */
			/* unnecessary. */
			
			
			if( ( dwNextIndex = CheckFATCache( pVolume, dwFATSector+1 ) ) == -1 )
			{
				ERRORMSG( 1, ( "SetClusEntryVal: failure, can't get dwFATSector(=0x%x)!!!.\r\n",dwFATSector ) );
				return ;
			}
			lpNextBuf = pVolume->scFatCache.pBuf + dwNextIndex * pVolume->fat_info.dwBytesPerSec;
		}

		if(dwClus & 0x0001) {
			dwVal = dwVal << 4; /* Cluster number is ODD */
			//*((WORD *) &SecBuff[ThisFATEntOffset]) =
				//(*((WORD *) &SecBuff[ThisFATEntOffset])) & 0x000F;
			pcSecBuff[dwOffsetInSec] &= 0x0F;
			if( lpNextBuf )
				*lpNextBuf &= 0x00;
			else
				pcSecBuff[dwOffsetInSec+1] &= 0x00;
		} else {
			dwVal = dwVal & 0x0FFF; /* Cluster number is EVEN */
			//*((WORD *) &SecBuff[ThisFATEntOffset]) =
				//(*((WORD *) &SecBuff[ThisFATEntOffset])) & 0xF000;
			pcSecBuff[dwOffsetInSec] &= 0x00;
			if( lpNextBuf )
				*lpNextBuf &= 0xF0;
			else
				pcSecBuff[dwOffsetInSec+1] &= 0xF0;
		}
		//*((WORD *) &SecBuff[ThisFATEntOffset]) =
			//(*((WORD *) &SecBuff[ThisFATEntOffset])) | FAT12ClusEntryVal;
		pcSecBuff[dwOffsetInSec] |= ((BYTE)dwVal);
		if( lpNextBuf )
			*lpNextBuf |= ( (BYTE)(dwVal>>8) );
		else
			pcSecBuff[dwOffsetInSec+1] |= ( (BYTE) (dwVal >> 8 ));
		if( lpNextBuf )
		{
			pVolume->scFatCache.bNeedFlush[dwNextIndex]= TRUE;
		}
	}
	//2004-09-13, add by lilin-end
	pVolume->scFatCache.bNeedFlush[iIndex]= TRUE;
	pVolume->bFlushed=FALSE;

	pVolume->dwLastWriteTime=GetTickCount( );
	return;
}
/*
void	SetClusEntryVal( PVOLUME_FAT pVolume, CHAR *pcSecBuff, DWORD dwOffsetInSec ,DWORD dwVal)
{
	if( pVolume->dwVersion ==FAT_VERSION16  ){
		*((WORD *) &pcSecBuff[dwOffsetInSec]) = (WORD)dwVal;
	}else {

		dwVal = dwVal & 0x0FFFFFFF;

		*((DWORD *) &pcSecBuff[ dwOffsetInSec ]) =
		(*((DWORD *) &pcSecBuff[ dwOffsetInSec ])) & 0xF0000000;
		*((DWORD *) &pcSecBuff[ dwOffsetInSec]) =
		(*((DWORD *) &pcSecBuff[ dwOffsetInSec ])) | dwVal;
	}
	pVolume->dwLastWriteTime=GetTickCount( );
	return;
}
*/

// ************************************************************************************************************
// IsEndOfCluster:: 给定一个数dwContent,判断它是否为一个文件的结尾cluster（如果不能理解这个函数，请查FAT规范）
// ************************************************************************************************************

static BOOL	IsEndOfCluster( PVOLUME_FAT  pVolume, DWORD dwContent)
{
	BOOL  IsEOF = FALSE;
	if(pVolume->dwVersion== FAT_VERSION16) {

		if(dwContent>= 0xFFF8)
			IsEOF = TRUE;
	} else if ( pVolume->dwVersion == FAT_VERSION32) {

		if(dwContent>= 0x0FFFFFF8)
			IsEOF = TRUE;
	} else if(pVolume->dwVersion ==FAT_VERSION12) {

		if(dwContent >= 0x0FF8)
			IsEOF = TRUE;
	}
	return IsEOF;
}

// ************************************************************************************************************
// InitializeCaches:: 初始化buffer(sector buffer 与cluster buffer),它只在系统启动时调用一次，
//　sector buufer 为FAT表中的最前面DISK_CACHE_SIZES个sector的内容,
//  cluster buffer为磁盘根目录开始的CLUSTER_CACHE_SIZES个cluster的内容.
// ************************************************************************************************************
BOOL	InitializeCaches( PVOLUME_FAT pVol )
{
	int i;
	
	//if( pVol->scFatCache.dwStartSector ==-1 ) 
	{

		if( FSDMGR_ReadDisk(pVol->hDisk, pVol->fat_info.dwRsvedSector, DISK_CACHE_SIZES,
						  pVol->scFatCache.pBuf, DISK_CACHE_SIZES*pVol->fat_info.dwBytesPerSec) )

		  return FALSE;

		for( i=0; i< DISK_CACHE_SIZES; i++ ){

			pVol->scFatCache.dwSector[i]= pVol->fat_info.dwRsvedSector+i;
			pVol->scFatCache.dwLastAccess[i]=GetTickCount();
			pVol->scFatCache.bNeedFlush[i]=FALSE;
		}
	}
	//if( pVol->ccClusterCache.dwStartCluster==-1 )
	{ //Now we should try to read out all root data to buffer.

		//2004-09-15, lilin 该部分功能在我调试14M(FAT12)的时候可能有错，所以我在初始化时不去读出
		/* 

		if( FSDMGR_ReadDisk(pVol->hDisk, pVol->dwRootDirSec, CLUSTER_CACHE_SIZES*pVol->fat_info.dwSecPerCluster,
						   pVol->ccClusterCache.pBufStart,  pVol->dwClusterSize * CLUSTER_CACHE_SIZES) )

		  return FALSE;
		for( i=0; i< CLUSTER_CACHE_SIZES; i++ ){

			pVol->ccClusterCache.dwCluster[i]=pVol->fat_info.dwRootCluster+i;
			pVol->ccClusterCache.dwLastAccess[i]=GetTickCount( );
			pVol->ccClusterCache.bNeedFlush[i]=FALSE;
		}
		*/
		for( i=0; i< CLUSTER_CACHE_SIZES; i++ ){

			pVol->ccClusterCache.dwCluster[i]=-1;
			pVol->ccClusterCache.dwLastAccess[i]=0;
			pVol->ccClusterCache.bNeedFlush[i]=FALSE;
		}
		// 2004-09-15-end


	}
	return TRUE;
}
// ************************************************************************************************************
// CheckFATCache:	给定一个sector 编号　dwSector，返回它是在scFatCache.dwSector中的第几个或错误－1，
// ************************************************************************************************************
static DWORD	CheckFATCache( PVOLUME_FAT pFatVol , DWORD dwSector )
{
	DWORD iMinAccess=0;
	int		i;
	DWORD dwCount;
	//2004-09-13， add
	LPDWORD lpdwSectorIndex = pFatVol->scFatCache.dwSector;
	LPDWORD lpdwLastAccessIndex = pFatVol->scFatCache.dwLastAccess;
	LPDWORD lpdwLastAccess, lpdwMinAccess;
	LPDWORD lpdwSector;
	//

	//2004-09-13， add
	dwCount=GetTickCount();
	lpdwLastAccess = lpdwMinAccess = lpdwLastAccessIndex;
	lpdwSector = lpdwSectorIndex;
	//

	for( i=0; i< DISK_CACHE_SIZES; i++, lpdwLastAccess++, lpdwSector++ ){

		//2004-09-13, del-begin
		//if( pFatVol->scFatCache.dwSector[i]== dwSector )
			//break;
		if( *lpdwSector == dwSector )
			break;
		//end

		//2004-09-13, del, lilin, 移到开始，没有必要每次都调用（浪费时间）
		//dwCount=GetTickCount();
		//2004-09-13, del, lilin-end

		//2004-09-13, del-begin
		//if( dwCount- pFatVol->scFatCache.dwSector[i] > dwCount -pFatVol->scFatCache.dwSector[iMinAccess]  )
			//iMinAccess=i;
		if( dwCount - *lpdwLastAccess > dwCount - *lpdwMinAccess  )
		{
			iMinAccess=i;
			lpdwMinAccess = lpdwLastAccess;
		}
		//end
	}
	if( i>=DISK_CACHE_SIZES ){

		i=iMinAccess;  //return value
		if( pFatVol->scFatCache.bNeedFlush[iMinAccess] ){
			WriteFatSector( pFatVol, iMinAccess);
		}
		if(FSDMGR_ReadDisk( pFatVol->hDisk, dwSector, 1, 
							pFatVol->scFatCache.pBuf + iMinAccess*pFatVol->fat_info.dwBytesPerSec
							, pFatVol->fat_info.dwBytesPerSec) )
			return -1;
		//2004-09-13, del-begin		
		//pFatVol->scFatCache.dwSector[i]=dwSector;
		//pFatVol->scFatCache.dwLastAccess[i]= GetTickCount( );		
		pFatVol->scFatCache.dwSector[iMinAccess] = dwSector;
		*lpdwMinAccess = GetTickCount();
		//end
	}
	else
	{
		//pFatVol->scFatCache.dwLastAccess[i]= dwCount;
		*lpdwLastAccess = dwCount;
	}
	return i;
}

// ************************************************************************************************************
// CheckFATCache:	给定一个cluster 编号　dwCluster，返回它是在ccClusterCache.dwCluster中的第几个或错误－1，
//　函数中的注释请参照CheckFATCache
// ************************************************************************************************************
DWORD	CheckClusterCache( PVOLUME_FAT pVolume , DWORD dwCluster )
{
	int	i, iMinAcc=0;
	LPDWORD lpdwCluster;
	LPDWORD lpdwLastAccess;
	LPDWORD lpdwMinAccess;
	DWORD dwCurTickCount;

	//2004-08-25, lilin
	//if( dwCluster > pVolume->fat_info.dwClusterCount )
	if( dwCluster >= pVolume->fat_info.dwClusterCount + 2 )
	//
	{
		ASSERT( 0 );
		return -1;
	}
	lpdwCluster = pVolume->ccClusterCache.dwCluster;
	lpdwMinAccess = lpdwLastAccess = pVolume->ccClusterCache.dwLastAccess;
	dwCurTickCount = GetTickCount();

	for( i=0, iMinAcc=0; i< CLUSTER_CACHE_SIZES; i++, lpdwLastAccess++, lpdwCluster++ ){

		if( *lpdwCluster ==dwCluster )
			break;

		if( dwCurTickCount - *lpdwLastAccess > dwCurTickCount - *lpdwMinAccess )
		{
			iMinAcc =i;
			lpdwMinAccess = lpdwLastAccess;
		}
	}
	if( i>= CLUSTER_CACHE_SIZES ){  //the cluster is not cached.

		i=iMinAcc;	//return ;
		if( pVolume->ccClusterCache.bNeedFlush[iMinAcc] ){

			/*WriteCluster(pVolume, pVolume->ccClusterCache.dwCluster[iMinAcc],
						 pVolume->ccClusterCache.pBufStart+ iMinAcc*pVolume->dwClusterSize ,
						 pVolume->dwClusterSize);

			*/
			WriteCluster( pVolume, iMinAcc,TRUE);
			//pVolume->ccClusterCache.bNeedFlush[iMinAcc]=FALSE;
		}
		if( !ReadCluster( pVolume, dwCluster,
							pVolume->ccClusterCache.pBufStart +iMinAcc*pVolume->dwClusterSize,
							pVolume->dwClusterSize
							) )
			return -1;
		//pVolume->ccClusterCache.dwLastAccess[i]=GetTickCount( );
		*lpdwMinAccess = GetTickCount( );
		pVolume->ccClusterCache.dwCluster[iMinAcc]=dwCluster;
	}
	else
	{
		//pVolume->ccClusterCache.dwLastAccess[i]=dwCurTickCount;
		*lpdwLastAccess = dwCurTickCount;
	}
	//pVolume->ccClusterCache.dwCluster[i]=dwCluster;
	return i;
}


// ************************************************************************************************************
// GetNextCluster:	给定一个cluster 编号　dwCurCluster，返回它的下一个cluster(在一个文件/目录内容的FAT链中)，
//
// ************************************************************************************************************
DWORD GetNextCluster( PVOLUME_FAT pFatVol, DWORD dwCurCluster )
{
	DWORD dwSector;
	DWORD dwOffset;
	DWORD dwPos;
	DWORD dwClus;

	if( dwCurCluster <  pFatVol->dwRootDirClusterNum )//所有的cluster在我们的系统中都被加了一个dwRootDirClusterNum，
		return  dwCurCluster +1 ;//所以，如果cluster的编号比dwRootDirClusterNum还小，这一定不是一个合法的cluster编号,
	//这种情况只会在FAT16中出现（在FAT32中dwRootDirClusterNum是0），这说明所给的cluster编号dwCurCluster，不是磁盘上一个
	//真正的cluster,只是根目录内容所占的位置，只需将其加1让它指向下一个cluster即可。
	else if( dwCurCluster >=  pFatVol->dwRootDirClusterNum +2 ){

		if(! GetClusterInFAT( pFatVol, dwCurCluster, &dwSector, &dwOffset) )
			return -1;
		
		if( (dwPos= CheckFATCache (pFatVol,dwSector)  )==-1 )
			return -1;

		dwClus= FetchClusEntryVal(	pFatVol,
									dwCurCluster,
									dwSector,
									pFatVol->scFatCache.pBuf + dwPos*pFatVol->fat_info.dwBytesPerSec,
									dwOffset);
		if( dwClus==0 )
			dwClus=-1;

		return dwClus;

	}else	//系统出错
		return -1;
}

// ************************************************************************************************************
// GetContinuesCluster:	给定一个cluster 编号　dwFromClus，返回从这个cluster开始的，连续的cluster数目（最多dwNum个），
//　并将下一个在这些连续的cluster之后的第一个cluster的编号放在pdwNextRead中
// ************************************************************************************************************
DWORD	GetContinuesCluster( PVOLUME_FAT	pVol, DWORD dwFromClus ,DWORD dwNum ,LPDWORD pdwNextRead)
{
	DWORD	i=0;

	for( i=0; i< dwNum; i++ ){

		*pdwNextRead=GetNextCluster(  pVol,  dwFromClus+i);

		if( *pdwNextRead ==-1 )
			return -1;
		if(  *pdwNextRead != dwFromClus+i+1)
			break;
	}
	if( *pdwNextRead == 0 )
		*pdwNextRead = -1;//FAT_CLUSTER_EOC;

	if( i< dwNum )
		return i+1;
	else
		return i;
}

// ************************************************************************************************************
// ReleaseClusterChain:	从一个给定的Cluster----dwStartCluster开始，释放其后的所有的cluster,直到遇到EOC（end of cluster）
// ************************************************************************************************************
BOOL	ReleaseClusterChain( PVOLUME_FAT pVolume , DWORD dwStartCluster )
{
	DWORD dwSector;
	DWORD dwOffset;
	DWORD dwPos=-1;
	DWORD dwCurCluster, dwNextCluster;
	DWORD dwOldPos;
	DWORD dwNum=0;

	if( dwStartCluster < pVolume->dwRootDirClusterNum +2 )
		return FALSE;

	for( dwCurCluster = dwStartCluster; !INVALID_CLUSTER( pVolume,dwCurCluster ); dwNum++ ){

		if( !GetClusterInFAT( pVolume, dwCurCluster, &dwSector, &dwOffset) )
			return FALSE;
		
		dwOldPos=dwPos;

		if( (dwPos= CheckFATCache (pVolume,dwSector)  )==-1 )
			return FALSE;

		if( dwCurCluster < pVolume->dwFirstFreeCluster) 
			pVolume->dwFirstFreeCluster= dwCurCluster;

		dwNextCluster=FetchClusEntryVal( pVolume,
								dwCurCluster,
								dwSector,
								pVolume->scFatCache.pBuf + dwPos * pVolume->fat_info.dwBytesPerSec,
								dwOffset);

		SetClusEntryVal( pVolume ,dwCurCluster, dwSector, dwPos,dwOffset,0);
		dwCurCluster = dwNextCluster;

#ifndef	FAT_FLUSH_DELAY

		if( dwOldPos!=-1 && dwPos != dwOldPos ){
		
			if( !WriteFatSector( pVolume, dwPos) )
				return FALSE;
		}
#endif
	}
	pVolume->fat_info.dwFreeCluster += dwNum;

#ifndef	FAT_FLUSH_DELAY

	if( !WriteFatSector( pVolume, dwPos) )
		return FALSE;
#endif

	return TRUE;
}

/*
#define	GET_CLUSTER_ENTRY(dwEntryVal, pVol ,dwCluster)		\
		if(!GetClusterInFAT( pVol, (dwCluster) ,&(dwSector), &(dwOffset)) ){	\
			return -1;															\
		}																		\
		dwPos= CheckFATCache( pVol , dwSector );								\
		if( dwPos == -1){					\
			return -1;						\
		}									\
	   dwEntryVal =	FetchClusEntryVal( pVol, \
					pVol->scFatCache.pBuf +pVol->fat_info.dwBytesPerSec* dwPos	\
					, dwOffset);				
*/

// ************************************************************************************************************
// GET_CLUSTER_ENTRY:	给定一个Cluster,读出它在FAT表中的位置中的值
// ************************************************************************************************************

static DWORD	GET_CLUSTER_ENTRY(PVOLUME_FAT pVol ,DWORD dwCluster)
{
	DWORD dwSector, dwOffset ,dwPos,dwEntryVal;
	if(!GetClusterInFAT( pVol, (dwCluster) ,&(dwSector), &(dwOffset)) ){	
		return -1;															
	}																		
	dwPos= CheckFATCache( pVol , dwSector );								
	if( dwPos == -1){					
		return -1;						
	}									
   dwEntryVal =	FetchClusEntryVal( pVol, 
				(dwCluster),
				dwSector,
				pVol->scFatCache.pBuf +pVol->fat_info.dwBytesPerSec* dwPos
				, dwOffset);				
	return dwEntryVal;
}
// ************************************************************************************************************
// LinkCluster:	将给定的两个Cluster, dwCluster1 与 dwCluster2在FAT表中链接起来。
// ************************************************************************************************************
BOOL	LinkCluster( PVOLUME_FAT	pVol, DWORD  dwCluster1, DWORD dwCluster2 )
{
	DWORD dwSector, dwOffset;
	DWORD dwPos;

	if(GetClusterInFAT( pVol,  dwCluster1, &dwSector, &dwOffset ) ){

		dwPos= CheckFATCache( pVol , dwSector );
		if( dwPos != -1){

		   SetClusEntryVal( pVol, 
							dwCluster1,
							dwSector,
							 dwPos,
							 dwOffset,   dwCluster2 );
		}
	}
	return TRUE;
}
// ******************************************************************************************************************
// FatAllocCluster:从FAT表中查岀　dwNum个空的 cluster.
//			pVol->dwFirstFreeCluster 的值将有可能被增长。
//	返回：成功：分配到的第一个cluster的编号。　　　失败：－1。
// ******************************************************************************************************************
DWORD	FatAllocCluster( PVOLUME_FAT pVol ,DWORD dwNum, DWORD *lpdwNumAlloced)
{
	DWORD	 i ,k;
	DWORD    dwSector, dwOffset, dwPos ,dwEntryVal;
	BOOL	 bFirst=TRUE;
	DWORD	 dwAlloced=-1;
	DWORD   dwMaxClusterCount = pVol->fat_info.dwClusterCount + 2; // 2004-08-25, lilin, 增加速度
	DWORD dwCluster;

	// 2004-08-25, lilin, 增加速度
	//for( i= pVol->dwFirstFreeCluster; i < pVol->fat_info.dwClusterCount; i++ ){
	for( i = pVol->dwFirstFreeCluster; i < dwMaxClusterCount; i++ ){
	//
		dwEntryVal=GET_CLUSTER_ENTRY( pVol,i);
		//GET_CLUSTER_ENTRY( dwEntryVal, pVol,i);
	   if( dwEntryVal ==0 ){			
		   //找到第一个
		   if( bFirst ){
			   bFirst=FALSE;
		   }
		   //是否还有？
		   // 2004-08-25， modify by lilin
		   //for( k = 1; k <= dwNum-1; k++ ){
		   for( k = 1; k < dwNum && (i + k) < dwMaxClusterCount; k++ ){
		   //
			   dwEntryVal=GET_CLUSTER_ENTRY( pVol,i+k);
			   //GET_CLUSTER_ENTRY( dwEntryVal, pVol,i+k);
			   if( dwEntryVal ){
				   break;
			   }
		   }
		   // 2004-08-25， modify by lilin
		   //是否找到全部？
		   //if( k>= dwNum-1 ){
		   if( k == dwNum ){
		   //找到
			   dwAlloced=dwNum;
			   goto Alloc_Cluster;
		   }else if ( k>=BURST_WRITE_CLUSTER ){

			   dwAlloced=BURST_WRITE_CLUSTER;
			   goto Alloc_Cluster;
		   }
	   }
	}
	// 2004-08-25, lilin, 增加速度
	//if( i>=pVol->fat_info.dwClusterCount){
	if( i >= dwMaxClusterCount ){
	//
		i=-1;
		//SetLastError( ERROR_HANDLE_DISK_FULL ); //The disk is full.
		SetLastError( ERROR_DISK_FULL );
		return -1;
	}

Alloc_Cluster:

	if( lpdwNumAlloced )
		*lpdwNumAlloced=dwAlloced;
	dwCluster = i;
	for( k=0; k< dwAlloced; k++ ){ //我们要将 i到　i+dwAlloced　串接起来,即：i的下一个为i+1,依次类推。
		dwCluster = i+k;

		if(GetClusterInFAT( pVol, dwCluster ,&dwSector, &dwOffset ) ){

			dwPos= CheckFATCache( pVol , dwSector );
			if( dwPos != -1){

			  SetClusEntryVal( pVol, dwCluster, dwSector,
								dwPos,
								 dwOffset,  dwCluster+1 );

			}
			else
			{
				ASSERT( 0 );
				break;
			}
		}
		else
		{
			ASSERT( 0 );
			break;
		}
	}
	SetClusEntryVal( pVol, dwCluster, dwSector, dwPos,
					 dwOffset,   FAT_CLUSTER_EOC);

	if( dwAlloced !=-1){

		pVol->fat_info.dwFreeCluster-=dwAlloced;
		pVol->dwFirstFreeCluster= i+dwAlloced;

#ifndef	FAT_FLUSH_DELAY
		if( !WriteFatSector( pVol, dwPos) )
			return -1;
#endif
	}
	return i;
}
// ******************************************************************************************************************
// FileGetCurrentCluster: 给定一个Cluster----dwFromClus,在FAT表中找出从它开始dwClusWalk之后的Cluster的编号
//
// ******************************************************************************************************************
DWORD FileGetCurrentCluster(PVOLUME_FAT pVol, DWORD	dwFromClus, DWORD dwClusWalk) 
{
	DWORD dwCluster=dwFromClus,i;
	
	for( i=0; i< dwClusWalk; i++ ){

		dwCluster=GetNextCluster( pVol, dwCluster );
		if( dwCluster ==- 1)
			return -1;
	}
	return dwCluster;
}
//得到最后一个有效的文件块
DWORD FileGetLastValidCluster( PVOLUME_FAT pVol, DWORD	dwFromClus )
{
	DWORD dwCluster=dwFromClus;
	DWORD dwNextCluster;
	
	while( 1 ){
		dwNextCluster=GetNextCluster( pVol, dwCluster );
		if( dwNextCluster == -1 )
			return -1;	//错误
		if( INVALID_CLUSTER( pVol, dwNextCluster ) )
		{	//块结束
			break;			
		}
		dwCluster = dwNextCluster;	//下一个有效块
	}
	return dwCluster;
}
// ******************************************************************************************************************
// INVALID_CLUSTER: 给定一个Cluster----dwCluster,判断这是否是一个合法的cluster
//
// ******************************************************************************************************************
BOOL	INVALID_CLUSTER(PVOLUME_FAT pVolume, DWORD dwCluster)
{
	if( dwCluster ==-1 ||  dwCluster ==FAT_CLUSTER_BAD )
		return TRUE;
//	if( dwCluster < pVolume->dwRootDirClusterNum+2)
//		return TRUE;

	if( pVolume->dwVersion ==FAT_VERSION16  ){
/* 2004-09-13, del
		if( dwCluster == FAT_CLUSTER_EOC16 ||dwCluster == FAT_CLUSTER_EOC)
			return TRUE;
		else if(  dwCluster > FAT_CLUSTER_EOC16 ){

			return TRUE;
		}
		
*/
		if( dwCluster >= FAT_CLUSTER_EOC16 )
			return TRUE;

	}else if( pVolume->dwVersion ==FAT_VERSION32  ){

		if( dwCluster == FAT_CLUSTER_EOC )
			return TRUE;
	} else if( pVolume->dwVersion ==FAT_VERSION12  ){
		if( dwCluster >= FAT_CLUSTER_EOC12 )
			return TRUE;
		//if( dwCluster == FAT_CLUSTER_EOC16 ||dwCluster == FAT_CLUSTER_EOC)
		//	return TRUE;
		//else if(  dwCluster > FAT_CLUSTER_EOC12 ){
		//	return TRUE;
		//}
	}
	return FALSE;
}


//-----------------------------------------------------------------------------
// ChkSum()
// Returns an unsigned byte checksum computed on an unsigned byte
// array. The array must be 11 bytes long and is assumed to contain
// a name stored in the format of a MS-DOS directory entry.
// Passed: pFcbName Pointer to an unsigned byte array assumed to be
// 11 bytes long.
// Returns: Sum An 8-bit unsigned checksum of the array pointed
// to by pFcbName.
//------------------------------------------------------------------------------
unsigned char ShortNameCheckSum (unsigned char *pFcbName)
{
	short FcbNameLen;
	unsigned char Sum;
	Sum =0;
	for (FcbNameLen=11; FcbNameLen!=0; FcbNameLen--) {
		// NOTE: The operation is an unsigned char rotate right
		Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *pFcbName++;
	}
	return (Sum);
}

/* 
BOOL isleap(int year)
{
    BOOL leap=FALSE;

	if ((year % 4) == 0)
	{
        leap = TRUE;
        if ((year % 100) == 0) {
            leap = (year%400) ? 0 : 1;
        }
    }
    return leap;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
short DayOfThisMonth( short year, short month )
{
	if(month<=7)
	{
		if( month%2) return 31;
		else if( month !=2) return 30;
		else if( isleap( year ) ) return 29;
		else  return 28;
	}
	else
	{
		if( month%2) return 30;
        else         return 31;
	}
}

DWORD  GetDaysFrom1601( PFAT_DATE pDate)
{
	DWORD year;
	DWORD leapYear=0;
	DWORD day;
	WORD  i;
	UCHAR cMonth=pDate->nMonth-1;

	year= pDate->nYearCount+1980; 
	year-=1601;

	leapYear = year/4;
	leapYear-= year/100;
	leapYear+= year/400;

	day= year*365 + leapYear;

	for( i=1; i< pDate->nMonth; i++ )
	{
		day+=DayOfThisMonth( (short)(pDate->nYearCount + 1980) , (short)i );
	}
	day+=pDate->nDay -1;
	return day;
}

DWORD DwordAdd(DWORD Src1,DWORD Src2,BYTE *pCarry)
{
	DWORD Result;

	Result=Src1+Src2;
	if (Result<Src1){

		*pCarry=1;

	}else{
		*pCarry=0;
	}
	return   Result;
}


void DwordMultiply(DWORD Src1,DWORD Src2,DWORD *pResultHi,DWORD *pResultLo)
{
	DWORD HH,HL1,HL2,LL;
	DWORD Src1Hi,Src1Lo,Src2Hi,Src2Lo;
	BYTE  Carry;

	Src1Hi=(Src1&0xffff0000)>>16;
	Src1Lo= Src1&0x0000ffff;
	Src2Hi=(Src2&0xffff0000)>>16;
	Src2Lo= Src2&0x0000ffff;

	HH =Src1Hi*Src2Hi;
	HL1=Src1Hi*Src2Lo;
	HL2=Src1Lo*Src2Hi;
	LL =Src1Lo*Src2Lo;

	*pResultHi=HH+((HL1&0xffff0000) >>16 )+ ((HL2&0xffff0000)>>16);
	// Have Carry ?
	*pResultLo=DwordAdd( (HL1&0x0000ffff)<<16, (HL2&0x0000ffff)<<16, &Carry);
	if (Carry)
	{
		*pResultHi+=1;
	}
	*pResultLo=DwordAdd(*pResultLo,LL,&Carry);
	if (Carry)
	{
		*pResultHi+=1;
	}
}
*/

void  ParseFileTime( PFILETIME pFileTime,  PFAT_DATE pDate, PFAT_TIME pTime)
{
	SYSTEMTIME	systime;

	systime.wDay=pDate->nDay;
	systime.wHour=pTime->nHour;
	systime.wMilliseconds=0;
	systime.wMinute= pTime->nMinute;
	systime.wMonth =pDate->nMonth;
	systime.wSecond=pTime->nTwiceSecond*2;
	systime.wYear= pDate->nYearCount+1980;

	SystemTimeToFileTime( &systime, pFileTime );
/*
	DWORD    day;
	DWORD    nsDay= 864000000;// 000; //8640,0000,0000
	BYTE     Carry;
	DWORD	nsDayHi,nsDayLow;

	day=GetDaysFrom1601( pDate);
	day*=1000;
	DwordMultiply(day,nsDay,&pFileTime ->dwHighDateTime,&pFileTime->dwLowDateTime);

	//FileTimeToSystemTime( pFileTime, &st_SysTime);

	nsDay= ((  60*pTime->nHour  + pTime->nMinute ) *60 +pTime->nTwiceSecond *2 ) ;

	DwordMultiply(nsDay,10000000 ,&nsDayHi,&nsDayLow);

	pFileTime->dwLowDateTime=DwordAdd( nsDayLow,pFileTime->dwLowDateTime, &Carry);
	
	//FileTimeToSystemTime( pFileTime, &st_SysTime);
	if( Carry )
	    pFileTime -> dwHighDateTime +=1;

	pFileTime->dwHighDateTime+= nsDayHi;
*/
	//FileTimeToSystemTime( pFileTime, &st_SysTime);
}

////////////////////////////////////////////
BOOL IsPath( char *pcsPath )
{
	int iCount =0;
	if( pcsPath )
	{
		while( *pcsPath ){			
			// modify by lilin-2004-08-03
			//if( *pcsPath =='\\' || *pcsPath =='/' ){		
			if( IS_PATH_MARK( pcsPath ) ){				
				iCount++;
				if( iCount > 1 )
				    return TRUE;
			}
			pcsPath++;
		}
	}
	//if( iCount >1 )
	//	return TRUE;
	//else
	return FALSE;
}

PUCHAR	  CutCurrentPath( PUCHAR	pPath )
{
    if( pPath )
	{
		// modify by lilin-2004-08-03
		//if(*pPath =='\\' || *pPath =='/' )
		if( IS_PATH_MARK( pPath ) )
			pPath++;
		//
		//while(  *pPath && *pPath !='\\' && *pPath !='/' ){
		while( *pPath && !IS_PATH_MARK( pPath ) ){
			
			pPath++;
		}
		//if( *pPath && *pPath=='\\' ){
        if( IS_PATH_MARK( pPath ) ){			
			return pPath;
		}
	}
	return NULL;
}
// *****************************************************************************
// GetFileNameFromPath:
// *****************************************************************************
PUCHAR	  GetFileNameFromPath( PUCHAR	szFileName )
{
	int iCount=0 ,i;

	while( * szFileName ){

		szFileName++;
		iCount++;
	}
	for( i=0; i < iCount ;i++, szFileName--  ){

		//if( *szFileName =='\\' ||*szFileName =='/' ){
		if( IS_PATH_MARK( szFileName ) ){

			szFileName++;
			break;
		}
	}
	return szFileName;
}
/////////////////////////////////////
#ifdef	UNDER_CE
BOOL	NotifyShell( PVOLUME_FAT pVol, const TCHAR *pName, const TCHAR* pOldName, DWORD dwAttr ,DWORD dwSize,DWORD dwEvent )
#else
BOOL	NotifyShell( PVOLUME_FAT pVol, const char *pName, const char* pOldName, DWORD dwAttr ,DWORD dwSize,DWORD dwEvent )
#endif
{
	FILECHANGEINFO	notify_info;
	SYSTEMTIME	sys_time;

	GetLocalTime(&sys_time);
	SystemTimeToFileTime(&sys_time,&notify_info.ftModified);

	notify_info.cbSize= sizeof( notify_info );
	notify_info.dwAttributes=dwAttr;

//	RETAILMSG( DEBUG_NOTIFY,(TEXT("Notify %s (%s)  %x(from %d)\r\n"),pName,pOldName,dwEvent,iLine));

	if( dwEvent== SHCNE_RENAMEFOLDER  || dwEvent == SHCNE_RENAMEITEM   ){

		notify_info.dwItem1=(DWORD)pOldName;
		notify_info.dwItem2=(DWORD)pName;

	}else{
		notify_info.dwItem1=(DWORD)pName;
		notify_info.dwItem2=0;
	}
	notify_info.nFileSize=dwSize;
	notify_info.uFlags=SHCNF_PATH;
	notify_info.wEventId=dwEvent;
	if( pVol->FuncShellNotify)
		pVol->FuncShellNotify( &notify_info);

	return TRUE;
}
