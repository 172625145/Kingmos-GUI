/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __EXPLORER_H
#define __EXPLORER_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

extern const char classFileBrowser[];
extern ATOM RegisterFileBrowserClass(HINSTANCE hInstance);

#define FBS_NOSIZECOLUMN	0x0001  // ����ʾ�ļ��ߴ���
#define FBS_NODATECOLUMN	0x0002  // ����ʾ������
#define FBS_NODIRCOLUMN		0x0004  // ����ʾĿ¼��
#define FBS_SHOWSUBDIR		0x0004  // ��ʾ��Ŀ¼��Ŀ


typedef BOOL (CALLBACK *FindFile)(LPTSTR lpFileName,FILE_FIND_DATA	*FindFileData);

/*
	�õ���ʾ�ľ��
	wParam = 0
	lParam = 0
*/
#define EXM_GETLISTVIEW		(WM_OS + 1345)       // �õ���ʾ�ؼ��ľ��
/*
	���õ�ǰ·��
	wParam = 0
	lParam = (LPTSTR)pCurPath ; //�µõ�ǰ·��
*/
#define EXM_SETCURPATH		(WM_OS + 1346)
/*
	�õ���ǰ·��
	wParam = 0
	lParam = (LPTSTR)pCurPath ; //Ҫ���õ�ǰ·���Ļ��棬��СΪMAX_PATH
*/
#define EXM_GETCURPATH		(WM_OS + 1347)
/*
	����һ��Ŀ¼
	wParam = 0
	lParam = 0
*/
#define EXM_UPPATH			(WM_OS + 1348)
/*
	ɾ��ѡ����ļ�
	wParam = 0
	lParam = 0
*/
#define EXM_DELETESEL		(WM_OS + 1349)
/*
	�����ļ�
	wParam = (FindFile)FindFileFunc; // ���ҵ�һ���ļ���ص��û��費��Ҫ���ļ�
	lParam = (LPTSTR)lpFindString ;  //Ҫ���ҵ��ļ�������ʹ��ͨ��������Բ��Ҷ���ļ�(eg: "*.bmp;*.txt")
*/
#define EXM_FINDFILE		(WM_OS + 1350)
/*
	����һ���µ�Ŀ¼
	wParam = 0
	lParam = 0
*/
#define EXM_NEWFOLDER		(WM_OS + 1351)
/*
	��ѯ��ǰ�Ƿ��ڲ���״̬��
	wParam = 0
	lParam = 0
	���򷵻�TRUE�����򷵻�FALSE
*/
#define EXM_ISFIND			(WM_OS + 1352)
/*
	�õ�ѡ����ļ���
	wParam = (UINT)cchMax; //����ĳ��ȣ���С��MAX_PATH
	lParam = (LPTSTR)lpFileName ; // ����ļ����Ļ���
	�ɹ�����TRUE�� ���򷵻�FALSE
*/
#define EXM_GETSELFILENAME	(WM_OS + 1353)

/*
	�õ������ִ�
	wParam = (UINT)cchMax; //����Ĵ�С
	lParam = (LPTSTR)lpFilter; // ��Ź����ִ��Ļ���
	�ɹ�����TRUE�����򷵻�FALSE
*/
#define EXM_GETFILTER		(WM_OS + 1354)

/*
	�õ������ִ�
	wParam = 0;
	lParam = (LPTSTR)lpFilter; // ��Ҫ���õ��µĹ����ִ�
	�ɹ�����TRUE�����򷵻�FALSE
*/

#define EXM_SETFILTER		(WM_OS + 1355)

typedef struct{
	TCHAR lpName[MAX_PATH];
	TCHAR lpPath[MAX_PATH];
	UINT  dwFileAttributes;     
	FILETIME ftCreationTime;	
	FILETIME ftLastAccessTime;
	DWORD dwTotalSize;   
	DWORD dwFreeSize;
}FILEPROPERTY, *LPFILEPROPERTY;

/*
	�õ���������
	wParam = 0;
	lParam = (LPFILEPROPERTY)lpFileProperty; 
	�ɹ�����TRUE�����򷵻�FALSE
*/

#define EXM_GETPROPERTY		(WM_OS + 1356)

//֪ͨ��Ϣ�Ľṹ
typedef struct _NMFILEEXPLORER
{
    NMHDR		  hdr;
	LPNMLISTVIEW  lpnmListView;
	HWND hListView;
} NMFILEEXPLORER, *LPNMFILEEXPLORER;


/*
	����ˢ����ʽ
	wParam = 0;
	lParam = 0; 
	�ɹ�����TRUE�����򷵻�FALSE
*/
#define EXM_REFRESH			(WM_OS + 1357)

/*
	�򿪵�ǰѡ����ļ���
	wParam = 0; 
	lParam = 0; 
	�ɹ�����TRUE�� ���򷵻�FALSE
*/
#define EXM_OPENCURSEL		(WM_OS + 1358)

/*
	��������ǰѡ���ļ���
	wParam = 0; 
	lParam = 0; 
	��
*/
#define EXM_RENAME		(WM_OS + 1359)

#define EXN_FIRST				 (0U-1000U)       
#define EXN_SELCHANGED		(EXN_FIRST - 0)     // ����Ŀ��ѡ��ı�ʱ��֪ͨ��Ϣ
#define EXN_DATACHANGED		(EXN_FIRST - 1)     // �����ݸı�ʱ��֪ͨ��Ϣ
#define EXN_ACTIVEITEM		(EXN_FIRST - 2)		// ���ǰ��һ����Ŀ
#define EXN_ENDRENAME		(EXN_FIRST - 3)		// ��������ǰ��һ����Ŀ���

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EXPLORER_H
