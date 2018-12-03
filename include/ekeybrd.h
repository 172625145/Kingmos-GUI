/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EKEYBRD_H
#define __EKEYBRD_H

#define  VkKeyScan KB_VkKeyScan
SHORT WINAPI KB_VkKeyScan( TCHAR uChar );

#define MapVirtualKey KB_MapVirtualKey
UINT WINAPI KB_MapVirtualKey(UINT uKeyCode, UINT fuMapType);

#define SetKeyState KB_SetKeyState
void WINAPI KB_SetKeyState( UINT8	KeyEvent,BOOL  bDownUp);

//HWND CreateSystemKeyboard(HWND hParent);

#endif         // __EKEYBRD_H
