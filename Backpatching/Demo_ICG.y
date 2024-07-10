%{
#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cmath>
#include "1905050.h"
#include<fstream>

using namespace std;

SymbolInfo *s = new SymbolInfo();
SymbolInfo *parser = new SymbolInfo();
string d_type = "";
int id = 1;
int yyparse(void);
int yylex(void);
extern FILE *yyin;
ofstream logout;
ofstream icg;
ofstream tempicg;
extern int line_count;
int err_count = 0;
SymbolTable table1(11);
vector<SymbolInfo*> argList;
unordered_map<int, string> umap;

int globalOffset = 0;
int paramOffset = 4;
int labelCount = 1;
int asmLineCount = 0;
string label;
string retLabel;
vector<int> retFlag;

void yyerror(char *s) {
    // Error handling
}

bool checkTable(SymbolInfo *s1) {
    SymbolInfo *temp = table1.LookupCurr(s1->getName());
    return temp == nullptr;
}

void insertParams() {
    if (!s->isClear()) {
        for (int i = 0; i < s->params.size(); i++) {
            if (table1.LookupCurr(s->params[i].getName()) == nullptr) {
                table1.Insert(s->params[i].getName(), s->params[i].getType(), "null", s->params[i].getDType());
                if (s->params[i].getDType() == "VOID") {
                    s->params[i].setVoidFlag(1);
                }
                SymbolInfo *temp = table1.LookupCurr(s->params[i].getName());
                temp->stackOffset = paramOffset;
                paramOffset += 2;
                temp->asmName = "[BP + " + to_string(temp->stackOffset) + "]";
            } else {
                error << "Line# " << line_count-1 << ": Redefinition of parameter '" << s->params[i].getName() << "'" << endl;
                err_count++;
                break;
            }
        }
        s->clearParam();
    }
}

string newLabel() {
    string s = "L" + to_string(labelCount);
    tempicg << s << ":" << endl;
    labelCount++;
    asmLineCount++;
    return s;
}

void backpatch(vector<int> &list, const string &label) {
    for (int i : list) {
        umap[i] = label;
        cout << i << " - " << label << endl;
    }
    list.clear();
}

%}

%union {
    SymbolInfo* table;
}

%token<table> IF ELSE FOR WHILE INT FLOAT VOID RETURN ASSIGNOP INCOP DECOP NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON PRINTLN
%token<table> CONST_INT CONST_FLOAT ID ADDOP MULOP RELOP LOGICOP

%type<table> start program unit var_declaration func_declaration func_definition type_specifier parameter_list 
%type<table> compound_statement M N statements statement expression_statement ifFor expression checkbool logic_expression 
%type<table> rel_expression simple_expression term unary_expression factor variable argument_list arguments non_array_subscript declaration_list

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

start : program {
    logout << "start : program " << endl;
    $$ = new SymbolInfo("start", "program");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    printTree($$, 0);
}
;

program : program unit {
    logout << "program : program unit " << endl;
    $$ = new SymbolInfo("program", "program unit");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->setStart($1->getStart());
    $$->setFinish($2->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
} | unit {
    logout << "program : unit " << endl;
    $$ = new SymbolInfo("program", "unit");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
}
;

unit : var_declaration {
    logout << "unit : var_declaration  " << endl;
    $$ = new SymbolInfo("unit", "var_declaration");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
} | func_declaration {
    logout << "unit : func_declaration " << endl;
    $$ = new SymbolInfo("unit", "func_declaration");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType(); 
} | func_definition {
    logout << "unit : func_definition  " << endl;
    $$ = new SymbolInfo("unit", "func_definition");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
}
;

func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON {
    logout << "func_declaration : type_specifier ID LPAREN parameter_list RPAREN SEMICOLON " << endl;
    if (checkTable($2)) {
        table1.Insert($2->getName(), "ID", "FUNCTION", $1->getDType());
        SymbolInfo *temp1 = table1.LookupCurr($2->getName());
        for (auto &param : s->params) {
            temp1->add_param(param);
        }
        temp1->paramListSize = temp1->params.size(); 
    } else {
        SymbolInfo *temp = table1.LookupCurr($2->getName());
        if (temp->getType1() != "FUNCTION" || temp->getDType() != $1->getDType()) {
            error << "Line# " << line_count << ": Conflicting types for '" << $2->getName() << "'" << endl;
            err_count++;
        }
    }
    $$ = new SymbolInfo("func_declaration", "type_specifier ID LPAREN parameter_list RPAREN SEMICOLON");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->children.push_back($4);
    $$->children.push_back($5);
    $$->children.push_back($6);
    $$->setStart($1->getStart());
    $$->setFinish($6->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    s->clearParam();
} | type_specifier ID LPAREN RPAREN SEMICOLON {
    logout << "func_declaration : type_specifier ID LPAREN RPAREN SEMICOLON " << endl;
    if (checkTable($2)) {
        table1.Insert($2->getName(), "ID", "FUNCTION", $1->getDType());
        SymbolInfo *temp1 = table1.LookupCurr($2->getName());
        temp1->paramListSize = temp1->params.size();
    } else {
        SymbolInfo *temp = table1.LookupCurr($2->getName());
        if (temp->getType1() != "FUNCTION" || temp->getDType() != $1->getDType()) {
            error << "Line# " << line_count << ": Conflicting types for '" << $2->getName() << "'" << endl;
            err_count++;
        }
    }
    $$ = new SymbolInfo("func_declaration", "type_specifier ID LPAREN RPAREN SEMICOLON");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->children.push_back($4);
    $$->children.push_back($5);
    $$->setStart($1->getStart());
    $$->setFinish($5->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    s->clearParam();
}
;

func_definition : type_specifier ID LPAREN parameter_list RPAREN {
    if (checkTable($2)) {
        table1.Insert($2->getName(), "ID", "FUNCTION", $1->getDType());
        SymbolInfo *temp1 = table1.LookupCurr($2->getName());
        for (auto &param : s->params) {
            temp1->add_param(param);
        }
    } else {
        SymbolInfo *temp = table1.LookupCurr($2->getName());
        if (temp->getType1() != "FUNCTION" || temp->getDType() != $1->getDType() || temp->params.size() != s->params.size()) {
            error << "Line# " << line_count << ": Conflicting types for '" << $2->getName() << "'" << endl;
            err_count++;
        }
    }
    SymbolInfo *temp1 = table1.Lookup($2->getName()); 
    tempicg << $2->getName() << " PROC" << endl;
    asmLineCount++;
    if ($2->getName() == "main") {
        tempicg << "\tMOV AX, @DATA\n\tMOV DS, AX" << endl;
        asmLineCount += 2;
    }
    tempicg << "\tPUSH BP\n\tMOV BP, SP" << endl;
    asmLineCount += 2;
} compound_statement {
    logout << "func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement " << endl;
    $$ = new SymbolInfo("func_definition", "type_specifier ID LPAREN parameter_list RPAREN compound_statement");
    retLabel = newLabel();
    backpatch(retFlag, retLabel);
    tempicg << "\tMOV SP, BP" << endl;
    asmLineCount++;
    tempicg << "\tADD SP, " << globalOffset << endl;
    asmLineCount++;
    tempicg << "\tPOP BP" << endl;
    asmLineCount++;
    if ($2->getName() == "main") {
        tempicg << "    MOV AX,4CH\n    INT 21H" << endl;
        asmLineCount += 2;
    } else {
        tempicg << "\tRET" << endl;
        asmLineCount++;
    }
    tempicg << $2->getName() << " ENDP" << endl;
    asmLineCount++;
    globalOffset = 0;
    paramOffset = 4;
}
;

parameter_list : parameter_list COMMA type_specifier ID {
    logout << "parameter_list : parameter_list COMMA type_specifier ID" << endl;
    SymbolInfo temp($4->getName(), $4->getType(), $4->getType1(), d_type);
    if ($3->getDType() == "VOID") {
        error << "Line# " << line_count << ": Variable or field '" << $4->getName() << "' declared void" << endl;
        err_count++;
    } else {
        s->params.push_back(temp);
    }
    $$ = new SymbolInfo("parameter_list", "parameter_list COMMA type_specifier ID");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->children.push_back($4);
    $$->setStart($1->getStart());
    $$->setFinish($4->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
} | parameter_list COMMA type_specifier {
    logout << "parameter_list : parameter_list COMMA type_specifier" << endl;
    SymbolInfo temp("nameless", "ID", "null", d_type);
    s->params.push_back(temp);
    $$ = new SymbolInfo("parameter_list", "parameter_list COMMA type_specifier");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->setStart($1->getStart());
    $$->setFinish($3->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
} | type_specifier ID {
    logout << "parameter_list : type_specifier ID" << endl;
    SymbolInfo temp($2->getName(), $2->getType(), $2->getType1(), d_type);
    if ($1->getDType() == "VOID") {
        error << "Line# " << line_count << ": Variable or field '" << $2->getName() << "' declared void" << endl;
        err_count++;
    } else {
        s->params.push_back(temp);
    }
    $$ = new SymbolInfo("parameter_list", "type_specifier ID");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->setStart($1->getStart());
    $$->setFinish($2->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
} | type_specifier {
    logout << "parameter_list : type_specifier" << endl;
    SymbolInfo temp("nameless", "ID", "null", d_type);
    s->params.push_back(temp);
    $$ = new SymbolInfo("parameter_list", "type_specifier");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
}
;

compound_statement : LCURL {
    table1.enterScope(id++);
    insertParams();
} statements RCURL {
    logout << "compound_statement : LCURL statements RCURL" << endl;
    $$ = new SymbolInfo("compound_statement", "LCURL statements RCURL");
    $$->children.push_back($1);
    $$->children.push_back($3);
    $$->setStart($1->getStart());
    $$->setFinish($3->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    table1.exitScope();
    backpatch($2->nextlist, $3->label);
}
;

var_declaration : type_specifier declaration_list SEMICOLON {
    logout << "var_declaration : type_specifier declaration_list SEMICOLON" << endl;
    $$ = new SymbolInfo("var_declaration", "type_specifier declaration_list SEMICOLON");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->setStart($1->getStart());
    $$->setFinish($3->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType($1->getDType());
}
;

type_specifier : INT {
    logout << "type_specifier : INT" << endl;
    d_type = "INT";
    $$ = new SymbolInfo("type_specifier", "INT");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType("INT");
} | FLOAT {
    logout << "type_specifier : FLOAT" << endl;
    d_type = "FLOAT";
    $$ = new SymbolInfo("type_specifier", "FLOAT");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType("FLOAT");
} | VOID {
    logout << "type_specifier : VOID" << endl;
    d_type = "VOID";
    $$ = new SymbolInfo("type_specifier", "VOID");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType("VOID");
    $$->setVoidFlag(1);
}
;

declaration_list : declaration_list COMMA ID {
    logout << "declaration_list : declaration_list COMMA ID" << endl;
    if (checkTable($3)) {
        if (d_type == "VOID") {
            error << "Line# " << line_count << ": Variable or field '" << $3->getName() << "' declared void" << endl;
            err_count++;
        } else {
            table1.Insert($3->getName(), "ID", $3->getType1(), d_type);
        }
    } else {
        SymbolInfo *temp = table1.LookupCurr($3->getName());
        if (temp->getDType() != $3->getDType() || temp->getType1() != $3->getType1()) {
            error << "Line# " << line_count << ": Conflicting types for '" << $3->getName() << "'" << endl;
            err_count++;
        }
    }
    $$ = new SymbolInfo("declaration_list", "declaration_list COMMA ID");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->setStart($1->getStart());
    $$->setFinish($3->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    if (table1.getCurrScopeID() == 1) {
        icg << "\t" << $3->getName() << " DW 1 DUP (0000H)" << endl;
        SymbolInfo *temp1 = table1.LookupCurr($3->getName());
        temp1->globalFlag = 1;
        temp1->asmName = $3->getName();
    } else {
        SymbolInfo *temp1 = table1.LookupCurr($3->getName());
        globalOffset += 2;
        temp1->stackOffset = globalOffset;
        tempicg << "\tSUB SP, 2" << endl;
        asmLineCount++;
        string s = "[BP-" + to_string(temp1->stackOffset) + "]";
        temp1->asmName = s;
    }
}
;

M : {
    label = "L" + to_string(labelCount);
    labelCount++;
    $$ = new SymbolInfo();
    $$->label = label;
    tempicg << label << ":" << endl;
    asmLineCount++;
}
;

N : {
    $$ = new SymbolInfo();
    tempicg << "\tJMP " << endl;
    asmLineCount++;
    $$->nextlist.push_back(asmLineCount);
}
;

statements : statement {
    logout << "statements : statement" << endl;
    $$ = new SymbolInfo("statements", "statement");
    $$->nextlist.insert($$->nextlist.end(), $1->nextlist.begin(), $1->nextlist.end());
    $$->truelist.insert($$->truelist.end(), $1->truelist.begin(), $1->truelist.end());
    $$->falselist.insert($$->falselist.end(), $1->falselist.begin(), $1->falselist.end());
} | statements M statement {
    logout << "statements : statements M statement" << endl;
    $$ = new SymbolInfo("statements", "statements M statement");
    backpatch($1->nextlist, $2->label);
    $$->nextlist.insert($$->nextlist.end(), $3->nextlist.begin(), $3->nextlist.end());
    $$->truelist.insert($$->truelist.end(), $3->truelist.begin(), $3->truelist.end());
    $$->falselist.insert($$->falselist.end(), $3->falselist.begin(), $3->falselist.end());
}
;

statement : var_declaration {
    logout << "statement : var_declaration" << endl;
    $$ = new SymbolInfo("statement", "var_declaration");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType($1->getDType());
    if ($1->getVoidFlag() == 1) {
        $$->setVoidFlag(1);
    }
} | expression_statement {
    logout << "statement : expression_statement" << endl;
    $$ = new SymbolInfo("statement", "expression_statement");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType($1->getDType());
    if ($1->getVoidFlag() == 1) {
        $$->setVoidFlag(1);
    }
    $$->nextlist.insert($$->nextlist.end(), $1->nextlist.begin(), $1->nextlist.end());
    $$->truelist.insert($$->truelist.end(), $1->truelist.begin(), $1->truelist.end());
    $$->falselist.insert($$->falselist.end(), $1->falselist.end());
    if (!ifFor) {
        tempicg << "\tPOP AX" << endl;
        asmLineCount++;
    }
    ifFor = 0;
}
;

checkbool : {
    $$ = new SymbolInfo();
    if (simpleFlag == 1) {
        tempicg << "\tCMP AX, 0" << endl;
        asmLineCount++;
        tempicg << "\tJE " << endl;
        asmLineCount++;
        $$->falselist.push_back(asmLineCount);
        tempicg << "\tJMP " << endl;
        asmLineCount++;
        $$->truelist.push_back(asmLineCount);
    }
}
;

ifFor : {
    $$ = new SymbolInfo();
    ifFor = 1;
}
;

expression : logic_expression {
    logout << "expression : logic_expression" << endl;
    $$ = new SymbolInfo("expression", "logic_expression");
    $$->children.push_back($1);
    $$->setStart($1->getStart());
    $$->setFinish($1->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    $$->setDType($1->getDType());
    if (!voidError($$, $1)) {
        checkZero($$, $1);
    }
    $$->setType($1->getType());
    $$->asmName = $1->asmName;
    $$->nextlist.insert($$->nextlist.end(), $1->nextlist.begin(), $1->nextlist.end());
    $$->truelist.insert($$->truelist.end(), $1->truelist.begin(), $1->truelist.end());
    $$->falselist.insert($$->falselist.end(), $1->falselist.begin(), $1->falselist.end());
    $$->asmFlag = $1->asmFlag;
} | variable ASSIGNOP logic_expression {
    logout << "expression : variable ASSIGNOP logic_expression" << endl;
    $$ = new SymbolInfo("expression", "variable ASSIGNOP logic_expression");
    $$->children.push_back($1);
    $$->children.push_back($2);
    $$->children.push_back($3);
    $$->setStart($1->getStart());
    $$->setFinish($3->getFinish());
    $$->sentence += $$->getName() + " : " + $$->getType();
    if ($1->getVoidFlag() == 1 || $3->getVoidFlag() == 1) {
        $$->setVoidFlag(1);
        error << "Line# " << line_count << ": Void cannot be used in expression " << endl;
        err_count++;
    } else {
        if (!assignError($1, $3)) {
            $$->setDType($1->getDType());
        }
    }
    if ($3->asmFlag == 1) {
        string s = newLabel();
        backpatch($3->truelist, s);
        tempicg << "\tMOV AX, 1" << endl;
        asmLineCount++;
        tempicg << "\tJMP " << endl;
        asmLineCount++;
        $3->nextlist.push_back(asmLineCount);
        s = newLabel();
        backpatch($3->falselist, s);
        tempicg << "\tMOV AX, 0" << endl;
        asmLineCount++;
    } else if ($3->asmFlag == 2) {
        string s = newLabel();
        backpatch($3->truelist, s);
        tempicg << "\tMOV AX, 1" << endl;
        asmLineCount++;
        tempicg << "\tJMP " << endl;
        asmLineCount++;
        $3->nextlist.push_back(asmLineCount);
        s = newLabel();
        backpatch($3->falselist, s);
        tempicg << "\tMOV AX, 0" << endl;
        asmLineCount++;
    }
    string s = newLabel();
    backpatch($3->nextlist, s);
    tempicg << "\tMOV " << $1->asmName << ", AX" << endl;
    asmLineCount++;
    tempicg << "\tPUSH AX" << endl;
    asmLineCount++;
}
;
%%

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input file>" << endl;
        return 1;
    }
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        cout << "Cannot open input file." << endl;
        return 1;
    }
    table1.enterScope(id++);
    logout.open("log.txt");
    icg.open("code.asm");
    tempicg.open("temp.asm");
    icg << ".MODEL SMALL\n.STACK 1000H\n.Data\n\tCR EQU 0DH\n\tLF EQU 0AH\n\tnumber DB \"00000$\"\n";
    tempicg << ".CODE" << endl;
    asmLineCount++;
    yyin = fp;
    yyparse();
    tempicg << funcs;
    tempicg << "END main" << endl;
    table1.printAll(logout);
    fclose(yyin);
    logout.close();
    icg.close();
    tempicg.close();
    ifstream obj("temp.asm");
    string line;
    int lcount = 0;
    while (getline(obj, line)) {
        lcount++;
        if (umap.find(lcount) != umap.end()) {
            line += umap[lcount];
        }
        icg << line << endl;
    }
    return 0;
}
