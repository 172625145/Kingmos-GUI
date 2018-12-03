/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����ͼ���豸��������(BLT & PAT)
�汾�ţ�3.0.0
����ʱ�ڣ�2000-9-06
���ߣ�����
�޸ļ�¼��
     1.LN 2003-05-06, Hatch brush�д���

******************************************************/

#ifndef FUNCTION
    #error not define FUNCTION!
#endif

#ifndef BLT_ROP
    #error not define BLT_ROP
#endif

static BOOL FUNCTION( _LPBLKBITBLT lpData )
{
    LPBYTE lpDestStart, lpDest;
    LPCBYTE lpPattern;
    DWORD clrFore, clrBack;
    int i, j, n,  rows, cols, shift;
    BYTE mask, bitMask;
	int iDestWidthBytes;

	iDestWidthBytes = lpData->lpDestImage->bmWidthBytes;

    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * iDestWidthBytes + lpData->lprcDest->left * 3;
    lpPattern = lpData->lpBrush->pattern;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    //shift = lpData->lprcMask->left & 0x07;

	if( lpData->lpptBrushOrg )
	{
		//shift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		//n = lpData->lprcMask->top - lpData->lpptBrushOrg->y;
		shift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		n = lpData->lprcDest->top - lpData->lpptBrushOrg->y;
	}
	else
	{
		shift = n = 0;
	}

    clrFore = lpData->lpBrush->color;
    clrBack = lpData->solidBkColor;
    //n = lpData->lprcDest->top;
    for( i = 0; i < rows; i++ )
    {
        mask = *(lpPattern+(n&0x07));
		n++; //LN 2003-05-06,����
        bitMask = 0x80 >> shift;
        lpDest = lpDestStart;
        for( j = 0; j < cols; j++ )
        {
            if( bitMask == 0 )
            {
                bitMask = 0x80;
            }
            if( mask & bitMask )
            {
                //*lpDest = BLT_ROP( *lpDest, (BYTE)clrFore );
				*lpDest = BLT_ROP( *lpDest, *( (LPBYTE)&clrFore ) );
				*( lpDest+1 ) = BLT_ROP( *(lpDest+1), *( (LPBYTE)&clrFore + 1 ) );
				*( lpDest+2 ) = BLT_ROP( *(lpDest+2), *( (LPBYTE)&clrFore + 2 ) );
            }
            else
			{
                //*lpDest = BLT_ROP( *lpDest, (BYTE)clrBack );
				*lpDest = BLT_ROP( *lpDest, *( (LPBYTE)&clrBack ) );
				*( lpDest+1 ) = BLT_ROP( *(lpDest+1), *( (LPBYTE)&clrBack + 1 ) );
				*( lpDest+2 ) = BLT_ROP( *(lpDest+2), *( (LPBYTE)&clrBack + 2 ) );
			}
            lpDest+=PIXEL_BYTES;//+;
            bitMask >>= 1;
        }
        lpDestStart += iDestWidthBytes;//lpData->lpDestImage->bmWidthBytes;
    }
    return TRUE;
}

