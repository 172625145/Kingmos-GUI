#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    DWORD scanDWords = lpData->lpDestImage->bmWidthBytes >> 2;
    DWORD destDWords = ( (lpData->lprcDest->right - 1) >> 3 ) - ( lpData->lprcDest->left >> 3 ) + 1;
    LPDWORD lpdwDstStart = (LPDWORD)lpData->lpDestImage->bmBits +
                                scanDWords * lpData->lprcDest->top +
                                (lpData->lprcDest->left >> 3);
    DWORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    DWORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    LPCBYTE lpPattern = lpData->lpBrush->pattern;
    DWORD clrFore, clrBack, color;
	int xShift, yShift;
	register WORD Temp;

    if( destDWords == 1 )
        leftMask &= rightMask;
    clrFore = dwPalette[lpData->lpBrush->color&PAL_INDEX_MASK];
    clrBack = dwPalette[lpData->solidBkColor&PAL_INDEX_MASK];
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

    if( leftMask != 0xffffffff )
    {
        register LPDWORD lpdwDst = lpdwDstStart;
        register int m;//, n = lpData->lprcDest->top;
        register DWORD pattern;

        for( m = 0; m < height; m++, yShift++, lpdwDst += scanDWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = (WORD)( ( ((WORD)pattern)<<8 ) | pattern );
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );
            color = (clrFore & pattern) | (clrBack & ~pattern);
            *lpdwDst = (*lpdwDst & ~leftMask) |
                     ( BLT_ROP(*lpdwDst,color) & leftMask );
        }
        destDWords--;
        lpdwDstStart++;
    }
    // fill right bytes if posible
    if( rightMask != 0xffffffff && destDWords > 0 )
    {
        register LPDWORD lpdwDst = lpdwDstStart + destDWords - 1;
        register int m;//, n = lpData->lprcDest->top;
        register DWORD pattern;
        for( m = 0; m < height; m++, yShift++, lpdwDst += scanDWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = (WORD)( ( ((WORD)pattern)<<8 ) | pattern );
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );

            color = (clrFore & pattern) | (clrBack & ~pattern);
            *lpdwDst = (*lpdwDst & ~rightMask) |
                     ( BLT_ROP(*lpdwDst,color) & rightMask );
        }
        destDWords--;
    }

    // fill middle bytes
    if( destDWords > 0 )
    {
        register LPDWORD lpdwDst;
        register int n, m;//, k = lpData->lprcDest->top;
        register DWORD pattern;

        for( m = 0; m < height; m++, yShift++ )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = (WORD)( ( ((WORD)pattern)<<8 ) | pattern );
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );
            lpdwDst = lpdwDstStart;
            color = (clrFore & pattern) | (clrBack & ~pattern);
            // line copy
            for( n = 0; (DWORD)n < destDWords; n++, lpdwDst++ )
            {
                *lpdwDst = BLT_ROP( *lpdwDst, color );
            }

            lpdwDstStart += scanDWords;
        }
    }
    return TRUE;
}
