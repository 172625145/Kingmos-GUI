/***************************************************
Copyright ? ��Ȩ����? 1998-2003΢�߼�����������Ȩ����
***************************************************/

#ifndef _ADPCM_H_
#define _ADPCM_H_


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */  

typedef struct tagADPCM_STATE{
		int valprev;
		int index;
}ADPCM_STATE, *LPADPCM_STATE;

// ********************************************************************
//������void ADPCM_Initialize(LPADPCM_STATE pState)
//������
//	IN pState -- ��ǰADPCM��״̬�ṹ
//
//����ֵ��
//	��
//
//������������ʼ����ǰ��ADPCM״̬��
//����: 
// ********************************************************************
void ADPCM_Initialize(LPADPCM_STATE pState);

// ********************************************************************
//������int ADPCM_Decoder(unsigned char *pInData, int iInLen,unsigned short *pOutData, int iOutLen, LPADPCM_STATE pState)
//������
//	IN pInData -- Ҫ��ѹ�������ݻ���
//	IN iInLen -- Ҫ��ѹ�������ݴ�С
//	IN pOutData -- ����Ѿ���ѹ��������ݻ���
//	IN iOutLen -- �����С
//	IN pState -- ��ǰADPCM��״̬�ṹ
//
//����ֵ��
//	�ɹ����ؽ�ѹ�������ݳߴ硣
//����������ADPCM���ݽ�ѹ����
//����: 
// ********************************************************************
int ADPCM_Decoder(unsigned char *pInData, int iInLen,unsigned short *pOutData, int iOutLen, LPADPCM_STATE pState);

// ********************************************************************
//������int ADPCM_Coder(unsigned short *pInData, int iInLen,unsigned char *pOutData, int iOutLen, LPADPCM_STATE pState)
//������
//	IN pInData -- Ҫѹ�������ݻ���
//	IN iInLen -- Ҫѹ�������ݴ�С
//	IN pOutData -- ����Ѿ�ѹ��������ݻ���
//	IN iOutLen -- �����С
//	IN pState -- ��ǰADPCM��״̬�ṹ
//
//����ֵ��
//	�ɹ�����ѹ�������ݳߴ硣
//����������ADPCM����ѹ����
//����: 
// ********************************************************************
int ADPCM_Coder(unsigned short *pInData, int iInLen,unsigned char *pOutData, int iOutLen, LPADPCM_STATE pState);



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_ADPCM_H_
