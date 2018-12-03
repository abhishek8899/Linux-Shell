#include "headers.h"

void stenv(char** arg, int noa)
{
	if(noa==2)
		setenv(arg[1]," ",2);
	else if(noa==3)
		setenv(arg[1],arg[2],2);
	else
		printf("Number of arguments must be 2 or 3\n");
}

void unstenv(char** arg, int noa)
{
	if(noa==1)
		printf("No arguments given");
	else
		for(int i=0;i<noa-1;i++)
			unsetenv(arg[i+1]);
}