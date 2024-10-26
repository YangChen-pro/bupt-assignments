
creat(uid, filename, mode) 
unsigned int uid;
char *filename;
unsigned short mode;
{
	int di_ith, di_ino;
	struct inode *inode;
	int i, j;

	i = 0;
	while (i < USERNUM) // user[]的值由函数login()注册，参看文件log.h
	{
		if (user[i].u_uid == uid)
		{
			user_id = i;
			break;
		}
		i++;
	}
	if (i == USERNUM)
	{
		printf("The user id is wrong.\n");
		exit(1);
	}

	di_ino = namei(filename);

	if (di_ino != -1) // 文件已经存在
	{
		inode = iget(di_ino);
		if (access(user_id, inode, mode) == 0)
		{
			iput(inode);
			printf("\nThe creating access not allowed\n");
			return 0;
		}
		for (i = 0; i < inode->di_size / BLOCKSIZ + 1; i++)
		{
			bfree(inode->di_addr[i]);
		}
		for (i = 0; i < SYSOPENFILE; i++)
			if (sys_ofile[i].f_inode == inode)
			{
				sys_ofile[i].f_off = 0;
			}
		for (i = 0; i < NOFILE; i++)
			if (user[user_id].u_ofile[i] == SYSOPENFILE + 1)
			{
				user[user_id].u_uid = inode->di_uid;
				user[user_id].u_gid = inode->di_gid;
				for (j = 0; j < SYSOPENFILE; i++)
					if (sys_ofile[j].f_count == 0)
					{
						user[user_id].u_ofile[i] = j;
						sys_ofile[j].f_flag = (char)mode;
					}
				return i;
			}
	}
	else
	{
		inode = ialloc();
		di_ith = iname(filename);

		dir.size++;

		dir.direct[di_ith].d_ino = inode->i_ino;
		inode->di_mode = user[user_id].u_default_mode;
		inode->di_uid = user[user_id].u_uid;
		inode->di_gid = user[user_id].u_gid;
		inode->di_size = 0;
		inode->di_number = 0;

		for (i = 0; i < SYSOPENFILE; i++)
			if (sys_ofile[i].f_count == 0)
			{
				break;
			}
		for (j = 0; j < NOFILE; i++)
			if (user[user_id].u_ofile[j] == SYSOPENFILE + 1)
			{
				break;
			}

		user[user_id].u_ofile[j] = i;
		sys_ofile[i].f_flag = (char)mode;
		sys_ofile[i].f_count = 0;
		sys_ofile[i].f_off = 0;
		sys_ofile[i].f_inode = inode;
		return j;
	}
	return 0;
}
