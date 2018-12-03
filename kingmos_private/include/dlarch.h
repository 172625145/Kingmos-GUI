/* Machine-dependent ELF dynamic relocation inline functions.  ARM version.
*/

#ifndef DLARCH_H
#define DLARCH_H

#define ELF_MACHINE_NAME "ARM"

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_ARCH_JMP_SLOT			R_ARMJMP_SLOT

/* The i386 never uses Elf32_Rela relocations.  */
#define ELF_ARCH_NO_RELA			1

#undef  MIN
#define MIN(a,b)		((a)>(b)?(b):(a))

/* Return nonzero iff ELF header is compatible with the running host.  */
static __inline int __dlarch_matches_host (const Elf32_Ehdr *ehdr)
{
	return ehdr->e_machine == EM_ARM;
}


/* Return the link-time address of _DYNAMIC.  Conveniently, this is the
first element of the GOT.  This must be inlined in a function which
uses global data.  */
static __inline Elf32_Addr __dlarch_dynamic (void)
{
	register Elf32_Addr *got asm ("r10");
	return *got;
}

/* Set up the loaded object described by L so its unrelocated PLT
entries will jump to the on-demand fixup code in dl-runtime.c.  */
static __inline int __dlarch_setup_runtime (struct __dlmodule *module, int lazy)
{
	Elf32_Addr *got;
	extern void __dlarch_runtime_resolve (Elf32_Word);
	
	if (module->l_info[DT_JMPREL] && lazy)
    {
		/* patb: this is different than i386 */
		/* The GOT entries for functions in the PLT have not yet been filled
		in.  Their initial contents will arrange when called to push an
		index into the .got section, load ip with &_GLOBAL_OFFSET_TABLE_[3],
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

extern void __dlarch_runtime_resolve (Elf32_Word);
/* This code is used in dl-runtime.c to call the `fixup' function
and then redirect to the address it returns.  */
/* macro for handling PIC situation....
#ifdef PIC
#define CALL_ROUTINE(x) " ldr sl,0f
add 	sl, pc, sl
1:	ldr	r2, 2f
mov	lr, pc
add	pc, sl, r2
b	3f
0:	.word	_GLOBAL_OFFSET_TABLE_ - 1b - 4
2:	.word " #x "(GOTOFF)
3:	"
#else
#define CALL_ROUTINE(x) " bl " #x
#endif

# define ELF_MACHINE_RUNTIME_TRAMPOLINE asm ("\
.text
.globl __dlarch_runtime_resolve
.type __dlarch_runtime_resolve, #function
.align 2
__dlarch_runtime_resolve:
@ we get called with
@ 	stack[0] contains the return address from this call
@	ip contains &GOT[n+3] (pointer to function)
@	lr points to &GOT[2]

@ save almost everything; lr is already on the stack
stmdb	sp!,{r0-r3,sl,fp}

@ prepare to call fixup()
@ change &GOT[n+3] into 8*n        NOTE: reloc are 8 bytes each
sub	r1, ip, lr
sub	r1, r1, #4
add	r1, r1, r1

@ get pointer to linker struct
ldr	r0, [lr, #-4]

@ call fixup routine
" CALL_ROUTINE(fixup) "

@ save the return
mov	ip, r0

@ restore the stack
ldmia	sp!,{r0-r3,sl,fp,lr}

@ jump to the newly found address
mov	pc, ip

.size __dlarch_runtime_resolve, .-__dlarch_runtime_resolve
");
*/
/* Mask identifying addresses reserved for the user program,
where the dynamic linker should not module anything.  */
#define ELF_MACHINE_USER_ADDRESS_MASK	0xf8000000UL

/* Nonzero iff TYPE should not be allowed to resolve to one of
the main executable's symbols, as for a COPY reloc.  */
#define __dlarch_lookup_noexec_p(type) ((type) == R_ARM_COPY)

/* Nonzero iff TYPE describes relocation of a PLT entry, so
PLT entries should not be allowed to define the value.  */
#define __dlarch_lookup_noplt_p(type) ((type) == R_ARM_JUMP_SLOT)

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_ARM_JUMP_SLOT

/* The ARM never uses Elf32_Rela relocations.  */
#define ELF_MACHINE_NO_RELA 1

static inline Elf32_Addr __dlarch_fixup_plt (struct __dlmodule *module, struct __dlmodule *t,
											 const Elf32_Rel *reloc,
											 Elf32_Addr *reloc_addr, Elf32_Addr value)
{
	return *reloc_addr = value;
}

/* Return the final value of a plt relocation.  */
static inline Elf32_Addr __dlarch_plt_value (struct __dlmodule *module, const Elf32_Rel *reloc,
					Elf32_Addr value)
{
	return value;
}


/* Deal with an out-of-range PC24 reloc.  */
static Elf32_Addr fix_bad_pc24 (Elf32_Addr *const reloc_addr, Elf32_Addr value)
{/*
	static void *fix_page;
	static unsigned int fix_offset;
	static size_t pagesize;
	Elf32_Word *fix_address;
	
	if (! fix_page)
    {
		if (! pagesize)
			pagesize = getpagesize ();
		fix_page = mmap (NULL, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (! fix_page)
			assert (! "could not module page for fixup");
		fix_offset = 0;
    }
	
	fix_address = (Elf32_Word *)(fix_page + fix_offset);
	fix_address[0] = 0xe51ff004;	/\* ldr pc, [pc, #-4] *\/
	fix_address[1] = value;
	
	fix_offset += 8;
	if (fix_offset >= pagesize)
		fix_page = NULL;
	
	return (Elf32_Addr)fix_address;*/
}

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
MAP is the object containing the reloc.  */

static __inline void __dlarch_rel (struct __dlmodule *module, const Elf32_Rel *reloc,
			  const Elf32_Sym *sym, /*const struct r_found_version *version,*/
			  Elf32_Addr *const reloc_addr)
{
	//RETAILMSG(TONE_DLL, ("xxx 666\r\n"));

	if (ELF32_R_TYPE (reloc->r_info) == R_ARM_RELATIVE)
    {
			*reloc_addr += module->l_addr;
    }
	else if (ELF32_R_TYPE (reloc->r_info) != R_ARM_NONE)
    {
		Elf32_Addr value;
		const Elf32_Sym *const refsym = sym;

		const char *strtab = (const void *) D_PTR (module, l_info[DT_STRTAB]);

		//RETAILMSG(TONE_DLL, ("xxx 777\r\n"));

#ifdef RTLD_BOOTSTRAP
		//RETAILMSG(TONE_DLL, ("xxx 888\r\n"));
		value = module->l_addr + sym->st_value;
#else
		//RETAILMSG(TONE_DLL, ("xxx 999\r\n"));
		value = __dllookup_value (strtab + sym->st_name, module);
#endif

		switch (ELF32_R_TYPE (reloc->r_info))
		{
		case R_ARM_COPY:
			if (sym == NULL)
			/*	This can happen in trace mode if an object could not be
				found.  */
			break;
			memcpy (reloc_addr, (void *) value, MIN (sym->st_size,
				refsym->st_size));
			break;

		case R_ARM_GLOB_DAT:
		case R_ARM_JUMP_SLOT:
//#ifdef RTLD_BOOTSTRAP
			/* Fix weak undefined references.  */
//			if (sym != NULL && sym->st_value == 0)
//				*reloc_addr = 0;
//			else
//#endif
				*reloc_addr = value;
			break;
		case R_ARM_ABS32:
				*reloc_addr += value;
				break;
		case R_ARM_PC24:
			{
				Elf32_Sword addend;
				Elf32_Addr newvalue, topbits;

				//__dlprintf ("--R_ARM_PC24\n");

				//RETAILMSG(TONE_DLL, ("yyy 111\r\n"));
				
				addend = *reloc_addr & 0x00ffffff;
				if (addend & 0x00800000) addend |= 0xff000000;
				
				newvalue = value - (Elf32_Addr)reloc_addr + (addend << 2);
				topbits = newvalue & 0xfe000000;
				if (topbits != 0xfe000000 && topbits != 0x00000000)
				{
					newvalue = fix_bad_pc24(reloc_addr, value)
						- (Elf32_Addr)reloc_addr + (addend << 2);
					topbits = newvalue & 0xfe000000;
					if (topbits != 0xfe000000 && topbits != 0x00000000)
					{
						//__dlprintf ("R_ARM_PC24 relocation out of range");
					}
				}
				//RETAILMSG(TONE_DLL, ("yyy 222\r\n"));

				newvalue >>= 2;
				value = (*reloc_addr & 0xff000000) | (newvalue & 0x00ffffff);
				*reloc_addr = value;
			}
			break;
		default:
			/* _dl_reloc_bad_type (module, ELF32_R_TYPE (reloc->r_info), 0);*/
			break;
		}
    }
}
			  
static __inline void __dlarch_lazy_rel (struct __dlmodule *module,
										Elf32_Addr l_addr, const Elf32_Rel *reloc)
{
	Elf32_Addr *const reloc_addr = (void *) (l_addr + reloc->r_offset);
	/* Check for unexpected PLT reloc type.  */
	if (ELF32_R_TYPE (reloc->r_info) == R_ARM_JUMP_SLOT)
	{
		*reloc_addr += l_addr;
	}
	else
	{
		/* _dl_reloc_bad_type (module, ELF32_R_TYPE (reloc->r_info), 1); */
	}
}

#endif /* !DLARCH_H */
