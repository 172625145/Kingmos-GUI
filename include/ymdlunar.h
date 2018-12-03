/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __YMDLUNAR_H
#define __YMDLUNAR_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//农历算法

//年月日输入参数是 公历
//
/*************************************************
Lunar_GetYearRange
功能：获取该农历算法的年范围
原型：void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax )
参数：
	pwYearMin - 保存最低年
	pwYearMax - 保存最高年
返回值：无
************************************************/
extern	void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax );

/*************************************************
Lunar_CheckYear
功能：检查wYear年，是否在该农历算法的年范围内
原型：BOOL	Lunar_CheckYear( WORD wYear )
参数：
	wYear - 待检查的年
返回值：TRUE表示在该范围内
************************************************/
extern	BOOL	Lunar_CheckYear( WORD wYear );

//年月日输入参数是 公历
//
/*************************************************
Lunar_GetYMDFromAD
功能：根据公历年月日，计算出对应的农历年月日，及是否农历润月
原型：BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap )
参数：
	wYear - 给定的公历年
	wMonth - 给定的公历月
	wDay - 给定的公历日
	pwLYear - 计算得到的农历年
	pwLMonth - 计算得到的农历月
	pwLDay - 计算得到的农历日
	pbLeap - 计算得到的是否农历润月
返回值：TRUE表示计算成功
************************************************/
extern	BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap );

/*************************************************
Lunar_GetYMDsFromADMonth
功能：根据公历年月和这个月的天数，计算出在该公历年月对应的农历年月日
原型：BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] )
参数：
	wYear - 给定的公历年
	wMonth - 给定的公历月
	wDaysOfMonth - 给定的公历月中的天数
	arrLunarYMD - 计算得到的农历年月日的数组
返回值：TRUE表示计算成功
************************************************/
extern	BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] );

//年月日输入参数是 农历
//
/*************************************************
Lunar_GetLeapMonth
功能：得到该年的润月
原型：WORD	Lunar_GetLeapMonth( WORD wYear )
参数：
	wYear - 给定的农历年
返回值：
	得到该年的润月，如果没有就返回0
************************************************/
extern	WORD	Lunar_GetLeapMonth( WORD wYear );

/*************************************************
Lunar_GetDaysOfMonth
功能：得到该年月的天数
原型：WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap )
参数：
	wYear - 给定的农历年
	wMonth - 给定的农历月
	bLeap - 给定的这个月是否为润月（第2个月）
返回值：得到该年月的天数
************************************************/
extern	WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap );

/*************************************************
Lunar_GetNameYear
功能：得到该年的甲子表示
原型：void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp )
参数：
	wYear - 给定的农历年
	pszTemp - 计算得到的农历年的甲子String
返回值：无
************************************************/
extern	void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp );

/*************************************************
Lunar_GetNameMonth
功能：得到该月的中文编号表示（“正、二、三、... 十、冬、腊”）
原型：void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp )
参数：
	wMonth - 给定的农历月
	pszTemp - 计算得到的农历月的中文编号表示String
返回值：无
************************************************/
extern	void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp );

/*************************************************
Lunar_GetNameDay
功能：得到该月指定日的中文编号表示（“初一 ... 十五 ... 三十”）
原型：void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp )
参数：
	wMonth - 给定的农历月
	wDay - 给定的农历日
	pszTemp - 计算得到的农历日的中文编号表示String
返回值：无
************************************************/
extern	void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp );

/*************************************************
Lunar_GetNameYMD
功能：得到该年月日的中文编号表示（“甲子年 正月 初一”）
原型：void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, TCHAR *szTemp )
参数：
	wYear - 给定的农历年
	wMonth - 给定的农历月
	wDay - 给定的农历日
	pszTemp - 计算得到的农历年月日的中文编号表示String
返回值：无
************************************************/
extern	void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, TCHAR *szTemp );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__YMDLUNAR_H
