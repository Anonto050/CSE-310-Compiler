<div id="top"></div>

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
- [Making it Easier: C Compiler](#making-it-easier-c-compiler)
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
    ./compiler program_name.c
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

## Making it Easier: C Compiler

This project involves the explanation of development of the C compiler that performs lexical, syntax, and semantic analysis, and generates intermediate code. The compiler covers a subset of the C language and uses Flex (lexer) and Bison (YACC parser) for analysis.

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
<p align="right"><a href="#top">back to top</a></p>

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

- **Additional Notes**

  - Whitespaces and comments are identified but not passed to the parser.
  - The lexer counts line numbers for newline characters.

**Tokens with \* are passed as `SymbolInfo` objects to the parser, containing the matched lexeme and returned token.**

### Lexical Errors:

The lexer detects and reports errors with corresponding line numbers. Types of errors include:

- Excessive decimal points, e.g., `1.2.345`
- Malformed numbers, e.g., `1E10.7`
- Invalid suffix on numeric constants or prefix on identifiers, e.g., `12abcd`
- Multi-character constants, e.g., `‘ab’`
- Unfinished characters, e.g., `‘a`, `‘\n`, or `‘\’`
- Empty character constants, e.g., `''`
- Unfinished strings, e.g., `"this is an unfinished string`
- Unfinished comments, e.g., `/* This is an unfinished comment`
- Unrecognized characters

<p align="right"><a href="#top">back to top</a></p>

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

## Syntax Analyser

### Subset Characteristics:

- Multiple functions with unique names, declared or defined before being called.
- No pre-processing directives like `#include` or `#define`.
- Variables can be declared inside functions or globally.
- Standard precedence and associativity rules apply, excluding consecutive logical or relational operators.
- No `break` or `switch-case` statements.
- `println(int n)` is used instead of `printf(“%d\n”, n)`.


## Semantic Analyser

### Semantic Checks:

<div>
<details>
<summary>Type Checking</summary>
<ol>
<li>Verifies assignment operand compatibility.</li>
<li>Ensures array indices are integers.</li>
<li>Requires integer operands for the modulus operator.</li>
<li>Validates function call arguments against definitions.</li>
<li>Prohibits void functions in expressions.</li>
</ol>
</details>
<details>
<summary>Type Conversion</summary>
<p>Errors on float-to-integer conversions in expressions. Logical and relational operations result in integers.</p>
</details>
<details>
<summary>Uniqueness Checking</summary>
<p>Ensures variables are declared and unique within their scope.</p>
</details>
<details>
<summary>Array Index</summary>
<p>Checks for correct usage of array indices.</p>
</details>
<details>
<summary>Function Parameters</summary>
<p>Validates the number and types of function call arguments, ensuring consistency with declarations.</p>
</details>

### Non-Terminal Data Types:

- **Basic Types:** `CONST_INT`, `CONST_FLOAT`, `CONST_CHAR`, `CONST_INT*`, `CONST_FLOAT*`, `CONST_CHAR*`
- **Special Types:** `UNDEC` (undeclared IDs), `ERROR` (expressions with errors), `FUNC_VOID` (void function return types), `VARIABLE` (other types)

<p align="right"><a href="#top">back to top</a></p>

### Part 4: Error Recovery

#### Overview

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

#### The Algorithm:

Intermediate code is generated on-the-fly by writing to `code.asm` as soon as a rule is matched. This involves using `PUSH` and `POP` instructions for stack operations.

##### Example:

Consider the grammar:

```bison
E -> E + T
E -> T
T -> T * F
T -> F
F -> id
F -> digit
```

For the expression `a + 2 * c`, the lexer generates tokens `id, +, digit, *, id`. The parser then matches rules and generates code as follows:

```asm
PUSH a
PUSH 2
PUSH c
POP BX
POP AX
MUL BX
PUSH AX
POP BX
POP AX
ADD AX, BX
PUSH AX
POP BX
POP AX
MOV d, BX
PUSH d
POP BX
```

##### Evaluating `for` Loop:

Special handling is required for evaluating `for` loops, saving the line number of the third expression and inserting corresponding code.

##### Evaluating Functions:

Refer to pages 303-305 (14.5.3 Using the stack for procedures) of "Assembly Language Programming and Organization of the IBM PC" by Ytha Yu, Charles Marut.

##### Declaring Variables:

Variables are declared by pushing dummy values to the stack or adding entries to the data segment.

##### Accessing Variables:

Variables are accessed by checking their scope and using appropriate addressing modes.

##### Example:

```asm
a               ;global variable
b[0]            ;global array
[BP + -2]       ;local variable
[BP + -4]       ;local array index 0
```

### Optimizing Assembly Code

#### Optimizations:

- **Remove Redundant Instructions:** Eliminate unnecessary `PUSH` and `POP` instructions.
- **Optimize `MOV` Instructions:** Remove redundant moves.
- **Simplify Jumps and Comparisons:** Remove unnecessary jump and compare instructions.




## Disclaimer

*The repository serves as an archive for the author's solutions to the course assignments. The solutions are **not guaranteed to be full-proof**. The author is not responsible for **any damage** caused by the use of the solutions. While it can serve as a **reference**, it is strongly discouraged to copy the solutions for **academic dishonesty**, and the author is not responsible for any consequences of such actions.*

## Contributing

Contributions are welcome! If you have any suggestions or improvements, feel free to create a pull request. Please ensure that your contributions adhere to the project guidelines.

## License

This repository is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.

---
