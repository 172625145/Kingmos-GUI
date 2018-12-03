/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/
#include <eframe.h>
#include <eapisrv.h>
//#include <ewavedev.h>
//#include <ewaveapi.h>
#include <emmsys.h>

#include <epcore.h>


enum{
    AUDIO_WAVEOUTGETNUMDEVS = 1,
    AUDIO_WAVEOUTGETDEVCAPS,
    AUDIO_WAVEOUTGETVOLUME,
    AUDIO_WAVEOUTSETVOLUME,
    AUDIO_WAVEOUTGETERRORTEXT,
    AUDIO_WAVEOUTCLOSE,
    AUDIO_WAVEOUTPREPAREHEADER,
    AUDIO_WAVEOUTUNPREPAREHEADER,
    AUDIO_WAVEOUTWRITE,
    AUDIO_WAVEOUTPAUSE,
    AUDIO_WAVEOUTRESTART,
    AUDIO_WAVEOUTRESET,
    AUDIO_WAVEOUTBREAKLOOP,
    AUDIO_WAVEOUTGETPOSITION,
    AUDIO_WAVEOUTGETPITCH,
    AUDIO_WAVEOUTSETPITCH,
    AUDIO_WAVEOUTGETPLAYBACKRATE,
    AUDIO_WAVEOUTSETPLAYBACKRATE,
    AUDIO_WAVEOUTGETID,
    AUDIO_WAVEOUTMESSAGE,
    AUDIO_WAVEOUTOPEN,
	AUDIO_WAVEINGETNUMDEVS,
    AUDIO_WAVEINGETDEVCAPS,
    AUDIO_WAVEINGETERRORTEXT,
    AUDIO_WAVEINCLOSE,
    AUDIO_WAVEINPREPAREHEADER,
    AUDIO_WAVEINUNPREPAREHEADER,
    AUDIO_WAVEINADDBUFFER,
    AUDIO_WAVEINSTART,
    AUDIO_WAVEINSTOP,
    AUDIO_WAVEINRESET,
    AUDIO_WAVEINGETPOSITION,
    AUDIO_WAVEINGETID,
    AUDIO_WAVEINMESSAGE,
    AUDIO_WAVEINOPEN,

#ifdef MIDI_SUPPORT    
    AUDIO_MIDICONNECT,
    AUDIO_MIDIDISCONNECT,
    AUDIO_MIDIINADDBUFFER,
    AUDIO_MIDIINCLOSE,
    AUDIO_MIDIINGETDEVCAPS,
    AUDIO_MIDIINGETERRORTEXT,
    AUDIO_MIDIINGETID,
    AUDIO_MIDIINGETNUMDEVS,
    AUDIO_MIDIINMESSAGE,
    AUDIO_MIDIINOPEN,
    AUDIO_MIDIINPREPAREHEADER,
    AUDIO_MIDIINRESET,
    AUDIO_MIDIINSTART,
    AUDIO_MIDIINSTOP,
    AUDIO_MIDIINUNPREPAREHEADER,
    AUDIO_MIDIOUTCACHEDRUMPATCHES,
    AUDIO_MIDIOUTCACHEPATCHES,
    AUDIO_MIDIOUTCLOSE,
    AUDIO_MIDIOUTGETDEVCAPS,
    AUDIO_MIDIOUTGETERRORTEXT,
    AUDIO_MIDIOUTGETID,
    AUDIO_MIDIOUTGETNUMDEVS,
    AUDIO_MIDIOUTGETVOLUME,
    AUDIO_MIDIOUTLONGMSG,
    AUDIO_MIDIOUTMESSAGE,
    AUDIO_MIDIOUTOPEN,
    AUDIO_MIDIOUTPREPAREHEADER,
    AUDIO_MIDIOUTRESET,
    AUDIO_MIDIOUTSETVOLUME,
    AUDIO_MIDIOUTSHORTMSG,
    AUDIO_MIDIOUTUNPREPAREHEADER,
    AUDIO_MIDISTREAMCLOSE,
    AUDIO_MIDISTREAMOPEN,
    AUDIO_MIDISTREAMOUT,
    AUDIO_MIDISTREAMPAUSE,
    AUDIO_MIDISTREAMPOSITION,
    AUDIO_MIDISTREAMPROPERTY,
    AUDIO_MIDISTREAMRESTART,
    AUDIO_MIDISTREAMSTOP,
#endif    
};


typedef BOOL ( WINAPI * PWAVEOUTGETNUMDEVS)( void);
UINT     WINAPI aud_waveOutGetNumDevs(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETNUMDEVS, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEOUTGETNUMDEVS pwaveOutGetNumDevs;

	CALLSTACK cs;
	UINT     retv = 0;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETNUMDEVS, &pwaveOutGetNumDevs, &cs ) )
	{
		retv = pwaveOutGetNumDevs();
		API_Leave(  );
	}
	return retv;
#endif
}

typedef MMRESULT (WINAPI * PWAVEOUTGETDEVCAPS)(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
MMRESULT WINAPI aud_waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETDEVCAPS, 3 );
    cs.arg0 = (DWORD)uDeviceID;
    return (DWORD)CALL_SERVER( &cs, pwoc, cbwoc);
#else
	PWAVEOUTGETDEVCAPS pwaveOutGetDevCaps;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETDEVCAPS, &pwaveOutGetDevCaps, &cs ) )
	{
		pwoc = MapProcessPtr( pwoc, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetDevCaps(uDeviceID, pwoc, cbwoc);
		API_Leave(  );
	}
	return retv;
#endif
}

typedef MMRESULT (WINAPI * PWAVEOUTGETVOLUME)(HWAVEOUT hwo, LPDWORD pdwVolume);
MMRESULT WINAPI aud_waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETVOLUME, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pdwVolume);
#else
	PWAVEOUTGETVOLUME pwaveOutGetVolume;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETVOLUME, &pwaveOutGetVolume, &cs ) )
	{
		pdwVolume = MapProcessPtr( pdwVolume, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetVolume(hwo, pdwVolume);
		API_Leave(  );
	}
	return retv;
#endif
}


typedef MMRESULT (WINAPI * PWAVEOUTSETVOLUME)(HWAVEOUT hwo, DWORD dwVolume);
MMRESULT WINAPI aud_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTSETVOLUME, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, dwVolume);
#else
	PWAVEOUTSETVOLUME pwaveOutSetVolume;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTSETVOLUME, &pwaveOutSetVolume, &cs ) )
	{
		retv = pwaveOutSetVolume(hwo, dwVolume);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTGETERRORTEXT)(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
MMRESULT WINAPI aud_waveOutGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETERRORTEXT, 3 );
    cs.arg0 = (DWORD)mmrError;
    return (DWORD)CALL_SERVER( &cs, pszText, cchText);
#else
	PWAVEOUTGETERRORTEXT pwaveOutGetErrorText;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETERRORTEXT, &pwaveOutGetErrorText, &cs ) )
	{
		pszText = MapProcessPtr( pszText, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetErrorText(mmrError, pszText, cchText);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTCLOSE)(HWAVEOUT hwo);
MMRESULT WINAPI aud_waveOutClose(HWAVEOUT hwo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTCLOSE, 1 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEOUTCLOSE pwaveOutClose;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTCLOSE, &pwaveOutClose, &cs ) )
	{
		retv = pwaveOutClose(hwo);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTPREPAREHEADER)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI aud_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pwh, cbwh);
#else
	PWAVEOUTPREPAREHEADER pwaveOutPrepareHeader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTPREPAREHEADER, &pwaveOutPrepareHeader, &cs ) )
	{
		pwh = MapProcessPtr( pwh, (LPPROCESS)cs.lpvData );
		pwh->lpData = MapProcessPtr( pwh->lpData , (LPPROCESS)cs.lpvData );

		retv = pwaveOutPrepareHeader(hwo, pwh, cbwh);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTUNPREPAREHEADER)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI aud_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTUNPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pwh, cbwh);
#else
	PWAVEOUTUNPREPAREHEADER pwaveOutUnprepareHeader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTUNPREPAREHEADER, &pwaveOutUnprepareHeader, &cs ) )
	{
		pwh = MapProcessPtr( pwh, (LPPROCESS)cs.lpvData );
		pwh->lpData = MapProcessPtr( pwh->lpData , (LPPROCESS)cs.lpvData );

		retv = pwaveOutUnprepareHeader(hwo, pwh, cbwh);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTWRITE)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI aud_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTWRITE, 3 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pwh, cbwh);
#else
	PWAVEOUTWRITE pwaveOutWrite;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTWRITE, &pwaveOutWrite, &cs ) )
	{
		pwh = MapProcessPtr( pwh, (LPPROCESS)cs.lpvData );
		pwh->lpData = MapProcessPtr( pwh->lpData , (LPPROCESS)cs.lpvData );

		retv = pwaveOutWrite(hwo, pwh, cbwh);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTPAUSE)(HWAVEOUT hwo);
MMRESULT WINAPI aud_waveOutPause(HWAVEOUT hwo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTPAUSE, 1 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEOUTPAUSE pwaveOutPause;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTPAUSE, &pwaveOutPause, &cs ) )
	{
		retv = pwaveOutPause(hwo);
		API_Leave(  );
	}
	return retv;
#endif

}


typedef MMRESULT (WINAPI *PWAVEOUTRESTART)(HWAVEOUT hwo);
MMRESULT WINAPI aud_waveOutRestart(HWAVEOUT hwo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTRESTART, 1 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEOUTRESTART pwaveOutRestart;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTRESTART, &pwaveOutRestart, &cs ) )
	{
		retv = pwaveOutRestart(hwo);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTRESET)(HWAVEOUT hwo);
MMRESULT WINAPI aud_waveOutReset(HWAVEOUT hwo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTRESET, 1 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEOUTRESET pwaveOutReset;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTRESET, &pwaveOutReset, &cs ) )
	{
		retv = pwaveOutReset(hwo);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTBREAKLOOP)(HWAVEOUT hwo);
MMRESULT WINAPI aud_waveOutBreakLoop(HWAVEOUT hwo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTBREAKLOOP, 1 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEOUTBREAKLOOP pwaveOutBreakLoop;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTBREAKLOOP, &pwaveOutBreakLoop, &cs ) )
	{
		retv = pwaveOutBreakLoop(hwo);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTGETPOSITION)(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
MMRESULT WINAPI aud_waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETPOSITION, 3 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pmmt, cbmmt);
#else
	PWAVEOUTGETPOSITION pwaveOutGetPosition;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETPOSITION, &pwaveOutGetPosition, &cs ) )
	{
		pmmt = MapProcessPtr( pmmt, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetPosition(hwo, pmmt, cbmmt);
		API_Leave(  );
	}
	return retv;
#endif

}


typedef MMRESULT (WINAPI *PWAVEOUTGETPITCH)(HWAVEOUT hwo, LPDWORD pdwPitch);
MMRESULT WINAPI aud_waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETPITCH, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pdwPitch);
#else
	PWAVEOUTGETPITCH pwaveOutGetPitch;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETPITCH, &pwaveOutGetPitch, &cs ) )
	{
		pdwPitch = MapProcessPtr( pdwPitch, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetPitch(hwo, pdwPitch);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEOUTSETPITCH)(HWAVEOUT hwo, DWORD dwPitch);
MMRESULT WINAPI aud_waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTSETPITCH, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, dwPitch);
#else
	PWAVEOUTSETPITCH pwaveOutSetPitch;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTSETPITCH, &pwaveOutSetPitch, &cs ) )
	{
		retv = pwaveOutSetPitch(hwo, dwPitch);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEOUTGETPLAYBACKRATE)(HWAVEOUT hwo, LPDWORD pdwRate);
MMRESULT WINAPI aud_waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETPLAYBACKRATE, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, pdwRate);
#else
	PWAVEOUTGETPLAYBACKRATE pwaveOutGetPlaybackRate;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETPLAYBACKRATE, &pwaveOutGetPlaybackRate, &cs ) )
	{
		pdwRate = MapProcessPtr( pdwRate, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetPlaybackRate(hwo, pdwRate);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEOUTSETPLAYBACKRATE)(HWAVEOUT hwo, DWORD dwRate);
MMRESULT WINAPI aud_waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTSETPLAYBACKRATE, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, dwRate);
#else
	PWAVEOUTSETPLAYBACKRATE pwaveOutSetPlaybackRate;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTSETPLAYBACKRATE, &pwaveOutSetPlaybackRate, &cs ) )
	{
		retv = pwaveOutSetPlaybackRate(hwo, dwRate);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEOUTGETID)(HWAVEOUT hwo, LPUINT puDeviceID);
MMRESULT WINAPI aud_waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTGETID, 2 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, puDeviceID);
#else
	PWAVEOUTGETID pwaveOutGetID;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTGETID, &pwaveOutGetID, &cs ) )
	{
		puDeviceID = MapProcessPtr( puDeviceID, (LPPROCESS)cs.lpvData );

		retv = pwaveOutGetID(hwo, puDeviceID);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEOUTMESSAGE)(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
MMRESULT WINAPI aud_waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTMESSAGE, 4 );
    cs.arg0 = (DWORD)hwo;
    return (DWORD)CALL_SERVER( &cs, uMsg, dw1, dw2);
#else
	PWAVEOUTMESSAGE pwaveOutMessage;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTMESSAGE, &pwaveOutMessage, &cs ) )
	{
		retv = pwaveOutMessage(hwo, uMsg, dw1, dw2);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEOUTOPEN)(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
MMRESULT WINAPI aud_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEOUTOPEN, 6 );
    cs.arg0 = (DWORD)phwo;
    return (DWORD)CALL_SERVER( &cs, uDeviceID,pwfx, dwCallback, dwInstance, fdwOpen);
#else
	PWAVEOUTOPEN pwaveOutOpen;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEOUTOPEN, &pwaveOutOpen, &cs ) )
	{
		pwfx = MapProcessPtr( pwfx, (LPPROCESS)cs.lpvData );

		retv = pwaveOutOpen(phwo, uDeviceID,pwfx, dwCallback, dwInstance, fdwOpen);
		API_Leave(  );
	}
	return retv;
#endif

}


typedef UINT     (WINAPI *PWAVEINGETNUMDEVS)(void);
UINT     WINAPI aud_waveInGetNumDevs(void)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINGETNUMDEVS, 0 );
    cs.arg0 = 0;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEINGETNUMDEVS pwaveInGetNumDevs;

	CALLSTACK cs;
	MMRESULT  retv = 0;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINGETNUMDEVS, &pwaveInGetNumDevs, &cs ) )
	{
		retv = pwaveInGetNumDevs();
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINGETDEVCAPS)(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
MMRESULT WINAPI aud_waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINGETDEVCAPS, 3 );
    cs.arg0 = (DWORD)uDeviceID;
    return (DWORD)CALL_SERVER( &cs, pwic, cbwic);
#else
	PWAVEINGETDEVCAPS pwaveInGetDevCaps;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINGETDEVCAPS, &pwaveInGetDevCaps, &cs ) )
	{
		pwic = MapProcessPtr( pwic, (LPPROCESS)cs.lpvData );

		retv = pwaveInGetDevCaps(uDeviceID, pwic, cbwic);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINGETERRORTEXT)(MMRESULT mmrError, LPTSTR pszText, UINT cchText);
MMRESULT WINAPI aud_waveInGetErrorText(MMRESULT mmrError, LPTSTR pszText, UINT cchText)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINGETERRORTEXT, 3 );
    cs.arg0 = (DWORD)mmrError;
    return (DWORD)CALL_SERVER( &cs, pszText, cchText);
#else
	PWAVEINGETERRORTEXT pwaveInGetErrorText;


	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINGETERRORTEXT, &pwaveInGetErrorText, &cs ) )
	{
		pszText = MapProcessPtr( pszText, (LPPROCESS)cs.lpvData );

		retv = pwaveInGetErrorText(mmrError, pszText, cchText);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINCLOSE)(HWAVEIN hwi);
MMRESULT WINAPI aud_waveInClose(HWAVEIN hwi)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINCLOSE, 1 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEINCLOSE pwaveInClose;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINCLOSE, &pwaveInClose, &cs ) )
	{
		retv = pwaveInClose(hwi);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINPREPAREHEADER)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI aud_waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs, pwh, cbwh);
#else
	PWAVEINPREPAREHEADER pwaveInPrepareHeader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINPREPAREHEADER, &pwaveInPrepareHeader, &cs ) )
	{
		pwh = MapProcessPtr( pwh, (LPPROCESS)cs.lpvData );
		pwh->lpData = MapProcessPtr( pwh->lpData , (LPPROCESS)cs.lpvData );

		retv = pwaveInPrepareHeader(hwi, pwh, cbwh);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINUNPREPAREHEADER)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI aud_waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINUNPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs, pwh, cbwh);
#else
	PWAVEINUNPREPAREHEADER pwaveInUnprepareHeader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINUNPREPAREHEADER, &pwaveInUnprepareHeader, &cs ) )
	{
		pwh = MapProcessPtr( pwh, (LPPROCESS)cs.lpvData );
		pwh->lpData = MapProcessPtr( pwh->lpData , (LPPROCESS)cs.lpvData );

		retv = pwaveInUnprepareHeader(hwi, pwh, cbwh);
		API_Leave(  );
	}
	return retv;
#endif
}


typedef MMRESULT (WINAPI *PWAVEINADDBUFFER)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
MMRESULT WINAPI aud_waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINADDBUFFER, 3 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs, pwh, cbwh);
#else
	PWAVEINADDBUFFER pwaveInAddBuffer;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINADDBUFFER, &pwaveInAddBuffer, &cs ) )
	{
		pwh = MapProcessPtr( pwh, (LPPROCESS)cs.lpvData );
		pwh->lpData = MapProcessPtr( pwh->lpData , (LPPROCESS)cs.lpvData );

		retv = pwaveInAddBuffer(hwi, pwh, cbwh);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEINSTART)(HWAVEIN hwi);
MMRESULT WINAPI aud_waveInStart(HWAVEIN hwi)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINSTART, 1 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEINSTART pwaveInStart;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINSTART, &pwaveInStart, &cs ) )
	{
		retv = pwaveInStart(hwi);
		API_Leave(  );
	}
	return retv;
#endif
}




typedef MMRESULT (WINAPI *PWAVEINSTOP)(HWAVEIN hwi);
MMRESULT WINAPI aud_waveInStop(HWAVEIN hwi)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINSTOP, 1 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEINSTOP pwaveInStop;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINSTOP, &pwaveInStop, &cs ) )
	{
		retv = pwaveInStop(hwi);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINRESET)(HWAVEIN hwi);
MMRESULT WINAPI aud_waveInReset(HWAVEIN hwi)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINRESET, 1 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs );
#else
	PWAVEINRESET pwaveInReset;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINRESET, &pwaveInReset, &cs ) )
	{
		retv = pwaveInReset(hwi);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINGETPOSITION)(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
MMRESULT WINAPI aud_waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINGETPOSITION, 3 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs, pmmt, cbmmt);
#else
	PWAVEINGETPOSITION pwaveInGetPosition;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINGETPOSITION, &pwaveInGetPosition, &cs ) )
	{
		pmmt = MapProcessPtr( pmmt, (LPPROCESS)cs.lpvData );

		retv = pwaveInGetPosition(hwi, pmmt, cbmmt);
		API_Leave(  );
	}
	return retv;
#endif
}



typedef MMRESULT (WINAPI *PWAVEINGETID)(HWAVEIN hwi, LPUINT puDeviceID);
MMRESULT WINAPI aud_waveInGetID(HWAVEIN hwi, LPUINT puDeviceID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINGETID, 2 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs, puDeviceID);
#else
	PWAVEINGETID pwaveInGetID;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINGETID, &pwaveInGetID, &cs ) )
	{
		puDeviceID = MapProcessPtr( puDeviceID, (LPPROCESS)cs.lpvData );

		retv = pwaveInGetID(hwi, puDeviceID);
		API_Leave(  );
	}
	return retv;
#endif

}


typedef MMRESULT (WINAPI *PWAVEINMESSAGE)(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
MMRESULT WINAPI aud_waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINMESSAGE, 4 );
    cs.arg0 = (DWORD)hwi;
    return (DWORD)CALL_SERVER( &cs, uMsg, dw1, dw2);
#else
	PWAVEINMESSAGE pwaveInMessage;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINMESSAGE, &pwaveInMessage, &cs ) )
	{
		retv = pwaveInMessage(hwi, uMsg, dw1, dw2);
		API_Leave(  );
	}
	return retv;
#endif

}



typedef MMRESULT (WINAPI *PWAVEINOPEN)(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
MMRESULT WINAPI aud_waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_WAVEINOPEN, 6 );
    cs.arg0 = (DWORD)phwi;
    return (DWORD)CALL_SERVER( &cs, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
#else
	PWAVEINOPEN pwaveInOpen;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_WAVEINOPEN, &pwaveInOpen, &cs ) )
	{
		pwfx = MapProcessPtr( pwfx, (LPPROCESS)cs.lpvData );

		retv = pwaveInOpen(phwi, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen);
		API_Leave(  );
	}
	return retv;
#endif
}

#ifdef MIDI_SUPPORT    

// ********************************************************************
// ������MMRESULT WINAPI aud_midiConnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
// ������
//	IN hMidi -- MIDI�����豸���
//	IN hmo  -- MIDI����豸���
//	IN pReserved -- ����������ΪNULL
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ��MIDI�����豸��һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDICONNECT)(HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);
MMRESULT WINAPI aud_midiConnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDICONNECT, 3 );
    cs.arg0 = (DWORD)hMidi;
    return (DWORD)CALL_SERVER( &cs, hmo,pReserved);
//	return ( (PMIDICONNECT)CALL_API( API_AUDIO, AUDIO_MIDICONNECT, 3 ) )(hMidi,hmo,pReserved);
#else
	PMIDICONNECT pmidiconnect;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDICONNECT, &pmidiconnect, &cs ) )
	{
		pReserved = MapProcessPtr( pReserved, (LPPROCESS)cs.lpvData );

		retv = pmidiconnect(hMidi,hmo,pReserved);
		API_Leave(  );
	}
	return retv;
#endif
}
// ********************************************************************
// ������MMRESULT WINAPI aud_midiDisconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
// ������
//	IN hMidi -- MIDI�����豸���
//	IN hmo  -- MIDI����豸���
//	IN pReserved -- ����������ΪNULL
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������Ͽ�һ��MIDI�����豸��һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIDISCONNECT)(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved);
MMRESULT WINAPI aud_midiDisconnect(  HMIDI hMidi,HMIDIOUT hmo,LPVOID pReserved)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIDISCONNECT, 3 );
    cs.arg0 = (DWORD)hMidi;
    return (DWORD)CALL_SERVER( &cs, hmo,pReserved);
//	return ( (PMIDIDISCONNECT)CALL_API( API_AUDIO, AUDIO_MIDIDISCONNECT, 3 ) )(hMidi,hmo,pReserved);
#else
	PMIDIDISCONNECT pmididisconnect;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIDISCONNECT, &pmididisconnect, &cs ) )
	{
		pReserved = MapProcessPtr( pReserved, (LPPROCESS)cs.lpvData );

		retv = pmididisconnect(hMidi,hmo,pReserved);
		API_Leave(  );
	}
	return retv;
#endif
}

// ********************************************************************
// ������MMRESULT WINAPI aud_midiInAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN lpMidiInHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiInHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ�����뻺�浽һ��MIDI�����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINADDBUFFER)(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
MMRESULT WINAPI aud_midiInAddBuffer(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINADDBUFFER, 3 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs, lpMidiInHdr,cbMidiInHdr);
//	return ( (PMIDIINADDBUFFER)CALL_API( API_AUDIO, AUDIO_MIDIINADDBUFFER, 3 ) )(hMidiIn,lpMidiInHdr,cbMidiInHdr);
#else
	PMIDIINADDBUFFER pmidiinaddbuffer;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINADDBUFFER, &pmidiinaddbuffer, &cs ) )
	{
		lpMidiInHdr = MapProcessPtr( lpMidiInHdr, (LPPROCESS)cs.lpvData );

		retv = pmidiinaddbuffer(hMidiIn,lpMidiInHdr,cbMidiInHdr);
		API_Leave(  );
	}
	return retv;
#endif
}

// ********************************************************************
// ������MMRESULT WINAPI aud_midiInClose(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������ر�һ��MIDI�����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINCLOSE)(HMIDIIN hMidiIn);
MMRESULT WINAPI aud_midiInClose(HMIDIIN hMidiIn)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINCLOSE, 1 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIINCLOSE)CALL_API( API_AUDIO, AUDIO_MIDIINCLOSE, 1 ) )(hMidiIn);
#else
	PMIDIINCLOSE pmidiinclose;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINCLOSE, &pmidiinclose, &cs ) )
	{
		retv = pmidiinclose(hMidiIn);
		API_Leave(  );
	}
	return retv;
#endif
}

// ********************************************************************
// ������MMRESULT WINAPI aud_midiInGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps)
// ������
//	IN uDeviceID -- MIDI�����豸��ID
//	IN lpMidiInCaps  -- һ��ָ��MIDIINCAPS�ṹ��ָ��
//	IN cbMidiInCaps -- �ṹMIDIINCAPS�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI�����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINGETDEVCAPS)(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps);
MMRESULT WINAPI aud_midiInGetDevCaps(UINT uDeviceID,LPMIDIINCAPS lpMidiInCaps,UINT cbMidiInCaps)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINGETDEVCAPS, 3 );
    cs.arg0 = (DWORD)uDeviceID;
    return (DWORD)CALL_SERVER( &cs, lpMidiInCaps,cbMidiInCaps);
//	return ( (PMIDIINGETDEVCAPS)CALL_API( API_AUDIO, AUDIO_MIDIINGETDEVCAPS, 3 ) )(uDeviceID,lpMidiInCaps,cbMidiInCaps);
#else
	PMIDIINGETDEVCAPS pmidiingetdevcaps;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINGETDEVCAPS, &pmidiingetdevcaps, &cs ) )
	{
		lpMidiInCaps = MapProcessPtr( lpMidiInCaps, (LPPROCESS)cs.lpvData );

		retv = pmidiingetdevcaps(uDeviceID,lpMidiInCaps,cbMidiInCaps);
		API_Leave(  );
	}
	return retv;
#endif
}

// ********************************************************************
// ������MMRESULT WINAPI aud_midiInGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText )
// ������
//	IN wError -- �������
//	IN lpText  -- һ����Ŵ����ı���ָ��
//	IN cchText -- �ı�����Ĵ�С
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ����������ı�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINGETERRORTEXT)(MMRESULT wError,LPSTR lpText,UINT cchText );
MMRESULT WINAPI aud_midiInGetErrorText(MMRESULT wError,LPSTR lpText,UINT cchText )
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINGETERRORTEXT, 3 );
    cs.arg0 = (DWORD)wError;
    return (DWORD)CALL_SERVER( &cs,lpText,cchText );
//	return ( (PMIDIINGETERRORTEXT)CALL_API( API_AUDIO, AUDIO_MIDIINGETERRORTEXT, 3 ) )(wError,lpText,cchText );
#else
	PMIDIINGETERRORTEXT pmidiingeterrortext;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINGETERRORTEXT, &pmidiingeterrortext, &cs ) )
	{
		lpText = MapProcessPtr( lpText, (LPPROCESS)cs.lpvData );

		retv = pmidiingeterrortext(wError,lpText,cchText );
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiInGetID(HMIDIIN hmi,LPUINT puDeviceID)
// ������
//	IN hmi -- MIDI�����豸���
//	IN puDeviceID  -- һ����������豸ID��ָ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI�����豸��ID
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINGETID)(HMIDIIN hmi,LPUINT puDeviceID);
MMRESULT WINAPI aud_midiInGetID(HMIDIIN hmi,LPUINT puDeviceID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINGETID, 2 );
    cs.arg0 = (DWORD)hmi;
    return (DWORD)CALL_SERVER( &cs, puDeviceID);
//	return ( (PMIDIINGETID)CALL_API( API_AUDIO, AUDIO_MIDIINGETID, 2 ) )(hmi,puDeviceID);
#else
	PMIDIINGETID pmidiingetid;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINGETID, &pmidiingetid, &cs ) )
	{
		puDeviceID = MapProcessPtr( puDeviceID, (LPPROCESS)cs.lpvData );

		retv = pmidiingetid(hmi,puDeviceID);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������UINT WINAPI aud_midiInGetNumDevs(VOID)
// ������
//	��
// ����ֵ��
//	�ɹ����ص�ǰMIDI�����豸�ĸ�����
// �����������õ���ǰMIDI�����豸�ĸ���
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef UINT (WINAPI *PMIDIINGETNUMDEVS)(VOID);
UINT WINAPI aud_midiInGetNumDevs(VOID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINGETNUMDEVS, 0 );
    cs.arg0 = (DWORD)0;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIINGETNUMDEVS)CALL_API( API_AUDIO, AUDIO_MIDIINGETNUMDEVS, 0 ) )();
#else
	PMIDIINGETNUMDEVS pmidiingetnumdevs;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINGETNUMDEVS, &pmidiingetnumdevs, &cs ) )
	{
		retv = pmidiingetnumdevs();
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������DWORD WINAPI aud_midiInMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2)
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
typedef DWORD (WINAPI *PMIDIINMESSAGE)(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2);
DWORD WINAPI aud_midiInMessage(HMIDIIN hMidiIn,UINT msg,DWORD dw1,DWORD dw2)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINMESSAGE, 4 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs, msg,dw1,dw2);
//	return ( (PMIDIINMESSAGE)CALL_API( API_AUDIO, AUDIO_MIDIINMESSAGE, 4 ) )(hMidiIn,msg,dw1,dw2);
#else
	PMIDIINMESSAGE pmidiinmessage;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINMESSAGE, &pmidiinmessage, &cs ) )
	{
		retv = pmidiinmessage(hMidiIn,msg,dw1,dw2);
		API_Leave(  );
	}
	return retv;
#endif
}



// ********************************************************************
// ������MMRESULT WINAPI aud_midiInOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
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
typedef MMRESULT (WINAPI *PMIDIINOPEN)(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);
MMRESULT WINAPI aud_midiInOpen(LPHMIDIIN lphMidiIn,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINOPEN, 5 );
    cs.arg0 = (DWORD)lphMidiIn;
    return (DWORD)CALL_SERVER( &cs, uDeviceID,dwCallback,dwCallbackInstance,dwFlags);
//	return ( (PMIDIINOPEN)CALL_API( API_AUDIO, AUDIO_MIDIINOPEN, 5 ) )(lphMidiIn,uDeviceID,dwCallback,dwCallbackInstance,dwFlags);
#else
	PMIDIINOPEN pmidiinopen;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINOPEN, &pmidiinopen, &cs ) )
	{
		lphMidiIn = MapProcessPtr( lphMidiIn, (LPPROCESS)cs.lpvData );

		retv = pmidiinopen(lphMidiIn,uDeviceID,dwCallback,dwCallbackInstance,dwFlags);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiInPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN lpMidiInHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiInHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������׼��һ�����뻺�档
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINPREPAREHEADER)(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
MMRESULT WINAPI aud_midiInPrepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs, lpMidiInHdr,cbMidiInHdr);
//	return ( (PMIDIINPREPAREHEADER)CALL_API( API_AUDIO, AUDIO_MIDIINPREPAREHEADER, 3 ) )(hMidiIn,lpMidiInHdr,cbMidiInHdr);
#else
	PMIDIINPREPAREHEADER pmidiinprepareheader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINPREPAREHEADER, &pmidiinprepareheader, &cs ) )
	{
		lpMidiInHdr = MapProcessPtr( lpMidiInHdr, (LPPROCESS)cs.lpvData );

		retv = pmidiinprepareheader(hMidiIn,lpMidiInHdr,cbMidiInHdr);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiInReset(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ�������豸�������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINRESET)(HMIDIIN hMidiIn);
MMRESULT WINAPI aud_midiInReset(HMIDIIN hMidiIn)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINRESET, 1 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIINRESET)CALL_API( API_AUDIO, AUDIO_MIDIINRESET, 1 ) )(hMidiIn);
#else
	PMIDIINRESET pmidiinreset;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINRESET, &pmidiinreset, &cs ) )
	{
		retv = pmidiinreset(hMidiIn);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiInStart(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������һ��ָ�������豸��ʼ�������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINSTART)(HMIDIIN hMidiIn);
MMRESULT WINAPI aud_midiInStart(HMIDIIN hMidiIn)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINSTART, 1 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIINSTART)CALL_API( API_AUDIO, AUDIO_MIDIINSTART, 1 ) )(hMidiIn);
#else
	PMIDIINSTART pmidiinstart;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINSTART, &pmidiinstart, &cs ) )
	{
		retv = pmidiinstart(hMidiIn);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiInStop(HMIDIIN hMidiIn)
// ������
//	IN hMidiIn -- MIDI�����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ�������豸�������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINSTOP)(HMIDIIN hMidiIn);
MMRESULT WINAPI aud_midiInStop(HMIDIIN hMidiIn)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINSTOP, 1 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIINSTOP)CALL_API( API_AUDIO, AUDIO_MIDIINSTOP, 1 ) )(hMidiIn);
#else
	PMIDIINSTOP pmidiinstop;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINSTOP, &pmidiinstop, &cs ) )
	{
		retv = pmidiinstop(hMidiIn);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiInUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
// ������
//	IN hMidiIn -- MIDI�����豸���
//	IN lpMidiInHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiInHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������һ���Ѿ�׼�������뻺�档
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIINUNPREPAREHEADER)(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr);
MMRESULT WINAPI aud_midiInUnprepareHeader(HMIDIIN hMidiIn,LPMIDIHDR lpMidiInHdr,UINT cbMidiInHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIINUNPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hMidiIn;
    return (DWORD)CALL_SERVER( &cs, lpMidiInHdr,cbMidiInHdr);
//	return ( (PMIDIINUNPREPAREHEADER)CALL_API( API_AUDIO, AUDIO_MIDIINUNPREPAREHEADER, 5 ) )(hMidiIn,lpMidiInHdr,cbMidiInHdr);
#else
	PMIDIINUNPREPAREHEADER pmidiinunprepareheader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIINUNPREPAREHEADER, &pmidiinunprepareheader, &cs ) )
	{
		lpMidiInHdr = MapProcessPtr( lpMidiInHdr, (LPPROCESS)cs.lpvData );

		retv = pmidiinunprepareheader(hMidiIn,lpMidiInHdr,cbMidiInHdr);
		API_Leave(  );
	}
	return retv;
#endif
}




// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags)
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
typedef MMRESULT (WINAPI *PMIDIOUTCACHEDRUMPATCHES)(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags);
MMRESULT WINAPI aud_midiOutCacheDrumPatches(HMIDIOUT hmo,UINT wPatch,WORD * lpKeyArray,UINT wFlags)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTCACHEDRUMPATCHES, 4 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, wPatch,lpKeyArray,wFlags);
//	return ( (PMIDIOUTCACHEDRUMPATCHES)CALL_API( API_AUDIO, AUDIO_MIDIOUTCACHEDRUMPATCHES, 4 ) )(hmo,wPatch,lpKeyArray,wFlags);
#else
	PMIDIOUTCACHEDRUMPATCHES pmidioutcachedrumpatches;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTCACHEDRUMPATCHES, &pmidioutcachedrumpatches, &cs ) )
	{
		lpKeyArray = MapProcessPtr( lpKeyArray, (LPPROCESS)cs.lpvData );

		retv = pmidioutcachedrumpatches(hmo,wPatch,lpKeyArray,wFlags);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags)
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
typedef MMRESULT (WINAPI *PMIDIOUTCACHEPATCHES)(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags);
MMRESULT WINAPI aud_midiOutCachePatches(HMIDIOUT hmo,UINT wBank,WORD * lpPatchArray,UINT wFlags)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTCACHEPATCHES, 4 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, wBank,lpPatchArray,wFlags);
//	return ( (PMIDIOUTCACHEPATCHES)CALL_API( API_AUDIO, AUDIO_MIDIOUTCACHEPATCHES, 4 ) )(hmo,wBank,lpPatchArray,wFlags);
#else
	PMIDIOUTCACHEPATCHES pmidioutcachepatches;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTCACHEPATCHES, &pmidioutcachepatches, &cs ) )
	{
		lpPatchArray = MapProcessPtr( lpPatchArray, (LPPROCESS)cs.lpvData );

		retv = pmidioutcachepatches(hmo,wBank,lpPatchArray,wFlags);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MRESULT WINAPI aud_midiOutClose(HMIDIOUT hmo)
// ������
//	IN hmo -- MIDI����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������ر�һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTCLOSE)(HMIDIOUT hmo);
MMRESULT WINAPI aud_midiOutClose(HMIDIOUT hmo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTCLOSE, 1 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIOUTCLOSE)CALL_API( API_AUDIO, AUDIO_MIDIOUTCLOSE, 1 ) )(hmo);
#else
	PMIDIOUTCLOSE pmidioutclose;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTCLOSE, &pmidioutclose, &cs ) )
	{
		retv = pmidioutclose(hmo);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps)
// ������
//	IN uDeviceID -- MIDI����豸��ID
//	IN lpMidiOutCaps  -- һ��ָ��MIDIOUTCAPS �ṹ��ָ��
//	IN cbMidiOutCaps -- �ṹMIDIOUTCAPS �ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTGETDEVCAPS)(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps);
MMRESULT WINAPI aud_midiOutGetDevCaps(UINT uDeviceID,LPMIDIOUTCAPS lpMidiOutCaps,UINT cbMidiOutCaps)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTGETDEVCAPS, 3 );
    cs.arg0 = (DWORD)uDeviceID;
    return (DWORD)CALL_SERVER( &cs, lpMidiOutCaps,cbMidiOutCaps);
//	return ( (PMIDIOUTGETDEVCAPS)CALL_API( API_AUDIO, AUDIO_MIDIOUTGETDEVCAPS, 3) )(uDeviceID,lpMidiOutCaps,cbMidiOutCaps);
#else
	PMIDIOUTGETDEVCAPS pmidioutgetdevcaps;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTGETDEVCAPS, &pmidioutgetdevcaps, &cs ) )
	{
		lpMidiOutCaps = MapProcessPtr( lpMidiOutCaps, (LPPROCESS)cs.lpvData );

		retv = pmidioutgetdevcaps(uDeviceID,lpMidiOutCaps,cbMidiOutCaps);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������UINT WINAPI aud_midiOutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText)
// ������
//	IN wError -- �������
//	IN lpText  -- һ����Ŵ����ı���ָ��
//	IN cchText -- �ı�����Ĵ�С
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ����������ı�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef UINT (WINAPI *PMIDIOUTGETERRORTEXT)(MMRESULT mmrError,LPSTR lpText,UINT cchText);
UINT WINAPI aud_midiOutGetErrorText(MMRESULT mmrError,LPSTR lpText,UINT cchText)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTGETERRORTEXT, 3 );
    cs.arg0 = (DWORD)mmrError;
    return (DWORD)CALL_SERVER( &cs, lpText,cchText);
//	return ( (PMIDIOUTGETERRORTEXT)CALL_API( API_AUDIO, AUDIO_MIDIOUTGETERRORTEXT, 3 ) )(mmrError,lpText,cchText);
#else
	PMIDIOUTGETERRORTEXT pmidioutgeterrortext;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTGETERRORTEXT, &pmidioutgeterrortext, &cs ) )
	{
		lpText = MapProcessPtr( lpText, (LPPROCESS)cs.lpvData );

		retv = pmidioutgeterrortext(mmrError,lpText,cchText);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutGetID(HMIDIOUT hmo,LPUINT puDeviceID)
// ������
//	IN hmo -- MIDI����豸���
//	IN puDeviceID  -- һ����������豸ID��ָ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI����豸��ID
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTGETID)(HMIDIOUT hmo,LPUINT puDeviceID);
MMRESULT WINAPI aud_midiOutGetID(HMIDIOUT hmo,LPUINT puDeviceID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTGETID, 2 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, puDeviceID);
//	return ( (PMIDIOUTGETID)CALL_API( API_AUDIO, AUDIO_MIDIOUTGETID, 2 ) )(hmo,puDeviceID);
#else
	PMIDIOUTGETID pmidioutgetid;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTGETID, &pmidioutgetid, &cs ) )
	{
		puDeviceID = MapProcessPtr( puDeviceID, (LPPROCESS)cs.lpvData );

		retv = pmidioutgetid(hmo,puDeviceID);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������UINT WINAPI aud_midiOutGetNumDevs(VOID)
// ������
//	��
// ����ֵ��
//	�ɹ����ص�ǰMIDI����豸�ĸ�����
// �����������õ���ǰMIDI����豸�ĸ���
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef UINT (WINAPI *PMIDIOUTGETNUMDEVS)(VOID);
UINT WINAPI aud_midiOutGetNumDevs(VOID)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTGETNUMDEVS, 0 );
    cs.arg0 = (DWORD)0;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIOUTGETNUMDEVS)CALL_API( API_AUDIO, AUDIO_MIDIOUTGETNUMDEVS, 0 ) )();
#else
	PMIDIOUTGETNUMDEVS pmidioutgetnumdevs;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTGETNUMDEVS, &pmidioutgetnumdevs, &cs ) )
	{
		retv = pmidioutgetnumdevs();
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume)
// ������
//	IN hmo -- MIDI����豸���
//	OUT lpdwVolume -- ��ŵ�ǰ������ָ��
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ���ǰMIDI����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTGETVOLUME)(HMIDIOUT hmo,LPDWORD lpdwVolume);
MMRESULT WINAPI aud_midiOutGetVolume(HMIDIOUT hmo,LPDWORD lpdwVolume)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTGETVOLUME, 2 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, lpdwVolume);
//	return ( (PMIDIOUTGETVOLUME)CALL_API( API_AUDIO, AUDIO_MIDIOUTGETVOLUME, 2 ) )(hmo,lpdwVolume);
#else
	PMIDIOUTGETVOLUME pmidioutgetvolume;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTGETVOLUME, &pmidioutgetvolume, &cs ) )
	{
		lpdwVolume = MapProcessPtr( lpdwVolume, (LPPROCESS)cs.lpvData );

		retv = pmidioutgetvolume(hmo,lpdwVolume);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hmo -- MIDI����豸���
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ����������һ��MIDI����豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTLONGMSG)(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
MMRESULT WINAPI aud_midiOutLongMsg(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTLONGMSG, 3 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, lpMidiOutHdr,cbMidiOutHdr);
//	return ( (PMIDIOUTLONGMSG)CALL_API( API_AUDIO, AUDIO_MIDIOUTLONGMSG, 3 ) )(hmo,lpMidiOutHdr,cbMidiOutHdr);
#else
	PMIDIOUTLONGMSG pmidioutlongmsg;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTLONGMSG, &pmidioutlongmsg, &cs ) )
	{
		lpMidiOutHdr = MapProcessPtr( lpMidiOutHdr, (LPPROCESS)cs.lpvData );

		retv = pmidioutlongmsg(hmo,lpMidiOutHdr,cbMidiOutHdr);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������DWORD WINAPI aud_midiOutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2)
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
typedef DWORD (WINAPI *PMIDIOUTMESSAGE)(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2);
DWORD WINAPI aud_midiOutMessage(HMIDIOUT hmo,UINT msg,DWORD dw1,DWORD dw2)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTMESSAGE, 4 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, msg,dw1,dw2);
//	return ( (PMIDIOUTMESSAGE)CALL_API( API_AUDIO, AUDIO_MIDIOUTMESSAGE, 4 ) )(hmo,msg,dw1,dw2);
#else
	PMIDIOUTMESSAGE pmidioutmessage;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTMESSAGE, &pmidioutmessage, &cs ) )
	{
		retv = pmidioutmessage(hmo,msg,dw1,dw2);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������UINT WINAPI aud_midiOutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
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
typedef UINT (WINAPI *PMIDIOUTOPEN)(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags);
UINT WINAPI aud_midiOutOpen(LPHMIDIOUT lphmo,UINT uDeviceID,DWORD dwCallback,DWORD dwCallbackInstance,DWORD dwFlags)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTOPEN, 5 );
    cs.arg0 = (DWORD)lphmo;
    return (DWORD)CALL_SERVER( &cs, uDeviceID,dwCallback,dwCallbackInstance,dwFlags);
//	return ( (PMIDIOUTOPEN)CALL_API( API_AUDIO, AUDIO_MIDIOUTOPEN, 5 ) )(lphmo,uDeviceID,dwCallback,dwCallbackInstance,dwFlags);
#else
	PMIDIOUTOPEN pmidioutopen;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTOPEN, &pmidioutopen, &cs ) )
	{
		lphmo = MapProcessPtr( lphmo, (LPPROCESS)cs.lpvData );

		retv = pmidioutopen(lphmo,uDeviceID,dwCallback,dwCallbackInstance,dwFlags);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hmo -- MIDI����豸���
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������׼��һ��������档
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTPREPAREHEADER)(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
MMRESULT WINAPI aud_midiOutPrepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, lpMidiOutHdr,cbMidiOutHdr);
//	return ( (PMIDIOUTPREPAREHEADER)CALL_API( API_AUDIO, AUDIO_MIDIOUTPREPAREHEADER, 3 ) )(hmo,lpMidiOutHdr,cbMidiOutHdr);
#else
	PMIDIOUTPREPAREHEADER pmidioutprepareheader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTPREPAREHEADER, &pmidioutprepareheader, &cs ) )
	{
		lpMidiOutHdr = MapProcessPtr( lpMidiOutHdr, (LPPROCESS)cs.lpvData );

		retv = pmidioutprepareheader(hmo,lpMidiOutHdr,cbMidiOutHdr);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutReset(HMIDIOUT hmo)
// ������
//	IN hmo -- MIDI����豸���
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ������豸������ݡ�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTRESET)(HMIDIOUT hmo);
MMRESULT WINAPI aud_midiOutReset(HMIDIOUT hmo)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTRESET, 1 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDIOUTRESET)CALL_API( API_AUDIO, AUDIO_MIDIOUTRESET, 1 ) )(hmo);
#else
	PMIDIOUTRESET pmidioutreset;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTRESET, &pmidioutreset, &cs ) )
	{
		retv = pmidioutreset(hmo);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutSetVolume(HMIDIOUT hmo,DWORD dwVolume)
// ������
//	IN hmo -- MIDI����豸���
//	IN dwVolume -- ��ǰҪ���õ�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������õ�ǰMIDI����豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTSETVOLUME)(HMIDIOUT hmo,DWORD dwVolume);
MMRESULT WINAPI aud_midiOutSetVolume(HMIDIOUT hmo,DWORD dwVolume)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTSETVOLUME, 2 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, dwVolume);
//	return ( (PMIDIOUTSETVOLUME)CALL_API( API_AUDIO, AUDIO_MIDIOUTSETVOLUME, 2 ) )(hmo,dwVolume);
#else
	PMIDIOUTSETVOLUME pmidioutsetvolume;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTSETVOLUME, &pmidioutsetvolume, &cs ) )
	{
		retv = pmidioutsetvolume(hmo,dwVolume);
		API_Leave(  );
	}
	return retv;
#endif
}

// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
// ������
//	IN hmo -- MIDI����豸���
//	IN dwMsg -- ��ǰҪ���͵���Ϣ
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������ǰMIDI����豸����һ���ϵ�MIDI��Ϣ��
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTSHORTMSG)(HMIDIOUT hmo, DWORD dwMsg);
MMRESULT WINAPI aud_midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTSHORTMSG, 2 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, dwMsg);
//	return ( (PMIDIOUTSHORTMSG)CALL_API( API_AUDIO, AUDIO_MIDIOUTSHORTMSG, 2 ) )(hmo, dwMsg);
#else
	PMIDIOUTSHORTMSG pmidioutshortmsg;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTSHORTMSG, &pmidioutshortmsg, &cs ) )
	{
		retv = pmidioutshortmsg(hmo, dwMsg);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiOutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hmo -- MIDI����豸���
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������һ���Ѿ�׼����������档
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDIOUTUNPREPAREHEADER)(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
MMRESULT WINAPI aud_midiOutUnprepareHeader(HMIDIOUT hmo,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDIOUTUNPREPAREHEADER, 3 );
    cs.arg0 = (DWORD)hmo;
    return (DWORD)CALL_SERVER( &cs, lpMidiOutHdr,cbMidiOutHdr);
//	return ( (PMIDIOUTUNPREPAREHEADER)CALL_API( API_AUDIO, AUDIO_MIDIOUTUNPREPAREHEADER, 3 ) )(hmo,lpMidiOutHdr,cbMidiOutHdr);
#else
	PMIDIOUTUNPREPAREHEADER pmidioutunprepareheader;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDIOUTUNPREPAREHEADER, &pmidioutunprepareheader, &cs ) )
	{
		lpMidiOutHdr = MapProcessPtr( lpMidiOutHdr, (LPPROCESS)cs.lpvData );

		retv = pmidioutunprepareheader(hmo,lpMidiOutHdr,cbMidiOutHdr);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamClose(HMIDISTRM hStream)
// ������
//	IN hStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������ر�һ��MIDI�������
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMCLOSE)(HMIDISTRM hStream);
MMRESULT WINAPI aud_midiStreamClose(HMIDISTRM hStream)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMCLOSE, 1 );
    cs.arg0 = (DWORD)hStream;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDISTREAMCLOSE)CALL_API( API_AUDIO, AUDIO_MIDISTREAMCLOSE, 1 ) )(hStream);
#else
	PMIDISTREAMCLOSE pmidistreamclose;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMCLOSE, &pmidistreamclose, &cs ) )
	{
		retv = pmidistreamclose(hStream);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen)
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
typedef MMRESULT (WINAPI *PMIDISTREAMOPEN)(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen);
MMRESULT WINAPI aud_midiStreamOpen(LPHMIDISTRM lphStream,LPUINT puDeviceID,DWORD cMidi,DWORD dwCallback,DWORD dwInstance,DWORD fdwOpen)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMOPEN, 6 );
    cs.arg0 = (DWORD)lphStream;
    return (DWORD)CALL_SERVER( &cs, puDeviceID,cMidi,dwCallback,dwInstance,fdwOpen);
//	return ( (PMIDISTREAMOPEN)CALL_API( API_AUDIO, AUDIO_MIDISTREAMOPEN, 6 ) )(lphStream,puDeviceID,cMidi,dwCallback,dwInstance,fdwOpen);
#else
	PMIDISTREAMOPEN pmidistreamopen;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMOPEN, &pmidistreamopen, &cs ) )
	{
		lphStream = MapProcessPtr( lphStream, (LPPROCESS)cs.lpvData );
		puDeviceID = MapProcessPtr( puDeviceID, (LPPROCESS)cs.lpvData );

		retv = pmidistreamopen(lphStream,puDeviceID,cMidi,dwCallback,dwInstance,fdwOpen);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
// ������
//	IN hMidiStream -- MIDI�����
//	IN lpMidiOutHdr  -- һ��ָ��MIDIHDR�ṹ��ָ��
//	IN cbMidiOutHdr -- �ṹMIDIHDR�ĳߴ�
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ��������������һ����������һ��MIDI���豸
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMOUT)(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr);
MMRESULT WINAPI aud_midiStreamOut(HMIDISTRM hMidiStream,LPMIDIHDR lpMidiOutHdr,UINT cbMidiOutHdr)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMOUT, 3 );
    cs.arg0 = (DWORD)hMidiStream;
    return (DWORD)CALL_SERVER( &cs, lpMidiOutHdr,cbMidiOutHdr);
//	return ( (PMIDISTREAMOUT)CALL_API( API_AUDIO, AUDIO_MIDISTREAMOUT, 3 ) )(hMidiStream,lpMidiOutHdr,cbMidiOutHdr);
#else
	PMIDISTREAMOUT pmidistreamout;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMOUT, &pmidistreamout, &cs ) )
	{
		lpMidiOutHdr = MapProcessPtr( lpMidiOutHdr, (LPPROCESS)cs.lpvData );

		retv = pmidistreamout(hMidiStream,lpMidiOutHdr,cbMidiOutHdr);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamPause(HMIDISTRM hMidiStream)
// ������
//	IN hMidiStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ������������ͣһ��MIDI���豸����
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMPAUSE)(HMIDISTRM hMidiStream);
MMRESULT WINAPI aud_midiStreamPause(HMIDISTRM hMidiStream)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMPAUSE, 1 );
    cs.arg0 = (DWORD)hMidiStream;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDISTREAMPAUSE)CALL_API( API_AUDIO, AUDIO_MIDISTREAMPAUSE, 1 ) )(hMidiStream);
#else
	PMIDISTREAMPAUSE pmidistreampause;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMPAUSE, &pmidistreampause, &cs ) )
	{
		retv = pmidistreampause(hMidiStream);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt)
// ������
//	IN hMidiStream -- MIDI�����
//  IN pmmt	-- һ��ָ��MMTIME�ṹ��ָ��
//  IN cbmmt -- MMTIME�ṹ�Ĵ�С
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�MIDI���豸�Ѿ����ŵ�λ��
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMPOSITION)(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt);
MMRESULT WINAPI aud_midiStreamPosition(HMIDISTRM hMidiStream,LPMMTIME pmmt,UINT cbmmt)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMPOSITION, 3 );
    cs.arg0 = (DWORD)hMidiStream;
    return (DWORD)CALL_SERVER( &cs, pmmt,cbmmt);
//	return ( (PMIDISTREAMPOSITION)CALL_API( API_AUDIO, AUDIO_MIDISTREAMPOSITION, 3 ) )(hMidiStream,pmmt,cbmmt);
#else
	PMIDISTREAMPOSITION pmidistreamposition;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMPOSITION, &pmidistreamposition, &cs ) )
	{
		pmmt = MapProcessPtr( pmmt, (LPPROCESS)cs.lpvData );

		retv = pmidistreamposition(hMidiStream,pmmt,cbmmt);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty)
// ������
//	IN hMidiStream -- MIDI�����
//  IN lppropdata	-- һ��ָ���������ݵ�ָ��
//  IN dwProperty -- ���Ե�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �����������õ�������MIDI���豸������
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMPROPERTY)(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty);
MMRESULT WINAPI aud_midiStreamProperty(HMIDISTRM hMidiStream,LPBYTE lppropdata,DWORD dwProperty)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMPROPERTY, 3 );
    cs.arg0 = (DWORD)hMidiStream;
    return (DWORD)CALL_SERVER( &cs, lppropdata,dwProperty);
//	return ( (PMIDISTREAMPROPERTY)CALL_API( API_AUDIO, AUDIO_MIDISTREAMPROPERTY, 3 ) )(hMidiStream,lppropdata,dwProperty);
#else
	PMIDISTREAMPROPERTY pmidistreamproperty;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMPROPERTY, &pmidistreamproperty, &cs ) )
	{
		lppropdata = MapProcessPtr( lppropdata, (LPPROCESS)cs.lpvData );

		retv = pmidistreamproperty(hMidiStream,lppropdata,dwProperty);
		API_Leave(  );
	}
	return retv;
#endif
}



// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamRestart(HMIDISTRM hMidiStream)
// ������
//	IN hMidiStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// �������������¿�ʼһ����ͣ��MIDI�����š�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMRESTART)(HMIDISTRM hMidiStream);
MMRESULT WINAPI aud_midiStreamRestart(HMIDISTRM hMidiStream)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMRESTART, 1 );
    cs.arg0 = (DWORD)hMidiStream;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDISTREAMRESTART)CALL_API( API_AUDIO, AUDIO_MIDISTREAMRESTART, 5 ) )(hMidiStream);
#else
	PMIDISTREAMRESTART pmidistreamrestart;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMRESTART, &pmidistreamrestart, &cs ) )
	{
		retv = pmidistreamrestart(hMidiStream);
		API_Leave(  );
	}
	return retv;
#endif
}


// ********************************************************************
// ������MMRESULT WINAPI aud_midiStreamStop(HMIDISTRM hMidiStream)
// ������
//	IN hMidiStream -- MIDI�����
// ����ֵ��
//	�ɹ�����MMSYSERR_NOERROR�����򷵻�������
// ����������ֹͣһ����MIDI�����š�
// ����: MIDI API�����ӿ�
// ********************************************************************
typedef MMRESULT (WINAPI *PMIDISTREAMSTOP)(HMIDISTRM hMidiStream);
MMRESULT WINAPI aud_midiStreamStop(HMIDISTRM hMidiStream)
{
#ifdef CALL_TRAP
    CALLTRAP cs;
    cs.apiInfo = CALL_API( API_AUDIO, AUDIO_MIDISTREAMSTOP, 1 );
    cs.arg0 = (DWORD)hMidiStream;
    return (DWORD)CALL_SERVER( &cs);
//	return ( (PMIDISTREAMSTOP)CALL_API( API_AUDIO, AUDIO_MIDISTREAMSTOP, 1 ) )(hMidiStream);
#else
	PMIDISTREAMSTOP pmidistreamstop;

	CALLSTACK cs;
	MMRESULT  retv = MMSYSERR_ERROR;

	if( API_Enter( API_AUDIO, AUDIO_MIDISTREAMSTOP, &pmidistreamstop, &cs ) )
	{
		retv = pmidistreamstop(hMidiStream);
		API_Leave(  );
	}
	return retv;
#endif
}
#endif