/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：创建逻辑字体
版本号：2.0.0
开发时期：1999
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <gdc.h>

extern HFONT _CreateSysFont( UINT id );

// **************************************************
// 声明：HFONT WINAPI WinFont_CreateIndirect( CONST LOGFONT *lplf )
// 参数：
// 	IN lplf - LOGFONT 结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************
/*
HFONT WINAPI WinFont_CreateIndirect( CONST LOGFONT *lplf )
{
	return _CreateSysFont(SYSTEM_FONT);
}
*/