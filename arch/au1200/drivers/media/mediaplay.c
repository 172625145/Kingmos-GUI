#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
//#define PLAYDIR "/AMDBeta2"
#define PLAYDIR "/AMDMaiplayer"

typedef struct _MEDIA{
	pid_t pid;
}MEDIA, *PMEDIA;

static MEDIA g_stmedia;

int _au1200mediaplay(
 const char *pszpfname,
 int imors
)
{
//	char szcommand[100];
	if(pszpfname)
	{
		pid_t pid = fork();
		if(pid == -1)
		{
			printf("fork error");
			return -1;
		}
		if(pid == 0)
		{
			static int iFirst = 1;
			const char *argv[3];
			chdir(PLAYDIR);
			
			if(iFirst)
			{
				system("insmod mae-driver.ko mae_phys_address=0x6000000");
//				system("insmod mae-driver.ko");
				iFirst = 0;
			}
			
			if(execl("./maiplayer", "./maiplayer", "-k", "-a", pszpfname, NULL) == -1)
			{
				printf("execv error \r\n");
			}
		}
		else
		{
			int status;
			printf("pid:%d \r\n", pid);
			g_stmedia.pid = pid;
			waitpid(pid, &status, 0);
			printf("media out 0x%x \r\n", status);
		}
	}
	else
	{
		printf("kill (in)pid:%d \r\n", g_stmedia.pid);
		kill(g_stmedia.pid, SIGTERM);
		printf("kill (out)pid:%d \r\n", g_stmedia.pid);
		if(imors)
		{
			system("./fbio.exe mixer 1 0 enable 1");
			system("./fbio.exe /dev/fb1 enable 0");
		}
	}
	return 0;	
}
