/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _WSP_PUSH_APP_ID_H_
#define _WSP_PUSH_APP_ID_H_

#ifndef _WSP_VAR_TYPE_H_
#include "wsp_vartype.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


//
//常规 PushAppID
//

#define	PUSHAPPID_WML_UA			0x02
#define	PUSHAPPID_MMS_UA			0x04


//
// PushAppID Assignments table
//

//字段 Name与Number对应的 结构
#define	LEN_WSP_PUSHAPPID_NAME		64
typedef	struct	_PAPPID_ASSIGN
{
	char		szName[LEN_WSP_PUSHAPPID_NAME];
	DWORD		dwNum;

} PAPPID_ASSIGN;

//字段 表
extern	const	PAPPID_ASSIGN	g_lpWell_Know_PushAppID_Tab[];
extern	const	DWORD	g_dwWell_Know_PushAppID_Max;

//字段 查找函数
extern	BOOL	VarFind_PAppIDAssign( DWORD dwFlag, IN OUT PAPPID_ASSIGN* lpPAppIDAssign );
#define	PAPPIDFIND_NAME_BYNUM			1
#define	PAPPIDFIND_NUM_BYNAME			2


//类型规则：
//Application-id-value = Uri-value | App-assigned-code
//App-assigned-code = Integer-value
#define	VarDec_PushAppID				VarDec_IntegerStr
#define	VarEnc_PushAppID				VarEnc_IntegerStr
#define	VarFree_PushAppID				VarFree_IntegerStr
#define	VarLen_PushAppID				VarLen_IntegerStr
#define	VarMap_PushAppID				VarMap_IntegerStr
#define	VarClone_PushAppID				VarClone_IntegerStr

extern	BOOL	VarCmp_PushAppID( INTEGER_STR* lpPushAppID1, INTEGER_STR* lpPushAppID2 );

	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_WSP_PUSH_APP_ID_H_
