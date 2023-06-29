#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>
#include <stdbool.h>
#include "..\include\dns_relay_server.h"
#include "..\include\thread_pool.h"

#define DEFAULT_PORT 53          // DNS服务器默认端口号
#define MAX_DNS_PACKET_SIZE 1024 // DNS请求报文最大长度
CRITICAL_SECTION threadPoolCS; // 线程池临界区
HANDLE semaphore;              // 信号量，用于线程池和等待队列之间的同步

int cmdOption = 0;

int main(int argc, char *argv[])
{
    // 默认监听端口号
    int port = DEFAULT_PORT;

    // 处理命令行参数
    if (argc > 1 && strcmp(argv[2], "-d") == 0) {
        cmdOption = 1; // 调试模式启动
        printf("调试模式启动\n");
    }

    // 初始化线程池和等待队列
    struct ThreadPool threadPool;
    init_thread_pool(&threadPool);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("初始化Winsock失败.\n");
        return 1;
    }

    // 创建socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        printf("创建监听socket失败.\n");
        return 1;
    }

    // 绑定端口
    if (argc > 1)
        port = atoi(argv[1]);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("绑定监听socket到端口%d失败.\n", port);
        closesocket(sock);
        return 1;
    }

    printf("DNS中继服务器正在监听端口%d.\n", port);

    // 创建字典树和缓存表
    struct Trie *trie = (struct Trie *)malloc(sizeof(struct Trie));
    initTrie(trie);
    loadLocalTable(trie);

    struct Cache *cache = (struct Cache *)malloc(sizeof(struct Cache));
    initCache(cache);

    while (true)
    {
        // 接收DNS请求
        char buf[MAX_DNS_PACKET_SIZE];
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        int recvLen = recvfrom(sock, buf, MAX_DNS_PACKET_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (recvLen == SOCKET_ERROR)
        {
            printf("接收DNS请求失败.\n");
            continue;
        }

        printf("接收来自%s:%d的DNS请求，长度为%d字节.\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), recvLen);

        // 从线程池中取出一个线程，如果线程池已满，则将请求放入等待队列中
        WaitForSingleObject(semaphore, INFINITE);
        EnterCriticalSection(&threadPoolCS);
        struct ThreadParam *param = NULL;
        if (threadPool.count > 0)
        {
            param = threadPool.params[--threadPool.count];
        }
        LeaveCriticalSection(&threadPoolCS);

        if (param->trie == NULL)
        {
            // 如果线程池中有空闲线程，则将请求分配给该线程处理
            param->trie = trie;
            param->cache = cache;
            param->sock = sock;
            param->clientAddr = clientAddr;
            param->clientAddrLen = clientAddrLen;
            _beginthreadex(NULL, 0, threadProc, param, 0, NULL);
        }
        else
        {
            // 如果线程池已满，则将请求放入等待队列中
            param = (struct ThreadParam *)malloc(sizeof(struct ThreadParam));
            param->trie = trie;
            param->cache = cache;
            param->sock = sock;
            param->clientAddr = clientAddr;
            param->clientAddrLen = clientAddrLen;

            EnterCriticalSection(&threadPoolCS);
            add_to_pool(&threadPool, param);
            LeaveCriticalSection(&threadPoolCS);
        }
    }

    // 关闭socket和清理资源
    closesocket(sock);
    free(trie);
    clearCache(cache);
    free(cache);
    // 销毁线程池和等待队列
    destroy_thread_pool(&threadPool);
    WSACleanup();

    return 0;
}