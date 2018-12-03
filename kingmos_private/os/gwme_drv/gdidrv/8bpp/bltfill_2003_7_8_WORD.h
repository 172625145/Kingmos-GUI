#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif


static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    LPBYTE lpDstStart = lpData->lpDestImage->bmBits +
                        lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top +
                        lpData->lprcDest->left;
    WORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    WORD destBytes = lpData->lprcDest->right - lpData->lprcDest->left;
//    LPBYTE lpDstEnd = lpDstStart + destBytes - 1;
    WORD color;
    int n;
    int m;
    int leftBytes;
    int midBytes;
    int rightBytes;

    n = (int)(((DWORD)lpDstStart) & 0x01);
//    m = (destBytes&0x01);
//    leftBytes = min( n, m );
//    midBytes = (destBytes - leftBytes) & (~0x01);
//    rightBytes = destBytes - midBytes - leftBytes;

    m = destBytes;
    leftBytes = min( n, m );
    midBytes = (destBytes - leftBytes) & (~0x0001);
    rightBytes = destBytes - midBytes - leftBytes;


    color = (WORD)lpData->solidColor;
    color = (color << 8) | color;
    for( n = 0; n < height; n++ )
    {
        LPBYTE lpDst = lpDstStart;
        LPBYTE lpEnd = lpDstStart + leftBytes;
        while( lpDst < lpEnd )
        {
            *lpDst = BLT_ROP( *lpDst, (BYTE)color );
            lpDst++;
        }
        lpEnd = lpDst + midBytes;
        while( lpDst < lpEnd )
        {
            *((WORD*)lpDst) = BLT_ROP( *((WORD*)lpDst), (WORD)color );
            lpDst += 2;
        }
        lpEnd = lpDst + rightBytes;
        while( lpDst < lpEnd )
        {
            *lpDst = BLT_ROP( *lpDst, (BYTE)color );
            lpDst++;
        }
        lpDstStart += scanBytes;
    }
    return TRUE;
}
