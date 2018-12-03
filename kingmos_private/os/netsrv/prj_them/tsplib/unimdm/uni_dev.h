/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TSP_MODEM_H_
#define _TSP_MODEM_H_

#ifdef __cplusplus
extern "C" {
#endif


extern	BOOL	lineDev_CheckType( LPTSTR pszDrvReg, DWORD* pdwDevType );
extern	BOOL	lineDev_Create( LINE_DEV* pLineDev );
extern	BOOL	lineDev_Open( LINE_DEV* pLineDev );
extern	BOOL	lineDev_GetInfo( LINE_DEV* pLineDev, OUT TSP_DLINFO* pInfoDl );
extern	BOOL	lineDev_Dial( LINE_DEV* pLineDev, LPCSTR pszLocalNumber );
extern	void	lineDev_SetRecv( LINE_DEV* pLineDev, HANDLE hIfDl, FNPROIN pFnIn );
extern	DWORD	lineDev_Send( LINE_DEV* pLineDev, LPBYTE pBuf, DWORD dwLen );
extern	void	lineDev_Stop0( LINE_DEV* pLineDev );
extern	void	lineDev_Stop( LINE_DEV* pLineDev );
extern	void	lineDev_Free( LINE_DEV* pLineDev );


#ifdef __cplusplus
}	
#endif

#endif	//_TSP_MODEM_H_

