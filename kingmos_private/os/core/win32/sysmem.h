/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __SYSMEM_H
#define __SYSMEM_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define PG_PERMISSION_MASK  0x00000FFF
#define PG_PHYS_ADDR_MASK   0xFFFFF000    
#define PG_VALID_MASK       0x00000002
#define PG_1K_MASK          0x00000001
#define PG_CACHE_MASK       0x0000000C
#define PG_GUARD            0x0000000C
#define PG_CACHE            (OEMARMCacheMode())
#define PG_MINICACHE        0x00000008
#define PG_NOCACHE          0x00000000
#define PG_DIRTY_MASK       0x00000010
#define PG_EXECUTE_MASK     0x00000000  /* not supported by HW */

#define PG_PROTECTION       0x00000FF0
#define PG_PROT_READ        0x00000000
#define PG_PROT_WRITE       0x00000FF0
#define PG_PROT_URO_KRW     0x00000AA0
#define PG_PROT_UNO_KRW     0x00000550      // user no access, kernel read/write
#define PG_SIZE_MASK        0x00000000

// define page flag
#define PGF_MASK        0x00000FFF
#define PGF_VALID       0x00000002
#define PGF_READ        0x00000000
#define PGF_READWRITE   0x00000FF0
//#define PGF_WRITECOPY
#define PGF_EXECUTE     0x00000000   // no support
//#define PGF_NOACCESS
#define PGF_CACHE       0x0000000C
#define PGF_NOCACHE     0x00000000

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif //__SYSMEM_H
