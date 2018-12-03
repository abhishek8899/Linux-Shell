#include "headers.h"

void jobs(int* chd,int ind)
{
	int i;
	for(i=0;i<ind;i++)
	{
		if(chd[i]==-1)
			continue;
		int pid,memSize,j,num;
		char *name,*prcs,*stats,str[5];
		// fprintf(chd[i],str,10);
		strcpy(prcs, "/proc/");
	    strcat(prcs,str);
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
		fclose(stat);
		printf("[%d] %s %d\n",i+1,name,chd[i]);
	}
}