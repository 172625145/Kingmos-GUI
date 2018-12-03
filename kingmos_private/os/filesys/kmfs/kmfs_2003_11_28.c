/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：文件名（含通配符）比较
版本号：1.0.0
开发时期：2001-04-20
作者：李林
修改记录：
    2003-09-29, CopyFile可以拷贝FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM文件
    2003-09-27:	增加 对DISK_INFO_FLAG_READONLY的检查
    2003-09-27: LN, 增加写检查 SetEndOfFile
    2003-09-13, 假如失败，则lpFile永远无法移出从list
    2003-09-11, 当CREATE_ALWAYS时，将长度被设为0
    2003-07-30, CREATE_ALWAY 应重新设置文件Attrib，
	            TRUNCATE_EXISTING将内容设为0长度
    2003-07-16, 假如路经为 "\\abc\\..\\lib\\test.txt",则找不到文件
    2003-06-03: DeleteAndRenameFile, 增加互斥 
    2003-05-30: CreateFile, 将对参数的检查放到fsmain
    2003-05-21: notify shell 的buffer放到 RAMFSDVOL 内
    2003-05-21: 不能用 stnicmp 和 stnicmp
	           当系统有name=StorgeDisk0时， 
			   stnicmp( StorgeDisk, StorgeDisk0, 10 )
			   回return 0
			   修改 FindOpened and FindOpenedSearchHandle
    2003-05-20: LN, CreateFile, SetFileAttributes， 加入对参数的检查
	            将FileNodeCompare改为FileNameCompare  
    2003-04-30: LN, 多次调用CopyFile( "a.txt", "b.txt" )时有错
******************************************************/


#ifdef WINCE_EML
    #include <windows.h> 
    #include <winioctl.h>
    #include <diskio.h>
    #include <string.h>
    #include <fat.h>
//#include <fsdmgr.h>
//#include <fsddrv.h>
#else
    #include <eframe.h>
    #include <efile.h>
    #include <edevice.h>
    #include <eassert.h>    
    #include <eobjlist.h>
    #include <efsdmgr.h>

    #include <epdev.h>
    #include <epfs.h>
    #include <estring.h>
	#include <estdlib.h>
    
#endif


#define EFILE_VERSION  0x01000001
#define NULL_SECTOR 0xffffffff

#define USER_NODES   2
#define ALL_NODES     0

//#define USE_USERHEAP 
//#ifdef USE_USERHEAP

//#include <ealloc.h>
//#define malloc  malloc
//#define free   free
//#define _krealloc realloc

//#endif

/*
#ifdef WINCE_EML
#define malloc  malloc
#define free   free
#define strncmp  wcsncmp
#define strcpy   wcscpy
#define strncpy  wcsncpy
#define strlen   wcslen
#define strcat   wcscat
#define stricmp  wcsicmp
#define strcmp   wcscmp
#endif
*/

const TCHAR idRam[] = TEXT("EOFS");
const TCHAR szVolMark[] = TEXT( "\\Vol:" );
// size of RAM_FILE_HEAD = Sector size, Sector Size must >= 32 bytes
#define FATBITS DWORD
typedef struct _RAM_FILE_HEAD
{
    BYTE  idName[4]; // EOFS
    DWORD ver;      //
	DWORD dwSerialNum;
	DWORD dwTotalSectors;
    DWORD dwFatStartSector;  // file alloc table start sector
    DWORD dwFatSectorNum;    // file alloc sector number
    DWORD dwFreeSector;
    DWORD nFreeCount;
    DWORD dwDataStartSector; // data start sector
}RAM_FILE_HEAD, * PRFH;


//#define FILE_NAME_LEN (28+1+3)
// align 32 byte
#define NODE_EMPTY   0
#define NODE_DELETED 0xfe
#define NODE_MARK    0xef

#define IS_NAME_BREAK( b ) ( (b) == '\\' || (b) == '/' )
/*
typedef struct _NODE
{
    TCHAR strName[FILE_NAME_LEN+1];  //if strName[0] = 0, mean NODE_EMPTY; 
                                     //= 1mean is NODE_DELETED else file node
    WORD wStartSector;
    WORD wAttrib;
	WORD  nChildNodeNum;   // if the node is folder, it's valid;

    FILETIME ft;         // 6  4  5  5  6  6  //64 years
    DWORD nFileLength;
}NODE, * PNODE;
*/

//#define NODE_EMPTY   0
//#define NODE_DELETED 0x80

//#define GET_NODE_NUM( bFlag ) ( (bFlag) & 0xf );
#define GET_NODE_TYPE( lpNode ) ( (lpNode)->bFlag )

//#define FIRST_NODE_NAME_LEN (8+3)
//#define OTHER_NODE_NAME_LEN (31)
#define NODE_ALIGN_SIZE 32
#define MAX_FILE_NAME_LEN 256

// the node struct must == NODE_ALIGN_SIZE and the  strName must >= 2, because the name :'..'3 
#define NODE_RESERVE_NAME_LEN 12
typedef struct _NODE
{
	//WORD wNodeFlag;  // NODE_EMPTY, NODE_DELETED, NODE_FIRST,
	BYTE bFlag; // 0 : EMPTY, 0x80 NODE_DELETED, (bFlag & 0x0F) = node Count
	BYTE bNameLen;
	WORD wAttrib; // file attrib
	//BYTE bCount;// node count
	//WORD nChildNodeNum;
	DWORD dwStartSector;  // total 8 byte
	//WORD wReserve;// reverve for > 32mbyte disk = 0

	FILETIME ft;  // create time 8 byte
	DWORD nFileLength;    // 4 byte
//	DWORD dwReserve;// = 0
	// the NAME_LEN must align 32 bytes 
	char strName[NODE_RESERVE_NAME_LEN];// file name  11 byte
}NODE, * PNODE;

#define MAX_NODE_BYTES ( ( MAX_FILE_NAME_LEN + sizeof( NODE ) - NODE_RESERVE_NAME_LEN + NODE_ALIGN_SIZE - 1 ) / NODE_ALIGN_SIZE * NODE_ALIGN_SIZE  )
#define GET_NODE_SIZE( pNode ) ( (pNode)->bNameLen + sizeof( NODE ) - NODE_RESERVE_NAME_LEN )
#define GET_NODE_ATTRIB_SIZE( pNode ) ( sizeof( NODE ) - NODE_RESERVE_NAME_LEN )
#define GET_NODE_NUM( pNode ) ( ( (pNode)->bNameLen + sizeof( NODE ) -  NODE_RESERVE_NAME_LEN + NODE_ALIGN_SIZE - 1 ) / NODE_ALIGN_SIZE )

//LN:2003.3.21-add the struct 
typedef struct _FILENODE
{
	struct _FILENODE *lpNext;
	LPTSTR lpszFileName;  // full path name
	PNODE pNode;
	DWORD dwRefCount;

    DWORD dwFolderSector;//folder's first sector of file node's 
    DWORD dwNodeSector;//file node at the which sector
    DWORD index;////file node index at the sector
	//WORD flag; // 1 normal file, 0, vol file  //LN:2003.3.21-add

    CRITICAL_SECTION csFileNode;
	BYTE bNodeBuf[MAX_NODE_BYTES]; // a buf for pNode, the pNode pointer the buffer
}FILENODE, * LPFILENODE;

//

struct _RAMFSDVOL;

typedef struct SF{
	struct SF * lpNext;
    // in
    struct _RAMFSDVOL FAR * lpVol;
    HANDLE hProc;
    LPTSTR lpszName;   // path file name
    void * pSector;
	PNODE pCurNode;
	// 
    //WORD nNodesPerSector;
	DWORD dwFolderStartSector;  // start sector of the folder
    DWORD dwCurNodeSector;   // current sector with pSector    

    WORD nCurNodeIndex;	// current index  with pSector
	WORD nCurNodeNum;
 

 //   WORD wCurFileNodeIndex; // index of file node base
	DWORD dwCurNameLen;      //current file name len
    
	//LPTSTR lpszCurFolder;   // temp var
    LPTSTR lpszCurName;   // current file name
//	PNODE pCurNode;	// current pointer with pSector    

    DWORD dwAddNodeSector;  // sector of has empty node in current sector
    UINT nEmptyNodeIndex;   // index of has empty node in current sector
    UINT nDelNodeIndex;
	UINT nFreeNodeNum;
}SF, * PSF;


#define NORMAL_FILE   0
#define VOL_FILE      1

typedef struct _RAMFSDFILE
{    
    struct _RAMFSDFILE * lpNext;

	//LPRAMFSDVOL lpVol;
	struct _RAMFSDVOL FAR * lpVol;

    HANDLE hProc;
    //LPTSTR lpszFileName;	//LN:2003.3.21-put to filenode struct

    DWORD dwAccess;
    DWORD dwShareMode;
    DWORD dwCreate;
    DWORD dwFlagsAndAttributes;
    //
    //DWORD dwFolderSector;//LN:2003.3.21-put to filenode struct
    //DWORD dwNodeSector;//LN:2003.3.21-put to filenode struct
    //WORD index;//LN:2003.3.21-put to filenode struct
	DWORD flag; // 1 normal file, 0, vol file  


    DWORD dwFilePointer;
    //WORD  wSector;  // relative dwFilePointer
    //WORD  wOffset;  // relative dwFilePointer
    //PNODE_LN lpNodeLN;  // long name node;
	//NODE node[1];// file node data
	LPBYTE lpbCacheBuf;
    UINT  uiCacheValid;
	DWORD dwCachePosStart;
	//DWORD dwCachePosEnd;

	//BYTE bNodeBuf[MAX_NODE_BYTES]; //LN:2003.3.21-put to filenode struct
	LPFILENODE lpfn;

}RAMFSDFILE, FAR * LPRAMFSDFILE;

//#define GET_NODE_PTR( lpFile ) ( (PNODE)(lpFile)->bNodeBuf )
#define GET_NODE_PTR( lpFile ) ( (lpFile)->lpfn->pNode )

#define GET_NODE_OFFSET( index, lpVol ) ( (index) % (lpVol)->nNodesPerSector * NODE_ALIGN_SIZE ) 
//(WORD)( ( (index) * NODE_ALIGN_SIZE ) % (lpVol)->fdd.nBytesPerSector )
typedef SF RAMFSDFIND;
typedef SF FAR *  LPRAMFSDFIND;

typedef struct _RAMFSDVOL
{
    struct _RAMFSDVOL * lpNext;

    HVOL hVol;
    HDSK hDsk;
	TCHAR szVolName[32];
    FSD_DISK_DATA fdd;
    RAM_FILE_HEAD  rfh;
	WORD fUpdate;
	WORD nNodesPerSector;
#define FATBYTES  ( sizeof( DWORD ) )
    DWORD * lpdwFat;    // file alloc table
	DWORD dwMinNeedUpdate;
	DWORD dwMaxNeedUpdate;
	
	CRITICAL_SECTION csNode;
//	CRITICAL_SECTION csNodeList;
	CRITICAL_SECTION csFat;

	CRITICAL_SECTION csFileList;
	CRITICAL_SECTION csFindList;
 
	LPRAMFSDFILE lpOpenFile;
	LPRAMFSDFIND lpFindFile;  
//	LPFILENODE   lpFileNode;

    SHELLFILECHANGEFUNC pscf;
	TCHAR szNotifyPath0[MAX_PATH];//LN 2003-05-21
	TCHAR szNotifyPath1[MAX_PATH];//LN 2003-05-21
}RAMFSDVOL, FAR * LPRAMFSDVOL;


#define FILE_ZONE  0
#define FILE_ERROR 1

static LPRAMFSDVOL lpListVol = NULL;
static CRITICAL_SECTION csVolumeList;

static LPRAMFSDVOL CreateVolume( void );
static BOOL RemoveVolume( LPRAMFSDVOL );
static LPRAMFSDVOL FindVolume( HDSK hDsk );
static BOOL AddToVolumeList( LPRAMFSDVOL );

static DWORD ReadSectors( LPRAMFSDVOL lpVol, DWORD dwStartSector, 
						  DWORD nOffset, LPBYTE lpBuf, DWORD dwSize,
						  LPBYTE pSector, UINT * lpCacheValid );

static DWORD WriteSectors( LPRAMFSDVOL lpVol, 
						   DWORD dwStartSector, 
						   DWORD nOffset, 
						   const unsigned char * lpBuf, 
						   DWORD dwSize,
						   LPBYTE pSector // temp buf
 						   );

static BOOL BeginSearch( SF * lpsf, LPRAMFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName );
static BOOL SearchNode( 
					   SF * lpsf );
static void EndSearch( SF * lpsf );
static BOOL AddFileHandleToVolume( LPRAMFSDFILE lpFile );
static BOOL AddFindHandleToVolume( LPRAMFSDFIND lpFind );
static LPCTSTR IgnoreSpace( LPCTSTR lpcszFileName );

#ifdef WINCE_EML
#define STATIC
#endif

#ifdef KINGMOS
#define STATIC static
#endif

STATIC BOOL KFSD_CloseFile( PFILE pf );
STATIC BOOL KFSD_CloseVolume( PVOL pVol );
STATIC BOOL KFSD_CopyFile( PVOL pVol, LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists );
STATIC BOOL KFSD_CreateDirectory( PVOL pVol, LPCTSTR lpszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes );
STATIC HANDLE KFSD_CreateFile( PVOL pVol, HANDLE hProc, LPCTSTR lpszFileName, DWORD dwAccess, DWORD dwShareMode,PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate,DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ); 
STATIC BOOL KFSD_DeleteAndRenameFile( PVOL pVol, LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName );
STATIC BOOL KFSD_DeleteFile( PVOL pVol, LPCTSTR lpszFileName );
STATIC BOOL KFSD_DeviceIoControl( DWORD pf, DWORD dwIoControlCode, LPVOID pInBuf, DWORD nInBufSize, LPVOID pOutBuf, DWORD nOutBufSize, LPDWORD pBytesReturned );
STATIC BOOL KFSD_FindClose( PFIND pfd );
STATIC HANDLE KFSD_FindFirstFile( PVOL pVol, HANDLE hProc, LPCTSTR lpszFileSpec, FILE_FIND_DATA * pfd ); 
STATIC BOOL KFSD_FindNextFile( PFIND pfd, FILE_FIND_DATA * pffd );
STATIC BOOL KFSD_FlushFileBuffers( PFILE pf );
STATIC BOOL KFSD_GetDiskFreeSpace( PVOL pVol, LPCTSTR lpszPathName, LPDWORD pSectorsPerCluster, LPDWORD pBytesPerSector, LPDWORD pFreeClusters, LPDWORD pClusters );
STATIC DWORD KFSD_GetFileAttributes( PVOL pVol, LPCTSTR lpszFileName );
STATIC BOOL KFSD_GetFileInformationByHandle( PFILE pf, FILE_INFORMATION * phfi );
STATIC DWORD KFSD_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh );
STATIC BOOL KFSD_GetFileTime( PFILE pf, FILETIME * pft, FILETIME *, FILETIME * );
STATIC BOOL KFSD_MountDisk( HDSK hdsk );
STATIC BOOL KFSD_MoveFile( PVOL pVol, LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName );
STATIC void KFSD_Notify( PVOL pVol, DWORD dwFlags );
STATIC BOOL KFSD_ReadFile( PFILE pf, LPVOID lpBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead,  LPOVERLAPPED pOverlapped );
STATIC BOOL KFSD_ReadFileWithSeek( PFILE pf, LPVOID pBuffer, DWORD cbRead, LPDWORD pcbRead, LPOVERLAPPED pOverlapped, DWORD dwLowOffset, DWORD dwHighOffset);
STATIC BOOL KFSD_RegisterFileSystemFunction( PVOL pVol, SHELLFILECHANGEFUNC pft );
STATIC BOOL KFSD_RemoveDirectory( PVOL pVol,LPCTSTR lpszPathName ); 
STATIC BOOL KFSD_SetEndOfFile( PFILE pf );
STATIC BOOL KFSD_SetFileAttributes( PVOL pVol, LPCTSTR lpszFileName, DWORD dwFileAttributes );
STATIC DWORD KFSD_SetFilePointer( PFILE pf, LONG lDistanceToMove, LPLONG pDistanceToMoveHigh, DWORD dwMothod );
STATIC BOOL KFSD_SetFileTime( PFILE pf, const FILETIME *pft, const FILETIME *, const FILETIME * );
STATIC BOOL KFSD_UnmountDisk( HDSK hdsk );
STATIC BOOL KFSD_WriteFile( PFILE pf, LPCVOID lpBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );
STATIC BOOL KFSD_WriteFileWithSeek( PFILE pFile, LPCVOID pBuffer, DWORD cbWrite, LPDWORD pcbWritten, DWORD dwLowOffset, DWORD dwHighOffset );

#define NEXT_SECTOR( pfat, dwSector )  (*((pfat)+(dwSector)))

#ifdef KINGMOS

static const FSDDRV drvFSD = {  
    KFSD_CloseFile,
    KFSD_CloseVolume,
    KFSD_CreateDirectory,
    KFSD_CreateFile,
    KFSD_DeleteAndRenameFile,
    KFSD_DeleteFile,
    KFSD_DeviceIoControl,
    KFSD_FindClose,
    KFSD_FindFirstFile,
    KFSD_FindNextFile,
    KFSD_FlushFileBuffers,
    KFSD_GetDiskFreeSpace,
    KFSD_GetFileAttributes,
    KFSD_GetFileInformationByHandle,
    KFSD_GetFileSize,
    KFSD_GetFileTime,
    KFSD_MountDisk,
    KFSD_MoveFile,
    KFSD_Notify,
    KFSD_ReadFile,
    KFSD_ReadFileWithSeek,
    KFSD_RegisterFileSystemFunction,
    KFSD_RemoveDirectory,
    KFSD_SetEndOfFile,
    KFSD_SetFileAttributes,
    KFSD_SetFilePointer,
    KFSD_SetFileTime,
    KFSD_UnmountDisk,
    KFSD_WriteFile,
    KFSD_WriteFileWithSeek,
	KFSD_CopyFile
};

//static CRITICAL_SECTION csNode;
//static CRITICAL_SECTION csFat;


BOOL _InstallDefaultFAT( void )
{
	InitializeCriticalSection( &csVolumeList );
	csVolumeList.lpcsName = "CS-VOL";
	//InitializeCriticalSection( &csNode );
	//InitializeCriticalSection( &csFat );
    return FSDMGR_RegisterFSD ( "KFSD", &drvFSD );
}

void _UnInstallDefaultFAT( void )
{
	FSDMGR_UnregisterFSD(  "KMFS" );
	DeleteCriticalSection( &csVolumeList );
}

#endif

static LPFILENODE CreateFileNode( LPCTSTR lpcszFileName )
{
	LPFILENODE lpfn = ( LPFILENODE)malloc( sizeof( FILENODE ) + (strlen( lpcszFileName ) + 1) * sizeof( TCHAR ) );
	if( lpfn )
	{
		memset( lpfn, 0, sizeof( FILENODE ) );
		lpfn->pNode = (PNODE)lpfn->bNodeBuf;
		lpfn->lpszFileName = (LPTSTR)(lpfn + 1);
		strcpy( lpfn->lpszFileName, lpcszFileName );
	    InitializeCriticalSection( &lpfn->csFileNode );
	}
	return lpfn;
}

static void DeleteFileNode( LPFILENODE lpfn )
{
	DeleteCriticalSection( &lpfn->csFileNode );
	free( lpfn );
}

static LPCTSTR IgnoreSpace( LPCTSTR lpcszFileName )
{
	while( *lpcszFileName == ' ' )
		lpcszFileName++;
	return lpcszFileName;
}

static LPRAMFSDVOL CreateVolume( void )
{
    LPRAMFSDVOL lpVol;
    lpVol = (LPRAMFSDVOL)malloc( sizeof( RAMFSDVOL ) );
    if( lpVol )
	{
        memset( lpVol, 0, sizeof( RAMFSDVOL ) );
		InitializeCriticalSection( &lpVol->csNode );
		lpVol->csNode.lpcsName = "CS-NODE";

		InitializeCriticalSection( &lpVol->csFat );
		lpVol->csFat.lpcsName = "CS-FAT";

		//InitializeCriticalSection( &lpVol->csNodeList );
		//lpVol->csFileList.lpcsName = "CS-NODELIST";


		InitializeCriticalSection( &lpVol->csFileList );
		lpVol->csFileList.lpcsName = "CS-FILELIST";

		InitializeCriticalSection( &lpVol->csFindList );
		lpVol->csFindList.lpcsName = "CS-FINDLIST";

	    lpVol->dwMinNeedUpdate = -1;
		lpVol->dwMaxNeedUpdate = 0;
	}


    return lpVol;
}

static void DeleteVolume( LPRAMFSDVOL lpVol )
{
	free( lpVol->lpdwFat );
    free( lpVol );    
}

static LPRAMFSDVOL FindVolume( HDSK hDsk )
{
    LPRAMFSDVOL lpList = NULL;

	EnterCriticalSection( &csVolumeList );
	
	lpList = lpListVol;
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: ReadDisk Success\r\n")));
	while( lpList )
	{
		if( lpList->hDsk == hDsk )
		{
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: the volume exist, serial=%x.\r\n"), lpList->rfh.dwSerialNum ) );
			break;
		}
		lpList = lpList->lpNext;
	}
				
	LeaveCriticalSection( &csVolumeList );
	return lpList;

/*
    BOOL retv = FALSE;
	FSD_DISK_DATA  fdd;
    LPRAMFSDVOL lpList = NULL;


	if( ERROR_SUCCESS == FSDMGR_GetDiskInfo( hDsk, &fdd ) )
	{
		void * pSector = malloc( fdd.nBytesPerSector );
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetDiskInfo Success\r\n")));
		if( pSector )
		{
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: kalloc Success\r\n")));
			if( ERROR_SUCCESS == FSDMGR_ReadDisk( hDsk, 0, 1, pSector, fdd.nBytesPerSector ) )
			{				
				PRFH prfh = (PRFH)pSector;

                EnterCriticalSection( &csVolumeList );

				lpList = lpListVol;
				RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: ReadDisk Success\r\n")));
				while( lpList )
				{
					if( lpList->rfh.dwSerialNum == prfh->dwSerialNum )
					{
						RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: the volume exist, serial=%x.\r\n"), lpList->rfh.dwSerialNum ) );
						break;
					}
					lpList = lpList->lpNext;
				}

				LeaveCriticalSection( &csVolumeList );
			}
			free( pSector );
		}
	}
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindVolume = 0x%x, Serial=%d \r\n"), lpList, lpList == 0 ? 0 : lpList->rfh.dwSerialNum ));
	return lpList;
*/
}

static BOOL RemoveVolume( LPRAMFSDVOL lpVol )
{
    BOOL retv = FALSE;

	EnterCriticalSection( &csVolumeList );

    if( lpListVol )
    {
        LPRAMFSDVOL lpList = lpListVol;
        if( lpListVol == lpVol )
        {
            lpListVol = lpListVol->lpNext;
            retv =  TRUE;
        }
        else
        {
            while( lpList->lpNext )
            {
                if( lpList->lpNext == lpVol )
                {
                    lpList->lpNext = lpVol->lpNext;
                    retv =  TRUE;
                    break;
                }
                lpList = lpList->lpNext;
            }
        }
    }

	LeaveCriticalSection( &csVolumeList );

    return retv;
}

static BOOL AddToVolumeList( LPRAMFSDVOL lpvol )
{
	BOOL bRetv = FALSE;

	EnterCriticalSection( &csVolumeList );

    if( lpvol->lpNext == 0 )
    {
        lpvol->lpNext = lpListVol;
        lpListVol = lpvol;
        bRetv = TRUE;
    }

	LeaveCriticalSection( &csVolumeList );

    return bRetv;
}

static void CheckFat( LPRAMFSDVOL lpVol )
{
//	int il;
		// for check
	EnterCriticalSection( &lpVol->csFat );

	{
		DWORD dwSector = lpVol->rfh.dwFreeSector;
		DWORD num = lpVol->rfh.nFreeCount;
		DWORD i;
		DWORD end = (lpVol->fdd.nSectors - 1);
		DWORD *lpdwFat;
		
		
		
		lpdwFat = lpVol->lpdwFat;
		
		for( i = 0; i < num; i++ )
		{
			if( dwSector > end )
			{
				RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Error Alloc=%d, i=%d, num=%d=====================================\r\n"), dwSector, i, num));
				break;
			}
			dwSector = *(lpdwFat+dwSector);
		}

	}
	LeaveCriticalSection( &lpVol->csFat );
}

static BOOL FreeSector( LPRAMFSDVOL lpVol, DWORD dwStartSector ) 
{
    DWORD * lpdwFat = lpVol->lpdwFat;
    DWORD n, w, k;//, dwMin, dwMax;	

	//CheckFat(lpVol);

    ASSERT( dwStartSector != NULL_SECTOR );
	if( dwStartSector == NULL_SECTOR )
		return TRUE;
	
	EnterCriticalSection( &lpVol->csFat );

    //dwMax = 0;
	//dwMin = 0xffffffff;

	k = w = dwStartSector;
    for( n = 0; w != NULL_SECTOR; n++ )
	{
        k = w;
		//dwMin = MIN( dwMin, w );
		//dwMax = MAX( dwMax, w );
		w = NEXT_SECTOR( lpdwFat, w );		
	}

    *(lpdwFat+k) = lpVol->rfh.dwFreeSector;
	lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, k );
	lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, k );


    lpVol->rfh.dwFreeSector = dwStartSector;
    lpVol->rfh.nFreeCount += n;
	lpVol->fUpdate = 1;


	LeaveCriticalSection( &lpVol->csFat );

    return TRUE;
}

static DWORD AllocSector( LPRAMFSDVOL lpVol, DWORD nSectorNum )
{

    EnterCriticalSection( &lpVol->csFat );

	{
		DWORD first, n, k;
		DWORD * pfat;
		DWORD end = (lpVol->fdd.nSectors - 1);
		
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: AllocSector=%d, end=%d, free count=%d\r\n"), nSectorNum, end, lpVol->rfh.nFreeCount));
		
		//CheckFat(lpVol);
		
		if( nSectorNum > 0 && lpVol->rfh.nFreeCount >= nSectorNum )
		{        
			first = lpVol->rfh.dwFreeSector;
			k = (nSectorNum-1);
			pfat = lpVol->lpdwFat;
			for( n = 0; n < k; n++ )
			{
				first = *(pfat+first);  // get next block
				if( first > end )
				{
					RETAILMSG(1, (TEXT("error in KFSD's AllocSector:  Alloc=%d, n=%d\r\n"), first, n));
				}
			}
			n = first;  // end block
			first = *(pfat+first);  // get next block
			*(pfat+n) = NULL_SECTOR;
			lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, n );
            lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, n );

			// save return value
			n = lpVol->rfh.dwFreeSector;
			lpVol->rfh.dwFreeSector = first;
			// reset free block and count
			lpVol->rfh.nFreeCount -= nSectorNum;
			lpVol->fUpdate = 1;

			LeaveCriticalSection( &lpVol->csFat );

			return n;
		}
		SetLastError(ERROR_DISK_FULL);
		RETAILMSG(1, (TEXT("error in KFSD's AllocSector: AllocSector no enough Sector.\r\n")));
	}

	LeaveCriticalSection( &lpVol->csFat );

    return NULL_SECTOR;
}

static DWORD ReallocSector(
                          LPRAMFSDVOL lpVol, 
                          DWORD dwAllocSector, 
                          DWORD nSectorNum )
{
    DWORD i;
	DWORD dwRetv = NULL_SECTOR;
	
	//CheckFat(lpVol);

    if( dwAllocSector == NULL_SECTOR )
    {
        return AllocSector( lpVol, nSectorNum );
    }
    else if( nSectorNum == 0 )
    {   // free all block
        FreeSector( lpVol, dwAllocSector );
        return NULL_SECTOR;
    }
    else
    {
        DWORD s = dwAllocSector;
        DWORD p = dwAllocSector;
        DWORD * pfat = lpVol->lpdwFat;
        i = 0;

		EnterCriticalSection( &lpVol->csFat );

        while( s != NULL_SECTOR )
        {
            i++;
            if( i > nSectorNum )
            {  // decrease sector, to free other sector
                RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RealocSector decrease\r\n")));
				*(pfat+p) = NULL_SECTOR;
                lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, p );
				lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, p );
				lpVol->fUpdate = 1;

                FreeSector( lpVol, s );
                dwRetv = dwAllocSector;
				goto _RETV;
            }
            p = s;
            s = NEXT_SECTOR( pfat, s );
        }  
        if( i < nSectorNum )
        {            
			s = AllocSector( lpVol, (nSectorNum - i) );
            if( s != NULL_SECTOR )
			{
                *(pfat+p) = s;   // link them
                lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, p );
				lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, p );


				lpVol->fUpdate = 1;
				RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RealocSector crease success\r\n")));
				dwRetv = dwAllocSector;
				goto _RETV;
			}
			else
			{
				//RETAILMSG(1, (TEXT("KFSD: RealocSector failure!.\r\n")));
				dwRetv = NULL_SECTOR;
				goto _RETV;
				//return NULL_SECTOR;
			}
        }
        else if( i == nSectorNum )
		{
			dwRetv = dwAllocSector;
			goto _RETV;
            //return wAllocSector;
		}

_RETV:
        LeaveCriticalSection( &lpVol->csFat );
    }


    return dwRetv;
}

static DWORD LinkSector( LPRAMFSDVOL lpVol, DWORD dwDest, DWORD dwSource )
{
    DWORD dwSector = dwDest;

	//CheckFat(lpVol);    

	EnterCriticalSection( &lpVol->csFat );

	ASSERT( dwDest != NULL_SECTOR && dwSource != NULL_SECTOR );
    while( *(lpVol->lpdwFat+dwSector) != NULL_SECTOR )
        dwSector = *(lpVol->lpdwFat+dwSector);
    *(lpVol->lpdwFat+dwSector) = dwSource;
    lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, dwSector );
    lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, dwSector );


	lpVol->fUpdate = 1;

	LeaveCriticalSection( &lpVol->csFat );

    return dwDest;
}

static DWORD GetSectorOffset( LPRAMFSDVOL lpVol, DWORD dwStartSector, UINT nOffset )
{
    DWORD * pfat = lpVol->lpdwFat;

	EnterCriticalSection( &lpVol->csFat );

    while( dwStartSector != NULL_SECTOR && nOffset )
    {
        dwStartSector = NEXT_SECTOR( pfat, dwStartSector );        
        nOffset--;
    }

	LeaveCriticalSection( &lpVol->csFat );

    return dwStartSector;
}

static DWORD GetSectorNum( LPRAMFSDVOL lpVol, DWORD dwStartSector )
{
    DWORD * pfat = lpVol->lpdwFat;
	DWORD i = 0;

	EnterCriticalSection( &lpVol->csFat );

    while( dwStartSector != NULL_SECTOR  )
    {
        dwStartSector = NEXT_SECTOR( pfat, dwStartSector );
        i++;
    }

	LeaveCriticalSection( &lpVol->csFat );

    return i;
}

static BOOL ReallocFile( LPRAMFSDFILE lpFile, DWORD dwSize )
{
    DWORD dwStartSector;
    if( dwSize != GET_NODE_PTR(lpFile)->nFileLength )
    {
        dwStartSector = ReallocSector( lpFile->lpVol, 
			                          GET_NODE_PTR(lpFile)->dwStartSector, 
									  ( (dwSize + lpFile->lpVol->fdd.nBytesPerSector - 1) / lpFile->lpVol->fdd.nBytesPerSector ) );
        if( dwStartSector != NULL_SECTOR || dwSize == 0 )
        {
            GET_NODE_PTR(lpFile)->dwStartSector = dwStartSector;
            GET_NODE_PTR(lpFile)->nFileLength = dwSize;
        }
        else
            return FALSE;       
    }
    return TRUE;
}

static int GetFolderAdr( LPTSTR * lppFolder, LPCTSTR lpcfn, int * lpNextLen )
{
    int i = 0;
	int fn = 0;
    if( IS_NAME_BREAK(*lpcfn)  )
	{
        lpcfn++;  ///parent level
		i++;
	}
    while( *lpcfn && fn < MAX_FILE_NAME_LEN  )
    {
        if( !IS_NAME_BREAK(*lpcfn) )
        {
			if( fn == 0 )
	            *lppFolder = (LPTSTR)lpcfn;

            //*lpFolder++ = *lpcfn++;
			lpcfn++;
            i++; fn++;
        }
        else
            break;
    }
	//if( i < FILE_NAME_LEN )
	//*lpFolder++ = 0;
	*lpNextLen = i;
    return fn;
}


//#define OP_DELFILE       0x0001    // delete node if found
//#define OP_DELFOLDER     0x0002    // delete node if found
#define OP_DEL           0x0001  //(OP_DELFILE | OP_DELFOLDER)
#define OP_ADD           0x0004    // add node if not found, the lpFile->node is valid
#define OP_UPDATE        0x0008    // update node, the lpFile->node is valid
#define OP_GET           0x0010    // get node, fill lpFile->node


static BOOL DoFolderInit( 
						 LPRAMFSDVOL lpVol , 
						 DWORD dwFolderSector, 
						 DWORD dwParentSector, 
						 WORD * pSector )
{   
    PNODE pNode = (PNODE)pSector;


    memset( pSector, 0, lpVol->fdd.nBytesPerSector );
    // init this folder
	pNode->bFlag = NODE_MARK;
    pNode->strName[0] = '.';
    //pNode->strName[1] = 0;
    pNode->dwStartSector = dwFolderSector;
	pNode->bNameLen = 1;
	pNode->wAttrib |= FILE_ATTRIBUTE_DIRECTORY;
	// remove it for test 
	//pNode->wAttrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

    // init parent folder
    pNode = (PNODE)( (LPBYTE)pSector + NODE_ALIGN_SIZE );
	pNode->bFlag = NODE_MARK;
    pNode->strName[0] = '.';
    pNode->strName[1] = '.';
	pNode->bNameLen = 2;
    //pNode->strName[2] = 0;
    pNode->dwStartSector = dwParentSector;
	pNode->wAttrib |= FILE_ATTRIBUTE_DIRECTORY;
	// remove it for test 
	//pNode->wAttrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

    if( ERROR_SUCCESS == FSDMGR_WriteDisk( lpVol->hDsk, dwFolderSector, 1, (LPBYTE)pSector, lpVol->fdd.nBytesPerSector ) )
		return TRUE;
	return FALSE;
}
/*
static BOOL FileNodeCompare( LPCTSTR lpcMask, int iMaskLen, LPCTSTR lpcSrc, int iSrcLen )
{
	//int nMask, nSrc;
	//BOOL bContinue = TRUE;
	LPCTSTR lpcMaskEnd, lpcSrcEnd;
	//int len = min( iMaskLen, iSrcLen );

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FileNameCompare- Dest=%s , Src =%s \r\n"), lpcDest, lpcSrc));
	//for( n = 0; n < len && bContinue; )//&& *lpcSrc && *lpcMask; )
	//nMask = nSrc = 0;
	lpcMaskEnd = lpcMask + iMaskLen;
	lpcSrcEnd = lpcSrc + iSrcLen;
	while( lpcMask < lpcMaskEnd && lpcSrc < lpcSrcEnd )
	{
		if( *lpcMask == '*' )
		{   // skip , get next name
			BOOL bContinue = FALSE;

			lpcMask++;			
			while( lpcMask < lpcMaskEnd )
			{
				if( *lpcMask++ == '.' )
				{
					bContinue = TRUE;
					break;
				}
			}

			if( bContinue == FALSE )
				return TRUE;  //no any char need to match
			
			while( lpcSrc < lpcSrcEnd )
			{
				if( *lpcSrc++ == '.' )
					break;
			}
			continue;
		}
		else if( *lpcMask == '?' || *lpcMask == *lpcSrc )
		{
			;  // continue next char
		}
		else if( *lpcMask >= 'A' && *lpcMask <= 'Z' )
		{
			if( ((int)*lpcMask - (int)*lpcSrc) != ('A' - 'a') )
				return FALSE;
		}
		else if( *lpcMask >= 'a' && *lpcMask <= 'z' )
		{
			if( ((int)*lpcMask - (int)*lpcSrc) != ('a' - 'A') )
				return FALSE;
		}
		else
			return FALSE;
		lpcSrc++; 
		lpcMask++;
	}

	if( lpcMask == lpcMaskEnd && lpcSrc == lpcSrcEnd )
	{  // compare all char
		return TRUE;
	}

	if( lpcSrc == lpcSrcEnd )
	{
		while( lpcMask < lpcMaskEnd )//n < iMaskLen )
		{
			if( *lpcMask == '?' || *lpcMask == '*' || *lpcMask == '.' )
			{
				lpcMask++;
				continue;
			}
			else
				return FALSE;
		}
		return TRUE;
	}

	// the n == iMaskLen but n < iSrcLen
	//if( n != iMaskLen )
		//n = iMaskLen;

	ASSERT( lpcMask == lpcMaskEnd );
	return FALSE;


//	if( n < iSrcLen || n < iMaskLen )
//	{
//		if( n < iMaskLen && *lpcMask != '*' )
//			return FALSE;
//	}
//	return TRUE;
}
*/

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
static BOOL SearchFolder( SF * psf, BOOL bNext, int nStartIndex )
{
    LPCTSTR lpcszFileName = psf->lpszCurName;//Folder;
    int iFileNameLen = psf->dwCurNameLen;
	PNODE lpNode;// = (PNODE)psf->pCurNode;
	UINT uiCurNodeNum = 0;
	UINT uiCurNodeIndex;
	UINT uiSectorOffset;
	DWORD dwCurSector;
	DWORD dwOffset;
	DWORD nBytesPerSector = psf->lpVol->fdd.nBytesPerSector;
	BOOL bUpdate, bRetv = FALSE;
    
    if( bNext )
    {   // search next file match with filename
        uiCurNodeIndex = psf->nCurNodeIndex + psf->nCurNodeNum;
    }
    else
    {
		uiCurNodeIndex = nStartIndex;//2;  // LN, 2003-07-16,假如路经为 "\\abc\\..\\lib\\test.txt",则找不到文件
		psf->dwAddNodeSector = NULL_SECTOR;
		psf->nEmptyNodeIndex = -1;
		psf->nDelNodeIndex = -1;
    }

	uiSectorOffset = uiCurNodeIndex / psf->lpVol->nNodesPerSector;//(WORD)(uiCurNodeIndex * NODE_ALIGN_SIZE / nBytesPerSector);
	dwCurSector = GetSectorOffset( psf->lpVol, psf->dwFolderStartSector, uiSectorOffset );//(WORD)(uiCurNodeIndex * NODE_ALIGN_SIZE / nBytesPerSector) );
    dwOffset = ( (uiCurNodeIndex * NODE_ALIGN_SIZE) % nBytesPerSector );

	bUpdate = TRUE;

	EnterCriticalSection( &psf->lpVol->csNode );


	while( dwCurSector != NULL_SECTOR )
    {                 
        // search node in sector
        if( bUpdate )
		{
			if( FSDMGR_ReadDisk( psf->lpVol->hDsk, dwCurSector, 1, psf->pSector, psf->lpVol->fdd.nBytesPerSector ) != ERROR_SUCCESS )
				break;
		}
		lpNode = (PNODE)( (LPBYTE)psf->pSector + dwOffset );
		if( lpNode->bFlag == NODE_EMPTY ||
			lpNode->bFlag == NODE_DELETED )
		{
			if( lpNode->bFlag == NODE_EMPTY )
			{
				if( psf->dwAddNodeSector == NULL_SECTOR )
				{
					psf->nEmptyNodeIndex = uiCurNodeIndex;
					psf->dwAddNodeSector = dwCurSector;
					psf->nFreeNodeNum = psf->lpVol->nNodesPerSector - uiCurNodeIndex % psf->lpVol->nNodesPerSector; 
				}
				goto RETV;
			}
			else
			{  // deleted node
				if( psf->dwAddNodeSector == NULL_SECTOR &&
					lpNode->bNameLen >= psf->dwCurNameLen )
				{
					psf->nDelNodeIndex = uiCurNodeIndex;
					psf->dwAddNodeSector = dwCurSector;
					psf->nFreeNodeNum = GET_NODE_NUM( lpNode );
				}				
			}
			uiCurNodeNum = GET_NODE_NUM( lpNode );
		}
		else if( lpNode->bFlag == NODE_MARK )
		{
			uiCurNodeNum = GET_NODE_NUM( lpNode );//(lpNode->bNameLen + sizeof( NODE ) - 1 + NODE_ALIGN_SIZE - 1) / NODE_ALIGN_SIZE;
			if( uiCurNodeNum > 1 && 
				(uiCurNodeNum * NODE_ALIGN_SIZE + dwOffset) > nBytesPerSector )
			{
				lpNode = psf->pCurNode;
		        if( ReadSectors( psf->lpVol, dwCurSector, dwOffset, (LPBYTE)lpNode, uiCurNodeNum * NODE_ALIGN_SIZE, psf->pSector, NULL ) == 0 )
					goto RETV;
			}
			// // LN, 2003-07-16,加入对 '.' 和 '..' 的比较
			if( lpcszFileName[0] == '.' )
			{   // '.' or '..' ? 
				if( (iFileNameLen == 1 && uiCurNodeIndex == 0) ||
					(iFileNameLen == 2 && uiCurNodeIndex == 1) )
				{   //yes, find it
					bRetv = TRUE;
				}				    
			}
			//if( FileNodeCompare( lpcszFileName, iFileNameLen, lpNode->strName, lpNode->bNameLen ) )
			else
				bRetv = FileNameCompare( lpcszFileName, iFileNameLen, lpNode->strName, lpNode->bNameLen );
			if( bRetv )
			{
				psf->nCurNodeIndex = uiCurNodeIndex;
				psf->nCurNodeNum = uiCurNodeNum;
				psf->dwCurNodeSector = dwCurSector;
				memcpy( psf->pCurNode, lpNode, GET_NODE_SIZE( lpNode ) );
				bRetv = TRUE;
				goto RETV;
			}
		}
		else
		{
			// error
			ASSERT( 0 );
			goto RETV;
		}
		// not find match name
        uiCurNodeIndex += uiCurNodeNum;
		{
			UINT t = uiCurNodeIndex / psf->lpVol->nNodesPerSector;
			bUpdate = uiSectorOffset < t; 
			for( ; uiSectorOffset < t && dwCurSector != NULL_SECTOR; uiSectorOffset++ )
			{
				dwCurSector =  NEXT_SECTOR( psf->lpVol->lpdwFat, dwCurSector );//wStartSector );//NEXT_SECTOR
			}
		}

        dwOffset = GET_NODE_OFFSET( uiCurNodeIndex, psf->lpVol );
	}

RETV:
	LeaveCriticalSection( &psf->lpVol->csNode );
    
    return bRetv;//FALSE;
}


static DWORD NewFolder( LPRAMFSDVOL lpVol, DWORD dwParent, void * lpSectorBuf )
{
	DWORD dwSector;

    dwSector = AllocSector( lpVol, 1 );
	if( dwSector != NULL_SECTOR )
	{
		if( DoFolderInit( lpVol , dwSector, dwParent, lpSectorBuf ) )
			return dwSector;
		else
		{
			FreeSector( lpVol, dwSector );
		}
	}
	return NULL_SECTOR;
}

static DWORD AddNodeSector( PSF psf, DWORD dwNeedSectors )
{
    DWORD dwNodeSector;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoNodeAdd, No Empty Node\r\n")));
//	    wNeedSectors = ( GET_NODE_SIZE( pNode ) + psf->lpVol->fdd.nBytesPerSector - 1 ) / psf->lpVol->fdd.nBytesPerSector;
	dwNodeSector = AllocSector( psf->lpVol, dwNeedSectors );
	if( dwNodeSector != NULL_SECTOR )
	{
		DWORD dwInitSector;
		//wOffset = 0;
			 
        LinkSector( psf->lpVol, psf->dwFolderStartSector, dwNodeSector );
        // init it
		memset( psf->pSector, 0, psf->lpVol->fdd.nBytesPerSector );
		dwInitSector = dwNodeSector;

		EnterCriticalSection( &psf->lpVol->csNode );

		while( dwNeedSectors-- )
		{
			ASSERT_NOTIFY( dwInitSector != NULL_SECTOR, "error sector\r\n" );
			if( FSDMGR_WriteDisk( psf->lpVol->hDsk, dwInitSector, 1, psf->pSector, psf->lpVol->fdd.nBytesPerSector ) != ERROR_SUCCESS )
			{
				dwNodeSector = NULL_SECTOR;
				break;
			}
		    dwInitSector = NEXT_SECTOR( psf->lpVol->lpdwFat, dwInitSector );
		}

		LeaveCriticalSection( &psf->lpVol->csNode );
	}
	return dwNodeSector;
}

static BOOL DoNodeAdd( PSF psf, PNODE pNode )
{
//    PNODE pn;
//    WORD wSector;
	//int iNeedNode = GET_NODE_NUM( pNode->bFlag );
	DWORD dwOffset;
	DWORD dwNodeSector;
	UINT uiNodeIndex;
	DWORD dwNeedSectors;
    DWORD dwNeedNodeNum;
	UINT nBytesPerSector = psf->lpVol->fdd.nBytesPerSector;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoNodeAdd\r\n")));
	
    EnterCriticalSection( &psf->lpVol->csNode );

	if( (dwNodeSector = psf->dwAddNodeSector) == NULL_SECTOR )
	{   // alloc enough sector to save the node
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoNodeAdd, No Empty Node\r\n")));
	    dwNeedSectors = ( GET_NODE_SIZE( pNode ) + nBytesPerSector - 1 ) / nBytesPerSector;
		dwNodeSector = AddNodeSector( psf, dwNeedSectors );
		//wNodeSector = AllocSector( psf->lpVol, wNeedSectors );
		if( dwNodeSector != NULL_SECTOR )
		{
		//	WORD wInitSector;
			dwOffset = 0;			 
          //  LinkSector( psf->lpVol, psf->wFolderStartSector, wNodeSector );
			uiNodeIndex = GetSectorNum( psf->lpVol, psf->dwFolderStartSector ) * psf->lpVol->nNodesPerSector;
		}
		else
		{
			LeaveCriticalSection( &psf->lpVol->csNode );
			return FALSE;
		}
	}
	else
	{
		dwNeedNodeNum = GET_NODE_NUM( pNode );
        if( psf->nDelNodeIndex != -1 )
		{
			ASSERT( dwNeedNodeNum <= psf->nFreeNodeNum );
			//if( GET_NODE_NUM( pNode ) <
			if( dwNeedNodeNum < psf->nFreeNodeNum )
			{
				NODE node;
				DWORD dwFreeSector;
				memset( &node, 0, sizeof( NODE ) );
				node.bFlag = NODE_DELETED;
				//node.bNameLen = (wNeedNodeNum - psf->nFreeNodeNum
				node.dwStartSector = NULL_SECTOR;
				node.bNameLen = (BYTE)( (psf->nFreeNodeNum - dwNeedNodeNum) * NODE_ALIGN_SIZE - sizeof( NODE ) + NODE_RESERVE_NAME_LEN ); 
				// get node index and offset to split 
	            dwFreeSector = GetSectorOffset( psf->lpVol, 
					                           psf->dwFolderStartSector, 
											  (psf->nDelNodeIndex + dwNeedNodeNum) / psf->lpVol->nNodesPerSector );
                dwOffset = GET_NODE_OFFSET( (psf->nDelNodeIndex + dwNeedNodeNum), psf->lpVol );
				ASSERT( dwFreeSector != NULL_SECTOR );
				if( WriteSectors( psf->lpVol, dwFreeSector, dwOffset, (LPBYTE)&node, NODE_ALIGN_SIZE - NODE_RESERVE_NAME_LEN, psf->pSector ) == 0 )
				{
					LeaveCriticalSection( &psf->lpVol->csNode );
	                return FALSE;//
				}

			}
		    // set node index and offset to added
			uiNodeIndex = psf->nDelNodeIndex;
			dwOffset = GET_NODE_OFFSET( psf->nDelNodeIndex, psf->lpVol );
		}
		else
		{   // has empty index
            ASSERT( psf->nEmptyNodeIndex != -1 );
			if( psf->nFreeNodeNum < dwNeedNodeNum )
			{  // add node sector
				dwNeedSectors = ( (dwNeedNodeNum - psf->nFreeNodeNum) * NODE_ALIGN_SIZE + nBytesPerSector - 1 ) / nBytesPerSector;
				if( AddNodeSector( psf, dwNeedSectors ) == NULL_SECTOR )
				{
					LeaveCriticalSection( &psf->lpVol->csNode );
	                return FALSE;// no sector
				}
			}
	        uiNodeIndex = psf->nEmptyNodeIndex;
			dwOffset = GET_NODE_OFFSET( psf->nEmptyNodeIndex, psf->lpVol );//(WORD)( (psf->nEmptyNodeIndex * NODE_ALIGN_SIZE) % psf->lpVol->fdd.nBytesPerSector );
		}
	}

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoNodeAdd, Sector=%d, Index=%d\r\n"), psf->dwAddNodeSector, psf->nEmptyNodeIndex));
//	pn = (PNODE)psf->pSector + psf->nEmptyNodeIndex;
//	*pn = *pNode;
	pNode->bFlag = NODE_MARK;
	
	if( ( dwOffset = WriteSectors( psf->lpVol, dwNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), psf->pSector ) ) )
	{
		psf->nCurNodeIndex = uiNodeIndex;//psf->nEmptyNodeIndex;
		psf->dwCurNodeSector = dwNodeSector;//psf->wEmptyNodeSector;	
		memcpy( psf->pCurNode, pNode, GET_NODE_SIZE(pNode) );
	}

	LeaveCriticalSection( &psf->lpVol->csNode );
		
	return dwOffset != 0;
}


static BOOL DoNodeUpdate( PSF psf, PNODE pNode )//, LPRAMFSDFILE lpFile )
{   
	BOOL bRetv;
	DWORD dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );//(WORD)( (psf->nCurNodeIndex * NODE_ALIGN_SIZE) % psf->lpVol->fdd.nBytesPerSector );

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoNodeUpdate\r\n")));

	EnterCriticalSection( &psf->lpVol->csNode );

	bRetv = WriteSectors( psf->lpVol, psf->dwCurNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), psf->pSector );

	LeaveCriticalSection( &psf->lpVol->csNode );

    if( bRetv && psf->pCurNode != pNode )
	    memcpy( psf->pCurNode, pNode, GET_NODE_SIZE( pNode ) );
    return bRetv;
    
}


static BOOL DoNodeGet( PSF psf, PNODE pNode )//, LPRAMFSDFILE lpFile )
{
	DWORD dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );//(WORD)( (psf->nCurNodeIndex * NODE_ALIGN_SIZE) % psf->lpVol->fdd.nBytesPerSector );

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoNodeGet\r\n")));
	
	memcpy( pNode, psf->pCurNode, GET_NODE_SIZE( psf->pCurNode ) );
	return TRUE;
}

static BOOL DoNodeDel( PSF psf, BOOL bFreeSector )
{
    //WORD wSector = psf->wCurSector;
    //WORD index = psf->nCurIndex;
//	PNODE pNode;
    DWORD dwOffset; 
	BOOL bRetv;

	EnterCriticalSection( &psf->lpVol->csNode );
    
	if( bFreeSector )
	{		
		if( psf->pCurNode->nFileLength ||
			(psf->pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) )
        {
            ASSERT( psf->pCurNode->dwStartSector != NULL_SECTOR );
            if( psf->pCurNode->dwStartSector != NULL_SECTOR )
                FreeSector( psf->lpVol, psf->pCurNode->dwStartSector ); 
        }
        else
        {
            //ASSERT( psf->pCurNode->wStartSector == NULL_SECTOR );
        }
	}

    //psf->pCurNode->strName[0] = NODE_DELETED;
	psf->pCurNode->bFlag = NODE_DELETED;
	dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );//(WORD)( (psf->nCurNodeIndex * NODE_ALIGN_SIZE) % psf->lpVol->fdd.nBytesPerSector );

	bRetv = WriteSectors( psf->lpVol, psf->dwCurNodeSector, dwOffset, (LPBYTE)psf->pCurNode, GET_NODE_SIZE( psf->pCurNode ), psf->pSector );
	
	LeaveCriticalSection( &psf->lpVol->csNode );

	return bRetv;
}

static BOOL DoFolderChange( 
                       SF * psf,
					   PNODE pNode,
                       int op )
{
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DoFolderChange\r\n")));
	switch( op )
	{
	case OP_DEL:
		{
			SF sfChild;
			//WORD cn;
			//psf->lpszCurName = "*";
			//psf->wCurNameLen = 1;
			//psf->wFolderStartSector = psf->pCurNode->wStartSector;//pNode->wStartSector;
			//if( SearchFolder( psf, FALSE ) )
			//{
			//	return FALSE;
			//}
			//else
			if( BeginSearch( &sfChild, psf->lpVol, psf->hProc, NULL ) )
			{
				sfChild.lpszCurName = "*";
				sfChild.dwCurNameLen = 1;
				sfChild.dwFolderStartSector = psf->pCurNode->dwStartSector;
				op = SearchFolder( &sfChild, FALSE, USER_NODES );
				//op = SearchNode( &sfChild );
				EndSearch( &sfChild );
				if( op )
				{
					SetLastError( ERROR_DIR_NOT_EMPTY );
					return FALSE;
				}
			}

			return DoNodeDel( psf, TRUE );
		}
		break;
	case OP_ADD:		
		pNode->dwStartSector = NewFolder( psf->lpVol, psf->dwFolderStartSector, psf->pSector );
		if( pNode->dwStartSector != NULL_SECTOR )
			return DoNodeAdd( psf, pNode );
		else
			return FALSE;
	}
	return FALSE;
}

static BOOL SearchNode( 
					   SF * lpsf )
	
{
    //TCHAR szFolder[FILE_NAME_LEN+1];
    int len;
	LPTSTR lpszName;
	BOOL bRetv = FALSE;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: SearchNode From Folder=%d\r\n"), lpsf->lpVol->rfh.dwDataStartSector));

	EnterCriticalSection( &lpsf->lpVol->csNode );

	lpsf->dwFolderStartSector = lpsf->lpVol->rfh.dwDataStartSector;
	lpszName = lpsf->lpszName;
	//lpsf->nNodesPerSector = (WORD)(lpsf->lpVol->fdd.nBytesPerSector / sizeof( NODE ));
	
	lpsf->dwCurNameLen = 0;
	lpsf->lpszCurName = NULL;//Folder = NULL;//szFolder;
	//szFolder[FILE_NAME_LEN] = 0;
	
	while( 1 )
	{
		int iNextLen;
		LPTSTR lpszCurName;
		len = GetFolderAdr( &lpszCurName, lpszName, &iNextLen );
		if( len )
		{
			lpsf->nCurNodeIndex = 0;
			lpsf->lpszCurName = lpszCurName;
			
			lpsf->dwCurNameLen = len;
			lpszName += iNextLen;

			if( SearchFolder( lpsf, FALSE, ALL_NODES ) )
			{	 // find node			
				if( *lpszName )
				{   // the node must is folder
					if( lpsf->pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY ) // the node is folder
					{
						lpsf->dwFolderStartSector = lpsf->pCurNode->dwStartSector;
						continue;   // continue search in sub folder
					}
					else
					{   // not folder
						RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Node is'nt folder=%s\r\n"), lpsf->lpszCurName));
					}
				}
				else
				{
					RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Find Node=%s\r\n"), lpsf->pCurNode->strName));
					
					bRetv = TRUE;
					//return TRUE;
					break;
				}
			}
			else
			{
				RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: not find folder=%s\r\n"), lpsf->lpszCurName ));
			}
		}
		break;
	}
	
	LeaveCriticalSection( &lpsf->lpVol->csNode );

	if( bRetv )
		SetLastError(ERROR_FILE_EXISTS);
	else
	    SetLastError(ERROR_FILE_NOT_FOUND);
	return bRetv;//FALSE;
}

static BOOL BeginSearch( SF * lpsf, LPRAMFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName )
{
	//lpsf->pSector = malloc( lpVol->fdd.nBytesPerSector );
	lpsf->pSector = malloc( MAX_NODE_BYTES + lpVol->fdd.nBytesPerSector );
	if( lpsf->pSector )
	{
//		BOOL bRetv;
		lpsf->pCurNode = (PNODE)( ((LPBYTE)lpsf->pSector) + lpVol->fdd.nBytesPerSector );//MAX_NODE_BYTES;
		lpsf->lpVol = lpVol;
		lpsf->hProc = hProc;
		lpsf->lpszName = lpszName;
		memset( lpsf->pCurNode, 0, MAX_NODE_BYTES );
		//lpsf->nNodesPerSector = lpVol->fdd.nBytesPerSector / NODE_ALIGN_SIZE;
		return TRUE;
	}
	return FALSE;
}

static void EndSearch( SF * lpsf )
{
	if( lpsf->pSector )
		free( lpsf->pSector );
}

static DWORD ReadSectors( LPRAMFSDVOL lpVol, DWORD dwStartSector, 
						  DWORD nOffset, LPBYTE lpBuf, DWORD dwSize,
						  LPBYTE pSector,
						  UINT * lpCacheValid )
{
    //LPBYTE pSector;
    DWORD nSectorSize = lpVol->fdd.nBytesPerSector;
    DWORD s = dwSize;
	BOOL bCache = FALSE;

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: ReadSector, Sector=%d, nOffset=%d, Size=%d\r\n"), wStartSector, nOffset, dwSize));
	ASSERT_NOTIFY( dwStartSector != NULL_SECTOR, "no valid sector number\r\n" );


    if( dwStartSector != NULL_SECTOR )
    {
        if( dwSize >= nSectorSize && nOffset == 0 )
        {
            if( FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, lpBuf, nSectorSize ) != ERROR_SUCCESS )
				goto _error_ret;

            dwSize -= nSectorSize;
            lpBuf += nSectorSize;
        }
        else
        {
            if( FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) != ERROR_SUCCESS )
				goto _error_ret;
            if( nOffset + dwSize >= nSectorSize )
            {
                memcpy( lpBuf, pSector + nOffset, nSectorSize - nOffset );
                dwSize -=  nSectorSize - nOffset;
                lpBuf += nSectorSize - nOffset;
            }
            else
            {
                memcpy( lpBuf, pSector + nOffset, dwSize );
                dwSize = 0;
            }
			bCache = TRUE;
        }
        while( dwSize )
        {
            dwStartSector = NEXT_SECTOR( lpVol->lpdwFat, dwStartSector );
            if( dwStartSector != NULL_SECTOR )
            {
                if( dwSize >= nSectorSize )
                {
                    if( FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, lpBuf, nSectorSize ) != ERROR_SUCCESS )
						goto _error_ret;
                    dwSize -= nSectorSize;
                    lpBuf += nSectorSize;
					bCache = FALSE;
                }
                else
                {
                    if( FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) != ERROR_SUCCESS )
						goto _error_ret;
                    memcpy( lpBuf, pSector, dwSize );
                    dwSize = 0;
					bCache = TRUE;
                }
            }
            else
                break;
        }
		//if( bFree )
          //  free( pSector );
    }
	if( lpCacheValid )
		*lpCacheValid = bCache;
    return s - dwSize;
_error_ret:
	return 0;
}

static DWORD WriteSectors( LPRAMFSDVOL lpVol, 
						   DWORD dwStartSector, 
						   DWORD nOffset, 
						   const unsigned char * lpBuf, 
						   DWORD dwSize,
						   LPBYTE pSector // temp buf
 						   )
{
    //LPBYTE pSector;
    DWORD nSectorSize = lpVol->fdd.nBytesPerSector;
    DWORD s = dwSize;

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: WriteSector, Sector=%d, nOffset=%d, Size=%d\r\n"), wStartSector, nOffset, dwSize));

    if( dwStartSector )
    {        
        //pSector = (LPBYTE)malloc( nSectorSize );
        //if( pSector == NULL )
          //  return 0;
        if( dwSize >= nSectorSize && nOffset == 0 )
        {
            if( FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)lpBuf, nSectorSize ) != ERROR_SUCCESS )
				goto _error_ret;

            dwSize -= nSectorSize;
            lpBuf += nSectorSize;
        }
        else
        {
            if( FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)pSector, nSectorSize ) != ERROR_SUCCESS )
				goto _error_ret;
            if( nOffset + dwSize >= nSectorSize )
            {
                memcpy( pSector + nOffset, lpBuf, nSectorSize - nOffset );
                dwSize -=  nSectorSize - nOffset;
                lpBuf += nSectorSize - nOffset;
            }
            else
            {
                memcpy( pSector + nOffset, lpBuf, dwSize );
                dwSize = 0;
            }
            if( FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) != ERROR_SUCCESS )
				goto _error_ret;
        }
        while( dwSize )
        {
            dwStartSector = NEXT_SECTOR( lpVol->lpdwFat, dwStartSector );
            if( dwStartSector != NULL_SECTOR )
            {
                if( dwSize >= nSectorSize )
                {
                    if( FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)lpBuf, nSectorSize ) != ERROR_SUCCESS )
				        goto _error_ret;
                    dwSize -= nSectorSize;
                    lpBuf += nSectorSize;
                }
                else
                {
                    if( FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)pSector, nSectorSize ) != ERROR_SUCCESS )
				        goto _error_ret;
                    memcpy( pSector, lpBuf, dwSize );
                    dwSize = 0;
					
                    if( FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) != ERROR_SUCCESS )
				        goto _error_ret;
                }
            }
            else
                break;
        }
        //free( pSector );
    }

    return s - dwSize;
_error_ret:
	return 0;
}

static DWORD GetSerialNum(void)
{
    srand( GetTickCount() );
	return random();   
}

static BOOL DoVolumeFormat( LPRAMFSDVOL lpVol, void * pBuf )
{
	DWORD i, n;
	DWORD dwfat;
	void * pSector;
	PRFH prfh;
	DWORD hDsk;
	BOOL bRetv = FALSE;
	
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Do Format Start\r\n")));

	if( pBuf == 0 )
		pSector = malloc( lpVol->fdd.nBytesPerSector );
	else
		pSector = pBuf;

	EnterCriticalSection( &lpVol->csFat );

	if( pSector )
	{

		memset( pSector, 0, lpVol->fdd.nBytesPerSector );
		prfh = (PRFH)pSector;
		hDsk = lpVol->hDsk;
		memcpy( prfh->idName, idRam, strlen( idRam ) ); 	
		// now setup a ram alloc table
		dwfat = ( ( (lpVol->fdd.nSectors * sizeof( DWORD ) + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector ) * lpVol->fdd.nBytesPerSector );
		i = dwfat / lpVol->fdd.nBytesPerSector;
		prfh->nFreeCount = (lpVol->fdd.nSectors - i - 1);  // 1 head space, i is fat space
		prfh->ver = EFILE_VERSION;
		prfh->dwSerialNum = GetSerialNum();
		prfh->dwTotalSectors =lpVol->fdd.nSectors;
		prfh->dwDataStartSector = (i + 1);
		prfh->dwFatSectorNum = i;
		prfh->dwFatStartSector = 1;
		prfh->dwFreeSector = (i + 1);

	// build fat table
		i += 1;
		for( n = 0; n < i; n++ )
			*(lpVol->lpdwFat+n) = NULL_SECTOR;
	
		n = lpVol->fdd.nSectors-1;
		for( ; i < n; i++ )
			*(lpVol->lpdwFat+i) = (i + 1);
		*(lpVol->lpdwFat+i) = NULL_SECTOR;
		lpVol->dwMinNeedUpdate = 0;
		lpVol->dwMaxNeedUpdate = i;
	
	
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Build FAT....... \r\n")));
	
		memcpy( &lpVol->rfh, prfh, sizeof( RAM_FILE_HEAD ) );

		if( FSDMGR_WriteDisk(hDsk,0,1,(LPBYTE)prfh,lpVol->fdd.nBytesPerSector) != ERROR_SUCCESS )
		{
			goto _error_ret;
		}
	
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Init System Folder....... \r\n")));
		n = AllocSector( lpVol, 1 );
		DoFolderInit( lpVol, n, n, pSector );
	
		if( FSDMGR_WriteDisk( hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (LPBYTE)lpVol->lpdwFat, lpVol->fdd.nBytesPerSector * lpVol->rfh.dwFatSectorNum ) != ERROR_SUCCESS )
		{
			goto _error_ret;
		}

		lpVol->fUpdate = 1;

		bRetv = TRUE;

	}

_error_ret:

	LeaveCriticalSection( &lpVol->csFat );

	if( pBuf == 0 && pSector )
		free( pSector );
	return bRetv;
}

static BOOL DoShellNotify( 
						  LPRAMFSDVOL lpVol, 
						  DWORD dwEvent, 
						  DWORD dwFlags, 
						  LPCTSTR lpcszItem1, 
						  LPCTSTR lpcszItem2,
						  PNODE pNode )
{
	FILECHANGEINFO fci;
	SYSTEMTIME st;
	BOOL bRetv = FALSE;
	TCHAR * path1 = lpVol->szNotifyPath0;//LN 2003-05-21;
	TCHAR * path2 = lpVol->szNotifyPath1;// LN 2003-05-21;

//    path1 = malloc( MAX_PATH * 2 * sizeof( TCHAR ) );// LN 2003-05-21;
//	if( path1 )// LN 2003-05-21;
//		path2 = path1 + MAX_PATH;// LN 2003-05-21;
//	else// LN 2003-05-21;
//		return FALSE; // LN 2003-05-21;
	
	if( lpVol->pscf )
	{
		if( lpcszItem1 )
		{
			strcpy( path1, lpVol->szVolName );
			strcat( path1, lpcszItem1 );
		}
		else
			path1[0] = 0;

		if( lpcszItem2 )
		{
			strcpy( path2, lpVol->szVolName );
			strcat( path2, lpcszItem2 );
		}
		else
			path2[0] = 0;

		memset( &fci, 0, sizeof( FILECHANGEINFO ) );
		fci.cbSize = sizeof( FILECHANGEINFO );
		fci.wEventId = dwEvent;
		fci.uFlags = dwFlags;
		fci.dwItem1 = (DWORD)path1;//(DWORD)lpcszItem1;
		fci.dwItem2 = (DWORD)path2;//(DWORD)lpcszItem2;
		fci.dwAttributes = pNode->wAttrib;
		fci.nFileSize = pNode->nFileLength;
		
		GetSystemTime( &st );  //Ok
		SystemTimeToFileTime( &st, &fci.ftModified );	
		bRetv = lpVol->pscf(&fci);
		//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Notify Shell=%d\r\n"), bRetv));
		if( lpcszItem1 )
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Shell Notify Item1=%s\r\n"), path1));
		if( lpcszItem2 )
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Shell Notify Item2=%s\r\n"), path2));
	}
//	free( path1 );// LN 2003-05-21;
	return bRetv;	
}

static BOOL DoUpdateVolume( LPRAMFSDVOL lpVol )
{
	DWORD dwRetv = !ERROR_SUCCESS;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Update Volume=%d\r\n"), lpVol->fUpdate));

    EnterCriticalSection( &lpVol->csFat );

	if( lpVol->fUpdate )
	{
		void * pSector = malloc( lpVol->fdd.nBytesPerSector );
		DWORD bytes = lpVol->fdd.nBytesPerSector;
		DWORD dwStart, dwEnd;

		if( pSector )
		{

			memset( pSector, 0, bytes );

			memcpy( pSector, &lpVol->rfh, sizeof( RAM_FILE_HEAD ) );
			if( FSDMGR_WriteDisk( lpVol->hDsk, 0, 1, pSector, bytes ) == ERROR_SUCCESS )
			{	// update fat		
			    dwStart = (lpVol->dwMinNeedUpdate *  FATBYTES) / lpVol->fdd.nBytesPerSector;
			    dwEnd = (lpVol->dwMaxNeedUpdate *  FATBYTES) / lpVol->fdd.nBytesPerSector;
				dwRetv = FSDMGR_WriteDisk( lpVol->hDsk, 
					                       lpVol->rfh.dwFatStartSector + dwStart, 
										   dwEnd - dwStart + 1, 
										   (LPBYTE)lpVol->lpdwFat + dwStart * lpVol->fdd.nBytesPerSector,
										   (dwEnd - dwStart + 1) * lpVol->fdd.nBytesPerSector );
				//RETAILMSG( 1, ( "update volume sectors:%d,%d.\r\n", dwStart,  dwEnd ) );
				if( dwRetv == ERROR_SUCCESS )
				{
				    lpVol->dwMaxNeedUpdate = 0;
				    lpVol->dwMinNeedUpdate = -1;
				}

			    //bytes *=  lpVol->rfh.dwFatSectorNum;
			    //dwRetv = FSDMGR_WriteDisk( lpVol->hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (BYTE*)lpVol->lpdwFat, bytes );
			}
			free( pSector );
			lpVol->fUpdate = 0;
		}
		else 
		{
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Update Volume Failure\r\n")));
			//bRetv = FALSE;
		}
	}
	else
		dwRetv = ERROR_SUCCESS;

	LeaveCriticalSection( &lpVol->csFat );

	return (dwRetv == ERROR_SUCCESS);
}
/*
static LPRAMFSDFILE FindOpened( LPRAMFSDVOL lpVol, LPCTSTR lpcszFileName, int n )
{	

	LPRAMFSDFILE lpFile;

	EnterCriticalSection( &lpVol->csFileList );
	lpFile = lpVol->lpOpenFile;

	while( lpFile )
	{
		if( n == 0 )
		{
			if( stricmp( lpFile->lpszFileName, lpcszFileName ) == 0 )
				break;
		}
		else
		{
			if( strncmp( lpFile->lpszFileName, lpcszFileName, n ) == 0 )
				break;
		}
		lpFile = lpFile->lpNext;
	}
	LeaveCriticalSection( &lpVol->csFileList );
	return lpFile;
}
*/

//static LPRAMFSDFILE FindOpened( LPRAMFSDVOL lpVol, LPCTSTR lpcszFileName, int n )
//static LPRAMFSDFILE FindOpened( LPRAMFSDVOL lpVol, LPCTSTR lpcszFileName, int n ) // LN: 2003-05-21
static LPRAMFSDFILE FindOpened( LPRAMFSDVOL lpVol, LPCTSTR lpcszFileName ) // LN: 2003-05-21
{	

	LPRAMFSDFILE lpFile;

	EnterCriticalSection( &lpVol->csFileList );
	lpFile = lpVol->lpOpenFile;

	while( lpFile )
	{
//		if( n == 0 )// LN: 2003-05-21
//		{// LN: 2003-05-21
		if( stricmp( lpFile->lpfn->lpszFileName, lpcszFileName ) == 0 )
			break;
//		}// LN: 2003-05-21
//		else// LN: 2003-05-21
//		{// LN: 2003-05-21
//			if( strncmp( lpFile->lpfn->lpszFileName, lpcszFileName, n ) == 0 )// LN: 2003-05-21
//				break;// LN: 2003-05-21
//		}// LN: 2003-05-21
		lpFile = lpFile->lpNext;
	}
	LeaveCriticalSection( &lpVol->csFileList );
	return lpFile;
}


//static LPRAMFSDFIND FindOpenedSearchHandle( LPRAMFSDVOL lpVol, LPCTSTR lpcszFileName, int n )// LN: 2003-05-21
static LPRAMFSDFIND FindOpenedSearchHandle( LPRAMFSDVOL lpVol, LPCTSTR lpcszFileName )// LN: 2003-05-21
{
	LPRAMFSDFIND lpFile;

	EnterCriticalSection( &lpVol->csFindList );

	lpFile = lpVol->lpFindFile;

	while( lpFile )
	{
//		if( n == 0 )// LN: 2003-05-21
		//{// LN: 2003-05-21
		if( stricmp( lpFile->lpszName, lpcszFileName ) == 0 )
			break;
//		}// LN: 2003-05-21
//		else// LN: 2003-05-21
//		{// LN: 2003-05-21
//			if( strncmp( lpFile->lpszName, lpcszFileName, n ) == 0 )// LN: 2003-05-21
//				break;// LN: 2003-05-21
//		}// LN: 2003-05-21
		lpFile = lpFile->lpNext;
	}

	LeaveCriticalSection( &lpVol->csFindList );

	return lpFile;
}


//static LPRAMFSDFILE _AllocFileHandle( LPRAMFSDVOL lpVol, UINT uiFileNameLen )
//{
  //  (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + ( uiFileNameLen + 1) * sizeof( TCHAR ) );
//}

//static void _FreeFileHandle( void )
//{
//}

static HANDLE CreateShareFile(
							  LPRAMFSDFILE lpOldFile,
							  PVOL pVol, 
							  HANDLE hProc, 
							  LPCTSTR lpcszFileName, 
							  DWORD dwAccess, 
							  DWORD dwShareMode,
							  PSECURITY_ATTRIBUTES pSecurityAttributes, 
							  DWORD dwCreate,
							  DWORD dwFlagsAndAttributes, 
							  HANDLE hTemplateFile ) 
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
	LPRAMFSDFILE lpFile;
    //HANDLE hFile = INVALID_HANDLE_VALUE;
	int op;

	op = 0;
	if( lpOldFile->dwShareMode && lpOldFile->dwShareMode == dwShareMode )
	{
		if( lpOldFile->dwShareMode == FILE_SHARE_READ )
		{
			if( dwAccess == GENERIC_READ )
				op = 1;
		}
		else if( lpOldFile->dwShareMode == FILE_SHARE_WRITE )
		{
			if( dwAccess == GENERIC_WRITE )
				op = 1;
		}
		else if( lpOldFile->dwShareMode == (FILE_SHARE_WRITE|FILE_SHARE_READ) )
			op = 1;
		if( op )
		{			
			//lpFile = (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + lpVol->fdd.nBytesPerSector + (strlen( lpcszFileName ) + 1) * sizeof( TCHAR ) );
			lpFile = (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + lpVol->fdd.nBytesPerSector );
			if( lpFile )
			{
				// init lpFile member
				memset( lpFile, 0, sizeof( RAMFSDFILE ) );
				lpFile->lpVol = lpVol;
				lpFile->hProc = hProc;
				lpFile->lpbCacheBuf = (LPBYTE)(lpFile+1);

				//lpFile->lpszFileName = (LPBYTE)( lpFile + 1) + lpVol->fdd.nBytesPerSector;
				//strcpy( lpFile->lpszFileName, lpcszFileName );

				lpFile->dwAccess = dwAccess;
				lpFile->dwShareMode = dwShareMode;
				lpFile->dwCreate =dwCreate;
				lpFile->dwFlagsAndAttributes = dwFlagsAndAttributes;

				//lpFile->dwFolderSector = lpOldFile->dwFolderSector;
				//lpFile->dwNodeSector = lpOldFile->dwNodeSector;

				//memcpy( GET_NODE_PTR(lpFile), GET_NODE_PTR(lpOldFile), sizeof( NODE ) );
//				lpFile->index = lpOldFile->index;
				lpFile->flag = lpOldFile->flag;
				lpFile->dwFilePointer = 0;
				
				InterlockedIncrement( &lpOldFile->lpfn->dwRefCount );
				lpFile->lpfn = lpOldFile->lpfn;
				
				// add to volume list
				AddFileHandleToVolume( lpFile );
				return lpFile;
			}
		}
	}
	SetLastError(ERROR_SHARING_VIOLATION);
	return INVALID_HANDLE_VALUE;
}

static HANDLE CreateVolumeFile(
							  LPRAMFSDVOL lpVol, 
							  HANDLE hProc,
							  LPCTSTR lpcszFileName ) 
{
	LPRAMFSDFILE lpFile;
//    HANDLE hFile = INVALID_HANDLE_VALUE;
//	WORD nvl;

	if( (lpFile = FindOpened( lpVol, szVolMark ) ) != 0 )// LN: 2003-05-21
	{
		SetLastError(ERROR_SHARING_VIOLATION);
		//return hFile;
		return INVALID_HANDLE_VALUE;
	}
	
	//lpFile = (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + lpVol->fdd.nBytesPerSector + (strlen( lpcszFileName ) +  1)*sizeof( TCHAR ) );
	lpFile = (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + lpVol->fdd.nBytesPerSector );

	if( lpFile )
	{
		// init lpFile member
		memset( lpFile, 0, sizeof( RAMFSDFILE ) );
		lpFile->lpfn = CreateFileNode( lpcszFileName );

		lpFile->lpbCacheBuf = (LPBYTE)(lpFile+1);
		//lpFile->dwCachePosStart = lpFile->dwCachePosEnd = -1;

		lpFile->lpVol = lpVol;
		lpFile->hProc = hProc;

		//lpFile->lpszFileName = (LPBYTE)(lpFile + 1) + lpVol->fdd.nBytesPerSector;//(TCHAR*)szVolMark;
		//strcpy( lpFile->lpszFileName, lpcszFileName );

		lpFile->dwAccess = 0;
		lpFile->dwShareMode = 0;
		lpFile->dwCreate = 0;//GENRIC_READ|GENRIC_WRITE;
		lpFile->dwFlagsAndAttributes = 0;

//		lpFile->dwFolderSector = NULL_SECTOR;
//		lpFile->dwNodeSector = NULL_SECTOR;

//		lpFile->index = 0;
		lpFile->flag = VOL_FILE;
		lpFile->dwFilePointer = 0;
		
		//hFile = FSDMGR_CreateFileHandle( lpVol->hVol, hProc, (DWORD)lpFile );
		//if( hFile )
		{						
			// add to volume list
			//lpFile->lpNext = lpVol->lpOpenFile;
			//lpVol->lpOpenFile = lpFile;
			InterlockedIncrement( &lpFile->lpfn->dwRefCount );
			AddFileHandleToVolume( lpFile );
			return lpFile;
		}
		//free( lpFile );
	}
	return INVALID_HANDLE_VALUE;
}


static int IsValidFilePathName( LPCTSTR lpcFileName )
{
	//static const TCHAR strInvalid[] = TEXT( ":?*\"<>|" );
	int n = 0;
	int nFileName = 0;
	BOOL bSeparate =FALSE;
	//LPCTSTR lpcStart = lpcFileName;
	if( lpcFileName  )
	{
		while( *lpcFileName )
		{
			TCHAR c = *lpcFileName;
			//TCHAR * p = strInvalid;
			if( c == ':' ||
				c == '?' ||
				c == '*' ||
				c == '\"' ||
				c == '<' ||
				c == '>' ||
				c == '|' ||
				n >= MAX_PATH )
			{
				SetLastError(ERROR_INVALID_NAME);
				return FALSE;
			}
			if( c == '\\' || c == '/' )
			{
				if( bSeparate )
				{
				    SetLastError(ERROR_INVALID_NAME);
				    return FALSE;
				}
				bSeparate = TRUE;
				nFileName = 0;
			}
			else
			{
				bSeparate = FALSE;
				nFileName++;
			}
			lpcFileName++;
			n++;
		}
/*
		// is valid file name ? 
		lpcFileName--;
		n--;
		for( ; n >=0 ; n-- )
		{
			TCHAR c = *lpcFileName;
			//TCHAR * p = strInvalid;
			if( c == ':' ||
				c == '?' ||
				c == '*' ||
				c == '\"' ||
				c == '<' ||
				c == '>' ||
				c == '|' )
			{
				SetLastError(ERROR_INVALID_NAME);
				return FALSE;
			}
			lpcFileName++;
			n++;
		}
*/
	}
	return nFileName;
}


#define IS_DISABLE_DEL( wAttrib ) ( (wAttrib) & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM) )

#define FILE_CREATE_ZONE 0

HANDLE KFSD_CreateFile( 
						 PVOL pVol, 
						 HANDLE hProc, 
						 LPCTSTR lpcszFileName, 
						 DWORD dwAccess, 
						 DWORD dwShareMode,
						 PSECURITY_ATTRIBUTES pSecurityAttributes, 
						 DWORD dwCreate,
						 DWORD dwFlagsAndAttributes, 
						 HANDLE hTemplateFile ) 
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;   // *pVol = hvol
    LPRAMFSDFILE lpFile, lpOldFile;

    //HANDLE hFile = INVALID_HANDLE_VALUE;
    //LPCTSTR lpcf;
    //int nf;
    //int op, opChange;
	//WORD nvl;

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: CreateFile: %s, PROC=0x%x, dwCreate=0x%x, dwFlagsAndAttributes=0x%x\r\n"), lpcszFileName, hProc, dwCreate, dwFlagsAndAttributes));
	//RETAILMSG(1, (TEXT("KFSD: CreateFile: %s, PROC=0x%x, dwCreate=0x%x, dwFlagsAndAttributes=0x%x\r\n"), lpcszFileName, hProc, dwCreate, dwFlagsAndAttributes));
	//EdbgOutputDebugString( "RAMFSD: CreateFile: %s, PROC=0x%x, dwCreate=0x%x, dwFlagsAndAttributes=0x%x\r\n", lpcszFileName, hProc, dwCreate, dwFlagsAndAttributes );
    ////RETAILMSG( 1, ( "CreateFile:1.\r\n" ) );
	//RETAILMSG( 1, ( "CreateFile:0=%s.\r\n", lpcszFileName ) );

    // check param is valid ?
/* //LN, 2003.05.20, remove to fsmain.c
    if( dwShareMode & ( ~( FILE_SHARE_READ | FILE_SHARE_WRITE ) ) )
        goto RET_INVALIDPARAM;
    if( dwAccess & ( ~( GENERIC_READ | GENERIC_WRITE ) ) )
        goto RET_INVALIDPARAM;
	if( !( dwCreate == CREATE_NEW ||
		  dwCreate == CREATE_ALWAYS ||
		  dwCreate == OPEN_ALWAYS ||
		  dwCreate == OPEN_EXISTING ||
		  dwCreate == TRUNCATE_EXISTING ) )
		goto RET_INVALIDPARAM;
	if( dwCreate == TRUNCATE_EXISTING && 
		!( dwAccess & GENERIC_WRITE ) )
	{
		SetLastError( ERROR_ACCESS_DENIED );
		return INVALID_HANDLE_VALUE;
	}

    if( dwFlagsAndAttributes & FILE_ATTRIBUTE_DIRECTORY )
		goto RET_INVALIDPARAM;

	if( (BYTE)dwFlagsAndAttributes == FILE_ATTRIBUTE_NORMAL )
        dwFlagsAndAttributes &= ~0xFFFF;
    else
        dwFlagsAndAttributes &= ~FILE_ATTRIBUTE_NORMAL;
    dwFlagsAndAttributes |= FILE_ATTRIBUTE_ARCHIVE;
	//
*/

	RETAILMSG( FILE_CREATE_ZONE, ( "c0=CreateFile(%s).\r\n", lpcszFileName ) );	

	// LN,2003-09-27, check volume is write enable ?
	if( (lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) &&
		(dwAccess & GENERIC_WRITE) )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_CreateFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return INVALID_HANDLE_VALUE;
	}
	//

	lpcszFileName = IgnoreSpace( lpcszFileName );

	if( stricmp( lpcszFileName, TEXT( "\\Vol:" ) ) == 0 )
	{   // special file name, used to DeviceIoControl
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: CreateVolumeFile\r\n")));
		return CreateVolumeFile( lpVol, hProc, lpcszFileName );
	}
	RETAILMSG( FILE_CREATE_ZONE, ( "c1.\r\n" ) );

	if( IsValidFilePathName( lpcszFileName ) == 0 )
	{		
		return INVALID_HANDLE_VALUE;
	}
	RETAILMSG( FILE_CREATE_ZONE, ( "c2.\r\n" ) );
	
	if( (lpOldFile = FindOpened( lpVol, lpcszFileName )) != 0 )// LN: 2003-05-21
	{
		RETAILMSG(FILE_CREATE_ZONE, (TEXT("c3: CreateShareFile\r\n")));
		return CreateShareFile( lpOldFile, pVol, hProc, lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
	}  
	RETAILMSG( FILE_CREATE_ZONE, ( "c4.\r\n" ) );

//	lpFile = (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + lpVol->fdd.nBytesPerSector + (strlen( lpcszFileName ) + 1) * sizeof( TCHAR ) );
	lpFile = (LPRAMFSDFILE)malloc( sizeof( RAMFSDFILE ) + lpVol->fdd.nBytesPerSector  );

	if( lpFile )
    {
	    SF sf;			
	    int bRetv;

		RETAILMSG( FILE_CREATE_ZONE, ( "c5.\r\n" ) );
		memset( lpFile, 0, sizeof( RAMFSDFILE ) );
		lpFile->lpfn = CreateFileNode(lpcszFileName);

		lpFile->lpbCacheBuf = (LPBYTE)(lpFile + 1);
	    EnterCriticalSection( &lpVol->csNode );

			RETAILMSG( FILE_CREATE_ZONE, ( "c6.\r\n" ) );

			if( lpFile->lpfn && BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpcszFileName ) )
			{
				BOOL bUpdate = FALSE;
				bRetv = SearchNode( &sf );
				RETAILMSG( FILE_CREATE_ZONE, ( "c7.\r\n" ) );
				if( *(sf.lpszCurName + sf.dwCurNameLen) == 0 )
				{
					RETAILMSG( FILE_CREATE_ZONE, ( "c8.\r\n" ) );
					if( bRetv == FALSE )
					{
						RETAILMSG( FILE_CREATE_ZONE, ( "c9.\r\n" ) );
						RETAILMSG(FILE_CREATE_ZONE, (TEXT("RAMFSD: CreateFile: Not Found\r\n")));
						if( dwCreate == CREATE_NEW ||
							dwCreate == CREATE_ALWAYS ||
							dwCreate == OPEN_ALWAYS )
						{  // not found , add a new
							SYSTEMTIME st;
							
							RETAILMSG(FILE_CREATE_ZONE, (TEXT("RAMFSD: CreateFile: New file name=%s\r\n"), sf.lpszCurName));
							memcpy( GET_NODE_PTR(lpFile)->strName, sf.lpszCurName, sf.dwCurNameLen );
							//GetFolderAdr( lpFile->node.strName, sf.lpszCurName );
							
							GetSystemTime( &st );
							SystemTimeToFileTime( &st, &GET_NODE_PTR(lpFile)->ft );
							
							GET_NODE_PTR(lpFile)->nFileLength = 0;
							GET_NODE_PTR(lpFile)->dwStartSector = NULL_SECTOR;
							GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
							GET_NODE_PTR(lpFile)->bNameLen = (BYTE)sf.dwCurNameLen;
							bUpdate = bRetv = DoNodeAdd( &sf, GET_NODE_PTR(lpFile) );
							//lpFile->dwNodeSector = sf.dwCurNodeSector;//Index;//.wEmptyNodeSector;
							lpFile->lpfn->dwNodeSector = sf.dwCurNodeSector;
							//lpFile->index = sf.nCurNodeIndex;//.nEmptyNodeIndex;							
							lpFile->lpfn->index = sf.nCurNodeIndex;//.nEmptyNodeIndex;							
							//bRetv = DoNodeChange( &sf, &lpFile->node, OP_ADD );
						}
					}
					else
					{	// found it, check valid
						RETAILMSG( FILE_CREATE_ZONE, ( "CreateFile:10.\r\n" ) );
						bRetv = FALSE;
						if( dwCreate == CREATE_ALWAYS ||
							dwCreate == OPEN_ALWAYS ||
							dwCreate == OPEN_EXISTING ||
							dwCreate == TRUNCATE_EXISTING )
						{
							//bRetv = DoNodeChange( &sf, &lpFile->node, OP_GET );
							RETAILMSG( FILE_CREATE_ZONE, ( "c11.\r\n" ) );
							bRetv = DoNodeGet( &sf, GET_NODE_PTR(lpFile) );
							if( bRetv )
							{
								RETAILMSG( FILE_CREATE_ZONE, ( "c12.\r\n" ) );
								if( ( GET_NODE_PTR(lpFile)->wAttrib & FILE_ATTRIBUTE_DIRECTORY ) ||
									( ( GET_NODE_PTR(lpFile)->wAttrib & ( FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM ) ) && 
									  (dwAccess & GENERIC_WRITE) ) )
								{
									RETAILMSG( FILE_CREATE_ZONE, ( "c13.\r\n" ) );
									bRetv = FALSE;  // not right to access
									SetLastError( ERROR_ACCESS_DENIED );
									RETAILMSG(FILE_CREATE_ZONE, (TEXT("RAMFSD: CreateFile: ACCESS_DENIED\r\n")));
								}
								else
								{								
									//RETAILMSG( 1, ( "CreateFile:14.\r\n" ) );
									//lpFile->dwNodeSector = sf.dwCurNodeSector;
									//lpFile->index = sf.nCurNodeIndex;
									lpFile->lpfn->dwNodeSector = sf.dwCurNodeSector;
									lpFile->lpfn->index = sf.nCurNodeIndex;

									//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: CreateFile: Open it\r\n")));
									RETAILMSG(FILE_CREATE_ZONE, (TEXT("RAMFSD: CreateFile: Open it\r\n")));
								}
							}
							RETAILMSG( FILE_CREATE_ZONE, ( "c15.\r\n" ) );
						}
					}				
				}
				else
					bRetv = FALSE;//error found

				if( bRetv )
				{
					RETAILMSG( FILE_CREATE_ZONE, ( "c16.\r\n" ) );
					//strcpy( lpFile->lpszFileName, lpcszFileName );

					//lpFile->dwFolderSector = sf.dwFolderStartSector;
					lpFile->lpfn->dwFolderSector = sf.dwFolderStartSector;

					lpFile->dwAccess = dwAccess;
					lpFile->dwCreate = dwCreate;
					lpFile->dwFlagsAndAttributes = dwFlagsAndAttributes;
					lpFile->dwShareMode = dwShareMode;
					lpFile->hProc = hProc;
					lpFile->lpVol = lpVol;
					lpFile->dwFilePointer = 0;
					lpFile->flag = NORMAL_FILE;
					
					//hFile = FSDMGR_CreateFileHandle( lpVol->hVol, hProc, (DWORD)lpFile );

					//if( hFile )
					{
						if( bUpdate )
						{
							LeaveCriticalSection( &lpVol->csNode );
							DoShellNotify( lpVol, SHCNE_CREATE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpFile->lpfn->lpszFileName, NULL, GET_NODE_PTR(lpFile) );
							EnterCriticalSection( &lpVol->csNode );
						}
						
						// add to volume list
						//lpFile->lpNext = lpVol->lpOpenFile;
						//lpVol->lpOpenFile = lpFile;
						InterlockedIncrement( &lpFile->lpfn->dwRefCount );
						AddFileHandleToVolume( lpFile );

						RETAILMSG( FILE_CREATE_ZONE, ( "c17.\r\n" ) );;

						// LN, 2003-07-30, CREATE_ALWAYS, clear attrib only
						// TRUNCATE_EXISTING set cotent to zero
						/* 2003-07-30

						if( dwCreate == CREATE_ALWAYS ||
							dwCreate == TRUNCATE_EXISTING )
						{   // set content to zero
							if( ReallocFile( lpFile, 0 ) )
							{
								GET_NODE_PTR(lpFile)->nFileLength = 0;
								GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
							}
						}
						*/
						if( dwCreate == CREATE_ALWAYS )
						{   // clear attrib
							GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
							// 2003-09-11, 增加（对MS的CREATE_ALWAYS进行测试，发现长度被设为0）
							if( ReallocFile( lpFile, 0 ) )
							{
								GET_NODE_PTR(lpFile)->nFileLength = 0;
							}
							// 2003-09-11
						}									
						else if( dwCreate == TRUNCATE_EXISTING )
						{//  set cotent to zero
							if( ReallocFile( lpFile, 0 ) )
							{
								GET_NODE_PTR(lpFile)->nFileLength = 0;
							}
						}
						//

						EndSearch( &sf );
						//return hFile;	
						LeaveCriticalSection( &lpVol->csNode );
						RETAILMSG( FILE_CREATE_ZONE, ( "c18.\r\n" ) );;
						//RETAILMSG( 1, ( "CreateFile:18.fn=%s, lpFile=%x.\r\n", lpcszFileName, lpFile ) );
						return lpFile;
					}
				}
				RETAILMSG( FILE_CREATE_ZONE, ( "c19.\r\n" ) );;
			    EndSearch( &sf );
				LeaveCriticalSection( &lpVol->csNode );
			}
		//}
		if( lpFile->lpfn )
		    DeleteFileNode( lpFile->lpfn );
		free( lpFile );
	}

    return INVALID_HANDLE_VALUE;
//RET_INVALIDPARAM:
//	SetLastError( ERROR_INVALID_PARAMETER );
//	return INVALID_HANDLE_VALUE;
}

#undef FILE_CREATE_ZONE


static BOOL AddFindHandleToVolume( LPRAMFSDFIND lpFind )
{
	EnterCriticalSection( &lpFind->lpVol->csFindList );
	lpFind->lpNext = lpFind->lpVol->lpFindFile;
	lpFind->lpVol->lpFindFile = lpFind;
	LeaveCriticalSection( &lpFind->lpVol->csFindList );
	return TRUE;
}

static BOOL RemoveFindHandleFromVolume( LPRAMFSDFIND lpFile )
{
	LPRAMFSDVOL lpVol; 
	LPRAMFSDFIND lpf;
	BOOL bRetv = FALSE;

    
	EnterCriticalSection( &lpFile->lpVol->csFindList );
	lpVol = lpFile->lpVol;	
	lpf = lpVol->lpFindFile;

	if(  lpf == lpFile )
	{
		lpVol->lpFindFile = lpFile->lpNext;
		lpFile->lpNext = NULL;
		bRetv = TRUE;
	}
	else
	{
		while ( lpf )
		{
			if( lpf->lpNext == lpFile )
			{
				lpf->lpNext = lpFile->lpNext;
				lpFile->lpNext = NULL;
				bRetv = TRUE;
				break;
			}
			lpf = lpf->lpNext;
		}
	}
    
	LeaveCriticalSection( &lpVol->csFindList );
	RETAILMSG( bRetv == FALSE && FILE_ZONE, (TEXT("RAMFSD: Error RemoveFindHandleFromVolume\r\n")));
	return bRetv;	
}

static BOOL AddFileHandleToVolume( LPRAMFSDFILE lpFile )
{
	//LPRAMFSDVOL lpVol = lpFile->lpVol; 

	EnterCriticalSection( &lpFile->lpVol->csFileList );

	lpFile->lpNext = lpFile->lpVol->lpOpenFile;
	lpFile->lpVol->lpOpenFile = lpFile;

	LeaveCriticalSection( &lpFile->lpVol->csFileList );
	return TRUE;
}

static BOOL RemoveFileHandleFromVolume( LPRAMFSDFILE lpFile )
{
	BOOL bRetv = FALSE;

	EnterCriticalSection( &lpFile->lpVol->csFileList );

	{

		LPRAMFSDVOL lpVol = lpFile->lpVol; 
		LPRAMFSDFILE lpf = lpVol->lpOpenFile;
		
		if(  lpf == lpFile )
		{
			lpVol->lpOpenFile = lpFile->lpNext;
			lpFile->lpNext = NULL;
			//return TRUE;
			bRetv = TRUE;
		}
		else
		{
			while ( lpf )
			{
				if( lpf->lpNext == lpFile )
				{
					lpf->lpNext = lpFile->lpNext;
					lpFile->lpNext = NULL;
					bRetv = TRUE;
					break;
				}
				lpf = lpf->lpNext;
			}
		}
		//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Error RemoveFileHandleFromVolume\r\n")));
		//return FALSE;
	}

	LeaveCriticalSection( &lpFile->lpVol->csFileList );
	
	RETAILMSG( bRetv == FALSE && FILE_ZONE, (TEXT("RAMFSD: Error RemoveFileHandleFromVolume\r\n")));

	return bRetv;
	
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL KFSD_CloseFile( PFILE pf )
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here
	BOOL bRetv = TRUE;
	
	//RETAILMSG(1, (TEXT("RAMFSD: CloseFile=%x.\r\n"), lpFile));
	//RETAILMSG(FILE_ZONE|1, (TEXT("RAMFSD: CloseFile=%s\r\n"), lpFile->lpszFileName));

	if( (lpFile->dwAccess & GENERIC_WRITE) && lpFile->flag == NORMAL_FILE )
	{
	 	LPBYTE pSector = (LPBYTE)malloc( lpFile->lpVol->fdd.nBytesPerSector );

		//RETAILMSG(1, (TEXT("RAMFSD: CloseFile0.\r\n") ));
				
		if( pSector )
		{
			PNODE pNode;
			DWORD dwOffset;
//			SYSTEMTIME st;
			//GetSystemTime( &st );
			//SystemTimeToFileTime( &st, &GET_NODE_PTR(lpFile)->ft );
			// entercriticalsector

			EnterCriticalSection( &lpFile->lpfn->csFileNode );

			pNode = GET_NODE_PTR( lpFile );
			dwOffset = GET_NODE_OFFSET( lpFile->lpfn->index, lpFile->lpVol );
			//RETAILMSG(1, (TEXT("RAMFSD: CloseFile2.\r\n") ));
			if( WriteSectors( lpFile->lpVol, lpFile->lpfn->dwNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), pSector ) )
			{
				LeaveCriticalSection( &lpFile->lpfn->csFileNode );
				//RETAILMSG(1, (TEXT("RAMFSD: CloseFile3.\r\n") ));
				DoUpdateVolume( lpFile->lpVol );
				//RETAILMSG(1, (TEXT("RAMFSD: CloseFile4.\r\n") ));
				DoShellNotify( lpFile->lpVol, SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpFile->lpfn->lpszFileName, NULL, pNode );
				//RETAILMSG(1, (TEXT("RAMFSD: CloseFile5.\r\n") ));
			}
			else
			{
				LeaveCriticalSection( &lpFile->lpfn->csFileNode );
				bRetv = FALSE;
			}
            
			// leaveercriticalsector
			//RETAILMSG(1, (TEXT("RAMFSD: CloseFile6.\r\n") ));
			free( pSector );
			//RETAILMSG(1, (TEXT("RAMFSD: CloseFile7.\r\n") ));
		}
		else
			bRetv = FALSE;
    }
	//RETAILMSG(1, (TEXT("RAMFSD: CloseFile8.\r\n") ));
	// 2003-09-13, 假如失败，则lpFile永远无法移出从list
	//if( bRetv )
	if( lpFile )
	//2003-09-13
	{
		RemoveFileHandleFromVolume( lpFile );
		if( InterlockedDecrement( &lpFile->lpfn->dwRefCount ) == 0 )
			DeleteFileNode( lpFile->lpfn );
		free( lpFile );
	}
	if( bRetv == FALSE )
	//else
	{
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Error CloseFile\r\n")));
	}
	//RETAILMSG(1, (TEXT("RAMFSD: CloseFile9.\r\n") ));
    return bRetv;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD KFSD_GetFileAttributes(
                            PVOL pVol,
                            LPCTSTR lpszFileName )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //do somthing here
	SF sf;
	DWORD bRetv=0xffffffffl;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileAttributes=%s\r\n"), lpszFileName));

	lpszFileName = IgnoreSpace( lpszFileName );

	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
	{
		bRetv = SearchNode( &sf );
        if( bRetv )
		{
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileAttributes, Attrib=0x%x\r\n"), sf.pCurNode->wAttrib));
			bRetv = sf.pCurNode->wAttrib;
		}
		else
		{
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileAttributes, Not Found\r\n")));
			bRetv = 0xffffffffl;
		}
		EndSearch( &sf );
	}
	//if( bRetv == 0xffffffffl )
		//SetLastError(ERROR_FILE_NOT_FOUND);
	return bRetv;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL KFSD_ReadFile(
                  PFILE pf,
                  LPVOID lpBuffer,
                  DWORD dwNumToRead,
                  LPDWORD lpdwNumRead, 
				  LPOVERLAPPED pOverlapped )
{    
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //do somthing here
    DWORD sr = 0;
	DWORD dwFilePointer;
    DWORD dwStartSector;
    DWORD dwOffset;
	UINT nBytesPerSector;


    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: ++ReadFile, num=%d, pointer=%d\r\n"), dwNumToRead,lpFile->dwFilePointer ));

	if( lpdwNumRead )
       *lpdwNumRead = 0;
    if( (lpFile->dwAccess & GENERIC_READ) && lpFile->flag == NORMAL_FILE )
	{
		nBytesPerSector = lpFile->lpVol->fdd.nBytesPerSector;

		EnterCriticalSection( &lpFile->lpfn->csFileNode );
		dwFilePointer = lpFile->dwFilePointer;

		if( dwFilePointer >= GET_NODE_PTR(lpFile)->nFileLength )
		{
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
			return TRUE;
		}

		if( dwFilePointer + dwNumToRead > GET_NODE_PTR(lpFile)->nFileLength )
			dwNumToRead = GET_NODE_PTR(lpFile)->nFileLength - dwFilePointer;
		if( dwNumToRead )
		{
			void * lpSector;

			if( lpFile->uiCacheValid )
			{
				DWORD dwCachePosEnd = lpFile->dwCachePosStart + nBytesPerSector;
				if( dwFilePointer >= lpFile->dwCachePosStart &&
					dwFilePointer < dwCachePosEnd )
				{   // hint the cache
					DWORD dwMaxNum = MIN( dwNumToRead, dwCachePosEnd - dwFilePointer );
					
					dwOffset = ( dwFilePointer % nBytesPerSector );
					memcpy( lpBuffer, lpFile->lpbCacheBuf + dwOffset, dwMaxNum );
					lpBuffer = (LPBYTE)lpBuffer + dwMaxNum;
					dwNumToRead -= dwMaxNum;
					sr += dwMaxNum;
					dwFilePointer += dwMaxNum;
				}
			}

			if( dwNumToRead )
			{
				dwStartSector = GetSectorOffset( lpFile->lpVol, GET_NODE_PTR(lpFile)->dwStartSector, (dwFilePointer / nBytesPerSector) );
				dwOffset = (dwFilePointer % nBytesPerSector );
				//lpSector = malloc( lpFile->lpVol->fdd.nBytesPerSector );
				lpSector = lpFile->lpbCacheBuf;
				//if( lpSector )
				{
					DWORD k;
					k = ReadSectors( lpFile->lpVol, dwStartSector, dwOffset, (LPBYTE)lpBuffer, dwNumToRead, lpSector, &lpFile->uiCacheValid );
					
					ASSERT_NOTIFY( k == dwNumToRead, "not read enough bytes\r\n" );
					if( k == 0 )
					{
						LeaveCriticalSection( &lpFile->lpfn->csFileNode );
						goto _error_ret; 
					}
					sr += k;
					
					if( lpFile->uiCacheValid )
					{
						lpFile->dwCachePosStart = ( (dwFilePointer + k - 1) / nBytesPerSector ) * nBytesPerSector;
						//lpFile->dwCachePosEnd = lpFile->dwCachePosStart + nBytesPerSector;
					}
					//free( lpSector );
				}
			}
		}
		if( lpdwNumRead )
			*lpdwNumRead = sr;
		lpFile->dwFilePointer += sr;

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );

		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: --ReadFile, readnum=%d\r\n"), sr ));
		return TRUE;
	}
	else
	    SetLastError(ERROR_ACCESS_DENIED);
_error_ret:
	return FALSE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL KFSD_ReadFileWithSeek(
                          PFILE pf,
                          LPVOID pBuffer,
                          DWORD cbRead,
                          LPDWORD pcbRead,
						  LPOVERLAPPED pOverlapped,
                          DWORD dwLowOffset,
                          DWORD dwHighOffset)
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here

    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: ReadFileWithSeek\r\n")));
	if(  lpFile->flag == NORMAL_FILE )
		;
    lpFile->uiCacheValid = FALSE;    //????
    return FALSE;    
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD KFSD_SetFilePointer(
                         PFILE pf,
                         LONG lDistanceToMove,
						 LPLONG pDistanceToMoveHigh,
                         DWORD dwMoveMethod)
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here
    LONG cur;

	if( (lpFile->dwAccess & (GENERIC_WRITE|GENERIC_READ)) && lpFile->flag == NORMAL_FILE )
	{
		EnterCriticalSection( &lpFile->lpfn->csFileNode );

		if( dwMoveMethod == FILE_BEGIN )
			cur = 0;
		else if( dwMoveMethod == FILE_CURRENT )
			cur = lpFile->dwFilePointer;
		else if( dwMoveMethod == FILE_END )
		{
			cur = GET_NODE_PTR(lpFile)->nFileLength;
		}
		cur += lDistanceToMove;
		if( cur < 0 )
			cur = 0;
		//else if( cur > lpFile->node.nFileLength )
			//cur = lpFile->node.nFileLength;
		lpFile->dwFilePointer = cur;

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );

		
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: SetFilePoiter=%d\r\n"), cur));
		
		//lpFile->wSector = GetSectorOffset( lpFile->lpVol, lpFile->node.wStartSector, lpFile->dwFilePointer / lpFile->lpVol->fdd.nBytesPerSector );
		//lpFile->wOffset = lpFile->dwFilePointer % lpFile->lpVol->fdd.nBytesPerSector;
		if( pDistanceToMoveHigh )
			*pDistanceToMoveHigh = 0;
		return cur;
	}
	else
	    SetLastError(ERROR_ACCESS_DENIED);

	return 0xFFFFFFFFl;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD KFSD_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh )
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here
	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileSize=%d\r\n"), GET_NODE_PTR(lpFile)->nFileLength));
	//RETAILMSG(1, (TEXT("RAMFSD: GetFileSize=%d.\r\n"), GET_NODE_PTR(lpFile)->nFileLength));
	if(  lpFile->flag == NORMAL_FILE )
	{
		DWORD dwSize;
		if( pFileSizeHigh )
			*pFileSizeHigh = 0;
		EnterCriticalSection( &lpFile->lpfn->csFileNode );
		dwSize = GET_NODE_PTR(lpFile)->nFileLength;
		LeaveCriticalSection( &lpFile->lpfn->csFileNode );
		return dwSize;
	}
	else
		return INVALID_FILE_SIZE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL KFSD_GetFileInformationByHandle(
                                    PFILE pf,
                                    FILE_INFORMATION * pfi )
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here	
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileInfoByHandle\r\n")));

	if( lpFile->flag == NORMAL_FILE )
	{		
		memset( pfi, 0, sizeof( FILE_INFORMATION ) );

		EnterCriticalSection( &lpFile->lpfn->csFileNode );

		pfi->dwFileAttributes = GET_NODE_PTR(lpFile)->wAttrib;
		pfi->dwVolumeSerialNumber = lpFile->lpVol->rfh.dwSerialNum;
		//pfi->ftCreationTime =
		pfi->nFileIndexHigh = 0;
		pfi->nFileIndexLow = 0;
		pfi->nFileSizeHigh = 0;
		pfi->nFileSizeLow = GET_NODE_PTR(lpFile)->nFileLength;
		pfi->nNumberOfLinks = 1;
		pfi->ftCreationTime.dwLowDateTime = GET_NODE_PTR(lpFile)->ft.dwLowDateTime;
		pfi->ftCreationTime.dwHighDateTime = GET_NODE_PTR(lpFile)->ft.dwHighDateTime;
		*(&pfi->ftLastAccessTime) = *(&pfi->ftCreationTime);
		*(&pfi->ftLastWriteTime) = *(&pfi->ftCreationTime);

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );
		
		return TRUE;
	}
	return FALSE;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL  KFSD_GetFileTime( 
                      PFILE pf,
                      FILETIME * pCreate,
					  FILETIME * pLastAccess, 
					  FILETIME * pLastWrite)
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here  
	BOOL bRetv = FALSE;

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileTime++\r\n")));
	if( lpFile->flag == NORMAL_FILE )
	{
		EnterCriticalSection( &lpFile->lpfn->csFileNode );
		
		if( pCreate )
		{
			pCreate->dwLowDateTime = GET_NODE_PTR(lpFile)->ft.dwLowDateTime; 
			pCreate->dwHighDateTime = GET_NODE_PTR(lpFile)->ft.dwHighDateTime;
			bRetv = TRUE;
		}
		if( pLastAccess )
		{
			pLastAccess->dwLowDateTime = GET_NODE_PTR(lpFile)->ft.dwLowDateTime; 
			pLastAccess->dwHighDateTime = GET_NODE_PTR(lpFile)->ft.dwHighDateTime;
			bRetv = TRUE;
		}
		if( pLastWrite )
		{
			pLastWrite->dwLowDateTime = GET_NODE_PTR(lpFile)->ft.dwLowDateTime; 
			pLastWrite->dwHighDateTime = GET_NODE_PTR(lpFile)->ft.dwHighDateTime;
			bRetv = TRUE;
		}
		LeaveCriticalSection( &lpFile->lpfn->csFileNode );
	}
	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetFileTime--\r\n")));
    return bRetv;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL KFSD_WriteFile( 
                   PFILE pf, 
                   LPCVOID lpBuffer,
                   DWORD dwNumToWrite, 
                   LPDWORD lpdwNumWrite,
				   LPOVERLAPPED pOverlapped)
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here
    DWORD sr = 0;
    DWORD dwStartSector;
    DWORD dwOffset;
    DWORD fs;
	void * lpSector;
	BOOL bRetv = FALSE;

    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: ++WriteFile, num= %d, pointer=%d, fl=%d\r\n"), dwNumToWrite, lpFile->dwFilePointer, GET_NODE_PTR(lpFile)->nFileLength));

	if( lpdwNumWrite )
        *lpdwNumWrite = 0;
	if( (lpFile->dwAccess & GENERIC_WRITE) && lpFile->flag == NORMAL_FILE )
	{
		if( dwNumToWrite )
		{
			EnterCriticalSection( &lpFile->lpfn->csFileNode );

			fs = lpFile->dwFilePointer + dwNumToWrite;
			
			
			if( fs > GET_NODE_PTR(lpFile)->nFileLength )
			{
				//WORD ws = (WORD)( (fs + lpFile->lpVol->fdd.nBytesPerSector - 1) / lpFile->lpVol->fdd.nBytesPerSector);		
				if( ReallocFile( lpFile, fs ) == FALSE )
				{
					LeaveCriticalSection( &lpFile->lpfn->csFileNode );
//					bRetv = FALSE;
					goto _return;
					//return FALSE;
				}
				GET_NODE_PTR(lpFile)->nFileLength = fs;
				RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Realloc Sector Success, fs=%d\r\n"), fs));
			}
			
			dwStartSector = GetSectorOffset( lpFile->lpVol, GET_NODE_PTR(lpFile)->dwStartSector, (lpFile->dwFilePointer / lpFile->lpVol->fdd.nBytesPerSector) );
			dwOffset = (lpFile->dwFilePointer % lpFile->lpVol->fdd.nBytesPerSector);
			
			lpSector = lpFile->lpbCacheBuf;//malloc( lpFile->lpVol->fdd.nBytesPerSector );

			sr = WriteSectors( lpFile->lpVol, dwStartSector, dwOffset, (const unsigned char *)lpBuffer, dwNumToWrite, lpSector );

			if( sr )
			{
				if( lpdwNumWrite )
					*lpdwNumWrite = sr;
				lpFile->dwFilePointer += sr;
				lpFile->uiCacheValid = FALSE;
				RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: --WriteFile, writenum= %d\r\n"), sr));
				bRetv = TRUE;
			}
//			else
//				bRetv = FALSE;
				//return FALSE;			
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
		}
		else
			bRetv = TRUE;
		//return TRUE;
	}
_return:
	return bRetv;//FALSE;
}

BOOL KFSD_FlushFileBuffers(  PFILE pf ) 
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FlushFileBuffer\r\n")));
	if( (lpFile->dwAccess & GENERIC_WRITE) && 
		lpFile->flag == NORMAL_FILE )
		;
	lpFile->uiCacheValid = FALSE;

    return TRUE;
}

BOOL KFSD_WriteFileWithSeek( 
                           PFILE pf, 
                           LPCVOID pBuffer, 
                           DWORD cbWrite, 
                           LPDWORD pcbWritten, 
                           DWORD dwLowOffset, 
                           DWORD dwHighOffset ) 
                           
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: WriteFileWithSeek\r\n")));
    if( (lpFile->dwAccess & GENERIC_WRITE) && 
		lpFile->flag == NORMAL_FILE )
		;
    lpFile->uiCacheValid = FALSE;
    return FALSE;    
}

BOOL KFSD_SetFileAttributes( 
                           PVOL pVol,
                           LPCTSTR lpszFileName, 
                           DWORD dwFileAttributes ) 
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //do somthing here
	SF sf;
	int bRetv = FALSE;
    
    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: SetFileAttrib:%s, Attrib=0x%x\r\n"), lpszFileName, dwFileAttributes));


	// LN,2003-09-27, check volume is write enable ?
	if( (lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_SetFileAttributes: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//


	lpszFileName = IgnoreSpace( lpszFileName );

    // LN: 2003-05-20, 检查参数
    if( dwFileAttributes == FILE_ATTRIBUTE_NORMAL )
        dwFileAttributes = 0;
    else
        dwFileAttributes &= ~( FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ROM | FILE_ATTRIBUTE_DIRECTORY );
	//

	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
    {        
        //do somthing here
		bRetv = SearchNode( &sf );
        if( bRetv )
        {
		    //PNODE pNode = ;

			//DoNodeGet( &sf, &node );
			// LN: 2003-05-20, 检查参数
            if( sf.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY )
			{
                dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            }
            if( sf.pCurNode->wAttrib & FILE_ATTRIBUTE_DEVICE )
			{
                dwFileAttributes |= FILE_ATTRIBUTE_DEVICE;
            }			
			//
			sf.pCurNode->wAttrib = (WORD)(dwFileAttributes & 0xFFFF);
			DoNodeUpdate( &sf, sf.pCurNode );//&node );
			DoShellNotify( lpVol, SHCNE_ATTRIBUTES, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszFileName, NULL, sf.pCurNode );//&node );
        }
        //else
			//SetLastError(ERROR_FILE_NOT_FOUND);
		EndSearch( &sf );
    }
    return bRetv;
}

BOOL KFSD_MoveFile( 
                  PVOL pVol,
                  LPCTSTR lpszOldFileName, 
                  LPCTSTR lpszNewFileName )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //do somthing here
	SF sfOld;
	SF sfNew;
	BOOL bRetv = FALSE;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: MoveFile, Old=%s, New=%s\r\n"), lpszOldFileName, lpszNewFileName));

	// LN,2003-09-27, check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_MoveFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//


	lpszOldFileName = IgnoreSpace( lpszOldFileName );
	lpszNewFileName = IgnoreSpace( lpszNewFileName );

	if( IsValidFilePathName( lpszNewFileName ) == 0 )
	{		
		return FALSE;
	}
	
	if( FindOpened( lpVol, lpszOldFileName ) != NULL )// LN: 2003-05-21
	{
		SetLastError(ERROR_SHARING_VIOLATION);
		return FALSE;
	}

    EnterCriticalSection( &lpVol->csNode );

	if( BeginSearch( &sfNew, lpVol, 0, (LPTSTR)lpszNewFileName ) )
	{   
		bRetv = SearchNode( &sfNew );
		if( bRetv )
		{// error , the new file is exist
			EndSearch( &sfNew );			
			bRetv = FALSE;
			goto RETV;
		}
		if( BeginSearch( &sfOld, lpVol, 0, (LPTSTR)lpszOldFileName ) )
		{
			bRetv = SearchNode( &sfOld );
			if( bRetv = ( bRetv && !( sfOld.pCurNode->wAttrib & FILE_ATTRIBUTE_DEVICE ) ) )
			{
				if( *(sfNew.lpszCurName + sfNew.dwCurNameLen) == 0 )
				{
					//DoNodeGet( &sfOld, &node );
					//GetFolderAdr( node.strName, sfNew.lpszCurName );
					//memcpy( sfNew.pCurNode, sfOld.pCurNode, GET_NODE_SIZE( ssfNew.wCurNameLen );
					*sfNew.pCurNode = *sfOld.pCurNode;
                    sfNew.pCurNode->bNameLen = (BYTE)sfNew.dwCurNameLen;
					memcpy( sfNew.pCurNode->strName, sfNew.lpszCurName, sfNew.dwCurNameLen );
					bRetv = DoNodeAdd( &sfNew, sfNew.pCurNode );//&node );
					if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{
			            PNODE pNode;
						// EnterCriticalSector
						//EnterCriticalSection( &lpVol->csNode );

						if( FSDMGR_ReadDisk( lpVol->hDsk, sfNew.pCurNode->dwStartSector, 1, sfNew.pSector, lpVol->fdd.nBytesPerSector ) == ERROR_SUCCESS )
						{
							pNode = (PNODE)( (LPBYTE)sfNew.pSector + NODE_ALIGN_SIZE );
							if( pNode->strName[0] == '.' &&  pNode->strName[1] == '.' )
							{   // update '..' node
								pNode->dwStartSector = sfNew.dwFolderStartSector;
								if( FSDMGR_WriteDisk( lpVol->hDsk, sfNew.pCurNode->dwStartSector, 1, sfNew.pSector, lpVol->fdd.nBytesPerSector ) == ERROR_SUCCESS )
									bRetv = TRUE;
								else
									bRetv = FALSE;
							}
							else
							{
								RETAILMSG(FILE_ERROR, (TEXT("RAMFSD: MoveFile, Error Folder\r\n")));
							}
						}
						// LeaveCriticalSector
						
						//LeaveCriticalSection( &lpVol->csNode );

					}
					if( bRetv )
					{   
						//NODE nodeOld;
						//DoNodeGet( &sfOld, &nodeOld );
						if( DoNodeDel( &sfOld, FALSE ) )
						{  // success, delete old
							DWORD dwEvent;
							NODE node = *sfOld.pCurNode;
							EndSearch( &sfOld );
							EndSearch( &sfNew );

							if( node.wAttrib & FILE_ATTRIBUTE_DIRECTORY )
								dwEvent = SHCNE_RENAMEFOLDER;
							else
								dwEvent = SHCNE_RENAMEITEM;

							LeaveCriticalSection( &lpVol->csNode );

							DoShellNotify( lpVol, dwEvent, SHCNF_PATH|SHCNF_FLUSHNOWAIT, lpszOldFileName, lpszNewFileName, &node );

							EnterCriticalSection( &lpVol->csNode );

							//DoShellNotify( lpVol, dwEvent, SHCNF_PATH|SHCNF_FLUSHNOWAIT, nodeOld.strName, node.strName, &node );
							bRetv = TRUE;
							goto RETV;
						}
					}
				}
			}
			EndSearch( &sfOld );
		}
		EndSearch( &sfNew );
	}

RETV:

	LeaveCriticalSection( &lpVol->csNode );

	return bRetv;
}

BOOL KFSD_DeleteFile(
                    PVOL pVol,
                    LPCTSTR lpszFileName )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //do somthing here
	SF sf;
	BOOL bRetv = FALSE;
	
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DeleteFile=%s\r\n"),lpszFileName));

	// LN,2003-09-27, check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_DeleteFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//


	lpszFileName = IgnoreSpace( lpszFileName );

	if( FindOpened( lpVol, lpszFileName ) != NULL )// LN: 2003-05-21
	{
		SetLastError(ERROR_SHARING_VIOLATION);
		return FALSE;
	}
	
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
	{
		bRetv = SearchNode( &sf );
		if( bRetv  )
		{
			//NODE node;
//			BOOL bDelete;
			//bRetv = DoNodeGet( &sf, &node );
			if( bRetv && 
				( sf.pCurNode->wAttrib & ( FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM ) ) == 0 )
				//(node.wAttrib&FILE_ATTRIBUTE_READONLY) == 0)//!IS_DISABLE_DEL( sf.pCurNode->wAttrib ) )
			{				
				if( (bRetv = DoNodeDel( &sf, TRUE )) != FALSE )
				{
					NODE node = *sf.pCurNode;
					EndSearch( &sf );

					DoUpdateVolume( lpVol );

					bRetv = DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszFileName, NULL, &node );
					//bRetv = DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, node.strName, NULL, &node );
					RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DeleteFile Sucess=%d\r\n"), bRetv));
					return TRUE;
				}
			}
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DeleteFile ACCESS_DENIED\r\n") ));
			SetLastError( ERROR_ACCESS_DENIED );
		}
		EndSearch( &sf );
	}
	return FALSE;
}

BOOL KFSD_DeleteAndRenameFile( 
                              PVOL pVol,
                              LPCTSTR lpszNewFileName, 
                              LPCTSTR lpszOldFileName )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //do somthing here
	SF sfOld;
	SF sfNew;
	LPRAMFSDFILE lpFile;
	BOOL bRetv = FALSE;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DeleteAndRenameFile,Old=%s, New=%s\r\n"), lpszOldFileName, lpszNewFileName));

	// LN,2003-09-27, check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_DeleteAndRenameFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//

	lpszNewFileName = IgnoreSpace( lpszNewFileName );
	lpszOldFileName = IgnoreSpace( lpszOldFileName );

	if( IsValidFilePathName( lpszNewFileName ) == 0 )
	{		
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );// LN, 2003-06-03, ADD

	if( FindOpened( lpVol, lpszOldFileName ) != NULL )// LN: 2003-05-21
	{
		SetLastError(ERROR_SHARING_VIOLATION);
		goto _RET;
	}

	if( (lpFile = FindOpened( lpVol, lpszNewFileName )) != NULL )// LN: 2003-05-21
	{
		if( !(lpFile->dwShareMode & FILE_SHARE_WRITE) )
		{
			SetLastError(ERROR_SHARING_VIOLATION);
			goto _RET;
		}
	}	

	if( BeginSearch( &sfNew, lpVol, 0, (LPTSTR)lpszNewFileName ) )
	{   
		bRetv = SearchNode( &sfNew );
		if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			//NODE nodeNew;
			//DoNodeGet( &sfNew, &nodeNew );
			if( BeginSearch( &sfOld, lpVol, 0, (LPTSTR)lpszOldFileName ) )
			{
				bRetv = SearchNode( &sfOld );
				if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
				{
					//NODE nodeOld;
					DWORD dwSector;
					//DoNodeGet( &sfOld, &nodeOld );
                    // Entercrisector ???
					dwSector = sfNew.pCurNode->dwStartSector;//nodeNew.wStartSector;
					
					sfNew.pCurNode->dwStartSector = sfOld.pCurNode->dwStartSector;
					sfNew.pCurNode->nFileLength = sfOld.pCurNode->nFileLength;

					if( DoNodeUpdate( &sfNew, sfNew.pCurNode ) )//&nodeNew ) )
					{					
						NODE nodeNew, nodeOld;

						nodeNew = *sfNew.pCurNode;
						nodeOld = *sfOld.pCurNode;

						if( dwSector != NULL_SECTOR )   // LN, 2003-06-03, ADD
						    FreeSector( lpVol,  dwSector );
						DoNodeDel( &sfOld, FALSE );
						EndSearch( &sfOld );
						EndSearch( &sfNew );

						DoUpdateVolume( lpVol );

						DoShellNotify( lpVol, SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszNewFileName, NULL, &nodeNew );
						DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszOldFileName, NULL, &nodeOld );
						bRetv = TRUE;
						goto _RET;
					}
				}
				EndSearch( &sfOld );
			}
		}
		EndSearch( &sfNew );
		bRetv = FALSE;
	}

_RET:

	LeaveCriticalSection( &lpVol->csNode );  // LN, 2003-06-03, ADD

	return bRetv;
}

static BOOL _CopySectors( LPRAMFSDVOL lpVol, 
						  DWORD dwSectorFrom, 
						  DWORD dwSectorTo, 
						  DWORD dwSize,
						  void * lpSectorBuf0,   // temp buf
						  void * lpSectorBuf1     // temp buf
						  )
{
//    void * lpSector;
	DWORD dwSectorSize = lpVol->fdd.nBytesPerSector;
	DWORD dwOffset;

    //lpSector = (LPBYTE)malloc( dwSectorSize );
    //if( lpSector == NULL )
    //    return FALSE;
	dwOffset = 0;
    while( dwOffset < dwSize )
	{
	    if( ReadSectors( lpVol, dwSectorFrom, 0, lpSectorBuf0, dwSectorSize, lpSectorBuf1, NULL ) )
		{
	        if( WriteSectors( lpVol, dwSectorTo, 0, lpSectorBuf0, dwSectorSize, lpSectorBuf1 ) == 0 )
				break;
		}
		else
			break;

		// get next sector
		dwSectorFrom = GetSectorOffset( lpVol, dwSectorFrom, 1 );
		dwSectorTo = GetSectorOffset( lpVol, dwSectorTo, 1 );

		dwOffset += dwSectorSize;
	}

	//free( lpSector );
	return (dwOffset >= dwSize);
}

BOOL KFSD_CopyFile(
                     PVOL pVol,
					 LPCTSTR lpExistingFileName, // pointer to name of an existing file
					 LPCTSTR lpNewFileName,  // pointer to filename to copy to
					 BOOL bFailIfExists      // flag for operation if file exists
					 )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //do somthing here
	SF sf, sfNew;
	BOOL bExistFile, bRetv = FALSE;
	
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: CopyFile=%s\r\n"),lpExistingFileName));

	// LN,2003-09-27, check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_CopyFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//

	lpExistingFileName = IgnoreSpace( lpExistingFileName );
	lpNewFileName = IgnoreSpace( lpNewFileName );

	if( IsValidFilePathName( lpNewFileName ) == 0 )
	{
		RETAILMSG( 1, ( "error in KFSD_CopyFile: not valid file path name.\r\n" ) );
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );

	if( BeginSearch( &sfNew, lpVol, NULL, (LPTSTR)lpNewFileName ) )
	{
		if( (bExistFile = SearchNode( &sfNew )) )
		{
			if( bFailIfExists || (sfNew.pCurNode->wAttrib&FILE_ATTRIBUTE_DIRECTORY) )
			{
				RETAILMSG( 1, ( "error in KFSD_CopyFile: new file (%s) exist or is a directory !.\r\n", lpNewFileName ) );
				//SetLastError( ERROR_FILE_EXISTS );
				EndSearch( &sfNew );
				//return FALSE;
				goto RETV;
			}
		}

	    if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpExistingFileName ) )
		{
			if( SearchNode( &sf )  )
			{   // find the file
				PNODE pNode, pNodeNew;
				pNode = sf.pCurNode;
				pNodeNew = sfNew.pCurNode;
                // ln, 2003-09-29, 可以拷贝FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM文件
				//if( ( pNode->wAttrib&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM) ) == 0 )
				if( ( pNode->wAttrib&(FILE_ATTRIBUTE_DIRECTORY) ) == 0 )
				{  // not directory, is file
					DWORD dwStartSector = NULL_SECTOR;

					if( bExistFile )
					{   // the new file is exist, now extend it's space
                        //if( DoNodeGet( &sfNew, &nodeNew ) )
					    dwStartSector = ReallocSector( lpVol, pNodeNew->dwStartSector, ( (pNode->nFileLength + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector) );
					}
					else
					{
						//nodeNew.nFileLength = 0;
						//nodeNew.wStartSector = NULL_SECTOR;
						//nodeNew.nChildNodeNum = node.nChildNodeNum;
						//nodeNew.wAttrib = node.wAttrib;//(WORD)dwFlagsAndAttributes;
						//GetFolderAdr( nodeNew.strName, sfNew.lpszCurName );
						//memcpy( pNodeNew, pNode, GET_NODE_SIZE(pNode) );//LN:2003-04-30，删除
						memset( pNodeNew, 0, MAX_NODE_BYTES );
						memcpy( pNodeNew, pNode, GET_NODE_ATTRIB_SIZE(pNode) );//LN:2003-04-30，增加
						memcpy( pNodeNew->strName, sfNew.lpszCurName, sfNew.dwCurNameLen );//LN:2003-04-30，增加
						pNodeNew->bNameLen = (BYTE)sfNew.dwCurNameLen;//LN:2003-04-30，增加
						pNodeNew->dwStartSector = NULL_SECTOR;
						pNodeNew->nFileLength = 0;
						//if( DoNodeAdd( &sfNew, &nodeNew ) )
                        if( DoNodeAdd( &sfNew, pNodeNew ) )
						{
							dwStartSector = ReallocSector( lpVol, pNodeNew->dwStartSector, ( (pNode->nFileLength + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector) );
							if( dwStartSector == NULL_SECTOR )
							{
								DoNodeDel( &sfNew, FALSE );
							}
						}
					}
					if( dwStartSector != NULL_SECTOR )
					{
						// copy source sector to target sector
			//			nodeNew.wAttrib = node.wAttrib;//(WORD)dwFlagsAndAttributes;
			//			nodeNew.ft = node.ft;
			//			nodeNew.wStartSector =  wStartSector;
			//			nodeNew.nFileLength = node.nFileLength;
			//			nodeNew.nChildNodeNum = node.nChildNodeNum;

						if( _CopySectors( lpVol, pNode->dwStartSector, dwStartSector, pNode->nFileLength, sf.pSector, sfNew.pSector ) )
						{
							pNodeNew->wAttrib = pNode->wAttrib;
							pNodeNew->ft = pNode->ft;
							pNodeNew->dwStartSector = dwStartSector;
							pNodeNew->nFileLength = pNode->nFileLength;
							
							bRetv = DoNodeUpdate( &sfNew, pNodeNew );//&nodeNew );
						}
						else if( !bExistFile )
						{
							DoNodeDel( &sfNew, TRUE );
						}
						DoUpdateVolume( lpVol );
					}
				}
			}
			else
			{
				RETAILMSG( 1, ( "error in KFSD_CopyFile: file not find(%s)!.\r\n", lpExistingFileName ) );
			}
			//else
				//SetLastError(ERROR_FILE_NOT_FOUND);
			EndSearch( &sf );
		}
		EndSearch( &sfNew );
	}
	
RETV:
	LeaveCriticalSection( &lpVol->csNode );

	return bRetv;
}


BOOL KFSD_SetEndOfFile( PFILE pf )
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here    
    DWORD fs = lpFile->dwFilePointer;
	BOOL bRetv = FALSE;

    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: SetEndOfFile\r\n")));
	// LN, 2003-09-27, 增加写检查
	if( (lpFile->dwAccess & GENERIC_WRITE) &&
		lpFile->flag == NORMAL_FILE )
	{
		EnterCriticalSection( &lpFile->lpfn->csFileNode );

		if( fs != GET_NODE_PTR(lpFile)->nFileLength )
			bRetv = ReallocFile( lpFile, fs );
		else
			bRetv = TRUE;
        if( bRetv )
		{
            GET_NODE_PTR(lpFile)->nFileLength = fs;
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
		    DoUpdateVolume( lpFile->lpVol );
		}
		else
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
		lpFile->uiCacheValid = FALSE;
	}
	return bRetv;
}

//
BOOL KFSD_SetFileTime(
                     PFILE pf, 
                     const FILETIME *pCreate,
					 const FILETIME *pLastAccess,
					 const FILETIME *pLastWrite )
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;
    //  do somthing here
	
	if( lpFile->flag == NORMAL_FILE )
	{
		if( pCreate )
		{
			EnterCriticalSection( &lpFile->lpfn->csFileNode );
			RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: SetFileTime, hi=0x%x, lo=0x%x\r\n"), pCreate->dwHighDateTime, pCreate->dwLowDateTime));
			GET_NODE_PTR(lpFile)->ft.dwLowDateTime = pCreate->dwLowDateTime;
			GET_NODE_PTR(lpFile)->ft.dwHighDateTime = pCreate->dwHighDateTime;
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
			return TRUE;
		}
		//else
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: SetFileTime, pCreate is null\r\n")));
	}

    return FALSE;
}

static CloseVolume( LPRAMFSDVOL lpVol )
{
    //do somthing here
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: CloseVolume\r\n")));
    DoUpdateVolume( lpVol );

    if( RemoveVolume( lpVol ) )
    {
        DeleteVolume( lpVol );    
        return TRUE;
    }
    return FALSE;
}

BOOL KFSD_CloseVolume( PVOL pVol )
{    
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
	if( lpVol )
	    return CloseVolume( (LPRAMFSDVOL)pVol );
	return FALSE;
}

BOOL KFSD_CreateDirectory( 
                         PVOL pVol, 
                         LPCTSTR lpszPathName,
						 PSECURITY_ATTRIBUTES pSecurityAttributes)
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //  do somthing here
	SF sf;
	int bRetv = FALSE;

    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: CreateDirectory: %s\r\n"), lpszPathName));

	// LN,2003-09-27, check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_CreateDirectory: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//

	lpszPathName = IgnoreSpace( lpszPathName );

	if( IsValidFilePathName( lpszPathName ) == 0 )
	{		
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );

	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszPathName ) )
	{			
//		NODE node;
		bRetv = SearchNode( &sf );
		if( bRetv == FALSE )
		{
			SYSTEMTIME st;
//			memset( &node, 0, sizeof( node ));

			memset( sf.pCurNode, 0, sizeof( NODE ) );
			sf.pCurNode->dwStartSector = NULL_SECTOR;
			sf.pCurNode->wAttrib = FILE_ATTRIBUTE_DIRECTORY;
			//GetFolderAdr( node.strName, sf.lpszCurName );
			memcpy( sf.pCurNode->strName, sf.lpszCurName, sf.dwCurNameLen );
			sf.pCurNode->bNameLen = (BYTE)sf.dwCurNameLen;
				
			GetSystemTime( &st );
		    SystemTimeToFileTime( &st, &sf.pCurNode->ft );

			bRetv = DoFolderChange( &sf, sf.pCurNode, OP_ADD );
			if( bRetv )
			{
				DoUpdateVolume( lpVol );
				DoShellNotify( lpVol, SHCNE_MKDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszPathName, NULL, sf.pCurNode );
			}

		}
		else
		{
			SetLastError(ERROR_ALREADY_EXISTS);
			bRetv = FALSE;
		}
		EndSearch( &sf );
	}
	
	LeaveCriticalSection( &lpVol->csNode );

	return bRetv;
}

BOOL
KFSD_DeviceIoControl(
    DWORD pf,
    DWORD dwIoControlCode,
    LPVOID pInBuf,
    DWORD nInBufSize,
    LPVOID pOutBuf,
    DWORD nOutBufSize,
    LPDWORD pBytesReturned
    )
{
    LPRAMFSDFILE lpFile = (LPRAMFSDFILE)pf;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DeviceIoControl\r\n")));   
	switch( dwIoControlCode )
	{
	case  IOCTL_DISK_FORMAT_VOLUME: // format disk
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DeviceIoControl - Format Volume\r\n")));   
		if( lpFile->lpVol->lpOpenFile == lpFile && lpFile->lpNext == NULL )
		{

			TCHAR szName[1];
			NODE node;
			
			DoVolumeFormat( lpFile->lpVol, NULL );
			memset( &node, 0, sizeof( node ) );
			szName[0] = 0;

			node.wAttrib = FILE_ATTRIBUTE_DIRECTORY;
			DoShellNotify( lpFile->lpVol, SHCNE_DRIVEREMOVED, SHCNF_PATH | SHCNF_FLUSHNOWAIT, szName, NULL, &node );
			DoShellNotify( lpFile->lpVol, SHCNE_DRIVEADD, SHCNF_PATH | SHCNF_FLUSHNOWAIT, szName, NULL, &node );
			return TRUE;
		}
		else
		{
			;// printf error message
			RETAILMSG(FILE_ERROR, (TEXT("RAMFSD: DeviceIoControl - No ALL FILE CLOSED\r\n")));   
		}
		break;
    case  IOCTL_DISK_READONLY:
		lpFile->lpVol->fdd.dwFlags |= DISK_INFO_FLAG_READONLY;
		return TRUE;
	}

	return FALSE;
}


BOOL KFSD_FindClose( PFIND pfd )
{
    LPRAMFSDFIND lpfd = (LPRAMFSDFIND)pfd;
    //  do somthing here
	
	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindClose\r\n")));
    //EdbgOutputDebugString( "RAMFSD: FindClose DataHandle Ptr=%x\r\n", lpfd );

    //free( lpfd->lpszName );
	RemoveFindHandleFromVolume( lpfd );
    free( lpfd );
    return TRUE;
}

HANDLE KFSD_FindFirstFile( 
                         PVOL pVol, 
                         HANDLE hProc, 
                         LPCTSTR lpszFileSpec, 
                         FILE_FIND_DATA *  pfd )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //  do somthing here
    RAMFSDFIND * lpsf;
    //HANDLE hFind = 0;
	WORD fLen;
//	LPCTSTR lpstr;
	int bRetv;
	
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindFirstFile: %s\r\n"), lpszFileSpec ));   

	lpszFileSpec = IgnoreSpace( lpszFileSpec );

    fLen = strlen( lpszFileSpec );
	if( fLen == 0 )
		return INVALID_HANDLE_VALUE;
    
	lpsf = (RAMFSDFIND *) malloc( sizeof( RAMFSDFIND ) + (fLen + 1)*sizeof( TCHAR ) );
	memset( pfd, 0, sizeof( FILE_FIND_DATA ) );

    if( lpsf )
    {        
		//EdbgOutputDebugString( "RAMFSD: FindFirst DataHandle Ptr=%x\r\n", lpsf );
        lpsf->lpszName = (LPTSTR)(lpsf+1);
            // this code is now check the file 
		strcpy( lpsf->lpszName, lpszFileSpec );
		if( BeginSearch( lpsf, lpVol, hProc, lpsf->lpszName ) )
        {
			bRetv = SearchNode( lpsf );
			if( bRetv )
			{
             	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindFirstFile:Has Found\r\n")));   
				// if all is ok, do this...
                //hFind = FSDMGR_CreateSearchHandle( lpVol->hVol, hProc, (DWORD)lpsf );
                //if( hFind )
                {	
					memset( pfd, 0, sizeof( FILE_FIND_DATA ) );

					RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindFirstFile: node=%s\r\n"), lpsf->pCurNode->strName ));   

                    strncpy( pfd->cFileName, lpsf->pCurNode->strName, lpsf->pCurNode->bNameLen );//FILE_NAME_LEN );

                    //if( lpsf->pCurNode->strName[FILE_NAME_LEN-1] != 0 )
                    //pfd->cFileName[FILE_NAME_LEN] = 0;

                    pfd->dwFileAttributes = lpsf->pCurNode->wAttrib;
					pfd->ftCreationTime.dwLowDateTime = lpsf->pCurNode->ft.dwLowDateTime;
					pfd->ftCreationTime.dwHighDateTime = lpsf->pCurNode->ft.dwHighDateTime;
					*(&pfd->ftLastAccessTime)= *(&pfd->ftCreationTime);
					*(&pfd->ftLastWriteTime) = *(&pfd->ftCreationTime);
					pfd->nFileSizeLow = lpsf->pCurNode->nFileLength;
					pfd->nFileSizeHigh = 0;

					EndSearch( lpsf );

					// add to volume list
					AddFindHandleToVolume( lpsf );
					//lpsf->lpNext = lpVol->lpFindFile;
					//lpVol->lpFindFile = lpsf;
					//
                    //return hFind;
					return lpsf;
                }
            }
           	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindFirstFile:not Found\r\n")));   
			EndSearch( lpsf );
        }
        free( lpsf );
	    //SetLastError(ERROR_FILE_NOT_FOUND);
		//SetLastError(ERROR_NO_MORE_FILES);
    }
    return INVALID_HANDLE_VALUE;
}

BOOL KFSD_FindNextFile( 
                      PFIND pfd,
                      FILE_FIND_DATA * pffd ) 
{
    LPRAMFSDFIND lpfd = (LPRAMFSDFIND)pfd;
    //NODE node;
    //PNODE pNode;
    BOOL retv = FALSE;
    
    //  do somthing here

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindNextFile Data Ptr:%x\r\n"), pfd));   
    //EdbgOutputDebugString( "RAMFSD: FindNextFile DataHandle Ptr=%x\r\n", lpfd );
    
	if( BeginSearch( lpfd, lpfd->lpVol, lpfd->hProc, lpfd->lpszName ) )
    {
		//if( IS_NAME_BREAK(*lpfd->lpszCurName) )
		//	lpfd->lpszCurFolder = lpfd->lpszCurName+1;
		//else
		//	lpfd->lpszCurFolder = lpfd->lpszCurName;

		if( TRUE == (retv = SearchFolder( lpfd, TRUE, ALL_NODES )) )   // search next node
		{
			memset( pffd, 0, sizeof( FILE_FIND_DATA ) );

            strncpy( pffd->cFileName, lpfd->pCurNode->strName, lpfd->pCurNode->bNameLen );//FILE_NAME_LEN );
			
            //if( lpfd->pCurNode->strName[FILE_NAME_LEN-1] != 0 )
             //pffd->cFileName[FILE_NAME_LEN] = 0;

             pffd->dwFileAttributes = lpfd->pCurNode->wAttrib;
			 pffd->ftCreationTime.dwLowDateTime = lpfd->pCurNode->ft.dwLowDateTime;
			 pffd->ftCreationTime.dwHighDateTime = lpfd->pCurNode->ft.dwHighDateTime;
			 *(&pffd->ftLastAccessTime)= *(&pffd->ftCreationTime);
			 *(&pffd->ftLastWriteTime) = *(&pffd->ftCreationTime);
			 pffd->nFileSizeLow = lpfd->pCurNode->nFileLength; 
			 pffd->nFileSizeHigh = 0;

			 RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FindNextFile: FileName=%s, Size=%d\r\n"), pffd->cFileName, pffd->nFileSizeLow ));
		}
		else
			SetLastError(ERROR_NO_MORE_FILES);
        EndSearch( lpfd );
    }
    return retv;
}


BOOL KFSD_GetDiskFreeSpace(
                          PVOL pVol,
                          LPCTSTR lpszPathName,
                          LPDWORD pSectorsPerCluster,
                          LPDWORD pBytesPerSector,
                          LPDWORD pFreeClusters,
                          LPDWORD pClusters )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //  do somthing here
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: GetDiskFreeSpace-%s\r\n"), lpszPathName));

    lpszPathName = IgnoreSpace( lpszPathName );

	*pSectorsPerCluster = 1;
	*pBytesPerSector = lpVol->fdd.nBytesPerSector;
	*pFreeClusters = lpVol->rfh.nFreeCount;
	*pClusters = lpVol->fdd.nSectors;

    return TRUE;
}


static BOOL IsValidDisk( PRFH prfh, FSD_DISK_DATA * pfdd )
{
    if( memcmp( prfh->idName, idRam, strlen( idRam ) ) == 0 )
	{
		if( prfh->dwTotalSectors == pfdd->nSectors )
		{
			if( prfh->nFreeCount < pfdd->nSectors )
			{
				if( prfh->ver == EFILE_VERSION )
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL KFSD_MountDisk( HDSK hDsk )
{
    BOOL retv = FALSE;
    LPRAMFSDVOL lpVol;
	
   //RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RegisterVolume\r\n")));
   //FSDMGR_RegisterVolume( hDsk, TEXT("RAMFSD"), (DWORD)lpVol );
   //return TRUE;
	//EdbgOutputDebugString( "RAMFSD: MountDisk=%x\r\n", hDsk );

	//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: MountDisk=%d\r\n"),hDsk));
	//Sleep( 1000 );
	if( FindVolume( hDsk ) )
	{
		EdbgOutputDebugString( "RAMFSD: Find Volume in Mount, return\r\n" );
		//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Find Volume in Mount\r\n")));
		return FALSE;
	}

	lpVol = (LPRAMFSDVOL)CreateVolume();
    if( lpVol )
    {
        //RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Call FSDMGR_GetDiskInfo\r\n")));
		//EdbgOutputDebugString( "RAMFSD: Call FSDMGR_GetDiskInfo\r\n" );
		if( ERROR_SUCCESS == FSDMGR_GetDiskInfo( hDsk, &lpVol->fdd ) )
        {
            WORD * pSector;
            DWORD dwfat;
            PRFH prfh;

			EdbgOutputDebugString( "DiskInfo: nBytesPerSector=%d,nCylinders=%d,nHeadsPerCylinder=%d,nSectors=%d,nSectorsPerTrack=%d\r\n", 
				 lpVol->fdd.nBytesPerSector,
				 lpVol->fdd.nCylinders,
				 lpVol->fdd.nHeadsPerCylinder,
				 lpVol->fdd.nSectors,
				 lpVol->fdd.nSectorsPerTrack );
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Call FSDMGR_GetDiskInfo Success\r\n")));
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: nBytesPerSector=%d\r\n"), lpVol->fdd.nBytesPerSector));
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: nCylinders=%d\r\n"), lpVol->fdd.nCylinders));
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: nHeadsPerCylinder=%d\r\n"), lpVol->fdd.nHeadsPerCylinder));
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: nSectors=%d\r\n"), lpVol->fdd.nSectors));
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: nSectorsPerTrack=%d\r\n"), lpVol->fdd.nSectorsPerTrack));

			//InitializeCriticalSection( &csFileVolumeList );


            if( lpVol->fdd.nBytesPerSector < 128 )
            {
				EdbgOutputDebugString( "RAMFSD: Error FAT Size < 128\r\n" );
                DeleteVolume( lpVol );
                return FALSE;
            }
            //EdbgOutputDebugString( "RAMFSD: T1\r\n" );
            lpVol->hDsk = hDsk;
			//EdbgOutputDebugString( "RAMFSD: T2_3\r\n" );
            // align to sector
			//wfat = (lpVol->fdd.nSectors * sizeof( WORD ) + lpVol->fdd.nBytesPerSector - 1);
			//EdbgOutputDebugString( "RAMFSD: T2_4\r\n" );
			//wfat = wfat / lpVol->fdd.nBytesPerSector;
			//EdbgOutputDebugString( "RAMFSD: T2_5\r\n" );
			//wfat = wfat * lpVol->fdd.nBytesPerSector;

            dwfat = ( ( (lpVol->fdd.nSectors * sizeof( DWORD ) + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector ) * lpVol->fdd.nBytesPerSector );
			//EdbgOutputDebugString( "RAMFSD: T3\r\n" );
			//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Alloc FAT Size=%d \r\n"),wfat));
			EdbgOutputDebugString( "RAMFSD: Alloc FAT Size=%d \r\n", dwfat );
			//while(1);
            lpVol->lpdwFat = (DWORD*)malloc( dwfat );
			//EdbgOutputDebugString( "RAMFSD: T4\r\n" );
			//EdbgOutputDebugString( "RAMFSD: Alloc ok\r\n" );
            if( lpVol->lpdwFat )
            {            
                pSector = (WORD*)malloc( lpVol->fdd.nBytesPerSector );
				//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Alloc pSector \r\n")));
				EdbgOutputDebugString( "RAMFSD: Alloc pSector \r\n" );
                if( pSector )
                {              
					//_HeapCheck();
                    if( ERROR_SUCCESS == FSDMGR_ReadDisk( hDsk, 0, 1, (LPBYTE)pSector, lpVol->fdd.nBytesPerSector ) )
                    {
//						_HeapCheck();
                        RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: Read 0 Sector Success \r\n")));
						prfh = (PRFH)pSector;
						if( IsValidDisk( prfh, &lpVol->fdd ) )
                        {   
                            memcpy( &lpVol->rfh, prfh, sizeof( RAM_FILE_HEAD ) );
                            FSDMGR_ReadDisk( hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (LPBYTE)lpVol->lpdwFat, lpVol->fdd.nBytesPerSector * lpVol->rfh.dwFatSectorNum );
							EdbgOutputDebugString( "RAMFSD: valid esoft filesys. \r\n" );
                        }
                        else
                        {   // no esoft ram file sys or other file sys or has some error
							EdbgOutputDebugString( "RAMFSD: invalid esoft filesys. now to format it.\r\n" );
							DoVolumeFormat( lpVol, pSector );
                        }
						//_HeapCheck();
                        
				        EdbgOutputDebugString( "RAMFSD: FAT INFO:freecount=%d,ver=%d,data=%d,fatnum=%d,freesector=%d,serial=%x\r\n", lpVol->rfh.nFreeCount,lpVol->rfh.ver,lpVol->rfh.dwDataStartSector,lpVol->rfh.dwFatSectorNum,lpVol->rfh.dwFreeSector, lpVol->rfh.dwSerialNum );
						//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FAT INFO:freecount=%d,ver=%d,data=%d,fatnum=%d,freesector=%d\r\n"),lpVol->rfh.nFreeCount,lpVol->rfh.ver,lpVol->rfh.wDataStartSector,lpVol->rfh.nFatSectorNum,lpVol->rfh.wFreeSector));
						//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RegisterVolume\r\n")));

                        //lpVol->hVol = FSDMGR_RegisterVolume( hDsk, TEXT("RAMFSD"), (DWORD)lpVol );
						//lpVol->hVol = FSDMGR_RegisterVolume( hDsk, TEXT( "ESOFT" ), (DWORD)lpVol );
						lpVol->hVol = FSDMGR_RegisterVolume( hDsk, NULL, (DWORD)lpVol );
//						_HeapCheck();
                        
                        if( lpVol->hVol )
                        {
                            //RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RegisterVolume Sucess\r\n")));
							EdbgOutputDebugString( "RAMFSD: RegisterVolume Sucess\r\n" );
							FSDMGR_GetVolumeName( lpVol->hVol, lpVol->szVolName, sizeof( lpVol->szVolName ) );
							//EdbgOutputDebugString( "RAMFSD: 1\r\n" );
							free( pSector );
							//EdbgOutputDebugString( "RAMFSD: 2\r\n" );
							lpVol->nNodesPerSector = (WORD)( (lpVol->fdd.nBytesPerSector / NODE_ALIGN_SIZE) );
                            AddToVolumeList( lpVol );
							EdbgOutputDebugString( "RAMFSD: KFSD_MountDisk return\r\n" );


                            return TRUE;
                        }
                    }
                    else
                    {   // error to readdisk
                    }
                    free( pSector );
                }
                free( lpVol->lpdwFat );
            }
        }
        DeleteVolume( lpVol );        
    }
    return FALSE;    
}

void KFSD_Notify(
                PVOL pVol,
                DWORD dwFlags )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //  do somthing here
    switch( dwFlags )
    {
    case FSNOTIFY_POWER_ON:
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FSNOTIFY_POWER_ON\r\n")));
        break;
    case FSNOTIFY_POWER_OFF:
		//RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FSNOTIFY_POWER_OFF\r\n")));
		DoUpdateVolume( lpVol );
        break;
    case FSNOTIFY_DEVICES_ON:   // the device has resumed after a suspended mode, and devices are now available for use
		RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: FSNOTIFY_DEVICES_ON\r\n")));
        break;
    }
}

BOOL KFSD_RegisterFileSystemFunction( 
                                       PVOL pVol,
                                       SHELLFILECHANGEFUNC pscf )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    FILECHANGEINFO fci;

	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RegisterFileSystemFunction=0x%x\r\n"), pscf));

    lpVol->pscf = pscf;
    
    // fill fci data here

    // notify to system
	if( pscf )
	{
		memset( &fci, 0, sizeof( fci ) );
		pscf( &fci );
	}
    return TRUE;
}

BOOL KFSD_RemoveDirectory( 
                         PVOL pVol, 
                         LPCTSTR lpszPathName )
{
    LPRAMFSDVOL lpVol = (LPRAMFSDVOL)pVol;
    //  do somthing here
	SF sf;   
	BOOL bRetv = FALSE;
	int len;
	RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RemoveDirectory=%s\r\n"), lpszPathName));

	// LN,2003-09-27, check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // error 
		RETAILMSG( 1, ( "error in KFSD_RemoveDirectory: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//


	lpszPathName = IgnoreSpace( lpszPathName );

	len = strlen( lpszPathName );
	if( FindOpened( lpVol, lpszPathName ) )// LN: 2003-05-21
	{
		SetLastError(ERROR_SHARING_VIOLATION);
		RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, FindOpened.\r\n") ));
		return FALSE;
	}

	if( FindOpenedSearchHandle( lpVol, lpszPathName ) )// LN: 2003-05-21
	{
		SetLastError(ERROR_SHARING_VIOLATION);
		RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, FindOpenedSearchHandle.\r\n") ));
		return FALSE;
	}

	RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, BeginSearch .\r\n") ));
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszPathName ) )    
    {		
		RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, SearchNode .\r\n") ));
		if( SearchNode( &sf ) )
		{
			//NODE node;

			//DoNodeGet( &sf, &node );
			RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, SearchNode OK.\r\n") ));
			if( sf.pCurNode->wAttrib&FILE_ATTRIBUTE_DIRECTORY )
			{   RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, Is dir, now call del.\r\n") ));
				if( DoFolderChange( &sf, NULL, OP_DEL ) )
				{
					RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, UpdateValume del.\r\n") ));
					DoUpdateVolume( lpVol );

					DoShellNotify( lpVol, SHCNE_RMDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszPathName, NULL, sf.pCurNode );//&node );
					bRetv = TRUE;
				}
			}
			else
				SetLastError( ERROR_ACCESS_DENIED );
		}
		EndSearch( &sf );
	}
	RETAILMSG( 1, (TEXT("RAMFSD: RemoveDirectory, END.\r\n") ));
    RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: RemoveDirectory retv=%d\r\n"), bRetv));
    return bRetv;
}

BOOL KFSD_UnmountDisk( HDSK hDsk )
{    
    LPRAMFSDVOL lpVol;
    
	RETAILMSG(FILE_ZONE, (TEXT("++RAMFSD: UnmountDisk=%d\r\n"), hDsk));
	//return FALSE; ///base file system, ??????
	

    if( (lpVol = FindVolume( hDsk )) != NULL )
    {   
		//;//lpListVol
	    //LPRAMFSDFILE lpOpenFile;
	    //LPRAMFSDFIND lpFindFile;
		
		
		while( lpVol->lpFindFile )
		{
			KFSD_FindClose( (PFIND)lpVol->lpFindFile );
		}

		while( lpVol->lpOpenFile )
		{
			KFSD_CloseFile( (PFILE)lpVol->lpOpenFile );
		}

		FSDMGR_DeregisterVolume( lpVol->hVol );
		CloseVolume( lpVol );
		// this function remove to FSDRAM_CloseVolume
        //RemoveVolume( lpVol );
        //DeleteVolume( lpVol );
		return TRUE;
    }
	
	RETAILMSG(FILE_ZONE, (TEXT("--RAMFSD: UnmountDisk\r\n")));
	//if( lpListVol )
	//	return FALSE;
	return FALSE;
}

#ifdef WINCE_EML
BOOL APIENTRY  DllMain(HINSTANCE DllInstance, DWORD Reason, LPVOID Reserved)
{
    switch(Reason) {
        case DLL_PROCESS_ATTACH:
            RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DLL_PROCESS_ATTACH\r\n")));
			break;
        case DLL_PROCESS_DETACH:
            RETAILMSG(FILE_ZONE, (TEXT("RAMFSD: DLL_PROCESS_DETACH\r\n")));
            break;
    }
	return TRUE;
}
#endif

