%{
#include <stdio.h>
# define ACC 1
extern int yylex();
int yyerror(const char* s);
int success = 1;
%}

%token PLUS MINUS TIMES DIV LP RP I_CONSTANT F_CONSTANT END
%start S

%%
S : E END { printf("S -> E\n"); return ACC; }
  ;

E: E PLUS T { printf("E -> E + T\n"); }
 | E MINUS T { printf("E -> E - T\n"); }
 | T { printf("E -> T\n"); }
 ;

T: T TIMES F { printf("T -> T * F\n"); }
 | T DIV F { printf("T -> T / F\n"); }
 | F { printf("T -> F\n"); }
 ;

F: LP E RP { printf("F -> (E)\n"); }
 | I_CONSTANT { printf("F -> num (I_CONSTANT)\n"); }
 | F_CONSTANT { printf("F -> num (F_CONSTANT)\n"); }
 ;

%%

int main(){
    yyparse();
    if (success == 1)
        printf("\033[32mParsing done.\033[0m\n");
    return 0;
}

int yyerror(const char *msg)
{
	extern int yylineno;
	printf("\033[31mParsing Failed\nLine Number: %d %s\033[0m\n", yylineno, msg);
    success = 0;
	return 0;
}

/* D:\win_flex_bison-latest\win_bison.exe -d parser.y */
/* D:\win_flex_bison-latest\win_flex.exe --wincompat lexcial.l */
/* gcc lex.yy.c parser.tab.c -o parser */
/* ./parser.exe */
/* 输入要以$结尾 */
