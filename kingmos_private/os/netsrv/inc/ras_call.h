/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _RAS_CALL_H_
#define _RAS_CALL_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// RAS拨号模式
extern	BOOL	WINAPI	RAPI_EnumDialMethod( OUT OPTIONAL LPDIALM_NAME lpDialMthodName, IN OUT DWORD* pdwCnt );
extern	BOOL	WINAPI	RAPI_GetDialMethod( LPCTSTR szDialName, DWORD dwOption, OUT LPDIAL_METHOD lpDialMthod );
extern	BOOL	WINAPI	RAPI_GenNum( RASENTRY* lpEntry, OPTIONAL LPCSTR pszNumLocal, OUT LPSTR pszNumNew, DWORD dwLen );


// RAS拨号条目
extern	BOOL	WINAPI	RAPI_EnumEntries( LPCTSTR lpszPhonebook, OUT RASENTRYNAME* lpEntryNames, IN OUT DWORD* lpdwCntEntries );
extern	BOOL	WINAPI	RAPI_GetEntryDef( LPCTSTR lpszPhonebook, OUT LPTSTR pszEntryName, DWORD dwLen );
extern	BOOL	WINAPI	RAPI_SetEntryDef( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );
extern	BOOL	WINAPI	RAPI_DeleteEntry( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );
extern	BOOL	WINAPI	RAPI_RenameEntry( LPCTSTR lpszPhonebook, LPCTSTR pszOldEntry, LPCTSTR pszNewEntry );
extern	BOOL	WINAPI	RAPI_IsValidEntryName( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );

extern	BOOL	WINAPI	RAPI_GetEntryProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT RASENTRY* lpEntry, IN OUT LPDWORD lpdwEntrySize );
extern	BOOL	WINAPI	RAPI_SetEntryProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, RASENTRY* lpEntry, DWORD dwEntrySize );

extern	BOOL	WINAPI	RAPI_GetEntryDialParams( LPCTSTR lpszPhonebook, IN OUT DIALPARAM* lpDialParams, OUT BOOL* lpfPassword );
extern	BOOL	WINAPI	RAPI_SetEntryDialParams( LPCTSTR lpszPhonebook, DIALPARAM* lpDialParams, BOOL fRemovePassword );

extern	BOOL	WINAPI	RAPI_GetEntryDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, OUT LPVOID lpDeviceConfigOut, IN OUT DWORD* lpdwSize );
extern	BOOL	WINAPI	RAPI_SetEntryDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, LPVOID lpDeviceConfigIn, DWORD dwSize );


// RAS拨号设备
extern	void	WINAPI	RAPI_EnumDevices( OUT RASDEVINFO* lpRasDevinfo, IN OUT DWORD* lpdwCntDevices );
extern	BOOL	WINAPI	RAPI_LookDriver( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName );
extern	BOOL	WINAPI	RAPI_CheckDev( DWORD dwChkFlag, DWORD dwChkDevType );


// RAS拨号连接
extern	DWORD	WINAPI	RAPI_Dial( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn );
extern	DWORD	WINAPI	RAPI_HangUp( HANDLE hRasConn );
extern	DWORD	WINAPI	RAPI_EnumConnections( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn );

extern	DWORD	WINAPI	RAPI_GetConnectInfo( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo );
extern	DWORD	WINAPI	RAPI_GetConnectStatus( HANDLE hRasConn, OUT DWORD* pdwDevStatus );

extern	DWORD	WINAPI	RAPI_EnableStat( HANDLE hRasConn, BOOL fEnbaleBps );
extern	DWORD	WINAPI	RAPI_GetLinkStatistics( HANDLE hRasConn, OUT RAS_STATS *lpStatistics );


// RAS拨号辅助操作
extern	DWORD	WINAPI	RAPI_GetStateStr( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize );
extern	DWORD	WINAPI	RAPI_GetErrStr( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize );



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_RAS_CALL_H_

