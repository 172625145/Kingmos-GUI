/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _RW_BUF_H_
#define _RW_BUF_H_

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------
//可读写的循环操作的BUFFER 结构
//-------------------------------------------------------

//
#define	RWF_W_ENOUGH	0x01

//                                                dwRcv_nxt
//  |-----------|--------------------------------------|------------|
//  | empty     | has received but no read             |    empty   |
//  |-----------|--------------------------------------|------------|
//              |-------------- dwCntRx ---------------|
//  0      dwReadRx                                dwWriteRx     (dwLenRx-1)


typedef	struct	_RWBUFFER
{
	CRITICAL_SECTION	csBufRW;
	LPBYTE				pBufRW;
	DWORD				dwLenRW;

	DWORD				dwReadRW;
	DWORD				dwWriteRW;
	DWORD				dwCntRW;

} RWBUFFER;

typedef	struct	_RWBUFUNIT
{
	LPBYTE				pBufRW;
	DWORD				dwLenRW;

	DWORD				dwReadRW;
	DWORD				dwWriteRW;
	DWORD				dwCntRW;

} RWBUF2;


//-------------------------------------------------------
//可读写的循环操作的BUFFER 函数
//-------------------------------------------------------

extern	BOOL	RWBuf_Alloc( RWBUFFER* lpRWBuf, DWORD nLenNew, DWORD nLenMin );
extern	void	RWBuf_Free( RWBUFFER* lpRWBuf );

extern	BOOL	RWBuf_WriteData( RWBUFFER* lpRWBuf, LPBYTE pData, IN OUT DWORD* pdwLenData, DWORD dwFlagWrite );
extern	BOOL	RWBuf_ReadData( RWBUFFER* lpRWBuf, LPBYTE pData, IN OUT DWORD* pdwLenData );

extern	DWORD	RWBuf_GetCntLeft( RWBUFFER* lpRWBuf, BOOL fUseCri );
extern	BOOL	RWBuf_SetReadOffset( RWBUFFER* lpRWBuf, DWORD dwReadOffset, BOOL fUseCri );
extern	BOOL	RWBuf_GetReadBuf( RWBUFFER* lpRWBuf, DWORD dwReadOffset, LPBYTE* ppBufRead, DWORD* pdwLenRead );


//---------------------------

extern	BOOL	RWBuf2_Alloc( RWBUF2* lpRWBuf2, DWORD nLenNew, DWORD nLenMin );
extern	void	RWBuf2_Free( RWBUF2* lpRWBuf2 );

extern	BOOL	RWBuf2_WriteData( RWBUF2* lpRWBuf2, LPBYTE pData, IN OUT DWORD* pdwLenData, DWORD dwFlagWrite );
extern	BOOL	RWBuf2_ReadData( RWBUF2* lpRWBuf2, LPBYTE pData, IN OUT DWORD* pdwLenData );


#ifdef __cplusplus
}	
#endif

#endif	//_RW_BUF_H_
