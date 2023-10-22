#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

// Token类，用于存储token的类型、行号、列号、值和错误
class Token
{
public:
    Token(); // 默认构造函数
    Token(std::string type, unsigned line, unsigned column, std::string value);
    ~Token(); // 析构函数
    std::string getType(); // 获取类型
    void setType(std::string type); // 设置类型
    void setValue(std::string value); // 设置token
    void setError(std::string error); // 设置错误
    friend std::ostream& operator<<(std::ostream& os, const Token& token); // 重载输出流
private:
    std::string type; // 类型
    unsigned line; // 行号
    unsigned column; // 列号
    std::string value; // 值
    std::string error; // 错误
};


#endif // TOKEN_H