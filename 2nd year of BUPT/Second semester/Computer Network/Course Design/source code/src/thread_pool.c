#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <stdbool.h>
#include "..\include\thread_pool.h"

extern CRITICAL_SECTION threadPoolCS; // 线程池临界区
extern HANDLE semaphore;              // 信号量，用于线程池和等待队列之间的同步

// 初始化线程池和等待队列
void init_thread_pool(struct ThreadPool *pool)
{
    // 初始化线程池
    pool->count = MAX_THREADS; //
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pool->params[i] = (struct ThreadParam *)malloc(sizeof(struct ThreadParam));
        pool->params[i]->sock = (int)INVALID_SOCKET;
        pool->params[i]->trie = NULL;
        pool->params[i]->cache = NULL;
    }
    InitializeCriticalSection(&threadPoolCS);                          // 初始化临界区
    semaphore = CreateSemaphore(NULL, MAX_THREADS, MAX_THREADS, NULL); // 初始化信号量
}

// 销毁线程池和等待队列
void destroy_thread_pool(struct ThreadPool *pool)
{
    for (int i = 0; i < pool->count; i++) // 释放线程池中的参数
    {
        free(pool->params[i]);
    }
    DeleteCriticalSection(&threadPoolCS); // 删除临界区
    CloseHandle(semaphore);               // 关闭信号量
}

// 添加DNS请求到线程池或等待队列中
void add_to_pool(struct ThreadPool *pool, struct ThreadParam *param)
{
    EnterCriticalSection(&threadPoolCS); // 进入临界区
    if (pool->count < MAX_THREADS)       // 如果线程池中有空闲线程
    {
        pool->params[pool->count++] = param;  // 将参数添加到线程池中
        ReleaseSemaphore(semaphore, 1, NULL); // 释放信号量
    }
    LeaveCriticalSection(&threadPoolCS); // 离开临界区
}

// 线程入口函数，用于处理DNS请求
unsigned __stdcall threadProc(void *pParam)
{
    struct ThreadParam *param = (struct ThreadParam *)pParam;                      // 获取参数
    handle_dns_request(param->trie, param->cache, param->sock, param->clientAddr); // 处理DNS请求
    param->trie = NULL;                                                            // 释放参数
    param->cache = NULL;
    param->sock = (int)INVALID_SOCKET;
    return 0;
}