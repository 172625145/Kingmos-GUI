#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
//BOOL _TextBitBlt( _LPBLKBITBLT lpData )
{
    LPBYTE lpDestStart, lpDest;
    LPCBYTE lpMaskStart, lpMask;
    WORD clFore, clBack;
    int i, j, rows, cols, shift;
    //BOOL bEraseBack = (lpData->backMode == OPAQUE);
    BYTE mask, bitMask;


    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes + lpData->lprcDest->left;
//    lpMaskStart = lpData->lpMaskImage->bmBits + lpData->lprcMask->top * lpData->lpMaskImage->bmWidthBytes + (lpData->lprcMask->left >> 3);
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
//    shift = lpData->lprcMask->left & 0x07;
    shift = lpData->lprcSrc->left & 0x07;
    clFore = (WORD)lpData->solidColor;
    clBack = (WORD)lpData->solidBkColor;

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
                *lpDest = BLT_ROP( *lpDest, (BYTE)clFore );
            }
            else// if( bEraseBack )
            {
                *lpDest = BLT_ROP( *lpDest, (BYTE)clBack );
            }
            lpDest++;
            bitMask >>= 1;
        }
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
//        lpMaskStart += lpData->lpMaskImage->bmWidthBytes;
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
    }
    return TRUE;
}

