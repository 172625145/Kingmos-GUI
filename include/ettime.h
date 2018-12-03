/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ETTIME_H
#define __ETTIME_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

void DateAddDay( LPSYSTEMTIME lpdt, int iDays );
void DateAddHour( LPSYSTEMTIME lpdt, int iHours );
void DateAddMin( LPSYSTEMTIME lpdt, int iMins );
void DateAddSecond( LPSYSTEMTIME lpdt, int iSecs );

void GetDateTime( LPSYSTEMTIME lpdt );
void SetDateTime( const SYSTEMTIME * lpDateTime );
// 1: lpdt1 > lpdt2; -1: lpdt1 < lpdt2; 0: lpdt1 == lpdt2
int CompareDateTime( const SYSTEMTIME * lpdt1, const SYSTEMTIME * lpdt2 );
// days = lpdtTo - lpdtFrom
int DateDiff( const SYSTEMTIME * lpdtFrom, const SYSTEMTIME * lpdtTo );

BOOL IsLeap( WORD iYear );
WORD GetDayofTheMonth( WORD iYear,WORD iMonth );
WORD GetWeekData( WORD iYear,WORD iMonth,WORD iDay );

//功能： 根据已知的每月指定日子(uiDayBase)是星期几(wDayWeekOk)，
//算出该月指定日子(uiTargetDay)是星期几(返回值)
#define GetDayWeekFromDayWeek( uiDayBase, uiWeekBase, uiTargetDay ) \
	        ( (uiTargetDay)>(uiDayBase) ? \
              ( ( (uiWeekBase) + (uiTargetDay) - (uiDayBase) ) % 7 ) : \
              ( ( (uiWeekBase) + (uiDayBase) - (uiTargetDay) ) % 7 ) )

//功能： 根据已知的每月1号是星期几(uiWeekBase)，算出该月指定日子(uiTargetDay)是星期几(返回值)
#define GetDayWeekFromFirst( uiWeekBase, uiTargetDay ) GetDayWeekFromDayWeek( 1, (uiWeekBase), (uiTargetDay) )


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif //__ETTIME_H
