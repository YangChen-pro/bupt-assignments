#include "ext2fs.h"

int a, i, z;
unsigned long inodes_per_block, block_count;
unsigned long inode_no, x2, itable_offset;
unsigned long lower_limit, upper_limit;
unsigned long block_num;
unsigned short temp1;

void inode()
{
    int j, r_blocknum;
    char tmp[20], *str, *s1 = "y", *s2 = "Y";

    block_num = 0;
    printf("\nEnter the inode number : ");
    scanf("%ld", &inode_no);

    sync();

    if (inode_no > inodes_count)
    {
        printf("\nError : Invalid inode number. Largest inode number for %s is %ld\n", device_name, inodes_count);
        return;
    }

    block_count = inodes_count / inodes_per_block;

    x2 = find_groupdes_offset(inode_no);

    read_a_block(x2, CURR_DEVICE, act_size);

    Group = (void *)(super_block_buffer);

    itable_offset = (Group->bg_inode_table * 4096) + ((inode_no - lower_limit) * sizeof(struct ext2_inode));

    read_a_block(itable_offset, CURR_DEVICE, act_size);

    Inode = (void *)(super_block_buffer);

    printf("\nMode : %d", Inode->i_mode); //&0xfff
    printf("\tOctal : %o\t", Inode->i_mode);
    z = 6;
    for (i = 0; i <= 6; i += 3)
    {
        temp1 = Inode->i_mode & 0x1ff;
        temp1 = temp1 >> z;
        z = z - 3;
        if (temp1 & 4)
            printf("r");
        else
            printf("-");
        if (temp1 & 2)
            printf("w");
        else
            printf("-");
        if (temp1 & 1)
            printf("x");
        else
            printf("-");
    }
    printf("\n");

    printf("\nUID               : %01d", Inode->i_uid);
    printf("\nGID               : %01d", Inode->i_gid);
    printf("\nSize              : %9ld", Inode->i_size);
    printf("\nAccess time       : %24s", ctime((time_t *)&Inode->i_atime));
    printf("\nCreation time     : %24s", ctime((time_t *)&Inode->i_ctime));
    printf("\nModification time : %24s", ctime((time_t *)&Inode->i_mtime));
    printf("\nDeletion time     : %24s", ctime((time_t *)&Inode->i_dtime));
    printf("\nLinks             : %d", Inode->i_links_count);
    printf("\nFlags             : %03o", (Inode->i_mode & 0x1ff000) >> 12);
    printf("\nFLAGS             : %ld", Inode->i_flags);

    if (S_ISLNK(Inode->i_mode))
    {
        printf("\n\nIt's a symbolic link.");
        if (Inode->i_size <= 60)
            printf("\nPath conatined in Direct block is: %s", (char *)&Inode->i_block[0]);
        else
            printf("Slow Symbolic link");
        return;
    }
    // Display Data Blocks
    j = -1;
    while (++j < EXT2_N_BLOCKS)
    {
        block_num = 0;

        if (Inode->i_block[j])
        {
            printf("\n-------------------------------------------------------------------------------\n");
            if ((j < EXT2_NDIR_BLOCKS))
                printf("\nDirect Block : ");
            else if ((EXT2_IND_BLOCK) && (j == EXT2_IND_BLOCK))
                printf("\nINdirect Block :");
            else if ((EXT2_DIND_BLOCK) && (j == EXT2_DIND_BLOCK))
                printf("\nDouble Indirect Block :");
            else if ((EXT2_TIND_BLOCK) && (j == EXT2_TIND_BLOCK))
                printf("\nTriple Indirect Block : ");

            printf(" %d ", Inode->i_block[j]);
            block_num = Inode->i_block[j];
            r_blocknum = block_num;
            while (1)
            {

                printf("\n\nDo you want to see the file? (y/n) ");
                scanf("%s", tmp);

                str = (char *)malloc(strlen(tmp));
                strcpy(str, tmp);

                if ((strcmp(str, "n") == 0) || (strcmp(str, "N") == 0) || (strcmp(str, "Y") == 0) || (strcmp(str, "y") == 0))
                    break;
            }

            if ((strcmp(str, s1) == 0) || (strcmp(str, s2) == 0))
            {
                if (S_ISDIR(Inode->i_mode))
                {
                    printf("\nOh! It's a directory. You can view only a regular file.\n");
                    break;
                }
                else
                {
                    printf("\n-------------------------------------------------------------------------------\n");
                    show_ascii_file(block_num);
                    printf("\n-------------------------------------------------------------------------------\n");
                    while (1)
                    {
                        printf("\nDo you want to see the file in Hex and ascii format? (y/n) ");
                        scanf("%s", tmp);

                        str = (char *)malloc(strlen(tmp));
                        strcpy(str, tmp);

                        if ((strcmp(str, "n") == 0) || (strcmp(str, "N") == 0) || (strcmp(str, "Y") == 0) || (strcmp(str, "y") == 0))
                            break;
                    }
                    if ((strcmp(str, s1) == 0) || (strcmp(str, s2) == 0))
                    {
                        block_num = r_blocknum;
                        show_file(block_num);
                    }
                }
                read_a_block(itable_offset, CURR_DEVICE, act_size);
                Inode = (void *)(super_block_buffer);
            }
        }
        else if (j == 0)
            printf("\n\n\tThere is nothing stored in this file.");
    }
}

unsigned long find_groupdes_offset(unsigned long inode_no)
{

    block_count = inodes_count / inodes_per_block;

    for (a = 0, lower_limit = 1, upper_limit = inodes_per_block; a < block_count; a++, lower_limit += inodes_per_block, upper_limit += inodes_per_block)
    {
        if (inode_no <= upper_limit)
        {
            x2 = 4096 + (32 * a);
            break;
        }
    }
    return (x2);
}

unsigned long find_block_no(unsigned long inode_num)
{
    unsigned long x, x1;
    x1 = find_groupdes_offset(inode_num);
    read_a_block(x1, CURR_DEVICE, act_size);

    Group = (void *)(super_block_buffer);

    itable_offset = (Group->bg_inode_table * 4096) + ((inode_num - lower_limit) * sizeof(struct ext2_inode));

    read_a_block(itable_offset, CURR_DEVICE, act_size);

    Inode = (void *)(super_block_buffer);

    j = -1;

    while (++j < EXT2_N_BLOCKS)
    {
        if (Inode->i_block[j])
            return (Inode->i_block[j]);
    }
}
