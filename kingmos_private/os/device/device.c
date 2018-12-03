/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�����豸����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

******************************************************/

#include <eframe.h>
#include <edevice.h>
#include <efile.h>
#include <eapisrv.h>
#include <eobjlist.h>
#include <epdev.h>
#include <devsrv.h>

// struct define 

// �ļ��豸�ṹ
typedef struct _DEVFSD
{
    struct _DEVFSD * lpNext;

	LPFSDINITDRV lpFSDInitDrv;
	HANDLE hFSDRegister;
	LPTSTR lpszFSDName;	
	int iRefCount;
}DEVFSD, FAR * LPDEVFSD;

// �豸ע��ṹ
typedef struct _DEVICE_DATA
{
	OBJLIST obj;

    LPTSTR lpszName;
    BYTE index;
    BYTE bFree;
	WORD wReserve;
    const DEVICE_DRIVER * lpDriver;
    DWORD hDevInit;   // init handle
	// file sys
	LPDEVFSD lpfsd;
	HANDLE hFSDAttach;
}DEVICE_DATA, FAR * LPDEVICE_DATA;

// �豸�򿪽ṹ
typedef struct _DEVICE_OPEN_DATA
{
	OBJLIST obj;

	LPDEVICE_DATA lpDev;
	DWORD  hDevOpen;
    HANDLE hProcess;
}DEVICE_OPEN_DATA, FAR * LPDEVICE_OPEN_FILE;

// ȫ������
static LPDEVICE_DATA lpDeviceObjList = NULL;  //��ע���豸��ʽ
static LPDEVICE_OPEN_FILE lpDeviceOpenObjList = NULL; //�Ѵ��豸��ʽ
static CRITICAL_SECTION csDeviceObjList; //�ٽ��

static const TCHAR szDll[]= "Dll";
static const TCHAR szIndex[] = "Index";
static const TCHAR szPrefix[] = "Prefix";
static const TCHAR szFlags[] = "Flags";
static const TCHAR szActiveKey[] = "Drivers\\Active";
static const TCHAR szBuiltInKey[] = "Drivers\\BuiltIn";
static const TCHAR szHandle[] = "handle";
// ��������
static LPOBJLIST FindDevice( LPCTSTR lpszName, UINT index );
//

extern BOOL SaveAsRegister(LPTSTR lpRegisterFile);

// *****************************************************************
//������static LPDEVICE_DATA _GetHDEVPtr( HANDLE handle )
//������
//	IN handle-�豸���

//����ֵ��
//	�ɹ��������豸����ָ��;ʧ�ܣ�����NULL
//����������Ҫ�����豸���ݣ�������øú����Եõ��豸����
//����: Ҫ�����豸���ݵĴ���
// *****************************************************************

static LPDEVICE_DATA _GetHDEVPtr( HANDLE handle )
{
	
    DEVICE_DATA * lp = handle;

	if( lp && lp->obj.objType == OBJ_DEV )
		return lp;
	ASSERT_NOTIFY( 0, "Invalid device handle\r\n" );
	return NULL;
}

// *****************************************************************
//������static LPDEVICE_OPEN_FILE _GetHDEVFilePtr( HANDLE hFile )
//������
//	IN hFile-�豸�򿪾��

//����ֵ��
//	�ɹ��������豸������ָ��;ʧ�ܣ�����NULL
//����������
//	Ҫ�����豸�����ݣ�������øú����Եõ��豸������
//����: 
//	Ҫ�����豸�����ݵĴ���
// *****************************************************************

static LPDEVICE_OPEN_FILE _GetHDEVFilePtr( HANDLE hFile )
{
	LPDEVICE_OPEN_FILE lpFile = (HANDLE)hFile;
	if( lpFile && lpFile->lpDev && lpFile->obj.objType == OBJ_DEVOPEN )
	{
		return lpFile;
	}
	return NULL;
}


// *****************************************************************
//������HANDLE DoRegisterDriver( LPCTSTR lpDevName,
//                                     int index, 
//                                     const DEVICE_DRIVER FAR * lpDriver,
//                                     LPVOID lpParam )
//������
//	IN lpDevName-�豸������
//	IN index-�豸������
//  IN lpDriver-�豸���ý���ָ��
//  IN lpParam-���ø��豸��Init����ʱ�����ݸò���
//����ֵ��
//	�ɹ��������豸���;ʧ�ܣ�����NULL
//����������
//	�����豸��������ĳ�ʼ������������ɹ��������豸�ӿڼ����豸����
//����: 

// *****************************************************************

#define DEBUG_DOREGISTERDRIVER 0
static HANDLE DoRegisterDriver( LPCTSTR lpDevName, UINT index, const DEVICE_DRIVER FAR * lpDriver, LPVOID lpParam, BOOL bFreeStruct )
{
    int s;
    DEVICE_DATA * lpdd;
    if( index < 0 || index > 9 )
        return NULL;

	DEBUGMSG( DEBUG_DOREGISTERDRIVER, ( "Device_Register:%s,lpDriver=%x.\r\n", lpDevName, lpDriver ) );

    s = strlen( lpDevName );
    lpdd = malloc( sizeof( DEVICE_DATA ) + s + 2 );
    if( lpdd )
    {
        memset( lpdd, 0, sizeof( DEVICE_DATA ) + s + 2 );
        lpdd->hDevInit = lpDriver->lpInit( (DWORD)lpParam ); // �����豸��������ӿڵĳ�ʼ������
        if( lpdd->hDevInit )  // ��ʼ���ɹ� ��
        {	// �ǣ������豸�����豸����
            lpdd->lpszName = (LPTSTR)(lpdd + 1);
            strcpy( lpdd->lpszName, lpDevName );
            *(lpdd->lpszName + s) = '0' + index;
            *(lpdd->lpszName + s + 1) = 0;

            lpdd->index = index;
            lpdd->lpDriver = lpDriver;
			lpdd->hFSDAttach = NULL;
			lpdd->lpfsd = NULL;
			lpdd->bFree = bFreeStruct;

			EnterCriticalSection( &csDeviceObjList );	

			ObjList_Init( (LPOBJLIST*)&lpDeviceObjList, &lpdd->obj, OBJ_DEV, (ULONG)GetCurrentProcess() );//(ULONG)GetCurrentProcessId() );
 
			LeaveCriticalSection( &csDeviceObjList );	

            return (HANDLE)lpdd;
        }
		else
		{
			WARNMSG( DEBUG_DOREGISTERDRIVER, ( "Device_Register: Device(%s) Index(%d) Init failure!.\r\n.", lpDevName, index ) );
		}
        free( lpdd );
    }
    return NULL;
}

// *****************************************************************
//������HANDLE WINAPI Device_RegisterDriver( LPCTSTR lpDevName,
//                                     int index, 
//                                     const DEVICE_DRIVER FAR * lpDriver,
//                                     LPVOID lpParam )
//������
//	IN lpDevName-�豸������
//	IN index-�豸������
//  IN lpDriver-�豸���ý���ָ��
//  IN lpParam-���ø��豸��Init����ʱ�����ݸò���
//����ֵ��
//	�ɹ��������豸���;ʧ�ܣ�����NULL
//����������
//	ע���豸�������򡣾�̬���Ӱ汾
//����: 
//	ϵͳAPI
// *****************************************************************
HANDLE WINAPI Dev_RegisterDriver( LPCTSTR lpDevName, UINT index, const DEVICE_DRIVER FAR * lpDriver, LPVOID lpParam )
{
	return DoRegisterDriver( lpDevName, index, lpDriver, lpParam, FALSE );
}

// *****************************************************************
//������HANDLE WINAPI Device_RegisterDevice( LPCTSTR lpDevName, UINT index, LPCTSTR lpszLib, LPVOID lpParam )
//������
//	IN lpDevName - �豸��
//	IN index - �豸������
//	IN lpszLib - ��̬���ӿ��ļ���
//	IN lpParam - ���ݸ��豸��ʼ�������Ĳ���
//����ֵ��
//	����ɹ����ط�NULL�ľ��,���򣬷���NULL
//����������
//	ע���豸�������򡣶�̬���ӿ�汾
//����: 
//	ϵͳAPI
// *****************************************************************

HANDLE WINAPI Device_RegisterDevice( LPCTSTR lpDevName, UINT index, LPCTSTR lpszLib, LPVOID lpParam )
{
	HANDLE handle = NULL;
	//check param
	if( strlen( lpDevName ) != 3 || index <= 9 )
	{
		HMODULE hModule = LoadLibrary( lpszLib );
		
		if( hModule )
		{
			const LPCTSTR szDrvName[] = { "_Init",
				                          "_Deinit",
										  "_IOControl",
										  "_Open",
										  "_Close",
										  "_Read",
										  "_Write",
										  "_Seek",
										  "_PowerUp",
										  "_PowerDown" };

			DEVICE_DRIVER * lpDriver = malloc( sizeof(DEVICE_DRIVER) );
			if( lpDriver )
			{
				int i;
				PFNVOID * lppFun = (PFNVOID *)lpDriver;
				TCHAR bufName[32];
				memset( lpDriver, 0, sizeof(DEVICE_DRIVER) );

				for( i = 0; i < sizeof(szDrvName) / sizeof(LPCTSTR); i++, lppFun++ )
				{
					strcpy( bufName, lpDevName );
					strcat( bufName, szDrvName[i] ); 
					
					*lppFun = (PFNVOID)GetProcAddress( hModule, bufName );
					if( *lppFun == 0 )
					{
						goto _error_return;
					}
				}
				//ok
				if( (handle = DoRegisterDriver( lpDevName, index, lpDriver, lpParam, TRUE )) == NULL )
				{
					free( lpDriver );
				}
			}
		}
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
	}
_error_return:
	return handle;
}

// *****************************************************************
// ������BOOL WINAPI Device_Deregister( HANDLE handle )
// ������
//		IN handle - �豸�������RegisterDevice �� RegisterDriver���صľ����
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		ע���豸��������
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_Deregister( HANDLE handle )
{
    DEVICE_DATA * lpdd = _GetHDEVPtr( handle ); // �ɾ���õ��豸���ݽṹָ��
	LPOBJLIST lpDevOpen;
	
	BOOL bRetv = FALSE;

	if( lpdd  )
	{
        EnterCriticalSection( &csDeviceObjList ); // �����ͻ��

		lpDevOpen = (LPOBJLIST)lpDeviceOpenObjList; //ȫ���豸��/���ö�������
		// �����Ѵ򿪸��豸�Ķ����������ע����
		for( ; lpDevOpen; lpDevOpen = lpDevOpen->lpNext )
		{
			if( ( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev == lpdd )  // 
			{   // �ҵ��ö���
				( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev = NULL; // ��������
				Interlock_Decrement( (LPLONG)&lpdd->obj.iRefCount ); // ��������
			}
		}
		// �ö����Ƿ���ĳ�ļ�ϵͳ���� ��
		if( lpdd->hFSDAttach && lpdd->lpfsd )
		{	//�ǣ�֪ͨ���ļ�ϵͳȥע����
			RETAILMSG( 1, ( "Detach device.\r\n." ) );
			lpdd->lpfsd->lpFSDInitDrv->lpDetachDevice( lpdd->hFSDAttach );
			--lpdd->lpfsd->iRefCount;// ���ٶԸ��ļ�ϵͳ������
		}
		// �����豸�����ϵͳ��������ȥ��
	    if( ObjList_Remove( (LPOBJLIST*)&lpDeviceObjList,  &lpdd->obj ) )
		{	// ֪ͨ�豸����������ͷź������ͷŸ��豸����
			lpdd->lpDriver->lpDeinit( lpdd->hDevInit );
			lpdd->obj.objType = OBJ_NULL;
			if( lpdd->bFree )
				free( (LPVOID)lpdd->lpDriver );
			free( lpdd );
			bRetv = TRUE;
		}
		LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��
	}
	return bRetv;
}

// *****************************************************************
// ������static DWORD FASTCALL EnumDevice( LPDEVICE_ENUM_PROC lpEnumFunc, LPVOID lpParam )
// ������
//		IN lpEnumFunc - ö�ٻص��������
//		IN lpParam - ���ݸ��ص������Ĳ���
// ����ֵ��
//		0	
// ����������
//		ö�������豸����
// ����: 
// *****************************************************************

typedef BOOL ( CALLBACK * LPDEVICE_ENUM_PROC )( LPDEVICE_DATA lpDev, LPVOID lpParam );
static DWORD FASTCALL EnumDevice( LPDEVICE_ENUM_PROC lpEnumFunc, LPVOID lpParam )
{
	LPDEVICE_DATA lpDevList;

	lpDevList = lpDeviceObjList;
	
	for( ; lpDevList ; lpDevList = (LPDEVICE_DATA)lpDevList->obj.lpNext )
	{
		if( lpEnumFunc( lpDevList, lpParam ) == FALSE )
			break;
	}
	return 0;
}

// *****************************************************************
// ������DWORD WINAPI Device_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen )
// ������
//		OUT lpszDevList �C ���ڽ����豸����ʽ�Ļ��棬���ص����ݵĸ�ʽΪ:��COM0:\0PRN1:\0USB5\0\0����
//		IN lpdwBufLen - lpszDevList����Ĵ�С�����绺�治���Ա������ݣ���������Ҫ�Ļ����С
// ����ֵ��
//		ERROR_SUCCESS �����ɹ�
//		ERROR_INSUFFICIENT_BUFFER �� ����Ĵ�С�����Ա����κ�һ������
//		ERROR_INVALID_PARAMETER��������Ч�Ĳ���
//		ERROR_MORE_DATA����������Ĵ�С�����Ա������е�����
//		ERROR_NO_MORE_DEVICES������û���豸
// ����������
//		ö�ٵ�ǰϵͳ���豸���������豸��
// ����: 
//		ϵͳAPI
// *****************************************************************

DWORD WINAPI Device_Enum( LPTSTR lpszDevList, LPDWORD lpdwBuflen )
{
	LPDEVICE_DATA lpDevList;
	DWORD dwBuflen;
	DWORD dwDeviceLen;
	DWORD dwRetv;
	LPTSTR lpstrBuf;

	//	������
	if( lpdwBuflen == NULL || lpszDevList == NULL )
	{
		return ERROR_INVALID_PARAMETER;
	}

	dwDeviceLen = 0;
	dwRetv = ERROR_SUCCESS;
	lpstrBuf = lpszDevList;
	dwBuflen = *lpdwBuflen - 1; // ��һ��Ϊ����\0������

	EnterCriticalSection( &csDeviceObjList );

	lpDevList = lpDeviceObjList;
	// �������е��豸���󣬲������ǵ���������lpszDevList
	for( ; lpDevList ; lpDevList = (LPDEVICE_DATA)lpDevList->obj.lpNext )
	{
		// 2 one is null char , one is ':'. because the lpDevList->lpszName no ':', example:"COM1"
		UINT len = strlen( lpDevList->lpszName ) + 2;
		dwDeviceLen += len;
		if( dwDeviceLen < dwBuflen )// �û������ڴ湻�� ��
		{	//��
		    strcpy( lpstrBuf, lpDevList->lpszName );
			*(lpstrBuf+len-2) = ':';
			*(lpstrBuf+len-1) = 0;
			lpstrBuf += len;
		}
		else
			dwRetv = !ERROR_SUCCESS; // ����������ѭ���ѵõ���ʵ���ڴ泤��
	}
    if( *lpdwBuflen )
	    *lpstrBuf = 0;
	dwDeviceLen++; // ����Ҫ�ĳ��ȣ���������'\0'��
	if( lpstrBuf != lpszDevList )
	{   // �Ѿ����������ݡ�has data in lpstrbuf, but may be not all data
		if( dwRetv == !ERROR_SUCCESS ) // �Ƿ񱣴������е����� ��
		{	//��������Ҫ�ĳ��Ⱥͷ���ֵ
			*lpdwBuflen = dwDeviceLen;
			dwRetv = ERROR_MORE_DATA;
		}
	}
	else
	{  // û�б����κ����ݡ�maybe ERROR_INSUFFICIENT_BUFFER  or ERROR_NO_MORE_DEVICES 
		if( lpDeviceOpenObjList == NULL )
			dwRetv = ERROR_NO_MORE_DEVICES; // û������豸
		else
		{
			*lpdwBuflen = dwDeviceLen;
			dwRetv = ERROR_INSUFFICIENT_BUFFER; // �ڴ泤�Ȳ����Ա����κ�����
		}
	}
	
	LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��

	return dwRetv;
}

// *****************************************************************
// ������HANDLE WINAPI Device_CreateFile( 
//							 LPCTSTR lpszName, 
//							 DWORD dwAccess, 
//							 DWORD dwShareMode, 
//							 HANDLE hProc )
// ������
//		IN lpszName - �豸��������"COM0:","PRN1:" 
//		IN dwAccess - ��ȡ����
//		IN dwShareMode - ����ģʽ
//		IN hProc - ӵ���߽��̾��
// ����ֵ��
//		����ɹ���������Ч���ļ������ʧ�ܣ�����INVALID_HANDLE_VALUE
// ����������
//		��ָ�����豸
// ����: 
//		ϵͳAPI
// *****************************************************************
#define DEBUG_DEVICE_CREATEFILE 0
HANDLE WINAPI Device_CreateFile( LPCTSTR lpszName, 
							 DWORD dwAccess, 
							 DWORD dwShareMode, 
							 HANDLE hProc )
{
    LPDEVICE_OPEN_FILE lpOpenFile = NULL;
	LPDEVICE_DATA lpDevObj;
	UINT index;

	DEBUGMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: lpszDevName=%s,dwAccess=0x%x,dwShareMode=0x%x.\r\n", lpszName, dwAccess, dwShareMode ) );

	// ������
	if( lpszName == NULL )
	{
		return INVALID_HANDLE_VALUE;  
	}
    if( strlen( lpszName ) < 4 )
		return INVALID_HANDLE_VALUE;
	if( *(lpszName + 3) < '0' || *(lpszName + 3) > '9' )
		return INVALID_HANDLE_VALUE;
	
	index = *(lpszName + 3) - '0';

	if( (lpDevObj = (LPDEVICE_DATA)FindDevice( lpszName, index ) ) ) // ��ϵͳ�豸��������������Ƿ��и��豸
	{	//ϵͳ�Ѿ�ע��
		DEBUGMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: has find device=%s.\r\n", lpszName ) );
		//����/׼�����豸�������ݽṹ
		lpOpenFile = malloc( sizeof(DEVICE_OPEN_DATA) );
		if( lpOpenFile )
		{
			lpOpenFile->hProcess = hProc;
			lpOpenFile->lpDev = lpDevObj;
			//�����豸��������Ĵ򿪺���
			lpOpenFile->hDevOpen = 0;
			if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
			{
				lpOpenFile->hDevOpen = lpDevObj->lpDriver->lpOpen( lpDevObj->hDevInit, dwAccess, dwShareMode );
				Sys_ReleaseException();
			}
			if( lpOpenFile->hDevOpen )  //�򿪳ɹ� ��
			{  // ��
				Interlock_Increment( (LPLONG)&lpDevObj->obj.iRefCount ); // ���ӶԸ��豸��������
				//���������
			    ObjList_Init( (LPOBJLIST*)&lpDeviceOpenObjList, &lpOpenFile->obj, OBJ_DEVOPEN, (ULONG)hProc );
				return (HANDLE)lpOpenFile;
			}
			else
			{
				WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: Dev_Open return NULL.\r\n" ) );
			}
		}
		else
		{
			WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: no enougn memory.\r\n" ) );
		}
		if( lpOpenFile )
		{
		    free( lpOpenFile );
		    lpOpenFile = NULL;
		}
	}
	else
	{
		WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: not find the device(%s).\r\n", lpszName ) );
	}
//	WARNMSG( DEBUG_DEVICE_CREATEFILE, ( "Device_CreateFile: not find the device(%s) or no memory!.\r\n", lpszName ) );
	return INVALID_HANDLE_VALUE;  
}

// *****************************************************************
// ������BOOL WINAPI Device_CloseFile( HANDLE hOpenFile )
// ������
//		IN hOpenFile - ����Device_Create���صľ�� 
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�ر�֮ǰ�򿪵��豸
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_CloseFile( HANDLE hOpenFile )
{
    BOOL bRetv = FALSE;
	LPDEVICE_OPEN_FILE lpOpenFile = (HANDLE)hOpenFile;

	if( lpOpenFile && lpOpenFile->obj.objType == OBJ_DEVOPEN )
	{	// �Ӷ��������Ƴ�
		ObjList_Remove( (LPOBJLIST*)&lpDeviceOpenObjList , &lpOpenFile->obj );
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			if( lpOpenFile->lpDev )
				lpOpenFile->lpDev->lpDriver->lpClose( lpOpenFile->hDevOpen ); // �����豸��������Ĺرպ���
			Sys_ReleaseException();
		}
		lpOpenFile->obj.objType = OBJ_NULL;
		free( lpOpenFile );
		bRetv = TRUE;
	}
	return bRetv;
}

// *****************************************************************
// ������BOOL WINAPI Device_ReadFile( 
//				  HANDLE hOpenFile,
//				  LPVOID lpvBuffer, 
//				  DWORD dwBytesToRead, 
//				  LPDWORD lpdwNumBytesRead,
//				  LPOVERLAPPED lpOverlapped )
// ������
//		IN hOpenFile - ���ļ����
//		OUT lpvBuffer - �������ݵ��ڴ�
//		IN dwBytesToRead - ��Ҫ�������ݴ�С
//		IN lpdwNumBytesRead - ����������ʵ��ȡ�����ݴ�С
//		IN lpOverlapped - ��֧��(����ΪNULL)
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		���豸��ȡ����
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_ReadFile( 
				  HANDLE hOpenFile,
				  LPVOID lpvBuffer, 
				  DWORD dwBytesToRead, 
				  LPDWORD lpdwNumBytesRead,
				  LPOVERLAPPED lpOverlapped )
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );
    BOOL bRetv = FALSE;

	if( lpOpenFile )
	{	//�򵥵����豸��������Ķ�����
		*lpdwNumBytesRead = 0xffffffff;
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			*lpdwNumBytesRead = lpOpenFile->lpDev->lpDriver->lpRead( lpOpenFile->hDevOpen, lpvBuffer, dwBytesToRead );
			Sys_ReleaseException();
		}
		if( *lpdwNumBytesRead == 0xffffffff )
			*lpdwNumBytesRead = 0;
		else
			bRetv = TRUE;
	}
	return bRetv;

}

// *****************************************************************
// ������BOOL WINAPI Device_WriteFile( 
//				   HANDLE hOpenFile, 
//				   LPCVOID lpvBuffer, 
//				   DWORD dwBytesToWrite,
//				   LPDWORD lpdwNumBytesWritten,
//				   LPOVERLAPPED lpOverlapped )
// ������
//		IN hOpenFile - ���ļ����
//		OUT lpvBuffer - д���豸�������ڴ�
//		IN dwBytesToWrite - ��Ҫд�����ݴ�С
//		IN lpdwNumBytesWritten - ����������ʵд�����ݴ�С
//		IN lpOverlapped - ��֧��(����ΪNULL)
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		д���ݵ��豸
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_WriteFile( 
				   HANDLE hOpenFile, 
				   LPCVOID lpvBuffer, 
				   DWORD dwBytesToWrite,
				   LPDWORD lpdwNumBytesWritten,
				   LPOVERLAPPED lpOverlapped )
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );
    BOOL bRetv = FALSE;

	*lpdwNumBytesWritten = 0;
	if( lpOpenFile )
	{	//�򵥵����豸���������д����
		*lpdwNumBytesWritten = 0xffffffff;
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			*lpdwNumBytesWritten = lpOpenFile->lpDev->lpDriver->lpWrite( lpOpenFile->hDevOpen, lpvBuffer, dwBytesToWrite );
			Sys_ReleaseException();
		}
		if( *lpdwNumBytesWritten == 0xffffffff )
			*lpdwNumBytesWritten = 0;
		else
			bRetv = TRUE;
	}
	return bRetv;
}

// *****************************************************************
// ������BOOL WINAPI Device_FlushFileBuffers( HANDLE hOpenFile ) 
// ������
//		IN hOpenFile - ���ļ����
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		����������ˢ�µ��豸
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_FlushFileBuffers( HANDLE hOpenFile ) 
{	//��ʱ��֧��
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// ������BOOL WINAPI Device_SetFileTime(
// 				     HANDLE hOpenFile, 
//					 CONST FILETIME *lpCreation, 
//					 CONST FILETIME *lpLastAccess, 
//					 CONST FILETIME *lpLastWrite ) 
// ������
//		IN hOpenFile - ���ļ����
//		IN lpCreation - �ļ�����ʱ�� 
//		IN lpLastAccess - �ļ�����ȡʱ��
//		IN lpLastWrite  - �ļ����д��ʱ��
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�����ļ�ʱ��
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_SetFileTime(
 				     HANDLE hOpenFile, 
					 CONST FILETIME *lpCreation, 
					 CONST FILETIME *lpLastAccess, 
					 CONST FILETIME *lpLastWrite ) 
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// ������BOOL WINAPI Device_GetFileTime(
// 				     HANDLE hOpenFile, 
//					 LPFILETIME lpftCreation, 
//					 LPFILETIME lpftLastAccess, 
//					 LPFILETIME lpftLastWrite ) 
// ������
//		IN hOpenFile - ���ļ����
//		OUT lpftCreation - �ļ�����ʱ�� 
//		OUT lpftLastAccess - �ļ�����ȡʱ��
//		OUT lpftLastWrite  - �ļ����д��ʱ��
// ����ֵ��
//		����ɹ�������TRUE;���򣬷���FALSE
// ����������
//		�õ��ļ�ʱ��
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_GetFileTime(
					HANDLE hOpenFile, 
					LPFILETIME lpftCreation, 
					LPFILETIME lpftLastAccess, 
					LPFILETIME lpftLastWrite ) 
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// ������DWORD WINAPI Device_GetFileSize( 
//						HANDLE hOpenFile, 
//						LPDWORD lpdwFileSizeHigh )

// ������
//		IN hOpenFile - ���ļ����
//		IN lpdwFileSizeHigh - �ļ���С�ĸ�32λ����
// ����ֵ��
//		����ɹ��������ļ���С�ĵ�32λ���ݣ�lpdwFileSizeHighΪ�ļ���С�ĸ�32λ���ݣ����򣬷���0xffffffff
// ����������
// ����:
//		ϵͳAPI 
// *****************************************************************

DWORD WINAPI Device_GetFileSize( 
						HANDLE hOpenFile, 
						LPDWORD lpdwFileSizeHigh )
{
    *lpdwFileSizeHigh = 0;
	SetLastError(ERROR_INVALID_FUNCTION);
    return 0xffffffff;
}

// *****************************************************************
// ������DWORD WINAPI Device_SetFilePointer(
//						 HANDLE hOpenFile, 
//						 LONG lDistanceToMove, 
//						 PLONG lpDistanceToMoveHigh,
//						 DWORD dwMoveMethod ) 
//
// ������
//		IN hOpenFile - ���ļ����
//		IN lDistanceToMove - �ƶ�����ĵ�32λ����
//		IN lpDistanceToMoveHigh - �����еĻ����ƶ�����ĸ�32λ����
//		IN dwMoveMethod  - �ƶ�����
// ����ֵ��
//		����ɹ������ص�ǰ��ֵ�����򣬷���0xffffffff
// ����������
//		�����ļ���ǰλ��
// ����: 
//		ϵͳAPI
// *****************************************************************

DWORD WINAPI Device_SetFilePointer(
						 HANDLE hOpenFile, 
						 LONG lDistanceToMove, 
						 PLONG lpDistanceToMoveHigh,
						 DWORD dwMoveMethod ) 
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );

    DWORD dwRetv = 0xffffffff;
    DWORD dodec = 0;

	if( lpOpenFile )
	{	// �򵥵����豸��������Ķ�λ����
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			dwRetv = lpOpenFile->lpDev->lpDriver->lpSeek( lpOpenFile->hDevOpen, lDistanceToMove, dwMoveMethod );
			Sys_ReleaseException();
		}
	}
    return dwRetv;
}

// *****************************************************************
// ������BOOL WINAPI Device_SetEndOfFile( HANDLE hOpenFile )
// ������
//		IN hOpenFile - ���ļ����
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�����ļ�����λ��
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_SetEndOfFile( HANDLE hOpenFile )
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// ������BOOL WINAPI Device_GetFileInformationByHandle(
//									HANDLE hOpenFile,
//									LPBY_HANDLE_FILE_INFORMATION lpFileInfo )

// ������
//		IN hOpenFile - ���ļ����
//		OUT lpFileInfo - �ļ���Ϣ�ṹ�����ڽ����ļ���Ϣ
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�õ��ļ���Ϣ
// ����: 
//		ϵͳAPI
// *****************************************************************

BOOL WINAPI Device_GetFileInformationByHandle(
									HANDLE hOpenFile,
									LPBY_HANDLE_FILE_INFORMATION lpFileInfo )
{
    SetLastError(ERROR_INVALID_FUNCTION);
    return FALSE;
}

// *****************************************************************
// ������BOOL WINAPI Device_IoControl(
//				   HANDLE hOpenFile,
//				   DWORD  dwIoControlCode,
//				   LPVOID lpvInBuf,
//				   DWORD dwInBufSize,
//				   LPVOID lpvOutBuf,
//				   DWORD dwOutBufSize,
//				   LPDWORD lpdwBytesReturned,
//				   LPOVERLAPPED lpOverlapped )
// ������
//		IN hOpenFile - �Ѵ��ļ����
//		IN dwIoControlCode - I/O���ƴ���
//		IN lpvInBuf - ��������ڴ��ַ
//		IN dwInBufSize - lpvInBuf���ڴ��С
//		OUT lpvOutBuf - ��������ڴ��ַ
//		IN dwOutBufSize - lpvOutBuf���ڴ��С
//		OUT lpdwBytesReturned - д��lpvOutBuf�����ݴ�С
//		IN lpOverlapped - ��֧�֣�ΪNULL��
// ����ֵ��
//		����ɹ�������TRUE; ���򣬷���FALSE
// ����������
//		�豸I/O���ƺ�����ڣ�ͨ������ͬ�Ŀ��ƴ�����ʵ�ֶ��豸�Ĺ������
// ����:
//		ϵͳAPI 
// *****************************************************************

BOOL WINAPI Device_IoControl(
				   HANDLE hOpenFile,
				   DWORD  dwIoControlCode,
				   LPVOID lpvInBuf,
				   DWORD dwInBufSize,
				   LPVOID lpvOutBuf,
				   DWORD dwOutBufSize,
				   LPDWORD lpdwBytesReturned,
				   LPOVERLAPPED lpOverlapped )
{
    LPDEVICE_OPEN_FILE lpOpenFile = _GetHDEVFilePtr( hOpenFile );

    BOOL bRetv = FALSE;

	if( lpOpenFile )
	{	//�򵥵����豸�����������غ���
		if( Sys_CaptureException() == EXCEPTION_NO_ERROR )
		{
			bRetv = lpOpenFile->lpDev->lpDriver->lpIOControl( 
													lpOpenFile->hDevOpen, 
													dwIoControlCode,
													lpvInBuf,
													dwInBufSize,
													lpvOutBuf,
													dwOutBufSize,
													lpdwBytesReturned );
			Sys_ReleaseException();
		}
	}
	return bRetv;
}


// *****************************************************************
// ������static LPOBJLIST FindDevice( LPCTSTR lpszName, UINT index )
// ������
//		IN lpszName - �豸�������磺"COM5"
//		IN index - �豸�����ţ� ��0 ~ 9
// ����ֵ��
//		���緢�֣����ظ��豸�Ķ���ָ�룻���򣬷���NULL
// ����������
//		���豸�������豸����ָ��
// ����: 
// *****************************************************************

static LPOBJLIST FindDevice( LPCTSTR lpszName, UINT index )
{
	LPOBJLIST lpRetv = NULL;
	LPOBJLIST lpObj;
//	int index;
	// ������
 //	if( strlen( lpszName ) < 4 )
//		goto DEV_RET;
//	if( *(lpszName + 3) < '0' || *(lpszName + 3) > '9' )
//		goto DEV_RET;//return FALSE;
//	index = *(lpszName + 3) - '0';

    EnterCriticalSection( &csDeviceObjList ); // �����ͻ��	
	lpObj = (LPOBJLIST)lpDeviceObjList;
	//������������
	while( lpObj )
	{
		if( strnicmp( ( (DEVICE_DATA *)lpObj)->lpszName, lpszName, 3 ) == 0 && 
			( (DEVICE_DATA *)lpObj )->index == index )
		{   // �ҵ���found it
			lpRetv = lpObj;
			break;
		}
		lpObj = lpObj->lpNext; // ��һ������
	}

//DEV_RET:
	LeaveCriticalSection( &csDeviceObjList ); //�뿪��ͻ��

	return lpRetv;
}

// *****************************************************************
// ������BOOL _InitDeviceMgr( void )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		��ʼ���豸������
// ����: 
//		���豸��������ʱ������øú���ȥ��ʼ���豸��������devsrc.c 
// *****************************************************************

BOOL _InitDeviceMgr( void )
{
	InitializeCriticalSection( &csDeviceObjList );
#ifdef __DEBUG
	csDeviceObjList.lpcsName = "CS-DEV";
#endif
    return TRUE;
}

// *****************************************************************
// ������BOOL _DeinitDeviceMgr( void )
// ������
//		��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		�ͷ��豸������
// ����: 
//		���豸�����˳�ʱ������øú���ȥ�ͷ��豸��������Դ��devsrc.c 
// *****************************************************************

BOOL _DeinitDeviceMgr( void )
{
	DeleteCriticalSection( &csDeviceObjList );
	return TRUE;
}

//////////////////////////////////////////////////////////////////
// file driver mgr

// *****************************************************************
// ������static LPDEVFSD GetFSD( LPCTSTR lpcszFSDName )
// ������
//		IN lpcszFSDName - �ļ�ϵͳ��
// ����ֵ��
//		����ɹ������ص�ǰϵͳ���ڵ��ļ�ϵͳ����ָ�롣���򷵻�NULL
// ����������
//		�õ��Ѽ��ص��ļ�ϵͳ����
// ����: 
// *****************************************************************

//	�ļ�ϵͳ��������
static LPDEVFSD lpFsdList = NULL;

static LPDEVFSD GetFSD( LPCTSTR lpcszFSDName )
{
    LPDEVFSD pfsd = lpFsdList;
    while( pfsd )
    {
        if( stricmp( pfsd->lpszFSDName, lpcszFSDName ) == 0 )
            return pfsd; //�ҵ���found
        pfsd = pfsd->lpNext;
    }
    return NULL;
}

// *****************************************************************
// ������BOOL WINAPI Device_RegisterFSD ( LPCTSTR lpcszFSDName, HANDLE hFSDRegister, const FSDINITDRV * lpfsd )
// ������
//		IN lpcszFSDName - �ļ�ϵͳ�� 
//		IN hFSDRegister - ���ݸ��ļ�ϵͳ��ʼ���ӿڵĲ���
//		IN lpfsd - �ļ�ϵͳ��ʼ���ӿ�
// ����ֵ��
//		����ɹ�������TRUE�����򷵻�FALSE
// ����������
//		ע���ļ�ϵͳ
// ����: 
// *****************************************************************
#define DEBUG_DEVICE_REGISTERFSD 0
BOOL WINAPI Device_RegisterFSD ( LPCTSTR lpcszFSDName, HANDLE hFSDRegister, const FSDINITDRV * lpfsd )
{	
    LPDEVFSD p;

	// �����ļ�ϵͳ�ṹ����
    DEBUGMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD: RegisterFSD(=%s).\r\n", lpcszFSDName ) );
	p = (LPDEVFSD)malloc( sizeof( DEVFSD ) + sizeof( FSDINITDRV ) + strlen( lpcszFSDName ) + 1 );
    if( p )
    {
	    EnterCriticalSection( &csDeviceObjList ); // �����ͻ��	
        DEBUGMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD: GetFSD.\r\n" ) );
		if( GetFSD( lpcszFSDName ) == NULL )
		{	// û�з���
			p->lpFSDInitDrv = (LPFSDINITDRV)(p + 1);
			p->lpszFSDName = (LPTSTR)(p->lpFSDInitDrv + 1);
			
			strcpy( p->lpszFSDName, lpcszFSDName );
			memcpy( p->lpFSDInitDrv, lpfsd, sizeof( FSDINITDRV ) );
			p->hFSDRegister = hFSDRegister;
			//p->hFsdInit = hFSD;
			p->lpNext = lpFsdList;
			p->iRefCount = 1;
			lpFsdList = p;

			LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��
		    DEBUGMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD: RegisterFSD(=%s) success.\r\n", lpcszFSDName ) );
		}
		else
		{   // �Ѿ����ڸ��ļ�ϵͳ���˳�
		    LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��
			free( p );
			p = NULL;
		}
    }
	else
	{
		WARNMSG( DEBUG_DEVICE_REGISTERFSD, ( "Device_RegisterFSD:no enough mem.\r\n" ) );
	}
    return p != NULL;
}

// *****************************************************************
// ������BOOL WINAPI Device_UnregisterFSD ( LPCTSTR lpcszFSDName )
// ������
//		IN lpcszFSDName - �ļ�ϵͳ�� 
// ����ֵ��
//		����ɹ�������TRUE�����򷵻�FALSE
// ����������
//		ע���ļ�ϵͳ
// ����: 
// *****************************************************************

BOOL WINAPI Device_UnregisterFSD ( LPCTSTR lpcszFSDName )
{
    LPDEVFSD pfsd, prev;

    EnterCriticalSection( &csDeviceObjList ); // �����ͻ��	
   
	prev = pfsd = lpFsdList;
    
    while( pfsd )
    {
        if( stricmp( pfsd->lpszFSDName, lpcszFSDName ) == 0 )
        {	//������
			if( --pfsd->iRefCount == 0 )
			{	//����������Ϊ0�����ͷŸö���
				if( prev == lpFsdList )
					lpFsdList = lpFsdList->lpNext;
				else
					prev->lpNext = pfsd->lpNext;
				free( pfsd );
			}
		    LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��	

            return TRUE;
        }
        prev = pfsd;
        pfsd = pfsd->lpNext;
    }

    LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��	
    return FALSE;
}

// *****************************************************************
// ������BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
// ������
//		IN hDevice - �豸���(����RegisterDevice / RegisterDriver���صľ��)
//		IN lpcszFSDName - �ļ�ϵͳ��
// ����ֵ��
//		����ɹ�������TRUE�����򷵻�FALSE
// ����������
//		��һ���豸��һ���ļ�ϵͳ����
// ����: 
//		ϵͳAPI
// *****************************************************************

#define DEBUG_DEVICE_LOADFSD 0
BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
{    
	LPDEVFSD lpfsd;
	DEVICE_DATA * lpdd;

    EnterCriticalSection( &csDeviceObjList ); // �����ͻ��	
	lpfsd = GetFSD( lpcszFSDName );
	if( lpfsd ) // �����и��ļ�ϵͳ��������������
	    lpfsd->iRefCount++;
    LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��	

	lpdd = _GetHDEVPtr( hDevice );
		
	if( lpfsd && lpdd )
	{   // �����ļ�ϵͳ�����ӹ���
		lpdd->hFSDAttach = lpfsd->lpFSDInitDrv->lpAttachDevice( lpfsd->hFSDRegister, lpdd->lpszName );
		DEBUGMSG( DEBUG_DEVICE_LOADFSD, ( "Device_LoadFSD:Attach Handle=%x.\r\n", lpdd->hFSDAttach ) );
		if( lpdd->hFSDAttach )
		{	//���ӳɹ�
			lpdd->lpfsd = lpfsd;
			return TRUE;
		}
	}
	// ʧ�ܣ�������������
	if( lpfsd )
	    lpfsd->iRefCount--;

    return FALSE;
}

// *****************************************************************
// ������static BOOL _Device_LoadFSD ( DEVICE_DATA * lpdd )
// ������
//		IN lpdd - �豸����ָ��
//		IN lpcszFSDName - �ļ�ϵͳ��
// ����ֵ��
//		����ɹ�������TRUE�����򷵻�FALSE
// ����������
//		��һ���豸��һ���ļ�ϵͳȥ������
// ����: 
//		
// *****************************************************************

#define DEBUG_DoUnloadFSD 1
static BOOL DoUnloadFSD ( DEVICE_DATA * lpdd )
{    
	BOOL bRetv = FALSE;

	DEBUGMSG( DEBUG_DoUnloadFSD, ( "DoUnloadFSD entry.\r\n." ) );
	if( lpdd  )
	{
		// �ö����Ƿ���ĳ�ļ�ϵͳ���� ��
		if( lpdd->hFSDAttach && lpdd->lpfsd )
		{	//�ǣ�֪ͨ���ļ�ϵͳȥע����
			//RETAILMSG( 1, ( "Detach device.\r\n." ) );
			lpdd->lpfsd->lpFSDInitDrv->lpDetachDevice( lpdd->hFSDAttach );
			--lpdd->lpfsd->iRefCount;// ���ٶԸ��ļ�ϵͳ������
			lpdd->hFSDAttach = lpdd->lpfsd = 0;
			bRetv = TRUE;
		}
		else
		{
			WARNMSG( DEBUG_DoUnloadFSD, ( "DoUnloadFSD the device not connect to a fsd.\r\n." ) );
		}
		

		/*

        EnterCriticalSection( &csDeviceObjList ); // �����ͻ��


		lpDevOpen = (LPOBJLIST)lpDeviceOpenObjList; //ȫ���豸��/���ö�������
		// �����Ѵ򿪸��豸�Ķ����������ע����
		for( ; lpDevOpen; lpDevOpen = lpDevOpen->lpNext )
		{
			if( ( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev == lpdd )  // 
			{   // �ҵ��ö���
				( (LPDEVICE_OPEN_FILE)lpDevOpen )->lpDev = NULL; // ��������
				Interlock_Decrement( (LPLONG)&lpdd->obj.iRefCount ); // ��������
			}
		}

		LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��
		*/
	}
	DEBUGMSG( DEBUG_DoUnloadFSD, ( "DoUnloadFSD leave(ret=%d).\r\n", bRetv ) );
	return bRetv;
}


// *****************************************************************
// ������BOOL WINAPI Device_LoadFSD ( HANDLE hDevice, LPCTSTR lpcszFSDName )
// ������
//		IN hDevice - �豸���(����RegisterDevice / RegisterDriver���صľ��)
//		IN lpcszFSDName - �ļ�ϵͳ��
// ����ֵ��
//		����ɹ�������TRUE�����򷵻�FALSE
// ����������
//		��һ���豸��һ���ļ�ϵͳ����
// ����: 
//		ϵͳAPI
// *****************************************************************

#define DEBUG_DEVICE_UNLOADFSD 0
BOOL WINAPI Device_UnloadFSD ( HANDLE hDevice )
{    
    DEVICE_DATA * lpdd = _GetHDEVPtr( hDevice ); // �ɾ���õ��豸���ݽṹָ��
	if( lpdd )
		return DoUnloadFSD( lpdd );
	return FALSE;
}

// *****************************************************************
// ������void DeregisterAllDevice( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		ע�����е��豸
// ����: 
// *****************************************************************
//
void DeregisterAllDevice( void )
{
	LPDEVICE_DATA lpdev;

	EnterCriticalSection( &csDeviceObjList ); // �����ͻ��	
	lpdev = (LPDEVICE_DATA)lpDeviceObjList;
	while( lpdev )
	{
		DoUnloadFSD( lpdev );
		Device_Deregister( (HANDLE)lpdev );
		lpdev = (LPDEVICE_DATA)lpDeviceObjList;
	}
	LeaveCriticalSection( &csDeviceObjList ); // �뿪��ͻ��	
}
//

// *****************************************************************
// ������void _InitDllDevice( void )
// ������
//		��
// ����ֵ��
//		��
// ����������
//		��ע��������������
// ����: 
// *****************************************************************

//��ע����������ص� �ļ�����
//[HKEY_HARDWARE_ROOT\Drivers\BuiltIn\Serial]
//"Prefix"="XXX" //����ǰ׺��,�ο�����������ӿڶ��塱
//"Dll"="serial.so" //���������ļ���
//"Index"=dword:1 //��ע����������ʱ��������
//�ο���RegisterDriver���͡�RegisterDevice��

#define DEBUG_InitDllDevice 0
void _InitDllDevice( void )
{
	HKEY hSubKey;
	HKEY hDriverKey;


    if( RegOpenKeyEx( HKEY_HARDWARE_ROOT, szBuiltInKey, 0, KEY_ALL_ACCESS, &hSubKey ) == ERROR_SUCCESS )
	{
		//TCHAR filePath[MAX_PATH];
		int n = 0;

		while( 1 )
		{
//			DWORD dwType;
			TCHAR tcValueName[32];  //
			DWORD dwValueNameSize = sizeof( tcValueName );
			TCHAR tcPrefix[8];
			DWORD dwPrefixSize = sizeof( tcPrefix );
			TCHAR tcDll[MAX_PATH];
			DWORD dwDllSize = sizeof( tcDll );
			DWORD dwIndex;
			DWORD dwIndexSize = sizeof(dwIndex);
			DEBUGMSG( DEBUG_InitDllDevice, ( "_InitDllDevice: call RegEnumKeyEx(%d).\r\n", n ) );			
			if( ERROR_SUCCESS != RegEnumKeyEx( 
										hSubKey, 
										n++, 
										tcValueName, 
										&dwValueNameSize, 
										0, 
										NULL, 
										NULL, 
										NULL ) )
				break;
			if( ERROR_SUCCESS != RegOpenKeyEx( 
										hSubKey,
										tcValueName,
										0,
										KEY_ALL_ACCESS,
										&hDriverKey ) )
				continue;
			if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szPrefix, NULL, NULL, tcPrefix, &dwPrefixSize ) )
			{
				goto _CLOSE_HKEY;
			}
			if( dwPrefixSize != 4 )
			{
				goto _CLOSE_HKEY;
			}
			if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szDll, NULL, NULL, tcDll, &dwDllSize ) )
			{
				goto _CLOSE_HKEY;
			}
			if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szIndex, NULL, NULL, (LPBYTE)&dwIndex, &dwIndexSize ) )
			{
				goto _CLOSE_HKEY;
			}
			if( dwIndex > 9 )
			{
				goto _CLOSE_HKEY;
			}
			Device_RegisterDevice( tcPrefix, dwIndex, tcDll, 0 );
_CLOSE_HKEY:
			RegCloseKey( hDriverKey );
		}
		RegCloseKey( hSubKey );
	}
}

// *****************************************************************
// ������static BOOL CALLBACK DoPowerHandler( LPDEVICE_DATA lpDev, LPVOID lpParam )
// ������
//		IN lpDev - �豸��������ӿڶ���
//		IN lpParam - �����0������������������0�����ػ�����
// ����ֵ��
//		TRUE
// ����������
//		ö�����е��豸����֪ͨ�俪����ػ�
// ����: 
//
// *****************************************************************

static BOOL CALLBACK DoPowerHandler( LPDEVICE_DATA lpDev, LPVOID lpParam )
{
	if( lpParam )
	{  //on
	    if( lpDev->lpDriver->lpPowerUp )
			lpDev->lpDriver->lpPowerUp(lpDev->hDevInit);
	}
	else
	{
	    if( lpDev->lpDriver->lpPowerDown )
			lpDev->lpDriver->lpPowerDown(lpDev->hDevInit);
	}
	return TRUE;
}

// **************************************************
// ������DWORD WINAPI Device_ServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
// ������
//		IN hServer - ������
//		IN dwServerCode - ������
//		IN dwParam - ��һ������
//		IN lpParam - �ڶ�������
// ����ֵ��
//		�����벻ͬ�Ŀ���
// ����������
//		�������߿��ƣ��������ϵͳ����֪ͨ����
// ����: 
//		ϵͳAPI	
// ************************************************

DWORD WINAPI Device_ServerHandler( HANDLE hServer, DWORD dwServerCode , DWORD dwParam, LPVOID lpParam )
{
	extern void ShutDownDevice( void );
	switch( dwServerCode )
	{
	case SCC_BROADCAST_POWER:
		if( dwParam == SBP_APMSUSPEND )
		{
			EnumDevice( DoPowerHandler, 0 );
			SaveAsRegister(NULL);  //�������е�ע�����Ϣ
		}
		else if( dwParam == SBP_APMRESUMESUSPEND )
		{
			EnumDevice( DoPowerHandler, (LPVOID)1 );
		}
		return TRUE;
	//case SCC_BROADCAST_POWERON:
		//EnumDevice( DoPowerHandler, (LPVOID)1 );
		//break;
	case SCC_BROADCAST_SHUTDOWN:
		ShutDownDevice();
		break;
	default:
		return Sys_DefServerProc( hServer, dwServerCode , dwParam, lpParam );
	}
	return 0;
}
/*
#define ACTIVE_KEY_PATH_LEN ( sizeof(szActiveKey) + 8 )
static BOOL ActiveAndLoadDriver( 
								LPCTSTR lpszDevKey,
								const REGINI * lpRegEnts, 
								DWORD cRegEnts, 
								LPVOID lpvParam,
								DWORD dwIndex,
								)
{
	UINT uiActiveNum;
    TCHAR szActiveKeyPath[ACTIVE_KEY_PATH_LEN];
	HKEY hActiveKey = NULL;
	DWORD dwDisposition;
	int n = 1;

    // �����µ�Active��
	while( n < 100 )
	{
		uiActiveNum = InterlockedIncrement( &uiCurrentActiveNum ) - 1;
		sprintf( szActiveKeyPath, "%s\\%02d", szActiveKey, uiActiveNum );
		
		// �����¼�
		if( ERROR_SUCCESS != RegCreateKeyEx(
			HKEY_HARDWARE_ROOT,
			szActiveKeyPath,
			0,
			NULL,
			0,
			0,
			NULL,
			&hActiveKey,     // HKEY result
			&dwDisposition) )
		{
			goto _error_return;
		}
		if( dwDisposition != REG_CREATED_NEW_KEY )
		{
			RegCloseKey( hActiveKey );
		}
		else
			break;
	}
	if( n == 100 )
		goto _error_return;
	// д�û���ֵ�� Active Keu
	if( lpRegEnts && cReg )
	{
		HANDLE hCallerProcess = GetCallerProcess();
        for( n = 0; n < cReg; n++ )
		{
            if( EEROR_SUCESS != RegSetValueEx(
									hActiveKey,
									MapPtrToProcess( lpReg[cReg].lpszVal, hCallerProcess ),
									0,
									lpReg[cReg].dwType,
									MapPtrToProcess( lpReg[cReg].pData, lpReg[cReg].dwLen ),
									lpReg[cReg].dwLen ) )
			{
				break;
			}
		}
		if( n != cReg )
			goto _error_return;
    }

	// д���������ֵ�� Active Keu��Load Key
    if( lpszDevKey != NULL )
	{    
        if( ERROR_SUCCESS != RegSetValueEx(
								hActiveKey,
								szPath,
								0,
								REG_SZ,
								(LPBYTE)lpszDevKey,
								strlen( lpszDevKey ) + 1 ) )
		{
			goto _error_return;
		}
    }

    if( dwIndex == -1 )
	{	// ��ϵͳ��������ֵ
        dwIndex = 0;
		EnterCriticalSection( &csDeviceObjList ); // �����ͻ��	

        while( dwIndex < 10 )
		{
			if( FindDevice( lpszPrefix, dwIndex ) == NULL )
				break;
			dwIndex++;
		}
		LeaveCriticalSection( &csDeviceObjList );
	}
	if( dwIndex < 10 )
	{
         char szDevName[16];
		 int len;
		 
		 strcpy( szDevName, lpszPrefix );
		 len = strlen( szDevName );
		 szDevName[len]=dwIndex+'0';
		 szDevName[len+1]=':';
		 szDevName[len+2]=0;
         if( ERROR_SUCCESS == RegSetValueEx( 
			                       hActiveKey,
                                   szName,
                                   0,
                                   REG_SZ,
                                   (LPBYTE)szDevName,
                                   strlen( szDevName ) + 1 ) )
		 {
			 HANDLE hDev;
			 if( ( hDev = Device_Register( lpszPrefix, dwIndex, lpszDll, lpvParam ) ) != NULL )
			 {	// �豸ע��ɹ�
				 if( ERROR_SUCCESS != RegSetValueEx( 
											hActiveKey,
											szHandle,
											0,
											REG_DWORD,
											(LPBYTE)hDev,
											sizeof(hDev) ) )
				 {
					 ERRORMSG( 1, ( "Can't write registry!.\r\n" ) );
				 }
				 if( *lpszPrefix )
				     SendDeviceInitMsg( lpszPrefix, dwIndex, hDev );

				 PNP_NotifyHandler( szActiveKeyPath );
				 return TRUE;
			 }
		 }
	}
_error_return:
	if( hActiveKey )
	{
		RegCloseKey( hActiveKey );
		RegDeleteKey( HKEY_HARDWARE_ROOT, szActiveKeyPath );		
	}
	return FALSE;
}

// **************************************************
// ������

// ������
//		IN 
// ����ֵ��
//		����ɹ������أ����򣬷���
// ����������
// ����: 
//		
// ************************************************

#define DEBUG_READ_DRIVER_REG 0
BOOL ReadDriverKeyFromRegistry( HKEY hDriverKey, 
							    DWORD * lpdwFlags,
								char * lpszDllBuf,
								UINT uiDllLen,
								char * lpszPrefixBuf,
								UINT uiPrefixLen��
								DWORD * lpdwIndex )
{
	DWORD dwValLen;	
    // 
    // �� flags
    dwValLen = sizeof(*lpdwFlags);
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szFlags,
        NULL,
        NULL,
        (LPBYTE)lpdwFlags,
        &dwValLen ) )
	{
        WARNMSG( DEBUG_READ_DRIVER_REG, ( "ReadDriverKeyFromRegistry: read flag error.\r\n") );
		goto _error_return;
	}

    // ����̬���ӿ��� read so name
    dwValLen = uiDllLen;
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szDll,
        NULL,
        NULL,
        lpszDllBuf,
        &dwValLen ) )
	{
		WARNMSG( DEBUG_READ_DRIVER_REG, ( "ReadDriverKeyFromRegistry: read dll error.\r\n") );
		goto _error_return;
	}

    // ��ǰ׺
    dwValLen = uiPrefixLen;
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szPrefix,
        NULL,
        NULL,
        lpszPrefixBuf,
        &dwValLen ) )
	{
		WARNMSG( DEBUG_READ_DRIVER_REG, ( "ReadDriverKeyFromRegistry: read prefix error.\r\n") );
		goto _error_return;
	}

    // ������

    dwValLen = sizeof(*lpdwIndex);
    if( ERROR_SUCCESS != RegQueryValueEx(
        hDriverKey,
        szIndex,
        NULL,
        NULL,
        (LPBYTE)lpdwIndex,
        &dwValLen ) )
	{
		*lpdwIndex = -1;
	}
	return TRUE;
_error_return:
	return FALSE;
}


typedef struct _REGINI {
	LPCWSTR lpszVal;
	LPBYTE pData;
	DWORD dwLen;
	DWORD dwType;
} REGINI;

// **************************************************
// ������HANDLE WINAPI Device_ActivateEx(
//							LPCWSTR lpszDevKey, 
//							const REGINI * lpRegEnts, 
//							DWORD cRegEnts, 
//							LPVOID lpvParam
//							)

// ������
//		IN lpszDevKey - ��Ҫע����豸��ע���
//		IN lpRegEnts - REGINI�ṹ����ָ�룬ָ����Ҫ���ӵļ�ֵ
//		IN cRegEnts - ָ��EGINI�ṹ����ĸ���
//		IN lpvParam - ���ݸ� XXX_Init( xxx, lpvParam ) �Ĳ���
// ����ֵ��
//		����ɹ������ط�NULL��������򣬷���NULL
// ����������
//		���û�ָ���ļ�ֵд��ע����[HKEY_HARDWARE_ROOT\Drivers\Active\(NO.xxx)]
//		ͨ��lpszDevKey���豸����ǰ׺�������ŵȼ����豸
//		���� lpvParam �� XXX_init( xxx, lpvParam )
// ����: 
//		ϵͳAPI	
// ************************************************
//static TCHAR const szBuiltInDriver[] = "Drivers\\Active";

#define DEBUG_ACTIVATEEX 0
HANDLE WINAPI Device_ActivateEx(
							LPCWSTR lpszDevKey, 
							const REGINI * lpRegEnts, 
							DWORD cRegEnts, 
							LPVOID lpvParam
							)
{
	TCHAR tcActiveNum[sizeof(szActiveKey)+2];
	HKEY hDriverKey;

	// ��/����Ƿ���� lpszDevKey

    if( EEROR_SUCCESS != RegOpenKeyEx(
        HKEY_HARDWARE_ROOT,
        lpszDevKey,
        0,
        KEY_ALL_ACCESS,
        &hDriverKey ) )
	{
        WARNMSG( DEBUG_ACTIVATEEX, (TEXT("registry key lpszDevKey=(%s) is not exist!.\r\n"), lpszDevKey ) );
		return NULL;
	}

    if( DoWriteInitReg( lpRegEnts, cRegEnts, tcActiveNum+sizeof(szActiveKey) ) )
	{
		tcActiveNum[sizeof(tcActiveNum)-1] = 0;
		if( ActiveAndLoadDriver( lpszDevKey, lpvParam ) == FALSE )
		{
			RegDeleteKey( HKEY_HARDWARE_ROOT, tcActiveNum );
		}
	}
}
	HKEY hDriverKey;
	if( ERROR_SUCCESS != RegOpenKeyEx( 
							HKEY_HARDWARE_ROOT,
							lpszDevKey,
							0,
							KEY_ALL_ACCESS,
							&hDriverKey ) )
		continue;
	if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, szPrefix, NULL, NULL, tcPrefix, &dwPrefixSize ) )
	{
		goto _CLOSE_HKEY;
	}
	if( dwPrefixSize != 4 )
	{
		goto _CLOSE_HKEY;
	}
	if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, "Dll", NULL, NULL, tcDll, &dwDllSize ) )
	{
		goto _CLOSE_HKEY;
	}
	if( ERROR_SUCCESS != RegQueryValueEx( hDriverKey, "Index", NULL, NULL, (LPBYTE)&dwIndex, &dwIndexSize ) )
	{
		goto _CLOSE_HKEY;
	}
	if( dwIndex > 9 )
	{
		goto _CLOSE_HKEY;
	}
	Device_RegisterDevice( tcPrefix, dwIndex, tcDll, 0 );

}


// **************************************************
// ������BOOL WINAPI Device_Deactivate(
//							HANDLE hActiveDevice
//							)

// ������
//		IN hActiveDevice - ��Device_ActivateEx���صľ��
// ����ֵ��
//		����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		ע���豸��������
//		ɾ��ע����[HKEY_HARDWARE_ROOT\Drivers\Active\(NO.xxx)]
//		�㲥WM_DEVICECHANGE
// ����: 
//		ϵͳAPI	
// ************************************************

BOOL WINAPI Device_Deactivate(
								HANDLE hActiveDevice
							)
{
}
*/

extern BOOL OEM_GetSystemPowerStatusEx(PSYSTEM_POWER_STATUS_EX pstatus, BOOL fUpdate);
BOOL WINAPI Device_GetSystemPowerStatusEx(
				PSYSTEM_POWER_STATUS_EX pstatus, 
				BOOL fUpdate )
{
#ifndef EML_WIN32
	if( pstatus )
		return OEM_GetSystemPowerStatusEx( pstatus, fUpdate );
#endif
	return FALSE;
}
