/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵��������FAT�ļ�ϵͳ���õ��� struct�����������õ���ֻ�������йص����á�
�汾�ţ�1.0.0
����ʱ�ڣ�2003-4-18
���ߣ�κ������
�޸ļ�¼:
******************************************************/
#ifndef		_FAT_H_
#define		_FAT_H_

#ifndef		UNDER_CE

#include <ewindows.h>
#include <efile.h>
#include "efsdmgr.h"
#define	CP_ACP	CP_GB2312
#else

#include <windows.h>
#include "kimosfile.h"
#include "Myfsdmgr.h"

#ifdef	UNDER_CE

#include "Fsdmgr.h"

#define stricmp		_stricmp

#define SHELLFILECHANGEFUNC 	SHELLFILECHANGEFUNC_t

#else

//#define	 RETAILMSG		//

#endif

#endif

#define	WideStrLen  wcslen

#define		FAT_DEBUG

#define		DEBUG_ERROR	1

#define		DEBUG_NOTIFY	0
#define		DEBUG_FAT	0
#define		DEBUG_READ	0
#define		DEBUG_WRITE	0
#define		DEBUG_FIND	0
#define		DEBUG_FILE	0

#define		DEBUG_MOUNT	0


#define		FAT_FLUSH_DELAY
#define		CLUSTER_FLUSH_DELAY

#ifdef		CLUSTER_FLUSH_DELAY
#define		FLUSH_CLUSTER		0
#else
#define		FLUSH_CLUSTER		1
#endif

#ifdef		FAT_FLUSH_DELAY
#define		FLUSH_DELAY_TIME	5000
#endif


#define		ROOT_NAME			"\\Vol:"
#define		FILE_ATTR_VOLUME		0xFF

#define		DEFAULT_SECTOR_SIZE			512
#define		FORMAT_SECTOR_NUM			8	//sector number writed every time when formating the disk.
/*
#define		DISK_CACHE_SIZES		8   //in sectors.
#define		CLUSTER_CACHE_SIZES		4

#define		BURST_WRITE_CLUSTER		8
*/
#define		DISK_CACHE_SIZES		128   //in sectors.
#define		CLUSTER_CACHE_SIZES		16

#define		BURST_WRITE_CLUSTER		16

#define		FAT_MAKDWORD(hi, low)	((hi)<<16 | (low) )

#define		FAT_VERSION12		1
#define		FAT_VERSION16		2
#define		FAT_VERSION32		3

#define		LABEL_SIZE			11

typedef		char * PCHAR;

typedef	struct _VOULME_FAT		VOLUME_FAT,*PVOLUME_FAT ;

typedef struct	_SECTOR_CACHE{			//this struct is only used for buffer  FAT table.

	DWORD	dwStartSector;					//�Ѿ�������
	DWORD	dwSector[DISK_CACHE_SIZES];		//��ǰ���buffer��������ĸ�sector�����ݡ�
	DWORD	dwLastAccess[DISK_CACHE_SIZES]; //���һ�δ�ȡʱ�� 
	BOOL	bNeedFlush[DISK_CACHE_SIZES];	//���buffer�Ƿ���Ҫˢ�µ����̡�
	char*	pBuf;//[DISK_CACHE_SIZES*DEFAULT_SECTOR_SIZE];

}SECTOR_CACHE, *PSECTOR_CACHE;

typedef struct	_CLUSTER_CACHE{		//it's used for cache the clusters storing directory information.

	DWORD   dwStartCluster;
	DWORD	dwCluster[CLUSTER_CACHE_SIZES];
	DWORD	dwLastAccess[CLUSTER_CACHE_SIZES];
	BOOL	bNeedFlush[CLUSTER_CACHE_SIZES];
	char *	pBufStart;//cBuf[CACHE_SIZES*DEFAULT_SECTOR_SIZE];

}CLUSTER_CACHE, *PCLUSTER_CACHE;

typedef	struct _FILE_FAT{

	struct _FILE_FAT	*pNext;

	char	csName[MAX_PATH];
	DWORD	dwSize;		// size of the file, in bytes.

	DWORD	dwParentStart;			//��Ŀ¼���ݿ�ʼ��cluster.
	DWORD	dwClusterParent;		//��Ŀ¼���� cluster.
	DWORD	dwClusterOffset;		//�ļ�Ŀ¼��¼�ڡ��ļ���Ŀ¼cluster(dwParenCluster)�е�ƫ��(��sizeof(DIR ENTRY)��).
									//short name.
	DWORD	dwPointer;				//�ļ�ָ�롣
	DWORD	dwStartCluster;			//���ļ���ʼ�� cluster��š�
	DWORD	dwClusterCurrent;		//���ļ�ָ��ָ��� cluster��š����������£�������EOF��
	DWORD	dwLastValidClus;        //���ļ����һ���кϷ����ݵ� cluster��š���ֻ������Ҫ���ļ������µ�clusterʱ�����á�
									//Ψһ��;�ǽ��ļ����һ��cluster���·��䵽��cluster����һ��

	//DWORD	dwClusIndex;			//dwClusterCurrent Ϊ�ļ��ĵڼ���cluster.��0��ʼ��

	DWORD	dwAccess;
	DWORD	dwShareMode;

	PVOLUME_FAT	pVolume;

	DWORD		dwId;
	DWORD		dwAttr;
	FILETIME	ftCreate;
	FILETIME    ftLastAccess;
	FILETIME    ftLastWrite;
 
	char		*pFileCache;
	DWORD		dwCachedCluster;//pFileCache��ָ���buffer�����cluster.
	BOOL		bCacheNeedFlush;//pFileCache��ָ���buffer���ݱ����¹�����ҪFlush�����̡�

	DWORD	dwLongNameOffset;  //�ļ���֮���ļ�����¼��dwLongNameCluster�е�ƫ�ƣ��ԣ���sizeof(DIR ENTRY)�ƣ���
	DWORD	dwLongNameCluster; //�ļ���֮���ļ�����¼���ڵ�cluster.

}FILE_FAT,*PFILE_FAT;


typedef struct _FIND_RESULT{

	struct _FIND_RESULT	* pNext;
	//following three member are only associated with finding some files(ex.*.*) under a folder.
	char    csCondition[MAX_PATH]; //FindFirstFile.�����õ�ͨ�����
	DWORD	dwParentStart;			//���ļ���Ŀ¼���ݿ�ʼ��cluster.
	DWORD	dwCluster;
	DWORD   dwOffset; //the current pointer in the current cluster, in the number of DIR ENTRY.

	DWORD	dwLongNameOffset;  
	DWORD	dwLongNameCluster;

	PVOLUME_FAT	pVolume;
}FIND_RESULT, *PFIND_RESULT;

typedef struct	_FAT_INFO{

	DWORD	dwClusterCount; //����Cluster��ţ���FAT16�У���������dwRootDirClusterNum������ʵ�ʴ��̴�СʱҪ��ȥ�������
	DWORD	dwFreeCluster;

	DWORD	dwBytesPerSec;
	DWORD	dwSecPerCluster;
	DWORD	dwRsvedSector;
	DWORD	dwNumFAT;
	DWORD	dwRootEntryCount; //only in FAT16/12.
	DWORD	dwTotalSector;
	DWORD	dwFATSize;			// size of FAT table, in sectors.
	DWORD	dwRootCluster;
	//PVOLUME_FAT	pVolume;
}FAT_INFO ,*PFAT_INFO;


struct _VOULME_FAT{
	
	struct _VOULME_FAT * pNext;
	char	csVolName[20];

	HDSK	hDisk;

	DWORD   dwVersion;
	DWORD	dwFirstDataSec;
	DWORD   dwClusterSize;
	DWORD	dwRootDirSec;   //It should be identical with dwFirstDataSec in FAT32. 
							//in FAT16, it should be in the front of dwFirstDataSec, with a distance of RootEntry size.
	DWORD	dwRootDirClusterNum; //FAT16�д�ֵΪ����Ŀ¼��ռ�� cluster��Ŀ��ֻ��ҪĿ¼��ռ�Ŀռ��� cluster���������ű�֧�֣���
								//FAT32�����ֵΪ0����FAT16�У����ǽ���Ӳ���϶����� cluster��ֵ���� dwRootDirClusterNum,
	DWORD	dwFirstFreeCluster;

	FSD_DISK_DATA	fda;
	char		cLabel[12];
	FAT_INFO	fat_info;

	CRITICAL_SECTION		csVolCrit;

	SHELLFILECHANGEFUNC		FuncShellNotify;

	SECTOR_CACHE			scFatCache;
	CLUSTER_CACHE			ccClusterCache;
	
	PFIND_RESULT			pFindListHead;
	PFILE_FAT				pFileListHead;

	DWORD					dwLastWriteTime;
	BOOL					bFlushed;
	HVOL					hVolume;
#ifdef	FAT_FLUSH_DELAY
	HANDLE				hDelayThread;
	BOOL				bThreadRun;
#endif

};//VOLUME_FAT,*PVOLUME_FAT;


typedef	struct _DIR_ENTRY_FIND{

	char    csName[512];
	DWORD	cAttr;
	DWORD	cChecksum;
	//UCHAR	cAttr;
	//UCHAR	cChecksum;
	BOOL	bLongName;
	DWORD	dwCurEntryIndex;
	DWORD	bValid;
	DWORD	dwLongNameOffset;  
	DWORD	dwLongNameCluster;

}DIR_ENTRY_FIND, *PDIR_ENTRY_FIND;

typedef	struct	_APPEND_RECORD{

	WCHAR *  pwszLongName;
	char	csFileName[MAX_PATH];
	int		iLongNameLen;
	PUCHAR	pszShortName;

	DWORD   dwLastCluster;
	DWORD	dwCluster;
	DWORD	dwFileStartClus;
	DWORD	dwOffsetInCluster;
	DWORD	dwLongNameOffset;

	FILETIME	ftTime;
	DWORD	dwAttr;
	BOOL	bFile;

}APPEND_RECORD,*PAPPEND_RECORD;

#ifndef	UNDER_CE

BOOL FAT_MountDisk( HDSK hdsk );

BOOL FAT_CloseFile( PFILE pf );  
BOOL FAT_CloseVolume( PVOL pVol );  
BOOL FAT_CloseFile( PFILE pf );  
BOOL FAT_CloseVolume( PVOL pVol );  
BOOL FAT_CreateDirectory( PVOL pVol, LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes);  
HANDLE FAT_CreateFile( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileName, DWORD dwAccess, DWORD dwShareMode, PSECURITY_ATTRIBUTES pSecurityAttributes, DWORD dwCreate, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );  
BOOL FAT_DeleteAndRenameFile( PVOL pVol, LPCTSTR lpcszNewFileName, LPCTSTR lpcszOldFileName );  
BOOL FAT_DeleteFile( PVOL pVol, LPCTSTR lpcszFileName );  
BOOL FAT_DeviceIoControl( PFILE pf, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpdwBytesReturned );  
BOOL FAT_FindClose( PFIND pSearch );  
HANDLE FAT_FindFirstFile( PVOL pVol, HANDLE hProc, LPCTSTR lpcszFileSpec, FILE_FIND_DATA * pfd );  
BOOL FAT_FindNextFile( PFIND pFind, FILE_FIND_DATA * pfd );  
BOOL FAT_FlushFileBuffers( PFILE pf );  
BOOL FAT_GetDiskFreeSpace( PVOL pVol, LPCTSTR lpcszPathName, LPDWORD pSectorsPerCluster, LPDWORD pBytesPerSector, LPDWORD pFreeClusters, LPDWORD pClusters );  
DWORD FAT_GetFileAttributes( PVOL pVol, LPCTSTR lpcszFileName );  
BOOL FAT_GetFileInformationByHandle( PFILE pf, FILE_INFORMATION * pfi );  
DWORD FAT_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh );  
BOOL FAT_GetFileTime( PFILE pf, FILETIME * pCreate, FILETIME * pLastAccess, FILETIME * pLastWrite );  
BOOL FAT_MoveFile( PVOL pVol, LPCTSTR lpcszOldFileName, LPCTSTR lpcszNewFileName );  
void FAT_Notify( PVOL pVol, DWORD dwFlags );       
BOOL FAT_ReadFile( PFILE pf, LPVOID lpvBuffer, DWORD dwNumToRead, LPDWORD lpdwNumRead, LPOVERLAPPED lpOverlapped );  
BOOL FAT_ReadFileWithSeek(PFILE pf,LPVOID pBuffer,DWORD cbRead,LPDWORD pcbRead,LPOVERLAPPED pOverlapped,DWORD dwLowOffset,DWORD dwHighOffset);  
BOOL FAT_RegisterFileSystemFunction( PVOL pVol, SHELLFILECHANGEFUNC pft );  
BOOL FAT_RemoveDirectory( PVOL pVol, LPCTSTR lpcszPathName );   
BOOL FAT_SetEndOfFile( PFILE pf );  
BOOL FAT_SetFileAttributes( PVOL pVol, LPCTSTR lpcszFileName, DWORD dwFileAttributes );  
DWORD FAT_SetFilePointer( PFILE pf, LONG lDistanceToMove, LPLONG lpDistanceToMoveHigh, DWORD dwMoveMethod);  
BOOL FAT_SetFileTime( PFILE pf, const FILETIME * pCreate, const FILETIME * pLastAccess, const FILETIME * pLastWrite );  
BOOL FAT_UnmountDisk( HDSK hdsk );  
BOOL FAT_WriteFile( PFILE pf, LPCVOID lpvBuffer, DWORD dwNumToWrite, LPDWORD lpdwNumWrite, LPOVERLAPPED pOverlapped );  
BOOL FAT_WriteFileWithSeek( PFILE pf, LPCVOID lpcvBuffer, DWORD cbWrite, LPDWORD pcbWritten, DWORD dwLowOffset, DWORD dwHighOffset );  

BOOL FAT_CopyFile( PVOL pVol, LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists );  
#endif
/////////////////// **************************************************

HANDLE	FindFile( PVOLUME_FAT pVol,  char* lpcszFileSpec, FILE_FIND_DATA * pfd );
void InitializeFATSys( void  );
/////////////////// **************************************************8
// in mount.cpp
BOOL MountFatDisk( HDSK  hDisk );
BOOL IsVolume( PVOLUME_FAT pVolume );
BOOL	UnmountFatDisk( HDSK hDsk );
BOOL	InitializeCaches( PVOLUME_FAT pVol );
DWORD	CalculateFreeCluster( PVOLUME_FAT	pVol );
BOOL	FormatTheVolume( PVOLUME_FAT pVolume  );
//functions in fatdisk.cpp
BOOL	ReadCluster(PVOLUME_FAT pVol, DWORD dwCluster, PCHAR pBuf,DWORD dwBufSize);
DWORD  ReadWholeCluster( PVOLUME_FAT pVol, char *pBuf , DWORD dwFromClus, DWORD dwNumCluster );

BOOL	WriteClusterWithBuf(PVOLUME_FAT pVol, DWORD dwCluster, PCHAR pBuf);
BOOL	WriteCluster(PVOLUME_FAT pVol, DWORD dwIndex,BOOL bFlush);
BOOL	WriteFatSector(PVOLUME_FAT pVol, int iIndex);
DWORD	WriteWholeCluster(PVOLUME_FAT pVol, PCHAR  pBuf, PFILE_FAT	pFile, DWORD dwNum);

/////////// functions in misc.cpp
DWORD	GetNextCluster( PVOLUME_FAT pFatVol, DWORD dwCurCluster );
DWORD	GetContinuesCluster( PVOLUME_FAT	pVol, DWORD dwFromClus ,DWORD dwNum ,DWORD *pdwNextReadStart);
 unsigned char ShortNameCheckSum (unsigned char *pFcbName);
DWORD   CheckClusterCache( PVOLUME_FAT pVolume , DWORD dwCluster );
PUCHAR	CutCurrentPath( PUCHAR	pPath );
BOOL    IsPath(  char* pcsPath );
DWORD	FatAllocCluster( PVOLUME_FAT pVol ,DWORD dwNum, DWORD *lpdwNumAlloced);
BOOL	LinkCluster( PVOLUME_FAT	pVol, DWORD  dwCluster1, DWORD dwCluster2 );
PUCHAR	GetFileNameFromPath( PUCHAR	szFileName );
DWORD	FileGetCurrentCluster(PVOLUME_FAT pVol, DWORD	dwFromClus, DWORD dwClusWalk);
DWORD FileGetLastValidCluster( PVOLUME_FAT pVol, DWORD	dwFromClus );
BOOL	INVALID_CLUSTER(PVOLUME_FAT pVolume, DWORD dwCluster);
//functions in search.cpp
DWORD FindFileInDirectory( PVOLUME_FAT  pVol, DWORD dwCurCluster,  char* lpFile, FILE_FIND_DATA * pFindData, PFIND_RESULT pFind,BOOL bNeedLongName);
BOOL	DeleteSearchHandle( PVOLUME_FAT pVol, PFIND_RESULT pSearch  );

/// following function is in file.cpp
HANDLE CreateFatFile( PVOLUME_FAT pVol, char* lpcszFileName, DWORD dwAccess, 
					  DWORD dwShareMode, DWORD dwCreate,DWORD dwFlagsAndAttributes ,BOOL  bFile);

BOOL	DeleteFileHandle( PVOLUME_FAT pVol, PFILE_FAT	pFileDel  ,BOOL bClosed );
BOOL	SetFileAttributesByHandle( PVOLUME_FAT pVolFat, PFILE_FAT  pFileFat, DWORD dwAttr);
BOOL   IsFileHandle( PVOLUME_FAT pVol, PFILE_FAT   pFileJudge);
BOOL ReadFatFile( PFILE_FAT	pFile, char * pBuf, DWORD dwNumToRead, LPDWORD lpdwNumRead);
BOOL  FatSetFileTime( PFILE_FAT pFile, const FILETIME *pCreate, const FILETIME *pLastAccess,const FILETIME * pLastWrite );
BOOL	FatDeleteFileByHanle( HANDLE  hFile ,BOOL bDelete);
BOOL	FatFileSysMoveFile( PVOLUME_FAT pVolume, char* szOldFileName, char*  szNewFileName ) ;
BOOL FatFileSysFileCopy( PVOLUME_FAT pVol,  char* lpExistingFileName,  char* lpNewFileName, DWORD dwNewFileCreateFlag );// bFailIfExists );
BOOL	WriteFatFile( PFILE_FAT pFile, char* lpBuffer, DWORD dwNumToWrite, DWORD* lpdwNumRead);
BOOL	ReleaseClusterChain( PVOLUME_FAT pVolume , DWORD dwStartCluster );
BOOL	FatSetEndOfFile(  PFILE_FAT  pFile );
BOOL	FlushFileBuffer( PFILE_FAT	pFile );

///////////////////////
BOOL CreateFatDirectory( PVOLUME_FAT pVolume , char* lpcszPathName );
BOOL CreateNameRecord(PVOLUME_FAT pVolume, PAPPEND_RECORD pAppendRec);
//BOOL AddNameRec( PVOLUME_FAT pVolume ,PUCHAR lpszPathName, DWORD dwAttr, BOOL bFile ,PFILE_FAT  pFile);
BOOL AddNameRec( PVOLUME_FAT pVolume ,PUCHAR lpszPathName, 	PAPPEND_RECORD	pAppendRec,PFILE_FAT  pFile);

BOOL DeleteFatDirectory(PVOLUME_FAT pVolume, PUCHAR pszPathName);
BOOL IsSearch( PVOLUME_FAT pVol, PFIND_RESULT pSearch  );
void FlushFAT( PVOLUME_FAT pVol );

#ifdef UNDER_CE
BOOL	NotifyShell( PVOLUME_FAT pVol, const TCHAR *pName, const TCHAR* pOldName, DWORD dwAttr ,DWORD dwSize,DWORD dwEvent );
#else
BOOL	NotifyShell( PVOLUME_FAT pVol, const CHAR *pName, const CHAR* pOldName, DWORD dwAttr ,DWORD dwSize,DWORD dwEvent );
#endif

//int	WideStrLen( WCHAR * lpszStr);
//�ж��Ƿ���·���ָ�����  
#define IS_PATH_MARK( lpszStr ) ( *(lpszStr) == '\\' || *(lpszStr) == '/' )

#endif
