
install()
{
	int i, j;
	printf("install\n");
	fd = fopen("filesystem", "a+b");
	if (fd == NULL)
	{
		printf("\nfilesys can not be loaded.\n");
		exit(0);
	}
	fseek(fd, BLOCKSIZ, SEEK_SET);
	fread(&filsys, 1, sizeof(struct filsys), fd);
	i = filsys.s_free[49];
	for (i = 0; i < NHINO; i++)
	{
		hinode[i].i_forw = NULL;
	}
	for (i = 0; i < SYSOPENFILE; i++)
	{
		sys_ofile[i].f_count = 0;
		sys_ofile[i].f_inode = NULL;
	}
	for (i = 0; i < USERNUM; i++)
	{
		user[i].u_uid = 0;
		user[i].u_gid = 0;
		for (j = 0; j < NOFILE; j++)
		{
			user[i].u_ofile[j] = SYSOPENFILE + 1;
		}
	}

	cur_path_inode = iget(1);
	dir.size = cur_path_inode->di_size / (DIRSIZ + 2);
	for (i = 0; i < DIRNUM; i++)
	{
		strcpy(dir.direct[i].d_name, "     ");
		dir.direct[i].d_ino = 0;
	}
	fseek(fd, DATASTART + BLOCKSIZ, SEEK_SET);
	fread(&dir.direct[BLOCKSIZ / (DIRSIZ + 2)], 1, 3 * (DIRSIZ + 2), fd);
	for (i = 0; i < dir.size; i++)
	{
		fseek(fd, DATASTART + BLOCKSIZ * cur_path_inode->di_addr[i], SEEK_SET);
		fread(&dir.direct[(BLOCKSIZ / (DIRSIZ + 2)) * i], 1, cur_path_inode->di_size % BLOCKSIZ, fd);
	}
}
