// Line comment *///*/
#include <stdio.h>

int main()
{
    /*
     * Block comment *
     */
    char *msg = "Hello ";
    char ch = 'w';
    float f = 0.145e+3;
    double d = 3.e3;
    float f2 = 0.145e-3;
    double d2 = 3.;
    int integer = 864;
    long long int longint = 1234567890123456789;
    printf("%s %f\n", msg, d);
    printf("%c\t%d\n", ch, integer);
    printf("%lld\v", longint);
    return 0;
}