#include <stdio.h>
#include <stdlib.h>        //exit
#include <sys/types.h>     //open,lseek
#include <unistd.h>        //lseek
#include <sys/stat.h>      //open
#include <fcntl.h>         //open
#include <linux/ext2_fs.h> //definition of structures
#include <time.h>          //for ctime
#include <string.h>
#include <termio.h>

/* Variable related with device */
char *device_name;
int CURR_DEVICE; // device handle

/* Variable used while using read() */
int act_size, read_size;

/*Buffer where information is read into */
char super_block_buffer[2048]; // buffer where information is read
unsigned long block_size;

void open_dev();
void super();
void init_super();
char *read_a_block(unsigned long, int, size_t);

/* Structures */
struct ext2_super_block *Super;
struct ext2_group_desc *Group;
struct ext2_inode *Inode;
struct ext2_dir_entry_2 *Edir;

/* super.c */
unsigned long inodes_count, blocks_count, group_blocks;
unsigned long i_map, b_map;
unsigned long inodes_per_block, INODE_BLOCK;
unsigned long temp, max_size;
unsigned long x, offset;
unsigned long first_group_desc_offset;
unsigned long frag_size;

/* group.c */
void show_group();
void group(int);
unsigned long g_offset;

/* chdc.c */
int file;       /*Flag used for distinguishing file and directory. If
            file=0;->directory
            file=1;->normal file
           */
int wrong_name; /* Flag used for distinguishing wrong name and right
         name. If
         wrong_name=0;-> name is right
         wrong_name=1;->name is wrong
         */
int i1, n1, j, j2;

unsigned long gdo, ito; /* gdo  - group descriptor table's offset
         ito  - Adjustment to inode table's offset
        */
unsigned long x, inod, itable_offset, Dblock;

char name_search[80], ntemp[80];
char *full_dir_name, *dir_name, *ptr, ch;

void directory();
int search(char *, char);
void show();
// void show1();

/*inode.c*/
void inode();

unsigned long find_groupdes_offset(unsigned long);
unsigned long find_block_no(unsigned long);

int a, i, z;

unsigned long inodes_per_block, block_count;
unsigned long inode_no, x2, itable_offset;
unsigned long lower_limit, upper_limit;
unsigned long block_num;
unsigned short temp1;

/*file.c*/
void show_file(unsigned long);
int show_hexascii();
unsigned char *ptr1, c;

/* ascii.c */
int show_ascii_file(unsigned long);
void show_ascii();
int i;

/* blockbitmap.c*/

unsigned char *ptr2;
void show_block_bm(unsigned long);
int z;
void blockbitmap();

/*inodebitmap.c*/
void show_inode_bm(unsigned long);
void inodebitmap();
