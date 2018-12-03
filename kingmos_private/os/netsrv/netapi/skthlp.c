/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：socket 辅助函数 
版本号：  1.0.0
开发时期：2004-03-09
作者：    肖远钢
修改记录：

******************************************************/
//root include
#include <ewindows.h>
//"\inc_app"
#include <socket.h>


/***************  全局区 定义， 声明 *****************/

/******************************************************/

//
//Internet address value		Meaning 
//
//"4.3.2.16"					Decimal 
//"004.003.002.020"				Octal ---不支持
//"0x4.0x3.0x2.0x10"			Hexadecimal 
//"4.003.002.0x10"				Mix 
//
// ********************************************************************
// 声明：
// 参数：
//	IN cp-
// 返回值：
//	返回
// 功能描述：将字符串表示的IP地址 转换为 DWORD表示的IP地址
// 引用: 
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
// 声明：
// 参数：
//	IN in-
// 返回值：
//	返回
// 功能描述：将 DWORD表示的IP地址 转换为 字符串表示的IP地址 
// 引用: 
// ********************************************************************
char	g_szinetaddr[32];
char*	Skt_inet_ntoa( struct in_addr in )
{
	LPBYTE	pTmp = (LPBYTE)&in;

	sprintf( g_szinetaddr, "%d.%d.%d.%d", *pTmp, pTmp[1], pTmp[2], pTmp[3] );
	return (char*)g_szinetaddr;
}
// ********************************************************************
// 声明：
// 参数：
//	IN in-
//	OUT paddr-
// 返回值：
//	返回
// 功能描述：将 DWORD表示的IP地址 转换为 字符串表示的IP地址 
// 引用: 
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

