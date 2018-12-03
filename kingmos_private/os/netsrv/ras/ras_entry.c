/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：管理 RAS注册表信息---条目、模式、驱动信息
版本号：  1.0.0
开发时期：2004-02-26
作者：    肖远钢
修改记录：
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


/***************  全局区 定义， 声明 *****************/

static	LONG	RasHlp_OpenKey( HKEY hRoot, LPCTSTR pszPath, LPCTSTR pszKey, OUT HKEY* phkResult, BOOL fCreate );
static	BOOL	RasHlp_OpenEntry( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT HKEY* phkResult, BOOL fCreate );

/******************************************************/


// ========================================================================================== //
// 拨号模式.................................................................................. //
// ========================================================================================== //

// ********************************************************************
// 声明：
// 参数：
//	OUT lpDialMthodName-保存获取到的拨号模式名称的数组
//	IN/OUT pdwCnt-指定拨号模式名称的数组的大小，并保存获取到的个数
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：枚举 拨号模式名称的个数，或者枚举所有的拨号模式名称。取决于 lpDialMthodName是否为NULL
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_EnumDialMethod( OUT OPTIONAL LPDIALM_NAME lpDialMthodName, IN OUT DWORD* pdwCnt )
{
	HKEY		hKey;
	DWORD		dwSubKeys;
	DWORD		dwMaxEntries;
	DWORD		i;

	//参数检查
	if( !pdwCnt )
	{
		return FALSE;
	}
	//枚举 拨号模式名称的个数
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
		//枚举 所有的拨号模式名称
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
// 声明：
// 参数：
//	IN szDialName-拨号模式名称
//	IN dwOptMethod-获取拨号模式名称的选项
//	OUT lpDialMthod-获取指定或者缺省的拨号模式名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取指定或者缺省的拨号模式名称
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetDialMethod( LPCTSTR szDialName, DWORD dwOptMethod, OUT LPDIAL_METHOD lpDialMthod )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;
	LPCTSTR		pszName;

	//参数检查
	if( !lpDialMthod )
	{
		return FALSE;
	}
	//获取 拨号模式的名称
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
// 声明：
// 参数：
//	IN lpEntry-指定拨号条目的名称
//	IN pszLocal-本地的电话
//	OUT pszPhoneNum-获取的最终电话号码
//	IN dwLen-指定pszPhoneNum的长度
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取指指定拨号条目的所确定的最终电话号码
// 引用: 
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

	//参数检查
	if( !lpEntry || dwLen<4 )
	{
		return FALSE;
	}
	//确定用户选择的拨号模式
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
	//获取拨号模式信息
	if( !RAPI_GetDialMethod( lpEntry->szDialName, dwOptMethod, &stDialM ) )
	{
		return FALSE;
	}

	//准备填充
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

	//填充 音频方式或者 脉冲方式
#if 0
	if( stDialM.szDial[0] )
	{
		*pszNumTmp++ = stDialM.szDial[0];
		dwBytes --;
	}
#endif
	//填充号码
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
// 拨号条目.................................................................................. //
// ========================================================================================== //


// ********************************************************************
// 声明：
// 参数：
//	OUT lpEntryNames-保存获取到的拨号条目名称的数组
//	IN/OUT lpdwCntEntries-指定拨号条目名称的数组的大小，并保存获取到的个数
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：枚举 拨号条目名称的个数，或者枚举所有的拨号条目名称。取决于 pszEntryName是否为NULL
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_EnumEntries( LPCTSTR lpszPhonebook, OUT RASENTRYNAME* lpEntryNames, IN OUT DWORD* lpdwCntEntries )
{
	HKEY		hKey;
	DWORD		dwSubKeys;
	DWORD		dwMaxEntries;
	DWORD		i;

	//参数检查
	if( !lpdwCntEntries )
	{
		return FALSE;
	}
	//枚举 拨号条目名称的个数
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
		//枚举 所有的拨号条目名称
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
// 声明：
// 参数：
//	OUT pszEntryName-指定拨号条目名称
//	IN/OUT pdwSize-指定 pszEntryName的大小，并保存获取到的大小
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取 缺省拨号条目的名称
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryDef( LPCTSTR lpszPhonebook, OUT LPTSTR pszEntryName, DWORD dwLen )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	
	//读取 Default 的拨号条目的名称
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
// 声明：
// 参数：
//	IN pszEntryName-指定拨号条目名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：设置 缺省拨号条目的名称
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryDef( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDisposition;
	
	//读取 Default 的拨号条目的名称
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
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
//	OUT lpEntry-如果不为NULL，则获取 RASENTRY内容
//	IN/OUT lpdwEntrySize-指定 lpEntry的大小，并保存获取到的大小
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取 指定或者缺省拨号条目的指定信息
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT RASENTRY* lpEntry, IN OUT LPDWORD lpdwEntrySize )
{
	DWORD		dwResult;
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;
	
	//参数检查
	if( !lpEntry || !lpdwEntrySize )
	{
		*lpdwEntrySize = 0;
		return FALSE;
	}
	//打开 拨号条目
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
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
//	IN lpEntry-如果不为NULL，则获取 RASENTRY内容
//	IN dwEntrySize-指定 lpEntry的大小
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：设置 拨号条目的指定信息，或者新建1个拨号条目
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, RASENTRY* lpEntry, DWORD dwEntrySize )
{
	DWORD		dwResult;
	HKEY		hKey;

	//参数检查
	if( !lpEntry || !dwEntrySize )
	{
		return FALSE;
	}
	//打开 拨号条目
	if( !RasHlp_OpenEntry( lpszPhonebook, szEntryName, &hKey, TRUE ) )
	{
		return FALSE;
	}
	//设置
	dwResult = RegSetValueEx( hKey, KITEM_RasE_ENTRY, 0, REG_BINARY, (LPBYTE)lpEntry, dwEntrySize );
	//
	RegCloseKey( hKey );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}


// ********************************************************************
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
//	OUT lpDeviceConfigOut-如果不为NULL，则获取 RASENTRY内容
//	IN/OUT lpdwSize-指定 lpDeviceConfigOut的大小，和获取配置后的长度
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取条目的配置，或者是设备的默认配置
// 引用: 
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
	//获取条目的配置
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
	//设备的默认配置
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
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
//	IN lpDeviceConfigIn-如果不为NULL，则获取 RASENTRY内容
//	IN dwSize-指定 lpDeviceConfigIn的大小
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取条目的配置，或者是设备的默认配置
// 引用: 
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
	//条目的配置
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
	//设备的默认配置
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
// 声明：
// 参数：
//	IN/OUT lpRasParams-拨号参数，用户要指定szEntryName，才能获取信息
//	OUT lpfPassword-是否获取密码
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取 拨号参数的指定信息： RASPARAM内容、是否得到密码
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_GetEntryDialParams( LPCTSTR lpszPhonebook, IN OUT DIALPARAM* lpRasParams, OUT BOOL* lpfPassword )
{
	HKEY		hKey;
	DWORD		dwType;
	DWORD		dwSize;
	LONG		dwResult;

	//参数检查
	if( lpRasParams==NULL || lpfPassword==NULL || (lpRasParams->dwSize<sizeof(DIALPARAM)) )
	{
		return FALSE;
	}
	//准备
	*lpfPassword = FALSE;
	*lpRasParams->szPhoneNumber = 0;
	*lpRasParams->szUserName = 0;
	*lpRasParams->szPassword = 0;
	*lpRasParams->szDomain = 0;
	//打开 拨号条目
	if( !lpRasParams->szEntryName[0] )
	{
		//默认的
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
// 声明：
// 参数：
//	IN lpRasParams-拨号参数，用户要指定szEntryName，才能设置信息
//	IN lpfPassword-是否去掉密码
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：设置 拨号参数的指定信息： RASPARAM内容、是否去掉密码
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_SetEntryDialParams( LPCTSTR lpszPhonebook, DIALPARAM* lpRasParams, BOOL fRemovePassword )
{
	LONG		dwResult;
	HKEY		hKey;

	//参数检查
	if( lpRasParams==NULL )
	{
		return FALSE;
	}
	//
	if( RasHlp_OpenEntry( lpszPhonebook, lpRasParams->szEntryName, &hKey, FALSE )==ERROR_SUCCESS )
	{
		return FALSE;
	}
	//设置条目的配置
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
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：删除 拨号条目
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_DeleteEntry( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	TCHAR		szEntryPath[400+8];

	//枚举 拨号条目名称的个数
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
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：改名 拨号条目
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_RenameEntry( LPCTSTR lpszPhonebook, LPCTSTR pszOldEntry, LPCTSTR pszNewEntry )
{
	return FALSE;
}


// ********************************************************************
// 声明：
// 参数：
//	IN szEntryName-指定拨号条目名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：判断 拨号条目名称是否有非法字符
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_IsValidEntryName( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	return FALSE;
}

// ********************************************************************
// 声明：
// 参数：
//	IN pszDrvReg-指定设备的驱动注册表
//	OUT pdwDevType-指定设备的设备类型
//	OUT pszTsp-指定设备的TSP名称
//	OUT pszDeviceName-指定设备的设备名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取指定设备的信息
// 引用: 
// ********************************************************************
BOOL	WINAPI	RAPI_LookDriver( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;

	//参数检查
	if( !pszDrvReg )
	{
		return FALSE;
	}
	if( !pdwDevType && !pdwMTU && !pszTsp && !pszDeviceName )
	{
		return FALSE;
	}
	// 获取 pszDrvReg----TSP, DevType
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
// 声明：
// 参数：
//	IN szEntryName-指定名称
//	OUT phkResult-保存 打开拨号条目的注册键
// 返回值：
//	成功，返回0;失败，返回非0
// 功能描述：打开 拨号条目的注册键
// 引用: 
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
	//得到拨号条目的名称
	if( !szEntryName || !*szEntryName )
	{
		//默认的
		if( !RAPI_GetEntryDef( lpszPhonebook, EntryDefault.szEntryName, RAS_MaxEntryName ) )
		{
			return FALSE;
		}
		pszEntryName = EntryDefault.szEntryName;
	}
	else
	{
		//指定的
		pszEntryName = szEntryName;
	}
	//
	dwResult = RasHlp_OpenKey( KROOT_RasE, KPATH_RasE, pszEntryName, phkResult, fCreate );
	return (dwResult==ERROR_SUCCESS?TRUE:FALSE);
}

// ********************************************************************
// 声明：
// 参数：
//	IN pszActiveReg-指定设备的Active注册表
//	OUT pszDrvReg-指定设备的驱动注册表
//	OUT pszOpenName-指定设备的设备打开名称
// 返回值：
//	成功，返回TRUE;失败，返回FALSE
// 功能描述：获取指定设备的驱动注册表和设备打开名称
// 引用: 
// ********************************************************************
BOOL	RasD_LookActive( LPCTSTR pszActiveReg, OUT LPTSTR pszDrvReg, OUT LPTSTR pszOpenName, DWORD dwLenName )
{
	HKEY		hKey;
	DWORD		dwDataType;
	DWORD		dwMaxEntry;

	// 获取 pszActiveReg----szDrvReg , szOpenName
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

