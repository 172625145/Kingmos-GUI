/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-03-06
���ߣ�    �ܱ�
�޸ļ�¼��
******************************************************/


#ifndef _KINGMOS_INC_STDDEF
#define _KINGMOS_INC_STDDEF

#ifdef  __cplusplus
extern "C" {
#endif



/* Define NULL pointer value and the offset() macro */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#define offsetof(s,m)   (size_t)&(((s *)0)->m)


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_STDDEF */
