/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _ERROR_INFO_H_
#define _ERROR_INFO_H_


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


typedef	struct	_ERROR_INFO
{
	DWORD		dwType;
	DWORD		dwValue;
	DWORD		dwParam;

} ERROR_INFO;

//
//dwType
//

//
#define	ERR_MASK_VALUE				0x80000000 // you can check----dwValue
#define	ERR_MASK_PARAM				0x40000000 // you can check----dwParam

#define	ERR_GET_MASK(dw)			(dw & (ERR_MASK_VALUE|ERR_MASK_PARAM))
#define	ERR_GET_TYPE(dw)			(dw & 0x0000FFFF))

//
#define	ERR_TYPE_UNKNOWN			0 //unknown error, you can't retrieval it
#define	ERR_TYPE_GENERAL			1 //general error
#define	ERR_TYPE_SOCKET				2 //socket error, you can WSAGetLastError()
#define	ERR_TYPE_WSP				3 //wap error, you can WSPGetLastError()


#define	ERR_TYPE_APP				1234 //some application customed this error value


//Added by xyg-2004_06_25
#define	ERR_TYPE_HTTP_STATUS		2000
#define	ERR_TYPE_WSP_STATUS			2005
#define	ERR_TYPE_WSP_ABORT			2006



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_ERROR_INFO_H_
