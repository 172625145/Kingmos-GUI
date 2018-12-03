/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __LARGENUM_H
#define __LARGENUM_H

#ifdef __cplusplus
extern "C" {
#endif

#define SIZE_OF_LARGENUM    3   // Number of UINT in a LARGE_NUM

typedef struct {
    BOOL    fNegative;
    union {
        struct {
            USHORT  s[2*SIZE_OF_LARGENUM];
        }   s16;	//16位
        struct {
            UINT  u[SIZE_OF_LARGENUM];
        }   s32;	//32位
    }   u;
} LARGENUM, *PLARGENUM;

// 功能申明

PLARGENUM
LargeNumSet(
    PLARGENUM   pNum,
    int       n
    );

BOOL
IsLargeNumNotZero(
    PLARGENUM   pNum
    );

BOOL
IsLargeNumNegative(
    PLARGENUM   pNum
    );

BOOL
IsLargeNumMagGreaterThan(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2
    );

BOOL
IsLargeNumMagLessThan(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2
    );

PLARGENUM
LargeNumMagInc(
    PLARGENUM   pNum
    );

char *
LargeNumToAscii(
    PLARGENUM   pNum
    );


PLARGENUM
LargeNumMagAdd(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    );

PLARGENUM
LargeNumMagSub(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    );

PLARGENUM
LargeNumAdd(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    );

PLARGENUM
LargeNumSub(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    );

PLARGENUM
LargeNumMulUINT(
    UINT      a,
    UINT      b,
    PLARGENUM   pResult
    );

PLARGENUM
LargeNumMulint(
    int       a,
    int       b,
    PLARGENUM   pResult
    );

PLARGENUM
LargeNumMult(
    PLARGENUM   pNum1,
    PLARGENUM   pNum2,
    PLARGENUM   pResult
    );

VOID
LargeNumRAShift(
    PLARGENUM   pNum,
    int       count
    );

UINT
LargeNumDivint32(
    PLARGENUM   pNum,
    int       divisor,
    PLARGENUM   pResult
    ); 

int
LargeNumBits(
    PLARGENUM   pNum
    );

#ifdef __cplusplus
}
#endif


#endif  // __LARGENUM_H




