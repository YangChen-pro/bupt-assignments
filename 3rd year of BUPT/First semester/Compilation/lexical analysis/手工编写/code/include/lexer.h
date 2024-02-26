#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include <cassert>
#include "token.h"

// 判断是否为字母或下划线
bool isLetter_(const char c);

// 判断是否为字母、下划线或数字
bool isLetterOrDigit(const char c);

// 判断是否为空白字符
bool isWhitespace(const char c);

// 判断是否为'e'或'E'
bool isE(const char c);

// 判断是否为'+'或'-'
bool isSign(const char c);

class Lexer
{
public:
    Lexer(std::string fileName); // 构造函数
    ~Lexer();                    // 析构函数
    bool lexer();                // 词法分析
    char eatChar();              // 读取一个字符
    char peekChar();             // 预读一个字符
    void printStat();            // 打印统计信息

private:
    std::string fileName; // 文件名
    std::ifstream file;   // 文件流
    unsigned countChar;   // 当前字符
    unsigned countLine;   // 当前行
    unsigned countColumn; // 当前列
    std::string buffer;   // 缓冲区
#define TOKEN_TYPE(type) unsigned count_##type;
#include "token_type.h"
#undef TOKEN_TYPE
    void getNextIdentifier(Token &token); // 获取下一个标识符
    void getNextNumerical(Token &token);  // 获取下一个数值
    void getCharConstant(Token &token);   // 获取下一个字符常量
    void getStringLiteral(Token &token);  // 获取下一个字符串字面量
};

#endif // LEXER_H