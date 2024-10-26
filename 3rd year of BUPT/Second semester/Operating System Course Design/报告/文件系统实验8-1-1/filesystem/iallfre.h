#include <stdio.h>
#include "structure.h"

static struct dinode block_buf1[BLOCKSIZ / DINODESIZ];

struct inode *ialloc()
{
	struct inode *temp_inode;
	unsigned int cur_di;
	int i, count, block_end_flag;

	i = filsys.s_pinode;
	if (filsys.s_pinode == NICINOD)
	{
		i = 0;
		count = 0;
		block_end_flag = 1;
		filsys.s_pinode = NICINOD - 1;
		cur_di = filsys.s_rinode;
		while ((count < NICINOD) || (count <= (int)filsys.s_ninode))
		{
			if (block_end_flag)
			{
				fseek(fd, DINODESTART + cur_di * DINODESIZ, SEEK_SET);
				fread(block_buf1, 1, BLOCKSIZ, fd);
				block_end_flag = 0;
				i = 0;
			}
			while (block_buf1[i].di_mode == DIEMPTY)
			{
				cur_di++;
				i++;
			}
			if (i == NICINOD)
				block_end_flag = 1;
			else
			{
				filsys.s_inode[filsys.s_pinode--] = cur_di;
				count++;
			}
		}
		filsys.s_rinode = cur_di;
	}
	i = filsys.s_inode[filsys.s_pinode];
	temp_inode = iget(filsys.s_inode[filsys.s_pinode]);
	fseek(fd, DINODESTART + filsys.s_inode[filsys.s_pinode] * DINODESIZ, SEEK_SET);
	fwrite(temp_inode, 1, DINODESIZ, fd);
	filsys.s_pinode++;
	filsys.s_ninode--;
	filsys.s_fmod = SUPDATE;
	return temp_inode;
}

ifree(unsigned dinodeid)
{
	filsys.s_ninode++;
	if (filsys.s_pinode != NICINOD)
	{
		filsys.s_inode[filsys.s_pinode] = dinodeid;
		filsys.s_pinode++;
	}
	else
	{
		if (dinodeid < filsys.s_rinode)
		{
			filsys.s_inode[NICINOD] = dinodeid;
			filsys.s_rinode = dinodeid;
		}
	}
}
