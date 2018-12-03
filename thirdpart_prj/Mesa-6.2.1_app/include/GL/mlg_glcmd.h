// 定义GLU库中没有定义的结构或者其他的定义

#ifndef __MLG_GLCMD_H
#define __MLG_GLCMD_H

#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */

//typedef unsigned short      WORD;

/* general constants */
#define MAXPNAMELEN      32     /* max product name length (including NULL) */
#define MAX_JOYSTICKOEMVXDNAME 260 /* max oem vxd name length (including NULL) */

#define JOY_RETURNALL 0x000000ffl

#define JOYSTICKID1 0

#define JOYERR_NOERROR 0

#define SM_CMOUSEBUTTONS        43

typedef struct joyinfoex_tag { 
    DWORD dwSize; 
    DWORD dwFlags; 
    DWORD dwXpos; 
    DWORD dwYpos; 
    DWORD dwZpos; 
    DWORD dwRpos; 
    DWORD dwUpos; 
    DWORD dwVpos; 
    DWORD dwButtons; 
    DWORD dwButtonNumber; 
    DWORD dwPOV; 
    DWORD dwReserved1; 
    DWORD dwReserved2; 
} JOYINFOEX; 

typedef struct { 
    WORD wMid; 
    WORD wPid; 
    CHAR szPname[MAXPNAMELEN]; 
    UINT wXmin; 
    UINT wXmax; 
    UINT wYmin; 
    UINT wYmax; 
    UINT wZmin; 
    UINT wZmax; 
    UINT wNumButtons; 
    UINT wPeriodMin; 
    UINT wPeriodMax; 
    UINT wRmin; 
    UINT wRmax; 
    UINT wUmin; 
    UINT wUmax; 
    UINT wVmin; 
    UINT wVmax; 
    UINT wCaps; 
    UINT wMaxAxes; 
    UINT wNumAxes; 
    UINT wMaxButtons; 
    CHAR szRegKey[MAXPNAMELEN]; 
    CHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME]; 
} JOYCAPS; 



#ifdef __cplusplus
}
#endif  // __cplusplus

#endif