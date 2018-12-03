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
#include <edevice.h>
#include <eassert.h>

#include <elf.h>
#include <exe_elf.h>
#include <romheader.h>
#include <module.h>
#include "dlstruct.h"
#include <diskio.h>
#include <epalloc.h>

ElfW(Addr) __dllookup_value (const char *name, struct __dlmodule *module);

#include <dlarch.h>
#include "dldyn.h"
#include <coresrv.h>

#define INVALID_SET_FILE_POINTER		((unsigned int)(-1))

#define DL_MEM_BLOCK					0x1000
#define ELF_ALIGN(addr)					((addr)&(~(DL_MEM_BLOCK-1)))
#define ELF_UPALIGN(addr)				(((addr)+DL_MEM_BLOCK-1)&(~(DL_MEM_BLOCK-1)))
#define ALIGN(addr,align)				(((addr)+(align)-1)&(~(align-1)))

#define INVALID_ADDR			0
//#define	DL_DYN_ENTRY			0xffffffff
#define DEFALT_STACK			0x0

#define	E_PHENTSIZE				0x20

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

#define SIZE_T					unsigned int
////////////////////////////////////////////////////////////////////////
#define elf_assert(assert_value)	ASSERT(assert_value)

#define elf_matches_host(value) value

#undef	__inline
#define	__inline

//#define		ROM_MODULE		1
//#define		ROM_FILE		2
//#define		RAM_FILE		3


//#define RETAILMSG(TONE_EXEC, 		EdbgOutputDebugString

void SHOW_ELF_HEADER(Elf32_Ehdr *ehdr);
void SHOW_ELF_PHDR(Elf32_Phdr	*phdr, UINT	phnum);

////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////

/*#ifndef VirtualProtect

BOOL WINAPI VirtualProtect (LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
	return FALSE;
}

#endif*/


/* check the elf header, return 0 failure
*/
static __inline unsigned int elf_check (const Elf32_Ehdr *ehdr)
{

	const unsigned char ELF_HEADER_MARK[EI_PAD] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};
	
	if (memcmp (ehdr->e_ident, ELF_HEADER_MARK, 4) != 0)
		return 0;

	if (ehdr->e_version != EV_CURRENT)
		return 0;

	if (ehdr->e_phentsize != sizeof (Elf32_Phdr))
		return 0;

	if (ehdr->e_type != ET_DYN && ehdr->e_type != ET_EXEC)
		return 0;

	if (! elf_matches_host (ehdr)) /* check arch */
		return 0;

	return 1;
}

/* read file from the offset, return readed bytes
*/
//__inline
 unsigned int read_file (HANDLE hFile, char *buff, unsigned int size,
										unsigned int offset)
{
	//unsigned int	read;
	DWORD			read;

	elf_assert(buff != NULL);

	if (SetFilePointer (hFile, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return 0;

	if (! ReadFile (hFile, buff, size, &read, NULL))
	{
		RETAILMSG(TONE_ERROR,("--read file:%x FAILURE!\n", hFile));
		return 0;
	}

	return read;
}
/* test file exist
 * return 0 iff file not exist!
 */
static __inline unsigned int elf_file_exist (const char *name)
{
	DWORD dwAttr = GetFileAttributes (name);
	RETAILMSG(TONE_EXEC, ("--file %s attributes: %x \n", name, dwAttr));
	if (dwAttr == (DWORD)-1)
		return 0;
	if (dwAttr&FILE_ATTRIBUTE_DIRECTORY)
		return 0;
	return 1;
}

/* find binary file
 * 1. current directory
 * 2  kingmos directory
 * 3. system directory
*/
//static __inline unsigned int elf_find_binary(const char *name, char *lpszBin)
#define DEBUG_elf_find_binary 0
__inline unsigned int elf_find_binary(const char *name, char *lpszBin)
{
	unsigned int len = strlen(name);
	//const char *p = name + len;
	const char *p = name;
	//const char *file;
 
	DEBUGMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: entry(%s).\r\n",name ));
	
	elf_assert(name != NULL);
	
	if (elf_file_exist (name) != 0)
	{
		//memcpy (lpszBin, name, len+1);
    	DEBUGMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: find it(%s).\r\n",name ));		
		strcpy(lpszBin, name);	
		return 1;
	}

	

//	file = strrchr (name, '\\');
//	if (file == NULL)
//		file = name; /* name without directory info */
//	else{ 		
		//file ++; /* skip '\\' */

//		return 0;	//if with directory info... 
//	}

	while(*p){
		if(*p == '\\'){
			return 0;
		}
		else
			p ++;
	}

//	while ((p >= name)&&(*p != '\\'))
//		p --;
//	p ++;

/*	GetCurrentDirectory (lpszBin,  MAX_PATH);

	if (lpszBin[strlen(lpszBin)-1] != '\\')
		strcat (lpszBin, "\\");
	strcat(lpszBin, p);
	if (elf_file_exist (lpszBin) != 0)
		return 1;
*/
	DEBUGMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: searching file in kingmos, now!.\n"));

	//strcpy (lpszBin, "\\kingmos\\");
	strcpy (lpszBin, KINGMOS_PATH);
	//strcat (lpszBin, file);
	strcat (lpszBin, name);
	//RETAILMSG(TONE_TEST, ("f:%s\n", lpszBin));
	if (elf_file_exist (lpszBin) != 0)
	{
        DEBUGMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: finded file in kingmos.\n"));		
		return 1;
	}

	RETAILMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: searching file in system, now!.\n"));
		
	//strcpy (lpszBin, "\\system\\");
	strcpy (lpszBin, WORK_PATH);
	//strcat (lpszBin, file);
	strcat (lpszBin, name);
	if (elf_file_exist (lpszBin) != 0)
	{
    	RETAILMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: finded file in system, now!.\n"));		
		return 1;
	}
    DEBUGMSG(TONE_EXEC|DEBUG_elf_find_binary, ("elf_find_binary: leave(%s).\r\n",name ));
	return 0;
}

char	*GetSegName(HANDLE hFile, Elf32_Ehdr *ehdr, Elf32_Shdr *shdr, UINT index)
{
	Elf32_Shdr *lshdr;
	UINT		i,lPosShtab = 0;
	//char		temp[100];
	char		*temp;
	DWORD		dwRead;
	char		*pstr;
	char		*pSeg;

	//RETAILMSG(TONE_EXEC, ("GetSegName: < \n"));

	for(i = 0; i < ehdr->e_shnum ; i++){
		lshdr = shdr + i;			
		if((lshdr->sh_type == SHT_STRTAB) && 
			(lshdr->sh_flags == 0)){
			lPosShtab = lshdr->sh_offset;
			break;
		}
	}
	if(lPosShtab != 0)
	{
		SetFilePointer(hFile, lPosShtab, NULL, FILE_BEGIN);

		//RETAILMSG(TONE_EXEC, ("GetSegName:  lshdr->sh_size: %x \n", lshdr->sh_size));

		temp = (char *)_kalloc(lshdr->sh_size);

		if(temp == NULL){
			RETAILMSG(TONE_ERROR, ("GetSegName: > error26 \n"));
			return NULL;
		}

		if(!ReadFile(hFile, temp, lshdr->sh_size, &dwRead, NULL)){
			RETAILMSG(TONE_ERROR, ("GetSegName: > error22 \n"));
			_kfree(temp);
			return NULL;
		}

		//for(i = 0; i < ehdr->e_shnum; i++){
			lshdr = shdr + index;			
			//index = lshdr->sh_name;
			pstr = &temp[lshdr->sh_name];
			//fprintf( stream,"section name: %s\r\n", pstr );			
			{
				int	i = strlen(pstr);

				//RETAILMSG(TONE_EXEC, ("pstr name: %s\n", pstr));
				//RETAILMSG(TONE_EXEC, ("pstr len: %x\n", i));
			}

			pSeg = (char *)_kalloc(strlen(pstr)+1);
			if(pSeg == NULL){
				RETAILMSG(TONE_ERROR, ("segment name: _kalloc fail!\n"));
				//RETAILMSG(TONE_EXEC, ("GetSegName: > error33 \n"));
				_kfree(temp);
				return NULL;
			}
			strcpy(pSeg, pstr);
			//pSeg[strlen(pstr)] = '\0';

			//RETAILMSG(TONE_EXEC, ("segment name: %s\n", pSeg));

			//RETAILMSG(TONE_EXEC, ("GetSegName: > %x \n", pSeg));
			_kfree(temp);
			return pSeg;
		//}
	}
	RETAILMSG(TONE_ERROR, ("GetSegName: > error11 \n"));

	//_kfree(temp);
	return	NULL;
}

//struct module_seg *GetSegInfo(HANDLE hFile, Elf32_Ehdr	*ehdr)
struct module_seg *GetSegInfo(HANDLE hFile)
{
	 struct module_seg	*seg_info = NULL;
	 struct module_seg	*seg_temp = NULL;
	 int	i;
	 Elf32_Shdr * shdr;
	 // 2004-10-21
	 //Elf32_Ehdr	*ehdr;	 	 
	 Elf32_Ehdr		elf_ehdr;
	 //

	 //RETAILMSG(TONE_MOD, ("GetDllSegInfo: <\n"));

	 // 2004-10-21
	 //ehdr = (Elf32_Ehdr *)_kalloc (sizeof(Elf32_Ehdr));
	 //if (ehdr == NULL){
		 //return	NULL;
	 //}	
	 // 
	
	RETAILMSG(TONE_MOD, ("GetDllSegInfo: <\n"));

	if (read_file (hFile, (char*)&elf_ehdr, sizeof(Elf32_Ehdr), 0) != sizeof(Elf32_Ehdr))
	{
		// 2004-10-21
		//_kfree(ehdr);
		//
		RETAILMSG(TONE_MOD, ("GetDllSegInfo: >xx\n"));
		return FALSE;
	}	 

	// 2004-10-21
	 //ASSERT(ehdr->e_shnum != 0);
	 ASSERT(elf_ehdr.e_shnum != 0);
	 //
	 
	 // 2004-10-21
	 //shdr = (Elf32_Shdr *)_kalloc (ehdr->e_shentsize * ehdr->e_shnum);
	 shdr = (Elf32_Shdr *)_kalloc (elf_ehdr.e_shentsize * elf_ehdr.e_shnum);
	 //
	 if (shdr == NULL){
 		// 2004-10-21
//		 _kfree(ehdr);
		 //
		 return	NULL;
	 }

	 // 2004-10-21
	 //if (read_file (hFile, (char*)shdr, ehdr->e_shentsize*ehdr->e_shnum, ehdr->e_shoff) 
		 //!= (unsigned int)(ehdr->e_shentsize*ehdr->e_shnum))
	 if (read_file (hFile, (char*)shdr, elf_ehdr.e_shentsize*elf_ehdr.e_shnum, elf_ehdr.e_shoff) 
		 != (unsigned int)(elf_ehdr.e_shentsize * elf_ehdr.e_shnum))
	 //
	 {
		 // 2004-10-21
		 //_kfree(ehdr);
		 //
		 _kfree(shdr);
		 return	NULL;
	 }
	 
	 RETAILMSG(TONE_EXEC,("elf_ehdr.e_shnum: %x\n", elf_ehdr.e_shnum));

//	 RETAILMSG(TONE_EXEC, ("GetSegInfo : ==>11!\n");
//	 return NULL;

	 // 2004-10-21
	 //for(i = 0; i < ehdr->e_shnum; i++)
	 for(i = 0; i < elf_ehdr.e_shnum; i++)
	 //
	 {
		 Elf32_Shdr	*lp = shdr + i;
		 //struct module_seg *seg_info;	
		 
		 if(lp->sh_type == SHT_NULL)
			 continue;
		 //2004-10-21
		 //seg_info = (struct module_seg *)_kalloc (sizeof(struct module_seg ));
		 seg_info = (struct module_seg *)KHeap_Alloc(sizeof(struct module_seg ));
		 //
		 if (seg_info == NULL){
			 RETAILMSG(TONE_ERROR,("GetSegInfo _kalloc	fail!\n"));
			 
			 _kfree(shdr);
			 //2004-10-21
			 //_kfree(ehdr);
			 //
			 return	NULL;
		 }
		 seg_info->sh_addr	= lp->sh_addr;
		 seg_info->sh_offset = lp->sh_offset;
		 seg_info->sh_size	= lp->sh_size;

		 //2004-10-21
		 //seg_info->sh_name = GetSegName(hFile, ehdr, shdr, i);
		 seg_info->sh_name = GetSegName(hFile, &elf_ehdr, shdr, i);
		 //
	 
		 seg_info->lpNext = seg_temp;
		 seg_temp = seg_info;

	 }

	 _kfree(shdr);
	 //2004-10-21
	 //_kfree(ehdr);
	 //

	 //RETAILMSG(TONE_EXEC, ("GetSegInfo : > result: %x, ok!\n", seg_temp);
	 return seg_temp;
}

#define		__KFREE_INTERPRET();	//\
elf_assert(0); \
_kfree (interpret);	


static __inline unsigned int elf_load_binary (struct exe_module *module, const char * name, PMODULE_HANDLE lpHandle, 
											  UINT	flag)
{
	HANDLE			hFile;	/* file handle of the binary file */
	Elf32_Ehdr		ehdr;   /* for read elf head */
	Elf32_Phdr		*phdr = NULL;
//	char			lpszBin[MAX_PATH];
	//char			*lpszBin;
	char szBin[MAX_PATH];

	char			*interpret = NULL;
	int				have_inter = 0;

	Elf32_Addr		entry_addr;
	Elf32_Addr		start = 0, end = 0;
	int				start_set = 0;

	Elf32_Addr		load_addr;
	Elf32_Addr		addr_offset;

	unsigned int	i;
	unsigned int	retval = INVALID_ADDR;

	Elf32_Phdr			*module_phdr = NULL;
	int					module_phdr_allocated = 0;
	Elf32_Dyn			*module_dyn = NULL;
	unsigned int		module_ndyn = 0;

	struct module_info	*info;

	DWORD			dwFileAttr;
	EXE_ROM			*pExeInfo;
	UINT			uFileType;
	DWORD			dwReturn;
	ELF_INFO		elfinfo;
	UINT			indexLoad = 0;
	BOOL			bMapModule = FALSE;
	BOOL			bRomModule = FALSE;
	//struct module_info	*lpInfo;
	PLOAD_MODULE	lpInfo;
	DWORD			flAllocationType;
	DWORD			flflag;

	unsigned long	romfileBase;
	unsigned long	romfileType;
	unsigned long	romfileEntry;
	unsigned long	romphnum = 0;

	elf_assert (name != NULL);

	if(lpHandle != NULL){
		//RETAILMSG(TONE_TEST, ("elf_load_binary: load new module!\n"));
		RETAILMSG(TONE_TEST, ("->(%s\n",name));
		lpInfo = lpHandle->info;
		bMapModule = TRUE;
	}
	else{
		//RETAILMSG(TONE_TEST, ("elf_load_binary: ref a module!\n"));
		RETAILMSG(TONE_TEST, ("->)%s\n",name));
		bMapModule = FALSE;
	}

	//2004-10-21
	//if(!(lpszBin = (char*)_kalloc(MAX_PATH))){
	//	RETAILMSG(TONE_ERROR,("_kalloc fail!\n"));
	//	return 0;
	//}
	//
		

	RETAILMSG(TONE_EXEC, ("--xxload file:%s \n", name));
	//2004-10-21
	//if (elf_find_binary(name, lpszBin) == 0)
	if (elf_find_binary(name, szBin) == 0)	
	//
	{
		//RETAILMSG(TONE_ERROR,("--file:%s not exist!\n", lpszBin));
		RETAILMSG(TONE_ERROR,("--file:%s not exist!\n", name));
		//2004-10-21
		//_kfree(lpszBin);
		//
		return 0;
	}
	//2004-10-21
	//hFile = CreateFile (lpszBin, GENERIC_READ, FILE_SHARE_READ, NULL,
		//OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	hFile = CreateFile (szBin, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RETAILMSG(TONE_ERROR,("--open file:%s failure!\n", szBin));

		//2004-10-21
		//_kfree(lpszBin);
		//
		return INVALID_ADDR;
	}

	module->hFile	= hFile;	

	if(flag == MODULE_LOAD){	//以后通过hFile获取文件属性和段信息!!!
		//2004-10-21
		//_kfree(lpszBin);
		//
		return 0x01;
	}

	RETAILMSG(TONE_EXEC,("--open file:%x OK!\n", hFile));
	//2004-10-21
	//dwFileAttr = GetFileAttributes(lpszBin);
	dwFileAttr = GetFileAttributes(szBin);
	//
	if (dwFileAttr == (DWORD)-1){
		//2004-10-21
		//_kfree(lpszBin);
		//
		return 0;
	}

	//2004-10-21
	//_kfree(lpszBin);
	//

	RETAILMSG(TONE_EXEC,("-- file attr:%x !\n", dwFileAttr));

	if(dwFileAttr & FILE_ATTRIBUTE_ROMMODULE )
	{
		uFileType = XIP_MODULE;
	}else{
		uFileType = NON_XIP_MODULE;
	}
	
	if(uFileType == XIP_MODULE){	

		bRomModule = TRUE;

		RETAILMSG(TONE_EXEC,("deviceiocontrol: %x!\r\n", &elfinfo));
		if(!DeviceIoControl(hFile, IOCTL_ROM_GET_ELF_INFO, NULL, 0, &elfinfo, 0, &dwReturn, NULL))
			return 0;

		RETAILMSG(TONE_EXEC,("deviceiocontrol ok!\r\n"));

		romfileBase  = elfinfo.ulFileBase;
		romfileType  = elfinfo.ulFileType;
		romfileEntry = elfinfo.ulEntry;
		romphnum =  elfinfo.ulPhdrnum;
		phdr  = (Elf32_Phdr *)(elfinfo.ulPhdroff + romfileBase);

		RETAILMSG(TONE_EXEC,("base:%x, type:%x, entry:%x, phnum:%x, phdr:%x!\r\n", romfileBase, romfileType, romfileEntry,
			romphnum, phdr));

		/*if(!DeviceIoControl(hFile, IOCTL_ROM_GET_EXE_INFO, NULL, 0, pExeInfo, 0, &dwReturn, NULL))
			return 0;
		RETAILMSG(TONE_EXEC, (TEXT("EXE: exe info: exe_imageflags:%x,exe_entryrav:%x,exe_vbase:%x,exe_stackmax:%x, \
				exe_vsize:%x,exe_phdroff:%x,exe_phdrnum:%x\r\n"), pExeInfo->exe_imageflags, pExeInfo->exe_entryrav,
				pExeInfo->exe_vbase, pExeInfo->exe_stackmax, pExeInfo->exe_vsize, pExeInfo->exe_phdroff, 
				pExeInfo->exe_phdrnum));
				*/
	}else{
		if (read_file (hFile, (char*)&ehdr, sizeof(Elf32_Ehdr), 0) != sizeof(Elf32_Ehdr))
		{
			//RETAILMSG(TONE_ERROR, ("-read %s ehdr failure!\n", lpszBin));
			CloseHandle (hFile);
			return INVALID_ADDR;
		}
		//RETAILMSG(TONE_EXEC, ("--read file:%s elf head OK!\n", lpszBin));
		
		SHOW_ELF_HEADER(&ehdr);

		if (! elf_check (&ehdr))
		{
			RETAILMSG(TONE_ERROR, ("--the binary can't run in this arch!\n"));
			CloseHandle (hFile);
			return INVALID_ADDR;
		}
		//RETAILMSG(TONE_EXEC, ("--check file:%s elf head OK!\n", lpszBin));
		

			
			phdr = (Elf32_Phdr *)_kalloc (ehdr.e_phentsize * ehdr.e_phnum);
			if (phdr == NULL)
			{
				CloseHandle (hFile);
				return INVALID_ADDR;
			}
			RETAILMSG(TONE_EXEC, ("--alloc phdr:%x  OK!\n", phdr));
			
			/* read phdr infomation */
			if (read_file (hFile, (char*)phdr, ehdr.e_phentsize*ehdr.e_phnum, ehdr.e_phoff) 
				!= (unsigned int)(ehdr.e_phentsize*ehdr.e_phnum))
			{
				RETAILMSG(TONE_ERROR, ("--read %s phdr failure!\n"));
				goto error_out;
			}
			//RETAILMSG(TONE_EXEC, ("--read file:%s elf phdr OK!\n", lpszBin));
			
			SHOW_ELF_PHDR(phdr, ehdr.e_phnum);

			romphnum		= ehdr.e_phnum;
			romfileType		= ehdr.e_type;
			romfileEntry	= ehdr.e_entry;
	}


	RETAILMSG(TONE_EXEC, ("load_binary_module: ==>111\n"));

	for (i = 0; i < romphnum; i ++)
	{
		Elf32_Phdr	*lp = phdr + i;

		if(lp->p_vaddr == 0)		//if the p_vaddr is 0, i think that is invalid phdr record...
			continue;

		switch (lp->p_type)
		{
		case PT_LOAD:
			{
				Elf32_Addr	segment_end = lp->p_vaddr + lp->p_memsz;
				
				if((++indexLoad) >= PT_LOAD_RES)
					break;

				if (start_set == 0)
				{
					start_set = 1;
					start = lp->p_vaddr;
				}
				end = end < segment_end ? segment_end : end;
			}
			break;
		case PT_INTERP:
			/* we just use the interpret of the execute binary file
			*/
			if(bRomModule){
				if ((! have_inter)&&(romfileType == ET_EXEC)) 
				{					
					//interpret = (char*)(romfileBase + lp->p_vaddr);
					interpret = (char*)(romfileBase + lp->p_offset);
					have_inter = 1;
					RETAILMSG(TONE_EXEC,("interpret: %s\n", interpret));
				}
			}else{
				if ((! have_inter)&&(romfileType == ET_EXEC)) 
				{
					interpret = (char *)_kalloc(lp->p_memsz + 1);
					
					//RETAILMSG(TONE_EXEC, ("--lp->p_memsz:%x \n", lp->p_memsz);
					
					if (interpret == NULL){
						RETAILMSG(TONE_ERROR, ("--PT_INTERP: fail! \n"));
						goto error_out;
					}
					//__KFREE_INTERPRET();
					//RETAILMSG(TONE_EXEC, ("--PT_INTERP:%x \n", interpret));
					
					//__KFREE_INTERPRET();
					
					memset (interpret, 0, lp->p_memsz + 1);
					
					if (read_file(hFile, interpret, lp->p_filesz, lp->p_offset) != lp->p_filesz)
						goto error_out;
					have_inter = 1;
					//__KFREE_INTERPRET();
				}
			}
			break;
		}
	}

	RETAILMSG(TONE_EXEC,("start: %x, end: %x !\r\n", start, end));

	/* if file have no PT_LOAD phdr, we just lose it */
	if (! start_set)
		goto error_out;

	/* align the end address of the module */
	end = ELF_UPALIGN(end);

	if((bMapModule)||(bRomModule)){//Ref loaded module or load rom module!

		RETAILMSG(TONE_EXEC, ("__dlmap_module: ->11\n"));
		flAllocationType = MEM_RESERVE;
		//NOTE: 2003-08-26
		//PAGE_NOCACHE 会降低系统性能
		//flflag = PAGE_NOCACHE|PAGE_EXECUTE_READWRITE; 
		flflag = PAGE_EXECUTE_READWRITE; 
		//NOTE: 2003-08-26
	}else{
		flAllocationType = MEM_COMMIT|MEM_RESERVE;
		flflag = PAGE_EXECUTE_READWRITE; 
	}

	if (romfileType == ET_EXEC)
	{
		/* execute binary need fix address */
		load_addr = (Elf32_Addr)VirtualAlloc ((LPVOID)ELF_ALIGN(start), end-ELF_ALIGN(start),
			flAllocationType, flflag);
		addr_offset = 0;
	}
	else
	{
		load_addr = (Elf32_Addr)VirtualAlloc (NULL, end-start,
			flAllocationType, flflag);
		addr_offset = load_addr - start;
	}
	__KFREE_INTERPRET();

	if (load_addr == INVALID_ADDR)
		goto error_out;

	RETAILMSG(TONE_EXEC, ("--virtual alloc address:%x \n", load_addr));
	RETAILMSG(TONE_EXEC, ("--virtual alloc size:%x \n", end-ELF_ALIGN(start)));
	RETAILMSG(TONE_EXEC, ("--virtual alloc OK \n"));


	indexLoad = 0;

	for (i = 0; i < romphnum; i ++)
	{
		Elf32_Phdr	*lp = phdr + i;
	
		if(lp->p_vaddr == 0)		//if the p_vaddr is 0, i think that is invalid phdr record...
			continue;

		switch (lp->p_type)
		{
		case PT_LOAD:
		{
			unsigned int nprotect = 0;
			//unsigned int oprotect;
			DWORD			oprotect;
			char *seg_addr = (char *)(lp->p_vaddr + addr_offset);
			
			if((++indexLoad) >= PT_LOAD_RES)
				break;

			RETAILMSG(TONE_EXEC, ("--load segment :at:%x size:%x off:%x \n", seg_addr, lp->p_filesz, lp->p_offset));

			if (lp->p_flags & PF_R)
				nprotect = PAGE_READONLY;
			if (lp->p_flags & PF_W)
				nprotect = PAGE_READWRITE; //PAGE_WRITECOPY
			if (lp->p_flags & PF_X)
				nprotect = PAGE_EXECUTE_READWRITE; //PAGE_EXECUTE_WRITECOPY

			if(bMapModule||bRomModule){
				//__dlprintf("module->l_map_start: %x, load addr:%x\n", module->l_map_start, lp->p_vaddr+module->l_addr);

				if(indexLoad == PT_LOAD_CODE){
					LPVOID	pDest;

					if(bMapModule){
						pDest = MapPtrToProcess((LPVOID)lpInfo->start, lpHandle->hProc);
					}else{
						pDest = (LPVOID)(romfileBase + lp->p_offset);
					}
					//RETAILMSG(TONE_EXEC,("Map CODE segment to previous loaded module...\n"));
					//RETAILMSG(TONE_EXEC,("lpInfo->start: %x\n", lpInfo->start));

					//NOTE: 2003-08-26
					//PAGE_NOCACHE 会降低系统性能
					//if(!VirtualCopy((LPVOID)(seg_addr), pDest, lp->p_memsz, nprotect|PAGE_NOCACHE)){
					if(!VirtualCopy((LPVOID)(seg_addr), pDest, lp->p_memsz, nprotect)){
					//NOTE: 2003-08-26
						
						RETAILMSG(TONE_ERROR,("virtualcopy fail@\n"));
						goto error_out;
					}
				}else if(indexLoad == PT_LOAD_DATA){				

					RETAILMSG(TONE_EXEC,("Load DATA segment to memory...\n"));

					VirtualAlloc ((LPVOID)(seg_addr), lp->p_memsz, MEM_COMMIT, nprotect);

					if(bMapModule){
						/* read program loadable segment */
						if (read_file (hFile, (char*)(seg_addr),lp->p_filesz, lp->p_offset) != lp->p_filesz)
							goto error_out;					
					}else{
						memcpy(seg_addr, (LPVOID)(romfileBase + lp->p_offset), lp->p_filesz);
					}
				}				
			}else{				
				RETAILMSG(TONE_EXEC, ("__dlmap_module: 44\n"));

				if (read_file (hFile, seg_addr, lp->p_filesz, lp->p_offset) != lp->p_filesz)
					goto error_out;				

				//VirtualProtect ((LPVOID)seg_addr, lp->p_memsz, nprotect, &oprotect);
			}
			VirtualProtect ((LPVOID)seg_addr, lp->p_memsz, nprotect, &oprotect);

			if (lp->p_memsz > lp->p_filesz)
			{
				memset (seg_addr+lp->p_filesz, 0, lp->p_memsz-lp->p_filesz);
			}
		}
			break;
		case PT_PHDR:
			module_phdr = (Elf32_Phdr *)(lp->p_vaddr + addr_offset);
			break;
		case PT_DYNAMIC:
			module_dyn  = (Elf32_Dyn *)(lp->p_vaddr + addr_offset);
			module_ndyn = lp->p_memsz / sizeof (Elf32_Dyn);
			break;
		}
	}
	
	//__KFREE_INTERPRET();

	RETAILMSG(TONE_EXEC, ("--load segment ok \n "));

	if (romfileType == ET_EXEC)
	{
		module_phdr = (Elf32_Phdr *)(MODULE_EXE_PHDR_START);

		info = module->exe_info;

		if (have_inter)
		{
			//changed for test...
			//entry_addr = elf_load_binary (module, interpret);
			entry_addr = DL_DYN_ENTRY;

			if (entry_addr == INVALID_ADDR)
				goto error_out;
		}else{
			//entry_addr = ehdr.e_entry;
			entry_addr = romfileEntry;
		}
		//info->entry		= ehdr.e_entry;;
		info->entry		= romfileEntry;
	}
	else
	{
		module_phdr = (Elf32_Phdr *)(MODULE_DLL_PHDR_START);

		module->inter = 1;
		info = module->dll_info;

		entry_addr = romfileEntry + addr_offset;
		info->entry		= entry_addr;
	}
	//__KFREE_INTERPRET();

	//memcpy (module_phdr, phdr, ehdr.e_phentsize*ehdr.e_phnum);
	memcpy (module_phdr, phdr, E_PHENTSIZE*romphnum);

	info->addr		= addr_offset;
	info->start		= load_addr;
	info->end		= load_addr + (start - end);
	info->ld		= module_dyn;
	info->nld		= module_ndyn;
	info->phdr		= module_phdr;
	info->nphdr		= romphnum;
	info->alloced	= module_phdr_allocated;

	//info->hFile		= hFile;	//zb add...
	//module->hFile	= hFile;	
	
	//RETAILMSG(TONE_EXEC, ("--%s entry : %x \n", name, entry_addr);
	RETAILMSG(TONE_EXEC, ("--%s entry : %x \n", name, info->entry));

	retval = entry_addr;

	RETAILMSG(TONE_EXEC, ("load_binary_file => 1!\n"));

//rom file will go to here:
rom_run:

	//module->entry = retval;

error_out:
	if(!bRomModule){
		RETAILMSG(TONE_EXEC, ("load_binary_file => 2!\n"));

		if (phdr != NULL)
			_kfree (phdr);
		
		__KFREE_INTERPRET();
		RETAILMSG(TONE_EXEC, ("load_binary_file => 3!\n"));
		if (interpret != NULL){
			RETAILMSG(TONE_EXEC, ("--PT_INTERP:%x \n", interpret));
			_kfree (interpret);
		}
	}

	RETAILMSG(TONE_EXEC, ("load_binary_file => 31!\n"));
	if (! retval)
	{
		RETAILMSG(TONE_EXEC, ("load_binary_file => 4!\n"));
		VirtualFree ((LPVOID)load_addr, start-end, MEM_DECOMMIT|MEM_RELEASE);

		RETAILMSG(TONE_EXEC, ("load_binary_file => 5!\n"));
		CloseHandle (hFile);
		RETAILMSG(TONE_ERROR, ("load_binary_file fail!\n"));

		return retval;
	}

	//CloseHandle (hFile);

	RETAILMSG(TONE_EXEC, ("load_binary_file OK!\n"));
	return retval;

}

typedef unsigned int (*LPSTART)(void *);
/* load the execute module and his interpret
 * this function will alloc resource for process's module and init the main
 * thread cpu context, when this function return, the process is live
 */
struct exe_module  *elf_exec (const char * name, PMODULE_HANDLE lpHandle, UINT	flag)
{
	unsigned int entry;
	/* execute binary module need fix address */
	struct exe_module *module = (struct exe_module *)VirtualAlloc (NULL,//(LPVOID)MODULE_INFO_START,
		MODULE_INFO_SIZE, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

	if (module == NULL)
	{
		DWORD dw = KL_GetLastError ();
		RETAILMSG(TONE_ERROR, ("--%x elf_load: can't alloc memory !\n", dw));
		return 0;
	}
	/*//add for test...
	module->pbuf = (UINT *)VirtualAlloc (NULL,
		sizeof(UINT)*10, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	if (module->pbuf == NULL)
	{
		DWORD dw = KL_GetLastError ();
		RETAILMSG(TONE_EXEC, ("--%x elf_load: can't alloc memory !\n", dw);
		return 0;
	}
	{
		int	i;

		for(i = 0; i<10; i++){
			module->pbuf[i] = i;
		}
	}
//	module->myinfo->addr	= 0x1111;	
//	module->myinfo->start	= 0x2222;	
	*/
	
	RETAILMSG(TONE_EXEC, ("--elf_load: alloc memory at: %x OK !\n", (unsigned int)module));

	memset(module, 0, sizeof(struct exe_module));

	module->exe_info = (struct module_info *)(module + 1);
	module->dll_info = (struct module_info *)(module->exe_info + 1);

	/*{
		struct module_info	*ptmp = (struct module_info *)(module->dll_info + 1);
		RETAILMSG(TONE_EXEC, ("ptmp: %x\n", ptmp);
	}*/
	entry = elf_load_binary (module, name, lpHandle, flag);
	if (entry != INVALID_ADDR)
	{
		RETAILMSG(TONE_EXEC, ("--elf_load_binary ok \n "));
		
		module->entry = entry;
		
		/*{
			LPSTART pstart = (LPSTART)entry;
			RETAILMSG(TONE_EXEC, ("--_start address:%x \n", pstart);
			return pstart(module->exe_info);
		}*/
		return module;
	}
	
	VirtualFree ((LPVOID)module, MODULE_INFO_SIZE, MEM_DECOMMIT|MEM_RELEASE);

	return NULL;
}


//BOOL GetProcAddr(struct __dlmodule* module, PMODULE_HANDLE pHandle, LPCSTR lpProcName)
/*BOOL GetProcAddr(struct __dlmodule* module, PMODULE_HANDLE pHandle)
{

		const ElfW(Sym)		*symtab;
		const char			*strtab;
//		const ElfW(Half)	*verstab;
//		Elf_Symndx			symidx;
		int					i = 0;
		const ElfW(Sym)		*sym;
		char				*name;

		
		RETAILMSG(TONE_EXEC, ("syn len: %x\r\n", sizeof(ElfW(Sym))));

		symtab = (const void *) D_PTR (module, l_info[DT_SYMTAB]);
		strtab = (const void *) D_PTR (module, l_info[DT_STRTAB]);
//		verstab = module->l_versyms;

		// Search the appropriate hash bucket in this object's symbol table
		//for a definition for the same symbol name.  
		RETAILMSG(TONE_EXEC, ("module->l_nchain: %x\r\n", module->l_nchain));

		for(i = 0; i < module->l_nchain; i++){

			sym = &symtab[i];

			//RETAILMSG(TONE_EXEC, ("GetProcAddr  111\r\n");

			if(sym == NULL)
				continue;

			 /// No value.  /
			if (sym->st_value == 0 )
				continue;
			
			//RETAILMSG(TONE_EXEC, ("GetProcAddr  222\r\n");

			///* Ignore all but STT_NOTYPE, STT_OBJECT and STT_FUNC entries
			//since these are no code/data definitions.  /
			if ( ELF32_ST_TYPE(sym->st_info) != STT_FUNC)
				continue;
			
			// Not the symbol we are looking for.  
//			if (sym != ref && strcmp (strtab + sym->st_name, undef_name))
//				continue;
			
			//RETAILMSG(TONE_EXEC, ("GetProcAddr  333\r\n");

			// There cannot be another entry for this symbol so stop here.  
			switch (ELF32_ST_BIND(sym->st_info))
			{
//			case STB_WEAK:
			case STB_GLOBAL:
				{
				PFUNCTION_SYM	pFun;
				UINT			len;

				//if(strcmp(lpProcName, (strtab + sym->st_name)) != 0)
				//	continue;

				pFun = (PFUNCTION_SYM)KHeap_Alloc(sizeof(FUNCTION_SYM));

				if(pFun == NULL){
						KL_DebugOutString("alloc fail!\n");
						return FALSE;
				}				

				len = strlen( (strtab + sym->st_name) ) + 1;
				pFun->lpFuncName = KHeap_Alloc( len * sizeof( TCHAR ) );
				
				if( pFun->lpFuncName == NULL ){
					KHeap_Free( pFun, sizeof( FUNCTION_SYM ) );
					//KL_LeaveCriticalSection( &csModuleList );	 
					RETAILMSG(TONE_MOD, ("GetProcAddr: >1\n"));
					//return NULL;
					return FALSE;
				}
				strcpy( pFun->lpFuncName, (strtab + sym->st_name) );	

				pFun->lpNext = pHandle->lpProc;
				pHandle->lpProc = pFun;

				//* Global definition.  Just what we need.  /
				//strcpy(pFun->lpszFunName, (strtab + sym->st_name));
				//name = strtab + sym->st_name;
			
				RETAILMSG(TONE_EXEC, ("funname: %s\r\n", pFun->lpszFunName));

				pFun->lpProc = __dllookup_value((const char*)pFun->lpszFunName, module);
				

				}
//				result->s = sym;
//				result->m = module;
//				return 1;
//			default:
				//* Local symbols are ignored.  /
//				break;
			}
		}//while (1);

		RETAILMSG(TONE_EXEC, ("GetProcAddr: >ok\r\n"));

		//return pFun->lpProc;
		return TRUE;
}*/


extern struct __dlmodule* __dlload_module (PMODULE_HANDLE lpHandle, const char * name);

struct __dlmodule* elf_load (PMODULE_HANDLE pHandle,const char * name, UINT flag)
{
	struct __dlmodule* loadmodule;
	//PMODULE_INFO pModInfo = &pmod->ModInfo;

	/*if(flag == LOAD_MODULE_NEW){
		loadmodule = __dlload_module(NULL, name);
	}else{
		loadmodule = __dlload_module(pmod->lpHandle, name);
	}*/

//	if(pmod->lpBase == NULL)
		//RETAILMSG(TONE_TEST, ("--elf_load new module! \n "));
//		RETAILMSG(TONE_TEST, ("-)%s\n",name));
//	else
		//RETAILMSG(TONE_TEST, ("--elf_load ref module! \n "));
//		RETAILMSG(TONE_TEST, ("-(%s\n",name));

//	loadmodule = __dlload_module(pmod->lpBase, name);
	loadmodule = __dlload_module(pHandle, name);

	if(loadmodule == NULL)
	{
		RETAILMSG(TONE_ERROR, ("--__dlload_module fail! \n "));
		//return FALSE;
		return NULL;
	}

	RETAILMSG(TONE_EXEC, ("--__dlload_module OK! \n "));

	return loadmodule;
	//return TRUE;
}

#define	NO_SHOW_ELF

void SHOW_ELF_HEADER(Elf32_Ehdr *ehdr)
{
#ifndef NO_SHOW_ELF
	RETAILMSG(TONE_EXEC, ("------------------------------------\n"));
	RETAILMSG(TONE_EXEC, ("ehdr->e_entry: 0x%x\n", ehdr->e_entry));
	RETAILMSG(TONE_EXEC, ("ehdr->e_phoff: 0x%x\n", ehdr->e_phoff));
	RETAILMSG(TONE_EXEC, ("ehdr->e_shoff: 0x%x\n", ehdr->e_shoff));
	RETAILMSG(TONE_EXEC, ("ehdr->e_ehsize: 0x%x\n", ehdr->e_ehsize));
	RETAILMSG(TONE_EXEC, ("ehdr->e_phentsize: 0x%x\n", ehdr->e_phentsize));
	RETAILMSG(TONE_EXEC, ("ehdr->e_phnum: 0x%x\n", ehdr->e_phnum));
	RETAILMSG(TONE_EXEC, ("ehdr->e_shentsize: 0x%x\n", ehdr->e_shentsize));
	RETAILMSG(TONE_EXEC, ("ehdr->e_shnum: 0x%x\n", ehdr->e_shnum));
	RETAILMSG(TONE_EXEC, ("ehdr->e_shstrndx: 0x%x\n", ehdr->e_shstrndx));
	RETAILMSG(TONE_EXEC, ("ehdr->e_type: 0x%x\n", ehdr->e_type));
#endif
}

void SHOW_ELF_PHDR(Elf32_Phdr	*phdr, UINT	phnum)
{

#ifndef NO_SHOW_ELF

	UINT		i;

	for(i = 0; i < phnum; i++)
	{
		Elf32_Phdr	*lp = phdr + i;
		
		RETAILMSG(TONE_EXEC, ("\n"));
		RETAILMSG(TONE_EXEC, ("-----------------------------\n"));
		if(lp->p_type == PT_LOAD)
			RETAILMSG(TONE_EXEC, ("lp->p_type: PT_LOAD\n"));					
		else if(lp->p_type == PT_PHDR)
			RETAILMSG(TONE_EXEC, ("lp->p_type: PT_PHDR\n"));
		else if(lp->p_type == PT_DYNAMIC)
			RETAILMSG(TONE_EXEC, ("lp->p_type: DYNAMIC\n"));
		
		RETAILMSG(TONE_EXEC, ("lp->p_offset: 0x%x\n", lp->p_offset);
		
		RETAILMSG(TONE_EXEC, ("lp->p_vaddr: 0x%x\n", lp->p_vaddr));
		RETAILMSG(TONE_EXEC, ("lp->p_paddr: 0x%x\n", lp->p_paddr));
		RETAILMSG(TONE_EXEC, ("lp->p_filesz: 0x%x\n", lp->p_filesz));
		RETAILMSG(TONE_EXEC, ("lp->p_memsz: 0x%x\n", lp->p_memsz));					
		RETAILMSG(TONE_EXEC, ("lp->p_align: 0x%x\n", lp->p_align));
		RETAILMSG(TONE_EXEC, ("lp->p_flags: 0x%x\n", lp->p_flags));
		
		if (lp->p_flags & PF_R)
			//nprotect = PAGE_READONLY;
			RETAILMSG(TONE_EXEC, ("PAGE_READONLY\n"));
		if (lp->p_flags & PF_W)
			//nprotect = PAGE_READWRITE; //PAGE_WRITECOPY
			RETAILMSG(TONE_EXEC, ("PAGE_READWRITE\n"));
		if (lp->p_flags & PF_X)
			//nprotect = PAGE_EXECUTE_READWRITE; //PAGE_EXECUTE_WRITECOPY
			RETAILMSG(TONE_EXEC, ("PAGE_EXECUTE_READWRITE\n"));				
	}	
	
	/*for(i = 0; i < info.nshdr; i++)
	{
		Elf32_Shdr	*lp = info.shdr + i;
		
		RETAILMSG(TONE_EXEC, ("\n");
		RETAILMSG(TONE_EXEC, ("-----------------------------\n");
		RETAILMSG(TONE_EXEC, ("lp->sh_name: 0x%x\n", lp->sh_name);
		RETAILMSG(TONE_EXEC, ("lp->sh_type: 0x%x\n", lp->sh_type);
		RETAILMSG(TONE_EXEC, ("lp->sh_flags: 0x%x\n", lp->sh_flags);
		RETAILMSG(TONE_EXEC, ("lp->sh_addr: 0x%x\n", lp->sh_addr);
		RETAILMSG(TONE_EXEC, ("lp->sh_offset: 0x%x\n", lp->sh_offset);
		RETAILMSG(TONE_EXEC, ("lp->sh_size: 0x%x\n", lp->sh_size);
		RETAILMSG(TONE_EXEC, ("lp->sh_link: 0x%x\n", lp->sh_link);
		RETAILMSG(TONE_EXEC, ("lp->sh_info: 0x%x\n", lp->sh_info);
		RETAILMSG(TONE_EXEC, ("lp->sh_addralign: 0x%x\n", lp->sh_addralign);
		RETAILMSG(TONE_EXEC, ("lp->sh_entsize: 0x%x\n", lp->sh_entsize);
	}*/
#endif
	
}

