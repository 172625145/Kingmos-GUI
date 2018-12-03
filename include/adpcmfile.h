/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

#ifndef _ADPCMFILE_H_
#define _ADPCMFILE_H_

#include <ewindows.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  

// **************************************************
// ������BOOL WINAPI PlayAdpcmFile( LPCTSTR lpszAdpcmName, UINT fuSound )
// ������
// 	IN lpszSoundName -- Ҫ���ŵ�����Դ
// 	IN fuSound -- ���ű�־
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ��������������һ��ADPCM��ʽ�������ļ���
// ����: 
// **************************************************
BOOL WINAPI PlayAdpcmFile( LPCTSTR lpszAdpcmName, UINT fuSound );

// **************************************************
// ������void Adpcm_StopPlay(void)
// ������
// 
// ����ֵ����
// ����������ֹͣ���š�
// ����: 
// **************************************************
void Adpcm_StopPlay(void);


typedef struct tagADPCM_FILE_FORMAT{
	UINT nChannels;
	UINT nSamplesPerSec;
	WORD wBitsPerSample;
}ADPCM_FILE_FORMAT, *LPADPCM_FILE_FORMAT;
// **************************************************
// ������BOOL WINAPI RecordAdpcmFile( LPCTSTR lpszAdpcmName, ADPCM_FILE_FORMAT Adpcm_File_Format )
// ������
// 	IN lpszSoundName -- Ҫ���ŵ�����Դ
// 	IN Adpcm_File_Format -- Ҫ¼�Ƶ�������ʽ
// 
// ����ֵ���ɹ�����TRUE�����򷵻�FALSE
// ����������¼��һ��ADPCM��ʽ�������ļ���
// ����: 
// **************************************************
BOOL WINAPI RecordAdpcmFile( LPCTSTR lpszAdpcmName, ADPCM_FILE_FORMAT Adpcm_File_Format);

// **************************************************
// ������void Adpcm_StopRecord(void)
// ������
// 
// ����ֵ����
// ����������ֹͣ¼����
// ����: 
// **************************************************
void Adpcm_StopRecord(void);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_ADPCMFILE_H_
