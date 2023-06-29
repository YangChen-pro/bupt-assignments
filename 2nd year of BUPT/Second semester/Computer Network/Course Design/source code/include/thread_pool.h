#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <stdbool.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <stdbool.h>
#include "dns_relay_server.h"

#define MAX_THREADS 40 // 线程池大小

// 结构体,用于传递给线程的参数
struct ThreadParam
{
    struct Trie *trie;
    struct Cache *cache;
    int sock;
    struct sockaddr_in clientAddr;
    int clientAddrLen;
};

// 线程池结构体
struct ThreadPool
{
    struct ThreadParam *params[MAX_THREADS]; // 线程池
    int count;                 // 线程池中空闲线程的数量
};

// 初始化线程池和等待队列
void init_thread_pool(struct ThreadPool *pool);

// 销毁线程池和等待队列
void destroy_thread_pool(struct ThreadPool *pool);

// 添加DNS请求到线程池或等待队列中
void add_to_pool(struct ThreadPool *pool, struct ThreadParam *param);

// 线程入口函数，用于处理DNS请求
unsigned __stdcall threadProc(void *pParam);

#endif