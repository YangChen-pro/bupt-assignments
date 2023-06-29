#ifndef _TRIE_H_
#define _TRIE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_NODE 1000001
#define MAX_ALPHABET 26 + 10 + 2

struct Node // 域名节点
{
    char domain[512]; // 域名
    struct Node *next;
};

struct Trie // Trie树
{
    int tree[MAX_NODE][MAX_ALPHABET]; // 字典树
    int prefix[MAX_NODE];             // 前缀
    bool isEnd[MAX_NODE];             // 是否是单词结尾
    int size;                         // 总节点数
    unsigned char toIp[MAX_NODE][4];  // IP地址
};

// 初始化Trie树
void initTrie(struct Trie *trie);

// 从dnsrelay.txt文件中读取域名和IP地址,并插入到Trie树中
void loadLocalTable(struct Trie *trie);

// 将域名简化,即将域名中的大写字母转换成小写字母
void simplifyDomain(char domain[]);

// 将域名插入到Trie树中
void insertNode(struct Trie *trie, const char domain[], unsigned char ipAddr[4]);

// 删除域名
void deleteNode(struct Trie *trie, const unsigned char domain[]);

// 查找域名
int findNode(struct Trie *trie, const unsigned char domain[]);

#endif // _TRIE_H_