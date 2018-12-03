#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef DEST_ROP
    #error not define DEST_ROP
#endif

#ifndef SRC_ROP
    #error not define SRC_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int width = lpData->lprcDest->right - lpData->lprcDest->left;
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    int srcBytes = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int maskBytes = ((lpData->lprcMask->left + width - 1) >> 3) - (lpData->lprcMask->left >> 3) + 1;
    int destDWords = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = (lpData->lprcSrc->left & 0x07);
    int maskOffset = (lpData->lprcMask->left & 0x07);
    int dstOffset = (lpData->lprcDest->left & 0x07) << 2;
    int shift = (dstOffset >> 2) - srcOffset;
    int shiftMask = (dstOffset >> 2) - maskOffset;
    int n, m;
    int scanDestDWords = lpData->lpDestImage->bmWidthBytes >> 2;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;
    int scanMaskBytes = lpData->lpMaskImage->bmWidthBytes;
    BYTE srcBuf[100];  // 800 pixel
    BYTE maskBuf[100];  // 800 pixel
    LPBYTE lpSrc = srcBuf;
    LPBYTE lpMask = maskBuf;
    LPBYTE lpSrcBuf;
    LPBYTE lpMaskBuf;
    LPDWORD lpdwDstBuf;
    DWORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    DWORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    DWORD maskSrc, mask;

    LPCBYTE lpSrcStart;
    LPCBYTE lpMaskStart;
    LPDWORD lpdwDstStart;

    lpSrcStart = (LPCBYTE)lpData->lpSrcImage->bmBits +
                          scanSrcBytes * lpData->lprcSrc->top +
                          (lpData->lprcSrc->left >> 3);
    lpMaskStart = (LPCBYTE)lpData->lpMaskImage->bmBits +
                          scanMaskBytes * lpData->lprcMask->top +
                          (lpData->lprcMask->left >> 3);
    lpdwDstStart = (LPDWORD)lpData->lpDestImage->bmBits +
                         scanDestDWords * lpData->lprcDest->top +
                         (lpData->lprcDest->left >> 3);

    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += scanSrcBytes * (height - 1);
        lpMaskStart += scanMaskBytes * (height - 1);
        lpdwDstStart += scanDestDWords * (height - 1);

        scanSrcBytes = -scanSrcBytes;
        scanMaskBytes = -scanMaskBytes;
        scanDestDWords = -scanDestDWords;
    }

    if(  destDWords == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
        __MoveByteBits( lpSrc, shift, lpSrcStart, srcBytes );// << 1 );
        __MoveByteBits( lpMask, shiftMask, lpMaskStart, maskBytes );

        m = 0;
        lpdwDstBuf = lpdwDstStart;
        lpSrcBuf = lpSrc;
        lpMaskBuf = lpMask;
        // copy start bytes
        if( leftMask != 0xffffffff )
        {
            maskSrc = EXT_PATTERN( *lpSrcBuf );
            mask = EXT_PATTERN( *lpMaskBuf );
            *lpdwDstBuf = (*lpdwDstBuf & (~leftMask)) |
                        ( (DEST_ROP(*lpdwDstBuf,mask) | SRC_ROP(maskSrc, mask)) & leftMask );
            m++;
            lpdwDstBuf++; lpSrcBuf++; lpMaskBuf++;
        }
        if( rightMask != 0xffffffff && m < destDWords )
        {
            LPDWORD lpdwDstT = lpdwDstStart+(destDWords-1);

            maskSrc = EXT_PATTERN( *(lpSrc+destDWords-1) );
            mask = EXT_PATTERN( *(lpMask+destDWords-1) );
            *lpdwDstT = (*lpdwDstT & (~rightMask)) |
                      ( (DEST_ROP(*lpdwDstT,mask) | SRC_ROP(maskSrc,mask) ) & rightMask );
            m++;
        }

        for( ; m < destDWords; m++ )
        {
            maskSrc = EXT_PATTERN( *lpSrcBuf );
            mask = EXT_PATTERN( *lpMaskBuf );
            *lpdwDstBuf = DEST_ROP( *lpdwDstBuf, mask ) | SRC_ROP( maskSrc, mask );
            lpdwDstBuf++;
            lpSrcBuf++;
            lpMaskBuf++;
        }

        lpSrcStart += scanSrcBytes;
        lpMaskStart += scanMaskBytes;
        lpdwDstStart += scanDestDWords;
    }
    return TRUE;
}
