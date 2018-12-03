/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EMMSYS_H
#define __EMMSYS_H

#include "edef.h"

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus 


//#define MIDI_SUPPORT    
#undef MIDI_SUPPORT   // ��֧��MIDI

// Ϊ����MicroSoft����,�ṩ���û������ݶ�����MicroSoftһ��

// �������ݽṹ
typedef struct wavehdr_tag {
    LPSTR					lpData;                 // ָ��һ�����ݻ��棬����ʱ���Ҫ���ŵ����ݣ�¼��ʱ���¼�������ݡ�
    DWORD					dwBufferLength;         // ���ݳ��ȣ�¼��ʱΪ�����С
    DWORD					dwBytesRecorded;        // ¼�������ݳ���
    DWORD					dwUser;                 // �û��Զ���ʹ�ã�ϵͳ��ʹ��
    DWORD					dwFlags;                // ���õ�ǰ���ݵĲ��ű�־��״̬ �����涨��
    DWORD					dwLoops;                // �طŴ��������ڱ�־����ΪWHDR_BEGINLOOPʱ��Ч
    struct wavehdr_tag FAR *lpNext;					// ϵͳ����ʹ��
    DWORD					reserved;               // ϵͳ����ʹ��
} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;

// WAVEHDR �е�dwFlags��־����
#define WHDR_DONE       0x00000001  // ��ɱ�־����ϵͳ���ã��û�������Ч
#define WHDR_PREPARED   0x00000002  // �����Ѿ�׼���ñ�־����ϵͳ���ã��û�������Ч
#define WHDR_BEGINLOOP  0x00000004  // �طſ�ʼ��־����WHDR_ENDLOOPΪֹ���ط�dwLoops��
#define WHDR_ENDLOOP    0x00000008  // �طŽ�����־
#define WHDR_INQUEUE    0x00000010  // ϵͳ�ڲ�ʹ�ñ�־


typedef HANDLE HWAVEIN;
typedef HANDLE HWAVEOUT;

typedef HWAVEIN *LPHWAVEIN;
typedef HWAVEOUT *LPHWAVEOUT;

// ��Ʒ���Ƶ���󳤶�
#define MAXPNAMELEN		32

// ��������豸����
typedef struct tagWAVEOUTCAPS {
    WORD		wMid;					// ������ID��
    WORD		wPid;					// ��ƷID��
    UINT		vDriverVersion;			// ��������汾
    TCHAR		szPname[MAXPNAMELEN];	// ��Ʒ����
    DWORD		dwFormats;				// ֧�ֵ�������ʽ����ϸ��ʽ�����涨��
    WORD		wChannels;				// ֧���������ͨ��
    WORD		wReserved1;				// ����
    DWORD		dwSupport;				// ֧�����������������涨��
} WAVEOUTCAPS, *PWAVEOUTCAPS, *NPWAVEOUTCAPS, *LPWAVEOUTCAPS;

// ���������豸����
typedef struct tagWAVEINCAPS {
    WORD		wMid;					// ������ID��
    WORD		wPid;					// ��ƷID��
    UINT		vDriverVersion;			// ��������汾
    TCHAR		szPname[MAXPNAMELEN];	// ��Ʒ����
    DWORD		dwFormats;				// ֧�ֵ�������ʽ����ϸ��ʽ�����涨��
    WORD		wChannels;				// ֧���������ͨ��
    WORD		wReserved1;				// ����
} WAVEINCAPS, *PWAVEINCAPS, *NPWAVEINCAPS, *LPWAVEINCAPS;

// ������ʽ����

#define WAVE_FORMAT_1M08       0x00000001       /* 11.025 kHz, ������, 8-bit  */
#define WAVE_FORMAT_1S08       0x00000002       /* 11.025 kHz, ������, 8-bit  */
#define WAVE_FORMAT_1M16       0x00000004       /* 11.025 kHz, ������, 16-bit */
#define WAVE_FORMAT_1S16       0x00000008       /* 11.025 kHz, ������, 16-bit */
#define WAVE_FORMAT_2M08       0x00000010       /* 22.05  kHz, ������, 8-bit  */
#define WAVE_FORMAT_2S08       0x00000020       /* 22.05  kHz, ������, 8-bit  */
#define WAVE_FORMAT_2M16       0x00000040       /* 22.05  kHz, ������, 16-bit */
#define WAVE_FORMAT_2S16       0x00000080       /* 22.05  kHz, ������, 16-bit */
#define WAVE_FORMAT_4M08       0x00000100       /* 44.1   kHz, ������, 8-bit  */
#define WAVE_FORMAT_4S08       0x00000200       /* 44.1   kHz, ������, 8-bit  */
#define WAVE_FORMAT_4M16       0x00000400       /* 44.1   kHz, ������, 16-bit */
#define WAVE_FORMAT_4S16       0x00000800       /* 44.1   kHz, ������, 16-bit */

// ֧�ֵ�������������
#define WAVECAPS_PITCH          0x0001   // ֧��pitch����
#define WAVECAPS_PLAYBACKRATE   0x0002   // ֧��¼���ط����ʿ���
#define WAVECAPS_VOLUME         0x0004   // ֧����������
#define WAVECAPS_LRVOLUME       0x0008   // ֧������������������
#define WAVECAPS_SYNC           0x0010   // ֧��ͬ��
#define WAVECAPS_SAMPLEACCURATE 0x0020	 // ֧�ֿ��Եõ�������ȷλ����Ϣ


// ������ʽ����ṹ���ڴ�һ��������ʱ��ʹ�ã���Ҫ��һ��ʲô��������
typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         // ������ʽ�����ͣ���������涨��
    WORD        nChannels;          // ����ͨ����Ŀ
    DWORD       nSamplesPerSec;     // ����Ҫ��Ĳ�������
    DWORD       nAvgBytesPerSec;    // ƽ��ÿ���ӵ�����������BYTE��
    WORD        nBlockAlign;        // ���ݵĿ��С��WAVE_FORMAT_PCM����ÿһ��SAMPLE�Ĵ�С����������������
    WORD        wBitsPerSample;     // ÿһ�����������ݵ�Bits��ֻ֧��8λ��16λ����
    WORD        cbSize;             // ��cbSize���ж��ٸ�BYTE��������
} WAVEFORMATEX, *PWAVEFORMATEX,*LPWAVEFORMATEX;

typedef const WAVEFORMATEX * LPCWAVEFORMATEX;

// ������ʽ������
#define WAVE_FORMAT_PCM     1   // û���κ�ѹ����ʽ������������

/* PCM �����ݸ�ʽ�ṹ*/
typedef struct tagPCMWAVEFORMAT {
    WAVEFORMATEX  wf;					// ��׼�����ݸ�ʽ�ṹ
    WORD        wBitsPerSample;        // PCM ���еĸ�ʽ
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT       *PPCMWAVEFORMAT;

// ���������Ƚṹ
typedef struct mmtime_tag
{
    UINT            wType;      // ָ��ʹ���������е���һ����������������ȵ����ͣ��������
    union
    {
		// TIME_MS
		DWORD       ms;         // ������
		// TIME_SAMPLES
		DWORD       sample;     // ������
		// TIME_BYTES
		DWORD       cb;         // BYTE ��
		// TIME_TICKS
		DWORD       ticks;      // MIDI���ĵδ���

		// TIME_SMPTE
		struct
		{
			BYTE    hour;       // Сʱ
			BYTE    min;        // ��
			BYTE    sec;        // ��
			BYTE    frame;      // ֡��
			BYTE    fps;        // ÿ���ӵ�֡�� ������Ӱÿ��24֡
			BYTE    dummy;      // ����
		} smpte;

		// TIME_MIDI 
		struct
		{
			DWORD songptrpos;   // ����ָ��λ��
		} midi;
    } u;
} MMTIME, *PMMTIME, NEAR *NPMMTIME, FAR *LPMMTIME;

// �������������ȵ�����
#define TIME_MS         0x0001  // �ú����������
#define TIME_SAMPLES    0x0002  // �ô����������������
#define TIME_BYTES      0x0004  // ���Ѿ��������BYTE��������
#define TIME_SMPTE      0x0008  // �ñ�׼ʱ����������ʱ���룩��	��Ӱ����ӹ���ʦЭ��ʱ��
#define TIME_MIDI       0x0010  // MIDIʱ��
#define TIME_TICKS      0x0020  // MIDI���ĵδ���

// ������ʱ�ı��
#define  WAVE_FORMAT_QUERY     0x00000001   // �����ǲ�ѯ�Ƿ���Դ򿪵�ǰ��������ʽ
#define  WAVE_ALLOWSYNC        0x00000002	// �����ظ�
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

// �����豸�����ṹ
typedef struct structMIDIInCaps{ 
    WORD      wMid;  // MIDI �����豸������ID��
    WORD      wPid;  // MIDI �����豸ID��
    MMVERSION vDriverVersion; // �����豸��������汾
    CHAR      szPname[MAXPNAMELEN]; // ��Ʒ���� 
    DWORD     dwSupport;  // ����������Ϊ0
} MIDIINCAPS, *LPMIDIINCAPS; 

// ����豸�����ṹ
typedef struct structMIDIOutCaps{ 
    WORD      wMid; // MIDI ����豸������ID��
    WORD      wPid; // MIDI ����豸ID��
    MMVERSION vDriverVersion; // ����豸��������汾
    CHAR      szPname[MAXPNAMELEN]; // ��Ʒ����
    WORD      wTechnology; // ����豸�����������������ͼ����������������
    WORD      wVoices;	// �ڲ��ϳ�����֧�ֵ�������Ŀ
    WORD      wNotes; // �ڲ��ϳ���֧��ͬʱ���ŵ�������Ŀ
    WORD      wChannelMask; // �ڲ��ϳ��豸֧�ֵ�ͨ����Ŀ
    DWORD     dwSupport; // �豸֧�ֵĿ�ѡ���ܣ����������Ŀ�ѡ����˵��
} MIDIOUTCAPS,*LPMIDIOUTCAPS; 
 
// ����豸��������
#define MOD_MIDIPORT	0x0001		// �豸��һ��MIDIӲ���˿�. 
#define MOD_SYNTH 		0x0002		// �豸��һ���ϳ���. 
#define MOD_SQSYNTH 	0x0004		// �豸��һ�������ϳ���. 
#define MOD_FMSYNTH 	0x0008		// �豸��һ����Ƶ�ϳ���. 
#define MOD_MAPPER 		0x0010		// �豸��һ��΢��MIDIӳ����. 
#define MOD_WAVETABLE 	0x0020		// �豸��һ��Ӳ����ϳ���. 
#define MOD_SWSYNTH 	0x0040		// �豸��һ������ϳ���. 

// ��ѡ����˵��

#define MIDICAPS_CACHE		0x0001	// ֧�������޲����ٻ���. 
#define MIDICAPS_LRVOLUME	0x0002	// ֧�ַֿ���������������
#define MIDICAPS_STREAM		0x0004	// ֧��ֱ�ӵ�MIDI���������
#define MIDICAPS_VOLUME		0x0008	// ֧����������

// MIDI ����ͷ�ṹ
typedef struct midihdr_tag{ 
    LPSTR  lpData;	// MIDI���ݻ���ָ�� 
    DWORD  dwBufferLength;  // �����С
    DWORD  dwBytesRecorded;  // ���ݴ�С
    DWORD  dwUser; // �û��Զ�������
    DWORD  dwFlags; // ��ǰ�����ݴ���״��,�����涨������ݴ����־
    struct midihdr_tag * lpNext;  //������
    DWORD  reserved;  // ����
    DWORD  dwOffset;  // �Ѿ������������
    DWORD  dwReserved[4];  // ����
} MIDIHDR,* LPMIDIHDR; 
 
// ���ݴ����־
#define MHDR_DONE       0x00000001  // ��ɱ�־����ϵͳ���ã��û�������Ч
#define MHDR_PREPARED   0x00000002  // �����Ѿ�׼���ñ�־����ϵͳ���ã��û�������Ч
#define MHDR_ISSTRM    0x00000010   // ��ǰ������һ���������־
#define MHDR_INQUEUE    0x00000010  // ϵͳ�ڲ����ڲ��ű�־

 
// Audio api ����ӿ�
// �õ��豸��Ŀ
#define waveOutGetNumDevs  aud_waveOutGetNumDevs
UINT     WINAPI waveOutGetNumDevs(void);
// �õ��豸���������豸���Բ�����Щ��������WAVEOUTCAPS����
#define waveOutGetDevCaps  aud_waveOutGetDevCaps
MMRESULT WINAPI waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
// �õ����������С
#define waveOutGetVolume  aud_waveOutGetVolume
MMRESULT WINAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
// �������������С
#define waveOutSetVolume  aud_waveOutSetVolume
MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
// �õ���������ı�
#define waveOutGetErrorText  aud_waveOutGetErrorText
MMRESULT WINAPI waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
// �ر��������
#define waveOutClose  aud_waveOutClose
MMRESULT WINAPI waveOutClose(HWAVEOUT hwo);
// ׼���������ͷ
#define waveOutPrepareHeader  aud_waveOutPrepareHeader
MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
// ж��׼���������ͷ
#define waveOutUnprepareHeader  aud_waveOutUnprepareHeader
MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
// д�������
#define waveOutWrite  aud_waveOutWrite
MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
// �����ͣ
#define waveOutPause  aud_waveOutPause
MMRESULT WINAPI waveOutPause(HWAVEOUT hwo);
// �����ʼ
#define waveOutRestart  aud_waveOutRestart
MMRESULT WINAPI waveOutRestart(HWAVEOUT hwo);
// �����λ
#define waveOutReset  aud_waveOutReset
MMRESULT WINAPI waveOutReset(HWAVEOUT hwo);
// �˳����ѭ��
#define waveOutBreakLoop  aud_waveOutBreakLoop
MMRESULT WINAPI waveOutBreakLoop(HWAVEOUT hwo);
// �õ����λ��
#define waveOutGetPosition  aud_waveOutGetPosition
MMRESULT WINAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
// �õ����pitch
#define waveOutGetPitch  aud_waveOutGetPitch
MMRESULT WINAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
// �������pitch
#define waveOutSetPitch  aud_waveOutSetPitch
MMRESULT WINAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
// �õ�¼���ط�����
#define waveOutGetPlaybackRate  aud_waveOutGetPlaybackRate
MMRESULT WINAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
// ����¼���ط�����
#define waveOutSetPlaybackRate  aud_waveOutSetPlaybackRate
MMRESULT WINAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
// �õ�����豸ID
#define waveOutGetID  aud_waveOutGetID
MMRESULT WINAPI waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID);
// �����Ϣ����
#define waveOutMessage  aud_waveOutMessage
MMRESULT WINAPI waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
// ���������
#define waveOutOpen  aud_waveOutOpen
MMRESULT WINAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);


// Audio api ����ӿ�
// �õ������豸��Ŀ
#define waveInGetNumDevs  aud_waveInGetNumDevs
UINT     WINAPI waveInGetNumDevs(void);
// �õ������豸������
#define waveInGetDevCaps  aud_waveInGetDevCaps
MMRESULT WINAPI waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
// �õ���������ı�
#define waveInGetErrorText  aud_waveInGetErrorText
MMRESULT WINAPI waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
// �ر���������
#define waveInClose  aud_waveInClose
MMRESULT WINAPI waveInClose(HWAVEIN hwi);
// ׼�����뻺��ͷ
#define waveInPrepareHeader  aud_waveInPrepareHeader
MMRESULT WINAPI waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
// ж��׼�����뻺��ͷ
#define waveInUnprepareHeader  aud_waveInUnprepareHeader
MMRESULT WINAPI waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
// ������뻺��
#define waveInAddBuffer  aud_waveInAddBuffer
MMRESULT WINAPI waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
// ��ʼ��������
#define waveInStart  aud_waveInStart
MMRESULT WINAPI waveInStart(HWAVEIN hwi);
// ֹͣ��������
#define waveInStop  aud_waveInStop
MMRESULT WINAPI waveInStop(HWAVEIN hwi);
// ������������
#define waveInReset  aud_waveInReset
MMRESULT WINAPI waveInReset(HWAVEIN hwi);
// �õ��������볤��
#define waveInGetPosition  aud_waveInGetPosition
MMRESULT WINAPI waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
// �õ������豸ID��
#define waveInGetID  aud_waveInGetID
MMRESULT WINAPI waveInGetID(HWAVEIN hwi, UINT *puDeviceID);
// ����������Ϣ����
#define waveInMessage  aud_waveInMessage
MMRESULT WINAPI waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
// ��һ����������
#define waveInOpen  aud_waveInOpen
MMRESULT WINAPI waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);





// ��������������һ��MIDI�����豸��һ��MIDI����豸
// ********************************************************************
#define midiConnect aud_midiConnect
MMRESULT WINAPI midiConnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);

// �����������Ͽ�һ��MIDI�����豸��һ��MIDI����豸
#define midiDisconnect	aud_midiDisconnect
MMRESULT WINAPI midiDisconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);

// ��������������һ�����뻺�浽һ��MIDI�����豸
#define midiInAddBuffer	aud_midiInAddBuffer
MMRESULT WINAPI midiInAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);

// �����������ر�һ��MIDI�����豸
#define midiInClose aud_midiInClose
MMRESULT WINAPI midiInClose(HMIDIIN hMidiIn);

// �����������õ�MIDI�����豸������
#define midiInGetDevCaps aud_midiInGetDevCaps
MMRESULT WINAPI midiInGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps);

// �����������õ����������ı�
#define midiInGetErrorText aud_midiInGetErrorText
MMRESULT WINAPI midiInGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText );


// �����������õ�MIDI�����豸��ID
#define midiInGetID aud_midiInGetID
MMRESULT WINAPI midiInGetID(HMIDIIN hmi,LPUINT puDeviceID);


// �����������õ���ǰMIDI�����豸�ĸ���
#define midiInGetNumDevs aud_midiInGetNumDevs
UINT WINAPI midiInGetNumDevs(VOID);


// ������������һ��MIDI�����豸����һ����Ϣ
#define midiInMessage aud_midiInMessage
DWORD WINAPI midiInMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2);



// ������������һ��ָ��ID��MIDI�����豸
#define midiInOpen aud_midiInOpen
MMRESULT WINAPI midiInOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);


// ����������׼��һ�����뻺�档
#define midiInPrepareHeader aud_midiInPrepareHeader
MMRESULT WINAPI midiInPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);


// ����������ֹͣһ�������豸�������ݡ�
#define midiInReset aud_midiInReset
MMRESULT WINAPI midiInReset(HMIDIIN hMidiIn);


// ������������һ��ָ�������豸��ʼ�������ݡ�
#define midiInStart aud_midiInStart
MMRESULT WINAPI midiInStart(HMIDIIN hMidiIn);


// ����������ֹͣһ�������豸�������ݡ�
#define midiInStop aud_midiInStop
MMRESULT WINAPI midiInStop(HMIDIIN hMidiIn);


// �������������һ���Ѿ�׼�������뻺�档
#define midiInUnprepareHeader aud_midiInUnprepareHeader
MMRESULT WINAPI midiInUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);


// ���������������һ�����ڵ�MIDI����豸���������Ĵ���ֽ����޲�����
#define midiOutCacheDrumPatches aud_midiOutCacheDrumPatches
MMRESULT WINAPI midiOutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags);


// ���������������һ�����ڵ�MIDI����豸����������ָ��װ�ý����޲�����
#define midiOutCachePatches aud_midiOutCachePatches
MMRESULT WINAPI midiOutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags);


// �����������ر�һ��MIDI����豸
#define midiOutClose aud_midiOutClose
MMRESULT WINAPI midiOutClose(HMIDIOUT hmo);


// �����������õ�MIDI����豸������
#define midiOutGetDevCaps aud_midiOutGetDevCaps
MMRESULT WINAPI midiOutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps);


// �����������õ����������ı�
#define midiOutGetErrorText aud_midiOutGetErrorText
UINT WINAPI midiOutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText);


// �����������õ�MIDI����豸��ID
#define midiOutGetID aud_midiOutGetID
MMRESULT WINAPI midiOutGetID(HMIDIOUT hmo,LPUINT puDeviceID);


// �����������õ���ǰMIDI����豸�ĸ���
#define midiOutGetNumDevs aud_midiOutGetNumDevs
UINT WINAPI midiOutGetNumDevs(VOID);


// �����������õ���ǰMIDI����豸������
#define midiOutGetVolume aud_midiOutGetVolume
MMRESULT WINAPI midiOutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume);


// ��������������һ����������һ��MIDI����豸
#define midiOutLongMsg aud_midiOutLongMsg
MMRESULT WINAPI midiOutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);


// ������������һ��MIDI����豸����һ����Ϣ
#define midiOutMessage aud_midiOutMessage
DWORD WINAPI midiOutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2);


// ������������һ��ָ��ID��MIDI����豸
#define midiOutOpen aud_midiOutOpen
UINT WINAPI midiOutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);


// ����������׼��һ��������档
#define midiOutPrepareHeader aud_midiOutPrepareHeader
MMRESULT WINAPI midiOutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);


// ����������ֹͣһ������豸������ݡ�
#define midiOutReset aud_midiOutReset
MMRESULT WINAPI midiOutReset(HMIDIOUT hmo);


// �������������õ�ǰMIDI����豸������
#define midiOutSetVolume aud_midiOutSetVolume
MMRESULT WINAPI midiOutSetVolume(HMIDIOUT hmo,DWORD dwVolume);

// ��������������ǰMIDI����豸����һ���ϵ�MIDI��Ϣ��
#define midiOutShortMsg aud_midiOutShortMsg
MMRESULT WINAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);


// �������������һ���Ѿ�׼����������档
#define midiOutUnprepareHeader aud_midiOutUnprepareHeader
MMRESULT WINAPI midiOutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);


// �����������ر�һ��MIDI�������
#define midiStreamClose aud_midiStreamClose
MMRESULT WINAPI midiStreamClose(HMIDISTRM hStream);


// ������������һ��MIDI�����������
#define midiStreamOpen aud_midiStreamOpen
MMRESULT WINAPI midiStreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen);


// ��������������һ����������һ��MIDI���豸
#define midiStreamOut aud_midiStreamOut
MMRESULT WINAPI midiStreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr);


// ������������ͣһ��MIDI���豸����
#define midiStreamPause aud_midiStreamPause
MMRESULT WINAPI midiStreamPause(HMIDISTRM hMidiStream);


// �����������õ�MIDI���豸�Ѿ����ŵ�λ��
#define midiStreamPosition aud_midiStreamPosition
MMRESULT WINAPI midiStreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt);


// �����������õ�������MIDI���豸������
#define midiStreamProperty aud_midiStreamProperty
MMRESULT WINAPI midiStreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty);


// �������������¿�ʼһ����ͣ��MIDI�����š�
#define midiStreamRestart aud_midiStreamRestart
MMRESULT WINAPI midiStreamRestart(HMIDISTRM hMidiStream);


// ����������ֹͣһ����MIDI�����š�
#define midiStreamStop aud_midiStreamStop
MMRESULT WINAPI midiStreamStop(HMIDISTRM hMidiStream);


// �������󷵻�ֵ

#define MMSYSERR_BASE			0


#define MMSYSERR_NOERROR      0                    // û�д���
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  // һ��û�ж���Ĵ���
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  // �豸ID�Ҳ���
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  // �豸û�м�����豸�ǲ����õ�
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  // �豸�Ѿ���ʹ��
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  // �������Ч��
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  // û���豸���
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  // �ڴ治��
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  // ���ܲ�֧��
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  // ��ֵ����
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) // ��Ч��־
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) // ��Ч����
#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) // ������ڱ�ʹ��
#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) // ָ���ı�����Ч
#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) // �������ݿ�
#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) // ע���û���ҵ�
#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) // ��ע���ʧ��
#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) // дע���ʧ��
#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) // ɾ��ע���ʧ��
#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) // ע��ֵû���ҵ�
#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) // ��������CALL�ص�

// �������������ʽ
#define	WAVERR_BASE				100
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    // ��֧�ֵ�������ʽ
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    // ���ڲ���
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    // û��׼����
#define WAVERR_SYNC           (WAVERR_BASE + 3)    // �豸�����ط�

// ������ʱ��CallBack����

#define CALLBACK_TYPEMASK   0x00070000l    // CALLBACK ��������
#define CALLBACK_NULL       0x00000000l    // û��CALLBACK
#define CALLBACK_WINDOW     0x00010000l    // CALLBACK ��һ�����ھ��
#define CALLBACK_TASK       0x00020000l    // CALLBACK ��һ������
#define CALLBACK_FUNCTION   0x00030000l    // CALLBACK ��һ������
#define CALLBACK_THREAD     0x00040000l	   // CALLBACK ��һ���߳�
#define CALLBACK_EVENT      0x00050000l    // CALLBACK ��һ���¼�


typedef void (CALLBACK DRVCALLBACK)(HANDLE hdrvr, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef DRVCALLBACK FAR *LPDRVCALLBACK;
typedef DRVCALLBACK     *PDRVCALLBACK;

// CALLBACKΪ���ھ��ʱ�Ĵ�����Ϣ
#define MM_WOM_OPEN         0x3BB           
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD

#define MM_WIM_OPEN         0x3BE           
#define MM_WIM_CLOSE        0x3BF
#define MM_WIM_DATA         0x3C0

// �����ص���Ϣ
#define WOM_OPEN        MM_WOM_OPEN
#define WOM_CLOSE       MM_WOM_CLOSE
#define WOM_DONE        MM_WOM_DONE
#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA


//����һ������Դ

BOOL WINAPI sndPlaySound( LPCTSTR lpszSoundName, UINT fuSound );  // ����WAV�����ļ�
#ifdef MIDI_SUPPORT    
BOOL WINAPI midiPlaySound( LPCTSTR lpszSoundName, UINT fuSound ); // ����MIDI�����ļ�
#endif
BOOL WINAPI PlaySoundFile( LPCTSTR lpszSoundName, UINT fuSound ); //���������ļ�

int PlayAMRFile(HWND hWnd ,LPTSTR lpFileName); //  ����AMR�ļ�
void StopPlayAMRFile(void); // ֹͣAMR�ļ�

BOOL WINAPI PlaySound( LPCTSTR pszSound, HANDLE hmod, DWORD fdwSound);

// ����Դ������
#define SND_ALIAS      0x00010000   // ����һ��ϵͳָ������������
#define SND_FILENAME   0x00020000   // ����һ�������ļ�
#define SND_RESOURCE   0x00040000   // ����һ��������Դ

// ������ʽ
#define SND_SYNC       0x00000000   // ͬ������
#define SND_ASYNC      0x00000001   // �첽����
#define SND_NODEFAULT  0x00000002   // ���û���ҵ�����Դ���򲻲���Ĭ������
#define SND_MEMORY     0x00000004   // ����Դָ��һ���ڴ��ļ�
#define SND_LOOP       0x00000008   // ѭ������ֱ����һ����������
#define SND_NOSTOP     0x00000010   // ��ֹͣ��ǰ���ڲ��ŵ�����

#define SND_NOWAIT     0x00002000   // ���ϵͳæ���򲻵ȴ�


#define AM_SETMICVOLUME          0x0100
#define AM_SETMUTE				 0x0101

#ifdef __cplusplus
}                       
#endif  // __cplusplus


#endif // __EMMSYS_H



