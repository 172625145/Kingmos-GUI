/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef _WSP_CHAR_SET_H_
#define _WSP_CHAR_SET_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
//���� charset
//

#define	CHARSET_ANY					0      //
#define	CHARSET_BIG5				0x07EA //"big5"
#define	CHARSET_ISO_8859_1			0x04   //"iso-8859-1"
#define	CHARSET_ISO_8859_2			0x05   //"iso-8859-2"
#define	CHARSET_ISO_8859_3			0x06   //"iso-8859-3"
#define	CHARSET_US_ASCII			0x03   //"us-ascii"
#define	CHARSET_UTF_8				0x6A   //"utf-8"
//#define	CHARSET_GSM_DEF				0


//
// charset Assignments table
//

//�ֶ� Name��Number��Ӧ�� �ṹ
typedef	struct	_CHARSET_ASSIGN
{
	char		szName[64];
	DWORD		dwNum;

} CHARSET_ASSIGN;

//�ֶ� ��
extern	const	CHARSET_ASSIGN	g_lpWell_Know_Charset_Assign_Tab[];
extern	const	DWORD	g_dwWell_Know_Charset_Assign_Max;

//�ֶ� ���Һ���
extern	BOOL	VarFind_Charset( DWORD dwFlag, IN OUT CHARSET_ASSIGN* lpCharsetAssign );



//
// Accept-Charset Field define
//

typedef	struct	_ACCEPT_CHARSET
{
	INTEGER_STR			stType;

	BOOL				fQValue;
	DWORD				dwQValue;

} ACCEPT_CHARSET;


extern	BOOL	VarDec_AcceptCharset( DATA_DEAL* lpDealData, OUT ACCEPT_CHARSET* lpAcceptCharset );
extern	BOOL	VarEnc_AcceptCharset( OUT DATA_DEAL* lpDealData, ACCEPT_CHARSET* lpAcceptCharset );
extern	void	VarFree_AcceptCharset( ACCEPT_CHARSET* lpAcceptCharset );


	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_CHAR_SET_H_

