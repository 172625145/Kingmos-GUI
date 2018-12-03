/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：图形设备驱动程序(BLT & PAT)
版本号：1.0.0
开发时期：2005-9-15
作者：李林
修改记录：
     

******************************************************/

#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif
 // 用bitmap刷子填充
static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPWORD lpDestStart, lpDest;
//    LPCBYTE lpPattern;
//    WORD clrFore, clrBack;
    int i, j, n,  rows, cols, shift;
//    BYTE mask, bitMask;
	int iDestWidthBytes = lpData->lpDestImage->bmWidthBytes;
	_LPBITMAPDATA lpSrc;
	UINT srcWidth, srcHeight;
	int iSrcWidthBytes;
	LPCBYTE lpSrcStart;

    lpDestStart = (LPWORD)( lpData->lpDestImage->bmBits + lpData->lprcDest->top * iDestWidthBytes ) + lpData->lprcDest->left;
    lpSrc = lpData->lpBrush->lpbmpBrush;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
	srcWidth = lpSrc->bmWidth;
	srcHeight = lpSrc->bmHeight;
	lpSrcStart = lpSrc->bmBits;
	iSrcWidthBytes = lpSrc->bmWidthBytes;
    //shift = ( lpData->lprcMask->left + lpData->lpBrush->origin.x ) % srcWidth;
	if( lpData->lpptBrushOrg )
	{
		//shift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % srcWidth;// + iWidthPat) % iWidthPat; 
		//n = ( lpData->lprcMask->top - lpData->lpptBrushOrg->y );
		shift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % srcWidth;// + iWidthPat) % iWidthPat; 
		n = ( lpData->lprcDest->top - lpData->lpptBrushOrg->y );
	}
	else
	{
		n = shift = 0;
	}
    //iTopOffset = (iHeightPat - pParms->pptlBrush->y % iHeightPat) % iHeightPat;
//

    //clrFore = (WORD)lpData->lpBrush->color;
    //clrBack = (WORD)lpData->solidBkColor;
    for( i = 0; i < rows; i++ )
    {
		LPCWORD lpSrcLineStart, lpSrcLineEnd, lpSrcLine;
        lpSrcLineStart = (LPCWORD)( lpSrcStart + (n % srcHeight) * iSrcWidthBytes );
		lpSrcLineEnd = (LPCWORD)( lpSrcLineStart + srcWidth );
		n++; //LN 2003-05-06,增加
        //bitMask = 0x80 >> shift;
        lpDest = lpDestStart;
		lpSrcLine = lpSrcLineStart + shift;
        for( j = 0; j < cols; j++ )
        {
			*lpDest++ = *lpSrcLine++;
			if( lpSrcLine >= lpSrcLineEnd )
				lpSrcLine = lpSrcLineStart;            
        }
        lpDestStart = (LPWORD)( (LPBYTE)lpDestStart + iDestWidthBytes );
    }
    return TRUE;
}

