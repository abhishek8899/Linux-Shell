#include "headers.h"

void prmpt(int lewn,char *myh)
{
	char *usr,*path,hostbuffer[256],cwd[256],*base,*loc;
	int i,n;
	gethostname(hostbuffer,sizeof(hostbuffer));
	usr=getenv("USER");
	getcwd(cwd,sizeof(cwd));
	// printf("%lu\n",sizeof(cwd));
	for(i=0;i<lewn;i++)
		if(myh[i]!=cwd[i])
			break;
	printf("\x1b[1;31m" "%s@%s:",usr,hostbuffer);
	if(i==lewn)
	{
		printf("\x1b[1;37m" "~");
		for(;cwd[i]!='\0';i++)
			printf("\x1b[1;37m" "%c",cwd[i]);
		printf("\x1b[1;37m" "> " "\x1b[1;34m");
	}
	else
		printf("\x1b[1;31m" "%s> " "\x1b[1;34m",cwd);
}