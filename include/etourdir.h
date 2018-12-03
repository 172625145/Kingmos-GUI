/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵��:��������Ŀ¼
�汾�ţ�1.0.0
����ʱ�ڣ�2000
���ߣ�tanphy
�޸ļ�¼��
******************************************************/
#ifndef __TOURDIR_H
#define __TOURDIR_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef BOOL (*TourDirTreeCallBack)(PVOID pParam, LPSTR pszFilePath, FILE_FIND_DATA *pFdata);

#define TOUR_DIR_ONLY (1 << 0)
#define TOUR_FILE_ONLY (1 << 1)
#define TOUR_ALL (1 << 2)
#define TOUR_TBT_RECUR (1 << 4)
#define TOUR_HIR_RECUR (1 << 5)

BOOL TourDirTree(LPSTR pszPathFile, DWORD dwFlag, TourDirTreeCallBack pCallBack, LPVOID pParam);
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif