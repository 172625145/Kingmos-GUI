/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：处理矩形框的拖动
版本号：3.0.0
开发时期：2003-03-06
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <eassert.h>
//#include <eapisrv.h>
//#include <winsrv.h>
//#include <gdisrv.h>



// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL TrackRect( HWND hWnd, POINT ptOld,
	              HWND hwndClipTo, LPRECT lprc )
{
    RECT rcNew = *lprc;
    HBRUSH hOldBrush;
    int rop2;
    int xoff, yoff;
    BOOL bEraseTrack;
    HWND hwndTrack;
    HDC hdcTrack;
    RECT rcTrack;
	MSG msg;
    
    bEraseTrack = TRUE;
    if ( GetCapture() != NULL)
		return FALSE;

    //if( hwndClipTo )
	UpdateWindow( hwndClipTo );	
	UpdateWindow( hWnd );
	
    hwndTrack = hWnd;
    SetCapture(hwndTrack);

	hdcTrack = GetDC(hwndClipTo);

    hOldBrush = SelectObject( hdcTrack, GetStockObject(NULL_BRUSH) );
    rop2 = SetROP2( hdcTrack, R2_NOT );

    Rectangle( hdcTrack, rcNew.left, rcNew.top, rcNew.right, rcNew.bottom );

    rcTrack = rcNew;
    bEraseTrack= FALSE;

	while( 1 )
	{
        //if( _posEvent.Peek( &msg, PM_REMOVE ) )
        //if( PeekMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE ) )
		//if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
		    //if( GetMessage( &msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST) == 0 )
			if( GetMessage( &msg, NULL, NULL, NULL ) == 0 )
			//if( GetMessage( &msg, NULL, 0, 0) == 0 )
            {
                ASSERT( 0 );
	            break;
            }
    		if( GetCapture() != hWnd )
            {
	            break;
            }
		    switch (msg.message)
		    {
		    case WM_LBUTTONUP:
		    case WM_MOUSEMOVE:
                // erase rectangle            
                Rectangle( hdcTrack, rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.bottom );
                bEraseTrack = TRUE;
                xoff = msg.pt.x - ptOld.x;
                yoff = msg.pt.y - ptOld.y;
                rcNew = *lprc;
                OffsetRect( &rcNew, xoff, yoff );
                if( msg.message == WM_LBUTTONUP )
                    goto ExitLoop;
                Rectangle( hdcTrack, rcNew.left, rcNew.top, rcNew.right, rcNew.bottom );
                bEraseTrack = FALSE;
                rcTrack = rcNew;
			    break;
		    default:
    			DispatchMessage(&msg);
	    		break;
    		}
        }
	}
ExitLoop:
    if( bEraseTrack == FALSE )
    {
        Rectangle( hdcTrack, rcTrack.left, rcTrack.top, rcTrack.right, rcTrack.bottom );
        bEraseTrack = TRUE;
    }
    hwndTrack = 0;
    SelectObject( hdcTrack, hOldBrush );
    ReleaseDC( hwndClipTo, hdcTrack );
    hdcTrack = 0;

	ReleaseCapture();
    if( EqualRect(lprc, &rcNew) )
        return FALSE;
    else
    {
        *lprc = rcNew;
        return TRUE;
    }
}

