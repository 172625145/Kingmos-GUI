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
    int srcBytes = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int destBytes = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = lpData->lprcSrc->left & 0x07;
    int dstOffset = lpData->lprcDest->left & 0x07;
    int shift = dstOffset - srcOffset;
    int n, m;
    int scanDestBytes = lpData->lpDestImage->bmWidthBytes;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;
    BYTE srcBuf[100];
    LPBYTE lpSrc = srcBuf;
    LPBYTE lpSrcBuf;
    LPBYTE lpDstBuf;
    BYTE leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    BYTE rightMask = rightFillMask[lpData->lprcDest->right & 0x07];

    LPCBYTE lpSrcStart;
    LPBYTE lpDstStart;

    lpSrcStart = lpData->lpSrcImage->bmBits +
                 lpData->lpSrcImage->bmWidthBytes * lpData->lprcSrc->top +
                 (lpData->lprcSrc->left >> 3);
    lpDstStart = lpData->lpDestImage->bmBits +
                 lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top +
                 (lpData->lprcDest->left >> 3);


    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += lpData->lpSrcImage->bmWidthBytes * (height - 1);
        lpDstStart += lpData->lpDestImage->bmWidthBytes * (height - 1);
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
    }

    if( ((DWORD)lpDstStart) & 0x01 )
        lpSrc++;     // align byte as dest
    if(  destBytes == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
        __MoveBits( lpSrc, shift, lpSrcStart, srcBytes );

        m = 0;
        lpDstBuf = lpDstStart;
        lpSrcBuf = lpSrc;
        // copy start bytes
        if( leftMask != 0xff )
        {
            *lpDstBuf = (*lpDstBuf & (~leftMask)) | ( BLT_ROP( *lpDstBuf, *lpSrcBuf ) & leftMask );
            m++;
            lpDstBuf++; lpSrcBuf++;
        }
        if( rightMask != 0xff && m < destBytes )
        {
            LPBYTE lpDstT = lpDstStart+(destBytes-1);
            *lpDstT = (*lpDstT & (~rightMask)) | ( BLT_ROP( *lpDstT, *(lpSrc+destBytes-1) ) & rightMask );
            m++;
        }

        for( ; m < destBytes; m++ )
        {
            *lpDstBuf = BLT_ROP( *lpDstBuf, *lpSrcBuf );
            lpDstBuf++;
            lpSrcBuf++;
        }

        lpSrcStart += scanSrcBytes;
        lpDstStart += scanDestBytes;
    }
    return TRUE;
}
