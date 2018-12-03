/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _GPRS_CFG_H_
#define _GPRS_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
#define	GPRSCLASS_A				0
#define	GPRSCLASS_B				1
#define	GPRSCLASS_C				2
#define	GPRSCLASS_CG			3
#define	GPRSCLASS_CC			4

#define	GPRSCLASS_MAX			GPRSCLASS_CC

//wNetworkType
#define	GPRS_NETTYPE_INTERNET		1
#define	GPRS_NETTYPE_WAP			2
#define	GPRS_NETTYPE_CURRENT		8
#define	GPRS_NETTYPE_UNKNOWN		15

//
#define	LEN_GPRSAPN					20
//#define	LEN_GPRSPPG					32
typedef	struct	_GPRS_CFG
{
	DWORD			dwNetworkType;
	char			szApn[LEN_GPRSAPN+4];
	//char			szPPG[LEN_GPRSPPG+4];

	DWORD			dwIP_PPG;
	WORD			wPort_PPG;

} GPRS_CFG;

//
extern	BOOL	GprsCfg_GetCurNetType( OUT DWORD* pdwNetworkType );
extern	BOOL	GprsCfg_SetCurNetType( DWORD dwNetworkType );

extern	BOOL	GprsCfg_GetApn( DWORD dwNetworkType, OUT LPSTR pszApn, DWORD dwLen );
extern	BOOL	GprsCfg_SetApn( DWORD dwNetworkType, LPSTR pszApn );

extern	BOOL	GprsCfg_GetPPG( DWORD dwNetworkType, OUT DWORD* pdwIP_PPG, OUT WORD* pwPort_PPG );
extern	BOOL	GprsCfg_SetPPG( DWORD dwNetworkType, DWORD dwIP_PPG, WORD wPort_PPG );

extern	BOOL	GprsCfg_GetAll( DWORD dwNetworkType, OUT GPRS_CFG* pCfgGprs );
extern	BOOL	GprsCfg_SetAll( GPRS_CFG* pCfgGprs );


//
//
//
extern	BOOL	MmsCfg_GetSC( LPSTR pszMmsSC, DWORD dwLen );
extern	BOOL	MmsCfg_SetSC( LPSTR pszMmsSC );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_GPRS_CFG_H_

