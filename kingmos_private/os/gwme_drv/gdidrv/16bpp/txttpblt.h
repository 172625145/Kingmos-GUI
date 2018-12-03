#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
//BOOL _TextBitBlt( _LPBLKBITBLT lpData )
{
    LPWORD lpDestStart, lpDest;
    LPCBYTE lpMaskStart, lpMask;
    WORD clFore, clBack;
    int i, j, rows, cols, shift;
    BYTE mask, bitMask;
	UINT bmSrcWidthBytes, bmDestWidthBytes;


	bmSrcWidthBytes = lpData->lpSrcImage->bmWidthBytes;
	bmDestWidthBytes = lpData->lpDestImage->bmWidthBytes;

    lpDestStart = (LPWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * bmDestWidthBytes) + lpData->lprcDest->left;
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * bmSrcWidthBytes + (lpData->lprcSrc->left >> 3);
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;

    shift = lpData->lprcSrc->left & 0x07;
    clFore = (WORD)lpData->solidColor;
    clBack = (WORD)lpData->solidBkColor;

    for( i = 0; i < rows; i++ )
    {
        lpDest = lpDestStart;
        lpMask = lpMaskStart;

        mask = *lpMask++;
        bitMask = 0x80 >> shift;
        for( j = 0; j < cols; j++ )
        {
            if( bitMask == 0 )
            {
                mask = *lpMask++;
                bitMask = 0x80;
            }
            if( mask & bitMask )
            {                
                *lpDest = BLT_ROP( *lpDest, (WORD)clFore );
            }
      //      else if( bEraseBack )
        //    {
          //      *lpDest = BLT_ROP( *lpDest, clBack );
//            }
            lpDest++;
            bitMask >>= 1;
        }
        lpDestStart = (LPWORD)((LPBYTE)lpDestStart + bmDestWidthBytes);
//        lpMaskStart += lpData->lpMaskImage->bmWidthBytes;
        lpMaskStart += bmSrcWidthBytes;
    }
    return TRUE;
}

