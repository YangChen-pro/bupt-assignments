#include <stdio.h>
#include "structure.h"

delete (char *filename)
{
	unsigned int dinodeid;
	struct inode *inode;

	dinodeid = namei(filename);
	if (dinodeid != (int)NULL)
		inode = iget(dinodeid);
	inode->di_number--;
	iput(inode);
}
