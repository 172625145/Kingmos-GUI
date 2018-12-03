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
    int srcBytes = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int maskBytes = ((lpData->lprcMask->left + width - 1) >> 3) - (lpData->lprcMask->left >> 3) + 1;
    int destBytes = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = lpData->lprcSrc->left & 0x07;
    int maskOffset = lpData->lprcMask->left & 0x07;
    int dstOffset = lpData->lprcDest->left & 0x07;
    int shift = dstOffset - srcOffset;
    int shiftMask = dstOffset - maskOffset;
    int n, m;
    int scanDestBytes = lpData->lpDestImage->bmWidthBytes;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;
    int scanMaskBytes = lpData->lpMaskImage->bmWidthBytes;
    BYTE srcBuf[100];
    BYTE maskBuf[100];
    LPBYTE lpSrc = srcBuf;
    LPBYTE lpMask = maskBuf;
    LPBYTE lpSrcBuf;
    LPBYTE lpDstBuf;
    LPBYTE lpMaskBuf;

    BYTE leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    BYTE rightMask = rightFillMask[lpData->lprcDest->right & 0x07];

    LPCBYTE lpSrcStart;
    LPCBYTE lpMaskStart;
    LPBYTE lpDstStart;

    lpSrcStart = lpData->lpSrcImage->bmBits +
                 lpData->lpSrcImage->bmWidthBytes * lpData->lprcSrc->top +
                 (lpData->lprcSrc->left >> 3);
    lpDstStart = lpData->lpDestImage->bmBits +
                 lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top +
                 (lpData->lprcDest->left >> 3);
    lpMaskStart = lpData->lpMaskImage->bmBits +
                 lpData->lpMaskImage->bmWidthBytes * lpData->lprcMask->top +
                 (lpData->lprcMask->left >> 3);


    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += lpData->lpSrcImage->bmWidthBytes * (height - 1);
        lpMaskStart += lpData->lpMaskImage->bmWidthBytes * (height - 1);
        lpDstStart += lpData->lpDestImage->bmWidthBytes * (height - 1);
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
        scanMaskBytes = -scanMaskBytes;
    }

    if( ((DWORD)lpDstStart) & 0x01 )
    {
        lpSrc++;     // align byte as dest
        lpMask++;
    }
    if(  destBytes == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
        __MoveBits( lpSrc, shift, lpSrcStart, srcBytes );
        __MoveBits( lpMask, shiftMask, lpMaskStart, maskBytes );

        m = 0;
        lpDstBuf = lpDstStart;
        lpSrcBuf = lpSrc;
        lpMaskBuf = lpMask;
        // copy start bytes
        if( leftMask != 0xff )
        {
            *lpDstBuf = (*lpDstBuf & (~leftMask)) |
                        ( ( DEST_ROP(*lpDstBuf,*lpMaskBuf) | SRC_ROP(*lpSrcBuf,*lpMaskBuf) ) & leftMask );
            m++;
            lpDstBuf++; lpSrcBuf++; lpMaskBuf++;
        }
        if( rightMask != 0xff && m < destBytes )
        {
            LPBYTE lpDstT = lpDstStart+(destBytes-1);
            *lpDstT = (*lpDstT & (~rightMask)) |
                      ( (DEST_ROP(*lpDstT, *(lpMask+destBytes-1) )|SRC_ROP(*(lpSrc+destBytes-1),*(lpMask+destBytes-1)) ) & rightMask );
            m++;
        }

        for( ; m < destBytes; m++ )
        {
//            *lpDstBuf = BLT_ROP( *lpDstBuf, *lpSrcBuf );
            *lpDstBuf = DEST_ROP( *lpDstBuf, *lpMaskBuf ) | SRC_ROP( *lpSrcBuf, *lpMaskBuf );
            lpDstBuf++;
            lpSrcBuf++;
            lpMaskBuf++;
        }

        lpSrcStart += scanSrcBytes;
        lpDstStart += scanDestBytes;
        lpMaskStart += scanMaskBytes;
    }
    return TRUE;
}
