#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef MASK_BLT_ROP
    #error not define MASK_BLT_ROP
#endif

// the src is mono bitmap
static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPDWORD lpDestStart;
    LPCBYTE lpSrcStart;
    LPCBYTE lpMaskStart;
    DWORD clrFore, clrBack;
    int i, j, rows, cols, shiftMask, maskBytes, shiftSrc, srcBytes;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;
    int scanDestBytes = lpData->lpDestImage->bmWidthBytes;
    int scanMaskBytes = lpData->lpMaskImage->bmWidthBytes;
    BYTE mask, bitMask, srcMask, srcBitMask;

    lpDestStart = (LPDWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * scanDestBytes) + lpData->lprcDest->left;
    lpSrcStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * scanSrcBytes + (lpData->lprcSrc->left >>3);
    lpMaskStart = lpData->lpMaskImage->bmBits + lpData->lprcMask->top * scanMaskBytes + (lpData->lprcMask->left >> 3);
    maskBytes = ((lpData->lprcMask->right - 1) >> 3) - (lpData->lprcMask->left>>3) + 1;
    srcBytes = ((lpData->lprcSrc->right - 1) >> 3) - (lpData->lprcSrc->left>>3) + 1;

    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    clrFore = (DWORD)lpData->solidColor;
    clrBack = (DWORD)lpData->solidBkColor;

    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += lpData->lpSrcImage->bmWidthBytes * (rows - 1);
        lpMaskStart += lpData->lpMaskImage->bmWidthBytes * (rows - 1);
        lpDestStart = (LPDWORD)((LPBYTE)lpDestStart + lpData->lpDestImage->bmWidthBytes * (rows - 1));
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
        scanMaskBytes = -scanMaskBytes;
    }

    if( !lpData->xPositive )
    {     // right -> left
        shiftMask = (lpData->lprcMask->right-1) & 0x07;
        shiftSrc = (lpData->lprcSrc->right-1) & 0x07;

        for( i = 0; i < rows; i++ )
        {
            LPDWORD lpDest = lpDestStart + cols - 1;
            LPCBYTE lpSrc = lpSrcStart + srcBytes - 1;
            LPCBYTE lpMask = lpMaskStart + maskBytes - 1;

            mask = *lpMask--;
            srcMask = *lpSrc--;
            bitMask = 0x80 >> shiftMask;
            srcBitMask = 0x80 >> shiftSrc;
            for( j = 0; j < cols; j++ )
            {
                if( bitMask == 0 )
                {
                    mask = *lpMask--;
                    bitMask = 0x01;
                }
                if( srcBitMask == 0 )
                {
                    srcMask = *lpSrc--;
                    srcBitMask = 0x01;
                }
                if( mask & bitMask )
                {
                    if( srcMask & srcBitMask )
                        *lpDest = MASK_BLT_ROP( *lpDest, clrBack, 0xffff );
                    else
                        *lpDest = MASK_BLT_ROP( *lpDest, clrFore, 0xffff );
                }
                else
                {
                    if( srcMask & srcBitMask )
                        *lpDest = MASK_BLT_ROP( *lpDest, clrBack, 0 );
                    else
                        *lpDest = MASK_BLT_ROP( *lpDest, clrFore, 0 );
                }

                bitMask <<= 1;
                srcBitMask <<= 1;
                lpDest--;
            }
            lpDestStart = (LPDWORD)((LPBYTE)lpDestStart + scanDestBytes);
            lpSrcStart += scanSrcBytes;
            lpMaskStart += scanMaskBytes;
        }
    }
    else
    {
        shiftMask = lpData->lprcMask->left & 0x07;
        shiftSrc = lpData->lprcSrc->left & 0x07;

        for( i = 0; i < rows; i++ )
        {
            LPDWORD lpDest = lpDestStart;
            LPCBYTE lpSrc = lpSrcStart;
            LPCBYTE lpMask = lpMaskStart;

            mask = *lpMask++;
            srcMask = *lpSrc++;
            bitMask = 0x80 >> shiftMask;
            srcBitMask = 0x80 >> shiftSrc;
            for( j = 0; j < cols; j++ )
            {
                if( bitMask == 0 )
                {
                    mask = *lpMask++;
                    bitMask = 0x80;
                }
                if( srcBitMask == 0 )
                {
                    srcMask = *lpSrc++;
                    srcBitMask = 0x80;
                }

                if( mask & bitMask )
                {
                    if( srcMask & srcBitMask )
                        *lpDest = MASK_BLT_ROP( *lpDest, clrBack, 0xffff );
                    else
                        *lpDest = MASK_BLT_ROP( *lpDest, clrFore, 0xffff );
                }
                else
                {
                    if( srcMask & srcBitMask )
                        *lpDest = MASK_BLT_ROP( *lpDest, clrBack, 0 );
                    else
                        *lpDest = MASK_BLT_ROP( *lpDest, clrFore, 0 );
                }

                bitMask >>= 1;
                srcBitMask >>= 1;
                lpDest++;
            }
            lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + scanDestBytes );
            lpSrcStart += scanSrcBytes;
            lpMaskStart += scanMaskBytes;
        }
    }
    return TRUE;
}
