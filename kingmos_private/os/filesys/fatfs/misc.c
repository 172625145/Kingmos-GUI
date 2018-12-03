/******************************************************
Copyright(c) °æÈ¨ËùÓÐ£¬1998-2003Î¢Âß¼­¡£±£ÁôËùÓÐÈ¨Àû¡£
******************************************************/

/*****************************************************
ÎÄ¼þËµÃ÷£ºFATÎÄ¼þÏµÍ³ÖÐÓÃµ½µÄÒ»Ð©Ð¡º¯Êý¡£
°æ±¾ºÅ£º1.0.0
¿ª·¢Ê±ÆÚ£º2003-4-18
×÷Õß£ºÎºº£Á
ÐÞ¸Ä¼ÇÂ¼£º
	2004-09-13, CheckFATCache del, lilin, ÒÆµ½¿ªÊ¼£¬Ã»ÓÐ±ØÒªÃ¿´Î¶¼µ÷ÓÃ£¨ÀË·ÑÊ±¼ä£©
	2004-08-03, ½« if(*lpFile == '\\' || *lpFile=='/' ) »»Îª if( IS_PATH_MARK( lpFile ) )
******************************************************/

#include "fatfs.h"
#include "fatfmt.h"
#include "eshell.h"
static DWORD	CheckFATCache( PVOLUME_FAT pFatVol , DWORD dwSector );
// ************************************************************************************************************
// GetClusterInFAT :: ¼ÆËãÒ»¸ö¸ø¶¨µÄclusterÔÚFAT±íÖÐµÄÎ»ÖÃ(ÉÈÇø±àºÅ¼°ÔÚÕâ¸öÉÈÇøÖÐµÄÆ«ÒÆ)£¬
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
// FetchClusEntryVal :: ´ÓpcSecBuffÖ¸ÏòµÄbufferÖÐ, ÔÚdwOffsetInSecÎ»ÖÃ£¬È¡³ö
//
//ËüÍ¨³£ÒªÓëGetClusterInFATÁªºÏÊ¹ÓÃ£¬ÏÈÓÃGetClusterInFATµÃµ½Ò»¸öclusterÔÚFAT±íÖÐµÄÎ»ÖÃ£¨ÉÈÇøÓëÔÚÉÈÇøÖÐµÄÆ«ÒÆ£©£¬
//ÔÙ½«¸ÃÉÈÇøµÄÄÚÈÝ¶ÁÈësector buffer,È»ºóµ÷ÓÃFetchClusEntryValÈ¥È¡Öµ¡£
//¸Ãº¯ÊýºÜ¼òµ¥£¬Ö»ÊÇÐèÒªÇø·ÖFAT16ÓëFAT32£¬²¢ÇÒÔÚ¶Á³öµÄÖµÎª0»òEOC£¨0x0FFFFFFF£©Ê±²»¼ÓdwRootDirClusterNum£¬
//¾ÍÊÇËµdwRootDirClusterNumÖ»ÊÇÔÚ¶Á³öµÄÖµÎªÒ»¸öÕý³£µÄcluster±àºÅÊ±²Å¼ÓdwRootDirClusterNum¡£
// ************************************************************************************************************
static DWORD FetchClusEntryVal( PVOLUME_FAT pVolume, 
							    DWORD dwClus,   // Cluster ºÅ
								DWORD dwFATSector, //µ±Ç°µÄ pcSecBuff Ëù¶ÔÓ¦µÄ FATµÄ´ÅÅÌSector
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
// SetClusEntryVal :: ÔÚÓÉiIndexÖ¸ÏòµÄsector bufferÖÐµÄdwOffsetInSecÎ»ÖÃ£¬Ð´ÈëÊýÖµdwValWrite.
//
//ËüµÄÔ­ÐÍ±¾À´ÊÇÏÂÃæµÄÒ»¸öº¯Êý£¬ºÍFetchClusEntryVal±È½ÏÀàËÆ£¬µ«ÎªÁËpVolume->scFatCache.bNeedFlush[iIndex]ÉèÎª
//TRUE£¬¾Í¸Ä³ÉÁËÏÖÔÚÕâ¸öÑù×Ó£¬Í¬Ñù£¬Ö»ÓÐdwValWriteÎªÒ»¸öºÏ·¨µÄcluster±àºÅÊ±£¬²Å½«ËüµÄÖµ¼ÓÒ»¸ö¡¡dwRootDirClusterNum£¬
//¶øÇÒ±ØÐë¼ÓÕâ¸öÖµ£¬Ð´ÈëÓ²¼þ¡£
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
// IsEndOfCluster:: ¸ø¶¨Ò»¸öÊýdwContent,ÅÐ¶ÏËüÊÇ·ñÎªÒ»¸öÎÄ¼þµÄ½áÎ²cluster£¨Èç¹û²»ÄÜÀí½âÕâ¸öº¯Êý£¬Çë²éFAT¹æ·¶£©
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
// InitializeCaches:: ³õÊ¼»¯buffer(sector buffer Óëcluster buffer),ËüÖ»ÔÚÏµÍ³Æô¶¯Ê±µ÷ÓÃÒ»´Î£¬
//¡¡sector buufer ÎªFAT±íÖÐµÄ×îÇ°ÃæDISK_CACHE_SIZES¸ösectorµÄÄÚÈÝ,
//  cluster bufferÎª´ÅÅÌ¸ùÄ¿Â¼¿ªÊ¼µÄCLUSTER_CACHE_SIZES¸öclusterµÄÄÚÈÝ.
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

		//2004-09-15, lilin ¸Ã²¿·Ö¹¦ÄÜÔÚÎÒµ÷ÊÔ14M(FAT12)µÄÊ±ºò¿ÉÄÜÓÐ´í£¬ËùÒÔÎÒÔÚ³õÊ¼»¯Ê±²»È¥¶Á³ö
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
// CheckFATCache:	¸ø¶¨Ò»¸ösector ±àºÅ¡¡dwSector£¬·µ»ØËüÊÇÔÚscFatCache.dwSectorÖÐµÄµÚ¼¸¸ö»ò´íÎó£­1£¬
// ************************************************************************************************************
static DWORD	CheckFATCache( PVOLUME_FAT pFatVol , DWORD dwSector )
{
	DWORD iMinAccess=0;
	int		i;
	DWORD dwCount;
	//2004-09-13£¬ add
	LPDWORD lpdwSectorIndex = pFatVol->scFatCache.dwSector;
	LPDWORD lpdwLastAccessIndex = pFatVol->scFatCache.dwLastAccess;
	LPDWORD lpdwLastAccess, lpdwMinAccess;
	LPDWORD lpdwSector;
	//

	//2004-09-13£¬ add
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

		//2004-09-13, del, lilin, ÒÆµ½¿ªÊ¼£¬Ã»ÓÐ±ØÒªÃ¿´Î¶¼µ÷ÓÃ£¨ÀË·ÑÊ±¼ä£©
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
// CheckFATCache:	¸ø¶¨Ò»¸öcluster ±àºÅ¡¡dwCluster£¬·µ»ØËüÊÇÔÚccClusterCache.dwClusterÖÐµÄµÚ¼¸¸ö»ò´íÎó£­1£¬
//¡¡º¯ÊýÖÐµÄ×¢ÊÍÇë²ÎÕÕCheckFATCache
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
// GetNextCluster:	¸ø¶¨Ò»¸öcluster ±àºÅ¡¡dwCurCluster£¬·µ»ØËüµÄÏÂÒ»¸öcluster(ÔÚÒ»¸öÎÄ¼þ/Ä¿Â¼ÄÚÈÝµÄFATÁ´ÖÐ)£¬
//
// ************************************************************************************************************
DWORD GetNextCluster( PVOLUME_FAT pFatVol, DWORD dwCurCluster )
{
	DWORD dwSector;
	DWORD dwOffset;
	DWORD dwPos;
	DWORD dwClus;

	if( dwCurCluster <  pFatVol->dwRootDirClusterNum )//ËùÓÐµÄclusterÔÚÎÒÃÇµÄÏµÍ³ÖÐ¶¼±»¼ÓÁËÒ»¸ödwRootDirClusterNum£¬
		return  dwCurCluster +1 ;//ËùÒÔ£¬Èç¹ûclusterµÄ±àºÅ±ÈdwRootDirClusterNum»¹Ð¡£¬ÕâÒ»¶¨²»ÊÇÒ»¸öºÏ·¨µÄcluster±àºÅ,
	//ÕâÖÖÇé¿öÖ»»áÔÚFAT16ÖÐ³öÏÖ£¨ÔÚFAT32ÖÐdwRootDirClusterNumÊÇ0£©£¬ÕâËµÃ÷Ëù¸øµÄcluster±àºÅdwCurCluster£¬²»ÊÇ´ÅÅÌÉÏÒ»¸ö
	//ÕæÕýµÄcluster,Ö»ÊÇ¸ùÄ¿Â¼ÄÚÈÝËùÕ¼µÄÎ»ÖÃ£¬Ö»Ðè½«Æä¼Ó1ÈÃËüÖ¸ÏòÏÂÒ»¸öcluster¼´¿É¡£
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

	}else	//ÏµÍ³³ö´í
		return -1;
}

// ************************************************************************************************************
// GetContinuesCluster:	¸ø¶¨Ò»¸öcluster ±àºÅ¡¡dwFromClus£¬·µ»Ø´ÓÕâ¸öcluster¿ªÊ¼µÄ£¬Á¬ÐøµÄclusterÊýÄ¿£¨×î¶àdwNum¸ö£©£¬
//¡¡²¢½«ÏÂÒ»¸öÔÚÕâÐ©Á¬ÐøµÄclusterÖ®ºóµÄµÚÒ»¸öclusterµÄ±àºÅ·ÅÔÚpdwNextReadÖÐ
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
// ReleaseClusterChain:	´ÓÒ»¸ö¸ø¶¨µÄCluster----dwStartCluster¿ªÊ¼£¬ÊÍ·ÅÆäºóµÄËùÓÐµÄcluster,Ö±µ½Óöµ½EOC£¨end of cluster£©
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
// GET_CLUSTER_ENTRY:	¸ø¶¨Ò»¸öCluster,¶Á³öËüÔÚFAT±íÖÐµÄÎ»ÖÃÖÐµÄÖµ
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
// LinkCluster:	½«¸ø¶¨µÄÁ½¸öCluster, dwCluster1 Óë dwCluster2ÔÚFAT±íÖÐÁ´½ÓÆðÀ´¡£
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
// FatAllocCluster:´ÓFAT±íÖÐ²éŒç¡¡dwNum¸ö¿ÕµÄ cluster.
//			pVol->dwFirstFreeCluster µÄÖµ½«ÓÐ¿ÉÄÜ±»Ôö³¤¡£
//	·µ»Ø£º³É¹¦£º·ÖÅäµ½µÄµÚÒ»¸öclusterµÄ±àºÅ¡£¡¡¡¡¡¡Ê§°Ü£º£­1¡£
// ******************************************************************************************************************
DWORD	FatAllocCluster( PVOLUME_FAT pVol ,DWORD dwNum, DWORD *lpdwNumAlloced)
{
	DWORD	 i ,k;
	DWORD    dwSector, dwOffset, dwPos ,dwEntryVal;
	BOOL	 bFirst=TRUE;
	DWORD	 dwAlloced=-1;
	DWORD   dwMaxClusterCount = pVol->fat_info.dwClusterCount + 2; // 2004-08-25, lilin, Ôö¼ÓËÙ¶È
	DWORD dwCluster;

	// 2004-08-25, lilin, Ôö¼ÓËÙ¶È
	//for( i= pVol->dwFirstFreeCluster; i < pVol->fat_info.dwClusterCount; i++ ){
	for( i = pVol->dwFirstFreeCluster; i < dwMaxClusterCount; i++ ){
	//
		dwEntryVal=GET_CLUSTER_ENTRY( pVol,i);
		//GET_CLUSTER_ENTRY( dwEntryVal, pVol,i);
	   if( dwEntryVal ==0 ){			
		   //ÕÒµ½µÚÒ»¸ö
		   if( bFirst ){
			   bFirst=FALSE;
		   }
		   //ÊÇ·ñ»¹ÓÐ£¿
		   // 2004-08-25£¬ modify by lilin
		   //for( k = 1; k <= dwNum-1; k++ ){
		   for( k = 1; k < dwNum && (i + k) < dwMaxClusterCount; k++ ){
		   //
			   dwEntryVal=GET_CLUSTER_ENTRY( pVol,i+k);
			   //GET_CLUSTER_ENTRY( dwEntryVal, pVol,i+k);
			   if( dwEntryVal ){
				   break;
			   }
		   }
		   // 2004-08-25£¬ modify by lilin
		   //ÊÇ·ñÕÒµ½È«²¿£¿
		   //if( k>= dwNum-1 ){
		   if( k == dwNum ){
		   //ÕÒµ½
			   dwAlloced=dwNum;
			   goto Alloc_Cluster;
		   }else if ( k>=BURST_WRITE_CLUSTER ){

			   dwAlloced=BURST_WRITE_CLUSTER;
			   goto Alloc_Cluster;
		   }
	   }
	}
	// 2004-08-25, lilin, Ôö¼ÓËÙ¶È
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
	for( k=0; k< dwAlloced; k++ ){ //ÎÒÃÇÒª½« iµ½¡¡i+dwAlloced¡¡´®½ÓÆðÀ´,¼´£ºiµÄÏÂÒ»¸öÎªi+1,ÒÀ´ÎÀàÍÆ¡£
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
// FileGetCurrentCluster: ¸ø¶¨Ò»¸öCluster----dwFromClus,ÔÚFAT±íÖÐÕÒ³ö´ÓËü¿ªÊ¼dwClusWalkÖ®ºóµÄClusterµÄ±àºÅ
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
//µÃµ½×îºóÒ»¸öÓÐÐ§µÄÎÄ¼þ¿é
DWORD FileGetLastValidCluster( PVOLUME_FAT pVol, DWORD	dwFromClus )
{
	DWORD dwCluster=dwFromClus;
	DWORD dwNextCluster;
	
	while( 1 ){
		dwNextCluster=GetNextCluster( pVol, dwCluster );
		if( dwNextCluster == -1 )
			return -1;	//´íÎó
		if( INVALID_CLUSTER( pVol, dwNextCluster ) )
		{	//¿é½áÊø
			break;			
		}
		dwCluster = dwNextCluster;	//ÏÂÒ»¸öÓÐÐ§¿é
	}
	return dwCluster;
}
// ******************************************************************************************************************
// INVALID_CLUSTER: ¸ø¶¨Ò»¸öCluster----dwCluster,ÅÐ¶ÏÕâÊÇ·ñÊÇÒ»¸öºÏ·¨µÄcluster
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
