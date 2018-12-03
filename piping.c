#include "headers.h"

void pip(char** args)
{
	int i,Pipes=0;
	for(i=0;args[i]!=NULL;i++)
	{
		if(strcmp(args[i],"|")==0)
			Pipes++;
	}
	int LEN=i;
	char* commands[100];
	int pipes[50][2];	
	int j=0;
	int curr=0;
	int k,lst=0;
	while(lst==0)
	{
		k=0;
		while(strcmp(args[j],"|")!=0)
		{
			commands[k]=args[j];
			j++;
			k++;
			if(args[j]==NULL)
			{
				lst=1;
				break;
			}			
		}
		j++;
		commands[k]=NULL;
		pipe(pipes[curr]);
		if(curr==0)
		{
			pid_t pid=fork();
			if(pid==0)
			{
				dup2(pipes[curr][1],1);
				close(pipes[curr][0]);
				execvp(commands[0],commands);
			}
			else
			{
				wait(NULL);
				close(pipes[curr][1]);
			}
		}
		else if(curr==Pipes)
		{
			pid_t pid=fork();
			if(pid==0)
			{
				dup2(pipes[curr-1][0],0);
				close(pipes[curr-1][1]);
				execvp(commands[0],commands);
			}
			else
			{
				wait(NULL);
				close(pipes[curr-1][0]);
			}
		}
		else
		{
			pid_t pid=fork();
			if(pid==0)
			{
				dup2(pipes[curr][1],1);
				dup2(pipes[curr-1][0],0);
				close(pipes[curr][0]);
				close(pipes[curr-1][1]);
				execvp(commands[0],commands);
			}
			else
			{
				wait(NULL);
				close(pipes[curr][1]);
				close(pipes[curr-1][0]);
			}
		}
		curr++;
	}
	return;
}

void f_pipes(char *cmd)
{
	char *arg[256];
	char *ar;
	ar=strtok(cmd,"|");
	int i=0;
	while(ar!=NULL)
	{
		arg[i++]=ar;
		ar=strtok(NULL,"|");
	}
	ar=NULL;
	arg[i]=NULL;
	pip(arg);
}