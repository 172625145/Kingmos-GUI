/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __FIXCALC_H
#define __FIXCALC_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

typedef long FIX16DOT16;
//
#define Fix16DOT16_IntToFix( iv ) ( (FIX16DOT16)( (iv) << 16 ) )
//
#define Fix16DOT16_FixToInt( fixV ) ( (fixV) >> 16 )
// fixV + fixAddV
#define Fix16DOT16_Add( fixV, fixAddV ) ( (FIX16DOT16)( (fixV0) + (fixV1) ) )
// fixV - fixSubV
#define Fix16DOT16_GetInteger( fixV ) ( (fixV) >> 16 )
#define Fix16DOT16_Sub( fixV, fixSubV ) ( (FIX16DOT16)( (fixV0) - (fixV1) ) )

FIX16DOT16 Fix16DOT16_StringToFix( char * lpszFix );
// fixV * fixMulV
FIX16DOT16 Fix16DOT16_Mul( FIX16DOT16 fixV, FIX16DOT16 fixMulV );
// fixV / fixDivV
FIX16DOT16 Fix16DOT16_Div( FIX16DOT16 fixV, FIX16DOT16 fixDivV );
// fixV * fixMulV / fixDivV
FIX16DOT16 Fix16DOT16_MulDiv( FIX16DOT16 fixV, FIX16DOT16 fixMulV, FIX16DOT16 fixDivV );
// sqrt( x )
FIX16DOT16 Fix16DOT16_Sqrt( FIX16DOT16  x );

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //  __FIXCALC_H
