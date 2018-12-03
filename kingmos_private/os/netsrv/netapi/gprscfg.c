/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����GPRS Config API
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-08-24
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <socket.h>
#include <gprscfg.h>
//"\inc_local"
#include "netsrvreg.h"


/***************  ȫ���� ���壬 ���� *****************/

static	BOOL	GprsCfg_OpenReg( DWORD dwNetworkType, BOOL fCreate, OUT HKEY* phKey, OUT DWORD* pdwNetworkType );

/******************************************************/

BOOL	GprsCfg_GetCurNetType( OUT DWORD* pdwNetworkType )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwLen;

	//
	//*pdwNetworkType = GPRS_NETTYPE_UNKNOWN;
	//��
	if( RegOpenKeyEx( KROOT_GprsCfg, KPATH_GprsCfg, 0, KEY_ALL_ACCESS, &hKey )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	//��ȡ
	dwDataType = REG_DWORD;
	dwLen = sizeof(DWORD);
	dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_CurType, 0, &dwDataType, (LPBYTE)pdwNetworkType, &dwLen);
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

BOOL	GprsCfg_SetCurNetType( DWORD dwNetworkType )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDisposition;

	//�������
	if( (dwNetworkType!=GPRS_NETTYPE_INTERNET) && (dwNetworkType!=GPRS_NETTYPE_WAP) )
	{
		return FALSE;
	}
	//��
	if( RegCreateKeyEx( KROOT_GprsCfg, KPATH_GprsCfg, 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, &dwDisposition )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	//����
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_CurType, 0, REG_DWORD, (LPBYTE)&dwNetworkType, sizeof(DWORD) );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


BOOL	GprsCfg_GetApn( DWORD dwNetworkType, OUT LPSTR pszApn, DWORD dwLen )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	
	//��
	if( !GprsCfg_OpenReg(dwNetworkType, FALSE, &hKey, NULL) )
	{
		return FALSE;
	}
	//��ȡ
	dwDataType = REG_SZ;
	dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_APN, 0, &dwDataType, (LPBYTE)pszApn, &dwLen);
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

BOOL	GprsCfg_SetApn( DWORD dwNetworkType, LPSTR pszApn )
{
	DWORD		dwResult;
	HKEY		hKey;
	
	//��
	if( !GprsCfg_OpenReg(dwNetworkType, TRUE, &hKey, NULL) )
	{
		return FALSE;
	}
	//����
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_APN, 0, REG_SZ, (LPBYTE)pszApn, strlen(pszApn)+sizeof(TCHAR) );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


BOOL	GprsCfg_GetPPG( DWORD dwNetworkType, OUT DWORD* pdwIP_PPG, OUT WORD* pwPort_PPG )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwLen;
	char		pszPPG[32];
	
	//��
	if( !GprsCfg_OpenReg(dwNetworkType, FALSE, &hKey, NULL) )
	{
		return FALSE;
	}
	//��ȡ
	if( pdwIP_PPG )
	{
		dwDataType = REG_SZ;
		dwLen = sizeof(pszPPG);
		memset( pszPPG, 0, sizeof(pszPPG) );
		dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_PPG_IP, 0, &dwDataType, (LPBYTE)pszPPG, &dwLen);
		*pdwIP_PPG = inet_addr( pszPPG );
	}
	//
	if( pwPort_PPG )
	{
		dwDataType = REG_SZ;
		dwLen = sizeof(pszPPG);
		memset( pszPPG, 0, sizeof(pszPPG) );
		dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_PPG_Port, 0, &dwDataType, (LPBYTE)pszPPG, &dwLen);
		*pwPort_PPG = (WORD)atoi( pszPPG );
	}
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

BOOL	GprsCfg_SetPPG( DWORD dwNetworkType, DWORD dwIP_PPG, WORD wPort_PPG )
{
	DWORD		dwResult;
	HKEY		hKey;
	char		pszPPG[32];
	
	//��
	if( !GprsCfg_OpenReg(dwNetworkType, TRUE, &hKey, NULL) )
	{
		return FALSE;
	}
	//����
	inet_ntoa_ip( (LPBYTE)&dwIP_PPG, pszPPG );
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_PPG_IP, 0, REG_SZ, (LPBYTE)pszPPG, strlen(pszPPG)+sizeof(char) );
	//
	sprintf( pszPPG, "%s", wPort_PPG );
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_PPG_Port, 0, REG_SZ, (LPBYTE)pszPPG, strlen(pszPPG)+sizeof(char) );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


BOOL	GprsCfg_GetAll( DWORD dwNetworkType, OUT GPRS_CFG* pCfgGprs )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwLen;
	char		pszPPG[32];

	//��
	if( !GprsCfg_OpenReg(dwNetworkType, FALSE, &hKey, &pCfgGprs->dwNetworkType) )
	{
		return FALSE;
	}
	//��ȡ
	//
	dwDataType = REG_SZ;
	dwLen = LEN_GPRSAPN;
	dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_APN, 0, &dwDataType, (LPBYTE)pCfgGprs->szApn, &dwLen);
	//
	dwDataType = REG_SZ;
	dwLen = sizeof(pszPPG);
	memset( pszPPG, 0, sizeof(pszPPG) );
	dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_PPG_IP, 0, &dwDataType, (LPBYTE)pszPPG, &dwLen);
	pCfgGprs->dwIP_PPG = inet_addr( pszPPG );
	//
	dwDataType = REG_SZ;
	dwLen = sizeof(pszPPG);
	memset( pszPPG, 0, sizeof(pszPPG) );
	dwResult = RegQueryValueEx(hKey, KITEM_GprsCfg_PPG_Port, 0, &dwDataType, (LPBYTE)pszPPG, &dwLen);
	pCfgGprs->wPort_PPG = (WORD)atoi( pszPPG );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

BOOL	GprsCfg_SetAll( GPRS_CFG* pCfgGprs )
{
	DWORD		dwResult;
	HKEY		hKey;
	char		pszPPG[32];

	//��
	if( !GprsCfg_OpenReg(pCfgGprs->dwNetworkType, TRUE, &hKey, &pCfgGprs->dwNetworkType) )
	{
		return FALSE;
	}
	//����
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_APN, 0, REG_SZ, (LPBYTE)pCfgGprs->szApn, strlen(pCfgGprs->szApn)+sizeof(char) );
	//
	inet_ntoa_ip( (LPBYTE)&pCfgGprs->dwIP_PPG, pszPPG );
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_PPG_IP, 0, REG_SZ, (LPBYTE)pszPPG, strlen(pszPPG)+sizeof(char) );
	//
	sprintf( pszPPG, "%s", pCfgGprs->wPort_PPG );
	dwResult = RegSetValueEx(hKey, KITEM_GprsCfg_PPG_Port, 0, REG_SZ, (LPBYTE)pszPPG, strlen(pszPPG)+sizeof(char) );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
	//
}

BOOL	GprsCfg_OpenReg( DWORD dwNetworkType, BOOL fCreate, OUT HKEY* phKey, OUT DWORD* pdwNetworkType )
{
	LPCTSTR		pszSubKey;
	DWORD		dwDisposition;
	
	//��
	if( dwNetworkType==GPRS_NETTYPE_CURRENT )
	{
		if( !GprsCfg_GetCurNetType( &dwNetworkType ) )
		{
			dwNetworkType = GPRS_NETTYPE_INTERNET;
		}
	}
	if( dwNetworkType==GPRS_NETTYPE_INTERNET )
	{
		pszSubKey = KPATH_GprsCfg_Internet;
	}
	else if( dwNetworkType==GPRS_NETTYPE_WAP )
	{
		pszSubKey = KPATH_GprsCfg_Wap;
	}
	else
	{
		return FALSE;
	}
	if( pdwNetworkType )
	{
		*pdwNetworkType = dwNetworkType;
	}
	if( fCreate )
	{
		if( RegCreateKeyEx( KROOT_GprsCfg, pszSubKey, 0, 0, 0, KEY_ALL_ACCESS, 0, phKey, &dwDisposition )!=ERROR_SUCCESS )
		{
			return FALSE;
		}
	}
	else
	{
		if( RegOpenKeyEx( KROOT_GprsCfg, pszSubKey, 0, KEY_ALL_ACCESS, phKey )!=ERROR_SUCCESS )
		{
			return FALSE;
		}
	}

	return TRUE;
}

