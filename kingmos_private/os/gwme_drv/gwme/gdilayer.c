/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：Layered Window 风格的功能
版本号：3.0.0
开发时期：2005-10-14
作者：李林
修改记录：

******************************************************/

#include <eframe.h>
#include <efile.h>
#include <eassert.h>
#include <enls.h>
#include <eapisrv.h>
#include <gwmeobj.h>
#include <gdc.h>
#include <bheap.h>
#include <epwin.h>
#include <eprgn.h>
#include <winsrv.h>
#include <gdisrv.h>

BOOL ResetDCData( HDC hMemDC, LPRECT lprc )
{
   _LPGDCDATA lpdc = _GetHDCPtr( hMemDC );
	if( lpdc )
	{
        lpdc->deviceOrg.x = lprc->left;
        lpdc->deviceOrg.y = lprc->top;
        lpdc->deviceExt.cx = lprc->right - lprc->left;
        lpdc->deviceExt.cy = lprc->bottom - lprc->top;
		SetRectRgn( lpdc->hrgn, lprc->left, lprc->top, lprc->right, lprc->bottom );
		return TRUE;
	}
	return FALSE;
}


HBITMAP SwitchMainFrameBuffer( HDC hdc, HBITMAP hFrameBuffer )
{
	_LPGDCDATA lpdc = _GetHDCPtr( hdc );
	if( lpdc )
	{
		HBITMAP hbmpOld = lpdc->hBitmap;// hbmpCurrentFrameBuffer;
		lpdc->hBitmap = hFrameBuffer;
		return hbmpOld;
	}
	return NULL;	
}
