#include "ext2fs.h"

int file = 0, wrong_name = 0, i1 = 0, n1 = 0, j = -1, j2;
char ch = '/';
unsigned long inod = 0;
char *full_dir_name, *dir_name, *ptr;

char name_search[80], ntemp[80];

unsigned long gdo, ito;
unsigned long itable_offset, Dblock;

void directory()
{
    char tstr[100];
    int len, count = 0;

    printf("\n\nThis shows how directories are maintained internally on the device %s. \nYou need to provide the absolute path to view the details. For example '/' would show the details of the root directory. \n\n", device_name);

again:
    printf("\nEnter the absolute path : ");
    scanf("%s", tstr);

    len = strlen(tstr);
    tstr[len] = '/';
    tstr[len + 1] = '\0';

    full_dir_name = (char *)malloc(strlen(tstr) + 2);
    strcpy(full_dir_name, tstr);

    if (*full_dir_name != '/')
    {
        printf("\nGive absolute path\n");
        goto again;
    }

    ptr = (char *)malloc(strlen(full_dir_name) + 1);
    ptr = full_dir_name;

    x = find_block_no(2); /*inode number 2  is reserved for root*/
    x = x * 4096;

    while (search(ptr, ch) != -1)
    {

        if (i1 == 0)
        {
            ptr++;
            continue;
        }
        else if (i1 > 0)
        {
            dir_name = (char *)malloc(i1 + 1);
            n1 = 0;
            while (n1 < i1)
            {
                dir_name[n1] = *ptr;
                ptr++;
                n1++;
            }
            dir_name[n1] = '\0';

            if (strlen(dir_name) == strlen("root"))
            {
                if (strcmp(dir_name, "root") == 0)
                {
                    ptr++;
                    full_dir_name = ptr;
                }
            }
            ptr++;
        }
    }

    // x=find_block_no(2);	/*inode number 2  is reserved for root*/
    // x=x*4096;

    i1 = 0;
    while (search(full_dir_name, ch) != -1)
    {
        if (i1 == 0)
        {
            full_dir_name++;
            continue;
        }
        else if (i1 > 0)
        {
            dir_name = (char *)malloc(i1 + 1);
            n1 = 0;
            while (n1 < i1)
            {
                dir_name[n1] = *full_dir_name;
                full_dir_name++;
                n1++;
            }
            dir_name[n1] = '\0';

            full_dir_name++;
            i1 = 0;
            show();
            if (file == 1)
            {
                printf("\n%s is not a directory\n", dir_name);
                file = 0;
                return;
            }
            if (wrong_name == 1)
            {
                wrong_name = 0;
                goto again;
            }
        }
    }

    printf("\nInode No	 Rec lenth	Name lenth	File name\n");

    while (1)
    {
        read_a_block(x, CURR_DEVICE, act_size);

        Edir = (void *)super_block_buffer;

        strncpy(ntemp, Edir->name, Edir->name_len);
        ntemp[Edir->name_len] = 0;

        printf("\n Inode = %-8lu rec_len = %-4lu name_len = %-3lu name = %s", Edir->inode, Edir->rec_len, Edir->name_len, ntemp);

        x += Edir->rec_len;
        if (x % 4096 == 0)
            break;
    }

    printf("\n");
}

void show()
{
    sync(); // flush file system buffers
    while (1)
    {
        inod = 0;
        read_a_block(x, CURR_DEVICE, act_size);

        Edir = (void *)(super_block_buffer);

        if (Edir->name_len == strlen(dir_name))
        /*search the dir for the string in dir_name*/
        {
            if (strncmp(Edir->name, dir_name, Edir->name_len) == 0)
            {
                /*
                 * If strings are same then find the inode number.
                 */
                inod = Edir->inode;

                if (inod <= 128768)
                {
                    /*
                     *Below variable "gdo" is for group descriptor table's
                     offset and variable "ito" for adjustments in inode table
                     offset.
                     */
                    if (inod <= 16096)
                    {
                        gdo = 1 * 4096;
                        ito = 1;
                    }
                    else if (inod <= 32192)
                    {
                        gdo = 1 * 4096 + 32;
                        ito = 16097;
                    }
                    else if (inod <= 48288)
                    {
                        gdo = 1 * 4096 + 64;
                        ito = 32193;
                    }
                    else if (inod <= 64384)
                    {
                        gdo = 1 * 4096 + 96;
                        ito = 48289;
                    }
                    else if (inod <= 80480)
                    {
                        gdo = 1 * 4096 + 128;
                        ito = 64385;
                    }
                    else if (inod <= 96576)
                    {
                        gdo = 1 * 4096 + 160;
                        ito = 80481;
                    }
                    else if (inod <= 112672)
                    {
                        gdo = 1 * 4096 + 192;
                        ito = 96577;
                    }
                    else if (inod < 128768)
                    {
                        gdo = 1 * 4096 + 224;
                        ito = 112673;
                    }
                }
                else
                    printf("\nCORRUPTED FS. USE FSCK\n");

                read_a_block(gdo, CURR_DEVICE, act_size);

                Group = (void *)(super_block_buffer);

                itable_offset = (Group->bg_inode_table * 4096) + ((inod - ito) * sizeof(struct ext2_inode));

                read_a_block(itable_offset, CURR_DEVICE, act_size);

                Inode = (void *)(super_block_buffer);
                if (S_ISLNK(Inode->i_mode))
                {
                    printf("\n==========================================================================");
                    printf("\nError : %s is a symbolic link. Symbolic links are currently not supported.", dir_name);
                    wrong_name = 1;
                    printf("\n==========================================================================\n");
                    return;
                }
                if (!S_ISDIR(Inode->i_mode))
                /* Determine whether it is a directory or not*/
                {
                    file = 1;
                    return;
                }
                else
                {
                    file = 0;
                }
                j = -1;
                while (++j < EXT2_N_BLOCKS)
                {
                    if (Inode->i_block[j])
                    {
                        Dblock = Inode->i_block[j];
                        x = Dblock * 4096;
                        return;
                    }
                }
            }
        }
        x += Edir->rec_len;
        if (x % 4096 == 0)
        {
            printf("\n%s is not a valid name\n", dir_name);
            wrong_name = 1;
            return;
        }
    }
    return;
}

int search(char *s, char ch)
{
    i1 = 0;
    while (*s)
    {
        if (*s == ch)
            return i1;
        s++;
        i1++;
    }
    return -1;
}

/*void dir()
{
    sync();

    while(1)
    {
        inod=0;
        read_a_block(x,CURR_DEVICE,act_size);
        Edir=(void *)(super_block_buffer);

        if(Edir->name_len==strlen(dir_name))
        {
            if(strncmp(Edir->name,dir_name,Edir->name_len)==0)
                {
                    x=find_block_no(Edir->inode);
                    x=x*4096;
                    break;
                }
        else x+=Edir->rec_len;
        }
    if(x%4096==0)
        break;
    }
}*/
