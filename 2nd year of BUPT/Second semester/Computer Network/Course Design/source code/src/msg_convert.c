#include "..\include\msg_convert.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// 从字节流中提取出header的内容
void getHeader(Dns_Header *header, const unsigned char *bytestream)
{
    header->id = ntohs(*(unsigned short *)bytestream);
    header->qr = (bytestream[2] >> 7) & 1;
    header->opcode = (bytestream[2] >> 3) & 0x0f;
    header->aa = (bytestream[2] >> 2) & 1;
    header->tc = (bytestream[2] >> 1) & 1;
    header->rd = (bytestream[2]) & 1;
    header->ra = (bytestream[3] >> 7) & 1;
    header->z = (bytestream[3] >> 4) & 0x07;
    header->rcode = (bytestream[3]) & 0x0f;
    header->qdcount = ntohs(*(unsigned short *)(bytestream + 4));
    header->ancount = ntohs(*(unsigned short *)(bytestream + 6));
    header->nscount = ntohs(*(unsigned short *)(bytestream + 8));
    header->arcount = ntohs(*(unsigned short *)(bytestream + 10));
}

// 获取域名
void getName(unsigned char *qname, const unsigned char *bytestream, unsigned short *offset)
{
    while (*(bytestream + *offset) != 0)
    {
        if (((*(bytestream + *offset) >> 6) & 3) == 3) // 压缩标签
        {
            unsigned short new_offset = ntohs(*(unsigned short *)(bytestream + *offset)) & 0x3fff;
            getName(qname, bytestream, &new_offset);
            (*offset) += 2;
            return;
        }
        *qname = *(bytestream + *offset);
        qname++;
        (*offset)++;
    }
    (*offset)++;
    *qname = '\0';
}

// 从字节流中提取出question的内容
void getQuestion(Dns_Question *quesiton, const unsigned char *bytestream, unsigned short *offset)
{
    quesiton->qname = (unsigned char *)malloc(sizeof(unsigned char) * UDP_MAX);
    if (!quesiton->qname)
    {
        puts("动态分配内存失败");
        exit(1);
    }
    getName(quesiton->qname, bytestream, offset);
    quesiton->qtype = ntohs(*(unsigned short *)(bytestream + *offset));
    (*offset) += 2;
    quesiton->qclass = ntohs(*(unsigned short *)(bytestream + *offset));
    (*offset) += 2;
}

// 从字节流中提取RR的内容
void getRR(Dns_RR *RR, const unsigned char *bytestream, unsigned short *offset)
{
    RR->name = (unsigned char *)malloc(sizeof(unsigned char) * UDP_MAX);
    if (!RR->name)
    {
        puts("动态分配内存失败");
        exit(1);
    }
    getName(RR->name, bytestream, offset);
    // printf("%d\n", *offset);
    RR->type = ntohs(*(unsigned short *)(bytestream + *offset));
    // printf("%d\n", RR->type);
    (*offset) += 2;
    RR->_class = ntohs(*(unsigned short *)(bytestream + *offset));
    (*offset) += 2;
    RR->ttl = ntohl(*(unsigned int *)(bytestream + *offset));
    (*offset) += 4;
    RR->rdlength = ntohs(*(unsigned short *)(bytestream + *offset));
    (*offset) += 2;
    RR->rdata = (unsigned char *)malloc(sizeof(unsigned char) * RR->rdlength + 1);
    memcpy(RR->rdata, bytestream + *offset, RR->rdlength);
    RR->rdata[RR->rdlength] = '\0';
    (*offset) += RR->rdlength;
}

// 字节流转换为dns报文结构体
Dns_Msg *bytestream_to_dnsmsg(const unsigned char *bytestream, unsigned short *offset)
{
    Dns_Msg *msg = (Dns_Msg *)malloc(sizeof(Dns_Msg));
    if (!msg)
    {
        puts("动态分配内存失败");
        exit(1);
    }

    // 转换header部分
    msg->header = (Dns_Header *)malloc(sizeof(Dns_Header));
    if (!msg->header)
    {
        puts("动态分配内存失败");
        exit(1);
    }
    getHeader(msg->header, bytestream);

    *offset = 12;
    // 转换question部分
    msg->question = NULL;
    Dns_Question *question_tail = NULL;
    for (int i = 0; i < msg->header->qdcount; i++)
    {
        Dns_Question *current = (Dns_Question *)malloc(sizeof(Dns_Question));
        if (!current)
        {
            puts("动态分配内存失败");
            exit(1);
        }
        if (!question_tail)
        {
            msg->question = current;
            current->next = NULL;
        }
        else
        {
            question_tail->next = current;
            current->next = NULL;
        }
        question_tail = current;
        getQuestion(current, bytestream, offset);
    }

    // 转换answer、authority、 additional部分
    unsigned short total_length = msg->header->ancount + msg->header->nscount + msg->header->arcount;
    msg->RRs = NULL;
    Dns_RR *RRs_tail = NULL;
    for (int i = 0; i < total_length; i++)
    {
        Dns_RR *current = (Dns_RR *)malloc(sizeof(Dns_RR));
        if (!current)
        {
            puts("动态分配内存失败");
            exit(1);
        }
        if (!RRs_tail)
        {
            msg->RRs = current;
            current->next = NULL;
        }
        else
        {
            RRs_tail->next = current;
            current->next = NULL;
        }
        RRs_tail = current;
        getRR(current, bytestream, offset);
    }

    return msg;
}

// 将header填入字节流
void putHeader(const Dns_Header *header, unsigned char *bytestream)
{
    bytestream[0] = header->id >> 8;
    bytestream[1] = header->id;
    bytestream[2] = 0;
    bytestream[2] |= header->qr << 7;
    bytestream[2] |= header->opcode << 3;
    bytestream[2] |= header->aa << 2;
    bytestream[2] |= header->tc << 1;
    bytestream[2] |= header->rd;
    bytestream[3] = 0;
    bytestream[3] |= header->ra << 7;
    bytestream[3] |= header->z << 4;
    bytestream[3] |= header->rcode;
    bytestream[4] = header->qdcount >> 8;
    bytestream[5] = header->qdcount;
    bytestream[6] = header->ancount >> 8;
    bytestream[7] = header->ancount;
    bytestream[8] = header->nscount >> 8;
    bytestream[9] = header->nscount;
    bytestream[10] = header->arcount >> 8;
    bytestream[11] = header->arcount;
}

// 将question填入字节流
void putQuestion(const Dns_Question *que, unsigned char *bytestream, unsigned short *offset)
{
    memcpy(bytestream + *offset, que->qname, strlen((char *)(que->qname)) + 1);
    (*offset) += strlen((char *)(que->qname)) + 1;
    *(bytestream + *offset) = que->qtype >> 8;
    (*offset)++;
    *(bytestream + *offset) = que->qtype;
    (*offset)++;
    *(bytestream + *offset) = que->qclass >> 8;
    (*offset)++;
    *(bytestream + *offset) = que->qclass;
    (*offset)++;
}

// 将RR填入字节流
void putRR(const Dns_RR *rr, unsigned char *bytestream, unsigned short *offset)
{
    memcpy(bytestream + *offset, rr->name, strlen((char *)(rr->name)) + 1);
    (*offset) += strlen((char *)(rr->name)) + 1;
    *(bytestream + *offset) = rr->type >> 8;
    (*offset)++;
    *(bytestream + *offset) = rr->type;
    (*offset)++;
    *(bytestream + *offset) = rr->_class >> 8;
    (*offset)++;
    *(bytestream + *offset) = rr->_class;
    (*offset)++;
    *(bytestream + *offset) = rr->ttl >> 24;
    (*offset)++;
    *(bytestream + *offset) = rr->ttl >> 16;
    (*offset)++;
    *(bytestream + *offset) = rr->ttl >> 8;
    (*offset)++;
    *(bytestream + *offset) = rr->ttl;
    (*offset)++;
    *(bytestream + *offset) = rr->rdlength >> 8;
    (*offset)++;
    *(bytestream + *offset) = rr->rdlength;
    (*offset)++;
    memcpy(bytestream + *offset, rr->rdata, rr->rdlength);
    (*offset) += rr->rdlength;
}

// dns报文结构体转换为字节流
unsigned char *dnsmsg_to_bytestream(const Dns_Msg *msg)
{
    unsigned char *bytestream = (unsigned char *)malloc(sizeof(unsigned char) * UDP_MAX);
    if (!bytestream)
    {
        puts("动态分配内存失败");
        exit(1);
    }

    // 转换header部分
    putHeader(msg->header, bytestream);

    // 转换question部分
    unsigned short offset = 12;
    Dns_Question *que = msg->question;
    while (que)
    {
        putQuestion(que, bytestream, &offset);
        que = que->next;
    }

    // 转换answer、authority、 additional部分
    Dns_RR *rr = msg->RRs;
    while (rr)
    {
        putRR(rr, bytestream, &offset);
        rr = rr->next;
    }

    return bytestream;
}

// 获得点分十进制形式的IPv4地址
void transIPv4(unsigned char *original, unsigned char *IPv4)
{
    sprintf((char *)(IPv4), "%d.%d.%d.%d", original[0], original[1], original[2], original[3]);
}

// 获得冒分十六进制形式的IPv6地址
void transIPv6(unsigned char *original, unsigned char *IPv6)
{
    sprintf((char *)(IPv6), "%x:%x:%x:%x:%x:%x:%x:%x", ntohs(*(unsigned short *)(original)), ntohs(*(unsigned short *)(original + 2)),
            ntohs(*(unsigned short *)(original + 4)), ntohs(*(unsigned short *)(original + 6)), ntohs(*(unsigned short *)(original + 8)),
            ntohs(*(unsigned short *)(original + 10)), ntohs(*(unsigned short *)(original + 12)), ntohs(*(unsigned short *)(original + 14)));
}

// 获得xx.xx.xx形式的域名
void transDN(unsigned char *original, unsigned char *DN)
{
    while (*original != 0)
    {
        unsigned short len = *original;
        original++;
        memcpy(DN, original, len);
        original += len;
        DN += len;
        *DN = '.';
        DN++;
    }
    *(DN - 1) = '\0';
}

// 释放dns报文结构体
void releaseMsg(Dns_Msg *msg)
{
    if (!msg)
        return;
    free(msg->header);
    Dns_Question *pQue = msg->question;
    while (pQue)
    {
        if (pQue->qname)
            free(pQue->qname);
        Dns_Question *temp = pQue;
        pQue = pQue->next;
        free(temp);
    }
    Dns_RR *pRR = msg->RRs;
    while (pRR)
    {
        if (pRR->name)
            free(pRR->name);
        if (pRR->rdata)
            free(pRR->rdata);
        Dns_RR *temp = pRR;
        pRR = pRR->next;
        free(temp);
    }
    free(msg);
}