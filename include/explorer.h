/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __EXPLORER_H
#define __EXPLORER_H

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

extern const char classFileBrowser[];
extern ATOM RegisterFileBrowserClass(HINSTANCE hInstance);

#define FBS_NOSIZECOLUMN	0x0001  // 不显示文件尺寸列
#define FBS_NODATECOLUMN	0x0002  // 不显示日期列
#define FBS_NODIRCOLUMN		0x0004  // 不显示目录列
#define FBS_SHOWSUBDIR		0x0004  // 显示子目录条目


typedef BOOL (CALLBACK *FindFile)(LPTSTR lpFileName,FILE_FIND_DATA	*FindFileData);

/*
	得到列示的句柄
	wParam = 0
	lParam = 0
*/
#define EXM_GETLISTVIEW		(WM_OS + 1345)       // 得到列示控件的句柄
/*
	设置当前路径
	wParam = 0
	lParam = (LPTSTR)pCurPath ; //新得当前路径
*/
#define EXM_SETCURPATH		(WM_OS + 1346)
/*
	得到当前路径
	wParam = 0
	lParam = (LPTSTR)pCurPath ; //要放置当前路径的缓存，大小为MAX_PATH
*/
#define EXM_GETCURPATH		(WM_OS + 1347)
/*
	向上一级目录
	wParam = 0
	lParam = 0
*/
#define EXM_UPPATH			(WM_OS + 1348)
/*
	删除选择的文件
	wParam = 0
	lParam = 0
*/
#define EXM_DELETESEL		(WM_OS + 1349)
/*
	查找文件
	wParam = (FindFile)FindFileFunc; // 查找到一个文件后回调用户需不需要此文件
	lParam = (LPTSTR)lpFindString ;  //要查找的文件，可以使用通配符，可以查找多个文件(eg: "*.bmp;*.txt")
*/
#define EXM_FINDFILE		(WM_OS + 1350)
/*
	创建一个新的目录
	wParam = 0
	lParam = 0
*/
#define EXM_NEWFOLDER		(WM_OS + 1351)
/*
	查询当前是否在查找状态下
	wParam = 0
	lParam = 0
	是则返回TRUE，否则返回FALSE
*/
#define EXM_ISFIND			(WM_OS + 1352)
/*
	得到选择的文件，
	wParam = (UINT)cchMax; //缓存的长度，不小于MAX_PATH
	lParam = (LPTSTR)lpFileName ; // 存放文件名的缓存
	成功返回TRUE， 否则返回FALSE
*/
#define EXM_GETSELFILENAME	(WM_OS + 1353)

/*
	得到过滤字串
	wParam = (UINT)cchMax; //缓存的大小
	lParam = (LPTSTR)lpFilter; // 存放过滤字串的缓存
	成功返回TRUE，否则返回FALSE
*/
#define EXM_GETFILTER		(WM_OS + 1354)

/*
	得到过滤字串
	wParam = 0;
	lParam = (LPTSTR)lpFilter; // 需要设置的新的过滤字串
	成功返回TRUE，否则返回FALSE
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
	得到磁盘属性
	wParam = 0;
	lParam = (LPFILEPROPERTY)lpFileProperty; 
	成功返回TRUE，否则返回FALSE
*/

#define EXM_GETPROPERTY		(WM_OS + 1356)

//通知消息的结构
typedef struct _NMFILEEXPLORER
{
    NMHDR		  hdr;
	LPNMLISTVIEW  lpnmListView;
	HWND hListView;
} NMFILEEXPLORER, *LPNMFILEEXPLORER;


/*
	重新刷新列式
	wParam = 0;
	lParam = 0; 
	成功返回TRUE，否则返回FALSE
*/
#define EXM_REFRESH			(WM_OS + 1357)

/*
	打开当前选择的文件，
	wParam = 0; 
	lParam = 0; 
	成功返回TRUE， 否则返回FALSE
*/
#define EXM_OPENCURSEL		(WM_OS + 1358)

/*
	重命名当前选择文件，
	wParam = 0; 
	lParam = 0; 
	无
*/
#define EXM_RENAME		(WM_OS + 1359)

#define EXN_FIRST				 (0U-1000U)       
#define EXN_SELCHANGED		(EXN_FIRST - 0)     // 当条目的选择改变时的通知消息
#define EXN_DATACHANGED		(EXN_FIRST - 1)     // 当内容改变时的通知消息
#define EXN_ACTIVEITEM		(EXN_FIRST - 2)		// 激活当前的一个条目
#define EXN_ENDRENAME		(EXN_FIRST - 3)		// 重命名当前的一个条目完成

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__EXPLORER_H
