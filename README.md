

# CSE 310 `Compiler`

This repository contains the implementation of assignments from the Compiler Sessional course, offered in the Department of Computer Science and Engineering at Bangladesh University of Engineering and Technology (BUET). The course focuses on developing a C compiler that performs lexical, syntax, and semantic analysis, and generates intermediate code.

---

## Table of Contents
- [Introduction](#introduction)
- [Getting Started](#getting-started)
- [Assignments](#assignments)
  - [Offline 1: Symbol Table](#offline-1-symbol-table)
  - [Offline 2: Lexical Analysis (FLEX)](#offline-2-lexical-analysis-flex)
  - [Offline 3: Syntax & Semantic Analysis (YACC/BISON)](#offline-3-syntax--semantic-analysis-yaccbison)
  - [Offline 4: Intermediate Code Generation](#offline-4-intermediate-code-generation)
- [Project: C Compiler](#project-c-compiler)
  - [Part 1: Symbol Table](#part-1-symbol-table)
  - [Part 2: Lexical Analysis](#part-2-lexical-analysis)
  - [Part 3: Syntax and Semantic Analysis](#part-3-syntax-and-semantic-analysis)
  - [Part 4: Error Recovery](#part-4-error-recovery)
  - [Part 5: Intermediate Code Generation](#part-5-intermediate-code-generation)
- [Disclaimer](#disclaimer)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This repository contains the implementation of various assignments for the CSE-310 Compiler Sessional course. The primary objective is to develop a C compiler that performs comprehensive lexical, syntax, and semantic analysis and generates intermediate code for the 8086 assembly language.

## Getting Started

To get started with this repository:

1. **Clone the repository**:
    ```bash
    git clone https://github.com/Anonto050/CSE-310-Compiler.git
    ```

2. **Navigate to the project directory**:
    ```bash
    cd CSE-310-Compiler
    ```

3. **Build the project**:
    ```bash
    make
    ```

4. **Run the compiler** on a C file:
    ```bash
    ./compiler your_program.c
    ```

## Assignments

### Offline 1: Symbol Table

**Symbol Table**
- [Problem Specification](Offline/Offline%201/Assignment1_Specification.pdf)
- [Solution](Offline/Offline%201/1705010)

### Offline 2: Lexical Analysis (FLEX)

**Lexical Analysis**
- [Problem Specification](Offline/Offline%202/Assignment%202%20Specification.pdf)
- [Solution](Offline/Offline%202/1705010)

### Offline 3: Syntax & Semantic Analysis (YACC/BISON)

**Syntax & Semantic Analysis**
- [Problem Specification](Offline/Offline%203/CSE310_January_2021_YACC_Assignment_Spec.pdf)
- [Solution](Offline/Offline%203/1705010)

### Offline 4: Intermediate Code Generation

**Intermediate Code Generation**
- [Problem Specification](Offline/Offline%204/CSE_310_January_2021_ICG_Spec.pdf)
- [Solution](Offline/Offline%204/1705010)

## Project: C Compiler

This project involves the development of a C compiler that performs lexical, syntax, and semantic analysis, and generates intermediate code. The compiler covers a subset of the C language and uses Flex (lexer) and Bison (YACC parser) for analysis.

### Part 1: Symbol Table

#### Overview

A Symbol Table is a data structure maintained by compilers to store information about the occurrence of various entities such as identifiers, objects, function names, etc. It helps in organizing the program's symbols and facilitates efficient lookup, insertion, and deletion operations.

#### Implementation

The Symbol Table is implemented using Scope Tables. Each Scope Table contains information about symbols within a specific scope. Here are the key components:

- **SymbolInfo**: Stores information about a symbol, such as its type, value, and scope.
- **ScopeTable**: Maintains a hash table for symbols within a scope, allowing for quick lookup and management of symbols.
- **SymbolTable**: Manages multiple scopes, allowing for entering and exiting scopes, which is essential for handling nested scopes in a program.

#### Example

```cpp
class SymbolInfo {
public:
    string name;
    string type;
    // Additional attributes can be added as needed
    SymbolInfo(string name, string type) : name(name), type(type) {}
};

class ScopeTable {
private:
    unordered_map<string, SymbolInfo*> table;
    int id;
    ScopeTable* parentScope;
public:
    ScopeTable(int id, ScopeTable* parent) : id(id), parentScope(parent) {}
    bool insert(SymbolInfo* symbol) {
        if (table.find(symbol->name) == table.end()) {
            table[symbol->name] = symbol;
            return true;
        }
        return false;
    }
    SymbolInfo* lookup(string name) {
        if (table.find(name) != table.end()) {
            return table[name];
        }
        if (parentScope != nullptr) {
            return parentScope->lookup(name);
        }
        return nullptr;
    }
    // Other methods for deletion, printing, etc.
};

class SymbolTable {
private:
    ScopeTable* currentScope;
    int currentScopeId;
public:
    SymbolTable() : currentScopeId(1) {
        currentScope = new ScopeTable(currentScopeId, nullptr);
    }
    void enterScope() {
        currentScopeId++;
        currentScope = new ScopeTable(currentScopeId, currentScope);
    }
    void exitScope() {
        if (currentScope->getParentScope() != nullptr) {
            ScopeTable* temp = currentScope;
            currentScope = currentScope->getParentScope();
            delete temp;
        }
    }
    bool insert(SymbolInfo* symbol) {
        return currentScope->insert(symbol);
    }
    SymbolInfo* lookup(string name) {
        return currentScope->lookup(name);
    }
    // Other methods for managing scopes and symbols
};
```

### Part 2: Lexical Analysis

#### Overview

Lexical analysis is the process of converting a sequence of characters into a sequence of tokens. This step is performed by the lexical analyzer (lexer), which identifies meaningful lexemes and classifies them into token types. In this project, we use `flex` to define the lexer.

#### Flex File Structure

A flex file consists of three sections:
1. **Definitions**: Define macros and include headers.
2. **Rules**: Specify regular expressions and the corresponding actions.
3. **User Code**: Additional C/C++ code used by the lexer.

#### Example Flex File

```c
%{
#include "y.tab.h"
%}

WHITESPACE [ \t\f\r\v]+ 
LETTER [a-zA-Z_]
DIGIT [0-9]
NEWLINE [\r]?\n
ALNUM [A-Za-z_0-9]
ALL_EXCEPT_BACKSLASH [^\\]

%%

{LETTER}{ALNUM}* {
    yylval.symbol_info = new SymbolInfo(yytext, "ID");
    return ID;
}

"+"|"-" {
    yylval.symbol_info = new SymbolInfo(yytext, "ADDOP");
    return ADDOP;
}

. { 
    // Handle other tokens
}

%%
```

In the example above, the lexer identifies identifiers and addition operators and returns the corresponding tokens to the parser.

### Part 3: Syntax and Semantic Analysis

#### Overview

The parser takes the sequence of tokens produced by the lexer and verifies that the sequence conforms to the grammar of the source language. This step is known as syntax analysis. Semantic analysis ensures that the program's constructs are semantically correct.

#### Bison File Structure

A Bison file consists of three sections:
1. **Definitions**: Define tokens, data types, and precedence rules.
2. **Rules**: Specify the grammar rules and corresponding actions.
3. **User Code**: Additional C/C++ code used by the parser.

#### Example Bison File

```bison
%{
#include "symbol_table.h"
void yyerror(const char* s);
%}

%union {
    SymbolInfo* symbol_info;
    // Other types
}

%token <symbol_info> ID ADDOP

%type <symbol_info> expression term factor

%%

start : program
      ;

program : program unit
        | unit
        ;

unit : var_declaration
     | func_declaration
     | func_definition
     ;

var_declaration : type_specifier declaration_list ';'
                ;

type_specifier : INT
               | FLOAT
               | VOID
               ;

declaration_list : declaration_list ',' ID
                 | ID
                 ;

%%

int main() {
    yyparse();
    return 0;
}

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}
```

In the example above, the grammar defines rules for a simple C-like language, including variable declarations and type specifiers.

### Part 4: Error Recovery

####

 Overview

Error recovery techniques allow the parser to continue processing after encountering an error, providing meaningful error messages and preventing abrupt termination. Bison provides mechanisms for handling syntax errors gracefully.

#### Example

```bison
%{
#include "symbol_table.h"
void yyerror(const char* s);
%}

%token <symbol_info> ID ADDOP

%type <symbol_info> expression term factor

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

statement : IF '(' expression ')' statement %prec LOWER_THAN_ELSE
          | IF '(' expression ')' statement ELSE statement
          ;

declaration_list : declaration_list ',' ID
                 | declaration_list error ',' ID
                 {
                     yyclearin;
                     yyerrok;
                 }
                 | ID
                 ;

%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s\n", s);
}
```

In the example above, the parser handles syntax errors in `declaration_list` by using the `error` token and clearing the lookahead token to continue parsing.

### Part 5: Intermediate Code Generation

#### Overview

Intermediate code generation translates the source code into an intermediate representation, which is then translated to machine code. In this project, we use 8086 assembly language as the intermediate representation.

#### Example

```asm
PUSH AX
PUSH BX
MOV AX, [BP+4]
MOV BX, [BP+6]
ADD AX, BX
POP BX
POP AX
```

In the example above, the intermediate code performs arithmetic operations using the stack and registers.

## Disclaimer

*The repository serves as an archive for the author's solutions to the course assignments. The solutions are **not guaranteed to be full-proof**. The author is not responsible for **any damage** caused by the use of the solutions. While it can serve as a **reference**, it is strongly discouraged to copy the solutions for **academic dishonesty**, and the author is not responsible for any consequences of such actions.*

## Contributing

Contributions are welcome! If you have any suggestions or improvements, feel free to create a pull request. Please ensure that your contributions adhere to the project guidelines.

## License

This repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

---
