#include "..\include\lexer.h"

// 读取一个字符
char Lexer::eatChar()
{
    char c = file.get();
    if (c >= 0)
    {
        buffer += c;
    }
    else
    {
        return 0;
    }
    if (c == '\n')
    {
        countLine++;
        countColumn = 1;
    }
    else
    {
        countColumn++;
    }
    countChar++;
    return c;
}

// 预读一个字符
char Lexer::peekChar()
{
    return file.peek();
}

// 是否为字母或下划线
bool isLetter_(const char c)
{
    return isalpha(c) || c == '_';
}

// 是否为字母、下划线或数字
bool isLetterOrDigit(const char c)
{
    return isalpha(c) || c == '_' || isdigit(c);
}

// 是否为空白字符
bool isWhitespace(const char c)
{
    return c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\n' || c <= 0;
}

// 是否为'e'或'E'
bool isE(const char c)
{
    return c == 'e' || c == 'E';
}

// 是否为'+'或'-'
bool isSign(const char c)
{
    return c == '+' || c == '-';
}

// 构造函数
Lexer::Lexer(std::string fileName) : fileName(fileName),
                                     countChar(0),
                                     countLine(1),
                                     countColumn(1)
{
    file.open(fileName);
    assert(file.is_open()); // 文件打开失败则报错
    // 初始化统计信息
#define TOKEN_TYPE(type) count_##type = 0;
#include "..\include\token_type.h"
#undef TOKEN_TYPE
    buffer.clear(); // 清空缓冲区
}

// 析构函数
Lexer::~Lexer()
{
    file.close();
}

// 读取标识符
void Lexer::getNextIdentifier(Token &token)
{
    token.setType("Identifier");
    eatChar(); // 读取第一个字符
    do
    {
        char c = peekChar();
        if (isLetterOrDigit(c))
        {
            eatChar();
        }
        else
        {
            break;
        }
    } while (true);
// 判断是否为关键字
#define KEYWORD(keyword)    \
    if (buffer == #keyword) \
        token.setType("Keyword"), count_Keyword++;
#include "..\include\keyword.h"
#undef KEYWORD

    if (token.getType() == "Identifier")
    {
        count_Identifier++;
    }
    token.setValue(buffer); // 设置标识符的值
}

// 读取数值
void Lexer::getNextNumerical(Token &token)
{
    token.setType("Numerical_Constant");
    eatChar();
    // 识别数值常量的自动机
    unsigned state = 2;
    char c;
    do
    {
        switch (state)
        {
        case 0:
            break;
        case 1:
            c = peekChar();
            if (isLetter_(c))
            {
                state = 1;
                eatChar();
            }
            else
            {
                state = 0;
            }
            break;
        case 2:
            c = peekChar();
            if (isdigit(c))
            {
                state = 2;
                eatChar();
            }
            else if (c == '.')
            {
                state = 3;
                eatChar();
            }
            else if (isE(c))
            {
                state = 5;
                eatChar();
            }
            else if (isLetter_(c))
            {
                token.setType("Error");
                token.setError("illegal name");
                count_Error++;
                state = 1;
                eatChar();
            }
            else
            {
                state = 0;
            }
            break;
        case 3:
            c = peekChar();
            // 识别小数或省略小数部分
            if (isdigit(c)) 
            {
                state = 4;
                eatChar();
            }
            else if (isE(c))
            {
                state = 5;
                eatChar();
            }
            else if (c == ';')
            {
                state = 0;
            }
            else
            {
                token.setType("Error");
                token.setError("illegal name");
                count_Error++;
                state = 1;
                eatChar();
            }
            break;
        case 4:
            c = peekChar();
            if (isdigit(c))
            {
                state = 4;
                eatChar();
            }
            else if (isE(c))
            {
                state = 5;
                eatChar();
            }
            else
            {
                state = 0;
            }
            break;
        case 5:
            c = peekChar();
            if (isdigit(c))
            {
                state = 7;
                eatChar();
            }
            else if (isSign(c))
            {
                state = 6;
                eatChar();
            }
            else
            {
                state = 0;
                token.setType("Error");
                token.setError("Invalid exponent in numerical constant: exponent symbol without following digits");
                count_Error++;
            }
            break;
        case 6:
            c = peekChar();
            if (isdigit(c))
            {
                state = 7;
                eatChar();
            }
            else
            {
                state = 0;
            }
            break;
        case 7:
            c = peekChar();
            if (isdigit(c))
            {
                state = 7;
                eatChar();
            }
            else
            {
                state = 0;
            }
            break;
        default:
            break;
        }
    } while (state != 0);
    token.setValue(buffer);
    if (token.getType() == "Numerical_Constant")
    {
        count_Numerical_Constant++;
    }
}

// 读取字符常量
void Lexer::getCharConstant(Token &token)
{
    token.setType("Char_Constant");
    eatChar(); // 读取第一个字符
    unsigned state = 1;
    char c;
    do
    {
        switch (state)
        {
        case 1:
            c = peekChar();
            if (c == '\\') // 转义字符
            {
                state = 3;
                eatChar();
            }
            else if (c == '\'') // 空字符
            {
                state = 0;
                eatChar();
            }
            else
            {
                state = 2;
                eatChar();
            }
            break;
        case 2:
            c = peekChar();
            if (c == '\'')
            {
                state = 0;
                eatChar();
            }
            else // 多字符字符常量
            {
                state = 0;
                token.setType("Error");
                token.setError("multi-character character constant or unclosed character constant");
                count_Error++;
            }
            break;
        case 3:
            c = peekChar();
            if (c == 'a' || c == 'b' || c == 'f' || c == 'n' ||
                c == 'r' || c == 't' || c == 'v' || c == '\\' ||
                c == '\'' || c == '\"' || c == '\?')
            {
                state = 2;
                eatChar();
            }
            else // 非法转义字符
            {
                state = 0;
                token.setType("Error");
                token.setError("Invalid escape sequence in character constant");
                count_Error++;
            }
            break;
        default:
            break;
        }
    } while (state != 0);
    token.setValue(buffer); // 设置字符常量的值
    if (token.getType() == "Char_Constant")
    {
        count_Char_Constant++;
    }
}

// 读取字符串字面量
void Lexer::getStringLiteral(Token &token)
{
    token.setType("String_Literal");
    eatChar();
    // 识别字符串常量的自动机
    unsigned state = 1;
    char c;
    do
    {
        switch (state)
        {
        case 1:
            c = peekChar();
            if (c == '\\')
            {
                state = 2;
                eatChar();
            }
            else if (c == '\"')
            {
                state = 0;
                eatChar();
            }
            else if (c == '\n') // 未闭合的字符串常量
            {
                token.setType("Error");
                token.setError("unclosed string");
                count_Error++;
                state = 0;
            }
            else
            {
                state = 1;
                eatChar();
            }
            break;
        case 2:
            c = peekChar();
            if (c == 'a' || c == 'b' || c == 'f' || c == 'n' ||
                c == 'r' || c == 't' || c == 'v' || c == '\\' ||
                c == '\'' || c == '\"' || c == '\?')
            {
                state = 1;
                eatChar();
            }
            else
            {
                state = 0;
                token.setType("Error");
                token.setError("Invalid escape sequence in string literal");
                count_Error++;
            }
            break;
        default:
            break;
        }
    } while (state != 0);
    token.setValue(buffer);
    if (token.getType() == "String_Literal")
    {
        count_String_Literal++;
    }
}

// 词法分析
bool Lexer::lexer()
{
    bool res = true;
    while (!(file.eof() && buffer.empty()))
    {
        char c = peekChar();
        if (isLetter_(c)) // 标识符或关键字
        {
            Token token("Identifier", countLine, countColumn, "");
            getNextIdentifier(token);
            std::cout << token << std::endl;
        }
        else if (isdigit(c)) // 数值常量
        {
            Token token("Numerical_Constant", countLine, countColumn, "");
            getNextNumerical(token);
            std::cout << token << std::endl;
        }
        else if (c == '\'') // 字符常量
        {
            Token token("Char_Constant", countLine, countColumn, "");
            getCharConstant(token);
            std::cout << token << std::endl;
        }
        else if (c == '\"') // 字符串字面量
        {
            Token token("String_Literal", countLine, countColumn, "");
            getStringLiteral(token);
            std::cout << token << std::endl;
        }
        else // 标点符号
        {
            if (!isWhitespace(c))
            {
                Token token("Punctuator", countLine, countColumn, "");
                eatChar();
                bool has_error = false;
                bool is_comment = false;
                switch (c) // 标点符号的特殊处理
                {
                case '.': // 识别.或...
                    c = peekChar();
                    if (c == '.')
                    {
                        eatChar();
                        if ((c = peekChar()) == '.') // 识别...
                        {
                            eatChar();
                        }
                        else // 识别错误的省略号
                        {
                            has_error = true;
                            token.setType("Error");
                        }
                    }
                    break;
                case '>': // >或>=或>>或>>=
                    c = peekChar();
                    if (c == '>') // >>或>>=
                    {
                        eatChar();
                        c = peekChar();
                        if (c == '=') // >>=
                        {
                            eatChar();
                        }
                    }
                    else if (c == '=') // >=
                        eatChar();
                    break;
                case '<': // <或<=或<<或<<=
                    c = peekChar();
                    if (c == '<') // <<或<<=
                    {
                        eatChar();
                        if ((c = peekChar()) == '=') // <<=
                        {
                            eatChar();
                        }
                    }
                    else if (c == '=') // <=
                        eatChar();
                    break;
                case '+': // +或++或+=
                    c = peekChar();
                    if (c == '=' || c == '+')
                        eatChar();
                    break;
                case '-': // -或--或-=或->
                    c = peekChar();
                    if (c == '=' || c == '-' || c == '>')
                        eatChar();
                    break;
                case '&': // &或&&或&=
                    c = peekChar();
                    if (c == '=' || c == '&')
                        eatChar();
                    break;
                case '|': // |或||或|=
                    c = peekChar();
                    if (c == '=' || c == '|')
                        eatChar();
                    break;
                case '*':
                case '%':
                case '^':
                case '=':
                case '!':
                    c = peekChar();
                    if (c == '=') // *=或%=或^=或==或!=
                        eatChar();
                    break;
                case ';':
                case '{':
                case '}':
                case ',':
                case ':':
                case '(':
                case ')':
                case '[':
                case ']':
                case '~':
                case '?':
                    break; // 单字符标点符号
                case '#':  // 预处理指令,如#include
                    is_comment = true; // 预处理指令不输出
                    do
                    {
                        c = eatChar();
                    } while (c != '\n'); // 跳过一行
                    break;
                case '/': // /或/=或/*或//
                    c = peekChar();
                    if (c == '*' || c == '/') // 注释
                    {
                        is_comment = true;
                        // 识别注释的自动机
                        unsigned state = 1;
                        do
                        {
                            c = peekChar();
                            if (c <= 0)
                                state = 5;
                            switch (state)
                            {
                            case 1:
                                if (c == '*')
                                {
                                    state = 2;
                                    eatChar();
                                }
                                else if (c == '/')
                                {
                                    state = 4;
                                    eatChar();
                                }
                                break;
                            case 2:
                                if (c == '*')
                                {
                                    state = 3;
                                    eatChar();
                                }
                                else
                                {
                                    state = 2;
                                    eatChar();
                                }
                                break;
                            case 3:
                                if (c == '*')
                                {
                                    state = 3;
                                    eatChar();
                                }
                                else if (c == '/')
                                {
                                    state = 0;
                                    eatChar();
                                }
                                else
                                {
                                    state = 2;
                                    eatChar();
                                }
                                break;
                            case 4:
                                if (c == '\n')
                                {
                                    state = 0;
                                    eatChar();
                                }
                                else
                                {
                                    state = 4;
                                    eatChar();
                                }
                                break;
                            default:  // 注释结束
                                state = 0;
                                break;
                            }
                        } while (state != 0);
                    }
                    else if (c == '=') // /=
                        eatChar();
                    break;
                default: // 未知字符
                    has_error = true;
                    token.setType("Error");
                    token.setError("unexpected character");
                    count_Error++;
                    token.setValue(buffer);
                    break;
                }
                res &= !has_error; // 词法分析是否成功, 有错误返回false
                if (!is_comment)   // 注释不输出
                {
                    if (token.getType() == "Punctuator")
                    {
                        count_Punctuator++;
                    }
                    token.setValue(buffer);
                    std::cout << token << std::endl;
                }
                else if (has_error) // 有错误输出错误信息
                {
                    std::cout << token << std::endl;
                }
            }
            else
            {
                eatChar();
            }
        }
        buffer.clear();
    }
    return res;
}

void Lexer::printStat()
{
    std::cout << std::endl;
    std::cout << "Total characters: \t" << countChar << std::endl;
    std::cout << "Total lines: \t\t" << countLine << std::endl;
#define TOKEN_TYPE(type) std::cout << #type << ": \t\t" << count_##type << std::endl;
#include "..\include\token_type.h"
#undef TOKEN_TYPE
}