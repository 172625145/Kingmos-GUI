/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __ECOMDATE_H
#define __ECOMDATE_H


#define CDM_GETDATETIME				(WM_USER + 1)
#define CDM_SETDATETIME				(WM_USER + 2)

#define ETM_GETTIME					(WM_USER + 3)
#define ETM_SETTIME					(WM_USER + 4)

#define ETM_SETMODIFY				(WM_USER + 5)
#define ETM_GETMODIFY				(WM_USER + 6)

#define ETM_TIMEFMTCHG				(WM_USER + 7)  // ϵͳʱ���ʽ�����ı�

#define ET_NOSECOND					0x00000001
#define ET_RUNCLOCK					0x00000002
#define ET_HORZUDS					0X00000004  //ʹ��ˮƽ����
#define ET_AUTOSIZE					0X00000008  //�Զ��������ڴ�С

extern const TCHAR	classCOMDATE[];
extern const TCHAR  classEDITTIME[];

struct DATETIMENMHDR{
	NMHDR			hdr;
	SYSTEMTIME		dt;
};

#endif //__ECOMDATE_H
