%{
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




void generateError(string s){
	error_file << "Line# " << count_line << ": " << s << endl;
	error_num++;
}

void yyerror(const char *s){

	//write your code
	//error_file << "Line# " << count_line << ": Syntax error" << s << endl;
	//generateError("Syntax error "+string(s));
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



void insertVariable(string name, string type){
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

			symbol_table.Insert(*symbol,log_file);
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


%}

%define parse.error verbose

%union {
    symbolInfo* symbol_info;
}

%token<symbol_info> IF ELSE FOR WHILE DO BREAK PRINTLN RETURN INT CHAR FLOAT DOUBLE VOID CONTINUE CASE DEFAULT
%token<symbol_info> LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON
%token<symbol_info> ID
%token<symbol_info> CONST_INT
%token<symbol_info> CONST_FLOAT
%token<symbol_info> ADDOP MULOP ASSIGNOP RELOP LOGICOP NOT INCOP DECOP INCOP_PRE DECOP_PRE INCOP_POST DECOP_POST
%type<symbol_info> start program unit func_declaration func_definition parameter_list compound_statement var_declaration type_specifier declaration_list statements statement expression_statement variable expression logic_expression rel_expression simple_expression term unary_expression factor argument_list arguments 


%left ASSIGNOP
%left LOGICOP
%left RELOP
%left ADDOP
%left MULOP
%right NOT
%right INCOP DECOP


%right INCOP_PRE DECOP_PRE
%left INCOP_POST DECOP_POST

%nonassoc ALTERNATIVE_ELSE
%nonassoc ELSE
 

%%

start : program
	{
		//write your code in this block in all the similar blocks below
        $$ = new symbolInfo($1->getName(), "variable");
		log_file<< "start : program " << endl;
		
		parse_line = "start : program";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

		printTree($$, 0);
		
	}
	;

program : program unit{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName() , "variable");
		log_file<< "program : program unit " << endl;

		parse_line = "program : program unit";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

}
	| unit{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "program : unit " << endl;

		parse_line = "program : unit";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);
	}
	;
	
unit : var_declaration{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "unit : var_declaration  " << endl;

		parse_line = "unit : var_declaration";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);
}
	 | func_declaration{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "unit : func_declaration " << endl;

		parse_line = "unit : func_declaration";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);
}
	 | func_definition{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "unit : func_definition  " << endl;

		parse_line = "unit : func_definition";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);
     }
     ;
     
func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName()+"("+ $4->getName()+")"+";", "variable");
		log_file<< "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON " << endl;

		parse_line = "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($6->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		$$->addChild($6);


             string str = UpperCase($1->getName());
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}
		string return_type = str;
		string name = $2->getName();

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
		| type_specifier ID LPAREN RPAREN SEMICOLON{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName()+"("+")"+";", "variable");
		log_file<< "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON " << endl;

		parse_line = "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($5->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);


		string str = UpperCase($1->getName());
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}

		string return_type = str;
		string name = $2->getName();

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
		;

		 
func_definition : type_specifier ID LPAREN parameter_list RPAREN {string return_type = "CONST_"+UpperCase($1->getName());string name = $2->getName();checkFunctionDeclaration(name, return_type);}compound_statement{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName()+"("+ $4->getName()+")"+ $7->getName(), "variable");
		log_file<< "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement " << endl;

		parse_line = "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($7->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		//$$->addChild($6);
		$$->addChild($7);


		checkFunctionDefinition();

		isFunctionDeclared = false;
		isFunctionDefined = false;
		isIdDeclared = false;
		isErrorFunction = false;

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();


		}
		| type_specifier ID LPAREN RPAREN {string return_type = "CONST_"+UpperCase($1->getName());string name = $2->getName();checkFunctionDeclaration(name, return_type);}compound_statement{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName()+"("+")"+ $6->getName(), "variable");
		log_file<< "func_definition : type_specifier ID LPAREN RPAREN compound_statement" << endl;

		parse_line = "func_definition : type_specifier ID LPAREN RPAREN compound_statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($6->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		//$$->addChild($5);
		$$->addChild($6);


		checkFunctionDefinition();

		isFunctionDeclared = false;
		isFunctionDefined = false;
		isIdDeclared = false;
		isErrorFunction = false;

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();
		}
 		;

				
new_scope : {
		
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

			symbol_table.Insert(*symbol,log_file);
		}
	}
	;

parameter_list  : parameter_list COMMA type_specifier ID{
		//write your code
		
		$$ = new symbolInfo($1->getName()+" "+ $3->getName()+" "+ $4->getName(), "variable");
		log_file<< "parameter_list  : parameter_list COMMA type_specifier ID" << endl;

		parse_line = "parameter_list : parameter_list COMMA type_specifier ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);


		string name = $4->getName();
		string type = $3->getName();

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
          generateError("Variable or field '"+ $4->getName() +"' declared void");
        }
		
        }
		| type_specifier ID{
		//write your code
		
		
			$$ = new symbolInfo($1->getName()+" "+ $2->getName(), "variable");
		log_file<< "parameter_list  : type_specifier ID" << endl;

		parse_line = "parameter_list : type_specifier ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
       
	    
		string name = $2->getName();
		string type = $1->getName();

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
		| parameter_list COMMA type_specifier{
		//write your code
		
			$$ = new symbolInfo($1->getName()+" "+ $3->getName(), "variable");
		log_file<< "parameter_list  : parameter_list COMMA type_specifier " << endl;

		parse_line = "parameter_list : parameter_list COMMA type_specifier";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);



		//string name = $4->getName();
		string type = $3->getName();

		
		
		parameter_list.push_back(make_pair("",type)); 
		


		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		}
		| parameter_list error COMMA type_specifier{
			
			$$ = new symbolInfo($1->getName()+" "+ $4->getName(), "variable");
		log_file<< "parameter_list  : parameter_list COMMA type_specifier " << endl;

		parse_line = "parameter_list : parameter_list COMMA type_specifier";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($3);
		$$->addChild($4);



		//string name = $4->getName();
		string type = $4->getName();

		
		
		parameter_list.push_back(make_pair("",type)); 
		


		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		generateError("Syntax error at parameter_list");
	
		}
		| type_specifier{
		//write your code
		
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "parameter_list  : type_specifier " << endl;
		parse_line = "parameter_list : type_specifier";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);



		//string name = $4->getName();
		string type = $1->getName();

		parameter_list.push_back(make_pair("",type)); 
		

		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		
		}
		| type_specifier error{
				$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "parameter_list  : type_specifier " << endl;
		parse_line = "parameter_list : type_specifier";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);



		//string name = $4->getName();
		string type = $1->getName();

		parameter_list.push_back(make_pair("",type)); 
		

		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		generateError("Syntax error at parameter_list");
		}
		;

 		
compound_statement : LCURL new_scope statements RCURL{
		//write your code
			$$ = new symbolInfo("{"+$3->getName()+"}", "variable");
		log_file<< "compound_statement : LCURL statements RCURL  " << endl;

		parse_line = "compound_statement : LCURL statements RCURL";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($3);
		$$->addChild($4);

		symbol_table.print_all(log_file);
		symbol_table.exit_scope(log_file);
}
		| LCURL new_scope RCURL{
		//write your code
			$$ = new symbolInfo("{}", "variable");
		log_file<< "compound_statement : LCURL RCURL " << endl;

		parse_line = "compound_statement : LCURL RCURL";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($3);


		symbol_table.print_all(log_file);
		symbol_table.exit_scope(log_file);
		}
		;
 		    
var_declaration : type_specifier declaration_list SEMICOLON{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName()+";", "variable");
		log_file<< "var_declaration : type_specifier declaration_list SEMICOLON  " << endl;

		parse_line = "var_declaration : type_specifier declaration_list SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);


		if(variable_type == "VOID"){
			generateError("Variable or field '"+ $2->getName() +"' declared void");
		}

		//variable_type clear
		variable_type = "";
}
		| type_specifier error SEMICOLON{
				$$ = new symbolInfo($1->getName()+";", "variable");
		log_file<< "var_declaration : type_specifier declaration_list SEMICOLON  " << endl;

		parse_line = "var_declaration : type_specifier declaration_list SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($3);
		//$$->addChild($4);


		if(variable_type == "VOID"){
			//generateError("Variable or field '"+ $2->getName() +"' declared void");
		}

		//variable_type clear
		variable_type = "";
		generateError("Syntax error at var_declaration");
		}
 		 ;
 		 
type_specifier	: INT{
	$$ = new symbolInfo("int", "int");
	variable_type = "CONST_INT";
	log_file<< "type_specifier	: INT " << endl;
	
	parse_line = "type_specifier : INT";
	$$->setParseLine(parse_line);
	$$->setStartLine($1->getStartLine());
	$$->setEndLine($1->getEndLine());
	$$->addChild($1);


	}
 		| FLOAT{
	$$ = new symbolInfo("float", "float");
	variable_type = "CONST_FLOAT";
	log_file<< "type_specifier	: FLOAT " << endl;

	parse_line = "type_specifier : FLOAT";
	$$->setParseLine(parse_line);
	$$->setStartLine($1->getStartLine());
	$$->setEndLine($1->getEndLine());
	$$->addChild($1);

		}
 		| VOID{
	$$ = new symbolInfo("void", "void");
	variable_type = "VOID";
	log_file<< "type_specifier	: VOID" << endl;

	parse_line = "type_specifier : VOID";
	$$->setParseLine(parse_line);
	$$->setStartLine($1->getStartLine());
	$$->setEndLine($1->getEndLine());
	$$->addChild($1);

		}
 		;
 		
declaration_list : declaration_list COMMA ID{
		//write your code
			$$ = new symbolInfo($1->getName()+","+ $3->getName(), "variable");


			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable($3->getName(), variable_type);

		log_file<< "declaration_list : declaration_list COMMA ID  " << endl;

		parse_line = "declaration_list : declaration_list COMMA ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

}
		  | declaration_list error COMMA ID{
		//write your code
		$$ = new symbolInfo($1->getName()+","+ $4->getName(), "variable");


			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable($4->getName(), variable_type);

		log_file<< "declaration_list : declaration_list COMMA ID  " << endl;

		parse_line = "declaration_list : declaration_list COMMA ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($3);
		$$->addChild($4);

		generateError("Syntax error at declaration_list");
		  
		  }
 		  | declaration_list COMMA ID LTHIRD CONST_INT RTHIRD{
		//write your code	
			$$ = new symbolInfo($1->getName()+","+ $3->getName()+"["+ $5->getName()+"]", "variable");

			variable_type = variable_type+"[]";
			insertVariable($3->getName(), variable_type);
		log_file<< "declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE " << endl;

		parse_line = "declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($6->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		$$->addChild($6);

		  }
 		  | ID{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");

			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable($1->getName(), variable_type);
		log_file<< "declaration_list : ID " << endl;

		parse_line = "declaration_list : ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);


		  }
		  | error ID{
					//write your code
			$$ = new symbolInfo($2->getName(), "variable");

			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable($2->getName(), variable_type);
		log_file<< "declaration_list : ID " << endl;

		parse_line = "declaration_list : ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($2->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($2);

		generateError("Syntax error at declaration_list");

		  }
 		  | ID LTHIRD CONST_INT RTHIRD{
		//write your code
			$$ = new symbolInfo($1->getName()+"["+ $3->getName()+"]", "variable");

			variable_type = variable_type+"[]";
			insertVariable($1->getName(), variable_type);
		log_file<< "declaration_list : ID LSQUARE CONST_INT RSQUARE " << endl;

		parse_line = "declaration_list : ID LSQUARE CONST_INT RSQUARE";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);

		  }

		  | ID LTHIRD error RTHIRD{

		  }
 		  ;
 		   
statements : statement{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "statements : statement  " << endl;

		parse_line = "statements : statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

}
	   | statements statement{
		//write your code	
			$$ = new symbolInfo($1->getName()+" "+$2->getName(), "variable");
		log_file<< "statements : statements statement  " << endl; 

		parse_line = "statements : statements statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

	   }
	   ;
	   
statement : var_declaration{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "statement : var_declaration " << endl;

		parse_line = "statement : var_declaration";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		
		$$->addChild($1);
}
	  | expression_statement{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "statement : expression_statement  " << endl;

		parse_line = "statement : expression_statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);


	  }
	  | compound_statement{

		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "statement : compound_statement " << endl;

		parse_line = "statement : compound_statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

	  }
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement{
		//write your code
			$$ = new symbolInfo("for("+ $3->getName()+";"+ $4->getName()+";"+ $5->getName()+")"+ $7->getName(), "variable");
		log_file<< "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement " << endl;

		parse_line = "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($7->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		$$->addChild($6);
		$$->addChild($7);


	  }
	   | IF LPAREN expression RPAREN statement %prec ALTERNATIVE_ELSE{
		//write your code
			$$ = new symbolInfo("if("+ $3->getName()+")"+ $5->getName(), "variable");
		log_file<< "statement : IF LPAREN expression RPAREN statement " << endl;

		parse_line = "statement : IF LPAREN expression RPAREN statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($5->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);

	  }
	  | IF LPAREN expression RPAREN statement ELSE statement{
		//write your code
			$$ = new symbolInfo("if("+ $3->getName()+")"+ $5->getName()+"else"+ $7->getName(), "variable");
		log_file<< "statement : IF LPAREN expression RPAREN statement ELSE statement " << endl;

		parse_line = "statement : IF LPAREN expression RPAREN statement ELSE statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($7->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);
		$$->addChild($6);
		$$->addChild($7);

	  }
	 
	  | WHILE LPAREN expression RPAREN statement{
		//write your code
			$$ = new symbolInfo("while("+ $3->getName()+")"+ $5->getName(), "variable");
		log_file<< "statement : WHILE LPAREN expression RPAREN statement " << endl;

		parse_line = "statement : WHILE LPAREN expression RPAREN statement";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($5->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);

	  }
	  | PRINTLN LPAREN ID RPAREN SEMICOLON{
		//write your code
			$$ = new symbolInfo("println("+ $3->getName()+");", "variable");
			string name = $3->getName();
			symbolInfo* temp = symbol_table.LookUp(name,log_file);
 
			if(temp == NULL){
				generateError("Undeclared variable '"+name+"'");
			}
		log_file<< "statement : PRINTLN LPAREN ID RPAREN SEMICOLON " << endl;

		parse_line = "statement : PRINTLN LPAREN ID RPAREN SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($5->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);
		$$->addChild($5);

	  }
	  | RETURN expression SEMICOLON{
		//write your code
			string name = "return " + $2->getName()+";";
			string type = $2->getType();

			if(function_type != " "){
				if(function_type =="CONST_FLOAT" && (type == "CONST_INT" || type == "CONST_FLOAT")){
				        type = "CONST_FLOAT";	
				}
				else{
					if(isErrorFunction == false){
						symbolInfo* temp = new symbolInfo("x",function_type);
					//error_file<<function_type<<"    "<<type<<endl;
					type = HandleTypeCast(temp, $2,"Return type mismatch with function type in "+function_name);
					}
					
				}
			}

			$$ = new symbolInfo(name, type);

		log_file<< "statement : RETURN expression SEMICOLON" << endl;

		parse_line = "statement : RETURN expression SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);


	  }
	  ;
	  
expression_statement 	: SEMICOLON{
		//write your code
			$$ = new symbolInfo(";", "variable");
		log_file<< "expression_statement : SEMICOLON" << endl;

		parse_line = "expression_statement : SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);
		
}
			| expression SEMICOLON{
		//write your code
			$$ = new symbolInfo($1->getName()+";", $1->getType());
		log_file<< "expression_statement : expression SEMICOLON 		 " << endl;

		parse_line = "expression_statement : expression SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

			}
			| expression error{
		//write your code
		$$ = new symbolInfo($1->getName()+";", $1->getType());
		log_file<< "expression_statement : expression SEMICOLON 		 " << endl;

		parse_line = "expression_statement : expression SEMICOLON";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);
		//$$->addChild($2);

		generateError("Syntax error at expression_statement");

			}
			;

	  
variable : ID{
		//write your code
	
		log_file<< "variable : ID 	 " << endl;


		string name = $1->getName();
		symbolInfo* temp = symbol_table.LookUp(name,log_file);
		

		if(temp == NULL){
			generateError("Undeclared variable '"+$1->getName()+"'");
			$$ = new symbolInfo($1->getName(), "undeclared");
		}

		else{
			variable_type = temp->getIdStructure();
			$$ = new symbolInfo($1->getName(), variable_type);
		}

		parse_line = "variable : ID";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

} 		
	 | ID LTHIRD expression RTHIRD{
		//write your code
	
		log_file<< "variable : ID LSQUARE expression RSQUARE  	 " << endl;


		string name = $1->getName();
		variable_type = "variable";

		symbolInfo* temp = symbol_table.LookUp(name,log_file);

		if(temp == NULL){
			generateError("Undeclared variable '"+$1->getName()+"'");
			variable_type = "undeclared";
			$$ = new symbolInfo($1->getName(), variable_type);
		}

		else if(temp->getType() == "ARRAY"){
			variable_type = temp->getIdStructure();
			$$ = new symbolInfo($1->getName(), variable_type);

			//error_file<<$3->getType()<<" "<<$3->getIdStructure()<<endl;

		if($3->getType() != "CONST_INT"){
			generateError("Array subscript is not an integer");
		}
		}

		else{
			generateError("'"+$1->getName() +"' is not an array");
			variable_type = temp->getIdStructure();
			$$ = new symbolInfo($1->getName(), variable_type);
		}


		parse_line = "variable : ID LSQUARE expression RSQUARE";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);

		}
	 ;
	 
 expression : logic_expression{
		//write your code
			$$ = new symbolInfo($1->getName(), $1->getType());
			//error_file<<$1->getType()<<endl;
		log_file<< "expression 	: logic_expression	 " << endl;

		parse_line = "expression : logic_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

 }	
	   | variable ASSIGNOP logic_expression{
		//write your code
		
		log_file<< "expression 	: variable ASSIGNOP logic_expression 		 " << endl;


		string name = $1->getName()+" "+ $2->getName()+" "+ $3->getName();
		string left_type = $1->getType();
		string right_type = $3->getType();
		string type = right_type;

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

		$$ = new symbolInfo($1->getName(), type);

		parse_line = "expression : variable ASSIGNOP logic_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

	   } 	
	   ;
			
logic_expression : rel_expression{
		//write your code
			$$ = new symbolInfo($1->getName(), $1->getType());
			//error_file<<$$->getType()<<endl;
		log_file<< "logic_expression : rel_expression 	 " << endl;

		parse_line = "logic_expression : rel_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

} 	
		 | rel_expression LOGICOP rel_expression{
		//write your code
	
		log_file<< "logic_expression : rel_expression LOGICOP rel_expression " << endl;

		string name = $1->getName()+" "+ $2->getName()+" "+ $3->getName();
		string left_type = $1->getType();
		string right_type = $3->getType();
		string type = "CONST_INT";

		if(checkVoid(left_type)){
		}

		else if(checkVoid(right_type)){
		
		}

		$$ = new symbolInfo($1->getName(), type);
		//error_file<<$$->getType()<<endl;
		
		parse_line = "logic_expression : rel_expression LOGICOP rel_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

		
		 } 	
		 ;
			
rel_expression	: simple_expression{
		//write your code
			$$ = new symbolInfo($1->getName(), $1->getType());
			//error_file<<$$->getType()<<endl;
		log_file<< "rel_expression	: simple_expression " << endl;

		parse_line = "rel_expression : simple_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

} 
		| simple_expression RELOP simple_expression{
		//write your code
			
		log_file<< "rel_expression : simple_expression RELOP simple_expression " << endl;
 
		string name = $1->getName()+" "+ $2->getName()+" "+ $3->getName();
		string left_type = $1->getType();
		string right_type = $3->getType();
		string type = "CONST_INT";
        
		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
			 //type = left_type;
		}

		else{
			HandleTypeCastAvoidSide($1, $3,"Type mismatch");
		}	
		$$ = new symbolInfo(name, type);

		parse_line = "rel_expression : simple_expression RELOP simple_expression";

		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		}
		;
				
simple_expression : term{
		//write your code
		string type = $1->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			$$ = new symbolInfo($1->getName(), type);
			//error_file<<$$->getType()<<endl;
		log_file<< "simple_expression : term " << endl;

		parse_line = "simple_expression : term";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

} 
		  | simple_expression ADDOP term{
		//write your code
		
		log_file<< "simple_expression : simple_expression ADDOP term  " << endl;

		string name = $1->getName()+" "+ $2->getName()+" "+ $3->getName();
		string left_type = $1->getType();
		string right_type = $3->getType();
		string type = right_type;


		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
             type = left_type;
		}

		else{
			type = HandleTypeCastAvoidSide($1, $3, "Type mismatch");
		}
		
			$$ = new symbolInfo(name, type);

		parse_line = "simple_expression : simple_expression ADDOP term";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

		  }
		  ;
					
term :	unary_expression{
		//write your code
		string type = $1->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			$$ = new symbolInfo($1->getName(), type);
		log_file<< "term :	unary_expression " << endl;

		parse_line = "term : unary_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

}
     |  term MULOP unary_expression{
		//write your code
		string name = $1->getName()+" "+ $2->getName()+" "+ $3->getName();
		string left_type = $1->getType();
		string right_type = $3->getType();
		string type = right_type;

		log_file<< "term :	term MULOP unary_expression " << endl;

		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
             type = left_type;
		}

		else if($2->getName() == "%"){
			if(left_type != "CONST_INT" || right_type != "CONST_INT"){
				//type = HandleTypeCheckAvoidSide($1, $3, "Type mismatch");
				generateError("Operands of modulus must be integers ");
			}
			else if($3->getName() == "0"){
				generateError("Warning: division by zero i=0f=1Const=0");
			}
			
				type = "CONST_INT";
		}
		else{
			type = HandleTypeCastAvoidSide($1, $3, "Type mismatch");
		}
		
			$$ = new symbolInfo(name, type);

		parse_line = "term : term MULOP unary_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

		
	 }
     ;

unary_expression : ADDOP unary_expression{
		//write your code
			$$ = new symbolInfo($1->getName()+" "+ $2->getName(), $2->getType());
		log_file<< "unary_expression : ADDOP unary_expression " << endl;

		parse_line = "unary_expression : ADDOP unary_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

  		}  
		 | NOT unary_expression{
		//write your code
			$$ = new symbolInfo("! "+ $2->getName(), $2->getType());
		log_file<< "unary_expression : NOT unary_expression " << endl;

		parse_line = "unary_expression : NOT unary_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

		 } 
		 | factor{
		//write your code
			string type = $1->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			$$ = new symbolInfo($1->getName(), type);
		log_file<< "unary_expression : factor " << endl;

		parse_line = "unary_expression : factor";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

		 } 
		 ;
	
factor	: variable{
		//write your code
		string type = $1->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			$$ = new symbolInfo($1->getName(), type);
		log_file<< "factor	: variable " << endl;

		parse_line = "factor : variable";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

		} 
	| ID LPAREN argument_list RPAREN{
		//write your code
		//$$ = new symbolInfo($1->getName+"("+ $3->getName+")", "variable");
		log_file<< "factor	: ID LPAREN argument_list RPAREN  " << endl;	

		string variable = $1->getName()+"("+ $3->getName()+")";

		symbolInfo* temp = symbol_table.LookUp($1->getName(),log_file);
		if(temp == NULL){
			generateError("Undeclared function '"+ $1->getName()+"'");
			$$ = new symbolInfo(variable, "undeclared");
		}

		//function
		else if(temp->getIsFunction()){
			string return_type = temp->getReturnType();

			if(return_type == "CONST_VOID")
			    return_type = "VOID";

			$$ = new symbolInfo(variable, return_type);
            
	        if(temp->getParameterListSize() != argument_list.size()){

				if(temp->getParameterListSize() < argument_list.size()){
                    generateError("Too many arguments to function '"+ $1->getName()+"'");
				}

				else if(temp->getParameterListSize() > argument_list.size()){
					generateError("Too few arguments to function '"+ $1->getName()+"'");
				}
			
	        }
	        else{
	            for(int i=0; i<argument_list.size(); i++){
	                //create dummy symbolInfo from parameter list and pass it to checkType
					symbolInfo* dummy = new symbolInfo("dummy", temp->getParameter(i));

					//error_file<<dummy->getType()<<" "<<argument_list[i]->getType() <<endl;
					
	                handleTypeCheck(dummy, argument_list[i], "Type mismatch for argument "+to_string(i+1)+" of '"+$1->getName()+"'");
	            }
	        }

			
		}

		//id
		else{
			generateError($1->getName()+" is not a function");
			$$ = new symbolInfo(variable, temp->getIdStructure());
		}

		//clear argument list
		argument_list.clear();

		parse_line = "factor : ID LPAREN argument_list RPAREN";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($4->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);
		$$->addChild($4);

	}
	| LPAREN expression RPAREN{
		//write your code
			$$ = new symbolInfo("("+ $2->getName()+")", $2->getType());
		log_file<< "factor : LPAREN expression RPAREN " << endl;	

		parse_line = "factor : LPAREN expression RPAREN";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

	}
	| CONST_INT{
		//write your code
			$$ = new symbolInfo($1->getName(), $1->getType());
		log_file<< "factor	: CONST_INT   " << endl;

		parse_line = "factor : CONST_INT";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

	} 
	| CONST_FLOAT{
		//write your code
			$$ = new symbolInfo($1->getName(), $1->getType());
		log_file<< "factor	: CONST_FLOAT   " << endl;

		parse_line = "factor : CONST_FLOAT";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

	}

	//Hnadles situation like a=b++ 
	| variable INCOP %prec INCOP_POST{
		//write your code
			$$ = new symbolInfo($1->getName()+"++", $1->getType());
		log_file<< "factor : variable INCOP " << endl;

		parse_line = "factor : variable INCOP";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

	} 
	//Handles situation like a=++b
	| INCOP variable %prec INCOP_PRE{
		//write your code
			$$ = new symbolInfo("++"+$2->getName(), $2->getType());
		log_file<< "factor : INCOP variable " << endl;

		parse_line = "factor : INCOP variable";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

	}
	//Handles situation like a=b--
	| variable DECOP %prec DECOP_POST{
		//write your code
			$$ = new symbolInfo($1->getName()+"--", $1->getType());
		log_file<< "factor : variable DECOP " << endl;

		parse_line = "factor : variable DECOP";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

	}
	//Handles situation like a=--b
	| DECOP variable %prec DECOP_PRE{
		//write your code
			$$ = new symbolInfo("--"+$2->getName(), $2->getType());
		log_file<< "factor : DECOP variable " << endl;

		parse_line = "factor : DECOP variable";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($2->getEndLine());
		$$->addChild($1);
		$$->addChild($2);

	}
	;
	
argument_list : arguments{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
		log_file<< "argument_list : arguments  " << endl;

		parse_line = "argument_list : arguments";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

}
			  | {
		//write your code
			$$ = new symbolInfo(" ", "variable");
		log_file<< "argument_list : " << endl;

		/*parse_line = "argument_list : ";
		$$->setParseLine(parse_line);
		$$->setStartLine(0);
		$$->setEndLine(0);
		//$$->addChild($1);
		*/
			  }
			  ;
	
arguments : arguments COMMA logic_expression{
		//write your code
			$$ = new symbolInfo($1->getName()+" , "+ $3->getName(), "variable");
			//error_file<<$3->getType()<<endl;
			argument_list.push_back(new symbolInfo($3->getName(), $3->getType()));
		log_file<< "arguments : arguments COMMA logic_expression " << endl;

		parse_line = "arguments : arguments COMMA logic_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($3->getEndLine());
		$$->addChild($1);
		$$->addChild($2);
		$$->addChild($3);

}
	      | logic_expression{
		//write your code
			$$ = new symbolInfo($1->getName(), "variable");
			argument_list.push_back(new symbolInfo($1->getName(), $1->getType()));
		log_file<< "arguments : logic_expression" << endl;

		parse_line = "arguments : logic_expression";
		$$->setParseLine(parse_line);
		$$->setStartLine($1->getStartLine());
		$$->setEndLine($1->getEndLine());
		$$->addChild($1);

		  }
	      ;
 

%%
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
	parseTree_file.open("parseTree.txt");

    yyin = fin;
    yyparse();

    //errorFile.close();
	log_file<<"Total Lines: "<<count_line<<endl;
	log_file<<"Total Errors: "<<error_num<<endl;

    log_file.close();
	error_file.close();
	parseTree_file.close();
    fclose(fin);
    
    return 0;
}

