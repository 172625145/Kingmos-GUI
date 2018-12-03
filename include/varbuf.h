/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef _VAR_BUFFER_H_
#define _VAR_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


// ���ݴ��� ����
typedef	struct	_DATA_DEAL
{
	DWORD		dwFlag;			//
	DWORD		dwLenGrow;		// ÿ�������ĳ���
	DWORD		dwLenMaxAlloc;	// ����������ݵĳ���

	DWORD		dwLenTotal;		// ���ݵĳ���
	DWORD		dwLenDealed;	// �Ѿ������˵����ݳ���
	LPBYTE		lpData;			// ���ݻ���

} DATA_DEAL, *LPDATA_DEAL;

//wFlag of DATA_WRITE
#define	DDF_GROW				0x8000	//��ʾ �����Զ����䣬�Զ�����lpData�ĳ���
#define	DDF_MAXALLOC			0x4000	//��ʾ ����lpData�³���ʱ������󳤶ȵ�����
#define	DDF_ZEROALLOC			0x2000	//��ʾ ����lpData�³���ʱ����ʼ����0
#define	DDF_NOWRITE				0x1000	//��ʾ ��������lpData�У�д������---��ʱ��֧��
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
