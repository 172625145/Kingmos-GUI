/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

#ifndef _QUOTED_BASE64_H_
#define _QUOTED_BASE64_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  

void DecodeQuoted(CHAR *szSource,CHAR *szDest);
void EncodeQuoted(CHAR *szSource,CHAR *szDest);
int  EncodeBase64(CHAR *szSource,CHAR *szDest,int iLen);
int  DecodeBase64(CHAR *szSource,CHAR *szDest);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_QUOTED_BASE64_H_
