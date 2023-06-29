#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#include "dns_msg.h"

// 输出调试信息
void debug(Dns_Msg *msg);

// 输出resource record
void RRInfo(Dns_RR *rr);

// 输出16进制字节流
void bytestreamInfo(unsigned char *bytestream);

// 打印程序执行时间
void printTime();

#endif // DEBUG_INFO_H