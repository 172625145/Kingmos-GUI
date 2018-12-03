/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/
/*****************************************************
�ļ�˵����MIDI API������API����
�汾�ţ�1.0.0
����ʱ�ڣ�2004-07-06
���ߣ��½��� (Jami chen)
�޸ļ�¼��
******************************************************/
#include <eversion.h>

#include "eWindows.h"
#include "EMMSys.h"
#include <Audio_ep.h>
//#include <waveddsi.h>
//#include <mmddk.h>
#include <eDevice.h>
#include <eobjcall.h>
#include <eapisrv.h>
#include "masound.h"					/* MA Sound Player API */

#ifndef INLINE_PROGRAM
#define MA5_SMAF_OUT
#else
#undef MA5_SMAF_OUT
#endif

typedef struct {
	LONG func_id;	
	LONG file_id;

	DWORD dwCallBack;
	DWORD dwFlag;
	HANDLE hCallerProcess;
}MA5SMAFSTRUCT, *LPMA5SMAFSTRUCT;


//static DWORD g_dwCallback = NULL;

static LONG Ma5Func(BYTE id);
static void CallApFunction(BYTE id);

static LPMA5SMAFSTRUCT g_lpMa5SmafStruct = NULL;

// ********************************************************************
// ������MMRESULT WINAPI midi_Connect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
// ������
//	IN hMidi -- MIDI�����豸���
//	IN hmo  -- MIDI����豸���
//	IN pReserved -- ����������ΪNULL
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ��MIDI�����豸��һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_Connect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
{
	return MMSYSERR_NOERROR;
}
// ********************************************************************
// ������MMRESULT WINAPI midi_Disconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
// ������
//	IN hMidi -- MIDI�����豸���
//	IN hmo  -- MIDI����豸���
//	IN pReserved -- ����������ΪNULL
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������Ͽ�һ��MIDI�����豸��һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_Disconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// ������MMRESULT WINAPI midi_InAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN lpMidiInHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiInHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ�����뻺�浽һ��MIDI�����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// ������MMRESULT WINAPI midi_InClose(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������ر�һ��MIDI�����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InClose(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// ������MMRESULT WINAPI midi_InGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps)
// ������
//	IN uDeviceID -- MIDI�����豸��ID
//	IN lpMidiInCaps  -- һ��ָ��MIDIINCAPS�ṹ��ָ��
//	IN cbMidiInCaps -- �ṹMIDIINCAPS�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI�����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// ������MMRESULT WINAPI midi_InGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText )
// ������
//	IN wError -- �������
//	IN lpText  -- һ����Ŵ����ı���ָ��
//	IN cchText -- �ı�����Ĵ�С
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ����������ı�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText )
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_InGetID(HMIDIIN hmi,LPUINT puDeviceID)
// ������
//	IN hmi -- MIDI�����豸���
//	IN puDeviceID  -- һ����������豸ID��ָ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI�����豸��ID
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InGetID(HMIDIIN hmi,LPUINT puDeviceID)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������UINT WINAPI midi_InGetNumDevs(VOID)
// ������
//	��
// ����ֵ��
//	�ɹ����ص�ǰMIDI�����豸�ĸ�����
// �����������õ���ǰMIDI�����豸�ĸ���
// ����: MIDI API�����ӿ�
// ********************************************************************
UINT WINAPI midi_InGetNumDevs(VOID)
{
	return 0;
}


// ********************************************************************
// ������DWORD WINAPI midi_InMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN msg  -- Ҫ���͵���Ϣ
//	IN dw1	-- ��Ϣ����1
//	IN dw2  -- ��Ϣ����2
// ����ֵ��
//	�����豸���ص�ֵ��
// ������������һ��MIDI�����豸����һ����Ϣ
// ����: MIDI API�����ӿ�
// ********************************************************************
DWORD WINAPI midi_InMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2)
{
	return 0;
}



// ********************************************************************
// ������MMRESULT WINAPI midi_InOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
// ������
//	OUT lphMidiIn -- һ��ָ��MIDI�����豸�����ָ��
//	IN uDeviceID  -- �豸ID
//	IN dwCallback	-- һ��ָ��ص����������ھ����������ֵ��������dwFlagsָ��
//	IN dwCallbackInstance  -- �ص��ߵ�ʵ�����
//	IN dwFlags -- �ص�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������һ��ָ��ID��MIDI�����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_InPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN lpMidiInHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiInHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������׼��һ�����뻺�档
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_InReset(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ�������豸�������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InReset(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_InStart(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������һ��ָ�������豸��ʼ�������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InStart(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_InStop(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ�������豸�������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InStop(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_InUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN lpMidiInHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiInHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������һ���Ѿ�׼�������뻺�档
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_InUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
	return MMSYSERR_NOERROR;
}




// ********************************************************************
// ������MMRESULT WINAPI midi_OutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags)
// ������
//	IN hmo -- MIDI����豸���
//	IN wPatch  -- �������޲�����
//	IN lpKeyArray -- һ��ָ��KEYARRAY�����ָ�룬ָ�����ֵ��޲�
//	IN wFlags	-- ����ѡ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ���������������һ�����ڵ�MIDI����豸���������Ĵ���ֽ����޲�����
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags)
// ������
//	IN hmo -- MIDI����豸���
//	IN wBank  -- �޲�����
//	IN lpPatchArray -- һ��ָ��PATCHARRAY �����ָ��
//	IN wFlags	-- ����ѡ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ���������������һ�����ڵ�MIDI����豸����������ָ��װ�ý����޲�����
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutClose(HMIDIOUT hmo)
// ������
//	IN hmo -- MIDI����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������ر�һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutClose(HMIDIOUT hmo)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps)
// ������
//	IN uDeviceID -- MIDI����豸��ID
//	IN lpMidiOutCaps  -- һ��ָ��MIDIOUTCAPS �ṹ��ָ��
//	IN cbMidiOutCaps -- �ṹMIDIOUTCAPS �ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������UINT WINAPI midi_OutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText)
// ������
//	IN wError -- �������
//	IN lpText  -- һ����Ŵ����ı���ָ��
//	IN cchText -- �ı�����Ĵ�С
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ����������ı�
// ����: MIDI API�����ӿ�
// ********************************************************************
UINT WINAPI midi_OutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutGetID(HMIDIOUT hmo,LPUINT puDeviceID)
// ������
//	IN hmo -- MIDI����豸���
//	IN puDeviceID  -- һ����������豸ID��ָ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI����豸��ID
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutGetID(HMIDIOUT hmo,LPUINT puDeviceID)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������UINT WINAPI midi_OutGetNumDevs(VOID)
// ������
//	��
// ����ֵ��
//	�ɹ����ص�ǰMIDI����豸�ĸ�����
// �����������õ���ǰMIDI����豸�ĸ���
// ����: MIDI API�����ӿ�
// ********************************************************************
UINT WINAPI midi_OutGetNumDevs(VOID)
{
	return 0;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume)
// ������
//	IN hmo -- MIDI����豸���
//	OUT lpdwVolume -- ��ŵ�ǰ������ָ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ���ǰMIDI����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hmo -- MIDI����豸���
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ����������һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������DWORD WINAPI midi_OutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2)
// ������
//	IN hmo -- MIDI����豸���
//	IN msg  -- Ҫ���͵���Ϣ
//	IN dw1	-- ��Ϣ����1
//	IN dw2  -- ��Ϣ����2
// ����ֵ��
//	�����豸���ص�ֵ��
// ������������һ��MIDI����豸����һ����Ϣ
// ����: MIDI API�����ӿ�
// ********************************************************************
DWORD WINAPI midi_OutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2)
{
	return  0;
}


// ********************************************************************
// ������UINT WINAPI midi_OutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
// ������
//	OUT lphmo -- һ��ָ��MIDI����豸�����ָ��
//	IN uDeviceID  -- �豸ID
//	IN dwCallback	-- һ��ָ��ص����������ھ����������ֵ��������dwFlagsָ��
//	IN dwCallbackInstance  -- �ص��ߵ�ʵ�����
//	IN dwFlags -- �ص�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������һ��ָ��ID��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
UINT WINAPI midi_OutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hmo -- MIDI����豸���
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������׼��һ��������档
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutReset(HMIDIOUT hmo)
// ������
//	IN hmo -- MIDI����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ������豸������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutReset(HMIDIOUT hmo)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutSetVolume(HMIDIOUT hmo,DWORD dwVolume)
// ������
//	IN hmo -- MIDI����豸���
//	IN dwVolume -- ��ǰҪ���õ�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������õ�ǰMIDI����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutSetVolume(HMIDIOUT hmo,DWORD dwVolume)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// ������MMRESULT WINAPI midi_OutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
// ������
//	IN hmo -- MIDI����豸���
//	IN dwMsg -- ��ǰҪ���͵���Ϣ
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������ǰMIDI����豸����һ���ϵ�MIDI��Ϣ��
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_OutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hmo -- MIDI����豸���
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������һ���Ѿ�׼����������档
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_OutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamClose(HMIDISTRM hStream)
// ������
//	IN hStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������ر�һ��MIDI�������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamClose(HMIDISTRM hStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		RETAILMSG(1,(TEXT("midi_StreamClose ... \r\n")));
		if (hStream == 0)
			return MMSYSERR_INVALHANDLE; // ��Ч���
		
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hStream;
		
		MaSound_Close(lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );
		
		MaSound_Unload( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );
		
		MaSound_Delete(lpMa5SmafStruct->func_id);

		free(lpMa5SmafStruct);

		g_lpMa5SmafStruct = NULL;

		RETAILMSG(1,(TEXT("midi_StreamClose OK \r\n")));
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen)
// ������
//	OUT lphStream -- ����MIDI�����
//  IN  puDeviceID -- ָ��һ���豸ID��ָ��
//	IN  cMidi -- ����������Ϊ1
//	IN dwCallback	-- һ��ָ��ص����������ھ����������ֵ��������fdwOpenָ��
//	IN dwInstance  -- �ص��ߵ�ʵ�����
//	IN fdwOpen -- �ص�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������һ��MIDI�����������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;

		RETAILMSG(1,(TEXT("midi_StreamOpen ... \r\n")));
		if (lphStream == NULL)
		{
			RETAILMSG(1,(TEXT("MMSYSERR_INVALPARAM ... \r\n")));
			return MMSYSERR_INVALPARAM;
		}
		
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)malloc(sizeof(MA5SMAFSTRUCT));
		if (lpMa5SmafStruct == NULL)
		{
			RETAILMSG(1,(TEXT("ALLOC FAULURE ... \r\n")));
			return MMSYSERR_NOMEM; // �ڴ治��
		}
		g_lpMa5SmafStruct = lpMa5SmafStruct;

		g_lpMa5SmafStruct->hCallerProcess = GetCallerProcess();  // ���ú��н���

		lpMa5SmafStruct->dwCallBack = dwCallback;
		lpMa5SmafStruct->dwFlag = fdwOpen;

		// Initializes and registers SMAF converter.
		RETAILMSG(1,(TEXT("MaSound_Create ... \r\n")));
		lpMa5SmafStruct->func_id = MaSound_Create(MASMW_CNVID_MMF); 
		
		if (lpMa5SmafStruct->func_id < 0)
		{
			// MaSound_Create Failure
			free(lpMa5SmafStruct);
			RETAILMSG(1,(TEXT("MaSound_Create Failure \r\n")));
			return MMSYSERR_ERROR;	
		}
		*lphStream = (HMIDISTRM)lpMa5SmafStruct;

		RETAILMSG(1,(TEXT("midi_StreamOpen OK \r\n")));
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr)
// ������
//	IN hMidiStream -- MIDI�����
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ����������һ��MIDI���豸
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr)
{

#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	int iRet;
	int repeat;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // ��Ч���
		
		RETAILMSG(1,(TEXT("midi_StreamOut ... \r\n")));
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;

		lpMidiHdr->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)lpMidiHdr->lpData, GetCallerProcess() );  // ӳ�����
		// ���ļ�ϵͳ��������ӵ�MA5	
//		lpMa5SmafStruct->file_id = MaSound_Load(lpMa5SmafStruct->func_id,lpMidiHdr->lpData,lpMidiHdr->dwBytesRecorded,0,lpMa5SmafStruct->dwCallBack,NULL);
		lpMa5SmafStruct->file_id = MaSound_Load(lpMa5SmafStruct->func_id,lpMidiHdr->lpData,lpMidiHdr->dwBytesRecorded,0,Ma5Func,NULL);
		
		if (lpMa5SmafStruct->file_id < 0)
		{
			RETAILMSG(1,(TEXT("MaSound_Load Failure \r\n")));
			return MASMW_ERROR;
		}
		
		// ��MA5
		iRet = MaSound_Open( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, 0 ,NULL );
		if (iRet != 0)
		{
			RETAILMSG(1,(TEXT("MaSound_Open Failure \r\n")));
			return MASMW_ERROR;
		}
		/* Standby for playing */
		iRet = MaSound_Standby( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );
		if ( iRet < MASMW_SUCCESS )
		{
			RETAILMSG(1,(TEXT("MaSound_Standby Failure \r\n")));
			return MASMW_ERROR;
		}

		/* Set repeat */
		repeat = 1;
		iRet = MaSound_Control( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, MASMW_SET_REPEAT, &repeat, NULL );
		if ( iRet < MASMW_SUCCESS )
		{
			RETAILMSG(1,(TEXT("MaSound_Control Failure \r\n")));
			return MASMW_ERROR;
		}
		// ��ʼ����
		iRet = MaSound_Start( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, 1,NULL );
		if ( iRet < MASMW_SUCCESS )
		{
			RETAILMSG(1,(TEXT("MaSound_Control Failure \r\n")));
			return MASMW_ERROR;
		}

		RETAILMSG(1,(TEXT("midi_StreamOut OK \r\n")));
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamPause(HMIDISTRM hMidiStream)
// ������
//	IN hMidiStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������ͣһ��MIDI���豸����
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamPause(HMIDISTRM hMidiStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // ��Ч���
		
		RETAILMSG(1,(TEXT("midi_StreamPause ... \r\n")));
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;
		MaSound_Pause( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );

		RETAILMSG(1,(TEXT("midi_StreamPause OK \r\n")));
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt)
// ������
//	IN hMidiStream -- MIDI�����
//  IN pmmt	-- һ��ָ��MMTIME�ṹ��ָ��
//  IN cbmmt -- MMTIME�ṹ�Ĵ�С
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI���豸�Ѿ����ŵ�λ��
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty)
// ������
//	IN hMidiStream -- MIDI�����
//  IN lppropdata	-- һ��ָ���������ݵ�ָ��
//  IN dwProperty -- ���Ե�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�������MIDI���豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty)
{
	return MMSYSERR_NOERROR;
}



// ********************************************************************
// ������MMRESULT WINAPI midi_StreamRestart(HMIDISTRM hMidiStream)
// ������
//	IN hMidiStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������¿�ʼһ����ͣ��MIDI�����š�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamRestart(HMIDISTRM hMidiStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // ��Ч���
		
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;
		MaSound_Restart( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// ������MMRESULT WINAPI midi_StreamStop(HMIDISTRM hMidiStream)
// ������
//	IN hMidiStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ����MIDI�����š�
// ����: MIDI API�����ӿ�
// ********************************************************************
MMRESULT WINAPI midi_StreamStop(HMIDISTRM hMidiStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // ��Ч���
		
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;
		MaSound_Stop( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );
#endif
		return MMSYSERR_NOERROR;
}


static LONG Ma5Func(BYTE id)
{
	RETAILMSG(1,("Ma5Func %d \r\n",id));
	CallApFunction(id);
	return 0;
}

// **************************************************
// ������static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
// ������
// 	IN lpwavStruct -- �����ṹ
// 	IN uMsg -- �ص���Ϣ
// 	IN dw1 -- ����1
// 	IN dw2 -- ����2
// 
// ����ֵ����
// �����������ص�Ӧ�ó���ص�������
// ����: 
// **************************************************
static void CallApFunction(BYTE id)
{
	CALLBACKDATA  CallbackData;
	
		CallbackData.hProcess = g_lpMa5SmafStruct->hCallerProcess;  // ����Ӧ�ó�����̾��
		CallbackData.lpfn = (FARPROC)g_lpMa5SmafStruct->dwCallBack; // ���ûص�����ָ��
		CallbackData.dwArg0 = (DWORD)g_lpMa5SmafStruct; // ���������ṹ
		
		Sys_ImplementCallBack( &CallbackData,id);
}
