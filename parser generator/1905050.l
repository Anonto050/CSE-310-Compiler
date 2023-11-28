%option noyywrap
%x CHARACTER_STATE
%x STRING_STATE
%x SINGLE_COMMENT
%x MULTI_COMMENT


%{
#include<bits/stdc++.h>
#include<fstream>
#include<string>
#include "1905050_symbolTable.h"
#include "y.tab.h"

using namespace std;

extern YYSTYPE yylval;
//extern SymbolTable *table;
void yyerror(char *);


int count_line = 1;
int error_num = 0;
int special_char_flag;
int multi_str_flag = 0;
int start = 0;

ofstream log_file;
ofstream token_file;

int bucket_size = 11;
int id = 1;

symbolTable symbol_table(id,bucket_size,log_file);

string char_input;
string token_str_input;
string log_str_input;
string char_output;
string str_output;
string out_comment;

void printLog(string token,string lexeme){
	log_file<< "Line# "<<count_line<<": Token <"<<token<<"> Lexeme "<<lexeme<<" found"<<endl;
}

string UpperCase(string s){
	for(int i = 0; s[i] != '\0'; i++){
		if(s[i]>='a' && s[i] <= 'z')
		    s[i] = s[i] - 32;
	}
	return s;
}

//st.enter_scope(id, bucket_size, log_file);


%}

WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z_]
ALPHA_NUMERIC [a-zA-Z_0-9]
DIGIT [0-9]
NEWLINE \n

%%

{NEWLINE} {count_line++;}
{WHITESPACE} {}

"if" {
	symbolInfo* symbol = new symbolInfo(yytext, "IF");
    symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("IF",yytext);
	return IF;
}
"else" {  
    symbolInfo* symbol = new symbolInfo(yytext, "ELSE");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("ELSE",yytext);
	return ELSE;
}
"for" {

	symbolInfo* symbol = new symbolInfo(yytext, "FOR");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("FOR",yytext);
	return FOR;
}
"while" {
	symbolInfo* symbol = new symbolInfo(yytext, "WHILE");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("WHILE",yytext);
	return WHILE;
}
"do" {
	symbolInfo* symbol = new symbolInfo(yytext, "DO");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("DO",yytext);
	return DO;
}
"break" {

	symbolInfo* symbol = new symbolInfo(yytext, "BREAK");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("BREAK",yytext);
	return BREAK;
}
"int" {
	symbolInfo* symbol = new symbolInfo(yytext, "INT");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("INT",yytext);
	return INT;
}
"char" {
	symbolInfo* symbol = new symbolInfo(yytext, "CHAR");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("CHAR",yytext);
	return CHAR;
}
"float" {
	symbolInfo* symbol = new symbolInfo(yytext, "FLOAT");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("FLOAT",yytext);
	return FLOAT;
}
"double" {
	symbolInfo* symbol = new symbolInfo(yytext, "DOUBLE");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("DOUBLE",yytext);
	return DOUBLE;
}
"void" {
	symbolInfo* symbol = new symbolInfo(yytext, "VOID");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("VOID",yytext);
	return VOID;
}
"return" {

	symbolInfo* symbol = new symbolInfo(yytext, "RETURN");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("RETURN",yytext);
	return RETURN;
}
"case" {

	symbolInfo* symbol = new symbolInfo(yytext, "CASE");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("CASE",yytext);
	return CASE;
}
"default" {

	symbolInfo* symbol = new symbolInfo(yytext, "DEFAULT");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("DEFAULT",yytext);
	return DEFAULT;
}
"continue" {

		symbolInfo* symbol = new symbolInfo(yytext, "CONTINUE");
		symbol->setStartLine(count_line);
		symbol->setEndLine(count_line);
		symbol->setIsLeaf(true);

		yylval.symbol_info = symbol;
		printLog("CONTINUE",yytext);
		return CONTINUE;
}
"println" {

			  symbolInfo* symbol = new symbolInfo(yytext, "PRINTLN");
			  symbol->setStartLine(count_line);
			  symbol->setEndLine(count_line);
			  symbol->setIsLeaf(true);

			  yylval.symbol_info = symbol;
	          printLog("PRINTLN",yytext);
			  return PRINTLN;
}

			
{LETTER}{ALPHA_NUMERIC}* {
	 symbolInfo* symbol = new symbolInfo(yytext, "ID");
	 symbol->setStartLine(count_line);
	 symbol->setEndLine(count_line);
	 symbol->setIsLeaf(true);
	 //symbol_table.insert(symbol);
	 yylval.symbol_info = symbol;

	 printLog("ID",yytext);
	 return ID;
			       
}	


{DIGIT}+ {
	 symbolInfo* symbol = new symbolInfo(yytext, "CONST_INT");
	 symbol->setStartLine(count_line);
	 symbol->setEndLine(count_line);
	 symbol->setIsLeaf(true);
	 yylval.symbol_info = symbol;
	 printLog("CONST_INT",yytext);
	 return CONST_INT; 
}

{DIGIT}+(\.{DIGIT}+)?([Ee][+-]?{DIGIT}+)?      {
			 symbolInfo* symbol = new symbolInfo(yytext, "CONST_FLOAT");
			 symbol->setStartLine(count_line);
			 symbol->setEndLine(count_line);
			 symbol->setIsLeaf(true);

			 yylval.symbol_info = symbol;
			 printLog("CONST_FLOAT",yytext);
			 return CONST_FLOAT;  
	   }
		

{DIGIT}+(\.{DIGIT}+)?([Ee][+-]?{DIGIT}+)?{LETTER}{ALPHA_NUMERIC}* {

	error_num++; 
	 //Error at line# 3: INVALID_ID_SUFFIX_NUM_PREFIX 12abcd
	 log_file<< "Error at line# "<<count_line<<": INVALID_ID_SUFFIX_NUM_PREFIX "<<yytext<<endl;  

}

{DIGIT}*(\.{DIGIT}*)(\.{DIGIT}*)+([Ee][+-]?{DIGIT}+)? {
	error_num++;
     log_file<< "Error at line# "<<count_line<<": TOO_MANY_DECIMAL_POINTS "<<yytext<<endl;  
}

{DIGIT}+(\.{DIGIT}*)?([Ee][+-]?{DIGIT}*(\.{DIGIT}*)?)* {
	error_num++;
     log_file<< "Error at line# "<<count_line<<": ILLFORMED_NUMBER "<<yytext<<endl;  
}


"+" |
"-" {
         symbolInfo* symbol = new symbolInfo(yytext, "ADDOP");
			symbol->setStartLine(count_line);
			symbol->setEndLine(count_line);
			symbol->setIsLeaf(true);

			yylval.symbol_info = symbol;

			printLog("ADDOP",yytext);
			return ADDOP; 	
}

"*" |
"/" |
"%" {
			 symbolInfo* symbol = new symbolInfo(yytext, "MULOP");
			symbol->setStartLine(count_line);
			symbol->setEndLine(count_line);
			symbol->setIsLeaf(true);

			yylval.symbol_info = symbol;
			printLog("MULOP",yytext);
			return MULOP; 	 	
} 

"++" {
	symbolInfo* symbol = new symbolInfo(yytext, "INCOP");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("INCOP",yytext);
	return INCOP;
}
"--" {
	symbolInfo* symbol = new symbolInfo(yytext, "DECOP");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("DECOP",yytext);
	return DECOP;
}

"<" |
"<=" |
">" |
">=" |
"==" |
"!=" {
			 symbolInfo* symbol = new symbolInfo(yytext, "RELOP");
			symbol->setStartLine(count_line);
			symbol->setEndLine(count_line);
			symbol->setIsLeaf(true);

			yylval.symbol_info = symbol;
			printLog("RELOP",yytext);
			return RELOP; 	 		 	
}

"=" {
			 symbolInfo* symbol = new symbolInfo(yytext, "ASSIGNOP");
			symbol->setStartLine(count_line);
			symbol->setEndLine(count_line);
			symbol->setIsLeaf(true);

			yylval.symbol_info = symbol;
			printLog("ASSIGNOP",yytext);
			return ASSIGNOP; 	 		 	
}

"&&" |
"||" {
			 symbolInfo* symbol = new symbolInfo(yytext, "LOGICOP");
			symbol->setStartLine(count_line);
			symbol->setEndLine(count_line);
			symbol->setIsLeaf(true);

			yylval.symbol_info = symbol;
			printLog("LOGICOP",yytext);
			return LOGICOP; 	 		 	
}


"!" {
			 symbolInfo* symbol = new symbolInfo(yytext, "NOT");
			symbol->setStartLine(count_line);
			symbol->setEndLine(count_line);
			symbol->setIsLeaf(true);

			yylval.symbol_info = symbol;
		   printLog("NOT",yytext);
			return NOT; 	 		 	  
}

"(" {

	symbolInfo* symbol = new symbolInfo(yytext, "LPAREN");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;

	printLog("LPAREN",yytext);
	return LPAREN;
}

")" {
	symbolInfo* symbol = new symbolInfo(yytext, "RPAREN");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("RPAREN",yytext);
	return RPAREN; 
}

"{" {
	symbolInfo* symbol = new symbolInfo(yytext, "LCURL");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("LCURL",yytext);
	return LCURL;
}

"}" {
	symbolInfo* symbol = new symbolInfo(yytext, "RCURL");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("RCURL",yytext);
	return RCURL; 
}
"[" {
	symbolInfo* symbol = new symbolInfo(yytext, "LSQUARE");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("LSQUARE",yytext);
	return LTHIRD; 
}
"]" {
	symbolInfo* symbol = new symbolInfo(yytext, "RSQUARE");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("RSQUARE",yytext);
	return RTHIRD; 
}
"," {
	symbolInfo* symbol = new symbolInfo(yytext, "COMMA");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("COMMA",yytext);
	return COMMA;   
}
";" {
	symbolInfo* symbol = new symbolInfo(yytext, "SEMICOLON");
	symbol->setStartLine(count_line);
	symbol->setEndLine(count_line);
	symbol->setIsLeaf(true);

	yylval.symbol_info = symbol;
	printLog("SEMICOLON",yytext);
	return SEMICOLON; 
}

"'" {

    char_input.clear();
	special_char_flag = 0;
	start = count_line;
	BEGIN CHARACTER_STATE;
}

\" {

    log_str_input.clear();
	token_str_input.clear();
	multi_str_flag = 0;
	start = count_line;
	BEGIN STRING_STATE;
}
\/\/ {
    out_comment.clear();
	out_comment += "//";
	start = count_line;
	BEGIN SINGLE_COMMENT;
}

\/\* {
	out_comment.clear();  
    out_comment += "/*";
	start = count_line;
	cout<<count_line;
    BEGIN MULTI_COMMENT;
}

<CHARACTER_STATE>(["\\"]["a"]) {
            
	   char_input += "\a";
	   special_char_flag = 1;
}
<CHARACTER_STATE>(["\\"]["b"]) {
            
	   char_input += "\b";
	     special_char_flag = 1;
}
<CHARACTER_STATE>(["\\"]["f"]) {
           
	   char_input += "\f";
	     special_char_flag = 1;
}
<CHARACTER_STATE>(["\\"]["n"]) {
           
	   char_input += "\n";
	     special_char_flag = 1;
		 count_line++;
}

<CHARACTER_STATE>(["\\"]["r"]) {
           
	   char_input += "\r";
	     special_char_flag = 1;
}
<CHARACTER_STATE>(["\\"]["t"]) {
           
	   char_input += "\t";
	     special_char_flag = 1;
	   //cout<<"\t"<<endl;
	   //cout<<yytext<<endl;
}
<CHARACTER_STATE>(["\\"]["v"]) {
           
	   char_input += "\v";
	     special_char_flag = 1;
}
<CHARACTER_STATE>(["\\"]["0"]) {
           
	   char_input += "\0";
	     special_char_flag = 1;
}
<CHARACTER_STATE>["\\"]. {
           
	   char_input += "\\";
       char_input += yytext[1];
	     special_char_flag = 1;
}

<CHARACTER_STATE><<EOF>> {

	char_output += "'";
	char_output += char_input;
        
	error_num++;
    log_file<< "Error at line# "<<count_line<<": UNFINISHED_CONST_CHAR "<<char_output<<endl;

	char_input.clear();
	char_output.clear();
	//special_char_flag = 0;

	BEGIN INITIAL;
}

<CHARACTER_STATE>"'" {

       char_output += "'";
	char_output += char_input;
	char_output += "'";

       if(char_input.size()==0){
             error_num++;
    log_file<< "Error at line# "<<count_line<<": EMPTY_CONST_CHAR "<<char_output<<endl;
	   }

	   else if(char_input.size()==1){
                 token_file<<"<CONST_CHAR, "<<char_input<<">"<<endl;
	log_file<< "Line# "<<start<<": Token <CONST_CHAR> Lexeme "<<char_input<<" found"<<endl; 
	   }
	   else if(char_input.size()==2 && special_char_flag==1){
            token_file<<"<CONST_CHAR, "<<char_input<<">"<<endl;
	log_file<< "Line# "<<start<<": Token <CONST_CHAR> Lexeme "<<char_input<<" found"<<endl; 
	   }
	  else{
        error_num++;
		
    log_file<< "Error at line# "<<count_line<<": MULTICHAR_CONST_CHAR "<<char_output<<endl;
	   }

      char_input.clear();
	  char_output.clear();
	  special_char_flag = 0;
	   BEGIN INITIAL;
}

<CHARACTER_STATE>{NEWLINE} {
    char_output += "'";
	char_output += char_input;

	error_num++;
    log_file<< "Error at line# "<<count_line<<": UNFINISHED_CONST_CHAR "<<char_output<<endl;

	count_line++;

	char_input.clear();
	char_output.clear();
	//special_char_flag = 0;
	BEGIN INITIAL;  
}
<CHARACTER_STATE>. {
	char_input += yytext[0];
}


<STRING_STATE>(["\\"]["a"]) {
            
	   token_str_input += "\a";
	   log_str_input += "\\a";
	   //special_char_flag = 1;
}
<STRING_STATE>(["\\"]["b"]) {
            
	   token_str_input += "\b";
	   log_str_input += "\\b";
	     //special_char_flag = 1;
}
<STRING_STATE>(["\\"]["f"]) {
           
	   token_str_input += "\f";
	   log_str_input += "\\f";
}
<STRING_STATE>(["\\"]["n"]) {
           
	   token_str_input += "\n";
	   log_str_input += "\\n";
}

<STRING_STATE>(["\\"]["r"]) {
           
	   token_str_input += "\r";
	   log_str_input += "\\r";
}
<STRING_STATE>(["\\"]["t"]) {
           
	   token_str_input += "\t";
	   log_str_input += "\\t";
}
<STRING_STATE>(["\\"]["v"]) {
           
	   token_str_input += "\v";
	   log_str_input += "\\v";
}
<STRING_STATE>(["\\"]["0"]) {
           
	   token_str_input += "\0";
	   log_str_input += "\\0";
}
<STRING_STATE>(["\\"].) {
           
	   token_str_input += "\\";
	   log_str_input += "\\";
	   token_str_input +=yytext[1];
	   log_str_input +=yytext[1];
}


<STRING_STATE><<EOF>> {

	str_output += "\"";
	str_output += log_str_input ;
        
	error_num++;
    log_file<< "Error at line# "<<count_line<<": UNFINISHED_STRING "<<str_output<<endl;

	log_str_input.clear();
	token_str_input.clear();
	str_output.clear();
	multi_str_flag = 0;

	BEGIN INITIAL;
}

<STRING_STATE>"\"" {

       str_output += "\"";
	   str_output += log_str_input ;
	   str_output += "\"";

	   if(multi_str_flag == 1){
	   token_file<<"<MULTI LINE STRING, "<<token_str_input<<">"<<endl;
	   log_file<< "Line# "<<start<<": Token <MULTI LINE STRING> Lexeme "<<str_output<<" found"<<endl; 
	   }
       else{
       token_file<<"<SINGLE LINE STRING, "<<token_str_input<<">"<<endl;
	   log_file<< "Line# "<<start<<": Token <SINGLE LINE STRING> Lexeme "<<str_output<<" found"<<endl; 
	   }
	  
      log_str_input.clear();
	token_str_input.clear();
	str_output.clear();
	multi_str_flag = 0;

	BEGIN INITIAL;
}

<STRING_STATE>\\[\r]?\n {
	multi_str_flag = 1;
	token_str_input += "\t";
	log_str_input += "\\\n";
	//start = count_line;
	count_line++;
}
<STRING_STATE>[\r]?\n {

    //token_str_input += "\n";
	//log_str_input += "\n";
	
	str_output += "\"";
	str_output += log_str_input ;
        
	error_num++;
    log_file<< "Error at line# "<<count_line<<": UNFINISHED_STRING "<<str_output<<endl;

    count_line++;
	log_str_input.clear();
	token_str_input.clear();
	str_output.clear();
	multi_str_flag = 0;

	BEGIN INITIAL;
}


<SINGLE_COMMENT>\\[\r]?\n {
	 
      count_line++;
	  out_comment += "\\\n";

}

<SINGLE_COMMENT>[\r]?\n {
      
	  log_file<< "Line# "<<start<<": Token <SINGLE LINE COMMENT> Lexeme "<<out_comment<<" found"<<endl; 
	  count_line++;
      BEGIN INITIAL;
}
<SINGLE_COMMENT><<EOF>> {
	log_file<< "Line# "<<start<<": Token <SINGLE LINE COMMENT> Lexeme "<<out_comment<<" found"<<endl; 
      BEGIN INITIAL;
}
<SINGLE_COMMENT>\\ {
	out_comment += "\\";
}
<SINGLE_COMMENT>. {
	out_comment += yytext[0];
}



<MULTI_COMMENT>\*\/ {
      
      out_comment += "*/";
	  log_file<< "Line# "<<start<<": Token <MULTI LINE COMMENT> Lexeme "<<out_comment<<" found"<<endl; 
      BEGIN INITIAL;
}
<MULTI_COMMENT><<EOF>> {

	error_num++;
	count_line--;
	log_file<< "Error at line# "<<count_line<<": UNFINISHED_COMMENT "<<out_comment<<endl;

	out_comment.clear();  
	BEGIN INITIAL;
}
<MULTI_COMMENT>\n {
	count_line++;
	out_comment += "\n";
}
<MULTI_COMMENT>. {
	out_comment += yytext[0];
}


. {
	error_num++;
    log_file<< "Error at line# "<<count_line<<": UNRECOGNIZED_CHAR "<<yytext<<endl;  
}

%%

