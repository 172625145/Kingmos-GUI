/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef __EPROGRESS_H
#define __EPROGRESS_H

//#define classPROGRESS         "MGLCTLS_PROGRESS"

extern const char classPROGRESS[];

#define PBS_SMOOTH              0x01
#define PBS_VERTICAL            0x04


#define PBM_SETRANGE            (WM_USER+1)
#define PBM_SETPOS              (WM_USER+2)
#define PBM_DELTAPOS            (WM_USER+3)
#define PBM_SETSTEP             (WM_USER+4)
#define PBM_STEPIT              (WM_USER+5)

#define PBM_SETRANGE32          (WM_USER+6)  
#define PBM_GETRANGE            (WM_USER+7)  
#define PBM_GETPOS              (WM_USER+8)
#define PBM_SETBARCOLOR         (WM_USER+9)	  
#define PBM_SETBKCOLOR          (WM_USER+10)  

typedef struct
{
   int iLow;
   int iHigh;
} PBRANGE, *PPBRANGE;


#endif  // __EPROGRESS_H
