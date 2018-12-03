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
    int srcWords = ((lpData->lprcSrc->left + width - 1) >> 3) - (lpData->lprcSrc->left >> 3) + 1;
    int maskBytes = ((lpData->lprcMask->left + width - 1) >> 3) - (lpData->lprcMask->left >> 3) + 1;
    int destWords = ((lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    int srcOffset = (lpData->lprcSrc->left & 0x07) << 1;
    int maskOffset = (lpData->lprcMask->left & 0x07);// << 1;
    int dstOffset = (lpData->lprcDest->left & 0x07) << 1;
    int shift = dstOffset - srcOffset;
    int shiftMask = (dstOffset >> 1) - maskOffset;
    int n, m;
    int scanDestWords = lpData->lpDestImage->bmWidthBytes >> 1;
    int scanSrcWords = lpData->lpSrcImage->bmWidthBytes >> 1;
    WORD scanMaskBytes = lpData->lpMaskImage->bmWidthBytes;// >> 1;
    WORD srcBuf[100];  // 400 pixel
    BYTE maskBuf[100];  // 400 pixel
    LPWORD lpSrc = srcBuf;
    LPBYTE lpMask = (LPBYTE)maskBuf;
    LPWORD lpSrcBuf;
    LPBYTE lpMaskBuf;
    LPWORD lpDstBuf;
    WORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    WORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    WORD mask;

    LPCWORD lpSrcStart;
    LPCBYTE lpMaskStart;
    LPWORD lpDstStart;

    lpSrcStart = (LPCWORD)lpData->lpSrcImage->bmBits +
                          scanSrcWords * lpData->lprcSrc->top +
                          (lpData->lprcSrc->left >> 3);
    lpMaskStart = (LPCBYTE)lpData->lpMaskImage->bmBits +
                          scanMaskBytes * lpData->lprcMask->top +
                          (lpData->lprcMask->left >> 3);
    lpDstStart = (LPWORD)lpData->lpDestImage->bmBits +
                         scanDestWords * lpData->lprcDest->top +
                         (lpData->lprcDest->left >> 3);

    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += scanSrcWords * (height - 1);
        lpMaskStart += scanMaskBytes * (height - 1);
        lpDstStart += scanDestWords * (height - 1);

        scanSrcWords = -scanSrcWords;
        scanDestWords = -scanDestWords;
        scanMaskBytes = -scanMaskBytes;
    }

//    if( ((DWORD)lpDstStart) & 0x01 )
//        lpSrc++;     // align byte as dest
    if(  destWords == 1 )
        leftMask &= rightMask;

    for( n = 0; n < height; n++ )
    {
        __MoveBits( lpSrc, shift, lpSrcStart, srcWords );// << 1 );
        __MoveByteBits( lpMask, shiftMask, lpMaskStart, maskBytes );

        m = 0;
        lpDstBuf = lpDstStart;
        lpSrcBuf = lpSrc;
        lpMaskBuf = lpMask;
        // copy start bytes
        if( leftMask != 0xffff )
        {
            mask = EXT_PATTERN( *lpMaskBuf );
#ifdef EML_DOS
            *HIPTR(lpDstBuf) = (*HIPTR(lpDstBuf) & (~HIBYTE(leftMask))) |
                        ( (DEST_ROP(*HIPTR(lpDstBuf),HIBYTE(mask)) | SRC_ROP(*HIPTR(lpSrcBuf), HIBYTE(mask))) & HIBYTE(leftMask) );
            *LOPTR(lpDstBuf) = (*LOPTR(lpDstBuf) & (~LOBYTE(leftMask))) |
                        ( (DEST_ROP(*LOPTR(lpDstBuf),LOBYTE(mask)) | SRC_ROP(*LOPTR(lpSrcBuf), LOBYTE(mask))) & LOBYTE(leftMask) );
#else
            *lpDstBuf = (*lpDstBuf & (~leftMask)) |
                        ( (DEST_ROP(*lpDstBuf,mask) | SRC_ROP(*lpSrcBuf, mask)) & leftMask );
#endif
            m++;
            lpDstBuf++; lpSrcBuf++; lpMaskBuf++;
        }
        if( rightMask != 0xffff && m < destWords )
        {
            LPWORD lpDstT = lpDstStart+(destWords-1);
            mask = EXT_PATTERN( *(lpMask+destWords-1) );
#ifdef EML_DOS
            *HIPTR(lpDstT) = (*HIPTR(lpDstT) & (~HIBYTE(rightMask))) |
                      ( (DEST_ROP(*HIPTR(lpDstT),HIBYTE(mask)) | SRC_ROP(*HIPTR((lpSrc+destWords-1)),HIBYTE(mask)) ) & HIBYTE(rightMask) );
            *LOPTR(lpDstT) = (*LOPTR(lpDstT) & (~LOBYTE(rightMask))) |
                      ( (DEST_ROP(*LOPTR(lpDstT),LOBYTE(mask)) | SRC_ROP(*LOPTR((lpSrc+destWords-1)),LOBYTE(mask)) ) & LOBYTE(rightMask) );
#else
            *lpDstT = (*lpDstT & (~rightMask)) |
                      ( (DEST_ROP(*lpDstT,mask) | SRC_ROP(*(lpSrc+destWords-1),mask) ) & rightMask );
#endif
            m++;
        }

        for( ; m < destWords; m++ )
        {
            mask = EXT_PATTERN( *lpMaskBuf );
#ifdef EML_DOS
            *HIPTR(lpDstBuf) = DEST_ROP( *HIPTR(lpDstBuf), HIBYTE(mask) ) | SRC_ROP( *HIPTR(lpSrcBuf), HIBYTE(mask) );
            *LOPTR(lpDstBuf) = DEST_ROP( *LOPTR(lpDstBuf), LOBYTE(mask) ) | SRC_ROP( *LOPTR(lpSrcBuf), LOBYTE(mask) );
#else
            *lpDstBuf = DEST_ROP( *lpDstBuf, mask ) | SRC_ROP( *lpSrcBuf, mask );
#endif
            lpDstBuf++;
            lpSrcBuf++;
            lpMaskBuf++;
        }

        lpSrcStart += scanSrcWords;
        lpDstStart += scanDestWords;
        lpMaskStart += scanMaskBytes;
    }
    return TRUE;
}
