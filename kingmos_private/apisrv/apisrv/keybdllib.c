/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵����keyboard API��
�汾�ţ�1.0.0.456
����ʱ�ڣ�2004-03-08
���ߣ��½��� JAMI CHEN
�޸ļ�¼��
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
// ������VOID WINAPI KB_CreateKeyboad(HINSTANCE hInstance, HWND hParent, int yPos)
// ������
//		hInstance -- ʵ�����
//		hParent	--   ���ھ��
//		yPos	--	 ���ڴ�����λ��
// ����ֵ��
// �����������������̡�
// ����: 
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

	if( API_Enter( API_KEYBD, KEYBD_CREATEKEYBOARD, &pCreateKeyboad, &cs ) )  // �õ�API����ָ��
	{
		hWnd = pCreateKeyboad(hInstance, hParent, yPos);  // ����API����
		API_Leave(  );
	}
	return hWnd;
#endif
}



// **************************************************
// ������VOID WINAPI KB_ShowKeyboard(HWND hWnd,DWORD dwFlag)
// ������
//	hWnd --���ھ��
//	dwFlag -- ��ʾ��־
// ����ֵ��
// ������������ʾ����
// ����: 
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

	if( API_Enter( API_KEYBD, KEYBD_SHOWKEYBOAD, &pShowKeyboard, &cs ) )  // �õ�API����ָ��
	{
		pShowKeyboard(hWnd,dwFlag);  // ����API����
		API_Leave(  );
	}
	return;
#endif
}


// **************************************************
// ������SHORT WINAPI KB_VkKeyScan(TCHAR uChar)
// ������
// 	IN uChar -- ָ���ַ�
// 
// ����ֵ�������ַ�����Ӧ�����ֵ
// �����������õ��ַ�����Ӧ�����ֵ��
// ����: 
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

	if( API_Enter( API_KEYBD, KEYBD_VKKEYSCAN, &pVkKeyScan, &cs ) )  // �õ�API����ָ��
	{
		wVk = pVkKeyScan(uChar);  // ����API����
		API_Leave(  );
	}
	return wVk;
#endif
}


// **************************************************
// ������UINT WINAPI KB_MapVirtualKey(UINT uCode, UINT uMapType)
// ������
// 	IN uCode -- ��Ҫӳ��Ĵ���
// 	IN uMapType - ӳ������
// 
// ����ֵ������ӳ���Ĵ���
// ����������ӳ�����ֵ��
// ����: 
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

	if( API_Enter( API_KEYBD, KEYBD_MAPVIRTUALKEY, &pMapVirtualKey, &cs ) )  // �õ�API����ָ��
	{
		dwVk = pMapVirtualKey(uCode,uMapType);  // ����API����
		API_Leave(  );
	}
	return dwVk;
#endif
}


// **************************************************
// ������void WINAPI KB_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
// ������
// 	IN KeyEvent -- �����¼�
// 	IN bDownUp -- ����״̬
// 
// ����ֵ����
// ��������������ϵͳ����״̬��
// ����: 
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

	if( API_Enter( API_KEYBD, KEYBD_SETKEYSTATE, &pSetKeyState, &cs ) )  // �õ�API����ָ��
	{
		pSetKeyState(KeyEvent,bDownUp);  // ����API����
		API_Leave(  );
	}
	return ;
#endif
}
