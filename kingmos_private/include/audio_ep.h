/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
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
#define AM_OUT_CLOSE			AUDIO_MESSAGE_START + 1  //�رյ�ǰ�Ĳ���
#define AM_OUT_GETDEVCAPS		AUDIO_MESSAGE_START + 2  //�õ��豸������
#define AM_OUT_GETNUMDEVS		AUDIO_MESSAGE_START + 3  // �õ��豸����Ŀ
#define AM_OUT_GETPITCH			AUDIO_MESSAGE_START + 4
#define AM_OUT_GETPLAYBACKRATE	AUDIO_MESSAGE_START + 5
#define AM_OUT_GETPOS			AUDIO_MESSAGE_START + 6
#define AM_OUT_GETVOLUME		AUDIO_MESSAGE_START + 7
#define AM_OUT_OPEN				AUDIO_MESSAGE_START + 8 // ��һ������
#define AM_OUT_PAUSE			AUDIO_MESSAGE_START + 9 //��ͣ��ǰ����
#define AM_OUT_PREPARE			AUDIO_MESSAGE_START + 10
#define AM_OUT_RESET			AUDIO_MESSAGE_START + 11
#define AM_OUT_RESTART			AUDIO_MESSAGE_START + 12  // ��ʼһ������
#define AM_OUT_SETPITCH			AUDIO_MESSAGE_START + 13
#define AM_OUT_SETPLAYBACKRATE	AUDIO_MESSAGE_START + 14
#define AM_OUT_SETVOLUME		AUDIO_MESSAGE_START + 15
#define AM_OUT_UNPREPARE		AUDIO_MESSAGE_START + 16
#define AM_OUT_WRITE			AUDIO_MESSAGE_START + 17   //����һ���������ݻ���

 //������¼������
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
    HANDLE          hAudio;			  // ��ǰ���������
    PWAVEFORMATEX lpFormat;          // ��ǰ�򿪵��������ݸ�ʽ
    DWORD          dwCallback;        // �ص�����
    DWORD          dwInstance;        // ��ǰӦ�ó����ʵ�����
} AUDIOMDD_OPEN, * LPAUDIOMDD_OPEN;


// ϵͳ�ڲ�������֯�ṹ
typedef struct {
    PWAVEHDR        pwh;				// ��ǰ���ڴ��������
    PWAVEHDR        pwhLoop;			// Loop ����ʼ����
//    BOOL            bInLoop;
    BOOL            bStarted;			// �Ƿ��Ѿ���ʼ���Ż�¼��
    BOOL            bPaused;            // �Ƿ���ͣ
	BOOL			bLooped;			// �Ƿ���Loop�� 

    DRVCALLBACK*    pfnCallback;		// �ص�API�ĺ���
    PWAVEFORMATEX   lpFormat;			// ��ǰ�򿪵��������ݸ�ʽ
    HANDLE          hAudio;				// ��ǰ�������������API�趨
    DWORD           dwDataLen;			// ��ǰ�Ѿ��������������Ŀ
	DWORD			dwCallBackParam;	// �ص�API�ĺ����Ĳ���
	DWORD			fdwOpen;			// ��ǰ�򿪵�ģʽ
	HANDLE			hCallerProcess;		// �����߽��̾��
} AUDIOMDD_INFO, *LPAUDIOMDD_INFO;


#define LOOPBEGIN(dwFlags)	(dwFlags & WHDR_BEGINLOOP)
#define LOOPEND(dwFlags)	(dwFlags & WHDR_ENDLOOP)
#define SETLOOP(pwh)		((pwh)->reserved = 1)
#define CLEARLOOP(pwh)		((pwh)->reserved = 0)
#define ISLOOP(pwh)			((pwh)->reserved)

#define CLEARDONE(pwh)		((pwh)->dwFlags &= ~WHDR_BEGINLOOP)
#define SETDONE(pwh)		((pwh)->dwFlags |= WHDR_BEGINLOOP)

//#define  AUDIO_MDD_TEST
#undef  AUDIO_MDD_TEST


#ifdef __cplusplus
}           
#endif  // __cplusplus


#endif // __AUDIO_EP_H


