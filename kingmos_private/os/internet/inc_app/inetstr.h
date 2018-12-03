/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _KINGMOS_INETSTR_H_
#define _KINGMOS_INETSTR_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ------------------------------------------------
// 数据类型、长度宏、字符串定义
// ------------------------------------------------

extern	const	char	HTTP_ACCEPT               [];//="Accept";             // ; Section 14.1
extern	const	char	HTTP_ACCEPT_CHARSET       [];//="Accept-Charset";     // ; Section 14.2
extern	const	char	HTTP_ACCEPT_ENCODING      [];//="Accept-Encoding";    // ; Section 14.3
extern	const	char	HTTP_ACCEPT_LANGUAGE      [];//="Accept-Language";    // ; Section 14.4
extern	const	char	HTTP_AUTHORIZATION        [];//="Authorization";      // ; Section 14.8
extern	const	char	HTTP_EXPECT               [];//="Expect";             // ; Section 14.20
extern	const	char	HTTP_FROM                 [];//="From";               // ; Section 14.22
extern	const	char	HTTP_HOST                 [];//="Host";               // ; Section 14.23
extern	const	char	HTTP_IF_MATCH             [];//="If-Match";           // ; Section 14.24
extern	const	char	HTTP_IF_MODIFIED_SINCE    [];//="If-Modified-Since";  // ; Section 14.25
extern	const	char	HTTP_IF_NONE_MATCH        [];//="If-None-Match";      // ; Section 14.26
extern	const	char	HTTP_IF_RANGE             [];//="If-Range";           // ; Section 14.27
extern	const	char	HTTP_IF_UNMODIFIED_SINCE  [];//="If-Unmodified-Since";// ; Section 14.28
extern	const	char	HTTP_MAX_FORWARDS         [];//="Max-Forwards";       // ; Section 14.31
extern	const	char	HTTP_PROXY_AUTHORIZATION  [];//="Proxy-Authorization";// ; Section 14.34
extern	const	char	HTTP_RANGE                [];//="Range";              // ; Section 14.35
extern	const	char	HTTP_REFERER              [];//="Referer";            // ; Section 14.36
extern	const	char	HTTP_TE                   [];//="TE";                 // ; Section 14.39
extern	const	char	HTTP_USER_AGENT           [];//="User-Agent";         // ; Section 14.43
extern const	char	HTTP_LOCATION             [];//="Location";           // ; Section 14.43

extern	const	char	HTTP_ALLOW                [];//="Allow";              // ; Section 14.7
extern	const	char	HTTP_CONTENT_ENCODING     [];//="Content-Encoding";   // ; Section 14.11
extern	const	char	HTTP_CONTENT_LANGUAGE     [];//="Content-Language";   // ; Section 14.12
extern	const	char	HTTP_CONTENT_LENGTH       [];//="Content-Length";     // ; Section 14.13
extern	const	char	HTTP_CONTENT_LOCATION     [];//="Content-Location";   // ; Section 14.14
extern	const	char	HTTP_CONTENT_MD5          [];//="Content-MD5";        // ; Section 14.15
extern	const	char	HTTP_CONTENT_RANGE        [];//="Content-Range";      // ; Section 14.16
extern	const	char	HTTP_CONTENT_TYPE         [];//="Content-Type";       // ; Section 14.17
extern	const	char	HTTP_EXPIRES              [];//="Expires";            // ; Section 14.21
extern	const	char	HTTP_LAST_MODIFIED        [];//="Last-Modified";      // ; Section 14.29




#define	LEN_HTTP_ACCEPT				        6  //(sizeof(HTTP_ACCEPT              )-1)
#define	LEN_HTTP_ACCEPT_CHARSET             14 //(sizeof(HTTP_ACCEPT_CHARSET      )-1)
#define	LEN_HTTP_ACCEPT_ENCODING            15 //(sizeof(HTTP_ACCEPT_ENCODING     )-1)
#define	LEN_HTTP_ACCEPT_LANGUAGE            15 //(sizeof(HTTP_ACCEPT_LANGUAGE     )-1)
#define	LEN_HTTP_AUTHORIZATION              13 //(sizeof(HTTP_AUTHORIZATION       )-1)
#define	LEN_HTTP_EXPECT                     6  //(sizeof(HTTP_EXPECT              )-1)
#define	LEN_HTTP_FROM                       4  //(sizeof(HTTP_FROM                )-1)
#define	LEN_HTTP_HOST                       4  //(sizeof(HTTP_HOST                )-1)
#define	LEN_HTTP_IF_MATCH                   8  //(sizeof(HTTP_IF_MATCH            )-1)
#define	LEN_HTTP_IF_MODIFIED_SINCE          17 //(sizeof(HTTP_IF_MODIFIED_SINCE   )-1)
#define	LEN_HTTP_IF_NONE_MATCH              13 //(sizeof(HTTP_IF_NONE_MATCH       )-1)
#define	LEN_HTTP_IF_RANGE                   8  //(sizeof(HTTP_IF_RANGE            )-1)
#define	LEN_HTTP_IF_UNMODIFIED_SINCE        19 //(sizeof(HTTP_IF_UNMODIFIED_SINCE )-1)
#define	LEN_HTTP_MAX_FORWARDS               12 //(sizeof(HTTP_MAX_FORWARDS        )-1)
#define	LEN_HTTP_PROXY_AUTHORIZATION        19 //(sizeof(HTTP_PROXY_AUTHORIZATION )-1)
#define	LEN_HTTP_RANGE                      5  //(sizeof(HTTP_RANGE               )-1)
#define	LEN_HTTP_REFERER                    7  //(sizeof(HTTP_REFERER             )-1)
#define	LEN_HTTP_TE                         2  //(sizeof(HTTP_TE                  )-1)
#define	LEN_HTTP_USER_AGENT                 10 //(sizeof(HTTP_USER_AGENT          )-1)
#define	LEN_HTTP_LOCATION                   8 //(sizeof(HTTP_LOCATION          )-1)


#define	LEN_HTTP_ALLOW                      5  //
#define	LEN_HTTP_CONTENT_ENCODING           16 //
#define	LEN_HTTP_CONTENT_LANGUAGE           16 //
#define	LEN_HTTP_CONTENT_LENGTH             14 //
#define	LEN_HTTP_CONTENT_LOCATION           16 //
#define	LEN_HTTP_CONTENT_MD5                11 //
#define	LEN_HTTP_CONTENT_RANGE              13 //
#define	LEN_HTTP_CONTENT_TYPE               12 //
#define	LEN_HTTP_EXPIRES                    7  //
#define	LEN_HTTP_LAST_MODIFIED              13 //




#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //_KINGMOS_INETSTR_H_
