/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����Layered Window ���Ĺ���
�汾�ţ�3.0.0
����ʱ�ڣ�2005-10-14
���ߣ�����
�޸ļ�¼��

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
