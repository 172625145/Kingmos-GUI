#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int width = lpData->lprcDest->right - lpData->lprcDest->left;
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    int srcDWords = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int destDWords = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = (lpData->lprcSrc->left & 0x07) << 2;
    int dstOffset = (lpData->lprcDest->left & 0x07) << 2;
    int shift = dstOffset - srcOffset;
    int n, m;
    int scanDestDWords = lpData->lpDestImage->bmWidthBytes >> 2;
    int scanSrcDWords = lpData->lpSrcImage->bmWidthBytes >> 2;
    DWORD srcBuf[100];  // 400 pixel
    LPDWORD lpdwSrc = srcBuf;
    LPDWORD lpdwSrcBuf;
    LPDWORD lpdwDstBuf;
    DWORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    DWORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];

    LPCDWORD lpdwSrcStart;
    LPDWORD lpdwDstStart;

    lpdwSrcStart = (LPCDWORD)lpData->lpSrcImage->bmBits +
                          scanSrcDWords * lpData->lprcSrc->top +
                          (lpData->lprcSrc->left >> 3);
    lpdwDstStart = (LPDWORD)lpData->lpDestImage->bmBits +
                         scanDestDWords * lpData->lprcDest->top +
                         (lpData->lprcDest->left >> 3);


    if( !lpData->yPositive )
    { // from end to top
        lpdwSrcStart += scanSrcDWords * (height - 1);
        lpdwDstStart += scanDestDWords * (height - 1);
        scanSrcDWords = -scanSrcDWords;
        scanDestDWords = -scanDestDWords;
    }

    if(  destDWords == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
        //__MoveBits( lpdwSrc, shift, lpdwSrcStart, srcDWords );
		__MoveByteBits( (LPBYTE)lpdwSrc, shift, (LPBYTE)lpdwSrcStart, srcDWords << 2 );

        m = 0;
        lpdwDstBuf = lpdwDstStart;
        lpdwSrcBuf = lpdwSrc;
        // copy start bytes
        if( leftMask != 0xffffffff )
        {
            *lpdwDstBuf = (*lpdwDstBuf & (~leftMask)) | ( BLT_ROP( *lpdwDstBuf, *lpdwSrcBuf ) & leftMask );
            m++;
            lpdwDstBuf++; lpdwSrcBuf++;
        }
        if( rightMask != 0xffffffff && m < destDWords )
        {
            LPDWORD lpdwDstT = lpdwDstStart+(destDWords-1);
            *lpdwDstT = (*lpdwDstT & (~rightMask)) | ( BLT_ROP( *lpdwDstT, *(lpdwSrc+destDWords-1) ) & rightMask );
            m++;
        }

        for( ; m < destDWords; m++ )
        {
            *lpdwDstBuf = BLT_ROP( *lpdwDstBuf, *lpdwSrcBuf );
            lpdwDstBuf++;
            lpdwSrcBuf++;
        }

        lpdwSrcStart += scanSrcDWords;
        lpdwDstStart += scanDestDWords;
    }
    return TRUE;
}
