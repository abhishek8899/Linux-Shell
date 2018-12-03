#include "headers.h"

void p_inf(char ** arg, int lewn, char* myh)
{
	char prcs[256],status,name[40],xpath[1000],stats[256];
	for(int ef=0;ef<1000;ef++)
		xpath[ef]='{';
	int pid,memSize,j;
	strcpy(prcs, "/proc/");
	
	if(arg[1]) 
	    strcat(prcs, arg[1]);
	else 
	{ 
		// printf("ercre\n");
		strcat(prcs, "self");
	}
	strcpy(stats,prcs);
	strcat(stats,"/stat");
	
	FILE * stat = fopen(stats, "r");
	if(stat==NULL)
	{
		printf("Error:PID does not exists\n");
		return;
	}
	fscanf(stat, "%d", &pid);
	fscanf(stat, "%s", name);
	fscanf(stat, " %c", &status);
	printf( "pid: %d\n", pid);
	printf( "prcs Status: %c\n", status);
	fclose(stat);
			    
	strcpy(stats, prcs);
	strcat(stats, "/statm");
	FILE * mem = fopen(stats, "r");
	fscanf(mem, "%d", &memSize);
	fprintf(stdout, "Memory: %d\n", memSize);
	fclose(mem);
	
	strcpy(stats, prcs);
	strcat(stats, "/exe");
	readlink(stats, xpath, sizeof(xpath));


	fprintf(stdout, "Executable Path: ");
	for(j=0;j<lewn;j++)
		if(myh[j]!=xpath[j] || xpath[j]=='{')
			break;
	if(j==lewn)
	{
		printf("~");
		for(;xpath[j]!='\0' && xpath[j]!='{';j++)
			printf("%c",xpath[j]);
	}		
	else
		for(j=0;xpath[j]!='\0' && xpath[j]!='{';j++)
			printf("%c",xpath[j]);
	printf("\n");
}