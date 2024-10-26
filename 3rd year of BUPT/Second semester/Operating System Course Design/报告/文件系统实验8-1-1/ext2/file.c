#include "ext2fs.h"

unsigned char *ptr1, c, *ptr2;

void show_file(unsigned long block_num)
{
    j = 0;
    block_num = block_num * 4096;

    read_a_block(block_num, CURR_DEVICE, act_size);

    ptr1 = super_block_buffer;

    printf("\nHex Format	        	Ascii Format\n");
    show_hexascii();

    if (block_size == EXT2_MAX_BLOCK_SIZE)
    {
        block_num = block_num + 2048;
        read_a_block(block_num, CURR_DEVICE, act_size);
        ptr1 = super_block_buffer;
        show_hexascii();
    }
}

int show_hexascii()
{
    j = 0;
    while (j * 16 < read_size) // 2048
    {

        for (i = j * 16; i < (j * 16 + 8); i++)
            if (i < read_size)             // 2048
                printf("%2.2X ", ptr1[i]); // 2nd col
            else
                printf("	");
        printf(":");
        for (i = (j * 16 + 8); i < (j * 16 + 16); i++)
            if (i < read_size)             // 2048
                printf("%2.2X ", ptr1[i]); // 3rd col
            else
                printf("	");

        /*		printf("| ");
                for(i=(j*16);((i<(j*16+16))&&(i<read_size));i++)//2048
                {
                    c=ptr1[i];
                    if(c==0x0)
                        return(0);
                            if ((c > 31) && (c < 127)) {

                                printf("%c", c);
                }
                            else
                            printf(" ");
                }*/
        j++;
    }
    printf("\n");
}
