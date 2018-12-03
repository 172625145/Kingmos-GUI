static WORD TEXT_ALPHA_ROP( WORD dest, WORD src, BYTE alpha )
{
	
	BYTE srcRed = src >> 11;
	BYTE srcGreen = (src >> 5) & 0x3f;
	BYTE srcBlue = src & 0x1f;

	BYTE destRed = dest >> 11;
	BYTE destGreen = (dest >> 5) & 0x3f;
	BYTE destBlue = dest & 0x1f;
	BYTE invertAlpha;

	
	DWORD alphaRed;
	DWORD alphaGreen;
	DWORD alphaBlue;

	alpha >>= 4;
	invertAlpha = 15 - alpha;


	alphaRed = ( srcRed * alpha + destRed * invertAlpha ) / 15;
	alphaGreen = ( srcGreen * alpha + destGreen * invertAlpha ) / 15;
	alphaBlue = ( srcBlue * alpha + destBlue * invertAlpha ) / 15;


	return (WORD)( (alphaRed << 11) | ( alphaGreen << 5 ) | alphaBlue );
}


// text alpha , bits must = 8, 256 level gray, 5r-6g-5b
//static BOOL FUNCTION( _LPBLKBITBLT lpData, BOOL bOPAQUE )
static BOOL __TextAlpha1616( _LPBLKBITBLT lpData, BOOL bOPAQUE )
{
    LPWORD lpDestStart, lpDest;
    LPCBYTE lpGrayStart, lpGray;
    WORD clFore, clBack;
    int i, j, rows, cols;//, shift;
	UINT destLineBytes, grayLineBytes;
    //BYTE mask, bitMask;

	grayLineBytes = lpData->lpSrcImage->bmWidthBytes;
	destLineBytes = lpData->lpDestImage->bmWidthBytes;

    lpDestStart = (LPWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * destLineBytes) + lpData->lprcDest->left;
    lpGrayStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * grayLineBytes + lpData->lprcSrc->left;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;

    //shift = lpData->lprcSrc->left & 0x07;
    clFore = (WORD)lpData->solidColor;
    clBack = (WORD)lpData->solidBkColor;

    for( i = 0; i < rows; i++ )
    {
		BYTE clrGray;
        lpDest = lpDestStart;
        lpGray = lpGrayStart;

        
        //bitMask = 0x80 >> shift;
        for( j = 0; j < cols; j++ )
        {
			clrGray = *lpGray;
			if( clrGray >= 0xf0 )
			{
				*lpDest = (WORD)clFore;
			}
			else if( clrGray == 0 )
			{
				if( bOPAQUE )
					*lpDest = (WORD)clBack;
			}
			else
			{
				if( bOPAQUE )
					*lpDest = TEXT_ALPHA_ROP( clBack, (WORD)clFore, clrGray );
				else
					*lpDest = TEXT_ALPHA_ROP( *lpDest, (WORD)clFore, clrGray );
			}
            lpDest++;
			lpGray++;
        }
        lpDestStart = (LPWORD)( (LPBYTE)lpDestStart + destLineBytes );
        lpGrayStart += grayLineBytes;
    }
    return TRUE;
}

