#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPDWORD lpDestStart, lpDest;
    LPCBYTE lpMaskStart, lpMask;
    DWORD clFore, clBack;
    int i, j, rows, cols, shift;
    BYTE mask, bitMask;

    lpDestStart = (LPDWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes) + lpData->lprcDest->left;
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    shift = lpData->lprcSrc->left & 0x07;
    clFore = (DWORD)lpData->solidColor;
    clBack = (DWORD)lpData->solidBkColor;

    for( i = 0; i < rows; i++ )
    {
        lpDest = lpDestStart;
        lpMask = lpMaskStart;

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
                *lpDest = BLT_ROP( *lpDest, (DWORD)clFore );
            }
            else// if( bEraseBack )
            {
                *lpDest = BLT_ROP( *lpDest, (DWORD)clBack );
            }
            lpDest++;
            bitMask >>= 1;
        }
        lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + lpData->lpDestImage->bmWidthBytes );
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
    }
    return TRUE;
}

