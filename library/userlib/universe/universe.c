/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：
版本号：1.0.0
开发时期：2002-06-10
作者：李林
修改记录：
******************************************************/

#include <ewindows.h>

void DrawBitMap(HDC hdc,HBITMAP hBitMap,RECT rect,UINT xOffset,UINT yOffset,DWORD dwRop)
{
	HDC hMemoryDC;
	if(hBitMap==0)
		return;
	hMemoryDC=CreateCompatibleDC(hdc);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	BitBlt( hdc, (short)rect.left,(short)rect.top,(short)(rect.right-rect.left),
		(short)(rect.bottom-rect.top),hMemoryDC,(short)xOffset,(short)yOffset,dwRop);
	hBitMap = SelectObject(hMemoryDC,hBitMap);
	DeleteDC(hMemoryDC);
}
/*
int GetGapDays( SYSTEMTIME dt1, SYSTEMTIME dt2 )
{
	int			iDays = 0, iStartDays = 0, iEndDays = 0;
	WORD		i ; 
	for( i = dt1.wMonth ; i <= 12  ; i ++ )
	{
		iStartDays += GetDayofTheMonth( dt1.wYear, i );
	}
	iStartDays -= dt1.wDay ;

	for( i = 1 ; i < dt2.wMonth ; i ++ )
	{
		iEndDays += GetDayofTheMonth( dt2.wYear, i );
	}
	iEndDays += dt2.wDay ;
	for( i = dt1.wYear + 1 ; i < dt2.wYear ; i ++ )
	{
		iDays += IsLeap( i ) ? 366 : 365 ;
	}
	if( dt1.wYear != dt2.wYear )
		return ( iDays+iStartDays+iEndDays );
	else
		return ( iStartDays+iEndDays - ( IsLeap( dt1.wYear )?366:365 ) );
}
*/



