/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
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
// GSM_API  ��ע����/�� �궨��---�� HKEY_SOFTWARE_ROOT ��
// ----------------------------------------------------------

//·��
#define	KROOT_GsmApi	HKEY_SOFTWARE_ROOT			//��·��
extern	const	TCHAR	KPATH_GsmApi[];

//GSM_API�� ����
//----"comm\\GsmApi"
extern	const	TCHAR	KITEM_GsmApi_Port[];



#ifdef __cplusplus
}	
#endif

#endif	//_GSM_REG_H_
