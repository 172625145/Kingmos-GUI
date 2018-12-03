/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __BHEAP_H
#define __BHEAP_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

//��ʼ�����,����֮ǰ�����ȵ��øú���
//��� ���ߴ� <= 4096      nCount <= 32
//
HANDLE WINAPI BlockHeap_Create( LPUINT lpuiSize, UINT nCount );

// ���� uiFlags ֵ
// �Ƿ�� BlockHeap�Ĳ������л��⣬���ڶ��߳�  
#define BLOCKHEAP_NO_SERIALIZE 1
// ��BlockHeap_Alloc��������size���ݣ��ʺ�string�ȿɱ�ķ���
#define BLOCKHEAP_RESIZABLE    2
//��ʼ��Ϊ0
#define BLOCKHEAP_ZERO_MEMORY    0x4
//

//�ӿ�ѷ���һ����
LPVOID WINAPI BlockHeap_Alloc( HANDLE handle, UINT uiFlags, UINT uiSize );
//�ͷ�һ����
BOOL WINAPI BlockHeap_Free( HANDLE handle, UINT uiFlags, LPVOID lpvBlock, UINT uiSize );
//�ӿ�ѷ���һ���ǹ̶���С�Ŀ�
#define BlockHeap_AllocString( h, f, s ) BlockHeap_Alloc( (h), ((f) | BLOCKHEAP_RESIZABLE), s )
//�ͷ���BlockHeap_AllocString�����һ����
#define BlockHeap_FreeString( h, f, lpv ) BlockHeap_Free( (h), ((f) | BLOCKHEAP_RESIZABLE), (lpv), 0 )

//����һ����򵥵ĵ��ã�����ģ��Ĭ�ϵĽ��̿�ѣ���Ĭ���õ�һ�δ����Ŀ�Ѷ���
//��ʼ��/�������
BOOL WINAPI BLK_Create( VOID );
//�ƻ�/�ͷſ��
BOOL WINAPI BLK_Destroy( VOID );
//�ӿ�ѷ���һ����
LPVOID WINAPI BLK_Alloc( UINT uiFlags, UINT uiSize );
//�ͷ�һ����
BOOL WINAPI BLK_Free( UINT uiFlags, LPVOID lpvBlock, UINT uiSize );
//�ӿ�ѷ���һ���ǹ̶���С�Ŀ飨�� string )
#define BLK_AllocString( f, s ) BLK_Alloc( ((f) | BLOCKHEAP_RESIZABLE), s )
//�ͷ���BlockHeap_AllocString�����һ����
#define BLK_FreeString( f, lpv ) BLK_Free( ((f) | BLOCKHEAP_RESIZABLE), (lpv), 0 )


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __BHEAP_H
