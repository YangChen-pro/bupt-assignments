#include "ext2fs.h"

int i;

int show_ascii_file(unsigned long block_num)
{

    block_num = block_num * 4096;

    read_a_block(block_num, CURR_DEVICE, act_size);

    ptr1 = super_block_buffer;

    printf("\n");

    show_ascii();

    if (block_size == EXT2_MAX_BLOCK_SIZE)
    {
        block_num = block_num + 2048;
        read_a_block(block_num, CURR_DEVICE, act_size);
        ptr1 = super_block_buffer;
        show_ascii();
    }
}

void show_ascii()
{

    for (i = 0; i < read_size; i++) // 2048
    {

        c = ptr1[i];

        if (c == 0xa) // New line
            printf("\n");

        else if (c == 0x0) // End of file
            break;

        else

        {
            if ((c > 31) && (c < 127))
            {

                printf("%c", c);
            }
            else
                printf(" ");
        }
    }

    ptr1 = 0;
}
