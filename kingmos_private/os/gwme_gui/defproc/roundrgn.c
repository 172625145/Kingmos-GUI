/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵�����Ի��� �߿����, �����ǲ���һ��Բ�� ����
�汾�ţ�1.0.0
����ʱ�ڣ�2005
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <eassert.h>
#include <edialog.h>

static const int width[] = {
	6,		//��1�е���  , left - top corner 
	4,		//��2�е���  , left - top corner 
	3,		//��3�е���  , left - top corner 
	2,		//��4�е���  , left - top corner 
	1,		//��5�е���  , left - top corner 
    1		//��6�е���  , left - top corner 
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
	OffsetRect( &rcWindow, -rcWindow.left, -rcWindow.top );  //����
	hrgn = CreateRectRgn( rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom );

	//ȥ�� left - top corner 
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( 0, row, width[row], row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}
	
	//ȥ�� left - bottom corner 
/*	
	yOffset = rcWindow.bottom - 1;
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( 0,  yOffset - row, width[row], yOffset - row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}
*/	

	//ȥ�� right - top corner 
	//xOffset = rcWindow.right;
	for( row = 0; row < sizeof( width ) / sizeof( int ); row++ )
	{
		hrgnDiff = CreateRectRgn( rcWindow.right - width[row],  row, rcWindow.right,  row + 1 );
		CombineRgn( hrgn, hrgn, hrgnDiff, RGN_DIFF );
	}

	//ȥ�� right - bottom corner 
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
