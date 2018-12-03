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
	2. 2003-09-06...zb: 增加对加载的文件的判断：即如果文件有XIP属性，
	                    而又不在\kingmos路径下，就不加载。
	3. 2003-09-16...zb: 文件名比较不应用strstr, 应全匹配。 
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <efile.h>

#include <epalloc.h>
#include <epheap.h>
#include <edevice.h>
#include "diskio.h"
#include <eprocess.h>
#include <epcore.h>
#include <coresrv.h>
#include <elf.h>
#include <module.h>

#include "dlstruct.h"

typedef unsigned int (*LPSTART)();


#define	MY_STATIC

typedef	HANDLE		HRSC;	

MY_STATIC HANDLE ELF_CreateModule(  LPCTSTR lpszFileName, HANDLE hProcess, UINT uiCreateFlag );
MY_STATIC BOOL ELF_FreeModule( HANDLE hModule );
MY_STATIC HANDLE ELF_GetModuleFileHandle( HANDLE hModule ); 
MY_STATIC LPVOID ELF_GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName );
MY_STATIC UINT ELF_LoadModule( HANDLE hModule, HANDLE hInst, LPCTSTR lpCmdLine, UINT uiLoadFlag );
MY_STATIC LPCTSTR ELF_GetModuleFileName( HANDLE hModule ); 

MY_STATIC DWORD  ELF_GetModuleSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName );

//MY_STATIC HANDLE  ELF_FindModuleRes(HANDLE hModule,LPCTSTR lpName, LPCTSTR lpType);
//MY_STATIC HANDLE ELF_LoadModuleRes(HANDLE hModule, HANDLE hResInfo);
MY_STATIC BOOL LoadModule(PMODULE_CLIENT pClient, UINT	uLoadFlag, UINT	*lpEntry);
//MY_STATIC BOOL GetModuleType(PMODULE_NODE pMod, UINT flag);
//MY_STATIC PMODULE_HANDLE GetNewModule(struct __dlmodule *__dl_module, HANDLE hProc, UINT flag);
MY_STATIC char	*FormatModuleName(LPCTSTR lpszFileName);
PMODULE_NODE FindLoadedMod( LPCTSTR lpcszApName );
MY_STATIC BOOL  SetModuleHandle(PMODULE_HANDLE lp, struct exe_module *pmod, struct __dlmodule *__dl_module, UINT flag );
//MY_STATIC BOOL	GetDllSegInfo(HANDLE hFile, PMODULE_HANDLE	lpHandle);
PMODULE_NODE FindOrCreateModule(LPCTSTR lpszFileName, UINT *uResult, UINT flag);
MY_STATIC PMODULE_CLIENT	CreateModuleClient();
MY_STATIC PMODULE_HANDLE CreateModuleHandle(PMODULE_NODE pMod, UINT uMode, HANDLE hProcess);
MY_STATIC void InsertHandleList(PMODULE_CLIENT pmodule, PMODULE_HANDLE pModHandle);
MY_STATIC UINT CheckNewModule(struct __dlmodule *__dl_mod, PMODULE_CLIENT lp, UINT	uLoadMode, struct __dlmodule *except_mod);
MY_STATIC void CleanLoadedDll();
MY_STATIC void InsertEmptyHandleList(PMODULE_CLIENT pmodule,PMODULE_HANDLE lpHandle);
PMODULE_HANDLE FindLoadedHandle(PMODULE_NODE pMod, UINT flag);
void ReleaseMapHandle(PMODULE_NODE	pNode);
struct __dlmodule *LoadDllModule(PMODULE_CLIENT lp, struct exe_module *pmod, UINT uLoadMode, UINT *uReturn);
struct exe_module *LoadExeModule(PMODULE_CLIENT lp, UINT uLoadMode, UINT *uReturn);
void InsertClient(PMODULE_CLIENT pClient);
BOOL RemoveClient(PMODULE_CLIENT pClient);
UINT GetNewProcAddr(PMODULE_HANDLE pHandle, LPCSTR lpProcName);
BOOL ModuleInitFini(PMODULE_CLIENT lp, PMODULE_HANDLE ptemp, UINT flag );

//extern BOOL GetProcAddr(struct __dlmodule* module, PMODULE_HANDLE pHandle, LPCSTR lpProcName);

extern struct module_seg *GetSegInfo(HANDLE hFile);
extern struct __dlmodule* elf_load (PMODULE_HANDLE pHandle,const char * name, UINT flag);
extern struct __dlmodule	*dldyn_start (void *args, char *AppName);
//note: 2003-09-04...zb
//extern unsigned int __dlclean_module (struct __dlmodule *module);
extern unsigned int __dlclean_module (struct __dlmodule *module, BOOL bNested);
//note: 2003-09-04...zb

extern struct exe_module  * elf_exec (const char * name, PMODULE_HANDLE lpHandle, UINT	flag);
extern unsigned int read_file (HANDLE hFile, char *buff, unsigned int size,
										unsigned int offset);
extern unsigned int elf_find_binary(const char *name, char *lpszBin);
extern struct __dlmodule *__dl_loaded;
extern unsigned int		 __dl_nloaded;

//note: 2003-09-06...zb
BOOL IsLoadModule(LPCTSTR lpszFileName);
//note: 2003-09-06...zb


const MODULE_DRV ElfModule =
{
	ELF_CreateModule,
    ELF_FreeModule,
    ELF_GetModuleFileHandle,
	ELF_GetModuleFileName,
    ELF_GetModuleProcAddress,
//	ELF_FindModuleRes,
//	ELF_LoadModuleRes,
	ELF_GetModuleSectionOffset,	
    ELF_LoadModule
};

const char SECT_NAME_RES[]	= ".res";	
const char KING_PATH[]		= "kingmos";	
const char KINGMOS_PATH[]	= "\\kingmos\\";
const char WORK_PATH[]		= "\\system\\";
//2004-10-20, remove by lilin
//UINT		bInitModule = 0;
//
MY_STATIC	PMODULE_NODE	lpElfModList = NULL;
MY_STATIC	PMODULE_CLIENT	lpClientList = NULL;

//CRITICAL_SECTION csModuleList; //临界段
//CRITICAL_SECTION csModuleLoad; //临界段

CRITICAL_SECTION csModuleList;	//module list 临界段
CRITICAL_SECTION csClientList;	//module list 临界段
//CRITICAL_SECTION csHandleList;	//module handle 临界段
//CRITICAL_SECTION csLoadExe;		//load exe 临界段
//CRITICAL_SECTION csLoadDll;		//load dll 临界段

CRITICAL_SECTION csLoadModule;		//load dll 临界段

//#undef	__inline
//#define	__inline


BOOL InitElfModuleMgr( void )
{    
	KL_InitializeCriticalSection( &csModuleList );
	KL_InitializeCriticalSection( &csClientList );
//	KL_InitializeCriticalSection( &csHandleList );
//	KL_InitializeCriticalSection( &csLoadExe );
//	KL_InitializeCriticalSection( &csLoadDll );

	KL_InitializeCriticalSection( &csLoadModule );

	//csDeviceObjList.lpcsName = "CS-DEV";
    return TRUE;
}

//KL_EnterCriticalSection( &csLoadExe );	 
//KL_LeaveCriticalSection( &csModuleList );	



//EXE file: MODULE_LOAD	--> don't load code and data segment to memory, only save the info of module.	
//			MODULE_EXEC --> load code and date segment to memory, and save info.
//DLL file: MODULE_LOAD --> load code and date segment to memory, and save info.
//			MODULE_EXEC --> not suport.
//
/*#define CREATE_EXE           0
#define CREATE_LIBRARY       1

#define LF_EXEC             0x0000001    // run .exe file or LoadLibrary with .dll 
#define LF_LOAD_TO_MEM      0x0000002    // LoadLibrary with .exe file only

#define LF_DEFLOAD          (LF_EXEC|LF_LOAD_TO_MEM)

#define LF_NOT_INIT_DLL     0x0000004    // LoadLibrary with .dll file but not call Dll_Init
*/

MY_STATIC void ReleaseModuleHandle(PMODULE_HANDLE lp)
{
	PFUNCTION_SYM	lpProcTmp;

	if(lp->info){
		//free(lp->info);
		//if(lp->info->phdr)
		//	KHeap_Free(lp->info->phdr, sizeof( Elf32_Phdr) * lp->info->nphdr);
		//2004-10-20
		KHeap_Free( lp->info, sizeof( LOAD_MODULE ) );
		//_kfree( lp->info );
		//
	}
	
	while(lp->lpProc){
		lpProcTmp = lp->lpProc;
		lp->lpProc = lp->lpProc->lpNext;
		
		{
		UINT	len = strlen( lpProcTmp->lpFuncName ) + 1;			

		//free(lpProcTmp);
		//_kfree(lpProcTmp);
		KHeap_Free((LPVOID)lpProcTmp->lpFuncName , len * sizeof( TCHAR ));
		KHeap_Free(lpProcTmp, sizeof(FUNCTION_SYM));
		}
	}
	//free(lp);
	KHeap_Free( lp, sizeof( MODULE_HANDLE ) );
	lp = NULL;
	
	return ;
}

void ReleaseSegInfo(struct module_seg *seg)
{
	struct module_seg *ptemp;

	while(seg){
		ptemp = seg;
		seg = seg->lpNext;

		if(ptemp->sh_name){
			RETAILMSG(TONE_DEBUG, ("SEG4\r\n"));
			_kfree(ptemp->sh_name);
		}
		RETAILMSG(TONE_DEBUG, ("SEG5\r\n"));
		//2004-10-21
		//_kfree(ptemp);
		KHeap_Free( ptemp, sizeof(struct module_seg) );
		//

	}
	return;
}

#define DEBUG_ReleaseModule 0
MY_STATIC void ReleaseModule(PMODULE_NODE  lpMod)
{
	BOOL			find = FALSE;
//	PMODULE_HANDLE	lpHanlde;
	PMODULE_NODE	lpTmp;
//	PMODULE_NODE	lpMod;
//	PFUNCTION_SYM	lpProcTmp;
//	BOOL			bRefModule = FALSE;

	RETAILMSG(TONE_MOD|DEBUG_ReleaseModule,("ReleaseModule: release module for %s\n", lpMod->lpszApName));
	
	lpTmp = lpElfModList;

	if(lpElfModList == lpMod){
		lpElfModList = lpElfModList->lpNext;
		find = TRUE;
	}else{
		while( lpTmp->lpNext )
		{

			if( lpTmp->lpNext == lpMod )
			{
				lpTmp->lpNext = lpMod->lpNext;				
				find = TRUE;
				break;
			}
			lpTmp = lpTmp->lpNext;
		}
	}

	if(find){
		RETAILMSG(TONE_MOD|DEBUG_ReleaseModule, ( "ReleaseModule: find it, release module,lpMod->lpszApName=%s!.\r\n", lpMod->lpszApName) );

		//RETAILMSG(TONE_TEST, ( "F3:%s\r\n", lpMod->lpszApName));
		//2004-10-20
		//KHeap_Free( lpMod->lpszApName, sizeof((strlen(lpMod->lpszApName) + 1) * sizeof(TCHAR)));
		KHeap_FreeString( lpMod->lpszApName );//
		//_kfree( lpMod->lpszApName );
		// 

		RETAILMSG(TONE_MOD, ( "ReleaseModule1: release module!\r\n") );

		ReleaseSegInfo(lpMod->seg_info);

		RETAILMSG(TONE_MOD, ( "ReleaseModule2: release module!\r\n") );
		//2004-10-20
		KHeap_Free( lpMod, sizeof( MODULE_NODE ) );
		//_kfree( lpMod );
		//

		//KL_LeaveCriticalSection( &csModuleList );	

	    RETAILMSG(TONE_MOD|DEBUG_ReleaseModule, ( "ReleaseModule: leave: >\r\n") );
		return;
	}else{
		//KL_LeaveCriticalSection( &csModuleList );	
	    WARNMSG(TONE_MOD,( "ReleaseModule: leave: > don't find the module!\r\n" ));
		return ;
	}

}

#define DEBUG_ReleaseRef 0
MY_STATIC void ReleaseRef(PMODULE_CLIENT lpClient, PMODULE_HANDLE lp)
{
//	PMODULE_HANDLE pHandle;
	BOOL			find = FALSE;
	PMODULE_HANDLE	lpHanlde;
	PMODULE_HANDLE	lpRefHandle; 
	PMODULE_NODE	lpTmp;
	PMODULE_NODE	lpMod;
	PFUNCTION_SYM	lpProcTmp;
	BOOL			bRefModule = FALSE;
	BOOL			bEnter = FALSE;

	RETAILMSG(TONE_MOD|DEBUG_ReleaseRef, ( "ReleaseRef: lp->uRef=%d entry.\r\n",lp->uRef) );

	ASSERT(lp);

	KL_EnterCriticalSection( &csModuleList );	

	//if(lp->uRef --){
	if(-- lp->uRef ){
		bEnter = TRUE;
		KL_LeaveCriticalSection( &csModuleList );	
		RETAILMSG(TONE_CS, ( "1(-\n"));

		RETAILMSG(TONE_CS, ( "===>1:%x\n", lp->uRef));
		KL_EnterCriticalSection( &csLoadModule );	

		RETAILMSG(TONE_CS, ( "-)2\n"));
		KL_EnterCriticalSection( &csModuleList );	
		ASSERT(lp->uRef == 0);
	}

	lpMod = lp->lpModule;

	//RETAILMSG(TONE_TEST,("ReleaseRef: release handle for %s\n", lpMod->lpszApName));
	RETAILMSG(TONE_TEST, ( "<=>%s\r\n", lpMod->lpszApName) );

	lpHanlde = lpMod->lpHandle;

	if(lp == lpMod->lpHandle){
		RETAILMSG(TONE_MOD,("ReleaseRef: => 11\n"));
		lpMod->lpHandle = lp->lpNext;
		find = TRUE;		
	}else{
		
		RETAILMSG(TONE_MOD,("ReleaseRef: => 12\n"));
		while( lpHanlde->lpNext )
		{
			RETAILMSG(TONE_MOD,("ReleaseRef: => 13\n"));
			if( lpHanlde->lpNext == lp )
			{
				RETAILMSG(TONE_MOD,("ReleaseRef: => 14\n"));

				lpHanlde->lpNext = lp->lpNext;
				find = TRUE;
				break;
			}
			lpHanlde = lpHanlde->lpNext;
		}
	}	
	if(find){
		ASSERT(lp->lpModule == lpMod);

		RETAILMSG(TONE_MOD, ( "ELF_FreeModule: fini module \r\n") );

		//ASSERT(ModuleInitFini(lpClient, lp, MODULE_FINI));
		ModuleInitFini(lpClient, lp, MODULE_FINI);

		RETAILMSG(TONE_MOD, ( "ELF_FreeModule: release module handle\r\n") );
		//lpMod->uRef--;
		ReleaseModuleHandle(lp);		
	}
	else
	{
		WARNMSG(TONE_MOD, ( "ELF_FreeModule: find NO module handle\r\n") );
	}

/*	if(lpMod->lpHandle == NULL){	//The module has no handle...
		RETAILMSG(TONE_MOD, ("Exec_module: ->20\r\n"));
		ReleaseModule(lpMod);
	}
*/
	RETAILMSG(TONE_TEST|DEBUG_ReleaseRef, ("ReleaseRef:lpMod->uRef=%x.\r\n", lpMod->uRef));
	lpMod->uRef --;
	

	if(lpMod->uRef == 0){
		RETAILMSG(TONE_MOD, ("Exec_module: ->20\r\n"));
		ASSERT(lpMod->lpHandle == NULL);
		ReleaseModule(lpMod);
	}

	KL_LeaveCriticalSection( &csModuleList );	
	RETAILMSG(TONE_CS, ( "2(-\n"));

	if(bEnter){
		
		KL_LeaveCriticalSection( &csLoadModule );	
		RETAILMSG(TONE_CS, ( "1<===\n"));
	}

	return ;
}

MY_STATIC HANDLE ELF_CreateModule(  LPCTSTR lpszFileName, HANDLE hProcess, UINT uiCreateFlag )
{
	PMODULE_NODE	pMod ;
	PMODULE_CLIENT	pmodule;
	UINT			len;
//	PMODULE_NODE	ploadmod;
//	char			*modulename;
	MODULE_HANDLE	*pModHandle;
	BOOL			bIsLoaded = FALSE;


	RETAILMSG(TONE_MOD, ( "ELF_CreateModule:< %s, %x\r\n", lpszFileName, uiCreateFlag) );

	RETAILMSG(TONE_TEST, ( "->>2\r\n"));

	//2004-10-20, remove by lilin
	//if(bInitModule == 0){
	//	bInitModule = 1;
	//	InitModuleMgr();
	//}
	//

	//note: 2003-09-06...zb
	if(!IsLoadModule(lpszFileName)){
		RETAILMSG(TONE_ERROR, ( "ELF_CreateModule: > can't load the module!\r\n") );	
		return NULL;
	}
	//note: 2003-09-06...zb

	pmodule = CreateModuleClient(lpszFileName,hProcess, uiCreateFlag);
	if(pmodule == NULL){
		RETAILMSG(TONE_ERROR, ( "ELF_CreateModule: > create module client fail!\r\n") );	
		RETAILMSG(TONE_TEST, ( "->>6\r\n"));
		return NULL;
	}

	RETAILMSG(TONE_MOD,( "create Success!\r\n" ));

	RETAILMSG(TONE_MOD, ( "ELF_CreateModule:>\r\n") );

	return (HANDLE)pmodule;
}	

#define DEBUG_ELF_FreeModule 0
MY_STATIC BOOL ELF_FreeModule( HANDLE hModule )
{
	//PMODULE_HANDLE	lp = (PMODULE_HANDLE)hModule;
	PMODULE_CLIENT			lp = (PMODULE_CLIENT)hModule;
	PHANDLE_LIST	lplist;
	PHANDLE_LIST	lpltTemp;


	RETAILMSG(TONE_MOD|DEBUG_ELF_FreeModule, ( "ELF_FreeModule: entry,hModule=0x%x,lp->lpszModuleName=%s.\r\n",hModule, lp->lpszModuleName) );

	//KL_EnterCriticalSection( &csModuleList );	
//	RETAILMSG(TONE_MOD, ( "ELF_FreeModule: free module %s\r\n", lp->lpszModuleName) );

	if(!RemoveClient(lp)){
		ERRORMSG(TONE_MOD, ( "ELF_FreeModule:> no the client\r\n") );
		return FALSE;
	}
	lplist = lp->pHandlelist;
	//Add for fix bug...by zb ...2003-07-08.
	if(lplist == NULL){
		KL_EnterCriticalSection( &csLoadModule );			
		KL_EnterCriticalSection( &csModuleList );	
		RETAILMSG(TONE_MOD, ( "<->\r\n") );
		ReleaseModule(lp->pModule);

		KL_LeaveCriticalSection( &csModuleList );	
		KL_LeaveCriticalSection( &csLoadModule );			
		
	}
	else
	//Add for fix bug...by zb ...2003-07-08.
	{
		RETAILMSG(TONE_MOD|DEBUG_ELF_FreeModule, ( "ELF_FreeModule: lplist:%x\r\n", lplist) );
		while(lplist){			
			//RETAILMSG(TONE_TEST, ( "||\r\n") );
			
			lpltTemp = lplist;
			lplist = lplist->pnext;		//fix bug
			
			ReleaseRef(lp, lpltTemp->pHandle);
			KHeap_Free(lpltTemp, sizeof(HANDLE_LIST));			
			//lplist = lplist->pnext;
		}
	}
	//RETAILMSG(TONE_MOD, ( "||\r\n") );

	//KHeap_Free(lp, sizeof(MODULE_HANDLE));
	//len = strlen( lpszFileName ) + 1;
	//pClient->lpszModuleName = KHeap_Alloc( len * sizeof( TCHAR ) );
	KHeap_FreeString( lp->lpszModuleName );
	lp->lpszModuleName = NULL;

	KHeap_Free(lp, sizeof(MODULE_CLIENT));

	//KL_LeaveCriticalSection( &csModuleList );	

	RETAILMSG(TONE_MOD, ( "ELF_FreeModule: >\r\n") );

	return TRUE;
}

MY_STATIC HANDLE ELF_GetModuleFileHandle( HANDLE hModule )
{
	//PMODULE_HANDLE	lp = (PMODULE_HANDLE)hModule;
	PMODULE_CLIENT				lp = (PMODULE_CLIENT)hModule;
	
	RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileHandle:< :%x:\r\n", lp));	
	if(lp){		
		RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileHandle:< :%x:\r\n", lp->pHandlelist));	
		RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileHandle:< :%s:\r\n", lp->lpszModuleName));	
		if(lp->pHandlelist){
			RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileHandle:< :%x:\r\n", lp->pHandlelist->pHandle));	
			if(lp->pHandlelist->pHandle){
				RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileHandle:< :%x:\r\n", lp->pHandlelist->pHandle->hFile));	
			}
		}
	}

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileHandle:> %x\r\n", lp->pHandlelist->pHandle->hFile) );

	return lp->pHandlelist->pHandle->hFile;
}

MY_STATIC LPCTSTR ELF_GetModuleFileName( HANDLE hModule )
{
	PMODULE_CLIENT				lp = (PMODULE_CLIENT)hModule;

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileName:< \r\n"));

	//RETAILMSG(TONE_MOD, ( "ELF_GetModuleFileName:> %s\r\n", lp->lpszModuleName));

	//return lp->pModule->lpszApName;
	return lp->lpszModuleName;
}

MY_STATIC LPVOID ELF_GetModuleProcAddress( HANDLE hModule, LPCSTR lpProcName )
{
	//PMODULE_HANDLE	lp = (PMODULE_HANDLE)hModule;
	PMODULE_CLIENT				lp = (PMODULE_CLIENT)hModule;
	PFUNCTION_SYM	pTmp;
	UINT			lpProcAddr;

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress:< \r\n"));

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress:< %s\r\n", lpProcName) );

	//RETAILMSG(TONE_TEST, ( "->3\r\n"));

	ASSERT(lp->pModule);
	ASSERT(lp->pHandlelist);
	ASSERT(lp->pHandlelist->pHandle);

	//RETAILMSG(TONE_MOD|TONE_TEST, ( "uType:%x, uMode:%x \r\n",lp->pModule->uType,lp->pHandlelist->pHandle->uMode));
	//RETAILMSG(TONE_MOD|TONE_TEST, ( "module:%s\r\n",lp->pModule->lpszApName));

	ASSERT(lp->pModule->uType == CREATE_LIBRARY);
	ASSERT(lp->pHandlelist->pHandle->uMode == MODULE_LOAD);

	/*if((lp->pModule->uType != CREATE_LIBRARY) || (lp->pHandlelist->pHandle->uMode != MODULE_LOAD)){
		RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress: the module don't support the function!\r\n") );
		RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress:>\r\n") );
		return NULL;
	}*/
	
	//RETAILMSG(TONE_TEST, ( "->4\r\n"));
	//ASSERT(lp->pHandlelist->pHandle->lpProc);

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress: -> 2 \r\n"));
	pTmp = lp->pHandlelist->pHandle->lpProc; 

	while(pTmp)
	{
		if(strcmp((const char *)pTmp->lpFuncName, lpProcName) == 0){
			RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress: find!\r\n") );
			RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress:>\r\n") );
			//RETAILMSG(TONE_TEST, ( "->5\r\n"));
			return (LPVOID)pTmp->lpProc;
		}
		else
			pTmp = pTmp->lpNext;
	}

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress: find new symbol!\n"));

	lpProcAddr = GetNewProcAddr(lp->pHandlelist->pHandle, lpProcName);

	if(lpProcAddr == NULL){
		RETAILMSG(TONE_MOD, ( "ELF_GetModuleProcAddress:> not find!\r\n") );
		//RETAILMSG(TONE_TEST, ( "->6\r\n"));
	}
	
	//RETAILMSG(TONE_TEST, ( "->7\r\n"));
	return (LPVOID)lpProcAddr;
}

MY_STATIC UINT CheckNewModule(struct __dlmodule *__dl_mod, PMODULE_CLIENT lp, UINT	uLoadMode, struct __dlmodule *except_mod)
{
	//struct __dlmodule	*__dl_mod;
	struct __dlmodule	*__dl_temp;
	struct __dlmodule	*__dl_temp2;
	PMODULE_NODE		pNode ;
	PMODULE_HANDLE		ptemp ;
	UINT				uModuleNum = 0;
	UINT				uType;
	//UINT				uIndex =0;

	RETAILMSG(TONE_MOD, ("CheckNewModule : <\n"));

	ASSERT(__dl_mod);
	ASSERT(lp);	
	ASSERT(lp->pModule);

	__dl_temp = __dl_mod;	
	
	while(__dl_temp ){
		__dl_temp2 = __dl_temp;
		__dl_temp = __dl_temp->l_next;
		
		RETAILMSG(TONE_MOD, ("__dl_temp:%x, module name: %s\n", __dl_temp2,__dl_temp2->l_name));
		
		if(__dl_temp2 != except_mod){	
			
			uModuleNum++;
			//ptemp = GetNewModule(__dl_temp2, lp->hProc, MODULE_EXEC);	//??
			
			ASSERT(lp->pModule); 

			//RETAILMSG(TONE_MOD, ("ELF_ExecModule =>12\n"));
			RETAILMSG(TONE_CS, ( "-)3\n"));
			KL_EnterCriticalSection( &csModuleList);
			
			pNode = FindLoadedMod(__dl_temp2->l_name);
			//pNode = CreateModuleNode(__dl_temp2->l_name, ptemp, MODULE_EXEC);				

			ASSERT(pNode);

			ReleaseMapHandle(pNode);

			ptemp = CreateModuleHandle(pNode, uLoadMode, lp->hProc);				
			if(ptemp == NULL){
				KL_LeaveCriticalSection( &csModuleList);
				RETAILMSG(TONE_CS, ( "3(-\n"));
				//KL_LeaveCriticalSection( &csLoadDll );	 
				return 0;
			}			
			
			if(uLoadMode == MODULE_LOAD)
				uType = MODULE_LOAD_DLL;
			else
				uType = MODULE_EXEC_DLL;

			//uType = uLoadMode;

			RETAILMSG(TONE_MOD, ("-->CheckNewModule: uLoadMode:%x\n", uLoadMode));

			SetModuleHandle(ptemp, NULL, __dl_temp2, uType);

			KL_LeaveCriticalSection( &csModuleList);
			RETAILMSG(TONE_CS, ( "4(-\n"));
			
			InsertHandleList(lp, ptemp);

			if(!ModuleInitFini(lp, ptemp, MODULE_INIT)){
				RETAILMSG(TONE_ERROR, ("Module init fail:%s\r\n", lp->lpszModuleName));
			}

			//RETAILMSG(TONE_ERROR, ("Module init fail:%s\r\n", lp->lpszModuleName));

		}
		
		//RETAILMSG(TONE_MOD, ("ELF_ExecModule =>131\n"));
		
		//The line commit to test...
		//__dlclean_module(__dl_temp2);
		__dl_temp2 = NULL;
	}
	
	//if(__dl_temp2 != NULL)
	//	__dlclean_module(__dl_temp2);
	
	//RETAILMSG(TONE_MOD, ("ELF_ExecModule =>14\n"));
	//		__dlfree(__dl_temp);
	
	//__dl_loaded = NULL;
	//__dl_nloaded = 0;

	RETAILMSG(TONE_MOD, ("CheckNewModule : find and create %x new module\n", uModuleNum));
	RETAILMSG(TONE_MOD, ("CheckNewModule : >\n"));

	return uModuleNum;	
}

typedef BOOL (*__init_fun) (PMODULE_CLIENT, DWORD, LPVOID);
typedef BOOL (*__fini_fun) (PMODULE_CLIENT, DWORD, LPVOID);

BOOL ModuleInitFini(PMODULE_CLIENT lp, PMODULE_HANDLE ptemp, UINT flag )
{
	//NOTE: 2003-08-20...zb
	// Fix a bug: no init value...
	//BOOL	result;
	BOOL	result = TRUE;
	//NOTE: 2003-08-20...zb

	RETAILMSG(TONE_MOD|TONE_TEST, ("ModuleInitFini : <\n"));

	
	if(ptemp->lpModule->uType != CREATE_LIBRARY){
		//ASSERT(!ptemp->info);
		RETAILMSG(TONE_MOD|TONE_TEST, ("ModuleInitFini >:%s NON dll\n", lp->lpszModuleName));		
		return FALSE;
	}

	ASSERT(ptemp->info);

	RETAILMSG(TONE_MOD|TONE_TEST, ("ModuleInitFini:%x:%s\n",flag, lp->lpszModuleName));

	if(flag == MODULE_INIT)
	{
		if (ptemp->info->initproc != NULL)
		{
			__init_fun init = (__init_fun)(ptemp->info->addr + ptemp->info->initproc);
			
			//RETAILMSG(TONE_MOD|TONE_TEST, ("init:<\n"));
			result = init (lp, DLL_PROCESS_ATTACH, 0);
			//RETAILMSG(TONE_MOD|TONE_TEST, ("init:>\n"));

			ASSERT(result);
		}else{
			RETAILMSG(TONE_MOD|TONE_TEST, ("Non init:\r\n"));
		}
	}else{
		if (ptemp->info->finiproc != NULL)
		{
			__fini_fun fini = (__fini_fun)(ptemp->info->addr + ptemp->info->finiproc);
	
			//RETAILMSG(TONE_MOD|TONE_TEST, ("fini:<\n"));
			result = fini (lp, DLL_PROCESS_DETACH, 0);
			//RETAILMSG(TONE_MOD|TONE_TEST, ("fini:>\n"));

			ASSERT(result);
		}else{
			RETAILMSG(TONE_MOD|TONE_TEST, ("Non fini:\r\n"));
		}
	}	

	RETAILMSG(TONE_MOD|TONE_TEST, ("ModuleInitFini : >\n"));

	return result;
}

MY_STATIC void CleanLoadedDll()
{
	ASSERT(__dl_loaded);

	//note: 2003-09-04...zb
	//fix a bug of free.
	//__dlclean_module(__dl_loaded);
	__dlclean_module(__dl_loaded, FALSE);
	//note: 2003-09-04...zb

	__dl_loaded = NULL;

	__dl_nloaded = 0;
}


struct __dlmodule *LoadDllModule(PMODULE_CLIENT lp, struct exe_module *pmod, UINT uLoadMode, UINT *uReturn)
{	
	PMODULE_HANDLE		ptemp;
//	struct exe_module	*pmod;
	struct __dlmodule	*pLoadMod = NULL; // lilin init it with NULL -2004-04-13 
	struct __dlmodule	*dl_mod;
	UINT				uType;

	ASSERT(lp);

	DEBUGMSG(TONE_MOD, ( "LoadDllModule: entry( %s ).\r\n", lp->lpszModuleName) );

	KL_EnterCriticalSection( &csLoadModule );	 

	/*
	RETAILMSG(TONE_CS, ( "-)4\n"));
	KL_EnterCriticalSection( &csModuleList);
	//lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, lp->uCreateFlag);
	lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, CREATE_LIBRARY);
	KL_LeaveCriticalSection( &csModuleList);		
	RETAILMSG(TONE_CS, ( "5(-\n"));*/

	//if(uLoadMode == LOAD_DLL_CLEARLY){
	if(uLoadMode == MODULE_LOAD){
		RETAILMSG(TONE_CS, ( "-)4\n"));
		KL_EnterCriticalSection( &csModuleList);
		//lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, lp->uCreateFlag);
		lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, CREATE_LIBRARY);
		// lilin add 2004-04-13
		if( lp->pModule )
		{
		// lilin 2004-04-13
			//KL_LeaveCriticalSection( &csModuleList);		
			//RETAILMSG(TONE_CS, ( "5(-\n"));
		
			RETAILMSG(TONE_MOD, ( "LoadDllModule: => 1\n"));

			//RETAILMSG(TONE_CS, ( "-)5\n"));
			//KL_EnterCriticalSection( &csModuleList);
			//ptemp = FindLoadedHandle(lp->pModule, FIND_MODULE);
			ptemp = FindLoadedHandle(lp->pModule, FIND_NON_XIP_REF);
			KL_LeaveCriticalSection( &csModuleList);		
			RETAILMSG(TONE_CS, ( "6(-\n"));

			if(ptemp != NULL)
				pLoadMod = elf_load(ptemp, lp->pModule->lpszApName, 0);
			else
				pLoadMod = elf_load(NULL, lp->pModule->lpszApName, 0);
		}
		// lilin add 2004-04-13		
		else
		{
			KL_LeaveCriticalSection( &csModuleList);
		}
		// lilin

	//}else if(uLoadMode == LOAD_DLL_QUIETLY){
	}else if(uLoadMode == MODULE_EXEC){

		ASSERT(pmod);
		//RETAILMSG(TONE_MOD, ( "LoadDllModule: => 2\n"));

		RETAILMSG(TONE_MOD, ("dldyn_start: loading quietly!\n"));

		pLoadMod = dldyn_start(pmod->exe_info, lp->pModule->lpszApName);

		RETAILMSG(TONE_MOD, ("Exec_module: ->1!\n"));
	}

	RETAILMSG(TONE_MOD, ( "LoadDllModule: => 3\n"));

	//if(!(pLoadMod = elf_load(pMod, pMod->lpszApName, 0))){
	if(!pLoadMod){
		
		RETAILMSG(TONE_MOD, ( "LoadDllModule: > fail!  %s\r\n", lp->lpszModuleName) );
		//KL_LeaveCriticalSection( &csLoadDll );	 
		
		KL_LeaveCriticalSection( &csLoadModule );	 
		RETAILMSG(TONE_CS, ( "2<===\n"));
		*uReturn = LOAD_FAIL;
		return NULL;
	}

	

	//if(uLoadMode == LOAD_DLL_CLEARLY){
	if(uLoadMode == MODULE_LOAD){
		dl_mod = pLoadMod;
		
		while(dl_mod->l_prev){
			dl_mod = dl_mod->l_prev;
		}
		CheckNewModule(dl_mod, lp, uLoadMode, NULL);
	}else
		CheckNewModule(pLoadMod, lp, uLoadMode, pLoadMod);
	
	CleanLoadedDll();
	
	
	//KL_LeaveCriticalSection( &csLoadDll );	 		
	KL_LeaveCriticalSection( &csLoadModule );	 
	RETAILMSG(TONE_CS, ( "3<===\n"));
	
	RETAILMSG(TONE_MOD, ( "LoadDllModule: > OK!  %s\r\n", lp->lpszModuleName) );
	//RETAILMSG(TONE_MOD, ( "->%s:> loaded dll!\r\n", pMod->lpszApName) );

	*uReturn = LOAD_OK;
	return pLoadMod;	
}

struct exe_module *LoadExeModule(PMODULE_CLIENT lp, UINT uLoadMode, UINT *uReturn)
{
	PMODULE_HANDLE		lpHandle;
	PMODULE_HANDLE		ptemp;
	struct exe_module	*pmod = NULL; //lilin init it with NULL 2004-04-13
	UINT				uType;

	ASSERT(lp);

	DEBUGMSG(TONE_MOD, ( "LoadExeModule: entry( %s ).\r\n", lp->lpszModuleName) );

	KL_EnterCriticalSection( &csLoadModule );	

/*	RETAILMSG(TONE_CS, ( "-)6\n"));
	KL_EnterCriticalSection( &csModuleList);
	//lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, lp->uCreateFlag);
	lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, CREATE_EXE);
	KL_LeaveCriticalSection( &csModuleList);		
	RETAILMSG(TONE_CS, ( "7(-\n"));
*/
	//we think the module is loadable always...
	/*if(lp->uMode != MODULE_EXEC){
		RETAILMSG(TONE_MOD,( "The module can't be exec!\r\n" ));
		goto	exec_exit;
		//return NULL;
	}*/

	//KL_EnterCriticalSection( &csLoadExe );	 
	//KL_EnterCriticalSection( &csModuleList);
	if(uLoadMode == MODULE_EXEC){
		RETAILMSG(TONE_CS, ( "-)6\n"));
		KL_EnterCriticalSection( &csModuleList);
		//lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, lp->uCreateFlag);
		lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, CREATE_EXE);
		// lilin add code -2004-04-13
		if( lp->pModule )
		// lilin 2004-04-13
		{
		//KL_LeaveCriticalSection( &csModuleList);		
		//RETAILMSG(TONE_CS, ( "7(-\n"));
		
		//RETAILMSG(TONE_CS, ( "-)7\n"));
		//KL_EnterCriticalSection( &csModuleList);
		//ptemp = FindLoadedHandle(lp->pModule, FIND_MODULE);
			ptemp = FindLoadedHandle(lp->pModule, FIND_NON_XIP_REF);
			KL_LeaveCriticalSection( &csModuleList);		
			RETAILMSG(TONE_CS, ( "8(-\n"));
		 
			if(ptemp){
				pmod = elf_exec(lp->pModule->lpszApName, ptemp, MODULE_EXEC);
			}else{
				pmod = elf_exec(lp->pModule->lpszApName, NULL, MODULE_EXEC);
			}
		}
		// lilin add code -2004-04-13		
		else
		{
			KL_LeaveCriticalSection( &csModuleList);
		}
		// lilin
	}else{
		RETAILMSG(TONE_CS, ( "-)6\n"));
		KL_EnterCriticalSection( &csModuleList);
		//lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, lp->uCreateFlag);
		lp->pModule = FindOrCreateModule(lp->lpszModuleName, NULL, CREATE_EXE);
		KL_LeaveCriticalSection( &csModuleList);		
		RETAILMSG(TONE_CS, ( "7(-\n"));
		if( lp->pModule )
		{		
		    pmod = elf_exec(lp->pModule->lpszApName, NULL, MODULE_LOAD);
		}
	}
	/*if(lp->pModule->lpBase){
		pmod = elf_exec(lp->pModule->lpszApName, lp->pModule->lpBase, MODULE_EXEC);
	}else{
		pmod = elf_exec(lp->pModule->lpszApName, NULL, MODULE_EXEC);
	}*/


	if(!pmod){
		RETAILMSG(TONE_MOD,( "elf_load_binary fail!\r\n" ));
		//ReleaseModule(lp->pModule);		
		//KL_LeaveCriticalSection( &csLoadExe );	 
		
		KL_LeaveCriticalSection( &csLoadModule );	 
		RETAILMSG(TONE_CS, ( "4<===\n"));
		//KL_LeaveCriticalSection( &csModuleList);
		//goto	exec_exit;
		RETAILMSG(TONE_MOD, ( "LoadExeModule: > fail %s\r\n", lp->lpszModuleName) );
		*uReturn = LOAD_FAIL;
		return NULL;
	}

	RETAILMSG(TONE_CS, ( "-)8\n"));
	KL_EnterCriticalSection( &csModuleList);

	if(uLoadMode == MODULE_EXEC){
		ReleaseMapHandle(lp->pModule);
	}

	lpHandle = CreateModuleHandle(lp->pModule, uLoadMode, lp->hProc);

	if(lpHandle == NULL){
		KL_LeaveCriticalSection( &csModuleList);
		RETAILMSG(TONE_CS, ( "9(-\n"));
		//KL_LeaveCriticalSection( &csLoadExe );	 
		
		KL_LeaveCriticalSection( &csLoadModule );	 
		RETAILMSG(TONE_CS, ( "5<===\n"));
		//goto	exec_exit;
		RETAILMSG(TONE_MOD, ( "LoadExeModule: > fail %s\r\n", lp->lpszModuleName) );
		*uReturn = LOAD_FAIL;
		return pmod;
	}

	if(uLoadMode == MODULE_EXEC)
		uType = MODULE_EXEC_EXE;
	else
		uType = MODULE_LOAD_EXE;

	if(!SetModuleHandle(lpHandle, pmod, NULL, uType)){
		KL_LeaveCriticalSection( &csModuleList);
		RETAILMSG(TONE_CS, ( "10(-\n"));
		//KL_LeaveCriticalSection( &csLoadExe );	 
		
		KL_LeaveCriticalSection( &csLoadModule );	 
		RETAILMSG(TONE_CS, ( "6<===\n"));
		//goto	exec_exit; 
		RETAILMSG(TONE_MOD|TONE_ERROR, ( "LoadExeModule: > fail %s\r\n", lp->lpszModuleName) );
		*uReturn = LOAD_FAIL;
		return pmod;

	}

	//if(uLoadMode == MODULE_EXEC)
	//	InsertEmptyHandleList(lp, lpHandle);
	//else
		InsertHandleList(lp, lpHandle);

	//lp->pHandle = lpHandle;

	KL_LeaveCriticalSection( &csModuleList);
	RETAILMSG(TONE_CS, ( "11(-\n"));
	//KL_LeaveCriticalSection( &csLoadExe );	 
	
	KL_LeaveCriticalSection( &csLoadModule );	 
	RETAILMSG(TONE_CS, ( "7<===\n"));

	RETAILMSG(TONE_MOD, ( "LoadExeModule: > Ok %s\r\n", lp->lpszModuleName) );
	*uReturn = LOAD_OK;
	return pmod;
}

MY_STATIC UINT ELF_LoadModule( HANDLE hModule, HANDLE hInst, LPCTSTR lpszCommandLine, UINT uiLoadFlag )
{
	//PMODULE_NODE	lp = (PMODULE_NODE)hModule;
	PMODULE_CLIENT	lp = (PMODULE_CLIENT)hModule;
//	PMODULE_HANDLE	lp = (PMODULE_HANDLE)hModule;
	struct exe_module	*pmod;
	PMODULE_HANDLE	lpHandle;
	PMODULE_HANDLE	ptemp;
	LPSTART			pstart;
	UINT			uResult;
	UINT			uEntry;
	UINT			flag;
	BOOL			bRet;

	struct __dlmodule	*__dl_mod;
//	struct __dlmodule	*__dl_temp;
//	struct __dlmodule	*__dl_temp2;
//	PMODULE_NODE		pNode ;
//	PMODULE_HANDLE		ptemp ;

	//RETAILMSG(TONE_MOD, ( "ELF_LoadModule: < %s\r\n", lp->pModule->lpszApName) );
	RETAILMSG(TONE_MOD, ( "ELF_LoadModule: < %s\r\n", lp->lpszModuleName) );

	RETAILMSG(TONE_TEST, ( "->>1\r\n"));

	if(uiLoadFlag&LF_LOAD_TO_MEM){
		if(uiLoadFlag & LF_EXEC){
			//RETAILMSG(TONE_MOD, ( "ELF_LoadModule: exec a module\n"));
			if(lp->uCreateFlag == CREATE_LIBRARY)
				flag = MODULE_LOAD;			
			else
				flag = MODULE_EXEC;		
		}else{
			//RETAILMSG(TONE_MOD, ( "ELF_LoadModule: load a module\n"));
			flag = MODULE_LOAD;			
		}
	}

	//RETAILMSG(TONE_TEST, ( "->>1\r\n"));

	bRet = LoadModule(lp, flag, &uEntry);

	//RETAILMSG(TONE_TEST, ( "->1\r\n"));

	if(!bRet){
		RETAILMSG(TONE_MOD, ( "ELF_LoadModule: > fail!\r\n") );
		return 0;
	}

	if((flag == MODULE_EXEC)&&(lp->uCreateFlag == CREATE_EXE)){
//		extern void CallUserStartupCode( DWORD ip );

		//RETAILMSG(TONE_TEST, ( "->2\r\n"));

		pstart = (LPSTART)(uEntry);

		RETAILMSG(1, ( "ELF_LoadModule: start exec %s, 0x%x.\r\n", lp->lpszModuleName, pstart ) );
//        pstart();
		CallUserStartupCode( pstart, 0 ); // 2004-02-26, add by lilin
	}

	RETAILMSG(TONE_MOD, ( "ELF_LoadModule: > %s\r\n", lp->lpszModuleName) );

	return 0;
}




PMODULE_NODE FindLoadedMod( LPCTSTR lpcszApName )
{
	char			*modulename;
	char			*pApName;
	PMODULE_NODE	lp;
	
	modulename = FormatModuleName(lpcszApName);

	//KL_EnterCriticalSection( &csModuleList );	 

	lp = lpElfModList;

	while( lp )
	{
		RETAILMSG(TONE_MOD, ( "FindLoadedMod: %s <==> %s\r\n",lp->lpszApName, modulename ) );
		//if( stricmp( lp->lpszApName, lpcszApName ) == 0 )		

		//note: 2003-09-16...zb
		//if( strstr( lp->lpszApName, modulename ) != NULL )
		pApName = FormatModuleName(lp->lpszApName);
		if(stricmp(pApName, modulename) == 0)
		//note: 2003-09-16...zb
		{
			RETAILMSG(TONE_MOD, ( "FindLoadedMod: find\r\n") );

			//RETAILMSG(TONE_TEST, ( "find:%s\r\n",lpcszApName) );

			//KL_LeaveCriticalSection( &csModuleList );	 
			return lp;
		}
		lp = lp->lpNext;
	}
	//KL_LeaveCriticalSection( &csModuleList );	 

	//RETAILMSG(TONE_TEST, ( "0find:%s\r\n",lpcszApName) );
	RETAILMSG(TONE_MOD, ( "FindLoadedMod: don't load it!\r\n") );
	return NULL;
}


MY_STATIC DWORD  ELF_GetModuleSectionOffset( HANDLE hModule, LPCTSTR lpcszSectionName )
{
	//PMODULE_HANDLE	lpmod = (PMODULE_HANDLE)hModule;
	PMODULE_CLIENT				lpmod = (PMODULE_CLIENT)hModule;
	int				i;
	int				inum = 0;
	struct module_seg		*seg_info;

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleSectionOffset:< %s\r\n",lpcszSectionName ) );


	ASSERT(strcmp(lpcszSectionName, ".res") == 0);

	/*if(lpmod->uMode != MODULE_LOAD){
		RETAILMSG(TONE_MOD, ("The module is n't load memory by way of MODULE_LOAD!\n"));
		return 0;
	}*/

	//RETAILMSG(TONE_MOD, ("lpmod->seg_info: %x\n", lpmod->seg_info));

	//seg_info = lpmod->pHandlelist->pHandle->seg_info;
	seg_info = lpmod->pModule->seg_info;

	while(seg_info){
		RETAILMSG(TONE_MOD, ("seg_info->sh_name: %s\n", seg_info->sh_name));

		if(strcmp(seg_info->sh_name, lpcszSectionName) == 0){
			RETAILMSG(TONE_MOD, ( "ELF_GetModuleSectionOffset:> find \r\n") );
			return seg_info->sh_offset;
		}
		seg_info = seg_info->lpNext;
	}

	RETAILMSG(TONE_MOD, ( "ELF_GetModuleSectionOffset:> fail \r\n") );
	return 0;
}

/*
MY_STATIC HANDLE ELF_FindModuleRes(HANDLE hModule,LPCTSTR lpName, LPCTSTR lpType)
{


}

MY_STATIC HANDLE ELF_LoadModuleRes(HANDLE hModule, HANDLE hResInfo)
{

}*/


//#define		LOAD_MODULE_NEW		1
//#define		LOAD_MODULE_REF		2

//extern struct module_seg *GetSegInfo(HANDLE hFile, Elf32_Ehdr	*ehdr);


//MY_STATIC PMODULE_HANDLE LoadModule(PMODULE_NODE pMod, HANDLE hProc,UINT uLoadMode, PMODULE_CLIENT pClient)
MY_STATIC BOOL LoadModule(PMODULE_CLIENT pClient, UINT	uLoadFlag, UINT	*lpEntry)
{
	struct __dlmodule* pLoadMod;
	struct __dlmodule* dl_mod;
	PMODULE_HANDLE	   pModHandle;
	struct exe_module	*pmod;
	PMODULE_HANDLE		ptmp;
	UINT				uResult;
	struct __dlmodule	*__dl_mod;

	RETAILMSG(TONE_MOD, ( "LoadModule:< \r\n") );

	ASSERT(lpEntry);
//	ASSERT(pMod);
	ASSERT(pClient);
	//ASSERT(pClient->pModule);

	if((uLoadFlag == MODULE_EXEC)&&(pClient->uCreateFlag == CREATE_EXE)){

		pmod = LoadExeModule(pClient, MODULE_EXEC, &uResult);
		
		if(uResult == LOAD_FAIL){
			RETAILMSG(TONE_MOD, ( "ELF_LoadModule: > fail %s\r\n", pClient->lpszModuleName) );
			return 0;
		}
		
		if(pmod->entry == DL_DYN_ENTRY){
			
			//RETAILMSG(TONE_MOD, ( "ELF_LoadModule: => 1\n"));
			
			//__dl_mod = LoadDllModule(pClient, pmod, LOAD_DLL_QUIETLY, &uResult);
			__dl_mod = LoadDllModule(pClient, pmod, MODULE_EXEC, &uResult);
			
			if(uResult == LOAD_FAIL){
				RETAILMSG(TONE_MOD, ( "ELF_LoadModule: > fail %s\r\n", pClient->lpszModuleName) );
				return 0;
			}
			
			*lpEntry = __dl_mod->l_entry;
			//pstart = (LPSTART)(__dl_mod->l_entry);
			
		}else	
			//The codes is test code, future these will be changed ...
		{
			*lpEntry = pmod->exe_info->entry;
			//pstart = (LPSTART)pmod->exe_info->entry;
			
			//RETAILMSG(TONE_MOD,( "entry the app process!\r\n" ));		
		}
		
		return TRUE;
	}

	if(pClient->uCreateFlag == CREATE_EXE){
		LoadExeModule(pClient, uLoadFlag, &uResult);
	}else{
		//LoadDllModule(pClient, MODULE_LOAD, &uResult);
		//LoadDllModule(pClient, NULL, LOAD_DLL_CLEARLY, &uResult);
		RETAILMSG(TONE_TEST, ( "->>3\r\n"));
		LoadDllModule(pClient, NULL, uLoadFlag, &uResult);
	}

	if(uResult == LOAD_FAIL){
		RETAILMSG(TONE_MOD, ( "LoadModule:> FAIL\r\n") );
		return FALSE;
	}else{
		RETAILMSG(TONE_MOD, ( "LoadModule:> OK\r\n") );
		return TRUE;
	}
}

/*MY_STATIC BOOL GetModuleType(PMODULE_NODE pMod, UINT flag)
{
	char	*pTmp = NULL;
	int		ch = '.';


	//RETAILMSG(TONE_MOD, ( "GetModuleType:< %s\r\n", pMod->lpszApName) );

	if(!(pTmp = strrchr(pMod->lpszApName, ch))){

		if(flag == MODULE_LOAD){
			pMod->uType = CREATE_LIBRARY;
			//strcat(pMod->lpszApName, DLL_EXTENSION);
		}
		else {
			pMod->uType = CREATE_EXE;
			//strcat(pMod->lpszApName, EXE_EXTENSION);
		}

		//RETAILMSG(TONE_MOD, ( "GetModuleType:>1 :%x:\r\n", pMod->uType) );
		return TRUE;
	}
	pTmp++;


	//RETAILMSG(TONE_MOD, ( "GetModuleType: %s\r\n", pTmp));

	if(!strcmp(pTmp, EXE_EXTENSION))
		pMod->uType = CREATE_EXE;
	else if(!strcmp(pTmp, DLL_EXTENSION))
		pMod->uType = CREATE_LIBRARY;
	else{
		//RETAILMSG(TONE_MOD,("The file is not support format!\n"));
		return FALSE;
	}

	//RETAILMSG(TONE_MOD, ( "GetModuleType:>2 :%x:\r\n", pMod->uType) );
	return TRUE;		
}*/

MY_STATIC char	*FormatModuleName(LPCTSTR lpszFileName)
{
	char	*pTmp = NULL;
	int		ch = '\\';


	//RETAILMSG(TONE_MOD, ( "FormatModuleName:< %s\r\n", lpszFileName) );

	if(!(pTmp = strrchr(lpszFileName, ch))){

		pTmp = (char*)lpszFileName;

		//RETAILMSG(TONE_MOD, ( "FormatModuleName:> %s1\r\n", pTmp) );
		return pTmp;
	}
	pTmp++;

	//RETAILMSG(TONE_MOD, ( "FormatModuleName:> %s 1\r\n", pTmp) );
	return pTmp;		
}

MY_STATIC BOOL  SetModuleHandle(PMODULE_HANDLE lp, struct exe_module *pmod, struct __dlmodule *__dl_module, UINT flag )
{
	DWORD		dwReturn = -1;


	RETAILMSG(TONE_MOD, ( "SetModuleHandle:< \r\n") );

	ASSERT(lp);
//	ASSERT(pmod);
//	ASSERT(__dl_module);

	if(flag != MODULE_LOAD_EXE){
		//2004-10-20
		lp->info = (LOAD_MODULE *)KHeap_Alloc( sizeof( LOAD_MODULE ));	
		//lp->info = (LOAD_MODULE *)_kalloc(sizeof( LOAD_MODULE ));//KHeap_Alloc( sizeof( LOAD_MODULE ));	
		//
		if(lp->info == NULL)
		{
			RETAILMSG(TONE_MOD,( "KHeap_Alloc fail!\r\n" ));
			return FALSE;
		}
		
		RETAILMSG(TONE_MOD, ( "SetModuleHandle: -> 1 \r\n") );

		if(flag == MODULE_EXEC_EXE){
			RETAILMSG(TONE_MOD, ( "SetModuleHandle: -> 2 \r\n") );
			//memcpy(lp->info, pmod->exe_info, sizeof( LOAD_MODULE )); 
			lp->info->start = pmod->exe_info->start;
			lp->info->end   = pmod->exe_info->end;
			lp->info->addr  = pmod->exe_info->addr;
			lp->info->entry = pmod->exe_info->entry;

		}else{
			//if(lp->lpModule->uPos == UNDEF_MODULE)
			//	lp->lpModule->uPos = __dl_module->filetype;

			ASSERT(__dl_module);

			RETAILMSG(TONE_MOD, ( "SetModuleHandle: -> 3 \r\n") );
			//lp->info->nphdr = __dl_module->l_phnum;
			lp->info->addr  = __dl_module->l_addr;
			lp->info->start  = __dl_module->l_map_start;
			lp->info->end  = __dl_module->l_map_end;
			lp->info->entry  = __dl_module->l_entry;
			//lp->info->nld  = 0;
			lp->info->strtab = (const void *) D_PTR (__dl_module, l_info[DT_STRTAB]);
			lp->info->symtab = (const void *) D_PTR (__dl_module, l_info[DT_SYMTAB]);
			lp->info->l_nchain = __dl_module->l_nchain;

			if(__dl_module->l_info[DT_INIT])
				lp->info->initproc = D_PTR (__dl_module, l_info[DT_INIT]);
			else
				lp->info->initproc = NULL;

			if(__dl_module->l_info[DT_FINI])
				lp->info->finiproc = D_PTR (__dl_module, l_info[DT_FINI]);
			else
				lp->info->finiproc = NULL;
		}
		
		/*lp->info->ld = NULL;
		lp->info->phdr = (Elf32_Phdr *)KHeap_Alloc( sizeof( Elf32_Phdr) * lp->info->nphdr);	
		if(lp->info->phdr == NULL){
			RETAILMSG(TONE_MOD,( "KHeap_Alloc fail!\r\n" ));
			KHeap_Free( lp->info, sizeof( LOAD_MODULE ));
			return FALSE;
		}
		
		if(flag == MODULE_EXEC_EXE)
			memcpy(lp->info->phdr, pmod->exe_info->phdr, sizeof( Elf32_Phdr) * lp->info->nphdr);
		else
			memcpy(lp->info->phdr, __dl_module->l_phdr, sizeof( Elf32_Phdr) * lp->info->nphdr);
		*/
	}
	
	//if((flag == CREATE_EXE) || (flag == MODULE_LOAD_EXE))
	//	lp->seg_info = pmod->seg_info;
	
	
	if((flag == MODULE_EXEC_EXE) || (flag == MODULE_LOAD_EXE))
		lp->hFile    = pmod->hFile;
	else
		lp->hFile = __dl_module->hFile;
	
	if(lp->lpModule->seg_info == NULL){		//以前未获取过module的段信息.
		
		RETAILMSG(TONE_DEBUG, ("SEG1\r\n"));

		if(lp->lpModule->uPos != XIP_MODULE){		
			RETAILMSG(TONE_DEBUG, ("SEG2\r\n"));
			//Add for test...
			//if(lp->lpModule->seg_info == NULL)
			lp->lpModule->seg_info = GetSegInfo(lp->hFile);
		}else{
			RETAILMSG(TONE_DEBUG, ("SEG3\r\n"));
			lp->lpModule->seg_info = (struct module_seg *)KHeap_Alloc(sizeof(struct module_seg ));//_kalloc (sizeof(struct module_seg ));
			if(lp->lpModule->seg_info == NULL){
				RETAILMSG(TONE_ERROR, ("SetModuleHandle: _kalloc fail\r\n"));
				return FALSE;
			}	
			
			//
			//Add the line to fix bug...
			//
			memset(lp->lpModule->seg_info, 0, sizeof(struct module_seg ));

			if(!DeviceIoControl(lp->hFile, IOCTL_ROM_GET_SEG_RES, NULL, 0, lp->lpModule->seg_info, 0, &dwReturn, NULL)){				
				KHeap_Free(lp->lpModule->seg_info,sizeof(struct module_seg ));//_kfree(lp->lpModule->seg_info);
				lp->lpModule->seg_info = NULL;
				
				/*if(dwReturn == 0){	
					RETAILMSG(TONE_ERROR, ("SetModuleHandle: no res seg\r\n"));
					return TRUE;
				}else{
					RETAILMSG(TONE_ERROR, ("SetModuleHandle: get res seg fail\r\n"));
					return FALSE;
				}*/

				RETAILMSG(TONE_ERROR, ("SetModuleHandle: get res seg fail\r\n"));
				return FALSE;
			}
			if(dwReturn == 0){
				KHeap_Free(lp->lpModule->seg_info,sizeof(struct module_seg ));
				//_kfree(lp->lpModule->seg_info);
				lp->lpModule->seg_info = NULL;
				RETAILMSG(TONE_DEBUG, ("SetModuleHandle: no res seg\r\n"));
				return TRUE;
			}

			lp->lpModule->seg_info->sh_name = (char*)_kalloc(sizeof(SECT_NAME_RES) + 1);
			if(lp->lpModule->seg_info->sh_name == NULL){
				//_kfree(lp->lpModule->seg_info);
				KHeap_Free(lp->lpModule->seg_info,sizeof(struct module_seg ));
				lp->lpModule->seg_info = NULL;

				RETAILMSG(TONE_ERROR, ("SetModuleHandle: _kalloc fail\r\n"));
				return FALSE;
			}
			
			strcpy(lp->lpModule->seg_info->sh_name , SECT_NAME_RES);
		}
	}

	RETAILMSG(TONE_MOD, ( "SetModuleHandle:> \r\n") );
	return TRUE;
}

/*MY_STATIC PMODULE_HANDLE GetNewModule(struct __dlmodule *__dl_module, HANDLE hProc, UINT flag)
{
	//struct exe_module	*pmod;
	PMODULE_HANDLE		pMod ;
	int					len;
	
	RETAILMSG(TONE_MOD, ( "GetNewModule: <\r\n") );
	
	pMod = (PMODULE_HANDLE)KHeap_Alloc( sizeof( MODULE_HANDLE ));	

	if(pMod == NULL){
		RETAILMSG(TONE_MOD,("GetNewModule  fail!\r\n"));
		RETAILMSG(TONE_MOD, ( "GetNewModule: >\r\n") );

		return NULL;
	}


	//init the members of struct MODULE_NODE.
	memset(pMod, 0, sizeof(MODULE_HANDLE));

	pMod->uRef++;
	pMod->hProc = hProc;
	pMod->uMode = flag;	
	//pMod->uRef	= 1;

//	if(flag == MODULE_LOAD){
//		LoadModule(pMod);
//	}else{
//		if( pMod->uType != CREATE_EXE)
//			goto createfail;
//	}

//	pMod->lpStart = __dl_module->l_entry;	

	pMod->info = (LOAD_MODULE *)KHeap_Alloc( sizeof( LOAD_MODULE ));	
	if(pMod->info == NULL)
	{
		RETAILMSG(TONE_MOD,( "KHeap_Alloc fail!\r\n" ));
		KHeap_Free( pMod, sizeof( MODULE_HANDLE ) );
		return 0;
	}

	//memcpy(lp->info, pmod->exe_info, sizeof( LOAD_MODULE )); 
	pMod->info->nphdr = __dl_module->l_phnum;
	pMod->info->addr  = __dl_module->l_addr;
	pMod->info->start  = __dl_module->l_map_start;
	pMod->info->end  = __dl_module->l_map_end;
	pMod->info->entry  = __dl_module->l_entry;
	pMod->info->nld  = 0;
	//pMod->info->hFile  = NULL;
	pMod->hFile = __dl_module->hFile;

	pMod->info->ld = NULL;
	pMod->info->phdr = (Elf32_Phdr *)KHeap_Alloc( sizeof( Elf32_Phdr) * pMod->info->nphdr);	
	if(pMod->info->phdr == NULL){
		RETAILMSG(TONE_MOD,( "KHeap_Alloc fail!\r\n" ));
		KHeap_Free( pMod->info, sizeof( LOAD_MODULE ));
		KHeap_Free( pMod, sizeof( MODULE_HANDLE ) );
		return 0;
	}

	memcpy(pMod->info->phdr, __dl_module->l_phdr, sizeof( Elf32_Phdr) * pMod->info->nphdr);
	//memcpy(lp->info->ld, pmod->exe_info->ld, sizeof( Elf32_Dyn) * lp->info->nld);

	RETAILMSG(TONE_MOD, ( "GetNewModule: > OK\r\n") );

	return pMod;
}*/


/*BOOL	GetDllSegInfo(HANDLE hFile, PMODULE_HANDLE	lpHandle)
{
	Elf32_Ehdr	ehdr;
	
	RETAILMSG(TONE_MOD, ("GetDllSegInfo: <\n"));

	if (read_file (hFile, (char*)&ehdr, sizeof(Elf32_Ehdr), 0) != sizeof(Elf32_Ehdr))
	{
		RETAILMSG(TONE_MOD, ("GetDllSegInfo: >xx\n"));
		return FALSE;
	}

	RETAILMSG(TONE_MOD, ("lpHandle->seg_info: %x\n", lpHandle->seg_info));

	lpHandle->seg_info = GetSegInfo(hFile, &ehdr);

	RETAILMSG(TONE_MOD, ("lpHandle->seg_info: %x\n", lpHandle->seg_info));

	if(lpHandle->seg_info == NULL){
		RETAILMSG(TONE_MOD, ("GetSegInfo fail!\n"));
		return FALSE;		
	}

	RETAILMSG(TONE_MOD, ("GetDllSegInfo: 11xx\n"));

	RETAILMSG(TONE_MOD, ("GetDllSegInfo: >ok\n"));
	return	TRUE;	
}*/

#define DEBUG_GetModulePos 0
void GetModulePos(PMODULE_NODE lpMod)
{
	char			lpszBin[MAX_PATH];
	DWORD			dwFileAttr;

	DEBUGMSG(TONE_MOD|DEBUG_GetModulePos,("GetModulePos entry(%s).\r\n", lpMod->lpszApName));

	//RETAILMSG(TONE_TEST,("F1:%s\r\n", lpMod->lpszApName));

	if(!elf_find_binary(lpMod->lpszApName, lpszBin)){
		RETAILMSG(TONE_ERROR|1, ("GetModulePos: %s no find!\r\n", lpMod->lpszApName));

		RETAILMSG(TONE_MOD,("GetModulePos:> fail !\n"));
		return ;
	}

	RETAILMSG(TONE_MOD,("::%s\r\n",lpszBin ));

	dwFileAttr = GetFileAttributes(lpszBin);

	ASSERT(dwFileAttr != (DWORD)-1);

	//RETAILMSG(TONE_EXEC,("-- file attr:%x !\n", dwFileAttr));

	if(dwFileAttr & FILE_ATTRIBUTE_ROMMODULE )
	{
		RETAILMSG(TONE_MOD, ("=xip\r\n"));
		lpMod->uPos = XIP_MODULE;
		//uFileType = ROM_MODULE;
	}else{
		RETAILMSG(TONE_MOD, ("=non_xip\r\n"));
		lpMod->uPos = NON_XIP_MODULE;
	}
	//RETAILMSG(TONE_TEST,("F2:%s\r\n", lpMod->lpszApName));
	DEBUGMSG(TONE_MOD|DEBUG_GetModulePos,("GetModulePos leave(%s).\r\n", lpMod->lpszApName));
	return ;
}

//note: 2003-09-06...zb
#define DEBUG_IsLoadModule 0
BOOL IsLoadModule(LPCTSTR lpszFileName)
{
	char			lpszBin[MAX_PATH];
	DWORD			dwFileAttr;

	DEBUGMSG(TONE_MOD | DEBUG_IsLoadModule,("IsLoadModule: entry( %s ).\n", lpszFileName));

	//RETAILMSG(TONE_TEST,("F1:%s\r\n", lpMod->lpszApName));

	if(!elf_find_binary(lpszFileName, lpszBin)){
		WARNMSG(TONE_ERROR|DEBUG_IsLoadModule, ("IsLoadModule: %s no find!\r\n", lpszFileName));
		return FALSE;
	}

	DEBUGMSG(TONE_MOD|DEBUG_IsLoadModule,("IsLoadModule lpszBin=%s.\r\n",lpszBin ));

	dwFileAttr = GetFileAttributes(lpszBin);

	ASSERT(dwFileAttr != (DWORD)-1);

	//RETAILMSG(TONE_EXEC,("-- file attr:%x !\n", dwFileAttr));

	if(dwFileAttr & FILE_ATTRIBUTE_ROMMODULE )
	{
		int leng;
		//RETAILMSG(TONE_MOD, ("=xip\r\n"));
		leng = strlen("\\kingmos\\");
		*(lpszBin + leng) = 0;
		if(stricmp(lpszBin, "\\kingmos\\") != 0){
			WARNMSG(DEBUG_IsLoadModule, ("IsLoadModule: xip module %s isn't  in rom!\r\n", lpszFileName));
			return FALSE;
		}		
	}else{
		//RETAILMSG(TONE_MOD, ("=non_xip\r\n"));		
	}
	
	DEBUGMSG(TONE_MOD|DEBUG_IsLoadModule,("IsLoadModule:leave(%s).!\n", lpszFileName));
	return TRUE;
}//note: 2003-09-06...zb

#define DEBUG_CreateModuleNode 0
PMODULE_NODE CreateModuleNode(LPCTSTR lpszFileName, UINT flag)
{
	PMODULE_NODE	pMod ;
//	UINT			len;
	//char			*modulename;
	//MODULE_HANDLE	*pModHandle;

	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("CreateModuleNode: entry:%s:\n", lpszFileName));

	//KL_EnterCriticalSection( &csModuleList );	 		
	//2004-10-20
	pMod = (PMODULE_NODE)KHeap_Alloc( sizeof( MODULE_NODE ));	
	//pMod = (PMODULE_NODE)_kalloc(sizeof( MODULE_NODE ));//KHeap_Alloc( sizeof( MODULE_NODE ));	
	//

	if(pMod == NULL){
		WARNMSG(TONE_ERROR|DEBUG_CreateModuleNode, ("CreateModuleNode: alloc struct MODULE_NODE fail! \n"));
		//KL_LeaveCriticalSection( &csModuleList );	 
		return NULL;
	}
	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("1111.\n" ) );
	//init the members of struct MODULE_NODE.
	memset(pMod, 0, sizeof(MODULE_NODE));

	pMod->lpNext = lpElfModList;
	lpElfModList = pMod;		

	pMod->uRef  = 1;
	//len = strlen( lpszFileName ) + 1;
	//2004-10-20
	//DEBUGMSG(TONE_MOD|1, ("Call KHeap_Alloc(%s).\n", lpszFileName ) );
	pMod->lpszApName = KHeap_AllocString( ( strlen( lpszFileName ) + 1) * sizeof( TCHAR ) );//KHeap_Alloc( len * sizeof( TCHAR ) );
	//pMod->lpszApName = _kalloc( len * sizeof( TCHAR ) );
	//

	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("2222.\n" ) );	
	if( pMod->lpszApName == NULL ){
		//2004-10-20
		KHeap_Free( pMod, sizeof( MODULE_NODE ) );
		//_kfree( pMod );
		//
		//KL_LeaveCriticalSection( &csModuleList );	 
		WARNMSG( TONE_MOD |DEBUG_CreateModuleNode, ("CreateModuleNode: alloc pMod->lpszApName failure.\r\n"));
		return NULL;
	}
	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("3333.\n" ) );	
	strcpy( pMod->lpszApName, lpszFileName );	
	pMod->uType = flag;

	//RETAILMSG(1, ("Calling GetModulePos!\r\n"));
	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("GetModulePos++.\n" ) );	
	GetModulePos(pMod);
	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("GetModulePos--.\n" ) );		

/*	file = strrchr (lpszFileName, '\\');
	if(file != NULL){
		if(strstr(lpszFileName, KING_PATH)){
			pMod->uPos = ROM_MODULE;
		}else{
			pMod->uPos = NORMAL_MODULE;
		}
	}*/
	//GetModuleType(pMod, flag);

	//KL_LeaveCriticalSection( &csModuleList );	 
	
	DEBUGMSG(TONE_MOD|DEBUG_CreateModuleNode, ("CreateModuleNode: leave.\r\n"));

	return pMod;
}


PMODULE_NODE FindOrCreateModule(LPCTSTR lpszFileName, UINT *uResult, UINT flag)
{
	PMODULE_NODE	pMod ;
	UINT			len;
	//char			*modulename;
	//MODULE_HANDLE	*pModHandle;

	RETAILMSG(TONE_MOD, ("FindOrCreateModule: <:%s:\n", lpszFileName));

	if(pMod = FindLoadedMod( lpszFileName )){		

		ASSERT(pMod->lpHandle);

		pMod->uRef ++;	//ref increase!!!	
		//RETAILMSG(TONE_TEST, ("Ref<:%x\r\n", pMod->uRef));
		/*if((pMod->uType == CREATE_LIBRARY) || 
			((pMod->uType == CREATE_EXE) && (pMod->lpHandle->uMode == MODULE_EXEC))){	

			pMod->lpHandle->uRef ++;	//ref increase!!!	
		}*/
		if(uResult)
			*uResult = FIND_MODULE;

		RETAILMSG(TONE_TEST, ( "find:%x:%s\r\n",pMod->uRef,lpszFileName) );
		//RETAILMSG(TONE_TEST, ("uRef:%x:%s\n", pMod->uRef,lpszFileName));		
	}else{
		if(uResult)
			*uResult = NEW_MODULE;

		RETAILMSG(TONE_TEST, ( "new:%s\r\n",lpszFileName) );
		pMod = CreateModuleNode(lpszFileName, flag);

		ASSERT(pMod);
	}
	
	RETAILMSG(TONE_MOD, ("FindOrCreateModule: >\n"));

	return pMod;
}

MY_STATIC PMODULE_CLIENT	CreateModuleClient( LPCTSTR lpszFileName, HANDLE hProcess, UINT uiCreateFlag)
{
	PMODULE_CLIENT		pClient;
	//UINT				len;

	pClient = (PMODULE_CLIENT)KHeap_Alloc( sizeof( MODULE_CLIENT ));	
	
	if(pClient == NULL){			
		RETAILMSG(TONE_ERROR, ( "CreateModuleClient: alloc fail!\r\n") );			
		return NULL;
	}
	memset(pClient, 0, sizeof(MODULE_CLIENT));

	//len = strlen( lpszFileName ) + 1;
	//pClient->lpszModuleName = KHeap_Alloc( len * sizeof( TCHAR ) );
	pClient->lpszModuleName = KHeap_AllocString( strlen( lpszFileName ) + 1 );
	
	if( pClient->lpszModuleName == NULL ){
		KHeap_Free( pClient, sizeof( MODULE_CLIENT ) );
		//KL_LeaveCriticalSection( &csModuleList );	 
		RETAILMSG(TONE_ERROR, ("ELF_CreateModule: KHeap_Alloc fail>1\n"));
		return NULL;
	}
	strcpy( pClient->lpszModuleName, lpszFileName );	

	pClient->hProc = hProcess;

	pClient->uCreateFlag = uiCreateFlag;

	/*if(strstr(lpszFileName, KING_PATH)){
		if(uiCreateFlag == CREATE_LIBRARY)
			pClient->uCreateFlag = ROM_MODULE_DLL;
		else
			pClient->uCreateFlag = ROM_MODULE_EXE;
	}else{
		if(uiCreateFlag == CREATE_LIBRARY)
			pClient->uCreateFlag = NORMAL_MODULE_DLL;
		else
			pClient->uCreateFlag = NORMAL_MODULE_EXE;
	}*/

	InsertClient(pClient);

	//RETAILMSG(TONE_ERROR, ("CreateModuleClient: >OK\n"));

	return pClient;
}

#define DEBUG_CreateModuleHandle 0
MY_STATIC PMODULE_HANDLE CreateModuleHandle(PMODULE_NODE pMod, UINT uMode, HANDLE hProcess)
{
	PMODULE_HANDLE		pModHandle;

	RETAILMSG(TONE_MOD|DEBUG_CreateModuleHandle, ( "CreateModuleHandle:< for %s, uMode:%x \r\n", pMod->lpszApName, uMode) );
	
	pModHandle = (PMODULE_HANDLE)KHeap_Alloc( sizeof( MODULE_HANDLE ));	
	
	if(pModHandle == NULL){			
		RETAILMSG(TONE_MOD, ( "ELF_CreateModule: alloc fail!\r\n") );			
		return NULL;
	}
	//init the members of struct MODULE_NODE.
	memset(pModHandle, 0, sizeof(MODULE_HANDLE));
	
	pModHandle->uRef = 1;
	//pModHandle->uRef++;
	//pModHandle->uMode = MODULE_EXEC;	
	pModHandle->uMode = uMode;	
	pModHandle->hProc = hProcess;
	
	pModHandle->lpModule = pMod;
	pModHandle->lpNext = NULL;

	/*if(pMod->lpBase != NULL){				//new add...
		//RETAILMSG(TONE_TEST, ( "ELF_CreateModule: %s ref base!\r\n", pMod->lpszApName) );			
		RETAILMSG(TONE_TEST, ( ">> %s\r\n", pMod->lpszApName) );			
		pModHandle->lpRef = pMod->lpBase;
		pMod->lpBase->uRef ++;
	}else if(((pMod->uType == CREATE_EXE) && (uMode == MODULE_EXEC)) ||
				(pMod->uType == CREATE_LIBRARY)){
		//RETAILMSG(TONE_TEST, ( "ELF_CreateModule: %s new base!\r\n", pMod->lpszApName) );			
		RETAILMSG(TONE_TEST, ( "<< %s\r\n", pMod->lpszApName) );			
		pMod->lpBase = pModHandle;
	}*/
	
	if(pMod->lpHandle){
		PMODULE_HANDLE	pTmp = pMod->lpHandle;		
		
		/*if(pMod->lpHandle->uMode != MODULE_EXEC){
			pModHandle->lpNext = pMod->lpHandle;
			pMod->lpHandle = pModHandle;
			RETAILMSG(TONE_MOD, ( "CreateModuleHandle: insert head \r\n") );
		}else{*/				
			while(pTmp->lpNext)
				pTmp = pTmp->lpNext;

			pTmp->lpNext = pModHandle;
			RETAILMSG(TONE_MOD, ( "CreateModuleHandle: insert tail \r\n") );
		//}
		
	}else{
		RETAILMSG(TONE_MOD, ( "CreateModuleHandle: insert first \r\n") );
		pMod->lpHandle = pModHandle;
	}			
	
	RETAILMSG(TONE_MOD, ( "CreateModuleHandle:> \r\n") );

	return pModHandle;
}
MY_STATIC void InsertEmptyHandleList(PMODULE_CLIENT pmodule,PMODULE_HANDLE lpHandle)
{
	PHANDLE_LIST	ptemp;
	PHANDLE_LIST	ptemp2;

	RETAILMSG(TONE_MOD, ("InsertEmptyHandleList: <!\n"));

	ptemp2 = ptemp = pmodule->pHandlelist;

	while(ptemp){
		ptemp2 = ptemp;
		if(ptemp2->pHandle == NULL){
			ptemp2->pHandle = lpHandle;			
			return ;
		}
		ptemp = ptemp->pnext;
	}
	RETAILMSG(TONE_MOD, ("InsertEmptyHandleList: NO find empty client solt!\n"));
	return ;
}

MY_STATIC void InsertHandleList(PMODULE_CLIENT pmodule, PMODULE_HANDLE pModHandle)
{
	PHANDLE_LIST	plist;
	PHANDLE_LIST	ptemp;
	PHANDLE_LIST	ptemp2;

	ASSERT(pmodule);

	ASSERT(pmodule->pModule);
	//ASSERT(pModHandle);

	//RETAILMSG(TONE_MOD, ( "InsertHandleList: < %s!\r\n", pmodule->pModule->lpszApName) );			

	plist = (PHANDLE_LIST)KHeap_Alloc( sizeof( HANDLE_LIST ));	
	
	if(plist == NULL){			
		RETAILMSG(TONE_MOD, ( "InsertHandleList: alloc fail!\r\n") );			
		return ;
	}
	plist->pHandle = pModHandle;
	plist->pnext   = NULL;

	//Insert 在最后！
	ptemp2 = ptemp = pmodule->pHandlelist;
	//RETAILMSG(TONE_MOD, ( "InsertHandleList: pmodule->pHandlelist: %x!\r\n",pmodule->pHandlelist) );			

	if(pmodule->pHandlelist == NULL)
		pmodule->pHandlelist = plist;
	else{
		while(ptemp->pnext){
			ptemp2 = ptemp;
			ptemp = ptemp->pnext;
		}
		//if(ptemp2)
			ptemp2->pnext = plist;
		//else
		//	pmodule->pHandlelist = plist;
	}
	RETAILMSG(TONE_MOD, ( "InsertHandleList: >!\r\n") );			
	return;
}


//#define		FIND_ONLY			1
//#define		FIND_NON_XIP		2
//#define		FIND_NON_XIP_REF	3

PMODULE_HANDLE FindLoadedHandle(PMODULE_NODE pMod, UINT flag)
{
	PMODULE_HANDLE		ptemp;

	RETAILMSG(TONE_MOD, ( "FindLoadedHandle: <!\r\n") );			

/*	if(pMod->uPos != XIP_MODULE){
		RETAILMSG(TONE_MOD, ( "FindLoadedHandle: XIP_MODULE >!\r\n") );			
		return NULL;
	}*/

	ptemp = pMod->lpHandle;
	//RETAILMSG(1, ( "FindLoadedHandle: ptemp=%x.\r\n", ptemp ) );			
	while(ptemp){
		RETAILMSG(TONE_MOD, ( "FindLoadedHandle:pMod->uType=%d,ptemp->uMode=%d.\r\n", pMod->uType, ptemp->uMode ) );			
		if(pMod->uType == CREATE_LIBRARY)
			break;
		if((pMod->uType == CREATE_EXE)&&(ptemp->uMode == MODULE_EXEC))
			break;
		//add for fix bug...
		ptemp = ptemp->lpNext;
	}
	if(!ptemp){
		RETAILMSG(TONE_MOD, ( "FindLoadedHandle: >no handle!\r\n") );			
		return NULL;
	}
	//if(flag == 

	if(flag == FIND_ONLY){
		return ptemp;
	}
	if(pMod->uPos == XIP_MODULE){
		RETAILMSG(TONE_TEST, ( "XIP:%s\r\n", pMod->lpszApName) );			
		return NULL;
	}

	if(flag == FIND_NON_XIP_REF){
		//RETAILMSG(TONE_TEST, ( "FindLoadedHandle: find loaded %s!\r\n", pMod->lpszApName) );			
		ptemp->uRef++;
		RETAILMSG(TONE_TEST, ( "->%x:%s\r\n", ptemp->uRef, pMod->lpszApName) );			
	}	

	RETAILMSG(TONE_MOD, ( "FindLoadedHandle: >\r\n") );			
	return ptemp;
}

void ReleaseMapHandle(PMODULE_NODE	pNode)
{
	PMODULE_HANDLE	ptemp;

	RETAILMSG(TONE_MOD, ( "ReleaseMapHandle: <\r\n") );			

	ptemp = FindLoadedHandle(pNode, FIND_NON_XIP);

	if(ptemp){
		ptemp->uRef --;
		//RETAILMSG(TONE_TEST, ( "ReleaseMapHandle: release ref for %s\r\n", pNode->lpszApName) );			
		RETAILMSG(TONE_TEST, ( "<-%x:%s\r\n", ptemp->uRef,pNode->lpszApName) );			
	}

	RETAILMSG(TONE_MOD, ( "ReleaseMapHandle: >\r\n") );			
	return;	
}

void InsertClient(PMODULE_CLIENT pClient)
{
	//RETAILMSG(TONE_MOD|TONE_TEST, ( "InsertClient: <\r\n") );			

	KL_EnterCriticalSection( &csClientList );	

	pClient->pNext = lpClientList;
	lpClientList = pClient;

	KL_LeaveCriticalSection( &csClientList );	
	//RETAILMSG(TONE_MOD|TONE_TEST, ( "InsertClient: >\r\n") );			
}

BOOL RemoveClient(PMODULE_CLIENT pClient)
{
	PMODULE_CLIENT	ptemp;
	BOOL			bResult = FALSE;

	KL_EnterCriticalSection( &csClientList );	

	ASSERT(lpClientList);

	if(pClient == lpClientList){
		lpClientList = lpClientList->pNext;
		bResult = TRUE;
		goto removeclient_exit;
		//return TRUE;
	}

	ptemp = lpClientList;

	ASSERT(ptemp);

	while(ptemp->pNext){
		if(ptemp->pNext == pClient){
			ptemp->pNext = pClient->pNext;
			bResult = TRUE;
			goto removeclient_exit;
			//return TRUE;
		}
		ptemp = ptemp->pNext;
	}
	
removeclient_exit:
	KL_LeaveCriticalSection( &csClientList );	
	return bResult;		
}

UINT GetNewProcAddr(PMODULE_HANDLE pHandle, LPCSTR lpProcName)
{
		const ElfW(Sym)		*symtab;
		const char			*strtab;
//		const ElfW(Half)	*verstab;
//		Elf_Symndx			symidx;
		int					i = 0;
		const ElfW(Sym)		*sym;
		char				*name;
		PFUNCTION_SYM		pFun;
		UINT				len;

		
		RETAILMSG(TONE_EXEC, ("syn len: %x\r\n", sizeof(ElfW(Sym))));

//		symtab = (const void *) D_PTR (module, l_info[DT_SYMTAB]);
//		strtab = (const void *) D_PTR (module, l_info[DT_STRTAB]);
		symtab = (const void *) pHandle->info->symtab;
		strtab = (const void *) pHandle->info->strtab;

//		verstab = module->l_versyms;

		/* Search the appropriate hash bucket in this object's symbol table
		for a definition for the same symbol name.  */
		//RETAILMSG(TONE_EXEC, ("module->l_nchain: %x\r\n", module->l_nchain));
		RETAILMSG(TONE_EXEC, ("module->l_nchain: %x\r\n", pHandle->info->l_nchain));

		for(i = 0; i < pHandle->info->l_nchain; i++){

			sym = &symtab[i];

			//RETAILMSG(TONE_EXEC, ("GetProcAddr  111\r\n");

			if(sym == NULL)
				continue;

			 /* No value.  */
			if (sym->st_value == 0 )
				continue;
			
			//RETAILMSG(TONE_EXEC, ("GetProcAddr  222\r\n");

			/* Ignore all but STT_NOTYPE, STT_OBJECT and STT_FUNC entries
			since these are no code/data definitions.  */
			if ( ELF32_ST_TYPE(sym->st_info) != STT_FUNC)
				continue;
			
			/* Not the symbol we are looking for.  */
//			if (sym != ref && strcmp (strtab + sym->st_name, undef_name))
//				continue;
			
			//RETAILMSG(TONE_EXEC, ("GetProcAddr  333\r\n");

			/* There cannot be another entry for this symbol so stop here.  */
			switch (ELF32_ST_BIND(sym->st_info))
			{
//			case STB_WEAK:
			case STB_GLOBAL:
				{

				if(strcmp(lpProcName, (strtab + sym->st_name)) != 0)
					continue;

				pFun = (PFUNCTION_SYM)KHeap_Alloc(sizeof(FUNCTION_SYM));

				if(pFun == NULL){
						KL_DebugOutString("alloc fail!\n");
						return 0;
				}				

				len = strlen( (strtab + sym->st_name) ) + 1;
				pFun->lpFuncName = KHeap_Alloc( len * sizeof( TCHAR ) );
				
				if( pFun->lpFuncName == NULL ){
					KHeap_Free( pFun, sizeof( FUNCTION_SYM ) );
					//KL_LeaveCriticalSection( &csModuleList );	 
					RETAILMSG(TONE_MOD, ("GetProcAddr: >1\n"));
					return 0;
				}
				strcpy( pFun->lpFuncName, (strtab + sym->st_name) );	

				pFun->lpNext = pHandle->lpProc;
				pHandle->lpProc = pFun;

				/* Global definition.  Just what we need.  */
				//strcpy(pFun->lpszFunName, (strtab + sym->st_name));
				//name = strtab + sym->st_name;
			
				RETAILMSG(TONE_EXEC, ("funname: %s\r\n", pFun->lpFuncName));

				//pFun->lpProc = __dllookup_value((const char*)pFun->lpszFunName, module);
				pFun->lpProc = sym->st_value + pHandle->info->addr;

				}
//				result->s = sym;
//				result->m = module;
//				return 1;
//			default:
				/* Local symbols are ignored.  */
//				break;
			}
		}//while (1);

		RETAILMSG(TONE_EXEC, ("GetProcAddr: >ok\r\n"));

		return pFun->lpProc;		
}






