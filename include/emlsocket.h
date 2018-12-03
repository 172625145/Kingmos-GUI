/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EMLSOCKET_H
#define __EMLSOCKET_H


#ifdef __cplusplus
extern "C" {
#endif


// --------------------------------------------------------------------------------
// 定义：数据类型 各类IP地址
// --------------------------------------------------------------------------------

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;


//typedef u_int				SOCKET;
typedef unsigned long		SOCKET;
typedef int					SKERR;	//注意类型
typedef int					SKRET;	//注意类型


#define	INVALID_SOCKET		(SOCKET)(~0)
#define	SOCKET_ERROR		(0xFFFFFFFF)

#define SOMAXCONN			10

//
//
//
#ifdef	BIG_END
//环回
#define	IN_LOOPBACKNET			127
#define INADDR_LOOPBACK         ((DWORD) 0x7f000001)
#define	IN_LOOPBACK(a)			((((DWORD) (a)) & 0xff000000) == 0x7f000000)
//多播
#define INADDR_UNSPEC_GROUP   	0xe0000000U	/* 224.0.0.0   */
#define INADDR_ALLHOSTS_GROUP 	0xe0000001U	/* 224.0.0.1   */
#define INADDR_ALLRTRS_GROUP    0xe0000002U	/* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  0xe00000ffU	/* 224.0.0.255 */
//测试广播
#define	INADDR_TEST_BC(a)		( (((DWORD)a) & ((DWORD)0xFF000000)) == 0xFF000000 )

//分类
#define IN_CLASSA(i)            (((long)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET           0xff000000
#define IN_CLASSA_NSHIFT        24
#define IN_CLASSA_HOST          0x00ffffff
#define IN_CLASSA_MAX           128

#define IN_CLASSB(i)            (((long)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET           0xffff0000
#define IN_CLASSB_NSHIFT        16
#define IN_CLASSB_HOST          0x0000ffff
#define IN_CLASSB_MAX           65536

#define IN_CLASSC(i)            (((long)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET           0xffffff00
#define IN_CLASSC_NSHIFT        8
#define IN_CLASSC_HOST          0x000000ff

#else	//BIG_END
//环回
#define	IN_LOOPBACKNET			127
#define INADDR_LOOPBACK         ((DWORD) 0x0100007f)
#define	IN_LOOPBACK(a)			((((DWORD) (a)) & 0x000000ff) == 0x0000007f)
//多播
#define INADDR_UNSPEC_GROUP   	0x000000e0U	/* 224.0.0.0   */
#define INADDR_ALLHOSTS_GROUP 	0x010000e0U	/* 224.0.0.1   */
#define INADDR_ALLRTRS_GROUP    0x020000e0U	/* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  0xff0000e0U	/* 224.0.0.255 */
//测试广播
#define	INADDR_TEST_BC(a)		( (((DWORD)a) & ((DWORD)0x000000FF)) == 0x000000FF )

//分类
#define IN_CLASSA(i)            (((long)(i) & 0x00000080) == 0)
#define IN_CLASSA_NET           0x000000ff
#define IN_CLASSA_NSHIFT        24
#define IN_CLASSA_HOST          0xffffff00
#define IN_CLASSA_MAX           128

#define IN_CLASSB(i)            (((long)(i) & 0x000000c0) == 0x00000080)
#define IN_CLASSB_NET           0x0000ffff
#define IN_CLASSB_NSHIFT        16
#define IN_CLASSB_HOST          0xffff0000
#define IN_CLASSB_MAX           65536

#define IN_CLASSC(i)            (((long)(i) & 0x000000e0) == 0x000000c0)
#define IN_CLASSC_NET           0x00ffffff
#define IN_CLASSC_NSHIFT        8
#define IN_CLASSC_HOST          0xff000000

#endif	//BIG_END


// --------------------------------------------------------------------------------
// 定义：SOCKET函数的 参数
// --------------------------------------------------------------------------------


// -------------------------------------
// 参数: WSAData
// -------------------------------------

#define WSADESCRIPTION_LEN      256
#define WSASYS_STATUS_LEN       128
typedef	struct	WSAData
{
	WORD                    wVersion;
	WORD                    wHighVersion;
	char                    szDescription[WSADESCRIPTION_LEN+1];
	char                    szSystemStatus[WSASYS_STATUS_LEN+1];
	unsigned short          iMaxSockets;
	unsigned short          iMaxUdpDg;
	char FAR *              lpVendorInfo;

} _PACKED_ WSADATA, FAR *LPWSADATA;


/*
// -------------------------------------
// 参数: 地址协议族 / 协议族 / 地址长度
// -------------------------------------

//地址协议族
//基本定义
#define AF_UNSPEC				0	// unspecified 
#define AF_UNKNOWN				1	// Somebody is using this! 
#define AF_MAX					100
//现在的实现
#define AF_INET					10	// internetwork: UDP, TCP, etc. 
#define AF_IRDA					11	// 
#define AF_BTH					12	// 
//将来
#define AF_IPX					13	// IPX and SPX 
#define AF_NETBIOS				14	// NetBios-style addresses 
#define AF_APPLETALK			15	// AppleTalk 
#define AF_CCITT				16	// CCITT protocols, X.25 etc 
//将来可能
#define AF_UNIX					20	// local to host (pipes, portals) 
#define AF_ISO					21	// ISO protocols 
#define AF_OSI					AF_ISO	// OSI is ISO 
#define AF_DECnet				22	// DECnet 
#define AF_VOICEVIEW			23	// VoiceView 
*/
//地址协议族
//基本定义

#define AF_UNSPEC       0               /* unspecified */
#define AF_UNIX         1               /* local to host (pipes, portals) */
#define AF_INET         2               /* internetwork: UDP, TCP, etc. */
#define AF_IMPLINK      3               /* arpanet imp addresses */
#define AF_PUP          4               /* pup protocols: e.g. BSP */
#define AF_CHAOS        5               /* mit CHAOS protocols */
#define AF_IPX          6               /* IPX and SPX */
#define AF_NS           6               /* XEROX NS protocols */
#define AF_ISO          7               /* ISO protocols */
#define AF_OSI          AF_ISO          /* OSI is ISO */
#define AF_ECMA         8               /* european computer manufacturers */
#define AF_DATAKIT      9               /* datakit protocols */
#define AF_CCITT        10              /* CCITT protocols, X.25 etc */
#define AF_SNA          11              /* IBM SNA */
#define AF_DECnet       12              /* DECnet */
#define AF_DLI          13              /* Direct data link interface */
#define AF_LAT          14              /* LAT */
#define AF_HYLINK       15              /* NSC Hyperchannel */
#define AF_APPLETALK    16              /* AppleTalk */
#define AF_NETBIOS      17              /* NetBios-style addresses */
#define AF_VOICEVIEW    18              /* VoiceView */
#define AF_FIREFOX      19              /* FireFox */
#define AF_UNKNOWN1     20              /* Somebody is using this! */
#define AF_BAN          21              /* Banyan */

#define AF_MAX          22


//地址协议族
//

#define PF_UNSPEC				AF_UNSPEC	
#define PF_UNKNOWN				AF_UNKNOWN	
#define PF_MAX					AF_MAX		

#define PF_INET					AF_INET		
#define PF_IRDA					AF_IRDA		
#define PF_BTH					AF_BTH		

#define PF_IPX					AF_IPX		
#define PF_NETBIOS				AF_NETBIOS	
#define PF_APPLETALK			AF_APPLETALK
#define PF_CCITT				AF_CCITT	

#define PF_UNIX					AF_UNIX		
#define PF_ISO					AF_ISO		
#define PF_OSI					AF_OSI		
#define PF_DECnet				AF_DECnet	
#define PF_VOICEVIEW			AF_VOICEVIEW



// 地址长度
#define	ADDRLEN_IPV4			4
#define	ADDRLEN_IPV6			128


// -------------------------------------
// 参数: Socket类型
// -------------------------------------
#define SOCK_STREAM				1	// stream socket 
#define SOCK_DGRAM				2	// datagram socket 
#define SOCK_RAW				3	// raw-protocol interface 
#define SOCK_RDM				4	// reliably-delivered message 
#define SOCK_SEQPACKET			5	// sequenced packet stream 


// -------------------------------------
// 参数: 协议类型、端口号
// -------------------------------------

//
//协议类型 ---RFC 1700 或者 IANA
//
#define IPPROTO_IP				0	// dummy for IP 
#define IPPROTO_ICMP			1	// control message protocol 
#define IPPROTO_IGMP			2	// group management protocol 
#define IPPROTO_GGP				3	// gateway^2 (deprecated) 
#define IPPROTO_TCP				6	// tcp 
#define IPPROTO_UDP				17	// user datagram protocol 

#define IPPROTO_RAW				255	// raw IP packet 
#define IPPROTO_MAX				256

//
// 端口号---RFC 1700 或者 IANA
//
// ---network standard functions:  tcp/udp
#define IPPORT_ECHO				7	//
#define IPPORT_DISCARD			9	//
#define IPPORT_DAYTIME			13	//
#define IPPORT_NETSTAT			15	//
#define IPPORT_FTP				21	//
#define IPPORT_TELNET			23	//
#define IPPORT_SMTP				25	//
#define IPPORT_TIMESERVER		37	//
#define IPPORT_NAMESERVER		42	//
#define IPPORT_WHOIS			43	//
#define IPPORT_MTP				57	//
// ---host specific functions:  tcp/udp
#define IPPORT_TFTP				69
#define IPPORT_RJE				77
#define IPPORT_FINGER			79
#define IPPORT_TTYLINK			87
#define IPPORT_SUPDUP			95
// ---UNIX TCP sockets:  
#define IPPORT_EXECSERVER		512
#define IPPORT_LOGINSERVER		513
#define IPPORT_CMDSERVER		514
#define IPPORT_EFSSERVER		520
// ---UNIX UDP sockets:  
#define IPPORT_BIFFUDP			512
#define IPPORT_WHOSERVER		513
#define IPPORT_ROUTESERVER		520
//RESERVED
#define IPPORT_RESERVED			1024

//
#define	PORTAUTO_MIN			49152	//1024
#define	PORTAUTO_MAX			65535

//
#define	SOCKET_MAX				1000	//xyg: 

// -------------------------------------
// 参数: 协议ID------dwProtoID--------
// -------------------------------------

//协议ID号：
#define	PROID_UNKNOWN			0x0000
//datalink
#define	PROID_LCP				0x0010
#define	PROID_PAP				0x0011
#define	PROID_CHAP				0x0012
#define	PROID_IPCP				0x0013

#define	PROID_PPP				0x0020
#define	PROID_ETHER				0x0021
#define	PROID_PPPOE				0x0022
//net
#define	PROID_IP				0x0030
//transfer
#define	PROID_TCP				0x0040
//every layer custom
#define	PROID_PPP_RSVD			0x1020


// -------------------------------------
// 参数: socket地址对结构
// -------------------------------------

// 地址参数---bind/connect/accept/sendto/recvfrom
struct	sockaddr
{
	unsigned short		sa_family;
	char				sa_data[14];
};

typedef struct sockaddr	SOCKADDR;

// -------------------------------------
// 定义：IP地址结构、socket_IP地址对结构、各类IP地址  
// -------------------------------------

//
// IP地址结构( 存储方式：网络字顺序的主机存储 )
//
typedef	struct	in_addr
{
	union
	{
		struct
		{
			BYTE s_b1, s_b2, s_b3, s_b4;
		} S_un_b;
		struct
		{
			USHORT s_w1, s_w2;
		} S_un_w;
		DWORD S_addr;
	} S_un;
#define s_addr  S_un.S_addr
#define s_imp   S_un.S_un_w.s_w2
#define s_net   S_un.S_un_b.s_b1
#define s_host  S_un.S_un_b.s_b2
#define s_lh    S_un.S_un_b.s_b3
#define s_impno S_un.S_un_b.s_b4
} IN_ADDR;

//
//当sa_family==PF_INET时， (sockaddr结构) 就是(sockaddr_in结构)
//socket_IP地址对结构
//
struct	sockaddr_in
{
	short				sin_family;
	u_short				sin_port;
	struct in_addr		sin_addr;
	char				sin_zero[8];
};
typedef struct sockaddr_in	SOCKADDR_IN;

//PF_IRDA地址对  的定义: 当sa_family==PF_IRDA时， sockaddr结构 就是sockaddr_irda结构
//struct sockaddr_irda
//{
//	u_short				irdaAddressFamily;
//	u_char				irdaDeviceID[4];
//	char				irdaServiceName[25];
//};


//
//各类IP地址
//
//任何、广播、无效、
#define	INADDR_ANY				((DWORD) 0x00000000)
#define	INADDR_BROADCAST		((DWORD) 0xffffffff)
#define	INADDR_NONE				((DWORD) 0xffffffff)


// -------------------------------------
// 参数: fd_set 和 timeval
// -------------------------------------

//
//fd_set结构
//
#ifndef	FD_SETSIZE
#define	FD_SETSIZE		64
#endif
typedef	struct	fd_set
{
	u_int		fd_count;
	SOCKET		fd_array[FD_SETSIZE];
} fd_set;

//
//fd_set操作
//

//删除s
#define FD_CLR(fd, set)\
do\
{\
	u_int _i_;\
	u_int _nCnt_;\
	_nCnt_ = ((fd_set FAR *)(set))->fd_count;\
	for(_i_=0; _i_<_nCnt_; _i_++)\
	{\
		if( ((fd_set FAR *)(set))->fd_array[_i_]==fd )\
		{\
			_nCnt_ = ((fd_set FAR *)(set))->fd_count - 1;\
			while( _i_<_nCnt_ )\
			{\
				((fd_set FAR *)(set))->fd_array[_i_] = ((fd_set FAR *)(set))->fd_array[_i_+1]; \
				_i_++;\
			}\
			((fd_set FAR *)(set))->fd_count--;\
			break;\
		}\
	}\
} while(0)

//添加s
#define FD_SET(fd, set)\
do\
{\
	if( ((fd_set FAR *)(set))->fd_count < FD_SETSIZE )\
	{\
		((fd_set FAR *)(set))->fd_array[((fd_set FAR *)(set))->fd_count++]=(fd);\
	}\
} while(0)

//清0
#define FD_ZERO(set)	( ((fd_set FAR *)(set))->fd_count = 0 )

//判断
extern int	Skt_IsSetFD( SOCKET s, fd_set FAR * set );
#define FD_ISSET(fd, set)	Skt_IsSetFD( (SOCKET)(fd), (fd_set FAR *)(set) )


//
//timeval结构
//
struct	timeval
{
	long	tv_sec;
	long	tv_usec;
};

//
//timeval操作
//
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0


// -------------------------------------
// 参数: shutdown 参数
// -------------------------------------
//
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02


// --------------------------------------------------------------------------------
// 定义：ioctlsocket函数的参数 IO控制码
// --------------------------------------------------------------------------------
#define IOCPARM_MASK    0x7f
#define IOC_VOID        0x20000000
#define IOC_OUT         0x40000000
#define IOC_IN          0x80000000
#define IOC_INOUT       (IOC_IN|IOC_OUT)

#define _IO(x,y)        (IOC_VOID|((x)<<8)|(y))
#define _IOR(x,y,t)     (IOC_OUT|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))
#define _IOW(x,y,t)     (IOC_IN|(((long)sizeof(t)&IOCPARM_MASK)<<16)|((x)<<8)|(y))

#define FIONREAD        _IOR('f', 127, u_long)
#define FIONBIO         _IOW('f', 126, u_long)
#define FIOASYNC        _IOW('f', 125, u_long)

#define SIO_FLUSH       _IOW('f', 122, u_long)	//XYG： 


// --------------------------------------------------------------------------------
// 定义：setsockopt函数参数： 选项级别、选项参数的结构、选项名称的定义方法、选项名称
// --------------------------------------------------------------------------------


//
//选项级别
//
#define SOL_SOCKET      0xffff


//
// 选项参数的结构
//

//SO_LINGER 选项参数的结构
struct	linger
{
	u_short	l_onoff;
	u_short	l_linger;

}_PACKED_;
typedef struct linger LINGER;
typedef struct linger *PLINGER;
typedef struct linger FAR *LPLINGER;

//IP_ADD_MEMBERSHIP/IP_DROP_MEMBERSHIP 选项参数的结构
struct	ip_mreq
{
	struct in_addr  imr_multiaddr;  // IP multicast address of group
	struct in_addr  imr_interface;  // local IP address of interface

};
typedef struct ip_mreq IP_MREQ;


//选项名称的定义方法
#define SOC_TYPE				int
#define SOC_OUT					0x8000
#define SOC_IN					0x4000
#define SOC_FIXLEN				0x2000
#define SOC_INOUT				(SOC_IN|SOC_OUT)
#define SOC_LENMASK				0x1F00
#define SOC_GETFIXLEN(opt)		( (opt & SOC_LENMASK)>>8 )
#define SOC_ISIN(opt)			( (opt & SOC_IN) )
#define SOC_ISOUT(opt)			( (opt & SOC_OUT) )

#define SOC_IOR(y,t)			( SOC_OUT | SOC_FIXLEN | (((SOC_TYPE)t<<8) & SOC_LENMASK) | y )
#define SOC_IOW(y,t)			( SOC_IN | SOC_FIXLEN | (((SOC_TYPE)t<<8) & SOC_LENMASK) | y )
#define SOC_IORW(y,t)			( SOC_INOUT | SOC_FIXLEN | (((SOC_TYPE)t<<8) & SOC_LENMASK) | y )

#define SOC_IORW_1(y)			( SOC_INOUT | y )

//
// SOL_SOCKET级别的 TCPIP选项名称
//
#define SO_DEBUG        SOC_IORW(1 , sizeof(BOOL))	//DEBUG开关---[IN/OUT]BOOL*
#define SO_ACCEPTCONN   SOC_IOR( 2 , sizeof(int))	//获取已经listen成功连接的个数---[OUT]int*
#define SO_REUSEADDR    SOC_IORW(3 , sizeof(BOOL))	//重用地址对---[IN/OUT]BOOL*，暂时忽略
#define SO_KEEPALIVE    SOC_IORW(4 , sizeof(BOOL))	//保活机制---[IN/OUT]BOOL*
#define SO_DONTROUTE    SOC_IORW(5 , sizeof(BOOL))	//直接发送---[IN/OUT]BOOL*，本系统忽略该项
#define SO_BROADCAST    SOC_IORW(6 , sizeof(BOOL))	//允许收发广播---[IN/OUT]BOOL*，收发广播数据包的条件是：设置允许广播，或者connect绑定指定的广播地址
#define SO_USELOOPBACK  SOC_IORW(7 , sizeof(BOOL))	//忽略
#define SO_LINGER       SOC_IORW(8 , sizeof(LINGER))//closesocket执行前是否，拖延的时间---[IN/OUT]LINGER结构*
#define SO_OOBINLINE    SOC_IORW(9 , sizeof(BOOL))	//忽略
#define SO_IPINTF		SOC_IORW(10, sizeof(DWORD))	//xyg: 直接使用指定IP接口收发数据---[IN/OUT]DWORD*，如果是0xFFFFFFFF，则表示无效IP接口
#define SO_DONTLINGER   SOC_IORW(11, sizeof(BOOL))	//closesocket执行前，不拖延---[IN/OUT]BOOL*


//非BSD支持的选项
#define SO_SNDBUF       SOC_IORW(20, sizeof(int))	//发送缓冲的长度---[IN/OUT]int*
#define SO_RCVBUF       SOC_IORW(21, sizeof(int))	//接收缓冲的长度---[IN/OUT]int*
#define SO_SNDLOWAT     SOC_IORW(22, sizeof(int))	//忽略
#define SO_RCVLOWAT     SOC_IORW(23, sizeof(int))	//忽略
#define SO_SNDTIMEO     SOC_IORW(24, sizeof(int))	//发送的超时时间---[IN/OUT]int*
#define SO_RCVTIMEO     SOC_IORW(25, sizeof(int))	//接收的超时时间---[IN/OUT]int*
#define SO_ERROR        SOC_IORW(26, sizeof(int))	//忽略
#define SO_TYPE         SOC_IOR( 27, sizeof(int))	//获取socket类型: SOCK_STREAM 等---[OUT]int*
/*
//
// SOL_SOCKET级别的 非TCPIP传输方式，如DECNet, OSI TP4等选项名称
//
#define SO_CONNDATA     0x7000
#define SO_CONNOPT      0x7001
#define SO_DISCDATA     0x7002
#define SO_DISCOPT      0x7003
#define SO_CONNDATALEN  0x7004
#define SO_CONNOPTLEN   0x7005
#define SO_DISCDATALEN  0x7006
#define SO_DISCOPTLEN   0x7007
*/
//
// IPPROTO_TCP级别的 TCP选项
//
#define TCP_NODELAY     SOC_IORW(29, sizeof(BOOL))	//禁止Nagle算法，忽略---[IN/OUT]BOOL
//#define TCP_BSDURGENT   0x7000	//忽略


//
// IPPROTO_IP级别的 IP选项
//
#define IP_OPTIONS          SOC_IORW_1(30)				//IP数据包头选项---[IN/OUT]char*
#define IP_MULTICAST_IF     SOC_IORW(31, sizeof(DWORD))	//IP数据包多播接口---[IN/OUT]DWORD* （IP地址）
#define IP_MULTICAST_TTL    SOC_IORW(32, sizeof(int))	//IP数据包多播TTL---[IN/OUT]int* 
#define IP_MULTICAST_LOOP   SOC_IORW(33, sizeof(BOOL))	//IP数据包多播允许投递给自己---[IN/OUT]BOOL*
#define IP_ADD_MEMBERSHIP   SOC_IORW(34, sizeof(IP_MREQ))	//添加组播成员---[IN]ip_mreq结构*
#define IP_DROP_MEMBERSHIP  SOC_IORW(35, sizeof(IP_MREQ))	//删除组播成员---[IN]ip_mreq结构*
#define IP_TTL              SOC_IORW(36, sizeof(int))	//IP数据包头中的TTL---[IN/OUT]int* 
#define IP_TOS              SOC_IORW(37, sizeof(int))	//IP数据包头中的TOS---[IN/OUT]int* 
#define IP_DONTFRAGMENT     SOC_IORW(38, sizeof(BOOL))	//IP数据包头中的DF标志---[IN/OUT]BOOL*

#define IP_DEFAULT_MULTICAST_TTL   1
#define IP_DEFAULT_MULTICAST_LOOP  1
#define IP_MAX_MEMBERSHIPS         20



/*
//
// SOL_SOCKET级别的 TCPIP选项名称
//
#define SO_DEBUG        0x0001	//DEBUG开关---[IN/OUT]BOOL*
#define SO_ACCEPTCONN   0x0002	//获取已经listen成功连接的个数---[OUT]int*
#define SO_REUSEADDR    0x0004	//重用地址对---[IN/OUT]BOOL*，暂时忽略
#define SO_KEEPALIVE    0x0008	//保活机制---[IN/OUT]BOOL*
#define SO_DONTROUTE    0x0010	//直接发送---[IN/OUT]BOOL*，本系统忽略该项
#define SO_BROADCAST    0x0020	//允许收发广播---[IN/OUT]BOOL*，收发广播数据包的条件是：设置允许广播，或者connect绑定指定的广播地址
#define SO_USELOOPBACK  0x0040	//忽略
#define SO_LINGER       0x0080	//closesocket执行前是否，拖延的时间---[IN/OUT]LINGER结构*
#define SO_OOBINLINE    0x0100	//忽略

#define SO_IPINTF		0x0200	//xyg: 直接使用指定IP接口收发数据---[IN/OUT]DWORD*，如果是0xFFFFFFFF，则表示无效IP接口

#define SO_DONTLINGER   (u_int)(~SO_LINGER)	//closesocket执行前，不拖延---[IN/OUT]BOOL*


//非BSD支持的选项
#define SO_SNDBUF       0x1001	//发送缓冲的长度---[IN/OUT]int*
#define SO_RCVBUF       0x1002	//接收缓冲的长度---[IN/OUT]int*
#define SO_SNDLOWAT     0x1003	//忽略
#define SO_RCVLOWAT     0x1004	//忽略
#define SO_SNDTIMEO     0x1005	//发送的超时时间---[IN/OUT]int*
#define SO_RCVTIMEO     0x1006	//接收的超时时间---[IN/OUT]int*
#define SO_ERROR        0x1007	//忽略
#define SO_TYPE         0x1008	//获取socket类型: SOCK_STREAM 等---[OUT]int*

//
// SOL_SOCKET级别的 非TCPIP传输方式，如DECNet, OSI TP4等选项名称
//
#define SO_CONNDATA     0x7000
#define SO_CONNOPT      0x7001
#define SO_DISCDATA     0x7002
#define SO_DISCOPT      0x7003
#define SO_CONNDATALEN  0x7004
#define SO_CONNOPTLEN   0x7005
#define SO_DISCDATALEN  0x7006
#define SO_DISCOPTLEN   0x7007

//
// IPPROTO_TCP级别的 TCP选项
//
#define TCP_NODELAY     0x0001	//禁止Nagle算法，忽略---[IN/OUT]BOOL
#define TCP_BSDURGENT   0x7000	//忽略


//
// IPPROTO_IP级别的 IP选项
//
#define IP_OPTIONS          1	//IP数据包头选项---[IN/OUT]char*
#define IP_MULTICAST_IF     2	//IP数据包多播接口---[IN/OUT]DWORD* （IP地址）
#define IP_MULTICAST_TTL    3	//IP数据包多播TTL---[IN/OUT]int* 
#define IP_MULTICAST_LOOP   4	//IP数据包多播允许投递给自己---[IN/OUT]BOOL*
#define IP_ADD_MEMBERSHIP   5	//添加组播成员---[IN]ip_mreq结构*
#define IP_DROP_MEMBERSHIP  6	//删除组播成员---[IN]ip_mreq结构*
#define IP_TTL              7	//IP数据包头中的TTL---[IN/OUT]int* 
#define IP_TOS              8	//IP数据包头中的TOS---[IN/OUT]int* 
#define IP_DONTFRAGMENT     9	//IP数据包头中的DF标志---[IN/OUT]BOOL*

#define IP_DEFAULT_MULTICAST_TTL   1
#define IP_DEFAULT_MULTICAST_LOOP  1
#define IP_MAX_MEMBERSHIPS         20


*/


// --------------------------------------------------------------------------------
// 定义：主机信息的结构
// --------------------------------------------------------------------------------

struct	hostent
{
	char*		h_name;		// official name of host
	char**		h_aliases;	// alias list
	short		h_addrtype;	// host address type
	short		h_length;	// length of address
	char**		h_addr_list;// list of addresses
#define h_addr  h_addr_list[0]	// address, for backward compat

} _PACKED_ ;

typedef	struct	hostent 	HOSTENT;

// --------------------------------------------------------------------------------
// 定义：调试的 参数
// --------------------------------------------------------------------------------
//dwOptionDbg
#define	SOCKDBG_IP			0x10000000
#define	SOCKDBG_TCP			0x20000000
#define	SOCKDBG_UDP			0x40000000

#define	SOCKDBG_TYPE		0xFFFF0000
#define	SOCKDBG_PARAM		0x0000FFFF



// --------------------------------------------------------------------------------
// 定义：SOCKET函数的 返回错误值
// --------------------------------------------------------------------------------

#define E_SUCCESS              0
#define E_BASEERR              10000

// C error constants
#define E_INTR                (E_BASEERR+4)
#define E_BADF                (E_BASEERR+9)
#define E_ACCES               (E_BASEERR+13)
#define E_FAULT               (E_BASEERR+14)
#define E_INVAL               (E_BASEERR+22)
#define E_MFILE               (E_BASEERR+24)

// Berkeley error constants
#define E_WOULDBLOCK          (E_BASEERR+35)
#define E_INPROGRESS          (E_BASEERR+36)
#define E_ALREADY             (E_BASEERR+37)
#define E_NOTSOCK             (E_BASEERR+38)
#define E_DESTADDRREQ         (E_BASEERR+39)
#define E_MSGSIZE             (E_BASEERR+40)
#define E_PROTOTYPE           (E_BASEERR+41)
#define E_NOPROTOOPT          (E_BASEERR+42)
#define E_PROTONOSUPPORT      (E_BASEERR+43)
#define E_SOCKTNOSUPPORT      (E_BASEERR+44)
#define E_OPNOTSUPP           (E_BASEERR+45)
#define E_PFNOSUPPORT         (E_BASEERR+46)
#define E_AFNOSUPPORT         (E_BASEERR+47)
#define E_ADDRINUSE           (E_BASEERR+48)
#define E_ADDRNOTAVAIL        (E_BASEERR+49)
#define E_NETDOWN             (E_BASEERR+50)
#define E_NETUNREACH          (E_BASEERR+51)
#define E_NETRESET            (E_BASEERR+52)
#define E_CONNABORTED         (E_BASEERR+53)
#define E_CONNRESET           (E_BASEERR+54)
#define E_NOBUFS              (E_BASEERR+55)
#define E_ISCONN              (E_BASEERR+56)
#define E_NOTCONN             (E_BASEERR+57)
#define E_SHUTDOWN            (E_BASEERR+58)
#define E_TOOMANYREFS         (E_BASEERR+59)
#define E_TIMEDOUT            (E_BASEERR+60)
#define E_CONNREFUSED         (E_BASEERR+61)
#define E_LOOP                (E_BASEERR+62)
#define E_NAMETOOLONG         (E_BASEERR+63)
#define E_HOSTDOWN            (E_BASEERR+64)
#define E_HOSTUNREACH         (E_BASEERR+65)
#define E_NOTEMPTY            (E_BASEERR+66)
#define E_PROCLIM             (E_BASEERR+67)
#define E_USERS               (E_BASEERR+68)
#define E_DQUOT               (E_BASEERR+69)
#define E_STALE               (E_BASEERR+70)
#define E_REMOTE              (E_BASEERR+71)

#define E_DISCON              (E_BASEERR+101)


//
#define E_SYSNOTREADY         (E_BASEERR+91)
#define E_VERNOTSUPPORTED     (E_BASEERR+92)
#define E_NOTINITIALISED      (E_BASEERR+93)

//
#define E_SYSFUN              (E_BASEERR+95)	//added by xyg_2004-04-09
#define E_NOMOREPORT          (E_BASEERR+96)	//added by xyg_2004-04-12
#define E_SND_UNKNOWN         (E_BASEERR+98)	//added by xyg_2004-04-22
#define E_NO_DNSSRV           (E_BASEERR+99)	//added by xyg_2004-05-09




#define WSAESUCCESS             E_SUCCESS        
#define WSAEBASEERR             E_BASEERR        

//
#define WSAEINTR                E_INTR           
#define WSAEBADF                E_BADF           
#define WSAEACCES               E_ACCES          
#define WSAEFAULT               E_FAULT          
#define WSAEINVAL               E_INVAL          
#define WSAEMFILE               E_MFILE          

//
#define WSAEWOULDBLOCK          E_WOULDBLOCK     
#define WSAEINPROGRESS          E_INPROGRESS     
#define WSAEALREADY             E_ALREADY        
#define WSAENOTSOCK             E_NOTSOCK        
#define WSAEDESTADDRREQ         E_DESTADDRREQ    
#define WSAEMSGSIZE             E_MSGSIZE        
#define WSAEPROTOTYPE           E_PROTOTYPE      
#define WSAENOPROTOOPT          E_NOPROTOOPT     
#define WSAEPROTONOSUPPORT      E_PROTONOSUPPORT 
#define WSAESOCKTNOSUPPORT      E_SOCKTNOSUPPORT 
#define WSAEOPNOTSUPP           E_OPNOTSUPP      
#define WSAEPFNOSUPPORT         E_PFNOSUPPORT    
#define WSAEAFNOSUPPORT         E_AFNOSUPPORT    
#define WSAEADDRINUSE           E_ADDRINUSE      
#define WSAEADDRNOTAVAIL        E_ADDRNOTAVAIL   
#define WSAENETDOWN             E_NETDOWN        
#define WSAENETUNREACH          E_NETUNREACH     
#define WSAENETRESET            E_NETRESET       
#define WSAECONNABORTED         E_CONNABORTED    
#define WSAECONNRESET           E_CONNRESET      
#define WSAENOBUFS              E_NOBUFS         
#define WSAEISCONN              E_ISCONN         
#define WSAENOTCONN             E_NOTCONN        
#define WSAESHUTDOWN            E_SHUTDOWN       
#define WSAETOOMANYREFS         E_TOOMANYREFS    
#define WSAETIMEDOUT            E_TIMEDOUT       
#define WSAECONNREFUSED         E_CONNREFUSED    
#define WSAELOOP                E_LOOP           
#define WSAENAMETOOLONG         E_NAMETOOLONG    
#define WSAEHOSTDOWN            E_HOSTDOWN       
#define WSAEHOSTUNREACH         E_HOSTUNREACH    
#define WSAENOTEMPTY            E_NOTEMPTY       
#define WSAEPROCLIM             E_PROCLIM        
#define WSAEUSERS               E_USERS          
#define WSAEDQUOT               E_DQUOT          
#define WSAESTALE               E_STALE          
#define WSAEREMOTE              E_REMOTE         


#define WSAEDISCON              E_DISCON         


//
#define WSAESYSNOTREADY         E_SYSNOTREADY    
#define WSAEVERNOTSUPPORTED     E_VERNOTSUPPORTED
#define WSAENOTINITIALISED      E_NOTINITIALISED 

//
#define WSAESYSFUN              E_SYSFUN         
#define WSAENOMOREPORT          E_NOMOREPORT     
#define WSAESOCKDOWN            E_SOCKDOWN       
#define WSAESND_UNKNOWN         E_SND_UNKNOWN    
#define WSAENO_DNSSRV           E_NO_DNSSRV      




/*
 * Maximum queue length specifiable by listen.
 */

#define MSG_OOB         0x1             /* process out-of-band data */
#define MSG_PEEK        0x2             /* peek at incoming message */
#define MSG_DONTROUTE   0x4             /* send without using routing tables */

#define MSG_MAXIOVLEN   16

#define MSG_PARTIAL     0x8000          /* partial send or recv for message xport */

/*
 * Define constant based on rfc883, used by gethostbyxxxx() calls.
 */
#define MAXGETHOSTSTRUCT        1024



///////////////////////////////////////////////////////////////////////////////////////



// --------------------------------------------------------------------------------
// 定义：函数
// --------------------------------------------------------------------------------

//----------
//辅助函数 的实现
//----------

extern	DWORD	Win32_inet_addr( const char* cp );
extern	char*	Win32_inet_ntoa( struct in_addr in );
extern	void	Win32_inet_ntoa_ex( struct in_addr in, char* paddr );
extern	void	Win32_inet_ntoa_ip( LPBYTE pIP, OUT char* paddr );

extern	BOOL	Win32_SavHostentAddr( OUT HOSTENT* pHostInfo, DWORD dwLenBuf, LPBYTE pAddrList, WORD wAddrLen, IN OUT WORD* pwCntAddr );


//----------
//SOCKET函数 的实现
//----------

extern	SKERR	WINAPI	Win32_WSAStartup( WORD wVersionRequired, LPWSADATA lpWSAData );
extern	SKRET	WINAPI	Win32_WSACleanup(void);
extern	void	WINAPI	Win32_WSASetLastError( SKERR nErrSock );
extern	SKERR	WINAPI	Win32_WSAGetLastError(void);

extern	SOCKET	WINAPI	Win32_socket( int af, int type, int protocol );
extern	SKRET	WINAPI	Win32_bind( SOCKET s, const SOCKADDR* myaddr, int addr_len );
extern	SKRET	WINAPI	Win32_connect( SOCKET s, const SOCKADDR* addr, int addr_len );
extern	SKRET	WINAPI	Win32_listen( SOCKET s, int queue_len );
extern	SOCKET	WINAPI	Win32_accept( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len );
extern	SKRET	WINAPI	Win32_ioctlsocket( SOCKET s, long cmd, IN OUT DWORD* argp );
extern	SKRET	WINAPI	Win32_send( SOCKET s, const char* buf, int len, int flags );
extern	SKRET	WINAPI	Win32_sendto( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen );
extern	SKRET	WINAPI	Win32_recv( SOCKET s, OUT char* buf, int len, int flags );
extern	SKRET	WINAPI	Win32_recvfrom( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, IN OUT int* fromlen );
extern	SKRET	WINAPI	Win32_select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout );

extern	SKRET	WINAPI	Win32_getsockopt( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen );
extern	SKRET	WINAPI	Win32_setsockopt( SOCKET s, int level, int optname, const char* optval, int optlen );
extern	SKRET	WINAPI	Win32_getpeername( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );
extern	SKRET	WINAPI	Win32_getsockname( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );

extern	SKRET	WINAPI	Win32_shutdown( SOCKET s, int how );
extern	SKRET	WINAPI	Win32_closesocket( SOCKET s );

extern	SKRET	WINAPI	Win32_gethostname( OUT char* name, int namelen );
extern	HOSTENT*	WINAPI	Win32_gethostbyname( const char* name );
extern	HOSTENT*	WINAPI	Win32_gethostbyaddr( const char* addr, int len, int type );
extern	HOSTENT*	WINAPI	Win32_gethostbynameex( const char* name, OUT LPBYTE pBufInfo, int len );
extern	HOSTENT*	WINAPI	Win32_gethostbyaddrex( const char* addr, int len, int type, OUT LPBYTE pBufInfo, int lenInfo );



//----------
//辅助函数 的宏替代
//----------

#define	inet_addr			Win32_inet_addr
#define	inet_ntoa			Win32_inet_ntoa
#define	inet_ntoa_ex		Win32_inet_ntoa_ex
#define	inet_ntoa_ip		Win32_inet_ntoa_ip

#define	SavHostentAddr		Win32_SavHostentAddr


//----------
//SOCKET函数 的宏替代
//----------

#define	WSAStartup			Win32_WSAStartup		
#define	WSACleanup			Win32_WSACleanup		
#define	WSASetLastError		Win32_WSASetLastError	
#define	WSAGetLastError		Win32_WSAGetLastError	

#define	socket				Win32_socket			
#define	bind				Win32_bind			
#define	connect				Win32_connect			
#define	listen				Win32_listen			
#define	accept				Win32_accept			
#define	ioctlsocket			Win32_ioctlsocket		
#define	send				Win32_send			
#define	sendto				Win32_sendto			
#define	recv				Win32_recv			
#define	recvfrom			Win32_recvfrom		
#define	select				Win32_select			

#define	getsockopt			Win32_getsockopt		
#define	setsockopt			Win32_setsockopt		
#define	getpeername			Win32_getpeername		
#define	getsockname			Win32_getsockname		

#define	shutdown			Win32_shutdown		
#define	closesocket			Win32_closesocket		

#define	gethostname			Win32_gethostname		
#define	gethostbyname		Win32_gethostbyname		
#define	gethostbyaddr		Win32_gethostbyaddr		
#define	gethostbynameex		Win32_gethostbynameex		
#define	gethostbyaddrex		Win32_gethostbyaddrex		



//
//#ifndef _NETSRVDBG_H_
//#include <netsrvdbg.h>
//#endif



#ifdef __cplusplus
}	
#endif

#endif	//__EMLSOCKET_H
