/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_VAR_TYPE_H_
#define _WSP_VAR_TYPE_H_

#ifndef	_VAR_BUFFER_H_
#include <varbuf.h>
#endif	//_VAR_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// -----------------------------------------------------------
// 基本数据类型 定义
// -----------------------------------------------------------
//
//数据类型定义
//
//0 - 30     This octet is followed by the indicated number (0 –30) of data octets
//31         This octet is followed by a uintvar, which indicates the number of data octets after it
//32 - 127   The value is a text string, terminated by a zero octet (NUL character)
//128 - 255  It is an encoded 7-bit value; this header has no more data
//

typedef	BYTE						OCTET;
typedef	BYTE						SHORTINT;
typedef	BYTE						UNTYPE;

//
//#define	OCTET_ISCTL(b)				( (b<=31) || (b==127) )
//#define	OCTET_ISCTL(b)				( (b<=31) )
//#define	OCTET_ISLWS(b)				( (b==13) || (b==10) || (b==32) || (b==9) )
//#define	OCTET_ISTEXT(b)				( ((b>31) && (b<127)) || OCTET_ISLWS(b) )
//#define	OCTET_ISTEXT(b)				( ((b>31) && (b<=127)) || OCTET_ISLWS(b) )

//0~30, 31
#define	OCTET_LENQUOTE				31
#define	OCTET_IS_ShortLen(b)		( (b<=30) )
#define	OCTET_IS_LenQuote(b)		( (b==31) )
#define	OCTET_IS_ValueLen(b)		( (b<=31) )
#define	OCTET_NOT_ValueLen(b)		( (b>31) )

#define	OCTET_IS_LongInt(b)			( b<=30 )

//32~127
#define	OCTET_TEXTQUOTE				34
#define	OCTET_IS_Text(b)			( (b>=32) && (b<=127) )

//128
#define	OCTET_SHORTINT				0x80
#define	OCTET_IS_ShortInt(b)		( b & 0x80 )

#define	OCTET_UINTVAR				0x80
#define	OCTET_IS_UintVar(b)			( b & 0x80 )

//
#define	OCTET_IS_Integer(b)			( OCTET_IS_ShortInt(b) || OCTET_IS_LongInt(b) )
#define	OCTET_IS_ConstrainedEnc(b)	( (b>31) )


//
//定义每种类型的最大长度
//

//基本类型

#define	LEN_UINTVAR					5

#define	LEN_LENVAL					(LEN_UINTVAR + 1)

#define	LEN_SHORTINT				1
#define	LEN_LONGINT					(4 + 1)
#define	LEN_INTEGER					LEN_LONGINT

//复合类型

#define	LEN_SHORT_A_INT				(LEN_LENVAL + LEN_SHORTINT + LEN_INTEGER)

#define	LEN_DATE_A_DELTASEC			(LEN_LENVAL + LEN_SHORTINT + LEN_INTEGER)

#define	LEN_INTEGER_A_SHORT			(LEN_LENVAL + LEN_SHORTINT + LEN_INTEGER)

#define	LEN_INTEGER_A_LONG			(LEN_LENVAL + LEN_LONGINT + LEN_INTEGER)

#define	LEN_INTEGER_A_INT			(LEN_LENVAL + LEN_INTEGER + LEN_INTEGER)


//
#define	LEN_MAX_INT					LEN_INTEGER_A_INT



//通用BODY
typedef	struct	_GEN_BODY
{
	WORD			wFlag;
	WORD			wXXX;

	LPBYTE			pBody;
	DWORD			dwLenBody;

} GEN_BODY;

//wFlag
#define	BODYF_ALLOC					0	//表示 pBody是 alloc分配的
#define	BODYF_COPYPTR				1	//表示 pBody是 copy某个buffer其中的地址位置
#define	BODYF_NOUSED				2	//表示 pBody是 不起作用

#define	BODYF_DEFAULT				BODYF_ALLOC



// -----------------------------------------------------------
// 复合数据类型 定义
// -----------------------------------------------------------


//
// 求或的
//

//Varibile: Short-integer | Text-string
typedef	struct	_SHORT_STR
{
	BOOL			fIsString;
	union
	{
		SHORTINT		bVal;
		LPSTR			pszString;
	};

} SHORT_STR;

//Varibile: Long-integer | Text-string
typedef	struct	_LONG_STR
{
	BOOL			fIsString;
	union
	{
		DWORD		dwVal;
		LPSTR		pszString;
	};

} LONG_STR;

//Varibile: Integer-value | Text-string
typedef	struct	_INTEGER_STR
{
	BOOL			fIsString;
	union
	{
		DWORD		dwVal;
		LPSTR		pszString;
	};

} INTEGER_STR;


//
// 并联的
//

//Varibile:  Value-length BUFFER
typedef	struct	_LENGTH_BUF
{
	DWORD			dwLenVal;
	LPBYTE			pBuf;

} LENGTH_BUF;

//Varibile: Value-length Short-integer Integer-value
typedef	struct	_SHORT_A_INT
{
	SHORTINT		bVal1;
	DWORD			dwVal2;

} SHORT_A_INT;

//Varibile: Value_length (Absolute_token Date_value | Relative_token Delta_seconds_value)
typedef	struct	_DATE_A_DELTASEC
{
	SHORTINT		bToken;
	DWORD			dwDate;

} DATE_A_DELTASEC;
//bToken of DATE_A_DELTASEC
#define	Date_Absolute						(0x80 -OCTET_SHORTINT)
#define	Date_Relative						(0x81 -OCTET_SHORTINT)

//Varibile: Value-length Integer-value Short-integer
typedef	struct	_INTEGER_A_SHORT
{
	DWORD			dwVal1;
	SHORTINT		bVal2;

} INTEGER_A_SHORT;

//Varibile: Value-length Integer-value Long-integer
typedef	struct	_INTEGER_A_LONG
{
	DWORD			dwVal1;
	DWORD			dwVal2;

} INTEGER_A_LONG;

//Varibile: Value-length Integer-value Integer-value
typedef	struct	_INTEGER_A_INT
{
	DWORD			dwVal1;
	DWORD			dwVal2;

} INTEGER_A_INT;

//Varibile: Value-length Integer-value Text-string
typedef	struct	_INTEGER_A_STR
{
	DWORD			dwVal;
	LPSTR			pszString;

} INTEGER_A_STR;

//Varibile: Encoded-string-value = Text-string | Value-length Char-set Text-string
typedef	struct	_ENCODED_STR
{
	DWORD			dwCharset;
	LPSTR			pszString;

} ENCODED_STR;

//Varibile: Value-length Short-integer Encoded-string-value
typedef	struct	_SHORT_A_ENCSTR
{
	SHORTINT		bVal;
	ENCODED_STR		stEncodedStr;

} SHORT_A_ENCSTR;

//Varibile: Value-length Integer-value Encoded-string-value
typedef	struct	_INTEGER_A_ENCSTR
{
	DWORD			dwVal;
	ENCODED_STR		stEncodedStr;

} INTEGER_A_ENCSTR;



// -----------------------------------------------------------
// 数据类型的操作函数 定义
// -----------------------------------------------------------

//
//操作函数 的前缀说明
//
//
// 1。原始BUFFER结构的操作：VarBuf
//
//                                     新进程操作
//                                       +    
//                                       |    
//                                       | VarMap 
//               VarDec/VarPeek          |                   VarShow
//               -------------------->   |            -------------------->
// 2。原始BUFFER                       解码后字段数值                       界面可视数据
//               <--------------------   |           <---------------------
//               VarEnc/VarLen         / |         \         VarMake
//                                    /  | VarCopy  \ 
//                            VarCmp /   | VarMove   \VarFree
//                                  /    | VarClone   \
//                                 +     +             +
//                    解码后字段数值2   新的备份       释放
//
//              VarFind
// 3。字段Name <-------------> 字段Number
//
// 4。公布的字段信息 <Name and Number (alose Version)>:  
//    ArrayTable(g_lpWell_Know_),  Element Count(g_dwWell_Know_)
//


//
//基本数据类型的 操作
//
extern	BOOL	VarDec_ShortInt( DATA_DEAL* lpDealData, OUT SHORTINT* pbValue );
extern	BOOL	VarEnc_ShortInt( DATA_DEAL* lpDealData, SHORTINT bValue );
#define	VarLen_ShortInt(p,b)	LEN_SHORTINT

extern	BOOL	VarDec_UintVar( DATA_DEAL* lpDealData, OUT DWORD* pdwValue );
extern	BOOL	VarEnc_UintVar( DATA_DEAL* lpDealData, DWORD dwValue );
#define	VarLen_UintVar(p,b)		LEN_UINTVAR

extern	BOOL	VarDec_LongInt( DATA_DEAL* lpDealData, OUT DWORD* pdwValue );
extern	BOOL	VarEnc_LongInt( DATA_DEAL* lpDealData, DWORD dwValue );
#define	VarLen_LongInt(p,b)		LEN_LONGINT

extern	BOOL	VarDec_Integer( DATA_DEAL* lpDealData, OUT DWORD* pdwValue );
extern	BOOL	VarEnc_Integer( DATA_DEAL* lpDealData, DWORD dwValue );
#define	VarLen_Integer(p,b)		LEN_INTEGER

extern	BOOL	VarDec_LenVal( DATA_DEAL* lpDealData, OUT DWORD* pdwValue );
extern	BOOL	VarEnc_LenVal( DATA_DEAL* lpDealData, DWORD dwValue );
extern	BOOL	VarEnc_LenValBuf( DATA_DEAL* lpDealData, DATA_DEAL* lpDealData_Buf );

extern	BOOL	VarDec_TextStr( DATA_DEAL* lpDealData, OUT LPSTR* ppszString );
extern	BOOL	VarDec_TextStrByLen( DATA_DEAL* lpDealData, DWORD dwLenRead, BOOL fMustBeLen, OUT LPSTR* ppszString );
extern	BOOL	VarEnc_TextStr( DATA_DEAL* lpDealData, LPSTR pszString );
extern	void	VarFree_TextStr( LPSTR pszString );
extern	void	VarFree2_TextStr( LPSTR* ppszString );
extern	DWORD	VarLen_TextStr( LPSTR pszString );
extern	void	VarMap_TextStr( IN OUT LPSTR* ppszString );
extern	LPSTR	VarMap2_TextStr( LPSTR pszString );
extern	BOOL	VarClone_TextStr( OUT LPSTR* ppszStringDes, LPSTR pszStringSrc );
extern	void	VarMove_TextStr( OUT LPSTR* ppszStringDes, IN OUT LPSTR* ppszStringSrc );

extern	BOOL	VarDec_QuotedStr( DATA_DEAL* lpDealData, OUT LPSTR* ppszString );
extern	BOOL	VarEnc_QuotedStr( DATA_DEAL* lpDealData, LPSTR pszString );
#define	VarFree_QuotedStr		VarFree_TextStr
#define	VarFree2_QuotedStr		VarFree2_TextStr
#define	VarLen_QuotedStr		VarLen_TextStr

extern	BOOL	VarDec_GenBody( DATA_DEAL* lpDealData, OUT GEN_BODY* lpBodyGen );
extern	BOOL	VarEnc_GenBody( OUT DATA_DEAL* lpDealData, GEN_BODY* lpBodyGen );
extern	void	VarFree_GenBody( GEN_BODY* lpBodyGen );
extern	DWORD	VarLen_GenBody( GEN_BODY* lpBodyGen );
extern	void	VarMap_GenBody( IN OUT GEN_BODY* lpBodyGen );
extern	void	VarMove_GenBody( OUT GEN_BODY* lpBodyGenDes, IN OUT GEN_BODY* lpBodyGenSrc );



//
//求或的---复合数据类型的 操作
//

extern	BOOL	VarDec_TextValue( DATA_DEAL* lpDealData, OUT LPSTR* ppszString );
extern	BOOL	VarEnc_TextValue( DATA_DEAL* lpDealData, LPSTR pszString );
#define	VarFree_TextValue		VarFree_TextStr
#define	VarFree2_TextValue		VarFree2_TextStr
#define	VarLen_TextValue		VarLen_TextStr

extern	BOOL	VarDec_ShortStr( DATA_DEAL* lpDealData, OUT SHORT_STR* lpShortStr );
extern	BOOL	VarEnc_ShortStr( DATA_DEAL* lpDealData, SHORT_STR* lpShortStr );
extern	void	VarFree_ShortStr( SHORT_STR* lpShortStr );
extern	DWORD	VarLen_ShortStr( SHORT_STR* lpShortStr );
extern	void	VarMove_ShortStr( OUT SHORT_STR* lpShortStrDes, IN OUT SHORT_STR* lpShortStrSrc );

extern	BOOL	VarDec_LongStr( DATA_DEAL* lpDealData, OUT LONG_STR* lpLongStr );
extern	BOOL	VarEnc_LongStr( DATA_DEAL* lpDealData, LONG_STR* lpLongStr );
extern	void	VarFree_LongStr( LONG_STR* lpLongStr );
extern	DWORD	VarLen_LongStr( LONG_STR* lpLongStr );
extern	void	VarMove_LongStr( OUT LONG_STR* lpLongStrDes, IN OUT LONG_STR* lpLongStrSrc );

extern	BOOL	VarDec_IntegerStr( DATA_DEAL* lpDealData, OUT INTEGER_STR* lpIntegerStr );
extern	BOOL	VarEnc_IntegerStr( DATA_DEAL* lpDealData, INTEGER_STR* lpIntegerStr );
extern	void	VarFree_IntegerStr( INTEGER_STR* lpIntegerStr );
extern	DWORD	VarLen_IntegerStr( INTEGER_STR* lpIntegerStr );
extern	void	VarMap_IntegerStr( IN OUT INTEGER_STR* lpIntegerStr );
extern	void	VarMap2_IntegerStr( OUT INTEGER_STR* lpIntegerStrDes, INTEGER_STR* lpIntegerStrSrc );
extern	DWORD	VarClone_IntegerStr( OUT INTEGER_STR* lpIntegerStrDes, INTEGER_STR* lpIntegerStrSrc );
extern	void	VarMove_IntegerStr( OUT INTEGER_STR* lpIntegerStrDes, INTEGER_STR* lpIntegerStrSrc );

//
//并联的---复合数据类型的 操作
//

extern	BOOL	VarDec_LenBuf( DATA_DEAL* lpDealData, OUT LENGTH_BUF* lpLenBuf );
extern	BOOL	VarEnc_LenBuf( DATA_DEAL* lpDealData, LENGTH_BUF* lpLenBuf );
extern	void	VarFree_LenBuf( LENGTH_BUF* lpLenBuf );
extern	DWORD	VarLen_LenBuf( LENGTH_BUF* lpLenBuf );

extern	BOOL	VarDec_ShortAndInt( DATA_DEAL* lpDealData, OUT SHORT_A_INT* lpShortAndInt );
extern	BOOL	VarEnc_ShortAndInt( DATA_DEAL* lpDealData, SHORT_A_INT* lpShortAndInt );
#define	VarLen_ShortAndInt(p,b)			LEN_SHORT_A_INT

extern	BOOL	VarDec_DateAndDeltasec( DATA_DEAL* lpDealData, OUT DATE_A_DELTASEC* lpDateAndDelta );
extern	BOOL	VarEnc_DateAndDeltasec( DATA_DEAL* lpDealData, DATE_A_DELTASEC* lpDateAndDelta );
#define	VarLen_DateAndDeltasec(p,b)		LEN_DATE_A_DELTASEC

extern	BOOL	VarDec_IntegerAndShort( DATA_DEAL* lpDealData, OUT INTEGER_A_SHORT* lpIntegerAndShort );
extern	BOOL	VarEnc_IntegerAndShort( DATA_DEAL* lpDealData, INTEGER_A_SHORT* lpIntegerAndShort );
#define	VarLen_IntegerAndShort(p,b)		LEN_INTEGER_A_SHORT

extern	BOOL	VarDec_IntegerAndLong( DATA_DEAL* lpDealData, OUT INTEGER_A_LONG* lpIntegerAndLong );
extern	BOOL	VarEnc_IntegerAndLong( DATA_DEAL* lpDealData, INTEGER_A_LONG* lpIntegerAndLong );
#define	VarLen_IntegerAndLong(p,b)		LEN_INTEGER_A_LONG

extern	BOOL	VarDec_IntegerAndInt( DATA_DEAL* lpDealData, OUT INTEGER_A_INT* lpIntegerAndInt );
extern	BOOL	VarEnc_IntegerAndInt( DATA_DEAL* lpDealData, INTEGER_A_INT* lpIntegerAndInt );
#define	VarLen_IntegerAndInt(p,b)		LEN_INTEGER_A_INT

extern	BOOL	VarDec_IntegerAndStr( DATA_DEAL* lpDealData, OUT INTEGER_A_STR* lpIntegerAndStr );
extern	BOOL	VarEnc_IntegerAndStr( DATA_DEAL* lpDealData, INTEGER_A_STR* lpIntegerAndStr );
extern	void	VarFree_IntegerAndStr( INTEGER_A_STR* lpIntegerAndStr );
extern	DWORD	VarLen_IntegerAndStr( INTEGER_A_STR* lpIntegerAndStr );

extern	BOOL	VarDec_EncodedStr( DATA_DEAL* lpDealData, OUT ENCODED_STR* lpEncodedStr );
extern	BOOL	VarEnc_EncodedStr( DATA_DEAL* lpDealData, ENCODED_STR* lpEncodedStr );
extern	void	VarFree_EncodedStr( ENCODED_STR* lpEncodedStr );
extern	DWORD	VarLen_EncodedStr( ENCODED_STR* lpEncodedStr );
extern	void	VarMap_EncodedStr( IN OUT ENCODED_STR* lpEncodedStr );
extern	void	VarMove_EncodedStr( OUT ENCODED_STR* lpEncodedStrDes, IN OUT ENCODED_STR* lpEncodedStrSrc );

extern	BOOL	VarDec_ShortAndEncStr( DATA_DEAL* lpDealData, OUT SHORT_A_ENCSTR* lpShortAndEncStr );
extern	BOOL	VarEnc_ShortAndEncStr( DATA_DEAL* lpDealData, SHORT_A_ENCSTR* lpShortAndEncStr );
extern	void	VarFree_ShortAndEncStr( SHORT_A_ENCSTR* lpShortAndEncStr );
extern	DWORD	VarLen_ShortAndEncStr( SHORT_A_ENCSTR* lpShortAndEncStr );
extern	void	VarMap_ShortAndEncStr( IN OUT SHORT_A_ENCSTR* lpShortAndEncStr );
extern	void	VarMove_ShortAndEncStr( OUT SHORT_A_ENCSTR* lpShortAndEncStrDes, IN OUT SHORT_A_ENCSTR* lpShortAndEncStrSrc );

extern	BOOL	VarDec_IntegerAndEncStr( DATA_DEAL* lpDealData, OUT INTEGER_A_ENCSTR* lpIntegerAndEncStr );
extern	BOOL	VarEnc_IntegerAndEncStr( DATA_DEAL* lpDealData, INTEGER_A_ENCSTR* lpIntegerAndEncStr );
extern	void	VarFree_IntegerAndEncStr( INTEGER_A_ENCSTR* lpIntegerAndEncStr );
extern	DWORD	VarLen_IntegerAndEncStr( INTEGER_A_ENCSTR* lpIntegerAndEncStr );



//
//别名数据类型的 操作
//

#define	VarDec_Date				VarDec_LongInt
#define	VarEnc_Date				VarEnc_LongInt
#define	VarLen_Date				VarLen_LongInt

#define	VarDec_Deltasec			VarDec_LongInt//(MMS)		//VarDec_Integer//(WSP)
#define	VarEnc_Deltasec			VarEnc_LongInt//(MMS)		//VarEnc_Integer//(WSP)
#define	VarLen_Deltasec			VarLen_LongInt//(MMS)		//VarEnc_Integer//(WSP)

#define	VarDec_Uri				VarDec_TextStr
#define	VarEnc_Uri				VarEnc_TextStr
#define	VarFree_Uri				VarFree_TextStr
#define	VarFree2_Uri			VarFree2_TextStr
#define	VarLen_Uri				VarLen_TextStr

//类型规则：
//Constrained-encoding = Extension-Media | Short-integer
//Extension-media = *TEXT End-of-string
#define	VarDec_ConstrainedEnc	VarDec_ShortStr
#define	VarEnc_ConstrainedEnc	VarEnc_ShortStr
#define	VarFree_ConstrainedEnc	VarFree_ShortStr
#define	VarLen_ConstrainedEnc	VarLen_ShortStr


//Q-value = 1*2 OCTET
#define	VarDec_QValue			VarDec_UintVar
#define	VarEnc_QValue			VarEnc_UintVar
#define	VarLen_QValue			VarLen_UintVar


//
//
//同样功能，不同原型的 数据类型操作
//
//

extern	DWORD	VarDec2_UintVar( LPBYTE pBufInt, OUT DWORD* pdwLen );
extern	BYTE	VarEnc2_UintVar( LPBYTE pBufInt, DWORD dwValue );

extern	DWORD	VarDec2_LongInt( LPBYTE pBufInt, OUT DWORD* pdwLen );
extern	BYTE	VarEnc2_LongInt( LPBYTE pBufInt, DWORD dwValue );

extern	DWORD	VarDec2_Integer( LPBYTE pBufInt, OUT DWORD* pdwLen );
extern	BYTE	VarEnc2_Integer( LPBYTE pBufInt, DWORD dwValue );

extern	DWORD	VarDec2_LenVal( LPBYTE pBufInt, OUT DWORD* pdwLen );
extern	BYTE	VarEnc2_LenVal( LPBYTE pBufInt, DWORD dwValue );


#ifdef XYG_PC_PRJ
extern	LPVOID	MapPtrToProcess( LPVOID lpv, HANDLE hProc );
extern	HANDLE	GetCallerProcess( );
#endif


	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_VAR_TYPE_H_
