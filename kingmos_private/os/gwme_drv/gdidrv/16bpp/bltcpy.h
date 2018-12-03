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

    LPCWORD lpSrcStart;
    LPWORD lpDestStart;

    lpSrcStart = (LPWORD)( lpData->lpSrcImage->bmBits +
                 lpData->lpSrcImage->bmWidthBytes * lpData->lprcSrc->top ) +
                 lpData->lprcSrc->left;
    lpDestStart = (LPWORD)( lpData->lpDestImage->bmBits +
                 lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top ) +
                 lpData->lprcDest->left;

    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart = (LPWORD)( (LPBYTE)lpSrcStart + lpData->lpSrcImage->bmWidthBytes * (height - 1) );
        lpDestStart = (LPWORD)( (LPBYTE)lpDestStart + lpData->lpDestImage->bmWidthBytes * (height - 1) );
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
    }

#ifdef FAST_OP

	cols *= sizeof( WORD );
    for ( n = 0; n < height; n++)
	{
		memcpy( lpDestStart, lpSrcStart, cols );
        lpDestStart = (LPWORD)( (LPBYTE)lpDestStart + scanDestBytes );
        lpSrcStart = (LPWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );
	}
#else

    if (!lpData->xPositive)
    {
        // right -> left
        for( n = 0; n < height; n++ )
        {
            LPWORD lpDest = lpDestStart + cols - 1;
            LPCWORD lpSrc = lpSrcStart + cols - 1;

            while (lpDest >= lpDestStart)
            {
                *lpDest = BLT_ROP(*lpDest, *lpSrc);
                lpDest--;
                lpSrc--;
            }
            lpDestStart = (LPWORD)( (LPBYTE)lpDestStart + scanDestBytes );
            lpSrcStart = (LPWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );
        }
    }
    else
    {
        // Copy from left to right
        for ( n = 0; n < height; n++)
        {
            LPWORD lpDest = lpDestStart;
            LPCWORD lpSrc = lpSrcStart;
            LPWORD lpDestEnd = lpDest + cols;

            while (lpDest < lpDestEnd)
            {
                *lpDest = BLT_ROP(*lpDest, *lpSrc);
                lpDest++;
                lpSrc++;
            }
            lpDestStart = (LPWORD)( (LPBYTE)lpDestStart + scanDestBytes );
            lpSrcStart = (LPWORD)( (LPBYTE)lpSrcStart + scanSrcBytes );
        }
    }

#endif


    return TRUE;
}
