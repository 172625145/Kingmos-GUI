/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __AUDIOMDD_H
#define __AUDIOMDD_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define WAPI_INOUT			BYTE
#define WAPI_OUT			0
#define WAPI_IN				1

#define AUDIO_IGNORE		0x0000

#define AUDIO_PLAY_MASK		0x000F
#define AUDIO_PLAYING		0x0001
#define AUDIO_STOPPLAY		0x0002

#define AUDIO_RECORD_MASK	0x00F0
#define AUDIO_RECORDING		0x0010
#define AUDIO_STOPRECORD	0x0020


#define WPDM_CLOSE			0x0001
#define WPDM_CONTINUE		0x0002
#define WPDM_GETDEVCAPS		0x0003
#define WPDM_OPEN			0x0004
#define WPDM_STANDBY		0x0005
#define WPDM_START			0x0006
#define WPDM_STOP			0x0007
#define WPDM_PAUSE			0x0008
#define WPDM_ENDOFDATA		0x0009
#define WPDM_RESTART		0x000A
#define WPDM_GETVOLUME		0x000B
#define WPDM_SETVOLUME		0x000C

BYTE PDD_AudioGetInterruptType(VOID);
DWORD PDD_AudioMessage(UINT uMsg,DWORD dwParam1,DWORD dwParam2);
BOOL PDD_AudioInitialize(DWORD dwIndex);
VOID PDD_AudioDeinitialize(VOID);
VOID PDD_AudioPowerHandler(BOOL power_down);
MMRESULT PDD_WaveProc(WAPI_INOUT apidir,DWORD dwCode,DWORD dwParam1,DWORD dwParam2);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //__AUDIOMDD_H

