#include "headers.h"

void ls(char *path, char *flag)
{
	struct dirent *ent;
	DIR *dir = opendir(path);
	if(dir==NULL)
	{
		printf("Directory does not exist\n");
		return;
	}


	if(strcmp(flag,"-l")==0 || strcmp(flag,"-la")==0 || strcmp(flag,"-al")==0)
	{
		while((ent=readdir(dir))!=NULL)
		{
			if((strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) && strcmp(flag,"-l")==0)
				continue;
			struct stat st;
			struct tm *starttime[2];
			unsigned char mod[13];
			time_t now;
			int year;
			stat(ent->d_name,&st);
			printf( (S_ISDIR(st.st_mode)) ? "d" : "-");
			printf( (st.st_mode & S_IRUSR) ? "r" : "-");
			printf( (st.st_mode & S_IWUSR) ? "w" : "-");
			printf( (st.st_mode & S_IXUSR) ? "x" : "-");
			printf( (st.st_mode & S_IRGRP) ? "r" : "-");
			printf( (st.st_mode & S_IWGRP) ? "w" : "-");
			printf( (st.st_mode & S_IXGRP) ? "x" : "-");
			printf( (st.st_mode & S_IROTH) ? "r" : "-");
			printf( (st.st_mode & S_IWOTH) ? "w" : "-");
			printf( (st.st_mode & S_IXOTH) ? "x" : "-");
			printf("  %s",getpwuid(st.st_uid)->pw_name); 
			printf("  %s",getgrgid(st.st_gid)->gr_name);
			printf(" \t%lld",(long long)st.st_size);
			time(&now);
			year = localtime(&now)->tm_year;
			starttime[1] = localtime(&st.st_ctime);
			if(starttime[1]->tm_year == year)
				strftime(mod,13,"%b %e %R",starttime[1]);
			else
				strftime(mod,13,"%b %e %Y",starttime[1]);
			printf(" \t%s",mod );	 	
			printf(" \t%s\n",ent->d_name);
		}
		return;
	}



	while((ent=readdir(dir))!=NULL)
	{
		if((strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0) && strcmp(flag,"-a")!=0)
			continue;
		printf("%s ",ent->d_name);
	}
	printf("\n");
	closedir(dir);
}