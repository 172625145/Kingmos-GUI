/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����Shell API��
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-03-08
���ߣ��½��� JAMI CHEN
�޸ļ�¼��
**************************************************/
#include <eframe.h>
#include <eapisrv.h>
//#include <Shellapi.h>

#include <epcore.h>


enum{
    SHELL_SHCHANGENOTIFY = 1,
    SHELL_GETSYSTIMEFORMAT,
    SHELL_SETSYSTIMEFORMAT,
};



typedef VOID (WINAPI * PSHCHANGENOTIFY)(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2);
VOID WINAPI SH_ChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2)
{
#ifdef CALL_TRAP
//	(PSHCHANGENOTIFY)CALL_API( API_SHELL, SHELL_SHCHANGENOTIFY, 4 ) )(wEventId, uFlags, dwItem1, dwItem2);
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_SHELL, SHELL_SHCHANGENOTIFY, 4 );
    cs.arg0 = (DWORD)wEventId;
    CALL_SERVER( &cs, uFlags, dwItem1, dwItem2);
	return;
#else
	PSHCHANGENOTIFY pSHChangeNotify;

	CALLSTACK cs;

	if( API_Enter( API_SHELL, SHELL_SHCHANGENOTIFY, &pSHChangeNotify, &cs ) )  // �õ�API����ָ��
	{
		dwItem1 = MapProcessPtr( dwItem1, (LPPROCESS)cs.lpvData ); // ӳ��ָ��
		dwItem2 = MapProcessPtr( dwItem2, (LPPROCESS)cs.lpvData ); // ӳ��ָ��

		pSHChangeNotify(wEventId, uFlags, dwItem1, dwItem2);  // ����API����
		API_Leave(  );
	}
	return ;
#endif
}


// **************************************************
// ������DWORD WINAPI SH_GetSysTimeFormat(void)
// ������
// 
// ����ֵ��
// �����������õ�ϵͳʱ���ʽ
// ����: 
// **************************************************
typedef DWORD (WINAPI * PGETSYSTIMEFORMAT)(void);
DWORD WINAPI SH_GetSysTimeFormat(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_SHELL, SHELL_GETSYSTIMEFORMAT, 0 );
    cs.arg0 = (DWORD)0;
    return CALL_SERVER( &cs);
//	(PSHCHANGENOTIFY)CALL_API( API_SHELL, SHELL_GETSYSTIMEFORMAT, 0 ) )();
//	return;
#else
	PGETSYSTIMEFORMAT pGetSysTimeFormat;

	CALLSTACK cs;
	DWORD retv;

	if( API_Enter( API_SHELL, SHELL_GETSYSTIMEFORMAT, &pGetSysTimeFormat, &cs ) )  // �õ�API����ָ��
	{
		retv = pGetSysTimeFormat();  // ����API����
		API_Leave(  );
	}
	return retv;
#endif
}

// **************************************************
// ������BOOL WINAPI SH_SetSysTimeFormat(DWORD dwFlag)
// ������
// 
// ����ֵ��
// ��������������ϵͳʱ���ʽ
// ����: 
// **************************************************
typedef BOOL (WINAPI * PSETSYSTIMEFORMAT)(DWORD dwFlag);
BOOL WINAPI SH_SetSysTimeFormat(DWORD dwFlag)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_SHELL, SHELL_SETSYSTIMEFORMAT, 1 );
    cs.arg0 = (DWORD)dwFlag;
    return CALL_SERVER( &cs);
//	(PSHCHANGENOTIFY)CALL_API( API_SHELL, SHELL_SETSYSTIMEFORMAT, 1 ) )(dwFlag);
//	return;
#else
	PSETSYSTIMEFORMAT pSetSysTimeFormat;

	CALLSTACK cs;
	BOOL retv;

	if( API_Enter( API_SHELL, SHELL_SETSYSTIMEFORMAT, &pSetSysTimeFormat, &cs ) )  // �õ�API����ָ��
	{
		retv = pSetSysTimeFormat(dwFlag);  // ����API����
		API_Leave(  );
	}
	return retv;
#endif
}
