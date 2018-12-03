#include <ewindows.h>
//#include <malloc.h>
//#include "ttffile.h"


#define DIR_UP      1
#define DIR_DOWN   (-1)

#define TAG_CONIC   0
#define TAG_ON      1
#define TAG_CUBIC   2

#define USE_CONIC_CUBIC 0
//#define VECTOR POINT
//#define LPVECTOR
typedef LONG POS;
typedef POINT VECTOR;
typedef VECTOR * LPVECTOR;

typedef BOOL (*LPLINE_CALL_BACK)( VOID * hLineData, int x0, int y, int x1 );

//#define POS USHORT

#define ABS( a ) ( (a) < 0 ? -(a) : (a) )

/*
typedef struct _VECTOR
{
	POS x;
	POS y;
}VECTOR, FAR * LPVECTOR;
*/
/*
typedef struct _GLYPH_DATA
{
	DWORD dwSize;
	//TTF_GLYPH_DATA_HEADER glyHeader;
	USHORT * lpusEndPtsOfContours;//Array of last points of each contour; n  is the number of contours
    //USHORT  usInstructionLength;//Total number of bytes for instructions.
    //BYTE   *lpbInstructions;//Array of instructions for each glyph; n  is the number of instructions.
	UINT   uDataLength;  // number of tag and vectors
    BYTE   *lpbFlags;//Array of flags for each coordinate in outline; n  is the number of flags.
	VECTOR *lpVectors;
    //SHORT  *lpuiXCoordinates;//First coordinates relative to (0,0); others are relative to previous point.
    //SHORT  *lpuiYCoordinates;//First coordinates relative to (0,0); others are relative to previous point.
}GLYPH_DATA;
*/
typedef struct _SNIPPET  //Æ¬¶Ï
{
	struct _SNIPPET FAR * lpNext;
	struct _SNIPPET FAR * lpNextList;

	SHORT  fDirection;  //fDirection DIR_UP, DIR_DOWN
	USHORT nHeight;
	POS   yEnd;
	//SHORT   dumy;
	POS   * lpX;
	//POS   iXArray[1];  //the array is variable, 
}SNIPPET, * PSNIPPET, FAR * LPSNIPPET;

typedef struct _SNIPPET_STATE
{
	int fCurDirection; //fDirection DIR_UP, DIR_DOWN
	LPSNIPPET lpCurSnippet;
	VECTOR  vtCurrent;
	LPBYTE lpbCachePos;
}SNIPPET_STATE, FAR * LPSNIPPET_STATE;


#define ALIGN_DWORD( lp ) ( ( ((DWORD)lp) + 3 ) & (~3) )

#define GET_TAG_TYPE( lptag ) ( (lptag) ? ( *(lptag) & 0x03 ) : TAG_ON )

typedef struct _CONTOUR
{
	struct _CONTOUR	FAR * lpNext;
	int num;
	LPBYTE lpTag;
	LPVECTOR lpVector;
}CONTOUR, * PCOUNTOUR, FAR * LPCONTOUR;

typedef struct _GLYPH
{
	struct _GLYPH * lpNext;

	UINT uiCode;
	LPBYTE lpbGlyphCache;
	
	POS * lpYPos;
	int yNum;
	short iConicLevel;
	short iCubicLevel;

	LPSNIPPET lpUpList;
	LPSNIPPET lpDownList;
}GLYPH, FAR * LPGLYPH;


typedef struct _FONT
{
	struct _FONT * lpNext;  // next font driver
	LPVECTOR lpVector;
	LPBYTE lpTag;
	LPBYTE lpbFontCache;
	LPBYTE lpbCachePos;
	LPGLYPH lpGlyph;
	DWORD dwCacheUsed;
}FONT, FAR * LPFONT;

static BOOL LineUp( LPSNIPPET_STATE lpState, POS  x1, POS  y1, POS  x2, POS  y2 )
{    
	int dm, sum, dy, dx, iMultiple, iRemainder, iGrain;
	int nCounter;
	POS * lpxCurPos = (POS*)lpState->lpbCachePos;//


	sum = dy = y2 - y1;
	dx = x2 - x1;
	iMultiple = dx / dy;
	iRemainder = dx % dy;
	
	iGrain = (iRemainder + iMultiple) / 2;
	if( dx > 0 )
	{
		dm = 1;
	}
	else
	{
		dm = -1;
		iRemainder = -iRemainder;
	}
	
	nCounter = 0;

	while( sum-- )
	{
		*lpxCurPos++ = x1;
		x1 += iMultiple;
		nCounter += iRemainder;
		if( nCounter >= dy )
		{
			nCounter -= dy;
			x1 += dm; 
		}
	

	}

	lpState->lpbCachePos = (LPBYTE)lpxCurPos;
	return TRUE;
}

static BOOL LineDown( LPSNIPPET_STATE lpState, POS  x1, POS  y1, POS  x2, POS  y2 )
{
	return LineUp( lpState, x1, -y1, x2, -y2 );
}

static BOOL BeginSnippet( LPGLYPH lpgh, int iDirection, LPSNIPPET_STATE lpState )
{
	LPSNIPPET lpst;
	
	lpst = (LPSNIPPET)lpState->lpbCachePos;
	lpst = (LPSNIPPET)ALIGN_DWORD( lpst );
	lpst->fDirection = iDirection;	

	lpst->lpNext = NULL;
	lpst->lpNextList = NULL;
	lpst->lpX = NULL;
	lpst->nHeight = 0;
	lpst->yEnd = 0;

    lpState->lpCurSnippet->lpNext = lpst;
	lpState->lpCurSnippet = lpst;
	lpState->fCurDirection = iDirection;
	lpState->lpbCachePos = (LPBYTE)lpst + sizeof( SNIPPET );
	
	return TRUE;
}

static BOOL EndSnippet( LPGLYPH lpGlyph, LPSNIPPET_STATE lpState )
{
	lpState->lpCurSnippet->nHeight = (lpState->lpbCachePos - (LPBYTE)lpState->lpCurSnippet - sizeof( SNIPPET )) / sizeof( POS );
	lpState->lpCurSnippet->yEnd = (POS)lpState->vtCurrent.y;
	return TRUE;
}

static BOOL DoLine( LPGLYPH lpgh, LPVECTOR lpvt, LPSNIPPET_STATE lpState )
{
	int iDirUp;

	if( lpvt->y > lpState->vtCurrent.y )
		iDirUp = DIR_UP;
	else if( lpvt->y < lpState->vtCurrent.y )
		iDirUp = DIR_DOWN;
	else
	{
	    lpState->vtCurrent.x = lpvt->x;
	    lpState->vtCurrent.y = lpvt->y;
		return TRUE;
	}

	if( lpState->fCurDirection != iDirUp )
	{
		EndSnippet( lpgh, lpState );  // close current snippet
		BeginSnippet( lpgh, iDirUp, lpState ); // open a new snippet
	}

#if 0   // test only
    {
		HPEN hpen = CreatePen( PS_SOLID, 1, RGB( red / 2, red /2, red/2 ) );
			
		hpen = SelectObject( hWindowDC, hpen );	
		//MoveToEx( hWindowDC, x1, ABS(y1) + 10, NULL );	
		//LineTo( hWindowDC, x2, ABS(y2) + 10 );
		MoveToEx( hWindowDC, lpState->vtCurrent.x, lpState->vtCurrent.y + 10, NULL );
		LineTo( hWindowDC, lpvt->x, lpvt->y + 10 );

		SelectObject( hWindowDC, hpen );
		DeleteObject( hpen );
		//red += 3;
		//red = (BYTE)(~red);
		//red /= 2;
	}
#endif 	
	
	if( iDirUp == DIR_UP )
		LineUp( lpState, lpState->vtCurrent.x, lpState->vtCurrent.y, lpvt->x, lpvt->y );
	else
		LineUp( lpState, lpState->vtCurrent.x, -lpState->vtCurrent.y, lpvt->x, -lpvt->y );

	lpState->vtCurrent.x = lpvt->x;
	lpState->vtCurrent.y = lpvt->y;
	return TRUE;
}

#ifdef USE_CONIC_CUBIC

static void ExtendConic( LPVECTOR lpVector )
{
    int m, n;
	LPVECTOR lpdot;
	int i;

	lpdot = (LPVECTOR)&lpVector->x;

	for( i = 0; i < 2; i++ )
	{
		*(POS*)(lpdot + 4) = *(POS*)(lpdot + 2);
		m = *(POS*)(lpdot + 1);
		n = *(POS*)(lpdot + 3) = ( *(POS*)(lpdot + 2) + m ) / 2;
		m = *(POS*)(lpdot + 1) = ( *(POS*)lpdot + m ) / 2;
		*(POS*)(lpdot + 2) = ( m + n ) / 2;
		
		lpdot = (LPVECTOR)&lpVector->y;
	}
}

static int DoConic( LPGLYPH lpgh, LPVECTOR lpvtControl, LPVECTOR lpvtTo, LPSNIPPET_STATE lpState )
{
    int deep;
	POS dmax, m;
    int iDeepStack[32], * lpStack;
	VECTOR vectors[128], * lpvt;
	VECTOR vt;
	
    m = lpState->vtCurrent.x + lpvtTo->x - lpvtControl->x * 2;
    if ( m < 0 )
		m = -m;
    dmax = lpState->vtCurrent.y + lpvtTo->y - lpvtControl->y * 2;
    if ( dmax < 0 )
		dmax = -dmax;
	
    if ( dmax < m )
		dmax = m;
	
    deep = 1;
	dmax /= lpgh->iConicLevel;
    while ( dmax > 0 )
    {
		dmax >>= 2;
		deep++;
    }
    
    lpvt = vectors;
	if ( deep <= 1 )
    {
		vt.x = ( lpState->vtCurrent.x + lpvtTo->x + 2 * lpvtControl->x ) / 4;
		vt.y = ( lpState->vtCurrent.y + lpvtTo->y + 2 * lpvtControl->y ) / 4;
		
		DoLine( lpgh, &vt, lpState );
		DoLine( lpgh, lpvtTo, lpState );
		return TRUE;
    }
	
    //in       = 0;
	lpStack = iDeepStack;
	*lpStack = deep;
	
    *lpvt = *lpvtTo;
    *(lpvt+1) = *lpvtControl;
    *(lpvt+2) = *(&lpState->vtCurrent);
	
    while ( lpStack >= iDeepStack )
    {
		deep = *lpStack;
		if ( deep > 1 )
		{
			POS miny, maxy;
			
			miny = maxy = lpvt->y;
			m = (lpvt+1)->y;
			if( miny > m )
				miny = m;
			else if( maxy < m )
				maxy = m;
			m = (lpvt+2)->y;
			if( miny > m )
				miny = m;
			else if( maxy < m )
				maxy = m;

			ExtendConic( lpvt );			
			lpvt += 2;
			deep--;
			*lpStack++ = deep;
			*lpStack = deep;			
			continue;
		}
		
			vt.x = ( lpState->vtCurrent.x + lpvt->x + 2 * (lpvt+1)->x ) / 4;
			vt.y = ( lpState->vtCurrent.y + lpvt->y + 2 * (lpvt+1)->y ) / 4;
			
			DoLine( lpgh, &vt, lpState );
			DoLine( lpgh, lpvt, lpState );
			
			lpvt -= 2;
			lpStack--;
    }
    return 0;
}

static  void  ExtendCubic( LPVECTOR  lpvt )
{
    POS a, b, c, d;
	LPVECTOR lpdot;
	int i;
	
	lpdot = (LPVECTOR)&lpvt->x;
	
	for( i = 0; i < 2; i++ )
	{
		*(POS*)(lpdot + 6) = *(POS*)(lpdot + 3);
		c = *(POS*)(lpdot + 1);
		d = *(POS*)(lpdot + 2);
		*(POS*)(lpdot + 1) = a = ( *(POS*)(lpdot + 0) + c ) / 2;
		*(POS*)(lpdot + 5) = b = ( *(POS*)(lpdot + 3) + d ) / 2;
		c = ( c + d ) / 2;
		*(POS*)(lpdot + 2) = a = ( a + c ) / 2;
		*(POS*)(lpdot + 4) = b = ( b + c ) / 2;
		*(POS*)(lpdot + 3) = ( a + b ) / 2;
		
		lpdot = (LPVECTOR)&lpvt->y;
	}
}


static int DoCubic( LPGLYPH lpgh, 
				   LPVECTOR lpvtCtl1,
				   LPVECTOR lpvtCtl2, 
				   LPVECTOR lpvtTo, 
				   LPSNIPPET_STATE lpState )
{
	POS m, n, dmaxa, dmaxb;
    int deep;
    int iDeepStack[32], * lpStack;
	VECTOR vt, vectors[128], * lpvt;
	
    m = lpState->vtCurrent.x + lpvtTo->x - lpvtCtl1->x * 2;
    n = lpState->vtCurrent.y + lpvtTo->y - lpvtCtl1->y * 2;
	if( ABS( m ) < ABS( n ) )
		dmaxa = n;
	else
		dmaxa = m;
	if( dmaxa < 0 )
		dmaxa = -dmaxa;
	
    m = lpState->vtCurrent.x + lpvtTo->x - 3 * ( lpvtCtl1->x + lpvtCtl2->x );
    n = lpState->vtCurrent.y + lpvtTo->y - 3 * ( lpvtCtl1->y + lpvtCtl2->y );
	
	if( ABS( m ) < ABS( n ) )
		dmaxb = n;
	else
		dmaxb = m;
	if( dmaxb < 0 )
		dmaxb = -dmaxb;
	
	deep = 1;
	dmaxa /= lpgh->iCubicLevel;
	dmaxb /= lpgh->iConicLevel;
    while ( dmaxa > 0 || dmaxb > 0 )
    {
		dmaxa >>= 2;
		dmaxb >>= 3;
		deep++;
    }
	
    if ( deep <= 1 )
    {
		vt.x = lpState->vtCurrent.x + lpvtTo->x + 3 * ( lpvtCtl1->x + lpvtCtl2->x ) / 8;
		vt.x = lpState->vtCurrent.y + lpvtTo->y + 3 * ( lpvtCtl1->y + lpvtCtl2->y ) / 8;
		
		DoLine( lpgh, &vt, lpState );
		DoLine( lpgh, lpvtTo, lpState );
    }
	
    lpvt = vectors;
	*lpvt = *lpvtTo;
	*(lpvt+1) = *lpvtCtl2;
	*(lpvt+2) = *lpvtCtl1;
	*(lpvt+3) = *(&lpState->vtCurrent);
	
    lpStack = iDeepStack;
	*lpStack = deep;
	
    while ( lpStack >= iDeepStack )
    {
		deep = *lpStack;
		if ( deep > 1 )
		{
			POS  miny, maxy;
			
			miny = maxy = lpvt->y;
			m = (lpvt+1)->y;
			if( miny > m )
				miny = m;
			else if( maxy < m )
				maxy = m;
			m = (lpvt+2)->y;
			if( miny > m )
				miny = m;
			else if( maxy < m )
				maxy = m;
			
			m = (lpvt+3)->y;
			if( miny > m )
				miny = m;
			else if( maxy < m )
				maxy = m;
			
            ExtendCubic( lpvt );
			
			deep--;
			*lpStack++ = deep;
			*lpStack = deep;			
            lpvt += 3;
			
            continue;
		}
		
		vt.x = ( lpState->vtCurrent.x + lpvt->x + 3 * ( (lpvt+1)->x + (lpvt+2)->x ) ) / 8;
		vt.y = ( lpState->vtCurrent.y + lpvt->y + 3 * ( (lpvt+1)->y + (lpvt+2)->y ) ) / 8;
		
		DoLine( lpgh, &vt, lpState );
		DoLine( lpgh, lpvt, lpState );
		
        lpStack--;
		lpvt -= 3;
    }
    return 0;
}

#endif //USE_CONIC_CUBIC


static BOOL SplitContour( LPGLYPH lpgh, LPCONTOUR lpct, SNIPPET_STATE * lpState )
{
	LPBYTE lpTag = lpct->lpTag;
	LPVECTOR lpvtStart, lpvt = lpct->lpVector;
	LPVECTOR lpvtEnd = lpvt + lpct->num;
	//int num = lpct->num;

	//memset( &spState, 0, sizeof( SNIPPET_STATE ) );
	lpState->lpCurSnippet = (LPSNIPPET)(lpState->lpbCachePos);
	memset( lpState->lpCurSnippet, 0, sizeof( SNIPPET ) );	
	lpState->vtCurrent.x = lpct->lpVector->x;
	lpState->vtCurrent.y = lpct->lpVector->y;
	lpvtStart = lpvt;
    
	while( lpvt < lpvtEnd )
	{
		switch( GET_TAG_TYPE( lpTag ) )
		{
		case TAG_ON:
			DoLine( lpgh, lpvt, lpState );
			break;
#ifdef USE_CONIC_CUBIC

		case TAG_CONIC:
			{
				VECTOR vtControl;
				vtControl = *lpvt;
				while( lpvt < lpvtEnd )
				{
					lpvt++;
					lpTag++;
					if( *lpTag == TAG_ON )
					{
						DoConic( lpgh, &vtControl, lpvt, lpState );
						goto _DO_END;
					}
					else if( *lpTag == TAG_CONIC )
					{
						VECTOR vtMid;
						
						vtMid.x = (vtControl.x + lpvt->x) / 2;
						vtMid.y = (vtControl.y + lpvt->y) / 2;
						DoConic( lpgh, &vtControl, &vtMid, lpState );
						
						vtControl = *lpvt;
					}
				}
				DoConic( lpgh, &vtControl, lpvtStart, lpState );
_DO_END:
				;
			}
			break;
		default:
            if( lpvt + 1 >= lpvtEnd ||
                *(lpTag+1) != TAG_CUBIC )
				break;   // error

            if ( lpvt + 2 < lpvtEnd )
				DoCubic( lpgh, lpvt, lpvt + 1, lpvt + 2, lpState );
			else
                DoCubic( lpgh, lpvt, lpvt + 1, lpvtStart, lpState );
			lpvt += 2;
			lpTag += 2;
			break;
#endif	//USE_CONIC_CUBIC

		}
		lpvt++;
		if( lpTag )
			lpTag++;
	}

	DoLine( lpgh, lpvtStart, lpState );
	EndSnippet( lpgh, lpState );  // close current snippet
	return TRUE;
}

static BOOL InitList( LPSNIPPET lpst )
{
	while( lpst )
	{
		lpst->lpNextList = lpst->lpNext;
		if( lpst->fDirection == DIR_DOWN )
		    lpst->lpX = (POS*)(lpst + 1) + lpst->nHeight - 1;
		else
			lpst->lpX = (POS*)(lpst + 1);
		lpst = lpst->lpNext;
	}
	return TRUE;
}

static BOOL UpdateList( LPSNIPPET * lppstDest, LPSNIPPET * lppstSrc, int miny, int maxy )
{
	LPSNIPPET lpPrev, lpNext, lpst;
	int bInsert;

	// del snippet in lppstDest that is not in range miny ~ maxy;
	lpst = *lppstDest;
	lpPrev = NULL;
	
	while( lpst )
	{
		bInsert = TRUE;
		lpNext = lpst->lpNextList;
		if( lpst->fDirection == DIR_UP )
		{
			if( lpst->yEnd < maxy || lpst->yEnd - lpst->nHeight > miny )
				bInsert = FALSE;
		}
		else
		{
			if( lpst->yEnd > miny || lpst->yEnd + lpst->nHeight < maxy )
				bInsert = FALSE;
		}
		if( bInsert == FALSE )
		{
			if( lpPrev )
				lpPrev->lpNextList = lpNext;
			else
				*lppstDest = lpNext;
		}
		else
			lpPrev = lpst;
		lpst = lpNext;
	}
	
	// insert snippet in lppstSrc that is in range miny ~ maxy;
	
	lpPrev = NULL;
	lpst = *lppstSrc;

	while( lpst )
	{
		bInsert = FALSE;
		lpNext = lpst->lpNextList;
		if( lpst->fDirection == DIR_UP )
		{
			if( lpst->yEnd >= maxy && lpst->yEnd - lpst->nHeight <= miny )
				bInsert = TRUE;
		}
		else
		{
			if( lpst->yEnd <= miny && lpst->yEnd + lpst->nHeight >= maxy )
				bInsert = TRUE;
		}
		if( bInsert )
		{
			// remove lpst from lpsrSrc
			if( lpPrev )
				lpPrev->lpNextList = lpNext;
			else
				*lppstSrc = lpNext;

			lpst->lpNextList = *lppstDest;
			*lppstDest = lpst;
		}
		else
		    lpPrev = lpst;
		lpst = lpNext;
	}
	return TRUE;
}

static BOOL NextX( LPSNIPPET lpst )
{
	while( lpst )
	{
		lpst->lpX += lpst->fDirection;
		lpst = lpst->lpNextList;
	}
	return TRUE;
}
static void  SortX( LPSNIPPET * lppList )
{
	LPSNIPPET  *lppOld, lpCur, lpNext;
	
	
	lppOld     = lppList;
	lpCur = *lppOld;
	
	if ( lpCur == NULL )
		return;
	
	lpNext = lpCur->lpNextList;
	
	while( lpNext )
	{
		if ( *lpCur->lpX <= *lpNext->lpX )
		{
			lppOld     = &lpCur->lpNextList;
			lpCur = *lppOld;
			
			if ( lpCur == NULL )
				return;
		}
		else
		{
			*lppOld          = lpNext;
			lpCur->lpNextList = lpNext->lpNextList;
			lpNext->lpNextList = lpCur;
			
			lppOld     = lppList;
			lpCur = *lppOld;
		}
		
		lpNext = lpCur->lpNextList;
	}
}


static BOOL DrawGlyph( HDC hdc, LPGLYPH lpgh, LPLINE_CALL_BACK lpLineCallBack, VOID * hLineData )
{
	LPSNIPPET lpUp, lpDown, lpLeft, lpRight;
	int maxy, miny;
	int yScanNum;
	POS * lpy;

	lpLeft = lpRight = NULL;

	lpUp = lpgh->lpUpList;
	lpDown = lpgh->lpDownList;

	InitList( lpUp );
	InitList( lpDown );

	yScanNum = lpgh->yNum;

	lpy = lpgh->lpYPos;
	miny = *lpy++;
	yScanNum--;
    if( yScanNum == 0 )
		maxy = miny;
	else
	{
		maxy = *lpy++;
		yScanNum--;
	}

	do
	{		
		UpdateList( &lpLeft, &lpUp, miny, maxy );
		UpdateList( &lpRight, &lpDown, miny, maxy );
		SortX( &lpLeft );
		SortX( &lpRight );
		for( ; miny < maxy; miny++ )
		{
            LPSNIPPET lpl, lpr;

			lpl = lpLeft, lpr = lpRight;
			while( lpl )
			{
				if( lpLineCallBack )
				{
					//lpLineCallBack( hLineData, *lpl->lpX, miny, *lpr->lpX+1 );
					lpLineCallBack( hLineData, *lpr->lpX, miny, *lpl->lpX+1 );
				}
				else
				{
					MoveToEx( hdc, *lpl->lpX, miny, NULL );
					LineTo( hdc, *lpr->lpX+1, miny );
				}

				lpl = lpl->lpNextList;
				lpr = lpr->lpNextList;
			}
			NextX( lpLeft );
			NextX( lpRight );
		}	
		if( yScanNum )
		{
		    miny = maxy;
	        maxy = *lpy++;
			yScanNum--;
		}
		else
			break;		
	}while( 1 );
	return TRUE;
}

BOOL ScanSnippet( LPGLYPH lpgh, SNIPPET_STATE * lpState )
{
	LPSNIPPET lpst = (LPSNIPPET)lpgh->lpbGlyphCache;
	LPSNIPPET lpNext;
	//POS * lpy = lpState->lpbCachePos;
	int num = 0;
	int y, miny, maxy;
	int i;
	
	while( lpst )
	{
		// insert y to lpy;
		
		lpNext = lpst->lpNext;
		if( lpst->fDirection == DIR_UP )
		{
			miny = lpst->yEnd - lpst->nHeight;
			maxy = lpst->yEnd;

			lpst->lpNext = lpgh->lpUpList;
			lpgh->lpUpList = lpst;
		}
		else
		{
			miny = lpst->yEnd;
			maxy = lpst->yEnd + lpst->nHeight;

			lpst->lpNext = lpgh->lpDownList;
			lpgh->lpDownList = lpst;
		}
		// insert miny and maxy to lpy
		for( i = 0, y = miny; i < 2; i++, y = maxy )
		{
			if( num )
			{
				int k = 0;
				POS * lp = (POS*)lpState->lpbCachePos;

				while( k < num && y > *lp ){ k++; lp++; };
				if( k == num )
				{
					*lp = y;
					num++;
				}
				else if( y < *lp )
				{
					int j = num;
					lp = (POS*)lpState->lpbCachePos + num;
					while( k < j )
					{
						*lp = *( lp - 1 );
						lp--; j--;
					}
					*lp = y;
					num++;
				}
			}
			else
			{
				*((POS*)lpState->lpbCachePos) = y;
				num++;
			}
		}
		// 
		lpst = lpNext;
	}

	lpgh->lpYPos = (POS*)lpState->lpbCachePos;
	lpgh->yNum = num;
	lpState->lpbCachePos += num * sizeof( POS );
	return TRUE;
}

static LPBYTE ScanGlyph( LPGLYPH lpgh, LPCONTOUR lpct )
{
	SNIPPET_STATE spState;
	spState.fCurDirection = 0;
	spState.lpCurSnippet = NULL;
	spState.vtCurrent.x = spState.vtCurrent.y = 0;
	spState.lpbCachePos = lpgh->lpbGlyphCache;
	while( lpct )
	{
		SplitContour( lpgh, lpct, &spState );
		lpct = lpct->lpNext;
	}

	ScanSnippet( lpgh, &spState );	

	return spState.lpbCachePos;
}

static VOID BeginGlyph( LPFONT lpft, LPGLYPH lpgh )
{
	memset( lpgh, 0, sizeof( GLYPH ) );
	lpgh->lpbGlyphCache = (LPBYTE)lpft->lpbCachePos;
	lpgh->lpUpList = lpgh->lpDownList = NULL;
	lpgh->iConicLevel = 16;
	lpgh->iCubicLevel = 8;
}

static VOID EndGlyph( LPFONT lpft, LPGLYPH lpgh )
{
	//lpft->dwCacheUsed += lpgh->dwCacheUsed;
}
/*
static LPBYTE ScanGlyphData( HDC hdc, LPGLYPH lpgh, GLYPH_DATA *lpGlyData )
{
	SNIPPET_STATE spState;
	LPSNIPPET lpPrevSnippet, lpNextSnippet = NULL;
	CONTOUR ct;
	int i;

	spState.fCurDirection = 0;
	spState.lpCurSnippet = NULL;
	spState.vtCurrent.x = spState.vtCurrent.y = 0;
	spState.lpbCachePos = lpgh->lpbGlyphCache;

	ct.lpNext = NULL;
	ct.lpTag = lpGlyData->lpbFlags;
	ct.lpVector = lpGlyData->lpVectors;
	ct.num = lpGlyData->lpusEndPtsOfContours[0] + 1;
	SplitContour( lpgh, &ct, &spState );
	for( i = 1; i < lpGlyData->glyHeader.siNumberOfContours; i++ )
	{
		lpPrevSnippet = spState.lpCurSnippet;
		spState.fCurDirection = 0;
		spState.lpCurSnippet = NULL;
	    spState.vtCurrent.x = spState.vtCurrent.y = 0;

		ct.lpTag = lpGlyData->lpbFlags + lpGlyData->lpusEndPtsOfContours[i-1] + 1;
		ct.lpVector = lpGlyData->lpVectors + lpGlyData->lpusEndPtsOfContours[i-1] + 1;
		ct.num = lpGlyData->lpusEndPtsOfContours[i] - lpGlyData->lpusEndPtsOfContours[i-1];

		// align 4 byte(dword)
		lpNextSnippet = (LPSNIPPET)( ( ((DWORD)spState.lpbCachePos + 3) / 4 ) * 4 );  
		spState.lpbCachePos = (LPBYTE)lpNextSnippet;
		if( SplitContour( lpgh, &ct, &spState ) )
		{
			lpPrevSnippet->lpNext = lpNextSnippet;
		}

	}

	ScanSnippet( lpgh, &spState );

	DrawGlyph( hdc, lpgh );

	return spState.lpbCachePos;
}
*/

VOID * Glyph_Begin( LPVECTOR lpvt, LPBYTE lpTag, int num )
{
	FONT * lpFont;

	lpFont = (FONT *)malloc( sizeof(FONT) + 1024 * 20 );
	
	if( lpFont )
	{
		CONTOUR ct;
		//LPGLYPH lpGlyph;

		lpFont->lpbFontCache = (BYTE*)(lpFont + 1);		
		
		lpFont->lpNext = NULL;
		lpFont->lpTag = lpTag;
		lpFont->lpVector = lpvt;
		lpFont->lpGlyph = (LPGLYPH)lpFont->lpbFontCache;
		lpFont->lpbCachePos = lpFont->lpbFontCache + sizeof( GLYPH );
		
		BeginGlyph( lpFont, lpFont->lpGlyph );

		ct.lpNext = NULL;
		ct.lpVector = lpvt;
		ct.lpTag = lpTag;
		ct.num = num;

		lpFont->lpbCachePos = ScanGlyph( lpFont->lpGlyph, &ct );

		//free( lpFont );
		return lpFont;
	}
	return 0;
}


VOID Glyph_Fill( VOID * hGlyph, LPLINE_CALL_BACK lpLineCallBack, VOID * hLineData )
{
	FONT * lpFont = (FONT*)hGlyph;

	DrawGlyph( NULL, lpFont->lpGlyph, lpLineCallBack, hLineData );
}

VOID Glyph_End( VOID * h )
{
	free( h );
}

/*
int TestTTF( HDC hdc, LPVECTOR lpvt, LPBYTE lpTag, int num )
{
	FONT font;

	font.lpbFontCache = (LPBYTE)malloc( 1024 * 20 );
	if( font.lpbFontCache )
	{
		CONTOUR ct;
		LPGLYPH lpGlyph;
		
		font.lpNext = NULL;
		font.lpTag = lpTag;
		font.lpVector = lpvt;
		lpGlyph = (LPGLYPH)font.lpbFontCache;
		font.lpbCachePos = font.lpbFontCache + sizeof( GLYPH );
		//font.dwCacheUsed = 0;
		BeginGlyph( &font, lpGlyph );

		ct.lpNext = NULL;
		ct.lpVector = lpvt;
		ct.lpTag = lpTag;
		ct.num = num;

		font.lpbCachePos = ScanGlyph( lpGlyph, &ct );
		DrawGlyph( hdc, lpgh, NULL, NULL );

		free( font.lpbFontCache );
	}
	return 0;
}
*/

/*
BOOL MyPolyPolygon(
  HDC hdc,                  // handle to DC
  CONST POINT *lpPoints,    // array of vertices
  CONST INT *lpPolyCounts,  // array of count of vertices
  int nCount                // count of polygons
)
{
	return 0;
}
*/
