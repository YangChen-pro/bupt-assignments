
struct inode *iget(unsigned int dinodeid)
{
	int existed = 0, inodeid;
	long addr;
	struct inode *temp, *newinode;

	inodeid = dinodeid % NHINO;
	if (hinode[inodeid].i_forw == NULL)
		temp = &hinode[inodeid];
	else
	{
		temp = hinode[inodeid].i_forw;
		while (temp->i_forw)
		{
			if ((int)temp->i_ino == inodeid)
			{
				existed = 1;
				temp->i_count++;
				return temp;
			}
			else
				temp = temp->i_forw;
		};
	}
	addr = DINODESTART + dinodeid * DINODESIZ;
	newinode = (struct inode *)malloc(sizeof(struct inode));
	fseek(fd, addr, SEEK_SET);
	fread(newinode, DINODESIZ, 1, fd);

	if (hinode[inodeid].i_forw == NULL)
	{
		newinode->i_forw = hinode[inodeid].i_forw;
		hinode[inodeid].i_forw = newinode;
		newinode->i_back = &(hinode[inodeid]);
	}
	else
	{
		newinode->i_forw = hinode[inodeid].i_forw;
		hinode[inodeid].i_forw = newinode;
		newinode->i_back = newinode->i_forw->i_back;
		newinode->i_forw->i_back = newinode;
	}

	newinode->i_count = 1;
	newinode->i_flag = 0;
	newinode->i_ino = dinodeid;
	fseek(fd, addr, SEEK_SET);
	fwrite(newinode, DINODESIZ, 1, fd);

	return newinode;
}
iput(struct inode *pinode)
{
	long addr;
	unsigned int block_num;
	int i;

	if (pinode->i_count > 1)
	{
		pinode->i_count--;
	}
	else
	{
		if (pinode->di_number != 0)
		{
			addr = DINODESTART + pinode->i_ino * DINODESIZ;
			fseek(fd, addr, SEEK_SET);
			fwrite(pinode, DINODESIZ, 1, fd);
		}
		else
		{
			block_num = pinode->di_size / BLOCKSIZ;
			for (i = 0; i < (int)block_num; i++)
			{
				balloc(pinode->di_addr[i]);
			}
			ifree(pinode->i_ino);
		};
		if (pinode->i_forw == NULL)
			pinode->i_back->i_forw = NULL;
		else
		{
			pinode->i_forw->i_back = pinode->i_back;
			pinode->i_back->i_forw = pinode->i_forw;
		};
		free(pinode);
	};
}
