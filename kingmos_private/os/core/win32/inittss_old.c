#include <eframe.h>
//#include <ethread.h> 
#include <equeue.h>

#include <epcore.h>


// put it to arch
void InitThreadTSS(
     LPTHREAD lpThread,
     LPTHREAD_START_ROUTINE lpStartAdr,
     LPBYTE lpStack,
     LPVOID lpParameter )
{

#ifdef EML_DOS
    WORD * p;
     p = (WORD*)(lpStack + dwSize);
     *((DWORD*)(p-4)) = (DWORD)ExitThread;
     *(p-1) = 0;
     lpThread->tss.ip = (WORD)lpStartAdr;
     lpThread->tss.cs = (WORD)((DWORD)lpStartAdr>>16);
     lpThread->tss.sp = (WORD)((DWORD)(p-4));
     lpThread->tss.ss = (WORD)(((DWORD)(p-4))>>16);
#endif

#ifdef EML_WIN32
     int * p;
     p = lpStack;//(int*)(lpStack + dwSize);
     *(p-1) = 0;// exit code for ExitThread
     *((DWORD*)(p-2)) = (DWORD)lpParameter;
     *((DWORD*)(p-3)) = (DWORD)ExitThread;
     lpThread->tss.eip = (DWORD)lpStartAdr;
     lpThread->tss.esp = (DWORD)(p-3);
     {
     
         TSS * p = &lpThread->tss;
#ifdef __GNUC__
		 asm ("
             mov p, %eax
             mov %ss, WORD PTR [eax+REG_SS]
             mov %cs, WORD PTR [eax+REG_CS]
			 ")
#else
         _asm
         {
             mov eax, p
             mov WORD PTR [eax+REG_SS], ss
             mov WORD PTR [eax+REG_CS], cs
         }
#endif //__GNUC__
     }
#endif

}



