#pragma once

#include "expr.h"

#include <sds.h>

typedef struct AstPrinter
{
    ExprVisitor visitor;
} AstPrinter;

AstPrinter ast_printer_new(void);
sds ast_printer_print(AstPrinter printer, Expr *expr);
