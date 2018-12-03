/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：GPRS Config API
版本号：  1.0.0
开发时期：2004-08-24
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <socket.h>
#include <gprscfg.h>
//"\inc_local"
#include "netsrvreg.h"


/***************  全局区 定义， 声明 *****************/

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
	//打开
	if( RegOpenKeyEx( KROOT_GprsCfg, KPATH_GprsCfg, 0, KEY_ALL_ACCESS, &hKey )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	//获取
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

	//参数检查
	if( (dwNetworkType!=GPRS_NETTYPE_INTERNET) && (dwNetworkType!=GPRS_NETTYPE_WAP) )
	{
		return FALSE;
	}
	//打开
	if( RegCreateKeyEx( KROOT_GprsCfg, KPATH_GprsCfg, 0, 0, 0, KEY_ALL_ACCESS, 0, &hKey, &dwDisposition )!=ERROR_SUCCESS )
	{
		return FALSE;
	}
	//设置
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
	
	//打开
	if( !GprsCfg_OpenReg(dwNetworkType, FALSE, &hKey, NULL) )
	{
		return FALSE;
	}
	//获取
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
	
	//打开
	if( !GprsCfg_OpenReg(dwNetworkType, TRUE, &hKey, NULL) )
	{
		return FALSE;
	}
	//设置
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
	
	//打开
	if( !GprsCfg_OpenReg(dwNetworkType, FALSE, &hKey, NULL) )
	{
		return FALSE;
	}
	//获取
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
	
	//打开
	if( !GprsCfg_OpenReg(dwNetworkType, TRUE, &hKey, NULL) )
	{
		return FALSE;
	}
	//设置
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

	//打开
	if( !GprsCfg_OpenReg(dwNetworkType, FALSE, &hKey, &pCfgGprs->dwNetworkType) )
	{
		return FALSE;
	}
	//获取
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

	//打开
	if( !GprsCfg_OpenReg(pCfgGprs->dwNetworkType, TRUE, &hKey, &pCfgGprs->dwNetworkType) )
	{
		return FALSE;
	}
	//设置
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
	
	//打开
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

