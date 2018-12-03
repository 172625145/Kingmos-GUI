/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
// ****************************************************
// Note:
// 得到系统是否有短信、彩新和未接电话，信息箱是否满
// ****************************************************

#ifndef __MNEWMSG_H
#define __MNEWMSG_H

// 反映系统当前有没有新的信息（包括短信、彩信）
typedef	struct	_NEWMSG{
	UINT	nSize;			// = sizeof(SYSNEWMSG)
	int		iNewSms		:8;	// 有几条未读的短信
	int		iNewMms		:8;	// 有几条未读的彩信
	int		iNewPhone	:8;	// 有几条未接且未读的电话
	BOOL	bSmsFull	:4;	// 短信信息箱是否满
	BOOL	bMmsFull	:4;	// 彩信信息箱是否满
	BYTE	bSQ;			// 信号强度
}SYSNEWMSG, FAR * LPSYSNEWMSG;

#endif //__MNEWMSG_H