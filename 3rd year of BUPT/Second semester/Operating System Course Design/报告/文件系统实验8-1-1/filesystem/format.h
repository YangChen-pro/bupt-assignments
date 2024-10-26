format()
{
	struct inode *inode;
	struct direct dir_buf[BLOCKSIZ / (DIRSIZ + 2)];
	struct pwd passwd[BLOCKSIZ / (PWDSIZ + 4)];
	struct filsys filsys;
	unsigned int block_buf[BLOCKSIZ / sizeof(int)];
	char *buf;
	char *empty;
	int i, j, k;

	fd = fopen("filesystem", "w+b");
	buf = (char *)malloc((DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char));
	if (fd == NULL)
	{
		printf("\nfile system file creat failed!!\n");
		exit(0);
	}
	fseek(fd, 0, SEEK_SET);
	fwrite(buf, 1, (DINODEBLK + FILEBLK + 2) * BLOCKSIZ * sizeof(char), fd);

	free(buf);

	passwd[0].p_uid = 2116;
	passwd[0].p_gid = 03;
	strcpy(passwd[0].password, "don1");
	passwd[1].p_uid = 2117;
	passwd[1].p_gid = 03;
	strcpy(passwd[1].password, "don2");
	passwd[2].p_uid = 2118;
	passwd[2].p_gid = 04;
	strcpy(passwd[2].password, "abcd");
	passwd[3].p_uid = 2119;
	passwd[3].p_gid = 04;
	strcpy(passwd[3].password, "don4");
	passwd[4].p_uid = 2220;
	passwd[4].p_gid = 05;
	strcpy(passwd[4].password, "don5");

	inode = iget(0);
	inode->di_mode = DIEMPTY;
	iput(inode);

	inode = iget(1);
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIDIR;
	inode->di_size = 3 * (DIRSIZ + 2);
	inode->di_addr[0] = 0;

	strcpy(dir_buf[0].d_name, "..");
	dir_buf[0].d_ino = 1;
	strcpy(dir_buf[1].d_name, ".");
	dir_buf[1].d_ino = 1;
	strcpy(dir_buf[2].d_name, "etc");
	dir_buf[2].d_ino = 2;

	fseek(fd, DATASTART, SEEK_SET);
	fwrite(dir_buf, 1, 3 * (DIRSIZ + 2), fd);

	iput(inode);
	fseek(fd, 1056, SEEK_SET);
	fread(inode, DINODESIZ, 1, fd);

	inode = iget(1);
	iput(inode);

	inode = iget(2);
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIDIR;
	inode->di_size = 3 * (DIRSIZ + 2);
	inode->di_addr[0] = 1;
	strcpy(dir_buf[0].d_name, "..");
	dir_buf[0].d_ino = 1;
	strcpy(dir_buf[1].d_name, ".");
	dir_buf[1].d_ino = 2;
	strcpy(dir_buf[2].d_name, "password");
	dir_buf[2].d_ino = 3;

	fseek(fd, DATASTART + BLOCKSIZ * 1, SEEK_SET);
	fwrite(dir_buf, 1, 3 * (DIRSIZ + 2), fd);
	iput(inode);
	inode = iget(3);
	inode->di_number = 1;
	inode->di_mode = DEFAULTMODE | DIFILE;
	inode->di_size = BLOCKSIZ;
	inode->di_addr[0] = 2;
	for (i = 5; i < PWDNUM; i++)
	{
		passwd[i].p_uid = 0;
		passwd[i].p_gid = 0;
		strcpy(passwd[i].password, "       ");
	}

	fseek(fd, DATASTART + 2 * BLOCKSIZ, SEEK_SET);
	fwrite(passwd, 1, BLOCKSIZ, fd);
	iput(inode);
	filsys.s_isize = DINODEBLK;
	filsys.s_fsize = FILEBLK;
	filsys.s_ninode = DINODEBLK * BLOCKSIZ / DINODESIZ - 4;
	filsys.s_nfree = FILEBLK - 3;
	for (i = 0; i < NICINOD; i++)
	{
		filsys.s_inode[i] = 4 + i;
	}
	filsys.s_pinode = 0;
	filsys.s_rinode = NICINOD + 4;

	for (i = NICFREE + 2; i < FILEBLK; i += 50) // 为何要加2，参看149行的注释
	{
		for (j = 0; j < NICFREE; j++)
		{
			block_buf[NICFREE - 1 - j] = i - j;
		}
		fseek(fd, DATASTART + BLOCKSIZ * (i - 49), SEEK_SET);
		fwrite(block_buf, 1, BLOCKSIZ, fd);
	} // 当i=502之后，完成文件块502～453的写入；
	  // 之后文件块512～503不能进行，需要特殊处理
	for (i = 503; i < 512; i++)
		block_buf[i - 503] = i;
	fseek(fd, DATASTART + BLOCKSIZ * 503, SEEK_SET);
	fwrite(block_buf, 1, BLOCKSIZ, fd); // 至此，才完成512块文件块的定位

	for (i = 0; i < NICFREE; i++)
	{
		filsys.s_free[i] = i + 3; // 从DATASTART的第一个BLOCK作为MAIN DIRECTORY
								  // 第二个BLOCK作为etc目录
								  // 第三个BLOCK作为password文件
		// 故此i要加3
	}
	// 每50个BLOCK成组，在每个BLOCK组当中的第一个BLOCK（称为地址块）放有整个BLOCK
	// 组的地址。这样，filsys.s_free[0]指向该组的地址块，而filsys.s_free[49]＋1
	// 则指向下一个组的地址块。
	j = 1;
	filsys.s_pfree = NICFREE - j;
	filsys.s_pinode = 0;
	fseek(fd, BLOCKSIZ, SEEK_SET);
	fwrite(&filsys, 1, sizeof(struct filsys), fd);
	fclose(fd);
}
