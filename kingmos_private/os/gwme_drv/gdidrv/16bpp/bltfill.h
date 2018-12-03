#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif


static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    LPWORD lpDstStart = (LPWORD)( lpData->lpDestImage->bmBits +
                        lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top ) +
                        lpData->lprcDest->left;
    DWORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    DWORD destWords = lpData->lprcDest->right - lpData->lprcDest->left;
//    LPBYTE lpDstEnd = lpDstStart + destBytes - 1;
    DWORD color;
    int n;
    int m;
    int leftWords;
    int midWords;
    int rightWords;

    n = (int)( ( (DWORD)lpDstStart >> 1 ) & 0x01 );
//    m = (destBytes&0x01);
//    leftBytes = min( n, m );
//    midBytes = (destBytes - leftBytes) & (~0x01);
//    rightBytes = destBytes - midBytes - leftBytes;

    m = destWords;
    leftWords = min( n, m );
    midWords = (destWords - leftWords) & (~0x0001);
    rightWords = destWords - midWords - leftWords;


    color = (WORD)lpData->solidColor;
    color = (color << 16) | color;
    for( n = 0; n < height; n++ )
    {
        LPWORD lpDst = lpDstStart;
        LPWORD lpEnd = lpDstStart + leftWords;
        while( lpDst < lpEnd )
        {
            *lpDst = BLT_ROP( *lpDst, (WORD)color );
            lpDst++;
        }
        lpEnd = lpDst + midWords;
        while( lpDst < lpEnd )
        {
            *((DWORD*)lpDst) = BLT_ROP( *((DWORD*)lpDst), (DWORD)color );
            lpDst += 2;
        }
        lpEnd = lpDst + rightWords;
        while( lpDst < lpEnd )
        {
            *lpDst = BLT_ROP( *lpDst, (WORD)color );
            lpDst++;
        }
        lpDstStart = (LPWORD)((LPBYTE)lpDstStart + scanBytes);
    }
    return TRUE;
}
