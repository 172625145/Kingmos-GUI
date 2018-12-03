/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����޹�.Kingmos file sys
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
���⣺�Ƿ���Ҫ��ô��CS ?
���⣺��lpVol->csNode������Χ������?!���Խڵ���в���ʱ��Ӧ���и��������Χ
	  example KFSD_MoveFile������Χ
	  KFSD_DeleteFile û����������

���⣺��Ӧ�ý�file sector cache ptr�ŵ�file node struct�ļ��ڵ㣬������
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
// �����ļ�ϵͳͷ�ṹ
typedef struct _RAM_FILE_HEAD
{
    BYTE  idName[4]; // ��ʶ = KMFS
    DWORD ver;      // �汾��
	DWORD dwSerialNum; // ���к�
	DWORD dwTotalSectors; // �ô����ܵ�������
    DWORD dwFatStartSector;  // �ô���FAT��ʼ���� file alloc table start sector
    DWORD dwFatSectorNum;    // �ô���FAT������ file alloc sector number
    DWORD dwFreeSector;		// �ô��̿���������ʼλ��
    DWORD nFreeCount;		// �ô��̿���������
    DWORD dwDataStartSector; // �ô�������������ʼλ�� data start sector
}RAM_FILE_HEAD, * PRFH;


//#define FILE_NAME_LEN (28+1+3)
// align 32 byte
// �ļ��ڵ�Ϊ�����ñ�־
#define NODE_EMPTY   0
// �ļ��ڵ�Ϊɾ����־
#define NODE_DELETED 0xfe
// �ļ��ڵ�Ϊռ�ñ�־
#define NODE_MARK    0xef

#define IS_NAME_BREAK( b ) ( (b) == '\\' || (b) == '/' )

#define GET_NODE_TYPE( lpNode ) ( (lpNode)->bFlag )

#define NODE_ALIGN_SIZE 32
#define MAX_FILE_NAME_LEN 256

#define NODE_RESERVE_NAME_LEN 12
// �����ļ����̽ڵ����ݽṹ��ע�⣺�����ݽṹ��С���� NODE_ALIGN_SIZE 
// the node struct must == NODE_ALIGN_SIZE and the  strName must >= 2, because the name :'..'3 
typedef struct _NODE
{
	BYTE bFlag; // = NODE_EMPTY; NODE_DELETED; NODE_MARK
	BYTE bNameLen;  // �ļ�������, file name length
	WORD wAttrib; // �ļ����� file attrib
	DWORD dwStartSector;  // �ļ����ݿ�ʼ����
	FILETIME ft;  // �ļ�����ʱ�� create time 8 byte
	DWORD nFileLength; // �ļ����� 4 byte
	char strName[NODE_RESERVE_NAME_LEN];// file name  11 byte
}NODE, * PNODE;

#define MAX_NODE_BYTES ( ( MAX_FILE_NAME_LEN + sizeof( NODE ) - NODE_RESERVE_NAME_LEN + NODE_ALIGN_SIZE - 1 ) / NODE_ALIGN_SIZE * NODE_ALIGN_SIZE  )
#define GET_NODE_SIZE( pNode ) ( (pNode)->bNameLen + sizeof( NODE ) - NODE_RESERVE_NAME_LEN )
#define GET_NODE_ATTRIB_SIZE( pNode ) ( sizeof( NODE ) - NODE_RESERVE_NAME_LEN )
#define GET_NODE_NUM( pNode ) ( ( (pNode)->bNameLen + sizeof( NODE ) -  NODE_RESERVE_NAME_LEN + NODE_ALIGN_SIZE - 1 ) / NODE_ALIGN_SIZE )

//������ļ��ڵ�ṹ
typedef struct _FILENODE
{
	struct _FILENODE *lpNext;  // ָ����һ���Ѵ��ļ��ṹ
	LPTSTR lpszFileName;  // ��ǰ���ļ�·���� full path name
	PNODE pNode;  // �ļ��ڵ�����ָ��
	DWORD dwRefCount; // �Ը��ļ���������
    DWORD dwFolderSector;// ���ļ����ڵ��ļ������ݵĿ�ʼ���� folder's first sector of file node's 
    DWORD dwNodeSector;// �ļ��ڵ����ڵ����� file node at the which sector
    DWORD index;// �ļ��ڵ��������������š�file node index at the sector
    CRITICAL_SECTION csFileNode; // ��ͻ��
	BYTE bNodeBuf[MAX_NODE_BYTES]; // Ϊ�ڵ�׼���Ļ��档a buf for pNode, the pNode pointer the buffer
}FILENODE, * LPFILENODE;

//

struct _KFSDVOL;

// ��������ļ��ṹ
typedef struct SF{
	struct SF * lpNext; // ָ����һ����ǰϵͳ�Ѵ򿪲����ļ�����
    // in
    struct _KFSDVOL FAR * lpVol; // �����
    HANDLE hProc;  // �ò��ҽṹ��ӵ����
    LPTSTR lpszName;   // ��Ҫ���ҵ��ļ�·�� path file name
    void * pSector;	//һ��������С��ָ�룬���ڶ�ȡ����
	PNODE pCurNode;  // ��ǰ�ļ��ڵ�
	// 
	DWORD dwFolderStartSector;  // ��ǰ�ļ��ڵ����ڵ��ļ������ݵĿ�ʼ���� start sector of the folder
    DWORD dwCurNodeSector;   // ��ǰ�ļ��ڵ��������� current sector with pSector    

    WORD nCurNodeIndex;	// ��ǰ�ļ��ڵ������������� current index  with pSector
	WORD nCurNodeNum;  // ��ǰ�ļ��ڵ���ռ�Ľڵ�����ÿ�ڵ�32bytes��

	DWORD dwCurNameLen;  // ��ǰ�ļ������� current file name len
    
    LPTSTR lpszCurName;   // ��ǰ�ļ���ָ�� current file name

    DWORD dwAddNodeSector;  // �����������з��ֵĿ��нڵ� sector of has empty node in current sector
    UINT nEmptyNodeIndex;   // �����������з��ֵĿ��нڵ������� index of has empty node in current sector
    UINT nDelNodeIndex;	// �����������з��ֵ���ɾ���ڵ�
	UINT nFreeNodeNum;	// �����������з��ֵ���ɾ���ڵ�������
}SF, * PSF;


#define NORMAL_FILE   0
#define VOL_FILE      1

// ������ļ��ṹ
typedef struct _KFSDFILE
{    
    struct _KFSDFILE * lpNext;  // ָ����һ����ǰϵͳ�Ѵ��ļ�����
	struct _KFSDVOL FAR * lpVol;  // ��ǰ�ļ����ڵľ����
    HANDLE hProc;	// ��ǰ���ļ������ӵ����

    DWORD dwAccess; // ��ǰ���ļ��Ĵ�ȡģʽ
    DWORD dwShareMode;	// ��ǰ���ļ��Ĺ���ģʽ
    DWORD dwCreate; // ��ǰ���ļ��Ĵ���ģʽ
    DWORD dwFlagsAndAttributes; // ����CreateFileʱ�� �ļ���־������
    //
	DWORD flag; // �ļ���־��1 normal file, 0, vol file  

    DWORD dwFilePointer;  // ��ǰ���ļ���ȡλ��
	LPBYTE lpbCacheBuf; // ���壬���ڼ���
    UINT  uiCacheValid;	// ����������Ч��־
	DWORD dwCachePosStart; //�����������ļ��Ŀ�ʼλ��
	LPFILENODE lpfn;  // �ļ��ڴ����еĽڵ�����
}KFSDFILE, FAR * LPKFSDFILE;

#define GET_NODE_PTR( lpFile ) ( (lpFile)->lpfn->pNode )
#define GET_NODE_OFFSET( index, lpVol ) ( (index) % (lpVol)->nNodesPerSector * NODE_ALIGN_SIZE ) 

typedef SF KFSDFIND;
typedef SF FAR *  LPKFSDFIND;

// ����򿪾�ṹ
typedef struct _KFSDVOL
{
    struct _KFSDVOL * lpNext;// ָ����һ����ǰϵͳ�Ѵ򿪾����

    HVOL hVol; // ����
    HDSK hDsk;	// �������ӵĴ���
	TCHAR szVolName[32];  // ����
    FSD_DISK_DATA fdd;  //������Ϣ
    RAM_FILE_HEAD  rfh; // KFSD�ļ�ϵͳͷ
	WORD fUpdate;  // �Ƿ���ˢ��
	WORD nNodesPerSector; // ÿ�������Ľڵ���
#define FATBYTES  ( sizeof( DWORD ) )
    DWORD * lpdwFat;    // �þ���ļ������ file alloc table
	DWORD dwMinNeedUpdate;  // �þ���ļ��������Ҫ���µĿ�ʼλ��
	DWORD dwMaxNeedUpdate;	// �þ���ļ��������Ҫ���µĽ���λ��
	
	CRITICAL_SECTION csNode; // ��ȡ�ڵ��õĳ�ͻ��
	CRITICAL_SECTION csFat;	// ��ȡFAT���õĳ�ͻ��

	CRITICAL_SECTION csFileList; // ��ȡ���ļ��õĳ�ͻ��
	CRITICAL_SECTION csFindList;// ��ȡ�����ļ��õĳ�ͻ��
 
	LPKFSDFILE lpOpenFile; // �Ѵ��ļ���������
	LPKFSDFIND lpFindFile;  // �Ѵ򿪲��Ҷ�������

    SHELLFILECHANGEFUNC pscf;  // ��ϵͳ֪ͨ�ı�Ļص�����

	TCHAR szNotifyPath0[MAX_PATH];// ��ϵͳ֪ͨ��Ҫ�Ļ���1
	TCHAR szNotifyPath1[MAX_PATH];// ��ϵͳ֪ͨ��Ҫ�Ļ���2
	LPVOID lpSectorBuf;    // Ϊ���д��������ʱ����
}KFSDVOL, FAR * LPKFSDVOL;


#define FILE_ZONE  0
#define FILE_ERROR 1

static LPKFSDVOL lpListVol = NULL; // �Ѵ򿪾�����
static CRITICAL_SECTION csVolumeList; // ��ȡ������ĳ�ͻ��

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
// �ļ�ϵͳ��������ӿ�
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
// ������BOOL _InstallKMFS( void )
// ������
// 		��  
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��ʼ��ϵͳĬ���ļ�ϵͳ
// ����: 
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
// ������void _UnInstallKMFS( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		�� _InstallKMFS �෴��ע��Kimgos file system
// ����: 
// **************************************************

void _UnInstallKMFS( void )
{
	FSDMGR_UnregisterFSD(  "KMFS" );
	DeleteCriticalSection( &csVolumeList );
}

#endif


static BOOL AccessTest( LPKFSDVOL pVolume )
{
	// ���Ƿ��д��check volume is write enable ?
	if( (pVolume->fdd.dwFlags & DISK_INFO_FLAG_READONLY)  )
	{  // ����д��error 
		WARNMSG( FILE_ZONE, ( "error in AccessTest ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//
	return TRUE;
}

// **************************************************
// ������static LPFILENODE CreateFileNode( LPCTSTR lpcszFileName )
// ������
// 		IN lpcszFileName - �ļ���
// ����ֵ��
//		����ɹ��������·�����ļ��ڵ�ָ�룻���򣬷���NULL
// ����������
//		�����ļ��ڵ�ָ��
// ����: 
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
// ������static void DeleteFileNode( LPFILENODE lpfn )
// ������
// 		IN lpfn - �ļ��ڵ�ָ��
// ����ֵ��
//		��
// ����������
//		��CreateFileNode�෴��ɾ���ڵ�
// ����: 
// **************************************************

static void DeleteFileNode( LPFILENODE lpfn )
{
	DeleteCriticalSection( &lpfn->csFileNode );
	free( lpfn );
}

// **************************************************
// ������static LPCTSTR IgnoreSpace( LPCTSTR lpcszFileName )
// ������
// 	IN lpcszFileName - �ļ���
// ����ֵ��
//	�ļ���ָ��
// ����������
//	�Թ��ļ�����ͷ�Ŀո񣬷��ص�һ���ǿո��ַ
// ����: 
// **************************************************

static LPCTSTR IgnoreSpace( LPCTSTR lpcszFileName )
{
	while( *lpcszFileName == ' ' )
		lpcszFileName++;
	return lpcszFileName;
}

// **************************************************
// ������static LPKFSDVOL CreateVolume( void )
// ������
// 	��
// ����ֵ��
//	����ɹ��������·���ľ�ָ�룻���򣬷���NULL
// ����������
// ����: 
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
// ������static void DeleteVolume( LPKFSDVOL lpVol )
// ������
// 	IN lpVol - ��ṹָ��
// ����ֵ��
//	��
// ����������
//	�� CreateVolume �෴��ɾ����
// ����: 
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
// ������static LPKFSDVOL FindVolume( HDSK hDsk )
// ������
// 	IN hDsk - ���̶�����
// ����ֵ��
//	�����ҵ������ؾ�ṹָ��; ���򣬷���NULL
// ����������
//	�д��̾���õ�������ľ����
// ����: 
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
		{	// ����
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: the volume exist, serial=%x.\r\n"), lpList->rfh.dwSerialNum ) );
			break;
		}
		lpList = lpList->lpNext;
	}
				
	LeaveCriticalSection( &csVolumeList );
	return lpList;
}

// **************************************************
// ������static BOOL RemoveVolume( LPKFSDVOL lpVol )
// ������
// 	IN lpVol - ��ṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��ϵͳ�Ƴ���
// ����: 
// **************************************************

static BOOL RemoveVolume( LPKFSDVOL lpVol )
{
    BOOL retv = FALSE;

	EnterCriticalSection( &csVolumeList );  // �����ͻ��

    if( lpListVol )
    {
        LPKFSDVOL lpList = lpListVol;
        if( lpListVol == lpVol )  
        {	// ����ĵ�һ��
            lpListVol = lpListVol->lpNext;
            retv =  TRUE;
        }
        else
        {
            while( lpList->lpNext )
            {
                if( lpList->lpNext == lpVol )
                {   // ���֣� �Ƴ�
                    lpList->lpNext = lpVol->lpNext;
                    retv =  TRUE;
                    break;
                }
                lpList = lpList->lpNext;
            }
        }
    }

	LeaveCriticalSection( &csVolumeList ); // �뿪��ͻ��

    return retv;
}

// **************************************************
// ������static BOOL AddToVolumeList( LPKFSDVOL lpvol )
// ������
// 	IN lpvol - ��ṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	���¾���������
// ����: 
// **************************************************

static BOOL AddToVolumeList( LPKFSDVOL lpvol )
{
	BOOL bRetv = FALSE;

	EnterCriticalSection( &csVolumeList ); // �����ͻ��

    if( lpvol->lpNext == 0 )
    {	// ����
        lpvol->lpNext = lpListVol;
        lpListVol = lpvol;
        bRetv = TRUE;
    }

	LeaveCriticalSection( &csVolumeList );// �뿪��ͻ��

    return bRetv;
}

// **************************************************
// ������static void CheckFat( LPKFSDVOL lpVol )
// ������
// 	IN lpVol - ��ṹָ��
// ����ֵ��
//	��
// ����������
//	���԰汾�����FAT�Ƿ���Ч
// ����: 
// **************************************************

static void CheckFat( LPKFSDVOL lpVol )
{
	EnterCriticalSection( &lpVol->csFat );// �����ͻ��

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
	LeaveCriticalSection( &lpVol->csFat );// �뿪��ͻ��
}

// **************************************************
// ������static BOOL FreeSector( LPKFSDVOL lpVol, DWORD dwStartSector ) 
// ������
// 	IN lpVol - ��ṹָ��
//	IN dwStartSector - ��ʼ����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�ͷ�����
// ����: 
// **************************************************

static BOOL FreeSector( LPKFSDVOL lpVol, DWORD dwStartSector ) 
{
    DWORD * lpdwFat = lpVol->lpdwFat;
    DWORD n, w, k;

	//CheckFat(lpVol);

    ASSERT( dwStartSector != NULL_SECTOR );
	if( dwStartSector == NULL_SECTOR )
		return TRUE;
	
	EnterCriticalSection( &lpVol->csFat );// �����ͻ��

	// ��������������õ����������������λ��
	k = w = dwStartSector;
    for( n = 0; w != NULL_SECTOR; n++ )
	{
        k = w;
		w = NEXT_SECTOR( lpdwFat, w );		
	}
	// k ��Ҫ�ͷŵ����һ����������
	// ����ϵͳFAT�Ŀ�������
    *(lpdwFat+k) = lpVol->rfh.dwFreeSector;
	// ��Ҫ���µ�FAT������
	lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, k );
	lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, k );

    lpVol->rfh.dwFreeSector = dwStartSector;  // �µĿ�����������
    lpVol->rfh.nFreeCount += n; // �µĿ���������
	lpVol->fUpdate = 1;	// ���±�־

	LeaveCriticalSection( &lpVol->csFat );// �뿪��ͻ��

    return TRUE;
}

// **************************************************
// ������static DWORD AllocSector( LPKFSDVOL lpVol, DWORD nSectorNum )
// ������
// 	IN lpVol - �����ָ��
//	IN nSectorNum - ��Ҫ��������
// ����ֵ��
//	����ɹ������ط�NULL_SECTORֵ�����򣬷���NULL_SECTOR
// ����������
//	����һ������������
// ����: 
// **************************************************

static DWORD AllocSector( LPKFSDVOL lpVol, DWORD nSectorNum )
{
	DWORD first, n, k;
	DWORD * pfat;
	DWORD end;
	
    EnterCriticalSection( &lpVol->csFat ); // �����ͻ��
	
	end = (lpVol->fdd.nSectors - 1);
		
	RETAILMSG( FILE_ZONE, ( TEXT("KFSD: AllocSector=%d, end=%d, free count=%d\r\n"), nSectorNum, end, lpVol->rfh.nFreeCount ) );
	
	//CheckFat(lpVol);
	
	if( nSectorNum > 0 && lpVol->rfh.nFreeCount >= nSectorNum )
	{   // ϵͳ���㹻������
		first = lpVol->rfh.dwFreeSector;
		k = (nSectorNum-1);
		pfat = lpVol->lpdwFat;
		for( n = 0; n < k; n++ )
		{
			first = *(pfat+first);  // get next block
			if( first > end )
			{   // ����Ӧ�õ�����
				ERRORMSG(FILE_ZONE, (TEXT("error in KFSD's AllocSector:  Alloc=%d, n=%d\r\n"), first, n));
			}
		}
		n = first;  // n Ϊ��������һ���� end block
		first = *(pfat+first);  // get next block
		*(pfat+n) = NULL_SECTOR;  // ���ÿ���������־
		// ����FAT��������
		lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, n );
		lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, n );
		// ��������ϵͳ���в��� reset free block and count
		n = lpVol->rfh.dwFreeSector;  // n = ����ֵ
		lpVol->rfh.dwFreeSector = first;
		lpVol->rfh.nFreeCount -= nSectorNum;
		lpVol->fUpdate = 1;
		//		
		LeaveCriticalSection( &lpVol->csFat );// �뿪��ͻ��		
		return n;
	}

	SetLastError(ERROR_DISK_FULL);

	WARNMSG( FILE_ZONE, (TEXT("error in KFSD's AllocSector: AllocSector no enough Sector.\r\n")));
	
	LeaveCriticalSection( &lpVol->csFat );// �뿪��ͻ��
	
    return NULL_SECTOR;
}

// **************************************************
// ������static DWORD ReallocSector(
//                          LPKFSDVOL lpVol, 
//                          DWORD dwAllocSector, 
//                          DWORD nSectorNum )
// ������
// 	IN lpVol - �����ָ��
//	IN dwAllocSector - �ѷ�������
//	IN nSectorNum - �µ�������
// ����ֵ��
//	����ɹ������ط�NULL_SECTORֵ�����򣬷���NULL_SECTOR
// ����������
//	�ı�/���·���֮ǰ�ѷ��������Ŀ���
// ����: 
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
    {	// ��ͬ�� AllocSector
        return AllocSector( lpVol, nSectorNum );
    }
    else if( nSectorNum == 0 )
    {   // ��ͬ���ͷ� free all block
        FreeSector( lpVol, dwAllocSector );
        return NULL_SECTOR;
    }
    else
    {
        DWORD s = dwAllocSector;
        DWORD p = dwAllocSector;
        DWORD * pfat;
        i = 0;

		EnterCriticalSection( &lpVol->csFat );// �����ͻ��
		pfat = lpVol->lpdwFat;

        while( s != NULL_SECTOR )
        {
            i++;
            if( i > nSectorNum )
            {  // �����������ͷ�һЩ��decrease sector, to free other sector
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
        {   // С�ڣ��������һЩ
			s = AllocSector( lpVol, (nSectorNum - i) );
            if( s != NULL_SECTOR )
			{
                *(pfat+p) = s;   // ������һ��link them
				// ��Ҫ���µ�FAT��
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
		{	// ���ڣ� ����Ҫ��ʲô
			dwRetv = dwAllocSector;
			goto _RETV;
		}
_RETV:
        LeaveCriticalSection( &lpVol->csFat );// �뿪��ͻ��
    }


    return dwRetv;
}

// **************************************************
// ������static DWORD LinkSector( LPKFSDVOL lpVol, DWORD dwDest, DWORD dwSource )
// ������
// 	IN lpVol - �����ָ��
//	IN dwDest - Ŀ������
//	IN dwSource - Դ����
// ����ֵ��
//	����ɹ������ط�NULL_SECTORֵ�����򣬷���NULL_SECTOR
// ����������
//	����������������������
// ����: 
// **************************************************

static DWORD LinkSector( LPKFSDVOL lpVol, DWORD dwDest, DWORD dwSource )
{
    DWORD dwSector = dwDest;

	//CheckFat(lpVol);    

	EnterCriticalSection( &lpVol->csFat );// �뿪��ͻ��

	ASSERT( dwDest != NULL_SECTOR && dwSource != NULL_SECTOR );
	// �ҵ�Ŀ��β
    while( *(lpVol->lpdwFat+dwSector) != NULL_SECTOR )
        dwSector = *(lpVol->lpdwFat+dwSector);
	// ���ӣ�dest + source
    *(lpVol->lpdwFat+dwSector) = dwSource;
	// ��Ҫ���µ�ϵͳ������
    lpVol->dwMaxNeedUpdate = MAX( lpVol->dwMaxNeedUpdate, dwSector );
    lpVol->dwMinNeedUpdate = MIN( lpVol->dwMinNeedUpdate, dwSector );
	lpVol->fUpdate = 1;

	LeaveCriticalSection( &lpVol->csFat );// �뿪��ͻ��

    return dwDest;
}

// **************************************************
// ������static DWORD GetSectorOffset( LPKFSDVOL lpVol, DWORD dwStartSector, UINT nOffset )
// ������
// 	IN lpVol - �����ָ��
//	IN dwStartSector - ��ʼ����
//	IN nOffset - �ӿ�ʼ�������ƫ����
// ����ֵ��
//	����ɹ������ط�NULL_SECTOR����������; ���򣬷���NULL_SECTOR
// ����������
//	�õ��Ӹ���������������ĵ�nOffset����������
// ����: 
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
// ������static DWORD GetSectorNum( LPKFSDVOL lpVol, DWORD dwStartSector )
// ������
// 	IN lpVol - �����ָ��
//	IN dwStartSector - ��ʼ����
// ����ֵ��
//	������
// ����������
//	�õ��ӿ�ʼ�������������
// ����: 
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
// ������static BOOL ReallocFile( LPKFSDFILE lpFile, DWORD dwSize )
// ������
//	IN lpFile - ���ļ�����ṹָ��
//	IN dwSize - �ļ�����
// ����ֵ��
//	����ɹ�������TRUE; ����, ����FALSE
// ����������
//	�����趨�ļ���С
// ����: 
// **************************************************

static BOOL ReallocFile( LPKFSDFILE lpFile, DWORD dwSize )
{
    DWORD dwStartSector;
    if( dwSize != GET_NODE_PTR(lpFile)->nFileLength )  // �Ƿ��뵱ǰ�ļ��Ĵ�С��� ��
    {	// ����ȣ��ط���
        dwStartSector = ReallocSector( lpFile->lpVol, 
			                          GET_NODE_PTR(lpFile)->dwStartSector, 
									  ( (dwSize + lpFile->lpVol->fdd.nBytesPerSector - 1) / lpFile->lpVol->fdd.nBytesPerSector ) );
		//
        if( dwStartSector != NULL_SECTOR || dwSize == 0 )
        {	// ��������
            GET_NODE_PTR(lpFile)->dwStartSector = dwStartSector;
            GET_NODE_PTR(lpFile)->nFileLength = dwSize;
        }
        else
            return FALSE;
    }
    return TRUE;
}

// **************************************************
// ������static int GetFolderAdr( LPTSTR * lppFolder, LPCTSTR lpcfn, int * lpNextLen )
// ������
// 	OUT lppFolder - ���ڽ����ļ������Ļ���
//	IN lpcfn - �ļ�·����
// 	OUT lpNextLen - ���ڽ�����һ���ļ��������λ��
// ����ֵ��
// ����������
// ����: 
// **************************************************

static int GetFolderAdr( LPTSTR * lppFolder, LPCTSTR lpcfn, int * lpNextLen )
{
    int i = 0;
	int fn = 0;
    if( IS_NAME_BREAK(*lpcfn)  )  // �����ַָ����"\" or "/"����
	{	// ��
        lpcfn++;  ///parent level
		i++;
	}

    while( *lpcfn && fn < MAX_FILE_NAME_LEN  )
    {	//  �����зǷָ������������
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


// �ļ��в�����
#define OP_DEL           0x0001    // (OP_DELFILE | OP_DELFOLDER)
#define OP_ADD           0x0004    // add node if not found, the lpFile->node is valid
#define OP_UPDATE        0x0008    // update node, the lpFile->node is valid
#define OP_GET           0x0010    // get node, fill lpFile->node

// **************************************************
// ������static BOOL DoFolderInit( 
//						 LPKFSDVOL lpVol , 
//						 DWORD dwFolderSector, 
//						 DWORD dwParentSector, 
//						 WORD * pSector )
// ������
//	IN lpVol - �����ṹָ��
//	IN dwFolderSector - ��ǰ�ļ���������������
//	IN dwParentSector - ��ǰ�ļ��е���һ���ļ���������������
//	IN/OUT pSector - һ��������С�ĵ�ǰ�ļ����ڴ�
// ����ֵ��
//	����ɹ�������TRUE; ���� ����FALSE
// ����������
//	��ʼ���µ��ļ������ݡ����µ��ļ�������д��pSector�ڴ沢����д�����
// ����: 
// **************************************************

static BOOL DoFolderInit( 
						 LPKFSDVOL lpVol , 
						 DWORD dwFolderSector, 
						 DWORD dwParentSector, 
						 WORD * pSector )
{   
    PNODE pNode = (PNODE)pSector;
	// ��0
    memset( pSector, 0, lpVol->fdd.nBytesPerSector );

	// ÿһ���ļ��еĵ�һ���ļ��ڵ��ʾ�Լ�
	// �ڶ����ļ��ڵ��ʾ�ϼ��ļ���/���ļ���
    // ��ʼ������init this folder
	pNode->bFlag = NODE_MARK;
    pNode->strName[0] = '.';  // �����ʾ
    pNode->dwStartSector = dwFolderSector; // ָ������ʼ����
	pNode->bNameLen = 1;
	pNode->wAttrib |= FILE_ATTRIBUTE_DIRECTORY;
	pNode->wAttrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;

    // ��ʼ�����ļ��� init parent folder
    pNode = (PNODE)( (LPBYTE)pSector + NODE_ALIGN_SIZE );
	pNode->bFlag = NODE_MARK;
    pNode->strName[0] = '.';// ���ļ��б�ʾ ".."
    pNode->strName[1] = '.';// ���ļ��б�ʾ
	pNode->bNameLen = 2;
    pNode->dwStartSector = dwParentSector;  // ָ���ļ��п�ʼ����
	pNode->wAttrib |= FILE_ATTRIBUTE_DIRECTORY;	
	pNode->wAttrib |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM;
	// д�����
    if( ERROR_SUCCESS == FSDMGR_WriteDisk( lpVol->hDsk, dwFolderSector, 1, (LPBYTE)pSector, lpVol->fdd.nBytesPerSector ) )
		return TRUE;
	return FALSE;
}
// **************************************************
// ������static BOOL SearchFolder( SF * psf, BOOL bNext, int nStartIndex )
// ������
// 	IN psf - SF�ṹָ��
//	IN bNext - �Ƿ����һ���ļ��ڵ㿪ʼ����/�Ƚϣ�����ΪTRUE,��nStartIndex��Ч��
//	IN nStartIndex - ��ʼ����/�Ƚϵ��ļ��ڵ������ţ�����bNextΪFALSE,��nStartIndex��Ч)
// ����ֵ��
//	�����ҵ�ƥ����ļ���������TRUE; ���� ����FALSE
// ����������
//	����ƥ����ļ�
// ����: 
// **************************************************

extern BOOL FileNameCompare( LPCTSTR lpcszMask, int iMaskLen, LPCTSTR lpcszSrc, int iSrcLen );
static BOOL SearchFolder( SF * psf, BOOL bNext, int nStartIndex )
{
    LPCTSTR lpcszFileName = psf->lpszCurName; // ��Ҫ���ҵ��ļ��������Ժ���ͨ�����
    int iFileNameLen = psf->dwCurNameLen; // ���ҵ��ļ�������
	PNODE lpNode;
	UINT uiCurNodeNum = 0;
	UINT uiCurNodeIndex;
	UINT uiSectorOffset;
	DWORD dwCurSector;
	DWORD dwOffset;
	DWORD nBytesPerSector = psf->lpVol->fdd.nBytesPerSector;
	BOOL bUpdate, bRetv = FALSE;
    
    if( bNext )
    {   // ������²��ң�����ǰ���ҽڵ�������Ϊ��һ���ļ��ڵ� 
		// search next file match with filename
        uiCurNodeIndex = psf->nCurNodeIndex + psf->nCurNodeNum;
    }
    else
    {	// ���ļ��еĿ�ʼ����
		uiCurNodeIndex = nStartIndex;
		psf->dwAddNodeSector = NULL_SECTOR;
		psf->nEmptyNodeIndex = -1;
		psf->nDelNodeIndex = -1;
    }
	// uiCurNodeIndex�����ڵڼ�������
	uiSectorOffset = uiCurNodeIndex / psf->lpVol->nNodesPerSector; 
	// uiCurNodeIndex����������������
	dwCurSector = GetSectorOffset( psf->lpVol, psf->dwFolderStartSector, uiSectorOffset );
	// uiCurNodeIndex������������ƫ��
    dwOffset = ( (uiCurNodeIndex * NODE_ALIGN_SIZE) % nBytesPerSector );

	bUpdate = TRUE;
	EnterCriticalSection( &psf->lpVol->csNode );
	// ���ڵ�����/����
	while( dwCurSector != NULL_SECTOR )
    {                 
        // search node in sector
        if( bUpdate )
		{   // ��ȡ�������ݵ� psf->pSector
			if( FSDMGR_ReadDisk( psf->lpVol->hDsk, dwCurSector, 1, psf->pSector, psf->lpVol->fdd.nBytesPerSector ) != ERROR_SUCCESS )
				break;
		}
		// ÿ�������������������ڵ�
		lpNode = (PNODE)( (LPBYTE)psf->pSector + dwOffset );
		if( lpNode->bFlag == NODE_EMPTY ||
			lpNode->bFlag == NODE_DELETED )
		{	// �ýڵ�Ϊ�ջ��ѱ�ɾ����
			if( lpNode->bFlag == NODE_EMPTY )
			{	// �սڵ㣬�趨psf�ṹ�� ��Ч�ڵ��Ա����
				if( psf->dwAddNodeSector == NULL_SECTOR )
				{	
					psf->nEmptyNodeIndex = uiCurNodeIndex;
					psf->dwAddNodeSector = dwCurSector;
					psf->nFreeNodeNum = psf->lpVol->nNodesPerSector - uiCurNodeIndex % psf->lpVol->nNodesPerSector; 
				}
				goto RETV; // ����Ҫ�ٲ���
			}
			else
			{  // // ��ɾ���ڵ㣬�趨psf�ṹ�� ɾ���ڵ��Ա���� deleted node
				if( psf->dwAddNodeSector == NULL_SECTOR &&
					lpNode->bNameLen >= psf->dwCurNameLen )
				{
					psf->nDelNodeIndex = uiCurNodeIndex;
					psf->dwAddNodeSector = dwCurSector;
					psf->nFreeNodeNum = GET_NODE_NUM( lpNode );
				}				
			}
			// �õ���ǰ�ļ��ڵ�Ľڵ���
			uiCurNodeNum = GET_NODE_NUM( lpNode );
		}
		else if( lpNode->bFlag == NODE_MARK )
		{	// ���ļ��ڵ��ѱ�ʹ��
			uiCurNodeNum = GET_NODE_NUM( lpNode );// �õ���ǰ�ļ��ڵ�Ľڵ���
			if( uiCurNodeNum > 1 && 
				(uiCurNodeNum * NODE_ALIGN_SIZE + dwOffset) > nBytesPerSector )
			{
				lpNode = psf->pCurNode;
		        if( 0 == ReadSectors( psf->lpVol, dwCurSector, dwOffset, (LPBYTE)lpNode, uiCurNodeNum * NODE_ALIGN_SIZE, psf->pSector, NULL ) )
					goto RETV; // ������
			}
			
			if( lpcszFileName[0] == '.' )// �Ƿ���Ҫ���ҵ��� '.' �� '..'  ��
			{   // �� '.' or '..' ? 
				if( (iFileNameLen == 1 && uiCurNodeIndex == 0) ||
					(iFileNameLen == 2 && uiCurNodeIndex == 1) )
				{   // yes, find it
					bRetv = TRUE;
				}				    
			}
			else  // ���ǣ��Ƚ�
				bRetv = FileNameCompare( lpcszFileName, iFileNameLen, lpNode->strName, lpNode->bNameLen );
			if( bRetv )
			{	// �ҵ�������psf�ṹ��س�Ա
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
		// ��ǰ�ڵ㲻���ţ�׼����һ���ļ��ڵ㡣not find match name
        uiCurNodeIndex += uiCurNodeNum;
		{
			UINT t = uiCurNodeIndex / psf->lpVol->nNodesPerSector;
			bUpdate = uiSectorOffset < t; // ��һ���ڵ��Ƿ��ڵ�ǰ���� ��
			for( ; uiSectorOffset < t && dwCurSector != NULL_SECTOR; uiSectorOffset++ )
			{
				dwCurSector =  NEXT_SECTOR( psf->lpVol->lpdwFat, dwCurSector );
			}
		}

        dwOffset = GET_NODE_OFFSET( uiCurNodeIndex, psf->lpVol );
	}

RETV:
	LeaveCriticalSection( &psf->lpVol->csNode );  // �뿪��ͻ��
    
    return bRetv;
}

// **************************************************
// ������static DWORD NewFolder( LPKFSDVOL lpVol, DWORD dwParent, void * lpSectorBuf )
// ������
// 	IN lpVol - ��������ݽṹ
//	IN dwParent - �ϼ��ļ���
//	IN lpSectorBuf - ���ڶ�ȡ�������ݵĻ���
// ����ֵ��
//	����ɹ����������ļ��е����������ţ�ʧ�ܣ�����NULL_SECTOR
// ����������
//	�����µ��ļ���
// ����: 
// **************************************************

static DWORD NewFolder( LPKFSDVOL lpVol, DWORD dwParent, void * lpSectorBuf )
{
	DWORD dwSector;

    dwSector = AllocSector( lpVol, 1 ); // Ϊ���ļ��з�������
	if( dwSector != NULL_SECTOR )
	{	// ����ɹ�����ʼ�����ļ�������
		if( DoFolderInit( lpVol , dwSector, dwParent, lpSectorBuf ) )
			return dwSector;
		else
		{	// ʧ��
			FreeSector( lpVol, dwSector );
		}
	}
	return NULL_SECTOR;
}

// **************************************************
// ������static DWORD AddNodeSector( PSF psf, DWORD dwNeedSectors )
// ������
// 	IN psf - ���ҽṹָ��
// 	IN dwNeedSectors - ��Ҫ��������
// ����ֵ��
//	����ɹ������ط� NULL_SECTORֵ�����򣬷�������ֵ
// ����������
//	Ϊ�µĽڵ����Ӵ洢����
// ����: 
// **************************************************

static DWORD AddNodeSector( PSF psf, DWORD dwNeedSectors )
{
    DWORD dwNodeSector;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd, No Empty Node\r\n")));
	dwNodeSector = AllocSector( psf->lpVol, dwNeedSectors );
	if( dwNodeSector != NULL_SECTOR )
	{	// ����ɹ�
		DWORD dwInitSector;

		memset( psf->pSector, 0, psf->lpVol->fdd.nBytesPerSector );
		
		EnterCriticalSection( &psf->lpVol->csNode );  // �����ͻ��
		// ���ӵ���ǰ�ļ���	 
        LinkSector( psf->lpVol, psf->dwFolderStartSector, dwNodeSector );
        // init it
		dwInitSector = dwNodeSector;		
		// ����ʼ������д�����
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

		LeaveCriticalSection( &psf->lpVol->csNode ); // �뿪��ͻ��
	}
	return dwNodeSector;
}

// **************************************************
// ������static BOOL DoNodeAdd( PSF psf, PNODE pNode )
// ������
// 	IN psf - ���ҽṹָ��
//	IN pNode - �ļ��ڵ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ڵ�ǰ�ļ���������һ���ļ��ڵ�
// ����: 
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
	
    EnterCriticalSection( &psf->lpVol->csNode );  // �����ͻ��

	if( (dwNodeSector = psf->dwAddNodeSector) == NULL_SECTOR )
	{   // ��ǰ�ļ���û�п��нڵ㣬���ӡ���alloc enough sector to save the node
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd, No Empty Node\r\n")));
	    dwNeedSectors = ( GET_NODE_SIZE( pNode ) + nBytesPerSector - 1 ) / nBytesPerSector;
		// Ϊ�ļ��з����µĴ�������
		dwNodeSector = AddNodeSector( psf, dwNeedSectors );
		
		if( dwNodeSector != NULL_SECTOR )
		{	//�ɹ�
			dwOffset = 0;			 
			uiNodeIndex = GetSectorNum( psf->lpVol, psf->dwFolderStartSector ) * psf->lpVol->nNodesPerSector;
		}
		else
		{	// ʧ��
			LeaveCriticalSection( &psf->lpVol->csNode );// �뿪��ͻ��
			return FALSE;
		}
	}
	else
	{	// �п��нڵ�
		dwNeedNodeNum = GET_NODE_NUM( pNode ); // �õ����ļ��ڵ�����ڵ���
        if( psf->nDelNodeIndex != -1 )
		{	// ���нڵ���ɾ���ڵ�
			ASSERT( dwNeedNodeNum <= psf->nFreeNodeNum );
			if( dwNeedNodeNum < psf->nFreeNodeNum )
			{	// �ڵ����㹻���ұ���Ҫ�Ķ࣬�����½ڵ�ķ����ȥ
				NODE node;
				DWORD dwFreeSector;
				memset( &node, 0, sizeof( NODE ) );
				// Ϊ���½ڵ�׼������
				node.bFlag = NODE_DELETED;
				node.dwStartSector = NULL_SECTOR;
				node.bNameLen = (BYTE)( (psf->nFreeNodeNum - dwNeedNodeNum) * NODE_ALIGN_SIZE - sizeof( NODE ) + NODE_RESERVE_NAME_LEN ); 
				// �õ����½ڵ��������������� get node index and offset to split 
	            dwFreeSector = GetSectorOffset( psf->lpVol, 
					                            psf->dwFolderStartSector, 
											    (psf->nDelNodeIndex + dwNeedNodeNum) / psf->lpVol->nNodesPerSector );
				// �õ����½ڵ��������������ڵ�ƫ��
                dwOffset = GET_NODE_OFFSET( (psf->nDelNodeIndex + dwNeedNodeNum), psf->lpVol );
				ASSERT( dwFreeSector != NULL_SECTOR );
				// д�����
				if( 0 == WriteSectors( psf->lpVol, dwFreeSector, dwOffset, (LPBYTE)&node, NODE_ALIGN_SIZE - NODE_RESERVE_NAME_LEN, psf->pSector ) )
				{	// ʧ��
					LeaveCriticalSection( &psf->lpVol->csNode );// �뿪��ͻ��
	                return FALSE;//
				}
			}
		    // set node index and offset to added
			// �µ��ļ��ڵ�������� �� ƫ��
			uiNodeIndex = psf->nDelNodeIndex;
			dwOffset = GET_NODE_OFFSET( psf->nDelNodeIndex, psf->lpVol );
		}
		else
		{   // �������ڵ� has empty index
            ASSERT( psf->nEmptyNodeIndex != -1 );
			if( psf->nFreeNodeNum < dwNeedNodeNum )
			{	// �㹻 add node sector
				dwNeedSectors = ( (dwNeedNodeNum - psf->nFreeNodeNum) * NODE_ALIGN_SIZE + nBytesPerSector - 1 ) / nBytesPerSector;
				if( AddNodeSector( psf, dwNeedSectors ) == NULL_SECTOR )
				{	// ʧ��
					LeaveCriticalSection( &psf->lpVol->csNode );// �뿪��ͻ��
	                return FALSE;// no sector
				}
			}
			// �µ��ļ��ڵ�������� �� ƫ��
	        uiNodeIndex = psf->nEmptyNodeIndex;
			dwOffset = GET_NODE_OFFSET( psf->nEmptyNodeIndex, psf->lpVol );
		}
	}

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeAdd, Sector=%d, Index=%d\r\n"), psf->dwAddNodeSector, psf->nEmptyNodeIndex));

	pNode->bFlag = NODE_MARK; // ����Ϊռ�ñ�־
	// д�����
	if( ( dwOffset = WriteSectors( psf->lpVol, dwNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), psf->pSector ) ) )
	{
		psf->nCurNodeIndex = uiNodeIndex;
		psf->dwCurNodeSector = dwNodeSector;
		memcpy( psf->pCurNode, pNode, GET_NODE_SIZE(pNode) );
	}

	LeaveCriticalSection( &psf->lpVol->csNode );// �뿪��ͻ��
		
	return dwOffset != 0;
}

// **************************************************
// ������static BOOL DoNodeUpdate( PSF psf, PNODE pNode )
// ������
// 	IN psf - ���ҽṹָ��
//	IN pNode - �ļ��ڵ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���µ�ǰ�ļ�����һ���ļ��ڵ�
// ����: 
// **************************************************

static BOOL DoNodeUpdate( PSF psf, PNODE pNode )
{   
	BOOL bRetv;
	DWORD dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeUpdate.\r\n")));

	EnterCriticalSection( &psf->lpVol->csNode );  // �����ͻ��
	// д�����
	bRetv = WriteSectors( psf->lpVol, psf->dwCurNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), psf->pSector );

	LeaveCriticalSection( &psf->lpVol->csNode ); // �뿪��ͻ��

    if( bRetv && psf->pCurNode != pNode )
	    memcpy( psf->pCurNode, pNode, GET_NODE_SIZE( pNode ) ); // ����psf��Ա
    return bRetv;
    
}

// **************************************************
// ������static BOOL DoNodeGet( PSF psf, PNODE pNode )
// ������
// 	IN psf - ���ҽṹָ��
//	IN pNode - �ļ��ڵ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ���ǰ�ļ�����һ���ļ��ڵ������
// ����: 
// **************************************************

static BOOL DoNodeGet( PSF psf, PNODE pNode )
{
	DWORD dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoNodeGet\r\n")));
	
	memcpy( pNode, psf->pCurNode, GET_NODE_SIZE( psf->pCurNode ) );
	return TRUE;
}

// **************************************************
// ������static BOOL DoNodeDel( PSF psf, BOOL bFreeSector )
// ������
// 	IN psf - ���ҽṹָ��
//	IN bFreeSector - ��Ҫ�ͷ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ���ļ��ڵ�
// ����: 
// **************************************************

static BOOL DoNodeDel( PSF psf, BOOL bFreeSector )
{
    DWORD dwOffset; 
	BOOL bRetv;

	EnterCriticalSection( &psf->lpVol->csNode ); // �����ͻ��
    
	if( bFreeSector )
	{	// �ͷ�����
		if( psf->pCurNode->nFileLength ||
			(psf->pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) )
        {
            ASSERT( psf->pCurNode->dwStartSector != NULL_SECTOR );
            if( psf->pCurNode->dwStartSector != NULL_SECTOR )
                FreeSector( psf->lpVol, psf->pCurNode->dwStartSector ); // �ͷ��ļ��ڵ����������
        }
        else
        {
            ;//ASSERT( psf->pCurNode->wStartSector == NULL_SECTOR );
        }
	}

	psf->pCurNode->bFlag = NODE_DELETED;  // ����ɾ����־
	dwOffset = GET_NODE_OFFSET( psf->nCurNodeIndex, psf->lpVol );
	// ������д������
	bRetv = WriteSectors( psf->lpVol, psf->dwCurNodeSector, dwOffset, (LPBYTE)psf->pCurNode, GET_NODE_SIZE( psf->pCurNode ), psf->pSector );
	
	LeaveCriticalSection( &psf->lpVol->csNode ); // �뿪��ͻ��
	return bRetv;
}

// **************************************************
// ������static BOOL DoFolderChange( 
//                       SF * psf,
//					     PNODE pNode,
//                       int op )
// ������
// 	IN psf - ���ҽṹָ��
//	IN pNode - �ļ��ڵ�ṹ
//	IN op - ������
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ļ�������ز���
// ����: 
// **************************************************

static BOOL DoFolderChange( 
                       SF * psf,
					   PNODE pNode,
                       int op )
{
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DoFolderChange.\r\n")));
	switch( op )
	{
	case OP_DEL:  // ɾ���ļ���
		{
			SF sfChild;
			if( BeginSearch( &sfChild, psf->lpVol, psf->hProc, NULL ) )  // ��ʼ�������ṹ����
			{	// ������ǰ�ļ������Ƿ�Ϊ�գ��б���ļ��ڵ㣿)
				sfChild.lpszCurName = "*";
				sfChild.dwCurNameLen = 1;
				sfChild.dwFolderStartSector = psf->pCurNode->dwStartSector;
				op = SearchFolder( &sfChild, FALSE, USER_NODES ); // ����
				EndSearch( &sfChild );
				if( op )  // �б���ļ��ڵ� ��
				{	// �У��ǿ�
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
	case OP_ADD:  // �����ļ���
		// �����µ��ļ������ݽṹ
		pNode->dwStartSector = NewFolder( psf->lpVol, psf->dwFolderStartSector, psf->pSector );
		if( pNode->dwStartSector != NULL_SECTOR )
			return DoNodeAdd( psf, pNode ); // д����̽ڵ�
		else
			return FALSE;
	}
	return FALSE;
}

// **************************************************
// ������static BOOL SearchNode( SF * lpsf )
// ������
// 	IN lpsf - ���ҽṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����ļ��ڵ�
// ����: 
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
	lpszName = lpsf->lpszName;  // �ļ�·����
	lpsf->dwCurNameLen = 0; //
	lpsf->lpszCurName = NULL;
	
	while( 1 )
	{
		int iNextLen;
		LPTSTR lpszCurName;
		// ��lpszName�õ����ļ�/·����
		// example : "\\kingmos\\folder1\\folder2\\folder3\\filename.txt" 
		//			     1filename  2fn     3fn      4fn      5fn  
		// lpszCurNameָ��ǰ�õ����ļ���������'\'����iNextLenΪ��һ���ļ�����lpszName��λ��
		len = GetFolderAdr( &lpszCurName, lpszName, &iNextLen );
		if( len )  // �õ��� ��
		{	//�ǣ�׼�����ҽṹ
			lpsf->nCurNodeIndex = 0;
			lpsf->lpszCurName = lpszCurName; // ��Ҫ���ҵ��ļ��ڵ���
			lpsf->dwCurNameLen = len;  // // ��Ҫ���ҵ��ļ��ڵ�������
			lpszName += iNextLen;  // ָ����һ����
			
			if( SearchFolder( lpsf, FALSE, ALL_NODES ) )  // ����
			{	 // ������ find node
				if( *lpszName )  // �����ļ����� ��
				{   // �ǣ������ǰ���ҵ������ļ��У������ the node must is folder
					if( lpsf->pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY ) // the node is folder
					{	// �ļ���
						lpsf->dwFolderStartSector = lpsf->pCurNode->dwStartSector;
						continue;   // continue search in sub folder
					}
					else
					{   // ���ļ��� not folder
						DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Node is'nt folder=%s.\r\n"), lpsf->lpszCurName));
					}
				}
				else
				{	// �ҵ�
					DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Find Node=%s.\r\n"), lpsf->pCurNode->strName));					
					bRetv = TRUE;
					break;
				}
			}
			else
			{   // û���ҵ�
				DEBUGMSG(FILE_ZONE, (TEXT("KFSD: not find folder=%s.\r\n"), lpsf->lpszCurName ));
			}
		}
		break;
	}
	
	LeaveCriticalSection( &lpsf->lpVol->csNode );  // �뿪��ͻ��

	if( bRetv )
		SetLastError(ERROR_FILE_EXISTS);
	else
	    SetLastError(ERROR_FILE_NOT_FOUND);
	return bRetv;
}

// **************************************************
// ������static BOOL BeginSearch( SF * lpsf, LPKFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName )
// ������
// 	IN lpsf - ���ҽṹָ��
//	IN lpVol - �����ṹָ��
//	IN hProc - ӵ���߽��̾��
//	IN lpszName - ��Ҫ���ҵ��ļ�·����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ��һ�����Ҷ���ṹ
// ����: 
// **************************************************

static BOOL BeginSearch( SF * lpsf, LPKFSDVOL lpVol, HANDLE hProc, LPTSTR lpszName )
{	// Ϊ��д��������׼������
	lpsf->pSector = malloc( MAX_NODE_BYTES + lpVol->fdd.nBytesPerSector );
	if( lpsf->pSector )
	{	// ��ʼ�����Ҷ���ṹ
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
// ������static void EndSearch( SF * lpsf )
// ������
// 	IN lpsf - ���ҽṹָ��
// ����ֵ��
//	��
// ����������
//	��BeginSearch�෴���ͷ���������
// ����: 
// **************************************************

static void EndSearch( SF * lpsf )
{
	if( lpsf->pSector )
		free( lpsf->pSector );
}

// **************************************************
// ������static DWORD ReadSectors( 
//							LPKFSDVOL lpVol, 
//							DWORD dwStartSector, 
//							DWORD nOffset, 
//							LPBYTE lpBuf, 
//							DWORD dwSize,
//							LPBYTE pSector,
//							UINT * lpCacheValid )
// ������
//		IN lpVol - �����ṹָ��
//		IN dwStartSector - ��ʼ����
//		IN nOffset - ��������ƫ���ֽ�
//		IN lpBuf - ���ڽ������ݵĻ���
//		IN dwSize - ��Ҫ���ܵ�����
//		IN pSector - ���ڶ�ȡ�������ݵ���ʱ����
//		IN lpCacheValid - ���ڽ����������������Ƿ���Ч
// ����ֵ��
//		��ȡ�����ݴ�С
// ����������
//		��ȡ���������ݣ���ʵ����Ҫ������д��lpBuf,������ʵ�ʶ�ȡ�����ݴ�С
// ����: 
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
        {	// ����������Ϊ��Ҫ����Ч����
            if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, lpBuf, nSectorSize ) )
				goto _error_ret;

            dwSize -= nSectorSize;
            lpBuf += nSectorSize;
        }
        else
        {	// �������ֲ�Ϊ��Ҫ����Ч����
            if(  ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
				goto _error_ret;
            if( nOffset + dwSize >= nSectorSize )
            {	// ���ݿ�������������nOffset��ʼ��������������
                memcpy( lpBuf, pSector + nOffset, nSectorSize - nOffset );
                dwSize -=  nSectorSize - nOffset;
                lpBuf += nSectorSize - nOffset;
            }
            else
            {	// �����������м䣬������Ҫ����
                memcpy( lpBuf, pSector + nOffset, dwSize );
                dwSize = 0;
            }
			bCache = TRUE; // CACHE ��Ч
        }
		// ������У���ȡ���µ�����.
        while( dwSize )
        {
            dwStartSector = NEXT_SECTOR( lpVol->lpdwFat, dwStartSector );
            if( dwStartSector != NULL_SECTOR )
            {
                if( dwSize >= nSectorSize )
                {	// ���µ����ݲ�С��һ��������С��ȡ��������
                    if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, lpBuf, nSectorSize ) )
						goto _error_ret;
                    dwSize -= nSectorSize;
                    lpBuf += nSectorSize;
					bCache = FALSE;
                }
                else
                {	// ���µ�����С��һ��������С��ȡ�����ֲ�
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
    return s - dwSize;// ʵ�ʶ�������
_error_ret:
	return 0;
}

// **************************************************
// ������static DWORD WriteSectors( 
//							LPKFSDVOL lpVol, 
//							DWORD dwStartSector, 
//							DWORD nOffset, 
//							const unsigned char * lpBuf, 
//							DWORD dwSize,
//							LPBYTE pSector
//						   )
// ������
//		IN lpVol - �����ṹָ��
//		IN dwStartSector - ��ʼ����
//		IN nOffset - ��������ƫ���ֽ�
//		IN lpBuf - ����д�����ݵĻ���
//		IN dwSize - ��Ҫд�������
//		IN pSector - ���ڶ�ȡ�������ݵ���ʱ����
// ����ֵ��
//		д������ݴ�С
// ����������
//		д���������ݣ�������ʵ��д������ݴ�С
// ����: 
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
        {	// ����������Ϊ��Ҫ���µ���Ч����
            if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)lpBuf, nSectorSize ) )
				goto _error_ret;  // ʧ��

            dwSize -= nSectorSize;
            lpBuf += nSectorSize;
        }
        else
        {	// ���������ֲ�Ϊ��Ҫ���µ���Ч���ݣ�ִ�ж�ȡ-����-д��
            if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)pSector, nSectorSize ) )
				goto _error_ret;
            if( nOffset + dwSize >= nSectorSize )
            {// ���ݿ�������������nOffset��ʼ��������Ҫ����������
                memcpy( pSector + nOffset, lpBuf, nSectorSize - nOffset );
                dwSize -=  nSectorSize - nOffset;
                lpBuf += nSectorSize - nOffset;
            }
            else
            {// �����������м䣬������Ҫ����
                memcpy( pSector + nOffset, lpBuf, dwSize );
                dwSize = 0;
            }
            if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
				goto _error_ret;  //д�����ʧ��
        }
		// д�����µĲ���
        while( dwSize )
        {
            dwStartSector = NEXT_SECTOR( lpVol->lpdwFat, dwStartSector );
            if( dwStartSector != NULL_SECTOR )
            {
                if( dwSize >= nSectorSize )
                {// ��������������Ҫ����
                    if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)lpBuf, nSectorSize ) )
				        goto _error_ret;
                    dwSize -= nSectorSize;
                    lpBuf += nSectorSize;
                }
                else
                {// �ֲ�����������Ҫ���£�ִ�ж�ȡ-����-д��
                    if( ERROR_SUCCESS != FSDMGR_ReadDisk( lpVol->hDsk, dwStartSector, 1, (LPBYTE)pSector, nSectorSize ) )
				        goto _error_ret;// ������
                    memcpy( pSector, lpBuf, dwSize );
                    dwSize = 0;					
                    if( ERROR_SUCCESS != FSDMGR_WriteDisk( lpVol->hDsk, dwStartSector, 1, pSector, nSectorSize ) )
				        goto _error_ret;// д����
                }
            }
            else
                break;
        }
    }

    return s - dwSize;  // ʵ��д�������
_error_ret:
	return 0;
}

// **************************************************
// ������static DWORD GetSerialNum(void)
// ������
//		��
// ����ֵ��
//		���ؾ����к�
// ����������
//		���������к�
// ����: 
// **************************************************

static DWORD GetSerialNum(void)
{
	return random();   
}

// **************************************************
// ������static BOOL DoVolumeFormat( LPKFSDVOL lpVol )
// ������
// 	IN lpVol - �����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��ʽ������
// ����: 
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

	EnterCriticalSection( &lpVol->csFat ); // �����ͻ��
	pSector = lpVol->lpSectorBuf;

	if( pSector )
	{
		memset( pSector, 0, lpVol->fdd.nBytesPerSector );
		hDsk = lpVol->hDsk;
		// ��ʼ�� prfh �ṹ
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

		// ��ʼ��FAT����ռ������ build fat table
		i += 1;
		for( n = 0; n < i; n++ )
			*(lpVol->lpdwFat+n) = NULL_SECTOR;
		// ��ʼ�����е�FAT
		n = lpVol->fdd.nSectors-1;
		for( ; i < n; i++ )
			*(lpVol->lpdwFat+i) = (i + 1);
		*(lpVol->lpdwFat+i) = NULL_SECTOR;

		// ��Ҫ���µ�FAT����
		lpVol->dwMinNeedUpdate = 0;
		lpVol->dwMaxNeedUpdate = i;	
	
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Build FAT....... \r\n")));
		// ���������ݵ���ṹ��Ա rfh
		memcpy( &lpVol->rfh, prfh, sizeof( RAM_FILE_HEAD ) );
		// ���³�Ա
		if( FSDMGR_WriteDisk(hDsk,0,1,(LPBYTE)prfh,lpVol->fdd.nBytesPerSector) != ERROR_SUCCESS )
		{
			goto _error_ret;
		}
	
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Init System Folder....... \r\n")));
		// ��ʼ�����ڵ�
		n = AllocSector( lpVol, 1 );
		DoFolderInit( lpVol, n, n, pSector );	
		// �����ڵ�����д�����
		if( FSDMGR_WriteDisk( hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (LPBYTE)lpVol->lpdwFat, lpVol->fdd.nBytesPerSector * lpVol->rfh.dwFatSectorNum ) != ERROR_SUCCESS )
		{
			goto _error_ret;
		}

		lpVol->fUpdate = 1;

		bRetv = TRUE;

	}

_error_ret:

	LeaveCriticalSection( &lpVol->csFat );  // �뿪��ͻ��

//	if( pBuf == 0 && pSector )
//		free( pSector );
	return bRetv;
}

// **************************************************
// ������static BOOL DoShellNotify( 
//						  LPKFSDVOL lpVol, 
//						  DWORD dwEvent, 
//						  DWORD dwFlags, 
//						  LPCTSTR lpcszItem1, 
//						  LPCTSTR lpcszItem2,
//						  PNODE pNode )
// ������
//	IN lpVol - �����ṹָ��
//	IN dwEvent - �¼�
//	IN dwFlags - ����ı�־
//	IN lpcszItem1 - ��Ŀһ
//	IN lpcszItem2 - ��Ŀ��
//	IN pNode - ��ǰ�޸Ľڵ�
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�����ļ�ϵͳ�ı䣬��ϵͳ����ʾ��Ϣ
// ����: 
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

	if( lpVol->pscf )  // �� notify �ص������� ��
	{	// ��

		// ��ʼ����Ŀ��Ϣ
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
		// ׼�� fci �ṹ
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
		bRetv = lpVol->pscf(&fci);  // ����ϵͳ�Ļص�����

		if( lpcszItem1 )
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: Shell Notify Item1=%s\r\n"), path1));
		if( lpcszItem2 )
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: Shell Notify Item2=%s\r\n"), path2));
	}
	return bRetv;	
}

// **************************************************
// ������static BOOL DoUpdateVolume( LPKFSDVOL lpVol )
// ������
//	IN lpVol - �����ṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	����̸��¾�����
// ����: 
// **************************************************

static BOOL DoUpdateVolume( LPKFSDVOL lpVol )
{
	DWORD dwRetv = !ERROR_SUCCESS;

	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: Update Volume=%d.\r\n"), lpVol->fUpdate));

    EnterCriticalSection( &lpVol->csFat );  // �����ͻ��

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
			{	// ���¾�ͷ�ɹ� update fat		
			    dwStart = (lpVol->dwMinNeedUpdate *  FATBYTES) / lpVol->fdd.nBytesPerSector;
			    dwEnd = (lpVol->dwMaxNeedUpdate *  FATBYTES) / lpVol->fdd.nBytesPerSector;
				// ����FAT
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

	LeaveCriticalSection( &lpVol->csFat );  //�뿪��ͻ��

	return (dwRetv == ERROR_SUCCESS);
}

// **************************************************
// ������static LPKFSDFILE FindOpened( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
// ������
// 	IN lpVol - �����ṹָ��
//	IN lpcszFileName - �ļ���
// ����ֵ��
//	����ϵͳ�Ѿ��򿪸��ļ����򷵻���ڵ�ָ�룻���򣬷���NULL
// ����������
//	����ϵͳ�Ƿ����Ѿ��򿪵��ļ����
// ����: 
// **************************************************

static LPKFSDFILE FindOpened( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
{	

	LPKFSDFILE lpFile;

	EnterCriticalSection( &lpVol->csFileList );
	lpFile = lpVol->lpOpenFile;

	while( lpFile )
	{
		if( stricmp( lpFile->lpfn->lpszFileName, lpcszFileName ) == 0 )
			break;  // ����
		lpFile = lpFile->lpNext;  // ��һ���ļ��ڵ�
	}
	LeaveCriticalSection( &lpVol->csFileList );
	return lpFile;
}

// **************************************************
// ������static LPKFSDFIND FindOpenedSearchHandle( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
// ������
// 	IN lpVol - �����ṹָ��
//	IN lpcszFileName - �ļ���
// ����ֵ��
//	����ϵͳ�Ѿ������ڲ��Ҹ��ļ��ľ�����򷵻������ָ�룻���򣬷���NULL
// ����������
//	����ϵͳ�Ƿ����Ѿ������ڲ��ҵ�ָ��
// ����: 
// **************************************************

static LPKFSDFIND FindOpenedSearchHandle( LPKFSDVOL lpVol, LPCTSTR lpcszFileName )
{
	LPKFSDFIND lpFile;

	EnterCriticalSection( &lpVol->csFindList );

	lpFile = lpVol->lpFindFile;

	while( lpFile )
	{
		if( stricmp( lpFile->lpszName, lpcszFileName ) == 0 )
			break;  //�ҵ�
		lpFile = lpFile->lpNext;
	}

	LeaveCriticalSection( &lpVol->csFindList );

	return lpFile;
}


// **************************************************
// ������static HANDLE CreateShareFile(
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
// ������
//	IN lpOldFile - ֮ǰ�Ѿ��򿪵��ļ��ڵ�ָ��
//	IN pVol - �����
//	IN hProc - ӵ���߽��̾��
//	IN lpcszFileName - ��Ҫ�򿪵��ļ���
//	IN dwAccess - 
//	IN dwShareMode
//	IN pSecurityAttributes
//	IN dwCreate
//	IN dwFlagsAndAttributes
//	IN hTemplateFile
// ����ֵ��
//	����ɹ��������ļ������������򷵻�NULL
// ����������
//	�����ļ�����
// ����: 
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
	{	// ��鹲���ͻ
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
		if( op )  // �й����ͻ�� ��
		{	// û�У������ļ��򿪽ڵ����
			lpFile = (LPKFSDFILE)malloc( sizeof( KFSDFILE ) + lpVol->fdd.nBytesPerSector );
			if( lpFile )
			{
				// ��ʼ�����ݳ�Ա init lpFile member
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
				// ���ӶԽڵ������
				InterlockedIncrement( (LPLONG)&lpOldFile->lpfn->dwRefCount );
				lpFile->lpfn = lpOldFile->lpfn;
				
				// �������ļ��ṹ����Ĵ��ļ�����add to volume list
				AddFileHandleToVolume( lpFile );
				return lpFile;
			}
		}
	}
	SetLastError(ERROR_SHARING_VIOLATION);
	return INVALID_HANDLE_VALUE;
}

// **************************************************
// ������static HANDLE CreateVolumeFile(
//							  LPKFSDVOL lpVol, 
//							  HANDLE hProc,
//							  LPCTSTR lpcszFileName ) 
// ������
//	IN lpVol - �����
//	IN hProc - ӵ���߽��̾��
//	IN lpcszFileName - ��Ҫ�򿪵ľ��ļ���
// ����ֵ��
//	����ɹ������ؾ��ļ������������򷵻�NULL
// ����������
//	�������ļ����󣬾��ļ�����ͬ�ھ���󣻾��ļ��������û��ɷ��ʵ��ļ�����
//	������ǶԴ��̾����������
// ����: 
// **************************************************

static HANDLE CreateVolumeFile(
							  LPKFSDVOL lpVol, 
							  HANDLE hProc,
							  LPCTSTR lpcszFileName ) 
{
	LPKFSDFILE lpFile;

	EnterCriticalSection( &lpVol->csFileList ); // �����ļ������ͻ��

	if( (lpFile = FindOpened( lpVol, szVolMark ) ) != 0 )  // ��ǰϵͳ�Ƿ��Ѿ��򿪾��ļ�����
	{	// �ǣ���ǰϵͳֻ�ܴ�һ�����ļ����󡣴���
		SetLastError(ERROR_SHARING_VIOLATION);
		lpFile = (LPKFSDFILE)INVALID_HANDLE_VALUE;
		goto _return;
	}
	// ϵͳ��û�У�����һ��
	lpFile = (LPKFSDFILE)malloc( sizeof( KFSDFILE ) + lpVol->fdd.nBytesPerSector );

	if( lpFile )
	{
		// ��ʼ�����ļ����� init lpFile member
		memset( lpFile, 0, sizeof( KFSDFILE ) );
		// �����ļ��ڵ�
		if( ( lpFile->lpfn = CreateFileNode( lpcszFileName ) ) != NULL )
		{	// �ɹ�,��ʼ��lpFileָ��
			lpFile->lpbCacheBuf = (LPBYTE)(lpFile+1);
			lpFile->lpVol = lpVol;
			lpFile->hProc = hProc;
			lpFile->dwAccess = 0;
			lpFile->dwShareMode = 0;
			lpFile->dwCreate = 0;
			lpFile->dwFlagsAndAttributes = 0;
			lpFile->flag = VOL_FILE;
			lpFile->dwFilePointer = 0;
			
			InterlockedIncrement( (LPLONG)&lpFile->lpfn->dwRefCount );// ���ӶԽڵ��������
			AddFileHandleToVolume( lpFile );//������ļ�����
		}
		else
		{	// ʧ��
			free( lpFile );
			lpFile = (LPKFSDFILE)INVALID_HANDLE_VALUE;
		}
	}

_return:

	LeaveCriticalSection( &lpVol->csFileList );// �뿪�ļ������ͻ��

	return lpFile;
}

// **************************************************
// ������static int IsValidFilePathName( LPCTSTR lpcFileName )
// ������
// 	IN lpcFileName - �ļ�·����
// ����ֵ��
//	�����ļ�����Ч�������ļ����Ĵ�С�����򣬷���0
// ����������
//	���һ������·�����ļ����Ƿ���Ч�����磺
//	"\\kingmos\\abc\\filenamelen.txt", return sizeof(filenamelen.txt)
// ����: 
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
			{	// ������Ч���ַ�
				SetLastError(ERROR_INVALID_NAME);
				return FALSE;
			}
			if( c == '\\' || c == '/' )
			{	// �ָ���
				if( bSeparate )
				{	// ���󣺰��������ķָ��
				    SetLastError(ERROR_INVALID_NAME);
				    return FALSE;
				}
				bSeparate = TRUE;//�зָ����˵��Ŀǰ���ļ������ļ�����
				nFileName = 0;
			}
			else
			{	// ��Ч���ļ���
				bSeparate = FALSE;
				nFileName++; //�ļ�����С 
			}
			lpcFileName++;
			n++;
		}
	}
	return nFileName;//�ļ�����С 
}


#define IS_DISABLE_DEL( wAttrib ) ( (wAttrib) & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM) )
#define FILE_CREATE_ZONE 0

// **************************************************
// ������HANDLE KFSD_CreateFile( 
//						 PVOL pVol, 
//						 HANDLE hProc, 
//						 LPCTSTR lpcszFileName, 
//						 DWORD dwAccess, 
//						 DWORD dwShareMode,
//						 PSECURITY_ATTRIBUTES pSecurityAttributes, 
//						 DWORD dwCreate,
//						 DWORD dwFlagsAndAttributes, 
//						 HANDLE hTemplateFile ) 

// ������
//	IN pVol - �����
//	IN hProc - ���ļ���ӵ���߽���
//	IN lpcszFileName-�ļ���
//	IN fAccess-��ȡ���ƣ�������ֵ��λ��ϣ�
//				GENERIC_WRITE-������
//				GENERIC_READ-д����
//	IN dwShareMode-����ģʽ��������ֵ��λ��ϣ�
//				FILE_SHARE_READ-�����
//				FILE_SHARE_WRITE-����д
//	IN pSecurityAttributes-��ȫ����(��֧�֣�ΪNULL)
//	IN dwCreate-������ʽ��������
//				CREATE_NEW-�������ļ��������ļ����ڣ���ʧ��
//				CREATE_ALWAYS-�����ļ��������ļ����ڣ��򸲸���
//				OPEN_EXISTING-���ļ��������ļ������ڣ���ʧ��
//				OPEN_ALWAYS-���ļ��������ļ������ڣ��򴴽�
//	IN dwFlagsAndAttributes-�ļ�����
//	IN hTemplateFile-��ʱ�ļ����(��֧��,ΪNULL)
// ����ֵ��
//	�ɹ������ؾ����
//	���򣺷���INVALID_HANDLE_VALUE
// ����������
//	����/���ļ�
// ����: 
//	ϵͳAPI
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

	// �����Ƿ��д ��check volume is write enable ?
	if( (dwAccess & GENERIC_WRITE) &&
		!AccessTest( lpVol ) )
	{	// ����error 
		return INVALID_HANDLE_VALUE;
	}
/*
	// �����Ƿ��д ��check volume is write enable ?
	if( (lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) &&
		(dwAccess & GENERIC_WRITE) )
	{  // ����error 
		WARNMSG( 1, ( "error in KFSD_CreateFile: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return INVALID_HANDLE_VALUE;
	}
*/
	lpcszFileName = IgnoreSpace( lpcszFileName );

	if( stricmp( lpcszFileName, TEXT( "\\Vol:" ) ) == 0 )  // �Ǿ��ļ��� ��
	{   // �ǣ��������ļ� special file name, used to DeviceIoControl
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CreateVolumeFile\r\n")));
		return CreateVolumeFile( lpVol, hProc, lpcszFileName );
	}

	DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: check filename.\r\n" ) );
	// ����ļ����Ƿ���Ч
	if( IsValidFilePathName( lpcszFileName ) == 0 )
	{	
		return INVALID_HANDLE_VALUE; // ��Ч
	}

	DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: find open.\r\n" ) );
	// ���֮ǰ�Ƿ��Ѿ��� ��
	if( (lpOldFile = FindOpened( lpVol, lpcszFileName )) != 0 )
	{	// �ǣ�����/�򿪹����ļ�
		DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: has open,CreateShareFile.\r\n")));
		return CreateShareFile( lpOldFile, pVol, hProc, lpcszFileName, dwAccess, dwShareMode, pSecurityAttributes, dwCreate, dwFlagsAndAttributes, hTemplateFile );
	}  

	DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: malloc KFSDFILE struct.\r\n" ) );
	// �����ļ�����ṹ
	lpFile = (LPKFSDFILE)malloc( sizeof( KFSDFILE ) + lpVol->fdd.nBytesPerSector  );

	if( lpFile )
    {
		SF sf;			
		int bRetv;
		
		memset( lpFile, 0, sizeof( KFSDFILE ) );
		DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:CreateFileNode.\r\n" ) );
		// �������ļ��Ľڵ����
		lpFile->lpfn = CreateFileNode(lpcszFileName);
		lpFile->lpbCacheBuf = (LPBYTE)(lpFile + 1); // ���ڶ�ȡ�����ļ�����
		
		EnterCriticalSection( &lpVol->csNode ); // �����ͻ��
		
		DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:BeginSearch.\r\n" ) );
		// ׼����������
		if( lpFile->lpfn && BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpcszFileName ) )
		{
			BOOL bUpdate = FALSE;
			
			DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:SearchNode.\r\n" ) );
			bRetv = SearchNode( &sf );  // �ڴ��������⽻�ڵ�
			if( *(sf.lpszCurName + sf.dwCurNameLen) == 0 )  // �Ƿ��Ѿ������������ļ��� ��
			{	// ��				
				if( bRetv == FALSE )
				{	// û�з��ָ��ļ��ڵ�
					DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: not found file node.\r\n")));
					if( dwCreate == CREATE_NEW ||
						dwCreate == CREATE_ALWAYS ||
						dwCreate == OPEN_ALWAYS )  //�Ƿ񴴽��µ� ��
					{  // �ǡ�not found , add a new
						SYSTEMTIME st;
						
						DEBUGMSG(FILE_CREATE_ZONE, (TEXT("KFSD: CreateFile: new file name=%s\r\n"), sf.lpszCurName));
						// ��ʼ���ļ����̽ڵ����
						memcpy( GET_NODE_PTR(lpFile)->strName, sf.lpszCurName, sf.dwCurNameLen );
						GetSystemTime( &st );
						SystemTimeToFileTime( &st, &GET_NODE_PTR(lpFile)->ft );
						GET_NODE_PTR(lpFile)->nFileLength = 0;
						GET_NODE_PTR(lpFile)->dwStartSector = NULL_SECTOR;
						GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
						GET_NODE_PTR(lpFile)->bNameLen = (BYTE)sf.dwCurNameLen;
						// ���ýڵ�����д�����
						bUpdate = bRetv = DoNodeAdd( &sf, GET_NODE_PTR(lpFile) );
						lpFile->lpfn->dwNodeSector = sf.dwCurNodeSector;
						lpFile->lpfn->index = sf.nCurNodeIndex;
					}
				}
				else
				{	// ϵͳ�и��ļ���found it, check valid
					DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: find file node.\r\n" ) );
					bRetv = FALSE; // ���滹����๤��Ҫ����������ΪFALSE
					if( dwCreate == CREATE_ALWAYS ||
						dwCreate == OPEN_ALWAYS ||
						dwCreate == OPEN_EXISTING ||
						dwCreate == TRUNCATE_EXISTING )	// �����Ƿ����Ҫ��
					{	// ����
						DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: file exist and dwCreate is valid.\r\n" ) );
						// �õ��ڵ�����
						bRetv = DoNodeGet( &sf, GET_NODE_PTR(lpFile) ); 
						if( bRetv )
						{	// 
							DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: check node attrib.\r\n" ) );
							// ���ڵ�����Ժʹ�ȡ�����Ƿ�Ϸ�
							if( ( GET_NODE_PTR(lpFile)->wAttrib & FILE_ATTRIBUTE_DIRECTORY ) ||
								( ( GET_NODE_PTR(lpFile)->wAttrib & ( FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_SYSTEM ) ) && 
								(dwAccess & GENERIC_WRITE) ) )
							{	// ��Ч�Ĵ�ȡ����
								DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile:access denied! can't write folder or readonly or system node!.\r\n" ) );
								bRetv = FALSE;  // not right to access
								SetLastError( ERROR_ACCESS_DENIED );
							}
							else
							{	// �Ϸ�,���ô��ļ��ڵ�����
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
			{	// ��Ӧ�õ�����
				WARNMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: not find(%s).\r\n", sf.lpszCurName ) );
				bRetv = FALSE;//error found
			}
			
			if( bRetv )
			{	// �ɹ�����/��
				DEBUGMSG( FILE_CREATE_ZONE, ( "KFSD: CreateFile: success! init lpFile struct.\r\n" ) );
				// ��ʼ���ļ�����ṹ
				lpFile->lpfn->dwFolderSector = sf.dwFolderStartSector;
				lpFile->dwAccess = dwAccess;
				lpFile->dwCreate = dwCreate;
				lpFile->dwFlagsAndAttributes = dwFlagsAndAttributes;
				lpFile->dwShareMode = dwShareMode;
				lpFile->hProc = hProc;
				lpFile->lpVol = lpVol;
				lpFile->dwFilePointer = 0;
				lpFile->flag = NORMAL_FILE;				
				// ���Ӷ��ļ��ڵ�������� �������ļ����󵽾�
				InterlockedIncrement( (LPLONG)&lpFile->lpfn->dwRefCount ); 
				AddFileHandleToVolume( lpFile );
				// �ͷ���������
				EndSearch( &sf );				
				if( dwCreate == CREATE_ALWAYS )
				{   // �����µ��ļ����Բ����ļ�������Ϊ0����MS��CREATE_ALWAYS���в��ԣ����ֳ��ȱ���Ϊ0����
					GET_NODE_PTR(lpFile)->wAttrib = (WORD)( dwFlagsAndAttributes & 0xFFFF );
					if( ReallocFile( lpFile, 0 ) )  // �ͷ��ļ��Ĵ��̿ռ�
					{	// 
						GET_NODE_PTR(lpFile)->nFileLength = 0;
					}
				}									
				else if( dwCreate == TRUNCATE_EXISTING ) 
				{	//  set cotent to zero
					if( ReallocFile( lpFile, 0 ) )	// �ͷ��ļ��Ĵ��̿ռ�
					{
						GET_NODE_PTR(lpFile)->nFileLength = 0;
					}
				}
				LeaveCriticalSection( &lpVol->csNode );  // �뿪��ͻ��
				if( bUpdate )	// �ļ��ڵ��Ƿ���� ��
				{	// �ǣ�֪ͨϵͳ�ļ�ϵͳ�ı仯��Ϣ
					DoShellNotify( lpVol, 
								   SHCNE_CREATE,
								   SHCNF_PATH | SHCNF_FLUSHNOWAIT,
								   lpFile->lpfn->lpszFileName,
								   NULL,
								   GET_NODE_PTR(lpFile) );
				}
				return lpFile;
			}
			// ʧ�ܣ����
			EndSearch( &sf );			
		}
		LeaveCriticalSection( &lpVol->csNode );		
		// ʧ�ܣ����
		if( lpFile->lpfn )
			DeleteFileNode( lpFile->lpfn );
		free( lpFile );
	}
	
    return INVALID_HANDLE_VALUE;
}

#undef FILE_CREATE_ZONE

// **************************************************
// ������static BOOL AddFindHandleToVolume( LPKFSDFIND lpFind )
// ������
// 	IN lpFind - ���ҽṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//	�����Ҷ��������Ҷ�������
// ����: 
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
// ������static BOOL RemoveFindHandleFromVolume( LPKFSDFIND lpFile )
// ������
// 	IN lpFile - ���ҽṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//	��AddFindHandleToVolume�෴ �����Ҷ����Ƴ����Ҷ�������
// ����: 
// **************************************************

static BOOL RemoveFindHandleFromVolume( LPKFSDFIND lpFile )
{
	LPKFSDVOL lpVol; 
	LPKFSDFIND lpf;
	BOOL bRetv = FALSE;
    
	EnterCriticalSection( &lpFile->lpVol->csFindList ); // �����ͻ��

	lpVol = lpFile->lpVol;
	lpf = lpVol->lpFindFile;
	if(  lpf == lpFile )	// ������ͷ�� ��
	{	//����ͷ
		lpVol->lpFindFile = lpFile->lpNext;
		lpFile->lpNext = NULL;
		bRetv = TRUE;
	}
	else
	{	//�ǣ���������
		while ( lpf )
		{
			if( lpf->lpNext == lpFile )
			{	// �ҵ�
				lpf->lpNext = lpFile->lpNext;
				lpFile->lpNext = NULL;
				bRetv = TRUE;
				break;
			}
			lpf = lpf->lpNext;
		}
	}
    
	LeaveCriticalSection( &lpVol->csFindList );// �뿪��ͻ��

	DEBUGMSG( bRetv == FALSE && FILE_ZONE, (TEXT("KFSD: Error RemoveFindHandleFromVolume.\r\n")));
	return bRetv;	
}

// **************************************************
// ������static BOOL AddFileHandleToVolume( LPKFSDFILE lpFile )
// ������
// 	IN lpFile - �ļ�����ṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//	���ļ���������ļ���������
// ����: 
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
// ������static BOOL RemoveFileHandleFromVolume( LPKFSDFILE lpFile )
// ������
// 	IN lpFile - ���ҽṹָ��
// ����ֵ��
//	����ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//	�� AddFileHandleToVolume �෴ ���ļ������Ƴ��ļ���������
// ����: 
// **************************************************

static BOOL RemoveFileHandleFromVolume( LPKFSDFILE lpFile )
{
	BOOL bRetv = FALSE;
	LPKFSDVOL lpVol; 
	LPKFSDFILE lpf;
	
	EnterCriticalSection( &lpFile->lpVol->csFileList );	// �����ͻ��
	
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
	
	LeaveCriticalSection( &lpFile->lpVol->csFileList );// �뿪��ͻ��
	
	DEBUGMSG( bRetv == FALSE && FILE_ZONE, (TEXT("KFSD: Error RemoveFileHandleFromVolume.\r\n")));
	
	return bRetv;
	
}


// **************************************************
// ������BOOL KFSD_CloseFile( PFILE pf )
// ������
// 	IN pf - �ļ�����
// ����ֵ��
//	����ɹ�������TRUE; ʧ�ܣ�����FALSE
// ����������
//	�ر��ļ�����
// ����: 
// **************************************************

BOOL KFSD_CloseFile( PFILE pf )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
	BOOL bRetv = TRUE;
	
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CloseFile=%s.\r\n"), lpFile->lpfn->lpszFileName));

	if( (lpFile->dwAccess & GENERIC_WRITE) && lpFile->flag == NORMAL_FILE )
	{   // malloc Ӧ���ø��õİ취�����ļ�cache ?
	 	LPBYTE pSector = (LPBYTE)malloc( lpFile->lpVol->fdd.nBytesPerSector );

		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CloseFile0.\r\n") ));
				
		if( pSector )
		{
			PNODE pNode;
			DWORD dwOffset;

			EnterCriticalSection( &lpFile->lpfn->csFileNode );

			pNode = GET_NODE_PTR( lpFile ); // �ļ����̽ڵ�
			// �ļ����̽ڵ����ļ��еı���
			dwOffset = GET_NODE_OFFSET( lpFile->lpfn->index, lpFile->lpVol );			
			DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CloseFile: write node to disk.\r\n") ));
			// ���ڵ�����д�����
			if( WriteSectors( lpFile->lpVol, lpFile->lpfn->dwNodeSector, dwOffset, (LPBYTE)pNode, GET_NODE_SIZE( pNode ), pSector ) )
			{
				LeaveCriticalSection( &lpFile->lpfn->csFileNode );  // �뿪��ͻ��
				DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CloseFile: DoUpdateVolume.\r\n") ));
				DoUpdateVolume( lpFile->lpVol );	// ���¾�����
				DEBUGMSG( FILE_ZONE, (TEXT("KFSD: CloseFile: DoShellNotify.\r\n") ));
				// ��ϵͳ��֪ͨ��Ϣ
				DoShellNotify( lpFile->lpVol, SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpFile->lpfn->lpszFileName, NULL, pNode );
			}
			else
			{
				LeaveCriticalSection( &lpFile->lpfn->csFileNode );// �뿪��ͻ��
				bRetv = FALSE;
			}            
			free( pSector );
		}
		else
			bRetv = FALSE;
    }
	if( lpFile )
	{	// �Ƴ��ļ�����
		RemoveFileHandleFromVolume( lpFile );
		// ��������ֵ�����û���κζ���ʹ���ļ��ڵ㣬���ͷ��ļ��ڵ�
		if( InterlockedDecrement( (LPLONG)&lpFile->lpfn->dwRefCount ) == 0 )
			DeleteFileNode( lpFile->lpfn );  // ɾ���ļ��ڵ�����
		free( lpFile ); // �ͷ��ļ�����ָ��
	}
	if( bRetv == FALSE )
	{
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: Error CloseFile\r\n") ) );
	}
    return bRetv;
}

// **************************************************
// ������DWORD KFSD_GetFileAttributes(
//                            PVOL pVol,
//                            LPCTSTR lpszFileName )
// ������
// 	IN pVol - �����
// 	IN lpszFileName - �ļ���
// ����ֵ��
//	����ɹ��������ļ����ԣ�ʧ��,����0xffffffff
// ����������
//	�õ��ļ�����
// ����: 
//	ϵͳAPI
// **************************************************

DWORD KFSD_GetFileAttributes(
                            PVOL pVol,
                            LPCTSTR lpszFileName )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
	SF sf;
	DWORD bRetv=0xffffffffl;

	DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: GetFileAttributes=%s.\r\n"), lpszFileName ) );

	lpszFileName = IgnoreSpace( lpszFileName ); // �����ļ���ǰ��Ŀո�

	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )  // ׼�����Ҷ���
	{
		bRetv = SearchNode( &sf );	// �����ļ��ڵ�
        if( bRetv )
		{	// �ҵ�
			DEBUGMSG(FILE_ZONE, (TEXT("KFSD: GetFileAttributes, Attrib=0x%x.\r\n"), sf.pCurNode->wAttrib));
			bRetv = sf.pCurNode->wAttrib;
		}
		else
		{	// û���ҵ��ļ�
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: GetFileAttributes, not found.\r\n")));
			bRetv = 0xffffffffl;
		}
		EndSearch( &sf ); // �ͷŲ��Ҷ���
	}
	return bRetv;
}

// **************************************************
// ������BOOL KFSD_ReadFile(
//                  PFILE pf,
//					LPVOID lpBuffer,
//                  DWORD dwNumToRead,
//                  LPDWORD lpdwNumRead, 
//					LPOVERLAPPED pOverlapped )

// ������
//	IN pf-�ļ�����
//	OUT lpBuffer-���ڽ������ݵ��ڴ�
//	IN dwNumToRead-�������ֽ���
//	OUT lpdwNumRead-����ʵ�ʶ����ֽ���������lpdwNumReadΪNULL,�򲻷���
//	IN pOverlapped-����(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	���ļ���ȡ����
// ����: 
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
	// ����ܷ����
    if( (lpFile->dwAccess & GENERIC_READ) && lpFile->flag == NORMAL_FILE )
	{	// ����
		nBytesPerSector = lpFile->lpVol->fdd.nBytesPerSector;
		EnterCriticalSection( &lpFile->lpfn->csFileNode );
		
		dwFilePointer = lpFile->dwFilePointer;
		if( dwFilePointer >= GET_NODE_PTR(lpFile)->nFileLength )
		{	// ��ǰ�ļ���ȥλ�����ļ�ĩβ��û�и��������
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );
			return TRUE;
		}
		// �ܷ��Ҫ��������� ��������ܣ�����ʵ�ʶ�������
		if( dwFilePointer + dwNumToRead > GET_NODE_PTR(lpFile)->nFileLength )
			dwNumToRead = GET_NODE_PTR(lpFile)->nFileLength - dwFilePointer;
		// 
		if( dwNumToRead )
		{
			void * lpSector;

			if( lpFile->uiCacheValid )  // cache��Ч�� ��
			{
				DWORD dwCachePosEnd = lpFile->dwCachePosStart + nBytesPerSector;
				if( dwFilePointer >= lpFile->dwCachePosStart &&
					dwFilePointer < dwCachePosEnd ) // ��ǰ��cache���ݷ���Ҫ���� ��
				{   // �ǣ�����Ҫ�� hint the cache
					DWORD dwMaxNum = MIN( dwNumToRead, dwCachePosEnd - dwFilePointer );
					// ��ȡ�ʵ�������
					dwOffset = ( dwFilePointer % nBytesPerSector ); 
					memcpy( lpBuffer, lpFile->lpbCacheBuf + dwOffset, dwMaxNum );
					lpBuffer = (LPBYTE)lpBuffer + dwMaxNum;
					dwNumToRead -= dwMaxNum;  // ���µ�������
					sr += dwMaxNum;
					dwFilePointer += dwMaxNum;
				}
			}

			if( dwNumToRead ) // ������ ��
			{	//û�У����������µ�����
				DWORD k;
				// �õ���ǰλ�õĿ�ʼ��
				dwStartSector = GetSectorOffset( lpFile->lpVol, GET_NODE_PTR(lpFile)->dwStartSector, (dwFilePointer / nBytesPerSector) );
				// �õ���ǰλ�õĿ���ƫ��
				dwOffset = (dwFilePointer % nBytesPerSector );
				//
				lpSector = lpFile->lpbCacheBuf;
				// �Ӵ��̶����µ�����
				k = ReadSectors( lpFile->lpVol, dwStartSector, dwOffset, (LPBYTE)lpBuffer, dwNumToRead, lpSector, &lpFile->uiCacheValid );
					
				ASSERT_NOTIFY( k == dwNumToRead, "not read enough bytes\r\n" );
				if( k == 0 )
				{	// ����
					LeaveCriticalSection( &lpFile->lpfn->csFileNode );
					goto _error_ret; 
				}

				sr += k;  // ʵ�ʶ�����������
				// ������ܣ�����cache
				if( lpFile->uiCacheValid )
				{	
					lpFile->dwCachePosStart = ( (dwFilePointer + k - 1) / nBytesPerSector ) * nBytesPerSector;
				}
			}
		}
		if( lpdwNumRead )
			*lpdwNumRead = sr;
		lpFile->dwFilePointer += sr;	// ���õ�ǰ�ļ���ȡλ��

		LeaveCriticalSection( &lpFile->lpfn->csFileNode ); //�뿪��ͻ��

		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: --ReadFile, readnum=%d\r\n"), sr ) );
		return TRUE;
	}
	else
	    SetLastError(ERROR_ACCESS_DENIED);
_error_ret:
	return FALSE;
}

// **************************************************
// ������BOOL KFSD_ReadFileWithSeek(
//                          PFILE pf,
//                          LPVOID lpBuffer,
//                          DWORD dwNumToRead,
//                          LPDWORD lpdwNumRead,
//							LPOVERLAPPED pOverlapped,
//                          DWORD dwLowOffset,
//                          DWORD dwHighOffset)
// ������
//	IN pf-�ļ�����
//	OUT lpBuffer-���ڽ������ݵ��ڴ�
//	IN dwNumToRead-�������ֽ���
//	OUT lpdwNumRead-����ʵ�ʶ����ֽ���������lpdwNumReadΪNULL,�򲻷���
//	IN pOverlapped-����(��֧�֣�ΪNULL)
//	IN dwLowOffset - ƫ��ֵ�ĵ�32λ
//	IN dwHighOffset - ƫ��ֵ�ĸ�32λ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	���ļ���ȡλ���赽��dwLowOffset dwHighOffsetȷ����ֵ,Ȼ����ļ���ȡ����
// ����: 
//	ϵͳAPI,��֧��
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
// ������DWORD KFSD_SetFilePointer(
//							PFILE pf,
//							LONG lDistanceToMove,
//							LPLONG pDistanceToMoveHigh,
//							DWORD dwMoveMethod )
// ������
//	IN pf - �ļ�����
//	IN lDistanceToMove-���ƫ��ֵ����32bits��
//	IN pDistanceToMoveHigh-(��32bits,��֧��,ΪNULL)
//	IN dwMethod-ƫ�Ƶ���ʼλ�ã�������
//		FILE_BEGIN-�ļ���ʼλ��
//		FILE_CURRENT-�ļ���ǰλ��
//		FILE_END-�ļ�����λ��
//
// ����ֵ��
//	�ɹ��������µ��ļ�λ��
//	���򣺷���0xffffffff
// ����������
//	�����ļ���ǰ��ȡλ��
// ����: 
//	ϵͳAPI
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
	// �Ƿ��ȡ�Ϸ� ��
	if( (lpFile->dwAccess & (GENERIC_WRITE|GENERIC_READ)) && lpFile->flag == NORMAL_FILE )
	{	// �ǣ��Ϸ�
		EnterCriticalSection( &lpFile->lpfn->csFileNode ); // �����ͻ��

		if( dwMoveMethod == FILE_BEGIN )	// �ӿ�ʼ�����ȡλ��
			cur = 0;
		else if( dwMoveMethod == FILE_CURRENT )	// ���ļ���ǰλ�ÿ�ʼ�����ȡλ��
			cur = lpFile->dwFilePointer;
		else if( dwMoveMethod == FILE_END )	// ���ļ�����λ�ÿ�ʼ�����ȡλ��
		{
			cur = GET_NODE_PTR(lpFile)->nFileLength;
		}
		cur += lDistanceToMove;	// �õ���Ҫ�趨λ��
		if( cur < 0 )
			cur = 0;	// ��Ϊ��ʼλ��
		lpFile->dwFilePointer = cur;

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );	// �뿪��ͻ��
		
		DEBUGMSG(FILE_ZONE, (TEXT("KFSD: SetFilePoiter=%d.\r\n"), cur));
		
		if( pDistanceToMoveHigh )
			*pDistanceToMoveHigh = 0;
		return cur;
	}
	else
	    SetLastError(ERROR_ACCESS_DENIED);

	return 0xFFFFFFFFl;	//����
}

// **************************************************
// ������DWORD KFSD_GetFileSize( PFILE pf, LPDWORD pFileSizeHigh )
// ������
//	IN pf-�ļ�����
//	IN pFileSizeHigh - �ļ��ߴ�ĸ�32bits(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ��������ļ���С
//	���򣺷��� INVALID_FILE_SIZE
// ����������
//	�õ��ļ���С
// ����: 
//	ϵͳAPI
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
// ������BOOL KFSD_GetFileInformationByHandle(
//                                    PFILE pf,
//                                    FILE_INFORMATION * pfi )
// ������
//	IN pf - �ļ�����
// 	OUT pfi - FILE_INFORMATION�ṹָ�룬���ڽ����ļ���Ϣ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ��ļ���Ϣ
// ����: 
//	ϵͳAPI
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

		EnterCriticalSection( &lpFile->lpfn->csFileNode ); // �����ͻ��

		pfi->dwFileAttributes = GET_NODE_PTR(lpFile)->wAttrib;
		pfi->dwVolumeSerialNumber = lpFile->lpVol->rfh.dwSerialNum;
		pfi->nFileIndexHigh = 0;
		pfi->nFileIndexLow = 0;
		pfi->nFileSizeHigh = 0;
		pfi->nFileSizeLow = GET_NODE_PTR(lpFile)->nFileLength;
		pfi->nNumberOfLinks = 1;
		// KMFS �ļ�ϵͳֻ��CreateTime������λ
		pfi->ftCreationTime.dwLowDateTime = GET_NODE_PTR(lpFile)->ft.dwLowDateTime;
		pfi->ftCreationTime.dwHighDateTime = GET_NODE_PTR(lpFile)->ft.dwHighDateTime;
		*(&pfi->ftLastAccessTime) = *(&pfi->ftCreationTime);
		*(&pfi->ftLastWriteTime) = *(&pfi->ftCreationTime);

		LeaveCriticalSection( &lpFile->lpfn->csFileNode );// �뿪��ͻ��
		
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������BOOL  KFSD_GetFileTime( 
//                      PFILE pf,
//                      FILETIME * pCreate,
//					    FILETIME * pLastAccess,
//					    FILETIME * pLastWrite)
// ������
//	IN pf - �ļ�����ָ��
//	OUT pCreate - ָ��FILETIME�ṹ��ָ�룬�ļ�����ʱ��
//	OUT pLastAccess - ָ��FILETIME�ṹ��ָ�룬�ļ����һ�δ�ȡʱ��(Ŀǰ�ݲ�֧�֣�ΪNULL)
//	OUT pLastWrite - ָ��FILETIME�ṹ��ָ�룬�ļ����һ�θ������ݵ�ʱ��(Ŀǰ�ݲ�֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�õ��ļ�ʱ��
// ����: 
//	ϵͳAPI
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
// ������BOOL KFSD_WriteFile( 
//                  PFILE pf, 
//                  LPCVOID lpBuffer,
//                  DWORD dwNumToWrite, 
//                  LPDWORD lpdwNumWrite,
//					LPOVERLAPPED pOverlapped)
// ������
//	IN pf-�ļ�����
//	IN lpBuffer-��д�����ݵĴ���ڴ�
//	IN dwNumToWrite-��д���ֽ���
//	OUT lpdwNumWrite-����ʵ��д���ֽ���������lpdwNumWriteΪNULL,�򲻷���
//	IN pOverlapped-����(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	д���ݵ��ļ�
// ����: 
//	ϵͳAPI
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
	//	����ȡ���
	if( (lpFile->dwAccess & GENERIC_WRITE) && lpFile->flag == NORMAL_FILE )
	{
		if( dwNumToWrite )
		{
			EnterCriticalSection( &lpFile->lpfn->csFileNode );	// �����ͻ��

			// ����Ƿ���Ҫ��չ�ļ���С
			fs = lpFile->dwFilePointer + dwNumToWrite;			
			if( fs > GET_NODE_PTR(lpFile)->nFileLength )	
			{	// ��Ҫ������ļ��ռ䣬���䣡
				if( ReallocFile( lpFile, fs ) == FALSE )
				{	// ����ʧ��
					LeaveCriticalSection( &lpFile->lpfn->csFileNode );	// �뿪��ͻ��
					goto _return;
				}
				// �ط����ļ��ɹ�
				GET_NODE_PTR(lpFile)->nFileLength = fs;
				DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: Realloc Sector Success, fs=%d.\r\n"), fs ) );
			}
			// �õ�д�����ݵĿ�ʼ����
			dwStartSector = GetSectorOffset( lpFile->lpVol, GET_NODE_PTR(lpFile)->dwStartSector, (lpFile->dwFilePointer / lpFile->lpVol->fdd.nBytesPerSector) );
			// �õ�д�����ݵ�������ƫ��
			dwOffset = (lpFile->dwFilePointer % lpFile->lpVol->fdd.nBytesPerSector);			
			lpSector = lpFile->lpbCacheBuf;
			// д������
			sr = WriteSectors( lpFile->lpVol, dwStartSector, dwOffset, (const unsigned char *)lpBuffer, dwNumToWrite, lpSector );
			if( sr )  // ʵ��д������
			{	// 
				if( lpdwNumWrite )
					*lpdwNumWrite = sr;
				lpFile->dwFilePointer += sr;	// �����ļ�ָ��
				lpFile->uiCacheValid = FALSE;	// ��ǰcache��Ч
				RETAILMSG(FILE_ZONE, (TEXT("KFSD: --WriteFile, writenum= %d\r\n"), sr));
				bRetv = TRUE;
			}

			LeaveCriticalSection( &lpFile->lpfn->csFileNode );	// �뿪��ͻ��
		}
		else
			bRetv = TRUE;
	}
_return:
	return bRetv;
}

// **************************************************
// ������BOOL KFSD_FlushFileBuffers(  PFILE pf ) 
// ������
// 	IN pf - �ļ�����ָ��
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	ˢ���ļ����棨�����еĻ���
// ����: 
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
// ������BOOL KFSD_WriteFileWithSeek( 
//                           PFILE pf, 
//                           LPCVOID pBuffer, 
//                           DWORD cbWrite, 
//                           LPDWORD pcbWritten, 
//                           DWORD dwLowOffset, 
//                           DWORD dwHighOffset ) 
// ������
//	IN pf-�ļ�����
//	OUT pBuffer-д�����ݵ��ڴ�
//	IN cbWrite-��д���ֽ���
//	OUT pcbWritten-����ʵ��д���ֽ��������� pcbWritten ΪNULL,�򲻷���
//	IN dwLowOffset - ƫ��ֵ�ĵ�32λ
//	IN dwHighOffset - ƫ��ֵ�ĸ�32λ
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	���ļ���ȡλ���赽��dwLowOffset dwHighOffsetȷ����ֵ,Ȼ��д���ݵ��ļ�
// ����: 
//	ϵͳAPI,��֧��
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
// ������BOOL KFSD_SetFileAttributes( 
//                           PVOL pVol,
//                           LPCTSTR lpszFileName, 
//                           DWORD dwFileAttributes ) 

// ������
//	IN pVol - �����
//	IN lpszFileName-�ļ���
//	IN dwFileAttributes-�ļ�����,������
//			FILE_ATTRIBUTE_ARCHIVE-�ĵ��ļ�
//			FILE_ATTRIBUTE_HIDDEN-�����ļ�
//			FILE_ATTRIBUTE_READONLY-ֻ���ļ�
//			FILE_ATTRIBUTE_SYSTEM-ϵͳ�ļ�
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�����
// ����: 
//	ϵͳAPI
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

	// �����Ƿ��д ��check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// ����error 
		return FALSE;
	}
/*
	// ���̾��д�� ��
	if( (lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) )
	{  // ������д������ error 
		RETAILMSG( 1, ( "error in KFSD_SetFileAttributes: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
*/
	// �����ļ������ַ���
	lpszFileName = IgnoreSpace( lpszFileName );
    if( dwFileAttributes == FILE_ATTRIBUTE_NORMAL )
        dwFileAttributes = 0;
    else
        dwFileAttributes &= ~( FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ROM | FILE_ATTRIBUTE_DIRECTORY );
	// ��ʼ����������
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
    {
        //do somthing here
		bRetv = SearchNode( &sf );	// �����ļ��ڵ�
        if( bRetv )
        {	// �ҵ�
            if( sf.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY )
			{	// ����ڵ����ļ��У�����ȥ��������
                dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
            }
            if( sf.pCurNode->wAttrib & FILE_ATTRIBUTE_DEVICE )
			{	// ����ڵ����ļ��У�����ȥ��������
                dwFileAttributes |= FILE_ATTRIBUTE_DEVICE;
            }			
			// ������
			sf.pCurNode->wAttrib = (WORD)(dwFileAttributes & 0xFFFF);
			//	���µ�����
			DoNodeUpdate( &sf, sf.pCurNode );//&node );
			//	֪ͨϵͳ�ļ��仯
			DoShellNotify( lpVol, SHCNE_ATTRIBUTES, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszFileName, NULL, sf.pCurNode );
        }
		EndSearch( &sf );  // �ͷ���������
    }
    return bRetv;
}

// **************************************************
// ������BOOL KFSD_MoveFile( 
//                  PVOL pVol,
//                  LPCTSTR lpszOldFileName, 
//                  LPCTSTR lpszNewFileName )
// ������
// 	IN pVol - �����
// 	IN lpszOldFileName - ��ǰ�Ѵ��ڵ��ļ����ļ���
// 	IN lpcszNewFileName - �µ��ļ����ļ�����(�µ��ļ����ļ����������Ѵ���)
// ����ֵ��
//	�ɹ������� TRUE
//	���򣺷��� FALSE
// ����������
//	�ƶ��ļ����ļ���
//	�µ��ļ����Ѵ��ڵ��ļ����ڲ�ͬ�ľ�;�µ��ļ������Ѵ��ڵ��ļ��б�������ͬ�ľ�
// ����: 
//	ϵͳAPI
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

	// �����Ƿ��д ��check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// ����error 
		return FALSE;
	}

	// �����̾��Ƿ��д check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // ����д error 
		//WARNMSG( 1, ( "error in KFSD_MoveFile: disk readonly, ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}
	// �����ļ����Ŀո�
	lpszOldFileName = IgnoreSpace( lpszOldFileName );
	lpszNewFileName = IgnoreSpace( lpszNewFileName );
	// �ļ����Ƿ���Ч ��
	if( IsValidFilePathName( lpszNewFileName ) == 0 )
	{	// ��Ч
		return FALSE;
	}
	// ��ǰϵͳ�Ƿ��Ѿ��򿪸��ļ�
	if( FindOpened( lpVol, lpszOldFileName ) != NULL )// LN: 2003-05-21
	{	// �Ѿ���
		SetLastError(ERROR_SHARING_VIOLATION);
		return FALSE;
	}

    EnterCriticalSection( &lpVol->csNode );  // �����ͻ��
	// ��ʼ����������
	if( BeginSearch( &sfNew, lpVol, 0, (LPTSTR)lpszNewFileName ) )
	{   // 
		bRetv = SearchNode( &sfNew ); // �����ļ����̽ڵ� 
		if( bRetv )
		{	// ���ļ����Ѿ����ڡ�error , the new file is exist
			EndSearch( &sfNew );
			bRetv = FALSE; // ���󷵻�
			goto LEAVE_CS;
		}
		// ��ʼ����������
		if( BeginSearch( &sfOld, lpVol, 0, (LPTSTR)lpszOldFileName ) )
		{	
			bRetv = SearchNode( &sfOld );// �������ļ��ڵ�
			if( bRetv = ( bRetv && !( sfOld.pCurNode->wAttrib & FILE_ATTRIBUTE_DEVICE ) ) )
			{	// �ļ����� 
				if( *(sfNew.lpszCurName + sfNew.dwCurNameLen) == 0 ) // �����ꣿ
				{	
					// ���Ѵ��ڵ��ļ��ڵ㿽�������ļ��ڵ�
					*sfNew.pCurNode = *sfOld.pCurNode; 
                    sfNew.pCurNode->bNameLen = (BYTE)sfNew.dwCurNameLen; // �ļ�������
					memcpy( sfNew.pCurNode->strName, sfNew.lpszCurName, sfNew.dwCurNameLen );
					//	�ڴ��������ļ��ڵ�
					bRetv = DoNodeAdd( &sfNew, sfNew.pCurNode );
					if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) )
					{	// �ļ��ڵ����ļ��У���Ҫ�ı��丸�ļ��е�ָ��
			            PNODE pNode;
						// ���ļ������ݵĵ�һ������
						if( FSDMGR_ReadDisk( lpVol->hDsk, sfNew.pCurNode->dwStartSector, 1, sfNew.pSector, lpVol->fdd.nBytesPerSector ) == ERROR_SUCCESS )
						{
							pNode = (PNODE)( (LPBYTE)sfNew.pSector + NODE_ALIGN_SIZE );
							if( pNode->strName[0] == '.' &&  pNode->strName[1] == '.' )
							{   // �ýڵ�ָ�򸸽ڵ� update '..' node
								pNode->dwStartSector = sfNew.dwFolderStartSector;  // �µĸ��ļ���
								// д�����
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
					{   // ɾ�����ļ�
						if( DoNodeDel( &sfOld, FALSE ) )
						{  // success, delete old
							DWORD dwEvent;
							NODE node = *sfOld.pCurNode;
							// �ɹ�ɾ�����ļ�
							// �ͷ���������
							EndSearch( &sfOld );
							EndSearch( &sfNew );
							if( node.wAttrib & FILE_ATTRIBUTE_DIRECTORY )
								dwEvent = SHCNE_RENAMEFOLDER;
							else
								dwEvent = SHCNE_RENAMEITEM;

							LeaveCriticalSection( &lpVol->csNode ); // �뿪��ͻ��
							// ��ϵͳ֪ͨ�ı�
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
// ������BOOL KFSD_DeleteFile(
//                    PVOL pVol,
//                    LPCTSTR lpszFileName )
// ������
// 	IN pVol - �����
//	IN lpszFileName - �ļ���
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	ɾ���ļ�
// ����: 
//	ϵͳAPI
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

	// �����Ƿ��д ��check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// ����error 
		return FALSE;
	}

	// �������Ƿ����д check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // ���̲���д��error 
	//	RETAILMSG( 1, ( "error in KFSD_DeleteFile: ACCESS_DENIED.\r\n" ) );
	//	SetLastError(ERROR_ACCESS_DENIED);
	//	return FALSE;	// ���󷵻�
	//}
	// �����ļ��Ŀո�
	lpszFileName = IgnoreSpace( lpszFileName );
	// ��ǰ�Ƿ��Ѿ��򿪸��ļ�
	if( FindOpened( lpVol, lpszFileName ) != NULL )// LN: 2003-05-21
	{	// �Ѵ򿪣����󷵻�
		SetLastError(ERROR_SHARING_VIOLATION);
		return FALSE;
	}
	// ��ʼ����������
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszFileName ) )
	{
		bRetv = SearchNode( &sf );  // ��������
		if( bRetv  )
		{	// �õ����ж��Ƿ��ܹ�ɾ��
			if( 0 == ( sf.pCurNode->wAttrib & ( FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_SYSTEM ) ) )
			{
				if( (bRetv = DoNodeDel( &sf, TRUE )) != FALSE )
				{	// ɾ���ɹ�
					NODE node = *sf.pCurNode;

					EndSearch( &sf );  // �ͷ���������
					// ���¾����
					DoUpdateVolume( lpVol ); // �ô�����Բ�Ҫ
					// ��ϵͳ֪ͨ�ı�
					bRetv = DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszFileName, NULL, &node );
					DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DeleteFile Sucess=%d.\r\n"), bRetv));
					return TRUE;
				}
			}
			DEBUGMSG(FILE_ZONE, (TEXT("KFSD: DeleteFile ACCESS_DENIED\r\n") ));
			SetLastError( ERROR_ACCESS_DENIED );
		}
		EndSearch( &sf );// �ͷ���������
	}
	return FALSE;
}

// **************************************************
// ������BOOL KFSD_DeleteAndRenameFile( 
//                              PVOL pVol,
//                              LPCTSTR lpszNewFileName, 
//                              LPCTSTR lpszOldFileName )
// ������
// 	IN pVol - �����
// 	IN lpcszNewFileName �C Ŀ���ļ�
// 	IN lpcszOldFileName �C Դ�ļ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��Դ�ļ����ݿ�����Ŀ���ļ�����ɾ��Դ�ļ�
// ����: 
//	ϵͳAPI
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
	// �����Ƿ��д ��check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// ����error 
		return FALSE;
	}

	// �����Ƿ��д�� check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // ����д error 
		//WARNMSG( FILE_ZONE, ( "error in KFSD_DeleteAndRenameFile: ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}

	// �����ļ��Ŀո�
	lpszNewFileName = IgnoreSpace( lpszNewFileName );
	lpszOldFileName = IgnoreSpace( lpszOldFileName );

	// ���ļ����Ƿ���Ч
	if( IsValidFilePathName( lpszNewFileName ) == 0 )
	{		
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );//�����ͻ��
	//�Ƿ�ϵͳ�Ѿ������ļ�
	if( FindOpened( lpVol, lpszOldFileName ) != NULL )
	{	// �Ѵ򿪣����󷵻�
		SetLastError(ERROR_SHARING_VIOLATION);
		goto _RET;
	}
	//�Ƿ�ϵͳ�Ѿ������ļ�
	if( FindOpened( lpVol, lpszNewFileName ) != NULL )
	{	// �Ѵ򿪣����󷵻�
		SetLastError(ERROR_SHARING_VIOLATION);
		goto _RET;
	}	
	// ��ʼ����������
	if( BeginSearch( &sfNew, lpVol, 0, (LPTSTR)lpszNewFileName ) )
	{   
		bRetv = SearchNode( &sfNew );	//�������ļ�
		if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{	// ��ʼ����������
			if( BeginSearch( &sfOld, lpVol, 0, (LPTSTR)lpszOldFileName ) )
			{
				bRetv = SearchNode( &sfOld );//�������ļ�
				if( bRetv && (sfNew.pCurNode->wAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0 )
				{	//���ļ�����
					DWORD dwSector;
					// �����ڵ���Ϣ
					dwSector = sfNew.pCurNode->dwStartSector;					
					sfNew.pCurNode->dwStartSector = sfOld.pCurNode->dwStartSector;
					sfNew.pCurNode->nFileLength = sfOld.pCurNode->nFileLength;

					if( DoNodeUpdate( &sfNew, sfNew.pCurNode ) )	//���½ڵ���Ϣ
					{					
						NODE nodeNew, nodeOld;

						nodeNew = *sfNew.pCurNode;// Ϊ�����DoShellNotify׼������
						nodeOld = *sfOld.pCurNode;// Ϊ�����DoShellNotify׼������

						if( dwSector != NULL_SECTOR )   // �ͷ��½ڵ��ȥ������
						    FreeSector( lpVol,  dwSector );
						DoNodeDel( &sfOld, FALSE ); //ɾ���Ͻڵ�
						// ��������ռ��������ڴ棬���ͷ�������Ϊ�˱�֤ DoUpdateVolume ���ڴ�Ҫ��
						EndSearch( &sfOld );	//�ͷ���������
						EndSearch( &sfNew );	//�ͷ���������

						DoUpdateVolume( lpVol ); // ���¾�
						//֪ͨϵͳ�ļ��Ѿ��ı�
						DoShellNotify( lpVol, SHCNE_UPDATEITEM, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszNewFileName, NULL, &nodeNew );
						DoShellNotify( lpVol, SHCNE_DELETE, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszOldFileName, NULL, &nodeOld );
						bRetv = TRUE;
						goto _RET;
					}
				}
				EndSearch( &sfOld );//�ͷ���������
			}
		}
		EndSearch( &sfNew );//�ͷ���������
		bRetv = FALSE;//ʧ��
	}

_RET:

	LeaveCriticalSection( &lpVol->csNode );  // LN, 2003-06-03, ADD

	return bRetv;
}

// **************************************************
// ������static BOOL _CopySectors( 
//							LPKFSDVOL lpVol, 
//							DWORD dwSectorFrom, 
//							DWORD dwSectorTo, 
//							DWORD dwSize,
//							void * lpSectorBuf0,   // temp buf
//							void * lpSectorBuf1     // temp buf
//							)

// ������
//	IN lpVol - �����
//	IN dwSectorFrom - Դ����
//	IN dwSectorTo - Ŀ������
//	IN dwSize - ��Ҫ�����Ĵ�С��Ӧ�����������ȵı�����
//	IN lpSectorBuf0 - ���ڶ�ȡ�������ݵ���ʱ����
//	IN lpSectorBuf1 - ���ڶ�ȡ�������ݵ���ʱ����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	�����ݴ�Դ����������Ŀ������
// ����: 
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

		// �õ�Դ��һ������ get next sector
		dwSectorFrom = GetSectorOffset( lpVol, dwSectorFrom, 1 );
		// �õ�Ŀ����һ������
		dwSectorTo = GetSectorOffset( lpVol, dwSectorTo, 1 );

		dwOffset += dwSectorSize;
	}

	return (dwOffset >= dwSize);
}

// **************************************************
// ������BOOL KFSD_CopyFile(
//						PVOL pVol,
//						LPCTSTR lpExistingFileName,
//						LPCTSTR lpNewFileName,
//						BOOL bFailIfExists
//						)
// ������
// 	IN pVol - �����
// 	IN lpExistingFileName �C Դ�ļ�����������ڣ�
// 	IN lpNewFileName �C Ŀ���ļ��������Դ���Ҳ���Բ����ڣ�
// 	IN bFailIfExists �C ����Ŀ���ļ����ڣ��Ƿ�ú���ʧ�ܡ�
//						����ΪTRUE����Ŀ���ļ��Ѿ����ڣ��ú���ʧ�ܣ�
//						����ΪFALSE����Ŀ���ļ��Ѿ����ڣ��ú��������Ŀ���ļ������ݲ���Դ�ļ����ݿ�����Ŀ���ļ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ļ����ݴ�Դ�ļ�������Ŀ���ļ�
// ����: 
//	ϵͳAPI
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

	// �����Ƿ��д ��check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// ����error 
		return FALSE;
	}

	// ���Ƿ��д�� check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{  // ����д error 
		//WARNMSG( FILE_ZONE, ( "error in KFSD_CopyFile: ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}
	//�����ļ�ǰ��Ŀո�
	lpExistingFileName = IgnoreSpace( lpExistingFileName );
	//�����ļ�ǰ��Ŀո�
	lpNewFileName = IgnoreSpace( lpNewFileName );
	//����ļ����Ƿ�Ϸ�
	if( IsValidFilePathName( lpNewFileName ) == 0 )
	{
		WARNMSG( FILE_ZONE, ( "error in KFSD_CopyFile: not valid file path name.\r\n" ) );
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode );	// �����ͻ��

	//�Ƿ�ϵͳ�Ѿ������ļ�
	if( FindOpened( lpVol, lpNewFileName ) != NULL )
	{	// �Ѵ򿪣����󷵻�
		SetLastError(ERROR_SHARING_VIOLATION);
		goto RETV;
	}	

	//��ʼ����������
	if( BeginSearch( &sfNew, lpVol, NULL, (LPTSTR)lpNewFileName ) )
	{
		if( (bExistFile = SearchNode( &sfNew )) )	//�������ļ�
		{	//���ļ�����
			if( bFailIfExists || (sfNew.pCurNode->wAttrib&FILE_ATTRIBUTE_DIRECTORY) )
			{	// ������ļ��� �� bFailIfExistsΪTRUE,���˳�
				WARNMSG( 1, ( "error in KFSD_CopyFile: new file (%s) exist or is a directory !.\r\n", lpNewFileName ) );
				EndSearch( &sfNew );//�ͷ���������
				goto RETV;
			}
		}
		//��ʼ����������
	    if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpExistingFileName ) )
		{	//
			if( SearchNode( &sf )  )	//�������ļ�
			{   // �������ļ� find the file
				PNODE pNode, pNodeNew;
				pNode = sf.pCurNode;
				pNodeNew = sfNew.pCurNode;
				if( ( pNode->wAttrib&(FILE_ATTRIBUTE_DIRECTORY) ) == 0 )
				{   // ���ļ����ļ��� not directory, is file
					DWORD dwStartSector = NULL_SECTOR;

					if( bExistFile )
					{   // ���ļ��Ѿ����ڣ�����Ϊ���ļ������ļ���С
						// ���ļ��ļ���СΪ���ļ���С��the new file is exist, now extend it's space
					    dwStartSector = ReallocSector( lpVol, pNodeNew->dwStartSector, ( (pNode->nFileLength + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector) );
					}
					else
					{	// ���ļ������ڣ���Ҫ����һ���µ�

						memset( pNodeNew, 0, MAX_NODE_BYTES );
						memcpy( pNodeNew, pNode, GET_NODE_ATTRIB_SIZE(pNode) );//LN:2003-04-30������
						memcpy( pNodeNew->strName, sfNew.lpszCurName, sfNew.dwCurNameLen );//LN:2003-04-30������
						pNodeNew->bNameLen = (BYTE)sfNew.dwCurNameLen;//LN:2003-04-30������
						pNodeNew->dwStartSector = NULL_SECTOR;
						pNodeNew->nFileLength = 0;
						// �����ļ��ڵ�
                        if( DoNodeAdd( &sfNew, pNodeNew ) )
						{	// Ϊ�����ļ������С
							dwStartSector = ReallocSector( lpVol, pNodeNew->dwStartSector, ( (pNode->nFileLength + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector) );
							if( dwStartSector == NULL_SECTOR )
							{	// ʧ�ܣ�ɾ�������ӵ��ļ��ڵ�
								DoNodeDel( &sfNew, FALSE );
							}
						}
					}
					//
					if( dwStartSector != NULL_SECTOR )
					{	// ���ļ�׼���ɹ������ڿ����ļ�����
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
						{	// ����ʧ�ܣ�ɾ���ڵ�
							DoNodeDel( &sfNew, TRUE );
						}
						DoUpdateVolume( lpVol );	//���¾�
					}
				}
			}
			else
			{
				WARNMSG( 1, ( "error in KFSD_CopyFile: file not find(%s)!.\r\n", lpExistingFileName ) );
			}
			EndSearch( &sf );  // �ͷ���������
		}
		EndSearch( &sfNew );// �ͷ���������
	}
	
RETV:
	LeaveCriticalSection( &lpVol->csNode );	// �뿪��ͻ��

	return bRetv;
}

// **************************************************
// ������BOOL KFSD_SetEndOfFile( PFILE pf )
// ������
// 	IN pf - �ļ�����
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�����λ��
// ����: 
//	ϵͳAPI
// **************************************************

BOOL KFSD_SetEndOfFile( PFILE pf )
{
    LPKFSDFILE lpFile = (LPKFSDFILE)pf;
    //  do somthing here    
    DWORD fs = lpFile->dwFilePointer;
	BOOL bRetv = FALSE;

    DEBUGMSG(FILE_ZONE, (TEXT("KFSD: SetEndOfFile.\r\n")));
	// �ļ��Ƿ��д? ����д���
	if( (lpFile->dwAccess & GENERIC_WRITE) &&
		lpFile->flag == NORMAL_FILE )
	{
		EnterCriticalSection( &lpFile->lpfn->csFileNode ); // �����ͻ��

		if( fs != GET_NODE_PTR(lpFile)->nFileLength )	//���ļ���С�뵱ǰ�ļ�ʵ�ʴ�С��һ���� ��
			bRetv = ReallocFile( lpFile, fs ); // ��Ҫ���·����ļ���С
		else
			bRetv = TRUE;//ͬ����С������Ҫ��ʲô
        if( bRetv )
		{	// �趨���ļ���С
            GET_NODE_PTR(lpFile)->nFileLength = fs;
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );//�뿪��ͻ��
		    DoUpdateVolume( lpFile->lpVol );//���¾�
		}
		else
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );//�뿪��ͻ��
		lpFile->uiCacheValid = FALSE;//��Ч�ļ�cache
	}
	return bRetv;
}

// **************************************************
// ������BOOL KFSD_SetFileTime(
//						PFILE pf, 
//						const FILETIME *pCreate,
//						const FILETIME *pLastAccess,
//						const FILETIME *pLastWrite )
// ������
// 	IN pf - �ļ����
//	IN pCreate-FILETIME�ṹָ�룬�ļ�����ʱ��
//	IN pLastAccess-FILETIME�ṹָ�룬�ļ���ȡʱ��(Ŀǰ��֧�֣�ΪNULL)
//	IN pLastWrite-FILETIME�ṹָ�룬�ļ�����ʱ��(Ŀǰ��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ�ʱ��
// ����: 
//	ϵͳAPI
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
			EnterCriticalSection( &lpFile->lpfn->csFileNode );//�����ͻ��
			RETAILMSG(FILE_ZONE, (TEXT("KFSD: SetFileTime, hi=0x%x, lo=0x%x\r\n"), pCreate->dwHighDateTime, pCreate->dwLowDateTime));
			GET_NODE_PTR(lpFile)->ft.dwLowDateTime = pCreate->dwLowDateTime;
			GET_NODE_PTR(lpFile)->ft.dwHighDateTime = pCreate->dwHighDateTime;
			LeaveCriticalSection( &lpFile->lpfn->csFileNode );//�뿪��ͻ��
			return TRUE;
		}
	}

    return FALSE;
}

// **************************************************
// ������static BOOL CloseVolume( LPKFSDVOL lpVol )
// ������
// 	IN lpVol - �����
// ����ֵ��
//	����ɹ�, ����TRUE; ���򣬷���FALSE
// ����������
//	�رվ�
// ����: 
// **************************************************

static BOOL CloseVolume( LPKFSDVOL lpVol )
{
    //do somthing here
	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: CloseVolume.\r\n")));

    DoUpdateVolume( lpVol );//�Ƚ������ݸ��µ�����

    if( RemoveVolume( lpVol ) )	//�Ƴ��þ����
    {
        DeleteVolume( lpVol );	//ɾ�������
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������BOOL KFSD_CloseVolume( PVOL pVol )
// ������
// 	IN lpVol - �����
// ����ֵ��
//	����ɹ�, ����TRUE; ���򣬷���FALSE
// ����������
//	�رվ�
// ����: 
//	�ļ�ϵͳ��������
// **************************************************

BOOL KFSD_CloseVolume( PVOL pVol )
{    
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
	if( lpVol )
	    return CloseVolume( (LPKFSDVOL)pVol );
	return FALSE;
}

// **************************************************
// ������BOOL KFSD_CreateDirectory( 
//							PVOL pVol, 
//							LPCTSTR lpszPathName,
//							PSECURITY_ATTRIBUTES pSecurityAttributes)
// ������
//	lpcszPath-Ŀ¼·����
//	lpSecurityAttributes-��ȫ����(��֧�֣�ΪNULL)
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�����ļ���
// ����: 
//	ϵͳAPI
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

	// �����Ƿ��д ��check volume is write enable ?
	if( !AccessTest( lpVol ) )
	{	// ����error 
		return FALSE;
	}

	// �����Ƿ����д, check volume is write enable ?
	//if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	//{   // error 
		//WARNMSG( FILE_ZONE, ( "error in KFSD_CreateDirectory: ACCESS_DENIED.\r\n" ) );
		//SetLastError(ERROR_ACCESS_DENIED);
		//return FALSE;
	//}
	//
	// �����ļ�ǰ�Ŀո�
	lpszPathName = IgnoreSpace( lpszPathName );
	// �ļ����Ƿ���Ч
	if( IsValidFilePathName( lpszPathName ) == 0 )
	{		
		return FALSE;
	}

	EnterCriticalSection( &lpVol->csNode ); // �����ͻ��
	// ��ʼ����������
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszPathName ) )
	{			
		bRetv = SearchNode( &sf );	// �����ļ��ڵ�
		if( bRetv == FALSE )
		{	// δ���ָ��ļ�
			SYSTEMTIME st;
			// ��ʼ���ļ��ڵ�����
			memset( sf.pCurNode, 0, sizeof( NODE ) );
			sf.pCurNode->dwStartSector = NULL_SECTOR;
			sf.pCurNode->wAttrib = FILE_ATTRIBUTE_DIRECTORY;
			memcpy( sf.pCurNode->strName, sf.lpszCurName, sf.dwCurNameLen );
			sf.pCurNode->bNameLen = (BYTE)sf.dwCurNameLen;				
			GetSystemTime( &st );
		    SystemTimeToFileTime( &st, &sf.pCurNode->ft );
			// ����һ���ļ��нڵ�
			bRetv = DoFolderChange( &sf, sf.pCurNode, OP_ADD );
			if( bRetv )
			{	// ����ɹ������¾��������
				DoUpdateVolume( lpVol );
				// ��ϵͳ֪ͨ�ı���Ϣ
				DoShellNotify( lpVol, SHCNE_MKDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszPathName, NULL, sf.pCurNode );
			}
		}
		else
		{	// �ļ����Ѿ����ڣ��˳�
			SetLastError(ERROR_ALREADY_EXISTS);
			bRetv = FALSE;
		}
		EndSearch( &sf );//�ͷ���������
	}
	
	LeaveCriticalSection( &lpVol->csNode ); // �뿪��ͻ��

	return bRetv;
}

// **************************************************
// ������BOOL KFSD_DeviceIoControl(
//					DWORD pf,
//					DWORD dwIoControlCode,
//					LPVOID lpInBuf,
//					DWORD nInBufSize,
//					LPVOID lpOutBuf,
//					DWORD nOutBufSize,
//					LPDWORD lpBytesReturned
//    )

// ������
//	IN pf-�ļ�����
//	IN dwIoControlCode-I/O��������ļ�ϵͳ������
//			IOCTL_DISK_SCAN_VOLUME��֪ͨ�ļ�ϵͳȥɨ���Ĵ���
//			IOCTL_DISK_FORMAT_VOLUME��֪ͨ�ļ�ϵͳȥ��ʽ����
//	IN lpInBuf-���뻺��
//	IN nInBufSize�����뻺��ĳߴ�
//	OUT lpOutBuf���������
//	IN nOutBufSize���������ĳߴ�
//	OUT lpBytesReturned-���ص����ݵĳߴ磬�������ڷ���ʱ�Ѵ���lpOutBuf
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�ļ�ϵͳ���ƹ���
// ����: 
//	ϵͳAPI
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
	case  IOCTL_DISK_FORMAT_VOLUME: // ��ʽ������ format disk
		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: DeviceIoControl - Format Volume.\r\n")) );
		if( lpFile->lpVol->lpOpenFile == lpFile && 
			lpFile->lpNext == NULL &&
			AccessTest( lpFile->lpVol ) )
			//(lpFile->lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY) == 0 )
		{	// �þ�û�б���ļ�����/ʹ��
			TCHAR szName[1];
			NODE node;
			
			DoVolumeFormat( lpFile->lpVol );
			memset( &node, 0, sizeof( node ) );
			szName[0] = 0;
			// ֪ͨϵͳ�ı���Ϣ
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
		//	���ô���Ϊֻ�����ڲ�ʹ�ã�
		//lpFile->lpVol->fdd.dwFlags |= DISK_INFO_FLAG_READONLY;
		//	���ô���Ϊֻ�����ڲ�ʹ�ã�
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
// ������BOOL KFSD_FindClose( PFIND pfd )
// ������
// 	IN pfd - ���Ҷ���
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	�رղ��Ҷ���
// ����: 
//	ϵͳAPI
// **************************************************

BOOL KFSD_FindClose( PFIND pfd )
{
    LPKFSDFIND lpfd = (LPKFSDFIND)pfd;
    //  do somthing here
	// �������Ƴ�����
	RemoveFindHandleFromVolume( lpfd );
	// �ͷŶ�����ڴ�
    free( lpfd );
    return TRUE;
}

// **************************************************
// ������HANDLE KFSD_FindFirstFile( 
//                         PVOL pVol, 
//                         HANDLE hProc, 
//                         LPCTSTR lpszFileSpec, 
//                         FILE_FIND_DATA *  pfd )
// ������
// 	IN pVol - �����
// 	IN hProc - �ö������ӵ���߾��
// 	IN lpszFileSpec - �ļ��������Ժ�ͨ���ַ���
//	OUT pfd - FILE_FIND_DATA���ݽṹָ�룬���ڽ����ҵ�������
// ����ֵ��
//	�ɹ���������Ч�ľ��
//	���򣺷���INVALID_HANDLE_VALUE
// ����������
//	����ƥ����ļ�
// ����: 
//	ϵͳAPI
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
	// �����ļ���ǰ�Ŀո�
	lpszFileSpec = IgnoreSpace( lpszFileSpec );
    fLen = strlen( lpszFileSpec );
	if( fLen == 0 )
		return INVALID_HANDLE_VALUE;
    // ������Ҷ���
	lpsf = (KFSDFIND *) malloc( sizeof( KFSDFIND ) + (fLen + 1)*sizeof( TCHAR ) );
	memset( pfd, 0, sizeof( FILE_FIND_DATA ) );
    if( lpsf )
    {        
        lpsf->lpszName = (LPTSTR)(lpsf+1);
        // this code is now check the file 
		strcpy( lpsf->lpszName, lpszFileSpec );
		// ��ʼ����������
		if( BeginSearch( lpsf, lpVol, hProc, lpsf->lpszName ) )
        {	//
			bRetv = SearchNode( lpsf );//�����ļ�����
			if( bRetv )
			{	// �����ļ�
				DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile:Has Found.\r\n")));
				// if all is ok, do this...
				memset( pfd, 0, sizeof( FILE_FIND_DATA ) );
				
				DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile: node=%s.\r\n"), lpsf->pCurNode->strName ));
				
				strncpy( pfd->cFileName, lpsf->pCurNode->strName, lpsf->pCurNode->bNameLen );
				// ��ʼ�� pfd �ṹ
				pfd->dwFileAttributes = lpsf->pCurNode->wAttrib;
				pfd->ftCreationTime.dwLowDateTime = lpsf->pCurNode->ft.dwLowDateTime;
				pfd->ftCreationTime.dwHighDateTime = lpsf->pCurNode->ft.dwHighDateTime;
				*(&pfd->ftLastAccessTime)= *(&pfd->ftCreationTime);
				*(&pfd->ftLastWriteTime) = *(&pfd->ftCreationTime);
				pfd->nFileSizeLow = lpsf->pCurNode->nFileLength;
				pfd->nFileSizeHigh = 0;
				
				EndSearch( lpsf );	// �ͷ���������
				
				// ������������������������� add to volume list
				AddFindHandleToVolume( lpsf );
				return lpsf;
            }
           	DEBUGMSG(FILE_ZONE, (TEXT("KFSD: FindFirstFile:not Found��\r\n")));   
			EndSearch( lpsf );// �ͷ���������
        }
		// �����ͷ�֮ǰ�ѷ���Ĳ��Ҷ���
        free( lpsf );
    }
    return INVALID_HANDLE_VALUE;
}

// **************************************************
// ������BOOL KFSD_FindNextFile( 
//                      PFIND pfd,
//                      FILE_FIND_DATA * pffd ) 
// ������
// 	IN pfd - ���Ҷ���
//	OUT pffd - FILE_FIND_DATA���ݽṹָ�룬���ڽ����ҵ�������
// ����ֵ��
//	�ɹ�������TRUE
//	���򣺷���FALSE
// ����������
//	������һ��ƥ����ļ�
// ����: 
//	ϵͳAPI
// **************************************************

BOOL KFSD_FindNextFile( 
                      PFIND pfd,
                      FILE_FIND_DATA * pffd ) 
{
    LPKFSDFIND lpfd = (LPKFSDFIND)pfd;
    BOOL retv = FALSE;

    //  do somthing here
	// ��ʼ����������
	if( BeginSearch( lpfd, lpfd->lpVol, lpfd->hProc, lpfd->lpszName ) )
    {	// ������һ���ļ�
		if( TRUE == (retv = SearchFolder( lpfd, TRUE, ALL_NODES )) )   // search next node
		{	//�ҵ�����Ҫ��ģ���ʼ��pffd�ṹ
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
			SetLastError(ERROR_NO_MORE_FILES);  // û���ҵ�
        EndSearch( lpfd ); // �ͷ������ṹ
    }
    return retv;
}

// **************************************************
// ������BOOL KFSD_GetDiskFreeSpace(
//                          PVOL pVol,
//                          LPCTSTR lpszPathName,
//                          LPDWORD pSectorsPerCluster,
//                          LPDWORD pBytesPerSector,
//                          LPDWORD pFreeClusters,
//                          LPDWORD pClusters )
// ������
// 	IN pVol - �����
//	IN pSectorsPerCluster - ���ڽ���ÿ���ص�������
//	IN pBytesPerSector - ���ڽ���ÿ���������ֽ���
//	IN pFreeClusters - ���ڽ��ܵ�ǰ���̿��еĴ���
//	IN pClusters - ���ڽ��ܵ�ǰ�����ܵĴ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ���ǰ��Ĵ���ʹ�����
// ����: 
//	ϵͳAPI
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
// ������static BOOL IsValidDisk( PRFH prfh, FSD_DISK_DATA * pfdd )
// ������
// 	IN prfh - �ļ�ϵͳͷ�ṹ
// 	IN pfdd - FSD_DISK_DATA�ṹָ�룬����������Ϣ
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	����ڴ����ϵ��ļ�ϵͳ�Ƿ���Ч
// ����: 
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
// ������BOOL KFSD_MountDisk( HDSK hDsk )
// ������
// 	IN hDsk - ���̶�����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��װ���̾�
// ����: 
//	ϵͳAPI
// **************************************************
#define DEBUG_KFSD_MountDisk 0
BOOL KFSD_MountDisk( HDSK hDsk )
{
    BOOL retv = FALSE;
    LPKFSDVOL lpVol;
	
	if( FindVolume( hDsk ) )
	{	// �ô��̾��Ѿ���ʹ��
		WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: Find Volume in Mount, return.\r\n") );
		return FALSE;
	}
	// �����¾�
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
                DeleteVolume( lpVol ); // �ͷ�֮ǰ����ľ����
                return FALSE;
            }
            lpVol->hDsk = hDsk;

            dwfat = ( ( (lpVol->fdd.nSectors * sizeof( DWORD ) + lpVol->fdd.nBytesPerSector - 1) / lpVol->fdd.nBytesPerSector ) * lpVol->fdd.nBytesPerSector );
			DEBUGMSG( DEBUG_KFSD_MountDisk, ( "KFSD: Alloc FAT size=%d.\r\n", dwfat ) );
			// ΪFAT�����ڴ�
            lpVol->lpdwFat = (DWORD*)malloc( dwfat );
            if( lpVol->lpdwFat )
            {   // Ϊ���д�������仺��
				DEBUGMSG( DEBUG_KFSD_MountDisk, ( "KFSD: Alloc pSector.\r\n" ) );
                lpVol->lpSectorBuf = pSector = (WORD*)malloc( lpVol->fdd.nBytesPerSector );				
                if( pSector )
                {	// ����ͷ(����0����)
                    if( ERROR_SUCCESS == FSDMGR_ReadDisk( hDsk, 0, 1, (LPBYTE)pSector, lpVol->fdd.nBytesPerSector ) )
                    {
                        DEBUGMSG( DEBUG_KFSD_MountDisk, (TEXT("KFSD: Read 0 Sector Success.\r\n") ) );
						prfh = (PRFH)pSector;
						if( IsValidDisk( prfh, &lpVol->fdd ) )
                        {   // ��Ч��KMFS �ļ�ϵͳ
                            memcpy( &lpVol->rfh, prfh, sizeof( RAM_FILE_HEAD ) );
							// ��FAT����
                            FSDMGR_ReadDisk( hDsk, lpVol->rfh.dwFatStartSector, lpVol->rfh.dwFatSectorNum, (LPBYTE)lpVol->lpdwFat, lpVol->fdd.nBytesPerSector * lpVol->rfh.dwFatSectorNum );
							DEBUGMSG( DEBUG_KFSD_MountDisk, ( "KFSD: valid kingmos file system.\r\n" ) );
                        }
                        else
                        {   // �� KMFS �ļ�ϵͳ����ʽ�����̡�no esoft ram file sys or other file sys or has some error
							WARNMSG( DEBUG_KFSD_MountDisk, ( "KFSD: invalid kingmos file sys, now to format it.\r\n" ) );
							DoVolumeFormat( lpVol );
                        }
                        
				        DEBUGMSG( DEBUG_KFSD_MountDisk, ("KFSD: FAT INFO:freecount=%d,ver=%d,data=%d,fatnum=%d,freesector=%d,serial=%x\r\n", lpVol->rfh.nFreeCount,lpVol->rfh.ver,lpVol->rfh.dwDataStartSector,lpVol->rfh.dwFatSectorNum,lpVol->rfh.dwFreeSector, lpVol->rfh.dwSerialNum ) );
						// ���ļ�ϵͳ������ע������
						lpVol->hVol = FSDMGR_RegisterVolume( hDsk, NULL, (DWORD)lpVol );
                        if( lpVol->hVol )
                        {
                            DEBUGMSG(DEBUG_KFSD_MountDisk, (TEXT("KFSD: RegisterVolume Sucess.\r\n")));
							// �õ�����
							FSDMGR_GetVolumeName( lpVol->hVol, lpVol->szVolName, sizeof( lpVol->szVolName ) );
							lpVol->nNodesPerSector = (WORD)( (lpVol->fdd.nBytesPerSector / NODE_ALIGN_SIZE) );
							// ����KMFS �ļ�ϵͳ������
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
        DeleteVolume( lpVol ); // �����ͷž�
    }
    else
    {
    	WARNMSG( DEBUG_KFSD_MountDisk, ("KFSD: error Can't CreateVolume struct.\r\n") );
    }
    return FALSE;    
}

// **************************************************
// ������void KFSD_Notify(
//                PVOL pVol,
//                DWORD dwFlags )
// ������
// 	IN pVol - �����
//	IN dwFlags - ֪ͨ��Ϣ
// ����ֵ��
//	��
// ����������
//	�ù��������ļ�ϵͳ������֮����и��ֿ��Ƶ�����
// ����: 
//	�ļ�ϵͳ��������ӿ�
// **************************************************

void KFSD_Notify(
                PVOL pVol,
                DWORD dwFlags )
{
    LPKFSDVOL lpVol = (LPKFSDVOL)pVol;
    //  do somthing here
    switch( dwFlags )
    {
    case FSNOTIFY_POWER_ON:  // �����¼�
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: FSNOTIFY_POWER_ON\r\n" ) ) );
        break;
    case FSNOTIFY_POWER_OFF:	// �ػ��¼�
		DoUpdateVolume( lpVol ); // ���¾�������д�����
        break;
    case FSNOTIFY_DEVICES_ON:   // the device has resumed after a suspended mode, and devices are now available for use
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: FSNOTIFY_DEVICES_ON\r\n") ) );
        break;
    }
}

// **************************************************
// ������BOOL KFSD_RegisterFileSystemFunction( 
//                                       PVOL pVol,
//                                       SHELLFILECHANGEFUNC pscf )
// ������
// 	IN pVol - �����
//	IN pscf - ϵͳ֪ͨ�ص�����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ע���ļ�ϵͳϵͳ֪ͨ�ص�����
// ����: 
//	�ļ�ϵͳ��������ӿ�
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
// ������BOOL KFSD_RemoveDirectory( 
//                         PVOL pVol, 
//                         LPCTSTR lpszPathName )
// ������
// 	IN pVol - �����
// 	IN lpszPathName - �ļ�����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	ɾ���ļ���
// ����: 
//	ϵͳAPI
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

	// ���Ƿ��д��check volume is write enable ?
	if( lpVol->fdd.dwFlags & DISK_INFO_FLAG_READONLY )
	{  // ����д��error 
		WARNMSG( FILE_ZONE, ( "error in KFSD_RemoveDirectory: ACCESS_DENIED.\r\n" ) );
		SetLastError(ERROR_ACCESS_DENIED);
		return FALSE;
	}
	//

	// �����ַ���ǰ���Ŀո�
	lpszPathName = IgnoreSpace( lpszPathName );
	len = strlen( lpszPathName );
	// ��ǰ�Ƿ�򿪸��ļ���
	if( FindOpened( lpVol, lpszPathName ) ) // LN: 2003-05-21
	{	// �Ѿ���
		SetLastError(ERROR_SHARING_VIOLATION);
		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory, FindOpened.\r\n") ));
		return FALSE;
	}
	// ��ǰ�Ƿ�򿪸��ļ��е��������
	if( FindOpenedSearchHandle( lpVol, lpszPathName ) )// LN: 2003-05-21
	{	// ��
		SetLastError(ERROR_SHARING_VIOLATION);
		DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory, FindOpenedSearchHandle.\r\n") ));
		return FALSE;
	}

	DEBUGMSG( FILE_ZONE, ( TEXT( "KFSD: RemoveDirectory, BeginSearch.\r\n" ) ) );
	// ��ʼ���������
	if( BeginSearch( &sf, lpVol, NULL, (LPTSTR)lpszPathName ) )    
    {		
		DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory, SearchNode.\r\n") ) );
		if( SearchNode( &sf ) )	// �ҵ��� ��
		{	// �ҵ�
			DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory, SearchNode OK.\r\n") ));
			if( sf.pCurNode->wAttrib&FILE_ATTRIBUTE_DIRECTORY )
			{   
				DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory, Is dir, now call del.\r\n") ) );
				if( DoFolderChange( &sf, NULL, OP_DEL ) )	// ɾ���ļ���
				{
					DEBUGMSG( FILE_ZONE, ( TEXT("KFSD: RemoveDirectory, UpdateValume del.\r\n") ) );
					// ���¾�����
					DoUpdateVolume( lpVol );
					// ��ϵͳ��֪ͨ��Ϣ
					DoShellNotify( lpVol, SHCNE_RMDIR, SHCNF_PATH | SHCNF_FLUSHNOWAIT, lpszPathName, NULL, sf.pCurNode );//&node );
					bRetv = TRUE;
				}
			}
			else
				SetLastError( ERROR_ACCESS_DENIED );
		}
		EndSearch( &sf );	// �ͷ���������
	}
    DEBUGMSG( FILE_ZONE, (TEXT("KFSD: RemoveDirectory retv=%d\r\n"), bRetv));
    return bRetv;
}

// **************************************************
// ������BOOL KFSD_UnmountDisk( HDSK hDsk )
// ������
// 	IN hDsk - ���̶�����
// ����ֵ��
//	����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//	��װ��
// ����: 
//	�ļ�ϵͳ�ӿڽ���
// **************************************************

BOOL KFSD_UnmountDisk( HDSK hDsk )
{    
    LPKFSDVOL lpVol;
    
	DEBUGMSG( FILE_ZONE, ( TEXT( "++KFSD: UnmountDisk=%d\r\n" ), hDsk ) );
	//return FALSE; ///base file system, ??????	

    if( ( lpVol = FindVolume( hDsk ) ) != NULL )  // ��ǰϵͳ�Ƿ����û�йرյ��ļ�/�������
    {   // ���ڣ��ص�����
		// ��������
		while( lpVol->lpFindFile )
		{
			KFSD_FindClose( (PFIND)lpVol->lpFindFile );
		}
		// �ļ�����
		while( lpVol->lpOpenFile )
		{
			KFSD_CloseFile( (PFILE)lpVol->lpOpenFile );
		}
		// ���еĶ��ص���ע����
		FSDMGR_DeregisterVolume( lpVol->hVol ); // 
		// �رվ�
		CloseVolume( lpVol );
		return TRUE;
    }
	
	DEBUGMSG( FILE_ZONE, ( TEXT("--KFSD: UnmountDisk\r\n") ) );
	return FALSE;
}

// **************************************************
// ������
// ������
// 	IN
// 	OUT
// 	IN/OUT
// ����ֵ��
// ����������
// ����: 
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

