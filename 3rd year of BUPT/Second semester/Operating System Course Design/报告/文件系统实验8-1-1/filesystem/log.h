#include <stdio.h>
#include "structure.h"

int login(uid, passwd)
unsigned short uid;
char *passwd;
{
	int i, j;
	char *a;
	fseek(fd, DATASTART + BLOCKSIZ * 2, SEEK_SET);
	fread(pwd, 1, sizeof(struct pwd) * PWDNUM, fd);

	i = pwd[2].p_uid;
	a = pwd[2].password;

	for (i = 0; i < PWDNUM; i++)
	{
		if ((uid == pwd[i].p_uid) && ((strcmp(passwd, pwd[i].password) == 0)))
		{
			j = 0;
			while (j < USERNUM)
			{
				if (user[j].u_uid == 0)
				{
					break;
				}
				else
				{
					j++;
				}
			}
			if (j == USERNUM)
			{
				printf("\nThere are too much users in the filesystem, please wait!!\n");
				return 0;
			}
			else
			{
				user[j].u_uid = uid;
				user[j].u_gid = pwd[i].p_gid;
				user[j].u_default_mode = DEFAULTMODE;
			}
			break;
		}
	}
	if (i == PWDNUM)
	{
		printf("\nincorrect password\n");
		return 0;
	}
	else
	{
		return 1;
	}
}

int logout(uid)
unsigned short uid;
{
	int i, j, sys_no;
	struct inode *inode;

	for (i = 0; i < USERNUM; i++)
		if (uid == user[i].u_uid)
			break;
	if (i == USERNUM)
	{
		printf("\nThere is no such a user in filesystem.\n");
		return (int)NULL;
	}

	for (j = 0; j < NOFILE; j++)
	{
		if (user[i].u_ofile[j] != SYSOPENFILE + 1)
		{
			sys_no = user[i].u_ofile[j];
			inode = sys_ofile[sys_no].f_inode;
			iput(inode);
			sys_ofile[sys_no].f_count--;
			user[i].u_ofile[j] = SYSOPENFILE + 1;
		}
	}
	return 1;
}
