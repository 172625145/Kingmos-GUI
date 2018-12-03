/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵��������ʱ�书��
�汾�ţ�1.0.0
����ʱ�ڣ�2000-01-20
���ߣ�����
�޸ļ�¼��
    2004-04-17������˵��
******************************************************/

#include <ewindows.h> 
#include <ettime.h>

static const char iMaxDayArray[]={ 0,31,28,31,30,31,30,31,31,30,31,30,31 };

// *****************************************************************
//������void GetDateTime( LPSYSTEMTIME lpdt )
//������
//  OUT lpdt-SYSTEMTIME ����ʱ��

//����ֵ��
//	��
//�����������õ�ϵͳ ������ʱ���룬���ڼ�
//����: 
// *****************************************************************

void GetDateTime( LPSYSTEMTIME lpdt )
{
	GetLocalTime(lpdt);
}

// *****************************************************************
//������void SetDateTime( LPSYSTEMTIME lpdt )
//������
//	IN  lpdt-SYSTEMTIME ����ʱ��

//����ֵ��
//   ��    
//��������������ϵͳ ������ʱ����
//����: 
// *****************************************************************

void SetDateTime( const SYSTEMTIME * lpdt )
{
	SetLocalTime( lpdt );
}

// *****************************************************************
//������BOOL IsLeap( WORD wYear )
//������
//	IN  wYear-��
//����ֵ��
//	TRUE��������;FALSE����������
//����������ȷ���Ƿ�������
//����: 
// *****************************************************************

BOOL IsLeap( WORD wYear )
{
    if( ( (wYear%4)==0 && (wYear%100)!=0 ) || 
		( (wYear%100)==0 && (wYear%400)==0 ) )
        return TRUE;
    return FALSE;
}

// *****************************************************************
//������WORD GetDayofTheMonth( WORD wYear,WORD wMonth )
//������
//	IN  wYear-��
//  IN  wMonth-��
//����ֵ��
//	����ĳ��ĳ�µ�����
//����������
//����: 
// *****************************************************************
WORD GetDayofTheMonth( WORD wYear,WORD wMonth )
{
    if( wMonth==2 )
		return IsLeap(wYear) ? 29 : 28; 
    else
		return iMaxDayArray[wMonth];
}

// *****************************************************************
//������WORD GetWeekData( WORD wYear,WORD wMonth,WORD wDay )
//������
//	IN  wYear-��
//	IN  wMonth-��
//	IN  wDay-��
//����ֵ��
//	���ڼ�
//����������
//  �����1980�꿪ʼ��1980��Ԫ��1�������ڶ�
//  ���߼����1901�꿪ʼ, 1901��Ԫ��һ��Ҳ�����ڶ�
//  Ҫ�õ���������,��ο����·�ʽ����!!
//����: 
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
//		days = (wYear-1900)*365L+(wYear-1897)/4 ;		// Get	( 1900��---ǰһ��)������
	k = (days+2)%7;								//	ʣ�༸��->k
	  
	for ( j	=1 ; j<wMonth ; j++ )				//	����1��m-1��������->k
		k += GetDayofTheMonth(wYear,(short)j);
	k += (wDay-1) ;		//	( k+��������)->k
	iWeek = k%7 ;				//  ����Ϊ���ڼ�
	return ( WORD )iWeek;
}

// *****************************************************************
//������void DateAddDays( LPSYSTEMTIME lpDateTime, int iDays )
//������
//	IN/OUT lpDateTime-����ʱ��ṹ
//  IN iDays-���Ӽ���
//����ֵ��
//	��
//����������
//  �����������n��������
//����: 
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
//������void DateAddHour( LPSYSTEMTIME lpdt, int wHour )
//������
//	IN/OUT  lpdt-����ʱ��ṹ
//  IN  wHour-Сʱ

//����ֵ��
//	��
//����������
//  �����������nСʱ�������
//����: 
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
//������void DateAddMin( LPSYSTEMTIME lpdt, int wHour )
//������
//	IN/OUT  lpdt-����ʱ��ṹ
//  IN  wMinute-����
//����ֵ��
//	��
//����������
//  �����������n���Ӻ������
//����: 
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
//������void DateAddSecond( LPSYSTEMTIME lpdt, int wSecond )
//������
//	IN/OUT  lpdt-����ʱ��ṹ
//  IN  wSecond-��
//����ֵ��
//	��
//����������
//  �����������n��������
//����: 
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
//������int CompareTime( const SYSTEMTIME * lpFirst, const SYSTEMTIME * lpSecond )
//������
//	IN  lpFirst-����ʱ��ṹ
//	IN  lpSecond-����ʱ��ṹ
//����ֵ��
// 1: lpFirst > lpSecond; -1: lpFirst < lpSecond; 0: lpFirst == lpSecond
//����������
//  �Ƚ���������ʱ��Ĵ�С
//����: 
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
//������int DateDiff( const SYSTEMTIME * lpdtFrom, const SYSTEMTIME * lpdtTo );
//������
//	IN  lpdtFrom-����ʱ��ṹ
//	IN  lpdtTo-����ʱ��ṹ
//����ֵ��
//  lpdtTo - lpdtFrom �ļ������
//����������
//  �õ���������֮����������
//����: 
// *****************************************************************

int DateDiff( const SYSTEMTIME * lpdtFrom, const SYSTEMTIME * lpdtTo )
{
	int			iDays = 0, iStartDays = 0, iEndDays = 0;
	WORD		i ; 
	int iBig;

	iBig = CompareDateTime( lpdtFrom, lpdtTo );

	if( iBig == 1 )
	{   // lpdtFrom > lpdtTo, ����
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




