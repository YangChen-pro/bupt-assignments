#include <stdio.h>
#include "structure.h"

close(user_id, cfd) 
unsigned int user_id;
unsigned short cfd;
{
	struct inode *inode;

	inode = sys_ofile[user[user_id].u_ofile[cfd]].f_inode;
	iput(inode);
	sys_ofile[user[user_id].u_ofile[cfd]].f_count--;
	user[user_id].u_ofile[cfd] = SYSOPENFILE + 1;
}
