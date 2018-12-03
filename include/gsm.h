/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef _GSM_H_
#define _GSM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <telephone.h>

//---------------------------------------------------------------------------
//GSM ��
//---------------------------------------------------------------------------

//------------------------------------------
//GSM ���غ�
//------------------------------------------


//
//�ַ���---�����WINCE�£����� WCHAR��KINGMOS�£�����char
//

#if 0
#define	GSM_IS_WCHAR_SUPPORT
#define	GSM_IS_WCHAR		TRUE
typedef WCHAR               GSMCHAR;
typedef LPWSTR              LPGSMSTR;
typedef LPCWSTR             LPCGSMSTR;

#else
//#define	GSM_IS_WCHAR_SUPPORT
#define	GSM_IS_WCHAR		FALSE
typedef char                GSMCHAR;
typedef LPSTR               LPGSMSTR;
typedef LPCSTR              LPCGSMSTR;

#endif


//
//�ֻ� GSM���� �� GSMģ��Ӳ�� �Ĺ�ϵ
//
#define GSM_CPI				//��ʾΪӲ��֧��CPI����


#define GSM_WAVECOM			//��ʾΪ���� WAVECOM ��Ӳ��


#ifdef GSM_WAVECOM
#undef GSM_CPI
#endif


//------------------------------------------
//GSM error
//------------------------------------------

#define	GERR_OK							0	//�ɹ�

#define	GERR_ME_MIN						3
#define	GERR_ME_MAX						47
#define	GERR_BASENEW					2000

#define	GERR_PENDING					(GERR_BASENEW+  0)	//GSMģ�鷵�أ����첽��ʼ����
#define	GERR_NO_INIT					(GERR_BASENEW+  1)	//GSMģ��û�г�ʼ���������Ѿ��˳�
#define	GERR_PARAM						(GERR_BASENEW+  2)	//�������ʧ��
#define	GERR_DEV_NOT_READY				(GERR_BASENEW+  3)	//������ȷ���豸׼����
#define	GERR_CANCEL						(GERR_BASENEW+  4)	//
#define	GERR_FAIL						(GERR_BASENEW+  5)	//����ʧ�ܣ���֪ԭ��
#define	GERR_TRY_AGAIN					(GERR_BASENEW+  6)	//���ܲ�����æ��������
#define	GERR_NOFOUND					(GERR_BASENEW+  7)	//����ʧ�ܣ�û���ҵ�AT�������ȷ����

#define	GERR_SYSFAIL					(GERR_BASENEW+  10)	//�����̡߳������ڴ桢��ʧ��
#define	GERR_OPENPORT					(GERR_BASENEW+  11)	//�򿪶˿�ʧ��
#define	GERR_CFGPORT					(GERR_BASENEW+  12)	//���ö˿�ʧ��

#define	GERR_SIM_UNREADY				(GERR_BASENEW+  13)	//SIM��״̬��û��READY��Ҳ����û��INS
#define	GERR_IS_DIALING					(GERR_BASENEW+  14)	//���ڲ��ţ����Ժ�����
#define	GERR_NO_PB_SMS					(GERR_BASENEW+  15)	//

#define	GERR_NOANSWER					(GERR_BASENEW+ 21)	//����û��Ӧ��
#define	GERR_NODIALTONE					(GERR_BASENEW+ 22)	//����û�в�����
#define	GERR_NOCARRIER					(GERR_BASENEW+ 23)	//����û���ز��ź�
#define	GERR_BUSY						(GERR_BASENEW+ 24)	//����æ

#define GERR_CALL_HAS_DISCONNECTED      (GERR_BASENEW + 25)	//ͨ���ѹҶ�

#define	GERR_OP_NOT_ALLOWED				3  // operation not allowed
#define	GERR_OP_NOT_SUPPORTED			4  // operation not supported
#define	GERR_PH_SIM_PIN_REQ				5  // PH-SIM PIN required
#define	GERR_PH_FSIM_PIN_REQ			6  // PH-FSIM PIN required
#define	GERR_PH_FSIM_PUK_REQ			7  // PH-FSIM PUK required
#define	GERR_SIM_NOT_INS				10 // SIM not inserted
#define	GERR_SIM_PIN_REQ				11 // SIM PIN required
#define	GERR_SIM_PUK_REQ				12 // SIM PUK required
#define	GERR_SIM_FAIL					13 // SIM failure
#define	GERR_SIM_BUSY					14 // SIM busy
#define	GERR_SIM_WRONG					15 // SIM wrong
#define	GERR_INCORRECT_PW				16 // incorrect password
#define	GERR_SIM_PIN2_REQ				17 // SIM PIN2 required
#define	GERR_SIM_PUK2_REQ				18 // SIM PUK2 required
#define	GERR_MEMORY_FULL				20 // memory full
#define	GERR_INVALID_INDEX				21 // invalid index
#define	GERR_NOT_FOUND					22 // not found
#define	GERR_MEMORY_FAIL				23 // memory failure
#define	GERR_TEXTSTR_TOO_LONG			24 // text string too long
#define	GERR_INVALID_CHAR_IN_TEXTSTR	25 // invalid characters in text string
#define	GERR_DIALSTR_TOO_LONG			26 // dial string too long
#define	GERR_INVALID_CHAR_IN_DIALSTR	27 // invalid characters in dial string
#define	GERR_NO_NETWORK_SRV				30 // no network service
#define	GERR_NETWORK_TIMEOUT			31 // network timeout
#define	GERR_EMERGENCY_CALLS_ONLY		32 // network not allowed - emergency calls only
#define	GERR_NETWORK_PIN_REQ			40 // network personalisation PIN required
#define	GERR_NETWORK_PUK_REQ			41 // network personalisation PUK required
#define	GERR_NETWORK_SUB_PIN_REQ		42 // network subset personalisation PIN required
#define	GERR_NETWORK_SUB_PUK_REQ		43 // network subset personalisation PUK required
#define	GERR_NETWORK_PROV_PIN_REQ		44 // service provider personalisation PIN required
#define	GERR_NETWORK_PROV_PUK_REQ		45 // service provider personalisation PUK required
#define	GERR_CO_PIN_REQ					46 // corporate personalisation PIN required
#define	GERR_CO_PUK_REQ					47 // corporate personalisation PUK required
#define GERR_STK_NOT_INIT				48 // STK����δ��ʼ��
#define GERR_STK_ILLEGAL_SESSION_STEP	49 // ���Ϸ���STK�Ự���衣
#define GERR_STK_NOT_SUPPORT			50 //  ģ���SIM����֧��STK����

#define	GERR_UNKNOWN					100// δ֪����

#define	GERR_SMSC_ADDR_UNKNOWN			330// SMSC address unknown

//------------------------------------------
//GSM Message
//------------------------------------------

//GSM Message Offset
#define	PHM_STATE				0 //wParam---see the following

#define	PHM_CALLPRC				1 //wParam==MAKEDWORD( MAKEWORD(bCallCid,bCallMsgType), 0 ), lParam==dwTickCnn
#define	PHM_CALLIN				2 //wParam==bCallCid, lParam==bRingType
#define	PHM_CALLWAIT			3 //wParam==bCallCid

#define	PHM_NEWSMS				5 //wParam==dwIndex
#define	PHM_NEWCBM				6 //��ʱ��֧��
				
#define	PHM_MAXCNT				10 //�뱣��n����Ϣ��GSMģ��ʹ��

//PHM_STATE's wParam
#define	PHS_FATAL				1 //GSM-Phone �������ش���
#define	PHS_PINS				2 //GSM-Phone PIN��״̬
#define	PHS_READSIM				3 //GSM-Phone ���Կ�ʼ��PB SMS

#define	PHS_SQ					5 //GSM-Phone ��ʾ �ź�ǿ��---lParam is ǿ����ֵ
#define	PHS_GSMNET				6 //GSM-Phone ��ʾGSM ����״̬
#define	PHS_GPRSNET				7 //GSM-Phone ��ʾGPRS����״̬

#define	PHS_BATTERY				10//GSM-Phone ��ʾ Battery ״̬

//���յ�STK��Proactive commandʱ���û�������WM_COMMAND��Ϣ,HIWORD(wParam)������������,LPARAMΪNULL
#define STK_SETUPMENU_SEL		0 //�Ự��ʼ����ʾ�û��õ���ʼ�˵�
#define STK_DISP_TEXT			1 //Ҫ���Ƿ���ʾһ���ı�
#define STK_GET_INKEY			2 //�õ��û�����
#define STK_GET_INPUT			3 //Ҫ��õ��û������һ���ı�
#define STK_SETUP_CALL			4 //��ʼ��绰
#define STK_PLAY_TONE			5 //������ʾ��Ƶ
#define STK_SEL_ITEM			6 //ѡ��˵�
#define STK_SEND_SMS			9 //STK������һ�����š�
#define STK_SESSION_END			99 //�Ự����

//�û����Ե���BOOL Gsm_StkMakeResponse(IN WORD wResponseCmdType, PVOID pData)
//������Ӧ������Ӧ��
#define STK_RES_SETUPMENU_SEL		0 //ѡ��ʼ�˵��Ĳ˵��pData=�˵����ʶ��
#define STK_RES_CONFIRM_DISPTEXT	1 //ȷ����ʾ�ı�,pDataΪNULL����ӦSTK_DISP_TEXT
#define STK_RES_GET_INKEY			4 //���û��İ�����Ϣ���͸�STK����ʽ��ӦSTK_GET_INKEY��pDataΪ������ascii��
#define STK_RES_GET_INPUT			6 //���û���������Ϣ���͸�STK����ʽ��ӦSTK_GET_INPUT��pDataΪ������Ϣ���ı������ַ��
#define STK_RES_SEL_ITEM			7 //ѡ��˵���������ӦSTK_SEL_ITEM��pDataΪ�˵����ʶ��
#define STK_RES_CONFIRM_CALL		8 //ȷ�Ͽ�ʼ��绰����ӦSTK_SETUP_CALL��pDataΪNULL
#define STK_RES_CANCEL_CALL			9 //ȡ����绰����ӦSTK_SETUP_CALL��pDataΪNULL
#define STK_RES_END_SESSION			99 //�û�Ҫ������Ự.
#define STK_RES_MOVEBACK			95 //�˵����˵���һ��.

#define STK_SETUP_CALL_NOT_BUSY		0
#define STK_SETUP_CALL_HELD_ALL		1
#define STK_SETUP_CALL_DROP_ALL		2

#define STK_DISP_TEXT_NEED_CONFIRM	1
#define STK_DISP_TEXT_NEED_NOT_CONFIRM 0

#define STK_INPUT_DIGIT				0 //�õ�����ʱ�û�������������(0-9,*,+,#)
#define STK_INPUT_TEXT				1 //�õ�����ʱΪ�ı���

//------------------------------------------
//GSM AT Information
//------------------------------------------

//PIN��״̬ ( PHS_PINS's lParam, dwPinState )
#define	PINS_READY				0
#define	PINS_NOT_INS			1
#define	PINS_SIM_PIN			4
#define	PINS_SIM_PUK			5
#define	PINS_SIM_PIN2			6
#define	PINS_SIM_PUK2			7
#define	PINS_PH_SIM_PIN			8
#define	PINS_PH_NET_PIN			9

#define	PINS_UNKNOWN			11


//����״̬ ( PHS_GSMNET's lParam, PHS_GPRSNET's lParam, bNetStatus )
#define	NETS_UNREGISTERED		0x30 //GSM���� û��ע��
#define	NETS_REGHOME			0x31 //GSM���� ע�ᵽ����
#define	NETS_SERACHING			0x32 //GSM���� ���ڲ���
#define	NETS_DENIED				0x33 //GSM���� ���ܾ�ע��
#define	NETS_UNKNOWN			0x34 //GSM���� ��֪��״̬
#define	NETS_REGROAM			0x35 //GSM���� ע�ᵽ����

#define	NETS_WAITTING			0x00 //������ȴ� GSM���緵����Ϣ


// ( PHS_BATTERY's lParam, bBatStatus )
#define	BATS_REACH_MIN			0
#define	BATS_REACH_MAX			1


//��������Ĭ����ֵ
#define	GSMVOL_MIN				1
#define	GSMVOL_MAX				5


//GSM������ ( bGsmClassx )
#define	GSMCLSX_VOICE			0x01
#define	GSMCLSX_DATA			0x02
#define	GSMCLSX_FAX				0x04
#define	GSMCLSX_SMS				0x08

#define	GSMCLSX_ALL				(GSMCLSX_VOICE |GSMCLSX_DATA |GSMCLSX_FAX)
#define	GSMCLSX_USEDEF			0


//���й��� ( PHM_CALLPRC's bCallMsgType )
#define	CALLPRC_SYNC			4
#define	CALLPRC_ALERT			2
#define	CALLPRC_DESC			5
#define	CALLPRC_CONNECT			6
#define	CALLPRC_DISCONNECT		1
#define CALLPRC_OP_HOLD 7 //�Է�����ͨ��
#define CALLPRC_OP_RECOVER 8//�Է��ָ�ͨ��
#define CALLPRC_UCF_ACTIVE		9 //�Է�����Ϊ����������ת��
#define CALLPRC_CF_ACTIVE		10 //�Է�����Ϊ������ת��
#define CALLPRC_FORWRDING		11 //���ں���ת��

#define	CALLPRC_STATUS			0xFF

#define	CALLCID_UNKNOWN			0xFF
#define	CALLCID_MAKE			0xFE

//�������� ( PHM_CALLIN's bRingType )
#define	CALLTYPE_VOICE			1
#define	CALLTYPE_FAX			2
#define	CALLTYPE_ASYNC			3
#define	CALLTYPE_SYNC			4


//����ת������ (bCFwReason)
#define	CFW_R_UNCONDITIONAL		0
#define	CFW_R_BUSY				1
#define	CFW_R_NO_REPLY			2
#define	CFW_R_NOT_REACHABLE		3
#define	CFW_R_ALL_CALL			4
#define	CFW_R_ALL_CONDITIONAL	5

//��������ʱ���������
#define	SMS_ALLENTRY_TYPE				1
#define SMS_RECVUNREADENTRY_TYPE		2
#define PB_ALLENTRY_TYPE				5

//�绰�������� (bNumType)
#define	NUMTYPE_UNKNOWN			0x81 //129
#define	NUMTYPE_INTER			0x91 //145
#define	NUMTYPE_NATIONAL		0xA1 //161
#define	NUMTYPE_NETWORK			0xB1 //177


//�绰��ѡ�� (dwPbStor)
#define	PBSTOR_SM				0 // [RWD] SIM phonebook
#define	PBSTOR_LD				1 // [R  ] SIM last-dialling-phonebook
#define	PBSTOR_MC				2 // [RD ] ME missed (unanswered received) calls list
#define	PBSTOR_ON				3 // [RW ] SIM (or ME) own numbers (MSISDNs) list (reading AT: +CNUM)
#define	PBSTOR_ME				4 // [RWD] ME phonebook
#define	PBSTOR_RC				5 // [RD ] ME received calls list
#define	PBSTOR_MT				6 // [R  ] combined ME and SIM phonebook
#define	PBSTOR_SN				7 // [R  ] services dialing phonebook

#define	PBSTOR_DC				8 // [R  ] ME dialled calls list------------------ !!! ---
#define	PBSTOR_EN				9 // [R  ] SIM (or ME) emergency number
#define	PBSTOR_FD				10// [R  ] SIM fixdialling-phonebook
#define	PBSTOR_TA				11// [R  ] TA phonebook

#define	PBSTOR_MAX				(PBSTOR_SN+1)
#define	PBSTOR_UNKNOWN			(PBSTOR_MAX+1)
#define	PBSTOR_CURRENT			(PBSTOR_UNKNOWN+1)


//ָ���豸��Ϣ (dwMeInfoID)
#define	MEINFO_MI				0
#define	MEINFO_MM				1
#define	MEINFO_MR				2
#define	MEINFO_SN				3
#define	MEINFO_IMEI				4


//ɾ����־ (dwDelFlag)
#define		SMSD_INDEX			0 // specified in <index>.
#define		SMSD_READ			1 // read.
#define		SMSD_READ_SENT		2 // read , sent out
#define		SMSD_NOT_UNREAD		3 // read , sent out , stored but not sent
#define		SMSD_ALL			4 // 


//---------------------------------------------------------------------------
//GSM struct
//---------------------------------------------------------------------------

//�绰�� ��д						//Write--------Read
//typedef	struct	_PB_ENTRY
//{
//	DWORD			dwIndex;		//[IN]         [IN]
//
//	//BYTE			bNumType;		//[no use]     [OUT]
//	LPSTR			pszNumber;		//[IN]         [OUT]
//	DWORD			dwCntNum;		//[IN]         [IN/OUT]
//
//	LPGSMSTR		pBufName;		//[IN]         [OUT] //����ϵͳ����ӦΪ char or wchar
//	DWORD			dwCntName;		//[IN]         [IN]  //the count of char or wchar
//
//} PB_ENTRY, *LPPB_ENTRY;


//���� ��
typedef	struct	_SMS_ENTRY
{
	//����
	DWORD			dwIndex;		//[IN]

	//�Ƿ����ɹ�
	BYTE			bDecOk;			//[OUT]

	//�������� �� ������������
	BYTE			bDataType;		//[OUT]

	//�洢״̬
	BYTE			bSmsState;		//[OUT]

	//������
	BYTE			bNumType;		//[OUT]
	LPSTR			pszNumber;		//[OUT] // xyg add: pszNumber[0]==0 ��ʾû��
	DWORD			dwCntNum;		//[IN]
	LPSTR			pszNumPb;		//[OUT] // xyg add: pszNumber[0]==0 ��ʾû��
	DWORD			dwCntNumPb;		//[IN]
	//����ʱ��
	SYSTEMTIME		stSysTime;		//[OUT] // xyg add: ��stSysTime.wMonth==(WORD)-1 ��ʾû��

	//��������
	LPGSMSTR		pBufText;		//[OUT]
	DWORD			dwCntText;		//[IN]  //the count of char or wchar
	//DWORD			dwCntTextGet;	//[OUT] //the count of char or wchar

	//
	//������ xyg ��ӵģ�Ϊ�˷��㴦��������͵Ķ���
	//
	//������ �����������Ķ��ţ�xyg������Ӧ�������ҷ����Ƿ�ɹ���
	BOOL			fIsHandled;		//[OUT]

	//����Ƿָ��ͨ�����Ͷ���( bDataType==SMST_EMSCAT )ʱ��ʹ�����³�Ա
	BYTE			bNumRef;		//[OUT]
	BYTE			bNumMax;		//[OUT]
	BYTE			bNumCur;		//[OUT]

} SMS_ENTRY, *LPSMS_ENTRY;

//this struct added by tanphy for phone book batched reading
typedef struct _PB_ENTRY{
	DWORD dwSize;
	LPSTR pszPhoneNumber;//[IN] ��ŵ绰�����buffer���׵�ַ
	LPSTR pszName;//[IN] ���������buffer���׵�ַ
	
	WORD  wPhoneLen;//[IN] ��ŵ绰�����buffer�Ĵ�С
	WORD  wNameLen;//[IN] ���������buffer�Ĵ�С

	WORD  wIndex;//[OUT]  ��GsmPb_GetFirstEntry��GsmPb_GetNextEntry�õ�һ����ϵʱ�����ظ���ϵ�ڵ绰������Ӧ��������
	WORD  wTotalEntries;//[IN] ����GsmPb_GetFirstEntryʱӦ����ĵ绰�������������
}PB_ENTRY, *PPBENTRY;

typedef struct _STK_CMDINFO{
	DWORD dwSize;
	WORD wCmdType;
	WORD wReserve;
	union _CMD_PARAM{
		struct _MENUSEL_PARAM{
			LPSTR pszMenuInfoBuf;//��Ϊ"�˵�����\0\0�˵����ʶ��1\0\0�˵���1\0\0�˵����ʶ��2\0\0�˵���2\0\0...�˵����ʶ��n\0\0�˵���n\0\0\0
			WORD wBufLen;//�˵���Ϣ����
			WORD wItemNum;//�˵������
		}stMenuSelParam;
		struct _DISPTEXT_PARAM{
			LPSTR pszPromptInfoBuf;//��ʾ�ı������ַ
			WORD wBufLen;//��ʾ�ı��ĳ���
			WORD wNeedConfirm;//����STK_DISP_TEXT_NEED_CONFIRM��ʾҪ�û���STK_RES_CONFIRM_DISPTEXTӦ����ܵõ���һ������;
							//����STK_DISP_TEXT_NEED_NOT_CONFIRM��ʾ�û�����Ӧ��ֻ��ȴ���һ�����
		}stDispTextParam;
		
		struct _PLAYTONE_PARAM{
			WORD wToneType;//��Ƶ����,�û����ݸ����Ͳ����ֻ��ж�Ӧ���͵���Ƶ��
			WORD wTonePlayLong;//��Ƶ�ĳ��ȣ��룩
			LPSTR pszTextBuf;//�ı���Ϣ
			WORD  wTextBufLen;//�ı�����
			WORD wReserve;//����
		}stPlayToneParam;
		
		struct _GETINKEY_PARAM{
			LPSTR pszPromptInfoBuf;//�õ����밴��ʱ����ʾ��Ϣ
			WORD wBufLen;//��ʾ��Ϣ�ĳ���
			WORD wInkeyFormat;//�������ݵĸ�ʽSTK_INPUT_DIGIT��STK_INPUT_TEXT
		}stGetInKeyParam;

		struct _GETINPUT_PARAM{
			LPSTR pszPromptInfoBuf;//��ʾ�ı������ַ
			WORD wBufLen;//��ʾ�ı��ĳ���
			WORD wInputFormat;//�������ݵĸ�ʽSTK_INPUT_DIGIT,STK_INPUT_TEXT
			WORD wMaxInput;//�������
			WORD wMinInput;//��������
		}stGetInputParam;
		
		struct _SETUPCALL_PARAM{
			LPSTR pszCalledNbBuf;//���з����绰����
			LPSTR pszCalledSubNbBuf;//���з��ӵ绰����
			WORD wNbBufLen;//���绰���볤��
			WORD wSubNbBufLen;//�ӵ绰���볤��
			WORD wType;//STK_SETUP_CALL_NOT_BUSY;STK_SETUP_CALL_HELD_ALL;STK_SETUP_CALL_DROP_ALL
			WORD wClass;//GSMCLSX_VOICE;GSMCLSX_DATA;GSMCLSX_FAX
		}stSetupCallParam;
		
		struct _OTHERTYPE_PARAM{
			LPSTR pszTextBuf;
			WORD wBufLen;
			WORD wReserve;
		}stOtherParam;
	}unCmdParam;
}STK_CMDINFO, *PSTK_CMDINFO;

//added by tanphy
typedef struct _SMS_SETTINGINFO{
	DWORD dwSize;
	BYTE bTprp:1; //�ظ�·��
	BYTE bTpsrp:1;//״̬����
	BYTE bTpid:6;//��Ϣ��ʽ
	BYTE bTpvp;//��Ч����
	BYTE bNumType_SCA;//�������ĺ�������
	BYTE szSCA[MAX_PHONE_LEN + 1];//�������ĺ���
	BYTE bReserve[8];
}SMS_SETTINGINFO, *PSMS_SETTINGINFO;

#define SMS_TEXT_FORMAT 0
#define SMS_VOICE_FORMAT 0x4
#define SMS_EMAIL_FORMAT 0x12
#define SMS_FAX_FORMAT 0x02
//Reference to gsm03.40,9.2.3.12.1
#define SMS_VALIDITY_1H 11
#define SMS_VALIDITY_6H 23
#define SMS_VALIDITY_12H 143
#define SMS_VALIDITY_1W  193
#define SMS_VALIDITY_1D  167
#define SMS_VALIDITY_LONGEST 0xff
//bSmsState

#define LOCAL_SMSINDEX_BASE (10000)
#define		SMSS_RECV_UNREAD	0 // recieved but not read.
#define		SMSS_RECV_READ		1 // recieved and read.
#define		SMSS_STO_UNSENT		2 // stored but not sent yet.
#define		SMSS_STO_SENT		3 // stored and sent out.
#define		SMSS_ALL			4 // all message
#define		SMSS_WAITTING		0xFF// xyg add: ��ʾ������ȷ����ֵ

//bDataType
#define		SMST_NORMAL			0
#define		SMST_UNKNOWN		1

#define		SMST_EMSCAT			3
#define		SMST_EMS			4
#define		SMST_WAP			5



//---------------------------------------------------------------------------
//GSM function
//---------------------------------------------------------------------------


//------------------------------------------
//GSM config
//------------------------------------------

extern	BOOL	GsmCfg_GetPort( OUT LPSTR pszPort, DWORD dwLen );
extern	BOOL	GsmCfg_SetPort( LPCSTR pszPort );


//------------------------------------------
//ע�ᡢ����
//------------------------------------------
extern	BOOL	GsmGen_Register( HWND hWnd, DWORD dwMsgBase );
extern	DWORD	GsmGen_Start( );


//------------------------------------------
//SIM and PIN
//------------------------------------------
extern	DWORD	GsmSim_ReadID( OUT LPSTR pszSimID, IN OUT DWORD* pdwLen );

extern	DWORD	GsmPin_GetState( OUT DWORD* pdwPinState );
extern	DWORD	GsmPin_Enter( LPSTR pszPuk, LPSTR pszPin );
extern	DWORD	GsmPin_Enter2( LPSTR pszPuk2, LPSTR pszPin2 );
extern	DWORD	GsmPin_Change( LPSTR pszPinOld, LPSTR pszPinNew );
extern	DWORD	GsmPin2_Change( LPSTR pszPinOld, LPSTR pszPinNew );
extern	DWORD	GsmPin_GetAttempt(PBYTE pbPin, PBYTE pbPin2, PBYTE pbPuk, PBYTE pbPuk2); // WaveCom

extern	DWORD	GsmSim_QueryLock( OUT BOOL* pfIsLockSim );
extern	DWORD	GsmSim_ExeLock( BOOL fIsLockSim, LPSTR pszPin );


//------------------------------------------
//Network and GPRS
//------------------------------------------
extern	DWORD	GsmOper_QueryNet( OUT OPTIONAL BYTE* pbNetStatus );
extern	DWORD	GsmOper_ReadCur( LPSTR pszOperName, DWORD dwLen );
extern	DWORD	GsmOper_ReadAvail( OUT DWORD* pdwOperNum, IN OUT DWORD* pdwSize );
extern	DWORD	GsmOper_Register( BOOL fIsAutoregOrUnreg );
extern	DWORD	GsmOper_SetManual( DWORD dwOperNum );

extern	DWORD	GsmGprs_QueryNet( OUT OPTIONAL BYTE* pbNetStatus );
extern	DWORD	GsmGprs_Attach( BOOL fIsAttach );
extern	DWORD	GsmGprs_SetContext( DWORD dwCid, LPSTR pszApn );
extern	DWORD	GsmGprs_GetClass( OUT DWORD* pdwGprsClass );
extern	DWORD	GsmGprs_SetClass( DWORD dwGprsClass );


//------------------------------------------
//Control and Status 1
//------------------------------------------
extern	DWORD	GsmGen_GetMeInfo( DWORD dwMeInfoID, OUT LPSTR pszMeInfo, IN OUT DWORD* pdwLen );

extern	DWORD	GsmMute_Get( OUT BOOL* pfIsMute );
extern	DWORD	GsmMute_Set( BOOL fIsMute );

extern	DWORD	GsmVol_GetRange( BOOL fIsRealVal, OUT WORD* pwVolMin, OUT WORD* pwVolMax );
extern	DWORD	GsmVol_SetRange( BOOL fIsRealVal, WORD wVolMin, WORD wVolMax );
extern	DWORD	GsmVol_Get( BOOL fIsRealVal, OUT WORD* pwVolCur );
extern	DWORD	GsmVol_Set( BOOL fIsRealVal, WORD wVolCur );

extern	DWORD	GsmGen_GetSQ( OUT DWORD* pdwSQ );
extern	DWORD	GsmGen_GetPAS( OUT DWORD* pdwPAS );


//------------------------------------------
//Control and Status 2
//------------------------------------------
extern	DWORD	GsmGen_GetAutoAnswer( OUT DWORD* pdwSecsBeforeAnswer );
extern	DWORD	GsmGen_SetAutoAnswer( DWORD dwSecsBeforeAnswer );

extern	DWORD	GsmCallFw_Query( BYTE bCFwReason, OUT BOOL* pfCFwActive, OUT BYTE* pbGsmClassx, OUT LPSTR pszNumber, DWORD dwCntNum );
extern	DWORD	GsmCallFw_Set( BYTE bCFwReason, BYTE bGsmClassx, LPSTR pszNumber );
extern	DWORD	GsmCallFw_Enable( BYTE bCFwReason, BOOL fCFwEnable );
extern	DWORD	GsmCallFw_Delete( BYTE bCFwReason );

extern	DWORD	GsmCallWait_Query( OUT BOOL* pfEnableCW );
extern	DWORD	GsmCallWait_Set( BOOL fEnableCW, BYTE bGsmClassx );


//------------------------------------------
//Phonebook
//------------------------------------------
extern	DWORD	GsmPb_SetCurStor( DWORD dwPbStor );
extern	DWORD	GsmPb_GetCurStor( OUT DWORD* pdwPbStor, OUT DWORD* pdwCntUsed, OUT DWORD* pdwCntTotal );
extern	DWORD	GsmPb_ReadLens( OUT DWORD* pdwLenNumber, OUT DWORD* pdwLenText );



extern	DWORD	GsmPb_GetEntry( DWORD dwIndex, OUT LPSTR pszNumber, DWORD dwCntNum, OUT LPGSMSTR pBufName, DWORD dwCntName );
extern	DWORD	GsmPb_WriteEntry( DWORD dwIndex, LPSTR pszNumber, LPGSMSTR pBufName );
extern	DWORD	GsmPb_DeleteEntry( DWORD dwIndex );


//------------------------------------------
//SMS
//------------------------------------------
extern	DWORD	GsmSms_GetSCA( OUT LPSTR pszSCA, DWORD dwLenSCA, OUT OPTIONAL BYTE* pbNumType );
extern	DWORD	GsmSms_SetSCA( LPSTR pszSCA, OUT OPTIONAL LPSTR pszSCA_Read, DWORD dwLenSCA_Read, OUT OPTIONAL BYTE* pbNumType_Read );
extern	DWORD	GsmSms_ReadCnt( OUT DWORD* pdwCntUsed, OUT DWORD* pdwCntTotal );
extern  DWORD   GsmSms_DoSeting( IN PSMS_SETTINGINFO pSmsSetingInfo);
extern  DWORD   GsmSms_GetSeting( OUT PSMS_SETTINGINFO pSmsSetingInfo);
extern	DWORD	GsmSms_GetEntry( IN OUT SMS_ENTRY* lpSmsEntry );
extern	DWORD	GsmSms_SendMsg( LPSTR pszNumber, LPGSMSTR pBufText, DWORD dwBytesText, OUT DWORD* pdwBytesSent );
extern	DWORD	GsmSms_WriteMsg( LPSTR pszNumber, LPGSMSTR pBufText, DWORD dwBytesText, OUT DWORD* pdwBytesWritten, OUT DWORD* pdwIndex );
extern	DWORD	GsmSms_SendStor( DWORD dwIndex, OPTIONAL LPSTR pszNumber );
extern	DWORD	GsmSms_DeleteStor( DWORD dwIndex, OPTIONAL DWORD dwDelFlag );

//added by tanphy
//���ú͵õ�����Ϣ�Ľ��շ�ʽ��
#define REC_SMS_TEMPSAVETO_RAM 1 //��mobile����Ϊ�÷�ʽʱ�յ��Ķ���Ϣ�ݴ���RAM,AP���յ�֪ͨʱҪ��ʱȥ��ȡ��ת�����������ᶪʧ��
#define REC_SMS_SAVETO_SIMCARD 2 //��mobile����Ϊ�÷�ʽʱ�յ��Ķ���Ϣ������SIM����

extern	DWORD	GsmSms_SetRecMode(DWORD dwRecMode);//���ö���Ϣ�Ľ��շ�ʽ������GERR_OK��ʾ�ɹ��������ʾʧ��
extern	DWORD	GsmSms_GetRecMode(DWORD *pdwRecMode);//�õ�����Ϣ�Ľ��շ�ʽ����GERR_OK��ʾ�ɹ��������ʾʧ��

extern	DWORD	GsmSms_GetUnchange( OUT BOOL* pfIsUnchange ); // WaveCom
extern	DWORD	GsmSms_SetUnchange( BOOL fIsUnchange ); // WaveCom
extern	DWORD	GsmSms_ChangeState( DWORD dwIndex, BYTE bSmsState ); // WaveCom

//------------------------------------------
//Call
//------------------------------------------
//�绰״̬
#define CALL_ACTIVE 0 //���˵������ͨ��
#define CALL_HELD 1 //����
#define CALL_DIALING 2 //���ڰκ�
#define CALL_ALERTING 3//���ź�
#define CALL_INCOMING 4//����
#define CALL_WAITING 5//�ȴ�
#define CALL_IDLE 6//���У�˵���õ绰�Ѿ��Ҷϻ��ͷ��ˡ�

//�෽ͨ��ʱӦ��ʽ
#define CALL_ANSWER_NORMAL 0 //��ǰ��û��ͨ��ʱ���ø÷�ʽӦ��
#define CALL_ANSWER_HOLD_CUR 1//���ֵ�ǰͨ������ͨ���еȴ�
#define CALL_ANSWER_REPL_CUR 2//�����еȴ��ĵ绰�滻��ǰͨ��

extern	DWORD	GsmCall_Make( LPSTR pszNumber,OUT PBYTE pbCallID);
extern	DWORD	GsmCall_Hangup(BYTE bCallID, BOOL fHangAll);//�Ҷ�,fHangAllΪTRUE��ʾ�Ҷ�����
extern	DWORD	GsmCall_Answer(BYTE bCallID, DWORD dwAnserMode);//����,dwAnserModeΪCALL_ANSWER_NORMAL��CALL_ANSWER_HOLD_CUR,CALL_ANSWER_REPL_CUR֮һ
extern	DWORD	GsmCall_SendTone(char chTone);
extern	DWORD   GsmCall_Release(BYTE bCallID);//�ͷ�
extern  DWORD	GsmCall_Recover(BYTE bCallID);//�ָ�
extern  DWORD	GsmCall_Hold(BYTE bCallID);//����
extern	DWORD	GsmCall_ReadNumIn( OUT LPSTR pszNumIn, DWORD dwCntNum, OUT LPSTR pszNumPbIn, DWORD dwCntNumPb, BYTE bCallID );
extern	DWORD	GsmCall_Reject( BYTE bCallID );//�ܾ�
extern	DWORD	GsmCall_Switch();//��·�л�
extern	DWORD	GsmCall_GetState(BYTE bCallID, PBYTE *pBstate);//�õ��绰״̬,���֣�����ȴ���.

extern HANDLE Gsm_GetFirstEntry(IN OUT LPVOID pEntry, IN DWORD dwEntryType);
extern BOOL Gsm_GetNextEntry(HANDLE hFind, IN OUT PPBENTRY pEntry);
extern BOOL Gsm_CloseFind(HANDLE hFind);

extern DWORD Gsm_StkSetupServ(HANDLE hUserAp, PVOID pUserApInfo, BOOL fIsWnd);
extern DWORD Gsm_StkGetCmdInfo(OUT PSTK_CMDINFO pstStkCmdInfo);
extern DWORD Gsm_StkMakeResponse(IN WORD wResponseCmdType, PVOID pData);
//------------------------------------------
//
//------------------------------------------

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_GSM_H_

