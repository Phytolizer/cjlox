#pragma once

#include "environment.h"
#include "expr.h"
#include "stmt.h"

typedef struct Interpreter
{
    ExprVisitor expr;
    StmtVisitor stmt;
    bool init;
    Environment environment;
} Interpreter;

Interpreter new_interpreter(void);
void interpret(Interpreter *interpreter, Stmt **statements);
