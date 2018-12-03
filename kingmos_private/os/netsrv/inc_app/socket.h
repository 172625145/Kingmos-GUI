/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _KINGMOS_SOCKET_H_
#define _KINGMOS_SOCKET_H_

#ifndef	_END_ALIGN_H_
#include <endalign.h>
#endif	//_END_ALIGN_H_

#ifdef __cplusplus
extern "C" {
#endif


// --------------------------------------------------------------------------------
// ���壺�������� ����IP��ַ
// --------------------------------------------------------------------------------

typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;


//typedef u_int				SOCKET;
typedef unsigned long		SOCKET;
typedef int					SKERR;	//ע������
typedef int					SKRET;	//ע������


#define	INVALID_SOCKET		(SOCKET)(~0)
#define	SOCKET_ERROR		(0xFFFFFFFF)

#define SOMAXCONN			10

//
//
//
#ifdef	BIG_END
//����
#define	IN_LOOPBACKNET			127
#define INADDR_LOOPBACK         ((DWORD) 0x7f000001)
#define	IN_LOOPBACK(a)			((((DWORD) (a)) & 0xff000000) == 0x7f000000)
//�ಥ
#define INADDR_UNSPEC_GROUP   	0xe0000000U	/* 224.0.0.0   */
#define INADDR_ALLHOSTS_GROUP 	0xe0000001U	/* 224.0.0.1   */
#define INADDR_ALLRTRS_GROUP    0xe0000002U	/* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  0xe00000ffU	/* 224.0.0.255 */
//���Թ㲥
#define	INADDR_TEST_BC(a)		( (((DWORD)a) & ((DWORD)0xFF000000)) == 0xFF000000 )

//����
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
//����
#define	IN_LOOPBACKNET			127
#define INADDR_LOOPBACK         ((DWORD) 0x0100007f)
#define	IN_LOOPBACK(a)			((((DWORD) (a)) & 0x000000ff) == 0x0000007f)
//�ಥ
#define INADDR_UNSPEC_GROUP   	0x000000e0U	/* 224.0.0.0   */
#define INADDR_ALLHOSTS_GROUP 	0x010000e0U	/* 224.0.0.1   */
#define INADDR_ALLRTRS_GROUP    0x020000e0U	/* 224.0.0.2 */
#define INADDR_MAX_LOCAL_GROUP  0xff0000e0U	/* 224.0.0.255 */
//���Թ㲥
#define	INADDR_TEST_BC(a)		( (((DWORD)a) & ((DWORD)0x000000FF)) == 0x000000FF )

//����
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
// ���壺SOCKET������ ����
// --------------------------------------------------------------------------------


// -------------------------------------
// ����: WSAData
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



// -------------------------------------
// ����: ��ַЭ���� / Э���� / ��ַ����
// -------------------------------------

//��ַЭ����
//��������
#define AF_UNSPEC				0	// unspecified 
#define AF_UNKNOWN				1	// Somebody is using this! 
#define AF_MAX					100
//���ڵ�ʵ��
#define AF_INET					10	// internetwork: UDP, TCP, etc. 
#define AF_IRDA					11	// 
#define AF_BTH					12	// 
//����
#define AF_IPX					13	// IPX and SPX 
#define AF_NETBIOS				14	// NetBios-style addresses 
#define AF_APPLETALK			15	// AppleTalk 
#define AF_CCITT				16	// CCITT protocols, X.25 etc 
//��������
#define AF_UNIX					20	// local to host (pipes, portals) 
#define AF_ISO					21	// ISO protocols 
#define AF_OSI					AF_ISO	// OSI is ISO 
#define AF_DECnet				22	// DECnet 
#define AF_VOICEVIEW			23	// VoiceView 


//��ַЭ����
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


// ��ַ����
#define	ADDRLEN_IPV4			4
#define	ADDRLEN_IPV6			128


// -------------------------------------
// ����: Socket����
// -------------------------------------
#define SOCK_STREAM				1	// stream socket 
#define SOCK_DGRAM				2	// datagram socket 
#define SOCK_RAW				3	// raw-protocol interface 
#define SOCK_RDM				4	// reliably-delivered message 
#define SOCK_SEQPACKET			5	// sequenced packet stream 


// -------------------------------------
// ����: Э�����͡��˿ں�
// -------------------------------------

//
//Э������ ---RFC 1700 ���� IANA
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
// �˿ں�---RFC 1700 ���� IANA
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
// ����: Э��ID------dwProtoID--------
// -------------------------------------

//Э��ID�ţ�
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
// ����: socket��ַ�Խṹ
// -------------------------------------

// ��ַ����---bind/connect/accept/sendto/recvfrom
struct	sockaddr
{
	unsigned short		sa_family;
	char				sa_data[14];
};

typedef struct sockaddr	SOCKADDR;

// -------------------------------------
// ���壺IP��ַ�ṹ��socket_IP��ַ�Խṹ������IP��ַ  
// -------------------------------------

//
// IP��ַ�ṹ( �洢��ʽ��������˳��������洢 )
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
//��sa_family==PF_INETʱ�� (sockaddr�ṹ) ����(sockaddr_in�ṹ)
//socket_IP��ַ�Խṹ
//
struct	sockaddr_in
{
	short				sin_family;
	u_short				sin_port;
	struct in_addr		sin_addr;
	char				sin_zero[8];
};
typedef struct sockaddr_in	SOCKADDR_IN;

//PF_IRDA��ַ��  �Ķ���: ��sa_family==PF_IRDAʱ�� sockaddr�ṹ ����sockaddr_irda�ṹ
//struct sockaddr_irda
//{
//	u_short				irdaAddressFamily;
//	u_char				irdaDeviceID[4];
//	char				irdaServiceName[25];
//};


//
//����IP��ַ
//
//�κΡ��㲥����Ч��
#define	INADDR_ANY				((DWORD) 0x00000000)
#define	INADDR_BROADCAST		((DWORD) 0xffffffff)
#define	INADDR_NONE				((DWORD) 0xffffffff)


// -------------------------------------
// ����: fd_set �� timeval
// -------------------------------------

//
//fd_set�ṹ
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
//fd_set����
//

//ɾ��s
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

//���s
#define FD_SET(fd, set)\
do\
{\
	if( ((fd_set FAR *)(set))->fd_count < FD_SETSIZE )\
	{\
		((fd_set FAR *)(set))->fd_array[((fd_set FAR *)(set))->fd_count++]=(fd);\
	}\
} while(0)

//��0
#define FD_ZERO(set)	( ((fd_set FAR *)(set))->fd_count = 0 )

//�ж�
extern int	Skt_IsSetFD( SOCKET s, fd_set FAR * set );
#define FD_ISSET(fd, set)	Skt_IsSetFD( (SOCKET)(fd), (fd_set FAR *)(set) )


//
//timeval�ṹ
//
struct	timeval
{
	long	tv_sec;
	long	tv_usec;
};

//
//timeval����
//
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
         (tvp)->tv_sec == (uvp)->tv_sec && (tvp)->tv_usec cmp (uvp)->tv_usec)
#define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0


// -------------------------------------
// ����: shutdown ����
// -------------------------------------
//
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02


// --------------------------------------------------------------------------------
// ���壺ioctlsocket�����Ĳ��� IO������
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

#define SIO_FLUSH       _IOW('f', 122, u_long)	//XYG�� 


// --------------------------------------------------------------------------------
// ���壺setsockopt���������� ѡ���ѡ������Ľṹ��ѡ�����ƵĶ��巽����ѡ������
// --------------------------------------------------------------------------------


//
//ѡ���
//
#define SOL_SOCKET      0xffff


//
// ѡ������Ľṹ
//

//SO_LINGER ѡ������Ľṹ
struct	linger
{
	u_short	l_onoff;
	u_short	l_linger;

}_PACKED_;
typedef struct linger LINGER;
typedef struct linger *PLINGER;
typedef struct linger FAR *LPLINGER;

//IP_ADD_MEMBERSHIP/IP_DROP_MEMBERSHIP ѡ������Ľṹ
struct	ip_mreq
{
	struct in_addr  imr_multiaddr;  // IP multicast address of group
	struct in_addr  imr_interface;  // local IP address of interface

};
typedef struct ip_mreq IP_MREQ;


//ѡ�����ƵĶ��巽��
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
// SOL_SOCKET����� TCPIPѡ������
//
#define SO_DEBUG        SOC_IORW(1 , sizeof(BOOL))	//DEBUG����---[IN/OUT]BOOL*
#define SO_ACCEPTCONN   SOC_IOR( 2 , sizeof(int))	//��ȡ�Ѿ�listen�ɹ����ӵĸ���---[OUT]int*
#define SO_REUSEADDR    SOC_IORW(3 , sizeof(BOOL))	//���õ�ַ��---[IN/OUT]BOOL*����ʱ����
#define SO_KEEPALIVE    SOC_IORW(4 , sizeof(BOOL))	//�������---[IN/OUT]BOOL*
#define SO_DONTROUTE    SOC_IORW(5 , sizeof(BOOL))	//ֱ�ӷ���---[IN/OUT]BOOL*����ϵͳ���Ը���
#define SO_BROADCAST    SOC_IORW(6 , sizeof(BOOL))	//�����շ��㲥---[IN/OUT]BOOL*���շ��㲥���ݰ��������ǣ���������㲥������connect��ָ���Ĺ㲥��ַ
#define SO_USELOOPBACK  SOC_IORW(7 , sizeof(BOOL))	//����
#define SO_LINGER       SOC_IORW(8 , sizeof(LINGER))//closesocketִ��ǰ�Ƿ����ӵ�ʱ��---[IN/OUT]LINGER�ṹ*
#define SO_OOBINLINE    SOC_IORW(9 , sizeof(BOOL))	//����
#define SO_IPINTF		SOC_IORW(10, sizeof(DWORD))	//xyg: ֱ��ʹ��ָ��IP�ӿ��շ�����---[IN/OUT]DWORD*�������0xFFFFFFFF�����ʾ��ЧIP�ӿ�
#define SO_DONTLINGER   SOC_IORW(11, sizeof(BOOL))	//closesocketִ��ǰ��������---[IN/OUT]BOOL*


//��BSD֧�ֵ�ѡ��
#define SO_SNDBUF       SOC_IORW(20, sizeof(int))	//���ͻ���ĳ���---[IN/OUT]int*
#define SO_RCVBUF       SOC_IORW(21, sizeof(int))	//���ջ���ĳ���---[IN/OUT]int*
#define SO_SNDLOWAT     SOC_IORW(22, sizeof(int))	//����
#define SO_RCVLOWAT     SOC_IORW(23, sizeof(int))	//����
#define SO_SNDTIMEO     SOC_IORW(24, sizeof(int))	//���͵ĳ�ʱʱ��---[IN/OUT]int*
#define SO_RCVTIMEO     SOC_IORW(25, sizeof(int))	//���յĳ�ʱʱ��---[IN/OUT]int*
#define SO_ERROR        SOC_IORW(26, sizeof(int))	//����
#define SO_TYPE         SOC_IOR( 27, sizeof(int))	//��ȡsocket����: SOCK_STREAM ��---[OUT]int*
/*
//
// SOL_SOCKET����� ��TCPIP���䷽ʽ����DECNet, OSI TP4��ѡ������
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
// IPPROTO_TCP����� TCPѡ��
//
#define TCP_NODELAY     SOC_IORW(29, sizeof(BOOL))	//��ֹNagle�㷨������---[IN/OUT]BOOL
//#define TCP_BSDURGENT   0x7000	//����


//
// IPPROTO_IP����� IPѡ��
//
#define IP_OPTIONS          SOC_IORW_1(30)				//IP���ݰ�ͷѡ��---[IN/OUT]char*
#define IP_MULTICAST_IF     SOC_IORW(31, sizeof(DWORD))	//IP���ݰ��ಥ�ӿ�---[IN/OUT]DWORD* ��IP��ַ��
#define IP_MULTICAST_TTL    SOC_IORW(32, sizeof(int))	//IP���ݰ��ಥTTL---[IN/OUT]int* 
#define IP_MULTICAST_LOOP   SOC_IORW(33, sizeof(BOOL))	//IP���ݰ��ಥ����Ͷ�ݸ��Լ�---[IN/OUT]BOOL*
#define IP_ADD_MEMBERSHIP   SOC_IORW(34, sizeof(IP_MREQ))	//����鲥��Ա---[IN]ip_mreq�ṹ*
#define IP_DROP_MEMBERSHIP  SOC_IORW(35, sizeof(IP_MREQ))	//ɾ���鲥��Ա---[IN]ip_mreq�ṹ*
#define IP_TTL              SOC_IORW(36, sizeof(int))	//IP���ݰ�ͷ�е�TTL---[IN/OUT]int* 
#define IP_TOS              SOC_IORW(37, sizeof(int))	//IP���ݰ�ͷ�е�TOS---[IN/OUT]int* 
#define IP_DONTFRAGMENT     SOC_IORW(38, sizeof(BOOL))	//IP���ݰ�ͷ�е�DF��־---[IN/OUT]BOOL*

#define IP_DEFAULT_MULTICAST_TTL   1
#define IP_DEFAULT_MULTICAST_LOOP  1
#define IP_MAX_MEMBERSHIPS         20



/*
//
// SOL_SOCKET����� TCPIPѡ������
//
#define SO_DEBUG        0x0001	//DEBUG����---[IN/OUT]BOOL*
#define SO_ACCEPTCONN   0x0002	//��ȡ�Ѿ�listen�ɹ����ӵĸ���---[OUT]int*
#define SO_REUSEADDR    0x0004	//���õ�ַ��---[IN/OUT]BOOL*����ʱ����
#define SO_KEEPALIVE    0x0008	//�������---[IN/OUT]BOOL*
#define SO_DONTROUTE    0x0010	//ֱ�ӷ���---[IN/OUT]BOOL*����ϵͳ���Ը���
#define SO_BROADCAST    0x0020	//�����շ��㲥---[IN/OUT]BOOL*���շ��㲥���ݰ��������ǣ���������㲥������connect��ָ���Ĺ㲥��ַ
#define SO_USELOOPBACK  0x0040	//����
#define SO_LINGER       0x0080	//closesocketִ��ǰ�Ƿ����ӵ�ʱ��---[IN/OUT]LINGER�ṹ*
#define SO_OOBINLINE    0x0100	//����

#define SO_IPINTF		0x0200	//xyg: ֱ��ʹ��ָ��IP�ӿ��շ�����---[IN/OUT]DWORD*�������0xFFFFFFFF�����ʾ��ЧIP�ӿ�

#define SO_DONTLINGER   (u_int)(~SO_LINGER)	//closesocketִ��ǰ��������---[IN/OUT]BOOL*


//��BSD֧�ֵ�ѡ��
#define SO_SNDBUF       0x1001	//���ͻ���ĳ���---[IN/OUT]int*
#define SO_RCVBUF       0x1002	//���ջ���ĳ���---[IN/OUT]int*
#define SO_SNDLOWAT     0x1003	//����
#define SO_RCVLOWAT     0x1004	//����
#define SO_SNDTIMEO     0x1005	//���͵ĳ�ʱʱ��---[IN/OUT]int*
#define SO_RCVTIMEO     0x1006	//���յĳ�ʱʱ��---[IN/OUT]int*
#define SO_ERROR        0x1007	//����
#define SO_TYPE         0x1008	//��ȡsocket����: SOCK_STREAM ��---[OUT]int*

//
// SOL_SOCKET����� ��TCPIP���䷽ʽ����DECNet, OSI TP4��ѡ������
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
// IPPROTO_TCP����� TCPѡ��
//
#define TCP_NODELAY     0x0001	//��ֹNagle�㷨������---[IN/OUT]BOOL
#define TCP_BSDURGENT   0x7000	//����


//
// IPPROTO_IP����� IPѡ��
//
#define IP_OPTIONS          1	//IP���ݰ�ͷѡ��---[IN/OUT]char*
#define IP_MULTICAST_IF     2	//IP���ݰ��ಥ�ӿ�---[IN/OUT]DWORD* ��IP��ַ��
#define IP_MULTICAST_TTL    3	//IP���ݰ��ಥTTL---[IN/OUT]int* 
#define IP_MULTICAST_LOOP   4	//IP���ݰ��ಥ����Ͷ�ݸ��Լ�---[IN/OUT]BOOL*
#define IP_ADD_MEMBERSHIP   5	//����鲥��Ա---[IN]ip_mreq�ṹ*
#define IP_DROP_MEMBERSHIP  6	//ɾ���鲥��Ա---[IN]ip_mreq�ṹ*
#define IP_TTL              7	//IP���ݰ�ͷ�е�TTL---[IN/OUT]int* 
#define IP_TOS              8	//IP���ݰ�ͷ�е�TOS---[IN/OUT]int* 
#define IP_DONTFRAGMENT     9	//IP���ݰ�ͷ�е�DF��־---[IN/OUT]BOOL*

#define IP_DEFAULT_MULTICAST_TTL   1
#define IP_DEFAULT_MULTICAST_LOOP  1
#define IP_MAX_MEMBERSHIPS         20


*/


// --------------------------------------------------------------------------------
// ���壺������Ϣ�Ľṹ
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
// ���壺���Ե� ����
// --------------------------------------------------------------------------------
//dwOptionDbg
#define	SOCKDBG_IP			0x10000000
#define	SOCKDBG_TCP			0x20000000
#define	SOCKDBG_UDP			0x40000000

#define	SOCKDBG_TYPE		0xFFFF0000
#define	SOCKDBG_PARAM		0x0000FFFF



// --------------------------------------------------------------------------------
// ���壺SOCKET������ ���ش���ֵ
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
// ���壺����
// --------------------------------------------------------------------------------

//----------
//�������� ��ʵ��
//----------

extern	DWORD	Skt_inet_addr( const char* cp );
extern	char*	Skt_inet_ntoa( struct in_addr in );
extern	void	Skt_inet_ntoa_ex( struct in_addr in, char* paddr );
extern	void	Skt_inet_ntoa_ip( LPBYTE pIP, OUT char* paddr );

extern	BOOL	Skt_SavHostentAddr( OUT HOSTENT* pHostInfo, DWORD dwLenBuf, LPBYTE pAddrList, WORD wAddrLen, IN OUT WORD* pwCntAddr );


//----------
//SOCKET���� ��ʵ��
//----------

extern	SKERR	WINAPI	Skt_WSAStartup( WORD wVersionRequired, LPWSADATA lpWSAData );
extern	SKRET	WINAPI	Skt_WSACleanup(void);
extern	void	WINAPI	Skt_WSASetLastError( SKERR nErrSock );
extern	SKERR	WINAPI	Skt_WSAGetLastError(void);

extern	SOCKET	WINAPI	Skt_socket( int af, int type, int protocol );
extern	SKRET	WINAPI	Skt_bind( SOCKET s, const SOCKADDR* myaddr, int addr_len );
extern	SKRET	WINAPI	Skt_connect( SOCKET s, const SOCKADDR* addr, int addr_len );
extern	SKRET	WINAPI	Skt_listen( SOCKET s, int queue_len );
extern	SOCKET	WINAPI	Skt_accept( SOCKET s, OUT SOCKADDR* addr, OUT int* paddr_len );
extern	SKRET	WINAPI	Skt_ioctlsocket( SOCKET s, long cmd, IN OUT DWORD* argp );
extern	SKRET	WINAPI	Skt_send( SOCKET s, const char* buf, int len, int flags );
extern	SKRET	WINAPI	Skt_sendto( SOCKET s, const char* buf, int len, int flags, const SOCKADDR* to, int tolen );
extern	SKRET	WINAPI	Skt_recv( SOCKET s, OUT char* buf, int len, int flags );
extern	SKRET	WINAPI	Skt_recvfrom( SOCKET s, OUT char* buf, int len, int flags, OUT SOCKADDR* from, IN OUT int* fromlen );
extern	SKRET	WINAPI	Skt_select( int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, const struct timeval* timeout );

extern	SKRET	WINAPI	Skt_getsockopt( SOCKET s, int level, int optname, OUT char* optval, IN OUT int* optlen );
extern	SKRET	WINAPI	Skt_setsockopt( SOCKET s, int level, int optname, const char* optval, int optlen );
extern	SKRET	WINAPI	Skt_getpeername( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );
extern	SKRET	WINAPI	Skt_getsockname( SOCKET s, OUT SOCKADDR* name, IN OUT int* namelen );

extern	SKRET	WINAPI	Skt_shutdown( SOCKET s, int how );
extern	SKRET	WINAPI	Skt_closesocket( SOCKET s );

extern	SKRET	WINAPI	Skt_gethostname( OUT char* name, int namelen );
extern	HOSTENT*	WINAPI	Skt_gethostbyname( const char* name );
extern	HOSTENT*	WINAPI	Skt_gethostbyaddr( const char* addr, int len, int type );
extern	HOSTENT*	WINAPI	Skt_gethostbynameex( const char* name, OUT LPBYTE pBufInfo, int len );
extern	HOSTENT*	WINAPI	Skt_gethostbyaddrex( const char* addr, int len, int type, OUT LPBYTE pBufInfo, int lenInfo );



//----------
//�������� �ĺ����
//----------

#define	inet_addr			Skt_inet_addr
#define	inet_ntoa			Skt_inet_ntoa
#define	inet_ntoa_ex		Skt_inet_ntoa_ex
#define	inet_ntoa_ip		Skt_inet_ntoa_ip

#define	SavHostentAddr		Skt_SavHostentAddr


//----------
//SOCKET���� �ĺ����
//----------

#define	WSAStartup			Skt_WSAStartup		
#define	WSACleanup			Skt_WSACleanup		
#define	WSASetLastError		Skt_WSASetLastError	
#define	WSAGetLastError		Skt_WSAGetLastError	

#define	socket				Skt_socket			
#define	bind				Skt_bind			
#define	connect				Skt_connect			
#define	listen				Skt_listen			
#define	accept				Skt_accept			
#define	ioctlsocket			Skt_ioctlsocket		
#define	send				Skt_send			
#define	sendto				Skt_sendto			
#define	recv				Skt_recv			
#define	recvfrom			Skt_recvfrom		
#define	select				Skt_select			

#define	getsockopt			Skt_getsockopt		
#define	setsockopt			Skt_setsockopt		
#define	getpeername			Skt_getpeername		
#define	getsockname			Skt_getsockname		

#define	shutdown			Skt_shutdown		
#define	closesocket			Skt_closesocket		

#define	gethostname			Skt_gethostname		
#define	gethostbyname		Skt_gethostbyname		
#define	gethostbyaddr		Skt_gethostbyaddr		
#define	gethostbynameex		Skt_gethostbynameex		
#define	gethostbyaddrex		Skt_gethostbyaddrex		



//
#ifndef _NETSRVDBG_H_
#include <netsrvdbg.h>
#endif



#ifdef __cplusplus
}	
#endif

#endif	//_KINGMOS_SOCKET_H_
