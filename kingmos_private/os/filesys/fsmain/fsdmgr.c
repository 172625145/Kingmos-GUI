/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：文件系统管理器
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：
	2005-02-24, FileMgr_RegisterVolume 加入 设备文件夹的隐含标志
******************************************************/

#include <eframe.h>
#include <efile.h>
#include <edevice.h>
#include <eapisrv.h>
#include <eobjlist.h>
#include <efsdmgr.h>
#include <epdev.h>
#include <epfs.h>
#include <eshell.h>
#include <filesrv.h>
#include <estring.h> 
#include <eassert.h>

#define MAX_VOL_NUM 16
#define FSDMGR_ZONE 0

typedef struct _FSDMGR
{
    struct _FSDMGR * lpNext;
    LPFSDDRV lpfsd;
    LPTSTR lpszFSDName;
}FSDMGR, * PFSDMGR;

//static PFSDMGR pSysfsd = NULL;
// 查找文件对象链表
static _LPFINDDATA lpFindObjList = NULL;
// 卷对象链表
static _LPVOLUME lpVolLink=NULL;

/*
// **************************************************
// 声明：
// 参数：
//		
// 返回值：
//		
// 功能描述：
//		
// 引用: 
// **************************************************

static LPFSDDRV GetFSD( LPCTSTR lpcszFSDName )
{
    PFSDMGR pfsd = pSysfsd;
    while( pfsd )
    {
        if( stricmp( pfsd->lpszFSDName, lpcszFSDName ) == 0 )
            return pfsd->lpfsd; //found
        pfsd = pfsd->lpNext;
    }
    return NULL;
}
*/


// **************************************************
// 声明：HANDLE WINAPI FileMgr_AttachDevice( HANDLE hFSD, LPCTSTR lpszDeviceName )
// 参数：
//		IN hFSD - 文件系统驱动程序对象句柄（由文件系统驱动程序 调用FileMgr_RegisterFSD/Dev_RegisterFSD所传递的参数）
//		IN lpszDeviceName - 需要与文件系统关联的设备名（如：DSK2，RAM1）
// 返回值：
//		假如成功，返回非NULL的句柄；否则，返回NULL
// 功能描述：
//		将一个设备与已经注册的文件系统关联
// 引用: 
//		系统API
// **************************************************
HANDLE WINAPI FileMgr_AttachDevice( HANDLE hFSD, LPCTSTR lpszDeviceName )
{
    LPFSDDRV lpfsd = (LPFSDDRV)hFSD;
    TCHAR buf[6];
	
    if( lpfsd )
    {   // 分配_FSDDSK结构对象     
        _LPFSDDSK lpdsk = (_LPFSDDSK)malloc( sizeof( _FSDDSK ) );
        if( lpdsk )
        {   // 将设备名组合成 "XXX1:" 的形式
			strncpy( buf, lpszDeviceName, 4 );
			buf[4] = ':';
			buf[5] = 0;	  
			// 打开该设备文件
			lpdsk->hFile = CreateFile(
				buf, 
				GENERIC_WRITE|GENERIC_READ, 
				0,
				NULL, 
				OPEN_EXISTING,
				0, 
				NULL );
			DEBUGMSG(FSDMGR_ZONE, (TEXT("FileMgr_AttachDevice: CreateFile-%s, handle=%x\r\n"), buf, lpdsk->hFile));
			
			if( lpdsk->hFile != INVALID_HANDLE_VALUE )
			{	// 打开文件成功
				lpdsk->lpfsd = lpfsd;
				// 得到磁盘信息
				FileMgr_GetDiskInfo( (HDSK)lpdsk, &lpdsk->fdi );
				// 安装磁盘卷
				if( lpfsd->lpMountDisk( (HDSK)lpdsk ) )   //-> regist volume
				{
					return (HANDLE)lpdsk;
				}
				else
				{
					WARNMSG( FSDMGR_ZONE, (TEXT("error in FileMgr_AttachDevice(%s): MountDisk(0x%x) failure.\r\n"), buf, lpdsk ));
				}
			}
			else
			{
				WARNMSG( FSDMGR_ZONE, (TEXT("error in FileMgr_AttachDevice: CreateFile(%s) failure.\r\n"), buf ) );
			}
			// 打开文件不成功
			free( lpdsk );
        }
    }
	WARNMSG(1, (TEXT("error in FileMgr_AttachDevice: Attach device failure.\r\n") ));
    return NULL;
}

// **************************************************
// 声明：BOOL WINAPI FileMgr_DetachDevice( HANDLE hDsk )
// 参数：
//		IN hDsk - 磁盘对象句柄	
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		将一个设备与已经注册的文件系统断开联结
// 引用: 
//		系统API
// **************************************************
BOOL WINAPI FileMgr_DetachDevice( HANDLE hDsk )
{
    _LPFSDDSK lpdsk = (_LPFSDDSK)hDsk;

	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DetachDevice:hDsk=%x.\r\n", hDsk ) );
	if( lpdsk )
	{
		_LPFINDDATA lpFindData;

		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DetachDevice:Call UnmountDisk.\r\n" ) );
		// 卸下磁盘卷
		lpdsk->lpfsd->lpUnmountDisk( (DWORD)hDsk );
		// 将系统的与该磁盘关联的搜索句柄释放掉
		// 
		// EnterCS ???
		lpFindData = lpFindObjList;
		// 
		while( lpFindData )
		{
			_LPFINDDATA lpNext;
			_LPVOLUME lpVol = (_LPVOLUME)lpFindData->hVol;

			if( lpVol->lpdsk == hDsk )
			{
				lpNext = (_LPFINDDATA)lpFindData->obj.lpNext;
				// 关闭搜索句柄
				FileMgr_CloseSearchHandle( lpFindData );
				lpFindData = lpNext;
			}
			else
			    lpFindData = (_LPFINDDATA)lpFindData->obj.lpNext;
		}
		// LeaveCS ???		
		CloseHandle( lpdsk->hFile );

		free( lpdsk );
		return TRUE;
	}
	return FALSE;
}
 
// **************************************************
// 声明：BOOL WINAPI FileMgr_RegisterFSD ( LPCTSTR lpcszFSDName, 
//                                         const FSDDRV * lpfsd )
// 参数：
//		IN lpcszFSDName - 文件系统名
//		IN lpfsd - 文件系统接口
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		注册文件系统驱动程序
// 引用: 
//		系统API
// **************************************************
BOOL WINAPI FileMgr_RegisterFSD ( LPCTSTR lpcszFSDName, const FSDDRV * lpfsd )
{
    FSDINITDRV fsdInit = { FileMgr_AttachDevice,    
		                   FileMgr_DetachDevice };
	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterFSD:lpcszFSDName=%s,lpfsd=%x.\r\n", lpcszFSDName, lpfsd ) );
    return Dev_RegisterFSD( lpcszFSDName, (HANDLE)lpfsd, &fsdInit );
}

// **************************************************
// 声明：BOOL WINAPI FileMgr_UnregisterFSD ( LPCTSTR lpcszFSDName )
// 参数：
//		IN lpcszFSDName - 文件系统名
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		与FileMgr_RegisterFSD相反，注销文件系统驱动程序
// 引用: 
//		系统API
// **************************************************
BOOL WINAPI FileMgr_UnregisterFSD ( LPCTSTR lpcszFSDName )
{
    return Dev_UnregisterFSD( lpcszFSDName );
}

// **************************************************
// 声明：HANDLE WINAPI FileMgr_CreateFileHandle( 
//                               HVOL hVol, 
//                               HANDLE hProc,
//                               PFILE pFile )
// 参数：
//		IN hVol - 卷对象
//		IN hProc - 拥有者进程对象
//		IN pFile - 由文件系统驱动程序产生的私有对象
// 返回值：
//		假如成功，返回非 INVALID_HANDLE_VALUE ; 否则，返回 INVALID_HANDLE_VALUE
// 功能描述：
//		创建与文件对象关联的文件句柄
// 引用: 
//		系统API
// **************************************************
HANDLE WINAPI FileMgr_CreateFileHandle( 
                               HVOL hVol, 
                               HANDLE hProc,
                               PFILE pFile )
{
	// 该功能具体实现已没有，已被移入 fsmain.c 's FileSys_CreateFile
	return (HANDLE)pFile;
}

// **************************************************
// 声明：BOOL WINAPI FileMgr_CloseFileHandle( HANDLE hFile )
// 参数：
//		IN hFile - 文件句柄
// 返回值：
//		假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//		关闭文件句柄
// 引用: 
// **************************************************
BOOL WINAPI FileMgr_CloseFileHandle( HANDLE hFile )
{
	// 该功能具体实现已没有
	return TRUE;
}

// **************************************************
// 声明：HANDLE WINAPI  FileMgr_CreateSearchHandle(
//                                  HVOL hVol, 
//                                  HANDLE hProc,
//                                  PFIND pfd )
// 参数：
//		IN hVol - 卷对象
//		IN hProc - 拥有者进程句柄
//		IN pfd - 由文件系统驱动程序创建的私有对象
// 返回值：
//		假如成功，返回非NULL句柄；否则，返回NULL
// 功能描述：
//		创建一个能被本管理器管理的搜索对象
// 引用: 
//		系统API
// **************************************************
HANDLE WINAPI  FileMgr_CreateSearchHandle(
                                  HVOL hVol, 
                                  HANDLE hProc,
                                  PFIND pfd )
{	// 分配 _FINDDATA 结构对象
    _LPFINDDATA lpfd =(_LPFINDDATA) malloc( sizeof( _FINDDATA ) );
    
    if( lpfd )
    {
        lpfd->hVol = hVol;
        lpfd->pFind = pfd; 
        // 加入搜索对象链表
		ObjList_Init( (LPOBJLIST*)&lpFindObjList, &lpfd->obj, OBJ_FINDFILE, (ULONG)hProc );
    }
    return (HANDLE)lpfd;
}

// **************************************************
// 声明：BOOL WINAPI FileMgr_CloseSearchHandle( HANDLE hFind )
// 参数：
//		IN hFind - 搜索句柄对象（由 FileMgr_CreateSearchHandle 创建）
// 返回值：
//		假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		关闭搜索句柄
// 引用: 
//		系统API
// **************************************************
BOOL WINAPI FileMgr_CloseSearchHandle( HANDLE hFind )
{
	_LPFINDDATA lpfd = _GetHFINDPtr( hFind );

	if( lpfd )
	{   // 从搜索对象链表中移出搜索句柄对象
		ObjList_Remove( (LPOBJLIST*)&lpFindObjList, &lpfd->obj );
		// 释放
		free( lpfd );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：int WINAPI FileMgr_GetVolumeName( 
//							HVOL hVol, 
//							LPTSTR lpszName,
//							int cMax )
// 参数：
//		IN hVol - 卷对象
//		IN lpszName - 用于接受卷文件名的缓存
//		IN cMax - 用于接受卷文件名的缓存大小
// 返回值：
//		实际拷贝到 lpszName 的文件名字符串数
// 功能描述：
//		得到卷文件名
// 引用: 
//		系统API
// **************************************************
int WINAPI FileMgr_GetVolumeName( 
    HVOL hVol, 
    LPTSTR lpszName,
    int cMax )
{
    _LPVOLUME lpVol = (_LPVOLUME)hVol;

    DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_GetVolumeName: lpVol->lpszVolName(%s).lpszName=(0x%x).\r\n", lpVol->lpszVolName, lpszName ) );
	strncpy( lpszName, lpVol->lpszVolName, cMax );
    return strlen( lpszName );
}

#define TOUPPER( c ) ( ( (c) >= 'a' && (c) <= 'z' ) ? ((c) - 'a' + 'A') : (c) )
#define TOLOWER( c ) ( ( (c) >= 'A' && (c) <= 'Z' ) ? ((c) - 'A' + 'a') : (c) )

// **************************************************
// 声明：int vol_nicmp( const char *szCmpVol, int szCmpVolLen, const char *szSysVol  )
// 参数：
//		IN szCmpVol - 卷名1
//		IN szCmpVolLen - 卷名1的长度
//		IN szSysVol - 卷名2
// 返回值：
//		假如 szCmpVol > szSysVol, 返回 1；
//		假如 szCmpVol < szSysVol, 返回 -1；
//		假如 szCmpVol == szSysVol，返回 0;
// 功能描述：
//		比较字符串的的大小
// 引用: 
// **************************************************
static int vol_nicmp( const char *szCmpVol, int szCmpVolLen, const char *szSysVol  )
{
	register const unsigned char * p1 = (const unsigned char *)szCmpVol;
	register const unsigned char * p2 = (const unsigned char *)szSysVol;
    register int count = szCmpVolLen;
	register unsigned char s1;
	register unsigned char s2;


	while( count )
	{
		s1 = *p1++;
		s2 = *p2++;

		count--;
		s1 = TOUPPER(s1);
		s2 = TOUPPER(s2);
		if(  s1 > s2 )
			return 1;
		else if( s1 < s2 )
			return -1;
		else if( s1 == 0 && s2 == 0 )
			return 0;
	}
	if( *p2 )
		return -1;

	return 0;
}

// **************************************************
// 声明：_LPVOLUME _FileMgr_FindVolume( 
//						LPCTSTR lpcszVolName, 
//						UINT uNameLen )
// 参数：
//		IN lpcszVolName - 卷名
//		IN uNameLen - 卷名的长度
// 返回值：
//		假如找到，返回卷结构指针；否则，返回 NULL
// 功能描述：
//		通过卷名发现卷对象
// 引用: 
//	该模块和fsmain.c
// **************************************************
_LPVOLUME _FileMgr_FindVolume( 
                      LPCTSTR lpcszVolName, UINT uNameLen )
{
    _LPVOLUME lpVol;

    //RETAILMSG( 1, ( "_FileMgr_FindVolume:%s,len=%d.\r\n", lpcszVolName, uNameLen ) );
    if( *lpcszVolName == '\\' )
    {
        lpcszVolName++;
        uNameLen--;
    }
	if( uNameLen )  
	{
		// EnterCS ???
		lpVol = (_LPVOLUME)lpVolLink;
		while( lpVol )
		{
			if( vol_nicmp( lpcszVolName, uNameLen, lpVol->lpszVolName ) == 0 )
				return lpVol;
			lpVol = lpVol->lpNext;
		}
		// LeaveCS ???
	}
    return NULL;
}

// **************************************************
// 声明：DWORD WINAPI FileMgr_GetDiskInfo ( 
//								HDSK hDsk,
//								PFDD pfdd )
// 参数：
//		IN hDsk - 磁盘对象句柄
//		IN pfdd - 用于保存磁盘信息的 pfdd 结构指针
// 返回值：
//	    假如成功，返回ERROR_SUCCESS	
// 功能描述：
//		得到磁盘信息
// 引用: 
//		系统API
// **************************************************

DWORD WINAPI FileMgr_GetDiskInfo ( 
					HDSK hDsk,
					PFDD pfdd )
{
    _LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;
    DWORD dwRet;

	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_GetDiskInfo: handle=%x.\r\n", lpfsd->hFile ) );
	// 给设备发控制信息
    if( DeviceIoControl( lpfsd->hFile, IOCTL_DISK_GETINFO, pfdd, sizeof( FSD_DISK_DATA ), NULL, 0, &dwRet, NULL ) )
        return ERROR_SUCCESS;  // 成功
    return !ERROR_SUCCESS;	// 失败
}

extern BOOL WINAPI KL_SetFileAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes );
extern BOOL WINAPI KL_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes );
extern BOOL WINAPI KL_RemoveDirectory( LPCTSTR lpcszPathName );

// **************************************************
// 声明：HVOL WINAPI FileMgr_RegisterVolume( 
//									HDSK hDsk,
//									LPCTSTR lpcszName,
//									PVOL pVol )
// 参数：
//		IN hDsk - 磁盘对象名
//		IN lpcszName - 卷名(可以为NULL,则调用DeviceIoCOntrol(...,IOCTL_DISK_GETNAME,...)得到名)
//		IN pVol - 文件系统驱动程序私有卷对象
// 返回值：
//		假如成功，返回非NULL卷句柄；否则，返回NULL
// 功能描述：
//		注册卷
// 引用: 
//		系统API
// **************************************************
HVOL WINAPI FileMgr_RegisterVolume( 
									HDSK hDsk,
									LPCTSTR lpcszName,
									PVOL pVol )
{    
    TCHAR tcVol[MAX_PATH+1];
	TCHAR * lpVolName = tcVol+1;// 将用 tcVol 创建一个文件夹 "\\valname"
								// 因此 tcVol[0] = '\\';
	int len = 0;
	_LPVOLUME lpVol;
	int index;
	BOOL bCreateFolder = FALSE;
	_LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;

    DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume(lpcszName(0x%x)=&s).\r\n", lpcszName, lpcszName ? lpcszName : "" ) );
	
	*lpVolName = 0;
	if( lpcszName == NULL || *lpcszName == 0 )  
	{	//  没有给出有效的卷名
		
		DWORD dwRet = 0;		

		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume:Call DeviceIoControl(IOCTL_DISK_GETNAME).\r\n" ) );
		// 从设备驱动程序得到卷名
		DeviceIoControl( 
						lpfsd->hFile, 
						IOCTL_DISK_GETNAME, 
						NULL, 
						0, 
						lpVolName, 
						MAX_PATH, 
						&dwRet, 
						NULL );

		if( dwRet == 0  ) 
		{   // 设备驱动程序没有返回有效的卷名，用默认的
			strcpy( tcVol, "\\disk0" );
		}
		else
		{	// 
			// strncpy( lpVolName, (TCHAR*)dwRet, MAX_PATH ); // 2004-02-11			
		    lpVolName[MAX_PATH]=0; // 安全起见
		}
	}
	else
	{
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume(%s).\r\n", lpcszName ) );
		strncpy( lpVolName, lpcszName, MAX_PATH );
		lpVolName[MAX_PATH]=0;// 安全起见
	}
    len = strlen( lpVolName );
	index = 0;
	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: get volume name=%s.\r\n", lpVolName ) );
	if( len < MAX_PATH - 2 )
	{   // 检查该名字是否合法/是否已经存在，get a default name
	    BOOL bFindOldFolder;
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: search existed volume.\r\n" ) );
		tcVol[0] = '\\';
		bFindOldFolder = FALSE;
		while( 1 )
		{	
			if( bFindOldFolder || _FileMgr_FindVolume( lpVolName, len ) )
			{  // 系统已经存在该卷名。在卷名后添加 '0' ~ '9'序号看行不行 add index and to find
				for( ; index < 10; index++ )
				{
					lpVolName[len] = '0' + index;
					lpVolName[len+1] = 0;
					if( _FileMgr_FindVolume( lpVolName, len+1 ) == FALSE )
						break; // 没有该名
				}
				if( index == 10 )
					return NULL; // 所有的卷名+序号都以检查完，失败
			}
			// 该卷名不存在，新增一个 get a volname
			if( lpVolLink )
			{   // 创建一个根文件夹，extend disk, to create a disk folder at the root
				DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: CreateDirectory=%s.\r\n", tcVol ) );
				if( ( bCreateFolder = KL_CreateDirectory( tcVol, NULL ) ) )
				{
					break; // 创建成功
				}
				bFindOldFolder = TRUE;
			}
			else
				break;
		}
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: can't create a valid folder.\r\n" ) );
	}
	else
	{
		return NULL;
	}

	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: Create folder(%s) success.\r\n", tcVol ) );
	
	// 分配新卷对象
	len = strlen( lpVolName );
	lpVol = malloc( sizeof( _VOLUME ) + len + 1 );

	if( lpVol )
	{
		lpVol->lpszVolName = (LPTSTR)(lpVol + 1);
		//2005-02-24
		if( lpVolLink )
		{   // 设置卷文件夹为设备属性 extend disk, set the folder with device attrib
			DWORD attr = FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_DIRECTORY;
			//2005-02-24, add code by lilin
			if( lpfsd->fdi.dwFlags & DISK_INFO_FLAG_HIDDEN )
			{
				attr |= FILE_ATTRIBUTE_HIDDEN;
			}
			KL_SetFileAttributes( tcVol, attr );
		}		
		strcpy( lpVol->lpszVolName, lpVolName );
		lpVol->lpdsk = (_LPFSDDSK)hDsk;
		lpVol->pVol = pVol;
		lpVol->lpNext = lpVolLink;
		lpVolLink = lpVol;
		// 想系统通知改变信息
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume ShellNotify.\r\n" ) );
		SHChangeNotify( SHCNE_DRIVEADD, 0, NULL, NULL );		
		return (HVOL)lpVol;
	}
	else
	{	// 初始化卷对象失败,删除之前创建的文件夹
		if( lpVolLink && bCreateFolder )
		{   // remove the folder
			KL_RemoveDirectory( tcVol );
		}
	}
	WARNMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: RegisterValume(%s.) failure.\r\n", tcVol ) );
    return NULL;
}

// **************************************************
// 声明：void WINAPI FileMgr_DeregisterVolume( HVOL hVol )
// 参数：
//		IN hVol - 卷对象句柄
// 返回值：
//		无
// 功能描述：
//		与FileMgr_RegisterVolume相反，注销一个卷
// 引用: 
//		系统API
// **************************************************

void WINAPI FileMgr_DeregisterVolume( HVOL hVol )
{
    _LPVOLUME lpVol = (_LPVOLUME)hVol;

	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DeregisterVolume:hVol=%x.\r\n", hVol ) );

    if( lpVol )
    {
        TCHAR buf[MAX_PATH];
		
		buf[0] = '\\';
		strcpy( buf+1, lpVol->lpszVolName );

		// EnterCS ????
		// 偏历卷链表
        if( lpVol == lpVolLink )
        {
            lpVolLink = lpVol->lpNext;
            free( lpVol );
        }
        else
        {
            _LPVOLUME lp = lpVolLink;
            while( lp )
            {
                if( lp->lpNext == lpVol )
                {   // 发现
                    lp->lpNext = lpVol->lpNext;
                    free( lpVol );
                    break;
                }
                lp = lp->lpNext;
            }
        }
        DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DeregisterVolume:Call KL_RemoveDirectory=%s.\r\n", buf ) );
		// 移出在根目录的文件夹名
		KL_RemoveDirectory( buf );
		// 想系统发送通知消息
		SHChangeNotify( SHCNE_DRIVEREMOVED, 0, NULL, NULL );
    }
}

// **************************************************
// 声明：DWORD WINAPI FileMgr_ReadDisk ( 
//                       HDSK hDsk,
//                       DWORD dwStartSector,
//                       DWORD nSectorNum,
//                       LPVOID lpBuffer,
//                       DWORD nSizeOfBuffer )
// 参数：
//		IN hDsk - 磁盘对象句柄
//      IN dwStartSector - 开始扇区
//      IN nSectorNum - 扇区数
//      OUT lpBuffer - 用于接受数据的缓存
//      IN nSizeOfBuffer - 用于接受数据的缓存大小
// 返回值：
//		假如成功，返回 ERROR_SUCCESS; 否则，返回错误代码
// 功能描述：
//		从磁盘的扇区读取数据
// 引用: 
//		系统API
// **************************************************
DWORD WINAPI FileMgr_ReadDisk ( 
                       HDSK hDsk,
                       DWORD dwStartSector,
                       DWORD nSectorNum,
                       LPVOID lpBuffer,
                       DWORD nSizeOfBuffer )
{
    _LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;
    DISK_RW drw;
    DWORD dwRet;
	DWORD dwReadSize = nSectorNum * lpfsd->fdi.nBytesPerSector;

	if( dwReadSize <= nSizeOfBuffer )
	{
		// 准备块读结构
		drw.dwSectorNumber = nSectorNum;
		drw.dwStartSector = dwStartSector;
		drw.dwStatus = 0;
		drw.nrwNum = 1;
		drw.rwBufs[0].dwSize = dwReadSize;
		drw.rwBufs[0].lpBuf = lpBuffer;
		//ASSERT( ( (DWORD)drw.rwBufs[0].lpBuf & 0x3) == 0 );

		
		if( DeviceIoControl( lpfsd->hFile, IOCTL_DISK_READ, &drw, sizeof( drw ), NULL, 0, &dwRet, NULL ) )
			return ERROR_SUCCESS;  // 成功
		return GetLastError();
	}
	else
	{
		ERRORMSG(FSDMGR_ZONE, (TEXT("error in FileMgr_ReadDisk: dwReadSize(%d) > nSizeOfBuffer(%d).\r\n"), dwReadSize, nSizeOfBuffer ));
		return ERROR_INVALID_PARAMETER;
	}
}

// **************************************************
// 声明：DWORD WINAPI FileMgr_WriteDisk ( 
//                       HDSK hDsk,
//                       DWORD dwStartSector,
//                       DWORD nSectorNum,
//                       LPVOID lpBuffer,
//                       DWORD nSizeOfBuffer )
// 参数：
//		IN hDsk - 磁盘对象句柄
//      IN dwStartSector - 开始扇区
//      IN nSectorNum - 扇区数
//      IN lpBuffer - 用于接受数据的缓存
//      IN nSizeOfBuffer - 用于接受数据的缓存大小
// 返回值：
//		假如成功，返回 ERROR_SUCCESS; 否则，返回错误代码
// 功能描述：
//		向磁盘的扇区写入数据
// 引用: 
//		系统API
// **************************************************
DWORD WINAPI FileMgr_WriteDisk ( 
                        HDSK hDsk,
                        DWORD dwStartSector,
                        DWORD nSectorNum,
                        LPCVOID lpBuffer,
                        DWORD nSizeOfBuffer )
{
    _LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;
    DISK_RW drw;
    DWORD dwRet;
	DWORD dwWriteSize = nSectorNum * lpfsd->fdi.nBytesPerSector;

	DEBUGMSG(FSDMGR_ZONE, (TEXT("FileMgr_WriteDisk: hDsk=%x, dwStartSector=%d, nSectorNum=%d, lpBuffer=%x, nSizeOfBuffer=%d\r\n"), hDsk, dwStartSector, nSectorNum, lpBuffer, nSizeOfBuffer));
	if( dwWriteSize <= nSizeOfBuffer )
	{		
		// 准备块写结构
		drw.dwSectorNumber = nSectorNum;
		drw.dwStartSector = dwStartSector;
		drw.dwStatus = 0;
		drw.nrwNum = 1;
		drw.rwBufs[0].dwSize = dwWriteSize;
		drw.rwBufs[0].lpBuf = (LPBYTE)lpBuffer;
		//ASSERT( ( (DWORD)drw.rwBufs[0].lpBuf & 0x3) == 0 );
		
		if( DeviceIoControl( lpfsd->hFile, IOCTL_DISK_WRITE, &drw, sizeof( drw ), NULL, 0, &dwRet, NULL ) )
			return ERROR_SUCCESS;  // 成功
		return GetLastError();
	}
	else
	{
		ERRORMSG(FSDMGR_ZONE, (TEXT("error in FileMgr_WriteDisk: dwWriteSize(%d) > nSizeOfBuffer(%d).\r\n"), dwWriteSize, nSizeOfBuffer ));
		return ERROR_INVALID_PARAMETER;
	}
    
}

// **************************************************
// 声明：BOOL WINAPI FileMgr_DeviceIoControl ( 
//                           HDSK hDsk,
//                           DWORD dwIoControlCode,
//                           LPVOID lpInBuf,
//                           DWORD nInBufSize,
//                           LPVOID lpOutBuf,
//                           DWORD nOutBufSize,
//                           LPDWORD lpdwBytesReturned,
//                           LPOVERLAPPED lpOverlapped )
// 参数：
//		IN hDsk-磁盘文件对象句柄
//		IN dwIoControlCode-I/O控制命令，文件系统包含：
//			IOCTL_DISK_SCAN_VOLUME－通知文件系统去扫描卷的错误
//			IOCTL_DISK_FORMAT_VOLUME－通知文件系统去格式化卷
//	    IN lpInBuf-输入缓存
//	    IN nInBufSize－输入缓存的尺寸
//	    OUT lpOutBuf－输出缓存
//	    IN nOutBufSize－输出缓存的尺寸
//	    IN lpBytesReturned-返回的数据的尺寸，该数据在返回时已存入lpOutBuf
//	    IN lpOverlapped-支持覆盖(不支持，为NULL)
		
// 返回值：
//		成功：返回TRUE
//		否则：返回FALSE
// 功能描述：
//		对设备进行控制
// 引用: 
//		系统API
// **************************************************
BOOL WINAPI FileMgr_DeviceIoControl ( 
                          HDSK hDsk,
                          DWORD dwIoControlCode,
                          LPVOID lpInBuf,
                          DWORD nInBufSize,
                          LPVOID lpOutBuf,
                          DWORD nOutBufSize,
                          LPDWORD lpdwBytesReturned,
                          LPOVERLAPPED lpOverlapped )
{
	 _LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;
     return DeviceIoControl( lpfsd->hFile, 
		                     dwIoControlCode,
							 lpInBuf,
							 nInBufSize,
							 lpOutBuf,
							 nOutBufSize,
							 lpdwBytesReturned, 
							 lpOverlapped );
}


