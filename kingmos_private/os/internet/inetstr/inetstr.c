/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：
版本号：  1.0.0
开发时期：2004-09-13
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <internet.h>


/***************  全局区 定义， 声明 *****************/


const	char	INET_CRLF    []="\r\n";

const	char	HTTP_SCHEME  []="http:";
const	char	HTTP_SCHEME2 []="http";
const	char	HTTP_VERSION []="HTTP/1.1";
const	char	HTTP_GET     []="GET";
const	char	HTTP_END     []="\r\n\r\n";

const	char	HTTP_ACCEPT               []="Accept";             // ; Section 14.1
const	char	HTTP_ACCEPT_CHARSET       []="Accept-Charset";     // ; Section 14.2
const	char	HTTP_ACCEPT_ENCODING      []="Accept-Encoding";    // ; Section 14.3
const	char	HTTP_ACCEPT_LANGUAGE      []="Accept-Language";    // ; Section 14.4
const	char	HTTP_AUTHORIZATION        []="Authorization";      // ; Section 14.8
const	char	HTTP_EXPECT               []="Expect";             // ; Section 14.20
const	char	HTTP_FROM                 []="From";               // ; Section 14.22
const	char	HTTP_HOST                 []="Host";               // ; Section 14.23
const	char	HTTP_IF_MATCH             []="If-Match";           // ; Section 14.24
const	char	HTTP_IF_MODIFIED_SINCE    []="If-Modified-Since";  // ; Section 14.25
const	char	HTTP_IF_NONE_MATCH        []="If-None-Match";      // ; Section 14.26
const	char	HTTP_IF_RANGE             []="If-Range";           // ; Section 14.27
const	char	HTTP_IF_UNMODIFIED_SINCE  []="If-Unmodified-Since";// ; Section 14.28
const	char	HTTP_MAX_FORWARDS         []="Max-Forwards";       // ; Section 14.31
const	char	HTTP_PROXY_AUTHORIZATION  []="Proxy-Authorization";// ; Section 14.34
const	char	HTTP_RANGE                []="Range";              // ; Section 14.35
const	char	HTTP_REFERER              []="Referer";            // ; Section 14.36
const	char	HTTP_TE                   []="TE";                 // ; Section 14.39
const	char	HTTP_USER_AGENT           []="User-Agent";         // ; Section 14.43
// Add
const	char	HTTP_LOCATION             []="Location";           // ; Section 14.43


const	char	HTTP_ALLOW                []="Allow";              // ; Section 14.7
const	char	HTTP_CONTENT_ENCODING     []="Content-Encoding";   // ; Section 14.11
const	char	HTTP_CONTENT_LANGUAGE     []="Content-Language";   // ; Section 14.12
const	char	HTTP_CONTENT_LENGTH       []="Content-Length";     // ; Section 14.13
const	char	HTTP_CONTENT_LOCATION     []="Content-Location";   // ; Section 14.14
const	char	HTTP_CONTENT_MD5          []="Content-MD5";        // ; Section 14.15
const	char	HTTP_CONTENT_RANGE        []="Content-Range";      // ; Section 14.16
const	char	HTTP_CONTENT_TYPE         []="Content-Type";       // ; Section 14.17
const	char	HTTP_EXPIRES              []="Expires";            // ; Section 14.21
const	char	HTTP_LAST_MODIFIED        []="Last-Modified";      // ; Section 14.29


/******************************************************/

