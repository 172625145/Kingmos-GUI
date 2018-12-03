#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    WORD scanWords = lpData->lpDestImage->bmWidthBytes >> 1;
    WORD destWords = ( (lpData->lprcDest->right - 1) >> 3 ) - ( lpData->lprcDest->left >> 3 ) + 1;
    LPWORD lpDstStart = (LPWORD)lpData->lpDestImage->bmBits +
                                scanWords * lpData->lprcDest->top +
                                (lpData->lprcDest->left >> 3);
    WORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    WORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    LPCBYTE lpPattern = lpData->lpBrush->pattern;
    WORD clrFore, clrBack, color;
	int xShift, yShift;
	register WORD Temp;

    if( destWords == 1 )
        leftMask &= rightMask;
    clrFore = palette[lpData->lpBrush->color];
    clrBack = palette[lpData->solidBkColor];
/*
    if( lpData->lpBrush->color )
        clrFore = 0xffff;
    else
        clrFore = 0;
    if( lpData->solidBkColor )
        clrBack = 0xffff;
    else
        clrBack = 0;
*/
    // fill left bytes if posible

	if( lpData->lpptBrushOrg )
	{
		//xShift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		//yShift = lpData->lprcMask->top - lpData->lpptBrushOrg->y;
		xShift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		yShift = lpData->lprcDest->top - lpData->lpptBrushOrg->y;
	}
	else
	{
		yShift = xShift = 0;
	}

    if( leftMask != 0xffff )
    {
        register LPWORD lpDst = lpDstStart;
        register int m;//, n = lpData->lprcDest->top;
        register WORD pattern;

        for( m = 0; m < height; m++, yShift++, lpDst += scanWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );
            color = (clrFore & pattern) | (clrBack & ~pattern);
#ifdef EML_DOS
            *HIPTR(lpDst) = (*HIPTR(lpDst) & ~HIBYTE(leftMask)) |
                     ( BLT_ROP(*HIPTR(lpDst),HIBYTE(color)) & HIBYTE(leftMask) );
            *LOPTR(lpDst) = (*LOPTR(lpDst) & ~LOBYTE(leftMask)) |
                     ( BLT_ROP(*LOPTR(lpDst),LOBYTE(color)) & LOBYTE(leftMask) );
#else
            *lpDst = (*lpDst & ~leftMask) |
                     ( BLT_ROP(*lpDst,color) & leftMask );
#endif
        }
        destWords--;
        lpDstStart++;
    }
    // fill right bytes if posible
    if( rightMask != 0xffff && destWords > 0 )
    {
        register LPWORD lpDst = lpDstStart + destWords - 1;
        register int m;//, n = lpData->lprcDest->top;
        register WORD pattern;
        for( m = 0; m < height; m++, yShift++, lpDst += scanWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );

            color = (clrFore & pattern) | (clrBack & ~pattern);
#ifdef EML_DOS
            *HIPTR(lpDst) = (*HIPTR(lpDst) & ~HIBYTE(rightMask)) |
                     ( BLT_ROP(*HIPTR(lpDst),HIBYTE(color)) & HIBYTE(rightMask) );
            *LOPTR(lpDst) = (*LOPTR(lpDst) & ~LOBYTE(rightMask)) |
                     ( BLT_ROP(*LOPTR(lpDst),LOBYTE(color)) & LOBYTE(rightMask) );
#else
            *lpDst = (*lpDst & ~rightMask) |
                     ( BLT_ROP(*lpDst,color) & rightMask );
#endif
        }
        destWords--;
    }

    // fill middle bytes
    if( destWords > 0 )
    {
        register LPWORD lpDst;
        register int n, m;//, k = lpData->lprcDest->top;
        register WORD pattern;

        for( m = 0; m < height; m++, k++ )
        {
            pattern = *(lpPattern+(k&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );
            lpDst = lpDstStart;
            color = (clrFore & pattern) | (clrBack & ~pattern);
            // line copy
            for( n = 0; n < destWords; n++, lpDst++ )
            {
#ifdef EML_DOS
                *HIPTR(lpDst) = BLT_ROP( *HIPTR(lpDst), HIBYTE(color) );
                *LOPTR(lpDst) = BLT_ROP( *LOPTR(lpDst), LOBYTE(color) );
#else
                *lpDst = BLT_ROP( *lpDst, color );
#endif
            }

            lpDstStart += scanWords;
        }
    }
    return TRUE;
}
