/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����EXE�������������
�汾�ţ�1.0.0
����ʱ�ڣ�2003-04-40
���ߣ�ZB
�޸ļ�¼��
    LN: 2003-09-02 _start  ����GetCommandLine�Ĵ���

******************************************************/

/*
 * startup function for every executable program
 */
#include <ewindows.h>

typedef void (*func_ptr) (void);
extern func_ptr __CTOR_LIST__[];
extern func_ptr __DTOR_LIST__[];

static void __do_global_dtors (void)
{
	static func_ptr *p = __DTOR_LIST__ + 1;
	
	/*
	* Call each destructor in the destructor list until a null pointer
	* is encountered.
	*/
	while (*p)
    {
		(*(p)) ();
		p++;
    }
}

static void __do_global_ctors (void)
{
	unsigned long nptrs = (unsigned long) __CTOR_LIST__[0];
	unsigned i;
	
	/*
	* If the first entry in the constructor list is -1 then the list
	* is terminated with a null entry. Otherwise the first entry was
	* the number of pointers in the list.
	*/
	if (nptrs == -1)
    {
		for (nptrs = 0; __CTOR_LIST__[nptrs + 1] != 0; nptrs++)
			;
    }
	
	/* 
	* Go through the list backwards calling constructors.
	*/
	for (i = nptrs; i >= 1; i--)
    {
		__CTOR_LIST__[i] ();
    }
	
	/*
	* Register the destructors for processing on exit.
	*/
//	atexit (__do_global_dtors);
}

static int initialized = 0;

//static LPSTR GetCommandLine ()
//{
//	return NULL;
//}

extern int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow);

int _start (void)
{
	int		nRet = -1;

	//EdbgOutputDebugString ("*************now in _start*************\n");

	if (!initialized)
	{
		HINSTANCE hInst;
		LPSTR	lpCmd = (LPSTR)GetCommandLine ();
		int		nShow;
/*		STARTUPINFO	si;

		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = 0; //STARTF_USESHOWWINDOW;
		GetStartupInfo (&si);si.wShowWindow;
*/		nShow = SW_SHOW;
		
		hInst = (HINSTANCE)GetModuleHandle (NULL);
		//RETAILMSG( 1, ( "******************GetModuleHandle=0x%x.\r\n", hInst ) );

		initialized = 1;
		__do_global_ctors ();
		// LN 2003-09-02, ����GetCommandLine�Ĵ���-begin
		while( *lpCmd && *lpCmd != ' ' )
			lpCmd++;
		if( *lpCmd == ' ' )
			lpCmd++;
		// LN 2003-09-02, ����GetCommandLine�Ĵ���-end


		nRet = WinMain (hInst, 0, lpCmd, nShow);

		__do_global_dtors ();
	}
	ExitProcess( nRet );
	return nRet;
}

