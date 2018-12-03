/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _END_ALIGN_H_
#define _END_ALIGN_H_

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
// 定义：字节顺序的操作---Byte0(MSB), Byte1~4(LSB)
// --------------------------------------------------------------------------------
#undef	BIG_END		//本机不是大端对齐的

#ifdef	BIG_END
// -------------------------------------------------------------------
// 大端对齐的 转换操作---             对应
//   主机0xC021(低存0xC0, 高存0x21) <------> 网络0xC021(先0xC0, 后0x21)
//   BYTE(0x21)--->WORD(0x0021)--->DWORD(0x00000021)----如果这是正确的
//   BYTE(0x21)--->WORD(0x2100)--->DWORD(0x21000000)----如果这是错误的
// -------------------------------------------------------------------

//主机存储顺序-->“网络字节存储顺序”
#define	REORDER_S(S)			( (WORD)(S) )
#define	REORDER_L(L)			( (DWORD)(L) )

//从主机存储字中，获取---“网络字节顺序的 指定字节”
#define	GET_HBYTE0_S(S)			( (BYTE)( S ) )			//从主机存储字中，获取---MSB: 0xC000--->0xC0
#define	GET_HBYTE1_S(S)			( (BYTE)( S<<8 ) )		//从主机存储字中，获取---LSB: 0x0021===>0x2100--->0x21

#define	GET_HBYTE0_L(L)			( (BYTE)( L ) )			//从主机存储字中，获取---MSB
#define	GET_HBYTE1_L(L)			( (BYTE)( L<<8 ) )
#define	GET_HBYTE2_L(L)			( (BYTE)( L<<16 ) )
#define	GET_HBYTE3_L(L)			( (BYTE)( L<<24 ) )		//从主机存储字中，获取---LSB

//单个字节-->“网络字节顺序的 指定字节”位置上！
#define	TO_NBYTE0_S(b0)			( ((WORD)b0)<<8 )		//0x21--->0x0021===>0x2100
#define	TO_NBYTE1_S(b1)			( ((WORD)b1) )			//0x21--->0x0021

#else	//BIG_END
// -------------------------------------------------------------------
// 小端对齐的 转换操作---             对应
//   主机0xC021(低存0x21, 高存0xC0) <------> 网络0xC021(先0xC0, 后0x21)
//   BYTE(0x21)--->WORD(0x0021)--->DWORD(0x00000021)
// -------------------------------------------------------------------

//主机存储顺序-->“网络字节存储顺序”
#define	REORDER_S(S)			( (WORD)(((S<<8) & 0xFF00) | ((S>>8) & 0x00FF)) )
#define	REORDER_L(L)			( (DWORD)(((L>>24)&0x000000FF) | ((L>>8)&0x0000FF00) | ((L<<24)&0xFF000000) | ((L<<8)&0x00FF0000)) )

//从主机存储字中，获取---“网络字节顺序的 指定字节”
#define	GET_HBYTE0_S(S)			( (BYTE)( S>>8 ) )		//MSB: 0xC000===>0x00C0--->0xC0
#define	GET_HBYTE1_S(S)			( (BYTE)( S ) )			//LSB: 0x0021--->0x21

#define	GET_HBYTE0_L(L)			( (BYTE)( L>>24 ) )		//MSB
#define	GET_HBYTE1_L(L)			( (BYTE)( L>>16 ) )
#define	GET_HBYTE2_L(L)			( (BYTE)( L>>8 ) )
#define	GET_HBYTE3_L(L)			( (BYTE)( L ) )			//LSB

//单个字节-->“网络字节顺序的 指定字节”位置上！
#define	TO_NBYTE0_S(b0)			( ((WORD)b0) )			//0x21--->0x0021
#define	TO_NBYTE1_S(b1)			( ((WORD)b1)<<8 )		//0xC0--->0x00C0===>0xC000

#endif	//BIG_END

//强制换顺序
#define	REORDER_S_OK(S)			( (WORD)(((S<<8) & 0xFF00) | ((S>>8) & 0x00FF)) )
#define	REORDER_L_OK(L)			( (DWORD)(((L>>24)&0x000000FF) | ((L>>8)&0x0000FF00) | ((L<<24)&0xFF000000) | ((L<<8)&0x00FF0000)) )

//从“网络字节存储顺序”字中，获取指定字节
#define	GET_NBYTE0_S(S)			( (BYTE)( S ) )			//MSB
#define	GET_NBYTE1_S(S)			( (BYTE)( S>>8 ) )		//LSB

#define	GET_NBYTE0_L(L)			( (BYTE)( L ) )			//MSB
#define	GET_NBYTE1_L(L)			( (BYTE)( L>>8 ) )
#define	GET_NBYTE2_L(L)			( (BYTE)( L>>16 ) )
#define	GET_NBYTE3_L(L)			( (BYTE)( L>>24 ) )		//LSB


//主机字-->网络字顺序的主机存储 操作
#define	htons_m( S )			REORDER_S(S)
#define	htonl_m( L )			REORDER_L(L)
#define	ntohs_m( S )			REORDER_S(S)
#define	ntohl_m( L )			REORDER_L(L)

extern	WORD	htons( WORD wWord );
extern	WORD	ntohs( WORD wWord );
extern	DWORD	htonl( DWORD dwWord );
extern	DWORD	ntohl( DWORD dwWord );
extern	WORD	phtons( LPBYTE pBuf );
extern	WORD	pntohs( LPBYTE pBuf );
extern	DWORD	phtonl( LPBYTE pBuf );
extern	DWORD	pntohl( LPBYTE pBuf );

extern	void	phtonsp( OUT LPBYTE pBufN, LPBYTE pBufH );
extern	void	pntohsp( LPBYTE pBufN, OUT LPBYTE pBufH );
extern	void	phtonlp( OUT LPBYTE pBufN, LPBYTE pBufH );
extern	void	pntohlp( LPBYTE pBufN, OUT LPBYTE pBufH );


#ifdef __cplusplus
}	
#endif

#endif	//_END_ALIGN_H_
