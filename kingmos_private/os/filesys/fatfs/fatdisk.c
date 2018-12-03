/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����ʵ�ִ�ȡcluster�ĺ�����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-4-18
���ߣ�κ����
�޸ļ�¼��
******************************************************/
#include "fatfs.h"
#include "fatfmt.h"

// ******************************************************************************************************************
// ReadCluster: ����һ��Cluster----dwCluster,�������ݶ�����pBufָ������ݣ�dwBufSizeΪ�ڴ��С
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
// WriteCluster: ��ccClusterCache.dwCluster�еĵ�dwIndex��buffer�У����bFlushΪTRUE,�ͽ����bufferд�����̣�bFlushΪ
//				FALSE��������һ����־����ʾ���buffer��Ҫˢ�¡�
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

		//pVol->bFlushed=FALSE; //2005-03-17�� �Ƶ�����
		pVol->ccClusterCache.bNeedFlush[dwIndex]=TRUE;
		pVol->ccClusterCache.dwLastAccess[dwIndex]=GetTickCount( );
		pVol->bFlushed=FALSE; // //2005-03-17�� �Ƶ�����
	}
	return TRUE;
}

// ******************************************************************************************************************
// WriteClusterWithBuf: ����һ��Cluster-----dwCluster,��pBufָ�������д����̡�
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
// WriteWholeCluster: �� dwCluster��ʼ����pBuf����ȡ���ݣ�дdwNum��cluster,����Ѿ������ļ���β�������
//						�µ�cluster����ǰ�ļ����������һ��cluster.
// ע�⣺��������� ReadWholeCluster������֮�������Ƕ�һ������������cluster���з��ʣ�������������ص���
//		 ������е����һ��cluster�ı�ţ���ReadWholeCluster���ص������������һ��cluster�ı�š�
// ****************************************************************************************************

// ****************************************************************************************************
// WriteWholeCluster: �� dwCluster��ʼ����pBuf����ȡ���ݣ�дdwNum��cluster,����Ѿ������ļ���β�������
//						�µ�cluster����ǰ�ļ���������һ��cluster(������EOF).
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

				if(  dwNextCluster == dwFromClus +1){//�·���ĺ�ԭ������������
				
					dwFromClus=dwNextCluster;
					dwClusterWrite +=dwClusAlloced;
					dwLastCluster=dwFromClus+ dwClusterWrite-1;
					dwNextCluster= FAT_CLUSTER_EOC;
				}

			}else{

				LinkCluster( pVol, dwFromClus + dwClusterWrite -1, dwNextCluster);

				if(  dwNextCluster == dwFromClus + dwClusterWrite ){//�·���ĺ�ԭ������������
				
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
//			�� dwFromClus����ʼ�������� dwNumCluster��cluster�����ݣ�����pBuf�У�
//			���أ�����֮��,��һ�ζ�ʱclusterӦָ���ֵ��
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
// WriteFatSector: �� iIndexָ���sector buffer������д����̣�ע�⣬FAT�������ݣ���������Ҫд������
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
		//�õ��ڶ���FAT������λ��
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
