#include <iostream>
#include "..\include\lexer.h"


int main()
{
    Lexer lexer("../test/test2.c");
    bool noerr = lexer.lexer();
    lexer.printStat();
    if (noerr)
    {
        std::cout << "No error found." << std::endl;
    }
    else
    {
        std::cout << "Error found." << std::endl;
    }
    return 0;
}

// ./src/main.exe ./test/test1.c

