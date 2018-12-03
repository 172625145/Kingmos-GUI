#ifndef __H_EFILEDIALOG__
#define __H_EFILEDIALOG__


//#define MAX_EXTENNAME				10

/*typedef struct tagOFN { 
  DWORD         lStructSize; 
  HWND          hWndOwner; 
  TCHAR			pFilter[MAX_EXTENNAME]; 
  TCHAR         pFileName[MAX_PATH]; 
  TCHAR         pDir[MAX_PATH]; 
  TCHAR         pTitle[MAX_PATH]; 
  TCHAR         pFileExten[MAX_EXTENNAME]; 
}OFN,*LPOFN; */
typedef struct tagOFN { /* ofn */
	DWORD lStructSize;
	HWND hWndOwner;
//	HINSTANCE hInstance;
//	LPCSTR lpstrFilter;
//	LPSTR lpstrCustomFilter;
//	DWORD nMaxCustFilter;
//	DWORD nFilterIndex;
	TCHAR *lpstrFile;
	DWORD nMaxFile;
	TCHAR *lpstrFileTitle;
	DWORD nMaxFileTitle;
	TCHAR *lpstrInitialDir;
	TCHAR *lpstrTitle;
//	DWORD Flags;
//	WORD nFileOffset;
//	WORD nFileExtension;
//	LPCSTR lpstrDefExt;
//	DWORD lCustData;
//	LPOFNHOOKPROC lpfnHook;
//	LPCSTR lpTemplateName;
} OPENFILENAME,*LPOPENFILENAME ;

#endif
