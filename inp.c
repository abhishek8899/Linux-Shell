#include "headers.h"

char *takeinput()
{
	char *str=malloc(sizeof(char)*256);
	fgets(str,100,stdin);
	return str;
}