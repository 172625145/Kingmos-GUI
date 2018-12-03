/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����socket �������� 
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-03-09
���ߣ�    ФԶ��
�޸ļ�¼��

******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <socket.h>


/***************  ȫ���� ���壬 ���� *****************/

/******************************************************/

//
//Internet address value		Meaning 
//
//"4.3.2.16"					Decimal 
//"004.003.002.020"				Octal ---��֧��
//"0x4.0x3.0x2.0x10"			Hexadecimal 
//"4.003.002.0x10"				Mix 
//
// ********************************************************************
// ������
// ������
//	IN cp-
// ����ֵ��
//	����
// �������������ַ�����ʾ��IP��ַ ת��Ϊ DWORD��ʾ��IP��ַ
// ����: 
// ********************************************************************
#define	BM_DECIMAL		0
#define	BM_HEX			1
DWORD	Skt_inet_addr( const char* cp )
{
	LPBYTE		pTmp = (LPBYTE)cp;
	LPBYTE		pTmpEnd = pTmp + 16;

	DWORD		dwIP = 0;
	LPBYTE		pIP = (LPBYTE)&dwIP;

	BYTE		bVal = 0;
	BYTE		bMean = BM_DECIMAL;	//
	BYTE		bPos = 0;

	while( *pTmp && (pTmp<pTmpEnd) )
	{
		if( *pTmp=='.' )
		{
			*pIP = bVal;
			pIP ++;

			bVal = 0;
			bPos = 0;
			bMean = BM_DECIMAL;
		}
		else
		{
			if( (*pTmp>='0') && (*pTmp<='9') )
			{
				if( bMean==BM_HEX )
				{
					bVal <<= 4;
				}
				else
				{
					bVal *= 10;
				}
				bVal += (*pTmp - '0');
			}
			else
			{
				if( bMean==BM_HEX ) 
				{
					bVal <<= 4;
					if( (*pTmp>='A') && (*pTmp<='F') )
					{
						bVal += (*pTmp - 'A' + 0x0A);
					}
					else
					{
						if( (*pTmp>='a') && (*pTmp<='f') )
						{
							bVal += (*pTmp - 'a' + 0x0A);
						}
						else
						{
							return INADDR_NONE;
						}
					}
				}
				else
				{
					if( (*pTmp=='x') || (*pTmp=='X') )
					{
						if( (bVal==0) && (bPos==1) )
						{
							bMean = BM_HEX;
						}
					}
					else
					{
						return INADDR_NONE;
					}
				}
			}

			bPos ++;
		}
	
		pTmp ++;
	}
	*pIP = bVal;
	return dwIP;
}

// ********************************************************************
// ������
// ������
//	IN in-
// ����ֵ��
//	����
// ������������ DWORD��ʾ��IP��ַ ת��Ϊ �ַ�����ʾ��IP��ַ 
// ����: 
// ********************************************************************
char	g_szinetaddr[32];
char*	Skt_inet_ntoa( struct in_addr in )
{
	LPBYTE	pTmp = (LPBYTE)&in;

	sprintf( g_szinetaddr, "%d.%d.%d.%d", *pTmp, pTmp[1], pTmp[2], pTmp[3] );
	return (char*)g_szinetaddr;
}
// ********************************************************************
// ������
// ������
//	IN in-
//	OUT paddr-
// ����ֵ��
//	����
// ������������ DWORD��ʾ��IP��ַ ת��Ϊ �ַ�����ʾ��IP��ַ 
// ����: 
// ********************************************************************
void	Skt_inet_ntoa_ex( struct in_addr in, OUT char* paddr )
{
	LPBYTE	pTmp = (LPBYTE)&in;
	sprintf( paddr, "%d.%d.%d.%d", *pTmp, pTmp[1], pTmp[2], pTmp[3] );
}

void	Skt_inet_ntoa_ip( LPBYTE pIP, OUT char* paddr )
{
	sprintf( paddr, "%d.%d.%d.%d", *pIP, pIP[1], pIP[2], pIP[3] );
}

int		Skt_IsSetFD( SOCKET s, fd_set FAR * set )
{
	u_int	i;
	for( i=0; i<set->fd_count; i++ )
	{
		if( set->fd_array[i]==s )
		{
			return 1;
		}
	}
	
	return 0;
}

