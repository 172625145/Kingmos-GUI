#ifndef __ECANOLSTVIEW_H
#define __ECANOLSTVIEW_H

#define CANO_ALBUMINDEX_LS (1 << 0)
#define CANO_IMGINDEX_LS (1 << 1)
#define CANO_FILEVIEW_LS (1 << 2)

extern TCHAR szCANOLST[];
extern ATOM RegisterCanolstClass(HINSTANCE hInstance);

HANDLE Canolst_FindFirstListFile(HWND hWnd, TCHAR szFile[MAX_PATH]);
BOOL Canolst_FindNextListFile(HANDLE hFind, TCHAR szFile[MAX_PATH]);
BOOL Canolst_FindPrevListFile(HANDLE hFind, TCHAR szFile[MAX_PATH]);
BOOL Canolst_FindCurrentFile(HANDLE hFind, TCHAR szFile[MAX_PATH]);
BOOL Canolst_FindClose(HANDLE hList);

#define CANLM_GETSELITEM_FILEPATH (WM_USER + 100)
//wParam:NULL
//lParam:szPathFile

#define CANLM_LOAD_PATHFILE (WM_USER + 101)
//wParam:NULL
//lParam:szPathFile


#endif  //__ECANOLSTVIEW_H
