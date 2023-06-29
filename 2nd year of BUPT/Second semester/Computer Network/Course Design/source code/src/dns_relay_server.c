#include "..\include\dns_relay_server.h"

#define MAX_DNS_PACKET_SIZE 1024 // DNS请求报文最大长度

extern int cmdOption;

// 通过线程池并发处理DNS请求
void handle_dns_request(struct Trie *trie, struct Cache *cache, SOCKET sock, struct sockaddr_in clientAddr)
{
    unsigned short offset = 0;
    unsigned char buf[MAX_DNS_PACKET_SIZE]; // 收到的DNS请求字节流
    // 将 char 缓冲区中的数据复制到 unsigned char 缓冲区中
    // memcpy(buf, recvBuf, len);
    // 解析DNS报文
    while (1)
    {
        // 接收来自用户端的DNS请求字节流
        int clientAddrLen = sizeof(clientAddr);
        int len = recvfrom(sock, (char *)buf, MAX_DNS_PACKET_SIZE, 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (len == SOCKET_ERROR)
        {
            printf("接收DNS请求失败.\n");
            continue;
        }
        Dns_Msg *msg = NULL;
        offset = 0;
        msg = bytestream_to_dnsmsg(buf, &offset);

        cmdOption == 1 ? bytestreamInfo(buf) : (void)0; // 打印DNS请求报文的字节流
        cmdOption == 1 ? debug(msg) : (void)0;     // 打印DNS请求报文的结构体

        if (msg->header->qr == 0 && msg->header->opcode == 0) // 只处理DNS请求报文
        {
            unsigned char domain[MAX_DOMAIN_LENGTH];
            transDN(msg->question->qname, domain); // 取出域名
            printf("收到来自用户端的DNS请求,域名为%s\n", domain);

            unsigned char *ipAddress = findIpAddress(trie, cache, domain); // 查找域名对应的IP地址
            if (ipAddress != NULL && ((ipAddress[4] == '\0' && msg->question->qtype == TYPE_A)||(ipAddress[4] != '\0' && msg->question->qtype == TYPE_AAAA)))                                         // 如果找到了,则发送DNS响应报文
            {
                printf("中继服务器查找成功,域名为%s,IP地址为%d.%d.%d.%d\n", domain, ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);
                addAnswer(msg, ipAddress, 60, msg->question->qtype);    // 将IP地址添加到DNS响应报文中
                send_dns_response(sock, msg, clientAddr); // 发送DNS响应报文
            }
            else // 如果没找到,则转发DNS请求报文给远程DNS服务器
            {
                printf("中继服务器查找失败,转发DNS请求报文给远程DNS服务器\n");

                // 将id和客户端绑定,产生新的id
                unsigned short newId = trans_port_id(msg->header->id, clientAddr);
                buf[0] = newId >> 8;
                buf[1] = newId;

                forward_dns_request(sock, buf, len); // 转发DNS请求报文给远程DNS服务器
            }
        }
        else if (msg->header->qr == 1) // 处理从远程DNS服务器返回的DNS响应报文
        {
            printf("收到来自远程DNS服务器的DNS响应报文\n");
            unsigned char domain[MAX_DOMAIN_LENGTH];
            unsigned char ipAddr[16];
            unsigned int ttl;
            unsigned short type;
            getDN_IP(buf, domain, ipAddr, &ttl, &type);
            addEntry(cache, domain, ipAddr, type, ttl);

            const struct sockaddr_in result = find_clientAddr(msg->header->id); // 通过id找到客户端地址
            unsigned short preId = find_id(msg->header->id);                    // 通过id找到原始id
            buf[0] = preId >> 8;
            buf[1] = preId;

            forward_dns_response(sock, buf, len, result, msg->header->id); // 转发DNS响应报文给用户端
        }
        else // 直接转发DNS报文给远程DNS服务器
        {
            unsigned short newId = trans_port_id(msg->header->id, clientAddr);
            buf[0] = newId >> 8;
            buf[1] = newId;

            forward_dns_request(sock, buf, len); // 转发DNS请求报文给远程DNS服务器
        }

        removeExpiredEntries(cache); // 每次处理完一个DNS请求,删除过期的缓存记录
        releaseMsg(msg);             // 释放DNS报文
    }
}

// 查找域名对应的IP地址
unsigned char *findIpAddress(struct Trie *trie, struct Cache *cache, unsigned char domain[MAX_DOMAIN_LENGTH])
{
    unsigned char ipAddr[16];
    unsigned char *ipAddress = NULL;

    // 先在缓存表中查找,找到返回
    if (findEntry(cache, domain, ipAddr, 1))
    {
        printf("在缓存表查找成功,域名为%s,IPv4地址为%d.%d.%d.%d\n", domain, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
        ipAddress = (unsigned char *)malloc(sizeof(unsigned char) * 4);
        memcpy(ipAddress, ipAddr, sizeof(unsigned char) * 4);
        ipAddress[4] = '\0';
    }
    else if (findEntry(cache, domain, ipAddr, 28))
    {
        printf("在缓存表查找成功,域名为%s,IPv6地址为%d.%d.%d.%d.%d.%d.%d.%d\n", domain, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3], ipAddr[4], ipAddr[5], ipAddr[6], ipAddr[7]);
        ipAddress = (unsigned char *)malloc(sizeof(unsigned char) * 16);
        memcpy(ipAddress, ipAddr, sizeof(unsigned char) * 16);
    }
    else
    {
        // 如果在本地表中找到了记录,将其添加到缓存表中
        int node = findNode(trie, domain);
        if (node != 0)
        {
            memcpy(ipAddr, trie->toIp[node], sizeof(ipAddr));
            addEntry(cache, domain, ipAddr, 1, CACHE_TTL);
            printf("在本地字典树查找成功,域名为%s,IP地址为%d.%d.%d.%d\n", domain, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
            ipAddress = (unsigned char *)malloc(sizeof(unsigned char) * 5);
            memcpy(ipAddress, ipAddr, sizeof(unsigned char) * 5);
            ipAddress[4] = '\0';
        }
        else // 本地表和缓存表都没有找到,需要转发到远程DNS服务器
        {
            printf("本地表和缓存表都未查找到域名%s,需要访问远程DNS服务器\n", domain);
            return NULL;
        }
    }
    return ipAddress;
}

// 向用户端发送DNS响应报文
void send_dns_response(int sock, Dns_Msg *msg, struct sockaddr_in clientAddr)
{
    unsigned char *bytestream = dnsmsg_to_bytestream(msg);
    int len;
    // 计算bytestream的长度
    Dns_Msg *temp = bytestream_to_dnsmsg(bytestream, (unsigned short *)(&len));

    int ret = sendto(sock, (char *)bytestream, len, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
    if (ret == SOCKET_ERROR)
        printf("send failed with error: %d\n", WSAGetLastError());
    else
    {
        cmdOption == 1 ? bytestreamInfo(bytestream) : (void)0; // 打印bytestream信息
        cmdOption == 1 ? debug(msg) : (void)0;            // 打印DNS报文信息
        printf("向用户端发送DNS响应报文成功\n");
    }
    releaseMsg(temp);
    free(bytestream);
}

// 转发DNS请求报文给远程DNS服务器
void forward_dns_request(int sock, unsigned char *buf, int len)
{
    struct sockaddr_in remoteAddr;
    memset(&remoteAddr, 0, sizeof(struct sockaddr_in));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = inet_addr("10.3.9.45"); // 远程DNS服务器地址
    remoteAddr.sin_port = htons(53);                     // DNS服务器端口号

    // 向远程DNS服务器发送DNS请求报文
    int ret = sendto(sock, (char *)buf, len, 0, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr_in));
    if (ret == SOCKET_ERROR)
        printf("sendto failed with error: %d\n", WSAGetLastError());
    else
    {
        cmdOption == 1 ? bytestreamInfo(buf) : (void)0; // 打印bytestream信息
        printf("向远程DNS服务器发送DNS请求报文成功\n");
    }
}

// 转发DNS响应报文给用户端
void forward_dns_response(int sock, unsigned char *buf, int len, struct sockaddr_in clientAddr, unsigned short id)
{
    int addrLen = sizeof(clientAddr);

    // 向用户端发送DNS响应报文
    int ret = sendto(sock, (char *)buf, len, 0, (struct sockaddr *)&clientAddr, addrLen);
    if (ret == SOCKET_ERROR)
        printf("sendto failed with error: %d\n", WSAGetLastError());
    else
    {
        cmdOption == 1 ? bytestreamInfo(buf) : (void)0; // 打印bytestream信息
        printf("向用户端发送DNS响应报文成功\n");
    }
}