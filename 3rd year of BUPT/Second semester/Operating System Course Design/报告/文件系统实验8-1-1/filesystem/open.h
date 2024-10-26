#include <stdio.h>
#include "structure.h"

unsigned short open(user_id, filename, openmode)
int user_id;
char *filename;
unsigned short openmode;
{
	unsigned int dinodeid;
	struct inode *inode;
	int i, j;

	dinodeid = namei(filename);
	if (dinodeid != (int)NULL)
	{
		printf("\nThe file does not existed!!!\n");
		return (int)NULL;
	}
	inode = iget(dinodeid);
	if (!access(user_id, inode, openmode))
	{
		printf("\nThe file was not allow to access!!!\n");
		iput(inode);
		return (int)NULL;
	}

	for (i = 1; i < SYSOPENFILE; i++)
		if (sys_ofile[i].f_count == 0)
			break;
	if (i == SYSOPENFILE)
	{
		printf("\nsystem open file too much\n");
		iput(inode);
		return (int)NULL;
	}
	sys_ofile[i].f_inode = inode;
	sys_ofile[i].f_flag = (char)openmode;
	sys_ofile[i].f_count = 1;
	if (openmode & FAPPEND)
		sys_ofile[i].f_off = inode->di_size;
	else
		sys_ofile[i].f_off = 0;

	for (j = 0; j < NOFILE; j++)
		if (user[user_id].u_ofile[j] == 0)
			break;
	if (j == NOFILE)
	{
		printf("\n There are too many files to be opend!!!\n");
		sys_ofile[i].f_count = 0;
		iput(inode);
		return (int)NULL;
	}
	user[user_id].u_ofile[j] = 1;

	if (openmode & FAPPEND)
	{
		for (i = 0; j < inode->di_size / BLOCKSIZ + 1; i++)
			bfree(inode->di_addr[i]);
		inode->di_size = 0;
	}
	return j;
}
