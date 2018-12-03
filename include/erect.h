/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __ERECT_H
#define __ERECT_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus


typedef struct _RECT
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;
typedef RECT FAR*  LPRECT;
typedef const RECT FAR*  LPCRECT;

typedef struct _POINT
{
    LONG x;
    LONG y;
} POINT;

typedef POINT FAR*  LPPOINT;

typedef struct _POINTS
{
    SHORT x;
    SHORT y;
} POINTS;

#define MAKEPOINTS(l)       (*((POINTS FAR *)&(l)))

typedef struct _SIZE
{
    LONG cx;
    LONG cy;
} SIZE;
typedef SIZE*       PSIZE;
typedef SIZE FAR*  LPSIZE;
typedef const SIZE FAR* LPCSIZE;

#define SetRect Rect_Set
BOOL WINAPI Rect_Set(LPRECT, int, int, int, int);

#define SetRectEmpty Rect_SetEmpty
BOOL WINAPI Rect_SetEmpty(LPRECT);

#define CopyRect Rect_Copy
BOOL WINAPI Rect_Copy(LPRECT, LPCRECT);

#define IsRectEmpty Rect_IsEmpty
BOOL WINAPI Rect_IsEmpty(LPCRECT);

#define EqualRect Rect_IsEqual
BOOL WINAPI Rect_IsEqual(LPCRECT, LPCRECT);

#define IntersectRect Rect_Intersect
BOOL WINAPI Rect_Intersect(LPRECT, LPCRECT, LPCRECT);

#define UnionRect Rect_Union
BOOL WINAPI Rect_Union(LPRECT, LPCRECT, LPCRECT);

#define SubtractRect Rect_Subtract
BOOL WINAPI Rect_Subtract(LPRECT, LPCRECT, LPCRECT);

#define OffsetRect Rect_Offset
BOOL WINAPI Rect_Offset(LPRECT, int, int);

#define InflateRect Rect_Inflate
BOOL WINAPI Rect_Inflate(LPRECT, int, int);

#define PtInRect Rect_PtInRect
BOOL WINAPI Rect_PtInRect(LPCRECT, POINT);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__ERECT_H
