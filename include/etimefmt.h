/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ETIMEFMT_H
#define __ETIMEFMT_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

#define TIMEFMT_12HOUR		0x0001// 12小时格式
#define TIMEFMT_24HOUR		0x0000// 24小时格式,为默认格式

// 得到当前的时间格式
#define GetSysTimeFormat SH_GetSysTimeFormat
DWORD WINAPI SH_GetSysTimeFormat(void);


// 得到当前的时间格式
#define SetSysTimeFormat SH_SetSysTimeFormat
BOOL WINAPI SH_SetSysTimeFormat(DWORD dwFlag);


// 得到格式化后的时间串
int FormatTime(CONST SYSTEMTIME *lpTime, LPTSTR lpTimeStr,int cchTime ,DWORD dwFlag);
// dwFlag 定义
#define FT_STANDARD		0x0000	//标准格式 即 HH:MM:SS 或 HH:MM ... 即每一位显示两位  eg 09:38
#define FT_SIMPLIFY		0x0001	//精简格式 即 H:M:S 或 H:M ... 即每一位可以显示一位  eg 9:38
#define FT_LONGSTYLE	0X0000	//长格式   即 显示时分秒
#define FT_SHORTSTYLE	0X0002	//短格式   即 显示时分

#ifdef __cplusplus
}
#endif  //__cplusplus


#endif  //__ETIMEFMT_H
