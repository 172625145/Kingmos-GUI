/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：农历算法
版本号：  1.0.0
开发时期：2002.01.06
作者：    kingkong and 肖远钢

修改：	  刘  军

2003-06-10:  
    by 肖远钢
         1> 更改函数名  
         2> 增加 年 的最大最小数值的获取和校验函数  
         3> 增加 头文件YMDlunar.h
         4> 原来文件名是LunarCld.c，可对应比较参考
2004-09-06:农历的日期范围限制（By Liujun）

******************************************************/

#include <ewindows.h>
#include <ymdlunar.h>

/***************  全局区 定义， 声明 *****************/

//农历算法的最小 最大值
#define LUNAR_YEAR_MIN					1901
#define LUNAR_YEAR_MAX					2050

//
//数组gLunarDay存入阴历1901年到2100年每年中的月天数信息，
//阴历每月只能是29或30天，一年用12（或13）个二进制位表示，对应位为1表30天，否则为29天
//每年的月份从高位开始,即最高位是一月,次之二月 等等.......
//测试数据只有1901.2.19 --2050.12.31
//
static const WORD g_LunarMonthDay[150] = {
   0x4ae0, 0xa570, 0x5268, 0xd260, 0xd950, 0x6aa8, 0x56a0, 0x9ad0, 0x4ae8, 0x4ae0,   //1910
   0xa4d8, 0xa4d0, 0xd250, 0xd548, 0xb550, 0x56a0, 0x96d0, 0x95b0, 0x49b8, 0x49b0,   //1920
   0xa4b0, 0xb258, 0x6a50, 0x6d40, 0xada8, 0x2b60, 0x9570, 0x4978, 0x4970, 0x64b0,   //1930
   0xd4a0, 0xea50, 0x6d48, 0x5ad0, 0x2b60, 0x9370, 0x92e0, 0xc968, 0xc950, 0xd4a0,   //1940
   0xda50, 0xb550, 0x56a0, 0xaad8, 0x25d0, 0x92d0, 0xc958, 0xa950, 0xb4a8, 0x6ca0,   //1950
   0xb550, 0x55a8, 0x4da0, 0xa5b0, 0x52b8, 0x52b0, 0xa950, 0xe950, 0x6aa0, 0xad50,   //1960
   0xab50, 0x4b60, 0xa570, 0xa570, 0x5260, 0xe930, 0xd950, 0x5aa8, 0x56a0, 0x96d0,   //1970
   0x4ae8, 0x4ad0, 0xa4d0, 0xd268, 0xd250, 0xd528, 0xb540, 0xb6a0, 0x96d0, 0x95b0,   //1980
   0x49b0, 0xa4b8, 0xa4b0, 0xb258, 0x6a50, 0x6d40, 0xada0, 0xab60, 0x9370, 0x4978,   //1990
   0x4970, 0x64b0, 0x6a50, 0xea50, 0x6b28, 0x5ac0, 0xab60, 0x9368, 0x92e0, 0xc960,   //2000
   0xd4a8, 0xd4a0, 0xda50, 0x5aa8, 0x56a0, 0xaad8, 0x25d0, 0x92d0, 0xc958, 0xa950,   //2010
   0xb4a0, 0xb550, 0xb550, 0x55a8, 0x4ba0, 0xa5b0, 0x52b8, 0x52b0, 0xa930, 0x74a8,   //2020
   0x6aa0, 0xad50, 0x4da8, 0x4b60, 0x9570, 0xa4e0, 0xd260, 0xe930, 0xd530, 0x5aa0,   //2030
   0x6b50, 0x96d0, 0x4ae8, 0x4ad0, 0xa4d0, 0xd258, 0xd250, 0xd520, 0xdaa0, 0xb5a0,   //2040
   0x56d0, 0x4ad8, 0x49b0, 0xa4b8, 0xa4b0, 0xaa50, 0xb528, 0x6d20, 0xada0, 0x55b0	 //2050
};
//数组gLanarMonth存放阴历1901年到2050年闰月的月份，如没有则为0，每字节存两年
static const BYTE  g_LunarLeapMonth[75] = {
   0x00, 0x50, 0x04, 0x00, 0x20,   //1910
   0x60, 0x05, 0x00, 0x20, 0x70,   //1920
   0x05, 0x00, 0x40, 0x02, 0x06,   //1930
   0x00, 0x50, 0x03, 0x07, 0x00,   //1940
   0x60, 0x04, 0x00, 0x20, 0x70,   //1950
   0x05, 0x00, 0x30, 0x80, 0x06,   //1960
   0x00, 0x40, 0x03, 0x07, 0x00,   //1970
   0x50, 0x04, 0x08, 0x00, 0x60,   //1980
   0x04, 0x0a, 0x00, 0x60, 0x05,   //1990
   0x00, 0x30, 0x80, 0x05, 0x00,   //2000
   0x40, 0x02, 0x07, 0x00, 0x50,   //2010
   0x04, 0x09, 0x00, 0x60, 0x04,   //2020
   0x00, 0x20, 0x60, 0x05, 0x00,   //2030
   0x30, 0xb0, 0x06, 0x00, 0x50,   //2040
   0x02, 0x07, 0x00, 0x50, 0x03	   //2050 
};
static const TCHAR szDays[30][10] =
{
	"初一", "初二", "初三", "初四", "初五",
    "初六", "初七", "初八", "初九", "初十",
    "十一", "十二", "十三", "十四", "十五",
    "十六", "十七", "十八", "十九", "二十",
    "廿一", "廿二", "廿三", "廿四", "廿五",
    "廿六", "廿七", "廿八", "廿九", "三十"
};
static const TCHAR szMonths[12][10]=
{   
	"正", "二", "三", "四", "五", "六", "七", "八", "九", "十",
    "冬", "腊"
};

#define	ARR_COUNT( arr )	(sizeof(arr)/sizeof(arr[0]))

//---------------------------------------------------
static	WORD	Lunar_GetDaysOfYear( WORD wYear );
static	WORD	Lunar_GetYearByDays( WORD wDays, WORD *pwDayYear );
static	WORD	Lunar_GetMonthByDays( WORD wYear,WORD wDays, WORD *pwDays, BOOL *pbLeap );

/******************************************************/


//---------------------------------------------------
// ********************************************************************
//声明：WORD	Lunar_GetDaysOfYear( WORD wYear )
//参数：
//	IN wYear - 指定的年
//返回值：
//	成功返回指定年的所有天数
//功能描述：获取指定年的所有天数
//引用: 
// ********************************************************************
WORD	Lunar_GetDaysOfYear( WORD wYear )
{
	WORD		wMonth = 0x8000;	// the number is use to get month day's
	WORD		wYearData ;
	WORD		wAllDays = 0 ;
	int			i, iMonths;

	if( (wYear>LUNAR_YEAR_MAX)||(wYear<LUNAR_YEAR_MIN) )
		return 0 ;

	wYearData = g_LunarMonthDay[ wYear - LUNAR_YEAR_MIN ] ;
	
	if( Lunar_GetLeapMonth( wYear ) == 0 )
		iMonths = 12 ;
	else 
		iMonths = 13 ;
	
	for( i = 1 ; i <= iMonths ; i ++ )
	{
		if( wYearData&wMonth )
		{
			wAllDays += 30 ;
		}else
		{
			wAllDays += 29 ;
		}
		wMonth = wMonth >> 1 ;
	}
	return wAllDays;
}
// ********************************************************************
//声明：WORD	Lunar_GetYearByDays( WORD wDays, WORD *pwDayYear )
//参数：
//	IN wDays - 指定的天数
//	OUT pwDayYear - 保存得到的年
//返回值：
//	成功返回指定的天数所确定的年
//功能描述：根据指定的天数，获取得到的年，并返回
//引用: 
// ********************************************************************
// the wDays is all days from you want to 1901.1.1
// the return year is the lunar year
// pwDayYear is the days of more than the year
WORD	Lunar_GetYearByDays( WORD wDays, WORD *pwDayYear )
{
	WORD			iYear  = LUNAR_YEAR_MIN ;
	WORD			wAllDays = 0, wDayOfYear = 0;

	while( 1 )
	{
		wDayOfYear = Lunar_GetDaysOfYear( iYear );
		if( wAllDays + wDayOfYear >= wDays )
			break;
		iYear ++ ;
		wAllDays += wDayOfYear ;
		if( iYear > 2050 )
			return 0 ;
	}
	*pwDayYear = wDays - wAllDays ;
	return iYear  ;
}
// ********************************************************************
//声明：WORD	Lunar_GetMonthByDays( WORD wYear, WORD wDays, WORD *pwDays, BOOL *pbLeap )
//参数：
//	IN wYear - 指定的年
//	IN wDays - 指定的天数
//	OUT pwDays - 保存该年该月的天数
//	OUT pbLeap - 保存该年该月是否为第2个月（即闰月）
//返回值：
//	成功返回年和指定的天数所确定的月
//功能描述：计算确定的月、该月的天数、该月是否为第2个月
//引用: 
// ********************************************************************
/*
return the month of the year.
if the wMonth is not leap month, the *pbLeap is zero
if the wMonth is leap month, 
if the month is Second leap month,*pbLeap is TRUE, otherwise is FALSE.
*/
WORD	Lunar_GetMonthByDays( WORD wYear, WORD wDays, WORD *pwDays, BOOL *pbLeap )
{
	WORD		wMonthIndex = 0x8000 ;
	WORD		wAllDays = 0, wThisMonth ;
	WORD		wLeapMonth;
	WORD		wYearData ;
	BOOL		bFirstLeapMonth = TRUE ;
	int			i, iMonths;

	ASSERT(wYear>=LUNAR_YEAR_MIN);
	ASSERT(wYear<=LUNAR_YEAR_MAX);

	*pbLeap = FALSE ;
	wYearData = g_LunarMonthDay[ wYear - LUNAR_YEAR_MIN ] ;
	wLeapMonth = Lunar_GetLeapMonth( wYear );
	
	if( wLeapMonth == 0 )
		iMonths = 12 ;
	else 
		iMonths = 13 ;
	
	for( i = 1 ; i <= iMonths ; i ++ )
	{
		if( wYearData&wMonthIndex )
		{
			wThisMonth = 30 ;
		}else
		{
			wThisMonth = 29 ;
		}
		if( wAllDays + wThisMonth >= wDays )
		{
			if( bFirstLeapMonth == FALSE )
			{
				i -- ;
				if( i == wLeapMonth )
					*pbLeap = TRUE ;
			}
			*pwDays = wDays - wAllDays ;
			return i;
		}
		wAllDays += wThisMonth ;
		wMonthIndex = wMonthIndex >> 1 ;
		if( wLeapMonth == i )
		{
			bFirstLeapMonth = FALSE ;
		}
	}
	return 0;
}

//---------------------------------------------------
//---------------------------------------------------

// ********************************************************************
//声明：WORD	Lunar_GetLeapMonth( WORD wYear )
//参数：
//	IN wYear - 给定的农历年
//返回值：
//	得到该年的润月，如果没有就返回0
//功能描述：得到该年的润月，如果没有就返回0
//引用: 
// ********************************************************************
WORD	Lunar_GetLeapMonth( WORD wYear )
{
	BYTE			bLeap;	// Get Leap data from global array 
	BYTE			bLeapThisYear = 0; // the leap month of the year
	WORD			wYearIndex; // the global array's index

	if( (wYear>LUNAR_YEAR_MAX)||(wYear<LUNAR_YEAR_MIN) )
	{
		return 0 ;
	}

	wYearIndex = ( wYear - LUNAR_YEAR_MIN ) / 2 ;
	bLeap = g_LunarLeapMonth[ wYearIndex ] ;
	if( ( wYear % 2 ) == 0 )
	{								// may use bLeap << 4 , but something wrong!!
		bLeapThisYear = ( BYTE )( bLeap &0x0f ) ;//Get Low of bLeap
	}else
	{
		bLeapThisYear = ( BYTE )( bLeap >> 4 ) ;//Get Hight of bLeap
	}
	return ( WORD )bLeapThisYear ;
}

// ********************************************************************
//声明：WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap )
//参数：
//	IN wYear - 给定的农历年
//	IN wMonth - 给定的农历月
//	IN bLeap - 给定的这个月是否为润月（第2个月）
//返回值：
//	得到该年月的天数
//功能描述：得到该年月的天数
//引用: 
// ********************************************************************
WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap )
{
	WORD				wYearDays ; // store data from array
	WORD				wLeapMonth ;
	WORD				wMonthIndex ;// 1 ~ 12 or 1 ~ 13 if the year have leap month

	if( ( wYear > LUNAR_YEAR_MAX )||( wYear < LUNAR_YEAR_MIN ) )
	{
		return 0 ;
	}
	if ( (wMonth<1)||(wMonth>12) )	// 保证月份的正确性
	{
		return 0;
	}

	wYearDays = g_LunarMonthDay[ wYear - LUNAR_YEAR_MIN ] ;

	wLeapMonth = Lunar_GetLeapMonth( wYear ) ;
	if( ( ( wLeapMonth == wMonth )&&( bLeap == TRUE ) )||
		( ( wLeapMonth > 0 )&&( wMonth > wLeapMonth ) ) )
		wMonthIndex = wMonth + 1 ;
	else
		wMonthIndex = wMonth ;

	wMonthIndex = 0x8000 >> ( wMonthIndex - 1 ) ;
	if( wMonthIndex&wYearDays )
		return 30 ;
	else
		return 29 ;
}

// ********************************************************************
//声明：BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap )
//参数：
//	IN wYear - 给定的公历年
//	IN wMonth - 给定的公历月
//	IN wDay - 给定的公历日
//	IN pwLYear - 计算得到的农历年
//	IN pwLMonth - 计算得到的农历月
//	IN pwLDay - 计算得到的农历日
//	IN pbLeap - 计算得到的是否农历润月
//返回值：
//	TRUE表示计算成功
//功能描述：根据公历年月日，计算出对应的农历年月日，及是否农历润月
//引用: 
// ********************************************************************
BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap )
{
	WORD			wAllDays=0 ;
	WORD			wLunarYear, wLunarMonth, wLunarDays, wLunarMonthDays ;
	WORD			i;

	if( (wYear>LUNAR_YEAR_MAX)||(wYear<LUNAR_YEAR_MIN) )		// 年份限制
	{
		return FALSE ;
	}
	if( (wYear==LUNAR_YEAR_MIN)&&(wMonth==2)&&(wDay<=18) )		// 最低日期限制
	{
		return FALSE ;
	}
	if( (wYear==LUNAR_YEAR_MIN)&&(wMonth==1) )					// 最低日期限制
	{
		return FALSE;
	}
	if ( (wMonth<1||wMonth>12)||(wDay<1||wDay>31) )				// 月和日限制
	{
		return FALSE;
	}

	// Get the days from LUNAR_YEAR_MIN to wYear, but exclude wYear
	for( i = LUNAR_YEAR_MIN ; i < wYear  ; i ++ )
	{
        wAllDays+=IsLeap( ( short ) i )? 366 : 365 ;
	}
	// Get the Days from Jannary to wMonth,but exclude wMonth
	for( i = 1 ; i < wMonth; i ++ )
	{
		wAllDays += GetDayofTheMonth( wYear, i );
	}
	// Get the days of the wDay ;
	wAllDays += wDay ;

	wAllDays -= 49;// 49 = sun calendar days - lunar calendar days ;
	
	wLunarYear = Lunar_GetYearByDays( wAllDays, &wLunarDays );

	wLunarMonth = Lunar_GetMonthByDays( wLunarYear, wLunarDays,&wLunarMonthDays,pbLeap );
	
	*pwLYear = wLunarYear ;
	*pwLMonth = wLunarMonth ;
	*pwLDay = wLunarMonthDays ;

	return TRUE ;
}


// ********************************************************************
//声明：BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] )
//参数：
//	IN wYear - 给定的公历年
//	IN wMonth - 给定的公历月
//	IN wDaysOfMonth - 给定的公历月中的天数
//	IN arrLunarYMD - 计算得到的农历年月日的数组
//返回值：
//	TRUE表示计算成功
//功能描述：根据公历年月和这个月的天数，计算出在该公历年月对应的农历年月日
//引用：
//
// ********************************************************************
BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] )
{
	BOOL	bLeap;
	WORD	wDay;

	if ( (wYear<LUNAR_YEAR_MIN)||(wYear>LUNAR_YEAR_MAX) )	// 年的限制
	{
		return FALSE;
	}

	if ( (wYear==LUNAR_YEAR_MIN)&&(wMonth<2) )				// 临界值的判断
	{
		return FALSE;
	}

	if ( (wDaysOfMonth<1)||(wDaysOfMonth>31) )				// 日的限制
	{
		return FALSE;
	}

	if ( (wMonth<1)||(wMonth>12) )							// 月的限制
	{
		return FALSE;
	}

	if ( (wYear==LUNAR_YEAR_MIN)&&(wMonth==2) )				// 临界值赋值
	{
		for( wDay=1; wDay<19; wDay++ )
		{
			arrLunarYMD[wDay][0] = 0;	// 年
			arrLunarYMD[wDay][1] = 0;	// 月
			arrLunarYMD[wDay][2] = 0;	// 日
		}
		for( wDay=19; wDay<wDaysOfMonth; wDay++ )
		{
			if( !Lunar_GetYMDFromAD( wYear, wMonth, wDay, &arrLunarYMD[wDay][0], 
									&arrLunarYMD[wDay][1], &arrLunarYMD[wDay][2], &bLeap ) )
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	if( wDaysOfMonth==0 )
	{
		wDaysOfMonth = GetDayofTheMonth( wYear, wMonth );
	}
	for( wDay=1; wDay<=wDaysOfMonth; wDay++ )
	{
		if( !Lunar_GetYMDFromAD( wYear, wMonth, wDay, &arrLunarYMD[wDay][0], 
								&arrLunarYMD[wDay][1], &arrLunarYMD[wDay][2], &bLeap ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

//===========================================
static const TCHAR c1[][3] = 
{ "甲", "乙", "丙", "丁", "戊", "己", "庚", "辛", "壬", "癸" };
static const TCHAR c2[][3] = 
{ "子", "丑", "寅", "卯", "辰", "巳", "午", "未", "申", "酉", "戌", "亥" };

// ********************************************************************
//声明：void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp )
//参数：
//	IN wYear - 给定的农历年
//	IN pszTemp - 计算得到的农历年的甲子String
//返回值：
//	无
//功能描述：得到该年的甲子表示
//引用: 
// ********************************************************************
void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp )
{
	int x, y, ya ;

	if ( (wYear<LUNAR_YEAR_MIN)||(wYear>LUNAR_YEAR_MAX) )	// 年的限制
	{
		return ;
	}

	ya = wYear+8;
	if( ya < 1 )
	{
		ya = ya + 1;
	}
	if( ya < 12 )
	{
		ya = ya + 60;
	}
	x = ( ya + 8 - ( ( ya + 7 ) / 10 ) * 10 )%10;
	y = ( ya - ( ( ya-1 ) / 12 ) * 12 )%12;
	strcpy( pszTemp, c1[x] );
	strcat( pszTemp, c2[y] );
}

// ********************************************************************
//声明：void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp )
//参数：
//	IN wMonth - 给定的农历月
//	IN pszTemp - 计算得到的农历月的中文编号表示String
//返回值：
//	无
//功能描述：得到该月的中文编号表示（“正、二、三、... 十、冬、腊”）
//引用: 
// ********************************************************************
void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp )
{
	if ( (wMonth<1)||(wMonth>12) )		// 月的限制
	{
		return ;
	}

	ASSERT( wMonth<=ARR_COUNT(szMonths) );
	sprintf( pszTemp,"%s月",szMonths[wMonth-1] ) ;
}

// ********************************************************************
//声明：void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp )
//参数：
//	IN wMonth - 给定的农历月
//	IN wDay - 给定的农历日
//	IN pszTemp - 计算得到的农历日的中文编号表示String
//返回值：
//	无
//功能描述：得到该月指定日的中文编号表示（“初一 ... 十五 ... 三十”）
//引用: 
// ********************************************************************
void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp )
{
	if ( (wMonth<1)||(wMonth>12) )		// 月的限制
		return ;

	if ( (wDay<1)||(wDay>30) )			// 日的限制
		return ;

	if( !wMonth && (wDay == 1) )
	{
		sprintf( pszTemp,"%s月",szMonths[wMonth-1] ) ;
	}
	else
	{
		ASSERT( wDay<=ARR_COUNT(szDays) );
		strcpy( pszTemp, szDays[wDay-1] );
	}
}

// ********************************************************************
//声明：void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, LPTSTR pszTemp )
//参数：
//	IN wYear - 给定的农历年
//	IN wMonth - 给定的农历月
//	IN wDay - 给定的农历日
//	IN pszTemp - 计算得到的农历年月日的中文编号表示String
//返回值：
//	无
//功能描述：得到该年月日的中文编号表示（“甲子年 正月 初一”）
//引用: 
// ********************************************************************
void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, LPTSTR pszTemp )
{
	TCHAR	pszYear[12];

	if ( (wYear<LUNAR_YEAR_MIN)||(wYear>LUNAR_YEAR_MAX) )	// 年的限制
	{
		return ;
	}
	if ( (wMonth<1)||(wMonth>12) )							// 月的限制
	{
		return ;
	}

	ASSERT( wMonth<=ARR_COUNT(szMonths) );
	ASSERT( wDay<=ARR_COUNT(szDays) );
	Lunar_GetNameYear( wYear, pszYear );
	sprintf( pszTemp,"%s年 %s月 %s", pszYear, szMonths[wMonth-1], szDays[wDay-1] );
}


// ********************************************************************
//声明：void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax )
//参数：
//	IN pwYearMin - 保存最低年
//	IN pwYearMax - 保存最高年
//返回值：
//	无
//功能描述：获取该农历算法的年范围
//引用: 
// ********************************************************************
void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax )
{
	ASSERT( pwYearMin!= NULL);
	ASSERT( pwYearMax!= NULL);
	*pwYearMin = LUNAR_YEAR_MIN;
	*pwYearMax = LUNAR_YEAR_MAX;
}


// ********************************************************************
//声明：BOOL	Lunar_CheckYear( WORD wYear )
//参数：
//	IN wYear - 待检查的年
//返回值：
//	TRUE表示在该范围内
//功能描述：检查wYear年，是否在该农历算法的年范围内
//引用: 
// ********************************************************************
BOOL	Lunar_CheckYear( WORD wYear )
{
	if( wYear>=LUNAR_YEAR_MIN && wYear<=LUNAR_YEAR_MAX )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

