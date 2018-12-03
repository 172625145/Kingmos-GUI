#include <ewindows.h>
#include <gwmeobj.h>
#include <eoemkey.h>
//HANDLE hKeyEvent;
extern int KeybdRead(int *pcode, int *ptype);
int KeybdInit();
void KeybdDeinit();

typedef struct __KEY_CODE_TRANSLATE{
	int iCode;
	DWORD dwVirt_Key;
}KEY_CODE_TRANSLATE, *PKEY_CODE_TRANSLATE;
#if 0
static KEY_CODE_TRANSLATE KeyCodeTransTable[] = {
	{0x1b, VK_CANCEL}, {'.', VK_DELETE}, {'P', VK_POWER}, {'p', VK_PRINT},
	{'4', VK_LEFT},   {'6', VK_RIGHT}, {'2', VK_DOWN}, {'8', VK_UP},
	{0x0d, VK_SET}, {'+', VK_ZOOM_INC},  {'-', VK_ZOOM_DEC}, {'M', VK_LMENU},
	{'m', VK_LMENU}, {' ', VK_INFO},
};
#endif
#if 1
static KEY_CODE_TRANSLATE KeyCodeTransTable[] = {
	{0x1f, VK_CANCEL}, {0x20, VK_DELETE}, {0x74, VK_POWER}, {0x19, VK_PRINT},
	{0x69, VK_LEFT}, {0x6a, VK_RIGHT}, {0x6c, VK_DOWN}, {0x67, VK_UP},
	{0x1c, VK_SET},  {0x13, VK_ZOOM_DEC},  {0x21, VK_ZOOM_INC}, {0x22, VK_LMENU}, {0x10, VK_INFO}
};
#endif
DWORD TranslateKey(int iCode)
{
	int i, iCount = sizeof(KeyCodeTransTable) / sizeof(KeyCodeTransTable[0]);
	for(i = 0; i < iCount; i ++)
	{
		if(KeyCodeTransTable[i].iCode == iCode)
			return KeyCodeTransTable[i].dwVirt_Key;
	}
	return (DWORD)-1;
}
DWORD WINAPI KeybdThread(
  LPVOID lpParameter   // thread data
)
{
	RETAILMSG(1, ("KeybdThread enter ..... \r\n"));
	if(KeybdInit() < 0)
		return 0;
    while (1)
	{
		int icode, itype;
		DWORD dwVirtKey;
		if(KeybdRead(&icode, &itype) == -1)
			break;	
		RETAILMSG(1, ("Get key val 0x%x \r\n", icode));
		dwVirtKey = TranslateKey(icode);
		if(dwVirtKey != (DWORD)-1)
		{
			if( dwVirtKey == VK_POWER )
			{
				RETAILMSG(1, ("PowerOff.\r\n"));
				exit(0);
			}
			
			if(itype == 0)
				itype = KEYEVENTF_KEYUP;
			else
				itype = 0;
			keybd_event( dwVirtKey, 0, itype, 0  );
//			keybd_event( dwVirtKey, 0, itype, 0  );
			RETAILMSG(1, ("Vkey 0x%x \r\n", dwVirtKey));
		}
		else
		{
			RETAILMSG(1, ("Invalid key!!!\r\n"));
		}
	}
	KeybdDeinit();
	return 0;
}

BOOL InstallKeyboardDevice( void )
{
	HWND hThread;
	RETAILMSG(1, ("InstallKeyboardDevice 1111 \r\n"));
#if 1	
	system("modprobe ehci-hcd");
	system("modprobe ohci-hcd");
	system("modprobe usb-storage");
#endif	
	hThread = CreateThread(NULL, 0, KeybdThread, NULL, 0, NULL);
	RETAILMSG(1, ("InstallKeyboardDevice 2222 0x%x \r\n", hThread));
	ASSERT(hThread);
	return hThread ? TRUE : FALSE;
}

DWORD CALLBACK GWDI_KeyEnter( UINT msg, DWORD dwParam, LPVOID lParam )
{
    switch( msg )
    {
    case GWDI_CREATE:   //初始化硬件
        return InstallKeyboardDevice();
    case GWDI_DESTROY:   //关掉显示设备
        return 1;
    //case GWDI_POWEROFF:
        // 开关机处理
      //  return DoPowerOff();
    //case GWDI_POWERON:
    // 开关机处理
      //  return DoPowerOn();
    }
    return 0;
}

