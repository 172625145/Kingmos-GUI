/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _KINGMOS_RAS_H_
#define _KINGMOS_RAS_H_

#ifdef __cplusplus
extern "C" {
#endif


// ------------------------------------------------------
// RAS �궨��
// ------------------------------------------------------

// RAS����ģʽ
#define RAS_MaxDialMethod			(60)

#define RAS_MaxDM_Dial				(4)
#define RAS_MaxDM_Str				(24)

// RAS�����豸
#define RAS_MaxDrvReg				(280)
#define RAS_MaxDeviceName			(200)

// RAS������Ŀ
#define RAS_MaxEntryName			(100)
#define	RAS_MaxAreaCode				(24)

#define RAS_MaxInfoDial				(64)

// RAS���Ų���
#define RAS_MaxPhoneNumber			(128)
#define MAX_USERLEN					60
#define MAX_PWLEN					60
#define MAX_DNLEN					60


// RAS����ģʽ---
#define	DIALM_TONE					'T'//
#define	DIALM_PULSE					'P'//

// RAS����ģʽ--- szLocal / szArea / szCountry
#define	DIALNUM_E					'E'//���ʺ���
#define	DIALNUM_F					'F'//��;����
#define	DIALNUM_G					'G'//���غ���


// RAS����ģʽ---dwOptMethod
#define	RASM_OPT_DIAL				0x01
#define	RASM_OPT_LOCAL				0x02
#define	RASM_OPT_AREA				0x04
#define	RASM_OPT_COUNTRY			0x08
#define	RASM_OPT_ALL				(RASM_OPT_DIAL | RASM_OPT_LOCAL | RASM_OPT_AREA | RASM_OPT_COUNTRY)


// RAS�����豸---dwDevType
#define	RASDT_Direct				0x0100
#define	RASDT_Modem					0x0200
#define	RASDT_Isdn					0x0300
#define	RASDT_X25					0x0400
#define	RASDT_Vpn					0x0500
#define	RASDT_PPPoE					0x0600

#define	RASDT_SUB_GPRS				0x0001

#define	RASDT_GETMAIN(dwDevType)	(dwDevType & 0xFF00)
#define	RASDT_GETSUB(dwDevType)		(dwDevType & 0x00FF)

// RAS�����豸---dwChkFlag
#define	RASDT_CHKMAIN				1
#define	RASDT_CHKSUB				2
#define	RASDT_CHKTYPE				(RASDT_CHKMAIN | RASDT_CHKSUB)
#define	RASDT_CHKLINK				4


// RAS�����豸---dwDevStatus
#define	RASDEV_DISCNN				0x0002	//�豸���ڱ� �Ͽ������ͷţ���Ϊ���߻����û�ȡ������
#define	RASDEV_IDLE					0x0100	//�豸 ��������DIAL
#define	RASDEV_LOAD					0x0200
#define	RASDEV_DIALING				0x0500
#define	RASDEV_DIALED				0x0600
#define	RASDEV_WAITCNN				0x0700
#define	RASDEV_LINK					0x0A00	//�Ѿ����ӳɹ�


// RAS������Ŀ---dwRasOpt
#define	RASO_DEV_SPEAKER			0x00000001	//����������

#define RASO_PHONE_MASK				0x00000030	//
#define RASO_PHONE_LOCAL			0x00000000	//���ص绰
#define RASO_PHONE_AREA				0x00000010	//���ڳ�;
#define RASO_PHONE_COUNTRY			0x00000020	//���ʳ�;

#define	RASO_NET_IPADDR				0x00000100
#define	RASO_NET_DNSADDR			0x00000200
#define	RASO_NET_IPHDR_COMP			0x00000400	//Ip Header Compression
#define	RASO_NET_PAP				0x00001000
#define RASO_NET_CHAP				0x00002000
#define RASO_NET_EAP				0x00004000


// RAS��������---dwNotifierType
#define	DIALNOTIFY_NULL				0x0
#define	DIALNOTIFY_WND				0xFFFFFFFF

// RAS��������---���Ź���֪ͨ����Ϣ: ״̬������
#define	WM_RASDIALEVENT				(WM_NET_FIRST+0)
//dwRasState = (RASSTATE) wParam; 
//dwRasErr   = (DWORD) lParam; 

// RAS��������---���Ź���֪ͨ����Ϣ---״̬
enum
{
	//�豸��TSP�׶�
	RASCS_LoadDialParam = 0,		//TAPI--->RAS: ���ڼ����豸...
	RASCS_LoadDev,			//TAPI--->RAS: ���ڼ����豸...
	RASCS_OpenPort,			//TAPI--->RAS: ���ڴ��豸...
	RASCS_PortOpened,		//�豸�򿪳ɹ���
	RASCS_ModemReady,		//modem����׼��...
	RASCS_ConnectDevice,	//TAPI--->RAS: �豸���ڲ���...
	RASCS_DeviceConnected,	//TAPI--->RAS: �豸���ųɹ���

	//PPP�����
	RASCS_PPP,				//PPP--->RAS: ���ڽ�����������...
	RASCS_LCP_OK,			//PPP--->RAS: �������óɹ���

	RASCS_Authenticate,		//PPP--->RAS: ������֤�û�������...
	//RASCS_AuthNotify,		//PPP--->RAS: ������Ϣ...
	//RASCS_AuthRetry, 
	//RASCS_AuthCallback, 
	//RASCS_AuthChangePassword, 
	//RASCS_AuthProject, 
	//RASCS_PasswordExpired, 
	RASCS_AuthAck,			//PPP--->RAS: ��֤�û�������ɹ���

	RASCS_IPCP,				//PPP--->RAS: ����������ע������...
	RASCS_IPCP_OK,			//PPP--->RAS: ������ע�������ɹ���
	
	RASCS_Connected,		//PPP--->RAS: ���ӳɹ���
	RASCS_Disconnected,		//PPP--->RAS: ���ӶϿ�

	//RASEVT_TODEV = RASEVT_START,

} RASSTATE;


// RAS��������---���Ź���֪ͨ����Ϣ---����
#define	RASERR_SUCCESS			0	//
#define	RASERR_NOPARAM			1	//û�в��Ų��������߲��Ų����д���
#define	RASERR_BREAK			2	//�豸���Ų��������
#define	RASERR_NOSIGNAL			3	//�豸û���ź�
#define	RASERR_TIMEOUT			4	//�豸���ų�ʱ
#define	RASERR_NOTIDLE			5	//�豸���Ǵ��ڿ���״̬
//RASCS_LoadDev: in RasDial
#define	RASERR_NOENTRY			10	//�ڼ�鲦�����ӵ�����ʱ����
#define	RASERR_NODEVICE			11	//û���ҵ����豸
#define	RASERR_NOTSP			13	//����TSPʱʧ��
#define	RASERR_NOTSPDEV			14	//����TSP�豸ʱʧ��
#define	RASERR_NOTSPOPEN		15	//��TSP�豸ʱʧ��
//RASCS_OpenPort: in TSPI_OpenDev
#define	RASERR_TSP_OPEN			20	//���豸ʱʧ��
#define	RASERR_TSP_CFG			21	//�����豸ʱʧ��
#define	RASERR_TSP_ENTRY		22	//��ȡ������Ŀʱʧ��
#define	RASERR_TSP_START		23	//����MODEMʱʧ��
#define	RASERR_TSP_TESTMDM		24	//����MODEMʱʧ��
#define	RASERR_TSP_SETMDM		25	//����MODEMʱʧ��
//RASCS_ConnectDevice: in TSPI_lineDial
#define	RASERR_TSP_INIT			31	//�����豸����ʱʧ��
#define	RASERR_TSP_DIAL			32	//�豸����ʧ��
#define	RASERR_TSP_DIAL_BUSY	33	//�豸����ʧ�ܣ���Ϊռ��
#define	RASERR_TSP_DIAL_NOCAR	34	//�豸����ʧ�ܣ���Ϊû���ز��ź�
//RASCS_DeviceConnected: in RasDial
#define	RASERR_TSP_NOINFO		41	//��TSP DLL�л�ȡ�����Ϣʱʧ��

//RASCS_PPP
#define	RASERR_PPP_DEV			50	//�ڲ�����·������豸ʱʧ��
#define	RASERR_PPP_CFG			51	//�����������ʧ��
//RASCS_Authenticate
#define	RASERR_PPP_PASSWORD		60	//������û��������
#define	RASERR_PPP_TICKOFF		61	//���Է���ֹ���Ͽ�



// ------------------------------------------------------
// RAS IP ��ַ
// ------------------------------------------------------
typedef	struct	_RASIPADDR
{
	BYTE	a;
	BYTE	b;
	BYTE	c;
	BYTE	d;

} RASIPADDR;


// ------------------------------------------------------
// RAS����ģʽ
// ------------------------------------------------------

typedef	struct	_DIALM_NAME
{
	TCHAR       szDialName[ RAS_MaxDialMethod + 4 ];
} DIALM_NAME, *LPDIALM_NAME;

typedef	struct	_DIAL_METHOD
{
	TCHAR		szDial[ RAS_MaxDM_Dial + 4 ];  //ָ������T����P

	TCHAR		szLocal[ RAS_MaxDM_Str + 4 ];  //ѡ�� RASO_PHONE_LOCAL ʱ�� �绰��ʽ: "9,G"
	TCHAR		szArea[ RAS_MaxDM_Str + 4 ];   //ѡ�� RASO_PHONE_AREA ʱ�� �绰��ʽ: "9,FG"
	TCHAR		szCountry[ RAS_MaxDM_Str + 4 ];//ѡ�� RASO_PHONE_COUNTRY ʱ�� �绰��ʽ: "9,EFG"

} DIAL_METHOD, *LPDIAL_METHOD;


// ------------------------------------------------------
// RAS�����豸��Ϣ
// ------------------------------------------------------

typedef	struct	_RASDEVINFO
{
	DWORD		dwDevType;
	DWORD		dwDevStatus;
	TCHAR		szDrvReg[ RAS_MaxDrvReg + 4 ];
	TCHAR       szDeviceName[RAS_MaxDeviceName+8];

} RASDEVINFO;

// ------------------------------------------------------
// RAS������Ŀ
// ------------------------------------------------------

typedef	struct	_RASENTRYNAME
{
	TCHAR       szEntryName[ RAS_MaxEntryName + 4 ];
} RASENTRYNAME;

typedef	struct	_RASENTRY
{
	//������ݺʹ洢
	WORD		wVersion;
	WORD		wSize;

	//ѡ����Ϣ
	DWORD		dwRasOpt;

	//�豸��Ϣ
	TCHAR		szDrvReg[ RAS_MaxDrvReg + 4 ];

	//---�õ����ź�����Ϣ
	char		szLocalNumber[ RAS_MaxPhoneNumber + 4 ]; //��������
	char		szCountryCode[ RAS_MaxAreaCode + 4 ];    //��������
	char		szAreaCode[ RAS_MaxAreaCode + 4 ];       //��ʡ����
	char		szDialName[ RAS_MaxDialMethod + 4 ];

	//���Ų�����Ϣ���ز�����
	DWORD		dwCntRedial;

	//������Ϣ
	RASIPADDR	ipaddr;
	RASIPADDR	ipaddrDns;
	RASIPADDR	ipaddrDnsAlt;
	RASIPADDR	ipaddrAC;	//xyg: for PPPoE
	DWORD		dwNetProtocols;

	//ÿ���������ӵ� ������Ϣ
	BYTE		bInfoDial[ RAS_MaxInfoDial + 4 ];	//���籣�� PPPOE�� Service-Name

} RASENTRY;


// ------------------------------------------------------
// RAS���Ų���
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
// RAS����������Ϣ
// ------------------------------------------------------

typedef	struct	_RASCNNINFO
{
	RASDEVINFO		rasDevInfo;
	TCHAR			szEntryName[ RAS_MaxEntryName + 4 ];

} RASCNNINFO;


// ------------------------------------------------------
// RAS��������ͳ��
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
// RAS �ṩ�� API
// ------------------------------------------------------

//
//���� ��ʵ��
//

// RAS����ģʽ
extern	BOOL	WINAPI	RasMethod_Enum( OUT OPTIONAL LPDIALM_NAME lpDialNames, IN OUT DWORD* pdwCnt );
extern	BOOL	WINAPI	RasMethod_Get( LPCTSTR szDialName, DWORD dwOptMethod, OUT DIAL_METHOD* lpDialM );
extern	BOOL	WINAPI	RasMethod_GenNum( RASENTRY* lpEntry, LPCSTR pszNumLocal, OUT LPSTR pszNumNew, DWORD dwLen );


// RAS������Ŀ
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


// RAS�����豸
extern	void	WINAPI	RasDev_Enum( OUT RASDEVINFO* lpRasDevinfo, IN OUT DWORD* lpdwCntDevices );
extern	BOOL	WINAPI	RasDev_GetDrvDial( LPCTSTR pszDrvReg, OUT DWORD* pdwDevType, OUT DWORD* pdwMTU, OUT LPTSTR pszTsp, OUT LPTSTR pszDeviceName );
extern	BOOL	WINAPI	RasDev_Check( DWORD dwChkFlag, DWORD dwChkDevType );


// RAS��������
extern	DWORD	WINAPI	RasLink_Dial( LPCTSTR lpszPhonebook, DIALPARAM* pDialParam, DWORD dwNotifierType, LPVOID notifier, OUT HANDLE* phRasConn );
extern	DWORD	WINAPI	RasLink_HangUp( HANDLE hRasConn );
extern	DWORD	WINAPI	RasLink_Enum( HANDLE* phRasConn, IN OUT LPDWORD lpdwCntRasConn );

extern	DWORD	WINAPI	RasLink_GetInfo( HANDLE hRasConn, OUT RASCNNINFO* lpRasConnInfo );
extern	DWORD	WINAPI	RasLink_GetStatus( HANDLE hRasConn, OUT DWORD* pdwDevStatus );

extern	DWORD	WINAPI	RasLink_EnableStat( HANDLE hRasConn, BOOL fEnbaleBps );
extern	DWORD	WINAPI	RasLink_GetStat( HANDLE hRasConn, OUT RAS_STATS *lpStatistics );


// RAS���Ÿ�������
extern	DWORD	WINAPI	RasFun_GetStateStr( DWORD dwRasState, LPTSTR pszStateStr, DWORD dwBufSize );
extern	DWORD	WINAPI	RasFun_GetErrStr( DWORD dwRasErr, LPTSTR pszErrStr, DWORD dwBufSize );


// RAS����modem
extern	BOOL	WINAPI	RasMdm_RegModem( LPCTSTR pszActiveReg );
extern	void	WINAPI	RasMdm_UnregModem( LPCTSTR pszActiveReg );
extern	BOOL	WINAPI	RasMdm_RegPPPoEd( LPCTSTR pszDrvReg, HANDLE hDev );
extern	void	WINAPI	RasMdm_UnregPPPoEd( LPCTSTR pszDrvReg );



//
//���� �ĺ����
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
