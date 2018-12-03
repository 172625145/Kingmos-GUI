/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _ENCODE_H_
#define _ENCODE_H_

#ifndef _FRAGMENT_H_
#include "fragment.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


// ----------------------------------------
// MD5
// ----------------------------------------
#define MD5_SIGNATURE_SIZE 16

/* MD5 context. */
typedef struct {
  unsigned long state[4];  
  unsigned long count[2];  
  unsigned char buffer[64];  /* input buffer */
} MD5_CTX;

void MD5Init( MD5_CTX *context );
void MD5Update( MD5_CTX *context, unsigned char *input, unsigned int inputLen );
void MD5Final( unsigned char digest[16], MD5_CTX *context );

extern	void	CreateMD5( unsigned char * inBuf, unsigned int inLen,unsigned char * outBuf );
extern	void	CreateMD5_Ex( FRAGSETA* pFragSetA, OUT LPBYTE pBufDigest );

// ----------------------------------------
// BASE64
// ----------------------------------------

unsigned long base64_encode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );
unsigned long base64_decode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );
unsigned long quotedprint_encode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );
unsigned long quotedprint_decode( unsigned char * src_ptr,unsigned long src_len,unsigned char * dst_ptr );


// ----------------------------------------
// DIGEST
// ----------------------------------------
#define HASHLEN 16
typedef char HASH[HASHLEN];
#define HASHHEXLEN 32
typedef char HASHHEX[HASHHEXLEN+1];
#define IN
#define OUT

/* calculate H(A1) as per HTTP Digest spec */
void DigestCalcHA1(
    IN char * pszAlg,
    IN char * pszUserName,
    IN char * pszRealm,
    IN char * pszPassword,
    IN char * pszNonce,
    IN char * pszCNonce,
    OUT HASHHEX SessionKey
    );

/* calculate request-digest/response-digest as per HTTP Digest spec */
void DigestCalcResponse(
    IN HASHHEX HA1,           /* H(A1) */
    IN char * pszNonce,       /* nonce from server */
    IN char * pszNonceCount,  /* 8 hex digits */
    IN char * pszCNonce,      /* client nonce */
    IN char * pszQop,         /* qop-value: "", "auth", "auth-int" */
    IN char * pszMethod,      /* method from the request */
    IN char * pszDigestUri,   /* requested URL */
    IN HASHHEX HEntity,       /* H(entity body) if qop="auth-int" */
    OUT HASHHEX Response      /* request-digest or response-digest */
    );

	
#ifdef __cplusplus
}	
#endif

#endif	//	_ENCODE_H_

