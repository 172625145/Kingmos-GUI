/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __BHEAP_H
#define __BHEAP_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

//初始化块堆,在用之前必须先调用该函数
//块堆 最大尺寸 <= 4096      nCount <= 32
//
HANDLE WINAPI BlockHeap_Create( LPUINT lpuiSize, UINT nCount );

// 定义 uiFlags 值
// 是否对 BlockHeap的操作进行互斥，用于多线程  
#define BLOCKHEAP_NO_SERIALIZE 1
// 由BlockHeap_Alloc保存分配的size数据，适合string等可变的分配
#define BLOCKHEAP_RESIZABLE    2
//初始化为0
#define BLOCKHEAP_ZERO_MEMORY    0x4
//

//从块堆分配一个块
LPVOID WINAPI BlockHeap_Alloc( HANDLE handle, UINT uiFlags, UINT uiSize );
//释放一个块
BOOL WINAPI BlockHeap_Free( HANDLE handle, UINT uiFlags, LPVOID lpvBlock, UINT uiSize );
//从块堆分配一个非固定大小的块
#define BlockHeap_AllocString( h, f, s ) BlockHeap_Alloc( (h), ((f) | BLOCKHEAP_RESIZABLE), s )
//释放用BlockHeap_AllocString分配的一个块
#define BlockHeap_FreeString( h, f, lpv ) BlockHeap_Free( (h), ((f) | BLOCKHEAP_RESIZABLE), (lpv), 0 )

//这是一组更简单的调用，调用模块默认的进程块堆（即默认用第一次创建的块堆对象）
//初始化/创建块堆
BOOL WINAPI BLK_Create( VOID );
//破坏/释放块堆
BOOL WINAPI BLK_Destroy( VOID );
//从块堆分配一个块
LPVOID WINAPI BLK_Alloc( UINT uiFlags, UINT uiSize );
//释放一个块
BOOL WINAPI BLK_Free( UINT uiFlags, LPVOID lpvBlock, UINT uiSize );
//从块堆分配一个非固定大小的块（如 string )
#define BLK_AllocString( f, s ) BLK_Alloc( ((f) | BLOCKHEAP_RESIZABLE), s )
//释放用BlockHeap_AllocString分配的一个块
#define BLK_FreeString( f, lpv ) BLK_Free( ((f) | BLOCKHEAP_RESIZABLE), (lpv), 0 )


#ifdef __cplusplus
}
#endif  // __cplusplus

#endif // __BHEAP_H
