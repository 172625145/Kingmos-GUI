/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _XFUN_2004_07_08_
#define _XFUN_2004_07_08_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifndef	IN
#define	IN
#endif

#ifndef	OUT
#define	OUT
#endif

#ifndef	OPTION
#define	OPTION
#endif

#ifndef	OPTIONAL
#define	OPTIONAL
#endif

#define	DWORD_4B(dw)				( (BYTE)(dw>>(3*8)) )
#define	DWORD_3B(dw)				( (BYTE)(dw>>(2*8)) )
#define	DWORD_2B(dw)				( (BYTE)(dw>>(1*8)) )
#define	DWORD_1B(dw)				( (BYTE)(dw) )

//
#define	WM_MMS_NEW_MSG				7891
#define	WM_MMS_NEW_REPORT			7892
//#define	WM_MMS_TR_STATUS			7893



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_XFUN_2004_07_08_
