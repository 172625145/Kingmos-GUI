/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __AUDIO_EP_H
#define __AUDIO_EP_H

#include <eWindows.h>
#include "eMMSys.h"

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus 

#define IOCTL_AUDIO_MESSAGE   4000


typedef struct{
	UINT uDeviceID;
	UINT message;
	DWORD dwCallBackParam;
	LPARAM lParam;
	WPARAM wParam;
}AUDIOMDD_MSG, *PAUDIOMDD_MSG;

#define AUDIO_MESSAGE_START		0x1800

#define AM_OUT_BREAKLOOP		AUDIO_MESSAGE_START	
#define AM_OUT_CLOSE			AUDIO_MESSAGE_START + 1  //关闭当前的播放
#define AM_OUT_GETDEVCAPS		AUDIO_MESSAGE_START + 2  //得到设备的能力
#define AM_OUT_GETNUMDEVS		AUDIO_MESSAGE_START + 3  // 得到设备的树目
#define AM_OUT_GETPITCH			AUDIO_MESSAGE_START + 4
#define AM_OUT_GETPLAYBACKRATE	AUDIO_MESSAGE_START + 5
#define AM_OUT_GETPOS			AUDIO_MESSAGE_START + 6
#define AM_OUT_GETVOLUME		AUDIO_MESSAGE_START + 7
#define AM_OUT_OPEN				AUDIO_MESSAGE_START + 8 // 打开一个播放
#define AM_OUT_PAUSE			AUDIO_MESSAGE_START + 9 //暂停当前播放
#define AM_OUT_PREPARE			AUDIO_MESSAGE_START + 10
#define AM_OUT_RESET			AUDIO_MESSAGE_START + 11
#define AM_OUT_RESTART			AUDIO_MESSAGE_START + 12  // 开始一个播放
#define AM_OUT_SETPITCH			AUDIO_MESSAGE_START + 13
#define AM_OUT_SETPLAYBACKRATE	AUDIO_MESSAGE_START + 14
#define AM_OUT_SETVOLUME		AUDIO_MESSAGE_START + 15
#define AM_OUT_UNPREPARE		AUDIO_MESSAGE_START + 16
#define AM_OUT_WRITE			AUDIO_MESSAGE_START + 17   //插入一个声音数据缓存

 //下面是录音部分
#define AM_IN_ADDBUFFER			AUDIO_MESSAGE_START + 18
#define AM_IN_CLOSE				AUDIO_MESSAGE_START + 19
#define AM_IN_GETDEVCAPS		AUDIO_MESSAGE_START + 20
#define AM_IN_GETNUMDEVS		AUDIO_MESSAGE_START + 21
#define AM_IN_GETPOS			AUDIO_MESSAGE_START + 22
#define AM_IN_OPEN				AUDIO_MESSAGE_START + 23
#define AM_IN_PREPARE			AUDIO_MESSAGE_START + 24
#define AM_IN_RESET				AUDIO_MESSAGE_START + 25
#define AM_IN_START				AUDIO_MESSAGE_START + 26
#define AM_IN_STOP				AUDIO_MESSAGE_START + 27
#define AM_IN_UNPREPARE			AUDIO_MESSAGE_START + 28


typedef struct {
    HANDLE          hAudio;			  // 当前的声音句柄
    PWAVEFORMATEX lpFormat;          // 当前打开的声音数据格式
    DWORD          dwCallback;        // 回调参数
    DWORD          dwInstance;        // 当前应用程序的实例句柄
} AUDIOMDD_OPEN, * LPAUDIOMDD_OPEN;


// 系统内部数据组织结构
typedef struct {
    PWAVEHDR        pwh;				// 当前正在处理的数据
    PWAVEHDR        pwhLoop;			// Loop 的起始数据
//    BOOL            bInLoop;
    BOOL            bStarted;			// 是否已经开始播放或录音
    BOOL            bPaused;            // 是否被暂停
	BOOL			bLooped;			// 是否在Loop中 

    DRVCALLBACK*    pfnCallback;		// 回调API的函数
    PWAVEFORMATEX   lpFormat;			// 当前打开的声音数据格式
    HANDLE          hAudio;				// 当前的声音句柄，有API设定
    DWORD           dwDataLen;			// 当前已经处理过的数据数目
	DWORD			dwCallBackParam;	// 回调API的函数的参数
	DWORD			fdwOpen;			// 当前打开的模式
	HANDLE			hCallerProcess;		// 呼叫者进程句柄
} AUDIOMDD_INFO, *LPAUDIOMDD_INFO;


#define LOOPBEGIN(dwFlags)	(dwFlags & WHDR_BEGINLOOP)
#define LOOPEND(dwFlags)	(dwFlags & WHDR_ENDLOOP)
#define SETLOOP(pwh)		((pwh)->reserved = 1)
#define CLEARLOOP(pwh)		((pwh)->reserved = 0)
#define ISLOOP(pwh)			((pwh)->reserved)

#define CLEARDONE(pwh)		((pwh)->dwFlags &= ~WHDR_BEGINLOOP)
#define SETDONE(pwh)		((pwh)->dwFlags |= WHDR_BEGINLOOP)

#define AUDIO_API_IN_DEVICE

#ifdef __cplusplus
}           
#endif  // __cplusplus


#endif // __AUDIO_EP_H


