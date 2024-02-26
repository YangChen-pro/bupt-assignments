/*
 * c11.lex: An C11 scanner
 * 
 */

/* do not wrap */
%option noyywrap 

/* Some useful grammar */
O   [0-7]  
D   [0-9] 
NZ  [1-9] 
L   [a-zA-Z_] 
A   [a-zA-Z_0-9] 
H   [a-fA-F0-9] 
HP  (0[xX]) 
E   ([Ee][+-]?{D}+) 
P   ([Pp][+-]?{D}+) 
FS  (f|F|l|L) 
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?)) 
CP  (u|U|L) 
SP  (u8|u|U|L) 
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+)) 
NE  [^"\\\n] 
WS  [ \t\v\f] 

%{
#include <stdio.h>

/* C token types */
#define KEYWORD 1
#define IDENTIFIER 2
#define I_CONSTANT 3 // integer constant
#define F_CONSTANT 4 // floating constant
#define C_CONSTANT 5 // character constant
#define STRING_LITERAL 6
#define PUNCTUATOR 7

/* statistics */
int charCount = 0;
int lineCount = 1;
int columnCount = 1;
int keywordCount = 0;
int identifierCount = 0;
int integerConst = 0;
int floatingConst = 0;
int charConst = 0;
int stringCount = 0;
int punctuatorCount = 0;
int errorCount = 0;

/* handle errors */
void yyerror(const char *);

/* handle multi-line comments */
static void isComment(void);
%}

%%
"\n"                    { lineCount++; columnCount=0; return -1; } /* wrap */
"/*"                    { isComment(); return -1; }   /* multi-line comment begins */
"//".*                  { return -1; }  /* single-line comment */
"#".+                   { return -1; }  /* preprocessor directive */
{WS}			   	    	    {return -1;}    /* white space */

"auto"					{ return KEYWORD; } /* keywords */
"break"					{ return KEYWORD; }
"case"					{ return KEYWORD; }
"char"					{ return KEYWORD; }
"const"					{ return KEYWORD; }
"continue"				{ return KEYWORD; }
"default"				{ return KEYWORD; }
"do"					{ return KEYWORD; }
"double"				{ return KEYWORD; }
"else"					{ return KEYWORD; }
"enum"					{ return KEYWORD; }
"extern"				{ return KEYWORD; }
"float"					{ return KEYWORD; }
"for"					{ return KEYWORD; }
"goto"					{ return KEYWORD; }
"if"					{ return KEYWORD; }
"inline"				{ return KEYWORD; }
"int"					{ return KEYWORD; }
"long"					{ return KEYWORD; }
"register"				{ return KEYWORD; }
"restrict"				{ return KEYWORD; }
"return"				{ return KEYWORD; }
"short"					{ return KEYWORD; }
"signed"				{ return KEYWORD; }
"sizeof"				{ return KEYWORD; }
"static"				{ return KEYWORD; }
"struct"				{ return KEYWORD; }
"switch"				{ return KEYWORD; }
"typedef"				{ return KEYWORD; }
"union"					{ return KEYWORD; }
"unsigned"				{ return KEYWORD; }
"void"					{ return KEYWORD; }
"volatile"				{ return KEYWORD; }
"while"					{ return KEYWORD; }
"_Alignas"              { return KEYWORD; }
"_Alignof"              { return KEYWORD; }
"_Atomic"               { return KEYWORD; }
"_Bool"                 { return KEYWORD; }
"_Complex"              { return KEYWORD; }
"_Generic"              { return KEYWORD; }
"_Imaginary"            { return KEYWORD; }
"_Inline"               { return KEYWORD; }
"_Noreturn"             { return KEYWORD; }
"_Static_assert"        { return KEYWORD; }
"_Thread_local"         { return KEYWORD; }
"__func__"              { return KEYWORD; }

{L}{A}*					{ return IDENTIFIER; }  /* identifiers */

{HP}{H}+{IS}?			    	{ return I_CONSTANT; }  /* constants */
{NZ}{D}*{IS}?			    	{ return I_CONSTANT; }
"0"{O}*{IS}?		    		{ return I_CONSTANT; }
{CP}?"'"({NE}|{ES})+"'"		{ return C_CONSTANT; }

{D}+{E}{FS}?		    		{ return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ return F_CONSTANT; }
{D}+"."{E}?{FS}?		    	{ return F_CONSTANT; }
{HP}{H}+{P}{FS}?		    	{ return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?		    	{ return F_CONSTANT; }

({SP}?\"({NE}|{ES})*\"{WS}*)+	{ return STRING_LITERAL; }  /* strings literal */



"..."					{ return PUNCTUATOR; }  /* punctuators */
">>="					{ return PUNCTUATOR; }
"<<="					{ return PUNCTUATOR; }
"+="					{ return PUNCTUATOR; }
"-="					{ return PUNCTUATOR; }
"*="					{ return PUNCTUATOR; }
"/="					{ return PUNCTUATOR; }
"%="					{ return PUNCTUATOR; }
"&="					{ return PUNCTUATOR; }
"^="					{ return PUNCTUATOR; }
"|="					{ return PUNCTUATOR; }
">>"					{ return PUNCTUATOR; }
"<<"					{ return PUNCTUATOR; }
"++"					{ return PUNCTUATOR; }
"--"					{ return PUNCTUATOR; }
"->"					{ return PUNCTUATOR; }
"&&"					{ return PUNCTUATOR; }
"||"					{ return PUNCTUATOR; }
"<="					{ return PUNCTUATOR; }
">="					{ return PUNCTUATOR; }
"=="					{ return PUNCTUATOR; }
"!="					{ return PUNCTUATOR; }
";"			    		{ return PUNCTUATOR; }
("{"|"<%")				{ return PUNCTUATOR; }
("}"|"%>")				{ return PUNCTUATOR; }
","				    	{ return PUNCTUATOR; }
":"			    		{ return PUNCTUATOR; }
"="			    		{ return PUNCTUATOR; }
"("			    		{ return PUNCTUATOR; }
")"			    		{ return PUNCTUATOR; }
("["|"<:")				{ return PUNCTUATOR; }
("]"|":>")				{ return PUNCTUATOR; }
"."			    		{ return PUNCTUATOR; }
"&"			    		{ return PUNCTUATOR; }
"!"			    		{ return PUNCTUATOR; }
"~"				    	{ return PUNCTUATOR; }
"-"		    			{ return PUNCTUATOR; }
"+"			    		{ return PUNCTUATOR; }
"*"				    	{ return PUNCTUATOR; }
"/"	    				{ return PUNCTUATOR; }
"%"		    			{ return PUNCTUATOR; }
"<"			    		{ return PUNCTUATOR; }
">"				    	{ return PUNCTUATOR; }
"^"	    				{ return PUNCTUATOR; }
"|"		    			{ return PUNCTUATOR; }
"?"			    		{ return PUNCTUATOR; }


{D}+({A}|\.)+                   { yyerror("illegal name"); return -1; } /* errors */
{SP}?\"({NE}|{ES})*{WS}*    { yyerror("unclosed string"); return -1; }
.	    		        		{ yyerror("unexpected character"); return -1; }

%%

void yyerror(const char *err)
{
    errorCount++;
    fprintf(stderr, "%d:%d:[\033[31mError: \033[32m%s\033[39m]: %s\n", lineCount, columnCount, err, yytext);
}

static void isComment(void)
{
    int c;

    while ((c = yyinput()) != 0)
    {
        charCount++;
        if (c == '\n')
            lineCount++;
        if (c == '*')
        {
            while ((c = yyinput()) == '*')
            {
                charCount++;
            }
            charCount++;
            if (c == '\n')
            {
                lineCount++;
                columnCount = 1;
            }

            if (c == '/')
            {
                return;
            }

            if (c == 0)
                break;
        }
    }
    yyerror("unterminated comment");
}

int main()
{
    int code;
    yyin = fopen("D:\\program\\work\\code\\test\\test1.c", "r");

    while ((code = yylex()))    // code is the type of token
    {
        charCount += yyleng;    // character counts
        switch (code)
        {
        case KEYWORD:
            keywordCount++;
            printf("%d:%d: [Keyword]: %s\n", lineCount, columnCount, yytext);
            break;
        case IDENTIFIER:
            identifierCount++;
            printf("%d:%d: [Identifier]: %s\n", lineCount, columnCount, yytext);
            break;
        case I_CONSTANT:
            integerConst++;
            printf("%d:%d: [Integer]: %s\n", lineCount, columnCount, yytext);
            break;
        case C_CONSTANT:
            charConst++;
            printf("%d:%d: [Character]: %s\n", lineCount, columnCount, yytext);
            break;
        case F_CONSTANT:
            floatingConst++;
            printf("%d:%d: [Floating]: %s\n", lineCount, columnCount, yytext);
            break;
        case STRING_LITERAL:
            stringCount++;
            printf("%d:%d: [String]: %s\n", lineCount, columnCount, yytext);
            break;
        case PUNCTUATOR:
            punctuatorCount++;
            printf("%d:%d: [Punctuator]: %s\n", lineCount, columnCount, yytext);
            break;
        default:
            break;
        }
        columnCount += yyleng;  // cursor forward
    }
    printf("\nTotal characters:\t%d\n", charCount);
    printf("Total lines:\t%d\n", lineCount);
    printf("\n");
    printf("Keyword:\t%d\n", keywordCount);
    printf("Identifier:\t%d\n", identifierCount);
    printf("Integers:\t%d\n", integerConst);
    printf("Floatings:\t%d\n", floatingConst);
    printf("Characters:\t%d\n", charConst);
    printf("Strings:\t%d\n", stringCount);
    printf("Punctuators:\t%d\n", punctuatorCount);
    printf("\033[1mErrors:\t\t%d\n\033", errorCount);

    fclose(yyin);
    return 0;
}

/* 
 *D:\win_flex_bison-latest\win_flex.exe --wincompat --outfile=D:\win_flex_bison-latest\code.yy.c D:\win_flex_bison-latest\code.l
 */