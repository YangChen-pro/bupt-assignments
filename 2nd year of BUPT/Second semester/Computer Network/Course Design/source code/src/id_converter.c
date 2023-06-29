#include "..\include\id_converter.h"
#include <stdlib.h>
#include <string.h>

#define MAX_CLIENTS 65536  // 最大客户端数量
unsigned short index = 0;  // 当前映射的位置
KeyValue map[MAX_CLIENTS]; // 用于存储id和clientAddr的映射
int used[MAX_CLIENTS];

// 用于存储id和clientAddr的映射
int trans_port_id(unsigned short id, struct sockaddr_in clientAddr)
{
    int temp = index;
    while (used[index] == 1) // 找到一个未使用的位置
    {
        index = (index + 1) % MAX_CLIENTS;
        if (index == temp) // 已经遍历了所有位置
            return temp + 1;
    }

    map[index].value = index; // 保存映射
    map[index].key.id = id;
    map[index].key.clientAddr = clientAddr;
    used[index] = 1; // 标记为已使用
    return index;    // 返回映射的值
}

// 查找原始id
unsigned short find_id(unsigned Value)
{
    return map[Value].key.id;
}

// 查找原始clientAddr
struct sockaddr_in find_clientAddr(unsigned Value)
{
    return map[Value].key.clientAddr;
}

// 移除映射
void remove_id(unsigned Value)
{
    used[Value] = 0;
}