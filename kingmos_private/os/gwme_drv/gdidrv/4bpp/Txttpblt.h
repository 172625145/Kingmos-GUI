#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
//static BOOL __TextTransparentBitBlt( _LPBLKBITBLT lpData )
{
    LPDWORD lpdwDestStart, lpdwDst;
    LPCBYTE lpMaskStart, lpStart;
    DWORD clFore, scanDstDWords;
    DWORD mask, rightMask, destDWords, maskBytes;

	WORD  maskValue;

    int i, j, n,  rows, maskShift, dstShift, shift;

    // 以 DWORD 为单位，一行有多少 DWORD
	scanDstDWords = lpData->lpDestImage->bmWidthBytes >> 2;
	// 以 DWORD 为单位，目标地址
    lpdwDestStart = (LPDWORD)lpData->lpDestImage->bmBits + lpData->lprcDest->top * scanDstDWords + (lpData->lprcDest->left >> 3);
	// 以 BYTE 为单位，Mask目标地址
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;

    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = 0xffffff00l >> mask;
    else
        rightMask = 0xffffffffl;

    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;

// 以 DWORD 为单位，一行有多少 DWORD
    destDWords =( (lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    maskBytes = ( (lpData->lprcSrc->left + i - 1) >> 3 ) - (lpData->lprcSrc->left >> 3) + 1;

    clFore = dwPalette[lpData->solidColor&PAL_INDEX_MASK];

    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;

    for( i = 0; i < rows; i++ )
    {
	    //  handle first byte
        lpStart = lpMaskStart;
        lpdwDst = lpdwDestStart;

        maskValue = *lpStart;
        maskValue <<= (maskShift + 8);
        maskValue >>= dstShift;
        shift = maskShift + 8 - dstShift;

	    if( maskBytes == 1 )
	    {
            maskValue &= (rightMask << shift);
            shift = 0;
	    }
        lpStart++;
        j = 1;   /// count mask bytes
        n = 0;   // count dest bytes

        while( (DWORD)n < destDWords )
        {
            if( shift > 8 )
            {
                shift -= 8;
                maskValue |= *lpStart++ << shift;
                if( (DWORD)++j == maskBytes )
                {
                    maskValue &= (rightMask << shift);
                    shift = 0;
                }
            }
            mask = maskValue >> 8;
            maskValue <<= 8;
            shift += 8;

            mask = EXT_PATTERN( mask );

            *lpdwDst = (*lpdwDst & ~mask) | (clFore & mask);

            lpdwDst++;
            n++;
        }
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpdwDestStart += scanDstDWords;
    }
    return TRUE;
}
