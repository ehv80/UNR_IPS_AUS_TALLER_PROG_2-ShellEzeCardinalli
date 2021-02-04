%{
#include <iostream>
#include "glc.tab.h"
#include <errno.h>
%}
%%
[' '\t]+		;
"\n"			return '\n';
";"			return ';';
"|"			return '|';
"||"			return PIPPIP;
"&&"			return AMPAMP;
"<"			return '<';
">"			return '>';
">>"			return MAYMAY;
"("			return '(';
")"			return ')';
"exit"			return EXIT;
[\-/\+\.0-9a-zA-Z]+	return STRING;
.				
%%
int yywrap()
{
	return 1;
}
