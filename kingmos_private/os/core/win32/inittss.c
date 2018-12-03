#include <eframe.h>
//#include <ethread.h> 
//#include <equeue.h>

#include <epcore.h>
extern int  __ebp;  
// put it to arch
void InitThreadTSS(
     LPTHREAD lpThread,
     LPTHREAD_START_ROUTINE lpStartAdr,
     LPBYTE lpStack,
     LPVOID lpParameter )
{

#ifdef EML_DOS
//    WORD * p;
  //   p = (WORD*)(lpStack);// + dwSize);
    // *((DWORD*)(p-4)) = (DWORD)ExitThread;
     //*(p-1) = 0;
     //lpThread->tss.ip = (WORD)lpStartAdr;
     //lpThread->tss.cs = (WORD)((DWORD)lpStartAdr>>16);
     //lpThread->tss.sp = (WORD)((DWORD)(p-4));
     //lpThread->tss.ss = (WORD)(((DWORD)(p-4))>>16);
#endif

#ifdef EML_WIN32
     int * p;
     p = (int*)(lpStack);// + dwSize);     
	 *(p) = (DWORD)lpParameter;
	 *(p-1) = *( (int*)__ebp+1 );  // return address
//	 *(p-2) = (DWORD)lpStartAdr;
//	 *(p-3) = *( (int*)__ebp );  // ebp
     lpThread->tss.eip = (DWORD)lpStartAdr;
     lpThread->tss.ebp = lpThread->tss.esp = (DWORD)(p-1);

     {
     
         TSS * p = &lpThread->tss;
         _asm
         {
             mov eax, p
             mov WORD PTR [eax+REG_SS], ss
             mov WORD PTR [eax+REG_CS], cs
         }
     }
#endif

}

DWORD SetThreadIP( LPTHREAD lpThread, DWORD dwIP )
{
    DWORD dwOld = lpThread->tss.eip;
	lpThread->tss.eip = dwIP;
	return dwOld;
}


#define DEBUG_CallUserStartupCode 1
int CallUserStartupCode( LPVOID lpIP, LPVOID uParam )
{
	return ((LPTHREAD_START_ROUTINE)lpIP)( uParam );
}

UINT GetCPUMode( DWORD dwMode )
{
	return 0;
}

