%{

#include <stdio.h>
#include "parser.tab.h"

%}

/* Some useful grammar */
O    [0-7]  
D    [0-9] 
NZ   [1-9] 
L    [a-zA-Z_] 
A    [a-zA-Z_0-9] 
H    [a-fA-F0-9] 
HP   (0[xX]) 
E    ([Ee][+-]?{D}+) 
P    ([Pp][+-]?{D}+) 
FS   (f|F|l|L) 
IS   (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?)) 
CP   (u|U|L) 
SP   (u8|u|U|L) 
ES   (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+)) 
NE   [^"\\\n] 
WS   [ \t\v\f] 
end  [$\r]

%%

"//".*                          { return -1; }  /* single-line comment */
"#".+                           { return -1; }  /* preprocessor directive */
{WS}			   	    	    { return -1; }    /* white space */

{end}                           { return END; }
"("                             { return LP; }
")"                             { return RP; }
"+"                             { return PLUS; }
"-"                             { return MINUS; }
"*"                             { return TIMES; }
"/"                             { return DIV; }

{HP}{H}+{IS}?			    	{ return I_CONSTANT; }  /* constants */
{NZ}{D}*{IS}?			    	{ return I_CONSTANT; }
"0"{O}*{IS}?		    		{ return I_CONSTANT; }

{D}+{E}{FS}?		    		{ return F_CONSTANT; }
{D}*"."{D}+{E}?{FS}?			{ return F_CONSTANT; }
{D}+"."{E}?{FS}?		    	{ return F_CONSTANT; }
{HP}{H}+{P}{FS}?		    	{ return F_CONSTANT; }
{HP}{H}*"."{H}+{P}{FS}?			{ return F_CONSTANT; }
{HP}{H}+"."{P}{FS}?		    	{ return F_CONSTANT; }

{D}+({A}|\.)+                   { return -1; } /* errors */
{SP}?\"({NE}|{ES})*{WS}*        { return -1; }
.	    		        		{ return -1; }

%%

int yywrap(void){
    return 1;
}

