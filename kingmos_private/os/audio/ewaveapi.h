/**************************************************************************
* Copyright (c)  Î¢Âß¼­(WEILUOJI). All rights reserved.                   *
**************************************************************************/
#ifndef __EWAVEAPI_H__
#define __EWAVEAPI_H__

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */
#include "emmsys.h"

/* waveform audio function prototypes */
UINT     WINAPI Audio_waveOutGetNumDevs(void);
MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo);
MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo);
MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo);
MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo);
MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo);
MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID);
MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);


UINT     WINAPI Audio_waveInGetNumDevs(void);
MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi);
MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi);
MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi);
MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi);
MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, LPUINT puDeviceID);
MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);


MMRESULT WINAPI midi_Connect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);
MMRESULT WINAPI midi_Disconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);
MMRESULT WINAPI midi_InAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
MMRESULT WINAPI midi_InClose(HMIDIIN hMidiIn);
MMRESULT WINAPI midi_InGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps);
MMRESULT WINAPI midi_InGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText );
MMRESULT WINAPI midi_InGetID(HMIDIIN hmi,LPUINT puDeviceID);
UINT WINAPI midi_InGetNumDevs(VOID);
DWORD WINAPI midi_InMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2);
MMRESULT WINAPI midi_InOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);
MMRESULT WINAPI midi_InPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
MMRESULT WINAPI midi_InReset(HMIDIIN hMidiIn);
MMRESULT WINAPI midi_InStart(HMIDIIN hMidiIn);
MMRESULT WINAPI midi_InStop(HMIDIIN hMidiIn);
MMRESULT WINAPI midi_InUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
MMRESULT WINAPI midi_OutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags);
MMRESULT WINAPI midi_OutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags);
MMRESULT WINAPI midi_OutClose(HMIDIOUT hmo);
MMRESULT WINAPI midi_OutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps);
UINT WINAPI midi_OutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText);
MMRESULT WINAPI midi_OutGetID(HMIDIOUT hmo,LPUINT puDeviceID);
UINT WINAPI midi_OutGetNumDevs(VOID);
MMRESULT WINAPI midi_OutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume);
MMRESULT WINAPI midi_OutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
DWORD WINAPI midi_OutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2);
UINT WINAPI midi_OutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);
MMRESULT WINAPI midi_OutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
MMRESULT WINAPI midi_OutReset(HMIDIOUT hmo);
MMRESULT WINAPI midi_OutSetVolume(HMIDIOUT hmo,DWORD dwVolume);
MMRESULT WINAPI midi_OutShortMsg(HMIDIOUT hmo, DWORD dwMsg);
MMRESULT WINAPI midi_OutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
MMRESULT WINAPI midi_StreamClose(HMIDISTRM hStream);
MMRESULT WINAPI midi_StreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen);
MMRESULT WINAPI midi_StreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr);
MMRESULT WINAPI midi_StreamPause(HMIDISTRM hMidiStream);
MMRESULT WINAPI midi_StreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt);
MMRESULT WINAPI midi_StreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty);
MMRESULT WINAPI midi_StreamRestart(HMIDISTRM hMidiStream);
MMRESULT WINAPI midi_StreamStop(HMIDISTRM hMidiStream);

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif
