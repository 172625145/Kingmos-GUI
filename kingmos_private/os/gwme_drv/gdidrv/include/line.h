/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：画线段
版本号：1.0.0
开发时期：2001-03-06
作者：李林
修改记录： 
******************************************************/

/* change to use new option 平衡-2001.12.25
static BOOL _DiagonalLine( _LPLINEDATA lpLine )
{
    _PIXELDATA pixelData;
    _BLKBITBLT blt;
    RECT rectClip;
    BOOL bSingle;
    int half;
    int sum;
    int x0, y0, p, l;

    if( lpLine->w == 1 )
    {
        pixelData.color = lpLine->color;
        pixelData.rop = lpLine->rop;
        pixelData.lpDestImage = lpLine->lpDestImage;
        bSingle = TRUE;
    }
    else
    {
        blt.lpDestImage = lpLine->lpDestImage;
        blt.lpBrush = 0;
        blt.solidColor = lpLine->color;
        blt.dwRop = PATCOPY;
        blt.lprcDest = &rectClip;//lpLine->lprcClip;
        half = lpLine->w >> 1;
        bSingle = FALSE;
    }
    sum = lpLine->sum;
	//while( lpdLine->sum1 >= lpdLine->sum2 )
    x0 = lpLine->x0;
    y0 = lpLine->y0;
    l = lpLine->l;
    if( lpLine->k )
        p = x0;
    else
        p = y0;
    while( sum-- )
    {
	    //if( lpdLine->k )
	    {
		    if( lpLine->pattern & (0x80 >> (p&0x07) ) )
            {
                //pixelData.x = lpdLine->u;
                //pixelData.y = lpdLine->v;
                pixelData.x = x0;
                pixelData.y = y0;
                
                if( bSingle )
    		        //lpDispDrv->lpPutPixel( &pixelData );
                    _PutPixel( &pixelData );
                else
                {
                    rectClip.left = x0 - half;
                    rectClip.right = rectClip.left + lpLine->w;
                    rectClip.top = y0 - half;
                    rectClip.bottom = rectClip.top + lpLine->w;
                    if( IntersectRect( &rectClip, &rectClip, lpLine->lprcClip ) )
                        //lpDispDrv->lpBlkBitBlt( &blt );
                        _BlkBitBlt( &blt );
                }
            }
	    }
	    //lpdLine->sum1--;
   	    if( lpLine->k )
        {
	        l += lpLine->n; x0 += lpLine->dm;
            p = x0;
	        if( l >= lpLine->m )
	        {
	            y0 += lpLine->dn; l -= lpLine->m;
	        }
        }
        else
        {
	        l += lpLine->n; y0 += lpLine->dm;
            p = y0;
	        if( l >= lpLine->m )
	        {
	            x0 += lpLine->dn; l -= lpLine->m;
	        }
        }
    }
    return TRUE;
}
*/

static BOOL _VertialLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, unsigned int len )
{
    if( lpLineData->pattern == 0xff )
    {
        return __VertialSolidLine( lpLineData, x0, y0, yDir, len );
    }
    else
    {
        if( lpLineData->backMode == TRANSPARENT )
            return __VertialTransparentLine( lpLineData, x0, y0, yDir, len );
        else
            return __VertialPatternLine( lpLineData, x0, y0, yDir, len );
    }
}

static BOOL _ScanLine( _LPLINEDATA lpData, int x0, int y0, int xDir, unsigned int len )
//xl,  y0, xDir, xr - xl
{
    if( lpData->pattern == 0xff )
    {
        return __ScanSolidLine( lpData, x0, y0, xDir, len );
    }
    else
    {
        if( lpData->backMode == TRANSPARENT )
            return __ScanTransparentLine( lpData, x0, y0, xDir, len );
        else
            return __ScanPatternLine( lpData, x0, y0, xDir, len );
    }
}


//BOOL ClipLine( int * lpx0, int * lpy0, int *lpx1, int *lpy1, LPRECT lprcClip )
/*
static BOOL ClipLine( int x0, int y0, int dx, int dy, LPRECT lprcClip )
{
	//int dx;
	//int dy;
	//int x0, x1, y1, y0;
	int dv[4];
	int num[4];
    int iStartDV, iStartNum, iEndDV, iEndNum;
	int i;
	BOOL bVisible;
	//x0 = *lpx0;
	//y0 = *lpy0;
	//x1 = *lpx1;
	//y1 = *lpy1;

	//dx = x1 - x0;
	//dy = y1 - y0;
	dv[0] = -dx;
	dv[1] = dx;
	dv[2] = -dy;
	dv[3] = dy;
	num[0] = x0 - lprcClip->left;
	num[1] = lprcClip->right - x0;
	num[2] = y0 - lprcClip->top;
	num[3] = lprcClip->bottom - y0;

	iStartDV = 1;
	iStartNum = 0;
	iEndDV = 1;
	iEndNum = 1;
	bVisible = TRUE;
	for( i = 0; i < 4 && bVisible; i++ )
	{
		int t, ts, te;	
		if( dv[i] > 0 )
		{    // end border
			t = num[i] * iStartDV;
			ts = iStartNum * dv[i];
			// if( End < Start )
			//        return FALSE;
			if( (iStartDV > 0 && t < ts ) ||
				(iStartDV < 0 && t > ts) )
			{
				bVisible = FALSE;
				break;
			}
			
			t = num[i] * iEndDV;
			te = iEndNum * dv[i];
			//if( CurEnd < PrevEnd )
			if( (iEndDV > 0 && t < te) ||
				(iEndDV < 0 && t > te) )
			{
				iEndDV = dv[i];
				iEndNum = num[i];
			}
		}
		else if( dv[i] < 0 )
		{
			t = num[i] * iEndDV;
			te = iEndNum * dv[i];
			//if( Start > End )
			//    return FALSE;
			if( (iEndDV > 0 && t < te) ||
				(iEndDV < 0 && t > te) )
			{
				bVisible = FALSE;
				break;
			}
			
			t = num[i] * iStartDV;
			ts = iStartNum * dv[i];
			//if( CurStart > PrevStart )
			if( (iStartDV > 0 && t < ts) ||
				(iStartDV < 0 && t > ts) )
			{
				iStartDV = dv[i];
				iStartNum = num[i];
			}
		}
		else if( num[i] > 0 )
		{
			bVisible = FALSE;
			break;
		}
	}
//	if( bVisible )
//	{
//		*lpx0 = x0 + dx * iStartNum / iStartDV;
//		*lpy0 = y0 + dy * iStartNum / iStartDV;
//		
//		*lpx1 = x0 + dx * iEndNum / iEndDV;
//		*lpy1 = y0 + dy * iEndNum / iEndDV;
//	}
	return bVisible;
}
*/

#define WIDTH_LEFT( w ) ( (w) >> 1 )
#define WIDTH_TOP( w ) ( (w) >> 1 )
#define WIDTH_RIGHT( w ) ( (w) - ((w) >> 1) - 1 )
#define WIDTH_BOTTOM( w ) ( (w) - ((w) >> 1) - 1 )

/*
//void DrawClipLine( HDC lpLine, int x0, int y0, int x1, int y1, LPRECT lprcClip )
#define DRAW_SCAN_LINE( x ) \
					  do{\
                         if( width == 1 ) \
							_ScanLine( lpLine, (x),  y0, xDir, xr - xl ); \
						 else \
						 { \
						     if( xDir > 0 ) \
							 {\
						         rectClip.left = (x) - lthalf; \
						         rectClip.right = (x) + xr - xl + rbhalf;\
							 }\
							 else\
							 {\
						         rectClip.right = (x) + 1 + rbhalf; \
						         rectClip.left = (x) - xr + xl + 1 - lthalf;\
							 }\
						     rectClip.top = y0 - lthalf;\
						     rectClip.bottom = y0 + 1 + rbhalf;\
						     if( IntersectRect( &rectClip, &rectClip, lpLine->lprcClip ) )\
							     _BlkBitBlt( &blt );\
						 } \
					  }while(0)

#define DRAW_VERT_LINE( y ) \
	                  do{ \
                         if( width == 1 ) \
							_VertialLine( lpLine, x0,  (y), yDir, yb - yt ); \
						 else \
						 { \
						     rectClip.left = x0 - lthalf; \
						     rectClip.right = x0 + 1 + rbhalf;\
						     if( yDir > 0 ) \
							 {\
						         rectClip.top = (y) - lthalf; \
						         rectClip.bottom = (y) + yb - yt + rbhalf;\
							 }\
							 else\
							 {\
						         rectClip.bottom = (y) + 1+ rbhalf; \
						         rectClip.top = (y) - yb + yt + 1 - lthalf;\
							 }\
						     if( IntersectRect( &rectClip, &rectClip, lpLine->lprcClip ) )\
							     _BlkBitBlt( &blt );\
						 } \
					  }while(0)

#define DRAW_PIXEL( x0, y0 ) \
					  do{\
					      if( width == 1 ) \
						      _ScanLine( lpLine, (x0),  (y0), 1, 1 );\
						  else{\
                              rectClip.left = (x0) - half;\
                              rectClip.right = rectClip.left + width;\
                              rectClip.top = (y0) - half;\
                              rectClip.bottom = rectClip.top + width;\
                              if( IntersectRect( &rectClip, &rectClip, lpLine->lprcClip ) )\
                                   _BlkBitBlt( &blt );\
							 }\
					  }while(0)

//static BOOL _DiagonalLine( _LPLINEDATA lpLine )

*/

// 4\5|6/7
//-----+------ 
// 3/2|1\0

static BOOL _SingleLine( _LPLINEDATA lpLine )
{
	long lErrorCount = lpLine->iErrorCount;//(long)(lpLine->dn) + lpLine->iErrorCount;
	long iAdjUp = (long)(lpLine->dn);
	long iAdjDown = (long)(lpLine->dn) - (long)(lpLine->dm);
	long num = lpLine->cPels;
	short x = lpLine->xStart;
	short y = lpLine->yStart;
	int iDir = lpLine->iDir;
	int xDir;
	int yDir;
	//int m, n;

	if( lpLine->dn == 0 )
	{   // hori or vert line
		if( iDir == 0 )
		    __ScanSolidLine( lpLine, x, y, 1, num );
		else if( iDir == 1 )
			__VertialSolidLine( lpLine, x, y, 1, num );
		else if( iDir == 3 )
			__ScanSolidLine( lpLine, x, y, -1, num );
		else if( iDir == 6 )
			__VertialSolidLine( lpLine, x, y, -1, num );
	}
	else
	{   // dia line
		LPLINECALLBACK  lpfnCallBack = lpLine->lpfnCallback;
		//pixel.
		if( iDir == 0 ||
			iDir == 7 ||
			iDir == 3 ||
			iDir == 4 )
		{   // x is main dir
			xDir = (iDir == 0 || iDir == 7) ? 1 : -1;
			yDir = (iDir == 0 || iDir == 3) ? 1 : -1;
			//_ScanLine( lpLine, x, y, 1, 1 ); 
			for( num = lpLine->cPels; num; num-- )
			{
			    //_ScanLine( lpLine, x, y, 1, 1 ); 
				__ScanSolidLine( lpLine, x, y, 1, 1 ); 

				if( lpfnCallBack )
				{
					RECT rc = { x, y, x + 1, y + 1 };
				    lpfnCallBack( lpLine, &rc );
				}

				if( lErrorCount < 0 )
					lErrorCount += iAdjUp;
				else
				{
					lErrorCount += iAdjDown;
					y += yDir;
				}
				x += xDir;				
			}
		}
		else
		{  // y is main dir  1 2 5 6
			xDir = (iDir == 1 || iDir == 6) ? 1 : -1;
			yDir = (iDir == 1 || iDir == 2) ? 1 : -1;
			
			for( num = lpLine->cPels; num; num-- )
			{
				//_ScanLine( lpLine, x, y, 1, 1 ); 
				__ScanSolidLine( lpLine, x, y, 1, 1 ); 

				if( lpfnCallBack )
				{
					RECT rc = { x, y, x + 1, y + 1 };
				    lpfnCallBack( lpLine, &rc );
				}

				if( lErrorCount < 0 )
					lErrorCount += iAdjUp;
				else
				{
					lErrorCount += iAdjDown;
					x += xDir;
				}
				y += yDir;
				//_ScanLine( lpLine, x, y, 1, 1 ); 
			}
		}
	}
	return TRUE;
}

static BOOL _MixLine( _LPLINEDATA lpLine )
{
	long lErrorCount = lpLine->iErrorCount;//(long)(lpLine->dn) + lpLine->iErrorCount;
	long iAdjUp = (long)(lpLine->dn);
	long iAdjDown = (long)(lpLine->dn) - (long)(lpLine->dm);
	long num = lpLine->cPels;
	short x = lpLine->xStart;
	short y = lpLine->yStart;
	int iDir = lpLine->iDir;
	int xDir;
	int yDir;
	int width = lpLine->width;
	//int m, n;
	RECT rectClip, rect;
    _BLKBITBLT blt;

	if( lpLine->width != 1 )
	{
		blt.lpDestImage = lpLine->lpDestImage;
		blt.lpBrush = 0;
		blt.solidColor = lpLine->color;
		blt.dwRop = PATCOPY;
		blt.lprcDest = &rectClip;//lpLine->lprcClip;
	}

	if( lpLine->dn == 0 )
	{   // hori or vert line
		if( iDir == 0 || iDir == 3 )
		{    // horial
			if( width == 1 )
				_ScanLine( lpLine, x, y, iDir == 0 ? 1 : -1, num );
			else
			{
				rect.top = y - WIDTH_TOP( width );
				rect.bottom = y + 1 + WIDTH_BOTTOM( width ); 
				if( iDir == 0 )
				{
					rect.left = x - WIDTH_LEFT( width );
					rect.right = x + num + WIDTH_RIGHT( width );				
				}
				else
				{   
					//rect.left = x - num + 1 - WIDTH_LEFT( width );// LN, 2003-06-11, DEL
					//rect.right = x + 1 + WIDTH_RIGHT( width );// LN, 2003-06-11, DEL
					rect.left = x - num + 1 - WIDTH_RIGHT( width );// LN, 2003-06-11, ADD
					rect.right = x + 1 + WIDTH_LEFT( width );// LN, 2003-06-11, ADD
				}
				if( IntersectRect( &rectClip, &rect, lpLine->lprcClip ) )
				{		
                    _BlkBitBlt( &blt );					
				}
			}
		}
		else if( iDir == 1 || iDir == 6 )
		{   // vertial
			if( width == 1 )
			{
				_VertialLine( lpLine, x, y, iDir == 1 ? 1 : -1, num ); 
			}
			else
			{
				rect.left = x - WIDTH_LEFT( width );
				rect.right = x + 1 + WIDTH_RIGHT( width );
				if( iDir == 1 )
				{				
					rect.top = y - WIDTH_TOP( width );				
					rect.bottom = y + num + WIDTH_BOTTOM( width );
				}
				else
				{				
					//rect.top = y - num + 1 - WIDTH_TOP( width );  // LN, 2003-06-11, DEL
					//rect.bottom = y + 1 +  WIDTH_BOTTOM( width ); // LN, 2003-06-11, DEL
					rect.top = y - num + 1 - WIDTH_BOTTOM( width ); // LN, 2003-06-11, ADD
					rect.bottom = y + 1 +  WIDTH_TOP( width );  // LN, 2003-06-11, ADD
				}
				if( IntersectRect( &rectClip, &rect, lpLine->lprcClip ) )
				{			
                    _BlkBitBlt( &blt );
				}
			}
		}
	}
	else
	{   // dia line
		//pixel.
		int lthalf = WIDTH_TOP( width );
		int rbhalf = WIDTH_BOTTOM( width );
		LPLINECALLBACK  lpfnCallBack = lpLine->lpfnCallback;

		if( iDir == 0 ||
			iDir == 7 ||
			iDir == 3 ||
			iDir == 4 )
		{   // x is main dir
			xDir = (iDir == 0 || iDir == 7) ? 1 : -1;
			yDir = (iDir == 0 || iDir == 3) ? 1 : -1;
			for( num = lpLine->cPels; num; num-- )
			{
				if( width == 1 )
				{
				    _ScanLine( lpLine, x, y, 1, 1 ); 
				}
				else
				{					
					rectClip.left = x - lthalf;
					rectClip.right = x + rbhalf + 1;
					rectClip.top = y - lthalf;
					rectClip.bottom = y + rbhalf + 1;
					if( IntersectRect( &rectClip, &rectClip, lpLine->lprcClip ) )
					{
						_BlkBitBlt( &blt );
					}
					else
						break;
				}
				if( lpfnCallBack )
				{
					if( width == 1 )
					{
						RECT rc = { x, y, x + 1, y + 1 };
					    lpfnCallBack( lpLine, &rc );
					}
					else
					{
						lpfnCallBack( lpLine, &rectClip );
					}
				}

				if( lErrorCount < 0 )
					lErrorCount += iAdjUp;
				else
				{
					lErrorCount += iAdjDown;
					y += yDir;
				}
				x += xDir;
			}
		}
		else 
		{  // y is main dir  1 2 5 6
			xDir = (iDir == 1 || iDir == 6) ? 1 : -1;
			yDir = (iDir == 1 || iDir == 2) ? 1 : -1;
			for( num = lpLine->cPels; num; num-- )
			{
				if( width == 1 )
				    _ScanLine( lpLine, x, y, 1, 1 ); 
				else
				{					
					rectClip.left = x - lthalf;
					rectClip.right = x + rbhalf + 1;
					rectClip.top = y - lthalf;
					rectClip.bottom = y + rbhalf + 1;
					if( IntersectRect( &rectClip, &rectClip, lpLine->lprcClip ) )
					{
						_BlkBitBlt( &blt );
					}
					else
						break;
				}

				if( lpfnCallBack )
				{
					if( width == 1 )
					{
						RECT rc = { x, y, x + 1, y + 1 };
					    lpfnCallBack( lpLine, &rc );
					}
					else
					{
						lpfnCallBack( lpLine, &rectClip );
					}
				}


				if( lErrorCount < 0 )
					lErrorCount += iAdjUp;
				else
				{
					lErrorCount += iAdjDown;
					x += xDir;
				}
				y += yDir;
			}
		}
	}
	return TRUE;
}

static BOOL _Line( _LPLINEDATA lpLine )
{
	if( lpLine->width == 1 && lpLine->pattern == 0xff )
	{  // single line
		return _SingleLine( lpLine );
	}
	else
	{
		return _MixLine( lpLine );
	}
}


/*
static BOOL _Line( _LPLINEDATA lpLine )
{
    if( lpLine->n )
    {  //diagonal line
        return _DiagonalLine( lpLine );
    }
    else
    {   // h or v line
        if( lpLine->k )
        {  //horitial line
            return _ScanLine( lpLine );
        }
        else
        {  //vertial line
            return _VertialLine( lpLine );
        }
    }         
}
*/
