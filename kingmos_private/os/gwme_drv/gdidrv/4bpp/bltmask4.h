#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef DEST_ROP
    #error not define MASK_ROP
#endif

#ifndef SRC_ROP
    #error not define SRC_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int width = lpData->lprcDest->right - lpData->lprcDest->left;
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    int srcDWords = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int maskBytes = ((lpData->lprcMask->left + width - 1) >> 3) - (lpData->lprcMask->left >> 3) + 1;
    int destDWords = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = (lpData->lprcSrc->left & 0x07) << 2;
    int maskOffset = (lpData->lprcMask->left & 0x07);
    int dstOffset = (lpData->lprcDest->left & 0x07) << 2;
    int shift = dstOffset - srcOffset;
    int shiftMask = (dstOffset >> 2) - maskOffset;
    int n, m;
    int scanDestDWords = lpData->lpDestImage->bmWidthBytes >> 2;
    int scanSrcDWords = lpData->lpSrcImage->bmWidthBytes >> 2;
    int scanMaskBytes = lpData->lpMaskImage->bmWidthBytes;
    DWORD srcBuf[100];  // 800 pixel
    BYTE maskBuf[100];  // 800 pixel
    LPDWORD lpdwSrc = srcBuf;
    LPBYTE lpMask = (LPBYTE)maskBuf;
    LPDWORD lpdwSrcBuf;
    LPBYTE lpMaskBuf;
    LPDWORD lpdwDstBuf;
    DWORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    DWORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    DWORD mask;

    LPCDWORD lpdwSrcStart;
    LPCBYTE lpMaskStart;
    LPDWORD lpdwDstStart;

    lpdwSrcStart = (LPCDWORD)lpData->lpSrcImage->bmBits +
                          scanSrcDWords * lpData->lprcSrc->top +
                          (lpData->lprcSrc->left >> 3);
    lpMaskStart = (LPCBYTE)lpData->lpMaskImage->bmBits +
                          scanMaskBytes * lpData->lprcMask->top +
                          (lpData->lprcMask->left >> 3);
    lpdwDstStart = (LPDWORD)lpData->lpDestImage->bmBits +
                         scanDestDWords * lpData->lprcDest->top +
                         (lpData->lprcDest->left >> 3);

    if( !lpData->yPositive )
    { // from end to top
        lpdwSrcStart += scanSrcDWords * (height - 1);
        lpMaskStart += scanMaskBytes * (height - 1);
        lpdwDstStart += scanDestDWords * (height - 1);

        scanSrcDWords = -scanSrcDWords;
        scanDestDWords = -scanDestDWords;
        scanMaskBytes = -scanMaskBytes;
    }

    if(  destDWords == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
//        __MoveBits( lpdwSrc, shift, lpdwSrcStart, srcDWords );
		__MoveByteBits( (LPBYTE)lpdwSrc, shift, (LPBYTE)lpdwSrcStart, srcDWords << 2 );

        __MoveByteBits( (LPBYTE)lpMask, shiftMask, (LPBYTE)lpMaskStart, maskBytes );

        m = 0;
        lpdwDstBuf = lpdwDstStart;
        lpdwSrcBuf = lpdwSrc;
        lpMaskBuf = lpMask;
        // copy start bytes
        if( leftMask != 0xffffffff )
        {
            mask = EXT_PATTERN( *lpMaskBuf );
            *lpdwDstBuf = (*lpdwDstBuf & (~leftMask)) |
                        ( (DEST_ROP(*lpdwDstBuf,mask) | SRC_ROP(*lpdwSrcBuf, mask)) & leftMask );
            m++;
            lpdwDstBuf++; lpdwSrcBuf++; lpMaskBuf++;
        }
        if( rightMask != 0xffffffff && m < destDWords )
        {
            LPDWORD lpdwDstT = lpdwDstStart+(destDWords-1);
            mask = EXT_PATTERN( *(lpMask+destDWords-1) );
            *lpdwDstT = (*lpdwDstT & (~rightMask)) |
                      ( (DEST_ROP(*lpdwDstT,mask) | SRC_ROP(*(lpdwSrc+destDWords-1),mask) ) & rightMask );
            m++;
        }

        for( ; m < destDWords; m++ )
        {
            mask = EXT_PATTERN( *lpMaskBuf );
            *lpdwDstBuf = DEST_ROP( *lpdwDstBuf, mask ) | SRC_ROP( *lpdwSrcBuf, mask );
            lpdwDstBuf++;
            lpdwSrcBuf++;
            lpMaskBuf++;
        }

        lpdwSrcStart += scanSrcDWords;
        lpdwDstStart += scanDestDWords;
        lpMaskStart += scanMaskBytes;
    }
    return TRUE;
}
