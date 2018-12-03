#include <edef.h>
#include <erect.h>
#include <eassert.h>

#define _IS_RECT_EMPTY( lprc ) ((lprc)->right <= (lprc)->left ||\
                                (lprc)->bottom <= (lprc)->top)

#define _IS_RECT_EQUAL( lprc1, lprc2 )  (lprc1->left == lprc2->left &&\
                                         lprc1->top == lprc2->top &&\
                                         lprc1->right == lprc2->right &&\
                                         lprc1->bottom == lprc2->bottom)

#define _SET_RECT_ZERO( lprc ) (lprc->left = lprc->top = lprc->right = lprc->bottom = 0)


BOOL WINAPI Rect_Set(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom )
{
    ASSERT( lprc != 0 );
    lprc->left = xLeft; lprc->top = yTop;
    lprc->right = xRight; lprc->bottom = yBottom;
    return TRUE;
}

BOOL WINAPI Rect_SetEmpty(LPRECT lprc)
{
    ASSERT( lprc != 0 );
    _SET_RECT_ZERO( lprc );
    return TRUE;
}

BOOL WINAPI Rect_Copy(LPRECT lprcDest, LPCRECT lprcSrc)
{
    ASSERT( lprcDest && lprcSrc );
    *lprcDest = *lprcSrc;
    return TRUE;
}

BOOL WINAPI Rect_IsEmpty(LPCRECT lprc)
{
    ASSERT( lprc );
    return ( _IS_RECT_EMPTY( lprc ) );
}

BOOL WINAPI Rect_IsEqual(LPCRECT lprc1, LPCRECT lprc2)
{
    ASSERT( lprc1 && lprc2 );
    return( lprc1->left == lprc2->left &&
        lprc1->top == lprc2->top &&
        lprc1->right == lprc2->right &&
        lprc1->bottom == lprc2->bottom );
}

BOOL WINAPI Rect_Intersect(LPRECT lprcDest, LPCRECT lprcSrc1, LPCRECT lprcSrc2)
{
    ASSERT( lprcDest && lprcSrc1 && lprcSrc2 );
    lprcDest->left = MAX( lprcSrc1->left, lprcSrc2->left );
    lprcDest->top  = MAX( lprcSrc1->top, lprcSrc2->top );
    lprcDest->right = MIN( lprcSrc1->right, lprcSrc2->right );
    lprcDest->bottom = MIN( lprcSrc1->bottom, lprcSrc2->bottom );
    if( _IS_RECT_EMPTY( lprcDest ) )
    {
//        _SET_RECT_ZERO( lprcDest );
        return FALSE;
    }
    else
        return TRUE;
}

BOOL WINAPI Rect_Union(LPRECT lprcDest, LPCRECT lprcSrc1, LPCRECT lprcSrc2)
{
    ASSERT( lprcDest && lprcSrc1 && lprcSrc2 );
    lprcDest->left = MIN( lprcSrc1->left, lprcSrc2->left );
    lprcDest->top  = MIN( lprcSrc1->top, lprcSrc2->top );
    lprcDest->right = MAX( lprcSrc1->right, lprcSrc2->right );
    lprcDest->bottom = MAX( lprcSrc1->bottom, lprcSrc2->bottom );
    if( _IS_RECT_EMPTY( lprcDest ) )
    {
//        _SET_RECT_ZERO( lprcDest );
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI Rect_Subtract(LPRECT lprcDest, LPCRECT lprcSrc1, LPCRECT lprcSrc2)
{
    RECT r;

    ASSERT( lprcDest && lprcSrc1 && lprcSrc2 );

    _SET_RECT_ZERO( lprcDest );

    if( IntersectRect( &r, lprcSrc1, lprcSrc2 ) )
    {
        if( r.left == lprcSrc1->left && r.top == lprcSrc1->top )
        {
            if( r.bottom == lprcSrc1->bottom )         //___________
            {                                         //|\\\\|     |
                lprcDest->left = r.right;             //|\\\\| src |
                lprcDest->top = r.top;                //|\\\\|     |
                lprcDest->right = lprcSrc1->right;    //|\\\\|     |
                lprcDest->bottom = r.bottom;          //-----------
            }
            else if( r.right == lprcSrc1->right )     //___________
            {                                         //|\\\\\\\\\ | 
                lprcDest->left = r.left;              //|\\\\\\\\\ |
                lprcDest->top = r.bottom;             //|--------- |
                lprcDest->right = r.right;            //|    src   |
                lprcDest->bottom = lprcSrc1->bottom;  //-----------
            }
        }
        else if( r.right == lprcSrc1->right && r.bottom == lprcSrc1->bottom )
        {
            if( r.left == lprcSrc1->left )            //___________
            {                                         //|         |
                lprcDest->left = r.left;              //|---------|
                lprcDest->top =  lprcSrc1->top;       //|\\\\\\\\\|
                lprcDest->right = r.right;            //|\\\\\\\\\|
                lprcDest->bottom = r.top;             //-----------
            }
            else if( r.top == lprcSrc1->top )         //___________
            {                                         //|    |\\\\|
                lprcDest->left = lprcSrc1->left;      //|    |\\\\|
                lprcDest->top =  r.top;               //|    |\\\\|
                lprcDest->right = r.left;             //|    |\\\\|
                lprcDest->bottom = r.top;             //-----------
            }
        }
    }

    if( _IS_RECT_EMPTY( lprcDest ) )
    {
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI Rect_Offset(LPRECT lprc, int dx, int dy)
{
    ASSERT( lprc );
    lprc->left += dx;
    lprc->right += dx;
    lprc->top += dy;
    lprc->bottom += dy;
    return TRUE;
}

BOOL WINAPI Rect_Inflate(LPRECT lprc, int dx, int dy)
{
    ASSERT( lprc );
    lprc->left -= dx;
    lprc->right += dx;
    lprc->top -= dy;
    lprc->bottom += dy;
    return TRUE;
}

BOOL WINAPI Rect_PtInRect(LPCRECT lprc, POINT pt)
{
    ASSERT( lprc );
    return ( pt.x >= lprc->left && pt.x < lprc->right &&
        pt.y >= lprc->top && pt.y < lprc->bottom );
}
