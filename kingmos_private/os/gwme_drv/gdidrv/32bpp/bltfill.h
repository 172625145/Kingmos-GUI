#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif


static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    LPDWORD lpDstStart = (LPDWORD)( lpData->lpDestImage->bmBits +
                        lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top ) +
                        lpData->lprcDest->left;
    DWORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    DWORD destDWords = lpData->lprcDest->right - lpData->lprcDest->left;
    DWORD color;
	int n;

    color = (DWORD)lpData->solidColor;
    for( n = 0; n < height; n++ )
    {
        LPDWORD lpDst = lpDstStart;
        LPDWORD lpEnd = lpDstStart + destDWords;
        while( lpDst < lpEnd )
        {
            *lpDst = BLT_ROP( *lpDst, (DWORD)color );
            lpDst++;
        }
        lpDstStart = (LPDWORD)( (LPBYTE)lpDstStart + scanBytes );
    }
    return TRUE;
}
