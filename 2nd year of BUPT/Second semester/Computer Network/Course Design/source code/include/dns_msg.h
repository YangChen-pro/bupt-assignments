#ifndef DNS_MSG_H
#define DNS_MSG_H

#define UDP_MAX 512

// QR字段的值定义
#define HEADER_QR_QUERY 0
#define HEADER_QR_ANSWER 1

// OPCODE字段的值定义
#define HEADER_OPCODE_QUERY 0
#define HEADER_OPCODE_IQUERY 1
#define HEADER_OPCODE_STATUS 2

// RCODE字段的值定义
#define HEADER_RCODE_NO_ERROR 0
#define HEADER_RCODE_NAME_ERROR 3

// TYPE字段的值定义
#define TYPE_A 1
#define TYPE_NS 2
#define TYPE_CNAME 5
#define TYPE_SOA 6
#define TYPE_PTR 12
#define TYPE_HINFO 13
#define TYPE_MINFO 14
#define TYPE_MX 15
#define TYPE_TXT 16
#define TYPE_AAAA 28

// CLASS字段的值定义
#define CLASS_IN 1
#define CLASS_NOT 254
#define CLASS_ALL 255

// DNS报文Header部分
typedef struct
{
    unsigned short id;
    unsigned char qr : 1;
    unsigned char opcode : 4;
    unsigned char aa : 1;
    unsigned char tc : 1;
    unsigned char rd : 1;
    unsigned char ra : 1;
    unsigned char z : 3;
    unsigned char rcode : 4;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
} Dns_Header;

// DNS报文Question部分
typedef struct Question
{
    unsigned char *qname;
    unsigned short qtype;
    unsigned short qclass;
    struct Question *next;
} Dns_Question;

// Resource Record
typedef struct RR
{
    unsigned char *name;
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short rdlength;
    unsigned char *rdata;
    struct RR *next;
} Dns_RR;

// DNS报文
typedef struct
{
    Dns_Header *header;
    Dns_Question *question;
    Dns_RR *RRs;
} Dns_Msg;

#endif // DNS_MSG_H