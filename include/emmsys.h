/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EMMSYS_H
#define __EMMSYS_H

#include "edef.h"

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus 


//#define MIDI_SUPPORT    
#undef MIDI_SUPPORT   // 不支持MIDI

// 为了与MicroSoft兼容,提供给用户的内容定义与MicroSoft一致

// 声音数据结构
typedef struct wavehdr_tag {
    LPSTR					lpData;                 // 指向一个数据缓存，放音时存放要播放的数据，录音时存放录到的数据。
    DWORD					dwBufferLength;         // 数据长度，录音时为缓存大小
    DWORD					dwBytesRecorded;        // 录到的数据长度
    DWORD					dwUser;                 // 用户自定义使用，系统不使用
    DWORD					dwFlags;                // 设置当前数据的播放标志与状态 见下面定义
    DWORD					dwLoops;                // 重放次数，仅在标志设置为WHDR_BEGINLOOP时有效
    struct wavehdr_tag FAR *lpNext;					// 系统保留使用
    DWORD					reserved;               // 系统保留使用
} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;

// WAVEHDR 中的dwFlags标志定义
#define WHDR_DONE       0x00000001  // 完成标志，有系统设置，用户设置无效
#define WHDR_PREPARED   0x00000002  // 缓存已经准备好标志，有系统设置，用户设置无效
#define WHDR_BEGINLOOP  0x00000004  // 重放开始标志，到WHDR_ENDLOOP为止将重放dwLoops次
#define WHDR_ENDLOOP    0x00000008  // 重放结束标志
#define WHDR_INQUEUE    0x00000010  // 系统内部使用标志


typedef HANDLE HWAVEIN;
typedef HANDLE HWAVEOUT;

typedef HWAVEIN *LPHWAVEIN;
typedef HWAVEOUT *LPHWAVEOUT;

// 产品名称的最大长度
#define MAXPNAMELEN		32

// 声音输出设备能力
typedef struct tagWAVEOUTCAPS {
    WORD		wMid;					// 制造商ID号
    WORD		wPid;					// 产品ID号
    UINT		vDriverVersion;			// 驱动程序版本
    TCHAR		szPname[MAXPNAMELEN];	// 产品名称
    DWORD		dwFormats;				// 支持的声音格式，详细格式见下面定义
    WORD		wChannels;				// 支持声音输出通道
    WORD		wReserved1;				// 保留
    DWORD		dwSupport;				// 支持其他能力，见下面定义
} WAVEOUTCAPS, *PWAVEOUTCAPS, *NPWAVEOUTCAPS, *LPWAVEOUTCAPS;

// 声音输入设备能力
typedef struct tagWAVEINCAPS {
    WORD		wMid;					// 制造商ID号
    WORD		wPid;					// 产品ID号
    UINT		vDriverVersion;			// 驱动程序版本
    TCHAR		szPname[MAXPNAMELEN];	// 产品名称
    DWORD		dwFormats;				// 支持的声音格式，详细格式见下面定义
    WORD		wChannels;				// 支持声音输出通道
    WORD		wReserved1;				// 保留
} WAVEINCAPS, *PWAVEINCAPS, *NPWAVEINCAPS, *LPWAVEINCAPS;

// 声音格式定义

#define WAVE_FORMAT_1M08       0x00000001       /* 11.025 kHz, 单声道, 8-bit  */
#define WAVE_FORMAT_1S08       0x00000002       /* 11.025 kHz, 立体声, 8-bit  */
#define WAVE_FORMAT_1M16       0x00000004       /* 11.025 kHz, 单声道, 16-bit */
#define WAVE_FORMAT_1S16       0x00000008       /* 11.025 kHz, 立体声, 16-bit */
#define WAVE_FORMAT_2M08       0x00000010       /* 22.05  kHz, 单声道, 8-bit  */
#define WAVE_FORMAT_2S08       0x00000020       /* 22.05  kHz, 立体声, 8-bit  */
#define WAVE_FORMAT_2M16       0x00000040       /* 22.05  kHz, 单声道, 16-bit */
#define WAVE_FORMAT_2S16       0x00000080       /* 22.05  kHz, 立体声, 16-bit */
#define WAVE_FORMAT_4M08       0x00000100       /* 44.1   kHz, 单声道, 8-bit  */
#define WAVE_FORMAT_4S08       0x00000200       /* 44.1   kHz, 立体声, 8-bit  */
#define WAVE_FORMAT_4M16       0x00000400       /* 44.1   kHz, 单声道, 16-bit */
#define WAVE_FORMAT_4S16       0x00000800       /* 44.1   kHz, 立体声, 16-bit */

// 支持的其他能力定义
#define WAVECAPS_PITCH          0x0001   // 支持pitch控制
#define WAVECAPS_PLAYBACKRATE   0x0002   // 支持录音重放速率控制
#define WAVECAPS_VOLUME         0x0004   // 支持音量控制
#define WAVECAPS_LRVOLUME       0x0008   // 支持左右声道音量控制
#define WAVECAPS_SYNC           0x0010   // 支持同步
#define WAVECAPS_SAMPLEACCURATE 0x0020	 // 支持可以得到样本精确位置信息


// 声音格式定义结构，在打开一个声音的时候使用，即要打开一个什么样的声音
typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         // 声音格式的类型，具体见下面定义
    WORD        nChannels;          // 声音通道数目
    DWORD       nSamplesPerSec;     // 数据要求的播放速率
    DWORD       nAvgBytesPerSec;    // 平均每秒钟的数据量，按BYTE计
    WORD        nBlockAlign;        // 数据的块大小，WAVE_FORMAT_PCM下是每一个SAMPLE的大小（包含左右声道）
    WORD        wBitsPerSample;     // 每一个单声道数据的Bits，只支持8位，16位两种
    WORD        cbSize;             // 在cbSize后还有多少个BYTE附加数据
} WAVEFORMATEX, *PWAVEFORMATEX,*LPWAVEFORMATEX;

typedef const WAVEFORMATEX * LPCWAVEFORMATEX;

// 声音格式的类型
#define WAVE_FORMAT_PCM     1   // 没有任何压缩格式的声音数据流

/* PCM 的数据格式结构*/
typedef struct tagPCMWAVEFORMAT {
    WAVEFORMATEX  wf;					// 标准的数据格式结构
    WORD        wBitsPerSample;        // PCM 特有的格式
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT       *PPCMWAVEFORMAT;

// 声音处理长度结构
typedef struct mmtime_tag
{
    UINT            wType;      // 指出使用了联合中的哪一项，即度量声音处理长度的类型，定义见下
    union
    {
		// TIME_MS
		DWORD       ms;         // 毫秒数
		// TIME_SAMPLES
		DWORD       sample;     // 样本数
		// TIME_BYTES
		DWORD       cb;         // BYTE 数
		// TIME_TICKS
		DWORD       ticks;      // MIDI流的滴答数

		// TIME_SMPTE
		struct
		{
			BYTE    hour;       // 小时
			BYTE    min;        // 分
			BYTE    sec;        // 秒
			BYTE    frame;      // 帧数
			BYTE    fps;        // 每秒钟的帧数 ，例电影每秒24帧
			BYTE    dummy;      // 保留
		} smpte;

		// TIME_MIDI 
		struct
		{
			DWORD songptrpos;   // 歌曲指针位置
		} midi;
    } u;
} MMTIME, *PMMTIME, NEAR *NPMMTIME, FAR *LPMMTIME;

// 度量声音处理长度的类型
#define TIME_MS         0x0001  // 用毫秒度量长度
#define TIME_SAMPLES    0x0002  // 用处理的样本数来度量
#define TIME_BYTES      0x0004  // 用已经处理过的BYTE数来度量
#define TIME_SMPTE      0x0008  // 用标准时间来度量（时分秒），	电影与电视工程师协会时间
#define TIME_MIDI       0x0010  // MIDI时间
#define TIME_TICKS      0x0020  // MIDI流的滴答数

// 打开声音时的标记
#define  WAVE_FORMAT_QUERY     0x00000001   // 仅仅是查询是否可以打开当前的声音格式
#define  WAVE_ALLOWSYNC        0x00000002	// 允许重复
#define  WAVE_MAPPER           0x00000004	
#define  WAVE_FORMAT_DIRECT    0x00000008



typedef DWORD	MMRESULT;

// MIDI PART 

typedef HANDLE HMIDI;
typedef HANDLE HMIDIIN;
typedef HANDLE HMIDIOUT;
typedef HANDLE HMIDISTRM;

typedef HMIDIIN *LPHMIDIIN;
typedef HMIDIOUT *LPHMIDIOUT;
typedef HMIDISTRM *LPHMIDISTRM;

typedef UINT MMVERSION;

// 输入设备能力结构
typedef struct structMIDIInCaps{ 
    WORD      wMid;  // MIDI 输入设备制造商ID号
    WORD      wPid;  // MIDI 输入设备ID号
    MMVERSION vDriverVersion; // 输入设备驱动程序版本
    CHAR      szPname[MAXPNAMELEN]; // 产品名称 
    DWORD     dwSupport;  // 保留，必须为0
} MIDIINCAPS, *LPMIDIINCAPS; 

// 输出设备能力结构
typedef struct structMIDIOutCaps{ 
    WORD      wMid; // MIDI 输出设备制造商ID号
    WORD      wPid; // MIDI 输出设备ID号
    MMVERSION vDriverVersion; // 输出设备驱动程序版本
    CHAR      szPname[MAXPNAMELEN]; // 产品名称
    WORD      wTechnology; // 输出设备类型描述，具体类型见下面输出类型声明
    WORD      wVoices;	// 内部合成器所支持的声音数目
    WORD      wNotes; // 内部合成器支持同时播放的声音数目
    WORD      wChannelMask; // 内部合成设备支持的通道数目
    DWORD     dwSupport; // 设备支持的可选功能，具体见下面的可选功能说明
} MIDIOUTCAPS,*LPMIDIOUTCAPS; 
 
// 输出设备类型声明
#define MOD_MIDIPORT	0x0001		// 设备是一个MIDI硬件端口. 
#define MOD_SYNTH 		0x0002		// 设备是一个合成器. 
#define MOD_SQSYNTH 	0x0004		// 设备是一个方波合成器. 
#define MOD_FMSYNTH 	0x0008		// 设备是一个调频合成器. 
#define MOD_MAPPER 		0x0010		// 设备是一个微软MIDI映射器. 
#define MOD_WAVETABLE 	0x0020		// 设备是一个硬波表合成器. 
#define MOD_SWSYNTH 	0x0040		// 设备是一个软波表合成器. 

// 可选功能说明

#define MIDICAPS_CACHE		0x0001	// 支持声音修补高速缓存. 
#define MIDICAPS_LRVOLUME	0x0002	// 支持分开的左右音量控制
#define MIDICAPS_STREAM		0x0004	// 支持直接的MIDI流输出功能
#define MIDICAPS_VOLUME		0x0008	// 支持音量控制

// MIDI 数据头结构
typedef struct midihdr_tag{ 
    LPSTR  lpData;	// MIDI数据缓存指针 
    DWORD  dwBufferLength;  // 缓存大小
    DWORD  dwBytesRecorded;  // 数据大小
    DWORD  dwUser; // 用户自定义数据
    DWORD  dwFlags; // 当前的数据处理状况,见下面定义的数据处理标志
    struct midihdr_tag * lpNext;  //保留，
    DWORD  reserved;  // 保留
    DWORD  dwOffset;  // 已经处理过的数据
    DWORD  dwReserved[4];  // 保留
} MIDIHDR,* LPMIDIHDR; 
 
// 数据处理标志
#define MHDR_DONE       0x00000001  // 完成标志，有系统设置，用户设置无效
#define MHDR_PREPARED   0x00000002  // 缓存已经准备好标志，有系统设置，用户设置无效
#define MHDR_ISSTRM    0x00000010   // 当前缓存是一个流缓存标志
#define MHDR_INQUEUE    0x00000010  // 系统内部正在播放标志

 
// Audio api 输出接口
// 得到设备数目
#define waveOutGetNumDevs  aud_waveOutGetNumDevs
UINT     WINAPI waveOutGetNumDevs(void);
// 得到设备能力，即设备可以播放那些声音，见WAVEOUTCAPS定义
#define waveOutGetDevCaps  aud_waveOutGetDevCaps
MMRESULT WINAPI waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
// 得到输出音量大小
#define waveOutGetVolume  aud_waveOutGetVolume
MMRESULT WINAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
// 设置输出音量大小
#define waveOutSetVolume  aud_waveOutSetVolume
MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
// 得到输出错误文本
#define waveOutGetErrorText  aud_waveOutGetErrorText
MMRESULT WINAPI waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
// 关闭输出声音
#define waveOutClose  aud_waveOutClose
MMRESULT WINAPI waveOutClose(HWAVEOUT hwo);
// 准备输出数据头
#define waveOutPrepareHeader  aud_waveOutPrepareHeader
MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
// 卸除准备输出数据头
#define waveOutUnprepareHeader  aud_waveOutUnprepareHeader
MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
// 写输出数据
#define waveOutWrite  aud_waveOutWrite
MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
// 输出暂停
#define waveOutPause  aud_waveOutPause
MMRESULT WINAPI waveOutPause(HWAVEOUT hwo);
// 输出开始
#define waveOutRestart  aud_waveOutRestart
MMRESULT WINAPI waveOutRestart(HWAVEOUT hwo);
// 输出复位
#define waveOutReset  aud_waveOutReset
MMRESULT WINAPI waveOutReset(HWAVEOUT hwo);
// 退出输出循环
#define waveOutBreakLoop  aud_waveOutBreakLoop
MMRESULT WINAPI waveOutBreakLoop(HWAVEOUT hwo);
// 得到输出位置
#define waveOutGetPosition  aud_waveOutGetPosition
MMRESULT WINAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
// 得到输出pitch
#define waveOutGetPitch  aud_waveOutGetPitch
MMRESULT WINAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
// 设置输出pitch
#define waveOutSetPitch  aud_waveOutSetPitch
MMRESULT WINAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
// 得到录音重放速率
#define waveOutGetPlaybackRate  aud_waveOutGetPlaybackRate
MMRESULT WINAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
// 设置录音重放速率
#define waveOutSetPlaybackRate  aud_waveOutSetPlaybackRate
MMRESULT WINAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
// 得到输出设备ID
#define waveOutGetID  aud_waveOutGetID
MMRESULT WINAPI waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID);
// 输出消息处理
#define waveOutMessage  aud_waveOutMessage
MMRESULT WINAPI waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
// 打开输出声音
#define waveOutOpen  aud_waveOutOpen
MMRESULT WINAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);


// Audio api 输出接口
// 得到输入设备数目
#define waveInGetNumDevs  aud_waveInGetNumDevs
UINT     WINAPI waveInGetNumDevs(void);
// 得到输入设备的能力
#define waveInGetDevCaps  aud_waveInGetDevCaps
MMRESULT WINAPI waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
// 得到输入错误文本
#define waveInGetErrorText  aud_waveInGetErrorText
MMRESULT WINAPI waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
// 关闭声音输入
#define waveInClose  aud_waveInClose
MMRESULT WINAPI waveInClose(HWAVEIN hwi);
// 准备输入缓存头
#define waveInPrepareHeader  aud_waveInPrepareHeader
MMRESULT WINAPI waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
// 卸除准备输入缓存头
#define waveInUnprepareHeader  aud_waveInUnprepareHeader
MMRESULT WINAPI waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
// 添加输入缓存
#define waveInAddBuffer  aud_waveInAddBuffer
MMRESULT WINAPI waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
// 开始声音输入
#define waveInStart  aud_waveInStart
MMRESULT WINAPI waveInStart(HWAVEIN hwi);
// 停止声音输入
#define waveInStop  aud_waveInStop
MMRESULT WINAPI waveInStop(HWAVEIN hwi);
// 重置声音输入
#define waveInReset  aud_waveInReset
MMRESULT WINAPI waveInReset(HWAVEIN hwi);
// 得到声音输入长度
#define waveInGetPosition  aud_waveInGetPosition
MMRESULT WINAPI waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
// 得到输入设备ID号
#define waveInGetID  aud_waveInGetID
MMRESULT WINAPI waveInGetID(HWAVEIN hwi, UINT *puDeviceID);
// 声音输入消息处理
#define waveInMessage  aud_waveInMessage
MMRESULT WINAPI waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
// 打开一个声音输入
#define waveInOpen  aud_waveInOpen
MMRESULT WINAPI waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);





// 功能描述：连接一个MIDI输入设备到一个MIDI输出设备
// ********************************************************************
#define midiConnect aud_midiConnect
MMRESULT WINAPI midiConnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);

// 功能描述：断开一个MIDI输入设备到一个MIDI输出设备
#define midiDisconnect	aud_midiDisconnect
MMRESULT WINAPI midiDisconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);

// 功能描述：发送一个输入缓存到一个MIDI输入设备
#define midiInAddBuffer	aud_midiInAddBuffer
MMRESULT WINAPI midiInAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);

// 功能描述：关闭一个MIDI输入设备
#define midiInClose aud_midiInClose
MMRESULT WINAPI midiInClose(HMIDIIN hMidiIn);

// 功能描述：得到MIDI输入设备的能力
#define midiInGetDevCaps aud_midiInGetDevCaps
MMRESULT WINAPI midiInGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps);

// 功能描述：得到错误代码的文本
#define midiInGetErrorText aud_midiInGetErrorText
MMRESULT WINAPI midiInGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText );


// 功能描述：得到MIDI输入设备的ID
#define midiInGetID aud_midiInGetID
MMRESULT WINAPI midiInGetID(HMIDIIN hmi,LPUINT puDeviceID);


// 功能描述：得到当前MIDI输入设备的个数
#define midiInGetNumDevs aud_midiInGetNumDevs
UINT WINAPI midiInGetNumDevs(VOID);


// 功能描述：给一个MIDI输入设备发送一个消息
#define midiInMessage aud_midiInMessage
DWORD WINAPI midiInMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2);



// 功能描述：打开一个指定ID的MIDI输入设备
#define midiInOpen aud_midiInOpen
MMRESULT WINAPI midiInOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);


// 功能描述：准备一个输入缓存。
#define midiInPrepareHeader aud_midiInPrepareHeader
MMRESULT WINAPI midiInPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);


// 功能描述：停止一个输入设备输入数据。
#define midiInReset aud_midiInReset
MMRESULT WINAPI midiInReset(HMIDIIN hMidiIn);


// 功能描述：用一个指定输入设备开始输入数据。
#define midiInStart aud_midiInStart
MMRESULT WINAPI midiInStart(HMIDIIN hMidiIn);


// 功能描述：停止一个输入设备输入数据。
#define midiInStop aud_midiInStop
MMRESULT WINAPI midiInStop(HMIDIIN hMidiIn);


// 功能描述：解除一个已经准备好输入缓存。
#define midiInUnprepareHeader aud_midiInUnprepareHeader
MMRESULT WINAPI midiInUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);


// 功能描述：请求对一个内在的MIDI混合设备的输出缓存的打击乐进行修补处理。
#define midiOutCacheDrumPatches aud_midiOutCacheDrumPatches
MMRESULT WINAPI midiOutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags);


// 功能描述：请求对一个内在的MIDI混合设备的输出缓存的指定装置进行修补处理。
#define midiOutCachePatches aud_midiOutCachePatches
MMRESULT WINAPI midiOutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags);


// 功能描述：关闭一个MIDI输出设备
#define midiOutClose aud_midiOutClose
MMRESULT WINAPI midiOutClose(HMIDIOUT hmo);


// 功能描述：得到MIDI输出设备的能力
#define midiOutGetDevCaps aud_midiOutGetDevCaps
MMRESULT WINAPI midiOutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps);


// 功能描述：得到错误代码的文本
#define midiOutGetErrorText aud_midiOutGetErrorText
UINT WINAPI midiOutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText);


// 功能描述：得到MIDI输出设备的ID
#define midiOutGetID aud_midiOutGetID
MMRESULT WINAPI midiOutGetID(HMIDIOUT hmo,LPUINT puDeviceID);


// 功能描述：得到当前MIDI输出设备的个数
#define midiOutGetNumDevs aud_midiOutGetNumDevs
UINT WINAPI midiOutGetNumDevs(VOID);


// 功能描述：得到当前MIDI输出设备的音量
#define midiOutGetVolume aud_midiOutGetVolume
MMRESULT WINAPI midiOutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume);


// 功能描述：发送一个数据流到一个MIDI输出设备
#define midiOutLongMsg aud_midiOutLongMsg
MMRESULT WINAPI midiOutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);


// 功能描述：给一个MIDI输出设备发送一个消息
#define midiOutMessage aud_midiOutMessage
DWORD WINAPI midiOutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2);


// 功能描述：打开一个指定ID的MIDI输出设备
#define midiOutOpen aud_midiOutOpen
UINT WINAPI midiOutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);


// 功能描述：准备一个输出缓存。
#define midiOutPrepareHeader aud_midiOutPrepareHeader
MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);


// 功能描述：停止一个输出设备输出数据。
#define midiOutReset aud_midiOutReset
MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);


// 功能描述：设置当前MIDI输出设备的音量
#define midiOutSetVolume aud_midiOutSetVolume
MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo,DWORD dwVolume);

// 功能描述：给当前MIDI输出设备发送一个断的MIDI消息。
#define midiOutShortMsg aud_midiOutShortMsg
MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);


// 功能描述：解除一个已经准备好输出缓存。
#define midiOutUnprepareHeader aud_midiOutUnprepareHeader
MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);


// 功能描述：关闭一个MIDI流句柄。
#define midiStreamClose aud_midiStreamClose
MMRESULT WINAPI midiStreamClose(HMIDISTRM hStream);


// 功能描述：打开一个MIDI流用来输出。
#define midiStreamOpen aud_midiStreamOpen
MMRESULT WINAPI midiStreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen);


// 功能描述：发送一个数据流到一个MIDI流设备
#define midiStreamOut aud_midiStreamOut
MMRESULT WINAPI midiStreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr);


// 功能描述：暂停一个MIDI流设备播放
#define midiStreamPause aud_midiStreamPause
MMRESULT WINAPI midiStreamPause(HMIDISTRM hMidiStream);


// 功能描述：得到MIDI流设备已经播放的位置
#define midiStreamPosition aud_midiStreamPosition
MMRESULT WINAPI midiStreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt);


// 功能描述：得到或设置MIDI流设备的属性
#define midiStreamProperty aud_midiStreamProperty
MMRESULT WINAPI midiStreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty);


// 功能描述：重新开始一个暂停的MIDI流播放。
#define midiStreamRestart aud_midiStreamRestart
MMRESULT WINAPI midiStreamRestart(HMIDISTRM hMidiStream);


// 功能描述：停止一个的MIDI流播放。
#define midiStreamStop aud_midiStreamStop
MMRESULT WINAPI midiStreamStop(HMIDISTRM hMidiStream);


// 声音错误返回值

#define MMSYSERR_BASE			0


#define MMSYSERR_NOERROR      0                    // 没有错误
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  // 一个没有定义的错误
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  // 设备ID找不到
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  // 设备没有激活，即设备是不可用的
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  // 设备已经在使用
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  // 句柄是无效的
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  // 没有设备句柄
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  // 内存不足
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  // 功能不支持
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  // 数值错误
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) // 无效标志
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) // 无效参数
#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) // 句柄正在被使用
#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) // 指定的别名无效
#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) // 错误数据库
#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) // 注册键没有找到
#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) // 读注册表失败
#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) // 写注册表失败
#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) // 删除注册表失败
#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) // 注册值没有找到
#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) // 驱动不能CALL回调

// 播放声音错误标式
#define	WAVERR_BASE				100
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    // 比支持的声音格式
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    // 正在播放
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    // 没有准备好
#define WAVERR_SYNC           (WAVERR_BASE + 3)    // 设备正在重放

// 声音打开时的CallBack类型

#define CALLBACK_TYPEMASK   0x00070000l    // CALLBACK 类型掩码
#define CALLBACK_NULL       0x00000000l    // 没有CALLBACK
#define CALLBACK_WINDOW     0x00010000l    // CALLBACK 是一个窗口句柄
#define CALLBACK_TASK       0x00020000l    // CALLBACK 是一个任务
#define CALLBACK_FUNCTION   0x00030000l    // CALLBACK 是一个函数
#define CALLBACK_THREAD     0x00040000l	   // CALLBACK 是一个线程
#define CALLBACK_EVENT      0x00050000l    // CALLBACK 是一个事件


typedef void (CALLBACK DRVCALLBACK)(HANDLE hdrvr, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef DRVCALLBACK FAR *LPDRVCALLBACK;
typedef DRVCALLBACK     *PDRVCALLBACK;

// CALLBACK为窗口句柄时的窗口消息
#define MM_WOM_OPEN         0x3BB           
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#define MM_WIM_OPEN         0x3BE           
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

// 声音回调消息
#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA


//播放一个声音源

BOOL WINAPI sndPlaySound( LPCTSTR lpszSoundName, UINT fuSound );  // 播放WAV声音文件
#ifdef MIDI_SUPPORT    
BOOL WINAPI midiPlaySound( LPCTSTR lpszSoundName, UINT fuSound ); // 播放MIDI声音文件
#endif
BOOL WINAPI PlaySoundFile( LPCTSTR lpszSoundName, UINT fuSound ); //播放声音文件

int PlayAMRFile(HWND hWnd ,LPTSTR lpFileName); //  播放AMR文件
void StopPlayAMRFile(void); // 停止AMR文件

BOOL WINAPI PlaySound( LPCTSTR pszSound, HANDLE hmod, DWORD fdwSound);

// 声音源的来历
#define SND_ALIAS      0x00010000   // 播放一个系统指定的声音数据
#define SND_FILENAME   0x00020000   // 播放一个声音文件
#define SND_RESOURCE   0x00040000   // 播放一个声音资源

// 播放形式
#define SND_SYNC       0x00000000   // 同步播放
#define SND_ASYNC      0x00000001   // 异步播放
#define SND_NODEFAULT  0x00000002   // 如果没有找到声音源，则不播放默认声音
#define SND_MEMORY     0x00000004   // 声音源指向一个内存文件
#define SND_LOOP       0x00000008   // 循环播放直到下一个声音播放
#define SND_NOSTOP     0x00000010   // 不停止当前正在播放的声音

#define SND_NOWAIT     0x00002000   // 如果系统忙，则不等待


#define AM_SETMICVOLUME          0x0100
#define AM_SETMUTE				 0x0101

#ifdef __cplusplus
}                       
#endif  // __cplusplus


#endif // __EMMSYS_H



