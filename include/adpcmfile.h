/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
***************************************************/

#ifndef _ADPCMFILE_H_
#define _ADPCMFILE_H_

#include <ewindows.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  

// **************************************************
// 声明：BOOL WINAPI PlayAdpcmFile( LPCTSTR lpszAdpcmName, UINT fuSound )
// 参数：
// 	IN lpszSoundName -- 要播放的声音源
// 	IN fuSound -- 播放标志
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：播放一个ADPCM格式的声音文件。
// 引用: 
// **************************************************
BOOL WINAPI PlayAdpcmFile( LPCTSTR lpszAdpcmName, UINT fuSound );

// **************************************************
// 声明：void Adpcm_StopPlay(void)
// 参数：
// 
// 返回值：无
// 功能描述：停止播放。
// 引用: 
// **************************************************
void Adpcm_StopPlay(void);


typedef struct tagADPCM_FILE_FORMAT{
	UINT nChannels;
	UINT nSamplesPerSec;
	WORD wBitsPerSample;
}ADPCM_FILE_FORMAT, *LPADPCM_FILE_FORMAT;
// **************************************************
// 声明：BOOL WINAPI RecordAdpcmFile( LPCTSTR lpszAdpcmName, ADPCM_FILE_FORMAT Adpcm_File_Format )
// 参数：
// 	IN lpszSoundName -- 要播放的声音源
// 	IN Adpcm_File_Format -- 要录制的声音格式
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：录制一个ADPCM格式的声音文件。
// 引用: 
// **************************************************
BOOL WINAPI RecordAdpcmFile( LPCTSTR lpszAdpcmName, ADPCM_FILE_FORMAT Adpcm_File_Format);

// **************************************************
// 声明：void Adpcm_StopRecord(void)
// 参数：
// 
// 返回值：无
// 功能描述：停止录音。
// 引用: 
// **************************************************
void Adpcm_StopRecord(void);


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_ADPCMFILE_H_
