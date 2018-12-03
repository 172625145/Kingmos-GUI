/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵������GSM��MODEM�����š��������������õ��ĺ�����
�汾�ţ�1.0.0
����ʱ�ڣ�2003-6-18..
���ߣ�κ����
�޸ļ�¼��
******************************************************/
#ifndef	_GMS_API_H_
#define _GMS_API_H_

// *********************************************************************************************************
// Note:
// Explorer Call GsmApiRegisterUser ��GSM��serverע��,����������CALL���������
//�������뷢���ŵĳ���ֻҪcall  GsmApiSendMsg�����ɡ�
//  
// Explorer��ע���Ժ󣬿��ܻ��յ�������Ϣ��
//
// WM_NEW_MSG: �յ�һ���µĶ���Ϣ��(wParam: index of the message. lParam:0).
// WM_ENTER_CALL: �յ��绰��	   (wParam: phone number or 0, lParam: type of the number).
// WM_CALL_STAT:  ����֮�󣬵�ǰ��·��״̬(wParam: any of CALL_CONNECTED .....CALL_FINISHED.)
// *********************************************************************************************************
#define		MIN_CMD_RESULT  5
#define		MAX_MSG		500
#define		MAX_PHN		60


#define     WM_NEW_MSG		(WM_OS+2)	//a new message is coming.
#define		WM_ENTER_CALL   (WM_OS+3) //a new call is coming.
#define     WM_CALL_STAT	(WM_OS+4) //we initiate a call, and the line state changed.
#define     WM_MODEM_CHANGED	(WM_OS+5) //modem device is pulled out(wParm==0) or inserted (wParam=1).


//status  of the current call... these three value is filled as wParm when WM_CALL_STAT is set.
#define		CALL_CONNECTED	1	//the line is connected.
#define		CALL_BUSY		2	//the other peer is busy.
#define		CALL_FAILED		3   //the calling failed, or terminated by the other peer,
#define		CALL_WAITING	4   //
#define		CALL_FINISHED	5
//type of a message.
#define		RECV_UNREAD		0 // the message is recieved but not read.
#define		RECV_READ		1 // it's recieved and read.
#define		STO_SENT		3 // we stored the message and sent it out.
#define		STO_UNSENT		2 // we stored it but not sent yet.



#define		CALL_FORWARD_ALL	0
#define		CALL_FORWARD_BUSY	1
#define		CALL_FORWARD_NO_REPLAY	2
#define		CALL_FORWARD_NON_REACH	3


typedef struct MSG_INFO{
	int iIndex;
	char *csPhoneNum;
	int iPHLen;
	char *csMsg;
	int iMsgSize;
	SYSTEMTIME stMsgTime;
	int iType;
}MSG_INFO;


BOOL WINAPI GsmApiSendMsg( char *csPhone, char * csMsg  );
BOOL WINAPI GsmApiGetMsg( MSG_INFO* pMsg );
BOOL WINAPI GsmApiDeleteMsg( int iIndex );
int  WINAPI GsmApiGetMsgNum(  );


BOOL WINAPI GsmApiWriteChar( char cCh );
BOOL WINAPI GsmApiAnswerCall(  ) ;
BOOL WINAPI GsmApiHangUp(  );
int  WINAPI GsmApiWritePhoneEntry( int iIndex, char * csPhone, char * csText );
BOOL WINAPI GsmApiDialUp( char * csPhone );
int WINAPI GsmApiGetSimPhoneNum (   );
int WINAPI GsmApiDeletePhoneEntry( int iIndex );
int WINAPI GsmApiGetPhoneEntry( int iIndex, char * csPhone, char * csText, int * pType );
int WINAPI GsmApiGetSignalVal( );

BOOL WINAPI GsmApiDregisterNetwork( );
BOOL WINAPI GsmApiRegisterNetwork(  );
BOOL WINAPI GsmApiGetOperator (  char * csOps );
BOOL WINAPI GsmApiModemMute(  BOOL bMute);
BOOL WINAPI GsmApiGetVolume(  );
BOOL WINAPI GsmApiSetVolume( int iVolume );

BOOL WINAPI GsmApiRegisterUser( HWND hWnd );

#endif
