#pragma once

#include "expr.h"
#include "stmt.h"

typedef struct Interpreter
{
    ExprVisitor expr;
    StmtVisitor stmt;
    bool init;
} Interpreter;

Interpreter new_interpreter(void);
void interpret(Interpreter *interpreter, Stmt **statements);
