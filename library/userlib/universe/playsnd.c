/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/
/*****************************************************
文件说明：播放一个声音源，API部分
版本号：1.0.0
开发时期：2003-04-16
作者：陈建明 Jami chen
修改记录：
******************************************************/

#include <ewindows.h>
#include <emmsys.h>

/*

#define SND_SYNC       0x00000000   // 同步播放
#define SND_ASYNC      0x00000001   // 异步播放
#define SND_NODEFAULT  0x00000002   // 如果没有找到声音源，则不播放默认声音
#define SND_MEMORY     0x00000004   // 声音源指向一个内存文件
#define SND_LOOP       0x00000008   // 循环播放直到下一个声音播放
#define SND_NOSTOP     0x00000010   // 不停止当前正在播放的声音

*/
typedef struct {
	char  id[4];  	// 标志串 = "RIFF"
	DWORD len;    	// 在这个头以后的长度
}RIFFHEADER, *PRIFFHEADER;

typedef struct {
	char  id[4];  	// 标志 = "WAVE"
}WAVEID,*PWAVEID;

typedef struct {
	WORD	wFormatTag; // 格式类型， 目前支持WAVE_FORMAT_PCM
	WORD	wChannels; // 声音通道数目
	DWORD	dwSamplesPerSec; // 样本速率
	DWORD	dwAvgBytesPerSec; // 平均每秒数据数目
	WORD	wBlockAlign; // 数据块尺寸
}COMMON_FIELD, *PCOMMON_FIELD;

typedef struct {
	WORD	wBitsPerSample; // 样本大小，一般为 8 bit 或者 16 bit
}PCM_FORMAT_SPECIFIC, *PPCM_FORMAT_SPECIFIC;

typedef struct {
	char  id[4];  	// 标志串 = "fmt " 或 "data"
	DWORD len;    	// 在这个头以后的剩下的Chunk Header长度
	COMMON_FIELD common_field;  // 通用格式
	PCM_FORMAT_SPECIFIC  PCM_format_specific; // PCM 附加格式
}CHUNKHEADER, *PCHUNKHEADER;

typedef struct {
	char  id[4];  	// 标志串 = "RIFF"
	DWORD len;    	// 在这个头以后的长度
}DATAHEADER, *PDATAHEADER;


#define MAX_BUFFERNUM		4
#define MAX_BUFFERLENGTH	(1024 * 4)

static HANDLE hFile = INVALID_HANDLE_VALUE;
static WAVEHDR wh[MAX_BUFFERNUM];
//static WORD nWaveRate=44100;
//static WORD nWaveChannel=2;
//static WORD nWaveBit=16;
static RIFFHEADER g_stRiffHeader;
static WAVEID	g_stWaveID;
static CHUNKHEADER g_stChunkHeader;
static DATAHEADER g_stDataHeader;
static DWORD g_dwReadLen = 0;
static UINT g_fuSound = 0;
static HANDLE g_hThread = NULL;

static HWAVEOUT hwo = 0;
static BOOL bPlaying = FALSE;
static BOOL bStoping = FALSE;
static BOOL bStoped	 = TRUE;
static UINT uPlayingNum = 0;
static HANDLE hPlayOverEvent = NULL;

static BOOL GetSoundInfo(void );
void CALLBACK AudioProc( HWAVEOUT hwi,UINT uMsg, DWORD dwInstance,DWORD dwParam1);//, DWORD dwParam2 );
static void AddBuffer(LPWAVEHDR lpwh);
static BOOL FillBuffer(LPWAVEHDR lpwh);
static BOOL GetRiffHeader(PRIFFHEADER pRiffHeader);
static BOOL GetWaveID(PWAVEID pWaveID);
static BOOL GetChunkHeader(PCHUNKHEADER pChunkHeader);
static BOOL GetDataHeader(PDATAHEADER pDataHeader);
static BOOL OpenWav(void);
static DWORD WINAPI Audio_ASyncProc(VOID *pParam);


static void ShowErrorCode(MMRESULT result);
/*
#define SND_SYNC       0x00000000   // 同步播放
#define SND_ASYNC      0x00000001   // 异步播放
#define SND_NODEFAULT  0x00000002   // 如果没有找到声音源，则不播放默认声音
#define SND_MEMORY     0x00000004   // 声音源指向一个内存文件
#define SND_LOOP       0x00000008   // 循环播放直到下一个声音播放
#define SND_NOSTOP     0x00000010   // 不停止当前正在播放的声音

#define SND_NOWAIT     0x00002000   // 如果系统忙，则不等待
*/

// **************************************************
// 声明：BOOL WINAPI sndPlaySound( LPCTSTR lpszSoundName, UINT fuSound )
// 参数：
// 	IN lpszSoundName -- 要播放的声音源
// 	IN fuSound -- 播放O标志
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：播放一个声音文件。
// 引用: 
// **************************************************
BOOL WINAPI sndPlaySound( LPCTSTR lpszSoundName, UINT fuSound )
{
	int i;
	DWORD nFileLength = 0;

#ifdef EML_WIN32
	  return TRUE;	
#endif
//	  RETAILMSG(1,("***sndPlaySound +++ %d\r\n",bPlaying));
//	  RETAILMSG(1,("***bPlaying %d\r\n",bPlaying));
	  g_fuSound = fuSound; // 得到播放标志

	  if (bPlaying == TRUE)
	  { // 当前正在播放
//		  RETAILMSG(1,("The wave playing %x +++\r\n",fuSound));
		  if (fuSound & SND_NOSTOP)
		  { // 不要停止以前的播放
//			  RETAILMSG(1,("sndPlaySound Busy\r\n"));
			  return TRUE;
		  }

/*
		  if (hFile != INVALID_HANDLE_VALUE) 
		  {
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE; 
		  }
*/
		  bStoping = TRUE; // 停止以前的播放
//		  waveOutReset(hwo);
//		  if (lpszSoundName == NULL)
//			  return TRUE;
//		  waveOutClose(hwo);
		  while(bStoped == FALSE)
		  { // 等待完成
			  Sleep(100);
		  }
	  }
	  if (g_hThread)
	  {  // 关闭线程
//		  RETAILMSG(1,("Close Old Thread\r\n"));
		  CloseHandle(g_hThread);
		  g_hThread = NULL;
	  }
	  if (lpszSoundName == NULL)
	  {  // 没有声音要播放
		  RETAILMSG(1,("sndPlaySound No Name\r\n"));
		  return TRUE;
	  }

	  g_dwReadLen = 0;
//	  RETAILMSG(1,("Will Play a test Sound \r\n"));
	  // 打开要播放的文件
	  hFile = CreateFile(lpszSoundName, GENERIC_READ,
		    FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
		    OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_READONLY, 0);
	  if (hFile == INVALID_HANDLE_VALUE) 
	  { // 打开文件失败
			DWORD dwError = GetLastError();
			RETAILMSG(1,("Open <%s> failure %d\r\n",lpszSoundName,dwError));
			if (fuSound & SND_NODEFAULT)
				return TRUE; // 不播放默认声音
			// 播放默认声音，目前没有设置默认声音
			return TRUE;
	  }
//	  RETAILMSG(1,(("hFile = %X\r\n"),hFile));
//	  RETAILMSG(1,("Open Test audio file OK\r\n"));
	  if (GetSoundInfo() == FALSE)
	  { // 得到声音信息
		  CloseHandle(hFile);
		  return FALSE;
	  }
	  if (OpenWav() == FALSE)
	  { // 打开声音设备
		  RETAILMSG(1,("*********************************************Open Wav Failure\r\n"));
		  CloseHandle(hFile);
		  return FALSE;
	  }
//	  RETAILMSG(1,("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Open Wav OK\r\n"));
	  for (i = 0; i < MAX_BUFFERNUM; i++)
	  { // 填充数据
		if (FillBuffer(&wh[i])== FALSE)
			  break;
		AddBuffer(&wh[i]); // 播放已经填充好的数据
	  }
	  if (fuSound & SND_ASYNC)
	  { // 异步播放
			g_hThread = CreateThread(NULL, 0, Audio_ASyncProc, 0, 0, 0 ); // 创建一个播放线程
//			RETAILMSG(1,("Create hThread = %x \r\n",g_hThread));
	  }
	  else
	  {  // 同步播放
//		  while(bPlaying == TRUE)
//		  {
//			  Sleep(100);
//		  }
		  WaitForSingleObject(hPlayOverEvent,INFINITE);  // 等待播放完成
/*		  for (i=0;i<MAX_BUFFERNUM;i++)
		  {
			if (wh[i].lpData)
			{
				RETAILMSG(1,("free memory %x \r\n",wh[i].lpData));
				free(wh[i].lpData);
				wh[i].lpData = NULL;
			}
		  }
*/
	  }
//	  RETAILMSG(1,("sndPlaySound ---\r\n"));
	  return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI PlaySound( LPCTSTR pszSound, HANDLE hmod, DWORD fdwSound)
// 参数：
// 	IN pszSound -- 声音源
// 	IN hmod -- 播放的模块句柄
// 	IN fdwSound -- 播放标志
// 
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：播放一个声音源。
// 引用: 
// **************************************************
BOOL WINAPI PlaySound( LPCTSTR pszSound, HANDLE hmod, DWORD fdwSound)
{
	return TRUE;
}

// **************************************************
// 声明：void CALLBACK AudioProc( HWAVEOUT hwi, UINT uMsg,DWORD dwInstance, DWORD dwParam1)
// 参数：
// 	IN hwi -- 声音设备句柄
// 	IN uMsg -- 消息代码
// 	IN dwInstance -- 实例句柄
// 	IN dwParam1 -- 参数
// 
// 返回值：无
// 功能描述：声音数据处理函数。
// 引用: 
// **************************************************
void CALLBACK AudioProc( HWAVEOUT hwi,   
	  UINT uMsg,      
	  DWORD dwInstance,       
	  DWORD dwParam1)//, 
//	  DWORD dwParam2 )
{
//	DWORD SampleBytes;
	LPWAVEHDR lpwh;
	
//	RETAILMSG(1,(("*********************have a message form Audio (%d)\r\n"),uMsg));
	switch( uMsg )
	{
	case WOM_OPEN: // 声音设备打开
//		RETAILMSG(1,(("*********************The play is Open\r\n")));
		return;
		  
	case WOM_DONE: // 声音设备完成一个数据
//		RETAILMSG(1,(("WOM_DONE ++++ \r\n")));
		
		uPlayingNum --;
		lpwh=(LPWAVEHDR)dwParam1; // 得到完成的数据结构
//		RETAILMSG(1,(("*********************The Complete wh is %X,dwFlags=%X\r\n"),lpwh,lpwh->dwFlags));
		waveOutUnprepareHeader(hwo,lpwh,sizeof(WAVEHDR)); // 解除准备结构头
//		RETAILMSG(1,(("1	\r\n")));
//		free(lpwh->lpData);
//		RETAILMSG(1,(("2	\r\n")));
		if (FillBuffer(lpwh)== FALSE) // 填充缓存
			  break;
		AddBuffer(lpwh); // 将数据添加到播放列表
//		RETAILMSG(1,(("WOM_DONE ---- \r\n")));
		return;
		  
	case WOM_CLOSE: // 播放完成
		bPlaying = FALSE; // 不在播放中
//		 RETAILMSG(1,("***bPlaying %d\r\n",bPlaying));
		SetEvent(hPlayOverEvent); // 设置完成播放的事件
//		RETAILMSG(1,(("*********************The play is close\r\n")));
		return;
	}
}

// **************************************************
// 声明：static BOOL OpenWav(void)
// 参数：
// 	无
// 返回值：成功返回TRUE，否则返回FALSE
// 功能描述：打开声音设备。
// 引用: 
// **************************************************
static BOOL OpenWav(void)
{
	WAVEFORMATEX waveFormatEx;
	MMRESULT result;
	int i;

		bPlaying=TRUE;
//		RETAILMSG(1,("***bPlaying %d\r\n",bPlaying));
		bStoped = FALSE;
		bStoping = FALSE;
		waveFormatEx.wFormatTag = WAVE_FORMAT_PCM; // 要打开的声音格式
		waveFormatEx.nChannels = g_stChunkHeader.common_field.wChannels; //nWaveChannel;
		waveFormatEx.nSamplesPerSec = g_stChunkHeader.common_field.dwSamplesPerSec; // 每秒样本数
		waveFormatEx.wBitsPerSample = g_stChunkHeader.PCM_format_specific.wBitsPerSample; // 每个样本的BIT数
		waveFormatEx.nAvgBytesPerSec = g_stChunkHeader.common_field.dwAvgBytesPerSec; // 平均每秒要播放的BYTE数
		waveFormatEx.nBlockAlign = g_stChunkHeader.common_field.wBlockAlign; // BLOCK对齐的大小
		waveFormatEx.cbSize = 0;
//		RETAILMSG(1,("waveOutOpen ...\r\n"));
		// 打开声音设备
		result=waveOutOpen(
			  &hwo,            
			  0 ,            
			  (LPWAVEFORMATEX)&waveFormatEx,       
			  (DWORD)AudioProc,          
			  0,  
			  CALLBACK_FUNCTION);

		if (result!=MMSYSERR_NOERROR )
		{ // 打开声音设备失败
			RETAILMSG(1,("waveOutOpen Failure\r\n"));
//			MessageBox(NULL,("WaveOutOpen"),("Error"),MB_OK);
			ShowErrorCode(result);
			bPlaying=FALSE;
//		    RETAILMSG(1,("***bPlaying %d\r\n",bPlaying));
			return FALSE;
		}
		// 初始化数据结构
		for (i=0;i<MAX_BUFFERNUM;i++)
		{ 
//			wh[i].lpData=NULL;
			wh[i].dwBufferLength=0;
			wh[i].dwBytesRecorded=0;
			wh[i].lpNext=NULL;
			//InsertFreewh(&wh[i]);
		}
//		RETAILMSG(1,(("WaveOutOpen Success %X\r\n"),hwo));
		hPlayOverEvent = CreateEvent(NULL,0,0,NULL); // 创建一个播放完成事件
		return TRUE;
}
static void AddBuffer(LPWAVEHDR lpwh)
{
	MMRESULT result;

//			RETAILMSG(1,(("AddBuffer ...\r\n")));
			if (lpwh->dwBufferLength == 0)
			{
//				RETAILMSG(1,(("the buffer size == 0\r\n")));
				return ;
			}
			lpwh->dwBytesRecorded=0;
			lpwh->lpNext=NULL;
			
//			RETAILMSG(1,(("waveOutPrepareHeader ...\r\n")));
			result=waveOutPrepareHeader(hwo,lpwh,sizeof(WAVEHDR));
			if (result!=MMSYSERR_NOERROR )
			{
				ShowErrorCode(result);
//				RETAILMSG(1,("waveOutPrepareHeader Failure %d\r\n",result));
				return ;
			}
			
//			RETAILMSG(1,(("waveOutWrite ...\r\n")));
			result=waveOutWrite(hwo,lpwh,sizeof(WAVEHDR));
			if (result!=MMSYSERR_NOERROR )
			{
//				RETAILMSG(1,("waveOutWrite Failure %d\r\n",result));
				ShowErrorCode(result);
				return ;
			}
			uPlayingNum ++;
//			RETAILMSG(1,(("AddBuffer Success\r\n")));
}

static BOOL FillBuffer(LPWAVEHDR lpwh)
{
	LPTSTR lpBuffer;
	int dwReadLen;

//		RETAILMSG(1,(("FillBuffer ... lpwh = %X\r\n"),lpwh));
		if (hFile == INVALID_HANDLE_VALUE) 
		{
			if (uPlayingNum == 0)
			{
				waveOutClose(hwo);
				hwo = 0;
			}

//			RETAILMSG(1,(("The File Had not Open\r\n")));
			return FALSE;
		}
		if (bStoping == TRUE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE; 
			if (uPlayingNum == 0)
			{
				waveOutClose(hwo);
				hwo = 0;
			}
			return FALSE;
		}

		if (lpwh->lpData == NULL)
		{
			lpBuffer = (LPTSTR)malloc(MAX_BUFFERLENGTH);
			if (lpBuffer == NULL)
			{
//				RETAILMSG(1,(("Malloc Failure\r\n")));
				return FALSE;
			}
			lpwh->lpData = lpBuffer;
//			RETAILMSG(1,("malloc memory %x \r\n",lpwh->lpData));
		}
		else
		{
			lpBuffer = lpwh->lpData;
		}
READAGAIN:

		dwReadLen = g_stDataHeader.len - g_dwReadLen;
		if (dwReadLen > MAX_BUFFERLENGTH)
			dwReadLen = MAX_BUFFERLENGTH;
//		RETAILMSG(1,(("Read Data From File ...\r\n")));
//		RETAILMSG(1,(("hFile = %X\r\n"),hFile));
//		RETAILMSG(1,(("lpBuffer = %X\r\n"),lpBuffer));
		if (ReadFile(hFile,lpBuffer,dwReadLen,&dwReadLen,NULL) == FALSE)
		{
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE; 
			if (uPlayingNum == 0)
			{
				waveOutClose(hwo);
				hwo = 0;
			}
			return FALSE;
		}
		if (dwReadLen == 0)
		{
//			RETAILMSG(1,(("Read File To End , and Close The File\r\n")));
			if (g_fuSound & SND_LOOP)
			{
				g_dwReadLen = 0;
				SetFilePointer(hFile,0,0,FILE_BEGIN);
				goto READAGAIN;
			}
//			free(lpBuffer);
			CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE; 
			if (uPlayingNum == 0)
			{
				waveOutClose(hwo);
				hwo = 0;
			}
			return FALSE;
		}
//		RETAILMSG(1,(("Read Data From File OK !!!\r\n")));
		lpwh->dwBufferLength = dwReadLen;
		g_dwReadLen += dwReadLen;
		//lpwh->dwFlags
//		RETAILMSG(1,(("FillBuffer Success\r\n")));
		return TRUE;
}
static BOOL GetSoundInfo(void )
{
//	DWORD dwFileSize;
//	RIFFHEADER stRiffHeader;
///	WAVEID	stWaveID;
//	CHUNKHEADER stChunkHeader;
		
		if (GetRiffHeader(&g_stRiffHeader) == FALSE)
			return FALSE;
		if (GetWaveID(&g_stWaveID) == FALSE)
			return FALSE;
		if (GetChunkHeader(&g_stChunkHeader) == FALSE)
			return FALSE;
		if (GetDataHeader(&g_stDataHeader) == FALSE)
			return FALSE;

		return TRUE;		
}
static BOOL GetRiffHeader(PRIFFHEADER pRiffHeader)
{
	DWORD dwReadLen;

		ReadFile(hFile,pRiffHeader->id,4,&dwReadLen,NULL);
		if (dwReadLen != 4)
		{
			return FALSE;
		}
		if (strnicmp(pRiffHeader->id,"RIFF",4) != 0)
			return FALSE;

		ReadFile(hFile,&pRiffHeader->len,4,&dwReadLen,NULL);
		return TRUE;
}
static BOOL GetWaveID(PWAVEID pWaveID)
{
	DWORD dwReadLen;

		ReadFile(hFile,pWaveID->id,4,&dwReadLen,NULL);
		if (dwReadLen != 4)
		{
			return FALSE;
		}
		if (strnicmp(pWaveID->id,"WAVE",4) != 0)
			return FALSE;
		return TRUE;
}

static BOOL GetChunkHeader(PCHUNKHEADER pChunkHeader)
{
	DWORD dwReadLen;
	char *pRemainHeader;

		ReadFile(hFile,pChunkHeader->id,4,&dwReadLen,NULL);
		if (dwReadLen != 4)
		{
			return FALSE;
		}
		if (strnicmp(pChunkHeader->id,"fmt ",4) != 0)
		{
			if (strnicmp(pChunkHeader->id,"data",4) != 0)
				return FALSE;
		}

		ReadFile(hFile,&pChunkHeader->len,4,&dwReadLen,NULL);
		if (pChunkHeader->len == 0)
			return FALSE;

		pRemainHeader = (char *)malloc(pChunkHeader->len);
		if (pRemainHeader == NULL)
			return FALSE;

		ReadFile(hFile,pRemainHeader,pChunkHeader->len,&dwReadLen,NULL);
		if (pChunkHeader->len != dwReadLen)
			return FALSE;
		
		pChunkHeader->common_field.wFormatTag = MAKEWORD(pRemainHeader[0],pRemainHeader[1]);
		if (pChunkHeader->common_field.wFormatTag != WAVE_FORMAT_PCM)
		{
			free(pRemainHeader);
			return FALSE;
		}
		pChunkHeader->common_field.wChannels = MAKEWORD(pRemainHeader[2],pRemainHeader[3]);
		pChunkHeader->common_field.dwSamplesPerSec = *(DWORD *)&pRemainHeader[4];
		pChunkHeader->common_field.dwAvgBytesPerSec = *(DWORD *)&pRemainHeader[8];
		pChunkHeader->common_field.wBlockAlign = MAKEWORD(pRemainHeader[12],pRemainHeader[13]);
		pChunkHeader->PCM_format_specific.wBitsPerSample = MAKEWORD(pRemainHeader[14],pRemainHeader[15]);
		free(pRemainHeader);
		return TRUE;
}

static BOOL GetDataHeader(PDATAHEADER pDataHeader)
{
	DWORD dwReadLen;

NextID:
		ReadFile(hFile,pDataHeader->id,4,&dwReadLen,NULL);
		if (dwReadLen != 4)
		{
			return FALSE;
		}
		if (strnicmp(pDataHeader->id,"data",4) != 0)
		{
			// Have other chunk
			DWORD len, i;
			BYTE bData;
		
				ReadFile(hFile,&len,4,&dwReadLen,NULL);
				for (i = 0; i< len ; i++)
					ReadFile(hFile,&bData,1,&dwReadLen,NULL);
				goto NextID;
		}

		ReadFile(hFile,&pDataHeader->len,4,&dwReadLen,NULL);
		return TRUE;
}

// ********************************************************************
// 声明：DWORD WINAPI Audio_ASyncProc(VOID * pParam)
// 返回值：
//		成功返回TRUE,失败返回FALSE
// 功能描述：sndPlaySound处理异步操作时等待播放完成时的进程
// 引用: 
// ********************************************************************
static DWORD WINAPI Audio_ASyncProc(VOID * pParam)
{
//	int i;

//		while(bPlaying == TRUE)
		WaitForSingleObject(hPlayOverEvent,INFINITE);
/*		for (i=0;i<MAX_BUFFERNUM;i++)
		{
			if (wh[i].lpData)
			{
				RETAILMSG(1,("free memory %x \r\n",wh[i].lpData));
				free(wh[i].lpData);
				wh[i].lpData = NULL;
			}
		}
		*/
//		RETAILMSG(1,("Exit hThread \r\n"));
		bStoped = TRUE;
		return TRUE;
}

static void ShowErrorCode(MMRESULT result)
{
	switch(result)
	{
	case MMSYSERR_INVALHANDLE:
//		MessageBox(NULL,TEXT("Specified device handle is invalid. "),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("Specified device handle is invalid. \r\n")));
		break;
	case MMSYSERR_NODRIVER:
//		MessageBox(NULL,TEXT("No device driver is present. "),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("No device driver is present. \r\n")));
		break;
	case MMSYSERR_NOMEM:
//		MessageBox(NULL,TEXT("Unable to allocate or lock memory. "),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("Unable to allocate or lock memory. \r\n")));
		break;
	case MMSYSERR_NOTSUPPORTED:
//		MessageBox(NULL,TEXT("Specified device is synchronous and does not support pausing."),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("Specified device is synchronous and does not support pausing.\r\n")));
		break;
	case MMSYSERR_ALLOCATED: 
//		MessageBox(NULL,TEXT("Specified resource is already allocated. "),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("Specified resource is already allocated. \r\n")));
		break;
	case MMSYSERR_BADDEVICEID:
//		MessageBox(NULL,TEXT(" Specified device identifier is out of range. "),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT(" Specified device identifier is out of range. \r\n")));
		break;
	case WAVERR_BADFORMAT:
//		MessageBox(NULL,TEXT(" Attempted to open with an unsupported waveform-audio format. "),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT(" Attempted to open with an unsupported waveform-audio format. \r\n")));
		break;
	case WAVERR_SYNC :
//		MessageBox(NULL,TEXT("The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag."),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag.\r\n")));
		break;
	default:
//		MessageBox(NULL,TEXT("No Name Error."),TEXT("Error"),MB_OK);
		RETAILMSG(1,(TEXT("No Name Error.(%d)\r\n"),result));
		break;
	}
			return;
}
