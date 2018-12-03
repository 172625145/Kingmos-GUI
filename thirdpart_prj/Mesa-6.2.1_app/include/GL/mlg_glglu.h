// 定义GLU库中没有定义的结构或者其他的定义

#ifndef __MLG_GLGLU
#define __MLG_GLGLU

#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */

#include "mlg_glstruct.h"

//#define WINGDIAPI	//__declspec(dllimport)
//typedef	struct		tagPIXELFORMATDESCRIPTOR	PIXELFORMATDESCRIPTOR, *LPPIXELFORMATDESCRIPTOR;

struct timeb {
        long time;
        unsigned short millitm;
        short timezone;
        short dstflag;
};

//timeval结构
struct	timeval
{
	long	tv_sec;
	long	tv_usec;
};

/* Standard Cursor IDs */
#define IDC_WAIT            MAKEINTRESOURCE(32514)
#define IDC_CROSS           MAKEINTRESOURCE(32515)
#define IDC_UPARROW         MAKEINTRESOURCE(32516)
#define IDC_SIZENWSE        MAKEINTRESOURCE(32642)
#define IDC_SIZENESW        MAKEINTRESOURCE(32643)
#define IDC_SIZEWE          MAKEINTRESOURCE(32644)
#define IDC_SIZENS          MAKEINTRESOURCE(32645)
#define IDC_SIZEALL         MAKEINTRESOURCE(32646)
#define IDC_NO              MAKEINTRESOURCE(32648)
#define IDC_HELP            MAKEINTRESOURCE(32651)

#define assert	ASSERT		// 定义调试

/* SetWindowPos Flags */
#define SWP_NOOWNERZORDER   0x0200  /* Don't do owner Z ordering */
#define HWND_NOTOPMOST		((HWND)-2)
#define SW_MINIMIZE         6

/* Flags for ChangeDisplaySettings */
#define CDS_FULLSCREEN      0x00000004

/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL      0

/* Class styles */
#define CS_OWNDC					0x0020

/* Standard Icon IDs */
#define IDI_WINLOGO					32517

/*
 * Flags for TrackPopupMenu
 */
#define TPM_LEFTBUTTON  0x0000L
#define TPM_RIGHTBUTTON 0x0002L
#define MF_POPUP            0x00000010L

#define PC_NOCOLLAPSE   0x04    /* do not match color to system palette */


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif