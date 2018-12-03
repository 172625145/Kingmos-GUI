#include "windows.h"
#include "mmsystem.h"
#include "APICall.h"



UINT Wnd_waveOutGetNumDevs(void)
{
	return waveOutGetNumDevs();
}


MMRESULT Wnd_waveOutOpen(LPDWORD phwo, UINT uDeviceID,
PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	MMRESULT result;
	HWAVEOUT hwo;

	WAVEFORMATEX waveFormatEx;

//		RETAILMSG(1,("!!!!! Set bPlaying = TRUE\r\n"));
		// 设置声音格式
		waveFormatEx.wFormatTag=WAVE_FORMAT_PCM; 
		waveFormatEx.nChannels=pwfx->nChannels; // 设置声音通道（单声道，立体声）
		waveFormatEx.nSamplesPerSec=pwfx->nSamplesPerSec; // 设置声音频率 （8000，11250，22500，44100）
		waveFormatEx.wBitsPerSample=pwfx->wBitsPerSample; // 数据位（8 BIT ，16 BIT）
		waveFormatEx.nAvgBytesPerSec = waveFormatEx.nSamplesPerSec * waveFormatEx.nChannels * waveFormatEx.wBitsPerSample/8 ; // 得到每秒钟的数据量
		waveFormatEx.cbSize = 0;
		waveFormatEx.nBlockAlign = waveFormatEx.nChannels * waveFormatEx.wBitsPerSample/8 ;

		result = waveOutOpen(&hwo, WAVE_MAPPER,&waveFormatEx, (DWORD)dwCallback, 0, CALLBACK_FUNCTION);
		*phwo = (DWORD)hwo;

		return result;
}


MMRESULT Wnd_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
{
//	return waveOutGetDevCaps(uDeviceID, pwoc, cbwoc);
	return 0;
}

MMRESULT Wnd_waveOutGetVolume(DWORD hWav, LPDWORD pdwVolume)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutGetVolume(hwo, pdwVolume);
}

MMRESULT Wnd_waveOutSetVolume(DWORD hWav, DWORD dwVolume)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutSetVolume(hwo, dwVolume);
}

MMRESULT wnd_waveOuGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
//	return waveOutGetErrorText(mmrError, pszText, cchText);
	return 0;
}
MMRESULT Wnd_waveOutClose(DWORD hWav)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutClose(hwo);
}
MMRESULT Wnd_waveOutPrepareHeader(DWORD hWav, LPWAVEHDR pwh, UINT cbwh)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutPrepareHeader(hwo, pwh, cbwh);
}

MMRESULT Wnd_waveOutUnprepareHeader(DWORD hWav, LPWAVEHDR pwh, UINT cbwh)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutUnprepareHeader(hwo, pwh, cbwh);
}

MMRESULT Wnd_waveOutWrite(DWORD hWav, LPWAVEHDR pwh, UINT cbwh)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutWrite(hwo, pwh, cbwh);
}

MMRESULT Wnd_waveOutPause(DWORD hWav)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutPause(hwo);
}

MMRESULT Wnd_waveOutRestart(DWORD hWav)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutRestart(hwo);
}

MMRESULT Wnd_waveOutReset(DWORD hWav)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutReset(hwo);
}

MMRESULT Wnd_waveOutBreakLoop(DWORD hWav)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutBreakLoop(hwo);
}

MMRESULT Wnd_waveOutGetPosition(DWORD hWav, LPMMTIME pmmt, UINT cbmmt)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutGetPosition(hwo, pmmt, cbmmt);
}

MMRESULT Wnd_waveOutGetPitch(DWORD hWav, LPDWORD pdwPitch)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutGetPitch(hwo, pdwPitch);
}

MMRESULT Wnd_waveOutSetPitch(DWORD hWav, DWORD dwPitch)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutSetPitch(hwo, dwPitch);
}

MMRESULT Wnd_waveOutGetPlaybackRate(DWORD hWav, LPDWORD pdwRate)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutGetPlaybackRate(hwo, pdwRate);
}

MMRESULT Wnd_waveOutSetPlaybackRate(DWORD hWav, DWORD dwRate)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutSetPlaybackRate(hwo, dwRate);
}

MMRESULT Wnd_waveOutGetID(DWORD hWav, UINT *puDeviceID)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutGetID(hwo, puDeviceID);
}

MMRESULT Wnd_waveOutMessage(DWORD hWav, UINT uMsg, DWORD dw1, DWORD dw2)
{
	HWAVEOUT hwo = (HWAVEOUT)hWav;
	return waveOutMessage(hwo, uMsg, dw1, dw2);
}


/*
UINT     Audio_waveInGetNumDevs(void)
{
	return waveInGetNumDevs();
}
MMRESULT Audio_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
{
//	return waveInGetDevCaps(uDeviceID, pwic, cbwic);
	return 0;
}
MMRESULT Audio_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
//	return waveInGetErrorText(mmrError, pszText, cchText);
	return 0;
}
MMRESULT Audio_waveInClose(HWAVEIN hwi)
{
		return waveInClose(hwi) ;
}
MMRESULT Audio_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	return waveInPrepareHeader(hwi, pwh, cbwh);
}
MMRESULT Audio_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
	return waveInUnprepareHeader(hwi, pwh, cbwh);
}
MMRESULT Audio_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
		return waveInAddBuffer(hwi, pwh, cbwh);
}
MMRESULT Audio_waveInStart(HWAVEIN hwi)
{
		return waveInStart(hwi);
}
MMRESULT Audio_waveInStop(HWAVEIN hwi)
{
		return waveInStop(hwi);
}
MMRESULT Audio_waveInReset(HWAVEIN hwi)
{
		return waveInReset(hwi);
}
MMRESULT Audio_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
	return waveInGetPosition(hwi, pmmt, cbmmt);
}
MMRESULT Audio_waveInGetID(HWAVEIN hwi, UINT *puDeviceID)
{
	return waveInGetID(hwi, puDeviceID);
}
MMRESULT Audio_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
	return waveInMessage(hwi, uMsg, dw1, dw2) ;
}
MMRESULT Audio_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	return Audio_waveInOpen(phwi, uDeviceID,pwfx, dwCallback, dwInstance, fdwOpen);
}
*/

