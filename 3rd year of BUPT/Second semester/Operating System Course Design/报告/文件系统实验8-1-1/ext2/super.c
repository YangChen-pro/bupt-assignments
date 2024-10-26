#include "ext2fs.h"

void init_super()
{
    char super_buffer[2048];
    sync(); // flush file system buffers

    offset = 0;

    lseek(CURR_DEVICE, offset, SEEK_SET);
    if ((act_size = read(CURR_DEVICE, super_buffer, 2048)) != 2048)
        perror("\nUnable to read full block\n");

    Super = (void *)(super_buffer + 1024);
    if (Super->s_magic != EXT2_SUPER_MAGIC)
    {
        printf("\nSignature is NOT MATCHING.");
        printf("\nThe device %s does not have ext2 as its file system.\n", device_name);
        printf("\nthis programme currently supports only ext2.\nProvide device-name that supports ext2.\n\n");
        close(CURR_DEVICE);
        exit(3);
    }
    inodes_count = Super->s_inodes_count;
    blocks_count = Super->s_blocks_count;
    inodes_per_block = Super->s_inodes_per_group;
    block_size = EXT2_MIN_BLOCK_SIZE << Super->s_log_block_size;
    read_size = 2048;
    if (block_size == EXT2_MIN_BLOCK_SIZE)
    {
        first_group_desc_offset = 2 * EXT2_MIN_BLOCK_SIZE;
        read_size = block_size;
    }
    else
    {
        first_group_desc_offset = block_size;
        read_size = 2048;
    }
    group_blocks = Super->s_inodes_count / Super->s_inodes_per_group;
}

void super()
{
    unsigned long frag_size;
    sync(); // flush file system buffers

    offset = 0;

    read_a_block(offset, CURR_DEVICE, act_size);

    Super = (void *)(super_block_buffer + 1024);

    if (Super->s_magic != EXT2_SUPER_MAGIC)
        printf("\nSignature is NOT MATCHING-------------\n");
    inodes_count = Super->s_inodes_count;
    blocks_count = Super->s_blocks_count;
    inodes_per_block = Super->s_inodes_per_group;

    printf("\nInode count		: %ld\n", Super->s_inodes_count);
    printf("Block count		: %ld\n", Super->s_blocks_count);
    printf("Reserved Block count	: %ld\n", Super->s_r_blocks_count);
    printf("Free Blocks count	: %ld\n", Super->s_free_blocks_count);
    printf("Free Inodes count	: %ld\n", Super->s_free_inodes_count);
    frag_size = EXT2_MIN_BLOCK_SIZE << Super->s_log_frag_size;
    printf("Fragment size		: %ld \n", frag_size);
    block_size = EXT2_MIN_BLOCK_SIZE << Super->s_log_block_size;
    printf("Block size		: %ld\n", block_size);
    printf("Magic Signature 	: 0x%lX\n", Super->s_magic);
    printf("Inodes per block group	: %ld\n", Super->s_inodes_per_group);
    printf("Blocks per block group 	: %ld\n", Super->s_blocks_per_group);
    printf("First data block 	: %ld\n", Super->s_first_data_block);
    printf("Last Mount time		: %24s\n", ctime((time_t *)&Super->s_mtime));
    printf("Write time		: %24s\n", ctime((time_t *)&Super->s_wtime));
    printf("Mount count 		: %d\n", Super->s_mnt_count);
    printf("Filesystem state	: %d\n", Super->s_state);
    printf("System last checked on  : %24s\n", ctime((time_t *)&Super->s_lastcheck));
    // printf("Max time between checks : %24s\n",ctime((time_t *)&Super->s_checkinterval));

    group_blocks = Super->s_inodes_count / Super->s_inodes_per_group;
}

char *read_a_block(unsigned long offset, int CURR_DEVICE, size_t act_size)
{

    lseek(CURR_DEVICE, offset, SEEK_SET);
    /*if((act_size=read(CURR_DEVICE,super_block_buffer,read_size)) != read_size)
        perror("\nUnable to read full block\n");*/
    read(CURR_DEVICE, super_block_buffer, read_size);

    return (super_block_buffer);
}
