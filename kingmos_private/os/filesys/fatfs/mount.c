/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：实现和 Mount unmout 有关的函数,包括格式化。
版本号：1.0.0
开发时期：2003-4-18
作者：魏海龙
修改记录：
	lilin,2004-08-24， 格式化后的dwClusterCount计算有误
******************************************************/

#include "fatfs.h"
#include "fatfmt.h"
#include <devdrv.h>
#include <diskio.h>
#include <efsdmgr.h>
#include "partdrv.h"
#include "part.h"
#if (KINGMOS &&!UNDER_CE)
#include <eapisrv.h>
#include <edevice.h>
#include <devdrv.h>
#else
//#define  RETAILMSG  //
#endif

static PVOLUME_FAT			g_pFatVolumeList=NULL;
static CRITICAL_SECTION		g_csFatListCrit;

static BOOL	CheckFATValidity(PVOLUME_FAT  pVolume, char *pcBuf );
static void CheckFATError( PVOLUME_FAT pVolume );
static BOOL	CheckFAT32Part( PVOLUME_FAT pVolume ,char * pTempBuf );
static DWORD WINAPI  FatFlushDelayThread( LPVOID lParam);
void FlushFAT( PVOLUME_FAT pVol );
// ****************************************************************************
// MountFatDisk:
// ****************************************************************************
BOOL MountFatDisk( HDSK  hDisk )
{
	char *pBuf;
	PVOLUME_FAT  pVolume;

	DEBUGMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk entered..########..\r\n")));
	//return TRUE;

	pVolume=(PVOLUME_FAT)malloc( sizeof( VOLUME_FAT )  );
	if(! pVolume ){

		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk can't alloc Volume\r\n")));
		return FALSE;
	}
	memset( pVolume,0, sizeof( VOLUME_FAT) );

	pBuf=(char*)malloc(  DEFAULT_SECTOR_SIZE*3  );
	if( !pBuf ){

		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk can't alloc Buffer\r\n")));
		goto MountFailed;
	}
	pVolume->hDisk= hDisk;
	DEBUGMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk before GetDiskInfo\r\n")));

#if (KINGMOS &&!UNDER_CE)
	if(FSMGR_GetDiskInfo( hDisk, &pVolume->fda)){

		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk can't GetDiskInfo\r\n")));
		goto MountFailed;
	}
#else 
	if(!FatGetDiskInfo( hDisk, &pVolume->fda)){

		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk can't GetDiskInfo\r\n")));
		goto MountFailed;
	}
#endif

	if( pVolume->fda.nBytesPerSector != DEFAULT_SECTOR_SIZE && 
		pVolume->fda.nBytesPerSector != DEFAULT_SECTOR_SIZE*2 &&
		pVolume->fda.nBytesPerSector != DEFAULT_SECTOR_SIZE*4
		){

		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount DiskInfo.. invalid BytesPerSector %d\r\n"),pVolume->fda.nBytesPerSector));
		goto MountFailed;
	}
	/*if( pVolume->fda.nBytesPerSector != DEFAULT_SECTOR_SIZE ) {

		DWORD  dwSectorSize=pVolume->fda.nBytesPerSector;

		pVolume->fda.nBytesPerSector=DEFAULT_SECTOR_SIZE;
		pVolume->fda.nSectors       =dwSectorSize*pVolume->fda.nSectors/pVolume->fda.nBytesPerSector;

		if(FSMGR_SetDiskInfo( hDisk, &pVolume->fda)){

			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk can't GetDiskInfo\r\n")));
			goto MountFailed;
		}
	}*/
	//Now, read the boot sector and decide whether the volume is valid or not.
	if( pVolume->fda.nSectors <2048){
		
		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk Invalid sector num %d \r\n"),pVolume->fda.nSectors));
		goto MountFailed;
	}
	DEBUGMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount DiskInfo.(%d....%d )\r\n")
		,pVolume->fda.nBytesPerSector ,pVolume->fda.nSectors));

	DEBUGMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk before ReadDisk\r\n")));
	if( FSDMGR_ReadDisk(hDisk, 0, 3, pBuf, DEFAULT_SECTOR_SIZE*3 ) ){

		RETAILMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk can't ReadDisk\r\n")));
		goto MountFailed;
	}
	DEBUGMSG( DEBUG_MOUNT,(TEXT("MountFatDisk:FAT Mount Disk before Check volume\r\n")));
	
	if( !CheckFATValidity( pVolume, pBuf ) ){
		DEBUGMSG( DEBUG_MOUNT|1,(TEXT("MountFatDisk:Invalid disk, now ready to format it.!\r\n")));
/*
#if (KINGMOS &&!UNDER_CE)
		while( API_IsReady( API_FILESYS ) == FALSE ||
			API_IsReady( API_GWE ) == FALSE )
		{ //判断系统是否已经准备好
			RETAILMSG( 1, ( "wait filesys and gwe\r\n" ) );
			Sleep(200);
		}
		if( MessageBox( NULL,"您的磁盘格式无效,要格式化吗?\r\n", "FAT文件系统",MB_YESNO | MB_TOPMOST ) ==IDYES)
#else
		if( MessageBox( NULL,L"The volume is invalid,do you want to format it?", L"Question",MB_YESNO) ==IDYES)
#endif		
*/
		if( pVolume->fda.dwFlags & DISK_INFO_FLAG_AUTOFORMAT )
		{	//由系统自动处理
			DEBUGMSG( DEBUG_MOUNT|1,(TEXT("MountFatDisk:call FormatTheVolume!\r\n")));
			if( FormatTheVolume( pVolume  ) ){
				//2004-08-25, 没有必要，在FormatTheVolume已经做了			
				//pVolume->fat_info.dwFreeCluster= pVolume->fat_info.dwClusterCount;
				//
			}else{
				goto MountFailed;
			}
		}
		else
		{  //需要用户去选择 
			DEBUGMSG( DEBUG_MOUNT|1,(TEXT("MountFatDisk: find gwme!\r\n")));
			while( API_IsReady( API_FILESYS ) == FALSE ||
				API_IsReady( API_GWE ) == FALSE )
			{ //判断系统是否已经准备好
				DEBUGMSG( 1, ( "wait filesys and gwe\r\n" ) );
				Sleep(200);
			}
			DEBUGMSG( DEBUG_MOUNT|1,(TEXT("MountFatDisk:get user request!\r\n")));
			if( MessageBox( NULL,"您的磁盘格式无效,要格式化吗?\r\n", "FAT文件系统",MB_YESNO | MB_TOPMOST ) ==IDYES)
			{
				if( FormatTheVolume( pVolume  ) ){
					//2004-08-25, 没有必要，在FormatTheVolume已经做了			
					//pVolume->fat_info.dwFreeCluster= pVolume->fat_info.dwClusterCount;
					//
				}else{					
					goto MountFailed;
				}
			}
		}
//		else{
//			goto MountFailed;
//		}
	}
	else
	{
		DEBUGMSG( 1, ( "MountFatDisk:valid fat filesys.\r\n" ) );
	}
	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  after check\r\n")));

	pVolume->dwClusterSize= pVolume->fat_info.dwBytesPerSec * pVolume->fat_info.dwSecPerCluster;

	pVolume->ccClusterCache.pBufStart=(char*)malloc( pVolume->dwClusterSize * CLUSTER_CACHE_SIZES);

	if(! pVolume->ccClusterCache.pBufStart )
		goto MountFailed;

	pVolume->scFatCache.pBuf=(char *)malloc(pVolume->fat_info.dwBytesPerSec *DISK_CACHE_SIZES );

	if( !pVolume->scFatCache.pBuf){

		free( pVolume->ccClusterCache.pBufStart);
		goto MountFailed;
	}
	//pVolume->scFatCache.dwStartSector=-1;
	//pVolume->ccClusterCache.dwStartCluster=-1;
	pVolume->pFindListHead=NULL;
	pVolume->pFileListHead=NULL;
	
	if(!InitializeCaches(  pVolume ) ){

		free( pVolume->ccClusterCache.pBufStart);
		goto MountFailed;
	}
	CheckFATError( pVolume );
	strcpy(pVolume->csVolName,"StorageDev");

//#ifndef	WIN32
	{
		DWORD dwRet;
		if( FSDMGR_DeviceIoControl(pVolume->hDisk,IOCTL_DISK_GETNAME ,NULL,0,
			pVolume->csVolName, sizeof(pVolume->csVolName)-1, &dwRet,NULL) ){

			pVolume->csVolName[dwRet]=0;
		}else{

			strcpy(pVolume->csVolName,"StorageDev");
		}
	}
//#endif

	//FSDMGR_RegisterVolume( pVolume->hDisk,pVolume->csVolName, (PVOL)pVolume);
	//pVolume->hVolume=FSDMGR_RegisterVolume( pVolume->hDisk,pVolume->csVolName, (PVOL)pVolume);

#ifdef	FAT_FLUSH_DELAY
	pVolume->bThreadRun=TRUE;
	pVolume->hDelayThread=CreateThread(0,0,FatFlushDelayThread,pVolume,0,NULL);
	//SetThreadPriority(pVolume->hDelayThread, THREAD_PRIORITY_LOWEST);
	pVolume->bThreadRun=TRUE;
	if( pVolume->hDelayThread == INVALID_HANDLE_VALUE )
		goto MountFailed;
#endif

	InitializeCriticalSection(&pVolume->csVolCrit);

	EnterCriticalSection(&g_csFatListCrit);
	pVolume->pNext=g_pFatVolumeList;
	g_pFatVolumeList=pVolume;
	LeaveCriticalSection(&g_csFatListCrit);
#if (KINGMOS &&!UNDER_CE)
	pVolume->hVolume=FSDMGR_RegisterVolume( pVolume->hDisk,pVolume->csVolName, (PVOL)pVolume);
#else
	pVolume->hVolume=FSDMGR_RegisterVolume( pVolume->hDisk,L"StorageDev", (PVOLUME)pVolume);
#endif
//	pVolume->hVolume=FSDMGR_RegisterVolume( pVolume->hDisk, NULL, (PVOL)pVolume);
	
	if( !pVolume->hVolume ){

		FAT_CloseVolume( (PVOL)pVolume );
		free( pVolume);
		return FALSE;
	}
	return TRUE;
MountFailed:
	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk failed\r\n")));
	if( pBuf )
		free( pBuf );

	free( pVolume );
	return FALSE;
}

void DebugOutSector( unsigned char * lpbS )
{
#ifdef __DEBUG
	int i;
	unsigned char buf[128];
	
	DEBUGMSG( 1,(TEXT("dump sector-begin.\r\n") ));
	for( i =0; i < 16; i++ )
	{ 
		sprintf( buf, "%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x.\r\n", lpbS[0],lpbS[1],lpbS[2],lpbS[3],lpbS[4],lpbS[5],lpbS[6],lpbS[7],lpbS[8],lpbS[9],lpbS[10],lpbS[11],lpbS[12],lpbS[13],lpbS[14],lpbS[15] );
		DEBUGMSG( 1,(TEXT("%s"), buf ));
		lpbS += 16;
	}
	DEBUGMSG( 1,(TEXT("dump sector-end.\r\n") ));
#endif
}

BOOL	CheckFATValidity( PVOLUME_FAT  pVolume, char *pcBuf )
{
	PBPB_STRUCT		pPbp=(PBPB_STRUCT)pcBuf;
	PFAT32STRUCT    pPbp32;
	PFAT16STRUCT    pPbp16;

	//Following codes implemented exactly following the FAT specification published by Microsoft.
	DWORD  dwRootDirSectors  ,dwFATSize ,dwFirstDataSec;
	DWORD  dwCountofClusters,dwDataSec ,dwTotSec;

	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check Validity entered 0x%x\r\n"),pcBuf));

	pPbp32=(PFAT32STRUCT)(pcBuf+sizeof(BPB_STRUCT));
	pPbp16=(PFAT16STRUCT)(pcBuf+sizeof(BPB_STRUCT));

	if( pPbp->wBPB_BytePerSec==512 ||
		pPbp->wBPB_BytePerSec==1024 ||
		pPbp->wBPB_BytePerSec==2048 ||
		pPbp->wBPB_BytePerSec==4096 
		)
	{
		RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check rrrr ,%x\r\n"),pPbp));
	}
	else
	{
		WARNMSG( DEBUG_MOUNT,(TEXT("Invalid BytePerSec=(%d).!\r\n"), pPbp->wBPB_BytePerSec ));
		goto _error_return;
		//DebugOutSector( pcBuf );
		//return FALSE;
	}

	if( pPbp->nPBP_SecPerClus == 1  ||
		pPbp->nPBP_SecPerClus == 2  ||
		pPbp->nPBP_SecPerClus == 4  ||
		pPbp->nPBP_SecPerClus == 8  ||
		pPbp->nPBP_SecPerClus == 16  ||
		pPbp->nPBP_SecPerClus == 32  ||
		pPbp->nPBP_SecPerClus == 64  ||
		pPbp->nPBP_SecPerClus == 128
		)
	{
		if( pPbp->wBPB_BytePerSec * pPbp->nPBP_SecPerClus >(32*1024) ) 
		{
			WARNMSG( DEBUG_MOUNT,(TEXT("Invalid Clus Bytes=(%d).!\r\n"), pPbp->wBPB_BytePerSec * pPbp->nPBP_SecPerClus ));
			//DebugOutSector( pcBuf );
			//return FALSE;
			goto _error_return;
		}
	}
	else
	{
		WARNMSG( DEBUG_MOUNT,(TEXT("Invalid SecPerClus=(%d).!\r\n"), pPbp->nPBP_SecPerClus ));
		//DebugOutSector( pcBuf );
		//return FALSE;
		goto _error_return;
	}

	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check 000000000 ,%x\r\n"),pPbp));
	//Now, try to decide the version of the FAT.
	dwRootDirSectors = ((pPbp->wPBP_RootEntryCount* 32)+(pPbp->wBPB_BytePerSec-1)) / pPbp->wBPB_BytePerSec;
	//First, the size of root directory ;
	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check 111111 \r\n")));

	if(pPbp->wPBP_FATSize16!= 0)
		dwFATSize = pPbp->wPBP_FATSize16;
	else
		dwFATSize = pPbp32->dwBPB_FATSize32;

	if(pPbp->wPBP_TotalSector16!= 0)
		dwTotSec = pPbp->wPBP_TotalSector16;
	else
		dwTotSec = pPbp->dwPBP_TotalSector32;

	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check 22222222 \r\n")));

	dwFirstDataSec=(pPbp->wPBP_RsvdSecCnt + (pPbp->nPBP_NumFATs* dwFATSize) + dwRootDirSectors );

	dwDataSec = dwTotSec-dwFirstDataSec;

	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check 333333333 \r\n")));

	dwCountofClusters = dwDataSec / pPbp->nPBP_SecPerClus;
//   在 FormatVolume 所用的 内型方法和 这里是不以致的。
//   FormatVolume 只会format 为 FAT16 or FAT32 ,not FAT12
//   因此如果要支持FAT12, 必须在 FormatVolume里也做
#if 1
	if(dwCountofClusters < 4085) {
		pVolume->dwVersion=FAT_VERSION12;
		// Volume is FAT12 
		WARNMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  Check Validity FAT12,dwCountofClusters=(%d),dwDataSec(%d),nPBP_SecPerClus(%d).\r\n"), dwCountofClusters,dwDataSec,pPbp->nPBP_SecPerClus ));
		//DebugOutSector( pcBuf );
		//return FALSE;
		//goto _error_return;
	} 
	else
#endif
	{

		if(dwCountofClusters < 65525) {

		pVolume->dwVersion=FAT_VERSION16;
		// Volume is FAT16 
		} else {

			pVolume->dwVersion=FAT_VERSION32;
		// Volume is FAT32 
		}
	}
	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Disk  FAT file system detect\r\n")));

	if ( pPbp->cBS_JumpRoot[0] ==0xEB ){

		if ( pPbp->cBS_JumpRoot[2] !=0x90 ){
			goto _error_return;
			//return FALSE;
		}
	}else if(pPbp->cBS_JumpRoot[0] !=0xE9 ){
		//return FALSE;
		goto _error_return;
	}

	/*if( pPbp->wBPB_BytePerSec==512 ||
		pPbp->wBPB_BytePerSec==1024 ||
		pPbp->wBPB_BytePerSec==2048 ||
		pPbp->wBPB_BytePerSec==4096 
		){

		if( pPbp->nPBP_SecPerClus == 1  ||
			pPbp->nPBP_SecPerClus == 2  ||
			pPbp->nPBP_SecPerClus == 4  ||
			pPbp->nPBP_SecPerClus == 8  ||
			pPbp->nPBP_SecPerClus == 16  ||
			pPbp->nPBP_SecPerClus == 32  ||
			pPbp->nPBP_SecPerClus == 64  ||
			pPbp->nPBP_SecPerClus == 128
			){

			if( pPbp->wBPB_BytePerSec * pPbp->nPBP_SecPerClus >(32*1024) ) {

				return FALSE;
			}

		}
	}*/
	if( pPbp->wPBP_RsvdSecCnt ==0 )
	{
		goto _error_return;
		//return FALSE;
	}
	if( pVolume->dwVersion ==FAT_VERSION32 ){
		if (pPbp->wPBP_RootEntryCount !=0 ){
			goto _error_return;
			//return FALSE;
		}

		if( pPbp->wPBP_TotalSector16 !=0 ){
			goto _error_return;
			//return FALSE;
		}

		if( pPbp->wPBP_FATSize16 !=0 ){
			goto _error_return;
			//return FALSE;
		}

		if( pPbp->dwPBP_TotalSector32 ==0 ){
			goto _error_return;
			//return FALSE;
		}
	// 2004-09-13, lilin, modify
	//}else if( pVolume->dwVersion ==FAT_VERSION16 ){
	}else if( pVolume->dwVersion ==FAT_VERSION16 || pVolume->dwVersion ==FAT_VERSION12 ){
	//
		//if( pPbp->wPBP_TotalSector16 ==0 )
		//	return FALSE;

		if( pPbp->wPBP_FATSize16 ==0 ){
			goto _error_return;
			//return FALSE;
		}
		//if( pPbp->dwPBP_TotalSector32 !=0 )
		//	return FALSE;

		if( pPbp->wPBP_TotalSector16 ==0 ){
			if( pPbp->dwPBP_TotalSector32 ==0 ){
				//return FALSE;
				goto _error_return;
			}
		}
	}
	//if( pcBuf[510] !=0x55 ||pcBuf[511] !=0xAA)
	//	return FALSE;
	//Now, save the volume information.
	//pVolume->dwRootDirSec = FIRST_SECTOR_OF_CLUSTER( pVolume,pVolume->fat_info.dwRootCluster);
	//pPbp->wPBP_RsvdSecCnt + (pPbp->nPBP_NumFATs* dwFATSize);

	pVolume->fat_info.dwBytesPerSec= pPbp->wBPB_BytePerSec;

	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount Check BytesPerSec %d (%d) \r\n")
		,pVolume->fat_info.dwBytesPerSec , pPbp->wBPB_BytePerSec));

	pVolume->fat_info.dwRsvedSector =pPbp->wPBP_RsvdSecCnt;
	pVolume->fat_info.dwNumFAT		=pPbp->nPBP_NumFATs;
	pVolume->fat_info.dwSecPerCluster=pPbp->nPBP_SecPerClus;

	pVolume->dwFirstDataSec =dwFirstDataSec;
	pVolume->fat_info.dwClusterCount = dwCountofClusters;

	if( pVolume->dwVersion ==FAT_VERSION32 ){
		
		memcpy(pVolume->cLabel,pPbp32->cBS_VolLab, LABEL_SIZE );
		pVolume->cLabel[LABEL_SIZE]=0;	//set the string end.

		pVolume->fat_info.dwRootEntryCount= 0; //it's invalid in FAT32.
		pVolume->fat_info.dwFATSize		 =  pPbp32->dwBPB_FATSize32;
		//pVolume->fat_info.dwTotalSector	 =  pPbp->dwPBP_TotalSector32;
		pVolume->fat_info.dwRootCluster	 =  pPbp32->dwBPB_RootCluster;

		if( !CheckFAT32Part( pVolume ,pcBuf )  ){
			goto _error_return;
			//return FALSE;
		}
		pVolume->dwRootDirSec = FIRST_SECTOR_OF_CLUSTER( pVolume,pVolume->fat_info.dwRootCluster);
		pVolume->dwRootDirClusterNum=0;

	//}else if( pVolume->dwVersion ==FAT_VERSION16 ){
	}else if( pVolume->dwVersion ==FAT_VERSION16 || pVolume->dwVersion ==FAT_VERSION12 ){

		memcpy(pVolume->cLabel,pPbp16->cBS_VolLab, LABEL_SIZE );
		pVolume->cLabel[LABEL_SIZE]=0;	//set the string end.

		pVolume->fat_info.dwRootEntryCount= pPbp->wPBP_RootEntryCount;
		pVolume->fat_info.dwFATSize		 =  pPbp->wPBP_FATSize16;
		//pVolume->fat_info.dwTotalSector	 =  pPbp->wPBP_TotalSector16;
		pVolume->fat_info.dwRootCluster	 =  0;	//Braden modified 2003-4-29...only for testing.

		pVolume->dwRootDirSec = pPbp->wPBP_RsvdSecCnt + (pPbp->nPBP_NumFATs* dwFATSize);

		pVolume->dwRootDirClusterNum= dwRootDirSectors/ pVolume->fat_info.dwSecPerCluster;

		if( dwRootDirSectors% pVolume->fat_info.dwSecPerCluster ){
			goto _error_return;
			//return FALSE;
		}
	}

	if( pPbp->dwPBP_TotalSector32 )
		pVolume->fat_info.dwTotalSector	 =  pPbp->dwPBP_TotalSector32;
	else
		pVolume->fat_info.dwTotalSector	 =  pPbp->wPBP_TotalSector16;
#ifndef	FAT_DEBUG
	if( pVolume->fat_info.dwTotalSector > pVolume->fda.nSectors ){
		goto _error_return;
		//return FALSE;
	}
#endif

//	if( pVolume->fat_info.dwTotalSector	 != pVolume->fda.nSectors )
//		return FALSE;


	pVolume->fat_info.dwFreeCluster=CalculateFreeCluster( pVolume );
	pVolume->fat_info.dwClusterCount  += pVolume->dwRootDirClusterNum;

	RETAILMSG( DEBUG_MOUNT,(TEXT(" Caculate DiskSpace SecPerClu %d.. BytesPerSec %d .. Free %d .. Total %d\r\n"),
				pVolume->fat_info.dwSecPerCluster,
				pVolume->fat_info.dwBytesPerSec,
				pVolume->fat_info.dwFreeCluster,
				pVolume->fat_info.dwClusterCount ));

	//Now, try to locate the FAT table.
	return TRUE;

_error_return:
    DEBUGMSG( DEBUG_MOUNT,(TEXT("FAT error!.dump 0 sector.\r\n")));
	DebugOutSector( pcBuf );
	return FALSE;
}



BOOL	FormatTheVolume( PVOLUME_FAT pVolume  )
{
	static const DSKSZTOSECPERCLUS DskTableFAT16 [] = {

			//{ 8400, 1}, /* disks up to 4.1 MB, the 0 value for SecPerClusVal trips an error */
			{ 8400, 0}, /* disks up to 4.1 MB, the 0 value for SecPerClusVal trips an error */
			{ 32680, 2}, /* disks up to 16 MB, 1k cluster */
			{ 262144, 4}, /* disks up to 128 MB, 2k cluster */
			{ 524288, 8}, /* disks up to 256 MB, 4k cluster */
			{ 1048576, 16}, /* disks up to 512 MB, 8k cluster */
			/* The entries after this point are not used unless FAT16 is forced */
			{ 2097152, 32}, /* disks up to 1 GB, 16k cluster */
			{ 4194304, 64}, /* disks up to 2 GB, 32k cluster */
			{ 0xFFFFFFFF, 0} /* any disk greater than 2GB, 0 value for SecPerClusVal trips an error */
	};

	static const DSKSZTOSECPERCLUS DskTableFAT32 [] = {
	{ 66600, 0}, /* disks up to 32.5 MB, the 0 value for SecPerClusVal trips an error */
	{ 532480, 1}, /* disks up to 260 MB, .5k cluster */
	{ 16777216, 8}, /* disks up to 8 GB, 4k cluster */
	{ 33554432, 16}, /* disks up to 16 GB, 8k cluster */
	{ 67108864, 32}, /* disks up to 32 GB, 16k cluster */
	{ 0xFFFFFFFF, 64}/* disks greater than 32GB, 32k cluster */
	};

	DWORD	dwDiskSize  ;
	const DSKSZTOSECPERCLUS		*pDskDescripter;
	DWORD		iMaxCount, i;
	UCHAR	*		pBufTemp;
	DWORD	dwRootDirSectors, dwTmpVal1,dwTmpVal2;
	PBPB_STRUCT		pPbp;
	PFAT32STRUCT    pPbp32;
	PFAT16STRUCT    pPbp16;
	BOOL	bSuccess=FALSE;
//	DWORD	dwRsvedOffset;
//	DWORD   nFormatCount;
	DWORD  dwWriteSector, dwEndSector;

	if(! (pBufTemp =(UCHAR*) malloc( DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM ) ) ){

		RETAILMSG( DEBUG_MOUNT,(TEXT("FAT format malloc failed\r\n")));
		return FALSE;
	}
	memset( pBufTemp,0,   DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM);

	dwDiskSize= pVolume->fda.nSectors;
//2005-03-11, add support for fat12, by lilin	
	if( dwDiskSize <= 8400 )
	{
		pVolume->dwVersion =FAT_VERSION12;
		pDskDescripter= NULL; 
		iMaxCount=0;
		pVolume->fat_info.dwSecPerCluster = 0; // 后面计算
	}
	else
//
	{
		if( dwDiskSize < 512*1024*2 ) {// < 512 M
			
			pVolume->dwVersion =FAT_VERSION16;
			pDskDescripter = DskTableFAT16; 
			iMaxCount=8;
		}else{
			
			pVolume->dwVersion =FAT_VERSION32;
			pDskDescripter = DskTableFAT32; 
			iMaxCount=6;
		}
		pVolume->fat_info.dwSecPerCluster=0;
		for( i=0; i<iMaxCount;i++ ){
			
			if( pDskDescripter->dwDiskSize >= dwDiskSize ){
				
				pVolume->fat_info.dwSecPerCluster=pDskDescripter->cSecPerClusVal;
				break;
			}
			pDskDescripter++;
		}
		if( pVolume->fat_info.dwSecPerCluster == 0 ){
			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format failed 111111 \r\n")));
			goto InitVolumeFailed;
		}
	}

	pVolume->fat_info.dwBytesPerSec=pVolume->fda.nBytesPerSector;
	pVolume->fat_info.dwTotalSector=pVolume->fda.nSectors;

	pVolume->fat_info.dwNumFAT=2;
//2005-03-11, add support for fat12, by lilin	
	if( pVolume->dwVersion ==FAT_VERSION12 ){
//
		pVolume->fat_info.dwRsvedSector=1;
		pVolume->fat_info.dwRootEntryCount=256;
		pVolume->fat_info.dwRootCluster=0;

	}else if( pVolume->dwVersion ==FAT_VERSION16  ){
		pVolume->fat_info.dwRsvedSector=1;
		pVolume->fat_info.dwRootEntryCount=512;
		pVolume->fat_info.dwRootCluster=0;
	}else{
		//FAT32
		pVolume->fat_info.dwRootCluster=2;
		pVolume->fat_info.dwRsvedSector=32;
		pVolume->fat_info.dwRootEntryCount=0; //this member in FAT32 is useless.
	}
//2005-03-11, add support for fat12, by lilin	
	if( pVolume->dwVersion == FAT_VERSION12 )
//
	{
		UINT uLoop, uSectorFATs, uSectorFATsLast, uSectorRoot, uSectorCluster;
		DWORD dwSectorData, dwSectorHidden;
		DWORD dwClusterData;

		uSectorCluster = 1;
		dwSectorHidden = 0;

		uLoop = 4;
		uSectorFATs = uSectorRoot = 0;
		do {				
			dwSectorData = dwDiskSize -
				dwSectorHidden - pVolume->fat_info.dwRsvedSector - uSectorFATs - uSectorRoot;
			do {
				dwClusterData = dwSectorData / uSectorCluster;
				if (dwClusterData < 0x00000FF6) {
					break; //FAT12 必须小于这个值
				}					
				uSectorCluster *= 2;
			} while (TRUE);
			
			uSectorFATsLast = uSectorFATs;
			// 得到FAT需要的sectors, 12 意味着12个bits
			uSectorFATs = ( ( ( dwClusterData+DATA_CLUSTER ) * 12 + 7 ) / 8 + pVolume->fda.nBytesPerSector-1) / pVolume->fda.nBytesPerSector * pVolume->fat_info.dwNumFAT;
			//得到根扇区数
			uSectorRoot = ( sizeof(DIRENTRY) * pVolume->fat_info.dwRootEntryCount + pVolume->fda.nBytesPerSector-1) / pVolume->fda.nBytesPerSector;				
		} while( uSectorFATs != uSectorFATsLast && --uLoop );
		
    	if( uLoop )
		{
			pVolume->fat_info.dwSecPerCluster = uSectorCluster;
			pVolume->fat_info.dwFATSize = uSectorFATs;
			dwRootDirSectors = uSectorRoot;
			pVolume->fat_info.dwClusterCount = dwClusterData;
		}
		else
		{
			ASSERT( 0 );
			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format failed 2222.\r\n")));
			goto InitVolumeFailed;
		}			
	}
//2005-03-11, add support for fat12, by lilin	
	else
//
	{	//FAT_VERSION16 or FAT_VERSION32
		dwRootDirSectors = ((pVolume->fat_info.dwRootEntryCount * 32) + 
			(pVolume->fat_info.dwBytesPerSec- 1)) / pVolume->fat_info.dwBytesPerSec;
		
		dwTmpVal1 = dwDiskSize -( pVolume->fat_info.dwRsvedSector + dwRootDirSectors);
		dwTmpVal2 = (256 * pVolume->fat_info.dwSecPerCluster) + pVolume->fat_info.dwNumFAT;
		
		if(pVolume->dwVersion==  FAT_VERSION32)
			dwTmpVal2 = dwTmpVal2 / 2;
		
		pVolume->fat_info.dwFATSize = (dwTmpVal1 + (dwTmpVal2 - 1)) / dwTmpVal2;
		
		// lilin,2004-08-24,根据fatgen103.pdf,的说明：
		// info:DataSec = TotSec – (BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors);
		//      CountofClusters = DataSec / BPB_SecPerClus;
		//  以下语句应该是错的，导致dwClusterCount较大
		//pVolume->fat_info.dwClusterCount=dwTmpVal1 / pVolume->fat_info.dwSecPerCluster;
		//
		//
		pVolume->fat_info.dwClusterCount = ( dwTmpVal1- (pVolume->fat_info.dwFATSize * pVolume->fat_info.dwNumFAT) ) / pVolume->fat_info.dwSecPerCluster;
		// 2004-08-24
	}
	


	strcpy(pVolume->cLabel ,"           ");
	//strcpy(pVolume->cLabel ,"NO NAME");

	pVolume->dwFirstDataSec= pVolume->fat_info.dwRsvedSector +
							 pVolume->fat_info.dwNumFAT * pVolume->fat_info.dwFATSize
							 + dwRootDirSectors ;

	pVolume->dwRootDirSec= pVolume->fat_info.dwRsvedSector +
							pVolume->fat_info.dwNumFAT * pVolume->fat_info.dwFATSize;

	pPbp=(PBPB_STRUCT)pBufTemp;
	pPbp32=(PFAT32STRUCT)(pBufTemp+sizeof(BPB_STRUCT));
	pPbp16=(PFAT16STRUCT)(pBufTemp+sizeof(BPB_STRUCT));


	pPbp->cBS_JumpRoot[0]=0xEB;
	pPbp->cBS_JumpRoot[1]=0x58;
	pPbp->cBS_JumpRoot[2]=0x90;
 
	strcpy(pPbp->cBS_OemName,"MSWIN4.1" );
//	strcpy(pPbp->cBS_OemName,"KING_MOS" );
	
	pPbp->nPBP_SecPerClus=(BYTE)pVolume->fat_info.dwSecPerCluster;
	pPbp->dwPBP_HiddSector=0;
	pPbp->nPBP_MediaType  =0xF8; //Braden set the media type to fixed temporarily.
	pPbp->nPBP_NumFATs   =(BYTE)pVolume->fat_info.dwNumFAT;
	pPbp->nPBP_SecPerClus =(BYTE)pVolume->fat_info.dwSecPerCluster;
	pPbp->wBPB_BytePerSec =(WORD)pVolume->fat_info.dwBytesPerSec;
	pPbp->wPBP_NumHeads=0;
	pPbp->wPBP_RsvdSecCnt=(WORD)pVolume->fat_info.dwRsvedSector;
	pPbp->wPBP_SectorsPerTrack=0;

	pPbp->dwPBP_TotalSector32=pVolume->fat_info.dwTotalSector;

	pPbp->wPBP_TotalSector16 = 0;

	if(pVolume->dwVersion==  FAT_VERSION32){

		PFSINFO  pFs_Info = (PFSINFO )(pBufTemp+512);

		pPbp->wPBP_RootEntryCount= 0;

		pPbp->wPBP_TotalSector16 = 0;
		pPbp->wPBP_RootEntryCount= 0;
		pPbp->wPBP_FATSize16	 = 0;

		//now init the FAT32 data.
		//Please don't change the position of tbe following two lines.because our VolLab is a string longer than it's room.
		//
		strcpy(pPbp32->cBS_VolLab,pVolume->cLabel);
		strcpy(pPbp32->cBS_FilSysType,"FAT32");

		pPbp32->dwBPB_FATSize32 =pVolume->fat_info.dwFATSize;
		pPbp32->dwBPB_RootCluster=	pVolume->fat_info.dwRootCluster;
		pPbp32->dwBS_VolId=GetTickCount( );
		pPbp32->nBS_BootSig=0x29;
		pPbp32->nBS_DrvNum =0x80;
		pPbp32->wBPB_BackupBootSector=0;// I set the value from the specification,but don't know why.
		//pPbp32->wBPB_BackupBootSector=6;// I set the value from the specification,but don't know why.
		pPbp32->wBPB_ExtFlags=0;
		pPbp32->wBPB_FATVersion=0;	//version0:0.
		pPbp32->wBPB_FSInfo=1;		//position of FAT status information structure.

		//set the FS INFO struct.
		pFs_Info->dwFreeCount=0xFFFFFFFF;//I don't know how many should be set to,just invalid it.
		pFs_Info->dwLeadSig= 0x41615252;
		pFs_Info->dwNextFree=0xFFFFFFFF;
		//pFs_Info->dwNextFree=4;
		pFs_Info->dwStructSig=0x61417272;
		pFs_Info->dwTrailSig= 0xAA550000;
		
		pFs_Info++;
		pFs_Info->dwTrailSig= 0xAA550000;
		//pBufTemp[510+512]=0x55;
		//pBufTemp[511+512]=0xAA;

	} else {

		pPbp->wPBP_RootEntryCount= (WORD)pVolume->fat_info.dwRootEntryCount;
		pPbp->wPBP_FATSize16	 = (WORD)pVolume->fat_info.dwFATSize;

		//now init the FAT16 data.
		//Please don't change the position of tbe following two lines.because our VolLab is a string longer than it's room.
		//
		strcpy(pPbp16->cBS_VolLab,pVolume->cLabel);
		if( pVolume->dwVersion == FAT_VERSION16 )
		{
			strcpy(pPbp16->cBS_FilSysType,"FAT16");
		}
//2005-03-11, add support for fat12, by lilin	
		else
		{
			strcpy(pPbp16->cBS_FilSysType,"FAT12");
		}
//

		pPbp16->dwBS_VolId=GetTickCount( );
		pPbp16->nBS_BootSig=0x29;
		//pPbp16->nBS_DrvNum =0x80;
		pPbp16->nBS_DrvNum =0x0;
	}
	pBufTemp[510]=0x55;
	pBufTemp[511]=0xAA;

	pVolume->bFlushed=TRUE;
	//写 boot sector
	if( FSDMGR_WriteDisk( pVolume->hDisk, 0, 1, pBufTemp, DEFAULT_SECTOR_SIZE ) ){
		RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format Write failed.\r\n")));
		goto InitVolumeFailed;   //can't write disk, just return.
	}
	//写其它保留sector
	memset( pBufTemp, 0, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM );
	dwWriteSector = 1;
	dwEndSector = pVolume->fat_info.dwRsvedSector;
	//nFormatCount = pVolume->fat_info.dwRsvedSector;
	for( ; dwWriteSector < dwEndSector; dwWriteSector += FORMAT_SECTOR_NUM ){
		UINT number = dwEndSector - dwWriteSector;
		number = MIN( number, FORMAT_SECTOR_NUM );
		if( FSDMGR_WriteDisk( pVolume->hDisk, dwWriteSector, number, pBufTemp, DEFAULT_SECTOR_SIZE*number ) ){
			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format clear failed.\r\n")));
			goto InitVolumeFailed;   //can't write disk, just return.
		}
	}

	//写FAT
	// lilin add code-2004-08-31
	//nFormatCount = pVolume->fat_info.dwFATSize;//( (pVolume->dwRootDirSec+dwRootDirSectors+FORMAT_SECTOR_NUM-1) / FORMAT_SECTOR_NUM ) * FORMAT_SECTOR_NUM;
	//dwRsvedOffset= pVolume->fat_info.dwRsvedSector * DEFAULT_SECTOR_SIZE;
	//准备FAT[0] and FAT[1]的数据
	//写FAT表
	dwWriteSector = pVolume->fat_info.dwRsvedSector;
	dwEndSector = dwWriteSector + pVolume->fat_info.dwFATSize;
	i = 0;
	while( i++ < pVolume->fat_info.dwNumFAT )
	{	//写头FAT
		if( pVolume->dwVersion == FAT_VERSION32 ){
			*(DWORD*)&pBufTemp[0]=0xFFFFFFF8;  //FAT[0]
			*(DWORD*)&pBufTemp[4]=0x0FFFFFFF;	//FAT[1]
			//*(DWORD*)&pBufTemp[dwRsvedOffset]=0xFFFFFFF8;  //FAT[0]
			//*(DWORD*)&pBufTemp[dwRsvedOffset+4]=0x0FFFFFFF;	//FAT[1]
		}else if(pVolume->dwVersion == FAT_VERSION16){
			//*(WORD*)&pBufTemp[dwRsvedOffset]=0xFFF8;	//FAT[0]
			//*(WORD*)&pBufTemp[dwRsvedOffset+2]=0xFFFF;	//FAT[1]
			
			*(WORD*)&pBufTemp[0]=0xFFF8;	//FAT[0]
			*(WORD*)&pBufTemp[2]=0xFFFF;	//FAT[1]
		}
//2005-03-11, add support for fat12, by lilin	
		else
		{
			pBufTemp[0] = 0xF0;  //MEDIA_1440
			pBufTemp[1] = 0xFF;
			pBufTemp[2] = 0xFF;
		}
//

		if( FSDMGR_WriteDisk( pVolume->hDisk, dwWriteSector, FORMAT_SECTOR_NUM, pBufTemp, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM ) ){
			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format Write failed.\r\n")));
			goto InitVolumeFailed;   //can't write disk, just return.
		}
		dwWriteSector += FORMAT_SECTOR_NUM;
		memset( pBufTemp, 0, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM );
		//写其它的FAT
		for( ;dwWriteSector < dwEndSector; dwWriteSector += FORMAT_SECTOR_NUM )
		{
			UINT number = dwEndSector - dwWriteSector;
			number = MIN( number, FORMAT_SECTOR_NUM );
			if( FSDMGR_WriteDisk( pVolume->hDisk, dwWriteSector, number, pBufTemp, DEFAULT_SECTOR_SIZE*number ) ){
				RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format Write failed.\r\n")));
				goto InitVolumeFailed;   //can't write disk, just return.
			}
		}
		//准备下一个FAT
		dwWriteSector = dwEndSector;
		dwEndSector = dwEndSector + pVolume->fat_info.dwFATSize;
	}
	//初始化ROOT dir
	memset( pBufTemp, 0, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM );
	dwWriteSector = pVolume->dwRootDirSec;
	dwEndSector = dwWriteSector + dwRootDirSectors;
	for( ;dwWriteSector < dwEndSector; dwWriteSector += FORMAT_SECTOR_NUM )
	{
		UINT number = dwEndSector - dwWriteSector;
		number = MIN( number, FORMAT_SECTOR_NUM );
		if( FSDMGR_WriteDisk( pVolume->hDisk, dwWriteSector, number, pBufTemp, DEFAULT_SECTOR_SIZE*number ) ){
			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format Write failed.\r\n")));
			goto InitVolumeFailed;   //can't write disk, just return.
		}
	}


	//if( FSDMGR_WriteDisk( pVolume->hDisk, 0, FORMAT_SECTOR_NUM, pBufTemp, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM ) ){
	//	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format Write failed.\r\n")));
	//	goto InitVolumeFailed;   //can't write disk, just return.
	//}
	//if( FSDMGR_WriteDisk( pVolume->hDisk, 0, FORMAT_SECTOR_NUM, pBufTemp, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM ) ){
	//	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format Write failed.\r\n")));
	//	goto InitVolumeFailed;   //can't write disk, just return.
	//}

	//i = FORMAT_SECTOR_NUM;
	//清除FAT 和 ROOT dir 的数据（FAT[n]=0 and root[n] = 0）
	//memset( pBufTemp,0, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM );
	//for( ; i < nFormatCount; i += FORMAT_SECTOR_NUM ){
	//	if( FSDMGR_WriteDisk( pVolume->hDisk, i, FORMAT_SECTOR_NUM, pBufTemp, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM ) ){
	//		RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format clear failed.\r\n")));
	//		goto InitVolumeFailed;   //can't write disk, just return.
	//	}		
	//}

	/*
	for( i=0; i< pVolume->dwRootDirSec+FORMAT_SECTOR_NUM; i+=FORMAT_SECTOR_NUM ){

		if( i <= pVolume->fat_info.dwRsvedSector &&  pVolume->fat_info.dwRsvedSector < FORMAT_SECTOR_NUM + i ){
		
			dwRsvedOffset= (pVolume->fat_info.dwRsvedSector -i )*DEFAULT_SECTOR_SIZE;
			if(pVolume->dwVersion==  FAT_VERSION32){
				*(DWORD*)&pBufTemp[dwRsvedOffset]=0xFFFFFFF8;
				*(DWORD*)&pBufTemp[dwRsvedOffset+4]=0x0FFFFFFF;
				//*(DWORD*)&pBufTemp[dwRsvedOffset+8]=0x0FFFFFFF;
			}else{
				*(WORD*)&pBufTemp[dwRsvedOffset]=0xFFF8;
				*(WORD*)&pBufTemp[dwRsvedOffset+2]=0xFFFF;
			}
		}
		if( FSDMGR_WriteDisk( pVolume->hDisk, i, FORMAT_SECTOR_NUM, pBufTemp, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM ) ){
			RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Format failed 2222222 \r\n")));
			goto InitVolumeFailed;   //can't write disk, just return.
		}
		memset( pBufTemp,0, DEFAULT_SECTOR_SIZE*FORMAT_SECTOR_NUM );
	}
	*/
	pVolume->dwRootDirClusterNum= dwRootDirSectors/ pVolume->fat_info.dwSecPerCluster;
	pVolume->dwFirstFreeCluster = 2 + pVolume->dwRootDirClusterNum;
	pVolume->fat_info.dwFreeCluster=pVolume->fat_info.dwClusterCount;	

	//2004-08-25, 在 CheckFATValidity 中有该行！！
	pVolume->fat_info.dwClusterCount  += pVolume->dwRootDirClusterNum;
	//
	bSuccess=TRUE;


InitVolumeFailed:
	free( pBufTemp );
	return bSuccess;
}


BOOL	CheckFAT32Part( PVOLUME_FAT pVolume ,char * pTempBuf )
{
	PFSINFO  pFsInfo = (PFSINFO )(pTempBuf+512);

	if( pFsInfo ->dwLeadSig !=  0x41615252 )
		return FALSE;

	if( pFsInfo->dwStructSig !=0x61417272 )
		return FALSE;

	if(	pFsInfo->dwTrailSig  != 0xAA550000 )
		return FALSE;
/*
	pFsInfo ++;
	if(	pFsInfo->dwTrailSig  != 0xAA550000 )
		return FALSE;
*/
	return TRUE;
}

BOOL DeleteVolume ( PVOLUME_FAT pVolume)
{
	PVOLUME_FAT pVol=g_pFatVolumeList;
	BOOL   bSuccess=0;

	if( pVolume ==g_pFatVolumeList ) {

		g_pFatVolumeList= pVol->pNext;
		bSuccess=1;

	}else{

		while( pVol->pNext) {

			if( pVolume == pVol->pNext ) {

				pVol->pNext= pVolume->pNext;
				bSuccess=1;
				break;
			}
			pVol=pVol->pNext;
		}
	}
	return bSuccess;
}

#define DEBUG_UnmountFatDisk 1
BOOL	UnmountFatDisk( HDSK hDsk )
{
	PVOLUME_FAT pVol=g_pFatVolumeList;
	BOOL       bStatus=FALSE;

	EnterCriticalSection( &g_csFatListCrit);

	while( pVol )	{

		if( pVol ->hDisk ==hDsk ){
			bStatus=TRUE;
			break;
		}
		pVol=pVol->pNext;
	}
	LeaveCriticalSection( &g_csFatListCrit );

	if( bStatus ){

		FSDMGR_DeregisterVolume( (PVOL)pVol->hVolume );
		// 这里将主动Flush卷，先让FlushThread退出
		// lilin -2004-05-14
		pVol->bThreadRun=FALSE;

		// lilin -add code -2004-05-14
		//刷新FAT
		FlushFAT( pVol );
		//
		if( FAT_CloseVolume( (PVOL)pVol ) ){
			// lilin -2004-05-14, remove
			//pVol->bThreadRun=FALSE;
			//
			if( WaitForSingleObject( pVol->hDelayThread, 5000 ) ==WAIT_TIMEOUT ){
				WARNMSG( DEBUG_UnmountFatDisk, ( "UnmountFatDisk:now ready to TerminateThread.\r\n" ) );
				TerminateThread(pVol->hDelayThread,0 );
			}
			else
			{
				DEBUGMSG( DEBUG_UnmountFatDisk, ( "UnmountFatDisk:FlushThread exit.\r\n" ) );
			}
			// lilin -2004-05-14, add
			CloseHandle( pVol->hDelayThread );
			//

			if( DeleteVolume(  pVol ) )
			{
				free( pVol );
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CheckFATError( PVOLUME_FAT pVolume  ) 
{



}

BOOL IsVolume( PVOLUME_FAT pVolume )
{
	PVOLUME_FAT pVol=g_pFatVolumeList;
	BOOL       bStatus=FALSE;

	EnterCriticalSection( &g_csFatListCrit);

	while( pVol )	{

		if( pVol == pVolume ){
			bStatus=TRUE;
			break;
		}
		pVol=pVol->pNext;
	}
	LeaveCriticalSection( &g_csFatListCrit );
	if( !bStatus )
		SetLastError( ERROR_INVALID_HANDLE );
	return bStatus;
}
/*
DWORD	CalculateFreeCluster( PVOLUME_FAT	pVol )
{
	DWORD	i ,k;
	DWORD	dwFree=0;
	DWORD   dwBufSize=pVol->fat_info.dwBytesPerSec*16;
	char	*pTemp;
	char *  pBufTemp=(char*)malloc( dwBufSize);
	DWORD	dwCluster=0;


	for( i=0; i< pVol->fat_info.dwFATSize ; i+=16 ) {

		if( FSDMGR_ReadDisk(pVol->hDisk, pVol->fat_info.dwRsvedSector + i, 16, pBufTemp, dwBufSize  ) ){

			free(pBufTemp);
			return -1;
		}
		pTemp=pBufTemp;

		if( i+16 >= pVol->fat_info.dwFATSize ){

			dwBufSize= (pVol->fat_info.dwFATSize -i) *pVol->fat_info.dwBytesPerSec;

			if( pVol->dwVersion ==FAT_VERSION16  ){

				dwBufSize -=(pVol->fat_info.dwBytesPerSec/2 - pVol->fat_info.dwClusterCount%(pVol->fat_info.dwBytesPerSec/2 ) )*2;

			}else{
				dwBufSize -=(pVol->fat_info.dwBytesPerSec/4 - pVol->fat_info.dwClusterCount%(pVol->fat_info.dwBytesPerSec/4 ) )*4;
			}
		}
		for(k=0; k< dwBufSize; ){

			if( pVol->dwVersion ==FAT_VERSION16  ){

				if( *(WORD*)pTemp ==0 ){

					dwFree++;
				}
				pTemp+=2;
				k+=2;
			}else{
	
				if( *(DWORD*)pTemp ==0 ){

					if( dwFree ==0 ){

						pVol->dwFirstFreeCluster = dwCluster ;
					}
					dwFree++;

				}
				pTemp+=4;
				k+=4;
			}
			dwCluster++;
		}
	}
	pVol->dwFirstFreeCluster += pVol->dwRootDirClusterNum;
	free(pBufTemp);
	return dwFree+2;
}
*/

#define		SECTOR_NUM_CHECK	16
DWORD	CalculateFreeCluster( PVOLUME_FAT	pVol )
{
	DWORD	i ,k;
	DWORD	dwFree=0;
	DWORD   dwBufSize=pVol->fat_info.dwBytesPerSec*SECTOR_NUM_CHECK;
	char	*pTemp;
	char *  pBufTemp=(char*)malloc( dwBufSize);
	DWORD	dwCluster=0;
	DWORD   dwMaxCluster=pVol->fat_info.dwClusterCount + 2;
	DWORD   dwVersion = pVol->dwVersion;
	BYTE    bPreBytes;
	BOOL    bNeedPreBytes = FALSE;

	RETAILMSG( DEBUG_MOUNT,(TEXT("FAT Mount CalculateFreeCluster bufSize %d(%d)\r\n"),dwBufSize,pVol->fat_info.dwBytesPerSec));
	// lilin 2004-05-14, modify
	//for( i=0; i< pVol->fat_info.dwFATSize; i+=SECTOR_NUM_CHECK ) {
    for( i=0; i< pVol->fat_info.dwFATSize && dwCluster < dwMaxCluster; i+=SECTOR_NUM_CHECK ) {
	//

		if( FSDMGR_ReadDisk(pVol->hDisk, pVol->fat_info.dwRsvedSector + i, SECTOR_NUM_CHECK, pBufTemp, dwBufSize  ) ){
			free(pBufTemp);
			return -1;
		}
		pTemp=pBufTemp;

		if( i+SECTOR_NUM_CHECK >  pVol->fat_info.dwFATSize ){

			//dwBufSize= (pVol->fat_info.dwFATSize -i) *pVol->fat_info.dwBytesPerSec ;
			//DWORD	dwTemp;

			if( dwVersion ==FAT_VERSION16  ){
				//dwTemp=2;
				dwBufSize= ( dwMaxCluster - dwCluster ) * 2;//dwTemp;
			}else if( dwVersion ==FAT_VERSION32 ){
				//dwTemp=4;
				dwBufSize= ( dwMaxCluster - dwCluster ) * 4;//dwTemp;
			}else if( dwVersion ==FAT_VERSION12 ){
				dwBufSize= (( dwMaxCluster - dwCluster ) * 12 + 11) / 8;//dwTemp;
			}
			// lilin 2004-05-14, modify
			//dwBufSize= ( pVol->fat_info.dwClusterCount - dwCluster + 2 ) * dwTemp;
			//dwBufSize= ( dwMaxCluster - dwCluster ) * dwTemp;
			//
		}
		for(k=0; k< dwBufSize; ){

			if( dwFree ==0 ){
				pVol->dwFirstFreeCluster = dwCluster;
			}
			if( dwVersion ==FAT_VERSION16  ){
				if( *(WORD*)pTemp ==0 ){
					dwFree++;
				}
				pTemp+=2;
				k+=2;
			}else if( dwVersion == FAT_VERSION32 ){	
				pTemp+=4;
				k+=4;

				if( *(DWORD*)pTemp ==0 ){
					dwFree++;
				}
			}else if( dwVersion == FAT_VERSION12 ){
				
				UINT nFATOffset = dwCluster + dwCluster / 2;
				UINT nOffsetInBuf = nFATOffset % dwBufSize;
				UINT uFAT12ClusEntryVal;
				LPBYTE lpFATBuf = pTemp + nOffsetInBuf;
				//FAT12ClusEntryVal = *((WORD *) &SecBuff[ThisFATEntOffset]);
				if( nOffsetInBuf == (dwBufSize - 1) ) {
					/* This cluster access spans a sector boundary in the FAT */
					/* There are a number of strategies to handling this. The */
					/* easiest is to always load FAT sectors into memory */
					/* in pairs if the volume is FAT12 (if you want to load */
					/* FAT sector N, you also load FAT sector N+1 immediately */
					/* following it in memory unless sector N is the last FAT */
					/* sector). It is assumed that this is the strategy used here */
					/* which makes this if test for a sector boundary span */
					/* unnecessary. */
					bNeedPreBytes = TRUE;
					bPreBytes = *((BYTE*)lpFATBuf);
					k=dwBufSize;
					continue;
				}
				else
				{
					if( bNeedPreBytes )
					{
						uFAT12ClusEntryVal = (*((BYTE*)lpFATBuf) << 8 ) | bPreBytes;
						bNeedPreBytes = FALSE;
					}
					else
					{
						uFAT12ClusEntryVal = (*(lpFATBuf+1) << 8) | *lpFATBuf;
					}
					
					if(dwCluster & 0x0001)
						uFAT12ClusEntryVal = uFAT12ClusEntryVal >> 4; /* Cluster number is ODD */
					else
						uFAT12ClusEntryVal = uFAT12ClusEntryVal & 0x0FFF; /* Cluster number is EVEN */
					if( uFAT12ClusEntryVal == 0 )
						dwFree++;
				}
			}

			dwCluster++;
		}
	}
	pVol->dwFirstFreeCluster += pVol->dwRootDirClusterNum;
	free(pBufTemp);
	return dwFree;
}
/*
//是否刷新备份FAT
VOID FlushBackupFAT( PVOLUME_FAT	pVol )
{
	DWORD	i;
	DWORD   dwBufSize=pVol->fat_info.dwBytesPerSec*SECTOR_NUM_CHECK;
	char *  pBufTemp;
	DWORD dwFATSize = pVol->fat_info.dwFATSize;
	DWORD dwRsvedSector = pVol->fat_info.dwRsvedSector;
	DWORD hDisk = pVol->hDisk;
//	int nBytes;

	RETAILMSG( DEBUG_MOUNT,(TEXT("FlushBackupFAT-entry.\r\n")));
	

	if( pVol->fat_info.dwNumFAT != 2 )
		return;
	
	pBufTemp=(char*)malloc( dwBufSize);

	// lilin 2004-05-14, modify
	//for( i=0; i< pVol->fat_info.dwFATSize; i+=SECTOR_NUM_CHECK ) {
    for( i = 0; i < dwFATSize; i += SECTOR_NUM_CHECK ) 
	{
	//
		DWORD dwLast = dwFATSize - i;
		DWORD dwReadSectors = MIN( SECTOR_NUM_CHECK, dwLast );
		if( FSDMGR_ReadDisk( hDisk, dwRsvedSector + i, dwReadSectors, pBufTemp, dwBufSize  ) ){
			break;  //失败
		}
		if( FSDMGR_WriteDisk( hDisk, dwRsvedSector + i + dwFATSize, dwReadSectors, pBufTemp, dwBufSize  ) ){
			break;	//失败
		}
	}
	free(pBufTemp);
}
*/

//将FAT的数据更新到磁盘
#define DEBUG_FlushFAT 0
#define DEBUG_DIRTY_BLOCK 0
void FlushFAT( PVOLUME_FAT pVol )
{
	int		i;
	
	DEBUGMSG( DEBUG_FlushFAT, ("FlushFAT:pVol->bFlushed=%d,entry.\r\n", pVol->bFlushed) );
	//if( TryEnterCriticalSection( &pVol->csVolCrit) )
	EnterCriticalSection( &pVol->csVolCrit);
	{
		
		//是否已经更新 ？
		if(  pVol->bFlushed == FALSE ){
			//没有
			for( i=0; i< DISK_CACHE_SIZES; i++ ){
				
				if( pVol->scFatCache.bNeedFlush[i] ){
					DEBUGMSG( DEBUG_FlushFAT|DEBUG_DIRTY_BLOCK, ( "FlushFAT:flush sector(%d).\r\n", i ) );
					WriteFatSector( pVol, i);
					//pVol->scFatCache.bNeedFlush[i]=FALSE; //WriteFatSector已经做了更新
				}
			}
			for( i=0; i< CLUSTER_CACHE_SIZES; i++ ){
				
				if( pVol->ccClusterCache.bNeedFlush[i] ){
					DEBUGMSG( DEBUG_FlushFAT|DEBUG_DIRTY_BLOCK, ( "FlushFAT:flush cluster(%d).\r\n", i ) );
					WriteCluster( pVol,i,TRUE);
		//			pVol->ccClusterCache.bNeedFlush[i]=FALSE;
				}
			}
			pVol->bFlushed=TRUE;
		}
		
		LeaveCriticalSection( &pVol->csVolCrit);
	}
	DEBUGMSG( DEBUG_FlushFAT, ("FlushFAT:pVol->bFlushed=%d,leave.\r\n", pVol->bFlushed) );
}

#define DEBUG_FatFlushDelayThread 0
DWORD WINAPI  FatFlushDelayThread( LPVOID lParam)
{
	PVOLUME_FAT	pVol=(PVOLUME_FAT)lParam;

	DEBUGMSG( DEBUG_FatFlushDelayThread, ("FatFlushDelayThread,FLUSH_DELAY_TIME=%d,pVol->bThreadRun=%d,entry.\r\n",FLUSH_DELAY_TIME,pVol->bThreadRun) );
	while(pVol->bThreadRun){

		DEBUGMSG( DEBUG_FatFlushDelayThread, ("FatFlushDelayThread++:pVol->bThreadRun=%d, pVol->bFlushed=%d.\r\n",pVol->bThreadRun, pVol->bFlushed) );
		Sleep( FLUSH_DELAY_TIME );
		DEBUGMSG( DEBUG_FatFlushDelayThread, ("FatFlushDelayThread--:pVol->bThreadRun=%d, pVol->bFlushed=%d.\r\n",pVol->bThreadRun, pVol->bFlushed) );		
		if( pVol->bThreadRun == FALSE )
			break;//退出		
		if(  pVol->bFlushed == FALSE ){
			DEBUGMSG( DEBUG_FatFlushDelayThread, ("FatFlushDelayThread:call FlushFAT++.\r\n") );
			FlushFAT( pVol );
			DEBUGMSG( DEBUG_FatFlushDelayThread, ("FatFlushDelayThread:call FlushFAT--.\r\n") );			
		}
	}
	DEBUGMSG( DEBUG_FatFlushDelayThread, ("FatFlushDelayThread,pVol->bThreadRun=%d,leave.\r\n",pVol->bThreadRun) );
	return 0;
	
}

/*  whl的原始代码，-2004-05-14 lilin
DWORD WINAPI  FatFlushDelayThread( LPVOID lParam)
{
	PVOLUME_FAT	pVol=(PVOLUME_FAT)lParam;
	int		i;

	while(pVol->bThreadRun){

		Sleep( FLUSH_DELAY_TIME );
		
		if(  pVol->bFlushed ){
			continue;
		}
		EnterCriticalSection( &pVol->csVolCrit);

		for( i=0; i< DISK_CACHE_SIZES; i++ ){

			if( pVol->scFatCache.bNeedFlush[i] ){

				WriteFatSector( pVol, i);
				pVol->scFatCache.bNeedFlush[i]=FALSE;
			}
		}
		for( i=0; i< CLUSTER_CACHE_SIZES; i++ ){

			if( pVol->ccClusterCache.bNeedFlush[i] ){

				WriteCluster( pVol,i,TRUE);
				pVol->ccClusterCache.bNeedFlush[i]=FALSE;
			}
		}
		pVol->bFlushed=TRUE;

		LeaveCriticalSection( &pVol->csVolCrit);
	}
	return 0;
}
*/

//  **********************************************************************************
//  **********************************************************************************
void InitializeFATSys( void  )
{
	InitializeCriticalSection(&g_csFatListCrit);

} 
