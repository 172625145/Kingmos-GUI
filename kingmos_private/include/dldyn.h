/* Inline functions for dynamic linking.
*/

static __inline void puthex_virtual (a)
{
	__asm__ __volatile__(
	"stmfd	sp!, {r0-r14}\n"
	"mov		r4, %0\n"			
	"mov		r5, #28\n"
"nextc_v:\n"
	"mov     r0, r4, lsr r5\n"
	"and     r0, r0, #0xf\n"
	"add     r2, pc, #hex_v - (.+8)\n"
	"ldrb    r0, [r0, r2]\n"
 	"ldr		r12, =0xA8010000\n"
"11:	ldr		r1, [r12, #0x20]\n"	
	"tst		r1, #4\n"
	"beq		11b\n"			
	"str		r0, [r12, #0x14]\n"
	"subs    r5, r5, #4\n"
	"bge     nextc_v\n"
	"ldmfd   sp!, {r0-r14}\n"
	"b		 puthex_jump\n"

	"	.type	hex_v, #object\n"
"hex_v:	.byte 0x30, 0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x61,0x62\n"
	"	.size	hex_v, . - hex_v\n"
"puthex_jump:\n"
	:
	:"r" (a));

	return;
}

/* this function set the module->l_info feild for fast access dynamic info
   don't call any functions and global variables because bootstrap will call
   this function! */
static __inline unsigned int __dlget_dynamic_info (struct __dlmodule *module)
{
	ElfW(Dyn)	*dyn = module->l_ld;
	ElfW(Addr)	l_addr;
	ElfW(Dyn)	**info;

	if (! dyn)
		return 0;
	
	l_addr = module->l_addr;
	info = module->l_info;

	while (dyn->d_tag != DT_NULL)
	{
		if (dyn->d_tag < DT_NUM)
		{
			info[dyn->d_tag] = dyn;
		}
		else if (dyn->d_tag >= DT_LOPROC && dyn->d_tag < DT_LOPROC + DT_THISPROCNUM)
		{
			info[dyn->d_tag - DT_LOPROC + DT_NUM] = dyn;
		}
		else if ((Elf32_Word) DT_VERSIONTAGIDX (dyn->d_tag) < DT_VERSIONTAGNUM)
		{
			info[VERSYMIDX (dyn->d_tag)] = dyn;
		}
		else if ((Elf32_Word) DT_EXTRATAGIDX (dyn->d_tag) < DT_EXTRANUM)
		{
			info[DT_EXTRATAGIDX (dyn->d_tag) + DT_NUM + DT_THISPROCNUM
			+ DT_VERSIONTAGNUM] = dyn;
		}
		else
		{
			/* error return 0 */
			return 0;
		}

		++dyn;
    }

	if (info[DT_PLTGOT] != NULL)
		info[DT_PLTGOT]->d_un.d_ptr += l_addr;
	if (info[DT_STRTAB] != NULL)
		info[DT_STRTAB]->d_un.d_ptr += l_addr;
	if (info[DT_SYMTAB] != NULL)
		info[DT_SYMTAB]->d_un.d_ptr += l_addr;

# if ! ELF_ARCH_NO_RELA
	if (info[DT_RELA] != NULL)
	{
		__dlassert (info[DT_RELAENT]->d_un.d_val == sizeof (ElfW(Rela)));
		info[DT_RELA]->d_un.d_ptr += l_addr;
	}
# endif

# if ! ELF_ARCH_NO_REL
	if (info[DT_REL] != NULL)
	{
		__dlassert (info[DT_RELENT]->d_un.d_val == sizeof (ElfW(Rel)));
		info[DT_REL]->d_un.d_ptr += l_addr;
	}
# endif

	if (info[DT_PLTREL] != NULL)
	{
# if ELF_ARCH_NO_RELA
		__dlassert (info[DT_PLTREL]->d_un.d_val == DT_REL);
# elif ELF_ARCH_NO_REL
		__dlassert (info[DT_PLTREL]->d_un.d_val == DT_RELA);
# else
		__dlassert (info[DT_PLTREL]->d_un.d_val == DT_REL
			|| info[DT_PLTREL]->d_un.d_val == DT_RELA);
# endif
    }

	if (info[DT_JMPREL] != NULL)
		info[DT_JMPREL]->d_un.d_ptr += l_addr;
	if (info[VERSYMIDX (DT_VERSYM)] != NULL)
		info[VERSYMIDX (DT_VERSYM)]->d_un.d_ptr += l_addr;

	if (info[DT_FLAGS] != NULL)
    {
		/* Flags are used.  Translate to the old form where available.
		Since these l_info entries are only tested for NULL pointers it
		is ok if they point to the DT_FLAGS entry.  */
		ElfW(Word) flags = info[DT_FLAGS]->d_un.d_val;
		if (flags & DF_SYMBOLIC)
			info[DT_SYMBOLIC] = info[DT_FLAGS];
		if (flags & DF_TEXTREL)
			info[DT_TEXTREL] = info[DT_FLAGS];
		if (flags & DF_BIND_NOW)
			info[DT_BIND_NOW] = info[DT_FLAGS];
	}

	if (info[VERSYMIDX (DT_FLAGS_1)] != NULL)
		module->l_flags_1 = info[VERSYMIDX (DT_FLAGS_1)]->d_un.d_val;
	if (info[DT_RUNPATH] != NULL)
		/* If both RUNPATH and RPATH are given, the latter is ignored.  */
		info[DT_RPATH] = NULL;

	return 1;
}

/* Get the definitions of `__dldynamic_do_rel' and `__dldynamic_do_rela'.
These functions are almost identical, so we use cpp magic to avoid
duplicating their code.  It cannot be done in a more general function
because we must be able to completely inline.  */

/* On some machines, notably SPARC, DT_REL* includes DT_JMPREL in its
range.  Note that according to the ELF spec, this is completely legal!
But conditionally define things so that on machines we know this will
not happen we do something more optimal.  */

# ifdef ELF_MACHINE_PLTREL_OVERLAP

#define _ELF_DYNAMIC_DO_RELOC(RELOC, reloc, module, do_lazy, test_rel)		\
do {																		\
	struct { ElfW(Addr) start, size; int lazy; } ranges[3];					\
	int ranges_index;														\
																			\
	ranges[0].lazy = ranges[2].lazy = 0;									\
	ranges[1].lazy = 1;														\
	ranges[0].size = ranges[1].size = ranges[2].size = 0;					\
																			\
	if ((module)->l_info[DT_##RELOC])										\
	{																		\
		ranges[0].start = D_PTR ((module), l_info[DT_##RELOC]);				\
		ranges[0].size = (module)->l_info[DT_##RELOC##SZ]->d_un.d_val;	    \
	}																		\
																			\
	if ((do_lazy)&& (module)->l_info[DT_PLTREL]								\
			&& (!test_rel || (module)->l_info[DT_PLTREL]->d_un.d_val == DT_##RELOC)) \
	{																		\
		ranges[1].start = D_PTR ((module), l_info[DT_JMPREL]);				\
		ranges[1].size = (module)->l_info[DT_PLTRELSZ]->d_un.d_val;			\
		ranges[2].start = ranges[1].start + ranges[1].size;					\
		ranges[2].size = ranges[0].start + ranges[0].size - ranges[2].start;	\
		ranges[0].size = ranges[1].start - ranges[0].start;					\
	}																		\
																			\
	for (ranges_index = 0; ranges_index < 3; ++ranges_index)				\
	{																		\
		__dldynamic_##reloc ((module),										\
			ranges[ranges_index].start,										\
			ranges[ranges_index].size,										\
			ranges[ranges_index].lazy);										\
	}																		\
}while (0);

# else

#define _ELF_DYNAMIC_DO_RELOC(RELOC, reloc, module, do_lazy, test_rel)		\
do{																			\
	struct { ElfW(Addr) start; int size; int lazy; } ranges[2];				\
	int ranges_index;														\
																			\
	ranges[0].lazy = 0;														\
    ranges[0].size = ranges[1].size = 0;									\
    ranges[0].start = 0;													\
																			\
	if ((module)->l_info[DT_##RELOC])										\
	{																		\
		ranges[0].start = D_PTR ((module), l_info[DT_##RELOC]);				\
		ranges[0].size = (module)->l_info[DT_##RELOC##SZ]->d_un.d_val;		\
	}																		\
																			\
	if ((module)->l_info[DT_PLTREL]											\
		&& (!test_rel || (module)->l_info[DT_PLTREL]->d_un.d_val == DT_##RELOC))	\
	{																		\
		ElfW(Addr) start = D_PTR ((module), l_info[DT_JMPREL]);				\
																			\
		if ((do_lazy)														\
			|| ranges[0].start + ranges[0].size != start)					\
		{																	\
			ranges[1].start = start;										\
			ranges[1].size = (module)->l_info[DT_PLTRELSZ]->d_un.d_val;		\
			ranges[1].lazy = (do_lazy);										\
		}																	\
		else																\
		/* Combine processing the sections.  */								\
			ranges[0].size += (module)->l_info[DT_PLTRELSZ]->d_un.d_val;	\
	}																		\
																			\
	for (ranges_index = 0; ranges_index < 2; ++ranges_index)				\
	{																		\
		if (ranges[ranges_index].size > 0)									\
			__dldynamic_##reloc ((module),									\
			ranges[ranges_index].start,										\
			ranges[ranges_index].size,										\
			ranges[ranges_index].lazy);										\
	}																		\
} while (0);

#endif
	
#if ELF_ARCH_NO_REL || ELF_ARCH_NO_RELA
#define _ELF_CHECK_REL 0
#else
#define _ELF_CHECK_REL 1
#endif
	
#if ! ELF_ARCH_NO_REL
#include "dlreloc.h"
#define ELF_DYNAMIC_DO_REL(module, lazy) \
	_ELF_DYNAMIC_DO_RELOC (REL, rel, module, lazy, _ELF_CHECK_REL)
#else
#define ELF_DYNAMIC_DO_REL(module, lazy) /* Nothing to do.  */
#endif
	
#if ! ELF_ARCH_NO_RELA
#define DO_RELA
#include "dlreloc.h"
#define ELF_DYNAMIC_DO_RELA(module, lazy) \
	_ELF_DYNAMIC_DO_RELOC (RELA, rela, module, lazy, _ELF_CHECK_REL)
#else
#define ELF_DYNAMIC_DO_RELA(module, lazy) /* Nothing to do.  */
#endif


/* This can't just be an inline function because GCC is too dumb
to inline functions containing inlines themselves.  */
//    int edr_lazy = elf_machine_runtime_setup ((module), (lazy),

#define	ELF_DYNAMIC_RELOCATE(module,lazy)						\
do{																\
	int edr_lazy = __dlarch_setup_runtime((module), (lazy));	\
	ELF_DYNAMIC_DO_REL ((module), edr_lazy);					\
	ELF_DYNAMIC_DO_RELA ((module), edr_lazy);					\
}while(0)

