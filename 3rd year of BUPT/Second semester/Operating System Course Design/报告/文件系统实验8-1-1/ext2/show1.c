include "ext2fs.h"

    void
    show1()
{
    sync();

    while (1)
    {
        inod = 0;
        read_a_block(x, CURR_DEVICE, act_size);
        Edir = (void *)(super_block_buffer);

        if (Edir->name_len == strlen(dir_name))
        {
            if (strncmp(Edir->name, dir_name, Edir->name_len) == 0)
            {
                x = find_block_no(Edir->inode);
                x = x * 4096;
            }
            else
                x += Edir->rec_len;
        }
        if (x % 4096 == 0)
            break;
    }
}
