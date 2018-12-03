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
                        lpData->lprcDest->left * PIXEL_BYTES;
    DWORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    DWORD col = lpData->lprcDest->right - lpData->lprcDest->left;
    DWORD color;
	int n;

    color = lpData->solidColor;

    for( n = 0; n < height; n++ )
    {
        LPBYTE lpDst = lpDstStart;
        LPBYTE lpEnd = lpDstStart + col;
        while( lpDst < lpEnd )
        {
            //*lpDst = BLT_ROP( *lpDst, (BYTE)color );
			*lpDst = BLT_ROP( *lpDst, *( (LPBYTE)&color ) );
			*( lpDst+1 ) = BLT_ROP( *(lpDst+1), *( (LPBYTE)&color + 1 ) );
			*( lpDst+2 ) = BLT_ROP( *(lpDst+2), *( (LPBYTE)&color + 2 ) );

            lpDst += PIXEL_BYTES;
        }
        lpDstStart += scanBytes;
    }
    return TRUE;
}
