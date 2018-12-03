#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    LPBYTE lpDstStart = lpData->lpDestImage->bmBits +
                   lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top +
                   (lpData->lprcDest->left >> 3);
    WORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    WORD destBytes = ( (lpData->lprcDest->right - 1) >> 3 ) - ( lpData->lprcDest->left >> 3 ) + 1;
    BYTE leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    BYTE rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    WORD color;

    if( destBytes == 1 )
        leftMask &= rightMask;
    if( lpData->solidColor )
        color = 0xffff;
    else
        color = 0;

    // fill left bytes if posible
    if( leftMask != 0xff )
    {
        register LPBYTE lpDst = lpDstStart;
        register int m;
        for( m = 0; m < height; m++, lpDst += scanBytes )
            *lpDst = (*lpDst & ~leftMask) |
                     ( BLT_ROP(*lpDst,color) & leftMask );
        destBytes--;
        lpDstStart++;
    }
    // fill right bytes if posible
    if( rightMask != 0xff && destBytes > 0 )
    {
        register LPBYTE lpDst = lpDstStart + destBytes - 1;
        register int m;
        for( m = 0; m < height; m++, lpDst += scanBytes )
            *lpDst = (*lpDst & ~rightMask) |
                     ( BLT_ROP(*lpDst,color) & rightMask );
        destBytes--;
    }

    // fill middle bytes
    if( destBytes > 0 )
    {
        register LPBYTE lpDst;
        register int n, m;

        for( m = 0; m < height; m++ )
        {
            lpDst = lpDstStart;
            // line copy
            for( n = 0; n < destBytes; n++, lpDst++ )
                *lpDst = (BYTE)BLT_ROP( *lpDst, color );

            lpDstStart += scanBytes;
        }
    }
    return TRUE;
}
