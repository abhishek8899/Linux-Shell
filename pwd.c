#include "headers.h"

void pwdi()
{
	char* dir=malloc(sizeof(char)*256);;
	getcwd(dir,256);
	printf("%s\n",dir);
	free(dir);
}