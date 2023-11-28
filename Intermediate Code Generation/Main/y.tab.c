/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "1905050.y"

#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include "1905050_symbolTable.h"

using namespace std;

int yyparse(void);
int yylex(void);
extern FILE *yyin;


extern symbolTable symbol_table;
extern int count_line;
extern int error_num;

extern ofstream log_file;
ofstream error_file;
ofstream parseTree_file;

extern string UpperCase(string s);


vector<pair<string,string>> parameter_list;
vector<symbolInfo*> argument_list;
string function_name, function_type;
string variable_type;
bool isFunctionDefined = false;
bool isFunctionDeclared = false;
bool isIdDeclared = false;
bool functionReturnFound = false;
bool isErrorFunction = false;

//parse tree

string parse_line;

int scope_id = 1;
int bucketSize = 11;


int label_count = 1;
int temp_count = 0;
int asm_LineCount = 0;
int asm_CS_endLine = 0;
int asm_DS_endLine = 0;



string generateLabel(){
    string label;
	label = "L_" + to_string(label_count);
	label_count++;
	return label;
}

string new_temp(){
    string temp;
	temp = "t" + to_string(temp_count);
	temp_count++;
	return temp;
}

void generateError(string s){
	error_file << "Line# " << count_line << ": " << s << endl;
	error_num++;
}

void yyerror(const char *s){

	//write your code
	//error_file << "Line# " << count_line << ": Syntax error" << s << endl;
	//generateError("Syntax error "+string(s));
}


int string_line_count(string str){
	int line_count = 1;
	//count lines in string with new lines
	for(int i=0; i<str.size(); i++){
		if(str[i] == '\n'){
			line_count++;
		}
	}

	return line_count;
}

void addCS_endLine(int count){
	asm_CS_endLine += count;
	asm_LineCount += count;
}


void write(string file_name, string str,bool isAppend = false){
	ofstream file;
	if(isAppend){
		file.open(file_name, ios::app);
	}
	else{
		file.open(file_name);
	}
	file << str << endl;
	file.close();
}

void writeMiddle(string file_name, string str,int line){
	//make new file named "temp.asm"
	//copy the code from "code.asm" upto line
	//then append the str and append the rest of "code.asm" to "temp.asm"
	//copy "temp.asm" to "code.asm"

	ofstream temp_file;
	temp_file.open("temp.asm");

	ifstream asm_file;
	asm_file.open("code.asm", ios::in);

	string line_str;
	int line_count = 0;
    bool written = false;

	while(getline(asm_file, line_str)){
		//line_count++;
		
		if(line_count == line){
			
			temp_file << str << endl;
			written = true;
		}
		temp_file << line_str << endl;
		line_count++;
	}

    if(written == false){
		temp_file << str << endl;
			written = true;
	}

	//remove code.asm and rename temp.asm to code.asm

	asm_file.close();
	temp_file.close();

	remove("code.asm");
	rename("temp.asm", "code.asm");
	
}



void writeCS(string str){
	//cout<<asm_CS_endLine<<endl;
	
	writeMiddle("code.asm", str, asm_CS_endLine);
	addCS_endLine(string_line_count(str));
	
}


bool checkVoid(string type){
	//error_file<<type<<endl;
	if(type == "CONST_VOID"){
		generateError("Void cannot be used in expression ");
		return true;
	}
	return false;
}

void checkFunctionDeclaration(string name, string type){
	function_name = name;
	function_type = type;
	symbolInfo* symbol = new symbolInfo(name, "FUNCTION");
	symbol->setReturnType(type);
	symbol->setIdStructure(type);
	symbol->setIsFunction(true);
	bool inserted = symbol_table.Insert(*symbol,log_file);

	//inserted
	if(inserted){
	   symbol->setReturnType(type);

	   //add parameter_list to symbol and remove the list
	   for(int i=0; i<parameter_list.size(); i++){
	   		//symbolInfo* parameter = new symbolInfo(parameter_list[i].first, parameter_list[i].second);
			string str = UpperCase(parameter_list[i].second);
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}
	   					
			symbol->addParameter(str);
	   }
	   //parameter_list.clear();
		
	}
	//couldn't be inserted
	else{
        symbolInfo* s = symbol_table.LookUp(name, log_file);
		if(s->getIsFunction()){
            isFunctionDeclared = true;

			if(s->getIsDefined()){
				isFunctionDefined = true;
			}
			else if(s->getIdStructure()!=type){
				isErrorFunction = true;
			}
		}
		else{
			generateError("'" + name + "' redeclared as different kind of symbol");
			isIdDeclared = true;
			isErrorFunction = true;
		}
	}
}

void checkFunctionDefinition(){
	if(isIdDeclared){
		//generateError("Multiple Declaration of " + name);
		return;
	}
	else if(isFunctionDefined){
		generateError("Multiple definition of the function " + function_name);
		return;
	}
	else if(isErrorFunction){
		return;
	}
	
	symbolInfo* symbol = symbol_table.LookUp_current(function_name, log_file);
	symbol->setIsDefined(true);

	if(symbol->getReturnType() != function_type && symbol->getReturnType() != "CONST_VOID" && symbol->getName() != "main" && functionReturnFound == false){
		//error_file<<symbol->getReturnType()<<" "<<function_type<<endl;
		generateError("Function defintion of non void return type must have a return statement");
	}
}

void handleTypeCheck(symbolInfo* left_symbol, symbolInfo* right_symbol ,string err_message){

    if(left_symbol->getType() == "undeclared" || right_symbol->getType() == "undeclared"){
		generateError(err_message);
	}
	else if(left_symbol->getType() != right_symbol->getType()){
		if(left_symbol->getType() == "ARRAY"){
			generateError(err_message+", "+left_symbol->getName()+" is an array");
		}
		else if(right_symbol->getType() == "ARRAY"){
			generateError(err_message+", "+right_symbol->getName()+" is an array");
		}
		else{
			generateError(err_message);
		}
	}
}


string HandleTypeCast(symbolInfo* left_symbol, symbolInfo* right_symbol, string err_message){
	if(left_symbol->getType() == "undeclared" || right_symbol->getType() == "undeclared"){
		generateError(err_message);
	}
	else if(left_symbol->getType() != right_symbol->getType()){
		if(left_symbol->getType() == "ARRAY"){
			handleTypeCheck(left_symbol, right_symbol, err_message);
		}
		else if(right_symbol->getType() == "ARRAY"){
			handleTypeCheck(left_symbol, right_symbol, err_message);
		}
	
		else if(left_symbol->getType() == "CONST_FLOAT"){
                     return "CONST_FLOAT";
		}

		else if(left_symbol->getType() == "CONST_INT" && (right_symbol->getType()=="CONST_INT" || right_symbol->getType()=="CONST_CHAR" ) ){
               return "CONST_INT";
		}

		else{
			handleTypeCheck(left_symbol, right_symbol, err_message);
		}
	}
	return left_symbol->getType();
}

string HandleTypeCastAvoidSide(symbolInfo* left_symbol, symbolInfo* right_symbol, string err_message){
	if(left_symbol->getType() == "undeclared" || right_symbol->getType() == "undeclared"){
		generateError(err_message);
	}
	else if(left_symbol->getType() != right_symbol->getType()){
		if(left_symbol->getType() == "ARRAY"){
			handleTypeCheck(left_symbol, right_symbol, err_message);
		}
		else if(right_symbol->getType() == "ARRAY"){
			handleTypeCheck(left_symbol, right_symbol, err_message);
		}
	
		else if(left_symbol->getType() == "CONST_FLOAT" || right_symbol->getType() == "CONST_FLOAT"){
                     return "CONST_FLOAT";
		}

		else if(left_symbol->getType() == "CONST_INT" || right_symbol->getType() == "CONST_INT" ){
               return "CONST_INT";
		}

		else{
			handleTypeCheck(left_symbol, right_symbol, err_message);
		}
	}
	return left_symbol->getType();
}



void insertVariable(string name, string type, int array_size = 0){
	if(symbol_table.hasFunctionDefined(name,log_file)){
		generateError("Multiple Declaration of " + name + " as a function");
	}
	else if(symbol_table.LookUp_current(name, log_file) != NULL){
		//check if type contains [] or not
		symbolInfo* s = symbol_table.LookUp(name, log_file);
		if((type.find("[]") != string::npos)&& !(s->getIsArray())){
			generateError("'" + name + "' redeclared as different kind of symbol");
		}
		else if(!(type.find("[]") != string::npos) && s->getIsArray()){
			generateError("'" + name + "' redeclared as different kind of symbol");
		}
		else{
			generateError("Conflicting types for'" + name +"'");
		}
	}
	else{
		if(type == "CONST_VOID" || type == "void" || type == "VOID"){
			//generateError("Variable can not be of type void");
		}
	
		//check if type contains [] or not
		else if(type.find("[]") != string::npos){
			//array
			
			string str = UpperCase(type.substr(0, type.size()-2));
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}
			string idStructure = str;
			type = "ARRAY";
			symbolInfo* symbol = new symbolInfo(name, type);
			symbol->setIsArray(true);
            symbol->setIdStructure(idStructure);
			symbol->setArraySize(array_size);

			symbol_table.Insert(*symbol,log_file);

			if(symbol_table.get_current_id() != 1 && array_size > 0){
				writeCS("\tSUB SP, "+to_string(array_size*2));
			}
		}
		else{
			//variable
			string str = UpperCase(type);
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}
			string idStructure = str;
			type = "ID";
			symbolInfo* symbol = new symbolInfo(name, type);
			symbol->setIsArray(false);
			symbol->setIdStructure(idStructure);
			symbol->setIsID(true);

			symbol_table.Insert(*symbol,log_file);

			if(symbol_table.get_current_id() != 1 && array_size == 0){
				writeCS("\tPUSH BX");
			}
		}

	}
}

void printTree(symbolInfo* root, int level){

	for(int i=0; i<level; i++){
		parseTree_file <<" ";
	}
	if(root->getIsLeaf()==true){
        //parseTree_file << root->getName() << "	 " << "Line" << endl;
		parseTree_file << root->getType() << " : " << root->getName() <<"	"<<"<Line: "<<root->getStartLine()<<">"<< endl;
		return;
	}
	else{
		parseTree_file << root->getParseLine() <<" 	"<<"<Line: "<<root->getStartLine()<<"-"<<root->getEndLine()<<">"<< endl;
	}

	for(int i=0; i<root->getChildren().size(); i++){
		printTree(root->getChildren()[i], level+1);
	}
}

void startAsmFile(){
    ofstream asm_file;
	asm_file.open("code.asm");

	asm_file << ".MODEL SMALL" << endl;
	asm_file << ".STACK 1000H" << endl;
	asm_file << ".DATA" << endl;
	asm_file << "\tCR EQU 0DH\n\
	LF EQU 0AH\n" << endl;

	asm_LineCount += 6;
	asm_DS_endLine = asm_LineCount;

	asm_file << ".Code" << endl;
	asm_LineCount += 1;
	asm_CS_endLine = asm_LineCount;

	asm_file.close();	
}

void endAsmFile(){
    ofstream asm_file;
	asm_file.open("code.asm", ios::app);

	asm_file << "END main" << endl;
	asm_LineCount += 1;

	asm_file.close();
}

//Global variable and array helper functions
void global_var_declaration(string name, string type,string array_size = "1"){
	string str = "\t" + name + " DW "+array_size+" DUP (0000H)" ;
	asm_LineCount += 1;
	asm_CS_endLine += 1;

	writeMiddle("code.asm", str, asm_DS_endLine);
	asm_DS_endLine += 1;
}

void start_main(){
	string str = "\tMOV AX, @DATA\n\
	MOV DS, AX\n\
	PUSH BP\n\
	MOV BP, SP";

	write("code.asm",str,true);
	addCS_endLine(4);
}

void end_main(){
	string str = "\tMOV AX,4CH\n\
	INT 21H";
	write("code.asm",str,true);
	addCS_endLine(2);
}

void procedure_declaration(string func_name){
     symbol_table.reset_currentScopeIdCount();

    string str = func_name + " PROC";
	write("code.asm",str,true);
	addCS_endLine(1);

	if(func_name == "main"){
		start_main();
	}
	else{

	string str = "\tPUSH BP\n\
	MOV BP, SP";
	write("code.asm",str,true);
	addCS_endLine(2);
	}
}

void procedure_end(string func_name, int param_count){

	//cout << "function type: " << function_type << endl;
    if(func_name == "main"){
		end_main();
	}
	else if(function_type == "void" || function_type == "VOID" || function_type == "CONST_VOID"){
		string str = "\tMOV SP, BP\n";
		str += "POP BP\n\
		RET " + (param_count ? to_string(param_count*2) : "") ;
		writeCS(str);
	}
		write("code.asm",func_name+" ENDP",true);
		addCS_endLine(1);
	
}


string JumpConverter(string str){
	if(str == "=="){
		return "JE";
	}
	else if(str == "!="){
		return "JNE";
	}
	else if(str == ">"){
		return "JG";
	}
	else if(str == "<"){
		return "JL";
	}
	else if(str == ">="){
		return "JGE";
	}
	else if(str == "<="){
		return "JLE";
	}
	else{
		return "ERROR";
	}
}


//All sorts of write codes needed



void write_printProc(){
	string str ="";
	/*str += "print_output proc  ;print what is in ax\n\
    push ax\n\
    push bx\n\
    push cx\n\
    push dx\n\
    push si\n\
    lea si,number\n\
    mov bx,10\n\
    add si,4\n\
    cmp ax,0\n\
    jnge negate\n\
    print:\n\
    xor dx,dx\n\
    div bx\n\
    mov [si],dl\n\
    add [si],'0'\n\
    dec si\n\
    cmp ax,0\n\
    jne print\n\
    inc si\n\
    lea dx,si\n\
    mov ah,9\n\
    int 21h\n\
    pop si\n\
    pop dx\n\
    pop cx\n\
    pop bx\n\
    pop ax\n\
    ret\n\
    negate:\n\
    push ax\n\
    mov ah,2\n\
    mov dl,'-'\n\
    int 21h\n\
    pop ax\n\
    neg ax\n\
    jmp print\n\
print_output endp";
*/
    str += "\tPRINT_OUTPUT PROC NEAR\n\
        PUSH BP             ;Saving BP\n\
        MOV BP, SP          ;BP points to the top of the stack\n\
        MOV BX, [BP+4]    ;The number to be printed\n\
        ;if(BX < -1) then the number is positive\n\
        CMP BX, 0\n\
        JGE POSITIVE\n\
        ;else, the number is negative\n\
        MOV AH, 2           \n\
        MOV DL, '-'         ;Print a '-' sign\n\
        INT 21H\n\
        NEG BX              ;make BX positive\n\
        POSITIVE:\n\
        MOV AX, BX\n\
        MOV CX, 0        ;Initialize character count\n\
        PUSH_WHILE:\n\
            XOR DX, DX  ;clear DX\n\
            MOV BX, 10  ;BX has the divisor //// AX has the dividend\n\
            DIV BX\n\
            ;quotient is in AX and remainder is in DX\n\
            PUSH DX     ;Division by 10 will have a remainder less than 8 bits\n\
            INC CX       ;CX++\n\
            ;if(AX == 0) then break the loop\n\
            CMP AX, 0\n\
            JE END_PUSH_WHILE\n\
            ;else continue\n\
            JMP PUSH_WHILE\n\
        END_PUSH_WHILE:\n\
        MOV AH, 2\n\
        POP_WHILE:\n\
            POP DX      ;Division by 10 will have a remaainder less than 8 bits\n\
            ADD DL, '0'\n\
            INT 21H     ;So DL will have the desired character\n\
            DEC CX       ;CX--\n\
            ;if(CX <= 0) then end loop\n\
            CMP CX, 0\n\
            JLE END_POP_WHILE\n\
            ;else continue\n\
            JMP POP_WHILE\n\
        END_POP_WHILE:\n\
        ;Print newline\n\
        MOV DL, 0DH\n\
        INT 21H\n\
        MOV DL, 0AH\n\
        INT 21H\n\
        POP BP          ; Restore BP\n\
        RET 2\n\
    PRINT_OUTPUT ENDP";

	write("code.asm",str,true);
	addCS_endLine(string_line_count(str) + 1);
}

void write_newLineProc(){
	string str ="";
	/*str += "new_line proc\n\
    push ax\n\
    push dx\n\
    mov ah,2\n\
    mov dl,cr\n\
    int 21h\n\
    mov ah,2\n\
    mov dl,lf\n\
    int 21h\n\
    pop dx\n\
    pop ax\n\
    ret\n\
new_line endp";
*/
str += "\n\tNEW_LINE PROC\r\n\
						 ; PRINTS A NEW LINE\r\n\
						 PUSH AX\r\n\
						 PUSH DX\r\n\
						 MOV AH, 2\r\n\
						 MOV DL, cr\r\n\
						 INT 21H\r\n\
						 MOV AH, 2\r\n\
						 MOV DL, lf\r\n\
						 INT 21H\r\n\
						 POP DX\r\n\
						 POP AX\r\n\
						 RET\r\n\
						 NEW_LINE ENDP\r\n"; 

	write("code.asm",str,true);
	addCS_endLine(string_line_count(str) + 1);
}



//Optimization

//Takes an string and a delimiter and returns a vector of strings
//that are split by the delimiter.

vector<string> instruction_split(const string &s, char delim) {
	vector<string> elems;
	string item;
	item = "";
	for(int i=0;i<s.size();i++){
		if(s[i] == delim || s[i] == '\t'){
			if (item != ""){
				elems.push_back(item);
				item = "";
			}
		}
		else{
			item += s[i];
		}
	}

	if (item != ""){
		elems.push_back(item);
	}
	return elems;
}

bool getIsJump(string str){
	if(str == "JMP" || str == "JZ" || str == "JNZ" || str == "JG" || str == "JGE" || str == "JL" || str == "JLE")
		return true;
	else
		return false;
}

bool asm_optimization(string input_file, string output_file){
	ifstream input(input_file);
	ofstream output(output_file);
	string line_current;
	string line_previous;
	line_previous = "";
    vector<string> instruction_previous;

	bool gotOptimized = false;

	while(getline(input,line_current)){
		vector<string> instruction_current = instruction_split(line_current,' ');
		if(instruction_current.size() == 0)
		    continue;
        else if(instruction_current[0][0] == ';'){
			output << line_current << endl;
			continue;
		}

		if(instruction_previous.size()==0){
			//output << line_current << endl;
			line_previous = line_current;
			instruction_previous = instruction_current;
			continue;
		}

		if(instruction_previous[0] == "MOV" && instruction_current[0] == "MOV"){


			instruction_current[1] = instruction_split(instruction_current[1], ',')[0];
			instruction_previous[1] = instruction_split(instruction_previous[1], ',')[0];

			if(instruction_previous[1] == instruction_current[2] && instruction_previous[2] == instruction_current[1]){
				gotOptimized = true;
				output << line_previous << endl;
				output << "; " << line_current << endl;

				line_previous = "";
				instruction_previous.clear();
			}

			else if(instruction_previous[1] == instruction_current[1]){
				output << "; " << line_previous << endl;
				line_previous = line_current;
				instruction_previous = instruction_current;
				gotOptimized = true;
			}

			else {
				output << line_previous << endl;
				line_previous = line_current;
				instruction_previous = instruction_current;
			}
	
		}

       //cout << instruction_previous[0] << " " << instruction_current[0] << endl;

		//check if previous instruction is pop and current is push
        else if(instruction_current[0] == "POP" && instruction_previous[0] == "PUSH"){
			output << "; " << line_previous << endl;
			output << "; " << line_current << endl;

			gotOptimized = true;

			if(instruction_current[1] != instruction_previous[1]){
                   output << "\tMOV " << instruction_current[1] << ", " << instruction_previous[1] << endl;
			}

			instruction_previous.clear();
			line_previous = "";

		}

		// if the current instruction is MOV and both the registers or address are the same then just ignore the instruction
		else if(instruction_current[0] == "MOV" && instruction_current[1].substr(0,instruction_current[1].length()-1) == instruction_current[2]){
			//output << "; " << line_current << endl;
			gotOptimized = true;
		}


		//check if previous instruction is jump and current instruction is a label. If the label is the same as the jump then remove the jump
		else if(getIsJump(instruction_previous[0]) && instruction_current[0][instruction_current[0].length()-1] == ':'){
			if(instruction_previous[1] == instruction_current[0].substr(0,instruction_current[0].length()-1)){
				output << "; " << line_previous << endl;
				output << line_current << endl;

				line_previous = "";
				instruction_previous.clear();
				gotOptimized = true;
			}
			else{
				output << line_previous << endl;
				line_previous = line_current;
				instruction_previous = instruction_current;
			}
		}


		//check if current instruction is addop and one of its operands is 0. If yes then ignore the instruction
	 	else if(instruction_current[0] == "ADD" || instruction_current[0] == "SUB" ){
			if(instruction_current[2] == "0"){
				output << "; " << line_current << endl;
				gotOptimized = true;
			}
			else{
				output << line_previous << endl;
				line_previous = line_current;
				instruction_previous = instruction_current;
			}
		}		

		//check if current instruction in mulop and one of its operands is 1. If yes then ignore the instruction
		else if(instruction_current[0] == "IMUL" || instruction_current[0] == "IDIV" ){
			if(instruction_current[1] == "1"){
				output << "; " << line_current << endl;
				gotOptimized = true;
			}
			else{
				output << line_previous << endl;
				line_previous = line_current;
				instruction_previous = instruction_current;
			}
		}


		else {
				output << line_previous << endl;
				line_previous = line_current;
				instruction_previous = instruction_current;
			}
	
	}
	
	input.close();
	output.close();
	return gotOptimized;
}


void final_optimization(){

	int pass = 0;
	bool gotOptimized = false;

	gotOptimized = asm_optimization("code.asm","temp_op.asm");
	rename("temp_op.asm","optimized_code.asm");
	pass++;

	while(gotOptimized){
		gotOptimized = asm_optimization("optimized_code.asm","temp_op.asm");
		remove("optimized_code.asm");
		rename("temp_op.asm","optimized_code.asm");
		pass++;
	} 
	cout << "Optimization done in " << pass << " passes." << endl;
}


#line 928 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IF = 258,                      /* IF  */
    ELSE = 259,                    /* ELSE  */
    FOR = 260,                     /* FOR  */
    WHILE = 261,                   /* WHILE  */
    DO = 262,                      /* DO  */
    BREAK = 263,                   /* BREAK  */
    PRINTLN = 264,                 /* PRINTLN  */
    RETURN = 265,                  /* RETURN  */
    INT = 266,                     /* INT  */
    CHAR = 267,                    /* CHAR  */
    FLOAT = 268,                   /* FLOAT  */
    DOUBLE = 269,                  /* DOUBLE  */
    VOID = 270,                    /* VOID  */
    CONTINUE = 271,                /* CONTINUE  */
    CASE = 272,                    /* CASE  */
    DEFAULT = 273,                 /* DEFAULT  */
    LPAREN = 274,                  /* LPAREN  */
    RPAREN = 275,                  /* RPAREN  */
    LCURL = 276,                   /* LCURL  */
    RCURL = 277,                   /* RCURL  */
    LTHIRD = 278,                  /* LTHIRD  */
    RTHIRD = 279,                  /* RTHIRD  */
    COMMA = 280,                   /* COMMA  */
    SEMICOLON = 281,               /* SEMICOLON  */
    ID = 282,                      /* ID  */
    CONST_INT = 283,               /* CONST_INT  */
    CONST_FLOAT = 284,             /* CONST_FLOAT  */
    ADDOP = 285,                   /* ADDOP  */
    MULOP = 286,                   /* MULOP  */
    ASSIGNOP = 287,                /* ASSIGNOP  */
    RELOP = 288,                   /* RELOP  */
    LOGICOP = 289,                 /* LOGICOP  */
    NOT = 290,                     /* NOT  */
    INCOP = 291,                   /* INCOP  */
    DECOP = 292,                   /* DECOP  */
    INCOP_PRE = 293,               /* INCOP_PRE  */
    DECOP_PRE = 294,               /* DECOP_PRE  */
    INCOP_POST = 295,              /* INCOP_POST  */
    DECOP_POST = 296,              /* DECOP_POST  */
    ALTERNATIVE_ELSE = 297         /* ALTERNATIVE_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define IF 258
#define ELSE 259
#define FOR 260
#define WHILE 261
#define DO 262
#define BREAK 263
#define PRINTLN 264
#define RETURN 265
#define INT 266
#define CHAR 267
#define FLOAT 268
#define DOUBLE 269
#define VOID 270
#define CONTINUE 271
#define CASE 272
#define DEFAULT 273
#define LPAREN 274
#define RPAREN 275
#define LCURL 276
#define RCURL 277
#define LTHIRD 278
#define RTHIRD 279
#define COMMA 280
#define SEMICOLON 281
#define ID 282
#define CONST_INT 283
#define CONST_FLOAT 284
#define ADDOP 285
#define MULOP 286
#define ASSIGNOP 287
#define RELOP 288
#define LOGICOP 289
#define NOT 290
#define INCOP 291
#define DECOP 292
#define INCOP_PRE 293
#define DECOP_PRE 294
#define INCOP_POST 295
#define DECOP_POST 296
#define ALTERNATIVE_ELSE 297

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 860 "1905050.y"

	int int_val;
    symbolInfo* symbol_info;

#line 1070 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IF = 3,                         /* IF  */
  YYSYMBOL_ELSE = 4,                       /* ELSE  */
  YYSYMBOL_FOR = 5,                        /* FOR  */
  YYSYMBOL_WHILE = 6,                      /* WHILE  */
  YYSYMBOL_DO = 7,                         /* DO  */
  YYSYMBOL_BREAK = 8,                      /* BREAK  */
  YYSYMBOL_PRINTLN = 9,                    /* PRINTLN  */
  YYSYMBOL_RETURN = 10,                    /* RETURN  */
  YYSYMBOL_INT = 11,                       /* INT  */
  YYSYMBOL_CHAR = 12,                      /* CHAR  */
  YYSYMBOL_FLOAT = 13,                     /* FLOAT  */
  YYSYMBOL_DOUBLE = 14,                    /* DOUBLE  */
  YYSYMBOL_VOID = 15,                      /* VOID  */
  YYSYMBOL_CONTINUE = 16,                  /* CONTINUE  */
  YYSYMBOL_CASE = 17,                      /* CASE  */
  YYSYMBOL_DEFAULT = 18,                   /* DEFAULT  */
  YYSYMBOL_LPAREN = 19,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 20,                    /* RPAREN  */
  YYSYMBOL_LCURL = 21,                     /* LCURL  */
  YYSYMBOL_RCURL = 22,                     /* RCURL  */
  YYSYMBOL_LTHIRD = 23,                    /* LTHIRD  */
  YYSYMBOL_RTHIRD = 24,                    /* RTHIRD  */
  YYSYMBOL_COMMA = 25,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 26,                 /* SEMICOLON  */
  YYSYMBOL_ID = 27,                        /* ID  */
  YYSYMBOL_CONST_INT = 28,                 /* CONST_INT  */
  YYSYMBOL_CONST_FLOAT = 29,               /* CONST_FLOAT  */
  YYSYMBOL_ADDOP = 30,                     /* ADDOP  */
  YYSYMBOL_MULOP = 31,                     /* MULOP  */
  YYSYMBOL_ASSIGNOP = 32,                  /* ASSIGNOP  */
  YYSYMBOL_RELOP = 33,                     /* RELOP  */
  YYSYMBOL_LOGICOP = 34,                   /* LOGICOP  */
  YYSYMBOL_NOT = 35,                       /* NOT  */
  YYSYMBOL_INCOP = 36,                     /* INCOP  */
  YYSYMBOL_DECOP = 37,                     /* DECOP  */
  YYSYMBOL_INCOP_PRE = 38,                 /* INCOP_PRE  */
  YYSYMBOL_DECOP_PRE = 39,                 /* DECOP_PRE  */
  YYSYMBOL_INCOP_POST = 40,                /* INCOP_POST  */
  YYSYMBOL_DECOP_POST = 41,                /* DECOP_POST  */
  YYSYMBOL_ALTERNATIVE_ELSE = 42,          /* ALTERNATIVE_ELSE  */
  YYSYMBOL_YYACCEPT = 43,                  /* $accept  */
  YYSYMBOL_start = 44,                     /* start  */
  YYSYMBOL_program = 45,                   /* program  */
  YYSYMBOL_unit = 46,                      /* unit  */
  YYSYMBOL_func_declaration = 47,          /* func_declaration  */
  YYSYMBOL_48_1 = 48,                      /* $@1  */
  YYSYMBOL_49_2 = 49,                      /* $@2  */
  YYSYMBOL_func_definition = 50,           /* func_definition  */
  YYSYMBOL_51_3 = 51,                      /* $@3  */
  YYSYMBOL_52_4 = 52,                      /* $@4  */
  YYSYMBOL_new_scope = 53,                 /* new_scope  */
  YYSYMBOL_parameter_list = 54,            /* parameter_list  */
  YYSYMBOL_compound_statement = 55,        /* compound_statement  */
  YYSYMBOL_var_declaration = 56,           /* var_declaration  */
  YYSYMBOL_type_specifier = 57,            /* type_specifier  */
  YYSYMBOL_declaration_list = 58,          /* declaration_list  */
  YYSYMBOL_statements = 59,                /* statements  */
  YYSYMBOL_executeIf = 60,                 /* executeIf  */
  YYSYMBOL_statement = 61,                 /* statement  */
  YYSYMBOL_62_5 = 62,                      /* @5  */
  YYSYMBOL_63_6 = 63,                      /* @6  */
  YYSYMBOL_64_7 = 64,                      /* @7  */
  YYSYMBOL_65_8 = 65,                      /* $@8  */
  YYSYMBOL_66_9 = 66,                      /* @9  */
  YYSYMBOL_67_10 = 67,                     /* @10  */
  YYSYMBOL_68_11 = 68,                     /* @11  */
  YYSYMBOL_expression_statement = 69,      /* expression_statement  */
  YYSYMBOL_variable = 70,                  /* variable  */
  YYSYMBOL_expression = 71,                /* expression  */
  YYSYMBOL_logic_expression = 72,          /* logic_expression  */
  YYSYMBOL_rel_expression = 73,            /* rel_expression  */
  YYSYMBOL_simple_expression = 74,         /* simple_expression  */
  YYSYMBOL_term = 75,                      /* term  */
  YYSYMBOL_unary_expression = 76,          /* unary_expression  */
  YYSYMBOL_factor = 77,                    /* factor  */
  YYSYMBOL_argument_list = 78,             /* argument_list  */
  YYSYMBOL_arguments = 79                  /* arguments  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   183

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  37
/* YYNRULES -- Number of rules.  */
#define YYNRULES  82
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  140

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   891,   891,   908,   921,   934,   945,   956,   969,   969,
    1015,  1015,  1063,  1063,  1098,  1098,  1134,  1197,  1233,  1269,
    1299,  1323,  1348,  1364,  1382,  1403,  1426,  1439,  1451,  1465,
    1493,  1521,  1547,  1574,  1586,  1601,  1631,  1643,  1656,  1669,
    1681,  1699,  1702,  1669,  1747,  1767,  1767,  1804,  1814,  1804,
    1856,  1893,  1952,  1964,  1982,  2000,  2040,  2118,  2131,  2224,
    2237,  2316,  2329,  2382,  2401,  2455,  2473,  2537,  2560,  2587,
    2607,  2630,  2701,  2715,  2729,  2743,  2805,  2860,  2921,  2978,
    2990,  3004,  3020
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IF", "ELSE", "FOR",
  "WHILE", "DO", "BREAK", "PRINTLN", "RETURN", "INT", "CHAR", "FLOAT",
  "DOUBLE", "VOID", "CONTINUE", "CASE", "DEFAULT", "LPAREN", "RPAREN",
  "LCURL", "RCURL", "LTHIRD", "RTHIRD", "COMMA", "SEMICOLON", "ID",
  "CONST_INT", "CONST_FLOAT", "ADDOP", "MULOP", "ASSIGNOP", "RELOP",
  "LOGICOP", "NOT", "INCOP", "DECOP", "INCOP_PRE", "DECOP_PRE",
  "INCOP_POST", "DECOP_POST", "ALTERNATIVE_ELSE", "$accept", "start",
  "program", "unit", "func_declaration", "$@1", "$@2", "func_definition",
  "$@3", "$@4", "new_scope", "parameter_list", "compound_statement",
  "var_declaration", "type_specifier", "declaration_list", "statements",
  "executeIf", "statement", "@5", "@6", "@7", "$@8", "@9", "@10", "@11",
  "expression_statement", "variable", "expression", "logic_expression",
  "rel_expression", "simple_expression", "term", "unary_expression",
  "factor", "argument_list", "arguments", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-72)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-21)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     104,   -72,   -72,   -72,    12,   104,   -72,   -72,   -72,   -72,
      22,   -72,   -72,    -6,    43,    34,   -72,    73,     1,    -5,
     -72,    54,    10,    14,     9,    57,    59,    70,    71,   104,
     -72,   -72,   -72,    72,   -72,   -72,   -72,    75,    70,    83,
      13,    68,   -72,   -72,   -72,   -72,   101,   102,   -72,   107,
     142,   142,   -72,   -72,    88,   -72,   -72,   142,   142,    96,
      96,   -72,   -72,    25,   103,   -72,   -72,   -18,    16,   -72,
      93,    69,    97,   -72,   -72,   142,    28,   115,   108,   110,
     117,   142,   142,    33,   -72,   -72,   119,   -72,   -72,   122,
     -72,   -72,   142,   -72,   -72,   -72,   -72,   142,   142,   142,
     142,   126,   -72,   142,   130,   -72,   -72,   -72,   132,   129,
     131,   -72,   -72,    97,   128,   -72,   -72,    28,   -72,   134,
     -72,   142,   -72,   138,   -72,   136,   -72,   -72,   158,   -72,
     138,   -72,   142,   -72,   138,   -72,   -72,   143,   138,   -72
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    26,    27,    28,     0,     2,     4,     6,     7,     5,
       0,     1,     3,     0,    31,     0,    25,     0,     0,     0,
      24,    10,     0,     0,     0,    29,     0,     0,     8,     0,
      21,    18,    32,     0,    11,    16,    15,     0,     0,    19,
       0,     0,     9,    13,    17,    30,     0,     0,    47,     0,
       0,     0,    23,    52,    55,    73,    74,     0,     0,     0,
       0,    38,    36,     0,     0,    33,    37,    70,     0,    57,
      59,    61,    63,    65,    69,     0,     0,     0,     0,     0,
       0,    80,     0,    70,    67,    68,    55,    76,    78,    31,
      22,    34,     0,    75,    77,    54,    53,     0,     0,     0,
       0,     0,    39,     0,     0,    51,    72,    82,     0,    79,
       0,    58,    60,    64,    62,    66,    35,     0,    48,     0,
      71,     0,    56,     0,    40,     0,    50,    81,    44,    41,
       0,    45,     0,    49,     0,    42,    46,     0,     0,    43
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -72,   -72,   -72,   171,   -72,   -72,   -72,   -72,   -72,   -72,
     -72,   -72,   -14,    31,    11,   -72,   -72,   -72,   -62,   -72,
     -72,   -72,   -72,   -72,   -72,   -72,   -69,   -54,   -50,   -71,
      84,    81,    85,   -49,   -72,   -72,   -72
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     4,     5,     6,     7,    37,    26,     8,    38,    27,
      41,    22,    61,    62,    63,    15,    64,   123,    65,   117,
     129,   132,   137,   134,    77,   125,    66,    67,    68,    69,
      70,    71,    72,    73,    74,   108,   109
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      79,    80,    91,    83,    83,    87,    88,   102,    84,    85,
     107,    10,    11,    36,    92,    30,    10,    95,    93,    94,
      16,   111,    25,    13,    43,   101,    13,    83,    23,    24,
      28,     9,   110,    32,   -20,    29,     9,    45,    83,   -20,
      39,    31,    96,    83,    83,    83,    83,    51,   124,    14,
     127,   115,    89,   118,    53,    54,    55,    56,    57,    19,
      20,   128,    17,    58,    59,    60,    18,    83,   133,    93,
      94,    46,   136,    47,    48,   -14,   139,    49,    50,     1,
      33,     2,   135,     3,     1,    34,     2,    51,     3,    35,
      52,    35,   -12,    21,    53,    54,    55,    56,    57,    98,
      40,    42,    99,    58,    59,    60,    46,    81,    47,    48,
      44,    82,    49,    50,     1,     1,     2,     2,     3,     3,
      75,    76,    51,    86,    35,    90,    78,    97,   100,    53,
      54,    55,    56,    57,   103,   104,   105,   106,    58,    59,
      60,    46,    82,    47,    48,    18,   116,    49,    50,     1,
     119,     2,   120,     3,   121,   122,   130,    51,    98,    35,
     126,    51,   131,   138,    53,    54,    55,    56,    57,    54,
      55,    56,    57,    58,    59,    60,    12,    58,    59,    60,
     114,   112,     0,   113
};

static const yytype_int16 yycheck[] =
{
      50,    51,    64,    57,    58,    59,    60,    76,    57,    58,
      81,     0,     0,    27,    32,     1,     5,     1,    36,    37,
      26,    92,    27,     1,    38,    75,     1,    81,    17,    28,
      20,     0,    82,    24,    20,    25,     5,    24,    92,    25,
      29,    27,    26,    97,    98,    99,   100,    19,   117,    27,
     121,   100,    27,   103,    26,    27,    28,    29,    30,    25,
      26,   123,    19,    35,    36,    37,    23,   121,   130,    36,
      37,     3,   134,     5,     6,    21,   138,     9,    10,    11,
      23,    13,   132,    15,    11,    26,    13,    19,    15,    21,
      22,    21,    21,    20,    26,    27,    28,    29,    30,    30,
      28,    26,    33,    35,    36,    37,     3,    19,     5,     6,
      27,    23,     9,    10,    11,    11,    13,    13,    15,    15,
      19,    19,    19,    27,    21,    22,    19,    34,    31,    26,
      27,    28,    29,    30,    19,    27,    26,    20,    35,    36,
      37,     3,    23,     5,     6,    23,    20,     9,    10,    11,
      20,    13,    20,    15,    25,    24,    20,    19,    30,    21,
      26,    19,     4,    20,    26,    27,    28,    29,    30,    27,
      28,    29,    30,    35,    36,    37,     5,    35,    36,    37,
      99,    97,    -1,    98
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    11,    13,    15,    44,    45,    46,    47,    50,    56,
      57,     0,    46,     1,    27,    58,    26,    19,    23,    25,
      26,    20,    54,    57,    28,    27,    49,    52,    20,    25,
       1,    27,    24,    23,    26,    21,    55,    48,    51,    57,
      28,    53,    26,    55,    27,    24,     3,     5,     6,     9,
      10,    19,    22,    26,    27,    28,    29,    30,    35,    36,
      37,    55,    56,    57,    59,    61,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    19,    19,    67,    19,    71,
      71,    19,    23,    70,    76,    76,    27,    70,    70,    27,
      22,    61,    32,    36,    37,     1,    26,    34,    30,    33,
      31,    71,    69,    19,    27,    26,    20,    72,    78,    79,
      71,    72,    73,    75,    74,    76,    20,    62,    71,    20,
      20,    25,    24,    60,    69,    68,    26,    72,    61,    63,
      20,     4,    64,    61,    66,    71,    61,    65,    20,    61
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    46,    48,    47,
      49,    47,    51,    50,    52,    50,    53,    54,    54,    54,
      54,    54,    55,    55,    56,    56,    57,    57,    57,    58,
      58,    58,    58,    59,    59,    60,    61,    61,    61,    62,
      63,    64,    65,    61,    61,    66,    61,    67,    68,    61,
      61,    61,    69,    69,    69,    70,    70,    71,    71,    72,
      72,    73,    73,    74,    74,    75,    75,    76,    76,    76,
      77,    77,    77,    77,    77,    77,    77,    77,    77,    78,
      78,    79,    79
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     0,     7,
       0,     6,     0,     7,     0,     6,     0,     4,     2,     3,
       1,     2,     4,     3,     3,     3,     1,     1,     1,     3,
       6,     1,     4,     1,     2,     0,     1,     1,     1,     0,
       0,     0,     0,    11,     6,     0,     9,     0,     0,     7,
       5,     3,     1,     2,     2,     1,     4,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     2,     2,     1,
       1,     4,     3,     1,     1,     2,     2,     2,     2,     1,
       0,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* start: program  */
#line 892 "1905050.y"
        {
		//write your code in this block in all the similar blocks below
        (yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "start : program " << endl;
		
		parse_line = "start : program";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		//printTree($$, 0);
		
	}
#line 2518 "y.tab.c"
    break;

  case 3: /* program: program unit  */
#line 908 "1905050.y"
                      {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName() , "variable");
		log_file<< "program : program unit " << endl;

		parse_line = "program : program unit";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 2536 "y.tab.c"
    break;

  case 4: /* program: unit  */
#line 921 "1905050.y"
              {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "program : unit " << endl;

		parse_line = "program : unit";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
	}
#line 2552 "y.tab.c"
    break;

  case 5: /* unit: var_declaration  */
#line 934 "1905050.y"
                      {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "unit : var_declaration  " << endl;

		parse_line = "unit : var_declaration";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
}
#line 2568 "y.tab.c"
    break;

  case 6: /* unit: func_declaration  */
#line 945 "1905050.y"
                           {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "unit : func_declaration " << endl;

		parse_line = "unit : func_declaration";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
}
#line 2584 "y.tab.c"
    break;

  case 7: /* unit: func_definition  */
#line 956 "1905050.y"
                          {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "unit : func_definition  " << endl;

		parse_line = "unit : func_definition";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
     }
#line 2600 "y.tab.c"
    break;

  case 8: /* $@1: %empty  */
#line 969 "1905050.y"
                                                                  { procedure_declaration((yyvsp[-3].symbol_info)->getName()); }
#line 2606 "y.tab.c"
    break;

  case 9: /* func_declaration: type_specifier ID LPAREN parameter_list RPAREN $@1 SEMICOLON  */
#line 969 "1905050.y"
                                                                                                                     {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-6].symbol_info)->getName()+" "+ (yyvsp[-5].symbol_info)->getName()+"("+ (yyvsp[-3].symbol_info)->getName()+")"+";", "variable");
		log_file<< "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON " << endl;

		parse_line = "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-6].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


             string str = UpperCase((yyvsp[-6].symbol_info)->getName());
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}
		string return_type = str;
		string name = (yyvsp[-5].symbol_info)->getName();

		checkFunctionDeclaration(name, return_type);

		if(isFunctionDeclared == true){
			generateError("'"+name+"' redeclared as different kind of symbol");
		}

        //clear the function declaration
		isFunctionDeclared = false;
		isFunctionDefined = false;
		isIdDeclared = false;
		isErrorFunction = false;

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();


}
#line 2657 "y.tab.c"
    break;

  case 10: /* $@2: %empty  */
#line 1015 "1905050.y"
                                                  { procedure_declaration((yyvsp[-2].symbol_info)->getName()); }
#line 2663 "y.tab.c"
    break;

  case 11: /* func_declaration: type_specifier ID LPAREN RPAREN $@2 SEMICOLON  */
#line 1015 "1905050.y"
                                                                                                     {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-5].symbol_info)->getName()+" "+ (yyvsp[-4].symbol_info)->getName()+"("+")"+";", "variable");
		log_file<< "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON " << endl;

		parse_line = "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-5].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		string str = UpperCase((yyvsp[-5].symbol_info)->getName());
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}

		string return_type = str;
		string name = (yyvsp[-4].symbol_info)->getName();

		checkFunctionDeclaration(name, return_type);

		if(isFunctionDeclared == true){
			generateError("Multiple Declaration of " + name);
		}

        //clear the function declaration
		isFunctionDeclared = false;
		isFunctionDefined = false;
		isIdDeclared = false;
		isErrorFunction = false;

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();

		}
#line 2713 "y.tab.c"
    break;

  case 12: /* $@3: %empty  */
#line 1063 "1905050.y"
                                                                 {string return_type = "CONST_"+UpperCase((yyvsp[-4].symbol_info)->getName());string name = (yyvsp[-3].symbol_info)->getName();checkFunctionDeclaration(name, return_type); procedure_declaration(name); }
#line 2719 "y.tab.c"
    break;

  case 13: /* func_definition: type_specifier ID LPAREN parameter_list RPAREN $@3 compound_statement  */
#line 1063 "1905050.y"
                                                                                                                                                                                                                                                  {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-6].symbol_info)->getName()+" "+ (yyvsp[-5].symbol_info)->getName()+"("+ (yyvsp[-3].symbol_info)->getName()+")"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement " << endl;

		parse_line = "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-6].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		//$$->addChild($6);
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		checkFunctionDefinition();

		isFunctionDeclared = false;
		isFunctionDefined = false;
		isIdDeclared = false;
		isErrorFunction = false;


		procedure_end((yyvsp[-5].symbol_info)->getName(),parameter_list.size());

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();


		}
#line 2759 "y.tab.c"
    break;

  case 14: /* $@4: %empty  */
#line 1098 "1905050.y"
                                                  {string return_type = "CONST_"+UpperCase((yyvsp[-3].symbol_info)->getName());string name = (yyvsp[-2].symbol_info)->getName();checkFunctionDeclaration(name, return_type); procedure_declaration(name);}
#line 2765 "y.tab.c"
    break;

  case 15: /* func_definition: type_specifier ID LPAREN RPAREN $@4 compound_statement  */
#line 1098 "1905050.y"
                                                                                                                                                                                                                                  {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-5].symbol_info)->getName()+" "+ (yyvsp[-4].symbol_info)->getName()+"("+")"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "func_definition : type_specifier ID LPAREN RPAREN compound_statement" << endl;

		parse_line = "func_definition : type_specifier ID LPAREN RPAREN compound_statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-5].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		//$$->addChild($5);
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		checkFunctionDefinition();

		isFunctionDeclared = false;
		isFunctionDefined = false;
		isIdDeclared = false;
		isErrorFunction = false;

		procedure_end((yyvsp[-4].symbol_info)->getName(),parameter_list.size());


		function_name = "";
		function_type = "";

		//clear parameter list
		parameter_list.clear();
		}
#line 2803 "y.tab.c"
    break;

  case 16: /* new_scope: %empty  */
#line 1134 "1905050.y"
            {
		
		if(isFunctionDeclared==true){
			symbolInfo* symbol = symbol_table.LookUp(function_name,log_file);

			//check return type
			if(symbol->getIdStructure() != function_type){
				generateError("Conflicting types for '"+ function_name +"'");
			}

			if(parameter_list.size() != symbol->getParameterListSize()){
				generateError("Conflicting types for '"+ function_name +"'");
			}

			//check parameter type match with declaration

			for(int i=0; i<parameter_list.size(); i++){
				string str = UpperCase(parameter_list[i].second);
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}
				string ptype = str;
				if(ptype != symbol->getParameter(i)){
					generateError("Parameter type of function "+ function_name +" does not match with its declaration");
				}
			}
		}

		scope_id++;
		symbol_table.enter_scope(scope_id,bucketSize, log_file);

		int j = 0;
		j = parameter_list.size();
		//cout << "parameter size: " << j << endl;

		//insert parameters in symbol table
		for(int i=0; i<parameter_list.size(); i++){
			//symbolInfo* symbol = new symbolInfo(parameter_list[i].first, Uppercase(parameter_list[i].second));
			//symbol_table.Insert(*symbol, log_file);
			string name = parameter_list[i].first;
			string type = parameter_list[i].second;
			string idStructure = UpperCase(type);
			type = "ID";
			symbolInfo* symbol = new symbolInfo(name, type);
			symbol->setIsArray(false);
			symbol->setIdStructure(idStructure);
			symbol->setIsID(true);
			symbol->setIsParameter(true);

			symbol_table.Insert(*symbol,log_file);
			symbol->setOffset(2 + (j * 2));
			j--;
			//cout << "parameter name: " << name << endl;
			//cout << "parameter offset: " << symbol->get_offset() << endl;

			
		}
	}
#line 2869 "y.tab.c"
    break;

  case 17: /* parameter_list: parameter_list COMMA type_specifier ID  */
#line 1197 "1905050.y"
                                                        {
		//write your code
		
		(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "parameter_list  : parameter_list COMMA type_specifier ID" << endl;

		parse_line = "parameter_list : parameter_list COMMA type_specifier ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		string name = (yyvsp[0].symbol_info)->getName();
		string type = (yyvsp[-1].symbol_info)->getName();

		//check if parameter is declared before
		bool found = true;
		for(int i=0; i<parameter_list.size(); i++){
			if(parameter_list[i].first == name){
				generateError("Redefinition of parameter '"+ name +"'");
				found = false;
			}
		}
		if(found == true){
		parameter_list.push_back(make_pair(name,type)); 
		}
		
		if(type == "VOID"){         
          generateError("Variable or field '"+ (yyvsp[0].symbol_info)->getName() +"' declared void");
        }
		
        }
#line 2910 "y.tab.c"
    break;

  case 18: /* parameter_list: type_specifier ID  */
#line 1233 "1905050.y"
                                   {
		//write your code
		
		
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "parameter_list  : type_specifier ID" << endl;

		parse_line = "parameter_list : type_specifier ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
       
	    
		string name = (yyvsp[0].symbol_info)->getName();
		string type = (yyvsp[-1].symbol_info)->getName();

		//check if name is already in parameter list
		bool found = true;
		for(int i=0; i<parameter_list.size(); i++){
			if(parameter_list[i].first == name){
				generateError("Redefinition of parameter '"+ name +"'");
				found = false;
			}
		}
		if(found == true){
		parameter_list.push_back(make_pair(name,type)); 
		}
		

		if(type == "VOID"){         
          generateError("Variable or field '"+ name +"' declared void");
        }
		
		}
#line 2951 "y.tab.c"
    break;

  case 19: /* parameter_list: parameter_list COMMA type_specifier  */
#line 1269 "1905050.y"
                                                     {
		//write your code
		
			(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "parameter_list  : parameter_list COMMA type_specifier " << endl;

		parse_line = "parameter_list : parameter_list COMMA type_specifier";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));



		//string name = $4->getName();
		string type = (yyvsp[0].symbol_info)->getName();

		
		
		parameter_list.push_back(make_pair("",type)); 
		


		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		}
#line 2985 "y.tab.c"
    break;

  case 20: /* parameter_list: type_specifier  */
#line 1299 "1905050.y"
                                {
		//write your code
		
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "parameter_list  : type_specifier " << endl;
		parse_line = "parameter_list : type_specifier";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));



		//string name = $4->getName();
		string type = (yyvsp[0].symbol_info)->getName();

		parameter_list.push_back(make_pair("",type)); 
		

		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		
		}
#line 3014 "y.tab.c"
    break;

  case 21: /* parameter_list: type_specifier error  */
#line 1323 "1905050.y"
                                      {
				(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName(), "variable");
		log_file<< "parameter_list  : type_specifier " << endl;
		parse_line = "parameter_list : type_specifier";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[-1].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));



		//string name = $4->getName();
		string type = (yyvsp[-1].symbol_info)->getName();

		parameter_list.push_back(make_pair("",type)); 
		

		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		generateError("Syntax error at parameter_list");
		}
#line 3041 "y.tab.c"
    break;

  case 22: /* compound_statement: LCURL new_scope statements RCURL  */
#line 1348 "1905050.y"
                                                     {
		//write your code
			(yyval.symbol_info) = new symbolInfo("{"+(yyvsp[-1].symbol_info)->getName()+"}", "variable");
		log_file<< "compound_statement : LCURL statements RCURL  " << endl;

		parse_line = "compound_statement : LCURL statements RCURL";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		symbol_table.print_all(log_file);
		symbol_table.exit_scope(log_file);
}
#line 3062 "y.tab.c"
    break;

  case 23: /* compound_statement: LCURL new_scope RCURL  */
#line 1364 "1905050.y"
                                       {
		//write your code
			(yyval.symbol_info) = new symbolInfo("{}", "variable");
		log_file<< "compound_statement : LCURL RCURL " << endl;

		parse_line = "compound_statement : LCURL RCURL";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		symbol_table.print_all(log_file);
		symbol_table.exit_scope(log_file);
		}
#line 3083 "y.tab.c"
    break;

  case 24: /* var_declaration: type_specifier declaration_list SEMICOLON  */
#line 1382 "1905050.y"
                                                           {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+";", "variable");
		log_file<< "var_declaration : type_specifier declaration_list SEMICOLON  " << endl;

		parse_line = "var_declaration : type_specifier declaration_list SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		if(variable_type == "VOID"){
			generateError("Variable or field '"+ (yyvsp[-1].symbol_info)->getName() +"' declared void");
		}

		//variable_type clear
		variable_type = "";
}
#line 3109 "y.tab.c"
    break;

  case 25: /* var_declaration: type_specifier error SEMICOLON  */
#line 1403 "1905050.y"
                                                {
				(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName()+";", "variable");
		log_file<< "var_declaration : type_specifier declaration_list SEMICOLON  " << endl;

		parse_line = "var_declaration : type_specifier declaration_list SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
		//$$->addChild($4);


		if(variable_type == "VOID"){
			//generateError("Variable or field '"+ $2->getName() +"' declared void");
		}

		//variable_type clear
		variable_type = "";
		generateError("Syntax error at var_declaration");
		}
#line 3135 "y.tab.c"
    break;

  case 26: /* type_specifier: INT  */
#line 1426 "1905050.y"
                     {
	(yyval.symbol_info) = new symbolInfo("int", "int");
	variable_type = "CONST_INT";
	log_file<< "type_specifier	: INT " << endl;
	
	parse_line = "type_specifier : INT";
	(yyval.symbol_info)->setParseLine(parse_line);
	(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
	(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
	(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


	}
#line 3153 "y.tab.c"
    break;

  case 27: /* type_specifier: FLOAT  */
#line 1439 "1905050.y"
                       {
	(yyval.symbol_info) = new symbolInfo("float", "float");
	variable_type = "CONST_FLOAT";
	log_file<< "type_specifier	: FLOAT " << endl;

	parse_line = "type_specifier : FLOAT";
	(yyval.symbol_info)->setParseLine(parse_line);
	(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
	(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
	(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		}
#line 3170 "y.tab.c"
    break;

  case 28: /* type_specifier: VOID  */
#line 1451 "1905050.y"
                      {
	(yyval.symbol_info) = new symbolInfo("void", "void");
	variable_type = "VOID";
	log_file<< "type_specifier	: VOID" << endl;

	parse_line = "type_specifier : VOID";
	(yyval.symbol_info)->setParseLine(parse_line);
	(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
	(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
	(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		}
#line 3187 "y.tab.c"
    break;

  case 29: /* declaration_list: declaration_list COMMA ID  */
#line 1465 "1905050.y"
                                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName()+","+ (yyvsp[0].symbol_info)->getName(), "variable");


			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable((yyvsp[0].symbol_info)->getName(), variable_type);

		log_file<< "declaration_list : declaration_list COMMA ID  " << endl;

		if(symbol_table.get_current_id() == 1){
			global_var_declaration((yyvsp[0].symbol_info)->getName(), variable_type);
		}

		parse_line = "declaration_list : declaration_list COMMA ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 3220 "y.tab.c"
    break;

  case 30: /* declaration_list: declaration_list COMMA ID LTHIRD CONST_INT RTHIRD  */
#line 1493 "1905050.y"
                                                                     {
		//write your code	
			(yyval.symbol_info) = new symbolInfo((yyvsp[-5].symbol_info)->getName()+","+ (yyvsp[-3].symbol_info)->getName()+"["+ (yyvsp[-1].symbol_info)->getName()+"]", "variable");

			variable_type = variable_type+"[]";

			//array size
			int array_size = atoi((yyvsp[-1].symbol_info)->getName().c_str());

			insertVariable((yyvsp[-3].symbol_info)->getName(), variable_type, array_size);
		log_file<< "declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE " << endl;

         if(symbol_table.get_current_id() == 1){
			global_var_declaration((yyvsp[-3].symbol_info)->getName(), variable_type, (yyvsp[-1].symbol_info)->getName());
		}

		parse_line = "declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-5].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		  }
#line 3253 "y.tab.c"
    break;

  case 31: /* declaration_list: ID  */
#line 1521 "1905050.y"
                      {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");

			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable((yyvsp[0].symbol_info)->getName(), variable_type);
		log_file<< "declaration_list : ID " << endl;

		if(symbol_table.get_current_id() == 1){
			global_var_declaration((yyvsp[0].symbol_info)->getName(), variable_type);
		}

		parse_line = "declaration_list : ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		  }
#line 3283 "y.tab.c"
    break;

  case 32: /* declaration_list: ID LTHIRD CONST_INT RTHIRD  */
#line 1547 "1905050.y"
                                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName()+"["+ (yyvsp[-1].symbol_info)->getName()+"]", "variable");

			variable_type = variable_type+"[]";

			int array_size = atoi((yyvsp[-1].symbol_info)->getName().c_str());
			insertVariable((yyvsp[-3].symbol_info)->getName(), variable_type, array_size);
		log_file<< "declaration_list : ID LSQUARE CONST_INT RSQUARE " << endl;

		if(symbol_table.get_current_id() == 1){
			global_var_declaration((yyvsp[-3].symbol_info)->getName(), variable_type, (yyvsp[-1].symbol_info)->getName());
		}

		parse_line = "declaration_list : ID LSQUARE CONST_INT RSQUARE";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		  }
#line 3312 "y.tab.c"
    break;

  case 33: /* statements: statement  */
#line 1574 "1905050.y"
                      {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statements : statement  " << endl;

		parse_line = "statements : statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 3329 "y.tab.c"
    break;

  case 34: /* statements: statements statement  */
#line 1586 "1905050.y"
                                 {
		//write your code	
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statements : statements statement  " << endl; 

		parse_line = "statements : statements statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	   }
#line 3347 "y.tab.c"
    break;

  case 35: /* executeIf: %empty  */
#line 1601 "1905050.y"
            {
          string labelIfTrue = generateLabel();
		  string labelIfFalse = generateLabel();

		  (yyval.symbol_info) = new symbolInfo(labelIfFalse, "label");

		  string str = "";
		  str += "\tPOP AX\n";
		  str += "\tCMP AX, 0\n";
		  str += "\tJE " + labelIfFalse + "\n";
		  str += "\tJMP " + labelIfTrue + "\n";
		  str += labelIfTrue + ":\n";

		  writeCS(str);
}
#line 3367 "y.tab.c"
    break;

  case 36: /* statement: var_declaration  */
#line 1631 "1905050.y"
                           {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : var_declaration " << endl;

		parse_line = "statement : var_declaration";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
}
#line 3384 "y.tab.c"
    break;

  case 37: /* statement: expression_statement  */
#line 1643 "1905050.y"
                                {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : expression_statement  " << endl;

		parse_line = "statement : expression_statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


	  }
#line 3402 "y.tab.c"
    break;

  case 38: /* statement: compound_statement  */
#line 1656 "1905050.y"
                              {

		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : compound_statement " << endl;

		parse_line = "statement : compound_statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	  }
#line 3420 "y.tab.c"
    break;

  case 39: /* @5: %empty  */
#line 1669 "1905050.y"
                                            {
		//write your code
		string str = "";
        string startForLoopLabel = generateLabel();

		(yyval.symbol_info) = new symbolInfo(startForLoopLabel, "label");

		str += startForLoopLabel + ":\t;loop starts here";

		writeCS(str);


	  }
#line 3438 "y.tab.c"
    break;

  case 40: /* @6: %empty  */
#line 1681 "1905050.y"
                                 {
		//write your code

		string str = "";
		string endForLoopLabel = generateLabel();
		string conditionTrueLabel = generateLabel();

		(yyval.symbol_info) = new symbolInfo(endForLoopLabel, "label");

		//str += "\tPOP AX\n";
		str += "\tCMP AX, 0\n";
		str += "\tJE " + endForLoopLabel + "\t ; if false jump to end of loop\n";
		str += "\tJMP " + conditionTrueLabel + "\n";
		str += conditionTrueLabel + ":";

		writeCS(str);

	  }
#line 3461 "y.tab.c"
    break;

  case 41: /* @7: %empty  */
#line 1699 "1905050.y"
          { (yyval.int_val) = asm_CS_endLine;
	  //cout << "asm_CS_endLine: " << asm_CS_endLine << endl;
	  }
#line 3469 "y.tab.c"
    break;

  case 42: /* $@8: %empty  */
#line 1702 "1905050.y"
                     {
		//write your code

		string expr = (yyvsp[0].symbol_info)->getName();

		string str = "";
		string startForLoopLabel = (yyvsp[-4].symbol_info)->getName();
		string endForLoopLabel = (yyvsp[-2].symbol_info)->getName();

		str += "\tPOP AX\t; " + expr +" popped\n";
		str += "\tJMP " + startForLoopLabel + "\t; jump to start of loop\n";
		str += endForLoopLabel + ":\t; end of loop";

		//cout << "asm_CS_endLine: " << asm_CS_endLine << endl;

		writeCS(str);

		//cout << "asm_CS_endLine: " << asm_CS_endLine << endl;

		asm_CS_endLine = (yyvsp[-1].int_val);        

		//cout << "asm_CS_endLine: " << asm_CS_endLine << endl;

	  }
#line 3498 "y.tab.c"
    break;

  case 43: /* statement: FOR LPAREN expression_statement @5 expression_statement @6 @7 expression $@8 RPAREN statement  */
#line 1725 "1905050.y"
                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo("for("+ (yyvsp[-8].symbol_info)->getName()+";"+ (yyvsp[-6].symbol_info)->getName()+";"+ (yyvsp[-3].symbol_info)->getName()+")"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement " << endl;

		string str = "";
		asm_CS_endLine = asm_LineCount;

		parse_line = "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-10].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-10].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-9].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-8].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


	  }
#line 3525 "y.tab.c"
    break;

  case 44: /* statement: IF LPAREN expression RPAREN executeIf statement  */
#line 1747 "1905050.y"
                                                                                   {
		//write your code
			(yyval.symbol_info) = new symbolInfo("if("+ (yyvsp[-3].symbol_info)->getName()+")"+ (yyvsp[-1].symbol_info)->getName(), "variable");
		log_file<< "statement : IF LPAREN expression RPAREN statement " << endl;

		string str = "";
		str += (yyvsp[-1].symbol_info)->getName() + ":\n";
		writeCS(str);

		parse_line = "statement : IF LPAREN expression RPAREN statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-5].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[-1].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));

	  }
#line 3550 "y.tab.c"
    break;

  case 45: /* @9: %empty  */
#line 1767 "1905050.y"
                                                                 {
		//write your code
		    
		  string labelElse = (yyvsp[-2].symbol_info)->getName();
		  string labelElseEnd = generateLabel();

		  (yyval.symbol_info) = new symbolInfo(labelElseEnd, "label");

		  string str = "";
		  str += "\tJMP " + labelElseEnd + "\n";
		  str += labelElse + ":\n";

		  writeCS(str);

	  }
#line 3570 "y.tab.c"
    break;

  case 46: /* statement: IF LPAREN expression RPAREN executeIf statement ELSE @9 statement  */
#line 1781 "1905050.y"
                     {
		//write your code
			(yyval.symbol_info) = new symbolInfo("if("+ (yyvsp[-6].symbol_info)->getName()+")"+ (yyvsp[-4].symbol_info)->getName()+"else"+ (yyvsp[-2].symbol_info)->getName(), "variable");
		log_file<< "statement : IF LPAREN expression RPAREN statement ELSE statement " << endl;

		string str = "";	
		str += (yyvsp[-1].symbol_info)->getName() + ":\n";
		writeCS(str);

		parse_line = "statement : IF LPAREN expression RPAREN statement ELSE statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-8].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[-2].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-8].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-7].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));

	  }
#line 3597 "y.tab.c"
    break;

  case 47: /* @10: %empty  */
#line 1804 "1905050.y"
                  {
		string str = "";
		string startWhileLoopLabel = generateLabel();

		(yyval.symbol_info) = new symbolInfo(startWhileLoopLabel, "label");

		str += startWhileLoopLabel + ":\t;loop starts here";

		writeCS(str);

	  }
#line 3613 "y.tab.c"
    break;

  case 48: /* @11: %empty  */
#line 1814 "1905050.y"
                              {
		//write your code

		string str = "";
		string endWhileLoopLabel = generateLabel();
		string conditionTrueLabel = generateLabel();

		(yyval.symbol_info) = new symbolInfo(endWhileLoopLabel, "label");

		str += "\tPOP AX\t; " + (yyvsp[0].symbol_info)->getName() +" popped\n";
		str += "\tCMP AX, 0\n";
		str += "\tJE " + endWhileLoopLabel + "\t ; if false jump to end of loop\n";
		str += "\tJMP " + conditionTrueLabel + "\t; jump to start of loop\n";
		str += conditionTrueLabel + ":\t; condition true";

		writeCS(str);

	  }
#line 3636 "y.tab.c"
    break;

  case 49: /* statement: WHILE @10 LPAREN expression @11 RPAREN statement  */
#line 1831 "1905050.y"
                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo("while("+ (yyvsp[-3].symbol_info)->getName()+")"+ (yyvsp[-1].symbol_info)->getName(), "variable");
		log_file<< "statement : WHILE LPAREN expression RPAREN statement " << endl;

		string str = "";
		string endWhileLoopLabel = (yyvsp[-2].symbol_info)->getName();
		string startWhileLoopLabel = (yyvsp[-5].symbol_info)->getName();

		str += "\tJMP " + startWhileLoopLabel + "\t; jump to start of loop\n";
		str += endWhileLoopLabel + ":\t; end of loop";

		writeCS(str);

		parse_line = "statement : WHILE LPAREN expression RPAREN statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-6].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	  }
#line 3666 "y.tab.c"
    break;

  case 50: /* statement: PRINTLN LPAREN ID RPAREN SEMICOLON  */
#line 1856 "1905050.y"
                                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo("println("+ (yyvsp[-2].symbol_info)->getName()+");", "variable");
			string name = (yyvsp[-2].symbol_info)->getName();
			symbolInfo* temp = symbol_table.LookUp(name,log_file);

			string str = "";
 
			if(temp == NULL){
				generateError("Undeclared variable '"+name+"'");
			}

			if(temp->get_offset() == -1){
				//generateError("Undeclared variable '"+name+"'");
				str += "\tPUSH "+name+"\n";
			}
			else{
				str += "\tPUSH [BP+"+to_string(temp->get_offset())+"]\n"; 
				}

				str += "\tCALL print_output";

              //cout << "asm_CS_endLine: " << asm_CS_endLine << endl;
	          writeCS(str);
		log_file<< "statement : PRINTLN LPAREN ID RPAREN SEMICOLON " << endl;

		parse_line = "statement : PRINTLN LPAREN ID RPAREN SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-4].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	  }
#line 3708 "y.tab.c"
    break;

  case 51: /* statement: RETURN expression SEMICOLON  */
#line 1893 "1905050.y"
                                       {
		//write your code
			string name = "return " + (yyvsp[-1].symbol_info)->getName()+";";
			string type = (yyvsp[-1].symbol_info)->getType();

			if(function_type != " "){
				if(function_type =="CONST_FLOAT" && (type == "CONST_INT" || type == "CONST_FLOAT")){
				        type = "CONST_FLOAT";	
				}
				else{
					if(isErrorFunction == false){
						symbolInfo* temp = new symbolInfo("x",function_type);
					//error_file<<function_type<<"    "<<type<<endl;
					type = HandleTypeCast(temp, (yyvsp[-1].symbol_info),"Return type mismatch with function type in "+function_name);
					}
					
				}
			}

			string str = "";

			//save the return value in AX 

			str += "\tPOP AX\n";

            int param_count = 0;
            param_count = parameter_list.size();

			//restore the old base pointer
			if(function_name != "main"){
				str += "\tMOV SP, BP\n";
				str += "\tPOP BP\n";

				if(param_count > 0){
					str += "\tRET "+to_string(param_count*2);
				}
                else{
					str += "\tRET";
				}
                 writeCS(str);        
			}
		

			(yyval.symbol_info) = new symbolInfo(name, type);

		log_file<< "statement : RETURN expression SEMICOLON" << endl;

		parse_line = "statement : RETURN expression SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


	  }
#line 3770 "y.tab.c"
    break;

  case 52: /* expression_statement: SEMICOLON  */
#line 1952 "1905050.y"
                                   {
		//write your code
			(yyval.symbol_info) = new symbolInfo(";", "variable");
		log_file<< "expression_statement : SEMICOLON" << endl;

		parse_line = "expression_statement : SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
		
}
#line 3787 "y.tab.c"
    break;

  case 53: /* expression_statement: expression SEMICOLON  */
#line 1964 "1905050.y"
                                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+";", (yyvsp[-1].symbol_info)->getType());
		log_file<< "expression_statement : expression SEMICOLON 		 " << endl;

		string name = (yyvsp[-1].symbol_info)->getName();
		string str = "";
		str += "\tPOP AX\t ;Popping out "+name;
		writeCS(str);

		parse_line = "expression_statement : expression SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

			}
#line 3810 "y.tab.c"
    break;

  case 54: /* expression_statement: expression error  */
#line 1982 "1905050.y"
                                          {
		//write your code
		(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+";", (yyvsp[-1].symbol_info)->getType());
		log_file<< "expression_statement : expression SEMICOLON 		 " << endl;

		parse_line = "expression_statement : expression SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[-1].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		//$$->addChild($2);

		generateError("Syntax error at expression_statement");

			}
#line 3830 "y.tab.c"
    break;

  case 55: /* variable: ID  */
#line 2000 "1905050.y"
             {
		//write your code
	
		log_file<< "variable : ID 	 " << endl;


		string name = (yyvsp[0].symbol_info)->getName();
		symbolInfo* temp = symbol_table.LookUp(name,log_file);
		

		if(temp == NULL){
			generateError("Undeclared variable '"+(yyvsp[0].symbol_info)->getName()+"'");
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "undeclared");
		}

		else{
			variable_type = temp->getIdStructure();
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), variable_type);
			(yyval.symbol_info)->setOffset(temp->get_offset());
			(yyval.symbol_info)->setArraySize(temp->getArraySize());
			//$$->setIsArray(temp->getIsArray());
			(yyval.symbol_info)->setIdStructure(temp->getIdStructure());
		}

		if((yyval.symbol_info)->get_offset() == -1){
			writeCS("\tPUSH "+ name+"\t;global var pushing");
		}
		else{
			//cout<<"name : "<<name<<endl;
			//cout<<"offset : "<<$$->get_offset()<<endl;
            writeCS("\tPUSH [BP+"+to_string((yyval.symbol_info)->get_offset())+"]");
		}

		parse_line = "variable : ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 3875 "y.tab.c"
    break;

  case 56: /* variable: ID LTHIRD expression RTHIRD  */
#line 2040 "1905050.y"
                                      {
		//write your code
	
		log_file<< "variable : ID LSQUARE expression RSQUARE  	 " << endl;


		string name = (yyvsp[-3].symbol_info)->getName();
		variable_type = "variable";

		symbolInfo* temp = symbol_table.LookUp(name,log_file);

		if(temp == NULL){
			generateError("Undeclared variable '"+(yyvsp[-3].symbol_info)->getName()+"'");
			variable_type = "undeclared";
			(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName(), variable_type);
		}

		else if(temp->getType() == "ARRAY"){
			variable_type = temp->getIdStructure();
			(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName(), variable_type);

			//error_file<<$3->getType()<<" "<<$3->getIdStructure()<<endl;

		if((yyvsp[-1].symbol_info)->getType() != "CONST_INT"){
			generateError("Array subscript is not an integer");
		}
		}

		else{
			generateError("'"+(yyvsp[-3].symbol_info)->getName() +"' is not an array");
			variable_type = temp->getIdStructure();
			(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName(), variable_type);
		}

		(yyval.symbol_info)->setOffset(temp->get_offset());
		(yyval.symbol_info)->setArraySize(temp->getArraySize());
		//cout << temp->getArraySize() << endl;
		(yyval.symbol_info)->setIsArray(temp->getIsArray());
		(yyval.symbol_info)->setIdStructure(temp->getIdStructure());

		string str = "";
		str += "\tPOP BX\t;array index\n";
		str += "\tSHL BX, 1\n";

		int offset = temp->get_offset();

		if(offset == -1){
			str += "\tMOV AX, "+name+"[BX]\t;global array element\n";
		}
		else{
			str += "\tNEG BX\n";
			str += "\tADD BX, "+to_string(offset)+"\n";
            //save BP
			str += "\tPUSH BP\n";
			str += "\tADD BP, BX\n";
			str += "\tMOV BX, BP\n";
			str += "\tMOV AX, [BP]\n";
			str += "\tPOP BP\n";

		}

		str += "\tPUSH AX\t;array element\n";
		str += "\tPUSH BX\t;array index";

		writeCS(str);

		parse_line = "variable : ID LSQUARE expression RSQUARE";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		}
#line 3956 "y.tab.c"
    break;

  case 57: /* expression: logic_expression  */
#line 2118 "1905050.y"
                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
			//error_file<<$1->getType()<<endl;
		log_file<< "expression 	: logic_expression	 " << endl;

		parse_line = "expression : logic_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

 }
#line 3974 "y.tab.c"
    break;

  case 58: /* expression: variable ASSIGNOP logic_expression  */
#line 2131 "1905050.y"
                                               {
		//write your code
		
		log_file<< "expression 	: variable ASSIGNOP logic_expression 		 " << endl;


		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
		string type = right_type;

		string var_name =  (yyvsp[-2].symbol_info)->getName();
		string expression = (yyvsp[0].symbol_info)->getName();

		string str = "";
		str += "\tPOP AX\t;"+expression+" popped\n";

		if(checkVoid(left_type)){
		}

		else if(checkVoid(right_type)){
			//HandleTypeCast($1, $3,"Type mismatch");
		}

		else if(left_type == "CONST_INT" && right_type == "CONST_FLOAT"){
		     type = "CONST_INT";
			 generateError("Warning: possible loss of data in assignment of FLOAT to INT");
		}
		else if(left_type == "CONST_FLOAT" && right_type == "CONST_INT"){
		     type = "CONST_FLOAT";
			 //generateError("Warning: possible loss of data in assignment of INT to FLOAT");
		}
		else if(left_type != right_type){
			//HandleTypeCast($1, $3,"Type mismatch");
		}

		(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName(), type);
		(yyval.symbol_info)->setOffset((yyvsp[-2].symbol_info)->get_offset());
		(yyval.symbol_info)->setArraySize((yyvsp[-2].symbol_info)->getArraySize());
		//$$->setIsArray($1->getIsArray());
		(yyval.symbol_info)->setIdStructure((yyvsp[-2].symbol_info)->getIdStructure());

		int array_size = (yyvsp[-2].symbol_info)->getArraySize();
		if(array_size >0){
			str += "\tPOP BX\t;index of the array element popped \n";
		}


        //global
		if((yyvsp[-2].symbol_info)->get_offset() == -1){
			
			if(array_size >0){
				str += "\tMOV "+var_name+"[BX], AX\t; assigning value of "+(yyvsp[0].symbol_info)->getName()+" to "+var_name+"[BX]\n";
			}
			else{
				str += "\tMOV "+var_name+", AX\t; assigning value of "+(yyvsp[0].symbol_info)->getName()+" to "+var_name+"\n";
			}
			

		}

		//local
		else{
			if(array_size >0){
				//push/store bp
				//mov bx to bp
				//after accessing the array element, restore bp
				
				str += "\tPUSH BP\t;storing BP\n";
				str += "\tMOV BP, BX\n";
				
				str += "\tMOV [BP], AX\t; assigning value of "+(yyvsp[0].symbol_info)->getName()+" to "+var_name+"\n";
				str += "\tPOP BP\n";

			}
			else{
				str += "\tMOV [BP+"+to_string((yyvsp[-2].symbol_info)->get_offset())+"], AX\t; assigning value of "+(yyvsp[0].symbol_info)->getName()+" to "+var_name+"\n";
			}
		}

		writeCS(str);

		parse_line = "expression : variable ASSIGNOP logic_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	   }
#line 4070 "y.tab.c"
    break;

  case 59: /* logic_expression: rel_expression  */
#line 2224 "1905050.y"
                                 {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
			//error_file<<$$->getType()<<endl;
		log_file<< "logic_expression : rel_expression 	 " << endl;

		parse_line = "logic_expression : rel_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 4088 "y.tab.c"
    break;

  case 60: /* logic_expression: rel_expression LOGICOP rel_expression  */
#line 2237 "1905050.y"
                                                        {
		//write your code
	
		log_file<< "logic_expression : rel_expression LOGICOP rel_expression " << endl;

		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
		string type = "CONST_INT";

		if(checkVoid(left_type)){
		}

		else if(checkVoid(right_type)){
		
		}

		(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName(), type);
		//error_file<<$$->getType()<<endl;

		string str = "";
		string logic_op = (yyvsp[-1].symbol_info)->getName();

		str += "\t ;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+"\n";

		if(logic_op == "&&"){
            string leftTrueLabel = generateLabel();
			string AllTrueLabel = generateLabel();
			string endLabel = generateLabel();

			str += "\tPOP BX\t;"+(yyvsp[0].symbol_info)->getName()+" popped\n";
			str += "\tPOP AX\t;"+(yyvsp[-2].symbol_info)->getName()+" popped\n";
			str += "\tCMP AX, 0\t;"+(yyvsp[-2].symbol_info)->getName()+"\n";
			str += "\tJNE "+leftTrueLabel+"\t;"+(yyvsp[-2].symbol_info)->getName()+" is true\n";
			str += "\tPUSH 0\t;"+(yyvsp[-2].symbol_info)->getName()+" is false\n";
			str += "\tJMP "+endLabel+"\t;"+(yyvsp[-2].symbol_info)->getName()+" is false\n";
			str += leftTrueLabel+":\n";
			str += "\tCMP BX, 0\t;"+(yyvsp[0].symbol_info)->getName()+"\n";
			str += "\tJNE "+AllTrueLabel+"\t;"+(yyvsp[0].symbol_info)->getName()+" is true\n";
			str += "\tPUSH 0\t;"+(yyvsp[0].symbol_info)->getName()+" is false\n";
			str += "\tJMP "+endLabel+"\t;"+(yyvsp[0].symbol_info)->getName()+" is false\n";
			str += AllTrueLabel+":\n";
			str += "\tPUSH 1\t;"+(yyvsp[-2].symbol_info)->getName()+" AND "+(yyvsp[0].symbol_info)->getName()+" is true\n";
			str += endLabel+":\n";


		}
		else if(logic_op == "||"){
			string AllTrueLabel = generateLabel();
			string endLabel = generateLabel();

			str += "\tPOP BX\t;"+(yyvsp[0].symbol_info)->getName()+" popped\n";
			str += "\tPOP AX\t;"+(yyvsp[-2].symbol_info)->getName()+" popped\n";
			str += "\tCMP AX, 0\t;"+(yyvsp[-2].symbol_info)->getName()+"\n";
			str += "\tJNE "+AllTrueLabel+"\t;"+(yyvsp[-2].symbol_info)->getName()+" is true\n";
			str += "\tCMP BX, 0\t;"+(yyvsp[0].symbol_info)->getName()+"\n";
			str += "\tJNE "+AllTrueLabel+"\t;"+(yyvsp[0].symbol_info)->getName()+" is true\n";
			str += "\tPUSH 0\t;"+(yyvsp[-2].symbol_info)->getName()+" OR "+(yyvsp[0].symbol_info)->getName()+" is false\n";
			str += "\tJMP "+endLabel+"\t;"+(yyvsp[-2].symbol_info)->getName()+" OR "+(yyvsp[0].symbol_info)->getName()+" is false\n";
			str += AllTrueLabel+":\n";
			str += "\tPUSH 1\t;"+(yyvsp[-2].symbol_info)->getName()+" OR "+(yyvsp[0].symbol_info)->getName()+" is true\n";
			str += endLabel+":\n";

		}

		writeCS(str);
		
		parse_line = "logic_expression : rel_expression LOGICOP rel_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		
		 }
#line 4170 "y.tab.c"
    break;

  case 61: /* rel_expression: simple_expression  */
#line 2316 "1905050.y"
                                   {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
			//error_file<<$$->getType()<<endl;
		log_file<< "rel_expression	: simple_expression " << endl;

		parse_line = "rel_expression : simple_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 4188 "y.tab.c"
    break;

  case 62: /* rel_expression: simple_expression RELOP simple_expression  */
#line 2329 "1905050.y"
                                                           {
		//write your code
			
		log_file<< "rel_expression : simple_expression RELOP simple_expression " << endl;
 
		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
		string type = "CONST_INT";
        
		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
			 //type = left_type;
		}

		else{
			HandleTypeCastAvoidSide((yyvsp[-2].symbol_info), (yyvsp[0].symbol_info),"Type mismatch");
		}	
		(yyval.symbol_info) = new symbolInfo(name, type);

		string str = "";
		string relop = (yyvsp[-1].symbol_info)->getName();

		str += "\t ;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+"\n";

		string trueLabel = generateLabel();
		string endLabel = generateLabel();

		str += "\tPOP BX\t;"+(yyvsp[0].symbol_info)->getName()+" popped\n";
		str += "\tPOP AX\t;"+(yyvsp[-2].symbol_info)->getName()+" popped\n";
		str += "\tCMP AX, BX\t;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+"\n";
		str += "\t"+JumpConverter(relop)+" "+trueLabel+"\t;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+"\n";
		str += "\tPUSH 0\t;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+" is false\n";
		str += "\tJMP "+endLabel+"\t;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+" is false\n";
		str += trueLabel+":\n";
		str += "\tPUSH 1\t;"+(yyvsp[-2].symbol_info)->getName()+" "+(yyvsp[-1].symbol_info)->getName()+" "+(yyvsp[0].symbol_info)->getName()+" is true\n";
		str += endLabel+":\n";

		writeCS(str);

		parse_line = "rel_expression : simple_expression RELOP simple_expression";

		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
		}
#line 4244 "y.tab.c"
    break;

  case 63: /* simple_expression: term  */
#line 2382 "1905050.y"
                        {
		//write your code
		string type = (yyvsp[0].symbol_info)->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), type);
			//error_file<<$$->getType()<<endl;
		log_file<< "simple_expression : term " << endl;

		parse_line = "simple_expression : term";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 4268 "y.tab.c"
    break;

  case 64: /* simple_expression: simple_expression ADDOP term  */
#line 2401 "1905050.y"
                                                {
		//write your code
		
		log_file<< "simple_expression : simple_expression ADDOP term  " << endl;

		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
		string type = right_type;

		string expression = (yyvsp[-2].symbol_info)->getName();
		string term = (yyvsp[0].symbol_info)->getName();


		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
             type = left_type;
		}

		else{
			type = HandleTypeCastAvoidSide((yyvsp[-2].symbol_info), (yyvsp[0].symbol_info), "Type mismatch");
		}


		string str = "";
        str += "\tPOP BX\t; "+term+" poped from stack\n";
		str += "\tPOP AX\t; "+expression+" poped from stack\n";

		string addop = (yyvsp[-1].symbol_info)->getName();

			if(addop == "+"){
                   str += "\tADD AX, BX\n";
				   str += "\tPUSH AX\t; "+expression+" + "+term+" pushed to stack";
			}
			else{
                   str += "\tSUB AX, BX\n";
				   str += "\tPUSH AX\t; "+expression+" - "+term+" pushed to stack";
			}

		writeCS(str);

		parse_line = "simple_expression : simple_expression ADDOP term";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		  }
#line 4325 "y.tab.c"
    break;

  case 65: /* term: unary_expression  */
#line 2455 "1905050.y"
                        {
		//write your code
		string type = (yyvsp[0].symbol_info)->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), type);
		log_file<< "term :	unary_expression " << endl;

		parse_line = "term : unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 4348 "y.tab.c"
    break;

  case 66: /* term: term MULOP unary_expression  */
#line 2473 "1905050.y"
                                   {
		//write your code
		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
		string type = right_type;

		log_file<< "term :	term MULOP unary_expression " << endl;

		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
             type = left_type;
		}

		else if((yyvsp[-1].symbol_info)->getName() == "%"){
			if(left_type != "CONST_INT" || right_type != "CONST_INT"){
				//type = HandleTypeCheckAvoidSide($1, $3, "Type mismatch");
				generateError("Operands of modulus must be integers ");
			}
			else if((yyvsp[0].symbol_info)->getName() == "0"){
				generateError("Warning: division by zero i=0f=1Const=0");
			}
			
				type = "CONST_INT";
		}
		else{
			type = HandleTypeCastAvoidSide((yyvsp[-2].symbol_info), (yyvsp[0].symbol_info), "Type mismatch");
		}
		
			(yyval.symbol_info) = new symbolInfo(name, type);

		string mulop_name = (yyvsp[-1].symbol_info)->getName();
		string str = "";
		str += "\tPOP BX\t; "+(yyvsp[0].symbol_info)->getName()+" poped from stack\n";
		str += "\tPOP AX\t; "+(yyvsp[-2].symbol_info)->getName()+" poped from stack\n";


		if(mulop_name == "*"){
			str += "\tIMUL BX\n\tPUSH AX\t; "+(yyvsp[-2].symbol_info)->getName()+" * "+(yyvsp[0].symbol_info)->getName()+" pushed to stack";
		
		}
		else if(mulop_name == "/"){
			str += "\tXOR DX, DX\n\tIDIV BX\n\tPUSH AX\t; "+(yyvsp[-2].symbol_info)->getName()+" / "+(yyvsp[0].symbol_info)->getName()+" pushed to stack";
		}
		else if(mulop_name == "%"){
			str += "\tXOR DX, DX\n\tIDIV BX\n\tMOV AX,DX\n\tPUSH AX\t; "+(yyvsp[-2].symbol_info)->getName()+" % "+(yyvsp[0].symbol_info)->getName()+" pushed to stack";
		}
        
		writeCS(str);

		parse_line = "term : term MULOP unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		
	 }
#line 4415 "y.tab.c"
    break;

  case 67: /* unary_expression: ADDOP unary_expression  */
#line 2537 "1905050.y"
                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "unary_expression : ADDOP unary_expression " << endl;

		string addop_name = (yyvsp[-1].symbol_info)->getName();

		if(addop_name == "-"){
			string str = "";
			str += "\tPOP AX\t; "+(yyvsp[0].symbol_info)->getName()+" poped from stack\n";
			str += "\tNEG AX\n\tPUSH AX\t; -"+(yyvsp[0].symbol_info)->getName()+" pushed to stack";
			writeCS(str);
		}
		

		parse_line = "unary_expression : ADDOP unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

  		}
#line 4443 "y.tab.c"
    break;

  case 68: /* unary_expression: NOT unary_expression  */
#line 2560 "1905050.y"
                                       {
		//write your code
			(yyval.symbol_info) = new symbolInfo("! "+ (yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "unary_expression : NOT unary_expression " << endl;

		string str = "";
		string labelIfOne = generateLabel();
		string endLabel = generateLabel();

		str += "\tPOP AX\t; "+(yyvsp[0].symbol_info)->getName()+" poped from stack\n";
		str += "\tCMP AX, 0\n";
		str += "\tJNE "+labelIfOne+"\n";
		str += "\tPUSH 1\t; 1 pushed to stack\n";
		str += "\tJMP "+endLabel+"\n";
		str += labelIfOne+":\n";
		str += "\tPUSH 0\t; 0 pushed to stack\n";
		str += endLabel+":\n";
		writeCS(str);

		parse_line = "unary_expression : NOT unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		 }
#line 4475 "y.tab.c"
    break;

  case 69: /* unary_expression: factor  */
#line 2587 "1905050.y"
                         {
		//write your code
			string type = (yyvsp[0].symbol_info)->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), type);
		log_file<< "unary_expression : factor " << endl;

		parse_line = "unary_expression : factor";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		 }
#line 4498 "y.tab.c"
    break;

  case 70: /* factor: variable  */
#line 2607 "1905050.y"
                  {
		//write your code
		string type = (yyvsp[0].symbol_info)->getType();


		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), type);
		log_file<< "factor	: variable " << endl;

		if((yyvsp[0].symbol_info)->getArraySize()>0)
		         writeCS("POP BX\t ;Pop array index from stack");

		parse_line = "factor : variable";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		}
#line 4526 "y.tab.c"
    break;

  case 71: /* factor: ID LPAREN argument_list RPAREN  */
#line 2630 "1905050.y"
                                        {
		//write your code
		//$$ = new symbolInfo($1->getName+"("+ $3->getName+")", "variable");
		log_file<< "factor	: ID LPAREN argument_list RPAREN  " << endl;	

		string variable = (yyvsp[-3].symbol_info)->getName()+"("+ (yyvsp[-1].symbol_info)->getName()+")";

		symbolInfo* temp = symbol_table.LookUp((yyvsp[-3].symbol_info)->getName(),log_file);
		if(temp == NULL){
			generateError("Undeclared function '"+ (yyvsp[-3].symbol_info)->getName()+"'");
			(yyval.symbol_info) = new symbolInfo(variable, "undeclared");
		}

		//function
		else if(temp->getIsFunction()){
			string return_type = temp->getReturnType();

			if(return_type == "CONST_VOID")
			    return_type = "VOID";

			(yyval.symbol_info) = new symbolInfo(variable, return_type);
            
	        if(temp->getParameterListSize() != argument_list.size()){

				if(temp->getParameterListSize() < argument_list.size()){
                    generateError("Too many arguments to function '"+ (yyvsp[-3].symbol_info)->getName()+"'");
				}

				else if(temp->getParameterListSize() > argument_list.size()){
					generateError("Too few arguments to function '"+ (yyvsp[-3].symbol_info)->getName()+"'");
				}
			
	        }
	        else{
	            for(int i=0; i<argument_list.size(); i++){
	                //create dummy symbolInfo from parameter list and pass it to checkType
					symbolInfo* dummy = new symbolInfo("dummy", temp->getParameter(i));

					//error_file<<dummy->getType()<<" "<<argument_list[i]->getType() <<endl;
					
	                handleTypeCheck(dummy, argument_list[i], "Type mismatch for argument "+to_string(i+1)+" of '"+(yyvsp[-3].symbol_info)->getName()+"'");
	            }
	        }

			
		}

		//id
		else{
			generateError((yyvsp[-3].symbol_info)->getName()+" is not a function");
			(yyval.symbol_info) = new symbolInfo(variable, temp->getIdStructure());
		}

		//call function
		string str = "\tCALL "+(yyvsp[-3].symbol_info)->getName()+"\n";
		str += "\tPUSH AX";
		writeCS(str);

		//clear argument list
		argument_list.clear();

		parse_line = "factor : ID LPAREN argument_list RPAREN";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4602 "y.tab.c"
    break;

  case 72: /* factor: LPAREN expression RPAREN  */
#line 2701 "1905050.y"
                                  {
		//write your code
			(yyval.symbol_info) = new symbolInfo("("+ (yyvsp[-1].symbol_info)->getName()+")", (yyvsp[-1].symbol_info)->getType());
		log_file<< "factor : LPAREN expression RPAREN " << endl;	

		parse_line = "factor : LPAREN expression RPAREN";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4621 "y.tab.c"
    break;

  case 73: /* factor: CONST_INT  */
#line 2715 "1905050.y"
                   {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor	: CONST_INT   " << endl;

		writeCS("\tPUSH "+(yyvsp[0].symbol_info)->getName());

		parse_line = "factor : CONST_INT";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4640 "y.tab.c"
    break;

  case 74: /* factor: CONST_FLOAT  */
#line 2729 "1905050.y"
                     {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor	: CONST_FLOAT   " << endl;

		parse_line = "factor : CONST_FLOAT";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4657 "y.tab.c"
    break;

  case 75: /* factor: variable INCOP  */
#line 2743 "1905050.y"
                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+"++", (yyvsp[-1].symbol_info)->getType());
		log_file<< "factor : variable INCOP " << endl;

		string var = (yyvsp[-1].symbol_info)->getName();

		int array_size = (yyvsp[-1].symbol_info)->getArraySize();
		int offset = (yyvsp[-1].symbol_info)->get_offset();
		string str = "";

		if(array_size>0){
			str += "\tPOP BX\t ;Pop array index from stack\n";

			if(offset == -1){
			    str += "\tMOV AX, [BX]\t ;set AX to the value of " + var + "[BX]\n";
			}
			else{
			       str += "\tPUSH BP\n";
				   str += "\tMOV BP, BX\n";
				   str += "\tMOV AX, [BP]\t ;set AX to the value of " + var + "[BX]\n";

			}
		}
		else{
			str += "\tPOP AX\n";
			str += "\tPUSH AX\n";
		}

		str += "\tINC AX\t ;Increment AX\n";


		//now save the incremented value
		if(array_size>0){
			if(offset == -1){
			    str += "\tMOV " +var+ "[BX], AX\t ;set " + var + "[BX] to AX";
			}
			else{
			       str += "\tMOV [BP], AX\t ;set " + var + "[BX] to AX\n";
				   str += "\tPOP BP";
			}
		}
		else{
			if(offset == -1){
			    str += "\tMOV " + var + ", AX\t ;set " + var + " to AX";
			}
			else{
			       str += "\tMOV [BP+" + to_string(offset) + "], AX\t ;set " + var + " to AX";
			}
		}

		writeCS(str);

		parse_line = "factor : variable INCOP";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4723 "y.tab.c"
    break;

  case 76: /* factor: INCOP variable  */
#line 2805 "1905050.y"
                                        {
		//write your code
			(yyval.symbol_info) = new symbolInfo("++"+(yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor : INCOP variable " << endl;

		string str = "";
		int array_size = (yyvsp[0].symbol_info)->getArraySize();
		int offset = (yyvsp[0].symbol_info)->get_offset();
		string var = (yyvsp[0].symbol_info)->getName();

		if(array_size>0){
			str += "\tPOP BX\t ;Pop array index from stack\n";
            str += "\tPOP AX\t ;Pop array value from stack\n";
		}
		else{
			str += "\tPOP AX\t ;Pop value from stack\n";
		}

		str += "\tINC AX\t ;Increment AX\n";
		str += "\tPUSH AX\t ;Push incremented value to stack\n";

		//now save the incremented value
		if(array_size>0){
			if(offset == -1){
			    str += "\tMOV " +var+ "[BX], AX\t ;set " + var + "[BX] to AX";
			}
			else{
			       str += "\tPUSH BP\n";
				   str += "\tMOV BP, BX\n";
				   str += "\tMOV [BP], AX\t ;set " + var + "[BX] to AX\n";
				   str += "\tPOP BP";

			}
		}
		else{
			if(offset == -1){
			    str += "\tMOV " + var + ", AX\t ;set " + var + " to AX";
			}
			else{
			       str += "\tMOV [BP+" + to_string(offset) + "], AX\t ;set " + var + " to AX";
			}
		}

		writeCS(str);


		parse_line = "factor : INCOP variable";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4782 "y.tab.c"
    break;

  case 77: /* factor: variable DECOP  */
#line 2860 "1905050.y"
                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+"--", (yyvsp[-1].symbol_info)->getType());
		log_file<< "factor : variable DECOP " << endl;

		string var = (yyvsp[-1].symbol_info)->getName();
		string str = "";

		int array_size = (yyvsp[-1].symbol_info)->getArraySize();
		int offset = (yyvsp[-1].symbol_info)->get_offset();

		if(array_size>0){
			str += "\tPOP BX\t ;Pop array index from stack\n";

			if(offset == -1){
			    str += "\tMOV AX, [BX]\t ;set AX to the value of " + var + "[BX]\n";
			}
			else{
			       str += "\tPUSH BP\n";
				   str += "\tMOV BP, BX\n";
				   str += "\tMOV AX, [BP]\t ;set AX to the value of " + var + "[BX]\n";

			}
		}
		else{
			str += "\tPOP AX\n";
			str += "\tPUSH AX\n";
		}

		str += "\tDEC AX\t ;Decrement AX\n";

		//now save the decremented value
		if(array_size>0){
			if(offset == -1){
			    str += "\tMOV " +var+ "[BX], AX\t ;set " + var + "[BX] to AX";
			}
			else{
			       str += "\tMOV [BP], AX\t ;set " + var + "[BX] to AX\n";
				   str += "\tPOP BP";
			}
		}
		else{
			if(offset == -1){
			    str += "\tMOV " + var + ", AX\t ;set " + var + " to AX";
			}
			else{
			       str += "\tMOV [BP+" + to_string(offset) + "], AX\t ;set " + var + " to AX";
			}
		}

		writeCS(str);

		parse_line = "factor : variable DECOP";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
 
	}
#line 4847 "y.tab.c"
    break;

  case 78: /* factor: DECOP variable  */
#line 2921 "1905050.y"
                                        {
		//write your code
			(yyval.symbol_info) = new symbolInfo("--"+(yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor : DECOP variable " << endl;

		string str = "";
		int array_size = (yyvsp[0].symbol_info)->getArraySize();
		int offset = (yyvsp[0].symbol_info)->get_offset();
		string var = (yyvsp[0].symbol_info)->getName();

		if(array_size>0){
			str += "\tPOP BX\t ;Pop array index from stack\n";
			str += "\tPOP AX\t ;Pop array value from stack\n";
		}
		else{
			str += "\tPOP AX\t ;Pop value from stack\n";
		}

		str += "\tDEC AX\t ;Decrement AX\n";
		str += "\tPUSH AX\t ;Push decremented value to stack\n";

		//now save the decremented value
		if(array_size>0){
			if(offset == -1){
			    str += "\tMOV " +var+ "[BX], AX\t ;set " + var + "[BX] to AX";
			}
			else{
			       str += "\tPUSH BP\n";
				   str += "\tMOV BP, BX\n";
				   str += "\tMOV [BP], AX\t ;set " + var + "[BX] to AX\n";
				   str += "\tPOP BP";

			}
		}

		else {
			if(offset == -1){
			    str += "\tMOV " + var + ", AX\t ;set " + var + " to AX";
			}
			else{
			       str += "\tMOV [BP+" + to_string(offset) + "], AX\t ;set " + var + " to AX";
			}
		}

		writeCS(str);


		parse_line = "factor : DECOP variable";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 4907 "y.tab.c"
    break;

  case 79: /* argument_list: arguments  */
#line 2978 "1905050.y"
                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "argument_list : arguments  " << endl;

		parse_line = "argument_list : arguments";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 4924 "y.tab.c"
    break;

  case 80: /* argument_list: %empty  */
#line 2990 "1905050.y"
                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo(" ", "variable");
		log_file<< "argument_list : " << endl;

		/*parse_line = "argument_list : ";
		$$->setParseLine(parse_line);
		$$->setStartLine(0);
		$$->setEndLine(0);
		//$$->addChild($1);
		*/
			  }
#line 4941 "y.tab.c"
    break;

  case 81: /* arguments: arguments COMMA logic_expression  */
#line 3004 "1905050.y"
                                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName()+" , "+ (yyvsp[0].symbol_info)->getName(), "variable");
			//error_file<<$3->getType()<<endl;
			argument_list.push_back(new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType()));
		log_file<< "arguments : arguments COMMA logic_expression " << endl;

		parse_line = "arguments : arguments COMMA logic_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 4962 "y.tab.c"
    break;

  case 82: /* arguments: logic_expression  */
#line 3020 "1905050.y"
                                {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
			argument_list.push_back(new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType()));
		log_file<< "arguments : logic_expression" << endl;

		parse_line = "arguments : logic_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		  }
#line 4980 "y.tab.c"
    break;


#line 4984 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 3036 "1905050.y"

int main(int argc,char *argv[])
{

	if(argc < 2) {
		printf("Please provide input file\n");
		return 0;
	}
	
    FILE* fin = fopen(argv[1], "r");
    
    if(fin == NULL) {
        printf("Cannot open input file\n");
        return 0;
    } 

    log_file.open("log.txt");
	error_file.open("error.txt");
	//parseTree_file.open("parseTree.txt");

	startAsmFile();

    yyin = fin;
    yyparse();

    write_newLineProc();
    write_printProc();
	endAsmFile();

    log_file.close();
	error_file.close();
	//parseTree_file.close();
    fclose(fin);

	final_optimization();
    
    return 0;
}

