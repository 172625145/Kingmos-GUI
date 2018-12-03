/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：RAS API
版本号：  2.0.0
开发时期：2004-05-09
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
#include <apiexe.h>
//"\inc_app"
#include <ras.h>


/***************  全局区 定义， 声明 *****************/

enum
{
// RAS拨号模式
	  ERAPI_EnumDialMethod = 1
	, ERAPI_GetDialMethod
	, ERAPI_GenNum

// RAS拨号条目
	, ERAPI_EnumEntries
	, ERAPI_GetEntryDef
	, ERAPI_SetEntryDef
	, ERAPI_DeleteEntry
	, ERAPI_RenameEntry
	, ERAPI_IsValidEntryName

	, ERAPI_GetEntryProperties
	, ERAPI_SetEntryProperties

	, ERAPI_GetEntryDialParams
	, ERAPI_SetEntryDialParams

	, ERAPI_GetEntryDevConfig
	, ERAPI_SetEntryDevConfig

// RAS拨号设备
	, ERAPI_EnumDevices
	, ERAPI_LookDriver
	, ERAPI_CheckDev

// RAS拨号连接
	, ERAPI_Dial
	, ERAPI_HangUp
	, ERAPI_EnumConnections

	, ERAPI_GetConnectInfo
	, ERAPI_GetConnectStatus

	, ERAPI_EnableStat
	, ERAPI_GetLinkStatistics

// RAS拨号辅助操作
	, ERAPI_GetStateStr
	, ERAPI_GetErrStr

// RAS拨号modem
	, ERAPI_RegisterModem
	, ERAPI_DeregisterModem
	, ERAPI_RegisterPPPoEd
	, ERAPI_DeregisterPPPoEd

};

// RAS拨号模式
typedef	BOOL	(WINAPI* PRAPI_EnumDialMethod)( OUT LPDIALM_NAME lpDialMthodName, IN OUT DWORD* pdwCnt );
typedef	BOOL	(WINAPI* PRAPI_GetDialMethod)( LPCTSTR szDialName, DWORD dwOption, OUT LPDIAL_METHOD lpDialMthod );
typedef	BOOL	(WINAPI* PRAPI_GenNum)( RASENTRY* lpEntry, OPTIONAL LPCSTR pszNumLocal, OUT LPSTR pszNumNew, DWORD dwLen );

// RAS拨号条目
typedef	BOOL	(WINAPI* PRAPI_EnumEntries)( LPCTSTR lpszPhonebook, OUT RASENTRYNAME* lpEntryNames, IN OUT DWORD* lpdwCntEntries );
typedef	BOOL	(WINAPI* PRAPI_GetEntryDef)( LPCTSTR lpszPhonebook, OUT LPTSTR pszEntryName, DWORD dwLen );
typedef	BOOL	(WINAPI* PRAPI_SetEntryDef)( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );
typedef	BOOL	(WINAPI* PRAPI_DeleteEntry)( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );
typedef	BOOL	(WINAPI* PRAPI_RenameEntry)( LPCTSTR lpszPhonebook, LPCTSTR pszOldEntry, LPCTSTR pszNewEntry );
typedef	BOOL	(WINAPI* PRAPI_IsValidEntryName)( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );

typedef	BOOL	(WINAPI* PRAPI_GetEntryProperties)( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT RASENTRY* lpEntry, IN OUT LPDWORD lpdwEntrySize );
typedef	BOOL	(WINAPI* PRAPI_SetEntryProperties)( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, RASENTRY* lpEntry, DWORD dwEntrySize );

typedef	BOOL	(WINAPI* PRAPI_GetEntryDialParams)( LPCTSTR lpszPhonebook, IN OUT DIALPARAM* lpDialParams, OUT BOOL* lpfPassword );
typedef	BOOL	(WINAPI* PRAPI_SetEntryDialParams)( LPCTSTR lpszPhonebook, DIALPARAM* lpDialParams, BOOL fRemovePassword );

typedef	BOOL	(WINAPI* PRAPI_GetEntryDevConfig)( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, OUT LPVOID lpDeviceConfigOut, IN OUT DWORD* lpdwSize );
typedef	BOOL	(WINAPI* PRAPI_SetEntryDevConfig)( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, LPVOID lpDeviceConfigIn, DWORD dwSize );

// RAS拨号设备
typedef	void	(WINAPI* PRAPI_EnumDevices)( OUT RASDEVINFO* lpRasDevinfo, IN OUT DWORD* lpdwCntDevices );
typedef	BOOL	(WINAPI* PRAPI_LookDriver)( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName );
typedef	BOOL	(WINAPI* PRAPI_CheckDev)( DWORD dwChkFlag, DWORD dwChkDevType );

// RAS拨号连接
typedef	DWORD	(WINAPI* PRAPI_Dial)( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn );
typedef	DWORD	(WINAPI* PRAPI_HangUp)( HANDLE hRasConn );
typedef	DWORD	(WINAPI* PRAPI_EnumConnections)( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn );

typedef	DWORD	(WINAPI* PRAPI_GetConnectInfo)( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo );
typedef	DWORD	(WINAPI* PRAPI_GetConnectStatus)( HANDLE hRasConn, OUT DWORD* pdwDevStatus );

typedef	DWORD	(WINAPI* PRAPI_EnableStat)( HANDLE hRasConn, BOOL fEnbaleBps );
typedef	DWORD	(WINAPI* PRAPI_GetLinkStatistics)( HANDLE hRasConn, OUT RAS_STATS *lpStatistics );

// RAS拨号辅助操作
typedef	DWORD	(WINAPI* PRAPI_GetStateStr)( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize );
typedef	DWORD	(WINAPI* PRAPI_GetErrStr)( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize );

// RAS拨号设备的注册与注销
typedef	BOOL	(WINAPI* PRAPI_RegisterModem)( LPCTSTR pszActiveReg );
typedef	void	(WINAPI* PRAPI_DeregisterModem)( LPCTSTR pszActiveReg );
typedef	BOOL	(WINAPI* PRAPI_RegisterPPPoEd)( LPCTSTR pszDrvReg, HANDLE hDev );
typedef	void	(WINAPI* PRAPI_DeregisterPPPoEd)( LPCTSTR pszDrvReg );


/******************************************************/


// RAS拨号模式
BOOL	WINAPI	RasMethod_Enum( OUT OPTIONAL LPDIALM_NAME lpDialMthodName, IN OUT DWORD* pdwCnt )
{
	EXE_API2( API_RAS, ERAPI_EnumDialMethod, PRAPI_EnumDialMethod, BOOL, lpDialMthodName, pdwCnt );
}
BOOL	WINAPI	RasMethod_Get( LPCTSTR szDialName, DWORD dwOption, OUT LPDIAL_METHOD lpDialMthod )
{
	EXE_API3( API_RAS, ERAPI_GetDialMethod, PRAPI_GetDialMethod, BOOL, szDialName, dwOption, lpDialMthod );
}
BOOL	WINAPI	RasMethod_GenNum( RASENTRY* lpEntry, OPTIONAL LPCSTR pszNumLocal, OUT LPSTR pszNumNew, DWORD dwLen )
{
	EXE_API4( API_RAS, ERAPI_GenNum, PRAPI_GenNum, BOOL, lpEntry, pszNumLocal, pszNumNew, dwLen );
}



// RAS拨号条目
BOOL	WINAPI	RasEntry_Enum( LPCTSTR lpszPhonebook, OUT RASENTRYNAME* lpEntryNames, IN OUT DWORD* lpdwCntEntries )
{
	EXE_API3( API_RAS, ERAPI_EnumEntries, PRAPI_EnumEntries, BOOL, lpszPhonebook, lpEntryNames, lpdwCntEntries );
}
BOOL	WINAPI	RasEntry_GetDef( LPCTSTR lpszPhonebook, OUT LPTSTR pszEntryName, DWORD dwLen )
{
	EXE_API3( API_RAS, ERAPI_GetEntryDef, PRAPI_GetEntryDef, BOOL, lpszPhonebook, pszEntryName, dwLen );
}
BOOL	WINAPI	RasEntry_SetDef( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	EXE_API2( API_RAS, ERAPI_SetEntryDef, PRAPI_SetEntryDef, BOOL, lpszPhonebook, pszEntryName );
}
BOOL	WINAPI	RasEntry_Delete( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	EXE_API2( API_RAS, ERAPI_DeleteEntry, PRAPI_DeleteEntry, BOOL, lpszPhonebook, pszEntryName );
}
BOOL	WINAPI	RasEntry_Rename( LPCTSTR lpszPhonebook, LPCTSTR pszOldEntry, LPCTSTR pszNewEntry )
{
	EXE_API3( API_RAS, ERAPI_RenameEntry, PRAPI_RenameEntry, BOOL, lpszPhonebook, pszOldEntry,pszNewEntry  );
}
BOOL	WINAPI	RasEntry_IsValid( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName )
{
	EXE_API2( API_RAS, ERAPI_IsValidEntryName, PRAPI_IsValidEntryName, BOOL, lpszPhonebook, pszEntryName );
}

BOOL	WINAPI	RasEntry_GetProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT RASENTRY* lpEntry, IN OUT LPDWORD lpdwEntrySize )
{
	EXE_API4( API_RAS, ERAPI_GetEntryProperties, PRAPI_GetEntryProperties, BOOL, lpszPhonebook, szEntryName, lpEntry, lpdwEntrySize );
}
BOOL	WINAPI	RasEntry_SetProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, RASENTRY* lpEntry, DWORD dwEntrySize )
{
	EXE_API4( API_RAS, ERAPI_SetEntryProperties, PRAPI_SetEntryProperties, BOOL, lpszPhonebook, szEntryName, lpEntry, dwEntrySize );
}

BOOL	WINAPI	RasEntry_GetDialParams( LPCTSTR lpszPhonebook, IN OUT DIALPARAM* lpDialParams, OUT BOOL* lpfPassword )
{
	EXE_API3( API_RAS, ERAPI_GetEntryDialParams, PRAPI_GetEntryDialParams, BOOL, lpszPhonebook, lpDialParams, lpfPassword );
}
BOOL	WINAPI	RasEntry_SetDialParams( LPCTSTR lpszPhonebook, DIALPARAM* lpDialParams, BOOL fRemovePassword )
{
	EXE_API3( API_RAS, ERAPI_SetEntryDialParams, PRAPI_SetEntryDialParams, BOOL, lpszPhonebook, lpDialParams, fRemovePassword );
}

BOOL	WINAPI	RasEntry_GetDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, OUT LPVOID lpDeviceConfigOut, IN OUT DWORD* lpdwSize )
{
	EXE_API5( API_RAS, ERAPI_GetEntryDevConfig, PRAPI_GetEntryDevConfig, BOOL, lpszPhonebook, szEntryName, pszDrvReg, lpDeviceConfigOut, lpdwSize );
}
BOOL	WINAPI	RasEntry_SetDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, LPVOID lpDeviceConfigIn, DWORD dwSize )
{
	EXE_API5( API_RAS, ERAPI_SetEntryDevConfig, PRAPI_SetEntryDevConfig, BOOL, lpszPhonebook, szEntryName, pszDrvReg, lpDeviceConfigIn, dwSize );
}



// RAS拨号设备
void	WINAPI	RasDev_Enum( OUT RASDEVINFO* lpRasDevinfo, IN OUT DWORD* lpdwCntDevices )
{
	EXE_API2_VOID( API_RAS, ERAPI_EnumDevices, PRAPI_EnumDevices, lpRasDevinfo, lpdwCntDevices );
}
BOOL	WINAPI	RasDev_GetDrvDial( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName )
{
	EXE_API5( API_RAS, ERAPI_LookDriver, PRAPI_LookDriver, BOOL, pszDrvReg, pdwDevType, pdwMTU, pszTsp, pszDeviceName );
}
BOOL	WINAPI	RasDev_Check( DWORD dwChkFlag, DWORD dwChkDevType )
{
	EXE_API2( API_RAS, ERAPI_CheckDev, PRAPI_CheckDev, BOOL, dwChkFlag, dwChkDevType );
}



// RAS拨号连接
DWORD	WINAPI	RasLink_Dial( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn )
{
	EXE_API5( API_RAS, ERAPI_Dial, PRAPI_Dial, DWORD, lpszPhonebook, pDialParam, dwNotifierType, notifier, phRasConn );
}
DWORD	WINAPI	RasLink_HangUp( HANDLE hRasConn )
{
	EXE_API1( API_RAS, ERAPI_HangUp, PRAPI_HangUp, DWORD, hRasConn );
}
DWORD	WINAPI	RasLink_Enum( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn )
{
	EXE_API2( API_RAS, ERAPI_EnumConnections, PRAPI_EnumConnections, DWORD, phRasConn, lpdwCntRasConn );
}

DWORD	WINAPI	Ras_RasGetConnectInfo( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo )
{
	EXE_API2( API_RAS, ERAPI_GetConnectInfo, PRAPI_GetConnectInfo, DWORD, hRasConn, lpRasConnInfo );
}
DWORD	WINAPI	Ras_RasGetConnectStatus( HANDLE hRasConn, OUT DWORD* pdwDevStatus )
{
	EXE_API2( API_RAS, ERAPI_GetConnectStatus, PRAPI_GetConnectStatus, DWORD, hRasConn, pdwDevStatus );
}

DWORD	WINAPI	RasLink_EnableStat( HANDLE hRasConn, BOOL fEnbaleBps )
{
	EXE_API2( API_RAS, ERAPI_EnableStat, PRAPI_EnableStat, DWORD, hRasConn, fEnbaleBps );
}
DWORD	WINAPI	RasLink_GetStat( HANDLE hRasConn, OUT RAS_STATS *lpStatistics )
{
	EXE_API2( API_RAS, ERAPI_GetLinkStatistics, PRAPI_GetLinkStatistics, DWORD, hRasConn, lpStatistics );
}

// RAS拨号辅助操作
DWORD	WINAPI	RasFun_GetStateStr( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize )
{
	EXE_API3( API_RAS, ERAPI_GetStateStr, PRAPI_GetStateStr, DWORD, dwRasState, pszStateStr, dwBufSize );
}
DWORD	WINAPI	RasFun_GetErrStr( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize )
{
	EXE_API3( API_RAS, ERAPI_GetErrStr, PRAPI_GetErrStr, DWORD, dwRasErr, pszErrStr, dwBufSize );
}



// RAS拨号modem
BOOL	WINAPI	RasMdm_RegModem( LPCTSTR pszActiveReg )
{
	EXE_API1( API_RAS, ERAPI_RegisterModem, PRAPI_RegisterModem, BOOL, pszActiveReg );
#if 0
    CALLSTACK	cs;
	PRAPI_RegisterModem	pFnExe;
	BOOL		iRet;
	if( API_Enter( API_RAS, ERAPI_RegisterModem, &pFnExe, &cs ) )
	{
		iRet = pFnExe( pszActiveReg );
		API_Leave( );
	}
	return iRet;
#endif
}
void	WINAPI	RasMdm_UnregModem( LPCTSTR pszActiveReg )
{
	EXE_API1_VOID( API_RAS, ERAPI_DeregisterModem, PRAPI_DeregisterModem, pszActiveReg );
}
BOOL	WINAPI	RasMdm_RegPPPoEd( LPCTSTR pszDrvReg, HANDLE hDev )
{
	EXE_API2( API_RAS, ERAPI_RegisterPPPoEd, PRAPI_RegisterPPPoEd, BOOL, pszDrvReg, hDev );
}
void	WINAPI	RasMdm_UnregPPPoEd( LPCTSTR pszDrvReg )
{
	EXE_API1_VOID( API_RAS, ERAPI_DeregisterPPPoEd, PRAPI_DeregisterPPPoEd, pszDrvReg );
}
