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

    LPCBYTE lpSrcStart;
    LPBYTE lpDestStart;

    lpSrcStart = lpData->lpSrcImage->bmBits +
                 lpData->lpSrcImage->bmWidthBytes * lpData->lprcSrc->top +
                 lpData->lprcSrc->left;
    lpDestStart = lpData->lpDestImage->bmBits +
                 lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top +
                 lpData->lprcDest->left;


    if( !lpData->yPositive )
    { // from end to top
        lpSrcStart += lpData->lpSrcImage->bmWidthBytes * (height - 1);
        lpDestStart += lpData->lpDestImage->bmWidthBytes * (height - 1);
        scanSrcBytes = -scanSrcBytes;
        scanDestBytes = -scanDestBytes;
    }

	if (!lpData->xPositive)
    {
        // right -> left
        for( n = 0; n < height; n++ )
        {
            LPBYTE lpDest = lpDestStart + cols - 1;
            LPCBYTE lpSrc = lpSrcStart + cols - 1;

            while (lpDest >= lpDestStart)
            {
                *lpDest = BLT_ROP(*lpDest, *lpSrc);
                lpDest--;
                lpSrc--;
            }
            lpDestStart += scanDestBytes;
            lpSrcStart += scanSrcBytes;
        }
    }
    else
    {
        // Copy from left to right
        for ( n = 0; n < height; n++)
        {
            LPBYTE lpDest = lpDestStart;
            LPCBYTE lpSrc = lpSrcStart;
            LPBYTE lpDestEnd = lpDest + cols;

            while (lpDest < lpDestEnd)
            {
                *lpDest = BLT_ROP(*lpDest, *lpSrc);
                lpDest++;
                lpSrc++;
            }
            lpDestStart += scanDestBytes;
            lpSrcStart += scanSrcBytes;
        }
    }
    return TRUE;
}
