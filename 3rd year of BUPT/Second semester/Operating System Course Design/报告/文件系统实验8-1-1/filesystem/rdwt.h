#include <stdio.h>
#include "structure.h"

unsigned int read(int cfd, char *buf, unsigned int size)
{
	unsigned long off;
	int block, block_off, i, j;
	struct inode *inode;
	char *temp_buf;

	inode = sys_ofile[user[user_id].u_ofile[cfd]].f_inode;
	if (!(sys_ofile[user[user_id].u_ofile[cfd]].f_flag & FREAD))
	{
		printf("\nthe file is not opened for read\n");
		return 0;
	}

	temp_buf = buf;

	off = sys_ofile[user[user_id].u_ofile[cfd]].f_off;
	if ((off + size) > inode->di_size)
		size = inode->di_size - off;

	block_off = off % BLOCKSIZ;
	block = off / BLOCKSIZ;

	if (block_off + size < BLOCKSIZ)
	{
		fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ + block_off, SEEK_SET);
		fread(buf, 1, size, fd);
		return size;
	}

	fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ + block_off, SEEK_SET);
	fread(temp_buf, 1, BLOCKSIZ - block_off, fd);

	temp_buf += BLOCKSIZ - block_off;
	j = (inode->di_size - off - block_off) / BLOCKSIZ;
	for (i = 0; i < (int)(size - block_off) / BLOCKSIZ; i++)
	{
		fseek(fd, DATASTART + inode->di_addr[j + i] * BLOCKSIZ, SEEK_SET);
		fread(temp_buf, 1, BLOCKSIZ, fd);
		temp_buf += BLOCKSIZ;
	}
	block_off = (size - block_off) % BLOCKSIZ;
	block = inode->di_addr[off + size / BLOCKSIZ + 1];
	fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
	fread(temp_buf, 1, block_off, fd);

	sys_ofile[user[user_id].u_ofile[cfd]].f_off += size;

	return size;
}

unsigned int write(int cfd, char *buf, unsigned int size)
{
	unsigned long off;
	int block, block_off, i;
	struct inode *inode;
	char *temp_buf;
	i = user[user_id].u_ofile[cfd];
	inode = sys_ofile[user[user_id].u_ofile[cfd]].f_inode;
	if (!(sys_ofile[user[user_id].u_ofile[cfd]].f_flag & FWRITE))
	{
		printf("\nthe file is not opened for write\n");
		return 0;
	}
	temp_buf = buf;

	off = sys_ofile[user[user_id].u_ofile[cfd]].f_off;
	block_off = off % BLOCKSIZ;
	block = off / BLOCKSIZ;
	inode->di_addr[block] = balloc();
	if (block_off + size < BLOCKSIZ)
	{
		fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ + block_off, SEEK_SET);
		fwrite(buf, 1, size, fd);
		return size;
	}
	fseek(fd, DATASTART + inode->di_addr[block] * BLOCKSIZ + block_off, SEEK_SET);
	fwrite(temp_buf, 1, BLOCKSIZ - block_off, fd);

	temp_buf += BLOCKSIZ - block_off;
	for (i = 0; i < (int)(size - block_off) / BLOCKSIZ - 1; i++)
	{
		inode->di_addr[block + 1 + i] = balloc();
		fseek(fd, DATASTART + inode->di_addr[block + 1 + i] * BLOCKSIZ, SEEK_SET);
		fwrite(temp_buf, 1, BLOCKSIZ, fd);
		temp_buf += BLOCKSIZ;
	}
	// 下面是对不够一个block的数据进行处理
	block_off = (size - block_off) % BLOCKSIZ;
	block = inode->di_addr[off + size / BLOCKSIZ + 1] = balloc();
	fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
	fwrite(temp_buf, 1, block_off, fd);

	sys_ofile[user[user_id].u_ofile[cfd]].f_off += size;
	return size;
}
