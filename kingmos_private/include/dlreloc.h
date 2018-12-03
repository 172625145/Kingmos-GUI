/*some function must be __inline for bootstrap, but some compiler don't support
  __inline keyword in pure c program, so you must do it use define
*/

/* This file may be included twice, to define both
   `__dldynamic_rel' and `__dldynamic_rela'.  */

#ifdef DO_RELA

# define __dldynamic_rel	__dldynamic_rela
# define __dlarch_rel		__dlarch_rela

# define Rel			Rela

#endif /*DO_RELA*/

#ifndef VERSYMIDX
# define VERSYMIDX(sym)	(DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGIDX(sym))
#endif /*VERSYMIDX*/

/* Perform the relocations in module on the running program image as specified
   by RELTAG, SZTAG.  If LAZY is nonzero, this is the first pass on PLT
   relocations; they should be set up to call __dlresolve_runtime, rather
   than fully resolved now.
*/

static __inline void __dldynamic_rel (struct __dlmodule *module,
				 ElfW(Addr) reladdr, ElfW(Addr) relsize, int lazy)
{
	const ElfW(Rel) *r = (const void *) reladdr;
	const ElfW(Rel) *end = (const void *) (reladdr + relsize);
		
/*	if (lazy)
    {
		/\* Doing lazy PLT relocations; they need very little info.  *\/
		ElfW(Addr) l_addr = module->l_addr;
		puts ("lazy\n");
		for (; r < end; ++r)
		{
			__dlarch_lazy_rel (module, l_addr, r);
		}
    }
	else
*/    {
		const ElfW(Sym) *const symtab =
			(const void *) D_PTR (module, l_info[DT_SYMTAB]);
		
		//RETAILMSG(TONE_DLL, ("xxx 111\r\n"));

		if (module->l_info[VERSYMIDX (DT_VERSYM)])
		{
			const ElfW(Half) *const version =
				(const void *) D_PTR (module, l_info[VERSYMIDX (DT_VERSYM)]);
			
			//RETAILMSG(TONE_DLL, ("xxx 222\r\n"));

			for (; r < end; ++r)
			{
				ElfW(Half) ndx = version[ELF32_R_SYM(r->r_info)];
				//RETAILMSG(TONE_DLL, ("xxx 333\r\n"));

				__dlarch_rel (module, r, &symtab[ELF32_R_SYM(r->r_info)],
					/*&module->l_versions[ndx],*/
					(void *) (module->l_addr + r->r_offset));
			}
		}
		else
		{
			//RETAILMSG(TONE_DLL, ("xxx 444\r\n"));
			for (; r < end; ++r)
			{
				//RETAILMSG(TONE_DLL, ("xxx 555\r\n"));
				__dlarch_rel (module, r, &symtab[ELF32_R_SYM(r->r_info)],/* NULL,*/
				(void *) (module->l_addr + r->r_offset));
			}
		}
    }
}

#undef __dldynamic_rel
#undef Rel
#undef __dlarch_rel
