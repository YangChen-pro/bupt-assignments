#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "..\include\cache.h"

// 初始化缓存
void initCache(struct Cache *cache)
{
    memset(cache->table, 0, sizeof(cache->table)); // 将哈希表清零
    cache->head = NULL; // 链表头指针置空
    cache->tail = NULL; // 链表尾指针置空
}

// 计算哈希值
unsigned int hashCode(const unsigned char *domain)
{
    uint32_t hashValue = MurmurHash(domain, strlen((const char *)domain), 0) % CACHE_SIZE; // 调用 MurmurHash 算法计算哈希值
    return (unsigned int)hashValue; // 返回哈希值
}

// 查找缓存项
// 如果缓存项存在且未过期，将其移动到链表头部，并设置 IP 地址，返回 1
// 如果缓存项不存在或已过期，删除其对应的哈希表和链表项，返回 0
int findEntry(struct Cache *cache, const unsigned char *domain, unsigned char *ipAddr, int ipVersion)
{
    size_t domainLen = strlen((const char *)domain); // 获取域名长度
    unsigned int hash = hashCode(domain); // 获取哈希值
    time_t now = time(NULL); // 获取当前时间

    if (cache->head != cache->tail)
    {
        removeExpiredEntries(cache);
        return 0;
    }

    // 遍历哈希表
    struct CacheEntry *entry = cache->table[hash];
    while (entry != NULL)
    {
        if (entry->expireTime < 0) // 如果已经超时
        {
            cache->table[hash] = NULL;
            return 0;
        }
        // 如果找到了对应的域名
        if (strcmp((const char *)entry->domain, (const char *)domain) == 0)
        {
            // 如果缓存项未过期
            if (entry->expireTime >= now)
            {
                // LRU策略，将命中的缓存移动到链表头部
                if (entry->prev != NULL) // 如果不是链表头部
                    entry->prev->next = entry->next;
                else // 如果是链表头部
                    cache->head = entry->next;
                if (entry->next != NULL) // 如果不是链表尾部
                    entry->next->prev = entry->prev;
                else // 如果是链表尾部
                    cache->tail = entry->prev;
                
                entry->expireTime = now + 60; // 更新过期时间
                entry->prev = NULL;
                entry->next = cache->head;
                cache->head = entry;

                // 设置 IP 地址
                if (ipVersion == 1)
                    memcpy(ipAddr, entry->ipAddr, sizeof(entry->ipAddr));
                else
                    memcpy(ipAddr, entry->ipAddr6, sizeof(entry->ipAddr6));

                return 1; // 返回成功
            }
            else // 如果缓存项已过期
            {
                // 缓存过期，删除缓存
                if (entry->prev != NULL) // 如果不是链表头部
                    entry->prev->next = entry->next;
                else // 如果是链表头部
                    cache->table[hash] = entry->next;
                if (entry->next != NULL) // 如果不是链表尾部
                    entry->next->prev = entry->prev;
                else // 如果是链表尾部
                    cache->tail = entry->prev;
                free(entry); // 释放内存
                return 0; // 返回失败
            }
        }
        entry = entry->next; // 移动指针
    }

    return 0; // 返回失败
}

// 添加缓存项
void addEntry(struct Cache *cache, const unsigned char *domain, const unsigned char *ipAddr, int ipVersion, time_t ttl)
{
    size_t domainLen = strlen((const char *)domain); // 获取域名长度
    unsigned int hash = hashCode(domain); // 获取哈希值
    time_t now = time(NULL); // 获取当前时间

    // 新建缓存项
    struct CacheEntry *entry = (struct CacheEntry *)malloc(sizeof(struct CacheEntry));
    if (entry == NULL)
        return;

    // 复制域名
    memcpy(entry->domain, domain, domainLen + 1);
    entry->domain[domainLen] = '\0';

    // 复制IP地址
    unsigned char *ip = (ipVersion == 1) ? entry->ipAddr : entry->ipAddr6;
    memcpy(ip, ipAddr, (ipVersion == 1) ? sizeof(entry->ipAddr) : sizeof(entry->ipAddr6));

    // 设置过期时间
    entry->expireTime = now + ttl;

    // 添加到链表头部
    entry->prev = NULL;
    entry->next = cache->head;
    if (cache->head != NULL) // 如果链表不为空
        cache->head->prev = entry;
    cache->head = entry; // 更新链表头指针
    if (cache->tail == NULL) // 如果链表为空
        cache->tail = entry;

    // 添加到哈希表
    entry->prev = NULL;
    entry->next = cache->table[hash];
    if (cache->table[hash] != NULL)
        cache->table[hash]->prev = entry;
    cache->table[hash] = entry;

    // 删除过期的缓存项
    removeExpiredEntries(cache);
}

// 删除过期的缓存项
void removeExpiredEntries(struct Cache *cache)
{
    time_t now = time(NULL); // 获取当前时间
    // 从链表尾部开始删除过期的缓存项
    struct CacheEntry *entry = cache->tail; 
    while (entry != NULL && entry->expireTime < now)
    {
        struct CacheEntry *pre = entry->prev;
        
        // 从链表中删除过期的缓存项
        if (pre != NULL)
        {
            pre->next = NULL;
            cache->tail = pre;
        }            
        else
        {
            cache->tail = NULL;
            cache->head = NULL;
        }
        free(entry); // 释放内存
        entry = cache->tail;
    }
}

// 清空缓存
void clearCache(struct Cache *cache)
{
    // 遍历哈希表，释放缓存项内存
    for (int i = 0; i < CACHE_SIZE; i++)
    {
        struct CacheEntry *entry = cache->table[i];
        while (entry != NULL)
        {
            struct CacheEntry *next = entry->next;
            free(entry);
            entry = next;
        }
        cache->table[i] = NULL;
    }
    cache->head = NULL; // 链表头指针置空
    cache->tail = NULL; // 链表尾指针置空
}