/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_LANGUAGE_H_
#define _WSP_LANGUAGE_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
//常规 Language
//

#define	LANGUAGE_ANY				0 //

#define	LANGUAGE_CHINESE			0x41 //"zh"  "Chinese"
#define	LANGUAGE_ENGLISH			0x19 //"en"  "English"
#define	LANGUAGE_JAPANESE			0x36 //"ja"  "Japanese"

//
// Language Assignments table
//

//字段 Name与Number对应的 结构
typedef	struct	_LANG_ASSIGN
{
	char		szName[64];
	char		szShort[4];
	DWORD		dwNum;

} LANG_ASSIGN;

//字段 表
extern	const	LANG_ASSIGN	g_lpWell_Know_Lang_Assign_Tab[];
extern	const	DWORD	g_dwWell_Know_Lang_Assign_Max;

//字段 查找函数
extern	BOOL	VarFind_LangAssign( DWORD dwFlag, IN OUT LANG_ASSIGN* lpLangAssign );
#define	LANGFIND_NUM_BYSHORT		1
#define	LANGFIND_NUM_BYLANG			2
#define	LANGFIND_LAND_BYNUM			3
#define	LANGFIND_LAND_BYSHORT		4
#define	LANGFIND_SHORT_BYNUM		5
#define	LANGFIND_SHORT_BYLANG		6


//
// Accept-Language Field define
//

typedef	struct	_ACCEPT_LANGUAGE
{
	INTEGER_STR			stType;

	BOOL				fQValue;
	DWORD				dwQValue;

} ACCEPT_LANGUAGE;

extern	BOOL	VarDec_AcceptLanguage( DATA_DEAL* lpDealData, OUT ACCEPT_LANGUAGE* lpAcceptLanguage );
extern	BOOL	VarEnc_AcceptLanguage( OUT DATA_DEAL* lpDealData, ACCEPT_LANGUAGE* lpAcceptLanguage );
extern	void	VarFree_AcceptLanguage( ACCEPT_LANGUAGE* lpAcceptLanguage );


	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_LANGUAGE_H_

