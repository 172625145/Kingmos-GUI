/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __EPROGRAM_H
#define __EPROGRAM_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define RegisterApplication Application_Register
BOOL WINAPI RegisterApplication( LPCTSTR lpcszName, WINMAINPROC lpApProc, HICON hIcon );

//fIconType: 
#define SMALL_ICON 1
#define LARGE_ICON 2
#define GetApplicationIcon Application_GetIcon
HICON WINAPI Application_GetIcon( LPCTSTR lpcszName, UINT fIconType );

#define LoadApplication Application_Load
BOOL WINAPI Application_Load( LPCTSTR lpcszName, LPCTSTR lpcszCmdLine );


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EPROGRAM_H
