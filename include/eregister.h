/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef __H_REGISTER_2002_3_12_
#define __H_REGISTER_2002_3_12_

#define RGST_CONFIGURE				0x00010000L
//#define RGST_APPLICATION			0x00020000L

#define RGST_MENU					0x00000001L
#define RGST_DESKTOP				0x00000002L

#define RGST_AP_SYSTEM				0x00000004L
#define RGST_AP_ASSISTANT			0x00000008L
#define RGST_AP_NETWORK				0x00000010L
#define RGST_AP_GAME				0x00000020L
#define RGST_AP_TOOLS				0x00000040L

#define RGST_SYSTEM_SYSTEM			0x00000080L
#define RGST_SYSTEM_PERSONAL		0x00000100L
#define RGST_SYSTEM_COMMUNICATION	0x00000200L

#define RGST_INTERPRET				0x00000400L

#define MAX_APNAME					40
#define MAX_POSTFIX					20
#define REGFIELD					5

typedef struct tagRegistApplictionStruct
{
	DWORD		dwType ;
	TCHAR		lpszInternal[MAX_APNAME] ;
	TCHAR		lpszExternal[MAX_APNAME] ;
	TCHAR		lpszPostfix [MAX_POSTFIX] ;
	void		*lpReserve ;
} REGAPDATA, *LPREGAPDATA ;

#define	RGST_DESKTOP_TOPIC			0x00100000L
#define RGST_DESKTOP_STYLE			0x00200000L

typedef struct tagRegistConfigStruct
{
	DWORD		dwType ;
	TCHAR		*lpszData ;
}REGCONFIG, *LPREGCONFIG ;

BOOL ReadRegistConfig( LPREGCONFIG lprc ) ;
BOOL WriteRegistConfig( LPREGCONFIG lprc ) ;

BOOL GetRegistApplication( LPPTRLIST lplist, DWORD dwType, BOOL bType );
BOOL RegistApplication( const LPREGAPDATA  lprad, BOOL bModify ) ;
BOOL UnRegistApplication( LPREGAPDATA lprad ) ;
BOOL LoadApplicationImages( HIMAGELIST  hIcons, LPPTRLIST lpaps, BOOL bLarge );


void LoadRegsterApplication (LPREGAPDATA lpra);

#endif //__H_REGISTER_2002_3_12_




