/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：Shell API库
版本号：1.0.0.456
开发时期：2004-03-08
作者：陈建明 JAMI CHEN
修改记录：
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

	if( API_Enter( API_SHELL, SHELL_SHCHANGENOTIFY, &pSHChangeNotify, &cs ) )  // 得到API函数指针
	{
		dwItem1 = MapProcessPtr( dwItem1, (LPPROCESS)cs.lpvData ); // 映射指针
		dwItem2 = MapProcessPtr( dwItem2, (LPPROCESS)cs.lpvData ); // 映射指针

		pSHChangeNotify(wEventId, uFlags, dwItem1, dwItem2);  // 呼叫API函数
		API_Leave(  );
	}
	return ;
#endif
}


// **************************************************
// 声明：DWORD WINAPI SH_GetSysTimeFormat(void)
// 参数：
// 
// 返回值：
// 功能描述：得到系统时间格式
// 引用: 
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

	if( API_Enter( API_SHELL, SHELL_GETSYSTIMEFORMAT, &pGetSysTimeFormat, &cs ) )  // 得到API函数指针
	{
		retv = pGetSysTimeFormat();  // 呼叫API函数
		API_Leave(  );
	}
	return retv;
#endif
}

// **************************************************
// 声明：BOOL WINAPI SH_SetSysTimeFormat(DWORD dwFlag)
// 参数：
// 
// 返回值：
// 功能描述：设置系统时间格式
// 引用: 
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

	if( API_Enter( API_SHELL, SHELL_SETSYSTIMEFORMAT, &pSetSysTimeFormat, &cs ) )  // 得到API函数指针
	{
		retv = pSetSysTimeFormat(dwFlag);  // 呼叫API函数
		API_Leave(  );
	}
	return retv;
#endif
}
