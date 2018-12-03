#include<ewindows.h>
#include"mediaplay.h"
BOOL PlaySound(
  LPCSTR pszSound,
  HMODULE hmod,
  DWORD fdwSound
)
{
	_au1200mediaplay(pszSound, 0);
	return TRUE;
}