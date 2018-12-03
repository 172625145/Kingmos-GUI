/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����socket �������� 
�汾�ţ�  2.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��

******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <endalign.h>


/***************  ȫ���� ���壬 ���� *****************/

/******************************************************/


// ********************************************************************
// ������
// ������
//	IN wWord-
// ����ֵ��
//	����
// ����������������-->������˳��������洢 ����
// ����: 
// ********************************************************************
WORD	htons( WORD wWord )
{
	return REORDER_S(wWord);
}
// ********************************************************************
// ������
// ������
//	IN wWord-
// ����ֵ��
//	����
// ����������������˳��������洢-->������ ����
// ����: 
// ********************************************************************
WORD	ntohs( WORD wWord )
{
	return REORDER_S(wWord);
}
// ********************************************************************
// ������
// ������
//	IN dwWord-
// ����ֵ��
//	����
// ����������������-->������˳��������洢 ����
// ����: 
// ********************************************************************
DWORD	htonl( DWORD dwWord )
{
	return REORDER_L(dwWord);
}
// ********************************************************************
// ������
// ������
//	IN dwWord-
// ����ֵ��
//	����
// ����������������˳��������洢-->������ ����
// ����: 
// ********************************************************************
DWORD	ntohl( DWORD dwWord )
{
	return REORDER_L(dwWord);
}
// ********************************************************************
// ������
// ������
//	IN pBuf-
// ����ֵ��
//	����
// ����������������-->������˳��������洢 ����
// ����: 
// ********************************************************************
WORD	phtons( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	WORD	wWord;
	
	pTmp = (LPBYTE)&wWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
#else
	pTmp[1] = pBuf[0];
	pTmp[0] = pBuf[1];
#endif
	return wWord;
}
// ********************************************************************
// ������
// ������
//	IN pBuf-
// ����ֵ��
//	����
// ����������������˳��������洢-->������ ����
// ����: 
// ********************************************************************
WORD	pntohs( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	WORD	wWord;
	
	pTmp = (LPBYTE)&wWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
#else
	pTmp[1] = pBuf[0];
	pTmp[0] = pBuf[1];
#endif
	return wWord;
}
// ********************************************************************
// ������
// ������
//	IN pBuf-
// ����ֵ��
//	����
// ����������������-->������˳��������洢 ����
// ����: 
// ********************************************************************
DWORD	phtonl( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	DWORD	dwWord;
	
	pTmp = (LPBYTE)&dwWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
	pTmp[2] = pBuf[2];
	pTmp[3] = pBuf[3];
#else
	pTmp[3] = pBuf[0];
	pTmp[2] = pBuf[1];
	pTmp[1] = pBuf[2];
	pTmp[0] = pBuf[3];
#endif
	return dwWord;
}
// ********************************************************************
// ������
// ������
//	IN pBuf-
// ����ֵ��
//	����
// ����������������˳��������洢-->������ ����
// ����: 
// ********************************************************************
DWORD	pntohl( LPBYTE pBuf )
{
	LPBYTE	pTmp;
	DWORD	dwWord;
	
	pTmp = (LPBYTE)&dwWord;
#ifdef	BIG_END
	pTmp[0] = pBuf[0];
	pTmp[1] = pBuf[1];
	pTmp[2] = pBuf[2];
	pTmp[3] = pBuf[3];
#else
	pTmp[3] = pBuf[0];
	pTmp[2] = pBuf[1];
	pTmp[1] = pBuf[2];
	pTmp[0] = pBuf[3];
#endif
	return dwWord;
}

void	phtonsp( OUT LPBYTE pBufN, LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufN[0] = pBufH[0];
	pBufN[1] = pBufH[1];
#else
	pBufN[1] = pBufH[0];
	pBufN[0] = pBufH[1];
#endif
}
void	pntohsp( LPBYTE pBufN, OUT LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufH[0] = pBufN[0];
	pBufH[1] = pBufN[1];
#else
	pBufH[1] = pBufN[0];
	pBufH[0] = pBufN[1];
#endif
}
void	phtonlp( OUT LPBYTE pBufN, LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufN[0] = pBufH[0];
	pBufN[1] = pBufH[1];
	pBufN[2] = pBufH[2];
	pBufN[3] = pBufH[3];
#else
	pBufN[3] = pBufH[0];
	pBufN[2] = pBufH[1];
	pBufN[1] = pBufH[2];
	pBufN[0] = pBufH[3];
#endif
}
void	pntohlp( LPBYTE pBufN, OUT LPBYTE pBufH )
{
#ifdef	BIG_END
	pBufH[0] = pBufN[0];
	pBufH[1] = pBufN[1];
	pBufH[2] = pBufN[2];
	pBufH[3] = pBufN[3];
#else
	pBufH[3] = pBufN[0];
	pBufH[2] = pBufN[1];
	pBufH[1] = pBufN[2];
	pBufH[0] = pBufN[3];
#endif
}
