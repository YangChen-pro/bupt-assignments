
_ls()
{
	unsigned int di_mode;
	int i, j, one;
	struct inode *temp_inode;

	printf("CURRENT DIRECTORY:\n");
	for (i = 0; i < dir.size; i++)
	{
		if (dir.direct[i].d_ino != DIEMPTY)
		{
			printf("%sDIRSIZ", dir.direct[i].d_name);
			temp_inode = iget(dir.direct[i].d_ino);
			di_mode = temp_inode->di_mode;
			for (j = 0; j < 9; j++)
			{
				one = di_mode % 2;
				di_mode = di_mode / 2;

				if (one)
					printf("x");
				else
					printf("-");
			}
			if (temp_inode->di_mode && DIFILE == 1)
			{
				printf("%ld\n", temp_inode->di_size);
				printf("block chain:");
				for (i = 0; i < temp_inode->di_size / BLOCKSIZ + 1; i++)
					printf("%4d", temp_inode->di_addr[i]);
				printf("\n");
			}
			else
				printf("<dir>\n");
			iput(temp_inode);
		}
	}
}

mkdir(char *dirname)
{
	int dirid, dirpos;
	struct inode *inode;
	struct direct buf[BLOCKSIZ / (DIRSIZ + 2)];
	unsigned int block;

	dirid = namei(dirname);
	if (dirid != -1) // dirid==-1表示没有该目录名存在；
	{
		inode = iget(dirid);
		if (inode->di_mode & DIDIR)
			printf("\n%s directory already existed!!\n");
		else
			printf("\n%s is a file name&can not creat a dir the same name", dirname);
		iput(inode);
		return 0;
	}

	dirpos = iname(dirname);
	inode = ialloc();
	inode->i_ino = dirpos;
	dir.direct[dirpos].d_ino = inode->i_ino;
	dir.size++;

	strcpy(buf[0].d_name, ".");
	buf[0].d_ino = dirid;
	strcpy(buf[1].d_name, "..");
	buf[1].d_ino = cur_path_inode->i_ino;

	block = balloc();
	fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
	fwrite(buf, 1, BLOCKSIZ, fd);

	inode->di_size = 2 * (DIRSIZ + 2);
	inode->di_number = 1;
	inode->di_mode = user[user_id].u_default_mode;

	inode->di_uid = user[user_id].u_uid;
	inode->di_gid = user[user_id].u_gid;
	inode->di_addr[0] = block;

	iput(inode);
	return 0;
}

chdir(char *dirname)
{
	int dirid;
	int temp;
	struct inode *inode;
	short block;
	int i, j, low = 0, high = 0;
	dirid = namei(dirname);
	if (dirid == -1)
	{
		printf("\nThe %s is not a existed directory\n", dirname);
		return 0;
	}
	inode = iget(dirid);
	if (!access(user_id, inode, user[user_id].u_default_mode))
	{
		printf("\nThe filesystem has not access to the directory %s", dirname);
		iput(inode);
		return 0;
	}

	for (i = 0; i < dir.size; i++)
	{
		for (j = 0; j < DIRNUM; j++)
		{
			temp = dir.direct[j].d_ino;

			if (dir.direct[j].d_ino == 0 || dir.direct[j].d_ino > MAX)
				break;
		}
		dir.direct[j].d_ino = 0;
	}

	for (i = 0; i < cur_path_inode->di_size / BLOCKSIZ + 1; i++)
	{
		bfree(cur_path_inode->di_addr[i]);
	}
	i = dir.size;
	for (i = 0; i < dir.size; i += BLOCKSIZ / (DIRSIZ + 2))
	{
		block = balloc();
		cur_path_inode->di_addr[i] = block;
		fseek(fd, DATASTART + block * BLOCKSIZ, SEEK_SET);
		fwrite(&dir.direct[i], 1, BLOCKSIZ, fd);
	}
	cur_path_inode->di_size = dir.size * (DIRSIZ + 2);
	iput(cur_path_inode);

	cur_path_inode = inode;

	i = inode->di_size / BLOCKSIZ + 1;
	j = 0;
	for (i = 0; i < inode->di_size / BLOCKSIZ + 1; i++)
	{
		fseek(fd, DATASTART + inode->di_addr[i] * BLOCKSIZ, SEEK_SET);
		fread(&dir.direct[j], 1, BLOCKSIZ, fd);
		j += BLOCKSIZ / (DIRSIZ + 2);
	}
	return 0;
}
