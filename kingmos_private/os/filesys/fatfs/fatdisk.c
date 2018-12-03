/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：实现存取cluster的函数。
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙
修改记录：
******************************************************/
#include "fatfs.h"
#include "fatfmt.h"

// ******************************************************************************************************************
// ReadCluster: 给定一个Cluster----dwCluster,将其内容读入由pBuf指向的内容，dwBufSize为内存大小
//
// ******************************************************************************************************************

BOOL ReadCluster(PVOLUME_FAT pVol, DWORD dwCluster, PCHAR pBuf,DWORD dwBufSize)
{
	DWORD dwRet;
	DWORD dwSector;
//	DWORD dwSector=FIRST_SECTOR_OF_CLUSTER( pVol,dwCluster);
	START_SETOR_OF_CLUSTER(dwSector,pVol, dwCluster);
	if( dwSector ==-1)
		return FALSE;

	if( dwBufSize < pVol->dwClusterSize )
		return FALSE;

	dwRet=FSDMGR_ReadDisk(pVol->hDisk,
					dwSector,
					pVol->fat_info.dwSecPerCluster,
					pBuf,
					dwBufSize);
	if( dwRet ){
	
		RETAILMSG( 1,(TEXT("ReadCluster  failed...#########..\r\n")));
	}
	return dwRet? FALSE: TRUE;
}
// ******************************************************************************************************************
// WriteCluster: 在ccClusterCache.dwCluster中的第dwIndex个buffer中，如果bFlush为TRUE,就将这个buffer写到磁盘，bFlush为
//				FALSE，就设置一个标志，以示这个buffer需要刷新。
// ******************************************************************************************************************
//BOOL WriteCluster(PVOLUME_FAT pVol, DWORD dwCluster, PCHAR pBuf,DWORD dwBufSize)
BOOL WriteCluster(PVOLUME_FAT pVol, DWORD dwIndex,BOOL bFlush)
{
	DWORD dwRet;
	DWORD dwSector;
	DWORD dwCluster;

	if( bFlush ){
		pVol->ccClusterCache.bNeedFlush[dwIndex]=FALSE;

		dwCluster=pVol->ccClusterCache.dwCluster[dwIndex];

		START_SETOR_OF_CLUSTER(dwSector,pVol, dwCluster);
		if( dwSector ==-1)
			return FALSE;

		dwRet=FSDMGR_WriteDisk(pVol->hDisk,
						dwSector,
						pVol->fat_info.dwSecPerCluster,
						pVol->ccClusterCache.pBufStart+dwIndex*pVol->dwClusterSize,
						pVol->dwClusterSize);

		return dwRet? FALSE: TRUE;

	}else{

		//pVol->bFlushed=FALSE; //2005-03-17， 移到后面
		pVol->ccClusterCache.bNeedFlush[dwIndex]=TRUE;
		pVol->ccClusterCache.dwLastAccess[dwIndex]=GetTickCount( );
		pVol->bFlushed=FALSE; // //2005-03-17， 移到这里
	}
	return TRUE;
}

// ******************************************************************************************************************
// WriteClusterWithBuf: 给定一个Cluster-----dwCluster,把pBuf指向的内容写入磁盘。
// ******************************************************************************************************************
BOOL	WriteClusterWithBuf(PVOLUME_FAT pVol, DWORD dwCluster, PCHAR pBuf) 
{
	DWORD dwRet;
	DWORD dwSector;

	START_SETOR_OF_CLUSTER(dwSector,pVol, dwCluster);
	if( dwSector ==-1)
		return FALSE;

	dwRet=FSDMGR_WriteDisk(pVol->hDisk,
					dwSector,
					pVol->fat_info.dwSecPerCluster,
					pBuf,
					pVol->dwClusterSize);

	return dwRet? FALSE: TRUE;

}

// ****************************************************************************************************
// WriteWholeCluster: 从 dwCluster开始，从pBuf中提取数据，写dwNum个cluster,如果已经到达文件结尾，则分配
//						新的cluster给当前文件。返回最后一个cluster.
// 注意：这个函数和 ReadWholeCluster有相似之处，都是对一串可能连续的cluster进行访问，但这个函数返回的是
//		 这个串中的最后一个cluster的编号，而ReadWholeCluster返回的是这个串中下一个cluster的编号。
// ****************************************************************************************************

// ****************************************************************************************************
// WriteWholeCluster: 从 dwCluster开始，从pBuf中提取数据，写dwNum个cluster,如果已经到达文件结尾，则分配
//						新的cluster给当前文件。返回下一个cluster(可能是EOF).
// ****************************************************************************************************
DWORD WriteWholeCluster(PVOLUME_FAT pVol, PCHAR  pBuf, PFILE_FAT	pFile, DWORD dwNum)
{
	DWORD	i ,dwNextCluster ;
	DWORD   dwClusterWrite=0,dwClusAlloced=0;
	DWORD	dwSector ,dwLastCluster=-1; 
	DWORD   dwStartCluster;
	DWORD   dwFromClus ;
	
	dwFromClus=dwStartCluster=pFile->dwClusterCurrent;
	dwNextCluster=FAT_CLUSTER_EOC;
	
	for( i=0; i< dwNum; ){

		dwClusterWrite=0;

		if ( !INVALID_CLUSTER(pVol,dwFromClus) ){

			dwClusterWrite=GetContinuesCluster( pVol, dwFromClus ,dwNum -i ,&dwNextCluster);

			dwLastCluster=dwFromClus+ dwClusterWrite-1;
		}

		dwClusAlloced=0;
		if( i+dwClusterWrite< dwNum && INVALID_CLUSTER(pVol,dwNextCluster )){

			dwNextCluster=FatAllocCluster( pVol, dwNum-dwClusterWrite, &dwClusAlloced);

			if( !dwClusAlloced  )
				return -1;
			if( INVALID_CLUSTER(pVol,dwFromClus) ){

				dwFromClus = pFile->dwLastValidClus;
				LinkCluster( pVol, dwFromClus , dwNextCluster);

				if(  dwNextCluster == dwFromClus +1){//新分配的和原来的是连续的
				
					dwFromClus=dwNextCluster;
					dwClusterWrite +=dwClusAlloced;
					dwLastCluster=dwFromClus+ dwClusterWrite-1;
					dwNextCluster= FAT_CLUSTER_EOC;
				}

			}else{

				LinkCluster( pVol, dwFromClus + dwClusterWrite -1, dwNextCluster);

				if(  dwNextCluster == dwFromClus + dwClusterWrite ){//新分配的和原来的是连续的
				
					dwClusterWrite +=dwClusAlloced;
					dwLastCluster=dwFromClus+ dwClusterWrite-1;
					dwNextCluster= FAT_CLUSTER_EOC;
				}
			}
		}
		if( pBuf &&dwClusterWrite ) {

			START_SETOR_OF_CLUSTER(dwSector,pVol, dwFromClus);
			if( dwSector ==-1)
				return -1;

			if( FSDMGR_WriteDisk( pVol->hDisk,
							dwSector,
							dwClusterWrite*pVol->fat_info.dwSecPerCluster,
							pBuf + pVol->dwClusterSize * i,
							pVol->dwClusterSize* dwClusterWrite )  
							)
				return -1;

		}
		i+=dwClusterWrite;
		dwFromClus=dwNextCluster;
		pFile->dwLastValidClus=dwLastCluster;
		if(INVALID_CLUSTER(pVol, dwLastCluster)  ){

			if( !INVALID_CLUSTER (pVol,pFile->dwClusterCurrent) ){

				return -1;
			}
		}
	}
	return  dwNextCluster;
}
//  *************************************************************************************
//  ReadWholeCluster:
//			从 dwFromClus　开始，连续读 dwNumCluster个cluster的数据，放在pBuf中，
//			返回：读完之后,下一次读时cluster应指向的值。
//  *************************************************************************************
DWORD  ReadWholeCluster( PVOLUME_FAT pVol, char *pBuf , DWORD dwFromClus, DWORD dwNumCluster )
{
	DWORD	dwClusRead,i;
	DWORD	dwSector;
	DWORD	dwNextCluster;
	DWORD   dwTimes=0;

	for( i=0; i< dwNumCluster; dwTimes++){

		dwTimes++;
		
		dwClusRead=GetContinuesCluster( pVol, dwFromClus ,dwNumCluster -i ,&dwNextCluster);

		if( dwClusRead ==-1 )
			return -1;

		START_SETOR_OF_CLUSTER(dwSector,pVol, dwFromClus);
		if( dwSector ==-1)
			return -1;

		if( FSDMGR_ReadDisk( pVol->hDisk,
						dwSector,
						dwClusRead*pVol->fat_info.dwSecPerCluster,
						pBuf + pVol->dwClusterSize * i, 
						pVol->dwClusterSize* dwClusRead )
						)
			return -1;

		i+=dwClusRead;
		dwFromClus=dwNextCluster;
	}
	return dwNextCluster;
}


#define		SECOND_SECTOR(pVol, dwSector)   ( pVol->fat_info.dwFATSize + (dwSector) )
// ******************************************************************************************************************
// WriteFatSector: 将 iIndex指向的sector buffer的内容写入磁盘，注意，FAT表有两份，所以我们要写入两次
// ******************************************************************************************************************
BOOL	WriteFatSector(PVOLUME_FAT pVol, int iIndex)
{	
	DWORD dwSector = pVol->scFatCache.dwSector[iIndex];//=SECOND_SECTOR( pVol, pVol->scFatCache.dwSector[iIndex] );	

	FSDMGR_WriteDisk( pVol->hDisk, dwSector,
					  1,
					  pVol->scFatCache.pBuf + iIndex*pVol->fat_info.dwBytesPerSec,pVol->fat_info.dwBytesPerSec
					 );
	if( pVol->fat_info.dwNumFAT == 2 )
	{
		//得到第二个FAT表扇区位置
		//dwSector=SECOND_SECTOR( pVol, pVol->scFatCache.dwSector[iIndex] );
		FSDMGR_WriteDisk( pVol->hDisk, SECOND_SECTOR( pVol, dwSector ),
						  1,
						  pVol->scFatCache.pBuf + iIndex*pVol->fat_info.dwBytesPerSec,pVol->fat_info.dwBytesPerSec
						  );
	}
	pVol->scFatCache.bNeedFlush[iIndex]=FALSE;
	return TRUE;
}

#ifdef	UNDER_CE
BOOL FatGetDiskInfo( HDSK  hDisk, PFDD  pFdd )
{
	FSD_DISK_INFO 	fdi;

	if( !FSDMGR_GetDiskInfo( (HDSK) hDisk,   &fdi )  ){

		pFdd->nBytesPerSector = fdi.cbSector;
		pFdd->nCylinders =fdi.cCylinders;
		pFdd->nHeadsPerCylinder = fdi.cHeadsPerCylinder;
		pFdd->nSectors =fdi.cSectors;
		pFdd->nSectorsPerTrack =fdi.cSectorsPerTrack;
		return TRUE;
	}

	return FALSE;
}
#endif
