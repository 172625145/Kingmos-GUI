/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef __EWAKE_H
#define __EWAKE_H

#ifdef _cpluscplus
extern "C" {
#endif

#define APID_PLAN 0x123
#define APID_ALARM 0x124

#define SNDID_PLAN 0x123
#define SNDID_ALARM 0x124

#define MAX_WAKENAME_LEN 20
#define WAKETABLE_FIELDNUM 4

#define WAKE_FIELDID (1)
#define WAKE_FIELDDATA (2)
#define WAKE_FIELDNAME (3)
#define WAKE_FIELDFLAG (4)

typedef struct _REG_WAKE
{
	DWORD		dwApID;
	DWORD		dwSoundID;
	SYSTEMTIME	stWakeTime;
	TCHAR*		lpszWakeName;//唤醒标题最长为MAX_WAKENAME_LEN；
}REG_WAKE, *PREG_WAKE;

BOOL
RegisterWake(
	PREG_WAKE pWakeRegStruct,
	DWORD *pdwWakeID //当*pdwWakeID为0时注册一新的唤醒事件，否则修改*pdwWakeID对应的事件；
	);

BOOL
DeRegisterWake(
	DWORD dwApID,
	DWORD dwWakeID
	);

#ifdef _cpluscplus
}	//cpluscplus
#endif

#endif
