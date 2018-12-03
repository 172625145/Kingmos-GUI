/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _GPRS_SET_H_
#define _GPRS_SET_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
#define	LEN_GPRS_APN			20
//#define	LEN_GPRS_USER			64
//#define	LEN_GPRS_PASSWORD		128

//

typedef	struct	_GPRS_SET
{
	//
	WORD		wLinkType;
	WORD		wXXX;

	//
	char		pszAPN[LEN_GPRS_APN];
	//char		pszUser[LEN_GPRS_USER];
	//char		pszPassword[LEN_GPRS_PASSWORD];

	//
	DWORD		dwAP;//认证协议

} GPRS_SET;

//wLinkType
#define	GPRS_TYPE_NET			1
#define	GPRS_TYPE_WAP			2



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_GPRS_SET_H_

