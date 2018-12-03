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
#include <romheader.h>	//Add by zb.
#include <epcore.h>
#include <edevice.h>	//Add by zb.
#include <eassert.h>	//Add by zb.
#include <estring.h>	//Add by zb.
#include <module.h>		//Add by zb.

#include <coresrv.h>
#include <diskio.h>
//#define __dlmalloc		malloc
//#define __dlfree		free
#define __dlmalloc		_kalloc
#define __dlfree		_kfree

//#define RETAILMSG(TONE_DLL, 		EdbgOutputDebugString
#define __dlexit(exp)
#define __dlassert(exp)	ASSERT(exp)

//#define		ROM_MODULE		1
//#define		ROM_FILE		2
//#define		RAM_FILE		3


#include "dlstruct.h"

ElfW(Addr) __dllookup_value (const char *name, struct __dlmodule *module);

#include <dlarch.h>
#include "dldyn.h"

#define INVALID_SET_FILE_POINTER		1
#define ALIGN_SIZE						0x10000

#define ALIGN_ELF_SIZE					0x1000

#define ALIGN(addr,align)				(((addr))&(~(align-1)))
#define ALIGNUP(addr,align)				(((addr)+(align)-1)&(~(align-1)))

//#define PAGE_WRITECOPY					1
////////////////////////////////////////////////////////////////////////
typedef void (*__dlinit_fun) (int, char **, char **);
typedef void (*__dlfinit_fun)(void);

extern struct __dlmodule *__dl_loaded;
extern unsigned int		 __dl_nloaded;

////////////////////////////////////////////////////////////////////////

//NOTE:2003-09-04...ZB
// fix __dlclean_module()'s free bug.
//unsigned int __dlclean_module (struct __dlmodule *module);
unsigned int __dlclean_module (struct __dlmodule *module, BOOL bNested);
//NOTE:2003-09-04...ZB

struct __dlmodule * __dlload_module (PMODULE_HANDLE lpHandle, const char *name);

void __dlsetup_hash (struct __dlmodule* module);
int __dlopen_dependence (struct __dlmodule* module, unsigned int mode);
//---------------------------------------------------------------------
static ElfW(Addr)  __dllookup_symbol (const char *name, struct __dlmodule *module,
									const ElfW(Sym) **ref, struct __dlscope_elem *symbol_scope[],
									int reloc_type, int explicit_mode);

static void __dlcall_init (struct __dlmodule *module, int argc, char **argv, char **env);
static void __dlinit_module (struct __dlmodule *module, int argc, char **argv, char **env);

static struct __dlmodule * __dlmap_module (HANDLE hFile, ElfW(Ehdr) *ehdr, ELF_INFO *pElfinfo, PMODULE_HANDLE lpHandle,
											UINT flag);
static struct __dlmodule * __dlmap_rom_module (HANDLE hFile, ElfW(Ehdr) *ehdr, ELF_INFO *pElfinfo);
static struct __dlmodule* __dlopen_module (const char * name, unsigned int mode, struct __dlmodule *caller,
						   PMODULE_HANDLE lpHandle);

static char * __dlfind_module (const char *name, struct __dlmodule *caller);
static unsigned int __dlcheck_elf (const Elf32_Ehdr *ehdr);

static unsigned int __dlhash (const unsigned char *name);
static int __dllookup (const char *undef_name, unsigned long int hash,
					   const ElfW(Sym) *ref,
						struct __dlsym_val *result,
						struct __dlscope_elem *scope, size_t i,
						struct __dlmodule *skip, int noexec, int noplt);
////////////////////////////////////////////////////////////////////////
UINT GetSharedDirectory (LPSTR lpBuff, UINT nSize)
{
	const LPSTR lpDir = "\\system\\shared";
	UINT nLen = strlen (lpDir);
	if (nLen < nSize)
		strcpy (lpBuff, lpDir);

	return nLen;
}

#undef GetSystemDirectory
UINT GetSystemDirectory (LPSTR lpBuff, UINT nSize)
{
	const LPSTR lpDir = "\\system";
	UINT nLen = strlen (lpDir);

	if (nLen < nSize)
		strcpy (lpBuff, lpDir);

	return nLen;
}
char *strdup(const char *p)
{
	size_t n;
	char *dup;

	RETAILMSG(TONE_DLL,  ("--In strdup \n"));

	n = strlen(p);
	dup = (char *)__dlmalloc(n + 1);
	
	if (dup == NULL)
		return NULL;
	memset (dup, 0, n+1);
	memcpy (dup, p, n);
	return dup;
}

static unsigned int __dlfileexist (const char *p)
{
/*	WIN32_FIND_DATA	wfd;

	HANDLE hFind;
	
	hFind = FindFirstFile (p, &wfd);
*/	
	DWORD dwAttr;

	RETAILMSG(TONE_DLL,  ("--In * __dlfileexist: %s \n", p));

	RETAILMSG(TONE_DLL,  ("--In * __dlfileexist %x \n", GetFileAttributes));
	
	RETAILMSG(TONE_DLL,  ("--get GetFileAttributes : %x \n", GetFileAttributes));

	dwAttr = GetFileAttributes (p);

	//RETAILMSG(TONE_DLL,  ("--file %s attribute: %x \n", dwAttr);

	//if (dwAttr == (DWORD)-1)
	if(dwAttr == (DWORD)12345678)
		return 0;
	if (dwAttr&FILE_ATTRIBUTE_DIRECTORY)
		return 0;

	return 1;
}
////////////////////////////////////////////////////////////////////////

static void __dlcall_init (struct __dlmodule *module, int argc, char **argv, char **env)
{
	if (module->l_init_called)
		return;

	module->l_init_called = 1;
	
	if (module->l_type == lt_executable)
		return;
	
	if (module->l_info[DT_INIT] == NULL
		&& module->l_info[DT_INIT_ARRAY] == NULL)
		return;
	
	if (module->l_info[DT_INIT] != NULL)
    {
		__dlinit_fun init = (__dlinit_fun)(module->l_addr + module->l_info[DT_INIT]->d_un.d_ptr);
		
		init (argc, argv, env);
    }
	
	if (module->l_info[DT_INIT_ARRAY] != NULL)
    {
		unsigned int i;
		unsigned int nfun;
		ElfW(Addr) *addrs;
		
		nfun = module->l_info[DT_INIT_ARRAYSZ]->d_un.d_val / sizeof (ElfW(Addr));
		
		addrs = (ElfW(Addr) *) (module->l_info[DT_INIT_ARRAY]->d_un.d_ptr
			+ module->l_addr);
		for (i = 0; i < nfun; ++i)
			((__dlinit_fun) addrs[i]) (argc, argv, env);
    }
}


static void __dlinit_module (struct __dlmodule *module, int argc, char **argv, char **env)
{
	unsigned int i;
	/* Stupid users forced the ELF specification to be changed.  It now
	says that the dynamic loader is responsible for determining the
	order in which the constructors have to run.  The constructors
	for all dependencies of an object must run before the constructor
	for the object itself.  Circular dependencies are left unspecified.
	
	This is highly questionable since it puts the burden on the dynamic
	loader which has to find the dependencies at runtime instead of
	letting the user do it right.  Stupidity rules!  */
	
	i = module->l_searchlist.r_nlist;
	while (i-- > 0)
		__dlcall_init (module->l_initfini[i], argc, argv, env);
}

static unsigned int __dlhash (const unsigned char *name)
{
	unsigned long int hash = 0;

	//The below is added by zb
	if(name == NULL)
		return hash;

	if (*name != '\0')
    {
		hash = *name++;
		if (*name != '\0')
		{
			hash = (hash << 4) + *name++;
			if (*name != '\0')
			{
				hash = (hash << 4) + *name++;
				if (*name != '\0')
				{
					hash = (hash << 4) + *name++;
					if (*name != '\0')
					{
						hash = (hash << 4) + *name++;
						while (*name != '\0')
						{
							unsigned long int hi;
							hash = (hash << 4) + *name++;
							hi = hash & 0xf0000000;
							
							/* The algorithm specified in the ELF ABI is as
							follows:
							
							  if (hi != 0)
							  hash ^= hi >> 24;
							  
								hash &= ~hi;
								
								  But the following is equivalent and a lot
							faster, especially on modern processors.  */
							
							hash ^= hi;
							hash ^= hi >> 24;
						}
					}
				}
			}
		}
    }
	return hash;
}

void __dlsetup_hash (struct __dlmodule* module)
{
	Elf_Symndx *hash;
	Elf_Symndx nchain;
	
	if (!module->l_info[DT_HASH])
		return;
	hash = (void *)(module->l_addr + module->l_info[DT_HASH]->d_un.d_ptr);
	
	module->l_nbuckets = *hash++;
	nchain = *hash++;
	module->l_nchain = nchain;	//zb added...
	module->l_buckets = hash;
	hash += module->l_nbuckets;
	module->l_chain = hash;
}
/* read file from the offset, return readed bytes
*/
static __inline unsigned int read_file (HANDLE hFile, char *buff, unsigned int size,
										unsigned int offset)
{
	unsigned int	read;

	__dlassert (buff != NULL);

	if (SetFilePointer (hFile, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return 0;

	if (! ReadFile (hFile, buff, size, (LPDWORD)&read, NULL))
		return 0;

	return read;
}

/* lookup functions.  We return a value > 0 if we
   found the symbol, the value 0 if nothing is found and < 0 if
   something bad happened.  */
#define DEBUG_dllookup 0

static int __dllookup (const char *undef_name, unsigned long int hash, const ElfW(Sym) *ref,
     struct __dlsym_val *result, struct __dlscope_elem *scope, size_t i,
     struct __dlmodule *skip, int noexec, int noplt)
{
	struct __dlmodule **list = scope->r_list;
	size_t n = scope->r_nlist;
	struct __dlmodule *module;
	
	//RETAILMSG(TONE_DLL, ("scope->r_nlist: %x\r\n", scope->r_nlist);

	do
	{
		const ElfW(Sym)		*symtab;
		const char			*strtab;
//		const ElfW(Half)	*verstab;
		Elf_Symndx			symidx;
		const ElfW(Sym)		*sym;

		module = list[i];

		/* Here come the extra test needed for `__dllookup_symbol_skip'.  */
		if (skip != NULL && module == skip)
			continue;

		/* Don't search the executable when resolving a copy reloc.  */
		if (noexec && module->l_type == lt_executable)
			continue;

//		RETAILMSG(TONE_DLL,  ("-->hash value: %x\r\n", hash);
		
		symtab = (const void *) D_PTR (module, l_info[DT_SYMTAB]);
		strtab = (const void *) D_PTR (module, l_info[DT_STRTAB]);
//		verstab = module->l_versyms;

		/* Search the appropriate hash bucket in this object's symbol table
		for a definition for the same symbol name.  */

		DEBUGMSG( DEBUG_dllookup,  ("__dllookup: lookup in module: %s.\r\n", module->l_name ) );

		for (symidx = module->l_buckets[hash % module->l_nbuckets];
			symidx != STN_UNDEF;
			symidx = module->l_chain[symidx])
		{
			sym = &symtab[symidx];


			 /* No value.  */
			if (sym->st_value == 0 || ((noplt||module->l_type == lt_executable) && sym->st_shndx == SHN_UNDEF))
				continue;
			
			/* Ignore all but STT_NOTYPE, STT_OBJECT and STT_FUNC entries
			since these are no code/data definitions.  */
			if ( ELF32_ST_TYPE(sym->st_info) > STT_FUNC)
				continue;
			
			/* Not the symbol we are looking for.  */
			if (sym != ref && strcmp (strtab + sym->st_name, undef_name))
				continue;
			
			/* There cannot be another entry for this symbol so stop here.  */
			switch (ELF32_ST_BIND(sym->st_info))
			{
			case STB_WEAK:
			case STB_GLOBAL:
				/* Global definition.  Just what we need.  */
				RETAILMSG(TONE_DLL,  ("undef_name: %s, module->l_name: %s\n", undef_name, module->l_name));

				result->s = sym;
				result->m = module;
				return 1;
			default:
				/* Local symbols are ignored.  */
				break;
			}
		}
    }
	while (++i < n);
	
	/* We have not found anything until now.  */
	return 0;
}
/* find the module position
return NULL if can't find
*/
static char *__dlfind_module (const char *name, struct __dlmodule *caller)
{
	const char *file;
	char dir[MAX_PATH];
	const char *p = name;

	RETAILMSG(TONE_DLL,  ("--0want find module %s \n", name));

	RETAILMSG(TONE_DLL,  ("--len %d \n", strlen(name)));

	if (__dlfileexist (name))
	{
		RETAILMSG(TONE_DLL,  ("--find module %s OK\n", name));
		return strdup (name);
	}

	

//	if (caller == NULL)
//		return NULL;
	while(*p){
		if(*p == '\\')	//是绝对路径，没有找到就return...
			return 0;
		else
			p ++;
	}

//	file = strrchr (name, '\\');
//	if (file == NULL)
//		file = name; /* name without directory info */
//	else{ 		
		//file ++; /* skip '\\' */

//		RETAILMSG(TONE_DLL,  ("--have path! \n"));
//		return NULL;	//if with directory info... 
//	}

/*
	if (caller && caller->l_origin != NULL)
	{
		RETAILMSG(TONE_DLL,  ("--origin : %s \n", caller->l_origin));

		strcpy (dir, caller->l_origin);
		strcat (dir, "\\");
		strcat (dir,file);
		if (__dlfileexist (dir))
			return strdup (dir);
	}
*/

/*	GetCurrentDirectory (MAX_PATH, dir);
	strcat (dir, "\\");
	strcat (dir, file);
	if (__dlfileexist (dir))
		return strdup (dir);
*/
	/* search shared directory */
/*	GetSharedDirectory (dir, MAX_PATH);
	strcat (dir, "\\");
	strcat (dir, file);
	if (__dlfileexist (dir))
		return strdup (dir);
*/

	RETAILMSG(TONE_DLL,  ("--find kingmos.... \n"));
	// search in kingmos...
	//strcpy(dir, "\\kingmos\\");
	strcpy (dir, KINGMOS_PATH);
//	strcat(dir, file);
	strcat(dir, name);
	if (__dlfileexist (dir))
		return strdup (dir);
	
	RETAILMSG(TONE_DLL,  ("--find system....\n"));
	/* search system directory */
//	GetSystemDirectory (dir, MAX_PATH);
//	strcat (dir, "\\");
	strcpy (dir, WORK_PATH);
//	strcat (dir, file);
	strcat(dir, name);
	if (__dlfileexist (dir))
		return strdup (dir);


	return NULL;
}

unsigned int __dlcheck_elf (const Elf32_Ehdr *ehdr)
{
	const unsigned char ELF_HEADER_MARK[EI_PAD] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	__dlassert (ehdr != NULL);
	
	if (memcmp (ehdr->e_ident, ELF_HEADER_MARK, 4) != 0)
		return 0; /* not elf file */

	if (ehdr->e_version != EV_CURRENT)
		return 0; /* version is not current */

	if (ehdr->e_phentsize != sizeof (Elf32_Phdr))
		return 0; /* phdr size is not we want */

	if (! __dlarch_matches_host (ehdr))
		return 0; /* determine the binary code compatible */

	if (ehdr->e_type != ET_DYN && ehdr->e_type != ET_EXEC)
		return 0; /* just execute and dynamic binary can be load */
		
	return 1;
}

/* open the module dependence modules, whitch is in DT_NEEDED
   set the module's scope and searchlist and finit info
*/
int __dlopen_dependence (struct __dlmodule* module, unsigned int mode)
{
	Elf32_Dyn		*dyn;
	unsigned int	nneeded = 1; /* set one for self contained module safe */
	const char		*strtab = (const void *) D_PTR (module, l_info[DT_STRTAB]);

	__dlassert (module != NULL);

	RETAILMSG(TONE_DLL,  ("__dlopen_dependence: < !\n"));

/*	RETAILMSG(TONE_DLL,  ("*******************************************************\n");
	RETAILMSG(TONE_DLL,  ("--get GetFileAttributes : %x \n", GetFileAttributes);
	RETAILMSG(TONE_DLL,  ("--get file attr : %x \n", GetFileAttributes ("libmydll.so"));
	RETAILMSG(TONE_DLL,  ("--get file attr : %x \n", GetFileAttributes ("\\system\\hello.exe"));
	RETAILMSG(TONE_DLL,  ("*******************************************************\n");
*/	
	if (module->l_ld == NULL)
		return 0; /* if module have not dynamic info, it must be execute binary, ignore it */

	for (dyn = module->l_ld; dyn->d_tag != DT_NULL; dyn ++)
	{
		if (dyn->d_tag == DT_NEEDED)
			nneeded ++;
	}
	
	//RETAILMSG(TONE_TEST ,  ("__dlopen_dependence: nneeded:%x !\n", nneeded));

	module->l_initfini = (struct __dlmodule **)__dlmalloc ((2*nneeded+1) * sizeof(struct __dlmodule*));
	if (module->l_initfini == NULL)
	{
		RETAILMSG(TONE_ERROR,  ("can't alloc memory !\n"));
		return -1;
	}

	memset (module->l_initfini, 0, (2*nneeded+1) * sizeof(struct __dlmodule*));
	module->l_searchlist.r_list = module->l_initfini + nneeded + 1;
	module->l_searchlist.r_nlist = nneeded;
	module->l_searchlist.r_list[0] = module;
	module->l_scope[0] = &module->l_searchlist; /* the first scope is the searchlist */

	if (nneeded != 1)
	{
		nneeded = 1;  /* the first is the module self */
		for (dyn = module->l_ld; dyn->d_tag != DT_NULL; dyn ++)
		{
			if (dyn->d_tag == DT_NEEDED)
			{
				extern PMODULE_HANDLE FindLoadedHandle(PMODULE_NODE pMod, UINT flag);
				extern PMODULE_NODE FindOrCreateModule(LPCTSTR lpszFileName, UINT *uResult, UINT flag);
				extern CRITICAL_SECTION csModuleList;	//module list 临界段
				UINT		uResult;
				const char *need = (const char *)(strtab + dyn->d_un.d_val);
				PMODULE_NODE	pMod = NULL ;				
				PMODULE_HANDLE	ptemp;
				
				RETAILMSG(TONE_DLL,  ("--depend library : %s <\n", need));				

				EnterCriticalSection( &csModuleList );	 
				
				//pMod = FindOrCreateModule(need, NULL, MODULE_EXEC);
				pMod = FindOrCreateModule(need, NULL, CREATE_LIBRARY);
				
				LeaveCriticalSection( &csModuleList );	 

				//if(uResult == NEW_MODULE){				
				//if(pMod->lpBase == NULL){
				ptemp = FindLoadedHandle(pMod, FIND_NON_XIP_REF);

				if(ptemp == NULL){
					RETAILMSG(TONE_TEST, ("->%s: load new %s\n", module->l_name, need));

					module->l_searchlist.r_list[nneeded] = __dlopen_module (need, mode, module, NULL);
					RETAILMSG(TONE_DLL,  ("__dlopen_dependence: > 1100 !\n"));
					
				}else{
					RETAILMSG(TONE_TEST, ("->%s: ref %s\n", module->l_name, need));

					//module->l_searchlist.r_list[nneeded] = __dlopen_module (need, mode, module, pMod->lpBase);
					module->l_searchlist.r_list[nneeded] = __dlopen_module (need, mode, module, ptemp);

					RETAILMSG(TONE_DLL,  ("__dlopen_dependence: > 1200 !\n"));					
				}
				

				if (module->l_searchlist.r_list[nneeded] == NULL)
				{
					RETAILMSG(TONE_ERROR,  ("can't open dependence module !\n"));
					return -1;
				}
				nneeded ++;
			}
		}
	}
	RETAILMSG(TONE_DLL,  ("__dlopen_dependence: 1101 !\n"));

	//RETAILMSG(TONE_TEST ,  ("__dlopen_dependence: r_nlist:%x:%s !\n", module->l_searchlist.r_nlist,module->l_name));

	memcpy (module->l_initfini, module->l_searchlist.r_list,
		sizeof(struct __dlmodule*)*module->l_searchlist.r_nlist);

	RETAILMSG(TONE_DLL,  ("__dlopen_dependence: > !\n"));

	return --nneeded;
}

extern unsigned int read_file (HANDLE hFile, char *buff, unsigned int size,unsigned int offset);

/* map a binary file to memory */


static struct __dlmodule * __dlmap_module (HANDLE hFile, ElfW(Ehdr) *ehdr, ELF_INFO *pElfinfo, PMODULE_HANDLE lpHandle,
											UINT flag)
{
	struct __dlmodule	*module;
	Elf32_Phdr			*phdr = NULL;
	unsigned int		size;
	int		inload = 0;

	Elf32_Phdr			*lp = NULL;
	unsigned int		i;
	BOOL				bMapModule = FALSE;
	int					indexPtLoad = 0;
	DWORD				flAllocationType;
	DWORD				flflag;
	//struct module_info	*info;
	PLOAD_MODULE		info;
	Elf32_Addr			addr_start, addr_end = 0, addr_segstart;
	BOOL				bRomModule = FALSE;

	RETAILMSG(TONE_DLL,  ("__dlmap_module: <\n"));

	if(flag == XIP_MODULE)
		bRomModule = TRUE;

	else if(lpHandle != NULL){
		bMapModule = TRUE;
		info = lpHandle->info;

		RETAILMSG(TONE_DLL,  ("module_info: -------------\n"));
		RETAILMSG(TONE_DLL,  ("info->addr: %x\n", info->addr));
		RETAILMSG(TONE_DLL,  ("info->start: %x\n", info->start));
		RETAILMSG(TONE_DLL,  ("info->end: %x\n", info->end));
		RETAILMSG(TONE_DLL,  ("info->entry: %x\n", info->entry));
		//RETAILMSG(TONE_DLL,  ("info->addr: %x\n", info->addr));
		RETAILMSG(TONE_DLL,  ("module_info: -------------\n"));
	}else{
		bMapModule = FALSE;
	}

	if(flag == XIP_MODULE){
		phdr = (Elf32_Phdr *)(pElfinfo->ulFileBase + pElfinfo->ulPhdroff);
		if (phdr == NULL)
			return NULL;		
	}else{
		size = ehdr->e_phentsize * ehdr->e_phnum;
		
		phdr = (Elf32_Phdr *)__dlmalloc (size);
		if (phdr == NULL)
			return NULL;
		
		/* first load phdr info */
		if (read_file (hFile, (char*)phdr, size, ehdr->e_phoff) != size)
		{
			__dlfree (phdr);
			return NULL;
		}		
	}

	for (lp = phdr, i = 0; i < ehdr->e_phnum; i ++, lp ++)
	{
		if(lp->p_vaddr == 0)		//if the p_vaddr is 0, i think that is invalid phdr record...
			continue;

		switch (lp->p_type)
		{
		case PT_LOAD:
			if((++indexPtLoad) >= PT_LOAD_RES)		//.res section isn't loaded in memory....
				break;

			if (((lp->p_vaddr - lp->p_offset)&(lp->p_align - 1)) != 0)	//???
			{
				RETAILMSG(TONE_DLL,  ("loadable segment is not align!\n"));
				__dlfree (phdr); /* not align */
				return NULL;
			}

			RETAILMSG(TONE_DLL,  ("__dlmap_module: lp->p_align:%x\n", lp->p_align));

			//ZB changed...
			//addr_segstart = ALIGN (lp->p_vaddr, lp->p_align);
			addr_segstart = lp->p_vaddr;

			addr_end = addr_end > (addr_segstart + lp->p_memsz) ? addr_end : (addr_segstart + lp->p_memsz);

			RETAILMSG(TONE_DLL,  ("__dlmap_module: addr_segstart:%x, p_memsz:%x\n",addr_segstart, lp->p_memsz ));

			RETAILMSG(TONE_DLL,  ("__dlmap_module: addr_end:%x\n",addr_end));

			if (inload == 0)
			{	/* this is the first loadable phdr */
				addr_start = addr_segstart;
				/* addr_fstart = lp->p_offset; */

				inload = 1;
			}
			break;
		}
	}

//	RETAILMSG(TONE_DLL,  ("__dlmap_module: 11\n"));

	/* alloc module info memory */
	module = (struct __dlmodule *) __dlmalloc (sizeof (struct __dlmodule));
	if (module == NULL)
	{
		RETAILMSG(TONE_ERROR,  ("alloc memory for module info failure !\n"));
		__dlfree (phdr);
		return NULL;
	}
	memset (module, 0, sizeof(struct __dlmodule));
	module->l_phnum = ehdr->e_phnum;

//	if(flag == ROM_MODULE)
//		module->filetype = ROM_MODULE;
//	else
//		module->filetype = NORMAL_MODULE;
	//Zb add...
	module->hFile = hFile;

#ifdef WIN32_TEST
	/* on win32 platform, virtual meory must align 64k size,
	this just for test on win32 platform */
	addr_start = ALIGN(addr_start,ALIGN_SIZE);
#endif /* WIN32_TEST */

	size =  addr_end - addr_start;

	if(bMapModule||bRomModule){
		RETAILMSG(TONE_DLL,  ("__dlmap_module: ->11\n"));
		flAllocationType = MEM_RESERVE;
		//NOTE: 2003-08-26...ZB
		//PAGE_NOCACHE 会降低系统性能
		//flflag = PAGE_NOCACHE|PAGE_EXECUTE_READWRITE; 
		flflag = PAGE_EXECUTE_READWRITE; 
		//NOTE: 2003-08-26...ZB
	}else{
		flAllocationType = MEM_COMMIT|MEM_RESERVE;
		flflag = PAGE_EXECUTE_READWRITE; 
	}

	RETAILMSG(TONE_DLL,  ("__dlmap_module: 22\n"));

	if (ehdr->e_type == ET_EXEC)
	{ /* execute program must load at fix address */
		module->l_map_start = (ElfW(Addr))VirtualAlloc ((LPVOID)addr_start, size,			
			flAllocationType, flflag);
	}
	else
	{
		RETAILMSG(TONE_DLL,  ("__dlmap_module: ->12\n"));
		module->l_map_start = (ElfW(Addr))VirtualAlloc (NULL, size,
			flAllocationType, flflag);
	}

	if ((module->l_map_start == 0)
		||((module->l_map_start != addr_start)&&(ehdr->e_type == ET_EXEC)))
	{
		RETAILMSG(TONE_ERROR,  ("virtual alloc failure !\n"));
		__dlfree (module);
		__dlfree (phdr);
		return NULL;
	}
	
	module->l_map_end	= module->l_map_start + size;
	module->l_addr		= module->l_map_start - addr_start; /* in execute program this must be zero */
	module->l_entry		= ehdr->e_entry + module->l_addr;

	RETAILMSG(TONE_DLL,  ("module->l_map_start:%x \n", module->l_map_start));
	RETAILMSG(TONE_DLL,  ("module->l_map_end:%x \n", module->l_map_end));
	RETAILMSG(TONE_DLL,  ("module->l_addr:%x \n", module->l_addr));
	RETAILMSG(TONE_DLL,  ("module->l_entry:%x \n", module->l_entry));


	RETAILMSG(TONE_DLL,  ("__dlmap_module: 33\n"));

	indexPtLoad = 0;

	for (lp = phdr, i = 0; i < ehdr->e_phnum; i ++, lp ++)
	{
		if(lp->p_vaddr == 0)		//if the p_vaddr is 0, i think that is invalid phdr record...
			continue;

		switch (lp->p_type)
		{
		case PT_PHDR:
			module->l_phdr = (ElfW(Phdr) *)(lp->p_vaddr + module->l_addr);
			break;
		case PT_DYNAMIC:
			module->l_ld = (ElfW(Dyn) *)(lp->p_vaddr + module->l_addr);
			module->l_ldnum = lp->p_memsz / sizeof (ElfW(Dyn));
			break;
		case PT_LOAD:
			{
				unsigned int	nprotect;
				DWORD			oprotect;

				indexPtLoad++;
				
				if(indexPtLoad >= PT_LOAD_RES)		//.res section isn't loaded in memory....
					break;

				if (lp->p_flags & PF_R)
					nprotect = PAGE_READONLY;
				if (lp->p_flags & PF_W)
					nprotect = PAGE_READWRITE; //PAGE_WRITECOPY
				if (lp->p_flags & PF_X)
					nprotect = PAGE_EXECUTE_READWRITE; //PAGE_EXECUTE_WRITECOPY

				//NOTE: 2003-08-26...ZB
				//PAGE_NOCACHE 会降低系统性能
				//nprotect |= PAGE_NOCACHE;
				//NOTE: 2003-08-26...ZB

			if(bMapModule||bRomModule){
				RETAILMSG(TONE_DLL, ("module->l_map_start: %x, load addr:%x\n", module->l_map_start, lp->p_vaddr+module->l_addr));

				if(indexPtLoad == PT_LOAD_CODE){
					LPVOID	pDest ;

					if(bMapModule){
						RETAILMSG(TONE_DLL, ("LoadDll: -> 1\r\n"));
						pDest = MapPtrToProcess((LPVOID)info->start, lpHandle->hProc);
					}else{
						RETAILMSG(TONE_DLL, ("LoadDll: -> 2\r\n"));
						pDest = (LPVOID)(pElfinfo->ulFileBase + lp->p_offset);
					}
					//RETAILMSG(TONE_DLL, ("info->start: %x\n", info->start));

					RETAILMSG(TONE_DLL, ("LoadDll: -> 3\r\n"));

					if(!VirtualCopy((LPVOID)(lp->p_vaddr+module->l_addr), pDest, lp->p_memsz, nprotect)){
						
						RETAILMSG(TONE_ERROR, ("virtualcopy fail@\n"));
						goto cleanup;
					}
				}else if(indexPtLoad == PT_LOAD_DATA){

					RETAILMSG(TONE_DLL, ("LoadDll: -> 4\r\n"));

					VirtualAlloc ((LPVOID)(lp->p_vaddr+module->l_addr), lp->p_memsz,			
						MEM_COMMIT, PAGE_EXECUTE_READWRITE);
					
					if(bMapModule){
						/* read program loadable segment */
						if (read_file (hFile, (char*)(lp->p_vaddr+module->l_addr),
							lp->p_filesz, lp->p_offset) != lp->p_filesz)
							goto cleanup;
					}else{
						RETAILMSG(TONE_DLL, ("LoadDll: -> 5\r\n"));
						memcpy((void *)(lp->p_vaddr+module->l_addr), (void *)(pElfinfo->ulFileBase + lp->p_offset),
							lp->p_filesz);
					}					
				}
			}else{
				
				RETAILMSG(TONE_DLL,  ("__dlmap_module: 44\n"));

				/* read program loadable segment */
				if (read_file (hFile, (char*)(lp->p_vaddr+module->l_addr),
					lp->p_filesz, lp->p_offset) != lp->p_filesz)
					goto cleanup;				
			}

			if (lp->p_memsz > lp->p_filesz)
			{/* must set zero, some time this is the bss section */
				memset ((void*)(module->l_addr + lp->p_vaddr + lp->p_filesz), 
					0, lp->p_memsz-lp->p_filesz);
			}


			if ((module->l_phdr == 0)&&(lp->p_offset <= ehdr->e_phoff)
				&&((lp->p_filesz + lp->p_offset) 
				>= (ehdr->e_phoff + ehdr->e_phnum*sizeof(Elf32_Phdr))))
			{
				/* Found the program header in this segment.  */
				module->l_phdr = (Elf32_Phdr *)(ehdr->e_phoff + module->l_addr);

				RETAILMSG(TONE_DLL, ("found phdr at: %x!\n", module->l_phdr));
			}
			VirtualProtect ((LPVOID)(lp->p_vaddr+module->l_addr), lp->p_memsz, nprotect, &oprotect);
			break;
			}
		}
	}

	if ((module->l_phdr == NULL)||((Elf32_Addr)module->l_phdr < (module->l_addr + addr_start)))
	{
		ASSERT(0);
		/* the PT_PHDR segment is not load, load it (just reuse) */
		module->l_phdr = phdr;
		module->l_phdr_allocated = 1;

		RETAILMSG(TONE_DLL,  ("__dlmap_module: >22\n"));
		return module;
	}

	RETAILMSG(TONE_DLL,  ("__dlmap_module: >11\n"));

	if(!bRomModule)
		__dlfree (phdr);
	return module;

cleanup: /* goto cleanup must be used belowed the three resource be alloc */
	VirtualFree ((LPVOID)module->l_map_start, size, MEM_DECOMMIT);
	__dlfree (module);
	if(!bRomModule)
		__dlfree (phdr);
	return NULL;
}


/* map a binary file to memory */
/*static struct __dlmodule * __dlmap_rom_module (HANDLE hFile, ElfW(Ehdr) *ehdr, ELF_INFO *pElfinfo)
{
	struct __dlmodule	*module;
	Elf32_Phdr			*phdr = NULL;
	unsigned int		size;
	int					inload = 0;

	Elf32_Phdr			*lp = NULL;
	unsigned int		i;

	Elf32_Addr	addr_start, addr_end = 0, addr_segstart;

	size = ehdr->e_phentsize * ehdr->e_phnum;

	//phdr = (Elf32_Phdr *)__dlmalloc (size);
	phdr = (Elf32_Phdr *)(pElfinfo->ulFileBase + pElfinfo->ulPhdroff);
	if (phdr == NULL)
		return NULL;

	RETAILMSG(TONE_DLL, ("phdr: %x\n", phdr));
	

	for (lp = phdr, i = 0; i < ehdr->e_phnum; i ++, lp ++)
	{
		if(lp->p_vaddr == 0)		//if the p_vaddr is 0, i think that is invalid phdr record...
			continue;

		switch (lp->p_type)
		{
		case PT_LOAD:
			RETAILMSG(TONE_DLL, ("lp->p_vaddr: %x\n", lp->p_vaddr));
			RETAILMSG(TONE_DLL, ("lp->p_offset: %x\n", lp->p_offset));
			RETAILMSG(TONE_DLL, ("lp->p_align: %x\n", lp->p_align));
			RETAILMSG(TONE_DLL, ("lp->p_memsz: %x\n", lp->p_memsz));

			if (((lp->p_vaddr - lp->p_offset)&(lp->p_align - 1)) != 0)
			{
				RETAILMSG(TONE_DLL,  ("loadable segment is not align!\n"));
				__dlfree (phdr); // not align /
				return NULL;
			}
			//addr_segstart = ALIGN (lp->p_vaddr, lp->p_align);
			addr_segstart = lp->p_vaddr;

			addr_end = addr_end > (addr_segstart + lp->p_memsz) ? addr_end : (addr_segstart + lp->p_memsz);

			RETAILMSG(TONE_DLL, ("addr_segstart: %x, addr_end: %x\n", addr_segstart, addr_end));

			if (inload == 0)
			{	//* this is the first loadable phdr /
				addr_start = addr_segstart;
				//* addr_fstart = lp->p_offset; /

				inload = 1;
			}
			break;
		}
	}

	//* alloc module info memory /
	module = (struct __dlmodule *) __dlmalloc (sizeof (struct __dlmodule));
	if (module == NULL)
	{
		RETAILMSG(TONE_DLL,  ("alloc memory for module info failure !\n"));
		__dlfree (phdr);
		return NULL;
	}
	memset (module, 0, sizeof(struct __dlmodule));
	module->l_phnum = ehdr->e_phnum;

#ifdef WIN32_TEST
	//* on win32 platform, virtual meory must align 64k size,
	//this just for test on win32 platform /
	addr_start = ALIGN(addr_start,ALIGN_SIZE);
#endif //* WIN32_TEST /

	size =  addr_end - addr_start;

	RETAILMSG(TONE_DLL, ("size: %x\n", size));

	if (ehdr->e_type == ET_EXEC)
	{ //* execute program must load at fix address /
//		module->l_map_start = (ElfW(Addr))VirtualAlloc ((LPVOID)addr_start, size,
//			MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		module->l_map_start = (ElfW(Addr))VirtualAlloc ((LPVOID)addr_start, size,
			MEM_RESERVE, PAGE_NOCACHE|PAGE_EXECUTE_READWRITE);

	}
	else
	{
//		module->l_map_start = (ElfW(Addr))VirtualAlloc (NULL, size,
//			MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		module->l_map_start = (ElfW(Addr))VirtualAlloc (NULL, size,
			MEM_RESERVE, PAGE_NOCACHE|PAGE_EXECUTE_READWRITE);

	}
	
	RETAILMSG(TONE_DLL,  ("module->l_map_start: %x\n", module->l_map_start));

	if ((module->l_map_start == 0)
		||((module->l_map_start != addr_start)&&(ehdr->e_type == ET_EXEC)))
	{
		RETAILMSG(TONE_DLL,  ("virtual alloc failure !\n"));
		__dlfree (module);
		__dlfree (phdr);
		return NULL;
	}

	module->l_map_end	= module->l_map_start + size;
	module->l_addr		= module->l_map_start - addr_start; //* in execute program this must be zero /
	module->l_entry		= ehdr->e_entry + module->l_addr;

	RETAILMSG(TONE_DLL,  ("module->l_map_end: %x\n", module->l_map_end));
	RETAILMSG(TONE_DLL,  ("module->l_addr: %x\n", module->l_addr));
	RETAILMSG(TONE_DLL,  ("module->l_entry: %x\n", module->l_entry));


	for (lp = phdr, i = 0; i < ehdr->e_phnum; i ++, lp ++)
	{
		if(lp->p_vaddr == 0)		//if the p_vaddr is 0, i think that is invalid phdr record...
			continue;

		switch (lp->p_type)
		{
		case PT_PHDR:
			module->l_phdr = (ElfW(Phdr) *)(lp->p_vaddr + module->l_addr);
			break;
		case PT_DYNAMIC:
			module->l_ld = (ElfW(Dyn) *)(lp->p_vaddr + module->l_addr);
			module->l_ldnum = lp->p_memsz / sizeof (ElfW(Dyn));
			break;
		case PT_LOAD:

			//* read program loadable segment /
//			if (read_file (hFile, (char*)(lp->p_vaddr+module->l_addr),
//				lp->p_filesz, lp->p_offset) != lp->p_filesz)
//				goto cleanup;
			{
				unsigned int nprotect, oprotect;
				if (lp->p_flags & PF_R)
					nprotect = PAGE_READONLY;
				if (lp->p_flags & PF_W)
					nprotect = PAGE_READWRITE; //PAGE_WRITECOPY
				if (lp->p_flags & PF_X)
					nprotect = PAGE_EXECUTE_READWRITE; //PAGE_EXECUTE_WRITECOPY

				nprotect |= PAGE_NOCACHE ;
				
				RETAILMSG(TONE_DLL,  ("call virtualcopy\n"));

				if(!VirtualCopy((LPVOID)module->l_map_start, (LPVOID)(pElfinfo->ulFileBase), lp->p_memsz, nprotect)){

					RETAILMSG(TONE_DLL, ("virtualcopy fail@\n"));
					goto cleanup;
				}
			}

			if (lp->p_memsz > lp->p_filesz)
			{//* must set zero, some time this is the bss section /
				memset ((void*)(module->l_addr + lp->p_vaddr + lp->p_filesz), 
					0, lp->p_memsz-lp->p_filesz);
			}

			if ((module->l_phdr == 0)&&(lp->p_offset <= ehdr->e_phoff)
				&&((lp->p_filesz + lp->p_offset) 
				>= (ehdr->e_phoff + ehdr->e_phnum*sizeof(Elf32_Phdr))))
			{
				//* Found the program header in this segment.  /
				
				//module->l_phdr = (Elf32_Phdr *)(ehdr->e_phoff + module->l_addr);
				module->l_phdr = (Elf32_Phdr *)(ehdr->e_phoff + module->l_addr + addr_start);

				RETAILMSG(TONE_DLL, ("found phdr at: %x!\n", module->l_phdr));

			}
			{//* set the segment protect /
				unsigned int nprotect, oprotect;
				if (lp->p_flags & PF_R)
					nprotect = PAGE_READONLY;
				if (lp->p_flags & PF_W)
					nprotect = PAGE_READWRITE; //PAGE_WRITECOPY
				if (lp->p_flags & PF_X)
					nprotect = PAGE_EXECUTE_READWRITE; //PAGE_EXECUTE_WRITECOPY
			
//				VirtualProtect ((LPVOID)(lp->p_vaddr+module->l_addr), lp->p_memsz, nprotect, &oprotect);
			}
			break;
		}
	}

	RETAILMSG(TONE_DLL,  ("call virtualcopy ok!\n"));

	if ((module->l_phdr == NULL)||((Elf32_Addr)module->l_phdr < (module->l_addr + addr_start)))
	{
		//* the PT_PHDR segment is not load, load it (just reuse) /
		RETAILMSG(TONE_DLL, ("--PT_PHDR is not load !\n"));
		
		//if run to here , it have error! because the PT_PHDR must be part of PT_LOAD section.
		ASSERT(0);

		module->l_phdr = phdr;
		module->l_phdr_allocated = 1;

		return module;
	}

	//__dlfree (phdr);
	return module;

cleanup: //* goto cleanup must be used belowed the three resource be alloc /
	VirtualFree ((LPVOID)module->l_map_start, size, MEM_DECOMMIT);
	__dlfree (module);
	//__dlfree (phdr);
	return NULL;
}*/

// 2003-06-27 -add by ln
LPVOID WINAPI KL_OpenDllModule( LPCTSTR lpcszName, UINT uiMode, LPVOID lpvCaller )
{
	return NULL;
}

/* // 2003-06-27 -del by ln
struct __dlmodule* KL_OpenDllModule (const char * name, unsigned int mode, 
						   struct __dlmodule *caller)
{
	return NULL;
}
*/

// 2003-06-27 -add by ln
int WINAPI KL_OpenDllDependence( LPVOID lpvModule, UINT uiMode )
{
	return 0;
}

/* // 2003-06-27 -del by ln
int KL_OpenDllDependence (struct __dlmodule* module, unsigned int mode)
{
	return 0;
}
*/


/* open a module by the name */
static struct __dlmodule* __dlopen_module (const char * name, unsigned int mode, 
						   struct __dlmodule *caller,PMODULE_HANDLE lpHandle)
{

	struct __dlmodule	*module;
	struct __dlmodule	*dl;
	char				*file;
	char				*find;

	HANDLE			hFile;
	Elf32_Ehdr		ehdr;
	unsigned int	read;

	DWORD			dwFileAttr;
	//EXE_ROM			*pExeInfo;
	UINT			uFileType;
	DWORD			dwReturn;
	ELF_INFO		elfinfo;

	//unsigned long	romfileBase;
	//unsigned long	romfileType;
	//unsigned long	romfileEntry;
	//unsigned long	romphnum = 0;

	//RETAILMSG(TONE_TEST, ("__dlopen_module: 11\n"));

	//puthex_virtual(0xffff0001);

	__dlassert (name != NULL);

	RETAILMSG(TONE_DLL,  ("--open module : %s \n", name));

	if (! (mode & RTLD_LOAD))
		return NULL; /* some stupid user */

	file = __dlfind_module (name, caller);
	if (file == NULL){
		RETAILMSG(TONE_ERROR,  ("No find module : %s \n", file));
		return NULL; /* can't find the file */
	}

	RETAILMSG(TONE_DLL,  ("--find module : %s \n", file));

	RETAILMSG(TONE_DLL,  ("--find module : %x, %x \n", __dl_loaded,__dl_nloaded));

	//add code to here for check if the module has loaded before...
	//....
	//....


	if (__dl_loaded != NULL) /* search loaded module first */
	{
		find = strrchr (file, '\\');
		find = find == NULL ? file : find + 1;

		dl = __dl_loaded;
		while (dl != NULL)// && dl->l_next != NULL)
		{
			RETAILMSG(TONE_DLL, ("--open module: -->2001\n"));

			if (dl->l_name != NULL) /* the execute name maybe null, take care it */
			{
				RETAILMSG(TONE_DLL, ("--open module: %s:%s\n", dl->l_name, find ));

				if (strcmp (find, dl->l_name) == 0){
					RETAILMSG(TONE_DLL, ("--open module: the module be loaded before...\n"));
					return dl; /* find the module loaded, return it */
				}
			}
			
			dl = dl->l_next;
			RETAILMSG(TONE_DLL, ("--open module: %x\n", dl ));
		}
	}

	RETAILMSG(TONE_DLL,  ("--open module  -->111\n"));

	//hFile = CreateFile (file, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ, NULL,
	hFile = CreateFile (file, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RETAILMSG(TONE_ERROR,  ("open the module file error !\n"));
		return NULL;
	}

	dwFileAttr = GetFileAttributes(file);
	if (dwFileAttr == (DWORD)-1)
		return 0;

	RETAILMSG(TONE_DLL, ("-- file attr:%x !\n", dwFileAttr));

/*	if(dwFileAttr & FILE_ATTR_ROMMODULE)
	{
		uFileType = ROM_MODULE;
	}else if(dwFileAttr & FILE_ATTR_ROMFILE)
	{
		uFileType = ROM_FILE;		
	}else{
		uFileType = RAM_FILE;
	}
*/
	if(dwFileAttr & FILE_ATTRIBUTE_ROMMODULE )
	{
		uFileType = XIP_MODULE;
	}else{
		uFileType = NON_XIP_MODULE;
	}
	
	//load rom file...
	if(uFileType == XIP_MODULE){	
		RETAILMSG(TONE_DLL, ("deviceiocontrol: %x!\r\n", &elfinfo));

		if(!DeviceIoControl(hFile, IOCTL_ROM_GET_ELF_INFO, NULL, 0, &elfinfo, 0, &dwReturn, NULL))
			return 0;
		RETAILMSG(TONE_DLL, ("deviceiocontrol ok!\r\n"));

		//romfileBase  = elfinfo.ulFileBase;
		//romfileType  = elfinfo.ulFileType;
		//romfileEntry = elfinfo.ulEntry;
		//romphnum =  elfinfo.ulPhdrnum;
		//phdr  = (Elf32_Phdr *)(elfinfo.ulPhdroff + romfileBase);

		memcpy((void*)&ehdr, (void*)elfinfo.ulFileBase, sizeof(Elf32_Ehdr));

		RETAILMSG(TONE_DLL, ("base:%x, type:%x, entry:%x, phnum:%x, phdr:%x!\r\n", 
			elfinfo.ulFileBase, elfinfo.ulFileType, elfinfo.ulEntry,elfinfo.ulPhdrnum, elfinfo.ulPhdroff));

		if (__dlcheck_elf (&ehdr) == 0)
		{
			RETAILMSG(TONE_ERROR,  ("check elf hdr failure !\n"));
			CloseHandle (hFile);
			return NULL;
		}
				
		module = __dlmap_module (hFile, &ehdr, &elfinfo, NULL, uFileType);
		//CloseHandle (hFile);

	}else{
		
		//load ram file...
		if ((! ReadFile (hFile, &ehdr, sizeof(Elf32_Ehdr), (LPDWORD)&read, NULL))
			||(read != sizeof(Elf32_Ehdr)))
		{
			RETAILMSG(TONE_ERROR,  ("read the elf header error !\n"));
			CloseHandle (hFile);
			return NULL;
		}
		
		if (__dlcheck_elf (&ehdr) == 0)
		{
			RETAILMSG(TONE_ERROR,  ("check elf hdr failure !\n"));
			CloseHandle (hFile);
			return NULL;
		}		
		
		module = __dlmap_module (hFile, &ehdr, NULL, lpHandle, uFileType);

		//don't close handle , because core should use the handle of hFile in module info...
		//CloseHandle (hFile);		
	}

	if (module == NULL)
		return NULL;

	// __dllock_module ();
	if (__dl_loaded == NULL)
	{
		/* this is the first file loaded for this program */
		__dl_loaded = module;
	}
	else
	{
		dl = __dl_loaded;

		RETAILMSG(TONE_DLL, ("__dl_loaded: %x, dl: %x\n", __dl_loaded, dl));

		while (dl->l_next != NULL)
			dl = dl->l_next;

		dl->l_next = module;
		module->l_prev = dl;
	}
	RETAILMSG(TONE_DLL, ("++load 010!\n"));

	__dl_nloaded ++;
	// __dlunlock_module ();

	module->l_loader = caller;
	if (ehdr.e_type == ET_EXEC)
		module->l_type = lt_executable;
	else
		module->l_type = lt_loaded;

	if (caller && caller->l_type == lt_executable)
		module->l_type = lt_library; /* module load by the execute set this type */

	RETAILMSG(TONE_DLL, ("++load 011!\n"));

	module->l_origin = file; 
	find = strrchr (file, '\\');
	if (find == NULL)
	{
		module->l_name = (char *)module->l_origin; /* just make compiler happy */
		module->l_origin = NULL;
	}
	else
	{
		find[0] = 0;
		module->l_name = ++find; /* module name and path use same buff */
	}

	RETAILMSG(TONE_DLL, ("++load 012!\n"));

	__dlget_dynamic_info (module);
	
	RETAILMSG(TONE_DLL, ("++load 013!\n"));

	if (__dlopen_dependence (module, mode) < 0)
	{
		RETAILMSG(TONE_DLL, ("calling __dlclean_module!\n"));

		//NOTE: 2003-09-04...ZB
		// fix a bug of free.
		//__dlclean_module (module);
		__dlclean_module (module, FALSE);
		//NOTE: 2003-09-04...ZB

		return NULL;
	}

	RETAILMSG(TONE_DLL, ("++load 014!\n"));
	__dlsetup_hash (module);

	//
	//The dll's init and fini is doing at module manager. 	
	//
	//__dlinit_module (module, 0, NULL, NULL);

//	ELF_DYNAMIC_RELOCATE(module, mode);

	RETAILMSG(TONE_DLL, ("++load 015!\n"));

	__dlrelocate_module (module, module->l_scope, mode);

	RETAILMSG(TONE_DLL, ("++load 016!\n"));

	return module;
}

///////////////////////////////////////////////////////////////////////

unsigned int __dlrelocate_module(struct __dlmodule *module, struct __dlscope_elem **scope, unsigned int mode)
{
//	ELF_DYNAMIC_RELOCATE (module, mode);

	int edr_lazy;

	struct { ElfW(Addr) start; int size; int lazy; } ranges[2];
	int ranges_index;

	RETAILMSG(TONE_DLL, ("__dlrelocate_module: <\n"));

	if (module->l_relocated){
		RETAILMSG(TONE_DLL, ("__dlrelocate_module: haved relocated! >\n"));	
		return 1;
	}
	module->l_relocated = 1;
	
	edr_lazy = __dlarch_setup_runtime(module, mode);

	RETAILMSG(TONE_DLL, ("edr_lazy: %x\r\n", edr_lazy));

	ranges[0].lazy = 0;
    ranges[0].size = ranges[1].size = 0;
    ranges[0].start = 0;

	if ((module)->l_info[DT_REL])
	{
		ranges[0].start = D_PTR (module, l_info[DT_REL]);
		ranges[0].size = module->l_info[DT_RELSZ]->d_un.d_val;
	}

	if (module->l_info[DT_PLTREL] && (module->l_info[DT_PLTREL]->d_un.d_val == DT_REL))
	{
		ElfW(Addr) start = D_PTR (module, l_info[DT_JMPREL]);

		if (edr_lazy
			|| ranges[0].start + ranges[0].size != start)
		{
			RETAILMSG(TONE_DLL, ("ranges[1] set!\r\n"));

			ranges[1].start = start;
			ranges[1].size = module->l_info[DT_PLTRELSZ]->d_un.d_val;
			ranges[1].lazy = edr_lazy;
		}
		else
		/* Combine processing the sections.  */
			ranges[0].size += module->l_info[DT_PLTRELSZ]->d_un.d_val;
	}

	for (ranges_index = 0; ranges_index < 2; ++ranges_index)
	{
		RETAILMSG(TONE_DLL, ("ranges_index: <%x\r\n", ranges_index));

		__dldynamic_rel (module, ranges[ranges_index].start,
			ranges[ranges_index].size,
			ranges[ranges_index].lazy);

		RETAILMSG(TONE_DLL, ("ranges_index: >%x\r\n", ranges_index));
	}

	RETAILMSG(TONE_DLL, ("__dlrelocate_module: >\n"));

	return 1;
};

struct __dlmodule* __dlload_module (PMODULE_HANDLE lpHandle, const char * name)
{
	struct __dlmodule *module;

	__dlassert (name != NULL);

	//RETAILMSG(TONE_TEST, ("__dlload_module: 11\n"));

	module = __dlopen_module (name, RTLD_NOW, NULL, lpHandle);
	if (module == NULL){
		RETAILMSG(TONE_ERROR, ("__dlload_module FAIL!\n"));
		return NULL;
	}

	RETAILMSG(TONE_DLL, ("__dlload_module OK!\n"));

	return module;
}


unsigned int __dlclean_module (struct __dlmodule *module, BOOL bNested)
{
	/* Acquire the lock.  */
//	__libc_lock_lock (_dl_load_lock);

	//note: 2003-09-04...zb
	//fix a bug of free.
	#define MAX_MODULE	100

	static	struct __dlmodule * CleanedModule[MAX_MODULE];
	static	int    index;
	BOOL		   bHaveCleaned ;
	int			   j;

	if(!bNested){	//第一次进入
		index = 0;
		memset(CleanedModule, 0, sizeof(CleanedModule));		
	}else{
		if((index+1) < MAX_MODULE){
			CleanedModule[index ++] = module;
		}else{
			RETAILMSG(1, ("elfmodule:error: modules is larger than length of array!\r\n"));
		}
	}
	//note: 2003-09-04...zb

	RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: < %x:%s\n",module, module->l_name));

	/* Decrement the reference count.  */
	if (module->l_opencount > 1)
	{
		/* One decrement the object itself, not the dependencies.  */
		--module->l_opencount;

//		__libc_lock_unlock (_dl_load_lock);
		RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: > ref --: %d\n",module->l_opencount ));
		return 0;
    }

	if (module->l_initfini != NULL)
	{
		struct __dlmodule **list = module->l_initfini;
		unsigned int i;
		
		RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>%s\n", list[0]->l_name));
		/* first is himself */


		for (i = 1; list[i] != NULL; ++i)
		{
			//note: 2003-09-04...zb
			//add to fix a bug of free.
			bHaveCleaned = FALSE;

			for(j = 0; j < index; j++)
			{
				if(CleanedModule[j] == list[i]){
					bHaveCleaned = TRUE;
					break;
				}
			}
			if(!bHaveCleaned){
				RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: list[%d]:%x\n", i, list[i]));
				__dlclean_module (list[i], TRUE);
			}else{
				RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: module:%x have cleaned\n", list[i]));				
			}
			//note: 2003-09-04...zb
		}

		RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>1\n"));

		__dlfree (module->l_initfini);

		module->l_initfini = NULL;
	}
	
	//we do this at freelibrary!!! so remove it here!
	/*if (module->l_info[DT_FINI_ARRAY] != NULL)
	{
		ElfW(Addr) *array =
			(ElfW(Addr) *) (module->l_addr
			+ module->l_info[DT_FINI_ARRAY]->d_un.d_ptr);

		unsigned int sz = (module->l_info[DT_FINI_ARRAYSZ]->d_un.d_val
			/ sizeof (ElfW(Addr)));
		unsigned int cnt;
		
		RETAILMSG(TONE_DLL, ("__dlclean_module: ==>2\n"));
		for (cnt = 0; cnt < sz; ++cnt){
			((__dlfinit_fun) (module->l_addr + array[cnt])) ();
		}
	}

	RETAILMSG(TONE_DLL, ("__dlclean_module: ==>3\n"));
	if (module->l_info[DT_FINI] != NULL)
		((__dlfinit_fun)(module->l_addr + module->l_info[DT_FINI]->d_un.d_ptr)) ();
	*/
	
	if (module->l_prev != NULL)
		module->l_prev->l_next = module->l_next;
	else
		__dl_loaded = module->l_next;

	if (module->l_next)
		module->l_next->l_prev = module->l_prev;

	RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>4\n"));
	if (module->l_versions != NULL)
		__dlfree (module->l_versions); /* */

	RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>5\n"));
	if (module->l_origin != NULL){
		RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>51\n"));
		__dlfree ((char *) module->l_origin);
	}else{
		RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>52\n"));
		__dlfree (module->l_name);
	}

	RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>6\n"));
	if (module->l_phdr_allocated)
		__dlfree ((void *) module->l_phdr);
				
	RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: ==>7\n"));
	__dlfree (module);
//	__libc_lock_unlock (_dl_load_lock);

	RETAILMSG(TONE_DLL|TONE_DEBUG, ("__dlclean_module: > ok\n"));

	return 1;
}

/*	Search loaded objects' symbol tables for a definition of the symbol
	UNDEF_NAME.  */
static ElfW(Addr) __dllookup_symbol (const char *name, struct __dlmodule *module,
									const ElfW(Sym) **ref, struct __dlscope_elem *symbol_scope[],
									int reloc_type, int explicit_mode)
{
	const unsigned int hash = __dlhash (name);
	
	struct __dlsym_val value = { NULL, NULL };
	struct __dlscope_elem **scope;
	
	int protect;
	int noexec = __dlarch_lookup_noexec_p (reloc_type);
	int noplt = __dlarch_lookup_noplt_p (reloc_type);
	
	RETAILMSG(TONE_DLL, ("=> find symbol: %s...\n", name));

	/* Search the relevant loaded objects for a definition.  */
	for (scope = symbol_scope; *scope; ++scope)
	{
		//RETAILMSG(TONE_DLL, ("=> scope: %x...\n", *scope);

		if (__dllookup (name, hash, NULL, &value, *scope, 0, NULL,
			noexec, noplt))
		{
			break;
		}
	}
	
	if (value.s == NULL)
	{
		if (*ref == NULL || ELF32_ST_BIND((*ref)->st_info) != STB_WEAK)
		{
			/* We could find no value for a strong reference.  */
			/* XXX We cannot translate the messages.  */
			*ref = NULL;
		}
		RETAILMSG(TONE_DLL, ("=> no find symbol\n"));
		return 0;
	}

	protect = *ref && ELF32_ST_VISIBILITY((*ref)->st_other) == STV_PROTECTED;
		
	if (protect == 0)
	{
		*ref = value.s;
		//RETAILMSG(TONE_DLL, ("=> find symbol\n");
		return value.m->l_addr;
	}
	else
	{
		/* It is very tricky. We need to figure out what value to
		return for the protected symbol */
		struct __dlsym_val protected_value = { NULL, NULL };
		
		for (scope = symbol_scope; *scope; ++scope)
		{
			if (__dllookup (name, hash, *ref, &protected_value, *scope, 0,
				NULL, 0, 1))
				break;
		}

		if (protected_value.s == NULL || protected_value.m == module)
		{
			*ref = value.s;
			return value.m->l_addr;
		}
			
		return module->l_addr;
	}
}

#define DEBUG_dllookup_value 0
ElfW(Addr) __dllookup_value (const char *name, struct __dlmodule *module)
{
	const ElfW(Sym)	*ref = NULL;
	ElfW(Addr)		result;
	
	//RETAILMSG(TONE_DLL, ("__dllookup_value 11\n");
	
	//RETAILMSG(TONE_DLL, ("__dllookup_value : %s : %x\n", name, module);

	result = __dllookup_symbol (name, module, &ref, module->l_scope, 0, 1);

	//RETAILMSG(TONE_DLL, ("__dllookup_value 12\n");
	if( ref == NULL )
	{
		ERRORMSG( DEBUG_dllookup_value, ( "error in __dllookup_value: not find name=%s, module name=%s.\r\n", name, module->l_name ) );
		return 0;
	}

	return (result + ref->st_value);
}

/* macro for arch lazy relocat resolve */
void __dlarch_runtime_resolve (Elf32_Word addr)
{
}

//ELF_MACHINE_RUNTIME_TRAMPOLINE;
