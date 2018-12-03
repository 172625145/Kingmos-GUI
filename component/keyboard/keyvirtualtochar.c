/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明：键值转化函数
版本号：1.0.0.456
开发时期：2003-03-16
作者：陈建明 Jami chen
修改记录：
**************************************************/


#include <ewindows.h>
#include "kbdvirtochar.h"


// **************************************************
// 定义区域
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
// 函数区域
// **************************************************

// **************************************************
// 声明：UINT GetKeyStateIndex(KEY_STATE flags)
// 参数：
// 	IN flags -- 当前键的状态
// 
// 返回值：返回当前系统键状态
// 功能描述：得到当前系统键状态。
// 引用: 
// **************************************************
UINT GetKeyStateIndex(KEY_STATE flags)
{
     UINT index = 0;
     
     if (flags & KS_CAPS_FLAG)  // CAPS键有效
          index |= CAPS_INDEX; // 设置CAPS键有效

	 if (flags & KS_CTRL_FLAG)  // CTRL键按下
		  index |= CTRL_INDEX; // 设置CTRL键按下

	 if (flags & KS_SHIFT_FLAG)  // SHIFT键按下
		 index |= SHIFT_INDEX ;  // 设置SHIFT键按下
     
	 if (flags & KS_ALT_FLAG)  // ALT键按下
		 index |= ALT_INDEX;   // 设置ALT键按下

     if (flags & KS_NUMLOCK_FLAG )  // NUMLOCK有效
		 index |= NUMLOCK_INDEX;    // 设置NUMLOCK有效
//	 EdbgOutputDebugString("KeyState: 0x%x \r\n",index);
	 return index; // 返回系统键状态
}
//require the kernel of system to determine if judge
// **************************************************
// 声明：UINT VKToChar(UINT Virtkey,KEY_STATE *flags)
// 参数：
// 	IN Virtkey -- 当前的虚键
// 	IN\OUT flags -- 当前的按键状态
// 
// 返回值：虚键对应的字符
// 功能描述：将虚键转化为字符。
// 引用: 
// **************************************************
UINT VKToChar(UINT Virtkey,KEY_STATE *flags)
{//To make sure if we need send a WM_CHAR message to system.
    UINT  index =0;
	
	index = GetKeyStateIndex(*flags);  // 得到系统键的状态
	// 当前虚键是系统键，不用做任何事
    if ( Virtkey == VK_LSHIFT || Virtkey == VK_RSHIFT || Virtkey == VK_LWIN || Virtkey == VK_RWIN ||
                Virtkey == VK_LCONTROL || Virtkey == VK_RCONTROL || Virtkey == VK_LMENU || Virtkey == VK_RMENU )
                return -1;
//    	EdbgOutputDebugString("Shift flags :0x%x",*flags);

	if (Virtkey>= 0x30  && Virtkey <= 0x39)
	{  // VK_0  --> VK_9 数字
        if ((index & CTRL_INDEX) || (index & ALT_INDEX))
		{   // 如果已经按下ALT 和CTRL键，则没有字符对应
			*flags |= KS_NOCHAR_FLAG ; // 设置没有字符
			return -1; 
		}
		else
		{
			if (index & SHIFT_INDEX)  // 按下SHIFT
				return  NumKeyTable[Virtkey - VK_0]; // 返回数字键上面的字符
             else 
				 return Virtkey;  // 返回数字
		}
	}
    if (Virtkey >='A' && Virtkey<='Z')
	{  // 英文字符
        if (( index & CTRL_INDEX ) || (index & ALT_INDEX))
		{  // 已经按下ALT 和CTRL 键，则没有字符
			*flags |=KS_NOCHAR_FLAG;  // 设置没有字符
		    return -1;
		}
		else
		{
            if (index & SHIFT_INDEX )
			{   // 按下SHIFT
				if (index & CAPS_INDEX)
				{  // CAPS有效
					return Virtkey +32;  // 返回小写字符
				}
				else
				{
                    return Virtkey;// 返回大写字符
				}
			}
			else  
			{  // 没有按下SHIFT
				if (index & CAPS_INDEX)
				{  // CAPS有效
					return Virtkey ; //返回大写字符
				}
				else
				{
                    return Virtkey + 32 ;//返回小写字符
				}
			}

		}

	}
    if ((Virtkey >= 0xBA) && (Virtkey <= 0XC0) )
	{  // 符号组1 ，见Punctuation1Table定义
        if (( index & CTRL_INDEX ) || (index & ALT_INDEX))
		{  // 有按下ALT 和CTRL 键，则无字符对应
			*flags |=KS_NOCHAR_FLAG; // 设置无字符
		    return -1;
		}
		else
		{
			if (index & SHIFT_INDEX)  // 有SHIFT按下
				return Punctuation1Table[Virtkey - 0xBA ][1];  // 返回SHIFT按下状态字符
			else 
				return Punctuation1Table[Virtkey - 0xBA ][0];  // 返回常规字符
		}
        
	}
//    if ((Virtkey >= 0xDB) && (Virtkey <= 0xDF) )//VK_LBRACKET---VK_OFF
    if ((Virtkey >= 0xDB) && (Virtkey <= 0xDE) )//VK_LBRACKET---VK_APOSTROPHE
	{ // 符号组1 ，见Punctuation2Table定义
		if (( index & CTRL_INDEX ) || (index & ALT_INDEX))
		{  // 按下ALT 和 CTRL 键 。没有对应字符
			*flags |= KS_NOCHAR_FLAG;  // 设置没有对应字符
			return -1;
		}
		else
		{ 
			if (index & SHIFT_INDEX)  // 按下
				return Punctuation2Table[Virtkey -0xDB][1];  // 返回按下SHIFT的字符
			else
				return Punctuation2Table[Virtkey -0xDB][0];  // 返回没有按下SHIFT的字符
		}
	}
	if ((Virtkey >= 0x08) && (Virtkey <= 0x0D) )
	{  //  控制符号
		if ((index & SHIFT_INDEX) && ( index & CTRL_INDEX ) )
		{  // 同时按下SHIFT 和 CTRL ， 则无有效字符对应
			*flags |=KS_NOCHAR_FLAG;  // 设置无有效字符
			return -1;
		}
		else
		{
			if (index & CTRL_INDEX)
			{  // 按下CTRL键
				if (index & CAPS_INDEX)  // CAPS有效
					return SpecialTable[Virtkey -0x08][1];  // 返回有CAPS状态的值
				else
					return SpecialTable[Virtkey -0x08][0];  // 返回无CAPS状态的值
			}
			else
			{  // 直接返回虚键值
                return Virtkey;
			}
		}
	}
	if (Virtkey == 0X1B )//VK_ESCAPE
	{  // ESC 键
      if ((index & CTRL_INDEX ) && (index  &  CAPS_INDEX))
	  {  // 同时按下CTRL 和  CAPS键
		  *flags |= KS_NOCHAR_FLAG; // 设置无有效字符对应
		  return -1;
	  }
	  else
		  return Virtkey;  // 返回虚键值
	}
	
	if (Virtkey == 0x20 )//VK_SPACE
	{  // 空格
       if ((index & CTRL_INDEX ) && (index  &  CAPS_INDEX))
	  {// 同时按下CTRL 和 CAPS ，则无字符对应
		  *flags |= KS_NOCHAR_FLAG;
		  return -1;
	  }
	  else
		  return Virtkey; // 返回空格
	}
    if (Virtkey>=0x6A  && Virtkey <=0x6F)  // 数字键盘非数字部分
		return NumPadTable3[Virtkey - 0x6A];//THE del function of '.' is not permitted.
    if (Virtkey >= 0x25 && Virtkey <=0x28)  // 无对应字符
		return -1;//we should convert the combined keys.
	if (Virtkey >= 0x60/*NUM0*/ && Virtkey <=0x69)
	{  // 数字键盘数字键
       if (index & NUMLOCK_INDEX) 
	   {//NUMLOCK IS ON
          if ((index & CTRL_INDEX) && (index & ALT_INDEX))     
		  {  // 按下CTRL 和 ALT 键， 无对应字符
			  *flags |=KS_NOCHAR_FLAG; // 设置没有字符标志
			  return -1;
		  }
		  else
			  return (0x30+ Virtkey-0x60);  // 返回对应的数字
	   }
	   else
	   {//NUMLOCK is off
           // 没有对应的字符
		   return Virtkey;
	   }
	}

	//if (Virtkey == 0x2e)  return Virtkey;  //added temperaly,should mask later. and the return the virtual .
	return -1;
}

static KEY_STATE g_KeyState=0;
//before send the keystate to the system first set global keystate .
// **************************************************
// 声明：void WINAPI Keybd_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
// 参数：
// 	IN KeyEvent -- 键盘事件
// 	IN bDownUp -- 按下状态
// 
// 返回值：无
// 功能描述：设置系统键的状态。
// 引用: 
// **************************************************
void WINAPI Keybd_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp)
{
	if (KeyEvent == 0x10 )
	{ // SHIFT 
		if ( bDownUp )  // 键按下
			g_KeyState |= KS_LSHIFT_FLAG; // 设置LSHIFT标志
		else  // 键没有按下
            g_KeyState &=(~KS_LSHIFT_FLAG);  // 清除LSHIFT标志
	} 
    else if (KeyEvent == 0x11 )
	{ // CTRL
		if ( bDownUp )  // 键按下
			g_KeyState |= KS_LCTRL_FLAG; // 设置LCTRL 标志
		else // 键没有按下
            g_KeyState &=(~KS_LCTRL_FLAG);  // 清除LCTRL标志
	}
	else if (KeyEvent == 0XA0 )
	{ // LEFT SHIFT 	  
		if (bDownUp)  
		{ // 键按下
			g_KeyState |=  KS_LSHIFT_FLAG ; // 设置 LSHIFT 标志
		}
		else 
		{  // 键没有按下
			g_KeyState &= (~KS_LSHIFT_FLAG);  // 清楚LSHIFT 标志
		}
		
	}
	else if (KeyEvent == 0xA1 )
	{ // RIGHT SHIFT
		if (bDownUp)
		{  // 键按下
			g_KeyState |= KS_RSHIFT_FLAG; // 设置RSHIFT标志
		}
		else 
		{  // 键没有按下
			g_KeyState &= (~KS_RSHIFT_FLAG); // 清除 RSHIFT 标志
		}
	}
	else   if (KeyEvent == 0xA2 )
	{  // LEFT CTRL 
		if (bDownUp)
		{ // 键按下
			g_KeyState |= KS_LCTRL_FLAG; // 设置 LCTRL 标志
		}
		else
		{ // 键没有按下
			g_KeyState &= (~KS_LCTRL_FLAG); // 清除 LCTRL 标志
		}
	}
	else   if (KeyEvent == 0xA3 )
	{ // RIGHT CTRL 
		if (bDownUp)
		{ // 键按下
			g_KeyState |= KS_RCTRL_FLAG; // 设置RCTRL 标志
		}
		else
		{ // 键没有按下
			g_KeyState &= (~KS_RCTRL_FLAG); // 清除 RCTRL 标志
		}
	}
	else if (KeyEvent == 0xa4 )
	{ // LEFT ALT
		if (bDownUp)
		{ // 键按下
			g_KeyState &= (~KS_LALT_FLAG); // 设置 LALT 标志
		}
		else 
		{ // 键没有按下
			g_KeyState |= KS_LALT_FLAG; // 清除 LALT 标志
		}
	}
	else if (KeyEvent == 0xa5 )
	{  // RIGHT ALT
		if (bDownUp)
		{ // 键按下
			g_KeyState &= (~KS_RALT_FLAG);  // 设置 RALT 标志
		}
		else 
		{ // 键没有按下
			g_KeyState |= KS_RALT_FLAG;  // 清除 RALT 标志
		}
	}
	else if (KeyEvent == 0x14)
	{  // CAPS 键
		if (!bDownUp )
		{//caps 只处理弹起的时候
			if (g_KeyState & KS_CAPS_FLAG )  // CAPS 已经有效
				g_KeyState &= (~KS_CAPS_FLAG);  // 释放CAPS
			else   // 目前没有设置CAPS
				g_KeyState |= KS_CAPS_FLAG;  // 设置CAPS
		}
		
	}
	else if (KeyEvent == 0x90 )//NUMLOCK
	{  // NUM LOCK
		if (!bDownUp)
		{  // NUM LOCK 也只处理弹起的时候
			if (g_KeyState & KS_NUMLOCK_FLAG ) // NUMLOCK 已经有效
				g_KeyState &=(~KS_NUMLOCK_FLAG);  // 清除NUMLOCK 标志
			else  // 没有设置NUMLOCK 
				g_KeyState |= KS_NUMLOCK_FLAG;  // 设置NUMLOCK 标志
		}
	}
    
	if ((g_KeyState & (KS_LCTRL_FLAG | KS_RCTRL_FLAG)))  // 有CTRL键按下
		g_KeyState |= KS_CTRL_FLAG; // 设置CTRL标志
	else  // 没有CTRL键按下
		g_KeyState &=(~KS_CTRL_FLAG); // 清除 CTRL 标志
	
	if ((g_KeyState & (KS_LALT_FLAG | KS_RALT_FLAG)))  // 有ALT 键按下
		g_KeyState |= KS_ALT_FLAG; // 设置ALT 标志
	else // 没有ALT 键按下
        g_KeyState &= (~KS_ALT_FLAG); // 清除ALT 标志
	
	if ((g_KeyState & (KS_LSHIFT_FLAG | KS_RSHIFT_FLAG)))  // 有SHIFT 按下
		g_KeyState |= KS_SHIFT_FLAG ;  // 设置SHIFT 标志
	else // 没有SHIFT 按下
        g_KeyState &=(~KS_SHIFT_FLAG) ;  // 清楚SHIFT 标志
}

// **************************************************
// 声明：UINT WINAPI Keybd_MapVirtualKey(UINT uCode, UINT uMapType)
// 参数：
// 	IN uCode -- 需要映射的代码
// 	IN uMapType - 映射类型
// 
// 返回值：返回映射后的代码
// 功能描述：映射虚键值。
// 引用: 
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
     return (UINT)uCode;  // 没有映射，原样返回
}

