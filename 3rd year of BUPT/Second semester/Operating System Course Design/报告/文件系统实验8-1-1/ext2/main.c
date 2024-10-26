#include "ext2fs.h"

char *device_name;
char super_block_buffer[2048];

int CURR_DEVICE;
int act_size = 0, read_size = 0;

unsigned long inodes_count, blocks_count, group_blocks;
unsigned long block_size, first_group_desc_offset;
unsigned long i_map, b_map;
unsigned long inodes_per_block, INODE_BLOCK;
unsigned long temp, max_size;
unsigned long x = 4, offset;

struct ext2_super_block *Super;
struct ext2_group_desc *Group;
struct ext2_inode *Inode;
struct ext2_dir_entry_2 *Edir;

unsigned long g_offset;

main(int argc, char *argv[])
{
    int i, temp;
    char c, tmp[80], *str, *s1 = "n", *s2 = "N", *s3 = "y", *s4 = "Y";

    printf("\033[2J"); // ansi control code clear screen

    if (argc < 2)
    {
        printf("\nUsage :./ext2fs_std <device name>\n");
        exit(2);
    }
    device_name = (char *)malloc(strlen(argv[1] + 1));
    strcpy(device_name, argv[1]);

    open_dev();
    init_super();

    while (1)
    {
        printf("\n\nCommand h for help: ");
        scanf("%s", &c);
        c = tolower(c);
        switch (c)
        {
        case 'h':
            printf("\n ___________________________________________________");
            printf("\n| Command |          Action\t\t\t    |");
            printf("\n|_________|_________________________________________|");
            printf("\n|\t  |\t\t\t\t\t    |\n|   s     |    Show super block\t\t\t    |");
            printf("\n|   g     |    Show group descriptor\t\t    |");
            printf("\n|   d     |    Show internal structure of directory |");
            printf("\n|   i     |    Show inode information\t\t    |");
            printf("\n|   h	  |    Print this menu\t\t\t    |");
            printf("\n|   q     |    Quit\t\t\t\t    |");
            printf("\n|_________|_________________________________________|\n");

            break;

        case 's':
            super();
            break;
        case 'g':
            show_group();
            break;

        case 'd':
            directory();
            break;

        case 'i':
            inode();
            break;

        case 'q':
            close(CURR_DEVICE);
            exit(2);

        default:
            printf("Unknown command");
        }
        printf("\n===============================================================================");
    }
    close(CURR_DEVICE);
}

void open_dev()
{

    /* Open the device */
    CURR_DEVICE = 0;
    CURR_DEVICE = open(device_name, O_RDONLY);
    if (CURR_DEVICE < 0)
    {
        printf("\nUnable to open Device\n");
        exit(1);
    }
}
