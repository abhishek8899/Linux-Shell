#include "headers.h"

void cdd(char ** arg,char * myh)
{
	if(arg[1][0]=='~')
	{
		char templ[256]="";
		strcat(templ,myh);
		int krltv=strlen(templ);
		strcat(templ,arg[1]);
		arg[1]=templ;
		int jlevn=strlen(arg[1]);
		for(int ler=krltv;ler<=jlevn;ler++)
			arg[1][ler]=arg[1][ler+1];
		printf("s %s\n",arg[1]);
	}
	if(chdir(arg[1])==-1)
		printf("Directory does not exists\n");
}