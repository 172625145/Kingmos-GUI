/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：dynlink loader模块
版本号：1.0.0
开发时期：2003-04-40
作者：ZB
修改记录：
	1. 2003-08-26...ZB: 去处PAGE_NOCACHE标志，此标志降低了系统性能。

******************************************************/

#include <ewindows.h>
#include <elf.h>
#include <eapisrv.h>
#include <module.h>

//#define __dlmalloc		malloc
//#define __dlfree		free
#define __dlmalloc		_kalloc
#define __dlfree		_kfree

//#define RETAILMSG(TONE_DLL, 		EdbgOutputDebugString

//efine RETAILMSG(TONE_DLL, 		DbgOutString
#define __dlexit(exp)
#define __dlassert(exp)	ASSERT(exp)

#include "dlstruct.h"

	/* This #define produces dynamic linking inline functions for
	bootstrap relocation instead of general-purpose relocation.  */
#define RTLD_BOOTSTRAP

#define RESOLVE_MAP(sym, version, flags) \
	((*(sym))->st_shndx == SHN_UNDEF ? 0 : &bootstrap)

#define RESOLVE(sym, flags) \
	((*(sym))->st_shndx == SHN_UNDEF ? 0 : bootstrap.l_addr)

#include <dlarch.h>
#include "dldyn.h"

struct __dlmodule	*__dl_loaded;
unsigned int		__dl_nloaded;

//Zb add...


/* this struct come from exe_elf.c
   the __dlstartup param is set by exe_elf.c, that contained two
   module_info struct, first for exe binary file info, second for this
   bootstrap dll, don't changed this define and do stupid things!!
*/
/*struct module_info {
	Elf32_Addr	addr;		//* ==> l_addr /
	Elf32_Addr	start;		//* ==> l_map_start /
	Elf32_Addr	end;		//* ==> l_map_end /
	Elf32_Addr	entry;		//* ==> l_entry /
	
	Elf32_Phdr	*phdr;		//* ==> l_phdr /
	Elf32_Dyn	*ld;		//* ==> l_ld /

	unsigned int alloced;	//* 1 -> phdr alloced, 0 -> phdr in PT_LOAD /
	
	Elf32_Half	nphdr;		//* ==> l_phnum /
	Elf32_Half	nld;		//* ==> l_ldnum /
};*/

#ifndef MODULE_INFO_START

/* module info size, now is 4k bytes */
#define MODULE_INFO_SIZE			0x1000
/* the module info in the module local virtual address */
#define MODULE_INFO_START			(0x10000)
/* module and his dependence dll info */
#define MODULE_LINK_INFO			(MODULE_INFO_START+0x400)
/* the current exe module phdr start address*/
#define MODULE_EXE_PHDR_START		(MODULE_INFO_START+0x800)
/* iff current exe module need interpret, the interpret phdr start address */
#define MODULE_DLL_PHDR_START		(MODULE_INFO_START+0xc00)

#endif //MODULE_INFO_START
/* this is dynamic link bootstrap function which is called by os application loader(exe_elf.c)
   the param:args is the exe and dll info in memory which is set by application loader.
   you can't call any functions and global variables before relocate itself.
   Take Care!
*/

//static ElfW(Addr) __dlstartup (void *args)
//ElfW(Addr) _start (void *args)
//ElfW(Addr) dldyn_start (void *args)
struct __dlmodule	*dldyn_start (void *args, char *AppName)
{
	//struct __dlmodule	bootstrap;
	//struct __dlmodule	*dll_module;
	Elf32_Phdr			*phdr;
	size_t				cnt;
	struct module_info	*exe_info;
	//struct module_info	*dll_info;

//	struct module_info	*exe_info = (struct module_info *)args;
//	struct module_info	*dll_info = exe_info + 1;

	//puthex_virtual(0xffff0001);
	//puthex_virtual((UINT)_start);
	//__dlget_dynamic_info (&bootstrap);

	RETAILMSG(TONE_DLL,  ("-----------Now in Dynamic linker -----------------\n"));
	RETAILMSG(TONE_DLL,  ("AppName: %s\n", AppName));

	exe_info = (struct module_info *)args;
	//dll_info = exe_info + 1;

	/* initialize bootstrap info */
	/*bootstrap.l_addr		= dll_info->addr;
	bootstrap.l_map_start	= dll_info->start;
	bootstrap.l_map_end		= dll_info->end;
	bootstrap.l_ld			= dll_info->ld;
	bootstrap.l_ldnum		= dll_info->nld;
	bootstrap.l_phdr		= dll_info->phdr;
	bootstrap.l_phnum		= dll_info->nphdr;
*/

	/* Partly clean the `bootstrap' structure up.  Don't use `memset'
	since it might nor be built in or inlined and we cannot make function
	calls at this point.  */
//	for (cnt = 0; cnt < sizeof (bootstrap.l_info) / sizeof (bootstrap.l_info[0]); ++cnt)
//		bootstrap.l_info[cnt] = 0;

	
	
	/* __dlget_dynamic_info is inline function, so you can call it */
//	if (! __dlget_dynamic_info (&bootstrap))
//		return 0;

	/* Relocate ourselves so we can do normal function calls and
	data access using the global offset table.  */
	

	//ELF_DYNAMIC_RELOCATE (&bootstrap, 0);

/**	if (0)//(bootstrap.l_info[DT_INIT] != NULL)||(bootstrap.l_info[DT_INIT_ARRAY] != NULL))
	{
		/\* Type of the initializer.  *\/
		typedef void (*init_fun) (int, char **, char **);
		/\* Now run the local constructors.  There are two forms of them:
		- the one named by DT_INIT
		- the others in the DT_INIT_ARRAY.
		*\/
		if (bootstrap.l_info[DT_INIT] != NULL)
		{
			((init_fun)(bootstrap.l_addr + bootstrap.l_info[DT_INIT]->d_un.d_ptr))(0, NULL, NULL);
		}

		/\* Next see whether there is an array with initialization functions.  *\/
		if (bootstrap.l_info[DT_INIT_ARRAY] != NULL)
		{
			unsigned int j;
			unsigned int jm;
			ElfW(Addr) *addrs;
			
			jm = bootstrap.l_info[DT_INIT_ARRAYSZ]->d_un.d_val / sizeof (ElfW(Addr));
			
			addrs = (ElfW(Addr) *) (bootstrap.l_info[DT_INIT_ARRAY]->d_un.d_ptr
				+ bootstrap.l_addr);
			for (j = 0; j < jm; ++j)
				((init_fun) addrs[j]) (0, NULL, NULL);
		}
		bootstrap.l_init_called = 1;
	}
**/
//	bootstrap.l_relocated = 1;

	/* Now life is sane; we can call functions and access global data.
	Set up to use the operating system facilities, and find out from
	the operating system's program loader where to find the program
	header table in core. */
	
	//puthex_virtual(0x00000003);

	RETAILMSG(TONE_DLL,  ("******************Now Dynamic link OK*******************\n"));

	//puthex_virtual(0x00000004);

	//puthex_virtual(0x5555aaaa);
	//puthex_virtual(__dlmalloc);

	/* Create a __dlmodule for the executable itself. */
	__dl_loaded = (struct __dlmodule *)__dlmalloc (sizeof(struct __dlmodule));

	

	if (__dl_loaded == NULL)
	{
		RETAILMSG(TONE_ERROR,  ("can't alloc memory!\n"));
		__dlexit (1);
		return NULL;
	}
	memset (__dl_loaded, 0, sizeof(struct __dlmodule));

	/* create a __dlmodule for interpret */
/*	dll_module = (struct __dlmodule *)__dlmalloc (sizeof(struct __dlmodule));
	if (dll_module == NULL)
	{
		RETAILMSG(TONE_DLL,  ("can't alloc memory!\n");
		__dlexit (1);
		return 0;
	}
	memset (dll_module, 0, sizeof(struct __dlmodule));
	memcpy (dll_module, &bootstrap, sizeof(struct __dlmodule));
*/
	

	/* initialize exe binary info in memory block */
	__dl_loaded->l_addr		= exe_info->addr;
	__dl_loaded->l_entry	= exe_info->entry;
	__dl_loaded->l_map_start= exe_info->start;
	__dl_loaded->l_map_end	= exe_info->end;

	__dl_loaded->l_phdr		= exe_info->phdr;
	__dl_loaded->l_phnum	= exe_info->nphdr;
	__dl_loaded->l_ld		= exe_info->ld;
	__dl_loaded->l_ldnum	= exe_info->nld;

	RETAILMSG(TONE_DLL,  ("__dl_loaded->l_entry: %x\n", __dl_loaded->l_entry));

	{		
		unsigned int nsize = strlen(AppName);
		char *dll_name = (char *)__dlmalloc(nsize+1);
		if (dll_name == NULL)
		{
			RETAILMSG(TONE_ERROR,  ("can't alloc memory!\n"));
			__dlexit (1);
			return 0;
		}
		memcpy (dll_name, AppName, nsize);
		dll_name[nsize] = '\0';
		
		__dl_loaded->l_origin = dll_name;
		dll_name = strrchr (__dl_loaded->l_origin, '\\');
		if (dll_name == NULL)
		{
			__dl_loaded->l_name = __dl_loaded->l_origin;
			__dl_loaded->l_origin = NULL;
			RETAILMSG(TONE_DLL, ("__dl_loaded->l_name: %s\n", __dl_loaded->l_name));
		}
		else
		{
			*dll_name = '\0';
			__dl_loaded->l_name = ++dll_name;
			RETAILMSG(TONE_DLL, ("__dl_loaded->l_name: %s\n", __dl_loaded->l_name));
		}
	}
	/* Put the __dlmodule for ourselves on the chain so it can be found by
	name.  Note that at this point the global chain of link maps contains
	exactly one element, which is pointed to by __dl_loaded.  */
//	__dl_loaded->l_next		= dll_module;
	__dl_loaded->l_next		= NULL;

//	dll_module->l_prev		= __dl_loaded;
//	dll_module->l_next		= NULL;

	__dl_loaded->l_type		= lt_executable;
//	dll_module->l_type		= lt_library;

	__dl_loaded->l_opencount= 1;
//	__dl_loaded->l_opencount= 1;

//	__dl_nloaded = 2; /* contain exe and interpret */
	__dl_nloaded = 1; /* contain exe and interpret */
	
	/* Extract the contents of the dynamic section for easy access.  */
	__dlget_dynamic_info (__dl_loaded);

	/* Set up our cache of pointers into the hash table.  */
	if (__dl_loaded->l_info[DT_HASH])
		__dlsetup_hash (__dl_loaded);

//	if (dll_module->l_info[DT_HASH])
//		__dlsetup_hash (dll_module);
/*
	for (phdr = exe_info->phdr, cnt = 0; cnt < exe_info->nphdr; cnt ++, phdr ++)
	{
		if (phdr->p_type == PT_INTERP)
		{
extern char * strrchr(const char *s, int i);

			unsigned int nsize = phdr->p_memsz;
			char *dll_name = (char *)__dlmalloc(nsize+1);
			if (dll_name == NULL)
			{
				RETAILMSG(TONE_DLL,  ("can't alloc memory!\n");
				__dlexit (1);
				return 0;
			}
			memcpy (dll_name, (void*)(phdr->p_vaddr + __dl_loaded->l_addr), nsize);
			dll_name[nsize] = '\0';

			dll_module->l_origin = dll_name;
			dll_name = strrchr (dll_module->l_origin, '\\');
			if (dll_name == NULL)
			{
				dll_module->l_name = dll_module->l_origin;
				dll_module->l_origin = NULL;
			}
			else
			{
				*dll_name = '\0';
				dll_module->l_name = ++dll_name;
			}

			break;
		}
	}
*/
	/* Load all the libraries specified by DT_NEEDED entries.*/
	//__dlopen_dependence (__dl_loaded, RTLD_NOW|RTLD_LOAD);

	// if can't load all libs of dependence, so kill it..
	if(__dlopen_dependence (__dl_loaded, RTLD_NOW|RTLD_LOAD) < 0)
		return NULL;

	{
		/* Now we have all the objects loaded.  Relocate them all except for
		the dynamic linker itself.  We do this in reverse order so that copy
		relocs of earlier objects overwrite the data written by later
		objects.  We do not re-relocate the dynamic linker itself in this
		loop because that could result in the GOT entries for functions we
		call being changed, and that would break us.  It is safe to relocate
		the dynamic linker out of order because it has no copy relocs (we
		know that because it is self-contained).  */
		struct __dlmodule *module = __dl_loaded;

		RETAILMSG(TONE_DLL, ("==> 111\n"));

		while (module->l_next)
			module = module->l_next;

		do
		{
			//if (module != dll_module){
				RETAILMSG(TONE_DLL, ("==> 120\n"));

				RETAILMSG(TONE_DLL, ("==> module name: %s\n", module->l_name));

				__dlrelocate_module (module, module->l_scope, RTLD_NOW);

				RETAILMSG(TONE_DLL, ("==> 121\n"));
			//}
	
			module = module->l_prev;
		}
		while (module);
	  
		RETAILMSG(TONE_DLL, ("==> 112\n"));

//		if (dll_module->l_opencount > 1)
//		{
			/* There was an explicit ref to the dynamic linker as a shared lib.
			Re-relocate ourselves with user-controlled symbol definitions.  */
//			__dlrelocate_module (dll_module, __dl_loaded->l_scope, 0);
//		}
		{
			unsigned int *p = (unsigned int *)MODULE_LINK_INFO;
			*p = (unsigned int)__dl_loaded;
			p ++;
			*p = (unsigned int)&__dl_nloaded;
//			((struct __dlmodule *)MODULE_LINK_INFO) = __dl_loaded;
//			((unsigned int *)(MODULE_LINK_INFO+4)) = &__dl_nloaded;
		}
	}
/*	{
		/\* Initialize _r_debug.  *\/
		struct __dldebug *r = __dldebug_initialize (dll_module->l_addr);
		struct __dlmodule *module = __dl_loaded;
	
		if (module->l_info[DT_DEBUG])
		{
			/\* There is a DT_DEBUG entry in the dynamic section.  Fill it in
			with the run-time address of the r_debug structure  *\/
			module->l_info[DT_DEBUG]->d_un.d_ptr = (ElfW(Addr)) r;
		}
		/\* Fill in the pointer in the dynamic linker's own dynamic section, in
		case you run gdb on the dynamic linker directly.  *\/
		if (dll_module->l_info[DT_DEBUG])
			dll_module->l_info[DT_DEBUG]->d_un.d_ptr = (ElfW(Addr)) r;
		
		/\* Notify the debugger that all objects are now mapped in.  *\/
		r->r_state = RT_ADD;
		__dldebug_state ();
	}
*/
	/* call exe start code */
	/*{
		typedef int (*__startup_entry)(void);

		__startup_entry startup = (__startup_entry)(__dl_loaded->l_entry);

		RETAILMSG(TONE_DLL,  ("******************Dynamic call _start:0x%x*****************\n", startup);
		return startup();
	}*/

	RETAILMSG(TONE_DLL,  ("__dl_loaded->l_entry: %x\n", __dl_loaded->l_entry));

	return __dl_loaded;
}

