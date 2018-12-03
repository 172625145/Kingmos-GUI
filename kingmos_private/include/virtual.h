#ifndef __VIRTUAL_H
#define __VIRTUAL_H


//  The user mode virtual address space is 2GB split into 64 32M sections
//  of 512 64K blocks of 16 4K pages.
//
// Virtual address format:
//  3322222 222221111 1111 110000000000
//  1098765 432109876 5432 109876543210
//  zSSSSSS BBBBBBBBB PPPP oooooooooooo



//#define MEM_COMMIT           0x1000     
//#define MEM_RESERVE          0x2000     
//#define MEM_DECOMMIT         0x4000     
//#define MEM_RELEASE          0x8000     
//#define MEM_FREE            0x10000     
//#define MEM_PRIVATE         0x20000     
//#define MEM_MAPPED          0x40000     
//#define MEM_RESET           0x80000     

//#define PAGE_SIZE (1024*4)
//#define PAGE_MASK ( PAGE_SIZE - 1 ) 
#define SEGMENT_SIZE  0x02000000  // (32 * 1024 * 1024)
#define SEGMENT_BASE  ( dwAdr ) ( (dwAdr) & 0xfe000000 )
#define SEGMENT_MASK  0x01ffffff
#define MAX_SEGMENTS  (0x80000000 / 0x02000000)
//#define SEGMENT_OFFSET( dwAdr ) ( (dwAdr) & 0x01ffffff )

#define SEGMENT_SHIFT  25
//#define GET_SEGMENT_INDEX( dwAdr )  ( ( (dwAdr) >> SEGMENT_SHIFT ) & 0x3f )  
#define GET_SEGMENT_INDEX( dwAdr )  ( ( (dwAdr) >> SEGMENT_SHIFT ) )  

#define BLOCK_SIZE  0x10000
#define BLOCK_SHIFT 16
#define BLOCK_INDEX( dwAdr )  ( ( (dwAdr) >> BLOCK_SHIFT ) & 0x1ff ) 

#define BLOCKS_PER_SEGMENT (SEGMENT_SIZE/BLOCK_SIZE)
#define PAGES_PER_BLOCK  (BLOCK_SIZE / PAGE_SIZE)
#define PAGES_PER_MEGA   (1024*1024 / PAGE_SIZE)
#define PAGES_PER_SEGMENT  (SEGMENT_SIZE/PAGE_SIZE)
#define PAGE_INDEX_IN_SEGMENT( dwAdr )  ( ( dwAdr & SEGMENT_MASK ) / PAGE_SIZE )
#define PAGE_INDEX_IN_MEGA( dwAdr )  ( ( dwAdr & 0xfffff ) / PAGE_SIZE )

#define PAGE_INDEX( dwAdr ) ( ( (dwAdr) >> PAGE_SHIFT ) & 0xf )

#define MF_AUTO_COMMIT   0x80000000

typedef struct _MEMBLOCK
{
	UINT uiKey;
	UINT uiFlag;
	WORD idxFirst;
	WORD wLockCount;
	//UINT * lpuiPages;//[PAGES_PER_BLOCK];
	UINT uiPages[PAGES_PER_BLOCK];
}MEMBLOCK, FAR * LPMEMBLOCK;

#define NULL_BLOCK        0
//#define RESERVED_BLOCK    ( (LPMEMBLOCK)-1 )
#define RESERVED_BLOCK    ((LPMEMBLOCK)BLOCKS_PER_SEGMENT)
#define INVALID_PAGE      (-1)

typedef struct _SEGMENT
{
	LPMEMBLOCK lpBlks[BLOCKS_PER_SEGMENT];
}SEGMENT, FAR * LPSEGMENT;


//#define MAX_SEGMENTS 64
//LPSEGMENT lpSegments[MAX_SEGMENTS];

#define NOT_FIND_FIRST (-1)


#endif   //__VIRTUAL_H

