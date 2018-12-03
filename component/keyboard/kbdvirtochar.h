/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
#ifndef __KBDVIRTOCHAR_H
#define __KBDVIRTOCHAR_H
#ifdef __cplusplus 
extern "C" {
#endif  /* __cplusplus */    
// 这里是你的有关声明部分

typedef UINT32 KEY_STATE;

#define KS_CAPS_FLAG   0X80000000
#define KS_SHIFT_FLAG  0X40000000   
#define KS_CTRL_FLAG   0X20000000
#define KS_ALT_FLAG    0X10000000
#define KS_LWIN_FLAG   0X08000000
#define KS_LSHIFT_FLAG 0x04000000
#define KS_LCTRL_FLAG  0x02000000
#define KS_LALT_FLAG   0X01000000
#define KS_RWIN_FLAG   0X00800000
#define KS_RSHIFT_FLAG 0X00400000
#define KS_RCTRL_FLAG  0X00200000
#define KS_RALT_FLAG   0X00100000
#define KS_NUMLOCK_FLAG 0X00080000
#define KS_NOCHAR_FLAG   0x00010000//This flag tells system without CHAR message.

#define KS_ALLDOWN_FLAG 0xFFF80000

#define NUMLOCK_INDEX       16
#define CAPS_INDEX      8
#define SHIFT_INDEX     4
#define CTRL_INDEX      2
#define ALT_INDEX       1  

#define NO_CHAR        0xff

#define IsKeyStateDown(Flags)  (Flags & KS_ALLDOWN_FLAG)
UINT8 KeybdDriverMapVirtualKey(
								UINT8 uCode,
								UINT32 uMapType,
								KEY_STATE *pKeyState
								);

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  //__KBDVIRTOCHAR_H

