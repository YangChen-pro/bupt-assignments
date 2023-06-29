#include "..\include\dns_function.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "..\include\msg_convert.h"

// 添加answer字段
void addAnswer(Dns_Msg *msg, const unsigned char *IP, unsigned int _ttl, unsigned short _type)
{
    // 不良网站拦截
    if (_type == TYPE_A && *(unsigned int *)(IP) == 0)
    {
        msg->header->rcode = 3;
    }

    // 设置header字段的值
    msg->header->qr = 1;
    msg->header->rd = 1;
    msg->header->ra = 1;
    msg->header->ancount++;

    // 为answer字段添加一个Resource Record
    Dns_RR *rr = msg->RRs;
    Dns_RR *prev = NULL;
    while (rr)
    {
        prev = rr;
        rr = rr->next;
    }
    if (prev)
    {
        rr = (Dns_RR *)malloc(sizeof(Dns_RR));
        if (!rr)
        {
            puts("动态分配内存失败");
            exit(1);
        }
        prev->next = rr;
        rr->next = NULL;
    }
    else
    {
        msg->RRs = (Dns_RR *)malloc(sizeof(Dns_RR));
        msg->RRs->next = NULL;
        rr = msg->RRs;
    }
    rr->name = (unsigned char *)malloc(sizeof(unsigned char) * UDP_MAX);
    memcpy(rr->name, msg->question->qname, strlen((char *)(msg->question->qname)) + 1);
    rr->type = _type;
    rr->_class = CLASS_IN;
    rr->ttl = _ttl;
    if (_type == TYPE_A)
    {
        rr->rdlength = 4; // IPv4
    }
    else
    {
        rr->rdlength = 16; // IPv6
    }
    rr->rdata = (unsigned char *)malloc(sizeof(unsigned char) * rr->rdlength);
    memcpy(rr->rdata, IP, rr->rdlength);
}

// 从外部DNS的回复报文中提取域名和IP地址
void getDN_IP(const unsigned char *bytestream, unsigned char *DN, unsigned char *IP, unsigned int *_ttl, unsigned short *_type)
{
    unsigned short offset;
    Dns_Msg *msg = bytestream_to_dnsmsg(bytestream, &offset);
    transDN(msg->question->qname, DN);
    Dns_RR *rr = msg->RRs;
    while (rr)
    {
        if (rr->type == TYPE_A || rr->type == TYPE_AAAA)
        {
            *_type = rr->type;
            memcpy(IP, rr->rdata, rr->rdlength);
            *_ttl = rr->ttl;
            break;
        }
        rr = rr->next;
    }
    releaseMsg(msg);
}