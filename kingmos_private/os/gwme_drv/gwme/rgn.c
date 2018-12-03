/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：矩形区域管理器
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <gwmeobj.h>
#include <eassert.h>
#include <bheap.h>

#define __USE_MEM

// private proc define start
#define INSERT_NODE_BEFORE( lpdst, lpsrc )  { (lpsrc)->lpNext = (lpdst); (lpdst) = (lpsrc); }
#define SET_LPRECTNODE_VALUE( lpRect, l, t, r, b ) { (lpRect)->rect.left = l; (lpRect)->rect.top = t; (lpRect)->rect.right = r; (lpRect)->rect.bottom = b; }
#define SET_RECT_VALUE( rect, l, t, r, b ) { rect.left = l; rect.top = t; rect.right = r; rect.bottom = b; }

static int _AndRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpSrc2, BOOL sFlag );
static int _CopyRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc);
static int _DiffRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpRemove, BOOL sFlag );
static int _OrRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpSrc2, BOOL sFlag );
static int _XorRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpSrc2, BOOL sFlag );

static void *__AllocMem( void );
static void __FreeMem( void* );
static BOOL _DeleteObject( _LPRGNDATA lpData );

static int __AppendNodes( _LPRGNDATA lpData, _LPRGNDATA lpSrc );
static int __CalcRgnBox( _LPRECTNODE lpNodeFirst, LPRECT lpRect );
static int __SortAndUnite( _LPRGNDATA lpData );
#define FIX_NO_MEM -1
#define FIX_NO_INTERSECT 0
#define FIX_REMOVE_RECT 1
#define FIX_SPLIT_RECT 2
static int __FixRects( _LPRECTNODE *lpData, LPRECT lpDest, LPRECT lpRemove );
static int __FreeNodes( _LPRECTNODE lpNodes );
static _LPRECTNODE __LinkNodesAfter( _LPRECTNODE lpDest, _LPRECTNODE lpSrc );
// private proc define end

#define __AllocRectNode (_LPRECTNODE)__AllocMem


#define _SET_LPRECT_ZERO( lprc ) ((lprc)->left = (lprc)->top = (lprc)->right = (lprc)->bottom = 0)
#define _SET_RECT_ZERO( rect ) ((rect).left = (rect).top = (rect).right = (rect).bottom = 0)
#define IS_RECT_EQUAL( rc1, rc2 )  ( (rc1).left == (rc2).left &&\
                                     (rc1).top == (rc2).top &&\
                                     (rc1).right == (rc2).right &&\
                                     (rc1).bottom == (rc2).bottom)
#define OFFSET_RECT( rect, xOffset, yOffset ) { (rect).left += (xOffset);\
                                                (rect).right += (xOffset);\
                                                (rect).top += (yOffset);\
                                                (rect).bottom += (yOffset); }
#define UNION_RECT( rect0, rect1 ) { rect0.left = MIN( rect0.left, rect1.left );\
                                     rect0.top = MIN( rect0.top, rect1.top );\
                                     rect0.right = MAX( rect0.right, rect1.right );\
                                     rect0.bottom = MAX( rect0.bottom, rect1.bottom ); }

#define FORMAT_XY( l, t, r, b ) \
                 do{ \
                     int temp; \
                     if( (l) > (r) ) { temp = (l), (l) = (r), (r) = temp; } \
					 if( (t) > (b) ) { temp = (t), (t) = (b), (b) = temp; } \
				 }while(0);
//#define USE_USERHEAP 
//#ifdef USE_USERHEAP

//#define malloc  malloc
//#define free   free
//#define _krealloc realloc

//#endif


extern BOOL AddToObjMgr( LPOBJLIST lpObj, DWORD objType, ULONG hOwner );

////////////////////////////////////////////////////////////
// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static _LPRGNDATA __AllocRgnObjectHandle( void )
{
	return BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof( _RGNDATA ) );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static void __FreeRgnObjectHandle( _LPRGNDATA lpRgn )
{
	BlockHeap_Free( hgwmeBlockHeap, 0, lpRgn, sizeof(_RGNDATA) );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static HRGN DoCreateRectRgn( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, UINT uiObjFlag )
{
	_LPRGNDATA lpRgn = __AllocRgnObjectHandle();
	
	FORMAT_XY( nLeftRect, nTopRect, nRightRect, nBottomRect );

    if( lpRgn )
    {
        lpRgn->obj.objType = OBJ_REGION;
		
		lpRgn->count = 0;        
        if( nLeftRect < nRightRect && nTopRect < nBottomRect )
        {
            lpRgn->lpNodeFirst = __AllocRectNode();
            if( lpRgn->lpNodeFirst )
            {
                SET_LPRECTNODE_VALUE( lpRgn->lpNodeFirst, nLeftRect, nTopRect, nRightRect, nBottomRect );
                lpRgn->lpNodeFirst->lpNext = 0;
                lpRgn->count = 1;
                lpRgn->rect = lpRgn->lpNodeFirst->rect;
            }
            else
            {
                _DeleteObject( lpRgn );
                lpRgn = NULL;
            }
        }
        else
        {
            lpRgn->lpNodeFirst = 0;
            _SET_RECT_ZERO( lpRgn->rect );
        }
    }
    ASSERT_NOTIFY( lpRgn, "CreateRectRgn: No enough rgn resource\r\n" );
	if( lpRgn )
	{
		extern BOOL Gdi_AddToObjMgr( LPOBJLIST lpObj, DWORD objType, HANDLE hOwner );
	
		AddToObjMgr( &lpRgn->obj, OBJ_REGION | uiObjFlag, (ULONG)GetCallerProcess() );
	}

	if( lpRgn )
	    return (HRGN)PTR_TO_HANDLE( lpRgn );
	return NULL;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

HRGN WINAPI WinRgn_CreateRect( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect )
{
	return DoCreateRectRgn( nLeftRect, nTopRect, nRightRect, nBottomRect, 0 );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

HRGN _CreateSysRgn( int nLeftRect, int nTopRect, int nRightRect, int nBottomRect )
{
	return DoCreateRectRgn( nLeftRect, nTopRect, nRightRect, nBottomRect, OBJ_FREE_DISABLE );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

HRGN WINAPI WinRgn_CreateRectIndirect(LPCRECT lpRect)
{
    return WinRgn_CreateRect( lpRect->left, lpRect->top, lpRect->right, lpRect->bottom );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int WINAPI WinRgn_Combine( HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2, int fnCombineMode )
{
    int retv = ERROR;
	_LPRGNDATA lpRgnDest, lpRgnSrc1, lpRgnSrc2;

	lpRgnDest = _GetHRGNPtr( hrgnDest );	
    if( lpRgnDest == NULL )
        goto L_RET;
    switch( fnCombineMode )
    {
        case RGN_AND:
			//ASSERT( hrgnSrc1 && hrgnSrc2 );
			lpRgnSrc1 = _GetHRGNPtr( hrgnSrc1 );
			lpRgnSrc2 = _GetHRGNPtr( hrgnSrc2 );
			if( lpRgnSrc1 && lpRgnSrc2 )
				retv = _AndRgn( lpRgnDest, lpRgnSrc1, lpRgnSrc2, TRUE );
            break;
        case RGN_COPY:
			///ASSERT( hrgnSrc1 );
			lpRgnSrc1 = _GetHRGNPtr( hrgnSrc1 );
			if( lpRgnSrc1 )
                retv = _CopyRgn( lpRgnDest, lpRgnSrc1 );
            break;
        case RGN_DIFF:
            //ASSERT( hrgnSrc1 && hrgnSrc2 );
			lpRgnSrc1 = _GetHRGNPtr( hrgnSrc1 );
			lpRgnSrc2 = _GetHRGNPtr( hrgnSrc2 );
			if( lpRgnSrc1 && lpRgnSrc2 )
                retv = _DiffRgn( lpRgnDest, lpRgnSrc1, lpRgnSrc2, TRUE );
            break;
        case RGN_OR:
            //ASSERT( hrgnSrc1 && hrgnSrc2 );
			lpRgnSrc1 = _GetHRGNPtr( hrgnSrc1 );
			lpRgnSrc2 = _GetHRGNPtr( hrgnSrc2 );
			if( lpRgnSrc1 && lpRgnSrc2 )
                retv = _OrRgn( lpRgnDest, lpRgnSrc1, lpRgnSrc2, TRUE );
            break;
        case RGN_XOR:
            //ASSERT( hrgnSrc1 && hrgnSrc2 );
			lpRgnSrc1 = _GetHRGNPtr( hrgnSrc1 );
			lpRgnSrc2 = _GetHRGNPtr( hrgnSrc2 );
			if( lpRgnSrc1 && lpRgnSrc2 )
                retv = _XorRgn( lpRgnDest, lpRgnSrc1, lpRgnSrc2, TRUE );
            break;
        default:
            ASSERT( 0 );
    }

	// only for test
	//_CheckObjList();
L_RET:
    //LeaveCriticalSection( &csRgn );
    if( retv != ERROR )
    {
        if( (retv = lpRgnDest->count) == 0 )
            return NULLREGION;
        else if( retv == 1 )
            return SIMPLEREGION;
        else
            return COMPLEXREGION;
    }

    return retv;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

static BOOL _DeleteObject( _LPRGNDATA lpRgn )
{
	lpRgn->obj.objType = OBJ_NULL;
	__FreeNodes( lpRgn->lpNodeFirst );
	__FreeRgnObjectHandle( lpRgn );
	
	return TRUE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL _DeleteRgnObject(HRGN hrgn)
{
    if( hrgn )
	{
		_LPRGNDATA lpRgn = _GetHRGNPtr( hrgn );
		if( lpRgn && !(lpRgn->obj.objType & OBJ_FREE_DISABLE) )
		{     
			return _DeleteObject( lpRgn );
		}
	}
    return FALSE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL WINAPI WinRgn_Equal(HRGN hSrcRgn1, HRGN hSrcRgn2)
{
    int retv = ERROR;
	_LPRECTNODE lpNode1, lpNode2;
    _LPRGNDATA lpSrc1, lpSrc2;

	//EnterCriticalSection( &csRgn );

	lpSrc1 = _GetHRGNPtr( hSrcRgn1 );
	lpSrc2 = _GetHRGNPtr( hSrcRgn2 );
	//ASSERT( lpSrc1 && lpSrc2 );

    if( lpSrc1 && lpSrc2 )
    {
        //lpSrc1 = _GET_HRGN_PTR( hSrcRgn1 );
        //lpSrc2 = _GET_HRGN_PTR( hSrcRgn2 );
        ASSERT( lpSrc1->count >= 0 && lpSrc2->count >= 0 );

        lpNode1 = lpSrc1->lpNodeFirst;
        lpNode2 = lpSrc2->lpNodeFirst;
        if(  lpSrc1->count == lpSrc2->count &&
             IS_RECT_EQUAL(lpSrc1->rect, lpSrc2->rect) )
        {
            lpNode1 = lpSrc1->lpNodeFirst;
            lpNode2 = lpSrc2->lpNodeFirst;
            while( lpNode1 && lpNode2 )
            {
                if( !IS_RECT_EQUAL( lpNode1->rect, lpNode2->rect ) )
				{
                    retv = FALSE;//return FALSE;
					goto L_RET;
				}
                lpNode1 = lpNode1->lpNext;
                lpNode2 = lpNode2->lpNext;
            }
            if( lpNode1 == 0 && lpNode2 == 0 )
                retv = TRUE;//return TRUE;
        }
		else
			retv = FALSE;
        //return FALSE;
    }
L_RET:
    //LeaveCriticalSection( &csRgn );
    return retv;//ERROR;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int  WINAPI WinRgn_GetBox(HRGN hrgn, LPRECT lprc)
{
	_LPRGNDATA lpRgn;
	int retv = ERROR;

	lpRgn = _GetHRGNPtr( hrgn );
	//ASSERT( lpRgn );

    if( lpRgn )
	{
        int i;

        *lprc = lpRgn->rect;
		if( (i = lpRgn->count) == 0 )
		{
			retv = NULLREGION;//return NULLREGION;
			ASSERT( lpRgn->lpNodeFirst == NULL );
		}
		else if( i == 1 )
			retv = SIMPLEREGION;//return SIMPLEREGION;
		else
			retv = COMPLEXREGION;//return COMPLEXREGION;
	}

    return retv;//ERROR;

}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

// the option export for win.c
int _GetRgnInfo(HRGN hrgn)
{
	_LPRGNDATA lprgn = _GetHRGNPtr( hrgn );
	ASSERT( lprgn );
    if( lprgn )
    {
		int i;

        if( (i = lprgn->count) == 0 )
		{
			ASSERT( lprgn->lpNodeFirst == NULL );
            return NULLREGION;
		}
        else if( i == 1 )
            return SIMPLEREGION;
        else
            return COMPLEXREGION;
    }
    return ERROR;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int  WINAPI WinRgn_Offset(HRGN hrgn, int xOffset, int yOffset)
{
    _LPRGNDATA lprgn;
	int retv = ERROR;

	lprgn = _GetHRGNPtr( hrgn );

    if( lprgn )
    {
		_LPRECTNODE lpNode;
		int i;

		
        ASSERT( lprgn->count >=0 );
        OFFSET_RECT( lprgn->rect, xOffset, yOffset );
        lpNode = lprgn->lpNodeFirst;
        while( lpNode )
        {
            OFFSET_RECT( lpNode->rect, xOffset, yOffset );
            lpNode = lpNode->lpNext;
        }
		if( (i = lprgn->count) == 0 )
			retv = NULLREGION;
		else if( i == 1 )
			retv = SIMPLEREGION;
		else
			retv = COMPLEXREGION;

	}
	
    return retv;//ERROR;

}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL WINAPI WinRgn_PtInRegion(HRGN hrgn, int x, int y)
{
    _LPRGNDATA lprgn = _GetHRGNPtr( hrgn );
	int retv = FALSE;

    if( lprgn )
	{
		_LPRECTNODE lpNode;        		

        lpNode = lprgn->lpNodeFirst;
        ASSERT( lprgn->count >= 0 );
        while( lpNode )
        {
            if( x >= lpNode->rect.left && x < lpNode->rect.right &&
                y >= lpNode->rect.top && y < lpNode->rect.bottom )
			{
                retv =  TRUE;   // in rect
				break;
			}
            lpNode = lpNode->lpNext;
        }        
    }
	else
		retv = ERROR;

    return retv;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL WINAPI WinRgn_RectInRegion(HRGN hrgn, const RECT * lprc )
{
    _LPRGNDATA lprgn = _GetHRGNPtr( hrgn );
	int retv = FALSE;

    ASSERT( lprgn );

    if( lprgn )
	{
		_LPRECTNODE lpNode;
        lpNode = lprgn->lpNodeFirst;
        ASSERT( lprgn->count >= 0 );
        while( lpNode )
        {
            if( ( MAX(lpNode->rect.left, lprc->left) < MIN(lpNode->rect.right, lprc->right) ) &&
                ( MAX(lpNode->rect.top, lprc->top) < MIN(lpNode->rect.bottom, lprc->bottom) ) )
			{
                retv = TRUE;   // has intersect;
				break;
			}
            lpNode = lpNode->lpNext;
        }
    }
	else
		retv = ERROR;

    return retv;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL WINAPI WinRgn_SetRect(HRGN hrgn, int left, int top, int right, int bottom)
{
    _LPRGNDATA lprgn = _GetHRGNPtr( hrgn );
	int retv = FALSE;
	
    ASSERT( lprgn );

   FORMAT_XY( left, top, right, bottom );
	

    if( lprgn )
	{
		if( left < right && top < bottom )
		{	
			if( lprgn->lpNodeFirst )
			{   // 保留一个
				__FreeNodes( lprgn->lpNodeFirst->lpNext );
			}
			else
			{	// 之前就没有，分配一个
				lprgn->lpNodeFirst = __AllocRectNode();
			}
			if( lprgn->lpNodeFirst == NULL )
				retv = FALSE;
		}
		else
		{	// 新的矩形是无效的矩形， 全部释放
			__FreeNodes( lprgn->lpNodeFirst );
			lprgn->lpNodeFirst = NULL;
		}

		if( lprgn->lpNodeFirst )
		{
			lprgn->rect.left = left;
			lprgn->rect.top = top;
			lprgn->rect.right = right;
			lprgn->rect.bottom = bottom;
			lprgn->count = 1;
			lprgn->lpNodeFirst->rect = lprgn->rect;
			lprgn->lpNodeFirst->lpNext = 0;
			retv = TRUE;//return TRUE;
		}
		else
		{
			lprgn->count = 0;
			_SET_RECT_ZERO( lprgn->rect );
		}
	}

    return retv;//FALSE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

//======================== private proc ============================
int _AndRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpSrc2, BOOL sFlag )
{
    _LPRECTNODE lpNode, lpFirst0, lpFirst1, lpSaveDest = 0;
    RECT rect;
    int count;

	if( lpSrc1->count == 0 ||
		lpSrc2->count == 0 )
	{ // free lpDest
        __FreeNodes( lpDest->lpNodeFirst );
		lpDest->count = 0;
		lpDest->lpNodeFirst = NULL;
		_SET_RECT_ZERO( lpDest->rect );
		return !ERROR;
	}

    lpNode = __AllocRectNode();
    if( lpNode )
    {
        count = 0;
        for( lpFirst0 = lpSrc1->lpNodeFirst; lpFirst0 != 0; lpFirst0 = lpFirst0->lpNext )
        {
            rect = lpFirst0->rect;
            for( lpFirst1 = lpSrc2->lpNodeFirst; lpFirst1 != 0; lpFirst1 = lpFirst1->lpNext )
            {
                // if up speed, I may use inline option to replace IntersectRect
                if( IntersectRect( &(lpNode->rect), &rect, &(lpFirst1->rect) ) )
                {
                    // add to link table
                    INSERT_NODE_BEFORE( lpSaveDest, lpNode );
                    count++;
                    // alloc a new node for next use
                    lpNode = __AllocRectNode();
                    if( lpNode == 0 )
                    {   // failure to alloc mem
                        __FreeNodes( lpSaveDest );
                        return ERROR;
                    }
                }
            }
        }
    }
    else
        return ERROR;
    __FreeNodes( lpDest->lpNodeFirst );
    __FreeMem( lpNode );
    lpDest->lpNodeFirst = lpSaveDest;
    if( sFlag )
        __SortAndUnite( lpDest );
    return !ERROR;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int _CopyRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc)
{
    _LPRECTNODE lpNode, lpNewNode, lpSaveNode = 0;

    ASSERT( lpDest );
    if( lpDest == lpSrc ||
		lpSrc->count == 0 )
        return !ERROR;
    __FreeNodes( lpDest->lpNodeFirst );
    lpDest->lpNodeFirst = 0;
    if( lpSrc == 0 )
        return NULLREGION;
    lpNode = lpSrc->lpNodeFirst;
    while( lpNode )
    {
        if( ( lpNewNode = __AllocRectNode() ) != 0 )
        {
            lpNewNode->rect = lpNode->rect;
            INSERT_NODE_BEFORE( lpSaveNode, lpNewNode );
        }
        else
        {
            __FreeNodes( lpSaveNode );
            return ERROR;
        }
        lpNode = lpNode->lpNext;
    }
    lpDest->lpNodeFirst = lpSaveNode;
    lpDest->rect = lpSrc->rect;
    lpDest->count = lpSrc->count;
    return !ERROR;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int _DiffRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc, _LPRGNDATA lpRemove, BOOL sFlag)
{
    _LPRECTNODE lpFixNode, lpDestCurrent, lpRemoveFirst, lpDestPrev, lpTemp;
    _RGNDATA rgnDest;
    int retv = ERROR;

    ASSERT( lpDest && lpSrc && lpRemove );
    rgnDest.lpNodeFirst = 0;
    if( (retv = _CopyRgn( &rgnDest, lpSrc )) == ERROR )
        return ERROR;
    for( lpRemoveFirst = lpRemove->lpNodeFirst; lpRemoveFirst != 0 ; lpRemoveFirst = lpRemoveFirst->lpNext )
    {
        for( lpDestPrev = lpDestCurrent = rgnDest.lpNodeFirst; lpDestCurrent != 0; )
        {
            // make four rect to hrgnDest
            retv = __FixRects( &lpFixNode, &(lpDestCurrent->rect), &(lpRemoveFirst->rect) );
            if( retv == FIX_SPLIT_RECT || retv == FIX_REMOVE_RECT )
            {// lpDestCurrent->rect has split to some rect, so I delete lpDestCurrent
             // the lpNodeFirst of hrgnDest has changed after call _FixRects
                if( lpDestPrev == lpDestCurrent )
                {   // first
                    ASSERT( lpDestPrev == rgnDest.lpNodeFirst );
                    if( retv == FIX_SPLIT_RECT )
                    {
                        // search end of lpFixNode
                        lpTemp = lpFixNode;
                        while( lpTemp->lpNext )
                            lpTemp = lpTemp->lpNext;
                        // remove lpDestPrev
                        lpTemp->lpNext = lpDestPrev->lpNext;
                        // link rgnDest and lpFixNode
                        rgnDest.lpNodeFirst = lpFixNode;
                        // free lpDest
                        __FreeMem( lpDestCurrent );
                        // set for next loop
                        lpDestCurrent = lpTemp;
                    }
                    else
                    {   // retv is FIX_REMOVE_RECT
                        // only remove lpDestCurrent from rgnDest
                        rgnDest.lpNodeFirst = lpDestCurrent->lpNext;
                        __FreeMem( lpDestCurrent );
                        lpDestPrev = lpDestCurrent = rgnDest.lpNodeFirst;
                        continue;
                    }
                }
                else
                {   // lpDestPrev != lpDestCurrent
                    // remove lpDestCurrent and free it
                    lpDestPrev->lpNext = lpDestCurrent->lpNext;
                    __FreeMem( lpDestCurrent );
                    // set for next loop
                    lpDestCurrent = lpDestPrev;
                    if( retv == FIX_SPLIT_RECT )
                    {   // search end of lpFixNode
                        lpTemp = lpFixNode;
                        while( lpTemp->lpNext )
                            lpTemp = lpTemp->lpNext;
                        // link lpFixNode and rgnDest
                        lpTemp->lpNext = rgnDest.lpNodeFirst;
                        rgnDest.lpNodeFirst = lpFixNode;
                    }
                }
            }
            else if( retv == FIX_NO_MEM )
            {
                __FreeNodes( rgnDest.lpNodeFirst );
                return ERROR;
            }
            lpDestPrev = lpDestCurrent;
            lpDestCurrent = lpDestCurrent->lpNext;
        }
    }
    __FreeNodes( lpDest->lpNodeFirst );
    lpDest->lpNodeFirst = rgnDest.lpNodeFirst;
    if( sFlag )
        __SortAndUnite( lpDest );
    return !ERROR;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

//int _OrRgn(HRGN hrgnDest, HRGN hrgnSrc1, HRGN hrgnSrc2)
int _OrRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpSrc2, BOOL sFlag)
{
    int retv;

    if( (retv = _DiffRgn( lpDest, lpSrc1, lpSrc2, FALSE )) != ERROR )
        retv = __AppendNodes( lpDest, lpSrc2 );
    if( retv != ERROR && sFlag )
        __SortAndUnite( lpDest );
    return retv;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int _XorRgn(_LPRGNDATA lpDest, _LPRGNDATA lpSrc1, _LPRGNDATA lpSrc2, BOOL sFlag)
{
    _RGNDATA rgnOver, rgnDest1, rgnDest2;
    int retv;

    rgnDest1.lpNodeFirst = rgnDest2.lpNodeFirst = rgnOver.lpNodeFirst = 0;
    if( (retv = _AndRgn( &rgnOver, lpSrc1, lpSrc2, FALSE )) != ERROR )
    {
        if( rgnOver.lpNodeFirst == 0 )
        {   // null region
            if( (retv = __AppendNodes( &rgnOver, lpSrc1 )) != ERROR )
            {
                if( (retv = __AppendNodes( &rgnOver, lpSrc2 )) != ERROR )
                {
                    __FreeNodes( lpDest->lpNodeFirst );
                    lpDest->lpNodeFirst = rgnOver.lpNodeFirst;
                }
                else
                    __FreeNodes( rgnOver.lpNodeFirst );
            }
        }
        else
        {    // retv == COMPLEXREGION or retv == SIMPLEREGION
            if( (retv = _DiffRgn( &rgnDest1, lpSrc1, &rgnOver, FALSE )) != ERROR )
            {
                if( (retv = _DiffRgn( &rgnDest2, lpSrc2, &rgnOver, FALSE )) != ERROR )
                {
                    __FreeNodes( lpDest->lpNodeFirst );
                    // link rgnDest1 and rgnDest2
                    lpDest->lpNodeFirst = __LinkNodesAfter( rgnDest1.lpNodeFirst, rgnDest2.lpNodeFirst );
                }
                else
                    __FreeNodes( rgnDest1.lpNodeFirst );
            }
            __FreeNodes( rgnOver.lpNodeFirst );
        }
    }
    if( retv != ERROR && sFlag )
        __SortAndUnite( lpDest );
    return retv;
}

//#ifdef __USE_MEM
typedef struct __MEM_BLOCK
{
//    struct __MEM_BLOCK *lpNext;
    short int NextIndex;
    short int PrevIndex;
    union{
        _RECTNODE node;
        //_RGNDATA rgn;
    }data;
}_MEM_BLOCK;

static int __FreeIndex = 0;
static int __AllocIndex = -1;
static int __Count = 0;
//int rgnCount;
#define BLOCK_SIZE 400
//static _MEM_BLOCK * __lpMemoryBlock[BLOCK_SIZE];
static _MEM_BLOCK * __lpMemoryBlock;

static CRITICAL_SECTION csRgn;

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL _InitialRgn( void )
{
    int i;
//	LPBYTE lpmem; 

    InitializeCriticalSection( &csRgn );
	csRgn.lpcsName = "CS-RGN";

    __FreeIndex = 0;
    __AllocIndex = -1;
    __Count = 0;
    //memset( __lpMemoryBlock, 0, sizeof( __lpMemoryBlock ) );
	__lpMemoryBlock = malloc( sizeof( _MEM_BLOCK ) * BLOCK_SIZE );

	if( __lpMemoryBlock )
	{
		
		for( i = 0; i < (BLOCK_SIZE - 1); i++ )
		{
			__lpMemoryBlock[i].NextIndex = i + 1;
			__lpMemoryBlock[i].PrevIndex = i - 1;
		}
		//    rgnCount = i;
		
		__lpMemoryBlock[i].NextIndex = -1;
		__lpMemoryBlock[i].PrevIndex = i - 1;
		__FreeIndex = 0;
		__AllocIndex = -1;
		return TRUE;
	}
    return FALSE;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

void _DeInitialRgn( void )
{
    DeleteCriticalSection( &csRgn );
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

//#endif
static int __MaxCount=0;
void *__AllocMem( void )
{
//#ifdef __USE_MEM
    short int s;
	int iCount = 0;
	void * p = NULL;

	EnterCriticalSection( &csRgn );
    while( iCount < 20 )
	{
		if( __FreeIndex >= 0 )
		{
			s = __lpMemoryBlock[__FreeIndex].NextIndex;
			__lpMemoryBlock[__FreeIndex].NextIndex = __AllocIndex;
			__lpMemoryBlock[__FreeIndex].PrevIndex = -1;
			if( __AllocIndex >= 0 )
				__lpMemoryBlock[__AllocIndex].PrevIndex = __FreeIndex;
			__AllocIndex = __FreeIndex;
			__FreeIndex = s;
			__lpMemoryBlock[s].PrevIndex = -1;
			__Count++;
			if( __MaxCount < __Count )
				__MaxCount = __Count;
			p = &(__lpMemoryBlock[__AllocIndex].data);
			break;  // ok , i get it
		}
		else
		{
		    LeaveCriticalSection( &csRgn );

			Sleep(100);

			EnterCriticalSection( &csRgn );
			iCount++;
			RETAILMSG( 1, (TEXT("no enough rgn\r\n") ) );
		}
	}
	LeaveCriticalSection( &csRgn );
	return p;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

void __FreeMem( void * lpBlock )
{
//#ifdef __USE_MEM
    _MEM_BLOCK *p = (_MEM_BLOCK*)lpBlock;
    short index = p - __lpMemoryBlock;

    EnterCriticalSection( &csRgn );    

	ASSERT( index >= 0 && index < BLOCK_SIZE );
    if( __lpMemoryBlock[index].PrevIndex >= 0 )
        __lpMemoryBlock[__lpMemoryBlock[index].PrevIndex].NextIndex = __lpMemoryBlock[index].NextIndex;
    if( __lpMemoryBlock[index].NextIndex >= 0 )
        __lpMemoryBlock[__lpMemoryBlock[index].NextIndex].PrevIndex = __lpMemoryBlock[index].PrevIndex;
    if( index == __AllocIndex )
        __AllocIndex = __lpMemoryBlock[index].NextIndex;

    if( __FreeIndex >= 0 )
    {
        __lpMemoryBlock[__FreeIndex].PrevIndex = index;
        __lpMemoryBlock[index].NextIndex = __FreeIndex;
        __lpMemoryBlock[index].PrevIndex = -1;
    }
    else
    {
        __lpMemoryBlock[index].NextIndex = -1;
        __lpMemoryBlock[index].PrevIndex = -1;
    }
    __FreeIndex = index;
    __Count--;

    LeaveCriticalSection( &csRgn );
//#else
//    free( lpBlock );
//#endif
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int __AppendNodes( _LPRGNDATA lpData, _LPRGNDATA lpSrc )
{
    _LPRECTNODE lpNode = lpSrc->lpNodeFirst;
    _LPRECTNODE lpNew, lpFirst = 0;

    while( lpNode )
    {
        if( (lpNew = __AllocRectNode()) != 0 )
        {
            lpNew->rect = lpNode->rect;
            lpNew->lpNext = lpFirst;
            lpFirst = lpNew;

            lpNode = lpNode->lpNext;
        }
        else
        {
            __FreeNodes( lpFirst );
            return ERROR;
        }
    }
    if( lpFirst )
        lpData->lpNodeFirst=__LinkNodesAfter( lpFirst, lpData->lpNodeFirst );
    return !ERROR;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int __CalcRgnBox( _LPRECTNODE lpNodeFirst, LPRECT lpRect )
{
    _SET_LPRECT_ZERO( lpRect );
    for( ; lpNodeFirst != 0; lpNodeFirst = lpNodeFirst->lpNext )
        UnionRect( lpRect, lpRect, &(lpNodeFirst->rect) );
    return (!ERROR);
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int __FixRects( _LPRECTNODE * lppNode, LPRECT lpDest, LPRECT lpRemove )
{
    _LPRECTNODE lpNodeFirst = 0;
    RECT rect;
    int nCount;
    _LPRECTNODE lpNew;

    *lppNode = 0;
    if( IntersectRect( &rect, lpDest, lpRemove ) )
    {   // check four rects is valid ? if yes, insert to dest
        nCount = 0;
        if( lpDest->top != rect.top )
        {
            if( (lpNew = __AllocRectNode()) != 0 )
            {
                SET_LPRECTNODE_VALUE( lpNew, lpDest->left, lpDest->top, lpDest->right, rect.top );
                lpNew->lpNext = lpNodeFirst;
                lpNodeFirst = lpNew;
                nCount++;
            }
            else
                goto _TESTMEM;
        }
        if( lpDest->bottom != rect.bottom )
        {
            if( (lpNew = __AllocRectNode()) != 0 )
            {
                SET_LPRECTNODE_VALUE( lpNew, lpDest->left, rect.bottom, lpDest->right, lpDest->bottom );
                lpNew->lpNext = lpNodeFirst;
                lpNodeFirst = lpNew;
                nCount++;
            }
            else
                goto _TESTMEM;
        }
        if( lpDest->left != rect.left )
        {
            if( (lpNew = __AllocRectNode()) != 0 )
            {
                SET_LPRECTNODE_VALUE( lpNew, lpDest->left, rect.top, rect.left, rect.bottom );
                lpNew->lpNext = lpNodeFirst;
                lpNodeFirst = lpNew;
                nCount++;
            }
            else
                goto _TESTMEM;
        }
        if( lpDest->right != rect.right )
        {
            if( (lpNew = __AllocRectNode()) != 0 )
            {
                SET_LPRECTNODE_VALUE( lpNew, rect.right, rect.top, lpDest->right, rect.bottom );
                lpNew->lpNext = lpNodeFirst;
                lpNodeFirst = lpNew;
                nCount++;
            }
            else
                goto _TESTMEM;
        }
        *lppNode = lpNodeFirst;
        if( nCount )
            return FIX_SPLIT_RECT;
        else
            return FIX_REMOVE_RECT;
    }
    return FIX_NO_INTERSECT;
_TESTMEM:
    __FreeNodes( lpNodeFirst );
    return FIX_NO_MEM;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int __FreeNodes( _LPRECTNODE lpNodes )
{
     _LPRECTNODE lpTemp;

     while( lpNodes )
     {
         lpTemp = lpNodes->lpNext;
         __FreeMem( lpNodes );
         lpNodes = lpTemp;
     }
     return 1;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

_LPRECTNODE __LinkNodesAfter( _LPRECTNODE lpDest, _LPRECTNODE lpSrc )
{
    _LPRECTNODE p;
    if( lpDest )
    {
        p = lpDest;
        while( p->lpNext )
            p = p->lpNext;
        p->lpNext = lpSrc;
        return lpDest;
    }
    else if( lpSrc )
        return lpSrc;
    else
        return 0;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

int __SortAndUnite( _LPRGNDATA lpData )
{
    _LPRECTNODE *lppNode0PrevAdr, lpNode0;
    _LPRECTNODE lpNode1Prev, lpNode1;
    int cmpt, cmpl, flag, fUpdate;

// do unite
    SET_RECT_VALUE( lpData->rect, 0, 0, 0, 0 );
    lpNode0 = lpData->lpNodeFirst;
    if( lpNode0 == 0 )
    {
        lpData->count = 0;
        return 0;
    }
    else if( lpNode0->lpNext == 0 )
    {
        lpData->count = 1;
        lpData->rect = lpNode0->rect;
        return 1;
    }
	do{
		fUpdate = 0;
		lpNode0 = lpData->lpNodeFirst;		
		do{
			lpNode1 = lpNode0->lpNext;
			lpNode1Prev = lpNode0;
			while( lpNode1 )
			{   // compare rect
				cmpt = lpNode0->rect.top - lpNode1->rect.top;
				cmpl = lpNode0->rect.left - lpNode1->rect.left;
				flag = 0;
				if( cmpt == 0 )
				{
					if( lpNode0->rect.bottom == lpNode1->rect.bottom )
					{
						if( lpNode0->rect.left == lpNode1->rect.right )
						{   // union two rects                        //___________
							lpNode0->rect.left = lpNode1->rect.left;  //|////|    |
							flag =1;                                  //|////|    |
						}                                             //+---------+
						else if( lpNode0->rect.right == lpNode1->rect.left )
						{   // union two rects                         //___________
							lpNode0->rect.right = lpNode1->rect.right; //|    |////|
							flag =1;                                   //|    |////|
						}                                              //+----|----+
					}
				}
				else if( cmpl == 0 )
				{
					if( lpNode0->rect.right == lpNode1->rect.right )
					{
						if( lpNode0->rect.top == lpNode1->rect.bottom )
						{   // union two rects
							lpNode0->rect.top = lpNode1->rect.top;
							flag = 1;
						}
						else if( lpNode0->rect.bottom == lpNode1->rect.top )
						{   // union two rects
							lpNode0->rect.bottom = lpNode1->rect.bottom;
							flag = 1;
						}
					}
				}
				if( flag )
				{   // remove lpNode1 from table
					fUpdate = 1;
					lpNode1Prev->lpNext = lpNode1->lpNext;
					__FreeMem( lpNode1 ); // free lpNode1
					lpNode1 = lpNode1Prev->lpNext;
					continue;
				}
				else
				{
					lpNode1Prev = lpNode1;
					lpNode1 = lpNode1->lpNext;
				}
			}
			lpNode0 = lpNode0->lpNext;
		}while( lpNode0 );

	}while( fUpdate );
// do sort
    lpData->rect = lpData->lpNodeFirst->rect;
    lpData->count = 1;

    lpNode1Prev = lpData->lpNodeFirst;
    lpNode1 = lpNode1Prev->lpNext;
    while( lpNode1 )
    {
        lpData->count++;
        UNION_RECT( lpData->rect, lpNode1->rect );

        lpNode0 = lpData->lpNodeFirst;
        lppNode0PrevAdr = &lpData->lpNodeFirst;
        flag = 0;
        for( ; lpNode0 != lpNode1; lppNode0PrevAdr = &lpNode0->lpNext, lpNode0 = lpNode0->lpNext )
        {   //compare rect
            cmpt = lpNode0->rect.top - lpNode1->rect.top;
            cmpl = lpNode0->rect.left - lpNode1->rect.left;
            if( cmpt > 0 || ( cmpt == 0 && cmpl > 0 ) )
            {   // now, exchange rect
                lpNode1Prev->lpNext = lpNode1->lpNext; // remove lpNode1 from table
                lpNode1->lpNext = lpNode0; // insert lpNode1 to table
                *lppNode0PrevAdr = lpNode1;
                lpNode1 = lpNode1Prev->lpNext;
                flag = 1;
                break;
            }
        }
        if( flag == 0 )
        {
            lpNode1Prev = lpNode1;
            lpNode1 = lpNode1->lpNext;
        }
    }
    return 0;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

_LPRGNDATA _GetHRGNPtr( HRGN hrgn )
{
	_LPRGNDATA lprgn = (_LPRGNDATA)HANDLE_TO_PTR( hrgn );

	if( lprgn && (WORD)GET_OBJ_TYPE(lprgn) == OBJ_REGION )
		return lprgn;
	WARNMSG( 1, ("error: Invalid hrgn handle=0x%x\r\n", hrgn) );
	SetLastError( ERROR_INVALID_HANDLE );
	return NULL;
}
