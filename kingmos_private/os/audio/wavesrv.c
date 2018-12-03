/***************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
***************************************************/
/**************************************************
文件说明： 声音驱动程序服务程序
版本号：1.0.0.456
开发时期：2004-02-26
作者：陈建明 Jami chen
修改记录：
**************************************************/
#include <eframe.h>
//#include <ewavedev.h>
#include <eapisrv.h>

#include "ewaveapi.h"

// **************************************************
// 全局定义区
// **************************************************

///////////////////////////////////////////////////
const PFNVOID lpAudioAPI[] = 
{
	NULL,
    (PFNVOID)Audio_waveOutGetNumDevs,
    (PFNVOID)Audio_waveOutGetDevCaps,
    (PFNVOID)Audio_waveOutGetVolume,
    (PFNVOID)Audio_waveOutSetVolume,
    (PFNVOID)Audio_waveOutGetErrorText,
    (PFNVOID)Audio_waveOutClose,
    (PFNVOID)Audio_waveOutPrepareHeader,
    (PFNVOID)Audio_waveOutUnprepareHeader,
    (PFNVOID)Audio_waveOutWrite,
    (PFNVOID)Audio_waveOutPause,
    (PFNVOID)Audio_waveOutRestart,
    (PFNVOID)Audio_waveOutReset,
    (PFNVOID)Audio_waveOutBreakLoop,
    (PFNVOID)Audio_waveOutGetPosition,
    (PFNVOID)Audio_waveOutGetPitch,
    (PFNVOID)Audio_waveOutSetPitch,
    (PFNVOID)Audio_waveOutGetPlaybackRate,
    (PFNVOID)Audio_waveOutSetPlaybackRate,
    (PFNVOID)Audio_waveOutGetID,
    (PFNVOID)Audio_waveOutMessage,
    (PFNVOID)Audio_waveOutOpen,

	(PFNVOID)Audio_waveInGetNumDevs,
    (PFNVOID)Audio_waveInGetDevCaps,
    (PFNVOID)Audio_waveInGetErrorText,
    (PFNVOID)Audio_waveInClose,
    (PFNVOID)Audio_waveInPrepareHeader,
    (PFNVOID)Audio_waveInUnprepareHeader,
    (PFNVOID)Audio_waveInAddBuffer,
    (PFNVOID)Audio_waveInStart,
    (PFNVOID)Audio_waveInStop,
    (PFNVOID)Audio_waveInReset,
    (PFNVOID)Audio_waveInGetPosition,
    (PFNVOID)Audio_waveInGetID,
    (PFNVOID)Audio_waveInMessage,
    (PFNVOID)Audio_waveInOpen,

#ifdef MIDI_SUPPORT    
    (PFNVOID)midi_Connect,
    (PFNVOID)midi_Disconnect,
    (PFNVOID)midi_InAddBuffer,
    (PFNVOID)midi_InClose,
    (PFNVOID)midi_InGetDevCaps,
    (PFNVOID)midi_InGetErrorText,
    (PFNVOID)midi_InGetID,
    (PFNVOID)midi_InGetNumDevs,
    (PFNVOID)midi_InMessage,
    (PFNVOID)midi_InOpen,
    (PFNVOID)midi_InPrepareHeader,
    (PFNVOID)midi_InReset,
    (PFNVOID)midi_InStart,
    (PFNVOID)midi_InStop,
    (PFNVOID)midi_InUnprepareHeader,

    (PFNVOID)midi_OutCacheDrumPatches,
    (PFNVOID)midi_OutCachePatches,
    (PFNVOID)midi_OutClose,
    (PFNVOID)midi_OutGetDevCaps,
    (PFNVOID)midi_OutGetErrorText,
    (PFNVOID)midi_OutGetID,
    (PFNVOID)midi_OutGetNumDevs,
    (PFNVOID)midi_OutGetVolume,
    (PFNVOID)midi_OutLongMsg,
    (PFNVOID)midi_OutMessage,
    (PFNVOID)midi_OutOpen,
    (PFNVOID)midi_OutPrepareHeader,
    (PFNVOID)midi_OutReset,
    (PFNVOID)midi_OutSetVolume,
    (PFNVOID)midi_OutShortMsg,
    (PFNVOID)midi_OutUnprepareHeader,

    (PFNVOID)midi_StreamClose,
    (PFNVOID)midi_StreamOpen,
    (PFNVOID)midi_StreamOut,
    (PFNVOID)midi_StreamPause,
    (PFNVOID)midi_StreamPosition,
    (PFNVOID)midi_StreamProperty,
    (PFNVOID)midi_StreamRestart,
    (PFNVOID)midi_StreamStop,
#endif    
};

static const DWORD dwAudioArgs[] = {
	0,	
    ARG0_MAKE(),//waveOutGetNumDevs(void);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);,
    ARG2_MAKE( DWORD, PTR ),//waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
	ARG2_MAKE( DWORD, DWORD ),//waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
	ARG3_MAKE( DWORD, PTR, DWORD ),//waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
	ARG1_MAKE( DWORD ),//waveOutClose(HWAVEOUT hwo);
	ARG3_MAKE( DWORD, PTR, DWORD ),//waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
	ARG3_MAKE( DWORD, PTR, DWORD ),//waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
    ARG1_MAKE( DWORD ),//waveOutPause(HWAVEOUT hwo);
    ARG1_MAKE( DWORD ),//waveOutRestart(HWAVEOUT hwo);
	ARG1_MAKE( DWORD ),//waveOutReset(HWAVEOUT hwo);
    ARG1_MAKE( DWORD ),//waveOutBreakLoop(HWAVEOUT hwo);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
	ARG2_MAKE( DWORD, PTR ),//waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
    ARG2_MAKE( DWORD, DWORD ),//waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
    ARG2_MAKE( DWORD, PTR ),//waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
    ARG2_MAKE( DWORD, DWORD ),//waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
    ARG2_MAKE( DWORD, PTR ),//waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID);
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD ),//waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
    ARG6_MAKE( PTR, DWORD, PTR, DWORD, DWORD, DWORD ),//waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);

//  Audio Record
    ARG0_MAKE(),//waveInGetNumDevs(void);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
    ARG1_MAKE( DWORD ),//waveInClose(HWAVEIN hwi);
	ARG3_MAKE( DWORD, PTR, DWORD ),//waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
    ARG1_MAKE( DWORD ),//waveInStart(HWAVEIN hwi);
    ARG1_MAKE( DWORD ),//waveInStop(HWAVEIN hwi);
    ARG1_MAKE( DWORD ),//waveInReset(HWAVEIN hwi);
    ARG3_MAKE( DWORD, PTR, DWORD ),//waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
    ARG2_MAKE( DWORD, PTR ),//waveInGetID(HWAVEIN hwi, LPUINT puDeviceID);
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD),//waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
    ARG6_MAKE( PTR, DWORD, PTR, DWORD, DWORD, DWORD ),//waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);

#ifdef MIDI_SUPPORT    
//
    ARG3_MAKE( DWORD,DWORD,PTR),	//MMRESULT WINAPI midiConnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);
    ARG3_MAKE( DWORD,DWORD,PTR),	//MMRESULT WINAPI midiDisconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiInAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiInClose(HMIDIIN hMidiIn);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiInGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiInGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText );
    ARG2_MAKE( DWORD, PTR ),		//MMRESULT WINAPI midiInGetID(HMIDIIN hmi,LPUINT puDeviceID);
    ARG0_MAKE(),					//UINT WINAPI midiInGetNumDevs(VOID);
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD),//DWORD WINAPI midiInMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2);
    ARG5_MAKE( PTR, DWORD, DWORD, DWORD, DWORD),//MMRESULT WINAPI midiInOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiInPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiInReset(HMIDIIN hMidiIn);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiInStart(HMIDIIN hMidiIn);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiInStop(HMIDIIN hMidiIn);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiInUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
    ARG4_MAKE( DWORD, DWORD,PTR, DWORD),//MMRESULT WINAPI midiOutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags);
    ARG4_MAKE( DWORD, DWORD,PTR, DWORD),//MMRESULT WINAPI midiOutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags);
    ARG1_MAKE( DWORD ),				//MRESULT WINAPI midiOutClose(HMIDIOUT hmo);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiOutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps);
    ARG3_MAKE( DWORD,PTR,DWORD),	//UINT WINAPI midiOutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText);
    ARG2_MAKE( DWORD, PTR ),		//MMRESULT WINAPI midiOutGetID(HMIDIOUT hmo,LPUINT puDeviceID);
    ARG0_MAKE(),					//UINT WINAPI midiOutGetNumDevs(VOID);
    ARG2_MAKE( DWORD, PTR ),		//MMRESULT WINAPI midiOutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiOutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
    ARG4_MAKE( DWORD, DWORD, DWORD, DWORD),//DWORD WINAPI midiOutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2);
    ARG5_MAKE( PTR, DWORD, DWORD, DWORD, DWORD),//UINT WINAPI midiOutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);
    ARG2_MAKE( DWORD, DWORD ),		//MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo,DWORD dwVolume);
    ARG2_MAKE( DWORD, DWORD ),		//MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiStreamClose(HMIDISTRM hStream);
    ARG6_MAKE( PTR,PTR, DWORD, DWORD, DWORD, DWORD ),//MMRESULT WINAPI midiStreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiStreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiStreamPause(HMIDISTRM hMidiStream);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiStreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt);
    ARG3_MAKE( DWORD,PTR,DWORD),	//MMRESULT WINAPI midiStreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiStreamRestart(HMIDISTRM hMidiStream);
    ARG1_MAKE( DWORD ),				//MMRESULT WINAPI midiStreamStop(HMIDISTRM hMidiStream);
#endif    
};

////////////////////////////

//#include <filesys\main\fsmain.c>
//#include <filesys\main\fsdmgr.c>
DWORD WINAPI WavDriversThread(DWORD dwParam);


// **************************************************
// 声明：void InstallAudioServer( void )
// 参数：
// 	无
// 
// 返回值：无
// 功能描述：安装声音服务。
// 引用: 
// **************************************************
void InstallAudioServer( void )
{
    RETAILMSG(1, ("InstallAudioServer...\r\n") );
//	API_Register( API_AUDIO,  (PFNVOID)lpAudioAPI, sizeof( lpAudioAPI ) / sizeof(PFNVOID) );
	API_RegisterEx( API_AUDIO,  lpAudioAPI, dwAudioArgs, sizeof( lpAudioAPI ) / sizeof(PFNVOID) );  // 注册声音到API系统
    RETAILMSG(1, ("InstallAudioServer  OK!!!\r\n") );
}

// **************************************************
// 以下程序目前系统没有使用，曾使用独立的声音系统，现
// 声音是属于DEVICE.EXE。
// **************************************************


HANDLE hAudioEvent;

// **************************************************
// 声明：int WINAPI WinMain_AudioApi(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR     lpCmdLine,int nCmdShow)
// 参数：
// 	IN hInstance -- 当前应用程序的实例句柄
// 	IN hPrevInstance -- 前一个应用程序的实例句柄
// 	IN lpCmdLine -- 运行参数
// 	IN nCmdShow -- 显示标志
// 
// 返回值：无
// 功能描述：应用程序入口参数。
// 引用: 
// **************************************************
int WINAPI WinMain_AudioApi(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
{
//	HANDLE hThd;

    RETAILMSG(1, ("InitAudio...\r\n") );
/*
    hThd = CreateThread(0, 0, WavDriversThread, NULL, 0, NULL);
    if (!hThd) 
	{
	    RETAILMSG(1, ("InitAudio Success!!!\r\n") );
        return 0;
    }
    RETAILMSG(1, ("InitAudio Failure!!!\r\n") );
*/
	InstallAudioServer();  // 安装声音服务程序

	hAudioEvent = CreateEvent( NULL, FALSE, FALSE, "Audio_event" );  // 创建事件

	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hAudioEvent, INFINITE );  // 等待事件
		break;
	}
	
	CloseHandle( hAudioEvent );  // 关闭句柄
	
	return 0;
}

// **************************************************
// 声明：DWORD WINAPI WavDriversThread(DWORD dwParam)
// 参数：
// 	IN dwParam -- 线程参数
// 
// 返回值：无
// 功能描述：声音驱动线程。
// 引用: 
// **************************************************
DWORD WINAPI WavDriversThread(DWORD dwParam)
{

    RETAILMSG(1, ("WavDriversThread...\r\n") );

	InstallAudioServer();  // 安装声音服务程序
	hAudioEvent = CreateEvent( NULL, FALSE, FALSE, "Audio_event" );  // 创建事件

//	CreateThread()
	while( 1 )
	{
		int rv;
		rv = WaitForSingleObject( hAudioEvent, INFINITE );  // 等待事件
		break;
	}
	
	CloseHandle( hAudioEvent );  // 关闭句柄

	return 0;
}

