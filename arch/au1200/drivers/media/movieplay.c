#include<ewindows.h>
#include "mediaplay.h"
BOOL PlayMovie(
  LPCSTR pszMovie,  
  HMODULE hmod,     
  DWORD fdwSound 
)
{
	_au1200mediaplay(pszMovie, 1);
	return TRUE;
}