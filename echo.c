#include "headers.h"

void echo(char **arg, int leng)
{
	for(int u=1;u<leng;u++)
		printf("%s ",arg[u]);
	printf("\n");
}