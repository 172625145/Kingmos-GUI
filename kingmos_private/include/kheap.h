#ifndef __KHEAP_H
#define __KHEAP_H

#ifndef __EPCORE_H
#include <epcore.h>
#endif

#define ALIGN_INT32( n )    ( ( (n) + 3 ) & ~3 )

#define BLOCK_TIMERLIST_SIZE  ALIGN_INT32( sizeof( TIMERLIST ) )
#define BLOCK_WAITQUEUE_SIZE  ALIGN_INT32( sizeof( WAITQUEUE ) )
#define BLOCK_SEMAPHORE_SIZE  ALIGN_INT32( sizeof( SEMAPHORE ) )
#define BLOCK_THREAD_SIZE     ALIGN_INT32( sizeof( THREAD ) )

#endif //__KHEAP_H



