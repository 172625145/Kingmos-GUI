#include <eframe.h>
#include <epcore.h>
void GetMMUContext( LPTHREAD lpThread, UINT flag, LPPROCESS lpNewProcess )
{
	LPPROCESS lpProcess;

	lpProcess = lpThread->lpCurProcess = lpNewProcess;

	//if( lpProcess != &InitKernelProcess )  // LN, 2003-06-04, ADD
	  //  dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
	//else              // LN, 2003-06-04, ADD
	//	dwProcessSlots[0] = 0;         // LN, 2003-06-04, ADD

	//SetCPUId( lpProcess->dwVirtualAddressBase );
}
