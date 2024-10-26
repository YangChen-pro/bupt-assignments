#include "ext2fs.h"

void show_inode_bm(unsigned long block_num)
{
    block_num = block_num * 4096;

    read_a_block(block_num, CURR_DEVICE, act_size);

    ptr = (void *)(super_block_buffer);

    inodebitmap();
    block_num = 0;
}

void inodebitmap()
{
    char c;
    int count = 0;

    scanf("%c", &c);

    for (i = 0; i < 2012; i++, ptr++)
    {
        for (z = 1; z <= 128; z *= 2)
        {
            if ((*ptr) & z)
                printf("1");
            else
                printf("0");
        }
        printf(" ");
        if (i % 8 == 7 && count != 184)
        {
            printf("\n", i);
        }
        count++;
        if (count == 184)
        {
            printf("Press enter for the next page");
            scanf("%c", &c);
            count = 0;
            printf("\n");
        }
    }
}
