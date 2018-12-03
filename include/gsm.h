/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _GSM_H_
#define _GSM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <telephone.h>

//---------------------------------------------------------------------------
//GSM 宏
//---------------------------------------------------------------------------

//------------------------------------------
//GSM 开关宏
//------------------------------------------


//
//字符集---如果在WINCE下，是用 WCHAR；KINGMOS下，是用char
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
//手机 GSM功能 与 GSM模块硬件 的关系
//
#define GSM_CPI				//表示为硬件支持CPI功能


#define GSM_WAVECOM			//表示为采用 WAVECOM 的硬件


#ifdef GSM_WAVECOM
#undef GSM_CPI
#endif


//------------------------------------------
//GSM error
//------------------------------------------

#define	GERR_OK							0	//成功

#define	GERR_ME_MIN						3
#define	GERR_ME_MAX						47
#define	GERR_BASENEW					2000

#define	GERR_PENDING					(GERR_BASENEW+  0)	//GSM模块返回，在异步开始操作
#define	GERR_NO_INIT					(GERR_BASENEW+  1)	//GSM模块没有初始化，或者已经退出
#define	GERR_PARAM						(GERR_BASENEW+  2)	//参数检查失败
#define	GERR_DEV_NOT_READY				(GERR_BASENEW+  3)	//还不能确认设备准备好
#define	GERR_CANCEL						(GERR_BASENEW+  4)	//
#define	GERR_FAIL						(GERR_BASENEW+  5)	//操作失败，不知原因
#define	GERR_TRY_AGAIN					(GERR_BASENEW+  6)	//可能操作正忙，请重试
#define	GERR_NOFOUND					(GERR_BASENEW+  7)	//操作失败，没有找到AT命令的正确返回

#define	GERR_SYSFAIL					(GERR_BASENEW+  10)	//创建线程、分配内存、等失败
#define	GERR_OPENPORT					(GERR_BASENEW+  11)	//打开端口失败
#define	GERR_CFGPORT					(GERR_BASENEW+  12)	//配置端口失败

#define	GERR_SIM_UNREADY				(GERR_BASENEW+  13)	//SIM卡状态，没有READY，也可能没有INS
#define	GERR_IS_DIALING					(GERR_BASENEW+  14)	//正在拨号，请稍后重试
#define	GERR_NO_PB_SMS					(GERR_BASENEW+  15)	//

#define	GERR_NOANSWER					(GERR_BASENEW+ 21)	//拨号没有应答
#define	GERR_NODIALTONE					(GERR_BASENEW+ 22)	//拨号没有拨号音
#define	GERR_NOCARRIER					(GERR_BASENEW+ 23)	//拨号没有载波信号
#define	GERR_BUSY						(GERR_BASENEW+ 24)	//拨号忙

#define GERR_CALL_HAS_DISCONNECTED      (GERR_BASENEW + 25)	//通话已挂断

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
#define GERR_STK_NOT_INIT				48 // STK服务还未初始化
#define GERR_STK_ILLEGAL_SESSION_STEP	49 // 不合法的STK会话步骤。
#define GERR_STK_NOT_SUPPORT			50 //  模块或SIM卡不支持STK服务

#define	GERR_UNKNOWN					100// 未知错误

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
#define	PHM_NEWCBM				6 //暂时不支持
				
#define	PHM_MAXCNT				10 //请保留n个消息给GSM模块使用

//PHM_STATE's wParam
#define	PHS_FATAL				1 //GSM-Phone 出现严重错误
#define	PHS_PINS				2 //GSM-Phone PIN的状态
#define	PHS_READSIM				3 //GSM-Phone 可以开始读PB SMS

#define	PHS_SQ					5 //GSM-Phone 提示 信号强度---lParam is 强度数值
#define	PHS_GSMNET				6 //GSM-Phone 提示GSM 网络状态
#define	PHS_GPRSNET				7 //GSM-Phone 提示GPRS网络状态

#define	PHS_BATTERY				10//GSM-Phone 提示 Battery 状态

//当收到STK的Proactive command时向用户程序发送WM_COMMAND消息,HIWORD(wParam)包含如下命令,LPARAM为NULL
#define STK_SETUPMENU_SEL		0 //会话开始，提示用户得到开始菜单
#define STK_DISP_TEXT			1 //要求是否显示一段文本
#define STK_GET_INKEY			2 //得到用户按键
#define STK_GET_INPUT			3 //要求得到用户输入的一段文本
#define STK_SETUP_CALL			4 //开始打电话
#define STK_PLAY_TONE			5 //播放提示音频
#define STK_SEL_ITEM			6 //选择菜单
#define STK_SEND_SMS			9 //STK发送了一条短信。
#define STK_SESSION_END			99 //会话结束

//用户可以调用BOOL Gsm_StkMakeResponse(IN WORD wResponseCmdType, PVOID pData)
//用如下应答命令应答
#define STK_RES_SETUPMENU_SEL		0 //选择开始菜单的菜单项，pData=菜单项标识号
#define STK_RES_CONFIRM_DISPTEXT	1 //确定显示文本,pData为NULL，响应STK_DISP_TEXT
#define STK_RES_GET_INKEY			4 //将用户的按键信息发送给STK的形式响应STK_GET_INKEY，pData为按键的ascii码
#define STK_RES_GET_INPUT			6 //将用户的输入信息发送给STK的形式响应STK_GET_INPUT，pData为输入信息的文本缓冲地址。
#define STK_RES_SEL_ITEM			7 //选择菜单项用来响应STK_SEL_ITEM，pData为菜单项标识号
#define STK_RES_CONFIRM_CALL		8 //确认开始打电话，响应STK_SETUP_CALL，pData为NULL
#define STK_RES_CANCEL_CALL			9 //取消打电话，响应STK_SETUP_CALL，pData为NULL
#define STK_RES_END_SESSION			99 //用户要求结束会话.
#define STK_RES_MOVEBACK			95 //菜单回退到上一级.

#define STK_SETUP_CALL_NOT_BUSY		0
#define STK_SETUP_CALL_HELD_ALL		1
#define STK_SETUP_CALL_DROP_ALL		2

#define STK_DISP_TEXT_NEED_CONFIRM	1
#define STK_DISP_TEXT_NEED_NOT_CONFIRM 0

#define STK_INPUT_DIGIT				0 //得到输入时用户仅能输入数字(0-9,*,+,#)
#define STK_INPUT_TEXT				1 //得到输入时为文本。

//------------------------------------------
//GSM AT Information
//------------------------------------------

//PIN的状态 ( PHS_PINS's lParam, dwPinState )
#define	PINS_READY				0
#define	PINS_NOT_INS			1
#define	PINS_SIM_PIN			4
#define	PINS_SIM_PUK			5
#define	PINS_SIM_PIN2			6
#define	PINS_SIM_PUK2			7
#define	PINS_PH_SIM_PIN			8
#define	PINS_PH_NET_PIN			9

#define	PINS_UNKNOWN			11


//网络状态 ( PHS_GSMNET's lParam, PHS_GPRSNET's lParam, bNetStatus )
#define	NETS_UNREGISTERED		0x30 //GSM网络 没有注册
#define	NETS_REGHOME			0x31 //GSM网络 注册到本地
#define	NETS_SERACHING			0x32 //GSM网络 正在查找
#define	NETS_DENIED				0x33 //GSM网络 被拒绝注册
#define	NETS_UNKNOWN			0x34 //GSM网络 不知道状态
#define	NETS_REGROAM			0x35 //GSM网络 注册到漫游

#define	NETS_WAITTING			0x00 //软件正等待 GSM网络返回信息


// ( PHS_BATTERY's lParam, bBatStatus )
#define	BATS_REACH_MIN			0
#define	BATS_REACH_MAX			1


//音量级别默认数值
#define	GSMVOL_MIN				1
#define	GSMVOL_MAX				5


//GSM功能类 ( bGsmClassx )
#define	GSMCLSX_VOICE			0x01
#define	GSMCLSX_DATA			0x02
#define	GSMCLSX_FAX				0x04
#define	GSMCLSX_SMS				0x08

#define	GSMCLSX_ALL				(GSMCLSX_VOICE |GSMCLSX_DATA |GSMCLSX_FAX)
#define	GSMCLSX_USEDEF			0


//呼叫过程 ( PHM_CALLPRC's bCallMsgType )
#define	CALLPRC_SYNC			4
#define	CALLPRC_ALERT			2
#define	CALLPRC_DESC			5
#define	CALLPRC_CONNECT			6
#define	CALLPRC_DISCONNECT		1
#define CALLPRC_OP_HOLD 7 //对方保持通话
#define CALLPRC_OP_RECOVER 8//对方恢复通话
#define CALLPRC_UCF_ACTIVE		9 //对方设置为无条件呼叫转移
#define CALLPRC_CF_ACTIVE		10 //对方设置为有条件转移
#define CALLPRC_FORWRDING		11 //正在呼叫转移

#define	CALLPRC_STATUS			0xFF

#define	CALLCID_UNKNOWN			0xFF
#define	CALLCID_MAKE			0xFE

//呼叫类型 ( PHM_CALLIN's bRingType )
#define	CALLTYPE_VOICE			1
#define	CALLTYPE_FAX			2
#define	CALLTYPE_ASYNC			3
#define	CALLTYPE_SYNC			4


//呼叫转移理由 (bCFwReason)
#define	CFW_R_UNCONDITIONAL		0
#define	CFW_R_BUSY				1
#define	CFW_R_NO_REPLY			2
#define	CFW_R_NOT_REACHABLE		3
#define	CFW_R_ALL_CALL			4
#define	CFW_R_ALL_CONDITIONAL	5

//成批查找时的入口类型
#define	SMS_ALLENTRY_TYPE				1
#define SMS_RECVUNREADENTRY_TYPE		2
#define PB_ALLENTRY_TYPE				5

//电话号码类型 (bNumType)
#define	NUMTYPE_UNKNOWN			0x81 //129
#define	NUMTYPE_INTER			0x91 //145
#define	NUMTYPE_NATIONAL		0xA1 //161
#define	NUMTYPE_NETWORK			0xB1 //177


//电话本选择 (dwPbStor)
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


//指定设备信息 (dwMeInfoID)
#define	MEINFO_MI				0
#define	MEINFO_MM				1
#define	MEINFO_MR				2
#define	MEINFO_SN				3
#define	MEINFO_IMEI				4


//删除标志 (dwDelFlag)
#define		SMSD_INDEX			0 // specified in <index>.
#define		SMSD_READ			1 // read.
#define		SMSD_READ_SENT		2 // read , sent out
#define		SMSD_NOT_UNREAD		3 // read , sent out , stored but not sent
#define		SMSD_ALL			4 // 


//---------------------------------------------------------------------------
//GSM struct
//---------------------------------------------------------------------------

//电话本 读写						//Write--------Read
//typedef	struct	_PB_ENTRY
//{
//	DWORD			dwIndex;		//[IN]         [IN]
//
//	//BYTE			bNumType;		//[no use]     [OUT]
//	LPSTR			pszNumber;		//[IN]         [OUT]
//	DWORD			dwCntNum;		//[IN]         [IN/OUT]
//
//	LPGSMSTR		pBufName;		//[IN]         [OUT] //根据系统，相应为 char or wchar
//	DWORD			dwCntName;		//[IN]         [IN]  //the count of char or wchar
//
//} PB_ENTRY, *LPPB_ENTRY;


//短信 读
typedef	struct	_SMS_ENTRY
{
	//索引
	DWORD			dwIndex;		//[IN]

	//是否解码成功
	BYTE			bDecOk;			//[OUT]

	//处理类型 或 短信数据类型
	BYTE			bDataType;		//[OUT]

	//存储状态
	BYTE			bSmsState;		//[OUT]

	//发送者
	BYTE			bNumType;		//[OUT]
	LPSTR			pszNumber;		//[OUT] // xyg add: pszNumber[0]==0 表示没有
	DWORD			dwCntNum;		//[IN]
	LPSTR			pszNumPb;		//[OUT] // xyg add: pszNumber[0]==0 表示没有
	DWORD			dwCntNumPb;		//[IN]
	//发送时间
	SYSTEMTIME		stSysTime;		//[OUT] // xyg add: 当stSysTime.wMonth==(WORD)-1 表示没有

	//发送内容
	LPGSMSTR		pBufText;		//[OUT]
	DWORD			dwCntText;		//[IN]  //the count of char or wchar
	//DWORD			dwCntTextGet;	//[OUT] //the count of char or wchar

	//
	//以下是 xyg 添加的，为了方便处理各种类型的短信
	//
	//处理结果 （如果是特殊的短信，xyg会做相应处理，并且返回是否成功）
	BOOL			fIsHandled;		//[OUT]

	//如果是分割的通常类型短信( bDataType==SMST_EMSCAT )时，使用如下成员
	BYTE			bNumRef;		//[OUT]
	BYTE			bNumMax;		//[OUT]
	BYTE			bNumCur;		//[OUT]

} SMS_ENTRY, *LPSMS_ENTRY;

//this struct added by tanphy for phone book batched reading
typedef struct _PB_ENTRY{
	DWORD dwSize;
	LPSTR pszPhoneNumber;//[IN] 存放电话号码的buffer的首地址
	LPSTR pszName;//[IN] 存放姓名的buffer的首地址
	
	WORD  wPhoneLen;//[IN] 存放电话号码的buffer的大小
	WORD  wNameLen;//[IN] 存放姓名的buffer的大小

	WORD  wIndex;//[OUT]  用GsmPb_GetFirstEntry或GsmPb_GetNextEntry得到一个联系时，返回该联系在电话本中相应的索引号
	WORD  wTotalEntries;//[IN] 调用GsmPb_GetFirstEntry时应传入的电话本的入口总数。
}PB_ENTRY, *PPBENTRY;

typedef struct _STK_CMDINFO{
	DWORD dwSize;
	WORD wCmdType;
	WORD wReserve;
	union _CMD_PARAM{
		struct _MENUSEL_PARAM{
			LPSTR pszMenuInfoBuf;//形为"菜单标题\0\0菜单项标识符1\0\0菜单项1\0\0菜单项标识符2\0\0菜单项2\0\0...菜单项标识符n\0\0菜单项n\0\0\0
			WORD wBufLen;//菜单信息长度
			WORD wItemNum;//菜单项个数
		}stMenuSelParam;
		struct _DISPTEXT_PARAM{
			LPSTR pszPromptInfoBuf;//提示文本缓冲地址
			WORD wBufLen;//提示文本的长度
			WORD wNeedConfirm;//返回STK_DISP_TEXT_NEED_CONFIRM表示要用户以STK_RES_CONFIRM_DISPTEXT应答才能得到下一个命令;
							//返回STK_DISP_TEXT_NEED_NOT_CONFIRM表示用户不需应答，只需等待下一个命令。
		}stDispTextParam;
		
		struct _PLAYTONE_PARAM{
			WORD wToneType;//音频类型,用户根据该类型播放手机中对应类型的音频。
			WORD wTonePlayLong;//音频的长度（秒）
			LPSTR pszTextBuf;//文本信息
			WORD  wTextBufLen;//文本长度
			WORD wReserve;//保留
		}stPlayToneParam;
		
		struct _GETINKEY_PARAM{
			LPSTR pszPromptInfoBuf;//得到输入按键时的提示信息
			WORD wBufLen;//提示信息的长度
			WORD wInkeyFormat;//按键内容的格式STK_INPUT_DIGIT，STK_INPUT_TEXT
		}stGetInKeyParam;

		struct _GETINPUT_PARAM{
			LPSTR pszPromptInfoBuf;//提示文本缓冲地址
			WORD wBufLen;//提示文本的长度
			WORD wInputFormat;//输入内容的格式STK_INPUT_DIGIT,STK_INPUT_TEXT
			WORD wMaxInput;//最多输入
			WORD wMinInput;//最少输入
		}stGetInputParam;
		
		struct _SETUPCALL_PARAM{
			LPSTR pszCalledNbBuf;//被叫方主电话号码
			LPSTR pszCalledSubNbBuf;//被叫方子电话号码
			WORD wNbBufLen;//主电话号码长度
			WORD wSubNbBufLen;//子电话号码长度
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
	BYTE bTprp:1; //回复路径
	BYTE bTpsrp:1;//状态报告
	BYTE bTpid:6;//信息格式
	BYTE bTpvp;//有效期限
	BYTE bNumType_SCA;//短信中心号码类型
	BYTE szSCA[MAX_PHONE_LEN + 1];//短信中心号码
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
#define		SMSS_WAITTING		0xFF// xyg add: 表示不是正确的数值

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
//注册、启动
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
//设置和得到短消息的接收方式。
#define REC_SMS_TEMPSAVETO_RAM 1 //当mobile设置为该方式时收到的短消息暂存在RAM,AP在收到通知时要及时去读取并转储，否则掉电会丢失。
#define REC_SMS_SAVETO_SIMCARD 2 //当mobile设置为该方式时收到的短消息保存在SIM卡。

extern	DWORD	GsmSms_SetRecMode(DWORD dwRecMode);//设置短消息的接收方式。返回GERR_OK表示成功，否则表示失败
extern	DWORD	GsmSms_GetRecMode(DWORD *pdwRecMode);//得到短消息的接收方式返回GERR_OK表示成功，否则表示失败

extern	DWORD	GsmSms_GetUnchange( OUT BOOL* pfIsUnchange ); // WaveCom
extern	DWORD	GsmSms_SetUnchange( BOOL fIsUnchange ); // WaveCom
extern	DWORD	GsmSms_ChangeState( DWORD dwIndex, BYTE bSmsState ); // WaveCom

//------------------------------------------
//Call
//------------------------------------------
//电话状态
#define CALL_ACTIVE 0 //激活，说明正在通话
#define CALL_HELD 1 //保持
#define CALL_DIALING 2 //正在拔号
#define CALL_ALERTING 3//发信号
#define CALL_INCOMING 4//打入
#define CALL_WAITING 5//等待
#define CALL_IDLE 6//空闲，说明该电话已经挂断或释放了。

//多方通话时应答方式
#define CALL_ANSWER_NORMAL 0 //当前还没有通话时可用该方式应答
#define CALL_ANSWER_HOLD_CUR 1//保持当前通话，接通呼叫等待
#define CALL_ANSWER_REPL_CUR 2//将呼叫等待的电话替换当前通话

extern	DWORD	GsmCall_Make( LPSTR pszNumber,OUT PBYTE pbCallID);
extern	DWORD	GsmCall_Hangup(BYTE bCallID, BOOL fHangAll);//挂断,fHangAll为TRUE表示挂断所有
extern	DWORD	GsmCall_Answer(BYTE bCallID, DWORD dwAnserMode);//接听,dwAnserMode为CALL_ANSWER_NORMAL，CALL_ANSWER_HOLD_CUR,CALL_ANSWER_REPL_CUR之一
extern	DWORD	GsmCall_SendTone(char chTone);
extern	DWORD   GsmCall_Release(BYTE bCallID);//释放
extern  DWORD	GsmCall_Recover(BYTE bCallID);//恢复
extern  DWORD	GsmCall_Hold(BYTE bCallID);//保持
extern	DWORD	GsmCall_ReadNumIn( OUT LPSTR pszNumIn, DWORD dwCntNum, OUT LPSTR pszNumPbIn, DWORD dwCntNumPb, BYTE bCallID );
extern	DWORD	GsmCall_Reject( BYTE bCallID );//拒绝
extern	DWORD	GsmCall_Switch();//线路切换
extern	DWORD	GsmCall_GetState(BYTE bCallID, PBYTE *pBstate);//得到电话状态,保持，激活，等待等.

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

