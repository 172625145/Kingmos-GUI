/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：keyboard API库
版本号：1.0.0.456
开发时期：2004-03-08
作者：陈建明 JAMI CHEN
修改记录：
**************************************************/
#include <eframe.h>
#include <eapisrv.h>

#include <epcore.h>


enum{
    KEYBD_CREATEKEYBOARD = 1,
    KEYBD_SHOWKEYBOAD,
    KEYBD_VKKEYSCAN,
    KEYBD_MAPVIRTUALKEY,
    KEYBD_SETKEYSTATE,
};



// **************************************************
// 声明：VOID WINAPI KB_CreateKeyboad(HINSTANCE hInstance, HWND hParent, int yPos)
// 参数：
//		hInstance -- 实例句柄
//		hParent	--   窗口句柄
//		yPos	--	 窗口创建的位置
// 返回值：
// 功能描述：创建键盘。
// 引用: 
// **************************************************
typedef HWND (WINAPI * PCREATEKEYBOAD)(HINSTANCE hInstance, HWND hParent, int yPos);
HWND WINAPI KB_CreateKeyboad(HINSTANCE hInstance, HWND hParent, int yPos)
{
	HWND hWnd;
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KEYBD, KEYBD_CREATEKEYBOARD, 3 );
    cs.arg0 = (DWORD)hInstance;
    hWnd = CALL_SERVER( &cs, hParent, yPos);
	return hWnd;
#else
	PCREATEKEYBOAD pCreateKeyboad;

	CALLSTACK cs;

	if( API_Enter( API_KEYBD, KEYBD_CREATEKEYBOARD, &pCreateKeyboad, &cs ) )  // 得到API函数指针
	{
		hWnd = pCreateKeyboad(hInstance, hParent, yPos);  // 呼叫API函数
		API_Leave(  );
	}
	return hWnd;
#endif
}



// **************************************************
// 声明：VOID WINAPI KB_ShowKeyboard(HWND hWnd,DWORD dwFlag)
// 参数：
//	hWnd --窗口句柄
//	dwFlag -- 显示标志
// 返回值：
// 功能描述：显示键盘
// 引用: 
// **************************************************
typedef BOOL (WINAPI * PSHOWKEYBOARD)(HWND hWnd,DWORD dwFlag);
VOID WINAPI KB_ShowKeyboard(HWND hWnd,DWORD dwFlag)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KEYBD, KEYBD_SHOWKEYBOAD, 2 );
    cs.arg0 = (DWORD)hWnd;
    CALL_SERVER( &cs,dwFlag);
	return ;
#else
	PSHOWKEYBOARD pShowKeyboard;

	CALLSTACK cs;

	if( API_Enter( API_KEYBD, KEYBD_SHOWKEYBOAD, &pShowKeyboard, &cs ) )  // 得到API函数指针
	{
		pShowKeyboard(hWnd,dwFlag);  // 呼叫API函数
		API_Leave(  );
	}
	return;
#endif
}


// **************************************************
// 声明：SHORT WINAPI KB_VkKeyScan(TCHAR uChar)
// 参数：
// 	IN uChar -- 指定字符
// 
// 返回值：返回字符所对应的虚键值
// 功能描述：得到字符所对应的虚键值。
// 引用: 
// **************************************************
typedef SHORT (WINAPI * PVKKEYSCAN)(TCHAR uChar);
SHORT WINAPI KB_VkKeyScan(TCHAR uChar)
{
	SHORT wVk;
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KEYBD, KEYBD_VKKEYSCAN, 1 );
    cs.arg0 = (DWORD)uChar;
    wVk = CALL_SERVER( &cs);
	return wVk;
#else
	PVKKEYSCAN pVkKeyScan;

	CALLSTACK cs;

	if( API_Enter( API_KEYBD, KEYBD_VKKEYSCAN, &pVkKeyScan, &cs ) )  // 得到API函数指针
	{
		wVk = pVkKeyScan(uChar);  // 呼叫API函数
		API_Leave(  );
	}
	return wVk;
#endif
}


// **************************************************
// 声明：UINT WINAPI KB_MapVirtualKey(UINT uCode, UINT uMapType)
// 参数：
// 	IN uCode -- 需要映射的代码
// 	IN uMapType - 映射类型
// 
// 返回值：返回映射后的代码
// 功能描述：映射虚键值。
// 引用: 
// **************************************************
typedef UINT (WINAPI * PMAPVIRTUALKEY)(UINT uCode, UINT uMapType);
UINT WINAPI KB_MapVirtualKey(UINT uCode, UINT uMapType)
{
	UINT dwVk;
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KEYBD, KEYBD_MAPVIRTUALKEY, 2 );
    cs.arg0 = (DWORD)uCode;
    dwVk = CALL_SERVER( &cs,uMapType);
	return dwVk;
#else
	PMAPVIRTUALKEY pMapVirtualKey;

	CALLSTACK cs;

	if( API_Enter( API_KEYBD, KEYBD_MAPVIRTUALKEY, &pMapVirtualKey, &cs ) )  // 得到API函数指针
	{
		dwVk = pMapVirtualKey(uCode,uMapType);  // 呼叫API函数
		API_Leave(  );
	}
	return dwVk;
#endif
}


// **************************************************
// 声明：void WINAPI KB_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
// 参数：
// 	IN KeyEvent -- 键盘事件
// 	IN bDownUp -- 按下状态
// 
// 返回值：无
// 功能描述：设置系统键的状态。
// 引用: 
// **************************************************
typedef void (WINAPI *PSETKEYSTATE)( UINT8	KeyEvent,BOOL  bDownUp);
void WINAPI KB_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_KEYBD, KEYBD_SETKEYSTATE, 2 );
    cs.arg0 = (DWORD)KeyEvent;
    CALL_SERVER( &cs,bDownUp);
	return ;
#else
	PSETKEYSTATE pSetKeyState;

	CALLSTACK cs;

	if( API_Enter( API_KEYBD, KEYBD_SETKEYSTATE, &pSetKeyState, &cs ) )  // 得到API函数指针
	{
		pSetKeyState(KeyEvent,bDownUp);  // 呼叫API函数
		API_Leave(  );
	}
	return ;
#endif
}
