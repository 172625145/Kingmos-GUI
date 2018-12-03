#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif
// the src is mono bitmap
static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPBYTE lpDestStart;
    LPCBYTE lpSrcStart;
    WORD clrFore, clrBack;
    int i, j, rows, cols, shiftSrc;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;
    int scanDestBytes = lpData->lpDestImage->bmWidthBytes;
    BYTE srcMask, srcBitMask;

    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * scanDestBytes + lpData->lprcDest->left;
    lpSrcStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * scanSrcBytes + (lpData->lprcSrc->left >>3);

    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    clrFore = lpData->solidColor;
    clrBack = lpData->solidBkColor;

    shiftSrc = lpData->lprcSrc->left & 0x07;

    for( i = 0; i < rows; i++ )
    {
        LPBYTE lpDest = lpDestStart;
        LPCBYTE lpSrc = lpSrcStart;

        srcMask = *lpSrc++;
        srcBitMask = 0x80 >> shiftSrc;
        for( j = 0; j < cols; j++ )
        {
            if( srcBitMask == 0 )
            {
                srcMask = *lpSrc++;
                srcBitMask = 0x80;
            }

            if( srcMask & srcBitMask )
                *lpDest = BLT_ROP( *lpDest, clrFore );
            else
                *lpDest = BLT_ROP( *lpDest, clrBack );

            srcBitMask >>= 1;
            lpDest++;
        }
        lpDestStart += scanDestBytes;
        lpSrcStart += scanSrcBytes;
    }
    return TRUE;
}
