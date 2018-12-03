/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_PPP_LCP_H_
#define	_PPP_LCP_H_

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------
//
//------------------------------------------------------

// bType of IPCP's config request option's TLD struct
#define	IPCP_TLD_TYPE_IP_COMP		2   //    IP-Compression-Protocol 
#define	IPCP_TLD_TYPE_IP_ADDR		3   //    IP-Address              
#define	IPCP_TLD_TYPE_M_IPV4			4   //    Mobile-IPv4 			   :
#define	IPCP_TLD_TYPE_P_DNS_ADDR		129 //    Primary DNS Server Address   
#define	IPCP_TLD_TYPE_P_NBNS_ADDR	130 //    Primary NBNS Server Address  
#define	IPCP_TLD_TYPE_S_DNS_ADDR		131 //    Secondary DNS Server Address 
#define	IPCP_TLD_TYPE_S_NBNS_ADDR	132 //    Secondary NBNS Server Address

// bLenTotal of LCP's config option's TLD struct
#define	IPCP_TLD_LEN_IP_ADDR		6   //    IP-Address, DNS, NBNS

// dwOption of IPCP_Up function
#define	IPCP_OPT_IP_COMP			0x0001
#define	IPCP_OPT_IP_ADDR			0x0002
#define	IPCP_OPT_M_IPV4				0x0004
#define	IPCP_OPT_P_DNS_ADDR			0x0008
#define	IPCP_OPT_P_NBNS_ADDR		0x0010
#define	IPCP_OPT_S_DNS_ADDR			0x0040
#define	IPCP_OPT_S_NBNS_ADDR		0x0080


#ifdef __cplusplus
}	
#endif


#endif	//_PPP_LCP_H_






