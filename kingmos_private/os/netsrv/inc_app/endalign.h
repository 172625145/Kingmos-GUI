/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _END_ALIGN_H_
#define _END_ALIGN_H_

#ifdef __cplusplus
extern "C" {
#endif

// --------------------------------------------------------------------------------
// ���壺�ֽ�˳��Ĳ���---Byte0(MSB), Byte1~4(LSB)
// --------------------------------------------------------------------------------
#undef	BIG_END		//�������Ǵ�˶����

#ifdef	BIG_END
// -------------------------------------------------------------------
// ��˶���� ת������---             ��Ӧ
//   ����0xC021(�ʹ�0xC0, �ߴ�0x21) <------> ����0xC021(��0xC0, ��0x21)
//   BYTE(0x21)--->WORD(0x0021)--->DWORD(0x00000021)----���������ȷ��
//   BYTE(0x21)--->WORD(0x2100)--->DWORD(0x21000000)----������Ǵ����
// -------------------------------------------------------------------

//�����洢˳��-->�������ֽڴ洢˳��
#define	REORDER_S(S)			( (WORD)(S) )
#define	REORDER_L(L)			( (DWORD)(L) )

//�������洢���У���ȡ---�������ֽ�˳��� ָ���ֽڡ�
#define	GET_HBYTE0_S(S)			( (BYTE)( S ) )			//�������洢���У���ȡ---MSB: 0xC000--->0xC0
#define	GET_HBYTE1_S(S)			( (BYTE)( S<<8 ) )		//�������洢���У���ȡ---LSB: 0x0021===>0x2100--->0x21

#define	GET_HBYTE0_L(L)			( (BYTE)( L ) )			//�������洢���У���ȡ---MSB
#define	GET_HBYTE1_L(L)			( (BYTE)( L<<8 ) )
#define	GET_HBYTE2_L(L)			( (BYTE)( L<<16 ) )
#define	GET_HBYTE3_L(L)			( (BYTE)( L<<24 ) )		//�������洢���У���ȡ---LSB

//�����ֽ�-->�������ֽ�˳��� ָ���ֽڡ�λ���ϣ�
#define	TO_NBYTE0_S(b0)			( ((WORD)b0)<<8 )		//0x21--->0x0021===>0x2100
#define	TO_NBYTE1_S(b1)			( ((WORD)b1) )			//0x21--->0x0021

#else	//BIG_END
// -------------------------------------------------------------------
// С�˶���� ת������---             ��Ӧ
//   ����0xC021(�ʹ�0x21, �ߴ�0xC0) <------> ����0xC021(��0xC0, ��0x21)
//   BYTE(0x21)--->WORD(0x0021)--->DWORD(0x00000021)
// -------------------------------------------------------------------

//�����洢˳��-->�������ֽڴ洢˳��
#define	REORDER_S(S)			( (WORD)(((S<<8) & 0xFF00) | ((S>>8) & 0x00FF)) )
#define	REORDER_L(L)			( (DWORD)(((L>>24)&0x000000FF) | ((L>>8)&0x0000FF00) | ((L<<24)&0xFF000000) | ((L<<8)&0x00FF0000)) )

//�������洢���У���ȡ---�������ֽ�˳��� ָ���ֽڡ�
#define	GET_HBYTE0_S(S)			( (BYTE)( S>>8 ) )		//MSB: 0xC000===>0x00C0--->0xC0
#define	GET_HBYTE1_S(S)			( (BYTE)( S ) )			//LSB: 0x0021--->0x21

#define	GET_HBYTE0_L(L)			( (BYTE)( L>>24 ) )		//MSB
#define	GET_HBYTE1_L(L)			( (BYTE)( L>>16 ) )
#define	GET_HBYTE2_L(L)			( (BYTE)( L>>8 ) )
#define	GET_HBYTE3_L(L)			( (BYTE)( L ) )			//LSB

//�����ֽ�-->�������ֽ�˳��� ָ���ֽڡ�λ���ϣ�
#define	TO_NBYTE0_S(b0)			( ((WORD)b0) )			//0x21--->0x0021
#define	TO_NBYTE1_S(b1)			( ((WORD)b1)<<8 )		//0xC0--->0x00C0===>0xC000

#endif	//BIG_END

//ǿ�ƻ�˳��
#define	REORDER_S_OK(S)			( (WORD)(((S<<8) & 0xFF00) | ((S>>8) & 0x00FF)) )
#define	REORDER_L_OK(L)			( (DWORD)(((L>>24)&0x000000FF) | ((L>>8)&0x0000FF00) | ((L<<24)&0xFF000000) | ((L<<8)&0x00FF0000)) )

//�ӡ������ֽڴ洢˳�����У���ȡָ���ֽ�
#define	GET_NBYTE0_S(S)			( (BYTE)( S ) )			//MSB
#define	GET_NBYTE1_S(S)			( (BYTE)( S>>8 ) )		//LSB

#define	GET_NBYTE0_L(L)			( (BYTE)( L ) )			//MSB
#define	GET_NBYTE1_L(L)			( (BYTE)( L>>8 ) )
#define	GET_NBYTE2_L(L)			( (BYTE)( L>>16 ) )
#define	GET_NBYTE3_L(L)			( (BYTE)( L>>24 ) )		//LSB


//������-->������˳��������洢 ����
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
