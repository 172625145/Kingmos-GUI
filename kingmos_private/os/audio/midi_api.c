/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：MIDI API函数，API部分
版本号：1.0.0
开发时期：2004-07-06
作者：陈建明 (Jami chen)
修改记录：
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
// 声明：MMRESULT WINAPI midi_Connect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
// 参数：
//	IN hMidi -- MIDI输入设备句柄
//	IN hmo  -- MIDI输出设备句柄
//	IN pReserved -- 保留，必须为NULL
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：连接一个MIDI输入设备到一个MIDI输出设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_Connect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
{
	return MMSYSERR_NOERROR;
}
// ********************************************************************
// 声明：MMRESULT WINAPI midi_Disconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
// 参数：
//	IN hMidi -- MIDI输入设备句柄
//	IN hmo  -- MIDI输出设备句柄
//	IN pReserved -- 保留，必须为NULL
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：断开一个MIDI输入设备到一个MIDI输出设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_Disconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// 声明：MMRESULT WINAPI midi_InAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
//	IN lpMidiInHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiInHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：发送一个输入缓存到一个MIDI输入设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// 声明：MMRESULT WINAPI midi_InClose(HMIDIIN hMidiIn)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：关闭一个MIDI输入设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InClose(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// 声明：MMRESULT WINAPI midi_InGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps)
// 参数：
//	IN uDeviceID -- MIDI输入设备的ID
//	IN lpMidiInCaps  -- 一个指向MIDIINCAPS结构的指针
//	IN cbMidiInCaps -- 结构MIDIINCAPS的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到MIDI输入设备的能力
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// 声明：MMRESULT WINAPI midi_InGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText )
// 参数：
//	IN wError -- 错误代码
//	IN lpText  -- 一个存放错误文本的指针
//	IN cchText -- 文本缓存的大小
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到错误代码的文本
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText )
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_InGetID(HMIDIIN hmi,LPUINT puDeviceID)
// 参数：
//	IN hmi -- MIDI输入设备句柄
//	IN puDeviceID  -- 一个用来填充设备ID的指针
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到MIDI输入设备的ID
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InGetID(HMIDIIN hmi,LPUINT puDeviceID)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：UINT WINAPI midi_InGetNumDevs(VOID)
// 参数：
//	无
// 返回值：
//	成功返回当前MIDI输入设备的个数。
// 功能描述：得到当前MIDI输入设备的个数
// 引用: MIDI API函数接口
// ********************************************************************
UINT WINAPI midi_InGetNumDevs(VOID)
{
	return 0;
}


// ********************************************************************
// 声明：DWORD WINAPI midi_InMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
//	IN msg  -- 要发送的消息
//	IN dw1	-- 消息参数1
//	IN dw2  -- 消息参数2
// 返回值：
//	返回设备返回的值。
// 功能描述：给一个MIDI输入设备发送一个消息
// 引用: MIDI API函数接口
// ********************************************************************
DWORD WINAPI midi_InMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2)
{
	return 0;
}



// ********************************************************************
// 声明：MMRESULT WINAPI midi_InOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
// 参数：
//	OUT lphMidiIn -- 一个指向MIDI输入设备句柄的指针
//	IN uDeviceID  -- 设备ID
//	IN dwCallback	-- 一个指向回调函数，窗口句柄。。。的值，具体有dwFlags指定
//	IN dwCallbackInstance  -- 回调者的实例句柄
//	IN dwFlags -- 回调类型
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：打开一个指定ID的MIDI输入设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_InPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
//	IN lpMidiInHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiInHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：准备一个输入缓存。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_InReset(HMIDIIN hMidiIn)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：停止一个输入设备输入数据。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InReset(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_InStart(HMIDIIN hMidiIn)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：用一个指定输入设备开始输入数据。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InStart(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_InStop(HMIDIIN hMidiIn)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：停止一个输入设备输入数据。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InStop(HMIDIIN hMidiIn)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_InUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// 参数：
//	IN hMidiIn -- MIDI输入设备句柄
//	IN lpMidiInHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiInHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：解除一个已经准备好输入缓存。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_InUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
	return MMSYSERR_NOERROR;
}




// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN wPatch  -- 鼓声的修补数量
//	IN lpKeyArray -- 一个指向KEYARRAY数组的指针，指向打击乐的修补
//	IN wFlags	-- 处理选项
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：请求对一个内在的MIDI混合设备的输出缓存的打击乐进行修补处理。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN wBank  -- 修补数量
//	IN lpPatchArray -- 一个指向PATCHARRAY 数组的指针
//	IN wFlags	-- 处理选项
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：请求对一个内在的MIDI混合设备的输出缓存的指定装置进行修补处理。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutClose(HMIDIOUT hmo)
// 参数：
//	IN hmo -- MIDI输出设备句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：关闭一个MIDI输出设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutClose(HMIDIOUT hmo)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps)
// 参数：
//	IN uDeviceID -- MIDI输出设备的ID
//	IN lpMidiOutCaps  -- 一个指向MIDIOUTCAPS 结构的指针
//	IN cbMidiOutCaps -- 结构MIDIOUTCAPS 的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到MIDI输出设备的能力
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：UINT WINAPI midi_OutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText)
// 参数：
//	IN wError -- 错误代码
//	IN lpText  -- 一个存放错误文本的指针
//	IN cchText -- 文本缓存的大小
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到错误代码的文本
// 引用: MIDI API函数接口
// ********************************************************************
UINT WINAPI midi_OutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutGetID(HMIDIOUT hmo,LPUINT puDeviceID)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN puDeviceID  -- 一个用来填充设备ID的指针
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到MIDI输出设备的ID
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutGetID(HMIDIOUT hmo,LPUINT puDeviceID)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：UINT WINAPI midi_OutGetNumDevs(VOID)
// 参数：
//	无
// 返回值：
//	成功返回当前MIDI输出设备的个数。
// 功能描述：得到当前MIDI输出设备的个数
// 引用: MIDI API函数接口
// ********************************************************************
UINT WINAPI midi_OutGetNumDevs(VOID)
{
	return 0;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	OUT lpdwVolume -- 存放当前音量的指针
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到当前MIDI输出设备的音量
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN lpMidiOutHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiOutHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：发送一个数据流到一个MIDI输出设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：DWORD WINAPI midi_OutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN msg  -- 要发送的消息
//	IN dw1	-- 消息参数1
//	IN dw2  -- 消息参数2
// 返回值：
//	返回设备返回的值。
// 功能描述：给一个MIDI输出设备发送一个消息
// 引用: MIDI API函数接口
// ********************************************************************
DWORD WINAPI midi_OutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2)
{
	return  0;
}


// ********************************************************************
// 声明：UINT WINAPI midi_OutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
// 参数：
//	OUT lphmo -- 一个指向MIDI输出设备句柄的指针
//	IN uDeviceID  -- 设备ID
//	IN dwCallback	-- 一个指向回调函数，窗口句柄。。。的值，具体有dwFlags指定
//	IN dwCallbackInstance  -- 回调者的实例句柄
//	IN dwFlags -- 回调类型
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：打开一个指定ID的MIDI输出设备
// 引用: MIDI API函数接口
// ********************************************************************
UINT WINAPI midi_OutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN lpMidiOutHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiOutHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：准备一个输出缓存。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutReset(HMIDIOUT hmo)
// 参数：
//	IN hmo -- MIDI输出设备句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：停止一个输出设备输出数据。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutReset(HMIDIOUT hmo)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutSetVolume(HMIDIOUT hmo,DWORD dwVolume)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN dwVolume -- 当前要设置的音量
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：设置当前MIDI输出设备的音量
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutSetVolume(HMIDIOUT hmo,DWORD dwVolume)
{
	return MMSYSERR_NOERROR;
}

// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN dwMsg -- 当前要发送的消息
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：给当前MIDI输出设备发送一个断的MIDI消息。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_OutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// 参数：
//	IN hmo -- MIDI输出设备句柄
//	IN lpMidiOutHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiOutHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：解除一个已经准备好输出缓存。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_OutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_StreamClose(HMIDISTRM hStream)
// 参数：
//	IN hStream -- MIDI流句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：关闭一个MIDI流句柄。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamClose(HMIDISTRM hStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		RETAILMSG(1,(TEXT("midi_StreamClose ... \r\n")));
		if (hStream == 0)
			return MMSYSERR_INVALHANDLE; // 无效句柄
		
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
// 声明：MMRESULT WINAPI midi_StreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen)
// 参数：
//	OUT lphStream -- 返回MIDI流句柄
//  IN  puDeviceID -- 指向一个设备ID的指针
//	IN  cMidi -- 保留，必须为1
//	IN dwCallback	-- 一个指向回调函数，窗口句柄。。。的值，具体有fdwOpen指定
//	IN dwInstance  -- 回调者的实例句柄
//	IN fdwOpen -- 回调类型
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：打开一个MIDI流用来输出。
// 引用: MIDI API函数接口
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
			return MMSYSERR_NOMEM; // 内存不够
		}
		g_lpMa5SmafStruct = lpMa5SmafStruct;

		g_lpMa5SmafStruct->hCallerProcess = GetCallerProcess();  // 设置呼叫进程

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
// 声明：MMRESULT WINAPI midi_StreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr)
// 参数：
//	IN hMidiStream -- MIDI流句柄
//	IN lpMidiOutHdr  -- 一个指向MIDIHDR结构的指针
//	IN cbMidiOutHdr -- 结构MIDIHDR的尺寸
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：发送一个数据流到一个MIDI流设备
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiHdr,UINT cbMidiHdr)
{

#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	int iRet;
	int repeat;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // 无效句柄
		
		RETAILMSG(1,(TEXT("midi_StreamOut ... \r\n")));
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;

		lpMidiHdr->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)lpMidiHdr->lpData, GetCallerProcess() );  // 映射参数
		// 将文件系统的数据添加到MA5	
//		lpMa5SmafStruct->file_id = MaSound_Load(lpMa5SmafStruct->func_id,lpMidiHdr->lpData,lpMidiHdr->dwBytesRecorded,0,lpMa5SmafStruct->dwCallBack,NULL);
		lpMa5SmafStruct->file_id = MaSound_Load(lpMa5SmafStruct->func_id,lpMidiHdr->lpData,lpMidiHdr->dwBytesRecorded,0,Ma5Func,NULL);
		
		if (lpMa5SmafStruct->file_id < 0)
		{
			RETAILMSG(1,(TEXT("MaSound_Load Failure \r\n")));
			return MASMW_ERROR;
		}
		
		// 打开MA5
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
		// 开始播放
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
// 声明：MMRESULT WINAPI midi_StreamPause(HMIDISTRM hMidiStream)
// 参数：
//	IN hMidiStream -- MIDI流句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：暂停一个MIDI流设备播放
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamPause(HMIDISTRM hMidiStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // 无效句柄
		
		RETAILMSG(1,(TEXT("midi_StreamPause ... \r\n")));
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;
		MaSound_Pause( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );

		RETAILMSG(1,(TEXT("midi_StreamPause OK \r\n")));
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_StreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt)
// 参数：
//	IN hMidiStream -- MIDI流句柄
//  IN pmmt	-- 一个指向MMTIME结构的指针
//  IN cbmmt -- MMTIME结构的大小
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到MIDI流设备已经播放的位置
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt)
{
	return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_StreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty)
// 参数：
//	IN hMidiStream -- MIDI流句柄
//  IN lppropdata	-- 一个指向属性数据的指针
//  IN dwProperty -- 属性的类型
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：得到或设置MIDI流设备的属性
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty)
{
	return MMSYSERR_NOERROR;
}



// ********************************************************************
// 声明：MMRESULT WINAPI midi_StreamRestart(HMIDISTRM hMidiStream)
// 参数：
//	IN hMidiStream -- MIDI流句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：重新开始一个暂停的MIDI流播放。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamRestart(HMIDISTRM hMidiStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // 无效句柄
		
		lpMa5SmafStruct = (LPMA5SMAFSTRUCT)hMidiStream;
		MaSound_Restart( lpMa5SmafStruct->func_id, lpMa5SmafStruct->file_id, NULL );
#endif
		return MMSYSERR_NOERROR;
}


// ********************************************************************
// 声明：MMRESULT WINAPI midi_StreamStop(HMIDISTRM hMidiStream)
// 参数：
//	IN hMidiStream -- MIDI流句柄
// 返回值：
//	成功返回MMSYSERR_NOERROR，否则返回其他。
// 功能描述：停止一个的MIDI流播放。
// 引用: MIDI API函数接口
// ********************************************************************
MMRESULT WINAPI midi_StreamStop(HMIDISTRM hMidiStream)
{
#ifdef MA5_SMAF_OUT
	LPMA5SMAFSTRUCT lpMa5SmafStruct;
	
		if (hMidiStream == 0)
			return MMSYSERR_INVALHANDLE; // 无效句柄
		
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
// 声明：static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
// 参数：
// 	IN lpwavStruct -- 声音结构
// 	IN uMsg -- 回调消息
// 	IN dw1 -- 参数1
// 	IN dw2 -- 参数2
// 
// 返回值：无
// 功能描述：回调应用程序回调函数。
// 引用: 
// **************************************************
static void CallApFunction(BYTE id)
{
	CALLBACKDATA  CallbackData;
	
		CallbackData.hProcess = g_lpMa5SmafStruct->hCallerProcess;  // 设置应用程序进程句柄
		CallbackData.lpfn = (FARPROC)g_lpMa5SmafStruct->dwCallBack; // 设置回调函数指针
		CallbackData.dwArg0 = (DWORD)g_lpMa5SmafStruct; // 设置声音结构
		
		Sys_ImplementCallBack( &CallbackData,id);
}
