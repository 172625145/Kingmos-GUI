/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：图形设备驱动程序(BLT & PAT)
版本号：3.0.0
开发时期：2000-9-06
作者：李林
修改记录：
     1.LN 2003-05-06, Hatch brush有错误

******************************************************/

#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPDWORD lpDestStart, lpDest;
    LPCBYTE lpPattern;
    DWORD clrFore, clrBack;
    int i, j, n,  rows, cols, shift;
    BYTE mask, bitMask;
	int iDestWidthBytes = lpData->lpDestImage->bmWidthBytes;

    lpDestStart = (LPDWORD)( lpData->lpDestImage->bmBits + lpData->lprcDest->top * iDestWidthBytes ) + lpData->lprcDest->left;
    lpPattern = lpData->lpBrush->pattern;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    //shift = lpData->lprcMask->left & 0x07;
    clrFore = (DWORD)lpData->lpBrush->color;
    clrBack = (DWORD)lpData->solidBkColor;
    //n = lpData->lprcDest->top;

	if( lpData->lpptBrushOrg )
	{
		//shift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		//n = lpData->lprcMask->top - lpData->lpptBrushOrg->y;
		shift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		n = lpData->lprcDest->top - lpData->lpptBrushOrg->y;
	}
	else
	{
		shift = n = 0;
	}

    for( i = 0; i < rows; i++ )
    {
        mask = *(lpPattern+(n&0x07));
		n++;
        bitMask = 0x80 >> shift;
        lpDest = lpDestStart;
        for( j = 0; j < cols; j++ )
        {
            if( bitMask == 0 )
            {
                bitMask = 0x80;
            }
            if( mask & bitMask )
            {
                *lpDest = BLT_ROP( *lpDest, (DWORD)clrFore );
            }
            else
                *lpDest = BLT_ROP( *lpDest, (DWORD)clrBack );
            lpDest++;
            bitMask >>= 1;
        }
        lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + iDestWidthBytes );
    }
    return TRUE;
}

