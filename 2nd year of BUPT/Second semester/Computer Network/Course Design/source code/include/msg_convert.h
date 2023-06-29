#ifndef MSG_CONVERT_H
#define MSG_CONVERT_H

#include "dns_msg.h"

// 字节流转换为dns报文结构体
Dns_Msg *bytestream_to_dnsmsg(const unsigned char *bytestream, unsigned short *offset);

// dns报文结构体转换为字节流
unsigned char *dnsmsg_to_bytestream(const Dns_Msg *msg);

// 从字节流中提取出header的内容
void getHeader(Dns_Header *header, const unsigned char *bytestream);

// 获取域名
void getName(unsigned char *qname, const unsigned char *bytestream, unsigned short *offset);

// 从字节流中提取出question的内容
void getQuestion(Dns_Question *quesiton, const unsigned char *bytestream, unsigned short *offset);

// 从字节流中提取RR的内容
void getRR(Dns_RR *RR, const unsigned char *bytestream, unsigned short *offset);

// 将header填入字节流
void putHeader(const Dns_Header *header, unsigned char *bytestream);

// 将question填入字节流
void putQuestion(const Dns_Question *que, unsigned char *bytestream, unsigned short *offset);

// 将RR填入字节流
void putRR(const Dns_RR *rr, unsigned char *bytestream, unsigned short *offset);

// 获得点分十进制形式的IPv4地址
void transIPv4(unsigned char *original, unsigned char *IPv4);

// 获得冒分十六进制形式的IPv6地址
void transIPv6(unsigned char *original, unsigned char *IPv6);

// 获得xx.xx.xx形式的域名
void transDN(unsigned char *original, unsigned char *DN);

// 释放dns报文结构体
void releaseMsg(Dns_Msg *msg);

#endif // MSG_CONVERT_H