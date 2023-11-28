%option noyywrap

%x STATE_a
%x STATE_b

%{
#include<bits/stdc++.h>
using namespace std;

string str;
int line_count=1;
%}

WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z]
DIGIT [0-9]
DIGITS {DIGIT}+
NEWLINE [\n|\r\n]

CONST_INT {DIGITS}
CONST_FLOAT {DIGITS}?(\.{DIGITS})?(E[+-]?{DIGITS})?
ID [a-zA-Z_][a-zA-Z0-9_]*
NORMAL_CHAR '([ -~]{-}['\\])'
RELOP ("<"|"<="|">"|">="|"=="|"!=")
LOGICOP ("&&"|"||")
VAR ({ID}|{CONST_INT}|{CONST_FLOAT}|{NORMAL_CHAR})
OPERATOR ({RELOP}|{LOGICOP})

TERNARY ({VAR}"=")?({VAR}{OPERATOR}{VAR}"?"{VAR}":"{VAR})
NESTED_1 ({VAR}"=")?({VAR}{OPERATOR}{VAR}"?"{TERNARY}":"{VAR})
NESTED_2 ({VAR}"=")?({VAR}{OPERATOR}{VAR}"?"{VAR}":"{TERNARY})
NESTED_3 ({VAR}"=")?({VAR}{OPERATOR}{VAR}"?"{TERNARY}":"{TERNARY})

%%

{WHITESPACE} {}

{NEWLINE} { line_count++; }	

{TERNARY}|{NESTED_1}|{NESTED_2}|{NESTED_3} { cout << "Ternary operation detected" << endl; }

. { }

%%

int main(int argc,char *argv[]) {
	if(argc!=2) {
		printf("Please provide input file name and try again\n");
		return 0;
	}
	
	FILE *fin=fopen(argv[1],"r");
	if(fin==NULL) {
		printf("Cannot open specified file\n");
		return 0;
	}

	yyin= fin;
	yylex();
	fclose(yyin);
	return 0;
}
