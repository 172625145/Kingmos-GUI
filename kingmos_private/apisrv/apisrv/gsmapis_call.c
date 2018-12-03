/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：gsm api 定义 
版本号：1.0.0
开发时期：2003
作者：whl
修改记录：
	2004-02-24, 李林修改，系统调用改变为新的方法
******************************************************/

#include "ewindows.h"
#include "gsmapi.h"
#include "eapisrv.h"


// ************************************************************************************
enum{
	APIENTERY_GsmSendMsg=1,
	APIENTERY_GsmGetMsg,
	APIENTERY_GsmDeleteMsg,
	APIENTERY_GsmGetMsgNum,
	APIENTERY_GsmWriteChar,
	APIENTERY_GsmAnswerCall,
	APIENTERY_GsmHangUp,
	APIENTERY_GsmWritePhoneEntry,
	APIENTERY_GsmDialUp,
	APIENTERY_GsmGetSimPhoneNum,
	APIENTERY_GsmDeletePhoneEntry,
	APIENTERY_GsmGetPhoneEntry,
	APIENTERY_GsmGetSignalVal,
	APIENTERY_GsmDregisterNetwork,
	APIENTERY_GsmRegisterNetwork,
	APIENTERY_GsmGetOperator,
	APIENTERY_GsmModemMute,
	APIENTERY_GsmGetVolume,
	APIENTERY_GsmSetVolume,
	APIENTERY_GsmRegisterUser,
};
// **************************************************************************************
// User calling API starts
// *************************************************************************************
typedef BOOL  ( WINAPI * GSMAPI_NoParam )( );
typedef BOOL  ( WINAPI * GSMAPI_OneParam )( DWORD iIndex );
// *************************************************************************************
typedef BOOL  ( WINAPI * GSMAPI_SendMsg )( char *, char*);
BOOL WINAPI GsmApiSendMsg( char *csPhone, char * csMsg  )
{
//	return (( GSMAPI_SendMsg )CALL_API( API_GSM, APIENTERY_GsmSendMsg, 2 ) )( csPhone, csMsg);	
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmSendMsg, 2 );
    cs.arg0 = csPhone;
    return (BOOL)CALL_SERVER( &cs,csMsg );
#else
	return FALSE;
#endif
}

typedef BOOL  ( WINAPI * GSMAPI_GetMsg )( MSG_INFO* pMsg);
BOOL WINAPI GsmApiGetMsg( MSG_INFO* pMsg )
{
//	return (( GSMAPI_GetMsg )CALL_API( API_GSM, APIENTERY_GsmGetMsg, 1 ) )( pMsg );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetMsg, 1 );
    cs.arg0 = pMsg;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiDeleteMsg( int iIndex )
{
//	return ( ( GSMAPI_OneParam )CALL_API( API_GSM, APIENTERY_GsmDeleteMsg, 1 ) )( iIndex );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmDeleteMsg, 1 );
    cs.arg0 = iIndex;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiGetMsgNum( void ) 
{
//	return ( ( GSMAPI_NoParam )CALL_API( API_GSM, APIENTERY_GsmGetMsgNum, 0 ) )(  );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetMsgNum, 0 );
    cs.arg0 = 0;
    return (HMENU)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiWriteChar( char cCh )
{
//	return  (( GSMAPI_OneParam )CALL_API( API_GSM, APIENTERY_GsmWriteChar, 1 ) )( cCh );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmWriteChar, 1 );
    cs.arg0 = cCh;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
}

BOOL WINAPI GsmApiAnswerCall( void )  
{
//	return ( ( GSMAPI_NoParam )CALL_API( API_GSM, APIENTERY_GsmAnswerCall, 0 ) )(  );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmAnswerCall, 0 );
    cs.arg0 = 0;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}
BOOL WINAPI GsmApiHangUp( void )
{
//	return ( ( GSMAPI_NoParam )CALL_API( API_GSM, APIENTERY_GsmHangUp, 0 ) )(  );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmHangUp, 0 );
    cs.arg0 = 0;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

typedef BOOL  ( WINAPI * GSMAPI_WritePhoneEntry)( int iIndex, char * csPhone, char * csText );

int  WINAPI GsmApiWritePhoneEntry( int iIndex, char * csPhone, char * csText ) 
{
//	return ( ( GSMAPI_WritePhoneEntry )CALL_API( API_GSM, APIENTERY_GsmWritePhoneEntry, 3 ) )( iIndex, csPhone, csText );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmWritePhoneEntry, 3 );
    cs.arg0 = iIndex;
    return (int)CALL_SERVER( &cs,csPhone,csText );
#else
	return FALSE;
#endif
	
}

typedef BOOL  ( WINAPI * GSMAPI_Dialup)( char * csPhone);
BOOL WINAPI GsmApiDialUp( char * csPhone ) 
{
//	return ( ( GSMAPI_Dialup )CALL_API( API_GSM, APIENTERY_GsmDialUp, 1 )) (csPhone);
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmDialUp, 1 );
    cs.arg0 = csPhone;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}
int WINAPI GsmApiGetSimPhoneNum ( void ) 
{
//	return ( ( GSMAPI_NoParam)CALL_API( API_GSM, APIENTERY_GsmGetSimPhoneNum, 0 ) )( );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetSimPhoneNum, 0 );
    cs.arg0 = 0;
    return (int)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}
int WINAPI GsmApiDeletePhoneEntry( int iIndex )
{
//	return ( ( GSMAPI_OneParam)CALL_API( API_GSM, APIENTERY_GsmDeletePhoneEntry, 1 ) )( iIndex );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmDeletePhoneEntry, 1 );
    cs.arg0 = iIndex;
    return (int)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

typedef BOOL  ( WINAPI * GSMAPI_GetPhoneEntry)( int iIndex, char * csPhone, char * csText , int * pType);
int WINAPI GsmApiGetPhoneEntry( int iIndex, char * csPhone, char * csText, int * pType )
{
//	return((GSMAPI_GetPhoneEntry)CALL_API( API_GSM, APIENTERY_GsmGetPhoneEntry, 4 ) )( iIndex, csPhone, csText , pType);
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetPhoneEntry,4 );
    cs.arg0 = iIndex;
    return (int)CALL_SERVER( &cs,csPhone,csText,pType );
#else
	return FALSE;
#endif
	
}
int WINAPI GsmApiGetSignalVal( void ) 
{
//	return ( ( GSMAPI_NoParam)CALL_API( API_GSM, APIENTERY_GsmGetSignalVal, 0 )) ( );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetSignalVal, 0 );
    cs.arg0 = 0;
    return (int)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiDregisterNetwork( void )
{
//	return  (( GSMAPI_NoParam)CALL_API( API_GSM, APIENTERY_GsmDregisterNetwork, 0 ) )( );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmDregisterNetwork, 0 );
    cs.arg0 = 0;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiRegisterNetwork( void )
{
//	return  (( GSMAPI_NoParam)CALL_API( API_GSM, APIENTERY_GsmRegisterNetwork, 0 ) )( );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmRegisterNetwork, 0 );
    cs.arg0 = 0;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

typedef BOOL  ( WINAPI * GSMAPI_GetOperator)( char * csOps);
BOOL WINAPI GsmApiGetOperator (  char * csOps )
{
//	return  (( GSMAPI_GetOperator )CALL_API( API_GSM, APIENTERY_GsmGetOperator, 1 ) )( csOps );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetOperator, 1 );
    cs.arg0 = csOps;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiModemMute(  BOOL bMute )
{
//	return  (( GSMAPI_OneParam)CALL_API( API_GSM, APIENTERY_GsmModemMute, 1 )) ( bMute );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmModemMute, 1 );
    cs.arg0 = bMute;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}
BOOL WINAPI GsmApiGetVolume( void ) 
{
//	return ( ( GSMAPI_NoParam)CALL_API( API_GSM, APIENTERY_GsmGetVolume, 0 )) (  );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmGetVolume, 0 );
    cs.arg0 = 0;
    return (HMENU)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}
BOOL WINAPI GsmApiSetVolume( int iVolume )
{
//	return ( ( GSMAPI_OneParam)CALL_API( API_GSM, APIENTERY_GsmSetVolume, 1 ) )( iVolume );
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmSetVolume, 1 );
    cs.arg0 = iVolume;
    return (HMENU)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}

BOOL WINAPI GsmApiRegisterUser( HWND hWnd )
{
//	BOOL bSuccess= ( ( GSMAPI_OneParam)CALL_API( API_GSM, APIENTERY_GsmRegisterUser, 1 )) ( (DWORD)hWnd);
	
//	if( bSuccess ) {
//		
//		RETAILMSG(1, (TEXT("GsmApiRegisterUser  success\r\n")));
//		
//	}else{
//		
//		RETAILMSG(1, (TEXT("GsmApiRegisterUser  failed\r\n")));
//		
//	}
//	return bSuccess;
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_GSM, APIENTERY_GsmRegisterUser, 1 );
    cs.arg0 = hWnd;
    return (BOOL)CALL_SERVER( &cs );
#else
	return FALSE;
#endif
	
}
