/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����ļ�ϵͳ������
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2005-02-24, FileMgr_RegisterVolume ���� �豸�ļ��е�������־
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
// �����ļ���������
static _LPFINDDATA lpFindObjList = NULL;
// ���������
static _LPVOLUME lpVolLink=NULL;

/*
// **************************************************
// ������
// ������
//		
// ����ֵ��
//		
// ����������
//		
// ����: 
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
// ������HANDLE WINAPI FileMgr_AttachDevice( HANDLE hFSD, LPCTSTR lpszDeviceName )
// ������
//		IN hFSD - �ļ�ϵͳ������������������ļ�ϵͳ�������� ����FileMgr_RegisterFSD/Dev_RegisterFSD�����ݵĲ�����
//		IN lpszDeviceName - ��Ҫ���ļ�ϵͳ�������豸�����磺DSK2��RAM1��
// ����ֵ��
//		����ɹ������ط�NULL�ľ�������򣬷���NULL
// ����������
//		��һ���豸���Ѿ�ע����ļ�ϵͳ����
// ����: 
//		ϵͳAPI
// **************************************************
HANDLE WINAPI FileMgr_AttachDevice( HANDLE hFSD, LPCTSTR lpszDeviceName )
{
    LPFSDDRV lpfsd = (LPFSDDRV)hFSD;
    TCHAR buf[6];
	
    if( lpfsd )
    {   // ����_FSDDSK�ṹ����     
        _LPFSDDSK lpdsk = (_LPFSDDSK)malloc( sizeof( _FSDDSK ) );
        if( lpdsk )
        {   // ���豸����ϳ� "XXX1:" ����ʽ
			strncpy( buf, lpszDeviceName, 4 );
			buf[4] = ':';
			buf[5] = 0;	  
			// �򿪸��豸�ļ�
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
			{	// ���ļ��ɹ�
				lpdsk->lpfsd = lpfsd;
				// �õ�������Ϣ
				FileMgr_GetDiskInfo( (HDSK)lpdsk, &lpdsk->fdi );
				// ��װ���̾�
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
			// ���ļ����ɹ�
			free( lpdsk );
        }
    }
	WARNMSG(1, (TEXT("error in FileMgr_AttachDevice: Attach device failure.\r\n") ));
    return NULL;
}

// **************************************************
// ������BOOL WINAPI FileMgr_DetachDevice( HANDLE hDsk )
// ������
//		IN hDsk - ���̶�����	
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��һ���豸���Ѿ�ע����ļ�ϵͳ�Ͽ�����
// ����: 
//		ϵͳAPI
// **************************************************
BOOL WINAPI FileMgr_DetachDevice( HANDLE hDsk )
{
    _LPFSDDSK lpdsk = (_LPFSDDSK)hDsk;

	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DetachDevice:hDsk=%x.\r\n", hDsk ) );
	if( lpdsk )
	{
		_LPFINDDATA lpFindData;

		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DetachDevice:Call UnmountDisk.\r\n" ) );
		// ж�´��̾�
		lpdsk->lpfsd->lpUnmountDisk( (DWORD)hDsk );
		// ��ϵͳ����ô��̹�������������ͷŵ�
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
				// �ر��������
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
// ������BOOL WINAPI FileMgr_RegisterFSD ( LPCTSTR lpcszFSDName, 
//                                         const FSDDRV * lpfsd )
// ������
//		IN lpcszFSDName - �ļ�ϵͳ��
//		IN lpfsd - �ļ�ϵͳ�ӿ�
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		ע���ļ�ϵͳ��������
// ����: 
//		ϵͳAPI
// **************************************************
BOOL WINAPI FileMgr_RegisterFSD ( LPCTSTR lpcszFSDName, const FSDDRV * lpfsd )
{
    FSDINITDRV fsdInit = { FileMgr_AttachDevice,    
		                   FileMgr_DetachDevice };
	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterFSD:lpcszFSDName=%s,lpfsd=%x.\r\n", lpcszFSDName, lpfsd ) );
    return Dev_RegisterFSD( lpcszFSDName, (HANDLE)lpfsd, &fsdInit );
}

// **************************************************
// ������BOOL WINAPI FileMgr_UnregisterFSD ( LPCTSTR lpcszFSDName )
// ������
//		IN lpcszFSDName - �ļ�ϵͳ��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		��FileMgr_RegisterFSD�෴��ע���ļ�ϵͳ��������
// ����: 
//		ϵͳAPI
// **************************************************
BOOL WINAPI FileMgr_UnregisterFSD ( LPCTSTR lpcszFSDName )
{
    return Dev_UnregisterFSD( lpcszFSDName );
}

// **************************************************
// ������HANDLE WINAPI FileMgr_CreateFileHandle( 
//                               HVOL hVol, 
//                               HANDLE hProc,
//                               PFILE pFile )
// ������
//		IN hVol - �����
//		IN hProc - ӵ���߽��̶���
//		IN pFile - ���ļ�ϵͳ�������������˽�ж���
// ����ֵ��
//		����ɹ������ط� INVALID_HANDLE_VALUE ; ���򣬷��� INVALID_HANDLE_VALUE
// ����������
//		�������ļ�����������ļ����
// ����: 
//		ϵͳAPI
// **************************************************
HANDLE WINAPI FileMgr_CreateFileHandle( 
                               HVOL hVol, 
                               HANDLE hProc,
                               PFILE pFile )
{
	// �ù��ܾ���ʵ����û�У��ѱ����� fsmain.c 's FileSys_CreateFile
	return (HANDLE)pFile;
}

// **************************************************
// ������BOOL WINAPI FileMgr_CloseFileHandle( HANDLE hFile )
// ������
//		IN hFile - �ļ����
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�ر��ļ����
// ����: 
// **************************************************
BOOL WINAPI FileMgr_CloseFileHandle( HANDLE hFile )
{
	// �ù��ܾ���ʵ����û��
	return TRUE;
}

// **************************************************
// ������HANDLE WINAPI  FileMgr_CreateSearchHandle(
//                                  HVOL hVol, 
//                                  HANDLE hProc,
//                                  PFIND pfd )
// ������
//		IN hVol - �����
//		IN hProc - ӵ���߽��̾��
//		IN pfd - ���ļ�ϵͳ�������򴴽���˽�ж���
// ����ֵ��
//		����ɹ������ط�NULL��������򣬷���NULL
// ����������
//		����һ���ܱ����������������������
// ����: 
//		ϵͳAPI
// **************************************************
HANDLE WINAPI  FileMgr_CreateSearchHandle(
                                  HVOL hVol, 
                                  HANDLE hProc,
                                  PFIND pfd )
{	// ���� _FINDDATA �ṹ����
    _LPFINDDATA lpfd =(_LPFINDDATA) malloc( sizeof( _FINDDATA ) );
    
    if( lpfd )
    {
        lpfd->hVol = hVol;
        lpfd->pFind = pfd; 
        // ����������������
		ObjList_Init( (LPOBJLIST*)&lpFindObjList, &lpfd->obj, OBJ_FINDFILE, (ULONG)hProc );
    }
    return (HANDLE)lpfd;
}

// **************************************************
// ������BOOL WINAPI FileMgr_CloseSearchHandle( HANDLE hFind )
// ������
//		IN hFind - ������������� FileMgr_CreateSearchHandle ������
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ر��������
// ����: 
//		ϵͳAPI
// **************************************************
BOOL WINAPI FileMgr_CloseSearchHandle( HANDLE hFind )
{
	_LPFINDDATA lpfd = _GetHFINDPtr( hFind );

	if( lpfd )
	{   // �����������������Ƴ������������
		ObjList_Remove( (LPOBJLIST*)&lpFindObjList, &lpfd->obj );
		// �ͷ�
		free( lpfd );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������int WINAPI FileMgr_GetVolumeName( 
//							HVOL hVol, 
//							LPTSTR lpszName,
//							int cMax )
// ������
//		IN hVol - �����
//		IN lpszName - ���ڽ��ܾ��ļ����Ļ���
//		IN cMax - ���ڽ��ܾ��ļ����Ļ����С
// ����ֵ��
//		ʵ�ʿ����� lpszName ���ļ����ַ�����
// ����������
//		�õ����ļ���
// ����: 
//		ϵͳAPI
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
// ������int vol_nicmp( const char *szCmpVol, int szCmpVolLen, const char *szSysVol  )
// ������
//		IN szCmpVol - ����1
//		IN szCmpVolLen - ����1�ĳ���
//		IN szSysVol - ����2
// ����ֵ��
//		���� szCmpVol > szSysVol, ���� 1��
//		���� szCmpVol < szSysVol, ���� -1��
//		���� szCmpVol == szSysVol������ 0;
// ����������
//		�Ƚ��ַ����ĵĴ�С
// ����: 
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
// ������_LPVOLUME _FileMgr_FindVolume( 
//						LPCTSTR lpcszVolName, 
//						UINT uNameLen )
// ������
//		IN lpcszVolName - ����
//		IN uNameLen - �����ĳ���
// ����ֵ��
//		�����ҵ������ؾ�ṹָ�룻���򣬷��� NULL
// ����������
//		ͨ���������־����
// ����: 
//	��ģ���fsmain.c
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
// ������DWORD WINAPI FileMgr_GetDiskInfo ( 
//								HDSK hDsk,
//								PFDD pfdd )
// ������
//		IN hDsk - ���̶�����
//		IN pfdd - ���ڱ��������Ϣ�� pfdd �ṹָ��
// ����ֵ��
//	    ����ɹ�������ERROR_SUCCESS	
// ����������
//		�õ�������Ϣ
// ����: 
//		ϵͳAPI
// **************************************************

DWORD WINAPI FileMgr_GetDiskInfo ( 
					HDSK hDsk,
					PFDD pfdd )
{
    _LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;
    DWORD dwRet;

	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_GetDiskInfo: handle=%x.\r\n", lpfsd->hFile ) );
	// ���豸��������Ϣ
    if( DeviceIoControl( lpfsd->hFile, IOCTL_DISK_GETINFO, pfdd, sizeof( FSD_DISK_DATA ), NULL, 0, &dwRet, NULL ) )
        return ERROR_SUCCESS;  // �ɹ�
    return !ERROR_SUCCESS;	// ʧ��
}

extern BOOL WINAPI KL_SetFileAttributes( LPCTSTR lpcszFileName, DWORD dwFileAttributes );
extern BOOL WINAPI KL_CreateDirectory( LPCTSTR lpcszPathName, PSECURITY_ATTRIBUTES pSecurityAttributes );
extern BOOL WINAPI KL_RemoveDirectory( LPCTSTR lpcszPathName );

// **************************************************
// ������HVOL WINAPI FileMgr_RegisterVolume( 
//									HDSK hDsk,
//									LPCTSTR lpcszName,
//									PVOL pVol )
// ������
//		IN hDsk - ���̶�����
//		IN lpcszName - ����(����ΪNULL,�����DeviceIoCOntrol(...,IOCTL_DISK_GETNAME,...)�õ���)
//		IN pVol - �ļ�ϵͳ��������˽�о����
// ����ֵ��
//		����ɹ������ط�NULL���������򣬷���NULL
// ����������
//		ע���
// ����: 
//		ϵͳAPI
// **************************************************
HVOL WINAPI FileMgr_RegisterVolume( 
									HDSK hDsk,
									LPCTSTR lpcszName,
									PVOL pVol )
{    
    TCHAR tcVol[MAX_PATH+1];
	TCHAR * lpVolName = tcVol+1;// ���� tcVol ����һ���ļ��� "\\valname"
								// ��� tcVol[0] = '\\';
	int len = 0;
	_LPVOLUME lpVol;
	int index;
	BOOL bCreateFolder = FALSE;
	_LPFSDDSK lpfsd = (_LPFSDDSK)hDsk;

    DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume(lpcszName(0x%x)=&s).\r\n", lpcszName, lpcszName ? lpcszName : "" ) );
	
	*lpVolName = 0;
	if( lpcszName == NULL || *lpcszName == 0 )  
	{	//  û�и�����Ч�ľ���
		
		DWORD dwRet = 0;		

		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume:Call DeviceIoControl(IOCTL_DISK_GETNAME).\r\n" ) );
		// ���豸��������õ�����
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
		{   // �豸��������û�з�����Ч�ľ�������Ĭ�ϵ�
			strcpy( tcVol, "\\disk0" );
		}
		else
		{	// 
			// strncpy( lpVolName, (TCHAR*)dwRet, MAX_PATH ); // 2004-02-11			
		    lpVolName[MAX_PATH]=0; // ��ȫ���
		}
	}
	else
	{
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume(%s).\r\n", lpcszName ) );
		strncpy( lpVolName, lpcszName, MAX_PATH );
		lpVolName[MAX_PATH]=0;// ��ȫ���
	}
    len = strlen( lpVolName );
	index = 0;
	DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: get volume name=%s.\r\n", lpVolName ) );
	if( len < MAX_PATH - 2 )
	{   // ���������Ƿ�Ϸ�/�Ƿ��Ѿ����ڣ�get a default name
	    BOOL bFindOldFolder;
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: search existed volume.\r\n" ) );
		tcVol[0] = '\\';
		bFindOldFolder = FALSE;
		while( 1 )
		{	
			if( bFindOldFolder || _FileMgr_FindVolume( lpVolName, len ) )
			{  // ϵͳ�Ѿ����ڸþ������ھ�������� '0' ~ '9'��ſ��в��� add index and to find
				for( ; index < 10; index++ )
				{
					lpVolName[len] = '0' + index;
					lpVolName[len+1] = 0;
					if( _FileMgr_FindVolume( lpVolName, len+1 ) == FALSE )
						break; // û�и���
				}
				if( index == 10 )
					return NULL; // ���еľ���+��Ŷ��Լ���꣬ʧ��
			}
			// �þ��������ڣ�����һ�� get a volname
			if( lpVolLink )
			{   // ����һ�����ļ��У�extend disk, to create a disk folder at the root
				DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: CreateDirectory=%s.\r\n", tcVol ) );
				if( ( bCreateFolder = KL_CreateDirectory( tcVol, NULL ) ) )
				{
					break; // �����ɹ�
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
	
	// �����¾����
	len = strlen( lpVolName );
	lpVol = malloc( sizeof( _VOLUME ) + len + 1 );

	if( lpVol )
	{
		lpVol->lpszVolName = (LPTSTR)(lpVol + 1);
		//2005-02-24
		if( lpVolLink )
		{   // ���þ��ļ���Ϊ�豸���� extend disk, set the folder with device attrib
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
		// ��ϵͳ֪ͨ�ı���Ϣ
		DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume ShellNotify.\r\n" ) );
		SHChangeNotify( SHCNE_DRIVEADD, 0, NULL, NULL );		
		return (HVOL)lpVol;
	}
	else
	{	// ��ʼ�������ʧ��,ɾ��֮ǰ�������ļ���
		if( lpVolLink && bCreateFolder )
		{   // remove the folder
			KL_RemoveDirectory( tcVol );
		}
	}
	WARNMSG( FSDMGR_ZONE, ( "FileMgr_RegisterVolume: RegisterValume(%s.) failure.\r\n", tcVol ) );
    return NULL;
}

// **************************************************
// ������void WINAPI FileMgr_DeregisterVolume( HVOL hVol )
// ������
//		IN hVol - �������
// ����ֵ��
//		��
// ����������
//		��FileMgr_RegisterVolume�෴��ע��һ����
// ����: 
//		ϵͳAPI
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
		// ƫ��������
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
                {   // ����
                    lp->lpNext = lpVol->lpNext;
                    free( lpVol );
                    break;
                }
                lp = lp->lpNext;
            }
        }
        DEBUGMSG( FSDMGR_ZONE, ( "FileMgr_DeregisterVolume:Call KL_RemoveDirectory=%s.\r\n", buf ) );
		// �Ƴ��ڸ�Ŀ¼���ļ�����
		KL_RemoveDirectory( buf );
		// ��ϵͳ����֪ͨ��Ϣ
		SHChangeNotify( SHCNE_DRIVEREMOVED, 0, NULL, NULL );
    }
}

// **************************************************
// ������DWORD WINAPI FileMgr_ReadDisk ( 
//                       HDSK hDsk,
//                       DWORD dwStartSector,
//                       DWORD nSectorNum,
//                       LPVOID lpBuffer,
//                       DWORD nSizeOfBuffer )
// ������
//		IN hDsk - ���̶�����
//      IN dwStartSector - ��ʼ����
//      IN nSectorNum - ������
//      OUT lpBuffer - ���ڽ������ݵĻ���
//      IN nSizeOfBuffer - ���ڽ������ݵĻ����С
// ����ֵ��
//		����ɹ������� ERROR_SUCCESS; ���򣬷��ش������
// ����������
//		�Ӵ��̵�������ȡ����
// ����: 
//		ϵͳAPI
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
		// ׼������ṹ
		drw.dwSectorNumber = nSectorNum;
		drw.dwStartSector = dwStartSector;
		drw.dwStatus = 0;
		drw.nrwNum = 1;
		drw.rwBufs[0].dwSize = dwReadSize;
		drw.rwBufs[0].lpBuf = lpBuffer;
		//ASSERT( ( (DWORD)drw.rwBufs[0].lpBuf & 0x3) == 0 );

		
		if( DeviceIoControl( lpfsd->hFile, IOCTL_DISK_READ, &drw, sizeof( drw ), NULL, 0, &dwRet, NULL ) )
			return ERROR_SUCCESS;  // �ɹ�
		return GetLastError();
	}
	else
	{
		ERRORMSG(FSDMGR_ZONE, (TEXT("error in FileMgr_ReadDisk: dwReadSize(%d) > nSizeOfBuffer(%d).\r\n"), dwReadSize, nSizeOfBuffer ));
		return ERROR_INVALID_PARAMETER;
	}
}

// **************************************************
// ������DWORD WINAPI FileMgr_WriteDisk ( 
//                       HDSK hDsk,
//                       DWORD dwStartSector,
//                       DWORD nSectorNum,
//                       LPVOID lpBuffer,
//                       DWORD nSizeOfBuffer )
// ������
//		IN hDsk - ���̶�����
//      IN dwStartSector - ��ʼ����
//      IN nSectorNum - ������
//      IN lpBuffer - ���ڽ������ݵĻ���
//      IN nSizeOfBuffer - ���ڽ������ݵĻ����С
// ����ֵ��
//		����ɹ������� ERROR_SUCCESS; ���򣬷��ش������
// ����������
//		����̵�����д������
// ����: 
//		ϵͳAPI
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
		// ׼����д�ṹ
		drw.dwSectorNumber = nSectorNum;
		drw.dwStartSector = dwStartSector;
		drw.dwStatus = 0;
		drw.nrwNum = 1;
		drw.rwBufs[0].dwSize = dwWriteSize;
		drw.rwBufs[0].lpBuf = (LPBYTE)lpBuffer;
		//ASSERT( ( (DWORD)drw.rwBufs[0].lpBuf & 0x3) == 0 );
		
		if( DeviceIoControl( lpfsd->hFile, IOCTL_DISK_WRITE, &drw, sizeof( drw ), NULL, 0, &dwRet, NULL ) )
			return ERROR_SUCCESS;  // �ɹ�
		return GetLastError();
	}
	else
	{
		ERRORMSG(FSDMGR_ZONE, (TEXT("error in FileMgr_WriteDisk: dwWriteSize(%d) > nSizeOfBuffer(%d).\r\n"), dwWriteSize, nSizeOfBuffer ));
		return ERROR_INVALID_PARAMETER;
	}
    
}

// **************************************************
// ������BOOL WINAPI FileMgr_DeviceIoControl ( 
//                           HDSK hDsk,
//                           DWORD dwIoControlCode,
//                           LPVOID lpInBuf,
//                           DWORD nInBufSize,
//                           LPVOID lpOutBuf,
//                           DWORD nOutBufSize,
//                           LPDWORD lpdwBytesReturned,
//                           LPOVERLAPPED lpOverlapped )
// ������
//		IN hDsk-�����ļ�������
//		IN dwIoControlCode-I/O��������ļ�ϵͳ������
//			IOCTL_DISK_SCAN_VOLUME��֪ͨ�ļ�ϵͳȥɨ���Ĵ���
//			IOCTL_DISK_FORMAT_VOLUME��֪ͨ�ļ�ϵͳȥ��ʽ����
//	    IN lpInBuf-���뻺��
//	    IN nInBufSize�����뻺��ĳߴ�
//	    OUT lpOutBuf���������
//	    IN nOutBufSize���������ĳߴ�
//	    IN lpBytesReturned-���ص����ݵĳߴ磬�������ڷ���ʱ�Ѵ���lpOutBuf
//	    IN lpOverlapped-֧�ָ���(��֧�֣�ΪNULL)
		
// ����ֵ��
//		�ɹ�������TRUE
//		���򣺷���FALSE
// ����������
//		���豸���п���
// ����: 
//		ϵͳAPI
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


