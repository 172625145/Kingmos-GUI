/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
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
	TCHAR*		lpszWakeName;//���ѱ����ΪMAX_WAKENAME_LEN��
}REG_WAKE, *PREG_WAKE;

BOOL
RegisterWake(
	PREG_WAKE pWakeRegStruct,
	DWORD *pdwWakeID //��*pdwWakeIDΪ0ʱע��һ�µĻ����¼��������޸�*pdwWakeID��Ӧ���¼���
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
