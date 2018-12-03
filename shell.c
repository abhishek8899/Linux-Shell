#include "headers.h"

char* root,myhome[256];
char pnm[256][20];
int ln,ind,chd[256],rsrv[256],gpid;
volatile sig_atomic_t signalis=0,cint=0,zint=0;

void signalHandler(){
	cint=1;
	return;
}

void signZ(){
	zint=1;
	kill(gpid,SIGTSTP);
	return;
}


void sigintHandler() {
	signalis=1;
}


void pip(char** args)
{
	int i,pipno=0;
	for(i=0;args[i]!=NULL;i++)
		if(strcmp(args[i],"|")==0)
			pipno++;
	int LEN=i;
	char* actcmd[256];
	int pip_pos[256][2],j=0,cpos=0,jlen,runlp=1;
	while(runlp)
	{
		jlen=0;
		while(strcmp(args[j],"|")!=0)
		{
			actcmd[jlen]=args[j];
			j++;
			jlen++;
			if(args[j]==NULL)
			{
				runlp=0;
				break;
			}			
		}
		j++;
		actcmd[jlen]=NULL;
		pipe(pip_pos[cpos]);
		if(cpos!=0 && cpos!=pipno)
		{
			pid_t pid=fork();
			
			if(pid==0)
			{
				dup2(pip_pos[cpos][1],1);
				dup2(pip_pos[cpos-1][0],0);
				close(pip_pos[cpos][0]);
				close(pip_pos[cpos-1][1]);
				execvp(actcmd[0],actcmd);
			}
			else
			{
				wait(NULL);
				close(pip_pos[cpos][1]);
				close(pip_pos[cpos-1][0]);
			}
		}
		else if(cpos==0)
		{
			pid_t pid=fork();
			// fprintf(stderr,"eer .%s.     .%s.\n",actcmd[0],actcmd[1]);
			if(pid==0)
			{
				char *infil = actcmd[jlen-1];
				char* chkk = actcmd[jlen-2];
				dup2(pip_pos[cpos][1],1);
				close(pip_pos[cpos][0]);
				if(strcmp(chkk,"<")==0)
				{
					int fd=open(infil,O_RDONLY,0);
					if(fd<0)
					{
						perror("Error opening input file");
					}
					if(dup2(fd,0)<0)
					{
						perror("Error input duplication");
					}
					close(fd);
					actcmd[jlen-2]=NULL;
				}
				execvp(actcmd[0],actcmd);
			}
			else
			{
				wait(NULL);
				close(pip_pos[cpos][1]);
			}
		}
		else if(cpos==pipno)
		{
			pid_t pid=fork();
			if(pid==0)
			{
				char* outfil = actcmd[jlen-1];
				char* chkk = actcmd[jlen-2];
				dup2(pip_pos[cpos-1][0],0);
				close(pip_pos[cpos-1][1]);
				int isout=0,isapnd=0;
				if(strcmp(chkk,">")==0)
				{
					isout=1;
					// fprintf(stderr,"will write\n");
					struct stat st;
					if(stat(outfil,&st)!=0)
					{
						int fd=open(outfil,O_CREAT | O_WRONLY,0644);
						if(fd<0)
						{
							perror("Error opening output file");
						}
						if(dup2(fd,1)<0)
						{
							perror("Error output duplication");
						}
						close(fd);
					}
					else
					{
						int fd=open(outfil,O_WRONLY | O_TRUNC,0644);
						if(fd<0)
						{
							perror("Error opening output append file");
						}
						if(dup2(fd,1)<0)
						{
							perror("Error output append duplication");
						}
						close(fd);
					}
				}
				if(strcmp(chkk,">>")==0)
				{
					isapnd=1;
					struct stat st;
					if(stat(outfil,&st)!=0)
					{
						int fd=open(outfil,O_CREAT | O_WRONLY,0644);
						if(fd<0)
						{
							perror("Error opening output append file");
						}
						if(dup2(fd,1)<0)
						{
							perror("Error output append duplication");
						}
						close(fd);
					}
					else
					{
						//printf("\n\nercj\n\n");
						int fd=open(outfil,O_WRONLY | O_APPEND,0644);
						if(fd<0)
						{
							perror("Error opening output append file");
						}
						if(dup2(fd,1)<0)
						{
							perror("Error output append duplication");
						}
						close(fd);
					}
				}
				if(isout==1 || isapnd==1)
					actcmd[jlen-2]=NULL;
				execvp(actcmd[0],actcmd);
			}
			else
			{
				wait(NULL);
				close(pip_pos[cpos-1][0]);
			}
		}
		cpos++;
	}
	return;
}

void f_pipes(char *cmd)
{
	char *arg[256];
	char *ar;
	ar=strtok(cmd," \t\n");
	int i=0;
	while(ar!=NULL)
	{
		arg[i++]=ar;
		ar=strtok(NULL," \t\n");
	}
	ar=NULL;
	arg[i]=NULL;
	pip(arg);
}

void jobs()
{
	for(int i=0,nid=0;i<ind;i++)
	{
		if(chd[i]==-1)
			continue;
		nid++;
		if(chd[i]==-2)
			printf("[%d]- Stopped 		%s %d\n",nid,pnm[i],rsrv[i]);
		else
			printf("[%d]+ Running 		%s %d\n",nid,pnm[i],rsrv[i]);
	}
}

void fg(char ** arg)
{
	if(arg[1]==NULL)
	{
		printf("Improper number of arguments\n");
		return;
	}
	int dpe=atoi(arg[1]),nid=0,i,status,jb=0;
	for(i=0;i<ind;i++)
		if(chd[i]!=-1)
		{
			nid++;
			if(nid==dpe)
				break;
		}
	gpid=rsrv[i];
	if(i==ind)
		printf("No process at given refer id\n");
	else
	{
		signal(SIGINT,signalHandler);
		signal(SIGTSTP,signZ);
		kill(gpid,SIGCONT);
		do
		{
			if(cint==1)
			{
				kill(gpid,SIGINT);
				cint=0;
				break;
			}
			if(zint==1)
			{
				kill(gpid,SIGSTOP);
				zint=0;
				chd[i]=-2;
				printf("\n[%d] %s 		Stopped\n",nid,pnm[i]);
				break;
			}
			waitpid(rsrv[i], &status, WUNTRACED);
		} 
		while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
}

void overkill(char ** arg)
{
	if(arg[1]!=NULL)
	{
		printf("Improper number of arguments\n");
		return;
	}
	else
	{
		for(int i=0;i<ind;i++)
			if(chd[i]!=-1)
			{
				kill(chd[i],9);
				chd[i]=-1;
			}
	}
}

void kjob(char ** arg)
{
	int rpid=atoi(arg[1]);
	int sg=atoi(arg[2]),nid=0;
	if(arg[2]==NULL || arg[3]!=NULL)
	{
		printf("Improper number of arguments\n");
		return;
	}
	else
	{
		for(int i=0;i<ind;i++)
			if(chd[i]!=-1)
			{
				nid++;
				if(nid==rpid)
				{
					kill(rsrv[i],9);
					printf("%d exited\n",rsrv[i]);
					chd[i]=-1;
					break;
				}
			}
	}
}

void bg(char ** arg)
{
	if(arg[1]==NULL)
	{
		printf("Improper number of arguments\n");
		return;
	}
	int dpe=atoi(arg[1]),nid=0,i,status;
	for(i=0;i<ind;i++)
		if(chd[i]!=-1)
		{
			nid++;
			if(nid==dpe)
				break;
		}
	if(i==ind)
		printf("No process at given refer id\n");
	else
	{
		if(chd[i]==-2)
		{
			kill(rsrv[i],SIGCONT);
			chd[i]=rsrv[i];
		}
		else
		{
			printf("Process is already running\n");
		}
	}
}

void clockti(char ** arg)
{
	char tm[256],dt[256];
	int dew;
	if(arg[1]==NULL)
		sscanf("1","%d",&dew);
	else if(arg[2]==NULL)
		sscanf(arg[1],"%d",&dew);	
	else
		sscanf(arg[2],"%d",&dew);	
	signal(SIGINT,sigintHandler);
	while(signalis==0)
	{
		FILE *kj=fopen("/proc/driver/rtc","r");
		if(kj==NULL)
		{
			perror("Error: ");
			exit(-1);
		}
		for(int g=0;g<3;g++)
			fscanf(kj,"%s",tm);
		for(int g=0;g<3;g++)
			fscanf(kj,"%s",dt);
		printf("%s %s\n",dt,tm);
		sleep(dew);
	}
	signalis=0;
}

void cmd()
{
	signal(SIGINT,signalHandler);
	signal(SIGTSTP,signZ);
    char *cmds,*ar,*dir,*smsc,*tempstr;
	while(1)
	{
		char* com[256];
		int i=0,pipno=1,pipind[256];
		for(i=0;i<256;i++)
			com[i]=NULL;
		for(i=0;i<ind;i++)
		{
			int status;
			if(chd[i]==-1)
					continue;
			if (waitpid(rsrv[i],&status,WNOHANG)!=0)
			{
				printf("%d exited successfully\n",rsrv[i]);
				chd[i]=-1;
			}
		}
		prmpt(ln,myhome);
		cmds=takeinput();
		pipind[0]=-1;
		while(cmds[i]!='\0')
			{
				if(cmds[i]=='|')
					pipind[pipno++]=i;
				i++;
			}
		if(pipno>1)
		{
			f_pipes(cmds);
			continue;
		}
		for(int pipiter=1;pipiter<=pipno;pipiter++)
		{
			char* pipcom=malloc(sizeof(char)*256);
			char* tempocm;
			i=pipind[pipiter-1]+1;
			int comfil=0;
			// printf("cmd %d  %c\n",comfil,cmds[i]);
			while(cmds[i]!='|' && cmds[i]!='\0' && cmds[i]!='\n')
			{
				pipcom[comfil]=cmds[i];
				comfil++,i++;
			}
			// printf("\nrejjv\n");
			pipcom[comfil]='\0';
			tempocm=pipcom;
		// printf("\n\n\n\ncer\n\n\n\n");
			int and=0,redir=0;
			ar=strtok(tempocm," \t\n|");
			i=0;


			while(ar!=NULL)
			{
				com[i++]=ar;
				if(strcmp(ar,"&")==0)
				{
					and=1;
					com[--i]=NULL;
					break;
				}
				if(strcmp(ar,"<")==0 || strcmp(ar,">")==0 || strcmp(ar,">>")==0)
					redir=1;
				ar=strtok(NULL," \t\n");
			}
			ar=NULL;
			char *arg[i+1];
			int j;
			for(j=0;j<i;j++)
				arg[j]=com[j];
			arg[j]=NULL;
			

			//printf("no of com   %d\n",i);


			if(i==0)
				continue;

			if(strcmp(com[0],"quit")==0)
				return;

			pid_t pid,wpid;
			int state;
			

			if(and==0 && strcmp(com[0],"remindme")!=0)
			{

				if(strcmp(com[0],"pwd")==0 && redir==0)
				{
					pwdi();
					continue;
				}

				if(strcmp(com[0],"pinfo")==0 && redir==0)
				{
					p_inf(arg,ln,myhome);
					continue;
				}

				if(strcmp(com[0],"cd")==0 && redir==0)
				{
					if(i==1)
					{
						chdir(myhome);
						continue;
					}
					cdd(arg,myhome);
					continue;
				}

				if(strcmp(com[0],"echo")==0 && redir==0)
				{
					echo(arg,i);
					continue;
				}

				if(strcmp(com[0],"setenv")==0 && redir==0)
				{
					stenv(arg,i);
					continue;
				}

				if(strcmp(com[0],"unsetenv")==0 && redir==0)
				{
					unstenv(arg,i);
					continue;
				}

				if(strcmp(com[0],"clock")==0 && redir==0)
				{
					clockti(arg);
					continue;
				}

				if(strcmp(com[0],"fg")==0 && redir==0)
				{
					fg(arg);
					continue;
				}

				if(strcmp(com[0],"bg")==0 && redir==0)
				{
					bg(arg);
					continue;
				}

				if(strcmp(com[0],"ls")==0 && redir==0)
				{
					if(i==1)
					{
						ls(".","n");
						continue;
					}
					if(i==2)
					{
						if(strcmp(com[1],"-a")==0 || strcmp(com[1],"-l")==0 || strcmp(com[1],"-la")==0 || strcmp(com[1],"-al")==0)
							ls(".",com[1]);
						else
							ls(com[1],"n");
						continue;
					}
					if(i==3 && ((strcmp(com[1],"-a")==0 && strcmp(com[2],"-l")==0) || (strcmp(com[2],"-a")==0 && strcmp(com[1],"-l")==0)))
					{
						ls(".","-la");
						continue;
					}
					if(i==4 && ((strcmp(com[1],"-a")==0 && strcmp(com[2],"-l")==0) || (strcmp(com[2],"-a")==0 && strcmp(com[1],"-l")==0)))
					{
						ls(com[3],"-la");
						continue;
					}
					ls(com[2],com[1]);
					continue;
				}
				
				pid_t pid, wpid;
	  			int status;
	  			// printf("redir %d\n",redir);
				pid = fork();
				if (pid == 0)
				{
					int isinp=0,isout=0,isapnd=0,fd;
					char infil[100],outfil[100];
					for(int j=0;j<i;j++)
					{
						if(strcmp(arg[j],"<")==0)
						{
							isinp=1;
							arg[j]=NULL;
							strcpy(infil,arg[j+1]);
						}
						else if(strcmp(arg[j],">")==0)
						{
							arg[j]=NULL;
							strcpy(outfil,arg[j+1]);
							//arg[j+1]=NULL;
							isout=1;
						}
						else if(strcmp(arg[j],">>")==0)
						{
							arg[j]=NULL;
							strcpy(outfil,arg[j+1]);
							isapnd=1;
						}
						//fprintf(stderr,"isout %d\n",isout);
					}
					//printf("j    %d\n",j);
					int stdouttt=dup(1),stdinnn=dup(0);
					if(isinp==1)
					{
						fd=open(infil,O_RDONLY,0);
						if(fd<0)
						{
							perror("Error opening input file");
						}
						if(dup2(fd,0)<0)
						{
							perror("Error input duplication");
						}
						close(fd);
					}
					if(isout==1)
					{
						//fprintf(stderr,"will write\n");
						struct stat st;
						if(stat(outfil,&st)!=0)
						{
							int fd=open(outfil,O_CREAT | O_WRONLY,0644);
							if(fd<0)
							{
								perror("Error opening output append file");
							}
							if(dup2(fd,1)<0)
							{
								perror("Error output append duplication");
							}
							close(fd);
						}
						else
						{
							int fd=open(outfil,O_WRONLY | O_TRUNC,0644);
							if(fd<0)
							{
								perror("Error opening output append file");
							}
							if(dup2(fd,1)<0)
							{
								perror("Error output append duplication");
							}
							close(fd);
						}
					}
					if(isapnd==1)
					{
						struct stat st;
						if(stat(outfil,&st)!=0)
						{
							int fd=open(outfil,O_CREAT | O_WRONLY,0644);
							if(fd<0)
							{
								perror("Error opening output append file");
							}
							if(dup2(fd,1)<0)
							{
								perror("Error output append duplication");
							}
							close(fd);
						}
						else
						{
							//printf("\n\nercj\n\n");
							int fd=open(outfil,O_WRONLY | O_APPEND,0644);
							if(fd<0)
							{
								perror("Error opening output append file");
							}
							if(dup2(fd,1)<0)
							{
								perror("Error output append duplication");
							}
							close(fd);
						}
					}
					if(strcmp(arg[0],"jobs")==0)
					{
						jobs();
					}
					else if(strcmp(arg[0],"overkill")==0)
					{
						overkill(arg);
					}
					else if(strcmp(arg[0],"kjob")==0)
					{
						kjob(arg);
					}
					else if (execvp(arg[0],arg) == -1)
					{
				    	perror("Err");
				    }
					dup2(stdouttt,1);
					dup2(stdinnn,0);
				    exit(EXIT_FAILURE);
				}
				else if (pid < 0)
				{
				  	// Error forking
				    perror("lsh");
				}
				else
				{
					int lkmc=pid;
					gpid=pid;
				    do
				    {
				      if(cint==1)
				      	break;
				      if(zint==1)
				      {
				      	chd[ind]=-2;
				      	rsrv[ind]=gpid;
				      	strcpy(pnm[ind],com[0]);
				      	ind++;
				      	printf("\n[%d] %s 		Stopped\n",ind-1,pnm[ind-1]);
				      	break;
				      }
				      wpid = waitpid(pid, &status, WUNTRACED);
				    } 
				    while (!WIFEXITED(status) && !WIFSIGNALED(status));
					cint=zint=0;
				}
			}
			else
			{
				pid_t pid, wpid;
	  			int status;
	  			//printf("ewflwepprgfunrougoungeuoge\n");
				pid = fork();
				setpgid(0,0);
				if (pid == 0)
				{
					// Child prcs
					if(strcmp(com[0],"remindme")==0)
					{
						int ed;
						sscanf(com[1],"%d",&ed);
						sleep(ed);
						printf("\n\nReminder: ");
						int lkre=2;
						while(arg[lkre]!=NULL)
							printf("%s ",arg[lkre++]);
						printf("\n");
						_exit(0);
					}
					if (execvp(arg[0],arg) == -1)
					{
				    	perror("lsh");
				    }
				    exit(EXIT_FAILURE);
				}
				else if (pid < 0)
				{
				  	// Error forking
				    perror("lsh");
				}
				else
				{
					strcpy(pnm[ind],com[0]);
					rsrv[ind]=pid;
					chd[ind++]=pid;
					printf("Started process %d successfully\n",chd[ind-1]);
				}
			}
		}
		cmds=NULL;
	}
}

int main()
{
	root=getenv("PWD");
	char* dir=malloc(sizeof(char)*256);;
	getcwd(dir,256);
	ln=strlen(dir);
	for(int i=0;i<ln;i++)
		myhome[i]=dir[i];
	free(dir);
	cmd();
	return 0;
}
