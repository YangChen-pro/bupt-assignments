/**
 * 该程序使用poll()函数来处理多个客户端连接的简单TCP服务器示例。
 * 它监听指定的IP地址和端口，接受传入的连接，并从连接的客户端读取数据。
 * 
 * 用法：./program [local_ip] [local_port]
 * 
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数的数组。
 * @return 如果程序成功执行，则返回0，否则返回错误代码。
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

/**
 * 显示程序的用法。
 * 
 * @param proc 程序的名称。
 */
static void usage(const char *proc)
{
    printf("%s [local_ip] [local_port]\n", proc);
}

/**
 * 创建并设置一个用于监听指定IP地址和端口的套接字。
 * 
 * @param _ip 要绑定套接字的本地IP地址。
 * @param _port 要绑定套接字的本地端口。
 * @return 创建的套接字文件描述符，如果发生错误则返回错误代码。
 */
int start_up(const char *_ip, int _port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        return 2;
    }
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = inet_addr(_ip);
    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        return 3;
    }
    if (listen(sock, 10) < 0)
    {
        perror("listen");
        return 4;
    }
    return sock;
}

/**
 * 程序的主函数。
 * 
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数的数组。
 * @return 如果程序成功执行，则返回0，否则返回错误代码。
 */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        usage(argv[0]);
        return 1;
    }
    int sock = start_up(argv[1], atoi(argv[2]));
    struct pollfd peerfd[1024];
    peerfd[0].fd = sock;
    peerfd[0].events = POLLIN;
    int nfds = 1;
    int ret;
    int maxsize = sizeof(peerfd) / sizeof(peerfd[0]);
    int i = 1;
    int timeout = -1;
    for (; i < maxsize; ++i)
    {
        peerfd[i].fd = -1;
    }
    while (1)
    {
        switch (ret = poll(peerfd, nfds, timeout))
        {
        case 0:
            printf("超时...\n");
            break;
        case -1:
            perror("poll");
            break;
        default:
        {
            if (peerfd[0].revents & POLLIN)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int new_sock = accept(sock, (struct sockaddr *)&client, &len);
                printf("接受完成 %d\n", new_sock);
                if (new_sock < 0)
                {
                    perror("accept");
                    continue;
                }
                printf("获得一个新客户端\n");
                int j = 1;
                for (; j < maxsize; ++j)
                {
                    if (peerfd[j].fd < 0)
                    {
                        peerfd[j].fd = new_sock;
                        break;
                    }
                }
                if (j == maxsize)
                {
                    printf("客户端太多...\n");
                    close(new_sock);
                }
                peerfd[j].events = POLLIN;
                if (j + 1 > nfds)
                    nfds = j + 1;
            }
            for (i = 1; i < nfds; ++i)
            {
                if (peerfd[i].revents & POLLIN)
                {
                    printf("读取准备就绪\n");
                    char buf[1024];
                    ssize_t s = read(peerfd[i].fd, buf, sizeof(buf) - 1);
                    if (s > 0)
                    {
                        buf[s] = 0;
                        printf("客户端说：%s", buf);
                        fflush(stdout);
                        peerfd[i].events = POLLOUT;
                    }
                    else if (s <= 0)
                    {
                        close(peerfd[i].fd);
                        peerfd[i].fd = -1;
                    }
                    else
                    {
                        // 处理错误
                    }
                }
                else if (peerfd[i].revents & POLLOUT)
                {
                    char *msg = "HTTP/1.0 200 OK \
                                         <\r\n\r\n<html><h1> \
                                         yingying beautiful \
                                         </h1></html>\r\n";
                    write(peerfd[i].fd, msg, strlen(msg));
                    close(peerfd[i].fd);
                    peerfd[i].fd = -1;
                }
                else
                {
                    // 处理其他事件
                }
            }
        }
        break;
        }
    }
    return 0;
}
