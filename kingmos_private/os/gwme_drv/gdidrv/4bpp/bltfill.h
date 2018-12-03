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
    DWORD color;

	ASSERT( scanDWords );

    if( destDWords == 1 )
        leftMask &= rightMask;
    color = dwPalette[lpData->solidColor&PAL_INDEX_MASK];

    // fill left bytes if posible
    if( leftMask != 0xffffffff )
    {
        register LPDWORD lpdwDst = lpdwDstStart;
        register int m;
        for( m = 0; m < height; m++, lpdwDst += scanDWords )
        {
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
        register int m;
        for( m = 0; m < height; m++, lpdwDst += scanDWords )
        {
            *lpdwDst = (*lpdwDst & ~rightMask) |
                     ( BLT_ROP(*lpdwDst,color) & rightMask );
        }
        destDWords--;
    }

    // fill middle bytes
    if( destDWords > 0 )
    {
        register LPDWORD lpdwDst;
        register int n, m;

        for( m = 0; m < height; m++ )
        {
            lpdwDst = lpdwDstStart;
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
