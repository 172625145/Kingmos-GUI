

UINT Wnd_waveOutGetNumDevs(void);

MMRESULT Wnd_waveOutOpen(LPDWORD phwo, UINT uDeviceID,
PWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);

MMRESULT Wnd_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);

MMRESULT Wnd_waveOutGetVolume(DWORD hWav, LPDWORD pdwVolume);

MMRESULT Wnd_waveOutSetVolume(DWORD hWav, DWORD dwVolume);

MMRESULT wnd_waveOuGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText);

MMRESULT Wnd_waveOutClose(DWORD hWav);

MMRESULT Wnd_waveOutPrepareHeader(DWORD hWav, LPWAVEHDR pwh, UINT cbwh);

MMRESULT Wnd_waveOutUnprepareHeader(DWORD hWav, LPWAVEHDR pwh, UINT cbwh);

MMRESULT Wnd_waveOutWrite(DWORD hWav, LPWAVEHDR pwh, UINT cbwh);

MMRESULT Wnd_waveOutPause(DWORD hWav);

MMRESULT Wnd_waveOutRestart(DWORD hWav);

MMRESULT Wnd_waveOutReset(DWORD hWav);

MMRESULT Wnd_waveOutBreakLoop(DWORD hWav);

MMRESULT Wnd_waveOutGetPosition(DWORD hWav, LPMMTIME pmmt, UINT cbmmt);

MMRESULT Wnd_waveOutGetPitch(DWORD hWav, LPDWORD pdwPitch);

MMRESULT Wnd_waveOutSetPitch(DWORD hWav, DWORD dwPitch);

MMRESULT Wnd_waveOutGetPlaybackRate(DWORD hWav, LPDWORD pdwRate);

MMRESULT Wnd_waveOutSetPlaybackRate(DWORD hWav, DWORD dwRate);

MMRESULT Wnd_waveOutGetID(DWORD hWav, UINT *puDeviceID);

MMRESULT Wnd_waveOutMessage(DWORD hWav, UINT uMsg, DWORD dw1, DWORD dw2);


