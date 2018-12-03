/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_PPP_AP_H_
#define	_PPP_AP_H_

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------
//
//------------------------------------------------------

#if 0
typedef	struct	_PAP_HDR
{
	BYTE	bCode;
	BYTE	bAskerID;
	WORD	wLenTotal;

} PAP_HDR;
#endif

// bCode
#define	PAP_CODE_REQ			1  //     Authenticate-Request
#define	PAP_CODE_ACK			2  //     Authenticate-Ack
#define	PAP_CODE_NAK			3  //     Authenticate-Nak

#define CHAP_CODE_CHALLENGE		1
#define CHAP_CODE_RESPONSE		2
#define CHAP_CODE_SUCCESS		3
#define CHAP_CODE_FAILURE		4



#ifdef __cplusplus
}	
#endif


#endif	//_PPP_AP_H_




