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
    WORD color;

    if( destWords == 1 )
        leftMask &= rightMask;
    color = palette[lpData->solidColor];
/*
    if( lpData->solidColor )
        color = 0xffff;
    else
        color = 0;
*/

    // fill left bytes if posible
    if( leftMask != 0xffff )
    {
        register LPWORD lpDst = lpDstStart;
        register int m;
        for( m = 0; m < height; m++, lpDst += scanWords )
        {
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
        register int m;
        for( m = 0; m < height; m++, lpDst += scanWords )
        {
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
        register int n, m;

        for( m = 0; m < height; m++ )
        {
            lpDst = lpDstStart;
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
