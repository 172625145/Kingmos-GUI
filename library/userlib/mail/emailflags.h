/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef _EMAIL_FLAGS_H_
#define _EMAIL_FLAGS_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  

#define CRLF  0x0D0x0A  
#define CR   0x0D
#define LF   0x0A      
#define DOT  0x2E      // A char '.'

#define SUCCESS_SOCKET_ERROR     0
#define SERVER_NOT_READY         1
#define USER_SOCKET_ERROR        2
#define PASS_SOCKET_ERROR        3
#define UNKOWN_SOCKET_ERROR      4
#define SEND_SOCKET_ERROR        5
#define RECV_SOCKET_ERROR        6
#define FORMAT_SOCKET_ERROR      7
#define PARSEMAIL_SOCKET_ERROR   8
#define SERVER_NOT_SUPPORT       9
#define USER_INVALID             10
#define TO_NOT_KNOWN             11
#define SERVE_NOT_RECEIVE        12
#define SERVER_TIME_OUT          13
#define AUTH_PASS_ERROR          15
#define SERVER_LINK_ERROR        16
#define EMAIL_NOTEXIST           17
#define SYSTEM_ERROR_UNKOWN      18
#define INVALID_ADDRESS          19
#define NOT_MEMORY_AFFORD        20
#define SMTP_USER_INVALID        21
#define SMTP_PASSWORD_INVALID    22
#define ADDRESS_USER_INVALID     23 
#define NEWMAIL_NOTEXIST         24
#define ADDRESS_NOT_ACCEPT       25
#define MESSAGE_NOT_EXIST        26
//#define READ_ERROR_RETRY         27
	
#define INVALID_VALUE_FILE       100  

#define FILE_ERROR_EXCEPTION     101
#define ERROR_NOT_CONFIG         102
#define ERROR_CONFIG_SMTP        103
#define INVALID_EMAIL_ADDRESS    104
#define NOT_DISK_SPACE           105
#define TEXT_TYPE 1
#define MULTIPART_TYPE 2
#define NONE_TYPE 3

#define Quoted 1
#define Base64 2
#define NONE_FORMAT 3  // include 7bit,8bit.

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_EMAIL_FLAGS_H_
