#include "..\include\debug_info.h"
#include <stdio.h>
#include "..\include\msg_convert.h"
#include <time.h>

// 打印程序执行时间
void printTime()
{
    static clock_t start_time = 0;
    if (start_time == 0)
    {
        start_time = clock();
    }
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("%.3f:\n", elapsed_time);
}

// 输出resource record
void RRInfo(Dns_RR *rr)
{
    unsigned char name[UDP_MAX];
    transDN(rr->name, name);
    printf("NAME:%20s  ", name);
    printf("TYPE:%2d  ", rr->type);
    printf("CLASS:%2d\n", rr->_class);
    printf("TTL:%2d  ", rr->ttl);
    printf("RDLENGTH:%2d  ", rr->rdlength);
    if (rr->type == TYPE_A)
    {
        unsigned char IPv4[20];
        transIPv4(rr->rdata, IPv4);
        printf("RDATA:%20s", IPv4);
    }
    if (rr->type == TYPE_AAAA)
    {
        unsigned char IPv6[40];
        transIPv6(rr->rdata, IPv6);
        printf("RDATA:%20s", IPv6);
    }
    printf("\n");
}

// 输出调试信息
void debug(Dns_Msg *msg)
{
    printTime();
    printf("------------------------HEADER------------------------\n");
    printf("ID:%2d  ", msg->header->id);
    printf("QR:%2d  ", msg->header->qr);
    printf("Opcode:%2d  ", msg->header->opcode);
    printf("AA:%2d  ", msg->header->aa);
    printf("TC:%2d  ", msg->header->tc);
    printf("RD:%2d  ", msg->header->rd);
    printf("RA:%2d\n", msg->header->ra);
    printf("RCODE:%2d  ", msg->header->rcode);
    printf("QDCOUNT:%2d  ", msg->header->qdcount);
    printf("ANCOUNT:%2d  ", msg->header->ancount);
    printf("NSCOUNT:%2d  ", msg->header->nscount);
    printf("ARCOUNT:%2d\n", msg->header->arcount);

    Dns_Question *current_que = msg->question;
    printf("-----------------------QUESTION-----------------------\n");
    for (int i = 0; i < msg->header->qdcount; i++)
    {
        printf("QUESTION %d\n", i + 1);
        unsigned char name[512];
        transDN(current_que->qname, name);
        printf("QNAME:%20s  ", name);
        printf("QTYPE:%2d  ", current_que->qtype);
        printf("QCLASS:%2d\n", current_que->qclass);
        current_que = current_que->next;
    }

    Dns_RR *rr = msg->RRs;
    if (msg->header->ancount)
    {
        printf("------------------------ANSWER------------------------\n");
    }
    for (int i = 0; i < msg->header->ancount; i++)
    {
        printf("RR %d\n", i + 1);
        RRInfo(rr);
        rr = rr->next;
    }

    if (msg->header->nscount)
    {
        printf("-----------------------AUTHORITY----------------------\n");
    }
    for (int i = 0; i < msg->header->nscount; i++)
    {
        printf("RR %d\n", i + 1);
        RRInfo(rr);
        rr = rr->next;
    }

    if (msg->header->arcount)
    {
        printf("----------------------ADDITIONAL----------------------\n");
    }
    for (int i = 0; i < msg->header->arcount; i++)
    {
        printf("RR %d\n", i + 1);
        RRInfo(rr);
        rr = rr->next;
    }

    printf("------------------------------------------------------\n");
}

// 输出16进制字节流
void bytestreamInfo(unsigned char *bytestream)
{
    unsigned short offset;
    Dns_Msg *msg = bytestream_to_dnsmsg(bytestream, &offset);
    for (int i = 0; i < (int)(offset); i += 16)
    {
        printf("%04lx: ", i);
        for (int j = i; j < i + 16 && j < (int)(offset); j++)
        {
            printf("%02x ", (unsigned char)bytestream[j]);
        }
        printf("\n");
    }
}