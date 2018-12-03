/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_RAS_H_
#define	_NETIF_RAS_H_

#ifdef __cplusplus
extern "C" {
#endif


//
// RAS拨号设备的注册与注销
//
extern	BOOL	WINAPI	RAPI_RegisterModem( LPCTSTR pszActiveReg );
extern	void	WINAPI	RAPI_DeregisterModem( LPCTSTR pszActiveReg );
extern	BOOL	WINAPI	RAPI_RegisterPPPoEd( LPCTSTR pszDrvReg, HANDLE hDev );
extern	void	WINAPI	RAPI_DeregisterPPPoEd( LPCTSTR pszDrvReg );


//
//内部使用的通知函数
//
extern	void	RasIf_NotifyEvt( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr );
extern	DWORD	RasIf_DownData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen );

extern	DWORD	RasIf_HangUp( HANDLE hRasDev, DWORD dwOperation, DWORD dwRasCnnStatus, DWORD dwErr );
#define	RASH_OP_FREEDEV		0x00000001
#define	RASH_OP_DIRECT		0x00000004
#define	RASH_OP_WAITOVER	0x00000008


#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_RAS_H_
