/***************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
***************************************************/
/**************************************************
�ļ�˵������ֵת������
�汾�ţ�1.0.0.456
����ʱ�ڣ�2003-03-16
���ߣ��½��� Jami chen
�޸ļ�¼��
**************************************************/


#include <ewindows.h>
#include "kbdvirtochar.h"


// **************************************************
// ��������
// **************************************************
const UINT8 NumKeyTable[10] =
{
  ')','!','@','#','$','%','^','&','*','('
};
const UINT8 NumPadTable3[6] =
{
	'*','+',-1,'-','.','/'
};

const UINT8 Punctuation1Table[7][2] =
{
	{';',':'},  // BA
	{'=','+'},  // BB
	{',','<'},  // BC
	{'-','_'},  // BD
	{'.','>'},  // BE
	{'/','?'},  // BF
	{'`','~'}   // C0
};

const UINT8 Punctuation2Table[4][2] =
{
  {
    '['    ,  // lsquarebracket  // DB
    '{'    
    },
  {
    '\\',     // backslash      // DC
    '|' ,  
    },
  {
    ']'  ,    // rsquarebracket    // DD
    '}'  
    },
  {
    '\''   , // apostrophe      // DE
    '"'   
    }
};

const UINT8 SpecialTable[6][2] =
{
  {
    127,      // back + Control
    8         // back + Control + Caps
    },
  {
    -1,  // tab + Control
    -1   // tab + Control + Caps
    },
  {
    -1,  // unused VKey
    -1   // unused VKey
    },
  {
    -1,  // unused VKey
    -1   // unused VKey
    },
  {
    -1,  // unused VKey
    -1   // unused VKey
    },
  {
    10,        // return + control
    10        // return + control + Caps
    }
};
// **************************************************
// ��������
// **************************************************

// **************************************************
// ������UINT GetKeyStateIndex(KEY_STATE flags)
// ������
// 	IN flags -- ��ǰ����״̬
// 
// ����ֵ�����ص�ǰϵͳ��״̬
// �����������õ���ǰϵͳ��״̬��
// ����: 
// **************************************************
UINT GetKeyStateIndex(KEY_STATE flags)
{
     UINT index = 0;
     
     if (flags & KS_CAPS_FLAG)  // CAPS����Ч
          index |= CAPS_INDEX; // ����CAPS����Ч

	 if (flags & KS_CTRL_FLAG)  // CTRL������
		  index |= CTRL_INDEX; // ����CTRL������

	 if (flags & KS_SHIFT_FLAG)  // SHIFT������
		 index |= SHIFT_INDEX ;  // ����SHIFT������
     
	 if (flags & KS_ALT_FLAG)  // ALT������
		 index |= ALT_INDEX;   // ����ALT������

     if (flags & KS_NUMLOCK_FLAG )  // NUMLOCK��Ч
		 index |= NUMLOCK_INDEX;    // ����NUMLOCK��Ч
//	 EdbgOutputDebugString("KeyState: 0x%x \r\n",index);
	 return index; // ����ϵͳ��״̬
}
//require the kernel of system to determine if judge
// **************************************************
// ������UINT VKToChar(UINT Virtkey,KEY_STATE *flags)
// ������
// 	IN Virtkey -- ��ǰ�����
// 	IN\OUT flags -- ��ǰ�İ���״̬
// 
// ����ֵ�������Ӧ���ַ�
// ���������������ת��Ϊ�ַ���
// ����: 
// **************************************************
UINT VKToChar(UINT Virtkey,KEY_STATE *flags)
{//To make sure if we need send a WM_CHAR message to system.
    UINT  index =0;
	
	index = GetKeyStateIndex(*flags);  // �õ�ϵͳ����״̬
	// ��ǰ�����ϵͳ�����������κ���
    if ( Virtkey == VK_LSHIFT || Virtkey == VK_RSHIFT || Virtkey == VK_LWIN || Virtkey == VK_RWIN ||
                Virtkey == VK_LCONTROL || Virtkey == VK_RCONTROL || Virtkey == VK_LMENU || Virtkey == VK_RMENU )
                return -1;
//    	EdbgOutputDebugString("Shift flags :0x%x",*flags);

	if (Virtkey>= 0x30  && Virtkey <= 0x39)
	{  // VK_0  --> VK_9 ����
        if ((index & CTRL_INDEX) || (index & ALT_INDEX))
		{   // ����Ѿ�����ALT ��CTRL������û���ַ���Ӧ
			*flags |= KS_NOCHAR_FLAG ; // ����û���ַ�
			return -1; 
		}
		else
		{
			if (index & SHIFT_INDEX)  // ����SHIFT
				return  NumKeyTable[Virtkey - VK_0]; // �������ּ�������ַ�
             else 
				 return Virtkey;  // ��������
		}
	}
    if (Virtkey >='A' && Virtkey<='Z')
	{  // Ӣ���ַ�
        if (( index & CTRL_INDEX ) || (index & ALT_INDEX))
		{  // �Ѿ�����ALT ��CTRL ������û���ַ�
			*flags |=KS_NOCHAR_FLAG;  // ����û���ַ�
		    return -1;
		}
		else
		{
            if (index & SHIFT_INDEX )
			{   // ����SHIFT
				if (index & CAPS_INDEX)
				{  // CAPS��Ч
					return Virtkey +32;  // ����Сд�ַ�
				}
				else
				{
                    return Virtkey;// ���ش�д�ַ�
				}
			}
			else  
			{  // û�а���SHIFT
				if (index & CAPS_INDEX)
				{  // CAPS��Ч
					return Virtkey ; //���ش�д�ַ�
				}
				else
				{
                    return Virtkey + 32 ;//����Сд�ַ�
				}
			}

		}

	}
    if ((Virtkey >= 0xBA) && (Virtkey <= 0XC0) )
	{  // ������1 ����Punctuation1Table����
        if (( index & CTRL_INDEX ) || (index & ALT_INDEX))
		{  // �а���ALT ��CTRL ���������ַ���Ӧ
			*flags |=KS_NOCHAR_FLAG; // �������ַ�
		    return -1;
		}
		else
		{
			if (index & SHIFT_INDEX)  // ��SHIFT����
				return Punctuation1Table[Virtkey - 0xBA ][1];  // ����SHIFT����״̬�ַ�
			else 
				return Punctuation1Table[Virtkey - 0xBA ][0];  // ���س����ַ�
		}
        
	}
//    if ((Virtkey >= 0xDB) && (Virtkey <= 0xDF) )//VK_LBRACKET---VK_OFF
    if ((Virtkey >= 0xDB) && (Virtkey <= 0xDE) )//VK_LBRACKET---VK_APOSTROPHE
	{ // ������1 ����Punctuation2Table����
		if (( index & CTRL_INDEX ) || (index & ALT_INDEX))
		{  // ����ALT �� CTRL �� ��û�ж�Ӧ�ַ�
			*flags |= KS_NOCHAR_FLAG;  // ����û�ж�Ӧ�ַ�
			return -1;
		}
		else
		{ 
			if (index & SHIFT_INDEX)  // ����
				return Punctuation2Table[Virtkey -0xDB][1];  // ���ذ���SHIFT���ַ�
			else
				return Punctuation2Table[Virtkey -0xDB][0];  // ����û�а���SHIFT���ַ�
		}
	}
	if ((Virtkey >= 0x08) && (Virtkey <= 0x0D) )
	{  //  ���Ʒ���
		if ((index & SHIFT_INDEX) && ( index & CTRL_INDEX ) )
		{  // ͬʱ����SHIFT �� CTRL �� ������Ч�ַ���Ӧ
			*flags |=KS_NOCHAR_FLAG;  // ��������Ч�ַ�
			return -1;
		}
		else
		{
			if (index & CTRL_INDEX)
			{  // ����CTRL��
				if (index & CAPS_INDEX)  // CAPS��Ч
					return SpecialTable[Virtkey -0x08][1];  // ������CAPS״̬��ֵ
				else
					return SpecialTable[Virtkey -0x08][0];  // ������CAPS״̬��ֵ
			}
			else
			{  // ֱ�ӷ������ֵ
                return Virtkey;
			}
		}
	}
	if (Virtkey == 0X1B )//VK_ESCAPE
	{  // ESC ��
      if ((index & CTRL_INDEX ) && (index  &  CAPS_INDEX))
	  {  // ͬʱ����CTRL ��  CAPS��
		  *flags |= KS_NOCHAR_FLAG; // ��������Ч�ַ���Ӧ
		  return -1;
	  }
	  else
		  return Virtkey;  // �������ֵ
	}
	
	if (Virtkey == 0x20 )//VK_SPACE
	{  // �ո�
       if ((index & CTRL_INDEX ) && (index  &  CAPS_INDEX))
	  {// ͬʱ����CTRL �� CAPS �������ַ���Ӧ
		  *flags |= KS_NOCHAR_FLAG;
		  return -1;
	  }
	  else
		  return Virtkey; // ���ؿո�
	}
    if (Virtkey>=0x6A  && Virtkey <=0x6F)  // ���ּ��̷����ֲ���
		return NumPadTable3[Virtkey - 0x6A];//THE del function of '.' is not permitted.
    if (Virtkey >= 0x25 && Virtkey <=0x28)  // �޶�Ӧ�ַ�
		return -1;//we should convert the combined keys.
	if (Virtkey >= 0x60/*NUM0*/ && Virtkey <=0x69)
	{  // ���ּ������ּ�
       if (index & NUMLOCK_INDEX) 
	   {//NUMLOCK IS ON
          if ((index & CTRL_INDEX) && (index & ALT_INDEX))     
		  {  // ����CTRL �� ALT ���� �޶�Ӧ�ַ�
			  *flags |=KS_NOCHAR_FLAG; // ����û���ַ���־
			  return -1;
		  }
		  else
			  return (0x30+ Virtkey-0x60);  // ���ض�Ӧ������
	   }
	   else
	   {//NUMLOCK is off
           // û�ж�Ӧ���ַ�
		   return Virtkey;
	   }
	}

	//if (Virtkey == 0x2e)  return Virtkey;  //added temperaly,should mask later. and the return the virtual .
	return -1;
}

static KEY_STATE g_KeyState=0;
//before send the keystate to the system first set global keystate .
// **************************************************
// ������void WINAPI Keybd_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
// ������
// 	IN KeyEvent -- �����¼�
// 	IN bDownUp -- ����״̬
// 
// ����ֵ����
// ��������������ϵͳ����״̬��
// ����: 
// **************************************************
void WINAPI Keybd_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
{
	if (KeyEvent == 0x10 )
	{ // SHIFT 
		if ( bDownUp )  // ������
			g_KeyState |= KS_LSHIFT_FLAG; // ����LSHIFT��־
		else  // ��û�а���
            g_KeyState &=(~KS_LSHIFT_FLAG);  // ���LSHIFT��־
	} 
    else if (KeyEvent == 0x11 )
	{ // CTRL
		if ( bDownUp )  // ������
			g_KeyState |= KS_LCTRL_FLAG; // ����LCTRL ��־
		else // ��û�а���
            g_KeyState &=(~KS_LCTRL_FLAG);  // ���LCTRL��־
	}
	else if (KeyEvent == 0XA0 )
	{ // LEFT SHIFT 	  
		if (bDownUp)  
		{ // ������
			g_KeyState |=  KS_LSHIFT_FLAG ; // ���� LSHIFT ��־
		}
		else 
		{  // ��û�а���
			g_KeyState &= (~KS_LSHIFT_FLAG);  // ���LSHIFT ��־
		}
		
	}
	else if (KeyEvent == 0xA1 )
	{ // RIGHT SHIFT
		if (bDownUp)
		{  // ������
			g_KeyState |= KS_RSHIFT_FLAG; // ����RSHIFT��־
		}
		else 
		{  // ��û�а���
			g_KeyState &= (~KS_RSHIFT_FLAG); // ��� RSHIFT ��־
		}
	}
	else   if (KeyEvent == 0xA2 )
	{  // LEFT CTRL 
		if (bDownUp)
		{ // ������
			g_KeyState |= KS_LCTRL_FLAG; // ���� LCTRL ��־
		}
		else
		{ // ��û�а���
			g_KeyState &= (~KS_LCTRL_FLAG); // ��� LCTRL ��־
		}
	}
	else   if (KeyEvent == 0xA3 )
	{ // RIGHT CTRL 
		if (bDownUp)
		{ // ������
			g_KeyState |= KS_RCTRL_FLAG; // ����RCTRL ��־
		}
		else
		{ // ��û�а���
			g_KeyState &= (~KS_RCTRL_FLAG); // ��� RCTRL ��־
		}
	}
	else if (KeyEvent == 0xa4 )
	{ // LEFT ALT
		if (bDownUp)
		{ // ������
			g_KeyState &= (~KS_LALT_FLAG); // ���� LALT ��־
		}
		else 
		{ // ��û�а���
			g_KeyState |= KS_LALT_FLAG; // ��� LALT ��־
		}
	}
	else if (KeyEvent == 0xa5 )
	{  // RIGHT ALT
		if (bDownUp)
		{ // ������
			g_KeyState &= (~KS_RALT_FLAG);  // ���� RALT ��־
		}
		else 
		{ // ��û�а���
			g_KeyState |= KS_RALT_FLAG;  // ��� RALT ��־
		}
	}
	else if (KeyEvent == 0x14)
	{  // CAPS ��
		if (!bDownUp )
		{//caps ֻ�������ʱ��
			if (g_KeyState & KS_CAPS_FLAG )  // CAPS �Ѿ���Ч
				g_KeyState &= (~KS_CAPS_FLAG);  // �ͷ�CAPS
			else   // Ŀǰû������CAPS
				g_KeyState |= KS_CAPS_FLAG;  // ����CAPS
		}
		
	}
	else if (KeyEvent == 0x90 )//NUMLOCK
	{  // NUM LOCK
		if (!bDownUp)
		{  // NUM LOCK Ҳֻ�������ʱ��
			if (g_KeyState & KS_NUMLOCK_FLAG ) // NUMLOCK �Ѿ���Ч
				g_KeyState &=(~KS_NUMLOCK_FLAG);  // ���NUMLOCK ��־
			else  // û������NUMLOCK 
				g_KeyState |= KS_NUMLOCK_FLAG;  // ����NUMLOCK ��־
		}
	}
    
	if ((g_KeyState & (KS_LCTRL_FLAG | KS_RCTRL_FLAG)))  // ��CTRL������
		g_KeyState |= KS_CTRL_FLAG; // ����CTRL��־
	else  // û��CTRL������
		g_KeyState &=(~KS_CTRL_FLAG); // ��� CTRL ��־
	
	if ((g_KeyState & (KS_LALT_FLAG | KS_RALT_FLAG)))  // ��ALT ������
		g_KeyState |= KS_ALT_FLAG; // ����ALT ��־
	else // û��ALT ������
        g_KeyState &= (~KS_ALT_FLAG); // ���ALT ��־
	
	if ((g_KeyState & (KS_LSHIFT_FLAG | KS_RSHIFT_FLAG)))  // ��SHIFT ����
		g_KeyState |= KS_SHIFT_FLAG ;  // ����SHIFT ��־
	else // û��SHIFT ����
        g_KeyState &=(~KS_SHIFT_FLAG) ;  // ���SHIFT ��־
}

// **************************************************
// ������UINT WINAPI Keybd_MapVirtualKey(UINT uCode, UINT uMapType)
// ������
// 	IN uCode -- ��Ҫӳ��Ĵ���
// 	IN uMapType - ӳ������
// 
// ����ֵ������ӳ���Ĵ���
// ����������ӳ�����ֵ��
// ����: 
// **************************************************
UINT WINAPI Keybd_MapVirtualKey(UINT uCode, UINT uMapType)
{//This is compliant to Common Standard.
     UINT ch;
     switch (uMapType)
	 {
	 case 0:
		 //Virtual convert to Scan code.
		 break;
	 case 1:
		 //Scan code to Virtual
		 break;
	 case 2:  
		 //Virtual To CHAR
		  ch = (UINT)VKToChar(uCode,&g_KeyState);
		  return ch;
	     break;
	 case 3:
		 break;
	 }
     return (UINT)uCode;  // û��ӳ�䣬ԭ������
}

