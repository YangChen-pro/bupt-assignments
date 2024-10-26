#include <string.h>
#include <stdio.h>
#include "structure.h"

int namei(char *name)
{
	int i, notfound = 1;

	for (i = 0; i < dir.size; i++)
	{
		if (((strcmp(dir.direct[i].d_name, name) == 0)) && (dir.direct[i].d_ino != 0))
		{
			return i;
		}
	}
	return -1;
}

short iname(char *name)
{
	int i, notfound = 1;
	for (i = 0; ((i < DIRNUM) && (notfound)); i++)
		if (dir.direct[i].d_ino == 0 || dir.direct[i].d_ino > MAX)
		{
			notfound = 0;
			break;
		}
	if (notfound)
	{
		printf("\nThe current directory is full!!!\n");
		return -1;
	}
	else
	{
		strcpy(dir.direct[i].d_name, name);
		dir.direct[i].d_ino = 1;
		return i;
	}
}
