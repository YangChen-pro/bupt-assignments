
#include "ext2fs.h"

void show_block_bm(unsigned long block_num)
{

    block_num = block_num * 4096;

    read_a_block(block_num, CURR_DEVICE, act_size);

    ptr2 = (void *)(super_block_buffer);

    blockbitmap();

    /*Reading second half of the 2nd block*/
    if (block_size == EXT2_MAX_BLOCK_SIZE)
    {
        block_num = block_num + 2048;

        read_a_block(block_num, CURR_DEVICE, act_size);

        ptr2 = (void *)(super_block_buffer);
        blockbitmap();
    }
}

void blockbitmap()
{
    char c;
    int count = 0;
    scanf("%c", &c);
    for (i = 0; i < read_size; i++, ptr2++) // 4096,2048
    {
        for (z = 1; z <= 128; z *= 2)
        {
            if ((*ptr2) & z)
                printf("1");
            else
                printf("0");
        }
        printf(" ");
        if (i % 8 == 7 && count != 184)
        {
            printf("\n");
        }
        count++;
        if (count == 184)
        {
            printf("Press enter for the next page ");
            scanf("%c", &c);
            count = 0;
            printf("\n");
        }
    }
}
