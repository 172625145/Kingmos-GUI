#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef MASK_BLT_ROP
    #error not define MASK_BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPDWORD lpDestStart;
    LPCDWORD lpSrcStart;
    LPCBYTE lpMaskStart;
    int i, j, rows, cols, shift, maskBytes;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;
    int scanDestBytes = lpData->lpDestImage->bmWidthBytes;
    int scanMaskBytes = lpData->lpMaskImage->bmWidthBytes;
    BYTE mask, bitMask;

    lpDestStart = (LPDWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * scanDestBytes) + lpData->lprcDest->left;
    lpSrcStart = (LPDWORD)(lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * scanSrcBytes) + lpData->lprcSrc->left;
    lpMaskStart = lpData->lpMaskImage->bmBits + lpData->lprcMask->top * scanMaskBytes + (lpData->lprcMask->left >> 3);
    maskBytes = ((lpData->lprcMask->right - 1) >> 3) - (lpData->lprcMask->left>>3) + 1;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;

    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart = (LPDWORD)( (LPBYTE)lpSrcStart + lpData->lpSrcImage->bmWidthBytes * (rows - 1) );
        lpMaskStart += lpData->lpMaskImage->bmWidthBytes * (rows - 1);
        lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + lpData->lpDestImage->bmWidthBytes * (rows - 1) );
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
        scanMaskBytes = -scanMaskBytes;
    }

    if( !lpData->xPositive )
    {     // right -> left
        shift = (lpData->lprcMask->right-1) & 0x07;
        for( i = 0; i < rows; i++ )
        {
            LPDWORD lpDest = lpDestStart + cols - 1;
            LPCDWORD lpSrc = lpSrcStart + cols - 1;
            LPCBYTE lpMask = lpMaskStart + maskBytes - 1;

            mask = *lpMask--;
            bitMask = 0x80 >> shift;
            for( j = 0; j < cols; j++ )
            {
                if( bitMask == 0 )
                {
                    mask = *lpMask--;
                    bitMask = 0x01;
                }
                if( mask & bitMask )
                {
                    *lpDest = MASK_BLT_ROP( *lpDest, *lpSrc, 0xffff );
                }
                else
                    *lpDest = MASK_BLT_ROP( *lpDest, *lpSrc, 0 );

                bitMask <<= 1;
                lpDest--;
                lpSrc--;
            }
            lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart  + scanDestBytes );
            lpSrcStart = (LPDWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );
            lpMaskStart += scanMaskBytes;
        }

    }
    else
    {
        shift = lpData->lprcMask->left & 0x07;
        for( i = 0; i < rows; i++ )
        {
            LPDWORD lpDest = lpDestStart;
            LPCDWORD lpSrc = lpSrcStart;
            LPCBYTE lpMask = lpMaskStart;
            mask = *lpMask++;
            bitMask = 0x80 >> shift;
            for( j = 0; j < cols; j++ )
            {
                if( bitMask == 0 )
                {
                    mask = *lpMask++;
                    bitMask = 0x80;
                }
                if( mask & bitMask )
                {
                    *lpDest = MASK_BLT_ROP( *lpDest, *lpSrc, 0xffff );                    
                }
                else
                    *lpDest = MASK_BLT_ROP( *lpDest, *lpSrc, 0 );

                bitMask >>= 1;
                lpDest++;
                lpSrc++;
            }
            lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart  + scanDestBytes );
            lpSrcStart = (LPDWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );

            lpMaskStart += scanMaskBytes;
        }
    }
    return TRUE;
}
