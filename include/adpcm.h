/***************************************************
Copyright ? 版权所有? 1998-2003微逻辑。保留所有权利。
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
//声明：void ADPCM_Initialize(LPADPCM_STATE pState)
//参数：
//	IN pState -- 当前ADPCM的状态结构
//
//返回值：
//	无
//
//功能描述：初始化当前的ADPCM状态。
//引用: 
// ********************************************************************
void ADPCM_Initialize(LPADPCM_STATE pState);

// ********************************************************************
//声明：int ADPCM_Decoder(unsigned char *pInData, int iInLen,unsigned short *pOutData, int iOutLen, LPADPCM_STATE pState)
//参数：
//	IN pInData -- 要解压缩的数据缓存
//	IN iInLen -- 要解压缩的数据大小
//	IN pOutData -- 存放已经解压缩后的数据缓存
//	IN iOutLen -- 缓存大小
//	IN pState -- 当前ADPCM的状态结构
//
//返回值：
//	成功返回解压缩的数据尺寸。
//功能描述：ADPCM数据解压缩。
//引用: 
// ********************************************************************
int ADPCM_Decoder(unsigned char *pInData, int iInLen,unsigned short *pOutData, int iOutLen, LPADPCM_STATE pState);

// ********************************************************************
//声明：int ADPCM_Coder(unsigned short *pInData, int iInLen,unsigned char *pOutData, int iOutLen, LPADPCM_STATE pState)
//参数：
//	IN pInData -- 要压缩的数据缓存
//	IN iInLen -- 要压缩的数据大小
//	IN pOutData -- 存放已经压缩后的数据缓存
//	IN iOutLen -- 缓存大小
//	IN pState -- 当前ADPCM的状态结构
//
//返回值：
//	成功返回压缩的数据尺寸。
//功能描述：ADPCM数据压缩。
//引用: 
// ********************************************************************
int ADPCM_Coder(unsigned short *pInData, int iInLen,unsigned char *pOutData, int iOutLen, LPADPCM_STATE pState);



#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //_ADPCM_H_
