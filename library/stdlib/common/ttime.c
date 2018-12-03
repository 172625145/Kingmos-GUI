/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：日期时间功能
版本号：1.0.0
开发时期：2000-01-20
作者：李林
修改记录：
    2004-04-17：增加说明
******************************************************/

#include <ewindows.h> 
#include <ettime.h>

static const char iMaxDayArray[]={ 0,31,28,31,30,31,30,31,31,30,31,30,31 };

// *****************************************************************
//声明：void GetDateTime( LPSYSTEMTIME lpdt )
//参数：
//  OUT lpdt-SYSTEMTIME 日期时间

//返回值：
//	无
//功能描述：得到系统 年月日时分秒，星期几
//引用: 
// *****************************************************************

void GetDateTime( LPSYSTEMTIME lpdt )
{
	GetLocalTime(lpdt);
}

// *****************************************************************
//声明：void SetDateTime( LPSYSTEMTIME lpdt )
//参数：
//	IN  lpdt-SYSTEMTIME 日期时间

//返回值：
//   无    
//功能描述：设置系统 年月日时分秒
//引用: 
// *****************************************************************

void SetDateTime( const SYSTEMTIME * lpdt )
{
	SetLocalTime( lpdt );
}

// *****************************************************************
//声明：BOOL IsLeap( WORD wYear )
//参数：
//	IN  wYear-年
//返回值：
//	TRUE，是润年;FALSE，不是润年
//功能描述：确定是否是润年
//引用: 
// *****************************************************************

BOOL IsLeap( WORD wYear )
{
    if( ( (wYear%4)==0 && (wYear%100)!=0 ) || 
		( (wYear%100)==0 && (wYear%400)==0 ) )
        return TRUE;
    return FALSE;
}

// *****************************************************************
//声明：WORD GetDayofTheMonth( WORD wYear,WORD wMonth )
//参数：
//	IN  wYear-年
//  IN  wMonth-月
//返回值：
//	返回某年某月的天数
//功能描述：
//引用: 
// *****************************************************************
WORD GetDayofTheMonth( WORD wYear,WORD wMonth )
{
    if( wMonth==2 )
		return IsLeap(wYear) ? 29 : 28; 
    else
		return iMaxDayArray[wMonth];
}

// *****************************************************************
//声明：WORD GetWeekData( WORD wYear,WORD wMonth,WORD wDay )
//参数：
//	IN  wYear-年
//	IN  wMonth-月
//	IN  wDay-日
//返回值：
//	星期几
//功能描述：
//  计算从1980年开始，1980年元月1日是星期二
//  或者计算从1901年开始, 1901年元月一日也是星期二
//  要得到更早的年份,请参考如下方式加入!!
//引用: 
// *****************************************************************

WORD GetWeekData( WORD wYear, WORD wMonth, WORD wDay )
{
  DWORD iWeek,days;
  int i,j,k;//,m;

//2001.12.25 add by kingkong
	days=0;
	if( wYear >= 1980 )
	{
	    i=1980;
		while(i<wYear)
		{
			days+=IsLeap((short)i)?366:365;
	        i++;
		}
	}
	else
	{
	    i=1901;
		while(i<wYear)
		{
			days+=IsLeap((short)i)?366:365;
	        i++;
		}
	}
	//----------------------------------------by kingkong
//		days = (wYear-1900)*365L+(wYear-1897)/4 ;		// Get	( 1900年---前一年)总天数
	k = (days+2)%7;								//	剩余几天->k
	  
	for ( j	=1 ; j<wMonth ; j++ )				//	当年1至m-1月总天数->k
		k += GetDayofTheMonth(wYear,(short)j);
	k += (wDay-1) ;		//	( k+当月天数)->k
	iWeek = k%7 ;				//  求本日为星期几
	return ( WORD )iWeek;
}

// *****************************************************************
//声明：void DateAddDays( LPSYSTEMTIME lpDateTime, int iDays )
//参数：
//	IN/OUT lpDateTime-日期时间结构
//  IN iDays-增加几天
//返回值：
//	无
//功能描述：
//  求出日期增加n天后的日期
//引用: 
// *****************************************************************

void DateAddDay( LPSYSTEMTIME lpDateTime, int iDays )
{
	int			iMonthDay;
//	WORD		wMonths;
	int			iAllDays;
	WORD		i;
	iAllDays = lpDateTime->wDay + iDays ;
	if( iAllDays > 0 )
	{
		for( i = lpDateTime->wMonth ; i <= 12 ; )
		{
			iMonthDay = GetDayofTheMonth( lpDateTime->wYear, i );
			if( iMonthDay >= iAllDays )
				break;
			if( i == 12 )
			{
				lpDateTime->wYear ++ ;
				i = 0 ;
			}
			i ++ ;
			iAllDays -= iMonthDay ;
		}
		lpDateTime->wMonth = i ;
		lpDateTime->wDay = iAllDays ;
	}
	else
	{	
		if( lpDateTime->wMonth == 1 )
		{
			lpDateTime->wMonth = 13 ;
			lpDateTime->wYear -- ;
		}
		for( i = lpDateTime->wMonth - 1 ; i >= 1 ; )
		{
			iMonthDay = GetDayofTheMonth( lpDateTime->wYear, i );
			if( ( iAllDays + iMonthDay ) > 0 )
			{
				iAllDays += iMonthDay ;
				break;
			}
			if( i == 12 )
			{
				lpDateTime->wYear -- ;
				i = 13 ;
			}
			i -- ;
			iAllDays += iMonthDay ;
		}
		lpDateTime->wMonth = i ;
		lpDateTime->wDay = iAllDays ;
	}
	lpDateTime->wDayOfWeek = GetWeekData( lpDateTime->wYear, lpDateTime->wMonth, lpDateTime->wDay );
}

// *****************************************************************
//声明：void DateAddHour( LPSYSTEMTIME lpdt, int wHour )
//参数：
//	IN/OUT  lpdt-日期时间结构
//  IN  wHour-小时

//返回值：
//	无
//功能描述：
//  求出日期增加n小时后的日期
//引用: 
// *****************************************************************

void DateAddHour( LPSYSTEMTIME lpdt, int wHour )
{
	int		iAddHour;
	iAddHour = lpdt->wHour + wHour ;
	if( iAddHour >= 0 )
	{
		DateAddDay( lpdt, (int)iAddHour/24 );
		lpdt->wHour = iAddHour % 24 ;
	}else
	{
		lpdt->wHour = iAddHour + 24 ;
		DateAddDay( lpdt, -1 );
	}
}

// *****************************************************************
//声明：void DateAddMin( LPSYSTEMTIME lpdt, int wHour )
//参数：
//	IN/OUT  lpdt-日期时间结构
//  IN  wMinute-分钟
//返回值：
//	无
//功能描述：
//  求出日期增加n分钟后的日期
//引用: 
// *****************************************************************

void DateAddMin( LPSYSTEMTIME lpdt, int wMinute )
{
	int		iAddMin;
	ASSERT( ( wMinute > -60 )&&( wMinute < 60 ) ) ;
	iAddMin = lpdt->wMinute + wMinute ;
	if( iAddMin >= 0 )
	{
		DateAddHour( lpdt, (int)iAddMin/60 );
		lpdt->wMinute = iAddMin % 60 ;
	}else
	{
		DateAddHour( lpdt, -1 );
		lpdt->wMinute = iAddMin + 60 ;
	}
}

// *****************************************************************
//声明：void DateAddSecond( LPSYSTEMTIME lpdt, int wSecond )
//参数：
//	IN/OUT  lpdt-日期时间结构
//  IN  wSecond-秒
//返回值：
//	无
//功能描述：
//  求出日期增加n秒后的日期
//引用: 
// *****************************************************************

void DateAddSecond( LPSYSTEMTIME lpdt, int wSecond )
{
	int		iAddSec;
	iAddSec = lpdt->wSecond + wSecond ;
	if( iAddSec >= 0 )
	{
		DateAddMin( lpdt, (int)iAddSec/60 );
		lpdt->wSecond = iAddSec % 60 ;
	}else
	{
		DateAddMin( lpdt, -1 );
		lpdt->wSecond = iAddSec + 60 ;
	}
}

// *****************************************************************
//声明：int CompareTime( const SYSTEMTIME * lpFirst, const SYSTEMTIME * lpSecond )
//参数：
//	IN  lpFirst-日期时间结构
//	IN  lpSecond-日期时间结构
//返回值：
// 1: lpFirst > lpSecond; -1: lpFirst < lpSecond; 0: lpFirst == lpSecond
//功能描述：
//  比较两个日期时间的大小
//引用: 
// *****************************************************************

int CompareDateTime( const SYSTEMTIME * lpFirst, const SYSTEMTIME * lpSecond )
{
	if( lpFirst->wYear > lpSecond->wYear )
		return 1 ;
	if( lpFirst->wYear < lpSecond->wYear )
		return -1 ;
	if( lpFirst->wMonth > lpSecond->wMonth )
		return 1 ;
	if( lpFirst->wMonth < lpSecond->wMonth )
		return -1 ;
	if( lpFirst->wDay > lpSecond->wDay )
		return 1 ;
	if( lpFirst->wDay < lpSecond->wDay )
		return -1 ;
	if( lpFirst->wHour > lpSecond->wHour )
		return 1;
	if( lpFirst->wHour < lpSecond->wHour )
		return -1;
	if( lpFirst->wMinute > lpSecond->wMinute )
		return 1;
	if( lpFirst->wMinute < lpSecond->wMinute )
		return -1;
	if( lpFirst->wSecond > lpSecond->wSecond )
		return 1;
	if( lpFirst->wSecond < lpSecond->wSecond )
		return -1;
	return 0;
}

// *****************************************************************
//声明：int DateDiff( const SYSTEMTIME * lpdtFrom, const SYSTEMTIME * lpdtTo );
//参数：
//	IN  lpdtFrom-日期时间结构
//	IN  lpdtTo-日期时间结构
//返回值：
//  lpdtTo - lpdtFrom 的间隔天数
//功能描述：
//  得到两个日期之间间隔的天数
//引用: 
// *****************************************************************

int DateDiff( const SYSTEMTIME * lpdtFrom, const SYSTEMTIME * lpdtTo )
{
	int			iDays = 0, iStartDays = 0, iEndDays = 0;
	WORD		i ; 
	int iBig;

	iBig = CompareDateTime( lpdtFrom, lpdtTo );

	if( iBig == 1 )
	{   // lpdtFrom > lpdtTo, 交换
		const SYSTEMTIME * lpdt = lpdtFrom;
		lpdtFrom = lpdtTo;
		lpdtTo = lpdt;
	}
	else if( iBig == 0 )
		return 0;

	for( i = lpdtFrom->wMonth ; i <= 12  ; i ++ )
	{
		iStartDays += GetDayofTheMonth( lpdtFrom->wYear, i );
	}
	iStartDays -= lpdtFrom->wDay ;

	for( i = 1 ; i < lpdtTo->wMonth ; i ++ )
	{
		iEndDays += GetDayofTheMonth( lpdtTo->wYear, i );
	}
	iEndDays += lpdtTo->wDay ;

	for( i = lpdtFrom->wYear + 1 ; i < lpdtTo->wYear ; i++ )
	{
		iDays += IsLeap( i ) ? 366 : 365 ;
	}

	if( lpdtFrom->wYear != lpdtTo->wYear )
		iDays = ( iDays + iStartDays + iEndDays );
	else
		iDays = ( iStartDays+iEndDays - ( IsLeap( lpdtFrom->wYear )?366:365 ) );

	return (iBig == 1) ? (-iDays) : iDays;
}




