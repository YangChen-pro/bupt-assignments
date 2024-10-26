#include <stdio.h>
#include "structure.h"

static unsigned int block_buf[BLOCKSIZ / sizeof(int)];
unsigned int balloc()
{
	unsigned int free_block;
	int i, flag;

	if (filsys.s_nfree == 0)
	{
		printf("\nDisk Full!!\n");
		return DISKFULL;
	}
	i = filsys.s_pfree;
	flag = (i == 0);

	if (flag) // 该BLOCK组全部用了
	{
		fseek(fd, DATASTART + BLOCKSIZ * (filsys.s_free[NICFREE - 1] + 1), SEEK_SET);
		// filsys.s_free[NICFREE-1]+1指向下一个block组的地址块
		fread(block_buf, 1, BLOCKSIZ, fd);
		for (i = 0; i < NICFREE; i++)
		{
			filsys.s_free[i] = block_buf[i];
		} // 将待用block组的地址读入超级块
		filsys.s_pfree = NICFREE - 1;
		free_block = filsys.s_free[filsys.s_pfree];
	}
	else
	{
		free_block = filsys.s_free[filsys.s_pfree];
		filsys.s_pfree--;
	}
	filsys.s_nfree--;
	filsys.s_fmod = SUPDATE;

	return free_block;
}

bfree(unsigned int block_num)
{
	int i;

	if (filsys.s_pfree == NICFREE - 1)
	// 表示回收的block已经可以组成一个block组了
	{
		for (i = 0; i < NICFREE; i++)
		{
			block_buf[i] = filsys.s_free[NICFREE - 1 - i];
		}
		filsys.s_pfree = 0;

		fseek(fd, DATASTART + BLOCKSIZ * (filsys.s_free[0]), SEEK_SET);
		// filsys.s_free[0]为当前BLOCK组的地址块
		fwrite(block_buf, 1, BLOCKSIZ, fd);
	}
	else
		filsys.s_pfree++;
	filsys.s_nfree++;
	filsys.s_fmod = SUPDATE;
}
