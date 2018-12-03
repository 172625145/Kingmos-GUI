/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵�����������Ĺ��� ��ʼ���ӿ��ļ�
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-05-09
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/

//root include
#include <ewindows.h>


/***************  ȫ���� ���壬 ���� *****************/
extern	BOOL	Ras_Init( HWND hWndNetApp );
extern	void	Ras_Deinit( );
extern	BOOL	PPP_Init( );
extern	void	PPP_Deinit( );
extern	BOOL	IP_Init( );
extern	void	IP_Deinit( );
extern	BOOL	Icmp_Init( );
extern	void	Icmp_Deinit( );

extern	BOOL	Sock_Init( );
extern	void	Sock_Deinit( );
extern	BOOL	Tcp_Init( );
extern	void	Tcp_Deinit( );
extern	BOOL	Udp_Init( );
extern	void	Udp_Deinit( );
extern	BOOL	DNS_Init( );
extern	void	DNS_Deinit( );

extern	BOOL	Internet_Init( );
extern	void	Internet_Deinit( );

extern	BOOL	Wsp_Init( );
extern	void	Wsp_Deinit( );
extern	BOOL	MCtr_Init( );
extern	void	MCtr_Deinit( );

extern	void	Sock_RegAPI( );
extern	void	Ras_RegAPI( );
extern	void	Wap_RegAPI( );
extern	void	Mms_RegAPI( );
extern	void	Internet_RegAPI();

/******************************************************/

BOOL	SMgr_Init( )
{
	//��ʼ�� RAS/TAPI(TSP)
	if( !Ras_Init( 0 ) )
	{
		return FALSE;
	}
	if( !PPP_Init() )
	{
		return FALSE;
	}
	if( !IP_Init() )
	{
		return FALSE;
	}
	if( !Icmp_Init() )
	{
		return FALSE;
	}


	if( !Sock_Init() )
	{
		return FALSE;
	}
	if( !Tcp_Init() )
	{
		return FALSE;
	}
	if( !Udp_Init() )
	{
		return FALSE;
	}
	
	if( !DNS_Init() )
	{
		return FALSE;
	}
	if( !Internet_Init() )
	{
		return FALSE;
	}
#if 0
	//WAP and MMS
	if( !Wsp_Init() )
	{
		return FALSE;
	}
	if( !MCtr_Init() )
	{
		return FALSE;
	}
#endif
	return TRUE;
}

//
BOOL	SMgr_RegAPI( )
{
	//
	Ras_RegAPI( );
	Sock_RegAPI( );

	Internet_RegAPI();
#if 0
	Wap_RegAPI( );
	Mms_RegAPI( );
#endif
	return TRUE;
}


void	SMgr_Deinit( )
{
	Ras_Deinit( );
	PPP_Deinit();
	IP_Deinit();
	Icmp_Deinit();

	Sock_Deinit();
	Tcp_Deinit();
	Udp_Deinit();
	
	DNS_Deinit();
	//Internet_Deinit();

	//WAP and MMS
#if 0
	Wsp_Deinit();
	MCtr_Deinit();
#endif 
}






