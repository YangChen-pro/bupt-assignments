#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 128                // 每次读写缓存大小，影响运行效率
#define SRC_FILE_NAME "src_file.txt"   // 源文件名
#define DEST_FILE_NAME "dest_file.txt" // 目标文件名
#define OFFSET 0                       // 文件指针偏移量

int main()
{
    int src_file, dest_file;             // 源文件和目标文件描述符
    unsigned char src_buff[BUFFER_SIZE];  // 读缓存
    unsigned char dest_buff[BUFFER_SIZE]; // 写缓存
    int real_read_len = 0;               // 实际读取长度
    char str[BUFFER_SIZE] = "this is a test about\nclose()\nwrite()\nread()\nsleek()\nend of the file\n";

    // 创建源文件
    src_file = open(SRC_FILE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (src_file < 0)
    {
        printf("open file error!!!\n");
        exit(1);
    }

    // 向源文件写入数据
    write(src_file, str, strlen(str));
    // 创建目标文件
    dest_file = open(DEST_FILE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    if (dest_file < 0)
    {
        printf("open file error!!!\n");
        exit(1);
    }

    lseek(src_file, OFFSET, SEEK_SET); // 将源文件的读写指针移动到起始位置
    while ((real_read_len = read(src_file, src_buff, sizeof(src_buff))) > 0)
    {
        printf("src_buff: %s", src_buff);
        write(dest_file, src_buff, real_read_len);
    }
    lseek(dest_file, OFFSET, SEEK_SET); // 将目标文件的读写指针移动到起始位置
    while ((real_read_len = read(dest_file, dest_buff, sizeof(dest_buff))) > 0)
        ; // 读取目标文件内容
    printf("dest_buf: %s", dest_buff);
    close(src_file);
    close(dest_file);
    return 0;
}