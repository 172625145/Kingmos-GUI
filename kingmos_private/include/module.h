/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __MODULE_H
#define __MODULE_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define		DL_DYN_ENTRY			0xffffffff

#define		PT_LOAD_CODE	1
#define		PT_LOAD_DATA	2
#define		PT_LOAD_RES		3

#define		FIND_ONLY			1
#define		FIND_NON_XIP		2
#define		FIND_NON_XIP_REF	3
//#define		FIND_MODULE		1
//#define		FIND_AND_REF	2

#define		MODULE_INIT			1
#define		MODULE_FINI			2

#define		LOAD_FAIL	1
#define		LOAD_OK		2

#define		XIP_MODULE			1
#define		NON_XIP_MODULE		2

#define		UNDEF_MODULE		0
#define		ROM_MODULE			1
#define		NORMAL_MODULE		2
//#define		ROM_MODULE		1
//#define		ROM_FILE		2
//#define		RAM_FILE		3


#define		LOAD_MODULE_NEW		1
#define		LOAD_MODULE_REF		2

#define		MODULE_EXEC			0	//1
#define		MODULE_LOAD			2	//2

#define		ROM_MODULE_EXE		1
#define		ROM_MODULE_DLL		2
#define		NORMAL_MODULE_EXE	3
#define		NORMAL_MODULE_DLL	4


#define		MODULE_LOAD_EXE		1
#define		MODULE_EXEC_EXE		2
#define		MODULE_LOAD_DLL		3
#define		MODULE_EXEC_DLL		4
#define		LOAD_DLL_CLEARLY	5
#define		LOAD_DLL_QUIETLY	6


#define		FIND_MODULE			1
#define		NEW_MODULE			2

//#define		KING_PATH		"kingmos"	
//#define		KINGMOS_PATH	"\\kingmos\\"
//#define		WORK_PATH		"\\system\\"

extern const char KING_PATH[];
extern const char KINGMOS_PATH[];
extern const char WORK_PATH[];

//#define	SECT_NAME_STRTAB	".strtab"
//#define	SECT_NAME_DATA		".data"
//#define	SECT_NAME_BSS		".bss"
//#define	SECT_NAME_RES		".res"

//#define		DLL_EXTENSION		"dll"
//#define		EXE_EXTENSION		"exe"

#define		MAX_FUN_RES_LENGTH	20

typedef struct _FUNCTION_SYM{
	struct	_FUNCTION_SYM	*lpNext;
	//char				lpszFunName[MAX_FUN_RES_LENGTH];
	LPSTR				lpFuncName;
	UINT				lpProc;
}FUNCTION_SYM, *PFUNCTION_SYM;

typedef struct _RES_SYM{
	struct	_RES_SYM	*lpNext;
	char				lpszResName[MAX_FUN_RES_LENGTH];
	UINT				uType;
	UINT				lpRes;
}RES_SYM, *PRES_SYM;

typedef struct __MODULE_INFO{
//	UINT			pModBase;	//module start position.
	PFUNCTION_SYM	pFunSym;	//function pointer list.
	//PRES_SYM		pResSym;	//resource pointer list.
}MODULE_INFO, *PMODULE_INFO;




//---------------------------------------execelf.c------------------------------------------------------------//
typedef struct module_info {
	Elf32_Addr	addr;		/* ==> l_addr */
	Elf32_Addr	start;		/* ==> l_map_start */
	Elf32_Addr	end;		/* ==> l_map_end */
	Elf32_Addr	entry;		/* ==> l_entry */
	
	Elf32_Phdr	*phdr;		/* ==> l_phdr */
	Elf32_Dyn	*ld;		/* ==> l_ld */

//	HANDLE		hFile;		//Zb add.
	unsigned int alloced;	/* 1 -> phdr alloced, 0 -> phdr in PT_LOAD */
	
	Elf32_Half	nphdr;		/* ==> l_phnum */
	Elf32_Half	nld;		/* ==> l_ldnum */
}_MODULE_INFO;

typedef struct _load_module {
	Elf32_Addr	addr;		/* ==> l_addr */
	Elf32_Addr	start;		/* ==> l_map_start */
	Elf32_Addr	end;		/* ==> l_map_end */
	Elf32_Addr	entry;		/* ==> l_entry */
	
//	Elf32_Phdr	*phdr;		/* ==> l_phdr */
//	Elf32_Dyn	*ld;		/* ==> l_ld */

//	HANDLE		hFile;		//Zb add.
//	unsigned int alloced;	/* 1 -> phdr alloced, 0 -> phdr in PT_LOAD */
	
//	Elf32_Half	nphdr;		/* ==> l_phnum */
//	Elf32_Half	nld;		/* ==> l_ldnum */

	Elf32_Addr initproc;	//DT_INIT  addr;
	Elf32_Addr finiproc;    //DT_FINI  addr;

	const void *symtab;		//DT_SYMTAB addr;
	const void *strtab;		//DT_STRTAB addr;
	UINT		l_nchain;	//symbol num;

}LOAD_MODULE, *PLOAD_MODULE;

struct module_seg { 
  	  struct module_seg *lpNext;
      char	*sh_name; 
      //Elf32_Word sh_type; 
      //Elf32_Word sh_flags; 
      Elf32_Addr sh_addr; 
      Elf32_Off  sh_offset; 
      Elf32_Word sh_size; 
      //Elf32_Word sh_link; 
      //Elf32_Word sh_info; 
      //Elf32_Word sh_addralign; 
      //Elf32_Word sh_entsize; 
};

typedef struct exe_module {
	unsigned int	entry;		/* the main thread start address */
	unsigned int	stack;		/* the main thread stack */

	unsigned int	lock;		/* lock variable */
	unsigned int	inter;		/* if the program have interpret set to 1 */

	HANDLE			hFile;		//Zb add.
	//char			*szInter;	//inter name;

	//UINT				*pbuf;
	//struct module_info	*myinfo;	//Zb add ...
	
	struct module_seg	*seg_info;	//module's section info...

	struct module_info *exe_info; /* exe module in memory information */
	struct module_info *dll_info; /* interpret module in memory information */
}_EXE_MODULE;

//-------------------------------------------------------------------------------------------------------------//

struct _MODULE_HANDLE;

typedef struct _MODULE_NODE{
	struct _MODULE_NODE		*lpNext;
	struct _MODULE_HANDLE	*lpHandle;		//lpHandle 链表的第一个HANDLE优先放置EXEC的Module.
//	struct _MODULE_HANDLE   *lpBase;		//first load module handle.
//	UINT				uFlag;	//load or exec module.
	UINT				uType;	//exe module or dll module.
	UINT				uPos;	//rom module or other module
	UINT				uRef;	//refence number.
	LPTSTR				lpszApName;

	struct module_seg	*seg_info;	//module's section info...
//	BOOL				bValidSeg;	//indecate the seg_info whether valid...

//	UINT				lpStart;
//	HANDLE				hFile;
//	MODULE_INFO			ModInfo;
//	struct module_info	*info;	
	//_MODULE_INFO		*info2;
	//UINT				*pbuf;
	//_EXE_MODULE		*pmod;

//	WINMAINPROC lpProc;
//	HICON hIcon;
//	UINT uiRefCount;
}MODULE_NODE, * PMODULE_NODE;

typedef struct _MODULE_HANDLE{
	struct _MODULE_HANDLE * lpNext;
//	struct _MODULE_HANDLE * lpRef;
	
	struct _MODULE_NODE	*lpModule;
	HANDLE				hProc;	//The process' handle.
	UINT				uMode;	//load or exec module.
	HANDLE				hFile;
	UINT				uRef;	//will be referenced number...
//	UINT				uRefed;	//have be refed number by other...
//	UINT				uType;	//exe module or dll module.
//	MODULE_INFO			ModInfo;
	PFUNCTION_SYM		lpProc;
//	struct module_seg	*seg_info;	//module's section info...
//	struct module_info	*info;
	PLOAD_MODULE		info;
}MODULE_HANDLE, * PMODULE_HANDLE;

typedef struct _HANDLE_LIST{
	PMODULE_HANDLE	pHandle;
	struct _HANDLE_LIST *pnext;
}HANDLE_LIST, *PHANDLE_LIST;

typedef struct _MODULE_CLIENT{
	struct _MODULE_CLIENT	*pNext;
	PMODULE_NODE	pModule;
//	PMODULE_HANDLE	pHandle;
	PHANDLE_LIST	pHandlelist;
	LPTSTR			lpszModuleName;
	HANDLE			hProc;	//The process' handle.	
	UINT			uCreateFlag;
}MODULE_CLIENT, *PMODULE_CLIENT;


//define for debug ...

#if 0

#define		TONE_EXEC	0
#define		TONE_MOD	0
#define		TONE_DLL	0
#define		TONE_TEST	0
#define		TONE_CS		0
#define		TONE_ERROR	1
#define		TONE_DEBUG	0

#else

#define		TONE_EXEC	0
#define		TONE_MOD	0
#define		TONE_DLL	0
#define		TONE_TEST	0
#define		TONE_CS		0
#define		TONE_ERROR	1
#define		TONE_DEBUG	0

#endif


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  //






