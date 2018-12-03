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

//LCP Packet Formats:
//    0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Code      |  Identifier   |            Length             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |    Data ...
//   +-+-+-+-+

//Configuration Option format: 
//    0                   1
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |    Length     |    Data ...
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


// bCode
#define	LCP_CODE_CFG_REQ			1  //     Configure-Reque
#define	LCP_CODE_CFG_ACK			2  //     Configure-Ack
#define	LCP_CODE_CFG_NAK			3  //     Configure-Nak
#define	LCP_CODE_CFG_REJ			4  //     Configure-Rejec
#define	LCP_CODE_TER_REQ			5  //     Terminate-Requet
#define	LCP_CODE_TER_ACK			6  //     Terminate-Ackst
#define	LCP_CODE_REJ_CODE			7  //     Code-Reject
#define	LCP_CODE_REJ_PRO			8  //     Protocol-Reject
#define	LCP_CODE_ECHO_REQ			9  //     Echo-Request
#define	LCP_CODE_ECHO_REPLY			10 //     Echo-Reply
#define	LCP_CODE_DIS_REQ			11 //     Discard-Request


// bType of LCP's config request option's TLD struct
#define	LCP_TLD_TYPE_MRU			1   //    Maximum-Receive-Unit
#define	LCP_TLD_TYPE_ACCM			2   //    Async-Control-Character-Map: most significant octet first sent, The least bit of the least octet (the final octet transmitted) is numbered bit 0
#define	LCP_TLD_TYPE_AP				3   //    Authentication-Protocol
#define	LCP_TLD_TYPE_QP				4   //    Quality-Protocol
#define	LCP_TLD_TYPE_MN				5   //    Magic-Number
#define	LCP_TLD_TYPE_PFC			7   //    Protocol-Field-Compression
#define	LCP_TLD_TYPE_ACFC			8   //    Address-and-Control-Field-Compression
// bLenTotal of LCP's config option's TLD struct
#define	LCP_TLD_LEN_MRU				4   //    Maximum-Receive-Unit
#define	LCP_TLD_LEN_ACCM			6   //    Async-Control-Character-Map
#define	LCP_TLD_LEN_PAP				4   //    Authentication-Protocol
#define	LCP_TLD_LEN_CHAP			5   //    Authentication-Protocol
#define	LCP_TLD_LEN_QP_MIN			4   //    Quality-Protocol
#define	LCP_TLD_LEN_MN				6   //    Magic-Number
#define	LCP_TLD_LEN_PFC				2   //    Protocol-Field-Compression
#define	LCP_TLD_LEN_ACFC			2   //    Address-and-Control-Field-Compression

// dwOption of LCP_Up function
#define	LCP_OPT_MRU					0x0001
#define	LCP_OPT_ACCM				0x0002
#define	LCP_OPT_PAP					0x0004
#define	LCP_OPT_QP					0x0008
#define	LCP_OPT_MN					0x0010
#define	LCP_OPT_PFC					0x0040
#define	LCP_OPT_ACFC				0x0080
#define	LCP_OPT_CHAP				0x0100


//
#define	CHAP_MD5					5


#ifdef __cplusplus
}	
#endif


#endif	//_PPP_LCP_H_

