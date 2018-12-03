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
    DWORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    DWORD destBytes = lpData->lprcDest->right - lpData->lprcDest->left;
    DWORD color;
    int n;
    int m;
    int leftBytes;
    int midBytes;
    int rightBytes;

    n = (int)(((DWORD)lpDstStart) & 0x03);

    m = destBytes;
    leftBytes = min( n, m );
    midBytes = (destBytes - leftBytes) & (~0x0003);
    rightBytes = destBytes - midBytes - leftBytes;


    color = (WORD)lpData->solidColor;
    color = (color << 8) | color;
	color = (color << 16) | color;

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
        {   // DWORD ²Ù×÷
            *((DWORD*)lpDst) = BLT_ROP( *((DWORD*)lpDst), (DWORD)color );
            lpDst += 4;
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
