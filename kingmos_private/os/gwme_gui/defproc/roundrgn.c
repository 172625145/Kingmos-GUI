/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：对话框 边框管理, 这里是产生一个圆角 矩形
版本号：1.0.0
开发时期：2005
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <edialog.h>

static const int width[] = {
	6,		//第1行点数  , left - top corner 
	4,		//第2行点数  , left - top corner 
	3,		//第3行点数  , left - top corner 
	2,		//第4行点数  , left - top corner 
	1,		//第5行点数  , left - top corner 
    1		//第6行点数  , left - top corner 
};
//static const RECT rcRound[] = {
//	{0, 0, 5, 1},   
//	{0, 1, 3, 2},   // left - top corner 
//	{0, 2, 2, 3},   // left - top corner 
//	{0, 3, 1, 5},   // left - top corner 
//};


VOID _SetRoundWindowFrame( HWND hWnd )
{
	RECT rcWindow;
//	RECT rcDiff;
	HRGN hrgn, hrgnDiff;
	int row;
	int yOffset;

	GetWindowRect( hWnd, &rcWindow );
	OffsetRect( &rcWindow, -rcWindow.left, -rcWindow.top );  //归零
	hrgn = CreateRectRgn( rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom );

	//去掉 left - top corner 
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( 0, row, width[row], row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}
	
	//去掉 left - bottom corner 
/*	
	yOffset = rcWindow.bottom - 1;
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( 0,  yOffset - row, width[row], yOffset - row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}
*/	

	//去掉 right - top corner 
	//xOffset = rcWindow.right;
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( rcWindow.right - width[row],  row, rcWindow.right,  row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}

	//去掉 right - bottom corner 
	/*
	yOffset = rcWindow.bottom - 1;
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( rcWindow.right - width[row],  yOffset - row, rcWindow.right, yOffset - row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}
	*/

	SetWindowRgn( hWnd, hrgn, FALSE );
}
