/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����ũ���㷨
�汾�ţ�  1.0.0
����ʱ�ڣ�2002.01.06
���ߣ�    kingkong and ФԶ��

�޸ģ�	  ��  ��

2003-06-10:  
    by ФԶ��
         1> ���ĺ�����  
         2> ���� �� �������С��ֵ�Ļ�ȡ��У�麯��  
         3> ���� ͷ�ļ�YMDlunar.h
         4> ԭ���ļ�����LunarCld.c���ɶ�Ӧ�Ƚϲο�
2004-09-06:ũ�������ڷ�Χ���ƣ�By Liujun��

******************************************************/

#include <ewindows.h>
#include <ymdlunar.h>

/***************  ȫ���� ���壬 ���� *****************/

//ũ���㷨����С ���ֵ
#define LUNAR_YEAR_MIN					1901
#define LUNAR_YEAR_MAX					2050

//
//����gLunarDay��������1901�굽2100��ÿ���е���������Ϣ��
//����ÿ��ֻ����29��30�죬һ����12����13����������λ��ʾ����ӦλΪ1��30�죬����Ϊ29��
//ÿ����·ݴӸ�λ��ʼ,�����λ��һ��,��֮���� �ȵ�.......
//��������ֻ��1901.2.19 --2050.12.31
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
//����gLanarMonth�������1901�굽2050�����µ��·ݣ���û����Ϊ0��ÿ�ֽڴ�����
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
	"��һ", "����", "����", "����", "����",
    "����", "����", "����", "����", "��ʮ",
    "ʮһ", "ʮ��", "ʮ��", "ʮ��", "ʮ��",
    "ʮ��", "ʮ��", "ʮ��", "ʮ��", "��ʮ",
    "إһ", "إ��", "إ��", "إ��", "إ��",
    "إ��", "إ��", "إ��", "إ��", "��ʮ"
};
static const TCHAR szMonths[12][10]=
{   
	"��", "��", "��", "��", "��", "��", "��", "��", "��", "ʮ",
    "��", "��"
};

#define	ARR_COUNT( arr )	(sizeof(arr)/sizeof(arr[0]))

//---------------------------------------------------
static	WORD	Lunar_GetDaysOfYear( WORD wYear );
static	WORD	Lunar_GetYearByDays( WORD wDays, WORD *pwDayYear );
static	WORD	Lunar_GetMonthByDays( WORD wYear,WORD wDays, WORD *pwDays, BOOL *pbLeap );

/******************************************************/


//---------------------------------------------------
// ********************************************************************
//������WORD	Lunar_GetDaysOfYear( WORD wYear )
//������
//	IN wYear - ָ������
//����ֵ��
//	�ɹ�����ָ�������������
//������������ȡָ�������������
//����: 
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
//������WORD	Lunar_GetYearByDays( WORD wDays, WORD *pwDayYear )
//������
//	IN wDays - ָ��������
//	OUT pwDayYear - ����õ�����
//����ֵ��
//	�ɹ�����ָ����������ȷ������
//��������������ָ������������ȡ�õ����꣬������
//����: 
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
//������WORD	Lunar_GetMonthByDays( WORD wYear, WORD wDays, WORD *pwDays, BOOL *pbLeap )
//������
//	IN wYear - ָ������
//	IN wDays - ָ��������
//	OUT pwDays - ���������µ�����
//	OUT pbLeap - �����������Ƿ�Ϊ��2���£������£�
//����ֵ��
//	�ɹ��������ָ����������ȷ������
//��������������ȷ�����¡����µ������������Ƿ�Ϊ��2����
//����: 
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
//������WORD	Lunar_GetLeapMonth( WORD wYear )
//������
//	IN wYear - ������ũ����
//����ֵ��
//	�õ���������£����û�оͷ���0
//�����������õ���������£����û�оͷ���0
//����: 
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
//������WORD	Lunar_GetDaysOfMonth( WORD wYear, WORD wMonth, BOOL bLeap )
//������
//	IN wYear - ������ũ����
//	IN wMonth - ������ũ����
//	IN bLeap - ������������Ƿ�Ϊ���£���2���£�
//����ֵ��
//	�õ������µ�����
//�����������õ������µ�����
//����: 
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
	if ( (wMonth<1)||(wMonth>12) )	// ��֤�·ݵ���ȷ��
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
//������BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap )
//������
//	IN wYear - �����Ĺ�����
//	IN wMonth - �����Ĺ�����
//	IN wDay - �����Ĺ�����
//	IN pwLYear - ����õ���ũ����
//	IN pwLMonth - ����õ���ũ����
//	IN pwLDay - ����õ���ũ����
//	IN pbLeap - ����õ����Ƿ�ũ������
//����ֵ��
//	TRUE��ʾ����ɹ�
//�������������ݹ��������գ��������Ӧ��ũ�������գ����Ƿ�ũ������
//����: 
// ********************************************************************
BOOL	Lunar_GetYMDFromAD( WORD wYear, WORD wMonth, WORD wDay, WORD *pwLYear, WORD *pwLMonth, WORD *pwLDay, BOOL *pbLeap )
{
	WORD			wAllDays=0 ;
	WORD			wLunarYear, wLunarMonth, wLunarDays, wLunarMonthDays ;
	WORD			i;

	if( (wYear>LUNAR_YEAR_MAX)||(wYear<LUNAR_YEAR_MIN) )		// �������
	{
		return FALSE ;
	}
	if( (wYear==LUNAR_YEAR_MIN)&&(wMonth==2)&&(wDay<=18) )		// �����������
	{
		return FALSE ;
	}
	if( (wYear==LUNAR_YEAR_MIN)&&(wMonth==1) )					// �����������
	{
		return FALSE;
	}
	if ( (wMonth<1||wMonth>12)||(wDay<1||wDay>31) )				// �º�������
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
//������BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] )
//������
//	IN wYear - �����Ĺ�����
//	IN wMonth - �����Ĺ�����
//	IN wDaysOfMonth - �����Ĺ������е�����
//	IN arrLunarYMD - ����õ���ũ�������յ�����
//����ֵ��
//	TRUE��ʾ����ɹ�
//�������������ݹ������º�����µ�������������ڸù������¶�Ӧ��ũ��������
//���ã�
//
// ********************************************************************
BOOL	Lunar_GetYMDsFromADMonth( WORD wYear, WORD wMonth, WORD wDaysOfMonth, WORD arrLunarYMD[][3] )
{
	BOOL	bLeap;
	WORD	wDay;

	if ( (wYear<LUNAR_YEAR_MIN)||(wYear>LUNAR_YEAR_MAX) )	// �������
	{
		return FALSE;
	}

	if ( (wYear==LUNAR_YEAR_MIN)&&(wMonth<2) )				// �ٽ�ֵ���ж�
	{
		return FALSE;
	}

	if ( (wDaysOfMonth<1)||(wDaysOfMonth>31) )				// �յ�����
	{
		return FALSE;
	}

	if ( (wMonth<1)||(wMonth>12) )							// �µ�����
	{
		return FALSE;
	}

	if ( (wYear==LUNAR_YEAR_MIN)&&(wMonth==2) )				// �ٽ�ֵ��ֵ
	{
		for( wDay=1; wDay<19; wDay++ )
		{
			arrLunarYMD[wDay][0] = 0;	// ��
			arrLunarYMD[wDay][1] = 0;	// ��
			arrLunarYMD[wDay][2] = 0;	// ��
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
{ "��", "��", "��", "��", "��", "��", "��", "��", "��", "��" };
static const TCHAR c2[][3] = 
{ "��", "��", "��", "î", "��", "��", "��", "δ", "��", "��", "��", "��" };

// ********************************************************************
//������void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp )
//������
//	IN wYear - ������ũ����
//	IN pszTemp - ����õ���ũ����ļ���String
//����ֵ��
//	��
//�����������õ�����ļ��ӱ�ʾ
//����: 
// ********************************************************************
void	Lunar_GetNameYear( WORD wYear, LPTSTR pszTemp )
{
	int x, y, ya ;

	if ( (wYear<LUNAR_YEAR_MIN)||(wYear>LUNAR_YEAR_MAX) )	// �������
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
//������void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp )
//������
//	IN wMonth - ������ũ����
//	IN pszTemp - ����õ���ũ���µ����ı�ű�ʾString
//����ֵ��
//	��
//�����������õ����µ����ı�ű�ʾ����������������... ʮ������������
//����: 
// ********************************************************************
void	Lunar_GetNameMonth( WORD wMonth, LPTSTR pszTemp )
{
	if ( (wMonth<1)||(wMonth>12) )		// �µ�����
	{
		return ;
	}

	ASSERT( wMonth<=ARR_COUNT(szMonths) );
	sprintf( pszTemp,"%s��",szMonths[wMonth-1] ) ;
}

// ********************************************************************
//������void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp )
//������
//	IN wMonth - ������ũ����
//	IN wDay - ������ũ����
//	IN pszTemp - ����õ���ũ���յ����ı�ű�ʾString
//����ֵ��
//	��
//�����������õ�����ָ���յ����ı�ű�ʾ������һ ... ʮ�� ... ��ʮ����
//����: 
// ********************************************************************
void	Lunar_GetNameDay( WORD wMonth, WORD wDay, LPTSTR pszTemp )
{
	if ( (wMonth<1)||(wMonth>12) )		// �µ�����
		return ;

	if ( (wDay<1)||(wDay>30) )			// �յ�����
		return ;

	if( !wMonth && (wDay == 1) )
	{
		sprintf( pszTemp,"%s��",szMonths[wMonth-1] ) ;
	}
	else
	{
		ASSERT( wDay<=ARR_COUNT(szDays) );
		strcpy( pszTemp, szDays[wDay-1] );
	}
}

// ********************************************************************
//������void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, LPTSTR pszTemp )
//������
//	IN wYear - ������ũ����
//	IN wMonth - ������ũ����
//	IN wDay - ������ũ����
//	IN pszTemp - ����õ���ũ�������յ����ı�ű�ʾString
//����ֵ��
//	��
//�����������õ��������յ����ı�ű�ʾ���������� ���� ��һ����
//����: 
// ********************************************************************
void	Lunar_GetNameYMD( WORD wYear, WORD wMonth, WORD wDay, LPTSTR pszTemp )
{
	TCHAR	pszYear[12];

	if ( (wYear<LUNAR_YEAR_MIN)||(wYear>LUNAR_YEAR_MAX) )	// �������
	{
		return ;
	}
	if ( (wMonth<1)||(wMonth>12) )							// �µ�����
	{
		return ;
	}

	ASSERT( wMonth<=ARR_COUNT(szMonths) );
	ASSERT( wDay<=ARR_COUNT(szDays) );
	Lunar_GetNameYear( wYear, pszYear );
	sprintf( pszTemp,"%s�� %s�� %s", pszYear, szMonths[wMonth-1], szDays[wDay-1] );
}


// ********************************************************************
//������void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax )
//������
//	IN pwYearMin - ���������
//	IN pwYearMax - ���������
//����ֵ��
//	��
//������������ȡ��ũ���㷨���귶Χ
//����: 
// ********************************************************************
void	Lunar_GetYearRange( WORD* pwYearMin, WORD* pwYearMax )
{
	ASSERT( pwYearMin!= NULL);
	ASSERT( pwYearMax!= NULL);
	*pwYearMin = LUNAR_YEAR_MIN;
	*pwYearMax = LUNAR_YEAR_MAX;
}


// ********************************************************************
//������BOOL	Lunar_CheckYear( WORD wYear )
//������
//	IN wYear - ��������
//����ֵ��
//	TRUE��ʾ�ڸ÷�Χ��
//�������������wYear�꣬�Ƿ��ڸ�ũ���㷨���귶Χ��
//����: 
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

