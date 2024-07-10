## Intermediate Code Generation with Backpatching

### Overview

This project implements a parser that generates intermediate code for a hypothetical assembly language. The parser uses backpatching, a technique for handling forward jumps in control flow during code generation. The parser is built using Flex (for lexical analysis) and Bison (for parsing).

### Table of Contents

- [Overview](#overview)
- [Getting Started](#getting-started)
- [Backpatching Explained](#backpatching-explained)
  - [Key Concepts](#key-concepts)
  - [How Backpatching Works in This Code](#how-backpatching-works-in-this-code)
    - [Control Flow Structures](#control-flow-structures)
  - [Detailed Examples](#detailed-examples)
    - [If-Else Statement](#if-else-statement)
    - [While Loop](#while-loop)
    - [Example in This Code](#example-in-this-code)
      - [If Statement with Backpatching](#if-statement-with-backpatching)
      - [While Loop with Backpatching](#while-loop-with-backpatching)

### Getting Started

#### Prerequisites

- Flex
- Bison
- GCC

#### Building the Project

To build the project, run the following commands:

```sh
flex lexer.l
bison -d parser.y
g++ -o parser lex.yy.c parser.tab.c -lfl
```

#### Running the Parser

To run the parser on an input file:

```sh
./parser <input_file>
```

### Backpatching Explained

Backpatching is a technique used in intermediate code generation to handle forward jumps in control flow structures (such as if-else, while, and for loops). Since the target addresses for these jumps are not known in advance, placeholders are used initially, and the actual addresses are "patched" once they are determined.

#### Key Concepts

- **Next List**: A list of placeholders for the next statement to be executed after a control flow structure.
- **True List**: A list of placeholders where the condition evaluates to true.
- **False List**: A list of placeholders where the condition evaluates to false.
- **Labels**: Unique identifiers used to mark positions in the intermediate code.

#### How Backpatching Works in This Code

##### Control Flow Structures

###### If-Else Statement

1. Generate code for the condition.
2. If the condition is true, jump to the true block.
3. If the condition is false, jump to the false block (or next statement if there's no else).
4. Backpatch the false list to the position after the true block.

###### While Loop

1. Generate code for the condition.
2. If the condition is true, jump to the loop body.
3. If the condition is false, jump to the statement after the loop.
4. At the end of the loop body, jump back to the condition.
5. Backpatch the false list to the position after the loop.

### Detailed Examples

#### If-Else Statement

Consider the following code snippet:

```c
if (a < b) {
    // true block
} else {
    // false block
}
```

Intermediate code generation steps:

1. **Evaluate Condition**: Generate code to evaluate `a < b`.
2. **Jump Based on Condition**:
    - `JLE L1` (if `a < b` is true, jump to the true block)
    - `JMP L2` (if false, jump to `L2`)
3. `L1: // true block`
    - `JMP L3` (jump to the end after executing the true block)
4. `L2: // false block`
5. `L3: // next statement`

#### While Loop

Consider the following code snippet:

```c
while (a < b) {
    // loop body
}
```

Intermediate code generation steps:

1. **Evaluate Condition**: Generate code to evaluate `a < b`.
2. **Jump Based on Condition**:
    - `L1: CMP a, b`
    - `JGE L2` (if `a >= b`, jump to `L2` to exit the loop)
3. `L3: // loop body`
4. `JMP L1` (jump back to re-evaluate the condition)
5. `L2: // next statement`

### Example in This Code

#### If Statement with Backpatching

```bison
| IF LPAREN expression checkbool RPAREN M statement {
    logout << "statement : IF LPAREN expression RPAREN statement " << endl;
    $$ = new SymbolInfo("statement", "IF LPAREN expression RPAREN statement");
    backpatch($3->truelist, $6->label);
    $$->nextlist = merge($3->falselist, $7->nextlist);
}
```

- **Condition Evaluation**: `expression checkbool`
- **True List Backpatching**: `backpatch($3->truelist, $6->label)`
- **False List Management**: `$$->nextlist = merge($3->falselist, $7->nextlist)`

#### While Loop with Backpatching

```bison
| WHILE M LPAREN expression checkbool RPAREN M statement {
    logout << "statement : WHILE LPAREN expression RPAREN statement" << endl;
    $$ = new SymbolInfo("statement", "WHILE LPAREN expression RPAREN statement");
    backpatch($8->nextlist, $2->label);
    backpatch($4->truelist, $7->label);
    $$->nextlist = $4->falselist;
    tempicg << "\tJMP " << $2->label << endl;
    asmLineCount++;
}
```

- **Condition Evaluation**: `expression checkbool`
- **True List Backpatching**: `backpatch($4->truelist, $7->label)`
- **False List Management**: `$$->nextlist = $4->falselist`
- **Loop Back Jump**: `tempicg << "\tJMP " << $2->label << endl;`

### Backpatching Function

#### Definition

```cpp
void backpatch(vector<int> v, string s){
    for(int i = 0; i < v.size(); i++){
        umap[v[i]] = s;
        cout << v[i] << " - " << s << endl;
    }
}
```

#### How It Works

1. **Input Parameters**:
    - `v`: A vector of line numbers where jumps are to be patched.
    - `s`: The label to be used for patching.

2. **Operation**:
    - Iterate over the vector `v`.
    - For each entry in `v`, update the `umap` (an unordered map) with the label `s`.

3. **Output**:
    - Each placeholder in the intermediate code is replaced with the actual label once it's known.


By following the steps and understanding the backpatching mechanism, this project effectively generates correct intermediate code for complex control flow structures in the source code.
