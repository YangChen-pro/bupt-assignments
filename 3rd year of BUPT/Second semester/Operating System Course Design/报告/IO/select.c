/**
 * @file select.c
 * @brief 该程序演示了如何使用select()函数来处理多个套接字连接。
 *
 * 该程序创建了一个TCP服务器，该服务器在指定的IP地址和端口上监听传入的连接。
 * 它使用select()函数来监视多个文件描述符的读写事件。
 * 当新的客户端连接时，它接受连接并将新的套接字描述符添加到数组中。
 * 然后，它从已连接的客户端读取数据并发送回应。
 * 如果客户端断开连接，相应的套接字描述符将被关闭并在数组中设置为-1。
 *
 * 使用方法: ./select [local_ip] [local_port]
 *
 * @param local_ip 用于绑定服务器套接字的本地IP地址。
 * @param local_port 用于绑定服务器套接字的本地端口号。
 *
 * @return 成功返回0，错误返回-1。
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

// 用法函数，打印程序的使用方法
static void Usage(const char *proc)
{
    printf("%s [local_ip] [local_port]\n", proc);
}

// 存储文件描述符的数组
int array[4096];

// 启动服务器的函数，创建套接字并绑定到指定的IP和端口
static int start_up(const char *_ip, int _port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = inet_addr(_ip);
    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        exit(2);
    }
    if (listen(sock, 10) < 0)
    {
        perror("listen");
        exit(3);
    }
    return sock;
}

// 主函数
int main(int argc, char *argv[])
{
    // 检查参数数量
    if (argc != 3)
    {
        Usage(argv[0]);
        return -1;
    }
    // 启动服务器
    int listensock = start_up(argv[1], atoi(argv[2]));
    int maxfd = 0;
    fd_set rfds;
    fd_set wfds;
    array[0] = listensock;
    int i = 1;
    int array_size = sizeof(array) / sizeof(array[0]);
    // 初始化文件描述符数组
    for (; i < array_size; i++)
    {
        array[i] = -1;
    }
    // 主循环
    while (1)
    {
        // 清空文件描述符集合
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        // 将有效的文件描述符添加到集合中
        for (i = 0; i < array_size; ++i)
        {
            if (array[i] > 0)
            {
                FD_SET(array[i], &rfds);
                FD_SET(array[i], &wfds);
                if (array[i] > maxfd)
                {
                    maxfd = array[i];
                }
            }
        }
        // 使用select进行多路复用
        switch (select(maxfd + 1, &rfds, &wfds, NULL, NULL))
        {
        case 0:
        {
            printf("超时\n");
            break;
        }
        case -1:
        {
            perror("select");
            break;
        }
        default:
        {
            int j = 0;
            // 遍历文件描述符数组
            for (; j < array_size; ++j)
            {
                // 如果监听套接字有事件发生
                if (j == 0 && FD_ISSET(array[j], &rfds))
                {
                    struct sockaddr_in client;
                    socklen_t len = sizeof(client);
                    // 接受新的连接
                    int new_sock = accept(listensock, (struct sockaddr *)&client, &len);
                    if (new_sock < 0) // accept失败
                    {
                        perror("accept");
                        continue;
                    }
                    else // accept成功
                    {
                        printf("获得一个新的客户端%s\n", inet_ntoa(client.sin_addr));
                        fflush(stdout);
                        int k = 1;
                        // 将新的套接字添加到数组中
                        for (; k < array_size; ++k)
                        {
                            if (array[k] < 0)
                            {
                                array[k] = new_sock;
                                if (new_sock > maxfd)
                                    maxfd = new_sock;
                                break;
                            }
                        }
                        if (k == array_size)
                        {
                            close(new_sock);
                        }
                    }
                } // j == 0
                else if (j != 0 && FD_ISSET(array[j], &rfds))
                {
                    // 如果新的套接字有事件发生
                    char buf[1024];
                    // 读取数据
                    ssize_t s = read(array[j], buf, sizeof(buf) - 1);
                    if (s > 0) // 读取成功
                    {
                        buf[s] = 0;
                        printf("客户端说：%s\n", buf);
                        if (FD_ISSET(array[j], &wfds))
                        {
                            char *msg = "HTTP/1.0 200 OK <\r\n\r\n<html><h1>yingying beautiful</h1></html>\r\n";
                            write(array[j], msg, strlen(msg));
                        }
                    }
                    else if (0 == s)
                    {
                        printf("客户端退出！\n");
                        close(array[j]);
                        array[j] = -1;
                    }
                    else
                    {
                        perror("read");
                        close(array[j]);
                        array[j] = -1;
                    }
                } // else j != 0
            }
            break;
        }
        }
    }
    return 0;
}