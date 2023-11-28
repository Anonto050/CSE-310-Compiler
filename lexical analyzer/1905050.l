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

using namespace std;


int count_line = 1;
int error_num = 0;
int special_char_flag;
int multi_str_flag = 0;
int start = 0;

ofstream log_file;
ofstream token_file;

int bucket_size = 10;
int id = 1;

symbolTable symbol_table(id,bucket_size,log_file);

string char_input;
string token_str_input;
string log_str_input;
string char_output;
string str_output;
string out_comment;


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
//MAC (#define)
DIGIT [0-9]
NEWLINE \n

%%

{NEWLINE} {count_line++;}
{WHITESPACE} {}

"if" | 
"else" |
"for" |
"while" |
"do" |
"break" |
"int" |
"char" |
"float" |
"double" |
"void" |
"return" |
"switch" |
"case" |
"default" |
"continue" {
              token_file<<"<"<<UpperCase(yytext)<<", "<<yytext<<">"<<endl;
		    log_file<< "Line# "<<count_line<<": Token <"<<UpperCase(yytext)<<"> Lexeme "<<yytext<<" found"<<endl;
}
			
{LETTER}{ALPHA_NUMERIC}* {
	        token_file<<"<ID, "<<yytext<<">"<<endl;
		    log_file<< "Line# "<<count_line<<": Token <ID> Lexeme "<<yytext<<" found"<<endl;

            
			symbolInfo* symbol = new symbolInfo(yytext, "ID");


			if(symbol_table.Insert(*symbol,log_file)==true)
			{
                symbol_table.print_all(log_file);
			}
			else{
				log_file<<"\t"<<yytext<<" already exists in the current ScopeTable"<<endl;
			}
			       
}	


{DIGIT}+ {
	 token_file<<"<CONST_INT, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <CONST_INT> Lexeme "<<yytext<<" found"<<endl;  

}

{DIGIT}+(\.{DIGIT}+)?([Ee][+-]?{DIGIT}+)?      {
		token_file<<"<CONST_FLOAT, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <CONST_FLOAT> Lexeme "<<yytext<<" found"<<endl;  
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
         token_file<<"<ADDOP, "<<yytext<<">"<<endl;
		 log_file<< "Line# "<<count_line<<": Token <ADDOP> Lexeme "<<yytext<<" found"<<endl;  	
}

"*" |
"/" |
"%" {
	   token_file<<"<MULOP, "<<yytext<<">"<<endl;
	log_file<< "Line# "<<count_line<<": Token <MULOP> Lexeme "<<yytext<<" found"<<endl;  	
} 

"++" |
"--" {
	token_file<<"<INCOP, "<<yytext<<">"<<endl;
    log_file<< "Line# "<<count_line<<": Token <INCOP> Lexeme "<<yytext<<" found"<<endl;  	
}

"<" |
"<=" |
">" |
">=" |
"==" |
"!=" {
	 token_file<<"<RELOP, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <RELOP> Lexeme "<<yytext<<" found"<<endl;  	
}

"=" {
	token_file<<"<ASSIGNOP, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <ASSIGNOP> Lexeme "<<yytext<<" found"<<endl;  
}

"&&" |
"||" {
	token_file<<"<LOGICOP, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <LOGICOP> Lexeme "<<yytext<<" found"<<endl;  
}

"&" |
"|" |
"^" |
"<<" |
">>" {
	token_file<<"<BITOP, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <BITOP> Lexeme "<<yytext<<" found"<<endl;  
}

"!" {
	token_file<<"<NOT, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <NOT> Lexeme "<<yytext<<" found"<<endl;  
}

"(" {
	token_file<<"<LPAREN, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <LPAREN> Lexeme "<<yytext<<" found"<<endl;  
}

")" {
	token_file<<"<RPAREN, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <RPAREN> Lexeme "<<yytext<<" found"<<endl;  
}

"{" {
	token_file<<"<LCURL, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <LCURL> Lexeme "<<yytext<<" found"<<endl;  

	 id++;
	 symbol_table.enter_scope(id,bucket_size,log_file);
}

"}" {
	token_file<<"<RCURL, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <RCURL> Lexeme "<<yytext<<" found"<<endl;  
	 symbol_table.exit_scope(log_file);
}
"[" {
	token_file<<"<LSQUARE, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <LSQUARE> Lexeme "<<yytext<<" found"<<endl;  
}
"]" {
	token_file<<"<RSQUARE, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <RSQUARE> Lexeme "<<yytext<<" found"<<endl;  
}
"," {
	token_file<<"<COMMA, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <COMMA> Lexeme "<<yytext<<" found"<<endl;  
}
";" {
	token_file<<"<SEMICOLON, "<<yytext<<">"<<endl;
	 log_file<< "Line# "<<count_line<<": Token <SEMICOLON> Lexeme "<<yytext<<" found"<<endl;  
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

int main(int argc, char* argv[]) {
	if(argc!=2){
		printf("Please provide input file name and try again\n");
		return 0;
	}
	
	FILE *fin=fopen(argv[1],"r");

	if(fin == NULL){
		printf("Cannot open specified file\n");
		return 0;
	}
	
	log_file.open("log.txt");
	token_file.open("token.txt");

	yyin= fin;
	yylex();
	symbol_table.print_all(log_file);

    log_file<<"Total lines: "<<count_line<<endl;
	log_file<< "Total errors: "<<error_num<<endl;
	fclose(yyin);
	token_file.close();
	log_file.close();
	return 0;
}
