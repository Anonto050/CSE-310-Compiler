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



#line 369 "y.tab.c"

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
#line 301 "1905050.y"

    symbolInfo* symbol_info;

#line 510 "y.tab.c"

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
  YYSYMBOL_func_definition = 48,           /* func_definition  */
  YYSYMBOL_49_1 = 49,                      /* $@1  */
  YYSYMBOL_50_2 = 50,                      /* $@2  */
  YYSYMBOL_new_scope = 51,                 /* new_scope  */
  YYSYMBOL_parameter_list = 52,            /* parameter_list  */
  YYSYMBOL_compound_statement = 53,        /* compound_statement  */
  YYSYMBOL_var_declaration = 54,           /* var_declaration  */
  YYSYMBOL_type_specifier = 55,            /* type_specifier  */
  YYSYMBOL_declaration_list = 56,          /* declaration_list  */
  YYSYMBOL_statements = 57,                /* statements  */
  YYSYMBOL_statement = 58,                 /* statement  */
  YYSYMBOL_expression_statement = 59,      /* expression_statement  */
  YYSYMBOL_variable = 60,                  /* variable  */
  YYSYMBOL_expression = 61,                /* expression  */
  YYSYMBOL_logic_expression = 62,          /* logic_expression  */
  YYSYMBOL_rel_expression = 63,            /* rel_expression  */
  YYSYMBOL_simple_expression = 64,         /* simple_expression  */
  YYSYMBOL_term = 65,                      /* term  */
  YYSYMBOL_unary_expression = 66,          /* unary_expression  */
  YYSYMBOL_factor = 67,                    /* factor  */
  YYSYMBOL_argument_list = 68,             /* argument_list  */
  YYSYMBOL_arguments = 69                  /* arguments  */
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
#define YYLAST   199

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  27
/* YYNRULES -- Number of rules.  */
#define YYNRULES  76
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  139

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
       0,   332,   332,   349,   362,   375,   386,   397,   410,   456,
     504,   504,   536,   536,   568,   620,   656,   692,   721,   751,
     775,   800,   816,   834,   855,   878,   891,   903,   917,   941,
     967,   987,  1008,  1030,  1049,  1054,  1066,  1081,  1093,  1106,
    1119,  1138,  1154,  1173,  1189,  1211,  1246,  1258,  1271,  1289,
    1316,  1363,  1376,  1419,  1432,  1464,  1477,  1510,  1529,  1564,
    1582,  1627,  1640,  1653,  1673,  1691,  1757,  1771,  1783,  1797,
    1811,  1825,  1839,  1854,  1866,  1880,  1896
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
  "program", "unit", "func_declaration", "func_definition", "$@1", "$@2",
  "new_scope", "parameter_list", "compound_statement", "var_declaration",
  "type_specifier", "declaration_list", "statements", "statement",
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

#define YYPACT_NINF (-78)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-20)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      51,   -78,   -78,   -78,    20,    51,   -78,   -78,   -78,   -78,
      13,   -78,   -78,    66,     0,    12,   -78,   -78,    71,    11,
      -7,    -1,   -78,    35,    32,     4,    39,    56,     9,    52,
     -78,    80,    77,    81,    51,   -78,   -78,   -78,   -78,   -78,
      82,   -78,   -78,    51,   -78,    80,    84,    91,    68,   -78,
     -78,   -78,   -78,    98,   100,   101,   102,   162,   162,   -78,
     -78,    53,   -78,   -78,   162,   162,    96,    96,   -78,   -78,
      15,   103,   -78,   -78,    18,    33,   -78,    92,    55,    97,
     -78,   -78,   162,   150,   162,   107,   109,   116,   162,   162,
      63,   -78,   -78,   104,   -78,   -78,   114,   -78,   -78,   162,
     -78,   -78,   -78,   -78,   162,   162,   162,   162,   122,   150,
     125,   126,   -78,   -78,   -78,   130,   127,   131,   -78,   -78,
      97,   124,   -78,   138,   162,   138,   132,   -78,   162,   -78,
     152,   140,   -78,   -78,   -78,   138,   138,   -78,   -78
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    25,    26,    27,     0,     2,     4,     6,     7,     5,
       0,     1,     3,     0,    31,     0,    24,    32,     0,     0,
       0,     0,    23,    12,     0,     0,     0,     0,     0,    28,
       9,     0,     0,    10,     0,    20,    16,    34,    33,    29,
       0,    14,    13,     0,     8,     0,    17,     0,     0,    18,
      11,    15,    30,     0,     0,     0,     0,     0,     0,    22,
      46,    49,    67,    68,     0,     0,     0,     0,    39,    37,
       0,     0,    35,    38,    64,     0,    51,    53,    55,    57,
      59,    63,     0,     0,     0,     0,     0,     0,    74,     0,
      64,    61,    62,    49,    70,    72,    31,    21,    36,     0,
      69,    71,    48,    47,     0,     0,     0,     0,     0,     0,
       0,     0,    45,    66,    76,     0,    73,     0,    52,    54,
      58,    56,    60,     0,     0,     0,     0,    65,     0,    50,
      41,     0,    43,    44,    75,     0,     0,    42,    40
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -78,   -78,   -78,   156,   -78,   -78,   -78,   -78,   -78,   -78,
     -10,    60,    10,   -78,   -78,   -67,   -66,   -58,   -57,   -77,
      58,    57,    65,   -62,   -78,   -78,   -78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,     6,     7,     8,    45,    31,    48,    24,
      68,    69,    70,    15,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,   115,   116
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      86,    87,    91,    92,    98,    35,    90,    90,    94,    95,
      10,   114,    26,    20,    13,    10,    13,   109,    28,    18,
      11,    42,   118,    19,   -19,   108,    29,   110,    25,   -19,
      90,    36,   117,    32,   102,    50,    39,    21,    22,    27,
      14,    90,    96,   124,    46,   122,    90,    90,    90,    90,
      99,   134,    33,    49,   100,   101,   130,    34,   132,   103,
       9,    30,     1,    37,     2,     9,     3,   131,   137,   138,
      90,    53,    88,    54,    55,    40,    89,    56,    57,     1,
      38,     2,     1,     3,     2,   105,     3,    58,   106,    41,
      59,    23,    16,    17,    60,    61,    62,    63,    64,   100,
     101,    41,    43,    65,    66,    67,    53,    44,    54,    55,
      47,    51,    56,    57,     1,    52,     2,    82,     3,    83,
      84,    85,    58,    93,    41,    97,   104,    89,   107,    60,
      61,    62,    63,    64,   111,   112,   113,    19,    65,    66,
      67,    53,   123,    54,    55,   125,   126,    56,    57,     1,
     127,     2,   128,     3,   105,   129,   135,    58,   133,    41,
     136,    12,   119,   121,    60,    61,    62,    63,    64,    58,
     120,     0,     0,    65,    66,    67,    60,    61,    62,    63,
      64,    58,     0,     0,     0,    65,    66,    67,     0,    61,
      62,    63,    64,     0,     0,     0,     0,    65,    66,    67
};

static const yytype_int16 yycheck[] =
{
      57,    58,    64,    65,    71,     1,    64,    65,    66,    67,
       0,    88,     1,     1,     1,     5,     1,    83,    25,    19,
       0,    31,    99,    23,    20,    82,    27,    84,    18,    25,
      88,    27,    89,     1,     1,    45,    27,    25,    26,    28,
      27,    99,    27,   109,    34,   107,   104,   105,   106,   107,
      32,   128,    20,    43,    36,    37,   123,    25,   125,    26,
       0,    26,    11,    24,    13,     5,    15,   124,   135,   136,
     128,     3,    19,     5,     6,    23,    23,     9,    10,    11,
      24,    13,    11,    15,    13,    30,    15,    19,    33,    21,
      22,    20,    26,    27,    26,    27,    28,    29,    30,    36,
      37,    21,    25,    35,    36,    37,     3,    26,     5,     6,
      28,    27,     9,    10,    11,    24,    13,    19,    15,    19,
      19,    19,    19,    27,    21,    22,    34,    23,    31,    26,
      27,    28,    29,    30,    27,    26,    20,    23,    35,    36,
      37,     3,    20,     5,     6,    20,    20,     9,    10,    11,
      20,    13,    25,    15,    30,    24,     4,    19,    26,    21,
      20,     5,   104,   106,    26,    27,    28,    29,    30,    19,
     105,    -1,    -1,    35,    36,    37,    26,    27,    28,    29,
      30,    19,    -1,    -1,    -1,    35,    36,    37,    -1,    27,
      28,    29,    30,    -1,    -1,    -1,    -1,    35,    36,    37
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    11,    13,    15,    44,    45,    46,    47,    48,    54,
      55,     0,    46,     1,    27,    56,    26,    27,    19,    23,
       1,    25,    26,    20,    52,    55,     1,    28,    25,    27,
      26,    50,     1,    20,    25,     1,    27,    24,    24,    27,
      23,    21,    53,    25,    26,    49,    55,    28,    51,    55,
      53,    27,    24,     3,     5,     6,     9,    10,    19,    22,
      26,    27,    28,    29,    30,    35,    36,    37,    53,    54,
      55,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    19,    19,    19,    19,    61,    61,    19,    23,
      60,    66,    66,    27,    60,    60,    27,    22,    58,    32,
      36,    37,     1,    26,    34,    30,    33,    31,    61,    59,
      61,    27,    26,    20,    62,    68,    69,    61,    62,    63,
      65,    64,    66,    20,    59,    20,    20,    20,    25,    24,
      58,    61,    58,    26,    62,     4,    20,    58,    58
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    46,    47,    47,
      49,    48,    50,    48,    51,    52,    52,    52,    52,    52,
      52,    53,    53,    54,    54,    55,    55,    55,    56,    56,
      56,    56,    56,    56,    56,    57,    57,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    59,    59,    59,    60,
      60,    61,    61,    62,    62,    63,    63,    64,    64,    65,
      65,    66,    66,    66,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    68,    68,    69,    69
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     6,     5,
       0,     7,     0,     6,     0,     4,     2,     3,     4,     1,
       2,     4,     3,     3,     3,     1,     1,     1,     3,     4,
       6,     1,     2,     4,     4,     1,     2,     1,     1,     1,
       7,     5,     7,     5,     5,     3,     1,     2,     2,     1,
       4,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     2,     2,     1,     1,     4,     3,     1,     1,     2,
       2,     2,     2,     1,     0,     3,     1
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
#line 333 "1905050.y"
        {
		//write your code in this block in all the similar blocks below
        (yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "start : program " << endl;
		
		parse_line = "start : program";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		printTree((yyval.symbol_info), 0);
		
	}
#line 1944 "y.tab.c"
    break;

  case 3: /* program: program unit  */
#line 349 "1905050.y"
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
#line 1962 "y.tab.c"
    break;

  case 4: /* program: unit  */
#line 362 "1905050.y"
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
#line 1978 "y.tab.c"
    break;

  case 5: /* unit: var_declaration  */
#line 375 "1905050.y"
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
#line 1994 "y.tab.c"
    break;

  case 6: /* unit: func_declaration  */
#line 386 "1905050.y"
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
#line 2010 "y.tab.c"
    break;

  case 7: /* unit: func_definition  */
#line 397 "1905050.y"
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
#line 2026 "y.tab.c"
    break;

  case 8: /* func_declaration: type_specifier ID LPAREN parameter_list RPAREN SEMICOLON  */
#line 410 "1905050.y"
                                                                           {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-5].symbol_info)->getName()+" "+ (yyvsp[-4].symbol_info)->getName()+"("+ (yyvsp[-2].symbol_info)->getName()+")"+";", "variable");
		log_file<< "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON " << endl;

		parse_line = "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-5].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
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
#line 2077 "y.tab.c"
    break;

  case 9: /* func_declaration: type_specifier ID LPAREN RPAREN SEMICOLON  */
#line 456 "1905050.y"
                                                           {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-4].symbol_info)->getName()+" "+ (yyvsp[-3].symbol_info)->getName()+"("+")"+";", "variable");
		log_file<< "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON " << endl;

		parse_line = "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-4].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		string str = UpperCase((yyvsp[-4].symbol_info)->getName());
			if(str.find("CONST_") != string::npos){
				str = str;
			}
			else{
				str = "CONST_" + str;
			}

		string return_type = str;
		string name = (yyvsp[-3].symbol_info)->getName();

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
#line 2127 "y.tab.c"
    break;

  case 10: /* $@1: %empty  */
#line 504 "1905050.y"
                                                                 {string return_type = "CONST_"+UpperCase((yyvsp[-4].symbol_info)->getName());string name = (yyvsp[-3].symbol_info)->getName();checkFunctionDeclaration(name, return_type);}
#line 2133 "y.tab.c"
    break;

  case 11: /* func_definition: type_specifier ID LPAREN parameter_list RPAREN $@1 compound_statement  */
#line 504 "1905050.y"
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

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();


		}
#line 2170 "y.tab.c"
    break;

  case 12: /* $@2: %empty  */
#line 536 "1905050.y"
                                                  {string return_type = "CONST_"+UpperCase((yyvsp[-3].symbol_info)->getName());string name = (yyvsp[-2].symbol_info)->getName();checkFunctionDeclaration(name, return_type);}
#line 2176 "y.tab.c"
    break;

  case 13: /* func_definition: type_specifier ID LPAREN RPAREN $@2 compound_statement  */
#line 536 "1905050.y"
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

		function_name = "";
		function_type = "";
		//clear parameter list
		parameter_list.clear();
		}
#line 2210 "y.tab.c"
    break;

  case 14: /* new_scope: %empty  */
#line 568 "1905050.y"
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
#line 2265 "y.tab.c"
    break;

  case 15: /* parameter_list: parameter_list COMMA type_specifier ID  */
#line 620 "1905050.y"
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
#line 2306 "y.tab.c"
    break;

  case 16: /* parameter_list: type_specifier ID  */
#line 656 "1905050.y"
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
#line 2347 "y.tab.c"
    break;

  case 17: /* parameter_list: parameter_list COMMA type_specifier  */
#line 692 "1905050.y"
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
#line 2381 "y.tab.c"
    break;

  case 18: /* parameter_list: parameter_list error COMMA type_specifier  */
#line 721 "1905050.y"
                                                           {
			
			(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "parameter_list  : parameter_list COMMA type_specifier " << endl;

		parse_line = "parameter_list : parameter_list COMMA type_specifier";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));



		//string name = $4->getName();
		string type = (yyvsp[0].symbol_info)->getName();

		
		
		parameter_list.push_back(make_pair("",type)); 
		


		if(type == "void"){         
          generateError("Variable or field  declared void");
        }
		generateError("Syntax error at parameter_list");
	
		}
#line 2416 "y.tab.c"
    break;

  case 19: /* parameter_list: type_specifier  */
#line 751 "1905050.y"
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
#line 2445 "y.tab.c"
    break;

  case 20: /* parameter_list: type_specifier error  */
#line 775 "1905050.y"
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
#line 2472 "y.tab.c"
    break;

  case 21: /* compound_statement: LCURL new_scope statements RCURL  */
#line 800 "1905050.y"
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
#line 2493 "y.tab.c"
    break;

  case 22: /* compound_statement: LCURL new_scope RCURL  */
#line 816 "1905050.y"
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
#line 2514 "y.tab.c"
    break;

  case 23: /* var_declaration: type_specifier declaration_list SEMICOLON  */
#line 834 "1905050.y"
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
#line 2540 "y.tab.c"
    break;

  case 24: /* var_declaration: type_specifier error SEMICOLON  */
#line 855 "1905050.y"
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
#line 2566 "y.tab.c"
    break;

  case 25: /* type_specifier: INT  */
#line 878 "1905050.y"
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
#line 2584 "y.tab.c"
    break;

  case 26: /* type_specifier: FLOAT  */
#line 891 "1905050.y"
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
#line 2601 "y.tab.c"
    break;

  case 27: /* type_specifier: VOID  */
#line 903 "1905050.y"
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
#line 2618 "y.tab.c"
    break;

  case 28: /* declaration_list: declaration_list COMMA ID  */
#line 917 "1905050.y"
                                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName()+","+ (yyvsp[0].symbol_info)->getName(), "variable");


			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable((yyvsp[0].symbol_info)->getName(), variable_type);

		log_file<< "declaration_list : declaration_list COMMA ID  " << endl;

		parse_line = "declaration_list : declaration_list COMMA ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 2647 "y.tab.c"
    break;

  case 29: /* declaration_list: declaration_list error COMMA ID  */
#line 941 "1905050.y"
                                                   {
		//write your code
		(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName()+","+ (yyvsp[0].symbol_info)->getName(), "variable");


			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable((yyvsp[0].symbol_info)->getName(), variable_type);

		log_file<< "declaration_list : declaration_list COMMA ID  " << endl;

		parse_line = "declaration_list : declaration_list COMMA ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		generateError("Syntax error at declaration_list");
		  
		  }
#line 2678 "y.tab.c"
    break;

  case 30: /* declaration_list: declaration_list COMMA ID LTHIRD CONST_INT RTHIRD  */
#line 967 "1905050.y"
                                                                     {
		//write your code	
			(yyval.symbol_info) = new symbolInfo((yyvsp[-5].symbol_info)->getName()+","+ (yyvsp[-3].symbol_info)->getName()+"["+ (yyvsp[-1].symbol_info)->getName()+"]", "variable");

			variable_type = variable_type+"[]";
			insertVariable((yyvsp[-3].symbol_info)->getName(), variable_type);
		log_file<< "declaration_list : declaration_list COMMA ID LSQUARE CONST_INT RSQUARE " << endl;

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
#line 2703 "y.tab.c"
    break;

  case 31: /* declaration_list: ID  */
#line 987 "1905050.y"
                      {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");

			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable((yyvsp[0].symbol_info)->getName(), variable_type);
		log_file<< "declaration_list : ID " << endl;

		parse_line = "declaration_list : ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


		  }
#line 2729 "y.tab.c"
    break;

  case 32: /* declaration_list: error ID  */
#line 1008 "1905050.y"
                            {
					//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), "variable");

			if(variable_type.find("[]") != string::npos){
			//array
			
			variable_type = variable_type.substr(0, variable_type.size()-2);
		}

			insertVariable((yyvsp[0].symbol_info)->getName(), variable_type);
		log_file<< "declaration_list : ID " << endl;

		parse_line = "declaration_list : ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		generateError("Syntax error at declaration_list");

		  }
#line 2756 "y.tab.c"
    break;

  case 33: /* declaration_list: ID LTHIRD CONST_INT RTHIRD  */
#line 1030 "1905050.y"
                                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-3].symbol_info)->getName()+"["+ (yyvsp[-1].symbol_info)->getName()+"]", "variable");

			variable_type = variable_type+"[]";
			insertVariable((yyvsp[-3].symbol_info)->getName(), variable_type);
		log_file<< "declaration_list : ID LSQUARE CONST_INT RSQUARE " << endl;

		parse_line = "declaration_list : ID LSQUARE CONST_INT RSQUARE";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		  }
#line 2779 "y.tab.c"
    break;

  case 34: /* declaration_list: ID LTHIRD error RTHIRD  */
#line 1049 "1905050.y"
                                          {

		  }
#line 2787 "y.tab.c"
    break;

  case 35: /* statements: statement  */
#line 1054 "1905050.y"
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
#line 2804 "y.tab.c"
    break;

  case 36: /* statements: statements statement  */
#line 1066 "1905050.y"
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
#line 2822 "y.tab.c"
    break;

  case 37: /* statement: var_declaration  */
#line 1081 "1905050.y"
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
#line 2839 "y.tab.c"
    break;

  case 38: /* statement: expression_statement  */
#line 1093 "1905050.y"
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
#line 2857 "y.tab.c"
    break;

  case 39: /* statement: compound_statement  */
#line 1106 "1905050.y"
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
#line 2875 "y.tab.c"
    break;

  case 40: /* statement: FOR LPAREN expression_statement expression_statement expression RPAREN statement  */
#line 1119 "1905050.y"
                                                                                            {
		//write your code
			(yyval.symbol_info) = new symbolInfo("for("+ (yyvsp[-4].symbol_info)->getName()+";"+ (yyvsp[-3].symbol_info)->getName()+";"+ (yyvsp[-2].symbol_info)->getName()+")"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement " << endl;

		parse_line = "statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-6].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));


	  }
#line 2899 "y.tab.c"
    break;

  case 41: /* statement: IF LPAREN expression RPAREN statement  */
#line 1138 "1905050.y"
                                                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo("if("+ (yyvsp[-2].symbol_info)->getName()+")"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : IF LPAREN expression RPAREN statement " << endl;

		parse_line = "statement : IF LPAREN expression RPAREN statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-4].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	  }
#line 2920 "y.tab.c"
    break;

  case 42: /* statement: IF LPAREN expression RPAREN statement ELSE statement  */
#line 1154 "1905050.y"
                                                                {
		//write your code
			(yyval.symbol_info) = new symbolInfo("if("+ (yyvsp[-4].symbol_info)->getName()+")"+ (yyvsp[-2].symbol_info)->getName()+"else"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : IF LPAREN expression RPAREN statement ELSE statement " << endl;

		parse_line = "statement : IF LPAREN expression RPAREN statement ELSE statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-6].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-6].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-5].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	  }
#line 2943 "y.tab.c"
    break;

  case 43: /* statement: WHILE LPAREN expression RPAREN statement  */
#line 1173 "1905050.y"
                                                    {
		//write your code
			(yyval.symbol_info) = new symbolInfo("while("+ (yyvsp[-2].symbol_info)->getName()+")"+ (yyvsp[0].symbol_info)->getName(), "variable");
		log_file<< "statement : WHILE LPAREN expression RPAREN statement " << endl;

		parse_line = "statement : WHILE LPAREN expression RPAREN statement";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-4].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-4].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	  }
#line 2964 "y.tab.c"
    break;

  case 44: /* statement: PRINTLN LPAREN ID RPAREN SEMICOLON  */
#line 1189 "1905050.y"
                                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo("println("+ (yyvsp[-2].symbol_info)->getName()+");", "variable");
			string name = (yyvsp[-2].symbol_info)->getName();
			symbolInfo* temp = symbol_table.LookUp(name,log_file);
 
			if(temp == NULL){
				generateError("Undeclared variable '"+name+"'");
			}
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
#line 2991 "y.tab.c"
    break;

  case 45: /* statement: RETURN expression SEMICOLON  */
#line 1211 "1905050.y"
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
#line 3029 "y.tab.c"
    break;

  case 46: /* expression_statement: SEMICOLON  */
#line 1246 "1905050.y"
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
#line 3046 "y.tab.c"
    break;

  case 47: /* expression_statement: expression SEMICOLON  */
#line 1258 "1905050.y"
                                              {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+";", (yyvsp[-1].symbol_info)->getType());
		log_file<< "expression_statement : expression SEMICOLON 		 " << endl;

		parse_line = "expression_statement : expression SEMICOLON";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

			}
#line 3064 "y.tab.c"
    break;

  case 48: /* expression_statement: expression error  */
#line 1271 "1905050.y"
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
#line 3084 "y.tab.c"
    break;

  case 49: /* variable: ID  */
#line 1289 "1905050.y"
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
		}

		parse_line = "variable : ID";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

}
#line 3116 "y.tab.c"
    break;

  case 50: /* variable: ID LTHIRD expression RTHIRD  */
#line 1316 "1905050.y"
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


		parse_line = "variable : ID LSQUARE expression RSQUARE";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-3].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-3].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		}
#line 3166 "y.tab.c"
    break;

  case 51: /* expression: logic_expression  */
#line 1363 "1905050.y"
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
#line 3184 "y.tab.c"
    break;

  case 52: /* expression: variable ASSIGNOP logic_expression  */
#line 1376 "1905050.y"
                                               {
		//write your code
		
		log_file<< "expression 	: variable ASSIGNOP logic_expression 		 " << endl;


		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
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

		(yyval.symbol_info) = new symbolInfo((yyvsp[-2].symbol_info)->getName(), type);

		parse_line = "expression : variable ASSIGNOP logic_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	   }
#line 3230 "y.tab.c"
    break;

  case 53: /* logic_expression: rel_expression  */
#line 1419 "1905050.y"
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
#line 3248 "y.tab.c"
    break;

  case 54: /* logic_expression: rel_expression LOGICOP rel_expression  */
#line 1432 "1905050.y"
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
		
		parse_line = "logic_expression : rel_expression LOGICOP rel_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		
		 }
#line 3283 "y.tab.c"
    break;

  case 55: /* rel_expression: simple_expression  */
#line 1464 "1905050.y"
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
#line 3301 "y.tab.c"
    break;

  case 56: /* rel_expression: simple_expression RELOP simple_expression  */
#line 1477 "1905050.y"
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

		parse_line = "rel_expression : simple_expression RELOP simple_expression";

		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));
		}
#line 3337 "y.tab.c"
    break;

  case 57: /* simple_expression: term  */
#line 1510 "1905050.y"
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
#line 3361 "y.tab.c"
    break;

  case 58: /* simple_expression: simple_expression ADDOP term  */
#line 1529 "1905050.y"
                                                {
		//write your code
		
		log_file<< "simple_expression : simple_expression ADDOP term  " << endl;

		string name = (yyvsp[-2].symbol_info)->getName()+" "+ (yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName();
		string left_type = (yyvsp[-2].symbol_info)->getType();
		string right_type = (yyvsp[0].symbol_info)->getType();
		string type = right_type;


		if(checkVoid(left_type)){

		}
		else if(checkVoid(right_type)){
             type = left_type;
		}

		else{
			type = HandleTypeCastAvoidSide((yyvsp[-2].symbol_info), (yyvsp[0].symbol_info), "Type mismatch");
		}
		
			(yyval.symbol_info) = new symbolInfo(name, type);

		parse_line = "simple_expression : simple_expression ADDOP term";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		  }
#line 3399 "y.tab.c"
    break;

  case 59: /* term: unary_expression  */
#line 1564 "1905050.y"
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
#line 3422 "y.tab.c"
    break;

  case 60: /* term: term MULOP unary_expression  */
#line 1582 "1905050.y"
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

		parse_line = "term : term MULOP unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-2].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-2].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		
	 }
#line 3470 "y.tab.c"
    break;

  case 61: /* unary_expression: ADDOP unary_expression  */
#line 1627 "1905050.y"
                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+" "+ (yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "unary_expression : ADDOP unary_expression " << endl;

		parse_line = "unary_expression : ADDOP unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

  		}
#line 3488 "y.tab.c"
    break;

  case 62: /* unary_expression: NOT unary_expression  */
#line 1640 "1905050.y"
                                       {
		//write your code
			(yyval.symbol_info) = new symbolInfo("! "+ (yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "unary_expression : NOT unary_expression " << endl;

		parse_line = "unary_expression : NOT unary_expression";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		 }
#line 3506 "y.tab.c"
    break;

  case 63: /* unary_expression: factor  */
#line 1653 "1905050.y"
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
#line 3529 "y.tab.c"
    break;

  case 64: /* factor: variable  */
#line 1673 "1905050.y"
                  {
		//write your code
		string type = (yyvsp[0].symbol_info)->getType();
		//If type doesn't contain "CONST_" then add it to type
		if(type.find("CONST_") == string::npos){
			type = "CONST_"+type;
		}

			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), type);
		log_file<< "factor	: variable " << endl;

		parse_line = "factor : variable";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

		}
#line 3552 "y.tab.c"
    break;

  case 65: /* factor: ID LPAREN argument_list RPAREN  */
#line 1691 "1905050.y"
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
#line 3623 "y.tab.c"
    break;

  case 66: /* factor: LPAREN expression RPAREN  */
#line 1757 "1905050.y"
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
#line 3642 "y.tab.c"
    break;

  case 67: /* factor: CONST_INT  */
#line 1771 "1905050.y"
                   {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor	: CONST_INT   " << endl;

		parse_line = "factor : CONST_INT";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[0].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 3659 "y.tab.c"
    break;

  case 68: /* factor: CONST_FLOAT  */
#line 1783 "1905050.y"
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
#line 3676 "y.tab.c"
    break;

  case 69: /* factor: variable INCOP  */
#line 1797 "1905050.y"
                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+"++", (yyvsp[-1].symbol_info)->getType());
		log_file<< "factor : variable INCOP " << endl;

		parse_line = "factor : variable INCOP";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 3694 "y.tab.c"
    break;

  case 70: /* factor: INCOP variable  */
#line 1811 "1905050.y"
                                        {
		//write your code
			(yyval.symbol_info) = new symbolInfo("++"+(yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor : INCOP variable " << endl;

		parse_line = "factor : INCOP variable";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 3712 "y.tab.c"
    break;

  case 71: /* factor: variable DECOP  */
#line 1825 "1905050.y"
                                         {
		//write your code
			(yyval.symbol_info) = new symbolInfo((yyvsp[-1].symbol_info)->getName()+"--", (yyvsp[-1].symbol_info)->getType());
		log_file<< "factor : variable DECOP " << endl;

		parse_line = "factor : variable DECOP";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 3730 "y.tab.c"
    break;

  case 72: /* factor: DECOP variable  */
#line 1839 "1905050.y"
                                        {
		//write your code
			(yyval.symbol_info) = new symbolInfo("--"+(yyvsp[0].symbol_info)->getName(), (yyvsp[0].symbol_info)->getType());
		log_file<< "factor : DECOP variable " << endl;

		parse_line = "factor : DECOP variable";
		(yyval.symbol_info)->setParseLine(parse_line);
		(yyval.symbol_info)->setStartLine((yyvsp[-1].symbol_info)->getStartLine());
		(yyval.symbol_info)->setEndLine((yyvsp[0].symbol_info)->getEndLine());
		(yyval.symbol_info)->addChild((yyvsp[-1].symbol_info));
		(yyval.symbol_info)->addChild((yyvsp[0].symbol_info));

	}
#line 3748 "y.tab.c"
    break;

  case 73: /* argument_list: arguments  */
#line 1854 "1905050.y"
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
#line 3765 "y.tab.c"
    break;

  case 74: /* argument_list: %empty  */
#line 1866 "1905050.y"
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
#line 3782 "y.tab.c"
    break;

  case 75: /* arguments: arguments COMMA logic_expression  */
#line 1880 "1905050.y"
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
#line 3803 "y.tab.c"
    break;

  case 76: /* arguments: logic_expression  */
#line 1896 "1905050.y"
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
#line 3821 "y.tab.c"
    break;


#line 3825 "y.tab.c"

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

#line 1912 "1905050.y"

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

