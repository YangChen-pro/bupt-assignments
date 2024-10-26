#include "ext2fs.h"

void show_group()
{
    char tmp[80], *str, *rep1 = "n", *rep2 = "N";
    int count;
    printf("\nThere are total %d group_blocks", group_blocks);

    for (i = 0; i < group_blocks; i++)
    {
        printf("\n\nGroup %d of %d", i, group_blocks - 1);
        count = i;
        group(i);
        i = count;
        if (i < (group_blocks - 1))
        {
            while (1)
            {
                printf("\n\nDo you want to view the next  block? (y/n) ");
                scanf("%s", &tmp);
                str = (char *)malloc(strlen(tmp));
                strcpy(str, tmp);
                if ((strcmp(str, "n") == 0) || (strcmp(str, "N") == 0) || (strcmp(str, "Y") == 0) || (strcmp(str, "y") == 0))
                    break;
            }
            if ((strcmp(str, rep1) == 0) || (strcmp(str, rep2) == 0))
                break;
        }
    }
}

void group(int n)
{

    char tmp[20], *str, *s1 = "y", *s2 = "Y";
    int z;
    g_offset = first_group_desc_offset + (32 * n); // 4224;
    /* Setting offset to read about nth block table
       i.e., 4096+( n * sizeof(struct ext2_group_desc))
       If n=4 then
       offset = 4096+(4*32) = 4224
     */

    read_a_block(g_offset, CURR_DEVICE, act_size);
    Group = (void *)(super_block_buffer);
    printf("\nBlock bitmap         = %ld", Group->bg_block_bitmap);
    printf("\nInode bitmap         = %ld", Group->bg_inode_bitmap);
    printf("\nInode table          = %ld", Group->bg_inode_table);
    printf("\nFree blocks count    = %d", Group->bg_free_blocks_count);
    printf("\nFree Inodes count    = %d", Group->bg_free_inodes_count);
    printf("\nUsed directory count = %d\n", Group->bg_used_dirs_count);

    while (1)
    {
        printf("\nDo you want to see the block bitmap of the present group? (y/n) ");
        scanf("%s", tmp);
        str = (char *)malloc(strlen(tmp));
        strcpy(str, tmp);
        if ((strcmp(str, "n") == 0) || (strcmp(str, "N") == 0) || (strcmp(str, "y") == 0) || (strcmp(str, "Y") == 0))
            break;
    }

    if ((strcmp(str, s1) == 0) || (strcmp(str, s2) == 0))
    {
        show_block_bm(Group->bg_block_bitmap);
    }

    while (1)
    {
        printf("\nDo you want to see the inode bitmap of the present group? (y/n) ");
        scanf("%s", tmp);
        str = (char *)malloc(strlen(tmp));
        strcpy(str, tmp);
        if ((strcmp(str, "n") == 0) || (strcmp(str, "N") == 0) || (strcmp(str, "y") == 0) || (strcmp(str, "Y") == 0))
            break;
    }

    if ((strcmp(str, s1) == 0) || (strcmp(str, s2) == 0))
    {
        read_a_block(g_offset, CURR_DEVICE, act_size);
        Group = (void *)(super_block_buffer);
        show_inode_bm(Group->bg_inode_bitmap);
    }
}
