%{
#include <iostream>
#include <string.h>
#include <errno.h>
#include "exec.h"
//#define YYDEBUG 1
int yyerror(char*);

extern int yylex();
extern int ydebug;
extern char *yytext;
static class Exec *kkk;

char **dup_arreglo(char **a, char *p)
{
	int i;
	for(i=0; a[i]!=0; i++)
		;
	i++;
	char **tmp=new char*[i+1];
	int j;
	for(j=0; a[j]!=0; j++)
		tmp[j]=a[j];
	tmp[j]=p;
	tmp[j+1]=0;
	delete[] a;
	return tmp;
}
char **mete(char *p, char **a)
{
	int i;
	for(i=0; a[i]!=0; i++)
		;
	i++;
	char **tmp=new char*[i+1];
	int j;
	tmp[0]=p;
	for(j=0; a[j]!=0; j++)
		tmp[j+1]=a[j];
	tmp[j+1]=0;
	delete[] a;
	return tmp;
}
%}

%token MAYMAY PIPPIP AMPAMP
%token STRING
%token EXIT

%union {
	char *pc;
	char **ppc;
	class Exec *pex;
};

%type<pc> prog arg archivo
%type<ppc> args
%type<pex> redir pipe programa orden secuencia cond

%%
orden : secuencia '\n'		{ kkk=$1; YYACCEPT; }
	| '\n'			{ YYACCEPT; }
	;
secuencia: cond ';' secuencia	{ $$=new Secuencia($1,$3); }
	| cond 			{ $$=$1; }
	;
cond: redir AMPAMP cond		{ $$=new Cond_1($1,$3); }
	| redir PIPPIP cond	{ $$=new Cond_2($1,$3); }
	| redir			{ $$=$1; }
	;
redir: pipe '>' archivo		{ $$=new Redir_1($1,$3); }
	| pipe '<' archivo	{ $$=new Redir_2($1,$3); }
	| pipe MAYMAY archivo	{ $$=new Redir_3($1,$3); }
	| pipe			{ $$=$1; }
	;
pipe : programa '|' pipe	{ $$=new Pipe($1,$3); }
	| programa		{ $$=$1; }
	;
programa: prog args		{ $$=new Programa(mete($1, $2));}
	| EXIT			{ $$=new Exit;}
	;
prog: STRING			{ $$=strcpy(new char[strlen(yytext)+1], yytext); }
	;
args: args arg			{ $$=dup_arreglo($1, $2); }
	|			{ $$=new char*(0); }	
		;
arg: STRING			{ $$=strcpy(new char[strlen(yytext)+1], yytext); }
	;
archivo: STRING			{ $$=strcpy(new char[strlen(yytext)+1], yytext); }
	;
%%
int yyerror(char *s)
{
	cerr<<"ERROR"<<s<<'\n';
	return 0;
}
int main(int argc, char **argv)
{
	extern int yy_flex_debug;
	int miargc;
	int i;
	yydebug=yy_flex_debug=0;
	for(miargc=1; miargc<argc; miargc++)
		if(strcmp(argv[miargc], "--bison")==0)
			yydebug=1;
		else if(strcmp(argv[miargc], "--flex")==0)
			yy_flex_debug=1;
		else {
			cerr<<"uso: "<<argv[0]<<" [--flex | --bison]\n";
			exit(-1);
		}
	kkk=0;
	i=1;	
	while(i){
	
		yyparse();
		cout << "TallerIII :";
		if(kkk!=0) kkk->exec(_Return);
		delete kkk;
		kkk=0;
	}
	return 0;
}
