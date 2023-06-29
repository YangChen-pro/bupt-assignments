#ifndef ID_CONVERTER_H
#define ID_CONVERTER_H

#include <winsock2.h>

typedef struct
{
    unsigned short id;
    struct sockaddr_in clientAddr;
} Key; // 用于存储id和clientAddr的映射的键

typedef struct
{
    Key key;
    unsigned short value;
} KeyValue; // 用于存储id和clientAddr的映射的键值对

// 用于存储id和clientAddr的映射
int trans_port_id(unsigned short id, struct sockaddr_in clientAddr);

// 查找原始id
unsigned short find_id(unsigned Value);

// 查找原始clientAddr
struct sockaddr_in find_clientAddr(unsigned Value);

// 移除映射
void remove_id(unsigned Value);

#endif /* ID_CONVERTER_H */