/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef _VAR_BUFFER_H_
#define _VAR_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// 数据处理 定义
typedef	struct	_DATA_DEAL
{
	DWORD		dwFlag;			//
	DWORD		dwLenGrow;		// 每次增长的长度
	DWORD		dwLenMaxAlloc;	// 最大分配的数据的长度

	DWORD		dwLenTotal;		// 数据的长度
	DWORD		dwLenDealed;	// 已经处理了的数据长度
	LPBYTE		lpData;			// 数据缓存

} DATA_DEAL, *LPDATA_DEAL;

//wFlag of DATA_WRITE
#define	DDF_GROW				0x8000	//表示 可以自动分配，自动增长lpData的长度
#define	DDF_MAXALLOC			0x4000	//表示 分配lpData新长度时，有最大长度的限制
#define	DDF_ZEROALLOC			0x2000	//表示 分配lpData新长度时，初始化清0
#define	DDF_NOWRITE				0x1000	//表示 不可以向lpData中，写入数据---暂时不支持
#define	DDGROW_DEFAULT			40

//
extern	BOOL	VarBuf_Alloc( DATA_DEAL* lpDealData, DWORD dwFlag, DWORD dwLenGrow, DWORD dwLenMaxAlloc, DWORD dwLenTotal );
extern	BOOL	VarBuf_Grow( DATA_DEAL* lpDealData, DWORD dwLenGrow );
extern	void	VarBuf_Free( DATA_DEAL* lpDealData );
extern	BOOL	VarBuf_ReadFile( LPCTSTR pszFileName, OUT DATA_DEAL* lpDealDataRecv );

//
extern	BOOL	VarDec_Byte( DATA_DEAL* lpDealData, OUT BYTE* pbRead );
extern	BOOL	VarPeek_Byte( DATA_DEAL* lpDealData, OUT BYTE* pbRead );
extern	BOOL	VarEnc_Byte( DATA_DEAL* lpDealData, BYTE bWrite );
extern	BOOL	VarEnc_Buffer( DATA_DEAL* lpDealData, LPBYTE pBufWrite, DWORD dwLenWrite );

	
#ifdef __cplusplus
}
#endif  // __cplusplus

#endif //_VAR_BUFFER_H_
