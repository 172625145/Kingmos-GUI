#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    int cols = lpData->lprcDest->right - lpData->lprcDest->left;
    int n;
    int scanDestBytes = lpData->lpDestImage->bmWidthBytes;
    int scanSrcBytes = lpData->lpSrcImage->bmWidthBytes;

    LPCDWORD lpSrcStart;
    LPDWORD lpDestStart;

    lpSrcStart = (LPDWORD)( lpData->lpSrcImage->bmBits +
                 lpData->lpSrcImage->bmWidthBytes * lpData->lprcSrc->top ) +
                 lpData->lprcSrc->left;
    lpDestStart = (LPDWORD)( lpData->lpDestImage->bmBits +
                 lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top ) +
                 lpData->lprcDest->left;


    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart = (LPDWORD)( (LPBYTE)lpSrcStart + lpData->lpSrcImage->bmWidthBytes * (height - 1) );
        lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + lpData->lpDestImage->bmWidthBytes * (height - 1) );
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
    }

	if (!lpData->xPositive)
    {
        // right -> left
        for( n = 0; n < height; n++ )
        {
            LPDWORD lpDest = lpDestStart + cols - 1;
            LPCDWORD lpSrc = lpSrcStart + cols - 1;

            while (lpDest >= lpDestStart)
            {
                *lpDest = BLT_ROP(*lpDest, *lpSrc);
                lpDest--;
                lpSrc--;
            }
            lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + scanDestBytes );
            lpSrcStart = (LPDWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );
        }
    }
    else
    {
        // Copy from left to right
        for ( n = 0; n < height; n++)
        {
            LPDWORD lpDest = lpDestStart;
            LPCDWORD lpSrc = lpSrcStart;
            LPDWORD lpDestEnd = lpDest + cols;

            while (lpDest < lpDestEnd)
            {
                *lpDest = BLT_ROP(*lpDest, *lpSrc);
                lpDest++;
                lpSrc++;
            }
            lpDestStart = (LPDWORD)( (LPBYTE)lpDestStart + scanDestBytes );
            lpSrcStart = (LPDWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );
        }
    }
    return TRUE;
}
