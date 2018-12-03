/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _KINGMOS_RAS_H_
#define _KINGMOS_RAS_H_

#ifdef __cplusplus
extern "C" {
#endif


// ------------------------------------------------------
// RAS 宏定义
// ------------------------------------------------------

// RAS拨号模式
#define RAS_MaxDialMethod			(60)

#define RAS_MaxDM_Dial				(4)
#define RAS_MaxDM_Str				(24)

// RAS拨号设备
#define RAS_MaxDrvReg				(280)
#define RAS_MaxDeviceName			(200)

// RAS拨号条目
#define RAS_MaxEntryName			(100)
#define	RAS_MaxAreaCode				(24)

#define RAS_MaxInfoDial				(64)

// RAS拨号参数
#define RAS_MaxPhoneNumber			(128)
#define MAX_USERLEN					60
#define MAX_PWLEN					60
#define MAX_DNLEN					60


// RAS拨号模式---
#define	DIALM_TONE					'T'//
#define	DIALM_PULSE					'P'//

// RAS拨号模式--- szLocal / szArea / szCountry
#define	DIALNUM_E					'E'//国际号码
#define	DIALNUM_F					'F'//长途区号
#define	DIALNUM_G					'G'//本地号码


// RAS拨号模式---dwOptMethod
#define	RASM_OPT_DIAL				0x01
#define	RASM_OPT_LOCAL				0x02
#define	RASM_OPT_AREA				0x04
#define	RASM_OPT_COUNTRY			0x08
#define	RASM_OPT_ALL				(RASM_OPT_DIAL | RASM_OPT_LOCAL | RASM_OPT_AREA | RASM_OPT_COUNTRY)


// RAS拨号设备---dwDevType
#define	RASDT_Direct				0x0100
#define	RASDT_Modem					0x0200
#define	RASDT_Isdn					0x0300
#define	RASDT_X25					0x0400
#define	RASDT_Vpn					0x0500
#define	RASDT_PPPoE					0x0600

#define	RASDT_SUB_GPRS				0x0001

#define	RASDT_GETMAIN(dwDevType)	(dwDevType & 0xFF00)
#define	RASDT_GETSUB(dwDevType)		(dwDevType & 0x00FF)

// RAS拨号设备---dwChkFlag
#define	RASDT_CHKMAIN				1
#define	RASDT_CHKSUB				2
#define	RASDT_CHKTYPE				(RASDT_CHKMAIN | RASDT_CHKSUB)
#define	RASDT_CHKLINK				4


// RAS拨号设备---dwDevStatus
#define	RASDEV_DISCNN				0x0002	//设备正在被 断开或者释放，因为拔走或者用户取消拨号
#define	RASDEV_IDLE					0x0100	//设备 可以用于DIAL
#define	RASDEV_LOAD					0x0200
#define	RASDEV_DIALING				0x0500
#define	RASDEV_DIALED				0x0600
#define	RASDEV_WAITCNN				0x0700
#define	RASDEV_LINK					0x0A00	//已经连接成功


// RAS拨号条目---dwRasOpt
#define	RASO_DEV_SPEAKER			0x00000001	//启用扬声器

#define RASO_PHONE_MASK				0x00000030	//
#define RASO_PHONE_LOCAL			0x00000000	//本地电话
#define RASO_PHONE_AREA				0x00000010	//国内长途
#define RASO_PHONE_COUNTRY			0x00000020	//国际长途

#define	RASO_NET_IPADDR				0x00000100
#define	RASO_NET_DNSADDR			0x00000200
#define	RASO_NET_IPHDR_COMP			0x00000400	//Ip Header Compression
#define	RASO_NET_PAP				0x00001000
#define RASO_NET_CHAP				0x00002000
#define RASO_NET_EAP				0x00004000


// RAS拨号连接---dwNotifierType
#define	DIALNOTIFY_NULL				0x0
#define	DIALNOTIFY_WND				0xFFFFFFFF

// RAS拨号连接---拨号过程通知的消息: 状态、错误
#define	WM_RASDIALEVENT				(WM_NET_FIRST+0)
//dwRasState = (RASSTATE) wParam; 
//dwRasErr   = (DWORD) lParam; 

// RAS拨号连接---拨号过程通知的消息---状态
enum
{
	//设备和TSP阶段
	RASCS_LoadDialParam = 0,		//TAPI--->RAS: 正在加载设备...
	RASCS_LoadDev,			//TAPI--->RAS: 正在加载设备...
	RASCS_OpenPort,			//TAPI--->RAS: 正在打开设备...
	RASCS_PortOpened,		//设备打开成功！
	RASCS_ModemReady,		//modem正在准备...
	RASCS_ConnectDevice,	//TAPI--->RAS: 设备正在拨号...
	RASCS_DeviceConnected,	//TAPI--->RAS: 设备拨号成功！

	//PPP层操作
	RASCS_PPP,				//PPP--->RAS: 正在进行网络配置...
	RASCS_LCP_OK,			//PPP--->RAS: 网络配置成功！

	RASCS_Authenticate,		//PPP--->RAS: 正在验证用户和密码...
	//RASCS_AuthNotify,		//PPP--->RAS: 错误信息...
	//RASCS_AuthRetry, 
	//RASCS_AuthCallback, 
	//RASCS_AuthChangePassword, 
	//RASCS_AuthProject, 
	//RASCS_PasswordExpired, 
	RASCS_AuthAck,			//PPP--->RAS: 验证用户和密码成功！

	RASCS_IPCP,				//PPP--->RAS: 正在向网络注册计算机...
	RASCS_IPCP_OK,			//PPP--->RAS: 向网络注册计算机成功！
	
	RASCS_Connected,		//PPP--->RAS: 连接成功！
	RASCS_Disconnected,		//PPP--->RAS: 连接断开

	//RASEVT_TODEV = RASEVT_START,

} RASSTATE;


// RAS拨号连接---拨号过程通知的消息---错误
#define	RASERR_SUCCESS			0	//
#define	RASERR_NOPARAM			1	//没有拨号参数，或者拨号参数有错误
#define	RASERR_BREAK			2	//设备拨号操作被打断
#define	RASERR_NOSIGNAL			3	//设备没有信号
#define	RASERR_TIMEOUT			4	//设备拨号超时
#define	RASERR_NOTIDLE			5	//设备不是处在空闲状态
//RASCS_LoadDev: in RasDial
#define	RASERR_NOENTRY			10	//在检查拨号连接的属性时出错
#define	RASERR_NODEVICE			11	//没有找到该设备
#define	RASERR_NOTSP			13	//加载TSP时失败
#define	RASERR_NOTSPDEV			14	//加载TSP设备时失败
#define	RASERR_NOTSPOPEN		15	//绑定TSP设备时失败
//RASCS_OpenPort: in TSPI_OpenDev
#define	RASERR_TSP_OPEN			20	//打开设备时失败
#define	RASERR_TSP_CFG			21	//配置设备时失败
#define	RASERR_TSP_ENTRY		22	//获取拨号条目时失败
#define	RASERR_TSP_START		23	//启动MODEM时失败
#define	RASERR_TSP_TESTMDM		24	//测试MODEM时失败
#define	RASERR_TSP_SETMDM		25	//设置MODEM时失败
//RASCS_ConnectDevice: in TSPI_lineDial
#define	RASERR_TSP_INIT			31	//启动设备拨号时失败
#define	RASERR_TSP_DIAL			32	//设备拨号失败
#define	RASERR_TSP_DIAL_BUSY	33	//设备拨号失败，因为占线
#define	RASERR_TSP_DIAL_NOCAR	34	//设备拨号失败，因为没有载波信号
//RASCS_DeviceConnected: in RasDial
#define	RASERR_TSP_NOINFO		41	//在TSP DLL中获取相关信息时失败

//RASCS_PPP
#define	RASERR_PPP_DEV			50	//在拨号链路层添加设备时失败
#define	RASERR_PPP_CFG			51	//多次配置请求都失败
//RASCS_Authenticate
#define	RASERR_PPP_PASSWORD		60	//密码或用户密码错误
#define	RASERR_PPP_TICKOFF		61	//被对方终止而断开



// ------------------------------------------------------
// RAS IP 地址
// ------------------------------------------------------
typedef	struct	_RASIPADDR
{
	BYTE	a;
	BYTE	b;
	BYTE	c;
	BYTE	d;

} RASIPADDR;


// ------------------------------------------------------
// RAS拨号模式
// ------------------------------------------------------

typedef	struct	_DIALM_NAME
{
	TCHAR       szDialName[ RAS_MaxDialMethod + 4 ];
} DIALM_NAME, *LPDIALM_NAME;

typedef	struct	_DIAL_METHOD
{
	TCHAR		szDial[ RAS_MaxDM_Dial + 4 ];  //指定采用T或者P

	TCHAR		szLocal[ RAS_MaxDM_Str + 4 ];  //选择 RASO_PHONE_LOCAL 时的 电话方式: "9,G"
	TCHAR		szArea[ RAS_MaxDM_Str + 4 ];   //选择 RASO_PHONE_AREA 时的 电话方式: "9,FG"
	TCHAR		szCountry[ RAS_MaxDM_Str + 4 ];//选择 RASO_PHONE_COUNTRY 时的 电话方式: "9,EFG"

} DIAL_METHOD, *LPDIAL_METHOD;


// ------------------------------------------------------
// RAS拨号设备信息
// ------------------------------------------------------

typedef	struct	_RASDEVINFO
{
	DWORD		dwDevType;
	DWORD		dwDevStatus;
	TCHAR		szDrvReg[ RAS_MaxDrvReg + 4 ];
	TCHAR       szDeviceName[RAS_MaxDeviceName+8];

} RASDEVINFO;

// ------------------------------------------------------
// RAS拨号条目
// ------------------------------------------------------

typedef	struct	_RASENTRYNAME
{
	TCHAR       szEntryName[ RAS_MaxEntryName + 4 ];
} RASENTRYNAME;

typedef	struct	_RASENTRY
{
	//方便兼容和存储
	WORD		wVersion;
	WORD		wSize;

	//选项信息
	DWORD		dwRasOpt;

	//设备信息
	TCHAR		szDrvReg[ RAS_MaxDrvReg + 4 ];

	//---得到拨号号码信息
	char		szLocalNumber[ RAS_MaxPhoneNumber + 4 ]; //基本号码
	char		szCountryCode[ RAS_MaxAreaCode + 4 ];    //国家区号
	char		szAreaCode[ RAS_MaxAreaCode + 4 ];       //州省区号
	char		szDialName[ RAS_MaxDialMethod + 4 ];

	//拨号操作信息：重拨次数
	DWORD		dwCntRedial;

	//网络信息
	RASIPADDR	ipaddr;
	RASIPADDR	ipaddrDns;
	RASIPADDR	ipaddrDnsAlt;
	RASIPADDR	ipaddrAC;	//xyg: for PPPoE
	DWORD		dwNetProtocols;

	//每个拨号连接的 额外信息
	BYTE		bInfoDial[ RAS_MaxInfoDial + 4 ];	//比如保存 PPPOE的 Service-Name

} RASENTRY;


// ------------------------------------------------------
// RAS拨号参数
// ------------------------------------------------------

typedef	struct	_DIALPARAM
{
	DWORD	dwSize;

	TCHAR	szEntryName[ RAS_MaxEntryName + 4 ];

	char	szPhoneNumber[ RAS_MaxPhoneNumber + 4 ];
	char	szUserName[ MAX_USERLEN + 4 ];
	char	szPassword[ MAX_PWLEN + 4 ];
	char	szDomain[ MAX_DNLEN + 4 ];

} DIALPARAM;


// ------------------------------------------------------
// RAS拨号连接信息
// ------------------------------------------------------

typedef	struct	_RASCNNINFO
{
	RASDEVINFO		rasDevInfo;
	TCHAR			szEntryName[ RAS_MaxEntryName + 4 ];

} RASCNNINFO;


// ------------------------------------------------------
// RAS拨号连接统计
// ------------------------------------------------------

typedef struct _RAS_STATS
{
	DWORD		dwConnectDuration;
	DWORD		dwBytesXmited;
	DWORD		dwBytesRcved;
	DWORD		dwBps;
	DWORD		dwBps_Tx;

	DWORD		dwFramesXmited;
	DWORD		dwFramesRcved;
	//DWORD		dwCrcErr;

	//DWORD		dwTimeoutErr;
	//DWORD		dwAlignmentErr;
	//DWORD		dwHardwareOverrunErr;
	//DWORD		dwFramingErr;
	//DWORD		dwBufferOverrunErr;
	//DWORD		dwCompressionRatioIn;
	//DWORD		dwCompressionRatioOut;

} RAS_STATS;


// ------------------------------------------------------
// RAS 提供的 API
// ------------------------------------------------------

//
//函数 的实现
//

// RAS拨号模式
extern	BOOL	WINAPI	RasMethod_Enum( OUT OPTIONAL LPDIALM_NAME lpDialNames, IN OUT DWORD* pdwCnt );
extern	BOOL	WINAPI	RasMethod_Get( LPCTSTR szDialName, DWORD dwOptMethod, OUT DIAL_METHOD* lpDialM );
extern	BOOL	WINAPI	RasMethod_GenNum( RASENTRY* lpEntry, LPCSTR pszNumLocal, OUT LPSTR pszNumNew, DWORD dwLen );


// RAS拨号条目
extern	BOOL	WINAPI	RasEntry_Enum( LPCTSTR lpszPhonebook, OUT RASENTRYNAME* lpEntryNames, IN OUT DWORD* lpdwCntEntries );
extern	BOOL	WINAPI	RasEntry_GetDef( LPCTSTR lpszPhonebook, OUT LPTSTR pszEntryName, DWORD dwLen );
extern	BOOL	WINAPI	RasEntry_SetDef( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );
extern	BOOL	WINAPI	RasEntry_Delete( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );
extern	BOOL	WINAPI	RasEntry_Rename( LPCTSTR lpszPhonebook, LPCTSTR pszOldEntry, LPCTSTR pszNewEntry );
extern	BOOL	WINAPI	RasEntry_IsValid( LPCTSTR lpszPhonebook, LPCTSTR pszEntryName );

extern	BOOL	WINAPI	RasEntry_GetProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, OUT RASENTRY* lpEntry, IN OUT LPDWORD lpdwEntrySize );
extern	BOOL	WINAPI	RasEntry_SetProperties( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, RASENTRY* lpEntry, DWORD dwEntrySize );

extern	BOOL	WINAPI	RasEntry_GetDialParams( LPCTSTR lpszPhonebook, IN OUT DIALPARAM* lpDialParams, OUT BOOL* lpfPassword );
extern	BOOL	WINAPI	RasEntry_SetDialParams( LPCTSTR lpszPhonebook, DIALPARAM* lpDialParams, BOOL fRemovePassword );

extern	BOOL	WINAPI	RasEntry_GetDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, OUT LPVOID lpDeviceConfigOut, IN OUT DWORD* lpdwSize );
extern	BOOL	WINAPI	RasEntry_SetDevConfig( LPCTSTR lpszPhonebook, LPCTSTR szEntryName, LPCTSTR pszDrvReg, LPVOID lpDeviceConfigIn, DWORD dwSize );


// RAS拨号设备
extern	void	WINAPI	RasDev_Enum( OUT RASDEVINFO* lpRasDevinfo, IN OUT DWORD* lpdwCntDevices );
extern	BOOL	WINAPI	RasDev_GetDrvDial( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName );
extern	BOOL	WINAPI	RasDev_Check( DWORD dwChkFlag, DWORD dwChkDevType );


// RAS拨号连接
extern	DWORD	WINAPI	RasLink_Dial( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn );
extern	DWORD	WINAPI	RasLink_HangUp( HANDLE hRasConn );
extern	DWORD	WINAPI	RasLink_Enum( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn );

extern	DWORD	WINAPI	RasLink_GetInfo( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo );
extern	DWORD	WINAPI	RasLink_GetStatus( HANDLE hRasConn, OUT DWORD* pdwDevStatus );

extern	DWORD	WINAPI	RasLink_EnableStat( HANDLE hRasConn, BOOL fEnbaleBps );
extern	DWORD	WINAPI	RasLink_GetStat( HANDLE hRasConn, OUT RAS_STATS *lpStatistics );


// RAS拨号辅助操作
extern	DWORD	WINAPI	RasFun_GetStateStr( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize );
extern	DWORD	WINAPI	RasFun_GetErrStr( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize );


// RAS拨号modem
extern	BOOL	WINAPI	RasMdm_RegModem( LPCTSTR pszActiveReg );
extern	void	WINAPI	RasMdm_UnregModem( LPCTSTR pszActiveReg );
extern	BOOL	WINAPI	RasMdm_RegPPPoEd( LPCTSTR pszDrvReg, HANDLE hDev );
extern	void	WINAPI	RasMdm_UnregPPPoEd( LPCTSTR pszDrvReg );



//
//函数 的宏替代
//

#define	RasEnumDialMethod			RasMethod_Enum
#define	RasGetDialMethod			RasMethod_Get
#define	RasGenNumber				RasMethod_GenNum

#define	RasEnumEntries				RasEntry_Enum
#define	RasGetEntryDef				RasEntry_GetDef
#define	RasSetEntryDef				RasEntry_SetDef
#define	RasDeleteEntry				RasEntry_Delete
#define	RasRenameEntry				RasEntry_Rename
#define	RasValidateEntryName		RasEntry_IsValid
#define	RasGetEntryProperties		RasEntry_GetProperties
#define	RasSetEntryProperties		RasEntry_SetProperties
#define	RasGetEntryDialParams		RasEntry_GetDialParams
#define	RasSetEntryDialParams		RasEntry_SetDialParams
#define	RasGetEntryConfig			RasEntry_GetDevConfig
#define	RasSetEntryConfig			RasEntry_SetDevConfig

#define	RasEnumDevices				RasDev_Enum
#define	RasGetDrvDial				RasDev_GetDrvDial
#define	RasCheckDev					RasDev_Check

#define	RasDial						RasLink_Dial
#define	RasHangUp					RasLink_HangUp
#define	RasEnumConnections			RasLink_Enum
#define	RasGetConnectInfo			RasLink_GetInfo
#define	RasGetLinkStatus			RasLink_GetStatus
#define	RasEnableLinkStat			RasLink_EnableStat
#define	RasGetLinkStatistics		RasLink_GetStat

#define	RasGetStateString			RasFun_GetStateStr
#define	RasGetErrorString			RasFun_GetErrStr

#define	RasRegisterModem			RasMdm_RegModem
#define	RasDeregisterModem			RasMdm_UnregModem
#define	RasRegisterPPPoEd			RasMdm_RegPPPoEd
#define	RasDeregisterPPPoEd			RasMdm_UnregPPPoEd



#ifdef __cplusplus
}	
#endif

#endif	//_KINGMOS_RAS_H_
