#include <stdio.h>
#include "structure.h"

halt()
{
	int i, j;

	for (i = 0; i < USERNUM; i++)
	{
		if (user[i].u_uid != 0)
		{
			for (j = 0; j < NOFILE; j++)
			{
				if (user[i].u_ofile[j] != SYSOPENFILE + 1)
				{
					close(user[i].u_ofile[j]);
					user[i].u_ofile[j] = SYSOPENFILE + 1;
				}
			}
		}
	}
	fseek(fd, BLOCKSIZ, SEEK_SET);
	fwrite(&filsys, 1, sizeof(struct filsys), fd);
	fclose(fd);
	printf("\nGOOD bye. See You Next Time. Please turn off the switch.\n");
	exit(0);
}
