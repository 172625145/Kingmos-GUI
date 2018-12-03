/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ROMHEADER_H
#define __ROMHEADER_H

//#include <elf.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


#define KFLAG_DISALLOW_PAGING   0x00000001
#define KFLAG_NOTALLKMODE       0x00000002
#define KFLAG_TRUSTROMONLY      0x00000010
#define KFLAG_HONOR_DLL_BASE    0x00000040  // honor DLL's preferred load address.
                                            // Loading DLL at its preferred load address will release kernel
                                            // from reloacating the DLL. However, there might be potential Backward
                                            // compatibility issue since there might be existing DLL binaries
                                            // out there that sets it's preferred load address really low and thus
                                            // not able to be loaded.



#define ROM_EXTRA 9

typedef struct e32_rom {
    unsigned short  e32_objcnt;     /* Number of memory objects            */
    unsigned short  e32_imageflags; /* Image flags                         */
    unsigned long   e32_entryrva;   /* Relative virt. addr. of entry point */
    unsigned long   e32_vbase;      /* Virtual base address of module      */
    unsigned short  e32_subsysmajor;/* The subsystem major version number  */
    unsigned short  e32_subsysminor;/* The subsystem minor version number  */
    unsigned long   e32_stackmax;   /* Maximum stack size                  */
    unsigned long   e32_vsize;      /* Virtual size of the entire image    */
    unsigned long   e32_sect14rva;  /* section 14 rva */
    unsigned long   e32_sect14size; /* section 14 size */
//    struct info     e32_unit[ROM_EXTRA]; /* Array of extra info units      */
    unsigned short  e32_subsys;     /* The subsystem type                  */
} e32_rom;

typedef struct o32_rom {
    unsigned long       o32_vsize;      /* Virtual memory size              */
    unsigned long       o32_rva;        /* Object relative virtual address  */
    unsigned long       o32_psize;      /* Physical file size of init. data */
    unsigned long       o32_dataptr;    /* Image pages offset               */
    unsigned long		o32_realaddr;   /* pointer to actual                */
    unsigned long       o32_flags;      /* Attribute flags for the object   */
} o32_rom;


//exe_imageflags: 类型定义
#define		ELF_FILE_EXE	0x0001
#define		ELF_FILE_OBJ	0x0002

#define		ELF_LOAD_OFFSET		0x20000


typedef struct elf_exe_rom{
	unsigned short  exe_imageflags; /* Image flags                         */
	unsigned long	exe_entryrav;	/* Relative virt. addr. of entry point */
	unsigned long   exe_vbase;      /* Virtual base address of module      */
    unsigned long   exe_stackmax;   /* Maximum stack size                  */
    unsigned long   exe_vsize;      /* Virtual size of the entire image    */
	unsigned long	exe_phdroff;	// program header offset of base address.
	unsigned long	exe_phdrnum;	// program header number.
}EXE_ROM;

typedef struct elf_obj_rom {
    unsigned long       obj_vsize;      /* Virtual memory size              */
    unsigned long       obj_rva;        /* Object relative virtual address  */
    unsigned long       obj_psize;      /* Physical file size of init. data */
    unsigned long       obj_dataptr;    /* Image pages offset               */
    unsigned long		obj_realaddr;   /* pointer to actual                */
    unsigned long       obj_flags;      /* Attribute flags for the object   */
	unsigned long		obj_phdroff;	// program header offset of base address.
	unsigned long		obj_phdrnum;	// program header number.
} OBJ_ROM;

//
//  ROM Header Structure - pTOC in NK points here
//
/*typedef struct ROMHDR {
    ULONG   dllfirst;               // first DLL address
    ULONG   dlllast;                // last DLL address
    ULONG   physfirst;              // first physical address
    ULONG   physlast;               // highest physical address
    ULONG   nummods;                // number of TOCentry's
    ULONG   ulRAMStart;             // start of RAM
    ULONG   ulRAMFree;              // start of RAM free space
    ULONG   ulRAMEnd;               // end of RAM
    ULONG   ulCopyEntries;          // number of copy section entries
    ULONG   ulCopyOffset;           // offset to copy section
    ULONG   ulProfileLen;           // length of PROFentries RAM 
    ULONG   ulProfileOffset;        // offset to PROFentries
    ULONG   numfiles;               // number of FILES
    ULONG   ulKernelFlags;          // optional kernel flags from ROMFLAGS .bib config option
    ULONG   ulFSRamPercent;         // Percentage of RAM used for filesystem 
    ULONG   ulDrivglobStart;        // device driver global starting address
    ULONG   ulDrivglobLen;          // device driver global length
    USHORT  usCPUType;              // CPU (machine) Type
    USHORT  usMiscFlags;            // Miscellaneous flags
    PVOID   pExtensions;            // pointer to ROM Header extensions
    ULONG   ulTrackingStart;        // tracking memory starting address
    ULONG   ulTrackingLen;          // tracking memory ending address
} ROMHDR;*/

typedef struct _ROMHDR {
    ULONG   dllfirst;               // first DLL address
    ULONG   dlllast;                // last DLL address
    ULONG   physfirst;              // first physical address
    ULONG   physlast;               // highest physical address
    ULONG   nummods;                // number of TOCentry's
    ULONG   ulRAMStart;             // start of RAM
    ULONG   ulRAMFree;              // start of RAM free space
    ULONG   ulRAMEnd;               // end of RAM
    ULONG   ulCopyEntries;          // number of copy section entries
    ULONG   ulCopyOffset;           // offset to copy section

//Instead of the next two member:
	ULONG	ulModOffset;			// offset to toc section	
	ULONG	ulFileOffset;			// offset to file section
//    ULONG   ulProfileLen;           // length of PROFentries RAM 
//    ULONG   ulProfileOffset;        // offset to PROFentries

    ULONG   numfiles;               // number of FILES
    ULONG   ulKernelFlags;          // optional kernel flags from ROMFLAGS .bib config option
    ULONG   ulFSRamPercent;         // Percentage of RAM used for filesystem 
    ULONG   ulDrivglobStart;        // device driver global starting address
    ULONG   ulDrivglobLen;          // device driver global length
    USHORT  usCPUType;              // CPU (machine) Type
    USHORT  usMiscFlags;            // Miscellaneous flags
    PVOID   pExtensions;            // pointer to ROM Header extensions
    ULONG   ulTrackingStart;        // tracking memory starting address
    ULONG   ulTrackingLen;          // tracking memory ending address

} ROMHDR, *PROMHDR;

typedef struct ROMChain_t {
    struct ROMChain_t *pNext;
    ROMHDR *pTOC;
} ROMChain_t;


//
// ROM Header extension: PID
//
#define PID_LENGTH 10

typedef struct ROMPID {
    DWORD dwPID[PID_LENGTH];        // PID
    PVOID pNextExt;                 // pointer to next extension if any
} ROMPID;

typedef struct _ELF_INFO {
	ULONG	ulFileType;
	ULONG	ulEntry;				// file execute entry.
	ULONG	ulFileBase;				// The file's code position at rom.
	ULONG	ulPhdroff;				// program header offset of base address.
	ULONG	ulPhdrnum;				// program header number.
} ELF_INFO, *PELF_INFO;

/*typedef struct _MODULE_SEG { 
  	  //struct module_seg *lpNext;
      //char	*sh_name; 
      //Elf32_Word sh_type; 
      //Elf32_Word sh_flags; 
      ULONG		sh_addr; 
      ULONG		sh_offset; 
      ULONG		sh_size; 
      //Elf32_Word sh_link; 
      //Elf32_Word sh_info; 
      //Elf32_Word sh_addralign; 
      //Elf32_Word sh_entsize; 
}MODULE_SEG, *PMODULE_SEG;
*/

typedef struct _module_seg { 
  	  struct module_seg *lpNext;
      char	*sh_name; 
      //Elf32_Word sh_type; 
      //Elf32_Word sh_flags; 
      unsigned int sh_addr; 
      unsigned int sh_offset; 
      unsigned int sh_size; 
      //Elf32_Word sh_link; 
      //Elf32_Word sh_info; 
      //Elf32_Word sh_addralign; 
      //Elf32_Word sh_entsize; 
}MODULE_SEG, *PMODULE_SEG;

//
//  Module Table of Contents - follows ROMHDR in image
//
typedef struct TOCentry {           // MODULE BIB section structure
    DWORD	dwFileAttributes;
    FILETIME ftTime;
    DWORD	nFileSize;
    LPSTR   lpszFileName;
    ULONG   ulExeOffset;            // Offset to E32 structure
    ULONG   ulObjOffset;            // Offset to O32 structure
    ULONG   ulLoadOffset;           // MODULE load buffer offset
	ELF_INFO	ElfInfo;
	BOOL		bValidRes;			// 
	MODULE_SEG	ResSeg;				// segment res info.
} TOCentry, *LPTOCentry;

#define		MAX_NAME	20

typedef	struct _TOC_DUMP{
	char		filename[MAX_NAME];
	TOCentry	Tocentry;
	EXE_ROM		Exeinfo;
	OBJ_ROM		Objinfo;	
}TOC_DUMP,*PTOC_DUMP;

typedef	struct moduleentry{
//	char		file[MAX_PATH];
//	Elf32_Phdr	*pPhdr;
	TOCentry	*pToc;
	EXE_ROM		*pExeinfo;
	OBJ_ROM		*pObjinfo;	
}MODLIST, *PMODLIST;

//
//  Files Section Structure
//
typedef struct FILESentry {         // FILES BIB section structure
    DWORD		dwFileAttributes;
    FILETIME	ftTime;
    DWORD		nRealFileSize;
    DWORD		nCompFileSize;
    LPSTR		lpszFileName;
    ULONG		ulLoadOffset;           // FILES load buffer offset
} FILESentry, *LPFILESentry;

typedef	struct _FILE_DUMP{
	char		filename[MAX_NAME];
	FILESentry	Filentry;
}FILE_DUMP,*PFILE_DUMP;

//
//  Copy Section Structure
//
typedef struct COPYentry {
    ULONG   ulSource;               // copy source address
    ULONG   ulDest;                 // copy destination address
    ULONG   ulCopyLen;              // copy length
    ULONG   ulDestLen;              // copy destination length 
                                    // (zero fill to end if > ulCopyLen)
} COPYentry,*LPCOPYentry;






#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  //__MALLOC_H
