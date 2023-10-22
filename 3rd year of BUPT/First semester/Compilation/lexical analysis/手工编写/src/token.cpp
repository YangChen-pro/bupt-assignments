#include "..\include\token.h"

std::string Token::getType()
{
    return this->type;
}

void Token::setType(std::string type)
{
    this->type = type;
}

void Token::setValue(std::string value)
{
    this->value = value;
}

void Token::setError(std::string error)
{
    this->error = error;
}

std::ostream &operator<<(std::ostream &os, const Token &token)
{
    if (token.type != "Error")
    {
        os << token.line << ":" << token.column << ":[" << token.type << "]:" << token.value;
    }
    else
    {
        os << "\033[31m" << token.line << ":" << token.column << ":[" 
           << "\033[31mError: \033[32m" << token.error << "]:" << "\033[39m" << token.value
           << "\033[0m";
    }
    return os;
}

Token::Token()
{

}

Token::Token(std::string type, unsigned line, unsigned column, std::string value):
    type(type), line(line), column(column), value(value)
{

}

Token::~Token()
{

}
