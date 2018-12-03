/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������� RASע�����Ϣ---��Ŀ��ģʽ��������Ϣ
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-02-26
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <ras.h>
//"\inc_drv"
#include "netsrvreg.h"
//"\inc_local"
#include "ras_call.h"


/***************  ȫ���� ���壬 ���� *****************/

static	LONG	RasHlp_OpenKey( HKEY hRoot, LPCTSTR pszPath, LPCTSTR pszKey, OUT HKEY* phkResult, BOOL fCreate );
static	BOOL	RasHlp_OpenEntry( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT HKEY* phkResult, BOOL fCreate );

/******************************************************/


// ========================================================================================== //
// ����ģʽ.................................................................................. //
// ========================================================================================== //

// ********************************************************************
// ������
// ������
//	OUT lpDialMthodName-�����ȡ���Ĳ���ģʽ���Ƶ�����
//	IN/OUT pdwCnt-ָ������ģʽ���Ƶ�����Ĵ�С���������ȡ���ĸ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������ö�� ����ģʽ���Ƶĸ���������ö�����еĲ���ģʽ���ơ�ȡ���� lpDialMthodName�Ƿ�ΪNULL
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_EnumDialMethod( OUT OPTIONAL LPDIALM_NAME lpDialMthodName, IN OUT DWORD* pdwCnt )
{
	HKEY		hKey;
	DWORD		dwSubKeys;
	DWORD		dwMaxEntries;
	DWORD		i;

	//�������
	if( !pdwCnt )
	{
		return FALSE;
	}
	//ö�� ����ģʽ���Ƶĸ���
	dwMaxEntries = *pdwCnt;
	*pdwCnt = 0;
	if( RegOpenKeyEx( KROOT_RasM, KPATH_RasM, 0, KEY_ALL_ACCESS, &hKey )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	if( RegQueryInfoKey(hKey, 0, 0, 0, &dwSubKeys, 0, 0, 0, 0, 0, 0, 0)!=ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return FALSE;
	}
	if( lpDialMthodName==NULL )
	{
		*pdwCnt = dwMaxEntries;
	}
	else
	{
		//ö�� ���еĲ���ģʽ����
		if( dwMaxEntries > dwSubKeys )
		{
			dwMaxEntries = dwSubKeys;
		}
		*pdwCnt = dwMaxEntries;
		for( i=0; i<dwMaxEntries; i++ )
		{
			memset( lpDialMthodName[i].szDialName, 0, RAS_MaxDialMethod );
			dwSubKeys = RAS_MaxDialMethod;
			RegEnumKeyEx( hKey, i, lpDialMthodName[i].szDialName, &dwSubKeys, 0, 0, 0, 0 );
		}
	}
	RegCloseKey( hKey );
	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN szDialName-����ģʽ����
//	IN dwOptMethod-��ȡ����ģʽ���Ƶ�ѡ��
//	OUT lpDialMthod-��ȡָ������ȱʡ�Ĳ���ģʽ����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡָ������ȱʡ�Ĳ���ģʽ����
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetDialMethod( LPCTSTR szDialName, DWORD dwOptMethod, OUT LPDIAL_METHOD lpDialMthod )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;
	LPCTSTR		pszName;

	//�������
	if( !lpDialMthod )
	{
		return FALSE;
	}
	//��ȡ ����ģʽ������
	if( !szDialName || !*szDialName )
	{
		pszName = KNAME_RasMDef;
	}
	else
	{
		pszName = szDialName;
	}
	if( RasHlp_OpenKey( KROOT_RasM, KPATH_RasM, pszName, &hKey, FALSE )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	memset( lpDialMthod->szDial, 0, RAS_MaxDM_Dial );
	memset( lpDialMthod->szLocal, 0, RAS_MaxDM_Str );
	memset( lpDialMthod->szArea, 0, RAS_MaxDM_Str );
	memset( lpDialMthod->szCountry, 0, RAS_MaxDM_Dial );
	//
	if( dwOptMethod & RASM_OPT_DIAL )
	{
		dwDataType = REG_SZ;
		dwMaxEntry = RAS_MaxDM_Dial;
		RegQueryValueEx( hKey, KITEM_RasM_DIAL, 0, &dwDataType, (LPBYTE)lpDialMthod->szDial, &dwMaxEntry );
	}
	if( dwOptMethod & RASM_OPT_LOCAL )
	{
		dwDataType = REG_SZ;
		dwMaxEntry = RAS_MaxDM_Str;
		RegQueryValueEx( hKey, KITEM_RasM_LOCAL, 0, &dwDataType, (LPBYTE)lpDialMthod->szLocal, &dwMaxEntry );
	}
	if( dwOptMethod & RASM_OPT_AREA )
	{
		dwDataType = REG_SZ;
		dwMaxEntry = RAS_MaxDM_Str;
		RegQueryValueEx( hKey, KITEM_RasM_AREA, 0, &dwDataType, (LPBYTE)lpDialMthod->szArea, &dwMaxEntry );
	}
	if( dwOptMethod & RASM_OPT_COUNTRY )
	{
		dwDataType = REG_SZ;
		dwMaxEntry = RAS_MaxDM_Str;
		RegQueryValueEx( hKey, KITEM_RasM_COUNTRY, 0, &dwDataType, (LPBYTE)lpDialMthod->szCountry, &dwMaxEntry );
	}

	RegCloseKey( hKey );
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN lpEntry-ָ��������Ŀ������
//	IN pszLocal-���صĵ绰
//	OUT pszPhoneNum-��ȡ�����յ绰����
//	IN dwLen-ָ��pszPhoneNum�ĳ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡָָ��������Ŀ����ȷ�������յ绰����
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_GenNum( RASENTRY* lpEntry, OPTIONAL LPCSTR pszNumLocal, OUT LPSTR pszNumNew, DWORD dwLen )
{
	DIAL_METHOD	stDialM;
	LPSTR		pszDialSel;
	DWORD		dwOptMethod;

	LPCSTR		pszLocal;
	LPSTR		pszNumTmp;

	DWORD		dwBytes;
	DWORD		dwCopy;
	LPSTR		pszCopy;
	char		chSel;

	//�������
	if( !lpEntry || dwLen<4 )
	{
		return FALSE;
	}
	//ȷ���û�ѡ��Ĳ���ģʽ
	switch( lpEntry->dwRasOpt & RASO_PHONE_MASK )
	{
	case RASO_PHONE_COUNTRY:
		pszDialSel = stDialM.szCountry;
		dwOptMethod = RASM_OPT_COUNTRY;
		break;
	case RASO_PHONE_AREA:
		pszDialSel = stDialM.szArea;
		dwOptMethod = RASM_OPT_AREA;
		break;
	//case RASO_PHONE_LOCAL:
	default:
		pszDialSel = stDialM.szLocal;
		dwOptMethod = RASM_OPT_LOCAL;
		break;
	}
	dwOptMethod |= RASM_OPT_DIAL;
	//��ȡ����ģʽ��Ϣ
	if( !RAPI_GetDialMethod( lpEntry->szDialName, dwOptMethod, &stDialM ) )
	{
		return FALSE;
	}

	//׼�����
	dwBytes = dwLen - sizeof(TCHAR);
	memset( pszNumNew, 0, dwLen );
	pszNumTmp = pszNumNew;
	if( pszNumLocal && *pszNumLocal )
	{
		pszLocal = pszNumLocal;
	}
	else
	{
		pszLocal = lpEntry->szLocalNumber;
	}

	//��� ��Ƶ��ʽ���� ���巽ʽ
#if 0
	if( stDialM.szDial[0] )
	{
		*pszNumTmp++ = stDialM.szDial[0];
		dwBytes --;
	}
#endif
	//������
	while( *pszDialSel && dwBytes )
	{
		chSel = *pszDialSel++;
		if( chSel==DIALNUM_E )
		{
			pszCopy = lpEntry->szCountryCode;
		}
		else if( chSel==DIALNUM_F )
		{
			pszCopy = lpEntry->szAreaCode;
		}
		else if( chSel==DIALNUM_G )
		{
			pszCopy = (LPSTR)pszLocal;
		}
		else
		{
			*pszNumTmp++ = chSel;
			dwBytes --;
			pszCopy = NULL;
		}
		//
		if( pszCopy )
		{
			dwCopy = strlen(pszCopy);
			if( dwBytes < dwCopy )
			{
				dwCopy = dwBytes;
			}
			memcpy( pszNumTmp, pszCopy, dwCopy );
			dwBytes -= dwCopy;
		}
	}

	return TRUE;
}


// ========================================================================================== //
// ������Ŀ.................................................................................. //
// ========================================================================================== //


// ********************************************************************
// ������
// ������
//	OUT lpEntryNames-�����ȡ���Ĳ�����Ŀ���Ƶ�����
//	IN/OUT lpdwCntEntries-ָ��������Ŀ���Ƶ�����Ĵ�С���������ȡ���ĸ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������ö�� ������Ŀ���Ƶĸ���������ö�����еĲ�����Ŀ���ơ�ȡ���� pszEntryName�Ƿ�ΪNULL
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_EnumEntries( LPCTSTR lpszPhonebook, OUT RASENTRYNAME* lpEntryNames, IN OUT DWORD* lpdwCntEntries )
{
	HKEY		hKey;
	DWORD		dwSubKeys;
	DWORD		dwMaxEntries;
	DWORD		i;

	//�������
	if( !lpdwCntEntries )
	{
		return FALSE;
	}
	//ö�� ������Ŀ���Ƶĸ���
	dwMaxEntries = *lpdwCntEntries;
	*lpdwCntEntries = 0;
	if( RegOpenKeyEx( KROOT_RasM, KPATH_RasM, 0, KEY_ALL_ACCESS, &hKey )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	if( RegQueryInfoKey(hKey, 0, 0, 0, &dwSubKeys, 0, 0, 0, 0, 0, 0, 0)!=ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return FALSE;
	}
	if( lpEntryNames==NULL )
	{
		*lpdwCntEntries = dwMaxEntries;
	}
	else
	{
		//ö�� ���еĲ�����Ŀ����
		if( dwMaxEntries > dwSubKeys )
		{
			dwMaxEntries = dwSubKeys;
		}
		*lpdwCntEntries = dwMaxEntries;
		for( i=0; i<dwMaxEntries; i++ )
		{
			dwSubKeys = RAS_MaxDialMethod;
			memset( lpEntryNames[i].szEntryName, 0, dwSubKeys );
			RegEnumKeyEx( hKey, i, lpEntryNames[i].szEntryName, &dwSubKeys, 0, 0, 0, 0 );
		}
	}
	RegCloseKey( hKey );
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	OUT pszEntryName-ָ��������Ŀ����
//	IN/OUT pdwSize-ָ�� pszEntryName�Ĵ�С���������ȡ���Ĵ�С
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡ ȱʡ������Ŀ������
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryDef( LPCTSTR lpszPhonebook, OUT LPTSTR pszEntryName, DWORD dwLen )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	
	//��ȡ Default �Ĳ�����Ŀ������
	if( RegOpenKeyEx( KROOT_RasE, KPATH_RasEDef, 0, KEY_ALL_ACCESS, &hKey )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	dwDataType = REG_SZ;
	dwResult = RegQueryValueEx(hKey, KITEM_RasEDef, 0, &dwDataType, (LPBYTE)pszEntryName, &dwLen);
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

// ********************************************************************
// ������
// ������
//	IN pszEntryName-ָ��������Ŀ����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// �������������� ȱʡ������Ŀ������
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryDef( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDisposition;
	
	//��ȡ Default �Ĳ�����Ŀ������
	if( RegCreateKeyEx( KROOT_RasE, KPATH_RasEDef, 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, &dwDisposition )==ERROR_SUCCESS )
	{
		return FALSE;
	}
	dwResult = RegSetValueEx(hKey, KITEM_RasEDef, 0, REG_SZ, (LPBYTE)pszEntryName, strlen(pszEntryName)+sizeof(TCHAR) );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
//	OUT lpEntry-�����ΪNULL�����ȡ RASENTRY����
//	IN/OUT lpdwEntrySize-ָ�� lpEntry�Ĵ�С���������ȡ���Ĵ�С
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡ ָ������ȱʡ������Ŀ��ָ����Ϣ
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT RASENTRY* lpEntry, IN OUT LPDWORD lpdwEntrySize )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;
	
	//�������
	if( !lpEntry || !lpdwEntrySize )
	{
		*lpdwEntrySize = 0;
		return FALSE;
	}
	//�� ������Ŀ
	if( !RasHlp_OpenEntry( lpszPhonebook, szEntryName, &hKey, FALSE ) )
	{
		*lpdwEntrySize = 0;
		return FALSE;
	}
	//
	dwMaxEntry = *lpdwEntrySize;
	*lpdwEntrySize = 0;
	dwDataType = REG_BINARY;
	dwResult = RegQueryValueEx( hKey, KITEM_RasE_ENTRY, 0, &dwDataType, (LPBYTE)lpEntry, &dwMaxEntry );
	if( dwResult==ERROR_SUCCESS )
	{
		*lpdwEntrySize = dwMaxEntry;
	}
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
//	IN lpEntry-�����ΪNULL�����ȡ RASENTRY����
//	IN dwEntrySize-ָ�� lpEntry�Ĵ�С
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// �������������� ������Ŀ��ָ����Ϣ�������½�1��������Ŀ
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, RASENTRY* lpEntry, DWORD dwEntrySize )
{
	DWORD		dwResult;
	HKEY		hKey;

	//�������
	if( !lpEntry || !dwEntrySize )
	{
		return FALSE;
	}
	//�� ������Ŀ
	if( !RasHlp_OpenEntry( lpszPhonebook, szEntryName, &hKey, TRUE ) )
	{
		return FALSE;
	}
	//����
	dwResult = RegSetValueEx( hKey, KITEM_RasE_ENTRY, 0, REG_BINARY, (LPBYTE)lpEntry, dwEntrySize );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
//	OUT lpDeviceConfigOut-�����ΪNULL�����ȡ RASENTRY����
//	IN/OUT lpdwSize-ָ�� lpDeviceConfigOut�Ĵ�С���ͻ�ȡ���ú�ĳ���
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡ��Ŀ�����ã��������豸��Ĭ������
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, OUT LPVOID lpDeviceConfigOut, IN OUT DWORD* lpdwSize )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;
	BOOL		fGetCfg;

	//
	dwMaxEntry = *lpdwSize;
	*lpdwSize = 0;
	fGetCfg = FALSE;
	if( lpDeviceConfigOut==NULL )
	{
		return FALSE;
	}
	//��ȡ��Ŀ������
	if( szEntryName && *szEntryName )
	{
		if( RasHlp_OpenKey( KROOT_RasE, KPATH_RasE, szEntryName, &hKey, FALSE)==ERROR_SUCCESS )
		{
			dwDataType = REG_BINARY;
			if( RegQueryValueEx(hKey, KITEM_RasE_DEVCFG, 0, &dwDataType, (LPBYTE)lpDeviceConfigOut, &dwMaxEntry)==ERROR_SUCCESS )
			{
				fGetCfg = TRUE;
				*lpdwSize = dwMaxEntry;
			}
			RegCloseKey( hKey );
		}
	}
	//�豸��Ĭ������
	if( !fGetCfg && pszDrvReg )
	{
		if( RegOpenKeyEx( HKEY_HARDWARE_ROOT, pszDrvReg, 0, KEY_ALL_ACCESS, &hKey )==ERROR_SUCCESS )
		{
			dwDataType = REG_BINARY;
			if( RegQueryValueEx(hKey, ITEM_Drv_DEVCFG, 0, &dwDataType, (LPBYTE)lpDeviceConfigOut, &dwMaxEntry)==ERROR_SUCCESS )
			{
				*lpdwSize = dwMaxEntry;
				fGetCfg = TRUE;
			}
			RegCloseKey( hKey );
		}
	}

	return fGetCfg;
}

// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
//	IN lpDeviceConfigIn-�����ΪNULL�����ȡ RASENTRY����
//	IN dwSize-ָ�� lpDeviceConfigIn�Ĵ�С
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡ��Ŀ�����ã��������豸��Ĭ������
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, LPVOID lpDeviceConfigIn, DWORD dwSize )
{
	HKEY		hKey;
	DWORD		dwMaxEntry;
	BOOL		fSuccess;
	DWORD		dwDisposition;

	//
	if( lpDeviceConfigIn==NULL )
	{
		return FALSE;
	}
	//��Ŀ������
	dwMaxEntry = dwSize;
	fSuccess = TRUE;
	if( szEntryName )
	{
		if( RasHlp_OpenKey( KROOT_RasE, KPATH_RasE, szEntryName, &hKey, FALSE)==ERROR_SUCCESS )
		{
			if( RegSetValueEx(hKey, KITEM_RasE_DEVCFG, 0, REG_BINARY, lpDeviceConfigIn, dwMaxEntry)==ERROR_SUCCESS )
			{
				fSuccess = FALSE;
			}
			RegCloseKey( hKey );
		}
	}
	//�豸��Ĭ������
	if( pszDrvReg )
	{
		if( RegCreateKeyEx( HKEY_HARDWARE_ROOT, pszDrvReg, 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, &dwDisposition )==ERROR_SUCCESS )
		{
			if( RegSetValueEx(hKey, ITEM_Drv_DEVCFG, 0, REG_BINARY, lpDeviceConfigIn, dwMaxEntry)==ERROR_SUCCESS )
			{
				fSuccess = FALSE;
			}
			RegCloseKey( hKey );
		}
	}

	return fSuccess;
}



// ********************************************************************
// ������
// ������
//	IN/OUT lpRasParams-���Ų������û�Ҫָ��szEntryName�����ܻ�ȡ��Ϣ
//	OUT lpfPassword-�Ƿ��ȡ����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡ ���Ų�����ָ����Ϣ�� RASPARAM���ݡ��Ƿ�õ�����
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryDialParams( LPCTSTR lpszPhonebook, IN OUT DIALPARAM* lpRasParams, OUT BOOL* lpfPassword )
{
	HKEY		hKey;
	DWORD		dwType;
	DWORD		dwSize;
	LONG		dwResult;

	//�������
	if( lpRasParams==NULL || lpfPassword==NULL || (lpRasParams->dwSize<sizeof(DIALPARAM)) )
	{
		return FALSE;
	}
	//׼��
	*lpfPassword = FALSE;
	*lpRasParams->szPhoneNumber = 0;
	*lpRasParams->szUserName = 0;
	*lpRasParams->szPassword = 0;
	*lpRasParams->szDomain = 0;
	//�� ������Ŀ
	if( !lpRasParams->szEntryName[0] )
	{
		//Ĭ�ϵ�
		if( !RAPI_GetEntryDef( lpszPhonebook, lpRasParams->szEntryName, RAS_MaxEntryName ) )
		{
			return FALSE;
		}
	}
	if( RasHlp_OpenKey( KROOT_RasE, KPATH_RasE, lpRasParams->szEntryName, &hKey, FALSE )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	//
	//dwType = REG_SZ;
	//dwSize = RAS_MaxPhoneNumber;
	//dwResult = RegQueryValueEx( hKey, KITEM_RasE_PHONE, 0, &dwType, (LPBYTE)lpRasParams->szPhoneNumber, &dwSize );
	//
	dwType = REG_SZ;
	dwSize = MAX_USERLEN;
	dwResult = RegQueryValueEx( hKey, KITEM_RasE_USER, 0, &dwType, (LPBYTE)lpRasParams->szUserName, &dwSize );
	//
	dwType = REG_SZ;
	dwSize = MAX_PWLEN;
	dwResult = RegQueryValueEx( hKey, KITEM_RasE_PW, 0, &dwType, (LPBYTE)lpRasParams->szPassword, &dwSize );
	if( dwResult==ERROR_SUCCESS )
	{
		*lpfPassword = TRUE;
	}
	//
	dwType = REG_SZ;
	dwSize = MAX_DNLEN;
	dwResult = RegQueryValueEx( hKey, KITEM_RasE_DOMAIN, 0, &dwType, (LPBYTE)lpRasParams->szDomain, &dwSize );
	
	RegCloseKey( hKey );
	return TRUE;
}

// ********************************************************************
// ������
// ������
//	IN lpRasParams-���Ų������û�Ҫָ��szEntryName������������Ϣ
//	IN lpfPassword-�Ƿ�ȥ������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// �������������� ���Ų�����ָ����Ϣ�� RASPARAM���ݡ��Ƿ�ȥ������
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryDialParams( LPCTSTR lpszPhonebook, DIALPARAM* lpRasParams, BOOL fRemovePassword )
{
	LONG		dwResult;
	HKEY		hKey;

	//�������
	if( lpRasParams==NULL )
	{
		return FALSE;
	}
	//
	if( RasHlp_OpenEntry( lpszPhonebook, lpRasParams->szEntryName, &hKey, FALSE )==ERROR_SUCCESS )
	{
		return FALSE;
	}
	//������Ŀ������
	//
	//dwResult = RegSetValueEx( hKey, KITEM_RasE_PHONE, 0, REG_SZ, (LPBYTE)lpRasParams->szPhoneNumber, RAS_MaxPhoneNumber );
	//
	dwResult = RegSetValueEx( hKey, KITEM_RasE_USER, 0, REG_SZ, (LPBYTE)lpRasParams->szUserName, MAX_USERLEN );
	//
	if( fRemovePassword )
	{
		RegDeleteValue( hKey, KITEM_RasE_PW );
	}
	else
	{
		dwResult = RegSetValueEx( hKey, KITEM_RasE_PW, 0, REG_SZ, (LPBYTE)lpRasParams->szPassword, MAX_PWLEN );
	}
	//
	dwResult = RegSetValueEx( hKey, KITEM_RasE_DOMAIN, 0, REG_SZ, (LPBYTE)lpRasParams->szDomain, MAX_DNLEN );
	
	RegCloseKey( hKey );
	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ����������ɾ�� ������Ŀ
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_DeleteEntry( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	TCHAR		szEntryPath[400+8];

	//ö�� ������Ŀ���Ƶĸ���
	ASSERT((strlen(KPATH_RasE)+strlen(pszEntryName))<=400 );
	if( pszEntryName )
	{
		sprintf( szEntryPath, "%s\\%s", KPATH_RasE, pszEntryName );
		return RegDeleteKey( KROOT_RasE, szEntryPath );
	}
	else
	{
		return FALSE;
	}
}

// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// �������������� ������Ŀ
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_RenameEntry( LPCTSTR lpszPhonebook, LPCTSTR pszOldEntry, LPCTSTR pszNewEntry )
{
	return FALSE;
}


// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ��������Ŀ����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// �����������ж� ������Ŀ�����Ƿ��зǷ��ַ�
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_IsValidEntryName( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	return FALSE;
}

// ********************************************************************
// ������
// ������
//	IN pszDrvReg-ָ���豸������ע���
//	OUT pdwDevType-ָ���豸���豸����
//	OUT pszTsp-ָ���豸��TSP����
//	OUT pszDeviceName-ָ���豸���豸����
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡָ���豸����Ϣ
// ����: 
// ********************************************************************
BOOL	WINAPI	RAPI_LookDriver( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;

	//�������
	if( !pszDrvReg )
	{
		return FALSE;
	}
	if( !pdwDevType && !pdwMTU && !pszTsp && !pszDeviceName )
	{
		return FALSE;
	}
	// ��ȡ pszDrvReg----TSP, DevType
	if( RegOpenKeyEx(HKEY_HARDWARE_ROOT, pszDrvReg, 0, KEY_ALL_ACCESS, &hKey)!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	if( pdwDevType )
	{
		memset( pdwDevType, 0, sizeof(DWORD) );
		dwDataType = REG_DWORD;
		dwMaxEntry = sizeof(DWORD);
		if( RegQueryValueEx(hKey, ITEM_Drv_DEVTYPE, 0, &dwDataType, (LPBYTE)pdwDevType, &dwMaxEntry)!=ERROR_SUCCESS )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
	}	
	if( pdwMTU )
	{
		memset( pdwMTU, 0, sizeof(DWORD) );
		dwDataType = REG_DWORD;
		dwMaxEntry = sizeof(DWORD);
		if( RegQueryValueEx(hKey, ITEM_Drv_MTU, 0, &dwDataType, (LPBYTE)pdwMTU, &dwMaxEntry)!=ERROR_SUCCESS )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
	}	
	if( pszTsp )
	{
		memset( pszTsp, 0, MAX_PATH );
		dwDataType = REG_SZ;
		dwMaxEntry = MAX_PATH;
		if( RegQueryValueEx(hKey, ITEM_Drv_TSP, 0, &dwDataType, (LPBYTE)pszTsp, &dwMaxEntry)!=ERROR_SUCCESS )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
		if( pszTsp[0]==0 )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
	}
	if( pszDeviceName )
	{
		memset( pszDeviceName, 0, MAX_PATH );
		dwDataType = REG_SZ;
		dwMaxEntry = RAS_MaxDeviceName;
		if( RegQueryValueEx(hKey, ITEM_Drv_DEVICENAME, 0, &dwDataType, (LPBYTE)pszDeviceName, &dwMaxEntry)!=ERROR_SUCCESS )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
		if( pszDeviceName[0]==0 )
		{
			strcpy( pszDeviceName, "unknown device name" );
		}
	}
	RegCloseKey( hKey );

	return TRUE;
}


// ********************************************************************
// ������
// ������
//	IN szEntryName-ָ������
//	OUT phkResult-���� �򿪲�����Ŀ��ע���
// ����ֵ��
//	�ɹ�������0;ʧ�ܣ����ط�0
// ������������ ������Ŀ��ע���
// ����: 
// ********************************************************************
LONG	RasHlp_OpenKey( HKEY hRoot, LPCTSTR pszPath, LPCTSTR pszKey, OUT HKEY* phkResult, BOOL fCreate )
{
	TCHAR		szEntryPath[400+8];
	DWORD		dwDisposition;
	LONG		nErr;

	ASSERT(pszPath);
	ASSERT( (strlen(pszPath)+strlen(pszKey))<=400 );
	sprintf( szEntryPath, "%s\\%s", pszPath, pszKey );
	if( fCreate )
	{
		 nErr = RegCreateKeyEx( hRoot, szEntryPath, 0, 0, 0, KEY_ALL_ACCESS, 0, phkResult, &dwDisposition );
	}
	else
	{
		 nErr = RegOpenKeyEx( hRoot, szEntryPath, 0, KEY_ALL_ACCESS, phkResult );
		//DWORD	dwErr;
		//if( (dwErr=RegOpenKeyEx( hRoot, szEntryPath, 0, KEY_DEFAULT_ACCESS, phkResult ))!=ERROR_SUCCESS )
		//{
		//	dwErr = GetLastError();
		//}
		//return dwErr;
	}
	//
	if( nErr!=ERROR_SUCCESS )
	{
		RETAILMSG(1,(TEXT("  RasHlp_OpenKey: Failed---[%d], [%s]!\r\n"), fCreate, szEntryPath));
	}

	return nErr;
}

BOOL	RasHlp_OpenEntry( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT HKEY* phkResult, BOOL fCreate )
{
	DWORD			dwResult;
	LPCTSTR			pszEntryName;
	RASENTRYNAME	EntryDefault;

	//
	*phkResult = NULL;
	//�õ�������Ŀ������
	if( !szEntryName || !*szEntryName )
	{
		//Ĭ�ϵ�
		if( !RAPI_GetEntryDef( lpszPhonebook, EntryDefault.szEntryName, RAS_MaxEntryName ) )
		{
			return FALSE;
		}
		pszEntryName = EntryDefault.szEntryName;
	}
	else
	{
		//ָ����
		pszEntryName = szEntryName;
	}
	//
	dwResult = RasHlp_OpenKey( KROOT_RasE, KPATH_RasE, pszEntryName, phkResult, fCreate );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

// ********************************************************************
// ������
// ������
//	IN pszActiveReg-ָ���豸��Activeע���
//	OUT pszDrvReg-ָ���豸������ע���
//	OUT pszOpenName-ָ���豸���豸������
// ����ֵ��
//	�ɹ�������TRUE;ʧ�ܣ�����FALSE
// ������������ȡָ���豸������ע�����豸������
// ����: 
// ********************************************************************
BOOL	RasD_LookActive( LPCTSTR pszActiveReg, OUT LPTSTR pszDrvReg, OUT LPTSTR pszOpenName, DWORD dwLenName )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;

	// ��ȡ pszActiveReg----szDrvReg , szOpenName
	if( RegOpenKeyEx(KROOT_Active, pszActiveReg, 0, KEY_ALL_ACCESS, &hKey)!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	memset( pszDrvReg, 0, RAS_MaxDrvReg );
	dwDataType = REG_SZ;
	dwMaxEntry = RAS_MaxDrvReg;
	if( RegQueryValueEx(hKey, ITEM_Active_DRVPATH, 0, &dwDataType, (LPBYTE)pszDrvReg, &dwMaxEntry)!=ERROR_SUCCESS )
	{
		RegCloseKey( hKey );
		return FALSE;
	}
	if( pszOpenName )
	{
		memset( pszOpenName, 0, dwLenName );
		dwDataType = REG_SZ;
		dwMaxEntry = dwLenName;
		if( RegQueryValueEx(hKey, ITEM_Active_NAME, 0, &dwDataType, (LPBYTE)pszOpenName, &dwMaxEntry)!=ERROR_SUCCESS )
		{
			RegCloseKey( hKey );
			return FALSE;
		}
	}
	RegCloseKey( hKey );
	return TRUE;
}

