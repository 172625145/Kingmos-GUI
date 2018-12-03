/* Data structure for communication from the run-time dynamic linker for
   loaded ELF shared objects.
*/

#ifndef	DLSTRUCT_H
#define	DLSTRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY			0x00001		/* Lazy function call binding.  */
#define RTLD_NOW			0x00002		/* Immediate function call binding.  */
#define	RTLD_LOAD			0x00003			/* Mask of binding time value.  */
#define RTLD_NOLOAD			0x00004		/* Do not load the object.  */

/* We use this macro to refer to ELF types independent of the native wordsize.
   `ElfW(TYPE)' is used in place of `Elf32_TYPE' or `Elf64_TYPE'.  */
#define __ELF_NATIVE_CLASS		32

#define ElfW(type)	_ElfW (Elf, __ELF_NATIVE_CLASS, type)
#define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)	e##w##t

#define DT_THISPROCNUM				0

#ifndef VERSYMIDX
# define VERSYMIDX(sym)	(DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGIDX (sym))
#endif

/* All references to the value of l_info[DT_PLTGOT],
  l_info[DT_STRTAB], l_info[DT_SYMTAB], l_info[DT_RELA],
  l_info[DT_REL], l_info[DT_JMPREL], and l_info[VERSYMIDX (DT_VERSYM)]
  have to be accessed via the D_PTR macro.  The macro is needed since for
  most architectures the entry is already relocated - but for some not
  and we need to relocate at access time.  */
//#ifdef DL_RO_DYN_SECTION
//#define D_PTR(map,i)		(map->i->d_un.d_ptr+map->l_addr)
//#else
#define D_PTR(map,i) ((map)->i->d_un.d_ptr)
//#endif

struct __dlmodule;

/* Rendezvous structure used by the run-time dynamic linker to communicate
   details of shared object loading to the debugger.  If the executable's
   dynamic section has a DT_DEBUG element, the run-time linker sets that
   element's value to the address where this structure can be found.
*/

struct __dldebug
{
	int r_version;		/* Version number for this protocol.  */
	
	struct __dlmodule *r_map;	/* Head of the chain of loaded objects.  */
	
	/* This is the address of a function internal to the run-time linker,
	that will always be called when the linker begins to map in a
	library or unmap it, and again when the mapping change is complete.
	The debugger can set a breakpoint at this address if it wants to
	notice shared object mapping changes.
	*/
    ElfW(Addr) r_brk;
    enum
	{
	/* This state value describes the mapping change taking place when
		the `r_brk' address is called.
	*/
		RT_CONSISTENT,		/* Mapping change is complete.  */
		RT_ADD,			/* Beginning to add a new object.  */
		RT_DELETE		/* Beginning to remove an object mapping.  */
	} r_state;
	
	ElfW(Addr) r_ldbase;	/* Base address the linker is loaded at.  */
};


/* A data structure for a simple single linked list of strings.  */
struct __dllibname_list
{
    const char *name;		/* Name requested (before search).  */
    struct __dllibname_list *next;	/* Link to next name for this object.  */
	int dont_free;		/* Flag whether this element should be freed
						if the object is not entirely unloaded.  */
};

struct __dlsearch_path_elem
{
    /* This link is only used in the `all_dirs' member of `r_search_path'.  */
    struct __dlsearch_path_elem *next;
	
    /* Strings saying where the definition came from.  */
    const char *what;
    const char *where;
	
    /* Basename for this search path element.  The string must end with
	a slash character.  */
    const char *dirname;
    size_t dirnamelen;
	
//    enum r_dir_status status[0];
};

/* Structure to describe a single list of scope elements.  The lookup
functions get passed an array of pointers to such structures.  */
struct __dlscope_elem
{
	/* Array of maps for the scope.  */
	struct __dlmodule **r_list;
	/* Number of entries in the scope.  */
	unsigned int r_nlist;
	
	/* Array of maps which also includes duplicates.  */
//	struct __dlmodule **r_duplist;
	/* Number of elements in this list.  */
//	unsigned int r_nduplist;
};


/* Structure to record search path and allocation mechanism.  */
struct __dlsearch_path
{
	struct __dlsearch_path_elem **dirs;
	int malloced;
};
struct __dlsym_val
{
	const ElfW(Sym)			*s;
	struct __dlmodule	*m;
};

/* Structure describing a loaded shared object.  The `l_next' and `l_prev'
   members form a chain of all the shared objects loaded at startup.

   These data structures exist in space used by the run-time dynamic linker;
   modifying them may have disastrous results.

   This data structure might change in future, if necessary.  User-level
   programs must avoid defining objects of this type.
*/

struct __dlmodule
{
	/* These first few members are part of the protocol with the debugger.
	This is the same format used in SVR4.
	*/
	
	ElfW(Addr)		l_addr;		/* Base address shared object is loaded at.  */
	char			*l_name;		/* Absolute file name object was found in.  */
	ElfW(Dyn)		*l_ld;		/* Dynamic section of the shared object.  */
	struct __dlmodule	*l_next, *l_prev; /* Chain of loaded objects.  */
	
	/* All following members are internal to the dynamic linker.
	They may change without notice.  */
	
//    struct __dllibname_list *l_libname;
    /* Indexed pointers to dynamic section.
	[0,DT_NUM) are indexed by the processor-independent tags.
	[DT_NUM,DT_NUM+DT_THISPROCNUM) are indexed by the tag minus DT_LOPROC.
	[DT_NUM+DT_THISPROCNUM,DT_NUM+DT_THISPROCNUM+DT_EXTRANUM) are indexed
	by DT_EXTRATAGIDX(tagvalue) and
	[DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM,
	DT_NUM+DT_THISPROCNUM+DT_VERSIONTAGNUM+DT_EXTRANUM)
	are indexed by DT_EXTRATAGIDX(tagvalue) (see <elf.h>).  */
	
    ElfW(Dyn) *l_info[DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGNUM
		+ DT_EXTRANUM];

    const ElfW(Phdr) *l_phdr;	/* Pointer to program header table in core.  */
    ElfW(Addr) l_entry;		/* Entry point location.  */
    ElfW(Half) l_phnum;		/* Number of program header entries.  */
    ElfW(Half) l_ldnum;	/* Number of dynamic segment entries.  */
	
	/* Array of DT_NEEDED dependencies and their dependencies, in
	dependency order for symbol lookup (with and without
	duplicates).  There is no entry before the dependencies have
	been loaded.  */
    struct __dlscope_elem l_searchlist;
	
    /* We need a special searchlist to process objects marked with
	DT_SYMBOLIC.  */
 //   struct __dlscope_elem l_symbolic_searchlist;
	
    /* Dependent object that first caused this object to be loaded.  */
    struct __dlmodule *l_loader;
	
    /* Symbol hash table.  */
    Elf_Symndx l_nbuckets;
	Elf_Symndx l_nchain;	//Zb add...
    const Elf_Symndx *l_buckets, *l_chain;
	
    unsigned int l_opencount;	/* Reference count for dlopen/dlclose.  */
    enum			/* Where this object came from.  */
	{
		lt_executable,		/* The main executable program.  */
		lt_library,		/* Library needed by main executable.  */
		lt_loaded		/* Extra run-time loaded shared object.  */
	} l_type:2;

    unsigned int l_relocated:1;		/* Nonzero if object's relocations done.  */
    unsigned int l_init_called:1;	/* Nonzero if DT_INIT function called.  */
    unsigned int l_global:1;		/* Nonzero if object in _dl_global_scope.  */
    unsigned int l_reserved:2;		/* Reserved for internal use.  */
	unsigned int l_phdr_allocated:1;	/* Nonzero if the data structure pointed
										to by `l_phdr' is allocated.  */
	unsigned int l_soname_added:1;	/* Nonzero if the SONAME is for sure in
									the l_libname list.  */
	unsigned int l_faked:1;		/* Nonzero if this is a faked descriptor
								without associated file.  */
								
	/* Array with version names.  */
	unsigned int	l_nversions;
	struct __dlfound_version	*l_versions;
								
	/* Collected information about own RPATH directories.  */
//	struct __dlsearch_path	l_rpath_dirs;
								
	/* Collected results of relocation while profiling.  */
//	ElfW(Addr)		*l_reloc_result;
								
	/* Pointer to the version information if available.  */
	ElfW(Versym)	*l_versyms;
								
	/* String specifying the path where this object was found.  */
	char		*l_origin;
								
	/* Start and finish of memory map for this object.  l_map_start
	need not be the same as l_addr.  */
	ElfW(Addr)		l_map_start, l_map_end;
								
	/* This is an array defining the lookup scope for this __dlmodule.
	There are at most three different scope lists.  */
	struct __dlscope_elem		*l_scope[4];
								
	/* A similar array, this time only with the local scope.  This is
	used occasionally.  */
//	struct __dlscope_elem		*l_local_scope[2];
								
	/* This information is kept to check for sure whether a shared
	object is the same as one already loaded.  */
//	dev_t l_dev;
//	ino64_t l_ino;
								
	/* Collected information about own RUNPATH directories.  */
//	struct __dlsearch_path		l_runpath_dirs;
								
	/* List of object in order of the init and fini calls.  */
	struct __dlmodule **l_initfini;
								
	/* List of the dependencies introduced through symbol binding.  */
	unsigned int l_reldepsmax;
	unsigned int l_reldepsact;
	struct __dlmodule **l_reldeps;
								
	/* Various flag words.  */
	ElfW(Word) l_feature_1;
	ElfW(Word) l_flags_1;
								
	/* Temporarily used in `dl_close'.  */
//	unsigned int l_idx;

	/////////////////
	HANDLE		hFile;
	//UINT		filetype;	//rom or other.
//	HANDLE		hMap;
	/////////////////
};


extern unsigned int __dlrelocate_module(struct __dlmodule *module, struct __dlscope_elem **scope, unsigned int mode);
extern int __dlopen_dependence (struct __dlmodule* module, unsigned int mode);
extern void __dlsetup_hash (struct __dlmodule* module);

#ifdef __cplusplus
}
#endif 

#endif /* DLSTRUCT_H */
