/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：巨果.Kingmos file sys
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
问题：是否需要这么多CS ?
问题：对lpVol->csNode的锁范围有问题?!当对节点进行操作时，应该有更大的锁范围
	  example KFSD_MoveFile的锁范围
	  KFSD_DeleteFile 没有锁！！！

问题：我应该将file sector cache ptr放到file node struct文件节点，而不是
		open file struct ?!
******************************************************/


#ifdef WINCE_EML
    #include <windows.h> 
    #include <winioctl.h>
    #include <diskio.h>
    #include <string.h>
    #include <fat.h>
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
	#include <eshell.h>
	#include <diskio.h>
#endif

#define EFILE_VERSION  MAKE_VERSION( 2, 0, 0 )
#define NULL_SECTOR 0xffffffff

#define USER_NODES   2
#define ALL_NODES     0

const TCHAR idRam[] = TEXT("KMFS");
const TCHAR szVolMark[] = TEXT( "\\Vol:" );
// size of RAM_FILE_HEAD = Sector size, Sector Size must >= 32 bytes
// 定义文件系统头结构
typedef struct _RAM_FILE_HEAD
{
    BYTE  idName[4]; // 标识 = KMFS
    DWORD ver;      // 版本号
	DWORD dwSerialNum; // 序列号
	DWORD dwTotalSectors; // 该磁盘总的扇区数
    DWORD dwFatStartSector;  // 该磁盘FAT开始扇区 file alloc table start sector
    DWORD dwFatSectorNum;    // 该磁盘FAT扇区数 file alloc sector number
    DWORD dwFreeSector;		// 该磁盘空闲扇区开始位置
    DWORD nFreeCount;		// 该磁盘空闲扇区数
    DWORD dwDataStartSector; // 该磁盘数据扇区开始位置 data start sector
}RAM_FILE_HEAD, * PRFH;


//#define FILE_NAME_LEN (28+1+3)
// align 32 byte
// 文件节点为可以用标志
#define NODE_EMPTY   0
// 文件节点为删除标志
#define NODE_DELETED 0xfe
// 文件节点为占用标志
#define NODE_MARK    0xef

#define IS_NAME_BREAK( b ) ( (b) == '\\' || (b) == '/' )

#define GET_NODE_TYPE( lpNode ) ( (lpNode)->bFlag )

#define NODE_ALIGN_SIZE 32
#define MAX_FILE_NAME_LEN 256

#define NODE_RESERVE_NAME_LEN 12
// 定义文件磁盘节点数据结构，注意：该数据结构大小等于 NODE_ALIGN_SIZE 
// the node struct must == NODE_ALIGN_SIZE and the  strName must >= 2, because the name :'..'3 
typedef struct _NODE
{
	BYTE bFlag; // = NODE_EMPTY; NODE_DELETED; NODE_MARK
	BYTE bNameLen;  // 文件名长度, file name length
	WORD wAttrib; // 文件属性 file attrib
	DWORD dwStartSector;  // 文件数据开始扇区
	FILETIME ft;  // 文件创建时间 create time 8 byte
	DWORD nFileLength; // 文件长度 4 byte
	char strName[NODE_RESERVE_NAME_LEN];// file name  11 byte
}NODE, * PNODE;

#define MAX_NODE_BYTES ( ( MAX_FILE_NAME_LEN + sizeof( NODE ) - NODE_RESERVE_NAME_LEN + NODE_ALIGN_SIZE - 1 ) / NODE_ALIGN_SIZE * NODE_ALIGN_SIZE  )
#define GET_NODE_SIZE( pNode ) ( (pNode)->bNameLen + sizeof( NODE ) - NODE_RESERVE_NAME_LEN )
#define GET_NODE_ATTRIB_SIZE( pNode ) ( sizeof( NODE ) - NODE_RESERVE_NAME_LEN )
#define GET_NODE_NUM( pNode ) ( ( (pNode)->bNameLen + sizeof( NODE ) -  NODE_RESERVE_NAME_LEN + NODE_ALIGN_SIZE - 1 ) / NODE_ALIGN_SIZE )

//定义打开文件节点结构
typedef struct _FILENODE
{
	struct _FILENODE *lpNext;  // 指向下一个已打开文件结构
	LPTSTR lpszFileName;  // 当前打开文件路径名 full path name
	PNODE pNode;  // 文件节点数据指针
	DWORD dwRefCount; // 对该文件的引用数
    DWORD dwFolderSector;// 该文件所在的文件夹数据的开始扇区 folder's first sector of file node's 
    DWORD dwNodeSector;// 文件节点所在的扇区 file node at the which sector
    DWORD index;// 文件节点在扇区的索引号。file node index at the sector
    CRITICAL_SECTION csFileNode; // 冲突段
	BYTE bNodeBuf[MAX_NODE_BYTES]; // 为节点准备的缓存。a buf for pNode, the pNode pointer the buffer
}FILENODE, * LPFILENODE;

//

struct _KFSDVOL;

// 定义查找文件结构
typedef struct SF{
	struct SF * lpNext; // 指向下一个当前系统已打开查找文件对象
    // in
    struct _KFSDVOL FAR * lpVol; // 卷对象
    HANDLE hProc;  // 该查找结构的拥有者
    LPTSTR lpszName;   // 需要查找的文件路径 path file name
    void * pSector;	//一个扇区大小的指针，用于读取数据
	PNODE pCurNode;  // 当前文件节点
	// 
	DWORD dwFolderStartSector;  // 当前文件节点所在的文件夹数据的开始扇区 start sector of the folder
    DWORD dwCurNodeSector;   // 当前文件节点所在扇区 current sector with pSector    

    WORD nCurNodeIndex;	// 当前文件节点在扇区的索引 current index  with pSector
	WORD nCurNodeNum;  // 当前文件节点所占的节点数（每节点32bytes）

	DWORD dwCurNameLen;  // 当前文件名长度 current file name len
    
    LPTSTR lpszCurName;   // 当前文件名指针 current file name

    DWORD dwAddNodeSector;  // 在搜索过程中发现的空闲节点 sector of has empty node in current sector
    UINT nEmptyNodeIndex;   // 在搜索过程中发现的空闲节点索引号 index of has empty node in current sector
    UINT nDelNodeIndex;	// 在搜索过程中发现的已删除节点
	UINT nFreeNodeNum;	// 在搜索过程中发现的已删除节点索引号
}SF, * PSF;


#define NORMAL_FILE   0
#define VOL_FILE      1

// 定义打开文件结构
typedef struct _KFSDFILE
{    
    struct _KFSDFILE * lpNext;  // 指向下一个当前系统已打开文件对象
	struct _KFSDVOL FAR * lpVol;  // 当前文件所在的卷对象
    HANDLE hProc;	// 当前打开文件对象的拥有者

    DWORD dwAccess; // 当前打开文件的存取模式
    DWORD dwShareMode;	// 当前打开文件的共享模式
    DWORD dwCreate; // 当前打开文件的创建模式
    DWORD dwFlagsAndAttributes; // 调用CreateFile时的 文件标志和属性
    //
	DWORD flag; // 文件标志：1 normal file, 0, vol file  

    DWORD dwFilePointer;  // 当前打开文件存取位置
	LPBYTE lpbCacheBuf; // 缓冲，用于加速
    UINT  uiCacheValid;	// 缓冲数据有效标志
	DWORD dwCachePosStart; //缓冲数据在文件的开始位置
	LPFILENODE lpfn;  // 文件在磁盘中的节点数据
}KFSDFILE, FAR * LPKFSDFILE;

#define GET_NODE_PTR( lpFile ) ( (lpFile)->lpfn->pNode )
#define GET_NODE_OFFSET( index, lpVol ) ( (index) % (lpVol)->nNodesPerSector * NODE_ALIGN_SIZE ) 

typedef SF KFSDFIND;
typedef SF FAR *  LPKFSDFIND;

// 定义打开卷结构
typedef struct _KFSDVOL
{
    struct _KFSDVOL * lpNext;// 指向下一个当前系统已打开卷对象

    HVOL hVol; // 卷句柄
    HDSK hDsk;	// 卷所连接的磁盘
	TCHAR szVolName[32];  // 卷名
    FSD_DISK_DATA fdd;  //磁盘信息
    RAM_FILE_HEAD  rfh; // KFSD文件系统头
	WORD fUpdate;  // 是否以刷新
	WORD nNodesPerSector; // 每个扇区的节点数
#define FATBYTES  ( sizeof( DWORD ) )
    DWORD * lpdwFat;    // 该卷的文件分配表 file alloc table
	DWORD dwMinNeedUpdate;  // 该卷的文件分配表需要更新的开始位置
	DWORD dwMaxNeedUpdate;	// 该卷的文件分配表需要更新的结束位置
	
	CRITICAL_SECTION csNode; // 存取节点用的冲突段
	CRITICAL_SECTION csFat;	// 存取FAT表用的冲突段

	CRITICAL_SECTION csFileList; // 存取打开文件用的冲突段
	CRITICAL_SECTION csFindList;// 存取查找文件用的冲突段
 
	LPKFSDFILE lpOpenFile; // 已打开文件对象链表
	LPKFSDFIND lpFindFile;  // 已打开查找对象链表

    SHELLFILECHANGEFUNC pscf;  // 向系统通知改变的回调函数

	TCHAR szNotifyPath0[MAX_PATH];// 向系统通知需要的缓存1
	TCHAR szNotifyPath1[MAX_PATH];// 向系统通知需要的缓存2
	LPVOID lpSectorBuf;    // 为卷读写扇区的临时缓存
}KFSDVOL, FAR * LPKFSDVOL;


#define FILE_ZONE  0
#define FILE_ERROR 1

static LPKFSDVOL lpListVol = NULL; // 已打开卷链表
static CRITICAL_SECTION csVolumeList; // 存取卷链表的冲突段

static LPKFSDVOL CreateVolume( void );
static BOOL RemoveVolume( LPKFSDVOL );
static LPKFSDVOL FindVolume( HDSK hDsk );
static BOOL AddToVolumeList( LPKFSDVOL );

static DWORD ReadSectors( LPKFSDVOL lpVol, DWORD dwStartSector, 
						  DWORD nOffset, LPBYTE lpBuf, DWORD dwSize,
						  LPBYTE pSector, UINT * lpCacheValid );

static DWORD WriteSectors( LPKFSDVOL lpVol, 
						   DWORD dwStartSector, 
						   DWORD nOffset, 
						   const unsigned char * lpBuf, 
						   DWORD dwSize,
						   LPBYTE pSector // temp buf
 						   );

static BOOL BeginSearch( SF * lpsf, LPKFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName );
static BOOL SearchNode( 
					   SF * lpsf );
static void EndSearch( SF * lpsf );
static BOOL AddFileHandleToVolume( LPKFSDFILE lpFile );
static BOOL AddFindHandleToVolume( LPKFSDFIND lpFind );
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
// 文件系统驱动程序接口
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


// **************************************************
// 声明：BOOL _InstallKMFS( void )
// 参数：
// 		无  
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		初始化系统默认文件系统
// 引用: 
//		filesrv.c
// **************************************************

BOOL _InstallKMFS( void )
{
	InitializeCriticalSection( &csVolumeList );
#ifdef __DEBUG
	csVolumeList.lpcsName = "CS-VOL";
#endif
    return FSDMGR_RegisterFSD ( "KFSD", &drvFSD );
}

// **************************************************
// 声明：void _UnInstallKMFS( void )
// 参数：
//		无
// 返回值：
//		无
// 功能描述：
//		与 _InstallKMFS 相反，注销Kimgos file system
// 引用: 
// **************************************************

void _UnInstallKMFS( void )
{
	FSDMGR_UnregisterFSD(  "KMFS" );
	DeleteCriticalSection( &csVolumeList );
}

#endif


static BOOL AccessTest( LPKFSDVOL pVolume )
{
	// 卷是否可写。check volume is write enable ?
	if( (pVolume->fdd.dwFlags & DISK_INFO_FLAG_READONLY)  )
	{  // 不可写。error 
		WARNMSG( FILE_ZONE, ( "error in AccessTest ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//
	return TRUE;
}

// **************************************************
// 声明：static LPFILENODE CreateFileNode( LPCTSTR lpcszFileName )
// 参数：
// 		IN lpcszFileName - 文件名
// 返回值：
//		假如成功，返回新分配的文件节点指针；否则，返回NULL
// 功能描述：
//		创建文件节点指针
// 引用: 
// **************************************************

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

// **************************************************
// 声明：static void DeleteFileNode( LPFILENODE lpfn )
// 参数：
// 		IN lpfn - 文件节点指针
// 返回值：
//		无
// 功能描述：
//		与CreateFileNode相反，删除节点
// 引用: 
// **************************************************

static void DeleteFileNode( LPFILENODE lpfn )
{
	DeleteCriticalSection( &lpfn->csFileNode );
	free( lpfn );
}

// **************************************************
// 声明：static LPCTSTR IgnoreSpace( LPCTSTR lpcszFileName )
// 参数：
// 	IN lpcszFileName - 文件名
// 返回值：
//	文件名指针
// 功能描述：
//	略过文件名开头的空格，返回第一个非空格地址
// 引用: 
// **************************************************

static LPCTSTR IgnoreSpace( LPCTSTR lpcszFileName )
{
	while( *lpcszFileName == ' ' )
		lpcszFileName++;
	return lpcszFileName;
}

// **************************************************
// 声明：static LPKFSDVOL CreateVolume( void )
// 参数：
// 	无
// 返回值：
//	假如成功，返回新分配的卷指针；否则，返回NULL
// 功能描述：
// 引用: 
// **************************************************

static LPKFSDVOL CreateVolume( void )
{
    LPKFSDVOL lpVol;
    lpVol = (LPKFSDVOL)malloc( sizeof( KFSDVOL ) );
    if( lpVol )
	{
        memset( lpVol, 0, sizeof( KFSDVOL ) );
		InitializeCriticalSection( &lpVol->csNode );
#ifdef __DEBUG
		lpVol->csNode.lpcsName = "CS-NODE";
#endif
		InitializeCriticalSection( &lpVol->csFat );
#ifdef __DEBUG
		lpVol->csFat.lpcsName = "CS-FAT";
#endif
		InitializeCriticalSection( &lpVol->csFileList );
#ifdef __DEBUG
		lpVol->csFileList.lpcsName = "CS-FILELIST";
#endif
		InitializeCriticalSection( &lpVol->csFindList );
#ifdef __DEBUG
		lpVol->csFindList.lpcsName = "CS-FINDLIST";
#endif
	    lpVol->dwMinNeedUpdate = -1;
		lpVol->dwMaxNeedUpdate = 0;
	}
    return lpVol;
}

// **************************************************
// 声明：static void DeleteVolume( LPKFSDVOL lpVol )
// 参数：
// 	IN lpVol - 卷结构指针
// 返回值：
//	无
// 功能描述：
//	与 CreateVolume 相反，删除卷
// 引用: 
// **************************************************

static void DeleteVolume( LPKFSDVOL lpVol )
{
	DeleteCriticalSection( &lpVol->csNode );
	DeleteCriticalSection( &lpVol->csFat );
	DeleteCriticalSection( &lpVol->csFileList );
	DeleteCriticalSection( &lpVol->csFindList );

	free( lpVol->lpdwFat );
	free( lpVol->lpSectorBuf );
    free( lpVol );    
}

// **************************************************
// 声明：static LPKFSDVOL FindVolume( HDSK hDsk )
// 参数：
// 	IN hDsk - 磁盘对象句柄
// 返回值：
//	假如找到，返回卷结构指针; 否则，返回NULL
// 功能描述：
//	有磁盘句柄得到其关联的卷对象
// 引用: 
// **************************************************

static LPKFSDVOL FindVolume( HDSK hDsk )
{
    LPKFSDVOL lpList = NULL;

	EnterCriticalSection( &csVolumeList );
	
	lpList = lpListVol;
	RETAILMSG(FILE_ZONE, (TEXT("KFSD: ReadDisk Success\r\n")));
	while( lpList )
	{
		if( lpList->hDsk == hDsk )
		{	// 发现
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: the volume exist, serial=%x.\r\n"), lpList->rfh.dwSerialNum ) );
			break;
		}
		lpList = lpList->lpNext;
	}
				
	LeaveCriticalSection( &csVolumeList );
	return lpList;
}

// **************************************************
// 声明：static BOOL RemoveVolume( LPKFSDVOL lpVol )
// 参数：
// 	IN lpVol - 卷结构指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	从系统移出卷
// 引用: 
// **************************************************

static BOOL RemoveVolume( LPKFSDVOL lpVol )
{
    BOOL retv = FALSE;

	EnterCriticalSection( &csVolumeList );  // 进入冲突段

    if( lpListVol )
    {
        LPKFSDVOL lpList = lpListVol;
        if( lpListVol == lpVol )  
        {	// 链表的第一个
            lpListVol = lpListVol->lpNext;
            retv =  TRUE;
        }
        else
        {
            while( lpList->lpNext )
            {
                if( lpList->lpNext == lpVol )
                {   // 发现， 移出
                    lpList->lpNext = lpVol->lpNext;
                    retv =  TRUE;
                    break;
                }
                lpList = lpList->lpNext;
            }
        }
    }

	LeaveCriticalSection( &csVolumeList ); // 离开冲突段

    return retv;
}

// **************************************************
// 声明：static BOOL AddToVolumeList( LPKFSDVOL lpvol )
// 参数：
// 	IN lpvol - 卷结构指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	将新卷加入卷链表
// 引用: 
// **************************************************

static BOOL AddToVolumeList( LPKFSDVOL lpvol )
{
	BOOL bRetv = FALSE;

	EnterCriticalSection( &csVolumeList ); // 进入冲突段

    if( lpvol->lpNext == 0 )
    {	// 加入
        lpvol->lpNext = lpListVol;
        lpListVol = lpvol;
        bRetv = TRUE;
    }

	LeaveCriticalSection( &csVolumeList );// 离开冲突段

    return bRetv;
}

// **************************************************
// 声明：static void CheckFat( LPKFSDVOL lpVol )
// 参数：
// 	IN lpVol - 卷结构指针
// 返回值：
//	无
// 功能描述：
//	调试版本，检查FAT是否有效
// 引用: 
// **************************************************

static void CheckFat( LPKFSDVOL lpVol )
{
	EnterCriticalSection( &lpVol->csFat );// 进入冲突段

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
				RETAILMSG(FILE_ZONE, (TEXT("KFSD: Error Alloc=%d, i=%d, num=%d=====================================\r\n"), dwSector, i, num));
				break;
			}
			dwSector = *(lpdwFat+dwSector);
		}

	}
	LeaveCriticalSection( &lpVol->csFat );// 离开冲突段
}

// **************************************************
// 声明：static BOOL FreeSector( LPKFSDVOL lpVol, DWORD dwStartSector ) 
// 参数：
// 	IN lpVol - 卷结构指针
//	IN dwStartSector - 开始扇区
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	释放扇区
// 引用: 
// **************************************************

static BOOL FreeSector( LPKFSDVOL lpVol, DWORD dwStartSector ) 
{
    DWORD * lpdwFat = lpVol->lpdwFat;
    DWORD n, w, k;

	//CheckFat(lpVol);

    ASSERT( dwStartSector != NULL_SECTOR );
	if( dwStartSector == NULL_SECTOR )
		return TRUE;
	
	EnterCriticalSection( &lpVol->csFat );// 进入冲突段

	// 检查扇区链表，并得到扇区数和最后扇区位置
	k = w = dwStartSector;
    for( n = 0; w != NULL_SECTOR; n++ )
	{
        k = w;
		w = NEXT_SECTOR( lpdwFat, w );		
	}
	// k 需要释放的最后一个扇区索引
	// 连接系统FAT的空闲扇区
    *(lpdwFat+k) = lpVol->rfh.dwFreeSector;
	// 需要更新的FAT数据区
	lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, k );
	lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, k );

    lpVol->rfh.dwFreeSector = dwStartSector;  // 新的空闲扇区索引
    lpVol->rfh.nFreeCount += n; // 新的空闲扇区数
	lpVol->fUpdate = 1;	// 更新标志

	LeaveCriticalSection( &lpVol->csFat );// 离开冲突段

    return TRUE;
}

// **************************************************
// 声明：static DWORD AllocSector( LPKFSDVOL lpVol, DWORD nSectorNum )
// 参数：
// 	IN lpVol - 卷对象指针
//	IN nSectorNum - 需要的扇区数
// 返回值：
//	假如成功，返回非NULL_SECTOR值；否则，返回NULL_SECTOR
// 功能描述：
//	分配一定数量的扇区
// 引用: 
// **************************************************

static DWORD AllocSector( LPKFSDVOL lpVol, DWORD nSectorNum )
{
	DWORD first, n, k;
	DWORD * pfat;
	DWORD end;
	
    EnterCriticalSection( &lpVol->csFat ); // 进入冲突段
	
	end = (lpVol->fdd.nSectors - 1);
		
	RETAILMSG( FILE_ZONE, ( TEXT("KFSD: AllocSector=%d, end=%d, free count=%d\r\n"), nSectorNum, end, lpVol->rfh.nFreeCount ) );
	
	//CheckFat(lpVol);
	
	if( nSectorNum > 0 && lpVol->rfh.nFreeCount >= nSectorNum )
	{   // 系统有足够的扇区
		first = lpVol->rfh.dwFreeSector;
		k = (nSectorNum-1);
		pfat = lpVol->lpdwFat;
		for( n = 0; n < k; n++ )
		{
			first = *(pfat+first);  // get next block
			if( first > end )
			{   // 决不应该到这里
				ERRORMSG(FILE_ZONE, (TEXT("error in KFSD's AllocSector:  Alloc=%d, n=%d\r\n"), first, n));
			}
		}
		n = first;  // n 为分配的最后一个块 end block
		first = *(pfat+first);  // get next block
		*(pfat+n) = NULL_SECTOR;  // 设置块链结束标志
		// 设置FAT更新区间
		lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, n );
		lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, n );
		// 重新设置系统空闲参数 reset free block and count
		n = lpVol->rfh.dwFreeSector;  // n = 返回值
		lpVol->rfh.dwFreeSector = first;
		lpVol->rfh.nFreeCount -= nSectorNum;
		lpVol->fUpdate = 1;
		//		
		LeaveCriticalSection( &lpVol->csFat );// 离开冲突段		
		return n;
	}

	SetLastError(ERROR_DISK_FULL);

	WARNMSG( FILE_ZONE, (TEXT("error in KFSD's AllocSector: AllocSector no enough Sector.\r\n")));
	
	LeaveCriticalSection( &lpVol->csFat );// 离开冲突段
	
    return NULL_SECTOR;
}

// **************************************************
// 声明：static DWORD ReallocSector(
//                          LPKFSDVOL lpVol, 
//                          DWORD dwAllocSector, 
//                          DWORD nSectorNum )
// 参数：
// 	IN lpVol - 卷对象指针
//	IN dwAllocSector - 已分配扇区
//	IN nSectorNum - 新的扇区数
// 返回值：
//	假如成功，返回非NULL_SECTOR值；否则，返回NULL_SECTOR
// 功能描述：
//	改变/重新分配之前已分配扇区的块数
// 引用: 
// **************************************************

static DWORD ReallocSector(
                          LPKFSDVOL lpVol, 
                          DWORD dwAllocSector, 
                          DWORD nSectorNum )
{
    DWORD i;
	DWORD dwRetv = NULL_SECTOR;
	
	//CheckFat(lpVol);

    if( dwAllocSector == NULL_SECTOR )  // 
    {	// 等同于 AllocSector
        return AllocSector( lpVol, nSectorNum );
    }
    else if( nSectorNum == 0 )
    {   // 等同于释放 free all block
        FreeSector( lpVol, dwAllocSector );
        return NULL_SECTOR;
    }
    else
    {
        DWORD s = dwAllocSector;
        DWORD p = dwAllocSector;
        DWORD * pfat;
        i = 0;

		EnterCriticalSection( &lpVol->csFat );// 进入冲突段
		pfat = lpVol->lpdwFat;

        while( s != NULL_SECTOR )
        {
            i++;
            if( i > nSectorNum )
            {  // 减少扇区，释放一些。decrease sector, to free other sector
                DEBUGMSG(FILE_ZONE, (TEXT("ReallocSector: RealocSector decrease\r\n")));
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

        if( i < nSectorNum )  // < or == ?
        {   // 小于，额外分配一些
			s = AllocSector( lpVol, (nSectorNum - i) );
            if( s != NULL_SECTOR )
			{
                *(pfat+p) = s;   // 连接在一起。link them
				// 需要更新的FAT域
                lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, p );
				lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, p );
				lpVol->fUpdate = 1;

				DEBUGMSG(FILE_ZONE, (TEXT("ReallocSector: RealocSector crease success\r\n")));
				dwRetv = dwAllocSector;
				goto _RETV;
			}
			else
			{
				WARNMSG(FILE_ZONE, (TEXT("ReallocSector: RealocSector failure!.\r\n")));
				dwRetv = NULL_SECTOR;
				goto _RETV;
			}
        }
        else if( i == nSectorNum )
		{	// 等于， 不需要做什么
			dwRetv = dwAllocSector;
			goto _RETV;
		}
_RETV:
        LeaveCriticalSection( &lpVol->csFat );// 离开冲突段
    }


    return dwRetv;
}

// **************************************************
// 声明：static DWORD LinkSector( LPKFSDVOL lpVol, DWORD dwDest, DWORD dwSource )
// 参数：
// 	IN lpVol - 卷对象指针
//	IN dwDest - 目标扇区
//	IN dwSource - 源扇区
// 返回值：
//	假如成功，返回非NULL_SECTOR值；否则，返回NULL_SECTOR
// 功能描述：
//	将两个扇区链表连接起来
// 引用: 
// **************************************************

static DWORD LinkSector( LPKFSDVOL lpVol, DWORD dwDest, DWORD dwSource )
{
    DWORD dwSector = dwDest;

	//CheckFat(lpVol);    

	EnterCriticalSection( &lpVol->csFat );// 离开冲突段

	ASSERT( dwDest != NULL_SECTOR && dwSource != NULL_SECTOR );
	// 找到目标尾
    while( *(lpVol->lpdwFat+dwSector) != NULL_SECTOR )
        dwSector = *(lpVol->lpdwFat+dwSector);
	// 连接，dest + source
    *(lpVol->lpdwFat+dwSector) = dwSource;
	// 需要更新的系统数据区
    lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, dwSector );
    lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, dwSector );
	lpVol->fUpdate = 1;

	LeaveCriticalSection( &lpVol->csFat );// 离开冲突段

    return dwDest;
}

// **************************************************
// 声明：static DWORD GetSectorOffset( LPKFSDVOL lpVol, DWORD dwStartSector, UINT nOffset )
// 参数：
// 	IN lpVol - 卷对象指针
//	IN dwStartSector - 开始扇区
//	IN nOffset - 从开始扇区起的偏移数
// 返回值：
//	假如成功，返回非NULL_SECTOR的扇区索引; 否则，返回NULL_SECTOR
// 功能描述：
//	得到从给定的扇区链表起的第nOffset个扇区索引
// 引用: 
// **************************************************

static DWORD GetSectorOffset( LPKFSDVOL lpVol, DWORD dwStartSector, UINT nOffset )
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

// **************************************************
// 声明：static DWORD GetSectorNum( LPKFSDVOL lpVol, DWORD dwStartSector )
// 参数：
// 	IN lpVol - 卷对象指针
//	IN dwStartSector - 开始扇区
// 返回值：
//	扇区数
// 功能描述：
//	得到从开始扇区起的扇区数
// 引用: 
// **************************************************

static DWORD GetSectorNum( LPKFSDVOL lpVol, DWORD dwStartSector )
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

// **************************************************
// 声明：static BOOL ReallocFile( LPKFSDFILE lpFile, DWORD dwSize )
// 参数：
//	IN lpFile - 打开文件对象结构指针
//	IN dwSize - 文件长度
// 返回值：
//	假如成功，返回TRUE; 否则, 返回FALSE
// 功能描述：
//	重新设定文件大小
// 引用: 
// **************************************************

static BOOL ReallocFile( LPKFSDFILE lpFile, DWORD dwSize )
{
    DWORD dwStartSector;
    if( dwSize != GET_NODE_PTR(lpFile)->nFileLength )  // 是否与当前文件的大小相等 ？
    {	// 不相等，重分配
        dwStartSector = ReallocSector( lpFile->lpVol, 
			                          GET_NODE_PTR(lpFile)->dwStartSector, 
									  ( (dwSize + lpFile->lpVol->fdd.nBytesPerSector - 1) / lpFile->lpVol->fdd.nBytesPerSector ) );
		//
        if( dwStartSector != NULL_SECTOR || dwSize == 0 )
        {	// 更新数据
            GET_NODE_PTR(lpFile)->dwStartSector = dwStartSector;
            GET_NODE_PTR(lpFile)->nFileLength = dwSize;
        }
        else
            return FALSE;
    }
    return TRUE;
}

// **************************************************
// 声明：static int GetFolderAdr( LPTSTR * lppFolder, LPCTSTR lpcfn, int * lpNextLen )
// 参数：
// 	OUT lppFolder - 用于接收文件夹名的缓存
//	IN lpcfn - 文件路径名
// 	OUT lpNextLen - 用于接收下一个文件名的相对位置
// 返回值：
// 功能描述：
// 引用: 
// **************************************************

static int GetFolderAdr( LPTSTR * lppFolder, LPCTSTR lpcfn, int * lpNextLen )
{
    int i = 0;
	int fn = 0;
    if( IS_NAME_BREAK(*lpcfn)  )  // 是名字分割符（"\" or "/"）吗？
	{	// 是
        lpcfn++;  ///parent level
		i++;
	}

    while( *lpcfn && fn < MAX_FILE_NAME_LEN  )
    {	//  将所有非分割符拷贝到缓存
        if( !IS_NAME_BREAK(*lpcfn) )
        {
			if( fn == 0 )
	            *lppFolder = (LPTSTR)lpcfn;
			lpcfn++;
            i++; fn++;
        }
        else
            break;
    }
	*lpNextLen = i;  
    return fn;
}


// 文件夹操作码
#define OP_DEL           0x0001    // (OP_DELFILE | OP_DELFOLDER)
#define OP_ADD           0x0004    // add node if not found, the lpFile->node is valid
#define OP_UPDATE        0x0008    // update node, the lpFile->node is valid
#define OP_GET           0x0010    // get node, fill lpFile->node

// **************************************************
// 声明：static BOOL DoFolderInit( 
//						 LPKFSDVOL lpVol , 
//						 DWORD dwFolderSector, 
//						 DWORD dwParentSector, 
//						 WORD * pSector )
// 参数：
//	IN lpVol - 卷对象结构指针
//	IN dwFolderSector - 当前文件夹数据所在扇区
//	IN dwParentSector - 当前文件夹的上一级文件夹数据所在扇区
//	IN/OUT pSector - 一个扇区大小的当前文件夹内存
// 返回值：
//	假如成功，返回TRUE; 否则， 返回FALSE
// 功能描述：
//	初始化新的文件夹数据。将新的文件夹数据写入pSector内存并将其写入磁盘
// 引用: 
// **************************************************

static BOOL DoFolderInit( 
						 LPKFSDVOL lpVol , 
						 DWORD dwFolderSector, 
						 DWORD dwParentSector, 
						 WORD * pSector )
{   
    PNODE pNode = (PNODE)pSector;
	// 清0
    memset( pSector, 0, lpVol->fdd.nBytesPerSector );

	// 每一个文件夹的第一个文件节点表示自己
	// 第二个文件节点表示上级文件夹/父文件夹
    // 初始化自身。init this folder
	pNode->bFlag = NODE_MARK;
    pNode->strName[0] = '.';  // 自身标示
    pNode->dwStartSector = dwFolderSector; // 指向自身开始扇区
	pNode->bNameLen = 1;
	pNode->wAttrib |= FILE_ATTRIBUTE_DIRECTORY;
	pNode->wAttrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

    // 初始化父文件夹 init parent folder
    pNode = (PNODE)( (LPBYTE)pSector + NODE_ALIGN_SIZE );
	pNode->bFlag = NODE_MARK;
    pNode->strName[0] = '.';// 父文件夹标示 ".."
    pNode->strName[1] = '.';// 父文件夹标示
	pNode->bNameLen = 2;
    pNode->dwStartSector = dwParentSector;  // 指向父文件夹开始扇区
	pNode->wAttrib |= FILE_ATTRIBUTE_DIRECTORY;	
	pNode->wAttrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
	// 写入磁盘
    if( ERROR_SUCCESS == FSDMGR_WriteDisk( lpVol->hDsk, dwFolderSector, 1, (LPBYTE)pSector, lpVol->fdd.nBytesPerSector ) )
		return TRUE;
	return FALSE;
}
// **************************************************
// 声明：static BOOL SearchFolder( SF * psf, BOOL bNext, int nStartIndex )
// 参数：
// 	IN psf - SF结构指针
//	IN bNext - 是否从下一个文件节点开始查找/比较（假如为TRUE,则nStartIndex无效）
//	IN nStartIndex - 开始查找/比较的文件节点索引号（假如bNext为FALSE,则nStartIndex有效)
// 返回值：
//	假如找到匹配的文件名，返回TRUE; 否则， 返回FALSE
// 功能描述：
//	查找匹配的文件
// 引用: 
// **************************************************

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
static BOOL SearchFolder( SF * psf, BOOL bNext, int nStartIndex )
{
    LPCTSTR lpcszFileName = psf->lpszCurName; // 需要查找的文件名（可以含有通配符）
    int iFileNameLen = psf->dwCurNameLen; // 查找的文件名长度
	PNODE lpNode;
	UINT uiCurNodeNum = 0;
	UINT uiCurNodeIndex;
	UINT uiSectorOffset;
	DWORD dwCurSector;
	DWORD dwOffset;
	DWORD nBytesPerSector = psf->lpVol->fdd.nBytesPerSector;
	BOOL bUpdate, bRetv = FALSE;
    
    if( bNext )
    {   // 如果向下查找，将当前查找节点索引设为下一个文件节点 
		// search next file match with filename
        uiCurNodeIndex = psf->nCurNodeIndex + psf->nCurNodeNum;
    }
    else
    {	// 从文件夹的开始查找
		uiCurNodeIndex = nStartIndex;
		psf->dwAddNodeSector = NULL_SECTOR;
		psf->nEmptyNodeIndex = -1;
		psf->nDelNodeIndex = -1;
    }
	// uiCurNodeIndex数据在第几个扇区
	uiSectorOffset = uiCurNodeIndex / psf->lpVol->nNodesPerSector; 
	// uiCurNodeIndex数据所在扇区索引
	dwCurSector = GetSectorOffset( psf->lpVol, psf->dwFolderStartSector, uiSectorOffset );
	// uiCurNodeIndex数据在扇区内偏移
    dwOffset = ( (uiCurNodeIndex * NODE_ALIGN_SIZE) % nBytesPerSector );

	bUpdate = TRUE;
	EnterCriticalSection( &psf->lpVol->csNode );
	// 读节点数据/查找
	while( dwCurSector != NULL_SECTOR )
    {                 
        // search node in sector
        if( bUpdate )
		{   // 读取扇区数据到 psf->pSector
			if( FSDMGR_ReadDisk( psf->lpVol->hDsk, dwCurSector, 1, psf->pSector, psf->lpVol->fdd.nBytesPerSector ) != ERROR_SUCCESS )
				break;
		}
		// 每个扇区包含若干索引节点
		lpNode = (PNODE)( (LPBYTE)psf->pSector + dwOffset );
		if( lpNode->bFlag == NODE_EMPTY ||
			lpNode->bFlag == NODE_DELETED )
		{	// 该节点为空或已被删除的
			if( lpNode->bFlag == NODE_EMPTY )
			{	// 空节点，设定psf结构的 无效节点成员数据
				if( psf->dwAddNodeSector == NULL_SECTOR )
				{	
					psf->nEmptyNodeIndex = uiCurNodeIndex;
					psf->dwAddNodeSector = dwCurSector;
					psf->nFreeNodeNum = psf->lpVol->nNodesPerSector - uiCurNodeIndex % psf->lpVol->nNodesPerSector; 
				}
				goto RETV; // 不需要再查找
			}
			else
			{  // // 被删除节点，设定psf结构的 删除节点成员数据 deleted node
				if( psf->dwAddNodeSector == NULL_SECTOR &&
					lpNode->bNameLen >= psf->dwCurNameLen )
				{
					psf->nDelNodeIndex = uiCurNodeIndex;
					psf->dwAddNodeSector = dwCurSector;
					psf->nFreeNodeNum = GET_NODE_NUM( lpNode );
				}				
			}
			// 得到当前文件节点的节点数
			uiCurNodeNum = GET_NODE_NUM( lpNode );
		}
		else if( lpNode->bFlag == NODE_MARK )
		{	// 该文件节点已被使用
			uiCurNodeNum = GET_NODE_NUM( lpNode );// 得到当前文件节点的节点数
			if( uiCurNodeNum > 1 && 
				(uiCurNodeNum * NODE_ALIGN_SIZE + dwOffset) > nBytesPerSector )
			{
				lpNode = psf->pCurNode;
		        if( 0 == ReadSectors( psf->lpVol, dwCurSector, dwOffset, (LPBYTE)lpNode, uiCurNodeNum * NODE_ALIGN_SIZE, psf->pSector, NULL ) )
					goto RETV; // 读错误
			}
			
			if( lpcszFileName[0] == '.' )// 是否需要查找的是 '.' 或 '..'  ？
			{   // 是 '.' or '..' ? 
				if( (iFileNameLen == 1 && uiCurNodeIndex == 0) ||
					(iFileNameLen == 2 && uiCurNodeIndex == 1) )
				{   // yes, find it
					bRetv = TRUE;
				}				    
			}
			else  // 不是，比较
				bRetv = FileNameCompare( lpcszFileName, iFileNameLen, lpNode->strName, lpNode->bNameLen );
			if( bRetv )
			{	// 找到，设置psf结构相关成员
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
			ERRORMSG(FILE_ZONE, (TEXT("error in SearchFolder: find undef node flag(0x%x)!.\r\n"), lpNode->bFlag ));
			goto RETV;
		}
		// 当前节点不符号，准备下一个文件节点。not find match name
        uiCurNodeIndex += uiCurNodeNum;
		{
			UINT t = uiCurNodeIndex / psf->lpVol->nNodesPerSector;
			bUpdate = uiSectorOffset < t; // 下一个节点是否在当前扇区 ？
			for( ; uiSectorOffset < t && dwCurSector != NULL_SECTOR; uiSectorOffset++ )
			{
				dwCurSector =  NEXT_SECTOR( psf->lpVol->lpdwFat, dwCurSector );
			}
		}

        dwOffset = GET_NODE_OFFSET( uiCurNodeIndex, psf->lpVol );
	}

RETV:
	LeaveCriticalSection( &psf->lpVol->csNode );  // 离开冲突段
    
    return bRetv;
}

// **************************************************
// 声明：static DWORD NewFolder( LPKFSDVOL lpVol, DWORD dwParent, void * lpSectorBuf )
// 参数：
// 	IN lpVol - 卷对象数据结构
//	IN dwParent - 上级文件夹
//	IN lpSectorBuf - 用于读取扇区数据的缓存
// 返回值：
//	假如成功，返回新文件夹的扇区索引号；失败，返回NULL_SECTOR
// 功能描述：
//	创建新的文件夹
// 引用: 
// **************************************************

static DWORD NewFolder( LPKFSDVOL lpVol, DWORD dwParent, void * lpSectorBuf )
{
	DWORD dwSector;

    dwSector = AllocSector( lpVol, 1 ); // 为新文件夹分配扇区
	if( dwSector != NULL_SECTOR )
	{	// 分配成功，初始化新文件夹数据
		if( DoFolderInit( lpVol , dwSector, dwParent, lpSectorBuf ) )
			return dwSector;
		else
		{	// 失败
			FreeSector( lpVol, dwSector );
		}
	}
	return NULL_SECTOR;
}

// **************************************************
// 声明：static DWORD AddNodeSector( PSF psf, DWORD dwNeedSectors )
// 参数：
// 	IN psf - 查找结构指针
// 	IN dwNeedSectors - 需要的扇区数
// 返回值：
//	假如成功，返回非 NULL_SECTOR值；否则，返回扇区值
// 功能描述：
//	为新的节点增加存储扇区
// 引用: 
// **************************************************

static DWORD AddNodeSector( PSF psf, DWORD dwNeedSectors )
{
    DWORD dwNodeSector;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd, No Empty Node\r\n")));
	dwNodeSector = AllocSector( psf->lpVol, dwNeedSectors );
	if( dwNodeSector != NULL_SECTOR )
	{	// 分配成功
		DWORD dwInitSector;

		memset( psf->pSector, 0, psf->lpVol->fdd.nBytesPerSector );
		
		EnterCriticalSection( &psf->lpVol->csNode );  // 进入冲突段
		// 连接到当前文件夹	 
        LinkSector( psf->lpVol, psf->dwFolderStartSector, dwNodeSector );
        // init it
		dwInitSector = dwNodeSector;		
		// 将初始化数据写入磁盘
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

		LeaveCriticalSection( &psf->lpVol->csNode ); // 离开冲突段
	}
	return dwNodeSector;
}

// **************************************************
// 声明：static BOOL DoNodeAdd( PSF psf, PNODE pNode )
// 参数：
// 	IN psf - 查找结构指针
//	IN pNode - 文件节点
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在当前文件夹里增加一个文件节点
// 引用: 
// **************************************************

static BOOL DoNodeAdd( PSF psf, PNODE pNode )
{
	DWORD dwOffset;
	DWORD dwNodeSector;
	UINT uiNodeIndex;
	DWORD dwNeedSectors;
    DWORD dwNeedNodeNum;
	UINT nBytesPerSector = psf->lpVol->fdd.nBytesPerSector;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd.\r\n")));
	
    EnterCriticalSection( &psf->lpVol->csNode );  // 进入冲突段

	if( (dwNodeSector = psf->dwAddNodeSector) == NULL_SECTOR )
	{   // 当前文件夹没有空闲节点，增加。。alloc enough sector to save the node
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd, No Empty Node\r\n")));
	    dwNeedSectors = ( GET_NODE_SIZE( pNode ) + nBytesPerSector - 1 ) / nBytesPerSector;
		// 为文件夹分配新的磁盘扇区
		dwNodeSector = AddNodeSector( psf, dwNeedSectors );
		
		if( dwNodeSector != NULL_SECTOR )
		{	//成功
			dwOffset = 0;			 
			uiNodeIndex = GetSectorNum( psf->lpVol, psf->dwFolderStartSector ) * psf->lpVol->nNodesPerSector;
		}
		else
		{	// 失败
			LeaveCriticalSection( &psf->lpVol->csNode );// 离开冲突段
			return FALSE;
		}
	}
	else
	{	// 有空闲节点
		dwNeedNodeNum = GET_NODE_NUM( pNode ); // 得到新文件节点所需节点数
        if( psf->nDelNodeIndex != -1 )
		{	// 空闲节点是删除节点
			ASSERT( dwNeedNodeNum <= psf->nFreeNodeNum );
			if( dwNeedNodeNum < psf->nFreeNodeNum )
			{	// 节点数足够并且比需要的多，将余下节点的分离出去
				NODE node;
				DWORD dwFreeSector;
				memset( &node, 0, sizeof( NODE ) );
				// 为余下节点准备数据
				node.bFlag = NODE_DELETED;
				node.dwStartSector = NULL_SECTOR;
				node.bNameLen = (BYTE)( (psf->nFreeNodeNum - dwNeedNodeNum) * NODE_ALIGN_SIZE - sizeof( NODE ) + NODE_RESERVE_NAME_LEN ); 
				// 得到余下节点索引所在扇区。 get node index and offset to split 
	            dwFreeSector = GetSectorOffset( psf->lpVol, 
					                            psf->dwFolderStartSector, 
											    (psf->nDelNodeIndex + dwNeedNodeNum) / psf->lpVol->nNodesPerSector );
				// 得到余下节点索引所在扇区内的偏移
                dwOffset = GET_NODE_OFFSET( (psf->nDelNodeIndex + dwNeedNodeNum), psf->lpVol );
				ASSERT( dwFreeSector != NULL_SECTOR );
				// 写入磁盘
				if( 0 == WriteSectors( psf->lpVol, dwFreeSector, dwOffset, (LPBYTE)&node, NODE_ALIGN_SIZE - NODE_RESERVE_NAME_LEN, psf->pSector ) )
				{	// 失败
					LeaveCriticalSection( &psf->lpVol->csNode );// 离开冲突段
	                return FALSE;//
				}
			}
		    // set node index and offset to added
			// 新的文件节点的索引号 和 偏移
			uiNodeIndex = psf->nDelNodeIndex;
			dwOffset = GET_NODE_OFFSET( psf->nDelNodeIndex, psf->lpVol );
		}
		else
		{   // 空索引节点 has empty index
            ASSERT( psf->nEmptyNodeIndex != -1 );
			if( psf->nFreeNodeNum < dwNeedNodeNum )
			{	// 足够 add node sector
				dwNeedSectors = ( (dwNeedNodeNum - psf->nFreeNodeNum) * NODE_ALIGN_SIZE + nBytesPerSector - 1 ) / nBytesPerSector;
				if( AddNodeSector( psf, dwNeedSectors ) == NULL_SECTOR )
				{	// 失败
					LeaveCriticalSection( &psf->lpVol->csNode );// 离开冲突段
	                return FALSE;// no sector
				}
			}
			// 新的文件节点的索引号 和 偏移
	        uiNodeIndex = psf->nEmptyNodeIndex;
			dwOffset = GET_NODE_OFFSET( psf->nEmptyNodeIndex, psf->lpVol );
		}
	}

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd, Sector=%d, Index=%d\r\n"), psf->dwAddNodeSector, psf->nEmptyNodeIndex));

	pNode->bFlag = NODE_MARK; // 设置为占用标志
	// 写入磁盘
	if( ( dwOffset = WriteSectors( psf->lpVol, dwNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), psf->pSector ) ) )
	{
		psf->nCurNodeIndex = uiNodeIndex;
		psf->dwCurNodeSector = dwNodeSector;
		memcpy( psf->pCurNode, pNode, GET_NODE_SIZE(pNode) );
	}

	LeaveCriticalSection( &psf->lpVol->csNode );// 离开冲突段
		
	return dwOffset != 0;
}

// **************************************************
// 声明：static BOOL DoNodeUpdate( PSF psf, PNODE pNode )
// 参数：
// 	IN psf - 查找结构指针
//	IN pNode - 文件节点
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	更新当前文件夹里一个文件节点
// 引用: 
// **************************************************

static BOOL DoNodeUpdate( PSF psf, PNODE pNode )
{   
	BOOL bRetv;
	DWORD dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeUpdate.\r\n")));

	EnterCriticalSection( &psf->lpVol->csNode );  // 进入冲突段
	// 写入磁盘
	bRetv = WriteSectors( psf->lpVol, psf->dwCurNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), psf->pSector );

	LeaveCriticalSection( &psf->lpVol->csNode ); // 离开冲突段

    if( bRetv && psf->pCurNode != pNode )
	    memcpy( psf->pCurNode, pNode, GET_NODE_SIZE( pNode ) ); // 更新psf成员
    return bRetv;
    
}

// **************************************************
// 声明：static BOOL DoNodeGet( PSF psf, PNODE pNode )
// 参数：
// 	IN psf - 查找结构指针
//	IN pNode - 文件节点
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到当前文件夹里一个文件节点的数据
// 引用: 
// **************************************************

static BOOL DoNodeGet( PSF psf, PNODE pNode )
{
	DWORD dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeGet\r\n")));
	
	memcpy( pNode, psf->pCurNode, GET_NODE_SIZE( psf->pCurNode ) );
	return TRUE;
}

// **************************************************
// 声明：static BOOL DoNodeDel( PSF psf, BOOL bFreeSector )
// 参数：
// 	IN psf - 查找结构指针
//	IN bFreeSector - 需要释放扇区
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除文件节点
// 引用: 
// **************************************************

static BOOL DoNodeDel( PSF psf, BOOL bFreeSector )
{
    DWORD dwOffset; 
	BOOL bRetv;

	EnterCriticalSection( &psf->lpVol->csNode ); // 进入冲突段
    
	if( bFreeSector )
	{	// 释放扇区
		if( psf->pCurNode->nFileLength ||
			(psf->pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) )
        {
            ASSERT( psf->pCurNode->dwStartSector != NULL_SECTOR );
            if( psf->pCurNode->dwStartSector != NULL_SECTOR )
                FreeSector( psf->lpVol, psf->pCurNode->dwStartSector ); // 释放文件节点的数据扇区
        }
        else
        {
            ;//ASSERT( psf->pCurNode->wStartSector == NULL_SECTOR );
        }
	}

	psf->pCurNode->bFlag = NODE_DELETED;  // 设置删除标志
	dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );
	// 将数据写到磁盘
	bRetv = WriteSectors( psf->lpVol, psf->dwCurNodeSector, dwOffset, (LPBYTE)psf->pCurNode, GET_NODE_SIZE( psf->pCurNode ), psf->pSector );
	
	LeaveCriticalSection( &psf->lpVol->csNode ); // 离开冲突段
	return bRetv;
}

// **************************************************
// 声明：static BOOL DoFolderChange( 
//                       SF * psf,
//					     PNODE pNode,
//                       int op )
// 参数：
// 	IN psf - 查找结构指针
//	IN pNode - 文件节点结构
//	IN op - 操作码
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	对文件夹做相关操作
// 引用: 
// **************************************************

static BOOL DoFolderChange( 
                       SF * psf,
					   PNODE pNode,
                       int op )
{
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoFolderChange.\r\n")));
	switch( op )
	{
	case OP_DEL:  // 删除文件夹
		{
			SF sfChild;
			if( BeginSearch( &sfChild, psf->lpVol, psf->hProc, NULL ) )  // 初始化搜索结构数据
			{	// 搜索当前文件夹里是否为空（有别的文件节点？)
				sfChild.lpszCurName = "*";
				sfChild.dwCurNameLen = 1;
				sfChild.dwFolderStartSector = psf->pCurNode->dwStartSector;
				op = SearchFolder( &sfChild, FALSE, USER_NODES ); // 搜索
				EndSearch( &sfChild );
				if( op )  // 有别的文件节点 ？
				{	// 有，非空
					SetLastError( ERROR_DIR_NOT_EMPTY );
					return FALSE;
				}
				else
					return DoNodeDel( psf, TRUE );// 2004-02-03
			}
			// 2004-02-03
			//  return DoNodeDel( psf, TRUE );
			// 
			return FALSE;
		}
		break;
	case OP_ADD:  // 增加文件夹
		// 分配新的文件夹数据结构
		pNode->dwStartSector = NewFolder( psf->lpVol, psf->dwFolderStartSector, psf->pSector );
		if( pNode->dwStartSector != NULL_SECTOR )
			return DoNodeAdd( psf, pNode ); // 写入磁盘节点
		else
			return FALSE;
	}
	return FALSE;
}

// **************************************************
// 声明：static BOOL SearchNode( SF * lpsf )
// 参数：
// 	IN lpsf - 查找结构指针
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	查找文件节点
// 引用: 
// **************************************************

static BOOL SearchNode( 
					   SF * lpsf )
	
{
    int len;
	LPTSTR lpszName;
	BOOL bRetv = FALSE;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: SearchNode From Folder=%d.\r\n"), lpsf->lpVol->rfh.dwDataStartSector));

	EnterCriticalSection( &lpsf->lpVol->csNode );

	lpsf->dwFolderStartSector = lpsf->lpVol->rfh.dwDataStartSector;
	lpszName = lpsf->lpszName;  // 文件路径名
	lpsf->dwCurNameLen = 0; //
	lpsf->lpszCurName = NULL;
	
	while( 1 )
	{
		int iNextLen;
		LPTSTR lpszCurName;
		// 从lpszName得到其文件/路径名
		// example : "\\kingmos\\folder1\\folder2\\folder3\\filename.txt" 
		//			     1filename  2fn     3fn      4fn      5fn  
		// lpszCurName指向当前得到的文件名（不含'\'），iNextLen为下一个文件名在lpszName的位置
		len = GetFolderAdr( &lpszCurName, lpszName, &iNextLen );
		if( len )  // 得到吗 ？
		{	//是，准备查找结构
			lpsf->nCurNodeIndex = 0;
			lpsf->lpszCurName = lpszCurName; // 需要查找的文件节点名
			lpsf->dwCurNameLen = len;  // // 需要查找的文件节点名长度
			lpszName += iNextLen;  // 指向下一个名
			
			if( SearchFolder( lpsf, FALSE, ALL_NODES ) )  // 查找
			{	 // 发现它 find node
				if( *lpszName )  // 还有文件夹吗 ？
				{   // 是，如果当前查找到的是文件夹，则继续 the node must is folder
					if( lpsf->pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY ) // the node is folder
					{	// 文件夹
						lpsf->dwFolderStartSector = lpsf->pCurNode->dwStartSector;
						continue;   // continue search in sub folder
					}
					else
					{   // 非文件夹 not folder
						DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Node is'nt folder=%s.\r\n"), lpsf->lpszCurName));
					}
				}
				else
				{	// 找到
					DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Find Node=%s.\r\n"), lpsf->pCurNode->strName));					
					bRetv = TRUE;
					break;
				}
			}
			else
			{   // 没有找到
				DEBUGMSG(FILE_ZONE, (TEXT("KFSD: not find folder=%s.\r\n"), lpsf->lpszCurName ));
			}
		}
		break;
	}
	
	LeaveCriticalSection( &lpsf->lpVol->csNode );  // 离开冲突段

	if( bRetv )
		SetLastError(ERROR_FILE_EXISTS);
	else
	    SetLastError(ERROR_FILE_NOT_FOUND);
	return bRetv;
}

// **************************************************
// 声明：static BOOL BeginSearch( SF * lpsf, LPKFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName )
// 参数：
// 	IN lpsf - 查找结构指针
//	IN lpVol - 卷对象结构指针
//	IN hProc - 拥有者进程句柄
//	IN lpszName - 需要查找的文件路径名
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化一个查找对象结构
// 引用: 
// **************************************************

static BOOL BeginSearch( SF * lpsf, LPKFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName )
{	// 为读写磁盘扇区准备缓存
	lpsf->pSector = malloc( MAX_NODE_BYTES + lpVol->fdd.nBytesPerSector );
	if( lpsf->pSector )
	{	// 初始化查找对象结构
		lpsf->pCurNode = (PNODE)( ((LPBYTE)lpsf->pSector) + lpVol->fdd.nBytesPerSector );//MAX_NODE_BYTES;
		lpsf->lpVol = lpVol;
		lpsf->hProc = hProc;
		lpsf->lpszName = lpszName;
		memset( lpsf->pCurNode, 0, MAX_NODE_BYTES );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：static void EndSearch( SF * lpsf )
// 参数：
// 	IN lpsf - 查找结构指针
// 返回值：
//	无
// 功能描述：
//	与BeginSearch相反，释放搜索数据
// 引用: 
// **************************************************

static void EndSearch( SF * lpsf )
{
	if( lpsf->pSector )
		free( lpsf->pSector );
}

// **************************************************
// 声明：static DWORD ReadSectors( 
//							LPKFSDVOL lpVol, 
//							DWORD dwStartSector, 
//							DWORD nOffset, 
//							LPBYTE lpBuf, 
//							DWORD dwSize,
//							LPBYTE pSector,
//							UINT * lpCacheValid )
// 参数：
//		IN lpVol - 卷对象结构指针
//		IN dwStartSector - 开始扇区
//		IN nOffset - 在扇区内偏移字节
//		IN lpBuf - 用于接受数据的缓存
//		IN dwSize - 需要接受的数据
//		IN pSector - 用于读取扇区数据的临时缓存
//		IN lpCacheValid - 用于接收整个扇区数据是否有效
// 返回值：
//		读取的数据大小
// 功能描述：
//		读取扇区的数据，将实际需要的数据写入lpBuf,并返回实际读取的数据大小
// 引用: 
//		
// **************************************************

static DWORD ReadSectors( LPKFSDVOL lpVol, DWORD dwStartSector, 
						  DWORD nOffset, LPBYTE lpBuf, DWORD dwSize,
						  LPBYTE pSector,
						  UINT * lpCacheValid )
{
    DWORD nSectorSize = lpVol->fdd.nBytesPerSector;
    DWORD s = dwSize;
	BOOL bCache = FALSE;

	//RETAILMSG(FILE_ZONE, (TEXT("KFSD: ReadSector, Sector=%d, nOffset=%d, Size=%d\r\n"), wStartSector, nOffset, dwSize));
	ASSERT_NOTIFY( dwStartSector != NULL_SECTOR, "no valid sector number\r\n" );

    if( dwStartSector != NULL_SECTOR )
    {
        if( dwSize >= nSectorSize && nOffset == 0 )
        {	// 该扇区整个为需要的有效数据
            if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, lpBuf, nSectorSize ) )
				goto _error_ret;

            dwSize -= nSectorSize;
            lpBuf += nSectorSize;
        }
        else
        {	// 该扇区局部为需要的有效数据
            if(  ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
				goto _error_ret;
            if( nOffset + dwSize >= nSectorSize )
            {	// 数据跨扇区，拷贝从nOffset开始的余下扇区数据
                memcpy( lpBuf, pSector + nOffset, nSectorSize - nOffset );
                dwSize -=  nSectorSize - nOffset;
                lpBuf += nSectorSize - nOffset;
            }
            else
            {	// 数据在扇区中间，拷贝需要部分
                memcpy( lpBuf, pSector + nOffset, dwSize );
                dwSize = 0;
            }
			bCache = TRUE; // CACHE 有效
        }
		// 如果还有，读取余下的数据.
        while( dwSize )
        {
            dwStartSector = NEXT_SECTOR( lpVol->lpdwFat, dwStartSector );
            if( dwStartSector != NULL_SECTOR )
            {
                if( dwSize >= nSectorSize )
                {	// 余下的数据不小于一个扇区大小读取整个扇区
                    if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, lpBuf, nSectorSize ) )
						goto _error_ret;
                    dwSize -= nSectorSize;
                    lpBuf += nSectorSize;
					bCache = FALSE;
                }
                else
                {	// 余下的数据小于一个扇区大小读取扇区局部
                    if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
						goto _error_ret;
                    memcpy( lpBuf, pSector, dwSize );
                    dwSize = 0;
					bCache = TRUE;
                }
            }
            else
                break;
        }
    }
	if( lpCacheValid )
		*lpCacheValid = bCache;
    return s - dwSize;// 实际读的数据
_error_ret:
	return 0;
}

// **************************************************
// 声明：static DWORD WriteSectors( 
//							LPKFSDVOL lpVol, 
//							DWORD dwStartSector, 
//							DWORD nOffset, 
//							const unsigned char * lpBuf, 
//							DWORD dwSize,
//							LPBYTE pSector
//						   )
// 参数：
//		IN lpVol - 卷对象结构指针
//		IN dwStartSector - 开始扇区
//		IN nOffset - 在扇区内偏移字节
//		IN lpBuf - 用于写入数据的缓存
//		IN dwSize - 需要写入的数据
//		IN pSector - 用于读取扇区数据的临时缓存
// 返回值：
//		写入的数据大小
// 功能描述：
//		写入扇区数据，并返回实际写入的数据大小
// 引用: 
// **************************************************

static DWORD WriteSectors( LPKFSDVOL lpVol, 
						   DWORD dwStartSector, 
						   DWORD nOffset, 
						   const unsigned char * lpBuf, 
						   DWORD dwSize,
						   LPBYTE pSector // temp buf
 						   )
{
    DWORD nSectorSize = lpVol->fdd.nBytesPerSector;
    DWORD s = dwSize;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: WriteSector, Sector=%d, nOffset=%d, Size=%d.\r\n"), dwStartSector, nOffset, dwSize));

    if( dwStartSector != NULL_SECTOR )
    {
        if( dwSize >= nSectorSize && nOffset == 0 )
        {	// 首扇区整个为需要更新的有效数据
            if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)lpBuf, nSectorSize ) )
				goto _error_ret;  // 失败

            dwSize -= nSectorSize;
            lpBuf += nSectorSize;
        }
        else
        {	// 首扇扇区局部为需要更新的有效数据，执行读取-更新-写入
            if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)pSector, nSectorSize ) )
				goto _error_ret;
            if( nOffset + dwSize >= nSectorSize )
            {// 数据跨扇区，拷贝从nOffset开始的余下需要的扇区数据
                memcpy( pSector + nOffset, lpBuf, nSectorSize - nOffset );
                dwSize -=  nSectorSize - nOffset;
                lpBuf += nSectorSize - nOffset;
            }
            else
            {// 数据在扇区中间，拷贝需要部分
                memcpy( pSector + nOffset, lpBuf, dwSize );
                dwSize = 0;
            }
            if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
				goto _error_ret;  //写入磁盘失败
        }
		// 写入余下的部分
        while( dwSize )
        {
            dwStartSector = NEXT_SECTOR( lpVol->lpdwFat, dwStartSector );
            if( dwStartSector != NULL_SECTOR )
            {
                if( dwSize >= nSectorSize )
                {// 整个扇区数据需要更新
                    if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)lpBuf, nSectorSize ) )
				        goto _error_ret;
                    dwSize -= nSectorSize;
                    lpBuf += nSectorSize;
                }
                else
                {// 局部扇区数据需要更新，执行读取-更新-写入
                    if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)pSector, nSectorSize ) )
				        goto _error_ret;// 读错误
                    memcpy( pSector, lpBuf, dwSize );
                    dwSize = 0;					
                    if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
				        goto _error_ret;// 写错误
                }
            }
            else
                break;
        }
    }

    return s - dwSize;  // 实际写入的数据
_error_ret:
	return 0;
}

// **************************************************
// 声明：static DWORD GetSerialNum(void)
// 参数：
//		无
// 返回值：
//		返回卷序列号
// 功能描述：
//		产生卷序列号
// 引用: 
// **************************************************

static DWORD GetSerialNum(void)
{
	return random();   
}

// **************************************************
// 声明：static BOOL DoVolumeFormat( LPKFSDVOL lpVol )
// 参数：
// 	IN lpVol - 卷对象
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	格式化磁盘
// 引用: 
// **************************************************

static BOOL DoVolumeFormat( LPKFSDVOL lpVol )
{
	DWORD i, n;
	DWORD dwfat;
	void * pSector;
	PRFH prfh;
	DWORD hDsk;
	BOOL bRetv = FALSE;
	
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Do Format Start\r\n")));

	EnterCriticalSection( &lpVol->csFat ); // 进入冲突段
	pSector = lpVol->lpSectorBuf;

	if( pSector )
	{
		memset( pSector, 0, lpVol->fdd.nBytesPerSector );
		hDsk = lpVol->hDsk;
		// 初始化 prfh 结构
		prfh = (PRFH)pSector;
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

		// 初始化FAT表所占的索引 build fat table
		i += 1;
		for( n = 0; n < i; n++ )
			*(lpVol->lpdwFat+n) = NULL_SECTOR;
		// 初始化空闲的FAT
		n = lpVol->fdd.nSectors-1;
		for( ; i < n; i++ )
			*(lpVol->lpdwFat+i) = (i + 1);
		*(lpVol->lpdwFat+i) = NULL_SECTOR;

		// 需要更新的FAT扇区
		lpVol->dwMinNeedUpdate = 0;
		lpVol->dwMaxNeedUpdate = i;	
	
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Build FAT....... \r\n")));
		// 拷贝卷数据到卷结构成员 rfh
		memcpy( &lpVol->rfh, prfh, sizeof( RAM_FILE_HEAD ) );
		// 更新成员
		if( FSDMGR_WriteDisk(hDsk,0,1,(LPBYTE)prfh,lpVol->fdd.nBytesPerSector) != ERROR_SUCCESS )
		{
			goto _error_ret;
		}
	
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Init System Folder....... \r\n")));
		// 初始化根节点
		n = AllocSector( lpVol, 1 );
		DoFolderInit( lpVol, n, n, pSector );	
		// 将根节点数据写入磁盘
		if( FSDMGR_WriteDisk( hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (LPBYTE)lpVol->lpdwFat, lpVol->fdd.nBytesPerSector * lpVol->rfh.dwFatSectorNum ) != ERROR_SUCCESS )
		{
			goto _error_ret;
		}

		lpVol->fUpdate = 1;

		bRetv = TRUE;

	}

_error_ret:

	LeaveCriticalSection( &lpVol->csFat );  // 离开冲突段

//	if( pBuf == 0 && pSector )
//		free( pSector );
	return bRetv;
}

// **************************************************
// 声明：static BOOL DoShellNotify( 
//						  LPKFSDVOL lpVol, 
//						  DWORD dwEvent, 
//						  DWORD dwFlags, 
//						  LPCTSTR lpcszItem1, 
//						  LPCTSTR lpcszItem2,
//						  PNODE pNode )
// 参数：
//	IN lpVol - 卷对象结构指针
//	IN dwEvent - 事件
//	IN dwFlags - 额外的标志
//	IN lpcszItem1 - 项目一
//	IN lpcszItem2 - 项目二
//	IN pNode - 当前修改节点
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	假如文件系统改变，向系统做提示信息
// 引用: 
// **************************************************

static BOOL DoShellNotify( 
						  LPKFSDVOL lpVol, 
						  DWORD dwEvent, 
						  DWORD dwFlags, 
						  LPCTSTR lpcszItem1, 
						  LPCTSTR lpcszItem2,
						  PNODE pNode )
{
	FILECHANGEINFO fci;
	SYSTEMTIME st;
	BOOL bRetv = FALSE;
	TCHAR * path1 = lpVol->szNotifyPath0;
	TCHAR * path2 = lpVol->szNotifyPath1;

	if( lpVol->pscf )  // 有 notify 回调函数吗 ？
	{	// 有

		// 初始化项目信息
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
		// 准备 fci 结构
		memset( &fci, 0, sizeof( FILECHANGEINFO ) );
		fci.cbSize = sizeof( FILECHANGEINFO );
		fci.wEventId = dwEvent;
		fci.uFlags = dwFlags;
		fci.dwItem1 = (DWORD)path1;
		fci.dwItem2 = (DWORD)path2;
		fci.dwAttributes = pNode->wAttrib;
		fci.nFileSize = pNode->nFileLength;
		
		GetSystemTime( &st );  //Ok
		SystemTimeToFileTime( &st, &fci.ftModified );	
		bRetv = lpVol->pscf(&fci);  // 调用系统的回调函数

		if( lpcszItem1 )
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: Shell Notify Item1=%s\r\n"), path1));
		if( lpcszItem2 )
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: Shell Notify Item2=%s\r\n"), path2));
	}
	return bRetv;	
}

// **************************************************
// 声明：static BOOL DoUpdateVolume( LPKFSDVOL lpVol )
// 参数：
//	IN lpVol - 卷对象结构指针
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	向磁盘更新卷数据
// 引用: 
// **************************************************

static BOOL DoUpdateVolume( LPKFSDVOL lpVol )
{
	DWORD dwRetv = !ERROR_SUCCESS;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Update Volume=%d.\r\n"), lpVol->fUpdate));

    EnterCriticalSection( &lpVol->csFat );  // 进入冲突段

	if( lpVol->fUpdate )
	{
		void * pSector;
		DWORD bytes = lpVol->fdd.nBytesPerSector;
		DWORD dwStart, dwEnd;

		pSector = lpVol->lpSectorBuf;

		if( pSector )
		{
			memset( pSector, 0, bytes );
			memcpy( pSector, &lpVol->rfh, sizeof( RAM_FILE_HEAD ) );
			if( ERROR_SUCCESS == FSDMGR_WriteDisk( lpVol->hDsk, 0, 1, pSector, bytes ) )
			{	// 更新卷头成功 update fat		
			    dwStart = (lpVol->dwMinNeedUpdate *  FATBYTES) / lpVol->fdd.nBytesPerSector;
			    dwEnd = (lpVol->dwMaxNeedUpdate *  FATBYTES) / lpVol->fdd.nBytesPerSector;
				// 更新FAT
				dwRetv = FSDMGR_WriteDisk( lpVol->hDsk, 
					                       lpVol->rfh.dwFatStartSector + dwStart, 
										   dwEnd - dwStart + 1, 
										   (LPBYTE)lpVol->lpdwFat + dwStart * lpVol->fdd.nBytesPerSector,
										   (dwEnd - dwStart + 1) * lpVol->fdd.nBytesPerSector );
				if( dwRetv == ERROR_SUCCESS )
				{
				    lpVol->dwMaxNeedUpdate = 0;
				    lpVol->dwMinNeedUpdate = -1;
				}
			}
			//free( pSector );
			lpVol->fUpdate = 0;
		}
		else 
		{
			ERRORMSG(FILE_ZONE, (TEXT("KFSD: Update Volume Failure\r\n")));
		}
	}
	else
		dwRetv = ERROR_SUCCESS;

	LeaveCriticalSection( &lpVol->csFat );  //离开冲突段

	return (dwRetv == ERROR_SUCCESS);
}

// **************************************************
// 声明：static LPKFSDFILE FindOpened( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
// 参数：
// 	IN lpVol - 卷对象结构指针
//	IN lpcszFileName - 文件名
// 返回值：
//	假如系统已经打开该文件，则返回其节点指针；否则，返回NULL
// 功能描述：
//	发现系统是否有已经打开的文件句柄
// 引用: 
// **************************************************

static LPKFSDFILE FindOpened( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
{	

	LPKFSDFILE lpFile;

	EnterCriticalSection( &lpVol->csFileList );
	lpFile = lpVol->lpOpenFile;

	while( lpFile )
	{
		if( stricmp( lpFile->lpfn->lpszFileName, lpcszFileName ) == 0 )
			break;  // 发现
		lpFile = lpFile->lpNext;  // 下一个文件节点
	}
	LeaveCriticalSection( &lpVol->csFileList );
	return lpFile;
}

// **************************************************
// 声明：static LPKFSDFIND FindOpenedSearchHandle( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
// 参数：
// 	IN lpVol - 卷对象结构指针
//	IN lpcszFileName - 文件名
// 返回值：
//	假如系统已经有正在查找该文件的句柄，则返回其查找指针；否则，返回NULL
// 功能描述：
//	发现系统是否有已经有正在查找的指针
// 引用: 
// **************************************************

static LPKFSDFIND FindOpenedSearchHandle( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
{
	LPKFSDFIND lpFile;

	EnterCriticalSection( &lpVol->csFindList );

	lpFile = lpVol->lpFindFile;

	while( lpFile )
	{
		if( stricmp( lpFile->lpszName, lpcszFileName ) == 0 )
			break;  //找到
		lpFile = lpFile->lpNext;
	}

	LeaveCriticalSection( &lpVol->csFindList );

	return lpFile;
}


// **************************************************
// 声明：static HANDLE CreateShareFile(
//							  LPKFSDFILE lpOldFile,
//							  PVOL pVol, 
//							  HANDLE hProc, 
//							  LPCTSTR lpcszFileName, 
//							  DWORD dwAccess, 
//							  DWORD dwShareMode,
//							  PSECURITY_ATTRIBUTES pSecurityAttributes, 
//							  DWORD dwCreate,
//							  DWORD dwFlagsAndAttributes, 
//							  HANDLE hTemplateFile ) 
// 参数：
//	IN lpOldFile - 之前已经打开的文件节点指针
//	IN pVol - 卷对象
//	IN hProc - 拥有者进程句柄
//	IN lpcszFileName - 需要打开的文件名
//	IN dwAccess - 
//	IN dwShareMode
//	IN pSecurityAttributes
//	IN dwCreate
//	IN dwFlagsAndAttributes
//	IN hTemplateFile
// 返回值：
//	假如成功，返回文件对象句柄，否则返回NULL
// 功能描述：
//	创建文件对象
// 引用: 
// **************************************************

static HANDLE CreateShareFile(
							  LPKFSDFILE lpOldFile,
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
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
	LPKFSDFILE lpFile;
	int op;

	op = 0;
	if( lpOldFile->dwShareMode && lpOldFile->dwShareMode == dwShareMode )
	{	// 检查共享冲突
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
		if( op )  // 有共享冲突吗 ？
		{	// 没有，分配文件打开节点对象
			lpFile = (LPKFSDFILE)malloc( sizeof( KFSDFILE ) + lpVol->fdd.nBytesPerSector );
			if( lpFile )
			{
				// 初始化数据成员 init lpFile member
				memset( lpFile, 0, sizeof( KFSDFILE ) );
				lpFile->lpVol = lpVol;
				lpFile->hProc = hProc;
				lpFile->lpbCacheBuf = (LPBYTE)(lpFile+1);

				lpFile->dwAccess = dwAccess;
				lpFile->dwShareMode = dwShareMode;
				lpFile->dwCreate =dwCreate;
				lpFile->dwFlagsAndAttributes = dwFlagsAndAttributes;

				lpFile->flag = lpOldFile->flag;
				lpFile->dwFilePointer = 0;
				// 增加对节点的引用
				InterlockedIncrement( (LPLONG)&lpOldFile->lpfn->dwRefCount );
				lpFile->lpfn = lpOldFile->lpfn;
				
				// 增加新文件结构到卷的打开文件链表。add to volume list
				AddFileHandleToVolume( lpFile );
				return lpFile;
			}
		}
	}
	SetLastError(ERROR_SHARING_VIOLATION);
	return INVALID_HANDLE_VALUE;
}

// **************************************************
// 声明：static HANDLE CreateVolumeFile(
//							  LPKFSDVOL lpVol, 
//							  HANDLE hProc,
//							  LPCTSTR lpcszFileName ) 
// 参数：
//	IN lpVol - 卷对象
//	IN hProc - 拥有者进程句柄
//	IN lpcszFileName - 需要打开的卷文件名
// 返回值：
//	假如成功，返回卷文件对象句柄，否则返回NULL
// 功能描述：
//	创建卷文件对象，卷文件对象不同于卷对象；卷文件对象是用户可访问的文件对象；
//	卷对象是对磁盘卷的描述对象
// 引用: 
// **************************************************

static HANDLE CreateVolumeFile(
							  LPKFSDVOL lpVol, 
							  HANDLE hProc,
							  LPCTSTR lpcszFileName ) 
{
	LPKFSDFILE lpFile;

	EnterCriticalSection( &lpVol->csFileList ); // 进入文件链表冲突段

	if( (lpFile = FindOpened( lpVol, szVolMark ) ) != 0 )  // 当前系统是否已经打开卷文件对象？
	{	// 是，当前系统只能打开一个卷文件对象。错误
		SetLastError(ERROR_SHARING_VIOLATION);
		lpFile = (LPKFSDFILE)INVALID_HANDLE_VALUE;
		goto _return;
	}
	// 系统中没有，新增一个
	lpFile = (LPKFSDFILE)malloc( sizeof( KFSDFILE ) + lpVol->fdd.nBytesPerSector );

	if( lpFile )
	{
		// 初始化卷文件对象 init lpFile member
		memset( lpFile, 0, sizeof( KFSDFILE ) );
		// 创建文件节点
		if( ( lpFile->lpfn = CreateFileNode( lpcszFileName ) ) != NULL )
		{	// 成功,初始化lpFile指针
			lpFile->lpbCacheBuf = (LPBYTE)(lpFile+1);
			lpFile->lpVol = lpVol;
			lpFile->hProc = hProc;
			lpFile->dwAccess = 0;
			lpFile->dwShareMode = 0;
			lpFile->dwCreate = 0;
			lpFile->dwFlagsAndAttributes = 0;
			lpFile->flag = VOL_FILE;
			lpFile->dwFilePointer = 0;
			
			InterlockedIncrement( (LPLONG)&lpFile->lpfn->dwRefCount );// 增加对节点的引用数
			AddFileHandleToVolume( lpFile );//加入打开文件链表
		}
		else
		{	// 失败
			free( lpFile );
			lpFile = (LPKFSDFILE)INVALID_HANDLE_VALUE;
		}
	}

_return:

	LeaveCriticalSection( &lpVol->csFileList );// 离开文件链表冲突段

	return lpFile;
}

// **************************************************
// 声明：static int IsValidFilePathName( LPCTSTR lpcFileName )
// 参数：
// 	IN lpcFileName - 文件路径名
// 返回值：
//	假如文件名有效，返回文件名的大小；否则，返回0
// 功能描述：
//	检查一个包含路径的文件名是否有效，例如：
//	"\\kingmos\\abc\\filenamelen.txt", return sizeof(filenamelen.txt)
// 引用: 
// **************************************************

static int IsValidFilePathName( LPCTSTR lpcFileName )
{
	int n = 0;
	int nFileName = 0;
	BOOL bSeparate =FALSE;

	if( lpcFileName  )
	{
		while( *lpcFileName )
		{
			TCHAR c = *lpcFileName;
			if( c == ':' ||
				c == '?' ||
				c == '*' ||
				c == '\"' ||
				c == '<' ||
				c == '>' ||
				c == '|' ||
				n >= MAX_PATH )
			{	// 包含无效的字符
				SetLastError(ERROR_INVALID_NAME);
				return FALSE;
			}
			if( c == '\\' || c == '/' )
			{	// 分隔符
				if( bSeparate )
				{	// 错误：包含连续的分割符
				    SetLastError(ERROR_INVALID_NAME);
				    return FALSE;
				}
				bSeparate = TRUE;//有分割符则说明目前的文件名是文件夹名
				nFileName = 0;
			}
			else
			{	// 有效的文件名
				bSeparate = FALSE;
				nFileName++; //文件名大小 
			}
			lpcFileName++;
			n++;
		}
	}
	return nFileName;//文件名大小 
}


#define IS_DISABLE_DEL( wAttrib ) ( (wAttrib) & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM) )
#define FILE_CREATE_ZONE 0

// **************************************************
// 声明：HANDLE KFSD_CreateFile( 
//						 PVOL pVol, 
//						 HANDLE hProc, 
//						 LPCTSTR lpcszFileName, 
//						 DWORD dwAccess, 
//						 DWORD dwShareMode,
//						 PSECURITY_ATTRIBUTES pSecurityAttributes, 
//						 DWORD dwCreate,
//						 DWORD dwFlagsAndAttributes, 
//						 HANDLE hTemplateFile ) 

// 参数：
//	IN pVol - 卷对象
//	IN hProc - 该文件的拥有者进程
//	IN lpcszFileName-文件名
//	IN fAccess-存取控制，由以下值的位组合：
//				GENERIC_WRITE-读操作
//				GENERIC_READ-写操作
//	IN dwShareMode-共享模式，由以下值的位组合：
//				FILE_SHARE_READ-共享读
//				FILE_SHARE_WRITE-共享写
//	IN pSecurityAttributes-安全设置(不支持，为NULL)
//	IN dwCreate-创建方式，包含：
//				CREATE_NEW-创建新文件；假如文件存在，则失败
//				CREATE_ALWAYS-创建文件；假如文件存在，则覆盖它
//				OPEN_EXISTING-打开文件；假如文件不存在，则失败
//				OPEN_ALWAYS-打开文件；假如文件不存在，则创建
//	IN dwFlagsAndAttributes-文件属性
//	IN hTemplateFile-临时文件句柄(不支持,为NULL)
// 返回值：
//	成功：返回句柄；
//	否则：返回INVALID_HANDLE_VALUE
// 功能描述：
//	创建/打开文件
// 引用: 
//	系统API
// **************************************************

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
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    LPKFSDFILE lpFile, lpOldFile;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CreateFile: %s, PROC=0x%x, dwCreate=0x%x, dwFlagsAndAttributes=0x%x\r\n"), lpcszFileName, hProc, dwCreate, dwFlagsAndAttributes));

	// 检查卷是否可写 ？check volume is write enable ?
	if( (dwAccess & GENERIC_WRITE) &&
		!AccessTest( lpVol ) )
	{	// 错误。error 
		return INVALID_HANDLE_VALUE;
	}
/*
	// 检查卷是否可写 ？check volume is write enable ?
	if( (lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) &&
		(dwAccess & GENERIC_WRITE) )
	{  // 错误。error 
		WARNMSG( 1, ( "error in KFSD_CreateFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return INVALID_HANDLE_VALUE;
	}
*/
	lpcszFileName = IgnoreSpace( lpcszFileName );

	if( stricmp( lpcszFileName, TEXT( "\\Vol:" ) ) == 0 )  // 是卷文件吗 ？
	{   // 是，创建卷文件 special file name, used to DeviceIoControl
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CreateVolumeFile\r\n")));
		return CreateVolumeFile( lpVol, hProc, lpcszFileName );
	}

	DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: check filename.\r\n" ) );
	// 检查文件名是否有效
	if( IsValidFilePathName( lpcszFileName ) == 0 )
	{	
		return INVALID_HANDLE_VALUE; // 无效
	}

	DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: find open.\r\n" ) );
	// 检查之前是否已经打开 ？
	if( (lpOldFile = FindOpened( lpVol, lpcszFileName )) != 0 )
	{	// 是，创建/打开共享文件
		DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: has open,CreateShareFile.\r\n")));
		return CreateShareFile( lpOldFile, pVol, hProc, lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
	}  

	DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: malloc KFSDFILE struct.\r\n" ) );
	// 分配文件对象结构
	lpFile = (LPKFSDFILE)malloc( sizeof( KFSDFILE ) + lpVol->fdd.nBytesPerSector  );

	if( lpFile )
    {
		SF sf;			
		int bRetv;
		
		memset( lpFile, 0, sizeof( KFSDFILE ) );
		DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:CreateFileNode.\r\n" ) );
		// 创建该文件的节点对象
		lpFile->lpfn = CreateFileNode(lpcszFileName);
		lpFile->lpbCacheBuf = (LPBYTE)(lpFile + 1); // 用于读取缓冲文件数据
		
		EnterCriticalSection( &lpVol->csNode ); // 进入冲突段
		
		DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:BeginSearch.\r\n" ) );
		// 准备搜索对象
		if( lpFile->lpfn && BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpcszFileName ) )
		{
			BOOL bUpdate = FALSE;
			
			DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:SearchNode.\r\n" ) );
			bRetv = SearchNode( &sf );  // 在磁盘搜索外交节点
			if( *(sf.lpszCurName + sf.dwCurNameLen) == 0 )  // 是否已经搜索到最后的文件名 ？
			{	// 是				
				if( bRetv == FALSE )
				{	// 没有发现该文件节点
					DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: not found file node.\r\n")));
					if( dwCreate == CREATE_NEW ||
						dwCreate == CREATE_ALWAYS ||
						dwCreate == OPEN_ALWAYS )  //是否创建新的 ？
					{  // 是。not found , add a new
						SYSTEMTIME st;
						
						DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: new file name=%s\r\n"), sf.lpszCurName));
						// 初始化文件磁盘节点对象
						memcpy( GET_NODE_PTR(lpFile)->strName, sf.lpszCurName, sf.dwCurNameLen );
						GetSystemTime( &st );
						SystemTimeToFileTime( &st, &GET_NODE_PTR(lpFile)->ft );
						GET_NODE_PTR(lpFile)->nFileLength = 0;
						GET_NODE_PTR(lpFile)->dwStartSector = NULL_SECTOR;
						GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
						GET_NODE_PTR(lpFile)->bNameLen = (BYTE)sf.dwCurNameLen;
						// 将该节点数据写入磁盘
						bUpdate = bRetv = DoNodeAdd( &sf, GET_NODE_PTR(lpFile) );
						lpFile->lpfn->dwNodeSector = sf.dwCurNodeSector;
						lpFile->lpfn->index = sf.nCurNodeIndex;
					}
				}
				else
				{	// 系统有该文件，found it, check valid
					DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: find file node.\r\n" ) );
					bRetv = FALSE; // 下面还有许多工作要做，先设置为FALSE
					if( dwCreate == CREATE_ALWAYS ||
						dwCreate == OPEN_ALWAYS ||
						dwCreate == OPEN_EXISTING ||
						dwCreate == TRUNCATE_EXISTING )	// 参数是否符合要求？
					{	// 符合
						DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: file exist and dwCreate is valid.\r\n" ) );
						// 得到节点数据
						bRetv = DoNodeGet( &sf, GET_NODE_PTR(lpFile) ); 
						if( bRetv )
						{	// 
							DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: check node attrib.\r\n" ) );
							// 检查节点的属性和存取方法是否合法
							if( ( GET_NODE_PTR(lpFile)->wAttrib & FILE_ATTRIBUTE_DIRECTORY ) ||
								( ( GET_NODE_PTR(lpFile)->wAttrib & ( FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM ) ) && 
								(dwAccess & GENERIC_WRITE) ) )
							{	// 无效的存取方法
								DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:access denied! can't write folder or readonly or system node!.\r\n" ) );
								bRetv = FALSE;  // not right to access
								SetLastError( ERROR_ACCESS_DENIED );
							}
							else
							{	// 合法,设置打开文件节点数据
								lpFile->lpfn->dwNodeSector = sf.dwCurNodeSector;
								lpFile->lpfn->index = sf.nCurNodeIndex;
								DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: open it\r\n")));
							}
						}
						else
						{
							ERRORMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: get node failure.\r\n" ) );
						}
					}
				}				
			}
			else
			{	// 不应该到这里
				WARNMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: not find(%s).\r\n", sf.lpszCurName ) );
				bRetv = FALSE;//error found
			}
			
			if( bRetv )
			{	// 成功创建/打开
				DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: success! init lpFile struct.\r\n" ) );
				// 初始化文件对象结构
				lpFile->lpfn->dwFolderSector = sf.dwFolderStartSector;
				lpFile->dwAccess = dwAccess;
				lpFile->dwCreate = dwCreate;
				lpFile->dwFlagsAndAttributes = dwFlagsAndAttributes;
				lpFile->dwShareMode = dwShareMode;
				lpFile->hProc = hProc;
				lpFile->lpVol = lpVol;
				lpFile->dwFilePointer = 0;
				lpFile->flag = NORMAL_FILE;				
				// 增加对文件节点的引用数 并加入文件对象到卷
				InterlockedIncrement( (LPLONG)&lpFile->lpfn->dwRefCount ); 
				AddFileHandleToVolume( lpFile );
				// 释放搜索对象
				EndSearch( &sf );				
				if( dwCreate == CREATE_ALWAYS )
				{   // 设置新的文件属性并将文件长度设为0（对MS的CREATE_ALWAYS进行测试，发现长度被设为0）。
					GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
					if( ReallocFile( lpFile, 0 ) )  // 释放文件的磁盘空间
					{	// 
						GET_NODE_PTR(lpFile)->nFileLength = 0;
					}
				}									
				else if( dwCreate == TRUNCATE_EXISTING ) 
				{	//  set cotent to zero
					if( ReallocFile( lpFile, 0 ) )	// 释放文件的磁盘空间
					{
						GET_NODE_PTR(lpFile)->nFileLength = 0;
					}
				}
				LeaveCriticalSection( &lpVol->csNode );  // 离开冲突段
				if( bUpdate )	// 文件节点是否更新 ？
				{	// 是，通知系统文件系统的变化信息
					DoShellNotify( lpVol, 
								   SHCNE_CREATE,
								   SHCNF_PATH | SHCNF_FLUSHNOWAIT,
								   lpFile->lpfn->lpszFileName,
								   NULL,
								   GET_NODE_PTR(lpFile) );
				}
				return lpFile;
			}
			// 失败，清除
			EndSearch( &sf );			
		}
		LeaveCriticalSection( &lpVol->csNode );		
		// 失败，清除
		if( lpFile->lpfn )
			DeleteFileNode( lpFile->lpfn );
		free( lpFile );
	}
	
    return INVALID_HANDLE_VALUE;
}

#undef FILE_CREATE_ZONE

// **************************************************
// 声明：static BOOL AddFindHandleToVolume( LPKFSDFIND lpFind )
// 参数：
// 	IN lpFind - 查找结构指针
// 返回值：
//	假如成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//	将查找对象加入查找对象链表
// 引用: 
// **************************************************

static BOOL AddFindHandleToVolume( LPKFSDFIND lpFind )
{
	EnterCriticalSection( &lpFind->lpVol->csFindList );
	lpFind->lpNext = lpFind->lpVol->lpFindFile;
	lpFind->lpVol->lpFindFile = lpFind;
	LeaveCriticalSection( &lpFind->lpVol->csFindList );
	return TRUE;
}

// **************************************************
// 声明：static BOOL RemoveFindHandleFromVolume( LPKFSDFIND lpFile )
// 参数：
// 	IN lpFile - 查找结构指针
// 返回值：
//	假如成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//	与AddFindHandleToVolume相反 将查找对象移出查找对象链表
// 引用: 
// **************************************************

static BOOL RemoveFindHandleFromVolume( LPKFSDFIND lpFile )
{
	LPKFSDVOL lpVol; 
	LPKFSDFIND lpf;
	BOOL bRetv = FALSE;
    
	EnterCriticalSection( &lpFile->lpVol->csFindList ); // 进入冲突段

	lpVol = lpFile->lpVol;
	lpf = lpVol->lpFindFile;
	if(  lpf == lpFile )	// 在链表头吗 ？
	{	//链表头
		lpVol->lpFindFile = lpFile->lpNext;
		lpFile->lpNext = NULL;
		bRetv = TRUE;
	}
	else
	{	//非，查找他它
		while ( lpf )
		{
			if( lpf->lpNext == lpFile )
			{	// 找到
				lpf->lpNext = lpFile->lpNext;
				lpFile->lpNext = NULL;
				bRetv = TRUE;
				break;
			}
			lpf = lpf->lpNext;
		}
	}
    
	LeaveCriticalSection( &lpVol->csFindList );// 离开冲突段

	DEBUGMSG( bRetv == FALSE && FILE_ZONE, (TEXT("KFSD: Error RemoveFindHandleFromVolume.\r\n")));
	return bRetv;	
}

// **************************************************
// 声明：static BOOL AddFileHandleToVolume( LPKFSDFILE lpFile )
// 参数：
// 	IN lpFile - 文件对象结构指针
// 返回值：
//	假如成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//	将文件对象加入文件对象链表
// 引用: 
// **************************************************

static BOOL AddFileHandleToVolume( LPKFSDFILE lpFile )
{
	EnterCriticalSection( &lpFile->lpVol->csFileList );

	lpFile->lpNext = lpFile->lpVol->lpOpenFile;
	lpFile->lpVol->lpOpenFile = lpFile;

	LeaveCriticalSection( &lpFile->lpVol->csFileList );
	return TRUE;
}

// **************************************************
// 声明：static BOOL RemoveFileHandleFromVolume( LPKFSDFILE lpFile )
// 参数：
// 	IN lpFile - 查找结构指针
// 返回值：
//	假如成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//	与 AddFileHandleToVolume 相反 将文件对象移出文件对象链表
// 引用: 
// **************************************************

static BOOL RemoveFileHandleFromVolume( LPKFSDFILE lpFile )
{
	BOOL bRetv = FALSE;
	LPKFSDVOL lpVol; 
	LPKFSDFILE lpf;
	
	EnterCriticalSection( &lpFile->lpVol->csFileList );	// 进入冲突段
	
	lpVol = lpFile->lpVol;
	lpf = lpVol->lpOpenFile;
	if(  lpf == lpFile )
	{
		lpVol->lpOpenFile = lpFile->lpNext;
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
	
	LeaveCriticalSection( &lpFile->lpVol->csFileList );// 离开冲突段
	
	DEBUGMSG( bRetv == FALSE && FILE_ZONE, (TEXT("KFSD: Error RemoveFileHandleFromVolume.\r\n")));
	
	return bRetv;
	
}


// **************************************************
// 声明：BOOL KFSD_CloseFile( PFILE pf )
// 参数：
// 	IN pf - 文件对象
// 返回值：
//	假如成功，返回TRUE; 失败，返回FALSE
// 功能描述：
//	关闭文件对象
// 引用: 
// **************************************************

BOOL KFSD_CloseFile( PFILE pf )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
	BOOL bRetv = TRUE;
	
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CloseFile=%s.\r\n"), lpFile->lpfn->lpszFileName));

	if( (lpFile->dwAccess & GENERIC_WRITE) && lpFile->flag == NORMAL_FILE )
	{   // malloc 应该用更好的办法，用文件cache ?
	 	LPBYTE pSector = (LPBYTE)malloc( lpFile->lpVol->fdd.nBytesPerSector );

		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CloseFile0.\r\n") ));
				
		if( pSector )
		{
			PNODE pNode;
			DWORD dwOffset;

			EnterCriticalSection( &lpFile->lpfn->csFileNode );

			pNode = GET_NODE_PTR( lpFile ); // 文件磁盘节点
			// 文件磁盘节点在文件夹的便移
			dwOffset = GET_NODE_OFFSET( lpFile->lpfn->index, lpFile->lpVol );			
			DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CloseFile: write node to disk.\r\n") ));
			// 将节点数据写入磁盘
			if( WriteSectors( lpFile->lpVol, lpFile->lpfn->dwNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), pSector ) )
			{
				LeaveCriticalSection( &lpFile->lpfn->csFileNode );  // 离开冲突段
				DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CloseFile: DoUpdateVolume.\r\n") ));
				DoUpdateVolume( lpFile->lpVol );	// 更新卷数据
				DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CloseFile: DoShellNotify.\r\n") ));
				// 想系统发通知消息
				DoShellNotify( lpFile->lpVol, SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpFile->lpfn->lpszFileName, NULL, pNode );
			}
			else
			{
				LeaveCriticalSection( &lpFile->lpfn->csFileNode );// 离开冲突段
				bRetv = FALSE;
			}            
			free( pSector );
		}
		else
			bRetv = FALSE;
    }
	if( lpFile )
	{	// 移出文件对象
		RemoveFileHandleFromVolume( lpFile );
		// 减少引用值，如果没有任何对象使用文件节点，则释放文件节点
		if( InterlockedDecrement( (LPLONG)&lpFile->lpfn->dwRefCount ) == 0 )
			DeleteFileNode( lpFile->lpfn );  // 删除文件节点数据
		free( lpFile ); // 释放文件对象指针
	}
	if( bRetv == FALSE )
	{
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: Error CloseFile\r\n") ) );
	}
    return bRetv;
}

// **************************************************
// 声明：DWORD KFSD_GetFileAttributes(
//                            PVOL pVol,
//                            LPCTSTR lpszFileName )
// 参数：
// 	IN pVol - 卷对象
// 	IN lpszFileName - 文件名
// 返回值：
//	假如成功，返回文件属性；失败,返回0xffffffff
// 功能描述：
//	得到文件属性
// 引用: 
//	系统API
// **************************************************

DWORD KFSD_GetFileAttributes(
                            PVOL pVol,
                            LPCTSTR lpszFileName )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
	SF sf;
	DWORD bRetv=0xffffffffl;

	DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: GetFileAttributes=%s.\r\n"), lpszFileName ) );

	lpszFileName = IgnoreSpace( lpszFileName ); // 忽略文件名前面的空格

	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )  // 准备查找对象
	{
		bRetv = SearchNode( &sf );	// 查找文件节点
        if( bRetv )
		{	// 找到
			DEBUGMSG(FILE_ZONE, (TEXT("KFSD: GetFileAttributes, Attrib=0x%x.\r\n"), sf.pCurNode->wAttrib));
			bRetv = sf.pCurNode->wAttrib;
		}
		else
		{	// 没有找到文件
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: GetFileAttributes, not found.\r\n")));
			bRetv = 0xffffffffl;
		}
		EndSearch( &sf ); // 释放查找对象
	}
	return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_ReadFile(
//                  PFILE pf,
//					LPVOID lpBuffer,
//                  DWORD dwNumToRead,
//                  LPDWORD lpdwNumRead, 
//					LPOVERLAPPED pOverlapped )

// 参数：
//	IN pf-文件对象
//	OUT lpBuffer-用于接受数据的内存
//	IN dwNumToRead-欲读的字节数
//	OUT lpdwNumRead-返回实际读的字节数；假如lpdwNumRead为NULL,则不返回
//	IN pOverlapped-覆盖(不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	从文件读取数据
// 引用: 
// **************************************************

BOOL KFSD_ReadFile(
                  PFILE pf,
                  LPVOID lpBuffer,
                  DWORD dwNumToRead,
                  LPDWORD lpdwNumRead, 
				  LPOVERLAPPED pOverlapped )
{    
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    DWORD sr = 0;
	DWORD dwFilePointer;
    DWORD dwStartSector;
    DWORD dwOffset;
	UINT nBytesPerSector;


    DEBUGMSG( FILE_ZONE, (TEXT("KFSD: ++ReadFile, num=%d, pointer=%d.\r\n"), dwNumToRead,lpFile->dwFilePointer ));

	if( lpdwNumRead )
       *lpdwNumRead = 0;
	// 检查能否读区
    if( (lpFile->dwAccess & GENERIC_READ) && lpFile->flag == NORMAL_FILE )
	{	// 可以
		nBytesPerSector = lpFile->lpVol->fdd.nBytesPerSector;
		EnterCriticalSection( &lpFile->lpfn->csFileNode );
		
		dwFilePointer = lpFile->dwFilePointer;
		if( dwFilePointer >= GET_NODE_PTR(lpFile)->nFileLength )
		{	// 当前文件读去位置在文件末尾，没有更多的数据
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
			return TRUE;
		}
		// 能否读要求的数据量 ？如果不能，设置实际读的数据
		if( dwFilePointer + dwNumToRead > GET_NODE_PTR(lpFile)->nFileLength )
			dwNumToRead = GET_NODE_PTR(lpFile)->nFileLength - dwFilePointer;
		// 
		if( dwNumToRead )
		{
			void * lpSector;

			if( lpFile->uiCacheValid )  // cache有效吗 ？
			{
				DWORD dwCachePosEnd = lpFile->dwCachePosStart + nBytesPerSector;
				if( dwFilePointer >= lpFile->dwCachePosStart &&
					dwFilePointer < dwCachePosEnd ) // 当前的cache数据符合要求吗 ？
				{   // 是，符合要求 hint the cache
					DWORD dwMaxNum = MIN( dwNumToRead, dwCachePosEnd - dwFilePointer );
					// 读取适当的数据
					dwOffset = ( dwFilePointer % nBytesPerSector ); 
					memcpy( lpBuffer, lpFile->lpbCacheBuf + dwOffset, dwMaxNum );
					lpBuffer = (LPBYTE)lpBuffer + dwMaxNum;
					dwNumToRead -= dwMaxNum;  // 余下的数据数
					sr += dwMaxNum;
					dwFilePointer += dwMaxNum;
				}
			}

			if( dwNumToRead ) // 读完吗 ？
			{	//没有，继续读余下的数据
				DWORD k;
				// 得到当前位置的开始块
				dwStartSector = GetSectorOffset( lpFile->lpVol, GET_NODE_PTR(lpFile)->dwStartSector, (dwFilePointer / nBytesPerSector) );
				// 得到当前位置的块内偏移
				dwOffset = (dwFilePointer % nBytesPerSector );
				//
				lpSector = lpFile->lpbCacheBuf;
				// 从磁盘读余下的数据
				k = ReadSectors( lpFile->lpVol, dwStartSector, dwOffset, (LPBYTE)lpBuffer, dwNumToRead, lpSector, &lpFile->uiCacheValid );
					
				ASSERT_NOTIFY( k == dwNumToRead, "not read enough bytes\r\n" );
				if( k == 0 )
				{	// 错误
					LeaveCriticalSection( &lpFile->lpfn->csFileNode );
					goto _error_ret; 
				}

				sr += k;  // 实际读的数据数量
				// 假如可能，更新cache
				if( lpFile->uiCacheValid )
				{	
					lpFile->dwCachePosStart = ( (dwFilePointer + k - 1) / nBytesPerSector ) * nBytesPerSector;
				}
			}
		}
		if( lpdwNumRead )
			*lpdwNumRead = sr;
		lpFile->dwFilePointer += sr;	// 设置当前文件存取位置

		LeaveCriticalSection( &lpFile->lpfn->csFileNode ); //离开冲突段

		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: --ReadFile, readnum=%d\r\n"), sr ) );
		return TRUE;
	}
	else
	    SetLastError(ERROR_ACCESS_DENIED);
_error_ret:
	return FALSE;
}

// **************************************************
// 声明：BOOL KFSD_ReadFileWithSeek(
//                          PFILE pf,
//                          LPVOID lpBuffer,
//                          DWORD dwNumToRead,
//                          LPDWORD lpdwNumRead,
//							LPOVERLAPPED pOverlapped,
//                          DWORD dwLowOffset,
//                          DWORD dwHighOffset)
// 参数：
//	IN pf-文件对象
//	OUT lpBuffer-用于接受数据的内存
//	IN dwNumToRead-欲读的字节数
//	OUT lpdwNumRead-返回实际读的字节数；假如lpdwNumRead为NULL,则不返回
//	IN pOverlapped-覆盖(不支持，为NULL)
//	IN dwLowOffset - 偏移值的低32位
//	IN dwHighOffset - 偏移值的高32位
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	将文件存取位置设到由dwLowOffset dwHighOffset确定的值,然后从文件读取数据
// 引用: 
//	系统API,不支持
// **************************************************

BOOL KFSD_ReadFileWithSeek(
                          PFILE pf,
                          LPVOID pBuffer,
                          DWORD cbRead,
                          LPDWORD pcbRead,
						  LPOVERLAPPED pOverlapped,
                          DWORD dwLowOffset,
                          DWORD dwHighOffset)
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here

    RETAILMSG(FILE_ZONE, (TEXT("KFSD: ReadFileWithSeek\r\n")));
	if(  lpFile->flag == NORMAL_FILE )
		;
    lpFile->uiCacheValid = FALSE;    //????
    return FALSE;    
}


// **************************************************
// 声明：DWORD KFSD_SetFilePointer(
//							PFILE pf,
//							LONG lDistanceToMove,
//							LPLONG pDistanceToMoveHigh,
//							DWORD dwMoveMethod )
// 参数：
//	IN pf - 文件对象
//	IN lDistanceToMove-相对偏移值（低32bits）
//	IN pDistanceToMoveHigh-(高32bits,不支持,为NULL)
//	IN dwMethod-偏移的起始位置，包含：
//		FILE_BEGIN-文件开始位置
//		FILE_CURRENT-文件当前位置
//		FILE_END-文件结束位置
//
// 返回值：
//	成功：返回新的文件位置
//	否则：返回0xffffffff
// 功能描述：
//	设置文件当前存取位置
// 引用: 
//	系统API
// **************************************************

DWORD KFSD_SetFilePointer(
                         PFILE pf,
                         LONG lDistanceToMove,
						 LPLONG pDistanceToMoveHigh,
                         DWORD dwMoveMethod)
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here
    LONG cur;
	// 是否存取合法 ？
	if( (lpFile->dwAccess & (GENERIC_WRITE|GENERIC_READ)) && lpFile->flag == NORMAL_FILE )
	{	// 是，合法
		EnterCriticalSection( &lpFile->lpfn->csFileNode ); // 进入冲突段

		if( dwMoveMethod == FILE_BEGIN )	// 从开始计算存取位置
			cur = 0;
		else if( dwMoveMethod == FILE_CURRENT )	// 从文件当前位置开始计算存取位置
			cur = lpFile->dwFilePointer;
		else if( dwMoveMethod == FILE_END )	// 从文件结束位置开始计算存取位置
		{
			cur = GET_NODE_PTR(lpFile)->nFileLength;
		}
		cur += lDistanceToMove;	// 得到需要设定位置
		if( cur < 0 )
			cur = 0;	// 设为开始位置
		lpFile->dwFilePointer = cur;

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );	// 离开冲突段
		
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: SetFilePoiter=%d.\r\n"), cur));
		
		if( pDistanceToMoveHigh )
			*pDistanceToMoveHigh = 0;
		return cur;
	}
	else
	    SetLastError(ERROR_ACCESS_DENIED);

	return 0xFFFFFFFFl;	//错误
}

// **************************************************
// 声明：DWORD KFSD_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh )
// 参数：
//	IN pf-文件对象
//	IN pFileSizeHigh - 文件尺寸的高32bits(不支持，为NULL)
// 返回值：
//	成功：返回文件大小
//	否则：返回 INVALID_FILE_SIZE
// 功能描述：
//	得到文件大小
// 引用: 
//	系统API
// **************************************************

DWORD KFSD_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: GetFileSize=%d.\r\n"), GET_NODE_PTR(lpFile)->nFileLength));
	
	if(  lpFile->flag == NORMAL_FILE )
	{
		DWORD dwSize;
		if( pFileSizeHigh )
			*pFileSizeHigh = 0;
		// 32 bits CPU
		//EnterCriticalSection( &lpFile->lpfn->csFileNode );	// 
		dwSize = GET_NODE_PTR(lpFile)->nFileLength;
		//LeaveCriticalSection( &lpFile->lpfn->csFileNode );
		return dwSize;
	}
	else
		return INVALID_FILE_SIZE;
}

// **************************************************
// 声明：BOOL KFSD_GetFileInformationByHandle(
//                                    PFILE pf,
//                                    FILE_INFORMATION * pfi )
// 参数：
//	IN pf - 文件对象
// 	OUT pfi - FILE_INFORMATION结构指针，用于接受文件信息
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到文件信息
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_GetFileInformationByHandle(
                                    PFILE pf,
                                    FILE_INFORMATION * pfi )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: GetFileInfoByHandle.\r\n")));

	if( lpFile->flag == NORMAL_FILE )
	{		
		memset( pfi, 0, sizeof( FILE_INFORMATION ) );

		EnterCriticalSection( &lpFile->lpfn->csFileNode ); // 进入冲突段

		pfi->dwFileAttributes = GET_NODE_PTR(lpFile)->wAttrib;
		pfi->dwVolumeSerialNumber = lpFile->lpVol->rfh.dwSerialNum;
		pfi->nFileIndexHigh = 0;
		pfi->nFileIndexLow = 0;
		pfi->nFileSizeHigh = 0;
		pfi->nFileSizeLow = GET_NODE_PTR(lpFile)->nFileLength;
		pfi->nNumberOfLinks = 1;
		// KMFS 文件系统只有CreateTime的属性位
		pfi->ftCreationTime.dwLowDateTime = GET_NODE_PTR(lpFile)->ft.dwLowDateTime;
		pfi->ftCreationTime.dwHighDateTime = GET_NODE_PTR(lpFile)->ft.dwHighDateTime;
		*(&pfi->ftLastAccessTime) = *(&pfi->ftCreationTime);
		*(&pfi->ftLastWriteTime) = *(&pfi->ftCreationTime);

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );// 离开冲突段
		
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL  KFSD_GetFileTime( 
//                      PFILE pf,
//                      FILETIME * pCreate,
//					    FILETIME * pLastAccess,
//					    FILETIME * pLastWrite)
// 参数：
//	IN pf - 文件对象指针
//	OUT pCreate - 指向FILETIME结构的指针，文件创建时间
//	OUT pLastAccess - 指向FILETIME结构的指针，文件最后一次存取时间(目前暂不支持，为NULL)
//	OUT pLastWrite - 指向FILETIME结构的指针，文件最后一次更新数据的时间(目前暂不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	得到文件时间
// 引用: 
//	系统API
// **************************************************

BOOL  KFSD_GetFileTime( 
                      PFILE pf,
                      FILETIME * pCreate,
					  FILETIME * pLastAccess, 
					  FILETIME * pLastWrite)
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here  
	BOOL bRetv = FALSE;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: GetFileTime.\r\n")));
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
	
    return bRetv;
}


// **************************************************
// 声明：BOOL KFSD_WriteFile( 
//                  PFILE pf, 
//                  LPCVOID lpBuffer,
//                  DWORD dwNumToWrite, 
//                  LPDWORD lpdwNumWrite,
//					LPOVERLAPPED pOverlapped)
// 参数：
//	IN pf-文件对象
//	IN lpBuffer-欲写的数据的存放内存
//	IN dwNumToWrite-欲写的字节数
//	OUT lpdwNumWrite-返回实际写的字节数；假如lpdwNumWrite为NULL,则不返回
//	IN pOverlapped-覆盖(不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	写数据到文件
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_WriteFile( 
                   PFILE pf, 
                   LPCVOID lpBuffer,
                   DWORD dwNumToWrite, 
                   LPDWORD lpdwNumWrite,
				   LPOVERLAPPED pOverlapped)
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here
    DWORD sr = 0;
    DWORD dwStartSector;
    DWORD dwOffset;
    DWORD fs;
	void * lpSector;
	BOOL bRetv = FALSE;

    DEBUGMSG(FILE_ZONE, (TEXT("KFSD: ++WriteFile, num= %d, pointer=%d, fl=%d.\r\n"), dwNumToWrite, lpFile->dwFilePointer, GET_NODE_PTR(lpFile)->nFileLength));

	if( lpdwNumWrite )
        *lpdwNumWrite = 0;
	//	检查存取许可
	if( (lpFile->dwAccess & GENERIC_WRITE) && lpFile->flag == NORMAL_FILE )
	{
		if( dwNumToWrite )
		{
			EnterCriticalSection( &lpFile->lpfn->csFileNode );	// 进入冲突段

			// 检查是否需要扩展文件大小
			fs = lpFile->dwFilePointer + dwNumToWrite;			
			if( fs > GET_NODE_PTR(lpFile)->nFileLength )	
			{	// 需要更大的文件空间，分配！
				if( ReallocFile( lpFile, fs ) == FALSE )
				{	// 分配失败
					LeaveCriticalSection( &lpFile->lpfn->csFileNode );	// 离开冲突段
					goto _return;
				}
				// 重分配文件成功
				GET_NODE_PTR(lpFile)->nFileLength = fs;
				DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: Realloc Sector Success, fs=%d.\r\n"), fs ) );
			}
			// 得到写入数据的开始扇区
			dwStartSector = GetSectorOffset( lpFile->lpVol, GET_NODE_PTR(lpFile)->dwStartSector, (lpFile->dwFilePointer / lpFile->lpVol->fdd.nBytesPerSector) );
			// 得到写入数据的扇区内偏移
			dwOffset = (lpFile->dwFilePointer % lpFile->lpVol->fdd.nBytesPerSector);			
			lpSector = lpFile->lpbCacheBuf;
			// 写入数据
			sr = WriteSectors( lpFile->lpVol, dwStartSector, dwOffset, (const unsigned char *)lpBuffer, dwNumToWrite, lpSector );
			if( sr )  // 实际写的数据
			{	// 
				if( lpdwNumWrite )
					*lpdwNumWrite = sr;
				lpFile->dwFilePointer += sr;	// 重设文件指针
				lpFile->uiCacheValid = FALSE;	// 当前cache无效
				RETAILMSG(FILE_ZONE, (TEXT("KFSD: --WriteFile, writenum= %d\r\n"), sr));
				bRetv = TRUE;
			}

			LeaveCriticalSection( &lpFile->lpfn->csFileNode );	// 离开冲突段
		}
		else
			bRetv = TRUE;
	}
_return:
	return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_FlushFileBuffers(  PFILE pf ) 
// 参数：
// 	IN pf - 文件对象指针
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	刷新文件缓存（假如有的话）
// 引用: 
// **************************************************

BOOL KFSD_FlushFileBuffers(  PFILE pf ) 
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here

	RETAILMSG(FILE_ZONE, (TEXT("KFSD: FlushFileBuffer\r\n")));
	if( (lpFile->dwAccess & GENERIC_WRITE) && 
		lpFile->flag == NORMAL_FILE )
		;
	lpFile->uiCacheValid = FALSE;

    return TRUE;
}

// **************************************************
// 声明：BOOL KFSD_WriteFileWithSeek( 
//                           PFILE pf, 
//                           LPCVOID pBuffer, 
//                           DWORD cbWrite, 
//                           LPDWORD pcbWritten, 
//                           DWORD dwLowOffset, 
//                           DWORD dwHighOffset ) 
// 参数：
//	IN pf-文件对象
//	OUT pBuffer-写入数据的内存
//	IN cbWrite-欲写的字节数
//	OUT pcbWritten-返回实际写的字节数；假如 pcbWritten 为NULL,则不返回
//	IN dwLowOffset - 偏移值的低32位
//	IN dwHighOffset - 偏移值的高32位
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	将文件存取位置设到由dwLowOffset dwHighOffset确定的值,然后写数据到文件
// 引用: 
//	系统API,不支持
// **************************************************

BOOL KFSD_WriteFileWithSeek( 
                           PFILE pf, 
                           LPCVOID pBuffer, 
                           DWORD cbWrite, 
                           LPDWORD pcbWritten, 
                           DWORD dwLowOffset, 
                           DWORD dwHighOffset ) 
                           
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here

	RETAILMSG(FILE_ZONE, (TEXT("KFSD: WriteFileWithSeek\r\n")));
    if( (lpFile->dwAccess & GENERIC_WRITE) && 
		lpFile->flag == NORMAL_FILE )
		;
    lpFile->uiCacheValid = FALSE;
    return FALSE;    
}

// **************************************************
// 声明：BOOL KFSD_SetFileAttributes( 
//                           PVOL pVol,
//                           LPCTSTR lpszFileName, 
//                           DWORD dwFileAttributes ) 

// 参数：
//	IN pVol - 卷对象
//	IN lpszFileName-文件名
//	IN dwFileAttributes-文件属性,包含：
//			FILE_ATTRIBUTE_ARCHIVE-文档文件
//			FILE_ATTRIBUTE_HIDDEN-隐藏文件
//			FILE_ATTRIBUTE_READONLY-只读文件
//			FILE_ATTRIBUTE_SYSTEM-系统文件
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件属性
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_SetFileAttributes( 
                           PVOL pVol,
                           LPCTSTR lpszFileName, 
                           DWORD dwFileAttributes ) 
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //do somthing here
	SF sf;
	int bRetv = FALSE;
    
    DEBUGMSG(FILE_ZONE, (TEXT("KFSD: SetFileAttrib:%s, Attrib=0x%x.\r\n"), lpszFileName, dwFileAttributes));

	// 检查卷是否可写 ？check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// 错误。error 
		return FALSE;
	}
/*
	// 磁盘卷可写吗 ？
	if( (lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) )
	{  // 不可以写，错误 error 
		RETAILMSG( 1, ( "error in KFSD_SetFileAttributes: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
*/
	// 忽略文件名的字符串
	lpszFileName = IgnoreSpace( lpszFileName );
    if( dwFileAttributes == FILE_ATTRIBUTE_NORMAL )
        dwFileAttributes = 0;
    else
        dwFileAttributes &= ~( FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ROM | FILE_ATTRIBUTE_DIRECTORY );
	// 初始化搜索对象
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
    {
        //do somthing here
		bRetv = SearchNode( &sf );	// 搜索文件节点
        if( bRetv )
        {	// 找到
            if( sf.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY )
			{	// 如果节点是文件夹，不能去掉该属性
                dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            }
            if( sf.pCurNode->wAttrib & FILE_ATTRIBUTE_DEVICE )
			{	// 如果节点是文件夹，不能去掉该属性
                dwFileAttributes |= FILE_ATTRIBUTE_DEVICE;
            }			
			// 新属性
			sf.pCurNode->wAttrib = (WORD)(dwFileAttributes & 0xFFFF);
			//	更新到磁盘
			DoNodeUpdate( &sf, sf.pCurNode );//&node );
			//	通知系统文件变化
			DoShellNotify( lpVol, SHCNE_ATTRIBUTES, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszFileName, NULL, sf.pCurNode );
        }
		EndSearch( &sf );  // 释放搜索对象
    }
    return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_MoveFile( 
//                  PVOL pVol,
//                  LPCTSTR lpszOldFileName, 
//                  LPCTSTR lpszNewFileName )
// 参数：
// 	IN pVol - 卷对象
// 	IN lpszOldFileName - 当前已存在的文件或文件夹
// 	IN lpcszNewFileName - 新的文件或文件夹名(新的文件或文件夹名不能已存在)
// 返回值：
//	成功：返回 TRUE
//	否则：返回 FALSE
// 功能描述：
//	移动文件或文件夹
//	新的文件与已存在的文件可在不同的卷;新的文件夹与已存在的文件夹必须在相同的卷。
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_MoveFile( 
                  PVOL pVol,
                  LPCTSTR lpszOldFileName, 
                  LPCTSTR lpszNewFileName )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //do somthing here
	SF sfOld;
	SF sfNew;
	BOOL bRetv = FALSE;

	DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: MoveFile, old=%s, new=%s.\r\n" ), lpszOldFileName, lpszNewFileName ) );

	// 检查卷是否可写 ？check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// 错误。error 
		return FALSE;
	}

	// 检查磁盘卷是否可写 check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // 不可写 error 
		//WARNMSG( 1, ( "error in KFSD_MoveFile: disk readonly, ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}
	// 忽略文件名的空格
	lpszOldFileName = IgnoreSpace( lpszOldFileName );
	lpszNewFileName = IgnoreSpace( lpszNewFileName );
	// 文件名是否有效 ？
	if( IsValidFilePathName( lpszNewFileName ) == 0 )
	{	// 无效
		return FALSE;
	}
	// 当前系统是否已经打开该文件
	if( FindOpened( lpVol, lpszOldFileName ) != NULL )// LN: 2003-05-21
	{	// 已经打开
		SetLastError(ERROR_SHARING_VIOLATION);
		return FALSE;
	}

    EnterCriticalSection( &lpVol->csNode );  // 进入冲突段
	// 初始化搜索对象
	if( BeginSearch( &sfNew, lpVol, 0, (LPTSTR)lpszNewFileName ) )
	{   // 
		bRetv = SearchNode( &sfNew ); // 搜索文件磁盘节点 
		if( bRetv )
		{	// 新文件名已经存在。error , the new file is exist
			EndSearch( &sfNew );
			bRetv = FALSE; // 错误返回
			goto LEAVE_CS;
		}
		// 初始化搜索对象
		if( BeginSearch( &sfOld, lpVol, 0, (LPTSTR)lpszOldFileName ) )
		{	
			bRetv = SearchNode( &sfOld );// 搜索老文件节点
			if( bRetv = ( bRetv && !( sfOld.pCurNode->wAttrib & FILE_ATTRIBUTE_DEVICE ) ) )
			{	// 文件存在 
				if( *(sfNew.lpszCurName + sfNew.dwCurNameLen) == 0 ) // 搜索完？
				{	
					// 将已存在的文件节点拷贝到新文件节点
					*sfNew.pCurNode = *sfOld.pCurNode; 
                    sfNew.pCurNode->bNameLen = (BYTE)sfNew.dwCurNameLen; // 文件名长度
					memcpy( sfNew.pCurNode->strName, sfNew.lpszCurName, sfNew.dwCurNameLen );
					//	在磁盘增加文件节点
					bRetv = DoNodeAdd( &sfNew, sfNew.pCurNode );
					if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{	// 文件节点是文件夹，需要改变其父文件夹的指向
			            PNODE pNode;
						// 读文件夹数据的第一个扇区
						if( FSDMGR_ReadDisk( lpVol->hDsk, sfNew.pCurNode->dwStartSector, 1, sfNew.pSector, lpVol->fdd.nBytesPerSector ) == ERROR_SUCCESS )
						{
							pNode = (PNODE)( (LPBYTE)sfNew.pSector + NODE_ALIGN_SIZE );
							if( pNode->strName[0] == '.' &&  pNode->strName[1] == '.' )
							{   // 该节点指向父节点 update '..' node
								pNode->dwStartSector = sfNew.dwFolderStartSector;  // 新的父文件夹
								// 写入磁盘
								if( FSDMGR_WriteDisk( lpVol->hDsk, sfNew.pCurNode->dwStartSector, 1, sfNew.pSector, lpVol->fdd.nBytesPerSector ) == ERROR_SUCCESS )
									bRetv = TRUE;
								else
									bRetv = FALSE;
							}
							else
							{
								ERRORMSG(FILE_ERROR, (TEXT("KFSD: MoveFile, error folder info.\r\n")));
							}
						}
					}
					if( bRetv )
					{   // 删除老文件
						if( DoNodeDel( &sfOld, FALSE ) )
						{  // success, delete old
							DWORD dwEvent;
							NODE node = *sfOld.pCurNode;
							// 成功删除老文件
							// 释放搜索对象
							EndSearch( &sfOld );
							EndSearch( &sfNew );
							if( node.wAttrib & FILE_ATTRIBUTE_DIRECTORY )
								dwEvent = SHCNE_RENAMEFOLDER;
							else
								dwEvent = SHCNE_RENAMEITEM;

							LeaveCriticalSection( &lpVol->csNode ); // 离开冲突段
							// 向系统通知改变
							DoShellNotify( lpVol, dwEvent, SHCNF_PATH|SHCNF_FLUSHNOWAIT, lpszOldFileName, lpszNewFileName, &node );
							bRetv = TRUE;
							goto RETURN;
						}
					}
				}
			}
			EndSearch( &sfOld );
		}
		EndSearch( &sfNew );
	}

LEAVE_CS:
	LeaveCriticalSection( &lpVol->csNode );
RETURN:
	return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_DeleteFile(
//                    PVOL pVol,
//                    LPCTSTR lpszFileName )
// 参数：
// 	IN pVol - 卷对象
//	IN lpszFileName - 文件名
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	删除文件
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_DeleteFile(
                    PVOL pVol,
                    LPCTSTR lpszFileName )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //do somthing here
	SF sf;
	BOOL bRetv = FALSE;
	
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DeleteFile=%s.\r\n"),lpszFileName));

	// 检查卷是否可写 ？check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// 错误。error 
		return FALSE;
	}

	// 检查磁盘是否可以写 check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // 磁盘不可写。error 
	//	RETAILMSG( 1, ( "error in KFSD_DeleteFile: ACCESS_DENIED.\r\n" ) );
	//	SetLastError(ERROR_ACCESS_DENIED);
	//	return FALSE;	// 错误返回
	//}
	// 忽略文件的空格
	lpszFileName = IgnoreSpace( lpszFileName );
	// 当前是否已经打开该文件
	if( FindOpened( lpVol, lpszFileName ) != NULL )// LN: 2003-05-21
	{	// 已打开，错误返回
		SetLastError(ERROR_SHARING_VIOLATION);
		return FALSE;
	}
	// 初始化搜索对象
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
	{
		bRetv = SearchNode( &sf );  // 可是搜索
		if( bRetv  )
		{	// 得到，判断是否能够删除
			if( 0 == ( sf.pCurNode->wAttrib & ( FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM ) ) )
			{
				if( (bRetv = DoNodeDel( &sf, TRUE )) != FALSE )
				{	// 删除成功
					NODE node = *sf.pCurNode;

					EndSearch( &sf );  // 释放搜索对象
					// 更新卷对象
					DoUpdateVolume( lpVol ); // 该代码可以不要
					// 向系统通知改变
					bRetv = DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszFileName, NULL, &node );
					DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DeleteFile Sucess=%d.\r\n"), bRetv));
					return TRUE;
				}
			}
			DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DeleteFile ACCESS_DENIED\r\n") ));
			SetLastError( ERROR_ACCESS_DENIED );
		}
		EndSearch( &sf );// 释放搜索对象
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL KFSD_DeleteAndRenameFile( 
//                              PVOL pVol,
//                              LPCTSTR lpszNewFileName, 
//                              LPCTSTR lpszOldFileName )
// 参数：
// 	IN pVol - 卷对象
// 	IN lpcszNewFileName – 目标文件
// 	IN lpcszOldFileName – 源文件
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将源文件内容拷贝到目标文件后再删除源文件
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_DeleteAndRenameFile( 
                              PVOL pVol,
                              LPCTSTR lpszNewFileName, 
                              LPCTSTR lpszOldFileName )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //do somthing here
	SF sfOld;
	SF sfNew;
	BOOL bRetv = FALSE;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DeleteAndRenameFile,Old=%s, New=%s.\r\n"), lpszOldFileName, lpszNewFileName));
	// 检查卷是否可写 ？check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// 错误。error 
		return FALSE;
	}

	// 磁盘是否可写？ check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // 不可写 error 
		//WARNMSG( FILE_ZONE, ( "error in KFSD_DeleteAndRenameFile: ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}

	// 忽略文件的空格
	lpszNewFileName = IgnoreSpace( lpszNewFileName );
	lpszOldFileName = IgnoreSpace( lpszOldFileName );

	// 新文件名是否有效
	if( IsValidFilePathName( lpszNewFileName ) == 0 )
	{		
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );//进入冲突段
	//是否系统已经打开老文件
	if( FindOpened( lpVol, lpszOldFileName ) != NULL )
	{	// 已打开，错误返回
		SetLastError(ERROR_SHARING_VIOLATION);
		goto _RET;
	}
	//是否系统已经打开新文件
	if( FindOpened( lpVol, lpszNewFileName ) != NULL )
	{	// 已打开，错误返回
		SetLastError(ERROR_SHARING_VIOLATION);
		goto _RET;
	}	
	// 初始化搜索对象
	if( BeginSearch( &sfNew, lpVol, 0, (LPTSTR)lpszNewFileName ) )
	{   
		bRetv = SearchNode( &sfNew );	//搜索新文件
		if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{	// 初始化搜索对象
			if( BeginSearch( &sfOld, lpVol, 0, (LPTSTR)lpszOldFileName ) )
			{
				bRetv = SearchNode( &sfOld );//搜索老文件
				if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
				{	//老文件存在
					DWORD dwSector;
					// 交换节点信息
					dwSector = sfNew.pCurNode->dwStartSector;					
					sfNew.pCurNode->dwStartSector = sfOld.pCurNode->dwStartSector;
					sfNew.pCurNode->nFileLength = sfOld.pCurNode->nFileLength;

					if( DoNodeUpdate( &sfNew, sfNew.pCurNode ) )	//更新节点信息
					{					
						NODE nodeNew, nodeOld;

						nodeNew = *sfNew.pCurNode;// 为下面的DoShellNotify准备数据
						nodeOld = *sfOld.pCurNode;// 为下面的DoShellNotify准备数据

						if( dwSector != NULL_SECTOR )   // 释放新节点过去的数据
						    FreeSector( lpVol,  dwSector );
						DoNodeDel( &sfOld, FALSE ); //删除老节点
						// 搜索对象占用了许多内存，先释放它，是为了保证 DoUpdateVolume 的内存要求
						EndSearch( &sfOld );	//释放搜索对象
						EndSearch( &sfNew );	//释放搜索对象

						DoUpdateVolume( lpVol ); // 更新卷
						//通知系统文件已经改变
						DoShellNotify( lpVol, SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszNewFileName, NULL, &nodeNew );
						DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszOldFileName, NULL, &nodeOld );
						bRetv = TRUE;
						goto _RET;
					}
				}
				EndSearch( &sfOld );//释放搜索对象
			}
		}
		EndSearch( &sfNew );//释放搜索对象
		bRetv = FALSE;//失败
	}

_RET:

	LeaveCriticalSection( &lpVol->csNode );  // LN, 2003-06-03, ADD

	return bRetv;
}

// **************************************************
// 声明：static BOOL _CopySectors( 
//							LPKFSDVOL lpVol, 
//							DWORD dwSectorFrom, 
//							DWORD dwSectorTo, 
//							DWORD dwSize,
//							void * lpSectorBuf0,   // temp buf
//							void * lpSectorBuf1     // temp buf
//							)

// 参数：
//	IN lpVol - 卷对象
//	IN dwSectorFrom - 源扇区
//	IN dwSectorTo - 目标扇区
//	IN dwSize - 需要拷贝的大小（应该是扇区长度的倍数）
//	IN lpSectorBuf0 - 用于读取扇区数据的临时缓存
//	IN lpSectorBuf1 - 用于读取扇区数据的临时缓存
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	将数据从源扇区拷贝到目标扇区
// 引用: 
// **************************************************

static BOOL _CopySectors( LPKFSDVOL lpVol, 
						  DWORD dwSectorFrom, 
						  DWORD dwSectorTo, 
						  DWORD dwSize,
						  void * lpSectorBuf0,   // temp buf
						  void * lpSectorBuf1     // temp buf
						  )
{
	DWORD dwSectorSize = lpVol->fdd.nBytesPerSector;
	DWORD dwOffset;

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

		// 得到源下一个扇区 get next sector
		dwSectorFrom = GetSectorOffset( lpVol, dwSectorFrom, 1 );
		// 得到目标下一个扇区
		dwSectorTo = GetSectorOffset( lpVol, dwSectorTo, 1 );

		dwOffset += dwSectorSize;
	}

	return (dwOffset >= dwSize);
}

// **************************************************
// 声明：BOOL KFSD_CopyFile(
//						PVOL pVol,
//						LPCTSTR lpExistingFileName,
//						LPCTSTR lpNewFileName,
//						BOOL bFailIfExists
//						)
// 参数：
// 	IN pVol - 卷对象
// 	IN lpExistingFileName – 源文件名（必须存在）
// 	IN lpNewFileName – 目标文件名（可以存在也可以不存在）
// 	IN bFailIfExists – 假如目标文件存在，是否该函数失败。
//						假如为TRUE并且目标文件已经存在，该函数失败；
//						假如为FALSE并且目标文件已经存在，该函数将清除目标文件的内容并将源文件内容拷贝到目标文件
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将文件内容从源文件拷贝到目标文件
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_CopyFile(
                     PVOL pVol,
					 LPCTSTR lpExistingFileName,
					 LPCTSTR lpNewFileName,
					 BOOL bFailIfExists
					 )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //do somthing here
	SF sf, sfNew;
	BOOL bExistFile, bRetv = FALSE;
	
	DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CopyFile lpExistingFileName=%s,lpNewFileName=%s,bFailIfExists=%d.\r\n"),lpExistingFileName,lpNewFileName,bFailIfExists));

	// 检查卷是否可写 ？check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// 错误。error 
		return FALSE;
	}

	// 卷是否可写？ check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // 不可写 error 
		//WARNMSG( FILE_ZONE, ( "error in KFSD_CopyFile: ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}
	//忽略文件前面的空格
	lpExistingFileName = IgnoreSpace( lpExistingFileName );
	//忽略文件前面的空格
	lpNewFileName = IgnoreSpace( lpNewFileName );
	//检查文件名是否合法
	if( IsValidFilePathName( lpNewFileName ) == 0 )
	{
		WARNMSG( FILE_ZONE, ( "error in KFSD_CopyFile: not valid file path name.\r\n" ) );
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );	// 进入冲突段

	//是否系统已经打开新文件
	if( FindOpened( lpVol, lpNewFileName ) != NULL )
	{	// 已打开，错误返回
		SetLastError(ERROR_SHARING_VIOLATION);
		goto RETV;
	}	

	//初始化搜索对象
	if( BeginSearch( &sfNew, lpVol, NULL, (LPTSTR)lpNewFileName ) )
	{
		if( (bExistFile = SearchNode( &sfNew )) )	//搜索新文件
		{	//新文件存在
			if( bFailIfExists || (sfNew.pCurNode->wAttrib&FILE_ATTRIBUTE_DIRECTORY) )
			{	// 如果是文件夹 或 bFailIfExists为TRUE,则退出
				WARNMSG( 1, ( "error in KFSD_CopyFile: new file (%s) exist or is a directory !.\r\n", lpNewFileName ) );
				EndSearch( &sfNew );//释放搜索对象
				goto RETV;
			}
		}
		//初始化搜索对象
	    if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpExistingFileName ) )
		{	//
			if( SearchNode( &sf )  )	//搜索老文件
			{   // 发现老文件 find the file
				PNODE pNode, pNodeNew;
				pNode = sf.pCurNode;
				pNodeNew = sfNew.pCurNode;
				if( ( pNode->wAttrib&(FILE_ATTRIBUTE_DIRECTORY) ) == 0 )
				{   // 非文件夹文件， not directory, is file
					DWORD dwStartSector = NULL_SECTOR;

					if( bExistFile )
					{   // 新文件已经存在，重新为新文件分配文件大小
						// 新文件文件大小为老文件大小。the new file is exist, now extend it's space
					    dwStartSector = ReallocSector( lpVol, pNodeNew->dwStartSector, ( (pNode->nFileLength + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector) );
					}
					else
					{	// 新文件不存在，需要增加一个新的

						memset( pNodeNew, 0, MAX_NODE_BYTES );
						memcpy( pNodeNew, pNode, GET_NODE_ATTRIB_SIZE(pNode) );//LN:2003-04-30，增加
						memcpy( pNodeNew->strName, sfNew.lpszCurName, sfNew.dwCurNameLen );//LN:2003-04-30，增加
						pNodeNew->bNameLen = (BYTE)sfNew.dwCurNameLen;//LN:2003-04-30，增加
						pNodeNew->dwStartSector = NULL_SECTOR;
						pNodeNew->nFileLength = 0;
						// 增加文件节点
                        if( DoNodeAdd( &sfNew, pNodeNew ) )
						{	// 为该新文件分配大小
							dwStartSector = ReallocSector( lpVol, pNodeNew->dwStartSector, ( (pNode->nFileLength + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector) );
							if( dwStartSector == NULL_SECTOR )
							{	// 失败，删除刚增加的文件节点
								DoNodeDel( &sfNew, FALSE );
							}
						}
					}
					//
					if( dwStartSector != NULL_SECTOR )
					{	// 新文件准备成功，现在拷贝文件数据
						// copy source sector to target sector
						if( _CopySectors( lpVol, pNode->dwStartSector, dwStartSector, pNode->nFileLength, sf.pSector, sfNew.pSector ) )
						{
							pNodeNew->wAttrib = pNode->wAttrib;
							pNodeNew->ft = pNode->ft;
							pNodeNew->dwStartSector = dwStartSector;
							pNodeNew->nFileLength = pNode->nFileLength;
							
							bRetv = DoNodeUpdate( &sfNew, pNodeNew );
						}
						else if( !bExistFile )
						{	// 拷贝失败，删除节点
							DoNodeDel( &sfNew, TRUE );
						}
						DoUpdateVolume( lpVol );	//更新卷
					}
				}
			}
			else
			{
				WARNMSG( 1, ( "error in KFSD_CopyFile: file not find(%s)!.\r\n", lpExistingFileName ) );
			}
			EndSearch( &sf );  // 释放搜索对象
		}
		EndSearch( &sfNew );// 释放搜索对象
	}
	
RETV:
	LeaveCriticalSection( &lpVol->csNode );	// 离开冲突段

	return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_SetEndOfFile( PFILE pf )
// 参数：
// 	IN pf - 文件对象
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件结束位置
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_SetEndOfFile( PFILE pf )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here    
    DWORD fs = lpFile->dwFilePointer;
	BOOL bRetv = FALSE;

    DEBUGMSG(FILE_ZONE, (TEXT("KFSD: SetEndOfFile.\r\n")));
	// 文件是否可写? 增加写检查
	if( (lpFile->dwAccess & GENERIC_WRITE) &&
		lpFile->flag == NORMAL_FILE )
	{
		EnterCriticalSection( &lpFile->lpfn->csFileNode ); // 进入冲突段

		if( fs != GET_NODE_PTR(lpFile)->nFileLength )	//新文件大小与当前文件实际大小不一致吗 ？
			bRetv = ReallocFile( lpFile, fs ); // 需要重新分配文件大小
		else
			bRetv = TRUE;//同样大小，不需要做什么
        if( bRetv )
		{	// 设定新文件大小
            GET_NODE_PTR(lpFile)->nFileLength = fs;
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );//离开冲突段
		    DoUpdateVolume( lpFile->lpVol );//更新卷
		}
		else
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );//离开冲突段
		lpFile->uiCacheValid = FALSE;//无效文件cache
	}
	return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_SetFileTime(
//						PFILE pf, 
//						const FILETIME *pCreate,
//						const FILETIME *pLastAccess,
//						const FILETIME *pLastWrite )
// 参数：
// 	IN pf - 文件句柄
//	IN pCreate-FILETIME结构指针，文件创建时间
//	IN pLastAccess-FILETIME结构指针，文件存取时间(目前不支持，为NULL)
//	IN pLastWrite-FILETIME结构指针，文件更新时间(目前不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	设置文件时间
// 引用: 
//	系统API
// **************************************************
//
BOOL KFSD_SetFileTime(
                     PFILE pf, 
                     const FILETIME *pCreate,
					 const FILETIME *pLastAccess,
					 const FILETIME *pLastWrite )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here
	
	if( lpFile->flag == NORMAL_FILE )
	{
		if( pCreate )
		{
			EnterCriticalSection( &lpFile->lpfn->csFileNode );//进入冲突段
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: SetFileTime, hi=0x%x, lo=0x%x\r\n"), pCreate->dwHighDateTime, pCreate->dwLowDateTime));
			GET_NODE_PTR(lpFile)->ft.dwLowDateTime = pCreate->dwLowDateTime;
			GET_NODE_PTR(lpFile)->ft.dwHighDateTime = pCreate->dwHighDateTime;
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );//离开冲突段
			return TRUE;
		}
	}

    return FALSE;
}

// **************************************************
// 声明：static BOOL CloseVolume( LPKFSDVOL lpVol )
// 参数：
// 	IN lpVol - 卷对象
// 返回值：
//	假如成功, 返回TRUE; 否则，返回FALSE
// 功能描述：
//	关闭卷
// 引用: 
// **************************************************

static BOOL CloseVolume( LPKFSDVOL lpVol )
{
    //do somthing here
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CloseVolume.\r\n")));

    DoUpdateVolume( lpVol );//先将卷数据更新到磁盘

    if( RemoveVolume( lpVol ) )	//移出该卷对象
    {
        DeleteVolume( lpVol );	//删除卷对象
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL KFSD_CloseVolume( PVOL pVol )
// 参数：
// 	IN lpVol - 卷对象
// 返回值：
//	假如成功, 返回TRUE; 否则，返回FALSE
// 功能描述：
//	关闭卷
// 引用: 
//	文件系统驱动界面
// **************************************************

BOOL KFSD_CloseVolume( PVOL pVol )
{    
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
	if( lpVol )
	    return CloseVolume( (LPKFSDVOL)pVol );
	return FALSE;
}

// **************************************************
// 声明：BOOL KFSD_CreateDirectory( 
//							PVOL pVol, 
//							LPCTSTR lpszPathName,
//							PSECURITY_ATTRIBUTES pSecurityAttributes)
// 参数：
//	lpcszPath-目录路径名
//	lpSecurityAttributes-安全描述(不支持，为NULL)
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	创建文件夹
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_CreateDirectory( 
                         PVOL pVol, 
                         LPCTSTR lpszPathName,
						 PSECURITY_ATTRIBUTES pSecurityAttributes)
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //  do somthing here
	SF sf;
	int bRetv = FALSE;

    DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CreateDirectory: %s.\r\n"), lpszPathName));

	// 检查卷是否可写 ？check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// 错误。error 
		return FALSE;
	}

	// 检查卷是否可以写, check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{   // error 
		//WARNMSG( FILE_ZONE, ( "error in KFSD_CreateDirectory: ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}
	//
	// 忽略文件前的空格
	lpszPathName = IgnoreSpace( lpszPathName );
	// 文件名是否有效
	if( IsValidFilePathName( lpszPathName ) == 0 )
	{		
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode ); // 进入冲突段
	// 初始化搜索对象
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszPathName ) )
	{			
		bRetv = SearchNode( &sf );	// 搜索文件节点
		if( bRetv == FALSE )
		{	// 未发现该文件
			SYSTEMTIME st;
			// 初始化文件节点数据
			memset( sf.pCurNode, 0, sizeof( NODE ) );
			sf.pCurNode->dwStartSector = NULL_SECTOR;
			sf.pCurNode->wAttrib = FILE_ATTRIBUTE_DIRECTORY;
			memcpy( sf.pCurNode->strName, sf.lpszCurName, sf.dwCurNameLen );
			sf.pCurNode->bNameLen = (BYTE)sf.dwCurNameLen;				
			GetSystemTime( &st );
		    SystemTimeToFileTime( &st, &sf.pCurNode->ft );
			// 增加一个文件夹节点
			bRetv = DoFolderChange( &sf, sf.pCurNode, OP_ADD );
			if( bRetv )
			{	// 假如成功，更新卷对象数据
				DoUpdateVolume( lpVol );
				// 向系统通知改变消息
				DoShellNotify( lpVol, SHCNE_MKDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszPathName, NULL, sf.pCurNode );
			}
		}
		else
		{	// 文件夹已经存在，退出
			SetLastError(ERROR_ALREADY_EXISTS);
			bRetv = FALSE;
		}
		EndSearch( &sf );//释放搜索对象
	}
	
	LeaveCriticalSection( &lpVol->csNode ); // 离开冲突段

	return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_DeviceIoControl(
//					DWORD pf,
//					DWORD dwIoControlCode,
//					LPVOID lpInBuf,
//					DWORD nInBufSize,
//					LPVOID lpOutBuf,
//					DWORD nOutBufSize,
//					LPDWORD lpBytesReturned
//    )

// 参数：
//	IN pf-文件对象
//	IN dwIoControlCode-I/O控制命令，文件系统包含：
//			IOCTL_DISK_SCAN_VOLUME－通知文件系统去扫描卷的错误
//			IOCTL_DISK_FORMAT_VOLUME－通知文件系统去格式化卷
//	IN lpInBuf-输入缓存
//	IN nInBufSize－输入缓存的尺寸
//	OUT lpOutBuf－输出缓存
//	IN nOutBufSize－输出缓存的尺寸
//	OUT lpBytesReturned-返回的数据的尺寸，该数据在返回时已存入lpOutBuf
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	文件系统控制管理
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_DeviceIoControl(
    DWORD pf,
    DWORD dwIoControlCode,
    LPVOID pInBuf,
    DWORD nInBufSize,
    LPVOID pOutBuf,
    DWORD nOutBufSize,
    LPDWORD pBytesReturned
    )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;

	DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: DeviceIoControl code(%d).\r\n"), dwIoControlCode ) );
	switch( dwIoControlCode )
	{
	case  IOCTL_DISK_FORMAT_VOLUME: // 格式化磁盘 format disk
		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: DeviceIoControl - Format Volume.\r\n")) );
		if( lpFile->lpVol->lpOpenFile == lpFile && 
			lpFile->lpNext == NULL &&
			AccessTest( lpFile->lpVol ) )
			//(lpFile->lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) == 0 )
		{	// 该卷没有别的文件被打开/使用
			TCHAR szName[1];
			NODE node;
			
			DoVolumeFormat( lpFile->lpVol );
			memset( &node, 0, sizeof( node ) );
			szName[0] = 0;
			// 通知系统改变信息
			node.wAttrib = FILE_ATTRIBUTE_DIRECTORY;
			DoShellNotify( lpFile->lpVol, SHCNE_DRIVEREMOVED, SHCNF_PATH | SHCNF_FLUSHNOWAIT, szName, NULL, &node );
			DoShellNotify( lpFile->lpVol, SHCNE_DRIVEADD, SHCNF_PATH | SHCNF_FLUSHNOWAIT, szName, NULL, &node );
			return TRUE;
		}
		else
		{
			SetLastError(ERROR_FILE_EXISTS);// printf error message
			WARNMSG(FILE_ERROR, (TEXT("KFSD: DeviceIoControl - No ALL FILE CLOSED\r\n")));
		}
		break;
    case  IOCTL_DISK_READONLY:
		//	设置磁盘为只读（内部使用）
		//lpFile->lpVol->fdd.dwFlags |= DISK_INFO_FLAG_READONLY;
		//	设置磁盘为只读（内部使用）
		if( pInBuf && nInBufSize == sizeof(DWORD) )
		{
			if( *((LPDWORD)pInBuf) == READONLY_SET )
				lpFile->lpVol->fdd.dwFlags |= DISK_INFO_FLAG_READONLY;
			else if( *((LPDWORD)pInBuf) == READONLY_CLEAR )
			{
				lpFile->lpVol->fdd.dwFlags &= ~DISK_INFO_FLAG_READONLY;
			}
			else
			{
				SetLastError( ERROR_INVALID_PARAMETER);
				return FALSE;
			}
		}
		return TRUE;
	}

	return FALSE;
}

// **************************************************
// 声明：BOOL KFSD_FindClose( PFIND pfd )
// 参数：
// 	IN pfd - 查找对象
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	关闭查找对象
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_FindClose( PFIND pfd )
{
    LPKFSDFIND lpfd = (LPKFSDFIND)pfd;
    //  do somthing here
	// 从链表移出对象
	RemoveFindHandleFromVolume( lpfd );
	// 释放对象的内存
    free( lpfd );
    return TRUE;
}

// **************************************************
// 声明：HANDLE KFSD_FindFirstFile( 
//                         PVOL pVol, 
//                         HANDLE hProc, 
//                         LPCTSTR lpszFileSpec, 
//                         FILE_FIND_DATA *  pfd )
// 参数：
// 	IN pVol - 卷对象
// 	IN hProc - 该对象进程拥有者句柄
// 	IN lpszFileSpec - 文件名（可以含通配字符）
//	OUT pfd - FILE_FIND_DATA数据结构指针，用于接收找到的数据
// 返回值：
//	成功：返回有效的句柄
//	否则：返回INVALID_HANDLE_VALUE
// 功能描述：
//	查找匹配的文件
// 引用: 
//	系统API
// **************************************************

HANDLE KFSD_FindFirstFile( 
                         PVOL pVol, 
                         HANDLE hProc, 
                         LPCTSTR lpszFileSpec, 
                         FILE_FIND_DATA *  pfd )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //  do somthing here
    KFSDFIND * lpsf;
	WORD fLen;
	int bRetv;
	
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile: %s.\r\n"), lpszFileSpec ));   
	// 忽略文件名前的空格
	lpszFileSpec = IgnoreSpace( lpszFileSpec );
    fLen = strlen( lpszFileSpec );
	if( fLen == 0 )
		return INVALID_HANDLE_VALUE;
    // 分配查找对象
	lpsf = (KFSDFIND *) malloc( sizeof( KFSDFIND ) + (fLen + 1)*sizeof( TCHAR ) );
	memset( pfd, 0, sizeof( FILE_FIND_DATA ) );
    if( lpsf )
    {        
        lpsf->lpszName = (LPTSTR)(lpsf+1);
        // this code is now check the file 
		strcpy( lpsf->lpszName, lpszFileSpec );
		// 初始化搜索对象
		if( BeginSearch( lpsf, lpVol, hProc, lpsf->lpszName ) )
        {	//
			bRetv = SearchNode( lpsf );//搜索文件对象
			if( bRetv )
			{	// 发现文件
				DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile:Has Found.\r\n")));
				// if all is ok, do this...
				memset( pfd, 0, sizeof( FILE_FIND_DATA ) );
				
				DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile: node=%s.\r\n"), lpsf->pCurNode->strName ));
				
				strncpy( pfd->cFileName, lpsf->pCurNode->strName, lpsf->pCurNode->bNameLen );
				// 初始化 pfd 结构
				pfd->dwFileAttributes = lpsf->pCurNode->wAttrib;
				pfd->ftCreationTime.dwLowDateTime = lpsf->pCurNode->ft.dwLowDateTime;
				pfd->ftCreationTime.dwHighDateTime = lpsf->pCurNode->ft.dwHighDateTime;
				*(&pfd->ftLastAccessTime)= *(&pfd->ftCreationTime);
				*(&pfd->ftLastWriteTime) = *(&pfd->ftCreationTime);
				pfd->nFileSizeLow = lpsf->pCurNode->nFileLength;
				pfd->nFileSizeHigh = 0;
				
				EndSearch( lpsf );	// 释放搜索对象
				
				// 将搜索对象加入搜索对象链表 add to volume list
				AddFindHandleToVolume( lpsf );
				return lpsf;
            }
           	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile:not Found。\r\n")));   
			EndSearch( lpsf );// 释放搜索对象
        }
		// 错误，释放之前已分配的查找对象
        free( lpsf );
    }
    return INVALID_HANDLE_VALUE;
}

// **************************************************
// 声明：BOOL KFSD_FindNextFile( 
//                      PFIND pfd,
//                      FILE_FIND_DATA * pffd ) 
// 参数：
// 	IN pfd - 查找对象
//	OUT pffd - FILE_FIND_DATA数据结构指针，用于接收找到的数据
// 返回值：
//	成功：返回TRUE
//	否则：返回FALSE
// 功能描述：
//	查找下一个匹配的文件
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_FindNextFile( 
                      PFIND pfd,
                      FILE_FIND_DATA * pffd ) 
{
    LPKFSDFIND lpfd = (LPKFSDFIND)pfd;
    BOOL retv = FALSE;

    //  do somthing here
	// 初始化搜索对象
	if( BeginSearch( lpfd, lpfd->lpVol, lpfd->hProc, lpfd->lpszName ) )
    {	// 搜索下一个文件
		if( TRUE == (retv = SearchFolder( lpfd, TRUE, ALL_NODES )) )   // search next node
		{	//找到符合要求的，初始化pffd结构
			memset( pffd, 0, sizeof( FILE_FIND_DATA ) );
            strncpy( pffd->cFileName, lpfd->pCurNode->strName, lpfd->pCurNode->bNameLen );
			//
			pffd->dwFileAttributes = lpfd->pCurNode->wAttrib;
			pffd->ftCreationTime.dwLowDateTime = lpfd->pCurNode->ft.dwLowDateTime;
			pffd->ftCreationTime.dwHighDateTime = lpfd->pCurNode->ft.dwHighDateTime;
			*(&pffd->ftLastAccessTime)= *(&pffd->ftCreationTime);
			*(&pffd->ftLastWriteTime) = *(&pffd->ftCreationTime);
			pffd->nFileSizeLow = lpfd->pCurNode->nFileLength; 
			pffd->nFileSizeHigh = 0;
			
			DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindNextFile: FileName=%s, Size=%d.\r\n"), pffd->cFileName, pffd->nFileSizeLow ));
		}
		else
			SetLastError(ERROR_NO_MORE_FILES);  // 没有找到
        EndSearch( lpfd ); // 释放搜索结构
    }
    return retv;
}

// **************************************************
// 声明：BOOL KFSD_GetDiskFreeSpace(
//                          PVOL pVol,
//                          LPCTSTR lpszPathName,
//                          LPDWORD pSectorsPerCluster,
//                          LPDWORD pBytesPerSector,
//                          LPDWORD pFreeClusters,
//                          LPDWORD pClusters )
// 参数：
// 	IN pVol - 卷对象
//	IN pSectorsPerCluster - 用于接受每个簇的扇区数
//	IN pBytesPerSector - 用于接受每个扇区的字节数
//	IN pFreeClusters - 用于接受当前磁盘空闲的簇数
//	IN pClusters - 用于接受当前磁盘总的簇数
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到当前卷的磁盘使用情况
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_GetDiskFreeSpace(
                          PVOL pVol,
                          LPCTSTR lpszPathName,
                          LPDWORD pSectorsPerCluster,
                          LPDWORD pBytesPerSector,
                          LPDWORD pFreeClusters,
                          LPDWORD pClusters )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //  do somthing here
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: GetDiskFreeSpace-%s\r\n"), lpszPathName));

    lpszPathName = IgnoreSpace( lpszPathName );

	*pSectorsPerCluster = 1;
	*pBytesPerSector = lpVol->fdd.nBytesPerSector;
	*pFreeClusters = lpVol->rfh.nFreeCount;
	*pClusters = lpVol->fdd.nSectors;

    return TRUE;
}

// **************************************************
// 声明：static BOOL IsValidDisk( PRFH prfh, FSD_DISK_DATA * pfdd )
// 参数：
// 	IN prfh - 文件系统头结构
// 	IN pfdd - FSD_DISK_DATA结构指针，包含磁盘信息
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	检查在磁盘上的文件系统是否有效
// 引用: 
// **************************************************

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

// **************************************************
// 声明：BOOL KFSD_MountDisk( HDSK hDsk )
// 参数：
// 	IN hDsk - 磁盘对象句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	按装磁盘卷
// 引用: 
//	系统API
// **************************************************
#define DEBUG_KFSD_MountDisk 0
BOOL KFSD_MountDisk( HDSK hDsk )
{
    BOOL retv = FALSE;
    LPKFSDVOL lpVol;
	
	if( FindVolume( hDsk ) )
	{	// 该磁盘卷已经被使用
		WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: Find Volume in Mount, return.\r\n") );
		return FALSE;
	}
	// 创建新卷
	lpVol = (LPKFSDVOL)CreateVolume();
    if( lpVol )
    {
		if( ERROR_SUCCESS == FSDMGR_GetDiskInfo( hDsk, &lpVol->fdd ) )
        {
            WORD * pSector;
            DWORD dwfat;
            PRFH prfh;

			DEBUGMSG( DEBUG_KFSD_MountDisk, ("DiskInfo: nBytesPerSector=%d,nCylinders=%d,nHeadsPerCylinder=%d,nSectors=%d,nSectorsPerTrack=%d\r\n", 
				 lpVol->fdd.nBytesPerSector,
				 lpVol->fdd.nCylinders,
				 lpVol->fdd.nHeadsPerCylinder,
				 lpVol->fdd.nSectors,
				 lpVol->fdd.nSectorsPerTrack ) );

            if( lpVol->fdd.nBytesPerSector < 128 )
            {
				ERRORMSG( DEBUG_KFSD_MountDisk, ("KFSD: Error: sector size(%d) < 128.\r\n", lpVol->fdd.nBytesPerSector) );
                DeleteVolume( lpVol ); // 释放之前分配的卷对象
                return FALSE;
            }
            lpVol->hDsk = hDsk;

            dwfat = ( ( (lpVol->fdd.nSectors * sizeof( DWORD ) + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector ) * lpVol->fdd.nBytesPerSector );
			DEBUGMSG( DEBUG_KFSD_MountDisk, ( "KFSD: Alloc FAT size=%d.\r\n", dwfat ) );
			// 为FAT分配内存
            lpVol->lpdwFat = (DWORD*)malloc( dwfat );
            if( lpVol->lpdwFat )
            {   // 为卷读写扇区分配缓存
				DEBUGMSG( DEBUG_KFSD_MountDisk, ( "KFSD: Alloc pSector.\r\n" ) );
                lpVol->lpSectorBuf = pSector = (WORD*)malloc( lpVol->fdd.nBytesPerSector );				
                if( pSector )
                {	// 读卷头(放在0扇区)
                    if( ERROR_SUCCESS == FSDMGR_ReadDisk( hDsk, 0, 1, (LPBYTE)pSector, lpVol->fdd.nBytesPerSector ) )
                    {
                        DEBUGMSG( DEBUG_KFSD_MountDisk, (TEXT("KFSD: Read 0 Sector Success.\r\n") ) );
						prfh = (PRFH)pSector;
						if( IsValidDisk( prfh, &lpVol->fdd ) )
                        {   // 有效的KMFS 文件系统
                            memcpy( &lpVol->rfh, prfh, sizeof( RAM_FILE_HEAD ) );
							// 读FAT数据
                            FSDMGR_ReadDisk( hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (LPBYTE)lpVol->lpdwFat, lpVol->fdd.nBytesPerSector * lpVol->rfh.dwFatSectorNum );
							DEBUGMSG( DEBUG_KFSD_MountDisk, ( "KFSD: valid kingmos file system.\r\n" ) );
                        }
                        else
                        {   // 非 KMFS 文件系统，格式化磁盘。no esoft ram file sys or other file sys or has some error
							WARNMSG( DEBUG_KFSD_MountDisk, ( "KFSD: invalid kingmos file sys, now to format it.\r\n" ) );
							DoVolumeFormat( lpVol );
                        }
                        
				        DEBUGMSG( DEBUG_KFSD_MountDisk, ("KFSD: FAT INFO:freecount=%d,ver=%d,data=%d,fatnum=%d,freesector=%d,serial=%x\r\n", lpVol->rfh.nFreeCount,lpVol->rfh.ver,lpVol->rfh.dwDataStartSector,lpVol->rfh.dwFatSectorNum,lpVol->rfh.dwFreeSector, lpVol->rfh.dwSerialNum ) );
						// 想文件系统管理器注册卷对象
						lpVol->hVol = FSDMGR_RegisterVolume( hDsk, NULL, (DWORD)lpVol );
                        if( lpVol->hVol )
                        {
                            DEBUGMSG(DEBUG_KFSD_MountDisk, (TEXT("KFSD: RegisterVolume Sucess.\r\n")));
							// 得到卷名
							FSDMGR_GetVolumeName( lpVol->hVol, lpVol->szVolName, sizeof( lpVol->szVolName ) );
							lpVol->nNodesPerSector = (WORD)( (lpVol->fdd.nBytesPerSector / NODE_ALIGN_SIZE) );
							// 加入KMFS 文件系统卷链表
                            AddToVolumeList( lpVol );
							DEBUGMSG(DEBUG_KFSD_MountDisk, ( "KFSD: KFSD_MountDisk return TRUE.\r\n" ) );
                            return TRUE;
                        }
                        else
                        {
                        	WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: error Can't RegisterVolume.\r\n") );
                        }
                    }
                    else
                    {   
						ERRORMSG(DEBUG_KFSD_MountDisk, ( "KFSD error: KFSD_MountDisk read disk error!!!.\r\n" ) );// error to readdisk
                    }
                }
                else
                {
                	WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: error Can't alloc sector memory.\r\n") );
                }
            }
            else
            {
    			WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: error Can't alloc fat tabel memory.\r\n") );
            }
        }
        else
        {
    		WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: error Can't GetDiskInfo.\r\n") );        	
        }
        DeleteVolume( lpVol ); // 错误，释放卷
    }
    else
    {
    	WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: error Can't CreateVolume struct.\r\n") );
    }
    return FALSE;    
}

// **************************************************
// 声明：void KFSD_Notify(
//                PVOL pVol,
//                DWORD dwFlags )
// 参数：
// 	IN pVol - 卷对象
//	IN dwFlags - 通知消息
// 返回值：
//	无
// 功能描述：
//	该功能是与文件系统管理器之间进行各种控制的桥梁
// 引用: 
//	文件系统驱动程序接口
// **************************************************

void KFSD_Notify(
                PVOL pVol,
                DWORD dwFlags )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //  do somthing here
    switch( dwFlags )
    {
    case FSNOTIFY_POWER_ON:  // 开机事件
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: FSNOTIFY_POWER_ON\r\n" ) ) );
        break;
    case FSNOTIFY_POWER_OFF:	// 关机事件
		DoUpdateVolume( lpVol ); // 更新卷，将数据写入磁盘
        break;
    case FSNOTIFY_DEVICES_ON:   // the device has resumed after a suspended mode, and devices are now available for use
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: FSNOTIFY_DEVICES_ON\r\n") ) );
        break;
    }
}

// **************************************************
// 声明：BOOL KFSD_RegisterFileSystemFunction( 
//                                       PVOL pVol,
//                                       SHELLFILECHANGEFUNC pscf )
// 参数：
// 	IN pVol - 卷对象
//	IN pscf - 系统通知回调函数
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	注册文件系统系统通知回调函数
// 引用: 
//	文件系统驱动程序接口
// **************************************************

BOOL KFSD_RegisterFileSystemFunction( 
                                       PVOL pVol,
                                       SHELLFILECHANGEFUNC pscf )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    FILECHANGEINFO fci;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: RegisterFileSystemFunction=0x%x.\r\n"), pscf));

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

// **************************************************
// 声明：BOOL KFSD_RemoveDirectory( 
//                         PVOL pVol, 
//                         LPCTSTR lpszPathName )
// 参数：
// 	IN pVol - 卷对象
// 	IN lpszPathName - 文件夹名
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除文件夹
// 引用: 
//	系统API
// **************************************************

BOOL KFSD_RemoveDirectory( 
                         PVOL pVol, 
                         LPCTSTR lpszPathName )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //  do somthing here
	SF sf;   
	BOOL bRetv = FALSE;
	int len;

	DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory=%s.\r\n"), lpszPathName ) );

	// 卷是否可写。check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // 不可写。error 
		WARNMSG( FILE_ZONE, ( "error in KFSD_RemoveDirectory: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//

	// 忽略字符串前部的空格
	lpszPathName = IgnoreSpace( lpszPathName );
	len = strlen( lpszPathName );
	// 当前是否打开该文件夹
	if( FindOpened( lpVol, lpszPathName ) ) // LN: 2003-05-21
	{	// 已经打开
		SetLastError(ERROR_SHARING_VIOLATION);
		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory, FindOpened.\r\n") ));
		return FALSE;
	}
	// 当前是否打开该文件夹的搜索句柄
	if( FindOpenedSearchHandle( lpVol, lpszPathName ) )// LN: 2003-05-21
	{	// 有
		SetLastError(ERROR_SHARING_VIOLATION);
		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory, FindOpenedSearchHandle.\r\n") ));
		return FALSE;
	}

	DEBUGMSG( FILE_ZONE, ( TEXT( "KFSD: RemoveDirectory, BeginSearch.\r\n" ) ) );
	// 初始化搜索句柄
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszPathName ) )    
    {		
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory, SearchNode.\r\n") ) );
		if( SearchNode( &sf ) )	// 找到吗 ？
		{	// 找到
			DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory, SearchNode OK.\r\n") ));
			if( sf.pCurNode->wAttrib&FILE_ATTRIBUTE_DIRECTORY )
			{   
				DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory, Is dir, now call del.\r\n") ) );
				if( DoFolderChange( &sf, NULL, OP_DEL ) )	// 删除文件夹
				{
					DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory, UpdateValume del.\r\n") ) );
					// 更新卷到磁盘
					DoUpdateVolume( lpVol );
					// 向系统做通知消息
					DoShellNotify( lpVol, SHCNE_RMDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszPathName, NULL, sf.pCurNode );//&node );
					bRetv = TRUE;
				}
			}
			else
				SetLastError( ERROR_ACCESS_DENIED );
		}
		EndSearch( &sf );	// 释放搜索对象
	}
    DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory retv=%d\r\n"), bRetv));
    return bRetv;
}

// **************************************************
// 声明：BOOL KFSD_UnmountDisk( HDSK hDsk )
// 参数：
// 	IN hDsk - 磁盘对象句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	安装卷
// 引用: 
//	文件系统接口界面
// **************************************************

BOOL KFSD_UnmountDisk( HDSK hDsk )
{    
    LPKFSDVOL lpVol;
    
	DEBUGMSG( FILE_ZONE, ( TEXT( "++KFSD: UnmountDisk=%d\r\n" ), hDsk ) );
	//return FALSE; ///base file system, ??????	

    if( ( lpVol = FindVolume( hDsk ) ) != NULL )  // 当前系统是否存在没有关闭的文件/搜索句柄
    {   // 存在，关掉他们
		// 搜索链表
		while( lpVol->lpFindFile )
		{
			KFSD_FindClose( (PFIND)lpVol->lpFindFile );
		}
		// 文件链表
		while( lpVol->lpOpenFile )
		{
			KFSD_CloseFile( (PFILE)lpVol->lpOpenFile );
		}
		// 所有的都关掉，注销卷
		FSDMGR_DeregisterVolume( lpVol->hVol ); // 
		// 关闭卷
		CloseVolume( lpVol );
		return TRUE;
    }
	
	DEBUGMSG( FILE_ZONE, ( TEXT("--KFSD: UnmountDisk\r\n") ) );
	return FALSE;
}

// **************************************************
// 声明：
// 参数：
// 	IN
// 	OUT
// 	IN/OUT
// 返回值：
// 功能描述：
// 引用: 
// **************************************************

#ifdef WINCE_EML
BOOL APIENTRY  DllMain(HINSTANCE DllInstance, DWORD Reason, LPVOID Reserved)
{
    switch(Reason) {
        case DLL_PROCESS_ATTACH:
            RETAILMSG(FILE_ZONE, (TEXT("KFSD: DLL_PROCESS_ATTACH\r\n")));
			break;
        case DLL_PROCESS_DETACH:
            RETAILMSG(FILE_ZONE, (TEXT("KFSD: DLL_PROCESS_DETACH\r\n")));
            break;
    }
	return TRUE;
}
#endif

