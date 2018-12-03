/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_RAS_H_
#define	_NETIF_RAS_H_

#ifdef __cplusplus
extern "C" {
#endif


//
// RAS�����豸��ע����ע��
//
extern	BOOL	WINAPI	RAPI_RegisterModem( LPCTSTR pszActiveReg );
extern	void	WINAPI	RAPI_DeregisterModem( LPCTSTR pszActiveReg );
extern	BOOL	WINAPI	RAPI_RegisterPPPoEd( LPCTSTR pszDrvReg, HANDLE hDev );
extern	void	WINAPI	RAPI_DeregisterPPPoEd( LPCTSTR pszDrvReg );


//
//�ڲ�ʹ�õ�֪ͨ����
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
