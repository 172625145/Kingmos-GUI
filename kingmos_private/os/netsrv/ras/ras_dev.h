/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _RAS_DEV_H_
#define _RAS_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

extern	BOOL	RasD_AllocDev( RAS_DEV** ppRasDev );
extern	void	RasD_FreeDev( RAS_DEV* pRasDev );
extern	BOOL	RasD_FindDev( OUT RAS_DEV** ppRasDev, LPCTSTR pszDrvReg );
extern	DWORD	RasD_DeregisterDev( LPCTSTR pszDrvReg );

extern	BOOL	RasT_FindTsp( OUT TSPINFO** ppTspInfo, LPCTSTR pszDllName, BOOL fRef );
extern	BOOL	RasT_LoadTsp( OUT TSPINFO** ppTspInfo, LPCTSTR pszDllName, BOOL fRef );
extern	void	RasT_FreeTsp( TSPINFO* lpTspInfo );
extern	void	RasT_FreeDev( RAS_DEV* pRasDev );


#ifdef __cplusplus
}	
#endif

#endif	//_RAS_DEV_H_
