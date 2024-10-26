/**
 * @file epoll.c
 * @brief 该程序演示了如何使用epoll()函数处理多个套接字连接。
 * 
 * 该程序创建一个TCP服务器，监听指定的IP地址和端口上的传入连接。
 * 它使用epoll()函数监视多个文件描述符上的读事件。
 * 当有新的客户端连接时，它接受连接并将新的套接字描述符添加到epoll集合中。
 * 然后从连接的客户端读取数据并发送响应。
 * 如果客户端断开连接，则关闭相应的套接字描述符并从epoll集合中删除。
 * 
 * 用法：./epoll [local_ip] [local_port]
 * 
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数的数组。
 * @return 如果程序成功执行，则返回0；否则返回错误代码。
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

/**
 * 打印程序的用法。
 *
 * @param proc 程序的名称。
 */
static void Usage(const char *proc)
{
    printf("%s [local_ip] [local_port]\n", proc);
}

/**
 * 创建并设置一个套接字，用于监听指定的IP地址和端口。
 *
 * @param _ip 要绑定套接字的本地IP地址。
 * @param _port 要绑定套接字的本地端口。
 * @return 创建的套接字文件描述符。
 */
int start_up(const char *_ip, int _port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(2);
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = inet_addr(_ip);
    if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        perror("bind");
        exit(3);
    }
    if (listen(sock, 10) < 0)
    {
        perror("listen");
        exit(4);
    }
    return sock;
}

/**
 * 程序的主函数。
 *
 * @param argc 命令行参数的数量。
 * @param argv 命令行参数的数组。
 * @return 程序的退出状态。
 */
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }
    int sock = start_up(argv[1], atoi(argv[2]));
    int epollfd = epoll_create(256);
    if (epollfd < 0)
    {
        perror("epoll_create");
        return 5;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) < 0)
    {
        perror("epoll_ctl");
        return 6;
    }
    int evnums = 0; // epoll_wait返回值
    struct epoll_event evs[64];
    int timeout = -1;
    while (1)
    {
        switch (evnums = epoll_wait(epollfd, evs, 64, timeout))
        {
        case 0:
            printf("超时...\n");
            break;
        case -1:
            perror("epoll_wait");
            break;
        default:
        {
            int i = 0;
            for (; i < evnums; ++i)
            {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                if (evs[i].data.fd == sock && evs[i].events & EPOLLIN)
                {
                    int new_sock = accept(sock,
                                          (struct sockaddr *)&client, &len);
                    if (new_sock < 0)
                    {
                        perror("accept");
                        continue;
                    } // 如果accept失败
                    else
                    {
                        printf("获得一个新的客户端[%s]\n",
                               inet_ntoa(client.sin_addr));
                        ev.data.fd = new_sock;
                        ev.events = EPOLLIN;
                        epoll_ctl(epollfd, EPOLL_CTL_ADD,
                                  new_sock, &ev);
                    } // accept成功

                } // 如果fd == sock
                else if (evs[i].data.fd != sock &&
                         evs[i].events & EPOLLIN)
                {
                    char buf[1024];
                    ssize_t s = read(evs[i].data.fd, buf, sizeof(buf) - 1);
                    if (s > 0)
                    {
                        buf[s] = 0;
                        printf("客户端说#%s", buf);
                        ev.data.fd = evs[i].data.fd;
                        ev.events = EPOLLOUT;
                        epoll_ctl(epollfd, EPOLL_CTL_MOD,
                                  evs[i].data.fd, &ev);
                    } // 如果s > 0
                    else
                    {
                        close(evs[i].data.fd);
                        epoll_ctl(epollfd, EPOLL_CTL_DEL,
                                  evs[i].data.fd, NULL);
                    }
                } // 如果fd != sock
                else if (evs[i].data.fd != sock && evs[i].events & EPOLLOUT)
                {
                    char *msg = "HTTP/1.0 200 OK <\r\n\r\n<html><h1>yingying beautiful </h1></html>\r\n";
                    write(evs[i].data.fd, msg, strlen(msg));
                    close(evs[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL,
                              evs[i].data.fd, NULL);
                } // EPOLLOUT
                else
                {
                }
            } // for
        }     // default
        break;
        } // switch
    }     // while
    return 0;
}
