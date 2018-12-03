void pip(char** args)
{
	int i,pipno=0;
	for(i=0;args[i]!=NULL;i++)
		if(strcmp(args[i],"|")==0)
			pipno++;
	int LEN=i;
	char* actcmd[256];
	int pip_pos[256][2],j=0,cpos=0,jlen;
	bool runlp=true;
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
				runlp=false;
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
					fprintf(stderr,"will write\n");
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