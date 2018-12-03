/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _IPHLPAPI_H_
#define _IPHLPAPI_H_

#ifdef __cplusplus
extern "C" {
#endif


// -------------------------------------------------------------
// ͳһ�궨��
// -------------------------------------------------------------

#ifndef	ERROR_TIMEOUT
#define	ERROR_TIMEOUT					1460
#endif	
#ifndef	ERROR_FUNCTION_FAILED
#define	ERROR_FUNCTION_FAILED			1627
#endif	

typedef DWORD IPAddr;       // An IP address.
typedef DWORD IPMask;       // An IP subnet mask.

//typedef unsigned long		time_t; remove to edef.h

//
#define	MAX_HOSTNAME_LEN				128
#define	MAX_DOMAIN_NAME_LEN				128
#define	MAX_SCOPE_ID_LEN				256
#define	MAX_INTERFACE_NAME_LEN			256
#define	MAX_ADAPTER_NAME_LENGTH			256
#define	MAX_ADAPTER_DESCRIPTION_LENGTH	128

#define	MAX_ADAPTER_NAME				128

#define	MAXLEN_IFDESCR					256
#define	MAXLEN_PHYSADDR					8
#define	MAX_ADAPTER_ADDRESS_LENGTH		8

#define	ANY_SIZE						1


// -------------------------------------------------------------
// IP��ַ �Ľṹ����
// -------------------------------------------------------------

//IP��ַ��(���10���Ʊ�ʾ)
typedef	struct
{
	char	String[4 * 4];

} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

//IP��ַ���б���(���10���Ʊ�ʾ)
typedef	struct	_IP_ADDR_STRING
{
	struct	_IP_ADDR_STRING*	Next;
	
	IP_ADDRESS_STRING	IpAddress;
	IP_MASK_STRING		IpMask;
	DWORD				Context;

} IP_ADDR_STRING, *PIP_ADDR_STRING;

//IPͷ��Ϣ��TTL TOS FLAG OPTION
typedef	struct	ip_option_information
{
	UCHAR		Ttl;
	UCHAR		Tos;
	UCHAR		Flags;
	UCHAR		OptionsSize;
	PUCHAR		OptionsData;

} IP_OPTION_INFORMATION, *PIP_OPTION_INFORMATION;


// -------------------------------------------------------------
// ����Network��Ϣ
// -------------------------------------------------------------

//����Network��Ϣ
typedef	struct	_FIXED_INFO
{
	char		HostName[MAX_HOSTNAME_LEN + 4];
	char		DomainName[MAX_DOMAIN_NAME_LEN + 4];
	PIP_ADDR_STRING		CurrentDnsServer;
	IP_ADDR_STRING		DnsServerList;
	UINT		NodeType;
	char		ScopeId[MAX_SCOPE_ID_LEN + 4];
	UINT		EnableRouting;
	UINT		EnableProxy;
	UINT		EnableDns;

} FIXED_INFO, *PFIXED_INFO;

//NodeType
#define	BROADCAST_NODETYPE				1
#define	PEER_TO_PEER_NODETYPE			2
#define	MIXED_NODETYPE					4
#define	HYBRID_NODETYPE					8


// -------------------------------------------------------------
// IP�ӿ� �� IP Adapter��Ϣ �Ľṹ����
// -------------------------------------------------------------

//Adapter����Ϣ
typedef	struct	_IP_ADAPTER_INDEX_MAP
{
	DWORD		Index;
	TCHAR		szName[MAX_ADAPTER_NAME];

} IP_ADAPTER_INDEX_MAP, *PIP_ADAPTER_INDEX_MAP;

//Adapter�б�
typedef	struct	_IP_INTERFACE_INFO
{
	LONG		NumAdapters;
	IP_ADAPTER_INDEX_MAP	Adapter[1];

} IP_INTERFACE_INFO,*PIP_INTERFACE_INFO;

//ָ��IP�ӿ�Adapter����ϸ��Ϣ
typedef	struct	_IP_ADAPTER_INFO
{
	struct		_IP_ADAPTER_INFO* Next;

	DWORD		ComboIndex;
	char		AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
	char		Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
	UINT		AddressLength;
	BYTE		Address[MAX_ADAPTER_ADDRESS_LENGTH];

	DWORD		Index;
	UINT		Type;
	UINT		DhcpEnabled;

	PIP_ADDR_STRING		CurrentIpAddress;
	IP_ADDR_STRING		IpAddressList;
	IP_ADDR_STRING		GatewayList;
	IP_ADDR_STRING		DhcpServer;
	
	BOOL			HaveWins;
	IP_ADDR_STRING	PrimaryWinsServer;
	IP_ADDR_STRING	SecondaryWinsServer;

	time_t			LeaseObtained;
	time_t			LeaseExpires;

} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;


//ָ��IP�ӿڵ���ϸ��Ϣ
typedef struct	_MIB_IFROW
{
	TCHAR		szName[MAX_INTERFACE_NAME_LEN];
	DWORD		dwIndex;			//֧��

	DWORD		dwType;				//֧��
	DWORD		dwMtu;				//֧��
	DWORD		dwSpeed;
	DWORD		dwPhysAddrLen;		//֧��
	BYTE		bPhysAddr[MAXLEN_PHYSADDR];	//֧��
	DWORD		dwAdminStatus;
	DWORD		dwOperStatus;		//֧��
	DWORD		dwLastChange;
	DWORD		dwInOctets;			//֧�֣���������·��ͳ��
	DWORD		dwInUcastPkts;
	DWORD		dwInNUcastPkts;
	DWORD		dwInDiscards;
	DWORD		dwInErrors;
	DWORD		dwInUnknownProtos;
	DWORD		dwOutOctets;		//֧�֣���������·��ͳ��
	DWORD		dwOutUcastPkts;
	DWORD		dwOutNUcastPkts;
	DWORD		dwOutDiscards;
	DWORD		dwOutErrors;
	DWORD		dwOutQLen;
	DWORD		dwDescrLen;
	BYTE		bDescr[MAXLEN_IFDESCR];

} MIB_IFROW,*PMIB_IFROW;

//dwType
#define MIB_IF_TYPE_OTHER               1
#define MIB_IF_TYPE_ETHERNET            6
#define MIB_IF_TYPE_TOKENRING           9
#define MIB_IF_TYPE_FDDI                15
#define MIB_IF_TYPE_PPP                 23
#define MIB_IF_TYPE_LOOPBACK            24
#define MIB_IF_TYPE_SLIP                28
//dwAdminStatus
#define MIB_IF_ADMIN_STATUS_UP          1
#define MIB_IF_ADMIN_STATUS_DOWN        2
#define MIB_IF_ADMIN_STATUS_TESTING     3
//dwOperStatus
#define MIB_IF_OPER_STATUS_NON_OPERATIONAL      0	//LAN
#define MIB_IF_OPER_STATUS_UNREACHABLE          1	//WAN
#define MIB_IF_OPER_STATUS_DISCONNECTED         2	//WAN
#define MIB_IF_OPER_STATUS_CONNECTING           3	//WAN
#define MIB_IF_OPER_STATUS_CONNECTED            4	//WAN
#define MIB_IF_OPER_STATUS_OPERATIONAL          5	//LAN


//����IP�ӿڵ���ϸ��Ϣ
typedef	struct	_MIB_IFTABLE
{
	DWORD		dwNumEntries;
	MIB_IFROW	table[ANY_SIZE];

} MIB_IFTABLE, *PMIB_IFTABLE;


//ָ��IP�ӿ� ��Ӧ�� IP��ַ��Ϣ 
typedef struct _MIB_IPADDRROW
{
	DWORD		dwAddr;
	DWORD		dwIndex;
	DWORD		dwMask;
	DWORD		dwBCastAddr;
	DWORD		dwReasmSize;
	WORD		unused1;
	WORD		wType;

} MIB_IPADDRROW, *PMIB_IPADDRROW;


//����IP�ӿ� ��Ӧ�� IP��ַ��Ϣ
typedef struct _MIB_IPADDRTABLE
{
	DWORD			dwNumEntries;
	MIB_IPADDRROW	table[ANY_SIZE];

} MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;



// -------------------------------------------------------------
// IP���ͳ����Ϣ �Ľṹ����
// -------------------------------------------------------------


//IP���ͳ����Ϣ
typedef	struct	_MIB_IPSTATS
{
	DWORD		dwForwarding;			//֧��
	DWORD		dwDefaultTTL;			//֧��

	DWORD		dwInReceives;			//֧��
	DWORD		dwInHdrErrors;
	DWORD		dwInAddrErrors;
	DWORD		dwForwDatagrams;		//֧��
	DWORD		dwInUnknownProtos;
	DWORD		dwInDiscards;
	DWORD		dwInDelivers;

	DWORD		dwOutRequests;
	DWORD		dwRoutingDiscards;
	DWORD		dwOutDiscards;
	DWORD		dwOutNoRoutes;

	DWORD		dwReasmTimeout;
	DWORD		dwReasmReqds;
	DWORD		dwReasmOks;
	DWORD		dwReasmFails;

	DWORD		dwFragOks;
	DWORD		dwFragFails;
	DWORD		dwFragCreates;

	DWORD		dwNumIf;				//֧��
	DWORD		dwNumAddr;
	DWORD		dwNumRoutes;

} MIB_IPSTATS, *PMIB_IPSTATS;

//dwForwarding
#define	MIB_IP_FORWARDING				1
#define	MIB_IP_NOT_FORWARDING			2

// Note: These addr types have dependency on ipdef.h
#define MIB_IPADDR_PRIMARY      0x0001   // Primary ipaddr
#define MIB_IPADDR_DYNAMIC      0x0004   // Dynamic ipaddr
#define MIB_IPADDR_DISCONNECTED 0x0008   // Address is on disconnected interface
#define MIB_IPADDR_DELETED      0x0040   // Address being deleted
#define MIB_IPADDR_TRANSIENT    0x0080   // Transient address


// -------------------------------------------------------------
// IP·����Ϣ �Ľṹ����
// -------------------------------------------------------------

//IP·�������Ϣ
typedef	struct	_MIB_IPFORWARDROW
{
	DWORD		dwForwardDest;
	DWORD		dwForwardMask;
	DWORD		dwForwardPolicy;
	DWORD		dwForwardNextHop;
	DWORD		dwForwardIfIndex;
	DWORD		dwForwardType;
	DWORD		dwForwardProto;		//��֧�� ·��Э����
	DWORD		dwForwardAge;
	DWORD       dwForwardNextHopAS;
	DWORD		dwForwardMetric1;
	DWORD		dwForwardMetric2;
	DWORD		dwForwardMetric3;
	DWORD		dwForwardMetric4;
	DWORD		dwForwardMetric5;

}MIB_IPFORWARDROW, *PMIB_IPFORWARDROW;
//dwForwardType
#define	MIB_IPROUTE_TYPE_OTHER		1
#define	MIB_IPROUTE_TYPE_INVALID	2
#define	MIB_IPROUTE_TYPE_DIRECT		3
#define	MIB_IPROUTE_TYPE_INDIRECT	4
//dwForwardMetric
#define	MIB_IPROUTE_METRIC_UNUSED	(DWORD)-1


//IP·������б�
typedef	struct	_MIB_IPFORWARDTABLE
{
	DWORD				dwNumEntries;
	MIB_IPFORWARDROW	table[ANY_SIZE];

}MIB_IPFORWARDTABLE, *PMIB_IPFORWARDTABLE;





// -------------------------------------------------------------
// ARP��Ϣ �Ľṹ����
// -------------------------------------------------------------

//ARP��ַ�� ����
typedef	struct	_MIB_IPNETROW
{
	DWORD		dwIndex;
	DWORD		dwPhysAddrLen;
	BYTE		bPhysAddr[MAXLEN_PHYSADDR];
	DWORD		dwAddr;
	DWORD		dwType;
} MIB_IPNETROW, *PMIB_IPNETROW;
//dwType
#define	MIB_IPNET_TYPE_OTHER			1
#define	MIB_IPNET_TYPE_INVALID			2
#define	MIB_IPNET_TYPE_DYNAMIC			3
#define	MIB_IPNET_TYPE_STATIC			4

//ARP��ַ�� �б�
typedef	struct	_MIB_IPNETTABLE
{
    DWORD             dwNumEntries;
    MIB_IPNETROW      table[ANY_SIZE];

} MIB_IPNETTABLE, *PMIB_IPNETTABLE;


// -------------------------------------------------------------
// ICMP��Ϣ �Ľṹ����
// -------------------------------------------------------------

typedef	struct	icmp_echo_reply
{
	IPAddr		Address;
	ULONG		Status;
	ULONG		RoundTripTime;
	USHORT		DataSize;
	USHORT		Reserved;
	PVOID		Data;
	struct ip_option_information	Options;

} ICMP_ECHO_REPLY, *PICMP_ECHO_REPLY;


typedef struct _MIBICMPSTATS
{
    DWORD		dwMsgs;
    DWORD		dwErrors;
    DWORD		dwDestUnreachs;
    DWORD		dwTimeExcds;
    DWORD		dwParmProbs;
    DWORD		dwSrcQuenchs;
    DWORD		dwRedirects;
    DWORD		dwEchos;
    DWORD		dwEchoReps;
    DWORD		dwTimestamps;
    DWORD		dwTimestampReps;
    DWORD		dwAddrMasks;
    DWORD		dwAddrMaskReps;
} MIBICMPSTATS, *PMIBICMPSTATS;

typedef	struct _MIBICMPINFO
{
    MIBICMPSTATS	icmpInStats;
    MIBICMPSTATS	icmpOutStats;
} MIBICMPINFO;

typedef struct _MIB_ICMP
{
    MIBICMPINFO stats;
} MIB_ICMP,*PMIB_ICMP;

//
// ICMPv6 Type Values from RFC 2292.
//
typedef enum {
    ICMP6_DST_UNREACH          =   1,
    ICMP6_PACKET_TOO_BIG       =   2,
    ICMP6_TIME_EXCEEDED        =   3,
    ICMP6_PARAM_PROB           =   4,
    ICMP6_ECHO_REQUEST         = 128,
    ICMP6_ECHO_REPLY           = 129,
    ICMP6_MEMBERSHIP_QUERY     = 130,
    ICMP6_MEMBERSHIP_REPORT    = 131,
    ICMP6_MEMBERSHIP_REDUCTION = 132,
    ND_ROUTER_SOLICIT          = 133,
    ND_ROUTER_ADVERT           = 134,
    ND_NEIGHBOR_SOLICIT        = 135,
    ND_NEIGHBOR_ADVERT         = 136,
    ND_REDIRECT                = 137,
} ICMP6_TYPE, *PICMP6_TYPE;

//
// There are no RFC-specified defines for ICMPv4 message types, so we try to
// use the ICMP6 values from RFC 2292 modified to be prefixed with ICMP4.
//
typedef enum {
    ICMP4_ECHO_REPLY        =  0, // Echo Reply
    ICMP4_DST_UNREACH       =  3, // Destination Unreachable
    ICMP4_SOURCE_QUENCH     =  4, // Source Quench
    ICMP4_REDIRECT          =  5, // Redirect
    ICMP4_ECHO_REQUEST      =  8, // Echo Request
    ICMP4_ROUTER_ADVERT     =  9, // Router Advertisement
    ICMP4_ROUTER_SOLICIT    = 10, // Router Solicitation
    ICMP4_TIME_EXCEEDED     = 11, // Time Exceeded
    ICMP4_PARAM_PROB        = 12, // Parameter Problem
    ICMP4_TIMESTAMP_REQUEST = 13, // Timestamp Request
    ICMP4_TIMESTAMP_REPLY   = 14, // Timestamp Reply
    ICMP4_MASK_REQUEST      = 17, // Address Mask Request
    ICMP4_MASK_REPLY        = 18, // Address Mask Reply
} ICMP4_TYPE, *PICMP4_TYPE;

typedef struct _MIBICMPSTATS_EX
{
    DWORD       dwMsgs;
    DWORD       dwErrors;
    DWORD       rgdwTypeCount[256];
} MIBICMPSTATS_EX, *PMIBICMPSTATS_EX;

typedef struct _MIB_ICMP_EX
{
    MIBICMPSTATS_EX icmpInStats;
    MIBICMPSTATS_EX icmpOutStats;
} MIB_ICMP_EX,*PMIB_ICMP_EX;



// -------------------------------------------------------------
// TCP��Ϣ �Ľṹ����
// -------------------------------------------------------------

typedef struct _MIB_TCPSTATS
{
    DWORD       dwRtoAlgorithm;
    DWORD       dwRtoMin;
    DWORD       dwRtoMax;

    DWORD       dwMaxConn;
    DWORD       dwActiveOpens;
    DWORD       dwPassiveOpens;
    DWORD       dwAttemptFails;
    DWORD       dwEstabResets;
    DWORD       dwCurrEstab;
    DWORD       dwInSegs;
    DWORD       dwOutSegs;
    DWORD       dwRetransSegs;
    DWORD       dwInErrs;
    DWORD       dwOutRsts;
    DWORD       dwNumConns;
} MIB_TCPSTATS, *PMIB_TCPSTATS;
//dwRtoAlgorithm / dwRtoMin / dwRtoMax
#define MIB_TCP_RTO_OTHER       1
#define MIB_TCP_RTO_CONSTANT    2
#define MIB_TCP_RTO_RSRE        3
#define MIB_TCP_RTO_VANJ        4
//dwMaxConn
#define MIB_TCP_MAXCONN_DYNAMIC (DWORD)-1


typedef struct _MIB_TCPROW
{
    DWORD       dwState;
    DWORD       dwLocalAddr;
    DWORD       dwLocalPort;
    DWORD       dwRemoteAddr;
    DWORD       dwRemotePort;

} MIB_TCPROW, *PMIB_TCPROW;
//dwState
#define MIB_TCP_STATE_CLOSED            1
#define MIB_TCP_STATE_LISTEN            2
#define MIB_TCP_STATE_SYN_SENT          3
#define MIB_TCP_STATE_SYN_RCVD          4
#define MIB_TCP_STATE_ESTAB             5
#define MIB_TCP_STATE_FIN_WAIT1         6
#define MIB_TCP_STATE_FIN_WAIT2         7
#define MIB_TCP_STATE_CLOSE_WAIT        8
#define MIB_TCP_STATE_CLOSING           9
#define MIB_TCP_STATE_LAST_ACK         10
#define MIB_TCP_STATE_TIME_WAIT        11
#define MIB_TCP_STATE_DELETE_TCB       12


typedef struct _MIB_TCPTABLE
{
    DWORD      dwNumEntries;
    MIB_TCPROW table[ANY_SIZE];

} MIB_TCPTABLE, *PMIB_TCPTABLE;


#define MIB_USE_CURRENT_TTL         ((DWORD)-1)
#define MIB_USE_CURRENT_FORWARDING  ((DWORD)-1)




// -------------------------------------------------------------
// IPHELP���� ������
// -------------------------------------------------------------


//
// IP���ṩ�� ����
//

//
extern	DWORD	WINAPI	IPH_GetNetworkParams( OUT PFIXED_INFO pFixedInfo, IN OUT DWORD* pOutBufLen );

extern	BOOL	WINAPI	IPH_GetDnsSrvList( OUT LPBYTE pDnsList, DWORD dwLenBuf, OUT DWORD* pdwCntDnsSrv );


//IP�ӿڵĲ���
extern	DWORD	WINAPI	IPH_GetNumberOfInterfaces( OUT DWORD* pdwNumIf );

extern	DWORD	WINAPI	IPH_GetAdaptersInfo( OUT PIP_ADAPTER_INFO pAdapterInfo, IN OUT DWORD* pOutBufLen );
extern	DWORD	WINAPI	IPH_GetInterfaceInfo( OUT PIP_INTERFACE_INFO pIfTable, IN OUT DWORD* pdwOutBufLen );

extern	DWORD	WINAPI	IPH_GetIfEntry( IN OUT PMIB_IFROW pIfRow );
extern	DWORD	WINAPI	IPH_GetIfTable( OUT PMIB_IFTABLE pIfTable, IN OUT DWORD* pdwSize, BOOL bOrder );
extern	DWORD	WINAPI	IPH_SetIfEntry( PMIB_IFROW pIfRow );

extern	DWORD	WINAPI	IPH_GetIpAddrTable( OUT PMIB_IPADDRTABLE pIpAddrTable, IN OUT DWORD* pdwSize, BOOL bOrder );

extern	DWORD	WINAPI	IPH_GetBestInterface( IPAddr dwDestAddr, OUT DWORD* pdwBestIfIndex );


//
extern	DWORD	WINAPI	IPH_SetIpTTL( UINT nTTL );

extern	DWORD	WINAPI	IPH_GetIpStatistics( OUT PMIB_IPSTATS pStats );


//IP��ַ�Ĳ���
extern	DWORD	WINAPI	IPH_AddIPAddress( IPAddr Address, IPMask IpMask, DWORD IfIndex, DWORD* NTEContext, DWORD* NTEInstance );

extern	DWORD	WINAPI	IPH_IpReleaseAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo );
extern	DWORD	WINAPI	IPH_IpRenewAddress( PIP_ADAPTER_INDEX_MAP AdapterInfo );

extern	DWORD	WINAPI	IPH_NotifyAddrChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped );
extern	DWORD	WINAPI	IPH_NotifyRouteChange( OUT HANDLE* Handle, LPOVERLAPPED overlapped );


//IPת���Ĳ���
extern	DWORD	WINAPI	IPH_CreateIpForwardEntry( PMIB_IPFORWARDROW pRoute );
extern	DWORD	WINAPI	IPH_SetIpForwardEntry( PMIB_IPFORWARDROW pRoute );
extern	DWORD	WINAPI	IPH_DeleteIpForwardEntry( PMIB_IPFORWARDROW pRoute );


//
// ICMP���ṩ�� ����
//

extern	DWORD	WINAPI	IPH_GetIcmpStatistics( OUT PMIB_ICMP pStats );

extern	HANDLE	WINAPI	IPH_IcmpCreateFile(void);
extern	BOOL	WINAPI	IPH_IcmpCloseHandle( HANDLE hIcmpHandle );
extern	DWORD	WINAPI	IPH_IcmpSendEcho( HANDLE hIcmpHandle, IPAddr dwDesIP, LPVOID lpRequestData, WORD dwRequestSize, 
											PIP_OPTION_INFORMATION RequestOptions, OUT LPVOID lpReplyBuffer, IN OUT DWORD* pdwReplySize, DWORD dwTimeout );





// -------------------------------------------------------------
// IPHELP���� �ĺ����
// -------------------------------------------------------------

//
// IP���ṩ�� ����
//

#define	GetNetworkParams		IPH_GetNetworkParams
#define	GetDnsSrvList			IPH_GetDnsSrvList

#define	GetNumberOfInterfaces	IPH_GetNumberOfInterfaces
#define	GetAdaptersInfo			IPH_GetAdaptersInfo
#define	GetInterfaceInfo		IPH_GetInterfaceInfo
#define	GetIfEntry				IPH_GetIfEntry
#define	GetIfTable				IPH_GetIfTable
#define	SetIfEntry				IPH_SetIfEntry
#define	GetIpAddrTable			IPH_GetIpAddrTable
#define	GetBestInterface		IPH_GetBestInterface

#define	SetIpTTL				IPH_SetIpTTL
#define	GetIpStatistics			IPH_GetIpStatistics

#define	AddIPAddress			IPH_AddIPAddress
#define	IpReleaseAddress		IPH_IpReleaseAddress
#define	IpRenewAddress			IPH_IpRenewAddress
#define	NotifyAddrChange		IPH_NotifyAddrChange
#define	NotifyRouteChange		IPH_NotifyRouteChange

#define	CreateIpForwardEntry	IPH_CreateIpForwardEntry
#define	SetIpForwardEntry		IPH_SetIpForwardEntry
#define	DeleteIpForwardEntry	IPH_DeleteIpForwardEntry


#define	GetIcmpStatistics		IPH_GetIcmpStatistics
#define	IcmpCreateFile			IPH_IcmpCreateFile
#define	IcmpCloseHandle			IPH_IcmpCloseHandle
#define	IcmpSendEcho			IPH_IcmpSendEcho



#ifdef __cplusplus
}	
#endif

#endif	//_IPHLPAPI_H_

