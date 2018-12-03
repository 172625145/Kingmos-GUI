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
    int srcWords = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int destWords = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = (lpData->lprcSrc->left & 0x07) << 1;
    int dstOffset = (lpData->lprcDest->left & 0x07) << 1;
    int shift = dstOffset - srcOffset;
    int n, m;
    int scanDestWords = lpData->lpDestImage->bmWidthBytes >> 1;
    int scanSrcWords = lpData->lpSrcImage->bmWidthBytes >> 1;
    WORD srcBuf[100];  // 400 pixel
    LPWORD lpSrc = srcBuf;
    LPWORD lpSrcBuf;
    LPWORD lpDstBuf;
    WORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    WORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];

    LPCWORD lpSrcStart;
    LPWORD lpDstStart;

    lpSrcStart = (LPCWORD)lpData->lpSrcImage->bmBits +
                          scanSrcWords * lpData->lprcSrc->top +
                          (lpData->lprcSrc->left >> 3);
    lpDstStart = (LPWORD)lpData->lpDestImage->bmBits +
                         scanDestWords * lpData->lprcDest->top +
                         (lpData->lprcDest->left >> 3);


    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += scanSrcWords * (height - 1);
        lpDstStart += scanDestWords * (height - 1);
        scanSrcWords = -scanSrcWords;
        scanDestWords = -scanDestWords;
    }

//    if( ((DWORD)lpDstStart) & 0x01 )
//        lpSrc++;     // align byte as dest
    if(  destWords == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
        __MoveBits( lpSrc, shift, lpSrcStart, srcWords );//<< 1 );

        m = 0;
        lpDstBuf = lpDstStart;
        lpSrcBuf = lpSrc;
        // copy start bytes
        if( leftMask != 0xffff )
        {
#ifdef EML_DOS
            *HIPTR(lpDstBuf) = (*HIPTR(lpDstBuf) & (~HIBYTE(leftMask))) | ( BLT_ROP( *HIPTR(lpDstBuf), *HIPTR(lpSrcBuf) ) & HIBYTE(leftMask) );
            *LOPTR(lpDstBuf) = (*LOPTR(lpDstBuf) & (~LOBYTE(leftMask))) | ( BLT_ROP( *LOPTR(lpDstBuf), *LOPTR(lpSrcBuf) ) & LOBYTE(leftMask) );
#else
            *lpDstBuf = (*lpDstBuf & (~leftMask)) | ( BLT_ROP( *lpDstBuf, *lpSrcBuf ) & leftMask );
#endif
            m++;
            lpDstBuf++; lpSrcBuf++;
        }
        if( rightMask != 0xffff && m < destWords )
        {
            LPWORD lpDstT = lpDstStart+(destWords-1);
#ifdef EML_DOS
            *HIPTR(lpDstT) = (*HIPTR(lpDstT) & (~HIBYTE(rightMask))) | ( BLT_ROP( *HIPTR(lpDstT), *HIPTR((lpSrc+destWords-1)) ) & HIBYTE(rightMask) );
            *LOPTR(lpDstT) = (*LOPTR(lpDstT) & (~LOBYTE(rightMask))) | ( BLT_ROP( *LOPTR(lpDstT), *LOPTR((lpSrc+destWords-1)) ) & LOBYTE(rightMask) );
#else
            *lpDstT = (*lpDstT & (~rightMask)) | ( BLT_ROP( *lpDstT, *(lpSrc+destWords-1) ) & rightMask );
#endif
            m++;
        }

        for( ; m < destWords; m++ )
        {
#ifdef EML_DOS
            *HIPTR(lpDstBuf) = BLT_ROP( *HIPTR(lpDstBuf), *HIPTR(lpSrcBuf) );
            *LOPTR(lpDstBuf) = BLT_ROP( *LOPTR(lpDstBuf), *LOPTR(lpSrcBuf) );
#else
            *lpDstBuf = BLT_ROP( *lpDstBuf, *lpSrcBuf );
#endif
            lpDstBuf++;
            lpSrcBuf++;
        }

        lpSrcStart += scanSrcWords;
        lpDstStart += scanDestWords;
    }
    return TRUE;
}
