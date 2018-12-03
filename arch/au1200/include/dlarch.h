/* Machine-dependent ELF dynamic relocation inline functions.  i386 version.
*/

#ifndef DLARCH_H
#define DLARCH_H

#include "dlwin.h"
/////////////////////////////////////////////////////////////////////////

#define ELF_ARCH_NAME			"i386"

/* Mask identifying addresses reserved for the user program,
   where the dynamic linker should not map anything.  */
#define ELF_MACHINE_USER_ADDRESS_MASK	0xf8000000UL

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_ARCH_JMP_SLOT			R_386_JMP_SLOT

/* The i386 never uses Elf32_Rela relocations.  */
#define ELF_ARCH_NO_RELA			1

#define MIN(a,b)		((a)>(b)?(b):(a))

/////////////////////////////////////////////////////////////////////////

/* Return nonzero iff ELF header is compatible with the running host.  */
static __inline int __dlarch_matches_host (const Elf32_Ehdr *ehdr)
{
	return ehdr->e_machine == EM_386;
}

/* This code is used in dl-runtime.c to call the `__dlfixup' function
   and then redirect to the address it returns.  */
extern void __dlarch_runtime_resolve (Elf32_Word);
/*
#define ELF_MACHINE_RUNTIME_TRAMPOLINE asm ("\
	.text
	.globl __dlruntime_resolve
	.type __dlruntime_resolve, @function
	.align 16
__dlruntime_resolve:
	pushl %eax		# Preserve registers otherwise clobbered.
	pushl %ecx
	pushl %edx
	movl 16(%esp), %edx	# Copy args pushed by PLT in register.  Note
	movl 12(%esp), %eax	# that __dlfixup takes its parameters in regs.
	call __dlfixup		# Call resolver.
	popl %edx		# Get register content back.
	popl %ecx
	xchgl %eax, (%esp)	# Get %eax contents end store function address.
	ret $8			# Jump to function address.
	.size __dlruntime_resolve, .-__dlruntime_resolve
	.previous
");
*/

/* Set up the loaded object described by L so its unrelocated PLT
   entries will jump to the on-demand fixup code in dl-runtime.c.  */
static __inline int __dlarch_setup_runtime (struct __dlmodule *module, int lazy)
{
	Elf32_Addr *got;

	
	if (module->l_info[DT_JMPREL] && lazy)
    {
		/*The GOT entries for functions in the PLT have not yet been filled
		in.  Their initial contents will arrange when called to push an
		offset into the .rel.plt section, push _GLOBAL_OFFSET_TABLE_[1],
		and then jump to _GLOBAL_OFFSET_TABLE[2].  */
		got = (Elf32_Addr *) D_PTR (module, l_info[DT_PLTGOT]);
		
		got[1] = (Elf32_Addr) module;	/* Identify this shared object.  */
		
		/* The got[2] entry contains the address of a function which gets
		called to get the address of a so far unresolved function and
		jump to it.  The profiling extension of the dynamic linker allows
		to intercept the calls to collect information.  In this case we
		don't store the address in the GOT so that all future calls also
		end in this function.  */

		/* This function will get called to fix up the GOT entry indicated by
		the offset on the stack, and then jump to the resolved address.  */
		got[2] = (Elf32_Addr) &__dlarch_runtime_resolve;
    }
	
	return lazy;
}


/* Nonzero iff TYPE should not be allowed to resolve to one of
   the main executable's symbols, as for a COPY reloc.  */
#define __dlarch_lookup_noexec_p(type) ((type) == R_386_COPY)

/* Nonzero iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.  */
#define __dlarch_lookup_noplt_p(type) ((type) == R_386_JMP_SLOT)



static __inline Elf32_Addr
__dlarch_fixup_plt (struct __dlmodule *module, struct __dlmodule *t,
		       const Elf32_Rel *reloc,
		       Elf32_Addr *reloc_addr, Elf32_Addr value)
{
	return *reloc_addr = value;
}

/* Return the final value of a plt relocation.  */
static __inline Elf32_Addr
__dlarch_plt_value (struct __dlmodule *module, const Elf32_Rel *reloc,
		       Elf32_Addr value)
{
	return value;
}

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

static __inline void __dlarch_rel (struct __dlmodule *module, const Elf32_Rel *reloc,
		 const Elf32_Sym *sym,/* const struct __dlfound_version *version,*/
		 Elf32_Addr *const reloc_addr)
{
	if (ELF32_R_TYPE (reloc->r_info) == R_386_RELATIVE)
    {
			*reloc_addr += module->l_addr;
    }
	else if (ELF32_R_TYPE (reloc->r_info) != R_386_NONE)
    {
		Elf32_Addr value;
		const Elf32_Sym *const refsym = sym;

//		Elf32_Addr value = __dlresolve_symbol (&sym, version, ELF32_R_TYPE (reloc->r_info));

		const char *strtab = (const void *) D_PTR (module, l_info[DT_STRTAB]);
#ifdef RTLD_BOOTSTRAP
		value = module->l_addr + sym->st_value;
#else
		value = __dllookup_value (strtab + sym->st_name, module);
#endif


		switch (ELF32_R_TYPE (reloc->r_info))
		{
		case R_386_GLOB_DAT:
		case R_386_JMP_SLOT:
			*reloc_addr = value;
			break;
		case R_386_32:
			*reloc_addr += value;
			break;
		case R_386_PC32:
			*reloc_addr += (value - (Elf32_Addr) reloc_addr);
			break;
		case R_386_COPY:
			if (sym == NULL)
			/* This can happen in trace mode if an object could not be
			found.  */
				break;
			memcpy (reloc_addr, (void *) value, MIN (sym->st_size,
				refsym->st_size));
			break;
		default:
			/* We add these checks in the version to relocate ld.so only
			if we are still debugging.  */
//			__dlreloc_bad_type (module, ELFW(R_TYPE) (reloc->r_info), 0);
			break;
		}
    }
}

static __inline void __dlarch_lazy_rel (struct __dlmodule *module,
					Elf32_Addr l_addr, const Elf32_Rel *reloc)
{
	Elf32_Addr *const reloc_addr = (void *) (l_addr + reloc->r_offset);

	/* Check for unexpected PLT reloc type.  */
	if (ELF32_R_TYPE (reloc->r_info) == R_386_JMP_SLOT)
	{
		*reloc_addr += l_addr;
	}
	else
	{
//		__dlreloc_bad_type (module, ELFW(R_TYPE) (reloc->r_info), 1);
	}
}

#endif /* DLARCH_H */




