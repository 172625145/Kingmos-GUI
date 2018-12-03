/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：声音API函数，API部分
版本号：1.0.0
开发时期：2003-04-16
作者：陈建明 (Jami chen)
修改记录：
******************************************************/
#include <eversion.h>

#ifndef EML_WIN32

#include "eWindows.h"
#include "EMMSys.h"
#include <Audio_ep.h>
//#include <waveddsi.h>
//#include <mmddk.h>
#include <eDevice.h>
#include <eobjcall.h>
#include <eapisrv.h>

// *****************************************************
// 函数声明区
// *****************************************************
void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2);
//void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1);//, DWORD dw2);

typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam, DWORD wParam );
//typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam);//, DWORD wParam );


// *****************************************************
// 定义区
// *****************************************************
// hwo句柄内容
typedef struct wavStruct{
	HANDLE hWav;   // 声音设备句柄
	UINT uDeviceID;  // 设备标识号
	PWAVEFORMATEX pwfx;  // 声音格式
	DWORD dwCallback;  // 回调变量，可以是函数(Function),窗口句柄(HWND),...
	DWORD dwInstance;  // 实例句柄
	DWORD fdwOpen;   //打开方式
	HANDLE hCallerProcess;  // 呼叫窗口实例句柄
}WAVSTRUCT, *LPWAVSTRUCT;


static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2);

/**********************************************************/

// ********************************************************************
//声明：UINT     WINAPI Audio_waveOutGetNumDevs(void)
//参数：
//	无
//返回值：
//	返回当前系统支持的设备个数
//功能描述：得到设备数目
//引用: wave API函数接口
// ********************************************************************
UINT     WINAPI Audio_waveOutGetNumDevs(void)
{

	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	UINT numDevs;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetNumDevs\r\n");

	// 打开声音端口
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL);   
	if (hWav==INVALID_HANDLE_VALUE)
	{  // 打开失败
		DbgOutString("Can't Open Wav Device\r\n");
		return 0;
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.uDeviceID=0;
	Params.message=AM_OUT_GETNUMDEVS;  // 设置得到设备树目消息
	Params.dwCallBackParam=0;
	Params.lParam=0;
	Params.wParam=0;

	// 发送消息
	numDevs=DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0);  
	CloseHandle(hWav);  // 关闭句柄
	DbgOutString("-waveOutGetNumDevs\r\n");
	return numDevs;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
//参数：
//	UINT uDeviceID   --  设备标识号
//	LPWAVEOUTCAPS pwoc  -- 一个指向输出设备能力的指针
//  UINT cbwoc  --  结构WAVEOUTCAPS的大小，以BYTE为单位
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到设备能力
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetDevCaps\r\n");

	// 打开声音端口
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // 打开失败
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	Params.uDeviceID=uDeviceID;  // 设备标号
	Params.message=AM_OUT_GETDEVCAPS;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pwoc;  // 参数设置
	Params.wParam=cbwoc;
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );   // 映射指针

	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
		CloseHandle(hWav);  // 关闭句柄
 		return MMSYSERR_ERROR;
	}
	//消息成功
	CloseHandle(hWav);   // 关闭句柄
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveOutGetDevCaps\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPDWORD pdwVolume  -- 指向存放音量的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到当前的音量大小
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetVolume\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

	if (lpwavStruct==NULL)
	{  // 没有打开声音
		// 打开声音端口
		hWav=CreateFile("AUD1:", 0 , FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
		if (hWav==INVALID_HANDLE_VALUE)
		{  // 打开失败
			DbgOutString("Can't Open Wav Device\r\n");
			return MMSYSERR_ALLOCATED;
		}
		Params.uDeviceID=0;
	}
	else
	{  // 已经打开声音
		Params.uDeviceID=lpwavStruct->uDeviceID;
		hWav=lpwavStruct->hWav;
	}

	Params.message=AM_OUT_GETVOLUME;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pdwVolume;  // 设置参数
	Params.wParam=0;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // 映射指针
	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 发送消息失败
		if (lpwavStruct==NULL)
		{  // 关闭打开的句柄
			CloseHandle(hWav);
		}
 		return MMSYSERR_ERROR;
	}
	DbgOutString("-waveOutGetVolume\r\n");
	if (lpwavStruct==NULL)
	{
		CloseHandle(hWav); // 关闭句柄
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	DWORD dwVolume  -- 要设置音量大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：设置当前的音量大小
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutSetVolume\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 没有打开声音
		// 打开声音
		hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
		if (hWav==INVALID_HANDLE_VALUE)
		{  // 打开声音失败
			DbgOutString("Can't Open Wav Device\r\n");
			return MMSYSERR_ALLOCATED;
		}
		Params.uDeviceID=0;
	}
	else
	{ // 已经打开声音
		Params.uDeviceID=lpwavStruct->uDeviceID;
		hWav=lpwavStruct->hWav;  // 得到声音句柄
	}

	Params.message=AM_OUT_SETVOLUME;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=dwVolume;// 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 发送消息失败
		if (lpwavStruct==NULL)
		{
			CloseHandle(hWav);  // 关闭句柄
		}
 		return MMSYSERR_ERROR;
	}
	// 发送消息成功
	if (lpwavStruct==NULL)
	{
		CloseHandle(hWav);  // 关闭句柄
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;  // 返回错误
	}
	DbgOutString("-waveOutSetVolume\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//参数：
//	MMRESULT mmrError -- 要得到文本的错误信息
//	LPTSTR pszText  --  指向存放错误文本缓存的指针
//	UINT cchText  -- 缓存的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：设置当前的音量大小
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	DbgOutString("+waveOutGetErrorText\r\n");
	DbgOutString("-waveOutGetErrorText\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：关闭声音输出
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;
	AUDIOMDD_MSG  Params;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveOutClose\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

		if (lpwavStruct==NULL)
		{  // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}
		else
		{ // 已经打开声音
			Params.uDeviceID=lpwavStruct->uDeviceID;
			hWav=lpwavStruct->hWav;  // 得到声音句柄
		}
	
		Params.message=AM_OUT_CLOSE;  // 设置消息
		Params.dwCallBackParam=0;
		Params.lParam=0;// 设置参数
		Params.wParam=0;
		
		// 发送消息
		if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 发送消息失败
			if (lpwavStruct==NULL)
			{
				CloseHandle(hWav);  // 关闭句柄
			}
	 		return MMSYSERR_ERROR;
		}
		// 发送消息成功
		if (lpwavStruct==NULL)
		{
			CloseHandle(hWav);  // 关闭句柄
		}
		

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // 要求返回窗口
			DbgOutString("will Call back Window\r\n");
			SendMessage((HWND)lpwavStruct->dwCallback,MM_WOM_CLOSE,0,0);  // 发送关闭声音的消息给窗口
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{  // 要求回调线程
			DbgOutString("will Call back Thread\r\n");
			return MMSYSERR_NOERROR;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // 要求回调事件
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{ // 要求回调函数
			DbgOutString("will Call back Function\r\n");
			
			CallApFunction(lpwavStruct,WOM_CLOSE,0,0);  // 回调函数通知要关闭声音

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WOM_CLOSE,lpwavStruct->dwInstance,0,0);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // 没有要求回调
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // 没有设置回调
			DbgOutString("Vaild Parameter\r\n");
		}

		EdbgOutputDebugString("lpwavStruct in waveOutClose %X\r\n",lpwavStruct);
		CloseHandle(lpwavStruct->hWav);  // 关闭声音句柄
		DbgOutString("CloseHandle Complete\r\n");
		
		if (lpwavStruct->pwfx) // 释放空间
			free(lpwavStruct->pwfx);
		free(lpwavStruct);
		DbgOutString("-waveOutClose\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：准备声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	//DbgOutString("+waveOutPrepareHeader\r\n");
	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // 映射参数
	pwh->dwFlags |= WHDR_PREPARED;    // 设置已经准备好的标志
	//DbgOutString("-waveOutPrepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：卸除声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	//DbgOutString("+waveOutUnprepareHeader\r\n");
	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() ); // 映射指针
	pwh->dwFlags &= ~WHDR_PREPARED; // 清除标志
	//DbgOutString("-waveOutUnprepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：写声音数据块到设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

//	RETAILMSG(1,("+waveOutWrite\r\n"));

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // 映射指针

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_WRITE;  // 设置消息
	Params.dwCallBackParam=0;//lpwavStruct;
	Params.lParam=(DWORD)pwh;  // 设置参数
	Params.wParam=0;

//	RETAILMSG(1,("lpwavStruct in Write %X\r\n",lpwavStruct));
//	RETAILMSG(1,("lpwavStruct->hWav = %X\r\n",lpwavStruct->hWav));
//	RETAILMSG(1,("lpwavStruct->uDeviceID = %X\r\n",lpwavStruct->uDeviceID));
//	RETAILMSG(1,("lpwavStruct->pwfx = %X\r\n",lpwavStruct->pwfx));
//	RETAILMSG(1,("lpwavStruct->dwCallback = %X\r\n",lpwavStruct->dwCallback));
//	RETAILMSG(1,("lpwavStruct->dwInstance = %X\r\n",lpwavStruct->dwInstance));
//	RETAILMSG(1,("lpwavStruct->fdwOpen = %X\r\n",lpwavStruct->fdwOpen));

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 映射指针

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
		DbgOutString("Write Wav Failure\r\n");
 		return MMSYSERR_WRITEERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}

//	RETAILMSG(1,("-waveOutWrite\r\n"));
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：暂停声音播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	DWORD dwError = MMSYSERR_ERROR;
	LPWAVSTRUCT lpwavStruct;

	DbgOutString("+waveOutPause\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_PAUSE;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=0;  // 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveOutPause\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：重新开始声音播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutRestart\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_RESTART;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=0;  // 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveOutRestart\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：重新开始声音播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutReset\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_RESET;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=0;  // 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // 错误
		return dwError;
	}
	DbgOutString("-waveOutReset\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：中断循环播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutBreakLoop\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_BREAKLOOP;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=0; // 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // 消息有错误发生
		return dwError;
	}
	DbgOutString("-waveOutBreakLoop\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPMMTIME pmmt -- 指向存储位置的结构的指针
//	UINT cbmmt -- 位置的结构的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：中断循环播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

//	DbgOutString("+waveOutGetPosition\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_GETPOS; // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pmmt;  // 设置消息
	Params.wParam=cbmmt;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );   // 映射指针

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // 消息有错误发生
		return dwError;
	}
//	DbgOutString("-waveOutGetPosition\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPDWORD pdwPitch -- 指向当前的音调的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前的音调
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetPitch\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_GETPITCH;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pdwPitch;  // 设置参数
	Params.wParam=0;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 映射指针

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // 消息有错误
		return dwError;
	}
	DbgOutString("-waveOutGetPitch\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	DWORD dwPitch -- 要设置的音调
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 设置当前的音调
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutSetPitch\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_SETPITCH;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=dwPitch;  // 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // 消息有错误
		return dwError;
	}
	DbgOutString("-waveOutSetPitch\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPDWORD pdwRate -- 指向存放当前重放速率的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前的重放速率
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutGetPlaybackRate\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo; // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_GETPLAYBACKRATE;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=(DWORD)pdwRate;  // 设置参数
	Params.wParam=0;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); 

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // 消息有错误
		return dwError;
	}
	DbgOutString("-waveOutGetPlaybackRate\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	DWORD dwRate -- 当前要设置的重放速率
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 设置当前的重放速率
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveOutSetPlaybackRate\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{  // 声音没有打开
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_OUT_SETPLAYBACKRATE;  // 设置消息
	Params.dwCallBackParam=0;
	Params.lParam=dwRate;  // 设置参数
	Params.wParam=0;

	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // 消息有错误
		return dwError;
	}
	DbgOutString("-waveOutSetPlaybackRate\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	UINT *puDeviceID -- 指向存放当前播放的设备标识号的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前播放的设备标识号
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
{
	DbgOutString("+waveOutGetID\r\n");
	DbgOutString("-waveOutGetID\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	UINT uMsg  -- 要发送的消息
//	DWORD dw1  -- 参数
//	DWORD dw2  -- 参数
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 发送一个自定义的消息到设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;
	HANDLE hWav;

	DbgOutString("+waveOutMessage\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到结构指针

	if (lpwavStruct==NULL)
	{ // 声音没有打开
		// 打开声音端口
		hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
		if (hWav==INVALID_HANDLE_VALUE)
		{  // 打开声音失败
			DbgOutString("Can't Open Wav Device\r\n");
			return MMSYSERR_ALLOCATED;
		}
		Params.uDeviceID=0;
	}
	else
	{  // 已经打开声音
		Params.uDeviceID=lpwavStruct->uDeviceID;
		hWav=lpwavStruct->hWav; // 达到声音句柄
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=uMsg;  // 设置消息
	Params.dwCallBackParam=0;  // 设置参数
	Params.wParam=dw1;
	Params.lParam=dw2;

	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
		if (lpwavStruct==NULL)
		{
			CloseHandle(hWav); // 关闭句柄
		}
 		return MMSYSERR_ERROR;
	}
	if (lpwavStruct==NULL)
	{
		CloseHandle(hWav); // 关闭句柄
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // 消息有错误
		return dwError;
	}
	DbgOutString("-waveOutMessage\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//参数：
//	LPHWAVEOUT phwo	--	存放声音输出设备句柄的句柄
//	UINT uDeviceID -- 要打开的设备的标识号
//	PWAVEFORMATEX pwfx -- 要打开的声音的格式
//	DWORD dwCallback -- 回调句柄
//	DWORD dwInstance -- 实例句柄
//	DWORD fdwOpen -- 打开模式

//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 打开一个声音设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	HANDLE hWav;
	AUDIOMDD_MSG  Params;
    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
	DWORD dwError;
//	PROCBACK *lpProcBack;


	DbgOutString("mdd:waveOutOpen\r\n");
	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // 回调窗口
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // 回调线程，不支持
		DbgOutString("the call back thread is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{  // 回调事件，不支持
		DbgOutString("the call back Event is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{// 回调函数，支持
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{// 没有回调
	}
	else
	{  // 没有设置回调，参数错误
		DbgOutString("Invaild Parameter\r\n");
		return MMSYSERR_INVALPARAM;
	}

	//	*phwo=NULL;
//	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	// 打开声音端口
	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{ // 打开声音失败
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // 分配结构

	if (lpwavStruct==NULL)
	{  // 分配失败
		DbgOutString("Memory is Not Enough");
		CloseHandle(hWav);  // 关闭句柄
		return MMSYSERR_NOMEM; // 返回失败
	}
	EdbgOutputDebugString("lpwavStruct in Open %X\r\n",lpwavStruct);

	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // 分配声音格式结构
	if (pnewwfx==NULL)
	{ // 分配失败
		DbgOutString("Memory is Not Enough");
		free(lpwavStruct);  // 释放内存
		CloseHandle(hWav); // 关闭句柄
		return MMSYSERR_NOMEM;  // 返回失败
	}
	*pnewwfx=*pwfx;  // 设置声音格式

//	RETAILMSG(1, (TEXT( "pwfx->wFormatTag=%d\r\n" ),pwfx->wFormatTag) );
//	RETAILMSG(1, (TEXT( "pwfx->nChannels=%d\r\n" ),pwfx->nChannels) );
//	RETAILMSG(1, (TEXT( "pwfx->nSamplesPerSec=%d\r\n" ),pwfx->nSamplesPerSec) );
//	RETAILMSG(1, (TEXT( "pwfx->wBitsPerSample=%d\r\n" ),pwfx->wBitsPerSample) );

//	RETAILMSG(1, (TEXT( "pnewwfx->wFormatTag=%d\r\n" ),pnewwfx->wFormatTag) );
//	RETAILMSG(1, (TEXT( "pnewwfx->nChannels=%d\r\n" ),pnewwfx->nChannels) );
//	RETAILMSG(1, (TEXT( "pnewwfx->nSamplesPerSec=%d\r\n" ),pnewwfx->nSamplesPerSec) );
//	RETAILMSG(1, (TEXT( "pnewwfx->wBitsPerSample=%d\r\n" ),pnewwfx->wBitsPerSample) );

	lpwavStruct->hWav=hWav;  // 设置声音句柄
	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;
	lpwavStruct->dwCallback=dwCallback;  // 设置回调
	lpwavStruct->dwInstance=dwInstance; // 设置实例句柄
	lpwavStruct->fdwOpen=fdwOpen; // 设置打开方式
	lpwavStruct->hCallerProcess = GetCallerProcess();  // 设置呼叫进程



//	WOD.hWave=NULL;
	WOD.lpFormat=pnewwfx;
//	WOD.dwCallback=dwCallback;
	WOD.dwCallback=(DWORD)DrvCallBack;  // 设置驱动回调
	WOD.dwInstance=dwInstance;
//	WOD.uMappedDeviceID=uDeviceID;


	Params.uDeviceID=uDeviceID;
	Params.message=AM_OUT_OPEN;  // 设置消息
	Params.dwCallBackParam=(DWORD)lpwavStruct;  // 设置回调参数
	Params.lParam=(DWORD)&WOD; // 设置消息参数
	Params.wParam=fdwOpen;
	
	WOD.lpFormat = (PWAVEFORMATEX)MapPtrToProcess( (LPVOID)WOD.lpFormat, GetCurrentProcess() ); // 映射指针
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 映射指针
	Params.dwCallBackParam = (DWORD)MapPtrToProcess( (LPVOID)Params.dwCallBackParam, GetCurrentProcess() ); // 映射指针

//	RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );
	DbgOutString("Open Wav Device Success\r\n");
	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
		free(pnewwfx); // 释放内存
		free(lpwavStruct);
		CloseHandle(hWav); // 关闭句柄
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{ // 消息有错误
		free(pnewwfx);  // 释放内存
		free(lpwavStruct);
		CloseHandle(hWav); // 关闭句柄
 		return dwError;
	}
	*phwo=lpwavStruct;  // 返回当前结构作为句柄

	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // 回调窗口
		DbgOutString("will Call back Window\r\n");
		SendMessage((HWND)dwCallback,MM_WOM_OPEN,0,0);  // 发送打开声音的消息给窗口
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // 回调线程
		DbgOutString("will Call back Thread\r\n");
		return MMSYSERR_NOERROR;
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{ // 回调事件
		DbgOutString("will Call back Event\r\n");
		//SetEvent(dwCallback);
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // 回调函数
		DbgOutString("will Call back Function\r\n");

//		lpProcBack=dwCallback;
//		lpProcBack(lpwavStruct,WOM_OPEN,lpwavStruct->dwInstance,0,0);
		CallApFunction(lpwavStruct,WOM_OPEN,0,0);  // 调用回调函数通知已经打开声音

	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{  // 没有回调
		DbgOutString("will Call back NULL\r\n");
	}
	else
	{  // 没有设置回调
		DbgOutString("Vaild Parameter\r\n");
	}
	DbgOutString("-mdd:waveOutOpen\r\n");
	return MMSYSERR_NOERROR;
}


// ********************************************************************
//声明：UINT     WINAPI Audio_waveInGetNumDevs(void)
//参数：
//	无
//返回值：
//	返回当前系统支持的设备个数
//功能描述： 得到当前声音输入的设备数目
//引用: wave API函数接口
// ********************************************************************
UINT     WINAPI Audio_waveInGetNumDevs(void)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	UINT numDevs;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetNumDevs\r\n");

	// 打开声音端口
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // 打开声音失败
		DbgOutString("Can't Open Wav Device\r\n");
		return 0;
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.uDeviceID=0;
	Params.message=AM_IN_GETNUMDEVS;  // 设置消息
	Params.dwCallBackParam=0;  // 设置参数
	Params.lParam=0;
	Params.wParam=0;

	// 发送消息
	numDevs=DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0);
	CloseHandle(hWav);  // 关闭句柄
	DbgOutString("-waveInGetNumDevs\r\n");
	return numDevs;  // 返回设备个数
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
//参数：
//	UINT uDeviceID   --  设备标识号
//	LPWAVEOUTCAPS pwoc  -- 一个指向输入设备能力结构的指针
//  UINT cbwoc  --  结构WAVEOUTCAPS的大小，以BYTE为单位
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到输入设备能力
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetDevCaps\r\n");

	// 打开声音端口
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // 打开端口失败
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_GETDEVCAPS;  // 设置消息
	Params.dwCallBackParam=0;  // 设置参数
	Params.lParam=(DWORD)pwic;
	Params.wParam=cbwic;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // 映射指针
	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
		CloseHandle(hWav);  // 关闭句柄
 		return MMSYSERR_ERROR;
	}
	// 消息成功
	CloseHandle(hWav);  // 关闭句柄
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetDevCaps\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//参数：
//	MMRESULT mmrError -- 要得到文本的错误信息
//	LPTSTR pszText  --  指向存放错误文本缓存的指针
//	UINT cchText  -- 缓存的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到输入错误的文本
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	DbgOutString("+waveInGetErrorText\r\n");
	DbgOutString("-waveInGetErrorText\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：关闭声音输入
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInClose\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // 回调窗口
			DbgOutString("will Call back Window\r\n");
			SendMessage((HWND)lpwavStruct->dwCallback,MM_WIM_CLOSE,0,0);  // 发送关闭消息
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{ // 回调线程
			DbgOutString("will Call back Thread\r\n");
			return MMSYSERR_NOERROR;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // 回调事件
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{  // 回调函数
			DbgOutString("will Call back Function\r\n");

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WIM_CLOSE,lpwavStruct->dwInstance,0,0);
			CallApFunction(lpwavStruct,WIM_CLOSE,0,0);  // 调用函数发送关闭消息
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // 没有回调
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // 无效回调
			DbgOutString("Vaild Parameter\r\n");
		}

		CloseHandle(lpwavStruct->hWav); // 关闭句柄
		if (lpwavStruct->pwfx)  // 释放内存
			free(lpwavStruct->pwfx);
		free(lpwavStruct);  
		DbgOutString("-waveInClose\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEIN hwi  -- 声音输入句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：准备声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInPrepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );   // 映射指针
	pwh->dwFlags |= WHDR_PREPARED; // 设置准备好的标志

	DbgOutString("-waveInPrepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：卸除声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInUnprepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() ); 
	pwh->dwFlags &= ~WHDR_PREPARED;  // 取消准备标志

	DbgOutString("-waveInUnprepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：从设备读取声音数据块
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInAddBuffer\r\n");

		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // 映射指针
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID; 
		Params.message=AM_IN_ADDBUFFER; // 设置消息
		Params.dwCallBackParam=0;  // 设置参数
		Params.lParam=(DWORD)pwh;
		Params.wParam=cbwh;

		Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // 映射指针

		// 发送消息
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{ // 消息设备
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInAddBuffer\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：开始声音录音
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStart\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{ // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_START;  // 设置消息
		Params.dwCallBackParam=0; // 设置参数
		Params.lParam=0;
		Params.wParam=0;
 
		// 发送消息
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 消息失败
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStart\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：停止声音录音
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStop\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_STOP;  // 设置消息
		Params.dwCallBackParam=0;  // 设置参数
		Params.lParam=0;
		Params.wParam=0;

		// 发送消息
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 消息失败
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStop\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：重设声音输入设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInReset\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_RESET;  // 设置消息
		Params.dwCallBackParam=0; // 设置参数
		Params.lParam=0;
		Params.wParam=0;

		// 发送消息
		if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 消息失败
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInReset\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	LPMMTIME pmmt -- 指向存储位置的结构的指针
//	UINT cbmmt -- 位置的结构的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到输入数据的长度
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetPosition\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwi;

	if (lpwavStruct==NULL)
	{  // 没有打开声音
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_IN_GETPOS;  // 设置消息
	Params.dwCallBackParam=0; // 设置参数
	Params.lParam=(DWORD)pmmt;
	Params.wParam=cbmmt;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 影射指针
	// 发送消息
	if (DeviceIoControl(lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetPosition\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	UINT *puDeviceID -- 指向存放当前播放的设备标识号的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前录音的设备标识号
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
{
	DbgOutString("+waveInGetID\r\n");
	DbgOutString("-waveInGetID\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	UINT uMsg  -- 要发送的消息
//	DWORD dw1  -- 参数
//	DWORD dw2  -- 参数
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 发送一个自定义的消息到设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
	DbgOutString("+waveInMessage\r\n");
	DbgOutString("-waveInMessage\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//参数：
//	LPHWAVEIN phwi	--	存放声音输入设备句柄的句柄
//	UINT uDeviceID -- 要打开的设备的标识号
//	PWAVEFORMATEX pwfx -- 要开始录音的声音的格式
//	DWORD dwCallback -- 回调句柄
//	DWORD dwInstance -- 实例句柄
//	DWORD fdwOpen -- 打开模式

//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 打开一个输入声音设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	HANDLE hWav;
	AUDIOMDD_MSG  Params;
    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;


	DbgOutString("+waveInOpen\r\n");
	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // 回调窗口
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // 回调线程
		DbgOutString("the call back thread is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;  // 目前不支持
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{ // 回调事件
		DbgOutString("the call back Event is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED; // 目前不支持
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // 回调函数
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{ // 没有回调
	}
	else
	{ // 非法回调
		DbgOutString("Invaild Parameter\r\n");
		return MMSYSERR_INVALPARAM;
	}
//	*phwo=NULL;
//	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	// 打开声音端口
	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{// 打开端口失败
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // 分配内存

	if (lpwavStruct==NULL)
	{  // 分配失败
		DbgOutString("Memory is Not Enough");
		CloseHandle(hWav); // 关闭句柄
		return MMSYSERR_NOMEM;
	}
	EdbgOutputDebugString("lpwavStruct in Open %X\r\n",lpwavStruct);
	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // 分配内存
	if (pnewwfx==NULL)
	{// 分配失败
		DbgOutString("Memory is Not Enough");
		free(lpwavStruct);  // 释放已经分配的内存
		CloseHandle(hWav); // 关闭句柄
		return MMSYSERR_NOMEM;
	}
	*pnewwfx=*pwfx;  // 设置当前的声音模式

	lpwavStruct->hWav=hWav; // 设置声音句柄
	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;  // 设置声音模式
	lpwavStruct->dwCallback=dwCallback; // 设置回调
	lpwavStruct->dwInstance=dwInstance; // 设置实例句柄
	lpwavStruct->fdwOpen=fdwOpen; // 设置打开模式


//	WOD.hWave=NULL;
	WOD.lpFormat=pnewwfx;  // 设置结构
//	WOD.dwCallback=dwCallback;
	WOD.dwCallback=(DWORD)DrvCallBack;  // 设置驱动回调
	WOD.dwInstance=dwInstance;
//	WOD.uMappedDeviceID=uDeviceID;


	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_OPEN;  // 设置消息
	Params.dwCallBackParam=(DWORD)lpwavStruct;  // 设置回调参数
	Params.lParam=(DWORD)&WOD;  // 设置打开类型
	Params.wParam=fdwOpen; // 设置打开模式

	WOD.lpFormat = (PWAVEFORMATEX)MapPtrToProcess( (LPVOID)WOD.lpFormat, GetCurrentProcess() );  // 映射指针
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 映射指针
	Params.dwCallBackParam = (DWORD)MapPtrToProcess( (LPVOID)Params.dwCallBackParam, GetCurrentProcess() ); // 映射指针

	DbgOutString("Open Wav Device Success\r\n");
	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
		free(pnewwfx);  // 释放内存
		free(lpwavStruct);
		CloseHandle(hWav); // 关闭句柄
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // 有错误产生
		free(pnewwfx); // 释放内存
		free(lpwavStruct);
		CloseHandle(hWav); // 关闭句柄
		return dwError;
	}
	*phwi=lpwavStruct;  // 返回句柄

	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // 回调窗口
		DbgOutString("will Call back Window\r\n");
		SendMessage((HWND)dwCallback,MM_WIM_OPEN,0,0);  // 发送打开消息
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // 回调线程
		DbgOutString("will Call back Thread\r\n");
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{  // 回调事件
		DbgOutString("will Call back Event\r\n");
		//SetEvent(dwCallback);
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // 回调函数
		DbgOutString("will Call back Function\r\n");
//		lpProcBack=dwCallback;
//		lpProcBack(lpwavStruct,WIM_OPEN,lpwavStruct->dwInstance,0,0);
		CallApFunction(lpwavStruct,WIM_OPEN,0,0);  // 调用回调函数

	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{  // 没有回调
		DbgOutString("will Call back NULL\r\n");
	}
	else
	{  // 无效回调
		DbgOutString("Vaild Parameter\r\n");
	}
	DbgOutString("-waveInOpen\r\n");
	return MMSYSERR_NOERROR;
}

// **************************************************
// 声明：void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2)
// 参数：
// 	IN hdrvr -- 设备句柄
// 	IN uMsg -- 消息
// 	IN dwCallBackParam -- 回调参数
// 	IN dw1 -- 参数1
// 	IN dw2 -- 参数2
// 
// 返回值：无
// 功能描述：驱动程序回调函数，是提供给驱动程序回调使用。
// 引用: 
// **************************************************
void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2)
//void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1)//, DWORD dw2)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;

		//DbgOutString("+CALLBACK DrvCallBack\r\n");
		lpwavStruct=(LPWAVSTRUCT)dwCallBackParam;
//		RETAILMSG(1,("+CALLBACK DrvCallBack\r\n"));
//		RETAILMSG(1,("lpwavStruct in CallBack %X\r\n",lpwavStruct));
//		RETAILMSG(1,("lpwavStruct->hWav = %X\r\n",lpwavStruct->hWav));
//		RETAILMSG(1,("lpwavStruct->uDeviceID = %X\r\n",lpwavStruct->uDeviceID));
//		RETAILMSG(1,("lpwavStruct->pwfx = %X\r\n",lpwavStruct->pwfx));
//		RETAILMSG(1,("lpwavStruct->dwCallback = %X\r\n",lpwavStruct->dwCallback));
//		RETAILMSG(1,("lpwavStruct->dwInstance = %X\r\n",lpwavStruct->dwInstance));
//		RETAILMSG(1,("lpwavStruct->fdwOpen = %X\r\n",lpwavStruct->fdwOpen));
		
		if (lpwavStruct==NULL)  // 没有打开声音
			return;

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // 回调窗口
			DbgOutString("will Call back Window\r\n");
//			PostMessage((HWND)lpwavStruct->dwCallback,uMsg,dw1,dw2);
			PostMessage((HWND)lpwavStruct->dwCallback,uMsg,dw1,0);  // 发送指定消息给窗口
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{ // 回调线程
			DbgOutString("will Call back Thread\r\n");
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{ // 回调事件
			//SetEvent(lpwavStruct->dwCallback);
			DbgOutString("will Call back Event\r\n");
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{  // 回调函数
/*			CALLBACKDATA  CallbackData;
			
				RETAILMSG(1,("will Call back Function\r\n"));
				CallbackData.hProcess = lpwavStruct->hCallerProcess;
				CallbackData.lpfn = lpwavStruct->dwCallback;
				CallbackData.dwArg0 = lpwavStruct;
				
				RETAILMSG(1,("Call back Function ....\r\n"));
				Sys_ImplementCallBack4( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1,dw2);
				RETAILMSG(1,("Call back Function OK\r\n"));
*/
//				CallApFunction(lpwavStruct,uMsg,dw1,dw2);
				CallApFunction(lpwavStruct,uMsg,dw1,dw2);  // 调用回调函数

//				lpProcBack=lpwavStruct->dwCallback;
//				lpProcBack(lpwavStruct,uMsg,lpwavStruct->dwInstance,dw1,dw2);
//				DbgOutString("will Call back Function\r\n");

				return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // 没有回调
			DbgOutString("will Call back NULL\r\n");
			return ;
		}
		else
		{  // 非法回调
			DbgOutString("Vaild Parameter\r\n");
			return ;
		}
		//DbgOutString("-CALLBACK DrvCallBack\r\n");
//		RETAILMSG(1,("-CALLBACK DrvCallBack\r\n"));
		return ;
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
static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
{
	CALLBACKDATA  CallbackData;
	
//		RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );

//		RETAILMSG(1,("will Call back Function\r\n"));
		CallbackData.hProcess = lpwavStruct->hCallerProcess;  // 设置应用程序进程句柄
		CallbackData.lpfn = (FARPROC)lpwavStruct->dwCallback; // 设置回调函数指针
		CallbackData.dwArg0 = (DWORD)lpwavStruct; // 设置声音结构
		
//		RETAILMSG(1,("Call back Function ....\r\n"));
		// 呼叫系统回调函数
//		Sys_ImplementCallBack4( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1);//,dw2);
		Sys_ImplementCallBack( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1,dw2);
//		RETAILMSG(1,("Call back Function OK\r\n"));
}

#else
#include "eWindows.h"
#include "EMMSys.h"
#include <Audio_ep.h>
//#include <waveddsi.h>
//#include <mmddk.h>
#include <eDevice.h>
#include <eobjcall.h>
#include <eapisrv.h>

#include "APICall.h"


// *****************************************************
// 函数声明区
// *****************************************************
void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1, DWORD dw2);
//void CALLBACK DrvCallBack(HANDLE hdrvr, UINT uMsg, DWORD dwCallBackParam, DWORD dw1);//, DWORD dw2);

typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam, DWORD wParam );
//typedef void (CALLBACK PROCBACK)( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance, DWORD lParam);//, DWORD wParam );


// *****************************************************
// 定义区
// *****************************************************
// hwo句柄内容
typedef struct wavStruct{
	DWORD hWav;   // 声音设备句柄
	UINT uDeviceID;  // 设备标识号
	PWAVEFORMATEX pwfx;  // 声音格式
	DWORD dwCallback;  // 回调变量，可以是函数(Function),窗口句柄(HWND),...
	DWORD dwInstance;  // 实例句柄
	DWORD fdwOpen;   //打开方式
	HANDLE hCallerProcess;  // 呼叫窗口实例句柄
}WAVSTRUCT, *LPWAVSTRUCT;

LPWAVSTRUCT g_lpwavStruct = NULL;



void CALLBACK ProcessFunction_API( DWORD dwWindowHWO,   
	  UINT uMsg,      
	  DWORD dwInstance,       
	  DWORD dwParam1, 
	  DWORD dwParam2 );

static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2);

/**********************************************************/

// ********************************************************************
//声明：UINT     WINAPI Audio_waveOutGetNumDevs(void)
//参数：
//	无
//返回值：
//	返回当前系统支持的设备个数
//功能描述：得到设备数目
//引用: wave API函数接口
// ********************************************************************
UINT     WINAPI Audio_waveOutGetNumDevs(void)
{

	UINT numDevs;

		numDevs = Wnd_waveOutGetNumDevs();  // 调用微软API函数
		return numDevs;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
//参数：
//	UINT uDeviceID   --  设备标识号
//	LPWAVEOUTCAPS pwoc  -- 一个指向输出设备能力的指针
//  UINT cbwoc  --  结构WAVEOUTCAPS的大小，以BYTE为单位
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到设备能力
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
{
	MMRESULT  result = MMSYSERR_ERROR;
		
		result = Wnd_waveOutGetDevCaps(uDeviceID,pwoc,cbwoc);
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPDWORD pdwVolume  -- 指向存放音量的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到当前的音量大小
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

	if (lpwavStruct==NULL)
	{  // 没有打开声音
		hWav = 0;
	}
	else
	{  // 已经打开声音
		hWav = lpwavStruct->hWav;
	}

	result = Wnd_waveOutGetVolume(hWav,pdwVolume);

	return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	DWORD dwVolume  -- 要设置音量大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：设置当前的音量大小
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
	lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

	if (lpwavStruct==NULL)
	{  // 没有打开声音
		hWav = 0;
	}
	else
	{  // 已经打开声音
		hWav = lpwavStruct->hWav;
	}

	result = Wnd_waveOutSetVolume(hWav, dwVolume);
	return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//参数：
//	MMRESULT mmrError -- 要得到文本的错误信息
//	LPTSTR pszText  --  指向存放错误文本缓存的指针
//	UINT cchText  -- 缓存的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：设置当前的音量大小
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	wnd_waveOuGetErrorText(mmrError,pszText,cchText);
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：关闭声音输出
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutClose(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutClose(hWav);

		if (lpwavStruct->pwfx) // 释放空间
			free(lpwavStruct->pwfx);
		free(lpwavStruct);
		g_lpwavStruct = NULL;

		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：准备声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // 映射参数
//		pwh->dwFlags |= WHDR_PREPARED;    // 设置已经准备好的标志

		result = Wnd_waveOutPrepareHeader(hWav,pwh,cbwh);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：卸除声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
//		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // 映射参数
//		pwh->dwFlags |= WHDR_PREPARED;    // 设置已经准备好的标志

		result = Wnd_waveOutUnprepareHeader(hWav,pwh,cbwh);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：写声音数据块到设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutWrite(hWav,pwh,cbwh);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：暂停声音播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutPause(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutPause(hWav);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：重新开始声音播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutRestart(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutRestart(hWav);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：重新开始声音播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutReset(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutReset(hWav);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：中断循环播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutBreakLoop(HWAVEOUT hwo)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutBreakLoop(hWav);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPMMTIME pmmt -- 指向存储位置的结构的指针
//	UINT cbmmt -- 位置的结构的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：中断循环播放
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetPosition(hWav, pmmt, cbmmt);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPDWORD pdwPitch -- 指向当前的音调的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前的音调
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetPitch(hWav, pdwPitch);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	DWORD dwPitch -- 要设置的音调
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 设置当前的音调
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutSetPitch(hWav, dwPitch);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	LPDWORD pdwRate -- 指向存放当前重放速率的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前的重放速率
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetPlaybackRate(hWav, pdwRate);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	DWORD dwRate -- 当前要设置的重放速率
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 设置当前的重放速率
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutSetPlaybackRate(hWav, dwRate);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	UINT *puDeviceID -- 指向存放当前播放的设备标识号的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前播放的设备标识号
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutGetID(HWAVEOUT hwo, UINT *puDeviceID)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			RETAILMSG(1,(TEXT("The Wav had not Open !!!!!!\r\n")));
			return result;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutGetID(hWav, puDeviceID);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
//参数：
//	HWAVEOUT hwo  -- 声音输出设备句柄
//	UINT uMsg  -- 要发送的消息
//	DWORD dw1  -- 参数
//	DWORD dw2  -- 参数
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 发送一个自定义的消息到设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
	LPWAVSTRUCT lpwavStruct;
	DWORD hWav;
	MMRESULT  result = MMSYSERR_ERROR;

	
		lpwavStruct=(LPWAVSTRUCT)hwo;  // 得到声音结构

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			hWav = 0;
		}
		else
		{  // 已经打开声音
			hWav = lpwavStruct->hWav;
		}
		
		result = Wnd_waveOutMessage(hWav, uMsg, dw1, dw2);
		return result ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//参数：
//	LPHWAVEOUT phwo	--	存放声音输出设备句柄的句柄
//	UINT uDeviceID -- 要打开的设备的标识号
//	PWAVEFORMATEX pwfx -- 要打开的声音的格式
//	DWORD dwCallback -- 回调句柄
//	DWORD dwInstance -- 实例句柄
//	DWORD fdwOpen -- 打开模式

//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 打开一个声音设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
//	HANDLE hWav;
//	AUDIOMDD_MSG  Params;
//    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
//	DWORD dwError;

	MMRESULT result = MMSYSERR_ERROR;
//	PROCBACK *lpProcBack;
	
	RETAILMSG(1,(TEXT("API:waveOutOpen")));

	if (g_lpwavStruct)
	{
		RETAILMSG(1,(TEXT("The Audio Had Open \r\n")));
		return result;
	}

	//	*phwo=NULL;
	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // 分配结构

	if (lpwavStruct==NULL)
	{  // 分配失败
		DbgOutString("Memory is Not Enough");
		return MMSYSERR_NOMEM; // 返回失败
	}

	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // 分配声音格式结构
	if (pnewwfx==NULL)
	{ // 分配失败
		free(lpwavStruct);  // 释放内存
		return MMSYSERR_NOMEM;  // 返回失败
	}
	*pnewwfx=*pwfx;  // 设置声音格式

	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;
	lpwavStruct->dwCallback=dwCallback;  // 设置回调
	lpwavStruct->dwInstance=dwInstance; // 设置实例句柄
	lpwavStruct->fdwOpen=fdwOpen; // 设置打开方式
	lpwavStruct->hCallerProcess = GetCallerProcess();  // 设置呼叫进程

	g_lpwavStruct = lpwavStruct;

	result = Wnd_waveOutOpen(
			  &lpwavStruct->hWav,            
			  WAVE_MAPPER ,            
			  (LPWAVEFORMATEX)pwfx,       
			  (DWORD)ProcessFunction_API,          
			  0,  
			  CALLBACK_FUNCTION);



	*phwo=lpwavStruct;  // 返回当前结构作为句柄


	return result;
}


// ********************************************************************
//声明：UINT     WINAPI Audio_waveInGetNumDevs(void)
//参数：
//	无
//返回值：
//	返回当前系统支持的设备个数
//功能描述： 得到当前声音输入的设备数目
//引用: wave API函数接口
// ********************************************************************
UINT     WINAPI Audio_waveInGetNumDevs(void)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	UINT numDevs;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetNumDevs\r\n");

	// 打开声音端口
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // 打开声音失败
		DbgOutString("Can't Open Wav Device\r\n");
		return 0;
	}

//	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.uDeviceID=0;
	Params.message=AM_IN_GETNUMDEVS;  // 设置消息
	Params.dwCallBackParam=0;  // 设置参数
	Params.lParam=0;
	Params.wParam=0;

	// 发送消息
	numDevs=DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0);
	CloseHandle(hWav);  // 关闭句柄
	DbgOutString("-waveInGetNumDevs\r\n");
	return numDevs;  // 返回设备个数
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
//参数：
//	UINT uDeviceID   --  设备标识号
//	LPWAVEOUTCAPS pwoc  -- 一个指向输入设备能力结构的指针
//  UINT cbwoc  --  结构WAVEOUTCAPS的大小，以BYTE为单位
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到输入设备能力
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
{
	AUDIOMDD_MSG  Params;
//	LPWAVSTRUCT lpwavStruct;
	HANDLE hWav;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetDevCaps\r\n");

	// 打开声音端口
	hWav=CreateFile("AUD1:", 0, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{  // 打开端口失败
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_GETDEVCAPS;  // 设置消息
	Params.dwCallBackParam=0;  // 设置参数
	Params.lParam=(DWORD)pwic;
	Params.wParam=cbwic;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // 映射指针
	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{  // 消息失败
		CloseHandle(hWav);  // 关闭句柄
 		return MMSYSERR_ERROR;
	}
	// 消息成功
	CloseHandle(hWav);  // 关闭句柄
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetDevCaps\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
//参数：
//	MMRESULT mmrError -- 要得到文本的错误信息
//	LPTSTR pszText  --  指向存放错误文本缓存的指针
//	UINT cchText  -- 缓存的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到输入错误的文本
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
	DbgOutString("+waveInGetErrorText\r\n");
	DbgOutString("-waveInGetErrorText\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：关闭声音输入
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInClose(HWAVEIN hwi)
{
	LPWAVSTRUCT lpwavStruct;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInClose\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}

		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // 回调窗口
			DbgOutString("will Call back Window\r\n");
			SendMessage((HWND)lpwavStruct->dwCallback,MM_WIM_CLOSE,0,0);  // 发送关闭消息
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{ // 回调线程
			DbgOutString("will Call back Thread\r\n");
			return MMSYSERR_NOERROR;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // 回调事件
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{  // 回调函数
			DbgOutString("will Call back Function\r\n");

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WIM_CLOSE,lpwavStruct->dwInstance,0,0);
			CallApFunction(lpwavStruct,WIM_CLOSE,0,0);  // 调用函数发送关闭消息
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // 没有回调
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // 无效回调
			DbgOutString("Vaild Parameter\r\n");
		}

		CloseHandle((HANDLE)lpwavStruct->hWav); // 关闭句柄
		if (lpwavStruct->pwfx)  // 释放内存
			free(lpwavStruct->pwfx);
		free(lpwavStruct);  
		DbgOutString("-waveInClose\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEIN hwi  -- 声音输入句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：准备声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInPrepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );   // 映射指针
	pwh->dwFlags |= WHDR_PREPARED; // 设置准备好的标志

	DbgOutString("-waveInPrepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：卸除声音数据块的头
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	DbgOutString("+waveInUnprepareHeader\r\n");

	pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() ); 
	pwh->dwFlags &= ~WHDR_PREPARED;  // 取消准备标志

	DbgOutString("-waveInUnprepareHeader\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	LPWAVEHDR pwh -- 指向一个声音数据块的指针
//	UINT cbwh -- 声音数据块的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：从设备读取声音数据块
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInAddBuffer\r\n");

		pwh->lpData = (LPBYTE)MapPtrToProcess( (LPVOID)pwh->lpData, GetCallerProcess() );  // 映射指针
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID; 
		Params.message=AM_IN_ADDBUFFER; // 设置消息
		Params.dwCallBackParam=0;  // 设置参数
		Params.lParam=(DWORD)pwh;
		Params.wParam=cbwh;

		Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() );  // 映射指针

		// 发送消息
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{ // 消息设备
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInAddBuffer\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：开始声音录音
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInStart(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStart\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{ // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_START;  // 设置消息
		Params.dwCallBackParam=0; // 设置参数
		Params.lParam=0;
		Params.wParam=0;
 
		// 发送消息
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 消息失败
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStart\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：停止声音录音
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInStop(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInStop\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 没有打开声音
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_STOP;  // 设置消息
		Params.dwCallBackParam=0;  // 设置参数
		Params.lParam=0;
		Params.wParam=0;

		// 发送消息
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 消息失败
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInStop\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：重设声音输入设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInReset(HWAVEIN hwi)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

		DbgOutString("+waveInReset\r\n");
		lpwavStruct=(LPWAVSTRUCT)hwi;

		if (lpwavStruct==NULL)
		{  // 声音没有打开
			return MMSYSERR_INVALHANDLE;
		}

		Params.uDeviceID=lpwavStruct->uDeviceID;
		Params.message=AM_IN_RESET;  // 设置消息
		Params.dwCallBackParam=0; // 设置参数
		Params.lParam=0;
		Params.wParam=0;

		// 发送消息
		if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
		{  // 消息失败
 			return MMSYSERR_ERROR;
		}
		if (dwError != MMSYSERR_NOERROR)
		{
			return dwError;
		}
		DbgOutString("-waveInReset\r\n");
		return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	LPMMTIME pmmt -- 指向存储位置的结构的指针
//	UINT cbmmt -- 位置的结构的大小
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述：得到输入数据的长度
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
	AUDIOMDD_MSG  Params;
	LPWAVSTRUCT lpwavStruct;
	DWORD dwError = MMSYSERR_ERROR;

	DbgOutString("+waveInGetPosition\r\n");

	lpwavStruct=(LPWAVSTRUCT)hwi;

	if (lpwavStruct==NULL)
	{  // 没有打开声音
		return MMSYSERR_INVALHANDLE;
	}

	Params.uDeviceID=lpwavStruct->uDeviceID;
	Params.message=AM_IN_GETPOS;  // 设置消息
	Params.dwCallBackParam=0; // 设置参数
	Params.lParam=(DWORD)pmmt;
	Params.wParam=cbmmt;

	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 影射指针
	// 发送消息
	if (DeviceIoControl((HANDLE)lpwavStruct->hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{
		return dwError;
	}
	DbgOutString("-waveInGetPosition\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	UINT *puDeviceID -- 指向存放当前播放的设备标识号的指针
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 得到当前录音的设备标识号
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
{
	DbgOutString("+waveInGetID\r\n");
	DbgOutString("-waveInGetID\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
//参数：
//	HWAVEIN hwi  -- 声音输入设备句柄
//	UINT uMsg  -- 要发送的消息
//	DWORD dw1  -- 参数
//	DWORD dw2  -- 参数
//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 发送一个自定义的消息到设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
	DbgOutString("+waveInMessage\r\n");
	DbgOutString("-waveInMessage\r\n");
	return MMSYSERR_NOERROR ;
}
// ********************************************************************
//声明：MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
//参数：
//	LPHWAVEIN phwi	--	存放声音输入设备句柄的句柄
//	UINT uDeviceID -- 要打开的设备的标识号
//	PWAVEFORMATEX pwfx -- 要开始录音的声音的格式
//	DWORD dwCallback -- 回调句柄
//	DWORD dwInstance -- 实例句柄
//	DWORD fdwOpen -- 打开模式

//返回值：
//	成功返回MMSYSERR_NOERROR,否则失败
//功能描述： 打开一个输入声音设备
//引用: wave API函数接口
// ********************************************************************
MMRESULT WINAPI Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	HANDLE hWav;
	AUDIOMDD_MSG  Params;
    AUDIOMDD_OPEN WOD;
	LPWAVSTRUCT lpwavStruct;
	PWAVEFORMATEX pnewwfx;
//	PROCBACK *lpProcBack;
	DWORD dwError = MMSYSERR_ERROR;


	DbgOutString("+waveInOpen\r\n");
	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // 回调窗口
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // 回调线程
		DbgOutString("the call back thread is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED;  // 目前不支持
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{ // 回调事件
		DbgOutString("the call back Event is unsupport\r\n");
		return MMSYSERR_NOTSUPPORTED; // 目前不支持
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // 回调函数
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{ // 没有回调
	}
	else
	{ // 非法回调
		DbgOutString("Invaild Parameter\r\n");
		return MMSYSERR_INVALPARAM;
	}
//	*phwo=NULL;
//	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	// 打开声音端口
	hWav=CreateFile("AUD1:", GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, NULL); 
	if (hWav==INVALID_HANDLE_VALUE)
	{// 打开端口失败
		DbgOutString("Can't Open Wav Device\r\n");
		return MMSYSERR_ALLOCATED;
	}

	lpwavStruct=(LPWAVSTRUCT)malloc(sizeof(WAVSTRUCT));  // 分配内存

	if (lpwavStruct==NULL)
	{  // 分配失败
		DbgOutString("Memory is Not Enough");
		CloseHandle(hWav); // 关闭句柄
		return MMSYSERR_NOMEM;
	}
	EdbgOutputDebugString("lpwavStruct in Open %X\r\n",lpwavStruct);
	pnewwfx=(PWAVEFORMATEX)malloc(sizeof(WAVEFORMATEX));  // 分配内存
	if (pnewwfx==NULL)
	{// 分配失败
		DbgOutString("Memory is Not Enough");
		free(lpwavStruct);  // 释放已经分配的内存
		CloseHandle(hWav); // 关闭句柄
		return MMSYSERR_NOMEM;
	}
	*pnewwfx=*pwfx;  // 设置当前的声音模式

	lpwavStruct->hWav=(DWORD)hWav; // 设置声音句柄
	lpwavStruct->uDeviceID=uDeviceID;
	lpwavStruct->pwfx=pnewwfx;  // 设置声音模式
	lpwavStruct->dwCallback=dwCallback; // 设置回调
	lpwavStruct->dwInstance=dwInstance; // 设置实例句柄
	lpwavStruct->fdwOpen=fdwOpen; // 设置打开模式


//	WOD.hWave=NULL;
//	WOD.lpFormat=pnewwfx;  // 设置结构
//	WOD.dwCallback=dwCallback;
//	WOD.dwCallback=(DWORD)DrvCallBack;  // 设置驱动回调
//	WOD.dwInstance=dwInstance;
//	WOD.uMappedDeviceID=uDeviceID;


	Params.uDeviceID=uDeviceID;
	Params.message=AM_IN_OPEN;  // 设置消息
	Params.dwCallBackParam=(DWORD)lpwavStruct;  // 设置回调参数
	Params.lParam=(DWORD)&WOD;  // 设置打开类型
	Params.wParam=fdwOpen; // 设置打开模式

	WOD.lpFormat = (PWAVEFORMATEX)MapPtrToProcess( (LPVOID)WOD.lpFormat, GetCurrentProcess() );  // 映射指针
	Params.lParam = (DWORD)MapPtrToProcess( (LPVOID)Params.lParam, GetCurrentProcess() ); // 映射指针
	Params.dwCallBackParam = (DWORD)MapPtrToProcess( (LPVOID)Params.dwCallBackParam, GetCurrentProcess() ); // 映射指针

	DbgOutString("Open Wav Device Success\r\n");
	// 发送消息
	if (DeviceIoControl(hWav,IOCTL_AUDIO_MESSAGE,&Params,0,&dwError,sizeof(DWORD),0,0)==FALSE)
	{ // 消息失败
		free(pnewwfx);  // 释放内存
		free(lpwavStruct);
		CloseHandle(hWav); // 关闭句柄
 		return MMSYSERR_ERROR;
	}
	if (dwError != MMSYSERR_NOERROR)
	{  // 有错误产生
		free(pnewwfx); // 释放内存
		free(lpwavStruct);
		CloseHandle(hWav); // 关闭句柄
		return dwError;
	}
	*phwi=lpwavStruct;  // 返回句柄

	if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
	{  // 回调窗口
		DbgOutString("will Call back Window\r\n");
		SendMessage((HWND)dwCallback,MM_WIM_OPEN,0,0);  // 发送打开消息
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
	{ // 回调线程
		DbgOutString("will Call back Thread\r\n");
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
	{  // 回调事件
		DbgOutString("will Call back Event\r\n");
		//SetEvent(dwCallback);
	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
	{ // 回调函数
		DbgOutString("will Call back Function\r\n");
//		lpProcBack=dwCallback;
//		lpProcBack(lpwavStruct,WIM_OPEN,lpwavStruct->dwInstance,0,0);
		CallApFunction(lpwavStruct,WIM_OPEN,0,0);  // 调用回调函数

	}
	else if ((fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
	{  // 没有回调
		DbgOutString("will Call back NULL\r\n");
	}
	else
	{  // 无效回调
		DbgOutString("Vaild Parameter\r\n");
	}
	DbgOutString("-waveInOpen\r\n");
	return MMSYSERR_NOERROR;
}

// **************************************************
// 声明：void CALLBACK ProcessFunction( HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,       
//	  DWORD dwParam1,  DWORD dwParam2 )
// 参数：
// 	IN hwi -- 声音句柄
// 	IN uMsg -- 声音消息
// 	IN dwInstance -- 实例句柄
// 	IN dwParam1 -- 参数1
// 	IN dwParam2 -- 参数2
// 
// 返回值：无
// 功能描述：声音消息处理函数。
// 引用: 
// **************************************************
void CALLBACK ProcessFunction_API( DWORD dwWindowHWO,   
	  UINT uMsg,      
	  DWORD dwInstance,       
	  DWORD dwParam1, 
	  DWORD dwParam2 )
{
//	CALLBACKDATA  CallbackData;
	LPWAVSTRUCT lpwavStruct;
	
//		RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );
		
		if (g_lpwavStruct == NULL)
			return ;

		lpwavStruct = g_lpwavStruct;
		if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_WINDOW)
		{  // 要求返回窗口
			SendMessage((HWND)lpwavStruct->dwCallback,uMsg,dwParam1,dwParam2);  // 发送关闭声音的消息给窗口
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_THREAD)
		{  // 要求回调线程
			DbgOutString("will Call back Thread\r\n");
			return ;
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_EVENT)
		{  // 要求回调事件
			DbgOutString("will Call back Event\r\n");
			//SetEvent(dwCallback);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_FUNCTION)
		{ // 要求回调函数
			RETAILMSG(1,(TEXT("will Call back Function [%x]\r\n"),uMsg));
			
			CallApFunction(lpwavStruct,uMsg,dwParam1,dwParam2);  // 回调函数通知要关闭声音

//			lpProcBack=lpwavStruct->dwCallback;
//			lpProcBack(lpwavStruct,WOM_CLOSE,lpwavStruct->dwInstance,0,0);
		}
		else if ((lpwavStruct->fdwOpen&CALLBACK_TYPEMASK)==CALLBACK_NULL)
		{  // 没有要求回调
			DbgOutString("will Call back NULL\r\n");
		}
		else
		{  // 没有设置回调
			DbgOutString("Vaild Parameter\r\n");
		}
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
static void CallApFunction(LPWAVSTRUCT lpwavStruct,UINT uMsg,DWORD dw1,DWORD dw2)
{
	CALLBACKDATA  CallbackData;
	
//		RETAILMSG(1, (TEXT( " ****** lpwavStruct->hCallerProcess = %X\r\n" ),lpwavStruct->hCallerProcess) );

//		RETAILMSG(1,("will Call back Function\r\n"));
		CallbackData.hProcess = lpwavStruct->hCallerProcess;  // 设置应用程序进程句柄
		CallbackData.lpfn = (FARPROC)lpwavStruct->dwCallback; // 设置回调函数指针
		CallbackData.dwArg0 = (DWORD)lpwavStruct; // 设置声音结构
		
		RETAILMSG(1,("Call back Function Parameter[%x][%x][%x][%x][%x]....\r\n",lpwavStruct,uMsg,lpwavStruct->dwInstance,dw1,dw2));
		// 呼叫系统回调函数
		Sys_ImplementCallBack( &CallbackData, uMsg,lpwavStruct->dwInstance,dw1,dw2);
//		RETAILMSG(1,("Call back Function OK\r\n"));
}


#endif