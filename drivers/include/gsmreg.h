/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _GSM_REG_H_
#define _GSM_REG_H_

#ifndef _DRV_REG_H_
#include "drvreg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


// ----------------------------------------------------------
// GSM_API  的注册表键/项 宏定义---在 HKEY_SOFTWARE_ROOT 下
// ----------------------------------------------------------

//路径
#define	KROOT_GsmApi	HKEY_SOFTWARE_ROOT			//根路径
extern	const	TCHAR	KPATH_GsmApi[];

//GSM_API的 内容
//----"comm\\GsmApi"
extern	const	TCHAR	KITEM_GsmApi_Port[];



#ifdef __cplusplus
}	
#endif

#endif	//_GSM_REG_H_
